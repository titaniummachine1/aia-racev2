# RacingV2 — community / dev intel (Discord log, distilled)

Condensed from the Aialanders Discord threads (2026-05 → 2026-07). People:
AIA = the developer, Adam [CLST], Roody, Zudan, BlockheadedMoronicIdiot,
A Devil [SODA], Blue, Martico2432, SelkirkXVI.

## Sensors / nodes

- **World position of car parts**: `get car (self)` -> `get car part` ->
  `RelativePosition` (see `NODE_FINDINGS.md` §1 for the dumped chain).
- **Y is up.** Start/finish line is **waypoint 0**.
- `RacingV2` spherecast: read clear directions; "make them bigger" is the advice
  for flaky detections.
- **Closest-point semantics**: "closest point" = centre of the **car** to the
  nearest point of the collider, via a built-in Unity function — so it is *not*
  car-collider-to-waypoint-collider distance. Near the right-hand end of a
  waypoint the left/right closest points are not equal to the point you'd expect.
  AIA: "not a bug, slightly different information."

## Memory / logic

- **No delay nodes exist.** Memory cells are the workaround for flickering bools:
  `ConditionalSetFloat` with its output fed back into itself, plus a bool set/clear
  when you want to save state (Zudan; Blockhead's cart = "full digital computer of
  1000+ nodes").
- **Variables (set/get variable) do not count toward the node count** (Adam) —
  good for de-duplicating repeated expressions.
- Spherecast bools flicker; no built-in debounce.

## Bugs (as reported)

| Bug | Notes |
|---|---|
| Throttle stops after resetting **on** a waypoint / after finishing a race | reset the track; AIA "fixed it I think" (targeted 0.16). Workaround: constant throttle |
| Clicking **Go** after the start stops the car and restarts it from that spot | long-standing; just hit **reset** (SelkirkXVI, Blockhead) |
| Can't move/organise nodes after adding one | triggers randomly on add/duplicate (spherecast, string, colour, float, hit info); save+reload the car usually fixes; sometimes needs a reboot; once it crashed and self-reloaded |
| Relay -> Vector3Split passes the **whole vector** instead of the selected float | regression from a connection-type fix; workaround: add `0` to the float before the relay. AIA acknowledged ("ty") 15.07 |
| Graph display is delayed ~0.5 s | intentional caching so the line animates; Blockhead asked for a toggle |

## Feature requests / roadmap (dev's own list, 11.07)

- fix: missing node buttons; editor button not displaying; "last waypoint" ->
  "waypoint count"; waypoints on clover covering car width; hookup timeplot;
  pitch infinite value; setting to disable DQ/DNF
- future: clicking nodes takes priority over connections; "method" nodes; cars
  can still move when paused; snapshot/replay buttons for the simulation
- wanted by users: custom functions (SHIPPED 16.07 — "construct" with up to 4
  labelled inputs + 1 optional output, then a "custom function" node selects it
  via dropdown), lerp node, power node

## Race settings

`race_settings.json` (RacingV2\Settings) has: `raceLaps`, `playerCountLow/High`,
`raceSpawnType`, `raceDisableCarCollisions`, `raceDisableDqDnf`,
`randomizeTrack`/`raceTrackIndices`, `autoSimulate`, `autosimOnStart`,
`showTimer`, `exportResults`, `loadDefaultSaves`, `disableNametags`,
`disableSelectionCircle`, `showTrails`, timeplot window settings.

Default state of our local copy: 7 players, 3 laps, **car collisions disabled**,
DQ/DNF enabled, randomize track off.
