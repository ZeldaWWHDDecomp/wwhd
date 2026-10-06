/**
 * d_a_hookshot.cpp (WWHD)
 * Item - Hookshot
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_hookshot.cpp) to the WWHD layout and code, verified against
 * cking.rpx under documented verification contracts. "HD:" marks GameCube differences.
 *
 * HD: the chain is drawn by a new renderer: daHookshot_shape grew from a 0x10-byte J3DPacket to
 * 0xD0A0 bytes (vertex buffers, a material, up to 300 chain links of 0xA8 bytes at +0x980), and
 * an HD object of 0xB0 bytes follows it at 0xD44C. The GameCube members follow at 0xD4FC
 * (GameCube offset + 0xD25C). The shape's constructor (0217718C), destructor (02177FD4) and GX2
 * draw (021783F4) are reconstructed in d_a_hookshot_shape.cpp with explicit proof scopes.
 */
#include "bindings.h"
namespace {
struct HookshotLinkage {u8 bytes[32];};
// Every live guest local keeps the incoming EABI linkage below its own storage,
// including calls made by shared bindings.
template<class T> struct HookshotLocal {
    gabi::Local<T> value;
    gabi::Local<HookshotLinkage> linkage;
    T* get() const {return value.get();}
    T* operator->() const {return get();}
    T& operator*() const {return *get();}
    operator T*() const {return get();}
};
}


#define SAFESTRING_VTBL 0x10011560 /* this TU's sead::SafeString vtable */
#define HOOKSHOT_VTBL 0x100116A8   /* HD: daHookshot_c vtable */
#define l_at_cps_src 0x101B76AC
/* g_Counter.mTimer */
#define G_COUNTER_TIMER 0x101FF560u

enum {
    JA_SE_LK_HS_CHAIN = 0x206C,
    JA_SE_LK_HS_WIND_UP = 0x206D,
    JA_SE_LK_HS_WIND_UP_FIN = 0x286E,
    JA_SE_LK_HS_SPIKE = 0x286F,
    JA_SE_LK_HS_REBOUND = 0x287B,
};
enum { Mode_Wait = 0, Mode_Shot = 1, Mode_Return = 2, Mode_Pull = 3 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 028E8DE8 PSVECSquareDistance(a, b) */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* 028E8DAC PSVECSubtract(a, b, out) */
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
/* daPy_lk_c::getHookshotRootPos(): +0x72F0 (HD) */
static inline cXyz* link_hookshotRootPos(fopAc_ac_c* link) { return gabi::at<cXyz>(gabi::ea(link) + 0x72F0); }
/* daPy_lk_c::getBodyAngleX/Y (HD): +0x3D0 / +0x3D2 */
static inline u16 link_bodyAngleX(fopAc_ac_c* link) { return gabi::load<u16>(gabi::ea(link) + 0x3D0); }
static inline s16 link_bodyAngleY(fopAc_ac_c* link) { return gabi::load<s16>(gabi::ea(link) + 0x3D2); }
/* dComIfGp_roomControl_getStayNo(): s8 at 0x1047E6C8 */
static inline u8 dComIfGp_roomControl_getStayNo() { return gabi::load<u8>(0x1047E6C8); }
/* 024EF398 dBgS::ChkPolyHSStick (HD: out of line; the matcher calls it GetPolyId2) */
static inline BOOL dBgS_ChkPolyHSStick(dBgS* bgs, void* polyInfo) { return gabi::call<BOOL>(0x024EF398, bgs, polyInfo); }
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&) */
static inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo) { return gabi::call<s32>(0x024EF0F4, bgs, polyInfo); }
/* 02008254 cBgS::ChkPolySafe, 024EEABC dBgS::ChkMoveBG, 024EFA38 dBgS::MoveBgTransPos */
static inline BOOL cBgS_ChkPolySafe(dBgS* bgs, void* polyInfo) { return gabi::call<BOOL>(0x02008254, bgs, polyInfo); }
static inline BOOL dBgS_ChkMoveBG(dBgS* bgs, void* polyInfo) { return gabi::call<BOOL>(0x024EEABC, bgs, polyInfo); }
static inline void dBgS_MoveBgTransPos(dBgS* bgs, void* polyInfo, bool b, cXyz* pos, csXyz* a, csXyz* s) {
    gabi::call(0x024EFA38, bgs, polyInfo, b, pos, a, s);
}
/* 02017300 cM3d_CalcVecZAngle(const Vec&, csXyz*) */
static inline void cM3d_CalcVecZAngle(void* v, csXyz* out) { gabi::call(0x02017300, v, out); }
/* dCcD_Cps: cM3dGCps::Set(start, end, r) (020181B0 on the cps at +0x118) + CalcAtVec (At vec +0x7C) */
static inline void cps_SetStartEnd(u32 cps, cXyz* s, cXyz* e, f32 r) { gabi::call(0x020181B0, cps + 0x118, s, e, r); }
static inline void cps_CalcAtVec(u32 cps) {
    PSVECSubtract(gabi::at<cXyz>(cps + 0x124), gabi::at<cXyz>(cps + 0x118), gabi::at<cXyz>(cps + 0x7C));
}
/* dCcMassS_Mng::Set (play+0x4EF8) */
static inline void dComIfG_Ccsp_SetMass(u32 obj, u8 type) { gabi::call(0x02516C14, dComIfGp_ea() + 0x4EF8, obj, type); }
/* sead::SafeString equality (HD inline, as in d_a_kb) */
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
static inline bool dComIfGp_isStartStage(u32 lit) {
    HookshotLocal<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL;
    HookshotLocal<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}

static inline void fopAcM_SetParam(fopAc_ac_c* a, u32 v) { a->mParameters = v; }

/* GHS pointer to member function {s16 delta, s16 index, u32 fn} */
struct ProcFunc_l {
    be<s16> delta;
    be<s16> index;
    be<u32> fn;
};

struct daHookshot_c : fopAc_ac_c {
    BOOL draw();
    BOOL procWait_init(BOOL);
    BOOL procWait();
    BOOL procShot();
    BOOL procPlayerPull();
    BOOL procReturn();
    BOOL execute();
    BOOL hookshot_delete();
    cPhs_State create();
    void setProc(u32 fn) {
        mCurrProcFunc.delta = 0;
        mCurrProcFunc.index = -1;
        mCurrProcFunc.fn = fn;
    }
    u32 sightCps() { return gabi::ea(this) + 0xD630; }

    /* 0x03AC */ u8 mShape[0xD0A0];  /* HD daHookshot_shape (user area +0x18) */
    /* 0xD44C */ u8 mHdD44C[0xB0];   /* HD object (constructor 02080404) */
    /* 0xD4FC */ be<u8> m2A0;
    /* 0xD4FD */ be<u8> m2A1;
    /* 0xD4FE */ be<u8> mShipRideFlg;
    /* 0xD4FF */ be<u8> m2A3;
    /* 0xD500 */ be<s16> m2A4;
    /* 0xD502 */ be<u16> m2A6;
    /* 0xD504 */ be<s32> mChainCnt;
    /* 0xD508 */ be<u32> mMtrlSndId;
    /* 0xD50C */ be<s32> mObjHookFlg;
    /* 0xD510 */ csXyz mHookAngle;
    /* 0xD516 */ csXyz m2BA;
    /* 0xD51C */ cXyz mMoveVec;
    /* 0xD528 */ cXyz mObjSightCrossPos;
    /* 0xD534 */ u8 mLinChk[0x6C];   /* dBgS_RopeLinChk (poly info +0x14, cross +0x30, flags +0x4C) */
    /* 0xD5A0 */ u8 mGndChk[0x54];   /* dBgS_ObjGndChk */
    /* 0xD5F4 */ dCcD_Stts mStts;
    /* 0xD630 */ u8 mSightCps[0x138]; /* dCcD_Cps */
    /* 0xD768 */ cXyz mCarryOffset;
    /* 0xD774 */ be<u32> mCarryActorID;
    /* 0xD778 */ Mtx34 mMtx;
    /* 0xD7A8 */ ProcFunc_l mCurrProcFunc;
};
WWHD_OFFSET(daHookshot_c, mHdD44C, 0xD44C);
WWHD_OFFSET(daHookshot_c, m2A0, 0xD4FC);
WWHD_OFFSET(daHookshot_c, mChainCnt, 0xD504);
WWHD_OFFSET(daHookshot_c, mHookAngle, 0xD510);
WWHD_OFFSET(daHookshot_c, mLinChk, 0xD534);
WWHD_OFFSET(daHookshot_c, mStts, 0xD5F4);
WWHD_OFFSET(daHookshot_c, mSightCps, 0xD630);
WWHD_OFFSET(daHookshot_c, mCarryOffset, 0xD768);
WWHD_OFFSET(daHookshot_c, mMtx, 0xD778);
WWHD_OFFSET(daHookshot_c, mCurrProcFunc, 0xD7A8);
WWHD_SIZE(daHookshot_c, 0xD7B0);

#define PROC_WAIT 0x021789B4u
#define PROC_SHOT 0x02178DDCu
#define PROC_PULL 0x02179888u
#define PROC_RETURN 0x02179B98u
#define ROCKLINE_CALLBACK 0x02176CE0u

/* 0217674C: HD: Mtx returned by value (dst = src through FPRs) */
static void mtx_copy_value(Mtx34* dst, const Mtx34* src) {
    WWHD_FUNC(0x0217674C, void, dst, src);
    f32 v[12];
    for (int i = 0; i < 12; i++)
        v[i] = gabi::load<f32>(gabi::ea(src) + 4 * i);
    for (int i = 0; i < 12; i++)
        gabi::store<f32>(gabi::ea(dst) + 4 * i, v[i]);
}
VERIFY(0x0217674C, mtx_copy_value);

/* 021767EC: HD: GXColorS10 -> four floats / 255 (returned by value) */
static void colorS10_toF(f32* out, const s16* c) {
    WWHD_FUNC(0x021767EC, void, out, c);
    u32 o = gabi::ea(out), s = gabi::ea(c);
    f32 r = (f32)gabi::load<s16>(s + 0) / 255.0f;
    f32 g = (f32)gabi::load<s16>(s + 2) / 255.0f;
    f32 b = (f32)gabi::load<s16>(s + 4) / 255.0f;
    f32 a = (f32)gabi::load<s16>(s + 6) / 255.0f;
    gabi::store<f32>(o + 0, r);
    gabi::store<f32>(o + 4, g);
    gabi::store<f32>(o + 8, b);
    gabi::store<f32>(o + 12, a);
}
VERIFY(0x021767EC, colorS10_toF);

/* 021768B0: daHookshot_shape::draw (HD: fills the chain links' matrices and the material colours;
 * the GX2 draw itself is the virtual 021783F4) */
static void daHookshot_shape_draw(u8* shape) {
    WWHD_FUNC(0x021768B0, void, shape);
    u32 sh = gabi::ea(shape);
    daHookshot_c* hookshot = gabi::at<daHookshot_c>(gabi::load<u32>(sh + 0x18)); /* getUserArea() */
    u32 hs = gabi::ea(hookshot);
    s32 chain_count = hookshot->mChainCnt;
    if (chain_count <= 0)
        return;

    gabi::call(0x0255F84C);
    HookshotLocal<Mtx34> viewMtx;
    mtx_copy_value(viewMtx, gabi::at<Mtx34>(0x104B45C0 + 0x38)); /* j3dSys.getViewMtx() */
    u32 r5 = gabi::load<u32>(0x104B45C0 + 0x148);
    gabi::call(0x027FDA54, sh + 0x564, 0, viewMtx.get(), 0x104B45C0 + 0x14C, r5 + 0x240);

    u32 mat = gabi::load<u32>(sh + 0x568);
    HookshotLocal<f32[4]> c;
    colorS10_toF(*c, gabi::at<s16>(hs + 0x1A0)); /* tevStr.mColorC0 */
    HookshotLocal<f32[4]> c1;
    gabi::call(0x0274D458, c1.get(), c.get(), gabi::load<f32>(hs + 0x138));
    for (int i = 0; i < 4; i++)
        gabi::store<u32>(mat + 0x1C4 + 4 * i, gabi::load<u32>(gabi::ea(c1.get()) + 4 * i));
    mat = gabi::load<u32>(sh + 0x568);
    colorS10_toF(*c, gabi::at<s16>(hs + 0x270));
    HookshotLocal<f32[4]> c2;
    gabi::call(0x0274D458, c2.get(), c.get(), gabi::load<f32>(hs + 0x27C));
    for (int i = 0; i < 4; i++)
        gabi::store<u32>(mat + 0x1D4 + 4 * i, gabi::load<u32>(gabi::ea(c2.get()) + 4 * i));
    gabi::call(0x027FDFF4, sh + 0x564, 0);

    /* tevStr.mColorC0 (s16) and mColorK0 (u8) as floats / 255 */
    f32 r = (f32)gabi::load<s16>(hs + 0x1A0) / 255.0f;
    f32 g = (f32)gabi::load<s16>(hs + 0x1A2) / 255.0f;
    f32 b = (f32)gabi::load<s16>(hs + 0x1A4) / 255.0f;
    f32 a = (f32)gabi::load<s16>(hs + 0x1A6) / 255.0f;
    gabi::store<f32>(sh + 0x6D0, g);
    gabi::store<f32>(sh + 0x6D4, b);
    gabi::store<f32>(sh + 0x6CC, r);
    gabi::store<f32>(sh + 0x6D8, a);
    f32 kr = (f32)gabi::load<u8>(hs + 0x1A8) / 255.0f;
    f32 kg = (f32)gabi::load<u8>(hs + 0x1A9) / 255.0f;
    f32 kb = (f32)gabi::load<u8>(hs + 0x1AA) / 255.0f;
    f32 ka = (f32)gabi::load<u8>(hs + 0x1AB) / 255.0f;
    gabi::store<f32>(sh + 0x6E0, kg);
    gabi::store<f32>(sh + 0x6DC, kr);
    gabi::store<f32>(sh + 0x6E4, kb);
    gabi::store<f32>(sh + 0x6E8, ka);
    gabi::call(0x0274D2AC, sh + 0x6DC, gabi::load<f32>(hs + 0x134));
    u32 m2 = gabi::load<u32>(sh + 0x568);
    for (int i = 0; i < 4; i++)
        gabi::store<u32>(sh + 0x6AC + 4 * i, gabi::load<u32>(m2 + 0x1C4 + 4 * i));
    gabi::call(0x027FB678, sh + 0x618);

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    HookshotLocal<cXyz> chain_pos;
    cXyz* root = link_hookshotRootPos(link);
    chain_pos->x = root->x;
    chain_pos->y = root->y;
    chain_pos->z = root->z;
    u16 sx = gabi::load<u16>(hs + 0x328), sy = gabi::load<u16>(hs + 0x32A);
    HookshotLocal<cXyz> chain_offset;
    f32 cx = 7.0f * cM_scos(sx);
    chain_offset->x = cx * cM_ssin(sy);
    chain_offset->y = -7.0f * cM_ssin(sx);
    chain_offset->z = cx * cM_scos(sy);
    u32 timer = gabi::load<u32>(G_COUNTER_TIMER);
    s16 z = (s16)((timer << 12) | ((timer >> 4) & 0xFFF));
    for (s32 i = 0; i < chain_count; i++) {
        u32 lnk = sh + 0x980 + i * 0xA8;
        PSMTXTrans(mDoMtx_stack_c::get(), chain_pos->x, chain_pos->y, chain_pos->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), hookshot->shape_angle.x, hookshot->shape_angle.y, z);
        u32 t = gabi::load<u32>(G_COUNTER_TIMER);
        z = (s16)(z + (((((t + i) << 12) | ((t >> ((i & 0xF) + 1)) & 0xFFF))) & 0x1FFF) + 0x3000);
        HookshotLocal<Mtx34> m;
        mtx_copy_value(m, mDoMtx_stack_c::get());
        PSMTXCopy(m, gabi::at<Mtx34>(lnk + 0x74));
        gabi::call(0x027FB678, lnk);
        PSVECAdd(chain_pos, chain_offset, chain_pos);
    }
}
VERIFY(0x021768B0, daHookshot_shape_draw);

/* 02176C50: HD: the shape is entered, then an HD object and the chain matrices are updated */
BOOL daHookshot_c::draw() {
    WWHD_FUNC(0x02176C50, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    gabi::call(0x027F0E04, gabi::load<u32>(dComIfGp_ea() + 0x5D58), mShape, 0); /* dComIfGd_getOpaListP1()->entryImm */
    u32 p = gabi::ea(this) + 0xD44C;
    gabi::store<u32>(p + 0xA4, gabi::ea(&tevStr));
    gabi::call(0x02415A80, dComIfGp_getLinkPlayer(), p, mShape);
    daHookshot_shape_draw(mShape);
    return TRUE;
}
VERIFY(0x02176C50, &daHookshot_c::draw);

/* 02176CDC */
static BOOL daHookshot_Draw(daHookshot_c* i_this) {
    WWHD_FUNC(0x02176CDC, BOOL, i_this);
    return i_this->draw();
}
VERIFY(0x02176CDC, daHookshot_Draw);

/* 02176CE0 */
static void daHookshot_rockLineCallback(fopAc_ac_c* hookshot_actor, dCcD_GObjInf* objInf, fopAc_ac_c* collided_actor, dCcD_GObjInf*) {
    WWHD_FUNC(0x02176CE0, void, hookshot_actor, objInf, collided_actor, (u32)0);
    daHookshot_c* i_this = (daHookshot_c*)hookshot_actor;
    cXyz* hitPos = gabi::at<cXyz>(gabi::ea(objInf) + 0x70); /* GetAtHitPosP() */
    f32 f31 = PSVECSquareDistance(&i_this->mObjSightCrossPos, &i_this->current.pos);
    f32 f1 = PSVECSquareDistance(hitPos, &i_this->current.pos);
    if (f31 > f1) {
        i_this->mObjSightCrossPos.copy(*hitPos);
        /* HD (USA): fopAcStts_UNK80000_e | UNK200000_e | UNK10000000_e */
        i_this->mObjHookFlg = (collided_actor->actor_status & 0x10280000) != 0;
    }
}
VERIFY(0x02176CE0, daHookshot_rockLineCallback);

/* 02176D98: HD: the chain count is clamped to 300; the water effect is a free function */
BOOL daHookshot_c::execute() {
    WWHD_FUNC(0x02176D98, BOOL, this);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if (fopAcM_GetParam(this) != Mode_Wait) {
        mDoAud_seStart(JA_SE_LK_HS_CHAIN, &link->current.pos, 0, dComIfGp_getReverb(current.roomNo));
        if (fopAcM_GetParam(this) != Mode_Shot)
            mDoAud_seStart(JA_SE_LK_HS_WIND_UP, &link->current.pos, 0, dComIfGp_getReverb(current.roomNo));
    }
    if (mCurrProcFunc.index != 0)
        ptmf_call(gabi::ea(&mCurrProcFunc), this);

    /* eyePos = attention_info.position = current.pos (word copies) */
    u32 b = gabi::ea(this);
    u32 px = gabi::load<u32>(b + 0x314), py = gabi::load<u32>(b + 0x318), pz = gabi::load<u32>(b + 0x31C);
    gabi::store<u32>(b + 0x390, px);
    gabi::store<u32>(b + 0x394, py);
    gabi::store<u32>(b + 0x398, pz);
    gabi::store<u32>(b + 0x37C, px);
    gabi::store<u32>(b + 0x380, py);
    gabi::store<u32>(b + 0x384, pz);

    HookshotLocal<cXyz> root;
    cXyz* r = link_hookshotRootPos(link);
    root->x = r->x;
    root->y = r->y;
    root->z = r->z;
    HookshotLocal<cXyz> sp3C;
    cXyz_mi(&current.pos, sp3C, root);
    s32 cnt = gabi::ftoi(std_sqrtf(PSVECSquareMag(sp3C)) / 7.0f);
    if (cnt > 300)
        cnt = 300;
    mChainCnt = cnt;

    if (fopAcM_GetParam(this) != Mode_Pull || cnt > 18) {
        HookshotLocal<cXyz> xz;
        xz->x = sp3C->x;
        xz->y = 0.0f;
        xz->z = sp3C->z;
        f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
        shape_angle.x = cM_atan2s(-sp3C->y, absXZ);
        shape_angle.y = cM_atan2s(sp3C->x, sp3C->z);
    } else {
        shape_angle.x = link->shape_angle.x;
        shape_angle.y = link->shape_angle.y;
    }

    if (fopAcM_GetParam(this) != Mode_Wait && !m2A0) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, 0);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), -0x4000);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    } else {
        m2A0 = false;
    }

    u8 roomNo = dComIfGp_roomControl_getStayNo();
    gabi::store<u8>(b + 0x1C9, roomNo);       /* tevStr.mRoomNo */
    gabi::store<u8>(b + 0xD5F4 + 0x22, roomNo); /* mStts.SetRoomId */
    gabi::store<u8>(b + 0x326, roomNo);       /* current.roomNo */

    if (fopAcM_GetParam(this) != Mode_Wait) {
        /* HD: setItemWaterEffect(actor, u8, int) is not a member of the player */
        m2A1 = gabi::call<u8>(0x02443208, this, (u8)m2A1, 0);
        if (fopAcM_GetParam(this) == Mode_Shot) {
            speedF = std_sqrtf(PSVECSquareDistance(&current.pos, &old.pos));
            return TRUE;
        }
        if (fopAcM_GetParam(this) == Mode_Return || fopAcM_GetParam(this) == Mode_Pull) {
            speedF = -std_sqrtf(PSVECSquareDistance(&current.pos, &old.pos));
            return TRUE;
        }
    }
    speedF = 0.0f;
    return TRUE;
}
VERIFY(0x02176D98, &daHookshot_c::execute);

/* 021770EC */
static BOOL daHookshot_Execute(daHookshot_c* i_this) {
    WWHD_FUNC(0x021770EC, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x021770EC, daHookshot_Execute);

/* 021770F0 */
static BOOL daHookshot_IsDelete(daHookshot_c* i_this) {
    WWHD_FUNC(0x021770F0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021770F0, daHookshot_IsDelete);

/* 021770F8 */
BOOL daHookshot_c::hookshot_delete() {
    WWHD_FUNC(0x021770F8, BOOL, this);
    if (mCarryActorID != 0xFFFFFFFFu) {
        fopAc_ac_c* carry_actor = fopAcM_SearchByID(mCarryActorID);
        if (carry_actor != nullptr) {
            u32 st = carry_actor->actor_status;
            if (st & 0x00100000 /* fopAcStts_HOOK_CARRY_e */) {
                carry_actor->actor_status = st & ~0x00100000u; /* fopAcM_cancelHookCarryNow */
                mCarryActorID = 0xFFFFFFFFu;
            }
        }
    }
    return TRUE;
}
VERIFY(0x021770F8, &daHookshot_c::hookshot_delete);

/* 02177168 */
static BOOL daHookshot_Delete(daHookshot_c* i_this) {
    WWHD_FUNC(0x02177168, BOOL, i_this);
    i_this->hookshot_delete();
    return TRUE;
}
VERIFY(0x02177168, daHookshot_Delete);

/* 02177C00 */
BOOL daHookshot_c::procWait_init(BOOL playSe) {
    WWHD_FUNC(0x02177C00, BOOL, this, playSe);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    fopAcM_SetParam(this, Mode_Wait);
    setProc(PROC_WAIT);
    mChainCnt = 0;
    cXyz* r = link_hookshotRootPos(link);
    f32 x = r->x, y = r->y, z = r->z;
    current.pos.x = x;
    current.pos.y = y;
    current.pos.z = z;
    mObjHookFlg = FALSE;
    mCarryActorID = 0xFFFFFFFFu;
    if (playSe)
        mDoAud_seStart(JA_SE_LK_HS_WIND_UP_FIN, &link->current.pos, 0, dComIfGp_getReverb(current.roomNo));
    return TRUE;
}
VERIFY(0x02177C00, &daHookshot_c::procWait_init);

/* 02177CB8: HD: the actor gains a stage-specific setup in "GanonK" */
cPhs_State daHookshot_c::create() {
    WWHD_FUNC(0x02177CB8, cPhs_State, this);
    u32 b = gabi::ea(this);
    /* fopAcM_ct(this, daHookshot_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (b != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = HOOKSHOT_VTBL;
            gabi::call(0x0217718C, mShape); /* daHookshot_shape::daHookshot_shape (HD) */
            gabi::call(0x02080404, mHdD44C);
            /* dBgS_RopeLinChk */
            static const dBgS_LinChk_vt LINCHK_VT = {0x10011668, 0x10011678, 0x10011698, 0x10011688};
            dBgS_LinChk_ct(mLinChk, LINCHK_VT, false);
            gabi::store<u8>(b + 0xD534 + 0x62, 1); /* rope */
            /* dBgS_ObjGndChk */
            static const dBgS_GndChk_vt GNDCHK_VT = {0x100115E8, 0x100115F8, 0x10011618, 0x10011608};
            dBgS_GndChk_ct(mGndChk, GNDCHK_VT, true);
            dCcD_Stts_ct(&mStts);
            /* dCcD_Cps */
            u32 c = sightCps();
            gabi::call(0x02515FB8, c);                 /* dCcD_GObjInf::dCcD_GObjInf */
            gabi::store<u32>(c + 0x114, 0x100015A8);   /* cCcD_ShapeAttr */
            gabi::store<u32>(c + 0x110, 0x10011578);   /* cM3dGAab (this TU) */
            gabi::call(0x02018150, c + 0x118);         /* cM3dGCps::cM3dGCps */
            gabi::store<u32>(c + 0x3C, 0x1004AF18);
            gabi::store<u32>(c + 0x130, 0x1004AF60);
            gabi::store<u32>(c + 0x114, 0x1004AF70);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    gabi::store<u32>(b + 0x3AC + 0x18, b); /* mShape.setUserArea(this) */
    u32 lf = b + 0xD534 + 0x4C;
    gabi::store<u32>(lf, gabi::load<u32>(lf) & ~0x20000000u); /* mLinChk.ClrSttsRoofOff() */
    procWait_init(FALSE);
    gravity = -5.0f;
    mStts.Init(10, 0xFF, this);
    gabi::call(0x025164C0, sightCps(), l_at_cps_src); /* mSightCps.Set(l_at_cps_src) */
    gabi::store<u32>(sightCps() + 0x44, gabi::ea(&mStts));
    u8 roomNo = dComIfGp_roomControl_getStayNo();
    gabi::store<u8>(b + 0x1C9, roomNo);
    gabi::store<u8>(b + 0xD5F4 + 0x22, roomNo);
    gabi::store<u8>(b + 0x326, roomNo);
    if (dComIfGp_isStartStage(0x10011714 /* "GanonK" */))
        gabi::call(0x0207FD38, mHdD44C, 0);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02177CB8, &daHookshot_c::create);

/* 02177FD0 */
static cPhs_State daHookshot_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02177FD0, cPhs_State, i_actor);
    return reinterpret_cast<daHookshot_c*>(i_actor)->create();
}
VERIFY(0x02177FD0, daHookshot_Create);

// Preserve the recompiled lfs/stfs round-trip bits, including signaling NaNs.
static f32 copiedFloat(u32 bits) {f32 value;memcpy(&value,&bits,4);return value;}

/* 021789B4 */
BOOL daHookshot_c::procWait() {
    WWHD_FUNC(0x021789B4, BOOL, this);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    cXyz* r = link_hookshotRootPos(link);
    f32 rz = r->z, rx = r->x, ry = r->y;
    current.pos.x = rx;
    current.pos.z = rz;
    current.pos.y = ry;
    /* daPy_lk_c::getModelJointMtx(8) (virtual +0xFC): cl_LhandA */
    Mtx34* leftHandMtx = gabi::call_ptr<Mtx34*>(gabi::load<u32>(link->__vtbl + 0xFC), link, 8);
    PSMTXCopy(leftHandMtx, &mMtx);
    u32 m = gabi::ea(&mMtx);
    gabi::store<f32>(m + 0x0C, copiedFloat(gabi::load<u32>(gabi::ea(this) + 0x314)));
    gabi::store<f32>(m + 0x1C, copiedFloat(gabi::load<u32>(gabi::ea(this) + 0x318)));
    gabi::store<f32>(m + 0x2C, copiedFloat(gabi::load<u32>(gabi::ea(this) + 0x31C)));
    mObjHookFlg = FALSE;
    u32 c = sightCps();

    if (fopAcM_GetParam(this) == Mode_Shot) {
        s16 angleY = (s16)(link_bodyAngleY(link) + link->shape_angle.y);
        u16 angleX = link_bodyAngleX(link);
        mMoveVec.x = cM_ssin(angleY) * cM_scos(angleX);
        mMoveVec.y = -cM_ssin(angleX);
        mMoveVec.z = cM_scos(angleY) * cM_scos(angleX);
        gabi::call(0x02516138, c); /* mSightCps.ResetAtHit() */
        gabi::store<u32>(c, gabi::load<u32>(c) & ~0x10u); /* OffAtNoTgHitInfSet */
        gabi::store<u32>(c + 0x58, 0);                   /* SetAtHitCallback(NULL) */

        HookshotLocal<cXyz> mv;
        cXyz_ml(&mMoveVec, mv, 60.0f);
        HookshotLocal<cXyz> r1_3C;
        cXyz_mi(&current.pos, r1_3C, mv);
        dBgS_LinChk_Set(mLinChk, r1_3C, &current.pos, this);
        m2A0 = true;
        if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
            if (dBgS_ChkPolyHSStick(dComIfG_Bgsp(), mLinChk + 0x14)) {
                fopAcM_SetParam(this, Mode_Pull);
                mShipRideFlg = false;
                setProc(PROC_PULL);
                dBgS* bgs = dComIfG_Bgsp();
                cXyz* triPla = (cXyz*)cBgS_GetTriPla(bgs, gabi::load<u16>(gabi::ea(mLinChk) + 0x16), gabi::load<u16>(gabi::ea(mLinChk) + 0x14));
                if (triPla != nullptr) {
                    HookshotLocal<cXyz> xz;
                    xz->z = triPla->z;
                    xz->x = triPla->x;
                    xz->y = 0.0f;
                    f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
                    mHookAngle.x = cM_atan2s(triPla->y, absXZ);
                    mHookAngle.y = cM_atan2s(-triPla->x, -triPla->z);
                    mHookAngle.z = 0;
                } else { /* HD: null-checked plane */
                    mHookAngle.x = 0;
                    mHookAngle.y = 0;
                    mHookAngle.z = 0;
                }
                mMoveVec.copy(*cXyz_Zero);
            } else {
                fopAcM_SetParam(this, Mode_Return);
                setProc(PROC_RETURN);
            }
        } else {
            PSVECScale(&mMoveVec, &mMoveVec, 7.0f);
            cps_SetStartEnd(c, r1_3C, &current.pos, 5.0f);
            cps_CalcAtVec(c);
            dComIfG_Ccsp_Set(gabi::at<void>(c));
            dComIfG_Ccsp_SetMass(c, 1);
            m2A3 = false;
            setProc(PROC_SHOT);
        }
    } else {
        gabi::store<u32>(c, gabi::load<u32>(c) | 0x10u); /* OnAtNoTgHitInfSet */
        u16 sy = link->shape_angle.y;
        u16 bx = link_bodyAngleX(link);
        f32 py = current.pos.y;
        f32 px = current.pos.x;
        f32 pz = current.pos.z;
        f32 ssy = cM_ssin(sy), csy = cM_scos(sy);
        f32 sbx = cM_ssin(bx), cbx = cM_scos(bx);
        mObjSightCrossPos.x = gabi::fmadds(1500.0f * ssy, cbx, px);
        mObjSightCrossPos.y = gabi::fnmsubs(1500.0f, sbx, py);
        mObjSightCrossPos.z = gabi::fmadds(1500.0f * csy, cbx, pz);
        cps_SetStartEnd(c, &current.pos, &mObjSightCrossPos, 5.0f);
        cps_CalcAtVec(c);
        gabi::store<u32>(c + 0x58, ROCKLINE_CALLBACK); /* SetAtHitCallback */
        dComIfG_Ccsp_Set(gabi::at<void>(c));
    }
    return TRUE;
}
VERIFY(0x021789B4, &daHookshot_c::procWait);

/* 02178DDC */
BOOL daHookshot_c::procShot() {
    WWHD_FUNC(0x02178DDC, BOOL, this);
    if (fopAcM_GetParam(this) == Mode_Return) {
        setProc(PROC_RETURN);
        return TRUE;
    }
    u32 c = sightCps();
    if (gabi::call<BOOL>(0x025160DC, c) /* mSightCps.ChkAtHit() */) {
        fopAc_ac_c* hit_ac = gabi::call<fopAc_ac_c*>(0x02515BBC, c + 0x50); /* GetAtHitAc() */
        if (hit_ac != nullptr) {
            mCarryActorID = fopAcM_GetID(hit_ac);
            u32 st = hit_ac->actor_status;
            if (st & 0x00080000 /* fopAcStts_UNK80000_e */) {
                hit_ac->actor_status = st | 0x00100000; /* fopAcM_setHookCarryNow */
                HookshotLocal<cXyz> d;
                cXyz_mi(&hit_ac->current.pos, d, &current.pos);
                mCarryOffset.copy(*d);
                mDoAud_seStart(JA_SE_LK_HS_SPIKE, &current.pos, 0, dComIfGp_getReverb(current.roomNo));
                HookshotLocal<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, v);
                fopAcM_SetParam(this, Mode_Return);
                setProc(PROC_RETURN);
                return TRUE;
            } else if (st & 0x00200000 /* fopAcStts_UNK200000_e */) {
                current.pos.copy(*gabi::at<cXyz>(c + 0x70)); /* GetAtHitPosP() */
                HookshotLocal<cXyz> d;
                cXyz_mi(&current.pos, d, &hit_ac->current.pos);
                mCarryOffset.copy(*d);
                m2A4 = hit_ac->shape_angle.y;
                mHookAngle.x = shape_angle.x;
                mHookAngle.z = 0;
                mHookAngle.y = shape_angle.y;
                mMoveVec.copy(*cXyz_Zero);
                fopAcM_SetParam(this, Mode_Pull);
                mShipRideFlg = false;
                setProc(PROC_PULL);
                mDoAud_seStart(JA_SE_LK_HS_SPIKE, &current.pos, 0, dComIfGp_getReverb(current.roomNo));
                HookshotLocal<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, v);
                return TRUE;
            } else if (gabi::load<u32>(c + 0x54) & 1 /* mSightCps.ChkAtShieldHit() */) {
                mDoAud_seStart(JA_SE_LK_HS_REBOUND, &current.pos, 0x20, dComIfGp_getReverb(current.roomNo));
            }
        }
        fopAcM_SetParam(this, Mode_Return);
        setProc(PROC_RETURN);
        return TRUE;
    }

    /* m2A3 || current.pos.abs(root) >= 1500: m2A3 is read again after the distance */
    bool rebound = m2A3 != 0;
    bool stop = rebound;
    if (!rebound) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        HookshotLocal<cXyz> root;
        cXyz* r = link_hookshotRootPos(link);
        root->x = r->x;
        root->y = r->y;
        root->z = r->z;
        if (!(std_sqrtf(PSVECSquareDistance(&current.pos, root)) < 1500.0f)) {
            stop = true;
            rebound = m2A3 != 0;
        }
    }
    if (stop) {
        if (rebound) {
            u16 id = m2A6;
            dPa_control_c* pa = dComIfGp_getParticle();
            if (id == 0x825F /* dPa_name::ID_IT_SN_LK_FKSHOT_SUNA00 */)
                dPa_control_set(pa, 1, id, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            else
                dPa_control_set(pa, 1, id, &current.pos, &m2BA, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            u32 snd = mMtrlSndId;
            mDoAud_seStart(JA_SE_LK_HS_REBOUND, &current.pos, snd, dComIfGp_getReverb(current.roomNo));
        }
        fopAcM_SetParam(this, Mode_Return);
        setProc(PROC_RETURN);
        return TRUE;
    }

    HookshotLocal<cXyz> mv;
    cXyz_ml(&mMoveVec, mv, 15.0f);
    HookshotLocal<cXyz> sp8C;
    cXyz_pl(&current.pos, sp8C, mv);
    dBgS_LinChk_Set(mLinChk, &current.pos, sp8C, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
        sp8C->copy(*gabi::at<cXyz>(gabi::ea(mLinChk) + 0x30)); /* mLinChk.GetCross() */
        if (dBgS_ChkPolyHSStick(dComIfG_Bgsp(), mLinChk + 0x14)) {
            fopAcM_SetParam(this, Mode_Pull);
            mShipRideFlg = false;
            setProc(PROC_PULL);
            dBgS* bgs = dComIfG_Bgsp();
            cXyz* plane = (cXyz*)cBgS_GetTriPla(bgs, gabi::load<u16>(gabi::ea(mLinChk) + 0x16), gabi::load<u16>(gabi::ea(mLinChk) + 0x14));
            if (plane != nullptr) {
                HookshotLocal<cXyz> xz;
                xz->y = 0.0f;
                xz->z = plane->z;
                xz->x = plane->x;
                f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
                mHookAngle.x = cM_atan2s(plane->y, absXZ);
                mHookAngle.y = cM_atan2s(-plane->x, -plane->z);
                mHookAngle.z = 0;
            } else { /* HD: null-checked plane */
                mHookAngle.x = 0;
                mHookAngle.z = 0;
                mHookAngle.y = 0;
            }
            mMoveVec.copy(*cXyz_Zero);
            u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)(mLinChk + 0x14));
            mDoAud_seStart(JA_SE_LK_HS_SPIKE, &current.pos, snd, dComIfGp_getReverb(current.roomNo));
            HookshotLocal<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            dComIfGp_getVibration_StartShock(4, -0x21, v);
        } else {
            dBgS* bgs = dComIfG_Bgsp();
            void* plane = cBgS_GetTriPla(bgs, gabi::load<u16>(gabi::ea(mLinChk) + 0x16), gabi::load<u16>(gabi::ea(mLinChk) + 0x14));
            cM3d_CalcVecZAngle(plane, &m2BA);
            bool sand = false;
            /* HD: (y < 950 && (ground || sand attribute)) */
            if (dComIfGp_isStartStage(0x10011738 /* "kazeB" */) && sp8C->y < 950.0f) {
                bgs = dComIfG_Bgsp();
                cXyz* pla = (cXyz*)cBgS_GetTriPla(bgs, gabi::load<u16>(gabi::ea(mLinChk) + 0x16), gabi::load<u16>(gabi::ea(mLinChk) + 0x14));
                if ((pla != nullptr && !(pla->y < 0.5f)) /* cBgW_CheckBGround */ ||
                    dBgS_GetAttributeCode(dComIfG_Bgsp(), mLinChk + 0x14) == 0xB /* dBgS_Attr_SAND_e */)
                    sand = true;
            }
            m2A6 = sand ? 0x825F /* ID_IT_SN_LK_FKSHOT_SUNA00 */ : 0xC /* ID_AK_JN_NG */;
            mMtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)(mLinChk + 0x14));
            m2A3 = true;
        }
    }
    cps_SetStartEnd(c, &current.pos, sp8C, 5.0f);
    cps_CalcAtVec(c);
    dComIfG_Ccsp_Set(gabi::at<void>(c));
    dComIfG_Ccsp_SetMass(c, 1);
    current.pos.copy(*sp8C);
    return TRUE;
}
VERIFY(0x02178DDC, &daHookshot_c::procShot);

/* 02179888 */
BOOL daHookshot_c::procPlayerPull() {
    WWHD_FUNC(0x02179888, BOOL, this);
    fopAc_ac_c* carry_actor = nullptr;
    if (mCarryActorID != 0xFFFFFFFFu) {
        carry_actor = fopAcM_SearchByID(mCarryActorID);
        if (carry_actor == nullptr || fopAcM_GetParam(this) == Mode_Return) {
            setProc(PROC_RETURN);
            mCarryActorID = 0xFFFFFFFFu;
            return TRUE;
        }
        if (carry_actor->shape_angle.y != m2A4) {
            mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)(carry_actor->shape_angle.y - m2A4));
            HookshotLocal<cXyz> sp4C;
            PSMTXMultVec(mDoMtx_stack_c::get(), &mCarryOffset, sp4C);
            mCarryOffset.copy(*sp4C);
            m2A4 = carry_actor->shape_angle.y;
        }
        HookshotLocal<cXyz> p;
        cXyz_pl(&carry_actor->current.pos, p, &mCarryOffset);
        current.pos.copy(*p);
    } else {
        if (fopAcM_GetParam(this) == Mode_Return || !cBgS_ChkPolySafe(dComIfG_Bgsp(), mLinChk + 0x14)) {
            setProc(PROC_RETURN);
            return TRUE;
        }
        if (dBgS_ChkMoveBG(dComIfG_Bgsp(), mLinChk + 0x14))
            dBgS_MoveBgTransPos(dComIfG_Bgsp(), mLinChk + 0x14, true, &current.pos, &mHookAngle, nullptr);
    }

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    HookshotLocal<cXyz> root;
    cXyz* r = link_hookshotRootPos(link);
    root->x = r->x;
    root->y = r->y;
    root->z = r->z;
    HookshotLocal<cXyz> sp40;
    cXyz_mi(&current.pos, sp40, root);
    f32 f1 = std_sqrtf(PSVECSquareMag(sp40));
    s32 r5 = gabi::ftoi(f1 / 7.0f);
    if (r5 > 0) {
        s32 cnt = mChainCnt;
        r5 = (r5 > cnt ? s32(u32(r5) - u32(cnt)) : 0);
        s32 iVar6 = s32(u32(r5) + u32(cnt <= 9 ? cnt : 9));
        mChainCnt = s32(u32(cnt) - u32(iVar6));
        HookshotLocal<cXyz> mv;
        cXyz_ml(sp40, mv, (7.0f * (f32)iVar6) / f1);
        mMoveVec.copy(*mv);
        if (mChainCnt > 0)
            return TRUE;
    } else {
        mChainCnt = 0;
    }
    if (carry_actor != nullptr && gabi::ea(carry_actor) == gabi::load<u32>(dComIfGp_ea() + 0x5B3C) /* dComIfGp_getShipActor() */)
        mShipRideFlg = true;
    procWait_init(TRUE);
    return TRUE;
}
VERIFY(0x02179888, &daHookshot_c::procPlayerPull);

/* 02179B98 */
BOOL daHookshot_c::procReturn() {
    WWHD_FUNC(0x02179B98, BOOL, this);
    fopAc_ac_c* carry_actor;
    if (mCarryActorID != 0xFFFFFFFFu) {
        carry_actor = fopAcM_SearchByID(mCarryActorID);
        if (carry_actor == nullptr || !(carry_actor->actor_status & 0x00100000) /* fopAcM_checkHookCarryNow */) {
            mCarryActorID = 0xFFFFFFFFu;
            carry_actor = nullptr;
        }
    } else {
        carry_actor = nullptr;
    }

    bool done = mChainCnt <= 9;
    HookshotLocal<cXyz> sp4C;
    f32 f1 = 0.0f;
    if (!done) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        HookshotLocal<cXyz> root;
        cXyz* r = link_hookshotRootPos(link);
        root->x = r->x;
        root->y = r->y;
        root->z = r->z;
        cXyz_mi(&current.pos, sp4C, root);
        f1 = std_sqrtf(PSVECSquareMag(sp4C));
        done = gabi::ftoi(f1 / 7.0f) <= 9;
    }
    if (done) {
        if (carry_actor != nullptr)
            carry_actor->actor_status = carry_actor->actor_status & ~0x00100000u; /* fopAcM_cancelHookCarryNow */
        procWait_init(TRUE);
        procWait();
        return TRUE;
    }
    f32 s = 63.0f / f1;
    mChainCnt = mChainCnt - 9;
    HookshotLocal<cXyz> d;
    cXyz_ml(sp4C, d, s);
    PSVECSubtract(&current.pos, d, &current.pos);
    if (carry_actor != nullptr) {
        HookshotLocal<cXyz> p;
        cXyz_pl(&current.pos, p, &mCarryOffset);
        f32 x = p->x, y = p->y, z = p->z;
        carry_actor->current.pos.x = x;
        carry_actor->current.pos.y = y;
        carry_actor->current.pos.z = z;
        u32 gnd = gabi::ea(mGndChk);
        gabi::store<f32>(gnd + 0x24, x);
        gabi::store<f32>(gnd + 0x28, (y + 100.0f) + 100.0f);
        gabi::store<f32>(gnd + 0x2C, z);
        f32 ground_y = cBgS_GroundCross(dComIfG_Bgsp(), mGndChk);
        if (ground_y > carry_actor->current.pos.y) {
            carry_actor->current.pos.y = ground_y;
            mCarryOffset.y = ground_y - current.pos.y;
        }
    }
    return TRUE;
}
VERIFY(0x02179B98, &daHookshot_c::procReturn);

/* 02179D84 */
static void __sinit_d_a_hookshot_cpp() {
    WWHD_FUNC(0x02179D84, void, (u32)0);
    sinit_header_statics(0x10464744, 0x101B76FC);
    gabi::store<f32>(l_at_cps_src + 0x48, 5.0f); /* l_at_cps_src radius */
}
VERIFY(0x02179D84, __sinit_d_a_hookshot_cpp);

/* 02179E28: sead::SafeString deleting destructor (per-TU copy; trivial destructor) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02179E28, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02179E28, SafeString_dt);

/* 02179E3C: HD shape sub-object (0x254 bytes) constructor, for __construct_array */
static u32 shapeBuf_ct(u32 p) {
    WWHD_FUNC(0x02179E3C, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0x254));
        if (p == 0)
            return p;
    }
    gabi::call(0x027B5BD8, p + 4);
    gabi::call(0x027BF734, p + 0x158);
    gabi::store<u32>(p + 0x250, 0);
    gabi::store<u32>(p + 0x24C, 0);
    return p;
}
VERIFY(0x02179E3C, shapeBuf_ct);

/* 02179E98: HD 0x10-byte element constructor (trivial), for __construct_array */
static u32 elem10_ct(u32 p) {
    WWHD_FUNC(0x02179E98, u32, p);
    if (p == 0)
        p = gabi::ea(operator_new(0x10));
    return p;
}
VERIFY(0x02179E98, elem10_ct);

/* 02179EC4: HD chain link (0xA8 bytes) constructor, for __construct_array */
static u32 chainLink_ct(u32 p) {
    WWHD_FUNC(0x02179EC4, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0xA8));
        if (p == 0)
            return p;
    }
    gabi::call(0x027FB40C, p);
    gabi::store<u32>(p + 0xC, 0x1016EF84);
    gabi::call(0x028F521C, p + 0x74, 0x34);
    if (p + 0x74 == 0) /* inline placement new of a member */
        operator_new(0x30);
    return p;
}
VERIFY(0x02179EC4, chainLink_ct);

/* 02179F34: HD chain link destructor */
static void chainLink_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x02179F34, void, p, flags);
    if (p != 0) {
        gabi::call(0x027FB528, p, 0);
        if (flags & 1)
            operator_delete(gabi::at<void>(p));
    }
}
VERIFY(0x02179F34, chainLink_dt);

/* 02179F88: HD shape sub-object (0x254 bytes) destructor */
static void shapeBuf_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x02179F88, void, p, flags);
    if (p != 0) {
        gabi::call(0x027BF880, p + 0x158, 2);
        gabi::call(0x027B5CBC, p + 4, 2);
        if (flags & 1)
            operator_delete(gabi::at<void>(p));
    }
}
VERIFY(0x02179F88, shapeBuf_dt);

/* 02179FE8: empty virtual of daHookshot_shape (per-TU copy) */
static void empty_02179FE8(void* p) {
    WWHD_FUNC(0x02179FE8, void, p);
}
VERIFY(0x02179FE8, empty_02179FE8);

/* 02179FEC: daHookshot_c deleting destructor (inline member destructors) */
static void daHookshot_c_dt(daHookshot_c* i_this, s32 flags) {
    WWHD_FUNC(0x02179FEC, void, i_this, flags);
    if (i_this != nullptr) {
        u32 b = gabi::ea(i_this);
        gabi::call(0x02515980, b + 0xD630, 2); /* dCcD_GObjInf::~dCcD_GObjInf (mSightCps) */
        dCcD_Stts_dt(&i_this->mStts, 2);
        /* ~dBgS_ObjGndChk: this TU's vtables, then ~cBgS_Chk */
        u32 g = b + 0xD44C;
        gabi::store<u32>(g + 0x174, 0x100115B8);
        gabi::store<u32>(g + 0x194, 0x100115D8);
        gabi::store<u32>(g + 0x1A0, 0x10011598);
        gabi::call(0x02008DAC, g + 0x154, 0);
        /* ~dBgS_RopeLinChk */
        gabi::store<u32>(g + 0x14C, 0x10011598);
        gabi::store<u32>(g + 0x108, 0x10011588);
        gabi::store<u32>(g + 0x140, 0x10011658);
        gabi::call(0x02008B4C, g + 0xE8, 0);
        gabi::call(0x02082DDC, g, 2);           /* HD object at 0xD44C */
        gabi::call(0x02177FD4, i_this->mShape, 2); /* daHookshot_shape::~daHookshot_shape (HD) */
        gabi::call(0x025D50BC, i_this, 0);      /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02179FEC, daHookshot_c_dt);

/* 0217A0F0: sead::SafeString::assureTerminationImpl_ (per-TU copy; empty) */
static void SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x0217A0F0, void, p);
}
VERIFY(0x0217A0F0, SafeString_assureTermination);
