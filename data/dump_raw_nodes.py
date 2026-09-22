#!/usr/bin/env python3
"""Dump raw JSON of representative Racing nodes so we can see every field
(ports, data, modifier) and any label/option text hiding in there."""
import json
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent
GRAPH = HERE / "graphs" / "v2.2 Blockmobile.txt"  # biggest graph

txt = GRAPH.read_text(encoding="utf-8", errors="replace")
txt = re.sub(r"(?<=\d),(?=\d)", ".", txt)
d = json.loads(txt)

WANT = ["RacingV2GetFloat", "RacingV2GetCar", "RacingV2Waypoint",
        "RacingV2GetWaypoint", "RacingV2GetBool", "RelativePosition",
        "HitInfo", "Stat"]
seen = {}
for n in d["serializableNodes"]:
    k = n.get("id")
    if k in WANT and k not in seen:
        seen[k] = n
    if len(seen) == len(WANT):
        break

for k, n in seen.items():
    print("=" * 70)
    print(k)
    print(json.dumps(n, indent=1))
