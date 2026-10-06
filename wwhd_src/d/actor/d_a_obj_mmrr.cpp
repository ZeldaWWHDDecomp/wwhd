/**
 * d_a_obj_mmrr.cpp (WWHD)
 * Object - Earth Temple light-reflecting mirror (Mmirror)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_mmrr.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * WWHD range 02374AFC..02375C63 (02374AE0 belongs to d_a_obj_mknjd).
 * HD: L_attr is in .data (0x101CAFFC) and its triangle vertices are written by __sinit.
 */
#include "bindings.h"

#define M_arcname STR(0x1002CE00)        /* "Mmirror" */
#define SAFESTRING_VTBL 0x1002CC80       /* this TU's sead::SafeString vtable */
#define ACT_VTBL 0x1002CD48              /* daObjMmrr::Act_c vtable (HD virtual destructor) */
#define EFF_VTBL 0x1002CEA8              /* daObjMmrr::Eff_c vtable */
#define AAB_VTBL 0x1002CC98              /* this TU's cM3dGAab vtable */
#define FILE_NAME STR(0x1002CD78)        /* "d_a_obj_mmrr.cpp" */
#define M_tri_src gabi::at<dCcD_SrcTri>(0x1002CE08)
#define M_cps_src gabi::at<dCcD_SrcCps>(0x1002CE5C)

/* L_attr (0xD4) at 0x101CAFFC */
#define L_ATTR 0x101CAFFCu
#define L_attr_vtx(i, j) (L_ATTR + 0x24 * (i) + 0xC * (j))
#define L_attr_f(off) gabi::load<f32>(L_ATTR + (off))
#define L_attr_BC gabi::load<s16>(L_ATTR + 0xBC)
#define L_attr_BC_u gabi::load<u16>(L_ATTR + 0xBC)

enum { JA_SE_OBJ_MIRROR_REFLECT = 0x6961, JA_SE_OBJ_MIRROR_LIGHT = 0x7028 };
enum { dPa_name_ID_AK_SN_SASORIMIRROR00 = 0x8294 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void construct_array(u32 p, s32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void destroy_arr(u32 p, s32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0, 0); }
/* 0252A038 dDetect_c::chk_light(cXyz*) (play+0x5A20) */
static inline BOOL dComIfGp_getDetect_chk_light(cXyz* pos) { return gabi::call<BOOL>(0x0252A038, dComIfGp_ea() + PLAY_DETECT, pos); }
/* 0201924C cM3dGTri::setPos(a, b, c) */
static inline void cM3dGTri_setPos(u32 tri, cXyz* a, cXyz* b, cXyz* c) { gabi::call(0x0201924C, tri, a, b, c); }
/* 020181FC cM3dGCps::Set(const cM3dGCpsS&) */
static inline void cM3dGCps_Set(u32 cps, void* src) { gabi::call(0x020181FC, cps, src); }
static inline void PSVECSubtract(u32 a, u32 b, u32 out) { gabi::call(0x028E8DAC, a, b, out); }
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
static inline void cXyz_normalizeRS(u32 v) { gabi::call(0x0201B47C, v); }
/* dBgS_LinChk (stack, 0x6C) */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x30 - 0x24];
    /* 0x30 */ cXyz mCross;
    /* 0x3C */ u8 _3C[0x58 - 0x3C];
    /* 0x58 */ be<u32> __vtbl_58;
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
/* cM3dGCpsS {cXyz start, end; f32 radius} */
struct cM3dGCpsS_l {
    cXyz mStart;
    cXyz mEnd;
    be<f32> mRadius;
};
WWHD_SIZE(cM3dGCpsS_l, 0x1C);

static inline void word_copy(u32 dst, u32 src) { gabi::store<u32>(dst, gabi::load<u32>(src)); }

namespace daObjMmrr {

struct Eff_c : dPa_followEcallBack {
    void end();
    void remove();
};

struct Act_c : fopAc_ac_c {
    bool create_heap();
    void init_cc();
    void set_cc_rec_pos();
    void set_cc_trans_pos();
    void set_cull();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    void init_mtx();
    bool chk_light();
    void eff_start();
    void eff_stop();
    void eff_remove();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpMirrorModel;
    /* 0x3B8 */ gptr<J3DModel> mpBeamModel;
    /* 0x3BC */ mDoExt_btkAnm mMirrorBtkAnm;        /* HD 0x74 (GameCube 0x14) */
    /* 0x430 */ mDoExt_btkAnm mBeamBtkAnm;
    /* 0x4A4 */ dCcD_Tri field_0x2C8[5];
    /* 0xB34 */ dCcD_Stts field_0x958[5];
    /* 0xC60 */ dCcD_Cps field_0xA84;
    /* 0xD98 */ dCcD_Stts field_0xBBC;
    /* 0xDD4 */ be<u8> field_0xBF8;
    /* 0xDD5 */ be<u8> field_0xBF9;
    /* 0xDD6 */ u8 _DD6[2];
    /* 0xDD8 */ be<f32> field_0xBFC;
    /* 0xDDC */ be<f32> field_0xC00;
    /* 0xDE0 */ Eff_c field_0xC04;
};
WWHD_OFFSET(Act_c, mBeamBtkAnm, 0x430);
WWHD_OFFSET(Act_c, field_0xA84, 0xC60);
WWHD_OFFSET(Act_c, field_0xBF8, 0xDD4);
WWHD_OFFSET(Act_c, field_0xC04, 0xDE0);
WWHD_SIZE(Act_c, 0xDF4);

#define THIS gabi::ea(this)

/* 02374AFC */
void Eff_c::end() {
    WWHD_FUNC(0x02374AFC, void, this);
    if (mpEmitter) {
        gabi::store<u8>(gabi::ea(getEmitter()) + 0x247, 0); /* setGlobalAlpha(0) */
    }
    dPa_followEcallBack_end(this); /* dPa_followEcallBack::end() */
}
VERIFY(0x02374AFC, &Eff_c::end);

/* 02374B14 */
void Eff_c::remove() {
    WWHD_FUNC(0x02374B14, void, this);
    if (mpEmitter) {
        gabi::store<u8>(gabi::ea(getEmitter()) + 0x247, 0);
    }
    dPa_followEcallBack::remove(); /* this->end(): virtual */
}
VERIFY(0x02374B14, &Eff_c::remove);

/* 02374B38 */
bool Act_c::create_heap() {
    WWHD_FUNC(0x02374B38, bool, this);
    J3DModelData* bdl_Mmrr = (J3DModelData*)dComIfG_getObjectRes(M_arcname, 9, SAFESTRING_VTBL);
    if (bdl_Mmrr == nullptr)
        JUT_ASSERT_fail(FILE_NAME, 0x1e8, STR(0x1002CD58));
    mpMirrorModel = mDoExt_J3DModel__create(bdl_Mmrr, 0x80000, 0x11000222);

    J3DModelData* bdl_Yssmr00 = (J3DModelData*)dComIfG_getObjectRes(M_arcname, 0xB, SAFESTRING_VTBL);
    if (bdl_Yssmr00 == nullptr)
        JUT_ASSERT_fail(FILE_NAME, 0x1f1, STR(0x1002CD8C));
    mpBeamModel = mDoExt_J3DModel__create(bdl_Yssmr00, 0x80000, 0x11000222);

    J3DAnmTextureSRTKey* btk_Mmrr = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, 0xE, SAFESTRING_VTBL);
    if (btk_Mmrr == nullptr)
        JUT_ASSERT_fail(FILE_NAME, 0x1fc, STR(0x1002CD68));
    BOOL mirror_anm_init_res = mMirrorBtkAnm.init(bdl_Mmrr, btk_Mmrr, true, 2 /* EMode_LOOP */, 1.0f, 0, -1, false, 0);

    J3DAnmTextureSRTKey* btk_Yssmr00 = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, 0x10, SAFESTRING_VTBL);
    if (btk_Yssmr00 == nullptr)
        JUT_ASSERT_fail(FILE_NAME, 0x203, STR(0x1002CDA0));
    BOOL beam_anm_init_res = mBeamBtkAnm.init(bdl_Yssmr00, btk_Yssmr00, true, 2, 1.0f, 0, -1, false, 0);

    return mpMirrorModel && mpBeamModel && mirror_anm_init_res && beam_anm_init_res;
}
VERIFY(0x02374B38, &Act_c::create_heap);

/* 02374D28 */
static u8 solidHeapCB(fopAc_ac_c* i_actor) { /* tail call: create_heap's u8 result as is */
    WWHD_FUNC(0x02374D28, u8, i_actor);
    return gabi::call<u8>(0x02374B38, i_actor); /* ((Act_c*)i_actor)->create_heap() */
}
VERIFY(0x02374D28, solidHeapCB);

/* 02374D2C */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02374D2C, void, this);
    /* local_scale(scale.x, scale.y, scale.z * field_0xC00); mpBeamModel->setBaseScale(local_scale) */
    f32 z = scale.z * field_0xC00;
    u32 beam = gabi::ea(mpBeamModel);
    word_copy(beam + 0xBC, THIS + 0x330);
    word_copy(beam + 0xC0, THIS + 0x334);
    gabi::store<f32>(beam + 0xC4, z);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpMirrorModel, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpBeamModel, mDoMtx_stack_c::get());
}
VERIFY(0x02374D2C, &Act_c::set_mtx);

/* 02374E78 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x02374E78, void, this);
    u32 mirror = gabi::ea(mpMirrorModel);
    word_copy(mirror + 0xBC, THIS + 0x330);
    word_copy(mirror + 0xC0, THIS + 0x334);
    word_copy(mirror + 0xC4, THIS + 0x338);
    set_mtx();
}
VERIFY(0x02374E78, &Act_c::init_mtx);

/* 02374E98 */
void Act_c::set_cc_rec_pos() {
    WWHD_FUNC(0x02374E98, void, this);
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<cXyz> c;

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, L_attr_f(0xB4), L_attr_f(0xB8));
    mDoMtx_XrotM(mDoMtx_stack_c::get(), L_attr_BC);

    cXyz* out[3] = {a.get(), b.get(), c.get()};
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            u32 v = L_attr_vtx(i, j);
            word_copy(gabi::ea(temp.get()) + 0, v + 0);
            word_copy(gabi::ea(temp.get()) + 4, v + 4);
            word_copy(gabi::ea(temp.get()) + 8, v + 8);
            PSMTXMultVec(mDoMtx_stack_c::get(), temp.get(), out[j]);
        }
        cM3dGTri_setPos(gabi::ea(&field_0x2C8[i]) + 0x118, a.get(), b.get(), c.get());
    }
}
VERIFY(0x02374E98, &Act_c::set_cc_rec_pos);

/* 02374FC0 */
void Act_c::set_cc_trans_pos() {
    WWHD_FUNC(0x02374FC0, void, this);
    gabi::Local<cXyz> end;
    gabi::Local<cXyz> tmp;
    gabi::Local<cM3dGCpsS_l> cps;
    gabi::Local<dBgS_LinChk_l> chk;

    end->y = 0.0f;
    end->x = 0.0f;
    end->z = L_attr_f(0xD0);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(L_attr_f(0xC0), L_attr_f(0xC4), L_attr_f(0xC8));
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &cps->mStart);
    PSMTXMultVec(mDoMtx_stack_c::get(), end.get(), &cps->mEnd);
    cps->mRadius = L_attr_f(0xCC);

    /* dBgS_MirLightLinChk light_lin_chk (inline constructor) */
    cBgS_LinChk_ct(chk.get());
    chk->mpPolyPassChk = gabi::ea(chk.get()) + 0x58;
    for (int i = 0; i < 7; i++) chk->mPass[i] = 0;
    chk->mPass[3] = 1; /* light */
    chk->mpGrpPassChk = gabi::ea(chk.get()) + 0x64;
    chk->mGrp = 0x1F;
    chk->__vtbl_64 = 0x1002CD28;
    chk->__vtbl_10 = 0x1002CD08;
    chk->__vtbl_58 = 0x1002CD38;
    chk->__vtbl_20 = 0x1002CD18;

    dBgS_LinChk_Set(chk.get(), &cps->mStart, &cps->mEnd, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), chk.get())) {
        u32 e = gabi::ea(&cps->mEnd), x = gabi::ea(&chk->mCross);
        word_copy(e + 0, x + 0);
        word_copy(e + 8, x + 8);
        word_copy(e + 4, x + 4);
    }
    cM3dGCps_Set(THIS + 0xD78, cps.get());
    /* CalcAtVec: mVec = end - start */
    PSVECSubtract(THIS + 0xD84, THIS + 0xD78, THIS + 0xCDC);
    cXyz_normalizeRS(THIS + 0xCDC);
    /* cXyz(cps.mStart).abs(cps.mEnd) */
    f32 sx = cps->mStart.x, sy = cps->mStart.y, sz = cps->mStart.z;
    tmp->z = sz;
    tmp->y = sy;
    tmp->x = sx;
    f32 d = std_sqrtf(PSVECSquareDistance(tmp.get(), &cps->mEnd)) / L_attr_f(0xD0);
    /* ~dBgS_MirLightLinChk */
    chk->__vtbl_20 = 0x1002CCA8;
    chk->__vtbl_58 = 0x1002CCF8;
    chk->__vtbl_64 = 0x1002CCB8;
    field_0xC00 = d;
    cBgS_LinChk_dt(chk.get(), 0);
}
VERIFY(0x02374FC0, &Act_c::set_cc_trans_pos);

/* 023751B0 */
void Act_c::init_cc() {
    WWHD_FUNC(0x023751B0, void, this);
    for (int i = 0; i < 5; i++) {
        field_0x958[i].Init(0xff, 0xff, this);
        gabi::call(0x0251650C, &field_0x2C8[i], M_tri_src); /* dCcD_Tri::Set */
        field_0x2C8[i].SetStts(&field_0x958[i]);
        field_0x2C8[i].mGObjTg.mSPrm = field_0x2C8[i].mGObjTg.mSPrm | 4; /* OnTgNoHitMark */
        field_0xBBC.Init(0xff, 0xff, this);
        gabi::call(0x025164C0, &field_0xA84, M_cps_src); /* dCcD_Cps::Set */
        field_0xA84.SetStts(&field_0xBBC);
    }
    set_cc_rec_pos();
    set_cc_trans_pos();
}
VERIFY(0x023751B0, &Act_c::init_cc);

/* 02375264 */
void Act_c::set_cull() {
    WWHD_FUNC(0x02375264, void, this);
    f32 y1, z1;
    if (field_0xBF9) {
        u16 ang = L_attr_BC_u;
        f32 d0 = L_attr_f(0xD0);
        f32 s = cM_ssin(ang), c = cM_scos(ang);
        y1 = gabi::fnmsubs(d0, s, 680.0f); /* -d0 * sin + 680 */
        z1 = gabi::fmadds(d0, c, 160.0f);  /* d0 * cos + 160 */
    } else {
        f32 sin_val = 0.0f;
        y1 = sin_val + 680.0f;
        z1 = sin_val + 160.0f;
    }
    fopAcM_setCullSizeBox(this, -160.0f, -1.0f, -160.0f, 160.0f, y1, z1);
}
VERIFY(0x02375264, &Act_c::set_cull);

/* 02375300 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x02375300, cPhs_State, this);
    cPhs_State state;

    /* fopAcM_ct(this, Act_c): inline constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (THIS != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            mDoExt_btkAnm::ct(&mMirrorBtkAnm);
            mDoExt_btkAnm::ct(&mBeamBtkAnm);
            construct_array(THIS + 0x4A4, 5, 0x150, 0x02375B20);
            construct_array(THIS + 0xB34, 5, 0x3C, 0x02375AB8);
            /* dCcD_Cps */
            gabi::call(0x02515FB8, &field_0xA84);
            gabi::store<u32>(THIS + 0xD74, 0x100015A8);
            gabi::store<u32>(THIS + 0xD70, AAB_VTBL);
            gabi::call(0x02018150, THIS + 0xD78); /* cM3dGCps::cM3dGCps */
            field_0xA84.__vtbl_hitinf = 0x1004AF18;
            gabi::store<u32>(THIS + 0xD90, 0x1004AF60);
            gabi::store<u32>(THIS + 0xD74, 0x1004AF70);
            dCcD_Stts_ct(&field_0xBBC);
            dPa_followEcallBack_ct(&field_0xC04, 0, 0);
            field_0xC04.__vtbl = EFF_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    state = dComIfG_resLoad(&mPhs, M_arcname);
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x02374D28 /* solidHeapCB */, 0x1a80)) {
            J3DModel* m = mpMirrorModel;
            cullMtx = m ? gabi::ea(m) + 0xC8 : 0;
            init_mtx();
            init_cc();
            field_0xBF8 = false;
            field_0xBFC = 0.0f;
            field_0xBF9 = false;
            set_cull();
        } else {
            state = cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x02375300, &Act_c::_create);

/* 023754BC */
void Act_c::eff_remove() {
    WWHD_FUNC(0x023754BC, void, this);
    field_0xC04.remove();
}
VERIFY(0x023754BC, &Act_c::eff_remove);

/* 023754C4 */
bool Act_c::_delete() {
    WWHD_FUNC(0x023754C4, bool, this);
    eff_remove();
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}
VERIFY(0x023754C4, &Act_c::_delete);

/* 02375504 */
bool Act_c::chk_light() {
    WWHD_FUNC(0x02375504, bool, this);
    bool ret = false;
    if (dComIfGp_getDetect_chk_light(&eyePos)) {
        ret = true;
    } else {
        for (int i = 0; i < 5; i++) {
            dCcD_Tri* local_tri = &field_0x2C8[i];
            if (local_tri->ChkTgHit()) {
                u32 t = gabi::ea(local_tri);
                /* GetNP()->inprod(*GetTgRVecP()) */
                if (PSVECDotProduct(gabi::at<cXyz>(t + 0x118), gabi::at<cXyz>(t + 0xC0)) < 0.0f) {
                    ret = true;
                }
                /* ClrTgHit: virtual (hit-info vtable +0x3C) */
                gabi::call_ptr(gabi::load<u32>(local_tri->__vtbl_hitinf + 0x3C), local_tri);
            }
        }
    }
    return ret;
}
VERIFY(0x02375504, &Act_c::chk_light);

/* 023755D4 */
void Act_c::eff_start() {
    WWHD_FUNC(0x023755D4, void, this);
    dComIfGp_particle_set(dPa_name_ID_AK_SN_SASORIMIRROR00, &current.pos, &shape_angle, nullptr, 0xff,
                          (dPa_levelEcallBack*)&field_0xC04, -1, nullptr, nullptr, nullptr);
}
VERIFY(0x023755D4, &Act_c::eff_start);

/* 0237563C */
void Act_c::eff_stop() {
    WWHD_FUNC(0x0237563C, void, this);
    field_0xC04.end();
}
VERIFY(0x0237563C, &Act_c::eff_stop);

/* 02375644 */
bool Act_c::_execute() {
    WWHD_FUNC(0x02375644, bool, this);
    /* attention_info.position = (pos.x, pos.y + 260, pos.z); eyePos = attention_info.position */
    u32 x = gabi::load<u32>(THIS + 0x314);
    f32 y = current.pos.y + 260.0f;
    u32 z = gabi::load<u32>(THIS + 0x31C);
    gabi::store<u32>(THIS + 0x390, x);
    gabi::store<f32>(THIS + 0x394, y);
    gabi::store<u32>(THIS + 0x398, z);
    gabi::store<u32>(THIS + 0x37C, x);
    gabi::store<f32>(THIS + 0x380, y);
    gabi::store<u32>(THIS + 0x384, z);
    if (chk_light()) {
        cLib_chaseF(&field_0xBFC, 1.0f, 0.2f);
    } else {
        cLib_chaseF(&field_0xBFC, 0.0f, 0.2f);
    }
    u8 old_bf9 = field_0xBF9;
    field_0xBF9 = field_0xBFC > 0.999f;
    mMirrorBtkAnm.play();
    mBeamBtkAnm.play();
    if (field_0xBF8) {
        set_cc_rec_pos();
    }
    if (field_0xBF8 || field_0xBF9) {
        set_cc_trans_pos();
    }
    set_mtx();
    set_cull();
    if (field_0xBF9) {
        if (!old_bf9) {
            mDoAud_seStart(JA_SE_OBJ_MIRROR_REFLECT, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            eff_start();
        }
        mDoAud_seStart(JA_SE_OBJ_MIRROR_LIGHT, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    } else {
        if (old_bf9) {
            eff_stop();
        }
    }
    for (int i = 0; i < 5; i++) {
        dComIfG_Ccsp_Set(&field_0x2C8[i]);
    }
    if (field_0xBF9) {
        dComIfG_Ccsp_Set(&field_0xA84);
    }
    field_0xBF8 = false;
    return true;
}
VERIFY(0x02375644, &Act_c::_execute);

/* 02375860 */
bool Act_c::_draw() {
    WWHD_FUNC(0x02375860, bool, this);
    u8 bf9 = field_0xBF9;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpMirrorModel, &tevStr);
    mMirrorBtkAnm.entry(J3DModel_getModelData(mpMirrorModel), mMirrorBtkAnm.getFrame());
    if (bf9) {
        setLightTevColorType(dKy_getEnvlight(), mpBeamModel, &tevStr);
        mBeamBtkAnm.entry(J3DModel_getModelData(mpBeamModel), mBeamBtkAnm.getFrame());
    }
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpMirrorModel);
    dComIfGd_setList();
    if (bf9) {
        mDoExt_modelUpdateDL(mpBeamModel);
    }
    return true;
}
VERIFY(0x02375860, &Act_c::_draw);

/* 02375958 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x02375958, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x02375958, Mthd_Create);

/* 0237595C */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0237595C, BOOL, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x0237595C, Mthd_Delete);

/* 02375960 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x02375960, BOOL, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x02375960, Mthd_Execute);

/* 02375964 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x02375964, BOOL, i_this);
    return ((Act_c*)i_this)->_draw();
}
VERIFY(0x02375964, Mthd_Draw);

/* 02375968: header statics, then L_attr's triangle vertices (HD: initialised at run time) */
static void __sinit_d_a_obj_mmrr_cpp() {
    WWHD_FUNC(0x02375968, void, (u32)0);
    sinit_header_statics(0x1046A648, 0x101CB0D0);
    static const struct { u16 off; s16 v; } tbl[] = {
        {0x0C, -50}, {0x10, -230}, {0x18, 50},  {0x1C, -230}, {0x30, -60}, {0x34, -20}, {0x3C, -90},
        {0x40, -80}, {0x54, 90},   {0x58, -80}, {0x60, 60},   {0x64, -20}, {0x78, -90}, {0x7C, -80},
        {0x84, -50}, {0x88, -230}, {0x9C, 50},  {0xA0, -230}, {0xA8, 90},  {0xAC, -80},
    };
    for (const auto& e : tbl) gabi::store<f32>(L_ATTR + e.off, (f32)e.v);
}
VERIFY(0x02375968, __sinit_d_a_obj_mmrr_cpp);

/* 02375A9C: sead::SafeString deleting destructor (per-TU copy) */
static void safestring_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02375A9C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02375A9C, safestring_dt);

/* 02375AB0, 02375AB4: empty virtuals of Eff_c (dPa_followEcallBack overrides, per-TU copies) */
static void eff_empty_virtual_1(void* p) { WWHD_FUNC(0x02375AB0, void, p); }
VERIFY(0x02375AB0, eff_empty_virtual_1);
static void eff_empty_virtual_2(void* p) { WWHD_FUNC(0x02375AB4, void, p); }
VERIFY(0x02375AB4, eff_empty_virtual_2);

/* 02375AB8: dCcD_Stts::dCcD_Stts (per-TU copy, for __construct_array; allocates when this == NULL) */
static dCcD_Stts* dCcD_Stts_ctor(dCcD_Stts* p) {
    WWHD_FUNC(0x02375AB8, dCcD_Stts*, p);
    if (p == nullptr) {
        p = (dCcD_Stts*)operator_new(0x3C);
        if (p == nullptr)
            return p;
    }
    dCcD_Stts_ct(p);
    return p;
}
VERIFY(0x02375AB8, dCcD_Stts_ctor);

/* 02375B20: dCcD_Tri::dCcD_Tri (per-TU copy) */
static dCcD_Tri* dCcD_Tri_ctor(dCcD_Tri* p) {
    WWHD_FUNC(0x02375B20, dCcD_Tri*, p);
    if (p == nullptr) {
        p = (dCcD_Tri*)operator_new(0x150);
        if (p == nullptr)
            return p;
    }
    u32 t = gabi::ea(p);
    gabi::call(0x02515FB8, p); /* dCcD_GObjInf::dCcD_GObjInf */
    gabi::store<u32>(t + 0x114, 0x100015A8);
    gabi::store<u32>(t + 0x110, AAB_VTBL);
    gabi::call(0x02019040, t + 0x118); /* cM3dGTri::cM3dGTri */
    p->__vtbl_hitinf = 0x1004B010;
    gabi::store<u32>(t + 0x128, 0x1004B058);
    gabi::store<u32>(t + 0x114, 0x1004B068);
    return p;
}
VERIFY(0x02375B20, dCcD_Tri_ctor);

/* 02375BAC: an empty virtual of the SafeString vtable (per-TU copy) */
static void empty_virtual(void* p) { WWHD_FUNC(0x02375BAC, void, p); }
VERIFY(0x02375BAC, empty_virtual);

/* 02375BB0: Act_c deleting destructor (HD: Eff_c and the btk animations are not destroyed) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x02375BB0, void, i_this, flags);
    if (i_this != nullptr) {
        u32 a = gabi::ea(i_this);
        dCcD_Stts_dt(&i_this->field_0xBBC, 2);
        gabi::call(0x02515980, &i_this->field_0xA84, 2); /* dCcD_Cps dtor (dCcD_GObjInf::~dCcD_GObjInf) */
        destroy_arr(a + 0xB34, 5, 0x3C, 0x02515860);
        destroy_arr(a + 0x4A4, 5, 0x150, 0x025159F8);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02375BB0, Act_c_dt);

/* 02375C5C */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x02375C5C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02375C5C, Mthd_IsDelete);

} // namespace daObjMmrr
