# Decompilation playbook - obfuscated IL2CPP (RacingV2 v0.22, 2026-09-22)

Lessons from cracking RacingV2. Applies to the whole AIComp/Aialanders family
(Tennis v0.14/v0.15f, WorldCup, Survival, Racing) - same engine, same graph VM.

## 1. Diagnosis: when static dumping is dead

- `global-metadata.dat` starts with `D3 94 77 60`, NOT the standard magic
  `0xFAB11BAF` -> header obfuscated. Cpp2IL / Il2CppDumper refuse or misparse.
- `GameAssembly.dll` EXPORT NAMES are also obfuscated (`BEzeHmdXHBX`, `GakYfV_FNRk`...)
  so name-based static xrefs die too.
- BUT the metadata BODY is readable: 270 tooltips + 306 dropdown labels + all class
  names + a 53-kind node catalog were mined from it with plain string scans.
- Engine check: `Player.log` header = `Initialize engine version: 6000.4.3f1`.

=> abandon static dumping; use the RUNTIME PROBE below. Obfuscation costs nothing.

## 2. The runtime probe (tennis pattern, proven on racing)

`modhost\paritymod-src\bootstrap.c` = freestanding launcher EXE (no CRT):

1. walks `UnityPlayer.dll` PE import table, `VirtualProtect`s the `GetProcAddress`
   IAT slot, installs an observer,
2. logs every `GameAssembly.dll` export resolution as JSONL to `probe-startup.jsonl`:
   `{"kind":"resolve","index":N,"symbol":"...","rva":"0x..."}`,
3. `LoadLibraryW(UnityPlayer.dll)` + `UnityMain(GetModuleHandleW(0),0,<args>,1)`,
4. `metadata_probe.h` calls il2cpp APIs through `resolved[API_*]` (macro `V014_FN`)
   and resolves EVERYTHING BY MANAGED NAME: domain -> assemblies -> images ->
   `il2cpp_class_from_name` / `il2cpp_image_get_class` walk -> class + field + method
   rows (`metadata_class/metadata_field/metadata_method` + `class_catalog`).

### The API map is PORTABLE

The resolve order is the canonical `il2cpp-api-functions.h` order. Racing resolved
exactly **234** exports and tennis `api_indices.h` has exactly **234** defines
(0=`il2cpp_init` ... 233=`il2cpp_unity_set_android_network_up_state_func`) -> the
same header works across builds. Validate by count, and the probe self-validates
(wrong index = loud `metadata_*` failure, never a silent bad read).

### Build (w64devkit gcc, from user TEMP)

```powershell
$env:Path = "C:\Users\Terminatort8000\AppData\Local\Temp\opencode\w64devkit\bin;" + $env:Path
gcc -O1 -DV014_METADATA -DV014_NATURAL_TRACE -DPARITYMOD `
    -fno-stack-protector -fno-builtin -fno-strict-aliasing `
    -I <src> -c <src>\bootstrap.c -o <out>\x.obj
gcc -nostdlib -mwindows -e start -o <out>\probe.exe <out>\x.obj -lkernel32 -luser32 -lgcc
```

### Launch gotchas

- Renamed exe needs `<exename>_Data` -> `Aialanders_Data` **junction** (mklink /J),
  else Unity prints "There should be ..._Data folder next to the executable" and exits.
- GUI-subsystem exe does NOT block PowerShell `&` (no `$LASTEXITCODE`); use
  `Start-Process -PassThru -Wait`. Smoke-test without the game: run in an empty dir ->
  exit 92 + `{"kind":"load_failed"}` proves the logger works.
- Unsigned + IAT patching: needs a Defender exclusion when Defender exists (this box
  has none: `WinDefend` absent, `Get-MpComputerStatus` cmdlet missing).
- Long runs via scheduled task (agent shells kill children).

## 3. Command channel (drive the probe live)

`parity_poll_cmd` polls **`parity_cmd.json`** in the game dir (write-time gated;
re-write the file to re-trigger). Known commands: `quit` (snapshot+export_timeplot+
exit), `snapshot`, `rngtest`, `extras` (ours: value capture). Config file:
**`paritymod.json`**. Timeplot export is a first-class probe command
(`export_timeplot`) + a window subclass flushes on WM_CLOSE.

## 4. Value capture patterns (the `extras` command)

Rows are plain `XTRAS|kind|gate|key|index|value` lines in `probe-startup.jsonl`.

- Dropdown ground truth: `il2cpp_object_new(gate)` + invoke `.ctor` + invoke
  `BuildItemsAndPopulateDropdown` -> read private `System.String[] _dropdownLabels`
  or `System.ValueTuple<String,Func<T>>[] items` (Item1 = label at +0, stride 16).
  A null UI reference throws a MANAGED exception which `il2cpp_runtime_invoke`
  CAPTURES via its exception out-param -> poking UI classes is safe.
- Enum constants: the constant NAMES are class fields in DECLARATION order
  (`value__` first) - e.g. `DamageableVehiclePart.PartType` (33 parts),
  `GraphOpCode` (37 ops). Values via `il2cpp_field_static_get_value`.
- Static singletons: `<Instance>k__BackingField` via static object read
  (`VehiclePlannerService`), then raw-offset reads for embedded structs
  (`hybridOptions : PlanOptions` @0x48).
- Field offsets from the dump are OBJECT-ORIGIN-INCLUSIVE (first instance field is
  @0x10, after klass+monitor); static field offsets live in a separate blob space
  (both come from `il2cpp_field_get_offset` - distinguish via flags 0x10=STATIC).
- Strings: `il2cpp_string_length` + `il2cpp_string_chars` (UTF-16).

## 5. Asset-side docs (UnityPy) - no type trees needed

- TextAssets: `driver_config.json` (built-in driver tuning) + 3 dev-authored reference
  graphs were extracted straight from `sharedassets*.assets`.
- Node hint boxes live in `NodeTypeDataSO` MonoBehaviours (65 of them): serialized as
  ordered length-prefixed strings `[id, title, class, description, (portId, portDesc)*]`.
  `NodeConfigurationSO` holds port-type grammar + per-game mode docs.
- Technique: group every MonoBehaviour by its MonoScript (parse `m_Script` PPtr at
  raw offset 20: `[12 gobject][4 enabled][12 script][4+len name][custom...]`), then
  extract length-prefixed ASCII/UTF-16 strings in serialization order.

## 6. Encoding gotchas (game saves)

- Racing saves store dropdown INDEX as node `modifier` (tennis stores the LABEL).
- Part world position recipe: `RacingV2GetCar -> GetCarPart -> RelativePosition(13)`.
- The visual graph compiles to register bytecode `CompiledInstruction
  {Op, In0..2, Out0..2, Imm}` executed by `CompiledGraphPlan.Execute*` -
  see `docs\GRAPH_VM.md` (ISA tables) and `data\race_abi.json`.

## 7. Evidence trail (this session)

`aia-racev2\modhost\` (probe + builds), `versions\v0.22\probe-startup*.jsonl`
(stage1 api / stage2 inventory / stage3a enums), `data\racev2_api_order.csv`,
`data\racev2_class_catalog.txt` (1,563 classes), `data\racev2_meta_rows.jsonl`,
`data\race_abi.json` (consolidated), `data\race_node_hints.json` (65 nodes),
`data\hint_scan\`, compiler `AIA_Comp_Libry\racing\{dropmap.py,node_hints.json}`,
`docs\GRAPH_VM.md`, `docs\HANDOFF.md`.
