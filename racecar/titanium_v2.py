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

# tuning (game units; editable in-editor via the Float nodes)
FAST, GAIN, VMIN = 220.0, 350.0, 50.0
THR_NEAR, THR_GAIN = 2.0, 10.0
BRK_OVER, BRK_GAIN = 3.0, 0.3
# turn feasibility (closed-form rollout substitute, user 2026-09-22)
A_LAT = 25.0           # lateral grip budget in game units      [calibrate]
A_BRAKE = 8.0          # braking decel                          [calibrate]
K_WIDTH = 0.1          # corridor width -> radius factor        [calibrate]
# STAT BUDGET = 15 POINTS TOTAL (user 2026-09-22 - the 20 in RACING_CONSTANTS
# is WRONG; overexerting makes the game trim points top-to-bottom).

# optional inputs documented as such (validate() tolerates only these)
ALLOW_UNWIRED = {
    "RacingV2GetWaypoint": ["Float1"],   # index only needed in By-index mode
    "RacingV2Waypoint": ["Transform1"],  # hint: optional
    "TimePlot": ["Color1", "String2", "Float2", "Float3"],  # 1 series: name+value
}


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

    # ---- car properties block (the dev car's full 12-port pattern) ----
    props = g.add("ConstructRacingV2Properties")
    g.connect(g.add("String", modifier="TitaniumV2"), "String1", props, "String1")
    g.connect(g.add("String", modifier=""), "String1", props, "String2")
    car_color = g.add("Color", modifier="Yellow")
    g.connect(car_color, "Color1", props, "Color1")
    g.connect(g.add("Color", modifier="Yellow"), "Color1", props, "Color2")
    g.connect(g.add("RandomColor", modifier="Blonde"), "Color1", props, "Color3")
    g.connect(g.add("Country", modifier="United States of America"),
              "Country1", props, "Country1")
    for port in ("Float1", "Float2", "Float3"):
        g.connect(F(0.0), "Float1", props, port)
    g.connect(g.add("Stat", modifier="7"), "Stat1", props, "Stat1")    # speed (15-pt budget)
    g.connect(g.add("Stat", modifier="8"), "Stat1", props, "Stat2")    # turn (was not enough)
    g.connect(g.add("Stat", modifier="0"), "Stat1", props, "Stat3")    # health: useless

    # ---- sensors ----
    speed = g.add("RacingV2GetFloat", modifier="0")        # Speed
    wp_prev = g.add("RacingV2GetWaypoint", modifier="1")   # Previous
    wp_next = g.add("RacingV2GetWaypoint", modifier="0")   # Next
    g.connect(F(0.0), "Float1", wp_prev, "Float1")        # index: never null
    g.connect(F(0.0), "Float1", wp_next, "Float1")
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

    # ---- apex racing line from the Left/Right corridor edges (user 2026-09-22;
    #      Waypoint.GetLeftPoint/GetRightPoint = car-width-aware edges) ----
    # target = L + (R - L) * clamp(+cross_y * KAPEX, TMIN, TMAX). Unity is
    # LEFT-handed: cross_y = nA.z*nB.x - nA.x*nB.z > 0 on a RIGHT turn -> t->1
    # -> Right edge = INSIDE of a right turn (round 5 fix: the first build had
    # the sign inverted and drove the OUTSIDE line into the wall, user report).
    # TMIN/TMAX = 0.25/0.75: NEVER aim at the edge point - the car has width
    # and scrapes (user: totalled on the wall). All three = live-tunable nodes.
    l1 = g.add("RacingV2Waypoint", modifier="1")            # Left edge of Next
    r1 = g.add("RacingV2Waypoint", modifier="2")            # Right edge of Next
    g.connect(wp_next, "Waypoint1", l1, "Waypoint1")
    g.connect(wp_next, "Waypoint1", r1, "Waypoint1")
    sa = g.add("Vector3Split")
    g.connect(nA, "Vector31", sa, "Vector31")
    sb = g.add("Vector3Split")
    g.connect(nB, "Vector31", sb, "Vector31")
    m1 = g.add("MultiplyFloats")                            # nA.z * nB.x
    g.connect(sa, "Float3", m1, "Float1")
    g.connect(sb, "Float1", m1, "Float2")
    m2 = g.add("MultiplyFloats")                            # nA.x * nB.z
    g.connect(sa, "Float1", m2, "Float1")
    g.connect(sb, "Float3", m2, "Float2")
    crossy = g.add("SubtractFloats")                        # az*bx - ax*bz
    g.connect(m1, "Float1", crossy, "Float1")
    g.connect(m2, "Float1", crossy, "Float2")
    tside = g.add("MultiplyFloats")
    g.connect(crossy, "Float1", tside, "Float1")
    g.connect(F(2.0), "Float1", tside, "Float2")            # KAPEX + (handedness!)
    tclamp = g.add("ClampFloat")
    g.connect(tside, "Float1", tclamp, "Float1")
    g.connect(F(0.25), "Float1", tclamp, "Float2")          # stay 25% off the L edge
    g.connect(F(0.75), "Float1", tclamp, "Float3")          # stay 25% off the R edge
    edge = g.add("SubtractVector3")                         # R - L
    g.connect(r1, "Vector31", edge, "Vector31")
    g.connect(l1, "Vector31", edge, "Vector32")
    escale = g.add("ScaleVector3")                          # (R - L) * t
    g.connect(edge, "Vector31", escale, "Vector31")
    g.connect(tclamp, "Float1", escale, "Float1")
    apex = g.add("AddVector3")                              # L + (R - L) * t
    g.connect(l1, "Vector31", apex, "Vector31")
    g.connect(escale, "Vector31", apex, "Vector32")

    # ---- steering muscle (v0): Autosteer toward the computed apex ----
    steer = g.add("Autosteer")
    g.connect(apex, "Vector31", steer, "Vector31")

    # ---- turn feasibility: WILL THIS CORNER EAT THE WALL? (user 2026-09-22) ----
    # Closed-form rollout: corner radius R = abc/(2|cross_y|) from the waypoint
    # triple, shrunk when the corridor is narrow (the waypoint dims - edges are
    # car-extent aware via ProjectExtentsOntoAxis), then the energy test
    #   v^2 > A_LAT*R_eff + 2*A_BRAKE*d  ->  brake BEFORE the corner.
    # Squares only - no sqrt/power node needed (there is none in the ISA!).
    car_ref = g.add("RacingV2GetCar", modifier="2")            # Self
    g.connect(F(0.0), "Float1", car_ref, "Float1")
    cinfo = g.add("CarInfo")
    g.connect(car_ref, "Car1", cinfo, "Car1")
    cpos = g.add("RelativePosition", modifier="13")            # car world position
    g.connect(cinfo, "Transform1", cpos, "Transform1")
    ma = g.add("Magnitude")
    g.connect(dA, "Vector31", ma, "Vector31")
    mb = g.add("Magnitude")
    g.connect(dB, "Vector31", mb, "Vector31")
    dsum = g.add("AddVector3")                                 # (P2-P0) chord
    g.connect(dA, "Vector31", dsum, "Vector31")
    g.connect(dB, "Vector31", dsum, "Vector32")
    mc = g.add("Magnitude")
    g.connect(dsum, "Vector31", mc, "Vector31")
    ab = g.add("MultiplyFloats")
    g.connect(ma, "Float1", ab, "Float1")
    g.connect(mb, "Float1", ab, "Float2")
    abc = g.add("MultiplyFloats")
    g.connect(ab, "Float1", abc, "Float1")
    g.connect(mc, "Float1", abc, "Float2")
    crossabs = g.add("AbsFloat")
    g.connect(crossy, "Float1", crossabs, "Float1")
    denom = g.add("MultiplyFloats")                            # 2*|cross_y|
    g.connect(F(2.0), "Float1", denom, "Float1")
    g.connect(crossabs, "Float1", denom, "Float2")
    radius = g.add("DivideFloats")                             # R = abc/(2|cross|)
    g.connect(abc, "Float1", radius, "Float1")
    g.connect(denom, "Float1", radius, "Float2")
    width = g.add("Distance")                                  # corridor width |L-R|
    g.connect(l1, "Vector31", width, "Vector31")
    g.connect(r1, "Vector31", width, "Vector32")
    wscale = g.add("MultiplyFloats")
    g.connect(width, "Float1", wscale, "Float1")
    g.connect(F(K_WIDTH), "Float1", wscale, "Float2")
    wfac = g.add("ClampFloat")                                 # narrow track -> shrink
    g.connect(wscale, "Float1", wfac, "Float1")
    g.connect(F(0.5), "Float1", wfac, "Float2")
    g.connect(F(1.0), "Float1", wfac, "Float3")
    ref_r = g.add("MultiplyFloats")                            # R_eff = R * width factor
    g.connect(radius, "Float1", ref_r, "Float1")
    g.connect(wfac, "Float1", ref_r, "Float2")
    grip_r = g.add("MultiplyFloats")                           # A_LAT * R_eff
    g.connect(F(A_LAT), "Float1", grip_r, "Float1")
    g.connect(ref_r, "Float1", grip_r, "Float2")
    dist_c = g.add("Distance")                                 # d to the corner
    g.connect(cpos, "Vector31", dist_c, "Vector31")
    g.connect(p1, "Vector31", dist_c, "Vector32")
    brakebuf = g.add("MultiplyFloats")                         # 2*A_BRAKE*d
    g.connect(F(2.0 * A_BRAKE), "Float1", brakebuf, "Float1")
    g.connect(dist_c, "Float1", brakebuf, "Float2")
    budget_sum = g.add("AddFloats")                            # A_LAT*R + 2ab*d
    g.connect(grip_r, "Float1", budget_sum, "Float1")
    g.connect(brakebuf, "Float1", budget_sum, "Float2")
    vsq = g.add("MultiplyFloats")                              # v^2
    g.connect(speed, "Float1", vsq, "Float1")
    g.connect(speed, "Float1", vsq, "Float2")
    vsgap = g.add("SubtractFloats")                            # over budget?
    g.connect(vsq, "Float1", vsgap, "Float1")
    g.connect(budget_sum, "Float1", vsgap, "Float2")
    vscale = g.add("MultiplyFloats")
    g.connect(vsgap, "Float1", vscale, "Float1")
    g.connect(F(0.05), "Float1", vscale, "Float2")
    brake_corner = g.add("ClampFloat")
    g.connect(vscale, "Float1", brake_corner, "Float1")
    g.connect(F(0.0), "Float1", brake_corner, "Float2")
    g.connect(F(1.0), "Float1", brake_corner, "Float3")

    # ---- wall brake from the front ray (driver_config: r=0.75 d=12 avoid=4) ----
    sc = g.add("Spherecast")
    g.connect(F(0.75), "Float1", sc, "Float1")
    g.connect(F(12.0), "Float1", sc, "Float2")
    rays = g.add("CarRaycasts")
    g.connect(sc, "Spherecast1", rays, "Spherecast1")
    hit = g.add("HitInfo")
    g.connect(rays, "RaycastHit2", hit, "RaycastHit1")    # centre-front candidate
    gapw = g.add("SubtractFloats")
    g.connect(F(4.0), "Float1", gapw, "Float1")
    g.connect(hit, "Float1", gapw, "Float2")
    wscale = g.add("MultiplyFloats")
    g.connect(gapw, "Float1", wscale, "Float1")
    g.connect(F(0.5), "Float1", wscale, "Float2")
    brake_wall = g.add("ClampFloat")
    g.connect(wscale, "Float1", brake_wall, "Float1")
    g.connect(F(0.0), "Float1", brake_wall, "Float2")
    g.connect(F(1.0), "Float1", brake_wall, "Float3")
    # max3(brake, brake_wall, brake_corner) = two ((a+b-|a-b|)/2) stages
    # (no compare enums needed anywhere)
    bsum = g.add("AddFloats")
    g.connect(brake, "Float1", bsum, "Float1")
    g.connect(brake_wall, "Float1", bsum, "Float2")
    bdf = g.add("SubtractFloats")
    g.connect(brake, "Float1", bdf, "Float1")
    g.connect(brake_wall, "Float1", bdf, "Float2")
    babs = g.add("AbsFloat")
    g.connect(bdf, "Float1", babs, "Float1")
    bmax = g.add("SubtractFloats")
    g.connect(bsum, "Float1", bmax, "Float1")
    g.connect(babs, "Float1", bmax, "Float2")
    bpair = g.add("MultiplyFloats")
    g.connect(bmax, "Float1", bpair, "Float1")
    g.connect(F(0.5), "Float1", bpair, "Float2")               # max(brake, wall)
    csum = g.add("AddFloats")
    g.connect(bpair, "Float1", csum, "Float1")
    g.connect(brake_corner, "Float1", csum, "Float2")
    cdf = g.add("SubtractFloats")
    g.connect(bpair, "Float1", cdf, "Float1")
    g.connect(brake_corner, "Float1", cdf, "Float2")
    cabs = g.add("AbsFloat")
    g.connect(cdf, "Float1", cabs, "Float1")
    cmax = g.add("SubtractFloats")
    g.connect(csum, "Float1", cmax, "Float1")
    g.connect(cabs, "Float1", cmax, "Float2")
    brake_out = g.add("MultiplyFloats")
    g.connect(cmax, "Float1", brake_out, "Float1")
    g.connect(F(0.5), "Float1", brake_out, "Float2")           # max3 result

    # ---- controller + observability (TimePlot = ONE series: name + value) ----
    ctl = g.add("ModularCarController")
    thr_cut = g.add("SubtractFloats")                          # 1 - brake: lift
    g.connect(F(1.0), "Float1", thr_cut, "Float1")
    g.connect(brake_out, "Float1", thr_cut, "Float2")
    thr_final = g.add("MultiplyFloats")                        # throttle * lift
    g.connect(throttle, "Float1", thr_final, "Float1")
    g.connect(thr_cut, "Float1", thr_final, "Float2")
    g.connect(thr_final, "Float1", ctl, "Float1")
    g.connect(steer, "Float1", ctl, "Float2")
    g.connect(brake_out, "Float1", ctl, "Float3")
    plot1 = g.add("TimePlot")
    g.connect(g.add("String", modifier="v_tgt"), "String1", plot1, "String1")
    g.connect(v_tgt, "Float1", plot1, "Float1")
    plot2 = g.add("TimePlot")
    g.connect(g.add("String", modifier="speed"), "String1", plot2, "String1")
    g.connect(speed, "Float1", plot2, "Float1")
    dbg = g.add("Debug")
    g.connect(brake_out, "Float1", dbg, "Any1")
    dbg2 = g.add("Debug")
    g.connect(hit, "Float1", dbg2, "Any1")   # ray-B distance: identify the ray

    # tag the documented-optional inputs for validate()
    g._allow_unwired = set()
    for n in g.nodes:
        for p in n.get("serializablePorts", []):
            if p.get("id") in ALLOW_UNWIRED.get(n["id"], []):
                g._allow_unwired.add(p["sID"])
    return g


if __name__ == "__main__":
    build().save(SAVES, GRAPHS)