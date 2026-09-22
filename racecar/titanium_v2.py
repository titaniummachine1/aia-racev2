"""TitaniumV2 - the analytical controller AS A GAME GRAPH (deployable save).

The user-consult design in graph form (aha A22/A24): severity from waypoint
angles -> corner speed -> braking envelope -> lookahead target. Loop-free and
SMALL (size = solver speed). ZERO guessed semantics:
  - severity = 1 - dot(n(P1-P0), n(P2-P1))  (sign-invariant, no compare enums)
  - v_tgt    = clamp(FAST - severity*GAIN, MIN, FAST)      [ClampFloat: v,min,max]
  - throttle = clamp((v_tgt+2 - speed)*10, 0, 1)           [pure float, no bools]
  - brake    = clamp((speed - (v_tgt+3))*0.3, 0, 1)
  - steer    = Autosteer(P2 lookahead)  [v0 muscle; v1 swaps in the custom law]
  - out      = ModularCarController Float1=throttle Float2=steer Float3=brake
Tunable constants are Float nodes so they are editable in the game editor.
TimePlot(v_tgt, speed) + Debug(brake) make every assumption OBSERVABLE.
"""
from __future__ import annotations
import os
from graph_builder import Graph

HERE = os.path.dirname(os.path.abspath(__file__))
SAVES = (r"C:\Users\Terminatort8000\AppData\LocalLow\Unicorn One\AIComp"
         r"\Saves\RacingV2\TitaniumV2.txt")
GRAPHS = r"c:\gitProjects\aialanders-legacy\games\Racing\data\graphs\TitaniumV2.txt"

# tuning (mph-ish game units; editable in-editor via the Float nodes)
FAST, GAIN, VMIN = 220.0, 350.0, 50.0
THR_NEAR, THR_GAIN = 2.0, 10.0
BRK_OVER, BRK_GAIN = 3.0, 0.3


def build() -> Graph:
    g = Graph()
    _const = {}

    def F(v):
        """Float constant node - DEDUPED (size = solver speed, user directive)."""
        if v not in _const:
            _const[v] = g.add("Float", modifier=repr(v))
        return _const[v]
    def fl(node, port="Float1"):
        return g.port(node, port)

    # ---- sensors ----
    speed = g.add("RacingV2GetFloat", modifier="0")        # Speed
    wp_prev = g.add("RacingV2GetWaypoint", modifier="1")   # Previous
    wp_next = g.add("RacingV2GetWaypoint", modifier="0")   # Next
    idx = g.add("RacingV2GetFloat", modifier="2")          # next wp index
    plus1 = g.add("AddFloats")
    g.connect(idx, "Float1", plus1, "Float1")
    g.connect(F(1.0), "Float1", plus1, "Float2")
    wp_after = g.add("RacingV2GetWaypoint", modifier="2")  # By index
    g.connect(plus1, "Float1", wp_after, "Float1")

    p0 = g.add("RacingV2Waypoint", modifier="0")           # Center of Previous
    p1 = g.add("RacingV2Waypoint", modifier="0")           # Center of Next
    p2 = g.add("RacingV2Waypoint", modifier="0")           # Center of Next+1
    g.connect(wp_prev, "Waypoint1", p0, "Waypoint1")
    g.connect(wp_next, "Waypoint1", p1, "Waypoint1")
    g.connect(wp_after, "Waypoint1", p2, "Waypoint1")

    # ---- severity = 1 - dot(n(P1-P0), n(P2-P1))  (sign-invariant) ----
    dA = g.add("SubtractVector3")
    g.connect(p1, "Vector31", dA, "Vector31")
    g.connect(p0, "Vector31", dA, "Vector32")
    nA = g.add("Normalize")
    g.connect(dA, "Vector31", nA, "Vector31")
    dB = g.add("SubtractVector3")
    g.connect(p2, "Vector31", dB, "Vector31")
    g.connect(p1, "Vector31", dB, "Vector32")
    nB = g.add("Normalize")
    g.connect(dB, "Vector31", nB, "Vector31")
    dot = g.add("DotProduct")
    g.connect(nA, "Vector31", dot, "Vector31")
    g.connect(nB, "Vector31", dot, "Vector32")
    sharp = g.add("SubtractFloats")                        # 1 - dot
    g.connect(F(1.0), "Float1", sharp, "Float1")
    g.connect(dot, "Float1", sharp, "Float2")

    # ---- v_tgt = clamp(FAST - sharp*GAIN, VMIN, FAST) ----
    drop = g.add("MultiplyFloats")
    g.connect(sharp, "Float1", drop, "Float1")
    g.connect(F(GAIN), "Float1", drop, "Float2")
    vraw = g.add("SubtractFloats")
    g.connect(F(FAST), "Float1", vraw, "Float1")
    g.connect(drop, "Float1", vraw, "Float2")
    v_tgt = g.add("ClampFloat")
    g.connect(vraw, "Float1", v_tgt, "Float1")
    g.connect(F(VMIN), "Float1", v_tgt, "Float2")
    g.connect(F(FAST), "Float1", v_tgt, "Float3")

    # ---- throttle = clamp((v_tgt+THR_NEAR - speed)*THR_GAIN, 0, 1) ----
    want = g.add("AddFloats")
    g.connect(v_tgt, "Float1", want, "Float1")
    g.connect(F(THR_NEAR), "Float1", want, "Float2")
    tgap = g.add("SubtractFloats")
    g.connect(want, "Float1", tgap, "Float1")
    g.connect(speed, "Float1", tgap, "Float2")
    tscale = g.add("MultiplyFloats")
    g.connect(tgap, "Float1", tscale, "Float1")
    g.connect(F(THR_GAIN), "Float1", tscale, "Float2")
    throttle = g.add("ClampFloat")
    g.connect(tscale, "Float1", throttle, "Float1")
    g.connect(F(0.0), "Float1", throttle, "Float2")
    g.connect(F(1.0), "Float1", throttle, "Float3")

    # ---- brake = clamp((speed - (v_tgt+BRK_OVER))*BRK_GAIN, 0, 1) ----
    limit = g.add("AddFloats")
    g.connect(v_tgt, "Float1", limit, "Float1")
    g.connect(F(BRK_OVER), "Float1", limit, "Float2")
    bgap = g.add("SubtractFloats")
    g.connect(speed, "Float1", bgap, "Float1")
    g.connect(limit, "Float1", bgap, "Float2")
    bscale = g.add("MultiplyFloats")
    g.connect(bgap, "Float1", bscale, "Float1")
    g.connect(F(BRK_GAIN), "Float1", bscale, "Float2")
    brake = g.add("ClampFloat")
    g.connect(bscale, "Float1", brake, "Float1")
    g.connect(F(0.0), "Float1", brake, "Float2")
    g.connect(F(1.0), "Float1", brake, "Float3")

    # ---- steering muscle (v0): Autosteer toward the P2 lookahead ----
    steer = g.add("Autosteer")
    g.connect(p2, "Vector31", steer, "Vector31")

    # ---- controller + observability ----
    ctl = g.add("ModularCarController", x=900.0, y=0.0)
    g.connect(throttle, "Float1", ctl, "Float1")
    g.connect(steer, "Float1", ctl, "Float2")
    g.connect(brake, "Float1", ctl, "Float3")
    plot = g.add("TimePlot", x=900.0, y=200.0)
    g.connect(g.add("String", modifier="v_tgt"), "String1", plot, "String1")
    g.connect(v_tgt, "Float1", plot, "Float1")
    g.connect(g.add("String", modifier="speed"), "String1", plot, "String2")
    g.connect(speed, "Float1", plot, "Float2")
    dbg = g.add("Debug", x=900.0, y=-200.0)
    g.connect(brake, "Float1", dbg, "Any1")
    return g


if __name__ == "__main__":
    build().save(SAVES, GRAPHS)