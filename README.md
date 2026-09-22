# aia-racev2

Working copy of the **AIComp RacingV2** game, plus the archive of what we know.

## Layout

```
versions\v0.22\    RacingV2_v0_22   (unzipped, runnable: Aialanders.exe)
versions\v0.22f\   RacingV2_v0_22f  (unzipped, runnable)
Aialanders_registry_backup.reg   pre-fix display settings, see below
```

Source zips stay untouched in `c:\gitProjects\aialanders-legacy\games\Racing\`.

## Display settings (why the window was tiny)

**All AIComp games share ONE registry key** — `HKCU\Software\Unicorn One\Aialanders`
(note `SoccerMaxTime` / `TennisMaxTime` live in it too). An earlier Tennis/Soccer
session had saved **windowed 1280x720, Use Native = 0**, so Racing opened small
and would not go fullscreen.

Fixed 2026-09-22 to:

| Value | Before | After |
|---|---|---|
| `Screenmanager Fullscreen mode_h3630240806` | `3` windowed | `1` fullscreen window |
| `Screenmanager Resolution Use Native_h1405027254` | `0` | `1` |
| `Screenmanager Resolution Width/Height_h...` | 1280x720 | 1920x1080 (native) |
| `Screenmanager Resolution Window Width/Height_h...` | 1280x720 | 1920x1080 |

Restore the old behaviour with:

```
reg import Aialanders_registry_backup.reg
```

If the game is later closed while still windowed, it writes those values back on
exit — re-apply the same six values (or delete the key to reset to defaults).

## Simulator

There is **no Racing simulator** — the soccer/tennis one (`aia_comp-sim`) is the
only sim that exists. Racing has zero mined data; see
`aialanders-legacy\games\Racing\` (zips only) for the reference installs.
