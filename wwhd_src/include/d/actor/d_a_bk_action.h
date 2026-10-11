/* Bokoblin action bindings and HD-only stack layouts.
 * Existing bk_class/br_s layouts keep the GameCube field names at HD offsets. */
#ifndef D_A_BK_ACTION_H
#define D_A_BK_ACTION_H
#include "d/actor/d_a_bk_local.h"
#include "gc_api/d/d_com_inf_game.h"

// HD: the shared Bokoblin header defines br_s as opaque, so use a private prefix view.
struct bkActionBridgeSegment {
    /* 0x000 */ u8 _000[0x658];
    /* 0x658 */ be<f32> m3F4;
    /* 0x65C */ u8 _65C[0x664 - 0x65C];
    /* 0x664 */ be<s16> m400;
    /* 0x666 */ u8 _666[0x66A - 0x666];
    /* 0x66A */ be<u8> m406;
};
WWHD_OFFSET(bkActionBridgeSegment, m3F4, 0x658);
WWHD_OFFSET(bkActionBridgeSegment, m400, 0x664);
WWHD_OFFSET(bkActionBridgeSegment, m406, 0x66A);
WWHD_SIZE(bkActionBridgeSegment, 0x66B);
struct bkActionBridgeView : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<s16> mMoveProcMode;
    /* 0x3B6 */ u8 padding[2];
};
WWHD_OFFSET(bkActionBridgeView, mMoveProcMode, 0x3B4);
WWHD_SIZE(bkActionBridgeView, 0x3B8);

// HD: bindings used by this action split.
/* fopAcM_monsSeStart, HD inline: like fopAcM_seStart (actor and &eyePos checked), the process id
 * is read before dComIfGp_getReverb; 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
/* 025E1928 mDoAud_subBgmStop */
static inline void mDoAud_subBgmStop() { gabi::call(0x025E1928); }
/* fopAcM_onActor: dComIfGs_onActor(setID, home.roomNo) */
static inline void fopAcM_onActor(fopAc_ac_c* a) { dComIfGs_onActor(a->setID, a->home.roomNo); }

enum bkActionDisappearItem {
    daDisItem_IBALL_e = 0,
    daDisItem_NONE1_e = 1,
};
enum bkActionParticleID {
    ID_AK_JN_TUBA00 = 0xE,
    ID_IT_SN_O_MAGT_FCHIP = 0x8058,
    ID_IT_SN_O_FIREK_KASU = 0x8061,
};

enum { JA_SE_CV_BK_SURPRISE = 0x4830, JA_SE_CM_MD_PIYO = 0x50BC };
enum { UNK_0301 = 0x301, UNK_0480 = 0x480, COLORS_IN_HYRULE = 0x3802 };

enum {
    JA_SE_CV_BK_JUMP = 0x482F,
    JA_SE_CV_BK_FOUND_LINK = 0x482B,
    JA_SE_CM_BK_BB_LANDING = 0x5836,
    UNK_0004 = 0x4
};

static inline f32 REG12_F(int i) { return REG_F(12, i); }
static inline f32 REG14_F(int i) { return REG_F(14, i); }
enum { JA_SE_CV_BK_LOST_BOKO = 0x482C };
enum { fopAcStts_HD_80000_e = 0x80000 }; /* HD: cleared while hanging, set when the hang ends */

/* dBgS_GndChk (stack object, 0x54): HD layout, this TU's vtables (same as d_a_bk.cpp's
 * dBgS_GndChk_l) */
struct dBgS_GndChk_a {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40; /* dBgS_PolyPassChk */
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C; /* dBgS_GrpPassChk */
    /* 0x50 */ be<u32> mGrp;
};
WWHD_OFFSET(dBgS_GndChk_a, m_pos, 0x24);
WWHD_OFFSET(dBgS_GndChk_a, mPass, 0x44);
WWHD_OFFSET(dBgS_GndChk_a, mGrp, 0x50);
WWHD_SIZE(dBgS_GndChk_a, 0x54);
static void a_GndChk_ct(dBgS_GndChk_a* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    c->__vtbl_10 = gc_tu.gndChk.v10;
    c->__vtbl_20 = gc_tu.gndChk.v20;
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->__vtbl_4C = gc_tu.gndChk.v4C;
    c->__vtbl_40 = gc_tu.gndChk.v40;
    c->mpPolyPassChk = gabi::ea(&c->__vtbl_40);
    c->mpGrpPassChk = gabi::ea(&c->__vtbl_4C);
    c->mGrp = 1;
}
static void a_GndChk_dt(dBgS_GndChk_a* c) {
    c->__vtbl_20 = gc_tu.gndChk.v20;
    c->__vtbl_40 = gc_tu.gndChk.v40;
    c->__vtbl_4C = gc_tu.gndChkDt[2];
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
static inline f32 REG17_F(int i) { return REG_F(17, i); }
static inline s16 REG10_S(int i) { return REG_S(10, i); }
static inline void bkAction_correctPosition(dBgS_ObjAcch* a) { a->CrrPos(dComIfG_Bgsp()); }

/* HD: sead::SafeString comparison of the stage name (play+0x5134) with a literal, inlined:
 * the virtual at vtable +0x14 (cstr/termination check) is called twice on the literal's
 * SafeString and once on the stage name's, then the strings are compared (at most 0x40001 chars) */
static inline void SafeString_vcall(SafeString* s) {
    gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s);
}
static inline bool dComIfGp_checkStageName(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(name);
    a->__vtbl = gc_tu.safeString;
    u32 stage = dComIfGp_ea() + PLAY_OFF(mCurStage.mName); /* dStage_startStage name (HD) */
    gabi::Local<SafeString> b;
    b->__vtbl = gc_tu.safeString;
    b->mStringTop = stage;
    SafeString_vcall(a);
    SafeString_vcall(a);
    u32 pa = a->mStringTop;
    SafeString_vcall(b);
    u32 pb = b->mStringTop;
    if (pa == pb)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        u8 cb = gabi::load<u8>(pb + i);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false; /* GHS: the length limit counts as "not equal" here */
}
enum { JA_SE_CV_BK_ATTACK_L = 0x4826 };
static inline f32 REG8_F(int i) { return REG_F(8, i); }

/* attack_info_s tables: attack_info[] (pointers) at 0x10191528 */
struct attack_info_s {
    /* 0x00 */ be<s32> bckFileIdx;
    /* 0x04 */ be<f32> speed;
    /* 0x08 */ be<s32> soundFileIdx;
};
WWHD_OFFSET(attack_info_s, speed, 4);
WWHD_OFFSET(attack_info_s, soundFileIdx, 8);
WWHD_SIZE(attack_info_s, 12);
static inline attack_info_s* attack_info(s32 i) {
    return gabi::at<attack_info_s>(gabi::load<u32>(0x10191528 + 4 * i));
}
/* 025D9D0C fopAcM_setCarryNow(actor, BOOL) */
static inline void fopAcM_setCarryNow(fopAc_ac_c* a, BOOL stageLayer) {
    gabi::call(0x025D9D0C, a, stageLayer);
}
static inline f32 REG6_F(int i) { return REG_F(6, i); }
enum { JA_SE_CV_BK_FOUND_BOKO = 0x482E };
enum { AT_TYPE_UNK800 = 0x800, AT_TYPE_UNK2000 = 0x2000, dCcG_SE_UNK2 = 2, dCcG_SE_WOOD = 4 };

static inline void bkAction_anm_init(bk_class* a, int bck, f32 morf, u8 loop, f32 speed, int snd) {
    gabi::call(0x02098DA4, a, bck, morf, loop, speed, snd);
}
static inline void bkAction_wait_set(bk_class* a) { gabi::call(0x0209B264, a); }
#endif
