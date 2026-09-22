#!/usr/bin/env python3
"""Extract dropdown-label-like strings (Title Case / spaced labels) from a
Unity IL2CPP global-metadata.dat, filtered to graph-node / car terms.

Dropdown options in AIComp node graphs are plain managed strings, so they live
in this file even though the SAVE only stores an index.
"""
import re
import sys
from pathlib import Path

TXT = re.compile(rb"[ -~]{3,64}")
KEYWORDS = ("Position", "Velocity", "Speed", "Rotation", "Part", "Wheel",
            "Waypoint", "Car", "Track", "Lap", "Throttle", "Steer", "Grip",
            "Drift", "Self", "Opponent", "Engine", "Body", "Chassis",
            "Forward", "Up", "Right", "Left", "Front", "Rear", "Boost",
            "Checkpoint", "Count", "Time", "Progress", "Collider", "Hit")


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    p = Path(sys.argv[1])
    blob = p.read_bytes()

    labels = {}
    for m in TXT.finditer(blob):
        s = m.group().decode("ascii", "ignore").strip()
        if len(s) < 3 or len(s) > 48:
            continue
        # label-ish: letters/spaces/digits only, starts with a capital or digit
        if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9 \-()._/']*", s):
            continue
        # drop engine/boilerplate noise
        if any(n in s for n in ("Unity", "Module", "Tests", "Internal",
                                "System", "Mono", "Assembly", "Version")):
            continue
        if "." in s or s[:1].isdigit():
            continue
        if not any(k in s for k in KEYWORDS):
            continue
        labels[s] = labels.get(s, 0) + 1

    out = Path(__file__).resolve().parent / "metadata_option_labels.txt"
    with out.open("w", encoding="utf-8") as fh:
        fh.write("# dropdown-label candidates in global-metadata.dat\n")
        fh.write(f"# source: {p}\n")
        fh.write("# keyword-filtered; count = occurrences in the string table\n\n")
        for s, c in sorted(labels.items(), key=lambda kv: (-kv[1], kv[0])):
            fh.write(f"{c:5d}  {s}\n")

    print(f"candidates: {len(labels)} -> {out.name}")
    for s, c in sorted(labels.items(), key=lambda kv: (-kv[1], kv[0]))[:80]:
        print(f"  {c:5d}  {s}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
