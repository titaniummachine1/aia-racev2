"""Titanium controller - analytical curvature + severity + braking + virtual apex.

Design (user 2026-09-22 + their AI consult): NO simulation/optimisation loop,
NO search tree - pure vector math over the next HORIZON waypoints. That shape
is deliberate: the graph ISA has NO LOOPS (single-pass solver, aha A13), so the
controller must be unrollable to O(1) - HORIZON=8 fixed ops chains.

Pipeline: waypoint angles/distances -> curvature (circumradius of triples)
-> corner severity C = sum w_i|dtheta_i| -> target corner speed g(C) + the
braking envelope -> virtual racing-line point (outside->apex->inside) with
speed-scaled lookahead -> 8-ray wall bias -> steer/throttle/brake.

USER RULES (2026-09-22):
- health stat = USELESS (no wall contact = no damage) - never allocate it.
- speed > steering: you can always brake speed off to buy grip, you cannot
  invent top speed afterwards. Slow down = better steering; that trade is the
  controller's job, not the stat sheet's.
"""
from __future__ import annotations
import math

HORIZON = 8            # waypoints considered (the unroll count for the graph)
EPS = 1e-6
A_LAT = 18.0           # lateral accel budget m/s^2        [calibrate in-game]
A_BRAKE = 9.0          # braking m/s^2                    [calibrate in-game]
L0, KV = 4.0, 0.35     # lookahead = L0 + KV * v          [calibrate in-game]
K_STEER = 1.6          # steer gain over heading error    [calibrate in-game]
G_SEVERITY = 0.35      # g(C) = 1 / (1 + G_SEVERITY * C)  [calibrate in-game]
K_WALL = 0.8           # wall-bias steer gain             [calibrate in-game]


def _sub(a, b):
    return (a[0] - b[0], a[1] - b[1])


def _norm(a):
    return math.hypot(a[0], a[1])


def curvature(p0, p1, p2):
    """kappa = 1/R, R = circumradius = |AB||BC||CA| / (4 * area)."""
    a = _norm(_sub(p1, p0))
    b = _norm(_sub(p2, p1))
    c = _norm(_sub(p2, p0))
    cross = abs((p1[0] - p0[0]) * (p2[1] - p0[1]) - (p1[1] - p0[1]) * (p2[0] - p0[0]))
    if cross < EPS or a * b * c < EPS:
        return 0.0
    return 2.0 * cross / (a * b * c)          # == 4A * area / (abc) with area=cross/2


def corner_severity(wps, idx):
    """C = sum w_i * |dtheta_i| over the horizon, w_i decays with distance."""
    total, c = 0.0, 0.0
    prev = wps[idx % len(wps)]
    for k in range(1, HORIZON):
        cur = wps[(idx + k) % len(wps)]
        nxt = wps[(idx + k + 1) % len(wps)]
        v1, v2 = _sub(cur, prev), _sub(nxt, cur)
        d1, d2 = _norm(v1), _norm(v2)
        if d1 > EPS and d2 > EPS:
            cosang = max(-1.0, min(1.0, (v1[0] * v2[0] + v1[1] * v2[1]) / (d1 * d2)))
            dtheta = abs(math.acos(cosang))
            total += d1
            w = 1.0 / (1.0 + total / 25.0)     # distance decay
            c += w * dtheta
        prev = cur
    return c


def control(pos, yaw_deg, speed, wps, idx, rays=None, vmax=30.0):
    """-> (steer, throttle, brake). rays: 8 clearances (CarRaycasts order)."""
    n = len(wps)
    # 1. per-corner speed cap from curvature, backed off by the braking envelope
    v_lim = vmax
    ds = 0.0
    probe = pos
    for k in range(HORIZON - 2):
        i0, i1, i2 = (idx + k) % n, (idx + k + 1) % n, (idx + k + 2) % n
        kap = curvature(wps[i0], wps[i1], wps[i2])
        if kap > 1e-4:
            v_cap = math.sqrt(A_LAT / kap)
            v_here = math.sqrt(max(0.0, v_cap * v_cap + 2.0 * A_BRAKE * ds))
            v_lim = min(v_lim, v_here)
        ds += _norm(_sub(wps[i1], probe))
        probe = wps[i1]
    # 2. severity shaping (the g(C) term)
    sev = corner_severity(wps, idx)
    v_tgt = min(v_lim, vmax / (1.0 + G_SEVERITY * sev))
    # 3. virtual apex target: blend toward the inside of the coming corner
    look = L0 + KV * speed
    acc, ti = 0.0, 1
    while ti < HORIZON - 1 and acc < look:
        acc += _norm(_sub(wps[(idx + ti) % n], wps[(idx + ti - 1) % n]))
        ti += 1
    tx, ty = wps[(idx + ti) % n]
    nxt = wps[(idx + ti + 1) % n]
    seg = _sub(nxt, (tx, ty))
    sl = _norm(seg)
    if sl > EPS and sev > 1e-3:                 # bias to the inside line
        inward = (-seg[1] / sl, seg[0] / sl)
        turn_sign = 1.0 if (ty - pos[1]) * seg[0] - (tx - pos[0]) * seg[1] < 0 else -1.0
        bias = min(3.0, 1.5 * sev)
        tx += inward[0] * bias * turn_sign
        ty += inward[1] * bias * turn_sign
    # 4. wall bias from the 8 rays: P_wall = sum w_i/(d_i+eps)
    steer_bias = 0.0
    if rays:
        weights = (0.2, 0.35, 0.5, 0.7, 0.7, 0.5, 0.35, 0.2)
        sides = (-1, -1, -1, 0, 0, 1, 1, 1)
        for d, w, s in zip(rays, weights, sides):
            steer_bias += -s * w / (d + 0.5)
        steer_bias *= K_WALL
    # 5. steering + speed management (brake buys grip, throttle is all-or-near)
    err = ((math.degrees(math.atan2(ty - pos[1], tx - pos[0])) - yaw_deg + 540.0) % 360.0) - 180.0
    steer = max(-1.0, min(1.0, K_STEER * err / 45.0 + steer_bias))
    throttle = 1.0 if speed < v_tgt - 0.5 else 0.0
    brake = 1.0 if speed > v_tgt + 1.0 else 0.0
    return steer, throttle, brake