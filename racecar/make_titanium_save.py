"""make_titanium_save.py - emit the Titanium car save for the game.

Template: data/embedded_graph_3.txt (dev minimal waypoint follower, Stat[2,10,3],
byte-identical to graphs/AIA.txt). We set the stat allocation (Titanium build),
name the car via the String node, and write BOTH load points:
  - Saves/RacingV2/Titanium.txt        (default-saves load, loadDefaultSaves)
  - games/Racing/data/graphs/Titanium.txt (LoadGraphTexts picker)

Stat mapping is by PORT INDEX on ConstructRacingV2Properties (user screenshot:
port order = color, stat(max speed), stat(turning), stat(health)), traced via
serializableConnections port0SID -> port1SID. Script prints its trace so the
mapping is auditable - never guess silently.
"""
from __future__ import annotations
import json
import shutil
import sys

TEMPLATE = r"c:\gitProjects\aia-racev2\data\embedded_graph_3.txt"
SAVES = (r"C:\Users\Terminatort8000\AppData\LocalLow\Unicorn One\AIComp"
         r"\Saves\RacingV2\Titanium.txt")
GRAPHS = r"c:\gitProjects\aialanders-legacy\games\Racing\data\graphs\Titanium.txt"


def build(speed=4, turn=10, health=6, name="Titanium"):
    g = json.load(open(TEMPLATE, encoding="utf-8-sig"))
    nodes = g["serializableNodes"]
    conns = g.get("serializableConnections", [])

    props = next(n for n in nodes if "ConstructRacingV2Properties" in n["id"])
    ports = props["serializablePorts"]
    print("props ports:")
    for i, p in enumerate(ports):
        print(f"  [{i}] id={p.get('id')} sID={p.get('sID')[:8]}")

    # stat mapping BY PORT ID (Stat1=speed, Stat2=turn, Stat3=health =
    # RACING_CONSTANTS); port ORDER in JSON is NOT the visual order, and the
    # connection direction is not guaranteed -> accept either end.
    PORTMAP = {"Stat1": "speed", "Stat2": "turn", "Stat3": "health"}
    vals = {"speed": speed, "turn": turn, "health": health}
    set_count = 0
    for n in nodes:
        if n.get("id") != "Stat":
            continue
        my_sids = {p["sID"] for p in n.get("serializablePorts", [])}
        for c in conns:
            if c.get("port0SID") in my_sids:
                other = c.get("port1SID")
            elif c.get("port1SID") in my_sids:
                other = c.get("port0SID")
            else:
                continue
            port = next((p for p in ports if p["sID"] == other), None)
            label = PORTMAP.get(port.get("id")) if port else None
            if label:
                old = n.get("modifier")
                n["modifier"] = str(vals[label])
                print(f"Stat -> {label} ({port['id']}): {old} => {vals[label]}")
                set_count += 1
    assert set_count == 3, f"expected 3 stat links, found {set_count} - not writing"

    for n in nodes:
        if n.get("id") == "String":
            n["modifier"] = name
            print(f"String node named: {name}")

    for out in (SAVES, GRAPHS):
        with open(out, "w", encoding="utf-8") as f:
            json.dump(g, f, separators=(",", ":"))
        print(f"wrote {out}")
    return g


if __name__ == "__main__":
    a = sys.argv[1:]
    speed, turn, health = (int(a[0]), int(a[1]), int(a[2])) if len(a) >= 3 else (4, 10, 6)
    assert 0 <= speed <= 10 and 0 <= turn <= 10 and 0 <= health <= 10
    assert speed + turn + health <= 20, "20-point budget exceeded"
    build(speed, turn, health)