$ErrorActionPreference = "Stop"
$env:Path = "C:\Users\Terminatort8000\AppData\Local\Temp\opencode\w64devkit\bin;" + $env:Path
$src = "c:\gitProjects\aia-racev2\modhost\paritymod-src"
$out = "c:\gitProjects\aia-racev2\modhost\paritymod"
# STAGE 2: full metadata probe (tennis exact flags). api_indices.h is validated by
# count (234 == 234 racing resolves); runtime validation stays per tennis design.
gcc -O1 -DV014_METADATA -DV014_NATURAL_TRACE -DPARITYMOD `
    -fno-stack-protector -fno-builtin -fno-strict-aliasing `
    -I $src -c "$src\bootstrap.c" -o "$out\racev2-meta.obj"
if ($LASTEXITCODE -ne 0) { throw "compile failed" }
gcc -nostdlib -mwindows -e start -o "$out\Aialanders-racev2-meta.exe" `
    "$out\racev2-meta.obj" -lkernel32 -luser32 -lgcc
if ($LASTEXITCODE -ne 0) { throw "link failed" }
Write-Host ("stage2 meta exe: " + (Get-Item "$out\Aialanders-racev2-meta.exe").Length + " bytes")
