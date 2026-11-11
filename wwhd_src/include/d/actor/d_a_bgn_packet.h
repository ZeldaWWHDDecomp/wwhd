/* mDoExt_J3DModelPacketS_l: the HD model packet layout shared by the Puppet Ganon phases (bgn_class,
 * bgn3 part_s3). */
#pragma once
#include "bindings.h"

/* mDoExt_J3DModelPacketS (HD 0xB0; constructor 02080404, destructor 02082DDC, vtable 0x10008DA8
 * at +0xC). HD: the model is at +0x98; +0x9C/+0xA0/+0xA4 optional colour/material sources read by
 * the HD update (0207AB2C); +0xA8 a lazily created 0xC-byte helper (0207FD38). */
struct mDoExt_J3DModelPacketS_l {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ be<u32> __vtbl;
    /* 0x10 */ u8 _10[0x90 - 0x10];
    /* 0x90 */ be<u32> m90;
    /* 0x94 */ u8 _94[4];
    /* 0x98 */ gptr<J3DModel> mpModel;
    /* 0x9C */ be<u32> m9C;
    /* 0xA0 */ be<u32> mA0;
    /* 0xA4 */ be<u32> mA4;
    /* 0xA8 */ be<u32> mpHelper;
    /* 0xAC */ be<u8> mAC;
    /* 0xAD */ be<u8> mAD;
    /* 0xAE */ be<u8> mAE;
    /* 0xAF */ be<u8> mAF;
    void setModel(J3DModel* m) { mpModel = m; }
};
WWHD_SIZE(mDoExt_J3DModelPacketS_l, 0xB0);
