# RacingV2 — node findings (mined from real graphs)

Source: 14 car graphs in `data/graphs/` (Saves\RacingV2\*.txt, pulled 2026-09-22)
plus `global-metadata.dat` from `v0.22`.

## 1. World position of car parts — the recipe

Straight out of `roody.txt`, this is the working chain:

```
RacingV2GetCar[modifier=2]        <- "get car (self)"
      |
      v
GetCarPart[modifier=20 or 21]     <- car part (wheels etc.)
      |
      v
RelativePosition[modifier=13]     -> Vector3 world position
```

Dumped form:

```
RelativePosition[13](Transform1 = GetCarPart[20](Car1 = RacingV2GetCar[2](Float1=?)))
RelativePosition[13](Transform1 = GetCarPart[21](Car1 = RacingV2GetCar[2](Float1=?)))
```

Confirms the community answer (Adam, AIA):
- **`get car (self)`** = `RacingV2GetCar`
- **`get car part`** = `GetCarPart` (a "car data relative transform")
- **`RelativePosition`** = the node that yields the **world** position

Metadata also contains `RelativePositionWithStaticOrigin` (same idea, fixed origin)
and `GetCarFromTransform` (reverse: transform -> car).

## 2. Coordinate system

- **Y is up** (confirmed by AIA in the community log).
- Waypoints: **the start/finish line is waypoint 0** (AIA), so the first corner is 1.
  Note the `"last waypoint"` node label was renamed **"waypoint count"** in the
  dev's own TODO.

## 3. Node vocabulary seen in Racing graphs (53 kinds)

| Node | Uses | Meaning |
|---|---|---|
| `RacingV2GetCar` | 16 | car reference (`modifier 2` = self) |
| `RacingV2GetFloat` | 32 | racing float sensor (modifier = index) |
| `RacingV2GetBool` | 4 | racing bool sensor (modifier = index) |
| `RacingV2Waypoint` | 89 | waypoint selector (modifiers 0-3 seen) |
| `RacingV2GetWaypoint` | 63 | read a waypoint (modifiers 0/1/2 seen) |
| `GetCarPart` | 2 (roody) | car part reference |
| `RelativePosition` | 47 | world position of a transform |
| `HitInfo` / `Spherecast` / `CarRaycasts` | 48 / 3 / 1 | raycast + hit data |
| `Stat` | 39 | stat readout (modifiers 0-10 seen) |
| `ModularCarController` | 1 | controller reference |
| `Autothrottle` / `Autosteer` | 2 | built-in driving helpers |
| `ConstructRacingV2Properties` | 1 | build the car-properties struct |

Value storage differs from tennis: **`Float` nodes keep their value in
`modifier`** (e.g. `"300"`, `"-1"`, `"100000"`), and **dropdown nodes store only an
index** — there is **no `data` object at all** in Racing nodes (tennis had one).
So unlike tennis, the save does NOT carry dropdown label text; labels must be read
from the editor UI or mined from the gate classes
(`RacingV2GetFloatGate`, `RacingV2GetCarGate`, `GetCarPartGate`,
`RelativePositionGate`, `RacingV2WaypointGate`, `RacingV2GetBoolGate`,
`ConstructRacingV2PropertiesGate` — all found in `global-metadata.dat`).

## 4. Tools in this folder

| Script | What |
|---|---|
| `mine_race_graphs.py` | all graphs -> `race_node_catalog.json` (kinds, modifier counts, files) |
| `dump_catalog.py` | catalog -> `NODE_CATALOG.md` (readable tables) + focused Racing API print |
| `mine_metadata.py` | `global-metadata.dat` -> `metadata_race_strings.txt` (Racing identifiers) |
| `mine_option_labels.py` | `global-metadata.dat` -> `metadata_option_labels.txt` (label candidates) |
| `graph_dump.py` | dump one graph: histogram, `--find <NodeKind>`, `--tree` feed trees |

Example:

```
python graph_dump.py graphs/roody.txt --find RelativePosition --depth 4
```

## 5. Open questions

- Map dropdown **indices -> labels** (needs the editor UI, or the gate classes'
  option arrays). Every RacingV2* modifier in the catalog is an index.
- `RacingV2GetCar` has an unused-looking `Float1` input — car index? untested.
- `Stat` modifiers 0-10 unrecovered; 8 and 7 are the most used in top graphs.
