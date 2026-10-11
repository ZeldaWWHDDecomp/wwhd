/**
 * d_a_npc_p1.cpp (WWHD)
 * NPC - Gonzo, Senza, & Nudge (Tetra's pirates)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * _create/CreateHeap: d_a_npc_p1_create.cpp; kaji_anm, lookBack: d_a_npc_p1_kaji.cpp; actions:
 * d_a_npc_p1_action.cpp (normal/confuse/talk) and d_a_npc_p1_speak.cpp (speak/p1c_speak).
 */
#include "d/actor/d_a_npc_p1.h"

using namespace p1_cpp;

static daNpc_P1_HIO_c l_HIO;
static u32 l_msgId;
static msg_class* l_messageManager;
static u8 l_snap_idx_tbl[3] = {115, 116, 117};
static char* ActionNames[2] = {"4013_msg", "4014_msg"};
static char* cut_name_tbl[5] = {"4013_msg", "4014_msg", "SETANM", "HEADSWING", "TALKMSG"};
static p1_cpp::CylinderSource l_cyl_src;
static u32 l_demoObject;
static u32 l_demoFrame;

/* 022B57C8 */
static daNpc_P1_childHIO_c* daNpc_P1_childHIO_ct(daNpc_P1_childHIO_c* self) {
    // HD: the GHS constructor allocates when passed NULL; its vtable is at the end.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self == nullptr) {
        self = (daNpc_P1_childHIO_c*)operator_new(0x30);
        if (self == nullptr)
            return self;
    }
    self->__vtbl = 0x1001F8E4;
    return self;
}

/* 022B5808 */
static daNpc_P1_HIO_c* daNpc_P1_HIO_ct(daNpc_P1_HIO_c* self) {
    // HD: the GHS constructor allocates when passed NULL; its vtable is at the end.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self == nullptr) {
        self = (daNpc_P1_HIO_c*)operator_new(0x9C);
        if (self == nullptr)
            return self;
    }
    self->__vtbl = 0x1001F8F4;
    p1ConstructArray(self->children, 3, 0x30, 0x022B57C8); /* __construct_array */
    daNpc_P1_childHIO_c* c = self->children;
    c[0].mAttnYPosOffset = 50.0f;
    c[0].mMaxHeadX = 0x9C4;
    c[0].mMinHeadX = (s16)0xF63C;
    c[0].mMaxBackboneX = 2000;
    c[0].mMinBackboneX = (s16)0xF830;
    c[0].mMaxHeadY = 5000;
    c[0].mMinHeadY = (s16)0xEC78;
    c[0].mMaxBackboneY = 8000;
    c[0].mMinBackboneY = (s16)0xE0C0;
    c[0].mMaxTurnStep = 1000;
    c[0].mLookBackTargetY = 0x708;
    c[0].mUnused20 = 0;
    c[0].mMaxTalkDist = 300.0f;
    c[0].mMorfBackup = 8.0f;
    c[0].mUnused2C = 8.0f;
    c[1].mAttnYPosOffset = 45.0f;
    c[1].mMaxHeadX = 0x9C4;
    c[1].mMinHeadX = (s16)0xF63C;
    c[1].mMaxBackboneX = 2000;
    c[1].mMinBackboneX = (s16)0xF830;
    c[1].mMaxHeadY = 10000;
    c[1].mMinHeadY = (s16)0xD8F0;
    c[1].mMaxBackboneY = 8000;
    c[1].mMinBackboneY = (s16)0xE0C0;
    c[1].mMaxTurnStep = 1000;
    c[1].mLookBackTargetY = 0x708;
    c[1].mUnused20 = 0;
    c[1].mMaxTalkDist = 250.0f;
    c[1].mMorfBackup = 8.0f;
    c[1].mUnused2C = 8.0f;
    c[2].mAttnYPosOffset = 45.0f;
    c[2].mMaxHeadX = 0x9C4;
    c[2].mMinHeadX = (s16)0xF63C;
    c[2].mMaxBackboneX = 2000;
    c[2].mMinBackboneX = (s16)0xF830;
    c[2].mMaxHeadY = 5000;
    c[2].mMinHeadY = (s16)0xEC78;
    c[2].mMaxBackboneY = 8000;
    c[2].mMinBackboneY = (s16)0xE0C0;
    c[2].mMaxTurnStep = 1000;
    c[2].mLookBackTargetY = 0x708;
    c[2].mUnused20 = 0;
    c[2].mMaxTalkDist = 250.0f;
    c[2].mMorfBackup = 7.0f;
    c[2].mUnused2C = 16.0f;
    self->mNo = -1;
    self->m8 = -1;
    return self;
}

/* 022B22B8 */
void daNpc_P1_c::setAnimFromMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((mAnmNum == 10 || mAnmNum == 11) && !p1_checkEnd(mpMorf, 1.0f)) {
        return;
    }
    if (mAnmNum == 8 && p1_checkEnd(mpMorf, 1.0f)) {
        m66C = m66C - 1;
        if (m66C <= 0) {
            setAnm(4, 15.0f);
        }
    } else if (mAnmNum == 12 && p1_checkEnd(mpMorf, 1.0f)) {
        m66C = m66C - 1;
        if (m66C <= 0) {
            setAnm(9, 8.0f);
        }
    }
    int iVar1;
    switch (dComIfGp_getMesgAnimeAttrInfo()) {
    case 0:
        iVar1 = mType != TYPE_P1C_e ? 0 : 4;
        break;
    case 1:
        iVar1 = 1;
        break;
    case 2:
        iVar1 = 2;
        break;
    case 3:
        iVar1 = 3;
        break;
    case 4:
        iVar1 = mType != TYPE_P1C_e ? 9 : 4;
        break;
    case 5:
        iVar1 = 10;
        break;
    case 6:
        iVar1 = 0xB;
        break;
    case 7:
        iVar1 = 0xC;
        m66C = 4;
        break;
    case 8:
        iVar1 = 0xD;
        break;
    case 9:
        iVar1 = 0xE;
        break;
    case 10:
        iVar1 = 0xF;
        break;
    case 0xB:
        iVar1 = 0x10;
        break;
    case 0xC:
        iVar1 = 4;
        break;
    case 0xD:
        iVar1 = 5;
        break;
    case 0xE:
        iVar1 = 6;
        break;
    case 0xF:
        iVar1 = 7;
        break;
    case 0x10:
        iVar1 = 8;
        m66C = 6;
        break;
    default:
        return;
    }
    dComIfGp_setMesgAnimeAttrInfo(0xFF);
    setAnm(iVar1, -1.0f);
}

/* 022B1758 */
BOOL daNpc_P1_c::setAnm(int i_anm, f32 i_morf) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((u32)mAnmNum == (u32)i_anm) {
        return FALSE;
    }
    mAnmNum = i_anm;
    if (i_morf < 0.0f) {
        i_morf = p1_child(mType)->mMorfBackup;
    }
    /* dRes_INDEX_P1_BCK_* (HD indices), "P1" at 0x1001F9E0 */
    s32 idx;
    s32 attr = J3DFrameCtrl::EMode_LOOP; /* J3DFrameCtrl::EMode_LOOP */
    switch ((u32)i_anm) {
    case 0:
        idx = dRes_INDEX_P1_BCK_WAIT_e;
        break; /* WAIT */
    case 1:
        idx = dRes_INDEX_P1_BCK_TALK_e;
        break; /* TALK */
    case 2:
        idx = dRes_INDEX_P1_BCK_TALK02_e;
        break; /* TALK02 */
    case 3:
        idx = dRes_INDEX_P1_BCK_TALK03_e;
        break; /* TALK03 */
    case 9:
        idx = dRes_INDEX_P1_BCK_WAIT02_e;
        break; /* WAIT02 */
    case 10:
        idx = dRes_INDEX_P1_BCK_OMOKJ_e;
        attr = J3DFrameCtrl::EMode_NONE;
        break; /* OMOKJ */
    case 11:
        idx = dRes_INDEX_P1_BCK_TORIKJ_e;
        attr = J3DFrameCtrl::EMode_NONE;
        break; /* TORIKJ */
    case 12:
        idx = dRes_INDEX_P1_BCK_ANGRY_e;
        break; /* ANGRY */
    case 13:
        idx = dRes_INDEX_P1_BCK_TALK04_e;
        break; /* TALK04 */
    case 14:
        idx = dRes_INDEX_P1_BCK_CHECK01_e;
        break; /* CHECK01 */
    case 15:
        idx = dRes_INDEX_P1_BCK_CHECK02_e;
        break; /* CHECK02 */
    case 16:
        idx = dRes_INDEX_P1_BCK_LOOK_e;
        break; /* LOOK */
    case 4:
        idx = dRes_INDEX_P1_BCK_C_WAIT_e;
        break; /* C_WAIT */
    case 5:
        idx = dRes_INDEX_P1_BCK_C_TALK01_e;
        break; /* C_TALK01 */
    case 6:
        idx = dRes_INDEX_P1_BCK_C_TALK02_e;
        attr = J3DFrameCtrl::EMode_NONE;
        break; /* C_TALK02 */
    case 7:
        idx = dRes_INDEX_P1_BCK_C_STOP_e;
        break; /* C_STOP */
    case 8:
        idx = dRes_INDEX_P1_BCK_C_TALK01_e;
        break; /* C_TALK01 */
    default:
        return TRUE;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)p1_getRes("P1", idx);
    mpMorf->setAnm(anm, attr, i_morf, 1.0f, 0.0f, -1.0f, nullptr);
    return TRUE;
}

/* 022B5744 */
BOOL daNpc_P1_c::explainAction(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mActionStatus == ACTION_STARTING_e) {
        mActionStatus = 1;
    } else if (mActionStatus == ACTION_ENDING_e && !eventInfo.checkCommandTalk()) {
        fopAcM_orderSpeakEvent(this);
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        return FALSE;
    }
    return TRUE;
}

/* 022B205C */
u32 daNpc_P1_c::getNextMsgNo(int i_param_1) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    msg_class* mgr = l_messageManager;
    u32 o_retval;
    u32 curr = mCurrMesg;
    switch (curr) {
    case 0xC94:
        o_retval = 0xC99;
        break;
    case 0xFA1:
    case 0xFA2:
    case 0xFA5:
    case 0xFA6:
    case 0xFA7:
    case 0xFAA:
    case 0xFAB:
    case 0x1015:
    case 0x1007:
        o_retval = curr + 1;
        break;
    case 0x1009:
        o_retval = 0x102E;
        break;
    case 0x1016:
        o_retval = 0x1033;
        break;
    case 0xFAE:
        o_retval = 0xFA5;
        break;
    case 0xFA8:
    case 0xFAC:
        o_retval = 0xFA2;
        break;
    case 0xFA3: {
        s32 sel = (s32)p1_cpp::message(mgr)->mSelectNum; /* l_msg->mSelectNum */
        if (sel == 0) {
            o_retval = 0xFA4;
        } else if (sel == 1) {
            o_retval = 0xFB0;
        } else {
            o_retval = 0xFFFFFFFF;
        }
        break;
    }
    default:
        o_retval = 0xFFFFFFFF;
        break;
    }
    if (i_param_1 == 1) {
        mCurrMesg = o_retval;
        mPrevMesg = curr;
    }
    return o_retval;
}

/* 022B16A4 */
BOOL daNpc_P1_c::playTexPatternAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!p1BlinkTimer(&mBlinkTimer)) { /* cLib_calcTimer */
        u8 f = (u8)(mBlinkFrame + 1);
        mBlinkFrame = f;
        u32 tex = gabi::ea(mpTexture.get());
        s32 max = p1_cpp::frameMax(tex);
        if ((s32)f >= max) {
            tex = gabi::ea(mpTexture.get());
            max = p1_cpp::frameMax(tex);
            mBlinkFrame = (u8)(mBlinkFrame - max);
            s16 rng = (s16)gabi::ftoi(cM_rndF(100.0f));
            mBlinkTimer = (s16)(rng + 0x1E);
        }
    }
    return TRUE;
}

/* 022B1128 */
void daNpc_P1_c::demo_end_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mParam == 0) {
        mAnmNum = -1;
        /* setAction(&daNpc_P1_c::confuseAction) expanded without the comparison */
        ProcFunc_l* cur = &mActionFunc;
        if (cur->i != 0) {
            mActionStatus = ACTION_ENDING_e;
            p1_pmf_call(this, cur, nullptr);
        }
        s16 oi = cur->i;
        u32 of = cur->f;
        s16 od = cur->d;
        mPrevAction.f = of;
        cur->f = P1_confuseAction;
        cur->d = 0;
        cur->i = -1;
        mPrevAction.d = od;
        mActionStatus = ACTION_STARTING_e;
        mPrevAction.i = oi;
        gabi::call_ptr<BOOL>(P1_confuseAction, gabi::ea(this) + (s32)(s16)cur->d, (u32)0);
        /* fopAcM_SearchByName(fpcNm_Obj_Pirateship_e) */
        gabi::Local<be<s16>> name;
        *name = fpcNm_Obj_Pirateship_e;
        fopAc_ac_c* actor = fopAcIt_Judge(0x025E121C /* fpcSch_JudgeByName */, name.get());
        if (actor) {
            u32 px = (u32)p1_cpp::words(&actor->current.pos)->x;
            f32 x = actor->current.pos.x;
            p1_cpp::words(&current.pos)->x = px;
            f32 y = actor->current.pos.y;
            current.pos.y = y;
            u32 pz = (u32)p1_cpp::words(&actor->current.pos)->z;
            f32 z = actor->current.pos.z;
            p1_cpp::words(&current.pos)->z = pz;
            current.pos.y = y + 700.0f;
            x = gabi::fmadds(950.0f, cM_ssin((u16)actor->current.angle.y), x);
            current.pos.x = x;
            z = gabi::fmadds(950.0f, cM_scos((u16)actor->current.angle.y), z);
            current.pos.z = z;
            x = gabi::fmadds(150.0f, cM_scos((u16)actor->current.angle.y), x);
            current.pos.x = x;
            z = gabi::fnmsubs(150.0f, cM_ssin((u16)actor->current.angle.y), z);
            current.pos.z = z;
            s16 sVar2 = (s16)(actor->current.angle.y - 0x4000);
            actor->current.angle.y = sVar2;
            current.angle.y = sVar2;
        }
    }
    m_jnt.mbBackBoneLock = 0; /* offBackBoneLock */
    m670 = 0;
    m_jnt.mbHeadLock = 0; /* offHeadLock */
}

/* 022B12FC */
BOOL daNpc_P1_c::demo_move() {
    // HD: stage comparisons use two SafeStrings, with the original Local slots and call order.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* dComIfGp_demo_getActor(demoActorID) */
    u8 id = demoActorID;
    u32 actor = 0;
    if (id != 0 && id <= 0x20) {
        u32 obj = l_demoObject;
        if (obj == 0) {
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = l_demoObject;
        }
        actor = p1DemoActor(obj, (u32)id);
    }
    if (!actor) {
        if (m670 == 1) {
            demo_end_init();
        }
        return FALSE;
    }
    m_jnt.mbBackBoneLock = 1; /* onBackBoneLock */
    m_jnt.mbHeadLock = 1;     /* onHeadLock */
    m670 = 1;
    J3DAnmTexPattern* btp = p1DemoBtp(actor, "P1");
    if (btp) {
        p1c_btpInit(&mBtp, J3DModel_getModelData(mpHeadModel), btp, 1, 2, 1.0f, 0, -1, 1, 0);
        mBlinkFrame = 0;
    }
    u32 btpAnm = gabi::ea(mBtp.mpAnm.get());
    if (btpAnm) {
        u8 uVar1 = (u8)p1_cpp::frameMax(btpAnm); /* getFrameMax() */
        u8 f = (u8)(mBlinkFrame + 1);
        if (f < uVar1) {
            mBlinkFrame = f;
        } else {
            mBlinkFrame = uVar1;
        }
    }
    /* HD: in the stage "Demo17", between demo frames 0x654 and 0x6B0, the demo actor's flag 0x40 is
     * cleared and the animation plays at double speed */
    if (p1_isStage_Demo17()) {
        u32 frame = l_demoFrame;
        if (frame - 0x654 < 0x5D) {
            auto* demoActor = gabi::at<p1_cpp::DemoActorView>(actor);
            demoActor->mFlags = (u16)(demoActor->mFlags & ~0x40);
            mpMorf->setPlaySpeed(2.0f);
        }
    }
    gabi::Local<cXyz> pos;
    p1_copy12(gabi::ea(pos.get()), gabi::ea(&current.pos));
    u32 snd = p1GroundSound(pos.get(), 10.0f); /* dBgS_GetGndMtrlSndId_Func(current.pos, 10.0f) */
    p1DemoData(this, 0x6A, mpMorf.get(), "P1", 0, 0, snd, 0); /* dDemo_setDemoData */
    /* HD: Senza is moved 20 to the left between demo frames 0x38C and 0x406 of "Demo17" */
    if (mType == TYPE_P1B_e) {
        if (p1_isStage_Demo17()) {
            u32 frame = l_demoFrame;
            if (frame - 0x38C < 0x7B) {
                current.pos.x = current.pos.x - 20.0f;
            }
        }
    }
    return TRUE;
}

/* 022B27C4 */
BOOL daNpc_P1_c::event_move() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mEventCut6B0.cutProc()) {
        u8 attn = (u8)mEventCut6B0.mbAttention; /* getAttnFlag() */
        mbAttentionFlag = attn;
        if (attn == 0) {
            mEventCut6B0.mbAttention = attn; /* setAttnFlag(mbAttentionFlag) */
        }
        return TRUE;
    } else {
        return privateCut();
    }
}

/* 022B1B78 */
BOOL daNpc_P1_c::evn_setAnm_init(int i_staff_id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<s32>* idx_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staff_id, "idx", 3);
    if (idx_p != nullptr) {
        setAnm(*idx_p, -1.0f);
    } else if (mType == TYPE_P1C_e) {
        setAnm(4, -1.0f);
    } else {
        setAnm(0, -1.0f);
    }
    return TRUE;
}

/* 022B1C14 */
BOOL daNpc_P1_c::evn_talk_init(int i_staff_id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* mesg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staff_id, "MsgNo", 3);
    l_msgId = 0xFFFFFFFF;
    /* HD: no l_msg */
    if (mesg_no_p != nullptr) {
        mCurrMesg = *mesg_no_p;
    } else {
        mCurrMesg = 0;
    }
    return TRUE;
}

/* 022B21B0 */
BOOL daNpc_P1_c::evn_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    msg_class* mgr = l_messageManager;
    if (l_msgId == 0xFFFFFFFF) {
        l_msgId = p1_msgSet(mgr, mCurrMesg, &eyePos);
    } else if (p1_msgGetStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) { /* HD: no l_msg search */
        if (getNextMsgNo(1) != 0xFFFFFFFF) {
            p1_msgSetStatus(mgr, fopMsgStts_MSG_CONTINUES_e);
            p1_msgSet(mgr, mCurrMesg, nullptr);
        } else {
            p1_msgSetStatus(mgr, fopMsgStts_MSG_ENDS_e);
        }
    } else if (p1_msgGetStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
        p1_msgSetStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
        l_msgId = 0xFFFFFFFF;
        return TRUE;
    }
    return FALSE;
}

/* 022B1C84 */
BOOL daNpc_P1_c::minigameExplainCut() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    msg_class* mgr = l_messageManager; /* HD: the message manager, read once */
    /* ActionNames[] = {"4013_msg", "4014_msg"} (0x101C2BE8) */
    s32 staffId = dComIfGp_evmng_getMyStaffId(mEventCut6B0.getActorName(), nullptr, 0);
    s32 actIdx = p1_getMyActIdx(staffId, gabi::ea(ActionNames), 2, 1, 0);
    if (staffId == -1) {
        mbAttentionFlag = 0;
        return FALSE;
    }
    if (m65A == 0) {
        l_msgId = 0xFFFFFFFF;
        p1_onCameraAttentionStatus4();
        mPrevMesg = mCurrMesg;
        mCurrMesg = 0xFAD;
        m65A = (s8)(m65A + 1);
        return TRUE;
    }
    if (m65A == -1) {
        return TRUE;
    }
    if (l_msgId == 0xFFFFFFFF) {
        l_msgId = p1_msgSet(mgr, mCurrMesg, &eyePos);
        return TRUE;
    }
    if (m65A == 1) {
        /* HD: no fopMsgM_SearchByID */
        m65A = (s8)(m65A + 1);
    } else if (p1_msgGetStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e /* fopMsgStts_MSG_DISPLAYED_e */) {
        if (mCurrMesg == 0xFAD) {
            p1_msgSetStatus(mgr, fopMsgStts_MSG_CONTINUES_e /* fopMsgStts_MSG_CONTINUES_e */);
            mPrevMesg = mCurrMesg;
            mCurrMesg = 0xFAE;
            p1_msgSet(mgr, 0xFAE, nullptr);
        } else {
            mPrevMesg = mCurrMesg;
            mCurrMesg = 0xFFFFFFFF;
            p1_msgSetStatus(mgr, fopMsgStts_MSG_ENDS_e /* fopMsgStts_MSG_ENDS_e */);
        }
    } else if (p1_msgGetStatus(mgr) == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
        if (actIdx == 1) {
            dComIfGp_evmng_cutEnd(staffId);
        }
        p1_msgSetStatus(mgr, fopMsgStts_MSG_DESTROYED_e /* fopMsgStts_MSG_DESTROYED_e */);
        p1_offCameraAttentionStatus4();
    } else if (dComIfGp_checkMesgSendButton() && actIdx == 0) {
        dComIfGp_evmng_cutEnd(staffId);
    }
    if (p1_checkAction(this, P1_explainAction)) {
        if (p1EndCheck(dComIfGp_getPEvtManager(), "sea_exp_cam")) {
            p1_setAction(this, P1_speakAction);
            dComIfGp_event_reset();
            u32 cam = p1_camera0(); /* SkipSmoother */
            p1_cpp::camera(cam)->mSkipPosition = 1;
            p1_cpp::camera(cam)->mSkipFov = 1;
            p1_cpp::camera(cam)->mSkipTarget = 1;
            fopAcM_orderSpeakEvent(this);
            eventInfo.onCondition(dEvtCnd_CANTALK_e);
        }
    }
    return TRUE;
}

/* 022B2638 */
BOOL daNpc_P1_c::privateCut() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* cut_name_tbl[] = {"4013_msg", "4014_msg", "SETANM", "HEADSWING", "TALKMSG"} (0x101C2BF0) */
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(mEventCut6B0.getActorName(), nullptr, 0);
    if (staffIdx == -1) {
        return FALSE;
    }
    s32 actIdx = p1_getMyActIdx(staffIdx, gabi::ea(cut_name_tbl), 5, 1, 0);
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
    } else {
        int iVar4 = 0;
        if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
            switch (actIdx) {
            case 2:
                evn_setAnm_init(staffIdx);
                break;
            case 3:
                mHeadAnm.swing_vertical_init(2, 0x1000, 0x800, 1);
                break;
            case 4:
                evn_talk_init(staffIdx);
                break;
            }
        }
        switch ((u32)actIdx) {
        case 0:
        case 1:
            minigameExplainCut();
            return TRUE;
        case 4:
            iVar4 = evn_talk();
            setAnimFromMsg();
            break;
        default:
            iVar4 = 1;
            break;
        }
        if (iVar4 != 0) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
    return TRUE;
}

/* 022B0674 */
static BOOL nodeCallBack1(J3DNode* i_node, int i_param_2) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_param_2 == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        daNpc_P1_c* i_this = static_cast<daNpc_P1_c*>(model->getUserArea());
        u32 jntNo = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                gabi::Local<cXyz> headOffset;
                headOffset->x = REG_F(10, 0);
                headOffset->y = REG_F(10, 1);
                headOffset->z = REG_F(10, 2);
                cMtx_YrotM(calc_mtx(), (s16) - (i_this->m_jnt.mAngles[0][1] + i_this->mHeadAnm.field_0x00.y));
                cMtx_ZrotM(calc_mtx(), (s16) - (i_this->m_jnt.mAngles[0][0] + i_this->mHeadAnm.field_0x00.x));
                MtxPosition(headOffset, &i_this->eyePos);
            }
            if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]);
            }
            Mtx34* anm = model->getAnmMtx(jntNo);
            J3DModel::mtx_assign(anm, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}

/* 022B0ADC */
static BOOL daNpc_P1_Draw(daNpc_P1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_draw();
}

/* 022B1124 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_P1_c*)i_this)->CreateHeap();
}

/* 022B3274 */
BOOL daNpc_P1_c::setAttentionPos(cXyz* i_param_1) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto* dst = p1_cpp::words(i_param_1);
    auto* src = p1_cpp::words(&eyePos);
    dst->x = (u32)src->x;
    f32 y = eyePos.y;
    dst->y = (u32)src->y;
    dst->z = (u32)src->z;
    i_param_1->y = y + p1_child(mType)->mAttnYPosOffset;
    return TRUE;
}

/* 022B35A4 */
BOOL daNpc_P1_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_resDelete(&mPhs, "P1");
    if (heap && mpMorf) {
        mpMorf->stopZelAnime();
    }
    daNpc_P1_HIO_c* hio = (&l_HIO);
    s32 n = hio->m8;
    if (n >= 0) {
        n = n - 1;
        hio->m8 = n;
        if (n < 0) {
            mDoHIO_deleteChild(hio->mNo);
        }
    }
    return TRUE;
}

/* 022B362C */
u32 daNpc_P1_c::getKajiID() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 kaji_id = 0xFFFFFFFF;
    u32 parent_id = parentActorID;
    if (parent_id != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = parent_id;
        fopAc_ac_c* actor = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
        if (fopAc_IsActor(actor) && actor != nullptr && fpcM_GetName(actor) == fpcNm_Obj_Pirateship_e) {
            kaji_id = (u32) reinterpret_cast<p1_cpp::PirateShipView*>(actor)
                          ->mKajiId; /* daObjPirateship::Act_c::getKajiID() */
        }
    }
    return kaji_id;
}

/* 022B32B0 */
BOOL daNpc_P1_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_get(); /* HD: an unused call */
    mHeadAnm.move();
    if (!demo_move()) {
        playTexPatternAnm();
        u32 mtrlSndId;
        if (mObjAcch.m_flags & 0x20 /* ChkGroundHit */) {
            mtrlSndId = p1MaterialSound(dComIfG_Bgsp(),
                                        static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd) /* mObjAcch.m_gnd */);
        } else {
            mtrlSndId = 0;
        }
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mpMorf->play(&current.pos, mtrlSndId, (s8)reverb);
        if (p1_eventMode() == 0 || eventInfo.checkCommandTalk()) {
            p1_pmf_call(this, &mActionFunc, nullptr);
            m65A = 0;
            mbAttentionFlag = 0;
        } else {
            event_move();
        }
    }
    kaji_anm();
    lookBack();
    setAttentionPos(&attention_info.position); /* &attention_info.position */
    if (m670 == 0) {
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = p1RoomId(dComIfG_Bgsp(),
                              static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)); /* tevStr.mRoomNo = GetRoomId */
    u8 color = p1PolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)); /* GetPolyColor */
    J3DModel* model = mpMorf->getModel();
    tevStr.mEnvrIdxOverride = color; /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    return TRUE;
}

/* 022B0810 */
BOOL daNpc_P1_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());

    if (mType == TYPE_P1A_e && !dComIfGs_isEventBit(0x310)) {
        return TRUE;
    }
    J3DModel* model = mpMorf->getModel();
    J3DModelData* head_model_data = J3DModel_getModelData(mpHeadModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpHeadModel, &tevStr);
    p1BtpEntry(&mBtp, head_model_data, (u32)(u8)mBlinkFrame);
    /* HD: no material table swap for P1B/P1C (separate body models, see CreateHeap) and no blob shadow */
    u8 type = mType;
    mpMorf->updateDL();
    if (type == TYPE_P1A_e) {
        J3DModel* head = mpHeadModel;
        Mtx34* anm = model->getAnmMtx(m_jnt.mHeadJntNum);
        head->setBaseTRMtx(anm); /* mpHeadModel->setBaseTRMtx(getAnmMtx(head)) */
        mDoExt_modelUpdateDL(mpHeadModel, 0);
    } else {
        J3DModel* head = mpHeadModel;
        Mtx34* anm = model->getAnmMtx(m_jnt.mHeadJntNum);
        head->setBaseTRMtx(anm);
        mDoExt_modelUpdateDL(mpHeadModel, 0);
        if (mpDoraModel) {
            setLightTevColorType(dKy_getEnvlight(), mpDoraModel, &tevStr);
            s8 hand = m_handR_jnt_num;
            Mtx34* anm2 = model->getAnmMtx(hand);
            mpDoraModel->setBaseTRMtx(anm2);
            mDoExt_modelUpdateDL(mpDoraModel, 0);
        }
    }
    u8 snap = p1_cpp::entry(l_snap_idx_tbl, mType);
    p1RegisterFigure((u32)snap, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f,
                     1.0f); /* dSnap_RegistFig */
    return TRUE;
}

/* 022B3578 */
static BOOL daNpc_P1_Execute(daNpc_P1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    i_this->_execute();
    return TRUE;
}

/* 022B359C */
static BOOL daNpc_P1_IsDelete(daNpc_P1_c*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

/* 022B3628 */
static BOOL daNpc_P1_Delete(daNpc_P1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_delete();
}

/* 022B43E8 */
static cPhs_State daNpc_P1_Create(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_P1_c*)i_this)->_create();
}

// HD: compiler-generated and template code of this translation unit.

/* 022B5998 */
static void __sinit_d_a_npc_p1_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics(0x10467F24, 0x101C2C04);
    daNpc_P1_HIO_ct((&l_HIO)); /* l_HIO */
}

/* 022B5A38 */
static void SafeString_dtor(SafeString* self, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self != nullptr && (flags & 1))
        operator_delete(self);
}

/* 022B5A4C */
static void daNpc_P1_dtor(daNpc_P1_c* self, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self == nullptr)
        return;
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    p1CircleDtor(p1_cpp::circle(&self->mAcchCir), 2);         /* cM3dGCir::~cM3dGCir */
    p1_cpp::acch(&self->mObjAcch)->mActorVtable = 0x1001F8B4; /* dBgS_ObjAcch vtables (this TU) */
    p1_cpp::acch(&self->mObjAcch)->mCheckVtable = 0x1001F8C4;
    p1AcchDtor(&self->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
    p1ActorDtor(self, 0);           /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(self);
}

/* 022B5AE8 */
static void SafeString_assureTerminationImpl(SafeString* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// clang-format off
/* WWHD glue
data l_HIO 10467F40
data l_msgId 10467F20
data l_messageManager 101F4B5C
data l_snap_idx_tbl 101C2BE4
data ActionNames 101C2BE8
data cut_name_tbl 101C2BF0
data l_cyl_src 101C2BA0
data l_demoObject 101D5FFC
data l_demoFrame 101D600C
ext 025F795C u32 p1_msgGetStatus(msg_class*)
ext 025F74D0 void p1_msgSetStatus(msg_class*, u32)
ext 025F7DB0 u32 p1_msgSet(msg_class*, u32, cXyz*)
ext 02542EDC s32 p1ActionIndex(dEvent_manager_c*, s32, u32, s32, s32, s32)
ext 025E7B3C void p1BtpEntry(mDoExt_btpAnm*, J3DModelData*, u32)
ext 025BEBB8 void p1RegisterFigure(u32, daNpc_P1_c*, cXyz*, s32, f32, f32, f32)
ext 02526E70 u32 p1DemoActor(u32, u32)
ext 02527828 J3DAnmTexPattern* p1DemoBtp(u32, const char*)
ext 025E789C s32 p1c_btpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 024F1914 u32 p1GroundSound(cXyz*, f32)
ext 02527028 BOOL p1DemoData(daNpc_P1_c*, s32, mDoExt_McaMorf*, const char*, s32, s32, u32, s32)
ext 02055B64 s16 p1BlinkTimer(be<s16>*)
ext 0254457C BOOL p1EndCheck(dEvent_manager_c*, const char*)
ext 024EECAC u32 p1MaterialSound(dBgS*, void*)
ext 024EF130 s8 p1RoomId(dBgS*, void*)
ext 024EEEB8 u8 p1PolyColor(dBgS*, void*)
ext 028EFFD0 void p1ConstructArray(daNpc_P1_childHIO_c*, s32, s32, u32)
ext 02018034 void p1CircleDtor(void*, s32)
ext 024EFD9C void p1AcchDtor(dBgS_ObjAcch*, s32)
ext 025D50BC void p1ActorDtor(daNpc_P1_c*, s32)
ext 027F68FC u32 p1JointNameHeader(J3DModelData*)
ext 027DF9B0 s8 p1c_getIndex(u32, const char*)
ext 027F3F94 u32 p1JointRegistry(J3DModelData*)
ext 0259F7D4 void p1c_setActorInfo(dNpc_EventCut_c*, const char*, fopAc_ac_c*)
ext 0259DAA0 void p1c_JntCtrl_ct(void*)
ext 0259F740 void p1c_EventCut_ct(void*)
ext 025E7820 void p1c_btpAnm_ct(void*)
ext 0259DED0 void p1LookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s32, s32, u32)
ext 0200F164 void p1_cLib_addCalcPos2(cXyz*, cXyz*, f32, f32)
ext 025D77DC BOOL p1OrderOtherEvent(fopAc_ac_c*, u32, u32, u32)
ext 0252012C void p1NextStage(u32, s32, s32, s32, f32, u32, s32, u32)

str 1001F914 "d_demo.h" in 022B12FC
str 1001F904 "m_object != (0)" in 022B12FC
str 1001F9CC "P1" in 022B12FC
str 1001F9E4 "idx" in 022B1B78
str 1001F9E8 "MsgNo" in 022B1C14
str 1001F9F0 "sea_exp_cam" in 022B1C84
str 1001FA70 "P1" in 022B35A4
str 1001F9E0 "P1" in 022B1758
*/

// clang-format on
