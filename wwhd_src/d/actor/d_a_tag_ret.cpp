/**
 * d_a_tag_ret.cpp (WWHD)
 * Tag - Deku Leaf return area (sets the player's "return" flag for a link ID).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_ret.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_ret.h"

static dCcD_SrcCyl cyl_check_src;
enum { dItemNo_PEARL_FARORE_e = 0x6B };
using daTagRet::Act_c;

/* 024B2A14 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    // HD: PowerPC srw/slw shift counts of 32..63 give zero.
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
u32 Act_c::prm_get_linkID() { return PrmAbstract(this, PRM_LINK_ID_W, PRM_LINK_ID_S); }

/* 024B26CC */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);
    if (checkItemGet(dItemNo_PEARL_FARORE_e, TRUE)) {
        return cPhs_STOP_e;
    }
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&cyl_check_src);
    mCyl.SetR(1000.0f * scale.x);
    mCyl.SetH(100.0f * scale.y);
    mCyl.SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}

/* 024B2828 */
bool Act_c::_execute() {
    mCyl.SetC(&current.pos);
    mCyl.SetR(1000.0f * scale.x);
    mCyl.SetH(100.0f * scale.y);
    dComIfG_Ccsp_Set(&mCyl);
    if (mCyl.ChkCoHit()) {
        // HD: Load the player vtable before extracting the return link ID.
        daPy_py_c* player = daPy_getPlayerLinkActorClass();
        u32 vtable = player->__vtbl;
        u32 linkId = prm_get_linkID();
        daPy_py_c_onDekuSpReturnFlg(player, vtable, (u8)linkId);
    }
    /* set_mtx(): empty */
    return true;
}

/* method table entries (HD: _delete and _draw inlined) */
/* 024B28F4 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 024B28F8 */
static BOOL Mthd_Delete(void* i_this) {
    return true; /* _delete() */
}
/* 024B2900 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 024B2904 */
static BOOL Mthd_Draw(void* i_this) {
    return true; /* _draw() */
}

/* 024B290C */
static void __sinit_d_a_tag_ret_cpp() {
    sinit_header_statics(0x1046E634, 0x101D209C);
}

/* 024B29A0 */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024B2A0C */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* WWHD glue
data cyl_check_src 101D2058
*/
