/**
 * d_a_npc_ko1_b.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island), part B
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part B: setMtx, create/delete, events, route checks, movement, execute, draw.
 */
#define SAFESTRING_VTBL 0x1001C87C /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ko1_b.h"

// HD: Part A owns TU-wide state. These are aliases, never duplicate definitions.
static be<s32>& l_check_wrk = gabi::object_at<be<s32>>(0x104677B0);
static gptr<fopAc_ac_c> (&l_check_inf)[20] = gabi::object_at<gptr<fopAc_ac_c>[20]>(0x104678A8);
static Ko1MovementParameters (&l_movement_parameters)[2] = gabi::object_at<Ko1MovementParameters[2]>(0x104677E8);
static gptr<const char> (&l_staff_name)[9] = gabi::object_at<gptr<const char>[9]>(0x101BF7A8);
static be<u32> (&a_heap_size_tbl)[2] = gabi::object_at<be<u32>[2]>(0x101BF7CC);
static gptr<const char> (&a_cut_tbl)[1] = gabi::object_at<gptr<const char>[1]>(0x101BF7D4);
static dCcD_SrcCyl& l_cylinder_source = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static be<u32>& l_red_guard = gabi::object_at<be<u32>>(0x101FDA50);
static be<u32>& l_red_color = gabi::object_at<be<u32>>(0x101FEBF4);
static be<u32>& l_red_initial = gabi::object_at<be<u32>>(0x1001C868);
static be<u32>& l_blue_guard = gabi::object_at<be<u32>>(0x101FDAC0);
static be<u32>& l_blue_color = gabi::object_at<be<u32>>(0x101FEBF8);
static be<u32>& l_blue_initial = gabi::object_at<be<u32>>(0x1001C86C);
static be<u32>& l_green_guard = gabi::object_at<be<u32>>(0x101FDA48);
static be<u32>& l_green_color = gabi::object_at<be<u32>>(0x101FEBEC);
static be<u32>& l_green_initial = gabi::object_at<be<u32>>(0x1001C870);
static be<u32>& l_yellow_guard = gabi::object_at<be<u32>>(0x101FDA44);
static be<u32>& l_yellow_color = gabi::object_at<be<u32>>(0x101FEBE8);
static be<u32>& l_yellow_initial = gabi::object_at<be<u32>>(0x1001C874);

/* this TU's dBgS_LinChk / dBgS_GndChk vtables (inline constructors and destructors) */
static const dBgS_LinChk_vt l_linChk_vt = {0x1001C914, 0x1001C924, 0x1001C944, 0x1001C934};
static const dBgS_GndChk_vt l_gndChk_vt = {0x1001C8B4, 0x1001C8C4, 0x1001C8E4, 0x1001C8D4};

static constexpr u32 l_actor_vtable = 0x1001CD88; // HD: merged with the NPC base's vtable.
static constexpr u32 l_group_vtable = 0x1001C8A4;
static constexpr u32 l_poly_vtable = 0x1001C894;

/* 02274E28 */
bool daNpc_Ko1_c::createInit() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
    switch ((u32)(s32)mType) {
    case KO1_JOEL:
        attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9;
        attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9;
        break;
    case KO1_ZILL:
        attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA7;
        attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9;
        break;
    }
    gravity = -4.5f;
    u8 pathIdx = (fopAcM_GetParam(this) >> 16) & 0xFF;
    mInitialAngle.y = current.angle.y;
    mInitialPos.copy(current.pos);
    fields.mLookTarget.copy(current.pos);
    fields.mSpawnPos.copy(current.pos);
    mInitialAngle.x = current.angle.x;
    mInitialAngle.z = current.angle.z;
    s32 weight = 0xFF;
    if (pathIdx != 0xFF) {
        Ko1B_setPath(&mPathRun, pathIdx, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~fopAcStts_NOCULLEXEC_e;
        weight = 0xF0;
    }
    mEventCut.setActorInfo2(l_staff_name[mSpecificType].get(), this);
    fields.mAnimationNo = 0xE;
    bool ok;
    switch ((u32)(s32)mSpecificType) {
    case KO1_HNA_0:
        ok = init_HNA_0();
        break;
    case KO1_HNA_1:
        ok = init_HNA_1();
        weight = 0xF0;
        break;
    case KO1_HNA_2:
        ok = init_HNA_2();
        weight = 0xF0;
        break;
    case KO1_HNA_3:
        ok = init_HNA_3();
        break;
    case KO1_HNA_4:
        ok = init_HNA_4();
        break;
    case KO1_BOU_0:
        ok = init_BOU_0();
        break;
    case KO1_BOU_1:
        ok = init_BOU_1();
        weight = 0xF0;
        break;
    case KO1_BOU_2:
        ok = init_BOU_2();
        break;
    case KO1_BOU_3:
        ok = init_BOU_3();
        break;
    default:
        return false;
    }
    if (!ok) {
        return false;
    }
    fields.mModelAngle.x = current.angle.x;
    fields.mModelAngle.y = current.angle.y;
    fields.mModelAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&l_cylinder_source);
    mpMorf->setMorf(0.0f);
    fields.mpHeadMorf->setMorf(0.0f);
    setMtx(1);
    return true;
}

/* 02274A7C */
// HD: forward the full setEyePos register to setAttention without normalizing its GameCube bool.
void daNpc_Ko1_c::setMtx(u32 i_setEyePos) {
    Ko1MovementFields& fields = Ko1B_fields(this);
    if (fields.mInDemo == 0) {
        u32 sndId = 0;
        plyTexPttrnAnm();
        if (mObjAcch.ChkGroundHit()) {
            sndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
        }
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        fields.mAnimationEnded = (u8)Ko1B_play(mpMorf, &eyePos, sndId, reverb);
        mDoExt_McaMorf* morf = mpMorf;
        s8 animationNo = fields.mAnimationNo;
        if (morf->getFrame() < fields.mPreviousFrame) {
            morf = mpMorf;
            fields.mAnimationEnded = 1;
        }
        fields.mPreviousFrame = morf->getFrame();
        if (animationNo == 4 && mpMorf->checkFrame(10.0f)) {
            mDoAud_seStart(0x5817, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
        }
        Ko1B_play(fields.mpHeadMorf, &eyePos, 0, 0);
        mDoExt_McaMorf* balloonMorf = fields.mpBalloonMorf;
        if (balloonMorf != nullptr && fields.mBalloonState < 2) {
            balloonMorf->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(mpMorf->getFrame());
        }
        fields.mPreviousGroundHit = (mObjAcch.m_flags >> 5) & 1;
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    cBgS_PolyInfo* gndPoly = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd);
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gndPoly);
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), gndPoly);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(fields.mModelAngle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    {
        J3DModel* model = mpMorf->getModel();
        s8 jnt = fields.mBodyJoint;
        J3DModel* dst = fields.mpHeadMorf->getModel(); /* read before getAnmMtx's flag store */
        J3DModel_setBaseTRMtx(dst, getAnmMtx(model, jnt));
    }
    fields.mpHeadMorf->calc();
    if (fields.mpBalloonMorf.get() != nullptr) {
        fields.mpBalloonMorf->calc();
    }
    J3DModel* itemModel = fields.mpItemModel;
    if (itemModel != nullptr) {
        J3DModel_setBaseTRMtx(itemModel, getAnmMtx(mpMorf->getModel(), fields.mItemJoint));
        J3DModel_calc(fields.mpItemModel);
    }
    setAttention(i_setEyePos);
}

/* 02276198 */
void daNpc_Ko1_c::setPlaySpd(f32 i_speed) {
    Ko1MovementFields& fields = Ko1B_fields(this);
    fields.mpHeadMorf->mFrameCtrl.mRate = i_speed;
    mpMorf->mFrameCtrl.mRate = i_speed;
}

/* 02276578 */
void daNpc_Ko1_c::eventOrder() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    if (fields.mEventState == KO1_EVENT_SPEAK || fields.mEventState == KO1_EVENT_TALK) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (fields.mEventState == KO1_EVENT_SPEAK) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 02275504 */
void daNpc_Ko1_c::checkOrder() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    u16 cmd = eventInfo.mCommand;
    if (cmd == dEvtCmd_INDEMO_e) {
        return;
    }
    if (cmd == dEvtCmd_INTALK_e && (fields.mEventState == KO1_EVENT_SPEAK || fields.mEventState == KO1_EVENT_TALK)) {
        fields.mEventState = KO1_EVENT_NONE;
        fields.mTalkRequested = 1;
    }
}

/* 022758B8 */
void daNpc_Ko1_c::lookBack() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    s16 desiredYRot = current.angle.y;
    fields.mPreviousBodyY = desiredYRot;
    fields.mPreviousBackboneY = m_jnt.mAngles[1][1];
    fields.mPreviousHeadY = m_jnt.mAngles[0][1];
    gabi::Local<cXyz> dstPos;
    dstPos->set(0.0f, 0.0f, 0.0f);
    f32 eyeY = eyePos.y;
    f32 posX = current.pos.x;
    u8 headOnly = fields.mHeadOnly;
    cXyz* dstPosP = nullptr;
    f32 posZ = current.pos.z;
    switch ((u32)(s32)fields.mLookMode) {
    case KO1_LOOK_PLAYER: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        posX = current.pos.x;
        posZ = current.pos.z;
        eyeY = eyePos.y;
        dstPos->copy(*eye);
        dstPosP = dstPos;
        break;
    }
    case KO1_LOOK_TARGET:
        dstPos->copy(fields.mLookTarget);
        posX = current.pos.x;
        posZ = current.pos.z;
        dstPosP = dstPos;
        break;
    case KO1_LOOK_ANGLE:
        desiredYRot = fields.mLookAngle;
        break;
    }
    cLib_addCalcAngleS2(&fields.mTurnVelocity, l_movement_parameters[mType].mTurnMaxVelocity, 4, 0x800);
    s16 maxVel;
    if (!m_jnt.mbTrn) {
        fields.mTurnVelocity = 0;
        maxVel = 0;
    } else {
        maxVel = fields.mTurnVelocity;
    }
    gabi::Local<cXyz> eyeCopy;
    eyeCopy->set(posX, eyeY, posZ);
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPosP, eyeCopy, desiredYRot, maxVel, headOnly);
}

/* 02275358 */
u32 daNpc_Ko1_c::partner_srch_sub(u32 i_judgeFunc) {
    l_check_wrk = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf[i] = nullptr;
    }
    fpcM_Search(i_judgeFunc, this);
    u32 id = 0xFFFFFFFF;
    if (l_check_wrk != 0) {
        id = fopAcM_GetID(l_check_inf[0].get());
    }
    return id;
}

/* 022753EC */
void daNpc_Ko1_c::partner_srch() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    if (fields.mActionStep != 1)
        return;
    switch ((u32)(s32)mSpecificType) {
    case KO1_HNA_1:
        fields.mPartnerId = partner_srch_sub(gabi::guest_fn((const void*)Ko1B_searchBou));
        fields.mPartnerCount = 1;
        break;
    case KO1_HNA_3:
        fields.mPartnerId = partner_srch_sub(gabi::guest_fn((const void*)Ko1B_searchBou));
        fields.mOtherPartnerId = partner_srch_sub(gabi::guest_fn((const void*)Ko1B_searchOb));
        fields.mPartnerCount = 2;
        break;
    case KO1_BOU_1:
        fields.mPartnerId = partner_srch_sub(gabi::guest_fn((const void*)Ko1B_searchHna));
        fields.mPartnerCount = 1;
        break;
    case KO1_BOU_2:
        fields.mPartnerId = partner_srch_sub(gabi::guest_fn((const void*)Ko1B_searchHna));
        fields.mOtherPartnerId = partner_srch_sub(gabi::guest_fn((const void*)Ko1B_searchOb));
        fields.mPartnerCount = 2;
        break;
    }
}

/* 022761AC */
/* 0: moving, 1: point reached, 2: end of the path */
s32 daNpc_Ko1_c::ko_movPass() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    dPath* path = mPathRun.mPath;
    if (path != nullptr && (Ko1B_view<Ko1PathFlags>(path)->mClosed & 1)) {
        gabi::Local<cXyz> pos;
        pos->set(current.pos.x, current.pos.y, current.pos.z);
        if (Ko1B_checkPoint(&mPathRun, pos, mPathRun.mbDir != 0)) {
            Ko1B_nextPoint(&mPathRun);
            return KO1_POINT_REACHED;
        }
        return KO1_MOVING;
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(&fields.mMoveTarget, diff, &current.pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->set(diff->x, 0.0f, diff->z);
    f32 dist = std_sqrtf(PSVECSquareMag(diffXZ));
    s32 ret = KO1_MOVING;
    if (!(dist > fields.mArrivalRadius)) {
        ret = KO1_POINT_REACHED;
        if (mPathRun.mPath.get() != nullptr && !Ko1B_nextPoint(&mPathRun)) {
            ret = KO1_PATH_END;
        }
    }
    return ret;
}

/* 02276028 */
void daNpc_Ko1_c::ko_clcMovSpd() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&fields.mMoveTarget, diff, &current.pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->set(diff->x, 0.0f, diff->z);
    f32 distSq = PSVECSquareMag(diffXZ);
    gabi::Local<be<s16>> angle;
    *angle = cLib_targetAngleY(&current.pos, &fields.mMoveTarget);
    if (!routeCheck(distSq, angle)) {
        return;
    }
    switch (fields.mActionState) {
    case 4:
    case 0xB:
    case 0x18:
        /* zigzag */
        if (!cLib_calcTimer(&fields.mZigzagTimer)) {
            fields.mZigzagDirection = fields.mZigzagDirection ^ 1;
            fields.mZigzagTimer = (s16)cLib_getRndValue(8, 0x14);
        }
        *angle = (s16)(*angle + (fields.mZigzagDirection != 0 ? -0x2000 : 0x2000));
        break;
    }
    cLib_chaseAngleS(&current.angle.y, *angle, l_movement_parameters[mType].mAngleStep);
    cLib_chaseF(&speedF, fields.mTargetSpeed, fields.mAcceleration);
}

/* 022762AC */
void daNpc_Ko1_c::ko_clcSwmSpd() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&fields.mMoveTarget, diff, &current.pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->set(diff->x, 0.0f, diff->z);
    f32 distSq = PSVECSquareMag(diffXZ);
    gabi::Local<be<s16>> angle;
    *angle = cLib_targetAngleY(&current.pos, &fields.mMoveTarget);
    if (!routeCheck(distSq, angle)) {
        return;
    }
    cLib_chaseAngleS(&current.angle.y, *angle, l_movement_parameters[mType].mAngleStep);
    cLib_chaseF(&speed.y, fields.mSwimSpeed, 1.6f);
    cLib_chaseF(&speedF, 0.0f, 0.4f);
}

/* 022763A8 */
void daNpc_Ko1_c::ko_nMove() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    s32 pass;
    switch ((u32)(s32)fields.mMoveMode) {
    case KO1_MOVE_WAIT:
        break;
    case KO1_MOVE_WALK:
    case KO1_MOVE_RUN: {
        ko_clcMovSpd();
        Ko1MovementParameters& hio = l_movement_parameters[mType];
        f32 spd = speedF;
        f32 rate, maxRate;
        if (fields.mMoveMode == KO1_MOVE_WALK) {
            rate = hio.mWalkAnimationRate;
            maxRate = hio.mWalkMaxSpeed * rate;
        } else {
            rate = hio.mRunAnimationRate;
            maxRate = hio.mRunMaxSpeed * rate;
        }
        f32 play = spd * rate;
        play = (play - maxRate >= 0.0f) ? maxRate : play; /* fsel */
        if (play < 0.5f) {
            play = 0.5f;
        }
        setPlaySpd(play);
        pass = ko_movPass();
        if (pass == KO1_POINT_REACHED) {
            fields.mPointReached = 1;
            fields.mSkipMovement = 1;
            return;
        }
        if (pass == KO1_PATH_END) {
            fields.mPointReached = 1;
            fields.mMoveMode = KO1_MOVE_WAIT;
            fields.mSkipMovement = 1;
            return;
        }
        break;
    }
    case KO1_MOVE_SWIM:
        ko_clcSwmSpd();
        pass = ko_movPass();
        if (pass == KO1_POINT_REACHED) {
            fields.mPointReached = 1;
            fields.mSkipMovement = 1;
            return;
        }
        if (pass == KO1_PATH_END) {
            fields.mPointReached = 1;
            fields.mMoveMode = KO1_MOVE_WAIT;
            fields.mSkipMovement = 1;
            return;
        }
        break;
    case KO1_MOVE_JUMP:
        cLib_chaseF(&speedF, 0.1f, fields.mAcceleration);
        if (fields.mPreviousGroundHit == 0 && mObjAcch.ChkGroundHit()) {
            fields.mMoveMode = fields.mPreviousMoveMode;
            speedF = 0.0f;
            fields.mPointReached = 1;
            speed.y = 0.0f;
            fields.mSkipMovement = 1;
            gravity = -4.5f;
            return;
        }
        break;
    }
    if (fields.mPointReached != 0) {
        fields.mSkipMovement = 1;
    }
}

/* 02275D90 */
void daNpc_Ko1_c::chk_routeAngle(cXyz* i_wallNrm, be<s16>* io_angle) {
    Ko1MovementFields& fields = Ko1B_fields(this);
    gabi::Local<cXyz> side;
    Ko1B_outprod(&fields.mGroundNormal, side, i_wallNrm);
    s16 angle = cM_atan2s(side->x, side->z);
    if (!(fields.mGroundNormal.y < 0.999f) && cLib_distanceAngleS(angle, *io_angle) > 0x4000) {
        angle = (s16)(angle - 0x8000);
    } else if (side->y * (fields.mMoveTarget.y - current.pos.y) < 0.0f) {
        angle = (s16)(angle - 0x8000);
    }
    *io_angle = angle;
}

/* 02275E44 */
void daNpc_Ko1_c::routeWallCheck(cXyz* i_start, cXyz* i_end, be<s16>* io_angle) {
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk, l_linChk_vt, false);
    dBgS_LinChk_Set(linChk, i_start, i_end, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        cXyz* pla = (cXyz*)dBgS_GetTriPla(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(linChk.get()));
        if (pla != nullptr) {
            chk_routeAngle(pla, io_angle);
        }
    }
    /* ~dBgS_LinChk */
    linChk->dBgS_PolyPassChk::__vtbl = l_linChk_vt.v58;
    linChk->dBgS_GrpPassChk::__vtbl = l_group_vtable;
    linChk->cBgS_PolyInfo::__vtbl = l_poly_vtable;
    cBgS_LinChk_dt(linChk, 0);
}

/* 02275B80 */
f32 daNpc_Ko1_c::chk_ForwardGroundY(s16 i_angle) {
    cXyz* pla = (cXyz*)dBgS_GetTriPla(dComIfG_Bgsp(), &mAcchCir); /* the wall polygon's plane normal */
    if (pla != nullptr && cLib_distanceAngleS(i_angle, cM_atan2s(pla->x, pla->z)) > 0x4000) {
        gabi::Local<dBgS_GndChk> gndChk;
        dBgS_GndChk_ct(gndChk, l_gndChk_vt, false);
        /* gndChk.SetPos(pos + 80 in direction i_angle, 80 up) */
        f32 y = current.pos.y + 80.0f;
        f32 x = gabi::fmadds(80.0f, cM_ssin((u16)i_angle), current.pos.x);
        f32 z = gabi::fmadds(80.0f, cM_scos((u16)i_angle), current.pos.z);
        gndChk->mFlag &= ~2u;
        gndChk->m_pos.z = z;
        gndChk->m_pos.x = x;
        gndChk->m_pos.y = y;
        f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
        /* ~dBgS_GndChk */
        gndChk->cBgS_PolyInfo::__vtbl = l_gndChk_vt.v20;
        gndChk->dBgS_PolyPassChk::__vtbl = l_gndChk_vt.v40;
        gndChk->dBgS_GrpPassChk::__vtbl = l_group_vtable;
        Ko1B_baseGroundDelete(gndChk, 0);
        return groundY;
    }
    return -10000000.0f;
}

/* 02275D28 */
f32 daNpc_Ko1_c::chk_wallJump(s16 i_angle) {
    f32 h = chk_ForwardGroundY(i_angle);
    if (0.0f < h && h < 100.0f) { /* bge: NaN takes the -1 path */
        return std_sqrtf(h) * 3.2f;
    }
    return -1.0f;
}

/* 02275F60 */
/* i_distSq (squared xz distance to the target) is not used */
BOOL daNpc_Ko1_c::routeCheck(f32 i_distSq, be<s16>* io_angle) {
    Ko1MovementFields& fields = Ko1B_fields(this);
    if (fields.mMoveMode != KO1_MOVE_SWIM && mObjAcch.ChkWallHit()) {
        chk_wallJump(*io_angle);
    }
    f32 y = current.pos.y + 80.0f;
    u16 a = (u16)*io_angle;
    gabi::Local<cXyz> start;
    start->set(current.pos.x, y, current.pos.z);
    gabi::Local<cXyz> end;
    end->set(gabi::fmadds(80.0f, cM_ssin(a), current.pos.x), y, gabi::fmadds(80.0f, cM_scos(a), current.pos.z));
    routeWallCheck(start, end, io_angle);
    return TRUE;
}

/* 0227577C */
void daNpc_Ko1_c::event_actionInit(s32 i_staffIdx) {
    Ko1MovementFields& fields = Ko1B_fields(this);
    be<s32>* prm = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "ActNo");
    if (prm != nullptr) {
        fields.mCutAction = (u8)(s32)*prm;
    }
}

/* 022757DC */
BOOL daNpc_Ko1_c::event_action() {
    return TRUE;
}

/* 022757E4 */
void daNpc_Ko1_c::privateCut(s32 i_staffIdx) {
    Ko1MovementFields& fields = Ko1B_fields(this);
    if (i_staffIdx == -1) {
        return;
    }
    s8 actIdx = dComIfGp_evmng_getMyActIdx(i_staffIdx, gabi::ea(a_cut_tbl), 1, TRUE, 0);
    fields.mCutIndex = actIdx;
    if (actIdx != -1) {
        if (dComIfGp_evmng_getIsAddvance(i_staffIdx) && fields.mCutIndex == 0) {
            event_actionInit(i_staffIdx);
        }
        if (fields.mCutIndex == 0 && !event_action()) {
            return;
        }
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 02275738 */
void daNpc_Ko1_c::endEvent() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    dComIfGp_event_reset();
    fields.mAnimationAttribute = 0xFF;
    fields.mAnimationTag = 0xFF;
}

/* 022756F8 */
s32 daNpc_Ko1_c::isEventEntry() {
    const char* name = Ko1B_staffName(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 02275B28 */
void daNpc_Ko1_c::event_proc(s32 i_staffIdx) {
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
    lookBack();
}

/* 02275544 */
u8 daNpc_Ko1_c::demo() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    if (demoActorID == 0) {
        if (fields.mInDemo != 0) {
            fields.mInDemo = 0;
        }
        return 0;
    }
    u8 id = demoActorID;
    fields.mInDemo = 1;
    // HD: inline range check and asserted demo object; reload after the assertion.
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = l_demo_object;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = l_demo_object;
        }
        demo_actor = Ko1B_demoActor(obj, id); /* dDemo_object_c::getActor */
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = Ko1B_demoPattern(demo_actor, "Ko"); /* getP_BtpData */
        if (demopattern != nullptr) {
            m_hed_tex_pttrn = demopattern;
            J3DModelData* md = fields.mpHeadMorf->getModel()->getModelData();
            if (mDoExt_btpAnm_init(mBtpAnm, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNum = 4;
            }
        }
    }
    Ko1B_setDemoData(this, 0x6A, mpMorf, "Ko", 0, nullptr, 0, 0);
    return fields.mInDemo;
}

/* 0227684C */
BOOL daNpc_Ko1_c::_draw() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    J3DModel* hedModel = fields.mpHeadMorf->getModel();
    J3DModelData* hedData = hedModel->getModelData();
    J3DModel* model = mpMorf->getModel();
    if (fields.mHidden != 0 || fields.mDrawHidden != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: unused */
    if ((u32)(s32)mType <= 1) {
        mpMorf->entryDL();
    }
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, hedData, mBlinkFrame);
    fields.mpHeadMorf->entryDL();
    Ko1B_view<Ko1ModelAnimations>(hedData)->mpBtpAnimation = 0; /* mBtpAnm.remove(hedData) */
    setLightTevColorType(dKy_getEnvlight(), hedModel, &tevStr);
    if (fields.mpBalloonMorf.get() != nullptr) {
        fields.mpBalloonMorf->entryDL();
        mDoExt_McaMorf* morf = fields.mpBalloonMorf;
        setLightTevColorType(dKy_getEnvlight(), morf->getModel(), &tevStr);
    }
    if (fields.mpItemModel.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), fields.mpItemModel, &tevStr);
        mDoExt_modelEntryDL(fields.mpItemModel);
    }
    /* HD: no shadowDraw() */
    switch ((u32)(s32)mType) {
    case KO1_JOEL:
        dSnap_RegistFig(0x52, this, 1.0f, 1.0f, 1.0f);
        break;
    case KO1_ZILL:
        dSnap_RegistFig(0x51, this, 1.0f, 1.0f, 1.0f);
        break;
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_movement_parameters[mType].mDebugColors) {
        Ko1B_initializeColor(l_red_guard, l_red_color, l_red_initial);
        Ko1B_initializeColor(l_blue_guard, l_blue_color, l_blue_initial);
        Ko1B_initializeColor(l_green_guard, l_green_color, l_green_initial);
        Ko1B_initializeColor(l_red_guard, l_red_color, l_red_initial);
        Ko1B_initializeColor(l_green_guard, l_green_color, l_green_initial);
        Ko1B_initializeColor(l_yellow_guard, l_yellow_color, l_yellow_initial);
    }
    return TRUE;
}

/* 022765B0 */
BOOL daNpc_Ko1_c::_execute() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    if (!mbRanExecute) {
        mInitialPos.copy(current.pos);
        mInitialAngle.x = current.angle.x;
        mInitialAngle.y = current.angle.y;
        mInitialAngle.z = current.angle.z;
        mbRanExecute = 1;
    }
    Ko1MovementParameters& hio = l_movement_parameters[mType];
    m_jnt.setParam(hio.mBackboneMinX, hio.mBackboneMaxX, hio.mBackboneMinY,
                   hio.mBackboneMaxY, hio.mHeadMinX, hio.mHeadMaxX,
                   hio.mHeadMinY, hio.mHeadMaxY, hio.mJointScale);
    if (fields.mHidden != 0 && demoActorID == 0) {
        return TRUE;
    }
    fields.mSkipMovement = 0;
    fields.mHidden = 0;
    partner_srch();
    checkOrder();
    if (!demo()) {
        s32 staffIdx;
        if (dComIfGp_event_runCheck() && eventInfo.mCommand != dEvtCmd_INTALK_e &&
            (staffIdx = isEventEntry()) >= 0) {
            event_proc(staffIdx);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!fields.mSkipMovement) {
            ko_nMove();
            fopAcM_posMoveF(this, &mStts.m_cc_move);
        }
        void* pla = dBgS_GetTriPla(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
        u8 keepShape = fields.mKeepShape;
        if (pla != nullptr) {
            fields.mGroundNormal.copy(*(cXyz*)pla); /* the ground plane normal */
        }
        if (!keepShape) {
            fields.mModelAngle.z = current.angle.z;
            shape_angle.y = current.angle.y;
            fields.mModelAngle.y = current.angle.y;
            shape_angle.x = current.angle.x;
            shape_angle.z = current.angle.z;
            fields.mModelAngle.x = current.angle.x;
        }
        gabi::Local<cXyz> diff;
        cXyz_mi(&current.pos, diff, &fields.mSpawnPos);
        gabi::Local<cXyz> diffXZ;
        diffXZ->set(diff->x, 0.0f, diff->z);
        if (std_sqrtf(PSVECSquareMag(diffXZ)) > 3000.0f) {
            fopAcM_delete(this);
            return TRUE;
        }
    }
    eventOrder();
    setMtx(0);
    if (!fields.mInDemo) {
        setCollision(30.0f, 80.0f);
    }
    return TRUE;
}
/* 022752D4 */
BOOL daNpc_Ko1_c::_delete() {
    Ko1MovementFields& fields = Ko1B_fields(this);
    dComIfG_resDelete(&mPhs, "Ko");
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr)
            mpMorf->stopZelAnime();
        if (fields.mpHeadMorf.get() != nullptr)
            fields.mpHeadMorf->stopZelAnime();
        if (fields.mpBalloonMorf.get() != nullptr)
            fields.mpBalloonMorf->stopZelAnime();
    }
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&mRipple);
    return TRUE;
}

/* 02275180 */
cPhs_State daNpc_Ko1_c::_create() {
    /* fopAcM_SetupActor(this, daNpc_Ko1_c) */
    if (!fopAcM_CheckCondition(this, 8 /* fopAcCnd_INIT_e */)) {
        if (this != nullptr) {
            Ko1B_constructNpc(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = l_actor_vtable;
            Ko1B_constructBtp(mBtpAnm);    /* mDoExt_btpAnm::mDoExt_btpAnm */
            Ko1B_constructEvent(&mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
            Ko1B_constructRipple(&mRipple);   /* dPa_rippleEcallBack::dPa_rippleEcallBack */
        }
        fopAcM_OnCondition(this, 8);
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, "Ko");
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }
    if (!Ko1B_charDecide(this, fopAcM_GetParam(this) & 0xFF) /* charDecide(param & 0xFF) */) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)Ko1B_checkCreateHeap), a_heap_size_tbl[mType])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 120.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return phase;
}

/* 022752D0 */
static cPhs_State daNpc_Ko1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Ko1_c*)i_this)->_create();
}

/* 02275354 */
static BOOL daNpc_Ko1_Delete(daNpc_Ko1_c* i_this) {
    return i_this->_delete();
}

/* 02276848 */
static BOOL daNpc_Ko1_Execute(daNpc_Ko1_c* i_this) {
    return i_this->_execute();
}

/* 02276B20 */
static BOOL daNpc_Ko1_Draw(daNpc_Ko1_c* i_this) {
    return i_this->_draw();
}

/* 02276B24 */
static BOOL daNpc_Ko1_IsDelete(daNpc_Ko1_c*) {
    return TRUE;
}


/* WWHD glue
ext 0201B080 void Ko1B_outprod(const cXyz*, cXyz*, const cXyz*)
ext 02008DAC void Ko1B_baseGroundDelete(void*, s32)
ext 025E535C BOOL Ko1B_play(mDoExt_McaMorf*, cXyz*, u32, s32)
ext 02527028 BOOL Ko1B_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, void*, u32, s8)
ext 0259E6D0 BOOL Ko1B_setPath(dNpc_PathRun_l*, u8, s8, u8)
ext 0259E838 BOOL Ko1B_checkPoint(dNpc_PathRun_l*, cXyz*, u32)
ext 0259ED58 BOOL Ko1B_nextPoint(dNpc_PathRun_l*)
ext 025A1458 void Ko1B_constructNpc(daNpc_Ko1_c*)
ext 025E7820 void Ko1B_constructBtp(void*)
ext 0259F740 void Ko1B_constructEvent(dNpc_EventCut_c*)
ext 025A9084 void Ko1B_constructRipple(dPa_rippleEcallBack_l*)
ext 02274250 u32 Ko1B_charDecide(daNpc_Ko1_c*, u32)
ext 0227424C BOOL Ko1B_checkCreateHeap(fopAc_ac_c*)
ext 02273170 void* Ko1B_searchHna(void*, void*)
ext 022731F0 void* Ko1B_searchBou(void*, void*)
ext 02273270 void* Ko1B_searchOb(void*, void*)
ext 02526E70 void* Ko1B_demoActor(u32, u8)
ext 02527828 J3DAnmTexPattern* Ko1B_demoPattern(void*, const char*)
ext 022743EC bool daNpc_Ko1_c::init_HNA_0()
ext 02274498 bool daNpc_Ko1_c::init_HNA_1()
ext 0227452C bool daNpc_Ko1_c::init_HNA_2()
ext 022745B4 bool daNpc_Ko1_c::init_HNA_3()
ext 02274658 bool daNpc_Ko1_c::init_HNA_4()
ext 022746E8 bool daNpc_Ko1_c::init_BOU_0()
ext 02274778 bool daNpc_Ko1_c::init_BOU_1()
ext 02274808 bool daNpc_Ko1_c::init_BOU_2()
ext 022748B0 bool daNpc_Ko1_c::init_BOU_3()
ext 02274948 void daNpc_Ko1_c::plyTexPttrnAnm()
ext 02274A1C void daNpc_Ko1_c::setAttention(u32)
str 1001C974 "m_object != (0)"
str 1001C984 "d_demo.h"
str 1001CB78 "Ko" in 02275180
str 1001CB7B "Ko" in 022752D4
str 1001CB7E "Ko"
str 1001CB84 "ActNo"
*/
