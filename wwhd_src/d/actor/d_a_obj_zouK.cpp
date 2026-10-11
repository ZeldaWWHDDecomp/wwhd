/**
 * d_a_obj_zouK.cpp (WWHD)
 * Object - Hyrule Castle knight statue.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_zouK.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * HD: no ground check and no simple shadow (mGndChk/mGndY are gone); _draw inlines setEffectMtx
 * and also stores the effect matrix in the model (+0xF8).
 */
#include "d/actor/d_a_obj_zouK.h"
const gc_tu_data gc_tu = {.safeString = 0x10033C08};
namespace daObjZouk {
static ZoukText M_arcname;
static ZoukText demoArcname;
static ZoukText sourceName;
static ZoukText modelAssert;
static ZoukText bckAssert;
static ZoukText anmAssert;
static ZoukText beforeAssert;
static ZoukText afterAssert;
static dCcD_SrcCyl cylinderSource;
static u32 adjustGuard;
static Mtx34 mtx_adj;
static Mtx34 adjustInitial;
static cXyz baseY;
WWHD_OFFSET(Act_c, mStts0, 0x430);
WWHD_OFFSET(Act_c, mCyl2, 0x744);

static inline J3DModel *anm_model(Act_c *a) { return ((mDoExt_McaMorf *)a->M_anm)->getModel(); }

/* 023BF0E4 */
void daObjZouk::Act_c::set_mtx() {
    mDoMtx_stack_c::transS(0.0f, 0.0f, 0.0f);
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx); /* mDoMtx_copy */
    J3DModel_setBaseScale(anm_model(this), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(anm_model(this), mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

/* 023BF550 */
static u32 Act_c_solidHeapCB(fopAc_ac_c *i_this) {
    /* tail branch: r3 is create_heap's register unchanged */
    return zouk_createHeapTail(i_this);
}

/* 023BF1FC */
bool daObjZouk::Act_c::create_heap() {
    J3DModelData *mdl_data = (J3DModelData *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_VZOUK_BDL_VZOUK_e, gc_tu.safeString);
    if (mdl_data == nullptr) /* JUT_ASSERT(369, mdl_data != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0x171, modelAssert.bytes);
    J3DAnmTransform *bck = (J3DAnmTransform *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_VZOUK_BCK_VZOUK_e, gc_tu.safeString);
    M_bck_data = bck;
    if (bck == nullptr) /* JUT_ASSERT(373, M_bck_data != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0x175, bckAssert.bytes);

    bool anm_ok;
    if (mdl_data != nullptr && M_bck_data != nullptr) {
        mDoExt_McaMorf *anm = mDoExt_McaMorf::create(nullptr, mdl_data, nullptr, nullptr,
                                                     M_bck_data, J3DFrameCtrl::EMode_NONE, 1.0f, 0,
                                                     -1, 1, nullptr, 0, 0x11020203);
        M_anm = anm;
        anm_ok = anm != nullptr;
    } else {
        anm_ok = M_anm != nullptr;
    }
    if (!anm_ok) { /* JUT_ASSERT(387, M_anm != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0x183, anmAssert.bytes);
        anm_ok = M_anm != nullptr;
    }
    if (anm_ok) {
        BOOL collected = dComIfGs_isCollect(0, 1);
        M_anm->mFrameCtrl.mRate = 0.0f; /* setPlaySpeed(0.0f) */
        mDoExt_McaMorf *anm = M_anm;
        if (collected) {
            /* setFrame(M_bck_data->getFrameMax() - 1.0f) (HD: virtual getFrameMax, vtable at +4) */
            ZoukFrameData *b = reinterpret_cast<ZoukFrameData *>(M_bck_data.get());
            s32 fmax = gabi::call_ptr<s32>(b->mVtable->mGetFrameMax.get(), b);
            anm->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi((f32)fmax - 1.0f);
        } else {
            anm->mFrameCtrl.mFrame = 0.0f; /* setFrame(0.0f) */
        }
    }
    set_mtx();

    cBgD_t *bgw_data_before = (cBgD_t *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_VZOUK_DZB_MAEKISI_e, gc_tu.safeString);
    if (bgw_data_before == nullptr) { /* JUT_ASSERT(406, bgw_data_before != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0x196, beforeAssert.bytes);
    } else {
        dBgW *w = new_dBgW();
        mBgBefore = w;
        if (w != nullptr) {
            if (zouk_setBg(w, bgw_data_before, MOVE_BG, &mBgMtx) != 0) /* cBgW::Set == true */
                return false;
        }
    }
    cBgD_t *bgw_data_after = (cBgD_t *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_VZOUK_DZB_ATOKISI_e, gc_tu.safeString);
    if (bgw_data_after == nullptr) { /* JUT_ASSERT(420, bgw_data_after != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0x1A4, afterAssert.bytes);
    } else {
        dBgW *w = new_dBgW();
        mBgAfter = w;
        if (w != nullptr) {
            if (zouk_setBg(w, bgw_data_after, 1, &mBgMtx) != 0)
                return false;
        }
    }
    bool ret = false;
    if (mdl_data != nullptr && M_bck_data != nullptr && M_anm != nullptr && mBgBefore != nullptr &&
        mBgAfter != nullptr)
        ret = true;
    return ret;
}

/* SetTgVec(cXyz::Zero) (struct copy) + OnTgNoHitMark */
static inline void cyl_init(Act_c *a, dCcD_Stts *stts, dCcD_Cyl *cyl) {
    stts->Init(0xFF, 0xFF, a);
    cyl->Set(&cylinderSource);
    cyl->SetStts(stts);
    cyl->mGObjTg.mVec.copy(*cXyz_Zero);
    cyl->mGObjTg.mSPrm = cyl->mGObjTg.mSPrm | 4;
}

/* 023BF554 */
cPhs_State daObjZouk::Act_c::_create() {
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            dCcD_Stts_ct(&mStts0);
            dCcD_Cyl_ct(&mCyl0, AAB_VTBL);
            dCcD_Stts_ct(&mStts1);
            dCcD_Cyl_ct(&mCyl1, AAB_VTBL);
            dCcD_Stts_ct(&mStts2);
            dCcD_Cyl_ct(&mCyl2, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname.bytes);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)Act_c_solidHeapCB), 0x0)) {
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(anm_model(this))); /* fopAcM_SetMtx */
            fopAcM_setCullSizeBox(this, -1000.0f, -0.0f, -1000.0f, 1000.0f, 2800.0f, 1000.0f);
            cyl_init(this, &mStts0, &mCyl0);
            cyl_init(this, &mStts1, &mCyl1);
            cyl_init(this, &mStts2, &mCyl2);
            field_0x294 = 0;
            field_0x298 = 0;
            s32 arg0 = param_get_arg0();
            if (arg0 == 0) {
                if (dComIfGs_isCollect(0, 1)) {
                    if (mBgAfter != nullptr) {
                        dBgS *bgs = dComIfG_Bgsp();
                        dBgS_Regist(bgs, mBgAfter, this);
                        zouk_dBgW_SetCrrFunc(mBgAfter, 0);
                        mBgMode = AFTER;
                    }
                } else {
                    if (mBgBefore != nullptr) {
                        dBgS *bgs = dComIfG_Bgsp();
                        dBgS_Regist(bgs, mBgBefore, this);
                        zouk_dBgW_SetCrrFunc(mBgBefore, 0);
                        mBgMode = BEFORE;
                    }
                }
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}

/* 023BF8B8 */
bool daObjZouk::Act_c::_delete() {
    if (heap != nullptr) {
        if (mBgBefore != nullptr && dBgW_ChkUsed(mBgBefore)) {
            dBgS *bgs = dComIfG_Bgsp();
            cBgS_Release(bgs, mBgBefore);
        }
        if (mBgAfter != nullptr && dBgW_ChkUsed(mBgAfter)) {
            dBgS *bgs = dComIfG_Bgsp();
            cBgS_Release(bgs, mBgAfter);
        }
    }
    dComIfG_resDelete(&mPhs, M_arcname.bytes);
    return TRUE;
}

/* 023BFA24 */
u32 daObjZouk::Act_c::play_stop_joint_anime() {
    return ((mDoExt_McaMorf *)M_anm)->play(nullptr, 0, 0);
}

/* 023BF950 */
BOOL daObjZouk::Act_c::jokai_demo() {
    if (zouk_dComIfGp_demo_getActor(demoActorID) != nullptr) {
        /* ENABLE_TRANS | ENABLE_ROTATE | ENABLE_ANM | ENABLE_ANM_FRAME */
        zouk_dDemo_setDemoData(this, 0x6A, M_anm, demoArcname.bytes /* "VzouK" */, 0, 0, 0, 0);
        return true;
    } else {
        return false;
    }
}

/* 023BFA38 */
bool daObjZouk::Act_c::_execute() {
    set_mtx();
    texture_scroll();
    zouk_fopAcM_rollPlayerCrash(this, 288.0f, 0x0D);
    if (!jokai_demo())
        play_stop_joint_anime();
    if (param_get_arg0() == 0) {
        if (dComIfGp_event_runCheck()) {
            if (mBgBefore != nullptr && mBgAfter != nullptr) {
                if (dBgW_ChkUsed(mBgAfter)) {
                    dBgS *bgs = dComIfG_Bgsp();
                    cBgS_Release(bgs, mBgAfter);
                }
                if (!dBgW_ChkUsed(mBgBefore)) {
                    dBgS *bgs = dComIfG_Bgsp();
                    dBgS_Regist(bgs, mBgBefore, this);
                    zouk_dBgW_SetCrrFunc(mBgBefore, 0);
                    mBgMode = BEFORE;
                }
                dBgW *b = mBgBefore;
                if (dBgW_ChkUsed(b))
                    dBgW_Move(b);
            }
        } else {
            if (mBgMode == BEFORE && dComIfGs_isCollect(0, 1) && mBgBefore != nullptr &&
                mBgAfter != nullptr) {
                if (dBgW_ChkUsed(mBgBefore)) {
                    dBgS *bgs = dComIfG_Bgsp();
                    cBgS_Release(bgs, mBgBefore);
                }
                dBgS *bgs = dComIfG_Bgsp();
                dBgS_Regist(bgs, mBgAfter, this);
                zouk_dBgW_SetCrrFunc(mBgAfter, 0);
                mBgMode = AFTER;
            }
            if (mBgMode == BEFORE) {
                if (mBgBefore != nullptr)
                    dBgW_Move(mBgBefore);
            } else {
                if (mBgAfter != nullptr)
                    dBgW_Move(mBgAfter);
            }
        }
    }
    return true;
}

/* setEffectMtx(&eyePos, 0.5f), inlined into _draw. HD: the effect matrix is also kept in the model
 * (+0xF8), and each material's type-11 texture matrices point at it (dirty bits set) */
static inline void setEffectMtx(Act_c *a) {
    /* static Mtx mtx_adj (initialised on first use: guard 101FDC68, copied from 101CE210) */

    if ((u32)adjustGuard == 0) {
        adjustGuard = 1;
        memcpy_g(reinterpret_cast<u8 *>(&mtx_adj), reinterpret_cast<u8 *>(&adjustInitial), 0x30);
    }
    ZoukCamera *camera = gabi::at<ZoukCamera>(zouk_dCam_getCamera());
    gabi::Local<cXyz> lookDir, lightDir, refl;
    cXyz_mi(&a->eyePos, lookDir, &camera->mEye); /* *pos - camera->view.mLookat.mEye */
    zouk_dKyr_get_vectle_calc(&a->tevStr.mLightPosWorld /* tevStr.mLightPosWorld */, &a->eyePos,
                              lightDir);
    zouk_C_VECHalfAngle(lookDir, lightDir, refl);
    gabi::Local<Mtx34> lookatMtx;
    zouk_C_MTXLookAt(lookatMtx, cXyz_Zero, &baseY, refl);
    Mtx34 *now = mDoMtx_stack_c::get();
    zouk_PSMTXScale(now, 2.0f, 2.0f, 1.0f); /* scaleS(1 / 0.5, 1 / 0.5, 1) */
    zouk_PSMTXConcat(now, &mtx_adj, now);
    zouk_PSMTXConcat(now, lookatMtx, now);
    now->m[0][3] = 0.0f;
    now->m[1][3] = 0.0f;
    now->m[2][3] = 0.0f;
    PSMTXCopy(now, &reinterpret_cast<ZoukModel *>(anm_model(a))->mEffectMtx);

    ZoukModel *model = reinterpret_cast<ZoukModel *>(anm_model(a));
    u16 num = (u16)model->mMaterialNum;
    for (u16 i = 0; i < num; i++) {
        ZoukMaterial *mat = &model->mMaterials.get()[i];
        for (u32 j = 0; j < 8; j++) {
            gabi::Local<be<s32>> idx;
            *idx = -1;
            u32 texMtx = zouk_J3DMat_getTexMtx(gabi::ea(mat), j, idx.get());
            if (texMtx != 0 && gabi::at<ZoukTexMtx>(texMtx)->mType == TEX_EFFECT) {
                /* mark the texture matrix (and the one it refers to) dirty */
                ZoukMaterialResource *d = mat->mResource;
                s32 off = d->mTextureTableOffset;
                u32 tab = off != 0 ? gabi::ea(&d->mTextureTableOffset) + off : 0;
                s32 n = *idx;
                ZoukTextureEntry *ent =
                    gabi::at<ZoukTextureEntry>(tab + n * sizeof(ZoukTextureEntry));
                if ((s32)ent->mIndex >= 0) {
                    mat->mFlags |= TEX_DIRTY;
                    be<u32> *bits = &mat->mDirtyBits.get()[n >> 5];
                    *bits |= 1u << (n & 31);
                    d = mat->mResource;
                }
                s32 off2 = d->mTextureTableOffset;
                u32 tab2 = off2 != 0 ? gabi::ea(&d->mTextureTableOffset) + off2 : 0;
                u16 m = (u16)ent->mReference;
                if ((s32)gabi::at<ZoukTextureEntry>(tab2 + m * sizeof(ZoukTextureEntry))->mIndex >=
                    0) {
                    mat->mFlags |= TEX_DIRTY;
                    be<u32> *bits = &mat->mDirtyBits.get()[m >> 5];
                    *bits |= 1u << (m & 31);
                }
                be<u32> *slot = gabi::at<be<u32>>(zouk_J3DMat_getEffectMtxSlot(gabi::ea(mat), j));
                if (slot != nullptr && (u32)*slot != 0)
                    *slot = gabi::ea(
                        &model->mEffectMtx); /* texMtx->getTexMtxInfo().setEffectMtx(...) */
            }
        }
    }
}

/* 023BFC08 */
bool daObjZouk::Act_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    dComIfGd_setListBG();
    mDoExt_McaMorf *anm = M_anm;
    setLightTevColorType(dKy_getEnvlight(), anm->getModel(), &tevStr);
    setEffectMtx(this);
    ((mDoExt_McaMorf *)M_anm)->updateDL();
    dComIfGd_setList();
    return true;
}

/* method table entries (HD: tail branches) */
/* 023BFEDC */
static cPhs_State Mthd_Create(void *i_this) { return ((Act_c *)i_this)->_create(); }
/* 023BFEE0 */
static bool Mthd_Delete(void *i_this) { return ((Act_c *)i_this)->_delete(); }
/* 023BFEE4 */
static bool Mthd_Execute(void *i_this) { return ((Act_c *)i_this)->_execute(); }
/* 023BFEE8 */
static bool Mthd_Draw(void *i_this) { return ((Act_c *)i_this)->_draw(); }
/* 023C0034 */
static BOOL Mthd_IsDelete(void *) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 023BFEEC */
static void __sinit_d_a_obj_zouK_cpp() { sinit_header_statics(0x1046CAD0, 0x101CE240); }

/* 023BFF80 */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023BFF94 */
static void SafeString_assureTermination(void *) {}

/* 023BFF98 */
static void Act_c_dt(Act_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Stts_dt(&i_this->mStts2, 2);
        dCcD_Cyl_dt(&i_this->mCyl1, 2);
        dCcD_Stts_dt(&i_this->mStts1, 2);
        dCcD_Cyl_dt(&i_this->mCyl0, 2);
        dCcD_Stts_dt(&i_this->mStts0, 2);
        zouk_destroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
} // namespace daObjZouk

using namespace daObjZouk;
/* WWHD glue (tools/verify/autoglue.py)
data M_arcname 10033D48 nocheck
data demoArcname 10033D2C nocheck
data sourceName 10033CE4 nocheck
data modelAssert 10033CD4 nocheck
data bckAssert 10033CB0 nocheck
data anmAssert 10033CF8 nocheck
data beforeAssert 10033D04 nocheck
data afterAssert 10033CC0 nocheck
data cylinderSource 10033C5C nocheck
data adjustGuard 101FDC68
data mtx_adj 101FDC6C
data adjustInitial 101CE210
data baseY 101FFBC0
*/
