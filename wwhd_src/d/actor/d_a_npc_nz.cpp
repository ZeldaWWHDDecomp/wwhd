/**
 * d_a_npc_nz.cpp
 * NPC - Rat (shopkeeper)
 *
 * WWHD (cking.rpx), written in the style of the GameCube decompilation (zeldaret/tww) and verified
 * function by function against the recompiled original (tools/verify, unit d_a_npc_nz). Each
 * function is marked with its WWHD address; the glue block at the end lists this translation
 * unit's data (tools/verify/autoglue.py).
 */

#include "d/actor/d_a_npc_nz.h"

/* HD: this translation unit's copies of inline-constructed vtables (GHS emits one per TU) */
const gc_tu_data gc_tu = {
    .safeString = 0x1001EB6C,
    .safeStringAssure = 0x022A6034,
    .objAcch = {0x1001EBD4, 0x1001EBF4, 0x1001EBE4},
    .objAcchDt = {0x1001EBE4, 0x1001EBF4},
    .gndChk = {0x1001EB94, 0x1001EBA4, 0x1001EBC4, 0x1001EBB4},
    .gndChkDt = {0x1001EBA4, 0x1001EBC4, 0x1001EB84},
    .headerStatics = {0x10467CDC, 0x101C2594, 0x10467D10},
};

class daNpc_Nz_HIO_c {
public:
    daNpc_Nz_HIO_c();
    enum : u32 { __vt = 0x1001EC04 }; /* HD: its vtable */

public:
    /* HD: no vtable at +0 (it is the last member); GameCube offsets are +4 */
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01;
    /* 0x02 */ be<s16> mMaxHeadX;
    /* 0x04 */ be<s16> mMaxHeadY;
    /* 0x06 */ be<s16> mMaxBackboneX;
    /* 0x08 */ be<s16> mMaxBackboneY;
    /* 0x0A */ be<s16> mMinHeadX;
    /* 0x0C */ be<s16> mMinHeadY;
    /* 0x0E */ be<s16> mMinBackboneX;
    /* 0x10 */ be<s16> mMinBackboneY;
    /* 0x12 */ be<s16> mMaxTurnStep;
    /* 0x14 */ be<s16> mMaxFollowRotVel;
    /* 0x16 */ be<s8> field_0x1A;
    /* 0x17 */ u8 _17;
    /* 0x18 */ be<f32> field_0x1C;
    /* 0x1C */ be<f32> mBaitOffset;
    /* 0x20 */ be<f32> mBaitRange;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_OFFSET(daNpc_Nz_HIO_c, mMaxFollowRotVel, 0x14);
WWHD_OFFSET(daNpc_Nz_HIO_c, mBaitOffset, 0x1C);
WWHD_OFFSET(daNpc_Nz_HIO_c, __vtbl, 0x24);
WWHD_SIZE(daNpc_Nz_HIO_c, 0x28);

const char daNpc_Nz_c::m_arc_name[] = "Nz";       // HD: "Nz" (GameCube "NZ")
const char daNpc_Nz_c::m_bdl_arc_name[] = "Npcnz";

/* 022A29B8 */
static BOOL daNpcNz_NodeCallBack(J3DNode* node, int calcTiming) {
    return reinterpret_cast<daNpc_Nz_c*>(j3dSys.getModel()->getUserArea())->NodeCallBack(node, calcTiming);
}

/* 022A2838 */
BOOL daNpc_Nz_c::NodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        J3DJoint* joint = (J3DJoint*)node;
        s32 jntNo = joint->getJntNo();
        mDoMtx_stack_c::copy(model->getAnmMtx(jntNo));

        if (jntNo == m_jnt.getHeadJntNum()) {
            cXyz temp(0.0f, 0.0f, 0.0f);
            mDoMtx_stack_c::YrotM(m_jnt.getHead_y());
            mDoMtx_stack_c::ZrotM(-m_jnt.getHead_x());
            mDoMtx_stack_c::multVec(&temp, &eyePos);
        }
        if (jntNo == m_jnt.getBackboneJntNum()) {
            mDoMtx_stack_c::XrotM(m_jnt.getBackbone_y());
            mDoMtx_stack_c::ZrotM(-m_jnt.getBackbone_x());
        }

        model->setAnmMtx(jntNo, mDoMtx_stack_c::get());
        MTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    }

    return true;
}

/* 022A2AC0 */
static BOOL daNpcNz_TailNodeCallBack(J3DNode* node, int calcTiming) {
    return reinterpret_cast<daNpc_Nz_c*>(j3dSys.getModel()->getUserArea())->TailNodeCallBack(node, calcTiming);
}

/* 022A29D0 */
BOOL daNpc_Nz_c::TailNodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = (J3DJoint*)node;
        s32 jntNo = joint->getJntNo();
        s32 idx = 0;
        J3DModel* model = j3dSys.getModel();

        if (jntNo == 9) {
            idx = 1;
        }

        static cXyz offset(cXyz::Zero);

        mDoMtx_stack_c::copy(model->getAnmMtx(jntNo));
        mDoMtx_stack_c::multVec(&offset, &mTailRootPos[idx]);
    }

    return true;
}

/* 022A41A8 */
void daNpc_Nz_c::TailControl() {
    int i;
    s16 temp9;
    int temp7;

    cXyz sp64;
    cXyz sp58;
    cXyz sp4C;
    cXyz sp40;
    sp58.set(0.0f, 0.0f, 5.0f);
    mTailPos[0] = mTailRootPos[0];
    sp64 = mTailRootPos[1] - mTailRootPos[0];
    s32 angle = cM_atan2s(sp64.x, sp64.z);
    f32 dist = sp64.absXZ();
    s16 angle2 = -cM_atan2s(sp64.y, dist);
    mDoMtx_stack_c::YrotS(angle);
    mDoMtx_stack_c::XrotM(angle2);
    mDoMtx_stack_c::multVec(&sp58, &sp40);

    cXyz* r19 = &mTailPos[1];
    cXyz* r18 = &mTailVel[1];
    cXyz* r17 = mTailLine.getPos(0);
    dBgS_GndChk gndChk;
    for (i = 1; i < 10; i++, r19++, r18++) {
        f32 temp3 = fnmsubs(i - 1, 0.1f, 1.0f);
        cXyz sp34;
        sp34.x = fmadds(sp40.x, temp3, r18[0].x);
        sp34.y = fmadds(sp40.y, temp3, r18[0].y);
        sp34.z = fmadds(sp40.z, temp3, r18[0].z);
        f32 temp5 = r19[0].y + sp34.y - 2.0f;
        if (temp5 < home.pos.y + 5.0f) {
            temp5 = home.pos.y + 5.0f;
        }
        sp64.y = temp5 - r19[-1].y;
        sp64.x = sp34.x + (r19[0].x - r19[-1].x);
        sp64.z = sp34.z + (r19[0].z - r19[-1].z);
        temp7 = cM_atan2s(sp64.x, sp64.z);
        dist = sp64.absXZ();
        temp9 = -cM_atan2s(sp64.y, dist);
        sp58.set(0.0f, 0.0f, 20.0f);
        mDoMtx_stack_c::YrotS(temp7);
        mDoMtx_stack_c::XrotM(temp9);
        mDoMtx_stack_c::multVec(&sp58, &sp4C);
        r18[0] = r19[0];
        r19[0].x = r19[-1].x + sp4C.x;
        r19[0].y = r19[-1].y + sp4C.y;
        r19[0].z = r19[-1].z + sp4C.z;
        r18[0].x = (r19[0].x - r18[0].x) * 0.8f;
        r18[0].y = (r19[0].y - r18[0].y) * 0.8f;
        r18[0].z = (r19[0].z - r18[0].z) * 0.8f;
    }

    cXyz* r3 = mTailPos;
    for (int i = 0; i < 10; i++, r3++, r17++) {
        *r17 = r3[0];
    }
}

static daNpc_Nz_HIO_c l_HIO;

/* 022A2D68 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Nz_c*>(i_this)->_createHeap();
}

/* 022A2AD8 */
BOOL daNpc_Nz_c::_createHeap() {
    J3DModelData* modelData = static_cast<J3DModelData*>(dComIfG_getObjectRes(m_bdl_arc_name, 3 /* NZ.bdl */));
    JUT_ASSERT(211, modelData != NULL);

    // HD: `new` is the constructor called with this == NULL
    mpMorf = mDoExt_McaMorf::create(NULL,
        modelData,
        NULL, NULL,
        NULL,
        -1, 1.0f, 0, -1, 1,
        NULL,
        0x00080000,
        0x37441422
    );

    if (mpMorf == NULL || mpMorf->getModel() == NULL) {
        return false;
    }

    mpMorf->getModel()->setUserArea((uintptr_t)this);

    m_jnt.setHeadJntNum(modelData->getJointName()->getIndex("head"));
    JUT_ASSERT(222, m_jnt.getHeadJntNum() >= 0);

    m_jnt.setBackboneJntNum(modelData->getJointName()->getIndex("mune"));
    JUT_ASSERT(225, m_jnt.getBackboneJntNum() >= 0);

    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == m_jnt.getHeadJntNum() || i == m_jnt.getBackboneJntNum()) {
            modelData->getJointNodePointer(i)->setCallBack(daNpcNz_NodeCallBack);
        }
        if (i == 1 /* hip */ || i == 9 /* shippo */) {
            modelData->getJointNodePointer(i)->setCallBack(daNpcNz_TailNodeCallBack);
        }
    }

    ResTIMG* img = static_cast<ResTIMG*>(dComIfG_getObjectRes(m_arc_name, 0x2E /* sippo.bti */));
    if (mTailLine.init(1, 10, img, FALSE)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* 022A5DEC */
daNpc_Nz_HIO_c::daNpc_Nz_HIO_c() {
    __vtbl = __vt; // HD: the vtable pointer is the last member
    mNo = -1;
    mMaxHeadX = 0x1F40;
    mMaxHeadY = 0x1F40;
    mMaxBackboneX = 0;
    mMaxBackboneY = 0x1F40;
    mMinHeadX = -0xBB8;
    mMinHeadY = -0x1F40;
    mMinBackboneX = 0;
    mMinBackboneY = -0x1F40;
    mMaxTurnStep = 0x7D0;
    mMaxFollowRotVel = 0x640;
    field_0x1A = 0;
    field_0x1C = 100.0f;
    mBaitOffset = 250.0f;
    mBaitRange = 250.0f;
}

/* 022A2D6C */
static s16 daNpc_Nz_XyCheckCB(void* i_actor, int i_itemBtn) {
    daNpc_Nz_c* i_this = static_cast<daNpc_Nz_c*>(i_actor);

    if (dComIfGp_getSelectItem(i_itemBtn) == dItemNo_BIRD_BAIT_5_e) {
        f32 temp2 = l_HIO.mBaitOffset;
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        cXyz temp(player->current.pos);
        temp.x = fmadds(temp2, cM_ssin(player->current.angle.y), temp.x);
        temp.z = fmadds(temp2, cM_scos(player->current.angle.y), temp.z);
        cXyz temp3 = i_this->mShopPos - temp;
        if (temp3.absXZ() < l_HIO.mBaitRange) {
            return true;
        }

        return false;
    }

    return false;
}

/* 022A2EE8 */
static s16 daNpc_Nz_XyEventCB(void* i_this, int param_1) {
    return static_cast<daNpc_Nz_c*>(i_this)->XyEventCB(param_1);
}

/* 022A2EA4 */
s16 daNpc_Nz_c::XyEventCB(int) {
    mEventIdx = dComIfGp_evmng_getEventIdx("DEFAULT_NPC_NZ_ESA");
    return mEventIdx;
}

/* ---------------- d_a_npc_nz_cut.inc (a separate .text section on GameCube) ---------------- */

/* 022A3094 */
static void* searchEsa_CB(void* param_1, void* i_this) {
    return static_cast<daNpc_Nz_c*>(i_this)->_searchEsa((fopAc_ac_c*)param_1);
}

/* 022A2EEC */
void* daNpc_Nz_c::_searchEsa(fopAc_ac_c* pActor) {
    if (fopAcM_GetName(pActor) == fpcNm_ESA_e) {
        esa_class* pBait = static_cast<esa_class*>(pActor);
        if (pBait->field_0x298 == 0) {
            cXyz delta = pBait->current.pos - current.pos;
            if (mpMorf->getFrame() < 6.0f) {
                cLib_addCalcPos2(&pBait->current.pos, current.pos, 0.1f, 2.5f);
                mDoMtx_stack_c::transS(pBait->current.pos);
                mDoMtx_stack_c::ZXYrotM(pBait->current.angle);
                pBait->mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
            }

            if (delta.absXZ() < mBaitDist) {
                mpBait = pBait;
                mBaitDist = delta.absXZ();
            }
        }
    }

    return NULL;
}

/* 022A30A4 */
static void* searchNzAndBomb_CB(void* param_1, void*) {
    fopAc_ac_c* pActor = static_cast<fopAc_ac_c*>(param_1);

    if (fopAcM_GetName(pActor) == fpcNm_NZ_e || fopAcM_GetName(pActor) == fpcNm_BOMB_e) {
        fopAcM_delete(pActor);
    }

    return NULL;
}

/* 022A30F0 */
static void* searchTsuboAndBarrel_CB(void* param_1, void* param_2) {
    fopAc_ac_c* pActor1 = static_cast<fopAc_ac_c*>(param_1);
    daNpc_Nz_c* i_this = static_cast<daNpc_Nz_c*>(param_2);

    if (fopAcM_GetName(param_1) == fpcNm_TSUBO_e || fopAcM_GetName(param_1) == fpcNm_Obj_Barrel_e) {
        cXyz delta = i_this->mShopPos - pActor1->current.pos;

        if (delta.absXZ() < 300.0f) {
            bool success = false;
            if (fopAcM_GetName(pActor1) == fpcNm_TSUBO_e) {
                success = static_cast<daTsubo::Act_c*>(pActor1)->pos_init();
            } else if (fopAcM_GetName(pActor1) == fpcNm_TSUBO_e) {
                success = static_cast<daObjBarrel::Act_c*>(pActor1)->pos_init();
            }

            if (!success) {
                fopAcM_delete(pActor1);
            }
        }
    }

    return NULL;
}

/* 022A32B4 */
static void* searchEsaInit_CB(void* pActor1, void* pActor2) {
    if (fopAcM_GetName(pActor1) == fpcNm_ESA_e) {
        esa_class* pBait = static_cast<esa_class*>(pActor1);
        daNpc_Nz_c* pNz = static_cast<daNpc_Nz_c*>(pActor2);

        cXyz pos(pNz->mShopPos);
        pos.x = fmadds(150.0f, cM_ssin(pNz->current.angle.y), pos.x);
        pos.z = fmadds(150.0f, cM_scos(pNz->current.angle.y), pos.z);
        f32 rnd = cM_rndFX(358.0f);
        s16 angle = cM_deg2s(rnd);
        f32 mag = cM_rndF(30.0f);
        pos.x = fmadds(mag, cM_ssin(angle), pos.x);
        pos.z = fmadds(mag, cM_scos(angle), pos.z);

        pBait->current.pos = pos;
        pBait->old.pos = pos;

        mDoMtx_stack_c::transS(pBait->current.pos);
        mDoMtx_stack_c::ZXYrotM(pBait->current.angle);
        pBait->mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    }

    return NULL;
}

/* 022A4998 */
void daNpc_Nz_c::cutProc() {
    mStaffId = dComIfGp_evmng_getMyStaffId(mEventCut.getActorName());
    if (mStaffId == -1) {
        return;
    }

    static char* action_table[] = {
        "EATESA",
        "EATESA1ST",
        "KILLALL",
        "SHOW",
        "HIDE",
        "SETANM",
        "GOHOME",
        "CHKGETITEM",
    };

    static cut_entry_t cut_tbl[] = {
        {&daNpc_Nz_c::cutEatesaStart, &daNpc_Nz_c::cutEatesaProc},
        {&daNpc_Nz_c::cutEatesaFirstStart, &daNpc_Nz_c::cutEatesaFirstProc},
        {&daNpc_Nz_c::cutKillAllStart, &daNpc_Nz_c::cutKillAllProc},
        {&daNpc_Nz_c::cutShowStart, &daNpc_Nz_c::cutShowProc},
        {&daNpc_Nz_c::cutHideStart, &daNpc_Nz_c::cutHideProc},
        {&daNpc_Nz_c::cutSetAnmStart, &daNpc_Nz_c::cutSetAnmProc},
        {&daNpc_Nz_c::cutGoHomeStart, &daNpc_Nz_c::cutGoHomeProc},
        {&daNpc_Nz_c::cutChkGetItemStart, &daNpc_Nz_c::cutChkGetItemProc},
    };

    int actIdx = dComIfGp_evmng_getMyActIdx(mStaffId, action_table, ARRAY_SIZE(action_table), TRUE, 0);
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(mStaffId);
        return;
    }

    if (dComIfGp_evmng_getIsAddvance(mStaffId)) {
        (this->*cut_tbl[actIdx].start)();
    }

    (this->*cut_tbl[actIdx].proc)();
}

/* 022A5424 */
void daNpc_Nz_c::cutEatesaStart() {
    setAnm(6, false);
}

/* 022A5430 */
void daNpc_Nz_c::cutEatesaProc() {
    fopAcM_Search(&searchEsa_CB, this);
    fopAc_ac_c* pBait = mpBait;

    if (mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
        current.angle.y = shape_angle.y = home.angle.y + (s16)(ftoi(cM_rndFX(4.0f)) * 2048);
    }

    if (pBait) {
        fopAcM_delete(pBait);
        mpBait = NULL;
        mBaitDist = FLOAT_MAX;
    } else {
        if (mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
            current.angle.y = shape_angle.y = home.angle.y;
            dComIfGp_evmng_cutEnd(mStaffId);
        }
    }
}

/* 022A55AC */
void daNpc_Nz_c::cutEatesaFirstStart() {
    fopAcM_Search(&searchEsa_CB, this);
    current.angle.y = shape_angle.y = home.angle.y;
    setAnm(0xA, false);
}

/* 022A5600 */
void daNpc_Nz_c::cutEatesaFirstProc() {
    cXyz target(mShopPos);
    target.x = fmadds(100.0f, cM_ssin(home.angle.y), target.x);
    target.z = fmadds(100.0f, cM_scos(home.angle.y), target.z);

    if (mAnmPrmIdx == 10) {
        f32 temp2 = cLib_addCalcPosXZ(&current.pos, target, 0.25f, 10.0f, 2.0f);
        s16 temp3 = cLib_targetAngleY(&current.pos, &target);
        cLib_addCalcAngleS2(&shape_angle.y, temp3, 8, 0x1000);

        if (cLib_IsZero(temp2) && mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
            setAnm(6, false);
            mLoopCount = 2;
        } else {
            return;
        }
    }

    fopAcM_Search(&searchEsa_CB, this);
    fopAc_ac_c* pBait = mpBait;
    if (pBait) {
        if (mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f) && cLib_calcTimer(&mLoopCount) == 0) {
            fopAcM_delete(pBait);
            mpBait = NULL;
            dComIfGp_evmng_cutEnd(mStaffId);
        }
    } else {
        dComIfGp_evmng_cutEnd(mStaffId);
    }
}

/* 022A5818 */
void daNpc_Nz_c::cutKillAllStart() {
    fopAcM_Search(&searchNzAndBomb_CB, this);
    fopAcM_Search(&searchTsuboAndBarrel_CB, this);
    fopAcM_Search(&searchEsaInit_CB, this);

    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    cXyz pos = mShopPos;
    s16 r5 = home.angle.y + 0x8000;
    pos.x = fmadds(250.0f, cM_ssin(home.angle.y), pos.x);
    pos.z = fmadds(250.0f, cM_scos(home.angle.y), pos.z);

    ((daPy_py_c*)player)->setPlayerPosAndAngle(&pos, r5); // HD: one dComIfGp_getPlayer(0)
}

/* 022A58E0 */
void daNpc_Nz_c::cutKillAllProc() {
    dComIfGp_evmng_cutEnd(mStaffId);
}

/* 022A5918 */
void daNpc_Nz_c::cutShowStart() {
    mIsShow = true;
}

/* 022A5924 */
void daNpc_Nz_c::cutShowProc() {
    dComIfGp_evmng_cutEnd(mStaffId);
}

/* 022A595C */
void daNpc_Nz_c::cutHideStart() {
    mIsShow = false;
}

/* 022A5968 */
void daNpc_Nz_c::cutHideProc() {
    dComIfGp_evmng_cutEnd(mStaffId);
}

/* 022A59A0 */
void daNpc_Nz_c::cutSetAnmStart() {
    be<s32>* pAnmNo = dComIfGp_evmng_getMyIntegerP(mStaffId, "anm_no");
    s32 anmNo = 0;
    be<s32>* pLoopCount = dComIfGp_evmng_getMyIntegerP(mStaffId, "loop_count");

    if (pAnmNo) {
        anmNo = *pAnmNo;
    }

    if (pLoopCount) {
        mLoopCount = *pLoopCount;
    } else {
        mLoopCount = 0;
    }

    setAnm(anmNo, false);
    if (anmNo == 8) {
        fopAcM_seStart(this, JA_SE_CV_M_NZ_TALK_IN, 0);
    }
}

/* 022A5A80 */
void daNpc_Nz_c::cutSetAnmProc() {
    if (mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
        mLoopCount--;
    }

    if (mLoopCount <= 0) {
        dComIfGp_evmng_cutEnd(mStaffId);
    }

    if (mAnmPrmIdx == 8) {
        if (mpMorf->checkFrame(8.0f)) {
            setSmokeParticle();
        } else if (mpMorf->checkFrame(13.0f)) {
            J3DModel* pModel = mpMorf->getModel();

            cXyz scale(0.65f, 0.65f, 0.65f);
            cXyz temp4(10.0f, 0.0f, 10.0f);
            cXyz pos;
            GXColor color;
            color.r = 0x80;
            color.g = 0x80;
            color.b = 0x80;

            mDoMtx_stack_c::copy(pModel->getAnmMtx(m_jnt.getHeadJntNum()));
            mDoMtx_stack_c::multVec(&temp4, &pos);
            dComIfGp_particle_set(dPa_name::ID_IT_JN_LK_GEPPU00, &pos, &shape_angle, &scale, 0xFF, 0, -1, &color);
        }
    }
}

/* 022A5F58 */
void daNpc_Nz_c::cutGoHomeStart() {
    return;
}

/* 022A5C54 */
void daNpc_Nz_c::cutGoHomeProc() {
    s16 target = cLib_targetAngleY(&current.pos, &home.pos);
    cLib_addCalcAngleS2(&current.angle.y, target, 4, 0x1000);
    shape_angle.y = current.angle.y;
    s16 dist = cLib_distanceAngleS(target, current.angle.y);
    if (dist < 0x4000) {
        if (mAnmPrmIdx != 0xA) {
            fopAcM_seStart(this, JA_SE_CV_M_NZ_TALK_OUT, 0);
        }

        setAnm(0xA, false);
        if (cLib_addCalcPosXZ(&current.pos, home.pos, 0.5f, 10.0f, 1.0f) < 1.0f) {
            dComIfGp_evmng_cutEnd(mStaffId);
        }
    }
}

/* 022A5D60 */
void daNpc_Nz_c::cutChkGetItemStart() {
    if (mTalkMode >= 3) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, mEventIdx, 0 /* HD: GameCube dEvtFlag_NOPARTNER_e */, 0xFFFF);
    }
}

/* 022A5DB4 */
void daNpc_Nz_c::cutChkGetItemProc() {
    dComIfGp_evmng_cutEnd(mStaffId);
}

/* ---------------- end of d_a_npc_nz_cut.inc ---------------- */

/* 022A415C */
void daNpc_Nz_c::eventOrder() {
    if (mTalkMode == 1 || mTalkMode == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        eventInfo.onCondition(dEvtCnd_CANTALKITEM_e);
        if (mTalkMode == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 022A400C */
void daNpc_Nz_c::checkOrder() {
    if (eventInfo.checkCommandTalk() && (mTalkMode == 1 || mTalkMode == 2)) {
        if (mTalkMode == 2) {
            modeProcInit(1);
        }

        mTalkMode = 0;
    }
}

/* 022A418C */
void daNpc_Nz_c::setAttention() {
    attention_info.position = current.pos;
    attention_info.position.y = eyePos.y;
}

/* 022A4070 */
void daNpc_Nz_c::LookBack() {
    cXyz* dstTemp;
    s16 targetY = current.angle.y;
    cXyz dstPos = dNpc_playerEyePos(0.0f);
    dstTemp = &dstPos;

    s16 maxFollowRotVel;
    if (m_jnt.trnChk()) {
        maxFollowRotVel = l_HIO.mMaxFollowRotVel;
    } else {
        maxFollowRotVel = 0;
    }

    m_jnt.setParam(l_HIO.mMaxBackboneX, l_HIO.mMaxBackboneY, l_HIO.mMinBackboneX, l_HIO.mMinBackboneY, l_HIO.mMaxHeadX,
                   l_HIO.mMaxHeadY, l_HIO.mMinHeadX, l_HIO.mMinHeadY, l_HIO.mMaxTurnStep);
    m_jnt.lookAtTarget(&current.angle.y, dstTemp, eyePos, targetY, maxFollowRotVel, true);
}

/* 022A34AC */
void daNpc_Nz_c::setAnm(s8 param_1, u8 param_2) {
    static const dLib_anm_idx_c a_anm_idx_tbl[] = {
        {0x23, 0x12},
        {0x15, 0x07},
        {0x16, -1},
        {0x17, 0x08},
        {0x18, 0x09},
        {0x19, 0x0A},
        {0x1A, 0x0B},
        {0x1B, 0x0C},
        {0x1C, 0x0D},
        {0x1D, 0x0E},
        {0x1F, 0x10},
        {0x20, 0x11},
        {0x1E, 0x11},
        {0x21, -1},
        {0x22, -1},
    };
    static const dLib_anm_prm_c a_anm_prm_tbl[] = {
        {0x00, -1, 0x00, 8.0f, 1.0f, 2},
        {0x01, -1, 0x00, 8.0f, 1.0f, 0},
        {0x02, -1, 0x00, 8.0f, 1.0f, 0},
        {0x03, -1, 0x00, 8.0f, 1.0f, 0},
        {0x04, -1, 0x00, 8.0f, 1.0f, 0},
        {0x05, -1, 0x00, 8.0f, 1.0f, 0},
        {0x06, -1, 0x00, 8.0f, 1.0f, 2},
        {0x07, -1, 0x00, 8.0f, 1.0f, 2},
        {0x08, -1, 0x00, 8.0f, 1.0f, 0},
        {0x09, -1, 0x00, 8.0f, 1.0f, 0},
        {0x0A, -1, 0x00, 8.0f, 1.0f, 2},
        {0x0B, -1, 0x00, 8.0f, 1.0f, 2},
        {0x0C, -1, 0x00, 8.0f, 1.0f, 0},
        {0x0D, -1, 0x00, 8.0f, 1.0f, 0},
        {0x0E, -1, 0x00, 8.0f, 1.0f, 0},
    };

    if (param_1 != 0xF) {
        mAnmPrmIdx = param_1;
    }

    if (mAnmPrmIdx == 0) {
        m_jnt.offHeadLock();
        m_jnt.offBackBoneLock();
    } else {
        m_jnt.onHeadLock();
        m_jnt.onBackBoneLock();
    }

    dLib_setAnm(m_arc_name, mpMorf, &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx, a_anm_idx_tbl, a_anm_prm_tbl, param_2);
}

/* 022A369C */
void daNpc_Nz_c::setMtx() {
    mpMorf->getModel()->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpMorf->getModel()->setBaseTRMtx(mDoMtx_stack_c::get());

    if (mAnmPrmIdx != 0xC && mAnmPrmIdx != 0xB) {
        return;
    }

    daShopItem_c* item = getShopItem(mSelectItemIdx);
    if (item) {
        cXyz* pScale = item->getScaleP();
        mItemScaleTimer = cLib_maxLimit<int>(mItemScaleTimer + 1, 20);
        s16 r7 = 12;
        s16 r8 = 16;
        s16 r6 = 18;

        f32 scaleX;
        f32 scaleY;
        f32 multX1 = 0.85f;
        f32 multY1 = 1.4f;
        f32 multX2 = 1.1f;
        f32 multY2 = 0.8f;
        f32 multX3 = 1.0f;
        f32 multY3 = 1.0f;
        f32 temp4 = 0.0f;
        if (mItemScaleTimer < r7) {
            scaleX = mItemScaleTimer / (f32)r7;
            scaleY = mItemScaleTimer / (f32)r7;
            scaleX *= multX1;
            scaleY *= multY1;
        } else if (mItemScaleTimer < r8) {
            scaleX = (mItemScaleTimer - r7) / (f32)(r8 - r7);
            scaleY = (mItemScaleTimer - r7) / (f32)(r8 - r7);
            scaleX = fmadds(scaleX, multX2, (1.0f - scaleX) * multX1);
            scaleY = fmadds(scaleY, multY2, (1.0f - scaleY) * multY1);
        } else {
            scaleX = (mItemScaleTimer - r8) / (f32)(r6 - r8);
            scaleY = (mItemScaleTimer - r8) / (f32)(r6 - r8);
            scaleX = fmadds(1.0f - scaleX, multX2, scaleX * multX3);
            scaleY = fmadds(1.0f - scaleY, multY2, scaleY * multY3);
        }

        pScale->z = scaleX;
        pScale->x = scaleX;
        pScale->y = scaleY;

        J3DModel* pModel = mpMorf->getModel();
        mDoMtx_stack_c::copy(pModel->getAnmMtx(18 /* udel3 */));
        cXyz temp;
        mDoMtx_stack_c::multVec(&cXyz::Zero, &temp);
        mDoMtx_stack_c::copy(pModel->getAnmMtx(21 /* uder3 */));
        cXyz temp2;
        mDoMtx_stack_c::multVec(&cXyz::Zero, &temp2);
        cXyz temp3 = temp + temp2;

        switch (mSelectItemNo) {
        case dItemNo_BIRD_BAIT_5_e:
            temp4 = -15.0f;
            break;
        case dItemNo_HYOI_PEAR_e:
            temp4 = -5.0f;
            break;
        case dItemNo_RED_POTION_e:
        case dItemNo_BLUE_POTION_e:
            temp4 = -5.0f;
            break;
        case dItemNo_BOMB_10_e:
        case dItemNo_BOMB_30_e:
            temp4 = -15.0f;
            break;
        case dItemNo_ARROW_10_e:
        case dItemNo_ARROW_30_e:
            temp4 = -10.0f;
            break;
        }

        temp3 *= 0.5f;
        temp3.y += fmadds(scaleY, 20.0f, -20.0f);
        temp3.y += temp4;
        item->getPosP()->set(temp3);
    }
}

/* 022A5F50 */
void daNpc_Nz_c::modeWaitInit() {
    return;
}

/* 022A498C */
void daNpc_Nz_c::modeWait() {
    mTalkMode = 2;
}

/* 022A5F54 */
void daNpc_Nz_c::modeEventEsaInit() {
    return;
}

/* 022A4B84 */
void daNpc_Nz_c::modeEventEsa() {
    if (!mEventCut.cutProc()) {
        cutProc();
    }

    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        dComIfGp_event_reset();

        mEventIdx = -1;
        deleteShopItem();
        mIsShow = false;

        old.pos = home.pos;
        current.pos = old.pos;
        shape_angle = home.angle;
        current.angle = shape_angle;

        modeProcInit(0);
    }
}

/* 022A3574 */
void daNpc_Nz_c::modeProc(daNpc_Nz_c::Proc_e proc, int newMode) {
    static mode_entry_t mode_tbl[] = {
        {
            &daNpc_Nz_c::modeWaitInit,
            &daNpc_Nz_c::modeWait,
            "WAIT",
        },
        {
            &daNpc_Nz_c::modeEventEsaInit,
            &daNpc_Nz_c::modeEventEsa,
            "WAIT",
        },
    };

    if (proc == PROC_INIT_e) {
        mCurMode = newMode;
        (this->*mode_tbl[newMode].init)();
    } else if (proc == PROC_EXEC_e) {
        (this->*mode_tbl[mCurMode].run)();
    }
}

/* 022A3474 */
static cPhs_State daNpc_Nz_ShopItemCreateCB(void* i_item) {
    daShopItem_c* i_this = static_cast<daShopItem_c*>(i_item);
    i_this->hide();
    i_this->setTevType(TEV_TYPE_ACTOR);

    return cPhs_COMPLEATE_e;
}

/* 022A4C64 */
void daNpc_Nz_c::createShopItem(u8 itemNo1, u8 itemNo2) {
    if (mShopItemId[0] == fpcM_ERROR_PROCESS_ID_e) {
        mShopItemId[0] = fopAcM_createShopItem(&current.pos, itemNo1, &current.angle, fopAcM_GetRoomNo(this), 0, daNpc_Nz_ShopItemCreateCB);
    }
    if (mShopItemId[1] == fpcM_ERROR_PROCESS_ID_e) {
        mShopItemId[1] = fopAcM_createShopItem(&current.pos, itemNo2, &current.angle, fopAcM_GetRoomNo(this), 0, daNpc_Nz_ShopItemCreateCB);
    }
}

/* 022A3620 */
daShopItem_c* daNpc_Nz_c::getShopItem(int param_1) {
    if (param_1 == 0) {
        return (daShopItem_c*)fopAcM_SearchByID(mShopItemId[0]);
    } else {
        return (daShopItem_c*)fopAcM_SearchByID(mShopItemId[1]);
    }
}

/* 022A4B04 */
void daNpc_Nz_c::deleteShopItem() {
    if (mShopItemId[0] != fpcM_ERROR_PROCESS_ID_e) {
        daShopItem_c* item = getShopItem(0);
        if (item) {
            fopAcM_delete(item);
            mShopItemId[0] = fpcM_ERROR_PROCESS_ID_e;
        }
    }
    if (mShopItemId[1] != fpcM_ERROR_PROCESS_ID_e) {
        daShopItem_c* item = getShopItem(1);
        if (item) {
            fopAcM_delete(item);
            mShopItemId[1] = fpcM_ERROR_PROCESS_ID_e;
        }
    }
}

/* inlined into next_msgStatus */
static u32 daNpcNz_getShopBoughtMsg(u8 itemNo) {
    int messageRupee = dComIfGp_getMessageRupee();
    if (dComIfGs_getRupee() < messageRupee) {
        return 0x3405;
    }

    if ((itemNo == dItemNo_RED_POTION_e || itemNo == dItemNo_BLUE_POTION_e) && !dComIfGs_checkEmptyBottle()) {
        return 0x3406;
    }

    if (isBomb(itemNo) && dComIfGs_getBombNum() == dComIfGs_getBombMax()) {
        return 0x3407;
    }

    if (isArrow(itemNo) && dComIfGs_getArrowNum() == dComIfGs_getArrowMax()) {
        return 0x3407;
    }

    if ((itemNo == dItemNo_BIRD_BAIT_5_e || itemNo == dItemNo_HYOI_PEAR_e) && !dComIfGs_checkBaitItemEmpty()) {
        return 0x3407;
    }

    int itemTemp = itemNo; // probably fake
    if (checkItemGet(itemTemp, FALSE)) {
        dComIfGp_setItemRupeeCount(-messageRupee);
        execItemGet(itemTemp);
        return 0x3403;
    }

    return 0;
}

/* 022A4CFC */
u16 daNpc_Nz_c::next_msgStatus(be<u32>* pMsgNo) {
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e;

    static const u32 shop_next_msg_tbl[4][2] = {
        {0x33FB, 0x33FC},
        {0x33FD, 0x33FE},
        {0x33FF, 0x3400},
        {0x3401, 0x3402},
    };

    const u8 itemArr1[4][2] = {
        {dItemNo_BIRD_BAIT_5_e, dItemNo_HYOI_PEAR_e},
        {dItemNo_RED_POTION_e, dItemNo_BLUE_POTION_e},
        {dItemNo_BOMB_10_e, dItemNo_BOMB_30_e},
        {dItemNo_ARROW_10_e, dItemNo_ARROW_30_e},
    };

    int temp = 1;
    switch (*pMsgNo) {
    case 0x33F5:
        *pMsgNo = 0x33F6;
        break;
    case 0x33F6:
        if (dComIfGs_checkGetBottle()) {
            temp += 1;
        }
        if (dComIfGs_checkGetItem(dItemNo_BOMB_BAG_e)) {
            temp += 1;
        }
        if (dComIfGs_getItem(dInvSlot_BOW_e) != dItemNo_NONE_e) {
            temp += 1;
        }

        mItemListIdx = ftoi(cM_rndF(temp));
        *pMsgNo = mItemListIdx + 0x33F7;
        createShopItem(itemArr1[mItemListIdx][0], itemArr1[mItemListIdx][1]);
        mItemListMsgNo = *pMsgNo;
        break;
    case 0x33F7:
    case 0x33F8:
    case 0x33F9:
    case 0x33FA:
        switch (mpCurrMsg->mSelectNum) {
        case 0:
        case 1:
            mSelectItemNo = itemArr1[mItemListIdx][mpCurrMsg->mSelectNum];
            *pMsgNo = shop_next_msg_tbl[mItemListIdx][mpCurrMsg->mSelectNum];
            mSelectItemIdx = mpCurrMsg->mSelectNum;
            break;
        case 2:
            *pMsgNo = 0x3404;
            break;
        }

        break;
    case 0x33FB:
    case 0x33FC:
    case 0x33FD:
    case 0x33FE:
    case 0x33FF:
    case 0x3400:
    case 0x3401:
    case 0x3402:
        switch (mpCurrMsg->mSelectNum) {
        case 0: {
            u32 msgNo = daNpcNz_getShopBoughtMsg(mSelectItemNo);
            if (msgNo != 0) {
                *pMsgNo = msgNo;
                break;
            }

            daShopItem_c* item = getShopItem(mSelectItemIdx);
            if (item) {
                item->hide();
            }

            setAnm(0, false);
            int msg_rupee = dComIfGp_getMessageRupee();
            dComIfGp_setItemRupeeCount(-msg_rupee);
            mTalkMode = 3;
            mEventIdx = dComIfGp_evmng_getEventIdx("DEFAULT_NPC_NZ_GETITEM");
            dComIfGp_event_setGtItm(mSelectItemNo);
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        }
        case 1:
            *pMsgNo = mItemListMsgNo;
            break;
        default:
            break;
        }

        break;
    case 0x3404:
    case 0x3405:
    case 0x3406:
    case 0x3407:
        *pMsgNo = 0x3403;
        break;
    default:
        msgStatus = fopMsgStts_MSG_ENDS_e;
    }

    return msgStatus;
}

/* 022A513C */
void daNpc_Nz_c::anmAtr(u16 i_msgStatus) {
    if (mAnmPrmIdx == 9 && mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
        setAnm(0, false);
    } else if (mAnmPrmIdx == 0xC && mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
        setAnm(0xB, false);
    }

    msg_class* pCurrMsg = mpCurrMsg; // HD: read before the button check
    if (dComIfGp_checkMesgSendButton()) {
        if (pCurrMsg->mStatus == fopMsgStts_BOX_CLOSING_e ||
            pCurrMsg->mStatus == fopMsgStts_BOX_CLOSED_e ||
            pCurrMsg->mStatus == fopMsgStts_MSG_ENDS_e
        ) {
            return;
        }

        daShopItem_c* item;
        switch (pCurrMsg->mMsgNo) {
        case 0x33F5:
        case 0x33F6:
            break;
        case 0x33F7:
        case 0x33F8:
        case 0x33F9:
        case 0x33FA:
            item = getShopItem(mSelectItemIdx);
            if (item) {
                item->hide();
            }

            setAnm(0, false);
            break;
        case 0x33FB:
        case 0x33FC:
        case 0x33FD:
        case 0x33FE:
        case 0x33FF:
        case 0x3400:
        case 0x3401:
        case 0x3402:
            item = getShopItem(mSelectItemIdx);
            if (item) {
                item->show();
            }

            mItemScaleTimer = 0;
            fopAcM_seStart(this, JA_SE_CV_M_NZ_TALK_IN, 0);
            setAnm(0xC, false);
            break;
        case 0x3405:
        case 0x3406:
        case 0x3407:
            item = getShopItem(mSelectItemIdx);
            if (item) {
                item->hide();
            }

            setAnm(0, false);
            break;
        case 0x3404:
            item = getShopItem(mSelectItemIdx);
            if (item) {
                item->hide();
            }

            setAnm(0, false);
            break;
        case 0x3403:
            item = getShopItem(mSelectItemIdx);
            if (item) {
                item->hide();
            }

            setAnm(0, false);
            break;
        }
    }
}

/* 022A45E4 */
bool daNpc_Nz_c::_execute() {
    cLib_addCalc2(&speedF, mSpeedTarget, 0.3f, 4.0f);
    checkOrder();
    modeProc(PROC_EXEC_e, 2);
    LookBack();
    eventOrder();
    setAttention();
    mpMorf->play(NULL, 0, 0);
    mpMorf->calc();
    TailControl();
    mAcch.CrrPos(*dComIfG_Bgsp());
    mpBait = NULL;
    mBaitDist = FLOAT_MAX;
    setMtx();

    return false;
}

/* 022A46B8 */
bool daNpc_Nz_c::_draw() {
    if (!mIsShow) {
        return true;
    } else {
        J3DModel* pModel = mpMorf->getModel();
        J3DModelData* pModelData = pModel->getModelData();

        J3DJoint* rootJoint = pModelData->getJointNodePointer(0 /* kosi */);
        // HD: the materials are looked up by name (GameCube: indices 0, 1, 2)
        J3DShape* matShape = pModelData->getMaterialNodePointer("SC_eye_MX")->getShape();
        J3DShape* matShape2 = pModelData->getMaterialNodePointer("lambert2")->getShape();
        J3DShape* matShape3 = pModelData->getMaterialNodePointer("lambert7")->getShape();
        matShape->hide();

        g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
        g_env_light.setLightTevColorType(pModel, &tevStr);

        mpMorf->entryDL();

        dComIfGd_setListMaskOff();

        matShape->show();
        // HD: no pModel->getMatPacket(0)->unlock()
        matShape2->hide();
        matShape3->hide();
        rootJoint->entryIn(pModel);
        matShape2->show();
        matShape3->show();

        dComIfGd_setList();

        static const GXColor tail_color = {200, 200, 200, 255};
        mTailLine.update(10, scale.x * 5.0f, tail_color, 6, &tevStr);
        dComIfGd_set3DlineMat(&mTailLine);
    }

    return true;
}

/* 022A3C88 */
BOOL daNpc_Nz_c::createInit() {
    setAnm(0, false);
    modeProcInit(0);

    mAcchCir.SetWall(30.0f, 10.0f);
    mAcch.Set(fopAcM_GetPosition_p(this), fopAcM_GetOldPosition_p(this), this, 1, &mAcchCir, fopAcM_GetSpeed_p(this));
    mAcch.OnLineCheckNone();
    mAcch.SetWallNone();
    mAcch.SetRoofNone();
    fopAcM_SetGravity(this, 0.0f);
    mAcch.CrrPos(*dComIfG_Bgsp());

    setMtx();
    mpMorf->calc();
    fopAcM_SetMtx(this, mpMorf->getModel()->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -450.0f, -50.0f, -450.0f, 450.0f, 750.0f, 450.0f);
    fopAcM_setCullSizeFar(this, 10.0f);
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xB1;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xB1;
    attention_info.flags = fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_NOTALK_e;
    eventInfo.setXyCheckCB(&daNpc_Nz_XyCheckCB);
    eventInfo.setXyEventCB(&daNpc_Nz_XyEventCB);
    mEventCut.setActorInfo2("NpcNz", this);

    mTalkMode = 0;
    mIsShow = false;
    mLoopCount = 0;
    mShopPos = home.pos;
    home.pos.x = fnmsubs(50.0f, cM_ssin(home.angle.y), home.pos.x);
    home.pos.z = fnmsubs(50.0f, cM_scos(home.angle.y), home.pos.z);
    current.pos = home.pos;
    mShopItemId[0] = -1;
    mShopItemId[1] = -1;

    return true;
}

/* 022A5368 */
void daNpc_Nz_c::setSmokeParticle() {
    if (mSmokeCB.getEmitter() != NULL) {
        mSmokeCB.end();
    }

    if (mSmokeCB.getEmitter() == NULL) {
        JPABaseEmitter* emitter;
        emitter = dComIfGp_particle_setToon(dPa_name::ID_AK_JT_ELEMENTSMOKE00, &current.pos, &current.angle, 0, 0xB9, &mSmokeCB, fopAcM_GetRoomNo(this));
        if (emitter) {
            emitter->setRate(3.0f);
            emitter->setSpread(0.2f);
            emitter->setAwayFromAxisSpeed(0.2f);
            emitter->setMaxFrame(3);
        }
    }
}

/* inlined into _create */
void daNpc_Nz_c::getArg() {
    fopAcM_GetParam(this);
}

/* 022A3E74 */
cPhs_State daNpc_Nz_c::_create() {
    fopAcM_ct(this, daNpc_Nz_c);

    getArg();

    cPhs_State res = dComIfG_resLoad(&mPhs1, m_arc_name);
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    cPhs_State res2 = dComIfG_resLoad(&mPhs2, m_bdl_arc_name);
    if (res2 != cPhs_COMPLEATE_e) {
        return res2;
    }

    if (!fopAcM_entrySolidHeap(this, createHeap_CB, 0x2FE0)) {
        return cPhs_ERROR_e;
    }

    BOOL success = createInit();
    if (!success) {
        return cPhs_ERROR_e;
    }

    return cPhs_COMPLEATE_e;
}

/* inlined into _create (fopAcM_ct) */
daNpc_Nz_c::daNpc_Nz_c() {
    // HD: GHS constructs the base class and the members here (GameCube: implicit)
    __ct(static_cast<fopNpc_npc_c*>(this));
    __vtbl = __vt;
    __ct(&mAcch);
    __ct(&mAcchCir);
    __ct(&mSmokeCB, 1);
    __ct(&mTailLine);
}

/* 022A5F5C */
// HD: the compiler-generated deleting destructor (members in reverse order, then the base)
static void daNpc_Nz_c_dtor(daNpc_Nz_c* i_this, int flags) {
    if (i_this != NULL) {
        __dt(&i_this->mTailLine);
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mAcch);
        __dt(static_cast<fopNpc_npc_c*>(i_this));
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 022A3FA8 */
bool daNpc_Nz_c::_delete() {
    if (mSmokeCB.getEmitter()) {
        mSmokeCB.end();
    }

    dComIfG_resDeleteDemo(&mPhs1, m_arc_name);
    dComIfG_resDeleteDemo(&mPhs2, m_bdl_arc_name);

    return true;
}

/* 022A3FA4 */
static cPhs_State daNpc_NzCreate(void* i_this) {
    return static_cast<daNpc_Nz_c*>(i_this)->_create();
}

/* 022A4008 */
static BOOL daNpc_NzDelete(void* i_this) {
    return static_cast<daNpc_Nz_c*>(i_this)->_delete();
}

/* 022A46B4 */
static BOOL daNpc_NzExecute(void* i_this) {
    return static_cast<daNpc_Nz_c*>(i_this)->_execute();
}

/* 022A4988 */
static BOOL daNpc_NzDraw(void* i_this) {
    return static_cast<daNpc_Nz_c*>(i_this)->_draw();
}

/* 022A5F48 */
static BOOL daNpc_NzIsDelete(void*) {
    return true;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 022A5E94 */
// __sinit_d_a_npc_nz_cpp: the common header statics, then l_HIO
static void __sinit_d_a_npc_nz_cpp() {
    __sinit_header_statics();
    daNpc_Nz_HIO_c__ct(&l_HIO);
}

/* 022A5F34 */
// sead::SafeStringBase<char>::~SafeStringBase (deleting; this TU's copy)
static void SafeString_dtor(SafeString* i_this, int flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 022A6034 */
// sead::SafeStringBase<char>::assureTerminationImpl_ (empty; this TU's copy)
static void SafeString_assureTerminationImpl(SafeString* i_this) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str  1001F034                               # "Nz"
str  1001F02C                               # "Npcnz"
str  1001EC1C                               # "head"
str  1001EC24                               # "mune"
str  1001EC2C                               # "d_a_npc_nz.cpp"
str  1001EC3C "modelData != NULL"
str  1001EC50 "m_jnt.getHeadJntNum() >= 0"
str  1001EC6C "m_jnt.getBackboneJntNum() >= 0"
str  1001EC8C                               # "DEFAULT_NPC_NZ_ESA"
str  1001EEA8                               # "NpcNz"
str  1001EEC4                               # "SC_eye_MX"
str  1001EED0                               # "lambert2"
str  1001EEDC                               # "lambert7"
str  1001EFD0                               # "DEFAULT_NPC_NZ_GETITEM"
str  1001EFFC                               # "anm_no"
str  1001F004                               # "loop_count"
data l_HIO 10467CE8
data offset 10467D20 guard 10467D2C
data action_table 101C2574
data cut_tbl 1001EEE8
data a_anm_idx_tbl 1001ECC0
data a_anm_prm_tbl 1001ED38
data mode_tbl 1001EE28
data shop_next_msg_tbl 1001EFB0
data tail_color 101C2570
ext  021842B8 void daItemBase_c::hide()
ext  021842C8 void daItemBase_c::show()
ext  02483FA4 cXyz* daShopItem_c::getScaleP()
ext  02483FB4 cXyz* daShopItem_c::getPosP()
*/
