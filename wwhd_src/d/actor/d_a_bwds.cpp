/**
 * d_a_bwds.cpp (WWHD)
 * Enemy - Molgera Larva: everything but daBwds_Execute (d_a_bwds_exec.cpp).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bwds.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#define BWDS_STORAGE_OWNER
#include "d/actor/d_a_bwds.h"
#include "gc_api/f_op/f_op_actor_mng.h"

// HD: translation-unit vtables and header constructor storage.
const gc_tu_data gc_tu = {
    .safeString = 0x1000C22C,
    .headerStatics = {0x10462C38, 0x101B3994, 0x10462C64},
};

// One owner for the split translation unit's mutable globals.
static daBwds_HIO_c l_HIO;
static u8 hio_set = 0;
static GXColor eff_col;

// Resource indices and collision definitions match the GameCube table layout.
// body_bdl: KOBOSS_BODY (8), then KOBOSS_SHIPPO (10).
// The paired Bwd tables select GSP00 and GSP01 for BDL, BTK, BRK and BCK.
// HD: numeric table leaves let autoglue check every initializer against the image.
static const u16 body_bdl[13] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 10};
static const u32 s_bdl[2] = {33, 34};
static const u32 s_btk[2] = {48, 49};
static const u32 s_brk[2] = {43, 44};
static const u32 s_bck[2] = {24, 25};
struct BwdsSphereSource {
    be<u32> mFlags, mAttackType, mAttackPower, mAttackPrm;
    be<u32> mTargetType, mTargetPrm, mCollisionPrm;
    be<u32> mAttackInfo, mAttackFlags, mTargetInfo, mTargetFlags, mCollisionFlags;
    be<f32> mCenter[3];
    be<f32> mRadius;
};
WWHD_SIZE(BwdsSphereSource, 0x40);
static const BwdsSphereSource cc_sph_src = {0x00000000, 0x00000800, 0x02000000, 0x00000005, 0xFF1D7EFF, 0x00000003, 0x00000075, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000000, {0.0f, 0.0f, 0.0f}, 15.0f};
static const BwdsSphereSource body_sph_src = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF1D7EFF, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x050C0000, 0x00000003, 0x00000000, {0.0f, 0.0f, 0.0f}, 15.0f};
static const BwdsSphereSource hs_sph_src = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00008000, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000000, {0.0f, 0.0f, 0.0f}, 15.0f};

/* 02108DDC */
static daBwds_HIO_c* daBwds_HIO_c_ct(daBwds_HIO_c* hio) {
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

/* 02105DD8 */
static void anm_init(bwds_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Bwds", bckFileIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes("Bwds", soundFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Bwds", bckFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 02105F00 */
// HD: body_draw inlined
static BOOL daBwds_Draw(bwds_class* i_this) {
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
                                brk->getFrame());
            mDoExt_btkAnm* btk = i_this->mp18B8[i];
            btk->entry(J3DModel_getModelData(i_this->mp18B0[i]->getModel()), btk->getFrame());
            i_this->mp18B0[i]->entryDL();
        }
    }
    return TRUE;
}

/* 02108748 */
static BOOL daBwds_IsDelete(bwds_class*) {
    return TRUE;
}

/* 02108750 */
static BOOL daBwds_Delete(bwds_class* i_this) {
    dComIfG_resDelete(&i_this->m02AC, "Bwds");
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

/* 021087FC */
static BOOL useHeapInit(fopAc_ac_c* i_actor) {
    bwds_class* i_this = (bwds_class*)i_actor;

    J3DModelData* headData = (J3DModelData*)dComIfG_getObjectRes("Bwds", dRes_INDEX_BWDS_BDL_KOBOSS_HEAD_e, SAFESTRING_VTBL);
    J3DAnmTransform* headAnm = (J3DAnmTransform*)dComIfG_getObjectRes("Bwds", dRes_INDEX_BWDS_BCK_KOBOSS_CLOSE_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, headData, nullptr, nullptr, headAnm, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, 1,
                                            nullptr, 0, 0x11020203);
    mDoExt_McaMorf* morf = i_this->mpMorf;
    if (!morf || !morf->getModel()) {
        return FALSE;
    }

    for (s32 i = 0; i < 0xd; i++) {
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Bwds", body_bdl[i], SAFESTRING_VTBL);
        J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->mp02BC[i] = model;
        if (model == nullptr) {
            return FALSE;
        }
    }

    for (s32 i = 0; i < 2; i++) {
        J3DModelData* bdl = (J3DModelData*)dComIfG_getObjectRes("Bwd", s_bdl[i], SAFESTRING_VTBL);
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("Bwd", s_bck[i], SAFESTRING_VTBL);
        mDoExt_McaMorf* m = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                                   nullptr, 0, 0x11020203);
        i_this->mp18B0[i] = m;
        if (!m || !m->getModel()) {
            return FALSE;
        }
        J3DModelData* modelData = J3DModel_getModelData(m->getModel());

        void* btk = operator_new(0x74);
        if (btk != nullptr)
            btk = bw_btkCtor(btk);
        i_this->mp18B8[i] = (mDoExt_btkAnm*)btk;
        if (!btk) {
            return FALSE;
        }
        void* btkRes = dComIfG_getObjectRes("Bwd", s_btk[i], SAFESTRING_VTBL);
        if (!i_this->mp18B8[i]->init(modelData, (J3DAnmTextureSRTKey*)btkRes, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }

        void* brk = operator_new(0x78);
        if (brk != nullptr)
            brk = bw_brkCtor(brk);
        i_this->mp18C0[i] = (mDoExt_brkAnm*)brk; /* no NULL check */
        void* brkRes = dComIfG_getObjectRes("Bwd", s_brk[i], SAFESTRING_VTBL);
        if (!i_this->mp18C0[i]->init(modelData, (J3DAnmTevRegKey*)brkRes, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    return TRUE;
}

/* 02108B24 */
// HD: element constructor for m1820[] (dPa_smokeEcallBack(1))
static void* smoke_ct_1(void* p) {
    return bw_smokeCtor(p, 1);
}

/* 02108B2C */
// HD: bwds_class::bwds_class (HD: out of line; allocates when this == NULL)
static bwds_class* bwds_class_ct(bwds_class* i_this) {
    if (i_this == nullptr) {
        i_this = (bwds_class*)operator_new(0x19E4);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BWDS_VTBL;
    dCcD_Stts_ct(&i_this->m0508);
    bw_sphereCtor(&i_this->m0544);
    bw_constructArray(i_this->m0670, 0xF, 0x12C, BWDS_SPHERE_CTOR);
    bw_constructArray(i_this->m1820, 3, 0x20, gabi::guest_fn(reinterpret_cast<const void*>(smoke_ct_1)));
    dPa_followEcallBack_ct(&i_this->m1880, 0, 0);
    return i_this;
}

/* 02108BF0 */
static cPhs_State daBwds_Create(fopAc_ac_c* a_this) {
    bwds_class* i_this = (bwds_class*)a_this;
    /* fopAcM_ct(&i_this->actor, bwds_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            bwds_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->m02AC, "Bwds");
    if (res == cPhs_COMPLEATE_e) {
        i_this->m02B4 = (u8)fopAcM_GetParam(a_this);
        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), 0x4B000)) {
            return cPhs_ERROR_e;
        }
        if (hio_set == 0) {
            i_this->m18C8 = 1;
            hio_set = 1;
            // HD: glue maps this readable label to the original Shift-JIS "風ボス（子）" bytes.
            l_HIO.mNo = mDoHIO_createChild("Wind Boss (Child)", &l_HIO);
        }
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        a_this->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 4;
        a_this->health = 4;
        a_this->max_health = 4;
        i_this->m0508.Init(0xFA, 0, a_this);
        i_this->m0544.Set(reinterpret_cast<const dCcD_SrcSph*>(&hs_sph_src));
        i_this->m0544.SetStts(&i_this->m0508);

        for (s32 i = 0; i < 0xf; i++) {
            i_this->m0670[i].SetStts(&i_this->m0508);
            if (i == 0) {
                i_this->m0670[i].Set(reinterpret_cast<const dCcD_SrcSph*>(&cc_sph_src));
            } else {
                i_this->m0670[i].Set(reinterpret_cast<const dCcD_SrcSph*>(&body_sph_src));
            }
            i_this->m032C[i].copy(a_this->current.pos);
        }
        if (i_this->m02B4 == 0x23) {
            i_this->m02F8 = -10;
        }

        eff_col.r = 0xA0;
        eff_col.g = 0xA0;
        eff_col.b = 0x80;
        eff_col.a = 0x96;
    }
    return res;
}

/* 02108F10 */
// HD: fopAcM_seStart (this TU's out-of-line copy)
static void fopAcM_seStart_bw(fopAc_ac_c* a, u32 id, u32 param) {
    fopAcM_seStart(a, id, param);
}

/* 02108F7C */
// HD: sead::SafeString deleting destructor (trivial; this TU's copy)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02108F90 */
// HD: element destructor for m1820[] (dPa_smokeEcallBack: trivial)
static void smoke_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02109050 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy: empty)
static void SafeString_assure(void* p) {
}

/* 02108FA4 */
// HD: bwds_class deleting destructor (compiler-generated)
static void bwds_class_dt(bwds_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        bw_destroyArray(i_this->m1820, 3, 0x20, gabi::guest_fn(reinterpret_cast<const void*>(smoke_dt)), 0, 0);
        bw_destroyArray(i_this->m0670, 0xF, 0x12C, BWDS_SPHERE_DTOR, 0, 0);
        bw_sphereDtor(&i_this->m0544, 2);
        dCcD_Stts_dt(&i_this->m0508, 2);
        bw_actorDtor(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02108E70 */
// HD: header statics, then l_HIO's constructor
static void __sinit_d_a_bwds_cpp() {
    __sinit_header_statics();
    daBwds_HIO_c_ct(&l_HIO);
}

/* WWHD glue (tools/verify/autoglue.py): bindings and TU storage
ext 02518CC8 void bw_def_se_set(fopAc_ac_c* actor, void* object, u32 material)
ext 025192A8 void bw_cc_at_check(fopAc_ac_c* actor, CcAtInfo_bw* info)
ext 02008DAC void bw_checkDtor(void* check, s32 flags)
ext 025E80D0 void* bw_brkCtor(void* animation)
ext 025E7C6C void* bw_btkCtor(void* animation)
ext 028EFFD0 void bw_constructArray(void* array, u32 count, u32 stride, u32 constructor)
ext 028F0164 void bw_destroyArray(void* array, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved)
ext 025166F0 void bw_sphereCtor(void* sphere)
ext 02515AE8 void bw_sphereDtor(void* sphere, s32 flags)
ext 025A5B18 void* bw_smokeCtor(void* callback, u8 mode)
ext 025D50BC void bw_actorDtor(fopAc_ac_c* actor, s32 flags)
data body_bdl 101B3898
data s_bdl 101B38B4
data s_btk 101B38BC
data s_brk 101B38C4
data s_bck 101B38CC
data cc_sph_src 101B38D4
data body_sph_src 101B3914
data hs_sph_src 101B3954
data l_HIO 10462C44 nocheck
data hio_set 101B3874
data eff_col 10462C34 nocheck
str 1000C2BC "Bwds" in 02105DD8
str 1000C370 "Bwds" in 02108750
str 1000C37C "Bwds" in 021087FC
str 1000C378 "Bwd" in 021087FC
str 1000C384 "Bwds" in 02108BF0
str 1000C38C "Wind Boss (Child)" renamed in 02108BF0
*/
