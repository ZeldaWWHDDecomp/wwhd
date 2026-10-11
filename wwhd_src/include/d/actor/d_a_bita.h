#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
// HD: narrow views of fields used by this actor; the GameCube layouts differ.
struct BitaBrkAnm : mDoExt_baseAnm {
    u8 _10[0x78 - 0x10];
    BOOL init(J3DModelData *, J3DAnmTevRegKey *, bool, s32, f32, s16, s16, bool, s32);
    void entry(J3DModelData *, f32);
};
BitaBrkAnm *BitaBrkAnm_ct(void *);
void fopAcM_SetMin(fopAc_ac_c *, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c *, f32, f32, f32);
struct BitaBossHD : fopAc_ac_c {
    u8 _3AC[0x705A - 0x3AC];
    be<s16> m6E16;
};
struct BitaCameraHD {
    u8 _0[0xDC];
    cXyz mEye;
};
struct BitaPlayHD {
    u8 _0[0x5AF8];
    gptr<BitaCameraHD> mCamera;
};
struct BitaModelDataHD : J3DModelData {
    u8 _14[0x48 - 0x14];
    gptr<J3DAnmTevRegKey> mpTevRegAnimation;
};
struct BitaBgWHD : dBgW {
    u8 _04[0xA8 - 4];
    be<u32> mCrrFunc;
};
struct bita_class : fopAc_ac_c {
    enum Mode { MODE_NORMAL = 0, MODE_DEAD = 1 };
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<J3DModel> mpModelEf;
    /* 0x3BC */ gptr<BitaBrkAnm> mpBrkAnm;
    /* 0x3C0 */ be<u8> mType;
    /* 0x3C1 */ be<u8> mPrmScale;
    /* 0x3C2 */ be<u8> mTimer;
    /* 0x3C3 */ be<u8> field_0x2a7;
    /* 0x3C4 */ be<u8> mMode;
    /* 0x3C5 */ be<u8> mSub;
    /* 0x3C6 */ u8 _3C6[2];
    /* 0x3C8 */ dCcD_Stts mStts;
    /* 0x404 */ dCcD_Cyl mCyl;
    /* 0x534 */ Mtx34 mMtx;
    /* 0x564 */ gptr<dBgW> mpBgW;
};
WWHD_OFFSET(bita_class, mStts, 0x3C8);
WWHD_OFFSET(bita_class, mCyl, 0x404);
WWHD_OFFSET(bita_class, mMtx, 0x534);
WWHD_SIZE(bita_class, 0x568);
