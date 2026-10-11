/**
 * d_a_kokiie.cpp (WWHD)
 * Object - Forbidden Woods - Hanging flower house
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kokiie.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kokiie.h"
const gc_tu_data gc_tu = {.safeString = 0x100134AC};

#define SAFESTRING_VTBL 0x100134AC /* this TU's sead::SafeString vtable */
enum { dRes_INDEX_KOKIIE_BDL_KOKI_00_e = 4, dRes_INDEX_KOKIIE_DZB_KOKI_00_e = 7 };
enum { fpcNm_SHAND_e = 0x63, dItemNo_BOOMERANG_e = 0x2D };

enum { JA_SE_OBJ_KOKIRI_H_CRASH = 0x6A0E, JA_SE_OBJ_KOKIRI_H_LANDING = 0x6951, JA_SE_READ_RIDDLE_1 = 0x806 };
enum {
    ID_AK_SN_KOKIRIHOUSESPLASH00 = 0x82A4,
    ID_AK_SN_KOKIRIHOUSESPLASH01 = 0x82A5,
    ID_AK_SN_KOKIRIHOUSEHAMON00 = 0x82A6,
    ID_AK_SN_KOKIRIHOUSEHAMON01 = 0x82A7,
};

/* debug registers (g_regHIO): REGn_F(i) / REGn_S(i) */
#define REG8_F(i) REG_F(8, i)
#define REG12_F(i) REG_F(12, i)
#define REG14_F(i) REG_F(14, i)
#define REG6_S(i) REG_S(6, i)
#define REG12_S(i) REG_S(12, i)

// HD: actor creation lowers to a layer lookup followed by a scene request.
static u32 fopAcM_Create(s16 name, void* append) {
    void* layer = Kokiie_currentLayer();
    return Kokiie_request(layer, name, 0, 0, append);
}
// HD: csXyz::operator+ returns its six-byte value in r3/r4.
static inline void csXyz_pl_regs(const csXyz* a, const csXyz* b, u32* w0, u32* w1) {
    *w0 = Kokiie_addAngles(a, b);
    *w1 = gabi::cpu->r[4];
}
/* fopAcM_seStartCurrent (HD inline: null check on &current.pos) */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    cXyz* pos = &a->current.pos;
    if (gabi::ea(pos) != 0)
        mDoAud_seStart(id, pos, param, dComIfGp_getReverb(a->current.roomNo));
}
/* dComIfGp_getVibration().StartShock(REGn_S(i) + add, -0x21, cXyz(0, 1, 0)): the register is read
 * after the play-object accessor call */
static inline void StartShock(int child, int idx, s32 add) {
    dVibration_c* vib = dComIfGp_getVibration();
    s32 s = REG_S(child, idx) + add;
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    vib->StartShock(s, -0x21, v.get());
}

// Rope-state lookup tables, as in the GameCube implementation.
static u8 himo_off_check[] = {1, 2, 4, 8, 16};
static s16 himo_off_ya[] = {
    1, 0, 12000, 7500, 24500, 0, 20000, 12500,
    -26500, 0, 0, 6500, 32767, 32767, 26500, 20500,
    -12500, -7500, 0, 0, 0, -7000, 19500, 7500,
    -20000, -12500, -19500, -7500, -26500, -19500, -32768, 1,
};
static s16 himo_off_xa[] = {
    0, 2000, 2000, 3000, 2000, 0, 3000, 4000,
    2000, 0, 0, 2000, 3000, 2000, 4000, 4000,
    1000, 2000, 0, 4000, 0, 2000, 2000, 4000,
    3000, 4000, 3000, 4000, 4000, 4000, 4000, 4000,
};
static s16 himo_off_yp[] = {
    0, 150, 150, 300, 150, 400, 400, 500,
    150, 300, 300, 500, 300, 500, 500, 700,
    150, 300, 300, 500, 300, 500, 500, 700,
    300, 500, 500, 700, 500, 700, 700, 0,
};

/* 021A5708 */
static BOOL daKokiie_Draw(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &actor->current.pos, &actor->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(i_this->mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* kokiie_move (inlined into daKokiie_Execute) */
static inline void kokiie_move(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    i_this->m298 += 1;
    u32 mode = (u32)(s32)i_this->m29A;
    if (mode < 1) {
        int ropeMask = 0;
        for (int i = 0; i < 5; i++) {
            if (i_this->m32C[i] == 2) {
                i_this->m32C[i] = 0;
                i_this->m338 = 0.0f;
            }
            if (i_this->m32C[i] != 0) {
                if (i_this->m32C[i] == 1) {
                    i_this->m338 = 1.0f;
                    i_this->m32C[i] = 5;
                }
                ropeMask |= himo_off_check[i];
            }
        }
        s16 targetYaw = himo_off_ya[ropeMask];
        s16 target = himo_off_xa[ropeMask];
        cLib_addCalc2(&i_this->m2D0, REG8_F(4) - (f32)himo_off_yp[ropeMask], 0.1f, 50.0f * i_this->m338);
        if (ropeMask == 0x1F && dComIfGs_checkGetItem(dItemNo_BOOMERANG_e)) {
            actor->health = 0;
            i_this->m29A = 1;
            i_this->m378 = 1;
        }
        s16 maxSpeed = (s16)gabi::ftoi(10000.0f * i_this->m338);
        cLib_addCalcAngleS2(&i_this->m334, target, 4, maxSpeed);
        if (target != 0) {
            cLib_addCalcAngleS2(&i_this->m332, targetYaw, 2, maxSpeed);
        }
        cLib_addCalc2(&i_this->m338, 1.0f, 1.0f, REG8_F(14) + 0.001f);
        cLib_addCalcAngleS2(&actor->current.angle.x, 0, 10, 0x200);
        cLib_addCalcAngleS2(&actor->current.angle.z, 0, 10, 0x200);
        s16 t = i_this->m298;
        i_this->m2C8.x = (s16)gabi::ftoi(cM_ssin(t * 900) * i_this->m2BC);
        i_this->m2C8.z = (s16)gabi::ftoi(cM_ssin(t * 700) * i_this->m2C4);
        cLib_addCalc2(&i_this->m2BC, REG14_F(9) + 300.0f, 1.0f, REG14_F(3) + 20.0f);
        cLib_addCalc2(&i_this->m2C4, REG14_F(9) + 300.0f, 1.0f, REG14_F(3) + 20.0f);
        t = i_this->m298;
        i_this->m2A4.x = cM_ssin(t * 0x2EE) * i_this->m2B0;
        i_this->m2A4.z = cM_ssin(t * 900) * i_this->m2B8;
        cLib_addCalc0(&i_this->m2B0, 1.0f, 0.25f);
        cLib_addCalc0(&i_this->m2B8, 1.0f, 0.25f);
        /* actor->shape_angle = actor->current.angle + i_this->m2C8 */
        u32 w0, w1;
        csXyz_pl_regs(&actor->current.angle, &i_this->m2C8, &w0, &w1);
        actor->shape_angle.x = (s16)(w0 >> 16);
        actor->shape_angle.y = (s16)w0;
        actor->shape_angle.z = (s16)(w1 >> 16);
        /* actor->current.pos = actor->home.pos + i_this->m2A4 */
        gabi::Local<cXyz> sum;
        cXyz_pl(&actor->home.pos, sum, &i_this->m2A4);
        actor->current.pos.x = sum->x;
        actor->current.pos.y = sum->y;
        actor->current.pos.z = sum->z;
        actor->current.pos.y = actor->current.pos.y + i_this->m2D0;
        cLib_addCalc0(&i_this->m2A4.y, 0.05f, REG0_F(7) + 2.0f);
    } else if (mode == 1) {
        cLib_addCalcAngleS2(&actor->shape_angle.x, 0, 10, 0x800);
        cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 10, 0x800);
        cLib_addCalcAngleS2(&i_this->m334, 0, 4, 0x200);
        f32 vy = actor->speed.y;
        actor->current.pos.y = actor->current.pos.y + vy;
        actor->speed.y = vy - (REG12_F(14) + 3.0f);
        if (i_this->m37A == (s16)(REG0_S(4) + 0x1B)) {
            if (i_this->m2A2 != 0xFF) {
                dComIfGs_onSwitch(i_this->m2A2, fopAcM_GetRoomNo(i_this));
            }
            fopAcM_seStartCurrent(i_this, JA_SE_OBJ_KOKIRI_H_CRASH, 0);
            StartShock(0, 2, 4);
        }
        if (i_this->m37A == (s16)(REG0_S(5) + 0x40)) {
            gabi::Local<cXyz> sp44;
            sp44->x = actor->current.pos.x;
            sp44->y = actor->current.pos.y;
            sp44->z = actor->current.pos.z;
            sp44->y = 1170.0f;
            dComIfGp_particle_set(ID_AK_SN_KOKIRIHOUSESPLASH00, sp44);
            dComIfGp_particle_set(ID_AK_SN_KOKIRIHOUSESPLASH01, sp44);
            dComIfGp_particle_set(ID_AK_SN_KOKIRIHOUSEHAMON00, &actor->current.pos);
            dComIfGp_particle_set(ID_AK_SN_KOKIRIHOUSEHAMON01, sp44);
        }
        i_this->m374 = REG0_F(9) + 1380.0f; // HD: (REG0_F(9) + 5180) - 3800 is folded.
        if (!(actor->current.pos.y > i_this->m374)) {
            actor->current.pos.y = i_this->m374;
            if (actor->speed.y < -20.0f) {
                StartShock(6, 2, 7);
                fopAcM_seStartCurrent(i_this, JA_SE_OBJ_KOKIRI_H_LANDING, 0);
                i_this->m39C = REG12_F(10) + 90.0f; // HD: REG12_F(10) + 40 + 50 is folded.
            }
            actor->speed.y = 0.0f;
            cLib_addCalcAngleS2(&i_this->m334, 0, 2, 0x2000);
        }
    }
}

/* demo_camera (inlined into daKokiie_Execute); returns false where it returns early */
static inline bool demo_camera(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    // HD: the camera object follows a 0x248-byte process header.
    auto camera = reinterpret_cast<KokiieCameraProcess*>(dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)));
    KokiieCamera* cam = &camera->mCamera;
    cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
    u32 mode = (u32)(s32)i_this->m378;
    if (mode == 1 || mode == 2) {
        if (mode == 1) {
            if (!eventInfo_checkCommandDemoAccrpt(actor)) {
                fopAcM_orderPotentialEvent(actor, 2, 0xFFFF, 0);
                eventInfo_onCondition(actor, 2 /* dEvtCnd_UNK2_e */);
                return false;
            }
            i_this->m378 = (s16)(mode + 1);
            Kokiie_cameraStop(cam);
            Kokiie_cameraTrim(cam, 2);
            i_this->m37C.copy(actor->current.pos);
            gabi::Local<cXyz> sp5C, sp50;
            sp5C->x = REG12_F(2) + 500.0f; // HD: (REG12_F(2) + 1300) - 800 is folded.
            sp5C->y = 0.0f;
            sp5C->z = REG12_F(4) - 1000.0f;
            MtxPosition(sp5C, sp50);
            f32 px = actor->current.pos.x;
            i_this->m37C.x = px + sp50->x;
            i_this->m37C.y = REG12_F(5) + 1730.0f; // HD: ((REG12_F(5) + 4030) - 3800) + 1500 is folded.
            f32 pz = actor->current.pos.z;
            i_this->m37C.z = pz + sp50->z;
            i_this->m388.x = px;
            f32 py = actor->current.pos.y;
            i_this->m388.y = py;
            i_this->m388.z = pz;
            i_this->m388.y = py + (REG12_F(6) + -450.0f);
            i_this->m3A0 = REG12_F(18) + 65.0f;
            i_this->m37A = 0;
        }
        if (i_this->m37A < (s16)(REG12_S(3) + 10)) {
            actor->speed.y = 0.0f;
        }
        cLib_addCalc2(&i_this->m388.y, (actor->current.pos.y - 450.0f) + REG12_F(6), REG12_F(7) + 0.5f, REG12_F(8) + 200.0f);
        if (i_this->m37A > (s16)(REG12_S(4) + 0x32)) {
            cLib_addCalc2(&i_this->m3A0, REG12_F(19) + 80.0f, 0.02f, REG12_F(20) + 0.5f);
        }
        if (i_this->m37A == 100) {
            mDoAud_seStart_noPos(JA_SE_READ_RIDDLE_1);
        }
        if (i_this->m37A == 0x96 && REG0_S(3) == 0) {
            i_this->m378 = 0;
            dComIfGp_event_reset();
            gabi::Local<cXyz> center, eye;
            center->x = i_this->m388.x;
            center->y = i_this->m388.y;
            center->z = i_this->m388.z;
            eye->x = i_this->m37C.x;
            eye->y = i_this->m37C.y;
            eye->z = i_this->m37C.z;
            Kokiie_cameraReset(cam, center, eye);
            Kokiie_cameraStart(cam);
            Kokiie_cameraTrim(cam, 0);
        }
    }
    if (i_this->m378 != 0) {
        s16 cnt = i_this->m37A;
        f32 sin = cM_ssin(cnt * 0x3300) * i_this->m39C;
        f32 cos = cM_scos(cnt * 0x3000) * i_this->m39C;
        gabi::Local<cXyz> sp38, sp44;
        sp44->x = i_this->m37C.x + sin;
        sp38->x = i_this->m388.x + sin;
        sp38->y = i_this->m388.y + cos;
        sp44->y = i_this->m37C.y + cos;
        sp44->z = i_this->m37C.z;
        sp38->z = i_this->m388.z;
        s16 iVar1 = (s16)gabi::ftoi(cM_scos(i_this->m298 * 0x1C00) * i_this->m39C * 8.0f);
        Kokiie_cameraSet(cam, sp38, sp44, iVar1, i_this->m3A0);
        cLib_addCalc0(&i_this->m39C, 1.0f, REG0_F(16) + 5.0f);
        JUTReport(0x19A, 0x1AE, "K SUB  COUNT  %d", i_this->m37A);
        i_this->m37A += 1;
    }
    return true;
}

/* 021A57A0 */
// HD: kokiie_move and demo_camera inlined
static BOOL daKokiie_Execute(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: two accessor calls whose results are unused */
    dComIfGp_get();
    kokiie_move(i_this);
    MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), actor->shape_angle.y);
    cMtx_YrotM(calc_mtx(), i_this->m332);
    cMtx_XrotM(calc_mtx(), i_this->m334);
    cMtx_YrotM(calc_mtx(), (s16)-i_this->m332);
    cMtx_XrotM(calc_mtx(), actor->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), actor->shape_angle.z);
    cMtx_YrotM(calc_mtx(), 0xB54);
    i_this->mpModel->setBaseTRMtx(calc_mtx());
    PSMTXCopy(calc_mtx(), &i_this->m340);
    dBgW_Move(i_this->pm_bgw);
    cMtx_YrotM(calc_mtx(), -0xB54);
    for (s32 i = 0; i < 5; i++) {
        MtxPush();
        cMtx_YrotM(calc_mtx(), (s16)(i * 0x3333));
        gabi::Local<cXyz> sp08;
        sp08->x = 0.0f;
        sp08->y = REG0_F(5) + -170.0f;
        sp08->z = REG0_F(6) + 400.0f;
        MtxPosition(sp08, &i_this->m2F0[i]);
        MtxPull();
    }
    demo_camera(i_this);
    return TRUE;
}

/* 021A63B8 */
static BOOL daKokiie_IsDelete(kokiie_class*) {
    return TRUE;
}

/* 021A63C0 */
static BOOL daKokiie_Delete(kokiie_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Kokiie"); /* resDeleteDemo */
    if (i_this->heap != nullptr) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, i_this->pm_bgw);
    }
    return TRUE;
}

/* 021A6418 */
static BOOL CallbackCreateHeap(fopAc_ac_c* a_this) {
    kokiie_class* actor = (kokiie_class*)a_this;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Kokiie", dRes_INDEX_KOKIIE_BDL_KOKI_00_e, SAFESTRING_VTBL);
    actor->mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (actor->mpModel == nullptr) {
        return FALSE;
    }
    if (modelData == nullptr) /* JUT_ASSERT(945, modelData != NULL) */
        JUT_ASSERT_fail(__FILE_NAME__, 0x3B1, "modelData != NULL");
    actor->pm_bgw = new_dBgW();
    if (actor->pm_bgw == nullptr) /* JUT_ASSERT(950, actor->pm_bgw != NULL) */
        JUT_ASSERT_fail(__FILE_NAME__, 0x3B6, "actor->pm_bgw != NULL");
    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes("Kokiie", dRes_INDEX_KOKIIE_DZB_KOKI_00_e, SAFESTRING_VTBL);
    cBgW_Set(actor->pm_bgw, dzb, cBgW_MOVE_BG_e, &actor->m340);
    /* SetCrrFunc(dBgS_MoveBGProc_Typical) */
    reinterpret_cast<KokiieBgW*>(actor->pm_bgw.get())->mCrrFunc = 0x024EE658;
    return TRUE;
}

/* himo_create (inlined into daKokiie_Create) */
static inline BOOL himo_create(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->m2A2 != 0xFF && fopAcM_isSwitch(actor, i_this->m2A2)) {
        return FALSE;
    }
    s32 createdCount = 0;
    for (s32 i = 0; i < 5; i++) {
        u32 st = i_this->m2E8[i];
        if (st < 1) {
            KokiieAppend* p = Kokiie_createAppend();
            p->position.x = actor->current.pos.x;
            p->position.y = actor->current.pos.y;
            p->position.z = actor->current.pos.z;
            s16 ay = (s16)(actor->current.angle.y + i * 0x3333 + -13000);
            p->parameters = 0xFFFFFF01;
            p->angle.y = ay;
            p->roomNo = actor->current.roomNo;
            i_this->m2D4[i] = fopAcM_Create(fpcNm_SHAND_e, p);
            i_this->m2E8[i] += 1;
        } else if (st != 1) {
            continue;
        }
        /* case 1 */
        fopAc_ac_c* shand = fopAcM_SearchByID(i_this->m2D4[i]);
        if (shand != nullptr) {
            auto child = static_cast<KokiieShand*>(shand);
            child->field_308 = fopAcM_GetID(actor);
            child->field_310 = &i_this->m2F0[i];
            child->field_314 = &i_this->m32C[i];
            i_this->m2E8[i] += 1;
            createdCount++;
        }
    }
    return createdCount < 5;
}

/* 021A652C */
static cPhs_State daKokiie_Create(fopAc_ac_c* a_this) {
    kokiie_class* i_this = (kokiie_class*)a_this;
    dComIfGp_get(); /* unused accessor call */
    fopAcM_ct(a_this, kokiie_class);
    cPhs_State phase = dComIfG_resLoad(&i_this->mPhase, "Kokiie");
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }
    u8 prm = fopAcM_GetParam(a_this) & 0xFF;
    i_this->m2A0 = (prm == 0xFF) ? 0 : prm;
    i_this->m2A2 = fopAcM_GetParam(a_this) >> 0x18;
    if (himo_create(i_this)) {
        return FALSE;
    }
    if (!fopAcM_entrySolidHeap(a_this, CallbackCreateHeap, 0x10000)) {
        return cPhs_ERROR_e;
    }
    if (i_this->pm_bgw != nullptr) {
        dBgS* bgs = dComIfG_Bgsp();
        if (dBgS_Regist(bgs, i_this->pm_bgw, a_this))
            return cPhs_ERROR_e;
    }
    switch (i_this->m2A0) {
    case 1:
        a_this->scale.x = 0.9f;
        a_this->scale.z = 0.9f;
        break;
    case 2:
        a_this->scale.x = 0.8f;
        a_this->scale.z = 0.8f;
        break;
    case 3:
        a_this->scale.x = 0.7f;
        a_this->scale.z = 0.7f;
        break;
    default:
        a_this->scale.z = 1.0f;
        a_this->scale.x = 1.0f;
        break;
    }
    a_this->scale.y = 1.0f;
    fopAcM_SetMtx(a_this, i_this->mpModel->getBaseTRMtx());
    fopAcM_SetMin(a_this, -1000.0f * a_this->scale.x, -1500.0f, -1000.0f * a_this->scale.z);
    fopAcM_SetMax(a_this, 1000.0f * a_this->scale.x, 1000.0f, 1000.0f * a_this->scale.z);
    i_this->mpModel->setBaseScale(a_this->scale);
    a_this->health = 1;
    if (i_this->m2A2 != 0xFF && fopAcM_isSwitch(a_this, i_this->m2A2)) {
        a_this->current.pos.y = (i_this->m374 + 770.0f) + REG0_F(17); /* m374 + 50 + 720 + REG0_F(17) */
        i_this->m29A = 1;
        a_this->health = 0;
    }
    for (s32 i = 0; i < 2; i++) {
        daKokiie_Execute(i_this);
    }
    return phase;
}

/* 021A696C */
// HD: static initialisation of the translation unit (header statics only)
static void __sinit_d_a_kokiie_cpp() {
    sinit_header_statics(0x10464C3C, 0x101B8774);
}

/* 021A6A00 */
// HD: sead::SafeString deleting destructor (this TU's vtable 0x100134AC)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 021A6A14 */
// HD: kokiie_class deleting destructor (compiler-generated, HD virtual destructor)
static void kokiie_class_dt(kokiie_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021A6A68 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 10013568 "Kokiie" in 021A63C0
str 10013570 "Kokiie" in 021A6418
str 100135CC "Kokiie" in 021A652C
str 10013578 "d_a_kokiie.cpp" in 021A6418
str 10013588 "modelData != NULL"
str 1001359C "actor->pm_bgw != NULL"
str 100134D4 "K SUB  COUNT  %d"
data himo_off_check 101B868C
data himo_off_ya 101B86B4
data himo_off_xa 101B86F4
data himo_off_yp 101B8734
ext 025D5600 KokiieAppend* Kokiie_createAppend()
ext 025DED64 void* Kokiie_currentLayer()
ext 025E14A8 u32 Kokiie_request(void*, s16, u32, u32, void*)
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32)
ext 025D7B24 BOOL fopAcM_orderPotentialEvent(fopAc_ac_c*, u16, u16, u16)
ext 025E1988 void mDoAud_seStart_noPos(u32)
ext 02514F2C void Kokiie_cameraStop(KokiieCamera*)
ext 02514F38 void Kokiie_cameraStart(KokiieCamera*)
ext 02515280 void Kokiie_cameraTrim(KokiieCamera*, s32)
ext 0251510C void Kokiie_cameraReset(KokiieCamera*, cXyz*, cXyz*)
ext 02514FE8 void Kokiie_cameraSet(KokiieCamera*, cXyz*, cXyz*, s16, f32)
ext 027EC9E8 void JUTReport(s32, s32, const char*, s32)
ext 0201A4DC u32 Kokiie_addAngles(const csXyz*, const csXyz*)
*/
