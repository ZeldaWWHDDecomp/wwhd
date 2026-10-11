/**
 * d_a_npc_km1.cpp (WWHD)
 * NPC - Mila (rich)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_km1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_km1.h"

// HD: per-TU resource-key and compiler-generated vtables.
const gc_tu_data gc_tu = {.safeString=0x1001C604};
#define KM1_VTBL 0x1001C7F8        /* daNpc_Km1_c vtable (HD: merged with fopNpc_npc_c's) */

enum { fpcNm_NPC_KM1_e = 0x14D };
enum Km1State { KM1_WAIT=1, KM1_TALK=2 };
enum Km1LookMode { KM1_LOOK_PLAYER=1, KM1_LOOK_POSITION=2, KM1_LOOK_ANGLE=3 };
enum Km1MessageStatus { KM1_MSG_TYPING=6, KM1_MSG_DISPLAYED=14, KM1_MSG_DESTROYED=19 };
enum Km1Resource { KM1_BAS_WAIT=0, KM1_BCK_WAIT=1, KM1_BDL_BODY=2 };

// HD: named original data; one owner for each TU-wide object.
static daNpc_Km1_HIO_c& l_HIO = gabi::object_at<daNpc_Km1_HIO_c>(0x10467758);
static cXyz& a_att_pos_offst = gabi::object_at<cXyz>(0x10467790);
static cXyz& a_eye_pos_offst = gabi::object_at<cXyz>(0x1046779C);
static be<u32>& l_attention_offset_guard = gabi::object_at<be<u32>>(0x104677A8);
static be<u32>& l_eye_offset_guard = gabi::object_at<be<u32>>(0x104677AC);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static Km1ActionFunc& l_wait_action = gabi::object_at<Km1ActionFunc>(0x1001C5F0);
static dCcD_SrcCyl& dNpc_cyl_source = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static be<u32>& l_red_guard = gabi::object_at<be<u32>>(0x101FDA50);
static be<u32>& l_blue_guard = gabi::object_at<be<u32>>(0x101FDAC0);
static be<u32>& l_red_colour = gabi::object_at<be<u32>>(0x101FEBF4);
static be<u32>& l_blue_colour = gabi::object_at<be<u32>>(0x101FEBF8);
static s32 a_btp_arc_ix_tbl[] = {3};
static u32 a_heap_size_tbl[] = {160480};
static s8 a_tex_pattern_num_tbl[] = {0};
static u8 a_prm_tbl[] = {31,254,56,224,224,2,199,32,0,0,0,0,0,0,0,0,8,0,8,0,67,22,0,0,0,0,0,0};
static u32 a_red_colour[] = {4278190208};
static u32 a_blue_colour[] = {65408};
static const char* cut_name_tbl[] = {"ACTION"};
static daNpc_Km1_c::anm_prm_c a_anm_prm_tbl[] = {
    {-1,-1,0.0f,0.0f,-1},
    {0,0,8.0f,1.0f,2},
    {-1,-1,0.0f,0.0f,-1}
};
static daNpc_Km1_c::anm_prm_c a_anm_attr_tbl[] = {
    {0,0,8.0f,1.0f,2}
};

/* daNpc_Km1_HIO_c, HD: the vtable pointer is last (+0x24) */
/* 02272FB0 */
static daNpc_Km1_HIO_c* daNpc_Km1_HIO_c_ct(daNpc_Km1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Km1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C63C;
    memcpy_g(&i_this->mPrmTbl, a_prm_tbl, 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 022713C0 */
static BOOL nodeCallBack_Km(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Km1_c* km1Actor = static_cast<daNpc_Km1_c*>(model->getUserArea());
        if (km1Actor) {
            /* static cXyz a_att_pos_offst(0, 0, 0): guard 0x104677A8, object 0x10467790 */
            if ((u32)l_attention_offset_guard == 0) {
                a_att_pos_offst.x = 0.0f;
                a_att_pos_offst.z = 0.0f;
                l_attention_offset_guard = 1;
                a_att_pos_offst.y = 0.0f;
            }
            /* static cXyz a_eye_pos_offst(20, 20, 0): guard 0x104677AC, object 0x1046779C */
            if ((u32)l_eye_offset_guard == 0) {
                a_eye_pos_offst.z = 0.0f;
                l_eye_offset_guard = 1;
                a_eye_pos_offst.y = 20.0f;
                a_eye_pos_offst.x = 20.0f;
            }
            u32 jointIdx = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(model->getAnmMtx(jointIdx), stk);
            if (jointIdx == (u32)(s32)km1Actor->m_head_jnt_num) {
                PSMTXMultVec(stk, &a_att_pos_offst, &km1Actor->mAttPos);
                mDoMtx_XrotM(stk, km1Actor->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stk, km1Actor->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stk, &a_eye_pos_offst, &km1Actor->mEyePos);
            } else if (jointIdx == (u32)(s32)km1Actor->m_backbone_jnt_num) {
                mDoMtx_XrotM(stk, km1Actor->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stk, km1Actor->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(stk, J3DSys::mCurrentMtx);
            mtx_copy(model->getAnmMtx(jointIdx), stk);
        }
    }
    return TRUE;
}

/* 02271E78 */
bool daNpc_Km1_c::createInit() {
    mEventCut.setActorInfo2("Km1" /* "Km1" */, (fopNpc_npc_c*)(void*)this);
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    gravity = -4.0f;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB; /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    field_0x798.copy(current.pos);
    gabi::Local<Km1ActionFunc> pmf;
    Km1_loadAction(pmf, &l_wait_action);
    set_action(pmf, nullptr);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_source);
    mpMorf->setMorf(0.0f);
    field_0x7BC = 1;
    setMtx();
    return true;
}

/* 02271D1C */
void daNpc_Km1_c::setMtx() {
    if (field_0x7C7 == 0) {
        playTexPatternAnm();
        field_0x7B4 = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < field_0x7A4) {
            field_0x7B4 = 1;
        }
        field_0x7A4 = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = Km1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = Km1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention();
}

/* 02272980 */
bool daNpc_Km1_c::anmResID(int i_num, be<s32>* o_bck_num, be<s32>* o_bas_num) {
    /* a_anm_idx_tbl[1][2] = {dRes_ID_KM_BCK_KM_WAIT01_e (1), dRes_ID_KM_BAS_KM_WAIT01_e (0)} */
    if (i_num != 0) /* JUT_ASSERT(285, 0 <= i_num && i_num < ANM_END) */
        JUT_ASSERT_fail("d_a_npc_km1.cpp", 0x11D, "0 <= i_num && i_num < ANM_END");
    if (o_bck_num == nullptr || o_bas_num == nullptr) /* JUT_ASSERT(286, o_bck_num && o_bas_num) */
        JUT_ASSERT_fail("d_a_npc_km1.cpp", 0x11E, "o_bck_num && o_bas_num");
    *o_bck_num = KM1_BCK_WAIT;
    *o_bas_num = KM1_BAS_WAIT;
    return true;
}

/* 02271624 */
void daNpc_Km1_c::BtpNum2ResID(int i_num, be<s32>* o_btp_num) {
    if (i_num != 0) /* JUT_ASSERT(307, 0 <= i_num && i_num < TEXPATTERN_END) */
        JUT_ASSERT_fail("d_a_npc_km1.cpp", 0x133, "0 <= i_num && i_num < TEXPATTERN_END");
    *o_btp_num = a_btp_arc_ix_tbl[0]; /* a_btp_arc_ix_tbl[0] */
}

/* 02272A10 */
u32 daNpc_Km1_c::setAnm_tex(s8 i_param_1) {
    if (i_param_1 >= 0 && i_param_1 != field_0x7CD) {
        field_0x7CD = i_param_1;
        return initTexPatternAnm(true);
    }
    // HD: the unchanged-animation path returns the original this register.
    return (u32)gabi::ea(this);
}

/* 02271678 */
bool daNpc_Km1_c::init_btp(u32 param_1, int param_2) {
    J3DModelData* modelData = mpMorf->getModel()->getModelData();
    if (param_2 >= 0) {
        gabi::Local<be<s32>> btpId;
        BtpNum2ResID(param_2, btpId);
        const char* key_arc = "Km"; /* "Km" */
        m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(key_arc, *btpId);
        if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(340, m_head_tex_pattern != NULL) */
            JUT_ASSERT_fail("d_a_npc_km1.cpp", 0x154, "m_head_tex_pattern != (0)");
        if (Km1_btpInit(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, FALSE) == 0) {
            return false;
        }
        field_0x6F2 = 0;
        mBtpFrame = 0;
    }
    return true;
}

/* 0227178C */
bool daNpc_Km1_c::initTexPatternAnm(u32 param_1) {
    bool initialized = false;
    if (init_btp(param_1, field_0x7CD)) {
        initialized = true;
    }
    return initialized;
}

/* 02271BD0 */
void daNpc_Km1_c::playTexPatternAnm() {
    if (field_0x7CD == 0 && Km1_calcTimer(&field_0x6F2) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < Km1_frameMax(m_head_tex_pattern)) {
        return;
    }
    if (field_0x7CD) {
        mBtpFrame = (u8)Km1_frameMax(m_head_tex_pattern);
    } else {
        mBtpFrame = 0;
        field_0x6F2 = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
    }
}

/* 02272A34 */
s32 daNpc_Km1_c::setAnm_anm(anm_prm_c* i_anm_ptr) {
    s32 changed = 0;
    s8 num = i_anm_ptr->bckNum;
    if (num >= 0 && field_0x7CE != num) {
        field_0x7CE = num;
        if (mpMorf.get() != nullptr) {
            gabi::Local<be<s32>> bckId;
            gabi::Local<be<s32>> basId;
            anmResID(num, bckId, basId);
            if (*bckId >= 0) {
                dNpc_setAnmIDRes(mpMorf, i_anm_ptr->loopMode, i_anm_ptr->morf, i_anm_ptr->speed, *bckId, *basId, "Km" /* "Km" */);
            }
            changed = 1;
        }
        field_0x7B5 = 0;
        field_0x7A4 = 0.0f;
        field_0x7B4 = 0;
    }
    return changed;
}

/* 02272AF0 */
void daNpc_Km1_c::setAnm() {
    setAnm_tex(a_anm_prm_tbl[field_0x7D0].btpNum);
    setAnm_anm(&a_anm_prm_tbl[field_0x7D0]);
}

/* 02272BC0 */
void daNpc_Km1_c::chngAnmAtr(u32 param_1) {
    if (param_1 < 1 && param_1 != field_0x7CB) {
        field_0x7CB = param_1;
        setAnm_ATR(1);
    }
}

/* 02272B5C */
void daNpc_Km1_c::setAnm_ATR(int param_1) {
    anm_prm_c* a_anm_prm_tbl = a_anm_attr_tbl;
    if (param_1 != 0) {
        /* HD: the one-entry table is indexed with 0 here */
        setAnm_tex(a_anm_prm_tbl[0].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[field_0x7CB]);
}

/* 02272BE4 */
void daNpc_Km1_c::anmAtr(u16 i_msgStatus) {
    if (i_msgStatus == KM1_MSG_TYPING) {
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
    } else if (i_msgStatus == KM1_MSG_DISPLAYED) {
        field_0x7D6 = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}

/* 02272CA4 */
void daNpc_Km1_c::setStt(s8 param_1) {
    s8 uVar1 = field_0x7D0;
    field_0x7D0 = param_1;
    if (param_1 == KM1_TALK) {
        field_0x7D1 = uVar1;
        field_0x7CB = 0xFF;
        field_0x7D2 = 1;
        return;
    }
    setAnm();
}

/* 02272CD4 */
u16 daNpc_Km1_c::next_msgStatus(be<u32>*) {
    return fopMsgStts_MSG_ENDS_e;
}

/* 02272CDC */
u32 daNpc_Km1_c::getMsg() {
    return 0;
}

/* 02272668 */
void daNpc_Km1_c::eventOrder() {
    if (field_0x7CF == 1 || field_0x7CF == 2) {
        s8 v = field_0x7CF;
        eventInfo.onCondition(dEvtCnd_CANTALK_e); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (v == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 0227218C */
void daNpc_Km1_c::checkOrder() {
    u16 command = eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        return;
    }
    if (command != 1 /* dEvtCmd_INTALK_e */) {
        return;
    }
    if (field_0x7CF != 1 && field_0x7CF != 2) {
        return;
    }
    field_0x7CF = 0;
    field_0x7C5 = 1;
}

/* 02272424 */
void daNpc_Km1_c::lookBack() {
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
    case KM1_LOOK_PLAYER: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, -20.0f);
        srcX = current.pos.x;
        vec1->copy(*eye);
        srcY = eyePos.y;
        dstPos = vec1;
        srcZ = current.pos.z;
        break;
    }
    case KM1_LOOK_POSITION:
        vec1->copy(field_0x798);
        srcZ = current.pos.z;
        srcX = current.pos.x;
        dstPos = vec1;
        break;
    case KM1_LOOK_ANGLE:
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
    Km1_lookAtTarget(&m_jnt, &current.angle.y, dstPos, vec2, targetY, field_0x7B0, headOnlyFollow);
}

/* 02272CE4 */
u8 daNpc_Km1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (Km1_lockonTruth(attention)) {
        return this == (void*)Km1_lockonTarget(attention, 0);
    }
    return this == (void*)Km1_actionTarget(attention, 0);
}

/* 02271CBC */
void daNpc_Km1_c::setAttention() {
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

/* 02271A7C */
bool daNpc_Km1_c::decideType(int i_type) {
    field_0x7D3 = -1;
    if (fpcM_GetName(this) == fpcNm_NPC_KM1_e) {
        field_0x7D4 = 0;
        field_0x7D3 = 0;
    }
    return true;
}

/* 022722A4 */
void daNpc_Km1_c::event_actionInit(int param_1) {
    be<s32>* puVar1 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, "ActNo" /* "ActNo" */);
    dComIfGp_evmng_getMyIntegerP(param_1, "Timer" /* "Timer" */);
    if (puVar1 != nullptr) {
        field_0x7CA = (s8)(s32)*puVar1;
    }
}

/* 0227232C */
bool daNpc_Km1_c::event_action() {
    return true;
}

/* 02272334 */
void daNpc_Km1_c::privateCut() {
    /* cut_name_tbl (.data 0x101BF6BC): "ACTION" */
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Km1" /* "Km1" */, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    field_0x7C9 = dComIfGp_evmng_getMyActIdx(staffIdx, (gptr<char>*)cut_name_tbl, 1, TRUE, 0);
    if (field_0x7C9 == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        if (field_0x7C9 == 0) {
            event_actionInit(staffIdx);
        }
    }
    bool bVar1 = true;
    if (field_0x7C9 == 0) {
        bVar1 = event_action();
    }
    if (bVar1) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}

/* 02272D6C */
void daNpc_Km1_c::endEvent() {
    dComIfGp_event_reset();
    field_0x7CB = 0xFF;
}

/* 02272608 */
void daNpc_Km1_c::event_proc() {
    if (!mEventCut.cutProc()) {
        privateCut();
    }
    lookBack();
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}

/* 02271AA4 */
bool daNpc_Km1_c::set_action(Km1ActionFunc* i_action, void* param_2) {
    Km1ActionFunc* cur = &mAction;
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
        Km1_callAction(this, cur, param_2);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x7D5 = 0;
    Km1_callAction(this, cur, param_2);
    return true;
}

/* 02272DAC */
BOOL daNpc_Km1_c::wait01() {
    if (field_0x7C5 != 0) {
        field_0x7B7 = 0xFF;
        if (dComIfGp_event_chkTalkXY()) {
            if (dComIfGp_evmng_ChkPresentEnd() == 0) {
                return TRUE;
            }
            field_0x7B7 = dComIfGp_event_getPreItemNo();
        }
        setStt(KM1_TALK);
    } else {
        field_0x7CF = 2;
        if (field_0x7C4) {
            field_0x7D2 = 1;
        } else {
            s16 y = field_0x76C.y;
            field_0x7D2 = 3;
            field_0x7B2 = y;
            m_jnt.mbTrn = 1; /* setTrn() */
        }
    }
    return TRUE;
}

/* 02272E6C */
BOOL daNpc_Km1_c::talk01() {
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg) {
        if ((int)fopNpc_npc_c::mpCurrMsg->mStatus == KM1_MSG_DESTROYED) {
            field_0x7B7 = 0xFF;
            setStt(field_0x7D1);
            field_0x7C8 = 0;
            field_0x7C5 = 0;
            endEvent();
        }
    }
    return TRUE;
}

/* 02272EEC */
BOOL daNpc_Km1_c::wait_action1(void*) {
    if (field_0x7D5 == 0) {
        setStt(KM1_WAIT);
        field_0x7D5 = field_0x7D5 + 1;
    } else if (field_0x7D5 != -1) {
        field_0x7C4 = chkAttention();
        switch ((u32)(s32)field_0x7D0) {
        case KM1_TALK:
            field_0x7C0 = talk01();
            break;
        case KM1_WAIT:
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

/* 022721CC */
u8 daNpc_Km1_c::demo() {
    if (demoActorID == 0) {
        if (field_0x7C7 != 0) {
            field_0x7C7 = 0;
        }
    } else {
        u8 id = demoActorID;
        field_0x7C7 = 1;
        /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check; result unused) */
        if (id != 0 && id <= 0x20) {
            u32 obj = (u32)l_demo_object;
            if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
                JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
                obj = (u32)l_demo_object;
            }
            Km1_demoGetActor(obj, id);
        }
        Km1_demoData(this, 0x6A, mpMorf, "Km" /* "Km" */, 0, nullptr, 0, 0);
    }
    return field_0x7C7;
}

/* 02272858 */
BOOL daNpc_Km1_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = model->getModelData();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBtpFrame);
    mpMorf->entryDL();
    Km1_view<Km1ModelAnimation>(model_data)->mpBtpAnimation = 0; /* mBtpAnm.remove() */
    /* HD: no shadow model, no real/simple shadow */
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO.mPrmTbl.field_18) {
        Km1_initDebugColour(&l_red_guard, &l_red_colour, Km1_view<be<u32>>(a_red_colour));
        Km1_initDebugColour(&l_blue_guard, &l_blue_colour, Km1_view<be<u32>>(a_blue_colour));
    }
    dSnap_RegistFig(0x58 /* DSNAP_TYPE_NPC_KM1_KK1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 022726A0 */
BOOL daNpc_Km1_c::_execute() {
    if (field_0x7B8 == 0) {
        field_0x774.copy(current.pos);
        field_0x76C.x = current.angle.x;
        field_0x76C.y = current.angle.y;
        field_0x76C.z = current.angle.z;
        field_0x7B8 = 1;
    }
    daNpc_Km1_HIO_c::hio_prm_c& prm = l_HIO.mPrmTbl;
    m_jnt.setParam(prm.field_8, prm.field_A, prm.field_C, prm.field_E, prm.field_0, prm.field_2, prm.field_4,
                   prm.field_6, prm.field_10);
    checkOrder();
    if (demo() == 0) {
        if (dComIfGp_event_runCheck() && eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */) {
            event_proc();
        } else {
            Km1_callAction(this, &mAction, nullptr); /* (this->*field_0x6F4)(NULL) */
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
    setCollision(60.0f, 150.0f);
    return true;
}

/* 02272110 */
BOOL daNpc_Km1_c::_delete() {
    /* HD: no fopAcM_RegisterDeleteID; dComIfG_resDelete instead of resDeleteDemo */
    dComIfG_resDelete(&mPhs, "Km" /* "Km" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    if (l_HIO.field_0x8 >= 0) {
        s32 n = l_HIO.field_0x8 - 1;
        l_HIO.field_0x8 = n;
        if (n < 0) {
            mDoHIO_deleteChild(l_HIO.mNo);
        }
    }
    return true;
}

/* 02271A78 */
static BOOL CheckCreateHeap(fopAc_ac_c* actor) {
    return ((daNpc_Km1_c*)actor)->CreateHeap();
}

/* 02271F84 */
cPhs_State daNpc_Km1_c::_create() {
    /* fopAcM_ct(this, daNpc_Km1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            Km1_npcCtor(this);          /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = KM1_VTBL;
            Km1_btpCtor(mBtpAnm);       /* mDoExt_btpAnm::mDoExt_btpAnm */
            Km1_eventCutCtor(&mEventCut);    /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State resLoadResult = dComIfG_resLoad(&mPhs, "Km" /* "Km" */);
    if (resLoadResult != cPhs_COMPLEATE_e) {
        return resLoadResult;
    }
    if (l_HIO.field_0x8 < 0) {
        l_HIO.mNo = mDoHIO_createChild("\u008b\u00e0\u008e\u009d\u0082\u00bf\u0083\u0080\u0081|\u0083\u008b" /* "金持ちム−ル" */, &l_HIO);
    }
    l_HIO.field_0x8 = l_HIO.field_0x8 + 1;
    /* a_heap_size_tbl (.data 0x101BF6B8; HD: always entry 0) */
    if (!fopAcM_entrySolidHeap(this, 0x02271A78 /* CheckCreateHeap */, a_heap_size_tbl[0])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -20.0f, -80.0f, 60.0f, 160.0f, 60.0f);
    if (createInit() == 0) {
        resLoadResult = cPhs_ERROR_e;
    }
    return resLoadResult;
}

/* 022717D0 */
BOOL daNpc_Km1_c::CreateHeap() {
    J3DModelData* a_mdl_data = (J3DModelData*)dComIfG_getObjectIDRes("Km" /* "Km" */, KM1_BDL_BODY);
    if (a_mdl_data == nullptr) /* JUT_ASSERT(1335, a_mdl_data != NULL) */
        JUT_ASSERT_fail("d_a_npc_km1.cpp", 0x537, "a_mdl_data != (0)");
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes("Km", KM1_BCK_WAIT);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_data, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020002);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() != nullptr) {
        m_head_jnt_num = (s8)a_mdl_data->getJointName()->getIndex( "head" /* "head" */);
        if (m_head_jnt_num < 0)
            JUT_ASSERT_fail("d_a_npc_km1.cpp", 0x54B, "m_head_jnt_num >= 0");
        m_backbone_jnt_num = (s8)a_mdl_data->getJointName()->getIndex( "backbone" /* "backbone" */);
        if (m_backbone_jnt_num < 0)
            JUT_ASSERT_fail("d_a_npc_km1.cpp", 0x54D, "m_backbone_jnt_num >= 0");
        field_0x7CD = a_tex_pattern_num_tbl[0]; /* a_tex_pattern_num_tbl[0] (HD: always entry 0) */
        if (initTexPatternAnm(false) != 0) {
            /* HD: no shadow model */
            for (u16 i = 0; i < a_mdl_data->getJointNum(); i++) {
                if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
                    /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Km) */
                    J3DModelData* md = mpMorf->getModel()->getModelData();
                    md->getJointNodePointer(i)->setCallBack(nodeCallBack_Km);
                }
            }
            mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
            mAcchCir.SetWall(30.0f, 60.0f);
            mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
            return TRUE;
        }
    }
    mpMorf = nullptr;
    return FALSE;
}

/* 0227210C */
static cPhs_State daNpc_Km1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Km1_c*)i_this)->_create();
}

/* 02272188 */
static BOOL daNpc_Km1_Delete(daNpc_Km1_c* i_this) {
    return i_this->_delete();
}

/* 02272854 */
static BOOL daNpc_Km1_Execute(daNpc_Km1_c* i_this) {
    return i_this->_execute();
}

/* 02272974 */
static BOOL daNpc_Km1_Draw(daNpc_Km1_c* i_this) {
    return i_this->_draw();
}

/* 02272978 */
static BOOL daNpc_Km1_IsDelete(daNpc_Km1_c*) {
    return TRUE;
}

/* 0227301C */
static void __sinit_d_a_npc_km1_cpp() {
    sinit_header_statics_z(0x1046774C, 0x101BF71C, 0x10467780);
    daNpc_Km1_HIO_c_ct(&l_HIO); /* static daNpc_Km1_HIO_c l_HIO */
}

/* 022730BC */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 022730D0 */
static void daNpc_Km1_c_dt(daNpc_Km1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Km1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Km1_view<Km1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1001C61C;
        Km1_view<Km1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1001C62C;
        Km1_acchDestroy(&i_this->mObjAcch, 0);
        Km1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0227316C */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue
ext 024EF130 s8 Km1_roomId(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 Km1_polyColour(dBgS*, cBgS_PolyInfo*)
data a_btp_arc_ix_tbl 1001C670
data a_heap_size_tbl 101BF6B8
data a_tex_pattern_num_tbl 101BF6B4
data a_prm_tbl 101BF700
data a_red_colour 1001C5F8
data a_blue_colour 1001C5FC
data cut_name_tbl 101BF6BC
data a_anm_prm_tbl 101BF6C0
data a_anm_attr_tbl 101BF6F0
str 1001C754 "Km1" in 02271E78
str 1001C7A4 "d_a_npc_km1.cpp" in 02272980
str 1001C7B4 "0 <= i_num && i_num < ANM_END" in 02272980
str 1001C7D4 "o_bck_num && o_bas_num" in 02272980
str 1001C674 "d_a_npc_km1.cpp" in 02271624
str 1001C684 "0 <= i_num && i_num < TEXPATTERN_END" in 02271624
str 1001C6B0 "Km" in 02271678
str 1001C6B4 "d_a_npc_km1.cpp" in 02271678
str 1001C6C4 "m_head_tex_pattern != (0)" in 02271678
str 1001C7EC "Km" in 02272A34
str 1001C784 "ActNo" in 022722A4
str 1001C78C "Timer" in 022722A4
str 1001C794 "Km1" in 02272334
str 1001C64C "m_object != (0)" in 022721CC
str 1001C65C "d_demo.h" in 022721CC
str 1001C77F "Km" in 022721CC
str 1001C77C "Km" in 02272110
str 1001C768 "Km" in 02271F84
str 1001C76C "\u008b\u00e0\u008e\u009d\u0082\u00bf\u0083\u0080\u0081|\u0083\u008b" in 02271F84
str 1001C6E8 "Km" in 022717D0
str 1001C6EC "head" in 022717D0
str 1001C6F4 "d_a_npc_km1.cpp" in 022717D0
str 1001C704 "m_backbone_jnt_num >= 0" in 022717D0
str 1001C71C "m_head_jnt_num >= 0" in 022717D0
str 1001C730 "a_mdl_data != (0)" in 022717D0
str 1001C744 "backbone" in 022717D0
ext 02055B64 s16 Km1_calcTimer(be<s16>* t)
ext 025E789C s32 Km1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 024EDFCC bool Km1_lockonTruth(dAttention_c* a)
ext 024EC8D0 fopAc_ac_c* Km1_lockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* Km1_actionTarget(dAttention_c* a, s32 i)
ext 0259DED0 void Km1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 02527028 BOOL Km1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 02526E70 void* Km1_demoGetActor(u32 obj, u8 id)
ext 025A1458 void Km1_npcCtor(daNpc_Km1_c* p)
ext 025E7820 void Km1_btpCtor(void* p)
ext 0259F740 void Km1_eventCutCtor(dNpc_EventCut_c* p)
ext 02018034 void Km1_circleDestroy(void* p, s32 flags)
ext 024EFD9C void Km1_acchDestroy(dBgS_Acch* p, s32 flags)
ext 025D50BC void Km1_actorDestroy(daNpc_Km1_c* p, s32 flags)
*/
