#!/usr/bin/env python3
"""Grab dropdown-label candidates + tooltip text from v0.22f metadata.

Writes two files next to this script:
  metadata_dropdowns2.txt  - label-list candidates for missing dropdowns
  metadata_tooltips.txt    - long Inspector tooltip strings (constants surface)
"""
import re
from pathlib import Path

META = Path(r"C:\gitProjects\aia-racev2\versions\v0.22f\Aialanders_Data"
            r"\il2cpp_data\Metadata\global-metadata.dat")
HERE = Path(__file__).resolve().parent

blob = META.read_bytes()
strs = [x.decode("ascii", "replace") for x in re.findall(rb"[ -~]{3,120}", blob)]

# --- 1. dropdown label candidates: Title Case short strings -----------------
OK = set(" -+(),.'/")
cand = {}
for s in strs:
    if not s or not s[0].isupper():
        continue
    if len(s) > 44 or any(c not in OK and not c.isalnum() for c in s):
        continue
    if any(e in s for e in ("Unity", "Obi", "Asset", "System", "Module",
                            "Library", "Render", "Physics", "Engine", "Input",
                            "Text", "Animation", "Shader", "Light", "Camera")):
        continue
    want = ("speed", "waypoint", "lap", "rank", "competitor", "grounded",
            "disabled", "simulation", "delta", "time", "nearest", "furthest",
            "lowest", "highest", "health", "damage", "index", "self", "car",
            "part", "wheel", "front", "rear", "left", "right", "spoiler",
            "bumper", "hood", "roof", "door", "chassis", "body", "crucial",
            "weakest", "average", "target", "goal", "start", "next", "prev",
            "center", "point", "mode", "stat", "turn", "brake", "throttle",
            "grip", "drift", "yaw", "angle", "velocity", "position", "hit",
            "cast", "sphere", "lap", "pit")
    if any(w in s.lower() for w in want):
        cand[s] = cand.get(s, 0) + 1

# --- 2. tooltips: long English sentences ------------------------------------
tips = sorted({s for s in strs if len(s) > 25 and " " in s
               and s[0].isupper()
               and any(w in s.lower() for w in
                       ("steer", "throttle", "brake", "torque", "speed",
                        "yaw", "drift", "wheel", "stat", "health", "grip",
                        "slip", "angle", "accel", "engine", "gear", "kart",
                        "roll", "handbrake", "downforce"))
               and not any(e in s for e in ("UnityUpgradeable", "Obi",
                                            "Sentis", "UnityEngine",
                                            "SystemState", "ISystem"))})

out1 = HERE / "metadata_dropdowns2.txt"
out1.write_text(
    "# dropdown-label candidates (v0.22f metadata, title-case filtered)\n"
    "# sorted alphabetically; count = string-table occurrences\n\n"
    + "\n".join(f"{n:3d}  {t}" for t, n in sorted(cand.items())),
    encoding="utf-8")
out2 = HERE / "metadata_tooltips.txt"
out2.write_text(
    "# Inspector tooltip sentences (v0.22f metadata) - the constants surface\n\n"
    + "\n".join(tips), encoding="utf-8")
print("dropdown candidates:", len(cand), "->", out1.name)
print("tooltips:", len(tips), "->", out2.name)
