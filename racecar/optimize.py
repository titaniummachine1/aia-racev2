"""Fastest-car search: brute-force the budget grid.

USER RULES (2026-09-22): health = USELESS (no wall contact = no damage) -
NEVER allocate it. Speed > steering (brake speed off to buy grip; you cannot
invent top speed). Budget: speed/turn each 0..10, speed+turn <= 20.
"""
from __future__ import annotations
from laptime import simulate_lap, synthetic_track


def search(track=None):
    rows = []
    for speed in range(0, 11):
        for turn in range(0, 11):
            lt = simulate_lap(speed, turn, track)
            rows.append((lt, speed, turn))
    rows.sort()
    return rows


if __name__ == "__main__":
    for radius in (14.0, 18.0, 26.0):
        track = synthetic_track(corner_radius=radius)
        rows = search(track)
        print(f"\n== corner radius {radius}m (titanium controller) ==")
        for lt, s, t in rows[:6]:
            print(f"  speed={s:2d} turn={t:2d} health=0  lap={lt:6.2f}s")
        if rows:
            print(f"  spread best->worst: {rows[0][0]:.2f}s vs {rows[-1][0]:.2f}s")