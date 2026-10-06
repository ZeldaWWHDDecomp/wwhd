/**
 * d_a_obj_zouK.cpp (WWHD)
 * Object - Statue (VzouK; moves when the Triforce charts/pearls are collected).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_zouK.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * HD: no ground check and no simple shadow (mGndChk/mGndY are gone); _draw inlines setEffectMtx
 * and also stores the effect matrix in the model (+0xF8).
 */
#include "bindings.h"

#define M_arcname STR(0x10033D48)  /* "VzouK" */
#define SAFESTRING_VTBL 0x10033C08 /* this TU's sead::SafeString vtable */
#define ACT_VTBL 0x10033C30        /* daObjZouk::Act_c vtable (HD) */
#define AAB_VTBL 0x10033C20        /* this TU's cM3dGAab vtable */
#define M_cyl_src 0x10033C5C       /* dCcD_SrcCyl */
#define FILE_NAME STR(0x10033CE4)
#define solidHeapCB 0x023BF550

enum {
    dRes_INDEX_VZOUK_BCK_VZOUK_e = 5,
    dRes_INDEX_VZOUK_BDL_VZOUK_e = 8,
    dRes_INDEX_VZOUK_DZB_MAEKISI_e = 0xB,
    dRes_INDEX_VZOUK_DZB_ATOKISI_e = 0xC,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02349DFC daObj::PrmAbstract<int>(actor, width, shift) (shared out-of-line copy) */
static inline s32 daObj_PrmAbstract_i(fopAc_ac_c* a, s32 w, s32 s) { return gabi::call<s32>(0x02349DFC, a, w, s); }
/* 025B7A2C dSv_player_collect_c::isCollect(int, u8) on save+0xD4 */
static inline BOOL dComIfGs_isCollect(s32 i, u8 bit) {
    return gabi::call<BOOL>(0x025B7A2C, gabi::load<u32>(0x101F84DC) + 0xD4, i, bit);
}
/* 025D69FC fopAcM_rollPlayerCrash(actor, f32 dist, u32 flags) */
static inline void fopAcM_rollPlayerCrash(fopAc_ac_c* a, f32 d, u32 f) { gabi::call(0x025D69FC, a, d, f); }
/* dComIfGp_event_runCheck(): play byte +0x5292 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* dBgW::SetCrrFunc: +0xA8 */
static inline void dBgW_SetCrrFunc(dBgW* w, u32 f) { gabi::store<u32>(gabi::ea(w) + 0xA8, f); }
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, HD p6, HD p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, m, arc, n, ids, p6, p7);
}
/* dComIfGp_demo_getActor(id) (HD inline): id 0 or above 0x20 gives NULL; the demo object at
 * *0x101D5FFC (asserted), 02526E70 dDemo_object_c::getActor(u8) */
static inline void* dComIfGp_demo_getActor(u8 id) {
    if (id == 0 || id > 0x20)
        return nullptr;
    u32 obj = gabi::load<u32>(0x101D5FFC);
    if (obj == 0) { /* JUT_ASSERT(0x23a, ...) */
        JUT_ASSERT_fail(STR(0x10033C50), 0x23A, STR(0x10033C40));
        obj = gabi::load<u32>(0x101D5FFC);
    }
    return gabi::call<void*>(0x02526E70, obj, id);
}
/* matrix / vector helpers */
static inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void C_VECHalfAngle(const cXyz* a, const cXyz* b, cXyz* h) { gabi::call(0x028E9E88, a, b, h); }
static inline void C_MTXLookAt(Mtx34* m, const cXyz* eye, const cXyz* up, const cXyz* target) { gabi::call(0x028E9684, m, eye, up, target); }
static inline void dKyr_get_vectle_calc(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x02563F64, a, b, out); }
static inline u32 dCam_getCamera() { return gabi::call<u32>(0x024F8020); }
/* HD J3D material helpers: 027FA974 texMtx(j, &idx), 027FA678 effect-matrix slot(j) */
static inline u32 J3DMat_getTexMtx(u32 mat, u32 j, be<s32>* idx) { return gabi::call<u32>(0x027FA974, mat, j, idx); }
static inline u32 J3DMat_getEffectMtxSlot(u32 mat, u32 j) { return gabi::call<u32>(0x027FA678, mat, j); }
#define cXyz_BaseY gabi::at<cXyz>(0x101FFBC0)

namespace daObjZouk {
struct Act_c : fopAc_ac_c {
    s32 param_get_arg0() { return daObj_PrmAbstract_i(this, 0x04, 0x00); }
    bool create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    void texture_scroll() {}
    u32 play_stop_joint_anime();
    BOOL jokai_demo();
    bool _execute();
    bool _draw();

    /* 0x3AC */ be<u32> field_0x294;
    /* 0x3B0 */ be<u32> field_0x298;
    /* 0x3B4 */ request_of_phase_process_class mPhs;
    /* 0x3BC */ Mtx34 mBgMtx;
    /* 0x3EC */ Mtx34 mMtx;
    /* 0x41C */ gptr<J3DAnmTransform> M_bck_data;
    /* 0x420 */ gptr<mDoExt_McaMorf> M_anm;
    /* 0x424 */ be<s32> mBgMode;
    /* 0x428 */ gptr<dBgW> mBgBefore;
    /* 0x42C */ gptr<dBgW> mBgAfter;
    /* 0x430 */ dCcD_Stts mStts0;
    /* 0x46C */ dCcD_Cyl mCyl0;
    /* 0x59C */ dCcD_Stts mStts1;
    /* 0x5D8 */ dCcD_Cyl mCyl1;
    /* 0x708 */ dCcD_Stts mStts2;
    /* 0x744 */ dCcD_Cyl mCyl2;
};
WWHD_OFFSET(Act_c, mBgMtx, 0x3BC);
WWHD_OFFSET(Act_c, M_anm, 0x420);
WWHD_OFFSET(Act_c, mStts0, 0x430);
WWHD_OFFSET(Act_c, mCyl2, 0x744);

static inline J3DModel* anm_model(Act_c* a) { return ((mDoExt_McaMorf*)a->M_anm)->getModel(); }

/* 023BF0E4 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x023BF0E4, void, this);
    mDoMtx_stack_c::transS(0.0f, 0.0f, 0.0f);
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx); /* mDoMtx_copy */
    J3DModel_setBaseScale(anm_model(this), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(anm_model(this), mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}
VERIFY(0x023BF0E4, &Act_c::set_mtx);

/* 023BF550: solidHeapCB (HD: tail branch) */
static u32 Act_c_solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023BF550, u32, i_this);
    /* tail branch: r3 is create_heap's register unchanged */
    return gabi::call<u32>(0x023BF1FC, i_this);
}
VERIFY(0x023BF550, Act_c_solidHeapCB);

/* 023BF1FC */
bool Act_c::create_heap() {
    WWHD_FUNC(0x023BF1FC, bool, this);
    J3DModelData* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VZOUK_BDL_VZOUK_e, SAFESTRING_VTBL);
    if (mdl_data == nullptr) /* JUT_ASSERT(369, mdl_data != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x171, STR(0x10033CD4));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VZOUK_BCK_VZOUK_e, SAFESTRING_VTBL);
    M_bck_data = bck;
    if (bck == nullptr) /* JUT_ASSERT(373, M_bck_data != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x175, STR(0x10033CB0));

    bool anm_ok;
    if (mdl_data != nullptr && M_bck_data != nullptr) {
        mDoExt_McaMorf* anm = mDoExt_McaMorf::create(nullptr, mdl_data, nullptr, nullptr, M_bck_data, 0 /* EMode_NONE */, 1.0f, 0,
                                                     -1, 1, nullptr, 0, 0x11020203);
        M_anm = anm;
        anm_ok = anm != nullptr;
    } else {
        anm_ok = M_anm != nullptr;
    }
    if (!anm_ok) { /* JUT_ASSERT(387, M_anm != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x183, STR(0x10033CF8));
        anm_ok = M_anm != nullptr;
    }
    if (anm_ok) {
        BOOL collected = dComIfGs_isCollect(0, 1);
        gabi::store<f32>(gabi::ea((mDoExt_McaMorf*)M_anm) + 0x98, 0.0f); /* setPlaySpeed(0.0f) */
        u32 anm = gabi::ea((mDoExt_McaMorf*)M_anm);
        if (collected) {
            /* setFrame(M_bck_data->getFrameMax() - 1.0f) (HD: virtual getFrameMax, vtable at +4) */
            u32 b = gabi::ea((J3DAnmTransform*)M_bck_data);
            s32 fmax = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(b + 4) + 0x14), b);
            gabi::store<f32>(anm + 0x9C, (f32)(s16)gabi::ftoi((f32)fmax - 1.0f));
        } else {
            gabi::store<f32>(anm + 0x9C, 0.0f); /* setFrame(0.0f) */
        }
    }
    set_mtx();

    cBgD_t* bgw_data_before = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VZOUK_DZB_MAEKISI_e, SAFESTRING_VTBL);
    if (bgw_data_before == nullptr) { /* JUT_ASSERT(406, bgw_data_before != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x196, STR(0x10033D04));
    } else {
        dBgW* w = new_dBgW();
        mBgBefore = w;
        if (w != nullptr) {
            if (gabi::call<u32>(0x0200A030, w, bgw_data_before, 1 /* MOVE_BG_e */, &mBgMtx) != 0) /* cBgW::Set == true */
                return false;
        }
    }
    cBgD_t* bgw_data_after = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VZOUK_DZB_ATOKISI_e, SAFESTRING_VTBL);
    if (bgw_data_after == nullptr) { /* JUT_ASSERT(420, bgw_data_after != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x1A4, STR(0x10033CC0));
    } else {
        dBgW* w = new_dBgW();
        mBgAfter = w;
        if (w != nullptr) {
            if (gabi::call<u32>(0x0200A030, w, bgw_data_after, 1, &mBgMtx) != 0)
                return false;
        }
    }
    bool ret = false;
    if (mdl_data != nullptr && M_bck_data != nullptr && M_anm != nullptr && mBgBefore != nullptr && mBgAfter != nullptr)
        ret = true;
    return ret;
}
VERIFY(0x023BF1FC, &Act_c::create_heap);

/* SetTgVec(cXyz::Zero) (struct copy) + OnTgNoHitMark */
static inline void cyl_init(Act_c* a, dCcD_Stts* stts, dCcD_Cyl* cyl) {
    stts->Init(0xFF, 0xFF, a);
    cyl->Set(gabi::at<dCcD_SrcCyl>(M_cyl_src));
    cyl->SetStts(stts);
    cyl->mGObjTg.mVec.copy(*cXyz_Zero);
    cyl->mGObjTg.mSPrm = cyl->mGObjTg.mSPrm | 4;
}

/* 023BF554 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x023BF554, cPhs_State, this);
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
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0x0)) {
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
                        dBgS* bgs = dComIfG_Bgsp();
                        dBgS_Regist(bgs, mBgAfter, this);
                        dBgW_SetCrrFunc(mBgAfter, 0);
                        mBgMode = 1;
                    }
                } else {
                    if (mBgBefore != nullptr) {
                        dBgS* bgs = dComIfG_Bgsp();
                        dBgS_Regist(bgs, mBgBefore, this);
                        dBgW_SetCrrFunc(mBgBefore, 0);
                        mBgMode = 0;
                    }
                }
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}
VERIFY(0x023BF554, &Act_c::_create);

/* 023BF8B8 */
bool Act_c::_delete() {
    WWHD_FUNC(0x023BF8B8, bool, this);
    if (heap != nullptr) {
        if (mBgBefore != nullptr && dBgW_ChkUsed(mBgBefore)) {
            dBgS* bgs = dComIfG_Bgsp();
            cBgS_Release(bgs, mBgBefore);
        }
        if (mBgAfter != nullptr && dBgW_ChkUsed(mBgAfter)) {
            dBgS* bgs = dComIfG_Bgsp();
            cBgS_Release(bgs, mBgAfter);
        }
    }
    dComIfG_resDelete(&mPhs, M_arcname);
    return TRUE;
}
VERIFY(0x023BF8B8, &Act_c::_delete);

/* 023BFA24 */
u32 Act_c::play_stop_joint_anime() {
    WWHD_FUNC(0x023BFA24, u32, this);
    return ((mDoExt_McaMorf*)M_anm)->play(nullptr, 0, 0);
}
VERIFY(0x023BFA24, &Act_c::play_stop_joint_anime);

/* 023BF950 */
BOOL Act_c::jokai_demo() {
    WWHD_FUNC(0x023BF950, BOOL, this);
    if (dComIfGp_demo_getActor(demoActorID) != nullptr) {
        /* ENABLE_TRANS | ENABLE_ROTATE | ENABLE_ANM | ENABLE_ANM_FRAME */
        dDemo_setDemoData(this, 0x6A, M_anm, STR(0x10033D2C) /* "VzouK" */, 0, 0, 0, 0);
        return true;
    } else {
        return false;
    }
}
VERIFY(0x023BF950, &Act_c::jokai_demo);

/* 023BFA38 */
bool Act_c::_execute() {
    WWHD_FUNC(0x023BFA38, bool, this);
    set_mtx();
    texture_scroll();
    fopAcM_rollPlayerCrash(this, 288.0f, 0x0D);
    if (!jokai_demo())
        play_stop_joint_anime();
    if (param_get_arg0() == 0) {
        if (dComIfGp_event_runCheck()) {
            if (mBgBefore != nullptr && mBgAfter != nullptr) {
                if (dBgW_ChkUsed(mBgAfter)) {
                    dBgS* bgs = dComIfG_Bgsp();
                    cBgS_Release(bgs, mBgAfter);
                }
                if (!dBgW_ChkUsed(mBgBefore)) {
                    dBgS* bgs = dComIfG_Bgsp();
                    dBgS_Regist(bgs, mBgBefore, this);
                    dBgW_SetCrrFunc(mBgBefore, 0);
                    mBgMode = 0;
                }
                dBgW* b = mBgBefore;
                if (dBgW_ChkUsed(b))
                    dBgW_Move(b);
            }
        } else {
            if (mBgMode == 0 && dComIfGs_isCollect(0, 1) && mBgBefore != nullptr && mBgAfter != nullptr) {
                if (dBgW_ChkUsed(mBgBefore)) {
                    dBgS* bgs = dComIfG_Bgsp();
                    cBgS_Release(bgs, mBgBefore);
                }
                dBgS* bgs = dComIfG_Bgsp();
                dBgS_Regist(bgs, mBgAfter, this);
                dBgW_SetCrrFunc(mBgAfter, 0);
                mBgMode = 1;
            }
            if (mBgMode == 0) {
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
VERIFY(0x023BFA38, &Act_c::_execute);

/* setEffectMtx(&eyePos, 0.5f), inlined into _draw. HD: the effect matrix is also kept in the model
 * (+0xF8), and each material's type-11 texture matrices point at it (dirty bits set) */
static inline void setEffectMtx(Act_c* a) {
    /* static Mtx mtx_adj (initialised on first use: guard 101FDC68, copied from 101CE210) */
    const u32 guard = 0x101FDC68, mtx_adj = 0x101FDC6C;
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(mtx_adj), gabi::at<u8>(0x101CE210), 0x30);
    }
    u32 camera = dCam_getCamera();
    gabi::Local<cXyz> lookDir, lightDir, refl;
    cXyz_mi(&a->eyePos, lookDir, gabi::at<cXyz>(camera + 0xDC)); /* *pos - camera->view.mLookat.mEye */
    dKyr_get_vectle_calc(gabi::at<cXyz>(gabi::ea(a) + 0x194) /* tevStr.mLightPosWorld */, &a->eyePos, lightDir);
    C_VECHalfAngle(lookDir, lightDir, refl);
    gabi::Local<Mtx34> lookatMtx;
    C_MTXLookAt(lookatMtx, cXyz_Zero, cXyz_BaseY, refl);
    Mtx34* now = mDoMtx_stack_c::get();
    PSMTXScale(now, 2.0f, 2.0f, 1.0f); /* scaleS(1 / 0.5, 1 / 0.5, 1) */
    PSMTXConcat(now, gabi::at<Mtx34>(mtx_adj), now);
    PSMTXConcat(now, lookatMtx, now);
    now->m[0][3] = 0.0f;
    now->m[1][3] = 0.0f;
    now->m[2][3] = 0.0f;
    PSMTXCopy(now, gabi::at<Mtx34>(gabi::ea(anm_model(a)) + 0xF8));

    u32 model = gabi::ea(anm_model(a));
    u16 num = gabi::load<u16>(model + 0x2A);
    for (u16 i = 0; i < num; i++) {
        u32 mat = gabi::load<u32>(model + 0x34) + i * 0x3C;
        for (u32 j = 0; j < 8; j++) {
            gabi::Local<be<s32>> idx;
            *idx = -1;
            u32 texMtx = J3DMat_getTexMtx(mat, j, idx.get());
            if (texMtx != 0 && gabi::load<s32>(texMtx) == 11) {
                /* mark the texture matrix (and the one it refers to) dirty */
                u32 d = gabi::load<u32>(mat);
                s32 off = gabi::load<s32>(d + 0x34);
                u32 tab = off != 0 ? d + 0x34 + off : 0;
                s32 n = *idx;
                u32 ent = tab + n * 0x14;
                if (gabi::load<s32>(ent + 4) >= 0) {
                    gabi::store<u16>(mat + 4, (u16)(gabi::load<u16>(mat + 4) | 4));
                    u32 bits = gabi::load<u32>(mat + 0xC) + ((n >> 5) << 2);
                    gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (n & 31)));
                    d = gabi::load<u32>(mat);
                }
                s32 off2 = gabi::load<s32>(d + 0x34);
                u32 tab2 = off2 != 0 ? d + 0x34 + off2 : 0;
                u16 m = gabi::load<u16>(ent + 0xC);
                if (gabi::load<s32>(tab2 + m * 0x14 + 4) >= 0) {
                    gabi::store<u16>(mat + 4, (u16)(gabi::load<u16>(mat + 4) | 4));
                    u32 bits = gabi::load<u32>(mat + 0xC) + ((m >> 5) << 2);
                    gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (m & 31)));
                }
                u32 slot = J3DMat_getEffectMtxSlot(mat, j);
                if (slot != 0 && gabi::load<u32>(slot) != 0)
                    gabi::store<u32>(slot, model + 0xF8); /* texMtx->getTexMtxInfo().setEffectMtx(...) */
            }
        }
    }
}

/* 023BFC08 _draw (the matcher names it setEffectMtx) HD: TEV type 0, no simple shadow */
bool Act_c::_draw() {
    WWHD_FUNC(0x023BFC08, bool, this);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    dComIfGd_setListBG();
    mDoExt_McaMorf* anm = M_anm;
    setLightTevColorType(dKy_getEnvlight(), anm->getModel(), &tevStr);
    setEffectMtx(this);
    ((mDoExt_McaMorf*)M_anm)->updateDL();
    dComIfGd_setList();
    return true;
}
VERIFY(0x023BFC08, &Act_c::_draw);

/* method table entries (HD: tail branches) */
/* 023BFEDC */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x023BFEDC, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x023BFEDC, Mthd_Create);
/* 023BFEE0 */
static bool Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x023BFEE0, bool, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x023BFEE0, Mthd_Delete);
/* 023BFEE4 */
static bool Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x023BFEE4, bool, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x023BFEE4, Mthd_Execute);
/* 023BFEE8 */
static bool Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x023BFEE8, bool, i_this);
    return ((Act_c*)i_this)->_draw();
}
VERIFY(0x023BFEE8, Mthd_Draw);
/* 023C0034 */
static BOOL Mthd_IsDelete(void*) {
    WWHD_FUNC(0x023C0034, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023C0034, Mthd_IsDelete);

/* 023BFEEC */
static void __sinit_d_a_obj_zouK_cpp() {
    WWHD_FUNC(0x023BFEEC, void, (u32)0);
    sinit_header_statics(0x1046CAD0, 0x101CE240);
}
VERIFY(0x023BFEEC, __sinit_d_a_obj_zouK_cpp);

/* 023BFF80: sead::SafeString deleting destructor (this TU's copy; trivial) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023BFF80, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023BFF80, SafeString_dt);

/* 023BFF94: sead::SafeString::assureTermination (this TU's copy; empty) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x023BFF94, void, (u32)0);
}
VERIFY(0x023BFF94, SafeString_assureTermination);

/* 023BFF98: Act_c deleting destructor */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x023BFF98, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Stts_dt(&i_this->mStts2, 2);
        dCcD_Cyl_dt(&i_this->mCyl1, 2);
        dCcD_Stts_dt(&i_this->mStts1, 2);
        dCcD_Cyl_dt(&i_this->mCyl0, 2);
        dCcD_Stts_dt(&i_this->mStts0, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023BFF98, Act_c_dt);
}  // namespace daObjZouk
