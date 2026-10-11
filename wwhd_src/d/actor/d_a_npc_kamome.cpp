/**
 * d_a_npc_kamome.cpp (WWHD)
 * Player - Hyoi Seagull (execute is in d_a_npc_kamome_exec.cpp)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_kamome.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_kamome.h"

// HD: GHS emits this unit's own inline-constructed vtables and header statics.
const gc_tu_data gc_tu = {
    .safeString = KAM_SAFESTRING_VTBL,
    .safeStringAssure = 0x02260D60,
    .objAcch = {0x1001B4EC, 0x1001B50C, 0x1001B4FC},
    .objAcchDt = {0x1001B4FC, 0x1001B50C},
    .gndChk = {0x1001B4AC, 0x1001B4BC, 0x1001B4DC, 0x1001B4CC},
    .gndChkDt = {0x1001B4BC, 0x1001B4DC, 0x1001B49C},
    .headerStatics = {0x104673C8, 0x101BEDA0, 0},
};

static u8 l_hyoi_kamome;
static u32 l_act;
static char* l_staff_name;
static s16 l_demo_start_chk_cnt;
static s16 l_demo_start_chk_flag;
static u32 offset_guard;
static u32 offsetAttPos_guard;
static u32 center_guard;
static u32 splash_guard;
static u32 ripple_guard;
static cXyz l_ms_at_local_vec;
static cXyz l_ms_at_local_start;
static cXyz l_ms_at_local_end;
static cXyz l_line_bg_local_end;
static cXyz l_call_local_kyori;
static cXyz l_offset;
static cXyz l_offsetAttPos;
static cXyz l_center;
static cXyz splash_scale;
static cXyz ripple_scale;
static KamomeAudioInterface* l_audio;
static dCcD_SrcSph l_sph_src;
static KamomeCapsuleSource l_kam_at_cps_src;
static dPa_levelEcallBack l_rippleCallback;
static daNpc_kam_HIO_c l_HIO;
static KamomeActionEntry l_waitNpcAction = {&daNpc_kam_c::waitNpcAction};
static KamomeActionEntry l_waitPlayerAction = {&daNpc_kam_c::waitPlayerAction};
static KamomeActionEntry l_damagePlayerAction = {&daNpc_kam_c::damagePlayerAction};
static KamomeHioPrm l_hio_init = {3000.0f, 1000.0f, 250.0f, 0.0f, 0.0f, 0x4000, 0, 0};

static KamomeEventInit event_init_tbl[] = {
    {&daNpc_kam_c::initialWaitEvent},
    {&daNpc_kam_c::initialChangeEvent},
    {&daNpc_kam_c::initialDescendEvent},
    {&daNpc_kam_c::initialAreaOutTurn},
};
static KamomeEventAction event_action_tbl[] = {
    {&daNpc_kam_c::actionWaitEvent},
    {&daNpc_kam_c::actionDefault},
    {&daNpc_kam_c::actionDescendEvent},
    {&daNpc_kam_c::actionAreaOutTurn},
};
static char* cut_name_tbl[] = {"wait", "change", "descend", "out_turn"};
static char* event_name_tbl[] = {"OPTION_CHAR_END", "kamome_call", "kamome_out_turn"};
static KamomeAnmTbl l_anmTbl[] = {{dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, dRes_INDEX_KAMOME_BAS_KA_WAIT1_e},
    {dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e},
    {dRes_INDEX_KAMOME_BCK_KA_SING1_e, -1}};
// HD: WAIT1 uses a 40-frame morph; the debug register can force eight.
static KamomeAnmPrm l_anmPrm[] = {
    {0, {0, 0, 0}, 2, 40.0f, 1.0f, 0},
    {1, {0, 0, 0}, 2, 8.0f, 1.0f, 0},
    {2, {0, 0, 0}, 0, 8.0f, 1.0f, 0},
};

/* 022609E0 */
static daNpc_kam_HIO1_c* daNpc_kam_HIO1_c_ct(daNpc_kam_HIO1_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daNpc_kam_HIO1_c*)operator_new(0x2C);
        if (i_this == nullptr) {
            return nullptr;
        }
    }
    i_this->mSpeedF = 17.0f;
    i_this->mUnused08 = 0.1f;
    i_this->mGlidingAngVelY = 0x2000;
    i_this->mGlidingAngVelX = 0x1555;
    i_this->mMaxAngleZ = 0x2000;
    i_this->mAccelF = 0.7f;
    i_this->mFlappingSpeedF = 23.0f;
    i_this->mFlappingAngVelY = 0x2710;
    i_this->mFlappingAngVelX = 0x2000;
    i_this->mAngVelStepScale = 5;
    i_this->mAngVelMaxStep = 0x1000;
    i_this->mAngVelMinStep = 0x0400;
    i_this->mFlapDuration = 30;
    i_this->mFlapExhaustedDuration = 150;
    i_this->mFlapEnergyDuration = 150;
    i_this->__vtbl = 0x1001B55C;
    return i_this;
}

/* 02260A9C */
static daNpc_kam_HIO_c* daNpc_kam_HIO_c_ct(daNpc_kam_HIO_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daNpc_kam_HIO_c*)operator_new(0x50);
        if (i_this == nullptr) {
            return nullptr;
        }
    }
    i_this->__vtbl = 0x1001B56C;
    daNpc_kam_HIO1_c_ct(&i_this->mHio1);
    i_this->mNo = -1;
    /* static const hio_prm_c init_data (3000, 1000, 250, 0, 0, 0x4000, 0): prm = init_data */
    kam_copyWords(&i_this->prm, &l_hio_init, 6);
    return i_this;
}

/* 0225D354 */
static s16 daNpc_kam_XyCheckCB(void* i_this, int i_itemBtn) {

    return static_cast<daNpc_kam_c*>(i_this)->XyCheckCB(i_itemBtn);
}

/* 0225D2FC */
s16 daNpc_kam_c::XyCheckCB(int i_itemBtn) {

    if (dComIfGp_getSelectItem(i_itemBtn) == dItemNo_HYOI_PEAR_e) {
        return (s16)callDemoStartCheck();
    }
    return FALSE;
}

/* 0225D0D4 */
BOOL daNpc_kam_c::callDemoStartCheck() {

    if (l_demo_start_chk_cnt != 0) {
        return l_demo_start_chk_flag;
    }
    l_demo_start_chk_cnt = (s16)(l_demo_start_chk_cnt + 1);
    l_demo_start_chk_flag = 0;

    daPy_py_c* link = daPy_getPlayerLinkActorClass();
    gabi::Local<cXyz> hyoi_pear_pos;
    hyoi_pear_pos->x = link->mHeadTopPos.x;
    hyoi_pear_pos->y = link->mHeadTopPos.y;
    hyoi_pear_pos->z = link->mHeadTopPos.z;
    hyoi_pear_pos->y = hyoi_pear_pos->y + 20.0f;

    gabi::Local<cXyz> descend_start_pos;
    gabi::Local<cXyz> line_start_pos;
    gabi::Local<cXyz> line_end_pos;

    s16 angleY = (s16)(link->shape_angle.y + 0x8000); /* shape_angle.y */
    s16 angleX = -0x1555;
    mLinChk.mBackFlag = 1;

    line_end_pos->x = hyoi_pear_pos->x;
    line_end_pos->y = hyoi_pear_pos->y;
    line_end_pos->z = hyoi_pear_pos->z;

    cXyz* kyori = &l_call_local_kyori;
    for (int i = 0; i < 0x10; i++) {
        mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)(angleY + 0x8000));
        PSMTXMultVec(mDoMtx_stack_c::get(), kyori, line_end_pos);
        mDoMtx_YrotS(mDoMtx_stack_c::get(), angleY);
        PSMTXMultVec(mDoMtx_stack_c::get(), kyori, line_start_pos);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), angleX);
        PSMTXMultVec(mDoMtx_stack_c::get(), kyori, descend_start_pos);

        PSVECAdd(line_end_pos, hyoi_pear_pos, line_end_pos);
        PSVECAdd(line_start_pos, hyoi_pear_pos, line_start_pos);
        PSVECAdd(descend_start_pos, line_start_pos, descend_start_pos);

        dBgS_LinChk_Set(&mLinChk, descend_start_pos, line_start_pos, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
            dBgS_LinChk_Set(&mLinChk, line_start_pos, line_end_pos, this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
                /* found a direction with no collisions */
                kam_copyWords(&mDescendStartPos, descend_start_pos.get(), 3);
                kam_copyWords(&mDescendStartPosUnangled, line_start_pos.get(), 3); /* unused */
                mDescendStartAngle.x = (s16)-angleX;
                mDescendStartAngle.y = (s16)(angleY + 0x8000);
                mDescendStartAngle.z = 0;
                mLinChk.mBackFlag = 0;
                l_demo_start_chk_flag = 1;
                return TRUE;
            }
        }
        angleY = (s16)(angleY + 0x2000);
    }
    mLinChk.mBackFlag = 0;
    return FALSE;
}

/* 0225D378 */
static s16 daNpc_kam_XyEventCB(void* i_this, int i_itemBtn) {

    return static_cast<daNpc_kam_c*>(i_this)->XyEventCB(i_itemBtn);
}

/* 0225D358 */
s16 daNpc_kam_c::XyEventCB(int i_itemBtn) {

    onEventAccept();
    mCurrEventIdxIdx = 1;
    return mEventIdxs[1];
}

/* 0225F314 */
static void daNpc_kam_c_dtor(daNpc_kam_c* i_this, s32 flags) {

    if (i_this == nullptr) {
        return;
    }
    u32 t = gabi::ea(i_this);
    i_this->__vtbl = KAM_VTBL;
    if (l_act == t) {
        l_act = 0;
    }
    dComIfG_resDelete(&i_this->mPhs, "Kamome" );
    if (i_this->heap.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    daNpc_kam_HIO_c& hio = l_HIO;
    if (hio.mNo >= 0) {
        mDoHIO_deleteChild(hio.mNo);
        hio.mNo = -1;
    }
    daNpc_kam_c::offHyoiKamome();
    /* member destructors */
    kam_destroyObjectInfo(&i_this->mCps, 2);   /* dCcD_GObjInf::~dCcD_GObjInf */
    kam_destroySphere(&i_this->mAtSph, 2); /* dCcD_Sph::~dCcD_Sph */
    kam_destroyStatus(&i_this->mStts, 2);  /* dCcD_Stts::~dCcD_Stts */
    i_this->mLinChk.dBgS_PolyPassChk::__vtbl = 0x1001B54C;    /* dBgS_LinChk vtables */
    i_this->mLinChk.dBgS_GrpPassChk::__vtbl = 0x1001B49C;
    i_this->mLinChk.cBgS_PolyInfo::__vtbl = 0x1001B48C;
    kam_destroyLineCheck(&i_this->mLinChk, 0); /* cBgS_LinChk::~cBgS_LinChk */
    kam_destroyCircleGeometry(gabi::ea(&i_this->mAcchCir.m_cir), 2);       /* mAcchCir: cM3dGCir::~cM3dGCir */
    i_this->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];    /* dBgS_ObjAcch vtables */
    i_this->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
    kam_destroyAcch(&i_this->mAcch, 0);  /* dBgS_Acch::~dBgS_Acch */
    kam_destroyPlayer(i_this, 0);          /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0225F4D4 */
// HD: the int parameter is gone; with `false` the attention point is placed ahead of the
// player (debug-register offsets), with `true` at the seagull
void daNpc_kam_c::setAttention(bool param_1) {

    if (param_1) {
        f32 hy = mHeadTopPos.y;
        f32 hz = mHeadTopPos.z;
        f32 py = current.pos.y;
        f32 px = current.pos.x;
        eyePos.z = hz;
        eyePos.y = hy - 20.0f;
        eyePos.x = mHeadTopPos.x;

        attention_info.position.y = py; /* attention_info.position */
        attention_info.position.x = px;
        attention_info.position.z = current.pos.z;
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
        gabi::Local<cXyz> local;
        gabi::Local<cXyz> pos;
        local->y = REG_F(10, 10) + 100.0f;
        local->x = 0.0f;
        local->z = REG_F(10, 11) + 1000.0f;
        MtxPosition(local, pos);
        PSVECAdd(pos, &player->current.pos, pos);

        f32 x = pos->x, y = pos->y, z = pos->z;
        eyePos.z = z;
        eyePos.x = x;
        eyePos.y = y;
        attention_info.position.x = x;
        attention_info.position.y = y;
        attention_info.position.z = z;
    }
}

/* 0225E304 */
void daNpc_kam_c::setBaseMtx() {

    /* static cXyz l_offset(0, 30, 0) */
    if (offset_guard == 0) {
        offset_guard = 1;
        l_offset.y = 30.0f;
        l_offset.x = 0.0f;
        l_offset.z = 0.0f;
    }
    J3DModel* model = mpMorf->getModel();
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    if (REG0_S(0) == 0) {
        mDoMtx_stack_transM(l_offset.x, l_offset.y, l_offset.z);
    }
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    if (REG0_S(0) != 0) {
        mDoMtx_stack_transM(l_offset.x, l_offset.y, l_offset.z);
    }
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    mpMorf->calc();
}

/* 0225D37C */
static BOOL headNodeCallBack(J3DNode* node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys.getModel();
        daNpc_kam_c* i_this = static_cast<daNpc_kam_c*>(model->getUserArea());
        if (i_this != 0) {
            /* static cXyz l_offsetAttPos(0, 20, 0) */
            if (offsetAttPos_guard == 0) {
                offsetAttPos_guard = 1;
                l_offsetAttPos.y = 20.0f;
                l_offsetAttPos.x = 0.0f;
                l_offsetAttPos.z = 0.0f;
            }
            u32 jntNo = reinterpret_cast<KamomeJointNumber*>(J3DNode_toJoint(node))->mJointNo;
            PSMTXCopy(kam_getAnmMtx(model, jntNo), mDoMtx_stack_c::get());
            PSMTXMultVec(mDoMtx_stack_c::get(), &l_offsetAttPos, &i_this->mHeadTopPos);
        }
    }
    return TRUE;
}

/* 0225D44C */
BOOL daNpc_kam_c::createHeap() {

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Kamome", dRes_INDEX_KAMOME_BDL_KA_HYOI_e, KAM_SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_npc_kamome.cpp", 0x36F, "modelData != (0)" );
    }
    void* anm = dComIfG_getObjectRes("Kamome", dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, KAM_SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, (J3DAnmTransform*)anm, 2 /* EMode_LOOP */, 1.0f,
                                    0, -1, 1, nullptr, 0x00080000, 0x11000002);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s16 head1JntIdx = (s16)JUTNameTab_getIndex_kam(kam_getJointName(modelData), "j_ka_head1");
    if (head1JntIdx >= 0) {
        kam_setJointCallBack(modelData, (u16)head1JntIdx, headNodeCallBack);
    }
    m_jnt_body = (s16)JUTNameTab_getIndex_kam(kam_getJointName(modelData), "j_ka_spin1");
    if (m_jnt_body < 0) {
        JUT_ASSERT_fail("d_a_npc_kamome.cpp", 0x38B, "m_jnt_body >= 0" );
    }
    mpMorf->getModel()->setUserArea(gabi::ea(this)); /* setUserArea */
    return TRUE;
}

/* 0225D614 */
static BOOL checkCreateHeap(fopAc_ac_c* i_this) {

    return static_cast<daNpc_kam_c*>(i_this)->createHeap();
}

/* 0225F064 */
cPhs_State daNpc_kam_c::create() {

    /* fopAcM_ct(this, daNpc_kam_c): the constructor, inlined */
    if (!(actor_condition & 8)) {
        if (this != nullptr) {

            kam_constructPlayer(this); /* daPy_npc_c::daPy_npc_c */
            __vtbl = KAM_VTBL;
            dBgS_ObjAcch_ct(&mAcch, gc_tu.objAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dBgS_LinChk_ct(&mLinChk, dBgS_LinChk_vt{0x1001B51C, 0x1001B52C, 0x1001B54C, 0x1001B53C}, false);
            dCcD_Stts_ct(&mStts);
            kam_constructSphere(&mAtSph); /* dCcD_Sph::dCcD_Sph */
            /* dCcD_Cps mCps */
            kam_constructObjectInfo(&mCps); /* dCcD_GObjInf::dCcD_GObjInf */
            reinterpret_cast<KamomeCapsule*>(&mCps)->mShapeVtable = 0x100015A8;
            reinterpret_cast<KamomeCapsule*>(&mCps)->mGeometryVtable = 0x1001B46C;
            kam_constructCapsuleGeometry(gabi::ea(&reinterpret_cast<KamomeCapsule*>(&mCps)->mCps)); /* cM3dGCps::cM3dGCps */
            reinterpret_cast<KamomeCapsule*>(&mCps)->mCps.mLine.__vtbl = 0x1004AF60;
            reinterpret_cast<KamomeCapsule*>(&mCps)->mShapeVtable = 0x1004AF70;
            reinterpret_cast<KamomeCapsule*>(&mCps)->__vtbl_hitinf = 0x1004AF18;
            /* cBgS_PolyInfo mPolyInfo */
            mPolyInfo.mpBgW = 0;
            mPolyInfo.__vtbl = 0x1001B47C;
            mPolyInfo.mPolyIndex = 0xFFFF;
            mPolyInfo.mBgIndex = 0x100;
            mPolyInfo.mProcId = -1;
        }
        actor_condition = actor_condition | 8;
    }

    u32 act = l_act;
    if (act != 0 && act != gabi::ea(this)) {
        return cPhs_ERROR_e;
    }
    l_act = gabi::ea(this);

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, "Kamome" );
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x0225D614 /* checkCreateHeap */, 0x1360)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        daNpc_kam_HIO_c& hio = l_HIO;
        if (hio.mNo < 0) {
            s8 no = mDoHIO_createChild("\u0082\u00a9\u0082\u00e0\u0082\u00df" , &hio);
            hio.mpActor = gabi::ea(this);
            hio.mNo = no;
        }
        if (!init()) {
            phase_state = cPhs_ERROR_e;
        }
    }
    return phase_state;
}

/* 0225EE74 */
// HD: mMinY is 0 (GameCube: home.y - range); one wall circle sized by debug registers;
// roof correction height 40 (GameCube 20); no tg sphere
BOOL daNpc_kam_c::init() {

    offHyoiKamome();
    mCurrEventIdxIdx = -1;
    mUnusedC06 = 0;
    mEventState = -1;
    PSVECScale(&scale, &scale, 1000.0f);
    mMinY = 0.0f;
    mMaxY = home.pos.y + scale.y;

    daNpc_kam_HIO1_c& hio = l_HIO.mHio1;
    mTargetSpeedF = hio.mSpeedF;
    mAngVelY = hio.mGlidingAngVelY;
    mAngVelX = hio.mGlidingAngVelX;
    mTargetAngVelY = hio.mGlidingAngVelY;
    mTargetAngVelX = hio.mGlidingAngVelX;

    gabi::Local<KamomeAction> fn;
    kam_copyAction(fn, &l_waitNpcAction);
    setNpcAction(fn, nullptr);

    mAcchCir.SetWall(REG_F(10, 4) + 20.0f, REG_F(10, 5) + 50.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mAcch.m_flags = (mAcch.m_flags & ~8u) | 0x62000; /* ClrRoofNone, OnLineCheck, OnSeaCheckOn, OnSeaWaterHeight */
    mAcch.SetRoofCrrHeight(40.0f);
    mStts.Init(100, 0xFF, this);
    mAtSph.Set(&l_sph_src);
    mAtSph.SetStts(&mStts);
    kam_setCapsuleSource(&mCps, gabi::ea(&l_kam_at_cps_src)); /* dCcD_Cps::Set */
    mCps.SetStts(&mStts);

    setBaseMtx();

    attention_info.flags = 0; /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 38; /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 38; /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */

    for (int i = 0; i < 3; i++) {
        const char* name = event_name_tbl[i];
        mEventIdxs[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }

    eventInfo.setXyCheckCB(daNpc_kam_XyCheckCB); /* eventInfo.setXyCheckCB(daNpc_kam_XyCheckCB) */
    eventInfo.setXyEventCB(daNpc_kam_XyEventCB); /* eventInfo.setXyEventCB(daNpc_kam_XyEventCB) */
    return TRUE;
}

/* 0225DDF0 */
BOOL daNpc_kam_c::setAction(KamomeAction* cur, KamomeAction* newFunc, void* arg) {

    s16 newI = newFunc->i;
    s16 newD;
    u32 newF;
    bool endCurrentAction = true;
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
            endCurrentAction = false;
        }
    }
    if (endCurrentAction) {
        mActionStatus = ACTION_ENDING;
        kam_callAction<BOOL>(this, cur, arg);
    }
    cur->i = newI;
    cur->f = newF;
    cur->d = newD;
    mUnusedC14 = 0.0f;
    mUnusedC0E = 0;
    mC0A = 0;
    mC0C = 0;
    mC08 = 0;
    mActionStatus = ACTION_STARTING;
    kam_callAction<BOOL>(this, cur, arg);
    return TRUE;
}

/* 0225E108 */
void daNpc_kam_c::npcAction(void* arg) {

    if (mCurrNpcActionFunc.i == 0) {
        speedF = 0.0f;
        gabi::Local<KamomeAction> fn;
        kam_copyAction(fn, &l_waitNpcAction);
        offHyoiKamome();
        setNpcAction(fn, nullptr);
        kam_setAudioFlag(0); /* mDoAud_zelAudio_c::getInterface()->field_0x0062 = 0 */
    }
    kam_callAction<BOOL>(this, &mCurrNpcActionFunc, arg);
}

/* 0225E0B8 */
void daNpc_kam_c::setNpcAction(KamomeAction* actionFunc, void* arg) {

    mCurrPlayerActionFunc.d = 0;
    mCurrPlayerActionFunc.i = 0;
    mCurrPlayerActionFunc.f = 0;
    gabi::Local<KamomeAction> fn; /* by-value copy */
    kam_copyAction(fn, actionFunc);
    setAction(&mCurrNpcActionFunc, fn, arg);
}

/* 0225DF88 */
void daNpc_kam_c::playerAction(void* arg) {

    if (mCurrPlayerActionFunc.i == 0) {
        speedF = 0.0f;
        gabi::Local<KamomeAction> fn;
        kam_copyAction(fn, &l_waitPlayerAction);
        setPlayerAction(fn, nullptr);
    }
    dComIfGp_setRStatusForce(dActStts_RETURN_e);    /* dComIfGp_setRStatusForce(dActStts_RETURN_e) */
    dComIfGp_setDoStatus(dActStts_FLY_e); /* dComIfGp_setDoStatus(dActStts_FLY_e) */
    dComIfGp_setAStatus(dActStts_HIDDEN_e); /* dComIfGp_setAStatus(dActStts_HIDDEN_e) */
    kam_callAction<BOOL>(this, &mCurrPlayerActionFunc, arg);
}

/* 0225DF38 */
void daNpc_kam_c::setPlayerAction(KamomeAction* actionFunc, void* arg) {

    mCurrNpcActionFunc.d = 0;
    mCurrNpcActionFunc.i = 0;
    mCurrNpcActionFunc.f = 0;
    gabi::Local<KamomeAction> fn; /* by-value copy */
    kam_copyAction(fn, actionFunc);
    setAction(&mCurrPlayerActionFunc, fn, arg);
}

/* 0225E064 */
// returnLinkCheck (not named by the matcher). HD: one pad check (out of line), the
// GameCube R-or-START test
BOOL daNpc_kam_c::returnLinkCheck() {

    if (!dComIfGp_event_runCheck() ) {
        if (kam_padRorStart(0)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 0225FABC */
BOOL daNpc_kam_c::changeAreaCheck() {

    /* Can Link take control of a Hyoi Seagull from where he stands? */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    BOOL isForestHaven = FALSE;
    bool canChange = false;
    if (kam_isStartStage("sea") && current.roomNo == 41) {
        isForestHaven = TRUE;
    }
    if (isForestHaven) {
        /* Forest Haven sector: Link must stand on the rock with the Nintendo Gallery entrance */
        if (center_guard == 0) {
            center_guard = 1;
            l_center.z = 191870.0f;
            l_center.x = 211555.0f;
            l_center.y = 1000.0f;
        }
        cXyz* center = &l_center;
        if (player->current.pos.y > center->y) {
            f32 dist = kam_absXZ_delta(&player->current.pos, center);
            if (dist < 1300.0f) {
                canChange = true;
            }
        }
    } else {
        f32 dist = kam_absXZ_delta(&player->current.pos, &home.pos);
        if (dist < scale.x) {
            canChange = true;
        }
    }
    if (canChange) {
        mNoResetFlg1 = mNoResetFlg1 & ~0x40u;
        return TRUE;
    }
    mNoResetFlg1 = mNoResetFlg1 | 0x40;
    return FALSE;
}

/* 0225F8D4 */
BOOL daNpc_kam_c::areaOutCheck() {

    f32 dist = kam_absXZ_delta(&current.pos, &home.pos);
    return dist > scale.x + REG_F(10, 0) ? TRUE : FALSE;
}

/* 0225FD54 */
BOOL daNpc_kam_c::getStickAngY(be<s16>* pTargetAngleY, be<s16>* pTargetAngleZ) {

    BOOL isTurning = FALSE;
    s16 deltaAngleY = 0;
    s16 targetAngleZ = 0;
    if (CPad_GET_STICK_POS_X_kam() != 0.0f) {
        f32 sx = CPad_GET_STICK_POS_X_kam();
        deltaAngleY = (s16)gabi::ftoi(sx * (f32)(-(s32)mAngVelY));
        sx = CPad_GET_STICK_POS_X_kam();
        targetAngleZ = (s16)gabi::ftoi(sx * (f32)(s32)l_HIO.mHio1.mMaxAngleZ);
        isTurning = TRUE;
    } else if (wallHitCheck() >= 0) {
        if (isLeftLineHit()) {
            deltaAngleY = -0x4000;
            targetAngleZ = l_HIO.mHio1.mMaxAngleZ;
            isTurning = TRUE;
        } else if (isRightLineHit()) {
            deltaAngleY = 0x4000;
            targetAngleZ = (s16)-l_HIO.mHio1.mMaxAngleZ;
            isTurning = TRUE;
        } else if (isFrontLineHit()) {
            deltaAngleY = -0x4000;
            targetAngleZ = l_HIO.mHio1.mMaxAngleZ;
            isTurning = TRUE;
        }
    }
    u32 camera = gabi::ea(dComIfGp_getCamera(0)); /* dComIfGp_getCamera(0) */
    s16 angleY = kam_cameraAngleY(camera);     /* dCam_getControledAngleY */
    *pTargetAngleY = (s16)(deltaAngleY + angleY);
    *pTargetAngleZ = targetAngleZ;
    return isTurning;
}

/* 0225FEA4 */
// getAngleX (not named by the matcher). HD: on the sea in room 44, close to one spot, the
// seagull is turned down (as above its maximum height)
s16 daNpc_kam_c::getAngleX() {

    s16 angle;
    BOOL special = FALSE;
    if (current.pos.y < mMinY || mAcch.ChkGroundHit() || isWaterHit()) {
        /* turn up */
        angle = (s16)-mAngVelX;
        mLockAngleXTimer = 30;
    } else if (current.pos.y > mMaxY || mAcch.ChkRoofHit()) {
        /* turn down */
        angle = mAngVelX;
        mLockAngleXTimer = 30;
    } else {
        f32 sy = CPad_GET_STICK_POS_Y_kam();
        angle = (s16)gabi::ftoi(sy * (f32)(s32)mAngVelX);
    }
    if (kam_isStartStage("sea") && current.roomNo == 44) {
        special = TRUE;
    }
    if (special) {
        f32 dy = (2100.0f - current.pos.y) + REG_F(18, 7);
        f32 dx = -195260.0f - current.pos.x;
        f32 dz = 313671.0f - current.pos.z;
        if (dy < 50.0f && dy > -50.0f) {
            if (gabi::fmadds(dx, dx, dz * dz) < REG_F(18, 8) + 250000.0f) {
                angle = mAngVelX;
                mLockAngleXTimer = 30;
            }
        }
    }
    return angle;
}

/* 0225FD2C */
int daNpc_kam_c::wallHitCheck() {

    if (mAcch.ChkWallHit() && mAcchCir.ChkWallHit()) {
        return 0;
    }
    return -1;
}

/* 02260220 */
// HD: the seagull waits hidden at its home position (no flight, no animation) until
// Link calls it
BOOL daNpc_kam_c::waitNpcAction(void*) {

    kam_copyWords(&current, &home, 5); /* current = home */
    mHidden = 1;

    if (changeAreaCheck()) {
        attention_info.flags = attention_info.flags | (fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_NOTALK_e); /* ACTION_SPEAK | TALKFLAG_NOTALK */
        mEventState = 6;
    } else {
        attention_info.flags = attention_info.flags & ~(fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_NOTALK_e);
        mEventState = -1;
    }
    setAttention(false);
    return TRUE;
}

/* 022600FC */
BOOL daNpc_kam_c::keyProc() {

    daNpc_kam_HIO1_c& hio = l_HIO.mHio1;
    if (kam_calcTimer(&mFlapExhaustedTimer) == 0) {
        if (kam_padTriggerA(0) /* CPad_CHECK_TRIG_A(0) */) {
            mFlapTimer = hio.mFlapDuration;
        }
        if (kam_calcTimer(&mFlapTimer) != 0) {
            /* flapping */
            mTargetSpeedF = hio.mFlappingSpeedF;
            mTargetAngVelY = hio.mFlappingAngVelY;
            mTargetAngVelX = REG0_S(0) != 0 ? (s16)0x1555 : (s16)hio.mFlappingAngVelX;
            if (kam_calcTimer(&mFlapEnergyTimer) == 0) {
                /* flapping for too long without rest: exhausted */
                mFlapExhaustedTimer = hio.mFlapExhaustedDuration;
            }
        } else {
            /* gliding */
            mTargetSpeedF = hio.mSpeedF;
            mTargetAngVelY = hio.mGlidingAngVelY;
            mTargetAngVelX = REG0_S(0) != 0 ? (s16)0xAAA : (s16)hio.mGlidingAngVelX;
            /* build up energy to flap while gliding */
            mFlapEnergyTimer = (s16)(mFlapEnergyTimer + 1);
            if (mFlapEnergyTimer > hio.mFlapEnergyDuration) {
                mFlapEnergyTimer = hio.mFlapEnergyDuration;
            }
        }
    } else {
        mFlapTimer = 0;
        mFlapEnergyTimer = hio.mFlapEnergyDuration;
    }
    return TRUE;
}

/* 02260510 */
// HD: with REG0_S(0) == 0 (the release value) the steering turns shape_angle quickly and
// lets current.angle follow (GameCube: current.angle turns and shape_angle copies it)
BOOL daNpc_kam_c::waitPlayerAction(void*) {

    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = 1; /* ACTION_ONGOING_1 */
        onHyoiKamome();
        setAnm(ANM_WAIT1);
        daNpc_kam_HIO1_c& hio = l_HIO.mHio1;
        speedF = hio.mSpeedF;
        mTargetSpeedF = hio.mSpeedF;
        mAngVelY = hio.mGlidingAngVelY;
        mAngVelX = hio.mGlidingAngVelX;
        mTargetAngVelY = hio.mGlidingAngVelY;
        mTargetAngVelX = hio.mGlidingAngVelX;
        mC08 = (s16)kam_getRndValue(60, 90);
        mFlapTimer = 0;
        mC0A = 0;
        mFlapExhaustedTimer = 0;
        mFlapEnergyTimer = hio.mFlapEnergyDuration;
        return TRUE;
    }
    if (mActionStatus == ACTION_ENDING) {
        offHyoiKamome();
        return TRUE;
    }

    keyProc();
    kam_calcTimer(&mLockAngleXTimer);
    if (!dComIfGp_event_runCheck()  && areaOutCheck()) {
        mEventState = 2;
    }

    gabi::Local<be<s16>> targetAngleZ;
    gabi::Local<be<s16>> targetAngleY;
    *targetAngleZ = 0;
    *targetAngleY = 0;
    if (getStickAngY(targetAngleY, targetAngleZ)) {
        if (REG0_S(0) == 0) {
            cLib_addCalcAngleS2(&shape_angle.y, *targetAngleY, 8, 0x3000);
        } else {
            cLib_addCalcAngleS(&current.angle.y, *targetAngleY, 8, 0x2000, 0x400);
        }
    }
    if (REG0_S(0) == 0) {
        cLib_addCalcAngleS2(&current.angle.y, shape_angle.y, 8, 0x400);
    } else {
        shape_angle.y = current.angle.y;
    }

    if (mLockAngleXTimer == 0) {
        mTargetAngleX = getAngleX();
    }

    if (REG0_S(0) == 0) {
        cLib_addCalcAngleS2(&shape_angle.x, mTargetAngleX, 8, 0x3000);
        cLib_addCalcAngleS2(&shape_angle.z, *targetAngleZ, 8, 0x3000);
    } else {
        cLib_addCalcAngleS(&current.angle.x, mTargetAngleX, 8, 0x2000, 0x400);
        cLib_addCalcAngleS(&current.angle.z, *targetAngleZ, 8, 0x2000, 0x400);
    }
    setAttention(true);

    if (mAnmIdx != ANM_WAIT2) {
        if (mFlapTimer != 0) {
            setAnm(ANM_WAIT2);
        } else if (kam_calcTimer(&mC08) == 0) {
            mC08 = (s16)kam_getRndValue(90, 90);
            setAnm(ANM_SING);
        }
        if (mAnmIdx == ANM_SING) {
            if (mReachedAnimEnd) {
                setAnm(ANM_WAIT1);
            }
            if (mAnmIdx == ANM_SING && mpMorf->checkFrame(8.0f)) {
                mDoAud_seStart(JA_SE_CV_KAMOME, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
            }
        }
    } else if (mReachedAnimEnd && mFlapTimer == 0) {
        setAnm(ANM_SING);
    }

    if (mAtSph.ChkTgHit()) { // HD: the hit check uses the at sphere

        gabi::Local<KamomeAction> fn;
        kam_copyAction(fn, &l_damagePlayerAction);
        setPlayerAction(fn, nullptr);
    }
    return TRUE;
}

/* 022608A4 */
BOOL daNpc_kam_c::damagePlayerAction(void*) {

    if (mActionStatus == ACTION_STARTING) {
        gabi::Local<cXyz> dir;
        dir->x = 0.0f;
        dir->y = 1.0f;
        dir->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x21, dir);
        mDamageFogTimer = 150; /* setDamageFogTimer(150) */
        mActionStatus = (s8)(mActionStatus + 1); /* ACTION_ONGOING_1 */
        onHyoiKamome();
        setAnm(ANM_WAIT1);
        speedF = 0.0f;
        mDoAud_seStart(JA_SE_CV_KAMOME, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
        mC08 = 30;
    } else if (mActionStatus == ACTION_ENDING) {
        offHyoiKamome();
    } else {
        s16 y = (s16)(shape_angle.y + 0x2000);
        current.angle.y = y;
        shape_angle.y = y;
        setAttention(true);
        if (kam_calcTimer(&mC08) == 0) {
            returnLink();
        }
    }
    return TRUE;
}

/* 0225E1E0 */
void daNpc_kam_c::eventOrder() {

    if (isEventAccept()) {
        return;
    }
    if (mEventState == 5 || mEventState == 4) {
        if (dComIfGp_getPlayer(0) != this) {
            eventInfo.onCondition(dEvtCnd_CANTALK_e);
            if (mEventState == 5) {
                fopAcM_orderSpeakEvent(this);
            }
        } else {
            mEventState = -1;
        }
    } else if (mEventState == 6) {
        if (dComIfGp_getPlayer(0) != this) {
            eventInfo.onCondition(dEvtCnd_CANTALKITEM_e | dEvtCnd_CANTALK_e);
        } else {
            mEventState = -1;
        }
    } else if (mEventState != -1 && mEventState < 3) {
        s8 st = mEventState;
        mCurrEventIdxIdx = st;
        fopAcM_orderOtherEventId(this, mEventIdxs[st], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 0225D6B4 */
BOOL daNpc_kam_c::checkCommandTalk() {

    if (eventInfo.checkCommandTalk()) {
        if ((u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3) {
            if (mEventState == 6) {
                mEventState = -1;
            }
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/* 0225D738 */
void daNpc_kam_c::returnLinkPlayer() {

    daPy_py_changePlayer(this, dComIfGp_getLinkPlayer());
    offHyoiKamome();
    kam_setAudioFlag(0); /* mDoAud_zelAudio_c::getInterface()->field_0x0062 = 0 */
}

/* 0225D7F8 */
BOOL daNpc_kam_c::eventProc() {

    if (eventInfo.checkCommandDemoAccrpt()  && mEventState != -1) {
        if (mEventState == 0) {
            if (dEvmng_startCheckOld("OPTION_CHAR_END" ) || dEvmng_endCheckOld("OPTION_CHAR_END")) {
                /* dComIfGp_event_setTalkPartner(dComIfGp_getLinkPlayer()) */
                daPy_py_c* link = daPy_getPlayerLinkActorClass();
                dEvt_control_c* evt = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mEvtCtrl;
                evt->mPtTalk = kam_eventGetPId(gabi::ea(evt), gabi::ea(link));
                kam_seStartSimple(JA_SE_CTRL_NPC_TO_LINK); /* mDoAud_seStart(JA_SE_CTRL_NPC_TO_LINK) */
            } else {
                offReturnLink();
                mEventState = -1;
            }
        }
        if (mEventState != -1) {
            onEventAccept();
            mEventState = -1;
        }
    }

    const char* staffName = l_staff_name.get();
    s32 staffId = dComIfGp_evmng_getMyStaffId(staffName, nullptr, 0);
    if (dComIfGp_event_runCheck()  && !checkCommandTalk()) {
        if (staffId != -1) {
            s32 actIdx = kam_getMyActIdx(dComIfGp_getPEvtManager(), staffId, gabi::ea(cut_name_tbl), 4, 1, 0);
            dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
            if (actIdx == -1) {
                kam_cutEnd(evmng, staffId); /* cutEnd */
            } else {
                if (kam_getIsAdvance(evmng, staffId)) { /* getIsAddvance */
                    (this->*event_init_tbl[actIdx].action)(staffId);
                }
                if ((this->*event_action_tbl[actIdx].action)(staffId)) {
                    dComIfGp_evmng_cutEnd(staffId);
                }
            }
        }
        if (isEventAccept()) {
            s16 idx = mEventIdxs[mCurrEventIdxIdx];
            if (dComIfGp_evmng_endCheck(idx)) {
                eventEnd();
            }
        }
        return TRUE;
    }
    return FALSE;
}

/* 0225D78C */
void daNpc_kam_c::eventEnd() {

    dComIfGp_event_reset();
    offEventAccept();
    if (mCurrEventIdxIdx == 0) {
        returnLinkPlayer();
        offReturnLink();
    }
    mCurrEventIdxIdx = -1;
}

/* 0225F468 */
BOOL daNpc_kam_c::actionDefault(int evtStaffId) {

    return TRUE;
}

/* 0225F470 */
void daNpc_kam_c::initialWaitEvent(int evtStaffId) {

    auto* timerP = reinterpret_cast<KamomeEventTimer*>(dComIfGp_evmng_getMySubstanceP(evtStaffId, "timer", 3));
    s16 timer = 0;
    if (timerP != 0) {
        timer = timerP->mValue; /* (s16)*timerP */
    }
    mWaitTimer = timer;
}

/* 0225F5E0 */
BOOL daNpc_kam_c::actionWaitEvent(int evtStaffId) {

    setAttention(true);
    return kam_calcTimer(&mWaitTimer) == 0 ? TRUE : FALSE;
}

/* 0225F620 */
void daNpc_kam_c::initialChangeEvent(int evtStaffId) {

    current.roomNo = home.roomNo; /* fopAcM_SetRoomNo(this, fopAcM_GetHomeRoomNo(this)) */
    daPy_py_changePlayer(this, this);
    kam_emptyBaitItem(gabi::ea(&dComIfGs_info()->mSavedata.mPlayer.mBagItem));
    mHitFlags = mHitFlags & ~0x10u;
}

/* 0225F674 */
// HD: the event ends at once when the descent would start more than 500 above Link;
// the seagull is shown again (mHidden); descent timer 150 frames (GameCube 900)
void daNpc_kam_c::initialDescendEvent(int evtStaffId) {

    daPy_py_c* link = daPy_getPlayerLinkActorClass();
    f32 startY = mDescendStartPos.y;
    if (startY - link->current.pos.y > 500.0f) {
        eventEnd();
        return;
    }
    current.pos.y = startY;
    current.pos.x = mDescendStartPos.x;
    current.pos.z = mDescendStartPos.z;
    s16 ax = mDescendStartAngle.x, ay = mDescendStartAngle.y, az = mDescendStartAngle.z;
    current.angle.x = ax;
    current.angle.y = ay;
    current.angle.z = az;
    shape_angle.x = ax;
    shape_angle.y = ay;
    shape_angle.z = az;

    daNpc_kam_HIO1_c& hio = l_HIO.mHio1;
    f32 speed = hio.mSpeedF;
    mTargetSpeedF = speed;
    speedF = speed;
    mAngVelY = hio.mGlidingAngVelY;
    mAngVelX = hio.mGlidingAngVelX;
    mTargetAngVelY = hio.mGlidingAngVelY;
    mTargetAngVelX = hio.mGlidingAngVelX;

    mDoAud_seStart(JA_SE_HYOI_USE_DEMO, nullptr, 0, dComIfGp_getReverb(current.roomNo));
    kam_setAudioFlag(1); /* mDoAud_zelAudio_c::getInterface()->field_0x0062 = 1 */
    mHidden = 0;
    mHitFlags = mHitFlags | 0x10;
    mWaitTimer = 150;
}

/* 0225F794 */
BOOL daNpc_kam_c::actionDescendEvent(int evtStaffId) {

    daPy_py_c* link = daPy_getPlayerLinkActorClass();
    gabi::Local<cXyz> linkHeadTopPos;
    linkHeadTopPos->x = link->mHeadTopPos.x;
    linkHeadTopPos->y = link->mHeadTopPos.y;
    linkHeadTopPos->z = link->mHeadTopPos.z;
    linkHeadTopPos->y = linkHeadTopPos->y + 20.0f;

    setAttention(true);

    f32 hy = linkHeadTopPos->y;
    f32 y = current.pos.y;
    if (y < hy) {
        y = hy;
        current.pos.y = y;
    }
    if (y < hy + 50.0f) {
        cLib_addCalcAngleS(&current.angle.x, 0, 30, 0x2000, 0x400);
        shape_angle.x = current.angle.x;
    }

    gabi::Local<cXyz> delta;
    cXyz_mi(linkHeadTopPos, delta, &current.pos);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = delta->x;
    xz->z = delta->z;
    if (PSVECSquareMag(xz) < 10000.0f /* SQUARE(100.0f) */) {
        return TRUE;
    }
    if (kam_calcTimer(&mWaitTimer) == 0) {
        return TRUE;
    }
    return FALSE;
}

/* 0225F8B4 */
void daNpc_kam_c::initialAreaOutTurn(int evtStaffId) {

    mWaitTimer = 10;
    mAreaOutTimer = 180 * 30;
    current.angle.x = 0;
    current.angle.z = 0;
}

/* 0225F950 */
BOOL daNpc_kam_c::actionAreaOutTurn(int evtStaffId) {

    if (kam_calcTimer(&mWaitTimer) == 0) {
        speedF = 0.0f;
        s16 targetAngle = cLib_targetAngleY(&current.pos, &home.pos);
        s16 angleDiff = (s16)(current.angle.y - targetAngle);
        if (angleDiff != 0) {
            cLib_addCalcAngleS(&current.angle.y, targetAngle, 16, 0x2000, 0x400);
        } else {
            speedF = l_HIO.mHio1.mSpeedF;
        }
        shape_angle.y = current.angle.y;
        current.angle.z = angleDiff;
        s32 maxZ = l_HIO.mHio1.mMaxAngleZ;
        if (angleDiff > maxZ) {
            current.angle.z = (s16)maxZ;
        } else if (angleDiff < -maxZ) {
            current.angle.z = (s16)-maxZ;
        }
        if (!areaOutCheck()) {
            return TRUE;
        }
    }
    cLib_addCalcAngleS(&shape_angle.x, current.angle.x, 8, 0x2000, 0x400);
    cLib_addCalcAngleS(&shape_angle.z, current.angle.z, 8, 0x2000, 0x400);
    setAttention(true);
    return kam_calcTimer(&mAreaOutTimer) == 0 ? TRUE : FALSE;
}

/* 022602DC */
// HD: debug register REG0_S(0) forces morf 8; the wait animation starts at a random
// frame with a random speed (0.7..1.0)
void daNpc_kam_c::setAnm(int anmIdx) {

    mAnmIdx = anmIdx;
    KamomeAnmPrm* prm = &l_anmPrm[anmIdx]; /* anmPrm_c {s8 tbl; int loop; f32 morf; f32 speed; int m10} */
    s8 tblIdx = prm->mAnmTblIdx;
    f32 playSpeed = prm->mPlaySpeed;
    if ((s32)tblIdx == (s32)mAnmTblIdx && playSpeed == mpMorf->mFrameCtrl.mRate) {
        return;
    }
    mAnmTblIdx = tblIdx;
    KamomeAnmTbl* tbl = &l_anmTbl[tblIdx]; /* anmTbl_c {s8 bck; s8 bas} */
    mPrevMorfFrame = 0.0f;
    mReachedAnimEnd = 0;
    f32 morf = REG0_S(0) == 0 ? prm->mMorf : 8.0f;
    kam_setAnimation(mpMorf.get(), prm->mLoopMode, morf, playSpeed,
               (s32)tbl->mBckFileIdx, (s32)tbl->mSoundFileIdx, "Kamome" );
    if (prm->m10 < 0) {
        mDoExt_McaMorf* m = mpMorf;
        m->setFrame(m->getEndFrame());
    }
    if (anmIdx == 0 && REG0_S(0) == 0) {
        mDoExt_McaMorf* m = mpMorf;
        s32 rnd = kam_randomValue(0, gabi::ftoi(m->getEndFrame())); /* cLib_getRndValue<int> */
        m->setFrame((f32)rnd);
        mDoExt_McaMorf* m2 = mpMorf;
        m2->mFrameCtrl.mRate = cM_rndF(0.3f) + 0.7f;
    }
}

/* 0225E444 */
// HD: one sphere; it is registered with the mass collision manager (GameCube: the
// separate tg sphere)
void daNpc_kam_c::setCollision() {

    J3DModel* model = mpMorf->getModel();
    Mtx34* mtx = kam_getAnmMtx(model, (u32)(s32)m_jnt_body);
    gabi::Local<cXyz> center;
    center->x = mtx->m[0][3];
    center->y = mtx->m[1][3];
    center->z = mtx->m[2][3];

    mAtSph.SetC(center);
    dComIfG_Ccsp_Set(&mAtSph);
    kam_massSet(dComIfGp_ea() + PLAY_CCMASS, &mAtSph, 3); /* dCcMassS_Mng::Set */

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    gabi::Local<cXyz> at_global_start;
    gabi::Local<cXyz> at_global_end;
    gabi::Local<cXyz> at_global_vec;
    PSMTXMultVec(mDoMtx_stack_c::get(), &l_ms_at_local_start, at_global_start);
    PSMTXMultVec(mDoMtx_stack_c::get(), &l_ms_at_local_end, at_global_end);
    PSMTXMultVecSR(mDoMtx_stack_c::get(), &l_ms_at_local_vec, at_global_vec);
    PSVECAdd(at_global_start, center, at_global_start);
    PSVECAdd(at_global_end, center, at_global_end);

    kam_setCapsuleStartEnd(gabi::ea(&reinterpret_cast<KamomeCapsule*>(&mCps)->mCps), at_global_start.get(), at_global_end.get()); /* cM3dGCps::SetStartEnd */
    kam_copyWords(&mCps.mGObjAt.mVec, at_global_vec.get(), 3); /* SetAtVec */
    dComIfG_Ccsp_Set(&mCps);
}

/* 0225DAC8 */
// HD: the line angle starts from current.angle; when the front line hits and BG checks are
// on, a short debug-register line forward stops the horizontal movement ("KAMOME FRONT HIT")
void daNpc_kam_c::setLineBgCheck() {

    s16 ax = current.angle.x;
    s16 az = current.angle.z;
    s16 ay = current.angle.y;
    offLineHit();
    gabi::Local<cXyz> lineEndPos;
    cXyz* localEnd = &l_line_bg_local_end;

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, ay, az);
    PSMTXMultVec(mDoMtx_stack_c::get(), localEnd, lineEndPos);
    PSVECAdd(lineEndPos, &current.pos, lineEndPos);
    dBgS_LinChk_Set(&mLinChk, &current.pos, lineEndPos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        mHitFlags = mHitFlags | 1;
        if (!isNoBgCheck()) {
            gabi::Local<cXyz> vec;
            vec->z = 20.0f;
            vec->x = 0.0f;
            vec->y = REG_F(10, 3) + 20.0f;
            mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, ay, az);
            PSMTXMultVec(mDoMtx_stack_c::get(), vec, lineEndPos);
            PSVECAdd(lineEndPos, &current.pos, lineEndPos);
            dBgS_LinChk_Set(&mLinChk, &current.pos, lineEndPos, this);
            if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
                kam_report(0x1E, 0x190, "KAMOME FRONT HIT " ); /* JUTReport */
                current.pos.x = old.pos.x;
                current.pos.z = old.pos.z;
            }
        }
    }

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, (s16)(shape_angle.y + 0x2000), az);
    PSMTXMultVec(mDoMtx_stack_c::get(), localEnd, lineEndPos);
    PSVECAdd(lineEndPos, &current.pos, lineEndPos);
    dBgS_LinChk_Set(&mLinChk, &current.pos, lineEndPos, this);
    BOOL hit = cBgS_LineCross(dComIfG_Bgsp(), &mLinChk);
    s16 sy = shape_angle.y;
    if (hit) {
        mHitFlags = mHitFlags | 2;
    }

    mDoMtx_ZXYrotS_kam(mDoMtx_stack_c::get(), ax, (s16)(sy - 0x2000), az);
    PSMTXMultVec(mDoMtx_stack_c::get(), localEnd, lineEndPos);
    PSVECAdd(lineEndPos, &current.pos, lineEndPos);
    dBgS_LinChk_Set(&mLinChk, &current.pos, lineEndPos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        mHitFlags = mHitFlags | 4;
    }
}

/* 0225DD20 */
void daNpc_kam_c::animationPlay() {

    u32 mtrlSndId = 0;
    u32 gnd = gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)); /* mAcch.m_gnd (its cBgS_PolyInfo) */
    if (mAcch.ChkGroundHit() && kam_checkPolySafe(dComIfG_Bgsp(), gnd) /* ChkPolySafe */) {
        mtrlSndId = kam_getMaterialSound(dComIfG_Bgsp(), gnd); /* GetMtrlSndId */
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mReachedAnimEnd = (s8)kam_morfPlay(mpMorf.get(), &eyePos, mtrlSndId, reverb);

    mDoExt_McaMorf* morf = mpMorf;
    f32 rate = morf->mFrameCtrl.mRate;
    f32 frame = morf->mFrameCtrl.mFrame;
    f32 prev = mPrevMorfFrame;
    if (rate < 0.0f) {
        if (frame > prev) {
            mReachedAnimEnd = 1;
        }
    } else if (frame < prev) {
        mReachedAnimEnd = 1;
    }
    mPrevMorfFrame = frame;
}

/* 0225D618 */
BOOL daNpc_kam_c::draw() {

    if (mHidden == 0) {
        J3DModel* model = mpMorf->getModel();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
        kam_drawDamageFog(this); /* daPy_npc_c::drawDamageFog */
        setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
        mpMorf->entryDL();
        dSnap_RegistFig(0x55 /* DSNAP_TYPE_KAMOME */, this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}

/* 0225D6B0 */
static BOOL daNpc_kam_Draw(daNpc_kam_c* i_this) {

    return i_this->draw();
}

/* 0225EE60 */
static BOOL daNpc_kam_Execute(daNpc_kam_c* i_this) {

    return i_this->execute();
}

/* 0225EE64 */
static BOOL daNpc_kam_IsDelete(daNpc_kam_c* i_this) {

    return TRUE;
}

/* 0225EE6C */
static BOOL daNpc_kam_Delete(daNpc_kam_c* i_this) {

    return TRUE;
}

/* 0225F310 */
static cPhs_State daNpc_kam_Create(fopAc_ac_c* i_this) {

    return static_cast<daNpc_kam_c*>(i_this)->create();
}

// HD: compiler-generated and template code of this translation unit.

/* 02260B2C */
static void __sinit_d_a_npc_kamome_cpp() {

    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    daNpc_kam_HIO_c_ct(&l_HIO);
    kam_setVec(&l_line_bg_local_end, 0.0f, 0.0f, 500.0f);
    kam_setVec(&l_ms_at_local_end, -100.0f, 20.0f, 0.0f);
    kam_setVec(&l_ms_at_local_start, 100.0f, 20.0f, 0.0f);
    l_kam_at_cps_src.mRadius = 20.0f; // HD: l_kam_at_cps_src radius set at run time

    kam_setVec(&l_call_local_kyori, 0.0f, 0.0f, 500.0f);
    kam_setVec(&l_ms_at_local_vec, 0.0f, 0.0f, -1.0f);
}

/* 02260C54 */
static void kam_SafeString_dtor(void* i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02260C68 */
static s32 kam_getTactMusic(daNpc_kam_c* i_this) {

    return -1;
}

/* 02260C70 */
static s32 kam_getTactTimerCancel(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260C78 */
static s32 kam_checkPlayerGuard(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260C80 */
static s32 kam_getGrabMissActor(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260C88 */
static s32 kam_checkPlayerFly(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260C90 */
static s32 kam_checkFrontRoll(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260C98 */
static s32 kam_checkBottleSwing(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CA0 */
static s32 kam_checkCutCharge(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CA8 */
static s32 kam_getBokoFlamePos(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CB0 */
static s32 kam_checkTactWait(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CB8 */
static void kam_onDekuSpReturnFlg(daNpc_kam_c* i_this) {

}

/* 02260CBC */
static s32 kam_checkComboCutTurn(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CC4 */
static s32 kam_getItemID(daNpc_kam_c* i_this) {

    return -1;
}

/* 02260CCC */
static s32 kam_getThrowBoomerangID(daNpc_kam_c* i_this) {

    return -1;
}

/* 02260CD4 */
static s32 kam_getGrabActorID(daNpc_kam_c* i_this) {

    return -1;
}

/* 02260CDC */
static s32 kam_checkGrabBarrel(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CE4 */
static s32 kam_checkPlayerNoDraw(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CEC */
static s32 kam_checkRopeTag(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CF4 */
static s32 kam_checkRopeReadyAnime(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260CFC */
static void kam_voiceStart(daNpc_kam_c* i_this) {

}

/* 02260D00 */
static void kam_setOutPower(daNpc_kam_c* i_this) {

}

/* 02260D04 */
static void kam_onFrollCrashFlg(daNpc_kam_c* i_this) {

}

/* 02260D08 */
static s32 kam_setHookshotCarryOffset(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260D10 */
static void kam_setPlayerPosAndAnglePosition(daNpc_kam_c* i_this) {

}

/* 02260D14 */
static void kam_setPlayerPosAndAngleRotation(daNpc_kam_c* i_this) {

}

/* 02260D18 */
static void kam_setPlayerPosAndAngleMatrix(daNpc_kam_c* i_this) {

}

/* 02260D1C */
static s32 kam_setThrowDamage(daNpc_kam_c* i_this) {

    return 0;
}

/* 02260D24 */
static s32 kam_isTagCheckOK(daNpc_kam_c* i_this) {

    return 1;
}

/* 02260D2C */
static void kam_restartPoint(daNpc_kam_c* i_this) {

}

/* 02260D30 */
static f32 kam_getGroundY(daNpc_kam_c* i_this) {

    return i_this->mAcch.GetGroundH();
}

/* 02260D38 */
static u32 kam_getLeftHandMatrix(daNpc_kam_c* i_this) {

    return i_this->cullMtx;
}

/* 02260D40 */
static u32 kam_getRightHandMatrix(daNpc_kam_c* i_this) {

    return i_this->cullMtx;
}

/* 02260D48 */
static f32 kam_getBaseAnimeFrameRate(daNpc_kam_c* i_this) {

    return 1.0f;
}

/* 02260D54 */
static f32 kam_getBaseAnimeFrame(daNpc_kam_c* i_this) {

    return 0.0f;
}

/* 02260D60 */
static void kam_assureTermination(daNpc_kam_c* i_this) {

}

/* WWHD glue (tools/verify/autoglue.py)
data l_waitNpcAction 1001B408
data l_waitPlayerAction 1001B410
data l_damagePlayerAction 1001B418
data l_hio_init 1001B6F8
str 1001B5C0 in 0225D44C
str 1001B5D8 in 0225D44C
str 1001B5EC in 0225D44C
str 1001B600 in 0225D44C
str 1001B60C in 0225D44C
str 1001B5C8 in 0225D44C
str 1001B620 in 0225D7F8
str 1001B630 in 0225DAC8
str 1001B680 in 0225F064
str 1001B688 in 0225F064
str 1001B690 in 0225F314
str 1001B698 in 0225F470
str 1001B6B8 in 0225FABC
str 1001B6D0 in 0225FEA4
str 1001B6E0 in 022602DC
data l_sph_src 101BEC54 nocheck
data l_kam_at_cps_src 101BED10 nocheck
data l_rippleCallback 1047B2E4 nocheck
data l_audio 101FFC78 nocheck
data l_anmPrm 101BED64
data l_anmTbl 101BED5C
data event_name_tbl 101BED04
data cut_name_tbl 101BECD4
data event_action_tbl 101BECB4
data event_init_tbl 101BEC94
ext 02445A00 void kam_drawDamageFog(void*)
ext 0253F124 u32 kam_eventGetPId(u32,u32)
ext 025E1988 void kam_seStartSimple(u32)
ext 02542EDC s32 kam_getMyActIdx(dEvent_manager_c*,s32,u32,s32,s32,s32)
ext 02543280 void kam_cutEnd(dEvent_manager_c*,s32)
ext 025447C8 BOOL kam_getIsAdvance(dEvent_manager_c*,s32)
ext 027EC9E8 void kam_report(s32,s32,const char*)
ext 02008254 BOOL kam_checkPolySafe(dBgS*,u32)
ext 024EECAC u32 kam_getMaterialSound(dBgS*,u32)
ext 025E535C BOOL kam_morfPlay(mDoExt_McaMorf*,cXyz*,u32,s32)
ext 02007840 BOOL kam_padRorStart(s32)
ext 02516C14 void kam_massSet(u32,void*,s32)
ext 02018808 void kam_setCapsuleStartEnd(u32,cXyz*,cXyz*)
ext 025164C0 void kam_setCapsuleSource(dCcD_Cps*,u32)
ext 024450B4 void kam_constructPlayer(void*)
ext 025166F0 void kam_constructSphere(dCcD_Sph*)
ext 02515FB8 void kam_constructObjectInfo(void*)
ext 02018150 void kam_constructCapsuleGeometry(u32)
ext 02515980 void kam_destroyObjectInfo(void*,s32)
ext 02515AE8 void kam_destroySphere(void*,s32)
ext 02515860 void kam_destroyStatus(void*,s32)
ext 02008B4C void kam_destroyLineCheck(void*,s32)
ext 02018034 void kam_destroyCircleGeometry(u32,s32)
ext 024EFD9C void kam_destroyAcch(void*,s32)
ext 0244513C void kam_destroyPlayer(void*,s32)
ext 025B6994 void kam_emptyBaitItem(u32)
ext 024F8018 s16 kam_cameraAngleY(u32)
ext 02007898 BOOL kam_padTriggerA(s32)
ext 0259D454 void kam_setAnimation(mDoExt_McaMorf*,s32,f32,f32,s32,s32,const char*)
ext 021E1E78 s32 kam_randomValue(s32,s32)
ext 02445950 BOOL kam_checkPositionMove(fopAc_ac_c*,const char*)
ext 02008DAC void kam_destroyCheck(void*,s32)
ext 024EF130 s32 kam_getGroundRoom(dBgS*,u32)
ext 024EEEB8 u32 kam_getGroundColor(dBgS*,u32)
ext 0251641C void kam_clearSphereCollision(void*)
ext 023D4688 void kam_changePlayer(fopAc_ac_c*,fopAc_ac_c*)
ext 025445B8 BOOL kam_startCheckOld(dEvent_manager_c*,const char*)
ext 0254457C BOOL kam_endCheckOld(dEvent_manager_c*,const char*)
ext 025F1AA4 void kam_rotateZXY(Mtx34*,s16,s16,s16)
ext 02055B64 s32 kam_timer(be<s16>*)
ext 0207A9A0 s32 kam_timerByte(be<u8>*)
ext 0200796C f32 kam_stickX(s32)
ext 02007990 f32 kam_stickY(s32)
ext 027F68FC KamomeNameTable* kam_jointNames(J3DModelData*)
ext 027DF9B0 s32 kam_jointIndex(u32,const char*)
data l_hyoi_kamome 101D5F45 nocheck
data l_act 101BEC48 nocheck
data l_staff_name 101BEC4C nocheck
data l_demo_start_chk_cnt 101BEC50 nocheck
data l_demo_start_chk_flag 101BEC52 nocheck
data offset_guard 10467498 nocheck
data offsetAttPos_guard 10467494 nocheck
data center_guard 104674BC nocheck
data splash_guard 1046749C nocheck
data ripple_guard 104674A0 nocheck
data l_ms_at_local_vec 104673E4 nocheck
data l_ms_at_local_start 104673F0 nocheck
data l_ms_at_local_end 104673FC nocheck
data l_line_bg_local_end 10467408 nocheck
data l_call_local_kyori 10467414 nocheck
data l_offset 10467420 nocheck
data l_offsetAttPos 1046742C nocheck
data l_center 10467438 nocheck
data splash_scale 104674A4 nocheck
data ripple_scale 104674B0 nocheck
data l_HIO 10467444 nocheck
*/
