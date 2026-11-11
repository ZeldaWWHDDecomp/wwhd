/* Elemental arrow glow, WWHD layout. The actor prefix expands by0x11C; embedded
 * HD animation controllers grow to0x78/0x74. Numbered fields retain exact tww names.
 * Private views describe HD renderer/emitter fields. */
#pragma once
#include "d/actor/d_a_player.h"
#include "gc_api/gc_api.h"
enum {
    dRes_INDEX_LINK_BDL_GARWFI00_e = 0x32,
    dRes_INDEX_LINK_BDL_GARWFI01_e = 0x33,
    dRes_INDEX_LINK_BDL_GARWG00_e = 0x34,
    dRes_INDEX_LINK_BRK_GARWFI00_e = 0x52,
    dRes_INDEX_LINK_BRK_GARWFI01_e = 0x53,
    dRes_INDEX_LINK_BRK_GARWG00_e = 0x54,
    dRes_INDEX_LINK_BTK_GARWFI00_e = 0x5E,
    dRes_INDEX_LINK_BTK_GARWFI01_e = 0x5F,
    dRes_INDEX_LINK_BTK_GARWG00_e = 0x60,
};
struct daArrow_Lighteff_c : fopAc_ac_c {
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    void brk_play();

    void setTopPos();
    void setPointLight();
    void delete_particle();
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ be<u32> field_0x290, field_0x294;
    /* 0x3B4 */ gptr<J3DModel> mpModel;  /* was field_0x298: CreateHeap 02056B54 creates the effect model (mDoExt_J3DModel__create); CreateInit 0205715C sets cullMtx to its matrix; set_mtx 02056EB8 sets its base scale and matrix (tww the same) */
    /* 0x3B8 */ cXyz mTopPos;  /* was field_0x29C: setTopPos 020570BC: 62 along the parent arrow's mModelMtx; CreateInit 0205715C makes the type particles there; setPointLight 0205777C puts the point light there (tww the same); top position */
    /* 0x3C4 */ cXyz mScale;  /* was field_0x2A8: CreateInit 0205715C sets 1 (or the particle manager's global scale); _execute 02057930 stretches z up to 20 and shrinks x / y to 0.5 while the arrow flies; set_mtx 02056EB8 sets it as the model's base scale (tww the same); model scale */
    /* 0x3D0 */ mDoExt_brkAnm mBrk; /* mDoExt_brkAnm (HD 0x78, frame ctrl first) */
    /* 0x448 */ mDoExt_btkAnm mBtk;
    /* 0x4BC */ be<f32> mBrkFrame;  /* was field_0x2E0: brk_play (in _execute 02057930) counts it toward mBrk's end frame (toward 5 while the camera attention status 0x20 is set) and sets mBrk's frame to it; CreateHeap 02056B54 clears it (tww the same); brk frame */
    /* 0x4C0 */ be<s32> mEndTimer;  /* was field_0x2E4: CreateInit 0205715C clears it; _execute 02057930 counts it up while the arrow is stuck (param 2 / 4) and deletes the effect at 0x3C (tww the same) */
    /* 0x4C4 */ be<u8> mArrowType;  /* was field_0x2E8: the arrow type: CreateHeap 02056B54 stores the parameter; CreateInit 0205715C picks the particles, setPointLight 0205777C the light colour, delete_particle 02057510 the alpha fade by it (tww the same); type */
    /* 0x4C5 */ be<u8> mDraw;  /* was field_0x2E9: CreateInit 0205715C sets 1; _execute 02057930 clears it once the arrow is stuck; _draw 0205763C draws only while set (tww the same); draw */
    /* 0x4C6 */ be<u8> mUseArrowEffect;  /* was field_0x2EA: _execute 02057930 sets 1 when it calls Link's onUseArrowEffect (the arrow is shot); _delete 020575A4 calls offUseArrowEffect while set; _create 02057478 clears it (tww the same) */
    /* 0x4C7 */ u8 _4C7;
    /* 0x4C8 */ be<u32> mArrowParam;  /* was field_0x2EC: _execute 02057930 stores the parent arrow's parameter (its state: 1 flying, 3 returned, other stuck) each frame; setPointLight 0205777C lights only while it is 0 or 1; CreateInit 0205715C clears it (tww the same) */
    /* 0x4CC */ be<u32> mPrevArrowParam;  /* was field_0x2F0: _execute 02057930 copies mArrowParam into it before reading the new one; the first frame of param 1 (it != 1) starts the arrow effect; CreateInit 0205715C clears it (tww the same) */
    /* 0x4D0 */ dPa_followEcallBack mParticleCb0;  /* was field_0x2F4: the follow callback of the first type particle (HINOKO00 / KIRAKIRA00 / TSUBU00) made by CreateInit 0205715C; delete_particle 02057510 fades and removes it (tww the same) */
    /* 0x4E4 */ dPa_followEcallBack mParticleCb1;  /* was field_0x308: the follow callback of the second type particle (REIKI00 / FLASH00) made by CreateInit 0205715C; delete_particle 02057510 fades and removes it (tww the same) */
    /* 0x4F8 */ LIGHT_INFLUENCE mLight;  /* was field_0x31C: setPointLight 0205777C fills this LIGHT_INFLUENCE (position mTopPos, the type colour, power mLightPower * 150, fluctuation 250) (tww the same) */
    /* 0x51C */ be<f32> mLightPower;  /* was field_0x33C: setPointLight 0205777C eases it toward 1 + rnd(0.2) while the arrow flies with a non-normal type, else 0; the light power is it * 150 (tww the same) */
};
WWHD_OFFSET(daArrow_Lighteff_c, mpModel, 0x3B4);
WWHD_OFFSET(daArrow_Lighteff_c, mBtk, 0x448);
WWHD_OFFSET(daArrow_Lighteff_c, mBrkFrame, 0x4BC);
WWHD_OFFSET(daArrow_Lighteff_c, mParticleCb0, 0x4D0);
WWHD_OFFSET(daArrow_Lighteff_c, mLight, 0x4F8);
WWHD_SIZE(daArrow_Lighteff_c, 0x520);

// HD: a partial view of the parent arrow; no competing shared daArrow_c definition.
struct ArrowLightArrow : fopAc_ac_c {
    enum ArrowType { TYPE_NORMAL = 0, TYPE_FIRE = 1, TYPE_ICE = 2, TYPE_LIGHT = 3 };
    be<u8> mbSetByZelda;
    u8 _3AD[0x7CC - 0x3AD];
    Mtx34 mModelMtx;  /* was field_0x6b4: as daArrow_c::mModelMtx; setTopPos 020570BC / set_mtx 02056EB8 copy it */
    be<u8> mFlewFar;  /* was field_0x6e4: as daArrow_c::mFlewFar; _execute 02057930 deletes the effect while it is set */
    u8 _7FD;
    csXyz mHitAngle;  /* was field_0x6e6: as daArrow_c::mHitAngle; _execute 02057930 copies it into current.angle */
    bool isSetByZelda() { return mbSetByZelda != 0; }
};
WWHD_OFFSET(ArrowLightArrow, mbSetByZelda, 0x3AC);
WWHD_OFFSET(ArrowLightArrow, mModelMtx, 0x7CC);
WWHD_OFFSET(ArrowLightArrow, mFlewFar, 0x7FC);
WWHD_OFFSET(ArrowLightArrow, mHitAngle, 0x7FE);
WWHD_SIZE(ArrowLightArrow, 0x804);
struct ArrowLightPlayer : daPy_py_c {
    void onUseArrowEffect() { onNoResetFlg1(daPyFlg1_USE_ARROW_EFFECT); }
    void offUseArrowEffect() { offNoResetFlg1(daPyFlg1_USE_ARROW_EFFECT); }
};
struct ArrowLightEmitter {
    u8 _00[0x220];
    cXyz mGlobalDynamicsScale;
    u8 _22C[0xC];
    cXyz mGlobalParticleScale;
    GXColor mGlobalPrmColor;
    void setGlobalAlpha(u8 alpha) { mGlobalPrmColor.a = alpha; }
    void setGlobalScale(f32 x, f32 y, f32 z) {
        mGlobalDynamicsScale.x = x;
        mGlobalDynamicsScale.y = y;
        mGlobalDynamicsScale.z = z;
        mGlobalParticleScale.x = x;
        mGlobalParticleScale.y = y;
        mGlobalParticleScale.z = z;
    }
};
WWHD_OFFSET(ArrowLightEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(ArrowLightEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(ArrowLightEmitter, mGlobalPrmColor, 0x244);
WWHD_SIZE(ArrowLightEmitter, 0x248);
struct ArrowLightParticles {
    u8 _00[0x110];
    be<f32> mGlobalScale;
};
WWHD_OFFSET(ArrowLightParticles, mGlobalScale, 0x110);
WWHD_SIZE(ArrowLightParticles, 0x114);
struct ArrowLightGraphics {
    be<u8> mMonotone;
};
WWHD_SIZE(ArrowLightGraphics, 1);
struct ArrowLightModelData {
    u8 _00[0x44];
    gptr<J3DAnmTevRegKey> mBrk;
    gptr<J3DAnmTextureSRTKey> mBtk;
};
WWHD_OFFSET(ArrowLightModelData, mBrk, 0x44);
WWHD_OFFSET(ArrowLightModelData, mBtk, 0x48);
WWHD_SIZE(ArrowLightModelData, 0x4C);
enum {
    JA_SE_OBJ_FIRE_ARROW_AMB = 0x106F,
    JA_SE_OBJ_ICE_ARROW_AMB = 0x1070,
    JA_SE_OBJ_LIGHT_ARROW_AMB = 0x1071,
    dCamAttnStts_00000020_e = 0x20
};
namespace arrow_light_particle {
enum {
    ID_IT_JN_ARWF_HINOKO00 = 0x299,
    ID_IT_JN_ARWI_KIRAKIRA00 = 0x29C,
    ID_IT_JN_ARWI_REIKI00 = 0x29D,
    ID_IT_JN_ARWG_TSUBU00 = 0x29F,
    ID_IT_JN_ARWG_FLASH00 = 0x2A0,
    ID_AK_JP_O_KAGEROU00 = 0x4004
};
}
// HD: base destruction retains the explicit GHS deleting flag argument.
void arrowLight_actor_dt(fopAc_ac_c*, s32);

WWHD_OFFSET(daArrow_Lighteff_c, field_0x290, 0x3AC);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x294, 0x3B0);
WWHD_OFFSET(daArrow_Lighteff_c, mTopPos, 0x3B8);
WWHD_OFFSET(daArrow_Lighteff_c, mScale, 0x3C4);
WWHD_OFFSET(daArrow_Lighteff_c, mBrk, 0x3D0);
WWHD_OFFSET(daArrow_Lighteff_c, mEndTimer, 0x4C0);
WWHD_OFFSET(daArrow_Lighteff_c, mArrowType, 0x4C4);
WWHD_OFFSET(daArrow_Lighteff_c, mDraw, 0x4C5);
WWHD_OFFSET(daArrow_Lighteff_c, mUseArrowEffect, 0x4C6);
WWHD_OFFSET(daArrow_Lighteff_c, mArrowParam, 0x4C8);
WWHD_OFFSET(daArrow_Lighteff_c, mPrevArrowParam, 0x4CC);
WWHD_OFFSET(daArrow_Lighteff_c, mParticleCb1, 0x4E4);
WWHD_OFFSET(daArrow_Lighteff_c, mLightPower, 0x51C);
