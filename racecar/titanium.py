"""TITANIUM - our fastest-car AI (the user named it, 2026-09-22).

SANDBOX ONLY - NOT A SIMULATOR (user rule 2026-09-22). Numbers here are design
signals, not times. REAL baseline = 1:49 on Training, measured in-game.

This is the build sheet: pick the stat allocation per track shape and report
lap times. TIMING PROVENANCE: times from here are the PYTHON LAP SIMULATOR on
synthetic tracks - the game does NOT run for these (nothing should pop up).
In-game times arrive with the calibration + waypoint-probe session; the in-game
bot graph we author later is also called Titanium.
"""
from __future__ import annotations
from laptime import simulate_lap, synthetic_track
from optimize import search

NAME = "TITANIUM"


def build(corner_radius: float = 18.0) -> dict:
    """Search the budget on a track shape -> the Titanium allocation.

    HEALTH IS NEVER ALLOCATED (user rule 2026-09-22: useless stat - no wall
    contact = no damage). Speed first, steering second.
    """
    track = synthetic_track(corner_radius=corner_radius)
    rows = search(track)
    lt, speed, turn = rows[0]
    return {
        "name": NAME,
        "speed": speed,
        "turn": turn,
        "health": 0,
        "sim_lap": lt,
        "track_radius": corner_radius,
    }


if __name__ == "__main__":
    for r in (14.0, 18.0, 26.0):
        b = build(r)
        print(
            f"{NAME} build for corner r={b['track_radius']:4.1f}m -> "
            f"speed={b['speed']} turn={b['turn']} health=0 (useless)  "
            f"sim lap={b['sim_lap']:.2f}s"
        )
    print()
    print("NOTE: sim times = Python lap simulator (synthetic track). No game ran.")
    print("Real in-game times after: calibration session + real waypoint dumps.")