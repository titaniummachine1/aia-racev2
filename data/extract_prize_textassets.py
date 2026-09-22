#!/usr/bin/env python3
"""Extract prize TextAssets from sharedassets0/resources:
  * driver_config.json       - built-in AI driver tuning (real constants!)
  * embedded_graph_1..N.txt  - dev-authored reference racing graphs
"""
import json
import re
from pathlib import Path

import UnityPy

GAME = Path(r"C:\gitProjects\aia-racev2\versions\v0.22\Aialanders_Data")
OUT = Path(__file__).resolve().parent

graphs = []
for fname in ("resources.assets", "sharedassets0.assets"):
    env = UnityPy.load(str(GAME / fname))
    for obj in env.objects:
        if obj.type.name != "TextAsset":
            continue
        try:
            txt = obj.read().m_Script or ""
        except Exception:  # noqa: BLE001
            continue
        if not txt:
            continue
        if '"throttle"' in txt and '"steerSensitivity"' in txt:
            p = OUT / "driver_config.json"
            p.write_text(txt, encoding="utf-8")
            print("driver_config ->", p.name)
            print(txt.strip()[:400])
        elif '"serializableNodes"' in txt and len(txt) > 20000:
            graphs.append((fname, txt))

for i, (fname, txt) in enumerate(
        sorted(graphs, key=lambda x: -len(x[1])), 1):
    p = OUT / f"embedded_graph_{i}.txt"
    p.write_text(txt, encoding="utf-8")
    g = json.loads(re.sub(r"(?<=\d),(?=\d)", ".", txt))
    nodes = g.get("serializableNodes", [])
    kinds = {}
    for n in nodes:
        k = n.get("id", "?")
        kinds[k] = kinds.get(k, 0) + 1
    mods = [f'{n.get("id")}[{n.get("modifier")}]'
            for n in nodes
            if n.get("id", "").startswith(("Stat", "RacingV2", "Spherecast",
                                           "GetCarPart", "RelativePosition"))]
    print(f"graph_{i} -> {p.name} ({len(txt)} chars, {len(nodes)} nodes)")
    print("   kinds:", dict(sorted(kinds.items(), key=lambda kv: -kv[1])))
    print("   mods :", mods)
