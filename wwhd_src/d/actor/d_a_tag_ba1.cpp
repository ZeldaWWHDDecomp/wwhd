/**
 * d_a_tag_ba1.cpp (WWHD)
 * Tag - Grandma (lets the player use a fairy bottle on Link's grandmother).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_ba1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_photo.h / d_a_npc_people.h) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; } /* as in d_a_swhit0 */
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }

#define TAGBA1_VTBL 0x1003EF5C     /* daTag_Ba1_c vtable (HD virtual destructor) */
#define HIO_VTBL 0x1003EF4C        /* daTag_Ba1_HIO_c vtable */
#define l_evn_tbl gabi::at<be<u32>>(0x101D17D4) /* {"Use_Fairy"} */
#define a_prm_tbl 0x101D17F8       /* daTag_Ba1_HIO_c::a_prm_tbl (HD: plain .data, no guard) */

enum { dItemNo_FAIRY_BOTTLE_e = 0x57 };
enum { UNK_0520 = 0x0520, GRANDMA_HEALED = 0x2A20 };

/* l_HIO (0x1046E244): HD layout {mNo, mRefCount, mPrm, vtable} (GHS puts the vptr last) */
struct daTag_Ba1_HIO_c {
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ u8 _1[3];
    /* 0x4 */ be<s32> mRefCount;
    /* 0x8 */ be<u8> mPrm;
    /* 0x9 */ u8 _9[3];
    /* 0xC */ be<u32> __vtbl;
};
WWHD_OFFSET(daTag_Ba1_HIO_c, __vtbl, 0xC);
#define l_HIO (*gabi::at<daTag_Ba1_HIO_c>(0x1046E244))

struct daTag_Ba1_c : fopAc_ac_c {
    s16 XyCheck_cB(int);
    s16 XyEvent_cB(int);
    bool createInit();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();

    /* 0x3AC */ be<s16> mEventIds[1];
    /* 0x3AE */ be<s16> mEventIdx;
};
WWHD_OFFSET(daTag_Ba1_c, mEventIdx, 0x3AE);
WWHD_SIZE(daTag_Ba1_c, 0x3B0);

/* 024A5C28 */
static daTag_Ba1_HIO_c* daTag_Ba1_HIO_c_ct(daTag_Ba1_HIO_c* hio) {
    WWHD_FUNC(0x024A5C28, daTag_Ba1_HIO_c*, hio);
    if (hio == nullptr) {
        hio = (daTag_Ba1_HIO_c*)operator_new(0x10);
        if (hio == nullptr)
            return hio;
    }
    hio->__vtbl = HIO_VTBL;
    memcpy_g(&hio->mPrm, gabi::at<void>(a_prm_tbl), 1);
    hio->mNo = -1;
    hio->mRefCount = -1;
    return hio;
}
VERIFY(0x024A5C28, daTag_Ba1_HIO_c_ct);

/* 024A593C */
s16 daTag_Ba1_c::XyCheck_cB(int i_itemBtn) {
    WWHD_FUNC(0x024A593C, s16, this, i_itemBtn);
    return dComIfGp_getSelectItem(i_itemBtn) == dItemNo_FAIRY_BOTTLE_e;
}
VERIFY(0x024A593C, &daTag_Ba1_c::XyCheck_cB);

/* 024A597C */
static s16 daTag_Ba1_XyCheck_cB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x024A597C, s16, i_this, i_itemBtn);
    return static_cast<daTag_Ba1_c*>(i_this)->XyCheck_cB(i_itemBtn);
}
VERIFY(0x024A597C, daTag_Ba1_XyCheck_cB);

/* 024A5980 */
s16 daTag_Ba1_c::XyEvent_cB(int) {
    WWHD_FUNC(0x024A5980, s16, this);
    mEventIdx = 0;
    return mEventIds[0]; /* mEventIds[mEventIdx] */
}
VERIFY(0x024A5980, &daTag_Ba1_c::XyEvent_cB);

/* 024A5994 */
static s16 daTag_Ba1_XyEvent_cB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x024A5994, s16, i_this, i_itemBtn);
    return static_cast<daTag_Ba1_c*>(i_this)->XyEvent_cB(i_itemBtn);
}
VERIFY(0x024A5994, daTag_Ba1_XyEvent_cB);

/* 024A5998 */
bool daTag_Ba1_c::createInit() {
    WWHD_FUNC(0x024A5998, bool, this);
    bool needsInit = dComIfGs_isEventBit(UNK_0520);
    if (!needsInit) {
        return needsInit;
    }

    needsInit = !dComIfGs_isEventBit(GRANDMA_HEALED);
    if (needsInit) {
        u32 a = gabi::ea(this);
        gabi::store<u32>(a + 0x39C, 8);     /* attention_info.flags = fopAc_Attn_ACTION_SPEAK_e */
        gabi::store<u8>(a + 0x38B, 0x1A);   /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
        const char* name = gabi::at<const char>(l_evn_tbl[0]);
        mEventIds[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
        gabi::store<u32>(a + 0x104, 0x024A597C); /* eventInfo.setXyCheckCB(daTag_Ba1_XyCheck_cB) */
        gabi::store<u32>(a + 0x100, 0x024A5994); /* eventInfo.setXyEventCB(daTag_Ba1_XyEvent_cB) */
    }

    return needsInit;
}
VERIFY(0x024A5998, &daTag_Ba1_c::createInit);

/* 024A5B60 */
BOOL daTag_Ba1_c::_execute() {
    WWHD_FUNC(0x024A5B60, BOOL, this);
    int staffId = -1;
    if (dComIfGp_event_runCheck()) {
        if (gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            staffId = dComIfGp_evmng_getMyStaffId(STR(0x1003EF8C) /* "TagBa1" */, nullptr, 0);
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
VERIFY(0x024A5B60, &daTag_Ba1_c::_execute);

/* 024A5B10 */
BOOL daTag_Ba1_c::_delete() {
    WWHD_FUNC(0x024A5B10, BOOL, this);
    if (l_HIO.mRefCount >= 0 && (l_HIO.mRefCount = l_HIO.mRefCount - 1) < 0) {
        mDoHIO_deleteChild(l_HIO.mNo);
    }
    return TRUE;
}
VERIFY(0x024A5B10, &daTag_Ba1_c::_delete);

/* 024A5A58 */
cPhs_State daTag_Ba1_c::_create() {
    WWHD_FUNC(0x024A5A58, cPhs_State, this);
    if (l_HIO.mRefCount < 0) {
        l_HIO.mNo = mDoHIO_createChild(STR(0x1003EF78) /* "おばあちゃんタグ" */, &l_HIO);
    }
    l_HIO.mRefCount = l_HIO.mRefCount + 1;

    /* fopAcM_ct(this, daTag_Ba1_c): HD vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TAGBA1_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A5A58, &daTag_Ba1_c::_create);

/* 024A5B0C */
static cPhs_State daTag_Ba1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024A5B0C, cPhs_State, i_this);
    return ((daTag_Ba1_c*)i_this)->_create();
}
VERIFY(0x024A5B0C, daTag_Ba1_Create);

/* 024A5B5C */
static BOOL daTag_Ba1_Delete(daTag_Ba1_c* i_this) {
    WWHD_FUNC(0x024A5B5C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x024A5B5C, daTag_Ba1_Delete);

/* 024A5C14 */
static BOOL daTag_Ba1_Execute(daTag_Ba1_c* i_this) {
    WWHD_FUNC(0x024A5C14, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x024A5C14, daTag_Ba1_Execute);

/* 024A5C18: _draw inlined */
static BOOL daTag_Ba1_Draw(daTag_Ba1_c* i_this) {
    WWHD_FUNC(0x024A5C18, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A5C18, daTag_Ba1_Draw);

/* 024A5C20 */
static BOOL daTag_Ba1_IsDelete(daTag_Ba1_c*) {
    WWHD_FUNC(0x024A5C20, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024A5C20, daTag_Ba1_IsDelete);

/* 024A5C94: header statics, then l_HIO's constructor (HD: no destructor registration) */
static void __sinit_d_a_tag_ba1_cpp() {
    WWHD_FUNC(0x024A5C94, void, (u32)0);
    sinit_header_statics_z(0x1046E238, 0x101D17FC, 0x1046E254);
    daTag_Ba1_HIO_c_ct(&l_HIO);
}
VERIFY(0x024A5C94, __sinit_d_a_tag_ba1_cpp);

/* 024A5D34: daTag_Ba1_c deleting destructor (compiler-generated) */
static void daTag_Ba1_c_dt(daTag_Ba1_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A5D34, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A5D34, daTag_Ba1_c_dt);
