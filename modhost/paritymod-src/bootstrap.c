/*
 * v0.14-only minimal Windows x64 Unity launcher.
 *
 * This source intentionally has no runtime/verification macros and no
 * v0.12-specific API indices or call-site RVAs.  It observes only the
 * GameAssembly GetProcAddress stream and flushes every row immediately so a
 * bootstrap capture can be stopped after resolution without losing its log.
 */
typedef unsigned long DWORD;
int _fltused=0;
typedef unsigned short WORD;
typedef unsigned short wchar_t;
typedef unsigned char BYTE;
typedef unsigned long long QWORD;
typedef void* HANDLE;
typedef void* MODULE;
typedef int BOOL;
typedef void* PROC;
#define API __declspec(dllimport)
API MODULE __stdcall LoadLibraryW(const wchar_t*);
API PROC __stdcall GetProcAddress(MODULE,const char*);
API MODULE __stdcall GetModuleHandleW(const wchar_t*);
API DWORD __stdcall GetModuleFileNameW(MODULE,wchar_t*,DWORD);
API BOOL __stdcall SetCurrentDirectoryW(const wchar_t*);
API HANDLE __stdcall CreateFileW(const wchar_t*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
API BOOL __stdcall WriteFile(HANDLE,const void*,DWORD,DWORD*,void*);
API BOOL __stdcall ReadFile(HANDLE,void*,DWORD,DWORD*,void*);
API BOOL __stdcall CloseHandle(HANDLE);
API BOOL __stdcall FlushFileBuffers(HANDLE);
API BOOL __stdcall GetFileAttributesExW(const wchar_t*,int,void*);
API BOOL __stdcall VirtualProtect(void*,QWORD,DWORD,DWORD*);
API void* __stdcall VirtualAlloc(void*,QWORD,DWORD,DWORD);
API BOOL __stdcall FlushInstructionCache(HANDLE,const void*,QWORD);
API DWORD __stdcall GetLastError(void);
API void __stdcall SetLastError(DWORD);
API void __stdcall ExitProcess(DWORD);
API DWORD __stdcall GetCurrentThreadId(void);
/* user32: subclass the Unity window so an interactive close still flushes
 * timeplots (the mod's `quit` command only covers scripted closes). */
API void* __stdcall FindWindowW(const wchar_t*,const wchar_t*);
API void* __stdcall SetWindowLongPtrW(void*,int,void*);
API void* __stdcall CallWindowProcW(void*,void*,unsigned int,QWORD,QWORD);

/* The probe is linked without a CRT.  Larger C aggregates can still make
 * clang emit a memcpy call even with -fno-builtin, so provide the tiny local
 * primitive rather than pulling in a runtime library. */
void* memcpy(void* destination,const void* source,QWORD count) {
    BYTE* to=(BYTE*)destination;const BYTE* from=(const BYTE*)source;
    while(count--) *to++=*from++;
    return destination;
}

/* The freestanding event/getter translation unit has a larger bounded local
 * snapshot than bootstrap-only mode.  clang therefore emits a Windows stack
 * probe call; this probe owns its stack and does not link a CRT, so provide
 * the minimal symbol instead of importing an unpinned runtime helper. */
void __chkstk(void) {}

static HANDLE log_file;
static PROC (__stdcall *real_getproc)(MODULE,const char*)=GetProcAddress;
static char log_buffer[65536];
static DWORD log_used;
static QWORD resolve_count;
static int overflow_reported;
#ifdef V014_METADATA
#include "api_indices.h"
static PROC resolved[256];
static PROC real_runtime_invoke;
static DWORD metadata_main_thread;
static int metadata_busy;
static int metadata_discovered;
static void* __stdcall metadata_observe_invoke(void*,void*,void**,void**);
#endif

static int same(const char*a,const char*b) {
    while(*a&&*a==*b){++a;++b;}
    return *a==*b;
}
static void flush_log(void) {
    DWORD written=0;
    if(log_used) WriteFile(log_file,log_buffer,log_used,&written,0);
    log_used=0;
    FlushFileBuffers(log_file);
}
static void write_text(const char*s) {
    while(*s) {
        if(log_used==sizeof(log_buffer)) flush_log();
        log_buffer[log_used++]=*s++;
    }
}
static void write_hex(QWORD x) {
    char s[19];
    s[0]='0'; s[1]='x';
    for(int i=0;i<16;++i) s[2+i]="0123456789abcdef"[(x>>(60-4*i))&15];
    s[18]=0;
    write_text(s);
}
#ifdef V014_METADATA
#include "metadata_probe.h"
#endif
static PROC __stdcall observe_getproc(MODULE module,const char* name) {
    PROC result=real_getproc(module,name);
    DWORD error=GetLastError();
    if((QWORD)name>65535 && module==GetModuleHandleW(L"GameAssembly.dll")) {
        QWORD index=resolve_count;
        PROC observed=result;
#ifdef V014_METADATA
        if(index<sizeof(resolved)/sizeof(resolved[0])) resolved[index]=observed;
        if(index==API_il2cpp_runtime_invoke && observed) {
            real_runtime_invoke=observed;
            result=(PROC)&metadata_observe_invoke;
        }
#endif
        if(resolve_count<4096) {
            write_text("{\"kind\":\"resolve\",\"index\":\"");
            write_hex(resolve_count);
            write_text("\",\"symbol\":\"");
            write_text(name);
            write_text("\",\"rva\":\"");
            write_hex(observed?(QWORD)observed-(QWORD)module:0);
            write_text("\"}\n");
            flush_log();
        } else if(!overflow_reported) {
            write_text("{\"kind\":\"resolve_overflow\",\"limit\":4096}\n");
            flush_log();
            overflow_reported=1;
        }
        ++resolve_count;
    }
    SetLastError(error);
    return result;
}
static int observe_import(MODULE module) {
    BYTE* b=(BYTE*)module;
    if(*(WORD*)b!=0x5a4d) return 0;
    DWORD nt=*(DWORD*)(b+0x3c);
    if(*(DWORD*)(b+nt)!=0x4550||*(WORD*)(b+nt+24)!=0x20b) return 0;
    DWORD import_rva=*(DWORD*)(b+nt+24+112+8);
    if(!import_rva) return 0;
    DWORD* descriptor=(DWORD*)(b+import_rva);
    for(;descriptor[3];descriptor+=5) {
        if(!descriptor[0]) continue;
        QWORD* names=(QWORD*)(b+descriptor[0]);
        QWORD* targets=(QWORD*)(b+descriptor[4]);
        for(int i=0;names[i];++i) {
            if(names[i]>>63) continue;
            const char* name=(const char*)(b+names[i]+2);
            if(same(name,"GetProcAddress")) {
                DWORD old,ignored;
                if(!VirtualProtect(&targets[i],8,4,&old)) return 0;
                real_getproc=(void*)targets[i];
                targets[i]=(QWORD)&observe_getproc;
                VirtualProtect(&targets[i],8,old,&ignored);
                return 1;
            }
        }
    }
    return 0;
}
void start(void) {
#ifdef V014_METADATA
    metadata_main_thread=GetCurrentThreadId();
#endif
    wchar_t path[1024];
    DWORD length=GetModuleFileNameW(0,path,1024);
    if(!length||length>=1024) ExitProcess(90);
    while(length&&path[length-1]!=L'\\'&&path[length-1]!=L'/') --length;
    path[length]=0;
    SetCurrentDirectoryW(path);
    log_file=CreateFileW(L"probe-startup.jsonl",0x40000000,3,0,2,0x80,0);
    if(log_file==(HANDLE)-1) ExitProcess(91);
    write_text("{\"kind\":\"bootstrap\",\"version\":2,\"scope\":\"racev2-api-resolution\"}\n");
    flush_log();
    MODULE unity=LoadLibraryW(L"UnityPlayer.dll");
    if(!unity) {
        write_text("{\"kind\":\"load_failed\"}\n");
        flush_log();
        ExitProcess(92);
    }
    write_text(observe_import(unity)?"{\"kind\":\"resolver_observed\"}\n":"{\"kind\":\"resolver_not_found\"}\n");
    flush_log();
    int (__stdcall *unity_main)(MODULE,MODULE,wchar_t*,int)=(void*)GetProcAddress(unity,"UnityMain");
    if(!unity_main) ExitProcess(93);
    write_text("{\"kind\":\"enter_unity\"}\n");
    flush_log();
    int code=unity_main(GetModuleHandleW(0),0,L"-screen-fullscreen 0 -screen-width 1280 -screen-height 720 -logFile probe-player.log",1);
    write_text("{\"kind\":\"exit_unity\"}\n");
    flush_log();
    ExitProcess((DWORD)code);
}
