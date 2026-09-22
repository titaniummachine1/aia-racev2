#!/usr/bin/env python3
"""Mine AIComp RacingV2 car graphs (Saves\\RacingV2\\*.txt).

Emits data/race_node_catalog.json = every node kind + every dropdown `modifier`
label seen, per node kind. Those labels ARE the Racing API/sensor surface,
exactly like the soccer/tennis dumps.

Locale note: these files contain comma decimals (e.g. "1,5") -> the
(?<=\\d),(?=\\d) -> "." fix is mandatory (same rule as the tennis parser).
"""
import json
import re
import sys
from collections import defaultdict, Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
GRAPHS = HERE / "graphs"
OUT = HERE / "race_node_catalog.json"


def load(path: Path) -> dict:
    txt = path.read_text(encoding="utf-8", errors="replace")
    txt = re.sub(r"(?<=\d),(?=\d)", ".", txt)
    return json.loads(txt)


def main() -> int:
    files = sorted(GRAPHS.glob("*.txt"))
    if not files:
        print(f"no graphs in {GRAPHS}", file=sys.stderr)
        return 1

    catalog = defaultdict(lambda: {"count": 0, "modifiers": Counter(),
                                   "data_keys": Counter(), "files": set()})
    per_file = {}

    for f in files:
        try:
            d = load(f)
        except Exception as e:  # noqa: BLE001
            print(f"SKIP {f.name}: {e}", file=sys.stderr)
            continue
        ns = d.get("serializableNodes", [])
        conns = d.get("serializableConnections", [])
        per_file[f.name] = {"nodes": len(ns), "connections": len(conns)},
        for n in ns:
            kind = n.get("id", "?")
            e = catalog[kind]
            e["count"] += 1
            e["files"].add(f.name)
            mod = n.get("modifier")
            if mod:
                e["modifiers"][str(mod)] += 1
            data = n.get("data") or {}
            for k, v in data.items():
                e["data_keys"][k] += 1
                if isinstance(v, str) and v and k not in ("sID", "name"):
                    e["modifiers"][f"data.{k}={v}"] += 1

    out = {}
    for kind, e in sorted(catalog.items(), key=lambda kv: -kv[1]["count"]):
        out[kind] = {
            "count": e["count"],
            "files": sorted(e["files"]),
            "modifiers": dict(e["modifiers"].most_common()),
            "data_keys": dict(e["data_keys"].most_common()),
        }
    OUT.write_text(json.dumps(out, indent=1), encoding="utf-8")

    print(f"graphs parsed: {len(per_file)}")
    for name, meta in per_file.items():
        m = meta[0] if isinstance(meta, tuple) else meta
        print(f"  {name}: {m['nodes']} nodes, {m['connections']} connections")
    print(f"\nnode kinds: {len(out)}  -> {OUT.name}")
    print("\ntop 25 kinds:")
    for kind, e in list(out.items())[:25]:
        print(f"  {e['count']:6d}  {kind}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
