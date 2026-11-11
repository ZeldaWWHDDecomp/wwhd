/**
 * d_a_bdk_demo.cpp (WWHD)
 * Boss - Helmaroc King (battle): demo_camera (the opening, mask-break and death cutscene cameras).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk_demo.h"

using gabi::fadds_ppc;
using gabi::fmuls_ppc;
using gabi::fsubs_ppc;

/* demo_camera. HD: the player is placed (and turned towards the boss) in more steps, the camera
 * shakes around both points in opposite directions, no line check against the walls, three debug
 * reports, the bridge-rope objects are moved with the player at the start */
/* 0206FEF0 */
void demo_camera(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    daPy_py_c *player = (daPy_py_c *)dComIfGp_getPlayer(0);
    s8 camId = dComIfGp_getPlayerCameraID(0);
    auto camera = (BdkDemoCamera *)dComIfGp_getCamera(camId);
    auto cam = BdkDemo_cameraBody(camera);
    gabi::Local<cXyz> local_104;
    gabi::Local<cXyz> local_108;
    gabi::Local<csXyz> local_14C;
    gabi::Local<csXyz> local_154;

    f32 dVar16 = 45.0f;
    f32 dVar15 = 1.0f;
    f32 dVar14 = 0.05f;
    u8 r27 = 0;

    switch ((u16)i_this->mDemoMode) {
    case 0x0:
        break;
    case 0x1:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2 /* dEvtType_OTHER_e */, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->mDemoMode = i_this->mDemoMode + 1;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        i_this->mDemoMainCount = 0;
        i_this->mDemoTimer = 0;
        i_this->mDemoCamFovy = 45.0f;
        i_this->mMaskBreakStage = 4;
        bdk_changeOriginalDemo(player);
        /* fall through */
    case 0x2: {
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        local_104->x = fadds_ppc(REG0_F(0), -80.0f);
        local_104->y = fadds_ppc(REG0_F(1), -30.0f);
        local_104->z = fadds_ppc(REG0_F(2), 50.0f);
        MtxPosition(local_104, &i_this->mDemoCamCenter);
        PSVECAdd(&i_this->mDemoCamCenter, &i_this->mHeadPos, &i_this->mDemoCamCenter);

        local_104->x = fadds_ppc(REG0_F(5), 400.0f);
        local_104->y = fadds_ppc(REG0_F(6), -100.0f);
        local_104->z = fadds_ppc(REG0_F(7), 400.0f);
        MtxPosition(local_104, &i_this->mDemoCamEye);
        PSVECAdd(&i_this->mDemoCamEye, &i_this->mHeadPos, &i_this->mDemoCamEye);

        local_104->x = fadds_ppc(REG0_F(0), -230.0f);
        local_104->y = 0.0f;
        local_104->z = fadds_ppc(REG0_F(2), 110.0f);
        MtxPosition(local_104, local_108);
        local_108->x = fadds_ppc(local_108->x, i_this->mHeadPos.x);
        BdkDemo_copyFloat(local_108->y, player->current.pos.y);
        local_108->z = fadds_ppc(local_108->z, i_this->mHeadPos.z);
        bdk_setPlayerPosAndAngle(player, local_108, (s16)(actor->shape_angle.y + REG0_S(6) + 20000));

        s16 r4 = i_this->mDemoTimer;
        if (r4 > REG0_S(7) + 0x3C) {
            dVar16 = 20.0f;
            dVar15 = 1.0f;
            dVar14 = 0.5f;
        } else if (r4 > 0) {
            dVar16 = 35.0f;
            dVar15 = 0.35f;
        }
        if (r4 == 0x51) {
            bdk_changeDemoMode(player, daPy_demo_c::DEMO_SMILE_e);
        }
        cLib_addCalc2(&i_this->mDemoCamFovy, dVar16, dVar14, dVar15);
        break;
    }
    case 0xA:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->mDemoMode = i_this->mDemoMode + 1;
        i_this->mDemoCamSpeed = 0.0f;
        i_this->mDemoMainCount = 0;
        i_this->mDemoTimer = 0;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        i_this->mDemoCamFovy = 55.0f;
        /* HD: the cutscene starts from the current camera */
        BdkDemo_copyVector(&i_this->mDemoCamCenter, &camera->mCenter);
        BdkDemo_copyVector(&i_this->mDemoCamEye, &camera->mEye);
        mDoAud_bgmStreamPrepare_bdk(JA_STRM_DK_START);
        bdk_changeOriginalDemo(player);
        bdk_changeDemoMode(player, daPy_demo_c::DEMO_A_WAIT_e); /* HD */
        bdk_changeDemoParam0(player, 2);                        /* HD */
        /* fall through */
    case 0xB:
        /* HD: the player is held at the start point (and the object found by obj_s_sub moved with
         * it on the first frame); the camera points of GameCube case 0xA are set on frame 1 */
        if (i_this->mDemoTimer >= 0) {
            local_108->x = 2517.0f;
            BdkDemo_copyFloat(local_108->y, player->current.pos.y);
            local_108->z = -3662.0f;
            if (i_this->mDemoTimer == 0) {
                u32 found = gabi::ea(fpcM_Search(BdkDemoSearch::obj_s_sub, actor));
                if (found != 0) {
                    cXyz *fp = &gabi::at<fopAc_ac_c>(found)->current.pos;
                    fp->x = fadds_ppc(fp->x, fsubs_ppc(local_108->x, player->current.pos.x));
                    fp->z = fadds_ppc(fp->z, fsubs_ppc(local_108->z, player->current.pos.z));
                }
            }
            bdk_setPlayerPosAndAngle(player, local_108, player->shape_angle.y);
            if (i_this->mDemoTimer == 1) {
                i_this->mDemoCamCenter.x = 2598.0f;
                i_this->mDemoCamCenter.y = 9902.0f;
                i_this->mDemoCamCenter.z = -3498.0f;
                i_this->mDemoCamEye.x = 2228.0f;
                i_this->mDemoCamEye.y = 9961.0f;
                i_this->mDemoCamEye.z = -3403.0f;
            }
        }
        cLib_addCalc2(&i_this->mDemoCamCenter.x, 164.0f, 0.05f, fmuls_ppc(24.34f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamCenter.y, 11617.0f, 0.05f, fmuls_ppc(17.15f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamCenter.z, -2802.0f, 0.05f, fmuls_ppc(6.96f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamEye.x, -167.0f, 0.05f, fmuls_ppc(23.949999f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamEye.y, 11791.0f, 0.05f, fmuls_ppc(18.3f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamEye.z, -2706.0f, 0.05f, fmuls_ppc(6.97f, i_this->mDemoCamSpeed));

        if (i_this->mDemoTimer > (s16)(REG0_S(1) + 0x1E)) {
            cLib_addCalc2(&i_this->mDemoCamSpeed, fadds_ppc(REG0_F(1), 1.0f), 1.0f, fadds_ppc(REG0_F(2), 0.015f));
        }
        if (i_this->mDemoTimer >= (s16)(REG0_S(6) + 0xBE)) {
            if (i_this->mDemoTimer == (s16)(REG0_S(6) + 0xD2)) {
                bdk_changeDemoMode(player, daPy_demo_c::DEMO_UNK_028_e);
                mDoAud_bgmStreamPlay_bdk();
            }
            if (i_this->mDemoTimer == (s16)(REG0_S(6) + 0xBE)) {
                local_14C->x = 0;
                local_14C->z = 0;
                for (s32 i = 0; i <= 2; i++) {
                    local_14C->y = (s16)(10000 + i * 0x5555 + REG_S(8, 4));
                    bdk_particle_setToon(ID_IT_ST_DK_FUTATOJI_SMOKE_A00, &center_pos, local_14C, nullptr,
                                         0xB9, &i_this->mLidSmokeCb[i], (s8)fopAcM_GetRoomNo(actor));
                }
                bdk_particle_setToon(ID_IT_ST_DK_FUTATOJI_SMOKE_B00, &center_pos2, nullptr, nullptr, 0xB9,
                                     &i_this->mLidSmokeCb[3], (s8)fopAcM_GetRoomNo(actor));
                i_this->mpDeadHaneEmitter[1] =
                    dComIfGp_particle_set(ID_IT_SN_DK_FUTATOJI_ROCK00, &center_pos2, nullptr, nullptr, 0xFF);
            }
            i_this->mDemoShakeAmp = fadds_ppc(REG_F(18, 18), 0.75f); /* HD: 0.75 (GameCube 1.5) */
            mDoAud_seStart_simple_bdk(JA_SE_ATM_MJT_JINARI);
            cLib_addCalc0(&i_this->mShutterSlide, 1.0f, fadds_ppc(REG0_F(0), 1.0f));
            i_this->mShutterRotY = fadds_ppc(i_this->mShutterRotY, fadds_ppc(REG0_F(8), 0.003f));
            r27 = 1;
        }
        if (!(i_this->mDemoTimer > (s16)(REG0_S(2) + 0xFA))) {
            break; /* HD: the player is not placed again here */
        }
        i_this->mDemoMode = 0xC;
        i_this->mDemoTimer = 0;
        i_this->mDemoCamCenter.x = 2195.0f;
        i_this->mDemoCamCenter.y = 10007.0f;
        i_this->mDemoCamCenter.z = -4079.0f;
        i_this->mDemoCamEye.x = 1816.0f;
        i_this->mDemoCamEye.y = 10051.0f;
        i_this->mDemoCamEye.z = -4137.0f;
        i_this->mDemoCamSpeed = 0.0f;
        /* fall through */
    case 0xC:
        cLib_addCalc0(&i_this->mShutterSlide, 1.0f, fadds_ppc(REG0_F(0), 1.0f));
        i_this->mShutterRotY = fadds_ppc(i_this->mShutterRotY, fadds_ppc(REG0_F(8), 0.003f));
        r27 = 1;
        if (i_this->mDemoTimer < (s16)(REG0_S(6) + 0x9B)) {
            i_this->mDemoShakeAmp = fadds_ppc(REG_F(18, 5), 2.5f); /* HD: 2.5 (GameCube 5.0) */
            mDoAud_seStart_simple_bdk(JA_SE_ATM_MJT_JINARI);
        }
        if (i_this->mDemoTimer == (s16)(REG0_S(6) + 0x96)) {
            i_this->mDemoStepReq = 1;
        }
        if (i_this->mDemoTimer == 0xA3) { /* HD: a fixed frame (GameCube REG0_S(7) + 0xA0) */
            i_this->mDemoTimer = 0;
            i_this->mDemoMode = 0xD;
        }
        cLib_addCalc2(&i_this->mDemoCamCenter.x, 2682.0f, 0.05f, fmuls_ppc(4.87f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamCenter.y, 9904.0f, 0.05f, fmuls_ppc(1.03f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamCenter.z, -3600.0f, 0.05f, fmuls_ppc(4.79f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamEye.x, 2308.0f, 0.05f, fmuls_ppc(4.92f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamEye.y, 9961.0f, 0.05f, fmuls_ppc(0.9f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamEye.z, -3524.0f, 0.05f, fmuls_ppc(6.1299998f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamSpeed, fadds_ppc(REG0_F(3), 1.5f), 1.0f, fadds_ppc(REG0_F(4), 0.02f));

        local_108->x = 2517.0f; /* HD: 2517 / -3662 (GameCube 2580 / -3670) */
        BdkDemo_copyFloat(local_108->y, player->current.pos.y);
        local_108->z = -3662.0f;
        bdk_setPlayerPosAndAngle(player, local_108, 18000);
        break;
    case 0xD:
        if (i_this->mDemoTimer < (s16)(REG0_S(3) + 0x64)) {
            cLib_addCalc2(&i_this->mDemoCamFovy, 25.0f, 0.1f, 0.5f);
        } else {
            cLib_addCalc2(&i_this->mDemoCamFovy, 55.0f, 0.2f, 3.0f);
        }
        if (i_this->mDemoTimer == (s16)(REG0_S(8) + 0x37)) {
            i_this->mDemoStepReq = 1;
        }
        cLib_addCalc2(&i_this->mDemoCamCenter.x, i_this->mHeadPos.x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->mDemoCamCenter.y, fadds_ppc(fsubs_ppc(i_this->mHeadPos.y, 200.0f), REG0_F(11)), 0.1f,
                      50.0f);
        cLib_addCalc2(&i_this->mDemoCamCenter.z, i_this->mHeadPos.z, 0.1f, 50.0f);
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BDK_BCK_FLY3_e, 1.0f, 2, 1.0f, dRes_INDEX_BDK_BAS_FLY3_e, 0);
            bdk_changeDemoMode(player, daPy_demo_c::DEMO_A_WAIT_e);
            bdk_changeDemoParam0(player, 2);
        }
        if (i_this->mDemoTimer == (s16)(REG0_S(8) + 0xF0)) {
            /* HD: the sub count runs on; the shutter values of GameCube case 0xE are set here */
            i_this->mDemoMode = 0xE;
            i_this->mShutterSlide = fadds_ppc(REG0_F(17), -100.0f);
            i_this->mShutterRotY = 0.0f;
        }
        r27 = 1;
        break;
    case 0xE:
        i_this->mDemoMode = 0xF;
        i_this->mDemoCamCenter.x = 3304.0f;
        i_this->mDemoCamCenter.y = 10983.0f;
        i_this->mDemoCamCenter.z = -3644.0f;
        i_this->mDemoCamEye.x = 2841.0f;
        i_this->mDemoTimer = 0;
        i_this->mDemoCamEye.y = 13058.0f;
        i_this->mDemoCamEye.z = -3272.0f;
        i_this->mDemoCamSpeed = 0.0f;
        /* fall through */
    case 0xF:
        r27 = 1;
        /* fall through */
    case 0x10:
        cLib_addCalc0(&i_this->mShutterSlide, 1.0f, fadds_ppc(REG0_F(11), 1.5f));
        if (std::fabs((f32)i_this->mShutterSlide) < 1.0f) {
            cLib_addCalc0(&i_this->mShutterOffsetY, 1.0f, fadds_ppc(REG0_F(12), 1.0f));
            if (i_this->mDemoMode == 0xF) {
                dComIfGs_onTmpBit(0x401); /* dComIfGs_onTmpBit? UNK_0401 */
                BdkDemo_onDungeonItem(&dComIfGs_info()->mMemory.mMembit, dSv_memBit_c::STAGE_BOSS_DEMO);
                fpcM_Search(BdkDemoSearch::ep_delete_sub, actor);
                mDoAud_seStart(JA_SE_ATM_MJT_SHUTTER_END, &center_pos, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                bdk_StartShock(4);
                r27 = 0;
                i_this->mDemoMode = 0x10;
                local_154->x = 0;
                local_154->z = 0;
                for (s32 i = 0; i <= 2; i++) {
                    s32 room = fopAcM_GetRoomNo(actor);
                    local_154->y = (s16)(i * 0x5555);
                    bdk_particle_setToon(ID_IT_ST_DK_FUTATOJI_SMOKE_C00, &center_pos, local_154, nullptr,
                                         0xB9, &i_this->mLidSmokeCb[i], (s8)room);
                }
                BdkDemo_smokeEnd(&i_this->mLidSmokeCb[3]); /* HD: dPa_smokeEcallBack::end (GameCube remove()) */
                JPABaseEmitter *e = i_this->mpDeadHaneEmitter[1];
                if (e != nullptr) {
                    bdk_becomeInvalidEmitter(e);
                    i_this->mpDeadHaneEmitter[1] = nullptr;
                }
            }
        }
        cLib_addCalc2(&i_this->mDemoCamEye.x, 3152.0f, 0.05f, fmuls_ppc(3.11f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamEye.z, -3070.0f, 0.05f, fmuls_ppc(2.02f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamSpeed, fadds_ppc(REG0_F(3), 1.2f), 1.0f, fadds_ppc(REG0_F(4), 0.015f));
        if (i_this->mDemoTimer == 0xA0) {
            i_this->mDemoMode = 0x96;
            i_this->mAction = ACTION_FLY;
            i_this->mFlyAnm = -1;
            i_this->mState = 0;
            actor->gravity = fadds_ppc(REG0_F(4), -5.0f);
            i_this->mTowerBattle = 0;
            for (s32 i = 0; i < 10; i++) {
                i_this->mWindAtSph[i].SetAtType(AT_TYPE_WIND);
            }
            actor->shape_angle.x = 0;
            mDoAud_bgmStart(JA_BGM_DK_BATTLE);
        }
        break;
    case 0x32:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->mDemoCamSpeed = 0.0f;
        i_this->mDemoMode = i_this->mDemoMode + 1;
        i_this->mDemoMainCount = 0;
        i_this->mDemoTimer = 0;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        {
            f32 ey = actor->eyePos.y;
            BdkDemo_copyFloat(i_this->mDemoCamCenter.y, actor->eyePos.y);
            BdkDemo_copyFloat(i_this->mDemoCamCenter.z, actor->eyePos.z);
            i_this->mDemoCamEye.x = 3267.0f;
            i_this->mDemoCamEye.y = 9561.0f;
            i_this->mDemoCamEye.z = -4562.0f;
            BdkDemo_copyFloat(i_this->mDemoCamCenter.x, actor->eyePos.x);
            i_this->mDemoCamCenter.y = fsubs_ppc(ey, fadds_ppc(REG0_F(7), 100.0f));
        }
        i_this->mDemoCamFovy = 60.0f;
        bdk_changeOriginalDemo(player);
        /* fall through */
    case 0x33: {
        /* HD: the player is turned towards the boss */
        u32 vt = player->__vtbl;
        s16 a = fopAcM_searchActorAngleY(actor, dComIfGp_getPlayer(0));
        gabi::call_ptr(gabi::at<BdkDemoPlayerVtable>(vt)->setPlayerPosAndAngle, player, &player->current.pos,
                       (s16)(a + 0x8000));
        cLib_addCalc2(&i_this->mDemoCamCenter.x, actor->eyePos.x, 0.1f, fmuls_ppc(100.0f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamCenter.y, fsubs_ppc(actor->eyePos.y, fadds_ppc(REG0_F(7), 100.0f)), 0.3f,
                      fmuls_ppc(200.0f, i_this->mDemoCamSpeed));
        cLib_addCalc2(&i_this->mDemoCamCenter.z, actor->eyePos.z, 0.1f, fmuls_ppc(100.0f, i_this->mDemoCamSpeed));
        if (i_this->mAction == ACTION_START) {
            i_this->mDemoMode = i_this->mDemoMode + 1;
            i_this->mDemoTimer = 0;
        }
        cLib_addCalc2(&i_this->mDemoCamSpeed, 1.0f, 1.0f, fadds_ppc(REG0_F(4), 0.02f));
        break;
    }
    case 0x34:
        /* HD: the field of view is eased to 45 */
        cLib_addCalc2(&i_this->mDemoCamFovy, 45.0f, fadds_ppc(REG_F(18, 3), 0.02f), fadds_ppc(REG_F(18, 4), 0.075f));
        if (i_this->mDemoTimer >= 0x64) {
            i_this->mDemoMode = 0x96;
        }
        break;
    case 0x64:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->mDemoCamSpeed = 0.0f;
        i_this->mDemoMode = i_this->mDemoMode + 1;
        i_this->mDemoMainCount = 0;
        i_this->mDemoTimer = 0;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        {
            auto pCamera = (BdkDemoCamera *)dComIfGp_getCamera(0);
            cXyz *eye = &pCamera->mEye;
            BdkDemo_copyVector(&i_this->mDemoCamEye, eye);
            local_104->y = 0.0f;
            local_104->x = fsubs_ppc(eye->x, i_this->mHeadPos.x);
            local_104->z = fsubs_ppc(eye->z, i_this->mHeadPos.z);
            i_this->mDemoCamAngle.y = cM_atan2s(local_104->x, local_104->z);
            f32 z = local_104->z;
            f32 x = local_104->x;
            f32 d = std_sqrtf(gabi::fmadds(x, x, fmuls_ppc(z, z)));
            i_this->mDemoCamAngle.x = (s16)-cM_atan2s(local_104->y, d);
            s16 r = (s16)gabi::ftoi(cM_rndFX(4000.0f));
            i_this->mDemoCamDist = 1500.0f;
            actor->current.angle.y = (s16)(i_this->mDemoCamAngle.y + r);
            i_this->mDemoCamFovy = 45.0f;
            bdk_changeOriginalDemo(player);
            mDoAud_bgmStreamPrepare_bdk(JA_STRM_DK_CLEAR);
            /* HD: the camera target starts at the body (it is eased towards it in case 0x65) */
            f32 y = i_this->mHeadPos.y;
            BdkDemo_copyFloat(i_this->mDemoCamCenter.x, i_this->mHeadPos.x);
            BdkDemo_copyFloat(i_this->mDemoCamCenter.y, i_this->mHeadPos.y);
            BdkDemo_copyFloat(i_this->mDemoCamCenter.z, i_this->mHeadPos.z);
            i_this->mDemoCamCenter.y = fadds_ppc(y, fadds_ppc(REG0_F(16), -100.0f));
        }
        /* fall through */
    case 0x65:
        if (i_this->mDemoTimer >= 0xF) {
            /* HD: the player turns towards the boss from frame 15 on */
            bdk_changeDemoMode(player, daPy_demo_c::DEMO_LOOKUP_e);
            u32 vt = player->__vtbl;
            s16 a = fopAcM_searchActorAngleY(actor, dComIfGp_getPlayer(0));
            gabi::call_ptr(gabi::at<BdkDemoPlayerVtable>(vt)->setPlayerPosAndAngle, player,
                           &player->current.pos, (s16)(a + 0x8000));
            if (i_this->mDemoTimer == 0x3C) {
                mDoAud_bgmStreamPlay_bdk();
            }
        }
        cLib_addCalc2(&i_this->mDemoCamCenter.x, i_this->mHeadPos.x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->mDemoCamCenter.y, fsubs_ppc(i_this->mHeadPos.y, 100.0f), 0.1f, 50.0f);
        cLib_addCalc2(&i_this->mDemoCamCenter.z, i_this->mHeadPos.z, 0.1f, 50.0f);
        local_104->y = 0.0f;
        local_104->z = fadds_ppc(i_this->mDemoCamDist, REG0_F(17));
        local_104->x = 0.0f;
        cMtx_YrotS(calc_mtx(), i_this->mDemoCamAngle.y);
        cMtx_XrotM(calc_mtx(), i_this->mDemoCamAngle.x);
        MtxPosition(local_104, local_108);
        PSVECAdd(local_108, &actor->current.pos, local_108); /* HD: around the boss (GameCube mDemoCamCenter) */
        cLib_addCalc2(&i_this->mDemoCamEye.x, local_108->x, 0.1f, 100.0f);
        if (i_this->mFinalHitHigh == 0) {
            i_this->mDemoCamEye.y = fadds_ppc(REG0_F(18), 9950.0f);
        } else {
            cLib_addCalc2(&i_this->mDemoCamEye.y, fadds_ppc(local_108->y, REG0_F(13)), 0.1f, 100.0f);
        }
        cLib_addCalc2(&i_this->mDemoCamEye.z, local_108->z, 0.1f, 100.0f);
        break;
    case 0x6E:
        i_this->mDemoMode = 0x6F;
        i_this->mDemoCamFovy = 40.0f;
        i_this->mDemoCamEye.x = -25267.0f;
        i_this->mDemoCamEye.y = -948.0f;
        i_this->mDemoCamEye.z = 22916.0f;
        /* fall through */
    case 0x6F: {
        f32 fVar17 = actor->current.pos.y;
        i_this->mDemoCamCenter.x = 3600.0f;
        if (fVar17 < 30000.0f) {
            i_this->mDemoCamCenter.y = fadds_ppc(fsubs_ppc(fVar17, 6000.0f), REG0_F(19));
        }
        i_this->mDemoCamCenter.z = -3800.0f;
        cLib_addCalc2(&i_this->mDemoCamEye.z, 30449.0f, 0.02f, fmuls_ppc(75.33f, i_this->mDemoCamSpeed));
        if (i_this->mDemoTimer > REG0_S(2)) {
            cLib_addCalc2(&i_this->mDemoCamSpeed, fadds_ppc((f32)REG0_S(1), 1.5f), 1.0f, fadds_ppc(REG0_F(2), 0.02f));
        }
        local_108->x = 4444.0f;
        local_108->y = 9800.0f;
        local_108->z = -4500.0f;
        bdk_setPlayerPosAndAngle(player, local_108, 0);
        break;
    }
    case 0x70:
        i_this->mDemoMode = i_this->mDemoMode + 1;
        i_this->mDemoTimer = 0;
        i_this->mDemoCamFovy = dVar16;
        BdkDemo_copyVector(&i_this->mDemoCamCenter, &i_this->mTargetPos);
        /* fall through */
    case 0x71:
        if (i_this->mDemoTimer > REG0_S(7) + 0x43) {
            cLib_addCalc2(&i_this->mDemoCamCenter.y, fadds_ppc(REG0_F(4), 9900.0f), 0.5f,
                          fadds_ppc(REG0_F(6), 200.0f));
        }
        BdkDemo_copyVector(&i_this->mDemoCamEye, &i_this->mTargetPos);
        i_this->mDemoCamEye.y = fadds_ppc(REG0_F(5), 10000.0f);
        i_this->mDemoCamEye.x = fadds_ppc(i_this->mDemoCamEye.x, fadds_ppc(REG0_F(7), 500.0f));
        {
            f32 y = i_this->mTargetPos.y;
            local_108->x = fadds_ppc(i_this->mTargetPos.x, fsubs_ppc(REG0_F(11), 200.0f));
            local_108->y = y;
            local_108->z = fadds_ppc(i_this->mTargetPos.z, fsubs_ppc(REG0_F(12), 150.0f));
            BdkDemo_copyFloat(local_108->y, player->current.pos.y);
        }
        bdk_setPlayerPosAndAngle(player, local_108, 10000);
        if (i_this->mDemoTimer == REG_S(18, 2) + 0x69) {
            bdk_changeDemoMode(player, daPy_demo_c::DEMO_SMILE_e);
        }
        if (i_this->mDemoTimer == REG0_S(2) + 0x82) {
            bdk_changeDemoMode(player, daPy_demo_c::DEMO_UNK_029_e);
        }
        if (i_this->mDemoTimer == REG0_S(4) + 0xBE) {
            i_this->mDemoMode = i_this->mDemoMode + 1;
            i_this->mDemoTimer = 0;
            BdkDemo_copyFloat(local_104->x, center_pos.x);
            BdkDemo_copyFloat(local_104->z, center_pos.z);
            local_104->y = fadds_ppc(REG_F(18, 7), 10300.0f);
            eff_hane_set(i_this, local_104, 0x27, -1);
        }
        break;
    case 0x72:
        i_this->mDemoCamCenter.x = 4386.0f;
        i_this->mDemoCamCenter.y = 10020.0f;
        i_this->mDemoCamCenter.z = -4368.0f;
        i_this->mDemoCamEye.x = 4144.0f;
        i_this->mDemoCamEye.y = 10061.0f;
        i_this->mDemoCamEye.z = -4131.0f;
        if (i_this->mDemoTimer == 10) {
            BdkDemo_onDungeonItem(&dComIfGs_info()->mMemory.mMembit, dSv_memBit_c::STAGE_BOSS_ENEMY);
            mDoAud_seStart(JA_SE_OBJ_TOGE_IN, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        if (i_this->mDemoTimer == 0x28) {
            i_this->mDemoMode = 0x96;
        }
        break;
    case 0x96: {
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        BdkDemo_copyVector(center, &i_this->mDemoCamCenter);
        BdkDemo_copyVector(eye, &i_this->mDemoCamEye);
        i_this->mDemoMode = 0;
        dCamera_Reset_bdk(cam, center, eye);
        dCamera_Start_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 0);
        dComIfGp_event_reset();
        actor->actor_status &= ~fopAcStts_UNK4000_e;
        i_this->mBlurRate = 0;        /* HD */
        BdkDemo_globalFlag() = 0; /* HD: a global flag */
        break;
    }
    default:
        break;
    }

    if (i_this->mDemoMode != 0) {
        f32 fx, fy, fz;
        s16 bank = 0;
        f32 amp = i_this->mDemoShakeAmp;
        if (amp > 0.1f) {
            s16 c = i_this->mDemoMainCount;
            if (i_this->mDemoMode == 0x33) { /* HD: a faster shake for the start cutscene */
                fx = fmuls_ppc(cM_ssin(c * 0x6300), amp);
                fy = fmuls_ppc(cM_scos(c * 0x5900), amp);
                fz = fmuls_ppc(cM_ssin(c * 0x6900), amp);
            } else {
                fx = fmuls_ppc(cM_ssin(c * 0x3A00), amp);
                fy = fmuls_ppc(cM_scos(c * 0x3300), amp);
                fz = fmuls_ppc(cM_ssin(c * 0x3500), amp);
            }
            bank = (s16)gabi::ftoi(fmuls_ppc(cM_scos(c * 0x1C00), fadds_ppc(REG0_F(6), 50.0f)));
        } else {
            fx = 0.0f;
            fy = 0.0f;
            fz = 0.0f;
        }
        gabi::Local<cXyz> local_f0;
        gabi::Local<cXyz> local_e4;
        local_e4->x = fadds_ppc(i_this->mDemoCamEye.x, fx);
        local_f0->x = fsubs_ppc(i_this->mDemoCamCenter.x, fx);
        local_f0->y = fsubs_ppc(i_this->mDemoCamCenter.y, fy);
        local_e4->y = fadds_ppc(i_this->mDemoCamEye.y, fy);
        local_f0->z = fsubs_ppc(i_this->mDemoCamCenter.z, fz);
        local_e4->z = fadds_ppc(i_this->mDemoCamEye.z, fz);
        dCamera_Set_bdk(cam, local_f0, local_e4, bank, i_this->mDemoCamFovy);
        cLib_addCalc0(&i_this->mDemoShakeAmp, 1.0f, fadds_ppc(REG0_F(16), 1.0f));
        BdkDemo_report(0x1E, 0x190, BdkDemo_reports().mainCount, i_this->mDemoMainCount);
        BdkDemo_report(0x1E, 0x1A4, BdkDemo_reports().subCount, i_this->mDemoTimer);
        BdkDemo_report(0x1E, 0x1B8, BdkDemo_reports().demoMode, i_this->mDemoMode);
        i_this->mDemoTimer = i_this->mDemoTimer + 1;
        i_this->mDemoMainCount = i_this->mDemoMainCount + 1;
    }
    if (r27) {
        mDoAud_seStart(JA_SE_ATM_MJT_SHUTTER, &center_pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
    }
}

/* WWHD glue
ext 025B9098 void BdkDemo_onDungeonItem(dSv_memBit_c*, s32)
ext 02514F2C void dCamera_Stop_bdk(BdkDemoCameraBody*)
ext 02514F38 void dCamera_Start_bdk(BdkDemoCameraBody*)
ext 02515280 void dCamera_SetTrimSize_bdk(BdkDemoCameraBody*, s32)
ext 02514FE8 void dCamera_Set_bdk(BdkDemoCameraBody*, cXyz*, cXyz*, s16, f32)
ext 0251510C void dCamera_Reset_bdk(BdkDemoCameraBody*, cXyz*, cXyz*)
ext 025D7B24 BOOL fopAcM_orderPotentialEvent_bdk(fopAc_ac_c*, u16, u16, u16)
ext 025E1934 void mDoAud_bgmStreamPrepare_bdk(u32)
ext 025E1944 void mDoAud_bgmStreamPlay_bdk()
ext 025E1988 void mDoAud_seStart_simple_bdk(u32)
ext 027EC9E8 void BdkDemo_report(s32, s32, const char*, s32)
ext 025A5F88 void BdkDemo_smokeEnd(dPa_smokeEcallBack_bdk*)
*/
