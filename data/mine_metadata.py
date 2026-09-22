#!/usr/bin/env python3
"""Scan a Unity IL2CPP global-metadata.dat for Racing API / node identifiers.

Unity keeps all managed strings in this file as plain UTF-8, so the RacingV2*
sensor class + method names (and often the dropdown option strings) are
recoverable without running the game.

Usage: python mine_metadata.py <path-to-global-metadata.dat>
"""
import re
import sys
from collections import Counter
from pathlib import Path

PAT = re.compile(rb"[A-Za-z_][A-Za-z0-9_]{3,60}")
NEEDLES = ("RacingV2", "Racing", "Waypoint", "GetCar", "RelativePosition",
           "SphereCast", "Throttle", "Steering", "Wheel", "Axle")


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    p = Path(sys.argv[1])
    blob = p.read_bytes()
    print(f"{p.name}: {len(blob)/1e6:.1f} MB")

    hits = Counter()
    all_ids = Counter()
    for m in PAT.finditer(blob):
        s = m.group().decode("ascii", "ignore")
        all_ids[s] += 1
        if any(n in s for n in NEEDLES):
            hits[s] += 1

    out = Path(__file__).resolve().parent / "metadata_race_strings.txt"
    with out.open("w", encoding="utf-8") as fh:
        fh.write("# Racing-ish identifiers found in global-metadata.dat\n")
        fh.write(f"# source: {p}\n\n")
        for s, c in sorted(hits.items(), key=lambda kv: (-kv[1], kv[0])):
            fh.write(f"{c:4d}  {s}\n")

    print(f"racing-ish identifiers: {len(hits)} -> {out.name}")
    print(f"total identifiers: {len(all_ids)}")
    for s, c in sorted(hits.items(), key=lambda kv: (-kv[1], kv[0]))[:60]:
        print(f"  {c:4d}  {s}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
