/**
 * d_a_npc_cb1.cpp (WWHD)
 * NPC - Makar (Korok cellist): create, heap, callbacks, init, draw, destructor, HIO and statics
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_cb1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_cb1.h"

/* ---- this TU's statics (HD addresses) ---- */
#define CB1_L_HIO 0x10466B98          /* l_HIO (daNpc_Cb1_HIO_c, 0xF0) */
#define CB1_SINIT_P 0x10466B2C        /* per-TU header statics */
#define CB1_SINIT_D 0x101BDA00
#define CB1_M_PLAYER_ROOM 0x101BDC2C  /* bool daNpc_Cb1_c::m_playerRoom */
#define CB1_M_FLYING 0x101BDC2D       /* bool daNpc_Cb1_c::m_flying */
#define CB1_M_FLYING_TIMER 0x101BDC28 /* s16 daNpc_Cb1_c::m_flyingTimer */
#define CB1_M_STATUS 0x101BDC2A       /* u16 daNpc_Cb1_c::m_status */
#define CB1_PMF_waitNpcAction 0x10018BE0 /* pointer-to-member constants (8 bytes, .data) */
#define CB1_PMF_shipNpcAction 0x10018C10
#define CB1_PMF_musicNpcAction 0x10018C48
#define CB1_L_EVENT_NAME_TBL 0x101BD9E0 /* l_eventNameTbl[5] */
#define CB1_L_OFFSET_ATT_POS 0x10466B48 /* nodeCallBack's function-local statics (guards 0x10466B84/88) */
#define CB1_L_OFFSET_EYE_POS 0x10466B54
#define CB1_L_NUT_OFFSET 0x10466B60     /* nutNodeCallBack's function-local statics (guards 0x10466B8C/90) */
#define CB1_L_NUT_BASE 0x10466B6C
#define CB1_SAFESTRING_VTBL 0x10018CBC /* this TU's sead::SafeString vtable */
#define CB1_L_CYL_SRC 0x101BD958
#define CB1_L_WIND_CYL_SRC 0x101BD99C
#define CB1_L_PARTNER 0x101CEF74      /* HD: a global pointer to the companion NPC (also Medli's), cleared by the destructor */

/* daNpc_Cb1_HIO_c (0xF0): HD vtable at 0, dNpc_HIO_c (HD 0x28, vtable last) at 8; the members
 * from 0x30 on keep their GameCube offsets */
struct daNpc_Cb1_HIO_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<f32> m04;                 /* dNpc_HIO_c */
    /* 0x0C */ be<s16> mMaxHeadX;
    /* 0x0E */ be<s16> mMaxBackboneX;
    /* 0x10 */ be<s16> mMaxHeadY;
    /* 0x12 */ be<s16> mMaxBackboneY;
    /* 0x14 */ be<s16> mMinHeadX;
    /* 0x16 */ be<s16> mMinBackboneX;
    /* 0x18 */ be<s16> mMinHeadY;
    /* 0x1A */ be<s16> mMinBackboneY;
    /* 0x1C */ be<s16> mMaxTurnStep;
    /* 0x1E */ be<s16> mMaxHeadTurnVel;
    /* 0x20 */ be<f32> mAttnYOffset;
    /* 0x24 */ be<s16> mMaxAttnAngleY;
    /* 0x26 */ be<u8> m22;
    /* 0x27 */ u8 _27[1];
    /* 0x28 */ be<f32> mMaxAttnDistXZ;
    /* 0x2C */ be<u32> mNpcVtbl;
    /* 0x30 */ be<f32> f[0x24];             /* GameCube 0x30..0xBC (floats; 0x94 is a byte array) */
    /* 0xC0 */ be<f32> field_0xC0;
    /* 0xC4 */ be<f32> field_0xC4;
    /* 0xC8 */ be<s16> h[0x13];             /* GameCube 0xC8..0xEC */
    /* 0xEE */ be<u8> mDamageTimer;
    /* 0xEF */ be<u8> field_0xEF;
};
WWHD_SIZE(daNpc_Cb1_HIO_l, 0xF0);
static inline daNpc_Cb1_HIO_l& cb1_l_HIO() { return *gabi::at<daNpc_Cb1_HIO_l>(CB1_L_HIO); }
#define HIO_F(off) f[((off) - 0x30) / 4]
#define HIO_H(off) h[((off) - 0xC8) / 2]

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
/* daPy_getPlayerLinkActorClass(): play + 0x5B34 */
static inline u32 daPy_getPlayerLinkActorClass_ea() { return gabi::load<u32>(dComIfGp_ea() + 0x5B34); }
/* daPy_lk_c::checkCarryActionNow() (inline): mCurProc (+0x65F0) is one of the carry procs */
static inline BOOL daPy_lk_checkCarryActionNow(u32 link) {
    u32 proc = gabi::load<u32>(link + 0x65F0);
    return proc == 0x72 || proc == 0x6F || proc == 0x71;
}
/* J3DModel::getAnmMtx(jnt): the joint matrices are marked dirty (kamome) */
static inline Mtx34* cb1_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag, fpcM_DrawPriority(this)) */
static inline void fopAcM_onDraw(fopAc_ac_c* a) {
    s32 prio = gabi::call<s32>(0x025DF2B8, a);
    gabi::call(0x025DA874, gabi::ea(a) + 0xDC, prio);
}
static inline void cLib_offsetPos(cXyz* out, const cXyz* base, s16 ang, const cXyz* ofs) {
    gabi::call(0x0200F9D4, out, base, ang, ofs);
}
static inline BOOL dComIfGs_isStageBossEnemy(s32 no) { return gabi::call<BOOL>(0x02520A84, no); }
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== */
static inline bool cb1_isStartStage(u32 lit) {
    gabi::Local<SafeString> sa;
    sa->mStringTop = lit;
    sa->__vtbl = CB1_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> sb;
    sb->mStringTop = stage;
    sb->__vtbl = CB1_SAFESTRING_VTBL;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    u32 pa = sa->mStringTop;
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}
/* mDoExt_bckAnm::mDoExt_bckAnm (inline; HD 0x8C) */
static inline void cb1_bckAnm_ct(u32 b) {
    gabi::call(0x027F2BC0, b, 0); /* J3DFrameCtrl::init */
    gabi::store<u32>(b + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, b + 0x14);
    gabi::store<u32>(b + 0x84, 0);
    gabi::store<u32>(b + 0x88, 0);
    gabi::store<u32>(b + 0x80, 0);
    gabi::store<u32>(b + 0x58, 0);
    gabi::store<u32>(b + 0x10, 0x10018D24);
    gabi::store<u32>(b + 0x7C, 0);
    gabi::store<u32>(b + 0x48, 0x1016D820);
}
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868 */
static inline J3DModel* j3dSys_getModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
/* J3DModel::setAnmMtx(jnt, m) */
static inline void cb1_setAnmMtx(J3DModel* model, s32 jntNo, const Mtx34* m) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    mtx_copy(gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30), m);
}
static inline bool cM3d_IsZero(f32 f) { return __builtin_fabsf(f) < 3.8146973e-06f; }
static inline void cb1_copyWords(u32 dst, u32 src, int n) {
    for (int i = 0; i < n; i++) gabi::store<u32>(dst + 4 * i, gabi::load<u32>(src + 4 * i));
}
static inline J3DModelData* cb1_getObjectRes(s32 idx) {
    return (J3DModelData*)dComIfG_getObjectRes(STR(0x10018E54) /* "Cb" */, idx, CB1_SAFESTRING_VTBL);
}
/* modelData->getJointName()->getIndex(name) */
static inline s8 cb1_getJointIndex(J3DModelData* d, u32 name) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    u32 tab = off != 0 ? h + 0x10 + off : 0;
    return (s8)gabi::call<s32>(0x027DF9B0, tab, name);
}
/* HD: getJointNodePointer(idx) is bounds-checked (count +4, array +8, 0x1C each); setCallBack at +8 */
static inline void cb1_setJointCallBack(J3DModelData* d, u16 idx, u32 cb) {
    u32 count = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (idx < count) p += idx * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
#define CB1_ASSERT_FILE STR(0x10018E60)
#define CB1_ASSERT_MODELDATA STR(0x10018E88) /* "modelData != 0" */
/* GHS pointer-to-member compare with a constant {d = 0, i = -1, f} */
static inline BOOL cb1_pmf_eq(ProcFunc_l& cur, u32 f) {
    return cur.i == -1 && (cur.i == 0 || (cur.d == 0 && cur.f == f));
}
/* dComIfGp_getSelectItem(btn): play + 0x5BBB + btn */
static inline u8 dComIfGp_getSelectItem(s32 btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }

/* 0222176C */
static BOOL daNpc_Cb1_IsDelete(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x0222176C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0222176C, daNpc_Cb1_IsDelete);

/* 0221ED00 daNpc_Cb1_XyEventCB (not named by the matcher): mEventIdx[2] */
static s16 daNpc_Cb1_XyEventCB(daNpc_Cb1_c* i_this, int) {
    WWHD_FUNC(0x0221ED00, s16, i_this, 0);
    return i_this->mEventIdx[2];
}
VERIFY(0x0221ED00, daNpc_Cb1_XyEventCB);

/* 0221EC9C */
static s16 daNpc_Cb1_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x0221EC9C, s16, i_this, i_itemBtn);
    /* daNpc_Cb1_c::XyCheckCB (inline) */
    if (dComIfGp_getSelectItem(i_itemBtn) == 0x22 /* dItemNo_WIND_WAKER_e */ && dComIfGs_isEventBit(0x1880)) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0221EC9C, daNpc_Cb1_XyCheckCB);

/* 02226F30 HD: the destructor runs through the vtable (framework) */
static BOOL daNpc_Cb1_Delete(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226F30, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02226F30, daNpc_Cb1_Delete);

/* 02226B9C */
static daNpc_Cb1_HIO_l* daNpc_Cb1_HIO_c_ct(daNpc_Cb1_HIO_l* i_this) {
    WWHD_FUNC(0x02226B9C, daNpc_Cb1_HIO_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Cb1_HIO_l*)operator_new(0xF0);
        if (i_this == nullptr) {
            return nullptr;
        }
    }
    i_this->__vtbl = 0x10018DDC;
    gabi::call(0x0259DA18, gabi::ea(i_this) + 8); /* dNpc_HIO_c::dNpc_HIO_c */
    i_this->m04 = -20.0f;
    i_this->mMaxHeadX = 0;
    i_this->mMaxHeadY = 0;
    i_this->mMaxBackboneX = 0xBB8;
    i_this->mMaxBackboneY = 0x1770;
    i_this->mMinHeadX = 0;
    i_this->mMinHeadY = 0;
    i_this->mMinBackboneX = -0x3E8;
    i_this->mMinBackboneY = -0x1770;
    i_this->mMaxTurnStep = 0x7D0;
    i_this->mMaxHeadTurnVel = 0x7D0;
    i_this->mAttnYOffset = 65.0f;
    i_this->mMaxAttnAngleY = 0x4000;
    i_this->m22 = 0;
    i_this->mMaxAttnDistXZ = 180.0f;
    i_this->HIO_F(0x30) = 50.0f;
    i_this->HIO_F(0x34) = 140.0f;  /* mPlayerChaseDistance */
    i_this->HIO_F(0x38) = 0.05f;   /* mChaseDistScale */
    i_this->HIO_F(0x3C) = 9.0f;    /* mMaxWalkSpeed */
    i_this->HIO_F(0x40) = 3.0f;    /* mMinWalkSpeed */
    i_this->HIO_F(0x44) = 0.4f;    /* mForwardAccel */
    i_this->HIO_H(0xE8) = 0x8FC;
    i_this->HIO_H(0xEA) = 0x320;
    i_this->HIO_H(0xEC) = 0x3;
    i_this->HIO_F(0x48) = 0.9f;    /* mDecelScale */
    i_this->HIO_F(0x4C) = 1.0f;    /* mMaxDecel */
    i_this->HIO_F(0x50) = 1.0f;    /* mDecel */
    i_this->HIO_F(0x54) = 0.45f;   /* mWalkAnmSpeedScale */
    i_this->HIO_F(0x58) = 0.9f;    /* mMaxWalkAnmSpeed */
    i_this->HIO_F(0x5C) = 50.0f;   /* mNpcFlyLaunchSpeedF */
    i_this->HIO_F(0x60) = 20.0f;   /* mNpcFlyLaunchSpeedY */
    i_this->HIO_F(0x64) = 8.0f;
    i_this->HIO_F(0x68) = -1.0f;   /* mHitSpeedScaleF */
    i_this->HIO_F(0x6C) = 3.5f;    /* mHitSpeedScaleY */
    i_this->HIO_F(0x70) = -4.0f;
    i_this->HIO_F(0x74) = -1.5f;
    i_this->HIO_F(0x78) = -6.0f;
    i_this->HIO_F(0x7C) = -7.6f;
    i_this->HIO_F(0x80) = 0.1f;
    i_this->HIO_F(0x84) = 0.2f;
    i_this->HIO_F(0x88) = 10.0f;   /* mStickWalkSpeedScale */
    i_this->HIO_H(0xC8) = -0x1000;
    i_this->HIO_H(0xCA) = 450;     /* mPlayerFlyTimer */
    i_this->HIO_H(0xCC) = 0x64;
    i_this->HIO_H(0xCE) = 0xF;
    i_this->HIO_H(0xD0) = 0x4E20;
    i_this->HIO_H(0xD2) = 0x190;
    i_this->HIO_H(0xD4) = 0x9C4;
    i_this->HIO_H(0xD6) = 0xC8;
    i_this->HIO_F(0x8C) = 0.0002f;
    i_this->HIO_F(0x90) = -15.0f;
    i_this->HIO_F(0x98) = 10.0f;
    i_this->HIO_F(0x9C) = 10.0f;
    i_this->HIO_F(0xA0) = -2.5f;
    i_this->HIO_H(0xD8) = 1;
    i_this->HIO_F(0xA4) = 100.0f;
    i_this->HIO_F(0xA8) = 6000.0f;
    i_this->HIO_H(0xE0) = 0xC8;
    i_this->HIO_F(0xAC) = 0.5f;    /* mStickFlySpeedScale */
    i_this->HIO_H(0xDA) = 0x2968;
    i_this->HIO_F(0xB0) = 34.0f;
    i_this->HIO_F(0xB4) = 2.0f;
    i_this->HIO_H(0xDC) = 60;      /* mNpcFlyTimer */
    i_this->HIO_H(0xDE) = 0xD;
    i_this->HIO_F(0xB8) = 2.0f;
    i_this->HIO_F(0xBC) = 50.0f;   /* mFlyLaunchSpeedY */
    i_this->field_0xC0 = 400.0f;
    i_this->HIO_H(0xE2) = 0x5;
    i_this->HIO_H(0xE4) = 0x14;
    i_this->HIO_H(0xE6) = 0xBB8;
    i_this->field_0xC4 = 20.0f;
    i_this->mDamageTimer = 60;
    i_this->field_0xEF = 0;
    i_this->mNo = -1;
    return i_this;
}
VERIFY(0x02226B9C, daNpc_Cb1_HIO_c_ct);

/* 02226E7C __sinit_d_a_npc_cb1_cpp (new: compiler-generated) */
static void __sinit_d_a_npc_cb1_cpp() {
    WWHD_FUNC(0x02226E7C, void);
    sinit_header_statics(CB1_SINIT_P, CB1_SINIT_D);
    daNpc_Cb1_HIO_c_ct(&cb1_l_HIO());
}
VERIFY(0x02226E7C, __sinit_d_a_npc_cb1_cpp);

/* 02226F1C deleting destructor of an empty class (this TU's copy; new: compiler-generated) */
static void cb1_empty_dtor(void* i_this, s32 flags) {
    WWHD_FUNC(0x02226F1C, void, i_this, flags);
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}
VERIFY(0x02226F1C, cb1_empty_dtor);

/* 02226F38 dBgS_AcchCir deleting destructor (this TU's copy; new: compiler-generated) */
static void cb1_AcchCir_dtor(dBgS_AcchCir* i_this, s32 flags) {
    WWHD_FUNC(0x02226F38, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02018034, gabi::ea(i_this) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}
VERIFY(0x02226F38, cb1_AcchCir_dtor);

/* 022270A0 cLib_getRndValue<s16> (this TU's copy) */
static s16 cb1_cLib_getRndValue_s(s16 i_min, s16 i_range) {
    WWHD_FUNC(0x022270A0, s16, i_min, i_range);
    f32 base = (f32)i_min;
    return (s16)gabi::ftoi(gabi::fadds_ppc(base, cM_rndF((f32)i_range)));
}
VERIFY(0x022270A0, cb1_cLib_getRndValue_s);

/* ---- this TU's copies of daPy_py_c inline virtuals (new: compiler-generated) ---- */
static s32 cb1_inline_02226F8C(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226F8C, s32, i_this);
    return -1;
}
VERIFY(0x02226F8C, cb1_inline_02226F8C);
static s32 cb1_inline_02226F94(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226F94, s32, i_this);
    return 0;
}
VERIFY(0x02226F94, cb1_inline_02226F94);
static s32 cb1_inline_02226F9C(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226F9C, s32, i_this);
    return 0;
}
VERIFY(0x02226F9C, cb1_inline_02226F9C);
static s32 cb1_inline_02226FA4(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FA4, s32, i_this);
    return 0;
}
VERIFY(0x02226FA4, cb1_inline_02226FA4);
static s32 cb1_inline_02226FAC(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FAC, s32, i_this);
    return 0;
}
VERIFY(0x02226FAC, cb1_inline_02226FAC);
static s32 cb1_inline_02226FB4(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FB4, s32, i_this);
    return 0;
}
VERIFY(0x02226FB4, cb1_inline_02226FB4);
static s32 cb1_inline_02226FBC(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FBC, s32, i_this);
    return 0;
}
VERIFY(0x02226FBC, cb1_inline_02226FBC);
static s32 cb1_inline_02226FC4(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FC4, s32, i_this);
    return 0;
}
VERIFY(0x02226FC4, cb1_inline_02226FC4);
static s32 cb1_inline_02226FCC(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FCC, s32, i_this);
    return 0;
}
VERIFY(0x02226FCC, cb1_inline_02226FCC);
static s32 cb1_inline_02226FD4(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FD4, s32, i_this);
    return 0;
}
VERIFY(0x02226FD4, cb1_inline_02226FD4);
static void cb1_inline_02226FDC(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FDC, void, i_this);
}
VERIFY(0x02226FDC, cb1_inline_02226FDC);
static s32 cb1_inline_02226FE0(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FE0, s32, i_this);
    return 0;
}
VERIFY(0x02226FE0, cb1_inline_02226FE0);
static s32 cb1_inline_02226FE8(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FE8, s32, i_this);
    return -1;
}
VERIFY(0x02226FE8, cb1_inline_02226FE8);
static s32 cb1_inline_02226FF0(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FF0, s32, i_this);
    return -1;
}
VERIFY(0x02226FF0, cb1_inline_02226FF0);
static s32 cb1_inline_02226FF8(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02226FF8, s32, i_this);
    return -1;
}
VERIFY(0x02226FF8, cb1_inline_02226FF8);
static s32 cb1_inline_02227000(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227000, s32, i_this);
    return 0;
}
VERIFY(0x02227000, cb1_inline_02227000);
static s32 cb1_inline_02227008(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227008, s32, i_this);
    return 0;
}
VERIFY(0x02227008, cb1_inline_02227008);
static s32 cb1_inline_02227010(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227010, s32, i_this);
    return 0;
}
VERIFY(0x02227010, cb1_inline_02227010);
static s32 cb1_inline_02227018(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227018, s32, i_this);
    return 0;
}
VERIFY(0x02227018, cb1_inline_02227018);
static void cb1_inline_02227020(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227020, void, i_this);
}
VERIFY(0x02227020, cb1_inline_02227020);
static void cb1_inline_02227024(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227024, void, i_this);
}
VERIFY(0x02227024, cb1_inline_02227024);
static void cb1_inline_02227028(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227028, void, i_this);
}
VERIFY(0x02227028, cb1_inline_02227028);
static s32 cb1_inline_0222702C(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x0222702C, s32, i_this);
    return 0;
}
VERIFY(0x0222702C, cb1_inline_0222702C);
static void cb1_inline_02227034(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227034, void, i_this);
}
VERIFY(0x02227034, cb1_inline_02227034);
static void cb1_inline_02227038(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227038, void, i_this);
}
VERIFY(0x02227038, cb1_inline_02227038);
static void cb1_inline_0222703C(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x0222703C, void, i_this);
}
VERIFY(0x0222703C, cb1_inline_0222703C);
static s32 cb1_inline_02227040(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227040, s32, i_this);
    return 0;
}
VERIFY(0x02227040, cb1_inline_02227040);
static s32 cb1_inline_0222707C(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x0222707C, s32, i_this);
    return 1;
}
VERIFY(0x0222707C, cb1_inline_0222707C);
static s32 cb1_inline_02227084(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227084, s32, i_this);
    return 1;
}
VERIFY(0x02227084, cb1_inline_02227084);
static s32 cb1_inline_0222708C(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x0222708C, s32, i_this);
    return 1;
}
VERIFY(0x0222708C, cb1_inline_0222708C);
static s32 cb1_inline_02227094(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227094, s32, i_this);
    return 1;
}
VERIFY(0x02227094, cb1_inline_02227094);
static void cb1_inline_02227078(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227078, void, i_this);
}
VERIFY(0x02227078, cb1_inline_02227078);
/* sead::SafeString::assureTerminationImpl_ (empty) */
static void cb1_inline_0222709C(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x0222709C, void, i_this);
}
VERIFY(0x0222709C, cb1_inline_0222709C);

/* 02227048 getGroundY(): mAcch.GetGroundH() */
static f32 cb1_getGroundY(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227048, f32, i_this);
    return gabi::load<f32>(gabi::ea(i_this) + 0x4D0); /* mAcch (0x43C) + 0x94: ground height */
}
VERIFY(0x02227048, cb1_getGroundY);
/* 02227050 getLeftHandMatrix(): cullMtx */
static u32 cb1_getLeftHandMatrix(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227050, u32, i_this);
    return i_this->cullMtx;
}
VERIFY(0x02227050, cb1_getLeftHandMatrix);
/* 02227058 getRightHandMatrix(): cullMtx */
static u32 cb1_getRightHandMatrix(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227058, u32, i_this);
    return i_this->cullMtx;
}
VERIFY(0x02227058, cb1_getRightHandMatrix);
/* 02227060 getBaseAnimeFrameRate() */
static f32 cb1_getBaseAnimeFrameRate(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02227060, f32, i_this);
    return 1.0f;
}
VERIFY(0x02227060, cb1_getBaseAnimeFrameRate);
/* 0222706C getBaseAnimeFrame() */
static f32 cb1_getBaseAnimeFrame(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x0222706C, f32, i_this);
    return 0.0f;
}
VERIFY(0x0222706C, cb1_getBaseAnimeFrame);

/* 02221774 daNpc_Cb1_c::~daNpc_Cb1_c (deleting destructor). HD: no HIO child to delete; the
 * global companion pointer is cleared when it is this actor */
static void daNpc_Cb1_c_dtor(daNpc_Cb1_c* i_this, s32 flags) {
    WWHD_FUNC(0x02221774, void, i_this, flags);
    if (i_this == nullptr) {
        return;
    }
    u32 t = gabi::ea(i_this);
    i_this->__vtbl = CB1_VTBL;
    dComIfG_resDelete(&i_this->mPhs, STR(0x10018FA8) /* "Cb" */);
    if (i_this->heap.get() != nullptr && i_this->mpMorf.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    gabi::store<u8>(CB1_M_PLAYER_ROOM, 0); /* offFlying(), offPlayerRoom() */
    gabi::store<u8>(CB1_M_FLYING, 0);
    gabi::call(0x0221FECC, i_this); /* musicStop() */
    if (gabi::load<u32>(CB1_L_PARTNER) == t) {
        gabi::store<u32>(CB1_L_PARTNER, 0);
    }
    /* member destructors */
    gabi::call(0x02515A70, &i_this->mWindCyl, 2); /* dCcD_Cyl::~dCcD_Cyl (matcher: dBgS_Acch::~dBgS_Acch) */
    gabi::call(0x02515A70, &i_this->mCyl, 2);
    gabi::call(0x02515860, &i_this->mStts, 2);    /* dCcD_Stts::~dCcD_Stts */
    gabi::call(0x028F0164, &i_this->mAcchCir, 2, 0x40, 0x02226F38, 0); /* __destroy_arr(mAcchCir, 2, 0x40, ~dBgS_AcchCir, flags 0) */
    gabi::call(0x027F3628, t + 0x6C4, 0);         /* mNutBckAnim (frame control part) */
    gabi::call(0x027F3628, t + 0x638, 0);         /* mPropellerBckAnim */
    gabi::call(0x0244513C, i_this, 0);            /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}
VERIFY(0x02221774, daNpc_Cb1_c_dtor);

/* 02221894 isTagCheckOK (not named by the matcher) */
BOOL daNpc_Cb1_c::isTagCheckOK() {
    WWHD_FUNC(0x02221894, BOOL, this);
    if (cb1_pmf_eq(mNpcAction, 0x02223048 /* waitNpcAction */) || cb1_pmf_eq(mNpcAction, 0x022244A0 /* searchNpcAction */)) {
        return TRUE;
    }
    if (cb1_pmf_eq(mNpcAction, 0x02223694 /* carryNpcAction */) && !daPy_lk_checkCarryActionNow(daPy_getPlayerLinkActorClass_ea())) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02221894, &daNpc_Cb1_c::isTagCheckOK);

/* 0221F140 HD: GHS pointers to member (index compared first) */
BOOL daNpc_Cb1_c::setAction(ProcFunc_l* cur, ProcFunc_l* newFunc, void* arg) {
    WWHD_FUNC(0x0221F140, BOOL, this, cur, newFunc, arg);
    gabi::store<u8>(CB1_M_FLYING, 0);        /* offFlying() */
    gabi::store<s16>(CB1_M_FLYING_TIMER, 0); /* setFlyingTimer(0) */
    s16 newI = newFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0) {
            return TRUE;
        }
        newD = newFunc->d;
        newF = newFunc->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF)) {
            return TRUE;
        }
    } else {
        newF = newFunc->f;
        newD = newFunc->d;
        if (cur->i == 0) {
            goto set;
        }
    }
    m8F0 = -1;
    md_pmf_call<BOOL>(this, cur, arg);
set:
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    m8F0 = 0;
    m8F4 = 0;
    m8F6 = 0;
    m8F8 = 0;
    m8FA = 0;
    m8FC = 0.0f;
    m8F1 = 0;
    m8F2 = 0;
    shape_angle.x = 0;
    shape_angle.z = 0;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0); /* attention_info.flags */
    md_pmf_call<BOOL>(this, cur, arg);
    m8F0 = m8F0 + 1;
    return TRUE;
}
VERIFY(0x0221F140, &daNpc_Cb1_c::setAction);

/* 0221F2C4 */
void daNpc_Cb1_c::setNpcAction(ProcFunc_l* actionFunc, void* arg) {
    WWHD_FUNC(0x0221F2C4, void, this, actionFunc, arg);
    mPlayerAction.d = 0;
    mPlayerAction.i = 0;
    mPlayerAction.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mNpcAction, fn, arg);
}
VERIFY(0x0221F2C4, &daNpc_Cb1_c::setNpcAction);

/* 0221F314 */
BOOL daNpc_Cb1_c::shipRideCheck() {
    WWHD_FUNC(0x0221F314, BOOL, this);
    if (dComIfGs_isEventBit(0x1604) && gabi::load<u32>(dComIfGp_ea() + 0x5B3C) /* dComIfGp_getShipActor() */ != 0) {
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, CB1_PMF_shipNpcAction);
        setNpcAction(fn, nullptr);
        gabi::store<u16>(CB1_M_STATUS, (u16)(gabi::load<u16>(CB1_M_STATUS) | daCbStts_SHIP_RIDE)); /* onShipRide() */
        fopAcM_offDraw(this);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0221F314, &daNpc_Cb1_c::shipRideCheck);

/* 0221EC98 CheckCreateHeap (not named by the matcher) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0221EC98, BOOL, i_this);
    return gabi::call<BOOL>(0x0221E6E4, i_this); /* createHeap() */
}
VERIFY(0x0221EC98, CheckCreateHeap);

/* 022213BC HD: no shadows (the GameCube blob shadow and the real shadows of the cello, the stick
 * and the propeller are gone) */
BOOL daNpc_Cb1_c::draw() {
    WWHD_FUNC(0x022213BC, BOOL, this);
    u32 t = gabi::ea(this);
    if (gabi::load<u16>(CB1_M_STATUS) & daCbStts_SHIP_RIDE) {
        u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
        if (ship != 0 && (gabi::load<u32>(ship + 0x644) & 0x200000) /* checkHeadNoDraw() */) {
            return TRUE;
        }
    } else if (!(actor_status & 0x2000) /* !fopAcM_checkCarryNow(this) */) {
        s32 homeRoomNo = gabi::load<s8>(t + 0x2FE);
        if (homeRoomNo < 0) {
            return TRUE;
        }
        dComIfGp_ea();
        if (!(gabi::load<u8>(0x1047E8E8 + homeRoomNo * 0x22C) & 0x10)) { /* dComIfGp_roomControl_checkStatusFlag(room, 0x10) */
            return TRUE;
        }
    }

    J3DModel* pModel = mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    gabi::call(0x02445A00, this); /* daPy_npc_c::drawDamageFog */
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mpMorf->updateDL();

    J3DModel* pFaceModel = mpFaceModel;
    setLightTevColorType(dKy_getEnvlight(), pFaceModel, &tevStr);
    J3DModel_setBaseTRMtx(pFaceModel, cb1_getAnmMtx(pModel, m_backbone_jnt_num));
    mDoExt_modelUpdateDL(pFaceModel, 0);

    u16 status = gabi::load<u16>(CB1_M_STATUS);
    if (status & daCbStts_MUSIC) {
        setLightTevColorType(dKy_getEnvlight(), mpStickModel, &tevStr);
        J3DModel_setBaseTRMtx(mpStickModel, cb1_getAnmMtx(pModel, m_armRend_jnt_num));
        mDoExt_modelUpdateDL(mpStickModel, 0);
        setLightTevColorType(dKy_getEnvlight(), mpCelloModel, &tevStr);
        J3DModel_setBaseTRMtx(mpCelloModel, cb1_getAnmMtx(pModel, m_armL2_jnt_num));
        mDoExt_modelUpdateDL(mpCelloModel, 0);
    } else if (u32 prop = gabi::ea(mpPropellerModel.get()); prop != 0 && gabi::call<BOOL>(0x0221FC6C, this) /* isFlyAction() */ && m8DC != 2) {
        f32 frame = 11.0f;
        if (m8DC == 4) {
            frame = gabi::load<f32>(gabi::ea(mpMorf.get()) + 0x9C); /* mpMorf->getFrame() */
        }
        mPropellerBckAnim.entry(gabi::at<J3DModelData>(gabi::load<u32>(prop + 0xAC)), frame);
        setLightTevColorType(dKy_getEnvlight(), mpPropellerModel, &tevStr);
        mDoExt_modelUpdateDL(mpPropellerModel, 0);
    } else if (mpNutModel.get() != nullptr && (status & daCbStts_NUT)) {
        mNutBckAnim.entry(gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(mpNutModel.get()) + 0xAC)),
                          gabi::load<f32>(t + 0x6B8) /* mNutBckAnim frame */);
        setLightTevColorType(dKy_getEnvlight(), mpNutModel, &tevStr);
        mDoExt_modelUpdateDL(mpNutModel, 0);
    }

    dSnap_RegistFig(0x9C /* DSNAP_TYPE_NPC_CB1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x022213BC, &daNpc_Cb1_c::draw);

/* 02221768 */
static BOOL daNpc_Cb1_Draw(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02221768, BOOL, i_this);
    return i_this->draw();
}
VERIFY(0x02221768, daNpc_Cb1_Draw);

/* 0221FBD0 */
static cPhs_State daNpc_Cb1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0221FBD0, cPhs_State, i_this);
    return gabi::call<cPhs_State>(0x0221F700, i_this); /* create() */
}
VERIFY(0x0221FBD0, daNpc_Cb1_Create);

/* 0221ED08 HD: the attention and eye positions are placed here (GameCube: in execute) */
void daNpc_Cb1_c::setBaseMtx() {
    WWHD_FUNC(0x0221ED08, void, this);
    J3DModel* pModel = mpMorf->getModel();
    u32 t = gabi::ea(this);
    if (gabi::load<u16>(CB1_M_STATUS) & daCbStts_SHIP_RIDE) {
        u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
        if (ship != 0) {
            /* pShip->getHeadJntMtx(): joint 8 of the ship's model */
            J3DModel* shipModel = gabi::at<J3DModel>(gabi::load<u32>(gabi::load<u32>(ship + 0x3B8) + 0x90));
            PSMTXCopy(cb1_getAnmMtx(shipModel, 8), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(10.92f, 0.57f, -14.0f);
            gabi::call(0x025F19F8, mDoMtx_stack_c::get(), (s16)-0x3AAA, (s16)0, (s16)0x40FE); /* XYZrotM */
            J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
            Mtx34* pMtx = mDoMtx_stack_c::get();
            current.pos.x = pMtx->m[0][3];
            current.pos.y = pMtx->m[1][3];
            current.pos.z = pMtx->m[2][3];
            shape_angle.y = (s16)(gabi::load<s16>(ship + 0x32A) + 0x8000);
            actor_status = actor_status | 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
            gabi::store<u8>(t + 0x1C9, gabi::load<u8>(ship + 0x1C9)); /* tevStr.mRoomNo */
            gabi::store<u8>(t + 0x1CA, gabi::load<u8>(ship + 0x1CA)); /* tevStr.mEnvrIdxOverride */
            gabi::store<u32>(ship + 0x644, gabi::load<u32>(ship + 0x644) | 0x40000000); /* pShip->onCb1Ride() */
            fopAcM_onDraw(this);
        }
    } else {
        J3DModel_setBaseScale(pModel, &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y + 25.0f, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        mDoMtx_stack_c::transM(m904.x, m904.y, m904.z);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), shape_angle.x);
        mDoMtx_stack_c::transM(0.0f, -25.0f, 0.0f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), shape_angle.z);
        J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
        if (mpNutModel.get() != nullptr && (gabi::load<u16>(CB1_M_STATUS) & daCbStts_NUT)) {
            J3DModel_setBaseTRMtx(mpNutModel, mDoMtx_stack_c::get());
        }
        if (mpPropellerModel.get() != nullptr) {
            mDoMtx_stack_c::transM(0.0f, 24.8f, 1.5f);
            J3DModel_setBaseTRMtx(mpPropellerModel, mDoMtx_stack_c::get());
            J3DModel_calc(mpPropellerModel);
        }
    }
    daNpc_Cb1_HIO_l& hio = cb1_l_HIO();
    {
        gabi::Local<cXyz> ofs;
        ofs->x = 0.0f;
        ofs->y = hio.mAttnYOffset;
        ofs->z = 0.0f;
        cLib_offsetPos(gabi::at<cXyz>(t + 0x390) /* attention_info.position */, &current.pos, shape_angle.y, ofs.get());
    }
    {
        gabi::Local<cXyz> ofs;
        ofs->x = 0.0f;
        ofs->y = hio.HIO_F(0x30);
        ofs->z = 0.0f;
        cLib_offsetPos(gabi::at<cXyz>(t + 0x37C) /* eyePos */, &current.pos, shape_angle.y, ofs.get());
    }
}
VERIFY(0x0221ED08, &daNpc_Cb1_c::setBaseMtx);

/* 0221F3C0 */
BOOL daNpc_Cb1_c::init() {
    WWHD_FUNC(0x0221F3C0, BOOL, this);
    u32 t = gabi::ea(this);
    daNpc_Cb1_HIO_l& hio = cb1_l_HIO();
    gravity = hio.HIO_F(0x70);
    gabi::store<u8>(CB1_M_PLAYER_ROOM, 0);   /* offPlayerRoom() */
    gabi::store<u8>(CB1_M_FLYING, 0);        /* offFlying() */
    gabi::store<s16>(CB1_M_FLYING_TIMER, 0); /* setFlyingTimer(0) */
    gabi::store<u16>(CB1_M_STATUS, 0);
    gabi::store<u8>(t + 0x389, 0xAB); /* attention_info.distances[TALK] */
    gabi::store<u8>(t + 0x38B, 0xAB); /* [SPEAK] */
    gabi::store<u8>(t + 0x38C, 8);    /* [CARRY] */

    for (int i = 0; i < 5; i++) {
        mEventIdx[i] = dComIfGp_evmng_getEventIdx(STR(gabi::load<u32>(CB1_L_EVENT_NAME_TBL + i * 4)), 0xFF);
    }
    m8E3 = -1;
    m8DD = -1;
    gabi::store<u32>(t + 0x104, 0x0221EC9C); /* eventInfo.setXyCheckCB(daNpc_Cb1_XyCheckCB) */
    gabi::store<u32>(t + 0x100, 0x0221ED00); /* eventInfo.setXyEventCB(daNpc_Cb1_XyEventCB) */
    gabi::store<u32>(t + 0x3BC, gabi::load<u32>(t + 0x3BC) | 0x40); /* onNpcNotChange() */

    s32 prm = gabi::load<s32>(t + 0xB0); /* fopAcM_GetParam(this) */
    if (prm == 5 || prm == 1 || prm == 2) { /* isTypeKazeBoss() || isTypeForest() || isTypeWaterFall() */
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, CB1_PMF_musicNpcAction);
        setNpcAction(fn, nullptr);
        gabi::store<s8>(t + 0x2FE, -1); /* home.roomNo */
        gravity = -0.1f;
        gabi::store<s8>(t + 0x326, -1); /* current.roomNo */
    } else if (prm == 0) { /* isTypeBossDie() */
        gabi::Local<ProcFunc_l> fn;
        fn->d = 0;
        fn->i = -1;
        fn->f = dComIfGs_isStageBossEnemy(4 /* STAGE_FW */) ? 0x02223048 /* waitNpcAction */ : 0x02224BC0 /* rescueNpcAction */;
        gabi::Local<ProcFunc_l> arg;
        md_pmf_load(arg, gabi::ea(fn.get()));
        setNpcAction(arg, nullptr);
    } else if (prm != 3 && prm != 4 && shipRideCheck()) { /* !isTypeEkaze() && !isTypeKaze() */
        gabi::store<u8>(t + 0x38B, 0xAF);
        gabi::store<u8>(t + 0x389, 0xAF);
    } else {
        if (dComIfGs_isEventBit(0x1610)) {
            gabi::store<u32>(t + 0x3BC, gabi::load<u32>(t + 0x3BC) & ~0x40u); /* offNpcNotChange() */
        }
        if (fopAcM_GetParam(this) == 3) { /* isTypeEkaze() */
            gabi::store<u32>(t + 0x3BC, gabi::load<u32>(t + 0x3BC) | 2); /* onNpcCallCommand() */
        }
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, CB1_PMF_waitNpcAction);
        setNpcAction(fn, nullptr);
        gabi::store<u16>(CB1_M_STATUS, (u16)(gabi::load<u16>(CB1_M_STATUS) & ~daCbStts_MUSIC)); /* offMusic() */
    }

    mStts.Init(0xFE, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(CB1_L_CYL_SRC));
    mCyl.SetStts(&mStts);
    mWindCyl.Set(gabi::at<dCcD_SrcCyl>(CB1_L_WIND_CYL_SRC));
    mWindCyl.SetStts(&mStts);
    return TRUE;
}
VERIFY(0x0221F3C0, &daNpc_Cb1_c::init);

/* 0221F700 HD: no HIO child; the actor is published in the global companion pointer; the restart
 * option from the priest position also rewrites the priest record */
cPhs_State daNpc_Cb1_c::create() {
    WWHD_FUNC(0x0221F700, cPhs_State, this);
    u32 t = gabi::ea(this);
    /* fopAcM_ct(this, daNpc_Cb1_c): the constructor, inlined */
    if (!(actor_condition & 8)) {
        if (t != 0) {
            gabi::call(0x024450B4, this); /* daPy_npc_c::daPy_npc_c */
            __vtbl = CB1_VTBL;
            cb1_bckAnm_ct(t + 0x628); /* mPropellerBckAnim */
            cb1_bckAnm_ct(t + 0x6B4); /* mNutBckAnim */
            gabi::call(0x028EFFD0, &mAcchCir, 2, 0x40, 0x024EFE94); /* __construct_array(dBgS_AcchCir) */
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10018CF4);
            dCcD_Cyl_ct(&mWindCyl, 0x10018CF4);
            gabi::call(0x0259DAA0, &mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            mPolyInfo.mpBgW = 0;
            mPolyInfo.__vtbl = 0x10018D04;
            mPolyInfo.mPolyIndex = 0xFFFF;
            mPolyInfo.mBgIndex = 0x100;
            m8DB = 0xFF;
            mPolyInfo.mProcId = -1;
        }
        actor_condition = actor_condition | 8;
    }

    if (fopAcM_GetParam(this) != 0) { /* !isTypeBossDie() */
        if (dComIfGs_checkGetItem(0x3E /* dItemNo_MASTER_SWORD_3_e */)) {
            if (fopAcM_GetParam(this) != 5) return cPhs_ERROR_e;
        } else if (dComIfGs_isEventBit(0x2910)) {
            if (fopAcM_GetParam(this) != 4) return cPhs_ERROR_e;
        } else if (dComIfGs_isEventBit(0x2E02)) {
            if (fopAcM_GetParam(this) != 3) return cPhs_ERROR_e;
        } else if (dComIfGs_isEventBit(0x1610)) {
            BOOL error = TRUE;
            if (cb1_isStartStage(0x10018F54 /* "sea" */) && dComIfGs_isEventBit(0x1604)) {
                error = FALSE;
            }
            if (error) return cPhs_ERROR_e;
        } else if (dComIfGs_checkGetItem(0x3A /* dItemNo_MASTER_SWORD_2_e */)) {
            if (fopAcM_GetParam(this) != 2) return cPhs_ERROR_e;
        } else if (dComIfGs_isEventBit(0x1820)) {
            return cPhs_ERROR_e;
        } else if (dComIfGs_checkGetItem(0x6B /* dItemNo_PEARL_FARORE_e */)) {
            if (fopAcM_GetParam(this) != 1) return cPhs_ERROR_e;
        } else {
            return cPhs_ERROR_e;
        }
    }

    cPhs_State result = dComIfG_resLoad(&mPhs, STR(0x10018F58) /* "Cb" */);
    if (result == cPhs_COMPLEATE_e) {
        if (fopAcM_GetParam(this) == 4) { /* isTypeKaze() */
            u32 save = gabi::load<u32>(0x101F84DC);
            if (gabi::load<u8>(save + 0x1DB) == 1) { /* dComIfGs_getPlayerPriestFlag() */
                u32 priest = save + 0x1CC;
                s16 angle = gabi::load<s16>(priest + 0xC);
                s8 roomNo = gabi::load<s8>(priest + 0xE);
                gabi::call(0x025B9834, save + 0x1148, 1, priest, angle, roomNo); /* dSv_restart_c::setRestartOption */
                gabi::call(0x025B8880, gabi::load<u32>(0x101F84DC) + 0x1CC, 1, priest, angle, roomNo); /* HD: dSv_player_priest_c::set */
            }
            gabi::call(0x02445784, this, 1); /* daPy_npc_c::checkRestart */
        }
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) != 0) { /* dComIfGp_getCb1Player() */
            return cPhs_ERROR_e;
        }
        if ((u32)(fopAcM_GetParam(this) - 1) <= 4) { /* KazeBoss, Forest, WaterFall, Ekaze, Kaze */
            gabi::store<u16>(CB1_M_STATUS, (u16)(gabi::load<u16>(CB1_M_STATUS) | daCbStts_MUSIC)); /* onMusic() */
        }
        if (!fopAcM_entrySolidHeap(this, 0x0221EC98 /* CheckCreateHeap */, 0x9400)) {
            return cPhs_ERROR_e;
        }
        setBaseMtx();
        u32 model = gabi::load<u32>(gabi::ea(mpMorf.get()) + 0x90);
        cullMtx = model != 0 ? model + 0xC8 : 0; /* fopAcM_SetMtx(this, mpMorf->getModel()->getBaseTRMtx()) */
        if (!init()) {
            return cPhs_ERROR_e;
        }
        fopAcM_setStageLayer(this);
        gabi::store<u32>(CB1_L_PARTNER, t);
    }
    return result;
}
VERIFY(0x0221F700, &daNpc_Cb1_c::create);

/* 0221E08C */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0221E08C, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Cb1_c* a_this = gabi::at<daNpc_Cb1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (a_this != nullptr) {
            /* static cXyz l_offsetAttPos(0, 0, 0), l_offsetEyePos(20, 10, 0) */
            if (gabi::load<u32>(0x10466B84) == 0) {
                gabi::store<u32>(0x10466B84, 1);
                gabi::store<f32>(CB1_L_OFFSET_ATT_POS, 0.0f);
                gabi::store<f32>(CB1_L_OFFSET_ATT_POS + 8, 0.0f);
                gabi::store<f32>(CB1_L_OFFSET_ATT_POS + 4, 0.0f);
            }
            if (gabi::load<u32>(0x10466B88) == 0) {
                gabi::store<u32>(0x10466B88, 1);
                gabi::store<f32>(CB1_L_OFFSET_EYE_POS, 20.0f);
                gabi::store<f32>(CB1_L_OFFSET_EYE_POS + 8, 0.0f);
                gabi::store<f32>(CB1_L_OFFSET_EYE_POS + 4, 10.0f);
            }
            s32 jnt_no = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); /* getJntNo() */
            PSMTXCopy(cb1_getAnmMtx(model, jnt_no), mDoMtx_stack_c::get());
            mDoMtx_XrotM(mDoMtx_stack_c::get(), a_this->mJntCtrl.mAngles[1][1]); /* getBackbone_y() */
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-a_this->mJntCtrl.mAngles[1][0]); /* -getBackbone_x() */
            PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(CB1_L_OFFSET_EYE_POS), &a_this->mEyePos);
            if (a_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                a_this->mAttnSetCount = a_this->mAttnSetCount + 1;
            }
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
            cb1_setAnmMtx(model, jnt_no, mDoMtx_stack_c::get());
        }
    }
    return TRUE;
}
VERIFY(0x0221E08C, nodeCallBack);

/* 0221E5BC */
static BOOL ppNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0221E5BC, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Cb1_c* a_this = gabi::at<daNpc_Cb1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (a_this != nullptr) {
            J3DJoint* joint = J3DNode_toJoint(node);
            s16 work3 = a_this->m8FA; /* getWork3() */
            s32 jnt_no = gabi::load<u16>(gabi::ea(joint) + 4);
            mDoMtx_YrotS(mDoMtx_stack_c::get(), work3);
            PSMTXConcat(cb1_getAnmMtx(model, jnt_no), mDoMtx_stack_c::get(), mDoMtx_stack_c::get()); /* revConcat */
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
            cb1_setAnmMtx(model, jnt_no, mDoMtx_stack_c::get());
        }
    }
    return TRUE;
}
VERIFY(0x0221E5BC, ppNodeCallBack);

/* 0221E23C */
static BOOL nutNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0221E23C, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Cb1_c* a_this = gabi::at<daNpc_Cb1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (a_this != nullptr) {
            /* static cXyz l_nutOffset(0, 0, 0), l_nutBase(1, 0, 0) */
            if (gabi::load<u32>(0x10466B8C) == 0) {
                gabi::store<u32>(0x10466B8C, 1);
                gabi::store<f32>(CB1_L_NUT_OFFSET, 0.0f);
                gabi::store<f32>(CB1_L_NUT_OFFSET + 8, 0.0f);
                gabi::store<f32>(CB1_L_NUT_OFFSET + 4, 0.0f);
            }
            if (gabi::load<u32>(0x10466B90) == 0) {
                gabi::store<f32>(CB1_L_NUT_BASE + 4, 0.0f);
                gabi::store<u32>(0x10466B90, 1);
                gabi::store<f32>(CB1_L_NUT_BASE, 1.0f);
                gabi::store<f32>(CB1_L_NUT_BASE + 8, 0.0f);
            }
            u32 joint = gabi::ea(J3DNode_toJoint(node));
            /* temp2 = the joint's translation: three getAnmMtx(jnt_no) calls, each marking the
             * matrices dirty; GHS interleaves the loads with the flag stores */
            u32 mdl = gabi::ea(model);
            u32 blk1 = gabi::load<u32>(mdl + 0x2C);
            u16 f1 = gabi::load<u16>(blk1 + 4);
            s32 jnt_no = gabi::load<u16>(joint + 4); /* getJntNo() */
            gabi::store<u16>(blk1 + 4, (u16)(f1 | 0x10));
            u32 m1 = gabi::load<u32>(blk1 + 0x10);
            u32 blk2 = gabi::load<u32>(mdl + 0x2C);
            u16 f2 = gabi::load<u16>(blk2 + 4);
            u32 x = gabi::load<u32>(m1 + jnt_no * 0x30 + 0xC);
            gabi::store<u16>(blk2 + 4, (u16)(f2 | 0x10));
            u32 blk3 = gabi::load<u32>(mdl + 0x2C);
            u32 m2 = gabi::load<u32>(blk2 + 0x10);
            u16 f3 = gabi::load<u16>(blk3 + 4);
            u32 m3 = gabi::load<u32>(blk3 + 0x10);
            u32 y = gabi::load<u32>(m2 + jnt_no * 0x30 + 0x1C);
            gabi::store<u16>(blk3 + 4, (u16)(f3 | 0x10));
            u32 z = gabi::load<u32>(m3 + jnt_no * 0x30 + 0x2C);

            gabi::Local<cXyz> temp2;
            /* lfs/stfs pairs with no arithmetic: the recompiled original keeps the bits */
            u32 t2 = gabi::ea(temp2.get());
            gabi::store<u32>(t2, x);
            gabi::store<u32>(t2 + 8, z);
            gabi::store<u32>(t2 + 4, y);
            PSVECAdd(&a_this->mNutPos, &a_this->mNusSpeed, &a_this->mNutPos); /* getNutPos() += getNusSpeed() */
            gabi::Local<cXyz> res;
            cXyz_mi(&a_this->mNutPos, res.get(), temp2.get());
            gabi::Local<cXyz> temp;
            cb1_copyWords(gabi::ea(temp.get()), gabi::ea(res.get()), 3);

            f32 temp4 = std_sqrtf(PSVECSquareMag(temp.get())); /* temp.abs() */
            if (!cM3d_IsZero(temp4)) {
                PSVECScale(temp.get(), temp.get(), 14.0f / temp4);
            }
            cXyz_pl(temp2.get(), res.get(), temp.get());
            cb1_copyWords(gabi::ea(&a_this->mNutPos), gabi::ea(res.get()), 3); /* getNutPos() = temp2 + temp */

            cXyz_ml(temp.get(), res.get(), -0.15f);
            PSVECAdd(&a_this->mNusSpeed, res.get(), &a_this->mNusSpeed); /* getNusSpeed() += temp * -0.15f */
            a_this->mNusSpeed.y = a_this->mNusSpeed.y + -1.3f;
            f32 temp5 = std_sqrtf(PSVECSquareMag(&a_this->mNusSpeed));
            if (!cM3d_IsZero(temp5)) {
                PSVECScale(&a_this->mNusSpeed, &a_this->mNusSpeed, 5.8f / temp5);
            }

            gabi::Local<Mtx34> temp3;
            gabi::call(0x02017374, gabi::at<cXyz>(CB1_L_NUT_BASE), temp.get(), temp3.get()); /* cM3d_UpMtx_Base */
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    /* lfs/stfs pair with no arithmetic: the recompiled original keeps the bits */
                    u32 anm = gabi::ea(cb1_getAnmMtx(model, jnt_no));
                    gabi::store<u32>(anm + (i * 4 + j) * 4, gabi::load<u32>(gabi::ea(temp3.get()) + (i * 4 + j) * 4));
                }
            }
            PSMTXCopy(cb1_getAnmMtx(model, jnt_no), J3DSys_mCurrentMtx());
        }
    }
    return TRUE;
}
VERIFY(0x0221E23C, nutNodeCallBack);

/* 0221E6E4 HD: no assertion on the face model's data */
BOOL daNpc_Cb1_c::createHeap() {
    WWHD_FUNC(0x0221E6E4, BOOL, this);
    u32 t = gabi::ea(this);
    J3DModelData* modelData = cb1_getObjectRes(0x1D /* CB_BDL_CB */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3D3, CB1_ASSERT_MODELDATA);
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr,
                                    0x00080000, 0x11000022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    m_backbone_jnt_num = cb1_getJointIndex(modelData, 0x10018E9C /* "backbone" */);
    if (m_backbone_jnt_num < 0) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3E0, STR(0x10018E70) /* "m_backbone_jnt_num >= 0" */);
    }
    m_armRend_jnt_num = cb1_getJointIndex(modelData, 0x10018E40 /* "armRend" */);
    if (m_armRend_jnt_num < 0) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3E3, STR(0x10018EA8) /* "m_armRend_jnt_num >= 0" */);
    }
    m_armL2_jnt_num = cb1_getJointIndex(modelData, 0x10018E4C /* "armL2" */);
    if (m_armL2_jnt_num < 0) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3E6, STR(0x10018EC0) /* "m_armL2_jnt_num >= 0" */);
    }
    cb1_setJointCallBack(modelData, (u16)m_backbone_jnt_num, 0x0221E08C /* nodeCallBack */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, t); /* setUserArea(this) */

    mpFaceModel = mDoExt_J3DModel__create(cb1_getObjectRes(0x1F /* CB_BDL_CB_FACE */), 0x00080000, 0x11000022);
    if (mpFaceModel.get() == nullptr) {
        return FALSE;
    }

    if (gabi::load<u16>(CB1_M_STATUS) & daCbStts_MUSIC) {
        modelData = cb1_getObjectRes(0x21 /* CB_BDL_CB_STICK */);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x403, CB1_ASSERT_MODELDATA);
        }
        mpStickModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
        if (mpStickModel.get() == nullptr) {
            return FALSE;
        }
        m_nut_jnt_num = cb1_getJointIndex(modelData, 0x10018E48 /* "nut" */);
        if (m_nut_jnt_num < 0) {
            JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x40E, STR(0x10018ED8) /* "m_nut_jnt_num >= 0" */);
        }
        cb1_setJointCallBack(modelData, (u16)m_nut_jnt_num, 0x0221E23C /* nutNodeCallBack */);
        gabi::store<u32>(gabi::ea(mpStickModel.get()) + 0xB8, t);

        modelData = cb1_getObjectRes(0x1E /* CB_BDL_CB_CELLO */);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x41A, CB1_ASSERT_MODELDATA);
        }
        mpCelloModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
        if (mpCelloModel.get() == nullptr) {
            return FALSE;
        }
    }

    modelData = cb1_getObjectRes(0x22 /* CB_BDL_PP */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x426, CB1_ASSERT_MODELDATA);
    }
    mpPropellerModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    if (mpPropellerModel.get() == nullptr) {
        return FALSE;
    }
    s8 center = cb1_getJointIndex(modelData, 0x10018E58 /* "center" */);
    m_center_jnt_num = center;
    cb1_setJointCallBack(modelData, (u16)center, 0x0221E5BC /* ppNodeCallBack */);
    gabi::store<u32>(gabi::ea(mpPropellerModel.get()) + 0xB8, t);
    if (!mPropellerBckAnim.init(modelData, (J3DAnmTransform*)cb1_getObjectRes(0x0E /* CB_BCK_M_OPEN */), false, 0, 1.0f, 0, -1, false)) {
        return FALSE;
    }

    modelData = cb1_getObjectRes(0x20 /* CB_BDL_CB_NUT */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x443, CB1_ASSERT_MODELDATA);
    }
    mpNutModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    if (mpNutModel.get() == nullptr) {
        return FALSE;
    }
    if (!mNutBckAnim.init(modelData, (J3DAnmTransform*)cb1_getObjectRes(0x0F /* CB_BCK_NUT_SOW */), true, 0, 1.0f, 0, -1, false)) {
        return FALSE;
    }

    mAcchCir[0].SetWall(20.0f, 20.0f);
    mAcchCir[1].SetWall(80.0f, 20.0f);
    mAcch.SetRoofCrrHeight(100.0f);
    mAcch.m_flags = mAcch.m_flags & ~(u32)dBgS_Acch::ROOF_NONE; /* ClrRoofNone() */
    mAcch.Set(&current.pos, &old.pos, this, 2, mAcchCir, &speed);
    mAcch.OnLineCheck();
    return TRUE;
}
VERIFY(0x0221E6E4, &daNpc_Cb1_c::createHeap);
