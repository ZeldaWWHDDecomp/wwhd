/**
 * d_a_npc_ym1_a2.cpp (WWHD)
 * NPC - Mesa & Abe (Outset Island), part A2: create, delete, events, execute.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ym1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 */
#define SAFESTRING_VTBL 0x10023448 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ym1.h"

#define YM1_VTBL 0x100236A0      /* daNpc_Ym1_c vtable (HD: merged with fopNpc_npc_c's) */
#define YM1_AAB_VTBL 0x10023460  /* this TU's cM3dGAab vtable copy */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* the ground polygon of the actor's Acch (mObjAcch + 0xE8, see d_a_npc_ls1.cpp) */
static inline void* daNpc_gndPoly(fopNpc_npc_c* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
/* 02516C14 dCcMassS_Mng::Set(obj, u8 priority); the mass manager is at play + PLAY_CCMASS */
static inline void dComIfG_Ccsp_SetMass(void* obj, u8 prio) { gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, prio); }

/* 022F7D68 */
bool daNpc_Ym1_c::createInit() {
    WWHD_FUNC(0x022F7D68, bool, this);
    /* l_staff_name (.data 0x101C6A38), by mStaff */
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(0x101C6A38 + mStaff * 4)), this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    /* a_attn_dist (.data 0x101C6A30): {talk, speak} by mSubType */
    gabi::store<u8>(gabi::ea(this) + 0x389, gabi::load<u8>(0x101C6A30 + mSubType * 2));     /* distances[TALK] */
    mA1B = 0xD;
    gabi::store<u8>(gabi::ea(this) + 0x38B, gabi::load<u8>(0x101C6A30 + mSubType * 2 + 1)); /* distances[SPEAK] */
    /* HD: an actor placed at (-202066, *, 320123) is moved next to a global position offset
     * (0x1047BBB0/B4) */
    if (std::fabs(current.pos.x - -202066.0f) < 10.0f && std::fabs(current.pos.z - 320123.0f) < 10.0f) {
        f32 x = gabi::load<f32>(0x1047BBB0) + -202152.0f;
        current.pos.y = 1060.2f;
        current.pos.x = x;
        f32 z = gabi::load<f32>(0x1047BBB4) + 320091.0f;
        old.pos.y = 1060.2f;
        old.pos.x = x;
        old.pos.z = z;
        current.pos.z = z;
    }
    bool init_result;
    switch ((u32)(s32)mStaff) {
    case 0:
        init_result = gabi::call<u32>(0x022F74B4, this) != 0; /* init_YM1_0() */
        break;
    case 1:
        init_result = gabi::call<u32>(0x022F75E4, this) != 0;
        break;
    case 2:
        init_result = gabi::call<u32>(0x022F7668, this) != 0;
        break;
    case 3:
        init_result = gabi::call<u32>(0x022F7710, this) != 0;
        break;
    case 4:
        init_result = gabi::call<u32>(0x022F77B8, this) != 0;
        break;
    case 5:
        init_result = gabi::call<u32>(0x022F7848, this) != 0;
        break;
    case 6:
    case 7:
        init_result = gabi::call<u32>(0x022F78D8, this) != 0; /* init_YMx_error() */
        break;
    default:
        init_result = false;
        break;
    }
    if (!init_result) {
        return false;
    }
    m9B6.x = current.angle.x;
    m9B6.y = current.angle.y;
    m9B6.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    gravity = -4.5f;
    mStts.Init(0xFF, 0xFF, this);
    fopNpc_npc_c::mCyl.SetStts(&mStts);
    fopNpc_npc_c::mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    if (mStaff == 0) {
        /* the second cylinder (mCyl) stands 80 in front of the actor */
        gabi::Local<cXyz> offset;
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = 80.0f;
        mCyl.SetStts(&mStts);
        mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190));
        gabi::Local<cXyz> center;
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset, center);
        mCyl.SetC(center);
        mCyl.SetR(50.0f);
        mCyl.SetH(30.0f);
        dComIfG_Ccsp_SetMass(&mCyl, 3);
    }
    gabi::call(0x022F79D0, this); /* play_animation() */
    if (mStaff != 1) {
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    gabi::call(0x022F7B28, this, 1); /* setMtx(true) */
    return true;
}
VERIFY(0x022F7D68, &daNpc_Ym1_c::createInit);

/* 022F8108 */
cPhs_State daNpc_Ym1_c::_create() {
    WWHD_FUNC(0x022F8108, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Ym1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);    /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = YM1_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dCcD_Cyl_ct(&mCyl, YM1_AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!gabi::call<u32>(0x022F72B0, this, (u32)(fopAcM_GetParam(this) & 0xFF))) { /* decideType(prm) */
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mA0D = state == cPhs_COMPLEATE_e;
    if (!mA0D) {
        return state;
    }
    /* a_heap_size (.data 0x101C6A58), by mSubType */
    if (!fopAcM_entrySolidHeap(this, 0x022F72AC /* CheckCreateHeap */, gabi::load<u32>(0x101C6A58 + mSubType * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -80.0f, -20.0f, -40.0f, 80.0f, 180.0f, 130.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x022F8108, &daNpc_Ym1_c::_create);

/* 022F82A8 */
static cPhs_State daNpc_Ym1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022F82A8, cPhs_State, i_this);
    return ((daNpc_Ym1_c*)i_this)->_create();
}
VERIFY(0x022F82A8, daNpc_Ym1_Create);

/* 022F82AC */
BOOL daNpc_Ym1_c::_delete() {
    WWHD_FUNC(0x022F82AC, BOOL, this);
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022F82AC, &daNpc_Ym1_c::_delete);

/* 022F8300 */
static BOOL daNpc_Ym1_Delete(daNpc_Ym1_c* i_this) {
    WWHD_FUNC(0x022F8300, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022F8300, daNpc_Ym1_Delete);

/* 022F8304 */
void daNpc_Ym1_c::checkOrder() {
    WWHD_FUNC(0x022F8304, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        return;
    }
    if (cmd == 1 /* dEvtCmd_INTALK_e */ && (mA1C == 1 || mA1C == 2)) {
        mA1C = 0;
        mA13 = 1;
    }
}
VERIFY(0x022F8304, &daNpc_Ym1_c::checkOrder);

/* 022F8504 */
s32 daNpc_Ym1_c::isEventEntry() {
    WWHD_FUNC(0x022F8504, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x022F8504, &daNpc_Ym1_c::isEventEntry);

/* 022F8544 */
void daNpc_Ym1_c::endEvent() {
    WWHD_FUNC(0x022F8544, void, this);
    dComIfGp_event_reset();
    mA18 = -1;
    mA19 = -1;
}
VERIFY(0x022F8544, &daNpc_Ym1_c::endEvent);

/* 022F8588 */
void daNpc_Ym1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x022F8588, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101C6A64), one cut whose init and move are empty */
    if (i_staffIdx == -1) {
        return;
    }
    mA17 = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101C6A64, 1, TRUE, 0);
    if (mA17 != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022F8588, &daNpc_Ym1_c::privateCut);

/* 022F8620 */
void daNpc_Ym1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x022F8620, void, this, i_staffIdx);
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}
VERIFY(0x022F8620, &daNpc_Ym1_c::event_proc);

/* 022F8890 */
void daNpc_Ym1_c::eventOrder() {
    WWHD_FUNC(0x022F8890, void, this);
    if (mA1C == 1 || mA1C == 2) {
        u32 a = gabi::ea(this) + 0xFA;
        gabi::store<u16>(a, gabi::load<u16>(a) | 1); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (mA1C == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x022F8890, &daNpc_Ym1_c::eventOrder);

/* 022F8344 */
u8 daNpc_Ym1_c::demo() {
    WWHD_FUNC(0x022F8344, u8, this);
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
        mA0F = 0;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x100234C0), 0x23A, STR(0x100234B0));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<void*>(0x02526E70, obj, id);
    }
    /* the eye animation runs on to its last frame */
    if (gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10) != 0) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)))) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)));
        }
    }
    if (demo_actor_p) {
        void* demo_btp_p = gabi::call<void*>(0x02527828, demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpHeadModel), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBlinkFrame = 0;
            mA1A = 1;
        }
    }
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), mArcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mA16;
}
VERIFY(0x022F8344, &daNpc_Ym1_c::demo);

/* 022F8670 */
void daNpc_Ym1_c::lookBack() {
    WWHD_FUNC(0x022F8670, void, this);
    m9F0.y = m_jnt.mAngles[0][1];
    gabi::Local<cXyz> player_eye_pos;
    player_eye_pos->set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    f32 srcX = current.pos.x;
    m9F0.z = m_jnt.mAngles[1][1];
    f32 srcY = eyePos.y;
    u8 head_only = mA14;
    m9F0.x = target_y;
    f32 srcZ = current.pos.z;
    cXyz* player_eye_pos_p = nullptr;
    switch ((u32)(s32)mA1F) {
    case 0:
        break;
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        m9C8.copy(*eye);
        player_eye_pos->copy(*eye);
        player_eye_pos_p = player_eye_pos;
        break;
    }
    case 2:
        player_eye_pos->copy(m9C8);
        player_eye_pos_p = player_eye_pos;
        break;
    case 3:
        target_y = mA06;
        break;
    default:
        break;
    }
    gabi::Local<cXyz> current_pos; /* passed by value: a copy */
    current_pos->x = srcX;
    current_pos->y = srcY;
    current_pos->z = srcZ;
    /* l_HIO.mPrm[mSubType] (0x2C each), the turn speed at +0x12 */
    s16 vel = gabi::load<s16>(0x1046896E + mSubType * 0x2C);
    gabi::call(0x0259E2D4, &m_jnt, &current.angle.y, player_eye_pos_p, current_pos.get(), target_y, vel, head_only); /* lookAtTarget_2 */
}
VERIFY(0x022F8670, &daNpc_Ym1_c::lookBack);

/* 022F88C8 */
void daNpc_Ym1_c::set_cutGrass() {
    WWHD_FUNC(0x022F88C8, void, this);
    if (mA16 == 0 && mA1B == 1 && mpMorf->checkFrame(34.0f)) {
        /* grass cut at the left hand */
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), mHandLJointIndex), mDoMtx_stack_c::get());
        gabi::Local<cXyz> pos;
        Mtx34* stk = mDoMtx_stack_c::get();
        pos->x = stk->m[0][3];
        pos->y = stk->m[1][3];
        pos->z = stk->m[2][3];
        dComIfGp_particle_setSimple(0x3DA, pos);
        mDoAud_seStart(0x5812, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
}
VERIFY(0x022F88C8, &daNpc_Ym1_c::set_cutGrass);

/* 022F89CC */
void daNpc_Ym1_c::set_collision_sp() {
    WWHD_FUNC(0x022F89CC, void, this);
    if (mA13) {
        return;
    }
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> center;
    f32 r, h;
    switch ((u32)(s32)mA1B) {
    case 1:
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = 20.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), offset, center);
        r = 60.0f;
        h = 140.0f;
        break;
    case 5:
    case 6:
    case 7:
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = 60.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), offset, center);
        r = 80.0f;
        h = 150.0f;
        break;
    default:
        center->x = current.pos.x;
        center->y = current.pos.y;
        center->z = current.pos.z;
        r = 60.0f;
        h = 180.0f;
        break;
    }
    fopNpc_npc_c::mCyl.SetC(center);
    fopNpc_npc_c::mCyl.SetR(r);
    fopNpc_npc_c::mCyl.SetH(h);
    cCcS_Set(dComIfG_Ccsp(), &this->fopNpc_npc_c::mCyl);
}
VERIFY(0x022F89CC, &daNpc_Ym1_c::set_collision_sp);

/* 022F8C24 */
BOOL daNpc_Ym1_c::_execute() {
    WWHD_FUNC(0x022F8C24, BOOL, this);
    if (!mA11) {
        /* the initial pose (word copies) */
        m9A4 = gabi::load<u32>(gabi::ea(&current.pos.x));
        mA11 = 1;
        m9AC = gabi::load<u32>(gabi::ea(&current.pos.z));
        m9A8 = gabi::load<u32>(gabi::ea(&current.pos.y));
        m9B0 = current.angle.x;
        mRotYTarget = current.angle.y;
        m9B4 = current.angle.z;
    }
    if (gabi::call<u32>(0x022F7A3C, this) != 0) { /* chk_nbt_attn() */
        u32 prm = 0x1046895C + mSubType * 0x2C; /* l_HIO.mPrm[mSubType] */
        m_jnt.setParam(0, 0, 0, 0, 0x2000, 0x38E0, -0x2000, -0x38E0, gabi::load<s16>(prm + 0x10));
    } else {
        u32 prm = 0x1046895C + mSubType * 0x2C;
        m_jnt.setParam(gabi::load<s16>(prm + 8), gabi::load<s16>(prm + 0xA), gabi::load<s16>(prm + 0xC), gabi::load<s16>(prm + 0xE),
                       gabi::load<s16>(prm + 0), gabi::load<s16>(prm + 2), gabi::load<s16>(prm + 4), gabi::load<s16>(prm + 6),
                       gabi::load<s16>(prm + 0x10));
    }
    if (mA0E && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        if (mStaff != 1) {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
            mObjAcch.CrrPos(dComIfG_Bgsp());
        }
        gabi::call(0x022F79D0, this); /* play_animation() */
    } else {
        mA0E = 0;
    }
    eventOrder();
    m9B6.x = current.angle.x;
    m9B6.y = current.angle.y;
    m9B6.z = current.angle.z;
    if (!mA0F) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    gabi::call(0x022F7B28, this, 0); /* setMtx(false) */
    set_cutGrass();
    if (mStaff == 0) {
        /* dCcMassS_Mng::SetAreaChk(&mCyl, 3, area_check) */
        gabi::call(0x02516D48, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), &mCyl, 3, 0x022F68B8);
    }
    if (!mA16) {
        set_collision_sp();
    }
    return TRUE;
}
VERIFY(0x022F8C24, &daNpc_Ym1_c::_execute);

/* 022F8F2C */
static BOOL daNpc_Ym1_Execute(daNpc_Ym1_c* i_this) {
    WWHD_FUNC(0x022F8F2C, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022F8F2C, daNpc_Ym1_Execute);
