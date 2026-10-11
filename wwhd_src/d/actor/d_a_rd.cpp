/**
 * d_a_rd.cpp (WWHD)
 * Enemy - ReDead
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_rd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_rd.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"
#include "gc_api/f_op/f_op_actor_mng.h"

const gc_tu_data gc_tu = { .safeString = 0x10038E8C, .safeStringAssure = 0x0245F360 };

#define SAFESTRING_VTBL 0x10038E8C /* this TU's sead::SafeString vtable */
#define RD_VTBL 0x10038EE4         /* daRd_c vtable (HD virtual destructor) */
static const char m_arc_name[] = "Rd";
static const u32 m_heapsize = 0x2520;

static const dBgS_ObjAcch_vt RD_OBJACCH_VT = {0x10038EB4, 0x10038ED4, 0x10038EC4};
#define RD_AAB_VTBL 0x10038EA4 /* this TU's cM3dGAab vtable */

static BOOL nodeControl_CB(J3DNode*, int);
static BOOL nodeHeadControl_CB(J3DNode*, int);
static daRd_c* daRd_c_ct(daRd_c*);

/* ---- functions ---- */

/* 0245EFE0 */
static void* daRd_HIO_c_ct(void* p) {
    if (p == nullptr) {
        p = operator_new(0xA8);
        if (p == nullptr) return p;
    }
    daRd_HIO_c* h = static_cast<daRd_HIO_c*>(p);
    h->__vtbl = 0x10038EF4; /* vtable */
    RdHD::constructNpcHIO(&h->mNpc);    /* dNpc_HIO_c::dNpc_HIO_c */
    RdHD::constructJntHitHIO(h->m7C); /* JntHit_HIO_c::JntHit_HIO_c */
    h->m2C = 0;
    h->m30 = 650.0f;
    h->m34 = 650.0f;
    h->mCryRadius = 500.0f;
    h->mAttackRadius = 125.0f;
    h->m40 = 0x2000;
    h->mCrySpreadAngle = 0x1B58;
    h->mAttackSpreadAngle = 0x6000;
    h->m46 = 0xA;
    h->m48 = 0x5;
    h->m4A = 0x3C;
    h->m4C = 0x32;
    h->m4E = 0x384;
    h->m58 = 10.0f;
    h->m5C = 10.0f;
    h->m60 = 50.0f;
    h->m64 = 30.0f;
    h->m68 = 1.8f;
    h->mReturnWalkSpeed = 2.0f;
    h->m70 = 1.25f;
    h->m74 = 0.9f;
    h->m50 = 0x28;
    h->m78 = 0x2D;
    h->m54 = 0x87;
    h->m52 = 0x96;
    h->mParalysisDuration = 2 * 30;
    h->mNpc.m04 = -20.0f; /* mNpc.m04 */
    h->mNpc.mMaxHeadX = 0x1FFE;
    h->mNpc.mMaxHeadY = 0x4000;
    h->mNpc.mMaxBackboneX = 0x0;
    h->mNpc.mMaxBackboneY = 0x2000;
    h->mNpc.mMinHeadX = -0x9C4;
    h->mNpc.mMinHeadY = -0x4000;
    h->mNpc.mMinBackboneX = 0x0;
    h->mNpc.mMinBackboneY = -0x2000;
    h->mNpc.mMaxTurnStep = 0x250;
    h->mNpc.mMaxHeadTurnVel = 0x150;
    h->mNpc.mAttnYOffset = 50.0f;  /* mNpc.mAttnYOffset */
    h->mNpc.mMaxAttnAngleY = 0x7FFF; /* mNpc.mMaxAttnAngleY */
    h->mNpc.m22 = 0;       /* mNpc.m22 */
    h->mNpc.mMaxAttnDistXZ = 400.0f; /* mNpc.mMaxAttnDistXZ */
    return p;
}

/* 0245A20C */
static void* searchNeadDeadRd_CB(void* i_actor, void* i_this) {
    return ((daRd_c*)i_this)->_searchNearDeadRd((fopAc_ac_c*)i_actor);
}

/* model->setAnmMtx(jntNo, mDoMtx_stack_c::get()) */

/* 0245A194 */
fopAc_ac_c* daRd_c::_searchNearDeadRd(fopAc_ac_c* i_actor) {
    if (i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_RD_e) { // HD: fopAcM_GetName checks for NULL
        daRd_c* other = (daRd_c*)i_actor;
        if (other->mMode == MODE_DEATH) {
            if (fopAcM_searchActorDistanceXZ(this, i_actor) < l_HIO().m34) {
                return i_actor;
            }
        }
    }
    return nullptr;
}

/* 0245A494 */
static BOOL nodeControl_CB(J3DNode* node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys_model();
        daRd_c* i_this = static_cast<daRd_c*>(model->getUserArea());
        if (i_this != nullptr) {
            i_this->_nodeControl(node, model);
        }
    }
    return TRUE;
}

/* 0245A21C */
void daRd_c::_nodeControl(J3DNode* node, J3DModel* model) {
    J3DJoint* joint = J3DNode_toJoint(node);
    u32 jntNo = reinterpret_cast<RdJointData*>(joint)->mJntNo;
    PSMTXCopy(rd_getAnmMtx(model, jntNo), mtx_now());

    if (mJntCtrl.mHeadJntNum == (s32)jntNo) {
        static cXyz l_offsetAttPos;
        static cXyz l_offsetEyePos;
        static u32 attPosGuard;
        static u32 eyePosGuard;
        // HD: GHS writes each guard amid the component stores in this order.
        if (attPosGuard == 0) {
            l_offsetAttPos.x = 0.0f;
            l_offsetAttPos.z = 0.0f;
            attPosGuard = 1;
            l_offsetAttPos.y = 0.0f;
        }
        if (eyePosGuard == 0) {
            eyePosGuard = 1;
            l_offsetEyePos.x = 24.0f;
            l_offsetEyePos.z = 0.0f;
            l_offsetEyePos.y = -16.0f;
        }
        PSMTXMultVec(mtx_now(), &l_offsetAttPos, &mTargetPos);
        mDoMtx_XrotM(mtx_now(), mJntCtrl.mAngles[0][1]);
        mDoMtx_ZrotM(mtx_now(), mJntCtrl.mAngles[0][0]);
        PSMTXMultVec(mtx_now(), &l_offsetEyePos, &mRdEyePos);
        mDoMtx_XrotM(mtx_now(), mD1A);
        mDoMtx_ZrotM(mtx_now(), mD1C);
        mDoMtx_YrotM(mtx_now(), mD1E);
    } else if (mJntCtrl.mBackboneJntNum == (s32)jntNo) {
        mDoMtx_XrotM(mtx_now(), mJntCtrl.mAngles[1][1]);
        mDoMtx_ZrotM(mtx_now(), mJntCtrl.mAngles[1][0]);
    }

    PSMTXCopy(mtx_now(), J3DSys_mCurrentMtx());
    rd_setAnmMtx(model, jntNo);
}

/* 0245A678 */
static BOOL nodeHeadControl_CB(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys_model();
        daRd_c* i_this = static_cast<daRd_c*>(model->getUserArea());
        if (i_this != nullptr) {
            i_this->_nodeHeadControl(node, model);
        }
    }
    return TRUE;
}

/* 0245A4DC */
void daRd_c::_nodeHeadControl(J3DNode* node, J3DModel* model) {
    J3DJoint* joint = J3DNode_toJoint(node);
    u32 jntNo = reinterpret_cast<RdJointData*>(joint)->mJntNo;

    f32 d38 = mD38; /* cXyz temp4(mD38, 0, 0) */
    PSMTXCopy(rd_getAnmMtx(model, jntNo), mtx_now());
    mDoMtx_stack_transM(d38, 0.0f, 0.0f);

    PSMTXCopy(mtx_now(), J3DSys_mCurrentMtx());
    gabi::Local<cXyz> temp3;
    temp3->x = 0.0f;
    temp3->y = 0.0f;
    temp3->z = 50.0f;
    PSMTXMultVec(mtx_now(), temp3, &mRdEyePos);
    rd_setAnmMtx(model, jntNo);
    PSMTXCopy(mtx_now(), &mCE8);

    gabi::Local<Mtx34> mtx;
    gabi::Local<csXyz> angle;
    mDoMtx_inverseTranspose(mtx_now(), mtx);
    mDoMtx_MtxToRot(mtx, angle);
    mHeadAngle = angle->y;
}

/* 0245A980 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    return ((daRd_c*)i_this)->_createHeap();
}

/* 0245A72C */
BOOL daRd_c::_createHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BDL_RD_e, SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_rd.cpp", 0x1F8, "modelData != (0)"); /* modelData != NULL */
    }

    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BCK_SUWARIP_e, SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                    nullptr, 0x00080000, 0x37441622);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea */

    if (!invisibleModel_create(mInvisModel, mpMorf->getModel())) {
        return FALSE;
    }

    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BTK_RD_CLOSE_e, SAFESTRING_VTBL);
    if (btk == nullptr) {
        JUT_ASSERT_fail("d_a_rd.cpp", 0x20D, "btk != 0"); /* btk != NULL */
    }
    if (!mBtkAnm.init(modelData, btk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    rd_jointNode(modelData, RD_JNT_REE_ATAMA_1_e)->setCallBack(nodeHeadControl_CB);

    J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BRK_NML_e, SAFESTRING_VTBL);
    if (brk == nullptr) {
        JUT_ASSERT_fail("d_a_rd.cpp", 0x226, "brk != 0"); /* brk != NULL */
    }
    if (!brkAnm_init(&mBrkAnm, modelData, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    return createArrowHeap() ? TRUE : FALSE;
}

/* 0245A6C0 */
bool daRd_c::createArrowHeap() {
    static cXyz sebone_cyl_offset[] = {{0.0f, 0.0f, 0.0f},    {30.0f, 0.0f, 0.0f}};
    static cXyz muneA_cyl_offset[]  = {{5.0f, -5.0f, 0.0f},   {20.0f, -5.0f, 0.0f}};
    static cXyz muneB1_cyl_offset[] = {{15.0f, 0.0f, -15.0f}, {50.0f, 0.0f, -15.0f}};
    static cXyz muneB2_cyl_offset[] = {{15.0f, 0.0f, 0.0f},   {50.0f, 0.0f, 0.0f}};
    static cXyz muneB3_cyl_offset[] = {{15.0f, 0.0f, 15.0f},  {50.0f, 0.0f, 15.0f}};
    static cXyz kosi1_cyl_offset[]  = {{0.0f, 0.0f, -10.0f},  {20.0f, 0.0f, -10.0f}};
    static cXyz kosi2_cyl_offset[]  = {{0.0f, 0.0f, 0.0f},    {20.0f, 0.0f, 0.0f}};
    static cXyz kosi3_cyl_offset[]  = {{0.0f, 0.0f, 10.0f},   {20.0f, 0.0f, 10.0f}};
    static cXyz asi1_cyl_offset[]   = {{-8.0f, 0.0f, 0.0f},   {30.0f, 0.0f, 0.0f}};
    static cXyz asi2_cyl_offset[]   = {{-3.0f, 0.0f, 0.0f},   {15.0f, 0.0f, 0.0f}};
    static cXyz asi3_cyl_offset[]   = {{0.0f, 0.0f, 0.0f},    {11.0f, 0.0f, 0.0f}};
    static cXyz ude1_cyl_offset[]   = {{-5.0f, -8.0f, -1.0f}, {60.0f, 4.0f, 1.0f}};
    static cXyz ude2_cyl_offset[]   = {{-3.0f, 0.0f, 0.0f},   {55.0f, 0.0f, 0.0f}};
    static cXyz te_cyl_offset[]     = {{8.0f, 2.0f, 0.0f},    {30.0f, 2.0f, 0.0f}};
    static cXyz yubi_cyl_offset[]   = {{0.0f, 0.0f, 0.0f},    {17.0f, 0.0f, 0.0f}};
    static __jnt_hit_data_c search_data[] = {
        {
             JntHitType_CYL_e,
             RD_JNT_REE_KOSI_1_e,
             4.0f,
             RdJointOffsets::kosi1_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_KOSI_1_e,
             4.0f,
             RdJointOffsets::kosi2_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_KOSI_1_e,
             4.0f,
             RdJointOffsets::kosi3_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_ASI_L1_e,
             2.0f,
             RdJointOffsets::asi1_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_ASI_L2_e,
             2.0f,
             RdJointOffsets::asi2_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_ASI_L3_e,
             2.0f,
             RdJointOffsets::asi3_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_ASI_R1_e,
             2.0f,
             RdJointOffsets::asi1_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_ASI_R2_e,
             2.0f,
             RdJointOffsets::asi2_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_ASI_R3_e,
             2.0f,
             RdJointOffsets::asi3_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_MUNE_1_e,
             6.0f,
             RdJointOffsets::sebone_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_MUNE_2_e,
             10.0f,
             RdJointOffsets::muneA_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_MUNE_2_e,
             6.0f,
             RdJointOffsets::muneB1_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_MUNE_2_e,
             6.0f,
             RdJointOffsets::muneB2_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_MUNE_2_e,
             6.0f,
             RdJointOffsets::muneB3_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_UDE_L1_e,
             2.0f,
             RdJointOffsets::ude1_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_UDE_L2_e,
             2.0f,
             RdJointOffsets::ude2_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_TEKUBI_L1_e,
             6.0f,
             RdJointOffsets::te_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_YUBI_L1_e,
             6.0f,
             RdJointOffsets::yubi_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_UDE_R1_e,
             2.0f,
             RdJointOffsets::ude1_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_UDE_R2_e,
             2.0f,
             RdJointOffsets::ude2_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_TEKUBI_R1_e,
             6.0f,
             RdJointOffsets::te_cyl_offset,
        },
        {
             JntHitType_CYL_e,
             RD_JNT_REE_YUBI_R1_e,
             6.0f,
             RdJointOffsets::yubi_cyl_offset,
        },
    };
    mpJntHit = JntHit_create(mpMorf->getModel(), gabi::ea(search_data), 22);
    if (mpJntHit) {
        jntHit = gabi::ea(mpJntHit.get()); /* fopAcM_SetJntHit */
    } else {
        return false;
    }
    return true;
}

/* 0245D0C8 */
bool daRd_c::checkPlayerInAttack() {
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    return checkInFan(current.pos, player, shape_angle.y, l_HIO().mAttackSpreadAngle, l_HIO().mAttackRadius, 100.0f);
}

/* the wake-up test shared by modeWait and modeReturn */

/* 0245D970 */
bool daRd_c::checkPlayerInCry() {
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    return checkInFan(current.pos, player, mHeadAngle, l_HIO().mCrySpreadAngle, l_HIO().mCryRadius, 100.0f);
}

/* the walk speed toward the player (modeMove, modeCry) */

/* 0245B9A8 */
void daRd_c::lookBack() {
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    bool r29 = false;
    if (cLib_calcTimer(&mCE4) != 0 && mMode != MODE_ATTACK && mMode != MODE_CRY && mMode != MODE_DEATH) {
        mJntCtrl.mbTrn = 0;          /* clrTrn */
        mJntCtrl.mbHeadLock = 0;     /* offHeadLock */
        mJntCtrl.mbBackBoneLock = 0; /* offBackBoneLock */
        mTargetPos.copy(*daPy_getHeadTopPos(player));
    } else {
        switch ((u32)mMode) {
        case MODE_WAIT:
        case MODE_DEATH:
        case MODE_ATTACK:
            r29 = false;
            mJntCtrl.mbTrn = 0;
            mJntCtrl.mbHeadLock = 1;
            mJntCtrl.mbBackBoneLock = 1;
            break;
        case MODE_MOVE:
        case MODE_CRY:
        case MODE_RETURN:
        case MODE_SILENT_PRAY:
            mJntCtrl.mbTrn = 1; /* setTrn */
            /* fall through */
        default:
            mJntCtrl.mbHeadLock = 0;
            mJntCtrl.mbBackBoneLock = 0;
            break;
        }

        switch ((u32)mAnmPrmIdx) {
        case AnmPrm_SUWARIP:
        case AnmPrm_TATSU:
        case AnmPrm_SUWARU:
            r29 = false;
            mJntCtrl.mbTrn = 0;
            mJntCtrl.mbHeadLock = 1;
            mJntCtrl.mbBackBoneLock = 1;
            break;
        }

        switch ((u32)mMode) {
        case MODE_CRY:
            if (checkInFan(current.pos, player, shape_angle.y, l_HIO().mCrySpreadAngle, 150.0f, 100.0f)) {
                mJntCtrl.mbTrn = 0;
                mJntCtrl.mbHeadLock = 1;
                mJntCtrl.mbBackBoneLock = 1;
            }
            mTargetPos.copy(*daPy_getHeadTopPos(player));
            break;
        case MODE_RETURN:
            if (checkInCircle(mSpawnPos, this, 100.0f, 1000.0f)) {
                mJntCtrl.mbTrn = 0;
                mJntCtrl.mbHeadLock = 1;
                mJntCtrl.mbBackBoneLock = 1;
            }
            mTargetPos.copy(mSpawnPos);
            break;
        case MODE_SILENT_PRAY:
            break;
        default:
            mTargetPos.copy(*daPy_getHeadTopPos(player));
            break;
        }
    }

    if (mJntCtrl.mbTrn != 0) { /* trnChk */
        cLib_addCalcAngleS2(&mMaxHeadTurnVel, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
    } else {
        mMaxHeadTurnVel = 0;
    }

    lookAtTarget(&mJntCtrl, &shape_angle.y, &mTargetPos, mRdEyePos, shape_angle.y, mMaxHeadTurnVel, r29);
}

/* 0245C770 */
bool daRd_c::checkTgHit() {
    if (mMode == MODE_DEATH || mMode == MODE_KANOKE || mMode == MODE_SW_WAIT) {
        return false;
    }

    dCcD_GObjInf* hitObj;

    if (mCyl.ChkTgHit()) {
        hitObj = static_cast<dCcD_GObjInf*>(mCyl.GetTgHitObj());
        if ((u32)hitObj->mAtType == AT_TYPE_LIGHT) { /* GetAtType */
            rd_seStart(this, JA_SE_CM_PW_BECOME_SOLID, 0);
            modeProcInit(MODE_PARALYSIS);
            return true;
        }
    }

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    mStts.Move();
    if (cLib_calcTimer(&m2C4) == 0 && mCyl.ChkTgHit()) {
        bool r29 = true;
        hitObj = static_cast<dCcD_GObjInf*>(mCyl.GetTgHitObj());
        m2C4 = (u8)l_HIO().m48;
        if (hitObj == nullptr) {
            return false;
        }

        switch ((u32)hitObj->mAtType) {
        case AT_TYPE_SWORD:
        case AT_TYPE_MACHETE:
        case AT_TYPE_UNK800:
        case AT_TYPE_DARKNUT_SWORD:
        case AT_TYPE_MOBLIN_SPEAR:
            rd_seStart(this, JA_SE_LK_SW_HIT_S, 0x20);
            switch (daPy_getCutType(player)) {
            case RdCutType::CUT_TYPE_BT_JUMPCUT: case RdCutType::CUT_TYPE_CUT_EA: case RdCutType::CUT_TYPE_CUT_EB: case RdCutType::CUT_TYPE_CUT_TURN: case RdCutType::CUT_TYPE_CUT_ROLL: case RdCutType::CUT_TYPE_JUMPCUT_SWORD: /* jump cut, EA, EB, turn, roll, ... */
            case RdCutType::CUT_TYPE_JUMPCUT_STICK: case RdCutType::CUT_TYPE_JUMPCUT_MACHETE: case RdCutType::CUT_TYPE_BT_ROLLCUT: case RdCutType::CUT_TYPE_BT_VERTICALJUMPCUT: case RdCutType::CUT_TYPE_JUMPCUT_CLUB: case RdCutType::CUT_TYPE_JUMPCUT_DN_SWORD: case RdCutType::CUT_TYPE_JUMPCUT_SPEAR: case RdCutType::CUT_TYPE_CUT_EXA: case RdCutType::CUT_TYPE_CUT_EXB:
                mHitType = 1;
                break;
            default:
                mHitType = 0;
                break;
            }
            break;
        case AT_TYPE_WIND:
            r29 = false;
            mHitType = 3;
            mCE0 = 40;
            break;
        case AT_TYPE_BOOMERANG:
        case AT_TYPE_BOKO_STICK:
            r29 = false;
            rd_seStart(this, JA_SE_LK_W_WEP_HIT, 0x44);
            mHitType = 4;
            mCE0 = 40;
            break;
        case AT_TYPE_HOOKSHOT:
            rd_seStart(this, JA_SE_LK_HS_SPIKE, 0x44);
            r29 = false;
            mHitType = 12;
            break;
        case AT_TYPE_SKULL_HAMMER:
        case AT_TYPE_STALFOS_MACE:
            rd_seStart(this, JA_SE_LK_HAMMER_HIT, 0x20);
            mHitType = 7;
            if (daPy_getCutType(player) == RdCutType::CUT_TYPE_HAMMER_SIDESWING) {
                mHitType = 8;
            }
            break;
        case AT_TYPE_BOMB:
            mHitType = 6;
            break;
        case AT_TYPE_ICE_ARROW:
            r29 = false;
            rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x44);
            mEnemyIce.mFreezeDuration = l_HIO().m4A;
            enemy_fire_remove(&mEnemyFire);
            health = health + 4;
            mHitType = 9;
            break;
        case AT_TYPE_FIRE_ARROW:
            r29 = false;
            rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x44);
            mEnemyFire.mFireDuration = l_HIO().m4C;
            mHitType = 0xA;
            break;
        case AT_TYPE_LIGHT_ARROW:
            r29 = false;
            rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x20);
            mEnemyIce.mLightShrinkTimer = 1;
            mHitType = 0xB;
            break;
        case AT_TYPE_NORMAL_ARROW:
            mHitType = 5;
            if (!checkInCircle(current.pos, player, l_HIO().mCryRadius, 1000.0f)) {
                rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x44);
                mCE0 = 40;
                r29 = false;
            } else {
                rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x20);
            }
            break;
        case AT_TYPE_FIRE:
        case AT_TYPE_UNK20000:
            r29 = false;
            if (mEnemyFire.mMode == 0) {
                mEnemyFire.mFireDuration = l_HIO().m4C;
            } else {
                mHitType = 0xD;
            }
            break;
        case AT_TYPE_GRAPPLING_HOOK:
            dComIfGp_particle_set(RdParticle::ID_IT_JN_PIYOHIT00, attention_pos(this));
            rd_seStart(this, JA_SE_LK_W_WEP_HIT, 0x44);
            mHitType = 0xE;
            r29 = false;
            mCE0 = 40;
            break;
        }

        gabi::Local<CcAtInfo_l> atInfo;
        atInfo->pParticlePos = nullptr;
        atInfo->mpObj = gabi::ea(mCyl.GetTgHitObj());
        if (r29) {
            cXyz* hitPos = mCyl.GetTgHitPosP();
            cc_at_check(this, atInfo);
            // HD: a stronger hit (resulting attack type 1) re-arms the hit timer at 3 while the player's byte +0x69E8 is set
            if (atInfo->mResultingAttackType == 1 && (u8)static_cast<RdLinkState*>(player)->mStrongHitTimerActive != 0) {
                m2C4 = 3;
            }
            if (mHitType == 1 || mHitType == 7 || mHitType == 8 || health <= 0) {
                dComIfGp_particle_set(RdParticle::ID_AK_JN_CRITICALHITFLASH, hitPos);
                gabi::Local<cXyz> scale;
                scale->x = 2.0f;
                scale->y = 2.0f;
                scale->z = 2.0f;
                dComIfGp_particle_set(RdParticle::ID_AK_JN_CRITICALHIT, hitPos, &player->shape_angle, scale);
                if (health <= 0) {
                    modeProcInit(MODE_DEATH);
                } else {
                    modeProcInit(MODE_DAMAGE);
                }
            } else {
                dComIfGp_particle_set(RdParticle::ID_AK_JN_OK, hitPos, &player->shape_angle);
                modeProcInit(MODE_DAMAGE);
            }
        } else if (mHitType == 0xE) {
            s8 origHealth = health;
            health = 0xA;
            cc_at_check(this, atInfo);
            health = origHealth;
        }

        return true;
    }

    if (dComIfGp_getDetect_chk_light(&current.pos)) {
        rd_seStart(this, JA_SE_CM_PW_BECOME_SOLID, 0);
        modeProcInit(MODE_PARALYSIS);
        return true;
    }

    return false;
}

/* 0245BD98 */
void daRd_c::setCollision() {
    if (mMode == MODE_DEATH) {
        mCyl.OffCoSPrmBit(0x12); /* VsEnemy | IsEnemy */
        mCyl.OffTgSPrmBit(0x9);  /* Set | IsOther */
    } else if (mMode == MODE_ATTACK || mMode == MODE_CRY || dComIfGp_evmng_startCheckOld("DEFAULT_RD_CRY" /* "DEFAULT_RD_CRY" */)) {
        mCyl.OffCoSPrmBit(0x12);
    } else {
        mCyl.OnCoSPrmBit(0x12);
    }

    f32 r1 = REG_F(8, 1);
    bool suwari = isAnm(AnmPrm_SUWARIP);
    mCyl.SetR(r1 + 80.0f);
    f32 r0 = REG_F(8, 0);
    if (suwari) {
        mCyl.SetH(r0 + 170.0f);
    } else {
        mCyl.SetH(r0 + 250.0f);
    }

    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 0245B818 */
void daRd_c::setIceCollision() {
    if (isAnm(AnmPrm_SUWARIP)) {
        mEnemyIce.mWallRadius = 50.0f;
        mEnemyIce.mCylHeight = 170.0f;
    } else {
        mEnemyIce.mWallRadius = 50.0f;
        mEnemyIce.mCylHeight = 250.0f;
    }
}

/* 0245B854 */
void daRd_c::setAttention() {
    gabi::Local<cXyz> attnPos;
    attnPos->x = 60.0f;
    attnPos->y = 0.0f;
    attnPos->z = 0.0f;
    PSMTXCopy(rd_getAnmMtx(mpMorf->getModel(), RD_JNT_REE_ATAMA_1_e), mtx_now());
    PSMTXMultVec(mtx_now(), attnPos, attention_pos(this));
    /* mDoMtx_stack_c::multVecZero(&eyeballPos); eyePos = eyeballPos */
    Mtx34* m = mtx_now();
    eyePos.x = m->m[0][3];
    eyePos.z = m->m[2][3];
    f32 ey = m->m[1][3];
    eyePos.y = ey;
    attention_pos(this)->y = attention_pos(this)->y + l_HIO().m58;
    eyePos.y = ey + l_HIO().m5C;

    if (dComIfGp_event_runCheck()) {
        attention_pos(this)->copy(current.pos);
        eyePos.copy(current.pos);
        attention_pos(this)->y = attention_pos(this)->y + 150.0f;
        eyePos.y = eyePos.y + 150.0f;
    }

    if (mEnemyIce.mFreezeTimer > 20) {
        actor_status &= ~u32(fopAcStts_UNK200000_e); /* fopAcStts_UNK200000_e */
    } else {
        actor_status |= fopAcStts_UNK200000_e;
    }
}

/* 0245B154 */
void daRd_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(model, mtx_now());
}

/* createInit: the tail GHS duplicated into each branch */

/* 0245C72C */
void daRd_c::modeWaitInit() {
    AcchCir_SetWallR(&mAcchCir, 60.0f);
    speedF = 0.0f;
}

/* 0245D134 */
void daRd_c::modeWait() {
    if (checkTgHit()) {
        return;
    }

    speedF = 0.0f;
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    BOOL isOto = fopAcM_otoCheck(this, (f32)mAreaRadius);
    if (!isLinkControl()) {
        if (rd_noticePlayer(this, player, isOto)) {
            modeProcInit(MODE_MOVE);
            return;
        }
    }

    if (checkInCircle(mSpawnPos, this, 100.0f, 1000.0f)) {
        cLib_addCalcAngleS2(&shape_angle.y, mSpawnAngle, 0x4, 0x200);
        if (cLib_distanceAngleS(shape_angle.y, mSpawnAngle) <= 0x200) {
            shape_angle.y = mSpawnAngle;
            if (mWhichIdleAnm == 1 && !isAnm(AnmPrm_SUWARIP)) {
                setAnm(AnmPrm_SUWARU, false);
            }
            if (mWhichIdleAnm == 0) {
                setAnm(AnmPrm_TACHIP1, false);
            }
        }
    } else if (!checkInCircle(mSpawnPos, this, (f32)mAreaRadius, 1000.0f) ||
               !checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
        modeProcInit(MODE_RETURN);
        return;
    }
    if (!isLinkControl() && checkPlayerInAttack() && isAnm(AnmPrm_TACHIP1)) {
        modeProcInit(MODE_ATTACK);
    }
}

/* 0245D7E8 */
void daRd_c::modeDeathInit() {
    rd_monsSeStart(this, JA_SE_CV_RD_DIE, 0);
    setAnm(AnmPrm_DEAD, false);
    speedF = 0.0f;
    // HD: the body stays for a random 50..150 frames (GameCube: 10 seconds)
    mTimer1 = gabi::ftoi(cM_rndF(100.0f) + 50.0f);
}

/* 0245D87C */
void daRd_c::modeDeath() {
    if (!morf_isStop(mpMorf)) {
        return;
    }

    dComIfGs_onActor(setID, home.roomNo); /* fopAcM_onActor */

    /* Do not consider the ReDead to be a living enemy while its death animation is playing out. */
    group = fopAc_ENV_e; /* fopAcM_SetGroup(this, fopAc_ENV_e) */

    if (cLib_calcTimer(&mTimer1) == 0) {
        // HD: the disappear effect 20 units higher and size 8 (GameCube: at the position, size 5); same drop type 0 (daDisItem_IBALL_e)
        gabi::Local<cXyz> pos;
        f32 y = current.pos.y + 20.0f;
        pos->x = current.pos.x;
        pos->z = current.pos.z;
        pos->y = y;
        fopAcM_createDisappear(this, pos, 8, 0, 0xFF);
        fopAcM_delete(this);
    }
}

/* 0245D558 */
void daRd_c::modeDamageInit() {
    rd_monsSeStart(this, JA_SE_CV_RD_DAMAGE, 0);
    setAnm(AnmPrm_DAMAGE, true);
    speedF = 0.0f;
}

/* 0245D5C0 */
void daRd_c::modeDamage() {
    if (checkTgHit()) {
        return;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    if (!morf_isStop(mpMorf)) {
        return;
    }

    if (checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
        modeProcInit(MODE_MOVE);
    } else {
        modeProcInit(MODE_WAIT);
    }
}

/* 0245D698 */
void daRd_c::modeParalysisInit() {
    if (!isAnm(AnmPrm_BEAM_HIT) && !isAnm(AnmPrm_BEAM)) {
        setAnm(AnmPrm_BEAM_HIT, false);
    }
    mTimer1 = l_HIO().mParalysisDuration;
    speedF = 0.0f;
}

/* 0245D700 */
void daRd_c::modeParalysis() {
    if (isAnm(AnmPrm_BEAM_HIT) && morf_isStop(mpMorf)) {
        setAnm(AnmPrm_BEAM, false);
    } else if (isAnm(AnmPrm_BEAM)) {
        if (cLib_calcTimer(&mTimer1) == 0) {
            setAnm(AnmPrm_BEAM_END, false);
        }
    } else if (isAnm(AnmPrm_BEAM_END) && morf_isStop(mpMorf)) {
        modeProcInit(MODE_WAIT);
    }

    checkTgHit();
}

/* 0245D940 */
void daRd_c::modeMoveInit() {
    if (mWhichIdleAnm == 1 && (isAnm(AnmPrm_SUWARIP) || isAnm(AnmPrm_SUWARU))) {
        setAnm(AnmPrm_TATSU, false);
    }
}

/* 0245D9DC */
void daRd_c::modeMove() {
    if (!checkTgHit() && !isAnm(AnmPrm_TATSU)) {
        if (!isLinkControl() && checkPlayerInCry()) {
            modeProcInit(MODE_CRY);
            return;
        }
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        if (!checkInCircle(mSpawnPos, this, (f32)mAreaRadius, 1000.0f) ||
            !checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
            modeProcInit(MODE_RETURN);
            return;
        }
        int angle = cLib_distanceAngleS(fopAcM_searchPlayerAngleY(this), mHeadAngle);
        if (angle < 0x50) {
            rd_walkToPlayer(this, fopAcM_searchPlayerDistanceXZ(this));
        } else {
            cLib_addCalc2(&speedF, 0.0f, 0.1f, REG_F(12, 0) + 0.1f);
        }
        if (speedF < 0.1f) {
            setAnm(AnmPrm_TACHIP1, false);
        } else {
            setAnm(AnmPrm_WALK, false);
        }
        if (!isLinkControl() && checkPlayerInAttack()) {
            modeProcInit(MODE_ATTACK);
        }
    }
}

/* 0245DC9C */
void daRd_c::modeCryInit() {
    if (isLinkControl()) {
        modeProcInit(MODE_RETURN);
        return;
    }
    if (dComIfGp_evmng_startCheckOld("DEFAULT_RD_CRY" /* "DEFAULT_RD_CRY" */)) {
        dComIfGp_event_reset();
    }
    fopAcM_orderOtherEvent2(this, "DEFAULT_RD_CRY", 1, 0xFFFF);
    rd_monsSeStart(this, JA_SE_CV_RD_SCREAM, 0);
    mTimer1 = l_HIO().m54;
    mBreakFreeCounter = l_HIO().m78;
}

/* mashing the stick and buttons to break free (modeCry, modeAttack) */

/* 0245DD80 */
void daRd_c::modeCry() {
    setAnm(AnmPrm_WALK, false);

    f32 stickPosX = CPad_GET_STICK_POS_X(0);
    f32 stickPosY = CPad_GET_STICK_POS_Y(0);
    if (eventInfo_checkCommandDemoAccrpt(this) || dComIfGp_evmng_startCheckOld("DEFAULT_RD_CRY" /* "DEFAULT_RD_CRY" */)) {
        if (isLinkControl()) {
            dComIfGp_event_reset();
            modeProcInit(MODE_RETURN);
            return;
        }
        bool hit = checkTgHit();
        u32 play = dComIfGp_ea(); // HD: fetched once for both branches
        if (hit) {
            event_reset_at(play);
        } else {
            fopAcM_searchActorAngleY(this, player0_at(play)); /* s16 targetY (unused) */
            rd_walkToPlayer(this, fopAcM_searchPlayerDistanceXZ(this));

            rd_breakFree(this, stickPosX, stickPosY);

            if (cLib_calcTimer(&mTimer1) == 0 || mBreakFreeCounter < 0) {
                dComIfGp_event_reset();
                modeProcInit(MODE_CRY_WAIT);
            } else if (checkPlayerInAttack()) {
                dComIfGp_event_reset();
                modeProcInit(MODE_ATTACK);
            }
        }
    } else {
        modeProcInit(MODE_CRY_WAIT);
    }
}

/* 0245E144 */
void daRd_c::modeCryWaitInit() {
    mTimer1 = 60;
    mTimer2 = 45;
}

/* 0245E158 */
void daRd_c::modeCryWait() {
    if (checkTgHit()) {
        return;
    }
    if (isLinkControl()) {
        modeProcInit(MODE_RETURN);
        return;
    }
    if (dComIfGp_evmng_startCheckOld("DEFAULT_RD_CRY" /* "DEFAULT_RD_CRY" */)) {
        onIkari();
        setBtkAnm(3);
    }
    if (dComIfGp_event_runCheck()) {
        mTimer1 = 60;
        mTimer2 = 45;
    } else if (isAnm(AnmPrm_WALK)) {
        if (cLib_calcTimer(&mTimer1) == 0) {
            modeProcInit(MODE_MOVE);
        } else if (cLib_calcTimer(&mTimer2) == 0 && checkPlayerInAttack()) {
            modeProcInit(MODE_ATTACK);
        }
    }
}

/* 0245E298 */
void daRd_c::modeAttackInit() {
    AcchCir_SetWallR(&mAcchCir, 70.0f);
    mTimer1 = l_HIO().m52;
    mBreakFreeCounter = l_HIO().m50;
    mTimer2 = 30;
    setAnm(AnmPrm_WALK, false);
    speedF = 0.0f;
}

/* 0245E30C */
void daRd_c::modeAttack() {
    if (dComIfGp_evmng_startCheckOld("DEFAULT_RD_CRY" /* "DEFAULT_RD_CRY" */)) {
        dComIfGp_event_reset();
    }
    if ((dComIfGp_evmng_startCheckOld("DEFAULT_RD_CRY") || dComIfGp_evmng_startCheckOld("DEFAULT_RD_ATTACK" /* "DEFAULT_RD_ATTACK" */)) &&
        isLinkControl()) {
        dComIfGp_event_reset();
        modeProcInit(MODE_RETURN);
        return;
    }

    f32 stickPosX = CPad_GET_STICK_POS_X(0);
    f32 stickPosY = CPad_GET_STICK_POS_Y(0);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        if (isAnm(AnmPrm_ATACK)) {
            mDoExt_McaMorf* morf = mpMorf;
            f32 frame = daPy_getBaseAnimeFrame(player);
            morf->setFrame(frame);
            cLib_addCalcAngleS2(&shape_angle.y, player->shape_angle.y, 0x4, 0x2000);
        } else {
            s16 targetY = fopAcM_searchPlayerAngleY(this);
            cLib_addCalcAngleS2(&shape_angle.y, targetY, 0x4, 0x2000);
        }

        bool hit = checkTgHit();
        u32 play = dComIfGp_ea(); // HD: fetched once for both branches
        if (hit) {
            event_reset_at(play);
            return;
        }
        f32 dist = fopAcM_searchActorDistanceXZ(this, player0_at(play));
        /* HD branches: written as the original tests them (not greater than) */
        if (!(dist > REG_F(12, 5) + 50.0f) && !isAnm(AnmPrm_ATACK)) {
            offIkari();
            setAnm(AnmPrm_WALK2ATACK, false);
        }
        if (!(dist > REG_F(12, 2) + 20.0f)) {
            cLib_addCalcPosXZ2(&current.pos, &player->current.pos, 0.3f, 1.0f);
            if (cLib_calcTimer(&mTimer2) == 0) {
                bool noDamage = daPy_checkNoDamageMode(dComIfGp_getLinkPlayer());
                fopAc_ac_c* link = dComIfGp_getLinkPlayer();
                if (!noDamage) {
                    daPy_lk_setDamagePoint(link, -1.0f);
                } else {
                    daPy_lk_noDamagePoint(link, -1.0f); /* HD */
                }
                mTimer2 = 30;
            }
        } else if (mAcch.ChkWallHit()) {
            daPy_changeOriginalDemo(player);
            gabi::Local<cXyz> pos;
            pos->copy(player->current.pos);
            cLib_addCalcPosXZ2(pos, &current.pos, 0.3f, 10.0f);
            daPy_setPlayerPosAndAngle(player, pos, player->shape_angle.y);
        } else {
            cLib_addCalcPosXZ2(&current.pos, &player->current.pos, 0.3f, 10.0f);
        }

        if (dComIfGs_getLife() == 0) {
            dComIfGp_event_reset();
            setAnm(AnmPrm_ATACK2WALK, false);
            modeProcInit(MODE_WAIT);
            return;
        }

        rd_breakFree(this, stickPosX, stickPosY);

        if (cLib_calcTimer(&mTimer1) == 0 || mBreakFreeCounter < 0) {
            dComIfGp_event_reset();
            setAnm(AnmPrm_ATACK2WALK, false);
            modeProcInit(MODE_CRY_WAIT);
        }
    } else if (!checkTgHit()) {
        fopAcM_orderOtherEvent2(this, "DEFAULT_RD_ATTACK" /* "DEFAULT_RD_ATTACK" */, 1, 0xFF6F); /* GameCube: 0x1CF */
    }
}

/* 0245F250 */
void daRd_c::modeReturnInit() {
}

/* 0245E964 */
void daRd_c::modeReturn() {
    if (checkTgHit()) {
        return;
    }

    setAnm(AnmPrm_WALK, false);

    if (checkInCircle(mSpawnPos, this, 100.0f, 1000.0f)) {
        speedF = 0.0f;
        cLib_addCalcAngleS2(&shape_angle.y, mSpawnAngle, 0xA, 0x200);
        if (cLib_distanceAngleS(shape_angle.y, mSpawnAngle) <= 0x200) {
            shape_angle.y = mSpawnAngle;
            modeProcInit(MODE_WAIT);
            return;
        }
    } else {
        cLib_addCalc2(&speedF, l_HIO().mReturnWalkSpeed, 0.1f, REG_F(12, 0) + 0.1f);
    }

    if (dComIfGp_evmng_startCheckOld("DEFAULT_RD_ATTACK" /* "DEFAULT_RD_ATTACK" */)) {
        return;
    }

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    BOOL isOto = fopAcM_otoCheck(this, (f32)mAreaRadius);
    if (!isLinkControl()) {
        if (rd_noticePlayer(this, player, isOto)) {
            modeProcInit(MODE_MOVE);
            return;
        }
    }
}

/* 0245F254 */
void daRd_c::modeSilentPrayInit() {
}

/* 0245EC74 */
void daRd_c::modeSilentPray() {
    if (checkTgHit()) {
        return;
    }

    setAnm(AnmPrm_WALK, false);

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<gptr<fopAc_ac_c>> corpse;
    if (fopAcM_SearchByID_out(mCorpseID, corpse)) {
        mTargetPos.copy((*corpse)->current.pos);
        if (checkInCircle(mTargetPos, this, 200.0f, 1000.0f)) {
            setAnm(AnmPrm_TACHIP1, false);
            speedF = 0.0f;
        } else {
            cLib_addCalc2(&speedF, l_HIO().mReturnWalkSpeed, 0.1f, REG_F(12, 0) + 0.1f);
        }
    } else if (!isLinkControl()) {
        if (checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
            modeProcInit(MODE_MOVE);
        } else {
            modeProcInit(MODE_WAIT);
        }
    } else {
        modeProcInit(MODE_RETURN);
    }

    if (!isLinkControl()) {
        if (checkPlayerInCry()) {
            modeProcInit(MODE_CRY);
        }
        if (checkPlayerInAttack()) {
            modeProcInit(MODE_ATTACK);
        }
    }
}

/* 0245EE9C */
void daRd_c::modeSwWaitInit() {
    setAnm(AnmPrm_KANOKEP, false);
}

/* 0245EEA8 */
void daRd_c::modeSwWait() {
    if (dComIfGs_isSwitch(mSwNo, fopAcM_GetRoomNo(this))) {
        modeProcInit(MODE_KANOKE);
    }
}

/* 0245EF04 */
void daRd_c::modeKanokeInit() {
    setAnm(AnmPrm_KANOKEP, false);
    mTimer1 = 90;
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 150.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    PSMTXMultVec(mtx_now(), offset, &mSpawnPos);
}

/* 0245EF98 */
void daRd_c::modeKanoke() {
    if (cLib_calcTimer(&mTimer1) == 0) {
        modeProcInit(MODE_RETURN);
    }
}

/* 0245ADB0 */
void daRd_c::modeProc(daRd_c::Proc_e proc, int newMode) {
    static ModeEntry mode_tbl[] = {
        {
            &daRd_c::modeWaitInit,
            &daRd_c::modeWait,
            "WAIT",
        },
        {
            &daRd_c::modeDamageInit,
            &daRd_c::modeDamage,
            "DAMAGE",
        },
        {
            &daRd_c::modeParalysisInit,
            &daRd_c::modeParalysis,
            "PARALYSIS",
        },
        {
            &daRd_c::modeDeathInit,
            &daRd_c::modeDeath,
            "DEATH",
        },
        {
            &daRd_c::modeMoveInit,
            &daRd_c::modeMove,
            "MOVE",
        },
        {
            &daRd_c::modeCryInit,
            &daRd_c::modeCry,
            "CRY",
        },
        {
            &daRd_c::modeCryWaitInit,
            &daRd_c::modeCryWait,
            "CRY_WAIT",
        },
        {
            &daRd_c::modeAttackInit,
            &daRd_c::modeAttack,
            "ATTACK",
        },
        {
            &daRd_c::modeReturnInit,
            &daRd_c::modeReturn,
            "RETURN",
        },
        {
            &daRd_c::modeSilentPrayInit,
            &daRd_c::modeSilentPray,
            "SILENT_PRAY",
        },
        {
            &daRd_c::modeSwWaitInit,
            &daRd_c::modeSwWait,
            "SW_WAIT",
        },
        {
            &daRd_c::modeKanokeInit,
            &daRd_c::modeKanoke,
            "KANOKE",
        },
    };
    if (proc == PROC_INIT_e) {
        if (newMode == MODE_CRY || newMode == MODE_ATTACK) {
            onIkari();
            setBtkAnm(3);
        } else if (newMode != MODE_DAMAGE) {
            offIkari();
            setBtkAnm(4);
        }

        if (newMode == MODE_DEATH || newMode == MODE_SW_WAIT) {
            actor_status &= ~u32(fopAcStts_SHOWMAP_e);            /* fopAcM_OffStatus(SHOWMAP) */
            attention_info.flags &= ~u32(fopAc_Attn_LOCKON_BATTLE_e);      /* LOCKON_BATTLE */
        } else {
            actor_status |= fopAcStts_SHOWMAP_e;
            attention_info.flags |= fopAc_Attn_LOCKON_BATTLE_e;
        }

        mMode = newMode;
        (this->*mode_tbl[newMode].init)(); /* (this->*mode_tbl[mMode].init)() */
    } else if (proc == PROC_EXEC_e) {
        (this->*mode_tbl[mMode].run)(); /* (this->*mode_tbl[mMode].run)() */
    }
}

/* 0245AF80 */
void daRd_c::setBrkAnm(s8 idx) {

    static const int brk_anm_idx_tbl[] = {
        dRes_INDEX_RD_BRK_NML_e,
        dRes_INDEX_RD_BRK_BEAM_HIT_e,
        dRes_INDEX_RD_BRK_BEAM_e,
        dRes_INDEX_RD_BRK_BEAM_END_e,
    };
    static const int a_play_mod_tbl[] = {
        J3DFrameCtrl::EMode_NONE,
        J3DFrameCtrl::EMode_NONE,
        J3DFrameCtrl::EMode_LOOP,
        J3DFrameCtrl::EMode_NONE,
    };


    J3DModel* model = mpMorf->getModel();
    J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(m_arc_name, brk_anm_idx_tbl[idx], SAFESTRING_VTBL);
    if (brk == nullptr) {
        JUT_ASSERT_fail("d_a_rd.cpp", 0x77F, "brk != (0)"); /* brk != NULL */
    }
    brkAnm_init(&mBrkAnm, rd_modelData(model), brk, true, a_play_mod_tbl[idx], 1.0f, 0, -1, true, 0);
}

/* 0245AC14 */
void daRd_c::setBtkAnm(s8 idx) {

    static const int btk_anm_idx_tbl[] = {
        dRes_INDEX_RD_BTK_RD_IKARI_e,
        dRes_INDEX_RD_BTK_RD_NML_e,
        dRes_INDEX_RD_BTK_RD_OPEN_e,
        dRes_INDEX_RD_BTK_RD_CLOSE_e,
    };
    static const rd_anm_prm_struct btk_anm_prm_tbl[] = {
        {0x01,   -1, J3DFrameCtrl::EMode_NONE},
        {0x00,   -1, J3DFrameCtrl::EMode_LOOP},
        {0x01,   -1, J3DFrameCtrl::EMode_LOOP},
        {0x02, 0x01, J3DFrameCtrl::EMode_NONE},
        {0x03, 0x02, J3DFrameCtrl::EMode_NONE},
    };


    if (idx != 5) {
        m6DB = idx;
    }

    int r5 = btk_anm_prm_tbl[m6DB].m00;
    if (m6DC != m6DB && r5 != -1) {
        J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arc_name, btk_anm_idx_tbl[r5], SAFESTRING_VTBL);
        if (btk == nullptr) {
            JUT_ASSERT_fail("d_a_rd.cpp", 0x7A7, "btk != (0)"); /* btk != NULL */
        }
        J3DModelData* modelData = rd_modelData(mpMorf->getModel());
        mBtkAnm.init(modelData, btk, true, btk_anm_prm_tbl[m6DB].loopMode, 1.0f, 0, -1, true, 0);

        /* isStop */
        if (mBtkAnm.mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || mBtkAnm.mFrameCtrl.getRate() == 0.0f) {
            if (btk_anm_prm_tbl[m6DB].m01 != -1 && btk_anm_prm_tbl[m6DB].loopMode == J3DFrameCtrl::EMode_NONE) {
                m6DB = btk_anm_prm_tbl[m6DB].m01;
            }
        }
    }

    m6DC = m6DB;
}

/* 0245B050 */
void daRd_c::setAnm(s8 anmPrmIdx, bool force) {

    static const int a_anm_bcks_tbl[] = {
        dRes_INDEX_RD_BCK_TACHIP_e,
        dRes_INDEX_RD_BCK_SUWARIP_e,
        dRes_INDEX_RD_BCK_WALK2ATACK_e,
        dRes_INDEX_RD_BCK_ATACK_e,
        dRes_INDEX_RD_BCK_ATACK2WALK_e,
        dRes_INDEX_RD_BCK_WALK_e,
        dRes_INDEX_RD_BCK_DAMAGE_e,
        dRes_INDEX_RD_BCK_DEAD_e,
        dRes_INDEX_RD_BCK_TATSU_e,
        dRes_INDEX_RD_BCK_SUWARU_e,
        dRes_INDEX_RD_BCK_KANOKEP_e,
        dRes_INDEX_RD_BCK_BEAM_HIT_e,
        dRes_INDEX_RD_BCK_BEAM_e,
        dRes_INDEX_RD_BCK_BEAM_END_e,
    };
    static const dLib_anm_prm_c a_anm_prm_tbl[] = {
        {

             BckIdx_TACHIP,
             -1,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_LOOP
        },
        {

             BckIdx_TACHIP,
             -1,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_LOOP
        },
        {

             BckIdx_SUWARIP,
             -1,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_LOOP
        },
        {

             BckIdx_WALK2ATACK,
             AnmPrm_ATACK,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_NONE
        },
        {

             BckIdx_ATACK,
             -1,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_LOOP
        },
        {

             BckIdx_ATACK2WALK,
             AnmPrm_WALK,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_NONE
        },
        {

             BckIdx_WALK,
             -1,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_LOOP
        },
        {

             BckIdx_DAMAGE,
             AnmPrm_TACHIP1,
             0,
             2.0f,
             1.0f,
             J3DFrameCtrl::EMode_NONE
        },
        {

             BckIdx_DEAD,
             -1,
             0,
             2.0f,
             1.0f,
             J3DFrameCtrl::EMode_NONE
        },
        {

             BckIdx_TATSU,
             AnmPrm_TACHIP1,
             0,
             8.0f,
             0.5f,
             J3DFrameCtrl::EMode_NONE
        },
        {

             BckIdx_SUWARU,
             AnmPrm_SUWARIP,
             0,
             8.0f,
             0.5f,
             J3DFrameCtrl::EMode_NONE
        },
        {

             BckIdx_KANOKEP,
             -1,
             0,
             8.0f,
             1.0f,
             J3DFrameCtrl::EMode_LOOP
        },
        {

             BckIdx_BEAM_HIT,
             -1,
             0,
             2.0f,
             1.0f,
             J3DFrameCtrl::EMode_NONE
        },
        {

             BckIdx_BEAM,
             -1,
             0,
             2.0f,
             1.0f,
             J3DFrameCtrl::EMode_LOOP
        },
        {

             BckIdx_BEAM_END,
             -1,
             0,
             2.0f,
             1.0f,
             J3DFrameCtrl::EMode_NONE
        },
    };


    if (anmPrmIdx != AnmPrm_NULL) {
        mAnmPrmIdx = anmPrmIdx;
    }

    if (mOldAnmPrmIdx != mAnmPrmIdx) {
        if (isAnm(AnmPrm_BEAM_HIT)) {
            setBrkAnm(1);
        } else if (isAnm(AnmPrm_BEAM)) {
            setBrkAnm(2);
        } else if (isAnm(AnmPrm_BEAM_END)) {
            setBrkAnm(3);
        } else {
            setBrkAnm(0);
        }
    }

    if (mBckIdx == BckIdx_BEAM_HIT || mBckIdx == BckIdx_BEAM || mBckIdx == BckIdx_BEAM_END) {
        /* mBrkAnm.setFrame(mpMorf->getFrame()) */
        *reinterpret_cast<be<u32>*>(&mBrkAnm.mFrameCtrl.mFrame) = *reinterpret_cast<be<u32>*>(&mpMorf->mFrameCtrl.mFrame);
    }

    dLib_bcks_setAnm(m_arc_name, mpMorf, &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx, gabi::ea(a_anm_bcks_tbl),
                     gabi::ea(a_anm_prm_tbl), force);
}

/* 0245BF50 */
bool daRd_c::_execute() {
    if (mMode == MODE_SW_WAIT) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
        mAcch.CrrPos(dComIfG_Bgsp());
        setMtx();
        mpMorf->play(nullptr, 0, 0);
        mpMorf->calc();
        modeProc(PROC_EXEC_e, MODE_NULL);
        return true;
    }

    fopAcM_setGbaName(this, dItemNo_MIRROR_SHIELD_e, 0x12, 0x30);
    setIceCollision();
    if (mMode != MODE_SILENT_PRAY && mMode != MODE_DEATH && mMode != MODE_DAMAGE && mMode != MODE_ATTACK && mMode != MODE_CRY &&
        mMode != MODE_CRY_WAIT) {
        fopAc_ac_c* corpse = static_cast<fopAc_ac_c*>(fopAcM_Search(searchNeadDeadRd_CB, this));
        if (corpse != nullptr) {
            mCorpseID = fopAcM_GetID(corpse);
            modeProcInit(MODE_SILENT_PRAY);
        }
    }
    if (mMode != MODE_ATTACK && mMode != MODE_WAIT && mMode != MODE_DEATH && mMode != MODE_RETURN) {
        if (dComIfGp_evmng_startCheckOld("DEFAULT_RD_ATTACK" /* "DEFAULT_RD_ATTACK" */)) {
            modeProcInit(MODE_RETURN);
        }
    }
    current.angle.y = shape_angle.y;
    current.angle.z = shape_angle.z;
    current.angle.x = shape_angle.x;

    if (mEnemyFire.mMode == 0) {
        mCyl.SetAtType(0);
        m6D4 = l_HIO().m4E;
    } else {
        mCyl.SetAtType(AT_TYPE_FIRE);
        if (mMode != MODE_DEATH && mMode != MODE_CRY && mMode != MODE_ATTACK) {
            if (cLib_calcTimer(&m6D4) == 0) {
                modeProcInit(MODE_DEATH);
            }
        }
    }

    if (enemy_ice(&mEnemyIce)) {
        J3DModel* model = mpMorf->getModel();
        J3DModel_setBaseTRMtx(model, mtx_now());
        mpMorf->calc();
        speedF = 0.0f;
        setAttention();
        return true;
    }

    if (mCE0 == 1) {
        mCE4 = 4 * 30;
    }

    if (cLib_calcTimer(&mCE0) == 0) {
        cLib_addCalcAngleS2(&mD1E, 0, 0xC, 0x100);
        cLib_addCalcAngleS2(&mD2E, 0, 0xC, 0x100);
    } else {
        cLib_addCalcAngleS2(&mD1E, 0x2000, 0x4, 0x800);
        cLib_addCalcAngleS2(&mD2E, 0x500, 0x4, 0x200);
    }

    dComIfGp_get(); // HD: a dead dComIfGp_get() call before the HIO reads
    mJntCtrl.setParam(l_HIO().mNpc.mMaxBackboneX, l_HIO().mNpc.mMaxBackboneY, l_HIO().mNpc.mMinBackboneX, l_HIO().mNpc.mMinBackboneY,
                      l_HIO().mNpc.mMaxHeadX, l_HIO().mNpc.mMaxHeadY, l_HIO().mNpc.mMinHeadX, l_HIO().mNpc.mMinHeadY,
                      l_HIO().mNpc.mMaxTurnStep);

    if (mMode != MODE_PARALYSIS) {
        lookBack();
    }

    if (isIkari()) {
        cLib_addCalc2(&mD38, l_HIO().m60, 0.1f, l_HIO().m64);
    } else {
        cLib_addCalc2(&mD38, 0.0f, 0.1f, l_HIO().m64);
    }

    if (isAnm(AnmPrm_WALK)) {
        f32 temp = speedF * l_HIO().m70;
        f32 m74 = l_HIO().m74;
        /* temp < m74 ? m74 : temp, as fsel (temp - m74 >= 0 ? temp : m74) */
        mpMorf->setPlaySpeed(temp - m74 >= 0.0f ? temp : m74);
        if (mMode == MODE_ATTACK) {
            mpMorf->setPlaySpeed(3.0f);
        }
    }

    setAttention();
    setCollision();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    setMtx();
    mBtkAnm.play();
    mDoExt_baseAnm_play(&mBrkAnm);
    mpMorf->play(&current.pos, 0, 0);
    mpMorf->calc();
    enemy_fire(&mEnemyFire);
    modeProc(PROC_EXEC_e, MODE_NULL);
    setAnm(AnmPrm_NULL, false);
    setBtkAnm(5);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

    return false;
}

/* 0245C49C */
void daRd_c::debugDraw() {
    rd_static_color(0x101FDA48, 0x101FEBEC, 0x10038E78); /* the unused colour {0x00, 0xFF, 0x00, 0x80} */
    gabi::Local<cXyz> pos;
    f32 py = current.pos.y + 10.0f;
    pos->x = current.pos.x;
    pos->z = current.pos.z;
    pos->y = py;
    u32 yellow = rd_static_color(0x101FDA44, 0x101FEBE8, 0x10038E7C);
    dLib_debugDrawFan(pos, mHeadAngle, l_HIO().mCrySpreadAngle, l_HIO().mCryRadius, yellow);
    u32 red = rd_static_color(0x101FDA50, 0x101FEBF4, 0x10038E80);
    dLib_debugDrawFan(pos, shape_angle.y, l_HIO().mAttackSpreadAngle, l_HIO().mAttackRadius, red);
    u32 magenta = rd_static_color(0x101FDAC4, 0x101FEBFC, 0x10038E84);
    dLib_debugDrawFan(pos, shape_angle.y, l_HIO().m40, l_HIO().m34, magenta);
    dLib_debugDrawAxis(&mCE8, 50.0f);
}

/* 0245C620 */
bool daRd_c::_draw() {
    if (mMode == MODE_SW_WAIT) {
        return true;
    }

    if ((u8)l_HIO().m2C != 0) {
        debugDraw();
    }

    J3DModel* model = mpMorf->getModel();
    J3DModelData* modelData = rd_modelData(model);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);

    if (mEnemyIce.mFreezeTimer > 20) {
        dMat_control_iceEntryDL(mpMorf, -1, mInvisModel);
    } else {
        brkAnm_entry(&mBrkAnm, modelData, mBrkAnm.mFrameCtrl.getFrame());
        mBtkAnm.entry(modelData, mBtkAnm.getFrame());
        mpMorf->updateDL();
        // HD: mBtkAnm.remove / mBrkAnm.remove inline: clear the model data's texture-SRT and tev-register animations
        reinterpret_cast<RdModelAnimations*>(modelData)->mpAnmTexMtx = nullptr;
        reinterpret_cast<RdModelAnimations*>(modelData)->mpAnmTevReg = nullptr;
    }

    // HD: no blob shadow (dComIfGd_setShadow removed)
    dSnap_RegistFig(DSNAP_TYPE_RD, this, 1.0f, 1.0f, 1.0f);
    return true;
}

/* 0245D088 */
bool daRd_c::isLinkControl() {
    fopAc_ac_c* p0 = dComIfGp_getPlayer(0);
    return p0 != dComIfGp_getLinkPlayer();
}

/* 0245B234 */
void daRd_c::createInit() {
    mStts.Init(0xFF, 0, this);
    mCyl.Set(rd_cylinderSource());
    mCyl.SetStts(&mStts);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mAcch.SetRoofNone();
    J3DModelData* modelData = rd_modelData(mpMorf->getModel());
    mJntCtrl.mHeadJntNum = RD_JNT_REE_KUBI_1_e;
    mJntCtrl.mBackboneJntNum = RD_JNT_REE_MUNE_1_e;
    rd_jointNode(modelData, RD_JNT_REE_KUBI_1_e)->setCallBack(nodeControl_CB);
    rd_jointNode(modelData, RD_JNT_REE_MUNE_1_e)->setCallBack(nodeControl_CB);
    setBtkAnm(2);

    if (mChecksSwitch == 0) {
        modeProcInit(MODE_SW_WAIT);
        gabi::Local<cXyz> offset;
        offset->x = 0.0f;
        offset->y = 40.0f;
        offset->z = 10.0f;
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        PSMTXMultVec(mtx_now(), offset, &current.pos);
    } else {
        modeProcInit(MODE_WAIT);
        switch ((u32)mWhichIdleAnm) {
        case 0:
            setAnm(AnmPrm_TACHIP1, false);
            break;
        case 1:
            setAnm(AnmPrm_SUWARIP, false);
            break;
        }
    }

    createInit_tail(this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -100.0f, -10.0f, -100.0f, 100.0f, 250.0f, 150.0f);

    mD3C = 1;
    mD40 = 1;
    stealItemLeft = 5;

    mEnemyFire.mpMcaMorf = mpMorf;
    mEnemyFire.mpActor = this;
    static s8 fire_j[10] = {
        RD_JNT_REE_ATAMA_1_e,
        RD_JNT_REE_KOSI_1_e,
        RD_JNT_REE_UDE_L1_e,
        RD_JNT_REE_TEKUBI_L1_e,
        RD_JNT_REE_UDE_R1_e,
        RD_JNT_REE_TEKUBI_R1_e,
        RD_JNT_REE_ASI_L1_e,
        RD_JNT_REE_ASI_L3_e,
        RD_JNT_REE_ASI_R1_e,
        RD_JNT_REE_ASI_R3_e,
    };
    static f32 fire_sc[10] = {
        2.0f,
        2.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
    };
    for (int i = 0; i < 10; i++) {
        mEnemyFire.mFlameJntIdxs[i] = fire_j[i];                                        /* fire_j */
        *reinterpret_cast<be<u32>*>(&mEnemyFire.mParticleScale[i]) = *reinterpret_cast<be<u32>*>(&fire_sc[i]); /* fire_sc */
    }

    mEnemyIce.mpActor = this;
    mEnemyIce.m00C = 1;
    mEnemyIce.mWallRadius = 50.0f;
    mEnemyIce.mCylHeight = 250.0f;

    max_health = (s8)l_HIO().m46;
    health = max_health;
    mSpawnPos.copy(current.pos);
    mSpawnAngle = shape_angle.y;
    gravity = -4.5f;

    if (mWhichIdleAnm == 0) {
        itemTableIdx = dComIfGp_CharTbl_GetNameIndex("Rdead1" /* "Rdead1" */, 0);
    }
    if (mWhichIdleAnm == 1) {
        itemTableIdx = dComIfGp_CharTbl_GetNameIndex("Rdead2" /* "Rdead2" */, 0);
    }
}

/* 0245AB98 */
void daRd_c::getArg() {
    u32 param = fopAcM_GetParam(this);
    mWhichIdleAnm = param & 1;
    u8 radiusParam = (param >> 1) & 0x7F;
    int areaRadius = radiusParam;
    mChecksSwitch = (param >> 8) & 0xFF;
    mSwNo = (param >> 0x18) & 0xFF;
    if (areaRadius == 0x7F) {
        areaRadius = 0;
    }
    mAreaRadius = gabi::ftoi(l_HIO().m30 + (f32)areaRadius);
}

/* 0245B6FC */
cPhs_State daRd_c::_create() {
    /* fopAcM_ct(this, daRd_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daRd_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, m_arc_name);
    if (phase_state == cPhs_COMPLEATE_e) {
        getArg();
        if (!fopAcM_entrySolidHeap(this, createHeap_CB, m_heapsize)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return phase_state;
}

/* 0245B7BC */
bool daRd_c::_delete() {
    dComIfG_resDelete(&mPhs, m_arc_name);
    enemy_fire_remove(&mEnemyFire);
    if (heap) {
        mpMorf->stopZelAnime();
    }
    return true;
}

/* 0245B7B8 */
static cPhs_State daRdCreate(void* i_this) {
    return ((daRd_c*)i_this)->_create();
}

/* 0245B814 */
static BOOL daRdDelete(void* i_this) {
    return ((daRd_c*)i_this)->_delete();
}

/* 0245C498 */
static BOOL daRdExecute(void* i_this) {
    return ((daRd_c*)i_this)->_execute();
}

/* function-local static GXColor initialised on first use (guard, copy of 4 bytes from .rodata) */

/* 0245C728 */
static BOOL daRdDraw(void* i_this) {
    return ((daRd_c*)i_this)->_draw();
}

/* 0245F248 */
static BOOL daRdIsDelete(void* i_this) {
    return TRUE;
}

/* dBgS_ObjAcch destructor (inline): this TU's vtables back, then dBgS_Acch::~dBgS_Acch */

// HD: compiler-generated and template code of this translation unit.

/* 0245A984 */
// HD: enemyfire::enemyfire (compiler-generated; this TU's copy)
static enemyfire_l* enemyfire_ct(enemyfire_l* p) {
    if (p == nullptr) {
        p = (enemyfire_l*)operator_new(0x22C);
        if (p == nullptr) return p;
    }
    if (gabi::ea(p) + 0x8C == 0)
        operator_new(0xC);
    dCcD_Stts_ct(&p->mStts);
    RdHD::constructSphere(&p->mSph); /* dCcD_Sph::dCcD_Sph */
    p->m228 = 1.0f;
    return p;
}

/* 0245AA10 */
// HD: daRd_c::daRd_c (compiler-generated)
static daRd_c* daRd_c_ct(daRd_c* p) {
    if (p == nullptr) {
        p = (daRd_c*)operator_new(0xF1C);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = RD_VTBL;
    mDoExt_btkAnm::ct(&p->mBtkAnm);
    RdHD::constructBrk(&p->mBrkAnm); /* mDoExt_brkAnm::mDoExt_brkAnm */
    dBgS_ObjAcch_ct(&p->mAcch, RD_OBJACCH_VT);
    dBgS_AcchCir_ct(&p->mAcchCir);
    dCcD_Stts_ct(&p->mStts);
    dCcD_Cyl_ct(&p->mCyl, RD_AAB_VTBL);
    RdHD::constructInvisibleModel(p->mInvisModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    RdHD::constructJointControl(&p->mJntCtrl);   /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, RD_AAB_VTBL);
    dBgS_AcchCir_ct(&p->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&p->mEnemyIce.mBgAcch, RD_OBJACCH_VT);
    enemyfire_ct(&p->mEnemyFire);
    return p;
}

/* 0245F194 */
// HD: static initialisation (header statics, then l_HIO)
static void __sinit_d_a_rd_cpp() {
    sinit_header_statics(0x1046D66C, 0x101CF634);
    daRd_HIO_c_ct(&l_HIO());
}

/* 0245F234 */
// HD: sead::SafeString deleting destructor (this TU's vtable 0x10038E8C, slot 0xC)
static void rd_SafeString_dtor(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0245F258 */
// HD: daRd_c deleting destructor (compiler-generated; HD virtual destructor)
static void daRd_c_dt(daRd_c* self, s32 flags) {
    if (self == nullptr) return;
    RdHD::destroySphere(&self->mEnemyFire.mSph); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    rd_objacch_dt(&self->mEnemyIce.mBgAcch);
    RdHD::destroyCircle(&self->mEnemyIce.mBgAcchCir.m_cir); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    RdHD::destroyInvisibleModel(self->mInvisModel); /* mDoExt_invisibleModel::~mDoExt_invisibleModel */
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    RdHD::destroyCircle(&self->mAcchCir.m_cir);
    rd_objacch_dt(&self->mAcch);
    RdHD::destroyActor(self); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}

/* 0245F360 */
// HD: empty virtual (sead::SafeString::assureTerminationImpl_, this TU's vtable + 0x14)
static void rd_SafeString_assureTermination(void*) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1003937C "Rd"
str 10039284 "Rdead1" in 0245B234
str 1003928C "Rdead2" in 0245B234
str 100392A4 "DEFAULT_RD_CRY" in 0245BD98
str 100392BC "DEFAULT_RD_ATTACK" in 0245BF50
str 100392D8 "DEFAULT_RD_CRY" in 0245DC9C
str 100392E8 "DEFAULT_RD_CRY" in 0245DD80
str 100392F8 "DEFAULT_RD_CRY" in 0245E158
str 10039328 "DEFAULT_RD_CRY" in 0245E30C
str 10039314 "DEFAULT_RD_ATTACK" in 0245E30C
str 10039338 "DEFAULT_RD_ATTACK" in 0245E964
data l_offsetAttPos 1046D688
data l_offsetEyePos 1046D694
data attPosGuard 1046D748
data eyePosGuard 1046D74C
data mode_tbl 10038FA8
data btk_anm_idx_tbl 10038F58
data btk_anm_prm_tbl 10038F68
data brk_anm_idx_tbl 10039100
data a_play_mod_tbl 10039110
data a_anm_bcks_tbl 10039138
data a_anm_prm_tbl 10039170
data fire_j 101CF628
data fire_sc 101CF600
data search_data 101CF390
data sebone_cyl_offset 101CF528
data muneA_cyl_offset 101CF540
data muneB1_cyl_offset 101CF558
data muneB2_cyl_offset 101CF570
data muneB3_cyl_offset 101CF588
data kosi1_cyl_offset 101CF498
data kosi2_cyl_offset 101CF4B0
data kosi3_cyl_offset 101CF4C8
data asi1_cyl_offset 101CF4E0
data asi2_cyl_offset 101CF4F8
data asi3_cyl_offset 101CF510
data ude1_cyl_offset 101CF5A0
data ude2_cyl_offset 101CF5B8
data te_cyl_offset 101CF5D0
data yubi_cyl_offset 101CF5E8
str 10038F18 "d_a_rd.cpp" in 0245A72C
str 10038F24 "modelData != (0)" in 0245A72C
str 10038F38 "btk != 0" in 0245A72C
str 10038F44 "brk != 0" in 0245A72C
str 10038F90 "d_a_rd.cpp" in 0245AC14
str 10038F9C "btk != (0)" in 0245AC14
str 10039120 "d_a_rd.cpp" in 0245AF80
str 1003912C "brk != (0)" in 0245AF80
*/
