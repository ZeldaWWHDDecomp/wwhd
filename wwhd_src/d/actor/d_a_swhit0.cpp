/**
 * d_a_swhit0.cpp (WWHD)
 * Object - Crystal switch (struck by an attack: sets a switch, optionally with an event or a timer).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swhit0.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003EAF4
#define ARC_ALWAYS STR(0x1003EB58) /* "Always" */
#define SWHIT0_VTBL 0x1003EB44
#define AAB_VTBL 0x1003EB0C
#define l_sph_src gabi::at<dCcD_SrcSph>(0x101D12F4)
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101D1354)

enum {
    dRes_INDEX_ALWAYS_BCK_OBM_SYOUGEKISW_e = 0x0D,
    dRes_INDEX_ALWAYS_BDL_OBM_SYOUGEKISW_e = 0x35,
    dRes_INDEX_ALWAYS_BTK_OBM_SYOUGEKISW_e = 0x58,
};
enum { JA_SE_SHOCK_SW_ON = 0x083E, JA_SE_OBJ_COL_SWC_NSTONE = 0x6820 };
enum { dEvtCnd_UNK2_e = 2 };

/* local bindings (WWHD functions only this actor uses) */
static inline u8 cLib_calcTimer_u8(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 table, s32 num, s32 force, s32 last) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, table, num, force, last);
}
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }

struct daSwhit0_c : fopAc_ac_c {
    s32 getSwNo();
    u8 getEvNo();
    u8 getType();
    s32 getTimer();
    s32 getSwNo2();
    BOOL CreateHeap();
    void decisionRtType();
    BOOL CreateInit();
    cPhs_State create();
    s32 checkHit();
    s32 DemoProc();
    s32 actionOffWait();
    s32 actionToOnReady();
    s32 actionToOnOrder();
    s32 actionToOnDemo();
    s32 actionOnWait();
    s32 actionOnTimer();
    void setDrawMtx();
    void onFlag(u16 f) { mFlags |= f; }
    void offFlag(u16 f) { mFlags &= (u16)~f; }
    s32 checkFlag(u16 f) { return mFlags & f; }

    /* 0x3AC */ gptr<J3DModel> mpModel;
    /* 0x3B0 */ mDoExt_bckAnm mAnm;
    /* 0x43C */ mDoExt_btkAnm mTexAnm;
    /* 0x4B0 */ dCcD_Stts mColStatus;
    /* 0x4EC */ dCcD_Cyl mColCyl;
    /* 0x61C */ dCcD_Sph mColSph;
    /* 0x748 */ be<u8> mHitTimer;
    /* 0x749 */ be<u8> mState;
    /* 0x74A */ be<u8> mTimer;
    /* 0x74B */ be<u8> mRetType;
    /* 0x74C */ be<s16> mOnTimer;
    /* 0x74E */ be<u16> mFlags;
    /* 0x750 */ be<s16> mEventIdx;
    /* 0x752 */ u8 _752[2];
    /* 0x754 */ be<s32> mStaffId;
};
WWHD_OFFSET(daSwhit0_c, mColSph, 0x61C);
WWHD_OFFSET(daSwhit0_c, mStaffId, 0x754);
WWHD_SIZE(daSwhit0_c, 0x758);

/* 024A1884 */
s32 daSwhit0_c::getSwNo() {
    WWHD_FUNC(0x024A1884, s32, this);
    return fopAcM_GetParam(this) & 0xFF;
}
VERIFY(0x024A1884, &daSwhit0_c::getSwNo);

/* 024A1878 */
u8 daSwhit0_c::getEvNo() {
    WWHD_FUNC(0x024A1878, u8, this);
    return fopAcM_GetParam(this) >> 8;
}
VERIFY(0x024A1878, &daSwhit0_c::getEvNo);

/* 024A1800 */
u8 daSwhit0_c::getType() {
    WWHD_FUNC(0x024A1800, u8, this);
    return (fopAcM_GetParam(this) >> 0x10) & 0x0F;
}
VERIFY(0x024A1800, &daSwhit0_c::getType);

/* 024A1C48 */
s32 daSwhit0_c::getTimer() {
    WWHD_FUNC(0x024A1C48, s32, this);
    u8 param = (u8)(fopAcM_GetParam(this) >> 0x14);
    s32 timer = param;
    if (param == 0xFF) {
        timer = 0;
    }
    return timer;
}
VERIFY(0x024A1C48, &daSwhit0_c::getTimer);

/* 024A1D2C */
s32 daSwhit0_c::getSwNo2() {
    WWHD_FUNC(0x024A1D2C, s32, this);
    return home.angle.z & 0xFF;
}
VERIFY(0x024A1D2C, &daSwhit0_c::getSwNo2);

/* 024A1288 */
BOOL daSwhit0_c::CreateHeap() {
    WWHD_FUNC(0x024A1288, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(ARC_ALWAYS, dRes_INDEX_ALWAYS_BDL_OBM_SYOUGEKISW_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xD5, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1003EB60), 0xD5, STR(0x1003EB70));
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000202);
    if (mpModel == nullptr) {
        return FALSE;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(ARC_ALWAYS, dRes_INDEX_ALWAYS_BCK_OBM_SYOUGEKISW_e, SAFESTRING_VTBL);
    if (mAnm.init(modelData, anm, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false) == 0) {
        return FALSE;
    }
    J3DAnmTextureSRTKey* texAnm =
        (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(ARC_ALWAYS, dRes_INDEX_ALWAYS_BTK_OBM_SYOUGEKISW_e, SAFESTRING_VTBL);
    if (mTexAnm.init(modelData, texAnm, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0) == 0) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x024A1288, &daSwhit0_c::CreateHeap);

/* 024A13F8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_actr) {
    WWHD_FUNC(0x024A13F8, BOOL, i_actr);
    return static_cast<daSwhit0_c*>(i_actr)->CreateHeap();
}
VERIFY(0x024A13F8, CheckCreateHeap);

/* 024A2034 */
void daSwhit0_c::decisionRtType() {
    WWHD_FUNC(0x024A2034, void, this);
    if (home.angle.x == 0) {
        mRetType = 0;
    } else if (home.angle.x < -0x4E20 || home.angle.x > 0x4E20) {
        mRetType = 2;
    } else {
        mRetType = 1;
    }
}
VERIFY(0x024A2034, &daSwhit0_c::decisionRtType);

/* 024A2068 */
BOOL daSwhit0_c::CreateInit() {
    WWHD_FUNC(0x024A2068, BOOL, this);
    setDrawMtx();
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    decisionRtType();
    mColStatus.Init(0xFF, 0xFF, this);
    cXyz& att = *gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 sx = cM_ssin(home.angle.x) * 65.0f;
    att.x = gabi::fmadds(sx, cM_ssin(home.angle.y), att.x);
    att.y = gabi::fmadds(65.0f, cM_scos(home.angle.x), att.y);
    att.z = gabi::fmadds(cM_ssin(home.angle.x) * 65.0f, cM_scos(home.angle.y), att.z);
    eyePos.x = att.x;
    eyePos.y = att.y;
    eyePos.z = att.z;
    if (mRetType == 0) {
        mColCyl.Set(l_cyl_src);
        mColCyl.SetStts(&mColStatus);
        mColCyl.SetC(&current.pos);
        onFlag(0x02);
    }
    mColSph.Set(l_sph_src);
    mColSph.SetStts(&mColStatus);
    mColSph.SetC(&att);

    if (dComIfGs_isSwitch(getSwNo(), fopAcM_GetRoomNo(this))) {
        mState = 4;
        onFlag(0x01);
    } else {
        mState = 0;
        offFlag(0x01);
    }
    u8 type = getType();
    u8 evNo = getEvNo();
    if (type == 0x03) {
        mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1003EBAC) /* "DEFAULT_SWITCH_NOSOUND" */, evNo);
    } else {
        mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1003EBC4) /* "DEFAULT_SWITCH" */, evNo);
    }
    return TRUE;
}
VERIFY(0x024A2068, &daSwhit0_c::CreateInit);

/* 024A227C */
cPhs_State daSwhit0_c::create() {
    WWHD_FUNC(0x024A227C, cPhs_State, this);
    /* fopAcM_ct(this, daSwhit0_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SWHIT0_VTBL;
            /* mDoExt_bckAnm::mDoExt_bckAnm() (inline) */
            u32 b = gabi::ea(&mAnm);
            gabi::call(0x027F2BC0, &mAnm.mFrameCtrl, 0); /* J3DFrameCtrl::init */
            gabi::store<u32>(b + 0x10, 0x1016E54C);
            gabi::call(0x027DA984, gabi::at<u8>(b + 0x14));
            gabi::store<u32>(b + 0x84, 0);
            gabi::store<u32>(b + 0x80, 0);
            gabi::store<u32>(b + 0x10, 0x1003EB1C);
            gabi::store<u32>(b + 0x88, 0);
            gabi::store<u32>(b + 0x48, 0x1016D820);
            gabi::store<u32>(b + 0x7C, 0);
            gabi::store<u32>(b + 0x58, 0);
            mDoExt_btkAnm::ct(&mTexAnm);
            dCcD_Stts_ct(&mColStatus);
            dCcD_Cyl_ct(&mColCyl, AAB_VTBL);
            gabi::call(0x025166F0, &mColSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    shape_angle.z = 0;
    current.angle.z = 0;
    if (fopAcM_entrySolidHeap(this, 0x024A13F8 /* CheckCreateHeap */, 0x34A0) == 0) {
        return cPhs_ERROR_e;
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A227C, &daSwhit0_c::create);

/* 024A180C */
s32 daSwhit0_c::checkHit() {
    WWHD_FUNC(0x024A180C, s32, this);
    if (cLib_calcTimer_u8(&mHitTimer) == 0 && mColSph.ChkTgHit() != 0) {
        mHitTimer = 8;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x024A180C, &daSwhit0_c::checkHit);

/* 024A1A48 */
s32 daSwhit0_c::DemoProc() {
    WWHD_FUNC(0x024A1A48, s32, this);
    /* static char* action_table[] = {"WAIT", "CHANGE"} (0x101D1398) */
    enum { ACT_WAIT, ACT_CHANGE };
    if (dComIfGp_evmng_getIsAddvance(mStaffId)) {
        switch (dComIfGp_evmng_getMyActIdx(mStaffId, 0x101D1398, 2, FALSE, 0)) {
        case ACT_CHANGE:
            dComIfGs_onSwitch(getSwNo(), fopAcM_GetRoomNo(this));
            onFlag(0x01);
            fopAcM_seStart(this, JA_SE_SHOCK_SW_ON, 0);
            break;
        }
    }
    dComIfGp_evmng_cutEnd(mStaffId);
    return TRUE;
}
VERIFY(0x024A1A48, &daSwhit0_c::DemoProc);

/* 024A188C */
s32 daSwhit0_c::actionOffWait() {
    WWHD_FUNC(0x024A188C, s32, this);
    u32 type = getType();
    if (checkHit() != 0) {
        switch (type) {
        case 1:
            mState = 1;
            mTimer = 5;
            break;
        case 3: {
            mState = 2;
            u8 evNo = getEvNo();
            fopAcM_orderOtherEventId(this, mEventIdx, evNo, 0xFFFF, 0, 1);
            eventInfo_onCondition(this, dEvtCnd_UNK2_e);
            break;
        }
        default:
            mState = 4;
            onFlag(0x01);
            dComIfGs_onSwitch(getSwNo(), fopAcM_GetRoomNo(this));
            break;
        }
        fopAcM_seStart(this, JA_SE_OBJ_COL_SWC_NSTONE, 0);
    }
    return TRUE;
}
VERIFY(0x024A188C, &daSwhit0_c::actionOffWait);

/* 024A19B4 */
s32 daSwhit0_c::actionToOnReady() {
    WWHD_FUNC(0x024A19B4, s32, this);
    if (dComIfGp_event_runCheck()) {
        return TRUE;
    }
    if (mTimer != 0) {
        mTimer--;
    } else {
        mState = 2;
        u8 evNo = getEvNo();
        fopAcM_orderOtherEventId(this, mEventIdx, evNo, 0xFFFF, 0, 1);
        eventInfo_onCondition(this, dEvtCnd_UNK2_e);
    }
    return TRUE;
}
VERIFY(0x024A19B4, &daSwhit0_c::actionToOnReady);

/* 024A1B2C */
s32 daSwhit0_c::actionToOnOrder() {
    WWHD_FUNC(0x024A1B2C, s32, this);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mState = 3;
        mStaffId = dComIfGp_evmng_getMyStaffId(STR(0x1003EBA0) /* "SWITCH" */, nullptr, 0);
        DemoProc();
    } else {
        u8 evNo = getEvNo();
        fopAcM_orderOtherEventId(this, mEventIdx, evNo, 0xFFFF, 0, 1);
        eventInfo_onCondition(this, dEvtCnd_UNK2_e);
    }
    return TRUE;
}
VERIFY(0x024A1B2C, &daSwhit0_c::actionToOnOrder);

/* 024A1BD0 */
s32 daSwhit0_c::actionToOnDemo() {
    WWHD_FUNC(0x024A1BD0, s32, this);
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = 4;
        dComIfGp_event_reset();
        return TRUE;
    }
    DemoProc();
    return TRUE;
}
VERIFY(0x024A1BD0, &daSwhit0_c::actionToOnDemo);

/* 024A1C60 */
s32 daSwhit0_c::actionOnWait() {
    WWHD_FUNC(0x024A1C60, s32, this);
    if (checkHit() != 0) {
        fopAcM_seStart(this, JA_SE_OBJ_COL_SWC_NSTONE, 0);
    }
    if (getType() == 0x02) {
        if (!dComIfGs_isSwitch(getSwNo(), fopAcM_GetRoomNo(this))) {
            mState = 0;
            offFlag(0x01);
            return TRUE;
        }
    }
    s32 timer = getTimer();
    if (timer != 0) {
        mOnTimer = (s16)(timer * 0x0F); /* HD: getTimer() called once */
        mState = 5;
    }
    return TRUE;
}
VERIFY(0x024A1C60, &daSwhit0_c::actionOnWait);

/* 024A1D34 */
s32 daSwhit0_c::actionOnTimer() {
    WWHD_FUNC(0x024A1D34, s32, this);
    if (checkHit() != 0) {
        fopAcM_seStart(this, JA_SE_OBJ_COL_SWC_NSTONE, 0);
    }
    if (dComIfGs_isSwitch(getSwNo2(), fopAcM_GetRoomNo(this))) {
        mState = 4;
    } else if (mOnTimer > 0) {
        mOnTimer--;
    } else {
        mState = 0;
        offFlag(0x01);
        dComIfGs_offSwitch(getSwNo(), fopAcM_GetRoomNo(this));
    }
    return TRUE;
}
VERIFY(0x024A1D34, &daSwhit0_c::actionOnTimer);

/* 024A1F54 */
void daSwhit0_c::setDrawMtx() {
    WWHD_FUNC(0x024A1F54, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x024A1F54, &daSwhit0_c::setDrawMtx);

/* HD material colour: J3DGXColorS10 / 255 as floats, converted by 0274D458 and stored in the
 * material's HD colour block (027F9F0C(&mat->m_flags, slot)) */
static void set_tev_color(u32 mat, s32 reg, u32 color, u32 dirtyBit, s32 slot) {
    u32 tev = gabi::load<u32>(mat + 0x18);
    u32 getTevColor = gabi::load<u32>(gabi::load<u32>(tev + 4) + 0x34);
    if (gabi::call_ptr<u32>(getTevColor, gabi::at<u8>(tev), reg) == 0)
        return;
    tev = gabi::load<u32>(mat + 0x18);
    u32 setTevColor = gabi::load<u32>(gabi::load<u32>(tev + 4) + 0x24);
    gabi::call_ptr(setTevColor, gabi::at<u8>(tev), reg, gabi::at<u8>(color));
    gabi::Local<be<f32>[4]> in;
    gabi::Local<be<f32>[4]> out;
    for (int k = 0; k < 4; k++) (*in)[k] = (f32)gabi::load<s16>(color + 2 * k) / 255.0f;
    gabi::call(0x0274D458, out.get(), in.get(), 1.0f);
    gabi::store<u32>(mat + 0xA0, gabi::load<u32>(mat + 0xA0) | dirtyBit);
    u32 dst = gabi::call<u32>(0x027F9F0C, gabi::at<u8>(mat + 0xA0), slot);
    f32 x = (*out)[0], y = (*out)[1], z = (*out)[2];
    f32 a = (f32)gabi::load<s16>(color + 6) / 255.0f;
    gabi::store<f32>(dst + 4, y);
    gabi::store<f32>(dst + 8, z);
    gabi::store<f32>(dst + 0, x);
    gabi::store<f32>(dst + 0xC, a);
}

/* 024A13FC: draw() inlined. HD: the tev colours also go to the HD material colour block */
static s32 daSwhit0_Draw(daSwhit0_c* i_this) {
    WWHD_FUNC(0x024A13FC, s32, i_this);
    /* static GXColorS10 l_color[4] (guard 0x101FDC9C, object 0x101FEB2C, initialiser 0x101D12D4) */
    be<u32>& guard = *gabi::at<be<u32>>(0x101FDC9C);
    if (guard == 0) {
        guard = 1;
        memcpy_g(gabi::at<u8>(0x101FEB2C), gabi::at<u8>(0x101D12D4), 0x20);
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    s32 flag = i_this->checkFlag(0x01);
    J3DModelData* modelData = J3DModel_getModelData(i_this->mpModel);
    u32 colors = 0x101FEB2C;
    if (flag) {
        colors += 0x10;
    }
    u32 md = gabi::ea(modelData);
    for (u16 i = 0; i < gabi::load<u16>(gabi::call<u32>(0x027F3F8C, modelData) + 0x24); i++) {
        u32 mat = gabi::load<u32>(md + 0x10);
        if (i < gabi::load<u32>(md + 0xC))
            mat += i * 0x39C;
        set_tev_color(mat, 1, colors, 0x20, 5);
        set_tev_color(mat, 2, colors + 8, 0x40, 6);
    }
    i_this->mAnm.entry(modelData, i_this->mAnm.getFrame());
    i_this->mTexAnm.entry(modelData, i_this->mTexAnm.getFrame());
    mDoExt_modelUpdateDL(i_this->mpModel);
    return TRUE;
}
VERIFY(0x024A13FC, daSwhit0_Draw);

/* 024A1E20: execute() inlined */
static s32 daSwhit0_Execute(daSwhit0_c* i_this) {
    WWHD_FUNC(0x024A1E20, s32, i_this);
    i_this->mAnm.play();
    i_this->mTexAnm.play();
    switch (i_this->mState) {
    case 0: i_this->actionOffWait(); break;
    case 1: i_this->actionToOnReady(); break;
    case 2: i_this->actionToOnOrder(); break;
    case 3: i_this->actionToOnDemo(); break;
    case 4: i_this->actionOnWait(); break;
    case 5: i_this->actionOnTimer(); break;
    }
    if (i_this->checkFlag(0x02)) {
        dComIfG_Ccsp_Set(&i_this->mColCyl);
    }
    dComIfG_Ccsp_Set(&i_this->mColSph);
    return TRUE;
}
VERIFY(0x024A1E20, daSwhit0_Execute);

/* 024A1F44 */
static s32 daSwhit0_IsDelete(daSwhit0_c* i_this) {
    WWHD_FUNC(0x024A1F44, s32, i_this);
    return TRUE;
}
VERIFY(0x024A1F44, daSwhit0_IsDelete);

/* 024A1F4C: HD: the explicit destructor call is gone */
static s32 daSwhit0_Delete(daSwhit0_c* i_this) {
    WWHD_FUNC(0x024A1F4C, s32, i_this);
    return TRUE;
}
VERIFY(0x024A1F4C, daSwhit0_Delete);

/* 024A2400 */
static cPhs_State daSwhit0_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024A2400, cPhs_State, i_this);
    return static_cast<daSwhit0_c*>(i_this)->create();
}
VERIFY(0x024A2400, daSwhit0_Create);

/* 024A2404 */
static void __sinit_d_a_swhit0_cpp() {
    WWHD_FUNC(0x024A2404, void, (u32)0);
    sinit_header_statics(0x1046E164, 0x101D13A0);
}
VERIFY(0x024A2404, __sinit_d_a_swhit0_cpp);

/* 024A2498: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024A2498, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024A2498, trivial_dt);

/* 024A24AC: daSwhit0_c deleting destructor */
static void daSwhit0_c_dt(daSwhit0_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A24AC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mColSph, 2);        /* dCcD_Sph::~dCcD_Sph */
        dCcD_Cyl_dt(&i_this->mColCyl, 2);
        dCcD_Stts_dt(&i_this->mColStatus, 2);
        gabi::call(0x027F3628, gabi::at<u8>(gabi::ea(&i_this->mAnm) + 0x10), 0); /* bckAnm sub-object */
        gabi::call(0x025D50BC, i_this, 0);                  /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A24AC, daSwhit0_c_dt);

/* 024A2530: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x024A2530, void, p);
}
VERIFY(0x024A2530, empty_virtual);
