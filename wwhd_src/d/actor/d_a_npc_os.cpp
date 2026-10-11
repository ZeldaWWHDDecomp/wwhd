/**
 * d_a_npc_os.cpp (WWHD)
 * NPC - Os (the Servant Tower statue controlled by Link): create, heaps,
 * node callbacks, setup helpers, HIO, static init, destructor.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Other parts: d_a_npc_os_a.cpp, d_a_npc_os_b.cpp, d_a_npc_os_c.cpp.
 */
#include "d/actor/d_a_npc_os_create.h"

// HD: both Os parts map this view to the same guest lighting storage.
static OsSmokeLightingHD osMain_smokeTevStr;
static u32 osMain_currentModel;
static Mtx34 osMain_currentMtx;
static Mtx34 osMain_now;
static u32 osMain_attPosGuard;
static u32 osMain_eyePosGuard;
static cXyz osMain_offsetAttPos;
static cXyz osMain_offsetEyePos;
static os_b::daNpc_Os_HIO_c osMain_HIO;
static s32 osMain_hioCounter;
static OsMainHeaderStatics osMain_headerStatics;
static cXyz osMain_smokeScale;
static OsMainTables::Brk osMain_brkAnmTbl;
// HD: the animation parameter table has six entries rather than five.
static OsMainTables::Anm osMain_anmPrm;
static OsMainTables::Indices osMain_anmTbl;
static OsMainTables::Names osMain_eventNames;
static OsMainTables::Staff osMain_staffNames;
static OsMainTables::Positions osMain_finishHomePos;
static ProcFunc_l osMain_waitAction;
static ProcFunc_l osMain_finishAction;
static u32 osMain_smokeLighting;
static u8 osMain_playerRooms;
static dCcD_SrcCyl osMain_cylSrc;
static os_b::Smoke osMain_smoke;
static OsMainGlobalRecord osMain_globalRecord;
static OsMainGlobalRecord osMain_headerRecordA;
static OsMainGlobalRecord osMain_headerRecordB;

// HD: preserve the matrix-pointer load before the dirty-flag write.
/* inlined into nodeCallBack and tunoNodeCallBack */
static inline u32 os_j3dSys_model() { return osMain_currentModel; }
/* inlined into nodeCallBack and tunoNodeCallBack */
static inline Mtx34 *os_mCurrentMtx() { return &osMain_currentMtx; }
/* inlined into nodeCallBack, tunoNodeCallBack and setBaseMtx */
static inline Mtx34 *os_now() { return &osMain_now; }
/* inlined into nodeCallBack and tunoNodeCallBack */
static inline Mtx34 *os_getAnmMtx(u32 model, u32 jntNo) {
    auto *block = gabi::at<J3DModel>(model)->mpMtxBuffer.get();
    u32 m = block->mpAnmMtx;
    block->mFlags = block->mFlags | 0x10;
    return gabi::at<Mtx34>(m + jntNo * 0x30);
}

/* 022AD20C */
static BOOL daNpc_Os_Draw(daNpc_Os_c *i_this) { return OsMain_draw(i_this); /* draw() */ }

/* 022A9AB8 */
static BOOL nodeCallBack(J3DNode *node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 model = os_j3dSys_model();
        daNpc_Os_c *i_this =
            reinterpret_cast<daNpc_Os_c *>(gabi::at<J3DModel>(model)->getUserArea());
        if (i_this != nullptr) {
            /* static cXyz l_offsetAttPos(0, 0, 0), l_offsetEyePos(15, 0, 0) (guarded function
             * statics) */
            if (osMain_attPosGuard == 0) {
                osMain_offsetAttPos.x = 0.0f;
                osMain_offsetAttPos.z = 0.0f;
                osMain_attPosGuard = 1;
                osMain_offsetAttPos.y = 0.0f;
            }
            if (osMain_eyePosGuard == 0) {
                osMain_offsetEyePos.y = 0.0f;
                osMain_eyePosGuard = 1;
                osMain_offsetEyePos.x = 15.0f;
                osMain_offsetEyePos.z = 0.0f;
            }
            u32 jntNo = reinterpret_cast<J3DJoint *>(node)->getJntNo();
            PSMTXCopy(os_getAnmMtx(model, jntNo), os_now());
            PSMTXMultVec(os_now(), &osMain_offsetAttPos, &i_this->field_0x754);
            mDoMtx_XrotM(os_now(), i_this->mJntCtrl.mAngles[0][1]);
            mDoMtx_ZrotM(os_now(), (s16)-i_this->mJntCtrl.mAngles[0][0]);
            PSMTXMultVec(os_now(), &osMain_offsetEyePos, &i_this->field_0x748);
            if (i_this->field_0x7A3 != 0xFF) { /* incAttnSetCount */
                i_this->field_0x7A3 = i_this->field_0x7A3 + 1;
            }
            PSMTXCopy(os_now(), os_mCurrentMtx());
            mtx_copy(os_getAnmMtx(model, jntNo), os_now()); /* model->setAnmMtx(jntNo, now) */
        }
    }
    return TRUE;
}

/* 022A9C74 */
BOOL daNpc_Os_c::jointCheck(s8 param_1) {
    if (argument == 0) {
        if (param_1 == mTuno3JointIdx) {
            return TRUE;
        }
    } else if (argument == 1) {
        if (param_1 == mTuno2JointIdx) {
            return TRUE;
        }
    } else if (argument == 2) {
        if (param_1 == mTuno1JointIdx) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 022A9CD4 */
static BOOL tunoNodeCallBack(J3DNode *node, int calcTiming) {
    if (calcTiming == 0) {
        u32 model = os_j3dSys_model();
        daNpc_Os_c *i_this =
            reinterpret_cast<daNpc_Os_c *>(gabi::at<J3DModel>(model)->getUserArea());
        if (i_this != nullptr) {
            u32 jntNo = reinterpret_cast<J3DJoint *>(node)->getJntNo();
            if (!i_this->jointCheck((s8)jntNo)) {
                PSMTXCopy(os_getAnmMtx(model, jntNo), os_now());
                mDoMtx_stack_scaleM(0.1f, 0.1f, 0.1f);
                PSMTXCopy(os_now(), os_mCurrentMtx());
                mtx_copy(os_getAnmMtx(model, jntNo), os_now());
            }
        }
    }
    return TRUE;
}

#define OS_SAFESTRING_VTBL SAFE_STRING_VTABLE_HD /* this TU's sead::SafeString vtable */
#define OS_ARC "Os"                              /* "Os" */
/* dComIfGs_isEventBit / onEventBit: dSv_event_c at save info + 0x644 */
/* inlined into wakeupCheck and finishCheck */
static inline BOOL os_isEventBit(u16 flag) { return dComIfGs_isEventBit(flag); }
/* mDoExt_brkAnm::init(data, key, anmPlay, mode, rate, start, end, modify, entry(stack))  */
/* inlined into initBrkAnm */
static inline BOOL os_brkAnm_init(mDoExt_brkAnm_os *a, J3DModelData *d, void *key, bool play,
                                  s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry) {
    return OsMain_initBrk(a, d, key, play, mode, rate, start, end, modify, entry);
}
/* inlined into initBrkAnm */
static inline J3DModelData *os_modelData(J3DModel *m) { return m->getModelData(); }

/* 022A9E0C */
BOOL daNpc_Os_c::wakeupCheck() {
    if (argument == 0) {
        if (os_isEventBit(0x1780)) {
            return TRUE;
        }
    } else if (argument == 1) {
        if (os_isEventBit(0x1740)) {
            return TRUE;
        }
    } else if (argument == 2) {
        if (os_isEventBit(0x1720)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 022A9EAC */
BOOL daNpc_Os_c::finishCheck() {
    if (argument == 0) {
        if (os_isEventBit(0x1710)) {
            return TRUE;
        }
    } else if (argument == 1) {
        if (os_isEventBit(0x1704)) {
            return TRUE;
        }
    } else if (argument == 2) {
        if (os_isEventBit(0x1B01)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 022A9F4C */
BOOL daNpc_Os_c::initBrkAnm(u8 param_1, bool param_2) {
    J3DModelData *modelData = os_modelData(mpMorf->getModel());
    BOOL ret = FALSE;
    if ((u32)(s32)field_0x7A2 != (u32)param_1) {
        const auto *e = gabi::at<const OsMainBrkEntry>(
            gabi::ea(&osMain_brkAnmTbl) +
            param_1 * sizeof(OsMainBrkEntry)); /* brkAnmTbl[param_1] */
        void *a_brk = dComIfG_getObjectRes(OS_ARC, e->brkFileIndex, OS_SAFESTRING_VTBL);
        if (a_brk == nullptr) {
            JUT_ASSERT_fail("d_a_npc_os.cpp", 0xBF2, "a_brk != NULL"); /* a_brk != NULL */
        }
        if (os_brkAnm_init(&mBrkAnm, modelData, a_brk, true, e->loopMode, e->speed, 0, -1, param_2,
                           0)) {
            field_0x7A2 = (s8)param_1;
            if (e->unk_4 < 0) {
                f32 end = (f32)(s16)mBrkAnm.mFrameCtrl.mEnd; /* setFrame(getEndFrame()) */
                mBrkAnm.mFrameCtrl.mFrame = end;
                field_0x764 = end;
            } else {
                field_0x764 = mBrkAnm.mFrameCtrl.mFrame;
            }
            ret = TRUE;
        }
    } else {
        ret = TRUE;
    }
    return ret;
}

/* 022AA42C */
s8 daNpc_Os_c::getRestartNumber() {
    if (argument == 0) {
        return 3;
    }
    if (argument == 1) {
        return 4;
    }
    if (argument == 2) {
        return 5;
    }
    return 0;
}

/* inlined into createHeap */
static inline s32 os_getJointIndex(J3DModelData *modelData, const char *name) {
    return modelData->getJointName()->getIndex(name);
}
/* 022AA080 */
BOOL daNpc_Os_c::createHeap() {
    J3DModelData *modelData =
        (J3DModelData *)dComIfG_getObjectRes("Os", BDL_OS, OS_SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_npc_os.cpp", 0x2FE, "modelData != NULL"); /* modelData != NULL */
    }
    J3DAnmTransform *anm =
        (J3DAnmTransform *)dComIfG_getObjectRes("Os", BCK_OS_MOVE01, OS_SAFESTRING_VTBL);
    mpMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 1, nullptr, 0x80000, 0x11000002);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    s16 headIdx = (s16)os_getJointIndex(modelData, "head_joint");
    if (headIdx >= 0) {
        modelData->getJointNodePointer((u16)headIdx)->setCallBack(nodeCallBack);
    }
    mTuno1JointIdx = (s8)os_getJointIndex(modelData, "tuno1_joint");
    if (mTuno1JointIdx >= 0) {
        modelData->getJointNodePointer((u16)(s8)mTuno1JointIdx)->setCallBack(tunoNodeCallBack);
    }
    mTuno2JointIdx = (s8)os_getJointIndex(modelData, "tuno2_joint");
    if (mTuno2JointIdx >= 0) {
        modelData->getJointNodePointer((u16)(s8)mTuno2JointIdx)->setCallBack(tunoNodeCallBack);
    }
    mTuno3JointIdx = (s8)os_getJointIndex(modelData, "tuno3_joint");
    if (mTuno3JointIdx >= 0) {
        modelData->getJointNodePointer((u16)(s8)mTuno3JointIdx)->setCallBack(tunoNodeCallBack);
    }

    mpMorf->getModel()->setUserArea(reinterpret_cast<uintptr_t>(this)); /* setUserArea */

    u8 anm_;
    if (wakeupCheck()) {
        if (finishCheck()) {
            anm_ = 1;
        } else {
            anm_ = 6;
        }
    } else {
        anm_ = 5;
    }
    field_0x7A2 = 9;
    if (!initBrkAnm(anm_, false)) {
        return FALSE;
    }

    mAcchCir[0].SetWall(20.0f, 40.0f);
    mAcchCir[1].SetWall(60.0f, 40.0f);
    mAcch.Set(&current.pos, &old.pos, this, 2, &mAcchCir[0], &speed, nullptr, nullptr);
    u32 f = mAcch.m_flags;
    mAcch.SetRoofCrrHeight(120.0f); /* SetRoofCrrHeight(120.0f) */
    mAcch.m_flags = (f | dBgS_Acch::LINE_CHECK) & ~dBgS_Acch::ROOF_NONE;
    return TRUE;
}

/* (unnamed)  */
/* 022AA428 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) { return ((daNpc_Os_c *)i_this)->createHeap(); }

/* 022AA464 */
void daNpc_Os_c::setBaseMtx() {
    J3DModel *pModel = mpMorf->getModel();
    if (actor_status & 0x2000 /* fopAcM_checkCarryNow */) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        mDoMtx_stack_transM(field_0x7E4.x, field_0x7E4.y, field_0x7E4.z);
        mDoMtx_XrotM(os_now(), shape_angle.x);
        mDoMtx_ZrotM(os_now(), shape_angle.z);
    } else {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y + 95.0f, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        mDoMtx_stack_transM(field_0x7E4.x, field_0x7E4.y, field_0x7E4.z);
        mDoMtx_XrotM(os_now(), shape_angle.x);
        mDoMtx_ZrotM(os_now(), shape_angle.z);
        mDoMtx_stack_transM(0.0f, -95.0f, 0.0f);
    }
    J3DModel_setBaseTRMtx(pModel, os_now());
    mpMorf->calc();
}

/* 022AA630 */
BOOL daNpc_Os_c::setAction(ProcFunc_l *cur, ProcFunc_l *newFunc, void *arg) {
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
        field_0x7A9 = -1;
        md_pmf_call<BOOL>(this, cur, arg);
    }
    cur->i = newI;
    cur->f = newF;
    cur->d = newD;
    field_0x7B8 = 0.0f;
    field_0x7B2 = 0;
    field_0x7AE = 0;
    field_0x7B0 = 0;
    field_0x7AC = 0;
    field_0x7A9 = 0;
    md_pmf_call<BOOL>(this, cur, arg);
    return TRUE;
}

/* 022AA778 */
void daNpc_Os_c::setNpcAction(ProcFunc_l *actionFunc, void *arg) {
    mPlayerAction.d = 0;
    mPlayerAction.i = 0;
    mPlayerAction.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mNpcAction, fn, arg);
}

/* 022AA7C8 */
BOOL daNpc_Os_c::dNpc_Os_setAnm(mDoExt_McaMorf *pMorf, int loopMode, f32 morf, f32 playSpeed,
                                int idx, const char *arcName) {
    BOOL ret = FALSE;
    if (pMorf != nullptr) {
        J3DAnmTransform *pAnimRes =
            (J3DAnmTransform *)dComIfG_getObjectRes(arcName, idx, OS_SAFESTRING_VTBL);
        pMorf->setAnm(pAnimRes, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
        ret = TRUE;
    }
    return ret;
}

/* 022AA88C */
void daNpc_Os_c::setAnm(int param_1) {
    field_0x78C = param_1;
    const auto *prm = gabi::at<const OsMainAnmPrm>(
        gabi::ea(&osMain_anmPrm) + (u32)param_1 * sizeof(OsMainAnmPrm)); /* l_anmPrm[param_1] */
    s8 idx = prm->mAnmTblIdx;
    f32 playSpeed = prm->mPlaySpeed;
    if (idx != field_0x7A0 || !(playSpeed == mpMorf->mFrameCtrl.mRate /* getPlaySpeed */)) {
        field_0x7A0 = idx;
        mReachedAnimEnd = 0;
        mPrevMorfFrame = 0.0f;
        s8 anm = *gabi::at<be<s8>>(gabi::ea(&osMain_anmTbl) + (u32)idx); /* l_anmTbl */
        dNpc_Os_setAnm(mpMorf, prm->mLoopMode, prm->mMorf, playSpeed, anm, "Os");
        if (prm->m10 < 0) {
            mDoExt_McaMorf *m = mpMorf;
            f32 end = (f32)(s16)m->mFrameCtrl.mEnd;           /* getEndFrame */
            m->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(end); /* setFrame */
        }
    }
}

// HD: lighting assignment preserves each original copy width and order.
/* inlined into init */
static inline void os_copy_tevstr(daNpc_Os_c *self) {
    const auto *source = reinterpret_cast<const OsSmokeLightingHD *>(&self->tevStr);
    osMain_smokeTevStr.mLight.mPosition.x = (f32)source->mLight.mPosition.x;
    osMain_smokeTevStr.mLight.mPosition.y = (f32)source->mLight.mPosition.y;
    osMain_smokeTevStr.mLight.mPosition.z = (f32)source->mLight.mPosition.z;
    osMain_smokeTevStr.mLight.mDirection.x = (f32)source->mLight.mDirection.x;
    osMain_smokeTevStr.mLight.mDirection.y = (f32)source->mLight.mDirection.y;
    osMain_smokeTevStr.mLight.mDirection.z = (f32)source->mLight.mDirection.z;
    osMain_smokeTevStr.mLight.mColor.r = (u8)source->mLight.mColor.r;
    osMain_smokeTevStr.mLight.mColor.g = (u8)source->mLight.mColor.g;
    osMain_smokeTevStr.mLight.mColor.b = (u8)source->mLight.mColor.b;
    osMain_smokeTevStr.mLight.mColor.a = (u8)source->mLight.mColor.a;
    osMain_smokeTevStr.mLight.mColorS10.r = (u16)source->mLight.mColorS10.r;
    osMain_smokeTevStr.mLight.mColorS10.g = (u16)source->mLight.mColorS10.g;
    osMain_smokeTevStr.mLight.mColorS10.b = (u16)source->mLight.mColorS10.b;
    osMain_smokeTevStr.mLight.mColorS10.a = (u16)source->mLight.mColorS10.a;
    osMain_smokeTevStr.mLight.mCosAtten[0] = (f32)source->mLight.mCosAtten[0];
    osMain_smokeTevStr.mLight.mCosAtten[1] = (f32)source->mLight.mCosAtten[1];
    osMain_smokeTevStr.mLight.mCosAtten[2] = (f32)source->mLight.mCosAtten[2];
    osMain_smokeTevStr.mLight.mDistAtten[0] = (f32)source->mLight.mDistAtten[0];
    osMain_smokeTevStr.mLight.mDistAtten[1] = (f32)source->mLight.mDistAtten[1];
    osMain_smokeTevStr.mLight.mDistAtten[2] = (f32)source->mLight.mDistAtten[2];
    osMain_smokeTevStr.mLight.mCutoff = (f32)source->mLight.mCutoff;
    osMain_smokeTevStr.mLight.mReferenceBrightness = (f32)source->mLight.mReferenceBrightness;
    osMain_smokeTevStr.mLightPosWorld.x = (u32)source->mLightPosWorld.x;
    osMain_smokeTevStr.mLightPosWorld.y = (u32)source->mLightPosWorld.y;
    osMain_smokeTevStr.mLightPosWorld.z = (u32)source->mLightPosWorld.z;
    osMain_smokeTevStr.mColorC0.r = (u16)source->mColorC0.r;
    osMain_smokeTevStr.mColorC0.g = (u16)source->mColorC0.g;
    osMain_smokeTevStr.mColorC0.b = (u16)source->mColorC0.b;
    osMain_smokeTevStr.mColorC0.a = (u16)source->mColorC0.a;
    osMain_smokeTevStr.mColorK0 = (u32)source->mColorK0;
    osMain_smokeTevStr.mColorK1 = (u32)source->mColorK1;
    osMain_smokeTevStr.mFogColor.r = (u16)source->mFogColor.r;
    osMain_smokeTevStr.mFogColor.g = (u16)source->mFogColor.g;
    osMain_smokeTevStr.mFogColor.b = (u16)source->mFogColor.b;
    osMain_smokeTevStr.mFogColor.a = (u16)source->mFogColor.a;
    osMain_smokeTevStr.mFogStartZ = (f32)source->mFogStartZ;
    osMain_smokeTevStr.mFogEndZ = (f32)source->mFogEndZ;
    osMain_smokeTevStr.mColpatBlend = (f32)source->mColpatBlend;
    osMain_smokeTevStr.mInitTimer = (u8)source->mInitTimer;
    osMain_smokeTevStr.mEnvrIdxCurr = (u8)source->mEnvrIdxCurr;
    osMain_smokeTevStr.mEnvrIdxPrev = (u8)source->mEnvrIdxPrev;
    osMain_smokeTevStr.mColpatCurr = (u8)source->mColpatCurr;
    osMain_smokeTevStr.mColpatPrev = (u8)source->mColpatPrev;
    osMain_smokeTevStr.mRoomNo = (u8)source->mRoomNo;
    osMain_smokeTevStr.mEnvrIdxOverride = (u8)source->mEnvrIdxOverride;
    osMain_smokeTevStr.mLightMode = (u8)source->mLightMode;
    osMain_smokeTevStr.mInitType = (u8)source->mInitType;
    osMain_smokeTevStr.mCachedLight.mPosition.x = (f32)source->mCachedLight.mPosition.x;
    osMain_smokeTevStr.mCachedLight.mPosition.y = (f32)source->mCachedLight.mPosition.y;
    osMain_smokeTevStr.mCachedLight.mPosition.z = (f32)source->mCachedLight.mPosition.z;
    osMain_smokeTevStr.mCachedLight.mDirection.x = (f32)source->mCachedLight.mDirection.x;
    osMain_smokeTevStr.mCachedLight.mDirection.y = (f32)source->mCachedLight.mDirection.y;
    osMain_smokeTevStr.mCachedLight.mDirection.z = (f32)source->mCachedLight.mDirection.z;
    osMain_smokeTevStr.mCachedLight.mColor.r = (u8)source->mCachedLight.mColor.r;
    osMain_smokeTevStr.mCachedLight.mColor.g = (u8)source->mCachedLight.mColor.g;
    osMain_smokeTevStr.mCachedLight.mColor.b = (u8)source->mCachedLight.mColor.b;
    osMain_smokeTevStr.mCachedLight.mColor.a = (u8)source->mCachedLight.mColor.a;
    osMain_smokeTevStr.mCachedLight.mColorS10.r = (u16)source->mCachedLight.mColorS10.r;
    osMain_smokeTevStr.mCachedLight.mColorS10.g = (u16)source->mCachedLight.mColorS10.g;
    osMain_smokeTevStr.mCachedLight.mColorS10.b = (u16)source->mCachedLight.mColorS10.b;
    osMain_smokeTevStr.mCachedLight.mColorS10.a = (u16)source->mCachedLight.mColorS10.a;
    osMain_smokeTevStr.mCachedLight.mCosAtten[0] = (f32)source->mCachedLight.mCosAtten[0];
    osMain_smokeTevStr.mCachedLight.mCosAtten[1] = (f32)source->mCachedLight.mCosAtten[1];
    osMain_smokeTevStr.mCachedLight.mCosAtten[2] = (f32)source->mCachedLight.mCosAtten[2];
    osMain_smokeTevStr.mCachedLight.mDistAtten[0] = (f32)source->mCachedLight.mDistAtten[0];
    osMain_smokeTevStr.mCachedLight.mDistAtten[1] = (f32)source->mCachedLight.mDistAtten[1];
    osMain_smokeTevStr.mCachedLight.mDistAtten[2] = (f32)source->mCachedLight.mDistAtten[2];
    osMain_smokeTevStr.mCachedLight.mCutoff = (f32)source->mCachedLight.mCutoff;
    osMain_smokeTevStr.mCachedLight.mReferenceBrightness =
        (f32)source->mCachedLight.mReferenceBrightness;
    osMain_smokeTevStr.mCachedLight2.mPosition.x = (f32)source->mCachedLight2.mPosition.x;
    osMain_smokeTevStr.mCachedLight2.mPosition.y = (f32)source->mCachedLight2.mPosition.y;
    osMain_smokeTevStr.mCachedLight2.mPosition.z = (f32)source->mCachedLight2.mPosition.z;
    osMain_smokeTevStr.mCachedLight2.mDirection.x = (f32)source->mCachedLight2.mDirection.x;
    osMain_smokeTevStr.mCachedLight2.mDirection.y = (f32)source->mCachedLight2.mDirection.y;
    osMain_smokeTevStr.mCachedLight2.mDirection.z = (f32)source->mCachedLight2.mDirection.z;
    osMain_smokeTevStr.mCachedLight2.mColor.r = (u8)source->mCachedLight2.mColor.r;
    osMain_smokeTevStr.mCachedLight2.mColor.g = (u8)source->mCachedLight2.mColor.g;
    osMain_smokeTevStr.mCachedLight2.mColor.b = (u8)source->mCachedLight2.mColor.b;
    osMain_smokeTevStr.mCachedLight2.mColor.a = (u8)source->mCachedLight2.mColor.a;
    osMain_smokeTevStr.mCachedLight2.mColorS10.r = (u16)source->mCachedLight2.mColorS10.r;
    osMain_smokeTevStr.mCachedLight2.mColorS10.g = (u16)source->mCachedLight2.mColorS10.g;
    osMain_smokeTevStr.mCachedLight2.mColorS10.b = (u16)source->mCachedLight2.mColorS10.b;
    osMain_smokeTevStr.mCachedLight2.mColorS10.a = (u16)source->mCachedLight2.mColorS10.a;
    osMain_smokeTevStr.mCachedLight2.mCosAtten[0] = (f32)source->mCachedLight2.mCosAtten[0];
    osMain_smokeTevStr.mCachedLight2.mCosAtten[1] = (f32)source->mCachedLight2.mCosAtten[1];
    osMain_smokeTevStr.mCachedLight2.mCosAtten[2] = (f32)source->mCachedLight2.mCosAtten[2];
    osMain_smokeTevStr.mCachedLight2.mDistAtten[0] = (f32)source->mCachedLight2.mDistAtten[0];
    osMain_smokeTevStr.mCachedLight2.mDistAtten[1] = (f32)source->mCachedLight2.mDistAtten[1];
    osMain_smokeTevStr.mCachedLight2.mDistAtten[2] = (f32)source->mCachedLight2.mDistAtten[2];
    osMain_smokeTevStr.mCachedLight2.mCutoff = (f32)source->mCachedLight2.mCutoff;
    osMain_smokeTevStr.mCachedLight2.mReferenceBrightness =
        (f32)source->mCachedLight2.mReferenceBrightness;
}

// HD: register this statue by its argument; absent from the GameCube create path.
/* inlined into init */
static inline void os_hd_register(daNpc_Os_c *a, s32 arg) { OsMain_registerActor(a, arg); }
/* dEvent_manager_c::getEventIdx(name, roomNo 0xFF)  */
/* inlined into init */
static inline s16 os_getEventIdx(u32 name) {
    return OsMain_eventIndex(dComIfGp_ea() + 0x52C4, name, 0xFF);
}

/* 022AA9A4 */
BOOL daNpc_Os_c::init() {
    attention_info.distances[fopAc_Attn_TYPE_CARRY_e] =
        0x27; /* attention_info.distances[fopAc_Attn_TYPE_CARRY_e] */
    field_0x794 = (mParameters >> 0x10) & 0xFF; /* fopAcM_GetParam */
    os_copy_tevstr(this);
    osMain_smokeLighting = gabi::ea(&osMain_smokeTevStr); /* m_smoke.setTevStr(&m_smoke_tevstr) */
    *gabi::at<be<u8>>(gabi::ea(&osMain_playerRooms) + (u32)argument) =
        0;           /* offPlayerRoom(argument) */
    field_0x784 = 0; /* clearStatus() */
    field_0x7A0 = 3;
    gravity = (f32)osMain_HIO.field_0x8C; /* l_HIO gravity */
    field_0x78C = 5;
    if (wakeupCheck()) {
        field_0x784 = field_0x784 | os_b::STATUS_GRAVITY;
    } else {
        field_0x784 = field_0x784 & ~os_b::STATUS_GRAVITY;
    }
    gabi::Local<ProcFunc_l> fn;
    if (finishCheck()) {
        Os_copyActionHD(fn, &osMain_finishAction); /* &daNpc_Os_c::finish02NpcAction */
        setNpcAction(fn, nullptr);
    } else {
        setAnm(0);
        Os_copyActionHD(fn, &osMain_waitAction); /* &daNpc_Os_c::waitNpcAction */
        setNpcAction(fn, nullptr);
    }
    field_0x7A5 = -1;
    field_0x748.copy(current.pos);
    mpPedestal = nullptr;
    field_0x754.copy(current.pos);
    field_0x788 = 120.0f;
    field_0x7AA = -1;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&osMain_cylSrc);
    mCyl.SetStts(&mStts);
    for (int i = 0; i < 0x10; i++) {
        field_0x7C4[i] = os_getEventIdx((u32)osMain_eventNames.entries[i]); /* event_name_tbl[i] */
    }
    os_hd_register(this, argument); /* HD */
    return TRUE;
}

// HD: GHS inlines the actor constructor into create.
/* inlined into create */
static inline void os_ct(daNpc_Os_c *p) {
    OsMain_constructNpc(p); /* daPy_npc_c::daPy_npc_c */
    p->__vtbl = OS_VTBL;
    OsMain_constructBrk(&p->mBrkAnm); /* mDoExt_brkAnm::mDoExt_brkAnm */
    OsMain_constructArray(p->mAcchCir, 2, 0x40,
                          ACCH_CIRCLE_CTOR_HD); /* __construct_array(dBgS_AcchCir) */
    dCcD_Stts_ct(&p->mStts);
    dCcD_Cyl_ct(&p->mCyl, AAB_VTABLE_HD /* this TU's cM3dGAab vtable */);
    OsMain_constructJointCtrl(&p->mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
    p->field_0x7FC.mpBgW = 0;
    p->field_0x7FC.__vtbl = POLY_INFO_VTABLE_HD; /* cBgS_PolyInfo vtable */
    p->field_0x7FC.mPolyIndex = 0xFFFF;
    p->field_0x738.__vtbl = INFINITE_CALLBACK_VTABLE_HD; /* daNpc_Os_infiniteEcallBack_c vtable */
    p->field_0x7FC.mProcId = -1;                         /* actor id */
    p->field_0x7FC.mBgIndex = 0x100;
    p->field_0x740.__vtbl = INFINITE_CALLBACK_VTABLE_HD;
}

/* 022AAEE0 */
cPhs_State daNpc_Os_c::create() {
    /* fopAcM_ct(this, daNpc_Os_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            os_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State result = dComIfG_resLoad(&mPhs, "Os");
    if (result == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((void *)CheckCreateHeap),
                                   0xFA0 /* l_heap_size */)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        if (!finishCheck()) {
            OsMain_checkRestart(this, (s32)getRestartNumber()); /* daPy_npc_c::checkRestart */
        } else if (argument < 3) {
            const auto *t = gabi::at<const cXyz>(
                gabi::ea(&osMain_finishHomePos) +
                (u32)argument * sizeof(cXyz)); /* l_finish_home_pos[argument] */
            f32 x = (f32)t->x;
            home.pos.x = x;
            f32 y = (f32)t->y;
            home.pos.y = y;
            f32 z = (f32)t->z;
            current.pos.x = x;
            home.pos.z = z;
            current.pos.y = y;
            current.pos.z = z;
        }
        setBaseMtx();
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        if ((s8)osMain_HIO.mNo < 0) {                                  /* l_HIO.mNo */
            s8 no = mDoHIO_createChild("お供石像" /* "\x82\xa8\x8b\x9f\x90\xce\x91\x9c" */,
                                       &osMain_HIO);
            osMain_HIO.field_0x5C = this; /* l_HIO.field_0x5C */
            osMain_HIO.mNo = no;
            osMain_hioCounter = 1; /* l_hio_counter */
        } else {
            osMain_hioCounter = osMain_hioCounter + 1;
        }
        if (!init()) {
            return cPhs_ERROR_e;
        }
        fopAcM_setStageLayer(this);
    }
    return result;
}

/* 022AB1C0 */
static cPhs_State daNpc_Os_Create(fopAc_ac_c *i_this) { return ((daNpc_Os_c *)i_this)->create(); }

/* fopAcM_seStartCurrent (HD inline, this known non-null): reverb, then mDoAud_seStart at
 * current.pos */
/* inlined into playBrkAnm */
static inline void os_seStartCurrent(fopAc_ac_c *a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    mDoAud_seStart(id, &a->current.pos, param, reverb);
}
// HD: signed room indices retain wrapping guest address arithmetic.

/* 022AB1C4 */
void daNpc_Os_c::checkPlayerRoom() {
    *gabi::at<be<u8>>(gabi::ea(&osMain_playerRooms) + (u32)argument) =
        0; /* offPlayerRoom(argument) */
    if (wakeupCheck() && !finishCheck()) {
        fopAc_ac_c *link = dComIfGp_getLinkPlayer();
        if (current.roomNo == link->current.roomNo) {
            *gabi::at<be<u8>>(gabi::ea(&osMain_playerRooms) + (u32)argument) =
                1; /* onPlayerRoom(argument) */
        }
    }
}

/* 022AB258 */
static void daNpc_Os_infiniteEcallBack_end(daNpc_Os_infiniteEcallBack_l *cb) {
    if (cb->mpBaseEmitter != nullptr) {
        auto *e = reinterpret_cast<OsMainEmitter *>(cb->mpBaseEmitter.get());
        e->mMaxFrame = -1; /* becomeInvalidEmitter: stopCreateParticle, maxFrame */
        e->mFlags = e->mFlags | 1;
        reinterpret_cast<OsMainEmitter *>(cb->mpBaseEmitter.get())->mEmitterCallBack =
            0; /* setEmitterCallBackPtr(NULL) */
        cb->mpBaseEmitter = nullptr;
    }
}

/* 022AB28C */
void daNpc_Os_c::endBeam() {
    daNpc_Os_infiniteEcallBack_end(&field_0x738);
    daNpc_Os_infiniteEcallBack_end(&field_0x740);
}

/* 022AB2BC */
void daNpc_Os_c::setCollision() {
    gabi::Local<cXyz> temp;
    temp->copy(current.pos);
    mCyl.mCyl.SetC(temp);
    mCyl.mCyl.SetR(30.0f);
    mCyl.mCyl.SetH(field_0x788);
    mStts.SetWeight(dComIfGp_getPlayer(0) == this ? 0xFE : 0xFF); /* mStts.SetWeight */
    cCcS_Set(dComIfG_Ccsp(), &mCyl);
}

/* 022AB350 */
void daNpc_Os_c::smokeSet(u16 particle) {
    dPa_control_set(dComIfGp_getParticle(), 0, particle, &current.pos, nullptr, nullptr, 0xFF,
                    reinterpret_cast<dPa_levelEcallBack *>(&osMain_smoke), -1, nullptr, nullptr,
                    nullptr);
}

/* 022AB3C4 */
void daNpc_Os_c::playBrkAnm() {
    field_0x7A1 = (s8)OsMain_playBrk(&mBrkAnm); /* mBrkAnm.play() */
    f32 rate = mBrkAnm.mFrameCtrl.mRate;
    f32 frame = mBrkAnm.mFrameCtrl.mFrame;
    if (rate < 0.0f) {
        if (frame > field_0x764) {
            field_0x7A1 = 1;
        }
    } else if (frame < field_0x764) {
        field_0x7A1 = 1;
    }
    field_0x764 = frame;
    if (field_0x7A2 == 6 && frame > 5.0f && frame < 50.0f) {
        os_seStartCurrent(this, 0x50FF /* JA_SE_OBJ_OSTATUE_BLINK */, 0);
    }
}

/* 022AB488 */
void daNpc_Os_c::animationPlay() {
    u32 mtrlSndId = 0;
    if ((field_0x784 & 0x10) && (mAcch.m_flags & dBgS_Acch::GROUND_HIT) &&
        OsMain_polygonSafe(dComIfG_Bgsp(),
                           reinterpret_cast<cBgS_PolyInfo_l *>(static_cast<cBgS_PolyInfo *>(
                               &mAcch.m_gnd))) /* ChkPolySafe(m_gnd) */) {
        mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd));
    }
    s8 reverb = (s8)dComIfGp_getReverb(current.roomNo);
    mReachedAnimEnd = (s8)mpMorf->play(&eyePos, mtrlSndId, reverb);
    mDoExt_McaMorf *m = mpMorf;
    f32 speed = m->mFrameCtrl.mRate;
    f32 frame = m->mFrameCtrl.mFrame;
    if (speed < 0.0f) {
        if (frame > mPrevMorfFrame) {
            mReachedAnimEnd = 1;
        }
    } else if (frame < mPrevMorfFrame) {
        mReachedAnimEnd = 1;
    }
    mPrevMorfFrame = frame;
    if (field_0x78C == 1 && mpMorf->checkFrame(17.0f) /* checkFrame(17) */) {
        smokeSet(0xA328 /* ID_AK_ST_OTOMOSMOKE00 */);
    }
    playBrkAnm();
}

/* 022AB5A4 */
int daNpc_Os_c::getMyStaffId() {
    if (argument < 3) {
        u32 name = *gabi::at<be<u32>>(gabi::ea(&osMain_staffNames) +
                                      (u32)argument * sizeof(be<u32>)); /* l_staff_name[argument] */
        return OsMain_staffId(dComIfGp_ea() + 0x52C4, name, 0, 0);
    }
    return -1;
}

/* 022AB618 */
void daNpc_Os_c::setFinish() {
    auto *ev = dComIfGs_getEvent();
    if (argument == 0) {
        ev->onEventBit(0x1710);
    } else if (argument == 1) {
        ev->onEventBit(0x1704);
    } else if (argument == 2) {
        ev->onEventBit(0x1B01);
    }
}

/* 022AB670 */
BOOL daNpc_Os_c::checkCommandTalk() {
    return eventInfo.mCommand ==
           dEvtCmd_INTALK_e; /* dComIfGp_event_chkTalkXY: eventInfo.mCommand == TALK */
}

/* 022AB684 */
void daNpc_Os_c::returnLinkPlayer() {
    OsMain_changePlayer(this, dComIfGp_getLinkPlayer()); /* daPy_py_c::changePlayer */
    mNoResetFlg1 = mNoResetFlg1 & ~2u;                   /* offNpcCallCommand */
    initBrkAnm(6, true);
}

/* 022B01A4 */
static void *daNpc_Os_HIO2_c_ct(void *p) {
    if (p == nullptr) {
        p = operator_new(0x2C);
        if (p == nullptr) {
            return p;
        }
    }
    auto *h = reinterpret_cast<os_b::daNpc_Os_HIO2_c *>(
        p); /* HD: fields from +0 (GameCube +4), vtable at the end (+0x28) */
    h->field_0x1C = 0.6f;
    h->field_0x24 = 3000;
    h->field_0x0C = 2.5f;
    h->vtable = HIO_CHILD_VTABLE_HD; /* vtable */
    h->field_0x20 = 1.0f;
    h->field_0x28 = 5;
    h->field_0x18 = 2.5f;
    h->field_0x10 = 1.8f;
    h->field_0x08 = 17.0f;
    h->field_0x14 = 0.6f;
    h->field_0x26 = 1000;
    h->field_0x04 = 3.5f;
    return p;
}

/* 022B024C */
static void *daNpc_Os_HIO_c_ct(void *p) {
    if (p == nullptr) {
        p = operator_new(0xB4);
        if (p == nullptr) {
            return p;
        }
    }
    auto *h = reinterpret_cast<os_b::daNpc_Os_HIO_c *>(
        p); /* HD: mNo at +0, mOs2 +4, mNpc +0x30, fields GameCube - 4, vtable at +0xB0 */
    h->vtable = HIO_VTABLE_HD; /* vtable */
    daNpc_Os_HIO2_c_ct(&h->mOs2);
    OsMain_constructNpcHio(gabi::ea(&h->mNpc)); /* dNpc_HIO_c::dNpc_HIO_c */
    h->mNpc.mMaxBackboneX = 0;
    h->mNpc.mMinHeadY = -5000;
    h->mNpc.mMinBackboneX = 0;
    h->mNpc.mMaxAttnDistXZ = 200.0f;
    h->mNpc.mMinHeadX = 0;
    h->field_0x68 = 0.05f;
    h->field_0x70 = 0.0f;
    h->mNpc.m22 = 0;
    h->field_0x78 = 0.2f;
    h->field_0x6C = 17.0f;
    h->field_0x60 = 80.0f;
    h->field_0x64 = 500.0f;
    h->mNpc.mMaxBackboneY = 0;
    h->field_0x74 = 0.5f;
    h->mNpc.mMaxAttnAngleY = 0x4000;
    h->mNpc.mMinBackboneY = 0;
    h->mNpc.mMaxTurnStep = 0x1000;
    h->mNpc.m04 = -20.0f;
    h->mNpc.mAttnYOffset = 130.0f;
    h->mNpc.mMaxHeadTurnVel = 0x800;
    h->mNpc.mMaxHeadY = 0x1388;
    h->field_0x80 = 15.0f;
    h->field_0xAC = 18.0f;
    h->field_0x98 = -0.56152f;
    h->field_0x8C = 4.0f;
    h->mNpc.mMaxHeadX = 0;
    h->field_0xA8 = 0.75f;
    h->field_0x5C = nullptr;
    h->field_0x84 = 22.0f;
    h->field_0x7C = 0.9f;
    h->field_0xA4 = 10.0f;
    h->field_0xB0 = 8.0f;
    h->field_0xA0 = 0.2f;
    h->mNo = -1;
    h->field_0x9C = 0.1f;
    h->field_0x90 = -1.51367f;
    h->field_0x94 = -1.09863f;
    return p;
}

// HD: compiler-generated and template code of this translation unit.
// Header statics, smoke culling distances, HIO and smoke scale.
/* 022B0408 */
static void __sinit_d_a_npc_os_cpp() {
    for (int i = 3; i >= 0; i--) {
        osMain_headerStatics.mHeaderWords[i] = 0;
    }
    __register_global_object(gabi::ea(&osMain_globalRecord));
    osMain_headerStatics.mMinAngle = -3.1415927f;
    osMain_headerStatics.mMaxAngle = 3.1415927f;
    OsMain_constructHeaderA(gabi::ea(&osMain_headerStatics.mHeaderA));
    __register_global_object(gabi::ea(&osMain_headerRecordA));
    OsMain_constructHeaderB(gabi::ea(&osMain_headerStatics.mHeaderB));
    __register_global_object(gabi::ea(&osMain_headerRecordB));
    osMain_headerStatics.mFarDistances[0] = 50000.0f;
    osMain_headerStatics.mFarDistances[3] = 10000.0f;
    osMain_headerStatics.mFarDistances[1] = 50000.0f;
    osMain_headerStatics.mFarDistances[2] = 10000.0f;
    daNpc_Os_HIO_c_ct(&osMain_HIO); /* l_HIO */
    osMain_smokeScale.x = 0.5f;     /* l_smoke_scale */
    osMain_smokeScale.z = 0.5f;
    osMain_smokeScale.y = 0.5f;
}

/* sead::SafeString deleting destructor (this TU's vtable)  */
/* 022B04F0 */
static void os_SafeString_dtor(void *p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 022B0504 */
// HD: the actor delete entry is an empty successful operation.
static BOOL daNpc_Os_Delete(daNpc_Os_c *i_this) { return TRUE; }

/* dBgS_AcchCir deleting destructor (this TU's copy)  */
/* 022B050C */
static void os_AcchCir_dtor(void *p, s32 flags) {
    if (p != nullptr) {
        OsMain_destroyCircle(gabi::ea(p) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}

/* daNpc_Os_infiniteEcallBack_c::setup(emitter, ...)  */
/* 022B0638 */
static void daNpc_Os_infiniteEcallBack_setup(daNpc_Os_infiniteEcallBack_l *cb, u32 emitter) {
    cb->mpBaseEmitter = gabi::at<JPABaseEmitter>(emitter);
}

/* daNpc_Os_c::getGroundY() (mAcch.GetGroundH())  */
/* 022B0640 */
static f32 os_getGroundY(daNpc_Os_c *p) { return p->mAcch.GetGroundH(); }

// HD: these virtual adapters preserve the unextended word return register.
/* 022B0648 */
static u32 os_getLeftHandMatrix(daNpc_Os_c *p) { return p->cullMtx; }
/* 022B0650 */
static u32 os_getRightHandMatrix(daNpc_Os_c *p) { return p->cullMtx; }

// GameCube virtual animation defaults.
/* 022B0658 */
static f32 os_getBaseAnimeFrameRate(daNpc_Os_c *) { return 1.0f; }
/* 022B0664 */
static f32 os_getBaseAnimeFrame(daNpc_Os_c *) { return 0.0f; }

/* This TU's copies of the inline virtual defaults of daPy_py_c / daPy_npc_c,
 * dPa_levelEcallBack and sead::SafeString (empty, return 0, or return -1) */

/* 022B0560 */
static void os_particleExecuteAfter() {}
/* 022B0564 */
static void os_particleDraw() {}
/* 022B0568 */
static void os_particleDrawAfterHD() {}
/* 022B05BC */
static void os_onDekuSpReturnFlg() {}
/* 022B0600 */
static void os_voiceStart() {}
/* 022B0604 */
static void os_setOutPower() {}
/* 022B0608 */
static void os_onFrollCrashFlg() {}
/* 022B0614 */
static void os_setPlayerPositionAndYaw() {}
/* 022B0618 */
static void os_setPlayerPositionAndAngles() {}
/* 022B061C */
static void os_setPlayerPositionAndMatrix() {}
/* 022B0630 */
static void os_setMessageAnimation() {}
/* 022B0634 */
static void os_particleExecute() {}
/* 022B0670 */
static void os_assureTerminationImpl() {}
/* 022B0574 */
static BOOL os_getTactTimerCancel() { return 0; }
/* 022B057C */
static BOOL os_checkPlayerGuard() { return 0; }
/* 022B0584 */
static BOOL os_getGrabMissActor() { return 0; }
/* 022B058C */
static BOOL os_checkPlayerFly() { return 0; }
/* 022B0594 */
static BOOL os_checkFrontRoll() { return 0; }
/* 022B059C */
static BOOL os_checkBottleSwing() { return 0; }
/* 022B05A4 */
static BOOL os_checkCutCharge() { return 0; }
/* 022B05AC */
static BOOL os_getBokoFlamePos() { return 0; }
/* 022B05B4 */
static BOOL os_checkTactWait() { return 0; }
/* 022B05C0 */
static BOOL os_checkComboCutTurn() { return 0; }
/* 022B05E0 */
static BOOL os_checkGrabBarrel() { return 0; }
/* 022B05E8 */
static BOOL os_checkPlayerNoDraw() { return 0; }
/* 022B05F0 */
static BOOL os_checkRopeTag() { return 0; }
/* 022B05F8 */
static BOOL os_checkRopeReadyAnime() { return 0; }
/* 022B060C */
static BOOL os_setHookshotCarryOffset() { return 0; }
/* 022B0620 */
static BOOL os_setThrowDamage() { return 0; }
/* 022B056C */
static s32 os_getTactMusic() { return -1; }
/* 022B05C8 */
static s32 os_getItemID() { return -1; }
/* 022B05D0 */
static s32 os_getThrowBoomerangID() { return -1; }
/* 022B05D8 */
static s32 os_getGrabActorID() { return -1; }
/* 022B0628 */
static BOOL os_isTagCheckOK() { return 1; }

// clang-format off
/* WWHD glue
data osMain_cylSrc 101C2920
data osMain_smoke 10475440
data osMain_globalRecord 101C2B2C
data osMain_headerRecordA 101C2B38
data osMain_headerRecordB 101C2B44
str 1001F5E4 "head_joint"
str 1001F59C "tuno1_joint"
str 1001F5A8 "tuno2_joint"
str 1001F5B4 "tuno3_joint"
str 1001F560 "Os" in 022A9F4C
str 1001F598 "Os" in 022AA080
str 1001F5FC "Os" in 022AA88C
str 1001F5FF "Os" in 022AAEE0
str 1001F604 "\u304a\u4f9b\u77f3\u50cf" renamed # HD: Shift-JIS guest name, UTF-8 source spelling
str 1001F574 "a_brk != NULL"
str 1001F5D0 "modelData != NULL"
str 1001F564 "d_a_npc_os.cpp" in 022A9F4C
str 1001F5C0 "d_a_npc_os.cpp" in 022AA080
data osMain_brkAnmTbl 101C2A24
data osMain_anmPrm 101C2AB8
data osMain_anmTbl 101C2AB4
data osMain_eventNames 101C2884
data osMain_staffNames 101C28E4
data osMain_finishHomePos 101C28FC
data osMain_waitAction 1001F260
data osMain_finishAction 1001F298
data osMain_smokeLighting 1047545C
data osMain_playerRooms 101D5F4B
data osMain_currentModel 104B462C
data osMain_currentMtx 104B4868
data osMain_now 1048D0CC
data osMain_attPosGuard 10467EF8
data osMain_eyePosGuard 10467EFC
data osMain_offsetAttPos 10467E2C
data osMain_offsetEyePos 10467E38
data osMain_HIO 10467E44
data osMain_hioCounter 10467DEC
data osMain_headerStatics 10467DF4
data osMain_smokeScale 10467E20
data osMain_smokeTevStr 10475460
ext 022AD138 BOOL OsMain_draw(daNpc_Os_c* actor)
ext 025E8154 BOOL OsMain_initBrk(mDoExt_brkAnm_os* animation, J3DModelData* data, void* key, bool play, s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry)
ext 0268A8DC void OsMain_registerActor(daNpc_Os_c* actor, s32 argument)
ext 02543F10 s16 OsMain_eventIndex(u32 manager, u32 name, s32 mapTool)
ext 024450B4 void OsMain_constructNpc(daNpc_Os_c* actor)
ext 025E80D0 void OsMain_constructBrk(mDoExt_brkAnm_os* animation)
ext 028EFFD0 void OsMain_constructArray(void* array, s32 count, s32 size, u32 constructor)
ext 0259DAA0 void OsMain_constructJointCtrl(dNpc_JntCtrl_c* control)
ext 02445784 void OsMain_checkRestart(daNpc_Os_c* actor, s32 restart)
ext 025E742C u32 OsMain_playBrk(mDoExt_brkAnm_os* animation)
ext 02008254 BOOL OsMain_polygonSafe(dBgS* background, cBgS_PolyInfo_l* polygon)
ext 02542D88 s32 OsMain_staffId(u32 manager, u32 name, s32 actor, s32 flag)
ext 023D4688 void OsMain_changePlayer(daNpc_Os_c* actor, fopAc_ac_c* player)
ext 0259DA18 void OsMain_constructNpcHio(u32 hio)
ext 028ED6F8 void OsMain_constructHeaderA(u32 object)
ext 028EAB2C void OsMain_constructHeaderB(u32 object)
ext 02018034 void OsMain_destroyCircle(u32 circle, s32 flags)
*/
