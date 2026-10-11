/**
 * d_a_npc_kg2.cpp (WWHD)
 * NPC - Salvatore (Cannon Minigame, Windfall)
 *
 * The GameCube decompilation of this unit is only
 * Nonmatching placeholders, so every function is written from the WWHD code (disassembly and
 * the recompiled C), keeping the GameCube names and member order.
 */
#include "d/actor/d_a_npc_kg2.h"
#include <cmath>

static s32 l_btpIndices[5] = {9, 11, 13, 12, 14};
static f32 l_morfFrames[15] = {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
                               8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f};
static s32 l_loopModes[15] = {2, 2, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 0, 0, 2};
static f32 l_anmSpeeds[15] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
                              1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f};
static s32 l_anmIndices[15] = {17, 26, 25, 20, 22, 18, 19, 21, 23, 24, 27, 28, 20, 22, 29};
static u8 l_prizes[3] = {7, 7, 215};

// HD: private adapters preserve the verified ABI widths and access order.
static u32 play() { return dComIfGp_ea(); }
static u32 evtMng() { return gabi::ea(&kg2Play(play())->play.mEvtManager); }
static u32 saveInfo() { return dComIfGs_ea(); }
static u32 envLight() { return Kg2Core_envLight(); }
static BOOL checkPass(u32 frameCtrl, f32 frame) {
    return Kg2Core_checkPass(kg2Address<J3DFrameCtrl>(frameCtrl), frame);
}
static u32 getEventReg(u32 save, u32 reg) {
    return Kg2Core_getEventReg(kg2Address<dSv_event_c>(save), reg);
}
static BOOL isEventBit(u32 save, u32 bit) {
    return Kg2Core_isEventBit(kg2Address<dSv_event_c>(save), bit);
}
static void onEventBit(u32 save, u32 bit) {
    Kg2Core_onEventBit(kg2Address<dSv_event_c>(save), bit);
}
static void* getMySubstanceP(s32 staff, u32 name, s32 type) {
    return Kg2Core_getSubstance(kg2Address<dEvent_manager_c>(evtMng()), staff,
                                kg2Address<const char>(name), type);
}
// HD: preserve the 8-byte SafeString resource-name temporary.
static u32 objectRes(u32 name, s32 index) {
    return gabi::ea(dComIfG_getObjectRes(kg2String(name), index, Kg2SafeStringVtable));
}
static void copyWords(u32 dst, u32 src, int n) {
    u32 v[12];
    for (int i = 0; i < n; ++i)
        v[i] = (u32)kg2Words(src)->mWord[i];
    for (int i = 0; i < n; ++i)
        kg2Words(dst)->mWord[i] = v[i];
}
/* lfs/stfs copy (matrices): float stores, so the harness's NaN payload rule applies */
static void copyFloats(u32 dst, u32 src, int n) {
    f32 v[12];
    for (int i = 0; i < n; ++i)
        v[i] = (f32)kg2Floats(src)->mValue[i];
    for (int i = 0; i < n; ++i)
        kg2Floats(dst)->mValue[i] = v[i];
}
/* J3DModel joint matrix i (marks the matrix block dirty) */
static u32 jointMtx(u32 model, u32 offs) {
    u32 block = (u32)gabi::ea((J3DMtxBuffer_hd*)kg2Model(model)->mpMtxBuffer);
    u16 flags = (u16)kg2Buffer(block)->mFlags;
    u32 base = (u32)kg2Buffer(block)->mpAnmMtx;
    kg2Buffer(block)->mFlags = flags | 0x10;
    return base + offs;
}
static f32 endFrameMinus1(u32 morf) { return (f32)(s16)kg2Morf(morf)->mFrameCtrl.mEnd - 1.0f; }

// HD: GHS PTMF comparison and its zero-index initialization path retain their store order.
static void setAction(daNpc_Kg2_c* a, u32 fn) {
    s16 i = a->mAction.i;
    if (i == -1) {
        if (a->mAction.d == 0 && a->mAction.f == fn)
            return;
    } else if (i == 0) {
        a->mActionState = 0;
        a->mAction.f = fn;
        a->mAction.d = 0;
        a->mAction.i = -1;
        (a->*a->mAction)(0);
        return;
    }
    a->mActionState = -1;
    (a->*a->mAction)(0);
    a->mAction.d = 0;
    a->mAction.i = -1;
    a->mAction.f = fn;
    a->mActionState = 0;
    (a->*a->mAction)(0);
}

/* 0226A8B8 */
daNpc_Kg2_HIO_c* daNpc_Kg2_HIO_ct(daNpc_Kg2_HIO_c* self) {
    u32 p = gabi::ea(self);
    if (!p) {
        p = Kg2Core_allocate(0x34);
        if (!p)
            return nullptr;
    }
    kg2HioAt(p)->__vtbl = Kg2HioVtable;
    Kg2Core_initHio(&kg2HioAt(p)->mEyeOffsetY);
    kg2HioAt(p)->mJntParam[0] = 2500;
    kg2HioAt(p)->mJntParam[3] = 8000;
    kg2HioAt(p)->mJntParam[5] = -2000;
    kg2HioAt(p)->mJntParam[4] = -2500;
    kg2HioAt(p)->mJntParam[1] = 2000;
    kg2HioAt(p)->mEyeHeight = 35.0f;
    kg2HioAt(p)->mDrawDebug = 0;
    kg2HioAt(p)->mJntParam[6] = -7000;
    kg2HioAt(p)->mJntParam[8] = 0x1000;
    kg2HioAt(p)->mLookSpeed = 1000;
    kg2HioAt(p)->mNo = -1;
    kg2HioAt(p)->mJntParam[2] = 7000;
    kg2HioAt(p)->mAttentionDist = 400.0f;
    kg2HioAt(p)->mAttentionDebug = 0;
    kg2HioAt(p)->mEyeOffsetY = -20.0f;
    kg2HioAt(p)->mAttentionAngle = 0x4000;
    kg2HioAt(p)->mJntParam[7] = -8000;
    return kg2HioAt(p);
}

/* 02267B60 */
BOOL daNpc_Kg2_nodeCallBack(J3DNode* node, s32 timing) {
    if (timing != 0)
        return TRUE;
    u32 model = gabi::ea(j3dSys.getModel()); /* j3dSys model */
    u32 actor = (u32)kg2Model(model)->mUserArea;
    if (!actor)
        return TRUE;
    u32 jnt = Kg2Core_jointResource(node);
    u32 jntNo = (u16)kg2JointResource(jnt)->mIndex;
    u32 offs = jntNo * 0x30;
    Kg2Core_copyMatrix(kg2Address<Mtx34>(jointMtx(model, offs)), kg2Matrix()); /* PSMTXCopy */
    if (jntNo == (u32)(s32)(s8)kg2Actor(actor)->m_jnt.mHeadJntNum) {

        f32 gx = (f32)kg2EyeDebug().x, gy = (f32)kg2EyeDebug().y;
        u32 gz = (u32)kg2Words(gabi::ea(&kg2EyeDebug()))->mWord[2];
        cXyz eyeOffs;
        kg2Words(gabi::ea(&eyeOffs))->mWord[2] = gz;
        eyeOffs.x = gabi::fadds_ppc(gx, 24.0f);
        eyeOffs.y = gabi::fadds_ppc(gy, 5.0f);
        if ((u32)kg2DebugOffsets().mHeadGuard == 0) {
            kg2DebugOffsets().mHeadGuard = 1;
            kg2DebugOffsets().mHead.z = 0.0f;
            kg2DebugOffsets().mHead.x = 24.0f;
            kg2DebugOffsets().mHead.y = -16.0f;
        }
        Kg2Core_multVec(kg2Matrix(), &eyeOffs, &kg2Actor(actor)->mHeadPos);      /* PSMTXMultVec */
        Kg2Core_rotateX(kg2Matrix(), (s16)kg2Actor(actor)->m_jnt.mAngles[0][1]); /* mDoMtx_XrotM */
        Kg2Core_rotateZ(kg2Matrix(),
                        (s16) - (s16)kg2Actor(actor)->m_jnt.mAngles[0][0]); /* mDoMtx_ZrotM */
        Kg2Core_multVec(kg2Matrix(), &kg2DebugOffsets().mHead, &kg2Actor(actor)->mAttentionPos);
        Kg2Core_multVec(kg2Matrix(), &eyeOffs, &kg2Actor(actor)->attention_info.position);
        kg2Actor(actor)->attention_info.position.y = gabi::fadds_ppc(
            (f32)kg2Actor(actor)->attention_info.position.y, (f32)kg2Hio().mEyeHeight);
    } else if (jntNo == (u32)(s32)(s8)kg2Actor(actor)->m_jnt.mBackboneJntNum) {
        Kg2Core_rotateX(kg2Matrix(), (s16)kg2Actor(actor)->m_jnt.mAngles[1][1]);
        Kg2Core_rotateZ(kg2Matrix(), (s16) - (s16)kg2Actor(actor)->m_jnt.mAngles[1][0]);
    }
    Kg2Core_copyMatrix(kg2Matrix(), kg2Address<Mtx34>(Kg2CurrentMatrix));
    copyFloats(jointMtx(model, offs), gabi::ea(kg2Matrix()), 12);
    return TRUE;
}

/* 02267E04 */
BOOL daNpc_Kg2_c::initTexPatternAnm(u32 modify) {
    auto* a = this;
    s32 idx = a->mBtpIdx;
    u32 modelData = gabi::ea(a->mpMorf.get()->mpModel.get()->mModelData.get());
    s32 res = (s32)l_btpIndices[idx];
    u32 btp = objectRes(gabi::guest_addr("Kg"), res);
    a->mpBtp = kg2Pattern(btp);
    if (!btp) {
        Kg2Core_assertion("d_a_npc_kg2.cpp", 0x12E, "m_btp != (0)");
        btp = gabi::ea(a->mpBtp.get());
    }
    if (!Kg2Core_initBtp(&a->mBtpAnm, kg2Address<J3DModelData>(modelData),
                         kg2Address<J3DAnmTexPattern>(btp), 1, 2, 1.0f, 0, -1, (u32)modify, 0))
        return FALSE;
    a->mBtpFrame = 0;
    a->mBlinkTimer = 0;
    return TRUE;
}

static u32 jointNameTable(u32 modelData) {
    u32 header = Kg2Core_modelResource(kg2Address<J3DModelData>(modelData));
    u32 offset = (u32)kg2Header(header)->mNameOffset;
    return offset ? header + 0x10 + offset : 0;
}

/* 02267F18 */
BOOL daNpc_Kg2_c::CreateHeap() {
    auto* a = this;
    u32 p = gabi::ea(a);
    u32 data = objectRes(gabi::guest_addr("Kg"), KG2_BDL_BODY);
    if (!data)
        Kg2Core_assertion("d_a_npc_kg2.cpp", 0x395, "modelData != (0)");
    u32 anm = objectRes(gabi::guest_addr("Kg"), KG2_BCK_WAIT);
    u32 morf = Kg2Core_newMorf(0, data, 0, 0, anm, 2, 0, -1, 1.0f, 1, 0, 0, 0x11020203);
    a->mpMorf = kg2Morf(morf);
    if (!morf || !(u32)gabi::ea((J3DModel*)kg2Morf(morf)->mpModel))
        return FALSE;
    s8 jnt = Kg2Core_jointIndex(kg2Address<void>(jointNameTable(data)), "head");
    a->m_jnt.mHeadJntNum = jnt;
    if (jnt < 0)
        Kg2Core_assertion("d_a_npc_kg2.cpp", 0x3A6, "m_jnt.getHeadJntNum() >= 0");
    jnt = Kg2Core_jointIndex(kg2Address<void>(jointNameTable(data)), "backbone2");
    a->m_jnt.mBackboneJntNum = jnt;
    if (jnt < 0)
        Kg2Core_assertion("d_a_npc_kg2.cpp", 0x3AB, "m_jnt.getBackboneJntNum() >= 0");
    jnt = Kg2Core_jointIndex(kg2Address<void>(jointNameTable(data)), "handL");
    a->mItemJntNum = jnt;
    if (jnt < 0)
        Kg2Core_assertion("d_a_npc_kg2.cpp", 0x3AF, "m_handL_num >= 0");
    a->mBtpIdx = 0;
    if (!a->initTexPatternAnm(0))
        return FALSE;
    u32 itemData = objectRes(gabi::guest_addr("Kg"), KG2_BDL_ITEM);
    u32 item = Kg2Core_newModel(kg2Address<J3DModelData>(itemData), 0x80000,
                                0x11020002); /* mDoExt_J3DModel__create */
    a->mpItemModel = kg2Model(item);
    if (!item)
        return FALSE;
    u32 itemBtp = objectRes(gabi::guest_addr("Kg"), KG2_BTP_ITEM);
    if (!Kg2Core_initBtp(&a->mItemBtpAnm, kg2Address<J3DModelData>(itemData),
                         kg2Address<J3DAnmTexPattern>(itemBtp), 1, 2, 0.0f, 0, -1, 0, 0))
        return FALSE;
    u32 modelData = gabi::ea(a->mpMorf.get()->mpModel.get()->mModelData.get());
    u32 idx = (u16)(s32)(s8)a->m_jnt.mHeadJntNum;
    u32 joints = (u32)kg2Data(modelData)->mJointArray;
    if (idx < (u32)kg2Data(modelData)->mJointNum)
        joints += idx * 0x1C;
    kg2Joint(joints)->mCallback =
        gabi::guest_fn(reinterpret_cast<const void*>(daNpc_Kg2_nodeCallBack));
    idx = (u16)(s32)(s8)a->m_jnt.mBackboneJntNum;
    joints = (u32)kg2Data(modelData)->mJointArray;
    if (idx < (u32)kg2Data(modelData)->mJointNum)
        joints += idx * 0x1C;
    kg2Joint(joints)->mCallback =
        gabi::guest_fn(reinterpret_cast<const void*>(daNpc_Kg2_nodeCallBack));
    a->mpMorf.get()->mpModel.get()->mUserArea = p;
    Kg2Core_setWall(&a->mAcchCir, 30.0f, 0.0f); /* dBgS_AcchCir::SetWall */
    Kg2Core_setAcch(&a->mObjAcch, &a->current.pos, &a->old.pos, a, 1, &a->mAcchCir, &a->speed, 0,
                    0); /* dBgS_Acch::Set */
    return TRUE;
}

/* 02268290 */
BOOL daNpc_Kg2_CallbackCreateHeap(daNpc_Kg2_c* a) { return a->CreateHeap(); }

/* 02268294 */
void daNpc_Kg2_c::set_mtx() {
    auto* a = this;
    u32 model = gabi::ea(a->mpMorf.get()->mpModel.get());
    Kg2Core_translateMatrix(kg2Matrix(), (f32)a->current.pos.x, (f32)a->current.pos.y,
                            (f32)a->current.pos.z);        /* PSMTXTrans */
    Kg2Core_rotateY(kg2Matrix(), (s16)a->current.angle.y); /* mDoMtx_YrotM */
    copyFloats(gabi::ea(&kg2Model(model)->mBaseTransformMtx), gabi::ea(kg2Matrix()), 12);
    Kg2Core_calcMorf(a->mpMorf.get()); /* McaMorf::calc */
    if (a->mbItemShown) {
        s32 jnt = a->mItemJntNum;
        Kg2Core_copyMatrix(kg2Address<Mtx34>(jointMtx(model, (u32)(jnt * 0x30))), kg2Matrix());
        Kg2Core_translateMultiply(23.46f, -22.26f, -47.05f);                    /* transM */
        Kg2Core_rotateXYZ(kg2Matrix(), (s16)0x1F4B, (s16)-0x4F00, (s16)0x1F4B); /* XYZrotM */
        copyFloats(gabi::ea(&a->mpItemModel.get()->mBaseTransformMtx), gabi::ea(kg2Matrix()), 12);
    }
}

/* 02268430 */
void daNpc_Kg2_c::setAnm(s8 anm, f32 morf) {
    auto* a = this;
    s32 idx = anm;
    if (morf < 0.0f)
        morf = (f32)l_morfFrames[idx];
    s32 cur = a->mAnmIdx;
    if ((u32)idx != (u32)cur && cur != -1) {
        a->mAnmIdx = anm;
        Kg2Core_setAnm(a->mpMorf.get(), (s32)l_loopModes[idx], morf, (f32)l_anmSpeeds[idx],
                       (s32)l_anmIndices[idx], -1, gabi::guest_addr("Kg")); /* dNpc_setAnm */
        cur = a->mAnmIdx;
        if (cur == 12) {
            u32 m = gabi::ea(a->mpMorf.get());
            s16 end = (s16)gabi::ftoi(endFrameMinus1(m));
            kg2Morf(m)->mFrameCtrl.mFrame = (f32)end;
            a->mpMorf.get()->mFrameCtrl.mRate = -1.0f;
            cur = a->mAnmIdx;
        }
    }
    a->m_jnt.mbHeadLock = cur == 2 ? 0 : 1;
    a->m_jnt.mbBackBoneLock = 1;
}

/* 0226856C */
BOOL daNpc_Kg2_c::CreateInit() {
    auto* a = this;
    a->gravity = -30.0f;
    a->mInitAngle[1] = (u16)a->current.angle.y;
    a->mInitAngle[0] = (u16)a->current.angle.x;
    a->mInitAngle[2] = (u16)a->current.angle.z;
    a->attention_info.distances[1] = 0x6E;
    a->attention_info.flags = 10;
    a->attention_info.distances[3] = 0x6E;
    setAction(a, ACT_wait);
    copyWords(gabi::ea(&a->mAttentionPos), gabi::ea(&a->current.pos), 3);
    Kg2Core_initStts(&a->mStts, 0xFF, 0xFF, a); /* dCcD_Stts::Init */
    Kg2Core_setCylinder(&a->mCyl,
                        kg2Address<const void>(Kg2CylinderSource)); /* dCcD_Cyl::Set (l_cyl_src) */
    a->mCyl.mStts = &a->mStts;
    a->setCollision(60.0f, 150.0f);
    Kg2Core_actorInfo(&a->mEventCut, "Kg2", a); /* setActorInfo2 */
    a->mbTicketBought = 0;
    a->mbItemShown = 0;
    a->mEventCut.mpJntCtrl = &a->m_jnt;
    a->mNextMsgNo = 0;
    a->mEventOrder = 0;
    a->mbGreeted = 0;
    a->m_jnt.mbBackBoneLock = 1;
    a->mItemBtpFrame = 0;
    a->m_jnt.mbHeadLock = 1;
    a->mEventNo = KG2_EVENT_NONE;
    a->mEventIds[0] =
        Kg2Core_eventIdx(kg2Address<dEvent_manager_c>(evtMng()), "KG2_START_DEMO", 0xFF);
    a->mEventIds[1] =
        Kg2Core_eventIdx(kg2Address<dEvent_manager_c>(evtMng()), "KG2_CLEAR_DEMO", 0xFF);
    a->mEventIds[2] = Kg2Core_eventIdx(kg2Address<dEvent_manager_c>(evtMng()), "KG2_GETDEMO", 0xFF);
    a->mEventIds[3] = Kg2Core_eventIdx(kg2Address<dEvent_manager_c>(evtMng()), "CANON_GAME", 0xFF);
    a->set_mtx();
    f32 time = (f32)kg2Save(saveInfo())->mSavedata.mPlayer.mPlayerStatusB.mTime;
    if (time < 105.0f || !(time < 300.0f)) {
        a->mbNight = 1;
        a->setAnm(13, 0.0f);
    } else {
        a->mbNight = 0;
        a->setAnm(1, -1.0f);
    }
    kg2Pointer() = a;
    return TRUE;
}

/* 02268864 */
cPhs_State daNpc_Kg2_c::_create() {
    auto* a = this;
    u32 flags = (u32)a->actor_condition;
    if (!(flags & 8)) {
        if (a) {
            Kg2Core_initNpc(a); /* fopNpc_npc_c::fopNpc_npc_c */
            a->__vtbl = Kg2ActorVtable;
            Kg2Core_initBtpObject(&a->mItemBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            Kg2Core_initBtpObject(&a->mBtpAnm);
            flags = (u32)a->actor_condition;
        }
        a->actor_condition = flags | 8;
    }
    cPhs_State phase = dComIfG_resLoad(&a->mPhs, "Kg");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!Kg2Core_entryHeap(
            a, gabi::guest_fn(reinterpret_cast<const void*>(daNpc_Kg2_CallbackCreateHeap)), 0x2D00))
        return cPhs_ERROR_e;
    u32 model = gabi::ea(a->mpMorf.get()->mpModel.get());
    a->cullMtx = model ? gabi::ea(&kg2Model(model)->mBaseTransformMtx) : 0;
    // HD: descriptive debug label replaces the original Japanese display text.
    if ((s8)kg2Hio().mNo < 0)
        kg2Hio().mNo = Kg2Core_createHioChild("Salvatore Cannon Game", &kg2Hio());
    if (!a->CreateInit())
        return cPhs_ERROR_e;
    return phase;
}

/* 02268988 */
cPhs_State daNpc_Kg2_Create(daNpc_Kg2_c* a) { return a->_create(); }

/* 0226898C */
BOOL daNpc_Kg2_c::_delete() {
    auto* a = this;
    dComIfG_resDelete(&a->mPhs, "Kg");
    if (gabi::ea(a->heap.get())) {
        u32 morf = gabi::ea(a->mpMorf.get());
        if (morf)
            Kg2Core_stopAnime(kg2Address<mDoExt_McaMorf>(morf)); /* McaMorf::stopZelAnime */
    }
    s8 no = (s8)kg2Hio().mNo;
    kg2Pointer() = nullptr;
    if (no >= 0) {
        Kg2Core_deleteHioChild(no); /* mDoHIO_deleteChild */
        kg2Hio().mNo = -1;
    }
    return TRUE;
}

/* 02268A0C */
BOOL daNpc_Kg2_Delete(daNpc_Kg2_c* a) { return a->_delete(); }

/* 02268A10 */
void daNpc_Kg2_c::playTexPatternAnm() {
    auto* a = this;
    if (a->mbNight == 1 && a->mBtpIdx == 0 && a->mBtpFrame == 2) {
        a->mBtpFrame = 0;
        a->mBtpIdx = 4;
        a->initTexPatternAnm(1);
        return;
    }
    if (Kg2Core_timer(&a->mBlinkTimer) != 0)
        return; /* cLib_calcTimer */
    if (a->mBtpIdx == 4) {
        if (a->mBtpFrame == 0) {
            a->mBtpFrame = 1;
            f32 r = Kg2Core_random(150.0f); /* cM_rndF */
            a->mBlinkTimer = (s16)gabi::ftoi(r + 150.0f);
        } else {
            a->mBtpFrame = 0;
            f32 r = Kg2Core_random(150.0f);
            a->mBlinkTimer = (s16)((s16)gabi::ftoi(r + 150.0f) * 2);
        }
        return;
    }
    u32 btp = gabi::ea(a->mpBtp.get());
    s32 max = gabi::call_ptr<s32>((u32)kg2BtpVtable(kg2Btp(btp)->mVtable)->mGetFrameMax, btp);
    if ((s32)a->mBtpFrame >= max) {
        btp = gabi::ea(a->mpBtp.get());
        max = gabi::call_ptr<s32>((u32)kg2BtpVtable(kg2Btp(btp)->mVtable)->mGetFrameMax, btp);
        a->mBtpFrame = (u8)(a->mBtpFrame - max);
        f32 r = Kg2Core_random(100.0f);
        a->mBlinkTimer = (s16)gabi::ftoi(r + 30.0f);
    } else {
        a->mBtpFrame = (u8)(a->mBtpFrame + 1);
    }
}

/* 02268C20 */
void daNpc_Kg2_c::checkOrder() {
    auto* a = this;
    u16 cond = (u16)a->eventInfo.mCommand;
    if (cond == 2) {
        setAction(a, ACT_event_wait);
        a->mEventOrder = 0;
    } else if (cond == 1) {
        s8 order = a->mEventOrder;
        if (order == 1 || order == 2) {
            a->mEventOrder = 0;
            a->mbTalkRequest = 1;
        }
    }
}

/* 02268DA0 */
BOOL daNpc_Kg2_c::evn_setAnm_init(s32 staff) {
    auto* a = this;
    u32 anm = gabi::ea(getMySubstanceP(staff, gabi::guest_addr("AnmNo"), 3));
    u32 loop = gabi::ea(getMySubstanceP(staff, gabi::guest_addr("count"), 3));
    u32 morf = gabi::ea(getMySubstanceP(staff, gabi::guest_addr("hokan"), 0));
    if (anm) {
        f32 m = -1.0f;
        if (morf)
            m = (f32)kg2Float(morf);
        a->setAnm((s8)kg2Substance(anm)->mByte[3], m);
        a->mEvnLoopCount = loop ? (s8)kg2Substance(loop)->mByte[3] : 0;
    }
    return TRUE;
}

/* 02268E88 */
BOOL daNpc_Kg2_c::evn_jnt_lock_init(s32 staff) {
    auto* a = this;
    u32 v = gabi::ea(getMySubstanceP(staff, gabi::guest_addr("prm"), 3));
    u32 lock = v ? (u32)kg2Word(v) : 0;
    if (lock < 2) {
        a->m_jnt.mbHeadLock = (u8)lock;
        a->m_jnt.mbBackBoneLock = 0;
    } else if (lock == 2) {
        a->m_jnt.mbHeadLock = 0;
        a->m_jnt.mbBackBoneLock = 1;
    } else if (lock == 3) {
        a->m_jnt.mbHeadLock = 1;
        a->m_jnt.mbBackBoneLock = 1;
    }
    return TRUE;
}

/* 02268F38 */
BOOL daNpc_Kg2_c::evn_talk_init(s32 staff) {
    auto* a = this;
    u32 v = gabi::ea(getMySubstanceP(staff, gabi::guest_addr("msg_num"), 3));
    a->mbHasMsg = 0;
    a->mCurrMsgBsPcId = 0xFFFFFFFF;
    a->mNextMsgNo = v ? (u32)kg2Word(v) : 0;
    return TRUE;
}

/* 02268FAC */
BOOL daNpc_Kg2_c::evn_createItem_init(s32 staff) {
    auto* a = this;
    u32 count = getEventReg(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0xB703);
    s32 id;
    if (count <= 2) {
        u8 item = (u8)l_prizes[count];
        id = Kg2Core_createItem(&a->current.pos, item, 0, -1, (s32)(s8)a->current.roomNo, 0, 0);
    } else {
        id = Kg2Core_createItem(&a->current.pos, 6, 0, -1, (s32)(s8)a->current.roomNo, 0, 0);
    }
    if (id != -1)
        kg2Play(play())->play.mEvtCtrl.mPtItem = id;
    return TRUE;
}

/* 02269068 */
BOOL daNpc_Kg2_c::evn_setAnm() {
    auto* a = this;
    u32 morf = gabi::ea(a->mpMorf.get());
    BOOL passed;
    if (a->mAnmIdx == 12)
        passed = checkPass(gabi::ea(&kg2Morf(morf)->mFrameCtrl), 1.0f);
    else
        passed = checkPass(gabi::ea(&kg2Morf(morf)->mFrameCtrl), endFrameMinus1(morf));
    if (passed) {
        s8 n = (s8)(a->mEvnLoopCount - 1);
        a->mEvnLoopCount = n;
        return n <= 0;
    }
    return a->mEvnLoopCount <= 0;
}

/* 02269128 */
BOOL daNpc_Kg2_c::evn_talk() {
    auto* a = this;
    return a->talk(1) == 0x12;
}

/* 02269158 */
BOOL daNpc_Kg2_c::privateCut() {
    auto* a = this;
    u32 name = (u32)a->mEventCut.mpEvtStaffName;
    s32 staff = Kg2Core_staffId(kg2Address<dEvent_manager_c>(evtMng()),
                                kg2Address<const char>(name), 0, 0); /* getMyStaffId */
    if (staff == -1)
        return FALSE;
    s32 act = Kg2Core_actIdx(kg2Address<dEvent_manager_c>(evtMng()), staff,
                             kg2Address<const void>(Kg2CutTable), 5, 1, 0); /* getMyActIdx */
    u32 ev = evtMng();
    if (act == -1) {
        Kg2Core_cutEnd(kg2Address<dEvent_manager_c>(ev), staff); /* cutEnd */
        return TRUE;
    }
    if (Kg2Core_isAdvance(kg2Address<dEvent_manager_c>(ev), staff)) { /* getIsAddvance */
        switch ((u32)act) {
        case 0:
            a->evn_setAnm_init(staff);
            break;
        case 1:
            a->evn_jnt_lock_init(staff);
            break;
        case 3:
            a->evn_talk_init(staff);
            break;
        case 4:
            a->evn_createItem_init(staff);
            break;
        }
    }
    switch ((u32)act) {
    case 0:
        if (!a->evn_setAnm())
            return TRUE;
        break;
    case 3:
        if (!a->evn_talk())
            return TRUE;
        break;
    }
    Kg2Core_cutEnd(kg2Address<dEvent_manager_c>(evtMng()), staff);
    return TRUE;
}

/* 022692EC */
BOOL daNpc_Kg2_c::processMove() {
    auto* a = this;
    (a->*a->mAction)(0);
    if (a->mEventCut.cutProc())
        return TRUE;
    return a->privateCut() ? TRUE : FALSE;
}

/* 022693A8 */
void daNpc_Kg2_c::eventOrder() {
    auto* a = this;
    s8 order = a->mEventOrder;
    if (order == 1 || order == 2) {
        order = a->mEventOrder;
        a->eventInfo.mCondition = (u16)a->eventInfo.mCondition | 1;
        if (order == 1)
            Kg2Core_orderSpeak(a); /* fopAcM_orderSpeakEvent */
    } else if (order == 3) {
        s16 id = a->mEventIds[(s32)a->mEventNo];
        Kg2Core_orderOther(a, id, 0xFF, 0xFFFF, 0, 1); /* fopAcM_orderOtherEventId */
    }
}

/* 02269410 */
void daNpc_Kg2_c::subAnm() {
    auto* a = this;
    u32 p = gabi::ea(a);
    if (a->mBtpIdx == 4 && p != 0 && gabi::ea(&a->eyePos) != 0) {
        s32 reverb = Kg2Core_reverb((s32)(s8)a->current.roomNo); /* dComIfGp_getReverb */
        Kg2Core_sound(0x4114, &a->eyePos, 0, reverb);            /* seStart */
    }
    s8 anm = a->mAnmIdx;
    if (anm == 3 || anm == 13) {
        if (checkPass(gabi::ea(&a->mpMorf.get()->mFrameCtrl), 1.0f)) {
            s8 cur = a->mAnmIdx;
            a->mbItemShown = 1;
            a->mItemBtpFrame = cur == 3 ? 3 : 4;
            return;
        }
        anm = a->mAnmIdx;
    }
    if (anm == 9) {
        if (checkPass(gabi::ea(&a->mpMorf.get()->mFrameCtrl), 30.0f)) {
            a->mbItemShown = 1;
            a->mItemBtpFrame = 2;
            return;
        }
        anm = a->mAnmIdx;
    }
    if (anm == 12) {
        if (checkPass(gabi::ea(&a->mpMorf.get()->mFrameCtrl), 1.0f)) {
            a->mbItemShown = 0;
            a->setAnm(1, -1.0f);
            return;
        }
        anm = a->mAnmIdx;
    }
    if (anm == 13) {
        u32 morf = gabi::ea(a->mpMorf.get());
        if (checkPass(gabi::ea(&kg2Morf(morf)->mFrameCtrl), endFrameMinus1(morf)))
            a->setAnm(4, -1.0f);
    }
}

/* 02269690 */
BOOL daNpc_Kg2_c::_execute() {
    auto* a = this;
    s16 prm[9];
    for (int i = 0; i < 9; ++i)
        prm[i] = (s16)kg2Hio().mJntParam[i];
    Kg2Core_setJointParam(&a->m_jnt, prm[1], prm[3], prm[5], prm[7], prm[0], prm[2], prm[4], prm[6],
                          prm[8]);
    a->playTexPatternAnm();
    Kg2Core_playMorf(a->mpMorf.get(), &a->eyePos, 0, 0); /* McaMorf::play */
    Kg2Core_calcMorf(a->mpMorf.get());                   /* McaMorf::calc */
    a->checkOrder();
    a->processMove();
    a->eventOrder();
    a->subAnm();
    Kg2Core_posMove(a, &a->mStts);                                      /* fopAcM_posMoveF */
    Kg2Core_correctPosition(&a->mObjAcch, &kg2Play(play())->play.mBgS); /* dBgS_Acch::CrrPos */
    a->tevStr.mRoomNo =
        Kg2Core_roomId(&kg2Play(play())->play.mBgS,
                       static_cast<cBgS_PolyInfo*>(&a->mObjAcch.m_gnd)); /* dBgS::GetRoomId */
    a->tevStr.mEnvrIdxOverride =
        Kg2Core_polyColor(&kg2Play(play())->play.mBgS,
                          static_cast<cBgS_PolyInfo*>(&a->mObjAcch.m_gnd)); /* dBgS::GetPolyColor */
    a->set_mtx();
    a->setCollision(60.0f, 150.0f);
    return TRUE;
}

/* 022697A0 */
BOOL daNpc_Kg2_Execute(daNpc_Kg2_c* a) { return a->_execute(); }

/* 022697A4 */
BOOL daNpc_Kg2_c::_draw() {
    auto* a = this;
    u32 model = gabi::ea(a->mpMorf.get()->mpModel.get());
    u32 data = gabi::ea(kg2Model(model)->mModelData.get());
    Kg2Core_settingTev(envLight(), 0, &a->current.pos, &a->tevStr); /* settingTevStruct */
    Kg2Core_lightTev(envLight(), kg2Address<J3DModel>(model),
                     &a->tevStr); /* setLightTevColorType */
    Kg2Core_entryBtp(&a->mBtpAnm, kg2Address<J3DModelData>(data),
                     (u32)a->mBtpFrame); /* mDoExt_btpAnm::entry */
    Kg2Core_entryDL(a->mpMorf.get());    /* McaMorf::entryDL */
    kg2Data(data)->mTexPattern = 0;
    if (a->mbItemShown) {
        u32 itemData = gabi::ea(a->mpItemModel.get()->mModelData.get());
        u32 env = envLight();
        J3DModel* item = a->mpItemModel.get();
        Kg2Core_lightTev(env, item, &a->tevStr);
        Kg2Core_entryBtp(&a->mItemBtpAnm, kg2Address<J3DModelData>(itemData),
                         (u32)a->mItemBtpFrame);
        Kg2Core_updateModel(a->mpItemModel.get(), 0); /* mDoExt_modelUpdateDL */
        kg2Data(itemData)->mTexPattern = 0;
    }
    Kg2Core_registerFigure(0x81, a, &a->current.pos, (s16)a->current.angle.y, 1.0f, 1.0f,
                           1.0f); /* dSnap_RegistFig */
    return TRUE;
}

/* 022698A0 */
BOOL daNpc_Kg2_Draw(daNpc_Kg2_c* a) { return a->_draw(); }

/* 022698A4 */
u8 daNpc_Kg2_c::chkAttention(cXyz* pos, s16 angle) {
    auto* a = this;
    u32 player = (u32)gabi::ea((fopAc_ac_c*)kg2Play(play())->play.mPlayerInfo[0].mpPlayer);
    f32 dz = (f32)kg2Actor(player)->current.pos.z - pos->z;
    f32 dx = (f32)kg2Actor(player)->current.pos.x - pos->x;
    f32 radius = (f32)kg2Hio().mAttentionDist;
    s32 limit = (s16)kg2Hio().mAttentionAngle;
    f32 dist = Kg2Core_squareRoot(gabi::fmadds(dx, dx, dz * dz)); /* sqrtf */
    s16 target = Kg2Core_atan2s(dx, dz);                          /* cM_atan2s */
    if (a->mbAttention) {
        radius = radius + 40.0f;
        limit += 0x71C;
    }
    s16 diff = (s16)(target - angle);
    s32 mag = diff < 0 ? -(s32)diff : diff;
    return limit > mag && radius > dist;
}

/* 022699D0 */
u16 daNpc_Kg2_c::next_msgStatus(be<u32>* msg) {
    auto* a = this;
    u32 cur = *msg;
    u32 msgMng = (u32)kg2MessagePointer();
    switch (cur) {
    case 0x3139:
        if (getEventReg(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0xB703) == 0)
            *msg = 0x313E;
        else if (Kg2Core_days() < 4)
            *msg = 0x313B; /* dLib_getIplDaysFromSaveTime */
        else if (isEventBit(gabi::ea(&kg2Save(saveInfo())->mTmp), 0x102))
            *msg = 0x313C;
        else {
            onEventBit(gabi::ea(&kg2Save(saveInfo())->mTmp), 0x102);
            *msg = 0x313D;
        }
        break;
    case 0x313A:
    case 0x313B:
    case 0x313C:
    case 0x313D:
    case 0x313E:
        *msg = 0x313F;
        break;
    case 0x313F:
    case 0x314F:
        if ((u32)kg2Message(msgMng)->mSelectNum == 0) {
            if ((u16)kg2Save(saveInfo())->mSavedata.mPlayer.mPlayerStatusA.mRupee < 50)
                *msg = 0x3143;
            else {
                u32 g = play();
                kg2Play(g)->play.mItemRupeeCount = (s32)kg2Play(g)->play.mItemRupeeCount - 50;
                kg2Play(play())->play.mAStatusForce = 25;
                *msg = 0x3144;
            }
        } else
            *msg = 0x3142;
        break;
    case 0x3141:
        if (!a->mbGreeted) {
            a->mbGreeted = 1;
            *msg = 0x3139;
        } else
            *msg = 0x313A;
        break;
    case 0x3149:
        if ((u32)kg2Message(msgMng)->mSelectNum == 0) {
            onEventBit(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0x2508);
            a->mbTicketBought = 1;
            *msg = 0x314B;
        } else
            *msg = 0x314A;
        break;
    case 0x314A:
        *msg = 0x3148;
        break;
    case 0x314D:
        *msg = 0x3147;
        break;
    case 0x3140:
    case 0x3145:
    case 0x3146:
    case 0x3147:
    case 0x3148:
    case 0x314C:
    case 0x314E:
    case 0x3150:
    case 0x3152:
    case 0x3154:
    case 0x3156:
    case 0x3158:
        *msg = cur + 1;
        break;
    default:
        return 0x10;
    }
    return 0xF;
}

/* 02269C44 */
u32 daNpc_Kg2_c::getMsg() {
    auto* a = this;
    if (a->mNextMsgNo != 0) {
        u32 count = getEventReg(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0xB703);
        u32 msg = a->mNextMsgNo;
        if (msg == 0x3145) {
            if (a->mbTicketBought == 1)
                msg = 0x314B;
            else if (!isEventBit(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0x2508)) {
                msg = a->mNextMsgNo;
                a->mNextMsgNo = 0;
                return msg;
            } else
                msg = 0x314C;
        } else if (msg == 0x3150) {
            if (count != 0)
                msg = 0x3154;
        } else if (msg == 0x3152) {
            if (count == 1)
                msg = 0x3156;
            else if (count >= 2)
                msg = 0x3158;
        }
        a->mNextMsgNo = 0;
        return msg;
    }
    if (a->mbNight == 1)
        return 0x315B;
    if (isEventBit(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0x2540) &&
        !isEventBit(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0x2520)) {
        onEventBit(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0x2520);
        return 0x3140;
    }
    if (!a->mbGreeted) {
        a->mbGreeted = 1;
        return 0x3139;
    }
    return 0x313A;
}

/* 02269DB0 */
void daNpc_Kg2_c::anmAtr(u16 atr) {
    auto* a = this;
    u8 req = (u8)kg2Play(play())->play.mMesgAnime;
    if (a->mbNight == 1)
        return;
    s8 anm = -1, btp = 0;
    switch (req) {
    case 0:
        anm = 0;
        btp = 0;
        break;
    case 1:
        anm = 1;
        btp = 0;
        break;
    case 2:
        anm = 2;
        btp = 1;
        break;
    case 3:
        anm = 3;
        btp = 0;
        break;
    case 4:
        anm = 5;
        btp = 1;
        break;
    case 5:
        anm = 6;
        btp = 1;
        break;
    case 6:
        anm = 7;
        btp = 2;
        break;
    case 7:
        anm = 8;
        btp = 3;
        break;
    case 8:
        anm = 9;
        btp = 0;
        break;
    case 9:
        anm = 10;
        btp = 1;
        break;
    case 10:
        anm = 11;
        btp = 2;
        break;
    case 11:
        anm = 14;
        btp = 2;
        break;
    }
    if (anm >= 0) {
        a->setAnm(anm, -1.0f);
        a->mBtpIdx = btp;
        a->initTexPatternAnm(1);
    }
    if (a->mAnmIdx == 3) {
        u32 morf = gabi::ea(a->mpMorf.get());
        if (checkPass(gabi::ea(&kg2Morf(morf)->mFrameCtrl), endFrameMinus1(morf))) {
            a->setAnm(4, -1.0f);
            a->mBtpIdx = 0;
            a->initTexPatternAnm(1);
        }
    }
    kg2Play(play())->play.mMesgAnime = 0xFF;
}

/* 0226A0B0 */
void daNpc_Kg2_c::setAttention() {
    auto* a = this;
    copyWords(gabi::ea(&a->eyePos), gabi::ea(&a->mAttentionPos), 3);
}

/* 0226A0CC */
void daNpc_Kg2_c::lookBack() {
    auto* a = this;
    s32 mode = a->mMode;
    u32 ex = 0, ey = 0, ez = 0; /* 0.0f */
    u32 target = 0;
    s16 angle = (s16)a->current.angle.y;
    cXyz eye;
    cXyz dst;
    cXyz eyePos;
    if ((u32)mode >= 1 && (u32)mode <= 2) {
        if (mode == 2)
            a->m_jnt.mbTrn = 1;
        if (a->mbAttention) {
            Kg2Core_playerEye(&eyePos, (f32)kg2Hio().mEyeOffsetY); /* dNpc_playerEyePos */
            copyWords(gabi::ea(&dst), gabi::ea(&eyePos), 3);
            ex = (u32)kg2Words(gabi::ea(&a->current.pos.x))->mWord[0];
            ey = (u32)kg2Words(gabi::ea(&a->eyePos.y))->mWord[0];
            ez = (u32)kg2Words(gabi::ea(&a->current.pos.z))->mWord[0];
            target = gabi::ea(&dst);
        }
    }
    if ((u8)a->m_jnt.mbTrn) {
        Kg2Core_approachAngle(&a->mLookSpeed, (s16)kg2Hio().mLookSpeed, 4,
                              0x800); /* cLib_addCalcAngleS2 */
        s16 speed = a->mLookSpeed;
        u32 e = gabi::ea(&eye);
        kg2Words(e)->mWord[0] = ex;
        kg2Words(e)->mWord[1] = ey;
        kg2Words(e)->mWord[2] = ez;
        Kg2Core_lookAt(&a->m_jnt, &a->current.angle.y, kg2Address<cXyz>(target), &eye, angle, speed,
                       1); /* lookAtTarget */
    } else {
        a->mLookSpeed = 0;
        u32 e = gabi::ea(&eye);
        kg2Words(e)->mWord[0] = ex;
        kg2Words(e)->mWord[1] = ey;
        kg2Words(e)->mWord[2] = ez;
        Kg2Core_lookAt(&a->m_jnt, &a->current.angle.y, kg2Address<cXyz>(target), &eye, angle, 0, 1);
    }
}

/* 0226A258 */
void daNpc_Kg2_c::wait01() {
    auto* a = this;
    f32 time = (f32)kg2Save(saveInfo())->mSavedata.mPlayer.mPlayerStatusB.mTime;
    if (time < 105.0f || !(time < 300.0f)) {
        if (a->mbNight == 0) {
            a->mbNight = 1;
            a->setAnm(13, -1.0f);
            a->mBtpIdx = 0;
            a->initTexPatternAnm(1);
        }
    } else if (a->mbNight == 1) {
        a->mbNight = 0;
        a->setAnm(12, -1.0f);
        a->mBtpIdx = 0;
        a->initTexPatternAnm(1);
    }
    if (a->mbTalkRequest) {
        a->mMode = KG2_MODE_TALK;
        return;
    }
    u32 player = (u32)gabi::ea((fopAc_ac_c*)kg2Play(play())->play.mPlayerInfo[0].mpPlayer);
    f32 dy = std::fabs((f32)a->current.pos.y - (f32)kg2Actor(player)->current.pos.y);
    if (a->mbAttention && dy < 50.0f) {
        s8 anm = a->mAnmIdx;
        if (anm != 13 && anm != 12)
            a->mEventOrder = 2;
    }
}

/* 0226A394 */
void daNpc_Kg2_c::talk01() {
    auto* a = this;
    if (a->talk(1) != 0x12)
        return;
    a->mMode = KG2_MODE_WAIT;
    u32 g = play();
    kg2Play(g)->play.mEvtCtrl.mEventFlag = (u16)kg2Play(g)->play.mEvtCtrl.mEventFlag | 8;
    u32 msgNo = a->mCurrMsgNo;
    a->mbTalkRequest = 0;
    if (msgNo == 0x3144) {
        a->mEventNo = KG2_EVENT_START;
        a->mEventOrder = 3;
        return;
    }
    if (a->mbNight == 0) {
        a->setAnm(1, -1.0f);
        a->mBtpIdx = 0;
        a->initTexPatternAnm(1);
    } else {
        a->setAnm(4, -1.0f);
    }
}

/* inlined into daNpc_Kg2_c::wait_action, daNpc_Kg2_c::event_wait_action */
static void chkAttentionHere(daNpc_Kg2_c* a) {
    s16 angle =
        (s16)((s16)a->current.angle.y + (s16)a->m_jnt.mAngles[0][1] + (s16)a->m_jnt.mAngles[1][1]);
    cXyz pos;
    copyWords(gabi::ea(&pos), gabi::ea(&a->current.pos), 3);
    a->mbAttention = a->chkAttention(&pos, angle);
}

/* 0226A474 */
BOOL daNpc_Kg2_c::wait_action(void* arg) {
    auto* a = this;
    s8 st = a->mActionState;
    if (st == 0) {
        a->mMode = KG2_MODE_WAIT;
        a->mActionState = (s8)(a->mActionState + 1);
        return TRUE;
    }
    if (st == -1)
        return TRUE;
    chkAttentionHere(a);
    s8 mode = a->mMode;
    a->mEventOrder = 0;
    if ((u32)mode == 1)
        a->wait01();
    else if ((u32)mode == 2)
        a->talk01();
    a->lookBack();
    a->setAttention();
    return TRUE;
}

/* 0226A574 */
BOOL daNpc_Kg2_c::event_wait_action(void* arg) {
    auto* a = this;
    s8 st = a->mActionState;
    if (st == 0) {
        a->mMode = KG2_MODE_WAIT;
        a->mActionState = (s8)(a->mActionState + 1);
        return TRUE;
    }
    if (st == -1)
        return TRUE;
    chkAttentionHere(a);
    a->lookBack();
    a->setAttention();
    s16 ev = a->mEventIds[(s32)a->mEventNo];
    if (!Kg2Core_endCheck(kg2Address<dEvent_manager_c>(evtMng()), ev))
        return TRUE; /* endCheck */
    switch (a->mEventNo) {
    case 0:
        a->mEventOrder = 3;
        a->mEventNo = KG2_EVENT_GAME;
        break;
    case 3:
        if ((u8)kg2CanonGameResult() == 1) {
            a->mEventOrder = 3;
            a->mEventNo = KG2_EVENT_CLEAR;
        } else {
            a->mEventOrder = 1;
            a->mNextMsgNo = 0x314E;
        }
        break;
    case 1: {
        a->mEventOrder = 3;
        a->mEventNo = KG2_EVENT_ITEM;
        s32 n = (s32)getEventReg(gabi::ea(&kg2Save(saveInfo())->mSavedata.mEvent), 0xB703) + 1;
        if (n > 3)
            n = 3;
        Kg2Core_setEventReg(&kg2Save(saveInfo())->mSavedata.mEvent, 0xB703,
                            (u8)n); /* setEventReg */
        break;
    }
    case 2:
        a->mEventOrder = 1;
        a->mNextMsgNo = 0x315A;
        break;
    default:
        a->mEventNo = KG2_EVENT_NONE;
        break;
    }
    u32 g = play();
    kg2Play(g)->play.mEvtCtrl.mEventFlag = (u16)kg2Play(g)->play.mEvtCtrl.mEventFlag | 8;
    setAction(a, ACT_wait);
    return TRUE;
}

// HD: compiler-generated header statics and deleting destructors.
/* 0226A99C */
void daNpc_Kg2_sinit() {
    kg2HeaderStatics().mZero[2] = 0;
    kg2HeaderStatics().mZero[0] = 0;
    kg2HeaderStatics().mZero[3] = 0;
    kg2HeaderStatics().mZero[1] = 0;
    Kg2Core_registerGlobal(kg2Address<void>(Kg2HeaderRecord)); /* __register_global_object */
    kg2HeaderStatics().mInitial[0] = kg2InitialWords().mFirst;
    kg2HeaderStatics().mInitial[1] = kg2InitialWords().mSecond;
    Kg2Core_initAngle(kg2Address<be<u8>>(Kg2AngleStatic));
    Kg2Core_registerGlobal(kg2Address<void>(Kg2AngleRecord));
    Kg2Core_initVector(kg2Address<be<u8>>(Kg2VectorStatic));
    Kg2Core_registerGlobal(kg2Address<void>(Kg2VectorRecord));
    daNpc_Kg2_HIO_ct(&kg2Hio());
}

// HD: daNpc_Kg2_HIO_c deleting destructor.
/* 0226AA3C */
void daNpc_Kg2_HIO_dt(daNpc_Kg2_HIO_c* self, u32 flags) {
    if (self && (flags & 1))
        Kg2Core_release(self);
}

/* 0226AA50 */
BOOL daNpc_Kg2_IsDelete(daNpc_Kg2_c* a) { return TRUE; }

// HD: daNpc_Kg2_c deleting destructor.
/* 0226AA58 */
void daNpc_Kg2_dt(daNpc_Kg2_c* a, u32 flags) {
    if (!a)
        return;
    Kg2Core_destroyCylinder(&a->mCyl, 2); /* dCcD_Cyl dtor */
    Kg2Core_destroyStts(&a->mStts, 2);    /* dCcD_Stts dtor */
    Kg2Core_destroyCircle(&a->mAcchCir.m_cir, 2);
    a->mObjAcch.dBgS_GrpPassChk::__vtbl = Kg2GroupVtable;
    a->mObjAcch.dBgS_PolyPassChk::__vtbl = Kg2PolyVtable;
    Kg2Core_destroyAcch(&a->mObjAcch, 0); /* dBgS_Acch dtor */
    Kg2Core_destroyActor(a, 0);           /* fopAc_ac_c dtor */
    if (flags & 1)
        Kg2Core_release(a);
}

// HD: empty virtual.
/* 0226AAF4 */
void daNpc_Kg2_emptyVirtual(void* self) {}

/* WWHD glue: unit-private HD ABI bindings and image strings.
data l_btpIndices 1001C000
data l_morfFrames 101BF1CC
data l_loopModes 101BF190
data l_anmSpeeds 101BF208
data l_anmIndices 1001BFC4
data l_prizes 1001C194
str 1001C028 "Kg" in 02267E04
str 1001C02C "d_a_npc_kg2.cpp" in 02267E04
str 1001C03C "m_btp != (0)" in 02267E04
str 1001C050 "handL" in 02267F18
str 1001C058 "Kg" in 02267F18
str 1001C05C "head" in 02267F18
str 1001C064 "d_a_npc_kg2.cpp" in 02267F18
str 1001C074 "backbone2" in 02267F18
str 1001C080 "modelData != (0)" in 02267F18
str 1001C094 "m_jnt.getHeadJntNum() >= 0" in 02267F18
str 1001C0B0 "m_jnt.getBackboneJntNum() >= 0" in 02267F18
str 1001C0D0 "m_handL_num >= 0" in 02267F18
str 1001C0FC "Kg" in 02268430
str 1001C114 "Kg2" in 0226856C
str 1001C118 "KG2_GETDEMO" in 0226856C
str 1001C124 "KG2_START_DEMO" in 0226856C
str 1001C134 "KG2_CLEAR_DEMO" in 0226856C
str 1001C144 "CANON_GAME" in 0226856C
str 1001C150 "Kg" in 02268864
str 1001C154 "Salvatore Cannon Game" renamed in 02268864
str 1001C168 "Kg" in 0226898C
str 1001C170 "AnmNo" in 02268DA0
str 1001C178 "count" in 02268DA0
str 1001C180 "hokan" in 02268DA0
str 1001C188 "prm" in 02268E88
str 1001C18C "msg_num" in 02268F38

ext 02555D0C u32 Kg2Core_envLight(void)
ext 027F2BF8 BOOL Kg2Core_checkPass(J3DFrameCtrl*,f32)
ext 025B8BB0 u8 Kg2Core_getEventReg(dSv_event_c*,u32)
ext 025B8B94 BOOL Kg2Core_isEventBit(dSv_event_c*,u32)
ext 025B8B68 void Kg2Core_onEventBit(dSv_event_c*,u32)
ext 0254487C void* Kg2Core_getSubstance(dEvent_manager_c*,s32,const char*,s32)
ext 0273AD10 u32 Kg2Core_allocate(s32)
ext 0259DA18 void Kg2Core_initHio(void*)
ext 027F7878 u32 Kg2Core_jointResource(J3DNode*)
ext 028E90D4 void Kg2Core_copyMatrix(Mtx34*,Mtx34*)
ext 028E8F64 void Kg2Core_multVec(Mtx34*,cXyz*,cXyz*)
ext 025F1BF4 void Kg2Core_rotateX(Mtx34*,s16)
ext 025F1C5C void Kg2Core_rotateZ(Mtx34*,s16)
ext 0273AA24 void Kg2Core_assertion(const char*,s32,const char*)
ext 025E789C BOOL Kg2Core_initBtp(mDoExt_btpAnm*,J3DModelData*,J3DAnmTexPattern*,s32,s32,f32,s32,s32,u32,s32)
ext 027F68FC u32 Kg2Core_modelResource(J3DModelData*)
ext 025E4F64 u32 Kg2Core_newMorf(s32,u32,s32,s32,u32,s32,s32,s32,f32,s32,s32,s32,u32)
ext 027DF9B0 s8 Kg2Core_jointIndex(void*,const char*)
ext 025E38E0 u32 Kg2Core_newModel(J3DModelData*,u32,u32)
ext 024EFF44 void Kg2Core_setWall(dBgS_AcchCir*,f32,f32)
ext 024F06B4 void Kg2Core_setAcch(dBgS_ObjAcch*,cXyz*,cXyz*,daNpc_Kg2_c*,s32,dBgS_AcchCir*,cXyz*,s32,s32)
ext 028E93CC void Kg2Core_translateMatrix(Mtx34*,f32,f32,f32)
ext 025F1C28 void Kg2Core_rotateY(Mtx34*,s16)
ext 025E55A0 void Kg2Core_calcMorf(mDoExt_McaMorf*)
ext 025F24E0 void Kg2Core_translateMultiply(f32,f32,f32)
ext 025F19F8 void Kg2Core_rotateXYZ(Mtx34*,s16,s16,s16)
ext 0259D454 void Kg2Core_setAnm(mDoExt_McaMorf*,s32,f32,f32,s32,s32,u32)
ext 02515F14 void Kg2Core_initStts(dCcD_Stts*,s32,s32,daNpc_Kg2_c*)
ext 02516518 void Kg2Core_setCylinder(dCcD_Cyl*,const void*)
ext 0259F814 void Kg2Core_actorInfo(dNpc_EventCut_c*,const char*,daNpc_Kg2_c*)
ext 02543F10 s16 Kg2Core_eventIdx(dEvent_manager_c*,const char*,s32)
ext 025A1458 void Kg2Core_initNpc(daNpc_Kg2_c*)
ext 025E7820 void Kg2Core_initBtpObject(mDoExt_btpAnm*)
ext 025D63E8 BOOL Kg2Core_entryHeap(daNpc_Kg2_c*,u32,u32)
ext 025F0A10 s8 Kg2Core_createHioChild(const char*,daNpc_Kg2_HIO_c*)
ext 025E563C void Kg2Core_stopAnime(mDoExt_McaMorf*)
ext 025F0A18 void Kg2Core_deleteHioChild(s8)
ext 02055B64 s16 Kg2Core_timer(be<s16>*)
ext 020198D8 f32 Kg2Core_random(f32)
ext 025D7DEC s32 Kg2Core_createItem(cXyz*,u32,s32,s32,s32,s32,s32)
ext 02542D88 s32 Kg2Core_staffId(dEvent_manager_c*,const char*,s32,s32)
ext 02542EDC s32 Kg2Core_actIdx(dEvent_manager_c*,s32,const void*,s32,s32,s32)
ext 02543280 void Kg2Core_cutEnd(dEvent_manager_c*,s32)
ext 025447C8 BOOL Kg2Core_isAdvance(dEvent_manager_c*,s32)
ext 025D76A8 void Kg2Core_orderSpeak(daNpc_Kg2_c*)
ext 025D7A58 void Kg2Core_orderOther(daNpc_Kg2_c*,s16,s32,s32,s32,s32)
ext 02520540 s32 Kg2Core_reverb(s32)
ext 025E1A40 void Kg2Core_sound(u32,cXyz*,s32,s32)
ext 0259E08C void Kg2Core_setJointParam(dNpc_JntCtrl_c*,s16,s16,s16,s16,s16,s16,s16,s16,s16)
ext 025E535C void Kg2Core_playMorf(mDoExt_McaMorf*,cXyz*,s32,s32)
ext 025D6870 void Kg2Core_posMove(daNpc_Kg2_c*,dCcD_Stts*)
ext 024F08A8 void Kg2Core_correctPosition(dBgS_ObjAcch*,dBgS*)
ext 024EF130 u8 Kg2Core_roomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u8 Kg2Core_polyColor(dBgS*,cBgS_PolyInfo*)
ext 025626A4 void Kg2Core_settingTev(u32,s32,cXyz*,dKy_tevstr_c*)
ext 02562F5C void Kg2Core_lightTev(u32,J3DModel*,dKy_tevstr_c*)
ext 025E7B3C void Kg2Core_entryBtp(mDoExt_btpAnm*,J3DModelData*,u32)
ext 025E5590 void Kg2Core_entryDL(mDoExt_McaMorf*)
ext 025E2DE0 void Kg2Core_updateModel(J3DModel*,s32)
ext 025BEBB8 void Kg2Core_registerFigure(s32,daNpc_Kg2_c*,cXyz*,s16,f32,f32,f32)
ext 028F4384 f32 Kg2Core_squareRoot(f32)
ext 020195B0 s16 Kg2Core_atan2s(f32,f32)
ext 0258839C s32 Kg2Core_days(void)
ext 0259D54C void Kg2Core_playerEye(cXyz*,f32)
ext 0200F428 void Kg2Core_approachAngle(be<s16>*,s16,s32,s32)
ext 0259DED0 void Kg2Core_lookAt(dNpc_JntCtrl_c*,be<s16>*,cXyz*,cXyz*,s16,s16,s32)
ext 025440C8 BOOL Kg2Core_endCheck(dEvent_manager_c*,s16)
ext 025B8AF4 void Kg2Core_setEventReg(dSv_event_c*,u32,u8)
ext 028F026C void Kg2Core_registerGlobal(void*)
ext 028ED6F8 void Kg2Core_initAngle(be<u8>*)
ext 028EAB2C void Kg2Core_initVector(be<u8>*)
ext 0273AF40 void Kg2Core_release(void*)
ext 02515A70 void Kg2Core_destroyCylinder(dCcD_Cyl*,s32)
ext 02515860 void Kg2Core_destroyStts(dCcD_Stts*,s32)
ext 02018034 void Kg2Core_destroyCircle(cM3dGCir_l*,s32)
ext 024EFD9C void Kg2Core_destroyAcch(dBgS_ObjAcch*,s32)
ext 025D50BC void Kg2Core_destroyActor(daNpc_Kg2_c*,s32)
*/
