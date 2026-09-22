"""DESIGN SANDBOX - NOT A SIMULATOR OF THE GAME (user rule, 2026-09-22).
Invented [unverified] constants; its times are NOT predictions. Only in-game
runs decide. Real baseline: 1:49 on Training (Titanium v1, user-measured).

Lap-time sandbox: Titanium analytical controller + the handling model.

Track geometry: real waypoint dumps come from in-game RacingV2Waypoint probes;
until then a parameterised synthetic loop stands in. The 8 rays (CarRaycasts)
are synthesised from centerline clearance in the sim; the in-game graph gets
the real ones.

Both drivers are kept on purpose:
  simulate_lap        = TITANIUM controller (analytical, unrollable, no loops)
  simulate_lap_naive  = plain pure-pursuit (stands in for Autosteer/Autothrottle
                        behaviour the user calls garbage) = the comparison.
"""
from __future__ import annotations
import math
from handling import stat_params, step, TICK
import controller as C


def synthetic_track(corner_radius=18.0, straight=60.0, n=28):
    """Racetrack-ish loop: 2 straights + 2 constant corners. Returns waypoints."""
    pts = []
    half = straight / 2.0
    for i in range(n // 4):
        pts.append((-half + straight * i / (n // 4), corner_radius))
    for i in range(n // 4):
        a = math.pi * i / (n // 4)
        pts.append((half + corner_radius * math.sin(a),
                    corner_radius * math.cos(a)))
    for i in range(n // 4):
        pts.append((half - straight * i / (n // 4), -corner_radius))
    for i in range(n // 4):
        a = math.pi * i / (n // 4)
        pts.append((-half - corner_radius * math.sin(a),
                    -corner_radius * math.cos(a)))
    return pts


def _seg_dist(p, a, b):
    vx, vy = b[0] - a[0], b[1] - a[1]
    wx, wy = p[0] - a[0], p[1] - a[1]
    L2 = vx * vx + vy * vy
    t = 0.0 if L2 < 1e-9 else max(0.0, min(1.0, (wx * vx + wy * vy) / L2))
    return math.hypot(p[0] - (a[0] + t * vx), p[1] - (a[1] + t * vy))


def _rays(pos, wps, half_width=6.0):
    """v0 synthetic clearances: corridor width minus centerline offset."""
    best = min(_seg_dist(pos, wps[i], wps[(i + 1) % len(wps)]) for i in range(len(wps)))
    clear = max(0.2, half_width - best)
    return [clear] * 8


def simulate_lap(speed_pts, turn_pts, wps=None, max_ticks=30000):
    """TITANIUM controller lap; returns lap time seconds (or inf)."""
    wps = wps or synthetic_track()
    p = stat_params(speed_pts, turn_pts)
    x, y, yaw, v = wps[0][0], wps[0][1] - 2.0, 90.0, 0.0
    idx, t, passed = 1, 0.0, 0
    while passed < len(wps) + 2 and t < max_ticks * TICK:
        if math.hypot(wps[idx % len(wps)][0] - x, wps[idx % len(wps)][1] - y) < 3.0:
            idx += 1
            passed += 1
            continue
        steer, throttle, brake = C.control(
            (x, y), yaw, v, wps, idx,
            rays=_rays((x, y), wps), vmax=p["vmax"])
        x, y, yaw, v = step(x, y, yaw, v, steer, throttle, brake, p)
        t += TICK
    return t if passed >= len(wps) + 2 else float("inf")


def simulate_lap_naive(speed_pts, turn_pts, wps=None, max_ticks=30000):
    """Plain pure-pursuit (the Autosteer/Autothrottle stand-in) for comparison."""
    wps = wps or synthetic_track()
    p = stat_params(speed_pts, turn_pts)
    x, y, yaw, v = wps[0][0], wps[0][1] - 2.0, 90.0, 0.0
    idx, t, passed = 1, 0.0, 0
    while passed < len(wps) + 2 and t < max_ticks * TICK:
        tx, ty = wps[idx % len(wps)]
        dx, dy = tx - x, ty - y
        if math.hypot(dx, dy) < 3.0:
            idx += 1
            passed += 1
            continue
        target = math.degrees(math.atan2(dy, dx)) % 360.0
        err = ((target - yaw + 540.0) % 360.0) - 180.0
        steer = max(-1.0, min(1.0, err / 45.0))
        throttle = 1.0 if abs(err) < 30.0 else 0.35
        brake = 1.0 if abs(err) > 80.0 and v > 8.0 else 0.0
        x, y, yaw, v = step(x, y, yaw, v, steer, throttle, brake, p)
        t += TICK
    return t if passed >= len(wps) + 2 else float("inf")


if __name__ == "__main__":
    for alloc in ((10, 10), (4, 10), (10, 4), (2, 10)):
        a = simulate_lap(alloc[0], alloc[1])
        b = simulate_lap_naive(alloc[0], alloc[1])
        print(f"speed={alloc[0]:2d} turn={alloc[1]:2d} -> titanium={a:6.2f}s  naive={b:6.2f}s")