#!/usr/bin/env python3
"""Dump string-heap neighborhoods around known dropdown labels.

Unity stores each dropdown option as its own heap string; sibling options of
the same gate usually sit adjacent in heap order. Find anchors we know from
the graphs/pylib and print surrounding strings -> reconstruct full arrays.

Writes metadata_label_neighborhoods.txt
"""
import re
from pathlib import Path

META = Path(r"C:\gitProjects\aia-racev2\versions\v0.22f\Aialanders_Data"
            r"\il2cpp_data\Metadata\global-metadata.dat")
HERE = Path(__file__).resolve().parent

blob = META.read_bytes()

# Collect (offset, text) for every short ASCII string.
spans = [(m.start(), m.group().decode("ascii", "replace"))
         for m in re.finditer(rb"[ -~]{2,48}", blob)]
by_off = spans

ANCHORS = ["By index", "By rank", "Nearest car", "Next waypoint",
           "Is Grounded", "Center", "Nearest point", "Signed Speed",
           "Waypoint count", "Current race rank", "Simulation started",
           "Furthest active car", "Start waypoint", "Nearest part",
           "Weakest part", "Nearest crucial part", "Nearest disabled car",
           "Last damaged car", "Lowest health car"]

lines = []
for anchor in ANCHORS:
    hits = [i for i, (off, t) in enumerate(by_off) if t == anchor]
    for hi in hits:
        lines.append(f"\n=== anchor {anchor!r} @ 0x{by_off[hi][0]:x} ===")
        lo = max(0, hi - 30)
        hi2 = min(len(by_off), hi + 31)
        for j in range(lo, hi2):
            mark = ">>" if j == hi else "  "
            off, t = by_off[j]
            if 2 <= len(t) <= 48 and re.fullmatch(r"[ -~]+", t):
                lines.append(f"{mark} 0x{off:08x} {t}")

out = HERE / "metadata_label_neighborhoods.txt"
out.write_text("\n".join(lines), encoding="utf-8")
print("wrote", out.name, len(lines), "lines")
