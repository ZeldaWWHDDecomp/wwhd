/**
 * d_a_bdk_sub.cpp (WWHD)
 * Boss - Helmaroc King (battle): eff_hane_set, anm_init, nodeCallBack, pos_move, ground_move,
 * wind_set.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;
using gabi::fsubs_ppc;

/* 02065278 */
void eff_hane_set(bdk_class* i_this, cXyz* offset, int param_3, s8 param_4) {
    WWHD_FUNC(0x02065278, void, i_this, offset, param_3, param_4);
    fopAc_ac_c* actor = i_this;
    s32 iVar4 = 0;
    for (s32 i = 0; i < 0x27; i++) {
        if (iVar4 >= param_3) {
            break;
        }
        s8 idx = i_this->m261A;
        bdk_eff_s* eff = &i_this->m261C[idx];
        s8 next = (s8)(idx + 1);
        if (next > 0x26) {
            next = 0;
        }
        i_this->m261A = next;

        eff->m000 = 1;
        eff->m03E = 2000;
        eff->m001 = 0;
        eff->m040 = 0;
        eff->m02C = 1.0f;

        u32 xb = gabi::load<u32>(gabi::ea(&offset->x));
        gabi::store<u32>(gabi::ea(&eff->m004.x), xb);
        bdk_fcopy(eff->m004.y, offset->y);
        bdk_fcopy(eff->m004.z, offset->z);
        f32 x = gabi::f32_from_bits(xb);
        if (param_4 < 0) { /* HD: the feathers start scattered around the point */
            f32 r = cM_rndFX(500.0f);
            eff->m004.x = fadds_ppc(x, r);
            r = cM_rndFX(500.0f);
            eff->m004.z = fadds_ppc(eff->m004.z, r);
            r = cM_rndF(500.0f);
            eff->m004.y = fadds_ppc(eff->m004.y, r);
        }

        eff->m024 = 0.0f;
        eff->m03C = (s16)gabi::ftoi(cM_rndF(65536.0f));
        eff->m036.z = (s16)gabi::ftoi(cM_rndFX(fadds_ppc(REG8_F(10), 1200.0f)));
        eff->m036.x = (s16)gabi::ftoi(cM_rndFX(fadds_ppc(REG8_F(16), 800.0f)));
        iVar4++;

        if (param_4 > 0) {
            s16 r = (s16)gabi::ftoi(cM_rndFX(5000.0f));
            eff->m030.y = actor->shape_angle.y + r;
            f32 a = fadds_ppc(cM_rndF(10.0f), 20.0f);
            eff->m020 = fadds_ppc(a, REG8_F(8));
            f32 b = fsubs_ppc(cM_rndFX(5.0f), 10.0f);
            eff->m01C = fadds_ppc(b, REG8_F(9));
            eff->m040 = (s8)gabi::ftoi(fadds_ppc(cM_rndF(20.0f), 20.0f));
        } else {
            if (param_4 < 0) {
                eff->m020 = cM_rndF(10.0f);
            } else {
                f32 a = fadds_ppc(cM_rndF(5.0f), 5.0f);
                eff->m020 = fadds_ppc(a, REG8_F(8));
            }
            f32 b = fadds_ppc(cM_rndF(5.0f), 5.0f);
            eff->m01C = fadds_ppc(b, REG8_F(9));
            eff->m030.y = (s16)gabi::ftoi(cM_rndF(65536.0f));
        }
    }
}
VERIFY(0x02065278, eff_hane_set);

/* 02065620 */
void anm_init(bdk_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx, u8 param7) {
    WWHD_FUNC(0x02065620, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx, param7);
    if (param7 == 0) {
        i_this->m2590 = loopMode;
        i_this->m2588 = bckFileIdx;
        i_this->m2591 = 0;
        i_this->m258C = soundFileIdx;
    } else {
        i_this->m2591 = 1;
    }

    if (soundFileIdx >= 0) {
        void* anm = dComIfG_getObjectRes(STR_BDK_ANM, bckFileIdx, BDK_SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(STR_BDK_ANM, soundFileIdx, BDK_SAFESTRING_VTBL);
        morf_setAnm(i_this->mpMorf, anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        void* anm = dComIfG_getObjectRes(STR_BDK_ANM, bckFileIdx, BDK_SAFESTRING_VTBL);
        morf_setAnm(i_this->mpMorf, anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x02065620, anm_init);

/* 02065794 (HD: no Y rotation of the neck; the tail index is bounds-checked) */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02065794, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_bdk* model = bdk_j3dSys_getModel();
        bdk_class* bdk = gabi::at<bdk_class>(model->mUserArea);
        s32 jnt_no = gabi::load<u16>(gabi::ea(joint) + 4);
        if (bdk != nullptr) {
            PSMTXCopy(bdk_getAnmMtx(model, jnt_no), calc_mtx());
            if (jnt_no == 0x17 /* DK_JNT_J_DK_KUBI1_e */) {
                cMtx_XrotM(calc_mtx(), bdk->mF12);
                bdk_mtx_copy(bdk_getAnmMtx(model, jnt_no), calc_mtx());
                PSMTXCopy(calc_mtx(), bdk_J3DSys_mCurrentMtx());
            } else if ((u32)(jnt_no - 0x1F) < 6 /* TOSAKA_A1 .. TOSAKA_B2 */) {
                s16 base = bdk->m112E + bdk->m1120;
                if (jnt_no >= 0x23 /* TOSAKA_B1 */) {
                    cMtx_YrotM(calc_mtx(), (s16)(base + bdk->m1124));
                    cMtx_ZrotM(calc_mtx(), (s16)(bdk->m112C + bdk->m1122 + bdk->m1126));
                } else {
                    cMtx_YrotM(calc_mtx(), (s16)-(base + bdk->m1128));
                    cMtx_ZrotM(calc_mtx(), (s16)-(bdk->m112C + bdk->m1122 + bdk->m112A));
                }
                bdk_mtx_copy(bdk_getAnmMtx(model, jnt_no), calc_mtx());
                PSMTXCopy(calc_mtx(), bdk_J3DSys_mCurrentMtx());
            } else {
                gabi::Local<cXyz> offset;
                s32 idx = (jnt_no - 0x3A /* O_LA2 */) / 2;
                offset->x = 0.0f;
                offset->y = 0.0f;
                offset->z = 0.0f;
                if ((u32)idx < 4) {
                    bdk_tail_s* tail = &bdk->m300[idx];
                    MtxPosition(offset, &tail->m0150[1]);
                    offset->x = -10.0f;
                    MtxPosition(offset, &tail->m0150[0]);
                }
            }
        }
    }
    return TRUE;
}
VERIFY(0x02065794, nodeCallBack);

/* 02066398 */
void pos_move(bdk_class* i_this) {
    WWHD_FUNC(0x02066398, void, i_this);
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> out;
    {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->m2CC, tmp, &actor->current.pos);
        bdk_fcopy(diff->x, tmp->x);
        bdk_fcopy(diff->y, tmp->y);
        bdk_fcopy(diff->z, tmp->z);
    }
    s16 targetAngleXZ = cM_atan2s(diff->x, diff->z);
    f32 dz = diff->z;
    s16 targetAngleY = (s16)-cM_atan2s(diff->y, std_sqrtf(gabi::fmadds(diff->x, diff->x, dz * dz)));
    s16 step = (s16)gabi::ftoi(i_this->m2DC * i_this->m2E0);
    s16 sVar1 = actor->current.angle.y;
    cLib_addCalcAngleS2(&actor->current.angle.y, targetAngleXZ, (s16)(REG0_S(3) + 10), step);

    s16 d = (s16)((s32)(sVar1 - actor->current.angle.y) << 5);
    s16 lim = (s16)(REG0_S(1) + 5500);
    if (d > lim) {
        d = lim;
    } else if (d < -(s32)lim) {
        d = (s16)-(s32)lim;
    }
    step = (s16)gabi::ftoi(i_this->m2DC * i_this->m2E0 * 0.5f);
    cLib_addCalcAngleS2(&actor->current.angle.z, d, (s16)(REG0_S(3) + 10), step);
    step = (s16)gabi::ftoi(i_this->m2DC * i_this->m2E0);
    cLib_addCalcAngleS2(&actor->current.angle.x, targetAngleY, (s16)(REG0_S(3) + 10), step);

    cLib_addCalc2(&i_this->m2E0, 1.0f, 1.0f, 0.05f);
    cLib_addCalc2(&actor->speedF, i_this->m2E4, 1.0f, i_this->m2E8);

    diff->x = 0.0f;
    diff->y = 0.0f;
    bdk_fcopy(diff->z, actor->speedF);
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    cMtx_XrotM(calc_mtx(), actor->current.angle.x);

    if (i_this->m2591 == 0) {
        MtxPosition(diff, &actor->speed);
    } else {
        MtxPosition(diff, out);
        bdk_fcopy(actor->speed.x, out->x);
        bdk_fcopy(actor->speed.z, out->z);
        if (morf_frame(i_this->mpMorf) < 20.0f) {
            actor->speed.y = fsubs_ppc(actor->speed.y, fadds_ppc(REG0_F(9), 1.5f));
        } else {
            actor->speed.y = fadds_ppc(actor->speed.y, fadds_ppc(REG0_F(10), 1.5f));
        }
    }
    PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
}
VERIFY(0x02066398, pos_move);

/* 02066634 */
void ground_move(bdk_class* i_this) {
    WWHD_FUNC(0x02066634, void, i_this);
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> out;
    cLib_addCalcAngleS2(&actor->current.angle.z, 0, 2, 0x800);
    cLib_addCalcAngleS2(&actor->current.angle.x, 0, 2, 0x800);
    cLib_addCalc2(&actor->speedF, i_this->m2E4, 1.0f, i_this->m2E8);

    diff->x = 0.0f;
    diff->y = 0.0f;
    bdk_fcopy(diff->z, actor->speedF);
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    MtxPosition(diff, out);
    bdk_fcopy(actor->speed.x, out->x);
    bdk_fcopy(actor->speed.z, out->z);
    PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
    actor->speed.y = fadds_ppc(actor->speed.y, actor->gravity);

    {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->m2CC, tmp, &actor->current.pos);
        bdk_fcopy(diff->x, tmp->x);
        bdk_fcopy(diff->y, tmp->y);
        bdk_fcopy(diff->z, tmp->z);
    }
    s16 target = cM_atan2s(diff->x, diff->z);
    cLib_addCalcAngleS2(&actor->current.angle.y, target, 2, (s16)gabi::ftoi(i_this->m2DC * i_this->m2E0));
    cLib_addCalc2(&i_this->m2E0, 1.0f, 1.0f, 0.1f);
}
VERIFY(0x02066634, ground_move);

/* 02066798 */
void wind_set(bdk_class* i_this, cXyz* param2) {
    WWHD_FUNC(0x02066798, void, i_this, param2);
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> vec1;
    if ((i_this->m2C4 & 1) == 0) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        vec1->y = 0.0f;
        vec1->x = 0.0f;
        vec1->z = fadds_ppc(REG0_F(4), 100.0f);
        for (s32 i = 0; i < 10; i++) {
            if (i_this->m2488[i] == 0) {
                i_this->m2488[i] = 1;
                Mtx34* m = calc_mtx();
                s16 r = (s16)gabi::ftoi(cM_rndFX(6000.0f));
                cMtx_YrotS(m, (s16)(actor->current.angle.y + r));
                MtxPosition(vec1, &i_this->m250C[i]);
                i_this->m2494[i].copy(*param2);
                bdk_fcopy(i_this->m2494[i].y, player->current.pos.y);
                break;
            }
        }
    }
    u32 camera = bdk_camera0();
    cXyz* eye = gabi::at<cXyz>(camera + 0xDC);
    gabi::Local<cXyz> vec2;
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> t3;
    cXyz_mi(&actor->eyePos, vec2, eye);
    cXyz_mi(eye, t1, &actor->eyePos);
    cXyz_ml(t1, t2, 0.8f);
    cXyz_pl(&actor->eyePos, t3, t2);
    wind_se_pos.copy(*t3.get());
    mDoAud_seStart(0x5076 /* JA_SE_CM_DK_WIND */, &wind_se_pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
}
VERIFY(0x02066798, wind_set);
