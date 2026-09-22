#!/usr/bin/env python3
"""Consolidate every captured ABI fact into race_abi.json + generate the
compiler dropmap module straight from the XTRAS capture rows."""
import json
import re
from pathlib import Path

DATA = Path(__file__).resolve().parent
GAME = Path(r"C:\gitProjects\aia-racev2\versions\v0.22")
PKG = Path(r"c:\gitProjects\aia_graphc\graphc\api\AIA_Comp_Libry\racing")

# 1. dropdown maps from live XTRAS rows
dropdowns = {}
for line in (GAME / "probe-startup.jsonl").read_text(encoding="utf-8",
                                                     errors="replace").splitlines():
    if not line.startswith("XTRAS|"):
        continue
    parts = line.split("|", 5)
    if len(parts) < 6:
        continue
    kind, gate, key, idx, val = parts[1], parts[2], parts[3], parts[4], parts[5]
    if kind == "item" and key in ("items", "_dropdownLabels"):
        dropdowns.setdefault(gate, {})[int(idx)] = val
maps = {g: [d[k] for k in sorted(d)] for g, d in dropdowns.items()}

# 2. enum names + struct layouts from the stage3a capture
rows = [json.loads(l) for l in
        (GAME / "probe-startup_stage3a.jsonl").read_text(
            encoding="utf-8", errors="replace").splitlines()
        if l.startswith("{")]
def fields_of(cls):
    return [{"name": r.get("name"), "type": r.get("type"),
             "offset": r.get("offset"), "flags": r.get("flags")}
            for r in rows if r.get("kind") == "metadata_field"
            and r.get("class") == cls]
graph_opcode = [f["name"] for f in fields_of("GraphOpCode")
                if f["name"] != "value__"]
part_type = [f["name"] for f in fields_of("PartType") if f["name"] != "value__"]
gvk = [f["name"] for f in fields_of("GraphValueKind") if f["name"] != "value__"]

# 3. hint / track / color / country assets
hints = json.loads((DATA / "race_node_hints.json").read_text(encoding="utf-8"))
rich = json.loads((DATA / "race_node_hints_rich.json").read_text(encoding="utf-8"))
tracks = json.loads((DATA / "race_track_definitions.json").read_text(encoding="utf-8"))
colors = json.loads((DATA / "race_color_data.json").read_text(encoding="utf-8"))
countries = json.loads((DATA / "race_country_data.json").read_text(encoding="utf-8"))

abi = {
 "source": "runtime capture 2026-09-22 (Aialanders-racev2-meta.exe extras + asset scan)",
 "engine": "Unity 6000.4.3f1",
 "dropdowns": maps,
 "graph_vm": {
   "instruction": {"Op": "GraphOpCode", "In0": "Int32", "In1": "Int32",
                    "In2": "Int32", "Out0": "Int32", "Out1": "Int32",
                    "Out2": "Int32", "Imm": "Int32"},
   "opcodes": graph_opcode,
   "value_kinds": gvk,
   "interpreter": ["Execute", "ExecuteInstructions", "ExecuteFunctionCall",
                    "EvaluateOperation", "EvaluateRelativePosition"],
   "compiler_methods": ["Compile", "CarryOverRegisters"]},
 "part_types": part_type,
 "structs": {c: fields_of(c) for c in
   ("VehicleData", "WheelFrictionData", "WheelSettingsSO", "DamageableVehiclePart",
    "HybridAStarTypes.PlanOptions", "VehiclePlannerService", "VehicleMovementJob")},
 "node_hints": hints,
 "hint_templates": rich,
 "tracks": tracks,
 "colors": colors,
 "countries": countries,
}
(DATA / "race_abi.json").write_text(json.dumps(abi, indent=1, ensure_ascii=False),
                                     encoding="utf-8")

# 4. compiler package: dropmap.py (generated) + hints json copy
dm = ['"""RacingV2 dropdown ground truth (index -> label), captured at runtime',
      '2026-09-22 from live gate instances. Saves store these INDICES as modifier."""',
      "", "# gate class -> label list in index order",
      "DROPDOWNS = " + json.dumps(maps, indent=1, ensure_ascii=False),
      "", "# DamageableVehiclePart.PartType declaration order",
      "PART_TYPES = " + json.dumps(part_type),
      "", "# GraphOpCode declaration order (compiled-graph VM ISA)",
      "GRAPH_OPCODES = " + json.dumps(graph_opcode),
      "", "# GraphValueKind declaration order",
      "VALUE_KINDS = " + json.dumps(gvk),
      ""]
PKG.mkdir(parents=True, exist_ok=True)
(PKG / "dropmap.py").write_text("\n".join(dm), encoding="utf-8")
(PKG / "node_hints.json").write_text(json.dumps(hints, indent=1,
                                                ensure_ascii=False),
                                     encoding="utf-8")
print("dropdowns:", {g: len(v) for g, v in maps.items()})
print("opcodes:", len(graph_opcode), "parts:", len(part_type),
      "kinds:", len(gvk), "hints:", len(hints))
print("race_abi.json:", (DATA / "race_abi.json").stat().st_size, "bytes")
print("dropmap.py:", (PKG / "dropmap.py").stat().st_size, "bytes")
