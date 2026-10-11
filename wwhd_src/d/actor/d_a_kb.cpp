/**
 * d_a_kb.cpp (WWHD)
 * NPC - Pig
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_kb.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 * daKb_Execute (with normal_move, carry_move, swim_move and pl_attack_hit_check inlined) is in
 * d_a_kb_exec.cpp, attack_move and esa_demo_move in d_a_kb_move.cpp.
 */
#include "d/actor/d_a_kb.h"

static void followEcallBack_dt(void *p, s32 flags);
static kb_class *kb_class_ct(kb_class *self);
static u32 followEcallBack_ct(dPa_followEcallBack *p);

const gc_tu_data gc_tu = {.safeString = SAFESTRING_VTBL};
static KbLightObjectHD l_lightDefaults;
static KbStatics l_kbStatics;
static u16 kb_btp_idx[7] = {41, 41, 42, 39, 39, 40, 41};
static u16 kb_bdl_idx[7] = {36, 34, 35, 33, 31, 32, 36};
static u32 gold_rate_dt[3] = {1, 2, 3};
static u32 item_rate_dt[3] = {16, 11, 9};
static dCcD_SrcSph co_sph_src;
static KbJ3DSysState l_j3dSys;
static Mtx34 l_currentMtx;
static u32 l_debugSwitch;
static s16 l_reg8s8;
static KbSineEntry l_sincosTable;
static u32 l_staticListHead;
static u32 l_staticConstructors;

/* 0218EB64 */
void anm_init(kb_class *i_this, int bck, f32 morf, u8 loop, f32 speed, int basIndex) {
    i_this->m50C = bck;
    if (basIndex >= 0) {
        J3DAnmTransform *anm = (J3DAnmTransform *)dComIfG_getObjectRes("Kb", bck, SAFESTRING_VTBL);
        void *bas = dComIfG_getObjectRes("Kb", basIndex, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loop, morf, speed, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform *anm = (J3DAnmTransform *)dComIfG_getObjectRes("Kb", bck, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loop, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 0219463C */
BOOL kb_eye_tex_anm(kb_class *i_this) {
    return mDoExt_baseAnm_play(&i_this->mBtp);
}

/* 0218EC90 */
void *esa_search_sub(void *param_1, void *param_2) {
    dBgS_LinChk linChk;
    dBgS_LinChk_ct(&linChk, LINCHK_VT, false);

    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == PROC_ESA) {
        fopAc_ac_c *pEsa = (fopAc_ac_c *)param_1;
        fopAc_ac_c *pActor = (fopAc_ac_c *)param_2;
        if (esa_field_0x298(pEsa) == 0 && esa_mState(pEsa) == 1 &&
            std::fabs(pActor->current.pos.y - pEsa->current.pos.y) < 40.0f) {
            if (fopAcM_searchActorDistanceXZ(pActor, pEsa) < 400.0f && [&] {
                    s16 ang = fopAcM_searchActorAngleY(pActor, pEsa);
                    return kbGuest_cLib_distanceAngleS_i(pActor->current.angle.y, ang);
                }() < 0x55F0) {
                cXyz temp;
                temp.x = (f32)pEsa->current.pos.x;
                temp.y = pEsa->current.pos.y + 40.0f;
                temp.z = (f32)pEsa->current.pos.z;
                cXyz temp2;
                temp2.x = (f32)pActor->current.pos.x;
                temp2.y = pActor->current.pos.y + 40.0f;
                temp2.z = (f32)pActor->current.pos.z;
                dBgS_LinChk_Set(&linChk, &temp2, &temp, pActor);
                if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                    dBgS_LinChk_dt(&linChk);
                    return param_1;
                }
            }
        }
    }
    dBgS_LinChk_dt(&linChk);
    return nullptr;
}

/* 0218EEB4 */
void *item_tag_search(void *param_1, void *param_2) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == PROC_TAG_KB_ITEM) {
        fopAc_ac_c *pItem = (fopAc_ac_c *)param_1;
        fopAc_ac_c *pActor = (fopAc_ac_c *)param_2;
        if (std::fabs(pActor->current.pos.y - pItem->current.pos.y) < 40.0f &&
            fopAcM_searchActorDistanceXZ(pActor, pItem) < 400.0f) {
            return param_1;
        }
    }
    return nullptr;
}

/* 0218EF4C */
BOOL carry_check(kb_class *i_this) {
    attn_flags(i_this) |= fopAc_Attn_ACTION_CARRY_e;
    if (fopAcM_checkCarryNow(i_this)) {
        attn_flags(i_this) &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
        attn_flags(i_this) &= ~(u32)fopAc_Attn_LOCKON_MISC_e;
        i_this->mSph.OnTgSPrmBit(1); /* OnTgSetBit */
        i_this->m41E = 1;
        i_this->m420 = 10;
        i_this->m4CC = 0.0f;
        i_this->m440 = 0;
        dPa_rippleEcallBack_end((dPa_rippleEcallBack *)&i_this->m540);
        i_this->m404 = 0;
        i_this->gravity = 0.0f;
        i_this->speed.y = 0.0f;

        fopAc_ac_c *pActor = fopAcM_SearchByID(i_this->m4D8);
        if (i_this->m4D8 != fpcM_ERROR_PROCESS_ID_e && pActor != nullptr) {
            esa_field_0x298(pActor) = 0;
        }
        i_this->m4D8 = fpcM_ERROR_PROCESS_ID_e;

        i_this->shape_angle.z = 0;
        i_this->speedF = 0.0f;
        i_this->speed.set(0.0f, 0.0f, 0.0f);
        i_this->m4BC = (f32)i_this->current.pos.y;
        i_this->m436 = 0;
        i_this->m438 = 0;
        i_this->m44A = 0;

        anm_init(i_this, dRes_INDEX_KB_BCK_JITA2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                 dRes_INDEX_KB_BAS_JITA2_e);
        kb_catch_se(i_this);
        return TRUE;
    }
    return FALSE;
}

/* 02194654 */
void speed_pos_set(kb_class *i_this) {
    fopAc_ac_c *pPlayer = dComIfGp_getPlayer(0);
    if (daPy_getGrabMissActor(pPlayer) == i_this) {
        i_this->m40B = 1;
        i_this->mpMorf->setPlaySpeed(0.0f);
        i_this->shape_angle.z = 0;
        i_this->shape_angle.x = 0;
        i_this->current.angle.x = 0;
        i_this->current.angle.z = 0;
    } else {
        if (i_this->m40B) {
            i_this->mpMorf->setPlaySpeed(1.0f);
            i_this->m40B = 0;
        }
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        cXyz temp1;
        cXyz temp2;
        temp1.x = 0.0f;
        temp1.y = 0.0f;
        temp1.z = (f32)i_this->speedF;
        MtxPosition(&temp1, &temp2);
        i_this->speed.x = (f32)temp2.x;
        f32 sy = i_this->speed.y + i_this->gravity;
        i_this->speed.z = (f32)temp2.z;
        if (sy < -20.0f) {
            sy = -20.0f;
        }
        i_this->speed.y = sy;
        // HD: no mSph.ChkCoSet() / GetCCMoveP() path
        if (!fopAcM_checkCarryNow(i_this)) {
            fopAcM_posMove(i_this, nullptr);
        }
    }
}

/* 0218F150 */
void hamon_set(kb_class *i_this, f32 param_2) {
    cXyz scale;
    f32 s = i_this->m5D0;
    scale.x = s;
    scale.y = s;
    scale.z = s;
    if (i_this->m540.mpEmitter.get() == nullptr) {
        /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &current.pos, NULL, &&scale, 0xFF, &m540)
         */
        dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->current.pos, nullptr, &scale,
                        0xFF, (dPa_levelEcallBack *)&i_this->m540, -1, nullptr, nullptr, nullptr);
        if (i_this->m540.mpEmitter.get() == nullptr) {
            return;
        }
    }
    i_this->m540.mRate = param_2;
}

/* 0218F1FC */
void sibuki_set(kb_class *i_this) {
    f32 scaleXZ = 0.4f;
    f32 scaleY = 0.75f;
    i_this->m440 = 1;
    cXyz pos;
    pos.copy(i_this->current.pos);
    pos.y = kb_wtr_height(i_this);
    i_this->m4C4 = 30.0f;
    if (i_this->m4D4 != 0.0f) {
        scaleXZ = REG_F(8, 4) + 0.8f;
    }
    fopKyM_createWpillar(&pos, scaleXZ, scaleY, 0);
    kb_se_start(i_this, JA_SE_OBJ_FALL_WATER_S);
    kb_catch_se(i_this);
    i_this->m5D0 = gabi::fmadds(i_this->m4D4, 0.5f, 1.0f);
    hamon_set(i_this, 1.0f);
}

/* 0218F380 */
BOOL swim_mode_change_check(kb_class *i_this) {
    f32 temp = gabi::fmadds(i_this->m4D4, 20.0f, 20.0f);
    if (i_this->mAcch.ChkWaterHit() && kb_wtr_height(i_this) > i_this->current.pos.y &&
        std::fabs(kb_wtr_height(i_this) - i_this->mAcch.GetGroundH()) > temp) {
        if (i_this->m440 == 0) {
            sibuki_set(i_this);
        } else {
            i_this->m5D0 = 1.0f;
            if (i_this->m540.mpEmitter.get() != nullptr) {
                i_this->m540.mRate = 1.0f;
            }
        }
        dPa_smokeEcallBack_end((dPa_smokeEcallBack *)&i_this->m554);
        i_this->m41E = 2;
        i_this->m420 = 0x14;
        return TRUE;
    }
    if (i_this->m404) {
        if (i_this->m440 == 0) {
            i_this->m440 = 1;
            i_this->m4C4 = 30.0f;
            i_this->m5D0 = gabi::fmadds(i_this->m4D4, 0.5f, 0.5f);
            hamon_set(i_this, 0.0f);
        }
    } else if (i_this->m440) {
        i_this->m440 = 0;
        dPa_rippleEcallBack_end((dPa_rippleEcallBack *)&i_this->m540);
    }
    return FALSE;
}

/* 0218F4EC */
void he_set(kb_class *i_this) {
    fopAc_ac_c *pPlayer = dComIfGp_getPlayer(0);
    i_this->m5D4[0].copy(i_this->m4A4);
    i_this->m5EC[0].x = (s16)pPlayer->shape_angle.x;
    i_this->m5EC[0].y = (s16)pPlayer->shape_angle.y;
    i_this->m5EC[0].z = (s16)pPlayer->shape_angle.z;
    if (i_this->m594.mpEmitter.get() == nullptr) {
        i_this->m5D4[0].copy(i_this->m4A4);
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        dPa_control_set(dComIfGp_getParticle(), 0, 0x8286 /* ID_AK_SN_PIGGAS00 */, &i_this->m5D4[0],
                        nullptr, nullptr, 0xFF, (dPa_levelEcallBack *)&i_this->m594, roomNo,
                        nullptr, nullptr, nullptr);
    }
    JPABaseEmitter *emitter = i_this->m594.mpEmitter;
    if (emitter != nullptr) {
        /* setGlobalRTMatrix(getAnmMtx(PG_JNT_J_PG_TAIL_e)) */
        Mtx34 *mtx = getAnmMtx(i_this->mpMorf->getModel(), PG_JNT_J_PG_TAIL_e);
        kbGuest_JPASetRMtxTVecfromMtx(mtx,
                                      gabi::ea(&kbEmitter(gabi::ea(emitter))->mGlobalRotation[0]),
                                      gabi::ea(&kbEmitter(gabi::ea(emitter))->mGlobalTranslation));
    }
}

/* 0218F600 */
void smoke_set(kb_class *i_this) {
    f32 scale = i_this->m4D4 + 1.0f;
    if (i_this->m554.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        /* dComIfGp_particle_setToon(ID_AK_JT_ELEMENTSMOKE00, &m5D4[0], &m5EC[0], NULL, 0xB9, &m554,
         * roomNo) */
        dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &i_this->m5D4[0], &i_this->m5EC[0],
                        nullptr, 0xB9, (dPa_levelEcallBack *)&i_this->m554, roomNo, nullptr,
                        nullptr, nullptr);
        if (i_this->m554.mpEmitter.get() == nullptr) {
            return;
        }
    }
    kbEmitter(emitter_ea(i_this->m554))->mRate = 1.0f;   /* setRate */
    kbEmitter(emitter_ea(i_this->m554))->mSpread = 1.0f; /* setSpread */
    u32 e = emitter_ea(i_this->m554);
    set_global_scale(e, scale, scale, scale);
}

/* 0218F71C */
void smoke_set2(kb_class *i_this) {
    f32 scale = i_this->m4D4 + 1.0f;
    if (i_this->m554.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        dPa_control_set(dComIfGp_getParticle(), 2, 0x2027, &i_this->m5D4[0], &i_this->shape_angle,
                        nullptr, 0xB9, (dPa_levelEcallBack *)&i_this->m554, roomNo, nullptr,
                        nullptr, nullptr);
        if (i_this->m554.mpEmitter.get() == nullptr) {
            return;
        }
    }
    kbEmitter(emitter_ea(i_this->m554))->mMaxFrame = 25;         /* setMaxFrame */
    kbEmitter(emitter_ea(i_this->m554))->mInitialVelOmni = 0.0f; /* setAwayFromCenterSpeed */
    kbEmitter(emitter_ea(i_this->m554))->mInitialVelAxis = 5.0f; /* setAwayFromAxisSpeed */
    kbEmitter(emitter_ea(i_this->m554))->mRate = 3.0f;           /* setRate */
    kbEmitter(emitter_ea(i_this->m554))->mInitialVelDir = 2.0f;  /* setDirectionalSpeed */
    u32 e = emitter_ea(i_this->m554);
    set_global_scale(e, scale, scale, scale);
    kbEmitter(emitter_ea(i_this->m554))->mLifeTime = 25; /* setLifeTime */
}

/* 0218F868 */
void smoke_set3(kb_class *i_this) {
    f32 s;
    if (i_this->mShapeType >= 8) {
        s = 3.0f;
    } else {
        s = 1.0f;
    }
    if (i_this->m574.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        dPa_control_set(dComIfGp_getParticle(), 2, 0xA307 /* ID_AK_ST_PIGDIGSMOKE00 */,
                        &i_this->m5D4[1], &i_this->shape_angle, nullptr, 0xA0,
                        (dPa_levelEcallBack *)&i_this->m574, roomNo, nullptr, nullptr, nullptr);
    }
    i_this->m574.mWindOff = 1; /* onWindOff */
    if (i_this->m574.mpEmitter.get() != nullptr) {
        set_global_scale(emitter_ea(i_this->m574), s, s, s);
    }
}

/* 0218F98C */
static BOOL nodeCallBack(J3DNode *i_node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint *joint = J3DNode_toJoint(i_node);
        J3DModel *pModel = j3dSys_getModel();
        kb_class *i_kb = gabi::at<kb_class>(pModel->mUserArea);
        s32 jntNo = gabi::at<KbJointDataHD>(gabi::ea(joint))->mJntNo;
        if (i_kb != nullptr && jntNo == PG_JNT_J_PG_TAIL_e) {
            PSMTXCopy(getAnmMtx((J3DModel *)pModel, jntNo), calc_mtx());
            cXyz temp;
            temp.x = 0.0f;
            temp.y = 0.0f;
            temp.z = 0.0f;
            MtxPosition(&temp, &i_kb->m4A4);
            mtx_copy(getAnmMtx((J3DModel *)pModel, jntNo), calc_mtx()); /* setAnmMtx */
            PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
        }
    }
    return TRUE;
}

/* 0218FABC */
void draw_SUB(kb_class *i_this) {
    J3DModel *pModel = i_this->mpMorf->getModel();
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
    cMtx_XrotM(calc_mtx(), i_this->m4F4.x);
    cMtx_ZrotM(calc_mtx(), i_this->m4F4.z);
    MtxScale(i_this->m4E8.x, i_this->m4E8.y, i_this->m4E8.x, true);
    if (i_this->m41E != 4) {
        cLib_addCalc2(&i_this->m4C0, i_this->m4C4, 1.0f, 5.0f);
    }
    MtxTrans(0.0f, i_this->m4C0, 0.0f, true);
    cMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    cMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
    MtxTrans(0.0f, -30.0f, 0.0f, true);
    J3DModel_setBaseTRMtx(pModel, calc_mtx());
}

/* dKy_tevstr_c::operator= (HD, inline): the members are copied one by one (floats through FPRs);
 * three 0x40-byte blocks are not copied */
// HD: floats pass through FPRs; colour/ambient bytes retain byte load/store ordering.
static inline void kbLightCopy(KbLightObjectHD *dst, KbLightObjectHD *src) {
    dst->position.x = (f32)src->position.x;
    dst->position.y = (f32)src->position.y;
    dst->position.z = (f32)src->position.z;
    dst->direction.x = (f32)src->direction.x;
    dst->direction.y = (f32)src->direction.y;
    dst->direction.z = (f32)src->direction.z;
    for (int i = 0; i < 4; ++i)
        dst->color[i] = (u8)src->color[i];
    for (int i = 0; i < 8; ++i)
        kbAmbientBytes(dst)->bytes[i] = (u8)kbAmbientBytes(src)->bytes[i];
    for (int i = 0; i < 2; ++i)
        dst->intensity[i] = (f32)src->intensity[i];
    for (int i = 0; i < 3; ++i)
        dst->cosAtten[i] = (f32)src->cosAtten[i];
    for (int i = 0; i < 3; ++i)
        dst->distanceAtten[i] = (f32)src->distanceAtten[i];
}
// HD: position/colour bytes are copied before the overlapping fog floats are loaded through FPRs.
static inline void tevstr_copy(dKy_tevstr_c *dst, dKy_tevstr_c *src) {
    kbLightCopy(&kbLighting(dst)->light, &kbLighting(src)->light);
    for (int i = 0; i < 0x30; ++i)
        kbPositionColorBytes(dst)->bytes[i] = (u8)kbPositionColorBytes(src)->bytes[i];
    dst->mFogStartZ = (f32)src->mFogStartZ;
    dst->mFogEndZ = (f32)src->mFogEndZ;
    dst->mColpatBlend = (f32)src->mColpatBlend;
    for (int i = 0; i < 9; ++i)
        kbControlBytes(dst)->bytes[i] = (u8)kbControlBytes(src)->bytes[i];
    kbLightCopy(&kbLighting(dst)->materialLight, &kbLighting(src)->materialLight);
    kbLightCopy(&kbLighting(dst)->secondMaterialLight, &kbLighting(src)->secondMaterialLight);
}

/* 0218FC3C */
static BOOL daKb_Draw(kb_class *i_this) {
    J3DModel *pModel = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    // HD: the eye texture animation is an mDoExt_btpAnm
    mDoExt_btpAnm_entry((mDoExt_btpAnm *)&i_this->mBtp, J3DModel_getModelData(pModel),
                        (s16)gabi::ftoi(i_this->mBtp.mFrameCtrl.mFrame));
    tevstr_copy(&i_this->mTevStr, &i_this->tevStr);

    f32 t = i_this->m4B0;
    // HD: the two material colours use signed-short and byte components respectively.
    s16 r = (s16)i_this->tevStr.mColorC0.r;
    i_this->mTevStr.mColorC0.r = (s16)gabi::ftoi(gabi::fmadds((f32)(0x78 - r), t, (f32)r));
    s16 g = (s16)i_this->tevStr.mColorC0.g;
    i_this->mTevStr.mColorC0.g = (s16)gabi::ftoi(gabi::fmadds((f32)(-g), t, (f32)g));
    s16 bl = (s16)i_this->tevStr.mColorC0.b;
    i_this->mTevStr.mColorC0.b = (s16)gabi::ftoi(gabi::fmadds((f32)(-bl), t, (f32)bl));
    u8 kr = (u8)i_this->tevStr.mColorK0.r;
    i_this->mTevStr.mColorK0.r = (u8)gabi::ftoi(gabi::fmadds((f32)(0x78 - kr), t, (f32)kr));
    u8 kg = (u8)i_this->tevStr.mColorK0.g;
    i_this->mTevStr.mColorK0.g = (u8)gabi::ftoi(gabi::fmadds((f32)(-kg), t, (f32)kg));
    u8 kb = (u8)i_this->tevStr.mColorK0.b;
    i_this->mTevStr.mColorK0.b = (u8)gabi::ftoi(gabi::fmadds((f32)(-kb), t, (f32)kb));
    setLightTevColorType(dKy_getEnvlight(), pModel, &i_this->mTevStr);
    i_this->mpMorf->updateDL();

    // HD: no blob shadow (the play object is still fetched)
    dComIfGp_get();
    if (i_this->m4D4 == 2.0f) {
        dSnap_RegistFig(DSNAP_TYPE_KB, i_this, i_this->m4D4 + 0.6f, i_this->m4D4 + -0.1f, 1.0f);
    } else {
        dSnap_RegistFig(DSNAP_TYPE_KB, i_this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}

/* 021901D4 */
s16 way_check(kb_class *i_this, s16 param_1, u8 param_2) {
    dBgS_LinChk linChk;
    dBgS_LinChk_ct(&linChk, LINCHK_VT, false);
    s16 angle = param_1;
    cXyz temp3;
    temp3.x = (f32)i_this->current.pos.x;
    temp3.y = i_this->current.pos.y + 50.0f;
    temp3.z = (f32)i_this->current.pos.z;
    s16 temp = 0x2000;
    if (cM_rnd() < 0.5f) {
        temp = -0x2000;
    }
    cXyz temp1;
    temp1.set(0.0f, 50.0f, 300.0f);
    if (param_2) {
        temp3.x = (f32)i_this->current.pos.x;
        temp3.y = i_this->current.pos.y + 100.0f;
        temp3.z = (f32)i_this->current.pos.z;
        temp1.x = 0.0f;
        temp1.y = 100.0f;
        temp1.z = 100.0f;
    }
    cXyz temp2;
    for (int i = 0; i < 8; i++) {
        cMtx_YrotS(calc_mtx(), angle);
        MtxPosition(&temp1, &temp2);
        PSVECAdd(&temp2, &i_this->current.pos, &temp2);
        dBgS_LinChk_Set(&linChk, &temp3, &temp2, i_this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
            dBgS_LinChk_dt(&linChk);
            return angle;
        }
        angle += temp;
    }
    dBgS_LinChk_dt(&linChk);
    return param_1;
}

/* 02190400 */
void target_set(kb_class *i_this, u8 param_1) {
    dComIfGp_get(); /* daPy_getPlayerActorClass() (unused) */
    /* HARNESS WORKAROUND: GHS passes way_check's r3 to YrotS unchanged (way_check returns a
     * sign-extended s16); the mock's r3 may have other upper bits, so the value is kept raw */
    u32 temp = 0;
    switch (param_1) {
    case 0:
        temp = (u32)(s32)i_this->shape_angle.y;
        if (i_this->m426[3] == 0) {
            temp = kbGuest_way_check_raw(i_this, (s16)gabi::ftoi(cM_rndFX(32768.0f)), 0);
        }
        break;
    case 1: {
        s32 a = fopAcM_searchPlayerAngleY(i_this) + 0x8000;
        temp = kbGuest_way_check_raw(i_this, (s16)(a + (s16)gabi::ftoi(cM_rndFX(10000.0f))), 0);
        break;
    }
    case 2: {
        f32 dx = i_this->m450.x - i_this->current.pos.x;
        f32 dz = i_this->m450.z - i_this->current.pos.z;
        temp = kbGuest_way_check_raw(i_this, cM_atan2s(dx, dz), 1);
        i_this->m422 = (s16)temp;
        break;
    }
    case 3:
        if (i_this->m4D8 != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c *pActor = fopAcM_SearchByID(i_this->m4D8);
            if (pActor != nullptr) {
                dBgS_LinChk linChk;
                dBgS_LinChk_ct(&linChk, LINCHK_VT, false);
                cXyz temp3;
                temp3.x = (f32)pActor->current.pos.x;
                temp3.y = pActor->current.pos.y + 10.0f;
                temp3.z = (f32)pActor->current.pos.z;
                cXyz temp4;
                temp4.x = (f32)i_this->current.pos.x;
                temp4.y = i_this->current.pos.y + 10.0f;
                temp4.z = (f32)i_this->current.pos.z;
                dBgS_LinChk_Set(&linChk, &temp4, &temp3, i_this);
                if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                    i_this->m45C.copy(pActor->current.pos);
                } else {
                    i_this->m4D8 = fpcM_ERROR_PROCESS_ID_e;
                }
                dBgS_LinChk_dt(&linChk);
            }
        }
        break;
    }
    if (param_1 < 3) {
        kbGuest_cMtx_YrotS(calc_mtx(), temp); /* cMtx_YrotS */
        cXyz temp2;
        temp2.set(0.0f, 0.0f, 1000.0f);
        MtxPosition(&temp2, &i_this->m45C);
        PSVECAdd(&i_this->m45C, &i_this->current.pos, &i_this->m45C);
    }
}

/* 02190708 */
BOOL esa_demo_check(kb_class *i_this) {
    if (i_this->m426[4] != 0) {
        return FALSE;
    }
    if (fopAcM_searchPlayerDistanceXZ(i_this) > 2000.0f) {
        return FALSE;
    }
    if (i_this->mAcch.ChkWallHit()) {
        return FALSE;
    }
    /* search_get_esa (inline) */
    fopAc_ac_c *pEsa =
        (fopAc_ac_c *)fpcM_Search(gabi::guest_fn((const void *)&esa_search_sub), i_this);
    if (pEsa != nullptr) {
        i_this->m4D8 = fopAcM_GetID(pEsa);
        i_this->field_0x498.copy(pEsa->current.pos);
        if (!l_kbStatics.DEMO_START) {
            l_kbStatics.DEMO_START = 1;
            i_this->m406 = 1;
            i_this->m41E = 4;
            i_this->m420 = 0x28;
            return TRUE;
        }
    }
    return FALSE;
}

/* 021907F4 */
void money_drop(kb_class *i_this) {

    int i;
    csXyz temp;
    temp.x = (s16)i_this->current.angle.x;
    temp.y = (s16)i_this->current.angle.y;
    temp.z = (s16)i_this->current.angle.z;
    cXyz temp3;
    cXyz temp4;
    u8 count = 0;
    i_this->m407 = 0;

    u32 temp2;
    for (i = 0; i < 2; i++) {
        f32 rnd = cM_rnd();
        if (REG_S(8, 4) == 0) {
            if (rnd < 0.9f) {
                rnd *= 10.0f;
                temp2 = (u32)gold_rate_dt[gabi::ftoi(rnd * 0.3f)];
                i_this->m407 = 1;
                Mtx34 *m = calc_mtx();
                cMtx_YrotS(m, (s16)gabi::ftoi(cM_rndFX(32768.0f)));
                temp3.set(0.0f, 0.0f, 20.0f);
                MtxPosition(&temp3, &temp4);
                PSVECAdd(&temp4, &i_this->current.pos, &temp4);
                f32 speedF = cM_rndF(5.0f);
                f32 speedY = cM_rndF(10.0f) + 20.0f;
                fopAc_ac_c *pItem = kbGuest_fopAcM_fastCreateItem(
                    &temp4, temp2, -1, &temp, &i_this->scale, speedF, speedY, -3.0f, -1, 0);
                if (pItem != nullptr) {
                    pItem->actor_status |= 0x4000; /* fopAcStts_UNK4000_e */
                    kb_se_start(i_this, JA_SE_OBJ_LUPY_OUT);
                    count += 1;
                }
            }
        }
    }

    for (i = 0; i < 2; i++) {
        f32 rnd = cM_rnd();
        if (rnd < 0.9f) {
            rnd *= 10.0f;
            temp2 = (u32)item_rate_dt[gabi::ftoi(rnd * 0.3f)];
            i_this->m407 = 1;
            Mtx34 *m = calc_mtx();
            cMtx_YrotS(m, (s16)gabi::ftoi(cM_rndFX(32768.0f)));
            temp3.set(0.0f, 0.0f, 20.0f);
            MtxPosition(&temp3, &temp4);
            PSVECAdd(&temp4, &i_this->current.pos, &temp4);
            f32 speedF = cM_rndF(5.0f);
            f32 speedY = cM_rndF(10.0f) + 20.0f;
            fopAc_ac_c *pItem = kbGuest_fopAcM_fastCreateItem(
                &temp4, temp2, -1, &temp, &i_this->scale, speedF, speedY, -3.0f, -1, 0);
            if (pItem != nullptr) {
                pItem->actor_status |= 0x4000;
                if (count == 0) {
                    kb_se_start(i_this, JA_SE_OBJ_ITEM_OUT);
                }
            }
        }
    }
}

/* 02190BFC */
void BG_check(kb_class *i_this) {
    // HD: the wall height also depends on REG10_F(0) and the low bit of the debug switch word
    f32 hd = (f32)((u32)l_debugSwitch & 1);
    f32 h = gabi::fmadds(i_this->m4D4, REG_F(12, 0x11) + 10.0f, 22.0f) + REG_F(10, 0);
    i_this->mAcchCir.SetWall(gabi::fmadds(hd, 6.0f, h), gabi::fmadds(i_this->m4D4, 30.0f, 35.0f));
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    i_this->tevStr.mRoomNo = (s8)kbGuest_dBgS_GetRoomId(dComIfG_Bgsp(), kb_gnd_poly(i_this));
    i_this->tevStr.mEnvrIdxOverride =
        (u8)kbGuest_dBgS_GetPolyColor(dComIfG_Bgsp(), kb_gnd_poly(i_this)); /* mEnvrIdxOverride */
}

/* 02193770 */
static BOOL daKb_IsDelete(kb_class *) {
    return TRUE;
}

/* 02193778 */
static BOOL daKb_Delete(kb_class *i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Kb");
    if (i_this->heap.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    dPa_rippleEcallBack_end((dPa_rippleEcallBack *)&i_this->m540);
    vremove(&i_this->m554);
    vremove(&i_this->m574);
    vremove(&i_this->m594);
    for (int i = 0; i < 2; i++) {
        vremove(&i_this->m5A8[i]);
    }
    if (i_this->m406) {
        l_kbStatics.DEMO_START = 0;
        i_this->m406 = 0;
    }
    return TRUE;
}

/* 0219385C */
static BOOL useHeapInit(fopAc_ac_c *i_actor) {
    kb_class *i_this = (kb_class *)i_actor;
    u32 temp = i_this->mShapeType & 3;
    if (i_this->mShapeType >= 8) {
        temp += 3;
    }
    // HD: the body colour is a model per shape (kb_bdl_idx), no material
    // table
    J3DModelData *pModelData =
        (J3DModelData *)dComIfG_getObjectRes("Kb", kb_bdl_idx[temp], SAFESTRING_VTBL);
    J3DAnmTransform *anm =
        (J3DAnmTransform *)dComIfG_getObjectRes("Kb", dRes_INDEX_KB_BCK_WAIT1_e, SAFESTRING_VTBL);
    void *bas = dComIfG_getObjectRes("Kb", dRes_INDEX_KB_BAS_WAIT1_e, SAFESTRING_VTBL);
    i_this->mpMorf =
        mDoExt_McaMorf::create(nullptr, pModelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 1, bas, 0x00080000, 0x11020022);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    temp = i_this->mShapeType & 3;
    if (i_this->mShapeType >= 8) {
        temp += 3;
    }
    // HD: the eye animation is an mDoExt_btpAnm (GameCube: tex_anm_set / J3DTexNoAnm)
    void *btp = dComIfG_getObjectRes("Kb", kb_btp_idx[temp], SAFESTRING_VTBL);
    if (btp == nullptr) {
        JUT_ASSERT_fail("d_a_kb.cpp", 0xEEC, "btp");
    }
    if (!kbGuest_mDoExt_btpAnm_init(&i_this->mBtp, pModelData, btp, 1, 0, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    ((J3DModel *)i_this->mpMorf->getModel())->mUserArea = gabi::ea(i_actor);
    for (u16 i = 0;
         i < J3DModelData_getJointNum(((J3DModel *)i_this->mpMorf->getModel())->mModelData); i++) {
        setJointCallBack(((J3DModel *)i_this->mpMorf->getModel())->mModelData, i,
                         gabi::guest_fn((const void *)&nodeCallBack));
    }
    return TRUE;
}

/* 02193E48 */
static cPhs_State daKb_Create(fopAc_ac_c *i_actor) {
    kb_class *i_this = (kb_class *)i_actor;
    /* fopAcM_ct(i_actor, kb_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr)
            kb_class_ct(i_this);
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }

    cPhs_State status = dComIfG_resLoad(&i_this->mPhs, "Kb");
    if (status != cPhs_COMPLEATE_e) {
        return status;
    }
    u32 prm = fopAcM_GetParam(i_actor);
    u8 shape = prm & 0xF;
    u8 m401 = (prm >> 4) & 0xF;
    i_this->mShapeType = shape;
    i_this->m401 = m401;
    i_this->mbCanBeBigPig = prm >> 8;
    i_this->m41D = prm >> 0x10;
    if (shape == 0xFF) {
        i_this->mShapeType = 0;
    }
    if (m401 == 0xFF) {
        i_this->m401 = 0;
    }
    u8 pathNo = i_this->m41D;
    i_actor->health = 10;
    if (pathNo != 0xFF) {
        i_this->mpPath = dPath_GetRoomPath(pathNo, fopAcM_GetRoomNo(i_actor));
    }
    i_actor->gravity = -3.0f;

    if (i_this->mbCanBeBigPig != 0xFF) {
        if (kbIsStartStage("sea") && fopAcM_GetRoomNo(i_actor) == dIsleRoom_OutsetIsland_e) {
            u8 reg = dSv_event_getEventReg(dComIfGs_getEvent(), 0xBFFF /* UNK_BFFF */);
            u8 temp = 1 << (i_this->mShapeType & 3);
            if (REG_S(8, 8) != 0) {
                dSv_event_setEventReg(dComIfGs_getEvent(), 0xBFFF, 0);
                l_reg8s8 = 0; /* REG8_S(8) = 0 */
            }
            if (REG_S(8, 9) != 0 ||
                dSv_event_isEventBit(dComIfGs_getEvent(), 0x520 /* UNK_0520 */)) {
                if (i_this->mbCanBeBigPig == 0) {
                    return cPhs_ERROR_e;
                }
                u8 temp3 = reg & 7;
                u8 temp2 = i_this->mShapeType & 3;
                if (temp3 == 0 || temp3 == 7 || temp3 == 5 || temp3 == 6) {
                    if (temp2 != 2) {
                        return cPhs_ERROR_e;
                    }
                } else if (temp3 == 3) {
                    if (temp2 != 0) {
                        return cPhs_ERROR_e;
                    }
                } else if ((reg & temp) == 0) {
                    return cPhs_ERROR_e;
                }
                i_this->m4D4 = 2.0f;
                i_actor->mParameters = fopAcM_GetParam(i_actor) + 8;
                i_this->mShapeType += 8;
                i_actor->attention_info.distances[fopAc_Attn_TYPE_CARRY_e] =
                    0x12; /* attention_info.distances[fopAc_Attn_TYPE_CARRY_e] */
                i_actor->actor_status |= 0x10000; /* fopAcStts_UNK10000_e */
            } else {
                if ((reg & temp) == 0) {
                    if (i_this->mbCanBeBigPig == 1) {
                        return cPhs_ERROR_e;
                    }
                } else if (i_this->mbCanBeBigPig == 0) {
                    return cPhs_ERROR_e;
                }
            }
        }

        if (kbIsStartStage("sea") && fopAcM_GetRoomNo(i_actor) == dIsleRoom_WindfallIsland_e &&
            i_this->m41D != 0xFF && i_this->mpPath.get() != nullptr) {
            s32 point = gabi::ftoi(cM_rndF(
                (f32)gabi::at<KbPathHD>(gabi::ea(i_this->mpPath.get()))->m_num /* m_num */));
            KbPathHD *path = gabi::at<KbPathHD>(gabi::ea(i_this->mpPath.get()));
            if (point == 0) {
                point = 1;
            } else if ((u32)point == (u16)path->m_num) {
                point -= 1;
            }
            KbPathPointHD *pnt = &path->m_points[point]; /* m_points[point].m_position */
            i_actor->current.pos.x = (f32)pnt->m_position.x;
            f32 y = (f32)pnt->m_position.y;
            i_actor->current.pos.y = y;
            i_actor->current.pos.z = (f32)pnt->m_position.z;
            i_actor->current.pos.y = y + 150.0f;

            // HD: the pig is put on the ground
            dBgS_GndChk gndChk;
            dBgS_GndChk_ct(&gndChk, GNDCHK_VT, false);
            dBgS_GndChk_SetPos(&gndChk, &i_actor->current.pos);
            f32 h = cBgS_GroundCross(dComIfG_Bgsp(), &gndChk);
            if (h != -1000000000.0f) {
                i_actor->old.pos.x = (f32)i_actor->current.pos.x;
                i_actor->old.pos.y = h;
                i_actor->old.pos.z = (f32)i_actor->current.pos.z;
                i_actor->current.pos.y = h;
            }
            i_this->m401 = 3;
            if (i_this->mbCanBeBigPig == 2) {
                i_actor->health = 5;
            }
            dBgS_GndChk_dt(&gndChk);
        }
    }

    f32 r = (f32)i_this->m401 * 100.0f;
    if (r == 0.0f) {
        r = 300.0f;
    }
    i_this->m4CC = r;
    f32 s = i_this->m4D4 + 1.0f;
    i_this->m4E8.x = s;
    i_this->m4E8.y = s;
    i_this->m4E8.z = s;

    if (!fopAcM_entrySolidHeap(i_actor, gabi::guest_fn((const void *)&useHeapInit), 0x3AB4)) {
        return cPhs_ERROR_e;
    }

    kbGuest_dKy_tevstr_init(&i_this->mTevStr, i_actor->home.roomNo, 0xFF);
    i_actor->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
    i_this->mAcch.Set(&i_actor->current.pos, &i_actor->old.pos, i_actor, 1, &i_this->mAcchCir,
                      &i_actor->speed, nullptr, nullptr);
    i_this->mAcch.m_flags &= ~(u32)dBgS_Acch::WATER_NONE; /* ClrWaterNone */
    i_this->mAcch.SetWaterCheckOffset(300.0f);
    i_this->m446 = (s16)gabi::ftoi(cM_rndF(10000.0f));
    i_this->mStts.Init(0x32, 2, i_actor);
    i_this->mSph.Set(&co_sph_src);
    i_this->mSph.SetStts(&i_this->mStts);
    if (i_this->mShapeType >= 8) {
        i_this->mSph.SetAtAtp(0xC);
    }
    attn_flags(i_actor) = fopAc_Attn_LOCKON_MISC_e;
    i_actor->model = gabi::ea(i_this->mpMorf->getModel());
    if (i_this->mShapeType >= 8) {
        i_this->mStts.SetWeight(0xF0);
    } else {
        i_this->mStts.SetWeight(0x32);
    }
    // HD: no mSph.ClrAtSet()
    i_this->m4C4 = 30.0f;
    i_this->m4C0 = 30.0f;
    i_this->m426[7] = 5;
    BG_check(i_this);
    draw_SUB(i_this);
    return status;
}

// HD: compiler-generated and template code of this translation unit.
/* 02193AB8 */
static u32 followEcallBack_ct(dPa_followEcallBack *p) {
    return kbGuest_followEcallBack_ct(p, (u8)0, (u8)0);
}

// HD: the tev constructor copies three enlarged default light blocks.
static inline void kbLightInit(KbLightObjectHD *dst) {
    dst->position.x = (f32)l_lightDefaults.position.x;
    dst->position.y = (f32)l_lightDefaults.position.y;
    dst->position.z = (f32)l_lightDefaults.position.z;
    dst->direction.x = (f32)l_lightDefaults.direction.x;
    dst->direction.y = (f32)l_lightDefaults.direction.y;
    dst->direction.z = (f32)l_lightDefaults.direction.z;
    for (int i = 0; i < 4; ++i)
        dst->color[i] = (u8)l_lightDefaults.color[i];
    for (int i = 0; i < 4; ++i)
        dst->ambient[i] = (s16)l_lightDefaults.ambient[i];
    for (int i = 0; i < 2; ++i)
        dst->intensity[i] = (f32)l_lightDefaults.intensity[i];
    for (int i = 0; i < 3; ++i)
        dst->cosAtten[i] = (f32)l_lightDefaults.cosAtten[i];
    for (int i = 0; i < 3; ++i)
        dst->distanceAtten[i] = (f32)l_lightDefaults.distanceAtten[i];
}
static inline void tevstr_ct(dKy_tevstr_c *t) {
    kbLightInit(&kbLighting(t)->light);
    kbLightInit(&kbLighting(t)->materialLight);
    kbLightInit(&kbLighting(t)->secondMaterialLight);
}

/* 02193AC4 */
static kb_class *kb_class_ct(kb_class *self) {
    if (self == nullptr) {
        self = (kb_class *)operator_new(sizeof(kb_class));
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = KB_VTBL;
    tevstr_ct(&self->mTevStr);
    tevstr_ct(&self->m340);
    kbGuest_mDoExt_btpAnm_ct(&self->mBtp);
    kbGuest_dPa_rippleEcallBack_ct(&self->m540);
    kbGuest_dPa_smokeEcallBack_ct(&self->m554, 1);
    kbGuest_dPa_smokeEcallBack_ct(&self->m574, 1);
    dPa_followEcallBack_ct(&self->m594, 0, 0);
    kbGuest___construct_array(self->m5A8, 2, sizeof(dPa_followEcallBack),
                              gabi::guest_fn((const void *)&followEcallBack_ct));
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    kbGuest_dCcD_Sph_ct(&self->mSph); /* dCcD_Sph::dCcD_Sph */
    kbGuest_dCcD_Sph_ct(&self->m968);
    return self;
}

/* 02194594 */
static void __sinit_d_a_kb_cpp() {
    sinit_header_statics(gabi::ea(&l_staticListHead), gabi::ea(&l_staticConstructors));
}

/* 02194628 */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02194644 */
void cLib_offBit_u32(be<u32> *value, u32 bit) {
    *value &= ~bit;
}

/* 02194798 */
f32 JMASinShort(const void *table, s16 angle) {
    return (f32)gabi::at<KbSineEntry>(gabi::ea(table))[(u16)angle >> 3].sine;
}

/* 0219615C */
static void followEcallBack_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02196170 */
static void kb_class_dt(kb_class *self, s32 flags) {
    if (self == nullptr)
        return;
    kbGuest_dCcD_Sph_dt(&self->m968, 2); /* dCcD_Sph::~dCcD_Sph */
    kbGuest_dCcD_Sph_dt(&self->mSph, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    /* ~dBgS_ObjAcch (inline) -> ~dBgS_Acch */
    u32 acch = gabi::ea(&self->mAcch);
    self->mAcch.dBgS_GrpPassChk::__vtbl = OBJACCH_VT.v20;
    self->mAcch.dBgS_PolyPassChk::__vtbl = OBJACCH_VT.v14;
    kbGuest_dBgS_Acch_dt(acch, 0);                           /* dBgS_Acch::~dBgS_Acch */
    kbGuest_cM3dGCir_dt(gabi::ea(&self->mAcchCir.m_cir), 2); /* cM3dGCir::~cM3dGCir */
    kbGuest___destroy_arr(self->m5A8, 2, sizeof(dPa_followEcallBack),
                          gabi::guest_fn((const void *)&followEcallBack_dt), 0);
    kbGuest_fopAc_ac_c_dt(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(self);
}

/* 02196238 */
static void SafeString_assureTermination(void *) {}

// clang-format off
/* WWHD glue
data kb_btp_idx 101B7FC4
data kb_bdl_idx 101B7FD4
data gold_rate_dt 101B7FE4
data item_rate_dt 101B7FF0
data co_sph_src 101B7FFC
data l_sincosTable 104A44F8
data l_staticListHead 10464AF0
data l_staticConstructors 101B803C
data l_j3dSys 104B462C
data l_currentMtx 104B4868
str 100128E0 "Kb" in 0218EB64
str 100129C8 "Kb" in 02193778
str 100129D0 "Kb" in 0219385C
str 100129D4 "d_a_kb.cpp" in 0219385C
str 100129CC "btp" in 0219385C
str 100129E8 "Kb" in 02193E48
str 100129E4 "sea" in 02193E48
data l_kbStatics 101B7FA0
data l_debugSwitch 101FF560
data l_reg8s8 1047BB18
ext 025F1884 void kbGuest_cMtx_YrotS(Mtx34* matrix, u32 angle)
ext 025A5894 u32 kbGuest_followEcallBack_ct(void* p, u8 a, u8 b)
ext 025166F0 void kbGuest_dCcD_Sph_ct(dCcD_Sph* p)
ext 02515AE8 void kbGuest_dCcD_Sph_dt(dCcD_Sph* p, s32 flags)
ext 024EFD9C void kbGuest_dBgS_Acch_dt(u32 p, s32 flags)
ext 02018034 void kbGuest_cM3dGCir_dt(u32 p, s32 flags)
ext 025D50BC void kbGuest_fopAc_ac_c_dt(kb_class* p, s32 flags)
ext 02008DAC void kbGuest_cBgS_GndChk_dt(void* p, s32 flags)
ext 025052BC void kbGuest_dCamera_ForceLockOff(u32 camera, u32 id)
ext 027F3F94 void* kbGuest_getJointTree(J3DModelData* p)
data l_lightDefaults 1016E414
ext 025E1AA4 void kbGuest_mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb)
ext 028249B0 void kbGuest_JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t)
ext 024EF130 s32 kbGuest_dBgS_GetRoomId(dBgS* bgs, cBgS_PolyInfo* p)
ext 024EEEB8 s32 kbGuest_dBgS_GetPolyColor(dBgS* bgs, cBgS_PolyInfo* p)
ext 0200FAAC s32 kbGuest_cLib_distanceAngleS_i(s16 a, s16 b)
ext 025D8AB0 fopAc_ac_c* kbGuest_fopAcM_fastCreateItem(cXyz* pos, s32 itemNo, s32 roomNo, csXyz* rot, cXyz* scale, f32 speedF, f32 speedY, f32 gravity, s32 bitNo, u32 createFunc)
ext 0255FFF4 void kbGuest_dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p)
ext 025E7820 void kbGuest_mDoExt_btpAnm_ct(void* p)
ext 025E789C BOOL kbGuest_mDoExt_btpAnm_init(void* a, J3DModelData* d, void* btp, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry)
ext 025A9084 void kbGuest_dPa_rippleEcallBack_ct(void* p)
ext 025A5B18 void kbGuest_dPa_smokeEcallBack_ct(void* p, u8 a)
ext 028EFFD0 void kbGuest___construct_array(void* p, u32 n, u32 size, u32 ctor)
ext 028F0164 void kbGuest___destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 flags)
ext 02515BBC fopAc_ac_c* kbGuest_dCcD_GAtTgCoCommonBase_GetAc(void* p)
ext 02514F2C void kbGuest_dCamera_Stop(dCamera_c* c)
ext 02514F38 void kbGuest_dCamera_Start(dCamera_c* c)
ext 02515280 void kbGuest_dCamera_SetTrimSize(dCamera_c* c, s32 s)
ext 02514F88 void kbGuest_dCamera_Set(dCamera_c* c, cXyz* center, cXyz* eye, f32 fovy, s16 bank)
ext 0251510C void kbGuest_dCamera_Reset(dCamera_c* c, cXyz* center, cXyz* eye)
ext 025D7B24 BOOL kbGuest_fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p)
ext 024EF09C s32 kbGuest_dBgS_GetSpecialCode(dBgS* bgs, cBgS_PolyInfo* p)
ext 024EF0F4 s32 kbGuest_dBgS_GetAttributeCode(dBgS* bgs, cBgS_PolyInfo* p)
ext 02526560 void kbGuest_daTagKbItem_kb_dig(void* item, fopAc_ac_c* pig)
ext 025192A8 void kbGuest_cc_at_check(fopAc_ac_c* a, CcAtInfo* info)
ext 02008570 void kbGuest_cBgS_GetTriPnt(dBgS* bgs, void* poly, cXyz* a, cXyz* b, cXyz* c)
ext 02516C14 void kbGuest_dCcMassS_Mng_Set(void* manager, void* obj, u8 p)
ext 021901D4 u32 kbGuest_way_check_raw(kb_class* i_this, s16 angle, u8 p)
ext 02194798 f32 kbGuest_JMASinShort_(u32 table, s16 a)
*/
// clang-format on
