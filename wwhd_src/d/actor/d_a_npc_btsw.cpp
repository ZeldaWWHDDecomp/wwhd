/**
 * d_a_npc_btsw.cpp (WWHD)
 * NPC - Baito (Dragon Roost postman, mail-sorting minigame)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_btsw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_btsw.h"
#include "gc_api/d/d_com_inf_game.h"

#include <cmath>

// Resource IDs follow the named BtswResourceId entries; numeric initializers are checked by glue.
static const s32 l_btp_ix_tbl[] = {14};
static const u32 l_bck_ix_tbl[] = {22, 23, 20, 21, 16, 19, 19, 24, 15, 17, 18};
static const u32 a_play_mode_tbl[] = {2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 0};
static const f32 a_morf_frame_tbl[] = {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
                                       8.0f, 8.0f, 8.0f, 8.0f, 8.0f};
static const f32 a_play_speed_tbl[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                       1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

static auto& aim_pos_data = gabi::object_at<BtswPositionTable>(0x104669B8);
static auto& cursor_pos_data = gabi::object_at<BtswPositionTable>(0x10466A00);
static auto& camera_center_data = gabi::object_at<BtswPositionTable>(0x10466A58);
static auto& camera_eye = gabi::object_at<cXyz>(0x10466AA0);

static auto& l_HIO = gabi::object_at<daNpc_Btsw_HIO_c>(BTSW_HIO);

// Named HD globals; guards remain explicit to preserve their store order.
static auto& l_zero = gabi::object_at<be<f32>>(0x10018744);
static auto& l_oneThird = gabi::object_at<be<f32>>(0x100187A4);
static auto& l_twoThirds = gabi::object_at<be<f32>>(0x100187A8);
static auto& l_piInitialHigh = gabi::object_at<be<f32>>(0x10018998);
static auto& l_piInitialLow = gabi::object_at<be<f32>>(0x1001899C);
static auto& m_no_buff = gabi::object_at<be<u8>>(0x101BD78C);
static auto& m_same_count = gabi::object_at<be<u8>>(0x101BD78D);
static auto& l_messageManager = gabi::object_at<be<u32>>(0x101F4B5C);
static auto& l_resourceControl = gabi::object_at<be<u32>>(0x101F4F28);
static auto& l_pad = gabi::object_at<be<u32>>(0x101F5088);
static auto& l_miniGameSystem = gabi::object_at<be<u32>>(0x101F8344);
static auto& l_debugColorOneGuard = gabi::object_at<be<u32>>(0x101FDA50);
static auto& l_debugColorTwoGuard = gabi::object_at<be<u32>>(0x101FDAC0);
static auto& l_cXyzZeroX = gabi::object_at<be<f32>>(0x101FFBA8);
static auto& l_cXyzZeroXWord = gabi::object_at<be<u32>>(0x101FFBA8);
static auto& l_cXyzZeroY = gabi::object_at<be<f32>>(0x101FFBAC);
static auto& l_cXyzZeroYWord = gabi::object_at<be<u32>>(0x101FFBAC);
static auto& l_cXyzZeroZ = gabi::object_at<be<f32>>(0x101FFBB0);
static auto& l_piHigh = gabi::object_at<be<f32>>(0x10466914);
static auto& l_piLow = gabi::object_at<be<f32>>(0x10466918);
static auto& l_largeFloatOne = gabi::object_at<be<f32>>(0x1046691C);
static auto& l_largeFloatTwo = gabi::object_at<be<f32>>(0x10466920);
static auto& l_largeFloatThree = gabi::object_at<be<f32>>(0x10466924);
static auto& l_largeFloatFour = gabi::object_at<be<f32>>(0x10466928);
static auto& l_zeroVectorWord0 = gabi::object_at<be<u32>>(0x10466930);
static auto& l_zeroVectorWord1 = gabi::object_at<be<u32>>(0x10466934);
static auto& l_zeroVectorWord2 = gabi::object_at<be<u32>>(0x10466938);
static auto& l_zeroVectorWord3 = gabi::object_at<be<u32>>(0x1046693C);
static auto& l_attentionOffsetX = gabi::object_at<be<f32>>(0x10466940);
static auto& l_attentionOffsetY = gabi::object_at<be<f32>>(0x10466944);
static auto& l_attentionOffsetZ = gabi::object_at<be<f32>>(0x10466948);
static auto& l_eyeOffsetX = gabi::object_at<be<f32>>(0x1046694C);
static auto& l_eyeOffsetY = gabi::object_at<be<f32>>(0x10466950);
static auto& l_eyeOffsetZ = gabi::object_at<be<f32>>(0x10466954);
static auto& l_attentionOffsetGuard = gabi::object_at<be<u32>>(0x10466A48);
static auto& l_eyeOffsetGuard = gabi::object_at<be<u32>>(0x10466A4C);
static auto& l_aimPositionGuard = gabi::object_at<be<u32>>(0x10466A50);
static auto& l_cursorPositionGuard = gabi::object_at<be<u32>>(0x10466A54);
static auto& l_cameraEyeX = gabi::object_at<be<f32>>(0x10466AA0);
static auto& l_cameraEyeY = gabi::object_at<be<f32>>(0x10466AA4);
static auto& l_cameraEyeZ = gabi::object_at<be<f32>>(0x10466AA8);
static auto& l_miniGameActive = gabi::object_at<be<u8>>(0x1047B09A);
static auto& l_currentModel = gabi::object_at<be<u32>>(0x104B462C);

using gabi::load;
using gabi::store;

static inline u32 A(const void* p) { return gabi::ea(p); }
static inline void btsw_seStart(u32 id) { Btsw_seStart(id); } /* mDoAud_seStart (HD: no position) */
static inline void btsw_assert(u32 file, s32 line, u32 msg) {
    Btsw_JUTAssertion_showAssert(file, line, msg);
}

/* dComIfG_getObjectIDRes(arc, id): HD dRes_control_c::getIDRes(SafeString, id) */
static u32 btsw_getIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_addr(arc);
    key->__vtbl = BTSW_SAFESTRING_VTBL;
    return Btsw_dRes_control_c_getIDRes(l_resourceControl.get(), key.get(), id);
}
/* modelData->getJointName()->getIndex(name) */
static s32 btsw_jointIndex(u32 modelData, u32 name) {
    u32 names = Btsw_jointNameHeader(modelData);
    u32 disp = gabi::object_at<BtswNameHeader>(names).mNameOffset.get();
    return Btsw_jointNameIndex(disp ? names + 0x10 + disp : 0u, name);
}
/* J3DModel::getAnmMtx(jnt) (HD: the joint matrix block at +0x2C, marked dirty) */
static u32 btsw_getAnmMtx(u32 model, s32 jnt) {
    u32 blk = gabi::object_at<J3DModel>(model).mpMtxBuffer.v.get();
    u16 flags = gabi::object_at<J3DMtxBuffer_hd>(blk).mFlags.get();
    u32 arr = gabi::object_at<J3DMtxBuffer_hd>(blk).mpAnmMtx.get();
    gabi::object_at<J3DMtxBuffer_hd>(blk).mFlags = (u16)(flags | 0x10);
    return arr + (u32)(jnt * 0x30);
}
/* (this->*mCurrActionFunc)(arg) */
static void btsw_actionCall(daNpc_Btsw_c* t, u32 arg) {
    u32 a = A(t);
    s16 d = t->mCurrActionFunc.d;
    s16 i = t->mCurrActionFunc.i;
    u32 recv = a + (u32)(s32)d;
    u32 target;
    if (i < 0) {
        target = t->mCurrActionFunc.f;
    } else {
        s16 vofs = Btsw_virtualOffset(t->mCurrActionFunc);
        target = Btsw_virtualTarget(recv, vofs, i);
    }
    gabi::call_ptr<BOOL>(target, recv, arg);
}
/* setAction(&daNpc_Btsw_c::fn, NULL) */
static void btsw_setAction(daNpc_Btsw_c* t, u32 fn) {
    s16 i = t->mCurrActionFunc.i;
    if (i == -1 && t->mCurrActionFunc.d == 0 && t->mCurrActionFunc.f == fn)
        return;
    if (i != 0) {
        t->mActionStatus = daNpc_Btsw_c::ACTION_ENDING;
        btsw_actionCall(t, 0);
    }
    t->mCurrActionFunc.d = 0;
    t->mCurrActionFunc.i = -1;
    t->mCurrActionFunc.f = fn;
    t->mActionStatus = daNpc_Btsw_c::ACTION_STARTING;
    s16 d = t->mCurrActionFunc.d;
    gabi::call_ptr<BOOL>(t->mCurrActionFunc.f.get(), A(t) + (u32)(s32)d, 0u);
}
/* SwMail2_c::SetProc(&SwMail2_c::fn) */
static void btsw_mailSetProc(u32 m, u32 fn) {
    auto& action = gabi::object_at<SwMail2_c>(m).mFunc;
    action.d = 0;
    action.i = -1;
    action.f = fn;
}
/* SwMail2_c::CheckProc(&SwMail2_c::fn) */
static bool btsw_mailCheckProc(u32 m, u32 fn) {
    auto& action = gabi::object_at<SwMail2_c>(m).mFunc;
    return action.i == -1 && action.d == 0 && action.f == fn;
}
/* calls through the mail pointers (field_0x920[i]->fn()) */
static inline void btsw_mailCall(u32 fn, u32 mail) { gabi::call(fn, mail); }
static inline void btsw_mailCall2(u32 fn, u32 mail, u32 arg) { gabi::call(fn, mail, arg); }
#define MAIL_init 0x02217E74u
#define MAIL_DummyInit 0x02218114u
#define MAIL_AppearInit 0x02219B24u
#define MAIL_ThrowInit 0x02219EA0u
#define MAIL_move 0x02219ADCu
#define MAIL_draw 0x022197C8u
#define MAIL_MailCreateInit 0x022181ECu
/* dComIfG_getTimerRestTimeMs(): HD frames left * 1000 / 30 */
/* dComIfGp_event_reset() */

static void btsw_endMiniGame() {
    u32 p = dComIfGp_ea();
    u16 flags = gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags.get();
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMiniGameType = 0;
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameResult = 0;
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags = (u16)(flags ^ 0x40);
    u32 g = l_miniGameSystem.get();
    l_miniGameActive = 0;
    Btsw_endMiniGame(gabi::object_at<BtswMiniGameSystem>(g).mpGame.get());
}

static void btsw_moveMails(u32 a) {
    btsw_mailCall(MAIL_move, gabi::object_at<daNpc_Btsw_c>(a).field_0x920[0].v.get());
    btsw_mailCall(MAIL_move, gabi::object_at<daNpc_Btsw_c>(a).field_0x920[1].v.get());
    btsw_mailCall(MAIL_move, gabi::object_at<daNpc_Btsw_c>(a).field_0x920[2].v.get());
}

/* dComIfGp_endMiniGame(7) (HD inline) */

/* field_0x920[0..2]->move() */

// HD: Load the matrix-array pointer before marking the buffer dirty; retain the explicit vector guard store order.
/* 0221798C */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming != 0 /* J3DNodeCBCalcTiming_In */)
        return TRUE;
    u32 model = l_currentModel.get(); /* j3dSys.getModel() */
    u32 i_this = gabi::object_at<J3DModel>(model).mUserArea.get();
    if (i_this == 0)
        return TRUE;
    /* static cXyz a_att_pos_offst(0.0f, 0.0f, 0.0f); */
    f32 zero = l_zero.get();
    if (l_attentionOffsetGuard.get() == 0) {
        l_attentionOffsetX = zero;
        l_attentionOffsetZ = zero;
        l_attentionOffsetGuard = 1;
        l_attentionOffsetY = zero;
    }
    /* static cXyz a_eye_pos_offst(10.0f, 20.0f, 0.0f); */
    if (l_eyeOffsetGuard.get() == 0) {
        l_eyeOffsetGuard = 1;
        l_eyeOffsetX = 10.0f;
        l_eyeOffsetY = 20.0f;
        l_eyeOffsetZ = zero;
    }
    u32 jointNo = gabi::object_at<BtswJointNumber>(Btsw_jointNumber(node)).mNumber.get();
    const u32 stack = 0x1048D0CC;                               /* mDoMtx_stack_c::now */
    Btsw_PSMTXCopy(btsw_getAnmMtx(model, (s32)jointNo), stack); /* mDoMtx_stack_c::copy */
    if (jointNo == (u32)(s32)gabi::object_at<daNpc_Btsw_c>(i_this).m_jnt.mHeadJntNum.get()) {
        Btsw_PSMTXMultVec(stack, 0x10466940u,
                          i_this + 0xD00); /* multVec(&a_att_pos_offst, &mAttPos) */
        Mtx34 mtx;
        Btsw_PSMTXCopy(stack, &mtx);
        f32 x = mtx.m[0][3];
        f32 z = mtx.m[2][3];
        mtx.m[2][3] = zero;
        f32 y = mtx.m[1][3];
        mtx.m[1][3] = zero;
        mtx.m[0][3] = zero;
        Btsw_PSMTXTrans(stack, x, y, z); /* transS(vec) */
        Btsw_mDoMtx_YrotM(stack, (s32)gabi::object_at<daNpc_Btsw_c>(i_this)
                                     .m_jnt.mAngles[0][1]
                                     .get()); /* YrotM(getHead_y()) */
        Btsw_mDoMtx_XrotM(stack, (s32)gabi::object_at<daNpc_Btsw_c>(i_this)
                                     .m_jnt.mAngles[0][0]
                                     .get()); /* XrotM(getHead_x()) */
        Btsw_PSMTXConcat(stack, &mtx, stack); /* concat(mtx) */
        Btsw_PSMTXMultVec(stack, 0x1046694Cu,
                          i_this + 0x37C); /* multVec(&a_eye_pos_offst, &eyePos) */
    } else if (jointNo ==
               (u32)(s32)gabi::object_at<daNpc_Btsw_c>(i_this).m_jnt.mBackboneJntNum.get()) {
        Btsw_mDoMtx_XrotM(stack, (s32)gabi::object_at<daNpc_Btsw_c>(i_this)
                                     .m_jnt.mAngles[1][1]
                                     .get()); /* XrotM(getBackbone_y()) */
        Btsw_mDoMtx_ZrotM(stack, (s32)(s16) - (s32)gabi::object_at<daNpc_Btsw_c>(i_this)
                                                  .m_jnt.mAngles[1][0]
                                                  .get()); /* ZrotM(-getBackbone_x()) */
    }
    Btsw_PSMTXCopy(stack, 0x104B4868u); /* J3DSys::mCurrentMtx */
    mtx_copy(&gabi::object_at<Mtx34>(btsw_getAnmMtx(model, (s32)jointNo)),
             &gabi::object_at<Mtx34>(stack)); /* setAnmMtx */
    return TRUE;
}

// HD: The HD pattern setup always reads the first texture resource ID; the modify flag stays a full word.
/* 02217C94 */
BOOL daNpc_Btsw_c::initTexPatternAnm(u32 i_modify) {
    u32 a = A(this);
    u32 model = mpMorf->mpModel.v.get();
    s32 btp = l_btp_ix_tbl[0]; /* HD: l_btp_ix_tbl[0] (GameCube l_btp_ix_tbl[field_0x9C4]) */
    u32 modelData = gabi::object_at<J3DModel>(model).mModelData.v.get();
    u32 pattern = btsw_getIDRes("Btsw" /* "Btsw" */, btp);
    m_head_tex_pattern.v = pattern;
    if (pattern == 0) {
        btsw_assert(gabi::guest_addr("d_a_npc_btsw.cpp"), 0x154,
                    gabi::guest_addr("m_head_tex_pattern != (0)"));
        pattern = m_head_tex_pattern.v.get();
    }
    /* mDoExt_btpAnm::init(modelData, pattern, TRUE, EMode_LOOP, 1.0f, 0, -1, i_modify, FALSE) */
    if (!Btsw_btpInit(gabi::ea(&field_0x7E8), modelData, pattern, 1, 2, 0, -1, i_modify, 0, 1.0f))
        return FALSE;
    field_0x7FC = 0;
    field_0x954 = 0;
    return TRUE;
}

/* 0221914C */
void daNpc_Btsw_c::playTexPatternAnm() {
    u32 a = A(this);
    if (Btsw_cLib_calcTimer(gabi::ea(&field_0x954)) != 0)
        return; /* cLib_calcTimer(&field_0x954) */
    u32 pattern = m_head_tex_pattern.v.get();
    s32 frameMax = gabi::call_ptr<s32>(
        gabi::object_at<BtswPatternWork>(pattern).mpVtable->mGetFrameMax.get(), pattern);
    if ((s32)(u32)field_0x7FC < frameMax) {
        field_0x7FC = (u8)(field_0x7FC + 1);
        return;
    }
    pattern = m_head_tex_pattern.v.get();
    frameMax = gabi::call_ptr<s32>(
        gabi::object_at<BtswPatternWork>(pattern).mpVtable->mGetFrameMax.get(), pattern);
    field_0x7FC = (u8)(field_0x7FC - (u32)frameMax);
    f32 random = Btsw_cM_rndF(100.0f);
    field_0x954 = (s16)gabi::ftoi(gabi::fadds_ppc(random, 30.0f));
}

/* 0221A3CC */
void daNpc_Btsw_c::setAnm(s8 idx) {
    u32 a = A(this);
    s8 cur = field_0x9C5;
    f32 morf = a_morf_frame_tbl[idx]; /* a_morf_frame_tbl[idx] */
    if (cur == 6 || cur == 5)
        morf = l_HIO.field_0x5C.get(); /* l_HIO.field_0x5C */
    if ((s32)idx == (s32)cur)
        return;
    field_0x9C5 = idx;
    /* dNpc_setAnmIDRes(mpMorf, a_play_mode_tbl[idx], morf, a_play_speed_tbl[idx], l_bck_ix_tbl[idx], -1, "Btsw") */
    Btsw_dNpc_setAnmIDRes(mpMorf.v.get(), a_play_mode_tbl[idx], morf, a_play_speed_tbl[idx],
                          l_bck_ix_tbl[idx], -1, gabi::guest_addr("Btsw"));
    s8 anm = field_0x9C5;
    if (anm == 9 || anm == 10 || anm == 6 || anm == 5) {
        m_jnt.mbBackBoneLock = 1; /* onHeadLock(), onBackBoneLock() */
        m_jnt.mbHeadLock = 1;
    } else {
        m_jnt.mbHeadLock = 0;
        m_jnt.mbBackBoneLock = 0;
    }
}

// HD: Keep the fused distance calculation and strict attention bounds.
/* 0221A49C */
bool daNpc_Btsw_c::chkAttention(cXyz* pos, s16 angle) {
    u32 player = gabi::ea(dComIfGp_getPlayer(0)); /* dComIfGp_getPlayer(0) */
    f32 posZ = pos->z;
    f32 playerZ = gabi::object_at<fopAc_ac_c>(player).current.pos.z.get();
    f32 playerX = gabi::object_at<fopAc_ac_c>(player).current.pos.x.get();
    f32 dz = gabi::fsubs_ppc(playerZ, posZ);
    f32 posX = pos->x;
    f32 maxAttnDistXZ = l_HIO.mNpc.mMaxAttnDistXZ.get();
    f32 dx = gabi::fsubs_ppc(playerX, posX);
    f32 sq = gabi::fmadds(dx, dx, gabi::fmuls_ppc(dz, dz));
    s32 maxAttnAngleY = l_HIO.mNpc.mMaxAttnAngleY.get();
    f32 distXZ = Btsw_squareRoot(sq);          /* std::sqrtf */
    s16 targetAngleY = Btsw_cM_atan2s(dx, dz); /* cM_atan2s */
    if (mHasAttention) {
        maxAttnDistXZ = gabi::fadds_ppc(maxAttnDistXZ, 40.0f);
        maxAttnAngleY += 0x71C; /* cM_deg2s(10.0f) */
    }
    s32 delta = (s16)(targetAngleY - angle);
    s32 absDelta = delta < 0 ? -delta : delta;
    return maxAttnAngleY > absDelta && maxAttnDistXZ > distXZ;
}

/* 022195A4 */
void daNpc_Btsw_c::eventOrder() {
    s8 order = field_0x9C7;
    if (order == 3) {
        Btsw_fopAcM_orderOtherEvent2(this,
                                     gabi::guest_addr("GETMOTHERLETTER") /* "GETMOTHERLETTER" */, 1,
                                     0xFFFF); /* fopAcM_orderOtherEvent */
    } else if (order == 4) {
        Btsw_fopAcM_orderOtherEventId(this, (s32)field_0x9C8, 0xFF, 0xFFFF, 0,
                                      1); /* fopAcM_orderOtherEventId */
    } else if (order == 1 || order == 2) {
        s8 o = field_0x9C7;
        u32 a = A(this);
        eventInfo.mCondition =
            (u16)(eventInfo.mCondition.get() | 0x21); /* onCondition(CANTALK | CANTALKITEM) */
        if (o == 1)
            Btsw_fopAcM_orderSpeakEvent(this); /* fopAcM_orderSpeakEvent */
    }
}

/* 02219210 */
void daNpc_Btsw_c::checkOrder() {
    u16 command = eventInfo.mCommand.get();
    if (command == 2) { /* eventInfo.checkCommandDemoAccrpt() */
        s8 order = field_0x9C7;
        if (order == 3) {
            field_0x9C7 = 0;
            btsw_setAction(this, BTSW_getdemo_action);
        } else if (order == 4) {
            field_0x9C7 = 0;
            btsw_setAction(this, BTSW_dummy_event_action);
        }
        return;
    }
    if (command != 1)
        return; /* eventInfo.checkCommandTalk() */
    s8 order = field_0x9C7;
    if (order != 1 && order != 2)
        return;
    field_0x9C7 = 0;
    field_0x9A9 = 1;
}

/* 0221A5C8 */
u16 daNpc_Btsw_c::next_msgStatus(be<u32>* pMsgNo) {
    u32 msgNo = *pMsgNo;
    /* HD: mpCurrMsg->mSelectNum is read from a message manager global */
    u32 msgMgr = l_messageManager.get();
    switch (msgNo) {
    case 0x1A91:
    case 0x1A92:
    case 0x1A94:
    case 0x1A95:
    case 0x1A96:
    case 0x1A98:
    case 0x1A99:
    case 0x1A9A:
    case 0x1AA7:
    case 0x1AAD:
        *pMsgNo = msgNo + 1;
        break;
    case 0x1AB5:
        *pMsgNo = 0x1A93;
        break;
    case 0x1A93:
        if (gabi::object_at<BtswMessageState>(msgMgr).mSelection.get() == 0) {
            dComIfGs_onEventBit(0x2702);
            *pMsgNo = 0x1A94;
        } else {
            *pMsgNo = 0x1AA0;
        }
        break;
    case 0x1AA1:
        if (gabi::object_at<BtswMessageState>(msgMgr).mSelection.get() == 0) {
            dComIfGs_onEventBit(0x2702);
            *pMsgNo = 0x1A94;
        } else {
            *pMsgNo = 0x1AA2;
        }
        break;
    case 0x1AA8:
        *pMsgNo = gabi::object_at<BtswMessageState>(msgMgr).mSelection.get() == 0 ? 0x1AA9 : 0x1AAA;
        break;
    case 0x1AA4:
    case 0x1AA5:
    case 0x1AA6:
        if (!dComIfGs_isEventBit(0x2701)) {
            *pMsgNo = 0x1AA7;
        } else if (Btsw_dLetter_isNoSend(0xAC03)) { /* dLetter_isNoSend(LETTER_BAITOS_MOM) */
            *pMsgNo = 0x1AAB;
        } else if (!Btsw_dSv_player_collect_c_isSymbol(dComIfGs_ea() + 0xD4,
                                                       1)) { /* dComIfGs_isSymbol(dSymbol_DIN_e) */
            *pMsgNo = 0x1AB4;
        } else if (!Btsw_dLetter_isRead(0xAC03)) { /* dLetter_isRead(LETTER_BAITOS_MOM) */
            *pMsgNo = 0x1AAD;
        } else if (!dComIfGs_isEventBit(0x3104)) {
            *pMsgNo = 0x1AAC;
            dComIfGs_onEventBit(0x3104);
        } else {
            *pMsgNo = 0x1AB3;
        }
        break;
    case 0x1AA9:
        *pMsgNo = 0x1A97;
        break;
    default:
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
}

// HD: The cached save address includes the HD header; keep the final event-register write on that same save object.
/* 0221A840 */
u32 daNpc_Btsw_c::getMsg() {
    u32 previous = field_0x9B0;
    if (previous != fpcM_ERROR_PROCESS_ID_e) {
        field_0x9B0 = fpcM_ERROR_PROCESS_ID_e;
        return previous;
    }
    if ((u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3)
        return 0x1AA3; /* dComIfGp_event_chkTalkXY() */
    u32 msg;
    u32 save;
    if (!dComIfGs_isEventBit(0x2704)) {
        dComIfGs_onEventBit(0x2704);
        save = dComIfGs_ea();
        msg =
            gabi::object_at<dSv_info_c>(save + 0x20).mSavedata.mPlayer.mInfo.mClearCount.get() != 0
                ? 0x1AB5
                : 0x1A91; /* dComIfGs_getClearCount() */
    } else if (!dComIfGs_isEventBit(0x2702)) {
        save = dComIfGs_ea();
        msg = 0x1AA1;
    } else {
        u32 reg = Btsw_dSv_event_c_getEventReg(dComIfGs_ea() + 0x644,
                                               0xAB03); /* dComIfGs_getEventReg(UNK_AB03) */
        save = dComIfGs_ea();
        msg = reg < 3 ? 0x1AA5 : 0x1AA4;
    }
    Btsw_dSv_event_c_setEventReg(save + 0x644, 0xAB03, 0); /* dComIfGs_setEventReg(UNK_AB03, 0) */
    return msg;
}

/* 0221A960 */
void daNpc_Btsw_c::anmAtr(u16 i_msgStatus) {
    u8 attr = dComIfGp_getMesgAnimeAttrInfo(); /* dComIfGp_getMesgAnimeAttrInfo() */
    if (attr <= 6)
        setAnm((s8)attr);
    u32 morf = mpMorf.v.get();
    f32 frame = (f32)gabi::object_at<mDoExt_McaMorf>(morf).mFrameCtrl.mEnd.get() -
                1.0f;                                      /* getEndFrame() - 1.0f */
    if (Btsw_J3DFrameCtrl_checkPass(morf + 0x98, frame)) { /* checkFrame */
        s8 anm = field_0x9C5;
        if (anm == 4 || anm == 6)
            setAnm(1);
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}

/* 02217C54 */
static s16 daNpc_Btsw_XyCheckCB(void* actor, int i_itemBtn) {
    /* dComIfGp_getSelectItem(i_itemBtn) == dItemNo_NOTE_TO_MOM_e */
    return dComIfGp_getSelectItem(i_itemBtn) == 0x99;
}

/* 02218D58 */
BOOL daNpc_Btsw_c::CreateInit() {
    u32 a = A(this);
    attention_info.flags = 10; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gravity = -30.0f;          /* gravity */
    /* field_0x96E = current.angle */
    field_0x96E.z = (s16)(u16)current.angle.z.get();
    field_0x96E.x = (s16)(u16)current.angle.x.get();
    field_0x96E.y = (s16)(u16)current.angle.y.get();
    btsw_setAction(this, BTSW_wait_action);
    /* mAttPos = current.pos */
    gabi::object_at<BtswVectorWords>(gabi::ea(&mAttPos)).x =
        gabi::object_at<BtswVectorWords>(gabi::ea(&current.pos)).x.get();
    gabi::object_at<BtswVectorWords>(gabi::ea(&mAttPos)).z =
        gabi::object_at<BtswVectorWords>(gabi::ea(&current.pos)).z.get();
    gabi::object_at<BtswVectorWords>(gabi::ea(&mAttPos)).y =
        gabi::object_at<BtswVectorWords>(gabi::ea(&current.pos)).y.get();
    Btsw_dCcD_Stts_Init(gabi::ea(&mStts), 0xFF, 0xFF, a); /* mStts.Init */
    Btsw_dCcD_Cyl_Set(gabi::ea(&mCyl), 0x101BD6A0u);      /* mCyl.Set(l_cyl_src) */
    mCyl.mStts.v = gabi::ea(&mStts);                      /* mCyl.SetStts(&mStts) */
    Btsw_fopNpc_npc_c_setCollision(a, 60.0f, 150.0f);     /* setCollision */
    field_0x9B4 = 0;
    eventInfo.mpCheckCB = gabi::guest_fn(
        (const void*)&daNpc_Btsw_XyCheckCB); /* eventInfo.setXyCheckCB(daNpc_Btsw_XyCheckCB) */
    Btsw_dNpc_EventCut_c_setActorInfo2(gabi::ea(&mEventCut), gabi::guest_addr("Btsw") /* "Btsw" */,
                                       a); /* mEventCut.setActorInfo2 */
    field_0x953 = 0;
    field_0x9C8 = -1;
    field_0x9B0 = fpcM_ERROR_PROCESS_ID_e;
    attention_info.distances[1] = 0x6F; /* attention_info.distances[TALK] */
    field_0x952 = 0;
    attention_info.distances[3] = 0x6F; /* attention_info.distances[SPEAK] */
    Btsw_STControl_setWaitParm(gabi::ea(&field_0x978), 5, 2, 3, 2, 0, 0x2000, 1.0f,
                               0.9f); /* field_0x978.setWaitParm */
    set_mtx();
    Btsw_mDoExt_McaMorf_calc(mpMorf.v.get()); /* mpMorf->calc() */
    Btsw_dKy_tevstr_init(gabi::ea(&field_0x6D4), (s32)home.roomNo.get(),
                         0xFF); /* dKy_tevstr_init(&field_0x6D4, home.roomNo, 0xFF) */
    return TRUE;
}

/* 02218958 */
void daNpc_Btsw_c::set_mtx() {
    u32 a = A(this);
    f32 x = current.pos.x.get(), z = current.pos.z.get();
    u32 model = mpMorf->mpModel.v.get();
    f32 y = current.pos.y.get();
    const u32 stack = 0x1048D0CC;
    Btsw_PSMTXTrans(stack, x, y, z);                      /* transS(current.pos) */
    Btsw_mDoMtx_YrotM(stack, (s32)current.angle.y.get()); /* YrotM(current.angle.y) */
    mtx_copy(&gabi::object_at<J3DModel>(model).mBaseTransformMtx, &gabi::object_at<Mtx34>(stack));
    s8 anm = field_0x9C5;
    if (anm == 10 || anm == 9) {
        u32 morf = mpMorf.v.get();
        f32 vx, vy, vz;
        s16 rx, ry, rz;
        s8 jnt;
        if (gabi::object_at<mDoExt_McaMorf>(morf).mFrameCtrl.mFrame.get() <
            19.0f) { /* mpMorf->getFrame() */
            jnt = m_handR;
            if (anm == 9) {
                vx = 28.68f;
                vy = 4.68f;
                vz = -8.45f;
                rx = -0x43A2;
                ry = 0x9CA;
                rz = -0x233;
            } else {
                vx = 28.68f;
                vy = -0.43f;
                vz = -8.19f;
                rx = -0x43A2;
                ry = 0x9CA;
                rz = -0x2CB;
            }
        } else {
            if (anm == 9) {
                vx = 23.61f;
                vy = -5.08f;
                vz = -7.22f;
                rx = -0x49FC;
                ry = -0x458;
                rz = -0x568F;
            } else {
                vx = 24.9f;
                vy = 0.62f;
                vz = -7.51f;
                rx = -0x4C7F;
                ry = -0x7A;
                rz = -0x491D;
            }
            jnt = m_handL;
        }
        Btsw_PSMTXCopy(btsw_getAnmMtx(gabi::object_at<mDoExt_McaMorf>(morf).mpModel.v.get(), jnt),
                       stack);                                 /* copy(getAnmMtx(jnt)) */
        Btsw_mDoMtx_stack_c_transM(vx, vy, vz);                /* transM(vec) */
        Btsw_mDoMtx_XYZrotM(stack, (s32)rx, (s32)ry, (s32)rz); /* XYZrotM */
        mtx_copy(&field_0x6D0->mBaseTransformMtx, &gabi::object_at<Mtx34>(stack));
    }
}

/* 0221AA18 */
void daNpc_Btsw_c::setAttention() {
    u32 a = A(this);
    f32 z = mAttPos.z, y = mAttPos.y;
    f32 offset = l_HIO.mNpc.mAttnYOffset.get(); /* l_HIO.mNpc.mAttnYOffset */
    f32 x = mAttPos.x;
    attention_info.position.z = z;
    attention_info.position.x = x;
    attention_info.position.y = gabi::fadds_ppc(y, offset);
}

/* 0221AA40 */
void daNpc_Btsw_c::lookBack() {
    u32 a = A(this);
    s8 mode = field_0x9CA;
    f32 vx = 0.0f, vy = 0.0f, vz = 0.0f;
    u32 dstPos = 0;
    s16 desired_y_rot = current.angle.y.get();
    cXyz eye;
    cXyz vec2;
    if (mode == 1 || mode == 2) {
        if (mode == 2)
            m_jnt.mbTrn = 1; /* setTrn() */
        if (mHasAttention) {
            Btsw_dNpc_playerEyePos(&eye,
                                   l_HIO.mNpc.m04.get()); /* dNpc_playerEyePos(l_HIO.mNpc.m04) */
            vec2.copy(*&eye);
            dstPos = A(&vec2);
            vx = current.pos.x.get();
            vy = eyePos.y.get(); /* eyePos.y */
            vz = current.pos.z.get();
        }
    }
    cXyz vec;
    s16 vel;
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        Btsw_cLib_addCalcAngleS2(
            gabi::ea(&field_0x96A), (s32)l_HIO.mNpc.mMaxHeadTurnVel.get(), 4,
            0x800); /* cLib_addCalcAngleS2(&field_0x96A, mMaxHeadTurnVel, 4, 0x800) */
        vel = field_0x96A;
    } else {
        field_0x96A = 0;
        vel = 0;
    }
    vec.x = vx;
    vec.y = vy;
    vec.z = vz;
    /* m_jnt.lookAtTarget(&current.angle.y, dstPos, vec, desired_y_rot, field_0x96A, true) */
    Btsw_dNpc_JntCtrl_c_lookAtTarget(gabi::ea(&m_jnt), gabi::ea(&current.angle.y), dstPos, &vec,
                                     (s32)desired_y_rot, (s32)vel, 1);
}

/* 0221ABCC */
void daNpc_Btsw_c::wait01() {
    if (field_0x9A9) {
        field_0x9CA = 2;
        return;
    }
    u32 morf = mpMorf.v.get();
    f32 frame = (f32)gabi::object_at<mDoExt_McaMorf>(morf).mFrameCtrl.mEnd.get() - 1.0f;
    if (Btsw_J3DFrameCtrl_checkPass(morf + 0x98,
                                    frame)) { /* mpMorf->checkFrame(getEndFrame() - 1.0f) */
        s8 count = (s8)(field_0x9C6 - 1);
        field_0x9C6 = count;
        if (count <= 0) {
            if (field_0x9C5 == 9) {
                field_0x9C6 = 1;
                setAnm(10);
            } else {
                f32 random = Btsw_cM_rndF(4.0f);
                field_0x9C6 = (s8)gabi::ftoi(gabi::fadds_ppc(random, 2.0f));
                setAnm(9);
            }
        }
    }
    if (mHasAttention)
        field_0x9C7 = 2;
}

/* 0221ACDC */
void daNpc_Btsw_c::talk01() {
    u32 a = A(this);
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e &&
        (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3 /* chkTalkXY */ &&
        !Btsw_dEvent_manager_c_ChkPresentEnd(dComIfGp_ea() +
                                             0x52C4) /* dComIfGp_evmng_ChkPresentEnd() */)
        return;
    u32 talk_res = Btsw_fopNpc_npc_c_talk(a, 1); /* talk(1) */
    /* HD: also for 0x1A97 */
    if (dComIfGp_checkMesgSendButton() == 1) { /* dComIfGp_checkMesgSendButton() */
        u32 msg = mCurrMsgNo;
        if (msg == 0x1A96 || msg == 0x1AA9 || msg == 0x1A97) {
            /* mSwCam.ActiveOn() */
            mSwCam.field_0x1E = 1;
            mSwCam.field_0x00.copy(
                camera_center_data.mPositions[0][1]); /* camera_center_data[0][1] */
            mSwCam.field_0x0C.copy(camera_eye);       /* camera_eye */
            mSwCam.field_0x1D = 0;
            mSwCam.field_0x18 = 58.0f;
            mSwCam.field_0x1C = 0;
        }
    }
    if (talk_res != 0x12 /* fopMsgStts_BOX_CLOSED_e */)
        return;
    field_0x9CA = 1;
    dComIfGp_event_reset();
    setAnm(1);
    u32 msg = mCurrMsgNo;
    field_0x9C6 = 1;
    field_0x9A9 = 0;
    if (msg == 0x1A97) {
        btsw_setAction(this, BTSW_shiwake_game_action);
    } else if (msg == 0x1A9B) {
        field_0x9C7 = 3;
    }
}

/* 0221AF2C */
BOOL daNpc_Btsw_c::wait_action(void* arg) {
    u32 a = A(this);
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        field_0x9CA = 1;
        field_0x9C6 = 1;
        setAnm(1);
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (status == ACTION_ENDING)
        return TRUE;
    f32 px = current.pos.x.get();
    s16 head_y = m_jnt.mAngles[0][1].get();
    cXyz pos;
    pos.x = px;
    s16 angle_y = current.angle.y.get();
    s16 backbone_y = m_jnt.mAngles[1][1].get();
    f32 pz = current.pos.z.get();
    f32 py = current.pos.y.get();
    pos.z = pz;
    pos.y = py;
    u8 att = Btsw_daNpc_Btsw_c_chkAttention(
        a, &pos, (s32)(s16)(angle_y + head_y + backbone_y)); /* chkAttention */
    s8 mode = field_0x9CA;
    mHasAttention = att;
    field_0x9C7 = 0;
    if (mode == 1)
        wait01();
    else if (mode == 2)
        talk01();
    lookBack();
    setAttention();
    return TRUE;
}

/* 0221B03C */
BOOL daNpc_Btsw_c::dummy_event_action(void* arg) {
    u32 a = A(this);
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        s8 st = mActionStatus;
        field_0x9CA = 2;
        mActionStatus = (s8)(st + 1);
        return TRUE;
    }
    if (status == ACTION_ENDING)
        return TRUE;
    /* dComIfGp_evmng_getMyStaffId(mEventCut.getActorName()) */
    u32 name = mEventCut.mpEvtStaffName.get();
    u32 staffIdx = Btsw_dEvent_manager_c_getMyStaffId(dComIfGp_ea() + 0x52C4, name, 0, 0);
    if (!Btsw_dNpc_EventCut_c_cutProc(gabi::ea(&mEventCut))) {          /* mEventCut.cutProc() */
        Btsw_dEvent_manager_c_cutEnd(dComIfGp_ea() + 0x52C4, staffIdx); /* dComIfGp_evmng_cutEnd */
    }
    s16 angle_y = current.angle.y.get();
    s16 backbone_y = m_jnt.mAngles[1][1].get();
    cXyz pos;
    f32 px = current.pos.x.get(), py = current.pos.y.get();
    s16 head_y = m_jnt.mAngles[0][1].get();
    pos.x = px;
    f32 pz = current.pos.z.get();
    pos.y = py;
    pos.z = pz;
    u8 att = Btsw_daNpc_Btsw_c_chkAttention(
        a, &pos, (s32)(s16)(angle_y + head_y + backbone_y)); /* chkAttention */
    s16 eventIdx = field_0x9C8;
    mHasAttention = att;
    if (Btsw_dEvent_manager_c_endCheck(dComIfGp_ea() + 0x52C4,
                                       (s32)eventIdx)) { /* dComIfGp_evmng_endCheck(field_0x9C8) */
        field_0x9C8 = -1;
        field_0x9C7 = 3;
        dComIfGp_event_reset();
        btsw_setAction(this, BTSW_wait_action);
    }
    lookBack();
    setAttention();
    return TRUE;
}

/* 0221B300 */
BOOL daNpc_Btsw_c::checkNextMailThrowOK() {
    u32 a = A(this);
    u8 mailIdx = field_0x92C;
    if (mailIdx < 2)
        mailIdx++;
    else
        mailIdx = 0;
    u32 mail = gabi::object_at<daNpc_Btsw_c>(a).field_0x920[mailIdx].v.get();
    if (btsw_mailCheckProc(mail, BTSW_MAIL_Dummy))
        return TRUE;
    if (btsw_mailCheckProc(mail, BTSW_MAIL_End))
        return TRUE;
    return FALSE;
}

/* 0221B254 */
void daNpc_Btsw_c::TimerCountDown() {
    s32 rest = dComIfG_getTimerRestTimeMs();
    s32 next = field_0x9A4;
    if (next > rest) {
        if (next <= 10000) {
            btsw_seStart(0x8AD); /* JA_SE_MINIGAME_TIMER_30 */
            field_0x9A4 = field_0x9A4 - 1000;
        } else {
            btsw_seStart(0x8AE); /* JA_SE_MINIGAME_TIMER_10 */
            field_0x9A4 = field_0x9A4 - 10000;
        }
    }
}

// HD: Pad repeat/trigger handling and minigame system notifications are HD additions. The two position guards are stored in different orders.
/* 0221B3A8 */
BOOL daNpc_Btsw_c::shiwake_game_action(void* arg) {
    u32 a = A(this);
    /* static cXyz aim_pos_data[2][3] (HD guest location) */
    if (l_aimPositionGuard.get() == 0) {

        l_aimPositionGuard = 1;
        const f32 x[3] = {-40.0f, -139.0f, -240.0f};
        for (u32 r = 0; r < 2; r++)
            for (u32 c = 0; c < 3; c++) {
                auto& position = aim_pos_data.mPositions[r][c];
                position.x = x[c];
                position.y = r == 0 ? 772.0f : 855.0f;
                position.z = 1035.0f;
            }
    }
    /* static cXyz cursor_pos_data[2][3] (HD guest location) */
    if (l_cursorPositionGuard.get() == 0) {

        const f32 x[3] = {-52.0f, -140.0f, -227.0f};
        for (u32 r = 0; r < 2; r++)
            for (u32 c = 0; c < 3; c++) {
                auto& position = cursor_pos_data.mPositions[r][c];
                position.x = x[c];
                position.y = r == 0 ? 804.0f : 889.0f;
                position.z = 996.0f;
            }
        l_cursorPositionGuard = 1;
    }
    const u32 h = BTSW_HIO;
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        u16 command = eventInfo.mCommand.get();
        field_0x9A0 = 0;
        field_0x9CA = 3;
        field_0x9A1 = 0;
        if (command != 2) { /* !eventInfo.checkCommandDemoAccrpt() */
            Btsw_fopAcM_orderOtherEvent2(a, gabi::guest_addr("SHIWAKEGAME") /* "SHIWAKEGAME" */, 1,
                                         0xFFFF); /* fopAcM_orderOtherEvent */
            eventInfo.mCondition =
                (u16)(eventInfo.mCondition.get() | 2); /* onCondition(dEvtCnd_UNK2_e) */
            return FALSE;
        }
        if (!field_0x952) {
            /* dTimer_createTimer(6, l_HIO.field_0x30, 3, 4, 221.0f, l_HIO.field_0x48, 32.0f, l_HIO.field_0x4C) */
            Btsw_dTimer_createTimer(6, (u32)(u16)l_HIO.field_0x30.get(), 3, 4, 221.0f,
                                    l_HIO.field_0x48.get(), 32.0f, l_HIO.field_0x4C.get());
            field_0x952 = 1;
            dComIfGp_setMiniGameRupee(0); /* dComIfGp_setMiniGameRupee(0) */
            /* dComIfGp_startMiniGame(7) (HD inline) */
            u32 p = dComIfGp_ea();
            u16 flags = gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags.get();
            gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMiniGameType = 7;
            gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags = (u16)(flags | 0x40);
            u32 g = l_miniGameSystem.get();
            l_miniGameActive = 1;
            Btsw_startMiniGame(gabi::object_at<BtswMiniGameSystem>(g).mpGame.get());
        }
        if (gabi::ea(dComIfG_getTimerPtr()) == 0)
            return FALSE; /* dComIfG_getTimerPtr() == NULL */
        /* dComIfG_TimerStart(6, l_HIO.field_0x32) (HD inline) */
        s16 time = l_HIO.field_0x32.get();
        if (gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mTimerInfo.mTimerMode.get() ==
            6) {
            u32 timer = gabi::ea(dComIfG_getTimerPtr());
            if (timer != 0) {
                if (time == 0)
                    Btsw_timerStart(timer);
                else
                    Btsw_dTimer_c_start(timer, (s32)time); /* dTimer_c::start */
            }
        }
        field_0x953 = 1;
        btsw_mailCall(MAIL_init, field_0x920[0].v.get());
        btsw_mailCall(MAIL_AppearInit, field_0x920[0].v.get());
        btsw_mailCall(MAIL_init, field_0x920[1].v.get());
        btsw_mailCall(MAIL_DummyInit, field_0x920[1].v.get());
        btsw_mailCall(MAIL_init, field_0x920[2].v.get());
        btsw_mailCall(MAIL_DummyInit, field_0x920[2].v.get());
        s8 cy = field_0x9A1, cx = field_0x9A0;
        field_0x92C = 0;
        mActionStatus = (s8)(mActionStatus + 1);
        /* mpShopCursor->setPos(cursor_pos_data[field_0x9A1][1 + field_0x9A0]); show() */
        Btsw_ShopCursor_setPos((u32)mpShopCursor, 0x10466A0Cu + (u32)((cy * 3 + cx) * 0xC));
        gabi::object_at<ShopCursor_c>((u32)mpShopCursor).m54 = 1;
        field_0x9A4 = 20000;
        Btsw_padSetRepeat(l_pad.get(), 5, 5); /* HD: pad repeat settings for the game */
        return TRUE;
    }
    if (status == ACTION_ENDING) {
        field_0x952 = 0;
        field_0x953 = 0;
        Btsw_padResetRepeat(l_pad.get()); /* HD: pad repeat reset */
        return TRUE;
    }
    if (dComIfG_getTimerRestTimeMs() > 0) {
        TimerCountDown();
        /* HD: field_0x978.checkTrigger() / check*Trigger() replaced by the pad's trigger words */
        u32 pad = l_pad.get();
        u32 hold = gabi::object_at<BtswPadState>(pad).mHold.get();
        u32 trig = gabi::object_at<BtswPadState>(pad).mTrigger.get() |
                   gabi::object_at<BtswPadState>(pad).mRepeatTrigger.get();
        if (hold & 0x00F00000)
            trig &= 0x00F00000;
        s8 cx = field_0x9A0;
        if (cx > -1 && (trig & 0x00440000)) { /* left */
            btsw_seStart(0x8B0);              /* JA_SE_LETTER_GAME_CURSOR */
            field_0x9A0 = (s8)(field_0x9A0 - 1);
        } else if (cx < 1 && (trig & 0x00880000)) { /* right */
            btsw_seStart(0x8B0);
            field_0x9A0 = (s8)(field_0x9A0 + 1);
        }
        s8 cy = field_0x9A1;
        if (cy < 1 && (trig & 0x00110000)) { /* up */
            btsw_seStart(0x8B0);
            field_0x9A1 = (s8)(field_0x9A1 + 1);
        } else if (cy > 0 && (trig & 0x00220000)) { /* down */
            btsw_seStart(0x8B0);
            field_0x9A1 = (s8)(field_0x9A1 - 1);
        }
        if (btsw_mailCheckProc(
                gabi::object_at<daNpc_Btsw_c>(a).field_0x920[(u32)field_0x92C].v.get(),
                BTSW_MAIL_Wait) &&
            gabi::object_at<BtswTimerState>(gabi::ea(dComIfG_getTimerPtr())).mStatus.get() !=
                1 /* dComIfG_getTimerPtr()->getStatus() */
            && Btsw_padCheckTriggerA(0) /* CPad_CHECK_TRIG_A(0) */ && checkNextMailThrowOK()) {
            s8 col = field_0x9A1;
            s8 row = field_0x9A0;
            u32 mail = gabi::object_at<daNpc_Btsw_c>(a).field_0x920[(u32)field_0x92C].v.get();
            auto& targetAim = aim_pos_data.entry(
                col, s32(row) + 1); /* aim_pos_data[field_0x9A1][field_0x9A0 + 1] */
            cXyz aim_pos;
            f32 ax = targetAim.x, az = targetAim.z, ay = targetAim.y;
            aim_pos.z = az;
            aim_pos.x = ax;
            aim_pos.y = ay;
            gabi::call(MAIL_ThrowInit, mail, &aim_pos,
                       (u32)(u8)(row - col * 3 + 4)); /* idx = row + 1 + (1 - col) * 3 */
            u8 mailIdx = field_0x92C;
            if (mailIdx < 2) {
                field_0x92C = (u8)(mailIdx + 1);
                btsw_seStart(0x8B1); /* JA_SE_LETTER_GAME_OK */
            } else {
                field_0x92C = 0;
                btsw_seStart(0x8B1);
            }
            btsw_mailCall(MAIL_AppearInit,
                          gabi::object_at<daNpc_Btsw_c>(a).field_0x920[(u32)field_0x92C].v.get());
        }
        /* mSwCam.setAimIdx(field_0x9A0, field_0x9A1) */
        s8 cy2 = field_0x9A1;
        s8 cx2 = field_0x9A0;
        mSwCam.field_0x1D = cy2;
        mSwCam.field_0x1C = cx2;
        Btsw_ShopCursor_setPos((u32)mpShopCursor,
                               0x10466A0Cu + (u32)((cy2 * 3 + cx2) * 0xC)); /* setPos */
        /* mpShopCursor->setScale(0.65f, 0.9f, 0.5f, 27.0f, 20.0f) */
        u32 cursor = mpShopCursor;
        gabi::object_at<ShopCursor_c>((u32)cursor).m50 = 0.5f;
        gabi::object_at<ShopCursor_c>((u32)cursor).m48 = 0.65f;
        gabi::object_at<ShopCursor_c>((u32)cursor).m4C = 0.9f;
        gabi::object_at<ShopCursor_c>((u32)cursor).m38 = 27.0f;
        gabi::object_at<ShopCursor_c>((u32)cursor).m3C = 20.0f;
        Btsw_ShopCursor_c_anm_play((u32)mpShopCursor); /* anm_play */
        btsw_moveMails(a);
        return TRUE;
    }
    s8 st = mActionStatus;
    if (st == ACTION_ONGOING) {
        btsw_seStart(0x8AF); /* JA_SE_LETTER_GAME_TIMER_0 */
        u32 timer = gabi::ea(dComIfG_getTimerPtr());
        Btsw_dTimer_c_end(
            timer, (s32)l_HIO.field_0x34.get()); /* dComIfG_getTimerPtr()->end(l_HIO.field_0x34) */
        gabi::object_at<ShopCursor_c>((u32)mpShopCursor).m54 = 0; /* hide() */
        mActionStatus = (s8)(mActionStatus + 1);
        btsw_moveMails(a);
        return TRUE;
    }
    u32 timer = gabi::ea(dComIfG_getTimerPtr());
    if (st == ACTION_UNK_2) {
        if (Btsw_timerIsDelete(timer)) {                                  /* deleteCheck */
            Btsw_dTimer_c_deleteRequest(gabi::ea(dComIfG_getTimerPtr())); /* deleteRequest */
            mActionStatus = (s8)(mActionStatus + 1);
        }
        btsw_moveMails(a);
        return TRUE;
    }
    if (timer == 0) {
        u32 staff_id = Btsw_dEvent_manager_c_getMyStaffId(
            dComIfGp_ea() + 0x52C4, gabi::guest_addr("Btsw") /* "Btsw" */, 0, 0);
        Btsw_dEvent_manager_c_cutEnd(dComIfGp_ea() + 0x52C4, staff_id); /* dComIfGp_evmng_cutEnd */
        dComIfGp_event_reset();
        s16 rupees = gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0)
                         .mMiniGameRupee.get(); /* dComIfGp_getMiniGameRupee() */
        /* daNpc_Btsw_getGameEndMsg(rupees) (inline) */
        u32 msgNo;
        if (rupees == 0)
            msgNo = 0x1A9D;
        else if (rupees == 1)
            msgNo = 0x1A9E;
        else if (rupees < 25)
            msgNo = 0x1A9F;
        else
            msgNo = 0x1A98;
        field_0x9B0 = msgNo;
        if (rupees < 25) {
            field_0x9C7 = 1;
            btsw_setAction(this, BTSW_wait_action);
            mSwCam.field_0x1E = 0; /* ActiveOff() */
            field_0x9C8 = -1;
        } else {
            field_0x9C7 = 4;
            u32 idx = Btsw_dEvent_manager_c_getEventIdx(
                dComIfGp_ea() + 0x52C4, gabi::guest_addr("BTSW_TALK_SUGOI") /* "BTSW_TALK_SUGOI" */,
                0xFF);
            field_0x9C8 = (s16)idx;
            btsw_setAction(this, BTSW_dummy_event_action);
            mSwCam.field_0x1E = 0;
        }
        if (dComIfGp_getMiniGameType() == 7)
            btsw_endMiniGame(); /* dComIfGp_getMiniGameType() == 7 */
    }
    btsw_moveMails(a);
    return TRUE;
}

/* 0221BE68 */
BOOL daNpc_Btsw_c::getdemo_action(void* arg) {
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        mActionStatus = 1;
        return TRUE;
    }
    if (status == ACTION_ENDING)
        return TRUE;
    if (!Btsw_dEvent_manager_c_endCheckOld(
            dComIfGp_ea() + 0x52C4, gabi::guest_addr("GETMOTHERLETTER") /* "GETMOTHERLETTER" */))
        return TRUE; /* dComIfGp_evmng_endCheck */
    dComIfGp_event_reset();
    field_0x9C7 = 1;
    field_0x9B0 = 0x1A9C;
    dComIfGs_onEventBit(0x2701);
    Btsw_dSv_event_c_setEventReg(dComIfGs_ea() + 0x644, 0x8AFF,
                                 25); /* dComIfGs_setEventReg(UNK_8AFF, 25) */
    btsw_setAction(this, BTSW_wait_action);
    return TRUE;
}

/* 02219840 */
BOOL daNpc_Btsw_c::_draw() {
    u32 a = A(this);
    u32 model = mpMorf->mpModel.v.get();
    u32 modelData = gabi::object_at<J3DModel>(model).mModelData.v.get();
    u32 env = Btsw_dKy_getEnvlight();
    Btsw_dScnKy_env_light_c_settingTevStruct(
        env, 0, gabi::ea(&current.pos),
        gabi::ea(&tevStr)); /* settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr) */
    env = Btsw_dKy_getEnvlight();
    Btsw_setLightTevColorType(env, model,
                              gabi::ea(&tevStr)); /* setLightTevColorType(model, &tevStr) */
    Btsw_mDoExt_btpAnm_entry(gabi::ea(&field_0x7E8), modelData,
                             (u32)field_0x7FC);  /* field_0x7E8.entry */
    Btsw_mDoExt_McaMorf_entryDL(mpMorf.v.get()); /* mpMorf->entryDL() */
    s8 anm = field_0x9C5;
    if (anm == 10 || anm == 9) {
        env = Btsw_dKy_getEnvlight();
        Btsw_dScnKy_env_light_c_settingTevStruct(env, 0, gabi::ea(&current.pos),
                                                 gabi::ea(&field_0x6D4));
        /* field_0x6D4.mColorK0 / mColorC0 */
        field_0x6D4.mColorC0.b = 0xC0;
        field_0x6D4.mColorK0.g = 0xAE;
        field_0x6D4.mColorK0.b = 0xC0;
        field_0x6D4.mColorC0.g = 0xAE;
        field_0x6D4.mColorK0.r = 0xC0;
        field_0x6D4.mColorC0.r = 0xC0;
        env = Btsw_dKy_getEnvlight();
        Btsw_setLightTevColorType(env, field_0x6D0.v.get(), gabi::ea(&field_0x6D4));
        Btsw_mDoExt_modelUpdateDL(field_0x6D0.v.get(), 0); /* mDoExt_modelUpdateDL(field_0x6D0) */
    }
    gabi::object_at<BtswModelDataWork>(modelData).mpTexPatternAnimation =
        0; /* field_0x7E8.remove(modelData) */
    if (field_0x953 != 0) {
        for (s32 i = 0; i < 3; i++)
            btsw_mailCall2(MAIL_draw, gabi::object_at<daNpc_Btsw_c>(a).field_0x920[i].v.get(),
                           gabi::ea(&tevStr));
    }
    Btsw_ShopCursor_c_draw((u32)mpShopCursor); /* mpShopCursor->draw() */
    Btsw_dSnap_RegistFig(0x98 /* DSNAP_TYPE_NPC_BTSW */, a, gabi::ea(&current.pos),
                         (s32)current.angle.y.get(), 1.0f, 1.0f, 1.0f);
    /* HD: function-local statics (the GameCube's unused GXColor table), initialised on first use
     * behind an HIO flag */
    if (l_HIO.mNpc.m22.get() != 0) {
        if (l_debugColorOneGuard.get() == 0) {
            l_debugColorOneGuard = 1;
            Btsw_copyDebugColor(/* memcpy */ 0x101FEBF4u, 0x100186B0u, 4);
        }
        if (l_debugColorTwoGuard.get() == 0) {
            l_debugColorTwoGuard = 1;
            Btsw_copyDebugColor(/* memcpy */ 0x101FEBF8u, 0x100186B4u, 4);
        }
    }
    return TRUE;
}

/* 02219618 */
BOOL daNpc_Btsw_c::_execute() {
    u32 a = A(this);
    const u32 h = BTSW_HIO;
    /* m_jnt.setParam(l_HIO.mNpc...) */
    s16 p1 = l_HIO.mNpc.mMaxBackboneX.get(), p6 = l_HIO.mNpc.mMaxHeadY.get(),
        p2 = l_HIO.mNpc.mMaxBackboneY.get(), p3 = l_HIO.mNpc.mMinBackboneX.get();
    s16 p7 = l_HIO.mNpc.mMinHeadX.get(), p4 = l_HIO.mNpc.mMinBackboneY.get(),
        p8 = l_HIO.mNpc.mMinHeadY.get(), p5 = l_HIO.mNpc.mMaxHeadX.get();
    s16 p9 = l_HIO.mNpc.mMaxTurnStep.get();
    Btsw_dNpc_JntCtrl_c_setParam(gabi::ea(&m_jnt), (s32)p1, (s32)p2, (s32)p3, (s32)p4, (s32)p5,
                                 (s32)p6, (s32)p7, (s32)p8, (s32)p9);
    Btsw_STControl_setWaitParm(gabi::ea(&field_0x978), 5, 2, 3, 2, 0, 0x2000,
                               l_HIO.field_0x50.get(), l_HIO.field_0x54.get()); /* setWaitParm */
    playTexPatternAnm();
    Btsw_mDoExt_McaMorf_play(mpMorf.v.get(), gabi::ea(&eyePos), 0,
                             0);              /* mpMorf->play(&eyePos, 0, 0) */
    Btsw_mDoExt_McaMorf_calc(mpMorf.v.get()); /* mpMorf->calc() */
    checkOrder();
    btsw_actionCall(this, 0);                         /* (this->*mCurrActionFunc)(NULL) */
    Btsw_SwCam2_c_Move(gabi::ea(&mSwCam.field_0x00)); /* mSwCam.Move() */
    eventOrder();
    Btsw_fopAcM_posMoveF(a, gabi::ea(&mStts)); /* fopAcM_posMoveF(this, mStts.GetCCMoveP()) */
    u32 play = dComIfGp_ea();
    Btsw_dBgS_Acch_CrrPos(gabi::ea(&mObjAcch),
                          play + 0x12A0); /* mObjAcch.CrrPos(*dComIfG_Bgsp()) */
    play = dComIfGp_ea();
    tevStr.mRoomNo = (s8)((u8)Btsw_dBgS_GetRoomId(
        play + 0x12A0, gabi::ea(static_cast<cBgS_PolyInfo*>(
                           &mObjAcch.m_gnd)))); /* tevStr.mRoomNo = GetRoomId(m_gnd) */
    play = dComIfGp_ea();
    tevStr.mEnvrIdxOverride = ((u8)Btsw_dBgS_GetPolyColor(
        play + 0x12A0, gabi::ea(static_cast<cBgS_PolyInfo*>(
                           &mObjAcch.m_gnd)))); /* mEnvrIdxOverride = GetPolyColor(m_gnd) */
    set_mtx();
    Btsw_fopNpc_npc_c_setCollision(a, 60.0f, 150.0f); /* setCollision */
    return TRUE;
}

/* 022190BC */
BOOL daNpc_Btsw_c::_delete() {
    u32 a = A(this);
    /* HD: no fopAcM_RegisterDeleteID */
    Btsw_dComIfG_resDelete(gabi::ea(&mPhs),
                           gabi::guest_addr("Btsw") /* "Btsw" */); /* dComIfG_resDeleteDemo */
    if (heap.v.get() != 0) {                                       /* heap */
        u32 morf = mpMorf.v.get();
        if (morf != 0)
            Btsw_mDoExt_McaMorf_stopZelAnime(morf); /* stopZelAnime */
    }
    Btsw_SwMail_SeDelete(gabi::ea(&mSwMail0)); /* mSwMail0.SeDelete() */
    Btsw_SwMail_SeDelete(gabi::ea(&mSwMail1));
    Btsw_SwMail_SeDelete(gabi::ea(&mSwMail2));
    s8 no = l_HIO.mNo.get();
    if (no >= 0) {
        Btsw_mDoHIO_subRoot_c_deleteChild((s32)no); /* mDoHIO_deleteChild */
        l_HIO.mNo = -1;
    }
    return TRUE;
}

/* 022186E8 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_this) { return ((daNpc_Btsw_c*)i_this)->CreateHeap(); }

/* 02218FA4 */
cPhs_State daNpc_Btsw_c::_create() {
    u32 a = A(this);
    /* fopAcM_ct(this, daNpc_Btsw_c) */
    u32 cond = actor_condition.get();
    if (!(cond & 8)) {
        if (a != 0) {
            Btsw_actorConstruct(a);
            cond = actor_condition.get();
        }
        actor_condition = cond | 8;
    }
    cPhs_State res = Btsw_dComIfG_resLoad(
        gabi::ea(&mPhs), gabi::guest_addr("Btsw") /* "Btsw" */); /* dComIfG_resLoad */
    if (res == cPhs_COMPLEATE_e) {
        if (!Btsw_fopAcM_entrySolidHeap(a, 0x022186E8u, 0xCDA0))
            return cPhs_ERROR_e; /* fopAcM_entrySolidHeap */
        u32 model = mpMorf->mpModel.v.get();
        cullMtx = model ? model + 0xC8 : 0; /* fopAcM_SetMtx(this, getBaseTRMtx()) */
        if (l_HIO.mNo.get() < 0) {
            /* mDoHIO_createChild("仕分けバイト君", &l_HIO) */
            l_HIO.mNo = (u8)Btsw_mDoHIO_subRoot_c_createChild(0x100188A0u, BTSW_HIO);
        }
        if (!CreateInit())
            return cPhs_ERROR_e;
    }
    return res;
}

/* 022182F4 */
BOOL daNpc_Btsw_c::CreateHeap() {
    u32 a = A(this);
    u32 modelData = btsw_getIDRes("Btsw" /* "Btsw" */, dRes_ID_BTSW_BDL_BN_e);
    if (modelData == 0)
        btsw_assert(gabi::guest_addr("d_a_npc_btsw.cpp"), 0x5BA,
                    gabi::guest_addr("modelData != (0)"));
    u32 bck = btsw_getIDRes("Btsw", dRes_ID_BTSW_BCK_BN_WAIT02_e);
    u32 morf = Btsw_mDoExt_McaMorf_mDoExt_McaMorf(0u, modelData, 0u, 0u, bck, 2, 0, -1, 1, 0,
                                                  0x80000, 0x15020022, 1.0f);
    mpMorf.v = morf;
    if (morf == 0 || gabi::object_at<mDoExt_McaMorf>(morf).mpModel.v.get() == 0)
        return FALSE;
    s8 head = (s8)btsw_jointIndex(modelData, gabi::guest_addr("head") /* "head" */);
    m_jnt.mHeadJntNum = head;
    if (head < 0)
        btsw_assert(gabi::guest_addr("d_a_npc_btsw.cpp"), 0x5CB,
                    gabi::guest_addr("m_jnt.getHeadJntNum() >= 0"));
    s8 backbone = (s8)btsw_jointIndex(modelData, gabi::guest_addr("backbone") /* "backbone" */);
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0)
        btsw_assert(gabi::guest_addr("d_a_npc_btsw.cpp"), 0x5CD,
                    gabi::guest_addr("m_jnt.getBackboneJntNum() >= 0"));
    m_handL = (s8)btsw_jointIndex(modelData, gabi::guest_addr("handL") /* "handL" */);
    m_handR = (s8)btsw_jointIndex(modelData, gabi::guest_addr("handR") /* "handR" */);
    u32 letterData = btsw_getIDRes("Btsw", dRes_ID_BTSW_BDL_BM_LETTER_e);
    u32 letter = Btsw_mDoExt_J3DModel__create(letterData, 0, 0x11020203);
    field_0x6D0.v = letter;
    if (letter == 0)
        return FALSE;
    field_0x9C4 = 0;
    if (!initTexPatternAnm(0))
        return FALSE;
    /* HD: getJointNum() through HD entry (named __nw by the matcher) */
    for (u32 i = 0;
         i < gabi::object_at<BtswJointTree>(Btsw_J3DModelData_jointTree(modelData)).mCount.get();
         i = (u16)(i + 1)) {
        if (i == (u32)(s32)(s8)m_jnt.mHeadJntNum || i == (u32)(s32)(s8)m_jnt.mBackboneJntNum) {
            /* getJointNodePointer(i)->setCallBack(nodeCallBack) */
            u32 data = mpMorf->mpModel->mModelData.v.get();
            u32 count = gabi::object_at<J3DModelData>(data).mJointNum.get();
            u32 arr = gabi::object_at<J3DModelData>(data).mJointNodes.get();
            u32 node = i < count ? arr + i * 0x1C : arr;
            gabi::object_at<J3DJoint>(node).setCallBack(nodeCallBack);
        }
    }
    mpMorf->mpModel->mUserArea = a; /* setUserArea(this) */
    field_0x920[0].v = gabi::ea(&mSwMail0);
    field_0x920[2].v = gabi::ea(&mSwMail2);
    field_0x92C = 0;
    field_0x920[1].v = gabi::ea(&mSwMail1);
    u32 eyeP = gabi::ea(&mSwCam.field_0x00), centerP = gabi::ea(&mSwCam.field_0x0C);
    if (!gabi::call<BOOL>(MAIL_MailCreateInit, gabi::ea(&mSwMail0), eyeP, centerP))
        return FALSE;
    if (!gabi::call<BOOL>(MAIL_MailCreateInit, field_0x920[1].v.get(), eyeP, centerP))
        return FALSE;
    if (!gabi::call<BOOL>(MAIL_MailCreateInit, field_0x920[2].v.get(), eyeP, centerP))
        return FALSE;
    u32 tevRegKey = btsw_getIDRes("Btsw", dRes_ID_BTSW_BRK_SHOP_CURSOR01_e);
    u32 cursorData = btsw_getIDRes("Btsw", dRes_ID_BTSW_BMD_SHOP_CURSOR01_e);
    u32 cursor = Btsw_ShopCursor_create(cursorData, tevRegKey, 0.65f); /* ShopCursor_create */
    mpShopCursor = cursor;
    if (cursor == 0)
        return FALSE;
    gabi::object_at<ShopCursor_c>((u32)cursor).m54 = 0;          /* m54 */
    Btsw_dBgS_AcchCir_SetWall(gabi::ea(&mAcchCir), 30.0f, 0.0f); /* mAcchCir.SetWall */
    Btsw_dBgS_Acch_Set(gabi::ea(&mObjAcch), gabi::ea(&current.pos), gabi::ea(&old.pos), a, 1,
                       gabi::ea(&mAcchCir), gabi::ea(&speed), 0u, 0u); /* mObjAcch.Set */
    return TRUE;
}

/* 022181EC */
BOOL SwMail2_c::MailCreateInit(cXyz* eye, cXyz* center) {
    u32 m = A(this);
    u32 modelData = btsw_getIDRes("Btsw", dRes_ID_BTSW_BDL_QMAIL_e);
    u32 model =
        Btsw_mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002); /* mDoExt_J3DModel__create */
    mpModel.v = model;
    if (model == 0)
        return FALSE;
    u32 pattern = btsw_getIDRes("Btsw", dRes_ID_BTSW_BTP_QMAIL_e);
    /* field_0x10.init(modelData, texPattern, TRUE, EMode_LOOP, 0.0f, 0, -1, false, FALSE) */
    if (!Btsw_btpInit(gabi::ea(&field_0x10), modelData, pattern, 1, 2, 0, -1, 0, 0, 0.0f))
        return FALSE;
    field_0x5C.v = A(center);
    field_0x54 = 0;
    field_0x58.v = A(eye);
    init();
    DummyInit();
    return TRUE;
}

/* 02217EE4 */
u8 Btsw_getNextNo(u8 previous_no) {
    f32 random = Btsw_cM_rndF(10.0f);
    u8 sameCount = m_same_count.get();                      /* m_same_count */
    f32 fVar4 = gabi::fnmsubs(1.25f, (f32)sameCount, 2.5f); /* 2.5f - m_same_count * 1.25f */
    s32 box_x = previous_no / 3;
    s32 box_y = previous_no - box_x * 3;
    f32 half = 0.5f;
    if (random < fVar4) {
        m_same_count = (u8)(sameCount + 1);
    } else {
        f32 new_threshold = gabi::fadds_ppc(2.5f, fVar4);
        if (random < new_threshold) {
            if (Btsw_cM_rndF(1.0f) > half) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else {
                    box_y = 2;
                    if (Btsw_cM_rndF(1.0f) > half)
                        box_y = 0;
                }
            } else {
                box_x = box_x == 0;
            }
        } else {
            f32 r = Btsw_cM_rndF(1.0f);
            if (r < l_oneThird.get() /* 0.3333f */) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else {
                    box_y = 2;
                    if (Btsw_cM_rndF(1.0f) > half)
                        box_y = 0;
                }
                box_x = box_x == 0;
            } else if (r < l_twoThirds.get() /* 0.6666f */) {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else {
                    box_y = Btsw_cM_rndF(1.0f) > half ? 0 : 2;
                }
            } else {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else {
                    box_y = Btsw_cM_rndF(1.0f) > half ? 0 : 2;
                }
                box_x = box_x == 0;
            }
        }
        m_same_count = 0;
    }
    u8 res = (u8)(box_y + box_x * 3);
    m_no_buff = res; /* m_no_buff */
    return res;
}

/* 02217E74 */
void SwMail2_c::init() {
    u32 m = A(this);
    field_0x56 = 0;
    field_0x54 = 0;
    field_0x30.copy(cXyz::Zero); /* cXyz::Zero */
    field_0x3C.copy(cXyz::Zero);
    field_0x55 = 0;
    u32 c = field_0x5C.v.get();
    f32 cx = gabi::object_at<cXyz>(c).x.get(), cy = gabi::object_at<cXyz>(c).y.get(),
        cz = gabi::object_at<cXyz>(c).z.get();
    field_0x24.x = cx;
    field_0x24.z = cz;
    field_0x24.y = gabi::fsubs_ppc(cy, 200.0f);
    set_mtx();
}

/* 02217D98 */
void SwMail2_c::set_mtx() {
    u32 m = A(this);
    f32 x = gabi::fadds_ppc(field_0x24.x, field_0x3C.x);
    f32 y = gabi::fadds_ppc(field_0x24.y, field_0x3C.y);
    f32 z = gabi::fadds_ppc(field_0x24.z, field_0x3C.z);
    Btsw_PSMTXTrans(0x1048D0CCu, x, y, z); /* transS */
    Btsw_mDoMtx_ZXYrotM(0x1048D0CCu, (s32)field_0x48.x, (s32)field_0x48.y,
                        (s32)field_0x48.z);                                     /* ZXYrotM */
    mtx_copy(&mpModel->mBaseTransformMtx, &gabi::object_at<Mtx34>(0x1048D0CC)); /* setBaseTRMtx */
}

/* 022199FC */
void SwMail2_c::set_mtx_throw() {
    u32 m = A(this);
    f32 x = gabi::fadds_ppc(field_0x24.x, field_0x3C.x);
    f32 y = gabi::fadds_ppc(field_0x24.y, field_0x3C.y);
    f32 z = gabi::fadds_ppc(field_0x24.z, field_0x3C.z);
    Btsw_PSMTXTrans(0x1048D0CCu, x, y, z);             /* transS */
    Btsw_mDoMtx_XrotM(0x1048D0CCu, (s32)field_0x48.x); /* XrotM */
    Btsw_mDoMtx_YrotM(0x1048D0CCu, (s32)field_0x48.y); /* YrotM */
    mtx_copy(&mpModel->mBaseTransformMtx, &gabi::object_at<Mtx34>(0x1048D0CC));
}

/* 02218114 */
void SwMail2_c::DummyInit() {
    u32 m = A(this);
    Btsw_cLib_targetAngleY(field_0x58.v.get(), field_0x5C.v.get()); /* cLib_targetAngleY (unused) */
    field_0x48.y = -0x4000;
    field_0x48.z = 0;
    field_0x48.x = 0;
    u32 c = field_0x5C.v.get();
    f32 cy = gabi::object_at<cXyz>(c).y.get(), cx = gabi::object_at<cXyz>(c).x.get(),
        cz = gabi::object_at<cXyz>(c).z.get();
    field_0x24.z = cz;
    field_0x24.y = gabi::fsubs_ppc(cy, 200.0f);
    field_0x24.x = gabi::fsubs_ppc(cx, 150.0f);
    u8 no = getNextNo(field_0x54);
    field_0x54 = no;
    field_0x4E.z = -0x4000;
    field_0x4E.x = 0;
    field_0x4E.y = -0x4000;
    field_0x3C.copy(cXyz::Zero);
    btsw_mailSetProc(m, BTSW_MAIL_Dummy);
}

/* 02219B20 */
void SwMail2_c::Dummy() { set_mtx(); }

/* 02219B24 */
void SwMail2_c::AppearInit() {
    u32 m = A(this);
    Btsw_cLib_targetAngleY(field_0x58.v.get(), field_0x5C.v.get()); /* cLib_targetAngleY (unused) */
    u32 c = field_0x5C.v.get();
    u8 no = field_0x54;
    field_0x48.y = -0x4000;
    field_0x48.x = 0;
    field_0x48.z = 0;
    f32 cy = gabi::object_at<cXyz>(c).y.get(), cx = gabi::object_at<cXyz>(c).x.get(),
        cz = gabi::object_at<cXyz>(c).z.get();
    field_0x24.x = cx;
    field_0x24.y = gabi::fsubs_ppc(cy, 200.0f);
    field_0x24.z = cz;
    no = getNextNo(no);
    field_0x4E.z = -0x4000;
    field_0x4E.x = 0;
    field_0x4E.y = -0x4000;
    field_0x54 = no;
    field_0x3C.copy(cXyz::Zero);
    field_0x56 = 0;
    btsw_seStart(0x69A5); /* JA_SE_LETTER_GAME_NEW */
    btsw_mailSetProc(m, BTSW_MAIL_Appear);
}

/* 02219C1C */
void SwMail2_c::Appear() {
    u32 m = A(this);
    s16 new_y = Btsw_cLib_targetAngleY(field_0x58.v.get(), field_0x5C.v.get());
    /* cXyz diff = *field_0x58 - *field_0x5C */
    cXyz diff;
    u32 rhs = field_0x5C.v.get();
    Btsw_cXyz___mi(field_0x58.v.get(), &diff, rhs);
    BOOL ok = Btsw_cXyz_normalizeRS(&diff); /* normalizeRS */
    u32 c = field_0x5C.v.get();
    if (!ok) {
        diff.x = 0.0f;
        diff.z = 1.0f;
        diff.y = 0.0f;
    }
    cXyz vec; /* cXyz vec = *field_0x5C */
    vec.copy(gabi::object_at<cXyz>(c));
    cXyz scaled;
    Btsw_cXyz___ml(&diff, &scaled, 120.0f); /* diff * 120.0f */
    diff.copy(*&scaled);
    Btsw_PSVECAdd(&vec, &diff, &vec); /* vec += diff */
    s16 bx = field_0x4E.x, by = field_0x4E.y;
    f32 vy = gabi::fsubs_ppc(vec.y, 40.0f);
    s16 bz = field_0x4E.z;
    field_0x48.x = bx;
    field_0x48.y = (s16)(by + new_y);
    vec.y = vy;
    Btsw_cLib_addCalcAngleS2(
        gabi::ea(&field_0x48.z), (s32)bz, 4,
        0x1000); /* cLib_addCalcAngleS2(&field_0x48.z, field_0x4E.z, 4, 0x1000) */
    f32 step = Btsw_cLib_addCalcPos(gabi::ea(&field_0x24), &vec, 0.25f, 30.0f, 2.5f);
    if (std::fabs(step) < 2.5f)
        WaitInit();
    set_mtx();
}

/* 02219BFC */
void SwMail2_c::WaitInit() { btsw_mailSetProc(A(this), BTSW_MAIL_Wait); }

/* 02219D88 */
void SwMail2_c::Wait() {
    u32 m = A(this);
    s16 new_y = Btsw_cLib_targetAngleY(field_0x58.v.get(), field_0x5C.v.get());
    cXyz diff;
    u32 rhs = field_0x5C.v.get();
    Btsw_cXyz___mi(field_0x58.v.get(), &diff, rhs);
    BOOL ok = Btsw_cXyz_normalizeRS(&diff);
    u32 c = field_0x5C.v.get();
    if (!ok) {
        diff.x = 0.0f;
        diff.z = 1.0f;
        diff.y = 0.0f;
    }
    field_0x24.copy(gabi::object_at<cXyz>(c)); /* field_0x24 = *field_0x5C */
    cXyz scaled;
    Btsw_cXyz___ml(&diff, &scaled, 120.0f);
    diff.copy(*&scaled);
    Btsw_PSVECAdd(gabi::ea(&field_0x24), &diff, gabi::ea(&field_0x24)); /* field_0x24 += diff */
    s16 by = field_0x4E.y;
    f32 py = field_0x24.y;
    s16 bz = field_0x4E.z;
    s16 bx = field_0x4E.x;
    field_0x48.y = (s16)(by + new_y);
    field_0x48.z = bz;
    field_0x48.x = bx;
    field_0x24.y = gabi::fsubs_ppc(py, 40.0f);
    set_mtx();
}

/* 02219EA0 */
void SwMail2_c::ThrowInit(cXyz* aim, u8 no) {
    u32 m = A(this), p = A(aim);
    gabi::object_at<BtswVectorWords>(gabi::ea(&field_0x30)).x =
        gabi::object_at<BtswVectorWords>(p).x.get();
    gabi::object_at<BtswVectorWords>(gabi::ea(&field_0x30)).y =
        gabi::object_at<BtswVectorWords>(p).y.get();
    u32 z = gabi::object_at<BtswVectorWords>(p).z.get();
    mFunc.d = 0;
    gabi::object_at<BtswVectorWords>(gabi::ea(&field_0x30)).z = z;
    field_0x56 = 0;
    mFunc.i = -1;
    mFunc.f = BTSW_MAIL_Throw;
    field_0x55 = no;
}

// HD: The target angle is calculated before the branch, and scoring retains the cached play object.
/* 02219F20 */
void SwMail2_c::Throw() {
    u32 m = A(this);
    u32 center = field_0x5C.v.get();
    u8 step = field_0x56;
    /* HD: x_angle computed once before the branch */
    s16 x_angle = Btsw_cLib_targetAngleX(gabi::ea(&field_0x30),
                                         center); /* cLib_targetAngleX(&field_0x30, field_0x5C) */
    if (step == 0) {
        u32 rhs = field_0x5C.v.get();
        s16 y_angle = Btsw_cLib_targetAngleY(field_0x58.v.get(), rhs);
        cXyz vec;
        vec.z = 0.0f;
        vec.x = -40.0f;
        vec.y = -50.0f;
        Btsw_mDoMtx_ZXYrotS(0x1048D0CCu, (s32)x_angle, (s32)y_angle, 0); /* ZXYrotS */
        cXyz multVec;
        Btsw_PSMTXMultVec(0x1048D0CCu, &vec, &multVec);        /* multVec */
        Btsw_PSVECAdd(&multVec, field_0x5C.v.get(), &multVec); /* multVec += *field_0x5C */
        f32 r = Btsw_cLib_addCalcPos(gabi::ea(&field_0x24), &multVec, 0.5f, 30.0f, 2.0f);
        if (r < 1.0f) {
            field_0x48.y = 0;
            field_0x4E.x = 0;
            btsw_seStart(0x2800); /* JA_SE_LK_SW_KAZEKIRI_S */
            field_0x56 = (u8)(field_0x56 + 1);
        }
        set_mtx();
        return;
    }
    u8 no = field_0x54;
    s16 ay = field_0x48.y;
    field_0x48.z = 0;
    field_0x48.x = (s16)-x_angle;
    field_0x48.y = (s16)(ay + (s32)no * 0x80 + 0x1000);
    f32 pos_step = Btsw_cLib_addCalcPos(gabi::ea(&field_0x24), gabi::ea(&field_0x30), 0.5f,
                                        l_HIO.field_0x44.get(), 1.0f);
    s16 bx = (s16)(field_0x4E.x + 4000);
    field_0x4E.x = bx;
    f32 cosv = cM_scos(bx); /* cM_scos */
    field_0x3C.y = gabi::fmuls_ppc(20.0f, gabi::fsubs_ppc(1.0f, cosv));
    if (pos_step < 1.0f) {
        Btsw_seStartAt(
            0x69A6, gabi::ea(&field_0x24)); /* mDoAud_seStart(JA_SE_LETTER_IN_BOX, &field_0x24) */
        u8 aim = field_0x55;
        u8 cur = field_0x54;
        u32 p = dComIfGp_ea() + 0x12A0;
        if (aim == cur) {
            /* dComIfGp_plusMiniGameRupee(1) (HD: clamped at 0) */
            s16 rupee = gabi::object_at<dComIfG_play_c>(p).mMiniGameRupee.get();
            if ((s32)rupee + 1 > 0)
                gabi::object_at<dComIfG_play_c>(p).mMiniGameRupee =
                    (s16)(gabi::object_at<dComIfG_play_c>(p).mMiniGameRupee.get() + 1);
            else
                gabi::object_at<dComIfG_play_c>(p).mMiniGameRupee = 0;
            btsw_seStart(0x8A5); /* JA_SE_MINIGAME_RIGHT */
        } else {
            cXyz dir;
            dir.x = 0.0f;
            dir.y = 1.0f;
            dir.z = 0.0f;
            Btsw_dVibration_c_StartShock(p + 0x46FC, 3, 9,
                                         &dir); /* dComIfGp_getVibration().StartShock */
            btsw_seStart(0x8A6);                /* JA_SE_MINIGAME_WRONG */
        }
        EndInit();
    }
    set_mtx_throw();
}

/* 02219EE0 */
void SwMail2_c::EndInit() {
    u32 m = A(this);
    /* field_0x30 = cXyz::Zero; field_0x30.z = -10.0f */
    gabi::object_at<BtswVectorWords>(gabi::ea(&field_0x30)).x = l_cXyzZeroXWord.get();
    u32 y = l_cXyzZeroYWord.get();
    field_0x30.z = -10.0f;
    gabi::object_at<BtswVectorWords>(gabi::ea(&field_0x30)).y = y;
    btsw_mailSetProc(m, BTSW_MAIL_End);
}

/* 0221A1E0 */
void SwMail2_c::End() {
    u32 m = A(this);
    u8 no = field_0x54;
    u8 aim = field_0x55;
    if (aim == no) {
        Btsw_cLib_addCalcAngleS2(gabi::ea(&field_0x48.y), 0, 4, 0x1000);
        cXyz target;
        f32 zx = l_cXyzZeroX.get(), zz = l_cXyzZeroZ.get(), zy = l_cXyzZeroY.get();
        target.x = zx;
        target.z = zz;
        target.y = zy;
        Btsw_cLib_addCalcPos2(gabi::ea(&field_0x3C), &target, 0.25f, 5.0f); /* cLib_addCalcPos2 */
        set_mtx_throw();
        return;
    }
    Btsw_cLib_addCalcAngleS2(gabi::ea(&field_0x48.y), -0x8000, 4, 0x1000);
    cXyz target;
    f32 zx = l_cXyzZeroX.get(), zy = l_cXyzZeroY.get(), zz = l_cXyzZeroZ.get();
    target.x = zx;
    target.y = zy;
    target.z = zz;
    Btsw_cLib_addCalcPos2(gabi::ea(&field_0x3C), &target, 0.25f, 5.0f);
    if (field_0x24.z < 995.0f) {
        f32 y = field_0x24.y;
        s16 targetAngle;
        if (y > 720.0f) {
            field_0x30.y = gabi::fsubs_ppc(field_0x30.y, 3.0f);
            targetAngle = -0x4000;
        } else {
            f32 sy = field_0x30.y, sz = field_0x30.z;
            f32 ny = gabi::fmuls_ppc(sy, 0.6f);
            f32 sx = field_0x30.x;
            f32 nz = gabi::fmuls_ppc(sz, 0.9f);
            field_0x30.y = ny;
            field_0x30.x = gabi::fmuls_ppc(sx, 0.9f);
            field_0x30.z = nz;
            targetAngle = 0;
        }
        if (!(field_0x24.y > 700.0f)) {
            field_0x24.y = 700.0f;
            field_0x30.y = 0.0f;
        }
        Btsw_cLib_addCalcAngleS2(gabi::ea(&field_0x48.x), (s32)targetAngle, 2, 0x800);
    }
    Btsw_PSVECScale(gabi::ea(&field_0x30), gabi::ea(&field_0x30), 0.9f); /* field_0x30 *= 0.9f */
    Btsw_PSVECAdd(gabi::ea(&field_0x24), gabi::ea(&field_0x30),
                  gabi::ea(&field_0x24)); /* field_0x24 += field_0x30 */
    set_mtx_throw();
}

/* 022190B4 */
void SwMail2_c::SeDelete() {
    Btsw_JAIZelBasic_seDeleteObject(A(this) + 0x80); /* mDoAud_seDeleteObject(&field_0x24) */
}

/* 02219ADC */
void SwMail2_c::move() {
    u32 m = A(this);
    s16 i = mFunc.i;
    s16 d = mFunc.d;
    u32 recv = m + (u32)(s32)d;
    u32 target;
    if (i < 0) {
        target = mFunc.f;
    } else {
        s16 vofs = Btsw_virtualOffset(mFunc);
        target = Btsw_virtualTarget(recv, vofs, i);
    }
    gabi::call_ptr(target, recv);
}

/* 022197C8 */
void SwMail2_c::draw(void* tevStr) {
    u32 m = A(this);
    u32 modelData = mpModel->mModelData.v.get();
    Btsw_mDoExt_btpAnm_entry(gabi::ea(&field_0x10), modelData,
                             (u32)field_0x54); /* field_0x10.entry(modelData, field_0x54) */
    u32 env = Btsw_dKy_getEnvlight();          /* g_env_light */
    Btsw_setLightTevColorType(env, mpModel.v.get(),
                              tevStr);             /* setLightTevColorType(mpModel, tevStr) */
    Btsw_mDoExt_modelUpdateDL(mpModel.v.get(), 0); /* mDoExt_modelUpdateDL(mpModel) */
    gabi::object_at<BtswModelDataWork>(modelData).mpTexPatternAnimation =
        0; /* field_0x10.remove(modelData) */
}

/* 0221949C */
void SwCam2_c::Move() {
    u32 c = A(this);
    if (field_0x1E == 0)
        return;
    /* camera_process_class* camera = dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)) */
    s32 camId = dComIfGp_getPlayerCameraID(0);
    u32 play = dComIfGp_ea();
    s8 aimY = field_0x1D;
    s8 aimX = field_0x1C;
    u32 camera = gabi::object_at<dComIfG_camera_info_class>(play + 0x5AF8 + u32(camId * 0x34))
                     .mpCamera.get();
    /* cXyz center = camera_center_data[field_0x1D][field_0x1C + 1] */
    auto& targetCenter = camera_center_data.entry(aimY, s32(aimX) + 1);
    cXyz center;
    f32 cy = targetCenter.y, cx = targetCenter.x, cz = targetCenter.z;
    center.x = cx;
    center.z = cz;
    center.y = cy;
    Btsw_cLib_addCalcPos(c, &center, 0.25f, 10.0f,
                         1.0f);      /* cLib_addCalcPos(&field_0x00, center, ...) */
    Btsw_cameraStay(camera + 0x248); /* camera->mCamera.Stay() */
    cXyz eye;
    cXyz ctr;
    f32 e1 = gabi::object_at<cXyz>(c).y.get(), c0 = field_0x0C.x.get();
    eye.y = e1;
    f32 c1 = field_0x0C.y.get();
    ctr.x = c0;
    f32 e0 = field_0x00.x.get();
    ctr.y = c1;
    eye.x = e0;
    f32 c2 = field_0x0C.z.get(), e2 = gabi::object_at<cXyz>(c).z.get();
    f32 fovy = field_0x18;
    eye.z = e2;
    ctr.z = c2;
    Btsw_dCamera_c_Set(camera + 0x248, &eye, &ctr, fovy,
                       0); /* camera->mCamera.Set(*getEyeP(), *getCenterP(), field_0x18, 0) */
}

/* 022190B0 */
static cPhs_State daNpc_Btsw_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Btsw_c*)i_this)->_create();
}

/* 02219148 */
static BOOL daNpc_Btsw_Delete(daNpc_Btsw_c* i_this) { return i_this->_delete(); }

/* 022197C4 */
static BOOL daNpc_Btsw_Execute(daNpc_Btsw_c* i_this) { return i_this->_execute(); }

/* 022199F8 */
static BOOL daNpc_Btsw_Draw(daNpc_Btsw_c* i_this) { return i_this->_draw(); }

/* 0221C2E8 */
static BOOL daNpc_Btsw_IsDelete(daNpc_Btsw_c* i_this) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

// HD: The allocating constructor initializes the extended letter light and preserves camera word-copy order.
/* 022186EC */
static daNpc_Btsw_c* daNpc_Btsw_c_ct(daNpc_Btsw_c* i_this) {
    u32 a = A(i_this);
    if (a == 0) {
        a = Btsw_allocate(0xD70);
        if (a == 0)
            return nullptr;
    }
    Btsw_fopNpc_construct(a); /* fopNpc_npc_c::fopNpc_npc_c */
    gabi::object_at<daNpc_Btsw_c>(a).__vtbl = BTSW_VTBL;
    /* dKy_tevstr_c constructor (HD): three copies of a 0x44-byte default block at +0, +0xC0, +0x144 */
    auto& actor = gabi::object_at<daNpc_Btsw_c>(a);
    actor.field_0x6D4.copyDefaults(gabi::object_at<BtswLightDefaultBlock>(0x1016E414));
    Btsw_btpConstruct(
        gabi::ea(&gabi::object_at<daNpc_Btsw_c>(a).field_0x7E8)); /* mDoExt_btpAnm constructors */
    Btsw_btpConstruct(gabi::ea(&actor.mSwMail0.field_0x10));
    Btsw_btpConstruct(gabi::ea(&actor.mSwMail1.field_0x10));
    Btsw_btpConstruct(gabi::ea(&actor.mSwMail2.field_0x10));
    /* SwCam2_c() */
    actor.mSwCam.field_0x1E = 0;
    actor.mSwCam.field_0x1C = 0;
    actor.mSwCam.field_0x1D = 0;
    actor.mSwCam.field_0x00.copy(
        camera_center_data.mPositions[0][1]); /* camera_center_data[0][1] */
    actor.mSwCam.field_0x0C.copy(camera_eye); /* camera_eye */
    actor.mSwCam.field_0x18 = 58.0f;
    /* STControl() */
    actor.field_0x978.__vtbl = 0x10050788;
    Btsw_STControl_setWaitParm(gabi::ea(&gabi::object_at<daNpc_Btsw_c>(a).field_0x978), 0xF, 0xF, 0,
                               0, 0, 0x2000, 0.9f, 0.5f);                         /* setWaitParm */
    Btsw_STControl_init(gabi::ea(&gabi::object_at<daNpc_Btsw_c>(a).field_0x978)); /* init */
    return gabi::at<daNpc_Btsw_c>(a);
}

/* 0221C00C */
static void* daNpc_Btsw_HIO_ct(void* obj) {
    u32 h = gabi::ea(obj);
    if (h == 0) {
        h = Btsw_allocate(0x60);
        if (h == 0)
            return nullptr;
    }
    auto& params = gabi::object_at<daNpc_Btsw_HIO_c>(h);
    params.__vtbl = 0x10018708;     /* vtable */
    Btsw_dNpc_HIO_construct(h + 4); /* dNpc_HIO_c::dNpc_HIO_c */
    /* mNpc */
    params.mNpc.m04 = 0.0f;       /* m04 */
    params.mNpc.mMaxHeadX = 8000; /* mMaxHeadX */
    params.mNpc.mMaxBackboneX = 0;
    params.mNpc.mMaxHeadY = 2000;
    params.mNpc.mMaxBackboneY = 8000;
    params.mNpc.mMinHeadX = -3000;
    params.mNpc.mMinBackboneX = 0;
    params.mNpc.mMinHeadY = -2000;
    params.mNpc.mMinBackboneY = -8000;
    params.mNpc.mMaxTurnStep = 2000;     /* mMaxTurnStep */
    params.mNpc.mMaxHeadTurnVel = 0x640; /* mMaxHeadTurnVel */
    params.mNpc.mAttnYOffset = 40.0f;    /* mAttnYOffset */
    params.mNpc.mMaxAttnAngleY = 0x2000; /* mMaxAttnAngleY */
    params.mNpc.m22 = 0;                 /* m22 */
    params.mNpc.mMaxAttnDistXZ = 300.0f; /* mMaxAttnDistXZ */
    params.field_0x30 = 0x1E;
    params.field_0x32 = 0xF;
    params.field_0x34 = 0x3C;
    params.r_1 = 192; /* r_1, g_1, b_1, r_2, g_2, b_2 */
    params.g_1 = 174;
    params.b_1 = 192;
    params.r_2 = 192;
    params.g_2 = 174;
    params.b_2 = 192;
    params.field_0x44 = 40.0f;
    params.field_0x48 = 80.0f;
    params.field_0x4C = 50.0f;
    params.field_0x50 = 0.8f;
    params.field_0x54 = 0.75f;
    params.field_0x58 = 0;
    params.field_0x5C = 10.0f;
    params.mNo = -1; /* mNo */
    return gabi::at<void>(h);
}

/* 0221C158 */
static void __sinit_d_a_npc_btsw_cpp() {
    l_zeroVectorWord3 = 0;
    l_zeroVectorWord2 = 0;
    l_zeroVectorWord1 = 0;
    l_zeroVectorWord0 = 0;
    Btsw_register_global_object(0x101BD768u); /* __register_global_object */
    l_piHigh = l_piInitialHigh.get();         /* -pi */
    l_piLow = l_piInitialLow.get();           /* pi */
    Btsw_cXyzConstruct(0x1046692Cu);
    Btsw_register_global_object(0x101BD774u);
    Btsw_csXyzConstruct(0x1046692Du);
    Btsw_register_global_object(0x101BD780u);
    l_largeFloatFour = 10000.0f;
    l_largeFloatOne = 50000.0f;
    l_largeFloatTwo = 50000.0f;
    l_largeFloatThree = 10000.0f;
    Btsw_daNpc_Btsw_HIO_c_daNpc_Btsw_HIO_c(BTSW_HIO); /* l_HIO */
    /* SwCam2_c::camera_center_data[2][3], SwCam2_c::camera_eye */
    const f32 x[3] = {-104.0f, -139.0f, -173.0f};
    const f32 z[2][3] = {{875.0f, 882.0f, 875.0f}, {868.0f, 875.0f, 868.0f}};
    for (u32 r = 0; r < 2; r++)
        for (u32 c = 0; c < 3; c++) {
            auto& position = camera_center_data.mPositions[r][c];
            position.x = x[c];
            position.y = r == 0 ? 805.0f : 825.0f;
            position.z = z[r][c];
        }
    l_cameraEyeX = -139.0f;
    l_cameraEyeY = 815.0f;
    l_cameraEyeZ = 662.0f;
}

/* 0221C2D4 */
static void SafeString_dtor(void* self, int flags) {
    if (self != nullptr && (flags & 1))
        Btsw_dl(self); /* __dl */
}

/* 0221C2F0 */
static void daNpc_Btsw_dtor(daNpc_Btsw_c* i_this, int flags) {
    u32 a = A(i_this);
    if (a == 0)
        return;
    Btsw_STControl_destroy(gabi::ea(&i_this->field_0x978), 2); /* STControl::~STControl */
    Btsw_dCcD_Cyl_destroy(gabi::ea(&i_this->mCyl), 2);         /* dCcD_Cyl::~dCcD_Cyl */
    Btsw_dCcD_Stts_destroy(gabi::ea(&i_this->mStts), 2);       /* dCcD_Stts::~dCcD_Stts */
    Btsw_cM3dGCir_destroy(a + 0x628, 2); /* dBgS_AcchCir: cM3dGCir::~cM3dGCir */
    gabi::object_at<BtswAcchVtables>(gabi::ea(&i_this->mObjAcch)).mVtableB =
        0x100186E8; /* dBgS_ObjAcch vtables */
    gabi::object_at<BtswAcchVtables>(gabi::ea(&i_this->mObjAcch)).mVtableA = 0x100186F8;
    Btsw_dBgS_Acch_destroy(gabi::ea(&i_this->mObjAcch), 0); /* dBgS_Acch::~dBgS_Acch */
    Btsw_fopAc_ac_c_fopAc_ac_c(a, 0);                       /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        Btsw_dl(a); /* __dl */
}

/* 0221C398 */
static void SafeString_assureTerminated(void* self) {}

// HD: these unit-local bindings retain the verified register argument/result widths.
/* WWHD glue
str 10018754 "Btsw" in 02217C94
str 1001875C "m_head_tex_pattern != (0)" in 02217C94
str 10018778 "d_a_npc_btsw.cpp" in 02217C94
str 100187B0 "Btsw" in 022181EC
str 100187C0 "handL" in 022182F4
str 100187C8 "handR" in 022182F4
str 100187D0 "Btsw" in 022182F4
str 100187D8 "head" in 022182F4
str 100187E0 "d_a_npc_btsw.cpp" in 022182F4
str 100187F4 "modelData != (0)" in 022182F4
str 10018808 "m_jnt.getHeadJntNum() >= 0" in 022182F4
str 10018824 "backbone" in 022182F4
str 10018830 "m_jnt.getBackboneJntNum() >= 0" in 022182F4
str 10018890 "Btsw" in 02218D58
str 10018898 "Btsw" in 02218FA4
str 100188B0 "Btsw" in 022190BC
str 100188C0 "GETMOTHERLETTER" in 022195A4
str 10018900 "Btsw" in 0221A3CC
str 1001894C "Btsw" in 0221B3A8
str 10018954 "BTSW_TALK_SUGOI" in 0221B3A8
str 10018964 "SHIWAKEGAME" in 0221B3A8
str 10018970 "GETMOTHERLETTER" in 0221BE68
data l_btp_ix_tbl 100186BC
data l_bck_ix_tbl 10018718
data a_play_mode_tbl 101BD6E4
data a_morf_frame_tbl 101BD710
data a_play_speed_tbl 101BD73C
ext 0200EE00 f32 Btsw_cLib_addCalcPos(u32 a0, cXyz * a1, f32 a2, f32 a3, f32 a4)
ext 0200EE00 f32 Btsw_cLib_addCalcPos(u32 a0, u32 a1, f32 a2, f32 a3, f32 a4)
ext 0200F164 void Btsw_cLib_addCalcPos2(u32 a0, cXyz * a1, f32 a2, f32 a3)
ext 0200F428 void Btsw_cLib_addCalcAngleS2(u32 a0, s32 a1, s32 a2, s32 a3)
ext 0200F93C s16 Btsw_cLib_targetAngleY(u32 a0, u32 a1)
ext 0200F974 s16 Btsw_cLib_targetAngleX(u32 a0, u32 a1)
ext 020195B0 s16 Btsw_cM_atan2s(f32 a0, f32 a1)
ext 020198D8 f32 Btsw_cM_rndF(f32 a0)
ext 0201ADE0 void Btsw_cXyz___mi(u32 a0, cXyz * a1, u32 a2)
ext 0201AE48 void Btsw_cXyz___ml(cXyz * a0, cXyz * a1, f32 a2)
ext 0201B47C s32 Btsw_cXyz_normalizeRS(cXyz * a0)
ext 0221949C void Btsw_SwCam2_c_Move(u32 a0)
ext 0221A49C u8 Btsw_daNpc_Btsw_c_chkAttention(u32 a0, cXyz * a1, s32 a2)
ext 0221C00C void Btsw_daNpc_Btsw_HIO_c_daNpc_Btsw_HIO_c(u32 a0)
ext 024EEEB8 u32 Btsw_dBgS_GetPolyColor(u32 a0, u32 a1)
ext 024EF130 u32 Btsw_dBgS_GetRoomId(u32 a0, u32 a1)
ext 024EFF44 void Btsw_dBgS_AcchCir_SetWall(u32 a0, f32 a1, f32 a2)
ext 024F06B4 void Btsw_dBgS_Acch_Set(u32 a0, u32 a1, u32 a2, u32 a3, s32 a4, u32 a5, u32 a6, u32 a7, u32 a8)
ext 024F08A8 void Btsw_dBgS_Acch_CrrPos(u32 a0, u32 a1)
ext 02514F88 void Btsw_dCamera_c_Set(u32 a0, cXyz * a1, cXyz * a2, f32 a3, s32 a4)
ext 02515A70 void Btsw_dCcD_Cyl_destroy(u32 a0, s32 a1)
ext 02515F14 void Btsw_dCcD_Stts_Init(u32 a0, s32 a1, s32 a2, u32 a3)
ext 02516518 void Btsw_dCcD_Cyl_Set(u32 a0, u32 a1)
ext 02520460 s32 Btsw_dComIfG_resLoad(u32 a0, u32 a1)
ext 025204C8 void Btsw_dComIfG_resDelete(u32 a0, u32 a1)
ext 02542D88 u32 Btsw_dEvent_manager_c_getMyStaffId(u32 a0, u32 a1, s32 a2, s32 a3)
ext 02543280 void Btsw_dEvent_manager_c_cutEnd(u32 a0, u32 a1)
ext 02543F10 u32 Btsw_dEvent_manager_c_getEventIdx(u32 a0, u32 a1, s32 a2)
ext 025440C8 u32 Btsw_dEvent_manager_c_endCheck(u32 a0, s32 a1)
ext 0254457C u32 Btsw_dEvent_manager_c_endCheckOld(u32 a0, u32 a1)
ext 02544950 u32 Btsw_dEvent_manager_c_ChkPresentEnd(u32 a0)
ext 02555D0C u32 Btsw_dKy_getEnvlight()
ext 0255FFF4 void Btsw_dKy_tevstr_init(u32 a0, s32 a1, s32 a2)
ext 025626A4 void Btsw_dScnKy_env_light_c_settingTevStruct(u32 a0, s32 a1, u32 a2, u32 a3)
ext 02586D24 u32 Btsw_dLetter_isNoSend(s32 a0)
ext 025885C4 void Btsw_STControl_setWaitParm(u32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, f32 a7, f32 a8)
ext 025885E8 void Btsw_STControl_init(u32 a0)
ext 0259D24C void Btsw_dNpc_setAnmIDRes(u32 a0, u32 a1, f32 a2, f32 a3, u32 a4, s32 a5, u32 a6)
ext 0259D54C void Btsw_dNpc_playerEyePos(cXyz * a0, f32 a1)
ext 0259DED0 void Btsw_dNpc_JntCtrl_c_lookAtTarget(u32 a0, u32 a1, u32 a2, cXyz * a3, s32 a4, s32 a5, s32 a6)
ext 0259E08C void Btsw_dNpc_JntCtrl_c_setParam(u32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9)
ext 0259F814 void Btsw_dNpc_EventCut_c_setActorInfo2(u32 a0, u32 a1, u32 a2)
ext 0259F858 u32 Btsw_dNpc_EventCut_c_cutProc(u32 a0)
ext 025A11EC u32 Btsw_fopNpc_npc_c_talk(u32 a0, s32 a1)
ext 025A1458 void Btsw_fopNpc_construct(u32 a0)
ext 025A15AC void Btsw_fopNpc_npc_c_setCollision(u32 a0, f32 a1, f32 a2)
ext 025B7D90 u32 Btsw_dSv_player_collect_c_isSymbol(u32 a0, s32 a1)
ext 025B8AF4 void Btsw_dSv_event_c_setEventReg(u32 a0, s32 a1, s32 a2)
ext 025B8BB0 u32 Btsw_dSv_event_c_getEventReg(u32 a0, s32 a1)
ext 025BBD7C u32 Btsw_ShopCursor_create(u32 a0, u32 a1, f32 a2)
ext 025BD290 void Btsw_ShopCursor_c_anm_play(u32 a0)
ext 025BD338 void Btsw_ShopCursor_c_draw(u32 a0)
ext 025BEBB8 void Btsw_dSnap_RegistFig(s32 a0, u32 a1, u32 a2, s32 a3, f32 a4, f32 a5, f32 a6)
ext 025C58A8 void Btsw_dTimer_c_end(u32 a0, s32 a1)
ext 025C58D8 void Btsw_dTimer_c_deleteRequest(u32 a0)
ext 025C6250 void Btsw_dTimer_c_start(u32 a0, s32 a1)
ext 025CB374 void Btsw_dVibration_c_StartShock(u32 a0, s32 a1, s32 a2, cXyz * a3)
ext 025D50BC void Btsw_fopAc_ac_c_fopAc_ac_c(u32 a0, s32 a1)
ext 025D63E8 s32 Btsw_fopAcM_entrySolidHeap(u32 a0, u32 a1, s32 a2)
ext 025D6870 void Btsw_fopAcM_posMoveF(u32 a0, u32 a1)
ext 025D76A8 void Btsw_fopAcM_orderSpeakEvent(daNpc_Btsw_c * a0)
ext 025D77DC void Btsw_fopAcM_orderOtherEvent2(daNpc_Btsw_c * a0, u32 a1, s32 a2, s32 a3)
ext 025D77DC void Btsw_fopAcM_orderOtherEvent2(u32 a0, u32 a1, s32 a2, s32 a3)
ext 025D7A58 void Btsw_fopAcM_orderOtherEventId(daNpc_Btsw_c * a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
ext 025E1B34 void Btsw_JAIZelBasic_seDeleteObject(u32 a0)
ext 025E2DE0 void Btsw_mDoExt_modelUpdateDL(u32 a0, s32 a1)
ext 025E38E0 u32 Btsw_mDoExt_J3DModel__create(u32 a0, s32 a1, s32 a2)
ext 025E4F64 u32 Btsw_mDoExt_McaMorf_mDoExt_McaMorf(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, f32 a12)
ext 025E535C void Btsw_mDoExt_McaMorf_play(u32 a0, u32 a1, s32 a2, s32 a3)
ext 025E5590 void Btsw_mDoExt_McaMorf_entryDL(u32 a0)
ext 025E55A0 void Btsw_mDoExt_McaMorf_calc(u32 a0)
ext 025E563C void Btsw_mDoExt_McaMorf_stopZelAnime(u32 a0)
ext 025E7B3C void Btsw_mDoExt_btpAnm_entry(u32 a0, u32 a1, u32 a2)
ext 025F0A10 u8 Btsw_mDoHIO_subRoot_c_createChild(u32 a0, u32 a1)
ext 025F0A18 void Btsw_mDoHIO_subRoot_c_deleteChild(s32 a0)
ext 025F19F8 void Btsw_mDoMtx_XYZrotM(u32 a0, s32 a1, s32 a2, s32 a3)
ext 025F1AA4 void Btsw_mDoMtx_ZXYrotS(u32 a0, s32 a1, s32 a2, s32 a3)
ext 025F1B48 void Btsw_mDoMtx_ZXYrotM(u32 a0, s32 a1, s32 a2, s32 a3)
ext 025F1BF4 void Btsw_mDoMtx_XrotM(u32 a0, s32 a1)
ext 025F1C28 void Btsw_mDoMtx_YrotM(u32 a0, s32 a1)
ext 025F1C5C void Btsw_mDoMtx_ZrotM(u32 a0, s32 a1)
ext 025F24E0 void Btsw_mDoMtx_stack_c_transM(f32 a0, f32 a1, f32 a2)
ext 026067F4 u32 Btsw_dRes_control_c_getIDRes(u32 a0, SafeString * a1, s32 a2)
ext 0273AA24 void Btsw_JUTAssertion_showAssert(u32 a0, s32 a1, u32 a2)
ext 0273AF40 void Btsw_dl(u32 a0)
ext 0273AF40 void Btsw_dl(void * a0)
ext 027F2BF8 u32 Btsw_J3DFrameCtrl_checkPass(u32 a0, f32 a1)
ext 027F3F94 u32 Btsw_J3DModelData_jointTree(u32 a0)
ext 028E8D88 void Btsw_PSVECAdd(cXyz * a0, cXyz * a1, cXyz * a2)
ext 028E8D88 void Btsw_PSVECAdd(cXyz * a0, u32 a1, cXyz * a2)
ext 028E8D88 void Btsw_PSVECAdd(u32 a0, cXyz * a1, u32 a2)
ext 028E8D88 void Btsw_PSVECAdd(u32 a0, u32 a1, u32 a2)
ext 028E8E64 void Btsw_PSVECScale(u32 a0, u32 a1, f32 a2)
ext 028E8F64 void Btsw_PSMTXMultVec(u32 a0, u32 a1, u32 a2)
ext 028E8F64 void Btsw_PSMTXMultVec(u32 a0, cXyz * a1, cXyz * a2)
ext 028E90D4 void Btsw_PSMTXCopy(u32 a0, Mtx34 * a1)
ext 028E90D4 void Btsw_PSMTXCopy(u32 a0, u32 a1)
ext 028E9108 void Btsw_PSMTXConcat(u32 a0, Mtx34 * a1, u32 a2)
ext 028E93CC void Btsw_PSMTXTrans(u32 a0, f32 a1, f32 a2, f32 a3)
ext 028F026C void Btsw_register_global_object(u32 a0)
ext 02007898 u32 Btsw_padCheckTriggerA(s32 a0)
ext 02018034 void Btsw_cM3dGCir_destroy(u32 a0, s32 a1)
ext 02055B64 u32 Btsw_cLib_calcTimer(u32 a0)
ext 022186EC void Btsw_actorConstruct(u32 a0)
ext 022190B4 void Btsw_SwMail_SeDelete(u32 a0)
ext 024EFD9C void Btsw_dBgS_Acch_destroy(u32 a0, s32 a1)
ext 02514F44 void Btsw_cameraStay(u32 a0)
ext 02515860 void Btsw_dCcD_Stts_destroy(u32 a0, s32 a1)
ext 02562F5C void Btsw_setLightTevColorType(u32 a0, u32 a1, u32 a2)
ext 02562F5C void Btsw_setLightTevColorType(u32 a0, u32 a1, void * a2)
ext 02586E1C u32 Btsw_dLetter_isRead(s32 a0)
ext 025886D8 void Btsw_STControl_destroy(u32 a0, s32 a1)
ext 0259DA18 void Btsw_dNpc_HIO_construct(u32 a0)
ext 025BD31C void Btsw_ShopCursor_setPos(u32 a0, u32 a1)
ext 025C5864 void Btsw_timerStart(u32 a0)
ext 025C60F4 void Btsw_dTimer_createTimer(s32 a0, u32 a1, s32 a2, s32 a3, f32 a4, f32 a5, f32 a6, f32 a7)
ext 025C6278 u32 Btsw_timerIsDelete(u32 a0)
ext 025E1988 void Btsw_seStart(u32 a0)
ext 025E19CC void Btsw_seStartAt(s32 a0, u32 a1)
ext 025E7820 void Btsw_btpConstruct(u32 a0)
ext 025E789C s32 Btsw_btpInit(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, f32 a9)
ext 025E789C s32 Btsw_btpInit(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4, s32 a5, s32 a6, u32 a7, s32 a8, f32 a9)
ext 02618760 void Btsw_padSetRepeat(u32 a0, s32 a1, s32 a2)
ext 02618774 void Btsw_padResetRepeat(u32 a0)
ext 026768E4 void Btsw_endMiniGame(u32 a0)
ext 02676900 void Btsw_startMiniGame(u32 a0)
ext 0273AD10 u32 Btsw_allocate(s32 a0)
ext 027DF9B0 s32 Btsw_jointNameIndex(u32 a0, u32 a1)
ext 027F68FC u32 Btsw_jointNameHeader(u32 a0)
ext 027F7878 u32 Btsw_jointNumber(J3DNode * a0)
ext 028EAB2C void Btsw_csXyzConstruct(u32 a0)
ext 028ED6F8 void Btsw_cXyzConstruct(u32 a0)
ext 028F4384 f32 Btsw_squareRoot(f32 a0)
ext C000A848 void Btsw_copyDebugColor(u32 a0, u32 a1, s32 a2)
*/
