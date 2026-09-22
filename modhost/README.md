# aia-racev2 modhost (RacingV2 v0.22 / v0.22f probe host)

Port of the tennis runtime probe (`AIA_tennis\modhost\paritymod-src`) to RacingV2.

## Why a runtime probe instead of a decompiler

Racing `global-metadata.dat` starts with `D3 94 77 60` instead of the standard IL2CPP
magic `0xFAB11BAF`, i.e. the metadata HEADER is obfuscated - `Cpp2IL` and `Il2CppDumper`
both fail on it (that is where the `Cpp2IL.exe.broken_metadata` artifact came from).
The metadata BODY is still readable (strings were mined from it), and the tennis probe
never needed a static dump: it resolves everything by MANAGED NAME at runtime.

## Stage 1 - resolve-stream capture (BUILT, ready to run)

`paritymod\Aialanders-racev2-probe.exe` is a freestanding launcher (no CRT, entry `start`).
It hooks `GetProcAddress` inside `UnityPlayer.dll` IAT, logs every `GameAssembly.dll`
export resolution as JSONL rows to `probe-startup.jsonl`, then boots Unity normally.

Run it FROM the game directory (it needs `UnityPlayer.dll` next to it):

```powershell
copy paritymod\Aialanders-racev2-probe.exe "c:\gitProjects\aia-racev2\versions\v0.22\"
cd "c:\gitProjects\aia-racev2\versions\v0.22"
.\Aialanders-racev2-probe.exe
# let the game reach the main menu, then close it - the log is flushed per row
```

Output: `probe-startup.jsonl` in the game dir, rows like
`{"kind":"resolve","index":N,"symbol":"il2cpp_...","rva":"0x..."}`.
That ORDER is what fills `paritymod-src\api_indices.h` (currently still tennis order -
must be regenerated from THIS capture before any metadata build).

## Stage 2 - metadata scope (not built yet)

Once `api_indices.h` matches racing, compile with
`-DV014_METADATA -DV014_NATURAL_TRACE -DPARITYMOD` and a racing scope that resolves
`AutosteerGate`, `AutoThrottleGate`, `RacingVehicleStats`, `ControllableWheelAuthoring`,
`WheelSettingsSO`, `GetCarPartGate`, `RelativePositionGate`, `RacingV2GetFloatGate` by
name; then field reads + invoke captures (matrices A steer, B engine, C fields).

## Build

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File build_probe.ps1
```

Toolchain: the user w64devkit gcc at
`C:\Users\Terminatort8000\AppData\Local\Temp\opencode\w64devkit\bin`.

## Notes

- Unsigned + patches an IAT in memory: needs a Windows Defender exclusion (tennis did).
- Long runs must go through a scheduled task - the agent shell kills child processes.
- The tennis host also subclassed the game window (user32) so an interactive close still
  flushes timeplots; racing will need the same once plots are involved.
