/**
 * d_a_npc_pm1.cpp (WWHD)
 * NPC - Maggie (poor)
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx), which is d_a_npc_km1's (Mila, rich) with other constants, so this file
 * follows the GameCube KM1 names and the verified PM1 behavior. HD differences are marked below.
 */
#include "d/actor/d_a_npc_pm1.h"

// HD: translation-unit copies of the resource-key and compiler-generated vtables.
const gc_tu_data gc_tu = {
    .safeString = 0x1002103C,
    .safeStringAssure = 0x022D2A5C,
    .objAcchDt = {0x10021054, 0x10021064},
    .headerStatics = {0x1046843C, 0x101C53C8, 0x10468470},
};
enum : u32 { PM1_VTBL = 0x10021240, PM1_HIO_VTBL = 0x10021074 };

enum { fpcNm_NPC_PM1_e = 0x162 };

/* ---- file statics ---- */
/* daNpc_Pm1_HIO_c, HD: the vtable pointer is last (+0x24) */
static daNpc_Pm1_HIO_c l_HIO;

// HD: checked GHS descriptor words for wait_action1.
static u32 a_wait_action[2] = {0x0000FFFF, 0x022D27DC};
static daNpc_Pm1_HIO_c::hio_prm_c a_prm_tbl = {8190, 14560, -8190, -14560, 0,     0,
                                               0,    0,     2048,  2048,   150.0, 0};
static cXyz a_att_pos_offst;
static cXyz a_eye_pos_offst;
static u32 a_att_guard;
static u32 a_eye_guard;
static dCcD_SrcCyl dNpc_cyl_src;
static s32 a_anm_idx_tbl[2][2] = {{5, 2}, {4, 1}};
static s32 a_btp_arc_ix_tbl[1] = {7};
static daNpc_Pm1_c::anm_prm_c a_anm_prm_tbl[3] = {
    {-1, -1, 0.0, 0.0, -1}, {0, 0, 8.0, 1.0, 2}, {-1, -1, 0.0, 0.0, -1}};
static daNpc_Pm1_c::anm_prm_c a_atr_prm_tbl[1] = {{0, 0, 8.0, 1.0, 2}};
static const char* cut_name_tbl[1] = {"ACTION"};
static u32 a_heap_size_tbl[1] = {160480};
static s8 a_tex_pattern_num_tbl[1] = {0};
static u32 a_red_source = 0xFF000080;
static u32 a_blue_source = 0x0000FF80;
static u32 a_red_guard;
static u32 a_blue_guard;
static u32 a_red;
static u32 a_blue;
typedef gptr<void> Pm1DemoPointer;
static Pm1DemoPointer demoObject;

// HD: the constructor allocates when its incoming object is null.
/* 022D28A0 */
static daNpc_Pm1_HIO_c *daNpc_Pm1_HIO_c_ct(daNpc_Pm1_HIO_c *i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Pm1_HIO_c *)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = PM1_HIO_VTBL;
    memcpy_g(&i_this->mPrmTbl, &a_prm_tbl, 0x1C);
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 022D0C7C */
static BOOL nodeCallBack_Pm(J3DNode *i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel *model = j3dSys.getModel();
        daNpc_Pm1_c *pm1Actor = static_cast<daNpc_Pm1_c *>(model->getUserArea());
        if (pm1Actor) {
            cXyz *attOffset = &a_att_pos_offst;
            if (a_att_guard == 0) {
                attOffset->x = 0.0f;
                attOffset->z = 0.0f;
                a_att_guard = 1;
                attOffset->y = 0.0f;
            }
            cXyz *eyeOffset = &a_eye_pos_offst;
            if (a_eye_guard == 0) {
                eyeOffset->z = 0.0f;
                a_eye_guard = 1;
                eyeOffset->x = 20.0f;
                eyeOffset->y = -25.0f;
            }
            u32 jointIdx = reinterpret_cast<J3DJoint *>(i_node)->getJntNo();
            Mtx34 *stk = mDoMtx_stack_c::get();
            PSMTXCopy(model->getAnmMtx(jointIdx), stk);
            if (jointIdx == (u32)(s32)pm1Actor->m_head_jnt_num) {
                PSMTXMultVec(stk, attOffset, &pm1Actor->mAttPos);
                // HD: PM1 rotates the head about Y and negates both angles.
                mDoMtx_YrotM(stk, (s16)-pm1Actor->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stk, (s16)-pm1Actor->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stk, eyeOffset, &pm1Actor->mEyePos);
            } else if (jointIdx == (u32)(s32)pm1Actor->m_backbone_jnt_num) {
                mDoMtx_XrotM(stk, pm1Actor->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stk, pm1Actor->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(stk, J3DSys::mCurrentMtx);
            mtx_copy(model->getAnmMtx(jointIdx), stk);
        }
    }
    return TRUE;
}

/* 022D174C */
bool daNpc_Pm1_c::createInit() {
    mEventCut.setActorInfo2("Pm1", (fopNpc_npc_c *)(void *)this);
    attention_info.flags = 0xA; /* attention_info.flags */
    gravity = -4.0f;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    field_0x798.copy(current.pos);
    gabi::Local<Pm1Action> pmf; // HD: eight-byte outgoing action descriptor.
    pm1_pmf_load(pmf, a_wait_action);
    set_action(pmf, nullptr);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    field_0x7BC = 1;
    setMtx();
    return true;
}

/* 022D15F0 */
void daNpc_Pm1_c::setMtx() {
    if (field_0x7C7 == 0) {
        playTexPatternAnm();
        field_0x7B4 = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < field_0x7A4) {
            field_0x7B4 = 1;
        }
        field_0x7A4 = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo =
        pm1_dBgS_GetRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo *>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        pm1_dBgS_GetPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo *>(&mObjAcch.m_gnd));
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention();
}

/* 022D2250 */
bool daNpc_Pm1_c::anmResID(int i_num, be<s32> *o_bck_num, be<s32> *o_bas_num) {
    // HD: PM1 has two animations.
    if ((u32)i_num >= 2) /* JUT_ASSERT(286, 0 <= i_num && i_num < ANM_END) */
        JUT_ASSERT_fail("d_a_npc_pm1.cpp", 0x11E, "0 <= i_num && i_num < ANM_END");
    if (o_bck_num == nullptr || o_bas_num == nullptr) /* JUT_ASSERT(287, o_bck_num && o_bas_num) */
        JUT_ASSERT_fail("d_a_npc_pm1.cpp", 0x11F, "o_bck_num && o_bas_num");
    *o_bck_num = a_anm_idx_tbl[i_num][0];
    *o_bas_num = a_anm_idx_tbl[i_num][1];
    return true;
}

/* 022D0EF8 */
void daNpc_Pm1_c::BtpNum2ResID(int i_num, be<s32> *o_btp_num) {
    if (i_num != 0) /* JUT_ASSERT(308, 0 <= i_num && i_num < TEXPATTERN_END) */
        JUT_ASSERT_fail("d_a_npc_pm1.cpp", 0x134, "0 <= i_num && i_num < TEXPATTERN_END");
    *o_btp_num = a_btp_arc_ix_tbl[0]; /* a_btp_arc_ix_tbl[0] */
}

/* 022D2300 */
u32 daNpc_Pm1_c::setAnm_tex(s8 i_param_1) {
    if (i_param_1 >= 0 && i_param_1 != field_0x7CD) {
        field_0x7CD = i_param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 022D0F4C */
bool daNpc_Pm1_c::init_btp(u32 param_1, int param_2) {
    J3DModelData *modelData = mpMorf->getModel()->getModelData();
    if (param_2 >= 0) {
        gabi::Local<be<s32>> btpId; // HD: four-byte resource output slot.
        BtpNum2ResID(param_2, btpId);
        const char *key_arc = "Pm";
        m_head_tex_pattern = (J3DAnmTexPattern *)dComIfG_getObjectIDRes(key_arc, *btpId);
        if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(341, m_head_tex_pattern != NULL) */
            JUT_ASSERT_fail("d_a_npc_pm1.cpp", 0x155, "m_head_tex_pattern != (0)");
        if (pm1_mDoExt_btpAnm_init(&mBtpAnm, modelData, m_head_tex_pattern, TRUE,
                                   2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, FALSE) == 0) {
            return false;
        }
        field_0x6F2 = 0;
        mBtpFrame = 0;
    }
    return true;
}

/* 022D1060 */
bool daNpc_Pm1_c::initTexPatternAnm(u32 param_1) {
    bool success = false;
    if (init_btp(param_1, field_0x7CD)) {
        success = true;
    }
    return success;
}

/* 022D14A4 */
void daNpc_Pm1_c::playTexPatternAnm() {
    if (field_0x7CD == 0 && cLib_calcTimer(&field_0x6F2) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < pm1_J3DAnm_getFrameMax(m_head_tex_pattern)) {
        return;
    }
    if (field_0x7CD) {
        mBtpFrame = (u8)pm1_J3DAnm_getFrameMax(m_head_tex_pattern);
    } else {
        mBtpFrame = 0;
        field_0x6F2 = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
    }
}

/* 022D2324 */
s32 daNpc_Pm1_c::setAnm_anm(anm_prm_c *i_anm_ptr) {
    s32 changed = 0;
    s8 num = i_anm_ptr->bckNum;
    if (num >= 0 && field_0x7CE != num) {
        field_0x7CE = num;
        if (mpMorf.get() != nullptr) {
            gabi::Local<be<s32>> bckId; // HD: separate four-byte animation output slots.
            gabi::Local<be<s32>> basId;
            anmResID(num, bckId, basId);
            if (*bckId >= 0) {
                dNpc_setAnmIDRes(mpMorf, i_anm_ptr->loopMode, i_anm_ptr->morf, i_anm_ptr->speed,
                                 *bckId, *basId, "Pm");
            }
            changed = 1;
        }
        field_0x7B5 = 0;
        field_0x7A4 = 0.0f;
        field_0x7B4 = 0;
    }
    return changed;
}

/* 022D23E0 */
void daNpc_Pm1_c::setAnm() {
    setAnm_tex(a_anm_prm_tbl[field_0x7D0].btpNum);
    setAnm_anm(&a_anm_prm_tbl[field_0x7D0]);
}

/* 022D24B0 */
void daNpc_Pm1_c::chngAnmAtr(u32 param_1) {
    if (param_1 < 1 && param_1 != field_0x7CB) {
        field_0x7CB = param_1;
        setAnm_ATR(1);
    }
}

/* 022D244C */
void daNpc_Pm1_c::setAnm_ATR(int param_1) {
    if (param_1 != 0) {
        // HD: the one-entry table is indexed with 0 here
        setAnm_tex(a_atr_prm_tbl[0].btpNum);
    }
    setAnm_anm(&a_atr_prm_tbl[field_0x7CB]);
}

/* 022D24D4 */
void daNpc_Pm1_c::anmAtr(u16 i_msgStatus) {
    if (i_msgStatus == MSG_TYPING) {
        if (field_0x7D6 == 0) {
            field_0x7CC = 0xFF;
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            field_0x7D6 = field_0x7D6 + 1;
        }
        u8 value = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        if (value != 0xFF && value != field_0x7CC) {
            dComIfGp_clearMesgAnimeTagInfo();
            field_0x7CC = value;
            /* chngAnmTag(): empty */
        }
    } else if (i_msgStatus == MSG_DISPLAYED) {
        field_0x7D6 = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}

/* 022D2594 */
void daNpc_Pm1_c::setStt(s8 param_1) {
    s8 value = field_0x7D0;
    field_0x7D0 = param_1;
    if (param_1 == STATE_TALK) {
        field_0x7D1 = value;
        field_0x7CB = 0xFF;
        field_0x7D2 = LOOK_PLAYER;
        return;
    }
    setAnm();
}

/* 022D25C4 */
u16 daNpc_Pm1_c::next_msgStatus(be<u32> *) { return fopMsgStts_MSG_ENDS_e; }

/* 022D25CC */
u32 daNpc_Pm1_c::getMsg() { return 0; }

/* 022D1F38 */
void daNpc_Pm1_c::eventOrder() {
    if (field_0x7CF == 1 || field_0x7CF == 2) {
        s8 v = field_0x7CF;
        eventInfo.onCondition(dEvtCnd_CANTALK_e); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (v == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 022D1A5C */
void daNpc_Pm1_c::checkOrder() {
    u16 command = eventInfo.getCommand(); /* eventInfo.mCommand */
    if (command == dEvtCmd_INDEMO_e) {
        return;
    }
    if (command != dEvtCmd_INTALK_e) {
        return;
    }
    if (field_0x7CF != 1 && field_0x7CF != 2) {
        return;
    }
    field_0x7CF = 0;
    field_0x7C5 = 1;
}

/* 022D1CF4 */
void daNpc_Pm1_c::lookBack() {
    cXyz vec1;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    vec1.z = 0.0f;
    vec1.x = 0.0f;
    u8 headOnlyFollow = mHeadOnlyFollow;
    f32 srcX = current.pos.x;
    vec1.y = 0.0f;
    s16 targetY = current.angle.y;
    cXyz *dstPos = nullptr;
    switch ((u32)(s32)field_0x7D2) {
    case LOOK_PLAYER: {
        cXyz eye;
        dNpc_playerEyePos(&eye, -20.0f);
        srcX = current.pos.x;
        vec1.copy(eye);
        srcY = eyePos.y;
        dstPos = &vec1;
        srcZ = current.pos.z;
        break;
    }
    case LOOK_POSITION:
        vec1.copy(field_0x798);
        srcZ = current.pos.z;
        srcX = current.pos.x;
        dstPos = &vec1;
        break;
    case LOOK_ANGLE:
        targetY = field_0x7B2;
        break;
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&field_0x7B0, l_HIO.mPrmTbl.field_12, 4, 0x800);
    } else {
        field_0x7B0 = 0;
    }
    cXyz vec2; /* passed by value: a copy */
    vec2.x = srcX;
    vec2.y = srcY;
    vec2.z = srcZ;
    pm1_dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, &vec2, targetY, field_0x7B0,
                                  headOnlyFollow);
}

/* 022D25D4 */
u8 daNpc_Pm1_c::chkAttention() {
    dAttention_c *attention = dComIfGp_getAttention();
    if (pm1_dAttention_LockonTruth(attention)) {
        return this == (void *)pm1_dAttention_LockonTarget(attention, 0);
    }
    return this == (void *)pm1_dAttention_ActionTarget(attention, 0);
}

/* 022D1590 */
void daNpc_Pm1_c::setAttention() {
    cXyz *attPos = &attention_info.position; /* attention_info.position */
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

/* 022D1350 */
bool daNpc_Pm1_c::decideType(int i_type) {
    field_0x7D3 = -1;
    if (fpcM_GetName(this) == fpcNm_NPC_PM1_e) {
        field_0x7D4 = 0;
        field_0x7D3 = 0;
    }
    return true;
}

/* 022D1B74 */
void daNpc_Pm1_c::event_actionInit(int param_1) {
    be<s32> *actionNo = (be<s32> *)dComIfGp_evmng_getMyIntegerP(param_1, "ActNo");
    dComIfGp_evmng_getMyIntegerP(param_1, "Timer");
    if (actionNo != nullptr) {
        field_0x7CA = (s8)(s32)*actionNo;
    }
}

/* 022D1BFC */
bool daNpc_Pm1_c::event_action() { return true; }

/* 022D1C04 */
void daNpc_Pm1_c::privateCut() {

    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Pm1", nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    field_0x7C9 = pm1_dComIfGp_evmng_getMyActIdx(staffIdx, cut_name_tbl, 1, TRUE, 0);
    if (field_0x7C9 == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        if (field_0x7C9 == 0) {
            event_actionInit(staffIdx);
        }
    }
    bool ended = true;
    if (field_0x7C9 == 0) {
        ended = event_action();
    }
    if (ended) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}

/* 022D265C */
void daNpc_Pm1_c::endEvent() {
    dComIfGp_event_reset();
    field_0x7CB = 0xFF;
}

/* 022D1ED8 */
void daNpc_Pm1_c::event_proc() {
    if (!mEventCut.cutProc()) {
        privateCut();
    }
    lookBack();
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}

/* 022D1378 */
bool daNpc_Pm1_c::set_action(Pm1Action *i_action, void *param_2) {
    Pm1Action *cur = &mAction;
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
        pm1_pmf_call(this, cur, param_2);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x7D5 = 0;
    pm1_pmf_call(this, cur, param_2);
    return true;
}

/* 022D269C */
BOOL daNpc_Pm1_c::wait01() {
    if (field_0x7C5 != 0) {
        field_0x7B7 = 0xFF;
        if (dComIfGp_event_chkTalkXY()) {
            if (dComIfGp_evmng_ChkPresentEnd() == 0) {
                return TRUE;
            }
            field_0x7B7 = dComIfGp_event_getPreItemNo();
        }
        setStt(STATE_TALK);
    } else {
        field_0x7CF = 2;
        if (field_0x7C4) {
            field_0x7D2 = LOOK_PLAYER;
        } else {
            s16 y = field_0x76C.y;
            field_0x7D2 = LOOK_ANGLE;
            field_0x7B2 = y;
            m_jnt.mbTrn = 1; /* setTrn() */
        }
    }
    return TRUE;
}

/* 022D275C */
BOOL daNpc_Pm1_c::talk01() {
    talk(1);
    // HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus)
    if (mbHasMsg) {
        if (pm1_fopMsgM_getStatus() == MSG_DESTROYED) {
            field_0x7B7 = 0xFF;
            setStt(field_0x7D1);
            field_0x7C8 = 0;
            field_0x7C5 = 0;
            endEvent();
        }
    }
    return TRUE;
}

/* 022D27DC */
BOOL daNpc_Pm1_c::wait_action1(void *) {
    if (field_0x7D5 == 0) {
        setStt(STATE_WAIT);
        field_0x7D5 = field_0x7D5 + 1;
    } else if (field_0x7D5 != -1) {
        field_0x7C4 = chkAttention();
        switch ((u32)(s32)field_0x7D0) {
        case STATE_TALK:
            field_0x7C0 = talk01();
            break;
        case STATE_WAIT:
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

/* 022D1A9C */
u8 daNpc_Pm1_c::demo() {
    if (demoActorID == 0) {
        if (field_0x7C7 != 0) {
            field_0x7C7 = 0;
        }
    } else {
        u8 id = demoActorID;
        field_0x7C7 = 1;
        /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check; result unused) */
        if (id != 0 && id <= 0x20) {
            void *obj = demoObject.get();
            if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
                JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
                obj = demoObject.get();
            }
            pm1_getDemoActor(obj, id);
        }
        pm1_dDemo_setDemoData(this, 0x6A, mpMorf, "Pm", 0, nullptr, 0, 0);
    }
    return field_0x7C7;
}

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(be<u32> *guard, be<u32> *dst, const be<u32> *src) {
    if (*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

/* 022D2128 */
BOOL daNpc_Pm1_c::_draw() {
    J3DModel *model = mpMorf->getModel();
    J3DModelData *model_data = model->getModelData();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call
    mDoExt_btpAnm_entry(&mBtpAnm, model_data, mBtpFrame);
    mpMorf->entryDL();
    pm1_removePattern(model_data); /* mBtpAnm.remove() */
    // HD: no shadow model, no real/simple shadow
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO.mPrmTbl.field_18) {
        local_static_init(&a_red_guard, &a_red, &a_red_source);
        local_static_init(&a_blue_guard, &a_blue, &a_blue_source);
    }
    dSnap_RegistFig(0x5A /* DSNAP_TYPE_NPC_PM1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 022D1F70 */
BOOL daNpc_Pm1_c::_execute() {
    if (field_0x7B8 == 0) {
        field_0x774.copy(current.pos);
        field_0x76C.x = current.angle.x;
        field_0x76C.y = current.angle.y;
        field_0x76C.z = current.angle.z;
        field_0x7B8 = 1;
    }
    daNpc_Pm1_HIO_c::hio_prm_c &prm = l_HIO.mPrmTbl;
    m_jnt.setParam(prm.field_8, prm.field_A, prm.field_C, prm.field_E, prm.field_0, prm.field_2,
                   prm.field_4, prm.field_6, prm.field_10);
    checkOrder();
    if (demo() == 0) {
        if (dComIfGp_event_runCheck() && eventInfo.getCommand() != dEvtCmd_INTALK_e) {
            event_proc();
        } else {
            pm1_pmf_call(this, &mAction, nullptr); /* (this->*field_0x6F4)(NULL) */
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
    setCollision(50.0f, 130.0f);
    return true;
}

/* 022D19E0 */
BOOL daNpc_Pm1_c::_delete() {
    // HD: resource deletion replaces resDeleteDemo; no delete-ID registration.
    dComIfG_resDelete(&mPhs, "Pm");
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

/* (tail call) */
/* 022D134C */
static BOOL CheckCreateHeap(fopAc_ac_c *actor) { return ((daNpc_Pm1_c *)actor)->CreateHeap(); }

/* 022D1858 */
cPhs_State daNpc_Pm1_c::_create() {
    // HD: merged NPC vtable and inline member constructors.
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            pm1_constructNpc(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = PM1_VTBL;
            pm1_constructBtp(&mBtpAnm);        /* mDoExt_btpAnm::mDoExt_btpAnm */
            pm1_constructEventCut(&mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State resLoadResult = dComIfG_resLoad(&mPhs, "Pm");
    if (resLoadResult != cPhs_COMPLEATE_e) {
        return resLoadResult;
    }
    if (l_HIO.field_0x8 < 0) {
        l_HIO.mNo = mDoHIO_createChild("Poor Maggie" /* (HIO name) */, &l_HIO);
    }
    l_HIO.field_0x8 = l_HIO.field_0x8 + 1;

    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)CheckCreateHeap),
                               a_heap_size_tbl[0])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 150.0f, 50.0f);
    if (createInit() == 0) {
        resLoadResult = cPhs_ERROR_e;
    }
    return resLoadResult;
}

/* 022D10A4 */
BOOL daNpc_Pm1_c::CreateHeap() {
    J3DModelData *a_mdl_data = (J3DModelData *)dComIfG_getObjectIDRes("Pm", dRes_ID_PM_BDL_PM_e);
    if (a_mdl_data == nullptr) /* JUT_ASSERT(1329, a_mdl_data != NULL) */
        JUT_ASSERT_fail("d_a_npc_pm1.cpp", 0x531, "a_mdl_data != (0)");
    J3DAnmTransform *bck = (J3DAnmTransform *)dComIfG_getObjectIDRes("Pm", dRes_ID_PM_BCK_WAIT01_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_data, nullptr, nullptr, bck, 2 /* EMode_LOOP */,
                                    1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020002);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() != nullptr) {
        m_head_jnt_num = pm1_JUTNameTab_getIndex(gabi::ea(a_mdl_data->getJointName()), "head");
        if (m_head_jnt_num < 0)
            JUT_ASSERT_fail("d_a_npc_pm1.cpp", 0x545, "m_head_jnt_num >= 0");
        m_backbone_jnt_num =
            pm1_JUTNameTab_getIndex(gabi::ea(a_mdl_data->getJointName()), "backbone");
        if (m_backbone_jnt_num < 0)
            JUT_ASSERT_fail("d_a_npc_pm1.cpp", 0x547, "m_backbone_jnt_num >= 0");
        field_0x7CD = a_tex_pattern_num_tbl[0]; /* a_tex_pattern_num_tbl[0] (HD: always entry 0) */
        if (initTexPatternAnm(false) != 0) {
            // HD: no shadow model
            for (u16 i = 0; i < a_mdl_data->getJointNum(); i++) {
                if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
                    /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Pm)
                     */
                    J3DModelData *md = mpMorf->getModel()->getModelData();
                    md->getJointNodePointer(i)->mCallBack =
                        gabi::guest_fn((const void *)nodeCallBack_Pm);
                }
            }
            mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
            mAcchCir.SetWall(30.0f, 50.0f);
            mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
            return TRUE;
        }
    }
    mpMorf = nullptr;
    return FALSE;
}

/* 022D19DC */
static cPhs_State daNpc_Pm1_Create(fopAc_ac_c *i_this) {
    return ((daNpc_Pm1_c *)i_this)->_create();
}

/* 022D1A58 */
static BOOL daNpc_Pm1_Delete(daNpc_Pm1_c *i_this) { return i_this->_delete(); }

/* 022D2124 */
static BOOL daNpc_Pm1_Execute(daNpc_Pm1_c *i_this) { return i_this->_execute(); }

/* 022D2244 */
static BOOL daNpc_Pm1_Draw(daNpc_Pm1_c *i_this) { return i_this->_draw(); }

/* 022D2248 */
static BOOL daNpc_Pm1_IsDelete(daNpc_Pm1_c *) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 022D290C */
static void __sinit_d_a_npc_pm1_cpp() {
    __sinit_header_statics();
    daNpc_Pm1_HIO_c_ct(&l_HIO); /* static daNpc_Pm1_HIO_c l_HIO */
}

/* 022D29AC */
static void SafeString_dt(SafeString *i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 022D29C0 */
static void daNpc_Pm1_c_dt(daNpc_Pm1_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopNpc_npc_c *>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 022D2A5C */
static void SafeString_assureTerminationImpl(SafeString *) {}

/* WWHD glue (tools/verify/autoglue.py)
ext 025E789C s32 pm1_mDoExt_btpAnm_init(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32)
ext 024EDFCC bool pm1_dAttention_LockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* pm1_dAttention_LockonTarget(dAttention_c*, s32)
ext 024EE464 fopAc_ac_c* pm1_dAttention_ActionTarget(dAttention_c*, s32)
ext 02542EDC s8 pm1_getMyActIdx(dEvent_manager_c*, s32, const gptr<char>*, s32, BOOL, s32)
ext 0259DED0 void pm1_dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 025F795C u32 pm1_messageStatus(msg_class*)
ext 027DF9B0 s8 pm1_JUTNameTab_getIndex(u32, const char*)
ext 024EF130 s8 pm1_dBgS_GetRoomId(dBgS*, void*)
ext 024EEEB8 u8 pm1_dBgS_GetPolyColor(dBgS*, void*)
ext 02527028 BOOL pm1_dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, void*, u32, s8)
ext 025A1458 void pm1_constructNpc(fopNpc_npc_c*)
ext 025E7820 void pm1_constructBtp(void*)
ext 0259F740 void pm1_constructEventCut(dNpc_EventCut_c*)
ext 02526E70 void* pm1_getDemoActor(void*, u8)
str 10021194 in 022D174C
str 100211EC in 022D2250
str 100211FC in 022D2250
str 1002121C in 022D2250
str 100210B0 in 022D0EF8
str 100210C0 in 022D0EF8
str 100210EC in 022D0F4C
str 100210F0 in 022D0F4C
str 10021100 in 022D0F4C
str 10021234 in 022D2324
str 100211BC in 022D1B74
str 100211C4 in 022D1B74
str 100211CC in 022D1C04
str 10021094 in 022D1A9C
str 10021084 in 022D1A9C
str 100211B7 in 022D1A9C
str 100211B4 in 022D19E0
str 100211A4 in 022D1858
str 100211A8 "Poor Maggie" renamed in 022D1858
str 10021124 in 022D10A4
str 10021130 in 022D10A4
str 1002116C in 022D10A4
str 10021128 in 022D10A4
str 10021158 in 022D10A4
str 10021180 in 022D10A4
str 10021140 in 022D10A4
data l_HIO 10468448
data a_wait_action 10021028
data a_prm_tbl 101C53AC
data a_att_pos_offst 10468480
data a_eye_pos_offst 1046848C
data a_att_guard 10468498
data a_eye_guard 1046849C
data dNpc_cyl_src 101EA190
data a_anm_idx_tbl 100211DC
data a_btp_arc_ix_tbl 100210AC
data a_anm_prm_tbl 101C536C
data a_atr_prm_tbl 101C539C
data cut_name_tbl 101C5368
data a_heap_size_tbl 101C5364
data a_tex_pattern_num_tbl 101C5360
data a_red_source 10021030
data a_blue_source 10021034
data a_red_guard 101FDA50
data a_blue_guard 101FDAC0
data a_red 101FEBF4
data a_blue 101FEBF8
data demoObject 101D5FFC
*/
