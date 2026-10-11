/**
 * d_a_am.cpp (WWHD)
 * Enemy - Armos
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_am.cpp) to the WWHD layout and verified against cking.rpx.
 * daAM_Execute (with medama_move, medama_atari_check and the four action functions inlined) is
 * in d_a_am_exec.cpp.
 */
#include "d/actor/d_a_am.h"

// HD: one set of compiler data for both sources of this translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x10006DEC,
    .objAcch = {0x10006E34, 0x10006E54, 0x10006E44},
    .objAcchDt = {0x10006E44, 0x10006E54},
    .headerStatics = {0x104613C0, 0x1018F9E0, 0x104613CC},
};

const dBgS_LinChk_vt AM_LINCHK_VT = {0x10006E64, 0x10006E74, 0x10006E94, 0x10006E84};

// HD: image-checked packed flags match the GameCube source records.
static const AmSphereSource eye_co_sph_src = {
    0x0, 0x1000, 0x0, 0x0, 0x81c4000, 0x3, 0x0, 0x0, 0x0, 0x0, 0x6, 0x0, {0.0f, 0.0f, 0.0f}, 15.0f,
};
static const AmSphereSource mouth_co_sph_src = {
    0x0, 0x1000, 0x0, 0x0, 0x0, 0x0, 0x145, 0x0, 0x0, 0x0, 0x2, 0x0, {0.0f, 0.0f, 0.0f}, 15.0f,
};
static const AmCylinderSource body_co_cyl_src = {
    0x0, 0x1000,  0x0, 0x0, 0xff1dfeff,         0x3,   0x75, 0x0,
    0x0, 0xc0000, 0x3, 0x0, {0.0f, 0.0f, 0.0f}, 15.0f, 0.0f,
};
static const AmCylinderSource sword_co_cyl_src = {
    0x0, 0x1000, 0x2000000,          0xf,   0x0,  0x0, 0x75, 0x600, 0x0, 0xc0000,
    0x3, 0x0,    {0.0f, 0.0f, 0.0f}, 15.0f, 0.0f,
};
static const f32 cyl2_eye_offset[] = {0.0f, 0.0f, 70.0f, 0.0f, 0.0f, 80.0f};
// HD: autoglue checks the guest address of cyl2_eye_offset in this record.
static const AmJointHitSource search_data[] = {{2, 5, 5.0f, 0x1018F8C0}};

/* a float moved by lfs/stfs without arithmetic: the recompiled code copies the bits */

/* model->setAnmMtx(jntNo, *calc_mtx); cMtx_copy(*calc_mtx, J3DSys::mCurrentMtx) */
static inline void am_node_store(J3DModel* model, s32 jntNo) {
    mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
    PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
}

static am_class* am_class_ct(am_class*);

/* 02048714 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        am_class* i_this = static_cast<am_class*>(model->getUserArea());
        u32 jntNo = reinterpret_cast<AmJointData*>(joint)->mJntNo;
        if (i_this != nullptr) {
            if (jntNo >= AM_JNT_KOSI_e && jntNo <= AM_JNT_EYE_e) {
                PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            }
            cXyz offset;
            switch (jntNo) {
            case AM_JNT_KOSI_e: /* waist */
                offset.set(0.0f, 240.0f, 60.0f);
                MtxPosition(&offset, &i_this->mEyeballPos);
                offset.set(0.0f, 150.0f, 70.0f);
                MtxPosition(&offset, &i_this->mMouthPos);
                offset.set(0.0f, 0.0f, 0.0f);
                MtxPosition(&offset, &i_this->mWaistPos);
                break;
            case AM_JNT_AGO_e: /* jaw */
                offset.set(0.0f, 0.0f, 0.0f);
                MtxPosition(&offset, &i_this->mJawPos);
                break;
            case AM_JNT_EYE_e:
                cMtx_YrotM(calc_mtx(), i_this->mEyeRot.y);
                cMtx_XrotM(calc_mtx(), i_this->mEyeRot.x);
                break;
            }
            if (jntNo >= AM_JNT_KOSI_e && jntNo <= AM_JNT_EYE_e) {
                am_node_store(model, jntNo);
            }
        }
    }
    return TRUE;
}

/* 02048A28 */
void draw_SUB(am_class* i_this) {
    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    i_this->mpMorf->calc();

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
}

/* 02048B3C */
// HD: HD: no simple shadow (dComIfGd_setSimpleShadow2 is gone)
static BOOL daAM_Draw(am_class* i_this) {
    J3DModel* model = i_this->mpMorf->getModel();
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);

    dSnap_RegistFig(DSNAP_TYPE_AM, i_this, 1.0f, 1.0f, 1.0f);

    i_this->mpMorf->entryDL();
    return TRUE;
}

/* 02048BAC */
void anm_init(am_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed,
              int soundFileIdx) {
    i_this->mCurrBckIdx = bckFileIdx;
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Am", bckFileIdx);
        void* snd = dComIfG_getObjectRes("Am", soundFileIdx);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Am", bckFileIdx);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 0204BDCC */
void body_atari_check(am_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */

    i_this->mStts.Move();

    if (i_this->mBodyCyl.ChkTgHit() || i_this->mNeedleCyl.ChkTgHit()) {
        if (i_this->mbIsBodyBeingHit) {
            return;
        }

        void* hitObj;
        if (i_this->mBodyCyl.ChkTgHit()) {
            hitObj = i_this->mBodyCyl.GetTgHitObj();
        } else {
            hitObj = i_this->mNeedleCyl.GetTgHitObj();
        }
        if (!hitObj) {
            return;
        }
        i_this->mbIsBodyBeingHit = 1;

        switch (static_cast<dCcD_GObjInf*>(hitObj)->mAtType) {
        case AT_TYPE_SWORD:
        case AT_TYPE_MACHETE:
        case AT_TYPE_UNK800:
        case AT_TYPE_DARKNUT_SWORD:
        case AT_TYPE_MOBLIN_SPEAR:
            fopAcM_seStart(i_this, JA_SE_LK_SW_HIT_S, 0x42);
            break;
        case AT_TYPE_BOOMERANG:
        case AT_TYPE_BOKO_STICK:
        case AT_TYPE_UNK2000:
        case AT_TYPE_STALFOS_MACE:
            fopAcM_seStart(i_this, JA_SE_LK_W_WEP_HIT, 0x42);
            break;
        case AT_TYPE_SKULL_HAMMER:
            fopAcM_seStart(i_this, JA_SE_LK_HAMMER_HIT, 0x42);
            if (i_this->mStartsInactive == 1 && i_this->mSwitch != 0xFF &&
                !dComIfGs_isSwitch(i_this->mSwitch, dComIfGp_roomControl_getStayNo())) {
                return;
            }
            if (i_this->mAction == ACTION_HANDOU_MOVE) {
                return;
            }
            if (i_this->mAction == ACTION_ITAI_MOVE) {
                return;
            }
            i_this->mAction = ACTION_HANDOU_MOVE;
            i_this->mMode = MODE_HANDOU_MOVE_INIT;
            i_this->mHugeKnockback = 0;
            if (daPy_getCutType(player) == CUT_TYPE_HAMMER_SIDESWING) {
                /* the Skull Hammer's side swing knocks it back much farther */
                i_this->mHugeKnockback = 1;
            }
            break;
        default:
            fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x42);
            break;
        }
    } else {
        i_this->mbIsBodyBeingHit = 0;
    }
}

/* 02048CD8 */
void bomb_move_set(am_class* i_this, u8 alwaysMoveY) {
    if (i_this->mSwallowedActorPID == fpcM_ERROR_PROCESS_ID_e) {
        return;
    }
    fopAc_ac_c* swallowedActor = fopAcM_SearchByID(i_this->mSwallowedActorPID);
    if (!swallowedActor) {
        return;
    }

    cMtx_YrotS(calc_mtx(), i_this->shape_angle.y);

    cXyz mouthOffset;
    mouthOffset.set(0.0f, 120.0f, 40.0f);
    cXyz mouthPos;
    MtxPosition(&mouthOffset, &mouthPos);
    PSVECAdd(&mouthPos, &i_this->current.pos, &mouthPos); /* &mouthPos += current.pos */

    /* Pull the bomb into the Armos' mouth by 50 units per frame on each axis. */
    cLib_addCalc2(&swallowedActor->current.pos.x, mouthPos.x, 1.0f, 50.0f);
    if (alwaysMoveY || mouthPos.y - 10.0f < swallowedActor->current.pos.y) {
        cLib_addCalc2(&swallowedActor->current.pos.y, mouthPos.y, 1.0f, 50.0f);
    }
    cLib_addCalc2(&swallowedActor->current.pos.z, mouthPos.z, 1.0f, 50.0f);

    swallowedActor->gravity = 0.0f;
    swallowedActor->speedF = 0.0f;
    swallowedActor->speed.set(0.0f, 0.0f, 0.0f);
    swallowedActor->current.angle.x = 0;
    swallowedActor->current.angle.y = 0;
    swallowedActor->current.angle.z = 0;
    swallowedActor->shape_angle.x = 0;
    swallowedActor->shape_angle.y = 0;
    swallowedActor->shape_angle.z = 0;
    swallowedActor->current.angle.y = i_this->shape_angle.y;
    swallowedActor->shape_angle.y = i_this->shape_angle.y;

    if (swallowedActor != nullptr && am_GetName(swallowedActor) == fpcNm_BOMB_e) {
        s16 t = i_this->mCountDownTimers[1];
        if (t == 1) {
            swallowedActor->scale.set(0.0f, 0.0f, 0.0f);
            daBomb_setBombNoEff(swallowedActor);
        } else if (t > 1) {
            swallowedActor->scale.set(1.0f, 1.0f, 1.0f);
        }
        daBomb_setBombRestTime(swallowedActor, 100);
    } else if (swallowedActor != nullptr && am_GetName(swallowedActor) == fpcNm_Bomb2_e) {
        s16 t = i_this->mCountDownTimers[1];
        if (t == 1) {
            swallowedActor->scale.set(0.0f, 0.0f, 0.0f);
            daBomb2_remove_fuse_effect(swallowedActor);
        } else if (t > 1) {
            swallowedActor->scale.set(1.0f, 1.0f, 1.0f);
        }
        daBomb2_set_time(swallowedActor, 100);
    }
}

/* 02048FA4 */
BOOL bomb_nomi_check(am_class* i_this) {
    (void)dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0) (unused) */
    i_this->mStts.Move();

    s32 bck = i_this->mCurrBckIdx;
    if (bck != dRes_INDEX_AM_BCK_OPEN_e && bck != dRes_INDEX_AM_BCK_OPEN_LOOP_e &&
        bck != dRes_INDEX_AM_BCK_DAMAGE_e && bck != dRes_INDEX_AM_BCK_DAMAGE_LOOP_e) {
        return FALSE;
    }

    s16 angleToPlayer = fopAcM_searchPlayerAngleY(i_this);
    s16 angleDiff = (s16)cLib_distanceAngleS(i_this->shape_angle.y, angleToPlayer);
    if (angleDiff > 0x2000) {
        return FALSE;
    }

    if (i_this->mMouthSph.ChkCoHit()) {
        void* hitObj = GetCoHitObj(&i_this->mMouthSph);
        if (hitObj) {
            fopAc_ac_c* hitActor = cCcD_Obj_GetAc(hitObj);
            if (hitActor) {
                if (hitActor != nullptr && am_GetName(hitActor) == fpcNm_BOMB_e) {
                    if (!daBomb_getBombCheck_Flag(hitActor) &&
                        daBomb_getBombRestTime(hitActor) > 1) {
                        if (i_this->mMouthPos.y - (REG_F(8, 1) + 20.0f) < hitActor->current.pos.y) {
                            /* Swallow the bomb. */
                            daBomb_setBombCheck_Flag(hitActor);
                            daBomb_change_state(hitActor, 2);
                            i_this->mSwallowedActorPID = fopAcM_GetID(hitActor);
                            daBomb_setBombNoHit(hitActor);
                            bomb_move_set(i_this, 0);
                            i_this->mAction = ACTION_ITAI_MOVE;
                            i_this->mMode = 44;
                            return TRUE;
                        }
                    }
                } else if (hitActor != nullptr && am_GetName(hitActor) == fpcNm_Bomb2_e) {
                    if (!daBomb2_chk_eat(hitActor) && daBomb2_get_time(hitActor) > 1) {
                        if (i_this->mMouthPos.y - (REG_F(8, 1) + 20.0f) < hitActor->current.pos.y) {
                            /* Swallow the bomb. */
                            daBomb2_set_eat(hitActor);
                            i_this->mSwallowedActorPID = fopAcM_GetID(hitActor);
                            daBomb2_set_no_hit(hitActor);
                            bomb_move_set(i_this, 0);
                            i_this->mAction = ACTION_ITAI_MOVE;
                            i_this->mMode = 44;
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}

/* 0204C0BC */
void BG_check(am_class* i_this) {
    i_this->mAcchCir.SetWall(REG_F(12, 3) + 30.0f, REG_F(12, 4) + 150.0f);

    f32 o = i_this->mCorrectionOffsetY;
    i_this->current.pos.y = i_this->current.pos.y - o;
    i_this->old.pos.y = i_this->old.pos.y - o;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    o = i_this->mCorrectionOffsetY;
    i_this->current.pos.y = i_this->current.pos.y + o;
    i_this->old.pos.y = i_this->old.pos.y + o;
}

/* 02049250 */
// HD: destPos is a by-value cXyz (the caller's copy)
BOOL Line_check(am_class* i_this, cXyz* destPos) {
    dBgS_LinChk linChk;
    dBgS_LinChk_ct(&linChk, AM_LINCHK_VT, false);
    cXyz centerPos;
    centerPos.copy(i_this->current.pos);
    centerPos.y = i_this->current.pos.y + 100.0f;
    destPos->y = destPos->y + 100.0f;
    dBgS_LinChk_Set(&linChk, &centerPos, destPos, i_this);
    BOOL cross = cBgS_LineCross(dComIfG_Bgsp(), &linChk);
    /* ~dBgS_LinChk (inline): this TU's vtables, then cBgS_LinChk::~cBgS_LinChk */
    linChk.dBgS_PolyPassChk::__vtbl = 0x10006E94;
    linChk.dBgS_GrpPassChk::__vtbl = 0x10006E24;
    linChk.cBgS_PolyInfo::__vtbl = 0x10006E14;
    cBgS_LinChk_dt(&linChk, 0);
    /* if (!LineCross) return TRUE; return FALSE; (GHS: xori with the bool result) */
    return cross ^ 1;
}

/* 0204B428 */
static BOOL daAM_IsDelete(am_class*) { return TRUE; }

/* 0204B430 */
static BOOL daAM_Delete(am_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Am");

    /* dPa_smokeEcallBack::remove(): virtual (vtable slot 0x44) */
    for (int i = 0; i < 4; i++) {
        dPa_smokeEcallBack* cb = &i_this->mSmokeCbs[i];
        cb->remove();
    }
    {
        dPa_smokeEcallBack* cb = &i_this->mSmokeCbs[2];
        cb->remove();
    }
    if (i_this->m033C) {
        JPA_becomeInvalidEmitter(i_this->m033C);
        i_this->m033C = nullptr;
    }
    if (i_this->m0340) {
        JPA_becomeInvalidEmitter(i_this->m0340);
        i_this->m0340 = nullptr;
    }

    return TRUE;
}

/* 0204B500 */
static BOOL useHeapInit(fopAc_ac_c* i_this) {
    am_class* a_this = (am_class*)i_this;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Am", dRes_INDEX_AM_BDL_AM_e);
    J3DAnmTransform* anm =
        (J3DAnmTransform*)dComIfG_getObjectRes("Am", dRes_INDEX_AM_BCK_SLEEP_LOOP_e);
    a_this->mpMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 1, nullptr, 0x00000000, 0x11020203);
    if (!a_this->mpMorf || !a_this->mpMorf->getModel()) {
        return FALSE;
    }

    ((J3DModel*)a_this->mpMorf->getModel())->mUserArea = gabi::ea(a_this);
    for (u16 i = 0; i < J3DModelData_getJointNum(J3DModel_getModelData(a_this->mpMorf->getModel()));
         i++) {
        setJointCallBack(J3DModel_getModelData(a_this->mpMorf->getModel()), i,
                         0x02048714 /* nodeCallBack */);
    }

    // One CYL2 hit on the pupil.
    a_this->mEyeJntHit = JntHit_create(a_this->mpMorf->getModel(), search_data, 1);
    if (a_this->mEyeJntHit) {
        a_this->jntHit = gabi::ea(a_this->mEyeJntHit.get()); /* fopAcM_SetJntHit */
    } else {
        return FALSE;
    }

    return TRUE;
}

/* 0204B8E0 */
static cPhs_State daAM_Create(fopAc_ac_c* i_this) {
    am_class* a_this = (am_class*)i_this;
    /* fopAcM_ct(i_this, am_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            am_class_ct(a_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&a_this->mPhase, "Am");
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(a_this, 0x0204B500 /* useHeapInit */, 0x1C80)) {
            return cPhs_ERROR_e;
        }

        a_this->stealItemLeft = 3;

        a_this->mSmokeCbs[0].m11 = 0;
        a_this->mSmokeCbs[1].m11 = 0;
        a_this->mSmokeCbs[3].m11 = 0;

        u32 prm = fopAcM_GetParam(a_this);
        a_this->mType = (prm >> 0x00) & 0xFF;
        a_this->mPrmAreaRadius = (prm >> 0x08) & 0xFF;
        a_this->mStartsInactive = (prm >> 0x10) & 0xFF;
        a_this->mSwitch = (prm >> 0x18) & 0xFF;

        if (a_this->mType == 0xFF) {
            a_this->mType = 0;
        }
        if (a_this->mStartsInactive == 0xFF) {
            a_this->mStartsInactive = 0;
        }
        if (REG_S(8, 9) != 0) {
            a_this->mType = 1;
        }
        if (a_this->mPrmAreaRadius == 0xFF || a_this->mPrmAreaRadius == 0) {
            a_this->mAreaRadius = 400.0f;
        } else {
            a_this->mAreaRadius = (f32)a_this->mPrmAreaRadius * 100.0f;
        }
        am_copyFloat(a_this->mSpawnPosY, a_this->current.pos.y);
        a_this->mSwallowedActorPID = fpcM_ERROR_PROCESS_ID_e;

        if (a_this->mStartsInactive == 0 && a_this->mSwitch != 0xFF &&
            dComIfGs_isSwitch(a_this->mSwitch, dComIfGp_roomControl_getStayNo())) {
            /* starts active; the switch keeps it from spawning again */
            return cPhs_ERROR_e;
        }
        if (a_this->mStartsInactive == 1 && a_this->mSwitch != 0xFF &&
            !dComIfGs_isSwitch(a_this->mSwitch, dComIfGp_roomControl_getStayNo())) {
            /* initially inactive until the switch is set */
            a_this->actor_status &= ~(u32)fopAcStts_SHOWMAP_e;
        }

        a_this->itemTableIdx = am_charTbl()->GetNameIndex("amos", 0);
        a_this->max_health = 10;
        a_this->health = 10;

        fopAcM_SetMtx(a_this, J3DModel_getBaseTRMtx(a_this->mpMorf->getModel()));
        fopAcM_setCullSizeBox(a_this, -100.0f, -10.0f, -80.0f, 120.0f, 400.0f, 100.0f);

        attention_flags(a_this) = 0;

        a_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &a_this->mAcchCir,
                          &a_this->speed);
        a_this->mStts.Init(254, 1, a_this);

        a_this->gravity = -10.0f;

        a_this->mEnemyIce.mpActor = a_this;
        a_this->mEnemyIce.mWallRadius = 80.0f;
        a_this->mEnemyIce.mCylHeight = 300.0f;

        a_this->mEyeSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&eye_co_sph_src));
        a_this->mEyeSph.SetStts(&a_this->mStts);
        a_this->mMouthSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&mouth_co_sph_src));
        a_this->mMouthSph.SetStts(&a_this->mStts);
        a_this->mBodyCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&body_co_cyl_src));
        a_this->mBodyCyl.SetStts(&a_this->mStts);
        a_this->mNeedleCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&sword_co_cyl_src));
        a_this->mNeedleCyl.SetStts(&a_this->mStts);

        needle_offAt(&a_this->mNeedleCyl);
        needle_offAt(&a_this->mNeedleCyl);

        a_this->mTargetAngleY = a_this->current.angle.y;
        a_this->mSpawnPos.copy(a_this->current.pos);
        a_this->mSpawnRotY = a_this->current.angle.y;

        draw_SUB(a_this);
    }

    return phase_state;
}

// HD: compiler-generated and inline copies of this translation unit.

/* 0204B698 */
// HD: the dPa_smokeEcallBack array constructor (dPa_smokeEcallBack(1))
static void* smokeEcallBack_ct(void* p) { return am_constructSmoke(p, (u8)1); }

/* 0204B6A0 */
// HD: enemyfire::enemyfire (inline constructor, this TU's copy)
static enemyfire* enemyfire_ct(enemyfire* self) {
    if (self == nullptr) {
        self = (enemyfire*)operator_new(sizeof(enemyfire));
        if (self == nullptr)
            return self;
    }
    if (gabi::ea(&self->mDirection) == 0)
        operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    am_constructSphere(&self->mSph);
    self->m228 = 1.0f;
    return self;
}

/* 0204B72C */
// HD: am_class::am_class (HD: allocates when this == NULL)
static am_class* am_class_ct(am_class* self) {
    if (self == nullptr) {
        self = (am_class*)operator_new(sizeof(am_class));
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = AM_VTBL;
    __construct_array(self->mSmokeCbs, 4, 0x20, 0x0204B698);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, gc_tu.objAcch);
    dCcD_Stts_ct(&self->mStts);
    am_constructSphere(&self->mEyeSph);
    am_constructSphere(&self->mMouthSph);
    dCcD_Cyl_ct(&self->mBodyCyl, AM_AAB_VTBL);
    dCcD_Cyl_ct(&self->mNeedleCyl, AM_AAB_VTBL);
    /* enemyice */
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, AM_AAB_VTBL);
    dBgS_AcchCir_ct(&self->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&self->mEnemyIce.mBgAcch, gc_tu.objAcch);
    enemyfire_ct(&self->mEnemyFire);
    return self;
}

/* 0204BCB8 */
// HD: static initialisation of the translation unit (header statics only)
static void __sinit_d_a_am_cpp() { __sinit_header_statics(); }

/* 0204BD4C */
// HD: fopAcM_seStart (out-of-line copy in this TU)
void am_seStart(fopAc_ac_c* a, u32 id, u32 param) { fopAcM_seStart(a, id, param); }

/* 0204BDB8 */
// HD: deleting SafeString destructor of this translation unit.
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0204C15C */
// HD: JPABaseEmitter status |= bit (out-of-line copy; used by daAM_Execute's *
// becomeInvalidEmitter)
void JPABaseEmitter_onStatus(JPABaseEmitter* e, u32 bit) {
    static_cast<AmEmitterView*>(e)->mStatus |= bit;
}

/* 0204C16C */
// HD: dPa_smokeEcallBack destructor of the mSmokeCbs array (trivial, deleting)
static void smokeEcallBack_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* ~dBgS_ObjAcch (inline): this TU's vtables, then dBgS_Acch::~dBgS_Acch and ~cM3dGCir of the circle
 */

/* 0204C180 */
// HD: am_class deleting destructor (HD virtual destructor)
static void am_class_dt(am_class* self, s32 flags) {
    if (self == nullptr)
        return;
    am_destroySphere(&self->mEnemyFire.mSph);
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    __dt(&self->mEnemyIce.mBgAcch);
    __dt(&self->mEnemyIce.mBgAcchCir);
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    dCcD_Cyl_dt(&self->mNeedleCyl, 2);
    dCcD_Cyl_dt(&self->mBodyCyl, 2);
    am_destroySphere(&self->mMouthSph);
    am_destroySphere(&self->mEyeSph);
    dCcD_Stts_dt(&self->mStts, 2);
    __dt(&self->mAcch);
    __dt(&self->mAcchCir);
    __destroy_arr(self->mSmokeCbs, 4, 0x20, 0x0204C16C, 0, 0);
    __dt(static_cast<fopAc_ac_c*>(self));
    if (flags & 1)
        operator_delete(self);
}

/* 0204C2C0 */
// HD: empty SafeString virtual of this translation unit.
static void SafeString_assureTermination(void*) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 025A5B18 void* am_constructSmoke(void*, u8)
str 10006ED0 "Am" in 02048BAC
str 10006F40 "Am" in 0204B430
str 10006F43 "Am" in 0204B500
str 10006F5C "Am" in 0204B8E0
str 10006F60 "amos"
data search_data 1018F8B4
data cyl2_eye_offset 1018F8C0
data eye_co_sph_src 1018F8D8
data mouth_co_sph_src 1018F918
data body_co_cyl_src 1018F958
data sword_co_cyl_src 1018F99C
*/
// clang-format on
