/* d_s_play part 3: heapSizeCheck and phase_00. WWHD. 
 * See d_s_play.cpp for the unit's range and layouts.
 *
 * HD phase_00 constructs the scene object in place, builds the HD scene system at +0x1D4 from a
 * stack description (0x214 bytes), loads the "LOD48" setup through the HD resource system and
 * initialises the camera-like object it returns, then does the GameCube reset handling. */
#include "bindings.h"

namespace d_s_play_3_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 mDoExt_getArchiveHeap_l() { return gabi::call<u32>(0x025E3268); }
static inline u32 mDoExt_getGameHeap_l() { return gabi::call<u32>(0x025E2FBC); }
static inline u32 mDoExt_getZeldaHeap_l() { return gabi::call<u32>(0x025EE048); }
static inline u32 mDoExt_getCommandHeap_l() { return gabi::call<u32>(0x025EE060); }
static inline s32 mDoExt_getSafeArchiveHeapSize_l() { return gabi::call<s32>(0x025EE078); }
static inline s32 mDoExt_getSafeGameHeapSize_l() { return gabi::call<s32>(0x025E2FF8); }
static inline s32 mDoExt_getSafeZeldaHeapSize_l() { return gabi::call<s32>(0x025EE054); }
static inline s32 mDoExt_getSafeCommandHeapSize_l() { return gabi::call<s32>(0x025EE06C); }
static inline s32 JKRHeap_getTotalFreeSize_l(u32 h) { return gabi::call<s32>(0x027EC1FC, h); }
static inline BOOL heapSizeCheck_l() { return gabi::call<BOOL>(0x025B0C30); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 ld_vt(u32 obj, u32 off) { return gabi::load<u32>(gabi::load<u32>(obj + 0xC) + off); }
static inline s32 JKRHeap_getFreeSize_l(u32 h) { return gabi::call_ptr<s32>(ld_vt(h, 0x74), h); } /* virtual */

struct sstr_l { be<u32> str, vt; };
struct u32_l { be<u32> v; };
struct u32x2_l { be<u32> a, b; };
struct desc_l { u8 b[0x214]; };

/* 025B0C30 */
static BOOL heapSizeCheck() {
    WWHD_FUNC(0x025B0C30, BOOL);
    s32 archive_free = JKRHeap_getFreeSize_l(mDoExt_getArchiveHeap_l());
    s32 archive_total_free = JKRHeap_getTotalFreeSize_l(mDoExt_getArchiveHeap_l());
    f32 at = (f32)archive_total_free;
    s32 archive_safe = mDoExt_getSafeArchiveHeapSize_l();
    f32 a1 = (f32)archive_free / at;
    f32 a2 = at / (f32)archive_safe;
    s32 game_free = JKRHeap_getFreeSize_l(mDoExt_getGameHeap_l());
    s32 game_total_free = JKRHeap_getTotalFreeSize_l(mDoExt_getGameHeap_l());
    f32 gt = (f32)game_total_free;
    s32 game_safe = mDoExt_getSafeGameHeapSize_l();
    f32 g1 = (f32)game_free / gt;
    f32 g2 = gt / (f32)game_safe;
    JKRHeap_getFreeSize_l(mDoExt_getZeldaHeap_l());
    f32 zt = (f32)JKRHeap_getTotalFreeSize_l(mDoExt_getZeldaHeap_l());
    f32 z = zt / (f32)mDoExt_getSafeZeldaHeapSize_l();
    JKRHeap_getFreeSize_l(mDoExt_getCommandHeap_l());
    f32 ct = (f32)JKRHeap_getTotalFreeSize_l(mDoExt_getCommandHeap_l());
    f32 c = ct / (f32)mDoExt_getSafeCommandHeapSize_l();
    if (a2 < 0.7f || a1 < 0.7f || g2 < 0.7f || g1 < 0.7f || z < 0.7f || c < 0.7f) return FALSE; /* bge on NaN: a NaN last ratio passes */
    return TRUE;
}
VERIFY(0x025B0C30, heapSizeCheck);

/* the HD scene system's entry for the current index (a bounded table lookup), or NULL */
static inline u32 scene_entry(u32 sys, u32 idxPtr) {
    u32 idx = (u32)(s32)(s16)ld16(idxPtr);
    u32 n1 = ld(sys + 8);
    u32 arr = ld(sys + 0xC);
    u32 e = idx < n1 ? arr + (idx << 2) : arr;
    if (ld16(e + 2) == 0) return 0;
    u32 n2 = ld(sys + 0x10);
    u32 e2 = idx < n1 ? arr + (idx << 2) : arr;
    u32 k = ld16(e2);
    u32 p2 = 0;
    if (k < n2) p2 = ld(sys + 0x14) + (k << 2);
    return ld(p2);
}

/* 025B0FB4: HD: see the file comment */
static s32 phase_00(u32 i_this) {
    WWHD_FUNC(0x025B0FB4, s32, i_this);
    if (ld8(0x1018F484) == 0) {
        gabi::call(0x02038624, gabi::call<u32>(0x0203E900));
        u32 x = gabi::call<u32>(0x0203E900);
        gabi::call(0x02038744, ld(0x1018F47C), x);
        gabi::call(0x02038A3C, ld(0x1018F47C));
    }
    if (gabi::call<BOOL>(0x025E20F0)) return 0; /* mDoAud_isUsedHeapForStreamBuffer */
    if (i_this != 0) gabi::call(0x025B0F2C, i_this); /* dScnPly_ply_c::dScnPly_ply_c (in place) */
    gabi::Local<desc_l> desc;
    u32 d = gabi::ea(desc.get());
    gabi::call(0x0276CAA4, d);
    gabi::call(0x0276F9F0, d, (u32)(s32)(s16)ld16(ld(0x104A1F64)), 1);
    gabi::call(0x0276F9F0, d, (u32)(s32)(s16)ld16(ld(0x104A1F70)), 8);
    gabi::call(0x0276F9F0, d, (u32)(s32)(s16)ld16(ld(0x104A1460)), 1);
    u32 idxPtr = ld(0x104A145C);
    gabi::call(0x0276F9F0, d, (u32)(s32)(s16)ld16(idxPtr), 1);
    u32 sys = i_this + 0x1D4;
    u32 heap = gabi::call<u32>(0x0203E8F4);
    gabi::call(0x0276C3F8, sys, d, heap);
    gabi::Local<sstr_l> a, b;
    a->vt = 0x10052EEC;
    b->vt = 0x10052EEC;
    a->str = 0x1005326C;
    b->str = 0x10053280;
    gabi::call(0x026124B0, ld(0x101F4F7C), gabi::ea(a.get()), gabi::ea(b.get()), 0);
    u32 r = gabi::call<u32>(0x027E2DC0);
    u32 off = ld(r + 0x4C);
    u32 p = off != 0 ? r + 0x4C + off : 0; /* relative pointer */
    u32 q = gabi::call<u32>(0x027DFA24, p, 0x10053274);
    u32 off2 = ld(q);
    u32 p2 = off2 != 0 ? q + off2 : 0;
    gabi::Local<u32_l> out;
    u32 res = gabi::call<u32>(0x027A7558, gabi::ea(out.get()), p2);
    u32 v = ld(res);
    gabi::Local<u32x2_l> pair;
    pair->b = v;
    pair->a = v;
    gabi::call(0x0276B710, sys, gabi::ea(pair.get()) + 0, gabi::ea(pair.get()) + 4, (u32)-1, 1.0f);
    u32 obj = scene_entry(sys, idxPtr);
    if (ld(0x101FD7E0) == 0) { /* function-local static */
        st(0x101FD7E0, 1);
        st(0x101FDD14, 0x10052FAC);
    }
    if (obj != 0 && gabi::call_ptr<u32>(ld(ld(obj + 0x58) + 0x44), obj, 0x101FDD14u) == 0) obj = 0; /* isKindOf(&type descriptor 101FDD14): r4 is live at the bctrl (game test 2026-10-04) */
    gabi::call(0x0277019C, obj, 1);
    stf(obj + 0x13C, 1.0f);
    st(obj + 0xF4, 1);
    stf(obj + 0x16C, 1.0f);
    stf(obj + 0x15C, 10000.0f);
    stf(obj + 0x14C, 1.0f);
    stf(obj + 0x18C, 1000.0f);
    stf(obj + 0x120, 0.0f);
    stf(obj + 0x108, 8000.0f);
    stf(obj + 0x124, 0.0f);
    stf(obj + 0x11C, 0.0f);
    stf(obj + 0x104, 0.0f);
    stf(obj + 0x134, 0.0f);
    stf(obj + 0x10C, 0.0f);
    stf(obj + 0x138, 0.0f);
    gabi::call(0x0276C8B8, sys);
    st(0x104B4708, sys);
    st8(i_this + 0x930, ld8(i_this + 0x930) | 2);
    st(i_this + 0x900, 0);
    stf(i_this + 0x9F0, 15.0f);
    stf(i_this + 0x9F4, 15.0f);
    st(i_this + 0x8FC, 0);
    stf(i_this + 0xA04, 0.02f);
    st(i_this + 0x904, 0);
    stf(i_this + 0xA08, 0.01f);
    st8(i_this + 0xA80, 1);
    gabi::call(0x0272D16C, ld(0x101F8710));
    st8(0x101F4825, 0);
    if (i_this != 0 && (s16)ld16(i_this + 8) == 7) { /* fpcNm_PLAY_SCENE */
        heapSizeCheck_l();
        return 2;
    }
    heapSizeCheck_l();
    if (ld(ld(0x101F4974)) != 0) { /* mDoRst::isReset() */
        gabi::call(0x025E1D8C);           /* audio reset recovery */
        st8(0x101F4827, 0);               /* mDoGph_gInf_c::offFade() */
        gabi::call(0x025F0830);           /* mDoGph_gInf_c::offMonotone() */
        st8(0x101D616C, 0);               /* dDlst_list_c::offWipe() */
        gabi::call(0x02526570);           /* daTitle_proc_c::daTitle_Kirakira_Sound_flag_on() */
        gabi::call(0x023AB3D0);           /* daObjTribox::Act_c::reset() */
        st8(dComIfGp_ea() + 0x514C, 0);   /* dComIfGp_offEnableNextStage() */
    }
    gabi::call(0x025B9A18, ld(0x101F84DC) + 0x20); /* dComIfGs_init() */
    return 2;
}
VERIFY(0x025B0FB4, phase_00);

} // namespace d_s_play_3_cpp
