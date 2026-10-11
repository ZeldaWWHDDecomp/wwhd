/* Crab.
 * HD: actor members move by 0x11C, and the callback has its vtable at zero. */
#pragma once
#include "gc_api/gc_api.h"
enum { /* HD archive order */
       dRes_INDEX_KN_BCK_PATA_e = 4,
       dRes_INDEX_KN_BCK_WAIT01_e = 5,
       dRes_INDEX_KN_BCK_WAIT02_e = 6,
       dRes_INDEX_KN_BCK_WALK_e = 7,
       dRes_INDEX_KN_BDL_KN_e = 0xA,
};

enum Mode {
    Mode_10_e = 0,
    Mode_11_e,
    Mode_12_e,
    Mode_13_e,
    Mode_14_e,
    Mode_15_e,
    Mode_16_e,
    Mode_17_e,
    Mode_18_e,
    Mode_19_e,
    Mode_20_e,
};

struct kn_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ u8 m2B6[2];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> m2BC;
    /* 0x3D9 */ be<u8> m2BD;
    /* 0x3DA */ be<u8> m2BE;
    /* 0x3DB */ be<u8> m2BF;
    /* 0x3DC */ be<u8> m2C0;
    /* 0x3DD */ u8 m2C1;
    /* 0x3DE */ be<s16> m2C2[4];
    /* 0x3E6 */ be<s16> m2CA;
    /* 0x3E8 */ be<f32> m2CC;
    /* 0x3EC */ be<s16> m2D0;
    /* 0x3EE */ be<s16> m2D2;
    /* 0x3F0 */ be<s16> m2D4;
    /* 0x3F2 */ u8 m2D6[6];
    /* 0x3F8 */ be<f32> m2DC;
    /* 0x3FC */ be<f32> m2E0;
    /* 0x400 */ be<f32> m2E4;
    /* 0x404 */ be<f32> m2E8;
    /* 0x408 */ cXyz m2EC;
    /* 0x414 */ csXyz m2F8;
    /* 0x41A */ u8 m2FE[2];
    /* 0x41C */ cXyz m300;
    /* 0x428 */ u8 m30C[8];
    /* 0x430 */ dPa_smokeEcallBack m314;
    /* 0x450 */ gptr<JPABaseEmitter> m334;
    /* 0x454 */ dBgS_AcchCir mAcchCir;
    /* 0x494 */ dBgS_ObjAcch mAcch;
};
WWHD_OFFSET(kn_class, mpMorf, 0x3D4);
WWHD_OFFSET(kn_class, m2CC, 0x3E8);
WWHD_OFFSET(kn_class, m300, 0x41C);
WWHD_OFFSET(kn_class, mAcch, 0x494);
WWHD_SIZE(kn_class, 0x658);

namespace Kn {
// HD: view of the additional emitter fields used by the crab's smoke and splash.
struct Emitter {
    u8 _00[0x34];
    be<f32> mRate;
    u8 _38[0x20];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    be<s16> mLifeTime;
    u8 _62[6];
    be<f32> mInitialVelOmni;
    be<f32> mInitialVelAxis;
    be<f32> mInitialVelDir;
    u8 _74[0x1AC];
    cXyz mGlobalScale;
    u8 _22C[0xC];
    cXyz mGlobalParticleScale;
    void setGlobalScale(f32 x, f32 y, f32 z) {
        mGlobalScale.x = x;
        mGlobalScale.y = y;
        mGlobalScale.z = z;
        mGlobalParticleScale.x = x;
        mGlobalParticleScale.y = y;
        mGlobalParticleScale.z = z;
    }
};
WWHD_OFFSET(Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Emitter, mInitialVelOmni, 0x68);
WWHD_OFFSET(Emitter, mInitialVelDir, 0x70);
WWHD_OFFSET(Emitter, mGlobalScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(Emitter, 0x244);
} // namespace Kn
enum {
    DSNAP_TYPE_KN = 0x56,
    dBgS_Attr_WATER_e = 0x13,
    ID_AK_JT_ELEMENTSMOKE01 = 0x2027,
    ID_AK_JN_ELEMENTSHIBUKI00 = 0x23
};

// HD: full-width attribute result, with the ground polygon base passed by address.
s32 knGetAttributeCode(dBgS*, cBgS_PolyInfo*);
