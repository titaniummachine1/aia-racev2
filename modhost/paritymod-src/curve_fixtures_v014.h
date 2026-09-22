#ifndef V014_CURVE_FIXTURES_V014_H
#define V014_CURVE_FIXTURES_V014_H

/*
 * Metadata-bound raw-word contracts for TennisBall's v0.14 curve helpers.
 *
 * This header deliberately contains no RVA, field offset, object layout, or
 * expected-result value.  The capture owner resolves each method by its live
 * managed name and arity, invokes through il2cpp_runtime_invoke, and records
 * the returned float as one raw word.  The small case list covers ordinary
 * manager/hitter context and the native fallback paths (null LastHitter,
 * close target, explicit fallback signs, and tiny strength).
 */

#ifdef V014_METADATA
typedef unsigned char v014_curve_uint8_t;
typedef unsigned int v014_curve_uint32_t;
typedef int v014_curve_int32_t;
#else
#include <stdint.h>
typedef uint8_t v014_curve_uint8_t;
typedef uint32_t v014_curve_uint32_t;
typedef int32_t v014_curve_int32_t;
#endif

typedef void* (*V014CurveMethodCheckedFn)(void* klass, const char* name,
                                          unsigned int expected_args);
typedef int (*V014CurveInvokeWordsFn)(void* method, void* object,
                                      void** args, unsigned int* words,
                                      unsigned int count,
                                      void** exception_out);

typedef struct V014CurveFixtureOps {
    V014CurveMethodCheckedFn method_checked;
    V014CurveInvokeWordsFn invoke_words;
} V014CurveFixtureOps;

typedef enum V014CurveOperation {
    V014_CURVE_COMPUTE_FLIGHT = 0,
    V014_CURVE_TOWARD_AIM = 1
} V014CurveOperation;

/* hitter_slot is -1 for a deliberately null LastHitter, 0 for the live home
 * object, and 1 for the live away object.  A null hitter is not a guessed
 * team: the writer records the actual post-write pointer and team fallback. */
typedef struct V014CurveFixtureCase {
    const char* id;
    v014_curve_uint32_t index;
    v014_curve_uint8_t operation;
    v014_curve_int32_t team;
    v014_curve_int32_t hitter_slot;
    v014_curve_int32_t physical_type;
    v014_curve_int32_t trick_curve_side;
    v014_curve_uint32_t power;
    v014_curve_uint32_t strength;
    v014_curve_uint32_t fallback_sign;
    v014_curve_uint32_t from[3];
    v014_curve_uint32_t aim_target[3];
} V014CurveFixtureCase;

typedef struct V014CurveCallOutput {
    v014_curve_uint32_t result[1];
    v014_curve_uint8_t invoke_ok;
    v014_curve_uint8_t exception;
} V014CurveCallOutput;

static inline void v014_curve_words_to_float3(const v014_curve_uint32_t input[3],
                                              float output[3]) {
    unsigned int i;
    for (i = 0; i < 3; ++i)
        *((v014_curve_uint32_t*)&output[i]) = input[i];
}

/* ComputeFlightCurve(Vector3, Vector3, TennisShotType, Single, Int32). */
static inline int v014_curve_fixture_compute(
    const V014CurveFixtureOps* ops, void* ball_class, void* ball,
    const V014CurveFixtureCase* input, V014CurveCallOutput* output,
    void** exception_out) {
    float from[3], aim_target[3], power;
    int shot_type, trick_curve_side;
    void* args[5];
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_words || !input || !output)
        return 0;
    output->result[0] = 0;
    output->invoke_ok = 0;
    output->exception = 0;
    v014_curve_words_to_float3(input->from, from);
    v014_curve_words_to_float3(input->aim_target, aim_target);
    *((v014_curve_uint32_t*)&power) = input->power;
    shot_type = (int)input->physical_type;
    trick_curve_side = (int)input->trick_curve_side;
    args[0] = from;
    args[1] = aim_target;
    args[2] = &shot_type;
    args[3] = &power;
    args[4] = &trick_curve_side;
    method = ops->method_checked(ball_class, "ComputeFlightCurve", 5);
    if (!method) return 0;
    ok = ops->invoke_words(method, ball, args, output->result, 1,
                           exception_out);
    output->invoke_ok = (v014_curve_uint8_t)(ok != 0);
    output->exception = (v014_curve_uint8_t)(exception_out &&
                                             *exception_out != 0);
    return ok;
}

/* CurveTowardAim(Vector3, Vector3, Single, Single). */
static inline int v014_curve_fixture_toward_aim(
    const V014CurveFixtureOps* ops, void* ball_class, void* ball,
    const V014CurveFixtureCase* input, V014CurveCallOutput* output,
    void** exception_out) {
    float from[3], aim_target[3], strength, fallback_sign;
    void* args[4];
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_words || !input || !output)
        return 0;
    output->result[0] = 0;
    output->invoke_ok = 0;
    output->exception = 0;
    v014_curve_words_to_float3(input->from, from);
    v014_curve_words_to_float3(input->aim_target, aim_target);
    *((v014_curve_uint32_t*)&strength) = input->strength;
    *((v014_curve_uint32_t*)&fallback_sign) = input->fallback_sign;
    args[0] = from;
    args[1] = aim_target;
    args[2] = &strength;
    args[3] = &fallback_sign;
    method = ops->method_checked(ball_class, "CurveTowardAim", 4);
    if (!method) return 0;
    ok = ops->invoke_words(method, ball, args, output->result, 1,
                           exception_out);
    output->invoke_ok = (v014_curve_uint8_t)(ok != 0);
    output->exception = (v014_curve_uint8_t)(exception_out &&
                                             *exception_out != 0);
    return ok;
}

#define V014_CURVE_NORMAL_CASE_COUNT 18U
#define V014_CURVE_FALLBACK_CASE_COUNT 6U
#define V014_CURVE_TOWARD_CASE_COUNT 5U
#define V014_CURVE_FIXTURE_CASE_COUNT \
    (V014_CURVE_NORMAL_CASE_COUNT + V014_CURVE_FALLBACK_CASE_COUNT + \
     V014_CURVE_TOWARD_CASE_COUNT)

static inline void v014_curve_copy3(v014_curve_uint32_t output[3],
                                     v014_curve_uint32_t x,
                                     v014_curve_uint32_t y,
                                     v014_curve_uint32_t z) {
    output[0] = x; output[1] = y; output[2] = z;
}

/* The list is generated instead of relying on an ABI-sensitive packed array.
 * Every word is explicit so near/fallback geometry remains replayable. */
static inline int v014_curve_fixture_case(
    unsigned int index, V014CurveFixtureCase* output) {
    static const int normal_types[6] = {0, 1, 3, 4, 5, 6};
    static const unsigned int powers[3] = {
        0x00000000U, 0x3f000000U, 0x3f800000U};
    static const char* normal_ids[6] = {
        "normal-physical-type-0", "normal-physical-type-1",
        "normal-physical-type-3", "normal-physical-type-4",
        "normal-physical-type-5", "normal-physical-type-6"};
    unsigned int normal_index, power_index;
    if (!output || index >= V014_CURVE_FIXTURE_CASE_COUNT) return 0;
    output->index = index;
    output->operation = V014_CURVE_COMPUTE_FLIGHT;
    output->team = 0;
    output->hitter_slot = 0;
    output->physical_type = 0;
    output->trick_curve_side = 0;
    output->power = 0;
    output->strength = 0x40a66666U;
    output->fallback_sign = 0;
    v014_curve_copy3(output->from, 0xc1000000U, 0x3f800000U, 0x00000000U);
    v014_curve_copy3(output->aim_target, 0x41000000U, 0x3f800000U,
                     0x40400000U);
    if (index < V014_CURVE_NORMAL_CASE_COUNT) {
        normal_index = index / 3U;
        power_index = index % 3U;
        output->team = (v014_curve_int32_t)(normal_index & 1U);
        output->hitter_slot = output->team;
        output->physical_type = normal_types[normal_index / 1U];
        output->power = powers[power_index];
        output->id = normal_ids[normal_index];
        return 1;
    }
    if (index < V014_CURVE_NORMAL_CASE_COUNT + V014_CURVE_FALLBACK_CASE_COUNT) {
        unsigned int fallback_index = index - V014_CURVE_NORMAL_CASE_COUNT;
        output->id = (fallback_index / 3U) == 0U
                         ? "fallback-close-physical-type-3"
                         : "fallback-close-physical-type-4";
        output->physical_type = (fallback_index / 3U) == 0U ? 3 : 4;
        output->power = powers[fallback_index % 3U];
        output->team = (v014_curve_int32_t)(fallback_index & 1U);
        output->hitter_slot = -1;
        v014_curve_copy3(output->from, 0x00000000U, 0x3f800000U,
                         0x00000000U);
        v014_curve_copy3(output->aim_target, 0x3dcccccdU, 0x3f800000U,
                         0x00000000U);
        return 1;
    }
    output->operation = V014_CURVE_TOWARD_AIM;
    index -= V014_CURVE_NORMAL_CASE_COUNT + V014_CURVE_FALLBACK_CASE_COUNT;
    if (index == 0U || index == 1U) {
        output->id = index == 0U ? "toward-normal-home" : "toward-normal-away";
        output->team = (v014_curve_int32_t)index;
        output->hitter_slot = (v014_curve_int32_t)index;
        return 1;
    }
    if (index == 2U || index == 3U) {
        output->id = index == 2U ? "toward-fallback-positive" :
                                   "toward-fallback-negative";
        output->team = 0;
        output->hitter_slot = -1;
        output->fallback_sign = index == 2U ? 0x3f800000U : 0xbf800000U;
        v014_curve_copy3(output->from, 0x00000000U, 0x3f800000U,
                         0x00000000U);
        v014_curve_copy3(output->aim_target, 0x3dcccccdU, 0x3f800000U,
                         0x00000000U);
        return 1;
    }
    output->id = "toward-tiny-strength";
    output->team = 0;
    output->hitter_slot = -1;
    output->strength = 0x3a83126fU;
    output->fallback_sign = 0x3f800000U;
    return 1;
}

#endif /* V014_CURVE_FIXTURES_V014_H */
