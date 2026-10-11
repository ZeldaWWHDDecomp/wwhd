#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
void __ct(dCcD_Sph *);
void JboSph_destroy(dCcD_Sph *, s32);
// HD: nodeCallBack has already resolved the joint's native data pointer.
struct JboJointDataHD {
    u8 _0[4];
    be<u16> mJntNo;
};
// HD: narrow views of the player flags and play fields read by the Baba Bud.
struct JboPlayerHD : fopAc_ac_c {
    u8 _3AC[0x3BC - 0x3AC];
    be<u32> mNoResetFlg1;
    void onForceVomitJump() {
        mNoResetFlg1 |= 0x10;
    }
};
struct JboPlayHD {
    u8 _0[0x5B2C];
    gptr<JboPlayerHD> mpPlayer;
    u8 _5B30[0x5B60 - 0x5B30];
    be<s16> mItemMagicCount;
    u8 _5B62[0x5CD8 - 0x5B62];
    be<u32> mPlayerStatus;
};
struct JboEmitterHD : JPABaseEmitter {
    u8 _70[0x247 - 0x70];
    be<u8> mGlobalAlpha;
    void setGlobalAlpha(u8 alpha) {
        mGlobalAlpha = alpha;
    }
};
struct jbo_class : fopAc_ac_c {
    /* 0x3AC */ u8 m290[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<u8> mType;
    /* 0x3D5 */ u8 m2B9;
    /* 0x3D6 */ be<u8> m2BA;
    /* 0x3D7 */ be<u8> m2BB;
    /* 0x3D8 */ be<u8> mMode;
    /* 0x3D9 */ u8 _3D9;
    /* 0x3DA */ be<s16> mFramesUntilJump;
    /* 0x3DC */ be<s16> mAnimRotation;
    /* 0x3DE */ be<s16> mAnimationSpeed;
    /* 0x3E0 */ cXyz mParticlePos;
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Sph mCoSph;
};
WWHD_OFFSET(jbo_class, mPhs, 0x3C8);
WWHD_OFFSET(jbo_class, mParticlePos, 0x3E0);
WWHD_OFFSET(jbo_class, mCoSph, 0x428);
WWHD_SIZE(jbo_class, 0x554);
