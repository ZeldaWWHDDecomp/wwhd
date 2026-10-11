// Gohma damage-check view. Private to this unit; shared actor layout stays unchanged.
// HD: actor prefix +0x128; collision arrays retain GC strides; death controller adds 0xC.
#pragma once
#include "bindings.h"
#include "d/actor/d_a_btd.h"
#include "d/d_cc_d.h"
struct BtdDamageViewHD {
    fopAc_ac_c actor;
    u8 _prefix[0x408 - sizeof(fopAc_ac_c)];
    be<u8> m02E0;
    u8 _409[3];
    be<s16> m02E4;
    be<s16> mGohmaState;
    u8 _410[12];
    be<s16> mActionState;
    be<u8> m02F6;
    u8 _41F[5];
    cXyz m02FC[19];
    cXyz m03E0;
    u8 _514[0x550 - 0x514];
    dCcD_Sph mAtSphs[19];
    dCcD_Sph mEyeSph;
    u8 _1CC0[0x5884 - 0x1CC0];
    dCcD_Cyl mHandCyls[6];
    u8 _5FA4[9];
    be<u8> mTotalDamage;
    u8 _5FAE[18];
    cXyz m5E98[25];
    be<s16> m5FC4[19];
    be<s16> m5FEA[6];
};
WWHD_OFFSET(BtdDamageViewHD, m02E0, 0x408);
WWHD_OFFSET(BtdDamageViewHD, m02E4, 0x40C);
WWHD_OFFSET(BtdDamageViewHD, mGohmaState, 0x40E);
WWHD_OFFSET(BtdDamageViewHD, mActionState, 0x41C);
WWHD_OFFSET(BtdDamageViewHD, m02F6, 0x41E);
WWHD_OFFSET(BtdDamageViewHD, m02FC, 0x424);
WWHD_OFFSET(BtdDamageViewHD, m03E0, 0x508);
WWHD_OFFSET(BtdDamageViewHD, mAtSphs, 0x550);
WWHD_OFFSET(BtdDamageViewHD, mEyeSph, 0x1B94);
WWHD_OFFSET(BtdDamageViewHD, mHandCyls, 0x5884);
WWHD_OFFSET(BtdDamageViewHD, mTotalDamage, 0x5FAD);
WWHD_OFFSET(BtdDamageViewHD, m5E98, 0x5FC0);
WWHD_OFFSET(BtdDamageViewHD, m5FC4, 0x60EC);
WWHD_OFFSET(BtdDamageViewHD, m5FEA, 0x6112);
WWHD_SIZE(BtdDamageViewHD, 0x611E);
struct BtdHioDamageHD {
    u8 _000[2];
    be<u8> m06;
    u8 _003[7];
    be<s16> m0E;
};
WWHD_OFFSET(BtdHioDamageHD, m06, 2);
WWHD_OFFSET(BtdHioDamageHD, m0E, 10);
WWHD_SIZE(BtdHioDamageHD, 12);
struct CcAtInfoBtdHD {
    gptr<void> mpObj;
    gptr<fopAc_ac_c> mpActor;
    be<u8> mDamage, mDead, mResultingAttackType, _pad;
    be<s16> mHitDirection[3];
    be<u16> mCutType;
    gptr<cXyz> pParticlePos;
    be<s32> mSound;
};
WWHD_OFFSET(CcAtInfoBtdHD, mDamage, 8);
WWHD_OFFSET(CcAtInfoBtdHD, pParticlePos, 20);
WWHD_SIZE(CcAtInfoBtdHD, 28);
// HD: one early death-path store overlays this play field with a float zero.
struct BtdPlayDamageHD {
    u8 _prefix[0x5B44];
    be<f32> mDeathReset;
};
WWHD_OFFSET(BtdPlayDamageHD, mDeathReset, 0x5B44);
WWHD_SIZE(BtdPlayDamageHD, 0x5B48);

// Private call bindings; full register signatures retained from this unit.
s32 btdGetReverb(s32);
void btdMonsSeStart(u32, cXyz*, s32, u32, s32);
void eff_off(btd_class*);
void def_se_set(fopAc_ac_c*, void*, u32);
void def_se_set_p(fopAc_ac_c*, cXyz*, void*, u32);
void at_power_check(CcAtInfoBtdHD*);
void anm_init(btd_class*, s32, f32, u8, f32, s32);
void btdParticleSet(dPa_control_c*, u32, u32, cXyz*, csXyz*, cXyz*, u32, void*, s32, void*, void*,
                    void*);
