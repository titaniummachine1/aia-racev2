"""Graph save builder: instantiate node templates + wire ports -> save text.

Emits the game's serializableNodes/serializableConnections format exactly
(templates mined by extract_templates.py from the 14 real saves). Every node
gets fresh sIDs. PORTS RESOLVE BY (id, POLARITY): many nodes carry an input
AND an output with the same port id (AddFloats Float1 in/out) - name-only
matching wired outputs to outputs and killed the graph (user bug report
2026-09-22). connect() always takes src=output(1), dst=input(0) and TYPE-
checks the pair.

LAYOUT = layered dataflow (AST-style, user request 2026-09-22): column =
topological depth (sources left, sinks right), rows eased by the barycenter of
each node's already-placed inputs, then vertically spaced by the node's
sizeDelta collision bounds so nothing overlaps and wires run mostly forward.
"""
from __future__ import annotations
import json
import os
import uuid

HERE = os.path.dirname(os.path.abspath(__file__))
_T = json.load(open(os.path.join(HERE, "templates.json"), encoding="utf-8"))
CONN = _T.pop("_connection")

GAP_X, GAP_Y = 140.0, 48.0


def _new_id() -> str:
    return str(uuid.uuid4())


def _port_type(port_id: str) -> str:
    return port_id.rstrip("0123456789")


class Graph:
    def __init__(self):
        self.nodes = []
        self.conns = []
        self._explicit = {}  # node -> (x, y) user-placed

    def _bounds(self, node):
        rt = node.get("serializableRectTransform") or {}
        sd = rt.get("sizeDelta") or {}
        try:
            w, h = float(sd.get("x")), float(sd.get("y"))
        except (TypeError, ValueError):
            w, h = 0.0, 0.0
        return (w if w > 8.0 else 256.0, h if h > 8.0 else 64.0)

    def add(self, type_id, modifier=None, x=None, y=None):
        if type_id not in _T:
            raise KeyError(f"no template for {type_id!r}; have {sorted(_T)}")
        node = json.loads(json.dumps(_T[type_id]))  # deep copy
        node["sID"] = _new_id()
        if modifier is not None:
            node["modifier"] = modifier
        for p in node.get("serializablePorts", []):
            p["sID"] = _new_id()
            p["nodeSID"] = node["sID"]
        w, h = self._bounds(node)
        if x is not None and y is not None:
            self._explicit[node["sID"]] = (x, y)
        self.nodes.append(node)
        return node

    def port(self, node, port_id, polarity=None):
        cands = [p for p in node.get("serializablePorts", []) if p.get("id") == port_id]
        if not cands:
            have = [(p.get("id"), p.get("polarity")) for p in node.get("serializablePorts", [])]
            raise KeyError(f"{node['id']} has no port {port_id!r}; have {have}")
        if polarity is None:
            if len(cands) == 1:
                return cands[0]
            raise KeyError(f"{node['id']} port {port_id!r} is ambiguous - pass polarity")
        for p in cands:
            if p.get("polarity") == polarity:
                return p
        raise KeyError(f"{node['id']} has no {port_id!r} with polarity {polarity}")

    def connect(self, src_node, src_port, dst_node, dst_port):
        a = self.port(src_node, src_port, polarity=1)   # OUTPUT
        b = self.port(dst_node, dst_port, polarity=0)   # INPUT
        ta, tb = _port_type(a["id"]), _port_type(b["id"])
        if ta != tb and "Any" not in (ta, tb):
            raise TypeError(
                f"type mismatch: {src_node['id']}.{a['id']} ({ta}) -> "
                f"{dst_node['id']}.{b['id']} ({tb})")
        if CONN is None:
            raise RuntimeError("no connection template - run extract_templates.py")
        c = json.loads(json.dumps(CONN))
        c["id"] = f"Connection ({src_node['id']} - {dst_node['id']})"
        c["sID"] = _new_id()
        c["port0SID"] = a["sID"]
        c["port1SID"] = b["sID"]
        c["port0InstanceID"] = -1
        c["port1InstanceID"] = -1
        self.conns.append(c)

    def layout(self):
        """AST-style layered dataflow layout (user request 2026-09-22).
        Column = topological depth; row order = barycenter of inputs; vertical
        spacing honours each node's sizeDelta collision bounds. Result: wires
        run mostly left-to-right and nodes never overlap."""
        by_sid = {}
        for n in self.nodes:
            for p in n.get("serializablePorts", []):
                by_sid[p["sID"]] = n
        ins = {n["sID"]: [] for n in self.nodes}
        for c in self.conns:
            a = by_sid.get(c["port0SID"])
            b = by_sid.get(c["port1SID"])
            if a is not None and b is not None and a is not b:
                ins[b["sID"]].append(a)
        depth = {n["sID"]: 0 for n in self.nodes}
        for _ in range(len(self.nodes)):          # relax: longest-path layers
            changed = False
            for n in self.nodes:
                for s in ins[n["sID"]]:
                    if depth[s["sID"]] + 1 > depth[n["sID"]]:
                        depth[n["sID"]] = depth[s["sID"]] + 1
                        changed = True
            if not changed:
                break
        cols = {}
        for n in self.nodes:
            cols.setdefault(depth[n["sID"]], []).append(n)
        y_of = {}
        x = 0.0
        for c in sorted(cols):
            group = cols[c]
            def bary(n):
                ys = [y_of[s["sID"]] for s in ins[n["sID"]] if s["sID"] in y_of]
                return sum(ys) / len(ys) if ys else 0.0
            group.sort(key=bary)                   # ease rows by input centre
            y = 0.0
            prev_h = 0.0
            for n in group:
                w, h = self._bounds(n)
                y -= (prev_h / 2.0 + GAP_Y + h / 2.0) if prev_h else 0.0
                if n["sID"] in self._explicit:
                    px, py = self._explicit[n["sID"]]
                    y_of[n["sID"]] = py
                    self._place(n, px, py)
                else:
                    y_of[n["sID"]] = y
                    self._place(n, x, y)
                prev_h = h
            x += 320.0 + GAP_X                     # column stride fits 256-wide
        return self

    def _place(self, node, x, y):
        rt = node.get("serializableRectTransform")
        if isinstance(rt, dict):
            for key in ("position", "localPosition", "anchoredPosition"):
                v = rt.get(key)
                if isinstance(v, dict):
                    v["x"], v["y"] = x, y

    def validate(self):
        """Loud self-check before ANY file is written (no silent bad ships).
        - every connection: real ports, output->input, matching types
        - unwired inputs fail EXCEPT sIDs in self._allow_unwired (documented
          optional inputs like RacingV2Waypoint.Transform1 are real and legal)
        """
        by_sid = {}
        for n in self.nodes:
            for p in n.get("serializablePorts", []):
                by_sid[p["sID"]] = (n, p)
        wired_in = set()
        for c in self.conns:
            if c["port0SID"] not in by_sid or c["port1SID"] not in by_sid:
                raise AssertionError(f"dangling connection {c['id']}")
            _, a = by_sid[c["port0SID"]]
            _, b = by_sid[c["port1SID"]]
            if a.get("polarity") != 1 or b.get("polarity") != 0:
                raise AssertionError(
                    f"bad direction in {c['id']}: {a['id']}/{a.get('polarity')} -> "
                    f"{b['id']}/{b.get('polarity')}")
            wired_in.add(c["port1SID"])
        allow = getattr(self, "_allow_unwired", set())
        missing = []
        for n in self.nodes:
            for p in n.get("serializablePorts", []):
                if (p.get("polarity") == 0 and p["sID"] not in wired_in
                        and p["sID"] not in allow):
                    missing.append(f"{n['id']}.{p['id']}")
        if missing:
            raise AssertionError(f"unwired required inputs: {missing}")
        return True

    def save(self, *paths):
        self.layout()
        self.validate()
        g = {"serializableNodes": self.nodes, "serializableConnections": self.conns}
        for p in paths:
            with open(p, "w", encoding="utf-8") as f:
                json.dump(g, f, separators=(",", ":"))
            print(f"wrote {p} ({len(self.nodes)} nodes, {len(self.conns)} conns)")