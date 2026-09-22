#ifndef V014_SHOT_FIXTURES_V014_H
#define V014_SHOT_FIXTURES_V014_H

/*
 * Metadata-driven direct-shot fixture contracts for Tennis v0.14.
 *
 * This file deliberately has no probe globals, JSON writer, runtime API
 * indices, field offsets, or call-site hooks.  The v0.14 capture owner binds
 * V014ShotFixtureOps to its already-validated metadata helpers and serializes
 * the raw words returned by the wrappers below.  A caller must snapshot any
 * Unity/native state outside the named managed fields as well; these helpers
 * only promise to restore the managed TennisBall/TennisGameManager fields
 * listed here.  The optional LastHitter reference is retained as two raw
 * pointer words because the constructor's close-target fallback consults that
 * managed player path.
 *
 * The method RVAs are provenance labels from the pinned metadata/disassembly
 * review.  They are never used for invocation: method_checked resolves the
 * current process by managed name and verifies arity.
 *
 * The capture owner should serialize a status/context row, one case row per
 * wrapper invocation (including invoke/exception flags and all raw words),
 * one restore row per case, and a terminal status only after the final
 * snapshot matches.  This mirrors the existing event-fixture ordering without
 * coupling this header to its JSON writer.
 */

#ifdef V014_METADATA
typedef unsigned long long size_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef int int32_t;
#ifndef NULL
#define NULL ((void*)0)
#endif
#ifndef strcmp
static int v014_shot_contract_strcmp(const char* left, const char* right) {
    if (!left || !right) return left == right ? 0 : (left ? 1 : -1);
    while (*left && *left == *right) {
        ++left;
        ++right;
    }
    return (unsigned char)*left - (unsigned char)*right;
}
#endif
#ifndef memset
static void* v014_shot_contract_memset(void* destination, int value,
                                       unsigned long long count) {
    unsigned char* bytes = (unsigned char*)destination;
    while (count--) *bytes++ = (unsigned char)value;
    return destination;
}
#endif
#ifndef memcpy
static void* v014_shot_contract_memcpy(void* destination, const void* source,
                                       unsigned long long count) {
    unsigned char* to = (unsigned char*)destination;
    const unsigned char* from = (const unsigned char*)source;
    while (count--) *to++ = *from++;
    return destination;
}
#endif
#ifndef memset
#define memset v014_shot_contract_memset
#endif
#ifndef strcmp
#define strcmp v014_shot_contract_strcmp
#endif
#ifndef memcpy
#define memcpy v014_shot_contract_memcpy
#endif
#else
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum V014ShotFieldEncoding {
    V014_SHOT_RAW_U32 = 0,
    V014_SHOT_RAW_I32 = 1,
    V014_SHOT_RAW_BOOL = 2,
    V014_SHOT_RAW_F32 = 3,
    V014_SHOT_RAW_OBJECT = 4
} V014ShotFieldEncoding;

typedef struct V014ShotFieldSpec {
    const char* key;
    const char* managed_name;
    uint8_t words;
    uint8_t encoding;
    uint8_t required;
} V014ShotFieldSpec;

typedef struct V014ShotMethodSpec {
    const char* declaring_class;
    const char* name;
    uint32_t args;
    uint32_t rva;
    const char* return_type;
} V014ShotMethodSpec;

/* These signatures intentionally match metadata_probe.h and the existing
 * serve fixture contract.  A shot fixture can therefore share the same
 * validated metadata API table without including the probe implementation. */
typedef int (*V014ShotReadWordsFn)(void* klass, void* object,
                                   const char* name, unsigned int* words,
                                   unsigned int count);
typedef int (*V014ShotWriteWordsFn)(void* klass, void* object,
                                    const char* name,
                                    const unsigned int* words,
                                    unsigned int count);
typedef void* (*V014ShotMethodCheckedFn)(void* klass, const char* name,
                                         unsigned int expected_args);
typedef int (*V014ShotInvokeWordsFn)(void* method, void* object,
                                     void** args, unsigned int* words,
                                     unsigned int count,
                                     void** exception_out);
typedef int (*V014ShotInvokeVoidFn)(void* method, void* object, void** args,
                                    void** exception_out);

typedef struct V014ShotFixtureOps {
    V014ShotReadWordsFn read_words;
    V014ShotWriteWordsFn write_words;
    V014ShotMethodCheckedFn method_checked;
    V014ShotInvokeWordsFn invoke_words;
    V014ShotInvokeVoidFn invoke_void;
} V014ShotFixtureOps;

/* The live ball fields are both the constructor inputs and the mutable state
 * touched by predictor-backed repair calls.  Keep the cache/pending fields in
 * the snapshot so a failed direct call cannot leak into the next case. */
static const V014ShotFieldSpec v014_shot_ball_fields[] = {
    {"last_hitter", "<LastHitter>k__BackingField", 2,
     V014_SHOT_RAW_OBJECT, 0},
    {"tennis_manager", "tennisManager", 2, V014_SHOT_RAW_OBJECT, 1},
    {"arcade_gravity", "arcadeGravity", 1, V014_SHOT_RAW_F32, 1},
    {"max_speed", "maxSpeed", 1, V014_SHOT_RAW_F32, 1},
    {"radius", "<Radius>k__BackingField", 1, V014_SHOT_RAW_F32, 1},
    {"slice_curve", "sliceCurve", 1, V014_SHOT_RAW_F32, 1},
    {"curve_team", "curveTeam", 1, V014_SHOT_RAW_I32, 1},
    {"last_hit_power", "lastHitPower", 1, V014_SHOT_RAW_F32, 1},
    {"flight_pace", "flightPace", 1, V014_SHOT_RAW_F32, 1},
    {"position", "<Position>k__BackingField", 3, V014_SHOT_RAW_F32, 1},
    {"velocity", "<Velocity>k__BackingField", 3, V014_SHOT_RAW_F32, 1},
    {"last_strike_pos", "lastStrikePos", 3, V014_SHOT_RAW_F32, 1},
    {"peak_y_since_hit", "peakYSinceHit", 1, V014_SHOT_RAW_F32, 1},
    {"last_position", "lastPosition", 3, V014_SHOT_RAW_F32, 1},
    {"cached_predicted_bounce", "cachedPredictedBounce", 3,
     V014_SHOT_RAW_F32, 1},
    {"cached_predicted_second_bounce", "cachedPredictedSecondBounce", 3,
     V014_SHOT_RAW_F32, 1},
    {"cached_predicted_bounce_time", "cachedPredictedBounceTime", 1,
     V014_SHOT_RAW_F32, 0},
    {"cached_predicted_second_bounce_time", "cachedPredictedSecondBounceTime",
     1, V014_SHOT_RAW_F32, 0},
    {"has_cached_predicted_bounce", "hasCachedPredictedBounce", 1,
     V014_SHOT_RAW_BOOL, 1},
    {"has_cached_predicted_second_bounce", "hasCachedPredictedSecondBounce",
     1, V014_SHOT_RAW_BOOL, 1},
    {"predicted_bounce_age", "predictedBounceAge", 1, V014_SHOT_RAW_F32, 0},
    {"shot_bounce_count", "shotBounceCount", 1, V014_SHOT_RAW_I32, 1},
    {"pending_topspin_kick", "pendingTopspinKick", 1, V014_SHOT_RAW_BOOL, 1},
    {"pending_slice_bounce", "pendingSliceBounce", 1, V014_SHOT_RAW_BOOL, 1},
    {"pending_drop_bounce", "pendingDropBounce", 1, V014_SHOT_RAW_BOOL, 1},
    {"has_crossed_net", "<HasCrossedNet>k__BackingField", 1,
     V014_SHOT_RAW_BOOL, 1},
    {"in_play", "<InPlay>k__BackingField", 1, V014_SHOT_RAW_BOOL, 1},
    {"is_serve", "<IsServe>k__BackingField", 1, V014_SHOT_RAW_BOOL, 1}
};

/* ComputeShotVelocity and the repair bodies read these manager tuning words
 * directly or through the reviewed manager getters.  They are captured as
 * raw words so a fixture cannot accidentally replace a v0.14 scene value with
 * a v0.12 default. */
static const V014ShotFieldSpec v014_shot_manager_fields[] = {
    {"court_length", "courtLength", 1, V014_SHOT_RAW_F32, 1},
    {"net_height", "netHeight", 1, V014_SHOT_RAW_F32, 1},
    {"court_y", "courtY", 1, V014_SHOT_RAW_F32, 1},
    {"aim_net_gap", "aimNetGap", 1, V014_SHOT_RAW_F32, 1},
    {"flat_speed", "flatSpeed", 1, V014_SHOT_RAW_F32, 1},
    {"flat_lift", "flatLift", 1, V014_SHOT_RAW_F32, 1},
    {"topspin_speed", "topspinSpeed", 1, V014_SHOT_RAW_F32, 1},
    {"topspin_lift", "topspinLift", 1, V014_SHOT_RAW_F32, 1},
    {"slice_speed", "sliceSpeed", 1, V014_SHOT_RAW_F32, 1},
    {"slice_lift", "sliceLift", 1, V014_SHOT_RAW_F32, 1},
    {"lob_speed", "lobSpeed", 1, V014_SHOT_RAW_F32, 1},
    {"lob_lift", "lobLift", 1, V014_SHOT_RAW_F32, 1},
    {"drop_speed", "dropSpeed", 1, V014_SHOT_RAW_F32, 1},
    {"drop_lift", "dropLift", 1, V014_SHOT_RAW_F32, 1},
    {"min_charge_power", "minChargePower", 1, V014_SHOT_RAW_F32, 1},
    {"max_charge_power", "maxChargePower", 1, V014_SHOT_RAW_F32, 1},
    {"rally_shot_count", "rallyShotCount", 1, V014_SHOT_RAW_I32, 1},
    {"is_training_mode", "<IsTrainingMode>k__BackingField", 1,
     V014_SHOT_RAW_BOOL, 1},
    {"training_player_team", "<TrainingPlayerTeam>k__BackingField", 1,
     V014_SHOT_RAW_I32, 1}
};

#define V014_SHOT_BALL_FIELD_CAPACITY \
    (sizeof(v014_shot_ball_fields) / sizeof(v014_shot_ball_fields[0]))
#define V014_SHOT_MANAGER_FIELD_CAPACITY \
    (sizeof(v014_shot_manager_fields) / sizeof(v014_shot_manager_fields[0]))

/* Getter rows are emitted beside the field rows.  The manager's effective
 * getters are important: the v0.14 TennisCourt may differ from raw manager
 * fallback fields, and get_BounceFloorY is the exact floor consumed by the
 * repair bodies. */
static const V014ShotMethodSpec v014_shot_methods[] = {
    {"TennisBall", "ComputeShotVelocity", 4, 0x000c1a4a0U,
     "UnityEngine.Vector3"},
    {"TennisBall", "RebuildLaunchForAim", 6, 0x000c1f0a0U,
     "System.Void"},
    {"TennisBall", "SteerVelocityToAim", 6, 0x000c20c70U,
     "System.Void"},
    {"TennisBall", "EnforceLobLoft", 5, 0x000c1b9a0U, "System.Void"},
    {"TennisBall", "get_BounceFloorY", 0, 0x000c23950U,
     "System.Single"},
    {"TennisBall", "NetRight", 1, 0x000c1e5c0U, "UnityEngine.Vector3"},
    {"TennisGameManager", "get_CourtY", 0, 0x000aa02e0U,
     "System.Single"},
    {"TennisGameManager", "GetDefaultAimTarget", 1, 0x000c2e8c0U,
     "UnityEngine.Vector3"},
    {"TennisGameManager", "get_CourtCenter", 0, 0x000c40c40U,
     "UnityEngine.Vector3"},
    {"TennisGameManager", "get_CourtLength", 0, 0x000c40da0U,
     "System.Single"},
    {"TennisGameManager", "get_NetHeight", 0, 0x000c41520U,
     "System.Single"}
};

typedef struct V014ShotRawField {
    unsigned int words[4];
    uint8_t valid;
} V014ShotRawField;

typedef struct V014ShotFixtureSnapshot {
    void* manager;
    void* ball;
    uint8_t manager_present;
    uint8_t ball_present;
    V014ShotRawField manager_fields[V014_SHOT_MANAGER_FIELD_CAPACITY];
    V014ShotRawField ball_fields[V014_SHOT_BALL_FIELD_CAPACITY];
} V014ShotFixtureSnapshot;

static inline const V014ShotMethodSpec* v014_shot_method_spec(
    const char* declaring_class, const char* name, uint32_t args) {
    unsigned int count = (unsigned int)(sizeof(v014_shot_methods) /
                                         sizeof(v014_shot_methods[0]));
    for (unsigned int i = 0; i < count; ++i) {
        const V014ShotMethodSpec* spec = &v014_shot_methods[i];
        if (spec->args == args &&
            strcmp(spec->declaring_class, declaring_class) == 0 &&
            strcmp(spec->name, name) == 0)
            return spec;
    }
    return NULL;
}

static inline int v014_shot_capture_object(
    const V014ShotFixtureOps* ops, void* klass, void* object,
    const V014ShotFieldSpec* specs, unsigned int count,
    V014ShotRawField* output, int required, void** object_slot,
    uint8_t* present_slot) {
    int ok = 1;
    if (!ops || !ops->read_words || !object_slot || !present_slot) return 0;
    *object_slot = object;
    *present_slot = (uint8_t)(object != NULL && klass != NULL);
    if (!*present_slot) return required ? 0 : 1;
    for (unsigned int i = 0; i < count; ++i) {
        memset(output[i].words, 0, sizeof(output[i].words));
        output[i].valid = (uint8_t)ops->read_words(
            klass, object, specs[i].managed_name, output[i].words,
            (unsigned int)specs[i].words);
        if (!output[i].valid && required && specs[i].required) ok = 0;
    }
    return ok;
}

static inline int v014_shot_restore_object(
    const V014ShotFixtureOps* ops, void* klass,
    const V014ShotFieldSpec* specs, unsigned int count,
    const V014ShotRawField* values, void* object, uint8_t present,
    int required) {
    int ok = 1;
    if (!present) return required ? 0 : 1;
    if (!ops || !ops->write_words || !klass || !object) return 0;
    for (unsigned int i = 0; i < count; ++i) {
        if (!values[i].valid) {
            if (required && specs[i].required) ok = 0;
            continue;
        }
        if (!ops->write_words(klass, object, specs[i].managed_name,
                              values[i].words,
                              (unsigned int)specs[i].words))
            ok = 0;
    }
    return ok;
}

static inline int v014_shot_fixture_snapshot_capture(
    const V014ShotFixtureOps* ops, void* manager_class, void* manager,
    void* ball_class, void* ball, V014ShotFixtureSnapshot* snapshot) {
    int ok;
    if (!snapshot) return 0;
    memset(snapshot, 0, sizeof(*snapshot));
    ok = v014_shot_capture_object(
        ops, manager_class, manager, v014_shot_manager_fields,
        (unsigned int)V014_SHOT_MANAGER_FIELD_CAPACITY,
        snapshot->manager_fields, 1, &snapshot->manager,
        &snapshot->manager_present);
    if (!v014_shot_capture_object(
            ops, ball_class, ball, v014_shot_ball_fields,
            (unsigned int)V014_SHOT_BALL_FIELD_CAPACITY,
            snapshot->ball_fields, 1, &snapshot->ball,
            &snapshot->ball_present))
        ok = 0;
    return ok;
}

static inline int v014_shot_fixture_snapshot_restore(
    const V014ShotFixtureOps* ops, void* manager_class, void* ball_class,
    const V014ShotFixtureSnapshot* snapshot) {
    int ok = 1;
    if (!snapshot) return 0;
    if (!v014_shot_restore_object(
            ops, manager_class, v014_shot_manager_fields,
            (unsigned int)V014_SHOT_MANAGER_FIELD_CAPACITY,
            snapshot->manager_fields, snapshot->manager,
            snapshot->manager_present, 1))
        ok = 0;
    if (!v014_shot_restore_object(
            ops, ball_class, v014_shot_ball_fields,
            (unsigned int)V014_SHOT_BALL_FIELD_CAPACITY,
            snapshot->ball_fields, snapshot->ball,
            snapshot->ball_present, 1))
        ok = 0;
    return ok;
}

static inline int v014_shot_fixture_read_getter(
    const V014ShotFixtureOps* ops, const char* declaring_class,
    void* klass, void* object, const char* name, unsigned int expected_args,
    unsigned int* output_words, unsigned int output_count,
    void** exception_out) {
    const V014ShotMethodSpec* spec =
        v014_shot_method_spec(declaring_class, name, expected_args);
    void* method;
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !output_words || !output_count || output_count > 4)
        return 0;
    method = ops->method_checked(klass, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, object, NULL, output_words,
                             output_count, exception_out);
}

static inline int v014_shot_fixture_read_bounce_floor_y(
    const V014ShotFixtureOps* ops, void* ball_class, void* ball,
    unsigned int output_words[1], void** exception_out) {
    return v014_shot_fixture_read_getter(
        ops, "TennisBall", ball_class, ball, "get_BounceFloorY", 0,
        output_words, 1, exception_out);
}

static inline int v014_shot_fixture_read_net_right(
    const V014ShotFixtureOps* ops, void* ball_class, void* ball,
    int32_t team, unsigned int output_words[3], void** exception_out) {
    const V014ShotMethodSpec* spec =
        v014_shot_method_spec("TennisBall", "NetRight", 1);
    void* method;
    int team_value = (int)team;
    void* args[1] = {&team_value};
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !output_words)
        return 0;
    method = ops->method_checked(ball_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, ball, args, output_words, 3,
                             exception_out);
}

static inline int v014_shot_fixture_read_default_aim_target(
    const V014ShotFixtureOps* ops, void* manager_class, void* manager,
    int32_t hitter_team, unsigned int output_words[3],
    void** exception_out) {
    const V014ShotMethodSpec* spec = v014_shot_method_spec(
        "TennisGameManager", "GetDefaultAimTarget", 1);
    void* method;
    int team_value = (int)hitter_team;
    void* args[1] = {&team_value};
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !output_words)
        return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, manager, args, output_words, 3,
                             exception_out);
}

typedef struct V014ShotSceneGetterWords {
    unsigned int court_y[1];
    unsigned int court_center[3];
    unsigned int court_length[1];
    unsigned int net_height[1];
    unsigned int bounce_floor_y[1];
    unsigned int net_right_team0[3];
    unsigned int net_right_team1[3];
    unsigned int default_aim_team0[3];
    unsigned int default_aim_team1[3];
    uint8_t court_y_valid;
    uint8_t court_center_valid;
    uint8_t court_length_valid;
    uint8_t net_height_valid;
    uint8_t bounce_floor_y_valid;
    uint8_t net_right_team0_valid;
    uint8_t net_right_team1_valid;
    uint8_t default_aim_team0_valid;
    uint8_t default_aim_team1_valid;
} V014ShotSceneGetterWords;

/* Capture the effective getter values in the same initialized scene as a
 * call.  `manager_class` is used for manager getters and `ball_class` for
 * ball getters; the wrapper never substitutes raw manager fields. */
static inline int v014_shot_fixture_capture_scene_getters(
    const V014ShotFixtureOps* ops, void* manager_class, void* manager,
    void* ball_class, void* ball, V014ShotSceneGetterWords* output) {
    void* exception = NULL;
    int ok = 1;
    if (!output) return 0;
    memset(output, 0, sizeof(*output));
    output->court_y_valid = (uint8_t)v014_shot_fixture_read_getter(
        ops, "TennisGameManager", manager_class, manager, "get_CourtY", 0,
        output->court_y, 1, &exception);
    exception = NULL;
    output->court_center_valid = (uint8_t)v014_shot_fixture_read_getter(
        ops, "TennisGameManager", manager_class, manager,
        "get_CourtCenter", 0, output->court_center, 3, &exception);
    exception = NULL;
    output->court_length_valid = (uint8_t)v014_shot_fixture_read_getter(
        ops, "TennisGameManager", manager_class, manager,
        "get_CourtLength", 0, output->court_length, 1, &exception);
    exception = NULL;
    output->net_height_valid = (uint8_t)v014_shot_fixture_read_getter(
        ops, "TennisGameManager", manager_class, manager,
        "get_NetHeight", 0, output->net_height, 1, &exception);
    exception = NULL;
    output->bounce_floor_y_valid = (uint8_t)
        v014_shot_fixture_read_bounce_floor_y(
            ops, ball_class, ball, output->bounce_floor_y, &exception);
    exception = NULL;
    output->net_right_team0_valid = (uint8_t)v014_shot_fixture_read_net_right(
        ops, ball_class, ball, 0, output->net_right_team0, &exception);
    exception = NULL;
    output->net_right_team1_valid = (uint8_t)v014_shot_fixture_read_net_right(
        ops, ball_class, ball, 1, output->net_right_team1, &exception);
    exception = NULL;
    output->default_aim_team0_valid = (uint8_t)
        v014_shot_fixture_read_default_aim_target(
            ops, manager_class, manager, 0, output->default_aim_team0,
            &exception);
    exception = NULL;
    output->default_aim_team1_valid = (uint8_t)
        v014_shot_fixture_read_default_aim_target(
            ops, manager_class, manager, 1, output->default_aim_team1,
            &exception);
    if (!output->court_y_valid || !output->court_center_valid ||
        !output->court_length_valid || !output->net_height_valid ||
        !output->bounce_floor_y_valid || !output->net_right_team0_valid ||
        !output->net_right_team1_valid || !output->default_aim_team0_valid ||
        !output->default_aim_team1_valid)
        ok = 0;
    return ok;
}

typedef struct V014ShotCallInput {
    unsigned int from[3];
    unsigned int velocity[3];
    unsigned int aim_target[3];
    unsigned int curve;
    unsigned int power;
    int32_t shot_type;
    uint8_t is_serve;
} V014ShotCallInput;

typedef struct V014ShotCallOutput {
    unsigned int result[3];
    unsigned int velocity_after[3];
    uint8_t invoke_ok;
    uint8_t exception;
} V014ShotCallOutput;

static inline void v014_shot_words_to_float3(const unsigned int words[3],
                                              float output[3]) {
    memcpy(output, words, sizeof(float) * 3U);
}

static inline void v014_shot_float3_to_words(const float input[3],
                                              unsigned int words[3]) {
    memcpy(words, input, sizeof(float) * 3U);
}

/* Invoke ComputeShotVelocity and retain its Vector3 result as three raw
 * words.  The input packet is caller-owned and is never overwritten. */
static inline int v014_shot_fixture_compute(
    const V014ShotFixtureOps* ops, void* ball_class, void* ball,
    const V014ShotCallInput* input, V014ShotCallOutput* output,
    void** exception_out) {
    const V014ShotMethodSpec* spec =
        v014_shot_method_spec("TennisBall", "ComputeShotVelocity", 4);
    float from[3];
    float aim_target[3];
    float power;
    int shot_type;
    void* args[4];
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !input || !output)
        return 0;
    memset(output, 0, sizeof(*output));
    v014_shot_words_to_float3(input->from, from);
    v014_shot_words_to_float3(input->aim_target, aim_target);
    memcpy(&power, &input->power, sizeof(power));
    shot_type = (int)input->shot_type;
    args[0] = from;
    args[1] = aim_target;
    args[2] = &shot_type;
    args[3] = &power;
    method = ops->method_checked(ball_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_words(method, ball, args, output->result, 3,
                            exception_out);
    output->invoke_ok = (uint8_t)(ok != 0);
    output->exception = (uint8_t)(exception_out && *exception_out != NULL);
    return ok;
}

static inline int v014_shot_fixture_invoke_velocity_method(
    const V014ShotFixtureOps* ops, void* ball_class, void* ball,
    const char* method_name, unsigned int expected_args,
    const V014ShotCallInput* input, V014ShotCallOutput* output,
    void** exception_out) {
    const V014ShotMethodSpec* spec =
        v014_shot_method_spec("TennisBall", method_name, expected_args);
    float from[3];
    float velocity[3];
    float aim_target[3];
    float curve;
    int shot_type;
    uint8_t is_serve;
    void* args[6];
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec ||
        !input || !output)
        return 0;
    memset(output, 0, sizeof(*output));
    memcpy(output->velocity_after, input->velocity,
           sizeof(output->velocity_after));
    v014_shot_words_to_float3(input->from, from);
    v014_shot_words_to_float3(input->velocity, velocity);
    v014_shot_words_to_float3(input->aim_target, aim_target);
    memcpy(&curve, &input->curve, sizeof(curve));
    shot_type = (int)input->shot_type;
    is_serve = input->is_serve;
    args[0] = from;
    args[1] = velocity;
    args[2] = aim_target;
    args[3] = &shot_type;
    args[4] = &curve;
    args[5] = &is_serve;
    method = ops->method_checked(ball_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_void(method, ball, args, exception_out);
    v014_shot_float3_to_words(velocity, output->velocity_after);
    output->invoke_ok = (uint8_t)(ok != 0);
    output->exception = (uint8_t)(exception_out && *exception_out != NULL);
    return ok;
}

static inline int v014_shot_fixture_rebuild(
    const V014ShotFixtureOps* ops, void* ball_class, void* ball,
    const V014ShotCallInput* input, V014ShotCallOutput* output,
    void** exception_out) {
    return v014_shot_fixture_invoke_velocity_method(
        ops, ball_class, ball, "RebuildLaunchForAim", 6, input, output,
        exception_out);
}

static inline int v014_shot_fixture_steer(
    const V014ShotFixtureOps* ops, void* ball_class, void* ball,
    const V014ShotCallInput* input, V014ShotCallOutput* output,
    void** exception_out) {
    return v014_shot_fixture_invoke_velocity_method(
        ops, ball_class, ball, "SteerVelocityToAim", 6, input, output,
        exception_out);
}

/* EnforceLobLoft has no shot-type or power argument.  The shared input packet
 * keeps those fields for case identity, but this wrapper intentionally passes
 * only the native five managed arguments. */
static inline int v014_shot_fixture_enforce_lob_loft(
    const V014ShotFixtureOps* ops, void* ball_class, void* ball,
    const V014ShotCallInput* input, V014ShotCallOutput* output,
    void** exception_out) {
    const V014ShotMethodSpec* spec =
        v014_shot_method_spec("TennisBall", "EnforceLobLoft", 5);
    float from[3];
    float velocity[3];
    float aim_target[3];
    float curve;
    uint8_t is_serve;
    void* args[5];
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec ||
        !input || !output)
        return 0;
    memset(output, 0, sizeof(*output));
    memcpy(output->velocity_after, input->velocity,
           sizeof(output->velocity_after));
    v014_shot_words_to_float3(input->from, from);
    v014_shot_words_to_float3(input->velocity, velocity);
    v014_shot_words_to_float3(input->aim_target, aim_target);
    memcpy(&curve, &input->curve, sizeof(curve));
    is_serve = input->is_serve;
    args[0] = from;
    args[1] = velocity;
    args[2] = aim_target;
    args[3] = &curve;
    args[4] = &is_serve;
    method = ops->method_checked(ball_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_void(method, ball, args, exception_out);
    v014_shot_float3_to_words(velocity, output->velocity_after);
    output->invoke_ok = (uint8_t)(ok != 0);
    output->exception = (uint8_t)(exception_out && *exception_out != NULL);
    return ok;
}

typedef enum V014ShotAimMode {
    V014_SHOT_AIM_NORMAL = 0,
    V014_SHOT_AIM_FALLBACK = 1,
    V014_SHOT_AIM_CROSSED_NET = 2,
    V014_SHOT_AIM_HIGH_PACE = 3,
    /* Aim 0.25 past the net: clearing the tape AND landing there requires a
     * hard stretch, so the game must either avoid (stretch) or fire through.
     * This profile discriminates which shots take the net into account. */
    V014_SHOT_AIM_NET_LIP = 4
} V014ShotAimMode;

typedef struct V014ShotFixtureCase {
    const char* id;
    unsigned int index;
    uint8_t profile;
    int32_t team;
    int32_t shot_type;
    unsigned int power;
    unsigned int curve;
    unsigned int from[3];
    unsigned int velocity[3];
    unsigned int aim_target[3];
    unsigned int flight_pace;
    uint8_t is_serve;
    uint8_t crossed_net;
    uint8_t fallback_aim;
    uint8_t high_pace;
} V014ShotFixtureCase;

/* 2 teams x 7 managed shot types x 3 charge points x 4 aim/pace profiles.
 * This is deliberately finite (168 cases), covers each requested dimension,
 * and contains no expected outputs.  The capture owner may invoke only the
 * methods admitted by its live scene, but must retain rejected/exception rows
 * instead of turning them into synthetic passes. */
#define V014_SHOT_TEAM_COUNT 2U
#define V014_SHOT_TYPE_COUNT 7U
#define V014_SHOT_POWER_COUNT 3U
#define V014_SHOT_PROFILE_COUNT 5U
#define V014_SHOT_FIXTURE_CASE_COUNT \
    (V014_SHOT_TEAM_COUNT * V014_SHOT_TYPE_COUNT * \
     V014_SHOT_POWER_COUNT * V014_SHOT_PROFILE_COUNT)

static const unsigned int v014_shot_power_words[V014_SHOT_POWER_COUNT] = {
    0x00000000U, 0x3f000000U, 0x3f800000U
};

static inline int v014_shot_fixture_case(
    unsigned int index, V014ShotFixtureCase* output) {
    unsigned int profile;
    unsigned int power_index;
    unsigned int type;
    unsigned int team;
    if (!output || index >= V014_SHOT_FIXTURE_CASE_COUNT) return 0;
    memset(output, 0, sizeof(*output));
    output->index = index;
    profile = index % V014_SHOT_PROFILE_COUNT;
    index /= V014_SHOT_PROFILE_COUNT;
    power_index = index % V014_SHOT_POWER_COUNT;
    index /= V014_SHOT_POWER_COUNT;
    type = index % V014_SHOT_TYPE_COUNT;
    team = index / V014_SHOT_TYPE_COUNT;
    output->profile = (uint8_t)profile;
    output->team = (int32_t)team;
    output->shot_type = (int32_t)type;
    output->power = v014_shot_power_words[power_index];
    output->curve = type == 5U ? 0x3f000000U :
                    (type == 6U ? 0xbf000000U : 0x00000000U);
    output->flight_pace = profile == V014_SHOT_AIM_HIGH_PACE
                              ? 0x3fe00000U
                              : 0x3f800000U;
    output->is_serve = 0;
    output->crossed_net = (uint8_t)(profile == V014_SHOT_AIM_CROSSED_NET);
    output->fallback_aim = (uint8_t)(profile == V014_SHOT_AIM_FALLBACK);
    output->high_pace = (uint8_t)(profile == V014_SHOT_AIM_HIGH_PACE);

    if (profile == V014_SHOT_AIM_FALLBACK) {
        output->from[0] = 0x00000000U;
        output->from[1] = 0x3f800000U;
        output->from[2] = 0x00000000U;
        output->aim_target[0] = 0x00000000U;
        output->aim_target[1] = 0x3f800000U;
        output->aim_target[2] = 0x00000000U;
        output->velocity[0] = 0x41200000U;
        output->velocity[1] = 0x40000000U;
        output->velocity[2] = 0x00000000U;
    } else {
        output->from[0] = profile == V014_SHOT_AIM_CROSSED_NET ||
                                  profile == V014_SHOT_AIM_HIGH_PACE ||
                                  profile == V014_SHOT_AIM_NET_LIP
                              ? 0xc1400000U
                              : 0xc1200000U;
        output->from[1] = 0x3f800000U;
        output->from[2] = 0x00000000U;
        output->aim_target[0] =
            profile == V014_SHOT_AIM_NET_LIP
                ? 0x3e800000U /* 0.25 past the net */
                : (profile == V014_SHOT_AIM_CROSSED_NET ||
                        profile == V014_SHOT_AIM_HIGH_PACE
                    ? 0x41400000U
                    : 0x41200000U);
        output->aim_target[1] = 0x3f800000U;
        output->aim_target[2] = 0x00000000U;
        output->velocity[0] = profile == V014_SHOT_AIM_HIGH_PACE
                                  ? 0x42340000U
                                  : 0x41700000U;
        output->velocity[1] = profile == V014_SHOT_AIM_HIGH_PACE
                                  ? 0x41200000U
                                  : 0x40a00000U;
        output->velocity[2] = 0x00000000U;
    }
    if (team != 0U) {
        output->from[0] ^= 0x80000000U;
        output->aim_target[0] ^= 0x80000000U;
        output->velocity[0] ^= 0x80000000U;
    }
    /* The id is a stable dimension label, not an expected-result key. */
    output->id = profile == V014_SHOT_AIM_NORMAL
                     ? "normal"
                     : profile == V014_SHOT_AIM_FALLBACK
                           ? "fallback"
                           : profile == V014_SHOT_AIM_CROSSED_NET
                                 ? "crossed-net"
                                 : profile == V014_SHOT_AIM_NET_LIP
                                       ? "net-lip"
                                       : "high-pace";
    return 1;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* V014_SHOT_FIXTURES_V014_H */
