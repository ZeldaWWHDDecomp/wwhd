/* d_s_play part 7: dScnPly_Delete. WWHD. 
 * See d_s_play.cpp for the unit's range and layouts.
 *
 * HD dScnPly_Delete first waits for an HD system (*1018F47C, state +0x18 == 1: not yet), then
 * tears the scene down in the GameCube order, also destroys the ocean heap and releases the HD
 * scene objects and the HD system selected by the scene mode (+0xB64), and finally runs the
 * scene's member and base destructors itself. */
#include "bindings.h"

namespace d_s_play_7_cpp {

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }

static constexpr u32 SSTR_VT = 0x10052EEC;
struct sstr_l { be<u32> str, vt; };
static inline void assure(u32 s) { gabi::call_ptr(ld(ld(s + 4) + 0x14), s); }

/* stage name == literal (literal assured twice, then the stage; pointer test, then at most
 * 0x40001 characters) */
static bool stage_is(gabi::Local<sstr_l>& lit, gabi::Local<sstr_l>& stage, u32 str) {
    lit->vt = SSTR_VT;
    lit->str = str;
    u32 play = dComIfGp_ea();
    stage->vt = SSTR_VT;
    stage->str = play + 0x5134;
    u32 a = gabi::ea(lit.get()), b = gabi::ea(stage.get());
    assure(a);
    assure(a);
    u32 s1 = ld(a);
    assure(b);
    u32 s2 = ld(b);
    if (s1 == s2) return true;
    for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
        u8 c = ld8(s1);
        if (c != ld8(s2)) return false;
        if (c == 0) return true;
    }
    return false;
}

/* 025B0630 */
static BOOL dScnPly_Delete(u32 i_this) {
    WWHD_FUNC(0x025B0630, BOOL, i_this);
    u32 sys = ld(0x1018F47C);
    if (sys != 0) {
        if (ld(sys + 0x18) == 1) return FALSE;
        gabi::call(0x0203899C, sys);
        u32 q = ld(0x1018F47C);
        u32 st80 = ld(ld(q + 0x1C) + 0x80);
        if (st80 == 3 || st80 == 4) gabi::call(0x020386CC, gabi::call<u32>(0x020389F0, q));
    }
    gabi::call(0x024EDB20, dComIfGp_ea() + 0x5804, 2); /* dComIfGp_getAttention().~dAttention_c() */
    gabi::call(0x025CC110, dComIfGp_ea() + 0x599C, 2); /* dComIfGp_getVibration().~dVibration_c() */
    u32 play = dComIfGp_ea();
    gabi::call_ptr(ld(ld(play + 0x26A0) + 0x1C), play + 0x12A0); /* dComIfG_Bgsp()->Dt() */
    u32 r = gabi::call<u32>(0x02518794, dComIfGp_ea() + 0x26A4); /* dComIfG_Ccsp()->Dt() */
    r = gabi::call<u32>(0x025BEF30, r);                           /* dSnap_Delete() */
    gabi::call(0x025C3370, r);                                    /* dStage_Delete() */
    gabi::call(0x0253EA70, dComIfGp_ea() + 0x51D0);               /* dComIfGp_event_remove() */
    r = gabi::call<u32>(0x025A809C, ld(dComIfGp_ea() + 0x5AB0));  /* dComIfGp_particle_removeScene() */
    r = gabi::call<u32>(0x02528B48, r);                           /* dComIfGp_removeDemo() */
    gabi::call(0x02591588, r);
    gabi::call(0x025DB8F4, ld(dComIfGp_ea() + 0x5C24));           /* fopMsgM_destroyExpHeap(dComIfGp_getExpHeap2D()) */
    gabi::Local<sstr_l> l0, s0, l1, s1, l2, s2, l3, s3;
    if (stage_is(l0, s0, 0x10053228) || stage_is(l1, s1, 0x10053230) || stage_is(l2, s2, 0x10053238) ||
        stage_is(l3, s3, 0x10053240)) /* "Xboss0".."Xboss3" */
        gabi::call(0x02522C60); /* dComIfGs_revPlayerRecollectionData() */
    gabi::call(0x02524C50, dComIfGp_ea() + 0x12A0); /* dComIfGp_removeMagma() */
    gabi::call(0x02524D3C, dComIfGp_ea() + 0x12A0); /* removeGrass */
    gabi::call(0x02524E3C, dComIfGp_ea() + 0x12A0); /* removeTree */
    gabi::call(0x02524F3C, dComIfGp_ea() + 0x12A0); /* removeWood */
    gabi::call(0x0252503C, dComIfGp_ea() + 0x12A0); /* removeFlower */
    play = dComIfGp_ea();
    st(play + 0x5B4C, 0);  /* dComIfGp_clearItemTimeCount() */
    st8(play + 0x5BB0, 0);
    s32 darkNo = (s8)ld8(0x1047B560);
    st(0x1047C89C + 0x10, (u32)-1); /* g_msgDHIO.field_0x10 */
    st8(0x1047C89C + 5, 0);         /* g_msgDHIO.field_0x06 */
    gabi::call(0x025F0A18, darkNo); /* mDoHIO_deleteChild(g_darkHIO.mNo) */
    gabi::call(0x025B05E4);         /* the HD ocean heap */
    st8(dComIfGp_ea() + 0x5AC9, 0); /* dComIfGp_setWindowNum(0) */
    s8 preLoadNo = (s8)ld8(0x101EACB4);
    if (preLoadNo >= 0) {
        u32 info = 0x100530D8 + preLoadNo * 0xC;
        u32 resName = ld(info + 4);
        u32 num = ld8(info + 8);
        if (resName != 0 && ld(resName) != 0 && (s32)num > 0) {
            u32 p = resName, off = 0;
            do {
                gabi::call(0x025204C8, 0x1047B448 + off, ld(p)); /* dComIfG_resDelete(&resPhase[i], resName[i]) */
                p += 4;
                off += 8;
            } while (--num != 0);
        }
    }
    gabi::call(0x0251FD14, dComIfGp_ea() + 0x12A0); /* dComIfGp_init() */
    u32 g = ld(0x101F95D0);
    st(0x104B4708, 0);
    u32 n = ld(g + 0x1020);
    u32 pp = ld(g + 0x1024);
    if (n > 1) pp += 4;
    u32 o = ld(pp);
    st(o + 0x6BCC, 0);
    st(o + 0x6BD0, 0);
    u32 mode = ld(i_this + 0xB64);
    if (mode == 0) {
        gabi::call(0x0271F3D8, gabi::call<u32>(0x0271F090, ld(0x101F83FC)));
    } else if (mode == 1) {
        u32 x = gabi::call<u32>(0x02715C10, ld(0x101F8344));
        if (ld8(0x101F8350) == 0) gabi::call(0x02717B70, x);
    } else if (mode == 2) {
        gabi::call(0x0271E6B8, gabi::call<u32>(0x0271DCC8, ld(0x101F83D0)));
        gabi::call(0x026FBC4C, ld(0x101F7274));
    } else if (mode == 3) {
        gabi::call(0x02708270, gabi::call<u32>(0x02707FA0, ld(0x101F8268)));
    }
    gabi::call(0x02792010, i_this + 0x5D4, 2);
    gabi::call(0x0276B21C, i_this + 0x1D4, 2);
    gabi::call(0x025DD630, i_this, 2); /* scene_class base destructor */
    return TRUE;
}
VERIFY(0x025B0630, dScnPly_Delete);

} // namespace d_s_play_7_cpp
