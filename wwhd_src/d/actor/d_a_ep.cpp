/**
 * d_a_ep.cpp (WWHD)
 * Object - Torches
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ep.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD changes: no alpha-model glow in Draw (ep_draw is gone), a flicker factor (+0x600) on the
 * point light, a darker light in some stage types, fire/heat-haze particles suppressed on the
 * "sea" stage outside a few areas, the light-info block copied to two HD-only slots.
 */
#include "d/actor/d_a_ep.h"

const gc_tu_data gc_tu = {.safeString = SAFESTRING_VTBL};

static const char* l_arcName = "Ep";
static const char* l_eventName = "SHOKUDAI_SWITCH";
static const char* l_staffName = "SHOKUDAI";
static const char* l_assertFile = "d_a_ep.cpp";
static const char* l_modelAssert = "modelData != NULL";
static EpAction actions[2] = {{"WAIT"}, {"FIRE"}};
// HD: distinct guest copies of "sea" and "SHOKUDAI_SWITCH" are used in Execute.
static u8 l_seaFire[4] = {115, 101, 97, 0};
static u8 l_seaHeat[4] = {115, 101, 97, 0};
static u8 l_eventCheckName[16] = {83, 72, 79, 75, 85, 68, 65, 73, 95, 83, 87, 73, 84, 67, 72, 0};
static u8 l_eventOrderName[16] = {83, 72, 79, 75, 85, 68, 65, 73, 95, 83, 87, 73, 84, 67, 72, 0};
static EpSphereSource sph_src = {0x0, 0x20000, 0x0, 0x0,  0x0,  0x0,  0xe,  0x660200, 0x9,
                                 0x0, 0x0,     0x0, 0x0,  0x0,  0x0,  0x0,  0x0,      0x0,
                                 0x0, 0x0,     0x0, 0.0f, 0.0f, 0.0f, 40.0f};
static EpCylinderSource co_cyl_src = {0x0,  0x0, 0x0, 0x0,  0x0,  0x0,  0x0,   0xff1dfebf, 0xf,
                                      0x79, 0x0, 0x0, 0x0,  0x0,  0x0,  0x0,   0x0,        0x0,
                                      0x0,  0x1, 0x0, 0.0f, 0.0f, 0.0f, 35.0f, 120.0f};

static EpLightInfo j3dDefaultLightInfo = {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 255,  255,
                                          255,  255,  255,  255,  255,   255,  1.0f, 1.0f,
                                          1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f};

/* HD: SafeString(lit) == the current stage name (play+0x5134), and the HD flag byte at
 * 0x1047E6C8 is 1 */
/* inlined into daEp_Execute */
static bool ep_chk_sea_stage(const be<u8>* lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::ea(lit);
    a->__vtbl = SAFESTRING_VTBL;
    bool r = false;
    u32 play = dComIfGp_ea();
    gabi::Local<SafeString> b;
    b->mStringTop = play + PLAY_OFF(mCurStage.mName);
    b->__vtbl = SAFESTRING_VTBL;
    SafeString_assure(a.get());
    SafeString_assure(a.get());
    u32 s1 = a->mStringTop;
    SafeString_assure(b.get());
    u32 s2 = b->mStringTop;
    bool eq = false;
    if (s1 == s2) {
        eq = true;
    } else {
        const be<u8>* left = gabi::at<be<u8>>(s1);
        const be<u8>* right = gabi::at<be<u8>>(s2);
        for (u32 i = 0; i < 0x40001; i++) {
            u8 c0 = left[i];
            u8 c1 = right[i];
            if (c0 != c1)
                break;
            if (c0 == 0) {
                eq = true;
                break;
            }
        }
    }
    if (eq && dComIfGp_roomControl_getStayNo() == 1)
        r = true;
    return r;
}
/* HD: the squared XZ distance of the player from (-300000, 0, -300000) */
/* inlined into daEp_Execute */
static f32 ep_sea_dist2(cXyz* player_pos) {
    gabi::Local<cXyz> c;
    c->x = -300000.0f;
    c->y = 0.0f;
    c->z = -300000.0f;
    gabi::Local<cXyz> d;
    cXyz_mi(player_pos, d.get(), c.get());
    gabi::Local<cXyz> xz;
    f32 dz = d->z, dx = d->x;
    xz->z = dz;
    xz->x = dx;
    xz->y = 0.0f;
    return PSVECSquareMag(xz.get());
}
/* inlined into daEp_Execute */
static void ep_player_pos(gabi::Local<cXyz>& p) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    p->x = pl->current.pos.x;
    p->y = pl->current.pos.y;
    p->z = pl->current.pos.z;
}

/* ga_draw, inlined in daEp_Draw */
/* inlined into daEp_Draw */
static void ga_draw(ep_class* i_this) {
    ep_ga_s* ga = i_this->mEpGa;
    for (s32 i = 0; i < 2; i++, ga++) {
        if (ga->mbEnabled == 1) {
            MtxTrans(ga->mPos.x, ga->mPos.y, ga->mPos.z, false);
            cMtx_YrotM(calc_mtx(), ga->rotY);
            cMtx_XrotM(calc_mtx(), ga->rotX);
            f32 s = ga->scaleXZ;
            MtxScale(s, s * ga->scaleY, s, true);
            MtxTrans(0.0f, REG_F(10, 9) + -2.0f, 0.0f, true); /* HD: GameCube -18.0f */
            J3DModel_setBaseTRMtx(ga->mpModel, calc_mtx());
            mDoExt_modelUpdateDL(ga->mpModel);
        }
    }
}

// HD: no ep_draw (alpha-model glow); the model is drawn when mbShouldDrawModel is 0
/* 0212D8DC */
static BOOL daEp_Draw(ep_class* i_this) {
    if ((i_this->mType == EP_TORCH) || (i_this->mType == EP_TORCH_NO_EVENT)) {
        if (i_this->mbShouldDrawModel == 0) {
            dComIfGd_setListBG();
            settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos,
                             &i_this->tevStr);
            setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
            mDoExt_modelUpdateDL(i_this->mpModel);
            dComIfGd_setList();
        }
        if (i_this->mbHasGa != 0) {
            ga_draw(i_this);
        }
    }
    return TRUE;
}

// HD: endCheckOld(name)
/* 0212DAE8 */
BOOL ep_switch_event_end(ep_class* i_this) {
    BOOL ret = FALSE;
    if (ep_eventEndCheckOld(l_eventName)) {
        dComIfGp_event_reset();
        ret = TRUE;
    }
    return ret;
}

/* 0212DB44 */
BOOL ep_switch_event_move(ep_class* i_this) {
    BOOL ret = FALSE;
    if (dComIfGp_evmng_getIsAddvance(i_this->m7E0)) {
        ret = dComIfGp_evmng_getMyActIdx(i_this->m7E0, reinterpret_cast<gptr<char>*>(actions), 2,
                                         FALSE, 0);
        switch (ret) {
        case 1:
            mDoAud_seStart2(JA_SE_OBJ_TORCH_IGNITION, &i_this->mPosTop);
            break;
        case 0:
        default:
            ret = FALSE;
            dComIfGp_evmng_cutEnd(i_this->m7E0);
            break;
        }
    }
    return ret;
}

/* ga_move, inlined in daEp_Execute */
/* inlined into daEp_Execute */
static void ga_move(ep_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    ep_ga_s* ga = &i_this->mEpGa[0];
    gabi::Local<cXyz> sp88;
    gabi::Local<cXyz> sp7C;
    gabi::Local<cXyz> sp58;
    sp88->set(0.0f, 0.0f, 10.0f);
    sp58->set(10000.0f, 10000.0f, 10000.0f);
    daPy_getBokoFlamePos(player, sp58.get());
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk.get(), EP_LINCHK_VT, false);
    for (s32 i = 0; i < 2; i++, ga++) {
        if (ga->mbEnabled != 0) {
            if (ga->m3B != 0) {
                ga->m3B = ga->m3B - 1;
            } else {
                ga->m3B = (u8)gabi::ftoi(cM_rndF(10.0f));
                f32 r = cM_rndFX(150.0f);
                ga->m1C.x = i_this->mPosTop.x + r;
                r = cM_rndFX(100.0f);
                ga->m1C.y = i_this->mPosTop.y + r;
                r = cM_rndFX(150.0f);
                ga->m1C.z = i_this->mPosTop.z + r;
                if (i == 0) {
                    gabi::Local<cXyz> tmp;
                    cXyz_mi(sp58.get(), tmp.get(), &ga->mPos);
                    gabi::Local<cXyz> sp64;
                    sp64->copy(*tmp.get());
                    if (std_sqrtf(PSVECSquareMag(sp64.get())) < 300.0f) { /* sp64.abs() */
                        r = cM_rndFX(100.0f);
                        ga->m1C.x = sp58->x + r;
                        r = cM_rndFX(100.0f);
                        ga->m1C.y = sp58->y + r;
                        r = cM_rndFX(100.0f);
                        ga->m1C.z = sp58->z + r;
                    }
                }
                f32 gy = ga->mPos.y;
                if (i_this->m4D0 == EP_OFF) {
                    ga->m1C.y = ga->m1C.y + 1000.0f;
                }
                if (std::fabs(i_this->mPosTop.y - gy) > 500.0f) {
                    ga->mbEnabled = 2;
                } else {
                    ga->mbEnabled = 1;
                }
            }
            gabi::Local<cXyz> sp70;
            cXyz_mi(&ga->m1C, sp70.get(), &ga->mPos);
            f32 x = sp70->x, z = sp70->z, y = sp70->y;
            cLib_addCalcAngleS2(&ga->rotY, cM_atan2s(x, z), 2, 0x1000);
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            cLib_addCalcAngleS2(&ga->rotX, (s16)-cM_atan2s(y, d), 2, 0x1000);

            cMtx_YrotS(calc_mtx(), ga->rotY);
            cMtx_XrotM(calc_mtx(), ga->rotX);
            MtxPosition(sp88.get(), sp7C.get());
            ga->m10.copy(ga->mPos);
            PSVECAdd(&ga->mPos, sp7C.get(), &ga->mPos); /* ga->mPos += sp7C */
            if (i_this->m7D4 != 0) {
                gabi::Local<cXyz> sp4C;
                cMtx_YrotS(calc_mtx(), i_this->m7DC);
                f32 k = REG0_F(9) + 5.0f;
                sp4C->x = 0.0f;
                sp4C->y = 1.0f;
                sp4C->z = i_this->m7D8 * k;
                MtxPosition(sp4C.get(), sp7C.get());
                PSVECAdd(&ga->mPos, sp7C.get(), &ga->mPos);
                /* sp7C = ga->m10 + (ga->mPos - ga->m10) * 1.05f */
                gabi::Local<cXyz> t0, t1, t2;
                cXyz_mi(&ga->mPos, t0.get(), &ga->m10);
                cXyz_ml(t0.get(), t1.get(), 1.05f);
                cXyz_pl(&ga->m10, t2.get(), t1.get());
                sp7C->copy(*t2.get());
                dBgS_LinChk_Set(linChk.get(), &ga->m10, sp7C.get(), i_this);
                if (cBgS_LineCross(dComIfG_Bgsp(), linChk.get())) {
                    ga->mPos.copy(linChk->mLin.mEnd);
                }
            }
            u16 a = (u16)ga->m38;
            ga->m38 = (s16)(a + 0x3E00);
            ga->scaleY = cM_ssin(a);
        }
    }
    /* ~dBgS_LinChk */
    linChk->dBgS_PolyPassChk::__vtbl = EP_LINCHK_VT.v58;
    linChk->dBgS_GrpPassChk::__vtbl = 0x1000E294;
    linChk->cBgS_PolyInfo::__vtbl = 0x1000E284;
    cBgS_LinChk_dt(linChk.get(), 0);
}

/* the point-light update at the end of ep_move */
/* inlined into daEp_Execute */
static void ep_light(ep_class* i_this) {
    i_this->mLight.mPos.copy(i_this->mPosTop);
    i_this->mLight.mColorR = 600;
    i_this->mLight.mColorG = 400;
    i_this->mLight.mColorB = 0x78;
    f32 k = 1.0f;
    if (dComIfGp_getStageStagInfo() != nullptr) {
        EpStageInfo* info = static_cast<EpStageInfo*>(dComIfGp_getStageStagInfo());
        u32 type = (info->mTypeAndFlags >> 16) & 7;
        if (type == 1 || type == 4)  /* HD: dimmer in these stage types */
            k = 0.6521739363670349f; /* 15/23 */
    }
    i_this->mLight.mPower =
        ((150.0f * i_this->mLightPower) * i_this->mFlicker) * k; /* HD: flicker, k */
    i_this->mLight.mFluctuation = 250.0f;
    cLib_addCalc2(&i_this->mFlicker, cM_rndF(0.2f) + 0.9f, 0.25f, 0.01f);
}

/* ep_move, inlined in daEp_Execute */
/* inlined into daEp_Execute */
static void ep_move(ep_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> pos;
    pos->x = i_this->mPosTop.x;
    pos->z = i_this->mPosTop.z;
    pos->y = (i_this->mPosTop.y + 10.0f) + REG_F(6, 0); /* -240 + 235 + 15 */
    switch ((u32)(s32)i_this->m4D0) {
    case EP_OFF:
        if (i_this->m508 != 0) {
            cLib_addCalc0(&i_this->mLightPower, 0.5f, 0.05f);
            if (i_this->mSph1.ChkTgHit()) {
                void* hitObj = i_this->mSph1.GetTgHitObj();
                if (hitObj == nullptr || !cCcD_Obj_ChkAtType(hitObj, 0x600000)) {
                    if (i_this->m505 != 0xff) {
                        i_this->mTimers[EP_EXTINGUISH_TIMER] = i_this->m505 * 10 + 1;
                    }
                    dComIfGs_onSwitch(i_this->m508 - 1, fopAcM_GetRoomNo(i_this));
                }
            }
            if (dComIfGs_isSwitch(i_this->m508 - 1, fopAcM_GetRoomNo(i_this))) {
                if (i_this->mType != EP_TORCH_NO_EVENT) {
                    /* dComIfGp_evmng_existence("SHOKUDAI_SWITCH") */
                    dEvent_manager_c* manager =
                        &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mEvtManager;
                    u32 idx = ep_eventIndex(reinterpret_cast<const char*>(l_eventCheckName));
                    if (ep_eventData(manager, idx) != 0) {
                        /* ep_switch_event_begin, inlined */
                        s32 begin = FALSE;
                        if (!i_this->eventInfo.checkCommandDemoAccrpt()) {
                            fopAcM_orderOtherEvent2(i_this,
                                                    reinterpret_cast<const char*>(
                                                        l_eventOrderName) /* "SHOKUDAI_SWITCH" */,
                                                    1, 0xFFFF);
                        } else {
                            i_this->eventInfo.onCondition(dEvtCnd_UNK2_e);
                            begin = dComIfGp_evmng_getMyStaffId(l_staffName, nullptr, 0);
                            if (begin == -1) {
                                ep_switch_event_end(i_this);
                            }
                        }
                        i_this->m7E0 = begin;
                        if (begin != 0) {
                            i_this->m508 = 0;
                            if (i_this->m7E0 != -1) {
                                ep_switch_event_move(i_this);
                            }
                        }
                        break;
                    }
                }
                mDoAud_seStart2(JA_SE_OBJ_TORCH_IGNITION, &i_this->mPosTop);
                i_this->m508 = 0;
            }
        } else if (i_this->mType != EP_TORCH_NO_EVENT && i_this->m7E0 != -1) {
            if (ep_switch_event_move(i_this)) {
                i_this->m4D0 = EP_IGNITE;
                i_this->m4E0 = i_this->scale.x;
            }
        } else {
            i_this->m4D0 = EP_IGNITE;
            i_this->m4E0 = i_this->scale.x;
        }
        break;
    case EP_IGNITE:
        i_this->m4D0 = EP_BURN;
        [[fallthrough]];
    case EP_BURN:
        cLib_addCalc2(&i_this->mLightPower, i_this->m4E0, 0.5f, 0.2f);
        if (i_this->mType != EP_LIGHT_ONLY) {
            if (i_this->m7D4 < (s16)(REG0_S(7) + 7)) {
                /* HD: no fire on the sea stage outside a few areas */
                bool suppress = false;
                if (ep_chk_sea_stage(l_seaFire)) {
                    gabi::Local<cXyz> pp;
                    ep_player_pos(pp);
                    f32 py = pp->y, px = pp->x, pz = pp->z;
                    if (py < 300.0f && px < -290000.0f && pz > -297500.0f) {
                        f32 y = i_this->current.pos.y;
                        if (!(y < 500.0f) && !(2750.0f < y && y < 2800.0f) &&
                            !(1900.0f < y && y < 1950.0f) && !(2350.0f < y && y < 2400.0f)) {
                            if (!(ep_sea_dist2(pp.get()) < 12250000.0f))
                                suppress = true;
                        }
                    }
                }
                if (!suppress)
                    dComIfGp_particle_setSimple(1 /* ID_AK_JN_O_FIRE00 */, pos.get());
                if (i_this->m7D4 == 0 && i_this->mSph1.ChkTgHit()) {
                    void* hitObj = i_this->mSph1.GetTgHitObj();
                    if (hitObj != nullptr && cCcD_Obj_ChkAtType(hitObj, 0x600000)) {
                        /* hitObj->GetAc() (HD: through its dCcD_Stts, NULL-checked) */
                        dCcD_Stts* stts = static_cast<dCcD_GObjInf*>(hitObj)->mStts;
                        fopAc_ac_c* ac = stts != nullptr ? stts->mp_actor.get() : nullptr;
                        if (ac != nullptr && fpcM_GetName(ac) == fpcNm_BDK_e) {
                            i_this->m7DC = ac->shape_angle.y;
                        } else {
                            i_this->m7DC = player->shape_angle.y;
                        }
                        i_this->m7D4 = REG0_S(2) + 40;
                    }
                }
            }
            pos->y = pos->y + 20.0f;
            /* HD: no heat haze on the sea stage away from the area */
            gabi::Local<cXyz> pp;
            ep_player_pos(pp);
            bool suppress = false;
            if (ep_chk_sea_stage(l_seaHeat)) {
                if (pp->y < 300.0f && !(ep_sea_dist2(pp.get()) < 12250000.0f))
                    suppress = true;
            }
            if (!suppress)
                dComIfGp_particle_setSimple(0x4004 /* ID_AK_JP_O_KAGEROU00 */, pos.get());
        }
        if (i_this->mTimers[EP_EXTINGUISH_TIMER] == 1 &&
            (i_this->m507 == 0xFF || !dComIfGs_isSwitch(i_this->m507, fopAcM_GetRoomNo(i_this)))) {
            dComIfGs_offSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(i_this));
        }
        if (i_this->mOnSwitchNo != 0xFF &&
            !dComIfGs_isSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(i_this))) {
            i_this->m4D0 = EP_OFF;
            if (i_this->mOnSwitchNo != 0xff) {
                i_this->m508 = i_this->mOnSwitchNo + 1;
            }
            mDoAud_seStart2(JA_SE_OBJ_TORCH_OFF, &i_this->mPosTop);
        } else {
            mDoAud_seStart2(JA_SE_OBJ_TORCH_BURNING, &i_this->mPosTop);
        }
        [[fallthrough]];
    default:
        if (i_this->mType != EP_TORCH_NO_EVENT && i_this->m7E0 != -1) {
            if (ep_switch_event_end(i_this)) {
                i_this->m7E0 = -1;
            } else {
                dComIfGp_evmng_cutEnd(i_this->m7E0);
            }
        }
        break;
    case EP_EXTINGUISH:
        cLib_addCalc0(&i_this->mLightPower, 1.0f, 0.1f);
        if (i_this->mLightPower < 0.05f) {
            if (i_this->mType == EP_TORCH || i_this->mType == EP_TORCH_NO_EVENT) {
                i_this->m4D0 = EP_OFF;
                if (i_this->mOnSwitchNo != 0xFF) {
                    dComIfGs_offSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(i_this));
                    i_this->m508 = i_this->mOnSwitchNo + 1;
                }
            } else {
                fopAcM_delete(i_this);
            }
        }
        break;
    }
    ep_light(i_this);
    f32 lp = i_this->mLightPower;
    i_this->scale.y = lp;
    if (lp > 0.5f) {
        i_this->mSph1.OnAtSPrmBit(1); /* OnAtSetBit */
    } else {
        i_this->mSph1.OffAtSPrmBit(1);
    }
    gabi::Local<cXyz> sp3C;
    sp3C->x = i_this->mPosTop.x;
    sp3C->y = i_this->mPosTop.y + 30.0f;
    sp3C->z = i_this->mPosTop.z;
    i_this->mSph1.SetC(sp3C.get());
    dComIfG_Ccsp_Set(&i_this->mSph1);
    if (i_this->m7D4 != 0) {
        s16 tmp = REG0_S(2) + 40;
        if (i_this->m7D4 == tmp && i_this->mpEmitter == nullptr) {
            gabi::Local<cXyz> scale;
            f32 s = REG0_F(6) + 1.0f;
            scale->z = s;
            scale->y = s;
            scale->x = s;
            pos->z = i_this->mPosTop.z;
            pos->x = i_this->mPosTop.x;
            pos->y = (i_this->mPosTop.y + 3.0f) + REG_F(6, 4); /* -240 + 235 + 8 */
            i_this->mpEmitter =
                dComIfGp_particle_set(0x1EA /* ID_AK_JN_TORCH */, pos.get(), nullptr, scale.get());
        }
        if (i_this->mpEmitter != nullptr) {
            f32 target = (i_this->m7D4 > (s16)(REG0_S(3) + 10)) ? REG0_F(5) + 4.0f : 0.0f;
            cLib_addCalc2(&i_this->m7D8, target, 1.0f, REG0_F(8) + 0.5f);
            cMtx_YrotS(calc_mtx(), i_this->m7DC);
            gabi::Local<cXyz> sp24;
            gabi::Local<cXyz> sp18;
            sp24->set(0.0f, 1.0f, i_this->m7D8);
            MtxPosition(sp24.get(), sp18.get());
            f32 dx = sp18->x, dz = sp18->z, dy = sp18->y;
            JPABaseEmitter_setDirection(i_this->mpEmitter, dx, dy, dz);
            if (i_this->m7D4 == 1) {
                /* becomeInvalidEmitter() */
                EpEmitterStatus* e = reinterpret_cast<EpEmitterStatus*>(i_this->mpEmitter.get());
                e->mMaxFrame = -1;
                e->mStatus |= 1;
                i_this->mpEmitter = nullptr;
            }
        }
        i_this->m7D4 = i_this->m7D4 - 1;
    }
}

/* 0212DBFC */
static BOOL daEp_Execute(ep_class* i_this) {
    fopAc_ac_c* a_this = i_this;
    i_this->mbNoEp = 0;
    i_this->mbShouldDrawModel = 0;
    if ((i_this->mType == EP_TORCH) || (i_this->mType == EP_TORCH_NO_EVENT)) {
        i_this->mCyl.SetC(&a_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mCyl);
        if (i_this->mCyl.ChkTgHit()) {
            daObj_HitSeStart(&a_this->current.pos, fopAcM_GetRoomNo(a_this), &i_this->mCyl, 11);
        }
        fopAcM_rollPlayerCrash(a_this, 35.0f, 0);
        mDoLib_clipper_changeFar(1000000.0f);
        i_this->mbShouldDrawModel = fopAcM_checkCullingBox(
            J3DModel_getBaseTRMtx(i_this->mpModel), -30.0f, 0.0f, -30.0f, 30.0f, 180.0f, 30.0f);
        mDoLib_clipper_resetFar();
        if (i_this->mbHasGa != 0) {
            ga_move(i_this);
        }
    }
    for (s32 i = 0; i < 3; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i] = i_this->mTimers[i] - 1;
        }
    }
    if ((i_this->mTimers[EP_EXTINGUISH_TIMER] != 0) &&
        (i_this->mTimers[EP_EXTINGUISH_TIMER] < 10000)) {
        i_this->mTimers[EP_EXTINGUISH_TIMER] = i_this->mTimers[EP_EXTINGUISH_TIMER] - 1;
    }
    if (i_this->mTimers[EP_ALPHA_TIMER] == 0) {
        i_this->mTimers[EP_ALPHA_TIMER] = (s16)gabi::ftoi(cM_rndF(REG0_F(2) + 5.0f) + REG0_F(3));
        i_this->mAlphaModelAlphaTarget = (cM_rndF(REG0_F(6) + 4.0f) + 8.0f) + REG0_F(7);
    }
    if (i_this->mTimers[EP_SCALE_TIMER] == 0) {
        if (i_this->m7D4 != 0) {
            i_this->mTimers[EP_SCALE_TIMER] = (s16)gabi::ftoi(cM_rndF(5.0f));
            i_this->mAlphaModelScaleTarget = cM_rndF(0.2f) + 0.55f;
        } else {
            i_this->mTimers[EP_SCALE_TIMER] = (s16)gabi::ftoi(cM_rndF(6.0f) + 3.0f);
            i_this->mAlphaModelScaleTarget = cM_rndF(0.075f) + 0.75f;
        }
    }
    /* HD: REG0_F(4) + 1.0f (GameCube REG0_F(4) + 0.1f + 0.9f) */
    cLib_addCalc2(&i_this->mAlphaModelAlpha, i_this->mAlphaModelAlphaTarget, 1.0f,
                  REG0_F(4) + 1.0f);
    cLib_addCalc2(&i_this->mAlphaModelScale, i_this->mAlphaModelScaleTarget, 0.4f, 0.04f);
    MtxTrans(i_this->mPosTop.x, i_this->mPosTop.y, i_this->mPosTop.z, false);
    cMtx_YrotM(calc_mtx(), i_this->mAlphaModelRotY);
    cMtx_XrotM(calc_mtx(), i_this->mAlphaModelRotX);
    f32 scale = i_this->mAlphaModelScale * i_this->mLightPower;
    MtxScale(scale, scale, scale, true);
    PSMTXCopy(calc_mtx(), &i_this->mAlphaModelMtx); /* cMtx_copy */
    mDoLib_clipper_changeFar(1000000.0f);
    i_this->mbNoEp = fopAcM_checkCullingBox(&i_this->mAlphaModelMtx, -160.0f, -160.0f, -160.0f,
                                            160.0f, 160.0f, 160.0f);
    mDoLib_clipper_resetFar();
    ep_move(i_this);
    i_this->mAlphaModelRotY = i_this->mAlphaModelRotY + 0xd0;
    i_this->mAlphaModelRotX = i_this->mAlphaModelRotX + 0x100;
    if (i_this->m4D0 > EP_OFF) {
        if (i_this->m4D0 < EP_EXTINGUISH) {
            i_this->mGroundCheckTimer = i_this->mGroundCheckTimer + 1;
            if ((i_this->mGroundCheckTimer & 0xF) == 0) {
                /* dBgS_ObjGndChk_Spl gndChk */
                gabi::Local<dBgS_GndChk> gndChk;
                dBgS_GndChk_ct(gndChk.get(), EP_GNDCHK_VT, true);
                gndChk->mGrp = 0xE; // Spl group
                gabi::Local<cXyz> sp08;
                f32 z = i_this->mPosTop.z;
                f32 y = i_this->mPosTop.y + 200.0f;
                f32 x = i_this->mPosTop.x;
                sp08->z = z;
                sp08->x = x;
                cXyz* p = &gndChk->m_pos;
                p->x = x;
                p->y = y;
                p->z = z;
                f32 fVar6 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
                if ((fVar6 != -1000000000.0f) && (i_this->mPosTop.y < fVar6)) {
                    i_this->m4D0 = EP_EXTINGUISH;
                }
                /* ~dBgS_ObjGndChk_Spl */
                gndChk->cBgS_PolyInfo::__vtbl = 0x1000E2B4;
                gndChk->dBgS_PolyPassChk::__vtbl = 0x1000E2D4;
                gndChk->dBgS_GrpPassChk::__vtbl = 0x1000E294;
                ep_destroyGroundCheck(gndChk.get(), 0);
            }
        }
    }
    return TRUE;
}

/* 0212F578 */
static BOOL daEp_IsDelete(ep_class* i_this) { return TRUE; }

/* 0212F580 */
static BOOL daEp_Delete(ep_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, l_arcName); /* dComIfG_resDeleteDemo */
    dKy_plight_cut(&i_this->mLight);
    mDoAud_seDeleteObject(&i_this->mPosTop);
    return TRUE;
}

/* 0212F5CC */
static BOOL daEp_CreateHeap(fopAc_ac_c* a_this) {
    ep_class* i_this = (ep_class*)a_this;
    J3DModelData* modelData;
    if (i_this->mbHasObm == 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_EP_BDL_VKTSD_e,
                                                        SAFESTRING_VTBL);
    } else {
        modelData = (J3DModelData*)dComIfG_getObjectRes(
            l_arcName, dRes_INDEX_EP_BDL_OBM_SHOKUDAI1_e, SAFESTRING_VTBL);
    }
    if (modelData == nullptr) /* JUT_ASSERT(997, modelData != NULL) */
        JUT_ASSERT_fail(l_assertFile, 0x477, l_modelAssert);
    i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (i_this->mpModel == nullptr) {
        return FALSE;
    }
    if (i_this->mbHasGa != 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_EP_BDL_EP_GA_e,
                                                        SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(1010, modelData != NULL) */
            JUT_ASSERT_fail(l_assertFile, 0x484, l_modelAssert);
        for (s32 i = 0; i < 2; i++) {
            i_this->mEpGa[i].mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (i_this->mEpGa[i].mpModel == nullptr) {
                return FALSE;
            }
            if (i == 0 || cM_rndF(1.0f) < 0.5f) {
                ep_ga_s& ga = i_this->mEpGa[i];
                ga.mbEnabled = true;
                ga.mPos.x = a_this->current.pos.x;
                f32 y = a_this->current.pos.y;
                ga.mPos.y = y;
                ga.mPos.z = a_this->current.pos.z;
                ga.mPos.y = y + 140.0f;
                ga.scaleXZ = cM_rndF(0.3f) + 0.3f;
                ga.m38 = (s16)gabi::ftoi(cM_rndF(30000.0f));
            }
        }
    }
    return TRUE;
}

// ep_class::ep_class() (HD: allocates when this == NULL)
/* 0212F878 */
static ep_class* ep_class_ct(ep_class* self) {
    if (self == nullptr) {
        self = (ep_class*)operator_new(0xB50);
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = EP_VTBL;
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, EP_OBJACCH_VT);
    self->mLight.mHD20 = 1.0f;
    dCcD_Stts_ct(&self->mStts);
    dCcD_Cyl_ct(&self->mCyl, 0x1000E274);
    dCcD_Sph_ct(&self->mSph1);
    // HD: read the complete default light first, then copy it to all three light-info blocks.
    const EpLightInfo* src = &j3dDefaultLightInfo;
    f32 fa[6], fb[8];
    u8 b[4];
    s16 h[4];
    for (int i = 0; i < 6; i++)
        fa[i] = src->mPositionAndDirection[i];
    for (int i = 0; i < 4; i++)
        b[i] = src->mColor[i];
    for (int i = 0; i < 4; i++)
        h[i] = src->mHdParameters[i];
    for (int i = 0; i < 8; i++)
        fb[i] = src->mAttenuation[i];
    EpLightInfo* dst[3] = {&self->m860, &self->mHdLightInfo1, &self->mHdLightInfo2};
    for (EpLightInfo* d : dst) {
        for (int i = 0; i < 6; i++)
            d->mPositionAndDirection[i] = fa[i];
        for (int i = 0; i < 4; i++)
            d->mColor[i] = b[i];
        for (int i = 0; i < 4; i++)
            d->mHdParameters[i] = h[i];
        for (int i = 0; i < 8; i++)
            d->mAttenuation[i] = fb[i];
    }
    return self;
}

/* daEp_set_mtx, inlined */
/* inlined into daEp_Create */
static void daEp_set_mtx(ep_class* i_this) {
    if ((i_this->mType == EP_TORCH) || (i_this->mType == EP_TORCH_NO_EVENT)) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
        cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
        gabi::Local<cXyz> sp08;
        sp08->x = 0.0f;
        sp08->z = 0.0f;
        sp08->y = REG0_F(0) + 140.0f;
        MtxPosition(sp08.get(), &i_this->mPosTop);
    } else {
        i_this->mPosTop.copy(i_this->current.pos);
    }
}

/* daEp_CreateInit, inlined */
/* inlined into daEp_Create */
static void daEp_CreateInit(fopAc_ac_c* a_this) {
    ep_class* i_this = (ep_class*)a_this;
    i_this->m4D0 = EP_OFF;
    i_this->m508 = 0;
    fopAcM_SetMtx(a_this, &i_this->mAlphaModelMtx);
    fopAcM_SetMin(a_this, -160.0f, -160.0f, -160.0f);
    fopAcM_SetMax(a_this, 160.0f, 160.0f, 160.0f);
    i_this->mAlphaModelRotX = (s16)gabi::ftoi(cM_rndF(0x8000));
    i_this->mAlphaModelRotY = (s16)gabi::ftoi(cM_rndF(0x8000));
    cXyz* att = &a_this->attention_info.position;
    f32 x = a_this->current.pos.x, y = a_this->current.pos.y;
    att->x = x;
    att->y = y + 100.0f;
    f32 z = a_this->current.pos.z;
    att->z = z;
    a_this->eyePos.x = x;
    a_this->eyePos.y = y + 130.0f;
    a_this->eyePos.z = z;
    daEp_set_mtx(i_this);
    i_this->mbNoEp = 0;
    i_this->mbShouldDrawModel = 0;
}

/* 0212FAF4 */
static cPhs_State daEp_Create(fopAc_ac_c* a_this) {
    ep_class* i_this = (ep_class*)a_this;
    /* fopAcM_ct(a_this, ep_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            ep_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, l_arcName);
    if (ret == cPhs_COMPLEATE_e) {
        i_this->mType = fopAcM_GetParam(a_this) & 0x3F;
        if (i_this->mType == 0x3F) {
            i_this->mType = EP_TORCH;
        }
        i_this->mbHasGa = (fopAcM_GetParam(a_this) >> 6) & 1;
        i_this->mbHasObm = (fopAcM_GetParam(a_this) >> 7) & 1;
        i_this->m505 = fopAcM_GetParam(a_this) >> 8;
        i_this->m507 = fopAcM_GetParam(a_this) >> 0x10;
        i_this->mOnSwitchNo = fopAcM_GetParam(a_this) >> 0x18;
        i_this->mStts.Init(0xFF, 0xFF, a_this);
        i_this->mSph1.Set(reinterpret_cast<const dCcD_SrcSph*>(&sph_src));
        i_this->mSph1.SetStts(&i_this->mStts);
        if ((i_this->mType == EP_TORCH) || (i_this->mType == EP_TORCH_NO_EVENT)) {
            u32 maxHeapSize;
            if (i_this->mbHasObm == 0) {
                maxHeapSize = 0x4C0;
            } else {
                maxHeapSize = 0x4E0;
            }
            if (i_this->mbHasGa != 0) {
                maxHeapSize += 0x9A0;
            }
            if (!fopAcM_entrySolidHeap(a_this, daEp_CreateHeap, maxHeapSize)) {
                return cPhs_ERROR_e;
            }
            i_this->mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&co_cyl_src));
            i_this->mCyl.SetStts(&i_this->mStts);
            i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir,
                              &a_this->speed, nullptr, nullptr);
            i_this->mAcchCir.SetWall(10.0f, 20.0f);
            i_this->mAcch.CrrPos(dComIfG_Bgsp());
        }
        daEp_CreateInit(a_this);
        if (i_this->mOnSwitchNo != 0xFF &&
            !dComIfGs_isSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(a_this))) {
            i_this->m508 = i_this->mOnSwitchNo + 1;
        }
        i_this->mTimers[EP_EXTINGUISH_TIMER] = 20000;
        dKy_plight_set(&i_this->mLight);
        i_this->mGroundCheckTimer = (u8)gabi::ftoi(cM_rndF(255.0f));
    }

    i_this->m7E0 = -1;
    i_this->mFlicker = 1.0f; /* HD */
    return ret;
}

/* 0212FF28 */
static void __sinit_d_a_ep_cpp() { sinit_header_statics(0x10463C78, 0x101B4954); }

// sead::SafeString deleting destructor (this TU's SafeString vtable)
/* 0212FFBC */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// ep_class deleting destructor (vtable +0xC)
/* 0212FFD0 */
static void ep_class_dt(ep_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        ep_destroySphere(&i_this->mSph1, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        i_this->mAcch.dBgS_GrpPassChk::__vtbl = EP_OBJACCH_VT.v20;
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = EP_OBJACCH_VT.v14;
        ep_destroyAcch(&i_this->mAcch, 0);
        ep_destroyCircle(&i_this->mAcchCir.m_cir, 2);
        ep_destroyActor(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

// sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 02130078 */
static void SafeString_assureTerminationImpl(void* p) {}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 1000E418 in 0212DAE8
str 1000E404 in 0212DBFC
str 1000E4F8 in 0212F580
str 1000E508 in 0212F5CC
str 1000E50C
str 1000E518 "modelData != NULL"
str 1000E538 in 0212FAF4
data l_seaFire 1000E258
data l_seaHeat 1000E254
data l_eventCheckName 1000E3F4
data l_eventOrderName 1000E3E4
data actions 101B48C8
data sph_src 101B48D0
data co_cyl_src 101B4910
data j3dDefaultLightInfo 1016E414
*/
