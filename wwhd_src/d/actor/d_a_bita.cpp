/**
 * d_a_bita.cpp (WWHD)
 * Object - Wooden platforms (Gohma fight)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bita.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * mode_normal and mode_dead are inlined into daBita_Execute.
 */
#include "d/actor/d_a_bita.h"
const gc_tu_data gc_tu = {.safeString = 0x100093AC, .headerStatics = {0x10462300, 0x10191414, 0x1046230C}};
// HD: per-unit vtables for the native actor and collision constructors.
enum { BITA_VTBL = 0x100093D4, CYL_AAB_VTBL = 0x100093C4 };
enum { JA_SE_OBJ_BTD_BOARD_BURN = 0x6940, JA_SE_OBJ_BTD_BOARD_BRK = 0x6941 };
enum { dPa_name_ID_AK_SN_BTDBURNEDBOARD00 = 0x80E3, dPa_name_ID_AK_SN_BTDBURNEDBOARD01 = 0x80E4 };
enum { fpcNm_BTD_e = 0xEA, AT_TYPE_FIRE = 0x200 };
using BitaBossPtr = gptr<BitaBossHD>;
static BitaBossPtr btd;
// Bita resource indices: MBIT1/2, their collision meshes, and burning-board animations.
static const s32 ita_bmd[] = {7, 8};
static const s32 ita_dzb[] = {15, 16};
static const s32 ita_Ef_bmd[] = {5, 6};
static const s32 ita_Ef[] = {11, 12};
static dCcD_SrcCyl body_cyl_src;
// HD: the inline sound helper checks both the actor and its position address.
static inline void fopAcM_seStartCurrent(fopAc_ac_c *a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}

/* 02098044 */
static void *b_a_sub(void *search, void *user) {
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_BTD_e)
        return search;
    return nullptr;
}

/* 02098094 */
static BOOL daBita_Draw(bita_class *i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    if (i_this->mMode == bita_class::MODE_DEAD && i_this->mSub == 1) {
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModelEf, &i_this->tevStr);
        J3DModel *ef = i_this->mpModelEf;
        BitaBrkAnm *brk = i_this->mpBrkAnm;
        brk->entry(ef->getModelData(), brk->getFrame());
        mDoExt_modelUpdateDL(i_this->mpModelEf);
        ((BitaModelDataHD *)i_this->mpModelEf->getModelData())->mpTevRegAnimation = nullptr;
    } else {
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->mpModel);
    }
    return TRUE;
}

/* 02098150 */
static void base_mtx_set(bita_class *i_this) {
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    cMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    if (i_this->mMode == bita_class::MODE_DEAD && i_this->mSub >= 1) {
        i_this->mpModelEf->setBaseTRMtx(calc_mtx());
        if (i_this->mTimer < 60)
            i_this->mpBrkAnm->play();
        i_this->cullMtx = gabi::ea(i_this->mpModelEf->getBaseTRMtx()); /* fopAcM_SetMtx */
    } else {
        i_this->mpModel->setBaseTRMtx(calc_mtx());
    }
}

/* inlined into daBita_Execute */
static inline void mode_normal(bita_class *i_this) {
    BitaBossHD *b = btd;
    if (b != nullptr) {
        if (b->m6E16 >= 100) { /* btd->m6E16 (HD +0x705A) */
            gabi::Local<cXyz> delta;
            BitaCameraHD *cam = ((BitaPlayHD *)dComIfGp_get())->mCamera;
            cXyz_mi(&cam->mEye, delta, &i_this->current.pos);
            if (std_sqrtf(PSVECSquareMag(delta)) < 1500.0f) { /* delta.abs() */
                fopAcM_delete(i_this);
            }
        } else {
            if (i_this->mCyl.ChkTgHit() &&
                ((dCcD_GObjInf *)i_this->mCyl.GetTgHitObj())->mAtType == AT_TYPE_FIRE) {
                i_this->mMode = bita_class::MODE_DEAD;
                i_this->mSub = 0;
            }
        }
    }
}

static inline void mode_dead(bita_class *i_this) {
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
                dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDBURNEDBOARD00, &i_this->current.pos,
                                      &i_this->shape_angle);
            } else {
                dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDBURNEDBOARD01, &i_this->current.pos,
                                      &i_this->shape_angle);
            }
        }

        if (i_this->mTimer >= 90)
            fopAcM_delete(i_this);
        break;
    }
}

/* 020982D8 */
static BOOL daBita_Execute(bita_class *i_this) {
    if (btd == nullptr)
        btd = (BitaBossHD *)fpcM_Search(gabi::guest_fn((const void *)b_a_sub), i_this);

    if (dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mLife == 0)
        i_this->actor_status &= ~(u32)fopAcStts_UNK4000_e; /* fopAcM_OffStatus(fopAcStts_UNK4000_e) */

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
    i_this->mpBgW->Move();

    if (btd != nullptr && i_this->mMode == bita_class::MODE_NORMAL) {
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

/* 02098720 */
static BOOL daBita_IsDelete(bita_class *) {
    return TRUE;
}

/* 02098728 */
static BOOL daBita_Delete(bita_class *i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Bita"); /* dComIfG_resDeleteDemo */
    if (i_this->heap != nullptr) {
        dComIfG_Bgsp()->Release(i_this->mpBgW);
    }
    return TRUE;
}

/* 02098780 */
static BOOL useHeapInit(fopAc_ac_c *i_ac) {
    bita_class *i_this = (bita_class *)i_ac;

    s32 type = i_this->mType;
    if (type > 1)
        type = 1;

    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes("Bita", ita_bmd[type]);
    if (modelData == nullptr) /* JUT_ASSERT(571, modelData != NULL) */
        JUT_ASSERT_fail("d_a_bita.cpp", 0x23B, "modelData != (0)");
    J3DModel *model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (model == nullptr)
        return FALSE;
    i_this->mpModel = model;

    modelData = (J3DModelData *)dComIfG_getObjectRes("Bita", ita_Ef_bmd[type]);
    if (modelData == nullptr) /* JUT_ASSERT(583, modelData != NULL) */
        JUT_ASSERT_fail("d_a_bita.cpp", 0x247, "modelData != (0)");
    model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (model == nullptr)
        return FALSE;
    i_this->mpModelEf = model;

    /* new mDoExt_brkAnm() */
    void *p = operator_new(0x78);
    if (p != nullptr)
        p = BitaBrkAnm_ct(p);
    i_this->mpBrkAnm = (BitaBrkAnm *)p;
    if (p == nullptr)
        return FALSE;
    J3DModel *ef = i_this->mpModelEf;
    J3DAnmTevRegKey *brk = (J3DAnmTevRegKey *)dComIfG_getObjectRes("Bita", ita_Ef[type]);
    if (!i_this->mpBrkAnm->init(ef->getModelData(), brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false,
                                0))
        return FALSE;

    i_this->mpBgW = new_dBgW();
    if (i_this->mpBgW == nullptr)
        return FALSE;
    cBgD_t *dzb = (cBgD_t *)dComIfG_getObjectRes("Bita", ita_dzb[type]);
    if (i_this->mpBgW->Set(dzb, dBgW::MOVE_BG_e, &i_this->mMtx) == true)
        return FALSE;

    ((BitaBgWHD *)(dBgW *)i_this->mpBgW)->mCrrFunc =
        0x024EE658; // HD: native dBgS_MoveBGProc_Typical callback;
    return TRUE;
}

/* 02098970 */
static cPhs_State daBita_Create(fopAc_ac_c *i_ac) {
    bita_class *i_this = (bita_class *)i_ac;
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

    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, "Bita");
    if (rt == cPhs_COMPLEATE_e) {
        btd = nullptr;
        u32 prm = fopAcM_GetParam(i_this);
        i_this->mType = (u8)(prm >> 0);
        i_this->mPrmScale = (u8)(fopAcM_GetParam(i_this) >> 8);
        if ((u8)prm == 0xFF)
            i_this->mType = 0;

        if (!fopAcM_entrySolidHeap(i_this, useHeapInit, 0x30C0))
            return cPhs_ERROR_e;

        if (dComIfG_Bgsp()->Regist(i_this->mpBgW, i_this))
            return cPhs_ERROR_e;

        switch (i_this->mPrmScale) {
        case 1:
            i_this->scale.set(1.5f, 1.5f, 1.5f);
            break;
        case 2:
            i_this->scale.set(2.0f, 2.0f, 2.0f);
            break;
        case 3:
            i_this->scale.set(3.0f, 3.0f, 3.0f);
            break;
        default:
            i_this->scale.y = i_this->scale.z = i_this->scale.x = 1.0f;
            break;
        }

        i_this->cullMtx = gabi::ea(i_this->mpModel->getBaseTRMtx()); /* fopAcM_SetMtx */
        fopAcM_SetMin(i_this, -1000.0f * i_this->scale.x, -500.0f, -1000.0f * i_this->scale.z);
        fopAcM_SetMax(i_this, 1000.0f * i_this->scale.x, 500.0f, 1000.0f * i_this->scale.z);
        i_this->mpModel->setBaseScale(i_this->scale);
        i_this->mpModelEf->setBaseScale(i_this->scale);
        i_this->shape_angle.x = i_this->current.angle.x;
        i_this->shape_angle.y = i_this->current.angle.y;
        i_this->shape_angle.z = i_this->current.angle.z;
        base_mtx_set(i_this);
        i_this->mpBgW->Move();
        i_this->mCyl.Set(&body_cyl_src);
        i_this->mCyl.SetStts(&i_this->mStts);
    }

    return rt;
}

/* 02098C8C */
static void __sinit_d_a_bita_cpp() {
    __sinit_header_statics();
}

/* 02098D20 */
static void trivial_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02098D34 */
static void bita_class_dt(bita_class *i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(i_this); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02098DA0 */
static void empty_virtual(void *p) {}

/* WWHD glue
data btd 101913AC
data ita_bmd 1019138C
data ita_dzb 10191394
data ita_Ef_bmd 1019139C
data ita_Ef 101913A4
data body_cyl_src 101913D0
str 1000940C in 02098780 # "Bita"
str 10009400 in 02098728 # "Bita"
str 10009454 in 02098970 # "Bita"
str 10009414 "d_a_bita.cpp"
str 10009424 "modelData != (0)"
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32)
ext 025E80D0 BitaBrkAnm* BitaBrkAnm_ct(void*)
ext 025E8154 BOOL BitaBrkAnm::init(J3DModelData*, J3DAnmTevRegKey*, bool, s32, f32, s16, s16, bool, s32)
ext 025E83FC void BitaBrkAnm::entry(J3DModelData*, f32)
*/
