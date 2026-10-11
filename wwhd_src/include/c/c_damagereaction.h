// Damage reactions: GameCube names with the verified WWHD layout.
// HD: 21 joint entries and reaction-tail offsets shifted by 0x34; fire light has a four-byte HD
// tail.
#pragma once
#include "bindings.h"
using gabi::load;
using gabi::store;
static void line_init(u32 p);
static void line_destroy(u32 p);

struct enemyice {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_OFFSET(enemyice, mCylHeight, 0x19C);
WWHD_SIZE(enemyice, 0x3B8);

/* enemyfire (c_damagereaction), HD 0x22C */
struct enemyfire {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ LIGHT_INFLUENCE mLight;
};
WWHD_OFFSET(enemyfire, mSph, 0xDC);
WWHD_SIZE(enemyfire, 0x22C);

struct DamageAngle : be<s16> {
    template <class T> DamageAngle& operator=(const T& x) {
        if constexpr (std::is_floating_point_v<T>)
            set((s16)gabi::ftoi(x));
        else
            set((s16)x);
        return *this;
    }
    DamageAngle& operator=(const DamageAngle& x) {
        set(x.get());
        return *this;
    }
    template <class T> DamageAngle& operator+=(const T& x) { return *this = get() + x; }
    template <class T> DamageAngle& operator-=(const T& x) { return *this = get() - x; }
};
struct DamageAngles {
    DamageAngle x, y, z;
};
static_assert(sizeof(DamageAngles) == 6);
struct DamageSmoke {
    u8 data[0x20];
    void end() { gabi::call(0x025A5F88, this); }
};
struct DamageStts {
    u8 data[0x3C];
    cXyz* GetCCMoveP() { return gabi::at<cXyz>(gabi::ea(this)); }
};
struct DamageLine : dBgS_LinChk {
    void Init() { line_init(gabi::ea(this)); }
    void End() { line_destroy(gabi::ea(this)); }
    void Set(cXyz* a, cXyz* b, fopAc_ac_c* actor) { gabi::call(0x024F1AFC, this, a, b, actor); }
    cXyz& GetCross() { return mLin.mEnd; }
};
struct DamageGround : dBgS_GndChk {
    void Init(bool spl) {
        u32 a = gabi::ea(this);
        gabi::call(0x02008E0C, this);
        mbObjThrough = 1;
        mbCamThrough = 0;
        mbLinkThrough = 0;
        mbArrowThrough = 0;
        mbBombThrough = 0;
        mbBoomerangThrough = 0;
        mbRopeThrough = 0;
        dBgS_GrpPassChk::mGrp = spl ? 14 : 4;
        cBgS_Chk::mPolyPassChk = a + 0x40;
        cBgS_Chk::mGrpPassChk = a + 0x4C;
        cBgS_Chk::__vtbl = spl ? 0x100069D4 : 0x10006994;
        cBgS_PolyInfo::__vtbl = spl ? 0x100069E4 : 0x100069A4;
        dBgS_PolyPassChk::__vtbl = spl ? 0x10006A04 : 0x100069C4;
        dBgS_GrpPassChk::__vtbl = spl ? 0x100069F4 : 0x100069B4;
    }
    void SetPos(const cXyz* v) { m_pos.set(v->x, v->y, v->z); }
    void End() {
        cBgS_PolyInfo::__vtbl = 0x10006924;
        dBgS_PolyPassChk::__vtbl = 0x10006944;
        dBgS_GrpPassChk::__vtbl = 0x10006904;
        gabi::call(0x02008DAC, this, 0);
    }
};
static_assert(sizeof(DamageLine) == 0x6C && sizeof(DamageGround) == 0x54);
struct damagereaction {
    enum { TYPE_MOBLIN = 1, TYPE_BOKOBLIN = 2, TYPE_DARKNUT = 3 };
    gptr<fopAc_ac_c> mpEnemy;
    be<s16> mMode, mAction, mEnemyType;
    u8 padA[2];
    be<s32> mTimer;
    DamageAngles m010[21], m088[21];
    cXyz m100[21], m1F0[21], m2E0[21];
    /*400*/ be<s32> m3D0[14];
    /*438*/ be<s32> m408;
    /*43C*/ be<s32> m40C;
    /*440*/ be<s32> m410;
    /*444*/ be<s32> m414;
    /*448*/ be<s32> m418;
    u8 pad44C[0x8];
    /*454*/ be<s32> m420;
    /*458*/ be<s32> m424;
    /*45C*/ be<f32> m428;
    /*460*/ cXyz m42C;
    /*46C*/ be<s32> m438;
    u8 pad470[0x4];
    /*474*/ be<s16> m440;
    /*476*/ be<s16> m442;
    /*478*/ be<s16> m444;
    /*47A*/ be<s16> m446;
    /*47C*/ be<s16> m448;
    u8 pad47E[0x2];
    /*480*/ cXyz m44C;
    /*48C*/ cXyz m458;
    u8 pad498[0x4];
    /*49C*/ be<f32> m468;
    /*4A0*/ be<f32> m46C;
    /*4A4*/ be<f32> m470;
    /*4A8*/ be<f32> m474;
    /*4AC*/ be<f32> m478;
    /*4B0*/ be<s32> m47C;
    /*4B4*/ be<s16> m480;
    /*4B6*/ be<s16> m482;
    /*4B8*/ be<s16> m484;
    /*4BA*/ be<s16> m486;
    /*4BC*/ be<s16> m488;
    /*4BE*/ be<s16> m48A;
    /*4C0*/ csXyz m48C;
    /*4C6*/ be<s16> m492;
    /*4C8*/ be<s16> m494;
    /*4CA*/ be<s16> m496;
    /*4CC*/ be<s16> m498;
    /*4CE*/ be<s16> m49A;
    /*4D0*/ be<s16> m49C;
    /*4D2*/ be<s16> m49E;
    /*4D4*/ DamageAngle m4A0;
    /*4D6*/ DamageAngle m4A2;
    /*4D8*/ DamageAngle m4A4;
    /*4DA*/ DamageAngle m4A6;
    /*4DC*/ DamageAngle m4A8;
    /*4DE*/ DamageAngle m4AA;
    /*4E0*/ DamageAngle m4AC;
    /*4E2*/ DamageAngle m4AE;
    /*4E4*/ DamageAngle m4B0;
    /*4E6*/ DamageAngle m4B2;
    /*4E8*/ DamageAngle m4B4;
    /*4EA*/ DamageAngle m4B6;
    /*4EC*/ DamageAngle m4B8;
    /*4EE*/ DamageAngle m4BA;
    /*4F0*/ DamageAngle m4BC;
    /*4F2*/ DamageAngle m4BE;
    /*4F4*/ be<s16> m4C0;
    u8 pad4F6[0x6];
    /*4FC*/ be<s16> m4C8[3];
    /*502*/ be<s16> mInvincibleTimer;
    /*504*/ be<s16> m4D0;
    u8 pad506[0x2];
    /*508*/ be<f32> m4D4;
    u8 pad50C[4];
    dBgS_AcchCir mAcchCir;
    dBgS_ObjAcch mAcch;
    /*714*/ be<s32> m6E0;
    /*718*/ be<f32> mSpawnY;
    /*71C*/ cXyz m6E8;
    /*728*/ cXyz m6F4[2];
    /*740*/ be<u8> m70C;
    /*741*/ be<u8> m70D;
    /*742*/ be<u8> m70E;
    u8 pad743[0x1];
    /*744*/ be<u8> m710;
    /*745*/ be<u8> m711;
    /*746*/ be<u8> m712;
    /*747*/ be<u8> m713;
    /*748*/ gptr<fopAc_ac_c> m714;
    /*74C*/ be<s16> m718;
    /*74E*/ be<s16> m71A;
    u8 pad750[0x2];
    /*752*/ be<s16> m71E;
    /*754*/ be<f32> mMaxFallDistance;
    DamageStts mStts;
    /*794*/ cXyz mParticlePos;
    /*7A0*/ csXyz mParticleAngle;
    u8 pad7A6[2];
    DamageSmoke mParticleCallBack;
    /*7C8*/ be<s16> m794;
    u8 pad7CA[0x2];
    /*7CC*/ be<f32> m798;
    /*7D0*/ gptr<cXyz> m79C;
    /*7D4*/ cXyz m7A0;
    /*7E0*/ csXyz m7AC;
    /*7E6*/ be<s16> m7B2;
    /*7E8*/ be<s16> m7B4;
    /*7EA*/ be<u8> m7B6;
    u8 pad7EB[0x1];
    /*7EC*/ be<u32> m7B8;
};
static_assert(offsetof(damagereaction, m47C) == 0x4B0);
static_assert(offsetof(damagereaction, m3D0) == 0x400);
static_assert(offsetof(damagereaction, mAcch) == 0x550);
static_assert(offsetof(damagereaction, mSpawnY) == 0x718);

WWHD_OFFSET(damagereaction, mpEnemy, 0x0);
WWHD_OFFSET(damagereaction, mMode, 0x4);
WWHD_OFFSET(damagereaction, mAction, 0x6);
WWHD_OFFSET(damagereaction, mEnemyType, 0x8);
WWHD_OFFSET(damagereaction, mTimer, 0xC);
WWHD_OFFSET(damagereaction, m010, 0x10);
WWHD_OFFSET(damagereaction, m088, 0x8E);
WWHD_OFFSET(damagereaction, m100, 0x10C);
WWHD_OFFSET(damagereaction, m1F0, 0x208);
WWHD_OFFSET(damagereaction, m2E0, 0x304);
WWHD_OFFSET(damagereaction, mAcchCir, 0x510);
WWHD_OFFSET(damagereaction, mAcch, 0x550);
WWHD_OFFSET(damagereaction, mStts, 0x758);
WWHD_OFFSET(damagereaction, mParticleCallBack, 0x7A8);
WWHD_OFFSET(damagereaction, m3D0, 0x400);
WWHD_OFFSET(damagereaction, m408, 0x438);
WWHD_OFFSET(damagereaction, m40C, 0x43C);
WWHD_OFFSET(damagereaction, m410, 0x440);
WWHD_OFFSET(damagereaction, m414, 0x444);
WWHD_OFFSET(damagereaction, m418, 0x448);
WWHD_OFFSET(damagereaction, m420, 0x454);
WWHD_OFFSET(damagereaction, m424, 0x458);
WWHD_OFFSET(damagereaction, m428, 0x45C);
WWHD_OFFSET(damagereaction, m42C, 0x460);
WWHD_OFFSET(damagereaction, m438, 0x46C);
WWHD_OFFSET(damagereaction, m440, 0x474);
WWHD_OFFSET(damagereaction, m442, 0x476);
WWHD_OFFSET(damagereaction, m444, 0x478);
WWHD_OFFSET(damagereaction, m446, 0x47A);
WWHD_OFFSET(damagereaction, m448, 0x47C);
WWHD_OFFSET(damagereaction, m44C, 0x480);
WWHD_OFFSET(damagereaction, m458, 0x48C);
WWHD_OFFSET(damagereaction, m468, 0x49C);
WWHD_OFFSET(damagereaction, m46C, 0x4A0);
WWHD_OFFSET(damagereaction, m470, 0x4A4);
WWHD_OFFSET(damagereaction, m474, 0x4A8);
WWHD_OFFSET(damagereaction, m478, 0x4AC);
WWHD_OFFSET(damagereaction, m47C, 0x4B0);
WWHD_OFFSET(damagereaction, m480, 0x4B4);
WWHD_OFFSET(damagereaction, m482, 0x4B6);
WWHD_OFFSET(damagereaction, m484, 0x4B8);
WWHD_OFFSET(damagereaction, m486, 0x4BA);
WWHD_OFFSET(damagereaction, m488, 0x4BC);
WWHD_OFFSET(damagereaction, m48A, 0x4BE);
WWHD_OFFSET(damagereaction, m48C, 0x4C0);
WWHD_OFFSET(damagereaction, m492, 0x4C6);
WWHD_OFFSET(damagereaction, m494, 0x4C8);
WWHD_OFFSET(damagereaction, m496, 0x4CA);
WWHD_OFFSET(damagereaction, m498, 0x4CC);
WWHD_OFFSET(damagereaction, m49A, 0x4CE);
WWHD_OFFSET(damagereaction, m49C, 0x4D0);
WWHD_OFFSET(damagereaction, m49E, 0x4D2);
WWHD_OFFSET(damagereaction, m4A0, 0x4D4);
WWHD_OFFSET(damagereaction, m4A2, 0x4D6);
WWHD_OFFSET(damagereaction, m4A4, 0x4D8);
WWHD_OFFSET(damagereaction, m4A6, 0x4DA);
WWHD_OFFSET(damagereaction, m4A8, 0x4DC);
WWHD_OFFSET(damagereaction, m4AA, 0x4DE);
WWHD_OFFSET(damagereaction, m4AC, 0x4E0);
WWHD_OFFSET(damagereaction, m4AE, 0x4E2);
WWHD_OFFSET(damagereaction, m4B0, 0x4E4);
WWHD_OFFSET(damagereaction, m4B2, 0x4E6);
WWHD_OFFSET(damagereaction, m4B4, 0x4E8);
WWHD_OFFSET(damagereaction, m4B6, 0x4EA);
WWHD_OFFSET(damagereaction, m4B8, 0x4EC);
WWHD_OFFSET(damagereaction, m4BA, 0x4EE);
WWHD_OFFSET(damagereaction, m4BC, 0x4F0);
WWHD_OFFSET(damagereaction, m4BE, 0x4F2);
WWHD_OFFSET(damagereaction, m4C0, 0x4F4);
WWHD_OFFSET(damagereaction, m4C8, 0x4FC);
WWHD_OFFSET(damagereaction, mInvincibleTimer, 0x502);
WWHD_OFFSET(damagereaction, m4D0, 0x504);
WWHD_OFFSET(damagereaction, m4D4, 0x508);
WWHD_OFFSET(damagereaction, m6E0, 0x714);
WWHD_OFFSET(damagereaction, mSpawnY, 0x718);
WWHD_OFFSET(damagereaction, m6E8, 0x71C);
WWHD_OFFSET(damagereaction, m6F4, 0x728);
WWHD_OFFSET(damagereaction, m70C, 0x740);
WWHD_OFFSET(damagereaction, m70D, 0x741);
WWHD_OFFSET(damagereaction, m70E, 0x742);
WWHD_OFFSET(damagereaction, m710, 0x744);
WWHD_OFFSET(damagereaction, m711, 0x745);
WWHD_OFFSET(damagereaction, m712, 0x746);
WWHD_OFFSET(damagereaction, m713, 0x747);
WWHD_OFFSET(damagereaction, m714, 0x748);
WWHD_OFFSET(damagereaction, m718, 0x74C);
WWHD_OFFSET(damagereaction, m71A, 0x74E);
WWHD_OFFSET(damagereaction, m71E, 0x752);
WWHD_OFFSET(damagereaction, mMaxFallDistance, 0x754);
WWHD_OFFSET(damagereaction, mParticlePos, 0x794);
WWHD_OFFSET(damagereaction, mParticleAngle, 0x7A0);
WWHD_OFFSET(damagereaction, m794, 0x7C8);
WWHD_OFFSET(damagereaction, m798, 0x7CC);
WWHD_OFFSET(damagereaction, m79C, 0x7D0);
WWHD_OFFSET(damagereaction, m7A0, 0x7D4);
WWHD_OFFSET(damagereaction, m7AC, 0x7E0);
WWHD_OFFSET(damagereaction, m7B2, 0x7E6);
WWHD_OFFSET(damagereaction, m7B4, 0x7E8);
WWHD_OFFSET(damagereaction, m7B6, 0x7EA);
WWHD_OFFSET(damagereaction, m7B8, 0x7EC);
WWHD_SIZE(damagereaction, 0x7F0);

// HD: word-copy views preserve the original lfs/stfs bit transport.
template <class T> inline T* damageView(u32 address) { return gabi::at<T>(address); }
template <class T> inline be<u32>& damageWord(T& field) {
    return *gabi::at<be<u32>>(gabi::ea(&field));
}

template <class T, class U> inline T* damageView(U* address) {
    return reinterpret_cast<T*>(address);
}

// HD: cCcS is opaque in the shared layout; this is its verified Set call.
inline void damageSetCollision(cCcS* cc, dCcD_GObjInf* object) {
    gabi::call(0x0200E240, cc, object);
}

// HD: emitter globals follow the expanded particle lists; group ID is at 0x262.
struct DamageEmitter : JPABaseEmitter {
    /* 0x070 */ u8 _070[0x220 - 0x70];
    /* 0x220 */ cXyz mGlobalDynamicsScale;
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ cXyz mGlobalParticleScale;
    /* 0x244 */ u8 _244[0x254 - 0x244];
    /* 0x254 */ be<u32> mFlags;
    /* 0x258 */ u8 _258[0x262 - 0x258];
    /* 0x262 */ be<u8> mGroupID;
    /* 0x263 */ u8 _263;
};
WWHD_OFFSET(DamageEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(DamageEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(DamageEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(DamageEmitter, mFlags, 0x254);
WWHD_OFFSET(DamageEmitter, mGroupID, 0x262);
WWHD_SIZE(DamageEmitter, 0x264);

// HD: per-translation-unit header statics emitted by the compiler.
struct DamageHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mMathState, mRandomState;
    u8 _00A[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(DamageHeaderStatics, mZero, 0xC);
WWHD_SIZE(DamageHeaderStatics, 0x1C);

// Narrow actor views used by the shared damage-reaction code.
struct DamageBokoblin : fopAc_ac_c {
    u8 _3AC[0x3D4 - 0x3AC];
    be<u8> m02B8;
};
struct DamageDarknut : fopAc_ac_c {
    u8 _3AC[0x3EC - 0x3AC];
    be<s8> mRemainingEquipmentPieces;
};
WWHD_OFFSET(DamageBokoblin, m02B8, 0x3D4);
WWHD_SIZE(DamageBokoblin, 0x3D5);
WWHD_OFFSET(DamageDarknut, mRemainingEquipmentPieces, 0x3EC);
WWHD_SIZE(DamageDarknut, 0x3ED);

// HD: these call sites retain double results or full-width integer registers.
inline f64 damageRndF(f32 limit) { return gabi::call<f64>(0x020198D8, limit); }
inline f64 damageRndFX(f32 limit) { return gabi::call<f64>(0x02019918, limit); }
inline f64 damageSqrt(f32 value) { return gabi::call<f64>(0x028F4384, value); }
inline void damageYrotS(Mtx34* matrix, s32 angle) { gabi::call(0x025F1884, matrix, angle); }
inline s32 damageAtan2s(f32 y, f32 x) { return gabi::call<s32>(0x020195B0, y, x); }

// HD: the compiler emits a draw-list object with nine eight-byte entries.
struct DamageDrawList {
    be<u32> __vtbl;
    be<u32> mRandomState[3];
    u8 mEntries[9][8];
    be<f32> mValue;
};
WWHD_OFFSET(DamageDrawList, mEntries, 0x10);
WWHD_OFFSET(DamageDrawList, mValue, 0x58);
WWHD_SIZE(DamageDrawList, 0x5C);
