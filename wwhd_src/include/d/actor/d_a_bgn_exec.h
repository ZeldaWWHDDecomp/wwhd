#pragma once
#include "d/actor/d_a_bgn.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

// HD: private prefixes expose only fields touched by the inlined cutscene. The shared boss
// layout and TU-wide objects remain owned by d_a_bgn.h and their original source.
namespace bgn_exec {
struct Words {
    be<u32> x, y, z;
};
WWHD_SIZE(Words, 12);
inline Words *words(cXyz *p) { return gabi::at<Words>(gabi::ea(p)); }
struct Line {
    be<u32> mpPos, mpSize;
};
WWHD_SIZE(Line, 8);
struct Demo {
    /* 0x00 */ be<u16> mDemoType;
    /* 0x02 */ be<s16> mDemoMoveAngle;
    /* 0x04 */ be<s16> mTimer;
    /* 0x06 */ be<s16> mParam2;
    /* 0x08 */ be<u32> mParam0;
    /* 0x0C */ be<u32> mParam1;
    /* 0x10 */ be<u32> mDemoMode;
};
WWHD_OFFSET(Demo, mParam0, 8);
WWHD_OFFSET(Demo, mDemoMode, 0x10);
WWHD_SIZE(Demo, 0x14);
struct Player {
    u8 _000[0xB4];
    be<u32> __vtbl;
    u8 _0B8[0x420 - 0xB8];
    Demo mDemo;
};
WWHD_OFFSET(Player, __vtbl, 0xB4);
WWHD_OFFSET(Player, mDemo, 0x420);
WWHD_SIZE(Player, 0x434);
struct PlayerVtable {
    u8 _000[0xE4];
    be<u32> voiceStart;
    u8 _0E8[0x114 - 0xE8];
    be<u32> setPlayerPosAndAngle;
};
WWHD_OFFSET(PlayerVtable, voiceStart, 0xE4);
WWHD_OFFSET(PlayerVtable, setPlayerPosAndAngle, 0x114);
WWHD_SIZE(PlayerVtable, 0x118);
struct Phase2 {
    u8 _000[0x33BC];
    be<u8> m2ED0;
};
WWHD_OFFSET(Phase2, m2ED0, 0x33BC);
WWHD_SIZE(Phase2, 0x33BD);
struct Play {
    u8 _000[0x5B2C];
    be<u32> mPlayer;
};
WWHD_OFFSET(Play, mPlayer, 0x5B2C);
WWHD_SIZE(Play, 0x5B30);
// HD: dCamera_c grows from GameCube0x78C to0x8E0; this view needs only its identity.
struct Camera {
    u8 _000[0x8E0];
};
WWHD_SIZE(Camera, 0x8E0);
struct CameraProcess {
    u8 _000[0x248];
    Camera mCamera;
};
WWHD_OFFSET(CameraProcess, mCamera, 0x248);
WWHD_SIZE(CameraProcess, 0xB28);
inline fopAc_ac_c *bgn2() { return gabi::at<fopAc_ac_c>(bgn2_g()); }
inline fopAc_ac_c *bgn3() { return gabi::at<fopAc_ac_c>(bgn3_g()); }
enum : u32 {
    JA_SE_CM_BGN_T_MOUSE_OPEN = 0x5978,
    JA_SE_CM_BGN_T_MOUSE_CLOSE = 0x5979,
    JA_BGM_BGN_TARABA = 0x8000005A,
    JA_SE_CV_BGN_DIE = 0x4972,
    JA_SE_CM_BGN_LAST_EXPLODE = 0x5984,
    ID_AK_JN_SIBOUBAKUEN = 0x13,
    ID_AK_JN_SIBOUFLASH = 0x16,
    TYPE_ORIGINAL_e = 3,
    DEMO_UNK_029_e = 0x1D,
    DEMO_LOOKUP_e = 0x1A,
    DEMO_SURPRISED_e = 0x18
};
// HD: mapped identities avoid duplicate definitions of the original TU's static objects.
inline be<u32> &thunderInitialized() { return *gabi::at<be<u32>>(0x10461A54); }
inline cXyz *thunderScale() { return gabi::at<cXyz>(0x10461CC4); }
inline cXyz *center_pos() { return gabi::at<cXyz>(0x10461CD0); }
// HD: guest callback identities retain the original GHS function-pointer representation.
enum : u32 {
    s_b_sub = 0x0207D3AC,
    bgn2_s_sub = 0x0207B22C,
    bgn3_s_sub = 0x0207B27C,
    ki_del_sub = 0x0207CF04,
    ks_del_sub = 0x0207CF58
};
enum { fpcNm_KI_e = 0xD7, dEvtFlag_STAFF_ALL_e = 2, TEV_TYPE_BG2 = 3, UNK_3F10 = 0x3F10 };
} // namespace bgn_exec
void bgn_camera_Stop(bgn_exec::Camera *);
void bgn_camera_Start(bgn_exec::Camera *);
void bgn_camera_SetTrimSize(bgn_exec::Camera *, s32);
void bgn_camera_Set(bgn_exec::Camera *, cXyz *, cXyz *, s16, f32);
BOOL bgn_orderPotentialEvent(fopAc_ac_c *, u16, u16, u16);
void bgn_dMeter_mtrShow();
void bgn_monsSeStart(u32, cXyz *, u32, s32);
void bgn_JUTReport(s32, s32, const char *, s32);
void bgn_setNextStage(const char *, s16, s8, s8, f32, u32, s32, s8);
void bgn_exec_move(bgn_class *);
