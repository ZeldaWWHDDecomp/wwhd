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
#include "d/actor/d_a_obj_mmrr.h"

namespace daObjMmrr {

// HD: vertices are zero in .data, then filled by the original file initializer.
static MmrrAttributes L_attr = {
    {
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
    },
    445.0f, 0.0f, -2000, 0, {0.0f, 320.0f, 40.0f}, 120.0f, 10000.0f
};
static const MmrrTriangleSource M_tri_src = {0x0, 0x0, 0, 0, 0, 0, 0x0, 0x800000, 0x9, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
static const MmrrCapsuleCollisionSource M_cps_src = {0x0, 0x800000, 0, 0, 0, 0, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 50.0f}};

static const char M_arcname[] = "Mmirror";
enum {
    SAFESTRING_VTBL = 0x1002CC80,
    ACT_VTBL = 0x1002CD48,
    EFF_VTBL = 0x1002CEA8,
    AAB_VTBL = 0x1002CC98,
    JA_SE_OBJ_MIRROR_REFLECT = 0x6961,
    JA_SE_OBJ_MIRROR_LIGHT = 0x7028,
    dPa_name_ID_AK_SN_SASORIMIRROR00 = 0x8294,
    dRes_INDEX_MMIRROR_BDL_MMRR_e = 9,
    dRes_INDEX_MMIRROR_BDL_YSSMR00_e = 11,
    dRes_INDEX_MMIRROR_BTK_MMRR_e = 14,
    dRes_INDEX_MMIRROR_BTK_YSSMR00_e = 16,
};

/* 02374AFC */
void Eff_c::end() {
    if (mpEmitter) {
        reinterpret_cast<MmrrEmitter*>(getEmitter())->mGlobalAlpha = 0; /* setGlobalAlpha(0) */
    }
    dPa_followEcallBack::end(); /* dPa_followEcallBack::end() */
}

/* 02374B14 */
void Eff_c::remove() {
    if (mpEmitter) {
        reinterpret_cast<MmrrEmitter*>(getEmitter())->mGlobalAlpha = 0;
    }
    dPa_followEcallBack::remove(); /* this->end(): virtual */
}

/* 02374D28 */
static u8 solidHeapCB(fopAc_ac_c* i_actor) { /* tail call: create_heap's u8 result as is */
    return Mmrr_createHeapResult(static_cast<Act_c*>(i_actor)); /* ((Act_c*)i_actor)->create_heap() */
}

/* 02374B38 */
bool Act_c::create_heap() {
    J3DModelData* bdl_Mmrr = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_BDL_MMRR_e, SAFESTRING_VTBL);
    if (bdl_Mmrr == nullptr)
        JUT_ASSERT_fail("d_a_obj_mmrr.cpp", 0x1e8, "bdl_Mmrr != 0");
    mpMirrorModel = mDoExt_J3DModel__create(bdl_Mmrr, 0x80000, 0x11000222);

    J3DModelData* bdl_Yssmr00 = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_BDL_YSSMR00_e, SAFESTRING_VTBL);
    if (bdl_Yssmr00 == nullptr)
        JUT_ASSERT_fail("d_a_obj_mmrr.cpp", 0x1f1, "bdl_Yssmr00 != 0");
    mpBeamModel = mDoExt_J3DModel__create(bdl_Yssmr00, 0x80000, 0x11000222);

    J3DAnmTextureSRTKey* btk_Mmrr = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_BTK_MMRR_e, SAFESTRING_VTBL);
    if (btk_Mmrr == nullptr)
        JUT_ASSERT_fail("d_a_obj_mmrr.cpp", 0x1fc, "btk_Mmrr != 0");
    BOOL mirror_anm_init_res = mMirrorBtkAnm.init(bdl_Mmrr, btk_Mmrr, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);

    J3DAnmTextureSRTKey* btk_Yssmr00 = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_BTK_YSSMR00_e, SAFESTRING_VTBL);
    if (btk_Yssmr00 == nullptr)
        JUT_ASSERT_fail("d_a_obj_mmrr.cpp", 0x203, "btk_Yssmr00 != 0");
    BOOL beam_anm_init_res = mBeamBtkAnm.init(bdl_Yssmr00, btk_Yssmr00, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);

    return mpMirrorModel && mpBeamModel && mirror_anm_init_res && beam_anm_init_res;
}

/* 023751B0 */
void Act_c::init_cc() {
    for (int i = 0; i < 5; i++) {
        field_0x958[i].Init(0xff, 0xff, this);
        Mmrr_setTriSource(&field_0x2C8[i], &M_tri_src); /* dCcD_Tri::Set */
        field_0x2C8[i].SetStts(&field_0x958[i]);
        field_0x2C8[i].mGObjTg.mSPrm = field_0x2C8[i].mGObjTg.mSPrm | 4; /* OnTgNoHitMark */
        field_0xBBC.Init(0xff, 0xff, this);
        Mmrr_setCpsSource(&field_0xA84, &M_cps_src); /* dCcD_Cps::Set */
        field_0xA84.SetStts(&field_0xBBC);
    }
    set_cc_rec_pos();
    set_cc_trans_pos();
}

/* 02374E98 */
void Act_c::set_cc_rec_pos() {
    cXyz temp;
    cXyz a;
    cXyz b;
    cXyz c;

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, L_attr.field_0xB4, L_attr.field_0xB8);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), L_attr.field_0xBC);

    cXyz* out[3] = {&a, &b, &c};
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            auto& from = positionWords(L_attr.vertex(i, j));
            auto& to = positionWords(temp);
            to.x = (u32)from.x;
            to.y = (u32)from.y;
            to.z = (u32)from.z;
            PSMTXMultVec(mDoMtx_stack_c::get(), &temp, out[j]);
        }
        Mmrr_setTri(&triangleShape(&field_0x2C8[i]).mTri, &a, &b, &c);
    }
}

/* 02374FC0 */
void Act_c::set_cc_trans_pos() {
    cXyz end;
    cXyz tmp;
    MmrrCapsuleSource cps;
    MmrrLightLinChk chk;

    end.y = 0.0f;
    end.x = 0.0f;
    end.z = L_attr.field_0xD0;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(L_attr.field_0xC0[0], L_attr.field_0xC0[1], L_attr.field_0xC0[2]);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &cps.mStart);
    PSMTXMultVec(mDoMtx_stack_c::get(), &end, &cps.mEnd);
    cps.mRadius = L_attr.radius;

    /* dBgS_MirLightLinChk light_lin_chk (inline constructor) */
    cBgS_LinChk_ct(&chk);
    chk.mpPolyPassChk = gabi::ea(&chk.__vtbl_58);
    for (int i = 0; i < 7; i++) chk.mPass[i] = 0;
    chk.mPass[3] = 1; /* light */
    chk.mpGrpPassChk = gabi::ea(&chk.__vtbl_64);
    chk.mGrp = 0x1F;
    chk.__vtbl_64 = 0x1002CD28;
    chk.__vtbl_10 = 0x1002CD08;
    chk.__vtbl_58 = 0x1002CD38;
    chk.__vtbl_20 = 0x1002CD18;

    dBgS_LinChk_Set(&chk, &cps.mStart, &cps.mEnd, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &chk)) {
        auto& endWords = positionWords(cps.mEnd);
        auto& crossWords = positionWords(chk.mCross);
        endWords.x = (u32)crossWords.x;
        endWords.z = (u32)crossWords.z;
        endWords.y = (u32)crossWords.y;
    }
    Mmrr_setCapsule(&capsuleShape(&field_0xA84).mCps, &cps);
    Mmrr_subtract(&capsuleShape(&field_0xA84).mCps.mEnd,
                  &capsuleShape(&field_0xA84).mCps.mStart, &field_0xA84.mGObjAt.mVec);
    Mmrr_normalizeRS(&field_0xA84.mGObjAt.mVec);
    /* cXyz(cps.mStart).abs(cps.mEnd) */
    f32 sx = cps.mStart.x, sy = cps.mStart.y, sz = cps.mStart.z;
    tmp.z = sz;
    tmp.y = sy;
    tmp.x = sx;
    f32 d = std_sqrtf(Mmrr_squareDistance(&tmp, &cps.mEnd)) / L_attr.field_0xD0;
    /* ~dBgS_MirLightLinChk */
    chk.__vtbl_20 = 0x1002CCA8;
    chk.__vtbl_58 = 0x1002CCF8;
    chk.__vtbl_64 = 0x1002CCB8;
    field_0xC00 = d;
    cBgS_LinChk_dt(&chk, 0);
}

/* 02375264 */
void Act_c::set_cull() {
    f32 y1, z1;
    if (field_0xBF9) {
        u16 ang = (u16)L_attr.field_0xBC;
        f32 d0 = L_attr.field_0xD0;
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

/* 02375300 */
cPhs_State Act_c::_create() {
    cPhs_State state;

    /* fopAcM_ct(this, Act_c): inline constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            mDoExt_btkAnm::ct(&mMirrorBtkAnm);
            mDoExt_btkAnm::ct(&mBeamBtkAnm);
            Mmrr_constructArray(field_0x2C8, 5, sizeof(dCcD_Tri), Mmrr_triangleConstructor);
            Mmrr_constructArray(field_0x958, 5, sizeof(dCcD_Stts), Mmrr_statusConstructor);
            /* dCcD_Cps */
            Mmrr_constructGObj(&field_0xA84);
            auto& capsule = capsuleShape(&field_0xA84);
            capsule.mShapeVtable = 0x100015A8;
            capsule.mAabVtable = AAB_VTBL;
            Mmrr_constructCps(&capsule.mCps);
            field_0xA84.__vtbl_hitinf = 0x1004AF18;
            capsule.mCps.__vtbl = 0x1004AF60;
            capsule.mShapeVtable = 0x1004AF70;
            dCcD_Stts_ct(&field_0xBBC);
            dPa_followEcallBack_ct(&field_0xC04, 0, 0);
            field_0xC04.__vtbl = EFF_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    state = dComIfG_resLoad(&mPhs, M_arcname);
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, Mmrr_solidHeapCallback, 0x1a80)) {
            J3DModel* m = mpMirrorModel;
            cullMtx = gabi::ea(m->getBaseTRMtx());
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

/* 023754C4 */
bool Act_c::_delete() {
    eff_remove();
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 02374D2C */
void Act_c::set_mtx() {
    /* local_scale(scale.x, scale.y, scale.z * field_0xC00); mpBeamModel->setBaseScale(local_scale) */
    f32 z = scale.z * field_0xC00;
    auto& beamScale = positionWords(mpBeamModel->mBaseScale);
    auto& actorScale = positionWords(scale);
    beamScale.x = (u32)actorScale.x;
    beamScale.y = (u32)actorScale.y;
    mpBeamModel->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpMirrorModel, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpBeamModel, mDoMtx_stack_c::get());
}

/* 02374E78 */
void Act_c::init_mtx() {
    auto& mirrorScale = positionWords(mpMirrorModel->mBaseScale);
    auto& actorScale = positionWords(scale);
    mirrorScale.x = (u32)actorScale.x;
    mirrorScale.y = (u32)actorScale.y;
    mirrorScale.z = (u32)actorScale.z;
    set_mtx();
}

/* 02375504 */
bool Act_c::chk_light() {
    bool ret = false;
    if (Mmrr_detectLight(Mmrr_getDetect(), &eyePos)) {
        ret = true;
    } else {
        for (int i = 0; i < 5; i++) {
            dCcD_Tri* local_tri = &field_0x2C8[i];
            if (local_tri->ChkTgHit()) {
                /* GetNP()->inprod(*GetTgRVecP()) */
                if (PSVECDotProduct(&triangleShape(local_tri).mTri.mNormal, &local_tri->mGObjTg.mRVec) < 0.0f) {
                    ret = true;
                }
                /* ClrTgHit: virtual (hit-info vtable +0x3C) */
                Mmrr_clearTargetHit(local_tri);
            }
        }
    }
    return ret;
}

/* 023755D4 */
void Act_c::eff_start() {
    dComIfGp_particle_set(dPa_name_ID_AK_SN_SASORIMIRROR00, &current.pos, &shape_angle, nullptr, 0xff,
                          (dPa_levelEcallBack*)&field_0xC04, -1, nullptr, nullptr, nullptr);
}

/* 0237563C */
void Act_c::eff_stop() {
    field_0xC04.end();
}

/* 023754BC */
void Act_c::eff_remove() {
    field_0xC04.remove();
}

/* 02375644 */
bool Act_c::_execute() {
    /* attention_info.position = (pos.x, pos.y + 260, pos.z); eyePos = attention_info.position */
    auto& posWords = positionWords(current.pos);
    u32 x = posWords.x;
    f32 y = current.pos.y + 260.0f;
    u32 z = posWords.z;
    auto& attentionWords = positionWords(attention_info.position);
    attentionWords.x = x;
    attention_info.position.y = y;
    attentionWords.z = z;
    auto& eyeWords = positionWords(eyePos);
    eyeWords.x = x;
    eyePos.y = y;
    eyeWords.z = z;
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

/* 02375860 */
bool Act_c::_draw() {
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

/* 02375958 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}

/* 0237595C */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}

/* 02375960 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}

/* 02375964 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->_draw();
}

/* 02375C5C */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* 02375968 */
// HD: header statics, then L_attr's triangle vertices (HD: initialised at run time)
static void __sinit_d_a_obj_mmrr_cpp() {
    sinit_header_statics(0x1046A648, 0x101CB0D0);
    L_attr.vertex(0, 1).x = -50.0f;
    L_attr.vertex(0, 1).y = -230.0f;
    L_attr.vertex(0, 2).x = 50.0f;
    L_attr.vertex(0, 2).y = -230.0f;
    L_attr.vertex(1, 1).x = -60.0f;
    L_attr.vertex(1, 1).y = -20.0f;
    L_attr.vertex(1, 2).x = -90.0f;
    L_attr.vertex(1, 2).y = -80.0f;
    L_attr.vertex(2, 1).x = 90.0f;
    L_attr.vertex(2, 1).y = -80.0f;
    L_attr.vertex(2, 2).x = 60.0f;
    L_attr.vertex(2, 2).y = -20.0f;
    L_attr.vertex(3, 1).x = -90.0f;
    L_attr.vertex(3, 1).y = -80.0f;
    L_attr.vertex(3, 2).x = -50.0f;
    L_attr.vertex(3, 2).y = -230.0f;
    L_attr.vertex(4, 1).x = 50.0f;
    L_attr.vertex(4, 1).y = -230.0f;
    L_attr.vertex(4, 2).x = 90.0f;
    L_attr.vertex(4, 2).y = -80.0f;

}

/* 02375A9C */
// HD: sead::SafeString deleting destructor (per-TU copy)
static void safestring_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02375AB0 */
static void eff_empty_virtual_1(void* p) {}
/* 02375AB4 */
static void eff_empty_virtual_2(void* p) {}

/* 02375AB8 */
// HD: dCcD_Stts::dCcD_Stts (per-TU copy, for __construct_array; allocates when this == NULL)
static dCcD_Stts* dCcD_Stts_ctor(dCcD_Stts* p) {
    if (p == nullptr) {
        p = (dCcD_Stts*)operator_new(0x3C);
        if (p == nullptr)
            return p;
    }
    dCcD_Stts_ct(p);
    return p;
}

/* 02375B20 */
// HD: dCcD_Tri::dCcD_Tri (per-TU copy)
static dCcD_Tri* dCcD_Tri_ctor(dCcD_Tri* p) {
    if (p == nullptr) {
        p = (dCcD_Tri*)operator_new(0x150);
        if (p == nullptr)
            return p;
    }
    Mmrr_constructGObj(p);
    auto& triangle = triangleShape(p);
    triangle.mShapeVtable = 0x100015A8;
    triangle.mAabVtable = AAB_VTBL;
    Mmrr_constructTri(&triangle.mTri);
    p->__vtbl_hitinf = 0x1004B010;
    triangle.mTri.__vtbl = 0x1004B058;
    triangle.mShapeVtable = 0x1004B068;
    return p;
}

/* 02375BAC */
// HD: an empty virtual of the SafeString vtable (per-TU copy)
static void empty_virtual(void* p) {}

/* 02375BB0 */
// HD: Act_c deleting destructor (HD: Eff_c and the btk animations are not destroyed)
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Stts_dt(&i_this->field_0xBBC, 2);
        Mmrr_destroyCps(&i_this->field_0xA84, 2); /* dCcD_Cps dtor (dCcD_GObjInf::~dCcD_GObjInf) */
        Mmrr_destroyArray(i_this->field_0x958, 5, sizeof(dCcD_Stts), Mmrr_statusDestructor, 0, 0);
        Mmrr_destroyArray(i_this->field_0x2C8, 5, sizeof(dCcD_Tri), Mmrr_triangleDestructor, 0, 0);
        Mmrr_destroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

} // namespace daObjMmrr

// HD: expose the original namespace names to the generated registration block.
using namespace daObjMmrr;

/* WWHD glue (tools/verify/autoglue.py)
data L_attr 101CAFFC
data M_tri_src 1002CE08
data M_cps_src 1002CE5C
str 1002CE00 "Mmirror"
str 1002CD78 "d_a_obj_mmrr.cpp"
str 1002CD58 "bdl_Mmrr != 0"
str 1002CD8C "bdl_Yssmr00 != 0"
str 1002CD68 "btk_Mmrr != 0"
str 1002CDA0 "btk_Yssmr00 != 0"
ext 028EFFD0 void Mmrr_constructArray(void*, s32, u32, u32)
ext 028F0164 void Mmrr_destroyArray(void*, s32, u32, u32, s32, s32)
ext 0252A038 BOOL Mmrr_detectLight(void*, cXyz*)
ext 0201924C void Mmrr_setTri(cM3dGTri*, cXyz*, cXyz*, cXyz*)
ext 020181FC void Mmrr_setCapsule(MmrrCapsuleGeometry*, MmrrCapsuleSource*)
ext 028E8DAC void Mmrr_subtract(const cXyz*, const cXyz*, cXyz*)
ext 028E8DE8 f32 Mmrr_squareDistance(const cXyz*, const cXyz*)
ext 0201B47C void Mmrr_normalizeRS(cXyz*)
ext 0251650C void Mmrr_setTriSource(dCcD_Tri*, const MmrrTriangleSource*)
ext 025164C0 void Mmrr_setCpsSource(dCcD_Cps*, const MmrrCapsuleCollisionSource*)
ext 02515FB8 void Mmrr_constructGObj(dCcD_GObjInf*)
ext 02018150 void Mmrr_constructCps(MmrrCapsuleGeometry*)
ext 02019040 void Mmrr_constructTri(cM3dGTri*)
ext 02515980 void Mmrr_destroyCps(dCcD_Cps*, s32)
ext 025D50BC void Mmrr_destroyActor(fopAc_ac_c*, s32)
ext 02374B38 u8 Mmrr_createHeapResult(daObjMmrr::Act_c*)
*/
