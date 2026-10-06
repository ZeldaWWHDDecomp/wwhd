/**
 * d_a_npc_os.cpp (WWHD)
 * NPC - Os (the stone-head helper that Link controls in the Earth/Wind temples): create, heaps, node
 * callbacks, setup helpers, HIO, static init, destructor.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Other parts: d_a_npc_os_a.cpp, d_a_npc_os_b.cpp, d_a_npc_os_c.cpp.
 */
#include "d/actor/d_a_npc_os_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, mDoMtx_stack_c::now 0x1048D0CC;
 * joint matrices in the model's block at +0x2C (+0x4 flags, +0x10 matrices), user area +0xB8 */
static inline u32 os_j3dSys_model() { return gabi::load<u32>(0x104B462C); }
static inline Mtx34* os_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline Mtx34* os_now() { return gabi::at<Mtx34>(0x1048D0CC); }
static inline Mtx34* os_getAnmMtx(u32 model, u32 jntNo) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    u32 m = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(m + jntNo * 0x30);
}

/* 022AD20C */
static BOOL daNpc_Os_Draw(daNpc_Os_c* i_this) {
    WWHD_FUNC(0x022AD20C, BOOL, i_this);
    return gabi::call<BOOL>(0x022AD138, i_this); /* draw() */
}
VERIFY(0x022AD20C, daNpc_Os_Draw);

/* 022A9AB8 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022A9AB8, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 model = os_j3dSys_model();
        daNpc_Os_c* i_this = gabi::at<daNpc_Os_c>(gabi::load<u32>(model + 0xB8));
        if (i_this != nullptr) {
            /* static cXyz l_offsetAttPos(0, 0, 0), l_offsetEyePos(15, 0, 0) (guarded function statics) */
            if (gabi::load<u32>(0x10467EF8) == 0) {
                gabi::store<f32>(0x10467E2C, 0.0f);
                gabi::store<f32>(0x10467E34, 0.0f);
                gabi::store<u32>(0x10467EF8, 1);
                gabi::store<f32>(0x10467E30, 0.0f);
            }
            if (gabi::load<u32>(0x10467EFC) == 0) {
                gabi::store<f32>(0x10467E3C, 0.0f);
                gabi::store<u32>(0x10467EFC, 1);
                gabi::store<f32>(0x10467E38, 15.0f);
                gabi::store<f32>(0x10467E40, 0.0f);
            }
            J3DJoint* joint = J3DNode_toJoint(node);
            u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
            PSMTXCopy(os_getAnmMtx(model, jntNo), os_now());
            PSMTXMultVec(os_now(), gabi::at<cXyz>(0x10467E2C), &i_this->field_0x754);
            mDoMtx_XrotM(os_now(), i_this->mJntCtrl.mAngles[0][1]);
            mDoMtx_ZrotM(os_now(), (s16)-i_this->mJntCtrl.mAngles[0][0]);
            PSMTXMultVec(os_now(), gabi::at<cXyz>(0x10467E38), &i_this->field_0x748);
            if (i_this->field_0x7A3 != 0xFF) { /* incAttnSetCount */
                i_this->field_0x7A3 = i_this->field_0x7A3 + 1;
            }
            PSMTXCopy(os_now(), os_mCurrentMtx());
            mtx_copy(os_getAnmMtx(model, jntNo), os_now()); /* model->setAnmMtx(jntNo, now) */
        }
    }
    return TRUE;
}
VERIFY(0x022A9AB8, nodeCallBack);

/* 022A9C74 */
BOOL daNpc_Os_c::jointCheck(s8 param_1) {
    WWHD_FUNC(0x022A9C74, BOOL, this, param_1);
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
VERIFY(0x022A9C74, &daNpc_Os_c::jointCheck);

/* 022A9CD4 */
static BOOL tunoNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022A9CD4, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = os_j3dSys_model();
        daNpc_Os_c* i_this = gabi::at<daNpc_Os_c>(gabi::load<u32>(model + 0xB8));
        if (i_this != nullptr) {
            J3DJoint* joint = J3DNode_toJoint(node);
            u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
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
VERIFY(0x022A9CD4, tunoNodeCallBack);

#define OS_SAFESTRING_VTBL 0x1001F34C /* this TU's sead::SafeString vtable */
#define OS_ARC STR(0x1001F560)        /* "Os" */
/* dComIfGs_isEventBit / onEventBit: dSv_event_c at save info + 0x644 */
static inline BOOL os_isEventBit(u16 flag) { return gabi::call<BOOL>(0x025B8B94, gabi::load<u32>(0x101F84DC) + 0x644, flag); }
/* 025E8154 mDoExt_brkAnm::init(data, key, anmPlay, mode, rate, start, end, modify, entry(stack)) */
static inline BOOL os_brkAnm_init(mDoExt_brkAnm_os* a, J3DModelData* d, void* key, bool play, s32 mode, f32 rate, s16 start,
                                  s16 end, u32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, key, play, mode, rate, start, end, modify, entry);
}
static inline J3DModelData* os_modelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }

/* 022A9E0C */
BOOL daNpc_Os_c::wakeupCheck() {
    WWHD_FUNC(0x022A9E0C, BOOL, this);
    if (argument == 0) {
        if (os_isEventBit(0x1780)) return TRUE;
    } else if (argument == 1) {
        if (os_isEventBit(0x1740)) return TRUE;
    } else if (argument == 2) {
        if (os_isEventBit(0x1720)) return TRUE;
    }
    return FALSE;
}
VERIFY(0x022A9E0C, &daNpc_Os_c::wakeupCheck);

/* 022A9EAC */
BOOL daNpc_Os_c::finishCheck() {
    WWHD_FUNC(0x022A9EAC, BOOL, this);
    if (argument == 0) {
        if (os_isEventBit(0x1710)) return TRUE;
    } else if (argument == 1) {
        if (os_isEventBit(0x1704)) return TRUE;
    } else if (argument == 2) {
        if (os_isEventBit(0x1B01)) return TRUE;
    }
    return FALSE;
}
VERIFY(0x022A9EAC, &daNpc_Os_c::finishCheck);

/* 022A9F4C */
BOOL daNpc_Os_c::initBrkAnm(u8 param_1, bool param_2) {
    WWHD_FUNC(0x022A9F4C, BOOL, this, param_1, param_2);
    J3DModelData* modelData = os_modelData(mpMorf->getModel());
    BOOL ret = FALSE;
    if ((u32)(s32)field_0x7A2 != (u32)param_1) {
        u32 e = 0x101C2A24 + param_1 * 0x10; /* brkAnmTbl[param_1] */
        void* a_brk = dComIfG_getObjectRes(OS_ARC, gabi::load<u8>(e), OS_SAFESTRING_VTBL);
        if (a_brk == nullptr) {
            JUT_ASSERT_fail(STR(0x1001F564), 0xBF2, STR(0x1001F574)); /* a_brk != NULL */
        }
        if (os_brkAnm_init(&mBrkAnm, modelData, a_brk, true, gabi::load<s32>(e + 4), gabi::load<f32>(e + 8), 0, -1,
                           param_2, 0)) {
            field_0x7A2 = (s8)param_1;
            if (gabi::load<s32>(e + 0xC) < 0) {
                f32 end = (f32)(s16)gabi::load<s16>(gabi::ea(&mBrkAnm) + 0xA); /* setFrame(getEndFrame()) */
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
VERIFY(0x022A9F4C, &daNpc_Os_c::initBrkAnm);

/* 022AA42C */
s8 daNpc_Os_c::getRestartNumber() {
    WWHD_FUNC(0x022AA42C, s8, this);
    if (argument == 0) return 3;
    if (argument == 1) return 4;
    if (argument == 2) return 5;
    return 0;
}
VERIFY(0x022AA42C, &daNpc_Os_c::getRestartNumber);

/* J3DModelData::getJointName()->getIndex(name) (HD inline: 027F68FC returns the joint-tree header, whose
 * +0x10 is a self-relative offset to the JUTNameTab; 027DF9B0 JUTNameTab::getIndex) */
static inline s32 os_getJointIndex(J3DModelData* md, u32 name) {
    u32 t = gabi::call<u32>(0x027F68FC, md);
    u32 off = gabi::load<u32>(t + 0x10);
    u32 tab = off != 0 ? t + 0x10 + off : 0;
    return gabi::call<s32>(0x027DF9B0, tab, name);
}
/* J3DModelData::getJointNodePointer(i) (HD inline): count +4, nodes (0x1C) at +8; out of range: node 0 */
static inline u32 os_jointNode(J3DModelData* md, u32 i) {
    u32 num = gabi::load<u32>(gabi::ea(md) + 4);
    u32 base = gabi::load<u32>(gabi::ea(md) + 8);
    return i < num ? base + i * 0x1C : base;
}

/* 022AA080 */
BOOL daNpc_Os_c::createHeap() {
    WWHD_FUNC(0x022AA080, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001F598) /* "Os" */, 9 /* BDL_OS */, OS_SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1001F5C0), 0x2FE, STR(0x1001F5D0)); /* modelData != NULL */
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1001F598), 6 /* BCK_OS_MOVE01 */, OS_SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11000002);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    s16 headIdx = (s16)os_getJointIndex(modelData, 0x1001F5E4 /* "head_joint" */);
    if (headIdx >= 0) {
        gabi::store<u32>(os_jointNode(modelData, (u16)headIdx) + 8, 0x022A9AB8 /* nodeCallBack */);
    }
    mTuno1JointIdx = (s8)os_getJointIndex(modelData, 0x1001F59C /* "tuno1_joint" */);
    if (mTuno1JointIdx >= 0) {
        gabi::store<u32>(os_jointNode(modelData, (u16)(s8)mTuno1JointIdx) + 8, 0x022A9CD4 /* tunoNodeCallBack */);
    }
    mTuno2JointIdx = (s8)os_getJointIndex(modelData, 0x1001F5A8 /* "tuno2_joint" */);
    if (mTuno2JointIdx >= 0) {
        gabi::store<u32>(os_jointNode(modelData, (u16)(s8)mTuno2JointIdx) + 8, 0x022A9CD4);
    }
    mTuno3JointIdx = (s8)os_getJointIndex(modelData, 0x1001F5B4 /* "tuno3_joint" */);
    if (mTuno3JointIdx >= 0) {
        gabi::store<u32>(os_jointNode(modelData, (u16)(s8)mTuno3JointIdx) + 8, 0x022A9CD4);
    }

    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */

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
    gabi::store<f32>(gabi::ea(&mAcch) + 0xC0, 120.0f); /* SetRoofCrrHeight(120.0f) */
    mAcch.m_flags = (f | 0x2000 /* OnLineCheck */) & ~8u /* ClrRoofNone */;
    return TRUE;
}
VERIFY(0x022AA080, &daNpc_Os_c::createHeap);

/* 022AA428 (unnamed) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022AA428, BOOL, i_this);
    return ((daNpc_Os_c*)i_this)->createHeap();
}
VERIFY(0x022AA428, CheckCreateHeap);

/* 022AA464 */
void daNpc_Os_c::setBaseMtx() {
    WWHD_FUNC(0x022AA464, void, this);
    J3DModel* pModel = mpMorf->getModel();
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
VERIFY(0x022AA464, &daNpc_Os_c::setBaseMtx);

/* 022AA630 */
BOOL daNpc_Os_c::setAction(ProcFunc_l* cur, ProcFunc_l* newFunc, void* arg) {
    WWHD_FUNC(0x022AA630, BOOL, this, cur, newFunc, arg);
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
    field_0x7A9 = -1;
    md_pmf_call<BOOL>(this, cur, arg);
set:
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
VERIFY(0x022AA630, &daNpc_Os_c::setAction);

/* 022AA778 */
void daNpc_Os_c::setNpcAction(ProcFunc_l* actionFunc, void* arg) {
    WWHD_FUNC(0x022AA778, void, this, actionFunc, arg);
    mPlayerAction.d = 0;
    mPlayerAction.i = 0;
    mPlayerAction.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mNpcAction, fn, arg);
}
VERIFY(0x022AA778, &daNpc_Os_c::setNpcAction);

/* 022AA7C8 */
BOOL daNpc_Os_c::dNpc_Os_setAnm(mDoExt_McaMorf* pMorf, int loopMode, f32 morf, f32 playSpeed, int idx, const char* arcName) {
    WWHD_FUNC(0x022AA7C8, BOOL, this, pMorf, loopMode, morf, playSpeed, idx, arcName);
    BOOL ret = FALSE;
    if (pMorf != nullptr) {
        J3DAnmTransform* pAnimRes = (J3DAnmTransform*)dComIfG_getObjectRes(arcName, idx, OS_SAFESTRING_VTBL);
        pMorf->setAnm(pAnimRes, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
        ret = TRUE;
    }
    return ret;
}
VERIFY(0x022AA7C8, &daNpc_Os_c::dNpc_Os_setAnm);

/* 022AA88C */
void daNpc_Os_c::setAnm(int param_1) {
    WWHD_FUNC(0x022AA88C, void, this, param_1);
    field_0x78C = param_1;
    u32 prm = 0x101C2AB8 + param_1 * 0x14; /* l_anmPrm[param_1] */
    s8 idx = gabi::load<s8>(prm);
    f32 playSpeed = gabi::load<f32>(prm + 0xC);
    if (idx != field_0x7A0 || !(playSpeed == gabi::load<f32>(gabi::ea(mpMorf.get()) + 0x98) /* getPlaySpeed */)) {
        field_0x7A0 = idx;
        mReachedAnimEnd = 0;
        mPrevMorfFrame = 0.0f;
        s8 anm = gabi::load<s8>(0x101C2AB4 + idx); /* l_anmTbl */
        dNpc_Os_setAnm(mpMorf, gabi::load<s32>(prm + 4), gabi::load<f32>(prm + 8), playSpeed, anm, STR(0x1001F5FC) /* "Os" */);
        if (gabi::load<s32>(prm + 0x10) < 0) {
            mDoExt_McaMorf* m = mpMorf;
            f32 end = (f32)(s16)gabi::load<s16>(gabi::ea(m) + 0xA2); /* getEndFrame */
            gabi::store<f32>(gabi::ea(m) + 0x9C, (f32)(s16)gabi::ftoi(end)); /* setFrame */
        }
    }
}
VERIFY(0x022AA88C, &daNpc_Os_c::setAnm);

/* m_smoke_tevstr = tevStr (dKy_tevstr_c::operator=, HD: member-wise; m_smoke_tevstr at 0x10475460) */
static inline void os_copy_tevstr(daNpc_Os_c* self) {
    u32 a = gabi::ea(self);
    u32 T = 0x10475460;
    gabi::store<f32>(T + 0x0, gabi::load<f32>(a + 0x110));
    gabi::store<f32>(T + 0x4, gabi::load<f32>(a + 0x114));
    gabi::store<f32>(T + 0x8, gabi::load<f32>(a + 0x118));
    gabi::store<f32>(T + 0xC, gabi::load<f32>(a + 0x11C));
    gabi::store<f32>(T + 0x10, gabi::load<f32>(a + 0x120));
    gabi::store<f32>(T + 0x14, gabi::load<f32>(a + 0x124));
    gabi::store<u8>(T + 0x18, gabi::load<u8>(a + 0x128));
    gabi::store<u8>(T + 0x19, gabi::load<u8>(a + 0x129));
    gabi::store<u8>(T + 0x1A, gabi::load<u8>(a + 0x12A));
    gabi::store<u8>(T + 0x1B, gabi::load<u8>(a + 0x12B));
    gabi::store<u16>(T + 0x1C, gabi::load<u16>(a + 0x12C));
    gabi::store<u16>(T + 0x1E, gabi::load<u16>(a + 0x12E));
    gabi::store<u16>(T + 0x20, gabi::load<u16>(a + 0x130));
    gabi::store<u16>(T + 0x22, gabi::load<u16>(a + 0x132));
    gabi::store<f32>(T + 0x24, gabi::load<f32>(a + 0x134));
    gabi::store<f32>(T + 0x28, gabi::load<f32>(a + 0x138));
    gabi::store<f32>(T + 0x2C, gabi::load<f32>(a + 0x13C));
    gabi::store<f32>(T + 0x30, gabi::load<f32>(a + 0x140));
    gabi::store<f32>(T + 0x34, gabi::load<f32>(a + 0x144));
    gabi::store<f32>(T + 0x38, gabi::load<f32>(a + 0x148));
    gabi::store<f32>(T + 0x3C, gabi::load<f32>(a + 0x14C));
    gabi::store<f32>(T + 0x40, gabi::load<f32>(a + 0x150));
    gabi::store<u32>(T + 0x84, gabi::load<u32>(a + 0x194));
    gabi::store<u32>(T + 0x88, gabi::load<u32>(a + 0x198));
    gabi::store<u32>(T + 0x8C, gabi::load<u32>(a + 0x19C));
    gabi::store<u16>(T + 0x90, gabi::load<u16>(a + 0x1A0));
    gabi::store<u16>(T + 0x92, gabi::load<u16>(a + 0x1A2));
    gabi::store<u16>(T + 0x94, gabi::load<u16>(a + 0x1A4));
    gabi::store<u16>(T + 0x96, gabi::load<u16>(a + 0x1A6));
    gabi::store<u32>(T + 0x98, gabi::load<u32>(a + 0x1A8)); /* lswi/stswi 4 bytes */
    gabi::store<u32>(T + 0x9C, gabi::load<u32>(a + 0x1AC));
    gabi::store<u16>(T + 0xA0, gabi::load<u16>(a + 0x1B0));
    gabi::store<u16>(T + 0xA2, gabi::load<u16>(a + 0x1B2));
    gabi::store<u16>(T + 0xA4, gabi::load<u16>(a + 0x1B4));
    gabi::store<u16>(T + 0xA6, gabi::load<u16>(a + 0x1B6));
    gabi::store<f32>(T + 0xA8, gabi::load<f32>(a + 0x1B8));
    gabi::store<f32>(T + 0xAC, gabi::load<f32>(a + 0x1BC));
    gabi::store<f32>(T + 0xB0, gabi::load<f32>(a + 0x1C0));
    gabi::store<u8>(T + 0xB4, gabi::load<u8>(a + 0x1C4));
    gabi::store<u8>(T + 0xB5, gabi::load<u8>(a + 0x1C5));
    gabi::store<u8>(T + 0xB6, gabi::load<u8>(a + 0x1C6));
    gabi::store<u8>(T + 0xB7, gabi::load<u8>(a + 0x1C7));
    gabi::store<u8>(T + 0xB8, gabi::load<u8>(a + 0x1C8));
    gabi::store<u8>(T + 0xB9, gabi::load<u8>(a + 0x1C9));
    gabi::store<u8>(T + 0xBA, gabi::load<u8>(a + 0x1CA));
    gabi::store<u8>(T + 0xBB, gabi::load<u8>(a + 0x1CB));
    gabi::store<u8>(T + 0xBC, gabi::load<u8>(a + 0x1CC));
    gabi::store<f32>(T + 0xC0, gabi::load<f32>(a + 0x1D0));
    gabi::store<f32>(T + 0xC4, gabi::load<f32>(a + 0x1D4));
    gabi::store<f32>(T + 0xC8, gabi::load<f32>(a + 0x1D8));
    gabi::store<f32>(T + 0xCC, gabi::load<f32>(a + 0x1DC));
    gabi::store<f32>(T + 0xD0, gabi::load<f32>(a + 0x1E0));
    gabi::store<f32>(T + 0xD4, gabi::load<f32>(a + 0x1E4));
    gabi::store<u8>(T + 0xD8, gabi::load<u8>(a + 0x1E8));
    gabi::store<u8>(T + 0xD9, gabi::load<u8>(a + 0x1E9));
    gabi::store<u8>(T + 0xDA, gabi::load<u8>(a + 0x1EA));
    gabi::store<u8>(T + 0xDB, gabi::load<u8>(a + 0x1EB));
    gabi::store<u16>(T + 0xDC, gabi::load<u16>(a + 0x1EC));
    gabi::store<u16>(T + 0xDE, gabi::load<u16>(a + 0x1EE));
    gabi::store<u16>(T + 0xE0, gabi::load<u16>(a + 0x1F0));
    gabi::store<u16>(T + 0xE2, gabi::load<u16>(a + 0x1F2));
    gabi::store<f32>(T + 0xE4, gabi::load<f32>(a + 0x1F4));
    gabi::store<f32>(T + 0xE8, gabi::load<f32>(a + 0x1F8));
    gabi::store<f32>(T + 0xEC, gabi::load<f32>(a + 0x1FC));
    gabi::store<f32>(T + 0xF0, gabi::load<f32>(a + 0x200));
    gabi::store<f32>(T + 0xF4, gabi::load<f32>(a + 0x204));
    gabi::store<f32>(T + 0xF8, gabi::load<f32>(a + 0x208));
    gabi::store<f32>(T + 0xFC, gabi::load<f32>(a + 0x20C));
    gabi::store<f32>(T + 0x100, gabi::load<f32>(a + 0x210));
    gabi::store<f32>(T + 0x144, gabi::load<f32>(a + 0x254));
    gabi::store<f32>(T + 0x148, gabi::load<f32>(a + 0x258));
    gabi::store<f32>(T + 0x14C, gabi::load<f32>(a + 0x25C));
    gabi::store<f32>(T + 0x150, gabi::load<f32>(a + 0x260));
    gabi::store<f32>(T + 0x154, gabi::load<f32>(a + 0x264));
    gabi::store<f32>(T + 0x158, gabi::load<f32>(a + 0x268));
    gabi::store<u8>(T + 0x15C, gabi::load<u8>(a + 0x26C));
    gabi::store<u8>(T + 0x15D, gabi::load<u8>(a + 0x26D));
    gabi::store<u8>(T + 0x15E, gabi::load<u8>(a + 0x26E));
    gabi::store<u8>(T + 0x15F, gabi::load<u8>(a + 0x26F));
    gabi::store<u16>(T + 0x160, gabi::load<u16>(a + 0x270));
    gabi::store<u16>(T + 0x162, gabi::load<u16>(a + 0x272));
    gabi::store<u16>(T + 0x164, gabi::load<u16>(a + 0x274));
    gabi::store<u16>(T + 0x166, gabi::load<u16>(a + 0x276));
    gabi::store<f32>(T + 0x168, gabi::load<f32>(a + 0x278));
    gabi::store<f32>(T + 0x16C, gabi::load<f32>(a + 0x27C));
    gabi::store<f32>(T + 0x170, gabi::load<f32>(a + 0x280));
    gabi::store<f32>(T + 0x174, gabi::load<f32>(a + 0x284));
    gabi::store<f32>(T + 0x178, gabi::load<f32>(a + 0x288));
    gabi::store<f32>(T + 0x17C, gabi::load<f32>(a + 0x28C));
    gabi::store<f32>(T + 0x180, gabi::load<f32>(a + 0x290));
    gabi::store<f32>(T + 0x184, gabi::load<f32>(a + 0x294));
}
/* 0268A8DC (unnamed, HD-only): registers the Os actor by argument (0..2) in a global table at 0x101F62FC */
static inline void os_hd_register(daNpc_Os_c* a, s32 arg) { gabi::call(0x0268A8DC, a, arg); }
/* 02543F10 dEvent_manager_c::getEventIdx(name, roomNo 0xFF) */
static inline s16 os_getEventIdx(u32 name) { return gabi::call<s16>(0x02543F10, dComIfGp_ea() + 0x52C4, name, 0xFF); }

/* 022AA9A4 */
BOOL daNpc_Os_c::init() {
    WWHD_FUNC(0x022AA9A4, BOOL, this);
    gabi::store<u8>(gabi::ea(this) + 0x38C, 0x27); /* attention_info.distances[fopAc_Attn_TYPE_CARRY_e] */
    field_0x794 = (gabi::load<u32>(gabi::ea(this) + 0xB0) >> 0x10) & 0xFF; /* fopAcM_GetParam */
    os_copy_tevstr(this);
    gabi::store<u32>(0x1047545C, 0x10475460); /* m_smoke.setTevStr(&m_smoke_tevstr) */
    gabi::store<u8>(0x101D5F4B + argument, 0);  /* offPlayerRoom(argument) */
    field_0x784 = 0;                            /* clearStatus() */
    field_0x7A0 = 3;
    gravity = gabi::load<f32>(0x10467ECC);      /* l_HIO gravity */
    field_0x78C = 5;
    if (wakeupCheck()) {
        field_0x784 = field_0x784 | 8;          /* onGravity() */
    } else {
        field_0x784 = field_0x784 & ~8u;        /* offGravity() */
    }
    gabi::Local<ProcFunc_l> fn;
    if (finishCheck()) {
        md_pmf_load(fn, 0x1001F298); /* &daNpc_Os_c::finish02NpcAction */
        setNpcAction(fn, nullptr);
    } else {
        setAnm(0);
        md_pmf_load(fn, 0x1001F260); /* &daNpc_Os_c::waitNpcAction */
        setNpcAction(fn, nullptr);
    }
    field_0x7A5 = -1;
    field_0x748.copy(current.pos);
    mpPedestal = nullptr;
    field_0x754.copy(current.pos);
    field_0x788 = 120.0f;
    field_0x7AA = -1;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101C2920) /* l_cyl_src */);
    mCyl.SetStts(&mStts);
    for (int i = 0; i < 0x10; i++) {
        field_0x7C4[i] = os_getEventIdx(gabi::load<u32>(0x101C2884 + 4 * i)); /* event_name_tbl[i] */
    }
    os_hd_register(this, argument); /* HD */
    return TRUE;
}
VERIFY(0x022AA9A4, &daNpc_Os_c::init);

/* daNpc_Os_c::daNpc_Os_c (inlined into create) */
static inline void os_ct(daNpc_Os_c* p) {
    gabi::call(0x024450B4, p); /* daPy_npc_c::daPy_npc_c */
    p->__vtbl = OS_VTBL;
    gabi::call(0x025E80D0, &p->mBrkAnm); /* mDoExt_brkAnm::mDoExt_brkAnm */
    gabi::call(0x028EFFD0, p->mAcchCir, 2, 0x40, 0x024EFE94); /* __construct_array(dBgS_AcchCir) */
    dCcD_Stts_ct(&p->mStts);
    dCcD_Cyl_ct(&p->mCyl, 0x1001F364 /* this TU's cM3dGAab vtable */);
    gabi::call(0x0259DAA0, &p->mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
    p->field_0x7FC.mpBgW = 0;
    gabi::store<u32>(gabi::ea(&p->field_0x7FC) + 0xC, 0x1001F374); /* cBgS_PolyInfo vtable */
    p->field_0x7FC.mPolyIndex = 0xFFFF;
    p->field_0x738.__vtbl = 0x1001F424;  /* daNpc_Os_infiniteEcallBack_c vtable */
    gabi::store<u32>(gabi::ea(&p->field_0x7FC) + 8, 0xFFFFFFFF); /* actor id */
    p->field_0x7FC.mBgIndex = 0x100;
    p->field_0x740.__vtbl = 0x1001F424;
}

/* 022AAEE0 */
cPhs_State daNpc_Os_c::create() {
    WWHD_FUNC(0x022AAEE0, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Os_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            os_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State result = dComIfG_resLoad(&mPhs, STR(0x1001F5FF) /* "Os" */);
    if (result == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x022AA428 /* CheckCreateHeap */, 0xFA0 /* l_heap_size */)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        if (!finishCheck()) {
            gabi::call(0x02445784, this, (s32)getRestartNumber()); /* daPy_npc_c::checkRestart */
        } else if (argument < 3) {
            u32 t = 0x101C28FC + argument * 0xC; /* l_finish_home_pos[argument] */
            f32 x = gabi::load<f32>(t);
            home.pos.x = x;
            f32 y = gabi::load<f32>(t + 4);
            home.pos.y = y;
            f32 z = gabi::load<f32>(t + 8);
            current.pos.x = x;
            home.pos.z = z;
            current.pos.y = y;
            current.pos.z = z;
        }
        setBaseMtx();
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        if (gabi::load<s8>(0x10467E44) < 0) { /* l_HIO.mNo */
            s8 no = mDoHIO_createChild(STR(0x1001F604) /* "お供石像" */, gabi::at<void>(0x10467E44));
            gabi::store<u32>(0x10467E44 + 0x58, gabi::ea(this)); /* l_HIO.field_0x5C */
            gabi::store<s8>(0x10467E44, no);
            gabi::store<s32>(0x10467DEC, 1); /* l_hio_counter */
        } else {
            gabi::store<s32>(0x10467DEC, gabi::load<s32>(0x10467DEC) + 1);
        }
        if (!init()) {
            return cPhs_ERROR_e;
        }
        fopAcM_setStageLayer(this);
    }
    return result;
}
VERIFY(0x022AAEE0, &daNpc_Os_c::create);

/* 022AB1C0 */
static cPhs_State daNpc_Os_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022AB1C0, cPhs_State, i_this);
    return ((daNpc_Os_c*)i_this)->create();
}
VERIFY(0x022AB1C0, daNpc_Os_Create);

/* fopAcM_seStartCurrent (HD inline, this known non-null): reverb, then mDoAud_seStart at current.pos */
static inline void os_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    mDoAud_seStart(id, &a->current.pos, param, reverb);
}
#define OS_M_PLAYERROOM 0x101D5F4B /* bool daNpc_Os_c::m_playerRoom[3] */

/* 022AB1C4 */
void daNpc_Os_c::checkPlayerRoom() {
    WWHD_FUNC(0x022AB1C4, void, this);
    gabi::store<u8>(OS_M_PLAYERROOM + argument, 0); /* offPlayerRoom(argument) */
    if (wakeupCheck() && !finishCheck()) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        if (current.roomNo == link->current.roomNo) {
            gabi::store<u8>(OS_M_PLAYERROOM + argument, 1); /* onPlayerRoom(argument) */
        }
    }
}
VERIFY(0x022AB1C4, &daNpc_Os_c::checkPlayerRoom);

/* 022AB258 */
static void daNpc_Os_infiniteEcallBack_end(daNpc_Os_infiniteEcallBack_l* cb) {
    WWHD_FUNC(0x022AB258, void, cb);
    if (cb->mpBaseEmitter != nullptr) {
        u32 e = gabi::ea(cb->mpBaseEmitter.get());
        gabi::store<s32>(e + 0x5C, -1); /* becomeInvalidEmitter: stopCreateParticle, maxFrame */
        gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1);
        gabi::store<u32>(gabi::ea(cb->mpBaseEmitter.get()) + 0x1E4, 0); /* setEmitterCallBackPtr(NULL) */
        cb->mpBaseEmitter = nullptr;
    }
}
VERIFY(0x022AB258, daNpc_Os_infiniteEcallBack_end);

/* 022AB28C */
void daNpc_Os_c::endBeam() {
    WWHD_FUNC(0x022AB28C, void, this);
    daNpc_Os_infiniteEcallBack_end(&field_0x738);
    daNpc_Os_infiniteEcallBack_end(&field_0x740);
}
VERIFY(0x022AB28C, &daNpc_Os_c::endBeam);

/* 022AB2BC */
void daNpc_Os_c::setCollision() {
    WWHD_FUNC(0x022AB2BC, void, this);
    gabi::Local<cXyz> temp;
    temp->copy(current.pos);
    mCyl.mCyl.SetC(temp);
    mCyl.mCyl.SetR(30.0f);
    mCyl.mCyl.SetH(field_0x788);
    gabi::store<u8>(gabi::ea(&mStts) + 0x14, dComIfGp_getPlayer(0) == this ? 0xFE : 0xFF); /* mStts.SetWeight */
    cCcS_Set(dComIfG_Ccsp(), &mCyl);
}
VERIFY(0x022AB2BC, &daNpc_Os_c::setCollision);

/* 022AB350 */
void daNpc_Os_c::smokeSet(u16 particle) {
    WWHD_FUNC(0x022AB350, void, this, particle);
    dPa_control_set(dComIfGp_getParticle(), 0, particle, &current.pos, nullptr, nullptr, 0xFF,
                    gabi::at<dPa_levelEcallBack>(0x10475440) /* &m_smoke */, -1, nullptr, nullptr, nullptr);
}
VERIFY(0x022AB350, &daNpc_Os_c::smokeSet);

/* 022AB3C4 */
void daNpc_Os_c::playBrkAnm() {
    WWHD_FUNC(0x022AB3C4, void, this);
    field_0x7A1 = (s8)gabi::call<u32>(0x025E742C, &mBrkAnm); /* mBrkAnm.play() */
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
VERIFY(0x022AB3C4, &daNpc_Os_c::playBrkAnm);

/* 022AB488 */
void daNpc_Os_c::animationPlay() {
    WWHD_FUNC(0x022AB488, void, this);
    u32 mtrlSndId = 0;
    if ((field_0x784 & 0x10) && (mAcch.m_flags & 0x20) /* ChkGroundHit */ &&
        gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xE8) /* ChkPolySafe(m_gnd) */) {
        mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<cBgS_PolyInfo>(gabi::ea(&mAcch) + 0xE8));
    }
    s8 reverb = (s8)dComIfGp_getReverb(current.roomNo);
    mReachedAnimEnd = (s8)mpMorf->play(&eyePos, mtrlSndId, reverb);
    mDoExt_McaMorf* m = mpMorf;
    f32 speed = gabi::load<f32>(gabi::ea(m) + 0x98);
    f32 frame = gabi::load<f32>(gabi::ea(m) + 0x9C);
    if (speed < 0.0f) {
        if (frame > mPrevMorfFrame) {
            mReachedAnimEnd = 1;
        }
    } else if (frame < mPrevMorfFrame) {
        mReachedAnimEnd = 1;
    }
    mPrevMorfFrame = frame;
    if (field_0x78C == 1 && gabi::call<BOOL>(0x027F2BF8, gabi::ea(mpMorf.get()) + 0x98, 17.0f) /* checkFrame(17) */) {
        smokeSet(0xA328 /* ID_AK_ST_OTOMOSMOKE00 */);
    }
    playBrkAnm();
}
VERIFY(0x022AB488, &daNpc_Os_c::animationPlay);

/* 022AB5A4 */
int daNpc_Os_c::getMyStaffId() {
    WWHD_FUNC(0x022AB5A4, int, this);
    if (argument < 3) {
        u32 name = gabi::load<u32>(0x101C28E4 + 4 * argument); /* l_staff_name[argument] */
        return gabi::call<s32>(0x02542D88, dComIfGp_ea() + 0x52C4, name, 0, 0);
    }
    return -1;
}
VERIFY(0x022AB5A4, &daNpc_Os_c::getMyStaffId);

/* 022AB618 */
void daNpc_Os_c::setFinish() {
    WWHD_FUNC(0x022AB618, void, this);
    u32 ev = gabi::load<u32>(0x101F84DC) + 0x644;
    if (argument == 0) {
        gabi::call(0x025B8B68, ev, 0x1710);
    } else if (argument == 1) {
        gabi::call(0x025B8B68, ev, 0x1704);
    } else if (argument == 2) {
        gabi::call(0x025B8B68, ev, 0x1B01);
    }
}
VERIFY(0x022AB618, &daNpc_Os_c::setFinish);

/* 022AB670 */
BOOL daNpc_Os_c::checkCommandTalk() {
    WWHD_FUNC(0x022AB670, BOOL, this);
    return gabi::load<u16>(gabi::ea(this) + 0xF8) == 1; /* dComIfGp_event_chkTalkXY: eventInfo.mCommand == TALK */
}
VERIFY(0x022AB670, &daNpc_Os_c::checkCommandTalk);

/* 022AB684 */
void daNpc_Os_c::returnLinkPlayer() {
    WWHD_FUNC(0x022AB684, void, this);
    gabi::call(0x023D4688, this, dComIfGp_getLinkPlayer()); /* daPy_py_c::changePlayer */
    mNoResetFlg1 = mNoResetFlg1 & ~2u; /* offNpcCallCommand */
    initBrkAnm(6, true);
}
VERIFY(0x022AB684, &daNpc_Os_c::returnLinkPlayer);

/* 022B01A4 */
static void* daNpc_Os_HIO2_c_ct(void* p) {
    WWHD_FUNC(0x022B01A4, void*, p);
    if (p == nullptr) {
        p = operator_new(0x2C);
        if (p == nullptr) return p;
    }
    u32 h = gabi::ea(p); /* HD: fields from +0 (GameCube +4), vtable at the end (+0x28) */
    gabi::store<f32>(h + 0x18, 0.6f);
    gabi::store<s16>(h + 0x20, 3000);
    gabi::store<f32>(h + 0x8, 2.5f);
    gabi::store<u32>(h + 0x28, 0x1001F464); /* vtable */
    gabi::store<f32>(h + 0x1C, 1.0f);
    gabi::store<s16>(h + 0x24, 5);
    gabi::store<f32>(h + 0x14, 2.5f);
    gabi::store<f32>(h + 0xC, 1.8f);
    gabi::store<f32>(h + 0x4, 17.0f);
    gabi::store<f32>(h + 0x10, 0.6f);
    gabi::store<s16>(h + 0x22, 1000);
    gabi::store<f32>(h + 0x0, 3.5f);
    return p;
}
VERIFY(0x022B01A4, daNpc_Os_HIO2_c_ct);

/* 022B024C */
static void* daNpc_Os_HIO_c_ct(void* p) {
    WWHD_FUNC(0x022B024C, void*, p);
    if (p == nullptr) {
        p = operator_new(0xB4);
        if (p == nullptr) return p;
    }
    u32 h = gabi::ea(p); /* HD: mNo at +0, mOs2 +4, mNpc +0x30, fields GameCube - 4, vtable at +0xB0 */
    gabi::store<u32>(h + 0xB0, 0x1001F474); /* vtable */
    daNpc_Os_HIO2_c_ct(gabi::at<void>(h + 4));
    gabi::call(0x0259DA18, h + 0x30); /* dNpc_HIO_c::dNpc_HIO_c */
    gabi::store<s16>(h + 0x36, 0);
    gabi::store<s16>(h + 0x40, -5000);
    gabi::store<s16>(h + 0x3E, 0);
    gabi::store<f32>(h + 0x50, 200.0f);
    gabi::store<s16>(h + 0x3C, 0);
    gabi::store<f32>(h + 0x64, 0.05f);
    gabi::store<f32>(h + 0x6C, 0.0f);
    gabi::store<s8>(h + 0x4E, 0);
    gabi::store<f32>(h + 0x74, 0.2f);
    gabi::store<f32>(h + 0x68, 17.0f);
    gabi::store<f32>(h + 0x5C, 80.0f);
    gabi::store<f32>(h + 0x60, 500.0f);
    gabi::store<s16>(h + 0x3A, 0);
    gabi::store<f32>(h + 0x70, 0.5f);
    gabi::store<s16>(h + 0x4C, 0x4000);
    gabi::store<s16>(h + 0x42, 0);
    gabi::store<s16>(h + 0x44, 0x1000);
    gabi::store<f32>(h + 0x30, -20.0f);
    gabi::store<f32>(h + 0x48, 130.0f);
    gabi::store<s16>(h + 0x46, 0x800);
    gabi::store<s16>(h + 0x38, 0x1388);
    gabi::store<f32>(h + 0x7C, 15.0f);
    gabi::store<f32>(h + 0xA8, 18.0f);
    gabi::store<f32>(h + 0x94, -0.56152f);
    gabi::store<f32>(h + 0x88, 4.0f);
    gabi::store<s16>(h + 0x34, 0);
    gabi::store<f32>(h + 0xA4, 0.75f);
    gabi::store<u32>(h + 0x58, 0);
    gabi::store<f32>(h + 0x80, 22.0f);
    gabi::store<f32>(h + 0x78, 0.9f);
    gabi::store<f32>(h + 0xA0, 10.0f);
    gabi::store<f32>(h + 0xAC, 8.0f);
    gabi::store<f32>(h + 0x9C, 0.2f);
    gabi::store<s8>(h + 0x0, -1);
    gabi::store<f32>(h + 0x98, 0.1f);
    gabi::store<f32>(h + 0x8C, -1.51367f);
    gabi::store<f32>(h + 0x90, -1.09863f);
    return p;
}
VERIFY(0x022B024C, daNpc_Os_HIO_c_ct);

/* 022B0408: static initialisation (header statics, m_smoke culling box, l_HIO, l_smoke_scale) */
static void __sinit_d_a_npc_os_cpp() {
    WWHD_FUNC(0x022B0408, void, (u32)0);
    for (int i = 3; i >= 0; i--) gabi::store<u32>(0x10467E10 + 4 * i, 0);
    __register_global_object(0x101C2B2C);
    gabi::store<f32>(0x10467DF4, -3.1415927f);
    gabi::store<f32>(0x10467DF8, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10467E0C);
    __register_global_object(0x101C2B38);
    gabi::call(0x028EAB2C, 0x10467E0D);
    __register_global_object(0x101C2B44);
    gabi::store<f32>(0x10467DFC, 50000.0f);
    gabi::store<f32>(0x10467E08, 10000.0f);
    gabi::store<f32>(0x10467E00, 50000.0f);
    gabi::store<f32>(0x10467E04, 10000.0f);
    daNpc_Os_HIO_c_ct(gabi::at<void>(0x10467E44)); /* l_HIO */
    gabi::store<f32>(0x10467E20, 0.5f); /* l_smoke_scale */
    gabi::store<f32>(0x10467E28, 0.5f);
    gabi::store<f32>(0x10467E24, 0.5f);
}
VERIFY(0x022B0408, __sinit_d_a_npc_os_cpp);

/* 022B04F0 sead::SafeString deleting destructor (this TU's vtable) */
static void os_SafeString_dtor(void* p, s32 flags) {
    WWHD_FUNC(0x022B04F0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x022B04F0, os_SafeString_dtor);

/* 022B0504 */
static BOOL daNpc_Os_Delete(daNpc_Os_c* i_this) {
    WWHD_FUNC(0x022B0504, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x022B0504, daNpc_Os_Delete);

/* 022B050C: dBgS_AcchCir deleting destructor (this TU's copy) */
static void os_AcchCir_dtor(void* p, s32 flags) {
    WWHD_FUNC(0x022B050C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x02018034, gabi::ea(p) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x022B050C, os_AcchCir_dtor);

/* 022B0638: daNpc_Os_infiniteEcallBack_c::setup(emitter, ...) */
static void daNpc_Os_infiniteEcallBack_setup(daNpc_Os_infiniteEcallBack_l* cb, u32 emitter) {
    WWHD_FUNC(0x022B0638, void, cb, emitter);
    gabi::store<u32>(gabi::ea(cb) + 4, emitter);
}
VERIFY(0x022B0638, daNpc_Os_infiniteEcallBack_setup);

/* 022B0640: daNpc_Os_c::getGroundY() (mAcch.GetGroundH()) */
static f32 os_getGroundY(daNpc_Os_c* p) {
    WWHD_FUNC(0x022B0640, f32, p);
    return gabi::load<f32>(gabi::ea(p) + 0x4D0);
}
VERIFY(0x022B0640, os_getGroundY);

/* 022B0648 / 022B0650: getLeftHandMatrix / getRightHandMatrix (cullMtx) */
static u32 os_getLeftHandMatrix(daNpc_Os_c* p) {
    WWHD_FUNC(0x022B0648, u32, p);
    return p->cullMtx;
}
VERIFY(0x022B0648, os_getLeftHandMatrix);
static u32 os_getRightHandMatrix(daNpc_Os_c* p) {
    WWHD_FUNC(0x022B0650, u32, p);
    return p->cullMtx;
}
VERIFY(0x022B0650, os_getRightHandMatrix);

/* 022B0658 / 022B0664: getBaseAnimeFrameRate (1.0) / getBaseAnimeFrame (0.0) */
static f32 os_getBaseAnimeFrameRate(daNpc_Os_c*) {
    WWHD_FUNC(0x022B0658, f32, (u32)0);
    return 1.0f;
}
VERIFY(0x022B0658, os_getBaseAnimeFrameRate);
static f32 os_getBaseAnimeFrame(daNpc_Os_c*) {
    WWHD_FUNC(0x022B0664, f32, (u32)0);
    return 0.0f;
}
VERIFY(0x022B0664, os_getBaseAnimeFrame);

/* 022B0560..022B0634: this TU's copies of the inline virtual defaults of daPy_py_c / daPy_npc_c,
 * dPa_levelEcallBack and sead::SafeString (empty, return 0, or return -1) */

static void os_empty_022B0560() { WWHD_FUNC(0x022B0560, void, (u32)0); }
VERIFY(0x022B0560, os_empty_022B0560);
static void os_empty_022B0564() { WWHD_FUNC(0x022B0564, void, (u32)0); }
VERIFY(0x022B0564, os_empty_022B0564);
static void os_empty_022B0568() { WWHD_FUNC(0x022B0568, void, (u32)0); }
VERIFY(0x022B0568, os_empty_022B0568);
static void os_empty_022B05BC() { WWHD_FUNC(0x022B05BC, void, (u32)0); }
VERIFY(0x022B05BC, os_empty_022B05BC);
static void os_empty_022B0600() { WWHD_FUNC(0x022B0600, void, (u32)0); }
VERIFY(0x022B0600, os_empty_022B0600);
static void os_empty_022B0604() { WWHD_FUNC(0x022B0604, void, (u32)0); }
VERIFY(0x022B0604, os_empty_022B0604);
static void os_empty_022B0608() { WWHD_FUNC(0x022B0608, void, (u32)0); }
VERIFY(0x022B0608, os_empty_022B0608);
static void os_empty_022B0614() { WWHD_FUNC(0x022B0614, void, (u32)0); }
VERIFY(0x022B0614, os_empty_022B0614);
static void os_empty_022B0618() { WWHD_FUNC(0x022B0618, void, (u32)0); }
VERIFY(0x022B0618, os_empty_022B0618);
static void os_empty_022B061C() { WWHD_FUNC(0x022B061C, void, (u32)0); }
VERIFY(0x022B061C, os_empty_022B061C);
static void os_empty_022B0630() { WWHD_FUNC(0x022B0630, void, (u32)0); }
VERIFY(0x022B0630, os_empty_022B0630);
static void os_empty_022B0634() { WWHD_FUNC(0x022B0634, void, (u32)0); }
VERIFY(0x022B0634, os_empty_022B0634);
static void os_empty_022B0670() { WWHD_FUNC(0x022B0670, void, (u32)0); }
VERIFY(0x022B0670, os_empty_022B0670);
static BOOL os_ret0_022B0574() { WWHD_FUNC(0x022B0574, BOOL, (u32)0); return 0; }
VERIFY(0x022B0574, os_ret0_022B0574);
static BOOL os_ret0_022B057C() { WWHD_FUNC(0x022B057C, BOOL, (u32)0); return 0; }
VERIFY(0x022B057C, os_ret0_022B057C);
static BOOL os_ret0_022B0584() { WWHD_FUNC(0x022B0584, BOOL, (u32)0); return 0; }
VERIFY(0x022B0584, os_ret0_022B0584);
static BOOL os_ret0_022B058C() { WWHD_FUNC(0x022B058C, BOOL, (u32)0); return 0; }
VERIFY(0x022B058C, os_ret0_022B058C);
static BOOL os_ret0_022B0594() { WWHD_FUNC(0x022B0594, BOOL, (u32)0); return 0; }
VERIFY(0x022B0594, os_ret0_022B0594);
static BOOL os_ret0_022B059C() { WWHD_FUNC(0x022B059C, BOOL, (u32)0); return 0; }
VERIFY(0x022B059C, os_ret0_022B059C);
static BOOL os_ret0_022B05A4() { WWHD_FUNC(0x022B05A4, BOOL, (u32)0); return 0; }
VERIFY(0x022B05A4, os_ret0_022B05A4);
static BOOL os_ret0_022B05AC() { WWHD_FUNC(0x022B05AC, BOOL, (u32)0); return 0; }
VERIFY(0x022B05AC, os_ret0_022B05AC);
static BOOL os_ret0_022B05B4() { WWHD_FUNC(0x022B05B4, BOOL, (u32)0); return 0; }
VERIFY(0x022B05B4, os_ret0_022B05B4);
static BOOL os_ret0_022B05C0() { WWHD_FUNC(0x022B05C0, BOOL, (u32)0); return 0; }
VERIFY(0x022B05C0, os_ret0_022B05C0);
static BOOL os_ret0_022B05E0() { WWHD_FUNC(0x022B05E0, BOOL, (u32)0); return 0; }
VERIFY(0x022B05E0, os_ret0_022B05E0);
static BOOL os_ret0_022B05E8() { WWHD_FUNC(0x022B05E8, BOOL, (u32)0); return 0; }
VERIFY(0x022B05E8, os_ret0_022B05E8);
static BOOL os_ret0_022B05F0() { WWHD_FUNC(0x022B05F0, BOOL, (u32)0); return 0; }
VERIFY(0x022B05F0, os_ret0_022B05F0);
static BOOL os_ret0_022B05F8() { WWHD_FUNC(0x022B05F8, BOOL, (u32)0); return 0; }
VERIFY(0x022B05F8, os_ret0_022B05F8);
static BOOL os_ret0_022B060C() { WWHD_FUNC(0x022B060C, BOOL, (u32)0); return 0; }
VERIFY(0x022B060C, os_ret0_022B060C);
static BOOL os_ret0_022B0620() { WWHD_FUNC(0x022B0620, BOOL, (u32)0); return 0; }
VERIFY(0x022B0620, os_ret0_022B0620);
static s32 os_retm1_022B056C() { WWHD_FUNC(0x022B056C, s32, (u32)0); return -1; }
VERIFY(0x022B056C, os_retm1_022B056C);
static s32 os_retm1_022B05C8() { WWHD_FUNC(0x022B05C8, s32, (u32)0); return -1; }
VERIFY(0x022B05C8, os_retm1_022B05C8);
static s32 os_retm1_022B05D0() { WWHD_FUNC(0x022B05D0, s32, (u32)0); return -1; }
VERIFY(0x022B05D0, os_retm1_022B05D0);
static s32 os_retm1_022B05D8() { WWHD_FUNC(0x022B05D8, s32, (u32)0); return -1; }
VERIFY(0x022B05D8, os_retm1_022B05D8);
static BOOL os_ret1_022B0628() { WWHD_FUNC(0x022B0628, BOOL, (u32)0); return 1; }
VERIFY(0x022B0628, os_ret1_022B0628);
