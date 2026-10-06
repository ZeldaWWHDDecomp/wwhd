/**
 * d_a_tsubo.cpp (WWHD)
 * Small, carriable objects (pots, skulls, barrels, ...): creation, specials (Bokoblin sticks),
 * mode inits, matrices, destruction.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tsubo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Damage/position/bound functions: d_a_tsubo_dmg.cpp; modes, tensors, effects, execute/draw:
 * d_a_tsubo_mode.cpp.
 */
#include "d/actor/d_a_tsubo.h"

namespace daTsubo {

enum { fpcNm_BOKO_e = 0x1CF };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D54C4 fopAcM_SearchByID(fpc_ProcID, fopAc_ac_c** out) -> found */
static inline BOOL fopAcM_SearchByID2(u32 id, gptr<fopAc_ac_c>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* 025D9D0C fopAcM_setCarryNow(actor, BOOL stageLayer) */
static inline void fopAcM_setCarryNow(fopAc_ac_c* a, s32 stageLayer) { gabi::call(0x025D9D0C, a, stageLayer); }
/* 025D6768 fopAcM_setCullSizeSphere(actor, x, y, z, r) */
static inline void fopAcM_setCullSizeSphere(fopAc_ac_c* a, f32 x, f32 y, f32 z, f32 r) { gabi::call(0x025D6768, a, x, y, z, r); }
/* 028E9B14 C_QUATRotAxisRad(Quaternion* q, const Vec* axis, f32 rad); the quaternion stack's
 * current entry is *(0x1048D3FC) (mDoMtx_quatStack_c::now) */
static inline void C_QUATRotAxisRad(u32 q, const cXyz* axis, f32 rad) { gabi::call(0x028E9B14, q, axis, rad); }
static inline u32 mDoMtx_quatStack_get() { return gabi::load<u32>(0x1048D3FC); }
/* 028E8C78 PSMTXQuat(Mtx, const Quaternion*), 028E9098 PSMTXIdentity, 028E9108 PSMTXConcat */
static inline void PSMTXQuat(Mtx34* m, u32 q) { gabi::call(0x028E8C78, m, q); }
static inline void PSMTXIdentity(Mtx34* m) { gabi::call(0x028E9098, m); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
/* 024EEB2C dBgS::ChkMoveBG_NoDABg(cBgS_PolyInfo&) */
static inline BOOL dBgS_ChkMoveBG_NoDABg(dBgS* bgs, cBgS_PolyInfo* p) { return gabi::call<BOOL>(0x024EEB2C, bgs, p); }
/* 0255F458 dKy_Sound_set(cXyz pos (by value: pointer to a copy), int, fpc_ProcID, int) */
static inline void dKy_Sound_set(cXyz* pos, s32 a, u32 id, s32 b) { gabi::call(0x0255F458, pos, a, id, b); }
/* the stage info: play+0x5150 (dStage_stageDt_c), virtual getStagInfo at vtable +0x15C; the
 * cull point is the u16 at +0x12 of the stage info */
static inline u32 dComIfGp_getStageStagInfo() {
    u32 dt = dComIfGp_ea() + 0x5150;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(dt) + 0x15C), dt);
}
/* 027DF9B0 JUTNameTab::getIndex(const char*), 027F591C J3DModel material op (HD; arguments
 * model, material, 1) */
static inline s32 JUTNameTab_getIndex(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* mDoExt_brkAnm: 025E80D0 constructor (matcher: "init"), 025E8154 init(data, key, anmPlay,
 * attr, rate, start, end, modify, entry) */
static inline BOOL brkAnm_init(void* a, J3DModelData* d, void* key, s32 anmPlay, s32 attr, f32 rate, s16 start, s16 end, bool modify,
                               s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, key, anmPlay, attr, start, end, modify, entry, rate);
}

/* 024CA194 / 024CA1A0: the constructor helpers of the followEcallBack arrays (__construct_array) */
static void followEcallBack_ct_a(dPa_followEcallBack* p) {
    WWHD_FUNC(0x024CA194, void, p);
    dPa_followEcallBack_ct(p, 0, 0);
}
VERIFY(0x024CA194, followEcallBack_ct_a);
static void followEcallBack_ct_b(dPa_followEcallBack* p) {
    WWHD_FUNC(0x024CA1A0, void, p);
    dPa_followEcallBack_ct(p, 0, 0);
}
VERIFY(0x024CA1A0, followEcallBack_ct_b);

/* 024CA1AC */
static void prmZ_init(Act_c* i_this) {
    WWHD_FUNC(0x024CA1AC, void, i_this);
    if (i_this->m67E != 0) {
        return;
    }
    i_this->mPrmZ = i_this->home.angle.z;
    i_this->m67E = 1;
    i_this->home.angle.z = 0;
    i_this->current.angle.z = 0;
    i_this->shape_angle.z = 0;
}
VERIFY(0x024CA1AC, prmZ_init);

/* 024CA1DC: Act_c::Act_c (fopAcM_ct; out of line in HD, allocates 0x93C when this == NULL) */
static Act_c* Act_c_ct(Act_c* i_this) {
    WWHD_FUNC(0x024CA1DC, Act_c*, i_this);
    if (i_this == nullptr) {
        i_this = (Act_c*)operator_new(0x93C);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = TSUBO_ACT_VTBL;
    dBgS_ObjAcch_ct(&i_this->mAcch, TSUBO_OBJACCH_VT);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    {
        /* dBgS_ObjGndChk_Yogan: mPass[0] = 1 (obj), mGrp 4 (lava) */
        u32 b = gabi::ea(&i_this->mGndChkYogan);
        gabi::call(0x02008E0C, b); /* cBgS_GndChk::cBgS_GndChk */
        gabi::store<u8>(b + 0x44, 1);
        for (int i = 1; i < 7; i++) gabi::store<u8>(b + 0x44 + i, 0);
        gabi::store<u32>(b + 0x0, b + 0x40);
        gabi::store<u32>(b + 0x20, TSUBO_YOGAN_VT.v20);
        gabi::store<u32>(b + 0x10, TSUBO_YOGAN_VT.v10);
        gabi::store<u32>(b + 0x4C, TSUBO_YOGAN_VT.v4C);
        gabi::store<u32>(b + 0x40, TSUBO_YOGAN_VT.v40);
        gabi::store<u32>(b + 0x4, b + 0x4C);
        gabi::store<u32>(b + 0x50, 4);
    }
    dCcD_Cyl_ct(&i_this->mCyl, TSUBO_CYL_AAB_VTBL);
    dCcD_Stts_ct(&i_this->mStts);
    cSAngle_ct(&i_this->m688);
    cSAngle_ct(&i_this->m68A);
    cSAngle_ct(&i_this->m68C);
    cSAngle_ct(&i_this->m68E);
    cSAngle_ct(&i_this->m690);
    cSAngle_ct(&i_this->m692);
    gabi::call(0x028EFFD0, i_this->m70C, 3, 0x14, 0x024CA194); /* __construct_array */
    gabi::call(0x028EFFD0, i_this->m748, 3, 0x14, 0x024CA1A0);
    dPa_followEcallBack_ct(&i_this->m784, 0, 0);
    return i_this;
}
VERIFY(0x024CA1DC, Act_c_ct);

/* 024CA3D0 */
static bool create_heap(Act_c* i_this) {
    WWHD_FUNC(0x024CA3D0, bool, i_this);
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(M_arcname(i_this->mType), data(i_this)->m6C, TSUBO_SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1881, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10040C38), 0x759, STR(0x10040C58));

    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    i_this->mpModel = model;
    if (model != nullptr && i_this->mType == 7) {
        /* HD: the material named 0x10040C30 gets 027F591C(model, material, 1) */
        gabi::Local<SafeString> name;
        name->mStringTop = 0x10040C30;
        name->__vtbl = TSUBO_SAFESTRING_VTBL;
        u32 md = gabi::load<u32>(gabi::ea(model) + 0xAC); /* J3DModelData */
        u32 matTbl = gabi::load<u32>(md);                 /* its material table */
        gabi::call(0x024D0A88, name.get());               /* (empty per-TU SafeString helper) */
        u32 off = gabi::load<u32>(matTbl + 0x18);
        u32 tab = off != 0 ? matTbl + 0x18 + off : 0;
        s32 idx = JUTNameTab_getIndex(tab, name->mStringTop);
        u32 mat;
        if (idx < 0) {
            mat = 0;
        } else {
            u32 num = gabi::load<u32>(md + 0xC);
            mat = gabi::load<u32>(md + 0x10);
            if ((u32)idx < num) {
                mat += idx * 0x39C;
            }
        }
        gabi::call(0x027F591C, (J3DModel*)i_this->mpModel, mat, 1);
    }

    bool ret = false;
    if (i_this->mType == 7) {
        void* brk_data = dComIfG_getObjectRes(M_arcname(i_this->mType), 7 /* dRes_INDEX_KMI00X_BRK_KMI_00X_e */, TSUBO_SAFESTRING_VTBL);
        if (brk_data == nullptr) /* JUT_ASSERT(1900, brk_data != NULL) */
            JUT_ASSERT_fail(STR(0x10040C38), 0x76C, STR(0x10040C48));
        void* brk = operator_new(0x78);
        if (brk != nullptr) {
            brk = gabi::call<void*>(0x025E80D0, brk); /* mDoExt_brkAnm::mDoExt_brkAnm */
        }
        i_this->mpBrk = (mDoExt_brkAnm*)brk;
        if (brk != nullptr) {
            if (brkAnm_init(brk, modelData, brk_data, false, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
                ret = true;
            }
        }
    } else {
        ret = true;
        i_this->mpBrk = nullptr;
    }
    return i_this->mpModel != nullptr && ret;
}
VERIFY(0x024CA3D0, create_heap);

/* 024CA5EC solidHeapCB */
static u8 solidHeapCB(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x024CA5EC, u8, a_this);
    return gabi::call<u8>(0x024CA3D0, a_this); /* tail call: create_heap's bool (byte passed through) */
}
VERIFY(0x024CA5EC, solidHeapCB);

/* 024CA5F0 */
static void cull_set_draw(Act_c* i_this) {
    WWHD_FUNC(0x024CA5F0, void, i_this);
    Act_c::Data_c* d = data(i_this);
    fopAcM_setCullSizeSphere(i_this, d->mCullSphX_Draw, d->mCullSphY_Draw, d->mCullSphZ_Draw, d->mCullSphR_Draw);
}
VERIFY(0x024CA5F0, cull_set_draw);

/* 024CA690 */
static void create_init_cull(Act_c* i_this) {
    WWHD_FUNC(0x024CA690, void, i_this);
    i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
    cull_set_draw(i_this);
    /* HD: strcmp as a sead::SafeString comparison */
    if (sead_streq(0x10040C78 /* "sea" */, [] { return dComIfGp_ea() + 0x5134; })) {
        u32 info = dComIfGp_getStageStagInfo();
        f32 fVar1 = (f32)gabi::load<u16>(info + 0x12); /* dStage_stagInfo_GetCullPoint */
        if (fVar1 > 1.0f) {
            i_this->cullSizeFar = data(i_this)->mB0 / fVar1;
        }
    }
}
VERIFY(0x024CA690, create_init_cull);

/* 024CA804 */
static void create_init_cc(Act_c* i_this) {
    WWHD_FUNC(0x024CA804, void, i_this);
    i_this->mStts.Init(data(i_this)->m10, 0xff, i_this);
    i_this->mCyl.Set(M_cyl_src);
    i_this->mCyl.SetStts(&i_this->mStts);
    i_this->mCyl.SetR((f32)data(i_this)->mAcchCirRad);
    i_this->mCyl.SetH((f32)data(i_this)->m70);
    i_this->mCyl.mGObjAt.mVec.copy(*cXyz_Zero); /* SetAtVec */
    i_this->mCyl.mGObjTg.mVec.copy(*cXyz_Zero); /* SetTgVec */
    /* OnTgShield (GObjTg SPrm bit 1) / OnTgNoHitMark (bit 4) */
    u32 bit = 4;
    if (data(i_this)->mFlag & Act_c::DATA_FLAG_4_e) {
        bit = 1;
    }
    i_this->mCyl.mGObjTg.mSPrm |= bit;
}
VERIFY(0x024CA804, create_init_cc);

/* 024CA948 */
static void create_init_bgc(Act_c* i_this) {
    WWHD_FUNC(0x024CA948, void, i_this);
    i_this->mAcchCir.SetWall(30.0f, (f32)data(i_this)->mAcchCirRad);
    i_this->mAcch.Set(&i_this->current.pos, &i_this->old.pos, i_this, 1, &i_this->mAcchCir, &i_this->speed, &i_this->current.angle,
                      &i_this->shape_angle);
    Acch_ClrWaterNone(&i_this->mAcch);
    Acch_ClrRoofNone(&i_this->mAcch);
    i_this->mAcch.SetRoofCrrHeight((f32)data(i_this)->mAcchRoofHeight);
    i_this->m4F8 = -1000000000.0f; /* -G_CM3D_F_INF */
    i_this->m4FC = -1000000000.0f;
    i_this->m500 = -1000000000.0f;
    i_this->m504 = 0;
    i_this->m505 = 0;
}
VERIFY(0x024CA948, create_init_bgc);

/* 024CAA54 */
static void init_rot_clean(Act_c* i_this) {
    WWHD_FUNC(0x024CAA54, void, i_this);
    cSAngle_Val(&i_this->m688, cSAngle__0);
    cSAngle_Val(&i_this->m68A, cSAngle__0);
    cSAngle_Val(&i_this->m68C, cSAngle__0);
    cSAngle_Val(&i_this->m68E, cSAngle__0);
    cSAngle_Val(&i_this->m690, cSAngle__0);
    cSAngle_Val(&i_this->m692, (s16)i_this->current.angle.y);
}
VERIFY(0x024CAA54, init_rot_clean);

/* 024CAAD0 */
static void mode_hide_init(Act_c* i_this) {
    WWHD_FUNC(0x024CAAD0, void, i_this);
    i_this->mCyl.OffAtSPrmBit(1);
    i_this->mCyl.OffTgSPrmBit(1);
    i_this->mCyl.OffCoSPrmBit(1);
    i_this->mAcch.SetRoofNone();
    i_this->mAcch.SetWallNone();
    i_this->mAcch.SetGrndNone();
    i_this->mAcch.SetWaterNone();
    i_this->mAcch.OffLineCheck();
    i_this->m67F = 1;
    fopAcM_SetSpeedF(i_this, 0.0f);
    attn_offBit(i_this, fopAc_Attn_ACTION_CARRY_e);
    i_this->m678 = 0;
}
VERIFY(0x024CAAD0, mode_hide_init);

/* 024CAB30; HD: the constant M_attrSpine members are immediates */
static void mode_appear_init(Act_c* i_this) {
    WWHD_FUNC(0x024CAB30, void, i_this);
    i_this->mCyl.OffAtSPrmBit(1);
    i_this->mCyl.OffTgSPrmBit(1);
    i_this->mCyl.OnCoSPrmBit(1);
    i_this->mAcch.SetRoofNone();
    i_this->mAcch.SetWallNone();
    i_this->mAcch.ClrGrndNone();
    i_this->mAcch.SetWaterNone();
    i_this->mAcch.OffLineCheck();
    i_this->m67F = 1;
    fopAcM_SetSpeedF(i_this, 0.0f);
    i_this->scale.set(0.01f, 0.01f, 0.01f);
    attn_offBit(i_this, fopAc_Attn_ACTION_CARRY_e);

    if (prm_get_spec(i_this) == 4) {
        i_this->m808 = 0.2f; /* M_attrSpine.m10 */
        i_this->m80C = 0.2f; /* M_attrSpine.m14 */
        i_this->m802 = 0x50; /* M_attrSpine.m18 */
    } else {
        i_this->speed.y = cM_rndF(5.0f) + 29.0f;
        i_this->gravity = data(i_this)->mGravity;
        i_this->m802 = 0xb;
    }
    i_this->m678 = 1;
}
VERIFY(0x024CAB30, mode_appear_init);

/* 024CAC44 */
static void mode_wait_init(Act_c* i_this) {
    WWHD_FUNC(0x024CAC44, void, i_this);
    i_this->mCyl.OffAtSPrmBit(1);
    i_this->mCyl.OnTgSPrmBit(1);
    i_this->mCyl.OnCoSPrmBit(1);
    Acch_ClrRoofNone(&i_this->mAcch);
    Acch_ClrWallNone(&i_this->mAcch);
    i_this->mAcch.ClrGrndNone();
    Acch_ClrWaterNone(&i_this->mAcch);
    i_this->mAcch.OffLineCheck();
    i_this->m67F = 0;
    fopAcM_SetSpeedF(i_this, 0.0f);
    i_this->mStts.Init(data(i_this)->m10, 0xff, i_this);
    i_this->m802 = 0;
    i_this->m678 = 2;
}
VERIFY(0x024CAC44, mode_wait_init);

/* 024CACEC */
static void eff_kutani_init(Act_c* i_this) {
    WWHD_FUNC(0x024CACEC, void, i_this);
    i_this->m798 = (u8)(gabi::ftoi(cM_rndF(30.0f)) + 1);
}
VERIFY(0x024CACEC, eff_kutani_init);

/* 024CAD38 */
static void spec_make_boko(Act_c* i_this, int arg1) {
    WWHD_FUNC(0x024CAD38, void, i_this, arg1);
    Act_c::M7A0* ptr = &i_this->m7A0[0];
    for (s32 i = 0; i < arg1; i++, ptr++) {
        ptr->m00 = fopAcM_create(fpcNm_BOKO_e, 0 /* daBoko_c::Type_BOKO_STICK_e */, &i_this->current.pos,
                                 i_this->home.roomNo, nullptr, nullptr, -1, 0);
        s16 a = M_data_spec_boko(i)->m00;
        ptr->m06 = a;
        ptr->m08 = a;
        ptr->m10 = M_data_spec_boko(i)->m04;
    }
}
VERIFY(0x024CAD38, spec_make_boko);

/* 024CADD8 spec_set_actor (matcher: unnamed) */
static void spec_set_actor(Act_c* i_this) {
    WWHD_FUNC(0x024CADD8, void, i_this);
    s32 spec = prm_get_spec(i_this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &i_this->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m00 == fpcM_ERROR_PROCESS_ID_e) {
                M_spec_act()[i] = nullptr;
            } else if (!fopAcM_SearchByID2(ptr->m00, &M_spec_act()[i])) {
                ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
            }
        }
    }
}
VERIFY(0x024CADD8, spec_set_actor);

/* 024CAE78 spec_carry_spec (matcher: unnamed) */
static void spec_carry_spec(Act_c* i_this) {
    WWHD_FUNC(0x024CAE78, void, i_this);
    s32 spec = prm_get_spec(i_this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &i_this->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 == 0 && M_spec_act()[i] != nullptr) {
                fopAcM_setCarryNow(M_spec_act()[i], 0);
                ptr->m04 = 1;
            }
        }
    }
}
VERIFY(0x024CAE78, spec_carry_spec);

/* 024CAF20 spec_clr_actor (matcher: unnamed) */
static void spec_clr_actor(Act_c* i_this) {
    WWHD_FUNC(0x024CAF20, void, i_this);
    s32 spec = prm_get_spec(i_this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        for (s32 i = 0; i < 3; i++) {
            M_spec_act()[i] = nullptr;
        }
    }
}
VERIFY(0x024CAF20, spec_clr_actor);

/* 024CAF78 */
static void spec_init(Act_c* i_this) {
    WWHD_FUNC(0x024CAF78, void, i_this);
    Act_c::M7A0* ptr = &i_this->m7A0[0];
    for (s32 i = 0; i < 3; i++, ptr++) {
        ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
        ptr->m04 = 0;
        ptr->m06 = 0;
        ptr->m08 = 0;
        ptr->m0C = 0.0f;
        ptr->m10 = 0.0f;
        ptr->m14 = 0.0f;
        ptr->m18 = 0;
        M_spec_act()[i] = nullptr;
    }

    s32 iVar2 = prm_get_spec(i_this);
    if (spec_is_boko(iVar2)) {
        if (iVar2 == 1) {
            spec_make_boko(i_this, 1);
        } else if (iVar2 == 2) {
            spec_make_boko(i_this, 2);
        } else if (iVar2 == 3) {
            spec_make_boko(i_this, 3);
        }
        /* (GameCube bug kept: ptr is one past the array, i.e. m7F4.x) */
        if (ptr->m00 != fpcM_ERROR_PROCESS_ID_e) {
            prm_set_cull_non(i_this);
        }
    }
    spec_set_actor(i_this);
    spec_carry_spec(i_this);
    spec_clr_actor(i_this);
}
VERIFY(0x024CAF78, spec_init);

/* 024CB088 */
static void set_tensor(Act_c* i_this, const cXyz* pos) {
    WWHD_FUNC(0x024CB088, void, i_this, pos);
    C_QUATRotAxisRad(mDoMtx_quatStack_get(), pos, (f32)(s16)i_this->m68E * gabi::load<f32>(0x10040C98) /* cM_s2rad */);
    PSMTXQuat(&i_this->mPoseMtx, mDoMtx_quatStack_get());
    mDoMtx_ZXYrotM(&i_this->mPoseMtx, i_this->shape_angle.x, i_this->shape_angle.y, i_this->shape_angle.z);
}
VERIFY(0x024CB088, set_tensor);

/* 024CB118 */
static void spec_mtx(Act_c* i_this) {
    WWHD_FUNC(0x024CB118, void, i_this);
    s32 spec = prm_get_spec(i_this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &i_this->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            fopAc_ac_c* boko = M_spec_act()[i];
            if (boko != nullptr) {
                gabi::Local<Mtx34> sp08;
                PSMTXCopy(mDoMtx_stack_c::get(), sp08);
                mDoMtx_stack_push();
                mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)-ptr->m06);
                mDoMtx_stack_c::transM(0.0f, ptr->m10, M_data_spec_boko(i)->m08);
                mDoMtx_XrotM(mDoMtx_stack_c::get(), M_data_spec_boko(i)->m02);
                gabi::Local<Mtx34> sp38;
                PSMTXConcat(sp08, mDoMtx_stack_c::get(), sp38);
                /* daBoko_c::setMatrix: the model (HD +0x3B4) gets the matrix */
                J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(boko) + 0x3B4));
                if (model != nullptr) {
                    J3DModel_setBaseTRMtx(model, sp38);
                }
                mDoMtx_stack_pop();
            }
        }
    }
}
VERIFY(0x024CB118, spec_mtx);

/* 024CB280 */
static void set_mtx(Act_c* i_this) {
    WWHD_FUNC(0x024CB280, void, i_this);
    J3DModel* model = i_this->mpModel;
    {
        gabi::Local<cXyz> s;
        cXyz_ml(&i_this->scale, s, data(i_this)->mModelScale);
        J3DModel_setBaseScale(model, s);
    }

    s32 type = i_this->mType;
    f32 fVar1;
    if (type == 13 || type == 7) {
        fVar1 = (f32)data(i_this)->mAcchRoofHeight * 0.5f * i_this->scale.y;
    } else {
        fVar1 = 0.0f;
    }

    Mtx34* now = mDoMtx_stack_c::get();
    if (i_this->m678 == 4 && i_this->m685) {
        PSMTXTrans(now, i_this->current.pos.x, i_this->current.pos.y + i_this->m6EC + fVar1, i_this->current.pos.z);
        PSMTXConcat(now, &i_this->mPoseMtx, now);
    } else {
        PSMTXTrans(now, i_this->current.pos.x, i_this->current.pos.y + data(i_this)->m04 + i_this->m6EC + fVar1, i_this->current.pos.z);
        PSMTXConcat(now, &i_this->mPoseMtx, now);
        mDoMtx_stack_c::transM(0.0f, -data(i_this)->m04, 0.0f);
    }
    J3DModel_setBaseTRMtx(i_this->mpModel, now);

    if (i_this->mType == 2) {
        /* static cXyz offset_pos(0.0f, 85.0f, 0.0f): function-local static (guard 0x1046EA7C) */
        be<u32>& guard = *gabi::at<be<u32>>(0x1046EA7C);
        cXyz* offset_pos = gabi::at<cXyz>(0x1046EA64);
        if (guard == 0) {
            guard = 1;
            offset_pos->x = 0.0f;
            offset_pos->y = 85.0f;
            offset_pos->z = 0.0f;
        }
        PSMTXMultVec(now, offset_pos, &i_this->m700);
    }
    spec_mtx(i_this);
}
VERIFY(0x024CB280, set_mtx);

/* 024CB564 */
static void init_mtx(Act_c* i_this) {
    WWHD_FUNC(0x024CB564, void, i_this);
    PSMTXIdentity(&i_this->mPoseMtx);
    for (int k = 0; k < 4; k++) /* m694 = ZeroQuat (0x101E9C38), a word copy */
        gabi::store<u32>(gabi::ea(&i_this->m694[0]) + 4 * k, gabi::load<u32>(0x101E9C38 + 4 * k));
    i_this->m6B0.x = cXyz_Zero->x;
    i_this->m6B0.y = cXyz_Zero->y;
    i_this->m6B0.z = cXyz_Zero->z;
    set_tensor(i_this, gabi::at<cXyz>(0x101FFBB4) /* cXyz::BaseX */);
    set_mtx(i_this);
}
VERIFY(0x024CB564, init_mtx);

/* 024CB5EC */
static cPhs_State _create(Act_c* i_this) {
    WWHD_FUNC(0x024CB5EC, cPhs_State, i_this);
    prmZ_init(i_this);
    s32 type = prm_get_type(i_this);
    if (type >= 0x10) { /* HD: clamped */
        type = 0xF;
    }
    i_this->mType = type;

    /* fopAcM_ct(this, Act_c): HD calls the constructor out of line */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            Act_c_ct(i_this);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State PVar2;
    if (data(i_this)->mFlag & Act_c::DATA_FLAG_1_e) {
        PVar2 = cPhs_COMPLEATE_e;
    } else {
        PVar2 = dComIfG_resLoad(&i_this->mPhase, M_arcname(i_this->mType));
        if (PVar2 != cPhs_COMPLEATE_e) {
            return PVar2;
        }
    }

    if (!fopAcM_entrySolidHeap(i_this, 0x024CA5EC /* solidHeapCB */, data(i_this)->mHeapSize)) {
        return cPhs_ERROR_e;
    }
    create_init_cull(i_this);
    create_init_cc(i_this);
    create_init_bgc(i_this);
    i_this->gravity = data(i_this)->mGravity;
    if (data(i_this)->mFlag & Act_c::DATA_FLAG_10_e) {
        i_this->actor_status = i_this->actor_status | 0x80000; /* fopAcStts_UNK80000_e */
    }
    if (data(i_this)->mFlag & Act_c::DATA_FLAG_20_e) {
        i_this->actor_status = i_this->actor_status | 0x8000000; /* fopAcStts_UNK8000000_e */
    }

    fopAcM_posMoveF(i_this, nullptr);
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    Acch_ClrGroundLanding(&i_this->mAcch);

    bool iVar1 = prm_get_stick(i_this) != 0;
    bool iVar3 = prm_get_moveBg(i_this);
    if (iVar1 || iVar3) {
        i_this->current.pos.x = i_this->home.pos.x;
        i_this->current.pos.y = i_this->home.pos.y;
        i_this->current.pos.z = i_this->home.pos.z;
        if (iVar3 && dBgS_ChkMoveBG_NoDABg(dComIfG_Bgsp(), Acch_gnd(&i_this->mAcch))) {
            prm_off_moveBg(i_this);
            i_this->current.pos.y = i_this->mAcch.GetGroundH();
        }
    }

    i_this->m680 = 1;
    if (prm_get_spec(i_this) == 5 && !is_switch(i_this)) {
        i_this->m681 = 1;
        prm_off_moveBg(i_this);
    } else {
        i_this->m681 = 0x14;
    }

    i_this->m6FC = i_this->current.pos.y;
    i_this->m6EC = 0.0f;
    /* cXyz::Zero is loaded once here and reused for m7F4 / m814 below */
    f32 zx = cXyz_Zero->x, zy = cXyz_Zero->y, zz = cXyz_Zero->z;
    i_this->m6F0.x = zx;
    i_this->m6F0.y = zy;
    i_this->m6F0.z = zz;
    attn_onBit(i_this, fopAc_Attn_ACTION_CARRY_e);
    u32 b = gabi::ea(i_this);
    gabi::store<u8>(b + 0x38C, data(i_this)->mAttnDist); /* distances[fopAc_Attn_TYPE_CARRY_e] */
    gabi::store<f32>(b + 0x390, i_this->current.pos.x);
    gabi::store<f32>(b + 0x394, i_this->current.pos.y + data(i_this)->mAttnY);
    gabi::store<f32>(b + 0x398, i_this->current.pos.z);
    i_this->m802 = 0;
    i_this->m800 = 0;
    if (i_this->mType == 7) {
        if (!prm_get_stick(i_this)) {
            i_this->m800 = 0x4B0; /* M_attrSpine.m30 */
        }
    }

    init_rot_clean(i_this);
    s32 iVar4 = prm_get_spec(i_this);
    if (iVar4 == 0 && !is_switch(i_this)) {
        mode_hide_init(i_this);
    } else if (iVar4 == 4) {
        mode_appear_init(i_this);
    } else {
        mode_wait_init(i_this);
    }

    i_this->m4F8 = -1000000000.0f;
    i_this->m67F = 0;
    i_this->m682 = 1;
    i_this->m683 = 0;
    i_this->m684 = 0;
    i_this->m685 = 0;
    i_this->m686 = 0;
    i_this->m811 = 0;

    if (i_this->mType == 2) {
        for (s32 i = 0; i < 3; i++) {
            /* dPa_followEcallBack::onEnd: flag byte +0x10 */
            u32 p = gabi::ea(&i_this->m70C[i]) + 0x10;
            gabi::store<u8>(p, gabi::load<u8>(p) | 1);
            u32 q = gabi::ea(&i_this->m748[i]) + 0x10;
            gabi::store<u8>(q, gabi::load<u8>(q) | 1);
        }
    }
    if (i_this->mType == 14) {
        eff_kutani_init(i_this);
    }
    spec_init(i_this);

    i_this->m7F4.set(zx, zy, zz);
    i_this->m814.set(zx, zy, zz);
    init_mtx(i_this);
    i_this->m810 = 0;
    return PVar2;
}
VERIFY(0x024CB5EC, _create);

/* 024CBB58 */
static void spec_remove(Act_c* i_this) {
    WWHD_FUNC(0x024CBB58, void, i_this);
    s32 spec = prm_get_spec(i_this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &i_this->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0) {
                fopAc_ac_c* boko = M_spec_act()[i];
                if (boko != nullptr) {
                    boko->current.roomNo = fopAcM_GetRoomNo(i_this); /* fopAcM_SetRoomNo */
                    fopAcM_cancelCarryNow(boko);
                    boko->current.angle.y = (s16)(i_this->shape_angle.y - ptr->m06 - 0x8000);
                    fopAcM_SetSpeedF(boko, cM_rndF(9.0f));
                    boko->speed.y = cM_rndF(13.0f);
                    ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
                }
            }
        }
    }
}
VERIFY(0x024CBB58, spec_remove);

/* 024CBC58 */
static bool _is_delete(Act_c* i_this) {
    WWHD_FUNC(0x024CBC58, bool, i_this);
    spec_set_actor(i_this);
    spec_remove(i_this);
    spec_clr_actor(i_this);
    return true;
}
VERIFY(0x024CBC58, _is_delete);

/* 024CBC98 _delete (matcher: unnamed) */
static bool _delete(Act_c* i_this) {
    WWHD_FUNC(0x024CBC98, bool, i_this);
    for (s32 i = 0; i < 3; i++) {
        i_this->m70C[i].remove();
        i_this->m748[i].remove();
    }
    i_this->m784.remove();
    if (!(data(i_this)->mFlag & Act_c::DATA_FLAG_1_e)) {
        dComIfG_resDelete(&i_this->mPhase, M_arcname(i_this->mType)); /* dComIfG_resDeleteDemo */
    }
    return true;
}
VERIFY(0x024CBC98, _delete);

/* 024CBD48 */
static void cull_set_move(Act_c* i_this) {
    WWHD_FUNC(0x024CBD48, void, i_this);
    Act_c::Data_c* d = data(i_this);
    fopAcM_setCullSizeSphere(i_this, d->mCullSphX_Move, d->mCullSphY_Move, d->mCullSphZ_Move, d->mCullSphR_Move);
}
VERIFY(0x024CBD48, cull_set_move);

/* 024CBDE8 set_senv (matcher: "daObj::PrmAbstract<daTsubo::Act_c::Prm_e>") */
static void set_senv(Act_c* i_this, int arg1, int arg2) {
    WWHD_FUNC(0x024CBDE8, void, i_this, arg1, arg2);
    gabi::Local<cXyz> pos; /* cXyz by value */
    pos->x = i_this->current.pos.x;
    pos->y = i_this->current.pos.y;
    pos->z = i_this->current.pos.z;
    dKy_Sound_set(pos, arg1, gabi::load<u32>(gabi::ea(i_this) + 4) /* fopAcM_GetID: HD no null check */, arg2);
}
VERIFY(0x024CBDE8, set_senv);

/* 024D09A8..024D09C8: thunks (per-TU tail calls): the eff_break / Method table entries */
static void thunk_eff_break_pinecone(Act_c* p) { WWHD_FUNC(0x024D09A8, void, p); gabi::call(0x024D08B8, p); }
VERIFY(0x024D09A8, thunk_eff_break_pinecone);
static void thunk_eff_break_stool_a(Act_c* p) { WWHD_FUNC(0x024D09AC, void, p); gabi::call(0x024D06F4, p); }
VERIFY(0x024D09AC, thunk_eff_break_stool_a);
static void thunk_eff_break_skull(Act_c* p) { WWHD_FUNC(0x024D09B0, void, p); gabi::call(0x024D07FC, p); }
VERIFY(0x024D09B0, thunk_eff_break_skull);
static void thunk_eff_break_stool_b(Act_c* p) { WWHD_FUNC(0x024D09B4, void, p); gabi::call(0x024D06F4, p); }
VERIFY(0x024D09B4, thunk_eff_break_stool_b);
/* Method::Create / Delete / Execute / Draw / IsDelete */
static cPhs_State Mthd_Create(void* p) { WWHD_FUNC(0x024D09B8, cPhs_State, p); return gabi::call<cPhs_State>(0x024CB5EC, p); }
VERIFY(0x024D09B8, Mthd_Create);
static bool Mthd_Delete(void* p) { WWHD_FUNC(0x024D09BC, bool, p); return gabi::call<bool>(0x024CBC98, p); }
VERIFY(0x024D09BC, Mthd_Delete);
static bool Mthd_Execute(void* p) { WWHD_FUNC(0x024D09C0, bool, p); return gabi::call<bool>(0x024CE71C, p); }
VERIFY(0x024D09C0, Mthd_Execute);
static bool Mthd_Draw(void* p) { WWHD_FUNC(0x024D09C4, bool, p); return gabi::call<bool>(0x024CE994, p); }
VERIFY(0x024D09C4, Mthd_Draw);
static bool Mthd_IsDelete(void* p) { WWHD_FUNC(0x024D09C8, bool, p); return gabi::call<bool>(0x024CBC58, p); }
VERIFY(0x024D09C8, Mthd_IsDelete);

/* 024D09CC */
static void __sinit_d_a_tsubo_cpp() {
    WWHD_FUNC(0x024D09CC, void, (u32)0);
    sinit_header_statics(0x1046EA48, 0x101D2AF8);
}
VERIFY(0x024D09CC, __sinit_d_a_tsubo_cpp);

/* 024D0A60 / 024D0A74: deleting destructors of classes with trivial destructors (the
 * followEcallBack array elements' destructor helper is 024D0A74) */
static void trivial_dt_a(void* p, s32 flags) {
    WWHD_FUNC(0x024D0A60, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024D0A60, trivial_dt_a);
static void trivial_dt_b(void* p, s32 flags) {
    WWHD_FUNC(0x024D0A74, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024D0A74, trivial_dt_b);

/* 024D0A88: empty (per-TU sead::SafeString helper) */
static void empty_fn(void* p) {
    WWHD_FUNC(0x024D0A88, void, p);
}
VERIFY(0x024D0A88, empty_fn);

/* 024D0A8C: Act_c deleting destructor (inline member destructors) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x024D0A8C, void, i_this, flags);
    if (i_this != nullptr) {
        u32 b = gabi::ea(i_this);
        gabi::call(0x028F0164, i_this->m748, 3, 0x14, 0x024D0A74, 0, 0); /* __destroy_arr */
        gabi::call(0x028F0164, i_this->m70C, 3, 0x14, 0x024D0A74, 0, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        gabi::store<u32>(b + 0x5E0, 0x10040B1C); /* ~dBgS_ObjGndChk_Yogan: base vtables */
        gabi::store<u32>(b + 0x600, 0x10040B3C);
        gabi::store<u32>(b + 0x60C, 0x10040AFC);
        gabi::call(0x02008DAC, &i_this->mGndChkYogan, 0); /* cBgS_GndChk::~cBgS_GndChk */
        gabi::call(0x02018034, gabi::at<u8>(b + 0x594), 2); /* mAcchCir's cM3dGCir */
        gabi::store<u32>(b + 0x3DC, TSUBO_OBJACCH_VT.v20);
        gabi::store<u32>(b + 0x3D0, TSUBO_OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);         /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024D0A8C, Act_c_dt);

} // namespace daTsubo
