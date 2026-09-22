#ifndef V014_SERVE_FIXTURES_V014_H
#define V014_SERVE_FIXTURES_V014_H

/*
 * Separate, metadata-driven fixture contracts for Tennis v0.14 serve/scoring.
 *
 * This header deliberately does not include metadata_probe.h and defines no
 * probe globals or entry point.  build_capture can include both headers and
 * bind the existing metadata_* functions through V014ServeFixtureOps.  The
 * callbacks below keep raw field words and by-reference method arguments
 * intact, so a fixture can restore the selected pre-call object state before
 * its next case.  The caller must additionally snapshot any Unity RNG state
 * and any nested/engine objects it uses; this header does not claim a complete
 * process rollback.  No expected score, classifier, or random result is
 * supplied by this file.
 */

/* The probe is linked without a CRT and its Windows-target clang invocation
 * intentionally has no MSVC sysroot.  Keep the contract usable in that
 * freestanding translation unit while retaining ordinary headers for the
 * standalone C/C++ contract tests. */
#ifdef V014_METADATA
typedef unsigned long long size_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;
#ifndef NULL
#define NULL ((void*)0)
#endif
static int v014_serve_contract_strcmp(const char* left,const char* right) {
    if(!left||!right) return left==right?0:(left?1:-1);
    while(*left&&*left==*right){++left;++right;}
    return (unsigned char)*left-(unsigned char)*right;
}
static void* v014_serve_contract_memset(void* destination,int value,
                                        unsigned long long count) {
    unsigned char* bytes=(unsigned char*)destination;
    while(count--) *bytes++=(unsigned char)value;
    return destination;
}
#define memset v014_serve_contract_memset
#define strcmp v014_serve_contract_strcmp
#else
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum V014ServeFieldEncoding {
    V014_SERVE_RAW_U32 = 0,
    V014_SERVE_RAW_I32 = 1,
    V014_SERVE_RAW_BOOL = 2,
    V014_SERVE_RAW_F32 = 3,
    V014_SERVE_RAW_U64 = 4,
    V014_SERVE_RAW_OBJECT = 5
} V014ServeFieldEncoding;

typedef struct V014ServeFieldSpec {
    const char* key;
    const char* managed_name;
    uint8_t words;
    uint8_t encoding;
    uint8_t required;
} V014ServeFieldSpec;

typedef struct V014ServeMethodSpec {
    const char* declaring_class;
    const char* name;
    uint32_t args;
    uint32_t rva;
    const char* return_type;
} V014ServeMethodSpec;

typedef struct V014ServeReasonSlot {
    const char* use;
    uint64_t absolute_address;
} V014ServeReasonSlot;

/* These signatures intentionally match the static helpers in
 * instrumentation/v014/metadata_probe.h. */
typedef int (*V014ServeReadWordsFn)(void* klass, void* object,
                                    const char* name, unsigned int* words,
                                    unsigned int count);
typedef int (*V014ServeWriteWordsFn)(void* klass, void* object,
                                     const char* name,
                                     const unsigned int* words,
                                     unsigned int count);
typedef void* (*V014ServeMethodCheckedFn)(void* klass, const char* name,
                                          unsigned int expected_args);
typedef int (*V014ServeInvokeWordsFn)(void* method, void* object,
                                      void** args, unsigned int* words,
                                      unsigned int count,
                                      void** exception_out);
typedef int (*V014ServeInvokeVoidFn)(void* method, void* object, void** args,
                                     void** exception_out);

/* These two callbacks are intentionally kept outside metadata_probe.h.  A
 * capture owner can bind them to the already-resolved image base and the
 * IL2CPP String APIs without making this fixture header depend on probe
 * globals.  `read_static_object` must dereference the initialized pointer at
 * the image-relative slot; the slot table below contains virtual addresses
 * from the pinned image, not managed String contents. */
typedef void* (*V014ServeReadStaticObjectFn)(uint64_t absolute_address);
typedef int (*V014ServeReadManagedStringFn)(void* string_object,
                                            uint32_t* length_out,
                                            const uint16_t** chars_out);

typedef struct V014ServeFixtureOps {
    V014ServeReadWordsFn read_words;
    V014ServeWriteWordsFn write_words;
    V014ServeMethodCheckedFn method_checked;
    V014ServeInvokeWordsFn invoke_words;
    V014ServeInvokeVoidFn invoke_void;
} V014ServeFixtureOps;

/* The direct serve fixture previously labelled a hitter by case dimension
 * only.  Read the actual managed Team field as well so a native
 * MustLetServeBounce result can be checked against the object passed to the
 * callback rather than against a caller-supplied expectation. */
static inline int v014_serve_fixture_read_player_team(
    const V014ServeFixtureOps* ops, void* player_class, void* player,
    uint32_t* team_word) {
    if (!ops || !ops->read_words || !player_class || !player || !team_word)
        return 0;
    return ops->read_words(player_class, player, "Team", team_word, 1);
}

/* The field names and widths are taken from the v0.14 metadata capture.  A
 * UInt64 is two raw 32-bit words in the little-endian IL2CPP field API. */
static const V014ServeFieldSpec v014_serve_manager_fields[] = {
    {"game_state", "<CurrentGameState>k__BackingField", 1,
     V014_SERVE_RAW_U32, 1},
    {"serving_team", "<ServingTeam>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"last_point_winner", "<LastPointWinner>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"last_point_was_ace", "<LastPointWasAce>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"last_serve_was_fault", "<LastServeWasFault>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"last_point_was_double_fault", "<LastPointWasDoubleFault>k__BackingField",
     1, V014_SERVE_RAW_BOOL, 1},
    {"last_point_was_foul", "<LastPointWasFoul>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"between_point_until_tick", "betweenPointUntilTick", 2,
     V014_SERVE_RAW_U64, 1},
    {"serve_deadline_tick", "serveDeadlineTick", 2, V014_SERVE_RAW_U64, 1},
    {"serve_setup_until_tick", "serveSetupUntilTick", 2,
     V014_SERVE_RAW_U64, 1},
    {"serve_setup_earliest_end_tick", "serveSetupEarliestEndTick", 2,
     V014_SERVE_RAW_U64, 1},
    {"serve_setup_receiver_settled_ticks", "serveSetupReceiverSettledTicks",
     1, V014_SERVE_RAW_I32, 1},
    {"point_resolved", "pointResolved", 1, V014_SERVE_RAW_BOOL, 1},
    {"serve_faults_this_point", "serveFaultsThisPoint", 1,
     V014_SERVE_RAW_I32, 1},
    {"consecutive_double_fault_points", "consecutiveDoubleFaultPoints", 1,
     V014_SERVE_RAW_I32, 1},
    {"double_fault_streak_teams", "doubleFaultStreakTeams", 1,
     V014_SERVE_RAW_U32, 1},
    {"alternating_foul_streak", "alternatingFoulStreak", 1,
     V014_SERVE_RAW_I32, 1},
    {"last_fouling_team", "lastFoulingTeam", 1, V014_SERVE_RAW_I32, 1},
    {"consecutive_ace_streak", "consecutiveAceStreak", 1,
     V014_SERVE_RAW_I32, 1},
    {"rally_shot_count", "rallyShotCount", 1, V014_SERVE_RAW_I32, 1},
    {"serve_countdown_display", "<ServeCountdownDisplay>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"serve_tossed", "<ServeTossed>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"require_serve_bounce", "requireServeBounce", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"serve_in_play", "serveInPlay", 1, V014_SERVE_RAW_BOOL, 1},
    {"receiver_touched_ball", "receiverTouchedBall", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"last_shot_was_serve", "lastShotWasServe", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"coin_toss_alternating_foul_streak", "coinTossAlternatingFoulStreak", 1,
     V014_SERVE_RAW_I32, 1},
    {"coin_toss_ace_streak", "coinTossAceStreak", 1,
     V014_SERVE_RAW_I32, 1},
    {"serve_timing_perfect_vy", "serveTimingPerfectVy", 1,
     V014_SERVE_RAW_F32, 1},
    {"serve_timing_good_vy", "serveTimingGoodVy", 1,
     V014_SERVE_RAW_F32, 1},
    {"serve_timing_perfect_vy_forgiven", "serveTimingPerfectVyForgiven", 1,
     V014_SERVE_RAW_F32, 1},
    {"serve_timing_good_vy_forgiven", "serveTimingGoodVyForgiven", 1,
     V014_SERVE_RAW_F32, 1},
    {"serve_timing_max_vy", "serveTimingMaxVy", 1, V014_SERVE_RAW_F32, 1},
    {"serve_timing_forgiveness_double_fault_streak",
     "serveTimingForgivenessDoubleFaultStreak", 1, V014_SERVE_RAW_I32, 1},
    {"serve_timing_forgiveness_expand_over_double_faults",
     "serveTimingForgivenessExpandOverDoubleFaults", 1,
     V014_SERVE_RAW_I32, 1}
};

static const V014ServeFieldSpec v014_serve_score_fields[] = {
    {"points_to_win_game", "<PointsToWinGame>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"matches_to_win", "<MatchesToWin>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"home_points", "<HomePoints>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"away_points", "<AwayPoints>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"home_matches", "<HomeMatches>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"away_matches", "<AwayMatches>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"game_over", "<GameOver>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"winner_team", "<WinnerTeam>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"just_won_game", "<JustWonGame>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"last_game_winner", "<LastGameWinner>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"award_serial", "<AwardSerial>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1}
};

static const V014ServeFieldSpec v014_serve_stats_fields[] = {
    {"aces", "Aces", 1, V014_SERVE_RAW_I32, 1},
    {"faults", "Faults", 1, V014_SERVE_RAW_I32, 1},
    {"double_faults", "DoubleFaults", 1, V014_SERVE_RAW_I32, 1},
    {"fouls", "Fouls", 1, V014_SERVE_RAW_I32, 1},
    {"outs", "Outs", 1, V014_SERVE_RAW_I32, 1},
    {"serving_points_won", "ServingPointsWon", 1, V014_SERVE_RAW_I32, 1},
    {"not_serving_points_won", "NotServingPointsWon", 1,
    V014_SERVE_RAW_I32, 1}
};

/* Ball words needed to drive the manager event callbacks.  Reference fields
 * such as LastHitter are retained as raw object words; callers must restore
 * the original object reference, not a copied host pointer. */
static const V014ServeFieldSpec v014_serve_ball_fields[] = {
    {"last_hitter", "<LastHitter>k__BackingField", 1,
     V014_SERVE_RAW_OBJECT, 1},
    {"last_shot_type", "<LastShotType>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"bounce_count_current_half", "<BounceCountOnCurrentHalf>k__BackingField",
     1, V014_SERVE_RAW_I32, 1},
    {"current_half", "<CurrentHalf>k__BackingField", 1,
     V014_SERVE_RAW_I32, 1},
    {"in_play", "<InPlay>k__BackingField", 1, V014_SERVE_RAW_BOOL, 1},
    {"is_serve", "<IsServe>k__BackingField", 1, V014_SERVE_RAW_BOOL, 1},
    {"has_crossed_net", "<HasCrossedNet>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"has_bounced_in_since_hit", "<HasBouncedInSinceHit>k__BackingField", 1,
     V014_SERVE_RAW_BOOL, 1},
    {"last_bounce_position", "<LastBouncePosition>k__BackingField", 3,
     V014_SERVE_RAW_F32, 1},
    {"has_fire", "<HasFire>k__BackingField", 1, V014_SERVE_RAW_BOOL, 1},
    {"velocity", "<Velocity>k__BackingField", 3, V014_SERVE_RAW_F32, 1},
    {"position", "<Position>k__BackingField", 3, V014_SERVE_RAW_F32, 1},
    {"radius", "<Radius>k__BackingField", 1, V014_SERVE_RAW_F32, 1},
    {"slice_curve", "sliceCurve", 1, V014_SERVE_RAW_F32, 1},
    {"curve_team", "curveTeam", 1, V014_SERVE_RAW_I32, 1},
    {"last_hit_power", "lastHitPower", 1, V014_SERVE_RAW_F32, 1},
    {"flight_pace", "flightPace", 1, V014_SERVE_RAW_F32, 1},
    /* OnBallBounced and its nested ball paths can update these prediction/
     * bounce caches.  They are optional so a startup-only scene remains
     * usable, but a direct event fixture should require and verify them. */
    {"last_strike_pos", "lastStrikePos", 3, V014_SERVE_RAW_F32, 0},
    {"peak_y_since_hit", "peakYSinceHit", 1, V014_SERVE_RAW_F32, 0},
    {"last_position", "lastPosition", 3, V014_SERVE_RAW_F32, 0},
    {"cached_predicted_bounce", "cachedPredictedBounce", 3,
     V014_SERVE_RAW_F32, 0},
    {"cached_predicted_second_bounce", "cachedPredictedSecondBounce", 3,
     V014_SERVE_RAW_F32, 0},
    {"cached_predicted_bounce_time", "cachedPredictedBounceTime", 1,
     V014_SERVE_RAW_F32, 0},
    {"cached_predicted_second_bounce_time", "cachedPredictedSecondBounceTime",
     1, V014_SERVE_RAW_F32, 0},
    {"has_cached_predicted_bounce", "hasCachedPredictedBounce", 1,
     V014_SERVE_RAW_BOOL, 0},
    {"has_cached_predicted_second_bounce", "hasCachedPredictedSecondBounce",
     1, V014_SERVE_RAW_BOOL, 0},
    {"predicted_bounce_age", "predictedBounceAge", 1,
     V014_SERVE_RAW_F32, 0},
    {"shot_bounce_count", "shotBounceCount", 1, V014_SERVE_RAW_I32, 0},
    {"release_scale_elapsed", "releaseScaleElapsed", 1,
     V014_SERVE_RAW_F32, 0}
};

/* These are static-string storage slots referenced by the reviewed event
 * bodies.  They are addresses in the pinned image, not managed string
 * contents.  A live capture must read the initialized object at each slot
 * after class initialization and serialize it with il2cpp_string_chars. */
static const V014ServeReasonSlot v014_serve_reason_slots[] = {
    {"AwardPoint.double_fault_compare", 0x187047d98ULL},
    {"AwardPoint.foul_compare/OnBallStruck", 0x186fdb870ULL},
    {"AwardPoint.out_compare/OnBallOut", 0x186fa30c0ULL},
    {"OnBallBounced.no_cross_handle_fault", 0x186ff1c88ULL},
    {"OnBallBounced.own_half_handle_fault", 0x186ff1ef0ULL},
    {"OnBallBounced.service_box_handle_fault", 0x186ff2628ULL},
    {"OnBallHitNet.handle_fault", 0x18704a770ULL},
    {"OnBallOut.serve_handle_fault", 0x186ff2028ULL},
    {"OnBallBounced.second_bounce_award", 0x187047c60ULL},
    {"OnBallBounced.short_bounce_award", 0x187041da8ULL},
    {"OnBallOut.last_hitter_award", 0x187053c08ULL}
};

static inline size_t v014_serve_reason_slot_count(void) {
    return sizeof(v014_serve_reason_slots) /
           sizeof(v014_serve_reason_slots[0]);
}

typedef struct V014ServeReasonCapture {
    const char* use;
    uint64_t slot_address;
    void* object;
    uint32_t length;
    const uint16_t* chars;
    uint8_t slot_read_ok;
    uint8_t string_read_ok;
} V014ServeReasonCapture;

/* Read every reviewed reason slot after managed class initialization.  This
 * only observes the slot/object/value; it never assigns a spelling or
 * manufactures an expected classifier result.  The returned `chars` pointer
 * remains owned by the managed runtime and must be serialized before the
 * caller changes scene/runtime state. */
static inline int v014_serve_capture_reason_slots(
    V014ServeReadStaticObjectFn read_static_object,
    V014ServeReadManagedStringFn read_managed_string,
    V014ServeReasonCapture* output, size_t output_count) {
    size_t i;
    const size_t count = v014_serve_reason_slot_count();
    int ok = read_static_object && read_managed_string && output &&
             output_count >= count;
    if (!ok) return 0;
    for (i = 0; i < count; ++i) {
        void* object = read_static_object(v014_serve_reason_slots[i].absolute_address);
        uint32_t length = 0;
        const uint16_t* chars = NULL;
        output[i].use = v014_serve_reason_slots[i].use;
        output[i].slot_address = v014_serve_reason_slots[i].absolute_address;
        output[i].object = object;
        output[i].length = 0;
        output[i].chars = NULL;
        output[i].slot_read_ok = object != NULL;
        output[i].string_read_ok = 0;
        if (object && read_managed_string(object, &length, &chars)) {
            output[i].length = length;
            output[i].chars = chars;
            output[i].string_read_ok = chars != NULL || length == 0;
        }
        if (!output[i].slot_read_ok || !output[i].string_read_ok) ok = 0;
    }
    return ok;
}

/* The RVAs are provenance labels only.  Method pointers must be resolved by
 * metadata_method_checked for the current process; callers must not add the
 * RVA to an assumed module base. */
static const V014ServeMethodSpec v014_serve_methods[] = {
    /* MustLetServeBounce first invokes UnityEngine.Object.op_Equality on the
     * player and null.  Keep this identity in the fixture contract so a
     * managed Player.Team read-back is not mistaken for an alive Unity object. */
    {"Object", "op_Equality", 2, 0x0524a260U, "System.Boolean"},
    {"TennisGameManager", "GetServeTimingWindows", 3, 0x000c31690U,
     "System.Void"},
    {"TennisGameManager", "get_ServeTimingForgivenessActive", 0,
     0x000c41850U, "System.Boolean"},
    {"TennisGameManager", "ResetServeTimingForgiveness", 0, 0x000c3a570U,
     "System.Void"},
    {"TennisGameManager", "NoteAceStreak", 1, 0x000c342c0U,
     "System.Void"},
    {"TennisGameManager", "NoteDoubleFaultStreak", 1, 0x000c34310U,
     "System.Void"},
    {"TennisGameManager", "NoteFoulStreak", 2, 0x000c34350U,
     "System.Void"},
    {"TennisGameManager", "BeginServe", 1, 0x000c28400U,
     "System.Void"},
    {"TennisGameManager", "ResetMatch", 0, 0x000c3a0b0U,
     "System.Void"},
    {"TennisGameManager", "HandleServeFault", 1, 0x000c32990U,
     "System.Void"},
    {"TennisGameManager", "AwardPoint", 4, 0x000c276b0U,
     "System.Void"},
    {"TennisGameManager", "RecordPointStats", 3, 0x000c392c0U,
     "System.Void"},
    {"TennisGameManager", "MustLetServeBounce", 1, 0x000c341e0U,
     "System.Boolean"},
    {"TennisGameManager", "OnBallStruck", 1, 0x000c35310U,
     "System.Void"},
    {"TennisGameManager", "OnBallBounced", 4, 0x000c344f0U,
     "System.Void"},
    {"TennisGameManager", "OnBallHitNet", 1, 0x000c34e10U,
     "System.Void"},
    {"TennisGameManager", "OnBallOut", 2, 0x000c35030U,
     "System.Void"},
    {"TennisGameManager", "IsInCourtXZ", 2, 0x000c335b0U,
     "System.Boolean"},
    {"TennisGameManager", "IsInDiagonalServiceBox", 1, 0x000c337b0U,
     "System.Boolean"},
    {"TennisGameManager", "GetDiagonalServiceBox", 5, 0x000c2ea50U,
     "System.Void"},
    {"TennisGameManager", "get_CourtCenter", 0, 0x000c40c40U,
     "UnityEngine.Vector3"},
    {"TennisGameManager", "get_CourtLength", 0, 0x000c40da0U,
     "System.Single"},
    {"TennisGameManager", "get_CourtSinglesWidth", 0, 0x000c40ec0U,
     "System.Single"},
    {"TennisGameManager", "get_CourtLineWidth", 0, 0x000c40e30U,
     "System.Single"},
    {"TennisGameManager", "get_CourtY", 0, 0x000aa02e0U,
     "System.Single"},
    {"TennisGameManager", "get_BallScale", 0, 0x000c40bf0U,
     "System.Single"},
    {"TennisGameManager", "get_OutMargin", 0, 0x000c415b0U,
     "System.Single"},
    {"TennisScore", "AwardPoint", 1, 0x000c68e90U, "System.Void"},
    {"TennisScore", "get_IsDecidingSet", 0, 0x000c69540U,
     "System.Boolean"},
    {"TennisPlayer", "GetServeTimingThresholds", 3, 0x000c5a1c0U,
     "System.Void"},
    {"TennisPlayer", "GradeServeTimingAtVy", 2, 0x000c5ac80U,
     "TennisContactTiming"},
    {"TennisPlayer", "GradeServeTiming", 2, 0x000c5ae10U,
     "TennisContactTiming"},
    {"TennisPlayer", "ApplyServeTiming", 4, 0x000c55600U,
     "System.Void"}
};

typedef struct V014ServeRawField {
    unsigned int words[2];
    uint8_t valid;
} V014ServeRawField;

#define V014_SERVE_MANAGER_FIELD_CAPACITY \
    (sizeof(v014_serve_manager_fields) / sizeof(v014_serve_manager_fields[0]))
#define V014_SERVE_SCORE_FIELD_CAPACITY \
    (sizeof(v014_serve_score_fields) / sizeof(v014_serve_score_fields[0]))
#define V014_SERVE_STATS_FIELD_CAPACITY \
    (sizeof(v014_serve_stats_fields) / sizeof(v014_serve_stats_fields[0]))
#define V014_SERVE_BALL_FIELD_CAPACITY \
    (sizeof(v014_serve_ball_fields) / sizeof(v014_serve_ball_fields[0]))

typedef struct V014ServeObjectSnapshot {
    void* object;
    uint8_t object_present;
    V014ServeRawField manager[V014_SERVE_MANAGER_FIELD_CAPACITY];
    V014ServeRawField score[V014_SERVE_SCORE_FIELD_CAPACITY];
    V014ServeRawField stats[V014_SERVE_STATS_FIELD_CAPACITY];
} V014ServeObjectSnapshot;

typedef struct V014ServeBallSnapshot {
    void* object;
    uint8_t object_present;
    V014ServeRawField ball[V014_SERVE_BALL_FIELD_CAPACITY];
} V014ServeBallSnapshot;

/* A snapshot is intentionally split by object.  `manager` and `score` are
 * required for a score fixture; player stats may be absent in a startup-only
 * fixture and are then restored only when their object was captured. */
typedef struct V014ServeFixtureSnapshot {
    V014ServeObjectSnapshot manager;
    V014ServeObjectSnapshot score;
    V014ServeObjectSnapshot home_stats;
    V014ServeObjectSnapshot away_stats;
    V014ServeBallSnapshot ball;
} V014ServeFixtureSnapshot;

static inline size_t v014_serve_field_count(const V014ServeFieldSpec* fields,
                                            size_t count) {
    (void)fields;
    return count;
}

static inline int v014_serve_capture_object(
    const V014ServeFixtureOps* ops, void* klass, void* object,
    const V014ServeFieldSpec* specs, size_t count, V014ServeRawField* raw,
    int required, void** object_slot, uint8_t* present_slot) {
    size_t i;
    int ok = 1;
    if (!ops || !ops->read_words || !object_slot || !present_slot) return 0;
    *object_slot = object;
    *present_slot = object != NULL && klass != NULL;
    if (!*present_slot) return required ? 0 : 1;
    for (i = 0; i < count; ++i) {
        raw[i].words[0] = 0;
        raw[i].words[1] = 0;
        raw[i].valid = (uint8_t)ops->read_words(
            klass, object, specs[i].managed_name, raw[i].words,
            (unsigned int)specs[i].words);
        if (!raw[i].valid && required && specs[i].required) ok = 0;
    }
    return ok;
}

static inline int v014_serve_restore_object(
    const V014ServeFixtureOps* ops, void* klass, V014ServeRawField* raw,
    const V014ServeFieldSpec* specs, size_t count, void* object,
    uint8_t present, int required) {
    size_t i;
    int ok = 1;
    if (!present) return required ? 0 : 1;
    if (!ops || !ops->write_words || !klass || !object) return 0;
    for (i = 0; i < count; ++i) {
        if (!raw[i].valid) {
            if (required && specs[i].required) ok = 0;
            continue;
        }
        if (!ops->write_words(klass, object, specs[i].managed_name,
                              raw[i].words, (unsigned int)specs[i].words))
            ok = 0;
    }
    return ok;
}

static inline int v014_serve_fixture_snapshot_capture(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* score_class, void* score, void* stats_class, void* home_stats,
    void* away_stats, V014ServeFixtureSnapshot* snapshot) {
    int ok;
    if (!snapshot) return 0;
    memset(snapshot, 0, sizeof(*snapshot));
    ok = v014_serve_capture_object(
        ops, manager_class, manager, v014_serve_manager_fields,
        V014_SERVE_MANAGER_FIELD_CAPACITY, snapshot->manager.manager, 1,
        &snapshot->manager.object, &snapshot->manager.object_present);
    if (!v014_serve_capture_object(
            ops, score_class, score, v014_serve_score_fields,
            V014_SERVE_SCORE_FIELD_CAPACITY, snapshot->score.score, 1,
            &snapshot->score.object, &snapshot->score.object_present))
        ok = 0;
    if (!v014_serve_capture_object(
            ops, stats_class, home_stats, v014_serve_stats_fields,
            V014_SERVE_STATS_FIELD_CAPACITY, snapshot->home_stats.stats, 0,
            &snapshot->home_stats.object, &snapshot->home_stats.object_present))
        ok = 0;
    if (!v014_serve_capture_object(
            ops, stats_class, away_stats, v014_serve_stats_fields,
            V014_SERVE_STATS_FIELD_CAPACITY, snapshot->away_stats.stats, 0,
            &snapshot->away_stats.object, &snapshot->away_stats.object_present))
        ok = 0;
    return ok;
}

static inline int v014_serve_fixture_snapshot_restore(
    const V014ServeFixtureOps* ops, void* manager_class, void* score_class,
    void* stats_class, V014ServeFixtureSnapshot* snapshot) {
    int ok = 1;
    if (!snapshot) return 0;
    if (!v014_serve_restore_object(
            ops, manager_class, snapshot->manager.manager,
            v014_serve_manager_fields, V014_SERVE_MANAGER_FIELD_CAPACITY,
            snapshot->manager.object, snapshot->manager.object_present, 1))
        ok = 0;
    if (!v014_serve_restore_object(
            ops, score_class, snapshot->score.score,
            v014_serve_score_fields, V014_SERVE_SCORE_FIELD_CAPACITY,
            snapshot->score.object, snapshot->score.object_present, 1))
        ok = 0;
    if (!v014_serve_restore_object(
            ops, stats_class, snapshot->home_stats.stats,
            v014_serve_stats_fields, V014_SERVE_STATS_FIELD_CAPACITY,
            snapshot->home_stats.object, snapshot->home_stats.object_present, 0))
        ok = 0;
    if (!v014_serve_restore_object(
            ops, stats_class, snapshot->away_stats.stats,
            v014_serve_stats_fields, V014_SERVE_STATS_FIELD_CAPACITY,
            snapshot->away_stats.object, snapshot->away_stats.object_present, 0))
        ok = 0;
    return ok;
}

/* Ball state is optional for the older timing/score fixtures.  Event cases
 * should use these paired helpers so the same managed TennisBall (including
 * its LastHitter object reference) is restored before the next branch. */
static inline int v014_serve_fixture_snapshot_capture_with_ball(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* score_class, void* score, void* stats_class, void* home_stats,
    void* away_stats, void* ball_class, void* ball,
    V014ServeFixtureSnapshot* snapshot) {
    int ok = v014_serve_fixture_snapshot_capture(
        ops, manager_class, manager, score_class, score, stats_class,
        home_stats, away_stats, snapshot);
    if (!snapshot) return 0;
    if (!v014_serve_capture_object(
            ops, ball_class, ball, v014_serve_ball_fields,
            V014_SERVE_BALL_FIELD_CAPACITY, snapshot->ball.ball, 1,
            &snapshot->ball.object, &snapshot->ball.object_present))
        ok = 0;
    return ok;
}

static inline int v014_serve_fixture_snapshot_restore_with_ball(
    const V014ServeFixtureOps* ops, void* manager_class, void* score_class,
    void* stats_class, void* ball_class, V014ServeFixtureSnapshot* snapshot) {
    int ok = v014_serve_fixture_snapshot_restore(
        ops, manager_class, score_class, stats_class, snapshot);
    if (!snapshot) return 0;
    if (!v014_serve_restore_object(
            ops, ball_class, snapshot->ball.ball, v014_serve_ball_fields,
            V014_SERVE_BALL_FIELD_CAPACITY, snapshot->ball.object,
            snapshot->ball.object_present, 1))
        ok = 0;
    return ok;
}

static inline const V014ServeMethodSpec* v014_serve_method_spec(
    const char* declaring_class, const char* name, uint32_t args) {
    size_t i;
    for (i = 0; i < sizeof(v014_serve_methods) / sizeof(v014_serve_methods[0]);
         ++i) {
        const V014ServeMethodSpec* spec = &v014_serve_methods[i];
        if (spec->args == args && strcmp(spec->declaring_class,
                                         declaring_class) == 0 &&
            strcmp(spec->name, name) == 0)
            return spec;
    }
    return NULL;
}

/* Invoke GetServeTimingWindows while retaining output words exactly.  The
 * caller should capture/restore the manager snapshot around this call. */
static inline int v014_serve_fixture_get_timing_windows(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int output_words[3], void** exception_out) {
    float perfect = 0.0f;
    float good = 0.0f;
    float maximum = 0.0f;
    void* args[3] = {&perfect, &good, &maximum};
    const V014ServeMethodSpec* spec =
        v014_serve_method_spec("TennisGameManager", "GetServeTimingWindows", 3);
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_void || !output_words ||
        !spec)
        return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_void(method, manager, args, exception_out);
    if (!ok) return 0;
    memcpy(&output_words[0], &perfect, sizeof(perfect));
    memcpy(&output_words[1], &good, sizeof(good));
    memcpy(&output_words[2], &maximum, sizeof(maximum));
    return 1;
}

/* Invoke the scalar forgiveness getter.  `output_words[0]` is its raw boxed
 * Boolean word; this does not coerce it to a C bool. */
static inline int v014_serve_fixture_get_forgiveness_active(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int output_words[1], void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "get_ServeTimingForgivenessActive", 0);
    void* method;
    if (!ops || !ops->method_checked || !ops->invoke_words || !output_words ||
        !spec)
        return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, manager, NULL, output_words, 1,
                             exception_out);
}

static inline int v014_serve_fixture_invoke_noargs(
    const V014ServeFixtureOps* ops, void* klass, void* object,
    const char* name, uint32_t expected_args, void** exception_out) {
    const V014ServeMethodSpec* spec =
        v014_serve_method_spec("TennisGameManager", name, expected_args);
    void* method;
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec) return 0;
    method = ops->method_checked(klass, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, object, NULL, exception_out);
}

/* These wrappers pass raw 32-bit argument storage to the existing metadata
 * invoke API.  Snapshot/restore belongs around each call, including a reset
 * method, so a later case cannot inherit mutated counters. */
static inline int v014_serve_fixture_note_bool(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    const char* name, unsigned int bool_word, void** exception_out) {
    const V014ServeMethodSpec* spec =
        v014_serve_method_spec("TennisGameManager", name, 1);
    void* method;
    void* args[1] = {&bool_word};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec) return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

static inline int v014_serve_fixture_note_foul(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int is_foul_word, unsigned int fouling_team_word,
    void** exception_out) {
    const V014ServeMethodSpec* spec =
        v014_serve_method_spec("TennisGameManager", "NoteFoulStreak", 2);
    void* method;
    void* args[2] = {&is_foul_word, &fouling_team_word};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec) return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

static inline int v014_serve_fixture_note_ace(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int is_ace_word, void** exception_out) {
    return v014_serve_fixture_note_bool(
        ops, manager_class, manager, "NoteAceStreak", is_ace_word,
        exception_out);
}

static inline int v014_serve_fixture_note_double_fault(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int is_double_fault_word, void** exception_out) {
    return v014_serve_fixture_note_bool(
        ops, manager_class, manager, "NoteDoubleFaultStreak",
        is_double_fault_word, exception_out);
}

static inline int v014_serve_fixture_reset_timing_forgiveness(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void** exception_out) {
    return v014_serve_fixture_invoke_noargs(
        ops, manager_class, manager, "ResetServeTimingForgiveness", 0,
        exception_out);
}

/* ResetMatch is included as a separate fixture operation because it is a
 * match-boundary reset, not the point-local BeginServe reset.  The caller
 * should snapshot all relevant objects before invoking it and restore them
 * before the next case. */
static inline int v014_serve_fixture_reset_match(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void** exception_out) {
    return v014_serve_fixture_invoke_noargs(
        ops, manager_class, manager, "ResetMatch", 0, exception_out);
}

/* Score helpers are kept raw for capture: the Boolean getter returns its
 * native result word and AwardPoint accepts the managed int argument without
 * imposing a host-side score model. */
static inline int v014_serve_fixture_score_is_deciding_set(
    const V014ServeFixtureOps* ops, void* score_class, void* score,
    unsigned int output_words[1], void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisScore", "get_IsDecidingSet", 0);
    void* method;
    if (!ops || !ops->method_checked || !ops->invoke_words || !output_words ||
        !spec)
        return 0;
    method = ops->method_checked(score_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, score, NULL, output_words, 1,
                             exception_out);
}

static inline int v014_serve_fixture_score_award_point(
    const V014ServeFixtureOps* ops, void* score_class, void* score,
    unsigned int winner_team_word, void** exception_out) {
    const V014ServeMethodSpec* spec =
        v014_serve_method_spec("TennisScore", "AwardPoint", 1);
    void* method;
    void* args[1] = {&winner_team_word};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec) return 0;
    method = ops->method_checked(score_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, score, args, exception_out);
}

/* Manager AwardPoint/HandleServeFault wrappers accept the actual managed
 * String object pointer.  The capture owner obtains that pointer from the
 * initialized static slot (or from a callback argument); this layer never
 * manufactures a reason label. */
static inline int v014_serve_fixture_manager_handle_fault(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* reason_string, void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "HandleServeFault", 1);
    void* method;
    void* reason_argument = reason_string;
    void* args[1] = {&reason_argument};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec) return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

static inline int v014_serve_fixture_manager_award_point(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int winner_word, void* reason_string,
    const unsigned int score_location_words[3],
    const unsigned int ball_velocity_words[3], void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "AwardPoint", 4);
    void* method;
    unsigned int winner = winner_word;
    void* reason_argument = reason_string;
    float score_location[3];
    float ball_velocity[3];
    void* args[4] = {&winner, &reason_argument, score_location, ball_velocity};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec ||
        !score_location_words || !ball_velocity_words)
        return 0;
    memcpy(score_location, score_location_words, sizeof(score_location));
    memcpy(ball_velocity, ball_velocity_words, sizeof(ball_velocity));
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

/* `MustLetServeBounce` uses Unity's overloaded Object == null check before it
 * reads Player.Team.  A direct capture may invoke this static operator with
 * the same managed player object used by the manager callback.  The result is
 * raw so a destroyed/fake-null Unity object remains distinguishable from a
 * Team mismatch. */
static inline int v014_serve_fixture_unity_object_is_null(
    const V014ServeFixtureOps* ops, void* object_class, void* object,
    unsigned int output_words[1], void** exception_out) {
    const V014ServeMethodSpec* spec =
        v014_serve_method_spec("Object", "op_Equality", 2);
    void* method;
    void* left = object;
    void* right = NULL;
    void* args[2] = {&left, &right};
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !output_words)
        return 0;
    method = ops->method_checked(object_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, NULL, args, output_words, 1,
                             exception_out);
}

/* MustLetServeBounce is exposed for a capture to record the native predicate
 * inputs/output alongside OnBallStruck.  The production adapter should derive
 * its own result from the manager/ball state; this wrapper never accepts an
 * expected Boolean. */
static inline int v014_serve_fixture_must_let_serve_bounce(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* player, unsigned int output_words[1], void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "MustLetServeBounce", 1);
    void* method;
    void* player_argument = player;
    void* args[1] = {&player_argument};
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !output_words)
        return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, manager, args, output_words, 1,
                             exception_out);
}

/* The event callbacks are intentionally exposed as direct metadata invokes.
 * Their effects are read from the restored manager/ball/stats snapshots by
 * the capture owner; this layer does not inject a reason, winner, or random
 * result.  Vector3 arguments are passed as their native three-word value. */
static inline int v014_serve_fixture_on_ball_struck(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* player, void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "OnBallStruck", 1);
    void* method;
    void* player_argument = player;
    void* args[1] = {&player_argument};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec) return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

static inline int v014_serve_fixture_on_ball_bounced(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* ball, unsigned int half_word, const unsigned int point_words[3],
    unsigned int impact_speed_word, void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "OnBallBounced", 4);
    void* method;
    void* ball_argument = ball;
    unsigned int half = half_word;
    float point[3];
    float impact_speed;
    void* args[4] = {&ball_argument, &half, point, &impact_speed};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec ||
        !point_words)
        return 0;
    memcpy(point, point_words, sizeof(point));
    memcpy(&impact_speed, &impact_speed_word, sizeof(impact_speed));
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

static inline int v014_serve_fixture_on_ball_hit_net(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* ball, void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "OnBallHitNet", 1);
    void* method;
    void* ball_argument = ball;
    void* args[1] = {&ball_argument};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec) return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

static inline int v014_serve_fixture_on_ball_out(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    void* ball, const unsigned int point_words[3], void** exception_out) {
    const V014ServeMethodSpec* spec =
        v014_serve_method_spec("TennisGameManager", "OnBallOut", 2);
    void* method;
    void* ball_argument = ball;
    float point[3];
    void* args[2] = {&ball_argument, point};
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec ||
        !point_words)
        return 0;
    memcpy(point, point_words, sizeof(point));
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_void(method, manager, args, exception_out);
}

/* Court predicates are exposed as raw metadata calls rather than reimplemented
 * in the fixture.  In particular, IsInDiagonalServiceBox obtains its serving
 * half and configured box from the manager; a caller must not replace that
 * guard with a hand-written rectangle. */
static inline int v014_serve_fixture_is_in_court_xz(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    const unsigned int point_words[3], unsigned int extra_margin_word,
    unsigned int output_words[1], void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "IsInCourtXZ", 2);
    void* method;
    float point[3];
    float extra_margin;
    void* args[2] = {point, &extra_margin};
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !point_words || !output_words)
        return 0;
    memcpy(point, point_words, sizeof(point));
    memcpy(&extra_margin, &extra_margin_word, sizeof(extra_margin));
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, manager, args, output_words, 1,
                             exception_out);
}

static inline int v014_serve_fixture_is_in_diagonal_service_box(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    const unsigned int point_words[3], unsigned int output_words[1],
    void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "IsInDiagonalServiceBox", 1);
    void* method;
    float point[3];
    void* args[1] = {point};
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !point_words || !output_words)
        return 0;
    memcpy(point, point_words, sizeof(point));
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, manager, args, output_words, 1,
                             exception_out);
}

/* GetDiagonalServiceBox has four ref Single outputs.  The output words are
 * intentionally copied back without converting through host formatting. */
static inline int v014_serve_fixture_get_diagonal_service_box(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int serving_team_word, unsigned int output_words[4],
    void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "GetDiagonalServiceBox", 5);
    void* method;
    unsigned int serving_team = serving_team_word;
    float min_x = 0.0f;
    float max_x = 0.0f;
    float min_z = 0.0f;
    float max_z = 0.0f;
    void* args[5] = {&serving_team, &min_x, &max_x, &min_z, &max_z};
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_void || !spec ||
        !output_words)
        return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_void(method, manager, args, exception_out);
    if (!ok) return 0;
    memcpy(&output_words[0], &min_x, sizeof(min_x));
    memcpy(&output_words[1], &max_x, sizeof(max_x));
    memcpy(&output_words[2], &min_z, sizeof(min_z));
    memcpy(&output_words[3], &max_z, sizeof(max_z));
    return 1;
}

/* Scalar/vector getters are useful for proving the rectangle inputs in the
 * same initialized scene as the event call.  The method name is constrained
 * to the reviewed v014 method table; no RVA lookup is performed here. */
static inline int v014_serve_fixture_get_geometry_scalar(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    const char* getter_name, unsigned int output_words[1],
    void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", getter_name, 0);
    void* method;
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !output_words)
        return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, manager, NULL, output_words, 1,
                             exception_out);
}

static inline int v014_serve_fixture_get_court_center(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    unsigned int output_words[3], void** exception_out) {
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisGameManager", "get_CourtCenter", 0);
    void* method;
    if (!ops || !ops->method_checked || !ops->invoke_words || !spec ||
        !output_words)
        return 0;
    method = ops->method_checked(manager_class, spec->name, spec->args);
    if (!method) return 0;
    return ops->invoke_words(method, manager, NULL, output_words, 3,
                             exception_out);
}

typedef struct V014ServeGeometryFieldWords {
    unsigned int court_length;
    unsigned int singles_width;
    unsigned int out_margin;
    unsigned int ball_scale;
    uint8_t court_length_valid;
    uint8_t singles_width_valid;
    uint8_t out_margin_valid;
    uint8_t ball_scale_valid;
} V014ServeGeometryFieldWords;

/* These are separate from the gameplay snapshot because a missing optional
 * scene/config field must not make an event fixture unusable. */
static inline int v014_serve_fixture_read_geometry_fields(
    const V014ServeFixtureOps* ops, void* manager_class, void* manager,
    V014ServeGeometryFieldWords* output) {
    if (!ops || !ops->read_words || !output) return 0;
    memset(output, 0, sizeof(*output));
    output->court_length_valid = (uint8_t)ops->read_words(
        manager_class, manager, "courtLength", &output->court_length, 1);
    output->singles_width_valid = (uint8_t)ops->read_words(
        manager_class, manager, "singlesWidth", &output->singles_width, 1);
    output->out_margin_valid = (uint8_t)ops->read_words(
        manager_class, manager, "outMargin", &output->out_margin, 1);
    output->ball_scale_valid = (uint8_t)ops->read_words(
        manager_class, manager, "ballScale", &output->ball_scale, 1);
    return output->court_length_valid && output->singles_width_valid &&
           output->out_margin_valid && output->ball_scale_valid;
}

/* GradeServeTimingAtVy takes a by-value float and a by-reference severity;
 * the return is a boxed enum word.  All three outputs are raw words in the
 * result packet so the launcher can compare native bit patterns. */
static inline int v014_serve_fixture_grade_timing_at_vy(
    const V014ServeFixtureOps* ops, void* player_class, void* player,
    unsigned int vy_word, unsigned int output_words[2],
    void** exception_out) {
    float vy;
    float severity = 0.0f;
    void* args[2] = {&vy, &severity};
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisPlayer", "GradeServeTimingAtVy", 2);
    void* method;
    int ok;
    memcpy(&vy, &vy_word, sizeof(vy));
    if (!ops || !ops->method_checked || !ops->invoke_words || !output_words ||
        !spec)
        return 0;
    method = ops->method_checked(player_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_words(method, player, args, output_words, 1,
                           exception_out);
    memcpy(&output_words[1], &severity, sizeof(severity));
    return ok;
}

static inline int v014_serve_fixture_grade_timing(
    const V014ServeFixtureOps* ops, void* player_class, void* player,
    void* ball, unsigned int output_words[2], void** exception_out) {
    float severity = 0.0f;
    void* ball_argument = ball;
    void* args[2] = {&ball_argument, &severity};
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisPlayer", "GradeServeTiming", 2);
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_words || !output_words ||
        !spec)
        return 0;
    method = ops->method_checked(player_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_words(method, player, args, output_words, 1,
                           exception_out);
    memcpy(&output_words[1], &severity, sizeof(severity));
    return ok;
}

/* GetServeTimingThresholds is a useful wrapper-level control: it confirms
 * which manager output reaches GradeServeTiming without assuming defaults. */
static inline int v014_serve_fixture_get_player_thresholds(
    const V014ServeFixtureOps* ops, void* player_class, void* player,
    unsigned int output_words[3], void** exception_out) {
    float nice = 0.0f;
    float good = 0.0f;
    float maximum = 0.0f;
    void* args[3] = {&nice, &good, &maximum};
    const V014ServeMethodSpec* spec = v014_serve_method_spec(
        "TennisPlayer", "GetServeTimingThresholds", 3);
    void* method;
    int ok;
    if (!ops || !ops->method_checked || !ops->invoke_void || !output_words ||
        !spec)
        return 0;
    method = ops->method_checked(player_class, spec->name, spec->args);
    if (!method) return 0;
    ok = ops->invoke_void(method, player, args, exception_out);
    if (!ok) return 0;
    memcpy(&output_words[0], &nice, sizeof(nice));
    memcpy(&output_words[1], &good, sizeof(good));
    memcpy(&output_words[2], &maximum, sizeof(maximum));
    return 1;
}

/* A compact case list for build_capture.  The values are inputs only; the
 * expected output fields are intentionally absent. */
typedef struct V014ServeTimingCase {
    const char* id;
    int32_t double_fault_points;
    uint32_t double_fault_team_mask;
    int32_t forgiveness_threshold;
    int32_t expansion_denominator;
} V014ServeTimingCase;

static const V014ServeTimingCase v014_serve_timing_cases[] = {
    {"inactive-below-threshold", 5, 3, 6, 6},
    {"active-at-threshold", 6, 3, 6, 6},
    {"active-after-one", 7, 3, 6, 6},
    {"active-missing-team", 6, 1, 6, 6},
    {"threshold-floor", 1, 3, 0, 0}
};

#define V014_SERVE_TIMING_CASE_COUNT \
    (sizeof(v014_serve_timing_cases) / sizeof(v014_serve_timing_cases[0]))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* V014_SERVE_FIXTURES_V014_H */
