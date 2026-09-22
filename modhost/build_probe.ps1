$ErrorActionPreference = "Stop"
$env:Path = "C:\Users\Terminatort8000\AppData\Local\Temp\opencode\w64devkit\bin;" + $env:Path
$src = "c:\gitProjects\aia-racev2\modhost\paritymod-src"
$out = "c:\gitProjects\aia-racev2\modhost\paritymod"

# STAGE 1 (this build): resolve-stream capture only.
# No -DV014_METADATA: bootstrap.c is version-agnostic and needs nothing else.
# Purpose: learn RacingV2 il2cpp export ORDER (api_indices.h) from the
# probe-startup.jsonl rows. Tennis needed the same first step.
gcc -O1 -fno-stack-protector -fno-builtin -fno-strict-aliasing `
    -I $src -c "$src\bootstrap.c" -o "$out\racev2-probe.obj"
if ($LASTEXITCODE -ne 0) { throw "compile failed" }
gcc -nostdlib -mwindows -e start -o "$out\Aialanders-racev2-probe.exe" `
    "$out\racev2-probe.obj" -lkernel32 -luser32 -lgcc
if ($LASTEXITCODE -ne 0) { throw "link failed" }
Write-Host ("racev2 probe exe: " + (Get-Item "$out\Aialanders-racev2-probe.exe").Length + " bytes")

# STAGE 2 (later, once api_indices.h is regenerated from a real capture):
# add -DV014_METADATA -DV014_NATURAL_TRACE -DPARITYMOD and a racing metadata scope.
