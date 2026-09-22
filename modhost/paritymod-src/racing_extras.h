/* RacingV2 on-demand value capture. Trigger: write {"cmd":"extras"} into
 * parity_cmd.json in the game dir; parity_poll_cmd dispatches here.
 * Rows are plain text, mixed into probe-startup.jsonl:
 *   XTRAS|kind|gate|key|index|value     (index -1 = scalar; value may contain |)
 */
static void xrow(const char* kind,const char* gate,const char* key,int index) {
    write_text("XTRAS|");write_text(kind);write_text("|");write_text(gate);
    write_text("|");write_text(key);write_text("|");
    metadata_write_signed_decimal(index);write_text("|");
}
static void xend(void) { char nl[2];nl[0]=10;nl[1]=0;write_text(nl); }
static void xutf16(void* str) {
    if(!str) { write_text("null"); return; }
    unsigned int len=V014_FN(il2cpp_string_length,unsigned int,void*)(str);
    const unsigned short* chars=V014_FN(il2cpp_string_chars,const unsigned short*,void*)(str);
    for(unsigned int i=0;i<len&&i<160;++i) {
        unsigned short c=chars[i];
        if(c>=32&&c<127) { char s[2];s[0]=(char)c;s[1]=0;write_text(s); }
        else { write_text("?"); }
    }
}
static void* xclass(const char* name) {
    return metadata_game_image?
        V014_FN(il2cpp_class_from_name,void*,void*,const char*,const char*)
        (metadata_game_image,"",name):0;
}
static void xscalar_u32(void* klass,void* obj,const char* gate,const char* key) {
    unsigned int v=0;
    if(metadata_read_u32_field(klass,obj,key,&v)) {
        xrow("scalar",gate,key,-1);metadata_number(v);xend();return;
    }
    void* field=metadata_find_field(klass,key);
    if(field) {
        V014_FN(il2cpp_field_static_get_value,void,void*,void*)(field,&v);
        xrow("scalar",gate,key,-1);metadata_number(v);xend();
    }
}
static void xgate(const char* name) {
    void* klass=xclass(name);void* exc=0;void* m;
    if(!klass) { xrow("missing",name,"",-1);xend();return; }
    void* obj=V014_FN(il2cpp_object_new,void*,void*)(klass);
    m=metadata_method_checked(klass,".ctor",0);
    if(m) metadata_invoke(m,obj,0,&exc);
    exc=0;
    m=metadata_method_checked(klass,"BuildItemsAndPopulateDropdown",0);
    if(m) metadata_invoke(m,obj,0,&exc);
    xrow("gate_exc",name,"BuildItemsAndPopulateDropdown",-1);
    metadata_write_signed_decimal(exc?1:0);xend();
    {
        void* arr=metadata_field_object(klass,obj,"_dropdownLabels");
        if(arr) {
            unsigned int len=V014_FN(il2cpp_array_length,unsigned int,void*)(arr);
            unsigned int i;
            xrow("arr",name,"_dropdownLabels",-1);metadata_number(len);xend();
            for(i=0;i<len&&i<160;++i) {
                xrow("item",name,"_dropdownLabels",(int)i);
                xutf16(*(void**)((char*)arr+0x20+8*i));xend();
            }
        }
    }
    {
        void* arr=metadata_field_object(klass,obj,"items");
        if(arr) {
            unsigned int len=V014_FN(il2cpp_array_length,unsigned int,void*)(arr);
            unsigned int i;
            xrow("arr",name,"items",-1);metadata_number(len);xend();
            for(i=0;i<len&&i<160;++i) {
                xrow("item",name,"items",(int)i);
                xutf16(*(void**)((char*)arr+0x20+16*i));xend();
            }
        }
    }
    {
        void* arr=metadata_field_object(klass,obj,"_partTypesByIndex");
        if(arr) {
            unsigned int len=V014_FN(il2cpp_array_length,unsigned int,void*)(arr);
            unsigned int i;
            xrow("arr",name,"_partTypesByIndex",-1);metadata_number(len);xend();
            for(i=0;i<len&&i<160;++i) {
                xrow("item",name,"_partTypesByIndex",(int)i);
                metadata_write_signed_decimal(*(int*)((char*)arr+0x20+4*i));xend();
            }
        }
    }
    xscalar_u32(klass,obj,name,"SpecialModeCount");
    flush_log();
}
static void xplanner(void) {
    static const char* fn[13]={"cellSizeMeters","headingBins","stepMeters",
        "wheelbaseMeters","maxSteerDegrees","steeringSamples","reversePenalty",
        "steerChangePenalty","collisionSamplesPerStep","maxExpansions","maxRuntimeMs",
        "goalPosToleranceMeters","goalYawToleranceDegrees"};
    static const unsigned int fo[13]={0x58,0x5c,0x60,0x64,0x68,0x6c,0x70,0x74,
        0x78,0x7c,0x80,0x84,0x88};
    int i;
    void* klass=xclass("VehiclePlannerService");
    if(!klass) { xrow("missing","VehiclePlannerService","",-1);xend();return; }
    void* inst=metadata_read_static_object_field(klass,"<Instance>k__BackingField");
    xrow("ptr","VehiclePlannerService","Instance",-1);
    metadata_write_unsigned_qword((QWORD)inst);xend();
    if(!inst) { flush_log(); return; }
    for(i=0;i<13;++i) {
        xrow("bits","VehiclePlannerService",fn[i],-1);
        metadata_number(*(unsigned int*)((char*)inst+fo[i]));xend();
    }
    xrow("bits","VehiclePlannerService","hybridGridPaddingMeters",-1);
    metadata_number(*(unsigned int*)((char*)inst+0x24));xend();
    xrow("bits","VehiclePlannerService","hybridGridY",-1);
    metadata_number(*(unsigned int*)((char*)inst+0x38));xend();
    xrow("bits","VehiclePlannerService","hybridObstacleLayers",-1);
    metadata_number(*(unsigned int*)((char*)inst+0x3c));xend();
    xrow("bits","VehiclePlannerService","maxCacheAgeSeconds",-1);
    metadata_number(*(unsigned int*)((char*)inst+0xcc));xend();
    flush_log();
}
static void racing_extras(void) {
    xgate("GetCarPartGate");
    xgate("RacingV2GetFloat");
    xgate("RacingV2GetCarGate");
    xgate("RacingV2GetBoolGate");
    xgate("RacingV2GetWaypointGate");
    xgate("RacingV2WaypointGate");
    xgate("GetCarFromTransform");
    xgate("ModularCarGate");
    xgate("ModularCarInfoGate");
    xplanner();
    xrow("done","racing_extras","",-1);xend();flush_log();
}
