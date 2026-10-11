#pragma once
#include "gc_api/gc_api.h"
struct FireEmitterHD;
class daFire_c : public fopAc_ac_c {
  public:
    bool _delete();
    // HD: verified return width is bool on CreateInit, signed32 on _create.
    bool CreateInit();
    s32 _create();
    bool _execute();
    void search_wind();
    void ctrlEffect();
    void demo_proc();
    void normal_proc();
    void eventOrder();
    void checkOrder();
    void execStopFire();
    void execStopNowFire();
    void execPlayFire();
    void setDirParticle(cXyz *);
    void checkCol();
    bool _draw();
    /* 0x3AC */ dCcD_Stts mStts;
    /* 0x3E8 */ dCcD_Cyl field_0x2CC[3];
    /* 0x778 */ dCcD_Cyl field_0x65C;
    /* 0x8A8 */ dCcD_Cyl field_0x78C;
    /* 0x9D8 */ gptr<FireEmitterHD> field_0x8BC, field_0x8C0, field_0x8C4;
    /* 0x9E4 */ be<s32> mSwitchNo;
    /* 0x9E8 */ be<u32> mTrboxBit;
    /* 0x9EC */ be<u8> mShape, field_0x8D1, field_0x8D2;
    /* 0x9EF */ u8 pad9EF;
    /* 0x9F0 */ cXyz field_0x8D4, field_0x8E0;
    /* 0xA08 */ be<u8> field_0x8EC, field_0x8ED;
    /* 0xA0A */ u8 field_0x8EE[2];
    /* 0xA0C */ be<u32> field_0x8F0;
    /* 0xA10 */ be<s32> field_0x8F4, field_0x8F8;
    /* 0xA18 */ be<s16> field_0x8FC, field_0x8FE;
    /* 0xA1C */ be<u8> mType;
    /* 0xA1D */ be<s8> field_0x901;
    /* 0xA1E */ be<u8> field_0x902, field_0x903;
    // HD: indexed access to the three separately named tww emitter fields.
    gptr<FireEmitterHD> &emitter(u32 i) {
        if (i == 0)
            return field_0x8BC;
        if (i == 1)
            return field_0x8C0;
        return field_0x8C4;
    }
};
WWHD_OFFSET(daFire_c, field_0x2CC, 0x3E8);
WWHD_OFFSET(daFire_c, field_0x65C, 0x778);
WWHD_OFFSET(daFire_c, field_0x78C, 0x8A8);
WWHD_OFFSET(daFire_c, field_0x8BC, 0x9D8);
WWHD_OFFSET(daFire_c, field_0x8D4, 0x9F0);
WWHD_OFFSET(daFire_c, field_0x8F0, 0xA0C);
WWHD_OFFSET(daFire_c, mType, 0xA1C);
WWHD_SIZE(daFire_c, 0xA20);
// HD: partial emitter fields used by the fire actor.
struct FireEmitterHD {
    u8 pad0[0x28];
    cXyz mDirection;
    u8 pad34[0x5C - 0x34];
    be<s32> mMaxFrame;
    u8 pad60[0x1B4 - 0x60];
    be<u32> mAliveParticles;
    u8 pad1B8[8];
    be<u32> mAliveChildren;
    u8 pad1C4[0x220 - 0x1C4];
    cXyz mGlobalScale, mGlobalTranslation, mGlobalParticleScale;
    u8 pad244[0x254 - 0x244];
    be<u32> mFlags;
};
WWHD_OFFSET(FireEmitterHD, mGlobalScale, 0x220);
WWHD_OFFSET(FireEmitterHD, mGlobalParticleScale, 0x238);
WWHD_OFFSET(FireEmitterHD, mFlags, 0x254);
WWHD_SIZE(FireEmitterHD, 0x258);
// HD: the cylinder constructor's shape vtables straddle the inherited padding.
struct FireCylinderVtableHD {
    u8 pad0[0x110];
    be<u32> mShapeBaseVtable, mShapeVtable;
};
WWHD_OFFSET(FireCylinderVtableHD, mShapeVtable, 0x114);
inline FireCylinderVtableHD *fireCylinderVtables(dCcD_Cyl *c) {
    return reinterpret_cast<FireCylinderVtableHD *>(c);
}
struct FirePlayHD {
    u8 pad0[0x52B8];
    be<u16> mEventFlags;
    u8 pad52BA[0x5AB0 - 0x52BA];
    gptr<u8> mpParticle;
};
WWHD_OFFSET(FirePlayHD, mpParticle, 0x5AB0);

void fireDeleteAllParticles(u32, s32);
u32 fireActorSearch(u32, u32);
void fireChaseF(be<f32> *, f32, f32);
u32 fireIsTbox(u32, u32);
void fireSttsInit(u32, s32, s32, u32);
void fireCylinderSet(u32, u32);
void fireCylinderRadius(u32, f32);
void fireOffDraw(u32);
s32 fireCheckTargetHit(u32);
u32 fireGetTargetHitObj(u32);
u32 fireGetTargetHitActor(u32);
void fireClearTargetHit(u32);
u32 fireParticleSet(u32, u32, u32, u32, u32, u32, u32, u32, s32, u32, u32, u32);
f32 fireSquareRoot(f32);
s32 fireNormalizeRS(cXyz *);
s32 fireReverb(s32);
void fireSoundStart(u32, const cXyz *, u32, s32);
u32 fireNew(u32);
void fireCollisionConstruct(u32);
void fireCylinderConstruct(u32);
void fireActorConstruct(fopAc_ac_c *);
void fireSttsConstruct(u32);
void fireGSttsConstruct(u32);
void fireConstructArray(u32, u32, u32, u32);
s32 fireSearchRoomEnemy(s32);
u8 fireCalcTimer(be<u8> *);
s32 fireOrderOtherEvent(fopAc_ac_c *, s32, u32, u32, u32, u32);
void fireCylinderCenter(cM3dGCyl *, cXyz *);
void fireCollisionSet(u32, u32);
void fireRegisterStatic(u32);
void fireInitializeOne(u32);
void fireInitializeZero(u32);
void fireCylinderDestroy(dCcD_Cyl *, s32);
void fireDestroyArray(u32, u32, u32, u32, u32, u32);
void fireSttsDestroy(dCcD_Stts *, s32);
void fireActorDestroy(fopAc_ac_c *, s32);
void fireDelete(u32);

struct FireSearchNameHD {
    be<u16> name;
    u8 unused[2];
};
WWHD_SIZE(FireSearchNameHD, 4);

// HD: guest constructor/destructor, search callbacks and TU-specific vtables;
// host member pointers have a different representation.
enum : u32 {
    FIRE_CYLINDER_CTOR = 0x0213FBECu,
    FIRE_CYLINDER_DTOR = 0x02515A70u,
    FIRE_SEARCH_BY_NAME = 0x025E121Cu,
    FIRE_SEARCH_BY_ID = 0x025E1234u,
    FIRE_ACTOR_VTABLE = 0x1000F0D4,
    FIRE_CYL_SHAPE_VTABLE = 0x1000F0C4,
    FIRE_SHAPE_BASE_VTABLE = 0x100015A8,
    FIRE_CYL_HIT_VTABLE = 0x1004B108,
    FIRE_CYL_VTABLE = 0x1004B150,
    FIRE_CYL_SHAPE_FINAL_VTABLE = 0x1004B160,
    FIRE_STTS_VTABLE = 0x1004AE88,
    FIRE_GSTTS_VTABLE = 0x1004AEC0,
};

inline const cXyz &fireBaseY() { return *gabi::at<cXyz>(0x101FFBC0); }
struct FireHeaderStaticsHD {
    be<f32> mMinimum, mMaximum;
    be<u8> mOne, mZero;
    u8 padA[2];
    be<u32> mCounterC, mCounter10, mCounter14, mCounter18;
};
WWHD_SIZE(FireHeaderStaticsHD, 0x1C);
inline FireHeaderStaticsHD *fireHeaderStatics() {
    return gabi::at<FireHeaderStaticsHD>(0x10463DB8);
}
