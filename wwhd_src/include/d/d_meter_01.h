// HUD lifetime and HIO layouts.
// HD: HUD panes follow a four-byte larger message prefix. HIO objects omit the
// GameCube host-I/O number; aligned fields retain the measured HD padding.
#pragma once
#include "d/d_meter_02.h"
using meter_detail::sub_meter_class;
struct dMeter_HIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x004 */ be<s16> field_0x6;
    /* 0x006 */ be<s16> mReserved6;
    /* 0x008 */ be<s16> mReserved8;
    u8 mPaddingA[0x2];
    /* 0x00C */ be<f32> field_0xc;
    /* 0x010 */ be<f32> field_0x10;
    /* 0x014 */ be<f32> field_0x14;
    /* 0x018 */ be<f32> field_0x18;
    /* 0x01C */ be<f32> field_0x1c;
    /* 0x020 */ be<f32> field_0x20;
    /* 0x024 */ be<f32> field_0x24;
    /* 0x028 */ be<f32> mScopeWipeMaxScale;
    /* 0x02C */ be<u8> mScopeWipeAlpha;
    /* 0x02D */ be<u8> field_0x2d;
    /* 0x02E */ be<s16> field_0x2e;
    /* 0x030 */ be<s16> field_0x30;
    /* 0x032 */ be<s16> field_0x32;
    /* 0x034 */ be<s16> field_0x34;
    /* 0x036 */ be<s16> field_0x36;
    /* 0x038 */ be<s16> field_0x38;
    u8 mPadding3A[0x2];
    /* 0x03C */ be<f32> field_0x3c;
    /* 0x040 */ be<s16> field_0x40;
    /* 0x042 */ be<s16> field_0x42;
    /* 0x044 */ be<s16> field_0x44;
    /* 0x046 */ be<s16> field_0x46;
    /* 0x048 */ be<f32> field_0x48;
    /* 0x04C */ be<f32> field_0x4c;
    /* 0x050 */ be<s16> field_0x50;
    /* 0x052 */ be<s16> field_0x52;
    /* 0x054 */ be<s16> field_0x54;
    /* 0x056 */ be<s16> field_0x56;
    /* 0x058 */ be<f32> field_0x58;
    /* 0x05C */ be<f32> field_0x5c;
    /* 0x060 */ be<u8> field_0x60;
    /* 0x061 */ be<u8> field_0x61;
    /* 0x062 */ be<u8> field_0x62;
    /* 0x063 */ be<u8> field_0x63;
    /* 0x064 */ be<s16> field_0x64;
    /* 0x066 */ be<s16> field_0x66;
    /* 0x068 */ be<s16> field_0x68;
    /* 0x06A */ be<s16> field_0x6a;
    /* 0x06C */ be<s16> field_0x6c[3];
    /* 0x072 */ be<s16> field_0x72[3];
    /* 0x078 */ be<s16> field_0x78[3];
    /* 0x07E */ be<s16> field_0x7e;
    /* 0x080 */ be<s16> mReserved80;
    /* 0x082 */ be<s16> mReserved82;
    /* 0x084 */ be<s16> mReserved84;
    /* 0x086 */ be<s16> mReserved86;
    /* 0x088 */ be<s16> field_0x88;
    /* 0x08A */ be<s16> field_0x8a;
    /* 0x08C */ be<s16> field_0x8c;
    /* 0x08E */ be<s16> field_0x8e;
    /* 0x090 */ be<s16> field_0x90;
    /* 0x092 */ be<s16> field_0x92;
    /* 0x094 */ be<s16> field_0x94;
    /* 0x096 */ be<s16> field_0x96;
    /* 0x098 */ be<s16> field_0x98;
    /* 0x09A */ be<s16> field_0x9a;
    /* 0x09C */ be<s16> field_0x9c;
    /* 0x09E */ be<s16> field_0x9e;
    /* 0x0A0 */ be<s16> field_0xa0;
    /* 0x0A2 */ be<s16> field_0xa2;
    /* 0x0A4 */ be<s16> field_0xa4;
    /* 0x0A6 */ be<s16> field_0xa6;
    /* 0x0A8 */ be<s16> field_0xa8;
    /* 0x0AA */ be<s16> field_0xaa;
    /* 0x0AC */ be<s16> field_0xac;
    /* 0x0AE */ be<s16> field_0xae;
    /* 0x0B0 */ be<s16> field_0xb0;
    /* 0x0B2 */ be<s16> field_0xb2;
    /* 0x0B4 */ be<s16> field_0xb4;
    /* 0x0B6 */ be<s16> field_0xb6;
    /* 0x0B8 */ be<s16> field_0xb8;
    /* 0x0BA */ be<s16> field_0xba;
    /* 0x0BC */ be<s16> field_0xbc;
    /* 0x0BE */ be<s16> field_0xbe;
    /* 0x0C0 */ be<s16> field_0xc0;
    /* 0x0C2 */ be<s16> field_0xc2;
    /* 0x0C4 */ be<s16> field_0xc4;
    /* 0x0C6 */ be<s16> field_0xc6;
    /* 0x0C8 */ be<s16> field_0xc8;
    /* 0x0CA */ be<s16> field_0xca;
    /* 0x0CC */ be<s16> field_0xcc;
    /* 0x0CE */ be<s16> field_0xce;
    /* 0x0D0 */ be<s16> field_0xd0;
    /* 0x0D2 */ be<s16> field_0xd2;
    /* 0x0D4 */ be<s16> field_0xd4;
    /* 0x0D6 */ be<s16> field_0xd6;
    /* 0x0D8 */ be<s16> field_0xd8;
    /* 0x0DA */ be<s16> field_0xda;
    /* 0x0DC */ be<s16> field_0xdc;
    /* 0x0DE */ be<s16> field_0xde;
    /* 0x0E0 */ be<s16> field_0xe0;
    /* 0x0E2 */ be<s16> field_0xe2;
    /* 0x0E4 */ be<s16> field_0xe4;
    /* 0x0E6 */ be<s16> field_0xe6;
    /* 0x0E8 */ be<s16> field_0xe8;
    /* 0x0EA */ be<s16> field_0xea;
    /* 0x0EC */ be<s16> field_0xec;
    /* 0x0EE */ be<s16> field_0xee;
    /* 0x0F0 */ be<s16> field_0xf0;
    /* 0x0F2 */ be<s16> field_0xf2;
    /* 0x0F4 */ be<s16> field_0xf4;
    /* 0x0F6 */ be<s16> field_0xf6;
    /* 0x0F8 */ be<s16> field_0xf8;
    /* 0x0FA */ be<s16> field_0xfa;
    /* 0x0FC */ be<s16> field_0xfc;
    /* 0x0FE */ be<s16> field_0xfe;
    /* 0x100 */ be<s16> field_0x100;
    /* 0x102 */ be<s16> field_0x102;
    /* 0x104 */ be<s16> field_0x104;
    /* 0x106 */ be<s16> field_0x106;
    /* 0x108 */ be<s16> field_0x108;
    /* 0x10A */ be<s16> field_0x10a;
    /* 0x10C */ be<s16> field_0x10c;
    /* 0x10E */ be<s16> field_0x10e;
    /* 0x110 */ be<s16> field_0x110;
    /* 0x112 */ be<s16> field_0x112;
    /* 0x114 */ be<s16> field_0x114;
    /* 0x116 */ be<s16> field_0x116;
    /* 0x118 */ be<s16> field_0x118;
    /* 0x11A */ be<s16> field_0x11a;
    /* 0x11C */ be<s16> field_0x11c;
    /* 0x11E */ be<s16> field_0x11e;
    /* 0x120 */ be<u8> field_0x120;
    /* 0x121 */ be<u8> field_0x121;
    /* 0x122 */ be<u8> field_0x122;
    /* 0x123 */ be<u8> field_0x123;
    /* 0x124 */ be<s16> field_0x124;
    /* 0x126 */ be<s16> field_0x126;
    /* 0x128 */ be<s16> field_0x128;
    /* 0x12A */ be<s16> field_0x12a;
    /* 0x12C */ be<s16> field_0x12c;
    /* 0x12E */ be<s16> field_0x12e;
    /* 0x130 */ be<s16> field_0x130;
    /* 0x132 */ be<s16> field_0x132;
    /* 0x134 */ be<s16> field_0x134;
    /* 0x136 */ be<s16> field_0x136;
    /* 0x138 */ be<s16> field_0x138;
    /* 0x13A */ be<s16> field_0x13a;
    /* 0x13C */ be<s16> field_0x13c;
    /* 0x13E */ be<s16> field_0x13e;
    /* 0x140 */ be<s16> field_0x140;
    /* 0x142 */ be<u8> field_0x142;
    /* 0x143 */ be<u8> field_0x143;
    /* 0x144 */ be<u8> field_0x144;
    /* 0x145 */ be<u8> field_0x145;
    /* 0x146 */ be<u8> field_0x146;
    /* 0x147 */ be<u8> field_0x147;
    /* 0x148 */ be<u8> field_0x148;
    /* 0x149 */ be<u8> field_0x149;
    /* 0x14A */ be<u8> field_0x14a;
    /* 0x14B */ be<u8> field_0x14b;
    /* 0x14C */ be<u8> field_0x14c;
    u8 mPadding14D[0x1];
    /* 0x14E */ be<s16> field_0x14e;
    /* 0x150 */ be<s16> field_0x150;
    /* 0x152 */ be<s16> field_0x152;
    /* 0x154 */ be<s16> field_0x154;
    u8 mTail[2];
};
WWHD_SIZE(dMeter_HIO_c, 0x158);
WWHD_OFFSET(dMeter_HIO_c, field_0x6, 0x4);
WWHD_OFFSET(dMeter_HIO_c, mReserved6, 0x6);
WWHD_OFFSET(dMeter_HIO_c, mReserved8, 0x8);
WWHD_OFFSET(dMeter_HIO_c, field_0xc, 0xC);
WWHD_OFFSET(dMeter_HIO_c, field_0x10, 0x10);
WWHD_OFFSET(dMeter_HIO_c, field_0x14, 0x14);
WWHD_OFFSET(dMeter_HIO_c, field_0x18, 0x18);
WWHD_OFFSET(dMeter_HIO_c, field_0x1c, 0x1C);
WWHD_OFFSET(dMeter_HIO_c, field_0x20, 0x20);
WWHD_OFFSET(dMeter_HIO_c, field_0x24, 0x24);
WWHD_OFFSET(dMeter_HIO_c, mScopeWipeMaxScale, 0x28);
WWHD_OFFSET(dMeter_HIO_c, mScopeWipeAlpha, 0x2C);
WWHD_OFFSET(dMeter_HIO_c, field_0x2d, 0x2D);
WWHD_OFFSET(dMeter_HIO_c, field_0x2e, 0x2E);
WWHD_OFFSET(dMeter_HIO_c, field_0x30, 0x30);
WWHD_OFFSET(dMeter_HIO_c, field_0x32, 0x32);
WWHD_OFFSET(dMeter_HIO_c, field_0x34, 0x34);
WWHD_OFFSET(dMeter_HIO_c, field_0x36, 0x36);
WWHD_OFFSET(dMeter_HIO_c, field_0x38, 0x38);
WWHD_OFFSET(dMeter_HIO_c, field_0x3c, 0x3C);
WWHD_OFFSET(dMeter_HIO_c, field_0x40, 0x40);
WWHD_OFFSET(dMeter_HIO_c, field_0x42, 0x42);
WWHD_OFFSET(dMeter_HIO_c, field_0x44, 0x44);
WWHD_OFFSET(dMeter_HIO_c, field_0x46, 0x46);
WWHD_OFFSET(dMeter_HIO_c, field_0x48, 0x48);
WWHD_OFFSET(dMeter_HIO_c, field_0x4c, 0x4C);
WWHD_OFFSET(dMeter_HIO_c, field_0x50, 0x50);
WWHD_OFFSET(dMeter_HIO_c, field_0x52, 0x52);
WWHD_OFFSET(dMeter_HIO_c, field_0x54, 0x54);
WWHD_OFFSET(dMeter_HIO_c, field_0x56, 0x56);
WWHD_OFFSET(dMeter_HIO_c, field_0x58, 0x58);
WWHD_OFFSET(dMeter_HIO_c, field_0x5c, 0x5C);
WWHD_OFFSET(dMeter_HIO_c, field_0x60, 0x60);
WWHD_OFFSET(dMeter_HIO_c, field_0x61, 0x61);
WWHD_OFFSET(dMeter_HIO_c, field_0x62, 0x62);
WWHD_OFFSET(dMeter_HIO_c, field_0x63, 0x63);
WWHD_OFFSET(dMeter_HIO_c, field_0x64, 0x64);
WWHD_OFFSET(dMeter_HIO_c, field_0x66, 0x66);
WWHD_OFFSET(dMeter_HIO_c, field_0x68, 0x68);
WWHD_OFFSET(dMeter_HIO_c, field_0x6a, 0x6A);
WWHD_OFFSET(dMeter_HIO_c, field_0x6c, 0x6C);
WWHD_OFFSET(dMeter_HIO_c, field_0x72, 0x72);
WWHD_OFFSET(dMeter_HIO_c, field_0x78, 0x78);
WWHD_OFFSET(dMeter_HIO_c, field_0x7e, 0x7E);
WWHD_OFFSET(dMeter_HIO_c, mReserved80, 0x80);
WWHD_OFFSET(dMeter_HIO_c, mReserved82, 0x82);
WWHD_OFFSET(dMeter_HIO_c, mReserved84, 0x84);
WWHD_OFFSET(dMeter_HIO_c, mReserved86, 0x86);
WWHD_OFFSET(dMeter_HIO_c, field_0x88, 0x88);
WWHD_OFFSET(dMeter_HIO_c, field_0x8a, 0x8A);
WWHD_OFFSET(dMeter_HIO_c, field_0x8c, 0x8C);
WWHD_OFFSET(dMeter_HIO_c, field_0x8e, 0x8E);
WWHD_OFFSET(dMeter_HIO_c, field_0x90, 0x90);
WWHD_OFFSET(dMeter_HIO_c, field_0x92, 0x92);
WWHD_OFFSET(dMeter_HIO_c, field_0x94, 0x94);
WWHD_OFFSET(dMeter_HIO_c, field_0x96, 0x96);
WWHD_OFFSET(dMeter_HIO_c, field_0x98, 0x98);
WWHD_OFFSET(dMeter_HIO_c, field_0x9a, 0x9A);
WWHD_OFFSET(dMeter_HIO_c, field_0x9c, 0x9C);
WWHD_OFFSET(dMeter_HIO_c, field_0x9e, 0x9E);
WWHD_OFFSET(dMeter_HIO_c, field_0xa0, 0xA0);
WWHD_OFFSET(dMeter_HIO_c, field_0xa2, 0xA2);
WWHD_OFFSET(dMeter_HIO_c, field_0xa4, 0xA4);
WWHD_OFFSET(dMeter_HIO_c, field_0xa6, 0xA6);
WWHD_OFFSET(dMeter_HIO_c, field_0xa8, 0xA8);
WWHD_OFFSET(dMeter_HIO_c, field_0xaa, 0xAA);
WWHD_OFFSET(dMeter_HIO_c, field_0xac, 0xAC);
WWHD_OFFSET(dMeter_HIO_c, field_0xae, 0xAE);
WWHD_OFFSET(dMeter_HIO_c, field_0xb0, 0xB0);
WWHD_OFFSET(dMeter_HIO_c, field_0xb2, 0xB2);
WWHD_OFFSET(dMeter_HIO_c, field_0xb4, 0xB4);
WWHD_OFFSET(dMeter_HIO_c, field_0xb6, 0xB6);
WWHD_OFFSET(dMeter_HIO_c, field_0xb8, 0xB8);
WWHD_OFFSET(dMeter_HIO_c, field_0xba, 0xBA);
WWHD_OFFSET(dMeter_HIO_c, field_0xbc, 0xBC);
WWHD_OFFSET(dMeter_HIO_c, field_0xbe, 0xBE);
WWHD_OFFSET(dMeter_HIO_c, field_0xc0, 0xC0);
WWHD_OFFSET(dMeter_HIO_c, field_0xc2, 0xC2);
WWHD_OFFSET(dMeter_HIO_c, field_0xc4, 0xC4);
WWHD_OFFSET(dMeter_HIO_c, field_0xc6, 0xC6);
WWHD_OFFSET(dMeter_HIO_c, field_0xc8, 0xC8);
WWHD_OFFSET(dMeter_HIO_c, field_0xca, 0xCA);
WWHD_OFFSET(dMeter_HIO_c, field_0xcc, 0xCC);
WWHD_OFFSET(dMeter_HIO_c, field_0xce, 0xCE);
WWHD_OFFSET(dMeter_HIO_c, field_0xd0, 0xD0);
WWHD_OFFSET(dMeter_HIO_c, field_0xd2, 0xD2);
WWHD_OFFSET(dMeter_HIO_c, field_0xd4, 0xD4);
WWHD_OFFSET(dMeter_HIO_c, field_0xd6, 0xD6);
WWHD_OFFSET(dMeter_HIO_c, field_0xd8, 0xD8);
WWHD_OFFSET(dMeter_HIO_c, field_0xda, 0xDA);
WWHD_OFFSET(dMeter_HIO_c, field_0xdc, 0xDC);
WWHD_OFFSET(dMeter_HIO_c, field_0xde, 0xDE);
WWHD_OFFSET(dMeter_HIO_c, field_0xe0, 0xE0);
WWHD_OFFSET(dMeter_HIO_c, field_0xe2, 0xE2);
WWHD_OFFSET(dMeter_HIO_c, field_0xe4, 0xE4);
WWHD_OFFSET(dMeter_HIO_c, field_0xe6, 0xE6);
WWHD_OFFSET(dMeter_HIO_c, field_0xe8, 0xE8);
WWHD_OFFSET(dMeter_HIO_c, field_0xea, 0xEA);
WWHD_OFFSET(dMeter_HIO_c, field_0xec, 0xEC);
WWHD_OFFSET(dMeter_HIO_c, field_0xee, 0xEE);
WWHD_OFFSET(dMeter_HIO_c, field_0xf0, 0xF0);
WWHD_OFFSET(dMeter_HIO_c, field_0xf2, 0xF2);
WWHD_OFFSET(dMeter_HIO_c, field_0xf4, 0xF4);
WWHD_OFFSET(dMeter_HIO_c, field_0xf6, 0xF6);
WWHD_OFFSET(dMeter_HIO_c, field_0xf8, 0xF8);
WWHD_OFFSET(dMeter_HIO_c, field_0xfa, 0xFA);
WWHD_OFFSET(dMeter_HIO_c, field_0xfc, 0xFC);
WWHD_OFFSET(dMeter_HIO_c, field_0xfe, 0xFE);
WWHD_OFFSET(dMeter_HIO_c, field_0x100, 0x100);
WWHD_OFFSET(dMeter_HIO_c, field_0x102, 0x102);
WWHD_OFFSET(dMeter_HIO_c, field_0x104, 0x104);
WWHD_OFFSET(dMeter_HIO_c, field_0x106, 0x106);
WWHD_OFFSET(dMeter_HIO_c, field_0x108, 0x108);
WWHD_OFFSET(dMeter_HIO_c, field_0x10a, 0x10A);
WWHD_OFFSET(dMeter_HIO_c, field_0x10c, 0x10C);
WWHD_OFFSET(dMeter_HIO_c, field_0x10e, 0x10E);
WWHD_OFFSET(dMeter_HIO_c, field_0x110, 0x110);
WWHD_OFFSET(dMeter_HIO_c, field_0x112, 0x112);
WWHD_OFFSET(dMeter_HIO_c, field_0x114, 0x114);
WWHD_OFFSET(dMeter_HIO_c, field_0x116, 0x116);
WWHD_OFFSET(dMeter_HIO_c, field_0x118, 0x118);
WWHD_OFFSET(dMeter_HIO_c, field_0x11a, 0x11A);
WWHD_OFFSET(dMeter_HIO_c, field_0x11c, 0x11C);
WWHD_OFFSET(dMeter_HIO_c, field_0x11e, 0x11E);
WWHD_OFFSET(dMeter_HIO_c, field_0x120, 0x120);
WWHD_OFFSET(dMeter_HIO_c, field_0x121, 0x121);
WWHD_OFFSET(dMeter_HIO_c, field_0x122, 0x122);
WWHD_OFFSET(dMeter_HIO_c, field_0x123, 0x123);
WWHD_OFFSET(dMeter_HIO_c, field_0x124, 0x124);
WWHD_OFFSET(dMeter_HIO_c, field_0x126, 0x126);
WWHD_OFFSET(dMeter_HIO_c, field_0x128, 0x128);
WWHD_OFFSET(dMeter_HIO_c, field_0x12a, 0x12A);
WWHD_OFFSET(dMeter_HIO_c, field_0x12c, 0x12C);
WWHD_OFFSET(dMeter_HIO_c, field_0x12e, 0x12E);
WWHD_OFFSET(dMeter_HIO_c, field_0x130, 0x130);
WWHD_OFFSET(dMeter_HIO_c, field_0x132, 0x132);
WWHD_OFFSET(dMeter_HIO_c, field_0x134, 0x134);
WWHD_OFFSET(dMeter_HIO_c, field_0x136, 0x136);
WWHD_OFFSET(dMeter_HIO_c, field_0x138, 0x138);
WWHD_OFFSET(dMeter_HIO_c, field_0x13a, 0x13A);
WWHD_OFFSET(dMeter_HIO_c, field_0x13c, 0x13C);
WWHD_OFFSET(dMeter_HIO_c, field_0x13e, 0x13E);
WWHD_OFFSET(dMeter_HIO_c, field_0x140, 0x140);
WWHD_OFFSET(dMeter_HIO_c, field_0x142, 0x142);
WWHD_OFFSET(dMeter_HIO_c, field_0x143, 0x143);
WWHD_OFFSET(dMeter_HIO_c, field_0x144, 0x144);
WWHD_OFFSET(dMeter_HIO_c, field_0x145, 0x145);
WWHD_OFFSET(dMeter_HIO_c, field_0x146, 0x146);
WWHD_OFFSET(dMeter_HIO_c, field_0x147, 0x147);
WWHD_OFFSET(dMeter_HIO_c, field_0x148, 0x148);
WWHD_OFFSET(dMeter_HIO_c, field_0x149, 0x149);
WWHD_OFFSET(dMeter_HIO_c, field_0x14a, 0x14A);
WWHD_OFFSET(dMeter_HIO_c, field_0x14b, 0x14B);
WWHD_OFFSET(dMeter_HIO_c, field_0x14c, 0x14C);
WWHD_OFFSET(dMeter_HIO_c, field_0x14e, 0x14E);
WWHD_OFFSET(dMeter_HIO_c, field_0x150, 0x150);
WWHD_OFFSET(dMeter_HIO_c, field_0x152, 0x152);
WWHD_OFFSET(dMeter_HIO_c, field_0x154, 0x154);
struct dMeter_menuHIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x004 */ be<f32> field_0x8;
    /* 0x008 */ be<f32> field_0xc;
    /* 0x00C */ be<f32> field_0x10;
    /* 0x010 */ be<f32> field_0x14;
    /* 0x014 */ be<f32> field_0x18;
    /* 0x018 */ be<f32> field_0x1c;
    /* 0x01C */ be<f32> field_0x20;
    /* 0x020 */ be<f32> field_0x24;
    /* 0x024 */ be<s16> field_0x28;
    /* 0x026 */ be<s16> field_0x2a;
    /* 0x028 */ be<s16> field_0x2c;
    /* 0x02A */ be<s16> field_0x2e;
    /* 0x02C */ be<s16> field_0x30;
    /* 0x02E */ be<s16> field_0x32;
    /* 0x030 */ be<s16> field_0x34;
    /* 0x032 */ be<s16> field_0x36;
    /* 0x034 */ be<s16> field_0x38;
    /* 0x036 */ be<s16> field_0x3a;
    /* 0x038 */ be<f32> field_0x3c;
    /* 0x03C */ be<f32> field_0x40;
    /* 0x040 */ be<f32> field_0x44;
    /* 0x044 */ be<f32> field_0x48;
    /* 0x048 */ be<f32> field_0x4c;
    /* 0x04C */ be<f32> field_0x50;
    /* 0x050 */ be<f32> field_0x54;
    /* 0x054 */ be<f32> field_0x58;
    /* 0x058 */ be<f32> field_0x5c;
    /* 0x05C */ be<f32> field_0x60;
    /* 0x060 */ be<f32> field_0x64;
    /* 0x064 */ be<f32> field_0x68;
    /* 0x068 */ be<u8> field_0x6c;
    /* 0x069 */ be<u8> field_0x6d;
    /* 0x06A */ be<u8> field_0x6e;
    /* 0x06B */ be<u8> field_0x6f;
    /* 0x06C */ be<u8> field_0x70;
    /* 0x06D */ be<u8> field_0x71;
    /* 0x06E */ be<u8> field_0x72;
    /* 0x06F */ be<u8> field_0x73;
    /* 0x070 */ be<u8> field_0x74;
    /* 0x071 */ be<u8> field_0x75;
    /* 0x072 */ be<u8> field_0x76;
    /* 0x073 */ be<u8> field_0x77;
    /* 0x074 */ be<u8> field_0x78;
    /* 0x075 */ be<u8> field_0x79;
    /* 0x076 */ be<u8> field_0x7a;
    /* 0x077 */ be<u8> field_0x7b;
    /* 0x078 */ be<s16> field_0x7c;
    /* 0x07A */ be<s16> field_0x7e;
    /* 0x07C */ be<s16> field_0x80;
    /* 0x07E */ be<s16> field_0x82;
    /* 0x080 */ be<s16> field_0x84;
    /* 0x082 */ be<s16> mReserved82;
    /* 0x084 */ be<s16> field_0x88;
    /* 0x086 */ be<s16> field_0x8a;
    /* 0x088 */ be<s16> field_0x8c;
    /* 0x08A */ be<s16> field_0x8e;
    /* 0x08C */ be<s16> field_0x90;
    /* 0x08E */ be<s16> field_0x92;
    /* 0x090 */ be<f32> field_0x94;
    /* 0x094 */ be<u8> field_0x98;
    /* 0x095 */ be<u8> field_0x99;
    /* 0x096 */ be<s16> field_0x9a;
    /* 0x098 */ be<s16> field_0x9c;
    /* 0x09A */ be<s16> field_0x9e;
    /* 0x09C */ be<f32> field_0xa0;
    /* 0x0A0 */ be<s16> field_0xa4;
    /* 0x0A2 */ be<s16> field_0xa6;
    /* 0x0A4 */ be<s16> field_0xa8;
    /* 0x0A6 */ be<s16> field_0xaa;
    /* 0x0A8 */ be<s16> field_0xac;
    /* 0x0AA */ be<s16> field_0xae;
    /* 0x0AC */ be<s16> field_0xb0;
    /* 0x0AE */ be<s16> field_0xb2;
    /* 0x0B0 */ be<s16> field_0xb4;
    /* 0x0B2 */ be<s16> field_0xb6;
    /* 0x0B4 */ be<u8> field_0xb8;
    u8 mPaddingB5[0x1];
    /* 0x0B6 */ be<s16> field_0xba;
    /* 0x0B8 */ be<s16> field_0xbc;
    /* 0x0BA */ be<u8> field_0xbe;
    /* 0x0BB */ be<u8> field_0xbf;
    /* 0x0BC */ be<u8> field_0xc0;
    /* 0x0BD */ be<u8> field_0xc1;
    /* 0x0BE */ be<u8> field_0xc2;
    /* 0x0BF */ be<u8> field_0xc3;
    /* 0x0C0 */ be<u8> field_0xc4;
    /* 0x0C1 */ be<u8> field_0xc5;
    /* 0x0C2 */ be<u8> field_0xc6;
    /* 0x0C3 */ be<u8> field_0xc7;
    /* 0x0C4 */ be<u8> field_0xc8;
    /* 0x0C5 */ be<u8> field_0xc9;
    /* 0x0C6 */ be<u8> field_0xca;
    u8 mTail[1];
};
WWHD_SIZE(dMeter_menuHIO_c, 0xC8);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x8, 0x4);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc, 0x8);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x10, 0xC);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x14, 0x10);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x18, 0x14);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x1c, 0x18);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x20, 0x1C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x24, 0x20);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x28, 0x24);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x2a, 0x26);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x2c, 0x28);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x2e, 0x2A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x30, 0x2C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x32, 0x2E);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x34, 0x30);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x36, 0x32);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x38, 0x34);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x3a, 0x36);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x3c, 0x38);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x40, 0x3C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x44, 0x40);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x48, 0x44);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x4c, 0x48);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x50, 0x4C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x54, 0x50);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x58, 0x54);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x5c, 0x58);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x60, 0x5C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x64, 0x60);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x68, 0x64);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6c, 0x68);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6d, 0x69);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6e, 0x6A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6f, 0x6B);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x70, 0x6C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x71, 0x6D);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x72, 0x6E);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x73, 0x6F);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x74, 0x70);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x75, 0x71);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x76, 0x72);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x77, 0x73);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x78, 0x74);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x79, 0x75);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7a, 0x76);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7b, 0x77);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7c, 0x78);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7e, 0x7A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x80, 0x7C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x82, 0x7E);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x84, 0x80);
WWHD_OFFSET(dMeter_menuHIO_c, mReserved82, 0x82);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x88, 0x84);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x8a, 0x86);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x8c, 0x88);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x8e, 0x8A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x90, 0x8C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x92, 0x8E);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x94, 0x90);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x98, 0x94);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x99, 0x95);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x9a, 0x96);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x9c, 0x98);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x9e, 0x9A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xa0, 0x9C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xa4, 0xA0);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xa6, 0xA2);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xa8, 0xA4);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xaa, 0xA6);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xac, 0xA8);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xae, 0xAA);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb0, 0xAC);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb2, 0xAE);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb4, 0xB0);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb6, 0xB2);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb8, 0xB4);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xba, 0xB6);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xbc, 0xB8);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xbe, 0xBA);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xbf, 0xBB);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc0, 0xBC);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc1, 0xBD);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc2, 0xBE);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc3, 0xBF);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc4, 0xC0);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc5, 0xC1);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc6, 0xC2);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc7, 0xC3);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc8, 0xC4);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc9, 0xC5);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xca, 0xC6);
struct dMeter_msg_HIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x004 */ GXColor field_0x5;
    /* 0x008 */ GXColor field_0x9;
    /* 0x00C */ be<u8> field_0xd;
    /* 0x00D */ be<u8> field_0xe;
    /* 0x00E */ be<u8> field_0xf;
    /* 0x00F */ be<u8> field_0x10;
    /* 0x010 */ be<u8> field_0x11;
    /* 0x011 */ be<u8> field_0x12;
    /* 0x012 */ be<u8> field_0x13;
    /* 0x013 */ be<u8> field_0x14;
    /* 0x014 */ be<u8> field_0x15;
    /* 0x015 */ be<u8> field_0x16;
    /* 0x016 */ be<u8> field_0x17;
    /* 0x017 */ be<u8> field_0x18;
    /* 0x018 */ be<u8> field_0x19;
    /* 0x019 */ be<u8> field_0x1a;
    /* 0x01A */ be<u8> field_0x1b;
    /* 0x01B */ be<u8> field_0x1c;
    /* 0x01C */ be<u8> field_0x1d;
    /* 0x01D */ be<u8> field_0x1e;
    /* 0x01E */ be<u8> field_0x1f;
    /* 0x01F */ be<u8> field_0x20;
    /* 0x020 */ be<u8> field_0x21;
    /* 0x021 */ be<u8> field_0x22;
    /* 0x022 */ be<u8> field_0x23;
    /* 0x023 */ be<u8> field_0x24;
    /* 0x024 */ be<u8> field_0x25;
    /* 0x025 */ be<u8> field_0x26;
    /* 0x026 */ be<u8> field_0x27;
    /* 0x027 */ be<u8> field_0x28;
    /* 0x028 */ be<u8> field_0x29;
    /* 0x029 */ be<u8> field_0x2a;
    /* 0x02A */ be<u8> field_0x2b;
    /* 0x02B */ be<u8> field_0x2c;
    /* 0x02C */ be<u8> field_0x2d;
    /* 0x02D */ be<u8> field_0x2e;
    /* 0x02E */ be<u8> field_0x2f;
    /* 0x02F */ be<u8> field_0x30;
    u8 mPadding30[0x4];
    /* 0x034 */ be<f32> field_0x38;
    /* 0x038 */ be<f32> field_0x3c;
    /* 0x03C */ be<f32> field_0x40;
    /* 0x040 */ be<s16> field_0x44;
    /* 0x042 */ be<s16> field_0x46;
    /* 0x044 */ be<s16> field_0x48;
    /* 0x046 */ be<s16> field_0x4a;
    /* 0x048 */ be<s16> field_0x4c;
    u8 mPadding4A[0x2];
    /* 0x04C */ be<f32> field_0x50;
    /* 0x050 */ be<f32> field_0x54;
    /* 0x054 */ be<u8> field_0x58;
    u8 mPadding55[0x1];
    /* 0x056 */ be<s16> field_0x5a;
    /* 0x058 */ be<s16> field_0x5c;
    /* 0x05A */ be<s16> field_0x5e;
    /* 0x05C */ be<s16> field_0x60;
    /* 0x05E */ be<s16> field_0x62;
    /* 0x060 */ be<u8> field_0x64;
    u8 mPadding61[0x1];
    /* 0x062 */ be<s16> field_0x66;
    /* 0x064 */ be<s16> field_0x68;
    /* 0x066 */ be<s16> field_0x6a;
    /* 0x068 */ be<s16> field_0x6c;
    /* 0x06A */ be<s16> field_0x6e;
    /* 0x06C */ be<s16> field_0x70;
    /* 0x06E */ be<u8> field_0x72;
    u8 mPadding6F[0x1];
    /* 0x070 */ be<s16> field_0x74;
    /* 0x072 */ be<s16> field_0x76;
    /* 0x074 */ be<s16> field_0x78;
    /* 0x076 */ be<s16> field_0x7a;
    /* 0x078 */ be<s16> field_0x7c;
    /* 0x07A */ be<u8> field_0x7e;
    /* 0x07B */ be<u8> field_0x7f;
    /* 0x07C */ be<u8> field_0x80;
    /* 0x07D */ be<u8> field_0x81;
    /* 0x07E */ be<u8> field_0x82;
    /* 0x07F */ be<u8> field_0x83;
    /* 0x080 */ be<u8> field_0x84;
    /* 0x081 */ be<u8> field_0x85;
    /* 0x082 */ be<u8> field_0x86;
    /* 0x083 */ be<u8> field_0x87;
    /* 0x084 */ be<u8> field_0x88;
    /* 0x085 */ be<u8> field_0x89;
    /* 0x086 */ be<u8> field_0x8a;
    u8 mTail[1];
};
WWHD_SIZE(dMeter_msg_HIO_c, 0x88);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x5, 0x4);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x9, 0x8);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0xd, 0xC);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0xe, 0xD);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0xf, 0xE);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x10, 0xF);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x11, 0x10);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x12, 0x11);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x13, 0x12);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x14, 0x13);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x15, 0x14);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x16, 0x15);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x17, 0x16);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x18, 0x17);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x19, 0x18);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1a, 0x19);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1b, 0x1A);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1c, 0x1B);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1d, 0x1C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1e, 0x1D);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1f, 0x1E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x20, 0x1F);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x21, 0x20);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x22, 0x21);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x23, 0x22);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x24, 0x23);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x25, 0x24);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x26, 0x25);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x27, 0x26);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x28, 0x27);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x29, 0x28);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2a, 0x29);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2b, 0x2A);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2c, 0x2B);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2d, 0x2C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2e, 0x2D);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2f, 0x2E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x30, 0x2F);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x38, 0x34);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x3c, 0x38);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x40, 0x3C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x44, 0x40);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x46, 0x42);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x48, 0x44);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x4a, 0x46);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x4c, 0x48);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x50, 0x4C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x54, 0x50);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x58, 0x54);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x5a, 0x56);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x5c, 0x58);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x5e, 0x5A);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x60, 0x5C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x62, 0x5E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x64, 0x60);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x66, 0x62);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x68, 0x64);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x6a, 0x66);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x6c, 0x68);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x6e, 0x6A);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x70, 0x6C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x72, 0x6E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x74, 0x70);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x76, 0x72);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x78, 0x74);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x7a, 0x76);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x7c, 0x78);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x7e, 0x7A);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x7f, 0x7B);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x80, 0x7C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x81, 0x7D);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x82, 0x7E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x83, 0x7F);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x84, 0x80);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x85, 0x81);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x86, 0x82);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x87, 0x83);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x88, 0x84);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x89, 0x85);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x8a, 0x86);

struct mapCtrlDisp_c {
    be<u8> field_0x0, field_0x1, field_0x2;
    u8 mPadding;
    be<s16> field_0x4;
    void initMapCtrlDisp();
};
WWHD_SIZE(mapCtrlDisp_c, 6);
WWHD_OFFSET(mapCtrlDisp_c, field_0x4, 4);
struct MeterHIOFloats {
    be<f32> mValues[24];
};
extern MeterHIOFloats& l_hioValues;
extern be<f32>& l_zero;
extern be<f32>& l_one;
extern be<f32>& l_half;
extern be<f32>& l_mapRange;
inline u32 meter01_allocate(u32 size) {
    return gabi::call<u32>(0x0273AD10, size);
}
inline void meter01_clear(void* address, u32 size) {
    gabi::call(0x028F521C, address, size);
}

struct dMeter_map_HIO_c {
    be<s8> mNo;
    be<u8> field_0x5, field_0x6, field_0x7;
    be<s16> field_0x8, field_0xa, field_0xc;
    u8 mPaddingA[2];
    be<f32> field_0x10;
    be<s16> field_0x14, field_0x16;
    u8 mPadding14[4];
    be<f32> mHDScale;
    be<u32> mVtable;
};
WWHD_SIZE(dMeter_map_HIO_c, 0x20);
WWHD_OFFSET(dMeter_map_HIO_c, field_0x8, 4);
WWHD_OFFSET(dMeter_map_HIO_c, field_0x14, 0x10);
struct MeterMapState {
    be<s16> mIconFreePosX;
    be<s16> mIconFreePosY;
    u8 mPadding4[2];
    be<s16> mPosLeftUpX;
    be<s16> mPosLeftUpY;
    u8 mPaddingA[6];
    be<u8> mSelfAlpha;
    u8 mPadding11[7];
    be<u8> mDisplayMode, mAlpha, mIconFreeAlpha, mIconDisplayMode;
};
WWHD_SIZE(MeterMapState, 0x1C);
WWHD_OFFSET(MeterMapState, mDisplayMode, 0x18);
extern dMeter_map_HIO_c& g_meter_mapHIO;
extern MeterMapState& l_mapState;
extern be<f32>& l_iconFreeScale;
extern be<f32>& l_iconSelfScale;
inline void meter01_setAlpha(fopMsgM_pane_class* pane) {
    gabi::call(0x025DB678, pane);
}
inline s32 meter01_isOverlapRequest() {
    return gabi::call<s32>(0x025DBE38);
}
inline void meter01_setDrawList(u32 object, u32 offset) {
    u32 list = gabi::call<u32>(0x025200D4) + 0x5D30;
    gabi::call(0x0252CDC0, list, list + offset, list + offset + 4, object);
}
struct MeterPrefixHD {
    u8 mMessage[0x100];
    be<u32> heap;
    fopMsgM_pane_class field_0x0100[3];
    fopMsgM_pane_class field_0x01a8;
    fopMsgM_pane_class field_0x01e0;
    fopMsgM_pane_class field_0x0218;
    fopMsgM_pane_class field_0x0250;
    fopMsgM_pane_class field_0x0288;
    fopMsgM_pane_class field_0x02c0;
    fopMsgM_pane_class field_0x02f8;
    fopMsgM_pane_class field_0x0330;
    fopMsgM_pane_class field_0x0368;
    fopMsgM_pane_class field_0x03a0;
    fopMsgM_pane_class field_0x03d8;
    fopMsgM_pane_class field_0x0410;
    fopMsgM_pane_class field_0x0448[4];
    fopMsgM_pane_class field_0x0528;
    fopMsgM_pane_class field_0x0560;
    fopMsgM_pane_class field_0x0598;
    fopMsgM_pane_class field_0x05d0;
    fopMsgM_pane_class field_0x0608;
};
WWHD_SIZE(MeterPrefixHD, 0x644);
WWHD_OFFSET(MeterPrefixHD, heap, 0x100);

struct MeterHeapVtable {
    u8 mPadding0[0x3C];
    be<u32> mFree;
    u8 mPadding40[4];
    be<u32> mRemoveResourceAll;
};
struct MeterHeap {
    u8 mPadding0[0xC];
    gptr<MeterHeapVtable> mVtable;
};
WWHD_OFFSET(MeterHeap, mVtable, 0xC);
WWHD_OFFSET(MeterHeapVtable, mFree, 0x3C);
WWHD_OFFSET(MeterHeapVtable, mRemoveResourceAll, 0x44);
struct MeterScreenVtable {
    u8 mPadding0[0xC];
    be<u32> mDestructor;
};
struct MeterScreen {
    u8 mPadding0[0xC8];
    gptr<MeterScreenVtable> mVtable;
};
WWHD_OFFSET(MeterScreen, mVtable, 0xC8);
WWHD_OFFSET(MeterScreenVtable, mDestructor, 0xC);
struct MeterHDState {
    be<f32> mPaneScales[8];
    be<u32> mStatusFlags;
    be<s16> mRoomNumber;
    be<u8> mStateBytes[0x26];
};
WWHD_OFFSET(MeterHDState, mStatusFlags, 0x20);
WWHD_OFFSET(MeterHDState, mStateBytes, 0x26);
WWHD_SIZE(MeterHDState, 0x4C);
extern MeterHDState& l_hdState;
extern be<u32>& sMainParts1;
extern be<u32>& sMainParts2;
extern be<u32>& sMainParts3;
extern be<u32>& sScrTimer1;
extern be<u32>& sScrTimer2;
extern be<u32>& sChoiceRoad;
extern be<u32>& moveItemPane;
extern be<u32>& dMeter_windID;
extern be<u8>& rupy_soundOnFlag;
extern be<u8>& rupy_soundSetFlag;
extern be<u8>& dMeter_windStatus;
inline u32 meter01_setCurrentHeap(u32 heap) {
    return gabi::call<u32>(0x025E3570, heap);
}
inline void meter01_soundStart(u32 sound) {
    gabi::call(0x025E1988, sound);
}
inline u32 meter01_searchMessage(u32 id) {
    return gabi::call<u32>(0x025DE50C, id);
}
inline void meter01_deleteMessage(u32 message) {
    gabi::call(0x025DF944, message);
}
inline void meter01_destroyExpHeap(u32 heap) {
    gabi::call(0x025DB8F4, heap);
}
inline void meter01_deleteHIOChild(s32 child) {
    gabi::call(0x025F0A18, child);
}
inline void meter01_freeTexture(sub_meter_class* meter, be<u32>& texture) {
    u32 heap = reinterpret_cast<MeterPrefixHD*>(meter)->heap;
    u32 target = gabi::at<MeterHeap>(heap)->mVtable->mFree;
    u32 resource = texture;
    gabi::call_ptr(target, heap, resource);
}
inline void meter01_destroyScreen(u32 screen) {
    if (screen) {
        u32 target = gabi::at<MeterScreen>(screen)->mVtable->mDestructor;
        gabi::call_ptr(target, screen, 3);
    }
}
inline void meter01_removeSwimResources() {
    u32 heap = gabi::load<u32>(gabi::call<u32>(0x025200D4) + 0x5AA0);
    u32 target = gabi::at<MeterHeap>(heap)->mVtable->mRemoveResourceAll;
    gabi::call_ptr(target, heap);
}
inline void meter01_clearHudState() {
    // HD: reset the expanded HUD state after releasing resources.
    l_hdState.mStatusFlags = 0;
    for (u32 offset : {0x62u, 0x63u, 0x3Eu, 0x3Fu, 0x40u, 0x41u, 0x42u, 0x43u, 0x44u, 0x5Au, 0x5Bu, 0x5Cu,
                       0x5Du, 0x5Eu, 0x5Fu})
        l_hdState.mStateBytes[offset - 0x3E] = 0;
    for (int i = 0; i < 8; i++)
        l_hdState.mPaneScales[i] = l_zero;
    l_hdState.mRoomNumber = 0;
    l_hdState.mStateBytes[0x60 - 0x3E] = 0;
    l_hdState.mStateBytes[0x61 - 0x3E] = 0;
    for (int i = 0; i < 7; i++) {
        l_hdState.mStateBytes[0x53 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x45 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x4C - 0x3E + i] = 0;
    }
}

inline u8 meter01_mapSetting() {
    return gabi::load<u8>(0x1047AD0C);
}
inline void meter01_restoreMapSetting(u8 setting) {
    // HD: this byte is separate from the shared play accessors.
    u32 game = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(game + 0x5C23, setting == 0);
}

WWHD_SIZE(MeterHIOFloats, 0x60);
WWHD_SIZE(MeterHeapVtable, 0x48);
WWHD_SIZE(MeterHeap, 0x10);
WWHD_SIZE(MeterScreenVtable, 0x10);
WWHD_SIZE(MeterScreen, 0xCC);
WWHD_OFFSET(mapCtrlDisp_c, field_0x0, 0);
WWHD_OFFSET(mapCtrlDisp_c, field_0x1, 1);
WWHD_OFFSET(mapCtrlDisp_c, field_0x2, 2);
WWHD_OFFSET(MeterMapState, mIconFreePosX, 0);
WWHD_OFFSET(MeterMapState, mIconFreePosY, 2);
WWHD_OFFSET(MeterMapState, mPosLeftUpX, 6);
WWHD_OFFSET(MeterMapState, mPosLeftUpY, 8);
WWHD_OFFSET(MeterMapState, mSelfAlpha, 0x10);
WWHD_OFFSET(MeterMapState, mAlpha, 0x19);
WWHD_OFFSET(MeterMapState, mIconFreeAlpha, 0x1A);
WWHD_OFFSET(MeterMapState, mIconDisplayMode, 0x1B);

// HUD frame-update views. HD: preserve repeated play lookups and virtual-call order.
struct MeterUpdateCamera {
  u8 mPadding[0x510];
  be<u32> mFlags;
};
WWHD_OFFSET(MeterUpdateCamera, mFlags, 0x510);
WWHD_SIZE(MeterUpdateCamera, 0x514);
struct MeterUpdate2D {
  be<u8> mShow;
};
WWHD_SIZE(MeterUpdate2D, 1);
struct MeterUpdateMenu {
  be<u8> mMenuStatus;
  u8 mPadding1;
  be<u8> mWindowFlag;
  u8 mPadding3[2];
  be<u8> mAuctionFlag;
};
WWHD_OFFSET(MeterUpdateMenu, mWindowFlag, 2);
WWHD_OFFSET(MeterUpdateMenu, mAuctionFlag, 5);
WWHD_SIZE(MeterUpdateMenu, 6);
struct MeterUpdateCameraAttention {
  be<u32> mStatus;
};
WWHD_SIZE(MeterUpdateCameraAttention, 4);
struct MeterUpdatePlayerVtable {
  u8 mPadding[0xD4];
  be<u32> mCheckRopeTag;
};
WWHD_OFFSET(MeterUpdatePlayerVtable, mCheckRopeTag, 0xD4);
WWHD_SIZE(MeterUpdatePlayerVtable, 0xD8);
inline dComIfG_play_c *meterUpdatePlay() {
  return &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play;
}
inline MeterUpdate2D *meterUpdate2DState() {
  return gabi::at<MeterUpdate2D>(dComIfGp_ea() + 0x5C22);
}
inline MeterUpdateMenu *meterUpdateMenu() {
  return gabi::at<MeterUpdateMenu>(0x101EA067);
}
inline s32 meterUpdateDemoMode() {
  return gabi::object_at<be<s32>>(0x101D6010);
}
inline u8 meterUpdateKamomeState() {
  return gabi::object_at<be<u8>>(0x101D5F45);
}
inline MeterUpdateCameraAttention *meterUpdateCameraAttention() {
  return gabi::at<MeterUpdateCameraAttention>(dComIfGp_ea() + 0x5B00);
}
inline BOOL meterUpdateCheckRopeTag(fopAc_ac_c *player) {
  u32 target = gabi::at<MeterUpdatePlayerVtable>(player->__vtbl)->mCheckRopeTag;
  return gabi::call_ptr<BOOL>(target, player);
}
inline meter_detail::StageInfo *meterUpdateStage() {
  return static_cast<meter_detail::StageInfo *>(dComIfGp_getStageStagInfo());
}
MeterUpdateCamera *meterUpdateCameraBody();
s32 meterUpdateStartCheckOld(dEvent_manager_c *, const char *);
u32 meterUpdateGetPictureRecord(u32);
u32 meterUpdatePictureActive(u32);

inline u32 meterUpdatePictureRecord() {
  u32 save = gabi::object_at<be<u32>>(0x101F84DC);
  return meterUpdateGetPictureRecord(save + 0x12C0);
}

f32 meterUpdateValueIncrease(s32, s32, u32);
inline dMeter_HIO_c *meterUpdateHIO() {
  return gabi::at<dMeter_HIO_c>(0x1047AD14);
}
inline s32 meterUpdateMenuTimer() {
  return gabi::object_at<be<s32>>(0x101EA02C);
}

struct MeterUpdateScratch {
  u8 mPadding0[8];
  be<u8> mButtonFlags;
  u8 mPadding9;
  be<u16> mSound;
  u8 mPaddingC[4];
  union {
    be<u32> mConvertedLife;
    struct {
      be<s16> mConversionHigh;
      be<s16> mLifeHalfword;
    };
  };
  u8 mPadding14[8];
};
WWHD_OFFSET(MeterUpdateScratch, mButtonFlags, 8);
WWHD_OFFSET(MeterUpdateScratch, mSound, 0xA);
WWHD_OFFSET(MeterUpdateScratch, mConvertedLife, 0x10);
WWHD_OFFSET(MeterUpdateScratch, mLifeHalfword, 0x12);
WWHD_SIZE(MeterUpdateScratch, 0x1C);
struct MeterUpdateSharedState {
  u8 mPadding0[0x18];
  be<f32> mActionAlpha, mSecondaryAlpha, mRupyAlpha, mKeyAlpha;
  u8 mPadding28[0x10];
  be<u32> mStatusFlags;
  be<u16> mMiniGameType;
  be<u8> mButtonFlags;
  u8 mPadding3F[2];
  be<u8> mButtonShowFlags;
  u8 mPadding42;
  be<u8> mBattleState, mBattleMode;
};
WWHD_OFFSET(MeterUpdateSharedState, mActionAlpha, 0x18);
WWHD_OFFSET(MeterUpdateSharedState, mStatusFlags, 0x38);
WWHD_OFFSET(MeterUpdateSharedState, mButtonShowFlags, 0x41);
WWHD_OFFSET(MeterUpdateSharedState, mBattleState, 0x43);
WWHD_SIZE(MeterUpdateSharedState, 0x45);
struct MeterUpdatePlayer {
  u8 mPadding0[0x3BC];
  be<u32> mSoupPowerFlags;
};
WWHD_OFFSET(MeterUpdatePlayer, mSoupPowerFlags, 0x3BC);
WWHD_SIZE(MeterUpdatePlayer, 0x3C0);
inline MeterUpdateSharedState *meterUpdateSharedState() {
  return gabi::at<MeterUpdateSharedState>(0x1047B03C);
}
inline dSv_player_status_a_c *meterUpdatePlayerStatus() {
  return &dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA;
}
inline be<u8> &meterUpdateBattleFlag() {
  return gabi::object_at<be<u8>>(0x101EA078);
}
inline be<u8> &meterUpdateHeartSoundFlag() {
  return gabi::object_at<be<u8>>(0x101EA077);
}
inline u8 meterUpdateBattleState() {
  return gabi::object_at<be<u8>>(0x101D5F44);
}
void meterUpdateResetHearts(sub_meter_class *);

inline u8 meterUpdateCollectMode() {
  return gabi::object_at<be<u8>>(0x101EA073);
}
void meterUpdateHeartScaleCountdown(u32);
void meterUpdateMaxLifeCountdown(u32);
void meterUpdateActionAudio();

inline u8 meterUpdateMessageMode() {
  return gabi::object_at<be<u8>>(0x1047B09E);
}

inline be<s16> &meterUpdateActionMode() {
  return gabi::object_at<be<s16>>(0x101EA04E);
}
inline be<s16> &meterUpdateActionFrame() {
  return gabi::object_at<be<s16>>(0x101EA04C);
}
inline be<u8> &meterUpdateButtonFlags() {
  return gabi::object_at<be<u8>>(0x1047B07A);
}
inline u8 meterUpdateButtonIconMode() {
  return gabi::object_at<be<u8>>(0x1047B09F);
}
inline u8 meterUpdateMenuButton() {
  return gabi::object_at<be<u8>>(0x101EA06A);
}
inline u8 meterUpdateItemMode() { return gabi::object_at<be<u8>>(0x101EA072); }
BOOL meterUpdateIsBit(be<u8> *, u8);
void meterUpdateOnBit(be<u8> *, u8);
void meterUpdateOffBit(be<u8> *, u8);

struct MeterUpdateButtonState {
  u8 mPadding0[0x3039];
  be<u8> mMaskFrames;
};
WWHD_OFFSET(MeterUpdateButtonState, mMaskFrames, 0x3039);
WWHD_SIZE(MeterUpdateButtonState, 0x303A);
struct MeterUpdateSystem {
  u8 mPadding0[0x17C];
  be<u32> mHudContext;
};
WWHD_OFFSET(MeterUpdateSystem, mHudContext, 0x17C);
WWHD_SIZE(MeterUpdateSystem, 0x180);
inline MeterUpdateSystem *meterUpdateSystem() {
  return gabi::at<MeterUpdateSystem>(gabi::object_at<be<u32>>(0x101F8344));
}
inline be<s16> &meterUpdateActionAlphaFrame() {
  return gabi::object_at<be<s16>>(0x101EA050);
}
void meterUpdateWeaponMove(sub_meter_class *);
void meterUpdateXYMove(sub_meter_class *);
void meterUpdateRAlpha(sub_meter_class *);
void meterUpdateEnemyMove(sub_meter_class *);
void meterUpdateMagicGaugeMove(sub_meter_class *);
void meterUpdateMagicAlpha(sub_meter_class *);
void meterUpdateMetronomeVisibility(sub_meter_class *);
void meterUpdateRupyMove(sub_meter_class *);
void meterUpdateKeyMove(sub_meter_class *);
void meterUpdateCompassMove(sub_meter_class *);
void meterUpdateRecollectBossData();
void meterUpdateMetronomeMove(sub_meter_class *);
void meterUpdateGaugeAlpha(sub_meter_class *);
void meterUpdateWindMove(sub_meter_class *);
BOOL meterUpdateGetterB();
BOOL meterUpdateIsPause();
void meterUpdateDecTimer(dSv_player_item_record_c *);
BOOL meterUpdateHudContextActive(u32);
void meterUpdateSetAlpha(fopMsgM_pane_class *, f32);
void meterUpdateSetAlphaZero(fopMsgM_pane_class *);
void *meterUpdateActorNameJudge(void *, void *);
using MeterUpdateActorJudgeFn = void *(*)(void *, void *);
void *meterUpdateActorJudge(MeterUpdateActorJudgeFn, void *);

namespace MeterUpdate {
enum dActStts_e : u8 {
  dActStts_BLANK_e = 0x0,
  dActStts_RETURN_e = 0x7,
  dActStts_CHOOSE_e = 0x17,
  dActStts_NEXT_e = 0x19,
  dActStts_PARRY_e = 0x1A,
  dActStts_ba_nageru__dupe_2D = 0x2D,
  dActStts_ba_motu__dupe_2E = 0x2E,
  dActStts_ba_sake__dupe_31 = 0x31,
  dActStts_bow_01 = 0x3D,
  dActStts_HIDDEN_e = 0x3E,
  dActStts_HD_RETURN_e = 0x46,
};
} // namespace MeterUpdate

// Meter weapon range: HD omits the GameCube HIO number and retains alignment.
struct dMeter_message_HIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x04 */ GXColor field_0x5;
    /* 0x08 */ GXColor field_0x9;
    /* 0x0C */ GXColor field_0xd;
    /* 0x10 */ GXColor field_0x11;
    /* 0x14 */ GXColor field_0x15;
    /* 0x18 */ GXColor field_0x19;
    /* 0x1C */ GXColor field_0x1d;
    /* 0x20 */ GXColor field_0x21;
    /* 0x24 */ GXColor field_0x25;
    /* 0x28 */ be<u8> field_0x29;
    /* 0x29 */ be<u8> field_0x2a;
    /* 0x2A */ be<u8> field_0x2b;
    /* 0x2B */ be<u8> field_0x2c;
    /* 0x2C */ be<u8> field_0x2d;
    /* 0x2D */ be<u8> field_0x2e;
    /* 0x2E */ be<u8> field_0x2f;
    /* 0x2F */ be<u8> field_0x30;
    /* 0x30 */ be<u8> field_0x31;
    /* 0x31 */ be<u8> field_0x32;
    /* 0x32 */ be<s16> field_0x34;
    /* 0x34 */ be<s16> field_0x36;
    /* 0x36 */ be<s16> field_0x38;
    /* 0x38 */ be<u8> field_0x3a;
    /* 0x39 */ be<u8> field_0x3b;
    /* 0x3A */ be<s16> field_0x3c;
    /* 0x3C */ be<f32> field_0x40;
    /* 0x40 */ be<u8> field_0x44;
    u8 mPadding41[1];
    /* 0x42 */ be<s16> field_0x46;
    /* 0x44 */ be<s16> field_0x48;
    /* 0x46 */ be<u8> field_0x4a;
    /* 0x47 */ be<u8> field_0x4b;
    /* 0x48 */ be<u8> field_0x4c;
    /* 0x49 */ be<u8> field_0x4d;
    /* 0x4A */ be<u8> field_0x4e;
    /* 0x4B */ be<u8> field_0x4f;
    /* 0x4C */ be<u8> field_0x50;
    /* 0x4D */ be<u8> field_0x51;
    /* 0x4E */ be<u8> field_0x52;
    /* 0x4F */ be<u8> field_0x53;
    /* 0x50 */ be<s16> field_0x54;
    /* 0x52 */ be<s16> field_0x56;
    /* 0x54 */ be<s16> field_0x58;
    /* 0x56 */ be<u8> field_0x5a;
    /* 0x57 */ be<u8> field_0x5b;
    /* 0x58 */ be<u8> field_0x5c;
    /* 0x59 */ be<u8> field_0x5d;
    /* 0x5A */ be<u8> field_0x5e;
    u8 mPadding5B[1];
    /* 0x5C */ be<s16> field_0x60;
    /* 0x5E */ be<s16> field_0x62;
    /* 0x60 */ be<s16> field_0x64;
    /* 0x62 */ be<s16> field_0x66;
    /* 0x64 */ be<s16> field_0x68;
    /* 0x66 */ be<s16> field_0x6a;
    /* 0x68 */ be<s16> field_0x6c;
    /* 0x6A */ be<s16> field_0x6e;
    /* 0x6C */ be<s16> field_0x70;
    /* 0x6E */ be<s16> field_0x72;
    /* 0x70 */ be<s16> field_0x74;
    /* 0x72 */ be<s16> field_0x76;
    /* 0x74 */ be<u8> field_0x78;
    u8 mPadding75[1];
    /* 0x76 */ be<s16> field_0x7a;
    /* 0x78 */ be<s16> field_0x7c;
    /* 0x7A */ be<s16> field_0x7e;
    /* 0x7C */ be<s16> field_0x80;
    /* 0x7E */ be<s16> field_0x82;
};
WWHD_SIZE(dMeter_message_HIO_c, 0x80);
WWHD_OFFSET(dMeter_message_HIO_c, mVtable, 0x0);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5, 0x4);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x9, 0x8);
WWHD_OFFSET(dMeter_message_HIO_c, field_0xd, 0xC);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x11, 0x10);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x15, 0x14);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x19, 0x18);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x1d, 0x1C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x21, 0x20);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x25, 0x24);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x29, 0x28);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2a, 0x29);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2b, 0x2A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2c, 0x2B);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2d, 0x2C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2e, 0x2D);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2f, 0x2E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x30, 0x2F);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x31, 0x30);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x32, 0x31);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x34, 0x32);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x36, 0x34);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x38, 0x36);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x3a, 0x38);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x3b, 0x39);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x3c, 0x3A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x40, 0x3C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x44, 0x40);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x46, 0x42);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x48, 0x44);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4a, 0x46);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4b, 0x47);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4c, 0x48);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4d, 0x49);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4e, 0x4A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4f, 0x4B);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x50, 0x4C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x51, 0x4D);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x52, 0x4E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x53, 0x4F);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x54, 0x50);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x56, 0x52);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x58, 0x54);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5a, 0x56);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5b, 0x57);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5c, 0x58);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5d, 0x59);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5e, 0x5A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x60, 0x5C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x62, 0x5E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x64, 0x60);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x66, 0x62);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x68, 0x64);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x6a, 0x66);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x6c, 0x68);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x6e, 0x6A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x70, 0x6C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x72, 0x6E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x74, 0x70);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x76, 0x72);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x78, 0x74);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x7a, 0x76);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x7c, 0x78);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x7e, 0x7A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x80, 0x7C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x82, 0x7E);

struct MeterWeaponMapHD {
    u8 mBeforeDisplay[0x14];
    be<u8> mHDDisplay;
};
WWHD_OFFSET(MeterWeaponMapHD, mHDDisplay, 0x14);
WWHD_SIZE(MeterWeaponMapHD, 0x15);
struct MeterWeaponGrafVtable {
    u8 mReserved[0x24];
    be<u32> mSetPort;
};
struct MeterWeaponGraf {
    u8 mReserved[0xB8];
    gptr<MeterWeaponGrafVtable> mVtable;
    void setPort() { gabi::call_ptr(mVtable->mSetPort, this); }
};
WWHD_OFFSET(MeterWeaponGraf, mVtable, 0xB8);
WWHD_SIZE(MeterWeaponGraf, 0xBC);
WWHD_SIZE(MeterWeaponGrafVtable, 0x28);
struct MeterWeaponScreenHD {
    u8 mReserved[0xCC];
    be<u8> mScissor;
};
WWHD_OFFSET(MeterWeaponScreenHD, mScissor, 0xCC);
WWHD_SIZE(MeterWeaponScreenHD, 0xCD);
struct MeterWeaponClockArray {
    u8 mClock[3][0xC4];
};
WWHD_SIZE(MeterWeaponClockArray, 0x24C);

struct MeterWeaponDrawList;

struct MeterWeaponApi {
    // HD: this existing meter-TU vtable is owned by d_meter_02.
    enum : u32 { safeStringVtable = 0x1005107C };
    static void registerGlobal(void *);
    static void initializeHeaderByte(void *);
    static void initializeHeaderFlag(void *);
    static void constructDrawList(MeterWeaponDrawList *);
    static void constructArray(void *, u32, u32, u32);

    static u8 getAStatusForce() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mAStatusForce; }
    static bool eventHind();
    static meter_detail::StageInfo *stageInfo() {
        auto data = reinterpret_cast<meter_detail::StageData *>(
            &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mStageData);
        return gabi::at<meter_detail::StageInfo>(data->mVtable->getStag(data));
    }
    static void drawScreen(u32, MeterWeaponGraf *, f32, f32);
    static void destroyArray(void *, u32, u32, u32, u32, u32);
    enum : u32 { destroyClock = 0x0259CFB4, constructClock = 0x0259CF60 };
    static MeterWeaponGraf *getGrafPort() {
        return gabi::at<MeterWeaponGraf>(gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCurrentGrafPort);
    }
};
extern be<f32> &l_messageScale;
extern be<f32> &l_mapIconScale;
extern be<u8> &l_itemMoveFlag;
extern be<u8> &l_menuPause;
extern MeterWeaponClockArray &l_meterClock;

// HD: scratch slots retain GHS SafeString and integer-conversion effects.
struct MeterWeaponScratch {
    u8 mReserved[8];
    be<u8> mMoveFlag;
    u8 mPadding9[3];
    meter_detail::SafeStringView mNames[4];
    u8 mPadding2C[4];
    union {
        struct {
            be<u32> mConversionHigh, mConversionLow;
        };
        be<u8> mConversionBytes[8];
    };
    meter_detail::SafeStringView mStages[4];
};
WWHD_OFFSET(MeterWeaponScratch, mNames, 0xC);
WWHD_OFFSET(MeterWeaponScratch, mConversionHigh, 0x30);
WWHD_OFFSET(MeterWeaponScratch, mStages, 0x38);
WWHD_SIZE(MeterWeaponScratch, 0x58);
struct MeterWeaponEventHD {
    u8 mBeforeHind[0x5292];
    be<u8> mEventFlag;
    u8 mBeforeMask[0x52A6 - 0x5293];
    be<u16> mHindMask;
};
WWHD_OFFSET(MeterWeaponEventHD, mEventFlag, 0x5292);
WWHD_OFFSET(MeterWeaponEventHD, mHindMask, 0x52A6);
WWHD_SIZE(MeterWeaponEventHD, 0x52A8);
extern be<f64> &l_weaponUnsignedBias;
extern be<f64> &l_weaponSignedBias;
extern be<s16> &l_weaponMoveFrame;
extern be<s16> &l_weaponMoveStatus;
extern be<s16> &l_weaponAlphaFrame;
extern be<u8> &l_weaponMenuButton;
extern be<u8> &l_weaponMenuStatus;
extern be<u8> &l_weaponItemMode;
inline bool MeterWeaponApi::eventHind() {
    if (gabi::at<MeterWeaponEventHD>(dComIfGp_ea())->mEventFlag == 0)
        return false;
    return (gabi::at<MeterWeaponEventHD>(dComIfGp_ea())->mHindMask & 4) != 0;
}

// HD: the common header initializer uses one-byte constructors and GHS registration nodes.
struct MeterWeaponInitializeWords {
    be<u32> mWords[4];
};
struct MeterWeaponRegistration {
    u8 mData[0xC];
};
struct MeterWeaponRegistrations {
    MeterWeaponRegistration mEntries[7];
};
struct MeterWeaponHeaderBytes {
    be<u8> mFirst, mSecond;
};
struct MeterWeaponTextureState {
    be<f32> mRateX, mRateY, mScaleX, mScaleY, mScaleMax, mScaleMin;
};
struct MeterWeaponDrawList {
    be<u32> mVtable;
};
WWHD_SIZE(MeterWeaponInitializeWords, 0x10);
WWHD_SIZE(MeterWeaponRegistration, 0xC);
WWHD_SIZE(MeterWeaponRegistrations, 0x54);
WWHD_SIZE(MeterWeaponHeaderBytes, 2);
WWHD_SIZE(MeterWeaponTextureState, 0x18);
WWHD_SIZE(MeterWeaponDrawList, 4);
extern MeterWeaponInitializeWords &l_meterInitializeWords;
extern MeterWeaponRegistrations &l_meterRegistrations;
extern MeterWeaponHeaderBytes &l_meterHeaderBytes;
extern MeterWeaponTextureState &l_meterTextureState;
extern MeterWeaponDrawList &l_meter1;
extern MeterWeaponDrawList &l_meter2;
extern MeterWeaponDrawList &l_meterTekari;
extern dMeter_HIO_c &g_meterHIO;
extern dMeter_menuHIO_c &g_menuHIO;
extern dMeter_msg_HIO_c &g_msgHIO;
extern dMeter_message_HIO_c &g_messageHIO;
extern be<f32> &l_meterRateX;
extern be<f32> &l_meterRateY;
extern be<f32> &l_meterScale;
extern be<f32> &l_meterScaleMax;

namespace meter_weapon {
enum ActionStatus {
    dActStts_BLANK_e = 0,
    dActStts_SPEAK_e = 2,
    dActStts_RETURN_e = 7,
    dActStts_BID_e = 37,
    dActStts_CANCEL_e = 39,
    dActStts_ba_nageru__dupe_2D = 45,
    dActStts_ba_motu__dupe_2E = 46,
    dActStts_ba_sake__dupe_31 = 49,
    dActStts_sword_01 = 53,
    dActStts_boko_stick = 55,
    dActStts_hatchet = 56,
    dActStts_iron_club = 57,
    dActStts_longsword = 58,
    dActStts_spear = 59,
    dActStts_spear__dupe_3C = 60,
    dActStts_HIDDEN_e = 62,
};
}

// HD: creation retains its original 0x130-byte frame, four screen bounds, and copied tag tables.
struct MeterCreateScratch {
    u8 mPadding0[8];
    union {
        be<u32> mInteger;
        struct { be<u16> mIntegerHigh; be<u16> mIntegerLow; };
    };
    be<u32> mConversionLow;
    J2DPane::Bounds mScreenBounds[4];
    be<u32> mHeartTags[20];
    be<u32> mHeartShadowTags[20];
    u8 mPaddingF0[0x40];
};
WWHD_OFFSET(MeterCreateScratch, mInteger, 8);
WWHD_OFFSET(MeterCreateScratch, mIntegerLow, 0xA);
WWHD_OFFSET(MeterCreateScratch, mScreenBounds, 0x10);
WWHD_OFFSET(MeterCreateScratch, mHeartTags, 0x50);
WWHD_OFFSET(MeterCreateScratch, mHeartShadowTags, 0xA0);
WWHD_SIZE(MeterCreateScratch, 0x130);
struct MeterCreateScreen {
    u8 mPadding0[0xC8];
    be<u32> mVtable;
    be<u8> mShow;
    be<u32> mColor;
    u8 mPaddingD1[3];
};
WWHD_OFFSET(MeterCreateScreen, mVtable, 0xC8);
WWHD_OFFSET(MeterCreateScreen, mShow, 0xCC);
WWHD_OFFSET(MeterCreateScreen, mColor, 0xCD);
WWHD_SIZE(MeterCreateScreen, 0xD4);
struct MeterCreateHeapVtable {
    u8 mPadding0[0x34];
    be<u32> mAlloc;
};
struct MeterCreateHeap {
    u8 mPadding0[0xC];
    gptr<MeterCreateHeapVtable> mVtable;
    u32 alloc(u32 size, s32 alignment) {
        return gabi::call_ptr<u32>(mVtable->mAlloc, this, size, alignment);
    }
};
WWHD_OFFSET(MeterCreateHeapVtable, mAlloc, 0x34);
WWHD_SIZE(MeterCreateHeapVtable, 0x38);
WWHD_OFFSET(MeterCreateHeap, mVtable, 0xC);
WWHD_SIZE(MeterCreateHeap, 0x10);
struct MeterCreateMenuState {
    be<s32> dMenu_frame_timer;
    be<f32> texScaleX, texScaleY, texRateX, texRateY;
    be<u32> dMeter_windID;
    be<s16> dMeter_itemTimer;
    u8 mPadding1A[0x20];
    be<u8> menu_status_old, menu_status, subWinFlag, dMenu_pause, dMenu_menuButton;
    be<u8> dMeter_btn_chk, dMeter_auctionFlag, dMeter_itemNum, dMeter_place_name;
    be<u8> dMeter_itemMoveFlag, dMenu_menuFlag, mapAlpha, dMeter_itemMode, dMeter_collectMode, dMeter_windStatus;
    be<u8> rupy_soundSetFlag, rupy_soundOnFlag;
};
WWHD_OFFSET(MeterCreateMenuState, menu_status_old, 0x3A);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemMoveFlag, 0x43);
WWHD_SIZE(MeterCreateMenuState, 0x4B);
inline MeterCreateMenuState *meterCreateMenuState() {
    return gabi::at<MeterCreateMenuState>(0x101EA02C);
}
inline be<u8> &meterCreateBaitNum(s32 index) { return gabi::at<be<u8>>(0x1047A98C)[index]; }
inline be<u32> &meterCreateArrowTextureName(u32 type) { return gabi::at<be<u32>>(0x101EA07C)[type]; }
inline mapCtrlDisp_c *meterCreateMapControl() { return gabi::at<mapCtrlDisp_c>(0x1047AD0C); }
inline u32 meterCreateMenuArchive() { return gabi::load<u32>(dComIfGp_ea() + 0x5A40); }
inline u32 meterCreateItemIconArchive() { return gabi::load<u32>(dComIfGp_ea() + 0x5A84); }
inline be<u8> &meterCreateMagicFlags() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BEF); }
inline be<u8> &meterCreateWindCancelFlag() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BD0); }
inline be<u8> &meterCreateItemOpenFlag() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BEC); }
inline be<u8> &meterCreateTimerFlag() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BE0); }
inline dSv_info_c *meterCreateSave() { return dComIfGs_info(); }
// HD: the bag selector is inlined and accepts all byte values. Keep its original bounded ranges.
inline u8 meterCreateSelectedItem(u8 slot) {
    auto *player = &meterCreateSave()->mSavedata.mPlayer;
    if (slot < 21) return player->mPlayerItem.mItems[slot];
    if (slot >= 24 && slot < 32) return player->mBagItem.mBeast[slot - 24];
    if (slot >= 36 && slot < 44) return player->mBagItem.mBait[slot - 36];
    if (static_cast<u32>(slot - 48) < 8) return player->mBagItem.mReserve[slot - 48];
    return 0xFF;
}

u32 meterCreateExpHeap(u32);
u32 meterCreateAllocate(u32, u32, s32);
u32 meterCreatePaneConstruct(u32, u32, u32, u32, J2DPane::Bounds *);
void meterCreateScreenSet(u32, const char *, u32);
u32 meterCreateAssert(const char *, s32, const char *);
u32 meterCreateHIOChild(const char *, dMeter_map_HIO_c *);
void meterCreateSetPaneData(fopMsgM_pane_class *, u32, u32);
void meterCreateWalletChange(sub_meter_class *);
u32 meterCreateIsDungeonItemCompass(u32, u32);
void meterCreateCompassInit(sub_meter_class *);
void meterCreateHeartInit(sub_meter_class *);
void meterCreateXYItemChange(sub_meter_class *, u32);
f32 meterCreateRndF(f32);
void meterCreateMagicLength(sub_meter_class *, f32);
void meterCreateMagicChange(sub_meter_class *, f32);
void meterCreateMagicTransNowInit(sub_meter_class *);
void meterCreateSetNowAlphaZero(fopMsgM_pane_class *);
u32 meterCreateArrowType();
void meterCreateReadTexture(u32, u32, u32, u32, u32);
void meterCreateStoreTexture(u32, u32);
void meterCreateStockTimer();
void meterCreateSetStageLayer(sub_meter_class *);
u32 meterCreateGetTimer(dSv_player_item_record_c *);

inline u32 meterCreateDungeonMemory() { return gabi::ea(&meterCreateSave()->mMemory.mMembit); }
inline u8 meterCreateKeyNum() { return meterCreateSave()->mMemory.mMembit.mKeyNum; }
enum MeterCreateVtables { l_createMyScreenVtable = 0x100511A4, l_createScreenVtable = 0x1016E084 };

// HD: HUD creation initializes this additional UI mode byte.
inline be<u8> &meterCreateHudMode() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5C2F); }

WWHD_OFFSET(MeterCreateMenuState, dMenu_frame_timer, 0x0);
WWHD_OFFSET(MeterCreateMenuState, texScaleX, 0x4);
WWHD_OFFSET(MeterCreateMenuState, texScaleY, 0x8);
WWHD_OFFSET(MeterCreateMenuState, texRateX, 0xC);
WWHD_OFFSET(MeterCreateMenuState, texRateY, 0x10);
WWHD_OFFSET(MeterCreateMenuState, dMeter_windID, 0x14);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemTimer, 0x18);
WWHD_OFFSET(MeterCreateMenuState, menu_status, 0x3B);
WWHD_OFFSET(MeterCreateMenuState, subWinFlag, 0x3C);
WWHD_OFFSET(MeterCreateMenuState, dMenu_pause, 0x3D);
WWHD_OFFSET(MeterCreateMenuState, dMenu_menuButton, 0x3E);
WWHD_OFFSET(MeterCreateMenuState, dMeter_btn_chk, 0x3F);
WWHD_OFFSET(MeterCreateMenuState, dMeter_auctionFlag, 0x40);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemNum, 0x41);
WWHD_OFFSET(MeterCreateMenuState, dMeter_place_name, 0x42);
WWHD_OFFSET(MeterCreateMenuState, dMenu_menuFlag, 0x44);
WWHD_OFFSET(MeterCreateMenuState, mapAlpha, 0x45);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemMode, 0x46);
WWHD_OFFSET(MeterCreateMenuState, dMeter_collectMode, 0x47);
WWHD_OFFSET(MeterCreateMenuState, dMeter_windStatus, 0x48);
WWHD_OFFSET(MeterCreateMenuState, rupy_soundSetFlag, 0x49);
WWHD_OFFSET(MeterCreateMenuState, rupy_soundOnFlag, 0x4A);
WWHD_OFFSET(MeterCreateScratch, mIntegerHigh, 8);
WWHD_OFFSET(MeterCreateScratch, mConversionLow, 0xC);
