/**
 * d_a_msw.cpp (WWHD)
 * Object - swinging platform hung on four chains.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_msw.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname_create STR(0x10015480) /* "Msw" (resLoad) */
#define M_arcname_delete STR(0x10015444) /* "Msw" (resDelete) */
#define M_arcname_res STR(0x10015448)    /* "Msw" (getRes) */
#define SAFESTRING_VTBL 0x100153D4
#define ACT_VTBL 0x100153FC     /* HD: msw_class vtable */
#define TU_AAB_VTBL 0x100153EC  /* this TU's cM3dGAab vtable */
#define himo_cyl_src gabi::at<dCcD_SrcCyl>(0x101BAD28)

enum {
    dRes_INDEX_MSW_BDL_MSWNG_e = 4,
    dRes_INDEX_MSW_BDL_OBM_CHAIN1_e = 5,
    dRes_INDEX_MSW_DZB_MSWING_e = 9,
};
enum {
    JA_SE_LK_HIT_SBRIDGE_CHAIN = 0x2839,
    ID_AK_JN_NG = 0x000C,
};

#define REG6_F(i) REG_F(6, i)
#define REG6_S(i) REG_S(6, i)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD: fopAcM_SetMin / fopAcM_SetMax are out of line (025D672C / 025D673C), as d_a_bita/mo2 */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* dBgW: SetCrrFunc +0xA8, SetRideCallback +0xB0; dBgS_MoveBGProc_Typical 024EE658 (as d_a_kita) */
static inline void dBgW_SetCrrFunc(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xA8, fn); }
static inline void dBgW_SetRideCallback(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xB0, fn); }
/* cBgW::Set: the caller tests the result against TRUE (cmpwi 1), not as a bool */
static inline BOOL cBgW_Set_i(dBgW* w, cBgD_t* data, u32 flags, Mtx34* mtx) { return gabi::call<BOOL>(0x0200A030, w, data, flags, mtx); }
/* csXyz::operator+ (0201A4DC) returns the 6-byte csXyz in r3:r4 (as d_a_kita) */
static inline void csXyz_pl_to(csXyz* dst, csXyz* a, csXyz* b) {
    u32 hi = gabi::call<u32>(0x0201A4DC, a, b);
    u32 lo = gabi::cpu->r[4];
    dst->x = (s16)(hi >> 16);
    dst->y = (s16)hi;
    dst->z = (s16)(lo >> 16);
}
/* GHS runtime array helpers (as d_a_kb.h) */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags, 0); }
/* mDoAud_seStart without the fopAcM_seStart null checks */
static inline void JAIZelBasic_seStart(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A40, id, pos, param, reverb); }

struct msw_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s16> m298;
    /* 0x3B6 */ be<s16> m29A;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> m2A0;
    /* 0x3BD */ u8 _3BD[3];
    /* 0x3C0 */ cXyz m2A4;
    /* 0x3CC */ cXyz m2B0;
    /* 0x3D8 */ cXyz m2BC;
    /* 0x3E4 */ csXyz m2C8;
    /* 0x3EA */ u8 _3EA[2];
    /* 0x3EC */ gptr<J3DModel> mpChainModels[4];
    /* 0x3FC */ cXyz m2E0[4];
    /* 0x42C */ cXyz m310[4];
    /* 0x45C */ dCcD_Stts mStts;
    /* 0x498 */ dCcD_Cyl mChainCyls[4];
    /* 0x958 */ be<s16> m83C[4];
    /* 0x960 */ be<s16> m844;
    /* 0x962 */ u8 m846[0x968 - 0x962];
    /* 0x968 */ Mtx34 mMtx;
    /* 0x998 */ gptr<dBgW> mpBgW;
};
WWHD_OFFSET(msw_class, mpModel, 0x3B8);
WWHD_OFFSET(msw_class, m2C8, 0x3E4);
WWHD_OFFSET(msw_class, mStts, 0x45C);
WWHD_OFFSET(msw_class, mChainCyls, 0x498);
WWHD_OFFSET(msw_class, m83C, 0x958);
WWHD_OFFSET(msw_class, mMtx, 0x968);
WWHD_OFFSET(msw_class, mpBgW, 0x998);
WWHD_SIZE(msw_class, 0x99C);

/* 021D6270 */
static void ride_call_back(dBgW* bgw, fopAc_ac_c* i_ac, fopAc_ac_c* i_pt) {
    WWHD_FUNC(0x021D6270, void, bgw, i_ac, i_pt);
    msw_class* pActor = static_cast<msw_class*>(i_ac);

    cMtx_YrotS(calc_mtx(), -i_ac->current.angle.y);

    gabi::Local<cXyz> res, tmp, pos1, pos2;
    cXyz_mi(&i_pt->current.pos, res.get(), &i_ac->current.pos); /* tmp = i_pt->current.pos - i_ac->current.pos */
    tmp->copy(*res);
    MtxPosition(tmp.get(), pos1.get());
    cXyz_mi(&i_pt->old.pos, res.get(), &i_ac->current.pos);
    tmp->copy(*res);
    MtxPosition(tmp.get(), pos2.get());

    f32 k = REG0_F(0) + 20.0f;
    f32 kx = k / i_ac->scale.x;
    f32 kz = k / i_ac->scale.z;
    s16 z = (s16)gabi::ftoi(pos1->z * kz);
    s16 x = (s16)gabi::ftoi(-(pos1->x * kx));

    cLib_addCalcAngleS2(&i_ac->current.angle.x, z, 10, 0x800);
    cLib_addCalcAngleS2(&i_ac->current.angle.z, x, 10, 0x800);

    f32 dist = (REG0_F(4) + 50.0f) * __builtin_fabsf(pos1->z - pos2->z);
    if (pActor->m2BC.x < dist) {
        pActor->m2BC.x = dist;
    }

    dist = (REG0_F(4) + 50.0f) * __builtin_fabsf(pos1->x - pos2->x);
    if (pActor->m2BC.z < dist) {
        pActor->m2BC.z = dist;
    }

    dist = (REG0_F(8) + 5.0f) * __builtin_fabsf(pos1->x - pos2->x);
    if (dist > 10.0f && pActor->m2B0.x < dist) {
        cLib_addCalc2(&pActor->m2B0.x, dist, 1.0f, REG0_F(7) + 1.2f);
    }

    dist = (REG0_F(8) + 5.0f) * __builtin_fabsf(pos1->z - pos2->z);
    if (dist > 10.0f && pActor->m2B0.z < dist) {
        cLib_addCalc2(&pActor->m2B0.z, dist, 1.0f, REG0_F(7) + 1.2f);
    }
}
VERIFY(0x021D6270, ride_call_back);

/* chain_Draw (inlined into daMsw_Draw) */
static inline void chain_Draw(msw_class* i_this) {
    for (int i = 0; i < 4; i++) {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->m310[i], tmp.get(), &i_this->m2E0[i]);
        f32 ty = tmp->y, tz = tmp->z, tx = tmp->x;
        s16 angle1 = -cM_atan2s(ty, tz);
        s16 angle2 = cM_atan2s(tx, std_sqrtf(gabi::fmadds(ty, ty, tz * tz)));

        MtxTrans(i_this->m2E0[i].x, i_this->m2E0[i].y, i_this->m2E0[i].z, false);

        s16 var_r4;
        s16 var_r20;
        if (i_this->m83C[i] != 0) {
            s16 t = i_this->m298;
            var_r20 = (s16)gabi::ftoi(cM_ssin(t * 18000) * (f32)i_this->m83C[i] * (REG6_F(6) + 100.0f));
            var_r4 = (s16)(t * 3000);
        } else {
            var_r4 = 0;
            var_r20 = 0;
        }

        cMtx_YrotM(calc_mtx(), var_r4);
        cMtx_XrotM(calc_mtx(), (s16)(angle1 + var_r20));
        cMtx_YrotM(calc_mtx(), angle2);

        J3DModel_setBaseTRMtx(i_this->mpChainModels[i], calc_mtx());
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->mpChainModels[i], &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->mpChainModels[i]);
    }
}

/* 021D6518 */
static BOOL daMsw_Draw(msw_class* i_this) {
    WWHD_FUNC(0x021D6518, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, i_this->mpModel, &i_this->tevStr);

    mDoExt_modelUpdateDL(i_this->mpModel);

    dComIfGd_setListBG();
    chain_Draw(i_this);
    dComIfGd_setList();

    return TRUE;
}
VERIFY(0x021D6518, daMsw_Draw);

/* msw_move (inlined into daMsw_Execute) */
static inline void msw_move(msw_class* i_this) {
    i_this->m298 = (s16)(i_this->m298 + 1);

    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x200);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x200);

    s16 t = i_this->m298;
    i_this->m2C8.x = (s16)gabi::ftoi(cM_ssin(t * 1500) * i_this->m2BC.x);
    i_this->m2C8.z = (s16)gabi::ftoi(cM_ssin(t * 1300) * i_this->m2BC.z);

    cLib_addCalc2(&i_this->m2BC.x, REG0_F(9), 1.0f, REG0_F(3) + 20.0f);
    cLib_addCalc2(&i_this->m2BC.z, REG0_F(9), 1.0f, REG0_F(3) + 20.0f);

    t = i_this->m298;
    i_this->m2A4.x = cM_ssin(t * 750) * i_this->m2B0.x;
    i_this->m2A4.z = cM_ssin(t * 900) * i_this->m2B0.z;

    cLib_addCalc0(&i_this->m2B0.x, 1.0f, REG0_F(6) + 0.25f);
    cLib_addCalc0(&i_this->m2B0.z, 1.0f, REG0_F(6) + 0.25f);

    csXyz_pl_to(&i_this->shape_angle, &i_this->current.angle, &i_this->m2C8); /* shape_angle = current.angle + m2C8 */
    gabi::Local<cXyz> res;
    cXyz_pl(&i_this->home.pos, res.get(), &i_this->m2A4); /* current.pos = home.pos + m2A4 */
    i_this->current.pos.copy(*res);
}

/* static f32 xd[4] = {1, 1, -1, -1}, zd[4] = {1, -1, 1, -1} (.data) */
static f32 xd(int i) { return gabi::load<f32>(0x101BAD08 + 4 * i); }
static f32 zd(int i) { return gabi::load<f32>(0x101BAD18 + 4 * i); }

/* 021D6880 */
static BOOL daMsw_Execute(msw_class* i_this) {
    WWHD_FUNC(0x021D6880, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    dComIfGp_get(); /* HD: a second play-object access whose result is unused */

    msw_move(i_this);

    MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), actor->shape_angle.y);
    cMtx_XrotM(calc_mtx(), actor->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), actor->shape_angle.z);
    MtxScale(actor->scale.x, 1.0f, actor->scale.z, true);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
    MtxPush();

    for (int chainIdx = 0; chainIdx < 4; chainIdx++) {
        gabi::Local<cXyz> src;
        f32 k = REG0_F(10) + 200.0f;
        src->y = 0.0f;
        src->z = zd(chainIdx) * k;
        src->x = xd(chainIdx) * k;
        MtxPosition(src.get(), &i_this->m2E0[chainIdx]);

        if (i_this->m844 != 0) {
            i_this->m844 = (s16)(i_this->m844 - 1);
            i_this->m310[chainIdx].copy(i_this->m2E0[chainIdx]);
            i_this->m310[chainIdx].y = i_this->m2E0[chainIdx].y + 1000.0f;
        }

        if (i_this->m83C[chainIdx] != 0) {
            i_this->m83C[chainIdx] = (s16)(i_this->m83C[chainIdx] - 1);
        }

        if (i_this->mChainCyls[chainIdx].ChkTgHit() && i_this->m83C[chainIdx] < 10) {
            i_this->m83C[chainIdx] = (s16)(REG6_S(3) + 15);

            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(actor));
            JAIZelBasic_seStart(JA_SE_LK_HIT_SBRIDGE_CHAIN, &i_this->m2E0[chainIdx], 0, reverb);

            gabi::Local<cXyz> scale;
            scale->z = 2.0f;
            scale->y = 2.0f;
            scale->x = 2.0f;
            dComIfGp_particle_set(ID_AK_JN_NG, i_this->mChainCyls[chainIdx].GetTgHitPosP(), &player->shape_angle, scale.get());
        }
        i_this->mChainCyls[chainIdx].SetC(&i_this->m2E0[chainIdx]);

        dComIfG_Ccsp_Set(&i_this->mChainCyls[chainIdx]);
    }

    MtxPull();
    PSMTXCopy(calc_mtx(), &i_this->mMtx); /* MTXCopy */
    dBgW_Move(i_this->mpBgW);

    return TRUE;
}
VERIFY(0x021D6880, daMsw_Execute);

/* 021D6D50 */
static BOOL daMsw_IsDelete(msw_class* i_this) {
    WWHD_FUNC(0x021D6D50, BOOL, i_this);
    for (int i = 0; i < 4; i++) {
        mDoAud_seDeleteObject(&i_this->m2E0[i]);
    }

    return TRUE;
}
VERIFY(0x021D6D50, daMsw_IsDelete);

/* 021D6D9C */
static BOOL daMsw_Delete(msw_class* i_this) {
    WWHD_FUNC(0x021D6D9C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, M_arcname_delete); /* dComIfG_resDeleteDemo */
    cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);

    return TRUE;
}
VERIFY(0x021D6D9C, daMsw_Delete);

/* 021D6DE8 */
static BOOL daMsw_CreateInit(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021D6DE8, BOOL, i_this);
    msw_class* pActor = static_cast<msw_class*>(i_this);

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, dRes_INDEX_MSW_BDL_MSWNG_e, SAFESTRING_VTBL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    pActor->mpModel = model;

    if (model == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, dRes_INDEX_MSW_BDL_OBM_CHAIN1_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(523, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1001544C), 0x20B, STR(0x10015458));

    for (int chainIdx = 0; chainIdx < 4; chainIdx++) {
        model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        pActor->mpChainModels[chainIdx] = model;
        if (model == nullptr) {
            return FALSE;
        }
    }

    dBgW* bgw = new_dBgW();
    pActor->mpBgW = bgw;

    if (bgw == nullptr) {
        return FALSE;
    }

    cBgD_t* pBgd = (cBgD_t*)dComIfG_getObjectRes(M_arcname_res, dRes_INDEX_MSW_DZB_MSWING_e, SAFESTRING_VTBL);

    BOOL error = cBgW_Set_i(pActor->mpBgW, pBgd, cBgW_MOVE_BG_e, &pActor->mMtx);
    if (error == TRUE) {
        return FALSE;
    }

    dBgW_SetCrrFunc(pActor->mpBgW, 0x024EE658 /* dBgS_MoveBGProc_Typical */);
    dBgW_SetRideCallback(pActor->mpBgW, 0x021D6270 /* ride_call_back */);

    return TRUE;
}
VERIFY(0x021D6DE8, daMsw_CreateInit);

/* 021D6F48 */
static cPhs_State daMsw_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021D6F48, cPhs_State, i_this);
    msw_class* a_this = static_cast<msw_class*>(i_this);
    dComIfGp_get(); /* HD: unused play-object access */

    /* fopAcM_ct(i_this, msw_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = ACT_VTBL;
            dCcD_Stts_ct(&a_this->mStts);
            __construct_array(a_this->mChainCyls, 4, 0x130, 0x021D72B0 /* dCcD_Cyl::dCcD_Cyl */);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&a_this->mPhs, M_arcname_create);

    if (phase_state == cPhs_COMPLEATE_e) {
        u8 prm = (u8)(fopAcM_GetParam(a_this) >> 0);
        a_this->m2A0 = prm == 0xFF ? 0 : prm;

        if (!fopAcM_entrySolidHeap(a_this, 0x021D6DE8 /* daMsw_CreateInit */, 0x10040)) {
            return cPhs_ERROR_e;
        }

        if (a_this->mpModel == nullptr) {
            return cPhs_ERROR_e;
        }

        if (dBgS_Regist(dComIfG_Bgsp(), a_this->mpBgW, a_this)) {
            return cPhs_ERROR_e;
        }

        switch (a_this->m2A0) {
        case 1:
            a_this->scale.x = 1.5f;
            a_this->scale.z = 1.5f;
            break;
        case 2:
            a_this->scale.x = 2.0f;
            a_this->scale.z = 2.0f;
            break;
        case 3:
            a_this->scale.x = 3.0f;
            a_this->scale.z = 3.0f;
            break;
        default:
            a_this->scale.z = 1.0f;
            a_this->scale.x = 1.0f;
            break;
        }

        a_this->scale.y = 1.0f;
        J3DModel* model = a_this->mpModel;
        a_this->cullMtx = model != nullptr ? gabi::ea(model) + 0xC8 : 0; /* fopAcM_SetMtx(getBaseTRMtx()) */
        fopAcM_SetMin(a_this, -200.0f * a_this->scale.x, -5000.0f, -200.0f * a_this->scale.z);
        fopAcM_SetMax(a_this, 200.0f * a_this->scale.x, 5000.0f, 200.0f * a_this->scale.z);

        a_this->mStts.Init(0xFF, 0xFF, a_this);

        for (int chainIdx = 0; chainIdx < 4; chainIdx++) {
            a_this->mChainCyls[chainIdx].Set(himo_cyl_src);
            a_this->mChainCyls[chainIdx].SetStts(&a_this->mStts);
        }

        a_this->m844 = 10;
        daMsw_Execute(a_this);
    }

    return phase_state;
}
VERIFY(0x021D6F48, daMsw_Create);

/* 021D7208 */
static void __sinit_d_a_msw_cpp() {
    WWHD_FUNC(0x021D7208, void, (u32)0);
    sinit_header_statics(0x1046582C, 0x101BAD6C);
}
VERIFY(0x021D7208, __sinit_d_a_msw_cpp);

/* 021D729C: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021D729C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021D729C, trivial_dt);

/* 021D72B0: dCcD_Cyl::dCcD_Cyl (this TU's copy; allocates when this == NULL) */
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl* c) {
    WWHD_FUNC(0x021D72B0, dCcD_Cyl*, c);
    if (c == nullptr) {
        c = (dCcD_Cyl*)operator_new(0x130);
        if (c == nullptr)
            return c;
    }
    dCcD_Cyl_ct(c, TU_AAB_VTBL);
    return c;
}
VERIFY(0x021D72B0, dCcD_Cyl_ct_tu);

/* 021D733C: msw_class deleting destructor */
static void msw_class_dt(msw_class* i_this, s32 flags) {
    WWHD_FUNC(0x021D733C, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr(i_this->mChainCyls, 4, 0x130, 0x02515A70 /* dCcD_Cyl::~dCcD_Cyl */, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021D733C, msw_class_dt);

/* 021D73BC: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x021D73BC, void, p);
}
VERIFY(0x021D73BC, empty_virtual);
