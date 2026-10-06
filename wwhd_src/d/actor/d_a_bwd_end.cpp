/**
 * d_a_bwd_end.cpp (WWHD)
 * Boss - Molgera: end() (the death sequence).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bwd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bwd.h"

/* 020FEA30 */
static void end(bwd_class* i_this) {
    WWHD_FUNC(0x020FEA30, void, i_this);
    fopAc_ac_c* actor = i_this;
    bool bVar2 = true;
    s16 uVar9 = 0;
    s16 uVar10 = 0;

    dComIfGp_get(); /* HD: result unused */
    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk.get(), BD_GNDCHK_VT, false);
    {
        f32 off = REG0_F(13) + 1000.0f;
        cXyz* p = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24);
        p->x = actor->current.pos.x;
        p->z = actor->current.pos.z;
        p->y = actor->current.pos.y + off;
    }
    f32 fVar13 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
    if (fVar13 != -1000000000.0f) {
        i_this->m18DC = fVar13;
    }
    /* HD: the wobble phases advance every frame, with smaller steps */
    i_this->m18FE = i_this->m18FE + (s16)(REG0_S(5) + 0x834);
    i_this->m1900 = i_this->m1900 + (s16)(REG0_S(5) + 0x9c4);

    switch ((u16)i_this->m18B0) {
    case 0:
        if (dComIfGp_getStartStageName0() == 'X') {
            gabi::call(0x02587EFC, 0, (s32)actor->current.roomNo); /* dLib_setNextStageBySclsNum */
            mDoAud_seStart(0x2888 /* JA_SE_LK_B_BOSS_WARP */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            gabi::call(0x025B8B68, gabi::load<u32>(0x101F84DC) + 0x644, 0x3208);  /* MOLGERA_TRIALS_CLEAR */
            gabi::call(0x025B8B68, gabi::load<u32>(0x101F84DC) + 0x1178, 0x480);  /* tmp bit UNK_0480 */
            bd_GndChk_dt(gndChk.get());
            return;
        }
        bwd_anm_init(i_this, 0xC /* BWD_DEAD1 */, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        actor->current.angle.y = 0;
        i_this->m3C1E = 100;
        i_this->m18B0 = 1;
        fpcM_Search(0x020F9AA0 /* ko_delete_sub */, i_this);
        {
            /* USA: delete the attention helper actor */
            fopAc_ac_c* att = fopAcM_SearchByID(i_this->m1BC0);
            if (att != nullptr)
                fopAcM_delete(att);
        }
        if (i_this->m3AEC == 0) {
            i_this->m3AEC = 0x28;
            gabi::call(0x025A5F88, &i_this->m3978[6]); /* HD: dPa_smokeEcallBack::end (GameCube remove) */
            s8 roomNo = actor->current.roomNo;
            i_this->m3954.x = actor->current.pos.x;
            i_this->m3954.z = actor->current.pos.z;
            dComIfGp_particle_setToon(0xA267 /* ID_IT_ST_BWO_LASTHIT_SMOKE00 */, &i_this->m3954, &actor->shape_angle, nullptr,
                                      gabi::load<u8>(eff_col + 3), &i_this->m3978[6], roomNo);
            smoke_setColor(&i_this->m3978[6], eff_col);
        }
        dComIfGp_particle_set(0x8265 /* ID_IT_SN_BWO_LASTHIT_SUNA00 */, &i_this->m3954, &actor->shape_angle);
        i_this->m3968 = dComIfGp_particle_set(0x8266 /* ID_IT_SN_BWO_LASTHIT_TAIEKI00 */, &i_this->m3954);
        i_this->m3C24 = 0;
        bd_seStart(actor, 0x5918 /* JA_SE_CM_BWD_LAST_HEAD_BACK */, 0);
        /* fallthrough */
    case 1:
        if (i_this->m3968 != nullptr) {
            mDoExt_McaMorf* morf = i_this->mpHeadMorf;
            if (morf->isStop()) {
                JPA_becomeInvalidEmitter(i_this->m3968);
                i_this->m3968 = nullptr;
            } else {
                JPA_setGlobalRTMatrix(i_this->m3968, bd_getAnmMtx(morf->getModel(), 0xE /* BWD_JNT_HEAD_e */));
            }
        }
        if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 30) {
            mDoAud_seStart(0x5919 /* JA_SE_CM_BWD_LAST_IN_SAND */, &i_this->m3954, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 50) {
            bwd_g_eff_off(i_this);
        }
        i_this->m1BB6 = 1;
        if (i_this->mpHeadMorf->isStop()) {
            mDoAud_bgmStreamPrepare(0xC000001B /* JA_STRM_BWD_CLEAR */);
            bwd_anm_init(i_this, 0xD /* BWD_DEAD2 */, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            actor->current.pos.y = -2000.0f;
            for (int i = 0; i < 256; i++) {
                i_this->m0508[i].x = actor->current.pos.x;
                i_this->m0508[i].y = actor->current.pos.y;
                i_this->m0508[i].z = actor->current.pos.z;
            }
            i_this->m18B4.x = 0.0f;
            i_this->m18B0 = 2;
            i_this->m18B4.y = l_HIO.m28 + 1500.0f;
            actor->current.angle.x = -0x4000;
            i_this->m18B4.z = 0.0f;
            i_this->m18CC[1] = REG0_S(9) + 0xb4;
            i_this->m1904 = REG0_S(1) + 3;
        }
        bVar2 = false;
        break;
    case 2: {
        i_this->m1BB6 = 2;
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> local_b4;
        cXyz_mi(&i_this->m18B4, tmp.get(), &actor->current.pos);
        local_b4->copy(*tmp);
        fVar13 = std_sqrtf(PSVECSquareMag(local_b4.get()));
        if ((fVar13 < REG0_F(14) + 500.0f) || (i_this->m18CC[0] == 0)) {
            i_this->m18CC[0] = (s16)gabi::ftoi(cM_rndF(20.0f));
            i_this->m18B4.x = cM_rndFX(REG0_F(16) + 1600.0f);
            f32 b = REG0_F(15) + 2000.0f;
            f32 r = cM_rndFX(500.0f);
            i_this->m18B4.y = (b + r) + l_HIO.m28;
            i_this->m18B4.z = cM_rndFX(REG0_F(16) + 1600.0f);
        }
        i_this->m18C4 = REG0_F(18) + 1000.0f;
        actor->speedF = REG0_F(17) + 65.0f;
        if (i_this->m3C20 == 0x96) {
            bd_seStart(actor, 0x48E2 /* JA_SE_CV_BWD_DIE */, 0);
        }
        if (i_this->m3C20 > 0x96) {
            bd_seStart(actor, 0x5110 /* JA_SE_CM_BWD_FLYING */, 0);
        }
        if (i_this->m18CC[1] == 0) {
            i_this->m3C44 = 0.0f;
            actor->speedF = 0.0f;
            i_this->m3C1E = 0x66;
            i_this->m18B0 = 0x17;
            i_this->m18CC[0] = 0x3c;
            i_this->m18CC[1] = 0x5a;
            i_this->m18C4 = 0.0f;
            i_this->m3C20 = 0;
            bd_seStart(actor, 0x591A /* JA_SE_CM_BWD_HARDEN */, 0);
            mDoAud_bgmStreamPlay();
        }
        f32 amp = REG0_F(13) + 7000.0f;
        uVar9 = (s16)gabi::ftoi(cM_ssin((u16)i_this->m18FE) * amp);
        uVar10 = (s16)gabi::ftoi(cM_scos((u16)i_this->m1900) * amp);
        break;
    }
    case 23:
        i_this->m1BB6 = 2;
        if (i_this->m18CC[0] == 0x14) {
            bwd_anm_init(i_this, 0xE /* BWD_DEAD3 */, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->m02C8 = 1.0f;
        }
        if (((i_this->m18CC[0] == 0) && ((i_this->m18AC & 3) == 0)) && (i_this->m0414 < 0x14)) {
            i_this->m03C4[(s32)i_this->m0414] = 1.0f;
            i_this->m0414 = i_this->m0414 + 1;
        }
        if (((i_this->m18CC[1] == 0) && ((i_this->m18AC & 3) == 0)) && (i_this->m3C24 < 0x14)) {
            if (i_this->m3C24 == 0x13) {
                i_this->m18B0 = 3;
                i_this->m18CC[0] = 0;
                bd_seStart(actor, 0x591B /* JA_SE_CM_BWD_LAST_EXPLODE */, 0);
            } else {
                i_this->m3C24 = i_this->m3C24 + 1;
            }
        }
        break;
    case 3:
        i_this->m1BB6 = 2;
        if (i_this->m18CC[0] == 0) {
            i_this->m18CC[0] = 2;
            gabi::Local<cXyz> local_c0;
            local_c0->x = 2.0f;
            local_c0->z = 2.0f;
            local_c0->y = 2.0f;
            gabi::Local<csXyz> local_d4;
            csXyz_ct(local_d4.get(), 0, 0, 0);
            local_d4->x = (s16)gabi::ftoi(cM_rndF(65536.0f));
            local_d4->y = (s16)gabi::ftoi(cM_rndF(65536.0f));
            dComIfGp_particle_set(0x825D /* ID_IT_SN_BWO_SIBOUBAKUEN00 */, &i_this->m0418[(s32)i_this->m3C24], local_d4.get(), local_c0.get());
            dComIfGp_particle_set(0x825E /* ID_IT_SN_BWO_SIBOUSUNA00 */, &i_this->m0418[(s32)i_this->m3C24], local_d4.get(), local_c0.get());
            s32 rev = dComIfGp_getReverb(fopAcM_GetRoomNo(actor));
            mDoAud_seStart(0x5801 /* JA_SE_CM_MONS_EXPLODE */, &i_this->m0418[(s32)i_this->m3C24], 0, rev);
            i_this->mpBodyModel[(s32)i_this->m3C24] = nullptr;
            if (i_this->m3C24 == 0) {
                i_this->m18B0 = 4;
                i_this->m18CC[0] = 10;
            } else {
                i_this->m3C24 = i_this->m3C24 - 1;
            }
        }
        break;
    case 4:
        i_this->m1BB6 = 2;
        if (i_this->m18CC[0] == 0) {
            fopAcM_createDisappear(actor, &actor->current.pos, 0x32, 1 /* daDisItem_NONE1_e */, 0xff);
            i_this->m18E4.copy(actor->current.pos);
            actor->current.pos.y = -20000.0f;
            i_this->m1B88.y = -20000.0f;
            i_this->m3C1E = 0x68;
            i_this->m3C20 = 0;
            i_this->m18B0 = 5;
        }
        bVar2 = false;
        break;
    case 5:
        i_this->m1BB6 = 2;
        bVar2 = false;
        break;
    }
    if (bVar2) {
        bwd_fly_pos_move(i_this, uVar9, uVar10);
        i_this->m170C = 1;
        /* HD: the body segments wobble until they harden (m03C4[i] >= 0.5), then settle */
        for (int i = 0; i < 20; i++) {
            if (!(i_this->m03C4[i] < 0.5f)) {
                cLib_addCalc0(&i_this->m03C4b[i], 1.0f, 0.01f);
            } else {
                s32 a = i_this->m18FE - i * (REG10_S(1) + 0x3a98);
                i_this->m03C4b[i] = cM_ssin((u16)a) * 0.075f;
            }
        }
    }
    bd_GndChk_dt(gndChk.get());
}
VERIFY(0x020FEA30, end);
