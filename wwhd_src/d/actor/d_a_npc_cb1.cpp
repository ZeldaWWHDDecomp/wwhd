/**
 * d_a_npc_cb1.cpp (WWHD)
 * NPC - Makar (Korok cellist): create, heap, callbacks, init, draw, destructor, HIO and statics
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_cb1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_cb1_create.h"

// HD: these aliases bind the original guest storage shared by all Makar parts.
static cb1_create::daNpc_Cb1_HIO_c cbMain_HIO;
static CbCreateHeaderStatics l_headerStatics;
static CbCreateGlobalRecords l_globalRecords;
static u8 m_playerRoom;
static u8 m_flying;
static s16 m_flyingTimer;
static u16 m_status;
static ProcFunc_l l_waitNpcAction;
static ProcFunc_l l_shipNpcAction;
static ProcFunc_l l_musicNpcAction;
static CbCreateEventNames l_eventNameTbl;
static cXyz l_offsetAttPos;
static cXyz l_offsetEyePos;
static cXyz l_nutOffset;
static cXyz l_nutBase;
static dCcD_SrcCyl l_cyl_src;
static dCcD_SrcCyl l_wind_cyl_src;
static u32 l_partner;
static u32 l_currentModel;
static Mtx34 l_currentMtx;
static u32 l_attGuard;
static u32 l_eyeGuard;
static u32 l_nutOffsetGuard;
static u32 l_nutBaseGuard;
static CbCreateRoomStatuses l_roomStatus;
#define CB1_SAFESTRING_VTBL cb1_create::SafeStringVtable

/* inlined into init and setBaseMtx */
static inline cb1_create::daNpc_Cb1_HIO_c &cb1_l_HIO() { return cbMain_HIO; }

// Private bindings preserve this TU’s HD calling conventions.
/* daPy_getPlayerLinkActorClass(): play + 0x5B34 */
static inline u32 daPy_getPlayerLinkActorClass_ea() { return gabi::ea(dComIfGp_getLinkPlayer()); }
/* daPy_lk_c::checkCarryActionNow() (inline): mCurProc (+0x65F0) is one of the carry procs */
static inline BOOL daPy_lk_checkCarryActionNow(u32 link) {
    u32 proc = gabi::at<CbCreateLinkView>(link)->mCurProc;
    return proc == cb1_create::daPyProc_GRAB_PUT_e || proc == cb1_create::daPyProc_GRAB_UP_e ||
           proc == cb1_create::daPyProc_GRAB_THROW_e;
}
/* J3DModel::getAnmMtx(jnt): the joint matrices are marked dirty (kamome) */
static inline Mtx34 *cb1_getAnmMtx(J3DModel *model, s32 jntNo) { return model->getAnmMtx(jntNo); }
/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag, fpcM_DrawPriority(this)) */
static inline void fopAcM_onDraw(fopAc_ac_c *a) {
    s32 prio = CbCreate_drawPriority(a);
    CbCreate_enqueueDraw(gabi::ea(&a->draw_tag), prio);
}
static inline void cLib_offsetPos(cXyz *out, const cXyz *base, s16 ang, const cXyz *ofs) {
    CbCreate_offsetPos(out, base, ang, ofs);
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== */
/* inlined into create */
static inline bool cb1_isStartStage(const char *lit) {
    gabi::Local<SafeString> sa;
    sa->mStringTop = gabi::guest_str(lit);
    sa->__vtbl = CB1_SAFESTRING_VTBL;
    u32 stage = gabi::ea(dComIfGp_getStartStageName());
    gabi::Local<SafeString> sb;
    sb->mStringTop = stage;
    sb->__vtbl = CB1_SAFESTRING_VTBL;
    gabi::call_ptr((u32)gabi::at<CbCreateSafeStringVtable>(sa->__vtbl)->mAssureTermination,
                   sa.get());
    gabi::call_ptr((u32)gabi::at<CbCreateSafeStringVtable>(sa->__vtbl)->mAssureTermination,
                   sa.get());
    u32 pa = sa->mStringTop;
    gabi::call_ptr((u32)gabi::at<CbCreateSafeStringVtable>(sb->__vtbl)->mAssureTermination,
                   sb.get());
    u32 pb = sb->mStringTop;
    if (pa == pb) {
        return true;
    }
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = (u8)*gabi::at<be<u8>>(pa + i);
        if (ca != (u8)*gabi::at<be<u8>>(pb + i)) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* mDoExt_bckAnm::mDoExt_bckAnm (inline; HD 0x8C) */
/* inlined into create */
static inline void cb1_bckAnm_ct(u32 b) {
    CbCreateBckView *animation = gabi::at<CbCreateBckView>(b);
    CbCreate_initFrameCtrl(gabi::ea(&animation->mFrameCtrl), 0);
    animation->mVtable = cb1_create::BaseAnmVtable;
    CbCreate_constructMtxCalc(gabi::ea(&animation->mMtxCalc));
    animation->mConstructorState[2] = 0;
    animation->mConstructorState[3] = 0;
    animation->mConstructorState[1] = 0;
    animation->mpAnm = 0;
    animation->mVtable = cb1_create::BckAnmVtable;
    animation->mConstructorState[0] = 0;
    animation->mMtxCalcVtable = cb1_create::MtxCalcVtable;
}
// HD: this TU accesses the shared current model and matrix storage.
static inline J3DModel *j3dSys_getModel() { return gabi::at<J3DModel>((u32)l_currentModel); }
static inline Mtx34 *J3DSys_mCurrentMtx() { return &l_currentMtx; }
static inline void PSMTXConcat(const Mtx34 *a, const Mtx34 *b, Mtx34 *ab) {
    CbCreate_concatMtx(a, b, ab);
}
/* J3DModel::setAnmMtx(jnt, m) */
static inline void cb1_setAnmMtx(J3DModel *model, s32 jntNo, const Mtx34 *m) {
    // HD: preserve the interleaved word copy used by this entry.
    mtx_copy(model->getAnmMtx(jntNo), m);
}
static inline bool cM3d_IsZero(f32 f) { return __builtin_fabsf(f) < 3.8146973e-06f; }
/* inlined into nutNodeCallBack */
static inline void cb1_copyWords(u32 dst, u32 src, int n) {
    for (int i = 0; i < n; i++) {
        *gabi::at<be<u32>>(dst + 4 * (u32)i) = (u32)*gabi::at<be<u32>>(src + 4 * (u32)i);
    }
}
/* inlined into createHeap */
static inline J3DModelData *cb1_getObjectRes(s32 idx) {
    return (J3DModelData *)dComIfG_getObjectRes("Cb", idx, CB1_SAFESTRING_VTBL);
}
/* modelData->getJointName()->getIndex(name) */
static inline s8 cb1_getJointIndex(J3DModelData *d, const char *name) {
    return (s8)d->getJointName()->getIndex(name);
}
/* HD: getJointNodePointer(idx) is bounds-checked (count +4, array +8, 0x1C each); setCallBack at +8
 */
static inline void cb1_setJointCallBack(J3DModelData *d, u16 idx, J3DJointCallBack cb) {
    d->getJointNodePointer(idx)->setCallBack(cb);
}
#define CB1_ASSERT_FILE "d_a_npc_cb1.cpp"
#define CB1_ASSERT_MODELDATA "modelData != (0)" /* "modelData != 0" */
/* GHS pointer-to-member compare with a constant {d = 0, i = -1, f} */
static inline BOOL cb1_pmf_eq(ProcFunc_l &cur, u32 f) {
    return cur.i == -1 && (cur.i == 0 || (cur.d == 0 && cur.f == f));
}

static s16 daNpc_Cb1_XyCheckCB(void *, int);
static s16 daNpc_Cb1_XyEventCB(daNpc_Cb1_c *, int);
static void cb1_AcchCir_dtor(dBgS_AcchCir *, s32);

/* 02226B9C */
static cb1_create::daNpc_Cb1_HIO_c *daNpc_Cb1_HIO_c_ct(cb1_create::daNpc_Cb1_HIO_c *i_this) {

    if (i_this == nullptr) {
        i_this = (cb1_create::daNpc_Cb1_HIO_c *)operator_new(0xF0);
        if (i_this == nullptr) {
            return nullptr;
        }
    }
    i_this->vtable = cb1_create::HioVtable;
    CbCreate_constructNpcHio(gabi::ea(&i_this->mNpc)); /* dNpc_HIO_c::dNpc_HIO_c */
    i_this->mNpc.m04 = -20.0f;
    i_this->mNpc.mMaxHeadX = 0;
    i_this->mNpc.mMaxHeadY = 0;
    i_this->mNpc.mMaxBackboneX = 0xBB8;
    i_this->mNpc.mMaxBackboneY = 0x1770;
    i_this->mNpc.mMinHeadX = 0;
    i_this->mNpc.mMinHeadY = 0;
    i_this->mNpc.mMinBackboneX = -0x3E8;
    i_this->mNpc.mMinBackboneY = -0x1770;
    i_this->mNpc.mMaxTurnStep = 0x7D0;
    i_this->mNpc.mMaxHeadTurnVel = 0x7D0;
    i_this->mNpc.mAttnYOffset = 65.0f;
    i_this->mNpc.mMaxAttnAngleY = 0x4000;
    i_this->mNpc.m22 = 0;
    i_this->mNpc.mMaxAttnDistXZ = 180.0f;
    i_this->field_0x30 = 50.0f;
    i_this->mPlayerChaseDistance = 140.0f;
    i_this->mChaseDistScale = 0.05f;
    i_this->mMaxWalkSpeed = 9.0f;
    i_this->mMinWalkSpeed = 3.0f;
    i_this->mForwardAccel = 0.4f;
    i_this->field_0xE8 = 0x8FC;
    i_this->field_0xEA = 0x320;
    i_this->field_0xEC = 0x3;
    i_this->mDecelScale = 0.9f;
    i_this->mMaxDecel = 1.0f;
    i_this->mDecel = 1.0f;
    i_this->mWalkAnmSpeedScale = 0.45f;
    i_this->mMaxWalkAnmSpeed = 0.9f;
    i_this->mNpcFlyLaunchSpeedF = 50.0f;
    i_this->mNpcFlyLaunchSpeedY = 20.0f;
    i_this->field_0x64 = 8.0f;
    i_this->mHitSpeedScaleF = -1.0f;
    i_this->mHitSpeedScaleY = 3.5f;
    i_this->field_0x70 = -4.0f;
    i_this->field_0x74 = -1.5f;
    i_this->field_0x78 = -6.0f;
    i_this->field_0x7C = -7.6f;
    i_this->field_0x80 = 0.1f;
    i_this->field_0x84 = 0.2f;
    i_this->mStickWalkSpeedScale = 10.0f;
    i_this->field_0xC8 = -0x1000;
    i_this->mPlayerFlyTimer = 450; /* mPlayerFlyTimer */
    i_this->field_0xCC = 0x64;
    i_this->field_0xCE = 0xF;
    i_this->field_0xD0 = 0x4E20;
    i_this->field_0xD2 = 0x190;
    i_this->field_0xD4 = 0x9C4;
    i_this->field_0xD6 = 0xC8;
    i_this->field_0x8C = 0.0002f;
    i_this->field_0x90 = -15.0f;
    i_this->field_0x98 = 10.0f;
    i_this->field_0x9C = 10.0f;
    i_this->field_0xA0 = -2.5f;
    i_this->field_0xD8 = 1;
    i_this->field_0xA4 = 100.0f;
    i_this->field_0xA8 = 6000.0f;
    i_this->field_0xE0 = 0xC8;
    i_this->mStickFlySpeedScale = 0.5f; /* mStickFlySpeedScale */
    i_this->field_0xDA = 0x2968;
    i_this->field_0xB0 = 34.0f;
    i_this->field_0xB4 = 2.0f;
    i_this->mNpcFlyTimer = 60;
    i_this->field_0xDE = 0xD;
    i_this->field_0xB8 = 2.0f;
    i_this->mFlyLaunchSpeedY = 50.0f;
    i_this->field_0xC0 = 400.0f;
    i_this->field_0xE2 = 0x5;
    i_this->field_0xE4 = 0x14;
    i_this->field_0xE6 = 0xBB8;
    i_this->field_0xC4 = 20.0f;
    i_this->mDamageTimer = 60;
    i_this->field_0xEF = 0;
    i_this->mNo = -1;
    return i_this;
}

/* 02221894 */
BOOL daNpc_Cb1_c::isTagCheckOK() {

    if (cb1_pmf_eq(mNpcAction, cb1_create::WaitNpcAction /* waitNpcAction */) ||
        cb1_pmf_eq(mNpcAction, cb1_create::SearchNpcAction /* searchNpcAction */)) {
        return TRUE;
    }
    if (cb1_pmf_eq(mNpcAction, cb1_create::CarryNpcAction /* carryNpcAction */) &&
        !daPy_lk_checkCarryActionNow(daPy_getPlayerLinkActorClass_ea())) {
        return TRUE;
    }
    return FALSE;
}

/* 0221EC98 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {

    return CbCreate_createHeap(i_this); /* createHeap() */
}

/* 0221E08C */
static BOOL nodeCallBack(J3DNode *node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel *model = j3dSys_getModel();
        daNpc_Cb1_c *a_this = static_cast<daNpc_Cb1_c *>(model->getUserArea()); /* getUserArea() */
        if (a_this != nullptr) {
            /* static cXyz l_offsetAttPos(0, 0, 0), l_offsetEyePos(20, 10, 0) */
            if ((u32)l_attGuard == 0) {
                l_attGuard = 1;
                l_offsetAttPos.x = 0.0f;
                l_offsetAttPos.z = 0.0f;
                l_offsetAttPos.y = 0.0f;
            }
            if ((u32)l_eyeGuard == 0) {
                l_eyeGuard = 1;
                l_offsetEyePos.x = 20.0f;
                l_offsetEyePos.z = 0.0f;
                l_offsetEyePos.y = 10.0f;
            }
            s32 jnt_no = (u16) reinterpret_cast<CbCreateJointData *>(J3DNode_toJoint(node))
                             ->mJointNo; /* getJntNo() */
            PSMTXCopy(cb1_getAnmMtx(model, jnt_no), mDoMtx_stack_c::get());
            mDoMtx_XrotM(mDoMtx_stack_c::get(),
                         a_this->mJntCtrl.mAngles[1][1]); /* getBackbone_y() */
            mDoMtx_ZrotM(mDoMtx_stack_c::get(),
                         (s16)-a_this->mJntCtrl.mAngles[1][0]); /* -getBackbone_x() */
            PSMTXMultVec(mDoMtx_stack_c::get(), &l_offsetEyePos, &a_this->mEyePos);
            if (a_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                a_this->mAttnSetCount = a_this->mAttnSetCount + 1;
            }
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
            cb1_setAnmMtx(model, jnt_no, mDoMtx_stack_c::get());
        }
    }
    return TRUE;
}

/* 0221E23C */
static BOOL nutNodeCallBack(J3DNode *node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel *model = j3dSys_getModel();
        daNpc_Cb1_c *a_this = static_cast<daNpc_Cb1_c *>(model->getUserArea()); /* getUserArea() */
        if (a_this != nullptr) {
            /* static cXyz l_nutOffset(0, 0, 0), l_nutBase(1, 0, 0) */
            if ((u32)l_nutOffsetGuard == 0) {
                l_nutOffsetGuard = 1;
                l_nutOffset.x = 0.0f;
                l_nutOffset.z = 0.0f;
                l_nutOffset.y = 0.0f;
            }
            if ((u32)l_nutBaseGuard == 0) {
                l_nutBase.y = 0.0f;
                l_nutBaseGuard = 1;
                l_nutBase.x = 1.0f;
                l_nutBase.z = 0.0f;
            }
            auto *joint = reinterpret_cast<CbCreateJointData *>(J3DNode_toJoint(node));
            /* temp2 = the joint's translation: three getAnmMtx(jnt_no) calls, each marking the
             * matrices dirty; GHS interleaves the loads with the flag stores */
            J3DMtxBuffer_hd *blk1 = model->mpMtxBuffer;
            u16 f1 = (u16)blk1->mFlags;
            s32 jnt_no = (u16)joint->mJointNo; /* getJntNo() */
            blk1->mFlags = (u16)(f1 | 0x10);
            u32 m1 = (u32)blk1->mpAnmMtx;
            J3DMtxBuffer_hd *blk2 = model->mpMtxBuffer;
            u16 f2 = (u16)blk2->mFlags;
            u32 x = (u32)gabi::at<CbCreateWordMatrix>(m1 + (u32)jnt_no * sizeof(Mtx34))->m[0][3];
            blk2->mFlags = (u16)(f2 | 0x10);
            J3DMtxBuffer_hd *blk3 = model->mpMtxBuffer;
            u32 m2 = (u32)blk2->mpAnmMtx;
            u16 f3 = (u16)blk3->mFlags;
            u32 m3 = (u32)blk3->mpAnmMtx;
            u32 y = (u32)gabi::at<CbCreateWordMatrix>(m2 + (u32)jnt_no * sizeof(Mtx34))->m[1][3];
            blk3->mFlags = (u16)(f3 | 0x10);
            u32 z = (u32)gabi::at<CbCreateWordMatrix>(m3 + (u32)jnt_no * sizeof(Mtx34))->m[2][3];

            gabi::Local<cXyz> temp2;
            /* lfs/stfs pairs with no arithmetic: the recompiled original keeps the bits */
            auto *t2 = reinterpret_cast<CbCreateWordVector *>(temp2.get());
            t2->x = x;
            t2->z = z;
            t2->y = y;
            PSVECAdd(&a_this->mNutPos, &a_this->mNusSpeed,
                     &a_this->mNutPos); /* getNutPos() += getNusSpeed() */
            gabi::Local<cXyz> res;
            cXyz_mi(&a_this->mNutPos, res.get(), temp2.get());
            gabi::Local<cXyz> temp;
            cb1_copyWords(gabi::ea(temp.get()), gabi::ea(res.get()), 3);

            f32 temp4 = std_sqrtf(PSVECSquareMag(temp.get())); /* temp.abs() */
            if (!cM3d_IsZero(temp4)) {
                PSVECScale(temp.get(), temp.get(), 14.0f / temp4);
            }
            cXyz_pl(temp2.get(), res.get(), temp.get());
            cb1_copyWords(gabi::ea(&a_this->mNutPos), gabi::ea(res.get()),
                          3); /* getNutPos() = temp2 + temp */

            cXyz_ml(temp.get(), res.get(), -0.15f);
            PSVECAdd(&a_this->mNusSpeed, res.get(),
                     &a_this->mNusSpeed); /* getNusSpeed() += temp * -0.15f */
            a_this->mNusSpeed.y = a_this->mNusSpeed.y + -1.3f;
            f32 temp5 = std_sqrtf(PSVECSquareMag(&a_this->mNusSpeed));
            if (!cM3d_IsZero(temp5)) {
                PSVECScale(&a_this->mNusSpeed, &a_this->mNusSpeed, 5.8f / temp5);
            }

            gabi::Local<Mtx34> temp3;
            CbCreate_upMtxBase(&l_nutBase, temp.get(), temp3.get()); /* cM3d_UpMtx_Base */
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    /* lfs/stfs pair with no arithmetic: the recompiled original keeps the bits */
                    auto *anm =
                        reinterpret_cast<CbCreateWordMatrix *>(cb1_getAnmMtx(model, jnt_no));
                    anm->m[i][j] =
                        (u32) reinterpret_cast<CbCreateWordMatrix *>(temp3.get())->m[i][j];
                }
            }
            PSMTXCopy(cb1_getAnmMtx(model, jnt_no), J3DSys_mCurrentMtx());
        }
    }
    return TRUE;
}

/* 0221E5BC */
static BOOL ppNodeCallBack(J3DNode *node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel *model = j3dSys_getModel();
        daNpc_Cb1_c *a_this = static_cast<daNpc_Cb1_c *>(model->getUserArea()); /* getUserArea() */
        if (a_this != nullptr) {
            J3DJoint *joint = J3DNode_toJoint(node);
            s16 work3 = a_this->m8FA; /* getWork3() */
            s32 jnt_no = (u16) reinterpret_cast<CbCreateJointData *>(joint)->mJointNo;
            mDoMtx_YrotS(mDoMtx_stack_c::get(), work3);
            PSMTXConcat(cb1_getAnmMtx(model, jnt_no), mDoMtx_stack_c::get(),
                        mDoMtx_stack_c::get()); /* revConcat */
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
            cb1_setAnmMtx(model, jnt_no, mDoMtx_stack_c::get());
        }
    }
    return TRUE;
}

// HD: the face model resource has no assertion.
/* 0221E6E4 */
BOOL daNpc_Cb1_c::createHeap() {

    J3DModelData *modelData = cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BDL_CB_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3D3, CB1_ASSERT_MODELDATA);
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x00080000, 0x11000022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    m_backbone_jnt_num = cb1_getJointIndex(modelData, "backbone");
    if (m_backbone_jnt_num < 0) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3E0,
                        "m_backbone_jnt_num >= 0" /* "m_backbone_jnt_num >= 0" */);
    }
    m_armRend_jnt_num = cb1_getJointIndex(modelData, "armRend");
    if (m_armRend_jnt_num < 0) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3E3,
                        "m_armRend_jnt_num >= 0" /* "m_armRend_jnt_num >= 0" */);
    }
    m_armL2_jnt_num = cb1_getJointIndex(modelData, "armL2");
    if (m_armL2_jnt_num < 0) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x3E6,
                        "m_armL2_jnt_num >= 0" /* "m_armL2_jnt_num >= 0" */);
    }
    cb1_setJointCallBack(modelData, (u16)m_backbone_jnt_num, nodeCallBack);
    mpMorf->getModel()->setUserArea(reinterpret_cast<uintptr_t>(this)); /* setUserArea(this) */

    mpFaceModel = mDoExt_J3DModel__create(cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BDL_CB_FACE_e),
                                          0x00080000, 0x11000022);
    if (mpFaceModel.get() == nullptr) {
        return FALSE;
    }

    if ((u16)m_status & daCbStts_MUSIC) {
        modelData = cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BDL_CB_STICK_e);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x403, CB1_ASSERT_MODELDATA);
        }
        mpStickModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
        if (mpStickModel.get() == nullptr) {
            return FALSE;
        }
        m_nut_jnt_num = cb1_getJointIndex(modelData, "nut");
        if (m_nut_jnt_num < 0) {
            JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x40E,
                            "m_nut_jnt_num >= 0" /* "m_nut_jnt_num >= 0" */);
        }
        cb1_setJointCallBack(modelData, (u16)m_nut_jnt_num, nutNodeCallBack);
        mpStickModel->setUserArea(reinterpret_cast<uintptr_t>(this));

        modelData = cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BDL_CB_CELLO_e);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x41A, CB1_ASSERT_MODELDATA);
        }
        mpCelloModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
        if (mpCelloModel.get() == nullptr) {
            return FALSE;
        }
    }

    modelData = cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BDL_PP_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x426, CB1_ASSERT_MODELDATA);
    }
    mpPropellerModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    if (mpPropellerModel.get() == nullptr) {
        return FALSE;
    }
    s8 center = cb1_getJointIndex(modelData, "center");
    m_center_jnt_num = center;
    cb1_setJointCallBack(modelData, (u16)center, ppNodeCallBack);
    mpPropellerModel->setUserArea(reinterpret_cast<uintptr_t>(this));
    if (!mPropellerBckAnim.init(
            modelData, (J3DAnmTransform *)cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BCK_M_OPEN_e),
            false, 0, 1.0f, 0, -1, false)) {
        return FALSE;
    }

    modelData = cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BDL_CB_NUT_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(CB1_ASSERT_FILE, 0x443, CB1_ASSERT_MODELDATA);
    }
    mpNutModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    if (mpNutModel.get() == nullptr) {
        return FALSE;
    }
    if (!mNutBckAnim.init(
            modelData, (J3DAnmTransform *)cb1_getObjectRes(cb1_create::dRes_INDEX_CB_BCK_NUT_SOW_e),
            true, 0, 1.0f, 0, -1, false)) {
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

/* 0221F140 */
BOOL daNpc_Cb1_c::setAction(ProcFunc_l *cur, ProcFunc_l *newFunc, void *arg) {

    m_flying = 0;      /* offFlying() */
    m_flyingTimer = 0; /* setFlyingTimer(0) */
    s16 newI = newFunc->i;
    s16 newD;
    u32 newF;
    bool callPrevious = true;
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
        callPrevious = cur->i != 0;
    }
    if (callPrevious) {
        m8F0 = -1;
        md_pmf_call<BOOL>(this, cur, arg);
    }
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
    attention_info.flags = 0; /* attention_info.flags */
    md_pmf_call<BOOL>(this, cur, arg);
    m8F0 = m8F0 + 1;
    return TRUE;
}

/* 0221F2C4 */
void daNpc_Cb1_c::setNpcAction(ProcFunc_l *actionFunc, void *arg) {

    mPlayerAction.d = 0;
    mPlayerAction.i = 0;
    mPlayerAction.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mNpcAction, fn, arg);
}

/* 0221F314 */
BOOL daNpc_Cb1_c::shipRideCheck() {

    if (dComIfGs_isEventBit(0x1604) &&
        gabi::ea(dComIfGp_getShipActor()) /* dComIfGp_getShipActor() */ != 0) {
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, gabi::ea(&l_shipNpcAction));
        setNpcAction(fn, nullptr);
        m_status = (u16)((u16)m_status | daCbStts_SHIP_RIDE); /* onShipRide() */
        fopAcM_offDraw(this);
        return TRUE;
    }
    return FALSE;
}

/* 0221ED08 */
void daNpc_Cb1_c::setBaseMtx() {

    J3DModel *pModel = mpMorf->getModel();
    if ((u16)m_status & daCbStts_SHIP_RIDE) {
        u32 ship = gabi::ea(dComIfGp_getShipActor()); /* dComIfGp_getShipActor() */
        if (ship != 0) {
            /* pShip->getHeadJntMtx(): joint 8 of the ship's model */
            J3DModel *shipModel = gabi::at<CbCreateShipView>(ship)->mpHeadAnm->getModel();
            PSMTXCopy(cb1_getAnmMtx(shipModel, 8), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(10.92f, 0.57f, -14.0f);
            CbCreate_rotateXYZ(mDoMtx_stack_c::get(), (s16)-0x3AAA, (s16)0,
                               (s16)0x40FE); /* XYZrotM */
            J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
            Mtx34 *pMtx = mDoMtx_stack_c::get();
            current.pos.x = pMtx->m[0][3];
            current.pos.y = pMtx->m[1][3];
            current.pos.z = pMtx->m[2][3];
            shape_angle.y = (s16)((s16)gabi::at<CbCreateShipView>(ship)->shape_angle.y + 0x8000);
            actor_status = actor_status | 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
            tevStr.mRoomNo = gabi::at<CbCreateShipView>(ship)->tevStr.mRoomNo; /* tevStr.mRoomNo */
            tevStr.mEnvrIdxOverride = gabi::at<CbCreateShipView>(ship)
                                          ->tevStr.mEnvrIdxOverride; /* tevStr.mEnvrIdxOverride */
            gabi::at<CbCreateShipView>(ship)->mStateFlag =
                (u32)gabi::at<CbCreateShipView>(ship)->mStateFlag |
                0x40000000; /* pShip->onCb1Ride() */
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
        if (mpNutModel.get() != nullptr && ((u16)m_status & daCbStts_NUT)) {
            J3DModel_setBaseTRMtx(mpNutModel, mDoMtx_stack_c::get());
        }
        if (mpPropellerModel.get() != nullptr) {
            mDoMtx_stack_c::transM(0.0f, 24.8f, 1.5f);
            J3DModel_setBaseTRMtx(mpPropellerModel, mDoMtx_stack_c::get());
            J3DModel_calc(mpPropellerModel);
        }
    }
    cb1_create::daNpc_Cb1_HIO_c &hio = cb1_l_HIO();
    {
        gabi::Local<cXyz> ofs;
        ofs->x = 0.0f;
        ofs->y = hio.mNpc.mAttnYOffset;
        ofs->z = 0.0f;
        cLib_offsetPos(&attention_info.position /* attention_info.position */, &current.pos,
                       shape_angle.y, ofs.get());
    }
    {
        gabi::Local<cXyz> ofs;
        ofs->x = 0.0f;
        ofs->y = hio.field_0x30;
        ofs->z = 0.0f;
        cLib_offsetPos(&eyePos /* eyePos */, &current.pos, shape_angle.y, ofs.get());
    }
}

/* 0221F3C0 */
BOOL daNpc_Cb1_c::init() {

    cb1_create::daNpc_Cb1_HIO_c &hio = cb1_l_HIO();
    gravity = hio.field_0x70;
    m_playerRoom = 0;  /* offPlayerRoom() */
    m_flying = 0;      /* offFlying() */
    m_flyingTimer = 0; /* setFlyingTimer(0) */
    m_status = 0;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB; /* [SPEAK] */
    attention_info.distances[fopAc_Attn_TYPE_CARRY_e] = 8;    /* [CARRY] */

    for (int i = 0; i < 5; i++) {
        mEventIdx[i] =
            dComIfGp_evmng_getEventIdx(gabi::at<const char>((u32)l_eventNameTbl.entries[i]), 0xFF);
    }
    m8E3 = -1;
    m8DD = -1;
    eventInfo.mpCheckCB = gabi::guest_fn(
        (const void *)daNpc_Cb1_XyCheckCB); /* eventInfo.setXyCheckCB(daNpc_Cb1_XyCheckCB) */
    eventInfo.mpEventCB = gabi::guest_fn(
        (const void *)daNpc_Cb1_XyEventCB);  /* eventInfo.setXyEventCB(daNpc_Cb1_XyEventCB) */
    mNoResetFlg1 = (u32)mNoResetFlg1 | 0x40; /* onNpcNotChange() */

    s32 prm = (s32)mParameters; /* fopAcM_GetParam(this) */
    if (prm == 5 || prm == 1 ||
        prm == 2) { /* isTypeKazeBoss() || isTypeForest() || isTypeWaterFall() */
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, gabi::ea(&l_musicNpcAction));
        setNpcAction(fn, nullptr);
        home.roomNo = -1; /* home.roomNo */
        gravity = -0.1f;
        current.roomNo = -1; /* current.roomNo */
    } else if (prm == 0) {   /* isTypeBossDie() */
        gabi::Local<ProcFunc_l> fn;
        fn->d = 0;
        fn->i = -1;
        fn->f = dComIfGs_isStageBossEnemy(4 /* STAGE_FW */)
                    ? cb1_create::WaitNpcAction /* waitNpcAction */
                    : cb1_create::RescueNpcAction /* rescueNpcAction */;
        gabi::Local<ProcFunc_l> arg;
        md_pmf_load(arg, gabi::ea(fn.get()));
        setNpcAction(arg, nullptr);
    } else if (prm != 3 && prm != 4 && shipRideCheck()) { /* !isTypeEkaze() && !isTypeKaze() */
        attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAF;
        attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAF;
    } else {
        if (dComIfGs_isEventBit(0x1610)) {
            mNoResetFlg1 = (u32)mNoResetFlg1 & ~0x40u; /* offNpcNotChange() */
        }
        if (fopAcM_GetParam(this) == 3) {         /* isTypeEkaze() */
            mNoResetFlg1 = (u32)mNoResetFlg1 | 2; /* onNpcCallCommand() */
        }
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, gabi::ea(&l_waitNpcAction));
        setNpcAction(fn, nullptr);
        m_status = (u16)((u16)m_status & ~daCbStts_MUSIC); /* offMusic() */
    }

    mStts.Init(0xFE, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    mWindCyl.Set(&l_wind_cyl_src);
    mWindCyl.SetStts(&mStts);
    return TRUE;
}

// HD: the blob shadow and auxiliary model shadows are absent.
/* 022213BC */
BOOL daNpc_Cb1_c::draw() {

    if ((u16)m_status & daCbStts_SHIP_RIDE) {
        u32 ship = gabi::ea(dComIfGp_getShipActor()); /* dComIfGp_getShipActor() */
        if (ship != 0 && ((u32)gabi::at<CbCreateShipView>(ship)->mStateFlag &
                          0x200000) /* checkHeadNoDraw() */) {
            return TRUE;
        }
    } else if (!(actor_status & 0x2000) /* !fopAcM_checkCarryNow(this) */) {
        s32 homeRoomNo = (s8)home.roomNo;
        if (homeRoomNo < 0) {
            return TRUE;
        }
        dComIfGp_ea();
        if (!((u8)gabi::at<CbCreateRoomStatus>(gabi::ea(&l_roomStatus) +
                                               (u32)homeRoomNo * sizeof(CbCreateRoomStatus))
                  ->mStatus &
              0x10)) { /* dComIfGp_roomControl_checkStatusFlag(room, 0x10) */
            return TRUE;
        }
    }

    J3DModel *pModel = mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    CbCreate_drawDamageFog(this); /* daPy_npc_c::drawDamageFog */
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mpMorf->updateDL();

    J3DModel *pFaceModel = mpFaceModel;
    setLightTevColorType(dKy_getEnvlight(), pFaceModel, &tevStr);
    J3DModel_setBaseTRMtx(pFaceModel, cb1_getAnmMtx(pModel, m_backbone_jnt_num));
    mDoExt_modelUpdateDL(pFaceModel, 0);

    u16 status = (u16)m_status;
    if (status & daCbStts_MUSIC) {
        setLightTevColorType(dKy_getEnvlight(), mpStickModel, &tevStr);
        J3DModel_setBaseTRMtx(mpStickModel, cb1_getAnmMtx(pModel, m_armRend_jnt_num));
        mDoExt_modelUpdateDL(mpStickModel, 0);
        setLightTevColorType(dKy_getEnvlight(), mpCelloModel, &tevStr);
        J3DModel_setBaseTRMtx(mpCelloModel, cb1_getAnmMtx(pModel, m_armL2_jnt_num));
        mDoExt_modelUpdateDL(mpCelloModel, 0);
    } else if (u32 prop = gabi::ea(mpPropellerModel.get());
               prop != 0 && CbCreate_isFlyAction(this) /* isFlyAction() */ && m8DC != 2) {
        f32 frame = 11.0f;
        if (m8DC == 4) {
            frame = mpMorf->getFrame(); /* mpMorf->getFrame() */
        }
        mPropellerBckAnim.entry(gabi::at<J3DModel>(prop)->getModelData(), frame);
        setLightTevColorType(dKy_getEnvlight(), mpPropellerModel, &tevStr);
        mDoExt_modelUpdateDL(mpPropellerModel, 0);
    } else if (mpNutModel.get() != nullptr && (status & daCbStts_NUT)) {
        mNutBckAnim.entry(mpNutModel->getModelData(),
                          mNutBckAnim.getFrame() /* mNutBckAnim frame */);
        setLightTevColorType(dKy_getEnvlight(), mpNutModel, &tevStr);
        mDoExt_modelUpdateDL(mpNutModel, 0);
    }

    dSnap_RegistFig(0x9C /* DSNAP_TYPE_NPC_CB1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

// HD: no HIO child; publish the companion and rewrite the priest record after restart setup.
/* 0221F700 */
cPhs_State daNpc_Cb1_c::create() {

    u32 t = gabi::ea(this);
    /* fopAcM_ct(this, daNpc_Cb1_c): the constructor, inlined */
    if (!(actor_condition & 8)) {
        if (t != 0) {
            CbCreate_constructNpc(this); /* daPy_npc_c::daPy_npc_c */
            __vtbl = CB1_VTBL;
            cb1_bckAnm_ct(gabi::ea(&mPropellerBckAnim)); /* mPropellerBckAnim */
            cb1_bckAnm_ct(gabi::ea(&mNutBckAnim));       /* mNutBckAnim */
            CbCreate_constructArray(
                &mAcchCir, 2, 0x40,
                cb1_create::AcchCirConstructor); /* __construct_array(dBgS_AcchCir) */
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, cb1_create::CylinderVtable);
            dCcD_Cyl_ct(&mWindCyl, cb1_create::CylinderVtable);
            CbCreate_constructJointCtrl(&mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            mPolyInfo.mpBgW = 0;
            mPolyInfo.__vtbl = cb1_create::PolyInfoVtable;
            mPolyInfo.mPolyIndex = 0xFFFF;
            mPolyInfo.mBgIndex = 0x100;
            m8DB = 0xFF;
            mPolyInfo.mProcId = -1;
        }
        actor_condition = actor_condition | 8;
    }

    if (fopAcM_GetParam(this) != 0) { /* !isTypeBossDie() */
        if (dComIfGs_checkGetItem(0x3E /* dItemNo_MASTER_SWORD_3_e */)) {
            if (fopAcM_GetParam(this) != 5) {
                return cPhs_ERROR_e;
            }
        } else if (dComIfGs_isEventBit(0x2910)) {
            if (fopAcM_GetParam(this) != 4) {
                return cPhs_ERROR_e;
            }
        } else if (dComIfGs_isEventBit(0x2E02)) {
            if (fopAcM_GetParam(this) != 3) {
                return cPhs_ERROR_e;
            }
        } else if (dComIfGs_isEventBit(0x1610)) {
            BOOL error = TRUE;
            if (cb1_isStartStage("sea") && dComIfGs_isEventBit(0x1604)) {
                error = FALSE;
            }
            if (error) {
                return cPhs_ERROR_e;
            }
        } else if (dComIfGs_checkGetItem(0x3A /* dItemNo_MASTER_SWORD_2_e */)) {
            if (fopAcM_GetParam(this) != 2) {
                return cPhs_ERROR_e;
            }
        } else if (dComIfGs_isEventBit(0x1820)) {
            return cPhs_ERROR_e;
        } else if (dComIfGs_checkGetItem(0x6B /* dItemNo_PEARL_FARORE_e */)) {
            if (fopAcM_GetParam(this) != 1) {
                return cPhs_ERROR_e;
            }
        } else {
            return cPhs_ERROR_e;
        }
    }

    cPhs_State result = dComIfG_resLoad(&mPhs, "Cb");
    if (result == cPhs_COMPLEATE_e) {
        if (fopAcM_GetParam(this) == 4) { /* isTypeKaze() */
            dSv_info_c *save = dComIfGs_info();
            if (save->mSavedata.mPlayer.mPriest.field_0xf == 1) {
                dSv_player_priest_c *priest = &save->mSavedata.mPlayer.mPriest;
                s16 angle = priest->field_0xc;
                s8 roomNo = priest->field_0xe;
                CbCreate_setRestartOption(gabi::ea(&save->mRestart), 1, gabi::ea(priest), angle,
                                          roomNo);
                // HD: reload the save singleton before rewriting the priest record.
                CbCreate_setPriest(gabi::ea(&dComIfGs_info()->mSavedata.mPlayer.mPriest), 1,
                                   gabi::ea(priest), angle, roomNo);
            }
            CbCreate_checkRestart(this, 1); /* daPy_npc_c::checkRestart */
        }
        if (gabi::ea(dComIfGp_getCb1Player()) != 0) { /* dComIfGp_getCb1Player() */
            return cPhs_ERROR_e;
        }
        if ((u32)(fopAcM_GetParam(this) - 1) <= 4) { /* KazeBoss, Forest, WaterFall, Ekaze, Kaze */
            m_status = (u16)((u16)m_status | daCbStts_MUSIC); /* onMusic() */
        }
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)CheckCreateHeap), 0x9400)) {
            return cPhs_ERROR_e;
        }
        setBaseMtx();
        J3DModel *model = mpMorf->getModel();
        cullMtx = model != nullptr ? gabi::ea(&model->mBaseTransformMtx) : 0;
        if (!init()) {
            return cPhs_ERROR_e;
        }
        fopAcM_setStageLayer(this);
        l_partner = t;
    }
    return result;
}

// HD: no HIO child; clear the shared companion pointer when it refers to this actor.
/* 02221774 */
static void daNpc_Cb1_c_dtor(daNpc_Cb1_c *i_this, s32 flags) {

    if (i_this == nullptr) {
        return;
    }
    u32 t = gabi::ea(i_this);
    i_this->__vtbl = CB1_VTBL;
    dComIfG_resDelete(&i_this->mPhs, "Cb");
    if (i_this->heap.get() != nullptr && i_this->mpMorf.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    m_playerRoom = 0; /* offFlying(), offPlayerRoom() */
    m_flying = 0;
    CbCreate_musicStop(i_this); /* musicStop() */
    if ((u32)l_partner == t) {
        l_partner = 0;
    }
    /* member destructors */
    CbCreate_destroyCylinder(&i_this->mWindCyl,
                             2); /* dCcD_Cyl::~dCcD_Cyl (matcher: dBgS_Acch::~dBgS_Acch) */
    CbCreate_destroyCylinder(&i_this->mCyl, 2);
    CbCreate_destroyStatus(&i_this->mStts, 2); /* dCcD_Stts::~dCcD_Stts */
    CbCreate_destroyArray(&i_this->mAcchCir, 2, 0x40,
                          gabi::guest_fn((const void *)cb1_AcchCir_dtor),
                          0); /* __destroy_arr(mAcchCir, 2, 0x40, ~dBgS_AcchCir, flags 0) */
    CbCreate_destroyMtxCalc(gabi::ea(&i_this->mNutBckAnim.mpAnm),
                            0); /* mNutBckAnim (frame control part) */
    CbCreate_destroyMtxCalc(gabi::ea(&i_this->mPropellerBckAnim.mpAnm), 0); /* mPropellerBckAnim */
    CbCreate_destroyNpc(i_this, 0); /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0221FBD0 */
static cPhs_State daNpc_Cb1_Create(fopAc_ac_c *i_this) {

    return CbCreate_create(i_this); /* create() */
}

// HD: framework deletion goes through the actor vtable; this wrapper only returns success.
/* 02226F30 */
static BOOL daNpc_Cb1_Delete(daNpc_Cb1_c *i_this) { return TRUE; }

/* 02221768 */
static BOOL daNpc_Cb1_Draw(daNpc_Cb1_c *i_this) { return i_this->draw(); }

/* 0222176C */
static BOOL daNpc_Cb1_IsDelete(daNpc_Cb1_c *i_this) { return TRUE; }

/* 0221EC9C */
static s16 daNpc_Cb1_XyCheckCB(void *i_this, int i_itemBtn) {

    /* daNpc_Cb1_c::XyCheckCB (inline) */
    if (dComIfGp_getSelectItem(i_itemBtn) == 0x22 /* dItemNo_WIND_WAKER_e */ &&
        dComIfGs_isEventBit(0x1880)) {
        return TRUE;
    }
    return FALSE;
}

/* 0221ED00 */
static s16 daNpc_Cb1_XyEventCB(daNpc_Cb1_c *i_this, int) { return i_this->mEventIdx[2]; }

// HD: compiler-generated and template code of this translation unit.
// Distinct compiler default copies retain their original result register widths.

/* 02226E7C */
static void __sinit_d_a_npc_cb1_cpp() {

    sinit_header_statics(gabi::ea(&l_headerStatics), gabi::ea(&l_globalRecords));
    daNpc_Cb1_HIO_c_ct(&cb1_l_HIO());
}

/* 02226F1C */
static void cb1_empty_dtor(void *i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02226F38 */
static void cb1_AcchCir_dtor(dBgS_AcchCir *i_this, s32 flags) {

    if (i_this != nullptr) {
        CbCreate_destroyCircle(gabi::ea(i_this) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 022270A0 */
static s16 cb1_cLib_getRndValue_s(s16 i_min, s16 i_range) {

    f32 base = (f32)i_min;
    return (s16)gabi::ftoi(gabi::fadds_ppc(base, cM_rndF((f32)i_range)));
}

/* 02226F8C */
static s32 cb1_getTactMusic(daNpc_Cb1_c *i_this) { return -1; }

/* 02226F94 */
static s32 cb1_getTactTimerCancel(daNpc_Cb1_c *i_this) { return 0; }

/* 02226F9C */
static s32 cb1_checkPlayerGuard(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FA4 */
static s32 cb1_getGrabMissActor(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FAC */
static s32 cb1_checkPlayerFly(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FB4 */
static s32 cb1_checkFrontRoll(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FBC */
static s32 cb1_checkBottleSwing(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FC4 */
static s32 cb1_checkCutCharge(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FCC */
static s32 cb1_getBokoFlamePos(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FD4 */
static s32 cb1_checkTactWait(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FDC */
static void cb1_onDekuSpReturnFlg(daNpc_Cb1_c *i_this) {}

/* 02226FE0 */
static s32 cb1_checkComboCutTurn(daNpc_Cb1_c *i_this) { return 0; }

/* 02226FE8 */
static s32 cb1_getItemID(daNpc_Cb1_c *i_this) { return -1; }

/* 02226FF0 */
static s32 cb1_getThrowBoomerangID(daNpc_Cb1_c *i_this) { return -1; }

/* 02226FF8 */
static s32 cb1_getGrabActorID(daNpc_Cb1_c *i_this) { return -1; }

/* 02227000 */
static s32 cb1_checkGrabBarrel(daNpc_Cb1_c *i_this) { return 0; }

/* 02227008 */
static s32 cb1_checkPlayerNoDraw(daNpc_Cb1_c *i_this) { return 0; }

/* 02227010 */
static s32 cb1_checkRopeTag(daNpc_Cb1_c *i_this) { return 0; }

/* 02227018 */
static s32 cb1_checkRopeReadyAnime(daNpc_Cb1_c *i_this) { return 0; }

/* 02227020 */
static void cb1_voiceStart(daNpc_Cb1_c *i_this) {}

/* 02227024 */
static void cb1_setOutPower(daNpc_Cb1_c *i_this) {}

/* 02227028 */
static void cb1_onFrollCrashFlg(daNpc_Cb1_c *i_this) {}

/* 0222702C */
static s32 cb1_setHookshotCarryOffset(daNpc_Cb1_c *i_this) { return 0; }

/* 02227034 */
static void cb1_setPlayerPositionAndYaw(daNpc_Cb1_c *i_this) {}

/* 02227038 */
static void cb1_setPlayerPositionAndAngles(daNpc_Cb1_c *i_this) {}

/* 0222703C */
static void cb1_setPlayerPositionAndMatrix(daNpc_Cb1_c *i_this) {}

/* 02227040 */
static s32 cb1_setThrowDamage(daNpc_Cb1_c *i_this) { return 0; }

/* 0222707C */
static s32 cb1_returnTrueTemplateDefault(daNpc_Cb1_c *i_this) { return 1; }

/* 02227084 */
static s32 cb1_returnTrueTemplateCopyA(daNpc_Cb1_c *i_this) { return 1; }

/* 0222708C */
static s32 cb1_returnTrueTemplateCopyB(daNpc_Cb1_c *i_this) { return 1; }

/* 02227094 */
static s32 cb1_returnTrueTemplateCopyC(daNpc_Cb1_c *i_this) { return 1; }

/* 02227078 */
static void cb1_emptyTemplateDefault(daNpc_Cb1_c *i_this) {}

/* 0222709C */
static void cb1_assureTerminationImpl(daNpc_Cb1_c *i_this) {}

/* 02227048 */
static f32 cb1_getGroundY(daNpc_Cb1_c *i_this) {

    return i_this->mAcch.GetGroundH(); /* mAcch (0x43C) + 0x94: ground height */
}

/* 02227050 */
static u32 cb1_getLeftHandMatrix(daNpc_Cb1_c *i_this) { return i_this->cullMtx; }

/* 02227058 */
static u32 cb1_getRightHandMatrix(daNpc_Cb1_c *i_this) { return i_this->cullMtx; }

/* 02227060 */
static f32 cb1_getBaseAnimeFrameRate(daNpc_Cb1_c *i_this) { return 1.0f; }

/* 0222706C */
static f32 cb1_getBaseAnimeFrame(daNpc_Cb1_c *i_this) { return 0.0f; }

// clang-format off
/* WWHD glue
str 10018E54 "Cb" in 0221E6E4
str 10018FA8 "Cb" in 02221774
str 10018F58 "Cb" in 0221F700
str 10018F54 "sea" in 0221F700
str 10018E60
str 10018E88
str 10018E70
str 10018EA8
str 10018EC0
str 10018ED8
str 10018E9C
str 10018E40
str 10018E4C
str 10018E48
str 10018E58
data cbMain_HIO 10466B98
data l_headerStatics 10466B2C
data l_globalRecords 101BDA00
data m_playerRoom 101BDC2C
data m_flying 101BDC2D
data m_flyingTimer 101BDC28
data m_status 101BDC2A
data l_waitNpcAction 10018BE0
data l_shipNpcAction 10018C10
data l_musicNpcAction 10018C48
data l_eventNameTbl 101BD9E0
data l_offsetAttPos 10466B48
data l_offsetEyePos 10466B54
data l_nutOffset 10466B60
data l_nutBase 10466B6C
data l_cyl_src 101BD958
data l_wind_cyl_src 101BD99C
data l_partner 101CEF74
data l_currentModel 104B462C
data l_currentMtx 104B4868
data l_attGuard 10466B84
data l_eyeGuard 10466B88
data l_nutOffsetGuard 10466B8C
data l_nutBaseGuard 10466B90
data l_roomStatus 1047E8E8
ext 025DF2B8 s32 CbCreate_drawPriority(fopAc_ac_c* actor)
ext 025DA874 void CbCreate_enqueueDraw(u32 tag, s32 priority)
ext 0200F9D4 void CbCreate_offsetPos(cXyz* out, const cXyz* base, s16 angle, const cXyz* offset)
ext 027F2BC0 void CbCreate_initFrameCtrl(u32 control, s32 end)
ext 027DA984 void CbCreate_constructMtxCalc(u32 control)
ext 028E9108 void CbCreate_concatMtx(const Mtx34* a, const Mtx34* b, Mtx34* out)
ext 0259DA18 void CbCreate_constructNpcHio(u32 hio)
ext 02018034 void CbCreate_destroyCircle(u32 circle, s32 flags)
ext 0221FECC void CbCreate_musicStop(daNpc_Cb1_c* actor)
ext 02515A70 void CbCreate_destroyCylinder(dCcD_Cyl* cylinder, s32 flags)
ext 02515860 void CbCreate_destroyStatus(dCcD_Stts* status, s32 flags)
ext 028F0164 void CbCreate_destroyArray(void* array, s32 count, s32 size, u32 destructor, s32 flags)
ext 027F3628 void CbCreate_destroyMtxCalc(u32 calculator, s32 flags)
ext 0244513C void CbCreate_destroyNpc(daNpc_Cb1_c* actor, s32 flags)
ext 0221E6E4 BOOL CbCreate_createHeap(fopAc_ac_c* actor)
ext 02445A00 void CbCreate_drawDamageFog(daNpc_Cb1_c* actor)
ext 0221FC6C BOOL CbCreate_isFlyAction(daNpc_Cb1_c* actor)
ext 0221F700 cPhs_State CbCreate_create(fopAc_ac_c* actor)
ext 025F19F8 void CbCreate_rotateXYZ(Mtx34* matrix, s16 x, s16 y, s16 z)
ext 024450B4 void CbCreate_constructNpc(daNpc_Cb1_c* actor)
ext 028EFFD0 void CbCreate_constructArray(void* array, s32 count, s32 size, u32 constructor)
ext 0259DAA0 void CbCreate_constructJointCtrl(dNpc_JntCtrl_c* joint)
ext 025B9834 void CbCreate_setRestartOption(u32 restart, s32 mode, u32 position, s16 angle, s8 room)
ext 025B8880 void CbCreate_setPriest(u32 priest, s32 mode, u32 position, s16 angle, s8 room)
ext 02445784 void CbCreate_checkRestart(daNpc_Cb1_c* actor, s32 mode)
ext 02017374 void CbCreate_upMtxBase(const cXyz* base, const cXyz* direction, Mtx34* result)
*/
