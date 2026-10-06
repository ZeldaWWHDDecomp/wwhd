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

/* 0218EB64 */
void anm_init(kb_class* i_this, int param_1, f32 param_2, u8 param_3, f32 param_4, int param_5) {
    WWHD_FUNC(0x0218EB64, void, i_this, param_1, param_2, param_3, param_4, param_5);
    i_this->m50C = param_1;
    if (param_5 >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100128E0) /* "Kb" */, param_1, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes(STR(0x100128E0), param_5, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, param_3, param_2, param_4, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100128E0), param_1, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, param_3, param_2, param_4, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x0218EB64, anm_init);

/* 0218EC90 */
void* esa_search_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x0218EC90, void*, param_1, param_2);
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VT, false);

    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == PROC_ESA) {
        fopAc_ac_c* pEsa = (fopAc_ac_c*)param_1;
        fopAc_ac_c* pActor = (fopAc_ac_c*)param_2;
        if (esa_field_0x298(pEsa) == 0 && esa_mState(pEsa) == 1 &&
            std::fabs(pActor->current.pos.y - pEsa->current.pos.y) < 40.0f) {
            if (fopAcM_searchActorDistanceXZ(pActor, pEsa) < 400.0f &&
                [&] { s16 ang = fopAcM_searchActorAngleY(pActor, pEsa); return cLib_distanceAngleS_i(pActor->current.angle.y, ang); }() < 0x55F0) {
                gabi::Local<cXyz> temp;
                temp->x = (f32)pEsa->current.pos.x;
                temp->y = pEsa->current.pos.y + 40.0f;
                temp->z = (f32)pEsa->current.pos.z;
                gabi::Local<cXyz> temp2;
                temp2->x = (f32)pActor->current.pos.x;
                temp2->y = pActor->current.pos.y + 40.0f;
                temp2->z = (f32)pActor->current.pos.z;
                dBgS_LinChk_Set(linChk, temp2, temp, pActor);
                if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                    dBgS_LinChk_dt(linChk);
                    return param_1;
                }
            }
        }
    }
    dBgS_LinChk_dt(linChk);
    return nullptr;
}
VERIFY(0x0218EC90, esa_search_sub);

/* 0218EEB4 */
void* item_tag_search(void* param_1, void* param_2) {
    WWHD_FUNC(0x0218EEB4, void*, param_1, param_2);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == PROC_TAG_KB_ITEM) {
        fopAc_ac_c* pItem = (fopAc_ac_c*)param_1;
        fopAc_ac_c* pActor = (fopAc_ac_c*)param_2;
        if (std::fabs(pActor->current.pos.y - pItem->current.pos.y) < 40.0f &&
            fopAcM_searchActorDistanceXZ(pActor, pItem) < 400.0f) {
            return param_1;
        }
    }
    return nullptr;
}
VERIFY(0x0218EEB4, item_tag_search);

/* 0218EF4C */
BOOL carry_check(kb_class* i_this) {
    WWHD_FUNC(0x0218EF4C, BOOL, i_this);
    attn_flags(i_this) |= fopAc_Attn_ACTION_CARRY_e;
    if (fopAcM_checkCarryNow(i_this)) {
        attn_flags(i_this) &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
        attn_flags(i_this) &= ~(u32)fopAc_Attn_LOCKON_MISC_e;
        i_this->mSph.OnTgSPrmBit(1); /* OnTgSetBit */
        i_this->m41E = 1;
        i_this->m420 = 10;
        i_this->m4CC = 0.0f;
        i_this->m440 = 0;
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m540);
        i_this->m404 = 0;
        i_this->gravity = 0.0f;
        i_this->speed.y = 0.0f;

        fopAc_ac_c* pActor = fopAcM_SearchByID(i_this->m4D8);
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

        anm_init(i_this, dRes_INDEX_KB_BCK_JITA2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_JITA2_e);
        kb_catch_se(i_this);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0218EF4C, carry_check);

/* 0218F150 */
void hamon_set(kb_class* i_this, f32 param_2) {
    WWHD_FUNC(0x0218F150, void, i_this, param_2);
    gabi::Local<cXyz> scale;
    f32 s = i_this->m5D0;
    scale->x = s;
    scale->y = s;
    scale->z = s;
    if (i_this->m540.mpEmitter.get() == nullptr) {
        /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &current.pos, NULL, &scale, 0xFF, &m540) */
        dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->current.pos, nullptr, scale, 0xFF,
                        (dPa_levelEcallBack*)&i_this->m540, -1, nullptr, nullptr, nullptr);
        if (i_this->m540.mpEmitter.get() == nullptr) {
            return;
        }
    }
    i_this->m540.mRate = param_2;
}
VERIFY(0x0218F150, hamon_set);

/* 0218F1FC */
void sibuki_set(kb_class* i_this) {
    WWHD_FUNC(0x0218F1FC, void, i_this);
    f32 scaleXZ = 0.4f;
    f32 scaleY = 0.75f;
    i_this->m440 = 1;
    gabi::Local<cXyz> pos;
    pos->copy(i_this->current.pos);
    pos->y = kb_wtr_height(i_this);
    i_this->m4C4 = 30.0f;
    if (i_this->m4D4 != 0.0f) {
        scaleXZ = REG_F(8, 4) + 0.8f;
    }
    fopKyM_createWpillar(pos, scaleXZ, scaleY, 0);
    kb_se_start(i_this, JA_SE_OBJ_FALL_WATER_S);
    kb_catch_se(i_this);
    i_this->m5D0 = gabi::fmadds(i_this->m4D4, 0.5f, 1.0f);
    hamon_set(i_this, 1.0f);
}
VERIFY(0x0218F1FC, sibuki_set);

/* 0218F380 */
BOOL swim_mode_change_check(kb_class* i_this) {
    WWHD_FUNC(0x0218F380, BOOL, i_this);
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
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m554);
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
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m540);
    }
    return FALSE;
}
VERIFY(0x0218F380, swim_mode_change_check);

/* 0218F4EC */
void he_set(kb_class* i_this) {
    WWHD_FUNC(0x0218F4EC, void, i_this);
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
    i_this->m5D4[0].copy(i_this->m4A4);
    i_this->m5EC[0].x = (s16)pPlayer->shape_angle.x;
    i_this->m5EC[0].y = (s16)pPlayer->shape_angle.y;
    i_this->m5EC[0].z = (s16)pPlayer->shape_angle.z;
    if (i_this->m594.mpEmitter.get() == nullptr) {
        i_this->m5D4[0].copy(i_this->m4A4);
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        dPa_control_set(dComIfGp_getParticle(), 0, 0x8286 /* ID_AK_SN_PIGGAS00 */, &i_this->m5D4[0], nullptr, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&i_this->m594, roomNo, nullptr, nullptr, nullptr);
    }
    JPABaseEmitter* emitter = i_this->m594.mpEmitter;
    if (emitter != nullptr) {
        /* setGlobalRTMatrix(getAnmMtx(PG_JNT_J_PG_TAIL_e)) */
        Mtx34* mtx = getAnmMtx(i_this->mpMorf->getModel(), PG_JNT_J_PG_TAIL_e);
        JPASetRMtxTVecfromMtx(mtx, gabi::ea(emitter) + 0x1F0, gabi::ea(emitter) + 0x22C);
    }
}
VERIFY(0x0218F4EC, he_set);

/* 0218F600 */
void smoke_set(kb_class* i_this) {
    WWHD_FUNC(0x0218F600, void, i_this);
    f32 scale = i_this->m4D4 + 1.0f;
    if (i_this->m554.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        /* dComIfGp_particle_setToon(ID_AK_JT_ELEMENTSMOKE00, &m5D4[0], &m5EC[0], NULL, 0xB9, &m554, roomNo) */
        dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &i_this->m5D4[0], &i_this->m5EC[0], nullptr, 0xB9,
                        (dPa_levelEcallBack*)&i_this->m554, roomNo, nullptr, nullptr, nullptr);
        if (i_this->m554.mpEmitter.get() == nullptr) {
            return;
        }
    }
    gabi::store<f32>(emitter_ea(i_this->m554) + 0x34, 1.0f); /* setRate */
    gabi::store<f32>(emitter_ea(i_this->m554) + 0x58, 1.0f); /* setSpread */
    u32 e = emitter_ea(i_this->m554);
    set_global_scale(e, scale, scale, scale);
}
VERIFY(0x0218F600, smoke_set);

/* 0218F71C */
void smoke_set2(kb_class* i_this) {
    WWHD_FUNC(0x0218F71C, void, i_this);
    f32 scale = i_this->m4D4 + 1.0f;
    if (i_this->m554.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        dPa_control_set(dComIfGp_getParticle(), 2, 0x2027, &i_this->m5D4[0], &i_this->shape_angle, nullptr, 0xB9,
                        (dPa_levelEcallBack*)&i_this->m554, roomNo, nullptr, nullptr, nullptr);
        if (i_this->m554.mpEmitter.get() == nullptr) {
            return;
        }
    }
    gabi::store<u32>(emitter_ea(i_this->m554) + 0x5C, 25);   /* setMaxFrame */
    gabi::store<f32>(emitter_ea(i_this->m554) + 0x68, 0.0f); /* setAwayFromCenterSpeed */
    gabi::store<f32>(emitter_ea(i_this->m554) + 0x6C, 5.0f); /* setAwayFromAxisSpeed */
    gabi::store<f32>(emitter_ea(i_this->m554) + 0x34, 3.0f); /* setRate */
    gabi::store<f32>(emitter_ea(i_this->m554) + 0x70, 2.0f); /* setDirectionalSpeed */
    u32 e = emitter_ea(i_this->m554);
    set_global_scale(e, scale, scale, scale);
    gabi::store<s16>(emitter_ea(i_this->m554) + 0x60, 25);   /* setLifeTime */
}
VERIFY(0x0218F71C, smoke_set2);

/* 0218F868 */
void smoke_set3(kb_class* i_this) {
    WWHD_FUNC(0x0218F868, void, i_this);
    f32 s;
    if (i_this->mShapeType >= 8) {
        s = 3.0f;
    } else {
        s = 1.0f;
    }
    if (i_this->m574.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        dPa_control_set(dComIfGp_getParticle(), 2, 0xA307 /* ID_AK_ST_PIGDIGSMOKE00 */, &i_this->m5D4[1], &i_this->shape_angle,
                        nullptr, 0xA0, (dPa_levelEcallBack*)&i_this->m574, roomNo, nullptr, nullptr, nullptr);
    }
    i_this->m574.mWindOff = 1; /* onWindOff */
    if (i_this->m574.mpEmitter.get() != nullptr) {
        set_global_scale(emitter_ea(i_this->m574), s, s, s);
    }
}
VERIFY(0x0218F868, smoke_set3);

/* 0218F98C */
static BOOL nodeCallBack(J3DNode* i_node, int calcTiming) {
    WWHD_FUNC(0x0218F98C, BOOL, i_node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(i_node);
        J3DModel_l* pModel = j3dSys_getModel();
        kb_class* i_kb = gabi::at<kb_class>(pModel->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_kb != nullptr && jntNo == PG_JNT_J_PG_TAIL_e) {
            PSMTXCopy(getAnmMtx((J3DModel*)pModel, jntNo), calc_mtx());
            gabi::Local<cXyz> temp;
            temp->x = 0.0f;
            temp->y = 0.0f;
            temp->z = 0.0f;
            MtxPosition(temp, &i_kb->m4A4);
            mtx_copy(getAnmMtx((J3DModel*)pModel, jntNo), calc_mtx()); /* setAnmMtx */
            PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
        }
    }
    return TRUE;
}
VERIFY(0x0218F98C, nodeCallBack);

/* 0218FABC */
void draw_SUB(kb_class* i_this) {
    WWHD_FUNC(0x0218FABC, void, i_this);
    J3DModel* pModel = i_this->mpMorf->getModel();
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
VERIFY(0x0218FABC, draw_SUB);

/* dKy_tevstr_c::operator= (HD, inline): the members are copied one by one (floats through FPRs);
 * three 0x40-byte blocks are not copied */
static inline void tevstr_copy(dKy_tevstr_c* dst, dKy_tevstr_c* src) {
    u32 d = gabi::ea(dst), s = gabi::ea(src);
    auto f = [&](u32 off, int n) { for (int i = 0; i < n; i++) gabi::store<f32>(d + off + 4 * i, gabi::load<f32>(s + off + 4 * i)); };
    auto b = [&](u32 off, int n) { for (int i = 0; i < n; i++) gabi::store<u8>(d + off + i, gabi::load<u8>(s + off + i)); };
    /* three blocks of the same shape at 0x00, 0xC0 and 0x144; the colours at 0x84..0xBD */
    f(0x00, 6); b(0x18, 4); b(0x1C, 8); f(0x24, 8);
    b(0x84, 0x30);
    f(0xA8, 3); b(0xB4, 9);
    f(0xC0, 6); b(0xD8, 4); b(0xDC, 8); f(0xE4, 8);
    f(0x144, 6); b(0x15C, 4); b(0x160, 8); f(0x168, 8);
}

/* 0218FC3C */
static BOOL daKb_Draw(kb_class* i_this) {
    WWHD_FUNC(0x0218FC3C, BOOL, i_this);
    J3DModel* pModel = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    /* HD: the eye texture animation is an mDoExt_btpAnm */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)&i_this->mBtp, J3DModel_getModelData(pModel),
                        (s16)gabi::ftoi(i_this->mBtp.mFrameCtrl.mFrame));
    tevstr_copy(&i_this->mTevStr, &i_this->tevStr);
    u32 src = gabi::ea(&i_this->tevStr), dst = gabi::ea(&i_this->mTevStr);
    f32 t = i_this->m4B0;
    /* mColorC0 (s16 rgba at +0x90), mColorK0 (u8 rgba at +0x98) */
    s16 r = gabi::load<s16>(src + 0x90);
    gabi::store<s16>(dst + 0x90, (s16)gabi::ftoi(gabi::fmadds((f32)(0x78 - r), t, (f32)r)));
    s16 g = gabi::load<s16>(src + 0x92);
    gabi::store<s16>(dst + 0x92, (s16)gabi::ftoi(gabi::fmadds((f32)(-g), t, (f32)g)));
    s16 bl = gabi::load<s16>(src + 0x94);
    gabi::store<s16>(dst + 0x94, (s16)gabi::ftoi(gabi::fmadds((f32)(-bl), t, (f32)bl)));
    u8 kr = gabi::load<u8>(src + 0x98);
    gabi::store<u8>(dst + 0x98, (u8)gabi::ftoi(gabi::fmadds((f32)(0x78 - kr), t, (f32)kr)));
    u8 kg = gabi::load<u8>(src + 0x99);
    gabi::store<u8>(dst + 0x99, (u8)gabi::ftoi(gabi::fmadds((f32)(-kg), t, (f32)kg)));
    u8 kb = gabi::load<u8>(src + 0x9A);
    gabi::store<u8>(dst + 0x9A, (u8)gabi::ftoi(gabi::fmadds((f32)(-kb), t, (f32)kb)));
    setLightTevColorType(dKy_getEnvlight(), pModel, &i_this->mTevStr);
    i_this->mpMorf->updateDL();

    /* HD: no blob shadow (the play object is still fetched) */
    dComIfGp_get();
    if (i_this->m4D4 == 2.0f) {
        dSnap_RegistFig(DSNAP_TYPE_KB, i_this, i_this->m4D4 + 0.6f, i_this->m4D4 + -0.1f, 1.0f);
    } else {
        dSnap_RegistFig(DSNAP_TYPE_KB, i_this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}
VERIFY(0x0218FC3C, daKb_Draw);

/* 021901D4 */
s16 way_check(kb_class* i_this, s16 param_1, u8 param_2) {
    WWHD_FUNC(0x021901D4, s16, i_this, param_1, param_2);
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VT, false);
    s16 angle = param_1;
    gabi::Local<cXyz> temp3;
    temp3->x = (f32)i_this->current.pos.x;
    temp3->y = i_this->current.pos.y + 50.0f;
    temp3->z = (f32)i_this->current.pos.z;
    s16 temp = 0x2000;
    if (cM_rnd() < 0.5f) {
        temp = -0x2000;
    }
    gabi::Local<cXyz> temp1;
    temp1->set(0.0f, 50.0f, 300.0f);
    if (param_2) {
        temp3->x = (f32)i_this->current.pos.x;
        temp3->y = i_this->current.pos.y + 100.0f;
        temp3->z = (f32)i_this->current.pos.z;
        temp1->x = 0.0f;
        temp1->y = 100.0f;
        temp1->z = 100.0f;
    }
    gabi::Local<cXyz> temp2;
    for (int i = 0; i < 8; i++) {
        cMtx_YrotS(calc_mtx(), angle);
        MtxPosition(temp1, temp2);
        PSVECAdd(temp2, &i_this->current.pos, temp2);
        dBgS_LinChk_Set(linChk, temp3, temp2, i_this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            dBgS_LinChk_dt(linChk);
            return angle;
        }
        angle += temp;
    }
    dBgS_LinChk_dt(linChk);
    return param_1;
}
VERIFY(0x021901D4, way_check);

static inline u32 way_check_raw(kb_class* i_this, s16 angle, u8 p) { return gabi::call<u32>(0x021901D4, i_this, angle, p); }

/* 02190400 */
void target_set(kb_class* i_this, u8 param_1) {
    WWHD_FUNC(0x02190400, void, i_this, param_1);
    dComIfGp_get(); /* daPy_getPlayerActorClass() (unused) */
    /* HARNESS WORKAROUND: GHS passes way_check's r3 to YrotS unchanged (way_check returns a
     * sign-extended s16); the mock's r3 may have other upper bits, so the value is kept raw */
    u32 temp = 0;
    switch (param_1) {
    case 0:
        temp = (u32)(s32)i_this->shape_angle.y;
        if (i_this->m426[3] == 0) {
            temp = way_check_raw(i_this, (s16)gabi::ftoi(cM_rndFX(32768.0f)), 0);
        }
        break;
    case 1: {
        s32 a = fopAcM_searchPlayerAngleY(i_this) + 0x8000;
        temp = way_check_raw(i_this, (s16)(a + (s16)gabi::ftoi(cM_rndFX(10000.0f))), 0);
        break;
    }
    case 2: {
        f32 dx = i_this->m450.x - i_this->current.pos.x;
        f32 dz = i_this->m450.z - i_this->current.pos.z;
        temp = way_check_raw(i_this, cM_atan2s(dx, dz), 1);
        i_this->m422 = (s16)temp;
        break;
    }
    case 3:
        if (i_this->m4D8 != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c* pActor = fopAcM_SearchByID(i_this->m4D8);
            if (pActor != nullptr) {
                gabi::Local<dBgS_LinChk> linChk;
                dBgS_LinChk_ct(linChk, LINCHK_VT, false);
                gabi::Local<cXyz> temp3;
                temp3->x = (f32)pActor->current.pos.x;
                temp3->y = pActor->current.pos.y + 10.0f;
                temp3->z = (f32)pActor->current.pos.z;
                gabi::Local<cXyz> temp4;
                temp4->x = (f32)i_this->current.pos.x;
                temp4->y = i_this->current.pos.y + 10.0f;
                temp4->z = (f32)i_this->current.pos.z;
                dBgS_LinChk_Set(linChk, temp4, temp3, i_this);
                if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                    i_this->m45C.copy(pActor->current.pos);
                } else {
                    i_this->m4D8 = fpcM_ERROR_PROCESS_ID_e;
                }
                dBgS_LinChk_dt(linChk);
            }
        }
        break;
    }
    if (param_1 < 3) {
        gabi::call(0x025F1884, calc_mtx(), temp); /* cMtx_YrotS */
        gabi::Local<cXyz> temp2;
        temp2->set(0.0f, 0.0f, 1000.0f);
        MtxPosition(temp2, &i_this->m45C);
        PSVECAdd(&i_this->m45C, &i_this->current.pos, &i_this->m45C);
    }
}
VERIFY(0x02190400, target_set);

/* 02190708 */
BOOL esa_demo_check(kb_class* i_this) {
    WWHD_FUNC(0x02190708, BOOL, i_this);
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
    fopAc_ac_c* pEsa = (fopAc_ac_c*)fpcM_Search(0x0218EC90 /* esa_search_sub */, i_this);
    if (pEsa != nullptr) {
        i_this->m4D8 = fopAcM_GetID(pEsa);
        i_this->field_0x498.copy(pEsa->current.pos);
        if (!DEMO_START()) {
            DEMO_START() = 1;
            i_this->m406 = 1;
            i_this->m41E = 4;
            i_this->m420 = 0x28;
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x02190708, esa_demo_check);

/* 02190BFC */
void BG_check(kb_class* i_this) {
    WWHD_FUNC(0x02190BFC, void, i_this);
    /* HD: the wall height also depends on REG10_F(0) and the low bit of the word at 0x101FF560 */
    f32 hd = (f32)(gabi::load<u32>(0x101FF560) & 1);
    f32 h = gabi::fmadds(i_this->m4D4, REG_F(12, 0x11) + 10.0f, 22.0f) + REG_F(10, 0);
    i_this->mAcchCir.SetWall(gabi::fmadds(hd, 6.0f, h), gabi::fmadds(i_this->m4D4, 30.0f, 35.0f));
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    i_this->tevStr.mRoomNo = (s8)dBgS_GetRoomId(dComIfG_Bgsp(), kb_gnd_poly(i_this));
    gabi::store<u8>(gabi::ea(&i_this->tevStr) + 0xBA, (u8)dBgS_GetPolyColor(dComIfG_Bgsp(), kb_gnd_poly(i_this))); /* mEnvrIdxOverride */
}
VERIFY(0x02190BFC, BG_check);

/* 02193770 */
static BOOL daKb_IsDelete(kb_class*) {
    WWHD_FUNC(0x02193770, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02193770, daKb_IsDelete);

/* 02193778 */
static BOOL daKb_Delete(kb_class* i_this) {
    WWHD_FUNC(0x02193778, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x100129C8) /* "Kb" */);
    if (i_this->heap.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m540);
    vremove(&i_this->m554);
    vremove(&i_this->m574);
    vremove(&i_this->m594);
    for (int i = 0; i < 2; i++) {
        vremove(&i_this->m5A8[i]);
    }
    if (i_this->m406) {
        DEMO_START() = 0;
        i_this->m406 = 0;
    }
    return TRUE;
}
VERIFY(0x02193778, daKb_Delete);

/* 021907F4 */
void money_drop(kb_class* i_this) {
    WWHD_FUNC(0x021907F4, void, i_this);
    /* gold_rate_dt (0x101B7FE4) and item_rate_dt (0x101B7FF0): u32[3] */
    int i;
    gabi::Local<csXyz> temp;
    temp->x = (s16)i_this->current.angle.x;
    temp->y = (s16)i_this->current.angle.y;
    temp->z = (s16)i_this->current.angle.z;
    gabi::Local<cXyz> temp3;
    gabi::Local<cXyz> temp4;
    u8 count = 0;
    i_this->m407 = 0;

    u32 temp2;
    for (i = 0; i < 2; i++) {
        f32 rnd = cM_rnd();
        if (REG_S(8, 4) == 0) {
            if (rnd < 0.9f) {
                rnd *= 10.0f;
                temp2 = gabi::load<u32>(0x101B7FE4 + 4 * gabi::ftoi(rnd * 0.3f));
                i_this->m407 = 1;
                Mtx34* m = calc_mtx();
                cMtx_YrotS(m, (s16)gabi::ftoi(cM_rndFX(32768.0f)));
                temp3->set(0.0f, 0.0f, 20.0f);
                MtxPosition(temp3, temp4);
                PSVECAdd(temp4, &i_this->current.pos, temp4);
                f32 speedF = cM_rndF(5.0f);
                f32 speedY = cM_rndF(10.0f) + 20.0f;
                fopAc_ac_c* pItem = fopAcM_fastCreateItem(temp4, temp2, -1, temp, &i_this->scale, speedF, speedY, -3.0f, -1, 0);
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
            temp2 = gabi::load<u32>(0x101B7FF0 + 4 * gabi::ftoi(rnd * 0.3f));
            i_this->m407 = 1;
            Mtx34* m = calc_mtx();
            cMtx_YrotS(m, (s16)gabi::ftoi(cM_rndFX(32768.0f)));
            temp3->set(0.0f, 0.0f, 20.0f);
            MtxPosition(temp3, temp4);
            PSVECAdd(temp4, &i_this->current.pos, temp4);
            f32 speedF = cM_rndF(5.0f);
            f32 speedY = cM_rndF(10.0f) + 20.0f;
            fopAc_ac_c* pItem = fopAcM_fastCreateItem(temp4, temp2, -1, temp, &i_this->scale, speedF, speedY, -3.0f, -1, 0);
            if (pItem != nullptr) {
                pItem->actor_status |= 0x4000;
                if (count == 0) {
                    kb_se_start(i_this, JA_SE_OBJ_ITEM_OUT);
                }
            }
        }
    }
}
VERIFY(0x021907F4, money_drop);

/* 0219385C */
static BOOL useHeapInit(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x0219385C, BOOL, i_actor);
    kb_class* i_this = (kb_class*)i_actor;
    u32 temp = i_this->mShapeType & 3;
    if (i_this->mShapeType >= 8) {
        temp += 3;
    }
    /* HD: the body colour is a model per shape (kb_bdl_idx, 0x101B7FD4), no material table */
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100129D0) /* "Kb" */, kb_bdl_idx(temp), SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100129D0), dRes_INDEX_KB_BCK_WAIT1_e, SAFESTRING_VTBL);
    void* bas = dComIfG_getObjectRes(STR(0x100129D0), dRes_INDEX_KB_BAS_WAIT1_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, pModelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                            bas, 0x00080000, 0x11020022);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    temp = i_this->mShapeType & 3;
    if (i_this->mShapeType >= 8) {
        temp += 3;
    }
    /* HD: the eye animation is an mDoExt_btpAnm (GameCube: tex_anm_set / J3DTexNoAnm) */
    void* btp = dComIfG_getObjectRes(STR(0x100129D0), kb_btp_idx(temp), SAFESTRING_VTBL);
    if (btp == nullptr) {
        JUT_ASSERT_fail(STR(0x100129D4) /* "d_a_kb.cpp" */, 0xEEC, STR(0x100129CC) /* "btp" */);
    }
    if (!mDoExt_btpAnm_init(&i_this->mBtp, pModelData, btp, 1, 0, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    ((J3DModel_l*)i_this->mpMorf->getModel())->mUserArea = gabi::ea(i_actor);
    for (u16 i = 0; i < J3DModelData_getJointNum(((J3DModel_l*)i_this->mpMorf->getModel())->mModelData); i++) {
        setJointCallBack(((J3DModel_l*)i_this->mpMorf->getModel())->mModelData, i, 0x0218F98C /* nodeCallBack */);
    }
    return TRUE;
}
VERIFY(0x0219385C, useHeapInit);

/* 02193AB8: dPa_followEcallBack array element constructor (__construct_array) */
static u32 followEcallBack_ct(dPa_followEcallBack* p) {
    WWHD_FUNC(0x02193AB8, u32, p);
    return gabi::call<u32>(0x025A5894, p, (u8)0, (u8)0);
}
VERIFY(0x02193AB8, followEcallBack_ct);

/* dKy_tevstr_c::dKy_tevstr_c (HD, inline): three light blocks from the default at 0x1016E414 */
static inline void tevstr_ct(dKy_tevstr_c* t) {
    u32 d = gabi::ea(t);
    const u32 s = 0x1016E414;
    static const u32 blocks[3] = {0x00, 0xC0, 0x144};
    for (u32 o : blocks) {
        for (int i = 0; i < 6; i++) gabi::store<f32>(d + o + 4 * i, gabi::load<f32>(s + 4 * i));
        for (int i = 0; i < 4; i++) gabi::store<u8>(d + o + 0x18 + i, gabi::load<u8>(s + 0x18 + i));
        for (int i = 0; i < 4; i++) gabi::store<s16>(d + o + 0x1C + 2 * i, gabi::load<s16>(s + 0x1C + 2 * i));
        for (int i = 0; i < 8; i++) gabi::store<f32>(d + o + 0x24 + 4 * i, gabi::load<f32>(s + 0x24 + 4 * i));
    }
}

/* 02193AC4: kb_class::kb_class (HD: allocates when this == NULL) */
static kb_class* kb_class_ct(kb_class* self) {
    WWHD_FUNC(0x02193AC4, kb_class*, self);
    if (self == nullptr) {
        self = (kb_class*)operator_new(sizeof(kb_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = KB_VTBL;
    tevstr_ct(&self->mTevStr);
    tevstr_ct(&self->m340);
    mDoExt_btpAnm_ct(&self->mBtp);
    dPa_rippleEcallBack_ct(&self->m540);
    dPa_smokeEcallBack_ct(&self->m554, 1);
    dPa_smokeEcallBack_ct(&self->m574, 1);
    dPa_followEcallBack_ct(&self->m594, 0, 0);
    __construct_array(self->m5A8, 2, sizeof(dPa_followEcallBack), 0x02193AB8);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mSph); /* dCcD_Sph::dCcD_Sph */
    gabi::call(0x025166F0, &self->m968);
    return self;
}
VERIFY(0x02193AC4, kb_class_ct);

/* 02193E48 */
static cPhs_State daKb_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02193E48, cPhs_State, i_actor);
    kb_class* i_this = (kb_class*)i_actor;
    /* fopAcM_ct(i_actor, kb_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) kb_class_ct(i_this);
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }

    cPhs_State status = dComIfG_resLoad(&i_this->mPhs, STR(0x100129E8) /* "Kb" */);
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
        if (dComIfGp_isStartStage(STR_SEA) && fopAcM_GetRoomNo(i_actor) == dIsleRoom_OutsetIsland_e) {
            u8 reg = dSv_event_getEventReg(dComIfGs_getEvent(), 0xBFFF /* UNK_BFFF */);
            u8 temp = 1 << (i_this->mShapeType & 3);
            if (REG_S(8, 8) != 0) {
                dSv_event_setEventReg(dComIfGs_getEvent(), 0xBFFF, 0);
                gabi::store<s16>(0x1047BB18, 0); /* REG8_S(8) = 0 */
            }
            if (REG_S(8, 9) != 0 || dSv_event_isEventBit(dComIfGs_getEvent(), 0x520 /* UNK_0520 */)) {
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
                gabi::store<u8>(gabi::ea(i_actor) + 0x38C, 0x12); /* attention_info.distances[fopAc_Attn_TYPE_CARRY_e] */
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

        if (dComIfGp_isStartStage(STR_SEA) && fopAcM_GetRoomNo(i_actor) == dIsleRoom_WindfallIsland_e && i_this->m41D != 0xFF &&
            i_this->mpPath.get() != nullptr) {
            s32 point = gabi::ftoi(cM_rndF((f32)gabi::load<u16>(gabi::ea(i_this->mpPath.get())) /* m_num */));
            u32 path = gabi::ea(i_this->mpPath.get());
            if (point == 0) {
                point = 1;
            } else if ((u32)point == gabi::load<u16>(path)) {
                point -= 1;
            }
            u32 pnt = gabi::load<u32>(path + 8) + point * 0x10; /* m_points[point].m_position */
            i_actor->current.pos.x = gabi::load<f32>(pnt + 4);
            f32 y = gabi::load<f32>(pnt + 8);
            i_actor->current.pos.y = y;
            i_actor->current.pos.z = gabi::load<f32>(pnt + 0xC);
            i_actor->current.pos.y = y + 150.0f;

            /* HD: the pig is put on the ground */
            gabi::Local<dBgS_GndChk> gndChk;
            dBgS_GndChk_ct(gndChk, GNDCHK_VT, false);
            dBgS_GndChk_SetPos(gndChk, &i_actor->current.pos);
            f32 h = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
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
            dBgS_GndChk_dt(gndChk);
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

    if (!fopAcM_entrySolidHeap(i_actor, 0x0219385C /* useHeapInit */, 0x3AB4)) {
        return cPhs_ERROR_e;
    }

    dKy_tevstr_init(&i_this->mTevStr, i_actor->home.roomNo, 0xFF);
    i_actor->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
    i_this->mAcch.Set(&i_actor->current.pos, &i_actor->old.pos, i_actor, 1, &i_this->mAcchCir, &i_actor->speed, nullptr, nullptr);
    i_this->mAcch.m_flags &= ~(u32)dBgS_Acch::WATER_NONE; /* ClrWaterNone */
    i_this->mAcch.SetWaterCheckOffset(300.0f);
    i_this->m446 = (s16)gabi::ftoi(cM_rndF(10000.0f));
    i_this->mStts.Init(0x32, 2, i_actor);
    i_this->mSph.Set(co_sph_src);
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
    /* HD: no mSph.ClrAtSet() */
    i_this->m4C4 = 30.0f;
    i_this->m4C0 = 30.0f;
    i_this->m426[7] = 5;
    BG_check(i_this);
    draw_SUB(i_this);
    return status;
}
VERIFY(0x02193E48, daKb_Create);

/* 02194594: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kb_cpp() {
    WWHD_FUNC(0x02194594, void, (u32)0);
    sinit_header_statics(0x10464AF0, 0x101B803C);
}
VERIFY(0x02194594, __sinit_d_a_kb_cpp);

/* 02194628: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02194628, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02194628, SafeString_dt);

/* 0219463C: kb_eye_tex_anm (HD: plays the eye mDoExt_btpAnm; out of line) */
BOOL kb_eye_tex_anm(kb_class* i_this) {
    WWHD_FUNC(0x0219463C, BOOL, i_this);
    return mDoExt_baseAnm_play(&i_this->mBtp);
}
VERIFY(0x0219463C, kb_eye_tex_anm);

/* 02194644: cLib_offBit<u32> (out of line) */
void cLib_offBit_u32(be<u32>* value, u32 bit) {
    WWHD_FUNC(0x02194644, void, value, bit);
    *value &= ~bit;
}
VERIFY(0x02194644, cLib_offBit_u32);

/* 02194654 */
void speed_pos_set(kb_class* i_this) {
    WWHD_FUNC(0x02194654, void, i_this);
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
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
        gabi::Local<cXyz> temp1;
        gabi::Local<cXyz> temp2;
        temp1->x = 0.0f;
        temp1->y = 0.0f;
        temp1->z = (f32)i_this->speedF;
        MtxPosition(temp1, temp2);
        i_this->speed.x = (f32)temp2->x;
        f32 sy = i_this->speed.y + i_this->gravity;
        i_this->speed.z = (f32)temp2->z;
        if (sy < -20.0f) {
            sy = -20.0f;
        }
        i_this->speed.y = sy;
        /* HD: no mSph.ChkCoSet() / GetCCMoveP() path */
        if (!fopAcM_checkCarryNow(i_this)) {
            fopAcM_posMove(i_this, nullptr);
        }
    }
}
VERIFY(0x02194654, speed_pos_set);

/* 02194798: JMath sin table lookup (out of line): table[(u16)angle >> 3].sin */
f32 JMASinShort(const void* table, s16 angle) {
    WWHD_FUNC(0x02194798, f32, table, angle);
    return gabi::load<f32>(gabi::ea(table) + (((s32)(u16)angle >> 3) << 3));
}
VERIFY(0x02194798, JMASinShort);

/* 0219615C: dPa_followEcallBack array element deleting destructor (__destroy_arr) */
static void followEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0219615C, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x0219615C, followEcallBack_dt);

/* 02196170: kb_class deleting destructor (HD virtual destructor) */
static void kb_class_dt(kb_class* self, s32 flags) {
    WWHD_FUNC(0x02196170, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x02515AE8, &self->m968, 2); /* dCcD_Sph::~dCcD_Sph */
    gabi::call(0x02515AE8, &self->mSph, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    /* ~dBgS_ObjAcch (inline) -> ~dBgS_Acch */
    u32 acch = gabi::ea(&self->mAcch);
    gabi::store<u32>(acch + 0x20, OBJACCH_VT.v20);
    gabi::store<u32>(acch + 0x14, OBJACCH_VT.v14);
    gabi::call(0x024EFD9C, acch, 0);                                /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2);  /* cM3dGCir::~cM3dGCir */
    __destroy_arr(self->m5A8, 2, sizeof(dPa_followEcallBack), 0x0219615C, 0);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x02196170, kb_class_dt);

/* 02196238: empty virtual (sead::SafeString::assureTerminationImpl_, this TU's vtable 0x100127E0 +0x14) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x02196238, void, (u32)0);
}
VERIFY(0x02196238, SafeString_assureTermination);
