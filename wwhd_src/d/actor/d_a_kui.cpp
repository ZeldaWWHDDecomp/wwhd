/**
 * d_a_kui.cpp (WWHD)
 * Object - Hookable posts for the Grappling Hook
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kui.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * search_dragontail / search_btd and demo_camera are inlined into daKui_Execute.
 */
#include <cmath>

#include "bindings.h"

#define M_arcname_res STR(0x1001396C) /* "Kui" (getRes) */
#define SAFESTRING_VTBL 0x100138E4    /* this TU's sead::SafeString vtable */
#define KUI_VTBL 0x100138FC           /* kui_class vtable (HD virtual destructor) */
#define FILE_NAME STR(0x10013970)
#define ASSERT_MSG STR(0x1001397C)    /* "modelData != NULL" */

enum { fpcNm_DR2_e = 0xDF, fpcNm_BTD_e = 0xEA };
enum { JA_SE_OBJ_ROPE_SW_ON = 0x69D2, JA_SE_OBJ_ST_CHIME = 0x69D3, JA_SE_READ_RIDDLE_1 = 0x806 };

#define mtx_adj gabi::at<Mtx34>(0x101B8A20)
#define bure_xa_d(i) gabi::load<s16>(0x101B8A50 + 2 * (i))
#define REG_S_(c, i) REG_S(c, i)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD: fopAcM_SetMin / fopAcM_SetMax are out of line (025D672C / 025D673C) */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* HD fopAcM_seStartCurrent inline where the actor is known non-null (only &current.pos is checked) */
static inline void fopAcM_seStartCurrent_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* 025E1988 mDoAud_seStart(id) (HD: no position, reads r3 only) */
static inline void mDoAud_seStart_id(u32 id) { gabi::call(0x025E1988, id); }
/* dVibration_c::StartShock(int, int, cXyz) with cXyz(0, 1, 0); the play object is fetched first, then
 * the strength (REG0_S(2) + 5 when reg, else 3) */
static inline void StartShock_up(bool reg) {
    u32 play = dComIfGp_ea();
    s32 s = reg ? REG0_S(2) + 5 : 3;
    gabi::Local<cXyz> up;
    up->set(0.0f, 1.0f, 0.0f);
    gabi::call<BOOL>(0x025CB374, play + PLAY_VIBRATION, s, -0x21, up.get());
}
/* camera_class: dComIfGp_getCamera(id) at play+0x5AF8 + id*0x34; its dCamera_c at +0x248; eye at +0xDC */
static inline u32 dComIfGp_getPlayerCameraID0() { return (u32)(s32)gabi::load<s8>(dComIfGp_ea() + 0x5B30); }
static inline u32 dCam_getCamera() { return gabi::call<u32>(0x024F8020); }
static inline void dCamera_Stop(u32 cam) { gabi::call(0x02514F2C, cam); }
static inline void dCamera_SetTrimSize(u32 cam, s32 s) { gabi::call(0x02515280, cam, s); }
static inline void dCamera_Start(u32 cam) { gabi::call(0x02514F38, cam); }
/* Reset / Set take two cXyz by value (pointers to copies) */
static inline void dCamera_Reset(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, cam, center, eye); }
static inline void dCamera_Set(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x02514F50, cam, center, eye); }
static inline BOOL fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 flag, u16 p2, u16 p3) {
    return gabi::call<BOOL>(0x025D7B24, a, flag, p2, p3);
}
/* 027EC9E8 JUTReport(x, y, fmt, ...) */
static inline void JUTReport(s32 x, s32 y, const char* fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }
/* dComIfGp_roomControl_getStayNo: s8 at 0x1047E6C8 (HD) */
static inline s32 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
static inline void dBgW_SetCrrFunc(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xA8, fn); }
inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }
inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
/* float copy of a cXyz through FPRs (GHS struct copies used for computation) */
static inline void cXyz_fcopy(cXyz* d, const cXyz* s) {
    f32 x = s->x, y = s->y, z = s->z;
    d->x = x;
    d->y = y;
    d->z = z;
}

struct kui_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel2;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> type;
    /* 0x3BD */ be<u8> field_0x2A1;
    /* 0x3BE */ be<u8> field_0x2A2;
    /* 0x3BF */ be<u8> mSwitchNo;
    /* 0x3C0 */ u8 _3C0[4];
    /* 0x3C4 */ Mtx34 field_0x2A8;
    /* 0x3F4 */ gptr<dBgW> field_0x2D8;
    /* 0x3F8 */ be<s16> field_0x2DC[3];
    /* 0x3FE */ be<s16> field_0x2E2;
    /* 0x400 */ be<s16> field_0x2E4;
    /* 0x402 */ be<s16> field_0x2E6;
    /* 0x404 */ be<s8> field_0x2E8;
    /* 0x405 */ u8 _405;
    /* 0x406 */ be<s16> field_0x2EA;
    /* 0x408 */ cXyz field_0x2EC;
    /* 0x414 */ cXyz field_0x2F8;
    /* 0x420 */ u8 _420[4];
    /* 0x424 */ be<s16> field_0x308;
    /* 0x426 */ u8 _426[2];
    /* 0x428 */ be<f32> field_0x30C;
};
WWHD_OFFSET(kui_class, field_0x2A8, 0x3C4);
WWHD_OFFSET(kui_class, field_0x2D8, 0x3F4);
WWHD_OFFSET(kui_class, field_0x2EC, 0x408);
WWHD_OFFSET(kui_class, field_0x30C, 0x428);
WWHD_SIZE(kui_class, 0x42C);

/* 021AC660 */
static void* s_a_i_sub(void* search, void* user) {
    WWHD_FUNC(0x021AC660, void*, search, user);
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_DR2_e)
        return search;
    return nullptr;
}
VERIFY(0x021AC660, s_a_i_sub);

/* 021AC6B0 */
static void* b_a_i_sub(void* search, void* user) {
    WWHD_FUNC(0x021AC6B0, void*, search, user);
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_BTD_e)
        return search;
    return nullptr;
}
VERIFY(0x021AC6B0, b_a_i_sub);

/* 021AC700. HD: takes the J3DModel; the effect matrix is written to the model (+0xF8) and every
 * material instance's texture-matrix slots of type 10/11 point to it (marking them dirty) */
static void setEffectMtx(fopAc_ac_c* a_this, J3DModel* model, f32 scale) {
    WWHD_FUNC(0x021AC700, void, a_this, model, scale);
    cXyz* eyePos = &a_this->eyePos;
    u32 camera = dCam_getCamera();

    gabi::Local<cXyz> look_dir, light_dir, refl;
    cXyz_mi(eyePos, look_dir, gabi::at<cXyz>(camera + 0xDC));
    u32 mb = gabi::ea(model);
    gabi::call(0x02563F64, gabi::ea(a_this) + 0x194 /* tevStr.mLightPosWorld */, eyePos, light_dir.get()); /* dKyr_get_vectle_calc */
    gabi::call(0x028E9E88, look_dir.get(), light_dir.get(), refl.get()); /* C_VECHalfAngle */
    gabi::Local<Mtx34> reflMtx;
    gabi::call(0x028E9684, reflMtx.get(), 0x101FFBA8u /* cXyz::Zero */, 0x101FFBC0u /* cXyz::BaseY */, refl.get()); /* C_MTXLookAt */

    Mtx34* now = mDoMtx_stack_c::get();
    PSMTXScale(now, scale, scale, 1.0f); /* mDoMtx_stack_c::scaleS */
    PSMTXConcat(now, mtx_adj, now);
    PSMTXConcat(now, reflMtx, now);
    now->m[0][3] = 0.0f;
    now->m[2][3] = 0.0f;
    now->m[1][3] = 0.0f;

    Mtx34* effMtx = gabi::at<Mtx34>(mb + 0xF8);
    PSMTXCopy(now, effMtx);

    u16 n = gabi::load<u16>(mb + 0x2A);
    for (u16 i = 0; i < n; i++) {
        u32 mat = gabi::load<u32>(mb + 0x34) + i * 0x3C;
        for (u32 j = 0; j < 8; j++) {
            gabi::Local<be<s32>> slot;
            *slot = -1;
            u32 texMtx = gabi::call<u32>(0x027FA974, mat, j, slot.get());
            if (texMtx != 0) {
                u32 info = gabi::load<u32>(texMtx);
                if (info >= 10 && info <= 11) {
                    u32 m = gabi::load<u32>(mat);
                    s32 off = gabi::load<s32>(m + 0x34);
                    u32 tbl = off != 0 ? m + 0x34 + off : 0;
                    s32 idx = *slot;
                    u32 e = tbl + idx * 0x14;
                    if (gabi::load<s32>(e + 4) >= 0) {
                        gabi::store<u16>(mat + 4, (u16)(gabi::load<u16>(mat + 4) | 4));
                        u32 bits = gabi::load<u32>(mat + 0xC) + ((idx >> 5) << 2);
                        gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (idx & 31)));
                    }
                    m = gabi::load<u32>(mat);
                    off = gabi::load<s32>(m + 0x34);
                    u32 tbl2 = off != 0 ? m + 0x34 + off : 0;
                    u32 idx2 = gabi::load<u16>(e + 0xC);
                    if (gabi::load<s32>(tbl2 + idx2 * 0x14 + 4) >= 0) {
                        gabi::store<u16>(mat + 4, (u16)(gabi::load<u16>(mat + 4) | 4));
                        u32 bits = gabi::load<u32>(mat + 0xC) + (((s32)idx2 >> 5) << 2);
                        gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (idx2 & 31)));
                    }
                    u32 p = gabi::call<u32>(0x027FA678, mat, j);
                    if (p != 0)
                        gabi::store<u32>(p, gabi::ea(effMtx));
                }
            }
        }
    }
}
VERIFY(0x021AC700, setEffectMtx);

/* 021AC94C */
static BOOL daKui_Draw(kui_class* i_this) {
    WWHD_FUNC(0x021AC94C, BOOL, i_this);
    if (i_this->type == 3) {
        u32 light_type = 0;
        if (REG0_S(0) != 0) {
            light_type = 1;
        }

        f32 x = i_this->current.pos.x, y = i_this->current.pos.y;
        i_this->eyePos.x = x;
        i_this->eyePos.y = y;
        i_this->eyePos.z = i_this->current.pos.z;
        i_this->eyePos.y = y + REG0_F(10);

        settingTevStruct(dKy_getEnvlight(), light_type, &i_this->eyePos, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel2, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);

        /* HD: setEffectMtx takes the model */
        setEffectMtx(i_this, i_this->mpModel2, REG0_F(11) + 1.0f);
        setEffectMtx(i_this, i_this->mpModel, REG0_F(12) + 1.0f);

        mDoExt_modelUpdateDL(i_this->mpModel2);
        mDoExt_modelUpdateDL(i_this->mpModel);
    } else if (i_this->type != 1) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel2, &i_this->tevStr);

        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(i_this->mpModel2);
        dComIfGd_setList();

        if (i_this->mpModel) {
            setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
            mDoExt_modelUpdateDL(i_this->mpModel);
        }
    }
    return TRUE;
}
VERIFY(0x021AC94C, daKui_Draw);

/* demo_camera (inlined into daKui_Execute) */
static inline void demo_camera(kui_class* i_this) {
    s32 camId = (s8)dComIfGp_getPlayerCameraID0();
    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + camId * 0x34);
    u32 dcam = camera + 0x248;
    s8 bVar2 = true;

    s8 st = i_this->field_0x2E8;
    switch (st) {
    case 0:
        break;
    case 1:
        if (!eventInfo_checkCommandDemoAccrpt(i_this)) {
            fopAcM_orderPotentialEvent(i_this, 2 /* dEvtFlag_STAFF_ALL_e */, 0xFFFF, 0);
            eventInfo_onCondition(i_this, 2 /* dEvtCnd_UNK2_e */);
            bVar2 = false;
            break;
        }
        i_this->field_0x2E8 = (s8)(st + 1);
        dCamera_Stop(dcam);
        dCamera_SetTrimSize(dcam, 2);
        i_this->field_0x2EA = 0;
        /* fallthrough */
    case 2: {
        i_this->current.pos.y = i_this->home.pos.y + i_this->field_0x30C;

        s32 uVar3 = i_this->field_0x2EA;
        if (uVar3 < 20) {
            f32 sin_result = cM_ssin((uVar3 & 0x1F) * 0x800);
            i_this->field_0x30C = sin_result * 5.0f;
        } else if (uVar3 <= 27) {
            if (uVar3 == 27) {
                fopAcM_seStartCurrent_nn(i_this, JA_SE_OBJ_ROPE_SW_ON, 0);
                StartShock_up(false);
            }
            cLib_addCalc2(&i_this->field_0x30C, -70.0f, 1.0f, 10.0f);
        } else if (uVar3 < 42) {
            f32 sin_result = cM_ssin(uVar3 * 0x3A00);
            i_this->field_0x30C = gabi::fmadds(sin_result, 5.0f, -70.0f);
        }

        cXyz_fcopy(&i_this->field_0x2F8, &i_this->home.pos);
        i_this->field_0x2F8.y = i_this->field_0x2F8.y + (REG_F(8, 0) + 200.0f);
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);

        gabi::Local<cXyz> vec, posVec, sum;
        vec->x = REG_F(8, 1) + 800.f;
        vec->y = REG_F(8, 2);
        vec->z = REG_F(8, 3) + 100.0f;
        MtxPosition(vec, posVec);

        cXyz_pl(&i_this->home.pos, sum, posVec);
        i_this->field_0x2EC.copy(*sum);

        if (i_this->field_0x2EA == 70) {
            gabi::Local<cXyz> c, e;
            cXyz_fcopy(c, &i_this->field_0x2F8);
            cXyz_fcopy(e, &i_this->field_0x2EC);
            i_this->field_0x2E8 = 0;
            dCamera_Reset(dcam, c, e);
            dCamera_Start(dcam);
            dCamera_SetTrimSize(dcam, 0);

            dComIfGp_event_reset();

            dComIfGs_onSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(i_this));
            mDoAud_seStart_id(JA_SE_READ_RIDDLE_1);
        }
        break;
    }
    }

    if ((i_this->field_0x2E8 != 0) && bVar2) {
        gabi::Local<cXyz> c, e;
        cXyz_fcopy(e, &i_this->field_0x2EC);
        cXyz_fcopy(c, &i_this->field_0x2F8);
        dCamera_Set(dcam, c, e);
        JUTReport(0x19a, 0x1ae, STR(0x1001390C) /* "K SUB  COUNT  %d" */, i_this->field_0x2EA);
        i_this->field_0x2EA = i_this->field_0x2EA + 1;
    }
}

/* type 3 (the bell): swing from the grappling hook */
static inline void kui_bell(kui_class* i_this, fopAc_ac_c* player) {
    fopAc_ac_c* actor = i_this;
    s16 target_x_angle;
    if (actor->health == 3) {
        gabi::Local<cXyz> hand, diff, temp2, temp;
        cXyz_fcopy(hand, gabi::at<cXyz>(gabi::ea(player) + 0x3F0)); /* daPy_py_c::getLeftHandPos() (HD +0x3F0) */
        cXyz_mi(hand, diff, &actor->home.pos);
        temp2->copy(*diff);
        cMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);

        MtxPosition(temp2, temp);
        f32 tz = temp->z * (REG0_F(1) + 1.0f);
        temp->z = tz;

        if (REG0_S(0) == 0) {
            target_x_angle = (s16)-cM_atan2s(tz, -temp->y);
        } else {
            target_x_angle = cM_atan2s(tz, -temp->y);
        }
        s16 unk = (s16)gabi::ftoi(std::fabs(cM_ssin(actor->shape_angle.y) * (REG0_F(2) + 3000.0f)));

        s32 unk_flag = 0;
        if (target_x_angle > unk) {
            target_x_angle = unk;
            unk_flag = 1;
        } else if (target_x_angle < (s16)-unk) {
            target_x_angle = (s16)-unk;
            unk_flag = 2;
        }

        /* HD: no `unk > 2000` condition */
        if (unk_flag != 0) {
            if (i_this->field_0x2DC[unk_flag] == 0) {
                i_this->field_0x2DC[unk_flag] = 0x50;
                i_this->field_0x2DC[0] = (s16)(REG_S_(10, 3) + 40); /* HD: REG10_S(3) (GameCube REG0_S(3)) */

                StartShock_up(true);
                fopAcM_seStartCurrent_nn(actor, JA_SE_OBJ_ST_CHIME, 0);

                s16 v = bure_xa_d(unk_flag - 1);
                i_this->field_0x2E4 = v;
                i_this->field_0x2E4 = (s16)(v * (REG_S_(17, 4) + 1));
                i_this->field_0x2E6 = (s16)(REG_S_(17, 5) + 0x400);
            }
        }

        s16 d = (s16)(actor->current.angle.y - player->shape_angle.y);
        if (REG0_S(1) == 0) {
            actor->shape_angle.y = (s16)-d;
        } else {
            actor->shape_angle.y = d;
        }
    } else {
        target_x_angle = 0;
    }

    cLib_addCalcAngleS2(&actor->current.angle.x, target_x_angle, 4, (s16)(REG0_S(1) + 0x200));
    s16 c1 = i_this->field_0x2DC[1];
    s16 c2 = i_this->field_0x2DC[2];
    if (c1 != 0) {
        i_this->field_0x2DC[1] = (s16)(c1 - 1);
    }
    s16 c0 = i_this->field_0x2DC[0];
    if (c2 != 0) {
        i_this->field_0x2DC[2] = (s16)(c2 - 1);
    }

    s16 x = 0;
    s16 z = 0;
    f32 unk_f = 1.0f;

    if (c0 != 0) {
        s16 iVar10 = (s16)(c0 - 1);
        i_this->field_0x2DC[0] = iVar10;

        f32 fv = (f32)iVar10;
        f32 fVar1 = fv * (REG0_F(16) + 40.0f);

        x = (s16)gabi::ftoi(cM_ssin(iVar10 * (REG0_S(4) + 0x1900)) * fVar1);
        z = (s16)gabi::ftoi((cM_scos(iVar10 * (REG0_S(5) + 0x2100)) * fVar1) * 0.25f);

        if ((iVar10 & 1) != 0) {
            unk_f = gabi::fmadds(fv, REG0_F(17) + 0.001f, 1.0f);
        }

        if (iVar10 == 0 && actor->health == 3 && REG0_S(3) == 0) {
            dComIfGs_onSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(actor));
        }
    }

    cLib_addCalcAngleS2(&i_this->field_0x2E2, i_this->field_0x2E4, 4, i_this->field_0x2E6);
    cLib_addCalcAngleS2(&i_this->field_0x2E4, 0, 1, (s16)(REG_S_(17, 6) + 0x80));
    cLib_addCalcAngleS2(&i_this->field_0x2E6, (s16)(REG_S_(17, 7) + 0x100), 1, (s16)(REG_S_(17, 8) + 0x40));

    MtxTrans(actor->home.pos.x, actor->home.pos.y, actor->home.pos.z, FALSE);
    cMtx_YrotM(calc_mtx(), actor->current.angle.y);

    MtxPush();
    cMtx_YrotM(calc_mtx(), actor->shape_angle.y);
    cMtx_XrotM(calc_mtx(), (s16)(x + i_this->field_0x2E2));
    cMtx_ZrotM(calc_mtx(), z);
    cMtx_YrotM(calc_mtx(), (s16)-actor->shape_angle.y);
    MtxScale(unk_f, unk_f, unk_f, TRUE);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());

    MtxPull();
    cMtx_YrotM(calc_mtx(), (s16)(actor->shape_angle.y + REG0_S(5)));
    cMtx_XrotM(calc_mtx(), (s16)(actor->current.angle.x + REG0_S(6)));
    {
        s16 r = REG0_S(5);
        cMtx_YrotM(calc_mtx(), (s16)-(actor->shape_angle.y + r + 0x4000));
    }

    MtxScale(unk_f, 1.0f, unk_f, TRUE);
    J3DModel_setBaseTRMtx(i_this->mpModel2, calc_mtx());
    MtxTrans(0.0f, REG0_F(6) + -850.0f, 0.0f, TRUE);

    gabi::Local<cXyz> zero;
    zero->set(0.0f, 0.0f, 0.0f);
    MtxPosition(zero, &actor->current.pos);

    MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, FALSE);
    cMtx_YrotM(calc_mtx(), actor->current.angle.y);
    MtxScale(actor->scale.x, actor->scale.y, actor->scale.z, TRUE);
    PSMTXCopy(calc_mtx(), &i_this->field_0x2A8);
    dBgW_Move(i_this->field_0x2D8);
}

/* 021ACB0C */
static BOOL daKui_Execute(kui_class* i_this) {
    WWHD_FUNC(0x021ACB0C, BOOL, i_this);
    fopAc_ac_c* actor = i_this;

    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    if (i_this->field_0x2A2 != 0) {
        fopAc_ac_c* dragon_tail = (fopAc_ac_c*)fpcM_Search(0x021AC660 /* s_a_i_sub */, i_this);
        fopAc_ac_c* btd = (fopAc_ac_c*)fpcM_Search(0x021AC6B0 /* b_a_i_sub */, i_this);

        if (dragon_tail != nullptr && btd != nullptr) {
            u32 dt = gabi::ea(dragon_tail);
            s16 u4BA = gabi::load<s16>(dt + 0x5D6); /* dr2_class::unk_4BA */
            if ((u4BA == 0 || u4BA >= 10) && gabi::load<s16>(gabi::ea(btd) + 0x40C) < 10) { /* btd_class::m02E4 */
                actor->current.pos.copy(*gabi::at<cXyz>(dt + 0x4CC)); /* dr2_class::unk_3B0 */
                actor->current.angle.x = dragon_tail->current.angle.x;
                actor->current.angle.y = dragon_tail->current.angle.y;
                actor->current.angle.z = dragon_tail->current.angle.z;
            } else {
                actor->current.pos.set(0.0f, -10000.0f, 0.0f);
            }
        } else {
            actor->current.pos.set(0.0f, -10000.0f, 0.0f);
        }
    }

    if (i_this->type == 3) {
        kui_bell(i_this, player);
    } else {
        MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, FALSE);
        cMtx_YrotM(calc_mtx(), actor->current.angle.y);
        if (i_this->type == 2 || i_this->type == 4) {
            J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
        }

        J3DModel_setBaseScale(i_this->mpModel2, &actor->scale);
        J3DModel_setBaseTRMtx(i_this->mpModel2, calc_mtx());
        gabi::Local<Mtx34> local_mtx;
        if (i_this->field_0x2A2) {
            PSMTXScale(local_mtx, 4.0f, 4.0f, 4.0f);
        } else {
            PSMTXScale(local_mtx, actor->scale.x, actor->scale.y, actor->scale.z);
        }

        PSMTXConcat(calc_mtx(), local_mtx, &i_this->field_0x2A8);
        dBgW_Move(i_this->field_0x2D8);
    }

    if (i_this->type == 2 || i_this->type == 4) {
        BOOL is_switch = dComIfGs_isSwitch(i_this->mSwitchNo, dComIfGp_roomControl_getStayNo());

        if (!is_switch) {
            if (actor->health == 3 && i_this->field_0x308 == 0) {
                i_this->field_0x308 = 1000; /* HD: folded with the decrement below (stores 999) */
            }
        } else {
            actor->current.pos.y = actor->home.pos.y - 70.0f;
        }

        if (i_this->field_0x308 != 0) {
            i_this->field_0x308 = (s16)(i_this->field_0x308 - 1);
            s16 finished = (s16)(REG_S_(8, 3) + 970);
            if (i_this->field_0x308 == finished) {
                if (i_this->type == 2) {
                    i_this->field_0x2E8 = 1;
                } else {
                    fopAcM_seStartCurrent_nn(actor, JA_SE_OBJ_ROPE_SW_ON, 0);
                    dComIfGs_onSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(actor));
                    mDoAud_seStart_id(JA_SE_READ_RIDDLE_1);
                }
            }
        }
        demo_camera(i_this);
    }

    actor->eyePos.copy(actor->current.pos);
    return TRUE;
}
VERIFY(0x021ACB0C, daKui_Execute);

/* 021ADAF4 */
static BOOL daKui_IsDelete(kui_class*) {
    WWHD_FUNC(0x021ADAF4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021ADAF4, daKui_IsDelete);

/* 021ADAFC */
static BOOL daKui_Delete(kui_class* i_this) {
    WWHD_FUNC(0x021ADAFC, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x10013968)); /* dComIfG_resDeleteDemo */
    cBgS_Release(dComIfG_Bgsp(), i_this->field_0x2D8);
    return TRUE;
}
VERIFY(0x021ADAFC, daKui_Delete);

/* 021ADB48 */
static BOOL daKui_CreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021ADB48, BOOL, a_this);
    kui_class* i_this = (kui_class*)a_this;
    J3DModelData* modelData;

    if (i_this->type == 3) {
        /* Bell body */
        modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, 4 /* BDL_HKANE1 */, SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(851, modelData != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x391, ASSERT_MSG);

        i_this->mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (!i_this->mpModel) {
            return FALSE;
        }

        /* Bell handle */
        modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, 5 /* BDL_HKANE2 */, SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(863, modelData != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x39D, ASSERT_MSG);

        i_this->mpModel2 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (!i_this->mpModel2) {
            return FALSE;
        }
    } else {
        /* Wooden post */
        modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, 7 /* BDL_OBI_ROPETAG */, SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(875, modelData != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x3A9, ASSERT_MSG);

        i_this->mpModel2 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
        if (!i_this->mpModel2) {
            return FALSE;
        }

        if (i_this->type == 2 || i_this->type == 4) {
            /* Device that holds the post */
            modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, 6 /* BDL_MROPESW */, SAFESTRING_VTBL);
            if (modelData == nullptr) /* JUT_ASSERT(887, modelData != NULL) */
                JUT_ASSERT_fail(FILE_NAME, 0x3B5, ASSERT_MSG);

            i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
            if (!i_this->mpModel) {
                return FALSE;
            }
        }
    }

    dBgW* bgw = new_dBgW();
    i_this->field_0x2D8 = bgw;
    if (!i_this->field_0x2D8) {
        return FALSE;
    }

    cBgD_t* pData = (cBgD_t*)dComIfG_getObjectRes(M_arcname_res, 0xA /* DZB_OBI_ROPETAG */, SAFESTRING_VTBL);
    if (cBgW_Set(i_this->field_0x2D8, pData, cBgW_MOVE_BG_e, &i_this->field_0x2A8) == true) {
        return FALSE;
    }

    dBgW_SetCrrFunc(i_this->field_0x2D8, 0x024EE658 /* dBgS_MoveBGProc_Typical */);

    return TRUE;
}
VERIFY(0x021ADB48, daKui_CreateHeap);

/* 021ADD6C */
static cPhs_State daKui_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021ADD6C, cPhs_State, a_this);
    kui_class* i_this = (kui_class*)a_this;
    cPhs_State result;

    /* fopAcM_ct(a_this, kui_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            i_this->__vtbl = KUI_VTBL;
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    result = dComIfG_resLoad(&i_this->mPhs, STR(0x100139A4));
    if (result == cPhs_COMPLEATE_e) {
        dComIfGp_get(); /* HD: result unused */

        if (fopAcM_GetParam(a_this) == 0xFFFFFFFF) {
            return cPhs_ERROR_e;
        }

        i_this->type = fopAcM_GetParam(a_this) & 0xF;
        i_this->field_0x2A2 = fopAcM_GetParam(a_this) & 0xF0;
        i_this->field_0x2A1 = (u8)(fopAcM_GetParam(a_this) >> 8);
        u8 sw = (u8)(fopAcM_GetParam(a_this) >> 0x18);

        if (sw == 0xFF) {
            i_this->mSwitchNo = 0;
        } else {
            i_this->mSwitchNo = sw;
        }
        if (i_this->type == 3) {
            i_this->field_0x2A1 = 4;
        }

        switch (i_this->field_0x2A1) {
        case 0:
            i_this->scale.x = 0.5f;
            i_this->scale.y = 0.5f;
            break;
        case 1:
            i_this->scale.x = 2.0f;
            i_this->scale.y = 2.0f;
            break;
        case 2:
            i_this->scale.z = 2.0f;
            break;
        case 3:
            i_this->scale.x = 0.5f;
            i_this->scale.y = 0.5f;
            i_this->scale.z = 2.0f;
            break;
        case 4:
            i_this->scale.set(2.0f, 2.0f, 2.0f);
            break;
        case 5:
            i_this->scale.x = 4.0f;
            i_this->scale.y = REG0_F(2) + 2.0f;
            i_this->scale.z = 4.0f;
            break;
        case 0xff:
        default:
            break;
        }

        if (!fopAcM_entrySolidHeap(i_this, 0x021ADB48 /* daKui_CreateHeap */, 0x29f4)) {
            return cPhs_ERROR_e;
        }

        if (dBgS_Regist(dComIfG_Bgsp(), i_this->field_0x2D8, i_this)) {
            return cPhs_ERROR_e;
        }

        i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel2)); /* fopAcM_SetMtx */
        if (i_this->type >= 2) {
            fopAcM_SetMin(i_this, -200.0f, -1000.0f, -200.0f);
            fopAcM_SetMax(i_this, 200.0f, 2000.0f, 200.0f);
        } else {
            fopAcM_SetMin(i_this, -200.0f, -200.0f, -200.0f);
            fopAcM_SetMax(i_this, 200.0f, 200.0f, 200.0f);
        }
    }
    return result;
}
VERIFY(0x021ADD6C, daKui_Create);

/* 021AE0EC */
static void __sinit_d_a_kui_cpp() {
    WWHD_FUNC(0x021AE0EC, void, (u32)0);
    sinit_header_statics(0x10464CAC, 0x101B8A54);
}
VERIFY(0x021AE0EC, __sinit_d_a_kui_cpp);

/* 021AE180: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021AE180, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021AE180, trivial_dt);

/* 021AE194: kui_class deleting destructor */
static void kui_class_dt(kui_class* i_this, s32 flags) {
    WWHD_FUNC(0x021AE194, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021AE194, kui_class_dt);

/* 021AE1E8: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x021AE1E8, void, p);
}
VERIFY(0x021AE1E8, empty_virtual);
