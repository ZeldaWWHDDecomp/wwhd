/**
 * d_a_swhit0.cpp (WWHD)
 * Object - Crystal switch (struck by an attack: sets a switch, optionally with an event or a
 * timer).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swhit0.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_swhit0.h"

// HD: GHS emits per-TU copies of the SafeString and inline constructor vtables.
const gc_tu_data gc_tu = {
    .safeString = 0x1003EAF4,
    .headerStatics = {0x1046E164, 0x101D13A0, 0},
};
static dCcD_SrcSph l_sph_src;
static dCcD_SrcCyl l_cyl_src;
static u32 action_table[2] = {};
static SwhitColor l_color[4] = {};
static SwhitColor initialColor[4] = {};
static u32 colorInitialized;

/* 024A1884 */
s32 daSwhit0_c::getSwNo() { return fopAcM_GetParam(this) & 0xFF; }

/* 024A1878 */
u8 daSwhit0_c::getEvNo() { return fopAcM_GetParam(this) >> 8; }

/* 024A1800 */
u8 daSwhit0_c::getType() { return (fopAcM_GetParam(this) >> 0x10) & 0x0F; }

/* 024A1C48 */
s32 daSwhit0_c::getTimer() {
    u8 param = (u8)(fopAcM_GetParam(this) >> 0x14);
    s32 timer = param;
    if (param == 0xFF) {
        timer = 0;
    }
    return timer;
}

/* 024A1D2C */
s32 daSwhit0_c::getSwNo2() { return home.angle.z & 0xFF; }

/* 024A1288 */
BOOL daSwhit0_c::CreateHeap() {
    J3DModelData *modelData =
        (J3DModelData *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_OBM_SYOUGEKISW_e);
    if (modelData == nullptr) /* JUT_ASSERT(0xD5, modelData != NULL) */
        JUT_ASSERT_fail("d_a_swhit0.cpp", 0xD5, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000202);
    if (mpModel == nullptr) {
        return FALSE;
    }
    J3DAnmTransform *anm =
        (J3DAnmTransform *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BCK_OBM_SYOUGEKISW_e);
    if (mAnm.init(modelData, anm, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false) == 0) {
        return FALSE;
    }
    J3DAnmTextureSRTKey *texAnm = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(
        "Always", dRes_INDEX_ALWAYS_BTK_OBM_SYOUGEKISW_e);
    if (mTexAnm.init(modelData, texAnm, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0) ==
        0) {
        return FALSE;
    }
    return TRUE;
}

/* 024A13F8 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_actr) {
    return static_cast<daSwhit0_c *>(i_actr)->CreateHeap();
}

/* 024A2034 */
void daSwhit0_c::decisionRtType() {
    if (home.angle.x == 0) {
        mRetType = 0;
    } else if (home.angle.x < -0x4E20 || home.angle.x > 0x4E20) {
        mRetType = 2;
    } else {
        mRetType = 1;
    }
}

/* 024A2068 */
BOOL daSwhit0_c::CreateInit() {
    setDrawMtx();
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    decisionRtType();
    mColStatus.Init(0xFF, 0xFF, this);
    cXyz &att = attention_info.position; /* attention_info.position */
    f32 sx = cM_ssin(home.angle.x) * 65.0f;
    att.x = gabi::fmadds(sx, cM_ssin(home.angle.y), att.x);
    att.y = gabi::fmadds(65.0f, cM_scos(home.angle.x), att.y);
    att.z = gabi::fmadds(cM_ssin(home.angle.x) * 65.0f, cM_scos(home.angle.y), att.z);
    eyePos.x = att.x;
    eyePos.y = att.y;
    eyePos.z = att.z;
    if (mRetType == 0) {
        mColCyl.Set(&l_cyl_src);
        mColCyl.SetStts(&mColStatus);
        mColCyl.SetC(&current.pos);
        onFlag(COLLISION_CYLINDER);
    }
    mColSph.Set(&l_sph_src);
    mColSph.SetStts(&mColStatus);
    mColSph.SetC(&att);

    if (dComIfGs_isSwitch(getSwNo(), fopAcM_GetRoomNo(this))) {
        mState = ON_WAIT;
        onFlag(SWITCH_ON);
    } else {
        mState = OFF_WAIT;
        offFlag(SWITCH_ON);
    }
    u8 type = getType();
    u8 evNo = getEvNo();
    if (type == TYPE_EVENT_NOSOUND) {
        mEventIdx = dComIfGp_evmng_getEventIdx(
            "DEFAULT_SWITCH_NOSOUND" /* "DEFAULT_SWITCH_NOSOUND" */, evNo);
    } else {
        mEventIdx = dComIfGp_evmng_getEventIdx("DEFAULT_SWITCH" /* "DEFAULT_SWITCH" */, evNo);
    }
    return TRUE;
}

/* 024A227C */
cPhs_State daSwhit0_c::create() {
    /* fopAcM_ct(this, daSwhit0_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = SWHIT_ACTOR_VTABLE; // HD: this actor's vtable
            /* mDoExt_bckAnm::mDoExt_bckAnm() (inline) */
            auto *b = reinterpret_cast<SwhitBckAnm *>(&mAnm);
            b->mFrameCtrl.init(0);
            b->mVtable = SWHIT_ANM_BASE_VTABLE;
            swhitInitTransformCache(b->mTransformCache);
            b->mResource = 0;
            b->mData = 0;
            b->mVtable = SWHIT_BCK_VTABLE;
            b->mFlags = 0;
            b->mCacheVtable = SWHIT_TRANSFORM_CACHE_VTABLE;
            b->mCacheCount = 0;
            b->mCacheData = 0;
            mDoExt_btkAnm::ct(&mTexAnm);
            dCcD_Stts_ct(&mColStatus);
            dCcD_Cyl_ct(&mColCyl, SWHIT_AAB_VTABLE);
            swhitConstructSphere(&mColSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    shape_angle.z = 0;
    current.angle.z = 0;
    if (fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)CheckCreateHeap), 0x34A0) == 0) {
        return cPhs_ERROR_e;
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 024A180C */
s32 daSwhit0_c::checkHit() {
    if (swhitCalcTimer(&mHitTimer) == 0 && mColSph.ChkTgHit() != 0) {
        mHitTimer = 8;
        return TRUE;
    }
    return FALSE;
}

/* 024A1A48 */
s32 daSwhit0_c::DemoProc() {
    // HD: the action-name table keeps the two guest string pointers.
    enum { ACT_WAIT, ACT_CHANGE };
    if (dComIfGp_evmng_getIsAddvance(mStaffId)) {
        switch (dComIfGp_evmng_getMyActIdx(mStaffId, reinterpret_cast<gptr<char> *>(action_table),
                                           2, FALSE, 0)) {
        case ACT_CHANGE:
            dComIfGs_onSwitch(getSwNo(), fopAcM_GetRoomNo(this));
            onFlag(SWITCH_ON);
            fopAcM_seStart(this, JA_SE_SHOCK_SW_ON, 0);
            break;
        }
    }
    dComIfGp_evmng_cutEnd(mStaffId);
    return TRUE;
}

/* 024A188C */
s32 daSwhit0_c::actionOffWait() {
    u32 type = getType();
    if (checkHit() != 0) {
        switch (type) {
        case TYPE_EVENT_DELAY:
            mState = TO_ON_READY;
            mTimer = 5;
            break;
        case TYPE_EVENT_NOSOUND: {
            mState = TO_ON_ORDER;
            u8 evNo = getEvNo();
            fopAcM_orderOtherEventId(this, mEventIdx, evNo, 0xFFFF, 0, 1);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
            break;
        }
        default:
            mState = ON_WAIT;
            onFlag(SWITCH_ON);
            dComIfGs_onSwitch(getSwNo(), fopAcM_GetRoomNo(this));
            break;
        }
        fopAcM_seStart(this, JA_SE_OBJ_COL_SWC_NSTONE, 0);
    }
    return TRUE;
}

/* 024A19B4 */
s32 daSwhit0_c::actionToOnReady() {
    if (dComIfGp_event_runCheck()) {
        return TRUE;
    }
    if (mTimer != 0) {
        mTimer--;
    } else {
        mState = TO_ON_ORDER;
        u8 evNo = getEvNo();
        fopAcM_orderOtherEventId(this, mEventIdx, evNo, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    }
    return TRUE;
}

/* 024A1B2C */
s32 daSwhit0_c::actionToOnOrder() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        mState = TO_ON_DEMO;
        mStaffId = dComIfGp_evmng_getMyStaffId("SWITCH" /* "SWITCH" */, nullptr, 0);
        DemoProc();
    } else {
        u8 evNo = getEvNo();
        fopAcM_orderOtherEventId(this, mEventIdx, evNo, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    }
    return TRUE;
}

/* 024A1BD0 */
s32 daSwhit0_c::actionToOnDemo() {
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = ON_WAIT;
        dComIfGp_event_reset();
        return TRUE;
    }
    DemoProc();
    return TRUE;
}

/* 024A1C60 */
s32 daSwhit0_c::actionOnWait() {
    if (checkHit() != 0) {
        fopAcM_seStart(this, JA_SE_OBJ_COL_SWC_NSTONE, 0);
    }
    if (getType() == TYPE_EXTERNAL_RESET) {
        if (!dComIfGs_isSwitch(getSwNo(), fopAcM_GetRoomNo(this))) {
            mState = OFF_WAIT;
            offFlag(SWITCH_ON);
            return TRUE;
        }
    }
    s32 timer = getTimer();
    if (timer != 0) {
        mOnTimer = (s16)(timer * 0x0F); /* HD: getTimer() called once */
        mState = ON_TIMER;
    }
    return TRUE;
}

/* 024A1D34 */
s32 daSwhit0_c::actionOnTimer() {
    if (checkHit() != 0) {
        fopAcM_seStart(this, JA_SE_OBJ_COL_SWC_NSTONE, 0);
    }
    if (dComIfGs_isSwitch(getSwNo2(), fopAcM_GetRoomNo(this))) {
        mState = ON_WAIT;
    } else if (mOnTimer > 0) {
        mOnTimer--;
    } else {
        mState = OFF_WAIT;
        offFlag(SWITCH_ON);
        dComIfGs_offSwitch(getSwNo(), fopAcM_GetRoomNo(this));
    }
    return TRUE;
}

/* 024A1F54 */
void daSwhit0_c::setDrawMtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    swhitXYZrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* HD material colour: J3DGXColorS10 / 255 as floats, converted by 0274D458 and stored in the
 * material's HD colour block (027F9F0C(&mat->m_flags, slot)) */
/* inlined into daSwhit0_Draw */
static void set_tev_color(SwhitMaterial *mat, s32 reg, SwhitColor *color, u32 dirtyBit, s32 slot) {
    SwhitTevBlock *tev = mat->mTevBlock;
    if (gabi::call_ptr<u32>(tev->mVtable->getTevColor.get(), tev, reg) == 0)
        return;
    tev = mat->mTevBlock;
    gabi::call_ptr(tev->mVtable->setTevColor.get(), tev, reg, color);
    gabi::Local<SwhitFloatColor> in;
    gabi::Local<SwhitFloatColor> out;
    for (int k = 0; k < 4; k++)
        in->rgba[k] = (f32)color->rgba[k] / 255.0f;
    swhitConvertColor(out.get(), in.get(), 1.0f);
    mat->mFlags |= dirtyBit;
    SwhitFloatColor *dst = swhitGetColorSlot(&mat->mFlags, slot);
    f32 x = out->rgba[0], y = out->rgba[1], z = out->rgba[2];
    f32 alpha = (f32)color->rgba[3] / 255.0f;
    dst->rgba[1] = y;
    dst->rgba[2] = z;
    dst->rgba[0] = x;
    dst->rgba[3] = alpha;
}

/* 024A13FC */
// HD: draw() is inlined; colours also update the HD material colour block.
static s32 daSwhit0_Draw(daSwhit0_c *i_this) {
    // HD: the initialization guard is stored before copying all four colours.
    if (colorInitialized == 0) {
        colorInitialized = 1;
        memcpy_g(l_color, initialColor, sizeof(l_color));
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    s32 flag = i_this->checkFlag(SWITCH_ON);
    J3DModelData *modelData = i_this->mpModel->getModelData();
    SwhitColor *colors = l_color;
    if (flag) {
        colors += 2;
    }
    for (u16 i = 0; i < swhitGetMaterialTable(modelData)->mMaterialNum; i++) {
        auto *mat = reinterpret_cast<SwhitMaterial *>(modelData->getMaterialNodePointer(i));
        set_tev_color(mat, 1, colors, 0x20, 5);
        set_tev_color(mat, 2, colors + 1, 0x40, 6);
    }
    i_this->mAnm.entry(modelData, i_this->mAnm.getFrame());
    i_this->mTexAnm.entry(modelData, i_this->mTexAnm.getFrame());
    mDoExt_modelUpdateDL(i_this->mpModel);
    return TRUE;
}

/* 024A1E20 */
// HD: execute() is inlined.
static s32 daSwhit0_Execute(daSwhit0_c *i_this) {
    i_this->mAnm.play();
    i_this->mTexAnm.play();
    switch (i_this->mState) {
    case OFF_WAIT:
        i_this->actionOffWait();
        break;
    case TO_ON_READY:
        i_this->actionToOnReady();
        break;
    case TO_ON_ORDER:
        i_this->actionToOnOrder();
        break;
    case TO_ON_DEMO:
        i_this->actionToOnDemo();
        break;
    case ON_WAIT:
        i_this->actionOnWait();
        break;
    case ON_TIMER:
        i_this->actionOnTimer();
        break;
    }
    if (i_this->checkFlag(COLLISION_CYLINDER)) {
        dComIfG_Ccsp_Set(&i_this->mColCyl);
    }
    dComIfG_Ccsp_Set(&i_this->mColSph);
    return TRUE;
}

/* 024A1F44 */
static s32 daSwhit0_IsDelete(daSwhit0_c *i_this) { return TRUE; }

/* 024A1F4C */
// HD: the explicit destructor call is gone.
static s32 daSwhit0_Delete(daSwhit0_c *i_this) { return TRUE; }

/* 024A2400 */
static cPhs_State daSwhit0_Create(fopAc_ac_c *i_this) {
    return static_cast<daSwhit0_c *>(i_this)->create();
}

// HD: compiler-generated and template code of this translation unit.
/* 024A2404 */
static void __sinit_d_a_swhit0_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 024A2498 */
static void trivial_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 024A24AC */
static void daSwhit0_c_dt(daSwhit0_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        swhitDestroySphere(&i_this->mColSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Cyl_dt(&i_this->mColCyl, 2);
        dCcD_Stts_dt(&i_this->mColStatus, 2);
        swhitDestroyBckCache(&reinterpret_cast<SwhitBckAnm *>(&i_this->mAnm)->mVtable,
                             0);      /* bckAnm sub-object */
        swhitDestroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024A2530 */
static void empty_virtual(void *p) {}

/* WWHD glue (tools/verify/autoglue.py)
data l_sph_src 101D12F4 nocheck
data l_cyl_src 101D1354 nocheck
data action_table 101D1398 nocheck
data l_color 101FEB2C nocheck
data initialColor 101D12D4 nocheck
data colorInitialized 101FDC9C nocheck
str 1003EB58
str 1003EB60
str 1003EB70
str 1003EBAC
str 1003EBC4
str 1003EBA0
ext 025F19F8 void swhitXYZrotM(Mtx34*, s16, s16, s16)
ext 0207A9A0 u8 swhitCalcTimer(be<u8>*)
ext 027DA984 void swhitInitTransformCache(void*)
ext 025166F0 void swhitConstructSphere(dCcD_Sph*)
ext 02515AE8 void swhitDestroySphere(dCcD_Sph*, s32)
ext 027F3628 void swhitDestroyBckCache(void*, s32)
ext 025D50BC void swhitDestroyActor(fopAc_ac_c*, s32)
ext 0274D458 void swhitConvertColor(SwhitFloatColor*, SwhitFloatColor*, f32)
ext 027F9F0C SwhitFloatColor* swhitGetColorSlot(be<u32>*, s32)
ext 027F3F8C SwhitMaterialTable* swhitGetMaterialTable(J3DModelData*)
*/
