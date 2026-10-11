/**
 * d_a_npc_ym1_a2.cpp (WWHD)
 * NPC - Mesa & Abe (Outset Island), part A2: create, delete, events, execute.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ym1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 */
#define SAFESTRING_VTBL 0x10023448
#include "d/actor/d_a_npc_ym1_a2.h"

// HD: image-checked tables and the scene placement offset used by this actor.
static const char* l_staff_name[] = {"Ym1", "Ym1", "Ym2", "Ym2", "Ym2", "Ym2", "Ym1", "Ym2"};
static const u8 a_attn_dist[4][2] = {{0, 0}, {170, 170}, {170, 170}, {0, 0}};
static const u32 a_heap_size[] = {0, 0, 0};
static const char* a_cut_tbl[] = {"DUMMY"};
static const u32 dNpc_cyl_src[] = {0, 0, 0, 0, 4294967231, 3, 117, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0};
static Ym1SceneOffset l_sceneOffset = {};
static Ym1JointParam l_jointParam[3] = {};
static Ym1DemoObject* l_demoObject = {0};

/* 022F7D68 */
bool daNpc_Ym1_c::createInit() {
    /* l_staff_name (.data 0x101C6A38), by mStaff */
    mEventCut.setActorInfo2(l_staff_name[mStaff], this);
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    /* a_attn_dist (.data 0x101C6A30): {talk, speak} by mSubType */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = a_attn_dist[mSubType][0];     /* distances[TALK] */
    ym1State(this).m8AB = 0xD;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = a_attn_dist[mSubType][1]; /* distances[SPEAK] */
    /* HD: an actor placed at (-202066, *, 320123) is moved next to a global position offset
     * (0x1047BBB0/B4) */
    if (std::fabs(current.pos.x - -202066.0f) < 10.0f && std::fabs(current.pos.z - 320123.0f) < 10.0f) {
        f32 x = l_sceneOffset.x + -202152.0f;
        current.pos.y = 1060.2f;
        current.pos.x = x;
        f32 z = l_sceneOffset.z + 320091.0f;
        old.pos.y = 1060.2f;
        old.pos.x = x;
        old.pos.z = z;
        current.pos.z = z;
    }
    bool init_result;
    switch ((u32)(s32)mStaff) {
    case 0:
        init_result = Ym1_init_YM1_0(this) != 0; /* init_YM1_0() */
        break;
    case 1:
        init_result = Ym1_init_YM1_1(this) != 0;
        break;
    case 2:
        init_result = Ym1_init_YM2_0(this) != 0;
        break;
    case 3:
        init_result = Ym1_init_YM2_1(this) != 0;
        break;
    case 4:
        init_result = Ym1_init_YM2_2(this) != 0;
        break;
    case 5:
        init_result = Ym1_init_YM2_3(this) != 0;
        break;
    case 6:
    case 7:
        init_result = Ym1_init_YMx_error(this) != 0; /* init_YMx_error() */
        break;
    default:
        init_result = false;
        break;
    }
    if (!init_result) {
        return false;
    }
    ym1Angles(this).m846.x = current.angle.x;
    ym1Angles(this).m846.y = current.angle.y;
    ym1Angles(this).m846.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    gravity = -4.5f;
    mStts.Init(0xFF, 0xFF, this);
    fopNpc_npc_c::mCyl.SetStts(&mStts);
    fopNpc_npc_c::mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(dNpc_cyl_src) /* dNpc_cyl_src */);
    if (mStaff == 0) {
        /* the second cylinder (mCyl) stands 80 in front of the actor */
        cXyz offset;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 80.0f;
        mCyl.SetStts(&mStts);
        mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(dNpc_cyl_src));
        cXyz center;
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &center);
        mCyl.SetC(&center);
        mCyl.SetR(50.0f);
        mCyl.SetH(30.0f);
        Ym1_setMass(ym1MassManager(), &mCyl, 3);
    }
    Ym1_play_animation(this); /* play_animation() */
    if (mStaff != 1) {
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    Ym1_setMtx(this, 1); /* setMtx(true) */
    return true;
}

/* 022F8108 */
cPhs_State daNpc_Ym1_c::_create() {
    /* fopAcM_ct(this, daNpc_Ym1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Ym1_npcCtor(this);    /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = YM1_VTBL;
            mDoExt_btpAnm::ct(ym1Btp(this)); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dCcD_Cyl_ct(&mCyl, YM1_AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!Ym1_decideType(this, (u32)(fopAcM_GetParam(this) & 0xFF))) { /* decideType(prm) */
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    ym1State(this).m89D = state == cPhs_COMPLEATE_e;
    if (!ym1State(this).m89D) {
        return state;
    }
    /* a_heap_size (.data 0x101C6A58), by mSubType */
    if (!fopAcM_entrySolidHeap(this, Ym1_CheckCreateHeap, a_heap_size[mSubType])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -80.0f, -20.0f, -40.0f, 80.0f, 180.0f, 130.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 022F82A8 */
static cPhs_State daNpc_Ym1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Ym1_c*)i_this)->_create();
}

/* 022F82AC */
BOOL daNpc_Ym1_c::_delete() {
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022F8300 */
static BOOL daNpc_Ym1_Delete(daNpc_Ym1_c* i_this) {
    return i_this->_delete();
}

/* 022F8304 */
void daNpc_Ym1_c::checkOrder() {
    u16 cmd = eventInfo.getCommand(); /* eventInfo.mCommand */
    if (cmd == dEvtCmd_INDEMO_e) {
        return;
    }
    if (cmd == dEvtCmd_INTALK_e && (mA1C == 1 || mA1C == 2)) {
        mA1C = 0;
        mA13 = 1;
    }
}

/* 022F8504 */
s32 daNpc_Ym1_c::isEventEntry() {
    const char* name = ym1EventStaffName(this);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 022F8544 */
void daNpc_Ym1_c::endEvent() {
    dComIfGp_event_reset();
    mA18 = -1;
    mA19 = -1;
}

/* 022F8588 */
void daNpc_Ym1_c::privateCut(int i_staffIdx) {
    /* a_cut_tbl (.data 0x101C6A64), one cut whose init and move are empty */
    if (i_staffIdx == -1) {
        return;
    }
    mA17 = dComIfGp_evmng_getMyActIdx(i_staffIdx, gabi::ea(a_cut_tbl), 1, TRUE, 0);
    if (mA17 != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 022F8620 */
void daNpc_Ym1_c::event_proc(int i_staffIdx) {
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}

/* 022F8890 */
void daNpc_Ym1_c::eventOrder() {
    if (mA1C == 1 || mA1C == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (mA1C == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 022F8344 */
u8 daNpc_Ym1_c::demo() {
    u8 id = demoActorID;
    if (id == 0) {
        if (mA16) {
            mA16 = 0;
        }
        return mA16;
    }
    if (!mA16) {
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
        mA16 = 1;
        ym1State(this).m89F = 0;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    Ym1DemoActor* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        Ym1DemoObject* obj = l_demoObject;
        if (obj == nullptr) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != NULL");
            obj = l_demoObject;
        }
        demo_actor_p = obj->getActor(id);
    }
    /* the eye animation runs on to its last frame */
    if (ym1Btp(this)->mpAnm.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(ym1Btp(this)->mpAnm.get())) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(ym1Btp(this)->mpAnm.get());
        }
    }
    if (demo_actor_p) {
        void* demo_btp_p = demo_actor_p->getP_BtpData(mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpHeadModel), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBlinkFrame = 0;
            mA1A = 1;
        }
    }
    Ym1_setDemoData(this, Ym1_DEMO_ENABLE, mpMorf.get(), mArcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mA16;
}

/* 022F8670 */
void daNpc_Ym1_c::lookBack() {
    m9F0.y = m_jnt.mAngles[0][1];
    cXyz player_eye_pos;
    player_eye_pos.set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    f32 srcX = current.pos.x;
    m9F0.z = m_jnt.mAngles[1][1];
    f32 srcY = eyePos.y;
    u8 head_only = ym1State(this).m8A4;
    m9F0.x = target_y;
    f32 srcZ = current.pos.z;
    cXyz* player_eye_pos_p = nullptr;
    switch ((u32)(s32)ym1State(this).m8AF) {
    case 0:
        break;
    case 1: {
        cXyz eye;
        dNpc_playerEyePos_l(&eye, -20.0f);
        m9C8.copy(eye);
        player_eye_pos.copy(eye);
        player_eye_pos_p = &player_eye_pos;
        break;
    }
    case 2:
        player_eye_pos.copy(m9C8);
        player_eye_pos_p = &player_eye_pos;
        break;
    case 3:
        target_y = mA06;
        break;
    default:
        break;
    }
    cXyz current_pos; /* passed by value: a copy */
    current_pos.x = srcX;
    current_pos.y = srcY;
    current_pos.z = srcZ;
    /* l_HIO.mPrm[mSubType] (0x2C each), the turn speed at +0x12 */
    s16 vel = l_jointParam[mSubType].mTurnSpeed;
    Ym1_lookAtTarget2(&m_jnt, &current.angle.y, player_eye_pos_p, &current_pos, target_y, vel, head_only); /* lookAtTarget_2 */
}

/* 022F88C8 */
void daNpc_Ym1_c::set_cutGrass() {
    if (mA16 == 0 && ym1State(this).m8AB == 1 && mpMorf->checkFrame(34.0f)) {
        /* grass cut at the left hand */
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), mHandLJointIndex), mDoMtx_stack_c::get());
        cXyz pos;
        Mtx34* stk = mDoMtx_stack_c::get();
        pos.x = stk->m[0][3];
        pos.y = stk->m[1][3];
        pos.z = stk->m[2][3];
        dComIfGp_particle_setSimple(Ym1_PARTICLE_GRASS, &pos);
        mDoAud_seStart(Ym1_SE_GRASS_CUT, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
}

/* 022F89CC */
void daNpc_Ym1_c::set_collision_sp() {
    if (mA13) {
        return;
    }
    cXyz offset;
    cXyz center;
    f32 r, h;
    switch ((u32)(s32)ym1State(this).m8AB) {
    case 1:
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 20.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &center);
        r = 60.0f;
        h = 140.0f;
        break;
    case 5:
    case 6:
    case 7:
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 60.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &center);
        r = 80.0f;
        h = 150.0f;
        break;
    default:
        center.x = current.pos.x;
        center.y = current.pos.y;
        center.z = current.pos.z;
        r = 60.0f;
        h = 180.0f;
        break;
    }
    fopNpc_npc_c::mCyl.SetC(&center);
    fopNpc_npc_c::mCyl.SetR(r);
    fopNpc_npc_c::mCyl.SetH(h);
    cCcS_Set(dComIfG_Ccsp(), &this->fopNpc_npc_c::mCyl);
}

/* 022F8C24 */
BOOL daNpc_Ym1_c::_execute() {
    if (!mA11) {
        /* the initial pose (word copies) */
        m9A4 = ym1PositionWords(this).x;
        mA11 = 1;
        m9AC = ym1PositionWords(this).z;
        m9A8 = ym1PositionWords(this).y;
        m9B0 = current.angle.x;
        mRotYTarget = current.angle.y;
        m9B4 = current.angle.z;
    }
    if (Ym1_chk_nbt_attn(this) != 0) { /* chk_nbt_attn() */
        Ym1JointParam* prm = &l_jointParam[mSubType]; /* l_HIO.mPrm[mSubType] */
        m_jnt.setParam(0, 0, 0, 0, 0x2000, 0x38E0, -0x2000, -0x38E0, prm->mTurnStep);
    } else {
        Ym1JointParam* prm = &l_jointParam[mSubType];
        m_jnt.setParam(prm->mMaxBackboneX, prm->mMaxBackboneY, prm->mMinBackboneX, prm->mMinBackboneY,
                       prm->mMaxHeadX, prm->mMaxHeadY, prm->mMinHeadX, prm->mMinHeadY,
                       prm->mTurnStep);
    }
    if (ym1State(this).m89E && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() && !eventInfo.checkCommandTalk() &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            (this->*ym1Action(this))(nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        if (mStaff != 1) {
            fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
            mObjAcch.CrrPos(dComIfG_Bgsp());
        }
        Ym1_play_animation(this); /* play_animation() */
    } else {
        ym1State(this).m89E = 0;
    }
    eventOrder();
    ym1Angles(this).m846.x = current.angle.x;
    ym1Angles(this).m846.y = current.angle.y;
    ym1Angles(this).m846.z = current.angle.z;
    if (!ym1State(this).m89F) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    Ym1_setMtx(this, 0); /* setMtx(false) */
    set_cutGrass();
    if (mStaff == 0) {
        /* dCcMassS_Mng::SetAreaChk(&mCyl, 3, area_check) */
        Ym1_setMassArea(ym1MassManager(), &mCyl, 3, Ym1_area_check);
    }
    if (!mA16) {
        set_collision_sp();
    }
    return TRUE;
}

/* 022F8F2C */
static BOOL daNpc_Ym1_Execute(daNpc_Ym1_c* i_this) {
    return i_this->_execute();
}

/* WWHD glue (tools/verify/autoglue.py)
str 10023614 "Ym1"
str 10023618 "Ym2"
str 10023630 "DUMMY"
str 100234C0 "d_demo.h"
str 100234B0 "m_object != NULL"
data l_staff_name 101C6A38
data a_attn_dist 101C6A30
data a_heap_size 101C6A58
data a_cut_tbl 101C6A64
data dNpc_cyl_src 101EA190
data l_sceneOffset 1047BBB0 nocheck
data l_jointParam 1046895C nocheck
data l_demoObject 101D5FFC
ext 022F74B4 u32 Ym1_init_YM1_0(daNpc_Ym1_c*)
ext 022F75E4 u32 Ym1_init_YM1_1(daNpc_Ym1_c*)
ext 022F7668 u32 Ym1_init_YM2_0(daNpc_Ym1_c*)
ext 022F7710 u32 Ym1_init_YM2_1(daNpc_Ym1_c*)
ext 022F77B8 u32 Ym1_init_YM2_2(daNpc_Ym1_c*)
ext 022F7848 u32 Ym1_init_YM2_3(daNpc_Ym1_c*)
ext 022F78D8 u32 Ym1_init_YMx_error(daNpc_Ym1_c*)
ext 02516C14 void Ym1_setMass(Ym1MassManager*, dCcD_Cyl*, u8)
ext 022F79D0 void Ym1_play_animation(daNpc_Ym1_c*)
ext 022F7B28 void Ym1_setMtx(daNpc_Ym1_c*, u32)
ext 025A1458 void Ym1_npcCtor(daNpc_Ym1_c*)
ext 022F72B0 u32 Ym1_decideType(daNpc_Ym1_c*, u32)
ext 022F7A3C u32 Ym1_chk_nbt_attn(daNpc_Ym1_c*)
ext 02526E70 Ym1DemoActor* Ym1DemoObject::getActor(u8)
ext 02527828 void* Ym1DemoActor::getP_BtpData(const char*)
ext 02527028 void Ym1_setDemoData(daNpc_Ym1_c*, u32, mDoExt_McaMorf*, const char*, u32, u32, u32, u32)
ext 0259E2D4 void Ym1_lookAtTarget2(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 02516D48 void Ym1_setMassArea(Ym1MassManager*, dCcD_Cyl*, u8, u32)
*/
