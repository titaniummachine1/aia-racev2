#!/usr/bin/env python3
"""Dump a RacingV2 car graph as a readable expression tree.

Ported from the tennis graph reader (same save format: serializableNodes /
serializableConnections, port polarity 1 = output, 0 = input).

Usage:
  python graph_dump.py "graphs/roody.txt"                 # node kind histogram
  python graph_dump.py "graphs/roody.txt" --tree          # feed tree of every
                                                          # value-producing node
  python graph_dump.py "graphs/roody.txt" --find RelativePosition
"""
import argparse
import json
import re
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent


def load(path: Path) -> dict:
    txt = path.read_text(encoding="utf-8", errors="replace")
    txt = re.sub(r"(?<=\d),(?=\d)", ".", txt)   # locale-comma rule
    return json.loads(txt)


def build_index(nodes):
    port_owner = {}
    for n in nodes:
        for p in n["serializablePorts"]:
            port_owner[p["sID"]] = (n, p)
    return port_owner


def build_ins(conns, port_owner):
    ins = {}
    for c in conns:
        n0, p0 = port_owner.get(c.get("port0SID"), (None, None))
        n1, p1 = port_owner.get(c.get("port1SID"), (None, None))
        if not n0 or not n1:
            continue
        if p0["polarity"] == 1:          # n0 out -> n1 in
            ins[p1["sID"]] = (n0, p0["id"])
    return ins


def feeder(node, port_id, ins):
    for p in node["serializablePorts"]:
        if p["polarity"] == 0 and p["id"] == port_id:
            r = ins.get(p["sID"])
            return r[0] if r else None
    return None


def label(n):
    if n is None:
        return "-"
    data = n.get("data") or {}
    if n["id"] == "Float":
        return f"F({data.get('value', '?')})"
    if n["id"] == "String":
        return repr(data.get("value", "?"))
    mod = n.get("modifier") or ""
    tag = f"[{mod}]" if mod != "" else ""
    return f"{n['id']}{tag}#{n['sID'][-4:]}"


def tree(n, ins, lvl=0, maxlvl=6):
    if n is None:
        return "-"
    if lvl >= maxlvl:
        return label(n)
    ins_txt = []
    for p in n["serializablePorts"]:
        if p["polarity"] == 0:
            f = feeder(n, p["id"], ins)
            ins_txt.append(f"{p['id']}=" + (tree(f, ins, lvl + 1, maxlvl) if f else "?"))
    return label(n) + "(" + ", ".join(ins_txt) + ")"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("graph")
    ap.add_argument("--tree", action="store_true")
    ap.add_argument("--find", metavar="NODEKIND")
    ap.add_argument("--depth", type=int, default=6)
    a = ap.parse_args()

    path = Path(a.graph)
    if not path.is_absolute():
        path = HERE / path
    d = load(path)
    nodes = d["serializableNodes"]
    conns = d.get("serializableConnections", [])
    port_owner = build_index(nodes)
    ins = build_ins(conns, port_owner)

    print(f"{path.name}: {len(nodes)} nodes, {len(conns)} connections")

    if a.find:
        for n in nodes:
            if n["id"] == a.find:
                print("  " + tree(n, ins, 0, a.depth))
        return 0

    hist = Counter(n["id"] for n in nodes)
    for kind, c in hist.most_common():
        print(f"  {c:5d}  {kind}")

    if a.tree:
        print("\n--- value-producing trees ---")
        for n in nodes:
            if n["id"] in ("SetThrottle", "SetSteering", "SetFloat",
                           "ConditionalSetFloatV2"):
                print("  " + tree(n, ins, 0, a.depth))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
