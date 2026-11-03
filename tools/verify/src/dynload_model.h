/* Model of the `osmem ... act`, `osmem ... dynload` and `osmem ... procui` parts: the port's nn::act HLE (runtime/src/hle/system_stubs.cpp
 * "nn_act" section), its OSDynLoad HLE (runtime/src/hle/coreinit_misc.cpp "OSDynLoad" section, with core.cpp
 * dispatch::register_host and hle_find) and its stateful erreula HLE (runtime/src/hle/erreula.cpp, whole), read at port
 * commit f5554821. dynload_equiv.py compiles those port sources with shims against this header.
 *
 *   act      Initialize C00046E8 -> 0, Finalize C00044E8 -> 0, GetSlotNo C0004660 -> 1, GetPrincipalId C0004640 -> 0,
 *            GetParentalControlSlotNoEx C0004618: *(u8*)r3 = 1, -> 0. Only r3 is written (no generated r4).
 *   dynload  OSDynLoad_Acquire C0009A98 (name r3, out r4): the name without a trailing ".rpl" (when longer than 4),
 *            handle = 0x70000000 | (std::hash<std::string>(name) & 0x0FFFFFFF) (the host libc++'s hash, as the port's
 *            build on the same host), remembered, stored through r4, -> 0.
 *            OSDynLoad_FindExport C0009AC0 (handle r3, isData r4, name r5, out r6): key = module + "." + name (module "?"
 *            for an unknown handle); the first lookup of a key hands out an address, later ones the same address:
 *            a function export that the port's HLE registry has (hle_find) gets the next register_host address
 *            (C2000000 + 8*n, n counted from 3: the boot registers MEMAllocFromDefaultHeap/Ex and MEMFreeToDefaultHeap
 *            first, coreinit_mem.cpp); one it lacks gets one too, for the port's logging stub (r3 = 0 when called);
 *            a data export gets a zeroed 0x100-byte runtime_alloc block (here from the osmem runtime bump region).
 *            The address is stored through r6, -> 0. Only erreula's registry is modelled: a function export of any
 *            other named module is a model rejection (the port may have an HLE for it).
 *            OSDynLoad_Release C0009AF8: no effect. OSDynLoad_SetAllocator C0009B00 -> 0.
 *            Each import is also answered at its RPX fimport stub (slot - 0xBD70BD88).
 *   erreula  (the function exports FindExport hands out for module "erreula") erreula.cpp's state machine: hidden /
 *            appearing / visible / disappearing, the "decided" flag (confirmed with the first button) and the
 *            HOME-disabled sign. The port times the fades by the host's steady_clock (0.2 s, then 1.0 s before the
 *            automatic confirm); here time is ErrEulaCalc calls at 30 per second (the game calls Calc once per 30 Hz
 *            frame): the n-th Calc after a state change sees n/30 s, so appearing -> visible and disappearing ->
 *            hidden at the 7th Calc (> 0.2 s), visible -> decided at the 31st (> 1.0 s). The LOG line of
 *            ErrEulaAppearError (it reads the AppearArg's error code and texts) is host output: not read here.
 *
 *   swkbd    (opt-in word on top of `dynload`; runtime/src/hle/swkbd.cpp at f5554821) module "swkbd" joins erreula: FindExport hands
 *            out the next register_host address for every function, so the game's loader 0286CD7C (35 names, all in the
 *            port's registry) succeeds. By need: only the export the J3D route (a) calls is
 *            answered, from the port's code: SwkbdCreate (swkbd.cpp:151-156: the form buffer runtime_alloc(0x2000) once, r3 = 1;
 *            g_need_font/predict are read only by exports that stay rejected). Calling any other export of the module is a
 *            model rejection naming it, so the run fails closed at the call.
 *
 * Templated on an environment E: uint8_t ld8(a); void st8(a, v); void st32(a, v); uint32_t runtime_alloc(size)
 * (zeroed, align 16); [[noreturn]] void reject(const char* why). */
#pragma once
#include <stdint.h>
#include <cstdio>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace dlm {

enum : uint32_t {
    ActFinalize = 0xC00044E8u, ActGetParentalControlSlotNoEx = 0xC0004618u, ActGetPrincipalId = 0xC0004640u,
    ActGetSlotNo = 0xC0004660u, ActInitialize = 0xC00046E8u,
    DynAcquire = 0xC0009A98u, DynFindExport = 0xC0009AC0u, DynRelease = 0xC0009AF8u, DynSetAllocator = 0xC0009B00u,
};
/* RPX fimport stub = slot - delta, one delta per library (measured as strictmocks_test.py 6 does) */
static const uint32_t kStubDelta = 0xBD70BD88u;    /* coreinit: OSDynLoad */
static const uint32_t kStubDeltaAct = 0xBD70BBC0u; /* nn_act */
static const uint32_t kStubDeltaProcUI = 0xBD70BCF0u; /* proc_ui */
/* `procui`: ProcUIProcessMessages: system_stubs.cpp:81 at f5554821 `ret(c, 0)`, PROCUI_STATUS_IN_FOREGROUND; r3 only */
static const uint32_t ProcUIProcessMessages = 0xC0007880u, kProcUIForeground = 0;
inline bool procui_slot(uint32_t t) { return t == ProcUIProcessMessages || t == ProcUIProcessMessages - kStubDeltaProcUI; }
static const uint32_t kActImports[] = {ActFinalize, ActGetParentalControlSlotNoEx, ActGetPrincipalId, ActGetSlotNo, ActInitialize};
static const uint32_t kDynImports[] = {DynAcquire, DynFindExport, DynRelease, DynSetAllocator};
static const uint32_t kHleFuncBase = 0xC2000000u; /* runtime.h: synthetic addresses of host functions, 8-byte steps */
static const uint32_t kFirstHost = 3;             /* the boot's three default-heap functions come first */
static const uint32_t kMaxHosts = 4096;           /* handed-out functions per run (a model rejection beyond) */
static const uint32_t kStub = 0xFFu;              /* the port's "unimplemented dynamic export" stub */

inline uint32_t act_slot(uint32_t t) {
    for (uint32_t s : kActImports) if (t == s || t == s - kStubDeltaAct) return s;
    return 0;
}
inline uint32_t dyn_slot(uint32_t t) {
    for (uint32_t s : kDynImports) if (t == s || t == s - kStubDelta) return s;
    return 0;
}
/* the dynamic-export address range (parse-time conflict check: a mock directive may not name one) */
inline bool host_range(uint32_t t) { return t - (kHleFuncBase + 8 * kFirstHost) < 8 * kMaxHosts; }

/* erreula.cpp, in file order (the names the port registers under lib "erreula") */
#define DLM_ERREULA(X)                                                                                    \
    X(Create, "ErrEulaCreate__3RplFPUcQ3_2nn7erreula10RegionTypeQ3_2nn7erreula8LangTypeP8FSClient")      \
    X(Destroy, "ErrEulaDestroy__3RplFv")                                                                  \
    X(AppearError, "ErrEulaAppearError__3RplFRCQ3_2nn7erreula9AppearArg")                                 \
    X(DisappearError, "ErrEulaDisappearError__3RplFv")                                                    \
    X(Calc, "ErrEulaCalc__3RplFRCQ3_2nn7erreula14ControllerInfo")                                         \
    X(GetStateErrorViewer, "ErrEulaGetStateErrorViewer__3RplFv")                                          \
    X(IsDecideSelectButtonError, "ErrEulaIsDecideSelectButtonError__3RplFv")                              \
    X(IsDecideSelectLeftButtonError, "ErrEulaIsDecideSelectLeftButtonError__3RplFv")                      \
    X(IsDecideSelectRightButtonError, "ErrEulaIsDecideSelectRightButtonError__3RplFv")                    \
    X(GetSelectButtonNumError, "ErrEulaGetSelectButtonNumError__3RplFv")                                  \
    X(GetResultCode, "ErrEulaGetResultCode__3RplFv")                                                      \
    X(GetResultType, "ErrEulaGetResultType__3RplFv")                                                      \
    X(AppearHomeNixSign, "ErrEulaAppearHomeNixSign__3RplFRCQ3_2nn7erreula14HomeNixSignArg")               \
    X(IsAppearHomeNixSign, "ErrEulaIsAppearHomeNixSign__3RplFv")                                          \
    X(DisappearHomeNixSign, "ErrEulaDisappearHomeNixSign__3RplFv")                                        \
    X(ChangeLang, "ErrEulaChangeLang__3RplFQ3_2nn7erreula8LangType")                                      \
    X(DrawTV, "ErrEulaDrawTV__3RplFv")                                                                    \
    X(DrawDRC, "ErrEulaDrawDRC__3RplFv")                                                                  \
    X(IsSelectCursorActive, "ErrEulaIsSelectCursorActive__3RplFv")                                        \
    X(SetControllerRemo, "ErrEulaSetControllerRemo__3RplFQ3_2nn7erreula14ControllerType")
enum Erreula : uint8_t {
#define DLM_E(id, s) E_##id,
    DLM_ERREULA(DLM_E)
#undef DLM_E
    E_COUNT
};
static const char* const kErreulaNames[] = {
#define DLM_N(id, s) s,
    DLM_ERREULA(DLM_N)
#undef DLM_N
};
inline int erreula_index(const std::string& sym) {
    for (int i = 0; i < E_COUNT; i++) if (sym == kErreulaNames[i]) return i;
    return -1;
}

/* swkbd.cpp's registry names, as the game's loader 0286CD7C looks them up (35 names, 104B67A0..104B6828) */
#define DLM_SWKBD(X)                                                                                      \
    X(AppearInputForm, "SwkbdAppearInputForm__3RplFRCQ3_2nn5swkbd9AppearArg")                             \
    X(AppearKeyboard, "SwkbdAppearKeyboard__3RplFRCQ3_2nn5swkbd11KeyboardArg")                            \
    X(CalcSubThreadFont, "SwkbdCalcSubThreadFont__3RplFv")                                                \
    X(CalcSubThreadPredict, "SwkbdCalcSubThreadPredict__3RplFv")                                          \
    X(Calc, "SwkbdCalc__3RplFRCQ3_2nn5swkbd14ControllerInfo")                                             \
    X(ConfirmUnfixAll, "SwkbdConfirmUnfixAll__3RplFv")                                                    \
    X(Create, "SwkbdCreate__3RplFPUcQ3_2nn5swkbd10RegionTypeUiP8FSClient")                                \
    X(Destroy, "SwkbdDestroy__3RplFv")                                                                    \
    X(DisappearInputForm, "SwkbdDisappearInputForm__3RplFv")                                              \
    X(DisappearKeyboard, "SwkbdDisappearKeyboard__3RplFv")                                                \
    X(DrawDRC, "SwkbdDrawDRC__3RplFv")                                                                    \
    X(DrawTV, "SwkbdDrawTV__3RplFv")                                                                      \
    X(GetDrawStringInfo, "SwkbdGetDrawStringInfo__3RplFPQ3_2nn5swkbd14DrawStringInfo")                    \
    X(GetInputFormString, "SwkbdGetInputFormString__3RplFv")                                              \
    X(GetKeyboardCondition, "SwkbdGetKeyboardCondition__3RplFPQ3_2nn5swkbd17KeyboardCondition")           \
    X(InitLearnDic, "SwkbdInitLearnDic__3RplFPv")                                                         \
    X(GetStateInputForm, "SwkbdGetStateInputForm__3RplFv")                                                \
    X(GetStateKeyboard, "SwkbdGetStateKeyboard__3RplFv")                                                  \
    X(IsCoveredWithSubWindow, "SwkbdIsCoveredWithSubWindow__3RplFv")                                      \
    X(IsDecideCancelButton, "SwkbdIsDecideCancelButton__3RplFPb")                                         \
    X(IsDecideOkButton, "SwkbdIsDecideOkButton__3RplFPb")                                                 \
    X(IsKeyboardTarget, "SwkbdIsKeyboardTarget__3RplFPQ3_2nn5swkbd14IEventReceiver")                      \
    X(IsNeedCalcSubThreadFont, "SwkbdIsNeedCalcSubThreadFont__3RplFv")                                    \
    X(IsNeedCalcSubThreadPredict, "SwkbdIsNeedCalcSubThreadPredict__3RplFv")                              \
    X(IsSelectCursorActive, "SwkbdIsSelectCursorActive__3RplFv")                                          \
    X(InactivateSelectCursor, "SwkbdInactivateSelectCursor__3RplFv")                                      \
    X(SetControllerRemo, "SwkbdSetControllerRemo__3RplFQ3_2nn5swkbd14ControllerType")                     \
    X(SetCursorPos, "SwkbdSetCursorPos__3RplFi")                                                          \
    X(SetEnableOkButton, "SwkbdSetEnableOkButton__3RplFb")                                                \
    X(SetReceiver, "SwkbdSetReceiver__3RplFRCQ3_2nn5swkbd11ReceiverArg")                                  \
    X(SetSelectFrom, "SwkbdSetSelectFrom__3RplFi")                                                        \
    X(SetUserControllerEventObj, "SwkbdSetUserControllerEventObj__3RplFPQ3_2nn5swkbd19IControllerEventObj") \
    X(SetUserSoundObj, "SwkbdSetUserSoundObj__3RplFPQ3_2nn5swkbd9ISoundObj")                              \
    X(SetInputFormString, "SwkbdSetInputFormString__3RplFPCw")                                            \
    X(MuteAllSound, "SwkbdMuteAllSound__3RplFb")
enum Swkbd : uint8_t {
#define DLM_S(id, s) S_##id,
    DLM_SWKBD(DLM_S)
#undef DLM_S
    S_COUNT
};
static const char* const kSwkbdNames[] = {
#define DLM_N(id, s) s,
    DLM_SWKBD(DLM_N)
#undef DLM_N
};
static const uint8_t kSwkbdBase = 0x40; /* State::hosts: 0x40 + Swkbd index */
static const uint32_t kFormBufBytes = 0x2000; /* swkbd.cpp kMaxForm * 2 */
inline int swkbd_index(const std::string& sym) {
    for (int i = 0; i < S_COUNT; i++) if (sym == kSwkbdNames[i]) return i;
    return -1;
}

enum ErrState : uint32_t { kHidden = 0, kAppearing = 1, kVisible = 2, kDisappearing = 3 };
/* Calc calls at 30 per second: elapsed() > 0.2 s <=> n > 6, > 1.0 s <=> n > 30 */
static const uint32_t kFadeCalcs = 6, kDecideCalcs = 30;

/* the HLEs' statics (coreinit_misc.cpp g_dyn_modules / g_dyn_exports, core.cpp g_next_host, erreula.cpp g_*): run state */
struct State {
    std::map<uint32_t, std::string> modules;   /* handle -> module name */
    std::map<std::string, uint32_t> exports;   /* "module.symbol" -> address */
    std::vector<uint8_t> hosts;                /* handed-out host functions in order: erreula index or kStub */
    uint32_t err_state = kHidden;
    bool err_decided = false, err_home_nix = false;
    uint32_t err_calcs = 0;                    /* Calc calls since the last set_state (saturating) */
    uint32_t sw_form_buf = 0;                  /* swkbd.cpp S.form_buf */
    int host_index(uint32_t t) const {
        uint32_t d = t - (kHleFuncBase + 8 * kFirstHost);
        if (d & 7 || d / 8 >= hosts.size()) return -1;
        return (int)(d / 8);
    }
    /* run state as words (osm::save / load) */
    void save(std::vector<uint32_t>& v) const {
        auto str = [&](const std::string& s) {
            v.push_back((uint32_t)s.size());
            for (size_t i = 0; i < s.size(); i += 4) {
                uint32_t w = 0;
                for (size_t j = 0; j < 4; j++) w = w << 8 | (i + j < s.size() ? (uint8_t)s[i + j] : 0);
                v.push_back(w);
            }
        };
        v.push_back((uint32_t)modules.size());
        for (auto& [h, n] : modules) { v.push_back(h); str(n); }
        v.push_back((uint32_t)exports.size());
        for (auto& [k, a] : exports) { str(k); v.push_back(a); }
        v.push_back((uint32_t)hosts.size());
        for (uint8_t k : hosts) v.push_back(k);
        v.push_back(err_state);
        v.push_back((uint32_t)err_decided | (uint32_t)err_home_nix << 1);
        v.push_back(err_calcs);
        v.push_back(sw_form_buf);
    }
    template <class NX> void load(NX nx) {
        *this = State();
        auto str = [&]() {
            uint32_t n = nx();
            std::string s;
            for (uint32_t i = 0; i < n; i += 4) {
                uint32_t w = nx();
                for (uint32_t j = 0; j < 4 && i + j < n; j++) s.push_back((char)(w >> (24 - 8 * j)));
            }
            return s;
        };
        for (uint32_t i = 0, n = nx(); i < n; i++) { uint32_t h = nx(); modules[h] = str(); }
        for (uint32_t i = 0, n = nx(); i < n; i++) { std::string k = str(); exports[k] = nx(); }
        for (uint32_t i = 0, n = nx(); i < n; i++) hosts.push_back((uint8_t)nx());
        err_state = nx();
        uint32_t f = nx();
        err_decided = f & 1;
        err_home_nix = f >> 1 & 1;
        err_calcs = nx();
        sw_form_buf = nx();
    }
};

template <class E> std::string read_cstr(E& e, uint32_t ea) { /* mem::read_cstr: null -> "" */
    std::string s;
    if (!ea) return s;
    for (uint32_t j = 0;; j++) {
        if (j >= 4096) e.reject("osmem dynload: name string longer than 4096 bytes");
        uint8_t b = e.ld8(ea + j);
        if (!b) break;
        s.push_back((char)b);
    }
    return s;
}

inline const char* import_name(uint32_t t) {
    switch (act_slot(t) ? act_slot(t) : dyn_slot(t)) {
    case ActFinalize: return "nn::act::Finalize"; case ActGetParentalControlSlotNoEx: return "nn::act::GetParentalControlSlotNoEx";
    case ActGetPrincipalId: return "nn::act::GetPrincipalId"; case ActGetSlotNo: return "nn::act::GetSlotNo";
    case ActInitialize: return "nn::act::Initialize"; case DynAcquire: return "OSDynLoad_Acquire";
    case DynFindExport: return "OSDynLoad_FindExport"; case DynRelease: return "OSDynLoad_Release";
    case DynSetAllocator: return "OSDynLoad_SetAllocator";
    }
    return "?";
}

/* erreula.cpp set_state */
inline void set_state(State& S, uint32_t s) { S.err_state = s; S.err_calcs = 0; }

/* one call to an erreula export (erreula.cpp): 1 = r3 is *r, 0 = void (r3 kept) */
inline int erreula_call(State& S, int fn, uint32_t* r) {
    switch (fn) {
    case E_Create: set_state(S, kHidden); *r = 1; return 1;
    case E_Destroy: set_state(S, kHidden); return 0;
    case E_AppearError: S.err_decided = false; set_state(S, kAppearing); return 0;
    case E_DisappearError: if (S.err_state == kVisible) set_state(S, kDisappearing); return 0;
    case E_Calc: /* this Calc is one frame (1/30 s) after the previous one */
        if (S.err_calcs != 0xFFFFFFFFu) S.err_calcs++;
        if (S.err_state == kAppearing && S.err_calcs > kFadeCalcs) set_state(S, kVisible);
        else if (S.err_state == kDisappearing && S.err_calcs > kFadeCalcs) set_state(S, kHidden);
        else if (S.err_state == kVisible && !S.err_decided && S.err_calcs > kDecideCalcs) S.err_decided = true;
        return 0;
    case E_GetStateErrorViewer: *r = S.err_state; return 1;
    case E_IsDecideSelectButtonError: case E_IsDecideSelectLeftButtonError: *r = S.err_decided; return 1;
    case E_IsDecideSelectRightButtonError: case E_GetSelectButtonNumError: case E_GetResultCode: *r = 0; return 1;
    case E_GetResultType: *r = S.err_decided ? 1 : 0; return 1;
    case E_AppearHomeNixSign: S.err_home_nix = true; return 0;
    case E_IsAppearHomeNixSign: *r = S.err_home_nix; return 1;
    case E_DisappearHomeNixSign: S.err_home_nix = false; return 0;
    case E_IsSelectCursorActive: *r = 0; return 1;
    default: return 0; /* ChangeLang, DrawTV, DrawDRC, SetControllerRemo */
    }
}

/* one call to a swkbd export: only the one the route calls; any other fails closed here, at the call */
template <class E> int swkbd_call(E& e, State& S, int fn, uint32_t* r) {
    if (fn == S_Create) { if (!S.sw_form_buf) S.sw_form_buf = e.runtime_alloc(kFormBufBytes); *r = 1; return 1; }
    static char why[200];
    snprintf(why, sizeof why, "osmem swkbd: call to %s (not modelled: by need)", kSwkbdNames[fn]);
    e.reject(why);
}

/* one modelled call: -1 = not modelled (the caller answers it as before), 1 = r3 is *r, 0 = void (r3 kept).
 * a = r0..r10 at the call. Only r3 is ever written. */
template <class E> int call(E& e, State& S, bool act, bool dyn, bool swk, uint32_t target, const uint32_t* a, uint32_t* r) {
    if (act)
        switch (act_slot(target)) {
        case ActInitialize: case ActFinalize: case ActGetPrincipalId: *r = 0; return 1;
        case ActGetSlotNo: *r = 1; return 1;
        case ActGetParentalControlSlotNoEx: e.st8(a[3], 1); *r = 0; return 1;
        }
    if (!dyn) return -1;
    switch (dyn_slot(target)) {
    case DynAcquire: {
        std::string name = read_cstr(e, a[3]);
        if (name.size() > 4 && name.substr(name.size() - 4) == ".rpl") name.resize(name.size() - 4);
        uint32_t h = 0x70000000u | (uint32_t)(std::hash<std::string>()(name) & 0x0FFFFFFF);
        S.modules[h] = name;
        e.st32(a[4], h);
        *r = 0;
        return 1;
    }
    case DynRelease: return 0;
    case DynSetAllocator: *r = 0; return 1;
    case DynFindExport: {
        uint32_t h = a[3], is_data = a[4], out = a[6];
        std::string sym = read_cstr(e, a[5]);
        auto m = S.modules.find(h);
        std::string lib = m != S.modules.end() ? m->second : "?";
        std::string key = lib + "." + sym;
        auto it = S.exports.find(key);
        if (it == S.exports.end()) {
            uint32_t addr;
            if (is_data) addr = e.runtime_alloc(0x100);
            else {
                int fn = -1;
                if (lib == "erreula") fn = erreula_index(sym);
                else if (swk && lib == "swkbd") { fn = swkbd_index(sym); if (fn >= 0) fn += kSwkbdBase; }
                else if (lib != "?") e.reject("osmem dynload: FindExport of a function in a module other than erreula (not modelled)");
                if (S.hosts.size() >= kMaxHosts) e.reject("osmem dynload: more than 4096 dynamic function exports");
                addr = kHleFuncBase + 8 * (kFirstHost + (uint32_t)S.hosts.size());
                S.hosts.push_back(fn < 0 ? (uint8_t)kStub : (uint8_t)fn);
            }
            it = S.exports.emplace(key, addr).first;
        }
        e.st32(out, it->second);
        *r = 0;
        return 1;
    }
    }
    int k = S.host_index(target);
    if (k < 0) return -1;
    if (S.hosts[k] == kStub) { *r = 0; return 1; } /* "[dynload] call to unimplemented dynamic export": r3 = 0 */
    if (S.hosts[k] >= kSwkbdBase) return swkbd_call(e, S, S.hosts[k] - kSwkbdBase, r);
    return erreula_call(S, S.hosts[k], r);
}

/* the export a host address stands for (trace) */
inline std::string host_name(const State& S, uint32_t t) {
    int k = S.host_index(t);
    if (k < 0) return "?";
    for (auto& [key, addr] : S.exports) if (addr == t) return key;
    return "?";
}

}  // namespace dlm
