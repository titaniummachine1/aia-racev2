#!/usr/bin/env python3
"""Group every MonoBehaviour by its MonoScript and extract ordered strings.
Dumps hint data (NodeConfigurationSO / TooltipData / rich-text ToolTip blobs)
for ALL nodes -> hint_scan/mb_groups.jsonl"""
import json
from pathlib import Path

import UnityPy

GAME = Path(r"C:\gitProjects\aia-racev2\versions\v0.22\Aialanders_Data")
OUT = Path(__file__).resolve().parent / "hint_scan"
OUT.mkdir(exist_ok=True)
files = sorted(set(GAME.glob("*.assets")) | {GAME / "resources.assets"})
files = [f for f in files if f.exists()]

scripts = {}
for f in files:
    try:
        env = UnityPy.load(str(f))
    except Exception:  # noqa: BLE001
        continue
    for obj in env.objects:
        if obj.type.name == "MonoScript":
            try:
                scripts[obj.path_id] = obj.read().m_Name
            except Exception:  # noqa: BLE001
                pass
print("monoscripts:", len(scripts))

def strings_of(raw):
    out = []
    i = 0
    n = len(raw)
    while i < n - 6:
        ln = int.from_bytes(raw[i:i + 4], "little")
        if 4 <= ln <= 2000 and i + 4 + ln <= n:
            chunk = raw[i + 4:i + 4 + ln]
            if all(32 <= b < 127 or b in (9, 10, 13) for b in chunk):
                out.append(chunk.decode("ascii"))
                i += 4 + ln
                continue
            if ln % 2 == 0:
                try:
                    s = chunk.decode("utf-16-le")
                    if s and all(c.isprintable() or c in "\n\r\t" for c in s):
                        out.append("U:" + s)
                        i += 4 + ln
                        continue
                except Exception:  # noqa: BLE001
                    pass
        i += 1
    return out

def script_pid_of(raw):
    if len(raw) < 32:
        return None
    return int.from_bytes(raw[20:28], "little", signed=True)

rows = []
for f in files:
    try:
        env = UnityPy.load(str(f))
    except Exception:  # noqa: BLE001
        continue
    for obj in env.objects:
        if obj.type.name != "MonoBehaviour":
            continue
        try:
            raw = obj.get_raw_data()
        except Exception:  # noqa: BLE001
            continue
        if not raw or len(raw) > 200000:
            continue
        pid = script_pid_of(raw)
        sname = scripts.get(pid, "?")
        ss = strings_of(raw)
        if len(ss) >= 2:
            rows.append({"file": f.name, "path_id": obj.path_id,
                         "script": sname, "script_pid": pid,
                         "len": len(raw), "strings": ss[:80]})

(OUT / "mb_groups.jsonl").write_text(
    "\n".join(json.dumps(r, ensure_ascii=False) for r in rows), encoding="utf-8")
from collections import Counter
c = Counter(r["script"] for r in rows)
print("mb objects with 2+ strings:", len(rows))
for name, cnt in c.most_common(20):
    print(f"{cnt:4d}  {name}")
