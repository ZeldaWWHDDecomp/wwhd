/**
 * d_a_npc_hi1.cpp (WWHD)
 * NPC - King of Hyrule
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_hi1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 * Compiler-generated functions (HIO constructor, __sinit, destructors) are written from the WWHD
 * code.
 */
#include "d/actor/d_a_npc_hi1.h"

// HD: each mapped object belongs to this translation unit.
const gc_tu_data gc_tu = {.safeString = 0x1001A3CC};
static daNpc_Hi1_HIO_c& l_HIO = gabi::object_at<daNpc_Hi1_HIO_c>(0x10467164);
static cXyz& a_eye_pos_off = gabi::object_at<cXyz>(0x1046719C);
static be<u32>& l_eye_guard = gabi::object_at<be<u32>>(0x104671A8);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static Hi1ActionFunc& l_wait_action = gabi::object_at<Hi1ActionFunc>(0x1001A3B0);
static dCcD_SrcCyl& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static gptr<void>& l_message_manager = gabi::object_at<gptr<void>>(0x101F4B5C);
static be<u32>& l_red_guard = gabi::object_at<be<u32>>(0x101FDA50);
static be<u32>& l_blue_guard = gabi::object_at<be<u32>>(0x101FDAC0);
static be<u32>& l_red_colour = gabi::object_at<be<u32>>(0x101FEBF4);
static be<u32>& l_blue_colour = gabi::object_at<be<u32>>(0x101FEBF8);
static be<u32>& a_red_colour = gabi::object_at<be<u32>>(0x1001A3B8);
static be<u32>& a_blue_colour = gabi::object_at<be<u32>>(0x1001A3BC);
using Hi1PrmBytes = be<u8>[0x1C];
static Hi1PrmBytes& a_prm_tbl = gabi::object_at<Hi1PrmBytes>(0x101BE798);
static be<s32>& a_btp_resID = gabi::object_at<be<s32>>(0x1001A43C);
static be<s32>& a_btk_resID = gabi::object_at<be<s32>>(0x1001A464);
static be<s8>& a_btk_num = gabi::object_at<be<s8>>(0x1001A488);
using Hi1BckIds = be<s32>[2];
static Hi1BckIds& a_bck_resID_tbl = gabi::object_at<Hi1BckIds>(0x1001A528);
using Hi1NumParams = daNpc_Hi1_c::anm_prm_c[2];
using Hi1StateParams = daNpc_Hi1_c::anm_prm_c[3];
static Hi1NumParams& a_anm_num_tbl = gabi::object_at<Hi1NumParams>(0x101BE728);
static Hi1StateParams& a_anm_state_tbl = gabi::object_at<Hi1StateParams>(0x101BE748);
static Hi1NumParams& a_anm_attr_tbl = gabi::object_at<Hi1NumParams>(0x101BE778);
using Hi1HeapSizes = be<u32>[2];
static Hi1HeapSizes& a_heap_size_tbl = gabi::object_at<Hi1HeapSizes>(0x101BE71C);
static gptr<char>& l_evn_name = gabi::object_at<gptr<char>>(0x101BE6F8);
static gptr<char>& a_cut_tbl = gabi::object_at<gptr<char>>(0x101BE724);
using Hi1ArchiveName = be<u8>[3];
static Hi1ArchiveName& l_arc_name = gabi::object_at<Hi1ArchiveName>(0x1001A4F8);
enum Hi1State { HI1_WAIT = 1, HI1_TALK = 2 };
enum Hi1EventCommand { HI1_COMMAND_TALK = 1, HI1_COMMAND_DEMO_ACCEPT = 2 };
enum Hi1DemoFlags { HI1_DEMO_TRANS_ROT_ANM_FRAME = 0x6A };
enum Hi1Resource { dRes_ID_HI_BDL_HI_e = 2 };
enum Hi1MessageStatus {
    HI1_MSG_TYPING = 6,
    HI1_MSG_DISPLAYED = 14,
    HI1_MSG_ENDS = 16,
    HI1_MSG_DESTROYED = 19
};

/* inlined into _execute: preserve angle load/store order */
static inline void hi1_setAngle(daNpc_Hi1_c* i_this) {
    s16 z = i_this->current.angle.z, y = i_this->current.angle.y, x = i_this->current.angle.x;
    i_this->field_0x736.x = x;
    i_this->field_0x736.y = y;
    i_this->field_0x736.z = z;
    if (i_this->field_0x7B3 == 0) {
        i_this->shape_angle.z = i_this->current.angle.z;
        i_this->shape_angle.y = i_this->current.angle.y;
        i_this->shape_angle.x = i_this->current.angle.x;
    }
}

/* 0223BEFC */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Hi1_c* actor = static_cast<daNpc_Hi1_c*>(model->getUserArea());
        if (actor != nullptr) {
            actor->_nodeCB_Head(i_node, model);
        }
    }
    return TRUE;
}

/* 0223BD84 */
void daNpc_Hi1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_pModel) {

    if ((u32)l_eye_guard == 0) {
        l_eye_guard = 1;
        a_eye_pos_off.z = 0.0f;
        a_eye_pos_off.x = 20.0f;
        a_eye_pos_off.y = 30.0f;
    }
    u32 jntNo = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_pModel->getAnmMtx(jntNo), stk);
    field_0x768.x = stk->m[0][3];
    field_0x768.y = stk->m[1][3];
    field_0x768.z = stk->m[2][3];
    PSMTXMultVec(stk, &a_eye_pos_off, &field_0x744);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_pModel->getAnmMtx(jntNo), stk);
}

/* 0223C068 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Hi1_c* actor = static_cast<daNpc_Hi1_c*>(model->getUserArea());
        if (actor != nullptr) {
            actor->_nodeCB_BackBone(i_node, model);
        }
    }
    return TRUE;
}

/* 0223BF44 */
void daNpc_Hi1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_pModel) {
    u32 jntNo = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_pModel->getAnmMtx(jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_pModel->getAnmMtx(jntNo), stk);
}

/* 0223C658 */
static BOOL CheckCreateHeap(fopAc_ac_c* ac) { return ((daNpc_Hi1_c*)ac)->CreateHeap(); }

/* 0223C7EC */
bool daNpc_Hi1_c::init_HI1_0() {
    Hi1ActionFunc pmf;
    Hi1_loadAction(&pmf, &l_wait_action);
    set_action(&pmf, nullptr);
    return true;
}

/* 0223CB30 */
bool daNpc_Hi1_c::createInit() {

    const char* name = l_evn_name.get();
    mEventIdx[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2("Hi1", this);
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e |
        fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9;  /* attention_info.distances[TALK] */
    field_0x7C0 = 2;
    gravity = 0.0f;
    bool ret;
    switch (field_0x7C6) {
    case 0:
        ret = init_HI1_0();
        break;
    default:
        ret = false;
        break;
    }
    if (!ret) {
        return false;
    }
    field_0x736.x = current.angle.x;
    field_0x736.y = current.angle.y;
    field_0x736.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = Hi1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Hi1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return true;
}

/* 0223C928 */
void daNpc_Hi1_c::play_animation() {
    u32 mtrlSndId = 0;
    play_btp_anm();
    play_btk_anm();
    if (mObjAcch.ChkGroundHit()) {
        mtrlSndId = Hi1_materialSound(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    field_0x7AC = (s8)Hi1_playMorf(mpMorf.get(), &eyePos, mtrlSndId, reverb);
    if (mpMorf->getFrame() < mFrame) {
        field_0x7AC = 1;
    }
    mFrame = mpMorf->getFrame();
}

/* 0223CA28 */
void daNpc_Hi1_c::setMtx(u32 param_1) {
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), field_0x736.x, field_0x736.y, field_0x736.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(param_1);
}

/* 0223DB18 */
s32 daNpc_Hi1_c::bckResID(int idx) { return (s32)a_bck_resID_tbl[idx]; }

/* 0223C0B0 */
s32 daNpc_Hi1_c::btpResID(int) {
    // HD: the single-entry table ignores the incoming index.

    return (s32)a_btp_resID;
}

/* 0223C1A8 */
s32 daNpc_Hi1_c::btkResID(int) {
    // HD: the single-entry table ignores the incoming index.
    return (s32)a_btk_resID;
}

/* 0223C0BC */
bool daNpc_Hi1_c::setBtp(s32 param_1, u32 param_2) {
    J3DModel* model = mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    J3DAnmTexPattern* a_btp =
        (J3DAnmTexPattern*)dComIfG_getObjectIDRes(mArcName, btpResID(param_1));
    if (a_btp == nullptr) /* JUT_ASSERT(0x195, a_btp != NULL) */
        JUT_ASSERT_fail("d_a_npc_hi1.cpp", 0x195, "a_btp != (0)");
    field_0x7BE = (s8)param_1;
    mBtpAnmFrame = 0;
    mTimer1 = 0;
    return Hi1_btpInit(mBtpAnm, model->getModelData(), a_btp, 1, 0 /* EMode_RESET */, 1.0f, 0, -1,
                       param_2, 0) != 0;
}

/* 0223C1B4 */
bool daNpc_Hi1_c::setBtk(s32 param_1, u32 param_2) {
    J3DModel* model = mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    J3DAnmTextureSRTKey* a_btk =
        (J3DAnmTextureSRTKey*)dComIfG_getObjectIDRes(mArcName, btkResID(param_1));
    if (a_btk == nullptr) /* JUT_ASSERT(0x1ad, a_btk != NULL) */
        JUT_ASSERT_fail("d_a_npc_hi1.cpp", 0x1AD, "a_btk != (0)");
    field_0x7BF = (s8)param_1;
    mBtkAnmFrame = 0;
    return Hi1_btkInit(mBtkAnm, model->getModelData(), a_btk, 1, 0 /* EMode_RESET */, 1.0f, 0, -1,
                       param_2, 0) != 0;
}

/* 0223C29C */
bool daNpc_Hi1_c::init_texPttrnAnm(s32 param_1, u32 param_2) {
    if (setBtp(param_1, param_2) == false) {
        return false;
    }

    // HD: use the only btk mapping entry, independent of param_1.
    return setBtk((s8)a_btk_num, param_2);
}

/* 0223C82C */
void daNpc_Hi1_c::play_btp_anm() {
    u8 frame = (u8)Hi1_frameMax(Hi1_view<Hi1BtpAnm>(mBtpAnm)->mpAnimation.get());
    if (field_0x7BE != 0 || Hi1_calcTimer(&mTimer1) == 0) {
        u8 f = (u8)(mBtpAnmFrame + 1);
        mBtpAnmFrame = f;
        if (f >= frame) {
            if (field_0x7BE == 0) {
                mTimer1 = (s16)Hi1_getRndValue(0x3C, 0x5A);
                frame = 0;
            }
            mBtpAnmFrame = frame;
        }
    }
}

/* 0223C8CC */
void daNpc_Hi1_c::play_btk_anm() {
    u8 frame = (u8)Hi1_frameMax(Hi1_view<Hi1BtkAnm>(mBtkAnm)->mpAnimation.get());
    u8 f = (u8)(mBtkAnmFrame + 1);
    if (f >= frame) {
        f = frame;
    }
    mBtkAnmFrame = f;
}

/* 0223DB2C */
void daNpc_Hi1_c::setAnm_anm(anm_prm_c* param_1) {
    s8 temp = param_1->field_0x0;
    if (temp < 0 || field_0x7C0 == temp) {
        return;
    }
    s32 resID = bckResID(temp);
    s32 loopMode = param_1->field_0xC;
    f32 morf = param_1->field_0x4;
    f32 speed = param_1->field_0x8;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    field_0x7AC = 0;
    field_0x7C0 = param_1->field_0x0;
    mFrame = 0.0f;
    field_0x7AD = 0;
}

/* 0223DBC4 */
void daNpc_Hi1_c::setAnm_NUM(int param_1, int param_2) {
    anm_prm_c* a_anm_prm_tbl = a_anm_num_tbl; /* [2] */
    if (param_2 != 0) {
        init_texPttrnAnm(a_anm_prm_tbl[param_1].field_0x1, 1);
    }
    setAnm_anm(&a_anm_prm_tbl[param_1]);
}

/* 0223DC34 */
void daNpc_Hi1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = a_anm_state_tbl; /* [3] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x7C2].field_0x1, 1);
    setAnm_anm(&a_anm_prm_tbl[field_0x7C2]);
}

/* 0223DD0C */
void daNpc_Hi1_c::chngAnmAtr(u32 param_1) {
    // HD: reject index 2 as well; the attribute table has only two entries.
    if (param_1 == field_0x7BC || param_1 >= 2) {
        return;
    }
    field_0x7BC = (u8)param_1;
    setAnm_ATR();
}

/* 0223DCA4 */
void daNpc_Hi1_c::setAnm_ATR() {
    anm_prm_c* a_anm_prm_tbl = a_anm_attr_tbl; /* [2] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x7BC].field_0x1, 1);
    setAnm_anm(&a_anm_prm_tbl[field_0x7BC]);
}

/* 0223DD28 */
void daNpc_Hi1_c::anmAtr(u32 i_msgStatus) {
    if (i_msgStatus == HI1_MSG_TYPING) {
        if (field_0x7C8 == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            field_0x7C8 = field_0x7C8 + 1;
        }
        u8 tagInfo = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tagInfo != 0xFF && tagInfo != field_0x7BD) {
            dComIfGp_clearMesgAnimeTagInfo(); /* dComIfGp_clearMesgAnimeTagInfo() */
            field_0x7BD = tagInfo;
            /* chngAnmTag(): empty */
        }
    } else if (i_msgStatus == HI1_MSG_DISPLAYED) {
        field_0x7C8 = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}

/* 0223DDF8 */
u16 daNpc_Hi1_c::next_msgStatus(be<u32>*) { return HI1_MSG_ENDS; }

/* 0223DE00 */
u32 daNpc_Hi1_c::getMsg() {
    // HD: getMsg_HI1_0 is inlined and the type switch is eliminated.
    return 0;
}

/* 0223D67C */
void daNpc_Hi1_c::eventOrder() {
    s8 cond = field_0x7C1;
    if (cond == 1 || cond == 2) {
        s8 c = field_0x7C1;
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (c == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (cond >= 3) {
        s16 idx = (s16)(cond - 3);
        field_0x796 = idx;
        fopAcM_orderOtherEventId(this, mEventIdx[idx], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 0223CE7C */
void daNpc_Hi1_c::checkOrder() {
    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == HI1_COMMAND_DEMO_ACCEPT) {
        if (dComIfGp_evmng_startCheck(mEventIdx[field_0x796]) && field_0x7C1 >= 3) {
            field_0x7C1 = 0;
            field_0x7BC = 0xFF;
            field_0x7BD = 0xFF;
        }
    } else if (command == HI1_COMMAND_TALK) {
        if (field_0x7C1 == 1 || field_0x7C1 == 2) {
            field_0x7C1 = 0;
            field_0x7B7 = 1;
        }
    }
}

/* 0223DE08 */
bool daNpc_Hi1_c::chk_talk() {
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    mItemNo = 0xFF;
    return true;
}

/* 0223DEA0 */
u8 daNpc_Hi1_c::chk_parts_notMov() {
    return field_0x788.y != m_jnt.mAngles[0][1] || field_0x788.z != m_jnt.mAngles[1][1] ||
           field_0x788.x != current.angle.y;
}

/* 0223D394 */
fopAc_ac_c* daNpc_Hi1_c::searchByID(fpc_ProcID pid, be<s32>* param_2) {
    typedef be<u32> ActorResult;
    ActorResult actor;
    actor = 0;
    *param_2 = 0;
    if (!Hi1_searchById(pid, &actor)) {
        *param_2 = 1;
    }
    return Hi1_actorFromIdResult(actor);
}

/* 0223D3E8 */
void daNpc_Hi1_c::lookBack() {
    cXyz temp5;
    temp5.x = 0.0f;
    temp5.y = 0.0f;
    temp5.z = 0.0f;
    field_0x788.y = m_jnt.mAngles[0][1];
    f32 srcX = current.pos.x;
    s16 targetY = current.angle.y;
    field_0x788.x = targetY;
    u8 temp4 = field_0x7B8;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    field_0x788.z = m_jnt.mAngles[1][1];
    cXyz* temp2 = nullptr;
    switch ((u32)(s32)field_0x7C4) {
    case 1: {
        cXyz eye;
        Hi1_playerEyePos(&eye, -20.0f);
        Hi1_copyWords(&field_0x750, &eye);
        Hi1_copyWords(&temp5, &eye);
        temp2 = &temp5;
        break;
    }
    case 2:
        Hi1_copyWords(&temp5, &field_0x750);
        temp2 = &temp5;
        break;
    case 3:
        targetY = field_0x7AA;
        break;
    case 4: {
        typedef be<s32> SearchResult;
        SearchResult temp3;
        fopAc_ac_c* actor = searchByID(mPId, &temp3);
        if (actor != nullptr && temp3 == 0) {
            auto* src = Hi1_view<be<u32>>(&actor->current.pos);
            auto* dst = Hi1_view<be<u32>>(&field_0x750);
            dst[0] = src[0];
            dst[1] = src[1];
            u32 z = (u32)src[2];
            f32 x = field_0x750.x;
            dst[2] = z;
            f32 ey = actor->eyePos.y;
            temp5.x = x;
            f32 zf = field_0x750.z;
            temp5.y = ey;
            temp5.z = zf;
            temp2 = &temp5;
            field_0x750.y = ey;
        }
        break;
    }
    default:
        break;
    }
    cXyz temp; /* passed by value: a copy */
    temp.x = srcX;
    temp.z = srcZ;
    temp.y = srcY;
    Hi1_lookAtTarget(&m_jnt, &current.angle.y, temp2, &temp, targetY, l_HIO.mPrmTbl.field_12,
                     temp4);
}

/* 0223DEE0 */
u8 daNpc_Hi1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (Hi1_lockonTruth(attention)) {
        return this == (void*)Hi1_lockonTarget(attention, 0);
    }
    return this == (void*)Hi1_actionTarget(attention, 0);
}

/* 0223C9D4 */
void daNpc_Hi1_c::setAttention(u32 param_1) {
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + l_HIO.mPrmTbl.mAttentionYOffset;
    attPos->x = x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (field_0x790 == 0 && param_1 == 0) {
        return;
    }
    eyePos.z = field_0x744.z;
    eyePos.y = field_0x744.y;
    eyePos.x = field_0x744.x;
}

/* 0223C65C */
bool daNpc_Hi1_c::decideType(int param_1) {
    if (field_0x7C5 > 0) {
        return true;
    }
    field_0x7C5 = 1;
    field_0x7C6 = 0;

    u8 c0 = l_arc_name[0], c1 = l_arc_name[1], c2 = l_arc_name[2];
    auto* dst = Hi1_bytes(mArcName);
    dst[0] = c0;
    dst[1] = c1;
    dst[2] = c2;
    bool temp = false;
    if (field_0x7C5 != -1 && field_0x7C6 != -1) {
        temp = true;
    }
    return temp;
}

/* 0223D258 */
void daNpc_Hi1_c::privateCut(int i_staffIdx) {
    if (i_staffIdx == -1) {
        return;
    }

    s8 actIdx = dComIfGp_evmng_getMyActIdx(i_staffIdx, &a_cut_tbl, 1, TRUE, 0);
    mActIdx = actIdx;
    if (actIdx != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 0223D214 */
void daNpc_Hi1_c::endEvent() {
    dComIfGp_event_reset();
    field_0x7BC = 0xFF;
    field_0x7BD = 0xFF;
}

/* 0223D1D4 */
s32 daNpc_Hi1_c::isEventEntry() {
    const char* name = Hi1_staffName(&mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 0223D2F0 */
void daNpc_Hi1_c::event_proc(int i_staffIdx) {
    s16 ev = mEventIdx[field_0x796];
    if (dComIfGp_evmng_endCheck(ev)) {
        endEvent();
        return;
    }
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}

/* 0223C6C0 */
BOOL daNpc_Hi1_c::set_action(Hi1ActionFunc* i_action, void* param_2) {
    Hi1ActionFunc* cur = &mCurrActionFunc;
    s16 newI = i_action->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_action->d;
        newF = i_action->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_action->f;
        newD = i_action->d;
    }
    if (cur->i != 0) {
        field_0x7C7 = 9;
        Hi1_callAction(this, cur, param_2);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x7C7 = 0;
    Hi1_callAction(this, cur, param_2);
    return TRUE;
}

/* 0223DF68 */
void daNpc_Hi1_c::setStt(u32 param_1) {
    s8 temp = field_0x7C2;
    field_0x7C2 = (s8)param_1;
    switch (param_1) {
    case HI1_WAIT:
        field_0x7C1 = 0;
        break;
    case HI1_TALK:
        field_0x7C1 = 0;
        field_0x7BD = 0xFF;
        field_0x7BC = 0xFF;
        field_0x7C3 = temp;
        field_0x7C8 = 0;
        break;
    }
    setAnm();
}

/* 0223DFB4 */
BOOL daNpc_Hi1_c::wait_1() {
    s16 timer = 0;
    if (field_0x7B9 != 0) {
        cLib_addCalcAngleS(&current.angle.y, field_0x730.y, 4, 0x800, 0x80);
        timer = (s16)(field_0x730.y - current.angle.y);
    }
    if (field_0x7B7 != 0) {
        if (chk_talk()) {
            setStt(HI1_TALK);
            field_0x7B8 = 0;
            field_0x7C4 = 1;
            field_0x7B9 = 0;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    field_0x7C4 = 0;
    field_0x7C1 = 2;
    field_0x7B8 = 1;
    if (timer == 0) {
        if (mHasAttention != 0) {
            mTimer2 = (s16)Hi1_getRndValue(0xF, 0x1E);
        }
        if (Hi1_calcTimer(&mTimer2) != 0) {
            field_0x7C4 = 1;
        }
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        cXyz pos; /* passed by value: a copy */
        pos.x = player->current.pos.x;
        pos.y = player->current.pos.y;
        pos.z = player->current.pos.z;
        if (!Hi1_checkAttention(this, &pos, 200.0f, 50.0f, 55.0f, field_0x7C4 == 1)) {
            field_0x7C4 = 0;
            field_0x7B9 = 1;
        }
    }
    return TRUE;
}

/* 0223E12C */
u32 daNpc_Hi1_c::talk_1() {
    u32 ret = chk_parts_notMov();
    talk(1);
    // HD: read status from the message manager rather than mpCurrMsg.
    if (mbHasMsg == 0) {
        return TRUE;
    }
    if (Hi1_messageStatus(l_message_manager.get()) == HI1_MSG_DESTROYED) {
        mItemNo = 0xFF;
        field_0x7B7 = 0;
        setStt(field_0x7C3);
        setAnm_NUM(0, 1);
        mTimer2 = (s16)Hi1_getRndValue(0xF, 0x1E);
        endEvent();
    }
    return ret;
}

/* 0223E204 */
BOOL daNpc_Hi1_c::wait_action1(void*) {
    s8 state = field_0x7C7;
    if (state == 0) {
        setStt(HI1_WAIT);
        field_0x7B9 = 1;
        field_0x7C7 = field_0x7C7 + 1;
        return TRUE;
    }
    if ((u32)(s32)state > 3) { /* case 9 and the rest */
        return TRUE;
    }
    mHasAttention = chkAttention();
    switch ((u32)(s32)field_0x7C2) {
    case HI1_WAIT:
        field_0x790 = wait_1();
        break;
    case HI1_TALK:
        field_0x790 = talk_1();
        break;
    }
    return TRUE;
}

/* 0223CF54 */
u8 daNpc_Hi1_c::demo() {
    u8 id = demoActorID;
    if (id == 0) {
        if (field_0x7BA != 0) {
            field_0x7BA = 0;
        }
        return field_0x7BA;
    }
    if (field_0x7BA == 0) {
        m_jnt.mAngles[0][1] = 0;
        m_jnt.mAngles[1][0] = 0;
        field_0x7BA = 1;
        field_0x7B3 = 0;
        m_jnt.mAngles[0][0] = 0;
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0;
    }
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        /* dComIfGp_demo_getActor(id) */
        u32 obj = (u32)l_demo_object;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demo_object;
        }
        demo_actor = Hi1_demoGetActor(obj, id);
    }
    J3DAnmTexPattern* btp = Hi1_view<Hi1BtpAnm>(mBtpAnm)->mpAnimation.get();
    if (btp != 0) {
        u8 frame = (u8)Hi1_frameMax(btp);
        u8 f = (u8)(mBtpAnmFrame + 1);
        if (f < frame) {
            mBtpAnmFrame = f;
        } else {
            mBtpAnmFrame = frame;
        }
    }
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* p = Hi1_demoBtp(demo_actor, mArcName);
        if (p != nullptr) {
            Hi1_btpInit(mBtpAnm, mpMorf->getModel()->getModelData(), p, 1, 0, 1.0f, 0, -1, 1, 0);
            mBtpAnmFrame = 0;
            field_0x7BE = 1;
        }
    }
    J3DAnmTextureSRTKey* btk = Hi1_view<Hi1BtkAnm>(mBtkAnm)->mpAnimation.get();
    if (btk != 0) {
        u8 frame = (u8)Hi1_frameMax(btk);
        u8 f = (u8)(mBtkAnmFrame + 1);
        if (f < frame) {
            mBtkAnmFrame = f;
        } else {
            mBtkAnmFrame = frame;
        }
    }
    if (demo_actor != nullptr) {
        J3DAnmTextureSRTKey* p = Hi1_demoBtk(demo_actor, mArcName);
        if (p != nullptr) {
            Hi1_btkInit(mBtkAnm, mpMorf->getModel()->getModelData(), p, 1, 0, 1.0f, 0, -1, 1, 0);
            mBtkAnmFrame = 0;
            field_0x7BF = 1;
        }
    }
    /* ENABLE_TRANS | ENABLE_ROTATE | ENABLE_ANM | ENABLE_ANM_FRAME */
    Hi1_demoData(this, HI1_DEMO_TRANS_ROT_ANM_FRAME, mpMorf, mArcName, 0, nullptr, 0, 0);
    return field_0x7BA;
}

/* 0223D9C0 */
BOOL daNpc_Hi1_c::_draw() {
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* modelData = pModel->getModelData();
    if (field_0x7B2 != 0 || field_0x7B4 != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    // HD: retain the unused accessor call.
    dComIfGp_get();
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, modelData, mBtpAnmFrame);
    mDoExt_btkAnm_entry((mDoExt_btkAnm*)(void*)mBtkAnm, modelData, (f32)(u8)mBtkAnmFrame);
    mpMorf->entryDL();
    Hi1_view<Hi1ModelAnimation>(modelData)->mpBtkAnimation = 0; /* mBtkAnm.remove(modelData) */
    Hi1_view<Hi1ModelAnimation>(modelData)->mpBtpAnimation = 0; /* mBtpAnm.remove(modelData) */
    // HD: shadowDraw is absent.
    // HD: debug colours retain their guarded first-use initialization.
    if (l_HIO.mPrmTbl.mDebug != 0) {
        Hi1_initDebugColour(&l_red_guard, &l_red_colour, &a_red_colour);
        Hi1_initDebugColour(&l_blue_guard, &l_blue_colour, &a_blue_colour);
    }
    return TRUE;
}

/* 0223D6EC */
BOOL daNpc_Hi1_c::_execute() {
    if (field_0x7B5 == 0) {
        field_0x730.y = current.angle.y;
        field_0x730.z = current.angle.z;
        Hi1_copyWords(&field_0x724, &current.pos);
        field_0x730.x = current.angle.x;
        field_0x7B5 = 1;
    }
    daNpc_Hi1_HIO_c::hio_prm_c& prm = l_HIO.mPrmTbl;
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (field_0x7B2 != 0 && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (demo() == 0) {
        s32 temp = -1;
        bool evt = false;
        if (dComIfGp_event_runCheck() && (u16)eventInfo.mCommand != HI1_COMMAND_TALK) {
            temp = isEventEntry();
            if (temp >= 0)
                evt = true;
        }
        if (evt || field_0x7B0 != 0) {
            event_proc(temp);
        } else {
            Hi1_callAction(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, &mStts.m_cc_move);
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
        eventOrder();
        hi1_setAngle(this);
    } else {
        field_0x7B2 = 0;
        eventOrder();
        hi1_setAngle(this);
    }
    tevStr.mRoomNo = Hi1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Hi1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    setMtx(0);
    if (field_0x7BA == 0) {
        setCollision(110.0f, 260.0f);
    }
    return TRUE;
}

/* 0223CE24 */
BOOL daNpc_Hi1_c::_delete() {
    // HD: resource deletion is unconditional.
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 0223CCC4 */
cPhs_State daNpc_Hi1_c::_create() {
    // HD: install this TU's vtable and construct both enlarged animation objects.
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Hi1_npcCtor(
                this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = 0x1001A548;
            Hi1_btkCtor(mBtkAnm); /* mDoExt_btkAnm::mDoExt_btkAnm (the matcher calls it init) */
            Hi1_btpCtor(mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mStateIsComplaete = state == cPhs_COMPLEATE_e;
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }

    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, (u32)a_heap_size_tbl[field_0x7C5])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -110.0f, -20.0f, -100.0f, 110.0f, 280.0f, 100.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 0223C308 */
BOOL daNpc_Hi1_c::bodyCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, dRes_ID_HI_BDL_HI_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1487, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_hi1.cpp", 0x5CF, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        // HD: delete the failed model through its virtual deleting destructor.
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            Hi1_destroyMorf(morf);
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, 0)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("head");
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(1509, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_hi1.cpp", 0x5E5, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone1");
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(1511, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_hi1.cpp", 0x5E7, "m_bbone_jnt_num >= 0");
    /* getJointNodePointer(jnt)->setCallBack(...): a joint index out of range uses the table base */
    {
        J3DModelData* md = mpMorf->getModel()->getModelData();
        md->getJointNodePointer((u16)(s32)m_hed_jnt_num)->setCallBack(nodeCB_Head);
    }
    {
        J3DModelData* md = mpMorf->getModel()->getModelData();
        md->getJointNodePointer((u16)(s32)m_bbone_jnt_num)->setCallBack(nodeCB_BackBone);
    }
    mpMorf->getModel()->setUserArea((uintptr_t)this);
    return TRUE;
}

/* 0223C5C8 */
BOOL daNpc_Hi1_c::CreateHeap() {
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 110.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 0223CE20 */
static cPhs_State daNpc_Hi1_Create(fopAc_ac_c* i_this) { return ((daNpc_Hi1_c*)i_this)->_create(); }

/* 0223CE78 */
static BOOL daNpc_Hi1_Delete(daNpc_Hi1_c* i_this) { return i_this->_delete(); }

/* 0223D9BC */
static BOOL daNpc_Hi1_Execute(daNpc_Hi1_c* i_this) { return i_this->_execute(); }

/* 0223DB0C */
static BOOL daNpc_Hi1_Draw(daNpc_Hi1_c* i_this) { return i_this->_draw(); }

/* 0223DB10 */
static BOOL daNpc_Hi1_IsDelete(daNpc_Hi1_c*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 0223E2BC */
static daNpc_Hi1_HIO_c* daNpc_Hi1_HIO_c_ct(daNpc_Hi1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Hi1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001A404;
    memcpy_g(&i_this->mPrmTbl, &a_prm_tbl, 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 0223E328 */
static void __sinit_d_a_npc_hi1_cpp() {
    sinit_header_statics_z(0x10467158, 0x101BE7B4, 0x1046718C);
    daNpc_Hi1_HIO_c_ct(&l_HIO); /* static daNpc_Hi1_HIO_c l_HIO */
}

/* 0223E3C8 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0223E3DC */
static void daNpc_Hi1_c_dt(daNpc_Hi1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Hi1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        Hi1_view<Hi1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1001A3E4;
        Hi1_view<Hi1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1001A3F4;
        Hi1_acchDestroy(&i_this->mObjAcch, 0);
        Hi1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0223E478 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 1001A444 "d_a_npc_hi1.cpp" in 0223C0BC
str 1001A454 "a_btp != (0)" in 0223C0BC
str 1001A468 "d_a_npc_hi1.cpp" in 0223C1B4
str 1001A478 "a_btk != (0)" in 0223C1B4
str 1001A494 "d_a_npc_hi1.cpp" in 0223C308
str 1001A4B0 "a_mdl_dat != (0)" in 0223C308
str 1001A48C "head" in 0223C308
str 1001A4C4 "m_hed_jnt_num >= 0" in 0223C308
str 1001A4A4 "backbone1" in 0223C308
str 1001A4D8 "m_bbone_jnt_num >= 0" in 0223C308
str 1001A4FC "Hi1" in 0223CB30
str 1001A424 "d_demo.h" in 0223CF54
str 1001A414 "m_object != (0)" in 0223CF54
ext 024EF130 s8 Hi1_roomId(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 Hi1_polyColour(dBgS*, cBgS_PolyInfo*)
ext 02055B64 s16 Hi1_calcTimer(be<s16>* t)
ext 025E789C s32 Hi1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 024EDFCC bool Hi1_lockonTruth(dAttention_c* a)
ext 024EC8D0 fopAc_ac_c* Hi1_lockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* Hi1_actionTarget(dAttention_c* a, s32 i)
ext 0259E2D4 void Hi1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 02527028 BOOL Hi1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 02526E70 void* Hi1_demoGetActor(u32 obj, u8 id)
ext 025A1458 void Hi1_npcCtor(daNpc_Hi1_c* p)
ext 025E7820 void Hi1_btpCtor(void* p)
ext 02018034 void Hi1_circleDestroy(void* p, s32 flags)
ext 024EFD9C void Hi1_acchDestroy(dBgS_Acch* p, s32 flags)
ext 025D50BC void Hi1_actorDestroy(daNpc_Hi1_c* p, s32 flags)
ext 02527828 J3DAnmTexPattern* Hi1_demoBtp(void*,const char*)
ext 021E1E78 s32 Hi1_getRndValue(s32,s32)
ext 0259D8BC BOOL Hi1_checkAttention(fopAc_ac_c*,cXyz*,f32,f32,f32,u32)
ext 024EECAC u32 Hi1_materialSound(dBgS*,cBgS_PolyInfo*)
ext 025D54C4 BOOL Hi1_searchById(u32,be<u32>*)
ext 025E535C BOOL Hi1_playMorf(mDoExt_McaMorf*,cXyz*,u32,s32)
ext 025F795C u32 Hi1_messageStatus(void*)
ext 025E7CE0 s32 Hi1_btkInit(void* anm, J3DModelData* d, J3DAnmTextureSRTKey* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 025279C8 J3DAnmTextureSRTKey* Hi1_demoBtk(void*, const char*)
ext 025E7C6C void Hi1_btkCtor(void*)
ext 0259D54C void Hi1_playerEyePos(cXyz*, f32)
*/
