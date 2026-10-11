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
    /* 0x3B4 */ gptr<J3DModel> field_0x298;
    /* 0x3B8 */ cXyz field_0x29C;   /* top position */
    /* 0x3C4 */ cXyz field_0x2A8;   /* model scale */
    /* 0x3D0 */ mDoExt_brkAnm mBrk; /* mDoExt_brkAnm (HD 0x78, frame ctrl first) */
    /* 0x448 */ mDoExt_btkAnm mBtk;
    /* 0x4BC */ be<f32> field_0x2E0; /* brk frame */
    /* 0x4C0 */ be<s32> field_0x2E4;
    /* 0x4C4 */ be<u8> field_0x2E8; /* type */
    /* 0x4C5 */ be<u8> field_0x2E9; /* draw */
    /* 0x4C6 */ be<u8> field_0x2EA;
    /* 0x4C7 */ u8 _4C7;
    /* 0x4C8 */ be<u32> field_0x2EC;
    /* 0x4CC */ be<u32> field_0x2F0;
    /* 0x4D0 */ dPa_followEcallBack field_0x2F4;
    /* 0x4E4 */ dPa_followEcallBack field_0x308;
    /* 0x4F8 */ LIGHT_INFLUENCE field_0x31C;
    /* 0x51C */ be<f32> field_0x33C;
};
WWHD_OFFSET(daArrow_Lighteff_c, field_0x298, 0x3B4);
WWHD_OFFSET(daArrow_Lighteff_c, mBtk, 0x448);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2E0, 0x4BC);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2F4, 0x4D0);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x31C, 0x4F8);
WWHD_SIZE(daArrow_Lighteff_c, 0x520);

// HD: a partial view of the parent arrow; no competing shared daArrow_c definition.
struct ArrowLightArrow : fopAc_ac_c {
    enum ArrowType { TYPE_NORMAL = 0, TYPE_FIRE = 1, TYPE_ICE = 2, TYPE_LIGHT = 3 };
    be<u8> mbSetByZelda;
    u8 _3AD[0x7CC - 0x3AD];
    Mtx34 field_0x6b4;
    be<u8> field_0x6e4;
    u8 _7FD;
    csXyz field_0x6e6;
    bool isSetByZelda() { return mbSetByZelda != 0; }
};
WWHD_OFFSET(ArrowLightArrow, mbSetByZelda, 0x3AC);
WWHD_OFFSET(ArrowLightArrow, field_0x6b4, 0x7CC);
WWHD_OFFSET(ArrowLightArrow, field_0x6e4, 0x7FC);
WWHD_OFFSET(ArrowLightArrow, field_0x6e6, 0x7FE);
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
WWHD_OFFSET(daArrow_Lighteff_c, field_0x29C, 0x3B8);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2A8, 0x3C4);
WWHD_OFFSET(daArrow_Lighteff_c, mBrk, 0x3D0);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2E4, 0x4C0);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2E8, 0x4C4);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2E9, 0x4C5);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2EA, 0x4C6);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2EC, 0x4C8);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2F0, 0x4CC);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x308, 0x4E4);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x33C, 0x51C);
