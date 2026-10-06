/**
 * d_a_kokiie.cpp (WWHD)
 * Object - Forbidden Woods - Hanging flower house
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kokiie.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KOKIIE_VTBL 0x100134C4     /* kokiie_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x100134AC /* this TU's sead::SafeString vtable */
enum { dRes_INDEX_KOKIIE_BDL_KOKI_00_e = 4, dRes_INDEX_KOKIIE_DZB_KOKI_00_e = 7 };
enum { fpcNm_SHAND_e = 0x63 };
enum { dItemNo_BOOMERANG_e = 0x2D };
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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u8* fopAcM_CreateAppend() { return gabi::call<u8*>(0x025D5600); }
static inline void* fpcLy_CurrentLayer() { return gabi::call<void*>(0x025DED64); }
/* fopAcM_Create(name, NULL, append) (HD: inline fpcSCtRq_Request(layer, name, 0, 0, append)) */
static inline u32 fopAcM_Create(s16 name, void* append) {
    void* layer = fpcLy_CurrentLayer();
    return gabi::call<u32>(0x025E14A8, layer, name, 0, 0, append);
}
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* 025D672C / 025D673C fopAcM_SetMin / fopAcM_SetMax (HD: out of line) */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
static inline BOOL fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 flag, u16 hind, u16 p) { return gabi::call<BOOL>(0x025D7B24, a, flag, hind, p); }
/* 025E1988: HD mDoAud_seStart(id) without a position */
static inline void mDoAud_seStart_noPos(u32 id) { gabi::call(0x025E1988, id); }
/* dCamera_c (camera_process_class + 0x248) */
static inline void dCamera_Stop(u32 cam) { gabi::call(0x02514F2C, cam); }
static inline void dCamera_Start(u32 cam) { gabi::call(0x02514F38, cam); }
static inline void dCamera_SetTrimSize(u32 cam, s32 size) { gabi::call(0x02515280, cam, size); }
static inline void dCamera_Reset(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, cam, center, eye); } /* by-value copies */
static inline void dCamera_Set(u32 cam, cXyz* center, cXyz* eye, s16 bank, f32 fovy) { gabi::call(0x02514FE8, cam, center, eye, bank, fovy); }
/* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): camera id s8 at play+0x5B30, cameras at play+0x5AF8 (0x34 each) */
static inline u32 dComIfGp_getPlayerCamera0() {
    s32 id = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    return gabi::load<u32>(dComIfGp_ea() + id * 0x34 + 0x5AF8);
}
/* 027EC9E8 JUTReport(x, y, fmt, ...) */
static inline void JUTReport(s32 x, s32 y, const char* fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }
/* csXyz::operator+ (0201A4DC): the result comes back in r3/r4 */
static inline void csXyz_pl_regs(const csXyz* a, const csXyz* b, u32* w0, u32* w1) {
    *w0 = gabi::call<u32>(0x0201A4DC, a, b);
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
    gabi::call(0x025CB374, vib, s, -0x21, v.get());
}

struct kokiie_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<s16> m298;
    /* 0x3B6 */ be<s16> m29A;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> m2A0;
    /* 0x3BD */ u8 _3BD;
    /* 0x3BE */ be<u8> m2A2;
    /* 0x3BF */ u8 _3BF;
    /* 0x3C0 */ cXyz m2A4;
    /* 0x3CC */ be<f32> m2B0;
    /* 0x3D0 */ u8 _3D0[4];
    /* 0x3D4 */ be<f32> m2B8;
    /* 0x3D8 */ be<f32> m2BC;
    /* 0x3DC */ u8 _3DC[4];
    /* 0x3E0 */ be<f32> m2C4;
    /* 0x3E4 */ csXyz m2C8;
    /* 0x3EA */ u8 _3EA[2];
    /* 0x3EC */ be<f32> m2D0;
    /* 0x3F0 */ be<u32> m2D4[5];
    /* 0x404 */ be<u8> m2E8[5];
    /* 0x409 */ u8 _409[3];
    /* 0x40C */ cXyz m2F0[5];
    /* 0x448 */ be<u8> m32C[5];
    /* 0x44D */ u8 _44D;
    /* 0x44E */ be<s16> m332;
    /* 0x450 */ be<s16> m334;
    /* 0x452 */ u8 _452[2];
    /* 0x454 */ be<f32> m338;
    /* 0x458 */ u8 _458[4];
    /* 0x45C */ Mtx34 m340;
    /* 0x48C */ gptr<dBgW> pm_bgw;
    /* 0x490 */ be<f32> m374;
    /* 0x494 */ be<s16> m378;
    /* 0x496 */ be<s16> m37A;
    /* 0x498 */ cXyz m37C;
    /* 0x4A4 */ cXyz m388;
    /* 0x4B0 */ u8 _4B0[8];
    /* 0x4B8 */ be<f32> m39C;
    /* 0x4BC */ be<f32> m3A0;
};
WWHD_OFFSET(kokiie_class, m2F0, 0x40C);
WWHD_OFFSET(kokiie_class, m340, 0x45C);
WWHD_OFFSET(kokiie_class, m3A0, 0x4BC);
WWHD_SIZE(kokiie_class, 0x4C0);

/* static tables of kokiie_move (.data) */
static u8 himo_off_check(int i) { return gabi::load<u8>(0x101B868C + i); }
static s16 himo_off_ya(int i) { return gabi::load<s16>(0x101B86B4 + 2 * i); }
static s16 himo_off_xa(int i) { return gabi::load<s16>(0x101B86F4 + 2 * i); }
static s16 himo_off_yp(int i) { return gabi::load<s16>(0x101B8734 + 2 * i); }

/* 021A5708 */
static BOOL daKokiie_Draw(kokiie_class* i_this) {
    WWHD_FUNC(0x021A5708, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &actor->current.pos, &actor->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(i_this->mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x021A5708, daKokiie_Draw);

/* kokiie_move (inlined into daKokiie_Execute) */
static inline void kokiie_move(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    i_this->m298 += 1;
    u32 mode = (u32)(s32)i_this->m29A;
    if (mode < 1) {
        int uVar6 = 0;
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
                uVar6 |= himo_off_check(i);
            }
        }
        s16 sVar1 = himo_off_ya(uVar6);
        s16 target = himo_off_xa(uVar6);
        cLib_addCalc2(&i_this->m2D0, REG8_F(4) - (f32)himo_off_yp(uVar6), 0.1f, 50.0f * i_this->m338);
        if (uVar6 == 0x1F && dComIfGs_checkGetItem(dItemNo_BOOMERANG_e)) {
            actor->health = 0;
            i_this->m29A = 1;
            i_this->m378 = 1;
        }
        s16 maxSpeed = (s16)gabi::ftoi(10000.0f * i_this->m338);
        cLib_addCalcAngleS2(&i_this->m334, target, 4, maxSpeed);
        if (target != 0) {
            cLib_addCalcAngleS2(&i_this->m332, sVar1, 2, maxSpeed);
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
        i_this->m374 = REG0_F(9) + 1380.0f; /* (REG0_F(9) + 5180) - 3800, folded */
        if (!(actor->current.pos.y > i_this->m374)) {
            actor->current.pos.y = i_this->m374;
            if (actor->speed.y < -20.0f) {
                StartShock(6, 2, 7);
                fopAcM_seStartCurrent(i_this, JA_SE_OBJ_KOKIRI_H_LANDING, 0);
                i_this->m39C = REG12_F(10) + 90.0f; /* REG12_F(10) + 40 + 50, folded */
            }
            actor->speed.y = 0.0f;
            cLib_addCalcAngleS2(&i_this->m334, 0, 2, 0x2000);
        }
    }
}

/* demo_camera (inlined into daKokiie_Execute); returns false where it returns early */
static inline bool demo_camera(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    u32 camera = dComIfGp_getPlayerCamera0();
    u32 cam = camera + 0x248; /* camera->mCamera */
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
            dCamera_Stop(cam);
            dCamera_SetTrimSize(cam, 2);
            i_this->m37C.copy(actor->current.pos);
            gabi::Local<cXyz> sp5C, sp50;
            sp5C->x = REG12_F(2) + 500.0f; /* (REG12_F(2) + 1300) - 800, folded */
            sp5C->y = 0.0f;
            sp5C->z = REG12_F(4) - 1000.0f;
            MtxPosition(sp5C, sp50);
            f32 px = actor->current.pos.x;
            i_this->m37C.x = px + sp50->x;
            i_this->m37C.y = REG12_F(5) + 1730.0f; /* ((REG12_F(5) + 4030) - 3800) + 1500, folded */
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
            dCamera_Reset(cam, center, eye);
            dCamera_Start(cam);
            dCamera_SetTrimSize(cam, 0);
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
        dCamera_Set(cam, sp38, sp44, iVar1, i_this->m3A0);
        cLib_addCalc0(&i_this->m39C, 1.0f, REG0_F(16) + 5.0f);
        JUTReport(0x19A, 0x1AE, STR(0x100134D4) /* "K SUB  COUNT  %d" */, i_this->m37A);
        i_this->m37A += 1;
    }
    return true;
}

/* 021A57A0: kokiie_move and demo_camera inlined */
static BOOL daKokiie_Execute(kokiie_class* i_this) {
    WWHD_FUNC(0x021A57A0, BOOL, i_this);
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
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
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
VERIFY(0x021A57A0, daKokiie_Execute);

/* 021A63B8 */
static BOOL daKokiie_IsDelete(kokiie_class*) {
    WWHD_FUNC(0x021A63B8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021A63B8, daKokiie_IsDelete);

/* 021A63C0 */
static BOOL daKokiie_Delete(kokiie_class* i_this) {
    WWHD_FUNC(0x021A63C0, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10013568) /* "Kokiie" */); /* resDeleteDemo */
    if (i_this->heap != nullptr) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, i_this->pm_bgw);
    }
    return TRUE;
}
VERIFY(0x021A63C0, daKokiie_Delete);

/* 021A6418 */
static BOOL CallbackCreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021A6418, BOOL, a_this);
    kokiie_class* actor = (kokiie_class*)a_this;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10013570), dRes_INDEX_KOKIIE_BDL_KOKI_00_e, SAFESTRING_VTBL);
    actor->mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (actor->mpModel == nullptr) {
        return FALSE;
    }
    if (modelData == nullptr) /* JUT_ASSERT(945, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10013578), 0x3B1, STR(0x10013588));
    actor->pm_bgw = new_dBgW();
    if (actor->pm_bgw == nullptr) /* JUT_ASSERT(950, actor->pm_bgw != NULL) */
        JUT_ASSERT_fail(STR(0x10013578), 0x3B6, STR(0x1001359C));
    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(STR(0x10013570), dRes_INDEX_KOKIIE_DZB_KOKI_00_e, SAFESTRING_VTBL);
    cBgW_Set(actor->pm_bgw, dzb, cBgW_MOVE_BG_e, &actor->m340);
    /* SetCrrFunc(dBgS_MoveBGProc_Typical) */
    gabi::store<u32>(gabi::ea(actor->pm_bgw.get()) + 0xA8, 0x024EE658);
    return TRUE;
}
VERIFY(0x021A6418, CallbackCreateHeap);

/* himo_create (inlined into daKokiie_Create) */
static inline BOOL himo_create(kokiie_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->m2A2 != 0xFF && fopAcM_isSwitch(actor, i_this->m2A2)) {
        return FALSE;
    }
    s32 uVar2 = 0;
    for (s32 i = 0; i < 5; i++) {
        u32 st = i_this->m2E8[i];
        if (st < 1) {
            u8* p = fopAcM_CreateAppend();
            u32 pa = gabi::ea(p);
            gabi::store<f32>(pa + 4, actor->current.pos.x);
            gabi::store<f32>(pa + 8, actor->current.pos.y);
            gabi::store<f32>(pa + 0xC, actor->current.pos.z);
            s16 ay = (s16)(actor->current.angle.y + i * 0x3333 + -13000);
            gabi::store<u32>(pa + 0, 0xFFFFFF01);
            gabi::store<s16>(pa + 0x12, ay);
            gabi::store<s8>(pa + 0x21, actor->current.roomNo);
            i_this->m2D4[i] = fopAcM_Create(fpcNm_SHAND_e, p);
            i_this->m2E8[i] += 1;
        } else if (st != 1) {
            continue;
        }
        /* case 1 */
        fopAc_ac_c* shand = fopAcM_SearchByID(i_this->m2D4[i]);
        if (shand != nullptr) {
            u32 s = gabi::ea(shand);
            gabi::store<u32>(s + 0x424, fopAcM_GetID(actor));       /* field_308 */
            gabi::store<u32>(s + 0x42C, gabi::ea(&i_this->m2F0[i])); /* field_310 */
            gabi::store<u32>(s + 0x430, gabi::ea(&i_this->m32C[i])); /* field_314 */
            i_this->m2E8[i] += 1;
            uVar2++;
        }
    }
    return uVar2 < 5;
}

/* 021A652C */
static cPhs_State daKokiie_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021A652C, cPhs_State, a_this);
    kokiie_class* i_this = (kokiie_class*)a_this;
    dComIfGp_get(); /* unused accessor call */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = KOKIIE_VTBL;
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar3 = dComIfG_resLoad(&i_this->mPhase, STR(0x100135CC) /* "Kokiie" */);
    if (PVar3 != cPhs_COMPLEATE_e) {
        return PVar3;
    }
    u8 prm = fopAcM_GetParam(a_this) & 0xFF;
    i_this->m2A0 = (prm == 0xFF) ? 0 : prm;
    i_this->m2A2 = fopAcM_GetParam(a_this) >> 0x18;
    if (himo_create(i_this)) {
        return FALSE;
    }
    if (!fopAcM_entrySolidHeap(a_this, 0x021A6418 /* CallbackCreateHeap */, 0x10000)) {
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
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
    fopAcM_SetMin(a_this, -1000.0f * a_this->scale.x, -1500.0f, -1000.0f * a_this->scale.z);
    fopAcM_SetMax(a_this, 1000.0f * a_this->scale.x, 1000.0f, 1000.0f * a_this->scale.z);
    J3DModel_setBaseScale(i_this->mpModel, &a_this->scale);
    a_this->health = 1;
    if (i_this->m2A2 != 0xFF && fopAcM_isSwitch(a_this, i_this->m2A2)) {
        a_this->current.pos.y = (i_this->m374 + 770.0f) + REG0_F(17); /* m374 + 50 + 720 + REG0_F(17) */
        i_this->m29A = 1;
        a_this->health = 0;
    }
    for (s32 i = 0; i < 2; i++) {
        daKokiie_Execute(i_this);
    }
    return PVar3;
}
VERIFY(0x021A652C, daKokiie_Create);

/* 021A696C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kokiie_cpp() {
    WWHD_FUNC(0x021A696C, void, (u32)0);
    sinit_header_statics(0x10464C3C, 0x101B8774);
}
VERIFY(0x021A696C, __sinit_d_a_kokiie_cpp);

/* 021A6A00: sead::SafeString deleting destructor (this TU's vtable 0x100134AC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021A6A00, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x021A6A00, SafeString_dt);

/* 021A6A14: kokiie_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kokiie_class_dt(kokiie_class* i_this, s32 flags) {
    WWHD_FUNC(0x021A6A14, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021A6A14, kokiie_class_dt);

/* 021A6A68: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x021A6A68, void, (u32)0);
}
VERIFY(0x021A6A68, SafeString_assureTerminationImpl);
