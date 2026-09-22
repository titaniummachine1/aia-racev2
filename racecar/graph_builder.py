"""Graph save builder: instantiate node templates + wire ports -> save text.

Emits the game's serializableNodes/serializableConnections format exactly
(templates mined by extract_templates.py from the 14 real saves). Every node
gets fresh sIDs; ports are addressable by their template port id.

Usage sketch:
    g = Graph()
    a = g.add("Float", modifier="200")
    b = g.add("Auto Throttle")
    g.connect(a, "Float1", b, "float")
    g.save(path)
"""
from __future__ import annotations
import json
import os
import uuid

HERE = os.path.dirname(os.path.abspath(__file__))
_T = json.load(open(os.path.join(HERE, "templates.json"), encoding="utf-8"))
CONN = _T.pop("_connection")


def _new_id() -> str:
    return str(uuid.uuid4())


class Graph:
    def __init__(self):
        self.nodes = []
        self.conns = []

    def add(self, type_id, modifier=None, x=0.0, y=0.0):
        if type_id not in _T:
            raise KeyError(f"no template for {type_id!r}; have {sorted(_T)}")
        node = json.loads(json.dumps(_T[type_id]))  # deep copy
        node["sID"] = _new_id()
        if modifier is not None:
            node["modifier"] = modifier
        for p in node.get("serializablePorts", []):
            p["sID"] = _new_id()
            p["nodeSID"] = node["sID"]
        rt = node.get("serializableRectTransform")
        if isinstance(rt, dict):
            for key in ("position", "localPosition", "anchoredPosition"):
                v = rt.get(key)
                if isinstance(v, dict):
                    v["x"], v["y"] = x, y
        self.nodes.append(node)
        return node

    def port(self, node, port_id):
        for p in node.get("serializablePorts", []):
            if p.get("id") == port_id:
                return p
        have = [p.get("id") for p in node.get("serializablePorts", [])]
        raise KeyError(f"{node['id']} has no port {port_id!r}; have {have}")

    def connect(self, src_node, src_port, dst_node, dst_port):
        a = self.port(src_node, src_port)
        b = self.port(dst_node, dst_port)
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

    def save(self, *paths):
        g = {"serializableNodes": self.nodes, "serializableConnections": self.conns}
        for p in paths:
            with open(p, "w", encoding="utf-8") as f:
                json.dump(g, f, separators=(",", ":"))
            print(f"wrote {p} ({len(self.nodes)} nodes, {len(self.conns)} conns)")