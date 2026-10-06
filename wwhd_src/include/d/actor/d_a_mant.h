#pragma once
#include "bindings.h"

/* daMant_packet_c (HD 0x28DC, a J3DPacket): mtx +0x98, mtx2 +0xC8, tevStr +0xF8, arg0 +0x894,
 * type +0x895; GX2 resources from +0x898 on */
struct mant_class : fopAc_ac_c {
    /* 0x03AC */ u8 _3AC[8];
    /* 0x03B4 */ u8 mPacket[0x28DC];
    /* 0x2C90 */ be<u8> mType;
    /* 0x2C91 */ u8 _2C91[0x3644 - 0x2C91];
    /* 0x3644 */ be<f32> m3644;
    /* 0x3648 */ be<f32> m3648;
    /* 0x364C */ u8 _364C[0x3654 - 0x364C];
    /* 0x3654 */ be<f32> m3654;
    /* 0x3658 */ u8 _3658[4];
    /* 0x365C */ dCcD_Stts mStts;
    /* 0x3698 */ dCcD_Sph mWindSph;
    /* 0x37C4 */ dCcD_Sph mMeshSph[9];
    /* 0x4250 */ Mtx34 mMtx;
    /* 0x4280 */ be<u8> m4280;
    /* 0x4281 */ u8 _4281[0x4288 - 0x4281];
};
WWHD_OFFSET(mant_class, mType, 0x2C90);
WWHD_OFFSET(mant_class, mStts, 0x365C);
WWHD_OFFSET(mant_class, mMeshSph, 0x37C4);
WWHD_OFFSET(mant_class, mMtx, 0x4250);
WWHD_SIZE(mant_class, 0x4288);
