/**
 * d_a_bgn_exec.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): daBgn_Execute with the inlined demo_camera.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dCamera_c (camera_process_class + 0x248) */
static inline void bgn_camera_Stop(u32 cam) { gabi::call(0x02514F2C, cam); }
static inline void bgn_camera_Start(u32 cam) { gabi::call(0x02514F38, cam); }
static inline void bgn_camera_SetTrimSize(u32 cam, s32 size) { gabi::call(0x02515280, cam, size); }
/* 02514FE8 dCamera_c::Set(cXyz center, cXyz eye, s16 bank, f32 fovy) (cXyz by value: pointers to copies) */
static inline void bgn_camera_Set(u32 cam, cXyz* center, cXyz* eye, s16 bank, f32 fovy) { gabi::call(0x02514FE8, cam, center, eye, bank, fovy); }
/* daPy_py_c (HD): vtable at +0xB4; setPlayerPosAndAngle(cXyz*, s16) slot 0x114, voiceStart(u32) slot 0xE4;
 * changeOriginalDemo: demo param +0x428 = 0, demo type +0x420 = 3; changeDemoMode: +0x430 */
static inline void bgn_setPlayerPosAndAngle(u32 pl, cXyz* pos, s16 angle) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(pl + 0xB4) + 0x114), pl, pos, angle);
}
/* 025D7B24 fopAcM_orderPotentialEvent(actor, type, flag, param) [as in d_a_kb] */
static inline BOOL bgn_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p) { return gabi::call<BOOL>(0x025D7B24, a, type, flag, p); }
/* 0259169C dMeter_mtrShow */
static inline void bgn_dMeter_mtrShow() { gabi::call(0x0259169C); }
/* 025E1A7C mDoAud_monsSeStart(id, pos, param, reverb) [as in d_a_bgn2] */
static inline void bgn_monsSeStart(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A7C, id, pos, param, reverb); }
/* 027EC9E8 JUTReport(x, y, fmt, ...) (HD keeps the debug reports) */
static inline void bgn_JUTReport(s32 x, s32 y, u32 fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) [as in d_a_npc_zl1] */
static inline void bgn_setNextStage(u32 stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, speed, mode, setPoint, wipe);
}

/* the camera block shared by every cut scene end (GameCube: label block_57) */
static inline void bgn_demo_end(bgn_class* i_this, u32 cam) {
    bgn_camera_SetTrimSize(cam, 0);
    bgn_camera_Start(cam);
    bgn_dMeter_mtrShow();
    dComIfGp_event_reset();
    i_this->mCA60 = 1;
    i_this->mCSMode = 0;
}

/* the rope of the defeat cut scene (GameCube last_himo_control, inlined). HD: the segments are 45 apart as
 * before, but the sway uses this TU's REG registers as on the GameCube */
static inline void bgn_last_himo_control(bgn_class* i_this) {
    i_this->mC450[0].x = -15.0f;
    i_this->mC450[0].y = gabi::fadds_ppc(REG_F(8, 4), 4441.46f);
    i_this->mC450[0].z = gabi::fadds_ppc(REG_F(8, 3), 375.17f);
    u32 lines = i_this->mDefeatCSRopeMat.mpLines;
    u32 pcVar8 = gabi::load<u32>(lines);
    u32 puVar8 = gabi::load<u32>(lines + 4);
    gabi::store<u8>(puVar8, 10);
    u32 c0 = gabi::ea(&i_this->mC450[0]);
    gabi::store<u32>(pcVar8, gabi::load<u32>(c0));
    gabi::store<u32>(pcVar8 + 4, gabi::load<u32>(c0 + 4));
    gabi::store<u32>(pcVar8 + 8, gabi::load<u32>(c0 + 8));
    gabi::Local<cXyz> local_a8;
    gabi::Local<cXyz> local_b4;
    f32 c724;
    if (i_this->mC720 == 1) {
        i_this->mC720 = 2;
        for (s32 i = 0; i < 59; i++) {
            u32 d = gabi::ea(&i_this->mC450[1 + i]);
            gabi::store<u32>(d, gabi::load<u32>(c0));
            gabi::store<u32>(d + 4, gabi::load<u32>(c0 + 4));
            gabi::store<u32>(d + 8, gabi::load<u32>(c0 + 8));
        }
        c724 = gabi::fadds_ppc(REG0_F(19), 5000.0f);
        i_this->mC724 = c724;
    } else {
        cLib_addCalc0(&i_this->mC724, 1.0f, REG0_F(18) + 50.0f);
        c724 = i_this->mC724;
    }
    local_a8->x = 0.0f;
    local_a8->y = 0.0f;
    local_a8->z = gabi::fadds_ppc(REG0_F(12), 45.0f);
    s16 k = 0;
    s16 j = 0;
    cXyz* pcVar5 = &i_this->mC450[1];
    u32 out = pcVar8 + 0xC;
    u32 size = puVar8 + 1;
    for (s32 i = 1; i < 60; i++, pcVar5++, out += 0xC, size++) {
        f32 y = gabi::fadds_ppc(gabi::fsubs_ppc(gabi::fsubs_ppc(pcVar5->y, pcVar5[-1].y), 35.0f), REG0_F(13));
        f32 z = gabi::fsubs_ppc(pcVar5->z, pcVar5[-1].z);
        f32 x = gabi::fsubs_ppc(pcVar5->x, pcVar5[-1].x);
        if (c724 > 0.01f) {
            s16 c746 = i_this->mC746;
            k = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(c746 * 2000 + i * (REG0_S(3) + 2000)), c724));
            j = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_scos(c746 * 0x8FC + i * (REG0_S(4) + 0x9C4)), c724));
        }
        u32 m = gabi::load<u32>(0x1018C7B0);
        s16 a = cM_atan2s(y, z);
        cMtx_XrotS(gabi::at<Mtx34>(m), (s16)(k - a));
        f32 d = gabi::fmadds(y, y, gabi::fmuls_ppc(z, z));
        m = gabi::load<u32>(0x1018C7B0);
        f32 sq = std_sqrtf(d);
        s16 b = cM_atan2s(x, sq);
        cMtx_YrotM(gabi::at<Mtx34>(m), (s16)(b + j));
        MtxPosition(local_a8, local_b4);
        f32 nx = gabi::fadds_ppc(pcVar5[-1].x, local_b4->x);
        pcVar5->x = nx;
        pcVar5->y = gabi::fadds_ppc(pcVar5[-1].y, local_b4->y);
        f32 nz = gabi::fadds_ppc(pcVar5[-1].z, local_b4->z);
        pcVar5->z = nz;
        gabi::store<f32>(out, nx);
        gabi::store<f32>(out + 4, pcVar5->y);
        gabi::store<f32>(out + 8, nz);
        gabi::store<u8>(size, 10);
        c724 = i_this->mC724;
    }
}

/* 0207E7B4 daBgn_Execute (demo_camera and bomb_splash_check inlined). HD: move() is no longer here (the
 * shape update shape_calc is called instead); the transformation and defeat no longer go through
 * l_HIO.m024 (m02B5 is set directly; the final event bit is set when the explosion starts); the first cut
 * scene shows thunder particles around Link at five fixed frames and zooms in from frame REG18_S(6) + 450,
 * Link looks up at frame 313 (GameCube 320); the cut scenes count mCA60 down; two debug reports. */
BOOL daBgn_Execute(bgn_class* i_this) {
    WWHD_FUNC(0x0207E7B4, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    if (i_this->m02B4 == 0xFF)
        return TRUE;
    dComIfGp_get();
    settingTevStruct(dKy_getEnvlight(), 3 /* TEV_TYPE_BG2 */, gabi::at<cXyz>(0x10461CD0) /* center_pos */,
                     gabi::at<dKy_tevstr_c>(bg_tevstr()));
    fpcM_Search(0x0207D3AC /* s_b_sub: bomb_splash_check */, i_this);
    bgn2_g() = gabi::ea(fpcM_Search(0x0207B22C /* bgn2_s_sub */, i_this));
    bgn3_g() = gabi::ea(fpcM_Search(0x0207B27C /* bgn3_s_sub */, i_this));
    if (l_HIO().m027 != 0)
        actor->health = l_HIO().m027;
    if (i_this->mCC90 == 1) {
        i_this->mCC90 = 0;
        i_this->m02B5 = 2;
    }
    i_this->mC746 = (s16)(i_this->mC746 + 1);
    for (s32 i = 0; i < 5; i++) {
        if (i_this->mC7AC[i] != 0)
            i_this->mC7AC[i] = (s16)(i_this->mC7AC[i] - 1);
    }
    if (i_this->mC7B6 != 0)
        i_this->mC7B6 = (s16)(i_this->mC7B6 - 1);
    if (i_this->mC7B8 != 0)
        i_this->mC7B8 = (s16)(i_this->mC7B8 - 1);
    if (i_this->mArrowHitFlashTimer != 0)
        i_this->mArrowHitFlashTimer = (s16)(i_this->mArrowHitFlashTimer - 1);
    if (i_this->m0304 != 0)
        i_this->m0304 = (s16)(i_this->m0304 - 1);
    if (i_this->m0302 != 0) {
        s16 t = (s16)(i_this->m0302 + 1);
        if (t > 100)
            i_this->m0302 = 0;
        else
            i_this->m0302 = t;
    }
    gabi::call(0x0208857C /* shape_calc */, i_this);

    /* demo_camera (inline) */
    u32 player = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER);
    s8 camId = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    u32 cam = gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + camId * 0x34) + 0x248;
    fopAc_ac_c* pl = gabi::at<fopAc_ac_c>(player);
    gabi::Local<cXyz> local_44;
    gabi::Local<cXyz> local_50;
    gabi::Local<cXyz> local_5c;
    bool accepted = true;
    switch (i_this->mCSMode) {
    case 0:
        break;
    case 1:
    case 2:
        if (i_this->mCSMode == 1) {
            if (!eventInfo_checkCommandDemoAccrpt(actor)) {
                accepted = false;
                break;
            }
            i_this->mCSMode = (s8)(i_this->mCSMode + 1);
            bgn_camera_Stop(cam);
            bgn_camera_SetTrimSize(cam, 2);
            i_this->mKSubCount = 0;
            u32 src = gabi::ea(&i_this->mC308);
            u32 dst = gabi::ea(&i_this->mCSCamCenter);
            u32 cy = gabi::load<u32>(src + 4);
            u32 cx = gabi::load<u32>(src);
            i_this->mCSFovY = 55.0f;
            gabi::store<u32>(dst, cx);
            u32 cz = gabi::load<u32>(src + 8);
            gabi::store<u32>(dst + 4, cy);
            gabi::store<u32>(dst + 8, cz);
            i_this->mCA98 = 0.0f;
            gabi::store<u32>(player + 0x428, 0); /* changeOriginalDemo */
            gabi::store<u16>(player + 0x420, 3);
            fpcM_Search(0x0207CF04 /* ki_del_sub */, i_this);
            i_this->mKeeseSpawnNum = 0;
            i_this->mCA60 = 0xB3;
        } else if (i_this->mCA60 > 1) {
            i_this->mCA60 = (s16)(i_this->mCA60 - 1);
        }
        {
            cMtx_YrotS(calc_mtx(), i_this->mC314.y);
            local_44->y = gabi::fadds_ppc(REG0_F(1), 100.0f);
            local_44->x = gabi::fadds_ppc(REG0_F(0), 200.0f);
            local_44->z = gabi::fadds_ppc(REG0_F(2), 2000.0f);
            MtxPosition(local_44, local_50);
            u32 e = gabi::ea(&i_this->mCSCamEye);
            gabi::store<u32>(e, gabi::load<u32>(local_50.a));
            gabi::store<u32>(e + 4, gabi::load<u32>(local_50.a + 4));
            gabi::store<u32>(e + 8, gabi::load<u32>(local_50.a + 8));
            f32 step = i_this->mKSubCount > 300 ? 0.1f : 0.5f;
            cLib_addCalc2(&i_this->mCSCamCenter.x, i_this->mC308.x, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.y, gabi::fadds_ppc(gabi::fadds_ppc(i_this->mC308.y, 100.0f), REG0_F(3)), step, 400.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.z, i_this->mC308.z, 0.1f, 300.0f);
            local_44->x = REG0_F(4);
            local_44->y = pl->current.pos.y;
            local_44->z = gabi::fadds_ppc(REG0_F(5), 1500.0f);
            MtxPosition(local_44, local_50);
            bgn_setPlayerPosAndAngle(player, local_50, (s16)(i_this->mC314.y + 0x8000));
            if (i_this->mKSubCount == (s16)(REG0_S(5) + 220))
                gabi::store<u32>(player + 0x430, 0x1D); /* changeDemoMode(DEMO_UNK_029_e) */
            if (i_this->mKSubCount == (s16)(REG0_S(6) + 313))
                gabi::store<u32>(player + 0x430, 0x1A); /* changeDemoMode(DEMO_LOOKUP_e) */
            s16 n = i_this->mKSubCount;
            if (n == 0xEB || n == 0xFA || n == 0x109 || n == 0x119 || n == 0x129) {
                /* HD: thunder around Link (a function-local static scale, initialised once) */
                if (gabi::load<u32>(0x10461A54) == 0) {
                    gabi::store<u32>(0x10461A54, 1);
                    gabi::store<f32>(0x10461CC4, 0.4f);
                    gabi::store<f32>(0x10461CC8, 0.4f);
                    gabi::store<f32>(0x10461CCC, 0.4f);
                }
                dComIfGp_particle_set(0x40, &pl->current.pos, nullptr, gabi::at<cXyz>(0x10461CC4));
            }
            if (i_this->mKSubCount >= (s16)(REG_S(18, 6) + 450)) {
                cLib_addCalc2(&i_this->mCSFovY, gabi::fadds_ppc(REG_F(18, 11), 35.0f), 0.02f, gabi::fadds_ppc(REG_F(18, 12), 0.1f));
            }
        }
        break;
    case 3: {
        cLib_addCalc2(&i_this->mCSCamCenter.x, gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.x, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.y, gabi::fadds_ppc(gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.y, REG0_F(13)), 0.1f, 200.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.z, gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.z, 0.1f, 100.0f);
        if (i_this->mKSubCount == (s16)(REG0_S(7) + 90))
            i_this->mCA9C = gabi::fadds_ppc(REG0_F(18), 30.0f);
        if (i_this->mKSubCount == (s16)(REG0_S(7) + 100)) {
            u32 vt = gabi::load<u32>(player + 0xB4);
            gabi::store<u32>(player + 0x430, 0x18); /* changeDemoMode(DEMO_SURPRISED_e) */
            gabi::call_ptr(gabi::load<u32>(vt + 0xE4), player, 29); /* voiceStart(29) */
        }
        if (i_this->mKSubCount > (s16)(REG0_S(8) + 110)) {
            cLib_addCalc2(&i_this->mCSFovY, gabi::fadds_ppc(REG0_F(11), 35.0f), 0.1f, gabi::fadds_ppc(REG0_F(12), 0.5f));
            if (i_this->mKSubCount == 130 || i_this->mKSubCount == 180)
                mDoAud_seStart(0x5978 /* JA_SE_CM_BGN_T_MOUSE_OPEN */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            if (i_this->mKSubCount == 152)
                mDoAud_seStart(0x5979 /* JA_SE_CM_BGN_T_MOUSE_CLOSE */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        cLib_addCalcAngleS2(&i_this->mCA60, 1, 1, 1);
        if (i_this->mKSubCount > (s16)(REG0_S(8) + 200)) {
            mDoAud_bgmStart(0x8000005A /* JA_BGM_BGN_TARABA */);
            bgn_demo_end(i_this, cam);
        }
        break;
    }
    case 10:
    case 11:
    case 12: {
        if (i_this->mCSMode == 10) {
            if (!eventInfo_checkCommandDemoAccrpt(actor)) {
                accepted = false;
                break;
            }
            i_this->mCSMode = (s8)(i_this->mCSMode + 1);
            bgn_camera_Stop(cam);
            bgn_camera_SetTrimSize(cam, 2);
            i_this->mKSubCount = 0;
            i_this->mCSFovY = 55.0f;
            i_this->mCA98 = 0.0f;
            u32 src = gabi::ea(&gabi::at<fopAc_ac_c>(bgn2_g())->current.pos);
            u32 dst = gabi::ea(&i_this->mCSCamCenter);
            gabi::store<u32>(dst, gabi::load<u32>(src));
            gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
            gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
            gabi::store<u16>(player + 0x420, 3); /* changeOriginalDemo */
            gabi::store<u32>(player + 0x428, 0);
            fpcM_Search(0x0207CF04 /* ki_del_sub */, i_this);
            gabi::store<u8>(bgn2_g() + 0x33BC, 0); /* bgn2->m2ED0 */
            i_this->mCA74.x = 200.0f;
            i_this->mCA74.y = 2100.0f;
            i_this->mCA60 = 0xB3;
            i_this->mCA74.z = 2500.0f;
        } else if (i_this->mCA60 > 1) {
            i_this->mCA60 = (s16)(i_this->mCA60 - 1);
        }
        cLib_addCalc2(&i_this->mCA74.x, -1800.0f, 0.05f, gabi::fmuls_ppc(2000.0f, i_this->mCA98));
        cLib_addCalc2(&i_this->mCA74.y, 1100.0f, 0.05f, gabi::fmuls_ppc(1000.0f, i_this->mCA98));
        cLib_addCalc2(&i_this->mCA74.z, 1000.0f, 0.05f, gabi::fmuls_ppc(1500.0f, i_this->mCA98));
        cLib_addCalc2(&i_this->mCA98, 0.01f, 1.0f, 0.0001f);
        cMtx_YrotS(calc_mtx(), gabi::at<fopAc_ac_c>(bgn2_g())->shape_angle.y);
        MtxPosition(&i_this->mCA74, local_50);
        u32 e = gabi::ea(&i_this->mCSCamEye);
        gabi::store<u32>(e, gabi::load<u32>(local_50.a));
        gabi::store<u32>(e + 4, gabi::load<u32>(local_50.a + 4));
        gabi::store<u32>(e + 8, gabi::load<u32>(local_50.a + 8));
        if (i_this->mCSMode == 11) {
            cLib_addCalc2(&i_this->mCSCamCenter.x, gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.x, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.y, gabi::fadds_ppc(gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.y, REG0_F(13)), 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.z, gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.z, 0.1f, 300.0f);
            local_44->x = REG0_F(4);
            local_44->y = pl->current.pos.y;
            local_44->z = gabi::fadds_ppc(REG0_F(5), -2000.0f);
            MtxPosition(local_44, local_50);
            u32 vt = gabi::load<u32>(player + 0xB4);
            gabi::call_ptr(gabi::load<u32>(vt + 0x114), player, local_50.get(), (s16)gabi::at<fopAc_ac_c>(bgn2_g())->shape_angle.y);
        } else {
            cLib_addCalc2(&i_this->mCSCamCenter.x, gabi::at<fopAc_ac_c>(bgn3_g())->current.pos.x, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.y,
                          gabi::fadds_ppc(gabi::fadds_ppc(gabi::at<fopAc_ac_c>(bgn3_g())->current.pos.y, 50.0f), REG_F(8, 16)), 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.z, gabi::at<fopAc_ac_c>(bgn3_g())->current.pos.z, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSFovY, gabi::fadds_ppc(REG0_F(17), 30.0f), gabi::fadds_ppc(REG0_F(18), 0.05f),
                          gabi::fadds_ppc(REG0_F(19), 0.2f));
            cLib_addCalcAngleS2(&i_this->mCA60, 1, 1, 2);
        }
        if (i_this->mKSubCount > (s16)(REG0_S(9) + 370))
            bgn_demo_end(i_this, cam);
        break;
    }
    case 20:
    case 21: {
        if (i_this->mCSMode == 20) {
            if (!eventInfo_checkCommandDemoAccrpt(actor)) {
                accepted = false;
                break;
            }
            i_this->mCSMode = (s8)(i_this->mCSMode + 1);
            bgn_camera_Stop(cam);
            bgn_camera_SetTrimSize(cam, 2);
            i_this->mKSubCount = 0;
            i_this->mCSFovY = 55.0f;
            i_this->mCA98 = 0.0f;
            u32 src = gabi::ea(&gabi::at<fopAc_ac_c>(bgn3_g())->current.pos);
            u32 dst = gabi::ea(&i_this->mCSCamCenter);
            gabi::store<u32>(dst, gabi::load<u32>(src));
            gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
            gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
            gabi::store<u16>(player + 0x420, 3); /* changeOriginalDemo */
            gabi::store<u32>(player + 0x428, 0);
            i_this->mCA60 = 0xB4;
            fpcM_Search(0x0207CF04 /* ki_del_sub */, i_this);
            fpcM_Search(0x0207CF58 /* ks_del_sub */, i_this);
        }
        cLib_addCalcAngleS2(&i_this->mCA60, 1, 1, 1);
        i_this->mCSCamEye.x = 500.0f;
        i_this->mCSCamEye.y = gabi::fadds_ppc(REG0_F(3), 500.0f);
        i_this->mCSCamEye.z = gabi::fadds_ppc(REG0_F(4), 2000.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.x, gabi::at<fopAc_ac_c>(bgn3_g())->current.pos.x, 0.1f, 300.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.y, gabi::fadds_ppc(gabi::at<fopAc_ac_c>(bgn3_g())->current.pos.y, REG0_F(13)), 0.1f, 300.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.z, gabi::at<fopAc_ac_c>(bgn3_g())->current.pos.z, 0.1f, 300.0f);
        local_44->x = REG0_F(4);
        local_44->y = pl->current.pos.y;
        local_44->z = gabi::fadds_ppc(REG0_F(5), -1000.0f);
        {
            u32 vt = gabi::load<u32>(player + 0xB4);
            gabi::call_ptr(gabi::load<u32>(vt + 0x114), player, local_44.get(), (s16)gabi::at<fopAc_ac_c>(bgn2_g())->shape_angle.y);
        }
        if (i_this->mKSubCount == 260)
            bgn_monsSeStart(0x4972 /* JA_SE_CV_BGN_DIE */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        if (i_this->mKSubCount >= 370) {
            f32 s = gabi::fadds_ppc(REG0_F(4), 10.0f);
            local_5c->x = s;
            local_5c->z = s;
            local_5c->y = s;
            dComIfGp_particle_set(0x13 /* ID_AK_JN_SIBOUBAKUEN */, &gabi::at<fopAc_ac_c>(bgn3_g())->current.pos, nullptr, local_5c);
            dComIfGp_particle_set(0x16 /* ID_AK_JN_SIBOUFLASH */, &gabi::at<fopAc_ac_c>(bgn3_g())->current.pos, nullptr, local_5c);
            mDoAud_seStart(0x5984 /* JA_SE_CM_BGN_LAST_EXPLODE */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            gabi::Local<cXyz> up;
            dVibration_c* vib = dComIfGp_getVibration();
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            StartShock(vib, REG0_S(2) + 8, -0x21, up);
            i_this->m02B5 = 3;
            dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), 0x3F10);
            s8 mode = (s8)(i_this->mCSMode + 1);
            i_this->mC720 = 1;
            i_this->mCA60 = 0xB4;
            i_this->mCSMode = mode;
        }
        break;
    }
    case 22:
        if (i_this->mKSubCount >= 390)
            bgn_last_himo_control(i_this);
        if (i_this->mKSubCount >= (s16)(REG_S(8, 7) + 500))
            i_this->mCC90 = 2;
        break;
    case 150:
        bgn_demo_end(i_this, cam);
        break;
    default:
        break;
    }
    if (accepted) {
        if (i_this->mCSMode != 0) {
            s16 n = i_this->mKSubCount;
            f32 ca9c = i_this->mCA9C;
            f32 fx = gabi::fmuls_ppc(cM_ssin(n * 0x3300), ca9c);
            f32 fy = gabi::fmuls_ppc(cM_scos(n * 0x3000), ca9c);
            gabi::Local<cXyz> camera_center;
            gabi::Local<cXyz> camera_eye;
            camera_center->x = gabi::fadds_ppc(i_this->mCSCamCenter.x, fx);
            camera_eye->x = gabi::fadds_ppc(i_this->mCSCamEye.x, fx);
            camera_eye->y = gabi::fadds_ppc(i_this->mCSCamEye.y, fy);
            camera_center->y = gabi::fadds_ppc(i_this->mCSCamCenter.y, fy);
            s16 bank = (s16)gabi::ftoi(gabi::fmuls_ppc(gabi::fmuls_ppc(cM_scos(i_this->mC746 * 0x1C00), ca9c), 7.5f));
            f32 fovy = i_this->mCSFovY;
            camera_eye->z = i_this->mCSCamEye.z;
            camera_center->z = i_this->mCSCamCenter.z;
            bgn_camera_Set(cam, camera_center, camera_eye, bank, fovy);
            cLib_addCalc0(&i_this->mCA9C, 1.0f, gabi::fadds_ppc(REG0_F(16), 1.0f));
            bgn_JUTReport(30, 420, 0x10008AF4 /* "K SUB  COUNT  %d" */, i_this->mKSubCount);
            bgn_JUTReport(30, 440, 0x10008B08 /* "K DEMO MODE   %d" */, i_this->mCSMode);
            i_this->mKSubCount = (s16)(i_this->mKSubCount + 1);
        }
    } else {
        bgn_orderPotentialEvent(actor, 2 /* dEvtFlag_STAFF_ALL_e */, 0xFFFF, 0);
        eventInfo_onCondition(actor, 2 /* dEvtCnd_UNK2_e */);
    }

    s8 spawn = i_this->mKeeseSpawnNum;
    if (spawn != 0) {
        i_this->mKeeseSpawnNum = (s8)(spawn - 1);
        if (ki_check(i_this) < l_HIO().mKeeseNumMax) {
            gabi::Local<cXyz> local_28;
            local_28->x = cM_rndFX(2500.0f);
            local_28->y = gabi::fadds_ppc(cM_rndF(500.0f), 3500.0f);
            local_28->x = cM_rndFX(2500.0f); /* GameCube bug kept: z is never set */
            fopAcM_create(0xD7 /* fpcNm_KI_e */, 0xFFFF0003, local_28, fopAcM_GetRoomNo(actor), nullptr, nullptr, -1, 0);
        }
    }
    if (i_this->mCC90 == 2) {
        i_this->mCC90 = 0;
        bgn_setNextStage(0x10008C40 /* "GanonK" */, 4, 0, 9, 0.0f, 0, 1, 0);
    }
    return TRUE;
}
VERIFY(0x0207E7B4, daBgn_Execute);
