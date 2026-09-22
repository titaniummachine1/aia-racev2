#!/usr/bin/env python3
"""Diag: which UnityPy typetree API works on v0.22 MonoBehaviours?"""
import UnityPy

p = (r"C:\gitProjects\aia-racev2\versions\v0.22\Aialanders_Data"
     r"\sharedassets0.assets")
env = UnityPy.load(p)
n = 0
for o in env.objects:
    if o.type.name != "MonoBehaviour":
        continue
    for api in ("read_typetree", "peek_typetree"):
        fn = getattr(o, api, None)
        if fn is None:
            print(api, "-> missing")
            continue
        try:
            t = fn()
            if isinstance(t, dict):
                print(api, "OK keys:", list(t)[:10])
            else:
                print(api, "-> returned", type(t).__name__)
        except Exception as e:  # noqa: BLE001
            print(api, "FAIL", type(e).__name__, str(e)[:120])
    try:
        r = o.read()
        print("read() ->", type(r).__name__,
              "m_Name=", getattr(r, "m_Name", "?"))
    except Exception as e:  # noqa: BLE001
        print("read() FAIL", type(e).__name__, str(e)[:120])
    n += 1
    if n >= 3:
        break
