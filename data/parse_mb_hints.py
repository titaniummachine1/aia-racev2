#!/usr/bin/env python3
"""Turn hint_scan/mb_groups.jsonl into compiler-ready JSON:
  race_node_hints.json        - per node: id, title, class, description, ports[]
  race_node_hints_rich.json   - NodeConfigurationSO rich-text templates
  race_color_data.json / race_country_data.json / race_track_definitions.json"""
import json
from pathlib import Path

DATA = Path(__file__).resolve().parent
rows = [json.loads(l) for l in
        (DATA / "hint_scan" / "mb_groups.jsonl").read_text(encoding="utf-8").splitlines()
        if l.strip()]
by = {}
for r in rows:
    by.setdefault(r["script"], []).append(r)

# --- NodeTypeDataSO: strings = [id, title, class, desc, (portId, portDesc)...]
hints = {}
for r in by.get("NodeTypeDataSO", []):
    s = [x[2:] if x.startswith("U:") else x for x in r["strings"]]
    if len(s) < 4:
        continue
    ports = []
    rest = s[4:]
    for i in range(0, len(rest) - 1, 2):
        ports.append({"id": rest[i], "desc": rest[i + 1]})
    hints[s[2]] = {"node_id": s[0], "title": s[1], "class": s[2],
                   "description": s[3], "ports": ports,
                   "asset": f'{r["file"]}:{r["path_id"]}'}
(DATA / "race_node_hints.json").write_text(
    json.dumps(hints, indent=1, ensure_ascii=False), encoding="utf-8")

rich = []
for r in by.get("NodeConfigurationSO", []):
    rich.append({"asset": f'{r["file"]}:{r["path_id"]}',
                 "strings": [x for x in r["strings"]]})
(DATA / "race_node_hints_rich.json").write_text(
    json.dumps(rich, indent=1, ensure_ascii=False), encoding="utf-8")

colors = []
for r in by.get("ColorData", []):
    colors.extend([x for x in r["strings"]])
(DATA / "race_color_data.json").write_text(
    json.dumps(sorted(set(colors)), indent=1, ensure_ascii=False), encoding="utf-8")

countries = []
for r in by.get("Country", []):
    countries.extend([x for x in r["strings"]])
(DATA / "race_country_data.json").write_text(
    json.dumps(sorted(set(countries)), indent=1, ensure_ascii=False), encoding="utf-8")

tracks = []
for r in by.get("RaceTrackDefinition", []):
    tracks.append({"asset": f'{r["file"]}:{r["path_id"]}', "strings": r["strings"]})
(DATA / "race_track_definitions.json").write_text(
    json.dumps(tracks, indent=1, ensure_ascii=False), encoding="utf-8")

print("node hints:", len(hints))
print("rich templates:", len(rich))
print("color names:", len(set(colors)))
print("country names:", len(set(countries)))
print("track defs:", len(tracks))
for k in list(hints)[:3]:
    h = hints[k]
    print(" sample:", k, "|", h["title"], "|", len(h["ports"]), "ports |",
          h["description"][:60])
