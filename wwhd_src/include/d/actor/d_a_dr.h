/** Valoo overworld: tww names on the WWHD actor and HIO layouts.
 * HD: actor fields shift by +0x11c; HIO vtable follows its members. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/gc_api.h"
enum {
    dRes_INDEX_DR_BAS_ABARE1_e = 5,
    dRes_INDEX_DR_BAS_ABARE2_e = 6,
    dRes_INDEX_DR_BAS_AKUBI1_e = 7,
    dRes_INDEX_DR_BAS_BIKU1_e = 8,
    dRes_INDEX_DR_BCK_DR_ABARE1_e = 0xB,
    dRes_INDEX_DR_BCK_DR_ABARE2_e = 0xC,
    dRes_INDEX_DR_BCK_DR_AKUBI1_e = 0xD,
    dRes_INDEX_DR_BCK_DR_BIKU1_e = 0xE,
    dRes_INDEX_DR_BCK_DR_HO1_e = 0xF,
    dRes_INDEX_DR_BCK_DR_WAIT1_e = 0x10,
    dRes_INDEX_DR_BMD_DR1_e = 0x13,
};
enum { DR1_JNT_DR_ALL_ROOT_e = 0, DR1_JNT_J_DR_SITA2_e = 0x20 };
enum { JA_SE_CM_DRG_MTOP_FIRE = 0x61AB, JA_SE_CM_DRG_MTOP_MAGMA = 0x783F };
enum {
    dPa_name_ID_AK_SN_DRPAINBIKU00 = 0x81C4,
    dPa_name_ID_AK_SN_DRPAINABARE00 = 0x81C5,
    dPa_name_ID_AK_SN_DRPAINHO00 = 0x81C6,
    dPa_name_ID_AK_SN_DRSPLASHMAGMA00 = 0x81C7,
};
enum { DSNAP_TYPE_DR = 0x99 };

/* daDr_HIO_c (HD: vtable after the members) */
struct daDr_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mScale;
    /* 0x08 */ be<u8> m0C;
    /* 0x09 */ u8 _09;
    /* 0x0A */ be<s16> m0E;
    /* 0x0C */ be<f32> mWait1Morf;
    /* 0x10 */ be<f32> mAkubi1Morf;
    /* 0x14 */ be<f32> mBiku1Morf;
    /* 0x18 */ be<f32> mAbare1Morf;
    /* 0x1C */ be<f32> mAbare2Morf;
    /* 0x20 */ be<f32> mHo1Morf;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(daDr_HIO_c, 0x28);

struct dr_class : fopAc_ac_c {
    dr_class() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1000E06C;
    }
    /* 0x3AC */ u8 m290[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<u8> mMode;
    /* 0x3D5 */ u8 m2B9;
    /* 0x3D6 */ be<s16> mCountDownTimers[3];
    /* 0x3DC */ be<s32> mCurrBckIdx;
    /* 0x3E0 */ gptr<JPABaseEmitter> mpBreathEmitter;
    /* 0x3E4 */ be<s8> m2C8;
    /* 0x3E5 */ be<s8> m2C9;
    /* 0x3E6 */ u8 m2CA[2];
};
WWHD_OFFSET(dr_class, mPhs, 0x3C8);
WWHD_OFFSET(dr_class, mpBreathEmitter, 0x3E0);
WWHD_SIZE(dr_class, 0x3E8);

enum { JPAEmtrStts_StopEmit = 1 };
// HD: narrow emitter extension for global rotation/translation and invalidation.
struct DrEmitterHD : JPABaseEmitter {
    u8 padding[0x1f0 - 0x70];
    be<f32> mGlobalRotation[9];
    u8 paddingRotation[0x22c - 0x214];
    cXyz mGlobalTranslation;
    u8 paddingTranslation[0x254 - 0x238];
    be<u32> mFlags;
};
WWHD_OFFSET(DrEmitterHD, mGlobalRotation, 0x1f0);
WWHD_OFFSET(DrEmitterHD, mGlobalTranslation, 0x22c);
WWHD_OFFSET(DrEmitterHD, mFlags, 0x254);
void dSnap_RegistFig(s32, fopAc_ac_c *, cXyz *, s16, f32, f32, f32);
