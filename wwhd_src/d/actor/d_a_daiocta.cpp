/**
 * d_a_daiocta.cpp (WWHD)
 * Mini-Boss - Big Octo (body)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_daiocta.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * The eye actor (daDaiocta_Eye_c) is its own unit (d_a_daiocta_eye.cpp); the inline eye
 * accessors used here (rndRotEye, mbIsDead, ...) read its layout from d_a_daiocta_eye.h.
 */
#include "d/actor/d_a_daiocta.h"
#include "d/actor/d_a_daiocta_eye.h"

#include <cmath>

using gabi::load;
using gabi::store;

static inline u32 A(const void* p) { return gabi::ea(p); }
static inline daDaiocta_c* T(u32 a) { return gabi::at<daDaiocta_c>(a); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGp_get() (the play object singleton) */
static inline u32 oct_play() { return gabi::ea(gabi::call<void*>(0x025200D4)); }
static inline u32 oct_evm() { return oct_play() + 0x52C4; }
static inline void oct_assert(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
/* l_HIO fields */
static inline f32 hio_f(u32 off) { return load<f32>(DAIOCTA_HIO + off); }
static inline s16 hio_s(u32 off) { return load<s16>(DAIOCTA_HIO + off); }
static inline u8 hio_b(u32 off) { return load<u8>(DAIOCTA_HIO + off); }
static const u32 MTX_STACK = 0x1048D0CC; /* mDoMtx_stack_c::now */
/* dComIfG_getObjectRes(m_arc_name, idx) (HD: sead::SafeString temporary, this TU's vtable) */
static u32 oct_getRes(s32 idx) { return gabi::ea(dComIfG_getObjectRes(gabi::at<const char>(DAIOCTA_ARC), idx, DAIOCTA_SAFESTRING_VTBL)); }
/* J3DModel::getAnmMtx(jnt) (HD: the joint matrix block at +0x2C, marked dirty) */
static u32 oct_getAnmMtx(u32 model, u32 jnt) {
    u32 blk = load<u32>(model + 0x2C);
    u32 arr = load<u32>(blk + 0x10);
    u16 flags = load<u16>(blk + 4);
    store<u16>(blk + 4, (u16)(flags | 0x10));
    return jnt * 0x30 + arr;
}
/* a float copied with lfs/stfs and no arithmetic is bit-exact in the recompiled original (a
 * signalling NaN is not quieted): a word copy */
static inline void fcpy(u32 dst, u32 src) { store<u32>(dst, load<u32>(src)); }
/* mDoMtx_stack_c::multVecZero(dst): the translation column */
static void oct_multVecZero(u32 dst) {
    fcpy(dst + 0, MTX_STACK + 0x0C);
    fcpy(dst + 4, MTX_STACK + 0x1C);
    fcpy(dst + 8, MTX_STACK + 0x2C);
}
/* matrix assignment: twelve loads, then twelve stores (bit-exact) */
static void oct_mtxCopy(u32 dst, u32 src) {
    u32 v[12];
    for (int i = 0; i < 12; i++) v[i] = load<u32>(src + 4 * i);
    for (int i = 0; i < 12; i++) store<u32>(dst + 4 * i, v[i]);
}
static inline void oct_mtxCopyFromStack(u32 dst) { oct_mtxCopy(dst, MTX_STACK); }
static inline void oct_setBaseTRMtx(u32 model) { oct_mtxCopy(model + 0xC8, MTX_STACK); }
/* the same copy where the recompiled original keeps the values in FPRs (signalling NaNs quieted) */
static inline void oct_setBaseTRMtxQ(u32 model) { mtx_copy(gabi::at<Mtx34>(model + 0xC8), gabi::at<Mtx34>(MTX_STACK)); }
/* J3DModel::setBaseScale (scale at +0xBC) */
static void oct_setBaseScale(u32 model, u32 s) {
    u32 x = load<u32>(s), y = load<u32>(s + 4), z = load<u32>(s + 8);
    store<u32>(model + 0xBC, x);
    store<u32>(model + 0xC4, z);
    store<u32>(model + 0xC0, y);
}
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL oct_searchByID(u32 id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* fopAcM_SearchByName(fpcNm_SHIP_e) (HD inline: fpcSch_JudgeForPName with the name on the stack) */
static u32 oct_searchShip(gabi::Local<be<s16>>& key) {
    *key = (s16)0xA5;
    return gabi::ea(fopAcIt_Judge(0x025E121C, key.get()));
}
/* fopAcM_seStart(this, id, 0) (HD inline: reverb of the current room, sound at eyePos) */
static void oct_seStart(u32 t, u32 id, u32 pos) {
    s32 reverb = gabi::call<s32>(0x02520540, (s32)load<s8>(t + 0x326)); /* dComIfGp_getReverb */
    gabi::call(0x025E1A40, id, pos, 0u, reverb);
}
/* inline strcmp (GHS) */
static s32 oct_strcmp(u32 a, u32 b) {
    for (;;) {
        u8 x = load<u8>(a++);
        u8 y = load<u8>(b++);
        if (x != y) return (s32)x - (s32)y;
        if (x == 0) return 0;
    }
}
/* mDoExt_McaMorf::isStop() on the HD frame control (rate +0x98, state +0xA7) */
static bool oct_morfIsStop(u32 morf) {
    if (load<u8>(morf + 0xA7) & 1) return true;
    return load<f32>(morf + 0x98) == 0.0f;
}
/* dVibration_c (play+0x599C) */
static void oct_startShock(s32 strength, gabi::Local<cXyz>& pos) {
    u32 vib = oct_play() + 0x599C;
    pos->x = 0.0f;
    pos->y = 1.0f;
    pos->z = 0.0f;
    gabi::call(0x025CB374, vib, strength, -0x21, pos.get());
}
/* |a - b| in XZ: cXyz::operator- into a temporary, then (x, 0, z) and sqrt(PSVECSquareMag) */
static f32 oct_absXZ(u32 a, u32 b, gabi::Local<cXyz>& diff, gabi::Local<cXyz>& xz) {
    cXyz_mi(gabi::at<cXyz>(a), diff.get(), gabi::at<cXyz>(b));
    f32 x = diff->x;
    f32 z = diff->z;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    return std_sqrtf(PSVECSquareMag(xz.get()));
}
static f32 oct_abs(u32 a, u32 b, gabi::Local<cXyz>& diff) {
    cXyz_mi(gabi::at<cXyz>(a), diff.get(), gabi::at<cXyz>(b));
    return std_sqrtf(PSVECSquareMag(diff.get()));
}
/* setLightTevColorType / settingTevStruct with the env light reached through its accessor */
static void oct_settingTevStruct(u32 t, s32 type) {
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, type, gabi::at<cXyz>(t + 0x314), gabi::at<dKy_tevstr_c>(t + 0x110));
}

/* forward declarations (calls between the unit's functions are guest calls) */
static void modeProc(daDaiocta_c* i_this, s32 proc, s32 mode);
static void setEffect(daDaiocta_c* i_this, u32 name);
static void setAnm(daDaiocta_c* i_this);
static void setAwaRandom(daDaiocta_c* i_this, s32 i);
static void initAwa(daDaiocta_c* i_this);
static void setMtx(daDaiocta_c* i_this);
static void setSuikomiMtx(daDaiocta_c* i_this);
static void setAwaMtx(daDaiocta_c* i_this);
static void initMtx(daDaiocta_c* i_this);
static bool isDead(daDaiocta_c* i_this);
static bool isDamageEye(daDaiocta_c* i_this);
static bool isDamageBombEye(daDaiocta_c* i_this);
static s32 cLib_calcTimer_i(be<s32>* timer);

/* 0211889C */
static void modeProc(daDaiocta_c* i_this, s32 proc, s32 mode) {
    WWHD_FUNC(0x0211889C, void, i_this, proc, mode);
    const u32 mode_tbl = 0x1000CED8; /* {init, exec: 8-byte pointers to member, name}, 0x14 each */
    if (proc == daDaiocta_c::PROC_INIT_e) {
        if (mode >= 7) return; /* HD: bounds check */
        u32 e = mode_tbl + (u32)(mode * 0x14);
        i_this->mMode = mode;
        ptmf_call(e, i_this);
    } else if (proc == daDaiocta_c::PROC_EXEC_e) {
        s32 m = i_this->mMode;
        if (m >= 7) return; /* HD: bounds check */
        ptmf_call(mode_tbl + (u32)(m * 0x14) + 8, i_this);
    }
}
VERIFY(0x0211889C, modeProc);

/* 02118958 */
static void _coHit(daDaiocta_c* i_this, fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02118958, void, i_this, i_actor);
    u32 a = A(i_actor);
    if (a == 0) return;
    if (load<s16>(a + 8) != 0x126) return; /* fpcNm_BOMB_e */
    u32 m = (u32)(s32)i_this->mMode - 1;    /* APPEAR, WAIT, DAMAGE, DAMAGE_BOMB */
    if (m > 3) return;
    if (gabi::call<u32>(0x020CB92C, a, 4) == 0) return; /* daBomb_c::chk_state(STATE_4) */
    modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE_BOMB);
}
VERIFY(0x02118958, _coHit);

/* 021189CC: coHit_CB */
static void coHit_CB(daDaiocta_c* i_this, u32 objInf, fopAc_ac_c* i_actor, u32 objInf2) {
    WWHD_FUNC(0x021189CC, void, i_this, objInf, i_actor, objInf2);
    _coHit(i_this, i_actor);
}
VERIFY(0x021189CC, coHit_CB);

/* 021189D4 */
static void _nodeControl(daDaiocta_c* i_this, J3DNode* i_nodeP, J3DModel* i_modelP) {
    WWHD_FUNC(0x021189D4, void, i_this, i_nodeP, i_modelP);
    u32 t = A(i_this);
    u32 model = A(i_modelP);
    u32 jnt_no = load<u16>(A(J3DNode_toJoint(i_nodeP)) + 4);
    if ((s32)jnt_no >= 37) return; /* HD: bounds check */
    PSMTXCopy(gabi::at<Mtx34>(oct_getAnmMtx(model, jnt_no)), gabi::at<Mtx34>(MTX_STACK));
    oct_multVecZero(t + 0x3AC + jnt_no * 12);

    gabi::Local<cXyz> base;
    u32 dst = 0;
    switch (jnt_no) {
    case 30: base->x = 450.0f; base->y = 0.0f; base->z = 0.0f; dst = t + 0x2320; break; /* m21A0 */
    case 36: base->x = 450.0f; base->y = 0.0f; base->z = 0.0f; dst = t + 0x232C; break; /* m21AC */
    case 20: fcpy(A(base.get()), DAIOCTA_HIO + 0x94); base->y = 0.0f; base->z = 0.0f; dst = t + 0x25B4; break; /* m2434 */
    case 6:  base->x = 450.0f; base->y = 0.0f; base->z = 0.0f; dst = t + 0x2338; break; /* m21B8 */
    case 9:  base->x = 300.0f; base->y = -200.0f; base->z = 0.0f; dst = t + 0x2344; break; /* m21C4 */
    case 7:  base->x = 300.0f; base->y = 0.0f; base->z = 0.0f; dst = t + 0x2350; break; /* m21D0 */
    case 10: base->x = 300.0f; base->y = 200.0f; base->z = 0.0f; dst = t + 0x235C; break; /* m21DC */
    case 8:  base->x = 300.0f; base->y = 0.0f; base->z = 0.0f; dst = t + 0x2368; break; /* m21E8 */
    }
    if (dst != 0) PSMTXMultVec(gabi::at<Mtx34>(MTX_STACK), base.get(), gabi::at<cXyz>(dst));

    PSMTXCopy(gabi::at<Mtx34>(MTX_STACK), gabi::at<Mtx34>(0x104B4868)); /* j3dSys.mCurrentMtx */
    oct_mtxCopyFromStack(oct_getAnmMtx(model, jnt_no));                   /* setAnmMtx */
}
VERIFY(0x021189D4, _nodeControl);

/* 02118CE0 */
static BOOL nodeControl_CB(J3DNode* i_nodeP, int i_calcTiming) {
    WWHD_FUNC(0x02118CE0, BOOL, i_nodeP, i_calcTiming);
    if (i_calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 model = load<u32>(0x104B462C); /* j3dSys.getModel() */
        u32 daiocta = load<u32>(model + 0xB8);
        if (daiocta == 0) oct_assert(0x1000D068, 0x1A4, 0x1000D078);
        _nodeControl(T(daiocta), i_nodeP, gabi::at<J3DModel>(model));
    }
    return TRUE;
}
VERIFY(0x02118CE0, nodeControl_CB);

/* 02118D60 */
static BOOL createBodyHeap(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02118D60, BOOL, i_this);
    u32 t = A(i_this);
    u32 modelData = oct_getRes(0x11); /* DO_MAIN1 bdl */
    if (modelData == 0) oct_assert(0x1000D08C, 0x246, 0x1000D09C);
    /* HD: 02587688 is dLib_brkInit (the matcher swapped it with dLib_btkInit) */
    if (gabi::call<u32>(0x02587688, modelData, t + 0x6AC, DAIOCTA_ARC, 0x20) == 0) return FALSE;
    u32 morf = gabi::call<u32>(0x025E4F64, 0u, modelData, 0u, 0u, 0u, -1, 1.0f, 0, -1, 1, 0u, 0x80000u, 0x11000222u);
    store<u32>(t + 0x6A8, morf);
    if (morf == 0 || load<u32>(morf + 0x90) == 0) return FALSE;
    store<u32>(load<u32>(morf + 0x90) + 0xB8, t); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x02118D60, createBodyHeap);

/* 02118E90 */
static BOOL createSuikomiHeap(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02118E90, BOOL, i_this);
    u32 t = A(i_this);
    u32 modelData = oct_getRes(0x13); /* GDO_SUI00 bdl */
    if (modelData == 0) oct_assert(0x1000D0B0, 0x22A, 0x1000D0C0);
    u32 model = A(mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11000222));
    store<u32>(t + 0x284C, model);
    if (model == 0) return FALSE;
    if (gabi::call<u32>(0x02587688, modelData, t + 0x2850, DAIOCTA_ARC, 0x1D) == 0 || /* dLib_brkInit */
        gabi::call<u32>(0x02587740, modelData, t + 0x28C8, DAIOCTA_ARC, 0x26) == 0)   /* dLib_btkInit */
        return FALSE;
    return TRUE;
}
VERIFY(0x02118E90, createSuikomiHeap);

/* 02118F90 */
static BOOL createAwaHeap(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02118F90, BOOL, i_this);
    u32 t = A(i_this);
    u32 modelData = oct_getRes(0x12); /* GAWA00 bdl */
    if (modelData == 0) oct_assert(0x1000D0D4, 0x207, 0x1000D0E4);
    u32 bck = oct_getRes(0x0A);
    if (bck == 0) oct_assert(0x1000D0D4, 0x20B, 0x1000D0F8);
    u32 btk = oct_getRes(0x25);
    if (btk == 0) oct_assert(0x1000D0D4, 0x20F, 0x1000D104);
    u32 brk = oct_getRes(0x1C);
    if (brk == 0) oct_assert(0x1000D0D4, 0x213, 0x1000D110);
    for (int i = 0; i < 30; i++) {
        u32 model = A(mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11000222));
        store<u32>(t + 0x293C + 4 * i, model);
        if (model == 0) return FALSE;
        /* mDoExt_bckAnm::init(data, bck, TRUE, EMode_NONE, 1.0f, 0, -1, false) */
        if (gabi::call<u32>(0x025E8508, t + 0x29B4 + i * 0x8C, modelData, bck, 1, 0, 1.0f, 0, -1, 0) == 0) return FALSE;
        /* mDoExt_btkAnm::init (HD: 9th argument on the stack) */
        if (gabi::call<u32>(0x025E7CE0, t + 0x3A1C + i * 0x74, modelData, btk, 1, 0, 1.0f, 0, -1, 0, 0) == 0) return FALSE;
        /* mDoExt_brkAnm::init (HD: 9th argument on the stack) */
        if (gabi::call<u32>(0x025E8154, t + 0x47B4 + i * 0x78, modelData, brk, 1, 0, 1.0f, 0, -1, 0, 0) == 0) return FALSE;
    }
    return TRUE;
}
VERIFY(0x02118F90, createAwaHeap);

/* 021191DC */
static BOOL createArrowHitHeap(daDaiocta_c* i_this) {
    WWHD_FUNC(0x021191DC, BOOL, i_this);
    u32 t = A(i_this);
    u32 model = load<u32>(load<u32>(t + 0x6A8) + 0x90);
    u32 jh = gabi::call<u32>(0x02552B60, model, 0x101B3F84u, 0x11); /* JntHit_create(model, search_data, 17) */
    store<u32>(t + 0x2848, jh);
    if (jh == 0) return FALSE;
    store<u32>(t + 0x36C, jh); /* fopAcM_SetJntHit */
    return TRUE;
}
VERIFY(0x021191DC, createArrowHitHeap);

/* 02119248 */
static BOOL _createHeap(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119248, BOOL, i_this);
    if (!createBodyHeap(i_this)) return FALSE;
    if (!createSuikomiHeap(i_this)) return FALSE;
    if (!createAwaHeap(i_this)) return FALSE;
    if (!createArrowHitHeap(i_this)) return FALSE;
    return TRUE;
}
VERIFY(0x02119248, _createHeap);

/* 021192C8: createHeap_CB */
static BOOL createHeap_CB(daDaiocta_c* i_this) {
    WWHD_FUNC(0x021192C8, BOOL, i_this);
    return _createHeap(i_this);
}
VERIFY(0x021192C8, createHeap_CB);

/* 021192CC */
static void getArg(daDaiocta_c* i_this) {
    WWHD_FUNC(0x021192CC, void, i_this);
    u32 t = A(i_this);
    s16 prm2 = load<s16>(t + 0x2FC); /* home.angle.z */
    u32 prm = load<u32>(t + 0xB0);
    store<s16>(t + 0x2FC, 0);
    store<u8>(t + 0x68F, (u8)prm);         /* mOctoType */
    store<u8>(t + 0x691, (u8)(prm >> 16)); /* m575 */
    store<s16>(t + 0x324, 0);              /* current.angle.z */
    store<s16>(t + 0x32C, 0);              /* shape_angle.z */
    store<u8>(t + 0x690, (u8)(prm >> 24)); /* mSwitchNo */
    store<u8>(t + 0x698, (u8)prm2);        /* m057C */
    u32 mult = (prm >> 8) & 0xFF;
    if (mult == 0xFF) {
        fcpy(t + 0x694, DAIOCTA_HIO + 0x78); /* mDefaultAppearRadius */
    } else {
        i_this->mAppearRadius = (f32)mult * 100.0f;
    }
    u8 type = i_this->mOctoType;
    if (type > 3) {
        type = 0;
        i_this->mOctoType = 0;
    }
    switch (type) {
    case 0: /* FOUR_EYED */
        i_this->mDemoEndTimer = 0;
        i_this->mEyeAlloc[0] = 1;
        i_this->mEyeAlloc[1] = 1;
        i_this->mEyeAlloc[2] = 1;
        i_this->mEyeAlloc[5] = 1;
        break;
    case 1: /* EIGHT_EYED */
        for (int i = 0; i < 8; i++) i_this->mEyeAlloc[i] = 1;
        i_this->mDemoEndTimer = 0;
        break;
    case 2: /* TWELVE_EYED */
        for (int i = 0; i < 12; i++) i_this->mEyeAlloc[i] = 1;
        i_this->mDemoEndTimer = 0;
        break;
    default:
        i_this->mDemoEndTimer = 0;
        break;
    }
}
VERIFY(0x021192CC, getArg);

/* 021193F8 */
static void setAwaRandom(daDaiocta_c* i_this, s32 i_awaIndex) {
    WWHD_FUNC(0x021193F8, void, i_this, i_awaIndex);
    u32 t = A(i_this);
    f32 f12 = hio_f(0xF0);
    f32 f31 = hio_f(0xEC);
    f32 f30 = hio_f(0xE4);
    f32 f0 = hio_f(0xE8);
    f32 f1 = gabi::fsubs_ppc(f12, f31);
    f32 f29 = gabi::fsubs_ppc(f0, f30);
    u32 sc = t + 0x57A4 + (u32)(i_awaIndex * 12);
    f32 s = gabi::fadds_ppc(cM_rndF(f1), f31);
    store<f32>(sc + 0, s); /* mAwaScale[i].setall() */
    store<f32>(sc + 4, s);
    store<f32>(sc + 8, s);
    f32 dist = gabi::fadds_ppc(cM_rndF(f29), f30);
    u16 theta = (u16)gabi::ftoi(cM_rndF(65536.0f));
    u32 tr = t + 0x563C + (u32)(i_awaIndex * 12);
    store<f32>(tr + 0, gabi::fmadds(dist, cM_ssin(theta), load<f32>(t + 0x314)));
    fcpy(tr + 4, t + 0x25B0);
    store<f32>(tr + 8, gabi::fmadds(dist, cM_scos(theta), load<f32>(t + 0x31C)));
}
VERIFY(0x021193F8, setAwaRandom);

/* 02119514 */
static void initAwa(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119514, void, i_this);
    u32 t = A(i_this);
    for (int i = 0; i < hio_s(0x16); i++) {
        setAwaRandom(i_this, i);
        f32 f0 = hio_f(0xF8);
        f32 f31 = hio_f(0xF4);
        f32 r = cM_rndF(gabi::fsubs_ppc(f0, f31));
        store<s32>(t + 0x55C4 + 4 * i, gabi::ftoi(gabi::fadds_ppc(r, f31)));
        /* setFrame(getEndFrame()) on the bck, btk and brk animations */
        u32 bck = t + 0x29B4 + i * 0x8C;
        store<f32>(bck + 4, (f32)load<s16>(bck + 0xA));
        u32 btk = t + 0x3A1C + i * 0x74;
        store<f32>(btk + 4, (f32)load<s16>(btk + 0xA));
        u32 brk = t + 0x47B4 + i * 0x78;
        store<f32>(brk + 4, (f32)load<s16>(brk + 0xA));
    }
}
VERIFY(0x02119514, initAwa);

/* 02119630 */
static void setSuikomiMtx(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119630, void, i_this);
    u32 t = A(i_this);
    PSMTXTrans(gabi::at<Mtx34>(MTX_STACK), load<f32>(t + 0x314), load<f32>(t + 0x25B0), load<f32>(t + 0x31C));
    mDoMtx_ZXYrotM(gabi::at<Mtx34>(MTX_STACK), load<s16>(t + 0x328), load<s16>(t + 0x32A), load<s16>(t + 0x32C));
    mDoMtx_stack_transM(hio_f(0xFC), hio_f(0x100), hio_f(0x104));
    oct_setBaseTRMtx(load<u32>(t + 0x284C));
}
VERIFY(0x02119630, setSuikomiMtx);

/* 0211970C: setAwaMtx (unnamed by the matcher) */
static void setAwaMtx(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211970C, void, i_this);
    u32 t = A(i_this);
    for (int i = 0; i < hio_s(0x16); i++) {
        u32 model = load<u32>(t + 0x293C + 4 * i);
        /* here the recompiled copy goes through the FPRs (a signalling NaN is quieted) */
        J3DModel_setBaseScale(gabi::at<J3DModel>(model), gabi::at<cXyz>(t + 0x57A4 + i * 12));
        u32 tr = t + 0x563C + i * 12;
        PSMTXTrans(gabi::at<Mtx34>(MTX_STACK), load<f32>(tr + 0), load<f32>(tr + 4), load<f32>(tr + 8));
        oct_setBaseTRMtxQ(load<u32>(t + 0x293C + 4 * i));
    }
}
VERIFY(0x0211970C, setAwaMtx);

/* 0211980C */
static void setMtx(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211980C, void, i_this);
    u32 t = A(i_this);
    gabi::Local<be<u32>> actor_p;
    gabi::Local<Mtx34> mtx;
    for (int i = 0; i < 12; i++) {
        if (load<u8>(t + 0x25F0 + i) != 0 && oct_searchByID(load<u32>(t + 0x25C0 + 4 * i), actor_p.get())) {
            u32 model = load<u32>(load<u32>(t + 0x6A8) + 0x90);
            u32 idx = load<u32>(t + 0x5FC + 4 * i);
            PSMTXCopy(gabi::at<Mtx34>(oct_getAnmMtx(model, idx)), gabi::at<Mtx34>(MTX_STACK));
            oct_multVecZero(*actor_p + 0x314);
            gabi::call(0x025F20B0, MTX_STACK, mtx.get());          /* cMtx_inverseTranspose */
            gabi::call(0x025F232C, mtx.get(), *actor_p + 0x328);   /* mDoMtx_MtxToRot(mtx, &shape_angle) */
        }
    }
    PSMTXTrans(gabi::at<Mtx34>(MTX_STACK), load<f32>(t + 0x314), load<f32>(t + 0x318), load<f32>(t + 0x31C));
    mDoMtx_ZXYrotM(gabi::at<Mtx34>(MTX_STACK), load<s16>(t + 0x328), load<s16>(t + 0x32A), load<s16>(t + 0x32C));
    oct_setBaseTRMtxQ(load<u32>(load<u32>(t + 0x6A8) + 0x90));
    setSuikomiMtx(i_this);
    setAwaMtx(i_this);
}
VERIFY(0x0211980C, setMtx);

/* 02119990 */
static void initMtx(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119990, void, i_this);
    u32 t = A(i_this);
    oct_setBaseScale(load<u32>(load<u32>(t + 0x6A8) + 0x90), t + 0x330);
    oct_setBaseScale(load<u32>(t + 0x284C), t + 0x330);
    setMtx(i_this);
    J3DModel_calc(gabi::at<J3DModel>(load<u32>(load<u32>(t + 0x6A8) + 0x90)));
    J3DModel_calc(gabi::at<J3DModel>(load<u32>(t + 0x284C)));
}
VERIFY(0x02119990, initMtx);

/* 02119A0C */
static void createInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119A0C, void, i_this);
    u32 t = A(i_this);
    /* m220C = current.pos (integer copy) */
    u32 px = load<u32>(t + 0x314), py = load<u32>(t + 0x318), pz = load<u32>(t + 0x31C);
    store<u32>(t + 0x2390, py);
    store<u32>(t + 0x2394, pz);
    store<u32>(t + 0x238C, px);
    initAwa(i_this);
    modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_HIDE);
    initMtx(i_this);
    /* shape_angle = current.angle; attention_info.flags = 0 */
    u16 ay = load<u16>(t + 0x322), ax = load<u16>(t + 0x320), az = load<u16>(t + 0x324);
    store<u32>(t + 0x39C, 0);
    store<u16>(t + 0x32C, az);
    store<u16>(t + 0x328, ax);
    store<u16>(t + 0x32A, ay);
    static const u32 eye_jnt[12] = {0x15, 0x16, 0x0F, 0x17, 0x18, 0x10, 0x11, 0x12, 0x0E, 0x0B, 0x0C, 0x0D};
    for (int i = 0; i < 12; i++) i_this->mAnmMtxIndices[i] = eye_jnt[i];
    for (u32 i = 0; i < 37; i++) {
        /* getJointNodePointer(i)->setCallBack(nodeControl_CB) (HD: sead buffer access, out of
         * range -> element 0) */
        u32 data = load<u32>(load<u32>(load<u32>(t + 0x6A8) + 0x90) + 0xAC);
        u32 n = load<u32>(data + 4);
        u32 p = load<u32>(data + 8);
        if ((i & 0xFFFF) < n) p += (i & 0xFFFF) * 0x1C;
        store<u32>(p + 8, 0x02118CE0u);
    }
    gabi::call(0x024EFF44, t + 0x255C, 30.0f, 30.0f); /* mAcchCir.SetWall(30, 30) */
    /* mAcch.Set(&pos, &old.pos, this, 1, &mAcchCir, &speed) (HD: extra argument on the stack) */
    gabi::call(0x024F06B4, t + 0x2398, t + 0x314, t + 0x300, t, 1, t + 0x255C, t + 0x33C, 0u, 0u);
    /* SetWallNone, SetRoofNone, OnSeaCheckOn, OnSeaWaterHeight */
    store<u32>(t + 0x23C0, load<u32>(t + 0x23C0) | 0x6000C);
    u32 model = load<u32>(load<u32>(t + 0x6A8) + 0x90);
    store<u32>(t + 0x348, model != 0 ? model + 0xC8 : 0); /* fopAcM_SetMtx(getBaseTRMtx()) */
    fopAcM_setCullSizeBox(i_this, -2300.0f, -500.0f, -2300.0f, 2300.0f, 2300.0f, 2300.0f);
    store<f32>(t + 0x364, 10.0f); /* fopAcM_setCullSizeFar */
    for (int i = 0; i < 12; i++) {
        if (load<u8>(t + 0x25F0 + i) != 0) {
            /* fopAcM_createChild(DAIOCTA_EYE, fpcM_GetID(this), -1, &pos, roomNo, NULL, NULL, -1, NULL) */
            u32 id = gabi::call<u32>(0x025D5A20, 0xE2, load<u32>(t + 4), -1, t + 0x314, (s32)load<s8>(t + 0x1C9), 0u, 0u, -1, 0u);
            store<u32>(t + 0x25C0 + 4 * i, id);
        }
    }
    i_this->mAuzuId = fopAcM_create(0x11B, 0x1100FF, gabi::at<cXyz>(t + 0x314), (s32)load<s8>(t + 0x1C9), nullptr, nullptr, -1, 0);
    gabi::call(0x02515F14, t + 0x22E4, 0, 0, t); /* mStts.Init(0, 0, this) */
    for (int i = 0; i < 6; i++) {
        u32 sph = t + 0x724 + i * 0x12C;
        gabi::call(0x0251677C, sph, 0x1000D424u); /* dCcD_Sph::Set(m_sph_src) */
        store<u32>(sph + 0x44, t + 0x22E4);       /* SetStts */
        store<u32>(sph + 0xE4, 0x021189CCu);      /* SetCoHitCallback(coHit_CB) */
    }
    for (int i = 0; i < 17; i++) {
        u32 cps = t + 0xE2C + i * 0x138;
        gabi::call(0x025164C0, cps, 0x1000D464u); /* dCcD_Cps::Set(m_cps_src) */
        store<u32>(cps + 0x44, t + 0x22E4);
        store<u32>(cps + 0xE4, 0x021189CCu);
    }
    i_this->mPrmIdx = 1;
}
VERIFY(0x02119A0C, createInit);

static inline void __construct_array(u32 p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(u32 p, u32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0u, 0u); }

/* 02119CE8 */
static cPhs_State _create(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119CE8, cPhs_State, i_this);
    u32 t = A(i_this);
    /* fopAcM_ct(this, daDaiocta_c) */
    u32 cond = load<u32>(t + 0x2E4);
    if (!(cond & 8)) {
        if (t != 0) {
            gabi::call(0x025D4ED0, t);             /* fopAc_ac_c::fopAc_ac_c */
            store<u32>(t + 0xB4, DAIOCTA_VTBL);
            gabi::call(0x025E80D0, t + 0x6AC);     /* mDoExt_brkAnm::mDoExt_brkAnm */
            __construct_array(t + 0x724, 6, 0x12C, 0x025166F0);  /* dCcD_Sph */
            __construct_array(t + 0xE2C, 17, 0x138, 0x0211D08C); /* dCcD_Cps (this TU's copy) */
            dCcD_Stts_ct(gabi::at<dCcD_Stts>(t + 0x22E4));
            dBgS_ObjAcch_ct(gabi::at<dBgS_Acch>(t + 0x2398), dBgS_ObjAcch_vt{0x1000D004, 0x1000D024, 0x1000D014});
            gabi::call(0x024EFE94, t + 0x255C);            /* dBgS_AcchCir::dBgS_AcchCir */
            gabi::call(0x025A5894, t + 0x259C, 0u, 0u);    /* dPa_followEcallBack */
            gabi::call(0x025E80D0, t + 0x2850);            /* mDoExt_brkAnm */
            gabi::call(0x025E7C6C, t + 0x28C8);            /* mDoExt_btkAnm */
            __construct_array(t + 0x29B4, 30, 0x8C, 0x0211D118); /* mDoExt_bckAnm (this TU's copy) */
            __construct_array(t + 0x3A1C, 30, 0x74, 0x025E7C6C); /* mDoExt_btkAnm */
            __construct_array(t + 0x47B4, 30, 0x78, 0x025E80D0); /* mDoExt_brkAnm */
            cond = load<u32>(t + 0x2E4);
        }
        store<u32>(t + 0x2E4, cond | 8);
    }
    cPhs_State state = dComIfG_resLoad(gabi::at<request_of_phase_process_class>(t + 0x6A0), gabi::at<const char>(DAIOCTA_ARC));
    if (state == cPhs_COMPLEATE_e) {
        getArg(i_this);
        u8 sw = i_this->mSwitchNo;
        if (sw != 0xFF && dComIfGs_isSwitch(sw, (s32)load<s8>(t + 0x326))) return cPhs_ERROR_e;
        if (!fopAcM_entrySolidHeap(i_this, 0x021192C8, 0xF740)) return cPhs_ERROR_e;
        createInit(i_this);
    }
    return state;
}
VERIFY(0x02119CE8, _create);

/* 02119EE8 */
static cPhs_State daDaioctaCreate(void* i_this) {
    WWHD_FUNC(0x02119EE8, cPhs_State, i_this);
    return _create((daDaiocta_c*)i_this);
}
VERIFY(0x02119EE8, daDaioctaCreate);

/* 02119EEC */
static bool _delete(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119EEC, bool, i_this);
    u32 t = A(i_this);
    dComIfG_resDelete(gabi::at<request_of_phase_process_class>(t + 0x6A0), gabi::at<const char>(DAIOCTA_ARC));
    i_this->mParticleCallback.remove();
    gabi::Local<be<u32>> actor_p;
    for (int i = 0; i < 12; i++) {
        if (oct_searchByID(load<u32>(t + 0x25C0 + 4 * i), actor_p.get())) fopAcM_delete(gabi::at<fopAc_ac_c>(*actor_p));
    }
    mDoAud_seDeleteObject(gabi::at<cXyz>(t + 0x25B4));
    return true;
}
VERIFY(0x02119EEC, _delete);

/* 02119F80 */
static BOOL daDaioctaDelete(void* i_this) {
    WWHD_FUNC(0x02119F80, BOOL, i_this);
    return _delete((daDaiocta_c*)i_this);
}
VERIFY(0x02119F80, daDaioctaDelete);

/* 02119F84 */
static void execAwa(daDaiocta_c* i_this) {
    WWHD_FUNC(0x02119F84, void, i_this);
    u32 t = A(i_this);
    for (int i = 0; i < hio_s(0x16); i++) {
        if (cLib_calcTimer_i(gabi::at<be<s32>>(t + 0x55C4 + 4 * i)) == 0) {
            u32 bck = t + 0x29B4 + i * 0x8C;
            mDoExt_baseAnm_play(gabi::at<void>(bck));
            u32 btk = t + 0x3A1C + i * 0x74;
            mDoExt_baseAnm_play(gabi::at<void>(btk));
            u32 brk = t + 0x47B4 + i * 0x78;
            mDoExt_baseAnm_play(gabi::at<void>(brk));
            if (i_this->m31C4 != 0 || hio_b(0x8) != 0) {
                /* mAwaBckAnms[i].isStop() */
                if ((load<u8>(bck + 0xF) & 1) || load<f32>(bck + 0) == 0.0f) {
                    store<f32>(bck + 0, 1.0f); /* setPlaySpeed(1.0f) */
                    store<f32>(bck + 4, 0.0f); /* setFrame(0.0f) */
                    store<f32>(btk + 4, 0.0f);
                    store<f32>(btk + 0, 1.0f);
                    store<f32>(brk + 4, 0.0f);
                    store<f32>(brk + 0, 1.0f);
                    f32 f29 = hio_f(0xF4);
                    f32 f13 = hio_f(0xF8);
                    f32 r = cM_rndF(gabi::fsubs_ppc(f13, f29));
                    store<s32>(t + 0x55C4 + 4 * i, gabi::ftoi(gabi::fadds_ppc(r, f29)));
                    setAwaRandom(i_this, i);
                }
            }
        }
    }
}
VERIFY(0x02119F84, execAwa);

/* 0211A104 */
static void setWater(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211A104, void, i_this);
    u32 t = A(i_this);
    f32 h = load<f32>(t + 0x2398 + 0xD0); /* mAcch.GetSeaHeight() */
    if (h == -1000000000.0f) h = 0.0f;     /* -G_CM3D_F_INF */
    i_this->mWaterY = h;
}
VERIFY(0x0211A104, setWater);

/* the eye search of isLivingEye/isDead/isDamageEye/isDamageBombEye/setRotEye: fopAcM_SearchByID inline */
static inline u32 oct_eye(u32 t, int i) { return A(fopAcM_SearchByID(load<u32>(t + 0x25C0 + 4 * i))); }

/* 0211A128 */
static bool isLivingEye(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211A128, bool, i_this);
    u32 t = A(i_this);
    bool is_alive = false;
    for (int i = 0; i < 12; i++) {
        if (load<u8>(t + 0x25F0 + i) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye == 0) {
                store<u8>(t + 0x25F0 + i, 0);
            } else if (gabi::at<daDaiocta_Eye_c>(eye)->dead == 0) {
                is_alive = true;
            }
        }
    }
    return is_alive;
}
VERIFY(0x0211A128, isLivingEye);

/* rndRotEye(): one axis (target, current, minimum and maximum rotation at the given offsets) */
static void oct_rndRotEyeAxis(u32 eye, u32 tgt, u32 cur, u32 mn, u32 mx, bool storeNo, s32 no) {
    s16 max = load<s16>(eye + mx);
    if (storeNo) gabi::at<daDaiocta_Eye_c>(eye)->eyeNumber = no; /* mEyeNo = i */
    s16 v = (s16)gabi::ftoi(cM_rndF((f32)max));
    s16 c = load<s16>(eye + cur);
    s16 m = load<s16>(eye + mn);
    store<s16>(eye + tgt, v);
    if (std::fabs((f32)((s32)v - (s32)c)) < (f32)m) store<s16>(eye + tgt, (s16)(v + m));
}

/* 0211A1E8 */
static void setRotEye(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211A1E8, void, i_this);
    u32 t = A(i_this);
    if (cLib_calcTimer_i(&i_this->mRotEyeTimer) == 0) {
        for (int i = 0; i < 12; i++) {
            if (load<u8>(t + 0x25F0 + i) != 0) {
                u32 eye = oct_eye(t, i);
                if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->dead == 0) {
                    /* targetRotation 0x4BC, eyeRotation 0x4C2, rotationMinimum 0x4C8, rotationMaximum 0x4CE */
                    oct_rndRotEyeAxis(eye, 0x4BC, 0x4C2, 0x4C8, 0x4CE, true, i);
                    oct_rndRotEyeAxis(eye, 0x4BE, 0x4C4, 0x4CA, 0x4D0, false, 0);
                    oct_rndRotEyeAxis(eye, 0x4C0, 0x4C6, 0x4CC, 0x4D2, false, 0);
                }
            }
        }
        f32 r = cM_rndF((f32)hio_s(0xE)); /* mRotEyeTimerAddMax */
        i_this->mRotEyeTimer = gabi::ftoi(gabi::fadds_ppc((f32)hio_s(0xC), r));
    }
}
VERIFY(0x0211A1E8, setRotEye);

/* 0211A4BC */
static void setCollision(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211A4BC, void, i_this);
    u32 t = A(i_this);
    u32 c = t + 0x3AC; /* mSphCenters */
    for (u32 i = 0; i < 6; i++) {
        u32 j = load<u8>(0x1000D150 + i); /* switch table {2, 3, 4, 5, 6, 20} */
        u32 sph = t + 0x724 + i * 0x12C;
        gabi::call(0x02018C8C, sph + 0x118, hio_f(0x1C + 4 * i)); /* SetR(mSphRadii[i]) */
        gabi::call(0x02018D40, sph + 0x118, c + j * 12);          /* SetC(mSphCenters[j]) */
        dComIfG_Ccsp_Set(gabi::at<void>(sph));
    }
    for (u32 i = 0; i < 10; i++) {
        u32 j = i < 5 ? 25 + i : 26 + i;
        u32 cps = t + 0xE2C + i * 0x138;
        gabi::call(0x02018808, cps + 0x118, c + j * 12, c + (j + 1) * 12); /* SetStartEnd */
        fcpy(cps + 0x134, DAIOCTA_HIO + 0x34 + 4 * i);                      /* SetR(mCpsRadii[i]) */
        dComIfG_Ccsp_Set(gabi::at<void>(cps));
    }
    struct { u32 cps, start, end, r; } tail[7] = {
        {10, 36 * 12, 0x232C, 0x5C}, /* m21AC */
        {11, 30 * 12, 0x2320, 0x60}, /* m21A0 */
        {12, 6 * 12, 0x2338, 0x64},  /* m21B8 */
        {13, 10 * 12, 0x235C, 0x68}, /* m21DC */
        {14, 8 * 12, 0x2368, 0x6C},  /* m21E8 */
        {15, 9 * 12, 0x2344, 0x70},  /* m21C4 */
        {16, 7 * 12, 0x2350, 0x74},  /* m21D0 */
    };
    for (int k = 0; k < 7; k++) {
        u32 cps = t + 0xE2C + tail[k].cps * 0x138;
        gabi::call(0x02018808, cps + 0x118, c + tail[k].start, t + tail[k].end);
        fcpy(cps + 0x134, DAIOCTA_HIO + tail[k].r);
        dComIfG_Ccsp_Set(gabi::at<void>(cps));
    }
}
VERIFY(0x0211A4BC, setCollision);

/* 0211A9A0 (the name is passed in a full register: compared unextended) */
static void setEffect(daDaiocta_c* i_this, u32 effect_name) {
    WWHD_FUNC(0x0211A9A0, void, i_this, effect_name);
    u32 t = A(i_this);
    gabi::Local<GXColor> amb;
    gabi::Local<GXColor> dif;
    gabi::Local<cXyz> pos;
    gabi::Local<csXyz> ang;
    gabi::call(0x025602F0, amb.get(), dif.get()); /* dKy_get_seacolor */
    s32 add = hio_s(0x18);
    u32 ca = A(amb.get());
    s32 r = load<u8>(ca + 0) + add;
    s32 g = load<u8>(ca + 1) + add;
    s32 b = load<u8>(ca + 2) + add;
    if (r >= 0xFF) r = 0xFF;
    if (g >= 0xFF) g = 0xFF;
    if (b >= 0xFF) b = 0xFF;
    u32 pp = A(pos.get());
    fcpy(pp + 0, t + 0x314);
    fcpy(pp + 4, t + 0x25B0);
    fcpy(pp + 8, t + 0x31C);
    u32 pa = A(ang.get());
    store<u16>(pa + 0, load<u16>(t + 0x328));
    store<u16>(pa + 2, load<u16>(t + 0x32A));
    store<u16>(pa + 4, load<u16>(t + 0x32C));
    store<u8>(ca + 0, (u8)r);
    store<u8>(ca + 2, (u8)b);
    store<u8>(ca + 1, (u8)g);
    if (effect_name < 0x81FE) return;
    if (effect_name == 0x81FE) { /* ID_IT_SN_DO_APPSHIBUKI00 */
        if (load<u32>(t + 0x25A0) == 0) {
            dComIfGp_particle_set((u16)effect_name, pos.get(), nullptr, nullptr, 0xFF,
                                  gabi::at<dPa_levelEcallBack>(t + 0x259C), -1, amb.get());
        }
    } else if (effect_name <= 0x8205) { /* HIT{A,B,C}_SHIBUKI{A,B}00, DOWNSHIBUKI00 */
        dComIfGp_particle_set((u16)effect_name, pos.get(), nullptr, nullptr, 0xFF, nullptr, -1, amb.get());
    } else if ((effect_name >= 0x8207 && effect_name <= 0x8209) || effect_name == 0x82CC) {
        /* SUIKOMI{A,B,C}00, HAKIDASHI00 */
        dComIfGp_particle_set((u16)effect_name, pos.get(), ang.get(), nullptr, 0xFF, nullptr, -1, amb.get());
    }
}
VERIFY(0x0211A9A0, setEffect);

/* 0211AB80 */
static void setAnm(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211AB80, void, i_this);
    u32 t = A(i_this);
    /* dLib_anm_prm_c anm_prms[9] (0x10 each: morf at +4, speed at +8), copied from .data */
    gabi::Local<u8[0x90]> prms;
    u32 p = A(prms.get());
    for (u32 k = 0; k < 0x90; k += 4) store<u32>(p + k, load<u32>(0x1000D1F8 + k));
    for (u32 i = 1; i < 9; i++) {
        fcpy(p + i * 0x10 + 4, DAIOCTA_HIO + 0xBC + 4 * i); /* m0BC[i] */
        fcpy(p + i * 0x10 + 8, DAIOCTA_HIO + 0x98 + 4 * i); /* m098[i] */
    }
    u32 morf = load<u32>(t + 0x6A8);
    u32 morf_model_p = load<u32>(morf + 0x90);
    s8 anm = i_this->mAnmIdx;
    bool check6 = true;
    if (anm == 3) {
        if (load<f32>(morf + 0x9C) == 10.0f || load<f32>(morf + 0x9C) == 25.0f || load<f32>(morf + 0x9C) == 45.0f) {
            gabi::Local<cXyz> shockPos;
            oct_startShock(6, shockPos);
            anm = i_this->mAnmIdx;
        } else {
            check6 = false;
        }
    }
    if (check6 && anm == 6) {
        morf = load<u32>(t + 0x6A8);
        if (load<f32>(morf + 0x9C) == 1.0f) {
            setEffect(i_this, 0x82CC); /* ID_IT_SN_DO_HAKIDASHI00 */
            morf = load<u32>(t + 0x6A8);
        }
        if (load<f32>(morf + 0x9C) == 26.0f) {
            gabi::Local<cXyz> shockPos;
            oct_startShock(7, shockPos);
        }
    }
    s8 prm = i_this->mPrmIdx;
    s8 old = i_this->mOldPrmIdx;
    s32 idx = load<s32>(0x1000D1C8 + (u32)(prm * 4)); /* a_brk_anm_prm_tbl[mPrmIdx] */
    if (old != prm && idx != -1) {
        u32 brk = oct_getRes(load<s32>(0x1000D1AC + (u32)(idx * 4))); /* a_brk_anm_idx_tbl */
        if (brk == 0) oct_assert(0x1000D19C, 0x727, 0x1000D1EC);
        /* mBrkAnm1.init(modelData, brk, TRUE, EMode_LOOP, 1.0f, 0, -1, true, FALSE) */
        gabi::call(0x025E8154, t + 0x6AC, load<u32>(morf_model_p + 0xAC), brk, 1, 2, 1.0f, 0, -1, 1, 0);
    }
    /* dLib_setAnm(arc, morf, &mAnmIdx, &mPrmIdx, &mOldPrmIdx, a_anm_idx_tbl, anm_prms, false) (HD: one
     * more argument on the stack) */
    gabi::call(0x025877F8, DAIOCTA_ARC, load<u32>(t + 0x6A8), t + 0x68C, t + 0x68D, t + 0x68E, 0x1000D164u, p, 0u, 0u);
    fcpy(t + 0x6B0, load<u32>(t + 0x6A8) + 0x9C); /* mBrkAnm1.setFrame(mpMorf->getFrame()) */
}
VERIFY(0x0211AB80, setAnm);

/* 0211AE68 */
static bool _execute(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211AE68, bool, i_this);
    u32 t = A(i_this);
    s32 mode = i_this->mMode;
    if (mode == daDaiocta_c::MODE_DELETE) {
        execAwa(i_this);
        mode = i_this->mMode;
    }
    if (mode != daDaiocta_c::MODE_HIDE) {
        if (mode != daDaiocta_c::MODE_APPEAR) {
            f32 dist = fopAcM_searchActorDistanceXZ(i_this, dComIfGp_getPlayer(0));
            s16 angle_y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            s32 dist_angle_s = cLib_distanceAngleS((s16)(load<s16>(t + 0x32A) + hio_s(0x14)), angle_y);
            if (dist < hio_f(0x84)) {
                s32 m = i_this->mMode;
                if (m != daDaiocta_c::MODE_DEMO && m != daDaiocta_c::MODE_DELETE && dist_angle_s < hio_s(0x12)) {
                    modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DEMO);
                }
            }
            mode = i_this->mMode;
        }
        if (mode != daDaiocta_c::MODE_HIDE) {
            setWater(i_this);
            mDoExt_baseAnm_play(gabi::at<void>(t + 0x2850));
            mDoExt_baseAnm_play(gabi::at<void>(t + 0x28C8));
            /* attention_info.position = eyePos = current.pos (integer copies) */
            u32 x = load<u32>(t + 0x314), y = load<u32>(t + 0x318);
            store<u32>(t + 0x390, x);
            u32 z = load<u32>(t + 0x31C);
            store<u32>(t + 0x394, y);
            store<u32>(t + 0x398, z);
            store<u32>(t + 0x37C, x);
            store<u32>(t + 0x384, z);
            store<u32>(t + 0x380, y);
            isLivingEye(i_this);
            mDoExt_baseAnm_play(gabi::at<void>(t + 0x6AC));
            gabi::call(0x025E535C, load<u32>(t + 0x6A8), 0u, 0u, 0u); /* mpMorf->play(NULL, 0, 0) */
            gabi::call(0x025E55A0, load<u32>(t + 0x6A8));             /* mpMorf->calc() */
            setRotEye(i_this);
            dBgS_Acch_CrrPos(gabi::at<void>(t + 0x2398), dComIfG_Bgsp());
            store<u8>(t + 0x1C9, load<u8>(t + 0x326)); /* tevStr.mRoomNo */
            /* tevStr.mEnvrIdxOverride = dComIfG_Bgsp()->GetPolyColor(mAcch.m_gnd) */
            u32 bgs = A(dComIfG_Bgsp());
            store<u8>(t + 0x1CA, (u8)gabi::call<u32>(0x024EEEB8, bgs, t + 0x2480));
        }
    }
    modeProc(i_this, daDaiocta_c::PROC_EXEC_e, daDaiocta_c::MODE_NULL);
    if (i_this->mMode != daDaiocta_c::MODE_HIDE) {
        setCollision(i_this);
        setAnm(i_this);
        setMtx(i_this);
        if (hio_b(0x7) != 0) {
            i_this->mPrmIdx = (s8)hio_s(0xA);
            setAnm(i_this);
        }
    }
    return false;
}
VERIFY(0x0211AE68, _execute);

/* 0211B058 */
static BOOL daDaioctaExecute(void* i_this) {
    WWHD_FUNC(0x0211B058, BOOL, i_this);
    return _execute((daDaiocta_c*)i_this);
}
VERIFY(0x0211B058, daDaioctaExecute);

/* function-local static GXColor (HD: initialised on first use, guard word + memcpy) */
static void oct_staticColor(u32 guard, u32 dst, u32 src) {
    if (load<u32>(guard) == 0) {
        store<u32>(guard, 1);
        memcpy_g(gabi::at<void>(dst), gabi::at<void>(src), 4); /* memcpy */
    }
}

/* 0211B05C: HD: the debug primitives are gone; what is left is the distance loop and the
 * first-use initialisation of the static colours */
static void drawDebug(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B05C, void, i_this);
    u32 t = A(i_this);
    gabi::Local<cXyz> d;
    for (int i = 0; i < 37; i++) {
        fopAc_ac_c* player_p = dComIfGp_getPlayer(0);
        cXyz_mi(gabi::at<cXyz>(A(player_p) + 0x314), d.get(), gabi::at<cXyz>(t + 0x3AC + i * 12));
        f32 dist = std_sqrtf(PSVECSquareMag(d.get()));
        if (dist < 50.0f) {
            oct_staticColor(0x101FDA48, 0x101FEBEC, 0x1000CFA0);
        } else {
            oct_staticColor(0x101FDA44, 0x101FEBE8, 0x1000CFA4);
        }
    }
    oct_staticColor(0x101FDA44, 0x101FEBE8, 0x1000CFA4);
    oct_staticColor(0x101FDA4C, 0x101FEBF0, 0x1000CFA8);
    oct_staticColor(0x101FDA50, 0x101FEBF4, 0x1000CFAC);
    oct_staticColor(0x101FDA48, 0x101FEBEC, 0x1000CFA0);
    if (load<u32>(0x101FDA50) == 0) {
        store<u32>(0x101FDA50, 1);
        memcpy_g(gabi::at<void>(0x101FEBF4), gabi::at<void>(0x1000CFAC), 4);
        oct_staticColor(0x101FDA50, 0x101FEBF4, 0x1000CFAC);
    }
}
VERIFY(0x0211B05C, drawDebug);

/* 0211B240 */
static void drawSuikomi(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B240, void, i_this);
    u32 t = A(i_this);
    dComIfGd_setListBG();
    oct_settingTevStruct(t, 2 /* TEV_TYPE_BG1 */);
    {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, gabi::at<J3DModel>(load<u32>(t + 0x284C)), gabi::at<dKy_tevstr_c>(t + 0x110));
    }
    u32 model = load<u32>(t + 0x284C);
    gabi::call(0x025E7FC4, t + 0x28C8, load<u32>(model + 0xAC), load<f32>(t + 0x28CC)); /* mBtkAnm.entry */
    model = load<u32>(t + 0x284C);
    gabi::call(0x025E83FC, t + 0x2850, load<u32>(model + 0xAC), load<f32>(t + 0x2854)); /* mBrkAnm2.entry */
    mDoExt_modelUpdateDL(gabi::at<J3DModel>(load<u32>(t + 0x284C)), 0);
    store<u32>(load<u32>(load<u32>(t + 0x284C) + 0xAC) + 0x44, 0); /* mBtkAnm.remove */
    store<u32>(load<u32>(load<u32>(t + 0x284C) + 0xAC) + 0x48, 0); /* mBrkAnm2.remove */
    dComIfGd_setList();
}
VERIFY(0x0211B240, drawSuikomi);

/* 0211B318 */
static void drawAwa(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B318, void, i_this);
    u32 t = A(i_this);
    dComIfGd_setListBG();
    oct_settingTevStruct(t, 2 /* TEV_TYPE_BG1 */);
    for (int i = 0; i < hio_s(0x16); i++) {
        u32 pm = t + 0x293C + 4 * i;
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, gabi::at<J3DModel>(load<u32>(pm)), gabi::at<dKy_tevstr_c>(t + 0x110));
        u32 bck = t + 0x29B4 + i * 0x8C;
        gabi::call(0x025E86B8, bck, load<u32>(load<u32>(pm) + 0xAC), load<f32>(bck + 4));
        u32 btk = t + 0x3A1C + i * 0x74;
        gabi::call(0x025E7FC4, btk, load<u32>(load<u32>(pm) + 0xAC), load<f32>(btk + 4));
        u32 brk = t + 0x47B4 + i * 0x78;
        gabi::call(0x025E83FC, brk, load<u32>(load<u32>(pm) + 0xAC), load<f32>(brk + 4));
        mDoExt_modelUpdateDL(gabi::at<J3DModel>(load<u32>(pm)), 0);
    }
    dComIfGd_setList();
}
VERIFY(0x0211B318, drawAwa);

/* 0211B420 */
static bool _draw(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B420, bool, i_this);
    u32 t = A(i_this);
    if (hio_b(0x4) != 0) drawDebug(i_this);
    s32 mode = i_this->mMode;
    if (mode == daDaiocta_c::MODE_HIDE) return true;
    if (mode == daDaiocta_c::MODE_DEMO) {
        drawSuikomi(i_this);
        mode = i_this->mMode;
    }
    if (mode == daDaiocta_c::MODE_DELETE) drawAwa(i_this);
    u32 morf_model_p = load<u32>(load<u32>(t + 0x6A8) + 0x90);
    oct_settingTevStruct(t, 0 /* TEV_TYPE_ACTOR */);
    {
        u32 morf = load<u32>(t + 0x6A8);
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, gabi::at<J3DModel>(load<u32>(morf + 0x90)), gabi::at<dKy_tevstr_c>(t + 0x110));
    }
    gabi::call(0x025E83FC, t + 0x6AC, load<u32>(morf_model_p + 0xAC), load<f32>(t + 0x6B0)); /* mBrkAnm1.entry */
    if (hio_b(0x5) == 0) gabi::call(0x025E5590, load<u32>(t + 0x6A8)); /* mpMorf->entryDL() */
    store<u32>(load<u32>(morf_model_p + 0xAC) + 0x48, 0);               /* mBrkAnm1.remove */
    dSnap_RegistFig(0xC6 /* DSNAP_TYPE_DAIOCTA */, i_this, 1.0f, 1.0f, 1.0f);
    return true;
}
VERIFY(0x0211B420, _draw);

/* 0211B52C */
static BOOL daDaioctaDraw(void* i_this) {
    WWHD_FUNC(0x0211B52C, BOOL, i_this);
    return _draw((daDaiocta_c*)i_this);
}
VERIFY(0x0211B52C, daDaioctaDraw);

/* 0211B530 */
static void modeHideInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B530, void, i_this);
    store<f32>(A(i_this) + 0x318, gabi::fsubs_ppc(i_this->mWaterY, 3000.0f));
}
VERIFY(0x0211B530, modeHideInit);

/* 0211B548 */
static void modeHide(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B548, void, i_this);
    f32 dist_xz = fopAcM_searchActorDistanceXZ(i_this, dComIfGp_getPlayer(0));
    if (dist_xz < i_this->mAppearRadius || dist_xz < hio_f(0x7C)) {
        if (load<u32>(oct_play() + 0x5CD8) & 0x10000) { /* dComIfGp_checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e) */
            modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_APPEAR);
        }
    }
}
VERIFY(0x0211B548, modeHide);

/* 0211B5C0 */
static void modeAppearInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B5C0, void, i_this);
    u32 t = A(i_this);
    store<u32>(t + 0x2E0, (load<u32>(t + 0x2E0) & ~0x3Fu) | 0x20); /* fopAcM_SetStatusMap(this, 0) */
    u32 player = A(dComIfGp_getPlayer(0));
    gabi::Local<cXyz> dir;
    gabi::Local<cXyz> norm;
    gabi::Local<cXyz> scaled;
    gabi::Local<cXyz> new_pos;
    cXyz_mi(gabi::at<cXyz>(t + 0x314), dir.get(), gabi::at<cXyz>(player + 0x314));
    gabi::call(0x0201B12C, dir.get(), norm.get()); /* cXyz::normZP */
    cXyz_ml(norm.get(), scaled.get(), hio_f(0x7C));
    cXyz_pl(gabi::at<cXyz>(player + 0x314), new_pos.get(), scaled.get());
    f32 auzu_y = gabi::fadds_ppc(i_this->mWaterY, 2.0f);
    u32 x = load<u32>(A(new_pos.get()) + 0);
    u32 auzuId = i_this->mAuzuId;
    store<u32>(t + 0x39C, 0); /* attention_info.flags */
    u32 z = load<u32>(A(new_pos.get()) + 8);
    store<u32>(t + 0x314, x);
    store<u32>(t + 0x31C, z);
    {
        gabi::Local<be<u32>> auzu_p;
        if (oct_searchByID(auzuId, auzu_p.get()) && *auzu_p != 0) {
            u32 a = *auzu_p;
            store<f32>(a + 0x318, auzu_y);
            store<u32>(a + 0x314, x);
            store<u32>(a + 0x31C, z);
        }
    }
    setEffect(i_this, 0x81FE); /* ID_IT_SN_DO_APPSHIBUKI00 */
    gabi::call(0x025E1960, 90); /* mDoAud_bgmAllMute(90) */
    gabi::Local<be<u32>> eye_p;
    for (int i = 0; i < 12; i++) {
        if (oct_searchByID(load<u32>(t + 0x25C0 + 4 * i), eye_p.get()) && *eye_p != 0) {
            gabi::at<daDaiocta_Eye_c>(*eye_p)->appeared = 1; /* mbAppear */
        }
    }
}
VERIFY(0x0211B5C0, modeAppearInit);

/* the eye flag scans */
static inline u8 oct_eyeFlag(u32 eye, u32 off) { return load<u8>(eye + off); }

/* 0211B734 */
static bool isDamageEye(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B734, bool, i_this);
    u32 t = A(i_this);
    bool is_damaged = false;
    for (int i = 0; i < 12; i++) {
        if (load<u8>(t + 0x25F0 + i) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->damaged != 0) is_damaged = true;
        }
    }
    return is_damaged;
}
VERIFY(0x0211B734, isDamageEye);

/* 0211B7D8 */
static void modeAppear(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211B7D8, void, i_this);
    u32 t = A(i_this);
    if (isDamageEye(i_this)) modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE);
    if (load<u32>(t + 0x25A0) != 0) { /* mParticleCallback.getEmitter() */
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        gabi::call(0x025602F0, amb.get(), dif.get()); /* dKy_get_seacolor */
        u32 ca = A(amb.get());
        s32 add = hio_s(0x18);
        s32 r = load<u8>(ca + 0) + add;
        s32 g = load<u8>(ca + 1) + add;
        s32 b = load<u8>(ca + 2) + add;
        if (r >= 0xFF) r = 0xFF;
        if (g >= 0xFF) g = 0xFF;
        if (b >= 0xFF) b = 0xFF;
        store<u8>(ca + 1, (u8)g);
        store<u8>(ca + 2, (u8)b);
        u32 em = load<u32>(t + 0x25A0);
        store<u8>(ca + 0, (u8)r);
        /* JPABaseEmitter::setGlobalPrmColor(r, g, b) */
        store<u8>(em + 0x244, (u8)r);
        store<u8>(em + 0x246, (u8)b);
        store<u8>(em + 0x245, (u8)g);
    }
    u32 auzu_p = A(fopAcM_SearchByID(i_this->mAuzuId));
    if (auzu_p != 0) {
        f32 s = load<f32>(auzu_p + 0x430); /* mScaleAnimFactor */
        store<u8>(auzu_p + 0x434, 1);      /* to_appear(): mbToAppear = true */
        if (s > 0.999f) {                  /* is_appear() */
            gabi::Local<cXyz> sp2C;
            fcpy(A(sp2C.get()) + 0, t + 0x314);
            fcpy(A(sp2C.get()) + 8, t + 0x31C);
            fcpy(A(sp2C.get()) + 4, t + 0x25B0);
            gabi::call(0x0200F164, t + 0x314, sp2C.get(), 0.1f, hio_f(0x80)); /* cLib_addCalcPos2 */
            gabi::Local<cXyz> d;
            f32 dist = oct_abs(t + 0x314, A(sp2C.get()), d);
            if (dist < 1900.0f && i_this->m26C0 == 0) {
                oct_seStart(t, 0x58C2 /* JA_SE_CM_DO_ENTER */, t + 0x37C);
                i_this->m26C0 = 1;
                gabi::call(0x025E1918, 0x8000002Bu); /* mDoAud_subBgmStart(JA_BGM_DIOCTA_BATTLE) */
            }
            if (!((f64)dist > 0.1)) {
                modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_WAIT);
            } else {
                s16 target = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                cLib_addCalcAngleS2(gabi::at<be<s16>>(t + 0x32A), target, 4, hio_s(0x10));
            }
        }
    }
}
VERIFY(0x0211B7D8, modeAppear);

/* 0211BA0C */
static bool isDead(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BA0C, bool, i_this);
    u32 t = A(i_this);
    s32 count = 0;
    for (int i = 0; i < 12; i++) {
        if (load<u8>(t + 0x25F0 + i) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->dead == 0) count++;
        }
    }
    return count == 0;
}
VERIFY(0x0211BA0C, isDead);

/* 0211BAB4 */
static void modeWaitInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BAB4, void, i_this);
    if (isDead(i_this)) modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
}
VERIFY(0x0211BAB4, modeWaitInit);

/* 0211BAF8 */
static bool isDamageBombEye(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BAF8, bool, i_this);
    u32 t = A(i_this);
    bool is_damaged = false;
    for (int i = 0; i < 12; i++) {
        if (load<u8>(t + 0x25F0 + i) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->damagedByBomb != 0) is_damaged = true;
        }
    }
    return is_damaged;
}
VERIFY(0x0211BAF8, isDamageBombEye);

/* modeWait / modeDamage / modeDamageBomb: the shared transition checks */
static void oct_checkTransitions(daDaiocta_c* i_this, bool hioDelete) {
    if (isDead(i_this) || (hioDelete && hio_b(0x6) != 0)) {
        modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
    } else if (isDamageEye(i_this)) {
        modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE);
    } else if (isDamageBombEye(i_this)) {
        modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE_BOMB);
    }
}

/* 0211BB9C */
static void modeWait(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BB9C, void, i_this);
    fcpy(A(i_this) + 0x318, A(i_this) + 0x25B0);
    oct_checkTransitions(i_this, true);
}
VERIFY(0x0211BB9C, modeWait);

/* 0211BC40 */
static void modeDamageInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BC40, void, i_this);
    if (isDead(i_this)) {
        modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
        return;
    }
    i_this->mPrmIdx = 2;
    setAnm(i_this);
    i_this->mPrmIdx = 4;
    setEffect(i_this, 0x8201); /* ID_IT_SN_DO_HITB_SHIBUKIA00 */
    setEffect(i_this, 0x8202); /* ID_IT_SN_DO_HITB_SHIBUKIB00 */
}
VERIFY(0x0211BC40, modeDamageInit);

static void oct_damageEnd(daDaiocta_c* i_this) {
    if (i_this->mPrmIdx == 2) {
        f32 d = std::fabs(gabi::fsubs_ppc(load<f32>(A(i_this) + 0x318), i_this->mWaterY));
        modeProc(i_this, daDaiocta_c::PROC_INIT_e, (f64)d > 0.1 ? daDaiocta_c::MODE_APPEAR : daDaiocta_c::MODE_WAIT);
    }
}

/* 0211BCD8 */
static void modeDamage(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BCD8, void, i_this);
    fcpy(A(i_this) + 0x318, A(i_this) + 0x25B0);
    oct_checkTransitions(i_this, false);
    oct_damageEnd(i_this);
}
VERIFY(0x0211BCD8, modeDamage);

/* 0211BDB0 */
static void modeDamageBombInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BDB0, void, i_this);
    if (isDead(i_this)) {
        modeProc(i_this, daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
        return;
    }
    oct_seStart(A(i_this), 0x2828 /* JA_SE_LK_LAST_HIT */, A(i_this) + 0x37C);
    i_this->mPrmIdx = 2;
    setAnm(i_this);
    i_this->mPrmIdx = 3;
    setEffect(i_this, 0x81FF); /* ID_IT_SN_DO_HITA_SHIBUKIA00 */
    setEffect(i_this, 0x8200); /* ID_IT_SN_DO_HITA_SHIBUKIB00 */
}
VERIFY(0x0211BDB0, modeDamageBombInit);

/* 0211BE6C */
static void modeDamageBomb(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BE6C, void, i_this);
    fcpy(A(i_this) + 0x318, A(i_this) + 0x25B0);
    oct_checkTransitions(i_this, false);
    oct_damageEnd(i_this);
}
VERIFY(0x0211BE6C, modeDamageBomb);

/* 0211BF44 */
static void modeDeleteInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BF44, void, i_this);
    u32 t = A(i_this);
    store<u32>(t + 0x39C, 0); /* attention_info.flags */
    oct_seStart(t, 0x48C9 /* JA_SE_CV_DO_DIE */, t + 0x37C);
    oct_seStart(t, 0x58C5 /* JA_SE_CM_DO_JITABATA */, t + 0x37C);
    oct_seStart(t, 0x2828 /* JA_SE_LK_LAST_HIT */, t + 0x37C);
    setEffect(i_this, 0x8203); /* ID_IT_SN_DO_HITC_SHIBUKIA00 */
    setEffect(i_this, 0x8204); /* ID_IT_SN_DO_HITC_SHIBUKIB00 */
}
VERIFY(0x0211BF44, modeDeleteInit);

/* event manager (play+0x52C4) */
static inline s32 oct_getMyStaffId(u32 name) { return gabi::call<s32>(0x02542D88, oct_evm(), name, 0u, 0u); }
static inline u32 oct_getMyNowCutName(s32 staff) { return gabi::call<u32>(0x02544830, oct_evm(), staff); }
static inline void oct_cutEnd(s32 staff) { gabi::call(0x02543280, oct_evm(), staff); }
static inline BOOL oct_endCheck(u32 name) { return gabi::call<BOOL>(0x0254457C, oct_evm(), name); } /* HD: endCheckOld */
/* fopAcM_orderOtherEvent(this, name) (HD: 025D77DC with flag 1 and hind 0xFFFF) */
static inline void oct_orderOtherEvent(u32 t, u32 name) { gabi::call(0x025D77DC, t, name, 1u, 0xFFFFu); }

/* 0211BFFC */
static void modeDelete(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211BFFC, void, i_this);
    u32 t = A(i_this);
    if (load<u16>(t + 0xF8) == 2) { /* eventInfo.checkCommandDemoAccrpt() */
        s32 staff_id = oct_getMyStaffId(0x1000D2B0); /* "Daiocta" */
        u32 cut_name = oct_getMyNowCutName(staff_id);
        if (cut_name == 0) { /* HD: null check */
            oct_assert(0x1000D2C8, 0x62F, 0x1000D2D8);
        } else {
            if (oct_strcmp(cut_name, 0x1000D2B8) == 0) { /* "DEATH1" */
                u32 morf = load<u32>(t + 0x6A8);
                i_this->mPrmIdx = 5;
                fcpy(t + 0x318, t + 0x25B0);
                if (oct_morfIsStop(morf)) {
                    /* fopAcM_monsSeStart(this, JA_SE_CV_DO_SINK, 0) */
                    s32 reverb = gabi::call<s32>(0x02520540, (s32)load<s8>(t + 0x326));
                    gabi::call(0x025E1AA4, 0x48CA, t + 0x37C, load<u32>(t + 4), 0u, reverb);
                    setEffect(i_this, 0x8205); /* ID_IT_SN_DO_DOWNSHIBUKI00 */
                    u32 auzu_p = A(fopAcM_SearchByID(i_this->mAuzuId));
                    if (auzu_p == 0) oct_assert(0x1000D2C8, 0x642, 0x1000D2E8); /* HD */
                    else store<u8>(auzu_p + 0x434, 0);                          /* to_disappear() */
                    gabi::Local<cXyz> q;
                    u32 vib = oct_play() + 0x599C;
                    q->z = 0.0f;
                    q->x = 0.0f;
                    q->y = 1.0f;
                    gabi::call(0x025CB408, vib, 7, -0x21, q.get()); /* StartQuake */
                    oct_cutEnd(staff_id);
                }
            }
            if (oct_strcmp(cut_name, 0x1000D2C0) == 0) { /* "DEATH2" */
                gabi::Local<cXyz> target_pos;
                u32 z = load<u32>(t + 0x31C);
                u32 x = load<u32>(t + 0x314);
                f32 w = i_this->mWaterY;
                store<u32>(A(target_pos.get()) + 0, x);
                f32 y = gabi::fsubs_ppc(w, 3000.0f);
                store<u32>(A(target_pos.get()) + 8, z);
                i_this->mPrmIdx = 7;
                f32 step = hio_f(0xE0);
                target_pos->y = y;
                gabi::call(0x0200F62C, t + 0x314, target_pos.get(), step); /* cLib_chasePos */
                if (std::fabs(gabi::fsubs_ppc(load<f32>(t + 0x318), i_this->mWaterY)) > 600.0f && i_this->m26C1 == 0) {
                    initAwa(i_this);
                    oct_seStart(t, 0x58C6 /* JA_SE_CM_DO_SINK_IN_SEA */, t + 0x37C);
                    i_this->m26C1 = 1;
                    i_this->m31C4 = 1;
                    gabi::call(0x025E1928); /* mDoAud_subBgmStop */
                }
                gabi::Local<cXyz> d;
                f32 dist = oct_abs(t + 0x314, A(target_pos.get()), d);
                if (dist < gabi::fmuls_ppc(hio_f(0xE0), 3.0f)) {
                    if (i_this->mSwitchNo != 0xFF) {
                        i_this->m31C4 = 0;
                        gabi::call(0x025CB610, oct_play() + 0x599C, -1); /* StopQuake(-1) */
                        /* HD: the riddle jingle only when the switch was not on yet */
                        if (!dComIfGs_isSwitch(i_this->mSwitchNo, (s32)load<s8>(t + 0x326)) && i_this->m057C != 1) {
                            gabi::call(0x025E1988, 0x806); /* mDoAud_seStart(JA_SE_READ_RIDDLE_1) */
                        }
                        dComIfGs_onSwitch(i_this->mSwitchNo, (s32)load<s8>(t + 0x326));
                        store<u32>(t + 0x2E0, load<u32>(t + 0x2E0) & ~0x3Fu); /* fopAcM_ClearStatusMap */
                    }
                    oct_cutEnd(staff_id);
                }
            }
        }
        if (cut_name == 0 && i_this->m057C == 1) return;
        if (i_this->m057C != 1 && oct_endCheck(0x1000D2F4)) { /* "DAIOCTA_DEAD" */
            dComIfGp_event_reset();
            fopAcM_delete(i_this);
        }
    } else if (i_this->m057C == 1) {
        oct_orderOtherEvent(t, 0x1000D304); /* "DAIOCTA_DEAD_ELF" */
    } else {
        oct_orderOtherEvent(t, 0x1000D2F4); /* "DAIOCTA_DEAD" */
    }
}
VERIFY(0x0211BFFC, modeDelete);

/* 0211C444 */
static void modeDemoInit(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211C444, void, i_this);
    u32 t = A(i_this);
    store<u32>(t + 0x39C, 0); /* attention_info.flags */
    setEffect(i_this, 0x8207); /* ID_IT_SN_DO_SUIKOMIA00 */
    setEffect(i_this, 0x8208); /* ID_IT_SN_DO_SUIKOMIB00 */
    setEffect(i_this, 0x8209); /* ID_IT_SN_DO_SUIKOMIC00 */
    u32 model_data_p = load<u32>(load<u32>(t + 0x284C) + 0xAC);
    u32 brk = oct_getRes(0x1D); /* GDO_SUI00 brk */
    if (brk == 0) oct_assert(0x1000D318, 0x549, 0x1000D328);
    u32 btk = oct_getRes(0x26); /* GDO_SUI00 btk */
    if (btk == 0) oct_assert(0x1000D318, 0x54C, 0x1000D334);
    gabi::call(0x025E8154, t + 0x2850, model_data_p, brk, 1, 0, 1.0f, 0, -1, 1, 0); /* mBrkAnm2.init */
    gabi::call(0x025E7CE0, t + 0x28C8, model_data_p, btk, 1, 0, 1.0f, 0, -1, 1, 0); /* mBtkAnm.init */
}
VERIFY(0x0211C444, modeDemoInit);

static inline void oct_initStartPos(u32 ship, u32 pos, s16 angle) { gabi::call(0x024832E0, ship, pos, (s32)angle); }
static inline f32 REG12_F(int i) { return REG_F(12, i); }
static inline s16 REG12_S(int i) { return REG_S(12, i); }

/* 0211C59C */
static void modeDemo(daDaiocta_c* i_this) {
    WWHD_FUNC(0x0211C59C, void, i_this);
    u32 t = A(i_this);
    gabi::Local<be<s16>> nameKey1;
    nameKey1->operator=((s16)0xA5);
    fcpy(t + 0x318, t + 0x25B0);
    u32 ship_p_1 = A(fopAcIt_Judge(0x025E121C, nameKey1.get())); /* fopAcM_SearchByName(fpcNm_SHIP_e) */
    if (ship_p_1 == 0) {                                           /* HD: null check */
        oct_assert(0x1000D36C, 0x559, 0x1000D39C);
        return;
    }
    /* m21F4 = ship->current.pos; m2200 = ship->current.angle */
    store<u32>(t + 0x2374, load<u32>(ship_p_1 + 0x314));
    store<u32>(t + 0x2378, load<u32>(ship_p_1 + 0x318));
    store<u32>(t + 0x237C, load<u32>(ship_p_1 + 0x31C));
    store<u16>(t + 0x2380, load<u16>(ship_p_1 + 0x320));
    store<u16>(t + 0x2382, load<u16>(ship_p_1 + 0x322));
    store<u16>(t + 0x2384, load<u16>(ship_p_1 + 0x324));
    if (load<u16>(t + 0xF8) != 2) { /* !eventInfo.checkCommandDemoAccrpt() */
        oct_orderOtherEvent(t, 0x1000D38C); /* "DAIOCTA_SUIKOMI" */
        return;
    }
    s32 staff_id = oct_getMyStaffId(0x1000D35C); /* "Daiocta" */
    u32 cut_name = oct_getMyNowCutName(staff_id);
    gabi::Local<cXyz> current_pos;
    gabi::Local<be<s16>> o_angle;
    u32 cp = A(current_pos.get());
    fcpy(cp + 4, t + 0x318);
    fcpy(cp + 8, t + 0x31C);
    fcpy(cp + 0, t + 0x314);
    gabi::call(0x02520630, (s32)load<s8>(t + 0x326), cp, cp + 8, o_angle.get()); /* dComIfGp_getMapTrans */
    s16 rot_y = cLib_targetAngleY(gabi::at<cXyz>(t + 0x314), current_pos.get());
    cLib_distanceAngleS(load<s16>(t + 0x32A), rot_y);
    if (cut_name == 0) { /* HD: null check */
        oct_assert(0x1000D36C, 0x578, 0x1000D37C);
    } else {
        if (oct_strcmp(cut_name, 0x1000D3A8) == 0) { /* "SUIKOMI1" */
            i_this->mPrmIdx = 6;
            oct_seStart(t, 0x50C3 /* JA_SE_CM_DO_SUCK_IN */, t + 0x25B4);
            gabi::Local<cXyz> d1, xz1;
            if (oct_absXZ(t + 0x25B4, t + 0x2374, d1, xz1) < 80.0f) {
                oct_cutEnd(staff_id);
            } else {
                u32 morf = load<u32>(t + 0x6A8);
                if (load<f32>(morf + 0x9C) == 50.0f) store<f32>(morf + 0x9C, 40.0f); /* setFrame(40) */
            }
            if (REG12_S(0) != 0) return;
            s16 target_angle_y = cLib_targetAngleY(gabi::at<cXyz>(t + 0x314), gabi::at<cXyz>(ship_p_1 + 0x314));
            s32 distance_angle_s = cLib_distanceAngleS((s16)(load<s16>(t + 0x32A) + hio_s(0x14)), target_angle_y);
            cLib_addCalc2(gabi::at<be<f32>>(t + 0x2388), hio_f(0x88), hio_f(0x90), hio_f(0x8C));
            {
                f32 m2208 = i_this->m2208;
                f32 tx = load<f32>(t + 0x25B4);
                cLib_chaseF(gabi::at<be<f32>>(t + 0x2374), tx, gabi::fmuls_ppc(m2208, 0.5f));
            }
            {
                f32 tz = load<f32>(t + 0x25BC);
                cLib_chaseF(gabi::at<be<f32>>(t + 0x237C), tz, i_this->m2208);
            }
            cLib_addCalcAngleS2(gabi::at<be<s16>>(t + 0x2382), (s16)(target_angle_y + 0x8000), 8, 0x400);
            if (distance_angle_s < hio_s(0x12)) {
                u16 a = (u16)(target_angle_y + 0x4000);
                gabi::Local<cXyz> d2, xz2;
                f32 dist = oct_absXZ(t + 0x314, ship_p_1 + 0x314, d2, xz2);
                f32 k1 = dist / gabi::fadds_ppc(REG12_F(1), 60.0f);
                store<f32>(t + 0x237C, gabi::fmadds(k1, cM_scos(a), load<f32>(t + 0x237C)));
                f32 k2 = dist / gabi::fadds_ppc(REG12_F(1), 60.0f);
                store<f32>(t + 0x2374, gabi::fmadds(k2, cM_ssin(a), load<f32>(t + 0x2374)));
            }
            gabi::Local<be<s16>> nameKey2;
            u32 ship_p_2 = oct_searchShip(nameKey2);
            if (ship_p_2 != 0) oct_initStartPos(ship_p_2, t + 0x2374, load<s16>(t + 0x2382)); /* HD: null check */
        }
        if (oct_strcmp(cut_name, 0x1000D3B4) == 0) { /* "SUIKOMI2" */
            if (load<f32>(load<u32>(t + 0x6A8) + 0x9C) == 65.0f) {
                oct_seStart(t, 0x58C4 /* JA_SE_CM_DO_SUCK_IN_FIN */, t + 0x37C);
            }
            s16 target_angle_y = cLib_targetAngleY(gabi::at<cXyz>(t + 0x314), gabi::at<cXyz>(ship_p_1 + 0x314));
            cLib_distanceAngleS(load<s16>(t + 0x32A), target_angle_y);
            gabi::call(0x0200F764, t + 0x2374, t + 0x314, (f32)i_this->m2208); /* cLib_chasePosXZ */
            cLib_addCalcAngleS2(gabi::at<be<s16>>(t + 0x2382), (s16)(target_angle_y + 0x8000), 4, 0x800);
            gabi::Local<be<s16>> nameKey3;
            u32 ship_p_2 = oct_searchShip(nameKey3);
            if (ship_p_2 != 0) { /* HD: null check */
                oct_initStartPos(ship_p_2, t + 0x2374, load<s16>(t + 0x2382));
                gabi::Local<cXyz> d3, xz3;
                if (oct_absXZ(t + 0x314, t + 0x2374, d3, xz3) < 10.0f) {
                    u32 morf = load<u32>(t + 0x6A8);
                    if (oct_morfIsStop(morf) || i_this->mPrmIdx == 2) {
                        u32 m = load<u32>(t + 0x6A8);
                        store<s16>(t + 0x32A, rot_y);
                        gabi::call(0x025E55A0, m); /* mpMorf->calc() */
                        /* current.pos = m220C (integer copy) */
                        u32 x = load<u32>(t + 0x238C), y = load<u32>(t + 0x2390);
                        store<u32>(t + 0x314, x);
                        u32 z = load<u32>(t + 0x2394);
                        store<u32>(t + 0x318, y);
                        store<u32>(t + 0x31C, z);
                        oct_initStartPos(ship_p_2, t + 0x314, rot_y);
                        oct_cutEnd(staff_id);
                    }
                }
            }
        }
        if (oct_strcmp(cut_name, 0x1000D3C0) == 0) { /* "HAKIDASU" */
            u32 actor_p = A(fopAcM_SearchByID(i_this->mAuzuId));
            if (actor_p != 0) { /* HD: null check */
                fcpy(actor_p + 0x314, t + 0x314);
                f32 y = load<f32>(t + 0x318);
                fcpy(actor_p + 0x318, t + 0x318);
                fcpy(actor_p + 0x31C, t + 0x31C);
                store<f32>(actor_p + 0x318, gabi::fadds_ppc(y, 20.0f));
            }
            u32 morf = load<u32>(t + 0x6A8);
            if (gabi::ftoi(load<f32>(morf + 0x9C)) == 26) {
                oct_seStart(t, 0x4937 /* JA_SE_CV_DO_SPIT_SHIP */, t + 0x25B4);
                morf = load<u32>(t + 0x6A8);
            }
            if (i_this->mPrmIdx == 8 && oct_morfIsStop(morf)) {
                oct_cutEnd(staff_id);
                gabi::call(0x025E1928); /* mDoAud_subBgmStop */
            } else if (load<f32>(morf + 0x9C) < 10.0f) {
                oct_initStartPos(ship_p_1, t + 0x25B4, rot_y);
                i_this->mDemoEndTimer = 60; /* HD */
            }
            i_this->mPrmIdx = 8;
        }
        if (oct_strcmp(cut_name, 0x1000D364) == 0) i_this->mPrmIdx = 2; /* "WAIT" */
    }
    /* HD: the fallback timer */
    s32 tm = i_this->mDemoEndTimer;
    if (tm != 0) i_this->mDemoEndTimer = tm - 1;
    if (oct_endCheck(0x1000D38C) || i_this->mDemoEndTimer == 1) { /* "DAIOCTA_SUIKOMI" */
        u8 scls = i_this->m575;
        u32 play = oct_play();
        if (scls == 0xFF) {
            /* daPy_getPlayerLinkActorClass()->setDaiokutaEnd(): startRestartRoom(6, 0xC9, -1.0f, 1) */
            gabi::call(0x023FD4E4, load<u32>(play + 0x5B34), 6, 0xC9, -1.0f, 1);
        } else {
            /* dComIfGp_setNextStage(dComIfGp_getStartStageName(), m575, roomNo, -1, 0.0f, 6) (HD: two more arguments) */
            gabi::call(0x0252012C, play + 0x5134, (u32)i_this->m575, (s32)load<s8>(t + 0x326), -1, 0.0f, 6, 1, 0);
        }
    }
}
VERIFY(0x0211C59C, modeDemo);

/* 0211CD80 */
static u32 daDaiocta_HIO_ct(u32 self) {
    WWHD_FUNC(0x0211CD80, u32, self);
    if (self == 0) {
        self = A(operator_new(0x108));
        if (self == 0) return 0;
    }
    store<u32>(self + 0, 0x1000D044); /* mDoHIO_entry_c vtable */
    store<u8>(self + 4, 0);
    store<u8>(self + 8, 0);
    store<u8>(self + 6, 0);
    store<u8>(self + 5, 0);
    static const struct { u16 off; f32 v; } f[] = {
        {0x1C, 600.0f}, {0x20, 500.0f}, {0x24, 450.0f}, {0x28, 350.0f}, {0x2C, 350.0f}, {0x30, 300.0f},
        {0x34, 60.0f}, {0x38, 60.0f}, {0x3C, 60.0f}, {0x40, 60.0f}, {0x44, 60.0f}, {0x48, 60.0f},
        {0x4C, 60.0f}, {0x50, 60.0f}, {0x54, 60.0f}, {0x58, 60.0f},
        {0x5C, 100.0f}, {0x60, 100.0f}, {0x64, 250.0f}, {0x68, 100.0f}, {0x6C, 150.0f}, {0x70, 100.0f},
        {0x74, 150.0f}, {0x78, 5000.0f}, {0x7C, 4500.0f}, {0x80, 30.0f}, {0x84, 1800.0f}, {0x88, 40.0f},
        {0x8C, 1.0f}, {0x90, 0.02f}, {0x94, 150.0f},
        {0xFC, 0.0f}, {0x100, 5.0f}, {0x104, 700.0f},
        {0xC0, 8.0f}, {0xC4, 8.0f}, {0xC8, 3.0f}, {0xCC, 3.0f}, {0xD0, 3.0f}, {0xD4, 7.0f}, {0xD8, 9.0f},
        {0x9C, 1.0f}, {0xA0, 0.5f}, {0xA4, 0.75f}, {0xA8, 0.625f}, {0xAC, 0.625f}, {0xB0, 0.5f}, {0xB4, 1.0f}, {0xB8, 1.0f},
        {0xE0, 20.0f}, {0xE4, 300.0f}, {0xE8, 1200.0f}, {0xEC, 1.0f}, {0xF0, 2.0f}, {0xF4, 10.0f}, {0xF8, 30.0f},
    };
    for (const auto& e : f) store<f32>(self + e.off, e.v);
    store<s16>(self + 0xC, 0x1E);   /* mRotEyeTimerMin */
    store<s16>(self + 0xE, 0x3C);   /* mRotEyeTimerAddMax */
    store<s16>(self + 0x10, 0x100);
    store<s16>(self + 0x12, 0x1500);
    store<s16>(self + 0x14, -0x800);
    store<s16>(self + 0x16, 0x1E);
    store<s16>(self + 0x18, 0x2D);
    return self;
}
VERIFY(0x0211CD80, daDaiocta_HIO_ct);

/* 0211CFD8: __sinit_d_a_daiocta_cpp (compiler-generated) */
static void __sinit_d_a_daiocta_cpp() {
    WWHD_FUNC(0x0211CFD8, void);
    sinit_header_statics(0x10463928, 0x101B40C8);
    daDaiocta_HIO_ct(DAIOCTA_HIO); /* l_HIO */
}
VERIFY(0x0211CFD8, __sinit_d_a_daiocta_cpp);

/* 0211D078: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dtor(SafeString* self, u32 flags) {
    WWHD_FUNC(0x0211D078, void, self, flags);
    if (self != nullptr && (flags & 1)) operator_delete(self);
}
VERIFY(0x0211D078, SafeString_dtor);

/* 0211D08C: dCcD_Cps::dCcD_Cps (this TU's inline copy, allocates when this == NULL) */
static u32 dCcD_Cps_ct(u32 self) {
    WWHD_FUNC(0x0211D08C, u32, self);
    if (self == 0) {
        self = A(operator_new(0x138));
        if (self == 0) return 0;
    }
    gabi::call(0x02515FB8, self);           /* dCcD_GObjInf::dCcD_GObjInf */
    store<u32>(self + 0x114, 0x100015A8);   /* cCcD_ShapeAttr */
    store<u32>(self + 0x110, 0x1000CFCC);   /* cM3dGAab (this TU) */
    gabi::call(0x02018150, self + 0x118);   /* cM3dGCps::cM3dGCps */
    store<u32>(self + 0x3C, 0x1004AF18);    /* dCcD_Cps */
    store<u32>(self + 0x130, 0x1004AF60);
    store<u32>(self + 0x114, 0x1004AF70);
    return self;
}
VERIFY(0x0211D08C, dCcD_Cps_ct);

/* 0211D118: mDoExt_bckAnm::mDoExt_bckAnm (this TU's inline copy, allocates when this == NULL) */
static u32 mDoExt_bckAnm_ct(u32 self) {
    WWHD_FUNC(0x0211D118, u32, self);
    if (self == 0) {
        self = A(operator_new(0x8C));
        if (self == 0) return 0;
    }
    gabi::call(0x027F2BC0, self, 0);        /* J3DFrameCtrl::init(0) */
    store<u32>(self + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, self + 0x14);
    store<u32>(self + 0x88, 0);
    store<u32>(self + 0x48, 0x1016D820);
    store<u32>(self + 0x58, 0);
    store<u32>(self + 0x84, 0);
    store<u32>(self + 0x10, 0x1000CFDC);
    store<u32>(self + 0x80, 0);
    store<u32>(self + 0x7C, 0);
    return self;
}
VERIFY(0x0211D118, mDoExt_bckAnm_ct);

/* 0211D1A8 */
static BOOL daDaioctaIsDelete(void*) {
    WWHD_FUNC(0x0211D1A8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0211D1A8, daDaioctaIsDelete);

/* 0211D1B0: mDoExt_bckAnm deleting destructor (this TU's copy) */
static void mDoExt_bckAnm_dtor(u32 self, u32 flags) {
    WWHD_FUNC(0x0211D1B0, void, self, flags);
    if (self == 0) return;
    gabi::call(0x027F3628, self + 0x10, 0); /* the anm member's destructor */
    if (flags & 1) operator_delete(gabi::at<void>(self));
}
VERIFY(0x0211D1B0, mDoExt_bckAnm_dtor);

/* 0211D204: daDaiocta_c deleting destructor (HD virtual destructor) */
static void daDaiocta_dtor(u32 self, u32 flags) {
    WWHD_FUNC(0x0211D204, void, self, flags);
    if (self == 0) return;
    __destroy_arr(self + 0x29B4, 30, 0x8C, 0x0211D1B0); /* mAwaBckAnms */
    gabi::call(0x02018034, self + 0x2570, 2);          /* mAcchCir: cM3dGCir::~cM3dGCir */
    store<u32>(self + 0x23B8, 0x1000D014);             /* dBgS_ObjAcch vtables (this TU) */
    store<u32>(self + 0x23AC, 0x1000D024);
    gabi::call(0x024EFD9C, self + 0x2398, 0);          /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x02515860, self + 0x22E4, 2);          /* dCcD_Stts::~dCcD_Stts */
    __destroy_arr(self + 0xE2C, 17, 0x138, 0x02515980); /* mCps */
    __destroy_arr(self + 0x724, 6, 0x12C, 0x02515AE8);  /* mSph */
    gabi::call(0x025D50BC, self, 0);                   /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(gabi::at<void>(self));
}
VERIFY(0x0211D204, daDaiocta_dtor);

/* 0211D2F4: empty virtual (sead::SafeString::assureTerminationImpl_, this TU's vtable 0x1000CFB4 + 0x14) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x0211D2F4, void, (u32)0);
}
VERIFY(0x0211D2F4, SafeString_assureTermination);

/* 0211D2F8: cLib_calcTimer<int> (this TU's copy) */
static s32 cLib_calcTimer_i(be<s32>* timer) {
    WWHD_FUNC(0x0211D2F8, s32, timer);
    s32 v = *timer;
    if (v != 0) {
        v--;
        *timer = v;
    }
    return v;
}
VERIFY(0x0211D2F8, cLib_calcTimer_i);
