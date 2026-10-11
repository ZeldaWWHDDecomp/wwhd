/**
 * d_a_tag_ba1.cpp (WWHD)
 * Tag - Grandma (lets the player use a fairy bottle on Link's grandmother).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_ba1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_ba1.h"
const gc_tu_data gc_tu = { .headerStatics={0x1046E238, 0x101D17FC, 0x1046E254} };
static daTag_Ba1_HIO_c l_HIO;
static daTag_Ba1_HIO_c::hio_prm_c a_prm_tbl;
static const char* l_evn_tbl[] = {"Use_Fairy"};

/* 024A5C28 */
static daTag_Ba1_HIO_c* daTag_Ba1_HIO_c_ct(daTag_Ba1_HIO_c* hio) {
    if (hio == nullptr) {
        hio = (daTag_Ba1_HIO_c*)operator_new(0x10);
        if (hio == nullptr)
            return hio;
    }
    hio->__vtbl = 0x1003EF4C;
    memcpy_g(&hio->mPrm, &a_prm_tbl, 1);
    hio->mNo = -1;
    hio->mRefCount = -1;
    return hio;
}

/* 024A593C */
s16 daTag_Ba1_c::XyCheck_cB(int i_itemBtn) {
    return dComIfGp_getSelectItem(i_itemBtn) == dItemNo_FAIRY_BOTTLE_e;
}

/* 024A597C */
static s16 daTag_Ba1_XyCheck_cB(void* i_this, int i_itemBtn) {
    return static_cast<daTag_Ba1_c*>(i_this)->XyCheck_cB(i_itemBtn);
}

/* 024A5980 */
s16 daTag_Ba1_c::XyEvent_cB(int) {
    mEventIdx = 0;
    return mEventIds[0]; /* mEventIds[mEventIdx] */
}

/* 024A5994 */
static s16 daTag_Ba1_XyEvent_cB(void* i_this, int i_itemBtn) {
    return static_cast<daTag_Ba1_c*>(i_this)->XyEvent_cB(i_itemBtn);
}

/* 024A5998 */
bool daTag_Ba1_c::createInit() {
    bool needsInit = dComIfGs_isEventBit(UNK_0520);
    if (!needsInit) {
        return needsInit;
    }

    needsInit = !dComIfGs_isEventBit(GRANDMA_HEALED);
    if (needsInit) {
        attention_info.flags = fopAc_Attn_ACTION_SPEAK_e;
        attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x1A;
        mEventIds[0] = dComIfGp_evmng_getEventIdx(l_evn_tbl[0], 0xFF);
        eventInfo.setXyCheckCB(daTag_Ba1_XyCheck_cB);
        eventInfo.setXyEventCB(daTag_Ba1_XyEvent_cB);
    }

    return needsInit;
}

/* 024A5B60 */
BOOL daTag_Ba1_c::_execute() {
    int staffId = -1;
    if (dComIfGp_event_runCheck()) {
        if (!eventInfo.checkCommandTalk()) {
            staffId = dComIfGp_evmng_getMyStaffId("TagBa1", nullptr, 0);
        }
    }

    if (staffId >= 0) {
        s16 ev = mEventIds[mEventIdx];
        if (dComIfGp_evmng_endCheck(ev)) {
            dComIfGp_event_reset();
            fopAcM_delete(this);
        }
    }

    return TRUE;
}

/* 024A5B10 */
BOOL daTag_Ba1_c::_delete() {
    if (l_HIO.mRefCount >= 0 && (l_HIO.mRefCount = l_HIO.mRefCount - 1) < 0) {
        mDoHIO_deleteChild(l_HIO.mNo);
    }
    return TRUE;
}

/* 024A5A58 */
cPhs_State daTag_Ba1_c::_create() {
    if (l_HIO.mRefCount < 0) {
        l_HIO.mNo = mDoHIO_createChild("おばあちゃんタグ", &l_HIO);
    }
    l_HIO.mRefCount = l_HIO.mRefCount + 1;

    fopAcM_ct(this, daTag_Ba1_c);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return cPhs_COMPLEATE_e;
}

/* 024A5B0C */
static cPhs_State daTag_Ba1_Create(fopAc_ac_c* i_this) {
    return ((daTag_Ba1_c*)i_this)->_create();
}

/* 024A5B5C */
static BOOL daTag_Ba1_Delete(daTag_Ba1_c* i_this) {
    return i_this->_delete();
}

/* 024A5C14 */
static BOOL daTag_Ba1_Execute(daTag_Ba1_c* i_this) {
    return i_this->_execute();
}

/* 024A5C18 */
// HD: _draw inlined
static BOOL daTag_Ba1_Draw(daTag_Ba1_c* i_this) {
    return TRUE;
}

/* 024A5C20 */
static BOOL daTag_Ba1_IsDelete(daTag_Ba1_c*) {
    return TRUE;
}

/* 024A5C94 */
// HD: header statics, then l_HIO's constructor (HD: no destructor registration)
static void __sinit_d_a_tag_ba1_cpp() {
    __sinit_header_statics();
    daTag_Ba1_HIO_c_ct(&l_HIO);
}

/* 024A5D34 */
// HD: daTag_Ba1_c deleting destructor (compiler-generated)
static void daTag_Ba1_c_dt(daTag_Ba1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data l_HIO 1046E244
data a_prm_tbl 101D17F8
data l_evn_tbl 101D17D4
str 1003EF78 "\u304a\u3070\u3042\u3061\u3083\u3093\u30bf\u30b0" renamed # HD: Shift-JIS debug label
str 1003EF8C
*/
