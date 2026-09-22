/*
 * Build-specific, read-only IL2CPP metadata discovery for Tennis v0.14.
 *
 * This file is included only by v014/bootstrap.c with V014_METADATA.  It uses
 * the candidate API order generated from the v0.14 resolver capture, then
 * validates the APIs by enumerating the live domain and managed images.  No
 * v0.12 RVA, vtable slot, object field offset, or call-site hook is used.
 */
#define V014_FN(name,result,...) ((result (*)(__VA_ARGS__))resolved[API_##name])

static QWORD metadata_module_base;
static QWORD metadata_module_size;
static unsigned int metadata_assembly_count;
static unsigned int metadata_catalog_count;
static unsigned int metadata_selected_count;
static unsigned int metadata_method_count;
static unsigned int metadata_field_count;
static int metadata_layout_written;
static int metadata_sweep_done;
static int metadata_sweep_wait_logged;
static int metadata_spawn_requested;
static int metadata_capture_complete;
static int metadata_physics_done;
static int metadata_physics_success;
static int metadata_physics_wait_logged;
static unsigned int metadata_physics_rows;
static unsigned int metadata_fatigue_rows;
static unsigned int metadata_force_rows;
static unsigned int metadata_effective_rows;
static unsigned int metadata_integrate_rows;
static void* metadata_core_image;
/* Assembly-CSharp is retained only as a metadata image handle.  Natural
 * traces use it to resolve GameManager.Instance by managed name; no native
 * field/RVA is embedded here. */
static void* metadata_game_image;
static void* metadata_core_time_class;
static void* metadata_core_object_class;
#if defined(V014_EVENT_CAPTURE) || defined(V014_NATURAL_TRACE)
static void* metadata_core_random_class;
#endif
#ifdef V014_EVENT_CAPTURE
static int metadata_event_done;
static int metadata_event_success;
/* Getter/serve context are terminal prerequisites for the event fixture.  A
 * retry is deliberately distinct from a failed terminal capture so the
 * event rows cannot be reported as complete before the live graph state is
 * known. */
static int metadata_event_getter_terminal;
static int metadata_event_getter_success;
static int metadata_event_serve_context_done;
static unsigned int metadata_event_rows;
static unsigned int metadata_simulate_rows;
static unsigned int metadata_nth_landing_rows;
static int metadata_shot_done;
static int metadata_shot_success;
static unsigned int metadata_shot_case_rows;
static unsigned int metadata_shot_call_rows;
static int metadata_serve_direct_done;
static int metadata_serve_direct_success;
static unsigned int metadata_serve_direct_rows;
static int metadata_curve_done;
static int metadata_curve_success;
static unsigned int metadata_curve_rows;
#endif
#ifdef V014_NATURAL_TRACE
static int metadata_natural_active;
static int metadata_natural_started;
static int metadata_natural_done;
static int metadata_natural_success;
static unsigned int metadata_natural_trace_seq;
static unsigned int metadata_natural_tick;
static unsigned int metadata_natural_callbacks;
/* Stamina.OnSimulationTick is called directly from the native simulation
 * driver, so il2cpp_runtime_invoke observation cannot see it.  Resolve its
 * MethodInfo by managed identity and install a guarded entry detour only for
 * this natural-trace process.  The target preamble is checked by INSTRUCTION
 * SHAPE (the guard load's disp32 is a build-specific static-field address and
 * is relocated, not matched) so the same probe works on v0.14 and v0.15f. */
typedef void (__stdcall *metadata_natural_stamina_fn)(void*,void*);
static void* metadata_natural_stamina_class;
static void* metadata_natural_stamina_method;
static void* metadata_natural_stamina_target;
static void* metadata_natural_stamina_trampoline;
static metadata_natural_stamina_fn metadata_natural_stamina_original;
static int metadata_natural_stamina_hook_attempted;
static int metadata_natural_stamina_hooked;
static unsigned int metadata_natural_stamina_callbacks;
static const char* metadata_natural_stamina_hook_reason="not_attempted";
/* Mover and Stamina boundaries are selected in r6 in addition to the r5
 * graph/player callbacks.  Keep the stop bounded while leaving enough room
 * for both owned players to reach the first naturally resolved point. */
static unsigned int metadata_natural_budget=8192;
static void* metadata_natural_manager;
static void* metadata_natural_manager_class;
static void* metadata_natural_game_manager;
static void* metadata_natural_game_manager_class;
#endif /* V014_NATURAL_TRACE */
#ifdef PARITYMOD
/* ---- ParityMod block A: config state + loader. Self-contained: uses only
 * bootstrap.c primitives (same/write_text/flush_log/file APIs) so it can
 * live at the top of this header. Proven probe behavior is unchanged when
 * PARITYMOD is not defined. */
static char parity_home_save[128]={"safe_corner_v02"};
static char parity_away_save[128]={"safe_corner_v02"};
static int parity_seed=20260907;
static unsigned int parity_points_target=1;
static unsigned int parity_restart_epoch=0;
static unsigned int parity_last_snapshot_tick=0;
static unsigned int parity_cmd_seen_wtime_lo=0;
static unsigned int parity_cmd_seen_wtime_hi=0;
static int parity_cmd_have_time=0;
typedef struct { DWORD attrs; DWORD c_lo; DWORD c_hi; DWORD a_lo; DWORD a_hi;
    DWORD w_lo; DWORD w_hi; DWORD size_hi; DWORD size_lo; } parity_fad_t;
static void parity_emit_uint(unsigned int value) {
    char buf[12];int at=11;buf[at]=0;
    do { buf[--at]=(char)('0'+value%10);value/=10; } while(value);
    write_text(buf+at);
}
static void parity_emit_int(int value) {
    long long wide=value;
    if(wide<0) { write_text("-");wide=-wide; }
    parity_emit_uint((unsigned int)wide);
}
static int parity_read_file(const wchar_t* name,char* buf,unsigned int cap,
    unsigned int* len_out) {
    HANDLE h=CreateFileW(name,0x80000000,1,0,3,0x80,0);
    DWORD got=0;
    if(h==(HANDLE)-1) return 0;
    if(!ReadFile(h,buf,cap-1,&got,0)) { CloseHandle(h);return 0; }
    CloseHandle(h);
    buf[got]=0;if(len_out) *len_out=got;
    return 1;
}
static const char* parity_find(const char* buf,const char* key) {
    unsigned int kl=0;
    if(!buf||!key) return 0;
    while(key[kl]) ++kl;
    for(unsigned int i=0;buf[i];++i) {
        unsigned int k=0;while(k<kl&&buf[i+k]==key[k]) ++k;
        if(k==kl) return buf+i+kl;
    }
    return 0;
}
static void parity_copy_str(const char* p,char* dst,unsigned int cap) {
    unsigned int n=0;
    if(!p||!dst||!cap) return;
    while(*p&&*p!='"') ++p;
    if(*p=='"') ++p;
    while(*p&&*p!='"'&&n+1<cap) dst[n++]=*p++;
    dst[n]=0;
}
static int parity_copy_int(const char* p,int* out) {
    int neg=0;long v=0;int any=0;
    if(!p||!out) return 0;
    while(*p&&((*p<'0'||*p>'9')&&*p!='-')) ++p;
    if(*p=='-') { neg=1;++p; }
    while(*p>='0'&&*p<='9') { v=v*10+(*p-'0');++p;any=1; }
    if(!any) return 0;
    *out=neg?-(int)v:(int)v;return 1;
}
static void parity_config_load(void) {
    char buf[1024];const char* p;int v=0;
    if(parity_read_file(L"paritymod.json",buf,sizeof(buf),0)) {
        p=parity_find(buf,"\"home\"");
        if(p) parity_copy_str(p,parity_home_save,sizeof(parity_home_save));
        p=parity_find(buf,"\"away\"");
        if(p) parity_copy_str(p,parity_away_save,sizeof(parity_away_save));
        p=parity_find(buf,"\"seed\"");
        if(p&&parity_copy_int(p,&v)) parity_seed=v;
        p=parity_find(buf,"\"points\"");
        if(p&&parity_copy_int(p,&v)&&v>0) parity_points_target=(unsigned int)v;
#ifdef V014_NATURAL_TRACE
        p=parity_find(buf,"\"budget\"");
        if(p&&parity_copy_int(p,&v)&&v>0) metadata_natural_budget=(unsigned int)v;
#endif /* V014_NATURAL_TRACE */
    }
    write_text("{\"kind\":\"parity_config\",\"home\":\"");write_text(parity_home_save);
    write_text("\",\"away\":\"");write_text(parity_away_save);
    write_text("\",\"seed\":");parity_emit_int(parity_seed);
    write_text(",\"points\":");parity_emit_uint(parity_points_target);
#ifdef V014_NATURAL_TRACE
    write_text(",\"budget\":");parity_emit_uint(metadata_natural_budget);
#endif
    write_text("}\n");flush_log();
}
#endif /* PARITYMOD */
static void metadata_quoted(const char*);

static void metadata_number(unsigned int value) {
    char buf[12];
    int at=11;
    buf[at]=0;
    do { buf[--at]=(char)('0'+value%10); value/=10; } while(value);
    write_text(buf+at);
}

static void metadata_write_unsigned_qword(QWORD value) {
    char buf[21];
    int at=20;
    buf[at]=0;
    do { buf[--at]=(char)('0'+value%10); value/=10; } while(value);
    write_text(buf+at);
}

static void metadata_write_signed_decimal(int value) {
    long long wide=value;
    if(wide<0) { write_text("-");wide=-wide; }
    metadata_write_unsigned_qword((QWORD)wide);
}

static void metadata_write_signed_hex(int value) {
    write_hex((QWORD)(long long)value);
}

static void metadata_words(const void* data,unsigned int count) {
    write_text("[");
    for(unsigned int i=0;i<count;++i) {
        if(i) write_text(",");
        metadata_number(((const unsigned int*)data)[i]);
    }
    write_text("]");
}

static void metadata_object_pointer(void* object) {
    if(object) { write_text("\"");write_hex((QWORD)object);write_text("\""); }
    else write_text("null");
}

static void* metadata_invoke(void* method,void* object,void** args,void** exception) {
    if(!method||!real_runtime_invoke) return 0;
    return ((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
        (method,object,args,exception);
}

static void* metadata_find_field(void* klass,const char* name) {
    for(unsigned int depth=0;klass&&depth<8;++depth) {
        void* field=V014_FN(il2cpp_class_get_field_from_name,void*,void*,const char*)
            (klass,name);
        if(field) return field;
        klass=V014_FN(il2cpp_class_get_parent,void*,void*)(klass);
    }
    return 0;
}

static void* metadata_field_object(void* klass,void* object,const char* name) {
    void* field=metadata_find_field(klass,name);void* value=0;
    if(field&&object) V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
        (object,field,&value);
    return value;
}

static int metadata_read_u32_field(void* klass,void* object,const char* name,
                                   unsigned int* value) {
    void* field=metadata_find_field(klass,name);
    if(!field||!object||!value) return 0;
    *value=0;
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,field,value);
    return 1;
}

static int metadata_set_u32_field(void* klass,void* object,const char* name,
                                  unsigned int value) {
    void* field=metadata_find_field(klass,name);
    if(!field||!object) return 0;
    V014_FN(il2cpp_field_set_value,void,void*,void*,void*)(object,field,&value);
    return 1;
}

static int metadata_read_words_field(void* klass,void* object,const char* name,
                                     unsigned int* value,unsigned int words) {
    void* field=metadata_find_field(klass,name);
    if(!field||!object||!value||!words||words>4) return 0;
    for(unsigned int i=0;i<words;++i) value[i]=0;
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,field,value);
    return 1;
}

/* Static GameManager fields must go through the IL2CPP static-value API.
 * Calling il2cpp_field_get_value with the live manager object is valid for
 * instance fields only and previously produced the misleading all-zero
 * Ticks rows in the natural trace. */
static int metadata_read_static_words_field(void* klass,const char* name,
                                            unsigned int* value,unsigned int words,
                                            unsigned int* flags_out) {
    void* field=metadata_find_field(klass,name);
    if(!field||!value||!words||words>4) return 0;
    unsigned int flags=V014_FN(il2cpp_field_get_flags,unsigned int,void*)(field);
    if(flags_out) *flags_out=flags;
    if(!(flags&16)) return 0;
    for(unsigned int i=0;i<words;++i) value[i]=0;
    V014_FN(il2cpp_field_static_get_value,void,void*,void*)(field,value);
    return 1;
}

static void* metadata_read_static_object_field(void* klass,const char* name) {
    void* field=metadata_find_field(klass,name);void* value=0;
    if(field) V014_FN(il2cpp_field_static_get_value,void,void*,void*)(field,&value);
    return value;
}

static int metadata_set_words_field(void* klass,void* object,const char* name,
                                    const unsigned int* value,unsigned int words) {
    void* field=metadata_find_field(klass,name);
    if(!field||!object||!value||!words||words>4) return 0;
    V014_FN(il2cpp_field_set_value,void,void*,void*,void*)(object,field,(void*)value);
    return 1;
}

static void* metadata_method_checked(void* klass,const char* name,
                                     unsigned int expected_args) {
    void* method=klass?V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
        (klass,name,(int)expected_args):0;
    if(!method) return 0;
    if(V014_FN(il2cpp_method_get_param_count,unsigned int,const void*)(method)!=expected_args)
        return 0;
    return method;
}

static int metadata_invoke_words(void* method,void* object,void** args,
                                 unsigned int* words,unsigned int count,
                                 void** exception_out) {
    if(exception_out) *exception_out=0;
    if(!method||!words||!count||count>4) return 0;
    void* exception=0;
    void* result=metadata_invoke(method,object,args,&exception);
    if(exception_out) *exception_out=exception;
    if(!result||exception) return 0;
    void* boxed=V014_FN(il2cpp_object_unbox,void*,void*)(result);
    if(!boxed) return 0;
    for(unsigned int i=0;i<count;++i) words[i]=((const unsigned int*)boxed)[i];
    return 1;
}

static int metadata_invoke_void(void* method,void* object,void** args,
                                void** exception_out) {
    if(exception_out) *exception_out=0;
    if(!method) return 0;
    void* exception=0;
    metadata_invoke(method,object,args,&exception);
    if(exception_out) *exception_out=exception;
    return exception==0;
}

static int metadata_invoke_i32(void* method,void* object,void** args,
                               unsigned int* value,void** exception_out) {
    return metadata_invoke_words(method,object,args,value,1,exception_out);
}

/* Write a raw value obtained through the build's metadata field API.  The
 * names are resolved at runtime; no field offset is embedded in this probe. */
static void metadata_write_raw_field(void* klass,void* object,const char* name,
                                     const char* encoding,unsigned int words) {
    void* field=metadata_find_field(klass,name);
    metadata_quoted(name);write_text(":");
    if(!field||!object) { write_text("null"); return; }
    unsigned int value[4]={0,0,0,0};
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,field,value);
    write_text("{\"encoding\":");metadata_quoted(encoding);
    write_text(",\"words\":");metadata_words(value,words);write_text("}");
}

static void metadata_write_getter(void* klass,void* object,const char* name,unsigned int words) {
    void* method=klass?V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
        (klass,name,0):0;
    void* exception=0;void* result=metadata_invoke(method,object,0,&exception);
    write_text("{\"name\":");metadata_quoted(name);write_text(",\"value\":");
    if(result&&!exception) metadata_words(V014_FN(il2cpp_object_unbox,void*,void*)(result),words);
    else write_text("null");
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text("}");
}

static void metadata_write_player_context(void* player,void* player_class,const char* slot) {
    write_text("{\"slot\":");metadata_quoted(slot);write_text(",\"object\":");
    metadata_object_pointer(player);write_text(",\"team\":");
    unsigned int team=0;void* team_field=metadata_find_field(player_class,"Team");
    if(player&&team_field) {
        V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(player,team_field,&team);
        metadata_words(&team,1);
    } else write_text("null");
    write_text(",\"unity_object_alive\":");
    void* alive_method=metadata_core_object_class?
        V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
            (metadata_core_object_class,"op_Implicit",1):0;
    void* exception=0;void* args[]={player};
    void* alive=player?metadata_invoke(alive_method,0,args,&exception):0;
    if(alive&&!exception) metadata_words(V014_FN(il2cpp_object_unbox,void*,void*)(alive),1);
    else write_text("null");
    write_text(",\"player_is_alive\":");
    void* player_alive_method=player_class?
        V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
            (player_class,"get_IsAlive",0):0;
    exception=0;alive=player?metadata_invoke(player_alive_method,player,0,&exception):0;
    if(alive&&!exception) metadata_words(V014_FN(il2cpp_object_unbox,void*,void*)(alive),1);
    else write_text("null");
    write_text("}");
}

static void metadata_write_runtime_context(void* manager,void* manager_class,
                                           void* home,void* away,
                                           const char* case_id,
                                           const char* mutation) {
    write_text("{\"kind\":\"clamp_context\",\"snapshot_only\":true,");
    if(case_id) {
        write_text("\"case_id\":");metadata_quoted(case_id);
        write_text(",\"snapshot_restore_only\":true,");
    }
    write_text("\"state_mutation\":");metadata_quoted(mutation?mutation:"none");
    write_text(",");
    write_text("\"manager_object\":");metadata_object_pointer(manager);
    write_text(",\"manager_fields\":{");
    const char* field_names[]={"<CurrentGameState>k__BackingField","<ServingTeam>k__BackingField",
        "<IsTrainingMode>k__BackingField","<TrainingPlayerTeam>k__BackingField","pointResolved"};
    const char* field_keys[]={"game_state","serving_team","is_training_mode",
        "training_player_team","point_resolved"};
    for(unsigned int i=0;i<sizeof(field_names)/sizeof(field_names[0]);++i) {
        if(i)write_text(",");write_text("\"");write_text(field_keys[i]);write_text("\":");
        void* field=metadata_find_field(manager_class,field_names[i]);
        unsigned int value=0;
        if(field&&manager) {
            V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(manager,field,&value);
            metadata_words(&value,1);
        } else write_text("null");
    }
    write_text("},\"initialized_config_fields\":{");
    metadata_write_raw_field(manager_class,manager,"fatigueShotSpeedGain","f32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "deuceFatigueGracePoints","i32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "rallyFatigueGraceShots","i32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "coinTossAlternatingFoulStreak","i32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "coinTossAceStreak","i32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "serveTimingForgivenessDoubleFaultStreak","i32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "serveTimingForgivenessExpandOverDoubleFaults","i32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "strikeRadius","f32",1);
    write_text("},\"effective_getters\":[");
    metadata_write_getter(manager_class,manager,"get_CourtCenter",3);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtLength",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtSinglesWidth",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtY",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_StrikeRadius",1);
    write_text("],\"fixed_delta_time\":");
    if(metadata_core_time_class) metadata_write_getter(metadata_core_time_class,0,"get_fixedDeltaTime",1);
    else write_text("null");
    write_text(",\"score\":");
    void* score=metadata_field_object(manager_class,manager,"<Score>k__BackingField");
    metadata_object_pointer(score);
    if(score) {
        void* score_class=V014_FN(il2cpp_object_get_class,void*,void*)(score);
        write_text(",\"score_points_played\":");
        metadata_write_getter(score_class,score,"get_PointsPlayedInGame",1);
        write_text(",\"score_point_fields\":{");
        metadata_write_raw_field(score_class,score,"<HomePoints>k__BackingField","i32",1);
        write_text(",");metadata_write_raw_field(score_class,score,
            "<AwayPoints>k__BackingField","i32",1);
        write_text("}");
    } else write_text(",\"score_points_played\":null");
    write_text(",\"players\":[");
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    metadata_write_player_context(home,home_class,"home");write_text(",");
    metadata_write_player_context(away,away_class,"away");
    write_text("]}\n");
}

static void metadata_spawn_owned_players(void* manager,void* manager_class) {
    if(metadata_spawn_requested) return;
    void* spawn=V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
        (manager_class,"Spawn",1);
    if(!spawn) {
        write_text("{\"kind\":\"owned_spawn\",\"status\":\"missing_method\"}\n");
        metadata_spawn_requested=1;return;
    }
    const char* save_name="safe_corner_v02";
#ifdef PARITYMOD
    const char* parity_slot_saves[2]={parity_home_save,parity_away_save};
#endif
    for(unsigned int i=0;i<2;++i) {
#ifdef PARITYMOD
        save_name=parity_slot_saves[i];
#endif
        void* save=V014_FN(il2cpp_string_new,void*,const char*)(save_name);
        void* args[]={save};void* exception=0;
        metadata_invoke(spawn,manager,args,&exception);
        write_text("{\"kind\":\"owned_spawn\",\"index\":");metadata_number(i);
        write_text(",\"save\":");metadata_quoted(save_name);
        write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
        write_text("}\n");
    }
    write_text("{\"kind\":\"owned_spawn\",\"status\":\"requested_twice\",\"save\":\"safe_corner_v01\"}\n");
    metadata_spawn_requested=1;
    flush_log();
}

static void metadata_quoted(const char* text) {
    write_text("\"");
    if(text) for(unsigned int i=0;text[i]&&i<1024;++i) {
        BYTE ch=(BYTE)text[i];
        if(ch=='"'||ch=='\\') write_text("\\");
        if(ch>=32) {
            char out[2]={(char)ch,0};
            write_text(out);
        }
    }
    write_text("\"");
}

static void metadata_nullable_string(const char* text) {
    if(text) metadata_quoted(text); else write_text("null");
}

static int metadata_prefix(const char* text,const char* prefix) {
    if(!text||!prefix) return 0;
    while(*prefix) if(*text++!=*prefix++) return 0;
    return 1;
}

static void metadata_type_name(void* type) {
    if(!type) { write_text("null"); return; }
    char* name=V014_FN(il2cpp_type_get_name,char*,void*)(type);
    metadata_nullable_string(name);
    if(name) V014_FN(il2cpp_free,void,void*)(name);
}

/* Resolve a managed reference by its declared metadata type.  This is used
 * for the player-owned Stamina component so the natural trace does not rely
 * on the static player layout guess from an earlier binary. */
static int metadata_type_name_matches(const char* actual,const char* wanted) {
    if(!actual||!wanted) return 0;
    if(same(actual,wanted)) return 1;
    const char* suffix=actual;
    for(const char* cursor=actual;*cursor;++cursor)
        if(*cursor=='.') suffix=cursor+1;
    return same(suffix,wanted);
}

static void* metadata_find_object_field_by_type(void* klass,void* object,
                                                const char* wanted_type,
                                                void** owner_out,void** object_out) {
    if(owner_out) *owner_out=0;
    if(object_out) *object_out=0;
    for(unsigned int depth=0;klass&&depth<12;++depth) {
        void* iterator=0;void* field=0;
        while((field=V014_FN(il2cpp_class_get_fields,void*,void*,void**)
                (klass,&iterator))) {
            unsigned int flags=V014_FN(il2cpp_field_get_flags,unsigned int,void*)(field);
            if(flags&16) continue;
            void* type=V014_FN(il2cpp_field_get_type,void*,void*)(field);
            char* type_name=type?V014_FN(il2cpp_type_get_name,char*,void*)(type):0;
            int match=metadata_type_name_matches(type_name,wanted_type);
            if(type_name) V014_FN(il2cpp_free,void,void*)(type_name);
            if(!match) continue;
            void* value=0;
            if(object) V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
                (object,field,&value);
            if(owner_out) *owner_out=klass;
            if(object_out) *object_out=value;
            return field;
        }
        klass=V014_FN(il2cpp_class_get_parent,void*,void*)(klass);
    }
    return 0;
}

/* Return the raw word width for the small set of scalar/value types used by
 * Stamina and the managed brain state.  Unknown fields remain in the output
 * as metadata with read_ok=false; they are never read with a guessed width. */
static unsigned int metadata_trace_type_words(const char* type_name) {
    if(!type_name) return 0;
    if(same(type_name,"System.Boolean")||same(type_name,"System.Byte")||
       same(type_name,"System.SByte")||same(type_name,"System.Char")||
       same(type_name,"System.Int16")||same(type_name,"System.UInt16")||
       same(type_name,"System.Int32")||same(type_name,"System.UInt32")||
       same(type_name,"System.Single")) return 1;
    if(same(type_name,"System.Int64")||same(type_name,"System.UInt64")||
       same(type_name,"System.Double")) return 2;
    if(same(type_name,"UnityEngine.Vector2")) return 2;
    if(same(type_name,"UnityEngine.Vector3")) return 3;
    if(same(type_name,"UnityEngine.Vector4")||
       same(type_name,"UnityEngine.Quaternion")||
       same(type_name,"UnityEngine.Color")) return 4;
    return 0;
}

static int metadata_trace_write_dynamic_fields(void* klass,void* object) {
    int ok=klass&&object;
    int first=1;
    write_text("[");
    for(unsigned int depth=0;klass&&depth<12;++depth) {
        void* iterator=0;void* field=0;
        while((field=V014_FN(il2cpp_class_get_fields,void*,void*,void**)
                (klass,&iterator))) {
            if(!first) write_text(",");
            first=0;
            const char* field_name=V014_FN(il2cpp_field_get_name,const char*,void*)(field);
            void* type=V014_FN(il2cpp_field_get_type,void*,void*)(field);
            char* type_name=type?V014_FN(il2cpp_type_get_name,char*,void*)(type):0;
            unsigned int flags=V014_FN(il2cpp_field_get_flags,unsigned int,void*)(field);
            int offset=V014_FN(il2cpp_field_get_offset,int,void*)(field);
            unsigned int words=metadata_trace_type_words(type_name);
            write_text("{\"owner\":");metadata_nullable_string(
                V014_FN(il2cpp_class_get_name,const char*,void*)(klass));
            write_text(",\"name\":");metadata_nullable_string(field_name);
            write_text(",\"type\":");metadata_nullable_string(type_name);
            write_text(",\"flags\":");metadata_number(flags);
            write_text(",\"offset\":\"");metadata_write_signed_hex(offset);
            write_text("\",\"word_count\":");metadata_number(words);
            if(words) {
                unsigned int value[4]={0,0,0,0};
                if(flags&16) {
                    V014_FN(il2cpp_field_static_get_value,void,void*,void*)
                        (field,value);
                } else if(object) {
                    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
                        (object,field,value);
                }
                write_text(",\"read_method\":\"");
                if(flags&16) write_text("il2cpp_field_static_get_value");
                else write_text("il2cpp_field_get_value");
                write_text("\",\"read_ok\":");
                if(object||flags&16) write_text("true");else write_text("false");
                write_text(",\"value_words\":");
                if(object||flags&16) metadata_words(value,words);else write_text("null");
            } else {
                /* Unknown value types are intentionally not interpreted.  A
                 * managed reference can still be recorded losslessly as an
                 * identity, but a custom value type must wait for a typed
                 * metadata fixture rather than an ABI guess. */
                int reference=type_name&&(
                    metadata_prefix(type_name,"UnityEngine.")||
                    metadata_prefix(type_name,"System.String")||
                    metadata_prefix(type_name,"System.Collections.")||
                    type_name[0]!='S');
                if(reference&&! (type_name&&metadata_prefix(type_name,"UnityEngine.Vector"))) {
                    void* value=0;
                    if(flags&16) V014_FN(il2cpp_field_static_get_value,void,void*,void*)
                        (field,&value);
                    else if(object) V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
                        (object,field,&value);
                    write_text(",\"read_method\":\"");
                    if(flags&16) write_text("il2cpp_field_static_get_value");
                    else write_text("il2cpp_field_get_value");
                    write_text("\",\"read_ok\":");
                    if(object||flags&16) write_text("true");else write_text("false");
                    write_text(",\"object\":");
                    if(object||flags&16) metadata_object_pointer(value);else write_text("null");
                } else {
                    write_text(",\"read_method\":null,\"read_ok\":false,");
                    write_text("\"unsupported_reason\":\"unknown_value_type\"");
                }
            }
            write_text("}");
            if(type_name) V014_FN(il2cpp_free,void,void*)(type_name);
        }
        klass=V014_FN(il2cpp_class_get_parent,void*,void*)(klass);
    }
    write_text("]");
    return ok;
}

static void metadata_write_module_span(void) {
    MODULE module=GetModuleHandleW(L"GameAssembly.dll");
    metadata_module_base=(QWORD)module;
    metadata_module_size=0;
    if(module) {
        BYTE* bytes=(BYTE*)module;
        DWORD nt=*(DWORD*)(bytes+0x3c);
        if(*(WORD*)bytes==0x5a4d&&*(DWORD*)(bytes+nt)==0x4550&&
           *(WORD*)(bytes+nt+24)==0x20b)
            metadata_module_size=*(DWORD*)(bytes+nt+24+56);
    }
    write_text("{\"kind\":\"metadata_module\",\"base\":\"");
    write_hex(metadata_module_base);
    write_text("\",\"size\":");metadata_number((unsigned int)metadata_module_size);
    write_text("}\n");
}

static void metadata_write_method_pointer(const void* method) {
    QWORD pointer=method?*(const QWORD*)method:0;
    write_text("\"method_pointer\":");
    if(pointer) { write_text("\"");write_hex(pointer);write_text("\""); }
    else write_text("null");
    write_text(",\"rva\":");
    if(pointer&&metadata_module_base&&metadata_module_size&&
       pointer>=metadata_module_base&&pointer<metadata_module_base+metadata_module_size) {
        write_text("\"");write_hex(pointer-metadata_module_base);write_text("\"");
    } else write_text("null");
}

static const char* metadata_method_candidate(const char* owner,const char* name) {
    if(!owner||!name) return 0;
    if(same(owner,"SimulationTickDriver")&&same(name,"FixedUpdate")) return "driver";
    if(same(owner,"DeterministicMover")&&same(name,"FixedUpdate")) return "movement-fixed-update";
    if(same(owner,"DeterministicMover")&&same(name,"ClampMoveDestination")) return "movement-clamp";
    if(same(owner,"TennisGameManager")&&same(name,"OnTennisTick")) return "manager-tick";
    return 0;
}

static void metadata_write_method(const char* image_name,const char* namespaze,
                                  const char* owner,const void* method) {
    if(!method) return;
    const char* name=V014_FN(il2cpp_method_get_name,const char*,const void*)(method);
    void* declaring=V014_FN(il2cpp_method_get_class,void*,const void*)(method);
    const char* declaring_name=declaring?
        V014_FN(il2cpp_class_get_name,const char*,void*)(declaring):owner;
    unsigned int argc=V014_FN(il2cpp_method_get_param_count,unsigned int,const void*)(method);
    const char* candidate=metadata_method_candidate(owner,name);
    write_text("{\"kind\":\"metadata_method\",\"image\":");metadata_nullable_string(image_name);
    write_text(",\"namespace\":");metadata_nullable_string(namespaze);
    write_text(",\"class\":");metadata_nullable_string(owner);
    write_text(",\"declaring_class\":");metadata_nullable_string(declaring_name);
    write_text(",\"name\":");metadata_nullable_string(name);
    write_text(",\"args\":");metadata_number(argc);
    write_text(",\"return_type\":");metadata_type_name(
        (void*)V014_FN(il2cpp_method_get_return_type,const void*,const void*)(method));
    write_text(",\"parameters\":[");
    for(unsigned int i=0;i<argc;++i) {
        if(i) write_text(",");
        write_text("{\"name\":");
        metadata_nullable_string(V014_FN(il2cpp_method_get_param_name,const char*,const void*,unsigned int)(method,i));
        write_text(",\"type\":");
        metadata_type_name((void*)V014_FN(il2cpp_method_get_param,const void*,const void*,unsigned int)(method,i));
        write_text("}");
    }
    write_text("],\"token\":\"");
    write_hex((QWORD)V014_FN(il2cpp_method_get_token,unsigned int,const void*)(method));
    write_text("\",\"flags\":\"");
    write_hex((QWORD)V014_FN(il2cpp_method_get_flags,unsigned int,const void*,unsigned int*)(method,0));
    write_text("\",");
    metadata_write_method_pointer(method);
    if(candidate) { write_text(",\"candidate\":");metadata_quoted(candidate); }
    if(!metadata_layout_written) {
        write_text(",\"method_info_layout\":{\"method_pointer_offset\":0,");
        write_text("\"source\":\"il2cpp-class-internals.h: MethodInfo.methodPointer\"}");
        metadata_layout_written=1;
    }
    write_text("}\n");
    ++metadata_method_count;
    if(candidate) {
        write_text("{\"kind\":\"metadata_candidate\",\"candidate\":");metadata_quoted(candidate);
        write_text(",\"class\":");metadata_nullable_string(owner);
        write_text(",\"method\":");metadata_nullable_string(name);
        write_text(",\"args\":");metadata_number(argc);
        write_text(",\"rva\":");
        QWORD pointer=*(const QWORD*)method;
        if(pointer&&metadata_module_base&&metadata_module_size&&pointer>=metadata_module_base&&
           pointer<metadata_module_base+metadata_module_size) {
            write_text("\"");write_hex(pointer-metadata_module_base);write_text("\"");
        } else write_text("null");
        write_text("}\n");
    }
}

static void metadata_write_field(const char* image_name,const char* namespaze,
                                 const char* owner,void* field) {
    if(!field) return;
    const char* name=V014_FN(il2cpp_field_get_name,const char*,void*)(field);
    void* type=V014_FN(il2cpp_field_get_type,void*,void*)(field);
    unsigned int flags=V014_FN(il2cpp_field_get_flags,unsigned int,void*)(field);
    int offset=V014_FN(il2cpp_field_get_offset,int,void*)(field);
    write_text("{\"kind\":\"metadata_field\",\"image\":");metadata_nullable_string(image_name);
    write_text(",\"namespace\":");metadata_nullable_string(namespaze);
    write_text(",\"class\":");metadata_nullable_string(owner);
    write_text(",\"name\":");metadata_nullable_string(name);
    write_text(",\"type\":");metadata_type_name(type);
    write_text(",\"offset\":\"");metadata_write_signed_hex(offset);
    write_text("\",\"flags\":\"");write_hex((QWORD)flags);write_text("\"}\n");
    ++metadata_field_count;
}

static void metadata_write_class(const char* image_name,void* klass) {
    if(!klass) return;
    const char* owner=V014_FN(il2cpp_class_get_name,const char*,void*)(klass);
    const char* namespaze=V014_FN(il2cpp_class_get_namespace,const char*,void*)(klass);
    void* parent=V014_FN(il2cpp_class_get_parent,void*,void*)(klass);
    const char* parent_name=parent?V014_FN(il2cpp_class_get_name,const char*,void*)(parent):0;
    const char* parent_namespace=parent?V014_FN(il2cpp_class_get_namespace,const char*,void*)(parent):0;
    write_text("{\"kind\":\"metadata_class\",\"image\":");metadata_nullable_string(image_name);
    write_text(",\"namespace\":");metadata_nullable_string(namespaze);
    write_text(",\"name\":");metadata_nullable_string(owner);
    write_text(",\"parent\":");metadata_nullable_string(parent_name);
    write_text(",\"parent_namespace\":");metadata_nullable_string(parent_namespace);
    write_text(",\"type_token\":\"");
    write_hex((QWORD)V014_FN(il2cpp_class_get_type_token,unsigned int,void*)(klass));
    write_text("\",\"flags\":\"");
    write_hex((QWORD)(unsigned int)V014_FN(il2cpp_class_get_flags,int,const void*)(klass));
    write_text("\",\"instance_size\":");
    metadata_write_signed_decimal(V014_FN(il2cpp_class_instance_size,int,const void*)(klass));
    write_text("}\n");
    ++metadata_selected_count;

    void* iter=0;void* field;unsigned int fields=0;
    while(fields<512&&(field=V014_FN(il2cpp_class_get_fields,void*,void*,void**)(klass,&iter))) {
        metadata_write_field(image_name,namespaze,owner,field);++fields;
    }
    if(field) {
        write_text("{\"kind\":\"metadata_cap\",\"class\":");metadata_nullable_string(owner);
        write_text(",\"members\":\"fields\"}\n");
    }
    iter=0;void* method;unsigned int methods=0;
    while(methods<1024&&(method=V014_FN(il2cpp_class_get_methods,void*,void*,void**)(klass,&iter))) {
        metadata_write_method(image_name,namespaze,owner,method);++methods;
    }
    if(method) {
        write_text("{\"kind\":\"metadata_cap\",\"class\":");metadata_nullable_string(owner);
        write_text(",\"members\":\"methods\"}\n");
    }
}

static int metadata_interesting_class(const char* name) {
    if(!name) return 0;
    return same(name,"DeterministicMover")||same(name,"SimulationTickDriver")||
        same(name,"CarInfo")||same(name,"GetCarPart")||same(name,"RelativePosition")||same(name,"Spherecast")||same(name,"HitInfo")||same(name,"CarRaycasts")||same(name,"Autosteer")||same(name,"Autothrottle")||same(name,"ModularCarController")||same(name,"ConstructRacingV2Properties")||same(name,"Stat")||same(name,"Keypress")||same(name,"Region")||same(name,"TennisGameManager")||same(name,"TennisPlayer")||same(name,"TennisBall")||
        same(name,"TennisScore")||same(name,"TennisCourt")||same(name,"GameManager")||
        same(name,"Aialander")||same(name,"Player")||same(name,"Stamina")||
        same(name,"AIBrain")||same(name,"GraphManager")||same(name,"CompiledGraphPlan")||
        same(name,"CompiledInstruction")||same(name,"GraphValue")||
        metadata_prefix(name,"Tennis")||metadata_prefix(name,"Graph")||
        metadata_prefix(name,"Operation")||metadata_prefix(name,"FloatGate")||
        metadata_prefix(name,"Part")||metadata_prefix(name,"Damageable")||metadata_prefix(name,"Body")||metadata_prefix(name,"Hybrid")||metadata_prefix(name,"Plan")||metadata_prefix(name,"Axle")||metadata_prefix(name,"Engine")||metadata_prefix(name,"Aia")||metadata_prefix(name,"Demo")||metadata_prefix(name,"Ecs")||metadata_prefix(name,"Fleet")||metadata_prefix(name,"Modular")||metadata_prefix(name,"Replay")||metadata_prefix(name,"Nav")||metadata_prefix(name,"Racing")||metadata_prefix(name,"Race")||metadata_prefix(name,"Car")||metadata_prefix(name,"Wheel")||metadata_prefix(name,"Vehicle")||metadata_prefix(name,"Waypoint")||metadata_prefix(name,"Get")||metadata_prefix(name,"Set")||metadata_prefix(name,"AddFloats")||metadata_prefix(name,"SetVariable")||
        metadata_prefix(name,"GetVariable");
}

static void metadata_record_catalog(const char* image_name,void* klass) {
    const char* name=V014_FN(il2cpp_class_get_name,const char*,void*)(klass);
    const char* namespaze=V014_FN(il2cpp_class_get_namespace,const char*,void*)(klass);
    write_text("{\"kind\":\"class_catalog\",\"image\":");metadata_nullable_string(image_name);
    write_text(",\"namespace\":");metadata_nullable_string(namespaze);
    write_text(",\"name\":");metadata_nullable_string(name);
    write_text(",\"type_token\":\"");
    write_hex((QWORD)V014_FN(il2cpp_class_get_type_token,unsigned int,void*)(klass));
    write_text("\"}\n");
    ++metadata_catalog_count;
}

static void metadata_named_core_class(void* image,const char* namespaze,const char* name,
                                      const char* image_name) {
    void* klass=V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
        (image,namespaze,name);
    if(klass) metadata_write_class(image_name,klass);
    else {
        write_text("{\"kind\":\"metadata_missing_class\",\"image\":");metadata_nullable_string(image_name);
        write_text(",\"namespace\":");metadata_nullable_string(namespaze);
        write_text(",\"name\":");metadata_nullable_string(name);write_text("}\n");
    }
}

static int metadata_apis_ready(void) {
    return V014_FN(il2cpp_domain_get,void*)&&V014_FN(il2cpp_domain_get_assemblies,void*)&&
        V014_FN(il2cpp_assembly_get_image,void*)&&V014_FN(il2cpp_image_get_name,void*)&&
        V014_FN(il2cpp_image_get_class_count,void*)&&V014_FN(il2cpp_image_get_class,void*)&&
        V014_FN(il2cpp_class_from_name,void*)&&V014_FN(il2cpp_class_get_name,void*)&&
        V014_FN(il2cpp_class_get_namespace,void*)&&V014_FN(il2cpp_class_get_parent,void*)&&
        V014_FN(il2cpp_class_get_type_token,void*)&&V014_FN(il2cpp_class_get_flags,void*)&&
        V014_FN(il2cpp_class_instance_size,void*)&&V014_FN(il2cpp_class_get_fields,void*)&&
        V014_FN(il2cpp_class_get_methods,void*)&&V014_FN(il2cpp_field_get_name,void*)&&
        V014_FN(il2cpp_field_get_type,void*)&&V014_FN(il2cpp_field_get_offset,void*)&&
        V014_FN(il2cpp_field_get_flags,void*)&&V014_FN(il2cpp_method_get_name,void*)&&
        V014_FN(il2cpp_method_get_class,void*)&&V014_FN(il2cpp_method_get_return_type,void*)&&
        V014_FN(il2cpp_method_get_param_count,void*)&&V014_FN(il2cpp_method_get_param,void*)&&
        V014_FN(il2cpp_method_get_param_name,void*)&&V014_FN(il2cpp_method_get_token,void*)&&
        V014_FN(il2cpp_method_get_flags,void*)&&V014_FN(il2cpp_type_get_name,void*)&&
        V014_FN(il2cpp_free,void*)&&V014_FN(il2cpp_class_get_method_from_name,void*)&&
        V014_FN(il2cpp_class_get_field_from_name,void*)&&V014_FN(il2cpp_field_get_value,void*)&&
        V014_FN(il2cpp_field_set_value,void*)&&V014_FN(il2cpp_field_static_get_value,void*)&&
        V014_FN(il2cpp_object_unbox,void*)&&V014_FN(il2cpp_object_get_class,void*)&&
        V014_FN(il2cpp_string_new,void*);
}

typedef struct {
    unsigned int game_state;
    unsigned int serving_team;
    unsigned int is_training_mode;
    unsigned int training_player_team;
    unsigned int point_resolved;
    unsigned int home_team;
    unsigned int away_team;
    unsigned int home_points;
    unsigned int away_points;
    void* score;
    int valid;
} metadata_clamp_snapshot;

typedef struct {
    const char* case_id;
    unsigned int game_state;
    unsigned int serving_team;
    unsigned int home_points;
    unsigned int away_points;
} metadata_clamp_case;

static int metadata_take_clamp_snapshot(void* manager,void* manager_class,
                                        void* home,void* away,
                                        metadata_clamp_snapshot* snapshot) {
    int valid=1;
    snapshot->score=metadata_field_object(manager_class,manager,
                                          "<Score>k__BackingField");
    if(!metadata_read_u32_field(manager_class,manager,
            "<CurrentGameState>k__BackingField",&snapshot->game_state)) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<ServingTeam>k__BackingField",&snapshot->serving_team)) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<IsTrainingMode>k__BackingField",&snapshot->is_training_mode)) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<TrainingPlayerTeam>k__BackingField",&snapshot->training_player_team)) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,"pointResolved",
                                &snapshot->point_resolved)) valid=0;
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    if(!metadata_read_u32_field(home_class,home,"Team",&snapshot->home_team)) valid=0;
    if(!metadata_read_u32_field(away_class,away,"Team",&snapshot->away_team)) valid=0;
    if(!snapshot->score) valid=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    if(!metadata_read_u32_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",&snapshot->home_points)) valid=0;
    if(!metadata_read_u32_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",&snapshot->away_points)) valid=0;
    snapshot->valid=valid;
    return valid;
}

static int metadata_restore_clamp_snapshot(void* manager,void* manager_class,
                                           void* home,void* away,
                                           const metadata_clamp_snapshot* snapshot) {
    if(!snapshot||!snapshot->valid) return 0;
    int valid=1;
    if(!metadata_set_u32_field(manager_class,manager,
            "<CurrentGameState>k__BackingField",snapshot->game_state)) valid=0;
    if(!metadata_set_u32_field(manager_class,manager,
            "<ServingTeam>k__BackingField",snapshot->serving_team)) valid=0;
    if(!metadata_set_u32_field(manager_class,manager,
            "<IsTrainingMode>k__BackingField",snapshot->is_training_mode)) valid=0;
    if(!metadata_set_u32_field(manager_class,manager,
            "<TrainingPlayerTeam>k__BackingField",snapshot->training_player_team)) valid=0;
    if(!metadata_set_u32_field(manager_class,manager,"pointResolved",
                               snapshot->point_resolved)) valid=0;
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    if(!metadata_set_u32_field(home_class,home,"Team",snapshot->home_team)) valid=0;
    if(!metadata_set_u32_field(away_class,away,"Team",snapshot->away_team)) valid=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    if(!metadata_set_u32_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",snapshot->home_points)) valid=0;
    if(!metadata_set_u32_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",snapshot->away_points)) valid=0;
    return valid;
}

static int metadata_clamp_snapshot_matches(void* manager,void* manager_class,
                                            void* home,void* away,
                                            const metadata_clamp_snapshot* snapshot) {
    if(!snapshot||!snapshot->valid) return 0;
    unsigned int value=0;int valid=1;
    if(!metadata_read_u32_field(manager_class,manager,
            "<CurrentGameState>k__BackingField",&value)||value!=snapshot->game_state) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<ServingTeam>k__BackingField",&value)||value!=snapshot->serving_team) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<IsTrainingMode>k__BackingField",&value)||value!=snapshot->is_training_mode) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<TrainingPlayerTeam>k__BackingField",&value)||value!=snapshot->training_player_team) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,"pointResolved",
                                &value)||value!=snapshot->point_resolved) valid=0;
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    if(!metadata_read_u32_field(home_class,home,"Team",&value)||value!=snapshot->home_team) valid=0;
    if(!metadata_read_u32_field(away_class,away,"Team",&value)||value!=snapshot->away_team) valid=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    if(!metadata_read_u32_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",&value)||value!=snapshot->home_points) valid=0;
    if(!metadata_read_u32_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",&value)||value!=snapshot->away_points) valid=0;
    return valid;
}

static void metadata_write_clamp_restore(void* manager,void* manager_class,
                                         void* home,void* away,const char* case_id,
                                         int restore_ok,
                                         const metadata_clamp_snapshot* snapshot) {
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    void* score_class=snapshot&&snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    int matches=metadata_clamp_snapshot_matches(manager,manager_class,home,away,snapshot);
    write_text("{\"kind\":\"clamp_restore\",\"case_id\":");metadata_quoted(case_id);
    write_text(",\"restore_api_ok\":");if(restore_ok)write_text("true");else write_text("false");
    write_text(",\"matches_snapshot\":");if(matches)write_text("true");else write_text("false");
    write_text(",\"manager_fields\":{");
    metadata_write_raw_field(manager_class,manager,"<CurrentGameState>k__BackingField","u32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "<ServingTeam>k__BackingField","u32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "<IsTrainingMode>k__BackingField","u32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,
        "<TrainingPlayerTeam>k__BackingField","u32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,"pointResolved","u32",1);
    write_text("},\"player_teams\":{\"home\":{");
    metadata_write_raw_field(home_class,home,"Team","i32",1);
    write_text("},\"away\":{");
    metadata_write_raw_field(away_class,away,"Team","i32",1);
    write_text("}},\"score_point_fields\":{");
    metadata_write_raw_field(score_class,snapshot?snapshot->score:0,
        "<HomePoints>k__BackingField","i32",1);write_text(",");
    metadata_write_raw_field(score_class,snapshot?snapshot->score:0,
        "<AwayPoints>k__BackingField","i32",1);
    write_text("}}\n");
}

static unsigned int metadata_clamp_sweep_case(void* manager,void* manager_class,
                                              void* home,void* away,
                                              void* method,
                                              const metadata_clamp_snapshot* snapshot,
                                              const metadata_clamp_case* fixture) {
    if(!metadata_restore_clamp_snapshot(manager,manager_class,home,away,snapshot)) {
        write_text("{\"kind\":\"clamp_case\",\"case_id\":");metadata_quoted(fixture->case_id);
        write_text(",\"status\":\"restore_before_failed\"}\n");
        return 0;
    }
    int applied=1;
    if(!metadata_set_u32_field(manager_class,manager,"<CurrentGameState>k__BackingField",
                               fixture->game_state)) applied=0;
    if(!metadata_set_u32_field(manager_class,manager,"<ServingTeam>k__BackingField",
                               fixture->serving_team)) applied=0;
    void* score=snapshot->score;
    void* score_class=score?V014_FN(il2cpp_object_get_class,void*,void*)(score):0;
    if(!metadata_set_u32_field(score_class,score,"<HomePoints>k__BackingField",
                               fixture->home_points)) applied=0;
    if(!metadata_set_u32_field(score_class,score,"<AwayPoints>k__BackingField",
                               fixture->away_points)) applied=0;
    unsigned int actual_state=0,actual_serving=0,actual_home_points=0,actual_away_points=0;
    int readback=1;
    if(!metadata_read_u32_field(manager_class,manager,"<CurrentGameState>k__BackingField",
                                &actual_state)||actual_state!=fixture->game_state) readback=0;
    if(!metadata_read_u32_field(manager_class,manager,"<ServingTeam>k__BackingField",
                                &actual_serving)||actual_serving!=fixture->serving_team) readback=0;
    if(!metadata_read_u32_field(score_class,score,"<HomePoints>k__BackingField",
                                &actual_home_points)||actual_home_points!=fixture->home_points) readback=0;
    if(!metadata_read_u32_field(score_class,score,"<AwayPoints>k__BackingField",
                                &actual_away_points)||actual_away_points!=fixture->away_points) readback=0;
    write_text("{\"kind\":\"clamp_case\",\"case_id\":");metadata_quoted(fixture->case_id);
    write_text(",\"game_state\":");metadata_number(fixture->game_state);
    write_text(",\"serving_team\":");metadata_number(fixture->serving_team);
    write_text(",\"home_points\":");metadata_number(fixture->home_points);
    write_text(",\"away_points\":");metadata_number(fixture->away_points);
    write_text(",\"readback\":{\"game_state\":");metadata_number(actual_state);
    write_text(",\"serving_team\":");metadata_number(actual_serving);
    write_text(",\"home_points\":");metadata_number(actual_home_points);
    write_text(",\"away_points\":");metadata_number(actual_away_points);write_text("}");
    write_text(",\"state_restore_before_ok\":true,\"inputs_applied\":");
    if(applied)write_text("true");else write_text("false");
    write_text(",\"readback_ok\":");if(readback)write_text("true");else write_text("false");
    write_text("}\n");
    if(!applied||!readback) {
        int restored=metadata_restore_clamp_snapshot(manager,manager_class,home,away,snapshot);
        metadata_write_clamp_restore(manager,manager_class,home,away,fixture->case_id,
                                     restored,snapshot);
        flush_log();
        return 0;
    }
    metadata_write_runtime_context(manager,manager_class,home,away,
                                   fixture->case_id,"snapshot_restore");
    float xs[]={-20.0f,-13.0f,-10.0f,-5.0f,0.0f,5.0f,10.0f,13.0f,20.0f};
    float ys[]={0.0f,1.25f};
    float zs[]={-15.0f,-9.25f,-3.0f,0.0f,3.0f,9.25f,15.0f};
    const char* slots[]={"home","away"};
    void* players[]={home,away};
    unsigned int rows=0;
    for(unsigned int slot=0;slot<2;++slot) {
        void* player=players[slot];
        for(unsigned int xi=0;xi<sizeof(xs)/sizeof(xs[0]);++xi)
            for(unsigned int yi=0;yi<sizeof(ys)/sizeof(ys[0]);++yi)
                for(unsigned int zi=0;zi<sizeof(zs)/sizeof(zs[0]);++zi) {
                    float world[3]={xs[xi],ys[yi],zs[zi]};
                    void* args[]={player,world};
                    void* exception=0;
                    void* result=((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
                        (method,manager,args,&exception);
                    write_text("{\"kind\":\"clamp_sweep\",\"case_id\":");
                    metadata_quoted(fixture->case_id);
                    write_text(",\"slot\":");metadata_quoted(slots[slot]);
                    write_text(",\"input_f32_words\":");metadata_words(world,3);
                    write_text(",\"result_f32_words\":");
                    if(result&&!exception) metadata_words(V014_FN(il2cpp_object_unbox,void*,void*)(result),3);
                    else write_text("null");
                    write_text(",\"exception\":");
                    if(exception) write_text("true");else write_text("false");
                    write_text("}\n");
                    ++rows;
                }
    }
    int restored=metadata_restore_clamp_snapshot(manager,manager_class,home,away,snapshot);
    metadata_write_clamp_restore(manager,manager_class,home,away,fixture->case_id,
                                 restored,snapshot);
    flush_log();
    return rows;
}

/*
 * The physics fixtures below deliberately stay at the managed-method
 * boundary.  Every field is looked up by metadata name and every method is
 * looked up by class/name/arity.  The only constants are finite input words
 * (including the captured fixed-delta word and the binary's documented
 * 1.001f threshold); no object or field offset is used by this probe.
 */
typedef union {
    unsigned int words;
    float value;
} metadata_float_bits;

typedef struct {
    void* score;
    void* ball;
    unsigned int game_state;
    unsigned int serving_team;
    unsigned int point_resolved;
    unsigned int fatigue_gain;
    unsigned int deuce_grace;
    unsigned int rally_grace;
    unsigned int rally_count;
    unsigned int score_home;
    unsigned int score_away;
    unsigned int ball_arcade_gravity;
    unsigned int ball_max_speed;
    unsigned int ball_flight_pace;
    unsigned int ball_velocity[3];
    unsigned int ball_position[3];
    unsigned int ball_slice_curve;
    unsigned int ball_curve_team;
    int valid;
} metadata_physics_snapshot;

typedef struct {
    const char* case_id;
    unsigned int score_home;
    unsigned int score_away;
    unsigned int rally_count;
    unsigned int gain_words;
    const char* gain_source;
} metadata_fatigue_case;

static float metadata_float_from_words(unsigned int words) {
    metadata_float_bits bits;
    bits.words=words;
    return bits.value;
}

static int metadata_take_physics_snapshot(void* manager,void* manager_class,
                                          metadata_physics_snapshot* snapshot) {
    int valid=1;
    snapshot->score=metadata_field_object(manager_class,manager,
                                          "<Score>k__BackingField");
    snapshot->ball=metadata_field_object(manager_class,manager,
                                         "<Ball>k__BackingField");
    if(!snapshot->score||!snapshot->ball) valid=0;
    void* ball_class=snapshot->ball?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->ball):0;
    const char* ball_name=ball_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(ball_class):0;
    if(!ball_name||!same(ball_name,"TennisBall")) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<CurrentGameState>k__BackingField",&snapshot->game_state)) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<ServingTeam>k__BackingField",&snapshot->serving_team)) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,"pointResolved",
                                &snapshot->point_resolved)) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"fatigueShotSpeedGain",
                                  &snapshot->fatigue_gain,1)) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"deuceFatigueGracePoints",
                                  &snapshot->deuce_grace,1)) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"rallyFatigueGraceShots",
                                  &snapshot->rally_grace,1)) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"rallyShotCount",
                                  &snapshot->rally_count,1)) valid=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    if(!metadata_read_words_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",&snapshot->score_home,1)) valid=0;
    if(!metadata_read_words_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",&snapshot->score_away,1)) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"arcadeGravity",
                                  &snapshot->ball_arcade_gravity,1)) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"maxSpeed",
                                  &snapshot->ball_max_speed,1)) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"flightPace",
                                  &snapshot->ball_flight_pace,1)) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,
            "<Velocity>k__BackingField",snapshot->ball_velocity,3)) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,
            "<Position>k__BackingField",snapshot->ball_position,3)) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"sliceCurve",
                                  &snapshot->ball_slice_curve,1)) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"curveTeam",
                                  &snapshot->ball_curve_team,1)) valid=0;
    snapshot->valid=valid;
    return valid;
}

static int metadata_restore_physics_snapshot(void* manager,void* manager_class,
                                             const metadata_physics_snapshot* snapshot) {
    if(!snapshot||!snapshot->valid) return 0;
    int valid=1;
    if(!metadata_set_u32_field(manager_class,manager,
            "<CurrentGameState>k__BackingField",snapshot->game_state)) valid=0;
    if(!metadata_set_u32_field(manager_class,manager,
            "<ServingTeam>k__BackingField",snapshot->serving_team)) valid=0;
    if(!metadata_set_u32_field(manager_class,manager,"pointResolved",
                               snapshot->point_resolved)) valid=0;
    if(!metadata_set_words_field(manager_class,manager,"fatigueShotSpeedGain",
                                 &snapshot->fatigue_gain,1)) valid=0;
    if(!metadata_set_words_field(manager_class,manager,"deuceFatigueGracePoints",
                                 &snapshot->deuce_grace,1)) valid=0;
    if(!metadata_set_words_field(manager_class,manager,"rallyFatigueGraceShots",
                                 &snapshot->rally_grace,1)) valid=0;
    if(!metadata_set_words_field(manager_class,manager,"rallyShotCount",
                                 &snapshot->rally_count,1)) valid=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    if(!metadata_set_words_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",&snapshot->score_home,1)) valid=0;
    if(!metadata_set_words_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",&snapshot->score_away,1)) valid=0;
    void* ball_class=snapshot->ball?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->ball):0;
    if(!metadata_set_words_field(ball_class,snapshot->ball,"arcadeGravity",
                                 &snapshot->ball_arcade_gravity,1)) valid=0;
    if(!metadata_set_words_field(ball_class,snapshot->ball,"maxSpeed",
                                 &snapshot->ball_max_speed,1)) valid=0;
    if(!metadata_set_words_field(ball_class,snapshot->ball,"flightPace",
                                 &snapshot->ball_flight_pace,1)) valid=0;
    if(!metadata_set_words_field(ball_class,snapshot->ball,
            "<Velocity>k__BackingField",snapshot->ball_velocity,3)) valid=0;
    if(!metadata_set_words_field(ball_class,snapshot->ball,
            "<Position>k__BackingField",snapshot->ball_position,3)) valid=0;
    if(!metadata_set_words_field(ball_class,snapshot->ball,"sliceCurve",
                                 &snapshot->ball_slice_curve,1)) valid=0;
    if(!metadata_set_words_field(ball_class,snapshot->ball,"curveTeam",
                                 &snapshot->ball_curve_team,1)) valid=0;
    return valid;
}

static int metadata_physics_snapshot_matches(void* manager,void* manager_class,
                                             const metadata_physics_snapshot* snapshot) {
    if(!snapshot||!snapshot->valid) return 0;
    int valid=1;unsigned int value[4]={0,0,0,0};
    if(!metadata_read_u32_field(manager_class,manager,
            "<CurrentGameState>k__BackingField",&value[0])||
       value[0]!=snapshot->game_state) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,
            "<ServingTeam>k__BackingField",&value[0])||
       value[0]!=snapshot->serving_team) valid=0;
    if(!metadata_read_u32_field(manager_class,manager,"pointResolved",&value[0])||
       value[0]!=snapshot->point_resolved) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"fatigueShotSpeedGain",
                                  &value[0],1)||value[0]!=snapshot->fatigue_gain) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"deuceFatigueGracePoints",
                                  &value[0],1)||value[0]!=snapshot->deuce_grace) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"rallyFatigueGraceShots",
                                  &value[0],1)||value[0]!=snapshot->rally_grace) valid=0;
    if(!metadata_read_words_field(manager_class,manager,"rallyShotCount",
                                  &value[0],1)||value[0]!=snapshot->rally_count) valid=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    if(!metadata_read_words_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",&value[0],1)||
       value[0]!=snapshot->score_home) valid=0;
    if(!metadata_read_words_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",&value[0],1)||
       value[0]!=snapshot->score_away) valid=0;
    void* ball_class=snapshot->ball?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->ball):0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"arcadeGravity",
                                  &value[0],1)||value[0]!=snapshot->ball_arcade_gravity) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"maxSpeed",
                                  &value[0],1)||value[0]!=snapshot->ball_max_speed) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"flightPace",
                                  &value[0],1)||value[0]!=snapshot->ball_flight_pace) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,
            "<Velocity>k__BackingField",value,3)) valid=0;
    else for(unsigned int i=0;i<3;++i) if(value[i]!=snapshot->ball_velocity[i]) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,
            "<Position>k__BackingField",value,3)) valid=0;
    else for(unsigned int i=0;i<3;++i) if(value[i]!=snapshot->ball_position[i]) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"sliceCurve",
                                  &value[0],1)||value[0]!=snapshot->ball_slice_curve) valid=0;
    if(!metadata_read_words_field(ball_class,snapshot->ball,"curveTeam",
                                  &value[0],1)||value[0]!=snapshot->ball_curve_team) valid=0;
    return valid;
}

static void metadata_write_physics_restore(void* manager,void* manager_class,
                                           const char* component,const char* case_id,
                                           int restore_ok,
                                           const metadata_physics_snapshot* snapshot) {
    int matches=metadata_physics_snapshot_matches(manager,manager_class,snapshot);
    write_text("{\"kind\":\"physics_restore\",\"schema\":\"v014-physics-v1\",\"component\":");
    metadata_quoted(component);write_text(",\"case_id\":");metadata_quoted(case_id);
    write_text(",\"restore_api_ok\":");if(restore_ok)write_text("true");else write_text("false");
    write_text(",\"matches_snapshot\":");if(matches)write_text("true");else write_text("false");
    write_text(",\"snapshot\":{\"manager\":{\"fatigue_gain\":");
    metadata_words(&snapshot->fatigue_gain,1);write_text(",\"rally_count\":");
    metadata_words(&snapshot->rally_count,1);write_text(",\"score_home\":");
    metadata_words(&snapshot->score_home,1);write_text(",\"score_away\":");
    metadata_words(&snapshot->score_away,1);write_text("},\"ball\":{\"flight_pace\":");
    metadata_words(&snapshot->ball_flight_pace,1);write_text(",\"velocity\":");
    metadata_words(snapshot->ball_velocity,3);write_text("}}}\n");
}

static void metadata_write_physics_context(void* manager,void* manager_class,
                                           const metadata_physics_snapshot* snapshot) {
    void* ball=snapshot?snapshot->ball:0;
    void* ball_class=ball?V014_FN(il2cpp_object_get_class,void*,void*)(ball):0;
    write_text("{\"kind\":\"physics_context\",\"schema\":\"v014-physics-v1\",\"snapshot_only\":true");
    write_text(",\"manager_object\":");metadata_object_pointer(manager);
    write_text(",\"ball_object\":");metadata_object_pointer(ball);
    write_text(",\"ball_class\":");
    metadata_nullable_string(ball_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(ball_class):0);
    write_text(",\"manager_fields\":{\"fatigueShotSpeedGain\":");
    metadata_words(&snapshot->fatigue_gain,1);
    write_text(",\"deuceFatigueGracePoints\":");metadata_words(&snapshot->deuce_grace,1);
    write_text(",\"rallyFatigueGraceShots\":");metadata_words(&snapshot->rally_grace,1);
    write_text(",\"rallyShotCount\":");metadata_words(&snapshot->rally_count,1);
    write_text(",\"CurrentGameState\":");metadata_words(&snapshot->game_state,1);
    write_text(",\"ServingTeam\":");metadata_words(&snapshot->serving_team,1);write_text("}");
    write_text(",\"score_fields\":{\"HomePoints\":");metadata_words(&snapshot->score_home,1);
    write_text(",\"AwayPoints\":");metadata_words(&snapshot->score_away,1);write_text("}");
    write_text(",\"ball_fields\":{\"arcadeGravity\":");
    metadata_words(&snapshot->ball_arcade_gravity,1);write_text(",\"maxSpeed\":");
    metadata_words(&snapshot->ball_max_speed,1);write_text(",\"flightPace\":");
    metadata_words(&snapshot->ball_flight_pace,1);write_text(",\"Velocity\":");
    metadata_words(snapshot->ball_velocity,3);write_text(",\"Position\":");
    metadata_words(snapshot->ball_position,3);write_text(",\"sliceCurve\":");
    metadata_words(&snapshot->ball_slice_curve,1);write_text(",\"curveTeam\":");
    metadata_words(&snapshot->ball_curve_team,1);write_text("}");
    write_text(",\"named_fields\":{");
    metadata_write_raw_field(ball_class,ball,"arcadeGravity","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"maxSpeed","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"flightPace","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,
        "<Velocity>k__BackingField","vector3-f32",3);
    write_text(",");metadata_write_raw_field(ball_class,ball,
        "<Position>k__BackingField","vector3-f32",3);
    write_text(",\"tennisManager\":");metadata_object_pointer(
        metadata_field_object(ball_class,ball,"tennisManager"));
    write_text("}");
    write_text(",\"ball_unity_object_alive\":");
    void* alive_method=metadata_core_object_class?
        metadata_method_checked(metadata_core_object_class,"op_Implicit",1):0;
    void* alive_args[]={ball};void* alive_exception=0;unsigned int alive_words[1]={0};
    int alive_ok=metadata_invoke_words(alive_method,0,alive_args,alive_words,1,&alive_exception);
    if(alive_ok)metadata_words(alive_words,1);else write_text("null");
    write_text(",\"ball_unity_object_exception\":");
    if(alive_exception)write_text("true");else write_text("false");
    write_text(",\"fixed_delta_time\":");
    if(metadata_core_time_class) metadata_write_getter(metadata_core_time_class,0,
                                                        "get_fixedDeltaTime",1);
    else write_text("null");
    write_text(",\"fixture_dt_f32_words\":[1016833504],\"fixture_dt_source\":");
    metadata_quoted("captured Unity Time.fixedDeltaTime raw word");
    write_text(",\"threshold_f32_words\":[1065361605],\"threshold_source\":");
    metadata_quoted("v014 rdata 0x3f8020c5");
    write_text(",\"integrate_early_dt_threshold_f32_words\":[" );
    metadata_number(0x358637bd);write_text("],\"integrate_early_dt_source\":");
    metadata_quoted("v014 rdata 0x56a5bec");
    write_text(",\"integrate_max_substeps\":24,\"integrate_max_substeps_source\":");
    metadata_quoted("v014 IntegrateFlight loop bound");
    write_text(",\"net_right\":{");
    void* method=metadata_method_checked(ball_class,"NetRight",1);
    for(unsigned int team=0;team<2;++team) {
        if(team) write_text(",");write_text("\"team");metadata_number(team);write_text("\":{");
        int team_value=(int)team;void* args[]={&team_value};void* exception=0;
        unsigned int words[3]={0,0,0};
        int ok=metadata_invoke_words(method,ball,args,words,3,&exception);
        write_text("\"output_f32_words\":");if(ok)metadata_words(words,3);else write_text("null");
        write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
        write_text("}");
    }
    write_text("}}\n");
}

static void metadata_write_exception_pointer(void* exception) {
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"exception_object\":");metadata_object_pointer(exception);
}

static int metadata_read_getter_words(void* klass,void* object,const char* name,
                                      unsigned int argc,unsigned int* words,
                                      unsigned int count,void** exception_out) {
    return metadata_invoke_words(metadata_method_checked(klass,name,argc),object,0,
                                  words,count,exception_out);
}

static int metadata_read_getter_i32(void* klass,void* object,const char* name,
                                    unsigned int* value,void** exception_out) {
    return metadata_read_getter_words(klass,object,name,0,value,1,exception_out);
}

static int metadata_write_effective_case(void* manager,void* manager_class,
                                         const metadata_physics_snapshot* snapshot,
                                         void* ball_class,const char* case_id,
                                         unsigned int pace_words) {
    unsigned int effective[1]={0};void* exception=0;
    int set_ok=metadata_set_words_field(ball_class,snapshot->ball,"flightPace",&pace_words,1);
    unsigned int pace_readback=0;
    int pace_readback_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                   "flightPace",&pace_readback,1);
    int ok=metadata_read_getter_words(ball_class,snapshot->ball,
                                      "get_EffectiveMaxSpeed",0,effective,1,&exception);
    write_text("{\"kind\":\"physics_effective_max_speed\",\"schema\":\"v014-physics-v1\",\"case_id\":");
    metadata_quoted(case_id);write_text(",\"input\":{\"flight_pace_f32_words\":");
    metadata_words(&pace_words,1);write_text(",\"flight_pace_readback_f32_words\":");
    if(pace_readback_ok)metadata_words(&pace_readback,1);else write_text("null");
    write_text(",\"max_speed_field_f32_words\":");
    metadata_words(&snapshot->ball_max_speed,1);write_text("},\"output\":{\"effective_max_speed_f32_words\":");
    if(ok) metadata_words(effective,1); else write_text("null");
    write_text("},\"set_ok\":");if(set_ok)write_text("true");else write_text("false");
    write_text(",\"readback_ok\":");if(pace_readback_ok)write_text("true");else write_text("false");
    write_text(",\"invoke_ok\":");if(ok)write_text("true");else write_text("false");
    metadata_write_exception_pointer(exception);write_text("}\n");
    ++metadata_effective_rows;++metadata_physics_rows;
    return set_ok&&pace_readback_ok&&ok;
}

static int metadata_apply_fatigue_case(void* manager,void* manager_class,
                                       const metadata_physics_snapshot* snapshot,
                                       void* ball_class,
                                       const metadata_fatigue_case* fixture) {
    if(!metadata_restore_physics_snapshot(manager,manager_class,snapshot)) return 0;
    int applied=1;
    if(!metadata_set_words_field(manager_class,manager,"fatigueShotSpeedGain",
                                 &fixture->gain_words,1)) applied=0;
    if(!metadata_set_words_field(manager_class,manager,"rallyShotCount",
                                 &fixture->rally_count,1)) applied=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    if(!metadata_set_words_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",&fixture->score_home,1)) applied=0;
    if(!metadata_set_words_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",&fixture->score_away,1)) applied=0;
    unsigned int read_gain=0,read_rally=0,read_home=0,read_away=0;
    if(!metadata_read_words_field(manager_class,manager,"fatigueShotSpeedGain",
                                  &read_gain,1)||read_gain!=fixture->gain_words) applied=0;
    if(!metadata_read_words_field(manager_class,manager,"rallyShotCount",
                                  &read_rally,1)||read_rally!=fixture->rally_count) applied=0;
    if(!metadata_read_words_field(score_class,snapshot->score,
            "<HomePoints>k__BackingField",&read_home,1)||read_home!=fixture->score_home) applied=0;
    if(!metadata_read_words_field(score_class,snapshot->score,
            "<AwayPoints>k__BackingField",&read_away,1)||read_away!=fixture->score_away) applied=0;
    unsigned int deuce_active=0,rally_active=0,active_scale[1]={0},getter_gain[1]={0};
    void* deuce_exception=0;void* rally_exception=0;void* scale_exception=0;void* gain_exception=0;
    int deuce_ok=metadata_read_getter_i32(manager_class,manager,
        "get_DeuceFatigueActivePoints",&deuce_active,&deuce_exception);
    int rally_ok=metadata_read_getter_i32(manager_class,manager,
        "get_RallyFatigueActivePoints",&rally_active,&rally_exception);
    int scale_ok=metadata_read_getter_words(manager_class,manager,
        "get_FatigueShotSpeedScale",0,active_scale,1,&scale_exception);
    int gain_ok=metadata_read_getter_words(manager_class,manager,
        "get_FatigueShotSpeedGain",0,getter_gain,1,&gain_exception);
    void* manager_from_ball=metadata_field_object(ball_class,snapshot->ball,
                                                   "tennisManager");
    int manager_matches=(manager_from_ball==manager);
    float velocity[3]={3.0f,-4.0f,5.0f};
    unsigned int velocity_input_words[3]={0,0,0};
    for(unsigned int i=0;i<3;++i) velocity_input_words[i]=((const unsigned int*)velocity)[i];
    void* args[]={velocity};void* apply_exception=0;
    void* apply_method=metadata_method_checked(ball_class,"ApplyFatiguePace",1);
    unsigned int pace_before=0,pace_after=0;
    int pace_before_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                 "flightPace",&pace_before,1);
    int apply_ok=metadata_invoke_void(apply_method,snapshot->ball,args,&apply_exception);
    int pace_after_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                "flightPace",&pace_after,1);
    write_text("{\"kind\":\"physics_fatigue_pace\",\"schema\":\"v014-physics-v1\",\"case_id\":");
    metadata_quoted(fixture->case_id);write_text(",\"gain_source\":");
    metadata_quoted(fixture->gain_source);write_text(",\"input\":{\"score_home\":");
    metadata_words(&fixture->score_home,1);write_text(",\"score_away\":");
    metadata_words(&fixture->score_away,1);write_text(",\"rallyShotCount\":");
    metadata_words(&fixture->rally_count,1);write_text(",\"fatigue_gain_f32_words\":");
    metadata_words(&fixture->gain_words,1);write_text(",\"velocity_f32_words\":");
    metadata_words(velocity_input_words,3);write_text("},\"readback\":{\"fields_applied\":");
    if(applied)write_text("true");else write_text("false");
    write_text(",\"fatigue_gain_f32_words\":");metadata_words(&read_gain,1);
    write_text(",\"rallyShotCount\":");metadata_words(&read_rally,1);
    write_text(",\"score_home\":");metadata_words(&read_home,1);
    write_text(",\"score_away\":");metadata_words(&read_away,1);
    write_text(",\"deuce_active_points\":");if(deuce_ok)metadata_words(&deuce_active,1);else write_text("null");
    write_text(",\"rally_active_points\":");if(rally_ok)metadata_words(&rally_active,1);else write_text("null");
    write_text(",\"scale_f32_words\":");if(scale_ok)metadata_words(active_scale,1);else write_text("null");
    write_text(",\"getter_gain_f32_words\":");if(gain_ok)metadata_words(getter_gain,1);else write_text("null");
    write_text(",\"ball_manager_object\":");metadata_object_pointer(manager_from_ball);
    write_text(",\"manager_matches\":");if(manager_matches)write_text("true");else write_text("false");
    write_text(",\"manager_available\":");if(manager_matches)write_text("true");else write_text("false");
    write_text(",\"flight_pace_before_f32_words\":");if(pace_before_ok)metadata_words(&pace_before,1);else write_text("null");
    write_text(",\"flight_pace_after_f32_words\":");if(pace_after_ok)metadata_words(&pace_after,1);else write_text("null");
    write_text(",\"velocity_out_f32_words\":");metadata_words((const unsigned int*)velocity,3);
    write_text(",\"invoke_ok\":");if(apply_ok)write_text("true");else write_text("false");
    write_text(",\"getter_ok\":");if(deuce_ok&&rally_ok&&scale_ok&&gain_ok)write_text("true");else write_text("false");
    write_text("},\"exceptions\":{\"deuce\":");if(deuce_exception)write_text("true");else write_text("false");
    write_text(",\"rally\":");if(rally_exception)write_text("true");else write_text("false");
    write_text(",\"scale\":");if(scale_exception)write_text("true");else write_text("false");
    write_text(",\"gain\":");if(gain_exception)write_text("true");else write_text("false");
    write_text(",\"apply\":");if(apply_exception)write_text("true");else write_text("false");
    write_text("}}\n");
    ++metadata_fatigue_rows;++metadata_physics_rows;
    int restored=metadata_restore_physics_snapshot(manager,manager_class,snapshot);
    metadata_write_physics_restore(manager,manager_class,"fatigue_pace",fixture->case_id,
                                   restored,snapshot);
    return applied&&manager_matches&&deuce_ok&&rally_ok&&scale_ok&&gain_ok&&apply_ok&&
        pace_before_ok&&pace_after_ok&&restored&&
        metadata_physics_snapshot_matches(manager,manager_class,snapshot);
}

static int metadata_apply_force_case(void* manager,void* manager_class,
                                     const metadata_physics_snapshot* snapshot,
                                     void* ball_class,void* force_method,
                                     const char* case_id,unsigned int case_index,
                                     const char* pace_case,unsigned int pace_words,
                                     int shot_type,unsigned int curve_words,
                                     unsigned int velocity_words[3],
                                     const char* curve_case,const char* velocity_case,
                                     unsigned int dt_words) {
    if(!metadata_restore_physics_snapshot(manager,manager_class,snapshot)) return 0;
    int applied=metadata_set_words_field(ball_class,snapshot->ball,"flightPace",
                                         &pace_words,1);
    metadata_float_bits curve_bits;curve_bits.words=curve_words;
    metadata_float_bits dt_bits;dt_bits.words=dt_words;
    metadata_float_bits velocity_bits[3];
    float velocity[3];
    for(unsigned int i=0;i<3;++i) {
        velocity_bits[i].words=velocity_words[i];velocity[i]=velocity_bits[i].value;
    }
    float curve=curve_bits.value;float dt=dt_bits.value;int shot=shot_type;
    unsigned int effective[1]={0};void* effective_exception=0;
    int effective_ok=metadata_read_getter_words(ball_class,snapshot->ball,
        "get_EffectiveMaxSpeed",0,effective,1,&effective_exception);
    void* args[]={velocity,&curve,&shot,&dt};void* force_exception=0;
    int invoke_ok=metadata_invoke_void(force_method,snapshot->ball,args,&force_exception);
    unsigned int pace_after=0;
    int pace_after_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                "flightPace",&pace_after,1);
    write_text("{\"kind\":\"physics_flight_forces\",\"schema\":\"v014-physics-v1\",\"case_id\":");
    metadata_quoted(case_id);write_text(",\"case_index\":");metadata_number(case_index);
    write_text(",\"input\":{\"pace_case\":");metadata_quoted(pace_case);
    write_text(",\"flight_pace_f32_words\":");
    metadata_words(&pace_words,1);write_text(",\"shot_type\":");metadata_write_signed_decimal(shot);
    write_text(",\"curve_case\":");metadata_quoted(curve_case);
    write_text(",\"curve_f32_words\":");metadata_words(&curve_words,1);
    write_text(",\"velocity_case\":");metadata_quoted(velocity_case);
    write_text(",\"velocity_f32_words\":");metadata_words(velocity_words,3);
    write_text(",\"dt_f32_words\":");metadata_words(&dt_words,1);
    write_text(",\"gravity_field_f32_words\":");metadata_words(&snapshot->ball_arcade_gravity,1);
    write_text(",\"max_speed_field_f32_words\":");metadata_words(&snapshot->ball_max_speed,1);
    write_text(",\"curve_team\":");metadata_words(&snapshot->ball_curve_team,1);
    write_text("},\"output\":{\"velocity_f32_words\":");
    metadata_words((const unsigned int*)velocity,3);write_text(",\"curve_f32_words\":");
    metadata_words((const unsigned int*)&curve,1);write_text(",\"flight_pace_f32_words\":");
    if(pace_after_ok)metadata_words(&pace_after,1);else write_text("null");
    write_text(",\"effective_max_speed_f32_words\":");if(effective_ok)metadata_words(effective,1);else write_text("null");
    write_text("},\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
    write_text(",\"effective_max_speed_ok\":");if(effective_ok)write_text("true");else write_text("false");
    write_text(",\"exceptions\":{\"force\":");if(force_exception)write_text("true");else write_text("false");
    write_text(",\"effective_max_speed\":");if(effective_exception)write_text("true");else write_text("false");
    write_text("}}\n");
    ++metadata_force_rows;++metadata_physics_rows;
    int restored=metadata_restore_physics_snapshot(manager,manager_class,snapshot);
    metadata_write_physics_restore(manager,manager_class,"flight_forces",case_id,
                                   restored,snapshot);
    return applied&&effective_ok&&invoke_ok&&pace_after_ok&&restored&&
        metadata_physics_snapshot_matches(manager,manager_class,snapshot);
}

static int metadata_apply_integrate_case(void* manager,void* manager_class,
                                         const metadata_physics_snapshot* snapshot,
                                         void* ball_class,void* integrate_method,
                                         const char* case_id,unsigned int case_index,
                                         const char* dt_case,unsigned int dt_words,
                                         const char* pace_case,unsigned int pace_words,
                                         int shot_type,const char* curve_case,
                                         unsigned int curve_words,unsigned int curve_team,
                                         const char* velocity_case,
                                         const unsigned int position_words[3],
                                         const unsigned int velocity_words[3]) {
    if(!metadata_restore_physics_snapshot(manager,manager_class,snapshot)) return 0;
    int pace_set=metadata_set_words_field(ball_class,snapshot->ball,"flightPace",
                                          &pace_words,1);
    int team_set=metadata_set_words_field(ball_class,snapshot->ball,"curveTeam",
                                          &curve_team,1);
    unsigned int pace_readback=0,team_readback=0;
    int pace_readback_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                   "flightPace",&pace_readback,1);
    int team_readback_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                   "curveTeam",&team_readback,1);
    metadata_float_bits position_bits[3];metadata_float_bits velocity_bits[3];
    float position[3];float velocity[3];
    unsigned int position_input_words[3]={0,0,0};
    unsigned int velocity_input_words[3]={0,0,0};
    for(unsigned int i=0;i<3;++i) {
        position_bits[i].words=position_words[i];position[i]=position_bits[i].value;
        velocity_bits[i].words=velocity_words[i];velocity[i]=velocity_bits[i].value;
        position_input_words[i]=((const unsigned int*)position)[i];
        velocity_input_words[i]=((const unsigned int*)velocity)[i];
    }
    metadata_float_bits curve_bits;curve_bits.words=curve_words;
    metadata_float_bits dt_bits;dt_bits.words=dt_words;
    float curve=curve_bits.value;float dt=dt_bits.value;int shot=shot_type;
    void* args[]={position,velocity,&curve,&shot,&dt};void* exception=0;
    int invoke_ok=metadata_invoke_void(integrate_method,snapshot->ball,args,&exception);
    unsigned int pace_after=0,team_after=0;
    int pace_after_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                "flightPace",&pace_after,1);
    int team_after_ok=metadata_read_words_field(ball_class,snapshot->ball,
                                                "curveTeam",&team_after,1);
    write_text("{\"kind\":\"physics_integrate_flight\",\"schema\":\"v014-physics-v1\",\"case_id\":");
    metadata_quoted(case_id);write_text(",\"case_index\":");metadata_number(case_index);
    write_text(",\"input\":{\"dt_case\":");metadata_quoted(dt_case);
    write_text(",\"dt_f32_words\":");metadata_words(&dt_words,1);
    write_text(",\"pace_case\":");metadata_quoted(pace_case);
    write_text(",\"flight_pace_f32_words\":");metadata_words(&pace_words,1);
    write_text(",\"shot_type\":");metadata_write_signed_decimal(shot);
    write_text(",\"curve_case\":");metadata_quoted(curve_case);
    write_text(",\"curve_team\":");metadata_words(&curve_team,1);
    write_text(",\"position_case\":\"synthetic-local-1-2-3\"");
    write_text(",\"velocity_case\":");metadata_quoted(velocity_case);
    write_text(",\"position_f32_words\":");metadata_words(position_input_words,3);
    write_text(",\"velocity_f32_words\":");metadata_words(velocity_input_words,3);
    write_text(",\"curve_f32_words\":");metadata_words(&curve_words,1);write_text("}");
    write_text(",\"field_readback\":{\"pace_set_ok\":");if(pace_set)write_text("true");else write_text("false");
    write_text(",\"team_set_ok\":");if(team_set)write_text("true");else write_text("false");
    write_text(",\"pace_f32_words\":");if(pace_readback_ok)metadata_words(&pace_readback,1);else write_text("null");
    write_text(",\"curve_team\":");if(team_readback_ok)metadata_words(&team_readback,1);else write_text("null");
    write_text("},\"output\":{\"position_f32_words\":");
    metadata_words((const unsigned int*)position,3);write_text(",\"velocity_f32_words\":");
    metadata_words((const unsigned int*)velocity,3);write_text(",\"curve_f32_words\":");
    metadata_words((const unsigned int*)&curve,1);write_text(",\"flight_pace_f32_words\":");
    if(pace_after_ok)metadata_words(&pace_after,1);else write_text("null");
    write_text(",\"curve_team\":");if(team_after_ok)metadata_words(&team_after,1);else write_text("null");
    write_text("},\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
    write_text(",\"exceptions\":{\"integrate\":");if(exception)write_text("true");else write_text("false");
    write_text("}}\n");
    ++metadata_integrate_rows;++metadata_physics_rows;
    int restored=metadata_restore_physics_snapshot(manager,manager_class,snapshot);
    metadata_write_physics_restore(manager,manager_class,"integrate_flight",case_id,
                                   restored,snapshot);
    return pace_set&&team_set&&pace_readback_ok&&team_readback_ok&&
        pace_readback==pace_words&&team_readback==curve_team&&invoke_ok&&
        pace_after_ok&&team_after_ok&&restored&&
        metadata_physics_snapshot_matches(manager,manager_class,snapshot);
}

static void metadata_write_physics_failure(const char* component,const char* case_id,
                                           const char* reason) {
    write_text("{\"kind\":\"physics_fixture_failure\",\"schema\":\"v014-physics-v1\",\"component\":");
    metadata_quoted(component);write_text(",\"case_id\":");metadata_quoted(case_id);
    write_text(",\"reason\":");metadata_quoted(reason);write_text("}\n");
}

static int metadata_physics_capture(void* manager,void* manager_class) {
    metadata_physics_snapshot snapshot;
    if(!metadata_take_physics_snapshot(manager,manager_class,&snapshot)) {
        if(!metadata_physics_wait_logged) {
            write_text("{\"kind\":\"physics_fixture_status\",\"schema\":\"v014-physics-v1\",\"status\":\"waiting_for_initialized_ball\"}\n");
            flush_log();metadata_physics_wait_logged=1;
        }
        return 0;
    }
    void* ball_class=V014_FN(il2cpp_object_get_class,void*,void*)(snapshot.ball);
    void* effective_method=metadata_method_checked(ball_class,"get_EffectiveMaxSpeed",0);
    void* fatigue_method=metadata_method_checked(ball_class,"ApplyFatiguePace",1);
    void* force_method=metadata_method_checked(ball_class,"ApplyFlightForces",4);
    void* integrate_method=metadata_method_checked(ball_class,"IntegrateFlight",5);
    void* net_method=metadata_method_checked(ball_class,"NetRight",1);
    void* scale_method=metadata_method_checked(manager_class,"get_FatigueShotSpeedScale",0);
    void* gain_method=metadata_method_checked(manager_class,"get_FatigueShotSpeedGain",0);
    void* deuce_method=metadata_method_checked(manager_class,"get_DeuceFatigueActivePoints",0);
    void* rally_method=metadata_method_checked(manager_class,"get_RallyFatigueActivePoints",0);
    int methods_ok=effective_method&&fatigue_method&&force_method&&integrate_method&&net_method&&
        scale_method&&gain_method&&deuce_method&&rally_method;
    write_text("{\"kind\":\"physics_fixture_status\",\"schema\":\"v014-physics-v1\",\"status\":");
    if(methods_ok) metadata_quoted("started");
    else metadata_quoted("missing_validated_method");
    write_text(",\"methods\":{\"get_EffectiveMaxSpeed\":");if(effective_method)write_text("true");else write_text("false");
    write_text(",\"ApplyFatiguePace\":");if(fatigue_method)write_text("true");else write_text("false");
    write_text(",\"ApplyFlightForces\":");if(force_method)write_text("true");else write_text("false");
    write_text(",\"IntegrateFlight\":");if(integrate_method)write_text("true");else write_text("false");
    write_text(",\"NetRight\":");if(net_method)write_text("true");else write_text("false");
    write_text(",\"get_FatigueShotSpeedScale\":");if(scale_method)write_text("true");else write_text("false");
    write_text(",\"get_FatigueShotSpeedGain\":");if(gain_method)write_text("true");else write_text("false");
    write_text(",\"get_DeuceFatigueActivePoints\":");if(deuce_method)write_text("true");else write_text("false");
    write_text(",\"get_RallyFatigueActivePoints\":");if(rally_method)write_text("true");else write_text("false");
    write_text("}}\n");
    if(!methods_ok) {
        metadata_write_physics_failure("all","methods","required metadata method missing or arity mismatch");
        metadata_physics_done=1;flush_log();return 0;
    }
    metadata_write_physics_context(manager,manager_class,&snapshot);

    const unsigned int pace_words[]={
        0x3f800000, /* 1.0 */
        0x3f801d7e, /* 1.0009 */
        0x3f8020c4, /* threshold - 1 ulp */
        0x3f8020c5, /* exact threshold */
        0x3f8020c6, /* threshold + 1 ulp */
        0x3f8a3d71, /* 1.08 */
        0x3fc00000, /* 1.5 */
        0x40000000  /* 2.0 */
    };
    const char* pace_names[]={"one","below-threshold","threshold-minus-ulp",
        "threshold","threshold-plus-ulp","one-point-zero-eight","one-point-five","two"};
    int all_ok=1;
    for(unsigned int i=0;i<sizeof(pace_words)/sizeof(pace_words[0]);++i) {
        if(!metadata_restore_physics_snapshot(manager,manager_class,&snapshot)) {
            metadata_write_physics_failure("effective_max_speed",pace_names[i],"restore_before_case_failed");
            all_ok=0;break;
        }
        int case_ok=metadata_write_effective_case(manager,manager_class,&snapshot,ball_class,
                                                  pace_names[i],pace_words[i]);
        int restored=metadata_restore_physics_snapshot(manager,manager_class,&snapshot);
        metadata_write_physics_restore(manager,manager_class,"effective_max_speed",
                                       pace_names[i],restored,&snapshot);
        if(!case_ok||!restored||!metadata_physics_snapshot_matches(manager,manager_class,&snapshot)) {
            if(!case_ok) metadata_write_physics_failure("effective_max_speed",pace_names[i],
                                                        "setter/readback/getter failed");
            all_ok=0;break;
        }
    }

    int grace=(int)snapshot.rally_grace;
    if(grace<1) grace=1;
    int rally_zero=grace-1;if(rally_zero<0)rally_zero=0;
    int rally_one=grace;
    int rally_two=grace+1;
    const metadata_fatigue_case fatigue_cases[]={
        {"zero-active",0,0,(unsigned int)rally_zero,snapshot.fatigue_gain,"initialized_gain"},
        {"rally-grace-boundary",0,0,(unsigned int)rally_one,snapshot.fatigue_gain,"initialized_gain"},
        {"rally-active-two",0,0,(unsigned int)rally_two,snapshot.fatigue_gain,"initialized_gain"},
        {"threshold-below-synthetic",0,0,(unsigned int)rally_one,0x3a831200,"synthetic_gain_for_threshold_below"},
        {"threshold-at-synthetic",0,0,(unsigned int)rally_one,0x3a8312ff,"synthetic_gain_for_threshold_at"},
        {"threshold-above-synthetic",0,0,(unsigned int)rally_one,0x3a832000,"synthetic_gain_for_threshold_above"},
        {"score-deuce-context",4,4,(unsigned int)rally_zero,snapshot.fatigue_gain,"initialized_gain_score_mutation"},
        {"score-deuce-active-context",5,5,(unsigned int)rally_zero,snapshot.fatigue_gain,"initialized_gain_score_mutation"}
    };
    for(unsigned int i=0;i<sizeof(fatigue_cases)/sizeof(fatigue_cases[0])&&all_ok;++i) {
        if(!metadata_apply_fatigue_case(manager,manager_class,&snapshot,ball_class,
                                        &fatigue_cases[i])) {
            all_ok=0;
            metadata_write_physics_failure("fatigue_pace",fatigue_cases[i].case_id,
                                           "case invocation or restore failed");
            break;
        }
    }

    const char* pace_case_names[]={"one","below-threshold","threshold-minus-ulp",
        "threshold","threshold-plus-ulp","one-point-zero-eight","one-point-five","two"};
    const int shot_types[]={0,5,6};
    const char* shot_names[]={"shot-0","shot-5","shot-6"};
    const unsigned int curve_words[]={0x00000000,0x40800000,0xc0800000};
    const char* curve_names[]={"zero","positive-four","negative-four"};
    const unsigned int velocity_words[][3]={
        {0x40800000,0x40000000,0x40400000}, /* (4,2,3), uncapped candidate */
        {0x447a0000,0x00000000,0x00000000}  /* (1000,0,0), capped candidate */
    };
    const char* velocity_names[]={"uncapped-candidate","capped-candidate"};
    unsigned int force_index=0;
    for(unsigned int p=0;p<sizeof(pace_words)/sizeof(pace_words[0])&&all_ok;++p)
        for(unsigned int s=0;s<sizeof(shot_types)/sizeof(shot_types[0])&&all_ok;++s)
            for(unsigned int c=0;c<sizeof(curve_words)/sizeof(curve_words[0])&&all_ok;++c)
                for(unsigned int v=0;v<sizeof(velocity_words)/sizeof(velocity_words[0])&&all_ok;++v) {
                    if(!metadata_apply_force_case(manager,manager_class,&snapshot,ball_class,
                            force_method,"flight-force-matrix",force_index++,pace_case_names[p],
                            pace_words[p],shot_types[s],curve_words[c],
                            (unsigned int*)velocity_words[v],curve_names[c],velocity_names[v],
                            1016833504U)) {
                        all_ok=0;
                        metadata_write_physics_failure("flight_forces","flight-force-matrix",
                                                       "case invocation or restore failed");
                    }
                }
    const unsigned int integrate_dt_words[]={
        0x00000000, /* early dt == 0 */
        0x358637bd, /* native early-stop threshold (1e-6) */
        0x3c9ba5e0, /* captured fixed delta */
        0x3da3d70a, /* ordinary multi-substep dt ~= 0.08 */
        0x3f000000, /* ordinary longer multi-substep dt == 0.5 */
        0x40a00000  /* bounded high-speed dt == 5 */
    };
    const char* integrate_dt_names[]={"early-zero","early-threshold",
        "ordinary-one-step","ordinary-multi-step","ordinary-long-multi-step",
        "bounded-24-high-speed"};
    const unsigned int integrate_position_words[3]={
        0x3f800000,0x40000000,0x40400000 /* (1,2,3) */
    };
    const unsigned int integrate_velocity_low[3]={
        0x40800000,0x40000000,0x40400000 /* (4,2,3) */
    };
    const unsigned int integrate_velocity_high[3]={
        0x447a0000,0x00000000,0x00000000 /* (1000,0,0) */
    };
    const int integrate_shot_types[]={0,5,6};
    const unsigned int integrate_curve_words[]={0x00000000,0x40800000,0xc0800000};
    const char* integrate_curve_names[]={"zero","positive-four","negative-four"};
    const char* integrate_velocity_names[] = {"uncapped-candidate", "capped-candidate"};
    unsigned int integrate_index=0;
    for(unsigned int d=0;d<sizeof(integrate_dt_words)/sizeof(integrate_dt_words[0])&&all_ok;++d)
        for(unsigned int s=0;s<sizeof(integrate_shot_types)/sizeof(integrate_shot_types[0])&&all_ok;++s)
            for(unsigned int team=0;team<2&&all_ok;++team) {
                unsigned int pace_index=(d*3+s*2+team)%
                    (sizeof(pace_words)/sizeof(pace_words[0]));
                const unsigned int* velocity=(d==5)?integrate_velocity_high:integrate_velocity_low;
                if(!metadata_apply_integrate_case(manager,manager_class,&snapshot,ball_class,
                        integrate_method,"integrate-flight-matrix",integrate_index++,
                        integrate_dt_names[d],integrate_dt_words[d],pace_case_names[pace_index],
                        pace_words[pace_index],integrate_shot_types[s],integrate_curve_names[s],
                        integrate_curve_words[s],team,integrate_velocity_names[d==5],
                        integrate_position_words,velocity)) {
                    all_ok=0;
                    metadata_write_physics_failure("integrate_flight","integrate-flight-matrix",
                                                   "case invocation or restore failed");
                }
            }
    int final_restore=metadata_restore_physics_snapshot(manager,manager_class,&snapshot);
    int final_matches=metadata_physics_snapshot_matches(manager,manager_class,&snapshot);
    write_text("{\"kind\":\"physics_fixture_status\",\"schema\":\"v014-physics-v1\",\"status\":");
    if(all_ok&&final_restore&&final_matches)metadata_quoted("complete");else metadata_quoted("failed");
    write_text(",\"effective_max_speed_rows\":");metadata_number(metadata_effective_rows);
    write_text(",\"fatigue_pace_rows\":");metadata_number(metadata_fatigue_rows);
    write_text(",\"flight_forces_rows\":");metadata_number(metadata_force_rows);
    write_text(",\"integrate_flight_rows\":");metadata_number(metadata_integrate_rows);
    write_text(",\"rows\":");metadata_number(metadata_physics_rows);
    write_text(",\"final_restore_api_ok\":");if(final_restore)write_text("true");else write_text("false");
    write_text(",\"final_matches_snapshot\":");if(final_matches)write_text("true");else write_text("false");
    write_text("}\n");
    metadata_physics_success=all_ok&&final_restore&&final_matches;
#if defined(V014_EVENT_CAPTURE) || defined(V014_NATURAL_TRACE)
    /* Event and natural-trace modes own the terminal marker.  The ordinary
     * metadata/physics mode keeps the historical marker here. */
#else
    if(metadata_physics_success) {
        write_text("{\"kind\":\"metadata_capture_complete\",\"fixture\":\"serving-clamp-snapshot-restore+physics-v014+integrate-flight\",\"status\":\"complete\",\"physics_rows\":");
        metadata_number(metadata_physics_rows);write_text("}\n");
    } else {
        write_text("{\"kind\":\"metadata_capture_complete\",\"fixture\":\"serving-clamp-snapshot-restore+physics-v014+integrate-flight\",\"status\":\"failed\",\"physics_rows\":");
        metadata_number(metadata_physics_rows);write_text("}\n");
    }
#endif
    metadata_physics_done=1;
#if !defined(V014_EVENT_CAPTURE) && !defined(V014_NATURAL_TRACE)
    metadata_capture_complete=1;
#endif
    flush_log();
    return all_ok&&final_restore&&final_matches;
}

#if defined(V014_EVENT_CAPTURE) || defined(V014_NATURAL_TRACE)
/*
 * The next captures intentionally use only the metadata field/method APIs.
 * These lists are the trace contract: they name the state that is observable
 * at the managed boundary without turning the static v0.14 offsets into a
 * second, unsafe ABI.  UInt64 values are emitted as two little-endian words.
 */
typedef struct {
    const char* name;
    unsigned int words;
} metadata_trace_field_spec;

static const metadata_trace_field_spec metadata_trace_manager_fields[] = {
    {"<ServeTossed>k__BackingField",1},
    {"<ServeCountdownDisplay>k__BackingField",1},
    {"<CurrentGameState>k__BackingField",1},
    {"<IsTrainingMode>k__BackingField",1},
    {"<TrainingPlayerTeam>k__BackingField",1},
    {"<PendingTrainingLoad>k__BackingField",1},
    {"<ServingTeam>k__BackingField",1},
    {"<LastPointWinner>k__BackingField",1},
    {"<LastPointWasAce>k__BackingField",1},
    {"<LastServeWasFault>k__BackingField",1},
    {"<LastPointWasDoubleFault>k__BackingField",1},
    {"<LastPointWasFoul>k__BackingField",1},
    {"betweenPointUntilTick",2},
    {"serveDeadlineTick",2},
    {"serveSetupUntilTick",2},
    {"serveSetupEarliestEndTick",2},
    {"serveSetupReceiverSettledTicks",1},
    {"pointResolved",1},
    {"serveFaultsThisPoint",1},
    {"consecutiveDoubleFaultPoints",1},
    {"doubleFaultStreakTeams",1},
    {"alternatingFoulStreak",1},
    {"lastFoulingTeam",1},
    {"consecutiveAceStreak",1},
    {"rallyShotCount",1},
    {"lastServeCountdownShown",1},
    {"lastSpawnedTeam",1},
    {"serveInPlay",1},
    {"receiverTouchedBall",1},
    {"lastShotIncomingBin",1},
    {"lastShotWasServe",1},
    {"trainingNextShotTick",2},
    {"trainingAwaitingShot",1},
    {"trainingPlayerServesNext",1},
    {"trainingPlayerServeTurn",1},
    {"trainingServeFromAdCourt",1}
};

static const metadata_trace_field_spec metadata_trace_score_fields[] = {
    {"<PointsToWinGame>k__BackingField",1},
    {"<MatchesToWin>k__BackingField",1},
    {"<HomePoints>k__BackingField",1},
    {"<AwayPoints>k__BackingField",1},
    {"<HomeMatches>k__BackingField",1},
    {"<AwayMatches>k__BackingField",1},
    {"<GameOver>k__BackingField",1},
    {"<WinnerTeam>k__BackingField",1},
    {"<JustWonGame>k__BackingField",1},
    {"<LastGameWinner>k__BackingField",1},
    {"<AwardSerial>k__BackingField",1}
};

static const metadata_trace_field_spec metadata_trace_ball_fields[] = {
    {"<LastShotType>k__BackingField",1},
    {"<BounceCountOnCurrentHalf>k__BackingField",1},
    {"<CurrentHalf>k__BackingField",1},
    {"<InPlay>k__BackingField",1},
    {"<IsServe>k__BackingField",1},
    {"<HasCrossedNet>k__BackingField",1},
    {"<HasBouncedInSinceHit>k__BackingField",1},
    {"<LastBouncePosition>k__BackingField",3},
    {"<HasFire>k__BackingField",1},
    {"<Velocity>k__BackingField",3},
    {"<Position>k__BackingField",3},
    {"<Radius>k__BackingField",1},
    {"pendingTopspinKick",1},
    {"pendingSliceBounce",1},
    {"pendingDropBounce",1},
    {"sliceCurve",1},
    {"curveTeam",1},
    {"lastHitPower",1},
    {"flightPace",1},
    {"lastStrikePos",3},
    {"peakYSinceHit",1},
    {"lastPosition",3},
    {"cachedPredictedBounce",3},
    {"cachedPredictedSecondBounce",3},
    {"cachedPredictedBounceTime",1},
    {"cachedPredictedSecondBounceTime",1},
    {"hasCachedPredictedBounce",1},
    {"hasCachedPredictedSecondBounce",1},
    {"predictedBounceAge",1},
    {"shotBounceCount",1},
    {"releaseScaleElapsed",1}
};

static const metadata_trace_field_spec metadata_trace_player_fields[] = {
    {"Team",1},
    {"<SwingState>k__BackingField",1},
    {"<LatchedShotType>k__BackingField",1},
    {"<LatchedTrickCurveSide>k__BackingField",1},
    {"<LatchedAim>k__BackingField",3},
    {"<ShotCharge>k__BackingField",1},
    {"<LastShotOption>k__BackingField",1},
    {"<LastShotType>k__BackingField",1},
    {"<LastShotFullyCharged>k__BackingField",1},
    {"<LastContactTiming>k__BackingField",1},
    {"<RandomShotOption>k__BackingField",1},
    {"<RandomAimTarget>k__BackingField",3},
    {"<LatchedOpponentShotGuess>k__BackingField",3},
    {"<HasLatchedOpponentShotGuess>k__BackingField",1},
    {"<ServeAimHint>k__BackingField",3},
    {"<HasServeAimHint>k__BackingField",1},
    {"<LastSwingMissed>k__BackingField",1},
    {"<IsSprinting>k__BackingField",1},
    {"moveDestination",3},
    {"aimTarget",3},
    {"graphShotAim",3},
    {"hasGraphShotAim",1},
    {"hasMoveInput",1},
    {"shotHeld",1},
    {"sprintHeld",1},
    {"lastShotHeld",1},
    {"shotOption",1},
    {"chargedShotOption",1},
    {"slideTarget",3},
    {"swingCommitUntilTick",2},
    {"recoverUntilTick",2},
    {"serveOverhand",1},
    {"chargePendingRelease",1},
    {"chargeElapsed",1},
    {"swingCalloutUntil",1},
    {"serveTossAnimPending",1},
    {"serveTossReleaseTick",2},
    {"serveTossPressTick",2},
    {"serveImpactTick",2},
    {"serveContactWindowUntilTick",2},
    {"serveReleaseAnimEndTick",2},
    {"serveAwaitingSwing",1},
    {"serveTossWasTap",1},
    {"rallyReleaseAnimEndTick",2},
    {"latchedSwingBackhand",1}
};

/* These are the graph/brain outputs consumed by TennisPlayer.ReadBrainInputs.
 * They are deliberately named through metadata rather than read through the
 * static offsets listed in the v0.14 catalogue.  Keeping them in a separate
 * group makes the consumption boundary explicit in a natural trace while
 * retaining the full player state above for replay context. */
static const metadata_trace_field_spec metadata_trace_brain_fields[] = {
    {"moveDestination",3},
    {"aimTarget",3},
    {"graphShotAim",3},
    {"hasGraphShotAim",1},
    {"hasMoveInput",1},
    {"shotHeld",1},
    {"sprintHeld",1},
    {"lastShotHeld",1},
    {"shotOption",1},
    {"chargedShotOption",1},
    {"trickDropIntent",1},
    {"slideTarget",3}
};

/* DeterministicMover's managed state and its owned Rigidbody are useful at
 * the graph-consumption boundary.  The Rigidbody getters below are resolved
 * from the live object's class, so this remains safe if Unity changes a
 * native implementation address in another v0.14 build. */
static const metadata_trace_field_spec metadata_trace_mover_fields[] = {
    {"pathTeamId",1},
    {"enableDynamicPathfinding",1},
    {"targetDestination",3},
    {"hasDestination",1},
    {"_reachedDestination",1},
    {"currentVelocity",3},
    {"jumpRequested",1},
    {"isGrounded",1},
    {"jumpCooldownTimer",1},
    {"_movementFrozen",1},
    {"_pathWaypointCount",1},
    {"_pathWaypointIndex",1},
    {"_pathReplanCountdown",1},
    {"_lastPlannedGoal",3},
    {"_pathGridCenter",3},
    {"_pathGridSize",1},
    {"_pathBufferCapacity",1},
    {"_pathSearchGoalX",1},
    {"_pathSearchGoalZ",1}
};

static int metadata_set_object_field(void* klass,void* object,const char* name,
                                     void* value) {
    void* field=metadata_find_field(klass,name);
    if(!field||!object) return 0;
    /* il2cpp_field_set_value takes the object reference itself for a managed
     * reference field (the scalar helper above takes the address of its raw
     * words).  Passing &value would store this stack slot as a dangling
     * managed reference and aliases every restored object field. */
    V014_FN(il2cpp_field_set_value,void,void*,void*,void*)(object,field,value);
    return 1;
}

/* --- version-variant fields ------------------------------------------------
 * Some named fields exist in one game build and not another.  An ABSENT
 * optional field is a *version difference*, not a read error, so it must emit
 * `null` without failing the enclosing group (a failing group aborts the whole
 * initial snapshot and the natural trace never starts).
 *
 * v0.15f removed the ball bounce-time cache when "ball time to ground being
 * cached" was fixed: `cachedPredictedBounceTime` (v0.14 @0x140),
 * `cachedPredictedSecondBounceTime` (@0x144) and `predictedBounceAge` (@0x14c)
 * no longer exist as fields; `hasCachedPredictedBounce` moved 0x148 -> 0x140.
 * v0.14 still has all three, so they stay listed for that build. */
static int metadata_name_is(const char* a,const char* b) {
    while(*a&&*a==*b){++a;++b;}
    return *a==*b;
}
static int metadata_trace_field_optional(const char* name) {
    static const char* optional[]={
        "cachedPredictedBounceTime",
        "cachedPredictedSecondBounceTime",
        "predictedBounceAge",
    };
    for(unsigned int i=0;i<sizeof(optional)/sizeof(optional[0]);++i)
        if(metadata_name_is(name,optional[i])) return 1;
    return 0;
}

static int metadata_trace_write_field_group(void* klass,void* object,
                                            const metadata_trace_field_spec* specs,
                                            unsigned int count) {
    int ok=1;
    write_text("{");
    for(unsigned int i=0;i<count;++i) {
        if(i) write_text(",");
        metadata_quoted(specs[i].name);write_text(":");
        unsigned int words[4]={0,0,0,0};
        if(metadata_read_words_field(klass,object,specs[i].name,words,specs[i].words))
            metadata_words(words,specs[i].words);
        else {
            write_text("null");
            if(!metadata_trace_field_optional(specs[i].name)) ok=0;
        }
    }
    write_text("}");
    return ok;
}

static int metadata_trace_write_getter_words(void* klass,void* object,
                                             const char* name,unsigned int words_count) {
    void* method=metadata_method_checked(klass,name,0);
    unsigned int words[4]={0,0,0,0};void* exception=0;
    int invoke_ok=metadata_invoke_words(method,object,0,words,words_count,&exception);
    write_text("{\"name\":");metadata_quoted(name);
    write_text(",\"method_found\":");if(method)write_text("true");else write_text("false");
    write_text(",\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
    write_text(",\"value\":");if(invoke_ok)metadata_words(words,words_count);else write_text("null");
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text("}");
    return invoke_ok;
}

static int metadata_trace_write_stamina_context(void* player,void* player_class) {
    void* field_owner=0;void* stamina=0;
    void* field=metadata_find_object_field_by_type(player_class,player,"Stamina",
                                                   &field_owner,&stamina);
    void* stamina_class=stamina?V014_FN(il2cpp_object_get_class,void*,void*)(stamina):0;
    int linked=field&&stamina&&stamina_class;
    write_text("{\"source\":\"metadata declared-type link\",\"requested_type\":\"Stamina\"");
    write_text(",\"pointer_field\":");
    if(field) {
        const char* name=V014_FN(il2cpp_field_get_name,const char*,void*)(field);
        void* type=V014_FN(il2cpp_field_get_type,void*,void*)(field);
        char* type_name=type?V014_FN(il2cpp_type_get_name,char*,void*)(type):0;
        unsigned int flags=V014_FN(il2cpp_field_get_flags,unsigned int,void*)(field);
        int offset=V014_FN(il2cpp_field_get_offset,int,void*)(field);
        write_text("{\"owner\":");metadata_nullable_string(field_owner?
            V014_FN(il2cpp_class_get_name,const char*,void*)(field_owner):0);
        write_text(",\"name\":");metadata_nullable_string(name);
        write_text(",\"type\":");metadata_nullable_string(type_name);
        write_text(",\"flags\":");metadata_number(flags);
        write_text(",\"offset\":\"");metadata_write_signed_hex(offset);
        write_text("\",\"read_ok\":");if(stamina)write_text("true");else write_text("false");
        write_text("}");
        if(type_name) V014_FN(il2cpp_free,void,void*)(type_name);
    } else write_text("null");
    write_text(",\"object\":");metadata_object_pointer(stamina);
    write_text(",\"class\":");metadata_nullable_string(stamina_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(stamina_class):0);
    write_text(",\"fields\":");
    if(stamina_class&&stamina) metadata_trace_write_dynamic_fields(stamina_class,stamina);
    else write_text("[]");
    write_text(",\"read_ok\":");if(linked)write_text("true");else write_text("false");
    write_text("}");
    return linked;
}

static int metadata_trace_write_mover_object_context(void* mover,
                                                     void* mover_class,
                                                     void* fallback_object,
                                                     void* fallback_class) {
    int ok=mover!=0&&mover_class!=0;
    write_text("{\"object\":");metadata_object_pointer(mover);
    write_text(",\"class\":");metadata_nullable_string(mover_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(mover_class):0);
    write_text(",\"fields\":");
    if(!metadata_trace_write_field_group(mover_class,mover,metadata_trace_mover_fields,
            sizeof(metadata_trace_mover_fields)/sizeof(metadata_trace_mover_fields[0]))) ok=0;
    void* rb=mover?metadata_field_object(mover_class,mover,"rb"):0;
    if(!rb && fallback_object && fallback_class)
        rb=metadata_field_object(fallback_class,fallback_object,"cachedRb");
    void* rb_class=rb?V014_FN(il2cpp_object_get_class,void*,void*)(rb):0;
    write_text(",\"rigidbody\":{\"object\":");metadata_object_pointer(rb);
    write_text(",\"class\":");metadata_nullable_string(rb_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(rb_class):0);
    write_text(",\"position\":");
    if(!metadata_trace_write_getter_words(rb_class,rb,"get_position",3)) ok=0;
    write_text(",\"linear_velocity\":");
    if(!metadata_trace_write_getter_words(rb_class,rb,"get_linearVelocity",3)) ok=0;
    write_text(",\"is_kinematic\":");
    if(!metadata_trace_write_getter_words(rb_class,rb,"get_isKinematic",1)) ok=0;
#ifdef V014_NATURAL_TRACE
    write_text(",\"rotation\":");
    if(!metadata_trace_write_getter_words(rb_class,rb,"get_rotation",4)) ok=0;
    write_text(",\"angular_velocity\":");
    if(!metadata_trace_write_getter_words(rb_class,rb,"get_angularVelocity",3)) ok=0;
#endif
    write_text("}");
    write_text(",\"read_ok\":");if(ok)write_text("true");else write_text("false");
    write_text("}");
    return ok;
}

static int metadata_trace_write_mover_context(void* player,void* player_class) {
    void* mover=metadata_field_object(player_class,player,"mover");
    void* mover_class=mover?V014_FN(il2cpp_object_get_class,void*,void*)(mover):0;
    return metadata_trace_write_mover_object_context(mover,mover_class,
                                                     player,player_class);
}

#ifdef V014_NATURAL_TRACE
/* The callback receiver is the actual managed component, not a guessed player
 * offset.  Keep this packet beside the manager state so a replay can align
 * mover/stamina side effects with the driver boundary. */
static int metadata_natural_write_callback_context(const char* owner,
                                                   const char* name,
                                                   void* object,
                                                   void* klass) {
    int ok=1;
    if(same(owner,"DeterministicMover")&&same(name,"FixedUpdate")) {
        write_text("{\"kind\":\"DeterministicMover\",\"context\":");
        if(!metadata_trace_write_mover_object_context(object,klass,0,0)) ok=0;
        write_text(",\"read_ok\":");if(ok)write_text("true");else write_text("false");
        write_text("}");
        return ok;
    }
    if(same(owner,"Stamina")&&same(name,"OnSimulationTick")) {
        write_text("{\"kind\":\"Stamina\",\"object\":");
        metadata_object_pointer(object);
        write_text(",\"class\":");metadata_nullable_string(klass?
            V014_FN(il2cpp_class_get_name,const char*,void*)(klass):0);
        write_text(",\"fields\":");
        if(!metadata_trace_write_dynamic_fields(klass,object)) ok=0;
        write_text(",\"read_ok\":");if(ok)write_text("true");else write_text("false");
        write_text("}");
        return ok;
    }
    write_text("null");
    return 1;
}
#endif

static int metadata_trace_write_brain_context(void* player,void* player_class) {
    int ok=1;
    void* brain=metadata_field_object(player_class,player,"Brain");
    void* brain_class=brain?V014_FN(il2cpp_object_get_class,void*,void*)(brain):0;
    write_text("{\"source\":\"TennisPlayer.ReadBrainInputs consumed managed slots\"");
    write_text(",\"brain_object\":");metadata_object_pointer(brain);
    write_text(",\"brain_class\":");metadata_nullable_string(brain_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(brain_class):0);
    write_text(",\"brain_fields\":");
    if(brain&&brain_class) metadata_trace_write_dynamic_fields(brain_class,brain);
    else write_text("[]");
    write_text(",\"fields\":");
    if(!metadata_trace_write_field_group(player_class,player,metadata_trace_brain_fields,
            sizeof(metadata_trace_brain_fields)/sizeof(metadata_trace_brain_fields[0]))) ok=0;
    write_text(",\"brain_object_read_ok\":");if(brain&&brain_class)write_text("true");else write_text("false");
    write_text(",\"read_ok\":");if(ok)write_text("true");else write_text("false");write_text("}");
    return ok;
}

static void metadata_trace_write_managed_string(void* value) {
    if(!value) { write_text("null");return; }
    if(!V014_FN(il2cpp_string_length,int,void*)||
       !V014_FN(il2cpp_string_chars,const WORD*,void*)) {
        metadata_object_pointer(value);return;
    }
    int length=V014_FN(il2cpp_string_length,int,void*)(value);
    const WORD* chars=V014_FN(il2cpp_string_chars,const WORD*,void*)(value);
    if(length<0) { metadata_object_pointer(value);return; }
    if(length>256) length=256;
    write_text("\"");
    for(int i=0;i<length;++i) {
        WORD ch=chars[i];
        if(ch=='"'||ch=='\\') write_text("\\");
        if(ch>=32&&ch<128) { char out[2]={(char)ch,0};write_text(out); }
        else {
            char out[7]={'\\','u','0','0','0','0',0};
            const char* digits="0123456789abcdef";
            out[2]=digits[(ch>>12)&15];out[3]=digits[(ch>>8)&15];
            out[4]=digits[(ch>>4)&15];out[5]=digits[ch&15];
            write_text(out);
        }
    }
    write_text("\"");
}

static int metadata_trace_write_state(void* manager,void* manager_class) {
    int ok=1;
    void* score=metadata_field_object(manager_class,manager,"<Score>k__BackingField");
    void* ball=metadata_field_object(manager_class,manager,"<Ball>k__BackingField");
    void* home=metadata_field_object(manager_class,manager,"<HomePlayer>k__BackingField");
    void* away=metadata_field_object(manager_class,manager,"<AwayPlayer>k__BackingField");
    void* score_class=score?V014_FN(il2cpp_object_get_class,void*,void*)(score):0;
    void* ball_class=ball?V014_FN(il2cpp_object_get_class,void*,void*)(ball):0;
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    write_text("{\"manager_object\":");metadata_object_pointer(manager);
    write_text(",\"manager_fields\":");
    if(!metadata_trace_write_field_group(manager_class,manager,
            metadata_trace_manager_fields,sizeof(metadata_trace_manager_fields)/sizeof(metadata_trace_manager_fields[0]))) ok=0;
    write_text(",\"score\":{\"object\":");metadata_object_pointer(score);
    write_text(",\"fields\":");
    if(!metadata_trace_write_field_group(score_class,score,metadata_trace_score_fields,
            sizeof(metadata_trace_score_fields)/sizeof(metadata_trace_score_fields[0]))) ok=0;
    write_text(",\"last_point_from\":");metadata_trace_write_managed_string(
        metadata_field_object(score_class,score,"<LastPointFromDisplay>k__BackingField"));
    write_text(",\"last_point_to\":");metadata_trace_write_managed_string(
        metadata_field_object(score_class,score,"<LastPointToDisplay>k__BackingField"));
    write_text("}");
    write_text(",\"ball\":{\"object\":");metadata_object_pointer(ball);
    write_text(",\"class\":");metadata_nullable_string(ball_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(ball_class):0);
    write_text(",\"fields\":");
    if(!metadata_trace_write_field_group(ball_class,ball,metadata_trace_ball_fields,
            sizeof(metadata_trace_ball_fields)/sizeof(metadata_trace_ball_fields[0]))) ok=0;
    write_text(",\"last_hitter_object\":");metadata_object_pointer(
        metadata_field_object(ball_class,ball,"<LastHitter>k__BackingField"));
    write_text(",\"hold_parent_object\":");metadata_object_pointer(
        metadata_field_object(ball_class,ball,"holdParent"));
    write_text(",\"tennis_manager_object\":");metadata_object_pointer(
        metadata_field_object(ball_class,ball,"tennisManager"));
    write_text("}");
    write_text(",\"players\":{\"home\":{\"object\":");metadata_object_pointer(home);
    write_text(",\"fields\":");
    if(!metadata_trace_write_field_group(home_class,home,metadata_trace_player_fields,
            sizeof(metadata_trace_player_fields)/sizeof(metadata_trace_player_fields[0]))) ok=0;
    write_text(",\"last_swing_label\":");metadata_trace_write_managed_string(
        metadata_field_object(home_class,home,"<LastSwingLabel>k__BackingField"));
    write_text(",\"stamina\":");
    if(!metadata_trace_write_stamina_context(home,home_class)) ok=0;
    write_text(",\"brain_inputs\":");
    if(!metadata_trace_write_brain_context(home,home_class)) ok=0;
    write_text(",\"mover\":");
    if(!metadata_trace_write_mover_context(home,home_class)) ok=0;
    write_text("},");
    write_text("\"away\":{\"object\":");metadata_object_pointer(away);
    write_text(",\"fields\":");
    if(!metadata_trace_write_field_group(away_class,away,metadata_trace_player_fields,
            sizeof(metadata_trace_player_fields)/sizeof(metadata_trace_player_fields[0]))) ok=0;
    write_text(",\"last_swing_label\":");metadata_trace_write_managed_string(
        metadata_field_object(away_class,away,"<LastSwingLabel>k__BackingField"));
    write_text(",\"stamina\":");
    if(!metadata_trace_write_stamina_context(away,away_class)) ok=0;
    write_text(",\"brain_inputs\":");
    if(!metadata_trace_write_brain_context(away,away_class)) ok=0;
    write_text(",\"mover\":");
    if(!metadata_trace_write_mover_context(away,away_class)) ok=0;
    write_text("}}");
    write_text("}");
    return ok;
}

static void metadata_trace_write_clock(void) {
    write_text("{\"fixed_time\":");
    if(metadata_core_time_class) metadata_write_getter(metadata_core_time_class,0,"get_fixedTime",1);
    else write_text("null");
    write_text(",\"frame_count\":");
    if(metadata_core_time_class) metadata_write_getter(metadata_core_time_class,0,"get_frameCount",1);
    else write_text("null");
    write_text(",\"fixed_delta_time\":");
    if(metadata_core_time_class) metadata_write_getter(metadata_core_time_class,0,"get_fixedDeltaTime",1);
    else write_text("null");
    write_text("}");
}

static void metadata_trace_write_method_identity(void* method,void* klass) {
    const char* owner=klass?V014_FN(il2cpp_class_get_name,const char*,void*)(klass):0;
    const char* name=method?V014_FN(il2cpp_method_get_name,const char*,const void*)(method):0;
    unsigned int args=method?V014_FN(il2cpp_method_get_param_count,unsigned int,const void*)(method):0;
    QWORD pointer=method?*(const QWORD*)method:0;
    write_text("{\"class\":");metadata_nullable_string(owner);
    write_text(",\"name\":");metadata_nullable_string(name);
    write_text(",\"args\":");metadata_number(args);
    write_text(",\"rva\":");
    if(pointer&&metadata_module_base&&metadata_module_size&&pointer>=metadata_module_base&&
       pointer<metadata_module_base+metadata_module_size) {
        write_text("\"");write_hex(pointer-metadata_module_base);write_text("\"");
    } else write_text("null");
    write_text("}");
}

#ifdef V014_NATURAL_TRACE
static const metadata_trace_field_spec metadata_trace_game_clock_fields[] = {
    {"Ticks",2},
    {"<TicksThisFrame>k__BackingField",1},
    {"<IsSolvingSimulationTick>k__BackingField",1}
};

static int metadata_natural_write_static_clock_field(void* klass,const char* name,
                                                     unsigned int* words,unsigned int count,
                                                     unsigned int* flags_out) {
    return metadata_read_static_words_field(klass,name,words,count,flags_out);
}

static void metadata_natural_resolve_game_manager(void) {
    if(metadata_natural_game_manager||metadata_natural_game_manager_class||
       !metadata_game_image) return;
    metadata_natural_game_manager_class=V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
        (metadata_game_image,"","GameManager");
    void* method=metadata_method_checked(metadata_natural_game_manager_class,
                                          "get_Instance",0);
    void* exception=0;
    void* result=metadata_invoke(method,0,0,&exception);
    if(result&&!exception) {
        metadata_natural_game_manager=result;
        void* result_class=V014_FN(il2cpp_object_get_class,void*,void*)(result);
        if(result_class) metadata_natural_game_manager_class=result_class;
    }
}

static void metadata_natural_write_random_state(void) {
    void* method=metadata_method_checked(metadata_core_random_class,"get_state",0);
    unsigned int words[4]={0,0,0,0};void* exception=0;
    int invoke_ok=metadata_invoke_words(method,0,0,words,4,&exception);
    write_text("{\"method\":");metadata_trace_write_method_identity(method,metadata_core_random_class);
    write_text(",\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
    write_text(",\"words\":");if(invoke_ok)metadata_words(words,4);else write_text("null");
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"exception_object\":");metadata_object_pointer(exception);write_text("}");
}

static int metadata_natural_write_simulation_clock(void) {
    metadata_natural_resolve_game_manager();
    void* klass=metadata_natural_game_manager_class;
    void* object=metadata_natural_game_manager;
    int ok=klass&&object;
    write_text("{\"object\":");metadata_object_pointer(object);
    write_text(",\"class\":");metadata_nullable_string(klass?
        V014_FN(il2cpp_class_get_name,const char*,void*)(klass):0);
    unsigned int ticks[2]={0,0};unsigned int ticks_flags=0;
    unsigned int frame_field[1]={0};unsigned int frame_flags=0;
    unsigned int solving_field[1]={0};unsigned int solving_flags=0;
    int ticks_ok=metadata_natural_write_static_clock_field(klass,"Ticks",ticks,2,&ticks_flags);
    void* frame_meta=metadata_find_field(klass,"<TicksThisFrame>k__BackingField");
    void* solving_meta=metadata_find_field(klass,"<IsSolvingSimulationTick>k__BackingField");
    if(frame_meta) frame_flags=V014_FN(il2cpp_field_get_flags,unsigned int,void*)(frame_meta);
    if(solving_meta) solving_flags=V014_FN(il2cpp_field_get_flags,unsigned int,void*)(solving_meta);
    int frame_static=(frame_flags&16)!=0;
    int solving_static=(solving_flags&16)!=0;
    int frame_ok=frame_static?
        metadata_natural_write_static_clock_field(klass,"<TicksThisFrame>k__BackingField",
                                                  frame_field,1,0):
        metadata_read_words_field(klass,object,"<TicksThisFrame>k__BackingField",
                                   frame_field,1);
    int solving_ok=solving_static?
        metadata_natural_write_static_clock_field(klass,"<IsSolvingSimulationTick>k__BackingField",
                                                  solving_field,1,0):
        metadata_read_words_field(klass,object,"<IsSolvingSimulationTick>k__BackingField",
                                  solving_field,1);
    if(!ticks_ok||!frame_ok||!solving_ok) ok=0;
    write_text(",\"fields\":{\"Ticks\":");
    if(ticks_ok) metadata_words(ticks,2); else write_text("null");
    write_text(",\"<TicksThisFrame>k__BackingField\":");
    if(frame_ok) metadata_words(frame_field,1); else write_text("null");
    write_text(",\"<IsSolvingSimulationTick>k__BackingField\":");
    if(solving_ok) metadata_words(solving_field,1); else write_text("null");
    write_text("},\"field_provenance\":{\"Ticks\":{\"read_method\":\"il2cpp_field_static_get_value\",\"flags\":");
    metadata_number(ticks_flags);write_text(",\"read_ok\":");if(ticks_ok)write_text("true");else write_text("false");
    write_text("},\"<TicksThisFrame>k__BackingField\":{\"read_method\":\"");
    if(frame_static)write_text("il2cpp_field_static_get_value");else write_text("il2cpp_field_get_value");
    write_text("\",\"flags\":");
    metadata_number(frame_flags);write_text(",\"read_ok\":");if(frame_ok)write_text("true");else write_text("false");
    write_text("},\"<IsSolvingSimulationTick>k__BackingField\":{\"read_method\":\"");
    if(solving_static)write_text("il2cpp_field_static_get_value");else write_text("il2cpp_field_get_value");
    write_text("\",\"flags\":");
    metadata_number(solving_flags);write_text(",\"read_ok\":");if(solving_ok)write_text("true");else write_text("false");write_text("}}");
    write_text(",\"getters\":{");
    write_text("\"ticks_this_frame\":");
    if(!metadata_trace_write_getter_words(klass,object,"get_TicksThisFrame",1)) ok=0;
    write_text(",\"is_solving_simulation_tick\":");
    if(!metadata_trace_write_getter_words(klass,object,"get_IsSolvingSimulationTick",1)) ok=0;
    write_text(",\"current_sim_time\":");
    if(!metadata_trace_write_getter_words(klass,object,"get_CurrentSimTime",1)) ok=0;
    write_text("},\"read_ok\":");if(ok)write_text("true");else write_text("false");
    write_text("}");
    return ok;
}
#endif

/*
 * Controlled ball-event fixtures.  These snapshots are intentionally wider
 * than the handful of inputs changed by a case: Simulate can cross a net,
 * bounce, or call back into the manager, so preserving the whole named
 * manager/score/ball state makes each row independently replayable and keeps
 * the live scene unchanged after the batch.  The arrays are raw words; their
 * interpretation is supplied by the field names and metadata map.
 */
#ifdef V014_EVENT_CAPTURE
/* These headers are intentionally included only after the metadata helpers
 * above have been defined.  Both walkers use metadata names and the resolved
 * IL2CPP API table; neither contains native offsets or object-layout guesses. */
#include "getter_items_v014.h"
#include "serve_fixtures_v014.h"
#include "shot_fixtures_v014.h"
#include "curve_fixtures_v014.h"

static void metadata_event_write_serve_field_specs(
    void* klass, void* object, const V014ServeFieldSpec* specs, size_t count) {
    for(size_t i=0;i<count;++i) {
        if(i) write_text(",");
        metadata_quoted(specs[i].key);write_text(":{\"managed_name\":");
        metadata_quoted(specs[i].managed_name);
        write_text(",\"word_count\":");metadata_number((unsigned int)specs[i].words);
        unsigned int words[4]={0,0,0,0};
        int read_ok=metadata_read_words_field(klass,object,specs[i].managed_name,
                                               words,specs[i].words);
        write_text(",\"read_ok\":");if(read_ok)write_text("true");else write_text("false");
        write_text(",\"value\":");if(read_ok)metadata_words(words,specs[i].words);else write_text("null");
        write_text(",\"encoding\":");metadata_number((unsigned int)specs[i].encoding);
        write_text(",\"required\":");if(specs[i].required)write_text("true");else write_text("false");
        write_text("}");
    }
}

static void metadata_event_write_serve_method_specs(void* manager_class,
                                                     void* score_class,
                                                     void* player_class) {
    write_text("[");
    int first=1;
    for(size_t i=0;i<sizeof(v014_serve_methods)/sizeof(v014_serve_methods[0]);++i) {
        const V014ServeMethodSpec* spec=&v014_serve_methods[i];
        void* declaring=0;
        if(same(spec->declaring_class,"TennisGameManager")) declaring=manager_class;
        else if(same(spec->declaring_class,"TennisScore")) declaring=score_class;
        else if(same(spec->declaring_class,"TennisPlayer")) declaring=player_class;
        void* method=declaring?metadata_method_checked(declaring,spec->name,spec->args):0;
        if(!first)write_text(",");first=0;
        write_text("{\"declaring_class\":");metadata_quoted(spec->declaring_class);
        write_text(",\"name\":");metadata_quoted(spec->name);
        write_text(",\"args\":");metadata_number(spec->args);
        write_text(",\"method\":");metadata_trace_write_method_identity(method,declaring);
        write_text("}");
    }
    write_text("]");
}

static void* metadata_event_player_stats(void* player, void* player_class) {
    void* method=metadata_method_checked(player_class,"get_TennisStats",0);
    void* exception=0;
    void* result=metadata_invoke(method,player,0,&exception);
    return (result&&!exception)?result:0;
}

static void metadata_event_write_serve_context(void* manager,void* manager_class,
                                               void* ball,void* ball_class) {
    void* score=metadata_field_object(manager_class,manager,"<Score>k__BackingField");
    void* score_class=score?V014_FN(il2cpp_object_get_class,void*,void*)(score):0;
    void* home=metadata_field_object(manager_class,manager,"<HomePlayer>k__BackingField");
    void* away=metadata_field_object(manager_class,manager,"<AwayPlayer>k__BackingField");
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    void* home_stats=metadata_event_player_stats(home,home_class);
    void* away_stats=metadata_event_player_stats(away,away_class);
    void* stats_class=home_stats?V014_FN(il2cpp_object_get_class,void*,void*)(home_stats):
                       (away_stats?V014_FN(il2cpp_object_get_class,void*,void*)(away_stats):0);
    write_text("{\"kind\":\"serve_fixture_context\",\"schema\":\"v014-serve-v1\",\"read_only\":true");
    write_text(",\"manager_object\":");metadata_object_pointer(manager);
    write_text(",\"manager_fields\":{");
    metadata_event_write_serve_field_specs(manager_class,manager,v014_serve_manager_fields,
        sizeof(v014_serve_manager_fields)/sizeof(v014_serve_manager_fields[0]));
    write_text("},\"score_object\":");metadata_object_pointer(score);
    write_text(",\"score_fields\":{");
    metadata_event_write_serve_field_specs(score_class,score,v014_serve_score_fields,
        sizeof(v014_serve_score_fields)/sizeof(v014_serve_score_fields[0]));
    write_text("},\"home_stats_object\":");metadata_object_pointer(home_stats);
    write_text(",\"home_stats_fields\":{");
    metadata_event_write_serve_field_specs(stats_class,home_stats,v014_serve_stats_fields,
        sizeof(v014_serve_stats_fields)/sizeof(v014_serve_stats_fields[0]));
    write_text("},\"away_stats_object\":");metadata_object_pointer(away_stats);
    write_text(",\"away_stats_fields\":{");
    metadata_event_write_serve_field_specs(stats_class,away_stats,v014_serve_stats_fields,
        sizeof(v014_serve_stats_fields)/sizeof(v014_serve_stats_fields[0]));
    write_text("},\"ball_object\":");metadata_object_pointer(ball);
    write_text(",\"methods\":");metadata_event_write_serve_method_specs(manager_class,score_class,home_class);
    V014ServeFixtureOps ops={metadata_read_words_field,metadata_set_words_field,
                             metadata_method_checked,metadata_invoke_words,metadata_invoke_void};
    unsigned int timing[3]={0,0,0};void* exception=0;
    int timing_ok=v014_serve_fixture_get_timing_windows(&ops,manager_class,manager,timing,&exception);
    write_text(",\"timing_windows\":{\"invoke_ok\":");if(timing_ok)write_text("true");else write_text("false");
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"words\":");if(timing_ok)metadata_words(timing,3);else write_text("null");write_text("}");
    unsigned int forgiveness[1]={0};exception=0;
    int forgiveness_ok=v014_serve_fixture_get_forgiveness_active(&ops,manager_class,manager,forgiveness,&exception);
    write_text(",\"timing_forgiveness_active\":{\"invoke_ok\":");if(forgiveness_ok)write_text("true");else write_text("false");
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"words\":");if(forgiveness_ok)metadata_words(forgiveness,1);else write_text("null");write_text("}");
    unsigned int deciding[1]={0};exception=0;
    int deciding_ok=v014_serve_fixture_score_is_deciding_set(&ops,score_class,score,deciding,&exception);
    write_text(",\"score_is_deciding_set\":{\"invoke_ok\":");if(deciding_ok)write_text("true");else write_text("false");
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"words\":");if(deciding_ok)metadata_words(deciding,1);else write_text("null");write_text("}");
    write_text(",\"player_thresholds\":{\"home\":");
    unsigned int thresholds[3]={0,0,0};exception=0;
    int home_thresholds=v014_serve_fixture_get_player_thresholds(&ops,home_class,home,thresholds,&exception);
    if(home_thresholds)metadata_words(thresholds,3);else write_text("null");
    write_text(",\"home_invoke_ok\":");if(home_thresholds)write_text("true");else write_text("false");
    write_text(",\"home_exception\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"away\":");memset(thresholds,0,sizeof(thresholds));exception=0;
    int away_thresholds=v014_serve_fixture_get_player_thresholds(&ops,away_class,away,thresholds,&exception);
    if(away_thresholds)metadata_words(thresholds,3);else write_text("null");
    write_text(",\"away_invoke_ok\":");if(away_thresholds)write_text("true");else write_text("false");
    write_text(",\"away_exception\":");if(exception)write_text("true");else write_text("false");write_text("}");
    write_text("}\n");flush_log();
}

typedef struct {
    void* score;
    void* ball;
    void* score_from;
    void* score_to;
    void* ball_last_hitter;
    void* ball_hold_parent;
    void* ball_tennis_manager;
    void* home_stats;
    void* away_stats;
    unsigned int manager_values[40][4];
    unsigned int score_values[16][4];
    unsigned int ball_values[40][4];
    unsigned int home_stats_values[V014_SERVE_STATS_FIELD_CAPACITY][4];
    unsigned int away_stats_values[V014_SERVE_STATS_FIELD_CAPACITY][4];
    int stats_valid;
    int valid;
} metadata_event_snapshot;

typedef struct {
    const char* case_id;
    unsigned int position[3];
    unsigned int velocity[3];
    unsigned int curve;
    unsigned int dt;
    int shot_type;
    unsigned int curve_team;
    unsigned int in_play;
    unsigned int is_serve;
    unsigned int crossed_net;
    unsigned int bounced_in;
    unsigned int bounce_count;
    const char* expected_branch;
    unsigned int current_game_state;
    unsigned int current_half;
    unsigned int last_strike_near;
    unsigned int peak_near;
    unsigned int last_hitter_set;
    unsigned int last_hitter_slot;
} metadata_simulate_case;

typedef struct {
    const char* case_id;
    unsigned int pos[3];
    unsigned int vel[3];
    unsigned int curve;
    int shot_type;
    unsigned int is_serve;
    unsigned int crossed_net;
    int bounces_needed;
    unsigned int topspin_kick;
    unsigned int slice_bounce;
    unsigned int drop_bounce;
    unsigned int stop_on_tape;
    unsigned int curve_team;
} metadata_nth_landing_case;

static int metadata_event_capture_group(void* klass,void* object,
                                        const metadata_trace_field_spec* specs,
                                        unsigned int count,
                                        unsigned int values[][4]) {
    int ok=1;
    for(unsigned int i=0;i<count;++i) {
        for(unsigned int j=0;j<4;++j) values[i][j]=0;
        /* A field the running build removed (e.g. v0.15's ball bounce-time
         * caches) must not fail the whole snapshot -- see
         * metadata_trace_field_optional. */
        if(!metadata_read_words_field(klass,object,specs[i].name,
                                      values[i],specs[i].words) &&
           !metadata_trace_field_optional(specs[i].name)) ok=0;
    }
    return ok;
}

static int metadata_event_capture_serve_group(void* klass,void* object,
                                              const V014ServeFieldSpec* specs,
                                              size_t count,
                                              unsigned int values[][4]) {
    int ok=1;
    for(size_t i=0;i<count;++i) {
        for(unsigned int j=0;j<4;++j) values[i][j]=0;
        if(!metadata_read_words_field(klass,object,specs[i].managed_name,
                                      values[i],specs[i].words) &&
           specs[i].required) ok=0;
    }
    return ok;
}

static int metadata_event_restore_serve_group(void* klass,void* object,
                                              const V014ServeFieldSpec* specs,
                                              size_t count,
                                              const unsigned int values[][4]) {
    int ok=1;
    for(size_t i=0;i<count;++i)
        if(!metadata_set_words_field(klass,object,specs[i].managed_name,
                                     values[i],specs[i].words) &&
           specs[i].required) ok=0;
    return ok;
}

static int metadata_event_serve_group_matches(void* klass,void* object,
                                              const V014ServeFieldSpec* specs,
                                              size_t count,
                                              const unsigned int values[][4]) {
    int ok=1;unsigned int current[4]={0,0,0,0};
    for(size_t i=0;i<count;++i) {
        if(!metadata_read_words_field(klass,object,specs[i].managed_name,current,
                                      specs[i].words)) {
            if(specs[i].required) ok=0;
            continue;
        }
        for(unsigned int j=0;j<specs[i].words;++j)
            if(current[j]!=values[i][j]) ok=0;
    }
    return ok;
}

static int metadata_event_restore_group(void* klass,void* object,
                                        const metadata_trace_field_spec* specs,
                                        unsigned int count,
                                        const unsigned int values[][4]) {
    int ok=1;
    for(unsigned int i=0;i<count;++i)
        if(!metadata_set_words_field(klass,object,specs[i].name,values[i],specs[i].words) &&
           !metadata_trace_field_optional(specs[i].name)) ok=0;
    return ok;
}

static int metadata_event_group_matches(void* klass,void* object,
                                        const metadata_trace_field_spec* specs,
                                        unsigned int count,
                                        const unsigned int values[][4]) {
    int ok=1;unsigned int current[4]={0,0,0,0};
    for(unsigned int i=0;i<count;++i) {
        if(!metadata_read_words_field(klass,object,specs[i].name,current,specs[i].words)) {
            if(!metadata_trace_field_optional(specs[i].name)) ok=0;
            continue;
        }
        for(unsigned int j=0;j<specs[i].words;++j)
            if(current[j]!=values[i][j]) ok=0;
    }
    return ok;
}

static int metadata_event_take_snapshot(void* manager,void* manager_class,
                                        metadata_event_snapshot* snapshot) {
    int ok=1;
    snapshot->score=metadata_field_object(manager_class,manager,"<Score>k__BackingField");
    snapshot->ball=metadata_field_object(manager_class,manager,"<Ball>k__BackingField");
    if(!snapshot->score||!snapshot->ball) ok=0;
    void* score_class=snapshot->score?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->score):0;
    void* ball_class=snapshot->ball?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->ball):0;
    void* home=metadata_field_object(manager_class,manager,"<HomePlayer>k__BackingField");
    void* away=metadata_field_object(manager_class,manager,"<AwayPlayer>k__BackingField");
    void* home_class=home?V014_FN(il2cpp_object_get_class,void*,void*)(home):0;
    void* away_class=away?V014_FN(il2cpp_object_get_class,void*,void*)(away):0;
    snapshot->home_stats=metadata_event_player_stats(home,home_class);
    snapshot->away_stats=metadata_event_player_stats(away,away_class);
    void* home_stats_class=snapshot->home_stats?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->home_stats):0;
    void* away_stats_class=snapshot->away_stats?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->away_stats):0;
    if(!metadata_event_capture_group(manager_class,manager,
            metadata_trace_manager_fields,
            sizeof(metadata_trace_manager_fields)/sizeof(metadata_trace_manager_fields[0]),
            snapshot->manager_values)) ok=0;
    if(!metadata_event_capture_group(score_class,snapshot->score,
            metadata_trace_score_fields,
            sizeof(metadata_trace_score_fields)/sizeof(metadata_trace_score_fields[0]),
            snapshot->score_values)) ok=0;
    if(!metadata_event_capture_group(ball_class,snapshot->ball,
            metadata_trace_ball_fields,
            sizeof(metadata_trace_ball_fields)/sizeof(metadata_trace_ball_fields[0]),
            snapshot->ball_values)) ok=0;
    snapshot->stats_valid=snapshot->home_stats&&snapshot->away_stats&&
        metadata_event_capture_serve_group(home_stats_class,snapshot->home_stats,
            v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
            snapshot->home_stats_values)&&
        metadata_event_capture_serve_group(away_stats_class,snapshot->away_stats,
            v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
            snapshot->away_stats_values);
    if(!snapshot->stats_valid) ok=0;
    snapshot->score_from=metadata_field_object(score_class,snapshot->score,
                                               "<LastPointFromDisplay>k__BackingField");
    snapshot->score_to=metadata_field_object(score_class,snapshot->score,
                                             "<LastPointToDisplay>k__BackingField");
    snapshot->ball_last_hitter=metadata_field_object(ball_class,snapshot->ball,
                                                     "<LastHitter>k__BackingField");
    snapshot->ball_hold_parent=metadata_field_object(ball_class,snapshot->ball,"holdParent");
    snapshot->ball_tennis_manager=metadata_field_object(ball_class,snapshot->ball,"tennisManager");
    if(!metadata_find_field(score_class,"<LastPointFromDisplay>k__BackingField")||
       !metadata_find_field(score_class,"<LastPointToDisplay>k__BackingField")||
       !metadata_find_field(ball_class,"<LastHitter>k__BackingField")||
       !metadata_find_field(ball_class,"holdParent")||
       !metadata_find_field(ball_class,"tennisManager")) ok=0;
    snapshot->valid=ok;
    return ok;
}

static int metadata_event_restore_snapshot(void* manager,void* manager_class,
                                           const metadata_event_snapshot* snapshot) {
    if(!snapshot||!snapshot->valid) return 0;
    int ok=1;
    void* score=snapshot->score;void* ball=snapshot->ball;
    void* score_class=score?V014_FN(il2cpp_object_get_class,void*,void*)(score):0;
    void* ball_class=ball?V014_FN(il2cpp_object_get_class,void*,void*)(ball):0;
    void* home_stats_class=snapshot->home_stats?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->home_stats):0;
    void* away_stats_class=snapshot->away_stats?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->away_stats):0;
    if(!metadata_event_restore_group(manager_class,manager,
            metadata_trace_manager_fields,
            sizeof(metadata_trace_manager_fields)/sizeof(metadata_trace_manager_fields[0]),
            snapshot->manager_values)) ok=0;
    if(!metadata_event_restore_group(score_class,score,metadata_trace_score_fields,
            sizeof(metadata_trace_score_fields)/sizeof(metadata_trace_score_fields[0]),
            snapshot->score_values)) ok=0;
    if(!metadata_event_restore_group(ball_class,ball,metadata_trace_ball_fields,
            sizeof(metadata_trace_ball_fields)/sizeof(metadata_trace_ball_fields[0]),
            snapshot->ball_values)) ok=0;
    if(!snapshot->stats_valid||
       !metadata_event_restore_serve_group(home_stats_class,snapshot->home_stats,
          v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
          snapshot->home_stats_values)) ok=0;
    if(!metadata_event_restore_serve_group(away_stats_class,snapshot->away_stats,
          v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
          snapshot->away_stats_values)) ok=0;
    if(!metadata_set_object_field(score_class,score,"<LastPointFromDisplay>",snapshot->score_from)) {
        /* Auto-property backing names are preferred; this fallback is for
         * runtimes whose metadata exposes the short field name only. */
        if(!metadata_set_object_field(score_class,score,
                "<LastPointFromDisplay>k__BackingField",snapshot->score_from)) ok=0;
    }
    if(!metadata_set_object_field(score_class,score,"<LastPointToDisplay>",snapshot->score_to)) {
        if(!metadata_set_object_field(score_class,score,
                "<LastPointToDisplay>k__BackingField",snapshot->score_to)) ok=0;
    }
    if(!metadata_set_object_field(ball_class,ball,"<LastHitter>",snapshot->ball_last_hitter)) {
        if(!metadata_set_object_field(ball_class,ball,
                "<LastHitter>k__BackingField",snapshot->ball_last_hitter)) ok=0;
    }
    if(!metadata_set_object_field(ball_class,ball,"holdParent",snapshot->ball_hold_parent)) ok=0;
    if(!metadata_set_object_field(ball_class,ball,"tennisManager",snapshot->ball_tennis_manager)) ok=0;
    return ok;
}

static int metadata_event_snapshot_matches(void* manager,void* manager_class,
                                           const metadata_event_snapshot* snapshot) {
    if(!snapshot||!snapshot->valid) return 0;
    int ok=1;void* score=snapshot->score;void* ball=snapshot->ball;
    void* score_class=score?V014_FN(il2cpp_object_get_class,void*,void*)(score):0;
    void* ball_class=ball?V014_FN(il2cpp_object_get_class,void*,void*)(ball):0;
    void* home_stats_class=snapshot->home_stats?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->home_stats):0;
    void* away_stats_class=snapshot->away_stats?
        V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->away_stats):0;
    if(!metadata_event_group_matches(manager_class,manager,
            metadata_trace_manager_fields,
            sizeof(metadata_trace_manager_fields)/sizeof(metadata_trace_manager_fields[0]),
            snapshot->manager_values)) ok=0;
    if(!metadata_event_group_matches(score_class,score,metadata_trace_score_fields,
            sizeof(metadata_trace_score_fields)/sizeof(metadata_trace_score_fields[0]),
            snapshot->score_values)) ok=0;
    if(!metadata_event_group_matches(ball_class,ball,metadata_trace_ball_fields,
            sizeof(metadata_trace_ball_fields)/sizeof(metadata_trace_ball_fields[0]),
            snapshot->ball_values)) ok=0;
    if(!snapshot->stats_valid||
       !metadata_event_serve_group_matches(home_stats_class,snapshot->home_stats,
          v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
          snapshot->home_stats_values)) ok=0;
    if(!metadata_event_serve_group_matches(away_stats_class,snapshot->away_stats,
          v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
          snapshot->away_stats_values)) ok=0;
    if(metadata_field_object(score_class,score,"<LastPointFromDisplay>k__BackingField")!=snapshot->score_from) ok=0;
    if(metadata_field_object(score_class,score,"<LastPointToDisplay>k__BackingField")!=snapshot->score_to) ok=0;
    if(metadata_field_object(ball_class,ball,"<LastHitter>k__BackingField")!=snapshot->ball_last_hitter) ok=0;
    if(metadata_field_object(ball_class,ball,"holdParent")!=snapshot->ball_hold_parent) ok=0;
    if(metadata_field_object(ball_class,ball,"tennisManager")!=snapshot->ball_tennis_manager) ok=0;
    return ok;
}

static void metadata_event_write_restore(void* manager,void* manager_class,
                                         const char* component,const char* case_id,
                                         int restore_ok,
                                         const metadata_event_snapshot* snapshot) {
    int matches=metadata_event_snapshot_matches(manager,manager_class,snapshot);
    int stats_matches=0;
    if(snapshot&&snapshot->stats_valid) {
        void* home_stats_class=snapshot->home_stats?
            V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->home_stats):0;
        void* away_stats_class=snapshot->away_stats?
            V014_FN(il2cpp_object_get_class,void*,void*)(snapshot->away_stats):0;
        stats_matches=home_stats_class&&away_stats_class&&
            metadata_event_serve_group_matches(home_stats_class,snapshot->home_stats,
                v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
                snapshot->home_stats_values)&&
            metadata_event_serve_group_matches(away_stats_class,snapshot->away_stats,
                v014_serve_stats_fields,V014_SERVE_STATS_FIELD_CAPACITY,
                snapshot->away_stats_values);
    }
    write_text("{\"kind\":\"event_restore\",\"schema\":\"v014-event-v1\",\"component\":");
    metadata_quoted(component);write_text(",\"case_id\":");metadata_quoted(case_id);
    write_text(",\"restore_api_ok\":");if(restore_ok)write_text("true");else write_text("false");
    write_text(",\"matches_snapshot\":");if(matches)write_text("true");else write_text("false");
    write_text(",\"restoration_scope\":\"manager_score_ball_and_home_away_stats\"");
    write_text(",\"stats_snapshot_valid\":");
    if(snapshot&&snapshot->stats_valid)write_text("true");else write_text("false");
    write_text(",\"stats_matches_snapshot\":");if(stats_matches)write_text("true");else write_text("false");
    write_text(",\"stats_state_restored\":");if(restore_ok&&stats_matches)write_text("true");else write_text("false");
    write_text(",\"state_restored\":");if(restore_ok&&matches)write_text("true");else write_text("false");
    if(!matches) {
        /* Keep a compact field-level reason with each failed restoration so a
         * capture cannot be mistaken for a clean restore merely because the
         * row itself parsed. */
        write_text(",\"mismatch_fields\":[");int first=1;unsigned int current[4]={0,0,0,0};
        void* score=snapshot?snapshot->score:0;void* ball=snapshot?snapshot->ball:0;
        void* score_class=score?V014_FN(il2cpp_object_get_class,void*,void*)(score):0;
        void* ball_class=ball?V014_FN(il2cpp_object_get_class,void*,void*)(ball):0;
        const metadata_trace_field_spec* groups[]={metadata_trace_manager_fields,
            metadata_trace_score_fields,metadata_trace_ball_fields};
        const unsigned int counts[]={sizeof(metadata_trace_manager_fields)/sizeof(metadata_trace_manager_fields[0]),
            sizeof(metadata_trace_score_fields)/sizeof(metadata_trace_score_fields[0]),
            sizeof(metadata_trace_ball_fields)/sizeof(metadata_trace_ball_fields[0])};
        void* classes[]={manager_class,score_class,ball_class};
        void* objects[]={manager,score,ball};
        const unsigned int (*saved[])[4]={snapshot->manager_values,snapshot->score_values,
            snapshot->ball_values};
        for(unsigned int g=0;g<3;++g) for(unsigned int i=0;i<counts[g];++i) {
            int equal=metadata_read_words_field(classes[g],objects[g],groups[g][i].name,
                                                current,groups[g][i].words);
            if(!equal) {
                if(!first)write_text(",");first=0;metadata_quoted(groups[g][i].name);
            } else for(unsigned int j=0;j<groups[g][i].words;++j)
                if(current[j]!=saved[g][i][j]) {
                    if(!first)write_text(",");first=0;metadata_quoted(groups[g][i].name);break;
                }
        }
        if(metadata_field_object(score_class,score,"<LastPointFromDisplay>k__BackingField")!=snapshot->score_from) {
            if(!first)write_text(",");first=0;metadata_quoted("<LastPointFromDisplay>k__BackingField");
        }
        if(metadata_field_object(score_class,score,"<LastPointToDisplay>k__BackingField")!=snapshot->score_to) {
            if(!first)write_text(",");first=0;metadata_quoted("<LastPointToDisplay>k__BackingField");
        }
        if(metadata_field_object(ball_class,ball,"<LastHitter>k__BackingField")!=snapshot->ball_last_hitter) {
            if(!first)write_text(",");first=0;metadata_quoted("<LastHitter>k__BackingField");
        }
        if(metadata_field_object(ball_class,ball,"holdParent")!=snapshot->ball_hold_parent) {
            if(!first)write_text(",");first=0;metadata_quoted("holdParent");
        }
        if(metadata_field_object(ball_class,ball,"tennisManager")!=snapshot->ball_tennis_manager) {
            if(!first)write_text(",");first=0;metadata_quoted("tennisManager");
        }
        write_text("]");
    }
    write_text("}\n");
    ++metadata_event_rows;
}

static int metadata_event_set_sim_state(void* manager,void* manager_class,
                                        void* ball,void* ball_class,
                                        const metadata_simulate_case* fixture) {
    int ok=1;
    unsigned int zero[3]={0,0,0};
    if(!metadata_set_words_field(ball_class,ball,"<Position>k__BackingField",
                                 fixture->position,3)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"<Velocity>k__BackingField",
                                 fixture->velocity,3)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"lastPosition",fixture->position,3)) ok=0;
    /* IsScrapePredicted treats a near strike point and a peak rise below the
     * native threshold as a scrape.  Start controlled event calls with both
     * native disqualifiers (a lateral strike distance > 1 and an adequate
     * prior peak), so a descending floor case reaches HandleBounce.  The raw
     * values remain visible in state_before for replay. */
    metadata_float_bits strike_x;strike_x.words=fixture->position[0];
    strike_x.value-=fixture->last_strike_near?0.0f:2.0f;
    metadata_float_bits peak_y;peak_y.words=fixture->position[1];
    peak_y.value+=fixture->peak_near?0.0f:0.5f;
    unsigned int strike_position[3]={strike_x.words,fixture->position[1],fixture->position[2]};
    if(!metadata_set_words_field(ball_class,ball,"lastStrikePos",strike_position,3)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"peakYSinceHit",&peak_y.words,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"<LastBouncePosition>k__BackingField",
                                 zero,3)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"sliceCurve",&fixture->curve,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"curveTeam",&fixture->curve_team,1)) ok=0;
    unsigned int shot=(unsigned int)fixture->shot_type;
    if(!metadata_set_words_field(ball_class,ball,"<LastShotType>k__BackingField",&shot,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"<InPlay>k__BackingField",
                                 &fixture->in_play,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"<IsServe>k__BackingField",
                                 &fixture->is_serve,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"<HasCrossedNet>k__BackingField",
                                 &fixture->crossed_net,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"<HasBouncedInSinceHit>k__BackingField",
                                 &fixture->bounced_in,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"<BounceCountOnCurrentHalf>k__BackingField",
                                 &fixture->bounce_count,1)) ok=0;
    unsigned int half=fixture->current_half;
    if(!metadata_set_words_field(ball_class,ball,"<CurrentHalf>k__BackingField",&half,1)) ok=0;
    unsigned int false_word=0;
    if(!metadata_set_words_field(ball_class,ball,"pendingTopspinKick",&false_word,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"pendingSliceBounce",&false_word,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"pendingDropBounce",&false_word,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"hasCachedPredictedBounce",&false_word,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"hasCachedPredictedSecondBounce",&false_word,1)) ok=0;
    if(!metadata_set_words_field(ball_class,ball,"shotBounceCount",&false_word,1)) ok=0;
    if(!metadata_set_words_field(manager_class,manager,"pointResolved",&false_word,1)) ok=0;
    unsigned int state=fixture->current_game_state?fixture->current_game_state:3;
    if(!metadata_set_words_field(manager_class,manager,
                                 "<CurrentGameState>k__BackingField",&state,1)) ok=0;
    if(fixture->last_hitter_set) {
        void* player_field=V014_FN(il2cpp_class_get_field_from_name,void*,void*,const char*)
            (manager_class,fixture->last_hitter_slot?"<AwayPlayer>k__BackingField":"<HomePlayer>k__BackingField");
        void* player=0;
        if(player_field) V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
            (manager,player_field,&player);
        if(!player||!metadata_set_object_field(ball_class,ball,"<LastHitter>k__BackingField",player)) ok=0;
    }
    return ok;
}

static void metadata_event_write_branch_observation(void* manager,void* manager_class,
                                                    void* ball,void* ball_class,
                                                    const metadata_simulate_case* fixture) {
    unsigned int words[4]={0,0,0,0};
    write_text(",\"branch_observation\":{\"expected_branch\":");
    if(fixture->expected_branch)metadata_quoted(fixture->expected_branch);else write_text("null");
    write_text(",\"bounce_count_current_half\":");
    if(metadata_read_words_field(ball_class,ball,"<BounceCountOnCurrentHalf>k__BackingField",words,1))metadata_words(words,1);else write_text("null");
    write_text(",\"shot_bounce_count\":");
    if(metadata_read_words_field(ball_class,ball,"shotBounceCount",words,1))metadata_words(words,1);else write_text("null");
    write_text(",\"has_crossed_net\":");
    if(metadata_read_words_field(ball_class,ball,"<HasCrossedNet>k__BackingField",words,1))metadata_words(words,1);else write_text("null");
    write_text(",\"has_bounced_in_since_hit\":");
    if(metadata_read_words_field(ball_class,ball,"<HasBouncedInSinceHit>k__BackingField",words,1))metadata_words(words,1);else write_text("null");
    write_text(",\"in_play\":");
    if(metadata_read_words_field(ball_class,ball,"<InPlay>k__BackingField",words,1))metadata_words(words,1);else write_text("null");
    write_text(",\"point_resolved\":");
    if(metadata_read_words_field(manager_class,manager,"pointResolved",words,1))metadata_words(words,1);else write_text("null");
    write_text(",\"last_hitter_object\":");metadata_object_pointer(
        metadata_field_object(ball_class,ball,"<LastHitter>k__BackingField"));
    write_text(",\"input_last_strike_near\":");if(fixture->last_strike_near)write_text("true");else write_text("false");
    write_text(",\"input_peak_near\":");if(fixture->peak_near)write_text("true");else write_text("false");
    write_text("}");
}

static int metadata_event_read_sim_inputs(void* ball,void* ball_class,
                                          const metadata_simulate_case* fixture) {
    unsigned int words[4]={0,0,0,0};int ok=1;
    if(!metadata_read_words_field(ball_class,ball,"<Position>k__BackingField",words,3)) ok=0;
    else for(unsigned int i=0;i<3;++i)if(words[i]!=fixture->position[i])ok=0;
    if(!metadata_read_words_field(ball_class,ball,"<Velocity>k__BackingField",words,3)) ok=0;
    else for(unsigned int i=0;i<3;++i)if(words[i]!=fixture->velocity[i])ok=0;
    if(!metadata_read_words_field(ball_class,ball,"sliceCurve",words,1)||words[0]!=fixture->curve)ok=0;
    if(!metadata_read_words_field(ball_class,ball,"curveTeam",words,1)||words[0]!=fixture->curve_team)ok=0;
    if(!metadata_read_words_field(ball_class,ball,"<InPlay>k__BackingField",words,1)||words[0]!=fixture->in_play)ok=0;
    if(!metadata_read_words_field(ball_class,ball,"<IsServe>k__BackingField",words,1)||words[0]!=fixture->is_serve)ok=0;
    return ok;
}

static int metadata_event_write_simulate_case(void* manager,void* manager_class,
                                              void* ball,void* ball_class,
                                              void* method,
                                              const metadata_event_snapshot* snapshot,
                                              const metadata_simulate_case* fixture,
                                              unsigned int index) {
    if(!metadata_event_restore_snapshot(manager,manager_class,snapshot)) return 0;
    int applied=metadata_event_set_sim_state(manager,manager_class,ball,ball_class,fixture);
    int readback=metadata_event_read_sim_inputs(ball,ball_class,fixture);
    metadata_float_bits dt_bits;dt_bits.words=fixture->dt;float dt=dt_bits.value;
    void* args[]={&dt};void* exception=0;int invoke_ok=0;int state_before_ok=0;
    /* Serialize the pre-call state before invoking the managed method.  This
     * ordering is part of the fixture contract; Simulate may mutate Position,
     * Velocity, bounce flags, and manager callbacks synchronously. */
    write_text("{\"kind\":\"simulate_fixture\",\"schema\":\"v014-event-v1\",\"case_id\":");
    metadata_quoted(fixture->case_id);write_text(",\"case_index\":");metadata_number(index);
    write_text(",\"method\":");metadata_trace_write_method_identity(method,ball_class);
    write_text(",\"input\":{\"position_f32_words\":");metadata_words(fixture->position,3);
    write_text(",\"velocity_f32_words\":");metadata_words(fixture->velocity,3);
    write_text(",\"curve_f32_words\":");metadata_words(&fixture->curve,1);
    write_text(",\"dt_f32_words\":");metadata_words(&fixture->dt,1);
    write_text(",\"shot_type\":");metadata_write_signed_decimal(fixture->shot_type);
    write_text(",\"curve_team\":");metadata_words(&fixture->curve_team,1);
    write_text(",\"in_play\":");metadata_words(&fixture->in_play,1);
    write_text(",\"is_serve\":");metadata_words(&fixture->is_serve,1);
    write_text(",\"crossed_net\":");metadata_words(&fixture->crossed_net,1);
    write_text(",\"bounced_in\":");metadata_words(&fixture->bounced_in,1);
    write_text(",\"bounce_count\":");metadata_words(&fixture->bounce_count,1);
    write_text(",\"expected_branch\":");if(fixture->expected_branch)metadata_quoted(fixture->expected_branch);else write_text("null");
    write_text(",\"current_game_state\":");metadata_words(&fixture->current_game_state,1);
    write_text(",\"current_half\":");metadata_words(&fixture->current_half,1);
    write_text(",\"last_hitter_set\":");metadata_words(&fixture->last_hitter_set,1);
    write_text(",\"last_hitter_slot\":");metadata_words(&fixture->last_hitter_slot,1);
    write_text(",\"last_strike_near\":");metadata_words(&fixture->last_strike_near,1);
    write_text(",\"peak_near\":");metadata_words(&fixture->peak_near,1);write_text("}");
    write_text(",\"input_readback_ok\":");if(readback)write_text("true");else write_text("false");
    write_text(",\"state_before\":");
    if(applied&&readback)state_before_ok=metadata_trace_write_state(manager,manager_class);
    else write_text("null");
    /* Keep the line open while the call runs.  The probe's log writer is
     * single-threaded on the Unity main thread, so the completed newline is
     * still an atomic record boundary for the launcher. */
    if(applied&&readback) invoke_ok=metadata_invoke_void(method,ball,args,&exception);
    write_text(",\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
    write_text(",\"exceptions\":{\"simulate\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"object\":");metadata_object_pointer(exception);write_text("}");
    write_text(",\"state_after\":");
    if(applied&&readback)metadata_trace_write_state(manager,manager_class);else write_text("null");
    if(applied&&readback)metadata_event_write_branch_observation(manager,manager_class,ball,ball_class,fixture);
    write_text("}\n");
    ++metadata_event_rows;++metadata_simulate_rows;
    int restored=metadata_event_restore_snapshot(manager,manager_class,snapshot);
    metadata_event_write_restore(manager,manager_class,"Simulate",fixture->case_id,restored,snapshot);
    return applied&&readback&&state_before_ok&&invoke_ok&&!exception&&restored&&
        metadata_event_snapshot_matches(manager,manager_class,snapshot);
}

static int metadata_event_write_nth_case(void* manager,void* manager_class,
                                         void* ball,void* ball_class,
                                         void* method,
                                         const metadata_event_snapshot* snapshot,
                                         const metadata_nth_landing_case* fixture,
                                         unsigned int index) {
    if(!metadata_event_restore_snapshot(manager,manager_class,snapshot)) return 0;
    /* Nth landing is a pure prediction call, but its curve-team and pace
     * branches read the live ball configuration.  Keep both requested values
     * explicit and verify them through the field API before invoking. */
    unsigned int requested_pace=0x3f800000;
    int applied=1;
    if(!metadata_set_words_field(ball_class,ball,"flightPace",&requested_pace,1))applied=0;
    if(!metadata_set_words_field(ball_class,ball,"curveTeam",&fixture->curve_team,1))applied=0;
    unsigned int pace_readback=0,team_readback=0;
    if(!metadata_read_words_field(ball_class,ball,"flightPace",&pace_readback,1))applied=0;
    if(!metadata_read_words_field(ball_class,ball,"curveTeam",&team_readback,1))applied=0;
    metadata_float_bits pos_bits[3];metadata_float_bits vel_bits[3];
    float pos[3];float vel[3];
    for(unsigned int i=0;i<3;++i) {pos_bits[i].words=fixture->pos[i];pos[i]=pos_bits[i].value;
        vel_bits[i].words=fixture->vel[i];vel[i]=vel_bits[i].value;}
    metadata_float_bits curve_bits;curve_bits.words=fixture->curve;float curve=curve_bits.value;
    int shot=fixture->shot_type;BYTE is_serve=(BYTE)fixture->is_serve;
    BYTE crossed=(BYTE)fixture->crossed_net;int bounces=fixture->bounces_needed;
    BYTE topspin=(BYTE)fixture->topspin_kick;BYTE slice=(BYTE)fixture->slice_bounce;
    BYTE drop=(BYTE)fixture->drop_bounce;BYTE stop=(BYTE)fixture->stop_on_tape;
    float landing[3]={0x1.fffffep+127f,0x1.fffffep+127f,0x1.fffffep+127f};
    float seconds=-1.0f;
    void* args[]={pos,vel,&curve,&shot,&is_serve,&crossed,&bounces,
                  &topspin,&slice,&drop,&stop,landing,&seconds};
    void* exception=0;void* result=0;unsigned int return_word=0;int invoke_ok=0;
    write_text("{\"kind\":\"nth_landing_fixture\",\"schema\":\"v014-event-v1\",\"case_id\":");
    metadata_quoted(fixture->case_id);write_text(",\"case_index\":");metadata_number(index);
    write_text(",\"method\":");metadata_trace_write_method_identity(method,ball_class);
    write_text(",\"input\":{\"pos_f32_words\":");metadata_words(fixture->pos,3);
    write_text(",\"vel_f32_words\":");metadata_words(fixture->vel,3);
    write_text(",\"curve_f32_words\":");metadata_words(&fixture->curve,1);
    write_text(",\"shot_type\":");metadata_write_signed_decimal(fixture->shot_type);
    write_text(",\"is_serve\":");metadata_words(&fixture->is_serve,1);
    write_text(",\"crossed_net\":");metadata_words(&fixture->crossed_net,1);
    write_text(",\"bounces_needed\":");metadata_write_signed_decimal(fixture->bounces_needed);
    write_text(",\"topspin_kick\":");metadata_words(&fixture->topspin_kick,1);
    write_text(",\"slice_bounce\":");metadata_words(&fixture->slice_bounce,1);
    write_text(",\"drop_bounce\":");metadata_words(&fixture->drop_bounce,1);
    write_text(",\"stop_on_tape\":");metadata_words(&fixture->stop_on_tape,1);
    write_text(",\"curve_team\":");metadata_words(&fixture->curve_team,1);
    write_text(",\"flight_pace_requested_f32_words\":");metadata_words(&requested_pace,1);
    write_text(",\"flight_pace_readback_f32_words\":");metadata_words(&pace_readback,1);
    write_text(",\"curve_team_readback\":");metadata_words(&team_readback,1);write_text("}");
    write_text(",\"state_before\":");
    int state_before_ok=0;
    if(applied) state_before_ok=metadata_trace_write_state(manager,manager_class);
    else write_text("null");
    if(applied&&pace_readback==requested_pace&&team_readback==fixture->curve_team) {
        result=metadata_invoke(method,ball,args,&exception);
        invoke_ok=result&&!exception;
        if(invoke_ok) {
            void* unboxed=V014_FN(il2cpp_object_unbox,void*,void*)(result);
            if(unboxed) {return_word=(unsigned int)*(const BYTE*)unboxed;}
            else invoke_ok=0;
        }
    }
    write_text(",\"return_bool_word\":");metadata_words(&return_word,1);
    write_text(",\"landing_f32_words\":");metadata_words(landing,3);
    write_text(",\"seconds_f32_words\":");metadata_words(&seconds,1);
    write_text(",\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
    write_text(",\"exceptions\":{\"prediction\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"object\":");metadata_object_pointer(exception);write_text("}");
    write_text(",\"state_after\":");if(applied)metadata_trace_write_state(manager,manager_class);else write_text("null");
    write_text("}\n");
    ++metadata_event_rows;++metadata_nth_landing_rows;
    int restored=metadata_event_restore_snapshot(manager,manager_class,snapshot);
    metadata_event_write_restore(manager,manager_class,"TryPredictNthLandingFrom",
                                 fixture->case_id,restored,snapshot);
    return applied&&state_before_ok&&pace_readback==requested_pace&&team_readback==fixture->curve_team&&
        invoke_ok&&!exception&&restored&&metadata_event_snapshot_matches(manager,manager_class,snapshot);
}

static void metadata_event_write_failure(const char* component,const char* reason) {
    write_text("{\"kind\":\"event_fixture_failure\",\"schema\":\"v014-event-v1\",\"component\":");
    metadata_quoted(component);write_text(",\"reason\":");metadata_quoted(reason);write_text("}\n");
}

/*
 * Keep the initialized scene/configuration that the event rows consume next
 * to the rows themselves.  These are metadata-name lookups on the live
 * objects, not copies of decompiler offsets.  A missing optional field or
 * getter is represented as null by the writer; the capture therefore cannot
 * silently substitute a v0.12 constant for a v0.14 value.
 */
static void metadata_event_write_scene_context(void* manager,void* manager_class,
                                                void* ball,void* ball_class) {
    void* court=metadata_field_object(manager_class,manager,"court");
    void* court_class=court?V014_FN(il2cpp_object_get_class,void*,void*)(court):0;
    write_text("{\"kind\":\"event_scene_context\",\"schema\":\"v014-event-v1\",\"read_only\":true");
    write_text(",\"manager_object\":");metadata_object_pointer(manager);
    write_text(",\"manager_fields\":{");
    metadata_write_raw_field(manager_class,manager,"courtLength","f32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,"netHeight","f32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,"courtY","f32",1);
    write_text(",");metadata_write_raw_field(manager_class,manager,"ballScale","f32",1);
    write_text(",\"courtCenter_object\":");
    metadata_object_pointer(metadata_field_object(manager_class,manager,"courtCenter"));
    write_text("},\"ball_bounce_config\":{");
    metadata_write_raw_field(ball_class,ball,"arcadeGravity","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"bounceRestitution","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"bounceForwardKeep","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"minBounceSpeed","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"maxSpeed","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"topspinForwardKick","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"topspinBounceScale","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"sliceForwardKeep","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"sliceBounceScale","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"dropForwardKeep","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"dropBounceScale","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"dropHopMin","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"dropHopMax","f32",1);
    write_text(",");metadata_write_raw_field(ball_class,ball,"flightPace","f32",1);
    write_text(",\"radius\":");
    void* radius_field=metadata_find_field(ball_class,"<Radius>k__BackingField");
    if(!radius_field||!ball) write_text("null");
    else {
        unsigned int radius_words[1]={0};
        V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
            (ball,radius_field,radius_words);
        write_text("{\"encoding\":\"f32\",\"words\":");
        metadata_words(radius_words,1);write_text("}");
    }
    write_text("},\"net_right\":{");
    void* net_method=metadata_method_checked(ball_class,"NetRight",1);
    write_text("\"method\":");metadata_trace_write_method_identity(net_method,ball_class);
    unsigned int net_words[3]={0,0,0};int net_ok[2]={0,0};void* net_exception=0;
    int team=0;void* net_args[]={&team};
    net_ok[0]=metadata_invoke_words(net_method,ball,net_args,net_words,3,&net_exception);
    write_text(",\"team0\":");if(net_ok[0])metadata_words(net_words,3);else write_text("null");
    write_text(",\"team0_exception\":");if(net_exception)write_text("true");else write_text("false");
    team=1;net_exception=0;
    net_ok[1]=metadata_invoke_words(net_method,ball,net_args,net_words,3,&net_exception);
    write_text(",\"team1\":");if(net_ok[1])metadata_words(net_words,3);else write_text("null");
    write_text(",\"team1_exception\":");if(net_exception)write_text("true");else write_text("false");
    write_text(",\"read_ok\":");if(net_ok[0]&&net_ok[1])write_text("true");else write_text("false");
    write_text("},\"court_object\":{\"object\":");metadata_object_pointer(court);
    write_text(",\"class\":");metadata_nullable_string(court_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(court_class):0);
    write_text(",\"fields\":{");
    metadata_write_raw_field(court_class,court,"courtLength","f32",1);
    write_text(",");metadata_write_raw_field(court_class,court,"doublesWidth","f32",1);
    write_text(",");metadata_write_raw_field(court_class,court,"singlesWidth","f32",1);
    write_text(",");metadata_write_raw_field(court_class,court,"netHeight","f32",1);
    write_text(",");metadata_write_raw_field(court_class,court,"courtY","f32",1);
    write_text(",");metadata_write_raw_field(court_class,court,"lineWidth","f32",1);
    write_text(",");metadata_write_raw_field(court_class,court,"interiorLineWidth","f32",1);
    write_text("},\"getters\":[");
    metadata_write_getter(court_class,court,"get_Length",1);
    write_text(",");metadata_write_getter(court_class,court,"get_Width",1);
    write_text(",");metadata_write_getter(court_class,court,"get_SinglesWidth",1);
    write_text(",");metadata_write_getter(court_class,court,"get_DoublesWidth",1);
    write_text(",");metadata_write_getter(court_class,court,"get_Net",1);
    write_text(",");metadata_write_getter(court_class,court,"get_SurfaceY",1);
    write_text(",");metadata_write_getter(court_class,court,"get_LineWidth",1);
    write_text(",");metadata_write_getter(court_class,court,"get_Center",3);
    write_text("]},\"effective_manager_getters\":[");
    metadata_write_getter(manager_class,manager,"get_CourtLength",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtWidth",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtSinglesWidth",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtDoublesWidth",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_NetHeight",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtY",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_CourtLineWidth",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_BallScale",1);
    write_text(",");metadata_write_getter(manager_class,manager,"get_OutMargin",1);
    write_text("],\"fixed_delta_time\":");
    if(metadata_core_time_class) metadata_write_getter(metadata_core_time_class,0,"get_fixedDeltaTime",1);
    else write_text("null");
    write_text("}\n");
    flush_log();
}

/*
 * Direct shot fixtures are kept beside the initialized event scene.  The
 * movement contract header supplies only metadata-bound calls; this adapter
 * owns JSON and the object-reference-safe snapshot.  In particular, an
 * il2cpp_field_set_value call for a managed reference must receive the object
 * itself, not the address of a temporary raw-word array.  The generic shot
 * contract remains useful for scalar fields and invocation, but this local
 * snapshot deliberately restores LastHitter/tennisManager through
 * metadata_set_object_field.
 */
typedef struct {
    void* manager;
    void* ball;
    void* last_hitter;
    void* tennis_manager;
    unsigned int manager_values[V014_SHOT_MANAGER_FIELD_CAPACITY][4];
    unsigned int ball_values[V014_SHOT_BALL_FIELD_CAPACITY][4];
    unsigned char manager_valid[V014_SHOT_MANAGER_FIELD_CAPACITY];
    unsigned char ball_valid[V014_SHOT_BALL_FIELD_CAPACITY];
    int valid;
} metadata_event_shot_snapshot;

static int metadata_event_shot_is_object(const V014ShotFieldSpec* spec) {
    return spec && spec->encoding == V014_SHOT_RAW_OBJECT;
}

static int metadata_event_shot_capture_snapshot(
    void* manager, void* manager_class, void* ball, void* ball_class,
    metadata_event_shot_snapshot* snapshot) {
    int ok = 1;
    if (!snapshot || !manager || !manager_class || !ball || !ball_class)
        return 0;
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->manager = manager;
    snapshot->ball = ball;
    for (unsigned int i = 0; i < V014_SHOT_MANAGER_FIELD_CAPACITY; ++i) {
        const V014ShotFieldSpec* spec = &v014_shot_manager_fields[i];
        if (metadata_event_shot_is_object(spec)) continue;
        snapshot->manager_valid[i] = (unsigned char)metadata_read_words_field(
            manager_class, manager, spec->managed_name,
            snapshot->manager_values[i], (unsigned int)spec->words);
        if (!snapshot->manager_valid[i] && spec->required) ok = 0;
    }
    for (unsigned int i = 0; i < V014_SHOT_BALL_FIELD_CAPACITY; ++i) {
        const V014ShotFieldSpec* spec = &v014_shot_ball_fields[i];
        if (metadata_event_shot_is_object(spec)) {
            void* value = metadata_field_object(ball_class, ball,
                                                 spec->managed_name);
            if (same(spec->managed_name, "<LastHitter>k__BackingField"))
                snapshot->last_hitter = value;
            else if (same(spec->managed_name, "tennisManager"))
                snapshot->tennis_manager = value;
            if (!value && spec->required) ok = 0;
            snapshot->ball_valid[i] = (unsigned char)(value != NULL ||
                                                       !spec->required);
            continue;
        }
        snapshot->ball_valid[i] = (unsigned char)metadata_read_words_field(
            ball_class, ball, spec->managed_name, snapshot->ball_values[i],
            (unsigned int)spec->words);
        if (!snapshot->ball_valid[i] && spec->required) ok = 0;
    }
    snapshot->valid = ok;
    return ok;
}

static int metadata_event_shot_restore_snapshot(
    void* manager_class, void* ball_class,
    const metadata_event_shot_snapshot* snapshot) {
    int ok = snapshot && snapshot->valid;
    if (!ok) return 0;
    for (unsigned int i = 0; i < V014_SHOT_MANAGER_FIELD_CAPACITY; ++i) {
        const V014ShotFieldSpec* spec = &v014_shot_manager_fields[i];
        if (!snapshot->manager_valid[i] || metadata_event_shot_is_object(spec)) {
            if (spec->required && !snapshot->manager_valid[i]) ok = 0;
            continue;
        }
        if (!metadata_set_words_field(manager_class, snapshot->manager,
                                      spec->managed_name,
                                      snapshot->manager_values[i],
                                      (unsigned int)spec->words)) ok = 0;
    }
    for (unsigned int i = 0; i < V014_SHOT_BALL_FIELD_CAPACITY; ++i) {
        const V014ShotFieldSpec* spec = &v014_shot_ball_fields[i];
        if (metadata_event_shot_is_object(spec)) {
            void* value = same(spec->managed_name,
                               "<LastHitter>k__BackingField")
                              ? snapshot->last_hitter
                              : snapshot->tennis_manager;
            if (!metadata_set_object_field(ball_class, snapshot->ball,
                                           spec->managed_name, value)) ok = 0;
            continue;
        }
        if (!snapshot->ball_valid[i]) {
            if (spec->required) ok = 0;
            continue;
        }
        if (!metadata_set_words_field(ball_class, snapshot->ball,
                                      spec->managed_name,
                                      snapshot->ball_values[i],
                                      (unsigned int)spec->words)) ok = 0;
    }
    return ok;
}

static int metadata_event_shot_snapshot_matches(
    void* manager_class, void* ball_class,
    const metadata_event_shot_snapshot* snapshot) {
    unsigned int current[4] = {0, 0, 0, 0};
    int ok = snapshot && snapshot->valid;
    if (!ok) return 0;
    for (unsigned int i = 0; i < V014_SHOT_MANAGER_FIELD_CAPACITY; ++i) {
        const V014ShotFieldSpec* spec = &v014_shot_manager_fields[i];
        if (metadata_event_shot_is_object(spec)) continue;
        if (!snapshot->manager_valid[i] || !metadata_read_words_field(
                manager_class, snapshot->manager, spec->managed_name, current,
                (unsigned int)spec->words)) {
            if (spec->required) ok = 0;
            continue;
        }
        for (unsigned int j = 0; j < spec->words; ++j)
            if (current[j] != snapshot->manager_values[i][j]) ok = 0;
    }
    for (unsigned int i = 0; i < V014_SHOT_BALL_FIELD_CAPACITY; ++i) {
        const V014ShotFieldSpec* spec = &v014_shot_ball_fields[i];
        if (metadata_event_shot_is_object(spec)) {
            void* value = metadata_field_object(ball_class, snapshot->ball,
                                                 spec->managed_name);
            void* expected = same(spec->managed_name,
                                  "<LastHitter>k__BackingField")
                                 ? snapshot->last_hitter
                                 : snapshot->tennis_manager;
            if (value != expected) ok = 0;
            continue;
        }
        if (!snapshot->ball_valid[i] || !metadata_read_words_field(
                ball_class, snapshot->ball, spec->managed_name, current,
                (unsigned int)spec->words)) {
            if (spec->required) ok = 0;
            continue;
        }
        for (unsigned int j = 0; j < spec->words; ++j)
            if (current[j] != snapshot->ball_values[i][j]) ok = 0;
    }
    return ok;
}

static void metadata_event_write_shot_field_specs(
    void* klass, void* object, const V014ShotFieldSpec* specs,
    unsigned int count) {
    write_text("{");
    for (unsigned int i = 0; i < count; ++i) {
        if (i) write_text(",");
        metadata_quoted(specs[i].key);
        write_text(":{\"managed_name\":");
        metadata_quoted(specs[i].managed_name);
        write_text(",\"word_count\":");
        metadata_number((unsigned int)specs[i].words);
        write_text(",\"encoding\":");
        metadata_number((unsigned int)specs[i].encoding);
        write_text(",\"required\":");
        if (specs[i].required) write_text("true"); else write_text("false");
        write_text(",\"read_ok\":");
        if (!klass || !object) {
            write_text("false,\"value\":null}");
            continue;
        }
        if (metadata_event_shot_is_object(&specs[i])) {
            write_text("true,\"value\":");
            metadata_object_pointer(metadata_field_object(
                klass, object, specs[i].managed_name));
            write_text("}");
            continue;
        }
        unsigned int words[4] = {0, 0, 0, 0};
        int read_ok = metadata_read_words_field(
            klass, object, specs[i].managed_name, words,
            (unsigned int)specs[i].words);
        if (read_ok) write_text("true,\"value\":");
        else write_text("false,\"value\":null");
        if (read_ok) metadata_words(words, (unsigned int)specs[i].words);
        write_text("}");
    }
    write_text("}");
}

static int metadata_event_write_shot_state(void* manager, void* manager_class,
                                           void* ball, void* ball_class) {
    int ok = manager && manager_class && ball && ball_class;
    write_text("{\"manager_fields\":");
    if (ok) metadata_event_write_shot_field_specs(
        manager_class, manager, v014_shot_manager_fields,
        V014_SHOT_MANAGER_FIELD_CAPACITY);
    else write_text("null");
    write_text(",\"ball_fields\":");
    if (ok) metadata_event_write_shot_field_specs(
        ball_class, ball, v014_shot_ball_fields, V014_SHOT_BALL_FIELD_CAPACITY);
    else write_text("null");
    write_text("}");
    return ok;
}

static void metadata_event_write_shot_method_specs(void* manager_class,
                                                   void* ball_class) {
    write_text("[");
    for (unsigned int i = 0;
         i < sizeof(v014_shot_methods) / sizeof(v014_shot_methods[0]); ++i) {
        const V014ShotMethodSpec* spec = &v014_shot_methods[i];
        void* klass = same(spec->declaring_class, "TennisBall")
                          ? ball_class
                          : manager_class;
        void* method = klass ? metadata_method_checked(klass, spec->name,
                                                        spec->args)
                             : 0;
        if (i) write_text(",");
        write_text("{\"declaring_class\":");
        metadata_quoted(spec->declaring_class);
        write_text(",\"name\":");metadata_quoted(spec->name);
        write_text(",\"args\":");metadata_number(spec->args);
        write_text(",\"reviewed_rva\":\"");write_hex(spec->rva);
        write_text("\",\"return_type\":");metadata_quoted(spec->return_type);
        write_text(",\"method\":");
        metadata_trace_write_method_identity(method, klass);
        write_text("}");
    }
    write_text("]");
}

static int metadata_event_write_shot_context(void* manager,
                                             void* manager_class,
                                             void* ball, void* ball_class) {
    V014ShotFixtureOps ops = {metadata_read_words_field,
                              metadata_set_words_field,
                              metadata_method_checked,
                              metadata_invoke_words,
                              metadata_invoke_void};
    V014ShotSceneGetterWords getters;
    int getter_ok = v014_shot_fixture_capture_scene_getters(
        &ops, manager_class, manager, ball_class, ball, &getters);
    write_text("{\"kind\":\"shot_fixture_context\",\"schema\":\"v014-shot-v1\"");
    write_text(",\"manager_object\":");metadata_object_pointer(manager);
    write_text(",\"ball_object\":");metadata_object_pointer(ball);
    write_text(",\"manager_fields\":");
    metadata_event_write_shot_field_specs(manager_class, manager,
        v014_shot_manager_fields, V014_SHOT_MANAGER_FIELD_CAPACITY);
    write_text(",\"ball_fields\":");
    metadata_event_write_shot_field_specs(ball_class, ball,
        v014_shot_ball_fields, V014_SHOT_BALL_FIELD_CAPACITY);
    write_text(",\"methods\":");
    metadata_event_write_shot_method_specs(manager_class, ball_class);
    write_text(",\"scene_getters\":{\"court_y\":");
    if (getters.court_y_valid) metadata_words(getters.court_y, 1); else write_text("null");
    write_text(",\"court_center\":");
    if (getters.court_center_valid) metadata_words(getters.court_center, 3); else write_text("null");
    write_text(",\"court_length\":");
    if (getters.court_length_valid) metadata_words(getters.court_length, 1); else write_text("null");
    write_text(",\"net_height\":");
    if (getters.net_height_valid) metadata_words(getters.net_height, 1); else write_text("null");
    write_text(",\"bounce_floor_y\":");
    if (getters.bounce_floor_y_valid) metadata_words(getters.bounce_floor_y, 1); else write_text("null");
    write_text(",\"net_right_team0\":");
    if (getters.net_right_team0_valid) metadata_words(getters.net_right_team0, 3); else write_text("null");
    write_text(",\"net_right_team1\":");
    if (getters.net_right_team1_valid) metadata_words(getters.net_right_team1, 3); else write_text("null");
    write_text(",\"default_aim_team0\":");
    if (getters.default_aim_team0_valid) metadata_words(getters.default_aim_team0, 3); else write_text("null");
    write_text(",\"default_aim_team1\":");
    if (getters.default_aim_team1_valid) metadata_words(getters.default_aim_team1, 3); else write_text("null");
    write_text("},\"read_ok\":");if(getter_ok)write_text("true");else write_text("false");
    write_text("}\n");flush_log();
    return getter_ok;
}

static void* metadata_event_shot_hitter_for_team(void* manager,
                                                 void* manager_class,
                                                 int team) {
    const char* field_name = team ? "<AwayPlayer>k__BackingField"
                                  : "<HomePlayer>k__BackingField";
    return metadata_field_object(manager_class, manager, field_name);
}

static int metadata_event_shot_apply_case(
    void* manager, void* manager_class, void* ball, void* ball_class,
    const V014ShotFixtureCase* fixture) {
    int ok = fixture && manager && manager_class && ball && ball_class;
    unsigned int shot_type;
    unsigned int in_play = 1U;
    unsigned int curve_team;
    unsigned int is_serve;
    unsigned int crossed_net;
    if (!ok) return 0;
    shot_type = (unsigned int)fixture->shot_type;
    curve_team = (unsigned int)fixture->team;
    is_serve = (unsigned int)fixture->is_serve;
    crossed_net = (unsigned int)fixture->crossed_net;
#define V014_SHOT_SET(name, value, words) \
    do { if (!metadata_set_words_field(ball_class, ball, name, value, words)) ok = 0; } while (0)
    V014_SHOT_SET("<Position>k__BackingField", fixture->from, 3);
    V014_SHOT_SET("<Velocity>k__BackingField", fixture->velocity, 3);
    V014_SHOT_SET("lastPosition", fixture->from, 3);
    V014_SHOT_SET("sliceCurve", &fixture->curve, 1);
    V014_SHOT_SET("curveTeam", &curve_team, 1);
    V014_SHOT_SET("lastHitPower", &fixture->power, 1);
    V014_SHOT_SET("flightPace", &fixture->flight_pace, 1);
    V014_SHOT_SET("<LastShotType>k__BackingField", &shot_type, 1);
    V014_SHOT_SET("<InPlay>k__BackingField", &in_play, 1);
    V014_SHOT_SET("<IsServe>k__BackingField", &is_serve, 1);
    V014_SHOT_SET("<HasCrossedNet>k__BackingField", &crossed_net, 1);
    void* hitter = metadata_event_shot_hitter_for_team(
        manager, manager_class, fixture->team);
    if (!hitter || !metadata_set_object_field(
        ball_class, ball, "<LastHitter>k__BackingField", hitter)) ok = 0;
#undef V014_SHOT_SET
    return ok;
}

static int metadata_event_shot_readback(
    void* manager, void* manager_class, void* ball, void* ball_class,
    const V014ShotFixtureCase* fixture) {
    unsigned int words[4] = {0, 0, 0, 0};
    unsigned int shot_type = (unsigned int)fixture->shot_type;
    unsigned int in_play = 1U;
    unsigned int is_serve = (unsigned int)fixture->is_serve;
    unsigned int crossed_net = (unsigned int)fixture->crossed_net;
    int ok = 1;
#define V014_SHOT_CHECK(name, value, words_count) \
    do { if (!metadata_read_words_field(ball_class, ball, name, words, words_count)) ok = 0; \
         else { const unsigned int* expected = value; for (unsigned int k = 0; k < words_count; ++k) if (words[k] != expected[k]) ok = 0; } } while (0)
    V014_SHOT_CHECK("<Position>k__BackingField", fixture->from, 3);
    V014_SHOT_CHECK("<Velocity>k__BackingField", fixture->velocity, 3);
    V014_SHOT_CHECK("lastPosition", fixture->from, 3);
    V014_SHOT_CHECK("sliceCurve", &fixture->curve, 1);
    V014_SHOT_CHECK("curveTeam", (const unsigned int*)&fixture->team, 1);
    V014_SHOT_CHECK("lastHitPower", &fixture->power, 1);
    V014_SHOT_CHECK("flightPace", &fixture->flight_pace, 1);
    V014_SHOT_CHECK("<LastShotType>k__BackingField", &shot_type, 1);
    V014_SHOT_CHECK("<InPlay>k__BackingField", &in_play, 1);
    V014_SHOT_CHECK("<IsServe>k__BackingField", &is_serve, 1);
    V014_SHOT_CHECK("<HasCrossedNet>k__BackingField", &crossed_net, 1);
    if (metadata_field_object(ball_class, ball, "<LastHitter>k__BackingField") !=
        metadata_event_shot_hitter_for_team(manager, manager_class,
                                            fixture->team)) ok = 0;
#undef V014_SHOT_CHECK
    return ok;
}

static int metadata_event_write_shot_case(
    void* manager, void* manager_class, void* ball, void* ball_class,
    const metadata_event_shot_snapshot* snapshot,
    const V014ShotFixtureOps* ops, const V014ShotFixtureCase* fixture) {
    static const char* operation_names[] = {
        "ComputeShotVelocity", "RebuildLaunchForAim", "SteerVelocityToAim",
        "EnforceLobLoft"};
    V014ShotCallInput input;
    int all_ok = 1;
    if (!fixture || !snapshot || !ops) return 0;
    memset(&input, 0, sizeof(input));
    memcpy(input.from, fixture->from, sizeof(input.from));
    memcpy(input.velocity, fixture->velocity, sizeof(input.velocity));
    memcpy(input.aim_target, fixture->aim_target, sizeof(input.aim_target));
    input.curve = fixture->curve;
    input.power = fixture->power;
    input.shot_type = fixture->shot_type;
    input.is_serve = fixture->is_serve;
    write_text("{\"kind\":\"shot_fixture\",\"schema\":\"v014-shot-v1\",\"case_id\":");
    metadata_quoted(fixture->id);write_text(",\"case_index\":");metadata_number(fixture->index);
    write_text(",\"dimensions\":{\"team\":");metadata_write_signed_decimal(fixture->team);
    write_text(",\"last_hitter_object\":");metadata_object_pointer(
        metadata_event_shot_hitter_for_team(manager, manager_class, fixture->team));
    write_text(",\"shot_type\":");metadata_write_signed_decimal(fixture->shot_type);
    write_text(",\"profile\":");metadata_number(fixture->profile);
    write_text(",\"power_f32_words\":");metadata_words(&fixture->power,1);
    write_text(",\"curve_f32_words\":");metadata_words(&fixture->curve,1);
    write_text(",\"flight_pace_f32_words\":");metadata_words(&fixture->flight_pace,1);
    write_text("},\"input\":{\"from_f32_words\":");metadata_words(input.from,3);
    write_text(",\"velocity_f32_words\":");metadata_words(input.velocity,3);
    write_text(",\"aim_target_f32_words\":");metadata_words(input.aim_target,3);
    write_text(",\"is_serve\":");metadata_words(&input.is_serve,1);write_text("}");
    write_text(",\"field_readback_ok\":");
    int applied = metadata_event_shot_apply_case(
        manager, manager_class, ball, ball_class, fixture);
    int readback = applied && metadata_event_shot_readback(
        manager, manager_class, ball, ball_class, fixture);
    if (!readback) all_ok = 0;
    if (readback) {
        /* The first call below is restored to the same controlled input after
         * every preceding operation; no direct method can leak velocity/cache
         * mutations into its sibling row. */
        write_text("true");
    } else write_text("false");
    write_text(",\"calls\":[");
    for (unsigned int op = 0; op < 4; ++op) {
        if (op) write_text(",");
        if (op) {
            int reset_ok = metadata_event_shot_restore_snapshot(
                manager_class, ball_class, snapshot);
            int reset_apply = reset_ok && metadata_event_shot_apply_case(
                manager, manager_class, ball, ball_class, fixture);
            int reset_readback = reset_apply && metadata_event_shot_readback(
                manager, manager_class, ball, ball_class, fixture);
            if (!reset_readback) all_ok = 0;
        }
        void* method = metadata_method_checked(ball_class, operation_names[op],
                                                op == 0 ? 4U : op == 3 ? 5U : 6U);
        V014ShotCallOutput output;
        void* exception = 0;
        int state_before_ok = 0;
        int invoke_ok = 0;
        int state_after_ok = 0;
        int restore_ok = 0;
        int restore_matches = 0;
        write_text("{\"operation\":");metadata_quoted(operation_names[op]);
        write_text(",\"method\":");metadata_trace_write_method_identity(method, ball_class);
        write_text(",\"state_before\":");
        if (readback && metadata_event_shot_apply_case(
                manager, manager_class, ball, ball_class, fixture) &&
            metadata_event_shot_readback(manager, manager_class, ball,
                                         ball_class, fixture))
            state_before_ok = metadata_event_write_shot_state(
                manager, manager_class, ball, ball_class);
        else write_text("null");
        if (state_before_ok) {
            if (op == 0) invoke_ok = v014_shot_fixture_compute(
                ops, ball_class, ball, &input, &output, &exception);
            else if (op == 1) invoke_ok = v014_shot_fixture_rebuild(
                ops, ball_class, ball, &input, &output, &exception);
            else if (op == 2) invoke_ok = v014_shot_fixture_steer(
                ops, ball_class, ball, &input, &output, &exception);
            else invoke_ok = v014_shot_fixture_enforce_lob_loft(
                ops, ball_class, ball, &input, &output, &exception);
        }
        write_text(",\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
        write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
        write_text(",\"result_f32_words\":");
        if (op == 0 && invoke_ok) metadata_words(output.result, 3); else write_text("null");
        write_text(",\"velocity_after_f32_words\":");
        if (invoke_ok) metadata_words(output.velocity_after, 3); else write_text("null");
        write_text(",\"state_after\":");
        state_after_ok = metadata_event_write_shot_state(
            manager, manager_class, ball, ball_class);
        write_text(",\"state_before_read_ok\":");if(state_before_ok)write_text("true");else write_text("false");
        write_text(",\"state_after_read_ok\":");if(state_after_ok)write_text("true");else write_text("false");
        restore_ok = metadata_event_shot_restore_snapshot(
            manager_class, ball_class, snapshot);
        restore_matches = metadata_event_shot_snapshot_matches(
            manager_class, ball_class, snapshot);
        write_text(",\"restore_api_ok\":");if(restore_ok)write_text("true");else write_text("false");
        write_text(",\"restore_matches_snapshot\":");if(restore_matches)write_text("true");else write_text("false");
        write_text("}");
        ++metadata_shot_call_rows;
        if (!method || !state_before_ok || !state_after_ok || !invoke_ok ||
            exception || !restore_ok || !restore_matches) all_ok = 0;
    }
    write_text("],\"restoration_scope\":\"manager_fields+ball_fields+LastHitter+tennisManager\"");
    write_text(",\"case_restore_matches_snapshot\":");
    int final_restore = metadata_event_shot_restore_snapshot(manager_class,
                                                              ball_class, snapshot);
    int final_matches = metadata_event_shot_snapshot_matches(manager_class,
                                                             ball_class, snapshot);
    if (final_matches) write_text("true"); else write_text("false");
    write_text(",\"case_restore_api_ok\":");if(final_restore)write_text("true");else write_text("false");
    write_text("}\n");flush_log();
    ++metadata_shot_case_rows;
    ++metadata_event_rows;
    return all_ok && applied && readback && final_restore && final_matches;
}

static int metadata_event_capture_shots(void* manager, void* manager_class,
                                        void* ball, void* ball_class) {
    V014ShotFixtureOps ops = {metadata_read_words_field,
                              metadata_set_words_field,
                              metadata_method_checked,
                              metadata_invoke_words,
                              metadata_invoke_void};
    metadata_event_shot_snapshot snapshot;
    int all_ok = 1;
    metadata_shot_case_rows = 0;
    metadata_shot_call_rows = 0;
    if (!metadata_event_write_shot_context(manager, manager_class, ball,
                                           ball_class)) all_ok = 0;
    if (!metadata_event_shot_capture_snapshot(manager, manager_class, ball,
                                              ball_class, &snapshot)) {
        metadata_event_write_failure("shot", "snapshot_fields_unavailable");
        metadata_shot_done = 1;
        metadata_shot_success = 0;
        return 0;
    }
    for (unsigned int i = 0; i < V014_SHOT_FIXTURE_CASE_COUNT; ++i) {
        V014ShotFixtureCase fixture;
        if (!v014_shot_fixture_case(i, &fixture) ||
            !metadata_event_write_shot_case(manager, manager_class, ball,
                                            ball_class, &snapshot, &ops,
                                            &fixture)) {
            all_ok = 0;
        }
    }
    int final_restore = metadata_event_shot_restore_snapshot(manager_class,
                                                              ball_class, &snapshot);
    int final_matches = metadata_event_shot_snapshot_matches(manager_class,
                                                             ball_class, &snapshot);
    if (!final_restore || !final_matches) all_ok = 0;
    write_text("{\"kind\":\"shot_fixture_status\",\"schema\":\"v014-shot-v1\",\"status\":");
    if (all_ok) metadata_quoted("complete"); else metadata_quoted("failed");
    write_text(",\"case_rows\":");metadata_number(metadata_shot_case_rows);
    write_text(",\"call_rows\":");metadata_number(metadata_shot_call_rows);
    write_text(",\"expected_case_rows\":");metadata_number(V014_SHOT_FIXTURE_CASE_COUNT);
    write_text(",\"expected_call_rows\":");metadata_number(V014_SHOT_FIXTURE_CASE_COUNT * 4U);
    write_text(",\"final_restore_api_ok\":");if(final_restore)write_text("true");else write_text("false");
    write_text(",\"final_matches_snapshot\":");if(final_matches)write_text("true");else write_text("false");
    write_text("}\n");flush_log();
    metadata_shot_done = 1;
    metadata_shot_success = all_ok && metadata_shot_case_rows == V014_SHOT_FIXTURE_CASE_COUNT &&
                            metadata_shot_call_rows == V014_SHOT_FIXTURE_CASE_COUNT * 4U;
    return metadata_shot_success;
}

typedef struct {
    const char* case_id;
    const char* operation;
    unsigned int team;
    unsigned int game_state;
    unsigned int require_serve_bounce;
    unsigned int serving_team;
    unsigned int has_bounced_in;
} metadata_direct_serve_case;

static int metadata_event_serve_direct_set_inputs(
    void* manager, void* manager_class, void* ball, void* ball_class,
    void* player, const metadata_direct_serve_case* fixture) {
    unsigned int serve_in_play = 1U;
    unsigned int receiver_touched = 0U;
    unsigned int last_shot_serve = 1U;
    unsigned int ball_serve = 1U;
    unsigned int ball_in_play = 1U;
    unsigned int crossed_net = 0U;
    unsigned int bounced_in = fixture ? fixture->has_bounced_in : 0U;
    int ok = manager && manager_class && ball && ball_class && player && fixture;
    if (!ok) return 0;
#define V014_SERVE_DIRECT_SET(klass, object, name, value, words) \
    do { if (!metadata_set_words_field(klass, object, name, value, words)) ok = 0; } while (0)
    V014_SERVE_DIRECT_SET(manager_class, manager,
                          "<CurrentGameState>k__BackingField",
                          &fixture->game_state, 1);
    V014_SERVE_DIRECT_SET(manager_class, manager,
                          "<ServingTeam>k__BackingField",
                          &fixture->serving_team, 1);
    V014_SERVE_DIRECT_SET(manager_class, manager, "requireServeBounce",
                          &fixture->require_serve_bounce, 1);
    V014_SERVE_DIRECT_SET(manager_class, manager, "serveInPlay",
                          &serve_in_play, 1);
    V014_SERVE_DIRECT_SET(manager_class, manager, "receiverTouchedBall",
                          &receiver_touched, 1);
    V014_SERVE_DIRECT_SET(manager_class, manager, "lastShotWasServe",
                          &last_shot_serve, 1);
    V014_SERVE_DIRECT_SET(ball_class, ball, "<IsServe>k__BackingField",
                          &ball_serve, 1);
    V014_SERVE_DIRECT_SET(ball_class, ball, "<InPlay>k__BackingField",
                          &ball_in_play, 1);
    V014_SERVE_DIRECT_SET(ball_class, ball,
                          "<HasCrossedNet>k__BackingField", &crossed_net, 1);
    V014_SERVE_DIRECT_SET(ball_class, ball,
                          "<HasBouncedInSinceHit>k__BackingField", &bounced_in, 1);
    if (!metadata_set_object_field(ball_class, ball,
                                   "<LastHitter>k__BackingField", player)) ok = 0;
#undef V014_SERVE_DIRECT_SET
    return ok;
}

static int metadata_event_serve_direct_readback(
    void* manager, void* manager_class, void* ball, void* ball_class,
    void* player, const metadata_direct_serve_case* fixture) {
    unsigned int words[2] = {0, 0};
    unsigned int one = 1U;
    unsigned int zero = 0U;
    int ok = 1;
#define V014_SERVE_DIRECT_CHECK(klass, object, name, expected, count) \
    do { if (!metadata_read_words_field(klass, object, name, words, count)) ok = 0; \
         else { const unsigned int* e = expected; for (unsigned int k = 0; k < count; ++k) if (words[k] != e[k]) ok = 0; } } while (0)
    V014_SERVE_DIRECT_CHECK(manager_class, manager,
                           "<CurrentGameState>k__BackingField",
                           &fixture->game_state, 1);
    V014_SERVE_DIRECT_CHECK(manager_class, manager,
                           "<ServingTeam>k__BackingField",
                           &fixture->serving_team, 1);
    V014_SERVE_DIRECT_CHECK(manager_class, manager, "requireServeBounce",
                           &fixture->require_serve_bounce, 1);
    V014_SERVE_DIRECT_CHECK(manager_class, manager, "serveInPlay", &one, 1);
    V014_SERVE_DIRECT_CHECK(manager_class, manager, "receiverTouchedBall",
                           &zero, 1);
    V014_SERVE_DIRECT_CHECK(manager_class, manager, "lastShotWasServe", &one, 1);
    V014_SERVE_DIRECT_CHECK(ball_class, ball, "<IsServe>k__BackingField", &one, 1);
    V014_SERVE_DIRECT_CHECK(ball_class, ball, "<InPlay>k__BackingField", &one, 1);
    V014_SERVE_DIRECT_CHECK(ball_class, ball,
                           "<HasCrossedNet>k__BackingField", &zero, 1);
    V014_SERVE_DIRECT_CHECK(ball_class, ball,
                           "<HasBouncedInSinceHit>k__BackingField",
                           &fixture->has_bounced_in, 1);
    void* player_class = player ? V014_FN(il2cpp_object_get_class,void*,void*)(player) : 0;
    unsigned int player_team = 0;
    if (!metadata_read_u32_field(player_class, player, "Team", &player_team) ||
        player_team != fixture->team) ok = 0;
    if (metadata_field_object(ball_class, ball,
                              "<LastHitter>k__BackingField") != player) ok = 0;
#undef V014_SERVE_DIRECT_CHECK
    return ok;
}

static int metadata_event_serve_direct_restore(
    void* manager, void* manager_class, void* ball, void* ball_class,
    const metadata_event_snapshot* snapshot, unsigned int original_require) {
    int ok = metadata_event_restore_snapshot(manager, manager_class, snapshot);
    if (!metadata_set_words_field(manager_class, manager, "requireServeBounce",
                                  &original_require, 1)) ok = 0;
    return ok;
}

static int metadata_event_serve_direct_require_matches(
    void* manager, void* manager_class, unsigned int expected) {
    unsigned int actual = 0;
    return metadata_read_words_field(manager_class, manager,
                                     "requireServeBounce", &actual, 1) &&
           actual == expected;
}

static int metadata_event_capture_serve_direct(void* manager,
                                               void* manager_class,
                                               void* ball, void* ball_class) {
    static const metadata_direct_serve_case cases[] = {
        {"must-let-home-player-home-server-required-b0", "MustLetServeBounce", 0, 2, 1, 0, 0},
        {"must-let-home-player-home-server-required-b1", "MustLetServeBounce", 0, 2, 1, 0, 1},
        {"must-let-home-player-home-server-not-required-b0", "MustLetServeBounce", 0, 2, 0, 0, 0},
        {"must-let-home-player-home-server-not-required-b1", "MustLetServeBounce", 0, 2, 0, 0, 1},
        {"must-let-home-player-away-server-required-b0", "MustLetServeBounce", 0, 2, 1, 1, 0},
        {"must-let-home-player-away-server-required-b1", "MustLetServeBounce", 0, 2, 1, 1, 1},
        {"must-let-home-player-away-server-not-required-b0", "MustLetServeBounce", 0, 2, 0, 1, 0},
        {"must-let-home-player-away-server-not-required-b1", "MustLetServeBounce", 0, 2, 0, 1, 1},
        {"must-let-away-player-home-server-required-b0", "MustLetServeBounce", 1, 2, 1, 0, 0},
        {"must-let-away-player-home-server-required-b1", "MustLetServeBounce", 1, 2, 1, 0, 1},
        {"must-let-away-player-home-server-not-required-b0", "MustLetServeBounce", 1, 2, 0, 0, 0},
        {"must-let-away-player-home-server-not-required-b1", "MustLetServeBounce", 1, 2, 0, 0, 1},
        {"must-let-away-player-away-server-required-b0", "MustLetServeBounce", 1, 2, 1, 1, 0},
        {"must-let-away-player-away-server-required-b1", "MustLetServeBounce", 1, 2, 1, 1, 1},
        {"must-let-away-player-away-server-not-required-b0", "MustLetServeBounce", 1, 2, 0, 1, 0},
        {"must-let-away-player-away-server-not-required-b1", "MustLetServeBounce", 1, 2, 0, 1, 1},
        {"struck-home-player-home-server-b0", "OnBallStruck", 0, 2, 1, 0, 0},
        {"struck-home-player-home-server-b1", "OnBallStruck", 0, 2, 1, 0, 1},
        {"struck-home-player-away-server-b0", "OnBallStruck", 0, 2, 1, 1, 0},
        {"struck-home-player-away-server-b1", "OnBallStruck", 0, 2, 1, 1, 1},
        {"struck-away-player-home-server-b0", "OnBallStruck", 1, 2, 1, 0, 0},
        {"struck-away-player-home-server-b1", "OnBallStruck", 1, 2, 1, 0, 1},
        {"struck-away-player-away-server-b0", "OnBallStruck", 1, 2, 1, 1, 0},
        {"struck-away-player-away-server-b1", "OnBallStruck", 1, 2, 1, 1, 1},
        {"score-home-direct", "TennisScore.AwardPoint", 0, 3, 0, 0, 0},
        {"score-away-direct", "TennisScore.AwardPoint", 1, 3, 0, 1, 0}
    };
    V014ServeFixtureOps ops = {metadata_read_words_field,
                               metadata_set_words_field,
                               metadata_method_checked,
                               metadata_invoke_words,
                               metadata_invoke_void};
    metadata_event_snapshot snapshot;
    void* score;
    void* score_class;
    void* home;
    void* away;
    unsigned int original_require = 0;
    int all_ok = 1;
    metadata_serve_direct_rows = 0;
    if (!metadata_event_take_snapshot(manager, manager_class, &snapshot) ||
        !metadata_read_words_field(manager_class, manager,
                                   "requireServeBounce", &original_require, 1)) {
        metadata_event_write_failure("serve_direct", "snapshot_fields_unavailable");
        metadata_serve_direct_done = 1;
        metadata_serve_direct_success = 0;
        return 0;
    }
    score = snapshot.score;
    score_class = score ? V014_FN(il2cpp_object_get_class, void*, void*)(score) : 0;
    home = metadata_field_object(manager_class, manager, "<HomePlayer>k__BackingField");
    away = metadata_field_object(manager_class, manager, "<AwayPlayer>k__BackingField");
    for (unsigned int i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const metadata_direct_serve_case* fixture = &cases[i];
        void* player = fixture->team ? away : home;
        void* method_class = fixture->operation[0] == 'T' ? score_class : manager_class;
        void* method = method_class ? metadata_method_checked(
            method_class, fixture->operation[0] == 'T' ? "AwardPoint" : fixture->operation,
            fixture->operation[0] == 'T' ? 1U : 1U) : 0;
        void* exception = 0;
        unsigned int output_words[1] = {0};
        void* player_class = player ? V014_FN(il2cpp_object_get_class,void*,void*)(player) : 0;
        unsigned int actual_player_team = 0;
        unsigned int actual_serving_team = 0;
        unsigned int actual_bounced_in = 0;
        int actual_player_team_ok = 0;
        int actual_serving_team_ok = 0;
        int actual_bounced_in_ok = 0;
        int invoke_ok = 0;
        int restored = 0;
        int matches = 0;
        int applied = metadata_event_serve_direct_restore(
            manager, manager_class, ball, ball_class, &snapshot,
            original_require);
        if (fixture->operation[0] == 'T') {
            unsigned int winner = fixture->team;
            /* Score AwardPoint is invoked on TennisScore and has no player or
             * ball input; retain the same baseline state around it. */
            applied = applied && player && score && score_class;
            write_text("{\"kind\":\"serve_direct_fixture\",\"schema\":\"v014-serve-direct-v1\",\"case_id\":");
            metadata_quoted(fixture->case_id);
            write_text(",\"operation\":\"TennisScore.AwardPoint\",\"winner_team\":");metadata_number(winner);
            write_text(",\"state_before\":");
            if (applied) metadata_trace_write_state(manager, manager_class); else write_text("null");
            if (applied) {
                invoke_ok = v014_serve_fixture_score_award_point(
                    &ops, score_class, score, winner, &exception);
            }
        } else {
            applied = applied && metadata_event_serve_direct_set_inputs(
                manager, manager_class, ball, ball_class, player, fixture);
            int readback = applied && metadata_event_serve_direct_readback(
                manager, manager_class, ball, ball_class, player, fixture);
            actual_player_team_ok = metadata_read_u32_field(
                player_class, player, "Team", &actual_player_team);
            actual_serving_team_ok = metadata_read_u32_field(
                manager_class, manager, "<ServingTeam>k__BackingField",
                &actual_serving_team);
            actual_bounced_in_ok = metadata_read_u32_field(
                ball_class, ball, "<HasBouncedInSinceHit>k__BackingField",
                &actual_bounced_in);
            write_text("{\"kind\":\"serve_direct_fixture\",\"schema\":\"v014-serve-direct-v1\",\"case_id\":");
            metadata_quoted(fixture->case_id);
            write_text(",\"operation\":");metadata_quoted(fixture->operation);
            write_text(",\"input\":{\"player_team_requested\":");metadata_number(fixture->team);
            write_text(",\"player_object\":");metadata_object_pointer(player);
            write_text(",\"player_team_actual\":");
            if (actual_player_team_ok) metadata_words(&actual_player_team,1); else write_text("null");
            write_text(",\"player_team_read_ok\":");if (actual_player_team_ok) write_text("true"); else write_text("false");
            write_text(",\"game_state\":");metadata_words(&fixture->game_state,1);
            write_text(",\"serving_team_requested\":");metadata_words(&fixture->serving_team,1);
            write_text(",\"serving_team_actual\":");
            if (actual_serving_team_ok) metadata_words(&actual_serving_team,1); else write_text("null");
            write_text(",\"serving_team_read_ok\":");if (actual_serving_team_ok) write_text("true"); else write_text("false");
            write_text(",\"require_serve_bounce\":");metadata_words(&fixture->require_serve_bounce,1);
            write_text(",\"has_bounced_in_requested\":");metadata_words(&fixture->has_bounced_in,1);
            write_text(",\"has_bounced_in_actual\":");
            if (actual_bounced_in_ok) metadata_words(&actual_bounced_in,1); else write_text("null");
            write_text(",\"has_bounced_in_read_ok\":");if (actual_bounced_in_ok) write_text("true"); else write_text("false");
            write_text("},\"input_readback_ok\":");if(readback)write_text("true");else write_text("false");
            write_text(",\"state_before\":");
            if (readback) metadata_trace_write_state(manager, manager_class); else write_text("null");
            int invoke_ok = 0;
            if (readback) {
                if (fixture->operation[0] == 'M') invoke_ok =
                    v014_serve_fixture_must_let_serve_bounce(
                        &ops, manager_class, manager, player, output_words, &exception);
                else invoke_ok = v014_serve_fixture_on_ball_struck(
                    &ops, manager_class, manager, player, &exception);
            }
            write_text(",\"return_bool_words\":");
            if (fixture->operation[0] == 'M' && invoke_ok) metadata_words(output_words,1); else write_text("null");
            write_text(",\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
            write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
            write_text(",\"state_after\":");metadata_trace_write_state(manager, manager_class);
            restored = metadata_event_serve_direct_restore(manager, manager_class,
                ball, ball_class, &snapshot, original_require);
            matches = restored && metadata_event_snapshot_matches(manager, manager_class,
                                                                   &snapshot) &&
                      metadata_event_serve_direct_require_matches(
                          manager, manager_class, original_require);
            write_text(",\"restore_api_ok\":");if(restored)write_text("true");else write_text("false");
            write_text(",\"restore_matches_snapshot\":");if(matches)write_text("true");else write_text("false");
            write_text(",\"restoration_scope\":\"manager_score_ball_and_home_away_stats+requireServeBounce\"}\n");
            flush_log();
            ++metadata_serve_direct_rows;
            if (!method || !readback || !invoke_ok || exception || !restored || !matches)
                all_ok = 0;
            continue;
        }
        /* The score path has no ball input but still reports the full managed
         * trace around the call, so score and manager side effects are visible. */
        write_text(",\"invoke_ok\":");if(invoke_ok)write_text("true");else write_text("false");
        write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
        write_text(",\"state_after\":");metadata_trace_write_state(manager, manager_class);
        restored = metadata_event_serve_direct_restore(manager, manager_class,
            ball, ball_class, &snapshot, original_require);
        matches = restored && metadata_event_snapshot_matches(manager, manager_class,
                                                               &snapshot) &&
                  metadata_event_serve_direct_require_matches(
                      manager, manager_class, original_require);
        write_text(",\"restore_api_ok\":");if(restored)write_text("true");else write_text("false");
        write_text(",\"restore_matches_snapshot\":");if(matches)write_text("true");else write_text("false");
        write_text(",\"restoration_scope\":\"manager_score_ball_and_home_away_stats+requireServeBounce\"}\n");
        flush_log();
        ++metadata_serve_direct_rows;
        if (!method || !invoke_ok || exception || !restored || !matches) all_ok = 0;
    }
    int final_restore = metadata_event_serve_direct_restore(
        manager, manager_class, ball, ball_class, &snapshot, original_require);
    int final_matches = final_restore && metadata_event_snapshot_matches(
        manager, manager_class, &snapshot) &&
        metadata_event_serve_direct_require_matches(manager, manager_class,
                                                     original_require);
    if (!final_restore || !final_matches) all_ok = 0;
    write_text("{\"kind\":\"serve_direct_fixture_status\",\"schema\":\"v014-serve-direct-v1\",\"status\":");
    if (all_ok) metadata_quoted("complete"); else metadata_quoted("failed");
    write_text(",\"rows\":");metadata_number(metadata_serve_direct_rows);
    write_text(",\"expected_rows\":");metadata_number(sizeof(cases) / sizeof(cases[0]));
    write_text(",\"final_restore_api_ok\":");if(final_restore)write_text("true");else write_text("false");
    write_text(",\"final_matches_snapshot\":");if(final_matches)write_text("true");else write_text("false");
    write_text("}\n");flush_log();
    metadata_serve_direct_done = 1;
    metadata_serve_direct_success = all_ok && metadata_serve_direct_rows == sizeof(cases) / sizeof(cases[0]);
    return metadata_serve_direct_success;
}

typedef struct {
    void* ball;
    void* original_last_hitter;
    int valid;
} metadata_event_curve_snapshot;

static int metadata_event_curve_snapshot_take(
    void* ball, void* ball_class, metadata_event_curve_snapshot* snapshot) {
    if (!ball || !ball_class || !snapshot ||
        !metadata_find_field(ball_class, "<LastHitter>k__BackingField"))
        return 0;
    snapshot->ball = ball;
    snapshot->original_last_hitter = metadata_field_object(
        ball_class, ball, "<LastHitter>k__BackingField");
    snapshot->valid = 1;
    return 1;
}

static int metadata_event_curve_snapshot_restore(
    void* ball_class, const metadata_event_curve_snapshot* snapshot) {
    if (!snapshot || !snapshot->valid || !snapshot->ball) return 0;
    if (!metadata_set_object_field(ball_class, snapshot->ball,
                                   "<LastHitter>k__BackingField",
                                   snapshot->original_last_hitter))
        return 0;
    return metadata_field_object(ball_class, snapshot->ball,
                                 "<LastHitter>k__BackingField") ==
           snapshot->original_last_hitter;
}

static void metadata_event_write_vector3_static_constants(void);

static void metadata_event_write_curve_context(
    void* manager, void* manager_class, void* ball, void* ball_class,
    const metadata_event_curve_snapshot* snapshot) {
    void* manager_method = metadata_method_checked(
        manager_class, "get_CourtCenter", 0);
    void* compute = metadata_method_checked(ball_class, "ComputeFlightCurve", 5);
    void* toward = metadata_method_checked(ball_class, "CurveTowardAim", 4);
    write_text("{\"kind\":\"curve_fixture_context\",\"schema\":\"v014-curve-v1\"");
    write_text(",\"manager_object\":");metadata_object_pointer(manager);
    write_text(",\"ball_object\":");metadata_object_pointer(ball);
    write_text(",\"tennis_manager_object\":");metadata_object_pointer(
        metadata_field_object(ball_class, ball, "tennisManager"));
    write_text(",\"last_hitter_object\":");metadata_object_pointer(
        snapshot ? snapshot->original_last_hitter : 0);
    write_text(",\"scene_getters\":{\"court_center\":");
    int center_ok = metadata_trace_write_getter_words(
        manager_class, manager, "get_CourtCenter", 3);
    write_text("},\"vector3_static_constants\":");
    metadata_event_write_vector3_static_constants();
    write_text(",\"methods\":{\"ComputeFlightCurve\":");
    metadata_trace_write_method_identity(compute, ball_class);
    write_text(",\"CurveTowardAim\":");
    metadata_trace_write_method_identity(toward, ball_class);
    write_text(",\"manager_court_center_method\":");
    metadata_trace_write_method_identity(manager_method, manager_class);
    write_text("},\"read_ok\":");
    if (manager_method && compute && toward && center_ok && manager && ball)
        write_text("true");
    else write_text("false");
    write_text("}\n");flush_log();
}

static void metadata_event_write_curve_hitter_team(void* hitter) {
    if (!hitter) { write_text("null"); return; }
    void* hitter_class = V014_FN(il2cpp_object_get_class, void*, void*)(hitter);
    unsigned int team = 0;
    int read_ok = metadata_read_u32_field(hitter_class, hitter, "Team", &team);
    if (read_ok) metadata_words(&team, 1); else write_text("null");
}

static void metadata_event_write_vector3_static_constants(void) {
    static const char* names[] = {"upVector", "downVector", "leftVector",
                                  "rightVector", "forwardVector",
                                  "backVector"};
    void* klass = metadata_core_image ? V014_FN(
        il2cpp_class_from_name, void*, void*, const char*, const char*)
        (metadata_core_image, "UnityEngine", "Vector3") : 0;
    write_text("{\"class\":");metadata_nullable_string(klass?
        V014_FN(il2cpp_class_get_name,const char*,void*)(klass):0);
    write_text(",\"namespace\":");metadata_nullable_string(klass?
        V014_FN(il2cpp_class_get_namespace,const char*,void*)(klass):0);
    write_text(",\"constants\":{");
    for (unsigned int i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        if (i) write_text(",");
        metadata_quoted(names[i]);write_text(":{");
        void* field = klass ? metadata_find_field(klass, names[i]) : 0;
        unsigned int flags = field ? V014_FN(
            il2cpp_field_get_flags,unsigned int,void*)(field) : 0;
        unsigned int words[3] = {0,0,0};
        int read_ok = field && (flags & 16U);
        if (read_ok) V014_FN(il2cpp_field_static_get_value,void,void*,void*)
            (field, words);
        write_text("\"flags\":");metadata_number(flags);
        write_text(",\"offset\":\"");
        if (field) metadata_write_signed_hex(V014_FN(
            il2cpp_field_get_offset,int,void*)(field));
        else write_text("0");
        write_text("\",\"read_method\":");
        if (read_ok) metadata_quoted("il2cpp_field_static_get_value");
        else write_text("null");
        write_text(",\"read_ok\":");if (read_ok) write_text("true"); else write_text("false");
        write_text(",\"value_f32_words\":");
        if (read_ok) metadata_words(words,3); else write_text("null");
        write_text("}");
    }
    write_text("},\"read_ok\":");if (klass) write_text("true"); else write_text("false");
    write_text("}");
}

static int metadata_event_capture_curves(void* manager, void* manager_class,
                                         void* ball, void* ball_class) {
    V014CurveFixtureOps ops = {metadata_method_checked, metadata_invoke_words};
    metadata_event_curve_snapshot snapshot;
    int all_ok = 1;
    metadata_curve_rows = 0;
    if (!metadata_event_curve_snapshot_take(ball, ball_class, &snapshot)) {
        write_text("{\"kind\":\"curve_fixture_status\",\"schema\":\"v014-curve-v1\",\"status\":\"failed\",\"reason\":\"last_hitter_field_unavailable\"}\n");
        flush_log();
        metadata_curve_done = 1;
        metadata_curve_success = 0;
        return 0;
    }
    metadata_event_write_curve_context(manager, manager_class, ball, ball_class,
                                       &snapshot);
    for (unsigned int i = 0; i < V014_CURVE_FIXTURE_CASE_COUNT; ++i) {
        V014CurveFixtureCase fixture;
        V014CurveCallOutput output;
        void* exception = 0;
        void* method = 0;
        void* requested_hitter = 0;
        void* actual_hitter = 0;
        int set_ok;
        int readback_ok;
        int invoke_ok = 0;
        int restore_ok;
        int matches;
        if (!v014_curve_fixture_case(i, &fixture)) {
            all_ok = 0;
            continue;
        }
        if (fixture.hitter_slot >= 0)
            requested_hitter = metadata_event_shot_hitter_for_team(
                manager, manager_class, fixture.hitter_slot);
        set_ok = metadata_event_curve_snapshot_restore(ball_class, &snapshot);
        set_ok = set_ok && metadata_set_object_field(
            ball_class, ball, "<LastHitter>k__BackingField", requested_hitter);
        actual_hitter = metadata_field_object(
            ball_class, ball, "<LastHitter>k__BackingField");
        readback_ok = set_ok && actual_hitter == requested_hitter;
        method = metadata_method_checked(
            ball_class, fixture.operation == V014_CURVE_COMPUTE_FLIGHT
                            ? "ComputeFlightCurve" : "CurveTowardAim",
            fixture.operation == V014_CURVE_COMPUTE_FLIGHT ? 5U : 4U);
        write_text("{\"kind\":\"curve_fixture\",\"schema\":\"v014-curve-v1\",\"case_id\":");
        metadata_quoted(fixture.id);
        write_text(",\"case_index\":");metadata_number(fixture.index);
        write_text(",\"operation\":");metadata_quoted(
            fixture.operation == V014_CURVE_COMPUTE_FLIGHT
                ? "ComputeFlightCurve" : "CurveTowardAim");
        write_text(",\"method\":");
        metadata_trace_write_method_identity(method, ball_class);
        write_text(",\"context\":{\"manager_available\":true,\"tennis_manager_object\":");
        metadata_object_pointer(metadata_field_object(ball_class, ball,
                                                      "tennisManager"));
        write_text(",\"hitter_slot\":");metadata_write_signed_decimal(
            fixture.hitter_slot);
        write_text(",\"requested_last_hitter\":");metadata_object_pointer(
            requested_hitter);
        write_text(",\"actual_last_hitter\":");metadata_object_pointer(
            actual_hitter);
        write_text(",\"actual_last_hitter_team\":");
        metadata_event_write_curve_hitter_team(actual_hitter);
        write_text(",\"set_ok\":");if (set_ok) write_text("true"); else write_text("false");
        write_text(",\"readback_ok\":");if (readback_ok) write_text("true"); else write_text("false");
        write_text("},\"input\":{\"team\":");metadata_write_signed_decimal(fixture.team);
        write_text(",\"physical_type\":");metadata_write_signed_decimal(fixture.physical_type);
        write_text(",\"trick_curve_side\":");metadata_write_signed_decimal(fixture.trick_curve_side);
        write_text(",\"from_f32_words\":");metadata_words(fixture.from, 3);
        write_text(",\"aim_target_f32_words\":");metadata_words(fixture.aim_target, 3);
        write_text(",\"power_f32_words\":");metadata_words(&fixture.power, 1);
        write_text(",\"strength_f32_words\":");metadata_words(&fixture.strength, 1);
        write_text(",\"fallback_sign_f32_words\":");metadata_words(&fixture.fallback_sign, 1);
        write_text("}");
        if (readback_ok) {
            if (fixture.operation == V014_CURVE_COMPUTE_FLIGHT)
                invoke_ok = v014_curve_fixture_compute(
                    &ops, ball_class, ball, &fixture, &output, &exception);
            else invoke_ok = v014_curve_fixture_toward_aim(
                &ops, ball_class, ball, &fixture, &output, &exception);
        }
        write_text(",\"invoke_ok\":");if (invoke_ok) write_text("true"); else write_text("false");
        write_text(",\"exception\":");if (exception) write_text("true"); else write_text("false");
        write_text(",\"result_f32_words\":");
        if (invoke_ok) metadata_words(output.result, 1); else write_text("null");
        restore_ok = metadata_event_curve_snapshot_restore(ball_class, &snapshot);
        matches = restore_ok && metadata_field_object(
            ball_class, ball, "<LastHitter>k__BackingField") ==
            snapshot.original_last_hitter;
        write_text(",\"restore_api_ok\":");if (restore_ok) write_text("true"); else write_text("false");
        write_text(",\"restore_matches_snapshot\":");if (matches) write_text("true"); else write_text("false");
        write_text(",\"restoration_scope\":\"ball.LastHitter\"}\n");
        flush_log();
        ++metadata_curve_rows;
        if (!method || !set_ok || !readback_ok || !invoke_ok || exception ||
            !restore_ok || !matches) all_ok = 0;
    }
    int final_restore = metadata_event_curve_snapshot_restore(ball_class, &snapshot);
    int final_matches = final_restore && metadata_field_object(
        ball_class, ball, "<LastHitter>k__BackingField") ==
        snapshot.original_last_hitter;
    write_text("{\"kind\":\"curve_fixture_status\",\"schema\":\"v014-curve-v1\",\"status\":");
    if (all_ok && final_restore && final_matches &&
        metadata_curve_rows == V014_CURVE_FIXTURE_CASE_COUNT)
        metadata_quoted("complete");
    else metadata_quoted("failed");
    write_text(",\"rows\":");metadata_number(metadata_curve_rows);
    write_text(",\"expected_rows\":");metadata_number(V014_CURVE_FIXTURE_CASE_COUNT);
    write_text(",\"final_restore_api_ok\":");if (final_restore) write_text("true"); else write_text("false");
    write_text(",\"final_matches_snapshot\":");if (final_matches) write_text("true"); else write_text("false");
    write_text("}\n");flush_log();
    metadata_curve_done = 1;
    metadata_curve_success = all_ok && final_restore && final_matches &&
                             metadata_curve_rows == V014_CURVE_FIXTURE_CASE_COUNT;
    return metadata_curve_success;
}

static int metadata_event_capture(void* manager,void* manager_class) {
    metadata_event_snapshot snapshot;
    if(!metadata_event_take_snapshot(manager,manager_class,&snapshot)) {
        metadata_event_write_failure("all","snapshot_fields_unavailable");
        return 0;
    }
    void* ball=snapshot.ball;
    void* ball_class=V014_FN(il2cpp_object_get_class,void*,void*)(ball);
    void* simulate=metadata_method_checked(ball_class,"Simulate",1);
    void* nth=metadata_method_checked(ball_class,"TryPredictNthLandingFrom",13);
    write_text("{\"kind\":\"event_fixture_status\",\"schema\":\"v014-event-v1\",\"status\":");
    if(simulate&&nth)metadata_quoted("started");else metadata_quoted("missing_validated_method");
    write_text(",\"methods\":{\"Simulate\":");if(simulate)write_text("true");else write_text("false");
    write_text(",\"TryPredictNthLandingFrom\":");if(nth)write_text("true");else write_text("false");write_text("}}\n");
    if(!simulate||!nth) {
        metadata_event_write_failure("all","required_method_missing_or_arity_mismatch");
        return 0;
    }
    metadata_event_write_scene_context(manager,manager_class,ball,ball_class);
    metadata_event_write_serve_context(manager,manager_class,ball,ball_class);
    metadata_event_serve_context_done=1;
    const metadata_simulate_case sim_cases[] = {
        {"early-zero",{0x3f800000,0x40000000,0x40400000},{0x40800000,0x40000000,0x40400000},0,0,0,0,1,0,0,0,0},
        {"ordinary-fixed-delta",{0xc1000000,0x40000000,0x00000000},{0x41300000,0x40400000,0x3f800000},0,0x3c9ba5e0,0,0,1,0,0,0,0},
        {"ordinary-lob",{0xc1000000,0x40000000,0x00000000},{0x41300000,0x40400000,0x3f800000},0,0x3f000000,5,1,1,0,0,0,0},
        {"net-cross-flat",{0xbe4ccccd,0x40000000,0x00000000},{0x41a00000,0x00000000,0x00000000},0,0x3ca3d70a,2,0,1,0,0,0,0},
        {"floor-bounce-drop",{0xc0a00000,0x3ecccccd,0x00000000},{0x40400000,0xc1a00000,0x00000000},0,0x3ca3d70a,4,0,1,0,0,0,0},
        {"lob-with-curve",{0xbf99999a,0x40400000,0x40000000},{0x41f00000,0x40000000,0x00000000},0x40800000,0x3d23d70a,5,1,1,0,0,0,0},
        {"curve-left",{0xbf99999a,0x40400000,0xc0000000},{0x41f00000,0x40000000,0x00000000},0xc0800000,0x3d23d70a,6,0,1,0,0,0,0},
        {"serve-floor",{0xc1200000,0x40400000,0x00000000},{0x41f00000,0xc1200000,0x00000000},0,0x3ca3d70a,0,0,1,0,1,0,1},
        {.case_id="tape-cross-doubles",.position={0xbf800000,0x3f800000,0},
         .velocity={0x41f00000,0,0},.curve=0,.dt=0x3da3d70a,.shot_type=0,
         .curve_team=0,.in_play=1,.is_serve=0,.crossed_net=0,.bounced_in=0,
         .bounce_count=0,.expected_branch="tape-cross",.current_game_state=3,
         .current_half=1,.last_strike_near=0,.peak_near=0,.last_hitter_set=1,
         .last_hitter_slot=0},
        {.case_id="floor-bounce-owned",.position={0x40a00000,0x3ecccccd,0},
         .velocity={0x40400000,0xc1a00000,0},.curve=0,.dt=0x3da3d70a,
         .shot_type=0,.curve_team=0,.in_play=1,.is_serve=0,.crossed_net=0,
         .bounced_in=0,.bounce_count=0,.expected_branch="floor-bounce",
         .current_game_state=3,.current_half=1,.last_strike_near=0,
         .peak_near=0,.last_hitter_set=1,.last_hitter_slot=0},
        {.case_id="scrape-near-contact",.position={0x40a00000,0x3ecccccd,0},
         .velocity={0x40400000,0xc1a00000,0},.curve=0,.dt=0x3da3d70a,
         .shot_type=0,.curve_team=0,.in_play=1,.is_serve=0,.crossed_net=0,
         .bounced_in=0,.bounce_count=0,.expected_branch="scrape",
         .current_game_state=3,.current_half=0,.last_strike_near=1,
         .peak_near=1,.last_hitter_set=1,.last_hitter_slot=0},
        {.case_id="out-after-floor",.position={0x41a00000,0x3ecccccd,0},
         .velocity={0,0xc1a00000,0},.curve=0,.dt=0x3da3d70a,.shot_type=0,
         .curve_team=0,.in_play=1,.is_serve=0,.crossed_net=0,.bounced_in=0,
         .bounce_count=0,.expected_branch="out",.current_game_state=3,
         .current_half=1,.last_strike_near=0,.peak_near=0,
         .last_hitter_set=1,.last_hitter_slot=0},
        {.case_id="second-bounce-long-step",.position={0x40a00000,0x3ecccccd,0},
         .velocity={0x40400000,0xc1a00000,0},.curve=0,.dt=0x3da3d70a,
         .shot_type=0,.curve_team=0,.in_play=1,.is_serve=0,.crossed_net=0,
         .bounced_in=0,.bounce_count=1,.expected_branch="second-bounce",
         .current_game_state=3,.current_half=1,.last_strike_near=0,
         .peak_near=0,.last_hitter_set=1,.last_hitter_slot=0}
    };
    int all_ok=metadata_event_getter_success&&metadata_event_serve_context_done;
    for(unsigned int i=0;i<sizeof(sim_cases)/sizeof(sim_cases[0]);++i) {
        if(!metadata_event_write_simulate_case(manager,manager_class,ball,ball_class,simulate,
                                               &snapshot,&sim_cases[i],i)) {
            metadata_event_write_failure("Simulate",sim_cases[i].case_id);all_ok=0;
        }
    }
    const metadata_nth_landing_case nth_cases[] = {
        {"flat-one-bounce",{0x00000000,0x3f800000,0x00000000},{0x41200000,0x41000000,0x00000000},0,2,0,0,1,0,0,0,0,0},
        {"flat-two-bounce",{0xc1000000,0x3f800000,0x00000000},{0x41800000,0x41200000,0x00000000},0,2,0,0,2,0,0,0,0,1},
        {"topspin-kick",{0xc1000000,0x3f800000,0x00000000},{0x41800000,0x41200000,0x00000000},0,0,0,0,1,1,0,0,0,0},
        {"slice-team-one",{0xc1000000,0x3f800000,0x40000000},{0x41800000,0x41200000,0x00000000},0x40800000,1,0,0,1,0,1,0,0,1},
        {"drop-shot",{0xbf800000,0x3f99999a,0x00000000},{0x41400000,0x3f800000,0x00000000},0xc0800000,4,0,1,1,0,0,1,0,0},
        {"crossed-net-flat",{0xbe800000,0x40400000,0x00000000},{0x41a00000,0x00000000,0x00000000},0,2,0,1,1,0,0,0,0,0},
        {"serve-stop-tape",{0xbe800000,0x3f800000,0x00000000},{0x41a00000,0x00000000,0x00000000},0,0,1,0,1,0,0,0,1,0},
        {"high-speed-cap-lob",{0xc1000000,0x40800000,0x00000000},{0x447a0000,0x00000000,0x00000000},0,5,0,0,1,0,0,0,0,1},
        {"negative-curve-team-one",{0x41000000,0x40400000,0xc0000000},{0xc1200000,0x41000000,0x00000000},0xc0800000,6,0,1,2,0,0,0,0,1},
        {"zero-velocity-flat",{0x00000000,0x3f800000,0x00000000},{0x00000000,0x00000000,0x00000000},0,2,0,0,1,0,0,0,0,0}
    };
    for(unsigned int i=0;i<sizeof(nth_cases)/sizeof(nth_cases[0]);++i) {
        if(!metadata_event_write_nth_case(manager,manager_class,ball,ball_class,nth,
                                          &snapshot,&nth_cases[i],i)) {
            metadata_event_write_failure("TryPredictNthLandingFrom",nth_cases[i].case_id);all_ok=0;
        }
    }
    if(!metadata_event_capture_shots(manager,manager_class,ball,ball_class))
        all_ok=0;
    if(!metadata_event_capture_serve_direct(manager,manager_class,ball,ball_class))
        all_ok=0;
    if(!metadata_event_capture_curves(manager,manager_class,ball,ball_class))
        all_ok=0;
    int restored=metadata_event_restore_snapshot(manager,manager_class,&snapshot);
    int matches=metadata_event_snapshot_matches(manager,manager_class,&snapshot);
    write_text("{\"kind\":\"event_fixture_status\",\"schema\":\"v014-event-v1\",\"status\":");
    if(all_ok&&restored&&matches)metadata_quoted("complete");else metadata_quoted("failed");
    write_text(",\"simulate_rows\":");metadata_number(metadata_simulate_rows);
    write_text(",\"nth_landing_rows\":");metadata_number(metadata_nth_landing_rows);
    write_text(",\"rows\":");metadata_number(metadata_event_rows);
    write_text(",\"getter_items_success\":");if(metadata_event_getter_success)write_text("true");else write_text("false");
    write_text(",\"serve_context_emitted\":");if(metadata_event_serve_context_done)write_text("true");else write_text("false");
    write_text(",\"shot_fixture_success\":");if(metadata_shot_success)write_text("true");else write_text("false");
    write_text(",\"shot_case_rows\":");metadata_number(metadata_shot_case_rows);
    write_text(",\"shot_call_rows\":");metadata_number(metadata_shot_call_rows);
    write_text(",\"serve_direct_success\":");if(metadata_serve_direct_success)write_text("true");else write_text("false");
    write_text(",\"serve_direct_rows\":");metadata_number(metadata_serve_direct_rows);
    write_text(",\"curve_success\":");if(metadata_curve_success)write_text("true");else write_text("false");
    write_text(",\"curve_rows\":");metadata_number(metadata_curve_rows);
    write_text(",\"final_restore_api_ok\":");if(restored)write_text("true");else write_text("false");
    write_text(",\"final_matches_snapshot\":");if(matches)write_text("true");else write_text("false");write_text("}\n");
    metadata_event_success=all_ok&&restored&&matches;metadata_event_done=1;
    if(metadata_event_success) {
        write_text("{\"kind\":\"metadata_capture_complete\",\"fixture\":\"simulate+nth-landing+direct-shot+direct-serve+curve-v014\",\"status\":\"complete\",\"event_rows\":");
        metadata_number(metadata_event_rows);write_text("}\n");
    } else {
        write_text("{\"kind\":\"metadata_capture_complete\",\"fixture\":\"simulate+nth-landing+direct-shot+direct-serve+curve-v014\",\"status\":\"failed\",\"event_rows\":");
        metadata_number(metadata_event_rows);write_text("}\n");
    }
    metadata_capture_complete=1;flush_log();
    return metadata_event_success;
}
#endif

#ifdef V014_NATURAL_TRACE
/*
 * Natural owned-match trace.  This mode deliberately does not call the
 * synthetic physics/event fixtures.  It seeds Unity once, queues the normal
 * graph-driven match, and records before/after managed callback boundaries
 * until the first point is resolved (or a bounded diagnostic budget expires).
 */
static int metadata_natural_is_selected(const char* owner,const char* name) {
    if(!owner||!name) return 0;
    if(same(owner,"SimulationTickDriver")&&same(name,"FixedUpdate")) return 1;
    if(same(owner,"DeterministicMover")&&same(name,"FixedUpdate")) return 1;
    if(same(owner,"Stamina")&&same(name,"OnSimulationTick")) return 1;
    if(same(owner,"TennisGameManager")&&
       (same(name,"Start")||same(name,"Update")||same(name,"FixedUpdate")||
        same(name,"OnTennisTick")||same(name,"BeginServe")||
        same(name,"OnBallStruck")||same(name,"OnBallBounced")||
        same(name,"OnBallOut")||same(name,"OnBallHitNet")||
        same(name,"HandleServeFault")||same(name,"AwardPoint"))) return 1;
    if(same(owner,"TennisBall")&&
       (same(name,"Simulate")||same(name,"Hit")||same(name,"LaunchInbound")||
        same(name,"TossForServe")||same(name,"ParkForServe")||
        same(name,"HandleBounce")||same(name,"BounceOffNet"))) return 1;
    if(same(owner,"TennisPlayer")&&
       (same(name,"FixedUpdate")||same(name,"ProcessSwing")||same(name,"TryContact")||
        same(name,"ReadBrainInputs")||
        same(name,"ReleaseChargedSwing")||same(name,"FinishSwing")||
        same(name,"CommitServeOverhandRelease")||same(name,"GradeRallyContact")||
        same(name,"GradeServeContact")||same(name,"EvaluateHitAccuracy"))) return 1;
    return 0;
}

static const char* metadata_natural_event_kind(const char* owner,const char* name) {
    if(!owner||!name) return "lifecycle";
    if(same(owner,"SimulationTickDriver")&&same(name,"FixedUpdate")) return "tick";
    if(same(owner,"DeterministicMover")&&same(name,"FixedUpdate")) return "movement_fixed_update";
    if(same(owner,"Stamina")&&same(name,"OnSimulationTick")) return "stamina_tick";
    if(same(name,"OnBallBounced")||same(name,"HandleBounce")) return "bounce";
    if(same(name,"OnBallStruck")||same(name,"Hit")||same(name,"LaunchInbound")||
       same(name,"TryContact")||same(name,"FinishSwing")||same(name,"GradeRallyContact")||
       same(name,"GradeServeContact")||same(name,"EvaluateHitAccuracy")) return "contact";
    if(same(name,"AwardPoint")) return "point";
    if(same(name,"HandleServeFault")) return "serve_fault";
    if(same(name,"BeginServe")||same(name,"TossForServe")||same(name,"ParkForServe")) return "serve";
    if(same(name,"ProcessSwing")||same(name,"ReleaseChargedSwing")||
       same(name,"CommitServeOverhandRelease")) return "action";
    if(same(owner,"TennisPlayer")&&same(name,"ReadBrainInputs")) return "brain_inputs";
    if(same(name,"Simulate")||same(name,"BounceOffNet")) return "ball_step";
    return "lifecycle";
}

static void metadata_natural_write_arg_words(void** args,unsigned int index,unsigned int words) {
    if(args&&args[index]) metadata_words(args[index],words); else write_text("null");
}

static void metadata_natural_write_arg_object(void** args,unsigned int index) {
    if(args&&args[index]) metadata_object_pointer(*(void**)args[index]); else write_text("null");
}

static void metadata_natural_write_arguments(const char* owner,const char* name,
                                             void** args) {
    write_text("{");
    if(same(owner,"SimulationTickDriver")&&same(name,"FixedUpdate")) {
        write_text("\"argument_count\":0");
    } else if(same(owner,"DeterministicMover")&&same(name,"FixedUpdate")) {
        write_text("\"argument_count\":0");
    } else if(same(owner,"Stamina")&&same(name,"OnSimulationTick")) {
        write_text("\"argument_count\":0");
    } else if(same(owner,"TennisPlayer")&&same(name,"ReadBrainInputs")) {
        write_text("\"argument_count\":0");
    } else if(same(owner,"TennisBall")&&same(name,"Simulate")) {
        write_text("\"dt_f32_words\":");metadata_natural_write_arg_words(args,0,1);
    } else if(same(owner,"TennisBall")&&same(name,"Hit")) {
        write_text("\"hitter\":");metadata_natural_write_arg_object(args,0);
        write_text(",\"shot_type\":");metadata_natural_write_arg_words(args,1,1);
        write_text(",\"aim_f32_words\":");metadata_natural_write_arg_words(args,2,3);
        write_text(",\"power_f32_words\":");metadata_natural_write_arg_words(args,3,1);
        write_text(",\"apply_fire\":");metadata_natural_write_arg_words(args,4,1);
        write_text(",\"trick_curve_side\":");metadata_natural_write_arg_words(args,5,1);
    } else if(same(owner,"TennisBall")&&same(name,"LaunchInbound")) {
        write_text("\"origin_f32_words\":");metadata_natural_write_arg_words(args,0,3);
        write_text(",\"aim_target_f32_words\":");metadata_natural_write_arg_words(args,1,3);
        write_text(",\"shot_type\":");metadata_natural_write_arg_words(args,2,1);
        write_text(",\"power_f32_words\":");metadata_natural_write_arg_words(args,3,1);
        write_text(",\"is_serve\":");metadata_natural_write_arg_words(args,4,1);
    } else if(same(owner,"TennisBall")&&same(name,"TossForServe")) {
        write_text("\"position_f32_words\":");metadata_natural_write_arg_words(args,0,3);
        write_text(",\"velocity_f32_words\":");metadata_natural_write_arg_words(args,1,3);
    } else if(same(owner,"TennisBall")&&same(name,"HandleBounce")) {
        write_text("\"point_f32_words\":");metadata_natural_write_arg_words(args,0,3);
    } else if(same(owner,"TennisGameManager")&&same(name,"OnBallStruck")) {
        write_text("\"hitter\":");metadata_natural_write_arg_object(args,0);
    } else if(same(owner,"TennisGameManager")&&same(name,"OnBallBounced")) {
        write_text("\"ball\":");metadata_natural_write_arg_object(args,0);
        write_text(",\"half\":");metadata_natural_write_arg_words(args,1,1);
        write_text(",\"point_f32_words\":");metadata_natural_write_arg_words(args,2,3);
        write_text(",\"impact_speed_f32_words\":");metadata_natural_write_arg_words(args,3,1);
    } else if(same(owner,"TennisGameManager")&&same(name,"OnBallOut")) {
        write_text("\"ball\":");metadata_natural_write_arg_object(args,0);
        write_text(",\"point_f32_words\":");metadata_natural_write_arg_words(args,1,3);
    } else if(same(owner,"TennisGameManager")&&same(name,"AwardPoint")) {
        write_text("\"team\":");metadata_natural_write_arg_words(args,0,1);
        write_text(",\"reason\":");
        if(args&&args[1])metadata_trace_write_managed_string(*(void**)args[1]);else write_text("null");
        write_text(",\"score_location_f32_words\":");metadata_natural_write_arg_words(args,2,3);
        write_text(",\"ball_velocity_f32_words\":");metadata_natural_write_arg_words(args,3,3);
    } else if(same(owner,"TennisGameManager")&&same(name,"HandleServeFault")) {
        write_text("\"reason\":");
        if(args&&args[0])metadata_trace_write_managed_string(*(void**)args[0]);else write_text("null");
    } else if(same(owner,"TennisGameManager")&&same(name,"BeginServe")) {
        write_text("\"is_ad_court\":");metadata_natural_write_arg_words(args,0,1);
    } else if(same(owner,"TennisGameManager")&&same(name,"OnBallHitNet")) {
        write_text("\"ball\":");metadata_natural_write_arg_object(args,0);
    } else {
        write_text("\"argument_capture\":\"not_recovered_for_this_method\"");
    }
    write_text("}");
}

static void metadata_natural_write_return(void* result) {
    write_text("{\"object\":");metadata_object_pointer(result);write_text("}");
}

static int metadata_natural_write_trace_row(void* method,void* klass,void* object,
                                            void** args,const char* owner,
                                            const char* name,const char* phase,
                                            unsigned int callback_index,
                                            unsigned int boundary_tick,
                                            void* result,void* exception) {
    write_text("{\"kind\":\"trace_callback\",\"schema\":\"v014-trace-v1\",\"callback_index\":");
    metadata_number(callback_index);write_text(",\"phase\":");metadata_quoted(phase);
    write_text(",\"boundary_tick\":");metadata_number(boundary_tick);
    write_text(",\"event_kind\":");metadata_quoted(metadata_natural_event_kind(owner,name));
    write_text(",\"owner\":");metadata_nullable_string(owner);
    write_text(",\"owner_object\":");metadata_object_pointer(object);
    write_text(",\"method\":");metadata_trace_write_method_identity(method,klass);
    write_text(",\"arguments\":");metadata_natural_write_arguments(owner,name,args);
    write_text(",\"clock\":");metadata_trace_write_clock();
    write_text(",\"simulation_clock\":");metadata_natural_write_simulation_clock();
    write_text(",\"random_state\":");metadata_natural_write_random_state();
    write_text(",\"state\":");
    int state_ok=metadata_trace_write_state(metadata_natural_manager,
                                            metadata_natural_manager_class);
    write_text(",\"state_read_ok\":");if(state_ok)write_text("true");else write_text("false");
    write_text(",\"callback_context\":");
    int callback_context_ok=metadata_natural_write_callback_context(
        owner,name,object,klass);
    write_text(",\"callback_context_read_ok\":");
    if(callback_context_ok)write_text("true");else write_text("false");
    write_text(",\"return\":");metadata_natural_write_return(result);
    write_text(",\"exception\":");if(exception)write_text("true");else write_text("false");
    write_text(",\"exception_object\":");metadata_object_pointer(exception);write_text("}\n");
    flush_log();
    return state_ok&&callback_context_ok;
}

static void metadata_natural_check_completion(void);
static void metadata_natural_write_terminal(int success,const char* reason);
static void __stdcall metadata_natural_stamina_hook(void* object,void* method_info);

static void metadata_natural_write_bytes(const BYTE* bytes,unsigned int count) {
    write_text("[");
    for(unsigned int i=0;i<count;++i) {
        if(i) write_text(",");
        metadata_number(bytes[i]);
    }
    write_text("]");
}

static void metadata_natural_write_stamina_hook_status(const BYTE* observed,
                                                       unsigned int observed_count) {
    write_text("{\"kind\":\"natural_hook_status\",\"schema\":\"v014-trace-v1\",\"owner\":\"Stamina\",\"method_name\":\"OnSimulationTick\",\"method\":");
    metadata_trace_write_method_identity(metadata_natural_stamina_method,
                                         metadata_natural_stamina_class);
    write_text(",\"target\":");
    if(metadata_natural_stamina_target) {
        write_text("\"");write_hex((QWORD)metadata_natural_stamina_target);write_text("\"");
    } else write_text("null");
    write_text(",\"trampoline\":");
    if(metadata_natural_stamina_trampoline) {
        write_text("\"");write_hex((QWORD)metadata_natural_stamina_trampoline);write_text("\"");
    } else write_text("null");
    write_text(",\"hook_attempted\":");
    if(metadata_natural_stamina_hook_attempted) write_text("true"); else write_text("false");
    write_text(",\"installed\":");
    if(metadata_natural_stamina_hooked) write_text("true"); else write_text("false");
    write_text(",\"reason\":");metadata_quoted(metadata_natural_stamina_hook_reason);
    write_text(",\"preamble_expected_bytes\":[64,83,72,131,236,64,128,61,14,215,122,6,0,72,139,217]");
    write_text(",\"preamble_observed_bytes\":");
    if(observed&&observed_count) metadata_natural_write_bytes(observed,observed_count);
    else write_text("null");
    write_text(",\"callback_count\":");metadata_number(metadata_natural_stamina_callbacks);
    write_text("}\n");
    flush_log();
}

/* Install a small absolute jump at the managed method entry.  The first
 * sixteen bytes contain one RIP-relative guard read; the instruction SHAPE
 * (opcodes + ModRM + imm8 + the trailing `mov rbx,rcx`) is validated before
 * copying it, and the displacement is relocated in the trampoline.  The
 * disp32 of the guard load (bytes 8..11) is a static-field address that moves
 * between builds (v0.14 vs v0.15f), so it is NOT part of the signature â€” the
 * trampoline below re-encodes the load from the OBSERVED bytes, so pinning it
 * would only make the hook build-specific.  No native address is selected
 * from a static RVA. */
static int metadata_natural_install_stamina_hook(void) {
    if(metadata_natural_stamina_hook_attempted)
        return metadata_natural_stamina_hooked;
    metadata_natural_stamina_hook_attempted=1;
    metadata_natural_stamina_hook_reason="not_attempted";
    static const BYTE expected[16]={0x40,0x53,0x48,0x83,0xec,0x40,0x80,0x3d,
                                    0x0e,0xd7,0x7a,0x06,0x00,0x48,0x8b,0xd9};
    BYTE observed[16];
    for(unsigned int i=0;i<16;++i) observed[i]=0;
    metadata_natural_stamina_class=metadata_game_image?
        V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
            (metadata_game_image,"","Stamina"):0;
    metadata_natural_stamina_method=metadata_method_checked(
        metadata_natural_stamina_class,"OnSimulationTick",0);
    metadata_natural_stamina_target=metadata_natural_stamina_method?
        (void*)*(const QWORD*)metadata_natural_stamina_method:0;
    if(metadata_natural_stamina_target&&metadata_module_base&&metadata_module_size&&
       (QWORD)metadata_natural_stamina_target>=metadata_module_base&&
       (QWORD)metadata_natural_stamina_target<=metadata_module_base+
           metadata_module_size-16) {
        for(unsigned int i=0;i<16;++i)
            observed[i]=((const BYTE*)metadata_natural_stamina_target)[i];
    }
    if(!metadata_natural_stamina_class) {
        metadata_natural_stamina_hook_reason="stamina_class_missing";
        metadata_natural_write_stamina_hook_status(observed,0);return 0;
    }
    if(!metadata_natural_stamina_method) {
        metadata_natural_stamina_hook_reason="stamina_method_missing_or_arity_mismatch";
        metadata_natural_write_stamina_hook_status(observed,0);return 0;
    }
    if(!metadata_natural_stamina_target||!metadata_module_base||!metadata_module_size||
       (QWORD)metadata_natural_stamina_target<metadata_module_base||
       (QWORD)metadata_natural_stamina_target>metadata_module_base+
           metadata_module_size-16) {
        metadata_natural_stamina_hook_reason="method_target_outside_gameassembly";
        metadata_natural_write_stamina_hook_status(observed,16);return 0;
    }
    int preamble_ok=1;
    for(unsigned int i=0;i<16;++i) {
        /* bytes 8..11 = RIP-relative disp32 to a static field: build-specific */
        if(i>=8&&i<12) continue;
        if(observed[i]!=expected[i]) preamble_ok=0;
    }
    if(!preamble_ok) {
        metadata_natural_stamina_hook_reason="preamble_mismatch";
        metadata_natural_write_stamina_hook_status(observed,16);return 0;
    }
    /* Version-tolerant sanity check in place of the pinned displacement: the
     * re-encoded guard must point INSIDE GameAssembly (a static field lives in
     * the module image).  This rejects a wrong match without pinning an
     * address that legitimately moves between builds. */
    {
        QWORD guard=(QWORD)((long long)((QWORD)metadata_natural_stamina_target+13)+
            (long long)*(const int*)(observed+8));
        if(guard<metadata_module_base||guard>=metadata_module_base+metadata_module_size) {
            metadata_natural_stamina_hook_reason="guard_outside_gameassembly";
            metadata_natural_write_stamina_hook_status(observed,16);return 0;
        }
    }

    /* The guard in this validated preamble is RIP-relative.  Re-encode the
     * copied prefix with a volatile absolute register instead of relying on
     * a near allocation (Wine may place VirtualAlloc far from GameAssembly). */
    BYTE* trampoline=(BYTE*)VirtualAlloc(0,0x1000,0x3000,0x40);
    if(!trampoline) {
        metadata_natural_stamina_hook_reason="trampoline_allocation_failed";
        metadata_natural_write_stamina_hook_status(observed,16);return 0;
    }
    QWORD guard_address=(QWORD)((long long)((QWORD)metadata_natural_stamina_target+13)+
        (long long)*(const int*)(observed+8));
    BYTE* trampoline_cursor=trampoline;
    *trampoline_cursor++=0x40;*trampoline_cursor++=0x53; /* push rbx */
    *trampoline_cursor++=0x48;*trampoline_cursor++=0x83;
    *trampoline_cursor++=0xec;*trampoline_cursor++=0x40; /* sub rsp,40h */
    *trampoline_cursor++=0x49;*trampoline_cursor++=0xbb;
    *(QWORD*)trampoline_cursor=guard_address;trampoline_cursor+=8;
    *trampoline_cursor++=0x41;*trampoline_cursor++=0x80;
    *trampoline_cursor++=0x3b;*trampoline_cursor++=0x00; /* cmp byte [r11],0 */
    *trampoline_cursor++=0x48;*trampoline_cursor++=0x8b;
    *trampoline_cursor++=0xd9; /* mov rbx,rcx; preserves cmp flags */
    BYTE return_jump[14]={0xff,0x25,0,0,0,0,0,0,0,0,0,0,0,0};
    *(QWORD*)(return_jump+6)=(QWORD)metadata_natural_stamina_target+16;
    memcpy(trampoline_cursor,return_jump,sizeof(return_jump));
    DWORD trampoline_old=0;
    VirtualProtect(trampoline,0x1000,0x20,&trampoline_old);

    BYTE patch[16]={0xff,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90};
    *(QWORD*)(patch+6)=(QWORD)&metadata_natural_stamina_hook;
    DWORD old_protect=0,ignored=0;
    if(!VirtualProtect(metadata_natural_stamina_target,16,0x40,&old_protect)) {
        metadata_natural_stamina_hook_reason="target_writable_protection_failed";
        metadata_natural_stamina_trampoline=trampoline;
        metadata_natural_write_stamina_hook_status(observed,16);return 0;
    }
    memcpy(metadata_natural_stamina_target,patch,16);
    int flushed=FlushInstructionCache((HANDLE)(QWORD)-1,
                                      metadata_natural_stamina_target,16);
    if(!flushed) {
        memcpy(metadata_natural_stamina_target,observed,16);
        FlushInstructionCache((HANDLE)(QWORD)-1,metadata_natural_stamina_target,16);
        VirtualProtect(metadata_natural_stamina_target,16,old_protect,&ignored);
        metadata_natural_stamina_hook_reason="instruction_cache_flush_failed";
        metadata_natural_stamina_trampoline=trampoline;
        metadata_natural_write_stamina_hook_status(observed,16);return 0;
    }
    VirtualProtect(metadata_natural_stamina_target,16,old_protect,&ignored);
    metadata_natural_stamina_trampoline=trampoline;
    metadata_natural_stamina_original=(metadata_natural_stamina_fn)trampoline;
    metadata_natural_stamina_hooked=1;
    metadata_natural_stamina_hook_reason="installed_dynamic_method_entry";
    metadata_natural_write_stamina_hook_status(observed,16);
    return 1;
}

static void __stdcall metadata_natural_stamina_hook(void* object,void* method_info) {
    int selected=metadata_natural_active&&!metadata_natural_done&&!metadata_busy&&
        GetCurrentThreadId()==metadata_main_thread&&
        metadata_natural_callbacks<metadata_natural_budget;
    unsigned int callback_index=0;
    unsigned int boundary_tick=metadata_natural_tick;
    if(selected) {
        callback_index=++metadata_natural_callbacks;
        ++metadata_natural_stamina_callbacks;
        metadata_natural_write_trace_row(metadata_natural_stamina_method,
            metadata_natural_stamina_class,object,0,"Stamina","OnSimulationTick",
            "before",callback_index,boundary_tick,0,0);
    }
    if(metadata_natural_stamina_original)
        metadata_natural_stamina_original(object,method_info);
    if(selected) {
        metadata_natural_write_trace_row(metadata_natural_stamina_method,
            metadata_natural_stamina_class,object,0,"Stamina","OnSimulationTick",
            "after",callback_index,boundary_tick,0,0);
        metadata_natural_check_completion();
    } else if(metadata_natural_active&&!metadata_natural_done&&!metadata_busy&&
              GetCurrentThreadId()==metadata_main_thread) {
        metadata_natural_check_completion();
    }
}

static void metadata_natural_write_terminal(int success,const char* reason) {
    write_text("{\"kind\":\"natural_trace_status\",\"schema\":\"v014-trace-v1\",\"status\":");
    if(success)metadata_quoted("complete");else metadata_quoted("failed");
    write_text(",\"reason\":");metadata_quoted(reason?reason:"");
    write_text(",\"callbacks\":");metadata_number(metadata_natural_callbacks);
    write_text(",\"stamina_hook_attempted\":");
    if(metadata_natural_stamina_hook_attempted)write_text("true");else write_text("false");
    write_text(",\"stamina_hook_installed\":");
    if(metadata_natural_stamina_hooked)write_text("true");else write_text("false");
    write_text(",\"stamina_callbacks\":");metadata_number(metadata_natural_stamina_callbacks);
    write_text(",\"ticks\":");metadata_number(metadata_natural_tick);
    write_text(",\"point_resolved\":");if(metadata_natural_success)write_text("true");else write_text("false");
    write_text(",\"clock\":");metadata_trace_write_clock();
    write_text(",\"simulation_clock\":");metadata_natural_write_simulation_clock();
    write_text(",\"random_state\":");metadata_natural_write_random_state();
    write_text(",\"final_state\":");
    int final_state_ok=metadata_trace_write_state(metadata_natural_manager,
                                                   metadata_natural_manager_class);
    write_text(",\"final_state_read_ok\":");if(final_state_ok)write_text("true");else write_text("false");
    write_text("}\n");
    write_text("{\"kind\":\"metadata_capture_complete\",\"fixture\":\"natural-owned-safe-v014\",\"status\":");
    if(success)metadata_quoted("complete");else metadata_quoted("failed");
    write_text(",\"trace_callbacks\":");metadata_number(metadata_natural_callbacks);write_text("}\n");
    metadata_natural_done=1;metadata_natural_active=0;metadata_capture_complete=1;flush_log();
}

static void metadata_natural_start_match(void* manager,void* manager_class) {
    metadata_natural_manager=manager;metadata_natural_manager_class=manager_class;
    if(!metadata_natural_install_stamina_hook()) {
        metadata_natural_write_terminal(0,"stamina_hook_install_failed");return;
    }
    int seed=20260907;void* seed_exception=0;
#ifdef PARITYMOD
    seed=parity_seed;
#endif
    void* seed_method=metadata_method_checked(metadata_core_random_class,"InitState",1);
    void* seed_args[]={&seed};int seed_ok=metadata_invoke_void(seed_method,0,seed_args,&seed_exception);
    write_text("{\"kind\":\"trace_initial\",\"schema\":\"v014-trace-v1\",\"seed\":");
    metadata_write_signed_decimal(seed);write_text(",\"seed_method\":");
    metadata_trace_write_method_identity(seed_method,metadata_core_random_class);
    write_text(",\"seed_invoke_ok\":");if(seed_ok)write_text("true");else write_text("false");
    write_text(",\"seed_exception\":");if(seed_exception)write_text("true");else write_text("false");
    write_text(",\"clock\":");metadata_trace_write_clock();
    write_text(",\"simulation_clock\":");metadata_natural_write_simulation_clock();
    write_text(",\"random_state\":");metadata_natural_write_random_state();
    write_text(",\"state\":");
    int initial_ok=metadata_trace_write_state(manager,manager_class);
    write_text(",\"state_read_ok\":");if(initial_ok)write_text("true");else write_text("false");write_text("}\n");
    flush_log();
    if(!seed_method||!seed_ok||seed_exception||!initial_ok) {
        metadata_natural_write_terminal(0,"seed_or_initial_snapshot_failed");return;
    }
    void* queue_method=metadata_method_checked(manager_class,"QueueStartMatchWhenGraphsReady",0);
    void* queue_exception=0;int queue_ok=metadata_invoke_void(queue_method,manager,0,&queue_exception);
    write_text("{\"kind\":\"trace_start\",\"schema\":\"v014-trace-v1\",\"method\":");
    metadata_trace_write_method_identity(queue_method,manager_class);
    write_text(",\"queue_invoke_ok\":");if(queue_ok)write_text("true");else write_text("false");
    write_text(",\"exception\":");if(queue_exception)write_text("true");else write_text("false");
    write_text(",\"exception_object\":");metadata_object_pointer(queue_exception);
    write_text(",\"clock\":");metadata_trace_write_clock();
    write_text(",\"simulation_clock\":");metadata_natural_write_simulation_clock();
    write_text(",\"random_state\":");metadata_natural_write_random_state();
    write_text(",\"state\":");
    int start_state_ok=metadata_trace_write_state(manager,manager_class);
    write_text(",\"state_read_ok\":");if(start_state_ok)write_text("true");else write_text("false");write_text("}\n");
    flush_log();
    if(!queue_method||!queue_ok||queue_exception||!start_state_ok) {
        metadata_natural_write_terminal(0,"queue_start_failed");return;
    }
    metadata_natural_started=1;metadata_natural_active=1;
    /* These flags prevent the historical synthetic fixture path from running
     * after Start; all subsequent observations are natural callbacks. */
    metadata_sweep_done=1;metadata_physics_done=1;
}

#ifdef PARITYMOD
/* ---- ParityMod block B: state snapshot, command poll, per-point re-arm.
 * Placed here so all metadata_* helpers above are already defined. Every
 * game-memory access is read-only; re-arm never writes game state. */
static unsigned int parity_points_done=0;
static int parity_last_resolved=0;
/* Per-point winner log (raw LastPointWinner words, resolution order).
 * The score class only exposes post-reset points, so shut-out games are
 * attributed from this log instead of guessed. */
static unsigned int parity_point_winners[256]= {0};
static unsigned int parity_point_count=0;
static unsigned int parity_point_truncated=0;
static HANDLE parity_snap_file;
static char parity_snap_buf[2048];
static unsigned int parity_snap_used;
static void parity_snap_flush(void) {
    DWORD w=0;
    if(parity_snap_used) WriteFile(parity_snap_file,parity_snap_buf,parity_snap_used,&w,0);
    parity_snap_used=0;
}
static void parity_snap_text(const char* s) {
    while(*s) {
        if(parity_snap_used==sizeof(parity_snap_buf)) parity_snap_flush();
        parity_snap_buf[parity_snap_used++]=*s++;
    }
}
static void parity_snap_uint(unsigned int v) {
    char b[12];int at=11;b[at]=0;
    do { b[--at]=(char)('0'+v%10);v/=10; } while(v);
    parity_snap_text(b+at);
}
static void parity_snap_int(int v) {
    long long w=v;
    if(w<0) { parity_snap_text("-");w=-w; }
    parity_snap_uint((unsigned int)w);
}
static void parity_snap_str(const char* s) {
    parity_snap_text("\"");
    if(s) while(*s) {
        char c=*s++;
        if(c=='"'||c=='\\') parity_snap_text("\\");
        if((BYTE)c>=32) { char o[2]={c,0};parity_snap_text(o); }
    }
    parity_snap_text("\"");
}
static unsigned int parity_read_manager_u32(const char* field) {
    unsigned int v=0;void* f;
    if(!metadata_natural_manager||!metadata_natural_manager_class) return 0;
    f=metadata_find_field(metadata_natural_manager_class,field);
    if(!f) return 0;
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
        (metadata_natural_manager,f,&v);
    return v;
}
static void parity_write_snapshot(int done_flag,int quit_flag) {
    unsigned int snap_resolved=0,game_state=0,serving=0,hp=0,ap=0;
    unsigned int last_point_winner=0,last_game_winner=0,hm=0,am=0;
    void* score=0;void* score_class=0;
    if(metadata_natural_manager&&metadata_natural_manager_class) {
        metadata_read_u32_field(metadata_natural_manager_class,
            metadata_natural_manager,"pointResolved",&snap_resolved);
        game_state=parity_read_manager_u32("<CurrentGameState>k__BackingField");
        serving=parity_read_manager_u32("<ServingTeam>k__BackingField");
        last_point_winner=parity_read_manager_u32(
            "<LastPointWinner>k__BackingField");
        score=metadata_field_object(metadata_natural_manager_class,
            metadata_natural_manager,"<Score>k__BackingField");
        if(score) {
            score_class=V014_FN(il2cpp_object_get_class,void*,void*)(score);
            if(score_class) {
                metadata_read_u32_field(score_class,score,
                    "<HomePoints>k__BackingField",&hp);
                metadata_read_u32_field(score_class,score,
                    "<AwayPoints>k__BackingField",&ap);
                metadata_read_u32_field(score_class,score,
                    "<LastGameWinner>k__BackingField",&last_game_winner);
                metadata_read_u32_field(score_class,score,
                    "<HomeMatches>k__BackingField",&hm);
                metadata_read_u32_field(score_class,score,
                    "<AwayMatches>k__BackingField",&am);
            }
        }
    }
    parity_snap_file=CreateFileW(L"parity_state.json",0x40000000,3,0,2,0x80,0);
    if(parity_snap_file==(HANDLE)-1) return;
    parity_snap_used=0;
    parity_snap_text("{\"tick\":");parity_snap_uint(metadata_natural_tick);
    parity_snap_text(",\"callbacks\":");parity_snap_uint(metadata_natural_callbacks);
    parity_snap_text(",\"points_done\":");parity_snap_uint(parity_points_done);
    parity_snap_text(",\"points_target\":");parity_snap_uint(parity_points_target);
    parity_snap_text(",\"point_resolved\":");parity_snap_uint(snap_resolved);
    parity_snap_text(",\"game_state\":");parity_snap_uint(game_state);
    parity_snap_text(",\"serving_team\":");parity_snap_uint(serving);
    parity_snap_text(",\"home_points\":");parity_snap_uint(hp);
    parity_snap_text(",\"away_points\":");parity_snap_uint(ap);
    parity_snap_text(",\"last_point_winner\":");parity_snap_uint(last_point_winner);
    parity_snap_text(",\"last_game_winner\":");parity_snap_uint(last_game_winner);
    parity_snap_text(",\"home_matches\":");parity_snap_uint(hm);
    parity_snap_text(",\"away_matches\":");parity_snap_uint(am);
    parity_snap_text(",\"point_winners\":[");
    for(unsigned int pi=0;pi<parity_point_count;pi++) {
        if(pi) parity_snap_text(",");
        parity_snap_uint(parity_point_winners[pi]);
    }
    parity_snap_text("],\"point_winners_truncated\":");
    parity_snap_text(parity_point_truncated?"true":"false");
    parity_snap_text(",\"seed\":");parity_snap_int(parity_seed);
    parity_snap_text(",\"restart_epoch\":");parity_snap_uint(parity_restart_epoch);
    parity_snap_text(",\"home\":");parity_snap_str(parity_home_save);
    parity_snap_text(",\"away\":");parity_snap_str(parity_away_save);
    parity_snap_text(",\"done\":");parity_snap_text(done_flag?"true":"false");
    parity_snap_text(",\"success\":");parity_snap_text(metadata_natural_success?"true":"false");
    parity_snap_text(",\"quit\":");parity_snap_text(quit_flag?"true":"false");
    parity_snap_text("}\n");
    parity_snap_flush();FlushFileBuffers(parity_snap_file);CloseHandle(parity_snap_file);
}

/* Find a game class by simple name across every loaded assembly. TimePlot is
 * not in the cached core/game image handles, so scan the domain on demand. */
static void* parity_find_class_by_name(const char* cls) {
    void* klass=0;
    if(!cls||!cls[0]) return 0;
    if(same(cls,"Random")) return metadata_core_random_class;
    void* domain=V014_FN(il2cpp_domain_get,void*)();
    QWORD n=0;void** asmbs=domain?
        (void**)V014_FN(il2cpp_domain_get_assemblies,void*,void*,QWORD*)(domain,&n):0;
    for(QWORD i=0;i<n&&!klass;++i) {
        void* img=asmbs[i]?V014_FN(il2cpp_assembly_get_image,void*,void*)(asmbs[i]):0;
        if(!img) continue;
        klass=V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
            (img,"",cls);
    }
    return klass;
}

/* Invoke the game's native TimePlot.ExportToJson on its singleton so the tick
 * series is written to the Timeplots dir WITHOUT relaunching. Called when a
 * restart replaces the live match, on quit, and on demand. */
static int parity_export_timeplot(const char* reason) {
    void* klass=parity_find_class_by_name("TimePlot");
    void* get_instance=klass?metadata_method_checked(klass,"get_Instance",0):0;
    void* export_json=klass?metadata_method_checked(klass,"ExportToJson",0):0;
    void* instance=0;void* exc=0;int ok=0;
    unsigned int serving=0xFFFFFFFFU;
    if(metadata_natural_manager&&metadata_natural_manager_class)
        metadata_read_u32_field(metadata_natural_manager_class,
            metadata_natural_manager,"<ServingTeam>k__BackingField",&serving);
    if(get_instance) instance=metadata_invoke(get_instance,0,0,&exc);
    if(!instance&&klass) {
        instance=metadata_read_static_object_field(klass,"<Instance>k__BackingField");
        if(!instance) instance=metadata_read_static_object_field(klass,"_instance");
        if(!instance) instance=metadata_read_static_object_field(klass,"instance");
    }
    exc=0;
    if(export_json&&instance) {
        metadata_invoke_void(export_json,instance,0,&exc);
        ok=exc?0:1;
    }
    write_text("{\"kind\":\"parity_cmd\",\"cmd\":\"export_timeplot\",\"reason\":");
    metadata_nullable_string(reason?reason:"");
    write_text(",\"class_found\":");write_text(klass?"true":"false");
    write_text(",\"instance\":");write_text(instance?"true":"false");
    write_text(",\"ok\":");write_text(ok?"true":"false");
    write_text(",\"seed\":");metadata_number((unsigned int)parity_seed);
    write_text(",\"epoch\":");metadata_number(parity_restart_epoch);
    write_text(",\"serving_team\":");metadata_number(serving);
    write_text(",\"tick\":");metadata_number(metadata_natural_tick);
    write_text(",\"home\":");metadata_nullable_string(parity_home_save);
    write_text(",\"away\":");metadata_nullable_string(parity_away_save);
    write_text("}\n");flush_log();
    return ok;
}

/* Subclass the game window so an interactive close (X button, WM_CLOSE) or
 * session end flushes the live match's timeplot before Unity tears down.
 * Installed lazily on the Unity main thread from parity_on_tick. */
static void* parity_window_original_proc=0;
static int parity_window_hooked=0;
static long long __stdcall parity_window_proc(void* hwnd,unsigned int msg,
                                              QWORD wparam,QWORD lparam) {
    if(msg==0x0010u) parity_export_timeplot("wm_close");
    else if(msg==0x0016u) parity_export_timeplot("wm_endsession");
    if(parity_window_original_proc)
        return (long long)CallWindowProcW(parity_window_original_proc,hwnd,msg,
                                          wparam,lparam);
    return 0;
}
static void parity_install_window_hook(void) {
    if(parity_window_hooked) return;
    void* hwnd=FindWindowW(L"UnityWndClass",0);
    if(!hwnd) return;
    parity_window_original_proc=SetWindowLongPtrW(hwnd,-4,
        (void*)&parity_window_proc);
    if(parity_window_original_proc) {
        parity_window_hooked=1;
        write_text("{\"kind\":\"parity_window\",\"hooked\":true,\"hwnd\":\"");
        write_hex((QWORD)hwnd);write_text("\"}\n");flush_log();
    }
}
#include "racing_extras.h"
static void parity_poll_cmd(void) {
    parity_fad_t fad;char buf[512];const char* p;char cmd[32];
    if(!GetFileAttributesExW(L"parity_cmd.json",0,&fad)) return;
    if(parity_cmd_have_time&&fad.w_lo==parity_cmd_seen_wtime_lo&&
        fad.w_hi==parity_cmd_seen_wtime_hi) return;
    parity_cmd_seen_wtime_lo=fad.w_lo;parity_cmd_seen_wtime_hi=fad.w_hi;
    parity_cmd_have_time=1;
    if(!parity_read_file(L"parity_cmd.json",buf,sizeof(buf),0)) return;
    p=parity_find(buf,"\"cmd\"");
    if(!p) return;
    parity_copy_str(p,cmd,sizeof(cmd));
    if(same(cmd,"quit")) {
        write_text("{\"kind\":\"parity_cmd\",\"cmd\":\"quit\"}\n");flush_log();
        parity_write_snapshot(1,1);
        parity_export_timeplot("quit");
        ExitProcess(0);
    } else if(same(cmd,"extras")) {
        write_text("XTRAS|cmd|extras|-1|1");xend();flush_log();
        racing_extras();
    } else if(same(cmd,"snapshot")) {
        write_text("{\"kind\":\"parity_cmd\",\"cmd\":\"snapshot\"}\n");flush_log();
        parity_write_snapshot(0,0);
    } else if(same(cmd,"rngtest")) {
        /* {"cmd":"rngtest","seed":N,"draws":D}: UnityEngine.Random
         * InitState(seed) then D x RandomRangeInt(0,2). Decisive test of
         * whether the setup server pick is the first RNG draw for the seed. */
        int seed=0,draws=4;const char* qq;void* exc=0;
        unsigned int out[8];int n=0;
        void* init=0;void* rr=0;
        qq=parity_find(buf,"\"seed\"");
        if(qq) parity_copy_int(qq,&seed);
        qq=parity_find(buf,"\"draws\"");
        if(qq) parity_copy_int(qq,&draws);
        if(draws<1) draws=1;if(draws>8) draws=8;
        if(metadata_core_random_class) {
            init=metadata_method_checked(metadata_core_random_class,"InitState",1);
            rr=metadata_method_checked(metadata_core_random_class,"RandomRangeInt",2);
        }
        if(init&&rr&&real_runtime_invoke) {
            void* a1[]={&seed};
            ((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
                (init,0,a1,&exc);
            if(!exc) {
                int lo=0,hi=2;
                for(;n<draws;++n) {
                    void* a2[]={&lo,&hi};void* res=0;
                    exc=0;
                    res=((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
                        (rr,0,a2,&exc);
                    if(!res||exc) break;
                    out[n]=(unsigned int)*(int*)V014_FN(il2cpp_object_unbox,void*,void*)(res);
                }
            }
        }
        write_text("{\"kind\":\"parity_cmd\",\"cmd\":\"rngtest\",\"seed\":");
        metadata_number((unsigned int)seed);write_text(",\"draws\":[");
        for(int i=0;i<n;++i) { if(i) write_text(",");metadata_number(out[i]); }
        write_text("]}\n");flush_log();
    } else if(same(cmd,"methods")) {
        /* List all methods (name + arg count) of metadata_core classes or a
         * named game class: {"cmd":"methods","class":"TennisGameManager"}.
         * Gives exact il2cpp names for follow-up invoke probes (RNG test). */
        const char* want=parity_find(buf,"\"class\"");
        char cls[128];cls[0]=0;
        if(want) parity_copy_str(want,cls,sizeof(cls));
        write_text("{\"kind\":\"parity_cmd\",\"cmd\":\"methods\",\"class\":");
        metadata_nullable_string(cls[0]?cls:"");
        write_text(",\"methods\":[");
        {
            void* klass=0;
            if(same(cls,"Random")) klass=metadata_core_random_class;
            else if(cls[0]) {
                void* domain=V014_FN(il2cpp_domain_get,void*)();
                QWORD n=0;void** asmbs=domain?
                    (void**)V014_FN(il2cpp_domain_get_assemblies,void*,void*,QWORD*)(domain,&n):0;
                for(QWORD i=0;i<n&&!klass;++i) {
                    void* img=asmbs[i]?V014_FN(il2cpp_assembly_get_image,void*,void*)(asmbs[i]):0;
                    if(!img) continue;
                    klass=V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
                        (img,"",cls);
                }
            }
            if(klass) {
                void* iter=0;void* mm=0;int first=1;
                while((mm=V014_FN(il2cpp_class_get_methods,void*,void*,void**)(klass,&iter))) {
                    const char* mn=V014_FN(il2cpp_method_get_name,const char*,const void*)(mm);
                    unsigned int ac=V014_FN(il2cpp_method_get_param_count,unsigned int,const void*)(mm);
                    if(!first) write_text(",");
                    first=0;
                    write_text("{\"name\":");metadata_nullable_string(mn);
                    write_text(",\"args\":");metadata_number(ac);write_text("}");
                }
            }
        }
        write_text("]}\n");flush_log();
    } else if(same(cmd,"export_timeplot")) {
        /* On-demand native timeplot flush: {"cmd":"export_timeplot"}. */
        parity_export_timeplot("api");
        parity_write_snapshot(0,0);
    } else if(same(cmd,"reset_match")) {
        /* Restart-button analog: invoke TennisGameManager.ResetMatch (0 args)
         * and report ServingTeam before/after. Lets the driver sweep the
         * setup/reset server selection without replaying full points. */
        unsigned int before=0xFFFFFFFFU,after=0xFFFFFFFFU;void* m=0;void* exc=0;
        if(metadata_natural_manager&&metadata_natural_manager_class) {
            metadata_read_u32_field(metadata_natural_manager_class,
                metadata_natural_manager,"<ServingTeam>k__BackingField",&before);
            m=V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
                (metadata_natural_manager_class,"ResetMatch",0);
            if(m&&real_runtime_invoke) {
                ((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
                    (m,metadata_natural_manager,0,&exc);
                metadata_read_u32_field(metadata_natural_manager_class,
                    metadata_natural_manager,"<ServingTeam>k__BackingField",&after);
            }
        }
        write_text("{\"kind\":\"parity_cmd\",\"cmd\":\"reset_match\",\"before\":");
        metadata_number(before);write_text(",\"after\":");metadata_number(after);
        write_text(",\"method\":");write_text(m?"true":"false");
        write_text(",\"rng\":");metadata_natural_write_random_state();
        write_text("}\n");flush_log();
        parity_write_snapshot(0,0);
    } else if(same(cmd,"restart_match")) {
        /* In-place rematch WITHOUT relaunching Unity: re-seed CoreRandom,
         * ResetMatch, then QueueStartMatchWhenGraphsReady â€” the same
         * sequence the launch path uses, so a driver can sweep seeds in
         * seconds instead of a ~90 s game boot per sample.
         *   {"cmd":"restart_match","seed":N,"points":P}
         * seed/points optional; without seed the RNG state is kept. */
        const char* qq;int new_seed=0;int have_seed=0;void* exc=0;
        int seed_ok=1,reset_ok=0,queue_ok=0;
        unsigned int before=0xFFFFFFFFU,after=0xFFFFFFFFU;
        qq=parity_find(buf,"\"seed\"");
        if(qq&&parity_copy_int(qq,&new_seed)) have_seed=1;
        qq=parity_find(buf,"\"points\"");
        if(qq){int pv=0;if(parity_copy_int(qq,&pv)&&pv>0)parity_points_target=(unsigned int)pv;}
        if(have_seed) {
            parity_seed=new_seed;
            void* init=metadata_core_random_class?
                metadata_method_checked(metadata_core_random_class,"InitState",1):0;
            if(init&&real_runtime_invoke) {
                void* a1[]={&new_seed};
                exc=0;
                ((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
                    (init,0,a1,&exc);
                seed_ok=!exc;
            } else seed_ok=0;
        }
        /* Capture the match being replaced before ResetMatch wipes it. */
        parity_export_timeplot("restart");
        if(metadata_natural_manager&&metadata_natural_manager_class) {
            metadata_read_u32_field(metadata_natural_manager_class,
                metadata_natural_manager,"<ServingTeam>k__BackingField",&before);
            void* rm=V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
                (metadata_natural_manager_class,"ResetMatch",0);
            if(rm&&real_runtime_invoke) {
                exc=0;
                ((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
                    (rm,metadata_natural_manager,0,&exc);
                reset_ok=!exc;
            }
            void* qm=metadata_method_checked(metadata_natural_manager_class,
                "QueueStartMatchWhenGraphsReady",0);
            if(qm&&real_runtime_invoke) {
                exc=0;
                ((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
                    (qm,metadata_natural_manager,0,&exc);
                queue_ok=!exc;
            }
            metadata_read_u32_field(metadata_natural_manager_class,
                metadata_natural_manager,"<ServingTeam>k__BackingField",&after);
        }
        /* Re-arm per-point accounting for the fresh match. */
        parity_points_done=0;parity_point_count=0;parity_point_truncated=0;
        parity_last_resolved=0;parity_last_snapshot_tick=0;
        ++parity_restart_epoch;
        metadata_natural_tick=0;metadata_natural_callbacks=0;
        metadata_natural_stamina_callbacks=0;
        metadata_natural_done=0;metadata_natural_active=1;
        write_text("{\"kind\":\"parity_cmd\",\"cmd\":\"restart_match\",\"seed\":");
        metadata_number((unsigned int)parity_seed);
        write_text(",\"have_seed\":");write_text(have_seed?"true":"false");
        write_text(",\"seed_ok\":");write_text(seed_ok?"true":"false");
        write_text(",\"reset_ok\":");write_text(reset_ok?"true":"false");
        write_text(",\"queue_ok\":");write_text(queue_ok?"true":"false");
        write_text(",\"before\":");metadata_number(before);
        write_text(",\"after\":");metadata_number(after);
        write_text(",\"rng\":");metadata_natural_write_random_state();
        write_text("}\n");flush_log();
        parity_write_snapshot(0,0);
    }
}
static void parity_on_tick(void) {
    parity_install_window_hook();
    parity_poll_cmd();
    if(metadata_natural_tick-parity_last_snapshot_tick>=256) {
        parity_last_snapshot_tick=metadata_natural_tick;
        parity_write_snapshot(0,0);
    }
}
/* Post-completion / idle poll: trace hooks go quiet once the point (or
 * budget) terminates, so the command file is also polled on every
 * main-thread invoke, throttled to 1 check per 256 invokes. */
static unsigned int parity_poll_counter=0;
static void parity_poll_throttled(void) {
    if((++parity_poll_counter&255)==0) parity_poll_cmd();
}
static void parity_handle_completion(void) {
    unsigned int resolved_value=0;
    int read_ok;
    if(!metadata_natural_active||metadata_natural_done||!metadata_natural_manager) return;
    read_ok=metadata_read_u32_field(metadata_natural_manager_class,
        metadata_natural_manager,"pointResolved",&resolved_value);
    if(read_ok&&resolved_value&&!parity_last_resolved) {
        int stamina_ok=metadata_natural_stamina_hooked&&
            metadata_natural_stamina_callbacks>0;
        unsigned int winner_value=0xFFFFFFFFU;
        int winner_ok=metadata_read_u32_field(metadata_natural_manager_class,
            metadata_natural_manager,"<LastPointWinner>k__BackingField",
            &winner_value);
        ++parity_points_done;
        parity_last_resolved=1;
        if(parity_point_count<256u) {
            parity_point_winners[parity_point_count++]=
                winner_ok?winner_value:0xFFFFFFFFU;
        } else {
            parity_point_truncated=1;
        }
        write_text("{\"kind\":\"parity_point\",\"index\":");metadata_number(parity_points_done);
        write_text(",\"winner\":");metadata_number(winner_ok?winner_value:0xFFFFFFFFU);
        write_text(",\"target\":");metadata_number(parity_points_target);
        write_text(",\"tick\":");metadata_number(metadata_natural_tick);
        write_text(",\"callbacks\":");metadata_number(metadata_natural_callbacks);
        write_text(",\"stamina_callbacks\":");metadata_number(metadata_natural_stamina_callbacks);
        write_text(",\"stamina_ok\":");write_text(stamina_ok?"true":"false");
        write_text(",\"clock\":");metadata_trace_write_clock();
        write_text(",\"simulation_clock\":");metadata_natural_write_simulation_clock();
        write_text(",\"random_state\":");metadata_natural_write_random_state();
        write_text("}\n");flush_log();
        if(parity_points_done>=parity_points_target) {
            metadata_natural_success=stamina_ok;
            metadata_natural_write_terminal(stamina_ok,
                stamina_ok?"point_resolved":"point_resolved_without_stamina_callback");
            parity_write_snapshot(1,0);
        } else {
            metadata_natural_tick=0;metadata_natural_callbacks=0;
            metadata_natural_stamina_callbacks=0;parity_last_snapshot_tick=0;
            parity_write_snapshot(0,0);
        }
    } else if(read_ok&&!resolved_value) {
        parity_last_resolved=0;
        if(metadata_natural_callbacks>=metadata_natural_budget) {
            metadata_natural_write_terminal(0,"callback_budget_exhausted_before_point");
            parity_write_snapshot(1,0);
        }
    } else if(metadata_natural_callbacks>=metadata_natural_budget) {
        metadata_natural_write_terminal(0,"callback_budget_exhausted_before_point");
        parity_write_snapshot(1,0);
    }
}
#endif
static void metadata_natural_check_completion(void) {
#ifdef PARITYMOD
    parity_handle_completion();
#else
    if(!metadata_natural_active||metadata_natural_done||!metadata_natural_manager) return;
    unsigned int resolved_value=0;
    int read_ok=metadata_read_u32_field(metadata_natural_manager_class,
        metadata_natural_manager,"pointResolved",&resolved_value);
    if(read_ok&&resolved_value) {
        int stamina_ok=metadata_natural_stamina_hooked&&
            metadata_natural_stamina_callbacks>0;
        metadata_natural_success=stamina_ok;
        metadata_natural_write_terminal(stamina_ok,
            stamina_ok?"point_resolved":"point_resolved_without_stamina_callback");
    } else if(metadata_natural_callbacks>=metadata_natural_budget) {
        metadata_natural_write_terminal(0,"callback_budget_exhausted_before_point");
    }
#endif
}
#endif

#endif

static void metadata_clamp_sweep(void* manager,void* manager_class) {
    void* method=V014_FN(il2cpp_class_get_method_from_name,void*,void*,const char*,int)
        (manager_class,"ClampMoveDestination",2);
    if(!method) {
        write_text("{\"kind\":\"clamp_sweep_status\",\"status\":\"missing_method\"}\n");
        metadata_sweep_done=1;
        return;
    }
    void* home_field=V014_FN(il2cpp_class_get_field_from_name,void*,void*,const char*)
        (manager_class,"<HomePlayer>k__BackingField");
    void* away_field=V014_FN(il2cpp_class_get_field_from_name,void*,void*,const char*)
        (manager_class,"<AwayPlayer>k__BackingField");
    void* fields[]={home_field,away_field};
    void* players[2]={0,0};
    for(unsigned int slot=0;slot<2;++slot) {
        if(fields[slot]) V014_FN(il2cpp_field_get_value,void,void*,void*,void*)
            (manager,fields[slot],&players[slot]);
    }
    if(!players[0]||!players[1]) {
        if(!metadata_sweep_wait_logged) {
            write_text("{\"kind\":\"clamp_sweep_status\",\"status\":\"waiting_for_both_players\"}\n");
            flush_log();metadata_sweep_wait_logged=1;
        }
        return;
    }
    metadata_clamp_snapshot snapshot;
    if(!metadata_take_clamp_snapshot(manager,manager_class,players[0],players[1],&snapshot)) {
        write_text("{\"kind\":\"clamp_sweep_status\",\"status\":\"snapshot_fields_unavailable\"}\n");
        metadata_sweep_done=1;flush_log();return;
    }
    const metadata_clamp_case cases[]={
        {"serve-state2-serve0-00",2,0,0,0},
        {"serve-state2-serve1-00",2,1,0,0},
        {"serve-state2-deuce-serve0-33",2,0,3,3},
        {"serve-state2-deuce-serve1-33",2,1,3,3},
        {"serve-state2-ad-home-serve0-43",2,0,4,3},
        {"serve-state2-ad-home-serve1-43",2,1,4,3},
        {"serve-state2-ad-away-serve0-34",2,0,3,4},
        {"serve-state2-ad-away-serve1-34",2,1,3,4},
    };
    unsigned int total_rows=0;unsigned int completed_cases=0;
    for(unsigned int i=0;i<sizeof(cases)/sizeof(cases[0]);++i) {
        unsigned int rows=metadata_clamp_sweep_case(manager,manager_class,
            players[0],players[1],method,&snapshot,&cases[i]);
        if(rows) {total_rows+=rows;++completed_cases;}
    }
    write_text("{\"kind\":\"clamp_sweep_status\",\"status\":\"complete\",\"cases\":");
    metadata_number(completed_cases);write_text(",\"rows\":");metadata_number(total_rows);
    write_text("}\n");
    metadata_sweep_done=1;
    flush_log();
}

static void metadata_try_clamp_after_callback(void* method,void* object) {
    if(metadata_sweep_done||!object||!method) return;
    void* klass=V014_FN(il2cpp_method_get_class,void*,const void*)(method);
    const char* owner=klass?V014_FN(il2cpp_class_get_name,const char*,void*)(klass):0;
    const char* name=V014_FN(il2cpp_method_get_name,const char*,const void*)(method);
    if(!same(owner,"TennisGameManager")||
       !(same(name,"Start")||same(name,"Update")||same(name,"FixedUpdate")||same(name,"OnTennisTick"))) return;
    void* home_field=V014_FN(il2cpp_class_get_field_from_name,void*,void*,const char*)
        (klass,"<HomePlayer>k__BackingField");
    void* away_field=V014_FN(il2cpp_class_get_field_from_name,void*,void*,const char*)
        (klass,"<AwayPlayer>k__BackingField");
    void* home=0;void* away=0;
    if(home_field)V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,home_field,&home);
    if(away_field)V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,away_field,&away);
    if(!home&&!away) {
        if(!metadata_sweep_wait_logged) {
            write_text("{\"kind\":\"clamp_sweep_status\",\"status\":\"waiting_for_players\"}\n");
            flush_log();metadata_sweep_wait_logged=1;
        }
        return;
    }
    metadata_clamp_sweep(object,klass);
}

static void metadata_discover(void) {
    metadata_discovered=1;
#ifdef PARITYMOD
    parity_config_load();
#endif
    write_text("{\"kind\":\"metadata_begin\",\"api_status\":\"candidate-resolved\",\"resolver_rows\":");
    metadata_write_unsigned_qword(resolve_count);
    write_text("}\n");
    metadata_write_module_span();
    write_text("{\"kind\":\"method_info_layout\",\"method_pointer_offset\":0,");
    write_text("\"source\":\"il2cpp-class-internals.h: MethodInfo.methodPointer\"}\n");
    metadata_layout_written=1;
    void* domain=V014_FN(il2cpp_domain_get,void*)();
    QWORD count=0;
    void** assemblies=domain?(void**)V014_FN(il2cpp_domain_get_assemblies,void*,void*,QWORD*)(domain,&count):0;
    if(!domain||!assemblies||count==0||count>4096) {
        write_text("{\"kind\":\"metadata_error\",\"reason\":\"domain_assemblies\"}\n");
        flush_log();return;
    }
    for(QWORD i=0;i<count;++i) {
        void* assembly=assemblies[i];
        void* image=assembly?V014_FN(il2cpp_assembly_get_image,void*,void*)(assembly):0;
        const char* image_name=image?V014_FN(il2cpp_image_get_name,const char*,void*)(image):0;
        write_text("{\"kind\":\"metadata_image\",\"name\":");metadata_nullable_string(image_name);
        write_text("}\n");
        ++metadata_assembly_count;
        if(!image||!image_name) continue;
        if(same(image_name,"Assembly-CSharp.dll")) {
            metadata_game_image=image;
            QWORD class_count=V014_FN(il2cpp_image_get_class_count,QWORD,void*)(image);
            if(class_count>20000) {
                write_text("{\"kind\":\"metadata_error\",\"reason\":\"class_count\"}\n");continue;
            }
            for(QWORD n=0;n<class_count;++n) {
                void* klass=(void*)V014_FN(il2cpp_image_get_class,void*,void*,QWORD)(image,n);
                if(!klass) continue;
                metadata_record_catalog(image_name,klass);
                const char* name=V014_FN(il2cpp_class_get_name,const char*,void*)(klass);
                if(metadata_interesting_class(name)) metadata_write_class(image_name,klass);
            }
        } else if(same(image_name,"UnityEngine.CoreModule.dll")) {
            metadata_core_image=image;
            metadata_core_time_class=V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
                (image,"UnityEngine","Time");
            metadata_core_object_class=V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
                (image,"UnityEngine","Object");
#if defined(V014_EVENT_CAPTURE) || defined(V014_NATURAL_TRACE)
            metadata_core_random_class=V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
                (image,"UnityEngine","Random");
#endif
            metadata_named_core_class(image,"UnityEngine","Time",image_name);
            metadata_named_core_class(image,"UnityEngine","Object",image_name);
#if defined(V014_EVENT_CAPTURE) || defined(V014_NATURAL_TRACE)
            metadata_named_core_class(image,"UnityEngine","Random",image_name);
#endif
        } else if(same(image_name,"UnityEngine.PhysicsModule.dll")) {
            metadata_named_core_class(image,"UnityEngine","Rigidbody",image_name);
            metadata_named_core_class(image,"UnityEngine","Physics",image_name);
            metadata_named_core_class(image,"UnityEngine","Collider",image_name);
        } else if(same(image_name,"UnityEngine.AIModule.dll")) {
            metadata_named_core_class(image,"UnityEngine.AI","NavMeshAgent",image_name);
        }
    }
    write_text("{\"kind\":\"metadata_complete\",\"assemblies\":");metadata_number(metadata_assembly_count);
    write_text(",\"catalog_classes\":");metadata_number(metadata_catalog_count);
    write_text(",\"selected_classes\":");metadata_number(metadata_selected_count);
    write_text(",\"methods\":");metadata_number(metadata_method_count);
    write_text(",\"fields\":");metadata_number(metadata_field_count);
    write_text(",\"fixtures_expected\":true,\"fixture_mode\":");
#ifdef V014_EVENT_CAPTURE
    metadata_quoted("serving-clamp-snapshot-restore+physics-v014+integrate-flight+event-sim-nth+direct-shot+direct-serve");
#elif defined(V014_NATURAL_TRACE)
    metadata_quoted("natural-owned-safe-v014");
#else
    metadata_quoted("serving-clamp-snapshot-restore+physics-v014+integrate-flight");
#endif
    write_text("}\n");
    flush_log();
}

static void* __stdcall metadata_observe_invoke(void* method,void* object,void** args,void** exception) {
    int on_main=!metadata_busy&&GetCurrentThreadId()==metadata_main_thread;
    if(on_main) {
        metadata_busy=1;
        if(!metadata_discovered) {
            if(metadata_apis_ready()) metadata_discover();
            else {
                write_text("{\"kind\":\"metadata_api_wait\",\"status\":\"candidate-incomplete\"}\n");
                flush_log();
            }
        }
        metadata_busy=0;
    }
    if(!real_runtime_invoke) return 0;
#ifdef V014_NATURAL_TRACE
    /* Natural capture is independent of the one-shot fixture gate below.
     * Once the owned match is queued, every selected managed boundary gets a
     * before/after row even though the metadata/clamp/physics setup flags are
     * already complete. */
    void* trace_klass=0;const char* trace_owner=0;const char* trace_name=0;
    int trace_selected=0;unsigned int trace_index=0;unsigned int trace_tick=0;
    if(on_main&&!metadata_busy&&metadata_natural_active&&!metadata_natural_done) {
        trace_klass=V014_FN(il2cpp_method_get_class,void*,const void*)(method);
        trace_owner=trace_klass?V014_FN(il2cpp_class_get_name,const char*,void*)(trace_klass):0;
        trace_name=V014_FN(il2cpp_method_get_name,const char*,const void*)(method);
        if(metadata_natural_is_selected(trace_owner,trace_name)&&
           metadata_natural_callbacks<metadata_natural_budget) {
            trace_selected=1;trace_index=++metadata_natural_callbacks;
            if(same(trace_owner,"SimulationTickDriver")&&same(trace_name,"FixedUpdate"))
                ++metadata_natural_tick;
            trace_tick=metadata_natural_tick;
            metadata_natural_write_trace_row(method,trace_klass,object,args,
                trace_owner,trace_name,"before",trace_index,trace_tick,0,0);
        }
    }
#endif
    void* result=((void* (__stdcall *)(void*,void*,void**,void**))real_runtime_invoke)
        (method,object,args,exception);
#ifdef V014_NATURAL_TRACE
    if(trace_selected) {
        void* call_exception=exception?*exception:0;
        metadata_natural_write_trace_row(method,trace_klass,object,args,
            trace_owner,trace_name,"after",trace_index,trace_tick,result,call_exception);
        metadata_natural_check_completion();
#ifdef PARITYMOD
        if(same(trace_owner,"SimulationTickDriver")&&same(trace_name,"FixedUpdate"))
            parity_on_tick();
#endif
    } else if(on_main&&!metadata_busy&&metadata_natural_active) {
        metadata_natural_check_completion();
    }
#endif
    if(on_main&&!metadata_busy&&
       (!metadata_sweep_done||!metadata_physics_done
#ifdef V014_EVENT_CAPTURE
        || !metadata_event_done
#endif
       )) {
        metadata_busy=1;
        void* klass=V014_FN(il2cpp_method_get_class,void*,const void*)(method);
        const char* owner=klass?V014_FN(il2cpp_class_get_name,const char*,void*)(klass):0;
        const char* name=V014_FN(il2cpp_method_get_name,const char*,const void*)(method);
#ifndef V014_NATURAL_TRACE
        if(same(owner,"TennisGameManager")&&same(name,"Start"))
            metadata_spawn_owned_players(object,klass);
        if(!metadata_sweep_done) metadata_try_clamp_after_callback(method,object);
        if(metadata_sweep_done&&!metadata_physics_done&&
           same(owner,"TennisGameManager")&&
            (same(name,"Start")||same(name,"Update")||same(name,"FixedUpdate")||
            same(name,"OnTennisTick")))
            metadata_physics_capture(object,klass);
#else
        if(same(owner,"TennisGameManager")&&same(name,"Start")&&!metadata_spawn_requested) {
            metadata_spawn_owned_players(object,klass);
            if(!metadata_natural_started&&!metadata_natural_done)
                metadata_natural_start_match(object,klass);
        }
#endif
#ifdef V014_EVENT_CAPTURE
        if(!metadata_event_done&&metadata_sweep_done&&metadata_physics_done&&
           same(owner,"TennisGameManager")&&
           (same(name,"Start")||same(name,"Update")||same(name,"FixedUpdate")||
            same(name,"OnTennisTick"))) {
            if(!metadata_event_getter_terminal) {
                int getter_status=getter_items_v014_capture(object,klass);
                if(getter_status==GETTER_ITEMS_V014_COMPLETE||
                   getter_status==GETTER_ITEMS_V014_FAILED) {
                    metadata_event_getter_terminal=1;
                    metadata_event_getter_success=(getter_status==GETTER_ITEMS_V014_COMPLETE);
                }
            }
            if(metadata_event_getter_terminal)
                metadata_event_capture(object,klass);
        }
#endif
        metadata_busy=0;
    }
#if defined(PARITYMOD) && defined(V014_NATURAL_TRACE)
    if(on_main&&!metadata_busy) parity_poll_throttled();
#endif
    return result;
}
