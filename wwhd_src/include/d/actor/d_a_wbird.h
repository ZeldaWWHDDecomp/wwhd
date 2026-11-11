/** Wind-direction demo actor: GameCube fields at 0x29C become WWHD fields at 0x3B8. */
#pragma once
#include "d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo_wether.h"
#include "gc_api/gc_api.h"
// HD: the angle helpers remain out of line; their guest representation is a signed halfword.
struct WbirdGlobeHD {
    be<f32> mRadius;
    be<s16> mAzimuth;
    be<s16> mInclination;
};
struct WbirdEnvLightHD {
    u8 _0[0xA24];
    be<u16> mTactWindAngleX;
    be<u16> mTactWindAngleY;
};
WWHD_OFFSET(WbirdEnvLightHD, mTactWindAngleX, 0xA24);
// HD: unsigned angles select an interleaved sine/cosine pair for every eight angle units.
struct WbirdSinCosTableHD {
    be<f32> mValues[8192][2];
};
// HD: the player's demo subobject begins at 0x420 (GameCube 0x304).
struct WbirdPlayerDemoHD {
    be<u16> mDemoType;
    be<s16> mDemoMoveAngle;
    u8 _4[0x10 - 4];
    be<u32> mDemoMode;
};
WWHD_SIZE(WbirdPlayerDemoHD, 0x14);
struct WbirdPlayerHD : fopAc_ac_c {
    u8 _3AC[0x420 - 0x3AC];
    WbirdPlayerDemoHD mDemo;
};
WWHD_OFFSET(WbirdPlayerHD, mDemo, 0x420);
WWHD_SIZE(WbirdPlayerHD, 0x434);
WWHD_SIZE(WbirdGlobeHD, 8);
WWHD_SIZE(WbirdEnvLightHD, 0xA28);
WWHD_SIZE(WbirdSinCosTableHD, 0x10000);
struct daWbird_c : fopAc_ac_c {
    void setAction(u8 action) {
        mAction = action;
    }
    void calcMtx();
    void setStartPos();
    BOOL CreateInit();
    cPhs_State create();
    void actionEnd();
    void actionMove();
    void actionSelect();

    /* 0x3AC */ u8 field_0x290[0x3B8 - 0x3AC]; /* GameCube 0x290 */
    /* 0x3B8 */ be<u8> mAction;
    /* 0x3B9 */ be<u8> mStartWindSe;  /* was field_0x29D: actionSelect 024DE55C sets it when the wind direction is decided; actionMove 024DE768 then plays JA_SE_TAKT_WIND_DEMO once and clears it (tww the same) */
    /* 0x3BA */ be<s16> mTimer;  /* was field_0x29E: CreateInit 024DE9F0 clears it; actionSelect 024DE55C counts it up (the wind window opens at 10); setStartPos 024DE334 sets 90 and actionMove 024DE768 counts it down: the bird moves while it is above 30, the tact wind starts at 20, the move ends at 0 */
    /* 0x3BC */ be<f32> mAccelY;  /* was field_0x2A0: setStartPos 024DE334 sets 1.0; actionMove 024DE768 adds it to speed.y every moving frame (tww the same) */
    /* 0x3C0 */ be<s16> mAngle;
    /* 0x3C2 */ be<s16> mEventIdx;
};
WWHD_OFFSET(daWbird_c, mAction, 0x3B8);
WWHD_OFFSET(daWbird_c, mEventIdx, 0x3C2);
WWHD_SIZE(daWbird_c, 0x3C4);
