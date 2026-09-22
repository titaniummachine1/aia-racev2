"""RacingV2 handling model - stat point-buy -> vehicle params -> one tick.

REPO POLICY: values marked [unverified] are educated from ABI scrapes and the
user observation (2026-09-22) that turning is a JOINT function of max-speed
stat, turning stat AND current speed - not one formula. They get replaced by
the capture session (RACING_CONSTANTS.md 3: stat sweeps + TimePlot yaw rate).
Everything CERTAIN (driver_config) is marked [certain].

Semantic contract (matches racing_vm.rs): f32 math, one ordered pass per tick.
"""
from __future__ import annotations

# ---- certain (driver_config.json TextAsset, v0.22) ----
STEER_SENSITIVITY = 1.0        # [certain]
THROTTLE_DEMO = 0.9            # [certain]
AVOID_NEAR_DISTANCE = 4.0      # [certain]
AVOID_STRENGTH = 1.25          # [certain]
SPHERECAST_RADIUS = 0.75       # [certain]
SPHERECAST_DISTANCE = 12.0     # [certain]

# ---- unverified calibration (the capture session replaces these) ----
V_BASE, V_PER_SPEED = 18.0, 1.4        # vmax m/s = V_BASE + V_PER_SPEED * speed_pts
A_BASE, A_PER_SPEED = 6.0, 0.35        # accel m/s^2 at v=0
T_BASE, T_PER_TURN = 40.0, 7.0         # turn rate deg/s at low speed
TURN_DRAG_FROM_SPEED = 0.45            # joint term: more speed pts = less grip
SLIP_KNEE = 22.0                       # m/s where grip starts falling off
SLIP_TAIL = 0.35                       # grip floor at very high speed
STAT_BUDGET = 20                       # [certain] 20-pt budget, 3x 0..10
TICK = 1.0 / 30.0                      # game tick guess [unverified]


def stat_params(speed_pts: float, turn_pts: float) -> dict:
    """Point-buy -> vehicle parameters (the joint model the user described)."""
    vmax = V_BASE + V_PER_SPEED * speed_pts
    accel0 = A_BASE + A_PER_SPEED * speed_pts
    # joint effect: max-speed points eat into cornering (user 2026-09-22)
    turn0 = (T_BASE + T_PER_TURN * turn_pts) * (
        1.0 - TURN_DRAG_FROM_SPEED * speed_pts / 10.0
    )
    return {"vmax": vmax, "accel0": accel0, "turn0": turn0}


def grip(speed: float) -> float:
    """Speed-dependent grip multiplier (slipCurve/downforceCurve shape)."""
    if speed <= SLIP_KNEE:
        return 1.0
    over = (speed - SLIP_KNEE) / max(1.0, SLIP_KNEE)
    return max(SLIP_TAIL, 1.0 / (1.0 + 0.08 * over * over))


def step(x, y, yaw_deg, speed, steer_in, throttle, brake, p, dt=TICK):
    """One bicycle-model tick -> new state. steer_in in [-1,1]."""
    yaw_rate = p["turn0"] * grip(speed) * steer_in * STEER_SENSITIVITY
    yaw_deg = (yaw_deg + yaw_rate * dt) % 360.0
    accel = p["accel0"] * (1.0 - speed / max(1.0, p["vmax"])) * throttle
    accel -= 10.0 * brake
    speed = max(0.0, speed + accel * dt)
    speed = min(speed, p["vmax"])
    rad = yaw_deg * 3.141592653589793 / 180.0
    import math
    x += math.cos(rad) * speed * dt
    y += math.sin(rad) * speed * dt
    return x, y, yaw_deg, speed