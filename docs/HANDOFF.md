# RACING HANDOFF LEDGER - 2026-09-22 - aia_graphc commit a6afecf

META: sessions may die unexpectedly - APPEND to this file after every task. A prior session
in this repo was pinned in plan mode and had no `editor` tool, so writes were blocked and
knowledge was captured in chat; this file is the recovery of that ledger.

## 1. PATHS / WHERE EVERYTHING IS (nothing was wiped)

| Thing | Path |
|---|---|
| Compiler | `c:\gitProjects\aia_graphc` (master, commit `a6afecf`) |
| Racing QOL lib | `aia_graphc\graphc\api\AIA_Comp_Libry\racing\{v022.py,constants.py,mathx.py}` |
| Racing game | `c:\gitProjects\aia-racev2\versions\v0.22\` and `\v0.22f\` (`fix_display.ps1` = 1920x1080 fullscreen + DPI) |
| Racing mined data | `c:\gitProjects\aia-racev2\data\` - mirror `aialanders-legacy\games\Racing\data\` |
| Racing docs | `aia-racev2\docs\{RE_NOTES.md,HANDOFF.md}`; `aialanders-legacy\games\Racing\data\docs\{NODE_FINDINGS,COMMUNITY_INTEL,RACING_CONSTANTS}.md` |
| Parity sim (mode gate only, no racing eval) | `c:\gitProjects\aia_comp-sim` (commit `4a1a37d`) |
| Tennis modhost = TEMPLATE | `c:\gitProjects\AIA_tennis\modhost\` |
| Legacy library | `c:\gitProjects\aialanders-legacy\games\<game>\<version>\<zip>` - **17 zips** |

Legacy inventory (verified): IndieDev500 2 zips | Racing 2 zips / 38 files |
SlimeVolleyball 2 | Survival 3 | Tennis 6 zips / 119 files | WorldCup 2 zips / 234 files.
`aialanders-legacy\versions\` is EMPTY - it never had content; that is the only thing that
looks missing.

## 2. REPO/BUILD STATE (do not redo)

- Python `10/10` (`_runtests.py` -> `ok=10 fail=0`), Rust `28/28`, `test_racing.py` +
  `test_racing_math.py` green, `examples\demo_racing.{py,json}` compiles.
- Compiler racing support: targets `racing` / `v0.22` + `v0.22f`; ops `race_get` (5 sensor
  kinds), `race_car_part`, `race_waypoint`, `race_drive`, `race_autosteer`,
  `race_autothrottle`; port tables + `out_port`; dropdown index tables +
  `racing_modifier()` resolver; `CompileReport.warnings`; `Parity::None` for racing.
- `Parity::None` -> EVERY racing emit prints a loud `NO PARITY SIMULATOR` warning
  (Rust report, CLI JSON `parity`, Python once-per-process stderr). By design.
- Frontend (`graphc\ast_fe.py`): racing `api.*` calls, racing branch in
  `_targetmod_call`, `race_drive` sink, docstring skip, helper-module hook
  (`_HELPER_MODULES` / `_helper_call` / `_helper_const`) that inlines `mathx` and resolves
  `constants.*` to numerics at compile time.
- Script-author usage:
  `from AIGamePyLibrary.racing.mathx import steer_angle, mph`
  `from AIGameLibrary.racing.constants import DRIVER_THROTTLE, MAX_STEER_DEG`
- `aia_comp-sim` racing = `GameMode::Racing` / `GameVersion::RacingV022`,
  `RACING_EXTRA_NODES`, `Parity::None` + `parity_warning()`, dropdown tables in
  `resolve()`, lowerer emits `UNKNOWN_ID` dead slots (no evaluation, on purpose).
- `gen_api.py` was damaged by editor hand-edits (escaping/BOM) and reverted to HEAD;
  racing stubs come from the separate `gen_racing_api.py`.

## 3. DECOMPILATION - WHAT WAS LEARNED (most important)

### 3a. STATIC DUMPING IS BLOCKED FOR RACING (verified)

```
v0.22   GameAssembly.dll      126,779,392 bytes
v0.22f  GameAssembly.dll      126,779,904 bytes
v0.22   global-metadata.dat    22,931,976 bytes
v0.22f  global-metadata.dat    22,931,688 bytes
v0.22f  metadata first 4 bytes:  D3 94 77 60
```

Standard IL2CPP metadata sanity magic is `0xFAB11BAF` (LE bytes `AF 1B B1 FA`).
Racing = `D3 94 77 60` => **the metadata header is obfuscated/encrypted**. Therefore:

- The bundled `Cpp2IL.exe` (in
  `AIA-Library\WorldCup\v0.5\MelonLoader\Dependencies\Il2CppAssemblyGenerator\Cpp2IL\`)
  will refuse or misparse -> this explains the `Cpp2IL.exe.broken_metadata` artifact seen
  earlier. Plain `Il2CppDumper` fails identically.
- **BUT strings ARE readable** - 270 tooltips, 306 dropdown labels, every node class name
  and the 53-kind node catalog were mined from the same file. The metadata BODY is
  readable; only the header/magic is protected. A `dump.cs` static pass needs the header
  de-obfuscated first (research task, not a 5-minute fix).
- Engine is **Unity 6** (user's own link:
  `docs.unity3d.com/6000.3/Documentation/ScriptReference/Physics.SphereCast.html`) while
  the bundled Cpp2IL is an old MelonLoader dependency -> version support doubtful anyway.

### 3b. WHAT WORKS INSTEAD - the tennis RUNTIME PROBE (reuse this pattern)

Tennis never decompiled statically. `AIA_tennis\modhost\paritymod-src\bootstrap.c` is a
**freestanding launcher EXE (no CRT, `-nostdlib -mwindows -e start`)** that:

1. hooks the `GetProcAddress` pointer inside `UnityPlayer.dll` IAT (walks PE headers,
   `VirtualProtect`s the slot),
2. logs the entire `GameAssembly.dll` export-resolution stream as JSONL
   `{"kind":"resolve","index":N,"symbol":"...","rva":"0x..."}` to
   `probe-startup.jsonl` - THIS is how the `il2cpp_*` API order (`api_indices.h`) is
   learned, with zero guessing,
3. `LoadLibraryW(L"UnityPlayer.dll")` then `UnityMain(GetModuleHandleW(0),0,
   L"-screen-fullscreen 0 -screen-width 1280 -screen-height 720 -logFile probe-player.log",1)`,
4. `metadata_probe.h` (320 KB) resolves everything **by managed NAME** at runtime:
   domain -> images -> classes -> methods/fields, observes `il2cpp_runtime_invoke`, and
   detours a method ONLY when a native driver calls it outside managed paths (guarded by
   **instruction shape**, not RVA - so it ports across versions),
5. `bootstrap.c` is version-agnostic; the ONLY version-coupled artifact is the resolve
   index order in `api_indices.h`, which SELF-VALIDATES (wrong index = loud `metadata_*`
   failure, never a silent bad read).

**Build commands** (verbatim from `build_paritymod_v015.ps1`):

```powershell
$env:Path = "C:\Users\Terminatort8000\AppData\Local\Temp\opencode\w64devkit\bin;" + $env:Path
gcc -O1 -DV014_METADATA -DV014_NATURAL_TRACE -DPARITYMOD `
    -fno-stack-protector -fno-builtin -fno-strict-aliasing `
    -I <src> -c "<src>\bootstrap.c" -o "<out>\paritymod-reset-v015.obj"
gcc -nostdlib -mwindows -e start -o "<out>\Aialanders-paritymod-reset-v015.exe" `
    "<out>\paritymod-reset-v015.obj" -lkernel32 -luser32 -lgcc
```

**Launch / control protocol** (verbatim from `launch_v015.cmd`):

```
set MODHOST_GAMEDIR=<gamedir>
set MODHOST_PARITY_EXE=<probe exe>
python modctl.py launch --home X --away Y --seed N --points P
python modctl.py wait --timeout 900
python modctl.py quit          # auto-exports a TimePlot
```

- Long runs MUST go through a **scheduled task** - the agent shell kills child processes.
- A `user32` window subclass exists so an INTERACTIVE window close still flushes
  timeplots (the mod `quit` command only covers scripted closes).
- Port notes: KEEP `bootstrap.c` + `metadata_probe.h`; DROP tennis fixtures
  (`curve_fixtures_v014.h`, `serve_fixtures_v014.h`, `shot_fixtures_v014.h`,
  `getter_items_v014.h`); REGENERATE `api_indices.h` from racing own resolve stream.
- Racing probe scope (resolve by name first): `AutosteerGate`, `AutoThrottleGate`,
  `RacingVehicleStats`, `ControllableWheelAuthoring`, `WheelSettingsSO`,
  `GetCarPartGate`, `RelativePositionGate`, `RacingV2GetFloatGate`,
  `RacingV2WaypointGate`. The obfuscated metadata header costs us NOTHING on this path.

## 4. EDITOR-CONFIRMED NODE ABI (live Inspector tooltips = ground truth)

- **Car Info** (`CarInfo`) - in: `car`. out: `transform`, `vector3` = **velocity**,
  `bool` = is AI-authored (includes LLM-created and ML solutions), `bool` = is immobile,
  `float` = health (total of all component parts), `float` = **rank**.
  (Tooltip body repeats "total health" for the last two - PIN LABELS WIN: last is rank.)
- **Auto Throttle** (`Autothrottle`) - "Controls an automobile throttle. It will attempt
  to avoid obstacles." in: `vector3` = target location, `float` = target speed ("desired
  speed to be at when reaching the target location"). out: `float` = resulting throttle.
  **No obstacle pins on the node -> avoidance is INTERNAL** (backs `driver_config.json`).
- **Controller** (`ModularCarController`) - `float` throttle (**1 = forward, -1 = reverse**),
  `float` steering (**-1 = left, 1 = right**), `float` brake (**any value over 0 applies brake**).
- **Properties** (`ConstructRacingV2Properties`) - `string` name, `country`, `color` skin,
  `float` body style (**0 = male, 1 = female, value WRAPS to prevent errors**),
  `float` hair style (wraps), `color` hair, `float` facial hair (wraps), `color` car color,
  `string` custom texture URL (outfit UV template), `stat` **max speed**, `stat` **turning**,
  `stat` **health**. Editor shows an available-points budget and 0..10 steppers.
- **Stat** - "A stat value represents a number value in stat form. This is meant to be
  constant per simulation." out: `stat` (resulting stat value), 10-segment stepper UI.
- **Waypoint** (`RacingV2Waypoint`) - "Process information about a specific waypoint."
  in: `waypoint`, `transform` (**optional - "only used in some dropdown options"**).
  out: `vector3` = world position of the processed waypoint, `float` = **index of the
  input waypoint**. Dropdown labels are per-track waypoint names (Left/Right/Center/
  Nearest point).
- **Spherecast** - "like a laser with a thickness that checks whether an object would hit
  anything in front of it." in: `float` radius, `float` max distance from the spherecast
  origin. out: `sphercast`. Thin wrapper over Unity
  `Physics.SphereCast(origin, radius, direction, out hit, maxDistance,
  layerMask = DefaultRaycastLayers, QueryTriggerInteraction = UseGlobal)`: `hit.distance`
  is measured from origin; **colliders overlapped at the start are NOT detected** (that is
  `OverlapSphere`); layer 2 "Ignore Raycast" excluded by default; trigger hits follow
  `Physics.queriesHitTriggers`. `[guess]` the node casts from the owner car transform
  along its forward.
- **Hit Info** (`HitInfo`) - "Processes a hit from a raycast and separates out some of its
  results." in: `raycast hit`. out: `bool` = was a collision detected?,
  `float` = distance of the collision (**returns infinity if no collision**),
  `string` = **tag of the hit collider GameObject**.
- Other kinds in editor / mined graphs (53 total in `data\race_node_catalog.json`):
  `Keypress`x5, `Relay`x3, `Region`x4, `Conditional Set Float V2`x5,
  `Conditional Set Vector3`, `Add Floats`, `Compare Floats` (`<`), `Distance`, `NOT`,
  `RandomColor`, `Debug`, `CarRaycasts`, `Float`, `String`, `Bool`, `Color`, `Country`,
  `GetCarPart`, `RacingV2GetFloat/GetBool/GetCar/GetWaypoint`.

## 5. CERTAIN CONSTANTS (shipped TextAsset `driver_config.json`, extracted 2026-09-22)

| key | value | meaning |
|---|---|---|
| `throttle` | **0.9** | demo driver fixed throttle |
| `brake` | **0.0** | demo never brakes |
| `steerSensitivity` | **1.0** | input -> steer scale |
| `spherecastRadius` | **0.75** | obstacle spherecast radius |
| `spherecastDistance` | **12.0** | obstacle spherecast length |
| `avoidNearDistance` | **4.0** | start steering away at this range |
| `avoidSteerStrength` | **1.25** | avoidance steering gain |
| `obstacleAvoidance` | `true` | on by default |

Mirrored in `constants.py` as `DRIVER_*` with `CERTAINTY = "certain"`, asserted in
`graphc\tests\test_racing_math.py`.

## 6. CERTAIN - 3 DEV-AUTHORED REFERENCE GRAPHS (from `sharedassets0.assets`)

Extractor: `aia-racev2\data\extract_prize_textassets.py` -> `embedded_graph_{1,2,3}.txt`
(mirrored to `aialanders-legacy\games\Racing\data\graphs\`).

| graph | nodes | what it is | wiring |
|---|---|---|---|
| `_3` | 26 | **MINIMAL WAYPOINT FOLLOWER (best template)** | `Autosteer`+`Autothrottle` fed by `RacingV2Waypoint[0]=Center` of `RacingV2GetWaypoint[0]=Next`; `Spherecast`+`CarRaycasts`; `Stat[2,10,3]` |
| `_2` | 29 | sensor follower | above + `CarInfo`, `GetCarPart[0]`, `RacingV2GetFloat[0]`, `RacingV2GetCar[0]`, `RacingV2GetBool[0]`, `HitInfo` |
| `_1` | 47 | **keyboard demo** | `Keypress`x5 + `ConditionalSetFloatV2` latches -> how the dev drives manually |

`Stat` modifiers in saves are the POINT VALUES (0..10 each, 20-point budget); graph `_3`
runs **speed 2 / turn 10 / health 3** (turn-heavy default).

## 7. DROPDOWN ENCODING (verified against 14 real saves)

- Saves store the **dropdown INDEX** in the node `modifier` field - the OPPOSITE of tennis
  (tennis stores a label). `RacingV2GetCar[2]` = `Self`; `RelativePosition[13]` = world.
- Car-part world position recipe: `RacingV2GetCar -> GetCarPart -> RelativePosition(13)`.
- Tables: `aia-racev2\data\race_node_catalog.json` + `metadata_dropdowns2.txt`; resolver
  `aia_graphc\src\lib.rs` (`racing_modifier()`), mirrored in
  `aia_comp-sim\src\graph\dropdowns.rs`.
- Resolved labels: `RacingV2GetFloat` 19/19, `RacingV2GetBool` 3/3, `RacingV2GetCar` 27/27,
  `RacingV2GetWaypoint` 4/4, `RacingV2Waypoint` 4/4.
- **STILL UNMAPPED:** `GetCarPart` ids **4, 5, 20, 21** (README documents only 0..3 =
  average of all parts / nearest part / weakest part / nearest crucial part) and the
  non-13 `RelativePosition` modes seen in real saves (0,1,2,3,4,9,10,11).

## 8. WHAT IS STILL UNKNOWN (the real remaining work)

1. **Physics values** (all `[unknown]` in `constants.py`): max steer angle, sensitivity
   lerp params, slew rate, yaw decay, wheelbase, motor torque cap, `Stat`->deg/s (turning),
   `Stat`->m/s (max speed), brake decel, tick rate. Tooltips give the model SHAPE only.
2. **Sensor semantics**: lap wrap at waypoint 0, rank after DNF, signed-speed at rest,
   health% meaning, what "Nearest point" projects onto.
3. **Enum gaps**: `GetCarPart` 4/5/20/21, non-13 `RelativePosition` modes.
4. **Autothrottle / Autosteer internals** (they own obstacle avoidance; tooltips imply a
   Hybrid A* planner: cell size, heading bins, step meters, max steer, steering samples).
5. **Tick rate / sim cadence** for any offline prediction.

## 9. WHAT I NEED FROM THE USER (racing-specific; racing != tennis/soccer)

Tennis/soccer could be driven from a static field with spawned entities. Racing needs a
SAVE (car graphs), a TRACK/map, a RACE SESSION (countdown -> sim), and a graceful END to
flush timeplots.

1. **Act mode that actually takes effect** - one session was pinned in plan mode with
   every write rejected (New-Item / Set-Content / `>` all blocked). Needed just to create
   `aia-racev2\modhost\`.
2. **Load a map/track once and walk me through the flow**: launch -> load track -> open
   graph editor / pick saved cars -> start race -> what "race over" looks like (and
   whether Esc/quit exports the TimePlot). Racing load order is the part I cannot guess.
3. **Load scripts + cars + map** (user offered): a loaded TRACK so waypoint indices exist;
   a save with **1 probe car** (constant steer / constant throttle - I write the JSON, user
   drops it into `Saves\RacingV2` or loads it in-editor); a second save with **2 cars**
   (one AI-driven) to read rank / relative-position sensors.
4. **Pick the target build**: `v0.22` or `v0.22f` (assume `v0.22f`, the `fix_display.ps1`
   1920x1080 fullscreen copy).
5. **Windows Defender exclusion** for the modhost folder/exe (unsigned, patches an IAT in
   memory; tennis needed one). Confirm whether `AIA_tennis\modhost` already has one, then
   mirror it for `aia-racev2\modhost`.
6. **TimePlot confirmation**: exact knob names (`timeplotVisible`, `exportResults`) and
   output path in racing, so capture runs can be judged automatically.
7. **Scheduled task** for long runs (the agent shell kills child processes) - user
   triggers, exact commands handed over by me.

## 10. NEXT STEPS, ORDERED

1. `aia-racev2\modhost\` skeleton: copy tennis `paritymod-src`, keep `bootstrap.c` +
   `metadata_probe.h`, split out a racing metadata scope that FIRST dumps only the resolve
   stream (`probe-startup.jsonl`) -> gives racing `api_indices.h` with zero guessing.
2. Racing type/method/field **inventory by name** (single capture, no behavior): the
   Autosteer / Autothrottle / Stats / Wheel / RacingV2* gates -> dump every field
   name+offset -> `race_abi.json` + `RACEV2_ROADMAP.md`.
3. Capture matrices: **A** steer (input x speed x Stat2 -> wheel angle + yaw/dot products),
   **B** engine (throttle/brake -> accel / top speed / decel per Stat1), **C** field reads
   (incl. tick rate), using the user probe car on the loaded track.
4. Feed results into `constants.py` / `mathx.py`, **flip `CERTAINTY` flags** with
   capture-row evidence, update `RACING_CONSTANTS.md`.
5. Only then consider a parity sim (otherwise racing keeps `Parity::None` forever).
6. Cleanup backlog: scratch files (`_race_meta_sentences.txt` at `c:\gitProjects`,
   `runtests3.log` already deleted, unused `mine_dropdowns2.py` /
   `mine_label_neighborhoods.py` outputs, `metadata_label_neighborhoods.txt` is 0 bytes);
   decide on duplicate `c:\gitProjects\games\` vs `aialanders-legacy\games\`, the empty
   `aialanders-legacy\versions\`, and `AIA-Library\`.

## 11. QUIRKS / GOTCHAS LEARNED

- Racing dropdown = INDEX in save `modifier`; tennis = LABEL. Never reuse tennis decoding.
- `RelativePosition(13)` is the world-space one; car-part world pos needs the full chain
  `RacingV2GetCar -> GetCarPart -> RelativePosition(13)`.
- `Stat` budget is 20 points, each stat 0..10; unfilled budget shows "available points 0"
  in the editor when nothing is wired.
- All AIComp games share one registry key `HKCU\Software\Unicorn One\Aialanders`; the
  display fix (`fix_display.ps1`) sets 1920x1080 fullscreen + HIGHDPIAWARE.
- `metadata_label_neighborhoods.txt` came out 0 bytes (that miner produced nothing).
- Racing has NO parity simulator: every compile warns loudly; do not silently trust
  sensor semantics or port order without a capture.

## 12. SESSION LOG (append after every task)

### 2026-09-22 (act-mode session)

- Wrote this ledger (recovered from a session pinned in plan mode where every write was
  rejected by the harness guard; knowledge had only lived in chat).
- Created `c:\gitProjects\aia-racev2\modhost\` (the tennis-probe port):
  `paritymod-src\bootstrap.c` (7582 B, copied verbatim - version-agnostic),
  `paritymod-src\api_indices.h` (10340 B - **STILL TENNIS ORDER, must be regenerated
  from a racing capture before any metadata build**),
  `paritymod-src\metadata_probe.h` (320395 B),
  `build_probe.ps1`, `README.md`.
- Toolchain verified present:
  `C:\Users\Terminatort8000\AppData\Local\Temp\opencode\w64devkit\bin\{gcc.exe,ld.exe}`.
- **BUILT STAGE 1 PROBE (success):**
  `modhost\paritymod\Aialanders-racev2-probe.exe` = **12,196 bytes**, freestanding
  (no CRT, `-nostdlib -mwindows -e start`, linked `-lkernel32 -luser32 -lgcc`).
  Build command: `powershell -NoProfile -ExecutionPolicy Bypass -File build_probe.ps1`.
  Stage 1 has NO `-DV014_METADATA`: it only captures the resolve stream
  (`probe-startup.jsonl`) - which is exactly the first thing racing needs.
- Copied the exe into `versions\v0.22\` and `versions\v0.22f\` (it must sit beside
  `UnityPlayer.dll`; it loads UnityPlayer itself and calls `UnityMain`).

**NEXT ACTION (user):** run `Aialanders-racev2-probe.exe` from the game directory once,
reach the main menu, close the game, then hand over `probe-startup.jsonl`. Its
`{"kind":"resolve","index":..,"symbol":..}` rows give racing il2cpp export order ->
regenerate `api_indices.h` -> stage 2 metadata build (field reads + capture matrices).

### 2026-09-22 - probe smoke test PASSED (validated by running)

Ran the stage-1 exe in an empty temp dir (no `UnityPlayer.dll`) via
`Start-Process -Wait` (NOTE: a GUI-subsystem exe does NOT block PowerShell `&`;
`$LASTEXITCODE` stays empty - always use `Start-Process -PassThru -Wait`):

```
exit code: 92                      # expected: ExitProcess(92) = UnityPlayer load failed
probe-startup.jsonl (86 bytes):
  {"kind":"bootstrap","version":2,"scope":"racev2-api-resolution"}
  {"kind":"load_failed"}
```

So the launcher, the logger, the per-row flush and the exit paths all work before
it ever touches the game. Scope string rebranded to `racev2-api-resolution` and the
exe rebuilt + redeployed to `versions\v0.22\` and `versions\v0.22f\`.

### 2026-09-22 - TARGET DECIDED: v0.22 (not v0.22f)

User: "F means free" - `v0.22f` is the FREE variant: identical gameplay code but a
limited/changed UI and some game parts harder to access. Always target
`versions\v0.22\` for captures and decomp work. `fix_display.ps1` and the registry
display fix still apply to either.

Admin guidance (settled): the probe does NOT need elevation - it only VirtualProtects
its OWN process IAT (no global hooks, no drivers). Admin is needed ONLY if Windows
Defender is alive and quarantines the exe; then relaunch the shell as admin (or any
admin PowerShell) to `Add-MpPreference -ExclusionPath c:\gitProjects\aia-racev2\versions\v0.22`
and restore the file. If SmartScreen only warns: right-click exe -> Properties ->
Unblock, or More info -> Run anyway. User believes Defender is absent (may have come
back via a Windows update) - check with Get-Service WinDefend / Get-MpComputerStatus.

### 2026-09-22 - environment settled: admin shell, NO Windows Defender

Verified: shell is elevated (IsInRole Administrator = True), `WinDefend` service is
ABSENT and the Defender PowerShell module (`Get-MpComputerStatus`) does not exist ->
no real-time AV to block the unsigned IAT-patching probe. The exe also has no
`Zone.Identifier` ADS (not Mark-of-the-Web) -> nothing to Unblock. Exclusion step is
CANCELLED as unnecessary. Probe launched from `versions\v0.22\` (the free-v0.22f is
only a UI-limited variant; gameplay code identical).

### 2026-09-22 - capture COMPLETE + api map validated by count + tooltip physics gold

- probe-startup.jsonl final: 237 rows = bootstrap, resolver_observed, enter_unity + **234 resolve rows**
  (indices 0x00..0xe9). Export names are OBFUSCATED (BEzeHmdXHBX, GakYfV_FNRk...) - same
  protection as the metadata header. Saved: `aia-racev2\data\racev2_api_order.csv`.
- **api_indices.h ALREADY FITS: it has exactly 234 #defines and the capture has exactly 234
  resolves** (canonical il2cpp-api-functions.h order: 0=il2cpp_init, 1=il2cpp_init_utf16, ...,
  233=il2cpp_unity_set_android_network_up_state_func). Candidate map valid; runtime validation
  per tennis design (loud metadata_* failure on mismatch).
- Engine exact version from Player.log: **Unity 6000.4.3f1 (39d1a88d4dd1)**. Player.log line:
  `Opening save directory: C:\Users\Terminatort8000\AppData\LocalLow\Unicorn One\AIComp\Saves\RacingV2`.
- **Timeplot export format decoded** (from the 3 exported files):
  `{ "simTime": 44.02, "series": [ ... ] }` -> saved as
  `Saves\RacingV2\Timeplots\timeplot_<date>.json`. Exports SUCCEEDED but series arrays were
  EMPTY (nothing plotted - no debug outputs wired / timeplotVisible false).
- **race_settings.json schema learned** (Saves\RacingV2\Settings): timeplotBackgroundOpacity,
  timeplotGraphOpacity, timeplotVisible, timeplotPosX/Y/Width/Height, timeplotIsFullscreen,
  timeplotXAxisMode, showTrails, **autoSimulate**, randomizePlayers, randomizeTrack,
  **raceTrackIndices: "-1"**, **exportResults: true**, **loadDefaultSaves**, **autosimOnStart**,
  disableNametags, disableSelectionCircle, showTimer, **playerCountLow/High: 7**, **raceLaps: 3**,
  raceSpawnType: 0, raceDisableCarCollisions: true, raceDisableDqDnf: false.
  => full race session is SCRIPTABLE by editing this JSON + launching the probe exe.
- **Real save library found** (14 graphs): Saves\RacingV2\*.txt (The Atom Returns x3,
  v2.2 Blockmobile 3MB, V3.14 Derpley, roody, MattCole, 22NODES, AwsmeAnthonyV2.0, ...).
  AIA.txt (91914 B) == embedded_graph_3 size and controller.txt (153448 B) == embedded_graph_1
  size => the embedded TextAssets ARE the default saves.
- **Dropdown evidence mined from the 14 real saves** (index counts):
  GetCarPart: 0(x3), 4, 5, 20, 21  |  RelativePosition: 0(x11), 1(x6), 2(x4), 3(x6), 4(x6),
  9(x2), 10(x2), 11(x2), 13(x5=world)  |  RacingV2GetCar: 2(x16=Self)  |
  RacingV2Waypoint: 0(x38=Center), 1(x25=Left), 2(x22=Right), 3(x4=Nearest point).
- **Physics-model shape from metadata tooltips (upgrade candidates for constants.py):**
  wheel array order = FL, FR, RL, RR; axle pairs = (left,right); FWD by default
  ("Enable 4-wheel drive. When false, only front wheels get motor torque"); brake = front/rear
  torque split with rear bias => rear lock => oversteer ("tail can step out"); engine angular
  velocity derived from wheel angular velocities ("Maximum is recommended");
  "Absolute cap on wheel motor torque"; wheel width + axis length in meters;
  stall detect = "All wheels inside must hold for this long (seconds)";
  ML hint: "Big reward when car successfully parks (all wheels in, slow, aligned)";
  damage: body pitch/roll per lost corner FL/FR/RL/RR; CarTypeSO sets stats + body/wheel visuals.

### 2026-09-22 - STAGE 2 PROBE WORKS ON RACING - full IL2CPP inventory captured

Built `Aialanders-racev2-meta.exe` (70,005 B, tennis exact flags:
-DV014_METADATA -DV014_NATURAL_TRACE -DPARITYMOD + 4 tennis fixture headers copied in as
build deps). Edited `metadata_interesting_class()` in metadata_probe.h to select racing
classes (CarInfo/GetCarPart/RelativePosition/Spherecast/HitInfo/CarRaycasts/Autosteer/
Autothrottle/ModularCarController/ConstructRacingV2Properties/Stat/Keypress/Region +
prefixes Racing/Race/Car/Wheel/Vehicle/Waypoint/Get/Set). Run rows (probe-startup.jsonl,
stage1 archived as probe-startup_stage1.jsonl):

```
bootstrap/resolver_observed/enter_unity + resolve 234  (API map VALIDATED)
parity_config 1, metadata_begin 1, metadata_module 1, method_info_layout 1
metadata_image 132 | class_catalog 1,563 | metadata_class 151
metadata_field 1,326 | metadata_method 2,394 | metadata_candidate 2 | metadata_complete 1
```

Saved: `data\racev2_class_catalog.txt` (1,563 classes), `data\racev2_meta_rows.jsonl`
(3,871 class/field/method rows), `data\racev2_api_order.csv` (234).

**Dropdown solution found in fields:** `GetCarPartGate` has
`SpecialModeCount : Int32`, `dropdown : TMP_Dropdown`,
**`_dropdownLabels : System.String[]`** (index->label),
**`_partTypesByIndex : DamageableVehiclePart.PartType[]`** (index->part enum),
`_worldPosProxy : Transform`, `transformOutPort`, `floatOutPort`,
static `CompiledInputs/CompiledOutputs : String[]`.
So GetCarPart modes 0..SpecialModeCount-1 = special (README: 0 avg / 1 nearest /
2 weakest / 3 nearest-crucial) and the rest = PART TYPES; saves use 0,4,5,20,21.
`PartType` is a standalone ENUM class in the catalog (+`DamageableVehiclePart`,
`DamageableArcadeVehicle`, `WheelSettingsSO`, `WheelFrictionData`, `WheelMount`,
`GetBodyPart`, `BodyTypeConfig`). `RelativePosition` fields: dropdown, debugLogs,
debugLogInterval, _nextDebugLogTime, outputPort, transformInputPort (NO label array).

**VehicleData (runtime stats object) = Stat mapping readable directly:**
`velocity : float3 @0x10`, `health : float @0x1c`, `isEliminated : bool @0x20`,
**`maxSpeed : float @0x24`, `acceleration : float @0x28`, `steerSpeed : float @0x2c`**.
-> matrices A/B = read 3 floats per Stat allocation. `WheelSet`: frontLeft/frontRight/
rearLeft/rearRight : Transform (FL,FR,RL,RR). Catalog also has `VehicleMovementJob`
(DETERMINISTIC Burst job => parity sim feasible), `VehiclePlannerService` (autosteer
planner), `ModularVehicleReplay*` (replay codec/recorder/state), `EcsVehicleRuntimeState`,
`FleetVehicle(System)`, `CarSpawner`, `RaceGameManager`, `RacingGamemode`, `RaceResult`,
`CarParking*` (parking gamemode w/ exporter+scoreboard), `SettingDisableCarCollisions`,
`SettingDisableDqDnf`, `SettingShowTimer`, `GraphCompiler`, `GraphOpCode`,
`CompiledInstruction`, `GraphValue(Kind)`, `GraphLine`, `CarCullData`, `VehicleSpatialPartition`.

STAGE 3 (next): widen filter (PartType/Damageable*/Wheel*/Body* missing from detailed
dump - PartType enum constants = field names of the enum class) and/or instantiate
GetCarPartGate (il2cpp_object_new + ctor) to READ `_dropdownLabels`+`SpecialModeCount`
at runtime -> final index->label map. Fast alternative: user screenshots the 2 open
dropdowns (GetCarPart, RelativePosition).

### 2026-09-22 - STAGE 3a: PartType enum + PlanOptions + damage model captured

Rebuilt meta exe (70,517 B) with widened filter (Part/Damageable/Body/Hybrid/Plan/Axle/
Engine/Aia/Demo/Ecs/Fleet/Modular/Replay/Nav prefixes). Capture: 8,078 rows / 2.2 MB
(probe-startup.jsonl; stage2 archived as probe-startup_stage2.jsonl): 227 metadata_class,
2,372 metadata_field, 3,540 metadata_method, 1,563 class_catalog, metadata_complete.

**PartType enum (DamageableVehiclePart.PartType) - 33 constants in DECLARATION ORDER:**
WheelFL, WheelFR, WheelRL, WheelRR, AxleFL, AxleFR, AxleRL, AxleRR, Engine, Driveshaft,
SuspensionFL, SuspensionFR, SuspensionRL, SuspensionRR, Hood, Trunk, BumperFront, BumperRear,
LicensePlateFront, LicensePlateRear, FenderFL, FenderFR, DoorL, DoorR, DoorRL, DoorRR,
TurnSignalFL, TurnSignalFR, HeadlightL, HeadlightR, TaillightL, TaillightR, WindshieldWipers.
GetCarPart saves use indices 0,4,5,20,21; specials are modes 0..SpecialModeCount-1
(README: 0 avg-all / 1 nearest / 2 weakest / 3 nearest-crucial), then parts in the order
above (e.g. if SpecialModeCount==4: 4=WheelFL, 5=WheelFR, 20=BumperFront, 21=BumperRear).
TO CONFIRM: read SpecialModeCount + _partTypesByIndex + _dropdownLabels VALUES at runtime
(stage-3b: il2cpp_object_new(GetCarPartGate) + invoke BuildItemsAndPopulateDropdown / .ctor).

**HybridAStarTypes.PlanOptions fields (autosteer planner knobs):**
cellSizeMeters@0x10, headingBins@0x14, stepMeters@0x18, **wheelbaseMeters@0x1c**,
**maxSteerDegrees@0x20**, steeringSamples@0x24, reversePenalty@0x28, steerChangePenalty@0x2c,
collisionSamplesPerStep@0x30, maxExpansions@0x34, maxRuntimeMs@0x38,
goalPosToleranceMeters@0x3c, goalYawToleranceDegrees@0x40, recordExpansionTimes@0x44,
recordExpansionTree@0x45, maxExpansionTreeEdges@0x48.
Read their VALUES via VehiclePlannerService.Instance (static singleton) -> hybridOptions @0x48.

**DamageableVehiclePart (damage model):** partType@0x20, aggregateChildColliders@0x24,
maxHealth@0x28, _legacyDamageSpeedThreshold@0x2c, _legacyDamagePerSpeedUnit@0x30,
_legacyMaxDamagePerHit@0x34, useDeformation@0x38, deformedThreshold@0x3c,
deformationSpring@0x40, deformationDamper@0x44; statics: LayerCacheUnset,
s_drivablePhysicsLayerCache, s_carbodyPhysicsLayerCache, s_defaultPhysicsLayerCache,
s_transparentFxLayerCache (LAYER NUMBERS - needed for Spherecast layerMask semantics).

**GetCarPartGate methods:** Initialize, CacheOutputPorts, OnDropdownChanged, ClampIndex,
BuildItemsAndPopulateDropdown, Solve, Compute, CompiledSolve, CollectPartsForQuery,
GetCarFromInput, get_CompiledInputPortIds, get_CompiledOutputPortIds.
**GetCarPartGate fields:** SpecialModeCount:Int32, dropdown:TMP_Dropdown, transformOutPort,
floatOutPort, _worldPosProxy:Transform, **_dropdownLabels:String[]**,
**_partTypesByIndex:DamageableVehiclePart.PartType[]**, static CompiledInputs/Outputs:String[].
RelativePosition fields: dropdown, debugLogs, debugLogInterval, _nextDebugLogTime,
outputPort, transformInputPort (NO label array - labels set in code/UI).
Other new classes: ModularCarGate, ModularCarInfoGate, BodyTypeConfigSO, AialanderCustomization(+Array),
AialanderState(+Array), AialanderRecorder, ModularVehicleReplay{Codec,Recorder,State,StateArray},
HybridAStarDisplay{Controller,DataOnTexture,Grid,OldCarPositions,ReedsShepp,ShortestPath},
HybridAStarDubinsPath, HybridAStarGridMap, HybridAStarGridDebugTextureMode.

VehiclePlannerService (static singleton <Instance>k__BackingField):
preferHybridAStar@0x20, hybridGridPaddingMeters@0x24, hybridGridMinSizeMeters:Vector2@0x28,
hybridGridMaxSizeMeters:Vector2@0x30, hybridGridY@0x38, hybridObstacleLayers:LayerMask@0x3c,
hybridRasterBoxHalfExtentMeters@0x40, hybridRasterYHalfExtentMeters@0x44,
hybridOptions:PlanOptions@0x48, maxCacheAgeSeconds@0xcc, goalMoveInvalidateMeters@0xd0,
startMoveInvalidateMeters@0xd4, goalHeadingInvalidateDegrees@0xd8.
VehicleData (FleetVehicleSystem.VehicleData): velocity:float3@0x10, health@0x1c,
isEliminated@0x20, **maxSpeed@0x24, acceleration@0x28, steerSpeed@0x2c** (Stat mapping!).
WheelSet: frontLeft/frontRight/rearLeft/rearRight:Transform (FL,FR,RL,RR).
WheelSettingsSO: forward:WheelFrictionData@0x18, sideways:WheelFrictionData@0x2c.
WheelFrictionData: stiffness@0x10, extremumSlip@0x14, extremumValue@0x18, asymptoteSlip@0x1c,
asymptoteValue@0x20 (= Unity WheelFrictionCurve shape).
VehicleMovementJob (deterministic Burst job): deltaTime@0x10, vehicleData:NativeArray@0x18,
flowField, gridOrigin, gridCellSize@0x44, gridSize, spatialMap, positions, spatialCellSize@0x70,
collisionRadius@0x74, searchRadius@0x78.

STAGE 3b (next capture, one small C addition): runtime VALUE reads:
1. il2cpp_object_new(GetCarPartGate)+BuildItemsAndPopulateDropdown -> _dropdownLabels +
   SpecialModeCount + _partTypesByIndex (final dropdown map).
2. VehiclePlannerService.Instance -> hybridOptions values (wheelbaseMeters, maxSteerDegrees = the
   two big unknowns) + layerMask.
3. DamageableVehiclePart static layer caches -> Spherecast layer semantics.

### 2026-09-22 - GRAPH VM + NODE HINTS + PLAYBOOK (user asks closed)

- **Graph interpreter captured** (`CompiledGraphPlan`): instruction =
  `{Op:GraphOpCode, In0..2, Out0..2, Imm}` register bytecode; 37 opcodes
  (ReadPort, Copy, float ops, bool ops, compares, vector3 ops, RelativePosition,
  ConditionalPick, Read/WriteVariable, CallGate, CallHandler, CallFunction);
  `GraphValue{Kind:None|Bool|Float|Vector3|Ref,...}`; interpreter = Execute /
  ExecuteInstructions / ExecuteFunctionCall / EvaluateOperation /
  **EvaluateRelativePosition** (native handler for op 32 = parity-probe target);
  `GraphCompiler.{Compile, CarryOverRegisters}`. Documented in `docs\GRAPH_VM.md`
  + `data\race_abi.json`. Parity path: construct CompiledInstruction[] and run
  `Execute` in the live game (fixture pattern) instead of rebuilding the VM.
- **All node hints grabbed and added to the compiler**: UnityPy scan grouped
  MonoBehaviours by MonoScript -> `NodeTypeDataSO` x65 = `[id,title,class,desc,
  (portId,portDesc)*]` per node -> `race_node_hints.json` + package copy
  `AIA_Comp_Libry\racing\node_hints.json`. Bonus: `NodeConfigurationSO` x2 =
  port-type grammar (String/Float/Bool/Vector3/RaycastHit/Spherecast/Stat/Properties/
  Color/Country/Transform/SurvivalState/SurvivalEmote/Any/Car/Waypoint) + the game-mode
  doc (front wheels = steering; rear wheels/engine/driveshaft = acceleration;
  engine 0 = AoE explosion). `ColorData` x17, `Country` x176,
  `RaceTrackDefinition` x7 (Beach Coil, Clover, Criss Cross, Figure Eight,
  Snetterton, Standard, Training).
- **Ground-truth dropdown maps generated from live capture** ->
  `AIA_Comp_Libry\racing\dropmap.py` (GetCarPartGate 37, GetFloat 19, GetCar 27,
  GetBool 3, GetWaypoint 4, Waypoint 4 + PART_TYPES 33 + GRAPH_OPCODES 37 +
  VALUE_KINDS 5) + `graphc\tests\test_racing_hints.py` (asserts all of it).
  FULL SUITE: `ok=11 fail=0`.
- **Decomp playbook written**: `aia-racev2\docs\RE_PLAYBOOK.md` (symptoms of
  obfuscated metadata, runtime probe method, 234=234 portable API map, build/launch
  gotchas incl. `<exename>_Data` junction + GUI-exe Start-Process rule, parity_cmd.json
  command channel, extras value-capture patterns, UnityPy MonoScript-grouping trick,
  save encoding gotchas) + appended to legacy `WorldCup\data\docs\RE_PLAYBOOK.md`.
- Consolidated ABI: `data\race_abi.json` (58 KB).
- STILL OPEN: `VehiclePlannerService.Instance` was NULL at menu -> planOptions values
  (wheelbaseMeters, maxSteerDegrees) need capture DURING a race (send `extras` while
  racing, or `autosimOnStart:true` + re-send); RelativePosition mode labels (13=world
  known; behavioral probing via EvaluateRelativePosition / GetRelative calls next);
  Rust-side dropdown table sync (`lib.rs` racing_modifier) to dropmap ground truth;
  matrices A/B/C (stat->speed/accel/steer) via VehicleData field reads while racing.


### 2026-09-22 - COMPILER VERSIONING MODEL (user directive) + HARD GAME GATE

- Model: DEFAULT compiler = graph interpreter of the LATEST game = tennis v15f
-  (family ceiling; new titles ship this or slightly better). General VM code
-  models tennis. Every older (game,version) = COMPATIBILITY PROFILE tuned to
-  behave like that game interpreter. NOTHING universal - gates differ per
-  game. game -> version -> that IS the compiler version. Gamemode/map gating
-  later.

-  graphc/profiles.py = the one registry: BASELINE, PROFILES (roles, families,
-  vm_isa capture status, dropdown_encoding label-vs-index, GATE_QUIRKS per
-  game), node_family/check_node + op_family/check_op + GatedOps list class.
- Wired: ast_fe._Ctx + desc.GraphCtx ops lists are now GatedOps - every
-  ctx.ops.append validates against the active (game,version). Tennis-only
-  nodes (TennisGet*/Stamina/Serve/Shot/Swing prefixes) fail loudly under
-  racing and vice versa; universal targets admit only common VM ops.
- Tests: graphc/tests/test_profiles.py; FULL SUITE ok=12 fail=0.
- Docs: aia_graphc/docs/DECOMP_PLAYBOOK.md - Compiler versioning model + VM
-  capture checklist per game (order: tennis v15f BASELINE first, then v0.14,
-  soccer v0.12; racing v0.22 done).
- Tooling note: run_commands payloads over ~8KB with quote-heavy content hit a
-  raw fallback and PowerShell fails to parse - write files in small chunks
-  (single-quoted one-line strings + Set-Content/Add-Content) then concatenate.
- Commit: aia_graphc 824236b (profiles.py + gate + tests 12/12).


- NOTE for future sessions: aia_graphc\docs\AHA_MOMENTS.md now holds every
-  non-obvious discovery (new-game recipe + A1-A12 discovery gotchas + T1-T8
-  tooling gotchas). READ IT FIRST before cracking another game - then append
-  any new aha the moment it happens.

- AHA_MOMENTS.md committed (aia_graphc/docs/, 8.8KB): NEW GAME RECIPE (8 steps)
-  + A1-A12 discovery ahas + T1-T8 tooling ahas. The aha to end all ahas:
-  run_commands silently no-ops some payloads (size-sensitive) - ALWAYS verify
-  writes with Test-Path/Get-Item output and build files from ~0.5-1KB chunks.

