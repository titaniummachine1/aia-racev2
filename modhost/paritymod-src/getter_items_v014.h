/*
 * v0.14 live getter-item capture.
 *
 * Include this file AFTER metadata_probe.h in the same translation unit.  It
 * deliberately uses the metadata/runtime helpers and the resolved IL2CPP API
 * table from that file.  The public entry point is:
 *
 *     getter_items_v014_capture(manager, manager_class)
 *
 * Call it from a post-runtime_invoke main-thread callback for a normal,
 * already-initialized TennisGameManager.  It is read-only: it walks
 * GameManager.graphManagers, admits only the two entries returned by the
 * metadata-resolved GetPlayerForGraph call for HomePlayer/AwayPlayer, then
 * walks those GraphManager.localNodes lists.  It calls only
 * System.Array.GetValue(int) to box array elements and reads ValueTuple
 * Item1/Item2 through metadata fields.  It never invokes a Func delegate,
 * creates an object, calls a Unity lifecycle method, or assumes a tuple/data
 * offset.  A return value of 0 means retry later, 1 means a terminal success,
 * and -1 means a terminal failure.
 */
#ifndef TENNIS_V014_GETTER_ITEMS_H
#define TENNIS_V014_GETTER_ITEMS_H

#define GETTER_ITEMS_V014_RETRY 0
#define GETTER_ITEMS_V014_COMPLETE 1
#define GETTER_ITEMS_V014_FAILED (-1)

#define GETTER_ITEMS_V014_MAX_GRAPHS 64
#define GETTER_ITEMS_V014_MAX_NODES 4096
#define GETTER_ITEMS_V014_MAX_ITEMS 1024

static int getter_items_v014_done;
static int getter_items_v014_failed;
static int getter_items_v014_wait_reported;
static unsigned int getter_items_v014_attempts;
static unsigned int getter_items_v014_graph_count;
static unsigned int getter_items_v014_gate_count;
static unsigned int getter_items_v014_item_count;
static unsigned int getter_items_v014_unresolved_delegate_identities;

static int getter_items_v014_same(const char* left,const char* right) {
    if(!left||!right) return 0;
    while(*left&&*left==*right){++left;++right;}
    return *left==*right;
}

static int getter_items_v014_prefix(const char* text,const char* prefix) {
    if(!text||!prefix) return 0;
    while(*prefix) if(*text++!=*prefix++) return 0;
    return 1;
}

static void getter_items_v014_write_signed(int value) {
    if(value<0) {
        write_text("-");
        metadata_number((unsigned int)(-(value+1))+1u);
    } else metadata_number((unsigned int)value);
}

static void getter_items_v014_write_class(void* klass) {
    if(!klass) { write_text("null"); return; }
    const char* name=V014_FN(il2cpp_class_get_name,const char*,void*)(klass);
    const char* namespaze=V014_FN(il2cpp_class_get_namespace,const char*,void*)(klass);
    write_text("{\"namespace\":");metadata_nullable_string(namespaze);
    write_text(",\"name\":");metadata_nullable_string(name);write_text("}");
}

static void getter_items_v014_write_type(void* type) {
    if(!type) { write_text("null"); return; }
    char* name=V014_FN(il2cpp_type_get_name,char*,void*)(type);
    metadata_nullable_string(name);
    if(name) V014_FN(il2cpp_free,void,void*)(name);
}

static int getter_items_v014_type_is(void* type,const char* expected) {
    if(!type||!expected) return 0;
    char* name=V014_FN(il2cpp_type_get_name,char*,void*)(type);
    int result=name&&getter_items_v014_same(name,expected);
    if(name) V014_FN(il2cpp_free,void,void*)(name);
    return result;
}

static void getter_items_v014_write_u16_escape(WORD value) {
    const char* digits="0123456789abcdef";
    char escaped[7]={'\\','u',digits[(value>>12)&15],digits[(value>>8)&15],
                     digits[(value>>4)&15],digits[value&15],0};
    write_text(escaped);
}

/* Unity strings are UTF-16.  Preserve every code unit as JSON text (including
 * non-ASCII labels) so distinct runtime labels cannot collide in evidence. */
static void getter_items_v014_write_managed_string(void* value) {
    if(!value) { write_text("null"); return; }
    int length=V014_FN(il2cpp_string_length,int,void*)(value);
    const WORD* chars=V014_FN(il2cpp_string_chars,const WORD*,void*)(value);
    if(length<0||length>4096||!chars) { write_text("null"); return; }
    write_text("\"");
    for(int i=0;i<length;++i) {
        WORD ch=chars[i];
        if(ch=='"'||ch=='\\') {
            char escaped[3]={'\\',(char)ch,0};write_text(escaped);
        } else if(ch=='\n') write_text("\\n");
        else if(ch=='\r') write_text("\\r");
        else if(ch=='\t') write_text("\\t");
        else if(ch>=32&&ch<128) { char out[2]={(char)ch,0};write_text(out); }
        else getter_items_v014_write_u16_escape(ch);
    }
    write_text("\"");
}

static void* getter_items_v014_field_object(void* klass,void* object,
                                            const char* name,int* found) {
    void* field=metadata_find_field(klass,name);void* value=0;
    if(found) *found=field&&object;
    if(field&&object)
        V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,field,&value);
    return value;
}

static int getter_items_v014_read_i32(void* klass,void* object,const char* name,
                                      int* result) {
    void* field=metadata_find_field(klass,name);
    if(!field||!object||!result||
       !getter_items_v014_type_is(V014_FN(il2cpp_field_get_type,void*,void*)(field),
                                  "System.Int32")) return 0;
    *result=0;
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,field,result);
    return 1;
}

static int getter_items_v014_read_bool(void* klass,void* object,const char* name,
                                       int* result) {
    void* field=metadata_find_field(klass,name);unsigned int value=0;
    if(!field||!object||!result||
       !getter_items_v014_type_is(V014_FN(il2cpp_field_get_type,void*,void*)(field),
                                  "System.Boolean")) return 0;
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(object,field,&value);
    *result=value?1:0;
    return 1;
}

static void getter_items_v014_write_object_identity(void* object) {
    if(!object) { write_text("null"); return; }
    write_text("{\"object\":");metadata_object_pointer(object);
    write_text(",\"class\":");
    getter_items_v014_write_class(V014_FN(il2cpp_object_get_class,void*,void*)(object));
    write_text("}");
}

/* Walk the class and its parents so inherited System.Array/Delegate methods
 * are resolved by metadata rather than by a native address or vtable slot. */
static void* getter_items_v014_find_method(void* klass,const char* name,
                                           unsigned int parameter_count) {
    for(unsigned int depth=0;klass&&depth<12;++depth) {
        void* iterator=0;void* method=0;
        while((method=V014_FN(il2cpp_class_get_methods,void*,void*,void**)
                              (klass,&iterator))) {
            const char* method_name=V014_FN(il2cpp_method_get_name,const char*,const void*)
                (method);
            if(getter_items_v014_same(method_name,name)&&
               V014_FN(il2cpp_method_get_param_count,unsigned int,const void*)(method)==
                   parameter_count) return method;
        }
        klass=V014_FN(il2cpp_class_get_parent,void*,void*)(klass);
    }
    return 0;
}

static void* getter_items_v014_find_array_get_value(void* array_class) {
    for(unsigned int depth=0;array_class&&depth<12;++depth) {
        void* iterator=0;void* method=0;
        while((method=V014_FN(il2cpp_class_get_methods,void*,void*,void**)
                              (array_class,&iterator))) {
            const char* name=V014_FN(il2cpp_method_get_name,const char*,const void*)
                (method);
            if(!getter_items_v014_same(name,"GetValue")||
               V014_FN(il2cpp_method_get_param_count,unsigned int,const void*)(method)!=1)
                continue;
            void* parameter=V014_FN(il2cpp_method_get_param,void*,const void*,unsigned int)
                (method,0);
            void* return_type=V014_FN(il2cpp_method_get_return_type,void*,const void*)
                (method);
            if(getter_items_v014_type_is(parameter,"System.Int32")&&
               getter_items_v014_type_is(return_type,"System.Object")) return method;
        }
        array_class=V014_FN(il2cpp_class_get_parent,void*,void*)(array_class);
    }
    return 0;
}

static void* getter_items_v014_array_get_value(void* method,void* array,int index,
                                               void** exception) {
    int argument=index;void* args[]={&argument};
    return metadata_invoke(method,array,args,exception);
}

typedef struct {
    void* list;
    void* list_class;
    void* items;
    void* array_class;
    void* element_class;
    int size;
    unsigned int array_length;
    unsigned int array_bytes;
    unsigned int header_size;
    int element_size;
} getter_items_v014_list_view;

/* Return 1 for a validated list, 0 when Unity is still hydrating it, and -1
 * for a type/layout mismatch.  No list field offset is embedded here. */
static int getter_items_v014_read_list(void* list,const char* expected_namespace,
                                       const char* expected_name,
                                       getter_items_v014_list_view* view,
                                       const char** reason) {
    if(reason) *reason="list_unavailable";
    if(!list||!view) return 0;
    view->list=list;view->list_class=0;view->items=0;view->array_class=0;
    view->element_class=0;view->size=-1;view->array_length=0;view->array_bytes=0;
    view->header_size=0;view->element_size=0;
    void* list_class=V014_FN(il2cpp_object_get_class,void*,void*)(list);
    const char* list_namespace=list_class?
        V014_FN(il2cpp_class_get_namespace,const char*,void*)(list_class):0;
    const char* list_name=list_class?
        V014_FN(il2cpp_class_get_name,const char*,void*)(list_class):0;
    if(!list_class||!getter_items_v014_same(list_namespace,"System.Collections.Generic")||
       !getter_items_v014_prefix(list_name,"List")) {
        if(reason) *reason="list_runtime_class";return -1;
    }
    void* size_field=metadata_find_field(list_class,"_size");
    void* items_field=metadata_find_field(list_class,"_items");
    if(!size_field||!items_field) { if(reason) *reason="list_fields";return -1; }
    if(!getter_items_v014_type_is(V014_FN(il2cpp_field_get_type,void*,void*)(size_field),
                                  "System.Int32")) {
        if(reason) *reason="list_size_type";return -1;
    }
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(list,size_field,&view->size);
    if(view->size<0) { if(reason) *reason="list_size_negative";return -1; }
    if(view->size>(int)GETTER_ITEMS_V014_MAX_NODES) {
        if(reason) *reason="list_size_over_budget";return -1;
    }
    V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(list,items_field,&view->items);
    if(!view->items) { if(reason) *reason="list_items_null";return 0; }
    void* declared_type=V014_FN(il2cpp_field_get_type,void*,void*)(items_field);
    void* declared_array=declared_type?
        V014_FN(il2cpp_class_from_type,void*,void*)(declared_type):0;
    void* declared_element=declared_array?
        V014_FN(il2cpp_class_get_element_class,void*,void*)(declared_array):0;
    int declared_rank=declared_array?
        V014_FN(il2cpp_class_get_rank,int,void*)(declared_array):0;
    const char* declared_namespace=declared_element?
        V014_FN(il2cpp_class_get_namespace,const char*,void*)(declared_element):0;
    const char* declared_name=declared_element?
        V014_FN(il2cpp_class_get_name,const char*,void*)(declared_element):0;
    if(!declared_array||!declared_element||declared_rank!=1||
       !getter_items_v014_same(declared_namespace,expected_namespace)||
       !getter_items_v014_same(declared_name,expected_name)) {
        if(reason) *reason="list_declared_element_type";return -1;
    }
    void* runtime_array_class=V014_FN(il2cpp_object_get_class,void*,void*)(view->items);
    void* runtime_element=runtime_array_class?
        V014_FN(il2cpp_class_get_element_class,void*,void*)(runtime_array_class):0;
    const char* runtime_namespace=runtime_element?
        V014_FN(il2cpp_class_get_namespace,const char*,void*)(runtime_element):0;
    const char* runtime_name=runtime_element?
        V014_FN(il2cpp_class_get_name,const char*,void*)(runtime_element):0;
    view->array_length=V014_FN(il2cpp_array_length,unsigned int,void*)(view->items);
    view->array_bytes=V014_FN(il2cpp_array_get_byte_length,unsigned int,void*)(view->items);
    view->header_size=V014_FN(il2cpp_array_object_header_size,unsigned int,void)();
    view->element_size=runtime_array_class?
        V014_FN(il2cpp_array_element_size,int,void*)(runtime_array_class):0;
    QWORD payload=(QWORD)view->array_length*(QWORD)(view->element_size>0?
                                                       view->element_size:0);
    if(!runtime_array_class||!runtime_element||
       !getter_items_v014_same(runtime_namespace,expected_namespace)||
       !getter_items_v014_same(runtime_name,expected_name)||
       view->header_size==0||view->element_size!=(int)sizeof(void*)||
       payload!=(QWORD)view->array_bytes||view->array_length<(unsigned int)view->size) {
        if(reason) *reason="list_runtime_array_layout";return -1;
    }
    view->list_class=list_class;view->array_class=runtime_array_class;
    view->element_class=runtime_element;
    return 1;
}

static int getter_items_v014_write_delegate_identity(void* delegate);

static int getter_items_v014_read_gate_items(void* gate,void* gate_class,
                                             int graph_index,int node_index,
                                             const char* gate_name) {
    void* field=metadata_find_field(gate_class,"items");
    if(!field) {
        write_text("{\"kind\":\"v014_getter_gate\",\"status\":\"failed\",\"reason\":\"items_field_missing\"}\n");
        return -1;
    }
    void* declared_type=V014_FN(il2cpp_field_get_type,void*,void*)(field);
    void* declared_array=declared_type?
        V014_FN(il2cpp_class_from_type,void*,void*)(declared_type):0;
    void* declared_element=declared_array?
        V014_FN(il2cpp_class_get_element_class,void*,void*)(declared_array):0;
    int rank=declared_array?V014_FN(il2cpp_class_get_rank,int,void*)(declared_array):0;
    void* items=0;V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(gate,field,&items);
    if(!items) return 0;
    void* runtime_array_class=V014_FN(il2cpp_object_get_class,void*,void*)(items);
    unsigned int length=V014_FN(il2cpp_array_length,unsigned int,void*)(items);
    unsigned int bytes=V014_FN(il2cpp_array_get_byte_length,unsigned int,void*)(items);
    unsigned int header=V014_FN(il2cpp_array_object_header_size,unsigned int,void)();
    int element_size=runtime_array_class?
        V014_FN(il2cpp_array_element_size,int,void*)(runtime_array_class):0;
    QWORD payload=(QWORD)length*(QWORD)(element_size>0?element_size:0);
    if(!declared_array||!declared_element||rank!=1||!runtime_array_class||
       !header||payload!=(QWORD)bytes||length>GETTER_ITEMS_V014_MAX_ITEMS) {
        write_text("{\"kind\":\"v014_getter_gate\",\"status\":\"failed\",\"reason\":\"items_array_layout\",\"gate\":");metadata_quoted(gate_name);write_text("}\n");
        return -1;
    }
    void* get_value=getter_items_v014_find_array_get_value(runtime_array_class);
    if(!get_value) {
        write_text("{\"kind\":\"v014_getter_gate\",\"status\":\"failed\",\"reason\":\"array_get_value_int_missing\",\"gate\":");metadata_quoted(gate_name);write_text("}\n");
        return -1;
    }
    int option_index=-1;
    int option_ok=getter_items_v014_read_i32(gate_class,gate,"optionIndex",&option_index);
    void* runtime_element=V014_FN(il2cpp_class_get_element_class,void*,void*)(runtime_array_class);
    int found_gate_graph=0;void* gate_graph=getter_items_v014_field_object(
        gate_class,gate,"graphManager",&found_gate_graph);
    int found_gate_manager=0;void* gate_manager=getter_items_v014_field_object(
        gate_class,gate,"gameManager",&found_gate_manager);
    write_text("{\"kind\":\"v014_getter_gate\",\"schema\":\"v014-getter-items-v1\",\"graph_index\":");metadata_number((unsigned int)graph_index);
    write_text(",\"node_index\":");metadata_number((unsigned int)node_index);
    write_text(",\"gate\":");metadata_quoted(gate_name);
    write_text(",\"object\":");metadata_object_pointer(gate);
    write_text(",\"declared_items_type\":");getter_items_v014_write_type(declared_type);
    write_text(",\"runtime_element_class\":");getter_items_v014_write_class(runtime_element);
    write_text(",\"array_rank\":");metadata_number((unsigned int)rank);
    write_text(",\"array_length\":");metadata_number(length);
    write_text(",\"array_byte_length\":");metadata_number(bytes);
    write_text(",\"array_header_size\":");metadata_number(header);
    write_text(",\"array_element_size\":");getter_items_v014_write_signed(element_size);
    write_text(",\"option_index\":");if(option_ok)getter_items_v014_write_signed(option_index);else write_text("null");
    write_text(",\"graph_manager\":");if(found_gate_graph)metadata_object_pointer(gate_graph);else write_text("null");
    write_text(",\"game_manager\":");if(found_gate_manager)metadata_object_pointer(gate_manager);else write_text("null");
    write_text(",\"items\":[");
    int decode_ok=option_ok&&found_gate_graph&&found_gate_manager;void* selected_label=0;
    int identity_complete=1;
    for(unsigned int index=0;index<length;++index) {
        if(index) write_text(",");
        void* exception=0;void* boxed=getter_items_v014_array_get_value(get_value,items,(int)index,&exception);
        write_text("{\"index\":");metadata_number(index);
        if(exception||!boxed) {
            write_text(",\"status\":\"boxed_tuple_unavailable\",\"exception\":");if(exception)write_text("true");else write_text("false");write_text("}");
            decode_ok=0;continue;
        }
        void* tuple_class=V014_FN(il2cpp_object_get_class,void*,void*)(boxed);
        void* label_field=metadata_find_field(tuple_class,"Item1");
        void* delegate_field=metadata_find_field(tuple_class,"Item2");
        void* label=0;void* delegate=0;
        if(label_field)V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(boxed,label_field,&label);
        if(delegate_field)V014_FN(il2cpp_field_get_value,void,void*,void*,void*)(boxed,delegate_field,&delegate);
        write_text(",\"tuple_class\":");getter_items_v014_write_class(tuple_class);
        write_text(",\"tuple_object\":");metadata_object_pointer(boxed);
        write_text(",\"label\":");getter_items_v014_write_managed_string(label);
        write_text(",\"delegate\":");
        int delegate_identity=getter_items_v014_write_delegate_identity(delegate);
        if(!label||!delegate||!label_field||!delegate_field) decode_ok=0;
        if(!delegate_identity) identity_complete=0;
        if((int)index==option_index) selected_label=label;
        write_text("}");
        ++getter_items_v014_item_count;
    }
    write_text("],\"selected_label\":");
    if(option_ok&&option_index>=0&&(unsigned int)option_index<length)
        getter_items_v014_write_managed_string(selected_label);
    else write_text("null");
    write_text(",\"identity_complete\":");if(identity_complete)write_text("true");else write_text("false");
    write_text(",\"decode_status\":\"");if(decode_ok)write_text("ok");else write_text("partial");write_text("\"}\n");
    if(!identity_complete) ++getter_items_v014_unresolved_delegate_identities;
    ++getter_items_v014_gate_count;
    return decode_ok?1:-1;
}

static int getter_items_v014_write_delegate_identity(void* delegate) {
    write_text("{\"object\":");metadata_object_pointer(delegate);
    if(!delegate) { write_text(",\"status\":\"null\"}");return 0; }
    void* delegate_class=V014_FN(il2cpp_object_get_class,void*,void*)(delegate);
    write_text(",\"class\":");getter_items_v014_write_class(delegate_class);
    int complete=1;
    void* target_method=getter_items_v014_find_method(delegate_class,"get_Target",0);
    void* exception=0;void* target=target_method?
        metadata_invoke(target_method,delegate,0,&exception):0;
    write_text(",\"target\":");
    if(!target_method||exception) { write_text("null");complete=0; }
    else getter_items_v014_write_object_identity(target);
    void* method_method=getter_items_v014_find_method(delegate_class,"get_Method",0);
    exception=0;void* method_object=method_method?
        metadata_invoke(method_method,delegate,0,&exception):0;
    write_text(",\"method\":");
    if(!method_method||exception||!method_object) {
        write_text("{\"status\":\"unresolved\"}");complete=0;
    } else {
        void* method_class=V014_FN(il2cpp_object_get_class,void*,void*)(method_object);
        write_text("{\"object\":");metadata_object_pointer(method_object);
        write_text(",\"class\":");getter_items_v014_write_class(method_class);
        void* name_method=getter_items_v014_find_method(method_class,"get_Name",0);
        exception=0;void* name=name_method?metadata_invoke(name_method,method_object,0,&exception):0;
        write_text(",\"name\":");
        if(!name_method||exception||!name) { write_text("null");complete=0; }
        else getter_items_v014_write_managed_string(name);
        void* declaring_method=getter_items_v014_find_method(method_class,"get_DeclaringType",0);
        exception=0;void* declaring=declaring_method?
            metadata_invoke(declaring_method,method_object,0,&exception):0;
        write_text(",\"declaring_type\":");
        if(!declaring_method||exception||!declaring) { write_text("null");complete=0; }
        else {
            write_text("{\"object\":");metadata_object_pointer(declaring);
            write_text(",\"class\":");getter_items_v014_write_class(
                V014_FN(il2cpp_object_get_class,void*,void*)(declaring));
            void* declaring_class=V014_FN(il2cpp_object_get_class,void*,void*)(declaring);
            void* full_name_method=getter_items_v014_find_method(declaring_class,"get_FullName",0);
            exception=0;void* full_name=full_name_method?
                metadata_invoke(full_name_method,declaring,0,&exception):0;
            write_text(",\"full_name\":");
            if(!full_name_method||exception||!full_name) { write_text("null");complete=0; }
            else getter_items_v014_write_managed_string(full_name);
            write_text("}");
        }
        write_text("}");
    }
    write_text(",\"identity_status\":\"");if(complete)write_text("complete");else write_text("unresolved");write_text("\"}");
    return complete;
}

static int getter_items_v014_read_auto_swing(void* gate,void* gate_class,
                                             int graph_index,int node_index) {
    int mode=0,prefer=0,pending=0;
    int mode_ok=getter_items_v014_read_i32(gate_class,gate,"modeIndex",&mode);
    int prefer_ok=getter_items_v014_read_bool(gate_class,gate,"randomPreferCharge",&prefer);
    int pending_ok=getter_items_v014_read_bool(gate_class,gate,"randomRollPending",&pending);
    int found_dropdown=0;void* dropdown=getter_items_v014_field_object(
        gate_class,gate,"modeDropdown",&found_dropdown);
    int found_graph=0;void* graph=getter_items_v014_field_object(
        gate_class,gate,"graphManager",&found_graph);
    int found_manager=0;void* manager=getter_items_v014_field_object(
        gate_class,gate,"gameManager",&found_manager);
    write_text("{\"kind\":\"v014_auto_swing_state\",\"schema\":\"v014-auto-swing-state-v1\",\"graph_index\":");metadata_number((unsigned int)graph_index);
    write_text(",\"node_index\":");metadata_number((unsigned int)node_index);
    write_text(",\"object\":");metadata_object_pointer(gate);
    write_text(",\"mode_index\":");if(mode_ok)getter_items_v014_write_signed(mode);else write_text("null");
    write_text(",\"random_prefer_charge\":");if(prefer_ok)metadata_number((unsigned int)prefer);else write_text("null");
    write_text(",\"random_roll_pending\":");if(pending_ok)metadata_number((unsigned int)pending);else write_text("null");
    write_text(",\"mode_dropdown\":");if(found_dropdown)metadata_object_pointer(dropdown);else write_text("null");
    write_text(",\"graph_manager\":");if(found_graph)metadata_object_pointer(graph);else write_text("null");
    write_text(",\"game_manager\":");if(found_manager)metadata_object_pointer(manager);else write_text("null");
    write_text(",\"status\":\"");
    if(mode_ok&&prefer_ok&&pending_ok&&found_dropdown&&found_graph&&found_manager)write_text("ok");
    else write_text("partial");
    write_text("\"}\n");
    return mode_ok&&prefer_ok&&pending_ok&&found_dropdown&&found_graph&&found_manager?1:-1;
}

static const char* getter_items_v014_gate_name(void* klass) {
    const char* name=klass?V014_FN(il2cpp_class_get_name,const char*,void*)(klass):0;
    if(getter_items_v014_same(name,"TennisGetBoolGate"))return "TennisGetBoolGate";
    if(getter_items_v014_same(name,"TennisGetFloatGate"))return "TennisGetFloatGate";
    if(getter_items_v014_same(name,"TennisGetTransformGate"))return "TennisGetTransformGate";
    if(getter_items_v014_same(name,"TennisGetVector3Gate"))return "TennisGetVector3Gate";
    if(getter_items_v014_same(name,"TennisAutoSwingGate"))return "TennisAutoSwingGate";
    return 0;
}

/* GraphManager.localNodes intentionally stores the serializable Node base,
 * not the MonoBehaviour gate component.  Emit its metadata-defined fields
 * once so the next capture can follow the actual node-to-gate reference by
 * name rather than treating the base object as one of the gate subclasses. */
static void getter_items_v014_write_node_layout(void* klass) {
    write_text("{\"kind\":\"v014_getter_node_layout\",\"schema\":\"v014-getter-items-v1\",\"classes\":[");
    int first_class=1;
    for(unsigned int depth=0;klass&&depth<8;++depth) {
        if(!first_class)write_text(",");first_class=0;
        write_text("{\"class\":");getter_items_v014_write_class(klass);
        write_text(",\"fields\":[");int first=1;void* iter=0;void* field=0;
        while((field=V014_FN(il2cpp_class_get_fields,void*,void*,void**)(klass,&iter))) {
            if(!first)write_text(",");first=0;
            write_text("{\"name\":");metadata_nullable_string(
                V014_FN(il2cpp_field_get_name,const char*,void*)(field));
            write_text(",\"type\":");metadata_type_name(
                V014_FN(il2cpp_field_get_type,void*,void*)(field));
            /* Keep the layout record strict JSON.  The metadata probe's
             * write_hex helper is intended for diagnostic text and emits a
             * 0x-prefixed token, which JSON does not permit as a number. */
            write_text(",\"flags\":");metadata_number(V014_FN(
                il2cpp_field_get_flags,unsigned int,void*)(field));write_text("}");
        }
        write_text("]}");
        klass=V014_FN(il2cpp_class_get_parent,void*,void*)(klass);
    }
    write_text("]}\n");flush_log();
}

static int getter_items_v014_apis_ready(void) {
    return V014_FN(il2cpp_object_get_class,void*)&&
        V014_FN(il2cpp_class_get_name,void*)&&V014_FN(il2cpp_class_get_namespace,void*)&&
        V014_FN(il2cpp_class_get_parent,void*)&&V014_FN(il2cpp_class_get_methods,void*)&&
        V014_FN(il2cpp_method_get_name,void*)&&V014_FN(il2cpp_method_get_param_count,void*)&&
        V014_FN(il2cpp_method_get_param,void*)&&V014_FN(il2cpp_method_get_return_type,void*)&&
        V014_FN(il2cpp_class_get_element_class,void*)&&V014_FN(il2cpp_class_get_rank,void*)&&
        V014_FN(il2cpp_class_from_type,void*)&&V014_FN(il2cpp_field_get_type,void*)&&
        V014_FN(il2cpp_field_get_value,void*)&&V014_FN(il2cpp_array_length,void*)&&
        V014_FN(il2cpp_array_get_byte_length,void*)&&V014_FN(il2cpp_array_element_size,void*)&&
        V014_FN(il2cpp_array_object_header_size,void*)&&V014_FN(il2cpp_runtime_invoke,void*)&&
        V014_FN(il2cpp_object_unbox,void*)&&V014_FN(il2cpp_string_length,void*)&&
        V014_FN(il2cpp_string_chars,void*)&&V014_FN(il2cpp_type_get_name,void*)&&
        V014_FN(il2cpp_free,void*);
}

static void getter_items_v014_write_wait(const char* reason) {
    if(getter_items_v014_wait_reported) return;
    write_text("{\"kind\":\"v014_getter_items_status\",\"schema\":\"v014-getter-items-v1\",\"status\":\"waiting\",\"reason\":");metadata_quoted(reason?reason:"not_ready");write_text("}\n");
    flush_log();getter_items_v014_wait_reported=1;
}

static int getter_items_v014_capture(void* manager,void* manager_class) {
    if(getter_items_v014_done) return getter_items_v014_failed?
        GETTER_ITEMS_V014_FAILED:GETTER_ITEMS_V014_COMPLETE;
    ++getter_items_v014_attempts;
    if(!getter_items_v014_apis_ready()) {
        write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":\"required_api_missing\"}\n");
        flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;
        return GETTER_ITEMS_V014_FAILED;
    }
    if(!manager||!manager_class) { getter_items_v014_write_wait("manager_unavailable");return GETTER_ITEMS_V014_RETRY; }
    int found_graphs=0;void* graph_list=getter_items_v014_field_object(
        manager_class,manager,"graphManagers",&found_graphs);
    if(!found_graphs||!graph_list) { getter_items_v014_write_wait("graphManagers_unavailable");return GETTER_ITEMS_V014_RETRY; }
    getter_items_v014_list_view graph_view;const char* reason=0;
    int list_status=getter_items_v014_read_list(
        graph_list,"MeadowGames.UINodeConnect4","GraphManager",&graph_view,&reason);
    if(list_status==0) { getter_items_v014_write_wait(reason);return GETTER_ITEMS_V014_RETRY; }
    if(list_status<0) {
        write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":");metadata_quoted(reason?reason:"graph_list_invalid");write_text("}\n");
        flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;return GETTER_ITEMS_V014_FAILED;
    }
    if(graph_view.size<=0||graph_view.size>(int)GETTER_ITEMS_V014_MAX_GRAPHS) {
        getter_items_v014_write_wait(graph_view.size==0?"graphManagers_empty":"graphManagers_over_budget");return GETTER_ITEMS_V014_RETRY;
    }
    int found_home=0;int found_away=0;
    void* home_player=getter_items_v014_field_object(
        manager_class,manager,"<HomePlayer>k__BackingField",&found_home);
    void* away_player=getter_items_v014_field_object(
        manager_class,manager,"<AwayPlayer>k__BackingField",&found_away);
    if(!found_home||!found_away||!home_player||!away_player||home_player==away_player) {
        getter_items_v014_write_wait("owned_players_unavailable");return GETTER_ITEMS_V014_RETRY;
    }
    void* get_player_for_graph=getter_items_v014_find_method(
        manager_class,"GetPlayerForGraph",1);
    if(!get_player_for_graph) {
        write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":\"owned_graph_method_missing\"}\n");
        flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;return GETTER_ITEMS_V014_FAILED;
    }
    void* graphs[GETTER_ITEMS_V014_MAX_GRAPHS];void* graph_classes[GETTER_ITEMS_V014_MAX_GRAPHS];
    int source_graph_indices[GETTER_ITEMS_V014_MAX_GRAPHS];int graph_player_slot[GETTER_ITEMS_V014_MAX_GRAPHS];
    int graph_ready[GETTER_ITEMS_V014_MAX_GRAPHS];void* node_lists[GETTER_ITEMS_V014_MAX_GRAPHS];
    getter_items_v014_list_view node_views[GETTER_ITEMS_V014_MAX_GRAPHS];
    void* graph_get_value=getter_items_v014_find_array_get_value(graph_view.array_class);
    if(!graph_get_value) {
        write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":\"graph_array_get_value_int_missing\"}\n");
        flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;return GETTER_ITEMS_V014_FAILED;
    }
    int graph_count=0;int home_seen=0;int away_seen=0;
    for(int source_index=0;source_index<graph_view.size;++source_index) {
        void* exception=0;void* candidate=getter_items_v014_array_get_value(
            graph_get_value,graph_view.items,source_index,&exception);
        if(exception||!candidate) { getter_items_v014_write_wait("graph_element_unavailable");return GETTER_ITEMS_V014_RETRY; }
        void* candidate_class=V014_FN(il2cpp_object_get_class,void*,void*)(candidate);
        const char* ns=V014_FN(il2cpp_class_get_namespace,const char*,void*)(candidate_class);
        const char* name=V014_FN(il2cpp_class_get_name,const char*,void*)(candidate_class);
        if(!getter_items_v014_same(ns,"MeadowGames.UINodeConnect4")||
           !getter_items_v014_same(name,"GraphManager")) {
            write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":\"graph_element_type\"}\n");
            flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;return GETTER_ITEMS_V014_FAILED;
        }
        void* graph_args[]={candidate};exception=0;
        void* player=metadata_invoke(get_player_for_graph,manager,graph_args,&exception);
        if(exception) { getter_items_v014_write_wait("owned_graph_lookup_exception");return GETTER_ITEMS_V014_RETRY; }
        int player_slot=player==home_player?0:(player==away_player?1:-1);
        if(player_slot<0) continue;
        int duplicate=0;
        for(int prior=0;prior<graph_count;++prior) if(graphs[prior]==candidate) duplicate=1;
        if(duplicate) continue;
        if(graph_count>=GETTER_ITEMS_V014_MAX_GRAPHS) {
            write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":\"owned_graphs_over_budget\"}\n");
            flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;return GETTER_ITEMS_V014_FAILED;
        }
        graphs[graph_count]=candidate;graph_classes[graph_count]=candidate_class;
        source_graph_indices[graph_count]=source_index;graph_player_slot[graph_count]=player_slot;
        if(player_slot==0) home_seen=1;else away_seen=1;
        graph_ready[graph_count]=0;
        if(!getter_items_v014_read_bool(graph_classes[graph_count],graphs[graph_count],"<IsSimGraphReady>k__BackingField",&graph_ready[graph_count])) {
            write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":\"graph_ready_field\"}\n");
            flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;return GETTER_ITEMS_V014_FAILED;
        }
        int found_nodes=0;node_lists[graph_count]=getter_items_v014_field_object(
            graph_classes[graph_count],graphs[graph_count],"localNodes",&found_nodes);
        if(!found_nodes||!node_lists[graph_count]) { getter_items_v014_write_wait("localNodes_unavailable");return GETTER_ITEMS_V014_RETRY; }
        if(!graph_ready[graph_count]) { getter_items_v014_write_wait("graph_not_ready");return GETTER_ITEMS_V014_RETRY; }
        const char* node_reason=0;
        int node_status=getter_items_v014_read_list(node_lists[graph_count],"","Node",
                                                    &node_views[graph_count],&node_reason);
        if(node_status==0) { getter_items_v014_write_wait(node_reason);return GETTER_ITEMS_V014_RETRY; }
        if(node_status<0) {
            write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"failed\",\"reason\":");metadata_quoted(node_reason?node_reason:"node_list_invalid");write_text("}\n");
            flush_log();getter_items_v014_done=1;getter_items_v014_failed=1;return GETTER_ITEMS_V014_FAILED;
        }
        ++graph_count;
    }
    if(graph_count!=2||!home_seen||!away_seen) {
        getter_items_v014_write_wait("owned_graphs_not_ready");return GETTER_ITEMS_V014_RETRY;
    }
    getter_items_v014_graph_count=(unsigned int)graph_count;
    /* The preflight above establishes that this is a stable graph snapshot.
     * Keep terminal counts local to the successful attempt; a gate whose
     * coroutine has not populated `items` is retriable below. */
    getter_items_v014_gate_count=0;
    getter_items_v014_item_count=0;
    getter_items_v014_unresolved_delegate_identities=0;
    write_text("{\"kind\":\"v014_getter_items_begin\",\"schema\":\"v014-getter-items-v1\",\"status\":\"capturing\",\"attempt\":");metadata_number(getter_items_v014_attempts);
    write_text(",\"graph_count\":");metadata_number(getter_items_v014_graph_count);write_text("}\n");
    int all_ok=1;int retry_needed=0;
    if(graph_count>0) {
        void* layout_class=V014_FN(il2cpp_object_get_class,void*,void*)(
            getter_items_v014_array_get_value(
                getter_items_v014_find_array_get_value(node_views[0].array_class),
                node_views[0].items,0,0));
        if(layout_class) getter_items_v014_write_node_layout(layout_class);
    }
    for(int graph_index=0;graph_index<graph_count;++graph_index) {
        void* brain= getter_items_v014_field_object(graph_classes[graph_index],graphs[graph_index],"Brain",0);
        write_text("{\"kind\":\"v014_getter_graph\",\"schema\":\"v014-getter-items-v1\",\"graph_index\":");metadata_number((unsigned int)graph_index);
        write_text(",\"source_graph_index\":");metadata_number((unsigned int)source_graph_indices[graph_index]);
        write_text(",\"player_slot\":");metadata_quoted(graph_player_slot[graph_index]==0?"home":"away");
        write_text(",\"object\":");metadata_object_pointer(graphs[graph_index]);
        write_text(",\"ready\":");if(graph_ready[graph_index])write_text("true");else write_text("false");
        write_text(",\"brain\":");metadata_object_pointer(brain);
        write_text(",\"node_count\":");metadata_number((unsigned int)node_views[graph_index].size);write_text("}\n");
        void* node_get_value=getter_items_v014_find_array_get_value(node_views[graph_index].array_class);
        if(!node_get_value) { all_ok=0;continue; }
        for(int node_index=0;node_index<node_views[graph_index].size;++node_index) {
            void* exception=0;void* node=getter_items_v014_array_get_value(
                node_get_value,node_views[graph_index].items,node_index,&exception);
            write_text("{\"kind\":\"v014_getter_node\",\"schema\":\"v014-getter-items-v1\",\"graph_index\":");
            metadata_number((unsigned int)graph_index);write_text(",\"node_index\":");
            metadata_number((unsigned int)node_index);write_text(",\"object\":");
            metadata_object_pointer(node);write_text(",\"exception\":");
            if(exception)write_text("true");else write_text("false");
            if(node) {
                void* raw_class=V014_FN(il2cpp_object_get_class,void*,void*)(node);
                write_text(",\"class\":");getter_items_v014_write_class(raw_class);
            } else write_text(",\"class\":null");
            write_text("}\n");
            if(exception||!node) { all_ok=0;continue; }
            void* node_class=V014_FN(il2cpp_object_get_class,void*,void*)(node);
            int found_gate=0;
            void* gate=getter_items_v014_field_object(node_class,node,"cachedGate",&found_gate);
            void* gate_class=gate?V014_FN(il2cpp_object_get_class,void*,void*)(gate):0;
            const char* gate_name=getter_items_v014_gate_name(gate_class);
            write_text("{\"kind\":\"v014_getter_node_gate\",\"schema\":\"v014-getter-items-v1\",\"graph_index\":");
            metadata_number((unsigned int)graph_index);write_text(",\"node_index\":");
            metadata_number((unsigned int)node_index);write_text(",\"node\":");metadata_object_pointer(node);
            write_text(",\"cached_gate_field_found\":");if(found_gate)write_text("true");else write_text("false");
            write_text(",\"gate\":");metadata_object_pointer(gate);write_text(",\"gate_class\":");
            if(gate_class)getter_items_v014_write_class(gate_class);else write_text("null");
            write_text("}\n");
            if(!gate_name) continue;
            if(getter_items_v014_same(gate_name,"TennisAutoSwingGate")) {
                if(getter_items_v014_read_auto_swing(gate,gate_class,graph_index,node_index)<0) all_ok=0;
            } else {
                int gate_status=getter_items_v014_read_gate_items(
                    gate,gate_class,graph_index,node_index,gate_name);
                if(gate_status==0) { all_ok=0;retry_needed=1; }
                else if(gate_status<0) all_ok=0;
            }
        }
    }
    if(retry_needed) {
        getter_items_v014_write_wait("gate_items_unavailable");
        return GETTER_ITEMS_V014_RETRY;
    }
    write_text("{\"kind\":\"v014_getter_items_complete\",\"schema\":\"v014-getter-items-v1\",\"status\":\"");
    if(all_ok)write_text("complete");else write_text("failed");
    write_text("\",\"graph_count\":");metadata_number(getter_items_v014_graph_count);
    write_text(",\"gate_count\":");metadata_number(getter_items_v014_gate_count);
    write_text(",\"item_count\":");metadata_number(getter_items_v014_item_count);
    write_text(",\"unresolved_delegate_identities\":");metadata_number(getter_items_v014_unresolved_delegate_identities);
    write_text("}\n");flush_log();
    getter_items_v014_done=1;getter_items_v014_failed=all_ok?0:1;
    return all_ok?GETTER_ITEMS_V014_COMPLETE:GETTER_ITEMS_V014_FAILED;
}

#endif
