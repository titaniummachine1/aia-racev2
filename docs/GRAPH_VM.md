# AIComp graph VM (CompiledGraphPlan) - captured 2026-09-22 from v0.22

Source: runtime probe (`Aialanders-racev2-meta.exe`), rows in
`data\race_abi.json` / `probe-startup_stage3a.jsonl`. Engine: Unity 6000.4.3f1.
This is the code that EXECUTES the visual scripting graphs - shared across AIComp
games (port type list includes Survival-only types, so Tennis/WorldCup/Survival use
the same VM family).

## Instruction format (register bytecode)

```
CompiledInstruction {
    Op  : GraphOpCode
    In0, In1, In2   : Int32   // register indices
    Out0, Out1, Out2: Int32   // register indices
    Imm : Int32               // immediate / constant slot
}
```

Values are boxed through `GraphValue { Kind: GraphValueKind, Bool, Float, Vector3, Ref }`
with `GraphValueKind = None, Bool, Float, Vector3, Ref` (cached statics:
`GraphValue.None`, `BoxedTrue`, `BoxedFalse`).

## Opcodes (37, declaration order = enum value)

| # | Op | # | Op |
|---|----|---|----|
| 0 | ReadPort | 19 | CompareBools |
| 1 | Copy | 20 | CompareFloats |
| 2 | AddFloats | 21 | CompareStrings |
| 3 | SubtractFloats | 22 | AddStrings |
| 4 | MultiplyFloats | 23 | AddVector3 |
| 5 | DivideFloats | 24 | SubtractVector3 |
| 6 | ModuloFloats | 25 | ScaleVector3 |
| 7 | PowerFloats | 26 | NormalizeVector3 |
| 8 | AbsFloat | 27 | MagnitudeVector3 |
| 9 | ClampFloat | 28 | DistanceVector3 |
| 10 | LerpFloat | 29 | DotVector3 |
| 11 | RandomFloat | 30 | ConstructVector3 |
| 12 | OperationFloat | 31 | SplitVector3 |
| 13 | AndBool | 32 | RelativePosition |
| 14 | OrBool | 33 | ConditionalPick |
| 15 | NotBool | 34 | ReadVariable |
| 16 | IsNull | 35 | WriteVariable |
| 17 | CompareBools* | 36 | CallGate / CallHandler / CallFunction |

(*) exact order authoritative in `race_abi.json` (`graph_vm.opcodes`) /
`AIA_Comp_Libry\racing\dropmap.py::GRAPH_OPCODES`.

## Interpreter (`CompiledGraphPlan`)

- `Execute` / `ExecuteInstructions` - the instruction loop
- `ExecuteFunctionCall` - `CallFunction` handling
- `EvaluateOperation` - the arithmetic/compare op family
- `EvaluateRelativePosition` - NATIVE handler for opcode 32 (this is the
  RelativePosition semantics in code - prime probing target)
- `GraphCompiler.Compile` / `CarryOverRegisters` - node graph -> register plan
  (`CarryOverRegisters` explains cross-frame state for latches/Relay)

Supporting: `CompiledInstruction`, `GraphValue(Kind)`, `ICompiledGate`,
`CompiledGatePorts`, `NodeSolver`, `GraphLine`, `GraphOpCode`.

## Port type grammar (NodeConfigurationSO "Default")

Input-capable: String1/2, RaycastHit1-6, Float1-3, Bool1-3, Spherecast1-3, Vector31/2,
Stat1-3, Properties1, Color1-3, Country1, Transform1/2, SurvivalState1/2,
SurvivalEmote1/2, Any1-3, Car1, Waypoint1.  (Survival* = other game; Any = wildcard.)

## Game mode doc (NodeConfigurationSO "RacingV2" text, verbatim)

> The goal of the simulation is to deal the most damage to other cars. Damage is dealt
> by colliding with other cars but each car has specific vulerable points that, if
> damaged, will affect how a car drives. Front wheels controls steering ability. Rear
> wheels, engine, drive shaft, control acceleration. If a car engine health hit 0 it
> will explode dealing damage to all nearby cars.

=> part->behavior map for the sim: WheelFL/FR + Steering -> steering;
WheelRL/RR + Engine + Driveshaft -> acceleration; Engine health 0 -> AoE explosion.

## How we use this (parity path)

1. Our compiler (`aia_graphc`) keeps its Python frontend but can target this ISA:
   ops map 1:1 onto `GraphOpCode` names (AddFloats etc. match node ids).
2. Behavioral parity testing WITHOUT rebuilding the VM: the probe can construct
   `CompiledInstruction[]` and call `CompiledGraphPlan.Execute` in the live game
   (same fixture pattern as tennis `simulate_fixture` rows), so parity fixtures can
   be validated against the real interpreter.
3. Node docs for the compiler: `AIA_Comp_Libry\racing\node_hints.json` (65 nodes
   with title/description/port descriptions) + `dropmap.py` (dropdown ground truth).

## Related captures

- dropdowns: GetCarPartGate 37 (4 specials + 33 PartType), RacingV2GetFloat 19,
  RacingV2GetCarGate 27, RacingV2GetBoolGate 3, GetWaypoint 4, Waypoint 4.
- structs: VehicleData (maxSpeed/acceleration/steerSpeed = Stat mapping),
  WheelFrictionData (grip curve), HybridAStarTypes.PlanOptions (wheelbaseMeters,
  maxSteerDegrees), DamageableVehiclePart (damage model).
