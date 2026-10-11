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

static daDaiocta_HIO_c l_HIO;
static DaioctaModes mode_tbl;
static DaioctaBrkIndices a_brk_anm_idx_tbl;
static DaioctaBrkParameters a_brk_anm_prm_tbl;
static DaioctaAnimationIndices a_anm_idx_tbl;
static DaioctaAnimationParameters l_anmParameters;
static DaioctaSphereJoints l_sphereJoints;
static DaioctaJointHitTable search_data;
static Mtx34 l_matrixStack;
// HD: the archive name is capitalized as "Daiocta".
static const char m_arc_name[8] = {68, 97, 105, 111, 99, 116, 97, 0};

using gabi::load;
using gabi::store;

static inline u32 A(const void* p) { return gabi::ea(p); }
static inline daDaiocta_c* T(u32 a) { return gabi::at<daDaiocta_c>(a); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGp_get() (the play object singleton) */
static inline u32 oct_play() { return gabi::ea(DaioctaHD_dComIfGp_get()); }
static inline u32 oct_evm() {
    return A(&DaioctaGuest<dComIfG_inf_c>(oct_play())->play.mEvtManager);
}
static inline void oct_assert(u32 file, s32 line, u32 msg) {
    DaioctaHD_JUTAssertion_showAssert(file, line, msg);
}
/* l_HIO fields */

/* dComIfG_getObjectRes(m_arc_name, idx) (HD: sead::SafeString temporary, this TU's vtable) */
static u32 oct_getRes(s32 idx) {
    return gabi::ea(
        dComIfG_getObjectRes(gabi::at<const char>(A(m_arc_name)), idx, DAIOCTA_SAFESTRING_VTBL));
}
/* J3DModel::getAnmMtx(jnt) (HD: the joint matrix block at +0x2C, marked dirty) */
static u32 oct_getAnmMtx(u32 model, u32 jnt) {
    u32 blk = (u32)DaioctaGuest<J3DModel>(model)->mpMtxBuffer.v;
    u32 arr = (u32)DaioctaGuest<J3DMtxBuffer_hd>(blk)->mpAnmMtx;
    u16 flags = (u16)DaioctaGuest<J3DMtxBuffer_hd>(blk)->mFlags;
    DaioctaGuest<J3DMtxBuffer_hd>(blk)->mFlags = (u16)(flags | 0x10);
    return jnt * 0x30 + arr;
}
/* a float copied with lfs/stfs and no arithmetic is bit-exact in the recompiled original (a
 * signalling NaN is not quieted): a word copy */
static inline void fcpy(u32 dst, u32 src) { store<u32>(dst, load<u32>(src)); }
/* mDoMtx_stack_c::multVecZero(dst): the translation column */
static void oct_multVecZero(u32 dst) {
    fcpy(dst + 0, A(&l_matrixStack) + 0x0C);
    fcpy(dst + 4, A(&l_matrixStack) + 0x1C);
    fcpy(dst + 8, A(&l_matrixStack) + 0x2C);
}
/* matrix assignment: twelve loads, then twelve stores (bit-exact) */
static void oct_mtxCopy(u32 dst, u32 src) {
    u32 v[12];
    for (int i = 0; i < 12; i++)
        v[i] = load<u32>(src + 4 * i);
    for (int i = 0; i < 12; i++)
        store<u32>(dst + 4 * i, v[i]);
}
static inline void oct_mtxCopyFromStack(u32 dst) { oct_mtxCopy(dst, A(&l_matrixStack)); }
static inline void oct_setBaseTRMtx(u32 model) {
    oct_mtxCopy(A(&DaioctaGuest<J3DModel>(model)->mBaseTransformMtx), A(&l_matrixStack));
}
/* the same copy where the recompiled original keeps the values in FPRs (signalling NaNs quieted) */
static inline void oct_setBaseTRMtxQ(u32 model) {
    mtx_copy(&DaioctaGuest<J3DModel>(model)->mBaseTransformMtx, &l_matrixStack);
}
/* J3DModel::setBaseScale (scale at +0xBC) */
static void oct_setBaseScale(u32 model, u32 s) {
    u32 x = load<u32>(s), y = load<u32>(s + 4), z = load<u32>(s + 8);
    store<u32>(model + 0xBC, x);
    store<u32>(model + 0xC4, z);
    store<u32>(model + 0xC0, y);
}
// HD: out-of-line actor search keeps its output argument.
static inline BOOL oct_searchByID(u32 id, be<u32>* out) {
    return DaioctaHD_fopAcM_SearchByID(id, out);
}
/* fopAcM_SearchByName(fpcNm_SHIP_e) (HD inline: fpcSch_JudgeForPName with the name on the stack) */
static u32 oct_searchShip(gabi::Local<be<s16>>& key) {
    *key = (s16)0xA5;
    return gabi::ea(fopAcIt_Judge(0x025E121C, key.get()));
}
/* fopAcM_seStart(this, id, 0) (HD inline: reverb of the current room, sound at eyePos) */
static void oct_seStart(u32 t, u32 id, u32 pos) {
    auto* i_this = T(t);
    s32 reverb =
        DaioctaHD_dComIfGp_getReverb((s32)(s8)i_this->current.roomNo); /* dComIfGp_getReverb */
    DaioctaHD_mDoAud_seStartAt(id, pos, 0u, reverb);
}
/* inline strcmp (GHS) */
static s32 oct_strcmp(u32 a, u32 b) {
    for (;;) {
        u8 x = load<u8>(a++);
        u8 y = load<u8>(b++);
        if (x != y)
            return (s32)x - (s32)y;
        if (x == 0)
            return 0;
    }
}
/* mDoExt_McaMorf::isStop() on the HD frame control (rate +0x98, state +0xA7) */
static bool oct_morfIsStop(u32 morf) {
    if ((u8)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mState & 1)
        return true;
    return (f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mRate == 0.0f;
}
/* dVibration_c (play+0x599C) */
static void oct_startShock(s32 strength, gabi::Local<cXyz>& pos) {
    u32 vib = A(&DaioctaGuest<dComIfG_inf_c>(oct_play())->play.mVibration);
    pos->x = 0.0f;
    pos->y = 1.0f;
    pos->z = 0.0f;
    DaioctaHD_dVibration_c_StartShock(vib, strength, -0x21, pos.get());
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
    auto* i_this = T(t);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, type, &i_this->current.pos, &i_this->tevStr);
}

/* forward declarations (calls between the unit's functions are guest calls) */
static s32 cLib_calcTimer_i(be<s32>* timer);

static inline void __construct_array(u32 p, u32 n, u32 size, u32 ctor) {
    DaioctaHD_construct_array(p, n, size, ctor);
}
static inline void __destroy_arr(u32 p, u32 n, u32 size, u32 dtor) {
    DaioctaHD_destroy_arr(p, n, size, dtor, 0u, 0u);
}

/* the eye search of isLivingEye/isDead/isDamageEye/isDamageBombEye/setRotEye: fopAcM_SearchByID
 * inline */
static inline u32 oct_eye(u32 t, int i) {
    auto* i_this = T(t);
    return A(fopAcM_SearchByID((u32)(*DaioctaArray(i_this->mDaioctaEyePcId, i))));
}

// HD: keep the maximum read before assigning the first axis's eye number.
static void oct_rndRotEyeAxis(daDaiocta_Eye_c* eye, be<s16>& target, be<s16>& current,
                              be<s16>& minimum, be<s16>& maximum, bool storeNo, s32 no) {
    s16 max = maximum;
    if (storeNo)
        eye->mEyeNo = no;
    s16 value = (s16)gabi::ftoi(cM_rndF((f32)max));
    s16 previous = current;
    s16 min = minimum;
    target = value;
    if (std::fabs((f32)((s32)value - (s32)previous)) < (f32)min)
        target = (s16)(value + min);
}

/* function-local static GXColor (HD: initialised on first use, guard word + memcpy) */
static void oct_staticColor(u32 guard, u32 dst, u32 src) {
    if (load<u32>(guard) == 0) {
        store<u32>(guard, 1);
        memcpy_g(gabi::at<void>(dst), gabi::at<void>(src), 4); /* memcpy */
    }
}

/* the eye flag scans */

/* modeWait / modeDamage / modeDamageBomb: the shared transition checks */
static void oct_checkTransitions(daDaiocta_c* i_this, bool hioDelete) {
    if (i_this->isDead() || (hioDelete && l_HIO.m006 != 0)) {
        i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
    } else if (i_this->isDamageEye()) {
        i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE);
    } else if (i_this->isDamageBombEye()) {
        i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE_BOMB);
    }
}

static void oct_damageEnd(daDaiocta_c* i_this) {
    if (i_this->mPrmIdx == 2) {
        f32 d = std::fabs(gabi::fsubs_ppc((f32)i_this->current.pos.y, i_this->mWaterY));
        i_this->modeProc(daDaiocta_c::PROC_INIT_e,
                         (f64)d > 0.1 ? daDaiocta_c::MODE_APPEAR : daDaiocta_c::MODE_WAIT);
    }
}

/* event manager (play+0x52C4) */
static inline s32 oct_getMyStaffId(u32 name) {
    return DaioctaHD_dEvent_manager_c_getMyStaffId(oct_evm(), name, 0u, 0u);
}
static inline u32 oct_getMyNowCutName(s32 staff) {
    return DaioctaHD_dEvent_manager_c_getMyNowCutName(oct_evm(), staff);
}
static inline void oct_cutEnd(s32 staff) { DaioctaHD_dEvent_manager_c_cutEnd(oct_evm(), staff); }
static inline BOOL oct_endCheck(u32 name) {
    return DaioctaHD_dEvent_manager_c_endCheckOld(oct_evm(), name);
} /* HD: endCheckOld */
/* fopAcM_orderOtherEvent(this, name) (HD: 025D77DC with flag 1 and hind 0xFFFF) */
static inline void oct_orderOtherEvent(u32 t, u32 name) {
    DaioctaHD_fopAcM_orderOtherEvent2(t, name, 1u, 0xFFFFu);
}

static inline void oct_initStartPos(u32 ship, u32 pos, s16 angle) {
    DaioctaHD_daShip_c_initStartPos(ship, pos, (s32)angle);
}
static inline f32 REG12_F(int i) { return REG_F(12, i); }
static inline s16 REG12_S(int i) { return REG_S(12, i); }

/* 0211CD80 */
static u32 daDaiocta_HIO_ct(u32 self) {
    if (self == 0) {
        self = A(operator_new(0x108));
        if (self == 0)
            return 0;
    }
    auto* hio = gabi::at<daDaiocta_HIO_c>(self);
    hio->__vtbl = 0x1000D044; /* mDoHIO_entry_c vtable */
    hio->m004 = 0;
    hio->m008 = 0;
    hio->m006 = 0;
    hio->m005 = 0;
    hio->mSphRadii[0] = 600.0f;
    hio->mSphRadii[1] = 500.0f;
    hio->mSphRadii[2] = 450.0f;
    hio->mSphRadii[3] = 350.0f;
    hio->mSphRadii[4] = 350.0f;
    hio->mSphRadii[5] = 300.0f;
    hio->mCpsRadii[0] = 60.0f;
    hio->mCpsRadii[1] = 60.0f;
    hio->mCpsRadii[2] = 60.0f;
    hio->mCpsRadii[3] = 60.0f;
    hio->mCpsRadii[4] = 60.0f;
    hio->mCpsRadii[5] = 60.0f;
    hio->mCpsRadii[6] = 60.0f;
    hio->mCpsRadii[7] = 60.0f;
    hio->mCpsRadii[8] = 60.0f;
    hio->mCpsRadii[9] = 60.0f;
    hio->m05C = 100.0f;
    hio->m060 = 100.0f;
    hio->m064 = 250.0f;
    hio->m068 = 100.0f;
    hio->m06C = 150.0f;
    hio->m070 = 100.0f;
    hio->m074 = 150.0f;
    hio->mDefaultAppearRadius = 5000.0f;
    hio->mMinAppearRadius = 4500.0f;
    hio->m080 = 30.0f;
    hio->m084 = 1800.0f;
    hio->m088 = 40.0f;
    hio->m08C = 1.0f;
    hio->m090 = 0.02f;
    hio->m094 = 150.0f;
    hio->m0FC.x = 0.0f;
    hio->m0FC.y = 5.0f;
    hio->m0FC.z = 700.0f;
    hio->m0BC[1] = 8.0f;
    hio->m0BC[2] = 8.0f;
    hio->m0BC[3] = 3.0f;
    hio->m0BC[4] = 3.0f;
    hio->m0BC[5] = 3.0f;
    hio->m0BC[6] = 7.0f;
    hio->m0BC[7] = 9.0f;
    hio->m098[1] = 1.0f;
    hio->m098[2] = 0.5f;
    hio->m098[3] = 0.75f;
    hio->m098[4] = 0.625f;
    hio->m098[5] = 0.625f;
    hio->m098[6] = 0.5f;
    hio->m098[7] = 1.0f;
    hio->m098[8] = 1.0f;
    hio->m0E0 = 20.0f;
    hio->m0E4 = 300.0f;
    hio->m0E8 = 1200.0f;
    hio->m0EC = 1.0f;
    hio->m0F0 = 2.0f;
    hio->m0F4 = 10.0f;
    hio->m0F8 = 30.0f;
    hio->mRotEyeTimerMin = 0x1E;    /* mRotEyeTimerMin */
    hio->mRotEyeTimerAddMax = 0x3C; /* mRotEyeTimerAddMax */
    hio->m010 = 0x100;
    hio->m012 = 0x1500;
    hio->m014 = -0x800;
    hio->m016 = 0x1E;
    hio->m018 = 0x2D;
    return self;
}

/* 021189CC */
static void coHit_CB(daDaiocta_c* i_this, u32 objInf, fopAc_ac_c* i_actor, u32 objInf2) {
    i_this->_coHit(i_actor);
}

/* 02118958 */
void daDaiocta_c::_coHit(fopAc_ac_c* i_actor) {
    auto* i_this = this;
    u32 a = A(i_actor);
    if (a == 0)
        return;
    if (s16(DaioctaGuest<fopAc_ac_c>(a)->mProcName) != fpcNm_BOMB_e)
        return;                          /* fpcNm_BOMB_e */
    u32 m = (u32)(s32)i_this->mMode - 1; /* APPEAR, WAIT, DAMAGE, DAMAGE_BOMB */
    if (m > 3)
        return;
    if (DaioctaHD_daBomb_c_chk_state(a, 4) == 0)
        return; /* daBomb_c::chk_state(STATE_4) */
    i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE_BOMB);
}

/* 02118CE0 */
static BOOL nodeControl_CB(J3DNode* i_nodeP, int i_calcTiming) {
    if (i_calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 model = A(j3dSys.getModel()); /* j3dSys.getModel() */
        u32 daiocta = (u32)gabi::at<J3DModel>(model)->mUserArea;
        if (daiocta == 0)
            oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x1A4,
                       gabi::guest_addr("daiocta != (0)"));
        T(daiocta)->_nodeControl(i_nodeP, gabi::at<J3DModel>(model));
    }
    return TRUE;
}

/* 021189D4 */
void daDaiocta_c::_nodeControl(J3DNode* i_nodeP, J3DModel* i_modelP) {
    auto* i_this = this;
    u32 model = A(i_modelP);
    u32 jnt_no = load<u16>(A(J3DNode_toJoint(i_nodeP)) + 4);
    if ((s32)jnt_no >= 37)
        return; /* HD: bounds check */
    PSMTXCopy(gabi::at<Mtx34>(oct_getAnmMtx(model, jnt_no)), &l_matrixStack);
    oct_multVecZero(A(&i_this->mSphCenters) + jnt_no * 12);

    gabi::Local<cXyz> base;
    u32 dst = 0;
    switch (jnt_no) {
    case 30:
        base->x = 450.0f;
        base->y = 0.0f;
        base->z = 0.0f;
        dst = A(&i_this->m21A0);
        break; /* m21A0 */
    case 36:
        base->x = 450.0f;
        base->y = 0.0f;
        base->z = 0.0f;
        dst = A(&i_this->m21AC);
        break; /* m21AC */
    case 20:
        fcpy(A(&base->x), A(&l_HIO.m094));
        base->y = 0.0f;
        base->z = 0.0f;
        dst = A(&i_this->m2434);
        break; /* m2434 */
    case 6:
        base->x = 450.0f;
        base->y = 0.0f;
        base->z = 0.0f;
        dst = A(&i_this->m21B8);
        break; /* m21B8 */
    case 9:
        base->x = 300.0f;
        base->y = -200.0f;
        base->z = 0.0f;
        dst = A(&i_this->m21C4);
        break; /* m21C4 */
    case 7:
        base->x = 300.0f;
        base->y = 0.0f;
        base->z = 0.0f;
        dst = A(&i_this->m21D0);
        break; /* m21D0 */
    case 10:
        base->x = 300.0f;
        base->y = 200.0f;
        base->z = 0.0f;
        dst = A(&i_this->m21DC);
        break; /* m21DC */
    case 8:
        base->x = 300.0f;
        base->y = 0.0f;
        base->z = 0.0f;
        dst = A(&i_this->m21E8);
        break; /* m21E8 */
    }
    if (dst != 0)
        PSMTXMultVec(&l_matrixStack, base.get(), gabi::at<cXyz>(dst));

    PSMTXCopy(&l_matrixStack, J3DSys::mCurrentMtx);     /* j3dSys.mCurrentMtx */
    oct_mtxCopyFromStack(oct_getAnmMtx(model, jnt_no)); /* setAnmMtx */
}

/* 021192C8 */
static BOOL createHeap_CB(daDaiocta_c* i_this) { return i_this->_createHeap(); }

/* 02119248 */
BOOL daDaiocta_c::_createHeap() {
    auto* i_this = this;
    if (!i_this->createBodyHeap())
        return FALSE;
    if (!i_this->createSuikomiHeap())
        return FALSE;
    if (!i_this->createAwaHeap())
        return FALSE;
    if (!i_this->createArrowHitHeap())
        return FALSE;
    return TRUE;
}

/* 02118F90 */
BOOL daDaiocta_c::createAwaHeap() {
    auto* i_this = this;
    u32 modelData = oct_getRes(dRes_INDEX_DAIOCTA_BDL_GAWA00_e); /* GAWA00 bdl */
    if (modelData == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x207,
                   gabi::guest_addr("modelData != (0)"));
    u32 bck = oct_getRes(dRes_INDEX_DAIOCTA_BCK_GAWA00_e);
    if (bck == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x20B, gabi::guest_addr("bck != 0"));
    u32 btk = oct_getRes(dRes_INDEX_DAIOCTA_BTK_GAWA00_e);
    if (btk == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x20F, gabi::guest_addr("btk != 0"));
    u32 brk = oct_getRes(dRes_INDEX_DAIOCTA_BRK_GAWA00_e);
    if (brk == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x213, gabi::guest_addr("brk != 0"));
    for (int i = 0; i < 30; i++) {
        u32 model =
            A(mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11000222));
        DaioctaArray(i_this->mpAwaModels, i)->v = model;
        if (model == 0)
            return FALSE;
        /* mDoExt_bckAnm::init(data, bck, TRUE, EMode_NONE, 1.0f, 0, -1, false) */
        if (DaioctaHD_mDoExt_bckAnm_init(A(DaioctaArray(i_this->mAwaBckAnms, i)), modelData, bck, 1,
                                         0, 1.0f, 0, -1, 0) == 0)
            return FALSE;
        /* mDoExt_btkAnm::init (HD: 9th argument on the stack) */
        if (DaioctaHD_mDoExt_btkAnm_initHD(A(DaioctaArray(i_this->mAwaBtkAnms, i)), modelData, btk,
                                           1, 0, 1.0f, 0, -1, 0, 0) == 0)
            return FALSE;
        /* mDoExt_brkAnm::init (HD: 9th argument on the stack) */
        if (DaioctaHD_mDoExt_brkAnm_initHD(A(DaioctaArray(i_this->mAwaBrkAnms, i)), modelData, brk,
                                           1, 0, 1.0f, 0, -1, 0, 0) == 0)
            return FALSE;
    }
    return TRUE;
}

/* 02118E90 */
BOOL daDaiocta_c::createSuikomiHeap() {
    auto* i_this = this;
    u32 modelData = oct_getRes(dRes_INDEX_DAIOCTA_BDL_GDO_SUI00_e); /* GDO_SUI00 bdl */
    if (modelData == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x22A,
                   gabi::guest_addr("modelData != (0)"));
    u32 model = A(mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11000222));
    i_this->mpSuikomiModel.v = model;
    if (model == 0)
        return FALSE;
    if (DaioctaHD_dLib_brkInit(modelData, A(&i_this->mBrkAnm2), A(m_arc_name),
                               dRes_INDEX_DAIOCTA_BRK_GDO_SUI00_e) == 0 || /* dLib_brkInit */
        DaioctaHD_dLib_btkInit(modelData, A(&i_this->mBtkAnm), A(m_arc_name),
                               dRes_INDEX_DAIOCTA_BTK_GDO_SUI00_e) == 0) /* dLib_btkInit */
        return FALSE;
    return TRUE;
}

/* 02118D60 */
BOOL daDaiocta_c::createBodyHeap() {
    auto* i_this = this;
    u32 t = A(i_this);
    u32 modelData = oct_getRes(dRes_INDEX_DAIOCTA_BDL_DO_MAIN1_e); /* DO_MAIN1 bdl */
    if (modelData == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x246,
                   gabi::guest_addr("modelData != (0)"));
    /* HD: 02587688 is dLib_brkInit (the matcher swapped it with dLib_btkInit) */
    if (DaioctaHD_dLib_brkInit(modelData, A(&i_this->mBrkAnm1), A(m_arc_name),
                               dRes_INDEX_DAIOCTA_BRK_WAIT1_e) == 0)
        return FALSE;
    u32 morf = DaioctaHD_mDoExt_McaMorf_mDoExt_McaMorf(0u, modelData, 0u, 0u, 0u, -1, 1.0f, 0, -1,
                                                       1, 0u, 0x80000u, 0x11000222u);
    i_this->mpMorf.v = morf;
    if (morf == 0 || (u32)gabi::at<mDoExt_McaMorf>(morf)->mpModel.v == 0)
        return FALSE;
    DaioctaGuest<J3DModel>((u32)gabi::at<mDoExt_McaMorf>(morf)->mpModel.v)->mUserArea =
        t; /* setUserArea(this) */
    return TRUE;
}

/* 021191DC */
BOOL daDaiocta_c::createArrowHitHeap() {
    auto* i_this = this;
    u32 model = u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v);
    u32 jh = DaioctaHD_JntHit_create(model, A(&search_data),
                                     0x11); /* JntHit_create(model, search_data, 17) */
    i_this->mpJntHit = jh;
    if (jh == 0)
        return FALSE;
    i_this->jntHit = jh; /* fopAcM_SetJntHit */
    return TRUE;
}

/* 0211980C */
void daDaiocta_c::setMtx() {
    auto* i_this = this;
    gabi::Local<be<u32>> actor_p;
    gabi::Local<Mtx34> mtx;
    for (int i = 0; i < 12; i++) {
        if ((u8)((*DaioctaArray(i_this->mEyeAlloc, i))) != 0 &&
            oct_searchByID((u32)((*DaioctaArray(i_this->mDaioctaEyePcId, i))), actor_p.get())) {
            u32 model = u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v);
            u32 idx = (u32)((*DaioctaArray(i_this->mAnmMtxIndices, i)));
            PSMTXCopy(gabi::at<Mtx34>(oct_getAnmMtx(model, idx)), &l_matrixStack);
            oct_multVecZero(A(&DaioctaGuest<fopAc_ac_c>(*actor_p)->current.pos));
            DaioctaHD_mDoMtx_inverseTranspose(A(&l_matrixStack),
                                              mtx.get()); /* cMtx_inverseTranspose */
            DaioctaHD_mDoMtx_MtxToRot(
                mtx.get(), A(&DaioctaGuest<fopAc_ac_c>(*actor_p)
                                  ->shape_angle)); /* mDoMtx_MtxToRot(mtx, &shape_angle) */
        }
    }
    PSMTXTrans(&l_matrixStack, (f32)i_this->current.pos.x, (f32)i_this->current.pos.y,
               (f32)i_this->current.pos.z);
    mDoMtx_ZXYrotM(&l_matrixStack, (s16)i_this->shape_angle.x, (s16)i_this->shape_angle.y,
                   (s16)i_this->shape_angle.z);
    oct_setBaseTRMtxQ(u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v));
    i_this->setSuikomiMtx();
    i_this->setAwaMtx();
}

/* 02119630 */
void daDaiocta_c::setSuikomiMtx() {
    auto* i_this = this;
    PSMTXTrans(&l_matrixStack, (f32)i_this->current.pos.x, (f32)i_this->mWaterY,
               (f32)i_this->current.pos.z);
    mDoMtx_ZXYrotM(&l_matrixStack, (s16)i_this->shape_angle.x, (s16)i_this->shape_angle.y,
                   (s16)i_this->shape_angle.z);
    mDoMtx_stack_transM(l_HIO.m0FC.x, l_HIO.m0FC.y, l_HIO.m0FC.z);
    oct_setBaseTRMtx((u32)i_this->mpSuikomiModel.v);
}

/* 0211970C */
void daDaiocta_c::setAwaMtx() {
    auto* i_this = this;
    for (int i = 0; i < l_HIO.m016; i++) {
        u32 model = (u32)(DaioctaArray(i_this->mpAwaModels, i)->v);
        /* here the recompiled copy goes through the FPRs (a signalling NaN is quieted) */
        J3DModel_setBaseScale(gabi::at<J3DModel>(model), DaioctaArray(i_this->mAwaScale, i));
        u32 tr = A(DaioctaArray(i_this->mAwaTranslation, i));
        PSMTXTrans(&l_matrixStack, (f32)gabi::at<cXyz>(tr)->x, (f32)gabi::at<cXyz>(tr)->y,
                   (f32)gabi::at<cXyz>(tr)->z);
        oct_setBaseTRMtxQ((u32)(DaioctaArray(i_this->mpAwaModels, i)->v));
    }
}

/* 02119990 */
void daDaiocta_c::initMtx() {
    auto* i_this = this;
    oct_setBaseScale(u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v),
                     A(&i_this->scale));
    oct_setBaseScale((u32)i_this->mpSuikomiModel.v, A(&i_this->scale));
    i_this->setMtx();
    J3DModel_calc(
        gabi::at<J3DModel>(u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v)));
    J3DModel_calc(gabi::at<J3DModel>((u32)i_this->mpSuikomiModel.v));
}

/* 0211A9A0 */
void daDaiocta_c::setEffect(u32 effect_name) {
    auto* i_this = this;
    u32 t = A(i_this);
    gabi::Local<GXColor> amb;
    gabi::Local<GXColor> dif;
    gabi::Local<cXyz> pos;
    gabi::Local<csXyz> ang;
    DaioctaHD_dKy_get_seacolor(amb.get(), dif.get()); /* dKy_get_seacolor */
    s32 add = l_HIO.m018;
    u32 ca = A(amb.get());
    s32 r = (u8)(amb->r) + add;
    s32 g = (u8)(amb->g) + add;
    s32 b = (u8)(amb->b) + add;
    if (r >= 0xFF)
        r = 0xFF;
    if (g >= 0xFF)
        g = 0xFF;
    if (b >= 0xFF)
        b = 0xFF;
    u32 pp = A(pos.get());
    DaioctaBits<u32>(pos->x) = (u32)DaioctaBits<u32>(i_this->current.pos.x);
    DaioctaBits<u32>(pos->y) = (u32)DaioctaBits<u32>(i_this->mWaterY);
    fcpy(pp + 8, t + 0x31C);
    u32 pa = A(ang.get());
    DaioctaBits<u16>(ang->x) = (u16)DaioctaBits<u16>(i_this->shape_angle.x);
    DaioctaBits<u16>(ang->y) = (u16)DaioctaBits<u16>(i_this->shape_angle.y);
    DaioctaBits<u16>(ang->z) = (u16)DaioctaBits<u16>(i_this->shape_angle.z);
    amb->r = (u8)r;
    amb->b = (u8)b;
    amb->g = (u8)g;
    if (effect_name < 0x81FE)
        return;
    if (effect_name == 0x81FE) { /* ID_IT_SN_DO_APPSHIBUKI00 */
        if (u32(i_this->mParticleCallback.mpEmitter.v) == 0) {
            dComIfGp_particle_set((u16)effect_name, pos.get(), nullptr, nullptr, 0xFF,
                                  reinterpret_cast<dPa_levelEcallBack*>(&i_this->mParticleCallback),
                                  -1, amb.get());
        }
    } else if (effect_name <= 0x8205) { /* HIT{A,B,C}_SHIBUKI{A,B}00, DOWNSHIBUKI00 */
        dComIfGp_particle_set((u16)effect_name, pos.get(), nullptr, nullptr, 0xFF, nullptr, -1,
                              amb.get());
    } else if ((effect_name >= 0x8207 && effect_name <= 0x8209) || effect_name == 0x82CC) {
        /* SUIKOMI{A,B,C}00, HAKIDASHI00 */
        dComIfGp_particle_set((u16)effect_name, pos.get(), ang.get(), nullptr, 0xFF, nullptr, -1,
                              amb.get());
    }
}

/* 021193F8 */
void daDaiocta_c::setAwaRandom(s32 i_awaIndex) {
    auto* i_this = this;
    f32 f12 = l_HIO.m0F0;
    f32 f31 = l_HIO.m0EC;
    f32 f30 = l_HIO.m0E4;
    f32 f0 = l_HIO.m0E8;
    f32 f1 = gabi::fsubs_ppc(f12, f31);
    f32 f29 = gabi::fsubs_ppc(f0, f30);
    u32 sc = A(DaioctaArray(i_this->mAwaScale, i_awaIndex));
    f32 s = gabi::fadds_ppc(cM_rndF(f1), f31);
    gabi::at<cXyz>(sc)->x = s; /* mAwaScale[i].setall() */
    gabi::at<cXyz>(sc)->y = s;
    gabi::at<cXyz>(sc)->z = s;
    f32 dist = gabi::fadds_ppc(cM_rndF(f29), f30);
    u16 theta = (u16)gabi::ftoi(cM_rndF(65536.0f));
    u32 tr = A(DaioctaArray(i_this->mAwaTranslation, i_awaIndex));
    gabi::at<cXyz>(tr)->x = gabi::fmadds(dist, cM_ssin(theta), (f32)i_this->current.pos.x);
    fcpy(tr + 4, A(&i_this->mWaterY));
    gabi::at<cXyz>(tr)->z = gabi::fmadds(dist, cM_scos(theta), (f32)i_this->current.pos.z);
}

/* 02119514 */
void daDaiocta_c::initAwa() {
    auto* i_this = this;
    for (int i = 0; i < l_HIO.m016; i++) {
        i_this->setAwaRandom(i);
        f32 f0 = l_HIO.m0F8;
        f32 f31 = l_HIO.m0F4;
        f32 r = cM_rndF(gabi::fsubs_ppc(f0, f31));
        *DaioctaArray(i_this->mAwaTimers, i) = gabi::ftoi(gabi::fadds_ppc(r, f31));
        /* setFrame(getEndFrame()) on the bck, btk and brk animations */
        u32 bck = A(DaioctaArray(i_this->mAwaBckAnms, i));
        gabi::at<mDoExt_bckAnm>(bck)->mFrameCtrl.mFrame =
            (f32)(s16)gabi::at<mDoExt_bckAnm>(bck)->mFrameCtrl.mEnd;
        u32 btk = A(DaioctaArray(i_this->mAwaBtkAnms, i));
        gabi::at<mDoExt_btkAnm>(btk)->mFrameCtrl.mFrame =
            (f32)(s16)gabi::at<mDoExt_btkAnm>(btk)->mFrameCtrl.mEnd;
        u32 brk = A(DaioctaArray(i_this->mAwaBrkAnms, i));
        gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame =
            (f32)(s16)gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mEnd;
    }
}

/* 02119F84 */
void daDaiocta_c::execAwa() {
    auto* i_this = this;
    for (int i = 0; i < l_HIO.m016; i++) {
        if (cLib_calcTimer_i(DaioctaArray(i_this->mAwaTimers, i)) == 0) {
            u32 bck = A(DaioctaArray(i_this->mAwaBckAnms, i));
            mDoExt_baseAnm_play(gabi::at<void>(bck));
            u32 btk = A(DaioctaArray(i_this->mAwaBtkAnms, i));
            mDoExt_baseAnm_play(gabi::at<void>(btk));
            u32 brk = A(DaioctaArray(i_this->mAwaBrkAnms, i));
            mDoExt_baseAnm_play(gabi::at<void>(brk));
            if (i_this->m31C4 != 0 || l_HIO.m008 != 0) {
                /* mAwaBckAnms[i].isStop() */
                if (((u8)gabi::at<mDoExt_bckAnm>(bck)->mFrameCtrl.mState & 1) ||
                    (f32)gabi::at<mDoExt_bckAnm>(bck)->mFrameCtrl.mRate == 0.0f) {
                    gabi::at<mDoExt_bckAnm>(bck)->mFrameCtrl.mRate = 1.0f;  /* setPlaySpeed(1.0f) */
                    gabi::at<mDoExt_bckAnm>(bck)->mFrameCtrl.mFrame = 0.0f; /* setFrame(0.0f) */
                    gabi::at<mDoExt_btkAnm>(btk)->mFrameCtrl.mFrame = 0.0f;
                    gabi::at<mDoExt_btkAnm>(btk)->mFrameCtrl.mRate = 1.0f;
                    gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame = 0.0f;
                    gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mRate = 1.0f;
                    f32 f29 = l_HIO.m0F4;
                    f32 f13 = l_HIO.m0F8;
                    f32 r = cM_rndF(gabi::fsubs_ppc(f13, f29));
                    *DaioctaArray(i_this->mAwaTimers, i) = gabi::ftoi(gabi::fadds_ppc(r, f29));
                    i_this->setAwaRandom(i);
                }
            }
        }
    }
}

/* 0211A128 */
bool daDaiocta_c::isLivingEye() {
    auto* i_this = this;
    u32 t = A(i_this);
    bool is_alive = false;
    for (int i = 0; i < 12; i++) {
        if ((u8)((*DaioctaArray(i_this->mEyeAlloc, i))) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye == 0) {
                *DaioctaArray(i_this->mEyeAlloc, i) = 0;
            } else if (gabi::at<daDaiocta_Eye_c>(eye)->mbIsDead == 0) {
                is_alive = true;
            }
        }
    }
    return is_alive;
}

/* 0211BA0C */
bool daDaiocta_c::isDead() {
    auto* i_this = this;
    u32 t = A(i_this);
    s32 count = 0;
    for (int i = 0; i < 12; i++) {
        if ((u8)((*DaioctaArray(i_this->mEyeAlloc, i))) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->mbIsDead == 0)
                count++;
        }
    }
    return count == 0;
}

/* 0211B734 */
bool daDaiocta_c::isDamageEye() {
    auto* i_this = this;
    u32 t = A(i_this);
    bool is_damaged = false;
    for (int i = 0; i < 12; i++) {
        if ((u8)((*DaioctaArray(i_this->mEyeAlloc, i))) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->mbIsDamaged != 0)
                is_damaged = true;
        }
    }
    return is_damaged;
}

/* 0211BAF8 */
bool daDaiocta_c::isDamageBombEye() {
    auto* i_this = this;
    u32 t = A(i_this);
    bool is_damaged = false;
    for (int i = 0; i < 12; i++) {
        if ((u8)((*DaioctaArray(i_this->mEyeAlloc, i))) != 0) {
            u32 eye = oct_eye(t, i);
            if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->mbIsDamagedBomb != 0)
                is_damaged = true;
        }
    }
    return is_damaged;
}

/* 0211A1E8 */
void daDaiocta_c::setRotEye() {
    auto* i_this = this;
    u32 t = A(i_this);
    if (cLib_calcTimer_i(&i_this->mRotEyeTimer) == 0) {
        for (int i = 0; i < 12; i++) {
            if ((u8)((*DaioctaArray(i_this->mEyeAlloc, i))) != 0) {
                u32 eye = oct_eye(t, i);
                if (eye != 0 && gabi::at<daDaiocta_Eye_c>(eye)->mbIsDead == 0) {
                    auto* actor = DaioctaGuest<daDaiocta_Eye_c>(eye);
                    oct_rndRotEyeAxis(actor, actor->mEyeRotTargetX, actor->mCurEyeRot.x,
                                      actor->m2EC, actor->mEyeRndRotMaxX, true, i);
                    oct_rndRotEyeAxis(actor, actor->mEyeRotTargetY, actor->mCurEyeRot.y,
                                      actor->m2EE, actor->mEyeRndRotMaxY, false, 0);
                    oct_rndRotEyeAxis(actor, actor->mEyeRotTargetZ, actor->mCurEyeRot.z,
                                      actor->m2F0, actor->mEyeRndRotMaxZ, false, 0);
                }
            }
        }
        f32 r = cM_rndF((f32)l_HIO.mRotEyeTimerAddMax); /* mRotEyeTimerAddMax */
        i_this->mRotEyeTimer = gabi::ftoi(gabi::fadds_ppc((f32)l_HIO.mRotEyeTimerMin, r));
    }
}

/* 0211A4BC */
void daDaiocta_c::setCollision() {
    auto* i_this = this;
    u32 c = A(&i_this->mSphCenters); /* mSphCenters */
    for (u32 i = 0; i < 6; i++) {
        u32 j = u8(*DaioctaArray(l_sphereJoints.mValues, i)); /* switch table {2, 3, 4, 5, 6, 20} */
        u32 sph = A(DaioctaArray(i_this->mSph, i));
        DaioctaHD_cM3dGSph_SetR(sph + 0x118,
                                f32(*DaioctaArray(l_HIO.mSphRadii, i))); /* SetR(mSphRadii[i]) */
        DaioctaHD_cM3dGSph_SetC(sph + 0x118, c + j * 12);                /* SetC(mSphCenters[j]) */
        dComIfG_Ccsp_Set(gabi::at<void>(sph));
    }
    for (u32 i = 0; i < 10; i++) {
        u32 j = i < 5 ? 25 + i : 26 + i;
        u32 cps = A(DaioctaArray(i_this->mCps, i));
        DaioctaHD_cM3dGCps_SetStartEnd(cps + 0x118, c + j * 12, c + (j + 1) * 12); /* SetStartEnd */
        fcpy(A(&DaioctaGuest<DaioctaCapsule>(cps)->mCps.mRadius),
             A(DaioctaArray(l_HIO.mCpsRadii, i))); /* SetR(mCpsRadii[i]) */
        dComIfG_Ccsp_Set(gabi::at<void>(cps));
    }
    struct CapsuleEnd {
        u32 index;
        u32 startJoint;
        cXyz* end;
        be<f32>* radius;
    } tail[7] = {
        {10, 36, &i_this->m21AC, &l_HIO.m05C}, {11, 30, &i_this->m21A0, &l_HIO.m060},
        {12, 6, &i_this->m21B8, &l_HIO.m064},  {13, 10, &i_this->m21DC, &l_HIO.m068},
        {14, 8, &i_this->m21E8, &l_HIO.m06C},  {15, 9, &i_this->m21C4, &l_HIO.m070},
        {16, 7, &i_this->m21D0, &l_HIO.m074},
    };
    for (int k = 0; k < 7; k++) {
        u32 cps = A(DaioctaArray(i_this->mCps, tail[k].index));
        DaioctaHD_cM3dGCps_SetStartEnd(A(&DaioctaGuest<DaioctaCapsule>(cps)->mCps),
                                       A(DaioctaArray(i_this->mSphCenters, tail[k].startJoint)),
                                       A(tail[k].end));
        fcpy(A(&DaioctaGuest<DaioctaCapsule>(cps)->mCps.mRadius), A(tail[k].radius));
        dComIfG_Ccsp_Set(gabi::at<void>(cps));
    }
}

/* 0211B530 */
void daDaiocta_c::modeHideInit() {
    auto* i_this = this;
    i_this->current.pos.y = gabi::fsubs_ppc(i_this->mWaterY, 3000.0f);
}

/* 0211B548 */
void daDaiocta_c::modeHide() {
    auto* i_this = this;
    f32 dist_xz = fopAcM_searchActorDistanceXZ(i_this, dComIfGp_getPlayer(0));
    if (dist_xz < i_this->mAppearRadius || dist_xz < l_HIO.mMinAppearRadius) {
        if (u32(DaioctaGuest<dComIfG_inf_c>(oct_play())->play.mPlayerStatus[0][0]) &
            0x10000) { /* dComIfGp_checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e) */
            i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_APPEAR);
        }
    }
}

/* 0211B5C0 */
void daDaiocta_c::modeAppearInit() {
    auto* i_this = this;
    i_this->actor_status =
        ((u32)i_this->actor_status & ~0x3Fu) | 0x20; /* fopAcM_SetStatusMap(this, 0) */
    u32 player = A(dComIfGp_getPlayer(0));
    gabi::Local<cXyz> dir;
    gabi::Local<cXyz> norm;
    gabi::Local<cXyz> scaled;
    gabi::Local<cXyz> new_pos;
    cXyz_mi(&i_this->current.pos, dir.get(), gabi::at<cXyz>(player + 0x314));
    DaioctaHD_cXyz_normZP(dir.get(), norm.get()); /* cXyz::normZP */
    cXyz_ml(norm.get(), scaled.get(), l_HIO.mMinAppearRadius);
    cXyz_pl(gabi::at<cXyz>(player + 0x314), new_pos.get(), scaled.get());
    f32 auzu_y = gabi::fadds_ppc(i_this->mWaterY, 2.0f);
    u32 x = (u32)(DaioctaBits<u32>(new_pos->x));
    u32 auzuId = i_this->mAuzuId;
    i_this->attention_info.flags = 0; /* attention_info.flags */
    u32 z = (u32)(DaioctaBits<u32>(new_pos->z));
    DaioctaBits<u32>(i_this->current.pos.x) = x;
    DaioctaBits<u32>(i_this->current.pos.z) = z;
    {
        gabi::Local<be<u32>> auzu_p;
        if (oct_searchByID(auzuId, auzu_p.get()) && *auzu_p != 0) {
            u32 a = *auzu_p;
            DaioctaGuest<fopAc_ac_c>(a)->current.pos.y = auzu_y;
            DaioctaBits<u32>(DaioctaGuest<fopAc_ac_c>(a)->current.pos.x) = x;
            DaioctaBits<u32>(DaioctaGuest<fopAc_ac_c>(a)->current.pos.z) = z;
        }
    }
    i_this->setEffect(0x81FE);       /* ID_IT_SN_DO_APPSHIBUKI00 */
    DaioctaHD_mDoAud_bgmAllMute(90); /* mDoAud_bgmAllMute(90) */
    gabi::Local<be<u32>> eye_p;
    for (int i = 0; i < 12; i++) {
        if (oct_searchByID((u32)((*DaioctaArray(i_this->mDaioctaEyePcId, i))), eye_p.get()) &&
            *eye_p != 0) {
            gabi::at<daDaiocta_Eye_c>(*eye_p)->mbAppear = 1; /* mbAppear */
        }
    }
}

/* 0211B7D8 */
void daDaiocta_c::modeAppear() {
    auto* i_this = this;
    u32 t = A(i_this);
    if (i_this->isDamageEye())
        i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DAMAGE);
    if (u32(i_this->mParticleCallback.mpEmitter.v) != 0) { /* mParticleCallback.getEmitter() */
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        DaioctaHD_dKy_get_seacolor(amb.get(), dif.get()); /* dKy_get_seacolor */
        u32 ca = A(amb.get());
        s32 add = l_HIO.m018;
        s32 r = (u8)(amb->r) + add;
        s32 g = (u8)(amb->g) + add;
        s32 b = (u8)(amb->b) + add;
        if (r >= 0xFF)
            r = 0xFF;
        if (g >= 0xFF)
            g = 0xFF;
        if (b >= 0xFF)
            b = 0xFF;
        amb->g = (u8)g;
        amb->b = (u8)b;
        u32 em = u32(i_this->mParticleCallback.mpEmitter.v);
        amb->r = (u8)r;
        /* JPABaseEmitter::setGlobalPrmColor(r, g, b) */
        DaioctaGuest<DaioctaEmitterColors>(em)->mGlobalPrmColor.r = (u8)r;
        DaioctaGuest<DaioctaEmitterColors>(em)->mGlobalPrmColor.b = (u8)b;
        DaioctaGuest<DaioctaEmitterColors>(em)->mGlobalPrmColor.g = (u8)g;
    }
    u32 auzu_p = A(fopAcM_SearchByID(i_this->mAuzuId));
    if (auzu_p != 0) {
        f32 s =
            (f32)DaioctaGuest<daObjAuzu::Act_c>(auzu_p)->mScaleAnimFactor; /* mScaleAnimFactor */
        DaioctaGuest<daObjAuzu::Act_c>(auzu_p)->mbToAppear = 1; /* to_appear(): mbToAppear = true */
        if (s > 0.999f) {                                       /* is_appear() */
            gabi::Local<cXyz> sp2C;
            DaioctaBits<u32>(sp2C->x) = (u32)DaioctaBits<u32>(i_this->current.pos.x);
            fcpy(A(sp2C.get()) + 8, t + 0x31C);
            DaioctaBits<u32>(sp2C->y) = (u32)DaioctaBits<u32>(i_this->mWaterY);
            DaioctaHD_cLib_addCalcPos2(A(&i_this->current.pos), sp2C.get(), 0.1f,
                                       f32(l_HIO.m080)); /* cLib_addCalcPos2 */
            gabi::Local<cXyz> d;
            f32 dist = oct_abs(A(&i_this->current.pos), A(sp2C.get()), d);
            if (dist < 1900.0f && i_this->m26C0 == 0) {
                oct_seStart(t, 0x58C2 /* JA_SE_CM_DO_ENTER */, A(&i_this->eyePos));
                i_this->m26C0 = 1;
                DaioctaHD_mDoAud_subBgmStart(
                    0x8000002Bu); /* mDoAud_subBgmStart(JA_BGM_DIOCTA_BATTLE) */
            }
            if (!((f64)dist > 0.1)) {
                i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_WAIT);
            } else {
                s16 target = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                cLib_addCalcAngleS2(gabi::at<be<s16>>(t + 0x32A), target, 4, l_HIO.m010);
            }
        }
    }
}

/* 0211BAB4 */
void daDaiocta_c::modeWaitInit() {
    auto* i_this = this;
    if (i_this->isDead())
        i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
}

/* 0211BB9C */
void daDaiocta_c::modeWait() {
    auto* i_this = this;
    fcpy(A(&i_this->current.pos.y), A(&i_this->mWaterY));
    oct_checkTransitions(i_this, true);
}

/* 0211BC40 */
void daDaiocta_c::modeDamageInit() {
    auto* i_this = this;
    if (i_this->isDead()) {
        i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
        return;
    }
    i_this->mPrmIdx = 2;
    i_this->setAnm();
    i_this->mPrmIdx = 4;
    i_this->setEffect(0x8201); /* ID_IT_SN_DO_HITB_SHIBUKIA00 */
    i_this->setEffect(0x8202); /* ID_IT_SN_DO_HITB_SHIBUKIB00 */
}

/* 0211BCD8 */
void daDaiocta_c::modeDamage() {
    auto* i_this = this;
    fcpy(A(&i_this->current.pos.y), A(&i_this->mWaterY));
    oct_checkTransitions(i_this, false);
    oct_damageEnd(i_this);
}

/* 0211BDB0 */
void daDaiocta_c::modeDamageBombInit() {
    auto* i_this = this;
    if (i_this->isDead()) {
        i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DELETE);
        return;
    }
    oct_seStart(A(i_this), 0x2828 /* JA_SE_LK_LAST_HIT */, A(i_this) + 0x37C);
    i_this->mPrmIdx = 2;
    i_this->setAnm();
    i_this->mPrmIdx = 3;
    i_this->setEffect(0x81FF); /* ID_IT_SN_DO_HITA_SHIBUKIA00 */
    i_this->setEffect(0x8200); /* ID_IT_SN_DO_HITA_SHIBUKIB00 */
}

/* 0211BE6C */
void daDaiocta_c::modeDamageBomb() {
    auto* i_this = this;
    fcpy(A(&i_this->current.pos.y), A(&i_this->mWaterY));
    oct_checkTransitions(i_this, false);
    oct_damageEnd(i_this);
}

/* 0211C444 */
void daDaiocta_c::modeDemoInit() {
    auto* i_this = this;
    i_this->attention_info.flags = 0; /* attention_info.flags */
    i_this->setEffect(0x8207);        /* ID_IT_SN_DO_SUIKOMIA00 */
    i_this->setEffect(0x8208);        /* ID_IT_SN_DO_SUIKOMIB00 */
    i_this->setEffect(0x8209);        /* ID_IT_SN_DO_SUIKOMIC00 */
    u32 model_data_p = u32(DaioctaGuest<J3DModel>(i_this->mpSuikomiModel.v)->mModelData.v);
    u32 brk = oct_getRes(dRes_INDEX_DAIOCTA_BRK_GDO_SUI00_e); /* GDO_SUI00 brk */
    if (brk == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x549, gabi::guest_addr("brk != (0)"));
    u32 btk = oct_getRes(dRes_INDEX_DAIOCTA_BTK_GDO_SUI00_e); /* GDO_SUI00 btk */
    if (btk == 0)
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x54C, gabi::guest_addr("btk != (0)"));
    DaioctaHD_mDoExt_brkAnm_initDemo(A(&i_this->mBrkAnm2), model_data_p, brk, 1, 0, 1.0f, 0, -1, 1,
                                     0); /* mBrkAnm2.init */
    DaioctaHD_mDoExt_btkAnm_initDemo(A(&i_this->mBtkAnm), model_data_p, btk, 1, 0, 1.0f, 0, -1, 1,
                                     0); /* mBtkAnm.init */
}

/* 0211C59C */
void daDaiocta_c::modeDemo() {
    auto* i_this = this;
    u32 t = A(i_this);
    gabi::Local<be<s16>> nameKey1;
    nameKey1->operator=((s16)0xA5);
    fcpy(t + 0x318, A(&i_this->mWaterY));
    u32 ship_p_1 =
        A(fopAcIt_Judge(0x025E121C, nameKey1.get())); /* fopAcM_SearchByName(fpcNm_SHIP_e) */
    if (ship_p_1 == 0) {                              /* HD: null check */
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x559, gabi::guest_addr("ship != (0)"));
        return;
    }
    /* m21F4 = ship->current.pos; m2200 = ship->current.angle */
    DaioctaBits<u32>(i_this->m21F4.x) =
        (u32)DaioctaBits<u32>(DaioctaGuest<fopAc_ac_c>(ship_p_1)->current.pos.x);
    DaioctaBits<u32>(i_this->m21F4.y) =
        (u32)DaioctaBits<u32>(DaioctaGuest<fopAc_ac_c>(ship_p_1)->current.pos.y);
    DaioctaBits<u32>(i_this->m21F4.z) =
        (u32)DaioctaBits<u32>(DaioctaGuest<fopAc_ac_c>(ship_p_1)->current.pos.z);
    DaioctaBits<u16>(i_this->m2200.x) =
        (u16)DaioctaBits<u16>(DaioctaGuest<fopAc_ac_c>(ship_p_1)->current.angle.x);
    DaioctaBits<u16>(i_this->m2200.y) =
        (u16)DaioctaBits<u16>(DaioctaGuest<fopAc_ac_c>(ship_p_1)->current.angle.y);
    DaioctaBits<u16>(i_this->m2200.z) =
        (u16)DaioctaBits<u16>(DaioctaGuest<fopAc_ac_c>(ship_p_1)->current.angle.z);
    if ((u16)i_this->eventInfo.mCommand != 2) { /* !eventInfo.checkCommandDemoAccrpt() */
        oct_orderOtherEvent(t, gabi::guest_addr("DAIOCTA_SUIKOMI")); /* "DAIOCTA_SUIKOMI" */
        return;
    }
    s32 staff_id = oct_getMyStaffId(gabi::guest_addr("Daiocta")); /* "Daiocta" */
    u32 cut_name = oct_getMyNowCutName(staff_id);
    gabi::Local<cXyz> current_pos;
    gabi::Local<be<s16>> o_angle;
    u32 cp = A(current_pos.get());
    fcpy(cp + 4, t + 0x318);
    fcpy(cp + 8, t + 0x31C);
    DaioctaBits<u32>(current_pos->x) = (u32)DaioctaBits<u32>(i_this->current.pos.x);
    DaioctaHD_dComIfGp_getMapTrans((s32)(s8)i_this->current.roomNo, cp, cp + 8,
                                   o_angle.get()); /* dComIfGp_getMapTrans */
    s16 rot_y = cLib_targetAngleY(&i_this->current.pos, current_pos.get());
    cLib_distanceAngleS((s16)i_this->shape_angle.y, rot_y);
    if (cut_name == 0) { /* HD: null check */
        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x578, gabi::guest_addr("cut_name != (0)"));
    } else {
        if (oct_strcmp(cut_name, gabi::guest_addr("SUIKOMI1")) == 0) { /* "SUIKOMI1" */
            i_this->mPrmIdx = 6;
            oct_seStart(t, 0x50C3 /* JA_SE_CM_DO_SUCK_IN */, A(&i_this->m2434));
            gabi::Local<cXyz> d1, xz1;
            if (oct_absXZ(A(&i_this->m2434), A(&i_this->m21F4), d1, xz1) < 80.0f) {
                oct_cutEnd(staff_id);
            } else {
                u32 morf = (u32)i_this->mpMorf.v;
                if ((f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame == 50.0f)
                    gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame = 40.0f; /* setFrame(40) */
            }
            if (REG12_S(0) != 0)
                return;
            s16 target_angle_y =
                cLib_targetAngleY(&i_this->current.pos, gabi::at<cXyz>(ship_p_1 + 0x314));
            s32 distance_angle_s =
                cLib_distanceAngleS((s16)((s16)i_this->shape_angle.y + l_HIO.m014), target_angle_y);
            cLib_addCalc2(&i_this->m2208, l_HIO.m088, l_HIO.m090, l_HIO.m08C);
            {
                f32 m2208 = i_this->m2208;
                f32 tx = (f32)i_this->m2434.x;
                cLib_chaseF(&i_this->m21F4.x, tx, gabi::fmuls_ppc(m2208, 0.5f));
            }
            {
                f32 tz = (f32)i_this->m2434.z;
                cLib_chaseF(gabi::at<be<f32>>(t + 0x237C), tz, i_this->m2208);
            }
            cLib_addCalcAngleS2(gabi::at<be<s16>>(t + 0x2382), (s16)(target_angle_y + 0x8000), 8,
                                0x400);
            if (distance_angle_s < l_HIO.m012) {
                u16 a = (u16)(target_angle_y + 0x4000);
                gabi::Local<cXyz> d2, xz2;
                f32 dist = oct_absXZ(A(&i_this->current.pos), ship_p_1 + 0x314, d2, xz2);
                f32 k1 = dist / gabi::fadds_ppc(REG12_F(1), 60.0f);
                i_this->m21F4.z = gabi::fmadds(k1, cM_scos(a), (f32)i_this->m21F4.z);
                f32 k2 = dist / gabi::fadds_ppc(REG12_F(1), 60.0f);
                i_this->m21F4.x = gabi::fmadds(k2, cM_ssin(a), (f32)i_this->m21F4.x);
            }
            gabi::Local<be<s16>> nameKey2;
            u32 ship_p_2 = oct_searchShip(nameKey2);
            if (ship_p_2 != 0)
                oct_initStartPos(ship_p_2, A(&i_this->m21F4),
                                 (s16)i_this->m2200.y); /* HD: null check */
        }
        if (oct_strcmp(cut_name, gabi::guest_addr("SUIKOMI2")) == 0) { /* "SUIKOMI2" */
            if (f32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mFrameCtrl.mFrame) == 65.0f) {
                oct_seStart(t, 0x58C4 /* JA_SE_CM_DO_SUCK_IN_FIN */, A(&i_this->eyePos));
            }
            s16 target_angle_y =
                cLib_targetAngleY(&i_this->current.pos, gabi::at<cXyz>(ship_p_1 + 0x314));
            cLib_distanceAngleS((s16)i_this->shape_angle.y, target_angle_y);
            DaioctaHD_cLib_chasePosXZ(A(&i_this->m21F4), A(&i_this->current.pos),
                                      (f32)i_this->m2208); /* cLib_chasePosXZ */
            cLib_addCalcAngleS2(gabi::at<be<s16>>(t + 0x2382), (s16)(target_angle_y + 0x8000), 4,
                                0x800);
            gabi::Local<be<s16>> nameKey3;
            u32 ship_p_2 = oct_searchShip(nameKey3);
            if (ship_p_2 != 0) { /* HD: null check */
                oct_initStartPos(ship_p_2, A(&i_this->m21F4), (s16)i_this->m2200.y);
                gabi::Local<cXyz> d3, xz3;
                if (oct_absXZ(A(&i_this->current.pos), A(&i_this->m21F4), d3, xz3) < 10.0f) {
                    u32 morf = (u32)i_this->mpMorf.v;
                    if (oct_morfIsStop(morf) || i_this->mPrmIdx == 2) {
                        u32 m = (u32)i_this->mpMorf.v;
                        i_this->shape_angle.y = rot_y;
                        DaioctaHD_mDoExt_McaMorf_calc(m); /* mpMorf->calc() */
                        /* current.pos = m220C (integer copy) */
                        u32 x = (u32)DaioctaBits<u32>(i_this->m220C.x),
                            y = (u32)DaioctaBits<u32>(i_this->m220C.y);
                        DaioctaBits<u32>(i_this->current.pos.x) = x;
                        u32 z = (u32)DaioctaBits<u32>(i_this->m220C.z);
                        DaioctaBits<u32>(i_this->current.pos.y) = y;
                        DaioctaBits<u32>(i_this->current.pos.z) = z;
                        oct_initStartPos(ship_p_2, A(&i_this->current.pos), rot_y);
                        oct_cutEnd(staff_id);
                    }
                }
            }
        }
        if (oct_strcmp(cut_name, gabi::guest_addr("HAKIDASU")) == 0) { /* "HAKIDASU" */
            u32 actor_p = A(fopAcM_SearchByID(i_this->mAuzuId));
            if (actor_p != 0) { /* HD: null check */
                fcpy(actor_p + 0x314, A(&i_this->current.pos));
                f32 y = (f32)i_this->current.pos.y;
                fcpy(actor_p + 0x318, t + 0x318);
                fcpy(actor_p + 0x31C, t + 0x31C);
                DaioctaGuest<fopAc_ac_c>(actor_p)->current.pos.y = gabi::fadds_ppc(y, 20.0f);
            }
            u32 morf = (u32)i_this->mpMorf.v;
            if (gabi::ftoi((f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame) == 26) {
                oct_seStart(t, 0x4937 /* JA_SE_CV_DO_SPIT_SHIP */, A(&i_this->m2434));
                morf = (u32)i_this->mpMorf.v;
            }
            if (i_this->mPrmIdx == 8 && oct_morfIsStop(morf)) {
                oct_cutEnd(staff_id);
                DaioctaHD_mDoAud_subBgmStop(); /* mDoAud_subBgmStop */
            } else if ((f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame < 10.0f) {
                oct_initStartPos(ship_p_1, A(&i_this->m2434), rot_y);
                i_this->mDemoEndTimer = 60; /* HD */
            }
            i_this->mPrmIdx = 8;
        }
        if (oct_strcmp(cut_name, gabi::guest_addr("WAIT")) == 0)
            i_this->mPrmIdx = 2; /* "WAIT" */
    }
    /* HD: the fallback timer */
    s32 tm = i_this->mDemoEndTimer;
    if (tm != 0)
        i_this->mDemoEndTimer = tm - 1;
    if (oct_endCheck(gabi::guest_addr("DAIOCTA_SUIKOMI")) ||
        i_this->mDemoEndTimer == 1) { /* "DAIOCTA_SUIKOMI" */
        u8 scls = i_this->m575;
        u32 play = oct_play();
        if (scls == 0xFF) {
            /* daPy_getPlayerLinkActorClass()->setDaiokutaEnd(): startRestartRoom(6, 0xC9, -1.0f, 1)
             */
            DaioctaHD_daPy_lk_c_startRestartRoom(
                u32(DaioctaGuest<dComIfG_inf_c>(play)->play.mpPlayerPtr[0].v), 6, 0xC9, -1.0f, 1);
        } else {
            /* dComIfGp_setNextStage(dComIfGp_getStartStageName(), m575, roomNo, -1, 0.0f, 6) (HD:
             * two more arguments) */
            DaioctaHD_dComIfGp_setNextStage(play + 0x5134, (u32)i_this->m575,
                                            (s32)(s8)i_this->current.roomNo, -1, 0.0f, 6, 1, 0);
        }
    }
}

/* 0211BF44 */
void daDaiocta_c::modeDeleteInit() {
    auto* i_this = this;
    u32 t = A(i_this);
    i_this->attention_info.flags = 0; /* attention_info.flags */
    oct_seStart(t, 0x48C9 /* JA_SE_CV_DO_DIE */, A(&i_this->eyePos));
    oct_seStart(t, 0x58C5 /* JA_SE_CM_DO_JITABATA */, A(&i_this->eyePos));
    oct_seStart(t, 0x2828 /* JA_SE_LK_LAST_HIT */, A(&i_this->eyePos));
    i_this->setEffect(0x8203); /* ID_IT_SN_DO_HITC_SHIBUKIA00 */
    i_this->setEffect(0x8204); /* ID_IT_SN_DO_HITC_SHIBUKIB00 */
}

/* 0211BFFC */
void daDaiocta_c::modeDelete() {
    auto* i_this = this;
    u32 t = A(i_this);
    if ((u16)i_this->eventInfo.mCommand == 2) { /* eventInfo.checkCommandDemoAccrpt() */
        s32 staff_id = oct_getMyStaffId(gabi::guest_addr("Daiocta")); /* "Daiocta" */
        u32 cut_name = oct_getMyNowCutName(staff_id);
        if (cut_name == 0) { /* HD: null check */
            oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x62F,
                       gabi::guest_addr("cut_name != (0)"));
        } else {
            if (oct_strcmp(cut_name, gabi::guest_addr("DEATH1")) == 0) { /* "DEATH1" */
                u32 morf = (u32)i_this->mpMorf.v;
                i_this->mPrmIdx = 5;
                fcpy(t + 0x318, A(&i_this->mWaterY));
                if (oct_morfIsStop(morf)) {
                    /* fopAcM_monsSeStart(this, JA_SE_CV_DO_SINK, 0) */
                    s32 reverb = DaioctaHD_dComIfGp_getReverb((s32)(s8)i_this->current.roomNo);
                    DaioctaHD_mDoAud_monsSeStart(0x48CA, A(&i_this->eyePos), u32(i_this->mBsPcId),
                                                 0u, reverb);
                    i_this->setEffect(0x8205); /* ID_IT_SN_DO_DOWNSHIBUKI00 */
                    u32 auzu_p = A(fopAcM_SearchByID(i_this->mAuzuId));
                    if (auzu_p == 0)
                        oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x642,
                                   gabi::guest_addr("uzu != (0)")); /* HD */
                    else
                        DaioctaGuest<daObjAuzu::Act_c>(auzu_p)->mbToAppear = 0; /* to_disappear() */
                    gabi::Local<cXyz> q;
                    u32 vib = A(&DaioctaGuest<dComIfG_inf_c>(oct_play())->play.mVibration);
                    q->z = 0.0f;
                    q->x = 0.0f;
                    q->y = 1.0f;
                    DaioctaHD_dVibration_c_StartQuake(vib, 7, -0x21, q.get()); /* StartQuake */
                    oct_cutEnd(staff_id);
                }
            }
            if (oct_strcmp(cut_name, gabi::guest_addr("DEATH2")) == 0) { /* "DEATH2" */
                gabi::Local<cXyz> target_pos;
                u32 z = (u32)DaioctaBits<u32>(i_this->current.pos.z);
                u32 x = (u32)DaioctaBits<u32>(i_this->current.pos.x);
                f32 w = i_this->mWaterY;
                DaioctaBits<u32>(target_pos->x) = x;
                f32 y = gabi::fsubs_ppc(w, 3000.0f);
                DaioctaBits<u32>(target_pos->z) = z;
                i_this->mPrmIdx = 7;
                f32 step = l_HIO.m0E0;
                target_pos->y = y;
                DaioctaHD_cLib_chasePos(A(&i_this->current.pos), target_pos.get(),
                                        step); /* cLib_chasePos */
                if (std::fabs(gabi::fsubs_ppc((f32)i_this->current.pos.y, i_this->mWaterY)) >
                        600.0f &&
                    i_this->m26C1 == 0) {
                    i_this->initAwa();
                    oct_seStart(t, 0x58C6 /* JA_SE_CM_DO_SINK_IN_SEA */, A(&i_this->eyePos));
                    i_this->m26C1 = 1;
                    i_this->m31C4 = 1;
                    DaioctaHD_mDoAud_subBgmStop(); /* mDoAud_subBgmStop */
                }
                gabi::Local<cXyz> d;
                f32 dist = oct_abs(A(&i_this->current.pos), A(target_pos.get()), d);
                if (dist < gabi::fmuls_ppc(l_HIO.m0E0, 3.0f)) {
                    if (i_this->mSwitchNo != 0xFF) {
                        i_this->m31C4 = 0;
                        DaioctaHD_dVibration_c_StopQuake(oct_play() + 0x599C,
                                                         -1); /* StopQuake(-1) */
                        /* HD: the riddle jingle only when the switch was not on yet */
                        if (!dComIfGs_isSwitch(i_this->mSwitchNo,
                                               (s32)(s8)i_this->current.roomNo) &&
                            i_this->m057C != 1) {
                            DaioctaHD_mDoAud_seStart(
                                0x806); /* mDoAud_seStart(JA_SE_READ_RIDDLE_1) */
                        }
                        dComIfGs_onSwitch(i_this->mSwitchNo, (s32)(s8)i_this->current.roomNo);
                        i_this->actor_status =
                            (u32)i_this->actor_status & ~0x3Fu; /* fopAcM_ClearStatusMap */
                    }
                    oct_cutEnd(staff_id);
                }
            }
        }
        if (cut_name == 0 && i_this->m057C == 1)
            return;
        if (i_this->m057C != 1 &&
            oct_endCheck(gabi::guest_addr("DAIOCTA_DEAD"))) { /* "DAIOCTA_DEAD" */
            dComIfGp_event_reset();
            fopAcM_delete(i_this);
        }
    } else if (i_this->m057C == 1) {
        oct_orderOtherEvent(t, gabi::guest_addr("DAIOCTA_DEAD_ELF")); /* "DAIOCTA_DEAD_ELF" */
    } else {
        oct_orderOtherEvent(t, gabi::guest_addr("DAIOCTA_DEAD")); /* "DAIOCTA_DEAD" */
    }
}

/* 0211889C */
void daDaiocta_c::modeProc(s32 proc, s32 mode) {
    auto* i_this = this;
    if (proc == daDaiocta_c::PROC_INIT_e) {
        if (mode >= 7)
            return; /* HD: bounds check */
        u32 e = gabi::ea(&mode_tbl) + u32(mode) * 20u;
        i_this->mMode = mode;
        auto* entry = reinterpret_cast<DaioctaModeEntry*>(PPC_MEM_BASE + e);
        (this->*(entry->mInitFunc))();
    } else if (proc == daDaiocta_c::PROC_EXEC_e) {
        s32 m = i_this->mMode;
        if (m >= 7)
            return; /* HD: bounds check */
        auto* entry = reinterpret_cast<DaioctaModeEntry*>(PPC_MEM_BASE +
                                                          (gabi::ea(&mode_tbl) + u32(m) * 20u));
        (this->*(entry->mUpdFunc))();
    }
}

/* 0211AB80 */
void daDaiocta_c::setAnm() {
    auto* i_this = this;
    u32 t = A(i_this);
    /* dLib_anm_prm_c anm_prms[9] (0x10 each: morf at +4, speed at +8), copied from .data */
    gabi::Local<u8[0x90]> prms;
    u32 p = A(prms.get());
    for (u32 k = 0; k < 0x90; k += 4)
        store<u32>(p + k, u32(l_anmParameters.mWords[k / 4]));
    for (u32 i = 1; i < 9; i++) {
        fcpy(p + i * 0x10 + 4, A(DaioctaArray(l_HIO.m0BC, i))); /* m0BC[i] */
        fcpy(p + i * 0x10 + 8, A(DaioctaArray(l_HIO.m098, i))); /* m098[i] */
    }
    u32 morf = (u32)i_this->mpMorf.v;
    u32 morf_model_p = (u32)gabi::at<mDoExt_McaMorf>(morf)->mpModel.v;
    s8 anm = i_this->mAnmIdx;
    bool check6 = true;
    if (anm == 3) {
        if ((f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame == 10.0f ||
            (f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame == 25.0f ||
            (f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame == 45.0f) {
            gabi::Local<cXyz> shockPos;
            oct_startShock(6, shockPos);
            anm = i_this->mAnmIdx;
        } else {
            check6 = false;
        }
    }
    if (check6 && anm == 6) {
        morf = (u32)i_this->mpMorf.v;
        if ((f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame == 1.0f) {
            i_this->setEffect(0x82CC); /* ID_IT_SN_DO_HAKIDASHI00 */
            morf = (u32)i_this->mpMorf.v;
        }
        if ((f32)gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame == 26.0f) {
            gabi::Local<cXyz> shockPos;
            oct_startShock(7, shockPos);
        }
    }
    s8 prm = i_this->mPrmIdx;
    s8 old = i_this->mOldPrmIdx;
    s32 idx = s32(*DaioctaArray(a_brk_anm_prm_tbl.mValues, prm)); /* a_brk_anm_prm_tbl[mPrmIdx] */
    if (old != prm && idx != -1) {
        u32 brk =
            oct_getRes(s32(*DaioctaArray(a_brk_anm_idx_tbl.mValues, idx))); /* a_brk_anm_idx_tbl */
        if (brk == 0)
            oct_assert(gabi::guest_addr("d_a_daiocta.cpp"), 0x727, gabi::guest_addr("brk != (0)"));
        /* mBrkAnm1.init(modelData, brk, TRUE, EMode_LOOP, 1.0f, 0, -1, true, FALSE) */
        DaioctaHD_mDoExt_brkAnm_initModelData(
            A(&i_this->mBrkAnm1), (u32)DaioctaGuest<J3DModel>(morf_model_p)->mModelData.v, brk, 1,
            2, 1.0f, 0, -1, 1, 0);
    }
    /* dLib_setAnm(arc, morf, &mAnmIdx, &mPrmIdx, &mOldPrmIdx, a_anm_idx_tbl, anm_prms, false) (HD:
     * one more argument on the stack) */
    DaioctaHD_dLib_setAnm(A(m_arc_name), (u32)i_this->mpMorf.v, A(&i_this->mAnmIdx),
                          A(&i_this->mPrmIdx), A(&i_this->mOldPrmIdx), A(&a_anm_idx_tbl), p, 0u,
                          0u);
    fcpy(t + 0x6B0, (u32)i_this->mpMorf.v + 0x9C); /* mBrkAnm1.setFrame(mpMorf->getFrame()) */
}

/* 0211A104 */
void daDaiocta_c::setWater() {
    auto* i_this = this;
    f32 h = f32(i_this->mAcch.m_sea_height); /* mAcch.GetSeaHeight() */
    if (h == -1000000000.0f)
        h = 0.0f; /* -G_CM3D_F_INF */
    i_this->mWaterY = h;
}

/* 0211AE68 */
bool daDaiocta_c::_execute() {
    auto* i_this = this;
    u32 t = A(i_this);
    s32 mode = i_this->mMode;
    if (mode == daDaiocta_c::MODE_DELETE) {
        i_this->execAwa();
        mode = i_this->mMode;
    }
    if (mode != daDaiocta_c::MODE_HIDE) {
        if (mode != daDaiocta_c::MODE_APPEAR) {
            f32 dist = fopAcM_searchActorDistanceXZ(i_this, dComIfGp_getPlayer(0));
            s16 angle_y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            s32 dist_angle_s =
                cLib_distanceAngleS((s16)((s16)i_this->shape_angle.y + l_HIO.m014), angle_y);
            if (dist < l_HIO.m084) {
                s32 m = i_this->mMode;
                if (m != daDaiocta_c::MODE_DEMO && m != daDaiocta_c::MODE_DELETE &&
                    dist_angle_s < l_HIO.m012) {
                    i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_DEMO);
                }
            }
            mode = i_this->mMode;
        }
        if (mode != daDaiocta_c::MODE_HIDE) {
            i_this->setWater();
            mDoExt_baseAnm_play(&i_this->mBrkAnm2);
            mDoExt_baseAnm_play(&i_this->mBtkAnm);
            /* attention_info.position = eyePos = current.pos (integer copies) */
            u32 x = (u32)DaioctaBits<u32>(i_this->current.pos.x),
                y = (u32)DaioctaBits<u32>(i_this->current.pos.y);
            DaioctaBits<u32>(i_this->attention_info.position.x) = x;
            u32 z = (u32)DaioctaBits<u32>(i_this->current.pos.z);
            DaioctaBits<u32>(i_this->attention_info.position.y) = y;
            DaioctaBits<u32>(i_this->attention_info.position.z) = z;
            DaioctaBits<u32>(i_this->eyePos.x) = x;
            DaioctaBits<u32>(i_this->eyePos.z) = z;
            DaioctaBits<u32>(i_this->eyePos.y) = y;
            i_this->isLivingEye();
            mDoExt_baseAnm_play(&i_this->mBrkAnm1);
            DaioctaHD_mDoExt_McaMorf_play((u32)i_this->mpMorf.v, 0u, 0u,
                                          0u);                    /* mpMorf->play(NULL, 0, 0) */
            DaioctaHD_mDoExt_McaMorf_calc((u32)i_this->mpMorf.v); /* mpMorf->calc() */
            i_this->setRotEye();
            dBgS_Acch_CrrPos(&i_this->mAcch, dComIfG_Bgsp());
            DaioctaBits<u8>(i_this->tevStr.mRoomNo) =
                (u8)DaioctaBits<u8>(i_this->current.roomNo); /* tevStr.mRoomNo */
            /* tevStr.mEnvrIdxOverride = dComIfG_Bgsp()->GetPolyColor(mAcch.m_gnd) */
            u32 bgs = A(dComIfG_Bgsp());
            i_this->tevStr.mEnvrIdxOverride = (u8)DaioctaHD_dBgS_GetPolyColor(bgs, t + 0x2480);
        }
    }
    i_this->modeProc(daDaiocta_c::PROC_EXEC_e, daDaiocta_c::MODE_NULL);
    if (i_this->mMode != daDaiocta_c::MODE_HIDE) {
        i_this->setCollision();
        i_this->setAnm();
        i_this->setMtx();
        if (l_HIO.m007 != 0) {
            i_this->mPrmIdx = (s8)l_HIO.m00A;
            i_this->setAnm();
        }
    }
    return false;
}

/* 0211B318 */
void daDaiocta_c::drawAwa() {
    auto* i_this = this;
    u32 t = A(i_this);
    dComIfGd_setListBG();
    oct_settingTevStruct(t, 2 /* TEV_TYPE_BG1 */);
    for (int i = 0; i < l_HIO.m016; i++) {
        u32 pm = A(DaioctaArray(i_this->mpAwaModels, i));
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, gabi::at<J3DModel>(u32(DaioctaGuest<gptr<J3DModel>>(pm)->v)),
                             &i_this->tevStr);
        u32 bck = A(DaioctaArray(i_this->mAwaBckAnms, i));
        DaioctaHD_mDoExt_bckAnm_entry(
            bck, u32(DaioctaGuest<J3DModel>(DaioctaGuest<gptr<J3DModel>>(pm)->v)->mModelData.v),
            (f32)gabi::at<mDoExt_bckAnm>(bck)->mFrameCtrl.mFrame);
        u32 btk = A(DaioctaArray(i_this->mAwaBtkAnms, i));
        DaioctaHD_mDoExt_btkAnm_entry(
            btk, u32(DaioctaGuest<J3DModel>(DaioctaGuest<gptr<J3DModel>>(pm)->v)->mModelData.v),
            (f32)gabi::at<mDoExt_btkAnm>(btk)->mFrameCtrl.mFrame);
        u32 brk = A(DaioctaArray(i_this->mAwaBrkAnms, i));
        DaioctaHD_mDoExt_brkAnm_entry(
            brk, u32(DaioctaGuest<J3DModel>(DaioctaGuest<gptr<J3DModel>>(pm)->v)->mModelData.v),
            (f32)gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame);
        mDoExt_modelUpdateDL(gabi::at<J3DModel>(u32(DaioctaGuest<gptr<J3DModel>>(pm)->v)), 0);
    }
    dComIfGd_setList();
}

/* 0211B240 */
void daDaiocta_c::drawSuikomi() {
    auto* i_this = this;
    u32 t = A(i_this);
    dComIfGd_setListBG();
    oct_settingTevStruct(t, 2 /* TEV_TYPE_BG1 */);
    {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, gabi::at<J3DModel>((u32)i_this->mpSuikomiModel.v),
                             &i_this->tevStr);
    }
    u32 model = (u32)i_this->mpSuikomiModel.v;
    DaioctaHD_mDoExt_btkAnm_entry(A(&i_this->mBtkAnm),
                                  (u32)DaioctaGuest<J3DModel>(model)->mModelData.v,
                                  f32(i_this->mBtkAnm.mFrameCtrl.mFrame)); /* mBtkAnm.entry */
    model = (u32)i_this->mpSuikomiModel.v;
    DaioctaHD_mDoExt_brkAnm_entry(A(&i_this->mBrkAnm2),
                                  (u32)DaioctaGuest<J3DModel>(model)->mModelData.v,
                                  f32(i_this->mBrkAnm2.mFrameCtrl.mFrame)); /* mBrkAnm2.entry */
    mDoExt_modelUpdateDL(gabi::at<J3DModel>((u32)i_this->mpSuikomiModel.v), 0);
    DaioctaGuest<octoEyeHD::ModelAnimationEntries>(
        DaioctaGuest<J3DModel>(i_this->mpSuikomiModel.v)->mModelData.v)
        ->mBtkEntry = 0; /* mBtkAnm.remove */
    DaioctaGuest<octoEyeHD::ModelAnimationEntries>(
        DaioctaGuest<J3DModel>(i_this->mpSuikomiModel.v)->mModelData.v)
        ->mBrkEntry = 0; /* mBrkAnm2.remove */
    dComIfGd_setList();
}

/* 0211B05C */
void daDaiocta_c::drawDebug() {
    // HD: debug primitives were removed; preserve the first-use color initialization.
    auto* i_this = this;
    gabi::Local<cXyz> d;
    for (int i = 0; i < 37; i++) {
        fopAc_ac_c* player_p = dComIfGp_getPlayer(0);
        cXyz_mi(gabi::at<cXyz>(A(player_p) + 0x314), d.get(),
                gabi::at<cXyz>(A(&i_this->mSphCenters) + i * 12));
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

/* 0211B420 */
bool daDaiocta_c::_draw() {
    auto* i_this = this;
    u32 t = A(i_this);
    if (l_HIO.m004 != 0)
        i_this->drawDebug();
    s32 mode = i_this->mMode;
    if (mode == daDaiocta_c::MODE_HIDE)
        return true;
    if (mode == daDaiocta_c::MODE_DEMO) {
        i_this->drawSuikomi();
        mode = i_this->mMode;
    }
    if (mode == daDaiocta_c::MODE_DELETE)
        i_this->drawAwa();
    u32 morf_model_p = u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v);
    oct_settingTevStruct(t, 0 /* TEV_TYPE_ACTOR */);
    {
        u32 morf = (u32)i_this->mpMorf.v;
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env,
                             gabi::at<J3DModel>((u32)gabi::at<mDoExt_McaMorf>(morf)->mpModel.v),
                             &i_this->tevStr);
    }
    DaioctaHD_mDoExt_brkAnm_entry(A(&i_this->mBrkAnm1),
                                  (u32)DaioctaGuest<J3DModel>(morf_model_p)->mModelData.v,
                                  f32(i_this->mBrkAnm1.mFrameCtrl.mFrame)); /* mBrkAnm1.entry */
    if (l_HIO.m005 == 0)
        DaioctaHD_mDoExt_McaMorf_entryDL((u32)i_this->mpMorf.v); /* mpMorf->entryDL() */
    DaioctaGuest<octoEyeHD::ModelAnimationEntries>(
        DaioctaGuest<J3DModel>(morf_model_p)->mModelData.v)
        ->mBrkEntry = 0; /* mBrkAnm1.remove */
    dSnap_RegistFig(0xC6 /* DSNAP_TYPE_DAIOCTA */, i_this, 1.0f, 1.0f, 1.0f);
    return true;
}

/* 021192CC */
void daDaiocta_c::getArg() {
    auto* i_this = this;
    s16 prm2 = (s16)i_this->home.angle.z; /* home.angle.z */
    u32 prm = (u32)i_this->mParameters;
    i_this->home.angle.z = 0;
    i_this->mOctoType = (u8)prm;         /* mOctoType */
    i_this->m575 = (u8)(prm >> 16);      /* m575 */
    i_this->current.angle.z = 0;         /* current.angle.z */
    i_this->shape_angle.z = 0;           /* shape_angle.z */
    i_this->mSwitchNo = (u8)(prm >> 24); /* mSwitchNo */
    i_this->m057C = (u8)prm2;            /* m057C */
    u32 mult = (prm >> 8) & 0xFF;
    if (mult == 0xFF) {
        fcpy(A(&i_this->mAppearRadius), A(&l_HIO.mDefaultAppearRadius)); /* mDefaultAppearRadius */
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
        for (int i = 0; i < 8; i++)
            i_this->mEyeAlloc[i] = 1;
        i_this->mDemoEndTimer = 0;
        break;
    case 2: /* TWELVE_EYED */
        for (int i = 0; i < 12; i++)
            i_this->mEyeAlloc[i] = 1;
        i_this->mDemoEndTimer = 0;
        break;
    default:
        i_this->mDemoEndTimer = 0;
        break;
    }
}

/* 02119A0C */
void daDaiocta_c::createInit() {
    auto* i_this = this;
    u32 t = A(i_this);
    /* m220C = current.pos (integer copy) */
    u32 px = (u32)DaioctaBits<u32>(i_this->current.pos.x),
        py = (u32)DaioctaBits<u32>(i_this->current.pos.y),
        pz = (u32)DaioctaBits<u32>(i_this->current.pos.z);
    DaioctaBits<u32>(i_this->m220C.y) = py;
    DaioctaBits<u32>(i_this->m220C.z) = pz;
    DaioctaBits<u32>(i_this->m220C.x) = px;
    i_this->initAwa();
    i_this->modeProc(daDaiocta_c::PROC_INIT_e, daDaiocta_c::MODE_HIDE);
    i_this->initMtx();
    /* shape_angle = current.angle; attention_info.flags = 0 */
    u16 ay = (u16)DaioctaBits<u16>(i_this->current.angle.y),
        ax = (u16)DaioctaBits<u16>(i_this->current.angle.x),
        az = (u16)DaioctaBits<u16>(i_this->current.angle.z);
    i_this->attention_info.flags = 0;
    DaioctaBits<u16>(i_this->shape_angle.z) = az;
    DaioctaBits<u16>(i_this->shape_angle.x) = ax;
    DaioctaBits<u16>(i_this->shape_angle.y) = ay;
    constexpr u32 eye_jnt[12] = {0x15, 0x16, 0x0F, 0x17, 0x18, 0x10,
                                 0x11, 0x12, 0x0E, 0x0B, 0x0C, 0x0D};
    for (int i = 0; i < 12; i++)
        i_this->mAnmMtxIndices[i] = eye_jnt[i];
    for (u32 i = 0; i < 37; i++) {
        /* getJointNodePointer(i)->setCallBack(nodeControl_CB) (HD: sead buffer access, out of
         * range -> element 0) */
        u32 data =
            DaioctaGuest<J3DModel>(u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v))
                ->mModelData.v;
        u32 n = DaioctaGuest<J3DModelData>(data)->mJointNum;
        u32 p = DaioctaGuest<J3DModelData>(data)->mJointNodes;
        if ((i & 0xFFFF) < n)
            p += (i & 0xFFFF) * 0x1C;
        DaioctaGuest<J3DJoint>(p)->mCallBack = 0x02118CE0u;
    }
    DaioctaHD_dBgS_AcchCir_SetWall(A(&i_this->mAcchCir), 30.0f,
                                   30.0f); /* mAcchCir.SetWall(30, 30) */
    /* mAcch.Set(&pos, &old.pos, this, 1, &mAcchCir, &speed) (HD: extra argument on the stack) */
    DaioctaHD_dBgS_Acch_Set(A(&i_this->mAcch), A(&i_this->current.pos), A(&i_this->old.pos), t, 1,
                            A(&i_this->mAcchCir), A(&i_this->speed), 0u, 0u);
    /* SetWallNone, SetRoofNone, OnSeaCheckOn, OnSeaWaterHeight */
    i_this->mAcch.m_flags = u32(i_this->mAcch.m_flags) | 0x6000C;
    u32 model = u32(DaioctaGuest<mDoExt_McaMorf>(i_this->mpMorf.v)->mpModel.v);
    i_this->cullMtx = model != 0 ? A(&DaioctaGuest<J3DModel>(model)->mBaseTransformMtx)
                                 : 0; /* fopAcM_SetMtx(getBaseTRMtx()) */
    fopAcM_setCullSizeBox(i_this, -2300.0f, -500.0f, -2300.0f, 2300.0f, 2300.0f, 2300.0f);
    i_this->cullSizeFar = 10.0f; /* fopAcM_setCullSizeFar */
    for (int i = 0; i < 12; i++) {
        if ((u8)((*DaioctaArray(i_this->mEyeAlloc, i))) != 0) {
            /* fopAcM_createChild(DAIOCTA_EYE, fpcM_GetID(this), -1, &pos, roomNo, NULL, NULL, -1,
             * NULL) */
            u32 id = DaioctaHD_fopAcM_createChild(0xE2, u32(i_this->mBsPcId), -1,
                                                  A(&i_this->current.pos),
                                                  (s32)(s8)i_this->tevStr.mRoomNo, 0u, 0u, -1, 0u);
            *DaioctaArray(i_this->mDaioctaEyePcId, i) = id;
        }
    }
    i_this->mAuzuId = fopAcM_create(0x11B, 0x1100FF, &i_this->current.pos,
                                    (s32)(s8)i_this->tevStr.mRoomNo, nullptr, nullptr, -1, 0);
    DaioctaHD_dCcD_Stts_Init(A(&i_this->mStts), 0, 0, t); /* mStts.Init(0, 0, this) */
    for (int i = 0; i < 6; i++) {
        u32 sph = A(DaioctaArray(i_this->mSph, i));
        DaioctaHD_dCcD_Sph_Set(sph, 0x1000D424u);             /* dCcD_Sph::Set(m_sph_src) */
        gabi::at<dCcD_Sph>(sph)->mStts.v = A(&i_this->mStts); /* SetStts */
        DaioctaGuest<dCcD_Sph>(sph)->mGObjCo.mHitCallback =
            0x021189CCu; /* SetCoHitCallback(coHit_CB) */
    }
    for (int i = 0; i < 17; i++) {
        u32 cps = A(DaioctaArray(i_this->mCps, i));
        DaioctaHD_dCcD_Cps_Set(cps, 0x1000D464u); /* dCcD_Cps::Set(m_cps_src) */
        gabi::at<dCcD_Cps>(cps)->mStts.v = A(&i_this->mStts);
        DaioctaGuest<dCcD_Cps>(cps)->mGObjCo.mHitCallback = 0x021189CCu;
    }
    i_this->mPrmIdx = 1;
}

/* 02119CE8 */
cPhs_State daDaiocta_c::_create() {
    auto* i_this = this;
    u32 t = A(i_this);
    /* fopAcM_ct(this, daDaiocta_c) */
    u32 cond = (u32)i_this->actor_condition;
    if (!(cond & 8)) {
        if (t != 0) {
            DaioctaHD_fopAc_ac_c_fopAc_ac_c(t); /* fopAc_ac_c::fopAc_ac_c */
            i_this->__vtbl = DAIOCTA_VTBL;
            DaioctaHD_mDoExt_brkAnm_construct(
                A(&i_this->mBrkAnm1)); /* mDoExt_brkAnm::mDoExt_brkAnm */
            __construct_array(A(&i_this->mSph), 6, 0x12C, 0x025166F0); /* dCcD_Sph */
            __construct_array(A(&i_this->mCps), 17, 0x138,
                              0x0211D08C); /* dCcD_Cps (this TU's copy) */
            dCcD_Stts_ct(&i_this->mStts);
            dBgS_ObjAcch_ct(&i_this->mAcch, dBgS_ObjAcch_vt{0x1000D004, 0x1000D024, 0x1000D014});
            DaioctaHD_dBgS_AcchCir_dBgS_AcchCir(
                A(&i_this->mAcchCir)); /* dBgS_AcchCir::dBgS_AcchCir */
            DaioctaHD_dPa_followEcallBack_dPa_followEcallBack(A(&i_this->mParticleCallback), 0u,
                                                              0u);   /* dPa_followEcallBack */
            DaioctaHD_mDoExt_brkAnm_construct(A(&i_this->mBrkAnm2)); /* mDoExt_brkAnm */
            DaioctaHD_mDoExt_btkAnm_construct(A(&i_this->mBtkAnm));  /* mDoExt_btkAnm */
            __construct_array(A(&i_this->mAwaBckAnms), 30, 0x8C,
                              0x0211D118); /* mDoExt_bckAnm (this TU's copy) */
            __construct_array(A(&i_this->mAwaBtkAnms), 30, 0x74, 0x025E7C6C); /* mDoExt_btkAnm */
            __construct_array(A(&i_this->mAwaBrkAnms), 30, 0x78, 0x025E80D0); /* mDoExt_brkAnm */
            cond = (u32)i_this->actor_condition;
        }
        i_this->actor_condition = cond | 8;
    }
    cPhs_State state = dComIfG_resLoad(&i_this->mPhs, gabi::at<const char>(A(m_arc_name)));
    if (state == cPhs_COMPLEATE_e) {
        i_this->getArg();
        u8 sw = i_this->mSwitchNo;
        if (sw != 0xFF && dComIfGs_isSwitch(sw, (s32)(s8)i_this->current.roomNo))
            return cPhs_ERROR_e;
        if (!fopAcM_entrySolidHeap(i_this, 0x021192C8, 0xF740))
            return cPhs_ERROR_e;
        i_this->createInit();
    }
    return state;
}

/* 02119EEC */
bool daDaiocta_c::_delete() {
    auto* i_this = this;
    dComIfG_resDelete(&i_this->mPhs, gabi::at<const char>(A(m_arc_name)));
    i_this->mParticleCallback.remove();
    gabi::Local<be<u32>> actor_p;
    for (int i = 0; i < 12; i++) {
        if (oct_searchByID((u32)((*DaioctaArray(i_this->mDaioctaEyePcId, i))), actor_p.get()))
            fopAcM_delete(gabi::at<fopAc_ac_c>(*actor_p));
    }
    mDoAud_seDeleteObject(&i_this->m2434);
    return true;
}

/* 02119EE8 */
static cPhs_State daDaioctaCreate(void* i_this) { return ((daDaiocta_c*)i_this)->_create(); }

/* 02119F80 */
static BOOL daDaioctaDelete(void* i_this) { return ((daDaiocta_c*)i_this)->_delete(); }

/* 0211B058 */
static BOOL daDaioctaExecute(void* i_this) { return ((daDaiocta_c*)i_this)->_execute(); }

/* 0211B52C */
static BOOL daDaioctaDraw(void* i_this) { return ((daDaiocta_c*)i_this)->_draw(); }

/* 0211D1A8 */
static BOOL daDaioctaIsDelete(void*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 0211CFD8 */
static void __sinit_d_a_daiocta_cpp() {
    sinit_header_statics(0x10463928, 0x101B40C8);
    daDaiocta_HIO_ct(A(&l_HIO)); /* l_HIO */
}

/* 0211D078 */
static void SafeString_dtor(SafeString* self, u32 flags) {
    if (self != nullptr && (flags & 1))
        operator_delete(self);
}

/* 0211D08C */
static u32 dCcD_Cps_ct(u32 self) {
    if (self == 0) {
        self = A(operator_new(0x138));
        if (self == 0)
            return 0;
    }
    DaioctaHD_dCcD_GObjInf_dCcD_GObjInf(self); /* dCcD_GObjInf::dCcD_GObjInf */
    DaioctaGuest<DaioctaCapsule>(self)->mShape.mVtable = 0x100015A8;      /* cCcD_ShapeAttr */
    DaioctaGuest<DaioctaCapsule>(self)->mShape.mAab.mVtable = 0x1000CFCC; /* cM3dGAab (this TU) */
    DaioctaHD_cM3dGCps_construct(
        A(&DaioctaGuest<DaioctaCapsule>(self)->mCps));              /* cM3dGCps::cM3dGCps */
    DaioctaGuest<DaioctaCapsule>(self)->__vtbl_hitinf = 0x1004AF18; /* dCcD_Cps */
    DaioctaGuest<DaioctaCapsule>(self)->mCps.mVtable = 0x1004AF60;
    DaioctaGuest<DaioctaCapsule>(self)->mShape.mVtable = 0x1004AF70;
    return self;
}

/* 0211D118 */
static u32 mDoExt_bckAnm_ct(u32 self) {
    if (self == 0) {
        self = A(operator_new(0x8C));
        if (self == 0)
            return 0;
    }
    DaioctaHD_J3DFrameCtrl_init(self, 0); /* J3DFrameCtrl::init(0) */
    store<u32>(self + 0x10, 0x1016E54C);
    DaioctaHD_J3DAnmTransform_construct(self + 0x14);
    store<u32>(self + 0x88, 0);
    store<u32>(self + 0x48, 0x1016D820);
    store<u32>(self + 0x58, 0);
    store<u32>(self + 0x84, 0);
    store<u32>(self + 0x10, 0x1000CFDC);
    store<u32>(self + 0x80, 0);
    store<u32>(self + 0x7C, 0);
    return self;
}

/* 0211D1B0 */
static void mDoExt_bckAnm_dtor(u32 self, u32 flags) {
    if (self == 0)
        return;
    DaioctaHD_J3DAnmTransform_delete(self + 0x10, 0); /* the anm member's destructor */
    if (flags & 1)
        operator_delete(gabi::at<void>(self));
}

/* 0211D204 */
static void daDaiocta_dtor(u32 self, u32 flags) {
    if (self == 0)
        return;
    __destroy_arr(self + 0x29B4, 30, 0x8C, 0x0211D1B0); /* mAwaBckAnms */
    DaioctaHD_cM3dGCir_delete(self + 0x2570, 2);        /* mAcchCir: cM3dGCir::~cM3dGCir */
    store<u32>(self + 0x23B8, 0x1000D014);              /* dBgS_ObjAcch vtables (this TU) */
    store<u32>(self + 0x23AC, 0x1000D024);
    DaioctaHD_dBgS_Acch_delete(self + 0x2398, 0);       /* dBgS_Acch::~dBgS_Acch */
    DaioctaHD_dCcD_Stts_delete(self + 0x22E4, 2);       /* dCcD_Stts::~dCcD_Stts */
    __destroy_arr(self + 0xE2C, 17, 0x138, 0x02515980); /* mCps */
    __destroy_arr(self + 0x724, 6, 0x12C, 0x02515AE8);  /* mSph */
    DaioctaHD_fopAc_ac_c_dtor(self, 0);                 /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(gabi::at<void>(self));
}

/* 0211D2F4 */
static void SafeString_assureTermination(void*) {}

/* 0211D2F8 */
static s32 cLib_calcTimer_i(be<s32>* timer) {
    s32 v = *timer;
    if (v != 0) {
        v--;
        *timer = v;
    }
    return v;
}

// clang-format off
/* WWHD glue
data l_HIO 10463944

ext 025200D4 void * DaioctaHD_dComIfGp_get()
ext 0273AA24 void DaioctaHD_JUTAssertion_showAssert(u32, s32, u32)
ext 025D54C4 int DaioctaHD_fopAcM_SearchByID(u32, be<u32> *)
ext 02520540 int DaioctaHD_dComIfGp_getReverb(s32)
ext 025E1A40 void DaioctaHD_mDoAud_seStartAt(u32, u32, u32, s32)
ext 025CB374 void DaioctaHD_dVibration_c_StartShock(u32, s32, int, cXyz *)
ext 020CB92C u32 DaioctaHD_daBomb_c_chk_state(u32, int)
ext 02587688 u32 DaioctaHD_dLib_brkInit(u32, u32, u32, int)
ext 025E4F64 u32 DaioctaHD_mDoExt_McaMorf_mDoExt_McaMorf(u32, u32, u32, u32, u32, int, f32, int, int, int, u32, u32, u32)
ext 02587740 u32 DaioctaHD_dLib_btkInit(u32, u32, u32, int)
ext 025E8508 u32 DaioctaHD_mDoExt_bckAnm_init(u32, u32, u32, int, int, f32, int, int, int)
ext 025E7CE0 u32 DaioctaHD_mDoExt_btkAnm_initHD(u32, u32, u32, int, int, f32, int, int, int, int)
ext 025E8154 u32 DaioctaHD_mDoExt_brkAnm_initHD(u32, u32, u32, int, int, f32, int, int, int, int)
ext 02552B60 u32 DaioctaHD_JntHit_create(u32, u32, int)
ext 025F20B0 void DaioctaHD_mDoMtx_inverseTranspose(u32, Mtx34 *)
ext 025F232C void DaioctaHD_mDoMtx_MtxToRot(Mtx34 *, u32)
ext 024EFF44 void DaioctaHD_dBgS_AcchCir_SetWall(u32, f32, f32)
ext 024F06B4 void DaioctaHD_dBgS_Acch_Set(u32, u32, u32, u32, int, u32, u32, u32, u32)
ext 025D5A20 u32 DaioctaHD_fopAcM_createChild(int, u32, int, u32, s32, u32, u32, int, u32)
ext 02515F14 void DaioctaHD_dCcD_Stts_Init(u32, int, int, u32)
ext 0251677C void DaioctaHD_dCcD_Sph_Set(u32, u32)
ext 025164C0 void DaioctaHD_dCcD_Cps_Set(u32, u32)
ext 028EFFD0 void DaioctaHD_construct_array(u32, u32, u32, u32)
ext 028F0164 void DaioctaHD_destroy_arr(u32, u32, u32, u32, u32, u32)
ext 025D4ED0 void DaioctaHD_fopAc_ac_c_fopAc_ac_c(u32)
ext 025E80D0 void DaioctaHD_mDoExt_brkAnm_construct(u32)
ext 024EFE94 void DaioctaHD_dBgS_AcchCir_dBgS_AcchCir(u32)
ext 025A5894 void DaioctaHD_dPa_followEcallBack_dPa_followEcallBack(u32, u32, u32)
ext 025E7C6C void DaioctaHD_mDoExt_btkAnm_construct(u32)
ext 02018C8C void DaioctaHD_cM3dGSph_SetR(u32, f32)
ext 02018D40 void DaioctaHD_cM3dGSph_SetC(u32, u32)
ext 02018808 void DaioctaHD_cM3dGCps_SetStartEnd(u32, u32, u32)
ext 025602F0 void DaioctaHD_dKy_get_seacolor(GXColor *, GXColor *)
ext 025E8154 void DaioctaHD_mDoExt_brkAnm_initModelData(u32, u32, u32, int, int, f32, int, int, int, int)
ext 025877F8 void DaioctaHD_dLib_setAnm(u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 025E535C void DaioctaHD_mDoExt_McaMorf_play(u32, u32, u32, u32)
ext 025E55A0 void DaioctaHD_mDoExt_McaMorf_calc(u32)
ext 024EEEB8 u32 DaioctaHD_dBgS_GetPolyColor(u32, u32)
ext 025E7FC4 void DaioctaHD_mDoExt_btkAnm_entry(u32, u32, f32)
ext 025E83FC void DaioctaHD_mDoExt_brkAnm_entry(u32, u32, f32)
ext 025E86B8 void DaioctaHD_mDoExt_bckAnm_entry(u32, u32, f32)
ext 025E5590 void DaioctaHD_mDoExt_McaMorf_entryDL(u32)
ext 0201B12C void DaioctaHD_cXyz_normZP(cXyz *, cXyz *)
ext 025E1960 void DaioctaHD_mDoAud_bgmAllMute(int)
ext 0200F164 void DaioctaHD_cLib_addCalcPos2(u32, cXyz *, f32, f32)
ext 025E1918 void DaioctaHD_mDoAud_subBgmStart(u32)
ext 02542D88 int DaioctaHD_dEvent_manager_c_getMyStaffId(u32, u32, u32, u32)
ext 02544830 u32 DaioctaHD_dEvent_manager_c_getMyNowCutName(u32, s32)
ext 02543280 void DaioctaHD_dEvent_manager_c_cutEnd(u32, s32)
ext 0254457C int DaioctaHD_dEvent_manager_c_endCheckOld(u32, u32)
ext 025D77DC void DaioctaHD_fopAcM_orderOtherEvent2(u32, u32, u32, u32)
ext 025E1AA4 void DaioctaHD_mDoAud_monsSeStart(int, u32, u32, u32, s32)
ext 025CB408 void DaioctaHD_dVibration_c_StartQuake(u32, int, int, cXyz *)
ext 0200F62C void DaioctaHD_cLib_chasePos(u32, cXyz *, f32)
ext 025E1928 void DaioctaHD_mDoAud_subBgmStop()
ext 025CB610 void DaioctaHD_dVibration_c_StopQuake(u32, int)
ext 025E1988 void DaioctaHD_mDoAud_seStart(int)
ext 025E8154 void DaioctaHD_mDoExt_brkAnm_initDemo(u32, u32, u32, int, int, f32, int, int, int, int)
ext 025E7CE0 void DaioctaHD_mDoExt_btkAnm_initDemo(u32, u32, u32, int, int, f32, int, int, int, int)
ext 024832E0 void DaioctaHD_daShip_c_initStartPos(u32, u32, s32)
ext 02520630 void DaioctaHD_dComIfGp_getMapTrans(s32, u32, u32, be<s16> *)
ext 0200F764 void DaioctaHD_cLib_chasePosXZ(u32, u32, f32)
ext 023FD4E4 void DaioctaHD_daPy_lk_c_startRestartRoom(u32, int, int, f32, int)
ext 0252012C void DaioctaHD_dComIfGp_setNextStage(u32, u32, s32, int, f32, int, int, int)
ext 02515FB8 void DaioctaHD_dCcD_GObjInf_dCcD_GObjInf(u32)
ext 02018150 void DaioctaHD_cM3dGCps_construct(u32)
ext 027F2BC0 void DaioctaHD_J3DFrameCtrl_init(u32, int)
ext 027DA984 void DaioctaHD_J3DAnmTransform_construct(u32)
ext 027F3628 void DaioctaHD_J3DAnmTransform_delete(u32, int)
ext 02018034 void DaioctaHD_cM3dGCir_delete(u32, int)
ext 024EFD9C void DaioctaHD_dBgS_Acch_delete(u32, int)
ext 02515860 void DaioctaHD_dCcD_Stts_delete(u32, int)
ext 025D50BC void DaioctaHD_fopAc_ac_c_dtor(u32, int)

data mode_tbl 1000CED8
str 1000D068 "d_a_daiocta.cpp" in 02118CE0
str 1000D078 "daiocta != (0)" in 02118CE0
str 1000D08C "d_a_daiocta.cpp" in 02118D60
str 1000D09C "modelData != (0)" in 02118D60
str 1000D0B0 "d_a_daiocta.cpp" in 02118E90
str 1000D0C0 "modelData != (0)" in 02118E90
str 1000D0D4 "d_a_daiocta.cpp" in 02118F90
str 1000D0E4 "modelData != (0)" in 02118F90
str 1000D0F8 "bck != 0" in 02118F90
str 1000D104 "btk != 0" in 02118F90
str 1000D110 "brk != 0" in 02118F90
str 1000D19C "d_a_daiocta.cpp" in 0211AB80
str 1000D1EC "brk != (0)" in 0211AB80
str 1000D2B0 "Daiocta" in 0211BFFC
str 1000D2B8 "DEATH1" in 0211BFFC
str 1000D2C0 "DEATH2" in 0211BFFC
str 1000D2C8 "d_a_daiocta.cpp" in 0211BFFC
str 1000D2D8 "cut_name != (0)" in 0211BFFC
str 1000D2E8 "uzu != (0)" in 0211BFFC
str 1000D2F4 "DAIOCTA_DEAD" in 0211BFFC
str 1000D304 "DAIOCTA_DEAD_ELF" in 0211BFFC
str 1000D318 "d_a_daiocta.cpp" in 0211C444
str 1000D328 "brk != (0)" in 0211C444
str 1000D334 "btk != (0)" in 0211C444
str 1000D35C "Daiocta" in 0211C59C
str 1000D364 "WAIT" in 0211C59C
str 1000D36C "d_a_daiocta.cpp" in 0211C59C
str 1000D37C "cut_name != (0)" in 0211C59C
str 1000D38C "DAIOCTA_SUIKOMI" in 0211C59C
str 1000D39C "ship != (0)" in 0211C59C
str 1000D3A8 "SUIKOMI1" in 0211C59C
str 1000D3B4 "SUIKOMI2" in 0211C59C
str 1000D3C0 "HAKIDASU" in 0211C59C
data a_brk_anm_idx_tbl 1000D1AC
data a_brk_anm_prm_tbl 1000D1C8
data a_anm_idx_tbl 1000D164
data l_anmParameters 1000D1F8
data l_sphereJoints 1000D150
data search_data 101B3F84
data l_matrixStack 1048D0CC
data m_arc_name 1000D418
*/

// clang-format on
