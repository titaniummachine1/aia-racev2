#!/usr/bin/env python3
"""Scan all game asset files for node-hint text (TooltipData blobs) and extract
length-prefixed strings in serialization order -> hint_scan/hints_raw.txt."""
import re
import sys
from pathlib import Path

import UnityPy

GAME = Path(r"C:\gitProjects\aia-racev2\versions\v0.22\Aialanders_Data")
OUT = Path(__file__).resolve().parent / "hint_scan"
OUT.mkdir(exist_ok=True)
MARKERS = (b"Input(s)", b"Output(s)", b"Information about", b"NODE",
           b"The car to be processed", b"read more", b"Read More")
STR_RE = re.compile(rb"(?:[\x02-\x90][\x00\x00\x00])((?:[\x20-\x7e]\x00?){4,})")

found = 0
lines = []
files = sorted(GAME.glob("*.assets")) + sorted(GAME.glob("*.resource"))
files += [GAME / "resources.assets", GAME / "sharedassets0.assets"]
files = [f for f in files if f.exists()]
for f in files:
    try:
        env = UnityPy.load(str(f))
    except Exception as e:  # noqa: BLE001
        print("skip", f.name, e)
        continue
    for obj in env.objects:
        if obj.type.name not in ("TextAsset", "MonoBehaviour", "GameObject",
                                 "RectTransform", "Transform", "MonoScript",
                                 "ScriptableObject"):
            continue
        try:
            raw = obj.get_raw_data()
        except Exception:  # noqa: BLE001
            try:
                raw = obj.read().m_Script or b""
            except Exception:  # noqa: BLE001
                continue
        if not raw:
            continue
        hit = [m.decode() for m in MARKERS if m in raw]
        if not hit:
            continue
        found += 1
        lines.append(f"=== {f.name} {obj.type.name} path_id={obj.path_id} "
                     f"len={len(raw)} markers={hit}")
        # length-prefixed ASCII (utf8 or utf16le) string extraction, in order
        i = 0
        n = len(raw)
        while i < n - 6:
            ln = int.from_bytes(raw[i:i + 4], "little")
            if 4 <= ln <= 500 and i + 4 + ln <= n:
                chunk = raw[i + 4:i + 4 + ln]
                if all(32 <= b < 127 or b in (9, 10, 13) for b in chunk):
                    s = chunk.decode("ascii", "replace")
                    lines.append("  " + repr(s))
                    i += 4 + ln
                    continue
                if ln % 2 == 0:
                    try:
                        s = chunk.decode("utf-16-le")
                        if s and all(c.isprintable() or c in "\n\r\t" for c in s):
                            lines.append("  U" + repr(s))
                            i += 4 + ln
                            continue
                    except Exception:  # noqa: BLE001
                        pass
            i += 1
        (OUT / f"blob_{f.stem}_{obj.path_id}.bin").write_bytes(raw)

(OUT / "hints_raw.txt").write_text("\n".join(lines), encoding="utf-8")
print(f"objects with markers: {found}")
print(f"strings file: {OUT / 'hints_raw.txt'} ({len(lines)} lines)")
