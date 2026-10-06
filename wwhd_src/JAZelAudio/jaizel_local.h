/* JAZelAudio (Zelda sound control): helpers shared by the JAIZel* translation units. WWHD.
 * Included inside each TU's own namespace, after
 * bindings.h. The JAudio library (JAI* / JAS*) is out of scope: its functions are mocked callees,
 * bound here by address (local bindings, SHARED-CANDIDATE). */

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

/* JAIZelBasic::zel_basic (the singleton, mDoAud_zelAudio_c::getInterface()) */
static inline u32 zel_basic() { return ld(0x101FFC78); }

struct Vec_l { be<u32> w[3]; };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* JAIZelBasic (game code, this unit) */
static inline u32 JAIZelBasic_seStart_l(u32 b, u32 id, u32 pos, u32 info, s32 reverb, f32 a, f32 c, f32 d, f32 e, u32 flag) {
    return gabi::call<u32>(0x0201EBA0, b, id, pos, info, (u32)reverb, flag, a, c, d, e);
}
static inline s32 JAIZelBasic_checkStreamPlaying_l(u32 b, u32 id) { return gabi::call<s32>(0x0201DB08, b, id); }
/* JAudio library (mocked) */
static inline void JAIAnimeSound_initActorAnimSound_l(u32 t, u32 data, u32 a, f32 f) { gabi::call(0x02801BA8, t, data, a, f); }
static inline void JAIAnimeSound_setAnimSoundVec_l(u32 t, u32 basic, u32 pos, f32 frame, f32 rate, u32 id, u32 b) {
    gabi::call(0x02801D04, t, basic, pos, id, b, frame, rate);
}
static inline void JAIBasic_startSoundActor_l(u32 b, u32 id, u32 sound, u32 actor, u32 a, u32 c) { gabi::call(0x02802844, b, id, sound, actor, a, c); }
static inline f32 JAIGlobalParameter_getParamDistanceMax_l() { return gabi::call<f32>(0x02804718); }
static inline void JAISound_stop_l(u32 s, u32 t) { gabi::call(0x0280B1D8, s, t); }
static inline void JAISound_setVolume_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280B68C, s, t, k, v); }
static inline void JAISound_setPitch_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280BD74, s, t, k, v); }
static inline void JAISound_setPortData_l(u32 s, u32 port, u32 v) { gabi::call(0x0280C6D8, s, port, v); }
static inline u32 JAISound_getID_l(u32 s) { return gabi::call<u32>(0x0280D2D8, s); }
/* runtime / SDK */
static inline void PSMTXMultVec_l(u32 m, u32 in, u32 out) { gabi::call(0x028E8F64, m, in, out); }
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }
/* heap-pointer check (result unused: a stripped assertion) */
static inline void ptr_check_l(u32 p) { gabi::call<u32>(0x0275CC9C, p); }
static inline void register_global_object_l(u32 rec) { gabi::call(0x028F026C, rec); }

/* The 148-byte header-statics initializer every JAIZel TU ends with: a zeroed 16-byte object
 * (base), a {-pi, pi} pair and two one-byte objects with constructors, each registered for
 * destruction. bss = the TU's block, rec = its three destruction records (12 bytes apart),
 * ro = its {-pi, pi} rodata pair. */
static inline void header_sinit(u32 bss, u32 rec, u32 ro) {
    st(bss + 0xC + 8, 0); st(bss + 0xC, 0); st(bss + 0xC + 0xC, 0); st(bss + 0xC + 4, 0);
    register_global_object_l(rec);
    f32 lo = ldf(ro), hi = ldf(ro + 4);
    stf(bss, lo);
    stf(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    register_global_object_l(rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    register_global_object_l(rec + 0x18);
}
