#!/usr/bin/env python3
"""Parse metadata_prefab_values.txt TextAsset blocks + summarize physics hits."""
import re
from pathlib import Path

t = (Path(__file__).resolve().parent / "metadata_prefab_values.txt").read_text(
    encoding="utf-8")
blocks = re.findall(
    r"--- TextAsset \((.+?), (\d+) B\) ---\n(.*?)(?=\n--- |\n=== |\Z)",
    t, re.S)
print("TextAssets:", len(blocks))
for f, sz, body in blocks:
    head = body[:120].replace("\n", " ").encode(
        "ascii", "backslashreplace").decode("ascii")
    print(f"  {f} {sz}B | {head}")

print("\nengine values:")
for ln in t.splitlines():
    if re.match(r"\s+m_(Mass|DynamicFriction|StaticFriction|AngularDamping)"
                r"|^\s+(spring|damper|targetVelocity) =", ln):
        print(" ", ln.strip())
