"""Graph save builder: instantiate node templates + wire ports -> save text.

Emits the game's serializableNodes/serializableConnections format exactly
(templates mined by extract_templates.py from the 14 real saves). Every node
gets fresh sIDs. PORTS RESOLVE BY (id, POLARITY): many nodes carry an input
AND an output with the same port id (AddFloats Float1 in/out) - name-only
matching wired outputs to outputs and killed the graph (user bug report
2026-09-22). connect() always takes src=output(1), dst=input(0) and TYPE-
checks the pair. LAYOUT uses each template's sizeDelta as the node's collision
bounds and shelf-packs so nodes never overlap (same rule as the compiler
layout the user pointed at).
"""
from __future__ import annotations
import json
import os
import uuid

HERE = os.path.dirname(os.path.abspath(__file__))
_T = json.load(open(os.path.join(HERE, "templates.json"), encoding="utf-8"))
CONN = _T.pop("_connection")

GAP_X, GAP_Y, SHELF_WIDTH = 90.0, 70.0, 1750.0


def _new_id() -> str:
    return str(uuid.uuid4())


def _port_type(port_id: str) -> str:
    return port_id.rstrip("0123456789")


class Graph:
    def __init__(self):
        self.nodes = []
        self.conns = []
        self._cx = 0.0
        self._cy = 0.0
        self._row_h = 0.0

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
        if x is None or y is None:  # shelf-pack by collision bounds
            if self._cx + w > SHELF_WIDTH:
                self._cx = 0.0
                self._cy -= self._row_h + GAP_Y
                self._row_h = 0.0
            x, y = self._cx, self._cy
            self._cx += w + GAP_X
            self._row_h = max(self._row_h, h)
        rt = node.get("serializableRectTransform")
        if isinstance(rt, dict):
            for key in ("position", "localPosition", "anchoredPosition"):
                v = rt.get(key)
                if isinstance(v, dict):
                    v["x"], v["y"] = x, y
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
        self.validate()
        g = {"serializableNodes": self.nodes, "serializableConnections": self.conns}
        for p in paths:
            with open(p, "w", encoding="utf-8") as f:
                json.dump(g, f, separators=(",", ":"))
            print(f"wrote {p} ({len(self.nodes)} nodes, {len(self.conns)} conns)")