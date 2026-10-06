/**
 * d_a_bita.cpp (WWHD)
 * Object - Wooden platforms (Gohma fight)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bita.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * mode_normal and mode_dead are inlined into daBita_Execute.
 */
#include "bindings.h"

#define M_arcname STR(0x1000940C)  /* "Bita" (getRes) */
#define SAFESTRING_VTBL 0x100093AC /* this TU's sead::SafeString vtable */
#define BITA_VTBL 0x100093D4       /* bita_class vtable (HD virtual destructor) */
#define CYL_AAB_VTBL 0x100093C4    /* cM3dGAab vtable (per TU) */
#define FILE_NAME STR(0x10009414)

enum { fpcNm_BTD_e = 0xEA };
enum { AT_TYPE_FIRE = 0x200 };
enum { JA_SE_OBJ_BTD_BOARD_BURN = 0x6940, JA_SE_OBJ_BTD_BOARD_BRK = 0x6941 };
enum { dPa_name_ID_AK_SN_BTDBURNEDBOARD00 = 0x80E3, dPa_name_ID_AK_SN_BTDBURNEDBOARD01 = 0x80E4 };

/* file statics */
static gptr<fopAc_ac_c>& btd() { return *gabi::at<gptr<fopAc_ac_c>>(0x101913AC); }
#define ita_bmd(i) gabi::load<u32>(0x1019138C + 4 * (i))
#define ita_dzb(i) gabi::load<u32>(0x10191394 + 4 * (i))
#define ita_Ef_bmd(i) gabi::load<u32>(0x1019139C + 4 * (i))
#define ita_Ef(i) gabi::load<u32>(0x101913A4 + 4 * (i))
#define body_cyl_src gabi::at<dCcD_SrcCyl>(0x101913D0)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGs_getLife(): u16 at *(0x101F84DC) + 0x22 (as in d_a_ki_execute.cpp) */
static inline u16 dComIfGs_getLife() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x22); }
/* dComIfGp_getCamera(0): camera_class* at play+0x5AF8; view.mLookat.mEye at +0xDC */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
/* HD fopAcM_seStartCurrent inline: checks the actor and &current.pos for NULL */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* HD: fopAcM_SetMin / fopAcM_SetMax are out of line (025D672C / 025D673C) */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (matcher: "init"), 025E8154 mDoExt_brkAnm::init (HD signature) */
static inline mDoExt_brkAnm* mDoExt_brkAnm_ct(void* p) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(mDoExt_brkAnm* a, J3DModelData* d, J3DAnmTevRegKey* k, bool play, s32 mode, f32 speed,
                                      s16 start, s16 end, bool b, s32 i) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, b, i);
}
/* HD: mDoExt_brkAnm::remove inline: clears the model data's tev-register animation (+0x48) */
static inline void brk_remove(J3DModelData* d) { gabi::store<u32>(gabi::ea(d) + 0x48, 0); }
static inline f32 anm_frame(void* anm) { return gabi::load<f32>(gabi::ea(anm) + 4); }
/* dBgW::SetCrrFunc: +0xA8; dBgS_MoveBGProc_Typical 024EE658 */
static inline void dBgW_SetCrrFunc(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xA8, fn); }

struct bita_class : fopAc_ac_c {
    enum Mode { MODE_NORMAL = 0, MODE_DEAD = 1 };
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<J3DModel> mpModelEf;
    /* 0x3BC */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x3C0 */ be<u8> mType;
    /* 0x3C1 */ be<u8> mPrmScale;
    /* 0x3C2 */ be<u8> mTimer;
    /* 0x3C3 */ be<u8> field_0x2a7;
    /* 0x3C4 */ be<u8> mMode;
    /* 0x3C5 */ be<u8> mSub;
    /* 0x3C6 */ u8 _3C6[2];
    /* 0x3C8 */ dCcD_Stts mStts;
    /* 0x404 */ dCcD_Cyl mCyl;
    /* 0x534 */ Mtx34 mMtx;
    /* 0x564 */ gptr<dBgW> mpBgW;
};
WWHD_OFFSET(bita_class, mStts, 0x3C8);
WWHD_OFFSET(bita_class, mCyl, 0x404);
WWHD_OFFSET(bita_class, mMtx, 0x534);
WWHD_SIZE(bita_class, 0x568);

/* 02098044 */
static void* b_a_sub(void* search, void* user) {
    WWHD_FUNC(0x02098044, void*, search, user);
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_BTD_e)
        return search;
    return nullptr;
}
VERIFY(0x02098044, b_a_sub);

/* 02098094 */
static BOOL daBita_Draw(bita_class* i_this) {
    WWHD_FUNC(0x02098094, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    if (i_this->mMode == bita_class::MODE_DEAD && i_this->mSub == 1) {
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModelEf, &i_this->tevStr);
        J3DModel* ef = i_this->mpModelEf;
        mDoExt_brkAnm* brk = i_this->mpBrkAnm;
        mDoExt_brkAnm_entry(brk, J3DModel_getModelData(ef), anm_frame(brk));
        mDoExt_modelUpdateDL(i_this->mpModelEf);
        brk_remove(J3DModel_getModelData(i_this->mpModelEf));
    } else {
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->mpModel);
    }
    return TRUE;
}
VERIFY(0x02098094, daBita_Draw);

/* 02098150 */
static void base_mtx_set(bita_class* i_this) {
    WWHD_FUNC(0x02098150, void, i_this);
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    cMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    if (i_this->mMode == bita_class::MODE_DEAD && i_this->mSub >= 1) {
        J3DModel_setBaseTRMtx(i_this->mpModelEf, calc_mtx());
        if (i_this->mTimer < 60)
            mDoExt_baseAnm_play(i_this->mpBrkAnm);
        i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModelEf)); /* fopAcM_SetMtx */
    } else {
        J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
    }
}
VERIFY(0x02098150, base_mtx_set);

/* inlined into daBita_Execute */
static inline void mode_normal(bita_class* i_this) {
    fopAc_ac_c* b = btd();
    if (b != nullptr) {
        if (gabi::load<s16>(gabi::ea(b) + 0x705A) >= 100) { /* btd->m6E16 (HD +0x705A) */
            gabi::Local<cXyz> delta;
            u32 cam = dComIfGp_getCamera0();
            cXyz_mi(gabi::at<cXyz>(cam + 0xDC), delta, &i_this->current.pos);
            if (std_sqrtf(PSVECSquareMag(delta)) < 1500.0f) { /* delta.abs() */
                fopAcM_delete(i_this);
            }
        } else {
            if (i_this->mCyl.ChkTgHit() && gabi::load<u32>(gabi::ea(i_this->mCyl.GetTgHitObj()) + 0x10) == AT_TYPE_FIRE) {
                i_this->mMode = bita_class::MODE_DEAD;
                i_this->mSub = 0;
            }
        }
    }
}

static inline void mode_dead(bita_class* i_this) {
    switch (i_this->mSub) {
    case 0:
        i_this->mTimer = 0;
        i_this->field_0x2a7 = (u8)gabi::ftoi(cM_rndF(4.0f));
        i_this->mSub = 1;
        break;
    case 1:
        i_this->mTimer = (u8)(i_this->mTimer + 1);
        if (i_this->mTimer == 20) {
            fopAcM_seStartCurrent(i_this, JA_SE_OBJ_BTD_BOARD_BURN, 0);
        }
        if (i_this->mTimer == 88) {
            fopAcM_seStartCurrent(i_this, JA_SE_OBJ_BTD_BOARD_BRK, 0);

            s32 type = i_this->mType;
            if (type > 1)
                type = 1;

            if (type == 0) {
                dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDBURNEDBOARD00, &i_this->current.pos, &i_this->shape_angle);
            } else {
                dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDBURNEDBOARD01, &i_this->current.pos, &i_this->shape_angle);
            }
        }

        if (i_this->mTimer >= 90)
            fopAcM_delete(i_this);
        break;
    }
}

/* 020982D8 */
static BOOL daBita_Execute(bita_class* i_this) {
    WWHD_FUNC(0x020982D8, BOOL, i_this);
    if (btd() == nullptr)
        btd() = (fopAc_ac_c*)fpcM_Search(0x02098044 /* b_a_sub */, i_this);

    if (dComIfGs_getLife() == 0)
        i_this->actor_status &= ~0x4000u; /* fopAcM_OffStatus(fopAcStts_UNK4000_e) */

    switch (i_this->mMode) {
    case bita_class::MODE_NORMAL:
        mode_normal(i_this);
        break;
    case bita_class::MODE_DEAD:
        mode_dead(i_this);
        break;
    }

    base_mtx_set(i_this);
    PSMTXCopy(calc_mtx(), &i_this->mMtx);
    dBgW_Move(i_this->mpBgW);

    if (btd() != nullptr && i_this->mMode == bita_class::MODE_NORMAL) {
        cMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
        cMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
        gabi::Local<cXyz> offs;
        offs->set(0.0f, -80.0f, 160.0f);
        gabi::Local<cXyz> pos;
        MtxPosition(offs, pos);
        PSVECAdd(pos, &i_this->current.pos, pos);
        i_this->mCyl.SetC(pos);
        i_this->mCyl.SetH(60.0f);
        i_this->mCyl.SetR(400.0f * i_this->scale.x);
        dComIfG_Ccsp_Set(&i_this->mCyl);
    }

    return TRUE;
}
VERIFY(0x020982D8, daBita_Execute);

/* 02098720 */
static BOOL daBita_IsDelete(bita_class*) {
    WWHD_FUNC(0x02098720, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02098720, daBita_IsDelete);

/* 02098728 */
static BOOL daBita_Delete(bita_class* i_this) {
    WWHD_FUNC(0x02098728, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x10009400)); /* dComIfG_resDeleteDemo */
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }
    return TRUE;
}
VERIFY(0x02098728, daBita_Delete);

/* 02098780 */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x02098780, BOOL, i_ac);
    bita_class* i_this = (bita_class*)i_ac;

    s32 type = i_this->mType;
    if (type > 1)
        type = 1;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, ita_bmd(type), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(571, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x23B, STR(0x10009424));
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (model == nullptr)
        return FALSE;
    i_this->mpModel = model;

    modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, ita_Ef_bmd(type), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(583, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x247, STR(0x10009424));
    model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (model == nullptr)
        return FALSE;
    i_this->mpModelEf = model;

    /* new mDoExt_brkAnm() */
    void* p = operator_new(0x78);
    if (p != nullptr)
        p = mDoExt_brkAnm_ct(p);
    i_this->mpBrkAnm = (mDoExt_brkAnm*)p;
    if (p == nullptr)
        return FALSE;
    J3DModel* ef = i_this->mpModelEf;
    J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(M_arcname, ita_Ef(type), SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init(i_this->mpBrkAnm, J3DModel_getModelData(ef), brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
        return FALSE;

    i_this->mpBgW = new_dBgW();
    if (i_this->mpBgW == nullptr)
        return FALSE;
    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, ita_dzb(type), SAFESTRING_VTBL);
    if (cBgW_Set(i_this->mpBgW, dzb, cBgW_MOVE_BG_e, &i_this->mMtx) == true)
        return FALSE;

    dBgW_SetCrrFunc(i_this->mpBgW, 0x024EE658 /* dBgS_MoveBGProc_Typical */);
    return TRUE;
}
VERIFY(0x02098780, useHeapInit);

/* 02098970 */
static cPhs_State daBita_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x02098970, cPhs_State, i_ac);
    bita_class* i_this = (bita_class*)i_ac;
    dComIfGp_get(); /* HD: result unused */

    /* fopAcM_ct(i_ac, bita_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_this->__vtbl = BITA_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            dCcD_Cyl_ct(&i_this->mCyl, CYL_AAB_VTBL);
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }

    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, STR(0x10009454));
    if (rt == cPhs_COMPLEATE_e) {
        btd() = nullptr;
        u32 prm = fopAcM_GetParam(i_this);
        i_this->mType = (u8)(prm >> 0);
        i_this->mPrmScale = (u8)(fopAcM_GetParam(i_this) >> 8);
        if ((u8)prm == 0xFF)
            i_this->mType = 0;

        if (!fopAcM_entrySolidHeap(i_this, 0x02098780 /* useHeapInit */, 0x30C0))
            return cPhs_ERROR_e;

        if (dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW, i_this))
            return cPhs_ERROR_e;

        switch (i_this->mPrmScale) {
        case 1: i_this->scale.set(1.5f, 1.5f, 1.5f); break;
        case 2: i_this->scale.set(2.0f, 2.0f, 2.0f); break;
        case 3: i_this->scale.set(3.0f, 3.0f, 3.0f); break;
        default: i_this->scale.y = i_this->scale.z = i_this->scale.x = 1.0f; break;
        }

        i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
        fopAcM_SetMin(i_this, -1000.0f * i_this->scale.x, -500.0f, -1000.0f * i_this->scale.z);
        fopAcM_SetMax(i_this, 1000.0f * i_this->scale.x, 500.0f, 1000.0f * i_this->scale.z);
        J3DModel_setBaseScale(i_this->mpModel, &i_this->scale);
        J3DModel_setBaseScale(i_this->mpModelEf, &i_this->scale);
        i_this->shape_angle.x = i_this->current.angle.x;
        i_this->shape_angle.y = i_this->current.angle.y;
        i_this->shape_angle.z = i_this->current.angle.z;
        base_mtx_set(i_this);
        dBgW_Move(i_this->mpBgW);
        i_this->mCyl.Set(body_cyl_src);
        i_this->mCyl.SetStts(&i_this->mStts);
    }

    return rt;
}
VERIFY(0x02098970, daBita_Create);

/* 02098C8C */
static void __sinit_d_a_bita_cpp() {
    WWHD_FUNC(0x02098C8C, void, (u32)0);
    sinit_header_statics(0x10462300, 0x10191414);
}
VERIFY(0x02098C8C, __sinit_d_a_bita_cpp);

/* 02098D20: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02098D20, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02098D20, trivial_dt);

/* 02098D34: bita_class deleting destructor (inline member destructors) */
static void bita_class_dt(bita_class* i_this, s32 flags) {
    WWHD_FUNC(0x02098D34, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02098D34, bita_class_dt);

/* 02098DA0: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02098DA0, void, p);
}
VERIFY(0x02098DA0, empty_virtual);
