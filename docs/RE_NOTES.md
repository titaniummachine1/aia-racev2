# RacingV2 — RE notes

**Status 2026-09-22: DATA EXISTS NOW.** First racing data mined — see
`NODE_FINDINGS.md` (node recipe for car-part world position, 53-kind catalog,
coordinate conventions) and `COMMUNITY_INTEL.md` (dev + community knowledge).

## What we have

| Source | Path | Contents |
|---|---|---|
| 14 car graphs | `graphs/*.txt` | real racing AIs incl. top builds (Blockmobile 724 nodes, Atom Returns V1.2 202, roody 63...) |
| Node catalog | `race_node_catalog.json`, `NODE_CATALOG.md` | every node kind + modifier counts |
| Race settings | `race_settings.json` | laps, players, collisions, DQ/DNF, track randomisation |
| Metadata strings | `metadata_race_strings.txt`, `metadata_option_labels.txt` | Racing class/gate names from `global-metadata.dat` |
| Mining tools | `mine_race_graphs.py`, `dump_catalog.py`, `graph_dump.py`, `mine_metadata.py`, `mine_option_labels.py` | rerunnable |

## Still to mine

- Dropdown **index -> label** mapping (not stored in saves; needs editor UI or the
  gate option arrays: `RacingV2GetFloatGate`, `GetCarPartGate`, `RelativePositionGate`, ...)
- Track geometry / waypoint transforms (from `level*` + `sharedassets*`, per
  `aia_comp-sim/docs/RE_PLAYBOOK.md` §8)
- Car physics constants (mass, torque, grip, drag) — not yet found; `RacingVehicleStats`,
  `WheelSettingsSO`, `SteeringSensitivityHighSpeed`, `Arcade Drift` are candidate field holders

## Log

- 2026-09-22: pulled `Saves\RacingV2` (14 graphs + settings); mined node catalog +
  metadata identifiers; confirmed `RacingV2GetCar -> GetCarPart -> RelativePosition`
  gives car-part world position; confirmed Y-up and waypoint 0 = start line.
