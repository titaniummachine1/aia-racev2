"""Lap-time simulator: pure-pursuit around a waypoint loop + the handling model.

Track geometry: real waypoint dumps come from in-game RacingV2Waypoint probes;
until then a parameterised synthetic loop stands in (corner radius sweep is
what the allocation search actually needs).
"""
from __future__ import annotations
import math
from handling import stat_params, step, TICK


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


def simulate_lap(speed_pts, turn_pts, wps=None, max_ticks=30000):
    """Pure pursuit to the next waypoint; returns lap time seconds (or inf)."""
    wps = wps or synthetic_track()
    p = stat_params(speed_pts, turn_pts)
    x, y, yaw, v = wps[0][0], wps[0][1] - 2.0, 90.0, 0.0
    idx, t, passed = 1, 0.0, 0
    while passed < len(wps) + 2 and t < max_ticks * TICK:
        tx, ty = wps[idx % len(wps)]
        dx, dy = tx - x, ty - y
        dist = math.hypot(dx, dy)
        if dist < 3.0:
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
    for alloc in ((2, 10, 3), (10, 2, 0), (7, 7, 0), (5, 8, 2)):
        lt = simulate_lap(alloc[0], alloc[1])
        print(f"speed={alloc[0]:2d} turn={alloc[1]:2d} health={alloc[2]:2d} -> {lt:6.2f}s")