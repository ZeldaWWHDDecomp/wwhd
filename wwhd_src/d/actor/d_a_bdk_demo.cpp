/**
 * d_a_bdk_demo.cpp (WWHD)
 * Boss - Helmaroc King (battle): demo_camera (the opening, mask-break and death cutscene cameras).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;
using gabi::fmuls_ppc;
using gabi::fsubs_ppc;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* debug registers by HD address (REGn indices that do not map to REG0) */
static inline f32 bdk_REGF(u32 a) { return gabi::load<f32>(a); }
static inline s16 bdk_REGS(u32 a) { return gabi::load<s16>(a); }
/* daPy_py_c demo (HD: mDemo at +0x420: type u16 +0x420, param0 +0x428, mode +0x430) */
static inline void bdk_changeOriginalDemo(fopAc_ac_c* pl) {
    gabi::store<u32>(gabi::ea(pl) + 0x428, 0);
    gabi::store<u16>(gabi::ea(pl) + 0x420, 3);
}
static inline void bdk_changeDemoMode(fopAc_ac_c* pl, u32 mode) { gabi::store<u32>(gabi::ea(pl) + 0x430, mode); }
static inline void bdk_changeDemoParam0(fopAc_ac_c* pl, s32 p) { gabi::store<s32>(gabi::ea(pl) + 0x428, p); }
/* daPy_py_c::setPlayerPosAndAngle(cXyz*, s16): virtual (vtable at +0xB4, slot +0x114) */
static inline void bdk_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 angle) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(pl) + 0xB4) + 0x114);
    gabi::call_ptr(fn, pl, pos, angle);
}
static inline void dCamera_Stop_bdk(u32 cam) { gabi::call(0x02514F2C, cam); }
static inline void dCamera_Start_bdk(u32 cam) { gabi::call(0x02514F38, cam); }
static inline void dCamera_SetTrimSize_bdk(u32 cam, s32 s) { gabi::call(0x02515280, cam, s); }
/* dCamera_c::Set(cXyz center, cXyz eye, s16 bank, f32 fovy) (by value: pointers to copies) */
static inline void dCamera_Set_bdk(u32 cam, cXyz* center, cXyz* eye, s16 bank, f32 fovy) { gabi::call(0x02514FE8, cam, center, eye, bank, fovy); }
static inline void dCamera_Reset_bdk(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, cam, center, eye); }
static inline BOOL fopAcM_orderPotentialEvent_bdk(fopAc_ac_c* a, u16 type, u16 flag, u16 p) { return gabi::call<BOOL>(0x025D7B24, a, type, flag, p); }
static inline void mDoAud_bgmStreamPrepare_bdk(u32 id) { gabi::call(0x025E1934, id); }
static inline void mDoAud_bgmStreamPlay_bdk() { gabi::call(0x025E1944); }
static inline void mDoAud_seStart_simple_bdk(u32 id) { gabi::call(0x025E1988, id); } /* HD: seStart(id) */
/* 027EC9E8 JUTReport(x, y, fmt, ...) */
static inline void JUTReport_bdk(s32 x, s32 y, u32 fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }
static inline void bdk_vbits(cXyz* d, const cXyz* s) {
    for (int k = 0; k < 12; k += 4) gabi::store<u32>(gabi::ea(d) + k, gabi::load<u32>(gabi::ea(s) + k));
}

/* demo_camera. HD: the player is placed (and turned towards the boss) in more steps, the camera
 * shakes around both points in opposite directions, no line check against the walls, three debug
 * reports, the bridge-rope objects are moved with the player at the start */
void demo_camera(bdk_class* i_this) {
    WWHD_FUNC(0x0206FEF0, void, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 camId = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + camId * 0x34);
    u32 cam = camera + 0x248; /* dCamera_c */
    gabi::Local<cXyz> local_104;
    gabi::Local<cXyz> local_108;
    gabi::Local<csXyz> local_14C;
    gabi::Local<csXyz> local_154;

    f32 dVar16 = 45.0f;
    f32 dVar15 = 1.0f;
    f32 dVar14 = 0.05f;
    u8 r27 = 0;

    switch ((u16)i_this->m25A0) {
    case 0x0:
        break;
    case 0x1:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2 /* dEvtType_OTHER_e */, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->m25A0 = i_this->m25A0 + 1;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        i_this->m25A4 = 0;
        i_this->m25A6 = 0;
        i_this->m25C8 = 45.0f;
        i_this->m8F8 = 4;
        bdk_changeOriginalDemo(player);
        /* fall through */
    case 0x2: {
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        local_104->x = fadds_ppc(REG0_F(0), -80.0f);
        local_104->y = fadds_ppc(REG0_F(1), -30.0f);
        local_104->z = fadds_ppc(REG0_F(2), 50.0f);
        MtxPosition(local_104, &i_this->m25B4);
        PSVECAdd(&i_this->m25B4, &i_this->m1150, &i_this->m25B4);

        local_104->x = fadds_ppc(REG0_F(5), 400.0f);
        local_104->y = fadds_ppc(REG0_F(6), -100.0f);
        local_104->z = fadds_ppc(REG0_F(7), 400.0f);
        MtxPosition(local_104, &i_this->m25A8);
        PSVECAdd(&i_this->m25A8, &i_this->m1150, &i_this->m25A8);

        local_104->x = fadds_ppc(REG0_F(0), -230.0f);
        local_104->y = 0.0f;
        local_104->z = fadds_ppc(REG0_F(2), 110.0f);
        MtxPosition(local_104, local_108);
        local_108->x = fadds_ppc(local_108->x, i_this->m1150.x);
        bdk_fcopy(local_108->y, player->current.pos.y);
        local_108->z = fadds_ppc(local_108->z, i_this->m1150.z);
        bdk_setPlayerPosAndAngle(player, local_108, (s16)(actor->shape_angle.y + REG0_S(6) + 20000));

        s16 r4 = i_this->m25A6;
        if (r4 > REG0_S(7) + 0x3C) {
            dVar16 = 20.0f;
            dVar15 = 1.0f;
            dVar14 = 0.5f;
        } else if (r4 > 0) {
            dVar16 = 35.0f;
            dVar15 = 0.35f;
        }
        if (r4 == 0x51) {
            bdk_changeDemoMode(player, 0x32 /* DEMO_SMILE_e */);
        }
        cLib_addCalc2(&i_this->m25C8, dVar16, dVar14, dVar15);
        break;
    }
    case 0xA:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->m25A0 = i_this->m25A0 + 1;
        i_this->m25CC = 0.0f;
        i_this->m25A4 = 0;
        i_this->m25A6 = 0;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        i_this->m25C8 = 55.0f;
        /* HD: the cutscene starts from the current camera */
        bdk_vbits(&i_this->m25B4, gabi::at<cXyz>(camera + 0xE8));
        bdk_vbits(&i_this->m25A8, gabi::at<cXyz>(camera + 0xDC));
        mDoAud_bgmStreamPrepare_bdk(0xC0000008 /* JA_STRM_DK_START */);
        bdk_changeOriginalDemo(player);
        bdk_changeDemoMode(player, 0x17); /* HD */
        bdk_changeDemoParam0(player, 2);  /* HD */
        /* fall through */
    case 0xB:
        /* HD: the player is held at the start point (and the object found by obj_s_sub moved with
         * it on the first frame); the camera points of GameCube case 0xA are set on frame 1 */
        if (i_this->m25A6 >= 0) {
            local_108->x = 2517.0f;
            bdk_fcopy(local_108->y, player->current.pos.y);
            local_108->z = -3662.0f;
            if (i_this->m25A6 == 0) {
                u32 found = gabi::ea(fpcM_Search(0x020651B8 /* obj_s_sub */, actor));
                if (found != 0) {
                    cXyz* fp = gabi::at<cXyz>(found + 0x314);
                    fp->x = fadds_ppc(fp->x, fsubs_ppc(local_108->x, player->current.pos.x));
                    fp->z = fadds_ppc(fp->z, fsubs_ppc(local_108->z, player->current.pos.z));
                }
            }
            bdk_setPlayerPosAndAngle(player, local_108, player->shape_angle.y);
            if (i_this->m25A6 == 1) {
                i_this->m25B4.x = 2598.0f;
                i_this->m25B4.y = 9902.0f;
                i_this->m25B4.z = -3498.0f;
                i_this->m25A8.x = 2228.0f;
                i_this->m25A8.y = 9961.0f;
                i_this->m25A8.z = -3403.0f;
            }
        }
        cLib_addCalc2(&i_this->m25B4.x, 164.0f, 0.05f, fmuls_ppc(24.34f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25B4.y, 11617.0f, 0.05f, fmuls_ppc(17.15f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25B4.z, -2802.0f, 0.05f, fmuls_ppc(6.96f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25A8.x, -167.0f, 0.05f, fmuls_ppc(23.949999f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25A8.y, 11791.0f, 0.05f, fmuls_ppc(18.3f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25A8.z, -2706.0f, 0.05f, fmuls_ppc(6.97f, i_this->m25CC));

        if (i_this->m25A6 > (s16)(REG0_S(1) + 0x1E)) {
            cLib_addCalc2(&i_this->m25CC, fadds_ppc(REG0_F(1), 1.0f), 1.0f, fadds_ppc(REG0_F(2), 0.015f));
        }
        if (i_this->m25A6 >= (s16)(REG0_S(6) + 0xBE)) {
            if (i_this->m25A6 == (s16)(REG0_S(6) + 0xD2)) {
                bdk_changeDemoMode(player, 0x1C /* DEMO_UNK_028_e */);
                mDoAud_bgmStreamPlay_bdk();
            }
            if (i_this->m25A6 == (s16)(REG0_S(6) + 0xBE)) {
                local_14C->x = 0;
                local_14C->z = 0;
                for (s32 i = 0; i <= 2; i++) {
                    local_14C->y = (s16)(10000 + i * 0x5555 + REG_S(8, 4));
                    bdk_particle_setToon(0xA14F /* ID_IT_ST_DK_FUTATOJI_SMOKE_A00 */, &center_pos, local_14C, nullptr, 0xB9, &i_this->m6130[i],
                                         (s8)fopAcM_GetRoomNo(actor));
                }
                bdk_particle_setToon(0xA150 /* ID_IT_ST_DK_FUTATOJI_SMOKE_B00 */, &center_pos2, nullptr, nullptr, 0xB9, &i_this->m6130[3],
                                     (s8)fopAcM_GetRoomNo(actor));
                i_this->m6100[1] = dComIfGp_particle_set(0x814E /* ID_IT_SN_DK_FUTATOJI_ROCK00 */, &center_pos2, nullptr, nullptr, 0xFF);
            }
            i_this->m25D4 = fadds_ppc(bdk_REGF(0x1047C078), 0.75f); /* HD: 0.75 (GameCube 1.5) */
            mDoAud_seStart_simple_bdk(0x7020 /* JA_SE_ATM_MJT_JINARI */);
            cLib_addCalc0(&i_this->m6320, 1.0f, fadds_ppc(REG0_F(0), 1.0f));
            i_this->m631C = fadds_ppc(i_this->m631C, fadds_ppc(REG0_F(8), 0.003f));
            r27 = 1;
        }
        if (!(i_this->m25A6 > (s16)(REG0_S(2) + 0xFA))) {
            break; /* HD: the player is not placed again here */
        }
        i_this->m25A0 = 0xC;
        i_this->m25A6 = 0;
        i_this->m25B4.x = 2195.0f;
        i_this->m25B4.y = 10007.0f;
        i_this->m25B4.z = -4079.0f;
        i_this->m25A8.x = 1816.0f;
        i_this->m25A8.y = 10051.0f;
        i_this->m25A8.z = -4137.0f;
        i_this->m25CC = 0.0f;
        /* fall through */
    case 0xC:
        cLib_addCalc0(&i_this->m6320, 1.0f, fadds_ppc(REG0_F(0), 1.0f));
        i_this->m631C = fadds_ppc(i_this->m631C, fadds_ppc(REG0_F(8), 0.003f));
        r27 = 1;
        if (i_this->m25A6 < (s16)(REG0_S(6) + 0x9B)) {
            i_this->m25D4 = fadds_ppc(bdk_REGF(0x1047C044), 2.5f); /* HD: 2.5 (GameCube 5.0) */
            mDoAud_seStart_simple_bdk(0x7020 /* JA_SE_ATM_MJT_JINARI */);
        }
        if (i_this->m25A6 == (s16)(REG0_S(6) + 0x96)) {
            i_this->m25D8 = 1;
        }
        if (i_this->m25A6 == 0xA3) { /* HD: a fixed frame (GameCube REG0_S(7) + 0xA0) */
            i_this->m25A6 = 0;
            i_this->m25A0 = 0xD;
        }
        cLib_addCalc2(&i_this->m25B4.x, 2682.0f, 0.05f, fmuls_ppc(4.87f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25B4.y, 9904.0f, 0.05f, fmuls_ppc(1.03f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25B4.z, -3600.0f, 0.05f, fmuls_ppc(4.79f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25A8.x, 2308.0f, 0.05f, fmuls_ppc(4.92f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25A8.y, 9961.0f, 0.05f, fmuls_ppc(0.9f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25A8.z, -3524.0f, 0.05f, fmuls_ppc(6.1299998f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25CC, fadds_ppc(REG0_F(3), 1.5f), 1.0f, fadds_ppc(REG0_F(4), 0.02f));

        local_108->x = 2517.0f; /* HD: 2517 / -3662 (GameCube 2580 / -3670) */
        bdk_fcopy(local_108->y, player->current.pos.y);
        local_108->z = -3662.0f;
        bdk_setPlayerPosAndAngle(player, local_108, 18000);
        break;
    case 0xD:
        if (i_this->m25A6 < (s16)(REG0_S(3) + 0x64)) {
            cLib_addCalc2(&i_this->m25C8, 25.0f, 0.1f, 0.5f);
        } else {
            cLib_addCalc2(&i_this->m25C8, 55.0f, 0.2f, 3.0f);
        }
        if (i_this->m25A6 == (s16)(REG0_S(8) + 0x37)) {
            i_this->m25D8 = 1;
        }
        cLib_addCalc2(&i_this->m25B4.x, i_this->m1150.x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m25B4.y, fadds_ppc(fsubs_ppc(i_this->m1150.y, 200.0f), REG0_F(11)), 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m25B4.z, i_this->m1150.z, 0.1f, 50.0f);
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, 0x2C /* BCK_FLY3 */, 1.0f, 2, 1.0f, 0xD /* BAS_FLY3 */, 0);
            bdk_changeDemoMode(player, 0x17 /* DEMO_A_WAIT_e */);
            bdk_changeDemoParam0(player, 2);
        }
        if (i_this->m25A6 == (s16)(REG0_S(8) + 0xF0)) {
            /* HD: the sub count runs on; the shutter values of GameCube case 0xE are set here */
            i_this->m25A0 = 0xE;
            i_this->m6320 = fadds_ppc(REG0_F(17), -100.0f);
            i_this->m631C = 0.0f;
        }
        r27 = 1;
        break;
    case 0xE:
        i_this->m25A0 = 0xF;
        i_this->m25B4.x = 3304.0f;
        i_this->m25B4.y = 10983.0f;
        i_this->m25B4.z = -3644.0f;
        i_this->m25A8.x = 2841.0f;
        i_this->m25A6 = 0;
        i_this->m25A8.y = 13058.0f;
        i_this->m25A8.z = -3272.0f;
        i_this->m25CC = 0.0f;
        /* fall through */
    case 0xF:
        r27 = 1;
        /* fall through */
    case 0x10:
        cLib_addCalc0(&i_this->m6320, 1.0f, fadds_ppc(REG0_F(11), 1.5f));
        if (std::fabs((f32)i_this->m6320) < 1.0f) {
            cLib_addCalc0(&i_this->m6324, 1.0f, fadds_ppc(REG0_F(12), 1.0f));
            if (i_this->m25A0 == 0xF) {
                dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178), 0x401); /* dComIfGs_onTmpBit? UNK_0401 */
                dSv_memBit_onDungeonItem(bdk_memBit(), 5);                                                     /* dComIfGs_onStageBossDemo */
                fpcM_Search(0x02064D9C /* ep_delete_sub */, actor);
                mDoAud_seStart(0x7822 /* JA_SE_ATM_MJT_SHUTTER_END */, &center_pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                bdk_StartShock(4);
                r27 = 0;
                i_this->m25A0 = 0x10;
                local_154->x = 0;
                local_154->z = 0;
                for (s32 i = 0; i <= 2; i++) {
                    s32 room = fopAcM_GetRoomNo(actor);
                    local_154->y = (s16)(i * 0x5555);
                    bdk_particle_setToon(0xA151 /* ID_IT_ST_DK_FUTATOJI_SMOKE_C00 */, &center_pos, local_154, nullptr, 0xB9, &i_this->m6130[i],
                                         (s8)room);
                }
                gabi::call(0x025A5F88, &i_this->m6130[3]); /* HD: dPa_smokeEcallBack::end (GameCube remove()) */
                JPABaseEmitter* e = i_this->m6100[1];
                if (e != nullptr) {
                    bdk_becomeInvalidEmitter(e);
                    i_this->m6100[1] = nullptr;
                }
            }
        }
        cLib_addCalc2(&i_this->m25A8.x, 3152.0f, 0.05f, fmuls_ppc(3.11f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25A8.z, -3070.0f, 0.05f, fmuls_ppc(2.02f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25CC, fadds_ppc(REG0_F(3), 1.2f), 1.0f, fadds_ppc(REG0_F(4), 0.015f));
        if (i_this->m25A6 == 0xA0) {
            i_this->m25A0 = 0x96;
            i_this->mAction = 0; /* ACTION_FLY */
            i_this->m2CA = -1;
            i_this->mState = 0;
            actor->gravity = fadds_ppc(REG0_F(4), -5.0f);
            i_this->m2B4 = 0;
            for (s32 i = 0; i < 10; i++) {
                i_this->mWindAtSph[i].SetAtType(0x200000 /* AT_TYPE_WIND */);
            }
            actor->shape_angle.x = 0;
            mDoAud_bgmStart(0x80000014 /* JA_BGM_DK_BATTLE */);
        }
        break;
    case 0x32:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->m25CC = 0.0f;
        i_this->m25A0 = i_this->m25A0 + 1;
        i_this->m25A4 = 0;
        i_this->m25A6 = 0;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        {
            f32 ey = actor->eyePos.y;
            bdk_fcopy(i_this->m25B4.y, actor->eyePos.y);
            bdk_fcopy(i_this->m25B4.z, actor->eyePos.z);
            i_this->m25A8.x = 3267.0f;
            i_this->m25A8.y = 9561.0f;
            i_this->m25A8.z = -4562.0f;
            bdk_fcopy(i_this->m25B4.x, actor->eyePos.x);
            i_this->m25B4.y = fsubs_ppc(ey, fadds_ppc(REG0_F(7), 100.0f));
        }
        i_this->m25C8 = 60.0f;
        bdk_changeOriginalDemo(player);
        /* fall through */
    case 0x33: {
        /* HD: the player is turned towards the boss */
        u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
        s16 a = fopAcM_searchActorAngleY(actor, dComIfGp_getPlayer(0));
        gabi::call_ptr(gabi::load<u32>(vt + 0x114), player, &player->current.pos, (s16)(a + 0x8000));
        cLib_addCalc2(&i_this->m25B4.x, actor->eyePos.x, 0.1f, fmuls_ppc(100.0f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25B4.y, fsubs_ppc(actor->eyePos.y, fadds_ppc(REG0_F(7), 100.0f)), 0.3f, fmuls_ppc(200.0f, i_this->m25CC));
        cLib_addCalc2(&i_this->m25B4.z, actor->eyePos.z, 0.1f, fmuls_ppc(100.0f, i_this->m25CC));
        if (i_this->mAction == 0xF /* ACTION_START */) {
            i_this->m25A0 = i_this->m25A0 + 1;
            i_this->m25A6 = 0;
        }
        cLib_addCalc2(&i_this->m25CC, 1.0f, 1.0f, fadds_ppc(REG0_F(4), 0.02f));
        break;
    }
    case 0x34:
        /* HD: the field of view is eased to 45 */
        cLib_addCalc2(&i_this->m25C8, 45.0f, fadds_ppc(bdk_REGF(0x1047C03C), 0.02f), fadds_ppc(bdk_REGF(0x1047C040), 0.075f));
        if (i_this->m25A6 >= 0x64) {
            i_this->m25A0 = 0x96;
        }
        break;
    case 0x64:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent_bdk(actor, 2, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            return;
        }
        i_this->m25CC = 0.0f;
        i_this->m25A0 = i_this->m25A0 + 1;
        i_this->m25A4 = 0;
        i_this->m25A6 = 0;
        dCamera_Stop_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 2);
        {
            u32 pCamera = bdk_camera0();
            cXyz* eye = gabi::at<cXyz>(pCamera + 0xDC);
            bdk_vbits(&i_this->m25A8, eye);
            local_104->y = 0.0f;
            local_104->x = fsubs_ppc(eye->x, i_this->m1150.x);
            local_104->z = fsubs_ppc(eye->z, i_this->m1150.z);
            i_this->m25C0.y = cM_atan2s(local_104->x, local_104->z);
            f32 z = local_104->z;
            f32 x = local_104->x;
            f32 d = std_sqrtf(gabi::fmadds(x, x, fmuls_ppc(z, z)));
            i_this->m25C0.x = (s16)-cM_atan2s(local_104->y, d);
            s16 r = (s16)gabi::ftoi(cM_rndFX(4000.0f));
            i_this->m25D0 = 1500.0f;
            actor->current.angle.y = (s16)(i_this->m25C0.y + r);
            i_this->m25C8 = 45.0f;
            bdk_changeOriginalDemo(player);
            mDoAud_bgmStreamPrepare_bdk(0xC000001A /* JA_STRM_DK_CLEAR */);
            /* HD: the camera target starts at the body (it is eased towards it in case 0x65) */
            f32 y = i_this->m1150.y;
            bdk_fcopy(i_this->m25B4.x, i_this->m1150.x);
            bdk_fcopy(i_this->m25B4.y, i_this->m1150.y);
            bdk_fcopy(i_this->m25B4.z, i_this->m1150.z);
            i_this->m25B4.y = fadds_ppc(y, fadds_ppc(REG0_F(16), -100.0f));
        }
        /* fall through */
    case 0x65:
        if (i_this->m25A6 >= 0xF) {
            /* HD: the player turns towards the boss from frame 15 on */
            bdk_changeDemoMode(player, 0x1A);
            u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
            s16 a = fopAcM_searchActorAngleY(actor, dComIfGp_getPlayer(0));
            gabi::call_ptr(gabi::load<u32>(vt + 0x114), player, &player->current.pos, (s16)(a + 0x8000));
            if (i_this->m25A6 == 0x3C) {
                mDoAud_bgmStreamPlay_bdk();
            }
        }
        cLib_addCalc2(&i_this->m25B4.x, i_this->m1150.x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m25B4.y, fsubs_ppc(i_this->m1150.y, 100.0f), 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m25B4.z, i_this->m1150.z, 0.1f, 50.0f);
        local_104->y = 0.0f;
        local_104->z = fadds_ppc(i_this->m25D0, REG0_F(17));
        local_104->x = 0.0f;
        cMtx_YrotS(calc_mtx(), i_this->m25C0.y);
        cMtx_XrotM(calc_mtx(), i_this->m25C0.x);
        MtxPosition(local_104, local_108);
        PSVECAdd(local_108, &actor->current.pos, local_108); /* HD: around the boss (GameCube m25B4) */
        cLib_addCalc2(&i_this->m25A8.x, local_108->x, 0.1f, 100.0f);
        if (i_this->m25A2 == 0) {
            i_this->m25A8.y = fadds_ppc(REG0_F(18), 9950.0f);
        } else {
            cLib_addCalc2(&i_this->m25A8.y, fadds_ppc(local_108->y, REG0_F(13)), 0.1f, 100.0f);
        }
        cLib_addCalc2(&i_this->m25A8.z, local_108->z, 0.1f, 100.0f);
        break;
    case 0x6E:
        i_this->m25A0 = 0x6F;
        i_this->m25C8 = 40.0f;
        i_this->m25A8.x = -25267.0f;
        i_this->m25A8.y = -948.0f;
        i_this->m25A8.z = 22916.0f;
        /* fall through */
    case 0x6F: {
        f32 fVar17 = actor->current.pos.y;
        i_this->m25B4.x = 3600.0f;
        if (fVar17 < 30000.0f) {
            i_this->m25B4.y = fadds_ppc(fsubs_ppc(fVar17, 6000.0f), REG0_F(19));
        }
        i_this->m25B4.z = -3800.0f;
        cLib_addCalc2(&i_this->m25A8.z, 30449.0f, 0.02f, fmuls_ppc(75.33f, i_this->m25CC));
        if (i_this->m25A6 > REG0_S(2)) {
            cLib_addCalc2(&i_this->m25CC, fadds_ppc((f32)REG0_S(1), 1.5f), 1.0f, fadds_ppc(REG0_F(2), 0.02f));
        }
        local_108->x = 4444.0f;
        local_108->y = 9800.0f;
        local_108->z = -4500.0f;
        bdk_setPlayerPosAndAngle(player, local_108, 0);
        break;
    }
    case 0x70:
        i_this->m25A0 = i_this->m25A0 + 1;
        i_this->m25A6 = 0;
        i_this->m25C8 = dVar16;
        bdk_vbits(&i_this->m25B4, &i_this->m2CC);
        /* fall through */
    case 0x71:
        if (i_this->m25A6 > REG0_S(7) + 0x43) {
            cLib_addCalc2(&i_this->m25B4.y, fadds_ppc(REG0_F(4), 9900.0f), 0.5f, fadds_ppc(REG0_F(6), 200.0f));
        }
        bdk_vbits(&i_this->m25A8, &i_this->m2CC);
        i_this->m25A8.y = fadds_ppc(REG0_F(5), 10000.0f);
        i_this->m25A8.x = fadds_ppc(i_this->m25A8.x, fadds_ppc(REG0_F(7), 500.0f));
        {
            f32 y = i_this->m2CC.y;
            local_108->x = fadds_ppc(i_this->m2CC.x, fsubs_ppc(REG0_F(11), 200.0f));
            local_108->y = y;
            local_108->z = fadds_ppc(i_this->m2CC.z, fsubs_ppc(REG0_F(12), 150.0f));
            bdk_fcopy(local_108->y, player->current.pos.y);
        }
        bdk_setPlayerPosAndAngle(player, local_108, 10000);
        if (i_this->m25A6 == bdk_REGS(0x1047C0AC) + 0x69) {
            bdk_changeDemoMode(player, 0x32 /* DEMO_SMILE_e */);
        }
        if (i_this->m25A6 == REG0_S(2) + 0x82) {
            bdk_changeDemoMode(player, 0x1D /* DEMO_UNK_029_e */);
        }
        if (i_this->m25A6 == REG0_S(4) + 0xBE) {
            i_this->m25A0 = i_this->m25A0 + 1;
            i_this->m25A6 = 0;
            bdk_fbits(gabi::ea(&local_104->x), gabi::ea(&center_pos.x));
            bdk_fbits(gabi::ea(&local_104->z), gabi::ea(&center_pos.z));
            local_104->y = fadds_ppc(bdk_REGF(0x1047C04C), 10300.0f);
            eff_hane_set(i_this, local_104, 0x27, -1);
        }
        break;
    case 0x72:
        i_this->m25B4.x = 4386.0f;
        i_this->m25B4.y = 10020.0f;
        i_this->m25B4.z = -4368.0f;
        i_this->m25A8.x = 4144.0f;
        i_this->m25A8.y = 10061.0f;
        i_this->m25A8.z = -4131.0f;
        if (i_this->m25A6 == 10) {
            dSv_memBit_onDungeonItem(bdk_memBit(), 3); /* dComIfGs_onStageBossEnemy */
            mDoAud_seStart(0x6977 /* JA_SE_OBJ_TOGE_IN */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        if (i_this->m25A6 == 0x28) {
            i_this->m25A0 = 0x96;
        }
        break;
    case 0x96: {
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        bdk_vbits(center, &i_this->m25B4);
        bdk_vbits(eye, &i_this->m25A8);
        i_this->m25A0 = 0;
        dCamera_Reset_bdk(cam, center, eye);
        dCamera_Start_bdk(cam);
        dCamera_SetTrimSize_bdk(cam, 0);
        dComIfGp_event_reset();
        actor->actor_status &= ~0x4000u; /* fopAcStts_UNK4000_e */
        i_this->m259E = 0;                /* HD */
        gabi::store<u8>(0x101F4825, 0);   /* HD: a global flag */
        break;
    }
    default:
        break;
    }

    if (i_this->m25A0 != 0) {
        f32 fx, fy, fz;
        s16 bank = 0;
        f32 amp = i_this->m25D4;
        if (amp > 0.1f) {
            s16 c = i_this->m25A4;
            if (i_this->m25A0 == 0x33) { /* HD: a faster shake for the start cutscene */
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
        local_e4->x = fadds_ppc(i_this->m25A8.x, fx);
        local_f0->x = fsubs_ppc(i_this->m25B4.x, fx);
        local_f0->y = fsubs_ppc(i_this->m25B4.y, fy);
        local_e4->y = fadds_ppc(i_this->m25A8.y, fy);
        local_f0->z = fsubs_ppc(i_this->m25B4.z, fz);
        local_e4->z = fadds_ppc(i_this->m25A8.z, fz);
        dCamera_Set_bdk(cam, local_f0, local_e4, bank, i_this->m25C8);
        cLib_addCalc0(&i_this->m25D4, 1.0f, fadds_ppc(REG0_F(16), 1.0f));
        JUTReport_bdk(0x1E, 0x190, 0x10007F88 /* "K MAIN COUNT  %d" */, i_this->m25A4);
        JUTReport_bdk(0x1E, 0x1A4, 0x10007F9C /* "K SUB  COUNT  %d" */, i_this->m25A6);
        JUTReport_bdk(0x1E, 0x1B8, 0x10007FB0 /* "K DEMO MODE   %d" */, i_this->m25A0);
        i_this->m25A6 = i_this->m25A6 + 1;
        i_this->m25A4 = i_this->m25A4 + 1;
    }
    if (r27) {
        mDoAud_seStart(0x7021 /* JA_SE_ATM_MJT_SHUTTER */, &center_pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
    }
}
VERIFY(0x0206FEF0, demo_camera);
