/**
 * d_a_bgn_exec.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): daBgn_Execute with the inlined demo_camera.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn_exec.h"

// HD: Link's virtual slots are guest addresses, not host member pointers.
static void bgn_setPlayerPosAndAngle(u32 player, cXyz *pos, s16 angle) {
    auto *link = gabi::at<bgn_exec::Player>(player);
    auto *table = gabi::at<bgn_exec::PlayerVtable>(link->__vtbl);
    gabi::call_ptr(table->setPlayerPosAndAngle, player, pos, angle);
}

/* the camera block shared by every cut scene end (GameCube: label block_57) */
/* inlined into daBgn_Execute */
static inline void bgn_demo_end(bgn_class *i_this, bgn_exec::Camera *cam) {
    bgn_camera_SetTrimSize(cam, 0);
    bgn_camera_Start(cam);
    bgn_dMeter_mtrShow();
    dComIfGp_event_reset();
    i_this->mCA60 = 1;
    i_this->mCSMode = 0;
}

// Defeat cutscene rope; keep the explicit guest word copies and scratch-vector order.
/* inlined into daBgn_Execute */
static inline void last_himo_control(bgn_class *i_this) {
    i_this->mC450[0].x = -15.0f;
    i_this->mC450[0].y = gabi::fadds_ppc(REG_F(8, 4), 4441.46f);
    i_this->mC450[0].z = gabi::fadds_ppc(REG_F(8, 3), 375.17f);
    auto *lines = gabi::at<bgn_exec::Line>(i_this->mDefeatCSRopeMat.mpLines);
    u32 pcVar8 = lines->mpPos; // HD: guest word view retains bit-exact vector copies.
    u32 puVar8 = lines->mpSize;
    *gabi::at<be<u8>>(puVar8) = 10;
    auto *c0 = bgn_exec::words(&i_this->mC450[0]);
    gabi::at<bgn_exec::Words>(pcVar8)->x = (u32)c0->x;
    gabi::at<bgn_exec::Words>(pcVar8)->y = (u32)c0->y;
    gabi::at<bgn_exec::Words>(pcVar8)->z = (u32)c0->z;
    gabi::Local<cXyz> local_a8;
    gabi::Local<cXyz> local_b4;
    f32 c724;
    if (i_this->mC720 == 1) {
        i_this->mC720 = 2;
        for (s32 i = 0; i < 59; i++) {
            auto *d = bgn_exec::words(&i_this->mC450[1 + i]);
            d->x = (u32)c0->x;
            d->y = (u32)c0->y;
            d->z = (u32)c0->z;
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
    cXyz *pcVar5 = &i_this->mC450[1];
    cXyz *out = gabi::at<cXyz>(pcVar8 + sizeof(cXyz));
    be<u8> *size = gabi::at<be<u8>>(puVar8 + 1);
    for (s32 i = 1; i < 60; i++, pcVar5++, out++, size++) {
        f32 y = gabi::fadds_ppc(gabi::fsubs_ppc(gabi::fsubs_ppc(pcVar5->y, pcVar5[-1].y), 35.0f),
                                REG0_F(13));
        f32 z = gabi::fsubs_ppc(pcVar5->z, pcVar5[-1].z);
        f32 x = gabi::fsubs_ppc(pcVar5->x, pcVar5[-1].x);
        if (c724 > 0.01f) {
            s16 c746 = i_this->mC746;
            k = (s16)gabi::ftoi(
                gabi::fmuls_ppc(cM_ssin(c746 * 2000 + i * (REG0_S(3) + 2000)), c724));
            j = (s16)gabi::ftoi(
                gabi::fmuls_ppc(cM_scos(c746 * 0x8FC + i * (REG0_S(4) + 0x9C4)), c724));
        }
        Mtx34 *m = calc_mtx();
        s16 a = cM_atan2s(y, z);
        cMtx_XrotS(m, (s16)(k - a));
        f32 d = gabi::fmadds(y, y, gabi::fmuls_ppc(z, z));
        m = calc_mtx();
        f32 sq = std_sqrtf(d);
        s16 b = cM_atan2s(x, sq);
        cMtx_YrotM(m, (s16)(b + j));
        MtxPosition(local_a8, local_b4);
        f32 nx = gabi::fadds_ppc(pcVar5[-1].x, local_b4->x);
        pcVar5->x = nx;
        pcVar5->y = gabi::fadds_ppc(pcVar5[-1].y, local_b4->y);
        f32 nz = gabi::fadds_ppc(pcVar5[-1].z, local_b4->z);
        pcVar5->z = nz;
        out->x = nx;
        out->y = pcVar5->y;
        out->z = nz;
        *size = 10;
        c724 = i_this->mC724;
    }
}

/* 0207E7B4 */
// HD: demo_camera and bomb_splash_check are inlined; move is an out-of-line call.
// The transformation and defeat no longer go through
// l_HIO.m024 (m02B5 is set directly; the final event bit is set when the explosion starts); the
// first cut scene shows thunder particles around Link at five fixed frames and zooms in from frame
// REG18_S(6) + 450, Link looks up at frame 313 (GameCube 320); the cut scenes count mCA60 down; two
// debug reports.
BOOL daBgn_Execute(bgn_class *i_this) {
    fopAc_ac_c *actor = i_this;
    if (i_this->m02B4 == 0xFF)
        return TRUE;
    dComIfGp_get();
    settingTevStruct(dKy_getEnvlight(), bgn_exec::TEV_TYPE_BG2, bgn_exec::center_pos(),
                     gabi::at<dKy_tevstr_c>(bg_tevstr()));
    fpcM_Search(bgn_exec::s_b_sub, i_this);
    bgn2_g() = gabi::ea(fpcM_Search(bgn_exec::bgn2_s_sub, i_this));
    bgn3_g() = gabi::ea(fpcM_Search(bgn_exec::bgn3_s_sub, i_this));
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
    bgn_exec_move(i_this);

    /* demo_camera (inline) */
    u32 player = gabi::at<bgn_exec::Play>(dComIfGp_ea())->mPlayer;
    s8 camId = dComIfGp_getPlayerCameraID(0);
    auto *cam = &gabi::at<bgn_exec::CameraProcess>(gabi::ea(dComIfGp_getCamera(camId)))->mCamera;
    fopAc_ac_c *pl = gabi::at<fopAc_ac_c>(player);
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
            auto *src = bgn_exec::words(&i_this->mC308);
            auto *dst = bgn_exec::words(&i_this->mCSCamCenter);
            u32 cy = (u32)src->y;
            u32 cx = (u32)src->x;
            i_this->mCSFovY = 55.0f;
            dst->x = cx;
            u32 cz = (u32)src->z;
            dst->y = cy;
            dst->z = cz;
            i_this->mCA98 = 0.0f;
            gabi::at<bgn_exec::Player>(player)->mDemo.mParam0 = 0; /* changeOriginalDemo */
            gabi::at<bgn_exec::Player>(player)->mDemo.mDemoType = bgn_exec::TYPE_ORIGINAL_e;
            fpcM_Search(bgn_exec::ki_del_sub, i_this);
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
            auto *e = bgn_exec::words(&i_this->mCSCamEye);
            e->x = (u32)bgn_exec::words(local_50.get())->x;
            e->y = (u32)bgn_exec::words(local_50.get())->y;
            e->z = (u32)bgn_exec::words(local_50.get())->z;
            f32 step = i_this->mKSubCount > 300 ? 0.1f : 0.5f;
            cLib_addCalc2(&i_this->mCSCamCenter.x, i_this->mC308.x, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.y,
                          gabi::fadds_ppc(gabi::fadds_ppc(i_this->mC308.y, 100.0f), REG0_F(3)),
                          step, 400.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.z, i_this->mC308.z, 0.1f, 300.0f);
            local_44->x = REG0_F(4);
            local_44->y = pl->current.pos.y;
            local_44->z = gabi::fadds_ppc(REG0_F(5), 1500.0f);
            MtxPosition(local_44, local_50);
            bgn_setPlayerPosAndAngle(player, local_50, (s16)(i_this->mC314.y + 0x8000));
            if (i_this->mKSubCount == (s16)(REG0_S(5) + 220))
                gabi::at<bgn_exec::Player>(player)->mDemo.mDemoMode = bgn_exec::DEMO_UNK_029_e;
            if (i_this->mKSubCount == (s16)(REG0_S(6) + 313))
                gabi::at<bgn_exec::Player>(player)->mDemo.mDemoMode = bgn_exec::DEMO_LOOKUP_e;
            s16 n = i_this->mKSubCount;
            if (n == 0xEB || n == 0xFA || n == 0x109 || n == 0x119 || n == 0x129) {
                /* HD: thunder around Link (a function-local static scale, initialised once) */
                if (bgn_exec::thunderInitialized() == 0) {
                    bgn_exec::thunderInitialized() = 1;
                    bgn_exec::thunderScale()->x = 0.4f;
                    bgn_exec::thunderScale()->y = 0.4f;
                    bgn_exec::thunderScale()->z = 0.4f;
                }
                dComIfGp_particle_set(0x40, &pl->current.pos, nullptr, bgn_exec::thunderScale());
            }
            if (i_this->mKSubCount >= (s16)(REG_S(18, 6) + 450)) {
                cLib_addCalc2(&i_this->mCSFovY, gabi::fadds_ppc(REG_F(18, 11), 35.0f), 0.02f,
                              gabi::fadds_ppc(REG_F(18, 12), 0.1f));
            }
        }
        break;
    case 3: {
        cLib_addCalc2(&i_this->mCSCamCenter.x, bgn_exec::bgn2()->current.pos.x, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.y,
                      gabi::fadds_ppc(bgn_exec::bgn2()->current.pos.y, REG0_F(13)), 0.1f, 200.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.z, bgn_exec::bgn2()->current.pos.z, 0.1f, 100.0f);
        if (i_this->mKSubCount == (s16)(REG0_S(7) + 90))
            i_this->mCA9C = gabi::fadds_ppc(REG0_F(18), 30.0f);
        if (i_this->mKSubCount == (s16)(REG0_S(7) + 100)) {
            auto *vt = gabi::at<bgn_exec::PlayerVtable>(gabi::at<bgn_exec::Player>(player)->__vtbl);
            gabi::at<bgn_exec::Player>(player)->mDemo.mDemoMode =
                0x18;                                   /* changeDemoMode(DEMO_SURPRISED_e) */
            gabi::call_ptr(vt->voiceStart, player, 29); /* voiceStart(29) */
        }
        if (i_this->mKSubCount > (s16)(REG0_S(8) + 110)) {
            cLib_addCalc2(&i_this->mCSFovY, gabi::fadds_ppc(REG0_F(11), 35.0f), 0.1f,
                          gabi::fadds_ppc(REG0_F(12), 0.5f));
            if (i_this->mKSubCount == 130 || i_this->mKSubCount == 180)
                mDoAud_seStart(bgn_exec::JA_SE_CM_BGN_T_MOUSE_OPEN, nullptr, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            if (i_this->mKSubCount == 152)
                mDoAud_seStart(bgn_exec::JA_SE_CM_BGN_T_MOUSE_CLOSE, nullptr, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        cLib_addCalcAngleS2(&i_this->mCA60, 1, 1, 1);
        if (i_this->mKSubCount > (s16)(REG0_S(8) + 200)) {
            mDoAud_bgmStart(bgn_exec::JA_BGM_BGN_TARABA);
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
            auto *src = bgn_exec::words(&bgn_exec::bgn2()->current.pos);
            auto *dst = bgn_exec::words(&i_this->mCSCamCenter);
            dst->x = (u32)src->x;
            dst->y = (u32)src->y;
            dst->z = (u32)src->z;
            gabi::at<bgn_exec::Player>(player)->mDemo.mDemoType =
                bgn_exec::TYPE_ORIGINAL_e; /* changeOriginalDemo */
            gabi::at<bgn_exec::Player>(player)->mDemo.mParam0 = 0;
            fpcM_Search(bgn_exec::ki_del_sub, i_this);
            gabi::at<bgn_exec::Phase2>(bgn2_g())->m2ED0 = 0;
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
        cMtx_YrotS(calc_mtx(), bgn_exec::bgn2()->shape_angle.y);
        MtxPosition(&i_this->mCA74, local_50);
        auto *e = bgn_exec::words(&i_this->mCSCamEye);
        e->x = (u32)bgn_exec::words(local_50.get())->x;
        e->y = (u32)bgn_exec::words(local_50.get())->y;
        e->z = (u32)bgn_exec::words(local_50.get())->z;
        if (i_this->mCSMode == 11) {
            cLib_addCalc2(&i_this->mCSCamCenter.x, bgn_exec::bgn2()->current.pos.x, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.y,
                          gabi::fadds_ppc(bgn_exec::bgn2()->current.pos.y, REG0_F(13)), 0.1f,
                          300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.z, bgn_exec::bgn2()->current.pos.z, 0.1f, 300.0f);
            local_44->x = REG0_F(4);
            local_44->y = pl->current.pos.y;
            local_44->z = gabi::fadds_ppc(REG0_F(5), -2000.0f);
            MtxPosition(local_44, local_50);
            auto *vt = gabi::at<bgn_exec::PlayerVtable>(gabi::at<bgn_exec::Player>(player)->__vtbl);
            gabi::call_ptr(vt->setPlayerPosAndAngle, player, local_50.get(),
                           (s16)bgn_exec::bgn2()->shape_angle.y);
        } else {
            cLib_addCalc2(&i_this->mCSCamCenter.x, bgn_exec::bgn3()->current.pos.x, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.y,
                          gabi::fadds_ppc(gabi::fadds_ppc(bgn_exec::bgn3()->current.pos.y, 50.0f),
                                          REG_F(8, 16)),
                          0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSCamCenter.z, bgn_exec::bgn3()->current.pos.z, 0.1f, 300.0f);
            cLib_addCalc2(&i_this->mCSFovY, gabi::fadds_ppc(REG0_F(17), 30.0f),
                          gabi::fadds_ppc(REG0_F(18), 0.05f), gabi::fadds_ppc(REG0_F(19), 0.2f));
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
            auto *src = bgn_exec::words(&bgn_exec::bgn3()->current.pos);
            auto *dst = bgn_exec::words(&i_this->mCSCamCenter);
            dst->x = (u32)src->x;
            dst->y = (u32)src->y;
            dst->z = (u32)src->z;
            gabi::at<bgn_exec::Player>(player)->mDemo.mDemoType =
                bgn_exec::TYPE_ORIGINAL_e; /* changeOriginalDemo */
            gabi::at<bgn_exec::Player>(player)->mDemo.mParam0 = 0;
            i_this->mCA60 = 0xB4;
            fpcM_Search(bgn_exec::ki_del_sub, i_this);
            fpcM_Search(bgn_exec::ks_del_sub, i_this);
        }
        cLib_addCalcAngleS2(&i_this->mCA60, 1, 1, 1);
        i_this->mCSCamEye.x = 500.0f;
        i_this->mCSCamEye.y = gabi::fadds_ppc(REG0_F(3), 500.0f);
        i_this->mCSCamEye.z = gabi::fadds_ppc(REG0_F(4), 2000.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.x, bgn_exec::bgn3()->current.pos.x, 0.1f, 300.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.y,
                      gabi::fadds_ppc(bgn_exec::bgn3()->current.pos.y, REG0_F(13)), 0.1f, 300.0f);
        cLib_addCalc2(&i_this->mCSCamCenter.z, bgn_exec::bgn3()->current.pos.z, 0.1f, 300.0f);
        local_44->x = REG0_F(4);
        local_44->y = pl->current.pos.y;
        local_44->z = gabi::fadds_ppc(REG0_F(5), -1000.0f);
        {
            auto *vt = gabi::at<bgn_exec::PlayerVtable>(gabi::at<bgn_exec::Player>(player)->__vtbl);
            gabi::call_ptr(vt->setPlayerPosAndAngle, player, local_44.get(),
                           (s16)bgn_exec::bgn2()->shape_angle.y);
        }
        if (i_this->mKSubCount == 260)
            bgn_monsSeStart(bgn_exec::JA_SE_CV_BGN_DIE, nullptr, 0,
                            dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        if (i_this->mKSubCount >= 370) {
            f32 s = gabi::fadds_ppc(REG0_F(4), 10.0f);
            local_5c->x = s;
            local_5c->z = s;
            local_5c->y = s;
            dComIfGp_particle_set(bgn_exec::ID_AK_JN_SIBOUBAKUEN, &bgn_exec::bgn3()->current.pos,
                                  nullptr, local_5c);
            dComIfGp_particle_set(bgn_exec::ID_AK_JN_SIBOUFLASH, &bgn_exec::bgn3()->current.pos,
                                  nullptr, local_5c);
            mDoAud_seStart(bgn_exec::JA_SE_CM_BGN_LAST_EXPLODE, nullptr, 0,
                           dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            gabi::Local<cXyz> up;
            dVibration_c *vib = dComIfGp_getVibration();
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            StartShock(vib, REG0_S(2) + 8, -0x21, up);
            i_this->m02B5 = 3;
            dComIfGs_onEventBit(bgn_exec::UNK_3F10);
            s8 mode = (s8)(i_this->mCSMode + 1);
            i_this->mC720 = 1;
            i_this->mCA60 = 0xB4;
            i_this->mCSMode = mode;
        }
        break;
    }
    case 22:
        if (i_this->mKSubCount >= 390)
            last_himo_control(i_this);
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
            s16 bank = (s16)gabi::ftoi(
                gabi::fmuls_ppc(gabi::fmuls_ppc(cM_scos(i_this->mC746 * 0x1C00), ca9c), 7.5f));
            f32 fovy = i_this->mCSFovY;
            camera_eye->z = i_this->mCSCamEye.z;
            camera_center->z = i_this->mCSCamCenter.z;
            bgn_camera_Set(cam, camera_center, camera_eye, bank, fovy);
            cLib_addCalc0(&i_this->mCA9C, 1.0f, gabi::fadds_ppc(REG0_F(16), 1.0f));
            bgn_JUTReport(30, 420, "K SUB  COUNT  %d", i_this->mKSubCount);
            bgn_JUTReport(30, 440, "K DEMO MODE   %d", i_this->mCSMode);
            i_this->mKSubCount = (s16)(i_this->mKSubCount + 1);
        }
    } else {
        bgn_orderPotentialEvent(actor, bgn_exec::dEvtFlag_STAFF_ALL_e, 0xFFFF, 0);
        eventInfo_onCondition(actor, dEvtCnd_UNK2_e);
    }

    s8 spawn = i_this->mKeeseSpawnNum;
    if (spawn != 0) {
        i_this->mKeeseSpawnNum = (s8)(spawn - 1);
        if (ki_check(i_this) < l_HIO().mKeeseNumMax) {
            gabi::Local<cXyz> local_28;
            local_28->x = cM_rndFX(2500.0f);
            local_28->y = gabi::fadds_ppc(cM_rndF(500.0f), 3500.0f);
            local_28->x = cM_rndFX(2500.0f); /* GameCube bug kept: z is never set */
            fopAcM_create(bgn_exec::fpcNm_KI_e, 0xFFFF0003, local_28, fopAcM_GetRoomNo(actor),
                          nullptr, nullptr, -1, 0);
        }
    }
    if (i_this->mCC90 == 2) {
        i_this->mCC90 = 0;
        bgn_setNextStage("GanonK", 4, 0, 9, 0.0f, 0, 1, 0);
    }
    return TRUE;
}

// clang-format off
/* WWHD glue
ext 02514F2C void bgn_camera_Stop(bgn_exec::Camera*)
ext 02514F38 void bgn_camera_Start(bgn_exec::Camera*)
ext 02515280 void bgn_camera_SetTrimSize(bgn_exec::Camera*,s32)
ext 02514FE8 void bgn_camera_Set(bgn_exec::Camera*,cXyz*,cXyz*,s16,f32)
ext 025D7B24 BOOL bgn_orderPotentialEvent(fopAc_ac_c*,u16,u16,u16)
ext 0259169C void bgn_dMeter_mtrShow()
ext 025E1A7C void bgn_monsSeStart(u32,cXyz*,u32,s32)
ext 027EC9E8 void bgn_JUTReport(s32,s32,const char*,s32)
ext 0252012C void bgn_setNextStage(const char*,s16,s8,s8,f32,u32,s32,s8)
ext 0208857C void bgn_exec_move(bgn_class*)
str 10008AF4
str 10008B08
str 10008C40
*/
