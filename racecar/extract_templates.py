"""Mine complete node/connection templates from every real graph save.

Writes racecar/templates.json = { "_connection": <conn template>,
                                  "<node id>": <first full node dict>, ... }
and prints each node id with its port map (port id, polarity) - that print IS
the builder's port reference (aha A24: python only BUILDS/ANALYSES artefacts).
"""
from __future__ import annotations
import glob
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
GRAPHS = r"c:\gitProjects\aialanders-legacy\games\Racing\data\graphs"


def main():
    out = {}
    conn = None
    for path in sorted(glob.glob(os.path.join(GRAPHS, "*.txt"))):
        if os.path.basename(path).lower().startswith("titanium"):
            continue  # our own outputs are not templates
        try:
            g = json.load(open(path, encoding="utf-8-sig"))
        except Exception as e:  # one bad save must not kill the sweep
            print(f"skip {os.path.basename(path)}: {e}")
            continue
        for n in g.get("serializableNodes", []):
            out.setdefault(n["id"], n)
        for c in g.get("serializableConnections", []):
            if conn is None and isinstance(c.get("line"), dict):
                conn = c

    payload = {"_connection": conn}
    payload.update(out)
    with open(os.path.join(HERE, "templates.json"), "w", encoding="utf-8") as f:
        json.dump(payload, f, indent=1)
    print(f"templates.json: {len(out)} node types, conn={'yes' if conn else 'NO'}")
    print()
    for k in sorted(out):
        v = out[k]
        ports = [(p.get("id"), p.get("polarity")) for p in v.get("serializablePorts", [])]
        print(f"{k:28s} {ports}")


if __name__ == "__main__":
    main()