/**
 * d_a_sitem.cpp (WWHD)
 * Enemy item: a hand-held object (pot, barrel, ...) hanging on a chain, dropped and broken
 * when its chain is cut.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_sitem.cpp) has only "Nonmatching" stubs for this unit (and no layout), so
 * every function here is written from the WWHD code (cking.rpx) and verified against it.
 */
#include "bindings.h"

#define M_arcname_res STR(0x1003CD44)   /* "Sitem" (useHeapInit's SafeString) */
#define SAFESTRING_VTBL 0x1003CD4C
#define SITEM_VTBL 0x1003CDE4           /* sitem_class vtable (HD virtual destructor) */
#define tg_sph_src 0x101D0824           /* dCcD_SrcSph (.data) */
#define bm_sph_src 0x101D0864
#define l_bmd_idx 0x101D078C            /* u16[type]: model resource index */
#define l_chain_color 0x101D0784        /* GXColor */
#define l_chain2_color 0x101D0788       /* GXColor */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
WWHD_OPAQUE(mDoExt_3DlineMat0_c);
/* 025E9960 mDoExt_3DlineMat0_c::mDoExt_3DlineMat0_c (HD 0x148), 025E99E0 its deleting destructor,
 * 025E9B80 init(u16 numLines, u16 numSegs, BOOL hasSize) (as d_a_mo2.h) */
static inline void mDoExt_3DlineMat0_ct(void* l) { gabi::call(0x025E9960, l); }
static inline void mDoExt_3DlineMat0_dt(void* l, s32 flags) { gabi::call(0x025E99E0, l, flags); }
static inline BOOL mDoExt_3DlineMat0_init(void* l, u16 numLines, u16 numSegs, BOOL hasSize) {
    return gabi::call<BOOL>(0x025E9B80, l, numLines, numSegs, hasSize);
}
/* 025EAF58 mDoExt_3DlineMat0_c::update(u16 segs, GXColor& color, dKy_tevstr_c*) (the overload without size) */
static inline void mDoExt_3DlineMat0_update2(void* l, s32 segs, const GXColor* color, dKy_tevstr_c* tev) {
    gabi::call(0x025EAF58, l, segs, color, tev);
}
/* dComIfGd_set3DlineMat: play + 0x5FB4 + getMaterialID() * 0x9C (as d_a_mo2.h) */
static inline void dComIfGd_set3DlineMat_l(void* l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(l) + 0x130) + 0x14), l); /* getMaterialID() */
    gabi::call(0x025EDD04, pkt + id * 0x9C, l);
}
static inline void mDoMtx_YrotM_l(Mtx34* m, s16 y) { gabi::call(0x025F1C28, m, y); }
/* GHS array helpers */
static inline void __construct_array_l(u32 p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr_l(u32 p, u32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0, 0); }

struct sitem_class : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<J3DModel> mpModel;
    /* 0x3D4 */ be<u8> mType;          /* param bits 0..7 (0xFF: 0) */
    /* 0x3D5 */ be<u8> m3D5;           /* param bits 8..15 (> 3: 0) */
    /* 0x3D6 */ be<u8> mItemBitNo;     /* param bits 16..23 */
    /* 0x3D7 */ be<u8> mItemTbl;       /* param bits 24..31 (0xFF: 0) */
    /* 0x3D8 */ be<s16> mCount;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<s16> mMode;
    /* 0x3DE */ be<s16> mTimer[2];
    /* 0x3E2 */ be<s16> mTimer2;
    /* 0x3E4 */ cXyz mPos;
    /* 0x3F0 */ cXyz mOldPos;
    /* 0x3FC */ cXyz mHandPos;
    /* 0x408 */ be<s16> mHandAngX;
    /* 0x40A */ be<s16> mHandAngY;
    /* 0x40C */ u8 _40C[0x410 - 0x40C];
    /* 0x410 */ be<f32> m410;
    /* 0x414 */ u8 _414[4];
    /* 0x418 */ be<f32> m418;
    /* 0x41C */ be<f32> m41C;
    /* 0x420 */ be<f32> mGroundY;
    /* 0x424 */ be<f32> m424;
    /* 0x428 */ u8 mChain[0x148];      /* mDoExt_3DlineMat0_c (HD 0x148; vtable at +0x130) */
    /* 0x570 */ u8 mChainSeg[10][0x1C];
    /* 0x688 */ u8 mChain2[0x148];
    /* 0x7D0 */ u8 mChain2Seg[5][0x1C];
    /* 0x85C */ dCcD_Stts mStts;
    /* 0x898 */ dCcD_Sph mSph[4];
    /* 0xD48 */ dCcD_Sph mBmSph;
    /* 0xE74 */ cXyz mE74;
    /* 0xE80 */ be<f32> mE80;
    /* 0xE84 */ be<s16> mE84;
    /* 0xE86 */ u8 _E86[2];
    /* 0xE88 */ dBgS_AcchCir mAcchCir;
    /* 0xEC8 */ dBgS_ObjAcch mAcch;
    /* 0x108C */ dPa_followEcallBack mFollow[2];
    /* 0x10B4 */ u8 _10B4[0x10C4 - 0x10B4];
};
WWHD_OFFSET(sitem_class, mPhs, 0x3C8);
WWHD_OFFSET(sitem_class, m424, 0x424);
WWHD_OFFSET(sitem_class, mChain2, 0x688);
WWHD_OFFSET(sitem_class, mStts, 0x85C);
WWHD_OFFSET(sitem_class, mBmSph, 0xD48);
WWHD_OFFSET(sitem_class, mAcchCir, 0xE88);
WWHD_OFFSET(sitem_class, mAcch, 0xEC8);
WWHD_OFFSET(sitem_class, mFollow, 0x108C);
WWHD_SIZE(sitem_class, 0x10C4);

/* 02486390 */
static BOOL daSitem_Draw(sitem_class* i_this) {
    WWHD_FUNC(0x02486390, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->eyePos, &i_this->tevStr);
    if (i_this->mMode < 6) {
        J3DModel* model = i_this->mpModel;
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        mDoExt_modelUpdateDL(model);
    }
    mDoExt_3DlineMat0_update2(i_this->mChain, 10, gabi::at<GXColor>(l_chain_color), &i_this->tevStr);
    dComIfGd_set3DlineMat_l(i_this->mChain);
    if (i_this->m424 > 0.1f) {
        mDoExt_3DlineMat0_update2(i_this->mChain2, 5, gabi::at<GXColor>(l_chain2_color), &i_this->tevStr);
        dComIfGd_set3DlineMat_l(i_this->mChain2);
    }
    return TRUE;
}
VERIFY(0x02486390, daSitem_Draw);

/* 024864A0 */
void hand_mtx_set(sitem_class* i_this) {
    WWHD_FUNC(0x024864A0, void, i_this);
    MtxTrans(i_this->mHandPos.x, i_this->mHandPos.y, i_this->mHandPos.z, 0);
    mDoMtx_XrotM(calc_mtx(), i_this->mHandAngX);
    mDoMtx_YrotM_l(calc_mtx(), i_this->mHandAngY);
    mDoMtx_XrotM(calc_mtx(), 0x4000);
    gabi::Local<cXyz> off;
    off->x = 0.0f;
    off->y = -60.0f;
    off->z = 0.0f;
    MtxPosition(off, &i_this->mE74);
    MtxTrans(0.0f, -50.0f, 0.0f, 1);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
}
VERIFY(0x024864A0, hand_mtx_set);

/* 024865CC */
void my_break(sitem_class* i_this) {
    WWHD_FUNC(0x024865CC, void, i_this);
    i_this->mTimer[0] = 50;
    i_this->mMode = 6;
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, i_this->current.angle.y, 0);
    fopAcM_createItemFromTable(&i_this->mPos, i_this->mItemTbl, i_this->mItemBitNo & 0x7F, i_this->home.roomNo, 0, angle, 1, nullptr);

    u8 type = i_this->mType;
    GXColor* color = gabi::at<GXColor>(gabi::ea(&i_this->tevStr) + 0x98); /* tevStr.mColorK0 */
    if (type <= 1) {
        dComIfGp_particle_set(0x8168, &i_this->mPos);
        dComIfGp_particle_set(0x8167, &i_this->mPos, nullptr, nullptr, 0xFF, nullptr, -1, color, color);
        type = i_this->mType;
        if (type == 1) {
            dComIfGp_particle_set(0x8169, &i_this->mPos, nullptr, nullptr, 0xFF, nullptr, -1, color, color);
            type = i_this->mType;
        }
    }
    if (type == 2)
        dComIfGp_particle_set(0x816A, &i_this->mPos, nullptr, nullptr, 0xFF, nullptr, -1, color, color);

    fopAcM_seStart(i_this, 0x6936, 0);
    fopAcM_create(0x1D3, 4, &i_this->mPos, -1, nullptr, nullptr, -1, 0);
}
VERIFY(0x024865CC, my_break);

/* 02487E94 */
static BOOL daSitem_IsDelete(sitem_class*) {
    WWHD_FUNC(0x02487E94, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02487E94, daSitem_IsDelete);

/* 02487E9C */
static BOOL daSitem_Delete(sitem_class* i_this) {
    WWHD_FUNC(0x02487E9C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1003CE4C) /* "Sitem" */);
    for (int i = 0; i < 2; i++) {
        /* mFollow[i].remove() (virtual, slot 0x44) */
        u32 cb = gabi::ea(&i_this->mFollow[i]);
        gabi::call_ptr<void>(gabi::load<u32>(gabi::load<u32>(cb) + 0x44), cb);
    }
    return TRUE;
}
VERIFY(0x02487E9C, daSitem_Delete);

/* 02487F00: useHeapInit, also the solid heap callback (daSitem_solidHeapCB merged) */
static BOOL useHeapInit(sitem_class* i_this) {
    WWHD_FUNC(0x02487F00, BOOL, i_this);
    s32 idx = gabi::load<u16>(l_bmd_idx + 2 * i_this->mType);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, idx, SAFESTRING_VTBL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    i_this->mpModel = model;
    if (model == nullptr)
        return FALSE;
    if (!mDoExt_3DlineMat0_init(i_this->mChain, 1, 10, TRUE))
        return FALSE;
    if (!mDoExt_3DlineMat0_init(i_this->mChain2, 1, 5, TRUE))
        return FALSE;
    return TRUE;
}
VERIFY(0x02487F00, useHeapInit);

/* 02487FD4: dPa_followEcallBack array element constructor (__construct_array) */
static u32 followEcallBack_ct(dPa_followEcallBack* p) {
    WWHD_FUNC(0x02487FD4, u32, p);
    return gabi::call<u32>(0x025A5894, p, (u8)0, (u8)0);
}
VERIFY(0x02487FD4, followEcallBack_ct);

/* 02487FE0: sitem_class::sitem_class (inline in fopAcM_ct; HD: allocates when this == NULL) */
static sitem_class* sitem_class_ct(sitem_class* self) {
    WWHD_FUNC(0x02487FE0, sitem_class*, self);
    if (self == nullptr) {
        self = (sitem_class*)operator_new(sizeof(sitem_class));
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = SITEM_VTBL;
    mDoExt_3DlineMat0_ct(self->mChain);
    mDoExt_3DlineMat0_ct(self->mChain2);
    dCcD_Stts_ct(&self->mStts);
    __construct_array_l(gabi::ea(self->mSph), 4, sizeof(dCcD_Sph), 0x025166F0 /* dCcD_Sph::dCcD_Sph */);
    gabi::call(0x025166F0, &self->mBmSph);
    dBgS_AcchCir_ct(&self->mAcchCir);
    static const dBgS_ObjAcch_vt acchVt = {0x1003CDB4, 0x1003CDD4, 0x1003CDC4};
    dBgS_ObjAcch_ct(&self->mAcch, acchVt);
    __construct_array_l(gabi::ea(self->mFollow), 2, sizeof(dPa_followEcallBack), 0x02487FD4);
    return self;
}
VERIFY(0x02487FE0, sitem_class_ct);

/* 024880E0 */
static cPhs_State daSitem_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x024880E0, cPhs_State, a_this);
    sitem_class* i_this = (sitem_class*)a_this;
    /* fopAcM_ct(i_this, sitem_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            sitem_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase = dComIfG_resLoad(&i_this->mPhs, STR(0x1003CE60) /* "Sitem" */);
    if (phase != cPhs_COMPLEATE_e)
        return phase;

    u8 type = fopAcM_GetParam(a_this) & 0xFF;
    i_this->mType = (type == 0xFF) ? 0 : type;
    u32 p = (fopAcM_GetParam(a_this) >> 8) & 0xFF;
    i_this->m3D5 = (p <= 3) ? p : 0;
    p = fopAcM_GetParam(a_this) >> 24;
    i_this->mItemTbl = (p == 0xFF) ? 0 : p;
    i_this->mItemBitNo = (fopAcM_GetParam(a_this) >> 16) & 0xFF;
    if (!fopAcM_entrySolidHeap(a_this, 0x02487F00 /* useHeapInit */, 0x3040))
        return cPhs_ERROR_e;

    /* HD: a static cXyz {0, 30000, -20000} (role unknown) */
    gabi::store<f32>(0x1046DED8, 0.0f);
    gabi::store<f32>(0x1046DEDC, 30000.0f);
    gabi::store<f32>(0x1046DEE0, -20000.0f);
    a_this->health = 2;
    i_this->mCount = (s16)gabi::ftoi(cM_rndF(10000.0f));
    i_this->mStts.Init(0xFF, 0xFF, a_this);
    for (int i = 0; i < 4; i++) {
        dCcD_Sph* sph = &i_this->mSph[i];
        sph->Set(gabi::at<dCcD_SrcSph>(tg_sph_src));
        sph->SetStts(&i_this->mStts);
        if (i_this->mType != 1)
            sph->OffAtSPrmBit(1); /* OffAtSPrmBit(cCcD_AtSPrm_Set_e) */
    }
    i_this->mBmSph.Set(gabi::at<dCcD_SrcSph>(bm_sph_src));
    i_this->mBmSph.SetStts(&i_this->mStts);
    i_this->mPos.copy(a_this->current.pos);
    i_this->mAcch.Set(&i_this->mPos, &i_this->mOldPos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
    i_this->mAcchCir.SetWall(30.0f, 50.0f);
    for (int i = 2; i != 0; i--)
        gabi::call<BOOL>(0x024867D0, i_this); /* daSitem_Execute */
    return phase;
}
VERIFY(0x024880E0, daSitem_Create);

/* 02488348 */
static void __sinit_d_a_sitem_cpp() {
    WWHD_FUNC(0x02488348, void, (u32)0);
    sinit_header_statics(0x1046DEBC, 0x101D08A4);
}
VERIFY(0x02488348, __sinit_d_a_sitem_cpp);

/* 024883DC: sead::SafeString deleting destructor (this TU's copy; trivial) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024883DC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024883DC, SafeString_dt);

/* 024883F0: dPa_followEcallBack array element deleting destructor (__destroy_arr; trivial) */
static void followEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024883F0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024883F0, followEcallBack_dt);

/* 02488404: sitem_class deleting destructor (compiler-generated, HD virtual destructor) */
static void sitem_class_dt(sitem_class* i_this, s32 flags) {
    WWHD_FUNC(0x02488404, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr_l(gabi::ea(i_this->mFollow), 2, sizeof(dPa_followEcallBack), 0x024883F0);
        /* ~dBgS_ObjAcch: this TU's vtables, then ~dBgS_Acch */
        u32 acch = gabi::ea(&i_this->mAcch);
        gabi::store<u32>(acch + 0x20, 0x1003CDC4);
        gabi::store<u32>(acch + 0x14, 0x1003CDD4);
        gabi::call(0x024EFD9C, acch, 0);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* ~cM3dGCir */
        gabi::call(0x02515AE8, &i_this->mBmSph, 2);                    /* ~dCcD_Sph */
        __destroy_arr_l(gabi::ea(i_this->mSph), 4, sizeof(dCcD_Sph), 0x02515AE8);
        dCcD_Stts_dt(&i_this->mStts, 2);
        mDoExt_3DlineMat0_dt(i_this->mChain2, 2);
        mDoExt_3DlineMat0_dt(i_this->mChain, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02488404, sitem_class_dt);

/* 024884F8: empty virtual in this TU's sead::SafeString vtable */
static void SafeString_empty(void* p) {
    WWHD_FUNC(0x024884F8, void, p);
}
VERIFY(0x024884F8, SafeString_empty);

/* ======================================================================================
 * daSitem_Execute (024867D0, 5.8 KB). The GameCube unit has hand_move, control1/2/3 and
 * cut_control1/2 as separate (stub) functions; WWHD inlines all of them here. The helpers
 * below follow the WWHD code's blocks; GHS duplicated the shared tails (addCalc2 x3, the
 * ground-check destructor) several times.
 * ====================================================================================== */
#define l_hang_y 0x101D07CC      /* f32[4]: hanging height per size (param bits 8..15) */
#define l_swing 0x101D07BC       /* f32[4]: chain segment length per size */
#define l_seg_ofs_y 0x101D07FC   /* f32[10]: per-segment swing offset (y) */
#define l_seg_size 0x101D0794    /* f32[10]: chain link sizes */
#define l_far_pos 0x1046DED8     /* static cXyz {0, 30000, -20000}: collision centre when disabled */
#define l_REG 0x1047B608         /* HD debug registers (f32 at +0x34, +0x4C) */
static const dBgS_GndChk_vt GNDCHK_VT = {0x1003CD74, 0x1003CD84, 0x1003CDA4, 0x1003CD94};

struct CcAtInfo_sitem {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B[0x1C - 0xB];
};
/* 02518DB0 at_power_check(CcAtInfo*), 02518CC8 def_se_set(actor, cCcD_Obj*, u32 mtrl) */
static inline void at_power_check_l(CcAtInfo_sitem* i) { gabi::call(0x02518DB0, i); }
static inline void def_se_set_l(fopAc_ac_c* a, u32 obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }

static inline cXyz* seg(sitem_class* i, int k) { return gabi::at<cXyz>(gabi::ea(i) + 0x570 + 0x1C * k); }
static inline cXyz* seg2(sitem_class* i, int k) { return gabi::at<cXyz>(gabi::ea(i) + 0x7D0 + 0x1C * k); }
static inline u32 segSize(cXyz* s) { return gabi::ea(s) + 0x18; }
static inline s16 neg_s16(s16 v) { return (s16)-(s32)v; }

/* the two angles of a direction: y from (x, z), x from (y, |xz|) */
static inline void dir_angles(f32 x, f32 y, f32 z, s16* ay, s16* ax) {
    *ay = cM_atan2s(x, z);
    f32 r = std_sqrtf(gabi::fmadds(x, x, z * z));
    *ax = neg_s16(cM_atan2s(y, r));
}

/* hand_move: the item's own movement by mode; returns `fall` (mode 5/6) */
static inline bool hand_move(sitem_class* i, cXyz* target, void* gnd) {
    f32 hangY = gabi::load<f32>(l_hang_y + 4 * i->m3D5);
    f32 calcTarget = gabi::load<f32>(l_swing + 4 * i->m3D5);
    f32 m41CTarget = gabi::load<f32>(l_REG + 0x34) + 5.0f;
    f32 calcStep = 1.0f;
    bool fall = false;
    mDoMtx_YrotS(calc_mtx(), i->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i->current.angle.x);
    s32 mode = i->mMode;
    if ((u32)mode <= 1) {
        s16 cnt = i->mCount;
        gabi::Local<cXyz> ofs, rot, res;
        ofs->y = hangY;
        ofs->x = cM_ssin((s16)(cnt * 600)) * 25.0f;
        ofs->z = cM_ssin((s16)(cnt * 700)) * 25.0f;
        MtxPosition(ofs, rot);
        cXyz_pl(&i->current.pos, res, rot);
        f32 z = res->z, x = res->x, y = res->y;
        target->y = y;
        target->x = x;
        target->z = z;
        if (i->mMode == 0) {
            i->mPos.x = x;
            i->mPos.y = y;
            i->mMode = 1;
            i->mPos.z = z;
        }
    } else if (mode == 5) {
        i->mOldPos.copy(i->mPos);
        i->m41C = 50.0f;
        m41CTarget = 50.0f;
        calcTarget = 25.0f;
        fall = true;
        PSVECAdd(&i->mPos, &i->speed, &i->mPos);
        f32 vy = i->speed.y - 3.0f;
        if (vy < -90.0f)
            vy = -90.0f;
        i->speed.y = vy;
        i->mAcch.CrrPos(dComIfG_Bgsp());
        f32 sy = i->speed.y;
        i->mTimer2 = 5;
        if (sy < 0.0f) {
            f32 y = i->mPos.y, z = i->mPos.z, x = i->mPos.x;
            cXyz* gpos = gabi::at<cXyz>(gabi::ea(gnd) + 0x24);
            gpos->z = z;
            gpos->x = x;
            gpos->y = y + 100.0f;
            f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), gnd);
            i->mGroundY = gy;
            if (gy == -1000000000.0f) {
                i->mPos.y = gy + 30.0f;
                my_break(i);
            } else {
                f32 g = gy + 30.0f;
                if (!(i->mPos.y > g)) {
                    i->mPos.y = g;
                    my_break(i);
                }
            }
        }
    } else if (mode == 6) {
        i->mTimer2 = 10;
        fall = true;
        m41CTarget = 0.0f;
        s16 t = i->mTimer[0];
        if (t < 40) {
            calcStep = calcTarget * 0.05f;
            calcTarget = 0.0f;
            if (t == 0)
                fopAcM_delete(i);
        }
    }
    cLib_addCalc2(&i->m410, calcTarget, 0.5f, calcStep);
    cLib_addCalc2(&i->m418, 0.0f, 1.0f, 0.2f);
    cLib_addCalc2(&i->m41C, m41CTarget, 1.0f, 1.5f);
    return fall;
}

/* the hand: position at the last chain link, angles from the second-to-last one */
static inline void hand_set(sitem_class* i) {
    i->mHandPos.copy(*seg(i, 9));
    gabi::Local<cXyz> res;
    cXyz_mi(seg(i, 8), res, seg(i, 9));
    f32 x = res->x, y = res->y, z = res->z;
    i->mHandAngX = neg_s16(cM_atan2s(y, z));
    f32 r = std_sqrtf(gabi::fmadds(y, y, z * z));
    i->mHandAngY = cM_atan2s(x, r);
    hand_mtx_set(i);
}

/* control1 / control2: the chain while the item hangs (pulled towards the target) */
static inline void control1(sitem_class* i, cXyz* target) {
    cLib_addCalc2(&i->speedF, 8.0f, 1.0f, 0.1f);
    if (i->mE80 > 1.0f && i->mMode != 3) {
        mDoMtx_YrotS(calc_mtx(), i->mE84);
        gabi::Local<cXyz> v, off;
        v->y = 0.0f;
        v->z = i->mE80;
        v->x = 0.0f;
        MtxPosition(v, off);
        PSVECAdd(target, off, target);
        f32 s = i->mE80 * 0.2f;
        if (s > 30.0f)
            i->speedF = 30.0f;
        else
            i->speedF = s;
    }
    cLib_addCalc0(&i->mE80, 1.0f, 5.0f);
    cLib_addCalc2(&i->mPos.x, target->x, 0.1f, i->speedF);
    cLib_addCalc2(&i->mPos.y, target->y, 0.1f, i->speedF);
    cLib_addCalc2(&i->mPos.z, target->z, 0.1f, i->speedF);

    seg(i, 0)->copy(i->current.pos);
    mDoMtx_YrotS(calc_mtx(), i->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i->current.angle.x);
    gabi::Local<cXyz> len, dir;
    len->x = 0.0f;
    len->y = 0.0f;
    len->z = i->m418;
    MtxPosition(len, dir);
    f32 step = i->m41C;
    len->z = i->m410;
    gabi::Local<cXyz> p, q, res;
    for (int k = 1; k <= 8; k++) {
        s16 cnt = i->mCount;
        p->x = cM_ssin((s16)(cnt * 0x44C + 4000 * k)) * step;
        p->y = gabi::load<f32>(l_seg_ofs_y + 4 * k);
        p->z = cM_scos((s16)(cnt * 800 + 4000 * k)) * step;
        MtxPosition(p, q);
        cXyz* prev = seg(i, k - 1);
        cXyz* cur = seg(i, k);
        f32 dx = ((cur->x - prev->x) + dir->x) + q->x;
        f32 dy = ((cur->y - prev->y) + dir->y) + q->y;
        f32 dz = ((cur->z - prev->z) + dir->z) + q->z;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        MtxPosition(len, p);
        MtxPull();
        cXyz_pl(prev, res, p);
        cur->copy(*res);
    }

    /* control2: from the item back up the chain */
    seg(i, 9)->copy(i->mPos);
    gabi::Local<cXyz> len2, d2, res2;
    len2->y = 0.0f;
    len2->x = 0.0f;
    len2->z = i->m410;
    for (int k = 8; k >= 1; k--) {
        cXyz* cur = seg(i, k);
        cXyz* next = seg(i, k + 1);
        f32 dx = cur->x - next->x;
        f32 dz = cur->z - next->z;
        f32 dy = cur->y - next->y;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        len2->z = i->m410;
        MtxPosition(len2, d2);
        cXyz_pl(next, res2, d2);
        cur->copy(*res2);
    }
    hand_set(i);
}

/* follow effect at a chain end */
static inline void follow_set(sitem_class* i, int n, cXyz* pos, u32 angleOfs, cXyz* a, cXyz* b) {
    dPa_followEcallBack* cb = &i->mFollow[n];
    if (cb->mpEmitter == nullptr && gabi::load<s8>(gabi::ea(i) + 0x10C0) != 0) {
        dPa_followEcallBack_end(cb);
        s8 room = i->current.roomNo;
        dPa_control_c* pa = dComIfGp_getParticle();
        dPa_control_set(pa, 0, 0x8184, pos, gabi::at<csXyz>(gabi::ea(i) + angleOfs), nullptr, 0xFF,
                        gabi::at<dPa_levelEcallBack>(gabi::ea(cb)), room, nullptr, nullptr, nullptr);
        if (n == 0)
            i->mTimer[1] = 60;
    } else {
        gabi::Local<cXyz> res;
        cXyz_mi(a, res, b);
        f32 x = res->x, y = res->y, z = res->z;
        gabi::store<s16>(gabi::ea(i) + angleOfs + 2, cM_atan2s(x, z));
        f32 r = std_sqrtf(gabi::fmadds(x, x, z * z));
        gabi::store<s16>(gabi::ea(i) + angleOfs, neg_s16(cM_atan2s(y, r)));
        if (i->mTimer[1] == 1 && cb->mpEmitter != nullptr) {
            dPa_followEcallBack_end(cb);
            if (n == 1)
                gabi::store<u8>(gabi::ea(i) + 0x10C0, 0);
        }
    }
}

/* cut_control1 / cut_control2: the cut chain (falling or broken item) */
static inline void cut_control(sitem_class* i) {
    seg2(i, 0)->copy(i->current.pos);
    mDoMtx_YrotS(calc_mtx(), i->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i->current.angle.x);
    gabi::Local<cXyz> len, p, q, res;
    len->y = 0.0f;
    len->x = 0.0f;
    len->z = i->m424;
    for (int k = 1; k <= 4; k++) {
        s16 cnt = i->mCount;
        p->x = cM_ssin((s16)(cnt * 0x1004 - 10000 * k)) * 50.0f;
        p->y = gabi::load<f32>(l_REG + 0x4C) + 50.0f;
        p->z = cM_scos((s16)(cnt * 0x1130 - 10000 * k)) * 50.0f;
        MtxPosition(p, q);
        cXyz* prev = seg2(i, k - 1);
        cXyz* cur = seg2(i, k);
        f32 dx = (cur->x - prev->x) + q->x;
        f32 dz = (cur->z - prev->z) + q->z;
        f32 dy = (cur->y - prev->y) + q->y;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        MtxPosition(len, p);
        MtxPull();
        cXyz_pl(prev, res, p);
        cur->copy(*res);
    }
    follow_set(i, 0, seg2(i, 4), 0x10B4, seg2(i, 4), seg2(i, 3));

    /* the chain left on the holder hangs down from the item */
    seg(i, 9)->copy(i->mPos);
    gabi::Local<cXyz> len2, d2, res2;
    len2->x = 0.0f;
    len2->y = 0.0f;
    len2->z = i->m410;
    f32 step = i->m41C;
    s32 ra = 24000, rb = 32000, rc = 28000;
    for (int k = 9; k >= 1; k--) {
        cXyz* cur = seg(i, k - 1);
        cXyz* next = seg(i, k);
        s16 cnt = i->mCount;
        f32 y = gabi::fmadds(cM_ssin((s16)(cnt * 0xB86 - rb)), step, cur->y - 10.0f);
        f32 ground = i->mGroundY + 5.0f;
        f32 dx = gabi::fmadds(cM_ssin((s16)(cnt * 0x9C4 - ra)), step, cur->x - next->x);
        if (y < ground)
            y = ground;
        f32 dz = (cur->z - next->z) + cM_scos((s16)(cnt * 0xAF0 - rc)) * step;
        f32 dy = y - next->y;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        len2->z = i->m410;
        MtxPosition(len2, d2);
        cXyz_pl(next, res2, d2);
        cur->copy(*res2);
        rc -= 3500;
        ra -= 3000;
        rb -= 4000;
    }
    hand_set(i);
    follow_set(i, 1, seg(i, 0), 0x10BA, seg(i, 0), seg(i, 1));

    /* the cut chain's links into the second line material */
    u32 lines = gabi::load<u32>(gabi::ea(i) + 0x7CC);
    u32 sizes = gabi::load<u32>(lines + 4);
    u32 pos = gabi::load<u32>(lines);
    for (int k = 0; k < 5; k++) {
        gabi::at<cXyz>(pos + 0xC * k)->copy(*seg2(i, k));
        gabi::store<u8>(sizes + k, (u8)gabi::ftoi(gabi::load<f32>(segSize(seg2(i, k)))));
    }
    cLib_addCalc0(&i->m424, 1.0f, 1.0f);
}

/* control3: link sizes and the line material, eye position, colliders */
static inline void control3(sitem_class* i, bool fall) {
    for (int k = 0; k < 10; k++) {
        s16 cnt = i->mCount;
        f32 f = gabi::fmadds(cM_ssin((s16)(cnt * 500 + 100 * k)), 0.1f, 0.8f);
        gabi::store<f32>(segSize(seg(i, k)), gabi::load<f32>(l_seg_size + 4 * k) * f);
    }
    u32 lines = gabi::load<u32>(gabi::ea(i) + 0x56C);
    u32 sizes = gabi::load<u32>(lines + 4);
    u32 pos = gabi::load<u32>(lines);
    for (int k = 0; k < 10; k++) {
        gabi::at<cXyz>(pos + 0xC * k)->copy(*seg(i, k));
        gabi::store<u8>(sizes + k, (u8)gabi::ftoi(gabi::load<f32>(segSize(seg(i, k)))));
    }
    pos = gabi::load<u32>(gabi::load<u32>(gabi::ea(i) + 0x56C));
    u32 ex = gabi::load<u32>(pos + 0x3C), ey = gabi::load<u32>(pos + 0x40), ez = gabi::load<u32>(pos + 0x44);
    gabi::store<u32>(gabi::ea(&i->eyePos), ex);
    gabi::store<u32>(gabi::ea(&i->eyePos) + 4, ey);
    gabi::store<u32>(gabi::ea(&i->eyePos) + 8, ez);
    gabi::store<u32>(gabi::ea(i) + 0x390, ex); /* attention_info.position */
    gabi::store<u32>(gabi::ea(i) + 0x394, ey);
    gabi::store<u32>(gabi::ea(i) + 0x398, ez);

    i->mStts.Move();
    i->mBmSph.SetC(fall ? gabi::at<cXyz>(l_far_pos) : &i->eyePos);
    dComIfG_Ccsp_Set(&i->mBmSph);
    gabi::Local<cXyz> c;
    for (int k = 0; k < 3; k++) {
        u32 n = (u32)(2 * k + (i->mCount & 3));
        n = n - ((n * 0x6667) >> 18) * 10;
        c->copy(*gabi::at<cXyz>(pos + 0xC * n));
        dCcD_Sph* sph = &i->mSph[k];
        sph->SetC(fall ? gabi::at<cXyz>(l_far_pos) : (cXyz*)c);
        if (i->mMode == 3)
            sph->OffCoSPrmBit(1);
        else
            sph->OnCoSPrmBit(1);
        dComIfG_Ccsp_Set(sph);
    }
    i->mSph[3].SetR(50.0f);
    i->mSph[3].SetC(&i->mE74);
    i->mSph[3].OnCoSPrmBit(1);
    dComIfG_Ccsp_Set(&i->mSph[3]);
}

static inline void gndchk_dt(void* gnd) {
    u32 b = gabi::ea(gnd);
    gabi::store<u32>(b + 0x40, 0x1003CDA4);
    gabi::store<u32>(b + 0x20, 0x1003CD84);
    gabi::store<u32>(b + 0x4C, 0x1003CD64);
    gabi::call(0x02008DAC, gnd, 0); /* cBgS_Chk::~cBgS_Chk */
}

/* the cut chain takes over the upper five links */
static inline void chain_cut(sitem_class* i) {
    for (int k = 0; k < 5; k++) {
        seg2(i, k)->copy(*seg(i, k));
        gabi::store<f32>(segSize(seg2(i, k)), gabi::load<f32>(segSize(seg(i, k))));
        if (k == 4) {
            gabi::Local<cXyz> d;
            cXyz_mi(seg2(i, 4), d, seg2(i, 3));
            i->m424 = std_sqrtf(PSVECSquareMag(d)) * 1.5f;
        }
    }
}

/* hits: returns FALSE for the early return (blown away by wind) */
static inline BOOL hit_check(sitem_class* i, CcAtInfo_sitem* at) {
    at->mpObj = 0;
    if (i->mTimer2 != 0)
        return TRUE;
    BOOL chainHit = FALSE;
    dCcD_Sph* hand = &i->mSph[3];
    if (hand->ChkTgHit() || (i->mE80 > 100.0f && hand->ChkCoHit())) {
        at->mpObj = gabi::ea(hand->GetTgHitObj());
        if (at->mpObj != 0 && (gabi::load<u32>(at->mpObj + 0x10) & 0x08000000))
            return TRUE;
        my_break(i);
    } else {
        bool hit = false;
        for (int k = 0; k < 3; k++) {
            if (i->mSph[k].ChkTgHit()) {
                at->mpObj = gabi::ea(i->mSph[k].GetTgHitObj());
                if (at->mpObj != 0 && (gabi::load<u32>(at->mpObj + 0x10) & 0x08000000))
                    return TRUE;
                chainHit = TRUE;
                hit = true;
                break;
            }
        }
        if (!hit && !i->mBmSph.ChkTgHit())
            return TRUE;
        if (i->mTimer2 != 0) {
            if (!chainHit)
                return TRUE;
        } else {
            i->mTimer2 = 20;
            if (!chainHit) {
                at->mpObj = gabi::ea(i->mBmSph.GetTgHitObj());
                at_power_check_l(at);
                if (at->mResultingAttackType == 8) {
                    i->mE80 = 300.0f;
                    i->mE84 = (s16)(fopAcM_searchActorAngleY(i, dComIfGp_getPlayer(0)) + 0x8000);
                    return FALSE;
                }
            }
            i->mMode = 5;
            i->speed.x = cM_rndFX(10.0f);
            i->speed.y = cM_rndF(5.0f) + 10.0f;
            i->speed.z = cM_rndFX(10.0f);
            gabi::Local<cXyz> sc;
            sc->x = 0.3f;
            sc->y = 0.3f;
            sc->z = 0.3f;
            dComIfGp_particle_set(0x16, &i->eyePos, nullptr, sc);
            gabi::store<u8>(gabi::ea(i) + 0x10C0, 1);
        }
    }
    if (at->mpObj != 0)
        def_se_set_l(i, at->mpObj, 0x21);
    chain_cut(i);
    return TRUE;
}

/* 024867D0 */
static BOOL daSitem_Execute(sitem_class* i_this) {
    WWHD_FUNC(0x024867D0, BOOL, i_this);
    i_this->mCount = i_this->mCount + 1;
    for (int k = 0; k < 2; k++) {
        if (i_this->mTimer[k] != 0)
            i_this->mTimer[k] = i_this->mTimer[k] - 1;
    }
    if (i_this->mTimer2 != 0)
        i_this->mTimer2 = i_this->mTimer2 - 1;

    gabi::call<u32>(0x025200D4); /* dComIfGp_get(): result unused (inlined accessor) */
    gabi::Local<dBgS_GndChk> gnd;
    dBgS_GndChk_ct(gnd, GNDCHK_VT, false);

    gabi::Local<cXyz> target;
    bool fall = hand_move(i_this, target, gnd);
    if (!fall) {
        control1(i_this, target);
    } else {
        cut_control(i_this);
    }
    control3(i_this, fall);

    gabi::Local<CcAtInfo_sitem> at;
    BOOL ok = hit_check(i_this, at);
    gndchk_dt(gnd);
    (void)ok;
    return TRUE;
}
VERIFY(0x024867D0, daSitem_Execute);
