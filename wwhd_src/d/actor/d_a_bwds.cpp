/**
 * d_a_bwds.cpp (WWHD)
 * Enemy - Molgera Larva: everything but daBwds_Execute (d_a_bwds_exec.cpp).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bwds.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bwds.h"

/* guest string literals (each use has its own copy) */
#define STR_BWDS_ANM STR(0x1000C2BC)    /* "Bwds" (anm_init) */
#define STR_BWDS_HEAP STR(0x1000C37C)   /* "Bwds" (useHeapInit) */
#define STR_BWD_HEAP STR(0x1000C378)    /* "Bwd" */
#define STR_BWDS_CREATE STR(0x1000C384) /* "Bwds" (daBwds_Create) */
#define STR_BWDS_DELETE STR(0x1000C370) /* "Bwds" (daBwds_Delete) */
#define STR_HIO_NAME STR(0x1000C38C)    /* "風ボス（子）" */
/* static tables (.data) */
static inline u16 body_bdl(int i) { return gabi::load<u16>(0x101B3898 + 2 * i); }
static inline u32 s_bdl(int i) { return gabi::load<u32>(0x101B38B4 + 4 * i); }
static inline u32 s_btk(int i) { return gabi::load<u32>(0x101B38BC + 4 * i); }
static inline u32 s_brk(int i) { return gabi::load<u32>(0x101B38C4 + 4 * i); }
static inline u32 s_bck(int i) { return gabi::load<u32>(0x101B38CC + 4 * i); }
#define cc_sph_src gabi::at<dCcD_SrcSph>(0x101B38D4)
#define body_sph_src gabi::at<dCcD_SrcSph>(0x101B3914)
#define hs_sph_src gabi::at<dCcD_SrcSph>(0x101B3954)

/* 02108DDC */
static daBwds_HIO_c* daBwds_HIO_c_ct(daBwds_HIO_c* hio) {
    WWHD_FUNC(0x02108DDC, daBwds_HIO_c*, hio);
    if (hio == nullptr) {
        hio = (daBwds_HIO_c*)operator_new(0x20);
        if (hio == nullptr)
            return hio;
    }
    hio->mNo = -1;
    hio->m005 = 0;
    hio->m008 = 0.79999995f;
    hio->m00C = 0.7f;
    hio->m010 = 7.0f;
    hio->m014 = 220;
    hio->m018 = 2000.0f;
    hio->m01C = 300.0f;
    hio->__vtbl = BWDS_HIO_VTBL;
    return hio;
}
VERIFY(0x02108DDC, daBwds_HIO_c_ct);

/* 02105DD8 */
static void anm_init(bwds_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x02105DD8, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWDS_ANM, bckFileIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(STR_BWDS_ANM, soundFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWDS_ANM, bckFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x02105DD8, anm_init);

/* 02105F00: body_draw inlined */
static BOOL daBwds_Draw(bwds_class* i_this) {
    WWHD_FUNC(0x02105F00, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &actor->current.pos, &actor->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
    i_this->mpMorf->entryDL();
    for (s32 i = 0; i < 0xd - i_this->m04F0; i++) {
        model = i_this->mp02BC[i];
        setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
        mDoExt_modelUpdateDL(model);
    }
    for (s32 i = 0; i < 2; i++) {
        if (i_this->m1894[i] != 0) {
            mDoExt_McaMorf* morf = i_this->mp18B0[i];
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, morf->getModel(), &actor->tevStr);
            mDoExt_brkAnm* brk = i_this->mp18C0[i];
            mDoExt_brkAnm_entry(brk, J3DModel_getModelData(i_this->mp18B0[i]->getModel()),
                                gabi::load<f32>(gabi::ea(brk) + 4) /* getFrame */);
            mDoExt_btkAnm* btk = i_this->mp18B8[i];
            btk->entry(J3DModel_getModelData(i_this->mp18B0[i]->getModel()), btk->getFrame());
            i_this->mp18B0[i]->entryDL();
        }
    }
    return TRUE;
}
VERIFY(0x02105F00, daBwds_Draw);

/* 02108748 */
static BOOL daBwds_IsDelete(bwds_class*) {
    WWHD_FUNC(0x02108748, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02108748, daBwds_IsDelete);

/* 02108750 */
static BOOL daBwds_Delete(bwds_class* i_this) {
    WWHD_FUNC(0x02108750, BOOL, i_this);
    dComIfG_resDelete(&i_this->m02AC, STR_BWDS_DELETE);
    if (i_this->m18C8 != 0) {
        hio_set = 0;
        mDoHIO_deleteChild(l_HIO.mNo);
    }
    for (s32 i = 0; i < 3; i++) {
        bw_vremove(&i_this->m1820[i]); /* remove() */
    }
    bw_vremove(&i_this->m1880);
    return TRUE;
}
VERIFY(0x02108750, daBwds_Delete);

/* 021087FC */
static BOOL useHeapInit(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x021087FC, BOOL, i_actor);
    bwds_class* i_this = (bwds_class*)i_actor;

    J3DModelData* headData = (J3DModelData*)dComIfG_getObjectRes(STR_BWDS_HEAP, dRes_INDEX_BWDS_BDL_KOBOSS_HEAD_e, SAFESTRING_VTBL);
    J3DAnmTransform* headAnm = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWDS_HEAP, dRes_INDEX_BWDS_BCK_KOBOSS_CLOSE_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, headData, nullptr, nullptr, headAnm, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, 1,
                                            nullptr, 0, 0x11020203);
    mDoExt_McaMorf* morf = i_this->mpMorf;
    if (!morf || !morf->getModel()) {
        return FALSE;
    }

    for (s32 i = 0; i < 0xd; i++) {
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR_BWDS_HEAP, body_bdl(i), SAFESTRING_VTBL);
        J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->mp02BC[i] = model;
        if (model == nullptr) {
            return FALSE;
        }
    }

    for (s32 i = 0; i < 2; i++) {
        J3DModelData* bdl = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, s_bdl(i), SAFESTRING_VTBL);
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWD_HEAP, s_bck(i), SAFESTRING_VTBL);
        mDoExt_McaMorf* m = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                                   nullptr, 0, 0x11020203);
        i_this->mp18B0[i] = m;
        if (!m || !m->getModel()) {
            return FALSE;
        }
        J3DModelData* modelData = J3DModel_getModelData(m->getModel());

        void* btk = operator_new(0x74);
        if (btk != nullptr)
            btk = mDoExt_btkAnm_ct(btk);
        i_this->mp18B8[i] = (mDoExt_btkAnm*)btk;
        if (!btk) {
            return FALSE;
        }
        void* btkRes = dComIfG_getObjectRes(STR_BWD_HEAP, s_btk(i), SAFESTRING_VTBL);
        if (!i_this->mp18B8[i]->init(modelData, (J3DAnmTextureSRTKey*)btkRes, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }

        void* brk = operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mp18C0[i] = (mDoExt_brkAnm*)brk; /* no NULL check */
        void* brkRes = dComIfG_getObjectRes(STR_BWD_HEAP, s_brk(i), SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mp18C0[i], modelData, brkRes, 1, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    return TRUE;
}
VERIFY(0x021087FC, useHeapInit);

/* 02108B24: element constructor for m1820[] (dPa_smokeEcallBack(1)) */
static void* smoke_ct_1(void* p) {
    WWHD_FUNC(0x02108B24, void*, p);
    return gabi::call<void*>(0x025A5B18, p, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
}
VERIFY(0x02108B24, smoke_ct_1);

/* 02108B2C: bwds_class::bwds_class (HD: out of line; allocates when this == NULL) */
static bwds_class* bwds_class_ct(bwds_class* i_this) {
    WWHD_FUNC(0x02108B2C, bwds_class*, i_this);
    if (i_this == nullptr) {
        i_this = (bwds_class*)operator_new(0x19E4);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BWDS_VTBL;
    dCcD_Stts_ct(&i_this->m0508);
    dCcD_Sph_ct(&i_this->m0544);
    __construct_array(i_this->m0670, 0xF, 0x12C, 0x025166F0);
    __construct_array(i_this->m1820, 3, 0x20, 0x02108B24);
    dPa_followEcallBack_ct(&i_this->m1880, 0, 0);
    return i_this;
}
VERIFY(0x02108B2C, bwds_class_ct);

/* 02108BF0 */
static cPhs_State daBwds_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02108BF0, cPhs_State, a_this);
    bwds_class* i_this = (bwds_class*)a_this;
    /* fopAcM_ct(&i_this->actor, bwds_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            bwds_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->m02AC, STR_BWDS_CREATE);
    if (res == cPhs_COMPLEATE_e) {
        i_this->m02B4 = (u8)fopAcM_GetParam(a_this);
        if (!fopAcM_entrySolidHeap(a_this, 0x021087FC /* useHeapInit */, 0x4B000)) {
            return cPhs_ERROR_e;
        }
        if (hio_set == 0) {
            i_this->m18C8 = 1;
            hio_set = 1;
            l_HIO.mNo = mDoHIO_createChild(STR_HIO_NAME, &l_HIO);
        }
        attn_flags(a_this) = 4; /* fopAc_Attn_LOCKON_BATTLE_e */
        gabi::store<u8>(gabi::ea(a_this) + 0x38A, 4); /* attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] */
        a_this->health = 4;
        a_this->max_health = 4;
        i_this->m0508.Init(0xFA, 0, a_this);
        i_this->m0544.Set(hs_sph_src);
        i_this->m0544.SetStts(&i_this->m0508);

        for (s32 i = 0; i < 0xf; i++) {
            i_this->m0670[i].SetStts(&i_this->m0508);
            if (i == 0) {
                i_this->m0670[i].Set(cc_sph_src);
            } else {
                i_this->m0670[i].Set(body_sph_src);
            }
            i_this->m032C[i].copy(a_this->current.pos);
        }
        if (i_this->m02B4 == 0x23) {
            i_this->m02F8 = -10;
        }

        gabi::store<u8>(eff_col + 0, 0xA0);
        gabi::store<u8>(eff_col + 1, 0xA0);
        gabi::store<u8>(eff_col + 2, 0x80);
        gabi::store<u8>(eff_col + 3, 0x96);
    }
    return res;
}
VERIFY(0x02108BF0, daBwds_Create);

/* 02108F10: fopAcM_seStart (this TU's out-of-line copy) */
static void fopAcM_seStart_bw(fopAc_ac_c* a, u32 id, u32 param) {
    WWHD_FUNC(0x02108F10, void, a, id, param);
    fopAcM_seStart(a, id, param);
}
VERIFY(0x02108F10, fopAcM_seStart_bw);

/* 02108F7C: sead::SafeString deleting destructor (trivial; this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02108F7C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02108F7C, SafeString_dt);

/* 02108F90: element destructor for m1820[] (dPa_smokeEcallBack: trivial) */
static void smoke_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02108F90, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02108F90, smoke_dt);

/* 02109050: sead::SafeString::assureTerminationImpl_ (this TU's copy: empty) */
static void SafeString_assure(void* p) {
    WWHD_FUNC(0x02109050, void, p);
}
VERIFY(0x02109050, SafeString_assure);

/* 02108FA4: bwds_class deleting destructor (compiler-generated) */
static void bwds_class_dt(bwds_class* i_this, s32 flags) {
    WWHD_FUNC(0x02108FA4, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr(i_this->m1820, 3, 0x20, 0x02108F90, 0, 0);
        __destroy_arr(i_this->m0670, 0xF, 0x12C, 0x02515AE8, 0, 0);
        dCcD_Sph_dt(&i_this->m0544, 2);
        dCcD_Stts_dt(&i_this->m0508, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02108FA4, bwds_class_dt);

/* 02108E70: header statics, then l_HIO's constructor */
static void __sinit_d_a_bwds_cpp() {
    WWHD_FUNC(0x02108E70, void, (u32)0);
    sinit_header_statics_z(0x10462C38, 0x101B3994, 0x10462C64);
    daBwds_HIO_c_ct(&l_HIO);
}
VERIFY(0x02108E70, __sinit_d_a_bwds_cpp);
