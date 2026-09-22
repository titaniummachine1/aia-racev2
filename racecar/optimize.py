"""Fastest-car search: brute-force the 20-point stat budget grid.

Budget: speed/turn/health each 0..10, sum <= 20 (health reserved for race-long
survival; hot-lap mode passes health through as a tie-break preference).
Output = the allocation table + sensitivity so we see the joint trade the user
described (max speed vs turning interplay) at a glance.
"""
from __future__ import annotations
from laptime import simulate_lap, synthetic_track


def search(track=None):
    rows = []
    for speed in range(0, 11):
        for turn in range(0, 11):
            health = max(0, 20 - speed - turn)
            if speed + turn > 20 or health > 10:
                continue
            lt = simulate_lap(speed, turn, track)
            rows.append((lt, speed, turn, min(health, 10)))
    rows.sort()
    return rows


if __name__ == "__main__":
    for radius in (14.0, 18.0, 26.0):
        track = synthetic_track(corner_radius=radius)
        rows = search(track)
        print(f"\n== corner radius {radius}m ==")
        for lt, s, t, h in rows[:6]:
            print(f"  speed={s:2d} turn={t:2d} health={h:2d}  lap={lt:6.2f}s")
        if rows:
            best = rows[0]
            worst = rows[-1]
            print(f"  spread best->worst: {best[0]:.2f}s vs {worst[0]:.2f}s")