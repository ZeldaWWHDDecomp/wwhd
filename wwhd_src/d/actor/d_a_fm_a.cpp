/**
 * d_a_fm_a.cpp (WWHD)
 * Enemy - Floormaster, part A (02142988..02145070): attack data, grab helpers, collision, execute,
 * draw, the hide/under-foot/path/goal-keeper modes, isGrabFoot and moveRndBack.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_fm_a.h"

/* inlined into daFm_c::grabPlayer */
static inline f32 fm_abs_copy(cXyz* vector) {
    gabi::Local<cXyz> copy;
    copy->copy(*vector);
    return std_sqrtf(PSVECSquareMag(copy.get()));
}
static inline void fm_multVecZero(cXyz* out) {
    auto* matrix = reinterpret_cast<fm_a::MatrixWords*>(mDoMtx_stack_c::get());
    auto* words = reinterpret_cast<fm_a::VectorWords*>(out);
    words->x = u32(matrix->m[0][3]);
    words->y = u32(matrix->m[1][3]);
    words->z = u32(matrix->m[2][3]);
}
/* inlined into daFm_c::modeGoalKeeper */
static inline f32 fm_absXZ_mi(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> diff;
    cXyz_mi(a, diff.get(), b);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    auto* dst = reinterpret_cast<fm_a::VectorWords*>(xz.get());
    auto* src = reinterpret_cast<fm_a::VectorWords*>(diff.get());
    dst->x = u32(src->x);
    dst->z = u32(src->z);
    return std_sqrtf(PSVECSquareMag(xz.get()));
}

// Numeric actor and HIO fields retain the names from zeldaret/tww.

/* 02142988 */
void daFm_c::spAttackNone() {
    /* setBtAttackData(100.0f, 100.0f, 10000.0f, 0); setBtNowFrame(0.0f) */
    mBtStartFrame = 100.0f;
    mBtNowFrame = 0.0f;
    mBtEndFrame = 100.0f;
    mBtAttackType = 0;
    mBtMaxDis = 10000.0f;
}

/* 02142BC4 */
bool daFm_c::isNpc(fopAc_ac_c* i_actor) {
    if (i_actor == nullptr) {
        return false;
    }
    // HD: fopAcM_GetName checks for NULL (inline, twice)
    return (i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_CB1_e) ||
           (i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_MD_e);
}

/* 02142C0C */
void daFm_c::MtxToRot(Mtx34* i_mtx, csXyz* angle) {
    gabi::Local<Mtx34> mtx;
    mDoMtx_inverseTranspose(i_mtx, mtx);
    mDoMtx_MtxToRot(mtx, angle);
}

/* 0214399C */
static bool daFmExecute(daFm_c* i_this) {
    return i_this->_execute();
}

/* 0214418C */
static bool daFmDraw(daFm_c* i_this) {
    return i_this->_draw();
}

/* 02144190 */
void daFm_c::modeSwWaitInit() {
    fm_setAnm(this, 8, false);
    field_0x3E0 = fm_a::hio().field_0x120;
}

/* 021441D0 */
void daFm_c::resetInvKine() {
    if (isBodyAppear()) {
        field_0x390 = 0;
        for (int i = 0; i < 6; i++) {
            C_QUATSlerp(&field_0x330[i], fm_a::zeroQuatAddress(), &field_0x330[i], 0.05f);
        }
        morf_calc(mpMorf);
    }
}

/* 021444FC */
int daFm_c::setRnd(int param_1, int param_2) {
    f32 rnd = cM_rndF(std::fabs((f32)(param_2 - param_1)));
    return param_1 + gabi::ftoi(rnd);
}

/* 02144568 */
void daFm_c::modeHideInit() {
    fm_setAnm(this, 8, false);
    field_0x650 = setRnd(fm_a::hio().field_0x084, fm_a::hio().field_0x086);
}

/* 021445B8 */
void daFm_c::modeHide() {
    field_0x690.copy(current.pos);
    resetInvKine();
    if (fm_setHoleScale(this, fm_a::hio().field_0x120, 0.1f, fm_a::hio().field_0x128)) {
        current.pos.x = mBaseTarget->current.pos.x;
        current.pos.z = mBaseTarget->current.pos.z;
        if (fm_calcTimer(&field_0x650) == 0) {
            // HD: field_0xAE4 != 0 (GameCube: == 1)
            if (!(std::fabs(field_0x69C.y - mBaseTarget->current.pos.y) > fm_a::hio().field_0x0A8) && field_0xAE4 != 0) {
                fm_modeProc(this, PROC_INIT_e, fm_a::MODE_UNDERFOOT);
            }
        }
    }
}

/* 0214468C */
void daFm_c::modeUnderFootInit() {
    fm_setAnm(this, 8, false);
    field_0x650 = fm_a::hio().field_0x066;
    current.pos.x = mBaseTarget->current.pos.x;
    current.pos.z = mBaseTarget->current.pos.z;
    field_0x3E0 = fm_a::hio().field_0x120;
}

/* 021446EC */
bool daFm_c::isLinkControl() {
    fopAc_ac_c* p0 = dComIfGp_getPlayer(0);
    return p0 != dComIfGp_getLinkPlayer();
}

/* 021449C4 */
void daFm_c::modePathMoveInit() {
    fm_setAnm(this, 8, false);
    field_0x3E0 = fm_a::hio().field_0x124;
}

/* 02144A04 */
void daFm_c::turnToBaseTarget() {
    cLib_addCalcAngleS2(&shape_angle.y, field_0x9D0, 4, 0x800);
}

/* 02144B54 */
void daFm_c::modeGoalKeeperInit() {
    fm_setAnm(this, 8, false);
    field_0x3E0 = fm_a::hio().field_0x124;
}

/* 02144D88 */
void daFm_c::modeAppearInit() {
    fm_setAnm(this, 2, false);
    field_0x660.copy(current.pos);
    shape_angle.y = field_0x9D0;
    field_0x650 = 0x1E;
}

/* 0214472C */
bool daFm_c::isGrabFoot() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    if (std::fabs(current.pos.y - pLink->current.pos.y) > 10.0f) {
        return false;
    }
    if (fopAcM_searchActorDistanceXZ(this, pLink) > fm_a::hio().field_0x0DC) {
        return false;
    }
    if (pLink->speedF > 4.0f) {
        return false;
    }
    if (daPy_checkPlayerFly(pLink) || daPy_checkFrontRoll(pLink) || daPy_getGrabActorID(pLink) != -1 || isLinkControl()) {
        return false;
    }
    return true;
}

/* 02144838 */
void daFm_c::modeUnderFoot() {
    if (fm_calcTimer(&field_0x648) == 0) {
        field_0x660.copy(mBaseTarget->current.pos);
        field_0x648 = fm_a::hio().field_0x08A;
    }
    fm_addCalcPosXZ2(&current.pos, &field_0x660, 0.1f, fm_a::hio().field_0x0C0);

    field_0x690.copy(current.pos);
    resetInvKine();
    if (std::fabs(field_0x69C.y - mBaseTarget->current.pos.y) > fm_a::hio().field_0x0A8 || field_0xAE4 == 0) {
        fm_modeProc(this, PROC_INIT_e, fm_a::MODE_HIDE);
    } else if (fm_setHoleScale(this, fm_a::hio().field_0x124, 0.1f, fm_a::hio().field_0x12C)) {
        if (fm_calcTimer(&field_0x650) == 0) {
            if (isGrabFoot() && !dComIfGp_event_runCheck()) {
                fm_modeProc(this, PROC_INIT_e, fm_a::MODE_GRAB_FOOT_DEMO);
            } else if (field_0x9D4 < fm_a::hio().field_0x0E4 && field_0xAE5 == 0) {
                fm_modeProc(this, PROC_INIT_e, fm_a::MODE_APPEAR);
            } else {
                fm_modeProc(this, PROC_INIT_e, fm_a::MODE_HIDE);
            }
        }
    }
}

/* 02144A18 */
void daFm_c::modePathMove() {
    field_0x690.copy(current.pos);
    resetInvKine();
    turnToBaseTarget();
    if (isGrabFoot() && !dComIfGp_event_runCheck()) {
        fm_modeProc(this, PROC_INIT_e, fm_a::MODE_GRAB_FOOT_DEMO);
    } else {
        dLib_pathMove(&field_0x3B0, &field_0x3BC, mpPath, 3.0f, pathMove_CB, this);
        fm_addCalcPosXZ2(&current.pos, &field_0x3B0, (fm_a::hio().field_0x0D0 + 1.0f) * 0.005f * field_0x394, 20.0f);
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        if (field_0x9D4 < fm_a::hio().field_0x0E4 && std::fabs(pLink->current.pos.y - current.pos.y) < fm_a::hio().field_0x0A8 &&
            field_0xAE5 == 0) {
            fm_modeProc(this, PROC_INIT_e, fm_a::MODE_APPEAR);
        }
    }
}

/* 02144B94 */
void daFm_c::modeGoalKeeper() {
    if (field_0x2E5 && fm_endCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_FM_SUIKOMI_NPC" /* "DEFAULT_FM_SUIKOMI_NPC" */)) {
        fm_modeProc(this, PROC_INIT_e, fm_a::MODE_DISAPPEAR);
        return;
    }

    resetInvKine();
    turnToBaseTarget();
    if (isGrabFoot() && !dComIfGp_event_runCheck()) {
        fm_modeProc(this, PROC_INIT_e, fm_a::MODE_GRAB_FOOT_DEMO);
    } else if (field_0x2E4 != 0) {
        fm_modeProc(this, PROC_INIT_e, fm_a::MODE_APPEAR);
    } else {
        f32 temp = std::fabs(current.pos.y - mBaseTarget->current.pos.y);
        if (field_0x9D4 < fm_a::hio().field_0x0E8 && temp < fm_a::hio().field_0x0A8) {
            if (fm_absXZ_mi(&mBaseTarget->current.pos, &field_0x69C) < field_0x2E0 && field_0xAE5 == 0) {
                fm_modeProc(this, PROC_INIT_e, fm_a::MODE_APPEAR);
            }
        }
        field_0x690.copy(field_0x69C);
        fm_addCalcPosXZ2(&current.pos, &field_0x690, (fm_a::hio().field_0x0CC + 1.0f) * 0.005f * field_0x394, 40.0f);
    }
}

/* 021442A8 */
void daFm_c::modeSwWait() {
    resetInvKine();
    if (!dComIfGs_isSwitch(field_0x2D4, current.roomNo)) {
        return;
    }

    if (field_0x2E4 != 0) {
        fm_setHoleScale(this, fm_a::hio().field_0x124, 0.1f, fm_a::hio().field_0x128);
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            if (field_0x2E4 != 0 && fm_calcTimer(&field_0x650) != 0) {
                return;
            }
        } else {
            // HD: event by name (startCheckOld) and fopAcM_orderOtherEvent2(name, 1, 0xFFFF)
            if (fm_startCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_FM_SW_APEEAR" /* "DEFAULT_FM_SW_APEEAR" */) == 0) {
                fopAcM_orderOtherEvent2(this, "DEFAULT_FM_SW_APEEAR", 1, 0xFFFF);
                return;
            }
        }
    } else {
        if (fm_startCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_FM_SW_APEEAR") != 0) {
            field_0x2E5 = 1;
        }
        actor_status |= fopAcStts_UNK4000_e; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
    }

    switch ((u32)field_0x2D0) {
    case 0:
        fm_modeProc(this, PROC_INIT_e, fm_a::MODE_UNDERFOOT);
        break;
    case 1:
        if (fm_setHoleScale(this, fm_a::hio().field_0x124, 0.1f, fm_a::hio().field_0x128)) {
            fm_modeProc(this, PROC_INIT_e, fm_a::MODE_PATH_MOVE);
        }
        break;
    case 2:
        if (fm_setHoleScale(this, fm_a::hio().field_0x124, 0.1f, fm_a::hio().field_0x128)) {
            fm_modeProc(this, PROC_INIT_e, fm_a::MODE_GOAL_KEEPER);
        }
        break;
    }
}

/* 02144DE4 */
void daFm_c::moveRndBack() {
    cLib_distanceAngleS(shape_angle.y, field_0x9D0); /* unused angle */
    f32 temp = 100000.0f;
    if (mpActorTarget != nullptr) {
        temp = fopAcM_searchActorDistanceXZ(this, mpActorTarget);
    }

    bool moveDirectly = false;
    f32 e0 = fm_a::hio().field_0x0E0;
    if (!(field_0x9D4 > e0) || !(temp > e0)) {
        if (!isGrabFoot()) {
            s32 timer = fm_calcTimer(&field_0x648);
            if (field_0xAE4 == 0 || mObjAcch.ChkWallHit()) {
                s16 old = field_0x680;
                f32 r = cM_rndF(3.0f);
                field_0x680 = (s16)(old + (s16)gabi::ftoi(gabi::fmadds(r, 4096.0f, (f32)(old + 0x5000))));
            } else if (timer == 0) {
                f32 base = (f32)(shape_angle.y + 0x3000);
                f32 r = cM_rndF(5.0f);
                field_0x680 = (s16)gabi::ftoi(gabi::fmadds(r, 4096.0f, base));
            } else {
                moveDirectly = true;
            }
            if (!moveDirectly) {
                field_0x394 = cM_rndF(9.0f) + 1.0f;
                field_0x660.z = gabi::fmadds(fm_a::hio().field_0x0AC, cM_scos(field_0x680), field_0x660.z);
                field_0x660.x = gabi::fmadds(fm_a::hio().field_0x0AC, cM_ssin(field_0x680), field_0x660.x);
                field_0x648 = fm_a::hio().field_0x0A0;
            }
        }
    }

    if (moveDirectly || (field_0xAE4 != 0 && !mObjAcch.ChkWallHit())) {
        fm_addCalcPosXZ2(&current.pos, &field_0x660, 0.1f, fm_a::hio().field_0x0B0);
    }
}

/* 02143FF0 */
void daFm_c::holeDraw() {
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    J3DModelData* modelData = J3DModel_getModelData(mpModel);
    mBtkAnm.entry(modelData, mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    // HD: mBtkAnm.remove inline: clear the model data's texture-SRT animation
    reinterpret_cast<fm_a::ModelData*>(J3DModel_getModelData(mpModel))->mTextureSRTAnimation = 0;
}

/* 02144058 */
void daFm_c::bodyDraw() {
    J3DModel* model = mpMorf->getModel();
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, model, &tevStr);
    if (mEnemyIce.mFreezeTimer > 0x14) {
        dMat_control_iceEntryDL(mpMorf, -1, mInvisibleModel);
    } else {
        mpMorf->entryDL();
    }
}

/* 021440D8 */
bool daFm_c::_draw() {
    debugDraw();
    if (field_0x3E0 > 0.015f) {
        holeDraw();
    }
    if (isBodyAppear()) {
        bodyDraw();
        dSnap_RegistFig_pos(DSNAP_TYPE_FM, this, &field_0x61C, shape_angle.y, 1.0f, 1.0f, 1.0f);
    }
    return true;
}

/* 02142E0C */
void daFm_c::grabBomb() {
    fopAc_ac_c* target = mpActorTarget;
    // HD: fopAcM_GetName checks for NULL
    if (target != nullptr && fpcM_GetName(target) == fpcNm_BOMB_e) {
        gabi::Local<cXyz> temp_pos;
        temp_pos->x = 5.0f;
        temp_pos->y = -10.0f;
        temp_pos->z = 5.0f;
        gabi::Local<cXyz> pos;
        cXyz_pl(&fm_a::hio().field_0x038, pos, temp_pos);
        PSMTXCopy(fm_a::anmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        PSMTXMultVec(mDoMtx_stack_c::get(), pos, &mpActorTarget->current.pos);
        mpActorTarget->gravity = 0.0f;
        mpActorTarget->speedF = 0.0f;
        mpActorTarget->speed.x = 0.0f;
        mpActorTarget->speed.y = 0.0f;
        mpActorTarget->speed.z = 0.0f;
    }
}

/* 021432B8 */
void daFm_c::setCollision() {
    if (isBodyAppear()) {
        if (dComIfGp_checkPlayerStatus0(0, 0x402)) {
            // HD: a fixed radius of 120 (GameCube: HIO radius * 1.5), set after the centre
            mSph.SetC(&field_0x61C);
            mSph.SetR(120.0f);
        } else {
            // HD: on odd frames the sphere is centred 30 above the actor's position
            if (fm_a::counter() & 1) {
                cM3dGSph_SetC_xyz(&mSph.mSph, current.pos.x, current.pos.y + 30.0f, current.pos.z);
            } else {
                mSph.SetC(&field_0x61C);
            }
            mSph.SetR(fm_a::hio().field_0x0C8);
        }
        dComIfG_Ccsp_Set(&mSph);
    }

    if (field_0x3E0 > 0.015f) { /* isHoleAppear */
        mCyl.SetR(field_0x3E0);
        mCyl.SetH(150.0f);
        mCyl.SetC(&current.pos);
        dComIfG_Ccsp_Set(&mCyl);
    }
}

/* 021429BC */
void daFm_c::grabPlayer() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> head;
    head->copy(static_cast<fm_a::Player*>(pLink)->mHeadTopPos);
    gabi::Local<cXyz> offset; /* unused */
    cXyz_mi(head, offset, &pLink->current.pos);
    gabi::Local<cXyz> temp;
    temp->x = 0.0f;
    temp->y = 0.0f;
    temp->z = 0.0f;
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, 0, 0);
    switch ((u32)(s32)mAnmPrmIdx) {
    case 4:
    case 6: {
        fopAc_ac_c* pLink2 = dComIfGp_getLinkPlayer();
        gabi::Local<cXyz> head2;
        head2->copy(static_cast<fm_a::Player*>(pLink2)->mHeadTopPos);
        gabi::Local<cXyz> offset2;
        cXyz_mi(head2, offset2, &pLink2->current.pos);
        temp->y = 0.0f;
        temp->x = 5.0f;
        temp->z = 10.0f;
        angle->x = -3000;
        angle->y = 0;
        angle->z = 7000;
        gabi::Local<cXyz> temp2;
        cXyz_pl(&fm_a::hio().field_0x038, temp2, temp);
        f32 ty = temp2->y;
        f32 tz = temp2->z;
        f32 tx = temp2->x;
        ty = ty - fm_abs_copy(offset2);

        PSMTXCopy(fm_a::anmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angle->x, angle->y, angle->z);
        mDoMtx_stack_c::transM(tx, ty, tz);
        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6BC);
        daPy_setPlayerPosAndAngle_mtx(pLink2, mDoMtx_stack_c::get());
    }
    case 5:
    default:
        break;
    }
}

/* 02142C4C */
void daFm_c::grabNPC() {
    if (isNpc(mpActorTarget) == true) {
        gabi::Local<cXyz> temp;
        temp->x = 0.0f;
        temp->y = 0.0f;
        temp->z = 0.0f;
        gabi::Local<csXyz> angle;
        csXyz_ct(angle, 0, 0, 0);
        fopAc_ac_c* target = mpActorTarget;
        // HD: fopAcM_GetName checks for NULL
        if (target != nullptr) {
            if (fpcM_GetName(target) == fpcNm_NPC_CB1_e) {
                temp->set(-15.0f, -10.0f, 8.0f);
                angle->x = 0;
                angle->y = 16000;
                angle->z = -4000;
            }
            if (target != nullptr && fpcM_GetName(target) == fpcNm_NPC_MD_e) {
                temp->set(10.0f, 0.0f, 5.0f);
                angle->x = 4000;
                angle->y = 18000;
                angle->z = -4000;
            }
        }
        gabi::Local<cXyz> temp2;
        cXyz_pl(&fm_a::hio().field_0x038, temp2, temp);
        PSMTXCopy(fm_a::anmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(temp2->x, temp2->y, temp2->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angle->x, angle->y, angle->z);
        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6BC);
        fm_multVecZero(&mpActorTarget->current.pos);
        MtxToRot(mDoMtx_stack_c::get(), &mpActorTarget->shape_angle);
    }
}

/* 02142EF4 */
void daFm_c::grabTsubo() {
    fopAc_ac_c* tsubo = mpActorTarget;
    // HD: fopAcM_GetName checks for NULL
    if (tsubo != nullptr && fpcM_GetName(tsubo) == fpcNm_TSUBO_e) {
        gabi::Local<cXyz> pos;
        pos->x = 0.0f;
        pos->y = 0.0f;
        pos->z = 0.0f;
        gabi::Local<csXyz> angle;
        csXyz_ct(angle, 0, 0, 0);
        switch (daObj_PrmAbstract(tsubo, 4, 0x18)) { /* daTsubo::Act_c::prm_get_type */
        case 0:
            pos->set(25.0f, -25.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 5000;
            break;
        case 1:
        case 2:
            pos->set(55.0f, -55.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 5000;
            break;
        case 4:
            pos->set(50.0f, -45.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 7000;
            break;
        case 5:
            pos->set(0.0f, -15.0f, 0.0f);
            angle->x = 0; angle->y = 16000; angle->z = 0;
            break;
        case 6:
            pos->set(40.0f, -20.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 7000;
            break;
        }

        gabi::Local<cXyz> pos2;
        cXyz_pl(&fm_a::hio().field_0x038, pos2, pos);
        PSMTXCopy(fm_a::anmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(pos2->x, pos2->y, pos2->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angle->x, angle->y, angle->z);
        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6BC);
        fm_multVecZero(&mpActorTarget->current.pos);
        MtxToRot(mDoMtx_stack_c::get(), &mpActorTarget->shape_angle);
        /* tsubo->tevStr.mEnvrIdxOverride = tevStr.mEnvrIdxOverride */
        tsubo->tevStr.mEnvrIdxOverride = u8(tevStr.mEnvrIdxOverride);
    }
}

/* 02143418 */
bool daFm_c::_execute() {
    fm_holeExecute(this);
    fopAcM_SearchByID_out(mProcId, &mpActorTarget);
    fopAc_ac_c* bomb = mpActorTarget;
    // HD: fopAcM_GetName checks for NULL
    if (bomb != nullptr && bomb != nullptr && fpcM_GetName(bomb) == fpcNm_BOMB_e && daBomb_chk_state(bomb, 0)) {
        mpActorTarget = nullptr;
        mProcId = 0;
        fm_modeProc(this, PROC_INIT_e, fm_a::MODE_WAIT);
    }

    if (enemy_ice(&mEnemyIce)) {
        fm_iceProc(this);
        return true;
    }

    fm_setBaseTarget(this);
    if (mBaseTarget == nullptr) {
        fm_bodySetMtx(this);
        fm_holeSetMtx(this);
        morf_calc(mpMorf);
        return true;
    }

    u8 area = fm_areaCheck(this);
    field_0xAE5 = 0;
    field_0xAE4 = area;

    gabi::Local<cXyz> temp;
    temp->copy(current.pos);
    temp->y = current.pos.y + 100.0f;
    gabi::Local<cXyz> temp2;
    temp2->copy(mBaseTarget->current.pos);
    temp2->y = temp2->y + 100.0f;

    if (field_0x9D4 < fm_a::hio().field_0x0E8 && fm_lineCheck(this, temp, temp2) != 0) {
        field_0xAE5 = 1;
    }
    if (field_0x2E4 != 0) {
        field_0xAE5 = 0;
    }

    if (mpActorTarget != nullptr) {
        if (mMode == fm_a::MODE_ATTACK && mAnmPrmIdx == 4 && fm_isLink(this, mpActorTarget)) {
            fm_spAttackVJump(this);
        } else if (mAnmPrmIdx == 0xB || mAnmPrmIdx == 0xC) {
            fm_spAttackJump(this);
        } else {
            spAttackNone();
        }
    }
    fm_modeProc(this, PROC_EXEC_e, fm_a::MODE_UNUSED_EXEC);
    fm_setAnm(this, 0xF, false);
    morf_play(mpMorf, &current.pos, 0, 0);
    morf_calc(mpMorf);

    if (field_0x684 != 0) {
        if (!fopAcM_SearchByID_out(mProcId, &mpActorTarget)) {
            field_0x684 = 0;
        }
        switch ((u32)field_0x684) {
        case 1:
            grabPlayer();
            break;
        case 4:
            grabNPC();
            break;
        case 2:
            grabBomb();
            break;
        case 3:
            grabTsubo();
            break;
        }
    }
    field_0x660.y = current.pos.y;
    fopAcM_posMoveF(this, reinterpret_cast<cXyz*>(&mStts)); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());

    fm_bodySetMtx(this);
    fm_holeSetMtx(this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    fm_setAttention(this);
    setCollision();
    return true;
}

// HD: debug calls were removed, but their guarded colour copies remain.
static const GXColor colorAInitial = {0, 0, 255, 128};
static GXColor colorA = {};
static FmColorGuard colorAGuard = {0};
static const GXColor colorBInitial = {0, 255, 0, 128};
static GXColor colorB = {};
static FmColorGuard colorBGuard = {0};
static const GXColor colorCInitial = {255, 0, 0, 128};
static GXColor colorC = {};
static FmColorGuard colorCGuard = {0};
static const GXColor colorDInitial = {255, 0, 255, 128};
static GXColor colorD = {};
static FmColorGuard colorDGuard = {0};
static const GXColor colorEInitial = {0, 255, 255, 128};
static GXColor colorE = {};
static FmColorGuard colorEGuard = {0};
static const GXColor colorFInitial = {255, 255, 0, 128};
static GXColor colorF = {};
static FmColorGuard colorFGuard = {0};
static inline void fm_static_color(be<u32>& guard, GXColor* dst, const GXColor* src) {
    if (guard == 0) {
        guard = 1;
        fm_copyColor(dst, src, 4);
    }
}

/* 021439A0 */
// HD: only debug axes, conditional static-colour initialization and accessor calls remain.
void daFm_c::debugDraw() {
    if (fm_a::hio().field_0x004 != 0 && isBodyAppear()) {
        if (mpActorTarget != nullptr) {
            fm_static_color(colorAGuard, &colorA, &colorAInitial);
        }
        if (mBaseTarget != nullptr) {
            fm_static_color(colorBGuard, &colorB, &colorBInitial);
        }
    }
    if (fm_a::hio().field_0x00C != 0) {
        for (int i = 0; i != 12; i++) {
        }
        fm_static_color(colorAGuard, &colorA, &colorAInitial);
        fm_static_color(colorAGuard, &colorA, &colorAInitial);
        fm_static_color(colorCGuard, &colorC, &colorCInitial);
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
        fm_static_color(colorDGuard, &colorD, &colorDInitial);
        dComIfGp_get();
    }
    if (fm_a::hio().field_0x00A != 0) {
        fm_static_color(colorEGuard, &colorE, &colorEInitial);
        if (mMode == fm_a::MODE_GRAB && field_0x684 == 4) {
            dComIfGp_get();
            fm_static_color(colorDGuard, &colorD, &colorDInitial);
        }
    }
    if (fm_a::hio().field_0x009 != 0) {
        fm_static_color(colorAGuard, &colorA, &colorAInitial);
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
        fm_static_color(colorCGuard, &colorC, &colorCInitial);
        fm_static_color(colorFGuard, &colorF, &colorFInitial);
        fm_static_color(colorCGuard, &colorC, &colorCInitial);
        fm_static_color(colorCGuard, &colorC, &colorCInitial);
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
        fm_static_color(colorCGuard, &colorC, &colorCInitial);
    }
    if (fm_a::hio().field_0x00B != 0) {
        for (int i = 0; i < 6; i++) {
            if (i == 4) {
                continue;
            }
            fm_static_color(colorCGuard, &colorC, &colorCInitial);
            fm_static_color(colorBGuard, &colorB, &colorBInitial);
            fm_static_color(colorBGuard, &colorB, &colorBInitial);
        }
    }
    if (fm_a::hio().field_0x006 != 0) {
        dLib_debugDrawAxis(&field_0x6BC, 100.0f);
    }
    if (fm_a::hio().field_0x007 != 0) {
        gabi::Local<Mtx34> mtx;
        J3DModel* mdl = fm_a::actorModel(this); /* fopAc_ac_c::model */
        PSMTXCopy(fm_a::anmMtx(mdl, FM_JNT_TE_e), mtx);
        dLib_debugDrawAxis(mtx, 100.0f);
    }
    if (fm_a::hio().field_0x005 != 0) {
        dComIfGp_get(); /* daPy_getPlayerLinkActorClass()->getHeadTopPos() */
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
        fm_static_color(colorAGuard, &colorA, &colorAInitial);
        fm_static_color(colorCGuard, &colorC, &colorCInitial);
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
        fm_static_color(colorCGuard, &colorC, &colorCInitial);
        fm_static_color(colorBGuard, &colorB, &colorBInitial);
    }
}

/* WWHD glue
ext 021418D0 void fm_setAnmFull(daFm_c* a, s32 idx, u32 force)
ext 02142450 bool fm_setHoleScale(daFm_c* a, f32 x, f32 y, f32 z)
ext 021412BC void fm_modeProc(daFm_c* a, s32 proc, s32 mode)
ext 02142200 void fm_holeExecute(daFm_c* a)
ext 021424D0 void fm_iceProc(daFm_c* a)
ext 02141714 void fm_setBaseTarget(daFm_c* a)
ext 0214153C void fm_bodySetMtx(daFm_c* a)
ext 0214162C void fm_holeSetMtx(daFm_c* a)
ext 0214264C u8 fm_areaCheck(daFm_c* a)
ext 021425BC u32 fm_lineCheck(daFm_c* a, cXyz* p1, cXyz* p2)
ext 02141280 u32 fm_isLink(daFm_c* a, fopAc_ac_c* t)
ext 021422A4 void fm_setAttention(daFm_c* a)
ext 02142870 void fm_spAttackVJump(daFm_c* a)
ext 021428FC void fm_spAttackJump(daFm_c* a)
ext 020CB92C u32 daBomb_chk_state(fopAc_ac_c* b, u32 st)
ext 025E535C void morf_play(mDoExt_McaMorf* m, cXyz* pos, u32 a, s32 b)
ext 028E9BC0 void C_QUATSlerp(Quaternion_fm* p, u32 q, Quaternion_fm* r, f32 t)
ext 025E55A0 void morf_calc(mDoExt_McaMorf* m)
ext 02048038 u32 daObj_PrmAbstract(fopAc_ac_c* a, u32 w, u32 sh)
ext 02018F3C void cM3dGSph_SetC_xyz(cM3dGSph* s, f32 x, f32 y, f32 z)
ext 02587D24 void dLib_pathMove(cXyz* pos, be<s8>* pnt, dPath* path, f32 spd, u32 cb, void* data)
ext 025BEBB8 void dSnap_RegistFig_pos(u32 type, fopAc_ac_c* actor, cXyz* pos, s16 angle, f32 x, f32 y, f32 z)
ext 0211D2F8 s32 fm_calcTimer(be<s32>* timer)
ext 0200F268 void fm_addCalcPosXZ2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep)
ext 025F20B0 void mDoMtx_inverseTranspose(Mtx34* src, Mtx34* dst)
ext 025F232C void mDoMtx_MtxToRot(Mtx34* src, csXyz* dst)
ext 0259138C void dMat_control_iceEntryDL(mDoExt_McaMorf* morf, s32 param, void* invisible)
ext 025D54C4 bool fopAcM_SearchByID_out(u32 id, gptr<fopAc_ac_c>* out)
ext 020402C8 BOOL enemy_ice(enemyice_l* ice)
ext 02587514 void dLib_debugDrawAxis(Mtx34* matrix, f32 scale)
ext 025445B8 BOOL fm_startCheckOld(void* manager, const char* name)
ext 0254457C BOOL fm_endCheckOld(void* manager, const char* name)
ext 025D77DC void fopAcM_orderOtherEvent2(fopAc_ac_c* actor, const char* name, u16 flag, u16 hind)
ext C000A848 void fm_copyColor(GXColor* dst, const GXColor* src, u32 size)
data colorAInitial 1000F23C
data colorA 101FEBF8 nocheck
data colorAGuard 101FDAC0 nocheck
data colorBInitial 1000F240
data colorB 101FEBEC nocheck
data colorBGuard 101FDA48 nocheck
data colorCInitial 1000F244
data colorC 101FEBF4 nocheck
data colorCGuard 101FDA50 nocheck
data colorDInitial 1000F248
data colorD 101FEBFC nocheck
data colorDGuard 101FDAC4 nocheck
data colorEInitial 1000F24C
data colorE 101FEBF0 nocheck
data colorEGuard 101FDA4C nocheck
data colorFInitial 1000F250
data colorF 101FEBE8 nocheck
data colorFGuard 101FDA44 nocheck
str 1000F878 "DEFAULT_FM_SUIKOMI_NPC"
str 1000F854 "DEFAULT_FM_SW_APEEAR"
*/
