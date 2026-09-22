# Re-apply AIComp (Aialanders) fullscreen + DPI fix.
# Run this AFTER the game is fully closed - the game rewrites its registry
# values on exit, so applying while it runs gets overwritten.
# Usage:  powershell -ExecutionPolicy Bypass -File fix_display.ps1

$k = 'HKCU\Software\Unicorn One\Aialanders'
$set = @{
  'Screenmanager Fullscreen mode_h3630240806'        = 1     # 1 = fullscreen window (3 = windowed)
  'Screenmanager Resolution Use Native_h1405027254'  = 1
  'Screenmanager Resolution Width_h182942802'        = 1920
  'Screenmanager Resolution Height_h2627697771'      = 1080
  'Screenmanager Resolution Window Width_h2524650974'  = 1920
  'Screenmanager Resolution Window Height_h1684712807' = 1080
}
foreach ($v in $set.Keys) {
  reg add $k /v $v /t REG_DWORD /d $set[$v] /f | Out-Null
}

# Stop Windows DPI scaling from shrinking the window (125% desktop scaling).
foreach ($exe in @(
  'C:\gitProjects\aia-racev2\versions\v0.22\Aialanders.exe',
  'C:\gitProjects\aia-racev2\versions\v0.22f\Aialanders.exe')) {
  reg add 'HKCU\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers' `
    /v $exe /t REG_SZ /d '~ HIGHDPIAWARE' /f | Out-Null
}

Write-Host 'Done. Launch Aialanders.exe -> should open fullscreen at 1920x1080.'
Write-Host 'Revert display settings with: reg import Aialanders_registry_backup.reg'
