/**
 * d_a_npc_kp1.cpp (WWHD)
 * NPC - Maggie (rich, Windfall)
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx). Its structure is d_a_npc_km1's (verified; much of the code is shared),
 * plus the letter / heart-piece events, messages, and two hand-held models; "differs from Km1"
 * marks the differences.
 */
#include "d/actor/d_a_npc_kp1.h"

// HD: resource-key vtable and per-TU data retain their original guest addresses.
const gc_tu_data gc_tu = {.safeString = 0x1001CDC4};
enum { fpcNm_NPC_KP1_e = 0x163 };
enum Kp1MessageStatus { KP1_MSG_TYPING = 6, KP1_MSG_DISPLAYED = 14, KP1_MSG_DESTROYED = 19 };
enum Kp1State { KP1_WAIT = 1, KP1_TALK = 2 };
enum Kp1Resource {
    KP1_BCK_WAIT = 5,
    KP1_BDL_LETTER = 6,
    KP1_BDL_HEART_PIECE = 7,
    KP1_BDL_BODY = 8
};
enum Kp1Item {
    KP1_ITEM_HEART_PIECE = 7,
    KP1_ITEM_JOYS_PENDANT = 0x45,
    KP1_ITEM_MAGGIES_LETTER = 0x9A,
    KP1_ITEM_MOBLINS_LETTER = 0x9B
};
enum Kp1Order {
    KP1_ORDER_TALK = 1,
    KP1_ORDER_ITEM = 2,
    KP1_ORDER_LETTER = 3,
    KP1_ORDER_HEART_PIECE = 4
};
static daNpc_Kp1_HIO_c& l_HIO = gabi::object_at<daNpc_Kp1_HIO_c>(0x10467908);
static cXyz& a_att_pos_offst = gabi::object_at<cXyz>(0x10467940);
static cXyz& a_eye_pos_offst = gabi::object_at<cXyz>(0x1046794C);
static be<u32>& l_att_guard = gabi::object_at<be<u32>>(0x10467958);
static be<u32>& l_eye_guard = gabi::object_at<be<u32>>(0x1046795C);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static Kp1ActionFunc& l_wait_action = gabi::object_at<Kp1ActionFunc>(0x1001CDB0);
static dCcD_SrcCyl& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static gptr<Kp1MessageManager>& l_message_manager =
    gabi::object_at<gptr<Kp1MessageManager>>(0x101F4B5C);
static be<u32>& l_red_guard = gabi::object_at<be<u32>>(0x101FDA50);
static be<u32>& l_blue_guard = gabi::object_at<be<u32>>(0x101FDAC0);
static be<u32>& l_red_colour = gabi::object_at<be<u32>>(0x101FEBF4);
static be<u32>& l_blue_colour = gabi::object_at<be<u32>>(0x101FEBF8);
static be<u32>& a_red_colour = gabi::object_at<be<u32>>(0x1001CDB8);
static be<u32>& a_blue_colour = gabi::object_at<be<u32>>(0x1001CDBC);
using Kp1PrmBytes = be<u8>[0x1C];
static Kp1PrmBytes& a_prm_tbl = gabi::object_at<Kp1PrmBytes>(0x101BFEDC);
using Kp1AnmIndices = be<s32>[6][2];
static Kp1AnmIndices& a_anm_idx_tbl = gabi::object_at<Kp1AnmIndices>(0x1001CFEC);
using Kp1BtpIndices = be<s32>[2];
static Kp1BtpIndices& a_btp_arc_ix_tbl = gabi::object_at<Kp1BtpIndices>(0x1001CE34);
using Kp1AnmParams = daNpc_Kp1_c::anm_prm_c[6];
using Kp1AttrParams = daNpc_Kp1_c::anm_prm_c[7];
static Kp1AnmParams& a_anm_prm_tbl = gabi::object_at<Kp1AnmParams>(0x101BFDDC);
static Kp1AnmParams& a_anm_state_tbl = gabi::object_at<Kp1AnmParams>(0x101BFE3C);
static Kp1AttrParams& a_anm_attr_tbl = gabi::object_at<Kp1AttrParams>(0x101BFE6C);
using Kp1EventNames = gptr<char>[5];
static Kp1EventNames& l_evn_tbl = gabi::object_at<Kp1EventNames>(0x101BFDC8);
static gptr<char>& cut_name_tbl = gabi::object_at<gptr<char>>(0x101BFDD0);
static be<u32>& a_heap_size = gabi::object_at<be<u32>>(0x101BFDCC);
static be<s8>& a_tex_pattern_num = gabi::object_at<be<s8>>(0x101BFDC8);

/* 0227D61C */
static daNpc_Kp1_HIO_c* daNpc_Kp1_HIO_c_ct(daNpc_Kp1_HIO_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daNpc_Kp1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001CDFC;
    memcpy_g(&i_this->mPrmTbl, &a_prm_tbl, 0x1C); /* a_prm_tbl */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 0227AEE4 */
static BOOL nodeCallBack_Kp(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Kp1_c* kp1Actor = static_cast<daNpc_Kp1_c*>(model->getUserArea());
        if (kp1Actor) {
            // HD: preserve guarded initialization and its store order.

            if ((u32)l_att_guard == 0) {
                a_att_pos_offst.x = 0.0f;
                a_att_pos_offst.z = 0.0f;
                l_att_guard = 1;
                a_att_pos_offst.y = 0.0f;
            }
            // HD: eye offset differs from Km1.

            if ((u32)l_eye_guard == 0) {
                l_eye_guard = 1;
                a_eye_pos_offst.x = 20.0f;
                a_eye_pos_offst.y = -20.0f;
                a_eye_pos_offst.z = 0.0f;
            }
            u32 jointIdx = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(model->getAnmMtx(jointIdx), stk);
            if (jointIdx == (u32)(s32)kp1Actor->m_head_jnt_num) {
                PSMTXMultVec(stk, &a_att_pos_offst, &kp1Actor->mAttPos);
                // HD: compared with Km1, Y instead of X, both angles negated
                mDoMtx_YrotM(stk, (s16)-kp1Actor->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stk, (s16)-kp1Actor->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stk, &a_eye_pos_offst, &kp1Actor->mEyePos);
            } else if (jointIdx == (u32)(s32)kp1Actor->m_backbone_jnt_num) {
                mDoMtx_XrotM(stk, kp1Actor->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stk,
                             (s16)-kp1Actor->m_jnt.mAngles[1][0]); // HD: compared with Km1, negated
            }
            PSMTXCopy(stk, J3DSys::mCurrentMtx);
            mtx_copy(model->getAnmMtx(jointIdx), stk);
        }
    }
    return TRUE;
}

/* 0227BC14 */
bool daNpc_Kp1_c::createInit() {

    if (!dComIfGs_isEventBit(0x2D01)) {
        return false;
    }
    mEventCut.setActorInfo2("Kp1", (fopNpc_npc_c*)(void*)this);
    gravity = -4.0f;
    field_0x798.copy(current.pos);
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB; /* attention_info.distances[TALK] */
    field_0x7B6 = 0xFF;
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB; /* attention_info.distances[SPEAK] */
    gabi::Local<Kp1ActionFunc> pmf;
    Kp1_loadAction(pmf, &l_wait_action);
    set_action(pmf, nullptr);
    shape_angle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    field_0x7BC = 1;
    setMtx();
    return true;
}

/* 0227B9AC */
void daNpc_Kp1_c::setMtx() {

    if (field_0x7C7 == 0) {
        playTexPatternAnm();
        field_0x7B4 = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < field_0x7A4) {
            field_0x7B4 = 1;
        }
        field_0x7A4 = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = Kp1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Kp1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    // HD: compared with Km1, the two hand-held models follow the left hand
    PSMTXCopy(mpMorf->getModel()->getAnmMtx(m_hand_jnt_num), mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpItemModel[0], mDoMtx_stack_c::get());
    J3DModel_calc(mpItemModel[0]);
    J3DModel_setBaseTRMtx(mpItemModel[1], mDoMtx_stack_c::get());
    J3DModel_calc(mpItemModel[1]);
    setAttention();
}

/* 0227CAB4 */
bool daNpc_Kp1_c::anmResID(int i_num, be<s32>* o_bck_num, be<s32>* o_bas_num) {

    // Animation/resource pairs follow the original table.
    if ((u32)i_num >= 6) /* JUT_ASSERT(301, 0 <= i_num && i_num < ANM_END) */
        JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x12D, "0 <= i_num && i_num < ANM_END");
    if (o_bck_num == nullptr || o_bas_num == nullptr) /* JUT_ASSERT(302, o_bck_num && o_bas_num) */
        JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x12E, "o_bck_num && o_bas_num");
    *o_bck_num = a_anm_idx_tbl[i_num][0];
    *o_bas_num = a_anm_idx_tbl[i_num][1];
    return true;
}

/* 0227B168 */
void daNpc_Kp1_c::BtpNum2ResID(int i_num, be<s32>* o_btp_num) {

    if ((u32)i_num >= 2) /* JUT_ASSERT(324, 0 <= i_num && i_num < TEXPATTERN_END) */
        JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x144, "0 <= i_num && i_num < TEXPATTERN_END");
    *o_btp_num = (s32)a_btp_arc_ix_tbl[i_num]; /* a_btp_arc_ix_tbl[i_num] */
}

/* 0227CB64 */
u32 daNpc_Kp1_c::setAnm_tex(s8 i_param_1) {

    if (i_param_1 >= 0 && i_param_1 != field_0x7CD) {
        field_0x7CD = i_param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); // HD: unchanged-animation path returns the original this register.
}

/* 0227B1D0 */
bool daNpc_Kp1_c::init_btp(u32 param_1, int param_2) {

    J3DModelData* modelData = mpMorf->getModel()->getModelData();
    if (param_2 >= 0) {
        gabi::Local<be<s32>> btpId;
        BtpNum2ResID(param_2, btpId);
        m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes("Kp", *btpId);
        if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(357, m_head_tex_pattern != NULL) */
            JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x165, "m_head_tex_pattern != (0)");
        if (Kp1_btpInit(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0,
                        -1, param_1, FALSE) == 0) {
            return false;
        }
        mBtpFrame = 0;
        field_0x6F2 = 0;
        if (field_0x7CD == 1) { // HD: compared with Km1, pattern 1 starts at frame 1
            mBtpFrame = 1;
        }
    }
    return true;
}

/* 0227B2FC */
bool daNpc_Kp1_c::initTexPatternAnm(u32 param_1) {

    bool initialized = false;
    if (init_btp(param_1, field_0x7CD)) {
        initialized = true;
    }
    return initialized;
}

/* 0227B854 */
void daNpc_Kp1_c::playTexPatternAnm() {

    s8 tex = field_0x7CD;
    if (tex == 1) { // HD: compared with Km1, pattern 1 is not animated
        return;
    }
    if (tex == 0 && Kp1_calcTimer(&field_0x6F2) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < Kp1_frameMax(m_head_tex_pattern)) {
        return;
    }
    if (field_0x7CD) {
        mBtpFrame = (u8)Kp1_frameMax(m_head_tex_pattern);
    } else {
        mBtpFrame = 0;
        field_0x6F2 = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
    }
}

/* 0227CB88 */
s32 daNpc_Kp1_c::setAnm_anm(anm_prm_c* i_anm_ptr) {

    s32 changed = 0;
    s8 num = i_anm_ptr->bckNum;
    if (num >= 0 && field_0x7CE != num) {
        field_0x7CE = num;
        if (mpMorf.get() != nullptr) {
            gabi::Local<be<s32>> bckId;
            gabi::Local<be<s32>> basId;
            anmResID(num, bckId, basId);
            if (*bckId >= 0) {
                dNpc_setAnmIDRes(mpMorf, i_anm_ptr->loopMode, i_anm_ptr->morf, i_anm_ptr->speed,
                                 *bckId, *basId, "Kp");
            }
            changed = 1;
        }
        field_0x7B5 = 0;
        field_0x7A4 = 0.0f;
        field_0x7B4 = 0;
    }
    return changed;
}

/* 0227CC44 */
void daNpc_Kp1_c::setAnm_NUM(int i_idx, int i_tex) {

    anm_prm_c* tbl = a_anm_prm_tbl; /* a_anm_prm_tbl */
    if (i_tex != 0) {
        setAnm_tex(tbl[i_idx].btpNum);
    }
    setAnm_anm(&tbl[i_idx]);
}

/* 0227CCB0 */
void daNpc_Kp1_c::setAnm() {

    anm_prm_c* a_anm_prm_tbl = a_anm_state_tbl;
    setAnm_tex(a_anm_prm_tbl[field_0x7D0].btpNum);
    setAnm_anm(&a_anm_prm_tbl[field_0x7D0]);
}

/* 0227CD8C */
void daNpc_Kp1_c::chngAnmAtr(u32 param_1) {

    if (param_1 >= 7 || param_1 == field_0x7CB) {
        return;
    }
    /* which hand-held model _draw shows */
    if (param_1 == 2) {
        mItemModelIndex = 2;
    } else if (param_1 == 3) {
        mItemModelIndex = 1;
    } else {
        mItemModelIndex = 0;
    }
    field_0x7CB = param_1;
    setAnm_ATR(1);
}

/* 0227CD1C */
void daNpc_Kp1_c::setAnm_ATR(int param_1) {

    anm_prm_c* a_anm_prm_tbl = a_anm_attr_tbl; /* [7] */
    if (param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[field_0x7CB].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[field_0x7CB]);
}

/* 0227CDE4 */
void daNpc_Kp1_c::ctrlAnmAtr() {

    switch (field_0x7CB) {
    case 1:
        if (field_0x7B4 == 0)
            return;
        field_0x7B5 = field_0x7B5 + 1;
        if (field_0x7B5 < 2)
            return;
        break;
    case 6:
        if (field_0x7B4 == 0)
            return;
        field_0x7B5 = field_0x7B5 + 1;
        if (field_0x7B5 <= 0)
            return;
        break;
    default:
        return;
    }
    field_0x7CB = 0;
    setAnm_NUM(0, 1);
}

/* 0227CE54 */
void daNpc_Kp1_c::anmAtr(u16 i_msgStatus) {

    if (i_msgStatus == KP1_MSG_TYPING) {
        if (field_0x7D6 == 0) {
            field_0x7CC = 0xFF;
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            field_0x7D6 = field_0x7D6 + 1;
        }
        u8 uVar1 = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        if (uVar1 != 0xFF && uVar1 != field_0x7CC) {
            dComIfGp_clearMesgAnimeTagInfo();
            field_0x7CC = uVar1;
            /* chngAnmTag(): empty */
        }
    } else if (i_msgStatus == KP1_MSG_DISPLAYED) {
        field_0x7D6 = 0;
    }
    ctrlAnmAtr();
    /* ctrlAnmTag(): empty */
}

/* 0227CF10 */
void daNpc_Kp1_c::setStt(s8 param_1) {

    s8 uVar1 = field_0x7D0;
    field_0x7D0 = param_1;
    mItemModelIndex = 0;
    if (param_1 == 2) {
        field_0x7D1 = uVar1;
        mCancelPresent = 0;
        field_0x7D2 = 1;
        field_0x7CB = 0xFF;
        m_jnt.mbTrn = 1; /* setTrn() */
        return;
    }
    setAnm();
}

/* 0227CF50 */
u16 daNpc_Kp1_c::next_msgStatus(be<u32>* pMsgNo) {

    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e;
    Kp1MessageManager* msgMng = l_message_manager;
    switch ((u32)*pMsgNo) {
    case 0x1E84:
        *pMsgNo = 0x1E85;
        break;
    case 0x1E85:
        *pMsgNo = 0x1E86;
        break;
    case 0x1E88:
        *pMsgNo = 0x1E89;
        break;
    case 0x1E8A:
        *pMsgNo = 0x1E8B;
        break;
    case 0x1E8B:
        *pMsgNo = 0x1E8C;
        break;
    case 0x1E8C:
        *pMsgNo = 0x1E8D;
        break;
    case 0x1E8D:
        *pMsgNo = 0x1E8E;
        break;
    case 0x1E8E:
        *pMsgNo = 0x1E8F;
        break;
    case 0x1E8F: {
        u32 select = (u32)msgMng->mSelectedAnswer; /* the answer chosen */
        if (select == 0) {
            *pMsgNo = 0x1E91;
        } else if (select == 1) {
            *pMsgNo = 0x1E90;
        }
        break;
    }
    case 0x1E90:
        mLetterOffered = 1;
        msgStatus = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    case 0x1E91:
        field_0x7CF = 3;
        msgStatus = fopMsgStts_MSG_ENDS_e;
        break;
    case 0x1E96:
        *pMsgNo = 0x1E97;
        mCancelPresent = 1;
        break;
    case 0x1E97:
        *pMsgNo = 0x1E98;
        break;
    case 0x1E98:
        *pMsgNo = 0x1E99;
        break;
    case 0x1E99:
        *pMsgNo = 0x1E9A;
        break;
    case 0x1E9A:
        *pMsgNo = 0x1E9B;
        break;
    case 0x1E9B:
        *pMsgNo = 0x1E9C;
        break;
    case 0x1E9C:
        *pMsgNo = 0x1E9D;
        break;
    case 0x1E9D:
        field_0x7CF = 4;
        msgStatus = fopMsgStts_MSG_ENDS_e;
        break;
    default:
        msgStatus = fopMsgStts_MSG_ENDS_e;
        break;
    }
    return msgStatus;
}

/* 0227D104 */
u32 daNpc_Kp1_c::getMsg() {

    u8 item = field_0x7B6;
    if (item != 0xFF) { /* an item was shown */
        if (item == KP1_ITEM_MOBLINS_LETTER) {
            return 0x1E96;
        }
        if (item == KP1_ITEM_JOYS_PENDANT) {
            return 0x1EA0;
        }
        return 0x1E9F;
    }
    if (mLetterEventEnded) {
        mLetterEventEnded = 0;
        return 0x1E92;
    }
    if (mHeartPieceEventEnded) {
        mHeartPieceEventEnded = 0;
        return 0x1E9E;
    }
    if (dComIfGs_isGetItemReserve(0xF) && !dComIfGs_checkReserveItem(KP1_ITEM_MOBLINS_LETTER)) {
        if (dComIfGs_getEventReg(0xCCFF) == 0) {
            return 0x1E83;
        }
        return 0x1E84;
    }
    if (dComIfGs_isGetItemReserve(0xE)) {
        if (dComIfGs_isGetItemBeast(0)) {
            return 0x1E87;
        }
        return 0x1E88;
    }
    if (mLetterOffered) {
        return 0x1E93;
    }
    return 0x1E8A;
}

/* 0227C714 */
void daNpc_Kp1_c::eventOrder() {

    s8 order = field_0x7CF;
    if (order == 1 || order == 2) {
        eventInfo.onCondition(
            dEvtCnd_CANTALK_e |
            dEvtCnd_CANTALKITEM_e); /* eventInfo.onCondition(dEvtCnd_CANTALK_e | CANTALKITEM) */
        if (field_0x7CF == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {
        // Event names for the letter and heart-piece orders.
        Kp1_orderOtherEvent(this, l_evn_tbl[order].get(), 1, 0xFFFF);
    }
}

/* 0227BF00 */
void daNpc_Kp1_c::checkOrder() {

    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == dEvtCmd_INDEMO_e) {
        if ((Kp1_startEvent(dComIfGp_getPEvtManager(), "GET_MAGYS_LTTR" /* "GET_MAGYS_LTTR" */) &&
             field_0x7CF == 3) ||
            (Kp1_startEvent(dComIfGp_getPEvtManager(), "GET_KAKERA_HRT" /* "GET_KAKERA_HRT" */) &&
             field_0x7CF == 4)) {
            field_0x7CF = 0;
        }
    } else if (command == dEvtCmd_INTALK_e) {
        if (field_0x7CF == 1 || field_0x7CF == 2) {
            field_0x7CF = 0;
            field_0x7C5 = 1;
        }
    }
}

/* 0227C41C */
void daNpc_Kp1_c::lookBack() {

    gabi::Local<cXyz> vec1;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    vec1->z = 0.0f;
    vec1->x = 0.0f;
    u8 headOnlyFollow = mHeadOnlyFollow;
    f32 srcX = current.pos.x;
    vec1->y = 0.0f;
    s16 targetY = current.angle.y;
    cXyz* dstPos = nullptr;
    switch ((u32)(s32)field_0x7D2) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, -20.0f);
        srcX = current.pos.x;
        vec1->copy(*eye);
        srcY = eyePos.y;
        dstPos = vec1;
        srcZ = current.pos.z;
        break;
    }
    case 2:
        vec1->copy(field_0x798);
        srcZ = current.pos.z;
        srcX = current.pos.x;
        dstPos = vec1;
        break;
    case 3:
        targetY = field_0x7B2;
        break;
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&field_0x7B0, l_HIO.mPrmTbl.field_12, 4, 0x800);
    } else {
        field_0x7B0 = 0;
    }
    gabi::Local<cXyz> vec2; /* passed by value: a copy */
    vec2->x = srcX;
    vec2->y = srcY;
    vec2->z = srcZ;
    Kp1_lookAtTarget(&m_jnt, &current.angle.y, dstPos, vec2, targetY, field_0x7B0, headOnlyFollow);
}

/* 0227D2C0 */
u8 daNpc_Kp1_c::chkAttention() {

    dAttention_c* attention = dComIfGp_getAttention();
    if (Kp1_lockonTruth(attention)) {
        return this == (void*)Kp1_lockonTarget(attention, 0);
    }
    return this == (void*)Kp1_actionTarget(attention, 0);
}

/* 0227B94C */
void daNpc_Kp1_c::setAttention() {

    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + l_HIO.mPrmTbl.mAttentionArrowYOffset;
    attPos->x = x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (!field_0x7BC && !field_0x7C0) {
        return;
    }
    f32 z = mEyePos.z;
    f32 ex = mEyePos.x;
    eyePos.z = z;
    f32 ey = mEyePos.y;
    eyePos.x = ex;
    eyePos.y = ey;
    field_0x7BC = 0;
}

/* 0227B700 */
bool daNpc_Kp1_c::decideType(int i_type) {

    field_0x7D3 = -1;
    if (fpcM_GetName(this) == fpcNm_NPC_KP1_e) {
        field_0x7D4 = 0;
        field_0x7D3 = 0;
    }
    return true;
}

/* 0227C1E0 */
void daNpc_Kp1_c::event_actionInit(int param_1) {

    be<s32>* actionNumber = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, "ActNo" /* "ActNo" */);
    dComIfGp_evmng_getMyIntegerP(param_1, "Timer" /* "Timer" */);
    if (actionNumber != nullptr) {
        field_0x7CA = (s8)(s32)*actionNumber;
    }
}

/* 0227C268 */
bool daNpc_Kp1_c::event_action() {

    bool ret = false;
    switch ((u32)(s32)field_0x7CA) {
    case 0: { /* hand over the item 0x9A */
        u32 id =
            Kp1_createPresent(&current.pos, KP1_ITEM_MAGGIES_LETTER, 0, -1, -1, nullptr, nullptr);
        if (id != (u32)-1) {
            dComIfGp_event_setItemPartnerId(id);
            ret = true;
        }
        break;
    }
    case 1: { /* hand over the item 7 */
        u32 id = Kp1_createPresent(&current.pos, KP1_ITEM_HEART_PIECE, 0, -1, -1, nullptr, nullptr);
        if (id != (u32)-1) {
            dComIfGp_event_setItemPartnerId(id);
            ret = true;
        }
        break;
    }
    default:
        ret = true;
        break;
    }
    return ret;
}

/* 0227C32C */
void daNpc_Kp1_c::privateCut() {

    // The single private cut is ACTION.
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Kp1", nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    field_0x7C9 = (s8)dComIfGp_evmng_getMyActIdx(staffIdx, &cut_name_tbl, 1, TRUE, 0);
    if (field_0x7C9 == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        if (field_0x7C9 == 0) {
            event_actionInit(staffIdx);
        }
    }
    bool complete = true;
    if (field_0x7C9 == 0) {
        complete = event_action();
    }
    if (complete) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}

/* 0227C1A0 */
void daNpc_Kp1_c::endEvent() {

    dComIfGp_event_reset();
    field_0x7CB = 0xFF;
}

/* 0227C600 */
void daNpc_Kp1_c::event_proc() {

    // HD: compared with Km1, the two letter/heart-piece events end here
    if (Kp1_endEvent(dComIfGp_getPEvtManager(), "GET_MAGYS_LTTR" /* "GET_MAGYS_LTTR" */)) {
        endEvent();
        shape_angle.x = current.angle.x;
        shape_angle.z = current.angle.z;
        field_0x7CF = 1;
        mLetterEventEnded = 1;
        shape_angle.y = current.angle.y;
    } else if (Kp1_endEvent(dComIfGp_getPEvtManager(), "GET_KAKERA_HRT" /* "GET_KAKERA_HRT" */)) {
        dComIfGs_setEventReg(0xCCFF, 0);
        endEvent();
        shape_angle.x = current.angle.x;
        shape_angle.z = current.angle.z;
        mHeartPieceEventEnded = 1;
        field_0x7CF = 1;
        shape_angle.y = current.angle.y;
    } else {
        if (!mEventCut.cutProc()) {
            privateCut();
        }
        lookBack();
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
}

/* 0227B728 */
bool daNpc_Kp1_c::set_action(Kp1ActionFunc* i_action, void* param_2) {

    Kp1ActionFunc* cur = &mAction;
    s16 newI = i_action->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return true;
        newD = i_action->d;
        newF = i_action->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return true;
    } else {
        newF = i_action->f;
        newD = i_action->d;
    }
    if (cur->i != 0) {
        field_0x7D5 = -1;
        Kp1_callAction(this, cur, param_2);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x7D5 = 0;
    Kp1_callAction(this, cur, param_2);
    return true;
}

/* 0227D348 */
BOOL daNpc_Kp1_c::chk_talk() {

    BOOL ret = TRUE;
    field_0x7B6 = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd() == 0) {
            return FALSE;
        }
        field_0x7B6 = dComIfGp_event_getPreItemNo();
    }
    return ret;
}

/* 0227D3C8 */
BOOL daNpc_Kp1_c::wait01() {

    s8 order = field_0x7CF;
    if (order == 3 || order == 4 || order == 1) {
        return TRUE;
    }
    if (field_0x7C5 != 0) {
        if (chk_talk()) {
            setStt(KP1_TALK);
        }
    } else {
        field_0x7CF = 2;
        if (field_0x7C4) {
            field_0x7D2 = 1;
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            s16 y = field_0x76C.y;
            field_0x7D2 = 3;
            field_0x7B2 = y;
            m_jnt.mbTrn = 1;
        }
    }
    return TRUE;
}

/* 0227D480 */
BOOL daNpc_Kp1_c::talk01() {

    talk(1);
    // HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus)
    if (mbHasMsg) {
        u32 status = Kp1_messageStatus(l_message_manager);
        if (status == 2 || status == 6) {
            if (mCancelPresent) {
                dComIfGp_evmng_CancelPresent();
                mCancelPresent = 0;
            }
        } else if (status == KP1_MSG_DESTROYED) {
            if (mCurrMsgNo == 0x1E9D) {
                dComIfGs_setReserveItemEmpty();
            }
            field_0x7B6 = 0xFF;
            setStt(field_0x7D1);
            field_0x7C8 = 0;
            field_0x7C5 = 0;
            endEvent();
        }
    }
    return TRUE;
}

/* 0227D558 */
BOOL daNpc_Kp1_c::wait_action1(void*) {

    if (field_0x7D5 == 0) {
        setStt(KP1_WAIT);
        field_0x7D5 = field_0x7D5 + 1;
    } else if (field_0x7D5 != -1) {
        field_0x7C4 = chkAttention();
        switch ((u32)(s32)field_0x7D0) {
        case 2:
            field_0x7C0 = talk01();
            break;
        case 1:
            field_0x7C0 = wait01();
            break;
        default:
            field_0x7C0 = 0;
            break;
        }
        lookBack();
    }
    return 1;
}

/* 0227BFE0 */
u8 daNpc_Kp1_c::demo() {

    if (demoActorID == 0) {
        if (field_0x7C7 != 0) {
            field_0x7C7 = 0;
        }
    } else {
        u8 id = demoActorID;
        field_0x7C7 = 1;
        void* demoAc = nullptr;
        /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
        if (id != 0 && id <= 0x20) {
            u32 obj = (u32)l_demo_object;
            if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
                JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
                obj = (u32)l_demo_object;
            }
            demoAc = Kp1_demoGetActor(obj, id);
        }
        // HD: compared with Km1, the eye pattern runs, and the demo may replace it
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if (frame >= Kp1_frameMax(m_head_tex_pattern)) {
            mBtpFrame = (u8)Kp1_frameMax(m_head_tex_pattern);
        }
        if (demoAc != nullptr) {
            J3DAnmTexPattern* btp = Kp1_demoBtp(demoAc, "Kp");
            if (btp != nullptr) {
                m_head_tex_pattern = btp;
                if (Kp1_btpInit(mBtpAnm, mpMorf->getModel()->getModelData(), btp, TRUE, 2, 1.0f, 0,
                                -1, TRUE, FALSE)) {
                    mBtpFrame = 0;
                }
            }
        }
        Kp1_demoData(this, 0x6A, mpMorf, "Kp", 0, nullptr, 0, 0);
    }
    return field_0x7C7;
}

/* 0227C930 */
BOOL daNpc_Kp1_c::_draw() {

    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = model->getModelData();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBtpFrame);
    mpMorf->entryDL();
    Kp1_view<Kp1ModelAnimation>(model_data)->mpBtpAnimation = 0; /* mBtpAnm.remove() */
    // HD: compared with Km1, the hand-held model chosen by chngAnmAtr
    J3DModel* item = nullptr;
    bool draw = true;
    if (mItemModelIndex == 1) {
        item = mpItemModel[0];
    } else if (mItemModelIndex == 2) {
        item = mpItemModel[1];
    } else {
        draw = false;
    }
    if (draw && item != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), item, &tevStr);
        mDoExt_modelEntryDL(item);
    }
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO.mPrmTbl.field_18) {
        Kp1_initDebugColour(&l_red_guard, &l_red_colour, &a_red_colour);
        Kp1_initDebugColour(&l_blue_guard, &l_blue_colour, &a_blue_colour);
    }
    dSnap_RegistFig(0x5A /* DSNAP_TYPE_NPC_KP1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 0227C774 */
BOOL daNpc_Kp1_c::_execute() {

    if (field_0x7B8 == 0) {
        field_0x774.copy(current.pos);
        field_0x76C.x = current.angle.x;
        field_0x76C.y = current.angle.y;
        field_0x76C.z = current.angle.z;
        field_0x7B8 = 1;
    }
    daNpc_Kp1_HIO_c::hio_prm_c& prm = l_HIO.mPrmTbl;
    m_jnt.setParam(prm.field_8, prm.field_A, prm.field_C, prm.field_E, prm.field_0, prm.field_2,
                   prm.field_4, prm.field_6, prm.field_10);
    checkOrder();
    if (demo() == 0) {
        if (dComIfGp_event_runCheck() &&
            (u16)eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */) {
            event_proc();
        } else {
            Kp1_callAction(this, &mAction, nullptr); /* (this->*field_0x6F4)(NULL) */
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    if (field_0x7C7 == 0) {
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* &mStts.m_cc_move */
    }
    setMtx();
    setCollision(60.0f, 170.0f);
    return true;
}

/* 0227BEA4 */
BOOL daNpc_Kp1_c::_delete() {

    dComIfG_resDelete(&mPhs, "Kp");
    // HD: compared with Km1, no HIO child; the animation is stopped only if the heap was made
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}

/* 0227B6FC */
static BOOL CheckCreateHeap(fopAc_ac_c* actor) { return ((daNpc_Kp1_c*)actor)->CreateHeap(); }

/* 0227BD5C */
cPhs_State daNpc_Kp1_c::_create() {

    /* fopAcM_ct(this, daNpc_Kp1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Kp1_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x1001D070;
            Kp1_btpCtor(mBtpAnm);         /* mDoExt_btpAnm::mDoExt_btpAnm */
            Kp1_eventCutCtor(&mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    // HD: compared with Km1, the resource first, no HIO child
    cPhs_State resLoadResult = dComIfG_resLoad(&mPhs, "Kp");
    if (resLoadResult != cPhs_COMPLEATE_e) {
        return resLoadResult;
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, (u32)a_heap_size)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 170.0f, 50.0f);
    if (createInit() == 0) {
        resLoadResult = cPhs_ERROR_e;
    }
    return resLoadResult;
}

/* 0227B340 */
BOOL daNpc_Kp1_c::CreateHeap() {

    J3DModelData* a_mdl_data = (J3DModelData*)dComIfG_getObjectIDRes("Kp", KP1_BDL_BODY);
    if (a_mdl_data == nullptr) /* JUT_ASSERT(1593, a_mdl_data != NULL) */
        JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x639, "a_mdl_data != (0)");
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes("Kp", KP1_BCK_WAIT);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_data, nullptr, nullptr, bck, 2 /* EMode_LOOP */,
                                    1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020002);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() != nullptr) {
        m_head_jnt_num = (s8)a_mdl_data->getJointName()->getIndex("head" /* "head" */);
        if (m_head_jnt_num < 0)
            JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x64D, "m_head_jnt_num >= 0");
        m_backbone_jnt_num = (s8)a_mdl_data->getJointName()->getIndex("backbone" /* "backbone" */);
        if (m_backbone_jnt_num < 0)
            JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x64F, "m_backbone_jnt_num >= 0");
        m_hand_jnt_num = (s8)a_mdl_data->getJointName()->getIndex("handL" /* "handL" */);
        if (m_hand_jnt_num < 0)
            JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x651, "m_handL_jnt_num >= 0");
        field_0x7CD = (s8)a_tex_pattern_num; /* a_tex_pattern_num_tbl[0] */
        if (initTexPatternAnm(false) != 0) {
            // HD: compared with Km1, two hand-held models (ids 6 and 7)
            J3DModelData* itemData = (J3DModelData*)dComIfG_getObjectIDRes("Kp", KP1_BDL_LETTER);
            if (itemData == nullptr)
                JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x66B, "a_itm_mdl_data != (0)");
            mpItemModel[0] = mDoExt_J3DModel__create(itemData, 0x80000, 0x11000002);
            if (mpItemModel[0].get() != nullptr) {
                itemData = (J3DModelData*)dComIfG_getObjectIDRes("Kp", KP1_BDL_HEART_PIECE);
                if (itemData == nullptr)
                    JUT_ASSERT_fail("d_a_npc_kp1.cpp", 0x675, "a_itm_mdl_data != (0)");
                mpItemModel[1] = mDoExt_J3DModel__create(itemData, 0x80000, 0x11000002);
                if (mpItemModel[1].get() != nullptr) {
                    for (u16 i = 0; i < a_mdl_data->getJointNum(); i++) {
                        if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
                            J3DModelData* md = mpMorf->getModel()->getModelData();
                            md->getJointNodePointer(i)->setCallBack(nodeCallBack_Kp);
                        }
                    }
                    mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
                    mAcchCir.SetWall(30.0f, 60.0f);
                    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr,
                                 nullptr);
                    return TRUE;
                }
            }
        }
        // HD: compared with Km1, the morf is deleted (virtual deleting destructor)
        if (mpMorf.get() != nullptr) {
            Kp1_destroyMorf(mpMorf);
        }
    } else {
        Kp1_destroyMorf(mpMorf);
    }
    mpMorf = nullptr;
    return FALSE;
}

/* 0227BEA0 */
static cPhs_State daNpc_Kp1_Create(fopAc_ac_c* i_this) { return ((daNpc_Kp1_c*)i_this)->_create(); }

/* 0227BEFC */
static BOOL daNpc_Kp1_Delete(daNpc_Kp1_c* i_this) { return i_this->_delete(); }

/* 0227C92C */
static BOOL daNpc_Kp1_Execute(daNpc_Kp1_c* i_this) { return i_this->_execute(); }

/* 0227CAA8 */
static BOOL daNpc_Kp1_Draw(daNpc_Kp1_c* i_this) { return i_this->_draw(); }

/* 0227CAAC */
static BOOL daNpc_Kp1_IsDelete(daNpc_Kp1_c*) { return TRUE; }

// HD: compiler-generated initialization and deleting destructors.
/* 0227D688 */
static void __sinit_d_a_npc_kp1_cpp() {

    sinit_header_statics_z(0x104678FC, 0x101BFEF8, 0x10467930);
    daNpc_Kp1_HIO_c_ct(&l_HIO); /* static daNpc_Kp1_HIO_c l_HIO */
}

/* 0227D728 */
static void SafeString_dt(SafeString* i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0227D73C */
static void daNpc_Kp1_c_dt(daNpc_Kp1_c* i_this, s32 flags) {

    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Kp1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Kp1_view<Kp1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1001CDDC;
        Kp1_view<Kp1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1001CDEC;
        Kp1_acchDestroy(&i_this->mObjAcch, 0);
        Kp1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0227D7D8 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 1001CF54 "Kp1" in 0227BC14
str 1001D01C "d_a_npc_kp1.cpp" in 0227CAB4
str 1001D02C "0 <= i_num && i_num < ANM_END" in 0227CAB4
str 1001D04C "o_bck_num && o_bas_num" in 0227CAB4
str 1001CE3C "d_a_npc_kp1.cpp" in 0227B168
str 1001CE4C "0 <= i_num && i_num < TEXPATTERN_END" in 0227B168
str 1001CE78 "Kp" in 0227B1D0
str 1001CE7C "d_a_npc_kp1.cpp" in 0227B1D0
str 1001CE8C "m_head_tex_pattern != (0)" in 0227B1D0
str 1001D064 "Kp" in 0227CB88
str 1001CF6C "GET_MAGYS_LTTR" in 0227BF00
str 1001CF7C "GET_KAKERA_HRT" in 0227BF00
str 1001CF90 "ActNo" in 0227C1E0
str 1001CF98 "Timer" in 0227C1E0
str 1001CFA0 "Kp1" in 0227C32C
str 1001CFAC "GET_MAGYS_LTTR" in 0227C600
str 1001CFBC "GET_KAKERA_HRT" in 0227C600
str 1001CE1C "d_demo.h" in 0227BFE0
str 1001CE0C "m_object != (0)" in 0227BFE0
str 1001CF8C "Kp" in 0227BFE0
str 1001CF67 "Kp" in 0227BEA4
str 1001CF64 "Kp" in 0227BD5C
str 1001CEB8 "Kp" in 0227B340
str 1001CEC4 "d_a_npc_kp1.cpp" in 0227B340
str 1001CF00 "a_mdl_data != (0)" in 0227B340
str 1001CEBC "head" in 0227B340
str 1001CEEC "m_head_jnt_num >= 0" in 0227B340
str 1001CF2C "backbone" in 0227B340
str 1001CED4 "m_backbone_jnt_num >= 0" in 0227B340
str 1001CEB0 "handL" in 0227B340
str 1001CF38 "m_handL_jnt_num >= 0" in 0227B340
str 1001CF14 "a_itm_mdl_data != (0)" in 0227B340
ext 024EF130 s8 Kp1_roomId(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 Kp1_polyColour(dBgS*, cBgS_PolyInfo*)
ext 02055B64 s16 Kp1_calcTimer(be<s16>* t)
ext 025E789C s32 Kp1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 024EDFCC bool Kp1_lockonTruth(dAttention_c* a)
ext 024EC8D0 fopAc_ac_c* Kp1_lockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* Kp1_actionTarget(dAttention_c* a, s32 i)
ext 0259DED0 void Kp1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 02527028 BOOL Kp1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 02526E70 void* Kp1_demoGetActor(u32 obj, u8 id)
ext 025A1458 void Kp1_npcCtor(daNpc_Kp1_c* p)
ext 025E7820 void Kp1_btpCtor(void* p)
ext 0259F740 void Kp1_eventCutCtor(dNpc_EventCut_c* p)
ext 02018034 void Kp1_circleDestroy(void* p, s32 flags)
ext 024EFD9C void Kp1_acchDestroy(dBgS_Acch* p, s32 flags)
ext 025D50BC void Kp1_actorDestroy(daNpc_Kp1_c* p, s32 flags)
ext 025F795C u32 Kp1_messageStatus(Kp1MessageManager*)

ext 025D77DC void Kp1_orderOtherEvent(fopAc_ac_c*,const char*,u16,u16)
ext 025445B8 BOOL Kp1_startEvent(dEvent_manager_c*,const char*)
ext 0254457C BOOL Kp1_endEvent(dEvent_manager_c*,const char*)
ext 025D7DEC u32 Kp1_createPresent(cXyz*,s32,u8,s32,s32,csXyz*,cXyz*)
ext 02527828 J3DAnmTexPattern* Kp1_demoBtp(void*,const char*)
*/
