// Molgera execute sequence.
#include "d/actor/d_a_bwd_execute.h"

static Mtx34 executeMatrix;
typedef gptr<Mtx34> ExecuteMatrixPointer;
static ExecuteMatrixPointer executeMatrixPointer;
static GXColor particleWhite;
static bwd_execute_detail::CollisionOffsets jointOffsets;
static u32 jointOffsetsInitialized;
// KUTIL2/3/4, KUTIR2/3/4, HIP, HIREL3, HIRER3 (GameCube BWD_JNT order).
static s32 jno[] = {0x10, 0x12, 0x13, 0x17, 0x19, 0x1A, 4, 7, 0xB};
static f32 jsize[] = {180.0f, 120.0f, 90.0f, 180.0f, 120.0f, 90.0f, 230.0f, 150.0f, 150.0f};
static s16 demo_ang[] = {0x3E80, -0x4480};

static BOOL hookshotReturnCheck(fopAc_ac_c*) {
    u32 link = gabi::ea(dComIfGp_getLinkPlayer());
    return bwdExecHookshotReturn(link);
}
static void bgmStart(u32 n) { bwdExecBgmStart(n); }
static void monsterSoundStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (!gabi::ea(&a->eyePos))
        return;
    u32 actorID = fopAcM_GetID(a);
    s32 rev = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    bwdExecMonsterSound(id, &a->eyePos, actorID, param, rev);
}
static void setCameraType(const char* key, void* a) {
    bwdExecCameraType(bwdExecCameraBody(), key, a);
}
static void setEnvironmentColor(u8 a, u8 b, f32 f) { bwdExecColorSet(a, b, f); }

static void stackXrotM(s16 a) { cMtx_XrotM(mDoMtx_stack_c::get(), a); }
static void stackZrotM(s16 a) { cMtx_ZrotM(mDoMtx_stack_c::get(), a); }
static void addCalcPos2(cXyz* a, cXyz* b, f32 rate, f32 step) {
    bwdExecAddCalcPos(a, b, rate, step);
}
static void damage_check(bwd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();
    bool scanBody = i_this->mTongueNoHitTimer == 0;
    if (i_this->mTongueNoHitTimer == 0) {
        gabi::Local<CcAtInfo_bd> atInfo;
        atInfo->pParticlePos = 0;
        if (auto* tracked = fopAcM_SearchByID(i_this->mAttId))
            tracked->current.pos.copy(actor->eyePos);
        i_this->mTongueSph.SetTgType(i_this->mTonguePulledTimer != 0 ? 0xFF1DFEFFu : 0x8000u);
        if (i_this->mTongueSph.ChkTgHit()) {
            i_this->mTongueNoHitTimer = 6;
            atInfo->mpObj = gabi::ea(i_this->mTongueSph.GetTgHitObj());
            if (atInfo->mpObj != 0) {
                atInfo->pParticlePos = gabi::ea(i_this->mTongueSph.GetTgHitPosP());
                if (gabi::at<dCcD_GObjInf>(atInfo->mpObj)->mAtType & 0x8000) {
                    i_this->mTongueHooked = 1;
                    u32 stts = gabi::ea(gabi::at<dCcD_GObjInf>(atInfo->mpObj)->mStts.get());
                    u32 owner = stts ? gabi::ea(gabi::at<dCcD_Stts>(stts)->GetActor()) : 0;
                    atInfo->mpActor = owner;
                    i_this->mHitOwnerId = owner ? fopAcM_GetID(gabi::at<fopAc_ac_c>(owner)) : 0xFFFFFFFFu;
                    i_this->mAction = 3;
                    i_this->mMode = 0;
                    if (actor != nullptr)
                        bd_seStart(actor, 0x286F, 0x20);
                } else if (i_this->mTonguePulledTimer != 0) {
                    s8 saved = actor->health;
                    actor->health = 20;
                    cc_at_check(actor, atInfo.get());
                    actor->health = saved - 1;
                    monsterSoundStart(actor, 0x48DE, 0);
                    i_this->mTongueHitCount++;
                    i_this->mTongueHitShakeTimer = 20;
                }
            }
        }
    }
    if (scanBody) {
        for (int i = 0; i < 9; i++)
            if (i_this->mBodySph[i].ChkTgHit()) {
                def_se_set(actor, i_this->mBodySph[i].GetTgHitObj(), 0x40);
                return;
            }
    }
}

static void control1(bwd_class* i_this, dBgS_GndChk* sharedGround) {
    fopAc_ac_c* actor = i_this;
    f32 dVar8;
    f32 dVar10;
    gabi::Local<cXyz> local_104;
    gabi::Local<cXyz> local_110;
    gabi::Local<cXyz> local_11c;
    gabi::Local<cXyz> local_128;

    PSMTXCopy(bd_getAnmMtx(i_this->mpHeadMorf->getModel(), bwd_execute_detail::BWD_JNT_SITA_e),
              calc_mtx());
    local_104->set(REG0_F(2) + -50.0f, REG0_F(3), REG0_F(4));
    MtxPosition(local_104.get(), &i_this->mTongueSegments[0].mPos);
    int i = 1;
    sita_s* sita_i = &i_this->mTongueSegments[1];
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    cMtx_XrotM(calc_mtx(), actor->current.angle.x);
    local_104->x = 0.0f;
    local_104->y = 0.0f;
    local_104->z = i_this->mTongueSegStep;
    MtxPosition(local_104.get(), local_11c.get());
    local_104->z = i_this->mTongueSegLength;
    f32 dVar9 = i_this->mTongueWaveAmplitude;
    local_128->set(0, 0, 0);
    dBgS_GndChk_ct(sharedGround, BD_GNDCHK_VT, false);
    for (i = 1; i < 0x1d; i++, ++sita_i) {
        if (i_this->mTongueHooked == 0) {
            // HD: the segment phase subtracts its index in each tongue wave.
            if (i_this->mTongueHitShakeTimer != 0) {
                dVar9 = i_this->mTongueHitShakeTimer * (REG0_F(3) + 4.5f);
                local_110->x =
                    dVar9 * cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 0x1fa4) - i * (REG0_S(6) + 0xa28));
                local_110->y = (dVar9 * cM_scos(i_this->mFrameCounter * (REG0_S(5) + 0x2198) -
                                                i * (REG0_S(6) + 0x898)));
                local_110->z =
                    dVar9 * cM_scos(i_this->mFrameCounter * (REG0_S(5) + 0x206c) - i * (REG0_S(6) + 0x960));
            } else {
                local_110->x =
                    dVar9 * cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 0x44c) - i * (REG0_S(6) + 2000));
                if (i_this->mTonguePulledTimer != 0) {
                    local_110->y = -5.0f;
                } else {
                    local_110->y = REG0_F(5) + 5.0f;
                }
                local_110->z =
                    dVar9 * cM_scos(i_this->mFrameCounter * (REG0_S(7) + 900) - i * (REG0_S(8) + 2000));
            }
            MtxPosition(local_110.get(), local_128.get());
        }
        dVar10 = (local_128->y + (sita_i->mPos.y + local_11c->y));
        if (local_110->y < 0.0f) {
            gabi::Local<cXyz> pos;
            pos->copy(sita_i->mPos);
            pos->y += 200.0f;
            sharedGround->SetPos(pos.get());
            f32 ground = cBgS_GroundCross(dComIfG_Bgsp(), sharedGround);
            // HD: the minimum ground clearance is 80; preserve the unordered clamp.
            dVar8 = (ground + REG0_F(16)) + 80.0f;
            if (!(dVar10 > dVar8)) {
                dVar10 = dVar8;
            }
        }
        f32 x = ((sita_i->mPos.x - sita_i[-1].mPos.x) + local_11c->x) + local_128->x;
        f32 y = (dVar10 - sita_i[-1].mPos.y);
        f32 z = ((sita_i->mPos.z - sita_i[-1].mPos.z) + local_11c->z) + local_128->z;
        s16 iVar3;
        int iVar2 = cM_atan2s(x, z);
        iVar3 = -cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        bwdExecMtxPush();
        cMtx_YrotS(calc_mtx(), iVar2);
        cMtx_XrotM(calc_mtx(), iVar3);
        MtxPosition(local_104.get(), local_110.get());
        bwdExecMtxPull();
        {
            gabi::Local<cXyz> added;
            bwdExecVectorAdd(&sita_i[-1].mPos, added.get(), local_110.get());
            sita_i->mPos.copy(*added.get());
        }
    }
    bd_GndChk_dt(sharedGround);
}

/* 0000527C-00005438       .text control2__FP9bwd_class */
static void control2(bwd_class* i_this) {
    gabi::Local<cXyz> rel_offset;
    gabi::Local<cXyz> abs_offset;
    rel_offset->x = 0;
    rel_offset->y = 0;
    rel_offset->z = i_this->mTongueSegLength;
    i_this->mTongueSegments[29].mPos.copy(i_this->mTongueTipPos);

    int i = 28;
    s16 Yangle;
    int XZangle;
    sita_s* sita_i = &i_this->mTongueSegments[28];
    for (i = 28; i >= 1; i--, sita_i--) {
        f32 delta_pos_x = sita_i->mPos.x - sita_i[1].mPos.x;
        f32 delta_pos_y = sita_i->mPos.y - sita_i[1].mPos.y;
        f32 delta_pos_z = sita_i->mPos.z - sita_i[1].mPos.z;
        XZangle = cM_atan2s(delta_pos_x, delta_pos_z);
        Yangle = -cM_atan2s(delta_pos_y, std_sqrtf(gabi::fmadds(delta_pos_x, delta_pos_x,
                                                                delta_pos_z * delta_pos_z)));
        cMtx_YrotS(calc_mtx(), XZangle);
        cMtx_XrotM(calc_mtx(), Yangle);
        MtxPosition(rel_offset.get(), abs_offset.get());
        {
            gabi::Local<cXyz> added;
            bwdExecVectorAdd(&sita_i[1].mPos, added.get(), abs_offset.get());
            sita_i->mPos.copy(*added.get());
        }
    }
}

/* 00005438-00005D78       .text sita_move__FP9bwd_class */
static void sita_move(bwd_class* i_this, fopAc_ac_c* player) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* pfVar2;
    f32 dVar7;
    f32 dVar8;
    f32 dVar9;
    f32 fVar10;
    gabi::Local<cXyz> local_c8;
    gabi::Local<cXyz> local_d4;
    gabi::Local<cXyz> local_f8;
    gabi::Local<dBgS_GndChk> sharedGround;

    if (i_this->mTongueHitShakeTimer != 0) {
        i_this->mTongueHitShakeTimer--;
    }
    local_c8->x = (REG0_F(13) + 100.0f) * cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 0x44c));
    local_c8->y = gabi::fmadds(cM_ssin(i_this->mFrameCounter * (REG0_S(4) + 700)), REG0_F(14) + 50.0f,
                               REG0_F(0) + 600.0f);
    local_c8->z = (REG0_F(13) + 100.0f) * cM_ssin(i_this->mFrameCounter * (REG0_S(7) + 900));
    bwdExecVectorAdd(&i_this->mTongueSegments[0].mPos, local_f8.get(), local_c8.get());
    if (i_this->mTongueTipPos.y < i_this->mSandSurfacePos.y) {
        actor->attention_info.flags = 0;
    }
    dVar8 = 0.1f;
    dVar9 = ((REG0_F(7) + 100.0f) * i_this->mTongueTipStepScale);
    if (i_this->mTongueHooked != 0) {
        pfVar2 = fopAcM_SearchByID(i_this->mHitOwnerId);
        if ((pfVar2 == NULL) || (!hookshotReturnCheck(player))) {
            i_this->mTongueHooked = 0;
        } else {
            i_this->mTongueNoHitTimer = 5;
            local_f8->copy(pfVar2->current.pos);
            dVar8 = 1.0f;
            dVar9 = 200.0f;
            if (i_this->mTongueSegLength > REG0_F(2) + 50.0f) {
                i_this->mTongueHooked = 0;
            }
        }
        i_this->mTongueTipStepScale = 0.0f;
        i_this->mTonguePulledTimer = (s8)l_HIO.mTonguePulledTime;
        i_this->mTonguePulledPos.copy(i_this->mTongueTipPos);
    } else {
        if (i_this->mTonguePulledTimer != 0) {
            i_this->mTonguePulledTimer--;
            f32 dx = actor->current.pos.x - i_this->mTonguePulledPos.x;
            f32 dz = actor->current.pos.z - i_this->mTonguePulledPos.z;
            f32 reach = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            // HD: constrain the tether reach, retaining the fused distance calculation.
            f32 maxReach = REG10_F(10) + 1200.0f;
            if (reach > maxReach) {
                gabi::Local<cXyz> tether;
                tether->copy(actor->current.pos);
                tether->y = i_this->mTonguePulledPos.y;
                addCalcPos2(&i_this->mTonguePulledPos, tether.get(), 1.0f, (reach - maxReach) * 0.05f);
                i_this->mTongueTipPos.copy(i_this->mTonguePulledPos);
            }
            dBgS_GndChk_ct(sharedGround.get(), BD_GNDCHK_VT, false);
            gabi::Local<cXyz> pos;
            pos->copy(i_this->mTongueTipPos);
            pos->y += 200.0f;
            sharedGround->SetPos(pos.get());
            f32 tongueGround = cBgS_GroundCross(dComIfG_Bgsp(), sharedGround.get());
            fVar10 = (tongueGround + REG0_F(16)) + 30.0f;
            if (fVar10 != -1.0e9f) {
                cLib_addCalc2(&i_this->mTongueTipPos.y, fVar10, 0.1f, REG0_F(11) + 2.0f);
            }
            if (i_this->mTongueHitShakeTimer != 0) {
                f32 f1 = i_this->mTongueHitShakeTimer * (REG0_F(3) + 3.0f);
                local_d4->x = (f1 * cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 0x1fa4)));
                local_d4->y = (f1 * cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 0x2260)));
                local_d4->z = (f1 * cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 0x206c)));
                dVar8 = 1.0f;
                dVar9 = f1;
                bwdExecVectorAdd(&i_this->mTonguePulledPos, local_f8.get(), local_d4.get());
            } else {
                local_f8->copy(i_this->mTonguePulledPos);
            }
            bd_GndChk_dt(sharedGround.get());
        } else {
            cLib_addCalc2(&i_this->mTongueTipStepScale, 1.0f, 1.0f, 0.01f);
        }
        if (i_this->mTongueThrashTimer != 0) {
            i_this->mTongueThrashTimer--;
            local_c8->x = (REG0_F(17) + 400.0f) * cM_ssin((int)i_this->mFrameCounter * (REG0_S(5) + 0xc1c));
            local_c8->y = 400.0f;
            local_c8->z =
                (REG0_F(17) + 400.0f) * cM_ssin((int)i_this->mFrameCounter * (REG0_S(7) + 0x1324));
            bwdExecVectorAdd(&i_this->mTongueSegments[0].mPos, local_f8.get(), local_c8.get());
            dVar8 = 1.0f;
            dVar9 = 200.0f;
        }
    }
    bwdExecVectorSubtract(&i_this->mTongueTipPos, local_c8.get(), &i_this->mTongueSegments[0].mPos);
    fVar10 = std_sqrtf(PSVECSquareMag(local_c8.get()));
    dVar7 = gabi::fmuls_ppc(
        fVar10, REG0_F(2) + 0.035f); /* fmuls f22,f1,f8: the length is frA (NaN payload order) */
    if (i_this->mTongueRetract != 0) {
        local_f8->copy(i_this->mTongueSegments[0].mPos);
        dVar8 = 1.0f;
        dVar9 = 50.0f;
        if (i_this->mTongueRetract != 2) {
            actor->attention_info.flags = 0;
        }
        if (dVar7 < (REG0_F(1) + 20.0f)) {
            dVar7 = (REG0_F(1) + 20.0f);
        }
    }
    cLib_addCalc2(&i_this->mTongueTipPos.x, local_f8->x, dVar8, dVar9);
    cLib_addCalc2(&i_this->mTongueTipPos.y, local_f8->y, dVar8, dVar9);
    cLib_addCalc2(&i_this->mTongueTipPos.z, local_f8->z, dVar8, dVar9);
    cLib_addCalc2(&i_this->mTongueSegLength, dVar7, 1.0f, 2.0f);
    i_this->mTongueWaveAmplitude = REG0_F(3) + 10.0f;
    control1(i_this, sharedGround.get());
    control2(i_this);
    sita_s* sita_i = i_this->mTongueSegments;
    for (int i = 0; i < 30; i++, ++sita_i) {
        MtxTrans(sita_i->mPos.x, sita_i->mPos.y, sita_i->mPos.z, false);
        fVar10 =
            gabi::fmadds(cM_scos((int)i_this->mFrameCounter * (REG0_S(7) + 1000) + i * (REG0_S(8) + 2000)),
                         (0.2f + REG0_F(9)), (1.0f + REG0_F(9)));
        MtxScale(fVar10, fVar10, fVar10, true);
        J3DModel_setBaseTRMtx(sita_i->mpModel, calc_mtx());
        if (i == 29) {
            actor->eyePos = sita_i->mPos;
            actor->attention_info.position.copy(actor->eyePos);
            actor->attention_info.position.y += 20.0f;
            i_this->mTongueSph.SetC(&sita_i->mPos);
            i_this->mTongueSph.SetR(REG10_F(15) + 100.0f);
            bwdExecRegisterCollider(dComIfG_Ccsp(), &i_this->mTongueSph);
        } else if ((((i == 0) || (i == 6)) || (i == 12)) || (i == 18 || (i == 24))) {
            int r0 = i / 6;
            i_this->mTongueCoSph[r0].SetC(&sita_i->mPos);
            bwdExecRegisterCollider(dComIfG_Ccsp(), &i_this->mTongueCoSph[r0]);
        }
    }
}

static void bwd_kankyo(bwd_class* i_this) {
    if (i_this->mKankyoTimer != 0) {
        i_this->mKankyoTimer--;
    }
    switch (i_this->mKankyoState) {
    case 0:
        setEnvironmentColor(0, 1, 0.0f);
        break;
    case 1:
        setEnvironmentColor(0, 1, i_this->mEnvColorRate);
        cLib_addCalc2(&i_this->mEnvColorRate, 1.0f, 1.0f, 0.01f);
        break;
    case 2:
        setEnvironmentColor(1, 2, i_this->mEnvColorRate);
        cLib_addCalc2(&i_this->mEnvColorRate, 1.0f, 1.0f, 0.05f);
        if (i_this->mKankyoTimer == 0) {
            if (i_this->mAction == 0xc) {
                i_this->mKankyoState = 4;
            } else {
                i_this->mKankyoState = 3;
            }
        }
        break;
    case 3:
        setEnvironmentColor(1, 2, i_this->mEnvColorRate);
        cLib_addCalc0(&i_this->mEnvColorRate, 1.0f, 0.02f);
        break;
    case 4:
        setEnvironmentColor(3, 2, i_this->mEnvColorRate);
        cLib_addCalc0(&i_this->mEnvColorRate, 1.0f, 0.02f);
        break;
    case 5:
        setEnvironmentColor(3, 4, i_this->mEnvColorRate);
        cLib_addCalc2(&i_this->mEnvColorRate, 1.0f, 1.0f, 0.02f);
        break;
    }
}

/* 000074A4-00008C24       .text daBwd_Execute__FP9bwd_class */
/* 020FA614 */
static BOOL bwd_execute(bwd_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    int i;
    int j;
    fopAc_ac_c* actor = i_this;
    J3DAnmTextureSRTKey* pBtk;
    J3DModel* model;
    f32 dVar24;
    f32 fVar26;
    gabi::Local<cXyz> local_110;
    gabi::Local<cXyz> cStack_11c;
    gabi::Local<cXyz> local_128;

    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    if (auto* tracked = fopAcM_SearchByID(i_this->mAttId))
        tracked->current.pos.y = 10000.0f;
    if (dComIfGp_getStartStageName0() == 'X') {
        i_this->mBlurRate = 0x32;
    }
    executeMatrixPointer = &executeMatrix;
    if (i_this->mCamTypeState != 0) {
        if (i_this->mCamTypeState == 2) {
            setCameraType("Boss03", nullptr);
            i_this->mCamTypeState = 1;
        } else {
            setCameraType("WindBoss", nullptr);
        }
    }
    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk.get(), BD_GNDCHK_VT, false);
    gabi::Local<cXyz> pos;
    pos->copy(actor->current.pos);
    pos->y += REG0_F(13) + 5500.0f;
    gndChk->SetPos(pos.get());
    fVar26 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
    if ((fVar26 < 850.0f) && (fVar26 != -1.0e9f)) {
        i_this->mSandSurfacePos.y = fVar26;
    }
    i_this->mTongueRetract = 0;
    ko_count = 0;
    for (int i = 0; i < l_HIO.mLarvaMax; i++) {
        ko_ac()[i] = 0;
    }
    bwdExecSearch(0x020F9A28, i_this);
    if (i_this->mBgmStartTimer > 1) {
        i_this->mBgmStartTimer--;
    }
    if ((ko_count == 0) && (i_this->mBgmStartTimer == 1)) {
        if (dComIfGp_getStartStageName0() == 'X') {
            bgmStart(0x8000004C);
        } else {
            bgmStart(0x80000029);
        }
        i_this->mBgmStartTimer = 0;
    }
    bwdExecVectorSubtract(&player->current.pos, local_110.get(), &actor->current.pos);
    local_110->y = 0.0f;
    i_this->mPlayerDistXZ = std_sqrtf(PSVECSquareMag(local_110.get()));
    i_this->mDrawBody = 0;
    i_this->mFrameCounter++;
    for (int i = 0; i < 5; i++) {
        auto* timer = &reinterpret_cast<bwd_execute_detail::Timers*>(i_this)->mTimers[i];
        if (*timer != 0) {
            *timer = (s16)*timer - 1;
        }
    }
    if (i_this->mTongueNoHitTimer != 0) {
        i_this->mTongueNoHitTimer--;
    }
    if (i_this->mSandCrossWait != 0) {
        i_this->mSandCrossWait--;
    }
    if (l_HIO.mNoMove == 0) {
        bwdExecMove(i_this);
        i_this->mpHeadMorf->play(&actor->eyePos, 0, 0);
        local_110->x = 0.0f;
        local_110->y = 0.0f;
        local_110->z = REG10_F(19) - 270.0f;
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        cMtx_XrotM(calc_mtx(), actor->current.angle.x);
        MtxPosition(local_110.get(), cStack_11c.get());
        bwd_execute_detail::historyAngles(i_this, (u32)i_this->mHistoryIndex)->y = actor->current.angle.y;
        bwd_execute_detail::historyAngles(i_this, (u32)i_this->mHistoryIndex)->x = actor->current.angle.x;
        // HD: no m18F0 angle offset; the roll amplitude is 4000 rather than 5000.
        f32 rollAmount = REG10_F(15) + 4000.0f;
        s16 phase = i_this->mRollPhase;
        s16 rollOffset = (s16)gabi::ftoi(cM_ssin(phase) * rollAmount);
        i_this->mRollPhase = phase + REG0_S(7) + 1000;
        bwd_execute_detail::historyAngles(i_this, (u32)i_this->mHistoryIndex)->z =
            actor->current.angle.z + rollOffset;
        bwdExecVectorAdd(&actor->current.pos, local_110.get(), cStack_11c.get());
        bwd_execute_detail::historyPosition(i_this, (u32)i_this->mHistoryIndex)->copy(*local_110.get());

        if (i_this->mDrawBody != 0) {
            s32 angleLag = REG10_S(7) + 10;
            for (int i = 0; i < 20; i++) {
                if (i_this->mpBodyModel[i] != 0) {
                    u32 uVar18 = (u32)i_this->mHistoryIndex - (u32)(i + 1) * (u32)i_this->mHistoryStep;
                    uVar18 = uVar18 & 0xFF;
                    int angleIndex = (uVar18 + angleLag + REG10_S(6) - 8) & 0xFF;
                    if (angleLag != 0)
                        angleLag -= REG10_S(8) + 2;
                    mDoMtx_stack_c::transS(bwd_execute_detail::historyPosition(i_this, uVar18)->x,
                                           bwd_execute_detail::historyPosition(i_this, uVar18)->y,
                                           bwd_execute_detail::historyPosition(i_this, uVar18)->z);
                    mDoMtx_stack_c::YrotM(
                        bwd_execute_detail::historyAngles(i_this, (u32)angleIndex)->y);
                    stackXrotM(bwd_execute_detail::historyAngles(i_this, (u32)angleIndex)->x);
                    stackZrotM(bwd_execute_detail::historyAngles(i_this, (u32)angleIndex)->z);
                    fVar26 = gabi::fmadds(-(f32)i, 0.02f, REG0_F(2) + 1.1f);
                    fVar26 *= i_this->m03C4b[i] + 1.0f;
                    mDoMtx_stack_c::scaleM(fVar26, fVar26, 1.0f);
                    J3DModel_setBaseTRMtx(i_this->mpBodyModel[i], mDoMtx_stack_c::get());
                    PSMTXCopy(mDoMtx_stack_c::get(), calc_mtx());
                    local_110->set(REG0_F(13), REG0_F(14), REG0_F(15));
                    MtxPosition(local_110.get(), &i_this->mBodyPos[i]);
                    if (((i & 1) == 0) && REG10_S(1) == 0 && (i_this->mBodyAnmSpeed[i] < 0.1f)) {
                        dComIfGp_particle_setSimple(0x8241, &i_this->mBodyPos[i], 0xff, &particleWhite,
                                                    &particleWhite, 0);
                    }
                    i_this->mpBodyMorf[i]->setPlaySpeed(i_this->mBodyAnmSpeed[i]);
                    mDoExt_baseAnm_play(i_this->mpBodyMorf[i]);
                }
            }
        }
        if (actor->speedF > 1.0f) {
            i_this->mHistoryIndex = (u32)i_this->mHistoryIndex + 1u;
            i_this->mHistoryIndex = i_this->mHistoryIndex & 0xFF;
        }
    }
    actor->shape_angle.x = actor->current.angle.x;
    actor->shape_angle.y = actor->current.angle.y;
    actor->shape_angle.z = actor->current.angle.z;
    model = i_this->mpHeadMorf->getModel();
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    stackXrotM(actor->shape_angle.x);
    stackZrotM(actor->shape_angle.z);
    if (((i_this->mAction == 10) || (i_this->mAction == 0xb)) ||
        ((i_this->mAction == 0xc) && (i_this->mMode >= 2))) {
        fVar26 = 1.0f;
    } else {
        fVar26 = l_HIO.mBodyScale;
    }
    mDoMtx_stack_c::scaleM(fVar26, fVar26, fVar26);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    i_this->mpHeadMorf->calc();
    sita_move(i_this, player);
    damage_check(i_this);
    if (jointOffsetsInitialized == 0) {
        for (int k = 0; k < 9; ++k) {
            jointOffsets.mPosition[k].set(k < 6 ? 100.0f : 0.0f, 0.0f,
                                          k < 3 ? 100.0f : (k < 6 ? -100.0f : 0.0f));
        }
        jointOffsetsInitialized = 1;
    }
    for (int i = 0; i < 9; i++) {
        PSMTXCopy(bd_getAnmMtx(i_this->mpHeadMorf->getModel(), jno[i]), calc_mtx());
        local_128->copy(jointOffsets.mPosition[i]);
        MtxPosition(local_128.get(), cStack_11c.get());
        i_this->mBodySph[i].SetC(cStack_11c.get());
        f32 radius = jsize[i];
        if (i_this->mAction == 10) {
            i_this->mBodySph[i].SetR(radius);
        } else {
            i_this->mBodySph[i].SetR(l_HIO.mBodyScale * radius);
        }
        bwdExecRegisterCollider(dComIfG_Ccsp(), &i_this->mBodySph[i]);
    }
    bwdExecDemoCamera(i_this);
    if ((i_this->mPushPlayer != 0) && (player->current.pos.y < 900.0f)) {
        bwdExecVectorSubtract(&i_this->mSandDentCenter, local_110.get(), &player->current.pos);
        local_110->y = 0.0f;
        f32 powerLength = std_sqrtf(PSVECSquareMag(local_110.get()));
        dVar24 = gabi::fmadds(-powerLength, REG0_F(4) + 0.00025f, 1.0f);
        if (!(dVar24 >= 0.0f)) {
            dVar24 = 0.0f;
        }
        auto* powerVtable = reinterpret_cast<bwd_execute_detail::Player*>(player)->mVtable.get();
        s16 powerAngle = cM_atan2s(local_110->x, local_110->z);
        bwd_execute_detail::setOutPower(powerVtable, player, l_HIO.mPushPower * dVar24, powerAngle);
        i_this->mPushPlayer = 0;
    }
    i_this->mpHeadBrkAnm->setPlaySpeed(i_this->mHeadBrkSpeed);
    mDoExt_baseAnm_play(i_this->mpHeadBrkAnm);
    model = i_this->mpTriforcePlatformModel;
    if (i_this->mPlatformShakeTimer != 0) {
        i_this->mPlatformShakeTimer--;
        i_this->mPlatformShakeOffset.x = cM_rndFX(REG0_F(18) + 4.0f);
        i_this->mPlatformShakeOffset.x = cM_rndFX(REG0_F(18) + 4.0f);
    }
    mDoMtx_stack_c::transS(i_this->mPlatformShakeOffset.x, l_HIO.mBaseHeight + (i_this->mPlatformShakeOffset.y + l_HIO.mPlatformHeight),
                           i_this->mPlatformShakeOffset.z);
    if (i_this->mPlatformHidden != 0) {
        mDoMtx_stack_c::transM(0.0f, -20000.0f, 0.0f);
    }
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mBgwMtx2);
    dBgW_Move(i_this->mpBgW2);
    for (int i = 0; i < 2; i++) {
        if (i_this->mSandfallState[i] != 0) {
            dBgW_Move(i_this->mpBgW1[i]);
            fVar26 = i_this->mSandBaseY + 50.0f + REG0_F(8);
            i_this->m3B3C = 0.0f;
            i_this->m3B40 = fVar26;
            i_this->m3B44 = 0.0f;
            i_this->mSandfallAngle[i].y = REG0_S(i + 0x6) + demo_ang[i];
            switch (i_this->mSandfallState[i]) {
            case 3:
                break;
            case 1:
                i_this->mSandfallTimer[i] = 0;
                i_this->mSandfallState[i] = 2;
                // fallthrough
            case 2:
                if (i_this->mSandfallTimer[i] == 0x32) {
                    for (int j = 0; j < 3; j++) {
                        int idx = j + (i * 3);
                        suna_gr_ang()[idx].x = suna_gr_ang()[idx].z = 0;
                        suna_gr_ang()[idx].y =
                            cM_atan2s(-suna_gr_pos()[idx].x, -suna_gr_pos()[idx].z);
                        dComIfGp_particle_setToon(
                            0xA260, &suna_gr_pos()[idx], &suna_gr_ang()[idx], NULL,
                            reinterpret_cast<GXColor*>(gabi::at<void>(eff_col))->a,
                            &i_this->mSandfallSmokeCb[idx], (s8)actor->current.roomNo);
                        smoke_setColor(&i_this->mSandfallSmokeCb[idx], eff_col);
                    }
                }
                if (i_this->mSandfallTimer[i] == 0xb4) {
                    model = i_this->mpSandfallModel[i];
                    pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(
                        "Bwd", bwd_execute_detail::dRes_INDEX_BWD_BTK_TAKI_WAIT_e, SAFESTRING_VTBL);
                    i_this->mpSandfallBtk[i]->init(J3DModel_getModelData(model), pBtk, true,
                                           J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, true, 0);
                    i_this->mSandfallState[i] = 3;
                }
                break;
            case 4:
                model = i_this->mpSandfallModel[i];
                pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(
                    "Bwd", bwd_execute_detail::dRes_INDEX_BWD_BTK_TAKI_END_e, SAFESTRING_VTBL);
                if (i_this->mpSandfallBtk[i]->init(J3DModel_getModelData(model), pBtk, true,
                                           J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, true, 0) == 1) {
                    i_this->mSandfallState[i] = 5;
                    i_this->mSandfallTimer[i] = 0;
                }
                break;
            case 5:
                if (i_this->mSandfallTimer[i] >= 0x3C) {
                    i_this->mSandfallState[i] = 0;
                    for (int j = 0; j < 3; j++) {
                        bwdExecRemoveSmoke(&i_this->mSandfallSmokeCb[j + (i * 3)]);
                    }
                }
                break;
            }
            i_this->mSandfallTimer[i]++;
            i_this->mpSandfallBtk[i]->play();
            model = i_this->mpSandfallModel[i];
            mDoMtx_stack_c::transS(0.0f, REG0_F(3) + -500.0f + l_HIO.mBaseHeight, 0.0f);
            mDoMtx_stack_c::YrotM(i_this->mSandfallAngle[i].y);
            J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
            PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mBgwMtx1[i]);
            if (((i_this->mSandfallState[i] == 2) && (i_this->mSandfallTimer[i] >= 0x32)) ||
                (i_this->mSandfallState[i] == 3)) {
                model = NULL;
                for (j = 0; j < 3; j++) {
                    mDoAud_seStart(0x7049, &suna_gr_pos()[j + (i * 3)], 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
            }
        }
    }
    for (i = 0; i < 6; i++) {
        pos->copy(suna_gr_pos()[i]);
        pos->y += (f32)(1000.0f + REG0_F(13));
        gndChk->SetPos(pos.get());
        fVar26 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
        if (fVar26 != -1.0e9f) {
            suna_gr_pos()[i].y = fVar26;
        }
    }
    if (i_this->mFaceSandState != 0) {
        if (i_this->mFaceSandState == 1) {
            i_this->mFaceSandState++;
            bwdExecRemoveFollow(&i_this->mFaceSandCb);
            dComIfGp_particle_set(0x824D, &i_this->mSandSurfacePos, &actor->shape_angle, NULL, 0xff,
                                  (dPa_levelEcallBack*)&i_this->mFaceSandCb, (s8)actor->current.roomNo);
        } else if (i_this->mFaceSandState < 0) {
            i_this->mFaceSandState = 0;
            bwdExecRemoveFollow(&i_this->mFaceSandCb);
        } else {
            mDoAud_seStart(0x510E, &i_this->mSandSurfacePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
    }
    for (i = 0; i < 10; i++) {
        // HD: walks ten contiguous timer words, including named fields and padding after mSmokeTimer[2].
        auto& effectTimer = reinterpret_cast<bwd_execute_detail::EffectTimers*>(i_this)->mEffect[i];
        if (effectTimer != 0) {
            effectTimer--;
            if (effectTimer == 0) {
                bwdExecRemoveSmoke(&i_this->mSmokeCb[i]);
                if (i < 2) {
                    bwdExecRemoveFollow(&i_this->mSandFollowCb[i]);
                }
            }
        }
    }
    for (i = 0; i < 2; i++) {
        if (i_this->mSplashFrame[i] != 0) {
            fVar26 = i_this->mSplashFrame[i] - 1.0f;
            i_this->mpGspMorf[i]->setFrame(fVar26);
            i_this->mpGspBtkAnm[i]->setFrame(fVar26);
            i_this->mpGspBrkAnm[i]->setFrame(fVar26);
            mDoMtx_stack_c::transS(i_this->mSplashPos[i].x, i_this->mSplashPos[i].y, i_this->mSplashPos[i].z);
            mDoMtx_stack_c::scaleM(l_HIO.mSplashScaleXZ, l_HIO.mSplashScaleY, l_HIO.mSplashScaleXZ);
            J3DModel_setBaseTRMtx(i_this->mpGspMorf[i]->getModel(), mDoMtx_stack_c::get());
            i_this->mSplashFrame[i]++;
            if (i_this->mSplashFrame[i] > 0x3c) {
                i_this->mSplashFrame[i] = 0;
            }
        }
    }
    if (i_this->mBwdgCreated == 0) {
        i_this->mBwdgCreated = 1;
        fopAcM_create(0xDB, 0, &actor->home.pos, fopAcM_GetRoomNo(actor), NULL, NULL, 0xff, 0);
    }
    if (i_this->mSandWaveMode != 0) {
        if (i_this->mSandWaveMode == 0xFF) {
            cLib_addCalc2(&i_this->mSandDentDepth, REG0_F(5) + 0.35f, 0.2f, REG0_F(6) + 0.1f);
            cLib_addCalcAngleS2(&i_this->mWaveSpeed, REG0_S(9) + -0x5dc, 1, 0x1e);
            cLib_addCalc2(&i_this->mWaveStepRate, 1.0f, 1.0f, 0.1f);
        } else if (i_this->mSandWaveMode == 200) {
            cLib_addCalcAngleS2(&i_this->mWaveSpeed, 0, 1, 0x1e);
        } else if (i_this->mSandWaveMode == 0xC9) {
            i_this->mSandDentDepth = REG0_F(9) + -0.3f;
        } else if (i_this->mSandWaveMode == 0xCA) {
            cLib_addCalc2(&i_this->mSandDentDepth, REG0_F(4) + -0.7f, 0.1f, 0.005f);
            cLib_addCalcAngleS2(&i_this->mWaveSpeed, REG0_S(9) + -500, 1, 0x1e);
            cLib_addCalc2(&i_this->mWaveStepRate, 1.0f, 1.0f, 0.005f);
        } else {
            cLib_addCalc2(&i_this->mSandDentDepth, -0.75f, 0.1f, 0.01f);
            cLib_addCalcAngleS2(&i_this->mWaveSpeed, REG0_S(7) + 1000, 1, 0x1e);
            cLib_addCalc2(&i_this->mWaveStepRate, 1.0f, 1.0f, 0.01f);
            i_this->mSandWaveMode--;
        }
    } else {
        cLib_addCalc0(&i_this->mSandDentDepth, 0.1f, 0.01f);
        cLib_addCalcAngleS2(&i_this->mWaveSpeed, 0, 1, 0x14);
        cLib_addCalc0(&i_this->mWaveStepRate, 1.0f, 0.01f);
    }
    bwd_kankyo(i_this);
    bd_GndChk_dt(gndChk.get());
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py): named external ABI bindings
data executeMatrix 101FF5F0
data executeMatrixPointer 1018C7B0
data particleWhite 101D5E98
data jointOffsets 10462B94
data jointOffsetsInitialized 10462C00
data jno 10192F48
data jsize 10192F6C
data demo_ang 10192F20
str 1000BF90 "Boss03" in 020FA614
str 1000BF98 "WindBoss" in 020FA614
str 1000BF8C "Bwd" in 020FA614
ext 024435FC BOOL bwdExecHookshotReturn(u32)
ext 025E18EC void bwdExecBgmStart(u32)
ext 025E1AA4 void bwdExecMonsterSound(u32, cXyz*, u32, u32, s32)
ext 024F8044 u32 bwdExecCameraBody()
ext 02514EE4 void bwdExecCameraType(u32, const char*, void*)
ext 0255FDA4 void bwdExecColorSet(u8, u8, f32)
ext 0200F164 void bwdExecAddCalcPos(cXyz*, cXyz*, f32, f32)
ext 0200FCF0 void bwdExecMtxPush()
ext 0200FD38 void bwdExecMtxPull()
ext 0201AD78 void bwdExecVectorAdd(cXyz*, cXyz*, cXyz*)
ext 0201ADE0 void bwdExecVectorSubtract(cXyz*, cXyz*, cXyz*)
ext 0200E240 void bwdExecRegisterCollider(cCcS*, void*)
ext 025DE508 void bwdExecSearch(u32, bwd_class*)
ext 020FF80C void bwdExecMove(bwd_class*)
ext 02101F30 void bwdExecDemoCamera(bwd_class*)
ext 025A5F88 void bwdExecRemoveSmoke(void*)
ext 025A5AC8 void bwdExecRemoveFollow(void*)
*/
