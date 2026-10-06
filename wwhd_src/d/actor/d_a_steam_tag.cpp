/**
 * d_a_steam_tag.cpp (WWHD)
 * Tag - Steam vent (particle emitter, wind capsule, point wind).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_steam_tag.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003D74C /* this TU's sead::SafeString vtable */
#define STEAMTAG_VTBL 0x1003D774   /* daSteamTag_c vtable (HD virtual destructor) */
#define AAB_VTBL 0x1003D764        /* this TU's cM3dGAab vtable */
#define l_cps_src 0x101D1020       /* dCcD_SrcCps */

/* static u8 daSteamTag_c::mEmitterNum */
static inline u32 mEmitterNum_ea() { return 0x10475650; }

enum { JA_SE_OBJ_STEAM = 0x612E };
enum { dPa_name_ID_AK_SN_STEAM00 = 0x808B, dPa_name_ID_AK_SN_STEAM01 = 0x808C }; /* HD ids */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025164C0 dCcD_Cps::Set(const dCcD_SrcCps&) */
static inline void dCcD_Cps_Set(dCcD_Cps* c, u32 src) { gabi::call(0x025164C0, c, src); }
/* 020181FC cM3dGCps::Set(const cM3dGCpsS&) */
static inline void cM3dGCps_Set(void* cps, void* src) { gabi::call(0x020181FC, cps, src); }
/* 028E8DAC PSVECSubtract(a, b, out) */
static inline void PSVECSubtract(const void* a, const void* b, void* out) { gabi::call(0x028E8DAC, a, b, out); }
/* dPointWind_c: 025AB3F8 set_pwind_init(cM3dGCpsS*), 025AB454 set_pwind_move(), 025AB710 set_pwind_delete() */
static inline void dPointWind_set_pwind_init(void* w, void* cps) { gabi::call(0x025AB3F8, w, cps); }
static inline void dPointWind_set_pwind_move(void* w) { gabi::call(0x025AB454, w); }
static inline void dPointWind_set_pwind_delete(void* w) { gabi::call(0x025AB710, w); }
/* 028245AC JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, Mtx out) */
static inline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx) { gabi::call(0x028245AC, x, y, z, mtx); }
/* 0254DA50 checkItemGet(u8 item, int) */
static inline BOOL checkItemGet(u8 item, s32 flag) { return gabi::call<BOOL>(0x0254DA50, item, flag); }
/* 02515980 dCcD_Cps::~dCcD_Cps (matcher: dCcD_GObjInf::~dCcD_GObjInf) */
static inline void dCcD_Cps_dt(dCcD_Cps* c, s32 flags) { gabi::call(0x02515980, c, flags); }
/* dCcD_Cps constructor as GHS expands it (cf. dCcD_Cyl_ct): 02018150 cM3dGCps::cM3dGCps */
static inline void dCcD_Cps_ct(dCcD_Cps* c, u32 tu_aab_vtbl) {
    gabi::call(0x02515FB8, c);                          /* dCcD_GObjInf::dCcD_GObjInf */
    gabi::store<u32>(gabi::ea(c) + 0x114, 0x100015A8);  /* cCcD_ShapeAttr */
    gabi::store<u32>(gabi::ea(c) + 0x110, tu_aab_vtbl); /* cM3dGAab (per TU) */
    gabi::call(0x02018150, c->mCps);                    /* cM3dGCps::cM3dGCps */
    c->__vtbl_hitinf = 0x1004AF18;                      /* dCcD_Cps */
    gabi::store<u32>(gabi::ea(c) + 0x114, 0x1004AF70);
    gabi::store<u32>(gabi::ea(c) + 0x130, 0x1004AF60);  /* cM3dGCps vtable */
}
/* JPABaseEmitter (HD offsets): global translation +0x22C, flag byte +0x262 (>= 7: y negated),
 * global rotation matrix +0x1F0, global alpha +0x247, status +0x254 (bit 0: stop create),
 * particle lists' counts +0x1B4/+0x1C0, +0x5C (-1: invalid) */
static inline u32 em(fopAc_ac_c* a) { return gabi::load<u32>(gabi::ea(a) + 0x3AC); }
static inline void JPA_stopCreateParticle(u32 e) { gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1); }
static inline void JPA_playCreateParticle(u32 e) { gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) & ~1u); }
static inline void JPA_setGlobalAlpha(u32 e, u8 a) { gabi::store<u8>(e + 0x247, a); }
static inline s32 JPA_getParticleNumber(u32 e) { return gabi::load<s32>(e + 0x1B4) + gabi::load<s32>(e + 0x1C0); }
static inline void JPA_becomeInvalidEmitter(u32 e) {
    gabi::store<s32>(e + 0x5C, -1);
    JPA_stopCreateParticle(e);
}
/* sead::SafeString equality (HD inline), as in d_a_kb.h */
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop) return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i)) return false;
        if (c == 0) return true;
    }
    return false;
}

struct cM3dGCpsS {
    /* 0x00 */ cXyz mStart;
    /* 0x0C */ cXyz mEnd;
    /* 0x18 */ be<f32> mRadius;
};

struct daSteamTag_c : fopAc_ac_c {
    BOOL CreateInit();
    BOOL createEmitter();
    bool endEmitter();
    cPhs_State create();
    BOOL execute();

    /* 0x3AC */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x3B0 */ u8 _3B0[2];
    /* 0x3B2 */ be<s16> mEmitTimer;
    /* 0x3B4 */ be<s16> mCreateTimer;
    /* 0x3B6 */ be<u8> m29A;
    /* 0x3B7 */ be<u8> m29B;
    /* 0x3B8 */ dCcD_Stts mGStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ dCcD_Cps mCps;
    /* 0x65C */ cM3dGCpsS mPntWindCpsS;
    /* 0x678 */ u8 mPointWind[0x20]; /* dPointWind_c */
    /* 0x698 */ be<f32> mPointWindPower; /* dPointWind_c power (+0x20) */
};
WWHD_OFFSET(daSteamTag_c, mGStts, 0x3B8);
WWHD_OFFSET(daSteamTag_c, mCyl, 0x3F4);
WWHD_OFFSET(daSteamTag_c, mCps, 0x524);
WWHD_OFFSET(daSteamTag_c, mPntWindCpsS, 0x65C);
WWHD_OFFSET(daSteamTag_c, mPointWind, 0x678);

/* 0249A5F8 (getData() inlined: the mData constants are immediates) */
BOOL daSteamTag_c::CreateInit() {
    WWHD_FUNC(0x0249A5F8, BOOL, this);
    m29B = (fopAcM_GetParam(this) >> 2) & 0xFF; /* daSteamTag_prm::getSchBit */
    mEmitTimer = (s16)gabi::ftoi(50.0f + cM_rndF(150.0f));
    s16 ct = (s16)gabi::ftoi(cM_rndF(300.0f));
    mpEmitter = nullptr;
    mCreateTimer = ct;
    mEmitTimer = 0;
    mGStts.Init(0xFF, 0xFF, this);
    dCcD_Cps_Set(&mCps, l_cps_src);
    mCps.SetStts(&mGStts);
    mPntWindCpsS.mStart.copy(current.pos);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    f32 windY = 200.0f;
    if (current.angle.x != 0) {
        windY = 50.0f;
    }
    mDoMtx_stack_c::transM(0.0f, windY, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &mPntWindCpsS.mEnd);
    mPntWindCpsS.mRadius = 50.0f;
    cM3dGCps_Set(mCps.mCps, &mPntWindCpsS);
    /* mCps.CalcAtVec(): at vector = end - start */
    PSVECSubtract(mCps.mCps + 0xC, mCps.mCps, &mCps.mGObjAt.mVec);
    dPointWind_set_pwind_init(mPointWind, &mPntWindCpsS);
    fopAcM_offDraw(this);
    return TRUE;
}
VERIFY(0x0249A5F8, &daSteamTag_c::CreateInit);

/* 0249A278 HD: dPa_control_c::set (group 2) instead of setToon */
BOOL daSteamTag_c::createEmitter() {
    WWHD_FUNC(0x0249A278, BOOL, this);
    if (gabi::load<u8>(mEmitterNum_ea()) < 8) {
        u16 particleID;
        if (((s16)gabi::ftoi(cM_rndF(100.0f)) % 2) != 0) {
            particleID = dPa_name_ID_AK_SN_STEAM00;
        } else {
            particleID = dPa_name_ID_AK_SN_STEAM01;
        }
        mpEmitter = dPa_control_set(dComIfGp_getParticle(), 2, particleID, &current.pos, &current.angle, &scale, 0xB4,
                                    nullptr, -1, nullptr, nullptr, nullptr);
        if (mpEmitter) {
            gabi::store<u8>(mEmitterNum_ea(), (u8)(gabi::load<u8>(mEmitterNum_ea()) + 1));
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x0249A278, &daSteamTag_c::createEmitter);

/* 0249A378 */
bool daSteamTag_c::endEmitter() {
    WWHD_FUNC(0x0249A378, bool, this);
    gabi::store<u8>(mEmitterNum_ea(), (u8)(gabi::load<u8>(mEmitterNum_ea()) - 1));
    return TRUE;
}
VERIFY(0x0249A378, &daSteamTag_c::endEmitter);

/* 0249AA4C */
static BOOL daSteamTag_Draw(daSteamTag_c*) {
    WWHD_FUNC(0x0249AA4C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0249AA4C, daSteamTag_Draw);

/* execute() inlined into daSteamTag_Execute */
BOOL daSteamTag_c::execute() {
    f32 windStrength;
    if ((mEmitTimer == 0) && (mCreateTimer > 0)) {
        mCreateTimer = mCreateTimer - 1;
        if (mCreateTimer == 0) {
            if (createEmitter()) {
                /* setGlobalTranslation / setGlobalRotation / setGlobalAlpha / playCreateParticle */
                u32 e = em(this);
                s16 ax = current.angle.x, ay = current.angle.y, az = current.angle.z;
                f32 px = current.pos.x, py = current.pos.y, pz = current.pos.z;
                gabi::store<f32>(e + 0x22C, px);
                gabi::store<f32>(e + 0x230, py);
                gabi::store<f32>(e + 0x234, pz);
                if (gabi::load<u8>(e + 0x262) >= 7) /* HD */
                    gabi::store<f32>(e + 0x230, -gabi::load<f32>(e + 0x230));
                JPAGetXYZRotateMtx(ax, ay, az, em(this) + 0x1F0);
                JPA_setGlobalAlpha(em(this), 0xB4);
                JPA_playCreateParticle(em(this));
                mEmitTimer = (s16)gabi::ftoi(50.0f + cM_rndF(150.0f));
            } else {
                mCreateTimer = 30;
            }
        }
    } else if ((mCreateTimer == 0) && (mEmitTimer > 0)) {
        mEmitTimer = mEmitTimer - 1;
        if (mEmitTimer == 0) {
            if (mpEmitter != nullptr) {
                windStrength = 0.0f;
                JPA_stopCreateParticle(em(this));
                u32 e = em(this);
                if (JPA_getParticleNumber(e) == 0) {
                    JPA_becomeInvalidEmitter(e);
                    endEmitter();
                    mpEmitter = nullptr;
                    mCreateTimer = (s16)gabi::ftoi(100.0f + cM_rndF(300.0f));
                } else {
                    mEmitTimer = 1;
                    windStrength = 1.0f;
                }
                mPointWindPower = windStrength; /* mPointWind.set_pwind_power */
            }
        } else {
            mDoAud_seStart(JA_SE_OBJ_STEAM, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            if (mpEmitter != nullptr) {
                JPA_setGlobalAlpha(em(this), 0xB4);
            }
            cM3dGCps_Set(mCps.mCps, &mPntWindCpsS);
            dComIfG_Ccsp_Set(&mCps);
            dPointWind_set_pwind_move(mPointWind);
        }
    }
    return TRUE;
}

/* 0249A390 */
static BOOL daSteamTag_Execute(daSteamTag_c* i_this) {
    WWHD_FUNC(0x0249A390, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x0249A390, daSteamTag_Execute);

/* 0249A5E8 */
static BOOL daSteamTag_IsDelete(daSteamTag_c*) {
    WWHD_FUNC(0x0249A5E8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0249A5E8, daSteamTag_IsDelete);

/* 0249A5F0 HD: the destructor is no longer called here (it is the virtual destructor, run by
 * the framework) */
static BOOL daSteamTag_Delete(daSteamTag_c*) {
    WWHD_FUNC(0x0249A5F0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0249A5F0, daSteamTag_Delete);

/* 0249AA68 daSteamTag_c deleting destructor (~daSteamTag_c + member destructors) */
static void daSteamTag_c_dt(daSteamTag_c* i_this, s32 flags) {
    WWHD_FUNC(0x0249AA68, void, i_this, flags);
    if (i_this != nullptr) {
        i_this->__vtbl = STEAMTAG_VTBL;
        if (i_this->mpEmitter != nullptr) {
            JPA_becomeInvalidEmitter(em(i_this));
            i_this->mpEmitter = nullptr;
        }
        dPointWind_set_pwind_delete(i_this->mPointWind);
        dCcD_Cps_dt(&i_this->mCps, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mGStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0249AA68, daSteamTag_c_dt);

/* create() inlined into daSteamTag_Create */
cPhs_State daSteamTag_c::create() {
    /* fopAcM_ct(this, daSteamTag_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = STEAMTAG_VTBL;
            dCcD_Stts_ct(&mGStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
            dCcD_Cps_ct(&mCps, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    CreateInit();

    cPhs_State phase_state;
    /* strcmp(dComIfGp_getStartStageName(), "Adanmae") == 0 (HD: sead::SafeString ==) */
    gabi::Local<SafeString> a;
    a->mStringTop = 0x1003D740; /* "Adanmae" */
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    if (SafeString_eq(a, b) && (current.roomNo == 0) && checkItemGet(0x6A /* dItemNo_PEARL_DIN_e */, TRUE)) {
        phase_state = cPhs_ERROR_e;
    } else {
        phase_state = cPhs_COMPLEATE_e;
    }
    return phase_state;
}

/* 0249A78C */
static cPhs_State daSteamTag_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0249A78C, cPhs_State, i_this);
    return ((daSteamTag_c*)i_this)->create();
}
VERIFY(0x0249A78C, daSteamTag_Create);

/* 0249A9B8 */
static void __sinit_d_a_steam_tag_cpp() {
    WWHD_FUNC(0x0249A9B8, void, (u32)0);
    sinit_header_statics(0x1046E00C, 0x101D106C);
}
VERIFY(0x0249A9B8, __sinit_d_a_steam_tag_cpp);

/* 0249AA54: sead::SafeString deleting destructor (this TU's copy; trivial) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0249AA54, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0249AA54, SafeString_dt);

/* 0249AB1C: sead::SafeString::assureTermination (this TU's copy; empty) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x0249AB1C, void, (u32)0);
}
VERIFY(0x0249AB1C, SafeString_assureTermination);
