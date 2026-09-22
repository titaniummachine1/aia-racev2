#!/usr/bin/env python3
"""Extract readable RacingV2 values from sharedassets*.assets.

IL2CPP MonoBehaviours have NO embedded type trees (layout lives in
global-metadata.dat), so we read what UnityPy *can*:
  * TextAsset       - usually JSON/config payloads with real numbers
  * engine types    - Rigidbody, PhysicMaterial, etc (known layouts)
  * any object whose serialized_type actually carries nodes
Writes metadata_prefab_values.txt (append-friendly: rerun after edits).
"""
import re
from pathlib import Path

import UnityPy

GAME = Path(r"C:\gitProjects\aia-racev2\versions\v0.22\Aialanders_Data")
OUT = Path(__file__).resolve().parent / "metadata_prefab_values.txt"
FILES = ["sharedassets0.assets", "resources.assets",
         "sharedassets1.assets", "sharedassets4.assets",
         "sharedassets5.assets", "sharedassets6.assets"]

ENGINE_TYPES = ("Rigidbody", "PhysicMaterial", "WheelCollider",
                "HingeJoint", "ConstantForce")

FLOAT_KEYS = re.compile(r"(speed|vel|torque|mass|steer|angle|wheelbase|radius|"
                        r"grip|friction|accel|brake|force|damp|length|width|"
                        r"height|drag|downforce|gear|rpm|sensitivity|decay|"
                        r"yaw|drift|spring|damper)", re.I)


def dump(d, lines, depth=0):
    if not isinstance(d, dict) or depth > 4:
        return
    for k, v in d.items():
        if k in ("m_Script", "m_CorrespondingSourceObject", "m_PrefabInternal"):
            continue
        if isinstance(v, (int, float)) and FLOAT_KEYS.search(str(k)):
            lines.append("  " * (depth + 1) + f"{k} = {v}")
        elif isinstance(v, dict):
            lines.append("  " * (depth + 1) + f"{k}:")
            dump(v, lines, depth + 1)


def main() -> int:
    lines = [f"# readable values from {GAME.parent.name} v0.22", ""]
    seen = set()
    for fname in FILES:
        path = GAME / fname
        if not path.exists():
            continue
        lines.append(f"=== {fname} ===")
        print(f"loading {fname} ({path.stat().st_size // 1048576} MB)...",
              flush=True)
        env = UnityPy.load(str(path))
        for obj in env.objects:
            tn = obj.type.name
            if tn == "TextAsset":
                try:
                    txt = obj.read().m_Script or ""
                except Exception:  # noqa: BLE001
                    txt = ""
                if txt and len(txt) < 200_000:
                    key = f"txt:{fname}:{txt[:60]}"
                    if key not in seen:
                        seen.add(key)
                        lines.append(f"--- TextAsset ({fname}, "
                                     f"{len(txt)} B) ---")
                        lines.append(txt[:4000])
                continue
            if tn in ENGINE_TYPES:
                key = f"{tn}:{fname}:{obj.path_id}"
                if key in seen:
                    continue
                seen.add(key)
                lines.append(f"--- {tn} ({fname}) ---")
                try:
                    dump(obj.read_typetree(), lines)
                except Exception:  # noqa: BLE001
                    pass
                continue
            st = getattr(obj, "serialized_type", None)
            if st is not None and getattr(st, "nodes", None) \
                    and tn == "MonoBehaviour":
                key = f"mb:{fname}:{obj.path_id}"
                if key in seen:
                    continue
                seen.add(key)
                lines.append(f"--- MonoBehaviour w/ typetree ({fname}) ---")
                try:
                    dump(obj.read_typetree(), lines)
                except Exception:  # noqa: BLE001
                    pass
    OUT.write_text("\n".join(lines), encoding="utf-8")
    print("wrote", OUT.name, len(lines), "lines")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
