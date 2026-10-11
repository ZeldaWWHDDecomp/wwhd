/**
 * d_a_npc_sarace.cpp (WWHD)
 * NPC - Loot the Sailor (Boating Course)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_sarace.cpp) has only Nonmatching placeholders for this unit, so every
 * function is written from the WWHD code (cking.rpx), keeping the GameCube names, and verified
 * against it.
 */
#include "d/actor/d_a_npc_sarace.h"

// HD: GHS emits a SafeString vtable in each translation unit.
const gc_tu_data gc_tu = {.safeString = 0x10021948, .objAcchDt = {0x10021960, 0x10021970}};

// HD: keep the Shift-JIS HIO label as bytes in the guest string pool.
static SaraceHioLabel l_hioLabel;
using SaraceWord = be<u32>;
using SaraceSignedWord = be<s32>;
static daNpc_Sarace_HIO_c l_HIO;
static cXyz l_offsetAttPos;
static SaraceBuoyPositions l_buoyPos;
static SaraceWord l_offsetAttPosInitialized;
static SaraceWord l_buoyPosInitialized;
static SaraceSignedWord l_shipRaceRupee;
static SaraceSignedWord l_shipRaceResult;
static SaraceSignedWord l_btpIndex;
static SaraceSignedTable l_bodyAnmMode;
static SaraceSignedTable l_headAnmMode;
static SaraceSignedTable l_anmIndex;
static SaraceFloatTable l_anmMorf;
static SaraceFloatTable l_anmSpeed;

static SaraceBuoyInitialPos l_buoyInitialPos;
static SaraceHeaderZeros l_headerZeros;
static SaraceHeaderStatics l_headerStatics;
static SaracePiLimits l_piLimits;
static dCcD_SrcCyl l_cyl_src;

static const s16 PROC_COURSE_BUOY = 0x1C9;
// HD: the search is implemented in another translation unit.
static const u32 SARACE_JUDGE_BY_ID = 0x025E1234;

static dComIfG_inf_c* saraceGameInfo() { return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get()); }
static void copyWords(cXyz* dst, const cXyz* src) {
    auto& source = reinterpret_cast<const SaracePositionWords&>(*src);
    auto& result = reinterpret_cast<SaracePositionWords&>(*dst);
    u32 x = source.x, y = source.y, z = source.z;
    result.x = x;
    result.y = y;
    result.z = z;
}
// HD: the lfs/stfs copies quiet signaling NaNs and load every element before any store.
template <class D, class S> static void copyFloats(D* dst, const S* src, int n) {
    auto* source = reinterpret_cast<const be<f32>*>(src);
    auto* result = reinterpret_cast<be<f32>*>(dst);
    f32 values[12];
    for (int i = 0; i < n; ++i)
        values[i] = source[i];
    for (int i = 0; i < n; ++i)
        result[i] = values[i];
}
// HD: matrix lookup caches the array address before marking the buffer dirty.
static Mtx34* jointMtx(J3DModel* model, u32 index) {
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    Mtx34* matrices = gabi::at<Mtx34>(block->mpAnmMtx);
    block->mFlags |= 0x10;
    return matrices + index;
}
/* inline setAction(int (daNpc_Sarace_c::*)(void*), void*): GHS pointer-to-member compare, then
 * the old action is called with mActionStatus -1 and the new one with 0 */
static void setAction(daNpc_Sarace_c* a, SaraceActionFunc action) {
    u32 func = saraceActionAddress(action);
    u32 p = gabi::ea(a);
    s16 index = a->mAction.i;
    bool callOld = true;
    if (index == -1) {
        if (a->mAction.d == 0 && a->mAction.f == func)
            return;
    } else if (index == 0) {
        callOld = false;
    }
    if (callOld) {
        s16 delta = a->mAction.d;
        index = a->mAction.i;
        u32 receiver = p + (s32)delta;
        a->mActionStatus = daNpc_Sarace_c::ACTION_END;
        saraceCallAction(a->mAction, index, receiver);
    }
    a->mAction.d = 0;
    a->mAction.i = -1;
    a->mAction.f = func;
    a->mActionStatus = daNpc_Sarace_c::ACTION_INIT;
    saraceCallAction(a->mAction, -1, p + (s32)a->mAction.d);
}

static s32 btpFrameMax(daNpc_Sarace_c* a) {
    return reinterpret_cast<SaraceTexPatternView*>(a->mpTexPattern.get())->getFrameMax();
}
/* 022DBA64 */
BOOL nodeCallBack(J3DNode* node, s32 calcTiming) {
    if (calcTiming != J3DNodeCBCalcTiming_In)
        return TRUE;
    J3DModel* model = j3dSys.getModel();
    auto* actor = static_cast<daNpc_Sarace_c*>(model->getUserArea());
    if (!actor)
        return TRUE;
    u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
    mDoMtx_stack_c::copy(jointMtx(model, jntNo));
    if (jntNo == (u32)(s32)actor->m_jnt.getHeadJntNum()) {
        if (!l_offsetAttPosInitialized) {
            l_offsetAttPos.x = 0.0f;
            l_offsetAttPos.z = 0.0f;
            l_offsetAttPosInitialized = 1;
            l_offsetAttPos.y = 0.0f;
        }
        gabi::Local<cXyz> eyeOffset;
        eyeOffset->x = 24.0f;
        eyeOffset->y = 14.0f;
        eyeOffset->z = 0.0f;
        mDoMtx_stack_c::multVec(&l_offsetAttPos, &actor->mAttPos);
        mDoMtx_stack_c::XrotM(actor->m_jnt.getHead_y());
        mDoMtx_stack_c::ZrotM((s16)-actor->m_jnt.getHead_x());
        mDoMtx_stack_c::multVec(eyeOffset.get(), &actor->mEyePos);
    } else if (jntNo == (u32)(s32)actor->m_jnt.getBackboneJntNum()) {
        mDoMtx_stack_c::XrotM(actor->m_jnt.getBackbone_y());
        mDoMtx_stack_c::ZrotM((s16)-actor->m_jnt.getBackbone_x());
    }
    MTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    copyFloats(jointMtx(model, jntNo), mDoMtx_stack_c::get(), 12);
    return TRUE;
}

/* 022DBCBC */
// HD: GameCube bool is forwarded as the complete incoming register.
BOOL daNpc_Sarace_c::initTexPatternAnm(u32 modify) {
    J3DModelData* modelData = mpHeadMorf->getModel()->getModelData();
    auto* pattern = static_cast<J3DAnmTexPattern*>(dComIfG_getObjectRes("Sarace", l_btpIndex));
    mpTexPattern = pattern;
    if (!pattern) {
        JUTAssertion::showAssert("d_a_npc_sarace.cpp", 0xFC, "m_btp != (0)");
        pattern = mpTexPattern;
    }
    if (!saraceInitTexPattern(&mBtpAnm, modelData, pattern, 1, J3DFrameCtrl::EMode_LOOP, 1.0f, 0,
                              -1, modify, 0))
        return FALSE;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 022DC7E8 */
void daNpc_Sarace_c::playTexPatternAnm() {
    if (cLib_calcTimer(&mBlinkTimer) != 0)
        return;
    s32 frameMax = btpFrameMax(this);
    if ((s32)mBtpFrame >= frameMax) {
        s32 max = btpFrameMax(this);
        mBtpFrame = (u8)(mBtpFrame - max);
        f32 r = cM_rndF(100.0f);
        mBlinkTimer = (s16)gabi::ftoi(r + 30.0f);
    } else {
        mBtpFrame = (u8)(mBtpFrame + 1);
    }
}

/* 022DC2C0 */
// HD: the s8 animation argument remains a full register for table indexing and comparison.
void daNpc_Sarace_c::setAnm(s32 idx, f32 morf) {
    s32 cur = mAnmIdx;
    if (morf < 0.0f)
        morf = (f32)l_anmMorf.values[idx];
    if ((u32)idx == (u32)cur || cur == -1)
        return;
    mAnmIdx = (s8)idx;
    dNpc_setAnm(mpMorf.get(), (s32)l_anmIndex.values[idx], morf, (f32)l_anmSpeed.values[idx],
                (s32)l_bodyAnmMode.values[idx], -1, "Sarace");
    s32 i = mAnmIdx;
    dNpc_setAnm(mpHeadMorf.get(), (s32)l_anmIndex.values[i], morf, (f32)l_anmSpeed.values[i],
                (s32)l_headAnmMode.values[i], -1, "Sarace");
}

/* 022DCCD0 */
// chkAttention(cXyz pos, s16 angle): the cXyz is passed by pointer to a copy
u8 daNpc_Sarace_c::chkAttention(cXyz* pos, s16 angle) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = (f32)player->current.pos.z - pos->z;
    f32 radius = (f32)l_HIO.mNpc.mMaxAttnDistXZ;
    f32 dx = (f32)player->current.pos.x - pos->x;
    s32 limit = (s16)l_HIO.mNpc.mMaxAttnAngleY;
    f32 dist = std_sqrtf(fmadds(dx, dx, dz * dz));
    s16 target = cM_atan2s(dx, dz);
    if (mbAttention) {
        radius = radius + 40.0f;
        limit += 0x71C;
    }
    s16 diff = (s16)(target - angle);
    s32 mag = diff < 0 ? -(s32)diff : diff;
    return limit > mag && radius > dist;
}

/* 022DCA3C */
void daNpc_Sarace_c::eventOrder() {
    s8 order = mEventOrder;
    if (order == ORDER_EXPCAM) {
        saraceOrderOtherEvent(this, "SARACE_EXPCAM", 1, 0xFFFF);
        return;
    }
    if (order == ORDER_SPEAK || order == ORDER_TALK) {

        order = mEventOrder;
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (order == ORDER_SPEAK)
            fopAcM_orderSpeakEvent(this);
    }
}

/* 022DC8AC */
// checkOrder (unnamed by the matcher; the matcher's checkOrder is wrong, GameCube 0x7D8)
void daNpc_Sarace_c::checkOrder() {
    u16 condition = (u16)eventInfo.mCommand;
    if (condition == dEvtCmd_INDEMO_e) {
        if (mEventOrder == ORDER_EXPCAM) {
            setAction(this, &daNpc_Sarace_c::event_endCheck_action);
            mEventOrder = ORDER_NONE;
        }
    } else if (condition == dEvtCmd_INTALK_e) {
        s8 order = mEventOrder;
        if (order == ORDER_SPEAK || order == ORDER_TALK) {
            mEventOrder = ORDER_NONE;
            mbTalkRequest = 1;
        }
    }
}

/* 022DCDFC */
u16 daNpc_Sarace_c::next_msgStatus(be<u32>* msgNo) {
    u32 msg = *msgNo;
    msg_class* msgObject = fopNpc_npc_c::mpCurrMsg;
    switch (msg) {
    case 0xFA1:
    case 0xFA2:
    case 0xFA5:
    case 0xFA7:
    case 0xFB1:
    case 0xFB2:
    case 0xFB4:
    case 0xFB5:
    case 0xFB7:
        *msgNo = msg + 1;
        break;
    case 0xFB8:
        *msgNo = 0xFB3;
        break;
    case 0xFA3: {
        s32 choice = (s32)msgObject->mSelectNum;
        if (choice == 2) {
            *msgNo = 0xFA4;
            break;
        }
        dSv_info_c* save = dComIfGs_info();
        if (choice == 0) {
            if ((u16)save->mSavedata.mPlayer.mPlayerStatusA.mRupee < 30) {
                *msgNo = 0xFAF;
                break;
            }
            dComIfG_inf_c* gameInfo = saraceGameInfo();
            gameInfo->play.mItemRupeeCount = (s32)gameInfo->play.mItemRupeeCount - 30;
            dComIfGs_offEventBit(0x2820);
            *msgNo = 0xFB0;
        } else if (!save->mSavedata.mEvent.isEventBit(0x2808)) {
            dComIfGs_onEventBit(0x2808);
            *msgNo = 0xFAD;
        } else {
            *msgNo = 0xFAE;
        }
        break;
    }
    case 0xFA8:
        if (dComIfGs_isEventBit(0x2820)) {
            *msgNo = 0xFAA;
            break;
        }
        *msgNo = 0xFA9;
        break;
    case 0xFAA:
        *msgNo = 0xFA9;
        break;
    case 0xFA9:
    case 0xFB3:
        *msgNo = 0xFA3;
        break;
    default:
        return fopMsgStts_MSG_ENDS_e;
    }
    return fopMsgStts_MSG_CONTINUES_e;
}

/* 022DCFC8 */
u32 daNpc_Sarace_c::getMsg() {
    u32 msg = mEventMsgNo;
    if (msg != 0) {
        if (msg == 0xFB4) {
            s32 result = (s32)l_shipRaceResult;
            if (result == 1) {
                mEventMsgNo = 0;
                return 0xFB7;
            }
            if (result == 3) {
                mEventMsgNo = 0;
                return 0xFB1;
            }
            s16 rupee = (s16)(s32)l_shipRaceRupee;
            saraceGameInfo()->play.mMsgCountNumber = rupee;
            msg = mEventMsgNo;
        }
        mEventMsgNo = 0;
        return msg;
    }
    if (!dComIfGs_isEventBit(0x2810)) {
        dComIfGs_onEventBit(0x2810);
        return 0xFA1;
    }
    if (dComIfGs_isEventBit(0x2840))
        return 0xFA2;
    return 0xFA1;
}

/* 022DD0B0 */
// anmAtr(u16): the attribute is ignored, the requested animation comes from the play object
void daNpc_Sarace_c::anmAtr(u16 attr) {
    u8 req = dComIfGp_getMesgAnimeAttrInfo();
    if (req == 0)
        setAnm(0, -1.0f);
    else if (req == 1)
        setAnm(1, (f32)l_HIO.mTalkMorf);
    else if (req == 2)
        setAnm(2, (f32)l_HIO.mWaitMorf);
    saraceGameInfo()->play.mMesgAnime = 0xFF;
}

/* 022DC3C8 */
BOOL daNpc_Sarace_c::CreateInit() {
    u16 ay = (u16)current.angle.y, az = (u16)current.angle.z, ax = (u16)current.angle.x;
    mHomeAngle[1] = ay;
    mHomeAngle[0] = ax;
    gravity = -30.0f;
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
    mHomeAngle[2] = az;
    setAction(this, &daNpc_Sarace_c::wait_action);
    auto& source = reinterpret_cast<SaracePositionWords&>(current.pos);
    auto& att = reinterpret_cast<SaracePositionWords&>(mAttPos);
    auto& eye = reinterpret_cast<SaracePositionWords&>(mEyePos);
    u32 z = source.z, x = source.x;
    att.z = z;
    att.x = x;
    eye.z = z;
    u32 y = source.y;
    eye.x = x;
    eye.y = y;
    att.y = y;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    setCollision(60.0f, 150.0f);
    mInitState = 0;
    mEventCut.setActorInfo2("Sarace", this);
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAD;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAD;
    set_mtx();
    mEventMsgNo = 0;
    mBuoyId[1] = 0xFFFFFFFF;
    mBuoyId[0] = 0xFFFFFFFF;
    setAnm(0, -1.0f);
    if (dComIfGp_getStartStagePoint() == 1 && dComIfGp_getStartStageRoomNo() == 0x30 &&
        (s32)l_shipRaceResult != 0) {
        mEventOrder = ORDER_SPEAK;
        mEventMsgNo = 0xFB4;
        fopAcM_orderSpeakEvent(this);
    }
    mpMorf->play(&eyePos, 0, 0);
    mpMorf->calc();
    mpHeadMorf->play(nullptr, 0, 0);
    mpHeadMorf->calc();
    return TRUE;
}

/* 022DD148 */
void daNpc_Sarace_c::setAttention() {
    copyFloats(&eyePos, &mEyePos, 3);
    f32 y = gabi::fadds_ppc((f32)mAttPos.y, (f32)l_HIO.mNpc.mAttnYOffset);
    f32 z = (f32)mAttPos.z, x = (f32)mAttPos.x;
    attention_info.position.z = z;
    attention_info.position.y = y;
    attention_info.position.x = x;
}

/* 022DD188 */
void daNpc_Sarace_c::lookBack() {
    f32 eyeX = 0.0f, eyeY = 0.0f, eyeZ = 0.0f;
    cXyz* target = nullptr;
    s16 yrot = (s16)current.angle.y;
    s8 mode = mMode;
    gabi::Local<cXyz> eye, tgt, playerEye, playerEye2;
    if ((u32)(s32)mode >= 1 && (u32)(s32)mode <= 2) {
        bool track;
        if (mode == MODE_TALK) {
            m_jnt.mbTrn = 1;
            track = mbAttention != 0;
            if (!track) {
                dNpc_playerEyePos(playerEye.get(), l_HIO.mNpc.m04);
                copyWords(tgt.get(), playerEye.get());
                s16 angle = cLib_targetAngleY(&current.pos, tgt.get());
                cLib_addCalcAngleS2(&current.angle.y, angle, 4, 0x1800);
                track = mbAttention != 0;
            }
        } else {
            track = mbAttention != 0;
        }
        if (track) {
            dNpc_playerEyePos(playerEye2.get(), l_HIO.mNpc.m04);
            eyeY = (f32)eyePos.y;
            copyWords(tgt.get(), playerEye2.get());
            eyeX = (f32)current.pos.x;
            target = tgt.get();
            eyeZ = (f32)current.pos.z;
        }
    }
    eye->x = eyeX;
    eye->y = eyeY;
    eye->z = eyeZ;
    if ((u8)m_jnt.mbTrn == 0) {
        mLookAngle = 0;
        saraceLookAtTarget(&m_jnt, &current.angle.y, target, eye.get(), yrot,
                           l_HIO.mNpc.mMaxHeadTurnVel, 1);
    } else {
        saraceLookAtTarget(&m_jnt, &current.angle.y, target, eye.get(), yrot,
                           l_HIO.mNpc.mMaxHeadTurnVel, 0);
    }
}

/* 022DD35C */
void daNpc_Sarace_c::wait01() {
    if (mbTalkRequest) {
        mMode = MODE_TALK;
        return;
    }
    if (mEventOrder == ORDER_NONE)
        mEventOrder = ORDER_TALK;
}

/* 022DD390 */
void daNpc_Sarace_c::talk01() {
    if (talk(1) == fopMsgStts_BOX_CLOSED_e) {
        u32 msg = mCurrMsgNo;
        mMode = MODE_WAIT;
        if (msg == 0xFA4) {
            /* static cXyz l_buoyPos[2], initialised on first use */
            if (!(u32)l_buoyPosInitialized) {
                l_buoyPosInitialized = 1;
                l_buoyPos.positions[1].y = (f32)l_buoyInitialPos.values[1];
                l_buoyPos.positions[0].y = (f32)l_buoyInitialPos.values[1];
                l_buoyPos.positions[0].z = (f32)l_buoyInitialPos.values[2];
                l_buoyPos.positions[1].z = (f32)l_buoyInitialPos.values[4];
                l_buoyPos.positions[0].x = (f32)l_buoyInitialPos.values[0];
                l_buoyPos.positions[1].x = (f32)l_buoyInitialPos.values[3];
            }
            dComIfG_inf_c* gameInfo = saraceGameInfo();
            gameInfo->play.mEvtCtrl.mEventFlag |= 8;
            mEventOrder = ORDER_EXPCAM;
            mBuoyId[0] = fopAcM_create(PROC_COURSE_BUOY, 0x017F0101, &l_buoyPos.positions[0], -1,
                                       nullptr, nullptr, -1, 0);
            u32 id = fopAcM_create(PROC_COURSE_BUOY, 0x007F0101, &l_buoyPos.positions[1], -1,
                                   nullptr, nullptr, -1, 0);
            mbTalkRequest = 0;
            mBuoyId[1] = id;
            return;
        }
        if (msg == 0xFB0) {
            dComIfGs_onEventBit(0x2840);
            saraceSetNextStage("Ocean", 1, 0, 0, 0.0f, 0, 1, 0);
            setAction(this, &daNpc_Sarace_c::dummy_action);
            mbTalkRequest = 0;
            return;
        }
        dComIfG_inf_c* gameInfo = saraceGameInfo();
        gameInfo->play.mEvtCtrl.mEventFlag |= 8;
        l_shipRaceRupee = 0;
        l_shipRaceResult = 0;
        setAnm(0, -1.0f);
        mbTalkRequest = 0;
        return;
    }
    if (mCurrMsgNo == 0xFB6 && dComIfGp_checkMesgSendButton() == 1) {
        s32 rupee = (s32)l_shipRaceRupee;
        dComIfG_inf_c* gameInfo = saraceGameInfo();
        gameInfo->play.mItemRupeeCount = (s32)gameInfo->play.mItemRupeeCount + rupee;
        u32 count = dComIfGs_getEventReg(0xAAFF);
        s32 next = (s32)count + 1;
        if (next > 12)
            next = 12;
        u8 value = (u8)next;
        dComIfGs_setEventReg(0xAAFF, value);
        if (count != (u32)value)
            dComIfGs_onEventBit(0x2820);
    }
}

/* 022DD728 */
BOOL daNpc_Sarace_c::dummy_action(void* prm) {
    if (mActionStatus == ACTION_INIT) {
        mMode = MODE_WAIT;
        mActionStatus = (s8)(mActionStatus + 1);
    }
    return TRUE;
}

/* 022DD750 */
BOOL daNpc_Sarace_c::wait_action(void* prm) {
    s8 status = mActionStatus;
    if (status == ACTION_INIT) {
        mMode = MODE_WAIT;
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (status == ACTION_END)
        return TRUE;
    gabi::Local<cXyz> pos;
    s16 angle = (s16)((s16)current.angle.y + (s16)m_jnt.mAngles[0][1] + (s16)m_jnt.mAngles[1][1]);
    copyFloats(pos.get(), &current.pos, 3);
    u8 attention = chkAttention(pos.get(), angle);
    s8 mode = mMode;
    mbAttention = attention;
    if (mode == MODE_WAIT)
        wait01();
    else if (mode == MODE_TALK)
        talk01();
    lookBack();
    setAttention();
    return TRUE;
}

/* 022DD844 */
BOOL daNpc_Sarace_c::event_endCheck_action(void* prm) {
    s8 status = mActionStatus;
    if (status == ACTION_INIT) {
        mActionStatus = ACTION_EXEC;
        return TRUE;
    }
    if (status == ACTION_END)
        return TRUE;
    if (!saraceEventEndCheck(dComIfGp_getPEvtManager(), "SARACE_EXPCAM"))
        return TRUE;
    mEventMsgNo = 0xFA7;
    dComIfG_inf_c* gameInfo = saraceGameInfo();
    gameInfo->play.mEvtCtrl.mEventFlag |= 8;
    mEventOrder = ORDER_SPEAK;
    setAction(this, &daNpc_Sarace_c::wait_action);
    gabi::Local<u32> id0, id1;
    fopAc_ac_c* buoy0 = nullptr;
    fopAc_ac_c* buoy1 = nullptr;
    u32 id = mBuoyId[0];
    *reinterpret_cast<be<u32>*>(id0.get()) = id;
    if (id != 0xFFFFFFFF)
        buoy0 = fopAcIt_Judge(SARACE_JUDGE_BY_ID, id0.get());
    id = mBuoyId[1];
    *reinterpret_cast<be<u32>*>(id1.get()) = id;
    if (id != 0xFFFFFFFF)
        buoy1 = fopAcIt_Judge(SARACE_JUDGE_BY_ID, id1.get());
    if (buoy0)
        reinterpret_cast<SaraceBuoyView*>(buoy0)->mbDeleteRequested = 1;
    if (buoy1)
        reinterpret_cast<SaraceBuoyView*>(buoy1)->mbDeleteRequested = 1;
    return TRUE;
}

/* 022DC154 */
// set_mtx (unnamed by the matcher; GameCube 0x173C)
void daNpc_Sarace_c::set_mtx() {
    J3DModel* model = mpMorf->getModel();
    J3DModel* headModel = mpHeadMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
    mpMorf->calc();
    headModel->setBaseTRMtx(jointMtx(model, (u32)(s32)m_jnt.getHeadJntNum()));
    mpHeadMorf->calc();
}

/* 022DCBFC */
BOOL daNpc_Sarace_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModel* headModel = mpHeadMorf->getModel();
    J3DModelData* headData = headModel->getModelData();
    g_env_light.settingTevStruct(0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(model, &tevStr);
    g_env_light.setLightTevColorType(headModel, &tevStr);
    saraceEntryTexPattern(&mBtpAnm, headData, (u32)mBtpFrame);
    mpMorf->entryDL();
    mpHeadMorf->entryDL();
    reinterpret_cast<SaraceModelDataView*>(headData)->mTexPatternEntries = 0;
    dSnap_RegistFig(0x82, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 022DCA94 */
BOOL daNpc_Sarace_c::_execute() {
    u32 p = gabi::ea(this);
    s16 hio[9];
    hio[0] = l_HIO.mNpc.mMaxHeadX;
    hio[1] = l_HIO.mNpc.mMaxBackboneX;
    hio[2] = l_HIO.mNpc.mMaxHeadY;
    hio[3] = l_HIO.mNpc.mMaxBackboneY;
    hio[4] = l_HIO.mNpc.mMinHeadX;
    hio[5] = l_HIO.mNpc.mMinBackboneX;
    hio[6] = l_HIO.mNpc.mMinHeadY;
    hio[7] = l_HIO.mNpc.mMinBackboneY;
    hio[8] = l_HIO.mNpc.mMaxTurnStep;
    m_jnt.setParam(hio[1], hio[3], hio[5], hio[7], hio[0], hio[2], hio[4], hio[6], hio[8]);
    playTexPatternAnm();
    mpMorf->play(nullptr, 0, 0);
    mpHeadMorf->play(nullptr, 0, 0);
    checkOrder();
    s16 index = mAction.i, delta = mAction.d;
    u32 receiver = p + (s32)delta;
    saraceCallAction(mAction, index, receiver);
    mEventCut.cutProc();
    eventOrder();
    saraceMoveF(this, &mStts);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = saraceGroundRoom(dComIfG_Bgsp(), &mObjAcch.m_gnd);
    tevStr.mEnvrIdxOverride = saraceGroundColor(dComIfG_Bgsp(), &mObjAcch.m_gnd);
    set_mtx();
    setCollision(60.0f, 150.0f);
    return TRUE;
}

/* 022DC77C */
BOOL daNpc_Sarace_c::_delete() {
    dComIfG_resDelete(&mPhs, "Sarace");
    mDoExt_McaMorf* morf = mpMorf;
    if (morf)
        morf->stopZelAnime();
    s8 child = (s8)l_HIO.mNo;
    if (child >= 0) {
        mDoHIO_deleteChild(child);
        l_HIO.mNo = -1;
    }
    return TRUE;
}

/* 022DC150 */
// CallbackCreateHeap: a tail branch to CreateHeap
BOOL daNpc_Sarace_CallbackCreateHeap(daNpc_Sarace_c* a) { return a->CreateHeap(); }

/* 022DC65C */
s32 daNpc_Sarace_c::_create() {
    u32 flags = (u32)actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            __ct(static_cast<fopNpc_npc_c*>(this));
            __vtbl = 0x10021B28;
            mDoExt_btpAnm::ct(&mBtpAnm);
            flags = (u32)actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "Sarace");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(
            this, reinterpret_cast<BOOL (*)(fopAc_ac_c*)>(daNpc_Sarace_CallbackCreateHeap), 0x2760))
        return cPhs_ERROR_e;
    J3DModel* model = mpMorf->getModel();
    fopAcM_SetMtx(this, model->getBaseTRMtx());
    if ((s8)l_HIO.mNo < 0)
        l_HIO.mNo = mDoHIO_createChild(l_hioLabel.text, &l_HIO);
    if (!CreateInit())
        return cPhs_ERROR_e;
    return phase;
}

/* 022DBDC0 */
BOOL daNpc_Sarace_c::CreateHeap() {
    auto* modelData = static_cast<J3DModelData*>(dComIfG_getObjectRes("Sarace", RES_BODY_MODEL));
    if (!modelData)
        JUTAssertion::showAssert("d_a_npc_sarace.cpp", 0x3F8, "modelData != (0)");
    auto* bck = static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Sarace", RES_BODY_ANM));
    mDoExt_McaMorf* morf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (morf)
        mpMorf = morf;
    if (!morf || !morf->getModel()) {
        mpMorf = nullptr;
        return FALSE;
    }
    s8 head = saraceJointNameIndex(modelData->getJointName(), "head");
    m_jnt.setHeadJntNum(head);
    if (head < 0)
        JUTAssertion::showAssert("d_a_npc_sarace.cpp", 0x409, "m_jnt.getHeadJntNum() >= 0");
    s8 backbone = saraceJointNameIndex(modelData->getJointName(), "backbone");
    m_jnt.setBackboneJntNum(backbone);
    if (backbone < 0)
        JUTAssertion::showAssert("d_a_npc_sarace.cpp", 0x40B, "m_jnt.getBackboneJntNum() >= 0");
    auto* headData = static_cast<J3DModelData*>(dComIfG_getObjectRes("Sarace", RES_HEAD_MODEL));
    if (!headData)
        JUTAssertion::showAssert("d_a_npc_sarace.cpp", 0x419, "headModelData != (0)");
    auto* headBck = static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Sarace", RES_HEAD_ANM));
    mDoExt_McaMorf* headMorf =
        mDoExt_McaMorf::create(nullptr, headData, nullptr, nullptr, headBck,
                               J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    mpHeadMorf = headMorf;
    if (!headMorf || !headMorf->getModel())
        return FALSE;
    mTexPatternState = 0;
    if (!initTexPatternAnm(0))
        return FALSE;
    for (u16 i = 0; i < modelData->getJointNum(); i = (u16)(i + 1)) {
        if ((u32)i == (u32)(s32)m_jnt.getHeadJntNum() ||
            (u32)i == (u32)(s32)m_jnt.getBackboneJntNum()) {
            mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack);
        }
    }
    mpMorf->getModel()->setUserArea(reinterpret_cast<uintptr_t>(this));
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 022DC778 */
s32 daNpc_Sarace_Create(daNpc_Sarace_c* a) { return a->_create(); }

/* 022DC7E4 */
BOOL daNpc_Sarace_Delete(daNpc_Sarace_c* a) { return a->_delete(); }

/* 022DCBF8 */
BOOL daNpc_Sarace_Execute(daNpc_Sarace_c* a) { return a->_execute(); }

/* 022DCCCC */
BOOL daNpc_Sarace_Draw(daNpc_Sarace_c* a) { return a->_draw(); }

/* 022DDBD0 */
BOOL daNpc_Sarace_IsDelete(daNpc_Sarace_c* a) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 022DDA2C */
void* daNpc_Sarace_HIO_ct(void* self) {
    auto* hio = static_cast<daNpc_Sarace_HIO_c*>(self);
    if (gabi::ea(hio) == 0) {
        hio = static_cast<daNpc_Sarace_HIO_c*>(operator_new((u32)sizeof(daNpc_Sarace_HIO_c)));
        if (!hio)
            return nullptr;
    }
    hio->__vtbl = 0x10021980;
    saraceNpcHioInit(&hio->mNpc);
    hio->mNpc.mMaxBackboneX = 0;
    hio->mNpc.m04 = -20.0f;
    hio->mNpc.mMaxAttnDistXZ = 400.0f;
    hio->mNpc.mMinHeadY = -6000;
    hio->mNpc.m22 = 0;
    hio->mNpc.mMinBackboneX = -2000;
    hio->mNpc.mMaxHeadTurnVel = 0x640;
    hio->mNpc.mMaxHeadX = 4000;
    hio->mNpc.mMaxHeadY = 6000;
    hio->mNpc.mMinBackboneY = -7000;
    hio->mNpc.mMaxTurnStep = 1000;
    hio->mNpc.mMaxAttnAngleY = 0x4000;
    hio->mNpc.mAttnYOffset = 45.0f;
    hio->mWaitMorf = 8.0f;
    hio->mNpc.mMaxBackboneY = 7000;
    hio->mTalkMorf = 11.0f;
    hio->mNo = -1;
    hio->mNpc.mMinHeadX = -2000;
    return hio;
}

/* 022DDB1C */
// __sinit_d_a_npc_sarace_cpp
void daNpc_Sarace_sinit() {
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(0x101C6258);
    l_headerStatics.minAngle = (f32)l_piLimits.minAngle;
    l_headerStatics.maxAngle = (f32)l_piLimits.maxAngle;
    saraceHeaderAngleInit(&l_headerStatics.angle);
    __register_global_object(0x101C6264);
    saraceHeaderCircleInit(&l_headerStatics.circle);
    __register_global_object(0x101C6270);
    daNpc_Sarace_HIO_ct(&l_HIO);
}

/* 022DDBBC */
// SafeString deleting destructor (this unit's copy)
void daNpc_Sarace_SafeString_delete(void* self, u32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}

/* 022DDBD8 */
// daNpc_Sarace_c deleting destructor
void daNpc_Sarace_dtor(daNpc_Sarace_c* a, u32 flags) {
    if (!a)
        return;
    __dt(&a->mCyl);
    __dt(&a->mStts);
    __dt(&a->mAcchCir);
    __dt(&a->mObjAcch);
    __dt(static_cast<fopAc_ac_c*>(a));
    if (flags & 1)
        operator_delete(a);
}

/* 022DDC74 */
// SafeString empty virtual (this unit's copy)
void daNpc_Sarace_SafeString_empty(void* self) {}
/* WWHD glue (tools/verify/autoglue.py): this translation unit
ext 025D77DC void saraceOrderOtherEvent(daNpc_Sarace_c*, const char*, u32, u32)
ext 025D6870 void saraceMoveF(daNpc_Sarace_c*, dCcD_Stts*)
ext 024EF130 u8 saraceGroundRoom(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 saraceGroundColor(dBgS*, cBgS_PolyInfo*)
ext 0259DED0 void saraceLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u32)
ext 0252012C void saraceSetNextStage(const char*, s32, s32, s32, f32, s32, s32, s32)
ext 0254457C BOOL saraceEventEndCheck(dEvent_manager_c*, const char*)
ext 0259DA18 void saraceNpcHioInit(dNpc_HIO_c*)
ext 028ED6F8 void saraceHeaderAngleInit(u8*)
ext 028EAB2C void saraceHeaderCircleInit(u8*)
ext 025E789C s32 saraceInitTexPattern(mDoExt_btpAnm*, J3DModelData*, J3DAnmTexPattern*, u32, s32, f32, s32, s32, u32, u32)
ext 025E7B3C void saraceEntryTexPattern(mDoExt_btpAnm*, J3DModelData*, u32)
ext 027DF9B0 s8 saraceJointNameIndex(JUTNameTab*, const char*)
str 100219E8
str 100219B8 in 022DBCBC
str 10021A80 in 022DC2C0
str 10021A98 in 022DC3C8
str 10021AA0 in 022DC65C
str 10021AC0 in 022DC77C
str 100219F8
str 100219C0 in 022DBCBC
str 10021ACC
str 10021AFC in 022DD844
str 100219D4
str 100219F0
str 10021A0C
str 10021A20
str 10021A3C
str 10021A48
str 10021A68
data l_hioLabel 10021AA8
str 10021AF4
data l_HIO 10468620
data l_buoyInitialPos 10021AE0
data l_headerZeros 10468670
data l_headerStatics 10468614
data l_piLimits 10021B20
data l_cyl_src 101C61F0
data l_offsetAttPos 10468680
data l_buoyPos 10468658
data l_offsetAttPosInitialized 1046868C
data l_buoyPosInitialized 10468690
data l_shipRaceRupee 101D5F18
data l_shipRaceResult 101D5F1C
data l_btpIndex 10021944
data l_bodyAnmMode 10021990
data l_headAnmMode 1002199C
data l_anmIndex 101C6234
data l_anmMorf 101C6240
data l_anmSpeed 101C624C
*/
