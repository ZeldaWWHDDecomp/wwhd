/* Environmental effect process. HD palette colors are bytes and the environment gained
 * additional palette parameters. Narrow accessed prefixes until the shared environment layout
 * is available. */
#pragma once
#include "SSystem/SComponent/c_phase.h"
#include "bindings.h"
#include "d/d_kankyo_data.h"
#include "f_op/f_op_kankyo.h"
class dKyeff_c : public kankyo_class {};
WWHD_SIZE(dKyeff_c, 0xFC);
struct kyVrboxView {
    u8 mPadding0[0x10];
    /* 0x0010 */ be<u8> mKumoColors[8];
    /* 0x0018 */ be<u8> mSkyColor[3];
    /* 0x001b */ be<u8> mUsoUmiColor[3];
    /* 0x001e */ be<u8> mKasumiMaeColor[3];
    u8 mPadding21[0x3];
    /* 0x0024 */ be<f32> mHdPaletteParams[4];
    /* 0x0034 */ be<f32> mUsoUmiAlpha;
};
WWHD_OFFSET(kyVrboxView, mKumoColors, 0x10);
WWHD_OFFSET(kyVrboxView, mSkyColor, 0x18);
WWHD_OFFSET(kyVrboxView, mUsoUmiColor, 0x1b);
WWHD_OFFSET(kyVrboxView, mKasumiMaeColor, 0x1e);
WWHD_OFFSET(kyVrboxView, mHdPaletteParams, 0x24);
WWHD_OFFSET(kyVrboxView, mUsoUmiAlpha, 0x34);
WWHD_SIZE(kyVrboxView, 0x38);
struct kyCameraView {
    u8 mPadding0[0xdc];
    /* 0x00dc */ be<f32> mLookat[6];
};
WWHD_OFFSET(kyCameraView, mLookat, 0xdc);
WWHD_SIZE(kyCameraView, 0xf4);
struct kyStageInfo {
    u8 mPadding0[0xc];
    /* 0x000c */ be<u32> mStageType;
};
WWHD_OFFSET(kyStageInfo, mStageType, 0xc);
WWHD_SIZE(kyStageInfo, 0x10);
struct kyStageView {
    /* 0x0000 */ gptr<gfn> mVtable;
};
WWHD_OFFSET(kyStageView, mVtable, 0x0);
WWHD_SIZE(kyStageView, 0x4);
struct kyGameInfoView {
    u8 mPadding0[0x5134];
    /* 0x5134 */ be<u8> mStartStageName[0x1C];
    /* 0x5150 */ kyStageView mStage;
    u8 mPadding5154[0x9a4];
    /* 0x5af8 */ gptr<kyCameraView> mCamera;
};
WWHD_OFFSET(kyGameInfoView, mStartStageName, 0x5134);
WWHD_OFFSET(kyGameInfoView, mStage, 0x5150);
WWHD_OFFSET(kyGameInfoView, mCamera, 0x5af8);
WWHD_SIZE(kyGameInfoView, 0x5afc);
struct kyEnvironmentView {
    u8 mPadding0[0xc];
    /* 0x000c */ gptr<kyVrboxView> mpVrboxInfo;
    /* 0x0010 */ gptr<dKyd_Schedule> mpSchejule;
    u8 mPadding14[0x9e8];
    /* 0x09fc */ be<f32> mWindVec[3];
    u8 mPaddinga08[0x10];
    /* 0x0a18 */ be<f32> mWindPower;
    u8 mPaddinga1c[0x98];
    /* 0x0ab4 */ be<u32> mThunderMode;
    u8 mPaddingab8[0xd4];
    /* 0x0b8c */ be<u8> mVrKasumiMaeColor[3];
    u8 mPaddingb8f[0x1];
    /* 0x0b90 */ be<u8> mVrSkyColor[4];
    /* 0x0b94 */ be<u8> mVrUsoUmiColor[4];
    /* 0x0b98 */ be<u8> mVrkumoColors[8];
    /* 0x0ba0 */ be<u8> mVrKasumiMaeOpaqueColor[4];
    u8 mPaddingba4[0x47c];
    /* 0x1020 */ be<f32> mCurTime;
    u8 mPadding1024[0xb0];
    /* 0x10d4 */ be<f32> mHdPaletteParams[5];
};
WWHD_OFFSET(kyEnvironmentView, mpVrboxInfo, 0xc);
WWHD_OFFSET(kyEnvironmentView, mpSchejule, 0x10);
WWHD_OFFSET(kyEnvironmentView, mWindVec, 0x9fc);
WWHD_OFFSET(kyEnvironmentView, mWindPower, 0xa18);
WWHD_OFFSET(kyEnvironmentView, mThunderMode, 0xab4);
WWHD_OFFSET(kyEnvironmentView, mVrKasumiMaeColor, 0xb8c);
WWHD_OFFSET(kyEnvironmentView, mVrSkyColor, 0xb90);
WWHD_OFFSET(kyEnvironmentView, mVrUsoUmiColor, 0xb94);
WWHD_OFFSET(kyEnvironmentView, mVrkumoColors, 0xb98);
WWHD_OFFSET(kyEnvironmentView, mVrKasumiMaeOpaqueColor, 0xba0);
WWHD_OFFSET(kyEnvironmentView, mCurTime, 0x1020);
WWHD_OFFSET(kyEnvironmentView, mHdPaletteParams, 0x10d4);
WWHD_SIZE(kyEnvironmentView, 0x10e8);
struct kyStageString {
    /* 0x0000 */ be<u32> mStringTop;
    /* 0x0004 */ be<u32> mVtable;
};
WWHD_OFFSET(kyStageString, mStringTop, 0x0);
WWHD_OFFSET(kyStageString, mVtable, 0x4);
WWHD_SIZE(kyStageString, 0x8);
kyEnvironmentView* dKy_getEnvironment();
kyGameInfoView* dKy_getGameInfo();
void dKyw_wether_draw();
void dKyw_wether_move();
void dKyw_wether_move_draw();
void mDoAud_setHour(s32 hour);
void mDoAud_setMinute(s32 minute);
dKyd_Schedule* dKyd_schejule_menu_getp();
kyVrboxView* dKyd_dmvrbox_menu_getp();
void kyAssertion(const char* file, u32 line, const char* condition);
void fopKy_destructor(dKyeff_c* process, s32 flags);
void fopKy_constructor(dKyeff_c* process);
void dKyw_wether_delete();
void dKyw_wether_init_hd();
u32 dKy_checkEventNightStop();
void dKyw_rain_set(u32 count);

namespace kyeff {
enum StageType { dStageType_OUTDOORS_e = 0, dStageType_MISC_e = 2, dStageType_SEA_e = 7 };
}
