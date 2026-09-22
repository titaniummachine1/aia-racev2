# RacingV2 — driving constants & world model (updated 2026-09-22)

Authoritative source for the values below: the 2026 AIGamePyLibrary fork
(`worldcupteams/AIGamePyLibrary`), the Racing docs in
`games/Racing/data/docs/` (`NODE_FINDINGS.md`, `COMMUNITY_INTEL.md`),
14 real car graphs, **and the game's own TextAssets** (driver config +
3 dev-authored embedded graphs, extracted from `sharedassets*.assets`).
**Everything marked `[unverified]` is educated from ABI scrapes; RacingV2 has
NO parity simulator.** The compiler mirrors these in
`AIGamePyLibrary/racing/constants.py` with a `CERTAINTY` registry.

## 0. CERTAIN — shipped driver config (TextAsset, v0.22)

The built-in demo driver's own tuning, byte-extracted from `driver_config.json`:

| Key | Value | Meaning |
|---|---|---|
| `throttle` | **0.9** | demo driver's fixed throttle |
| `brake` | **0.0** | (demo never brakes) |
| `steerSensitivity` | **1.0** | input → steer scale |
| `spherecastRadius` | **0.75** | obstacle spherecast radius |
| `spherecastDistance` | **12.0** | obstacle spherecast length |
| `avoidNearDistance` | **4.0** | start steering away at this range |
| `avoidSteerStrength` | **1.25** | avoidance steering gain |
| `obstacleAvoidance` | `true` | on by default |

These are the first **verified** racing numbers. Pair them with the
`RacingV2Waypoint` Center/Next pattern (dev graph #3, below) for a known-good
follower.

## 0b. CERTAIN — 3 dev-authored reference graphs (TextAssets)

Extracted from `sharedassets0.assets` → `graphs/embedded_graph_*.txt`:

| graph | nodes | what it is | key wiring |
|---|---|---|---|
| `embedded_graph_3` (26) | **minimal waypoint follower** | `Autosteer`+`Autothrottle` ← `RacingV2Waypoint[0]=Center` of `RacingV2GetWaypoint[0]=Next`; `Spherecast`+`CarRaycasts` for avoidance; stats `Stat[2,10,3]` (speed/turn/health) |
| `embedded_graph_2` (29) | sensor follower | same + `CarInfo`, `GetCarPart[0]`, `RacingV2GetFloat[0]=Speed`, `RacingV2GetCar[0]`, `RacingV2GetBool[0]`, `HitInfo` |
| `embedded_graph_1` (47) | **keyboard demo** | `Keypress`×5 (WASD-style) into a latch controller; how the dev drives a manual car |

`Stat` modifiers are the point VALUES (0..10, 20-pt budget) — graph 3 runs
**speed 2 / turn 10 / health 3**, i.e. the default demo is turn-heavy.

## 1. What the game actually exposes (pylib-proven)

### Stat budget — the only "constants" the game officially supports

- Cosmetics + 3 stats come through **`ConstructRacingV2Properties`** with a
  **20-point budget**: `Stat1 = speed`, `Stat2 = turn`, `Stat3 = health`.
- Every `StatN` node modifier `0..10` seen in real graphs = the point VALUE
  (distributions peak around 5-8 in top graphs).
- So "turn rate" as a tuner knob = **Stat2 points (0..10)**. No one has
  measured what 1 point is in deg/s yet — that is the first RE job (§3).

### Sensors (float channel / index, with community meaning)

`RacingV2GetFloat` (19): `Speed` (0), `Signed Speed` (1), next/prev waypoint
indices (2/3), `Waypoint count` (4), `Current race rank` (5), `Number of
competitors` (6), distances to next/prev waypoint (7/8), `Current lap time`
(9), `Best lap time` (10), `Total race time` (11), `Current lap` (12),
`Total laps` (13), sim/real clocks + `Pi` (14..18).

`RacingV2GetBool` (3): `Is Grounded` (0), `Is Disabled` (1),
`Simulation started` (2).

`RacingV2GetCar` (27): `Self` = **2** (verified in roody.txt: every
`GetCarPart` hangs off `RacingV2GetCar[2]`). `By index` (0) takes a `Float1`
input = car index. Health/rank/nearest selectors: see the pylib table.

`RacingV2GetWaypoint` (4): `Next` (0) / `Previous` (1) / `By index` (2, takes
a `Float1`) / `Start` (3). **Waypoint 0 = the start/finish line** (dev said it).

`RacingV2Waypoint` modes: `Center` (0) / `Left` (1) / `Right` (2) /
`Nearest point` (3) — plus a second `Float1` output = waypoint index.
Optional `Transform1` reference input.

### Car control (ports, unverified order)

- **`ModularCarController(Float1, Float2, Float3)`** — pylib order only.
  Community bots treat throttle/steering/brake as its inputs; which wire is
  which is [unverified].
- **`Autosteer(Vector31) -> Float1`** — steering assist to a target point.
- **`Autothrottle(Vector31, Float1?) -> Float1`** — throttle assist; second
  input is a speed limit [unverified].
- **`GetCarPart(Car1) -> Transform1 + Float1`** — car part transforms
  (modifiers 0/4/5/20/21 seen in real graphs; full part enum unknown).
- **`RelativePosition(Transform1) -> Vector31`**, mode **13 = world** — the
  recipe for "world position of car parts".
- **`Spherecast(Float1=radius, Float2=distance) -> Spherecast1`** feed into
  **`CarRaycasts(Spherecast1) -> RaycastHit1..8`**; **`HitInfo(RaycastHit1) ->
  Bool1 + Float1`** decodes a hit.

### Coordinates

- **Y up** (dev confirmed). Waypoints are colliders along the track center
  spline; "nearest point" = car CENTER to nearest collider point (Unity
  built-in), not car-collider to waypoint-collider.
- Level names in v0.22: `level0..8` (9 tracks incl. clover; waypoints on
  clover were widened to cover car width in the 15.07 update).
- `race_settings.json`: 7 cars default, 3 laps, collisions off, no DQ/DNF
  toggle setting exists (`raceDisableDqDnf`).

## 2. Turn rates and physics — what is NOT known (yet)

| Unknown | Why it matters | How to measure it |
|---|---|---|
| `Stat2` point -> deg/s | every racing AI needs a turn model | park one Stat2 value, log yaw rate from a TimePlot at full steer (compare_timeplots pipeline) |
| speed/turn/health mapping per point | stat-budget optimizer | stat sweeps in TimePlot: Speed sensor vs Stat1 points |
| wheel torque / accel m/s^2 | prediction + line planning | TimePlot `Speed` vs time at Stat1 values, standing start |
| grip / drift (Arcade Drift) | cornering model | yaw-vs-steer curves per Stat2 value |
| ModularCarController port order | currently ASSUMED (throttle/steer/brake -> Float1/2/3) | single-wire probes: constant into one port at a time, observe |
| GetCarPart ids | part ids 0/4/5/20/21 unidentified | editor readout / label probe in-game |
| Tick rate | scaling every constant | measure sim time vs wall time (soccer/tennis used restart sweeps) |

Candidate field holders seen in `global-metadata.dat`:
`RacingVehicleStats`, `WheelSettingsSO`, `SteeringSensitivityHighSpeed`,
`Body Type Config`, `Arcade Drift`, `ControllableWheelAuthoring`,
`PairedWheels`, `UpdateWheelDataJob`.

## 3. Where to record new measurements

- Mining tools: `games/Racing/data/*.py` (node catalog, metadata strings,
  option labels, `graph_dump.py` per-graph trees).
- Graph corpus: `games/Racing/data/graphs/` (top-community logic to imitate).
- Simulator status: none exists — any constants work must be verified
  in-game (TimePlot + replays), never trusted from the compiler alone.
