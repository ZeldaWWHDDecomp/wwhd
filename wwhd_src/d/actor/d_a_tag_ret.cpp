/**
 * d_a_tag_ret.cpp (WWHD)
 * Tag - Deku Leaf return area (sets the player's "return" flag for a link ID).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_ret.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x1003FC28 /* HD: daTagRet::Act_c vtable */
#define AAB_VTBL 0x1003FC18 /* cM3dGAab vtable (per TU) */
#define cyl_check_src gabi::at<dCcD_SrcCyl>(0x101D2058)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0254DA50 checkItemGet(u8 item, BOOL) (as in d_a_npc_bms1.h) */
static inline BOOL checkItemGet(u8 item, s32 flag) { return gabi::call<BOOL>(0x0254DA50, item, flag); }

enum { dItemNo_PEARL_FARORE_e = 0x6B };

namespace daTagRet {
struct Act_c : fopAc_ac_c {
    enum Prm_e { PRM_LINK_ID_W = 8, PRM_LINK_ID_S = 0 };
    cPhs_State _create();
    bool _execute();
    u32 prm_get_linkID();

    /* 0x3AC */ u8 m3AC[0x3B8 - 0x3AC];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mCyl, 0x3F4);
WWHD_SIZE(Act_c, 0x524);
}  // namespace daTagRet
using daTagRet::Act_c;

/* 024B2A14: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x024B2A14, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x024B2A14, PrmAbstract);
u32 Act_c::prm_get_linkID() { return PrmAbstract(this, PRM_LINK_ID_W, PRM_LINK_ID_S); }

/* 024B26CC */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x024B26CC, cPhs_State, this);
    /* fopAcM_ct(this, Act_c): inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (checkItemGet(dItemNo_PEARL_FARORE_e, TRUE)) {
        return 3; /* cPhs_STOP_e */
    }
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(cyl_check_src);
    mCyl.SetR(1000.0f * scale.x);
    mCyl.SetH(100.0f * scale.y);
    mCyl.SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024B26CC, &Act_c::_create);

/* 024B2828 */
bool Act_c::_execute() {
    WWHD_FUNC(0x024B2828, bool, this);
    mCyl.SetC(&current.pos);
    mCyl.SetR(1000.0f * scale.x);
    mCyl.SetH(100.0f * scale.y);
    dComIfG_Ccsp_Set(&mCyl);
    if (mCyl.ChkCoHit()) {
        /* daPy_getPlayerLinkActorClass()->onDekuSpReturnFlg(linkId): virtual (slot 0x8C) */
        u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR);
        u32 vt = gabi::load<u32>(link + 0xB4);
        u32 linkId = prm_get_linkID();
        gabi::call_ptr(gabi::load<u32>(vt + 0x8C), gabi::at<fopAc_ac_c>(link), (u8)linkId);
    }
    /* set_mtx(): empty */
    return true;
}
VERIFY(0x024B2828, &Act_c::_execute);

/* method table entries (HD: _delete and _draw inlined) */
/* 024B28F4 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x024B28F4, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x024B28F4, Mthd_Create);
/* 024B28F8 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x024B28F8, BOOL, i_this);
    return true; /* _delete() */
}
VERIFY(0x024B28F8, Mthd_Delete);
/* 024B2900 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x024B2900, BOOL, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x024B2900, Mthd_Execute);
/* 024B2904 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x024B2904, BOOL, i_this);
    return true; /* _draw() */
}
VERIFY(0x024B2904, Mthd_Draw);

/* 024B290C */
static void __sinit_d_a_tag_ret_cpp() {
    WWHD_FUNC(0x024B290C, void, (u32)0);
    sinit_header_statics(0x1046E634, 0x101D209C);
}
VERIFY(0x024B290C, __sinit_d_a_tag_ret_cpp);

/* 024B29A0: daTagRet::Act_c deleting destructor (inline member destructors) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x024B29A0, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024B29A0, Act_c_dt);

/* 024B2A0C */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x024B2A0C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B2A0C, Mthd_IsDelete);
