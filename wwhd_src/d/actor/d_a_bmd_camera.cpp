/**
 * d_a_bmd_camera.cpp (WWHD)
 * Boss - Kalle Demos: demo_camera() (020B3BF8), the boss's cut-scene camera.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bmd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source (several camera
 * positions moved and gained REG10_F debug offsets; the demo counters are printed with three
 * JUTReport calls).
 */
#include "d/actor/d_a_bmd.h"

#define ARC_BMD STR(0x10009A98) /* "Bmd" */

static inline f32 REG10_F(int i) { return REG_F(10, i); }

/* camera_process_class (HD): dCamera_c mCamera at +0x248; view.mLookat eye at +0xDC, center at +0xE8 */
static inline u32 dComIfGp_getCamera_bl(s32 id) { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + id * 0x34); }
static inline s32 dComIfGp_getPlayerCameraID0() { return gabi::load<s8>(dComIfGp_ea() + 0x5B30); }
static inline void cam_Stop(u32 cam) { gabi::call(0x02514F2C, cam + 0x248); }
static inline void cam_Start(u32 cam) { gabi::call(0x02514F38, cam + 0x248); }
static inline void cam_SetTrimSize(u32 cam, s32 n) { gabi::call(0x02515280, cam + 0x248, n); }
static inline void cam_Reset(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, cam + 0x248, center, eye); }
static inline void cam_Set(u32 cam, cXyz* center, cXyz* eye, s16 bank, f32 fovy) { gabi::call(0x02514FE8, cam + 0x248, center, eye, bank, fovy); }

/* daPy_py_c (HD) */
static inline void player_changeOriginalDemo(fopAc_ac_c* p) {
    gabi::store<s16>(gabi::ea(p) + 0x420, 3);
    gabi::store<u32>(gabi::ea(p) + 0x428, 0);
}
static inline void player_cancelOriginalDemo(fopAc_ac_c* p) {
    gabi::store<s16>(gabi::ea(p) + 0x420, 2);
    gabi::store<u32>(gabi::ea(p) + 0x430, 1);
}
static inline void player_changeDemoMode(fopAc_ac_c* p, u32 mode) { gabi::store<u32>(gabi::ea(p) + 0x430, mode); }
static inline void player_voiceStart(fopAc_ac_c* p, u32 id) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 0xB4) + 0xE4);
    gabi::call_ptr(fn, p, id);
}
static inline void player_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, s16 angle) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 0xB4) + 0x114);
    gabi::call_ptr(fn, p, pos, angle);
}
static inline void head_setAnm(bmd_class* i_this, s32 idx, s32 mode, f32 morf) {
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(ARC_BMD, idx, SAFESTRING_VTBL);
    i_this->mpHeadMorf->setAnm(anm, mode, morf, 1.0f, 0.0f, -1.0f, nullptr);
}
/* fopAcM_monsSeStart, with the inline's actor check folded (the actor was dereferenced) */
static inline void monsSeStart_e(fopAc_ac_c* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, 0, reverb);
    }
}
/* the camera's view (eye, center) copied word by word */
static inline void copy_from(cXyz* dst, u32 src) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(gabi::ea(dst) + i, gabi::load<u32>(src + i));
}

/* the demo's start: event order, then take the camera */
static inline bool order_event(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (!eventInfo_checkCommandDemoAccrpt(actor)) {
        gabi::call(0x025D7B24, actor, 2 /* dEvtFlag_STAFF_ALL_e */, 0xFFFF, 0); /* fopAcM_orderPotentialEvent */
        i_this->mB76 = 0;
        i_this->mB78 = 0;
        eventInfo_onCondition(actor, 2 /* dEvtCnd_UNK2_e */);
        return false;
    }
    return true;
}

/* 020B3BF8 */
void demo_camera(bmd_class* i_this) {
    WWHD_FUNC(0x020B3BF8, void, i_this);
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_44;
    gabi::Local<cXyz> local_50;

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 camId = dComIfGp_getPlayerCameraID0();
    u32 camera = dComIfGp_getCamera_bl(camId);

    switch (i_this->mB74) {
    case 0:
        break;
    case 1: {
        if (!order_event(i_this))
            return;
        i_this->mB74 = i_this->mB74 + 1;
        u32 camera2 = dComIfGp_getCamera_bl(0);
        copy_from(&i_this->mB7C, camera2 + 0xDC);
        copy_from(&i_this->mB88, camera2 + 0xE8);
        {
            /* HD: the angle from the camera to the boss (GameCube: to the player) */
            gabi::Local<cXyz> tmp;
            cXyz_mi(&actor->current.pos, tmp, &i_this->mB7C);
            local_44->x = tmp->x;
            local_44->y = tmp->y;
            local_44->z = tmp->z;
        }
        i_this->mB96 = cM_atan2s(local_44->x, local_44->z);
        cam_Stop(camera);
        cam_SetTrimSize(camera, 2);
        i_this->mB9C = 55.0f;
        player_changeOriginalDemo(player);
        /* HD: Link is put at the boss */
        player_setPlayerPosAndAngle(player, &actor->current.pos, i_this->mB96);
    }
        /* fallthrough */
    case 2:
        cMtx_YrotS(calc_mtx(), i_this->mB96);
        if ((s32)i_this->mB78 < REG0_S(0) + 0x55) {
            cLib_addCalc2(&i_this->mB88.x, actor->current.pos.x, 0.1f, 200.0f);
            cLib_addCalc2(&i_this->mB88.y, player->current.pos.y + 300.0f + REG0_F(5), 0.2f, 200.0f);
            cLib_addCalc2(&i_this->mB88.z, actor->current.pos.z, 0.2f, 200.0f);
            local_44->x = 0.0f;
            local_44->y = REG0_F(7) + 100.0f;
            local_44->z = REG0_F(6) - 1100.0f;
            MtxPosition(local_44, local_50);
            cLib_addCalc2(&i_this->mB7C.x, actor->current.pos.x + local_50->x, 0.1f, 200.0f);
            cLib_addCalc2(&i_this->mB7C.y, player->current.pos.y + local_50->y, 0.1f, 200.0f);
            cLib_addCalc2(&i_this->mB7C.z, actor->current.pos.z + local_50->z, 0.1f, 200.0f);
        } else if ((s32)i_this->mB78 < REG0_S(1) + 100) {
            cLib_addCalc2(&i_this->mB88.y, actor->current.pos.y + 1200.0f + REG0_F(5), 0.2f, REG0_F(6) + 100.0f);
        } else {
            cLib_addCalc2(&i_this->mB88.x, player->current.pos.x, 0.3f, 300.0f);
            cLib_addCalc2(&i_this->mB88.y, player->current.pos.y + 70.0f + REG0_F(5), 0.3f, 300.0f);
            cLib_addCalc2(&i_this->mB88.z, player->current.pos.z, 0.3f, 300.0f);
        }
        break;
    case 5:
        if (!order_event(i_this))
            break;
        i_this->mB74 = i_this->mB74 + 1;
        dComIfGp_get(); /* HD: an unused dComIfGp_getCamera(0) */
        cam_Stop(camera);
        cam_SetTrimSize(camera, 2);
        i_this->mB9C = 60.0f;
        player_changeOriginalDemo(player);
        /* fallthrough */
    case 6:
        /* HD: positions changed, with REG10_F offsets */
        i_this->mB7C.x = REG10_F(0) + 24.0f;
        i_this->mB7C.y = REG10_F(1) + 133.0f;
        i_this->mB7C.z = REG10_F(2) + 745.0f;
        i_this->mB88.x = REG10_F(3) + -37.0f;
        i_this->mB88.y = REG10_F(4) + 88.0f;
        i_this->mB88.z = REG10_F(5) + 102.0f;
        local_50->y = 0.0f;
        local_50->x = REG10_F(6) + 61.0f;
        local_50->z = REG10_F(7) + 492.0f;
        player_setPlayerPosAndAngle(player, local_50, -0x75FB);
        if (i_this->mB78 == 0x1E) {
            i_this->m2DC = 3;
            i_this->mB78 = 0;
            i_this->mB74 = i_this->mB74 + 1;
        }
        break;
    case 7:
        i_this->mB7C.x = -47.0f;
        i_this->mB7C.y = 115.0f;
        i_this->mB7C.z = 263.0f;
        i_this->mB88.x = -199.0f;
        i_this->mB88.y = -89.0f;
        i_this->mB88.z = -349.0f;
        if (i_this->mB78 == 0x1E) {
            i_this->mB78 = 0;
            i_this->mB74 = i_this->mB74 + 1;
        }
        break;
    case 8:
        i_this->mB7C.x = 2.0f;
        i_this->mB7C.y = 75.0f;
        i_this->mB7C.z = 403.0f;
        i_this->mB88.x = 401.0f;
        i_this->mB88.y = 228.0f;
        i_this->mB88.z = 957.0f;
        if (i_this->mB78 == 5) {
            player_changeDemoMode(player, 0x32 /* DEMO_SMILE_e */);
            player_voiceStart(player, 0x2E);
        }
        if (i_this->mB78 != 0x1E) {
            break;
        }
        i_this->mB78 = 0;
        i_this->mB74 = i_this->mB74 + 1;
        i_this->mB7C.x = -47.0f;
        i_this->mB7C.y = 115.0f;
        i_this->mB7C.z = 263.0f;
        cXyz_fcopy(&i_this->mB88, &actor->eyePos);
        i_this->mB88.y = i_this->mB88.y + (REG0_F(11) + 30.0f);
        i_this->m2DC = 4;
        /* fallthrough */
    case 9:
        cLib_addCalc2(&i_this->mB88.y, actor->eyePos.y + 30.0f, 0.2f, REG0_F(4) + 5.0f);
        if (i_this->mB78 == 0x1E) {
            head_setAnm(i_this, 0x22 /* NEW_SDEMO1 */, 0, 1.0f);
            if (gabi::ea(&actor->eyePos) != 0) {
                mDoAud_seStart(0x584A /* JA_SE_CM_BKM_CORE_ENTER */, &actor->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            }
            monsSeStart_e(actor, 0x485A /* JA_SE_CV_BKM_ENTER */);
        }
        if (i_this->mB78 > 0x1E) {
            if (i_this->mpHeadMorf->isStop()) {
                i_this->mB78 = 0;
                i_this->mB74 = i_this->mB74 + 1;
                head_setAnm(i_this, 0x23 /* NEW_SDEMO2 */, 2, 1.0f);
            }
        }
        break;
    case 10:
        i_this->mB7C.x = 2.0f;
        i_this->mB7C.y = 75.0f;
        i_this->mB7C.z = 403.0f;
        i_this->mB88.x = 401.0f;
        i_this->mB88.y = 228.0f;
        i_this->mB88.z = 957.0f;
        if (i_this->mB78 == 5) {
            player_changeDemoMode(player, 0x31 /* DEMO_S_SURP_e */);
            player_voiceStart(player, 0x1D);
        }
        if (i_this->mB78 != 0x19) {
            break;
        }
        i_this->mB78 = 0;
        i_this->mB74 = i_this->mB74 + 1;
        /* HD: no height offset (overwritten below anyway) */
        cXyz_fcopy(&i_this->mB88, &actor->eyePos);
        /* fallthrough */
    case 11:
        i_this->mB7C.x = -139.0f;
        i_this->mB7C.y = 125.0f;
        i_this->mB7C.z = 475.0f;
        i_this->mB88.x = -93.0f;
        i_this->mB88.y = 164.0f;
        i_this->mB88.z = 104.0f;
        if (i_this->mB78 == 0x1E) {
            /* the morf rate is read after the resource lookup */
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(ARC_BMD, 0x24 /* NEW_SDEMO3 */, SAFESTRING_VTBL);
            i_this->mpHeadMorf->setAnm(anm, 0, REG0_F(16) + 4.0f, 1.0f, 0.0f, -1.0f, nullptr);
        }
        if (i_this->mB78 > 0x1B) {
            if (i_this->mpHeadMorf->isStop()) {
                i_this->mB78 = 0;
                i_this->m2DC = 0;
                i_this->mB74 = i_this->mB74 + 1;
                mk_voice_set(i_this, 0x48C6);
                head_setAnm(i_this, 0x25 /* NEW_SDEMO4 */, 0, 1.0f);
                cXyz_fcopy(&i_this->mB88, &actor->eyePos);
                i_this->mB88.y = i_this->mB88.y + (REG0_F(15) + 60.0f);
            }
        }
        break;
    case 12:
        if ((i_this->mB78 >= 0x14) && (i_this->mB78 <= 0x28)) {
            i_this->mB7C.x = 2.0f;
            i_this->mB7C.y = 75.0f;
            i_this->mB7C.z = 403.0f;
            i_this->mB88.x = 401.0f;
            i_this->mB88.y = 228.0f;
            i_this->mB88.z = 957.0f;
            if (i_this->mB78 == 0x14) {
                player_changeDemoMode(player, 0x18 /* DEMO_SURPRISED_e */);
                player_voiceStart(player, 0x31);
            }
            i_this->mpHeadMorf->setPlaySpeed(0.0f);
            if (i_this->mB78 == 0x28) {
                i_this->mB7C.x = -139.0f;
                cXyz_fcopy(&i_this->mB88, &actor->eyePos);
                i_this->mB7C.y = 125.0f;
                i_this->mB7C.z = 475.0f;
                i_this->mB88.y = i_this->mB88.y + (REG0_F(15) + 60.0f);
                i_this->mpHeadMorf->setPlaySpeed(1.0f);
            }
        } else {
            cLib_addCalc2(&i_this->mB9C, REG0_F(12) + 35.0f, 0.2f, REG0_F(13) + 1.0f);
            cLib_addCalc2(&i_this->mB88.x, actor->eyePos.x, 0.2f, REG0_F(4) + 10.0f);
            cLib_addCalc2(&i_this->mB88.y, actor->eyePos.y + 10.0f + REG0_F(14), 0.2f, REG0_F(4) + 10.0f);
            cLib_addCalc2(&i_this->mB88.z, actor->eyePos.z, 0.2f, REG0_F(4) + 10.0f);
            if (i_this->mpHeadMorf->isStop()) {
                i_this->m302 = 3;
                i_this->mB78 = REG0_S(6);
                head_setAnm(i_this, 0x13 /* COA_START3 */, 2, 1.0f);
                i_this->m308[0] = 10;
                monsSeStart_e(actor, 0x485B /* JA_SE_CV_BKM_LAUGH */);
                i_this->mB74 = 0xF;
                i_this->mB9C = REG0_F(8) + 55.0f;
                i_this->mB76 = REG0_S(7) + 0x6E;
                /* HD: fixed positions (GameCube: around the boss) */
                i_this->mB7C.x = REG10_F(0) + -50.0f;
                i_this->mB7C.y = REG10_F(1) + 150.0f;
                i_this->mB7C.z = REG10_F(2) + 1520.0f;
                i_this->mB88.x = -106.0f;
                i_this->mB88.y = 500.0f;
                i_this->mB88.z = 63.0f;
                i_this->mBA0 = 0.0f;
            }
        }
        break;
    case 15:
        if ((s32)i_this->mB78 == REG0_S(6) + 0x37) {
            player_changeDemoMode(player, 0x18 /* DEMO_SURPRISED_e */);
        }
        if (i_this->mB78 == 0x96) {
            player_changeDemoMode(player, 0x1A /* DEMO_LOOKUP_e */);
        }
        if ((s32)i_this->mB78 >= REG0_S(5) + 0xD2) {
            if ((s32)i_this->mB78 <= REG0_S(6) + 0x159) {
                cLib_addCalc2(&i_this->mB9C, REG0_F(6) + 40.0f, 0.05f, REG0_F(7) + 0.0249999985f /* 0x3CCCCCCC */);
            } else {
                /* HD: a blur and a voice */
                if ((s32)i_this->mB78 == REG0_S(6) + 0x15A) {
                    i_this->mB72 = 0x96;
                }
                if ((s32)i_this->mB78 == (s16)(REG0_S(7) + 0x168)) {
                    player_changeDemoMode(player, 0x18 /* DEMO_SURPRISED_e */);
                    player_voiceStart(player, 0x1D);
                }
                cLib_addCalcAngleS2(&i_this->mB72, 1, 1, 1);
                cLib_addCalc2(&i_this->mB9C, REG0_F(8) + 70.0f, 0.2f, REG0_F(9) + 2.0f);
            }
        }
        if ((s32)i_this->mB78 >= REG0_S(5) + 0x96) {
            /* HD: the eye moves in x and z (GameCube: y) */
            cLib_addCalc2(&i_this->mB88.y, REG0_F(3) + 750.0f, 0.05f, 4.5f * i_this->mBA0);
            cLib_addCalc2(&i_this->mB7C.x, REG10_F(0) + 950.0f, 0.05f, 5.0f * i_this->mBA0);
            cLib_addCalc2(&i_this->mB7C.z, REG10_F(2) + 920.0f, 0.05f, 3.0f * i_this->mBA0);
            cLib_addCalc2(&i_this->mBA0, 1.0f, 1.0f, REG0_F(10) + 0.01f);
        }
        if (i_this->mB76 == 0x24E) {
            i_this->mB74 = 0x96;
            dComIfGp_event_reset();
            i_this->m332 = 7;
            i_this->mMode = 0;
            i_this->m302 = -1;
            i_this->mBE0 = 0;
            i_this->m308[0] = 0x3C;
            if (dComIfGp_getStartStageName0() == 'X') {
                mDoAud_bgmStart(0x8000004A /* JA_BGM_PAST_BKM */);
            } else {
                mDoAud_bgmStart(0x80000005 /* JA_BGM_KINDAN_BOSS */);
            }
            i_this->mB71 = 1;
            i_this->mB72 = 1; /* HD */
            dComIfGs_onDungeonItem_bl(5); /* dComIfGs_onStageBossDemo */
        }
        break;
    case 100: {
        if (!order_event(i_this))
            break;
        i_this->mB74 = i_this->mB74 + 1;
        u32 camera2 = dComIfGp_getCamera_bl(0);
        copy_from(&i_this->mB7C, camera2 + 0xDC);
        copy_from(&i_this->mB88, camera2 + 0xE8);
        cam_Stop(camera);
        cam_SetTrimSize(camera, 2);
        i_this->mB9C = 55.0f;
        player_changeOriginalDemo(player);
    }
        /* fallthrough */
    case 101:
        if ((i_this->mB78 >= 0x140) && (cLib_addCalc2(&i_this->mB9C, REG0_F(8) + 85.0f, 0.2f, 1.0f), i_this->mB78 == 0x140)) {
            i_this->m2DC = 5;
        }
        if (i_this->m924.y < i_this->m328 + 10.0f) {
            cLib_addCalc2(&i_this->mB88.x, actor->eyePos.x, 0.2f, 200.0f);
            cLib_addCalc2(&i_this->mB88.y, actor->eyePos.y + 20.0f + REG0_F(5), 0.5f, 200.0f);
            cLib_addCalc2(&i_this->mB88.z, actor->eyePos.z, 0.2f, 200.0f);
        } else {
            cLib_addCalc2(&i_this->mB88.x, i_this->m924.x, 0.2f, 200.0f);
            cLib_addCalc2(&i_this->mB88.y, i_this->m924.y, 0.5f, 200.0f);
            cLib_addCalc2(&i_this->mB88.z, i_this->m924.z, 0.2f, 200.0f);
        }
        cMtx_YrotS(calc_mtx(), (actor->shape_angle.y + REG0_S(4)) + 3000);
        local_44->x = 0.0f;
        local_44->y = REG0_F(7) + 70.0f;
        local_44->z = REG0_F(6) + 1000.0f;
        MtxPosition(local_44, local_50);
        PSVECAdd(local_50, &actor->current.pos, local_50); /* local_50 += current.pos */
        if (local_50->y > i_this->m328 + 300.0f + REG0_F(8)) {
            local_50->y = i_this->m328 + 300.0f + REG0_F(8);
        }
        cLib_addCalc2(&i_this->mB7C.x, local_50->x, 0.5f, 200.0f);
        cLib_addCalc2(&i_this->mB7C.y, local_50->y, 0.8f, 200.0f);
        cLib_addCalc2(&i_this->mB7C.z, local_50->z, 0.5f, 200.0f);
        /* HD: Makar's position first */
        local_44->x = 0.0f;
        local_44->y = 0.0f;
        local_44->z = REG0_F(6) + 850.0f;
        MtxPosition(local_44, &i_this->m2E0);
        PSVECAdd(&i_this->m2E0, &actor->current.pos, &i_this->m2E0); /* m2E0 += current.pos */
        i_this->m2FA = (actor->shape_angle.y + REG0_S(4)) + 3000;
        if (i_this->mB76 > 0x3C) {
            /* HD: angles changed */
            cMtx_YrotS(calc_mtx(), (s16)((actor->shape_angle.y + REG0_S(7)) + 0x1A2C));
            local_44->y = 0.0f;
            local_44->z = 0.0f;
            local_44->x = REG0_F(11) + 200.0f;
            MtxPosition(local_44, local_50);
            PSVECAdd(local_50, &actor->current.pos, local_50);
            player_setPlayerPosAndAngle(player, local_50, actor->shape_angle.y + REG0_S(6));
        }
        break;
    case 102: {
        i_this->mB9C = REG0_F(8) + 55.0f;
        i_this->mBA4 = 1500.0f;
        i_this->mB70 = 1;
        i_this->mB96 = actor->shape_angle.y + REG0_S(2);
        i_this->mBA0 = 0.0f;
        i_this->mB94 = -8000;
        i_this->mB74 = 0x67;
        gabi::Local<csXyz> cStack_b8;
        csXyz_ct(cStack_b8, 0, 0, 0);
        cStack_b8->y = actor->shape_angle.y + 0x8BB8;
        fopAcM_create(0x14E /* fpcNm_NPC_CB1_e */, 0, &i_this->m2E0, fopAcM_GetRoomNo(actor), cStack_b8, nullptr, -1, 0);
        i_this->m2DC = 0;
    }
        /* fallthrough */
    case 103: {
        s16 r6 = (s16)gabi::ftoi(i_this->mBA0 * 22384.0f);
        cLib_addCalcAngleS2(&i_this->mB96, (actor->shape_angle.y + REG0_S(2)) + 0x5770, 0x10, r6);
        r6 = (s16)gabi::ftoi(i_this->mBA0 * 3000.0f);
        cLib_addCalcAngleS2(&i_this->mB94, -5000, 0x10, r6);
        cLib_addCalc2(&i_this->mBA4, 500.0f, 0.0625f, i_this->mBA0 * 1000.0f);
        cLib_addCalc2(&i_this->mBA0, REG0_F(10) + 0.0048f, 0.1f, 4.8e-05f);
        cMtx_YrotS(calc_mtx(), i_this->mB96);
        cMtx_XrotM(calc_mtx(), i_this->mB94);
        local_44->y = 0.0f;
        local_44->x = 0.0f;
        local_44->z = i_this->mBA4;
        MtxPosition(local_44, local_50);
        {
            gabi::Local<cXyz> tmp;
            cXyz_pl(&actor->current.pos, tmp, local_50);
            f32 y = actor->current.pos.y;
            i_this->mB88.y = y;
            i_this->mB88.x = actor->current.pos.x;
            i_this->mB88.z = actor->current.pos.z;
            i_this->mB7C.copy(*tmp);
            i_this->mB88.y = y + ((f32)REG0_S(3) + 100.0f);
        }
        /* HD: the field of view eases to 70 */
        cLib_addCalc2(&i_this->mB9C, REG10_F(12) + 70.0f, 0.05f, REG10_F(13) + 0.25f);
        if (i_this->mB76 == 800) {
            i_this->mB74 = 0x96;
            dComIfGp_event_reset();
            i_this->m314 = 0;
            player_cancelOriginalDemo(player);
        }
        break;
    }
    case 150: {
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        center->x = i_this->mB88.x;
        center->y = i_this->mB88.y;
        eye->z = i_this->mB7C.z;
        eye->x = i_this->mB7C.x;
        center->z = i_this->mB88.z;
        eye->y = i_this->mB7C.y;
        i_this->mB74 = 0;
        cam_Reset(camera, center, eye);
        cam_Start(camera);
        cam_SetTrimSize(camera, 0);
        break;
    }
    }
    if (i_this->mB74 != 0) {
        f32 f1 = i_this->mBA8 * cM_ssin(i_this->mB78 * 0x3300);
        f32 f2 = i_this->mBA8 * cM_scos(i_this->mB78 * 0x3000);
        gabi::Local<cXyz> local_b0;
        gabi::Local<cXyz> local_a4;
        local_a4->x = i_this->mB7C.x + f1;
        local_b0->x = i_this->mB88.x + f1;
        local_b0->y = i_this->mB88.y + f2;
        local_b0->z = i_this->mB88.z;
        local_a4->y = i_this->mB7C.y + f2;
        local_a4->z = i_this->mB7C.z;
        s16 iVar6 = (s16)gabi::ftoi(i_this->mBA8 * cM_scos(i_this->m2FE * 0x1C00) * 7.5f);
        cam_Set(camera, local_b0, local_a4, iVar6, i_this->mB9C);
        cLib_addCalc0(&i_this->mBA8, 1.0f, REG0_F(16) + 2.0f);
        /* HD: JUTReport(x, y, fmt, ...) prints the three counters */
        gabi::call(0x027EC9E8, 0x1E, 0x186, STR(0x10009B2C), (s32)i_this->mB74);
        gabi::call(0x027EC9E8, 0x1E, 0x19A, STR(0x10009B40), (s32)i_this->mB76);
        gabi::call(0x027EC9E8, 0x1E, 0x1AE, STR(0x10009B54), (s32)i_this->mB78);
        i_this->mB78 = i_this->mB78 + 1;
        i_this->mB76 = i_this->mB76 + 1;
    }
}
VERIFY(0x020B3BF8, demo_camera);
