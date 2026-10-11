// Molgera camera sequence.
#include "d/actor/d_a_bwd_camera.h"

using namespace bwd_camera;

/* inlined into demo_camera */
static void startDemoQuake() {
    gabi::Local<cXyz> dir;
    dir->set(0, 1, 0);
    auto* vib = dComIfGp_getVibration();
    bwdCameraStartQuake(vib, REG0_S(5) + 3, -0x21, dir.get());
}
/* 02101F30 */
static void demo_camera(bwd_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    fopAc_ac_c* actor = i_this;
    f32 fVar1;
    f32 fVar2;
    s16 sVar3;
    bwd_camera::camera_process_class* camera2;
    fopAc_ac_c* pfVar7;
    gabi::Local<cXyz> local_98;
    gabi::Local<cXyz> local_a4;
    gabi::Local<cXyz> local_b0;
    gabi::Local<cXyz> local_d4;
    gabi::Local<cXyz> local_e0;

    daPy_py_c* player = (daPy_py_c*)dComIfGp_getPlayer(0);
    daPy_py_c* player2 = player;
    auto* camera = bwd_camera::getCamera(bwd_camera::getPlayerCameraID());
    switch (i_this->m3C1E) {
    case DEMO_IDLE:
        break;
    case DEMO_START:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            bwdCameraOrderEvent(actor, dEvtFlag_STAFF_ALL_e, 0xFFFF, 0);
            eventInfo_onCondition(actor, dEvtCnd_UNK2_e);
            return;
        }
        i_this->m3C1E++;
        camera2 = bwd_camera::getCamera(0);
        i_this->m3C28.copy(camera2->view.mLookat.mEye);
        i_this->m3C34.copy(camera2->view.mLookat.mCenter);
        bwdCameraStop(&camera->mCamera);
        bwdCameraTrim(&camera->mCamera, 2);
        i_this->m3C20 = 0;
        i_this->m3C22 = 0;
        i_this->m3C48 = 55.0f;
        i_this->m3C44 = 0.0f;
        i_this->m3C1C = 0x96;
        // fallthrough
    case DEMO_TRACK:
        local_98->x = 0.0f;
        local_98->y = 5000.0f;
        local_98->z = 0.0f;
        player2->setPlayerPosAndAngle(local_98.get(), (int)player->shape_angle.y);
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        local_98->x = 0.0f;
        local_98->y = REG0_F(6) + 500.0f;
        local_98->z = REG0_F(7) + 1500.0f;
        MtxPosition(local_98.get(), local_a4.get());
        bwdCameraVectorAdd(local_a4.get(), &actor->current.pos, local_a4.get());
        cLib_addCalc2(&i_this->m3C28.x, local_a4->x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.y, local_a4->y, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.z, local_a4->z, 0.1f, 50.0f);
        local_98->x = 0.0f;
        local_98->y = REG0_F(8) + 400.0f;
        local_98->z = REG0_F(9);
        MtxPosition(local_98.get(), local_a4.get());
        bwdCameraVectorAdd(local_a4.get(), &actor->current.pos, local_a4.get());
        cLib_addCalc2(&i_this->m3C34.x, local_a4->x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C34.y, local_a4->y, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C34.z, local_a4->z, 0.1f, 50.0f);
        break;
    case DEMO_TRACK_PLAYER:
        local_a4->copy(player->current.pos);
        local_a4->y += 100.0f;
        cLib_addCalc2(&i_this->m3C34.x, local_a4->x, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.y, local_a4->y, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.z, local_a4->z, 0.1f, 100.0f);
        if (i_this->m3C20 > 0x3c) {
            i_this->m3C1E = DEMO_END;
        }
        break;
    case DEMO_INTRO_START:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            bwdCameraOrderEvent(actor, dEvtFlag_STAFF_ALL_e, 0xFFFF, 0);
            eventInfo_onCondition(actor, dEvtCnd_UNK2_e);
            return;
        }
        bwdCameraStop(&camera->mCamera);
        bwdCameraTrim(&camera->mCamera, 2);
        i_this->m3C1E = DEMO_INTRO_SHAKE;
        i_this->m3C20 = 0;
        i_this->m3C22 = 0;
        i_this->m3C48 = 55.0f;
        dComIfGp_get();
        i_this->m3C44 = 0.0f;
        player2->mDemo.setDemoType(daPy_demo_c::TYPE_ORIGINAL_e);
        player2->mDemo.setParam0(0);
        mDoAud_bgmStreamPlay();
        i_this->m3C1C = 100;
    case DEMO_INTRO_SHAKE:
        // HD: retuned intro framing uses REG10 offsets, including the shared Y/Z register.
        i_this->m3C34.set(REG10_F(0) + 50.0f, REG10_F(1) + 500.0f, REG10_F(2) + 1000.0f);
        i_this->m3C28.set(REG10_F(3) + 200.0f, REG10_F(4) + 488.0f, REG10_F(4) + 3575.0f);
        local_98->set(0.0f, player->current.pos.y, REG0_F(4) + 3000.0f);
        player2->setPlayerPosAndAngle(local_98.get(), -0x8000);
        if (i_this->m3C20 >= 0x1E) {
            if (i_this->m3C20 == 0x1E) {
                startDemoQuake();
            }
            i_this->m3C4C = REG10_F(5) + 5.0f;
            if (i_this->m3C20 == 0x32) {
                player2->mDemo.setDemoMode(daPy_demo_c::DEMO_UNK_028_e);
            }
        }
        if ((int)i_this->m3C22 != REG0_S(3) + 0xaa) {
            break;
        }
        i_this->m3C1E = DEMO_INTRO_SAND;
        i_this->m3C20 = 0;
        i_this->m3C15 = 1;
    case DEMO_INTRO_SAND:
        i_this->m3C4C = REG10_F(6) + 1.5f;
        i_this->m1864 = 10;
        if (i_this->m3C20 >= 0x96) {
            if (i_this->m3C20 == 0x96) {
                i_this->m3C34.set(25.0f, REG0_F(8) + 597.0f, 1578.0f);
            }
            i_this->m3C34.x -= (REG0_F(4) + 0.5f);
            i_this->m3C28.x = -156.0f;
            i_this->m3C28.y = 767.0f;
            i_this->m3C28.z = 2148.0f;
        } else {
            if (i_this->m3C20 == 0x14) {
                i_this->m17E4[1] = 1;
                for (int i = 3; i < 6; i++) {
                    mDoAud_seStart(JA_SE_OBJ_BWD_SANDFALL_S, &suna_gr_pos()[i], 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
            }
            i_this->m3C34.x = -1110.0f;
            i_this->m3C34.y = 1598.0f;
            i_this->m3C34.z = -45.0f;
            i_this->m3C28.x = -4426.0f;
            i_this->m3C28.y = 2574.0f;
            i_this->m3C28.z = 888.0f;
            // fallthrough
        }
        if ((int)i_this->m3C22 != REG0_S(5) + 0x280) {
            break;
        }
        i_this->m3C1E = DEMO_INTRO_SURPRISE;
        i_this->m3C20 = 0;
        i_this->m3C48 = 45.0f;
        i_this->m17E0 = 900.0f;
        local_98->set(player->current.pos.x, player->current.pos.y + 400.0f, player->current.pos.z);
        player2->setPlayerPosAndAngle(local_98.get(), -0x8000);
        // fallthrough
    case DEMO_INTRO_SURPRISE:
        if ((i_this->m3C20 >= 0x46) && (i_this->m3C20 < (s16)(REG0_S(8) + 0x5a))) {
            // HD: the surprised pose begins at frame 83 (GC: 85).
            if (i_this->m3C20 == 0x53) {
                player2->mDemo.setDemoMode(daPy_demo_c::DEMO_S_SURP_e);
            }
            i_this->m3C34.set(player->current.pos.x, (player->eyePos.y + REG0_F(6)) - 30.0f,
                              player->current.pos.z);
            i_this->m3C28.set(player->current.pos.x + REG0_F(7),
                              (player->eyePos.y + REG0_F(8)) - 30.0f,
                              (player->current.pos.z - 200.0f) + REG0_F(9));
        } else {
            i_this->m3C34.x = 6.0f;
            i_this->m3C34.y = 1166.0f;
            i_this->m3C34.z = -319.0f;
            i_this->m3C28.x = 180.0f;
            i_this->m3C28.y = 1362.0f;
            i_this->m3C28.z = 3253.0f;
            cLib_addCalc2(&i_this->m3C48, 55.0f, 0.1f, 0.1f);
        }
        if (i_this->m3C22 < 0x2ee) {
            i_this->m3C4C = REG10_F(7) + 2.0f;
        }
        if ((int)i_this->m3C22 != REG0_S(3) + 0x2f8) {
            break;
        }
        i_this->m3C1E = DEMO_INTRO_REVEAL;
        i_this->m3C20 = 0;
        bwdCameraStopQuake(dComIfGp_getVibration(), -1);
        i_this->m3C1C = 200;
        // fallthrough
    case DEMO_INTRO_REVEAL:
        if (i_this->m3C1C > 2) {
            cLib_addCalcAngleS2(&i_this->m3C1C, 1, 1, 1);
        }
        cLib_addCalc2(&i_this->m3C34.x, actor->current.pos.x, 0.1f, 100.0f);
        local_98->y = i_this->mTongueSegments[0].m04.y + REG0_F(10) + 200.0f;
        fVar2 = (REG0_F(12) + 100.0f) + l_HIO.m28;
        if (local_98->y < fVar2) {
            local_98->y = fVar2;
        }
        cLib_addCalc2(&i_this->m3C34.y, local_98->y, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.z, actor->current.pos.z, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C28.x, actor->current.pos.x * 0.5f, 0.05f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.y, l_HIO.m28 + 100.0f + REG0_F(15) + 300.0f, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.z, (actor->current.pos.z + 2000.0f) + REG0_F(9), 0.05f, 50.0f);
        // HD: widen the late reveal and display its field of view.
        if (i_this->m3C20 > 550) {
            cLib_addCalc2(&i_this->m3C48, 65.0f, 0.05f, 0.1f);
            bwdCameraReportFloat(30, 360, "K FOVY  %f", (f32)i_this->m3C48);
        }
        if ((int)i_this->m3C22 == REG0_S(4) + 0x5b4) {
            i_this->m3C1E = DEMO_END;
            if (dComIfGp_getStartStageName0() == 'X') {
                bwdCameraBgmStart(JA_BGM_PAST_RANE);
            } else {
                bwdCameraBgmStart(JA_BGM_RANE_BATTLE);
            }
            i_this->m3C50 = 1;
            bwdCameraOnDungeonItem(getStageMemory(), dSv_memBit_c::STAGE_BOSS_DEMO);
        }
        break;
    case DEMO_DEATH_START:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            bwdCameraOrderEvent(actor, dEvtFlag_STAFF_ALL_e, 0xFFFF, 0);
            eventInfo_onCondition(actor, dEvtCnd_UNK2_e);
            return;
        }
        bwdCameraStop(&camera->mCamera);
        bwdCameraTrim(&camera->mCamera, 2);
        i_this->m3C1E = DEMO_DEATH_TRACK;
        i_this->m3C20 = 0;
        i_this->m3C22 = 0;
        i_this->m3C48 = 55.0f;
        camera2 = bwd_camera::getCamera(0);
        i_this->m3C28.copy(camera2->view.mLookat.mEye);
        i_this->m3C34.copy(camera2->view.mLookat.mCenter);
        i_this->m3C44 = 0.0f;
        player2->mDemo.setDemoType(daPy_demo_c::TYPE_ORIGINAL_e);
        player2->mDemo.setParam0(0);
        bwd_camera::getSetPosition()->x = actor->current.pos.x - 100.0f;
        bwd_camera::getSetPosition()->y = player->current.pos.y;
        bwd_camera::getSetPosition()->z = actor->current.pos.z + 777.0f;
        i_this->m3C1C = 0x96;
        // fallthrough
    case DEMO_DEATH_TRACK: {
        gabi::Local<dBgS_GndChk> gndChk;
        dBgS_GndChk_ct(gndChk.get(), BD_GNDCHK_VT, false);
        auto* pos = &gndChk->m_pos;
        pos->copy(*bwd_camera::getSetPosition());
        pos->y += 1000.0f;
        fVar1 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
        if (fVar1 != -1.0e9f) {
            bwd_camera::getSetPosition()->y = fVar1;
        } else {
            bwd_camera::getSetPosition()->y = 3000.0f;
        }
        player2->setPlayerPosAndAngle(bwd_camera::getSetPosition(), -0x8000);
        bd_GndChk_dt(gndChk.get());
    }
        cLib_addCalc2(&i_this->m3C34.x, actor->current.pos.x, 0.1f, 100.0f);
        local_98->y = i_this->mTongueSegments[0].m04.y + REG0_F(10) + 200.0f;
        fVar2 = (REG0_F(12) + 100.0f) + l_HIO.m28;
        if (local_98->y < fVar2) {
            local_98->y = fVar2;
        }
        cLib_addCalc2(&i_this->m3C34.y, local_98->y, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.z, actor->current.pos.z, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C28.x, actor->current.pos.x, 0.05f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.y, l_HIO.m28 + 100.0f + REG0_F(15) + 300.0f, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.z, (actor->current.pos.z + 2200.0f) + REG0_F(9), 0.05f, 50.0f);
        cLib_addCalcAngleS2(&i_this->m3C1C, 1, 1, 1);
        break;
    case DEMO_DEATH_SEGMENT:
        cLib_addCalc2(&i_this->m3C34.x, i_this->m0418[i_this->m3C24].x, 0.1f,
                      i_this->m3C44 * 200.0f);
        cLib_addCalc2(&i_this->m3C34.y, i_this->m0418[i_this->m3C24].y, 0.1f,
                      i_this->m3C44 * 200.0f);
        cLib_addCalc2(&i_this->m3C34.z, i_this->m0418[i_this->m3C24].z, 0.1f,
                      i_this->m3C44 * 200.0f);
        cLib_addCalc2(&i_this->m3C44, 1.0f, 1.0f, 0.02f);
        if (i_this->m3C20 == 0x32) {
            player2->mDemo.setDemoMode(daPy_demo_c::DEMO_LOOKUP_e);
        }
        break;
    case DEMO_HEART:
        i_this->m3C48 = REG0_F(6) + 55.0f;
        if (i_this->m3C20 == 0x14) {
            local_b0->x = 1.0f;
            local_b0->y = 1.0f;
            local_b0->z = 1.0f;
            i_this->m18E0 = bwdCameraCreateItem(&i_this->m18E4, 0, fopAcM_GetRoomNo(actor), NULL,
                                                local_b0.get(), 1);
        }
        if (i_this->m3C20 >= 0x2C) {
            i_this->m3C1E = DEMO_HEART_PLAYER;
            i_this->m3C20 = 0;
        }
        break;
    case DEMO_HEART_PLAYER:
        i_this->m3C28.x = (player->current.pos.x - 300.0f) + REG0_F(8);
        i_this->m3C28.y = ((player->current.pos.y - 50.0f) + REG0_F(9)) + 200.0f;
        i_this->m3C28.z = (player->current.pos.z - 600.0f) + REG0_F(10);
        i_this->m3C34.x = player->current.pos.x + REG0_F(11);
        i_this->m3C34.y = player->current.pos.y + REG0_F(12);
        i_this->m3C34.z = (player->current.pos.z - 350.0f) + REG0_F(13);
        pfVar7 = fopAcM_SearchByID(i_this->m18E0);
        if ((i_this->m3C20 < 10) && (pfVar7 != NULL)) {
            pfVar7->current.pos.set(player->current.pos.x, player->current.pos.y + 300.0f,
                                    player->current.pos.z - 350.0f);
            pfVar7->speed.set(0, 0, 0);
        }
        // HD: independently tunable smile and recovery timings.
        if ((s32)i_this->m3C20 == REG_S(10, 2) + 15) {
            player2->mDemo.setDemoMode(daPy_demo_c::DEMO_SMILE_e);
        }
        if ((s32)i_this->m3C20 == REG_S(10, 3) + 53) {
            player2->mDemo.setDemoMode(daPy_demo_c::DEMO_UNK_029_e);
        }
        if ((i_this->m3C20 >= 0x78) && (REG0_S(3) == 0)) {
            i_this->m3C1E = DEMO_WARP;
            i_this->m3C20 = 0;
            actor->current.angle.y = 0;
            i_this->m17C8.x = 0.0f;
            i_this->m17C8.z = 0.0f;
            i_this->m17C4 = 0xca;
            i_this->m1865 = 0;
            i_this->m3C15 = 5;
            i_this->m3C1C = 100;
        }
        break;
    case DEMO_WARP:
        if (i_this->m3C20 == 0x32) {
            i_this->m17E4[0]++;
        }
        if (i_this->m3C20 == 0x6e) {
            i_this->m17E4[1]++;
        }
        cLib_addCalc2(&i_this->m17E0, REG0_F(7) + 400.0f, 0.1f, 1.0f);
        i_this->m3C34.x = 0.0f;
        i_this->m3C34.z = 0.0f;
        i_this->m3C34.y = REG0_F(12) + 200.0f;
        actor->current.angle.y = actor->current.angle.y + gabi::ftoi(i_this->m3C44);
        if ((int)i_this->m3C20 < REG0_S(6) + 400) {
            cLib_addCalc2(&i_this->m3C44, 40.0f, 1.0f, 0.5f);
        }
        if ((int)i_this->m3C20 > REG0_S(6) + 600) {
            cLib_addCalc2(&i_this->m3C44, 0.0f, 1.0f, 0.5f);
        }
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        local_98->set(0.0f, REG0_F(13) + 1200.0f, REG0_F(14) + 2600.0f);
        MtxPosition(local_98.get(), &i_this->m3C28);
        if ((int)i_this->m3C20 == REG0_S(6) + 0x208) {
            i_this->m17C4 = 200;
            bwdCameraOnDungeonItem(getStageMemory(), dSv_memBit_c::STAGE_BOSS_ENEMY);
            local_98->x = 0.0f;
            local_98->y = 300.0f;
            local_98->z = 0.0f;
            bwdCameraCreateWarp(local_98.get(), 0, fopAcM_GetRoomNo(actor), 0);
            i_this->m3C50 = 0;
        }
        if ((int)i_this->m3C20 != REG0_S(6) + 0x2a8) {
            break;
        }
        i_this->m3C1E = DEMO_END;
    case DEMO_END:
        dComIfGp_event_reset();
        i_this->m3C1E = DEMO_IDLE;
        {
            gabi::Local<cXyz> resetCenter;
            gabi::Local<cXyz> resetEye;
            resetCenter->copy(i_this->m3C34);
            resetEye->copy(i_this->m3C28);
            bwdCameraReset(&camera->mCamera, resetCenter.get(), resetEye.get());
        }
        bwdCameraStart(&camera->mCamera);
        bwdCameraTrim(&camera->mCamera, 0);
        i_this->m3C1C = 1;
        break;
    default:
        break;
    }
    if (i_this->m3C1E != 0) {
        s16 shakeTimer = i_this->m3C20;
        f32 shakeAmount = i_this->m3C4C;
        f32 fx = cM_ssin(shakeTimer * 0x3300) * shakeAmount;
        f32 fy = cM_scos(shakeTimer * 0x3000) * shakeAmount;
        // HD: all three axes shake, with the center moving opposite the eye.
        f32 fz = cM_ssin(shakeTimer * 0x3700) * shakeAmount;
        f32 eyeX = i_this->m3C28.x + fx;
        f32 eyeY = i_this->m3C28.y + fy;
        f32 eyeZ = i_this->m3C28.z + fz;
        f32 centerX = i_this->m3C34.x - fx;
        f32 centerY = i_this->m3C34.y - fy;
        f32 centerZ = i_this->m3C34.z - fz;
        sVar3 = 0;
        if (shakeAmount > 0.1f) {
            f32 cosine = cM_scos(shakeTimer * 0x1C00);
            f32 randomBank = cM_rndF(20.0f);
            sVar3 = (s16)gabi::ftoi(cosine * ((REG10_F(8) + 50.0f) + randomBank));
        }
        local_e0->set(centerX, centerY, centerZ);
        local_d4->set(eyeX, eyeY, eyeZ);
        bwdCameraSet(&camera->mCamera, local_e0.get(), local_d4.get(), sVar3, (f32)i_this->m3C48);
        cLib_addCalc0(&i_this->m3C4C, 1.0f, REG0_F(16) + 2.0f);
        bwdCameraReportInt(30, 390, "K DEMO MODE   %d", (s32)i_this->m3C1E);
        bwdCameraReportInt(30, 410, "K MAIN COUNT  %d", (s32)i_this->m3C22);
        bwdCameraReportInt(30, 430, "K SUB  COUNT  %d", (s32)i_this->m3C20);
        i_this->m3C20++;
        i_this->m3C22++;
    }
    if ((i_this->m3C1E >= 0x32) && (i_this->m3C1E < 100)) {
        if ((i_this->m3C22 > 0x28) && (i_this->m3C22 < 0x2da)) {
            mDoAud_seStart(JA_SE_ATM_JINARI, NULL, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        if ((i_this->m3C22 > 0x154) && (i_this->m3C22 < 700)) {
            mDoAud_seStart(JA_SE_ATM_BWD_SAND_FLOW, center_pos, 0,
                           dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
    }
    if (((i_this->m3C1E >= 100) && (i_this->m3C22 >= 700)) && (i_this->m3C22 <= 0x4b0)) {
        mDoAud_seStart(JA_SE_ATM_BWD_SAND_EBB, center_pos, 0,
                       dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
    }
}

/* WWHD glue
ext 02515280 void bwdCameraTrim(bwd_camera::CameraController*, s32)
ext 025E18EC void bwdCameraBgmStart(u32)
ext 025CB408 void bwdCameraStartQuake(void*, s32, s32, cXyz*)
ext 025D8A5C u32 bwdCameraCreateItem(cXyz*, s32, s32, csXyz*, cXyz*, s32)
ext 025D9874 void bwdCameraCreateWarp(cXyz*, csXyz*, s32, s32)
ext 025D7B24 void bwdCameraOrderEvent(fopAc_ac_c*, s32, s32, s32)
ext 02514F2C void bwdCameraStop(bwd_camera::CameraController*)
ext 028E8D88 void bwdCameraVectorAdd(cXyz*, cXyz*, cXyz*)
ext 025CB610 void bwdCameraStopQuake(void*, s32)
ext 025B9098 void bwdCameraOnDungeonItem(void*, s32)
ext 0251510C void bwdCameraReset(bwd_camera::CameraController*, cXyz*, cXyz*)
ext 02514F38 void bwdCameraStart(bwd_camera::CameraController*)
ext 02514FE8 void bwdCameraSet(bwd_camera::CameraController*, cXyz*, cXyz*, s16, f32)
ext 027EC9E8 void bwdCameraReportFloat(s32, s32, const char*, f32)
ext 027EC9E8 void bwdCameraReportInt(s32, s32, const char*, s32)
str 1000BE7C "K FOVY  %f"
str 1000BE88 "K DEMO MODE   %d"
str 1000BE9C "K MAIN COUNT  %d"
str 1000BEB0 "K SUB  COUNT  %d"
*/
