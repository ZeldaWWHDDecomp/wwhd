/* himo2_class (Grappling Hook rope), WWHD layout. 
 *
 * GameCube -> WWHD: fopAc_ac_c +0x11C up to m1F30. mDoExt_3DlineMat1_c grew from 0x3C to 0x188
 * (constructor 025EB82C), so m1F6C..m1FD8 shift and dCcD_Stts m2014 is at 0x2514. HD keeps only
 * 100 entries of m218C (GameCube 200). After mpHookModel HD inserts four mDoExt_J3DModelPacketS
 * (0xB0 each, constructor 02080404), so the members from m24B4 on are +0x630. Size 0x2D68.
 * Offsets measured from daHimo2_Create, CallbackCreateHeap, s_a_d_sub and himo2_bg_check. */
#pragma once
#include "gc_api/gc_api.h"

struct himo2_xyzWords_c {
    be<u32> x, y, z;
};
WWHD_SIZE(himo2_xyzWords_c, 0xC);
struct himo2_safeStringVtable_c {
    u8 _00[0x14];
    be<u32> assureTermination;
};
WWHD_OFFSET(himo2_safeStringVtable_c, assureTermination, 0x14);
WWHD_SIZE(himo2_safeStringVtable_c, 0x18);
struct himo2_playerVtable_c {
    u8 _00[0x14];
    be<u32> getLeftHandMatrix;
    u8 _18[4];
    be<u32> getRightHandMatrix;
    u8 _20[0x1C];
    be<u32> checkPlayerGuard;
    u8 _40[0x0C];
    be<u32> checkPlayerFly;
    u8 _50[0x7C];
    be<u32> checkPlayerNoDraw;
    u8 _D0[0x0C];
    be<u32> checkRopeReadyAnime;
};
WWHD_OFFSET(himo2_playerVtable_c, getLeftHandMatrix, 0x14);
WWHD_OFFSET(himo2_playerVtable_c, getRightHandMatrix, 0x1C);
WWHD_OFFSET(himo2_playerVtable_c, checkPlayerGuard, 0x3C);
WWHD_OFFSET(himo2_playerVtable_c, checkPlayerFly, 0x4C);
WWHD_OFFSET(himo2_playerVtable_c, checkPlayerNoDraw, 0xCC);
WWHD_OFFSET(himo2_playerVtable_c, checkRopeReadyAnime, 0xDC);
WWHD_SIZE(himo2_playerVtable_c, 0xE0);
inline himo2_playerVtable_c* himo2_playerVtable(fopAc_ac_c* player) {
    return gabi::at<himo2_playerVtable_c>(player->__vtbl);
}
struct himo2_attention_c {
    u8 _00[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(himo2_attention_c, mFlags, 0x20);
WWHD_SIZE(himo2_attention_c, 0x24);
// HD: the guide particle's draw-status word is later than in the GameCube emitter.
struct himo2_guideEmitter_c {
    u8 _00[0x254];
    be<u32> mStatus;
};
WWHD_OFFSET(himo2_guideEmitter_c, mStatus, 0x254);
WWHD_SIZE(himo2_guideEmitter_c, 0x258);
extern gptr<himo2_guideEmitter_c>& l_guideEmitter;
struct himo2_bk_c : fopAc_ac_c {
    u8 _3AC[0x4A0 - 0x3AC];
    struct {
        be<s16> mMode, mAction;
    } dr;
};
WWHD_OFFSET(himo2_bk_c, dr, 0x4A0);
WWHD_SIZE(himo2_bk_c, 0x4A4);

// HD: the rope impact effect also sets the emitter's volume sweep.
struct himo2_particle_c {
    u8 _00[0x34];
    /* 0x34 */ be<f32> mRate;
    u8 _38[0x20];
    /* 0x58 */ be<f32> mSpread;
    /* 0x5C */ be<s32> mMaxFrame;
    u8 _60[0x0C];
    /* 0x6C */ be<f32> mInitialVelAxis;
    u8 _70[0x0C];
    /* 0x7C */ be<f32> mVolumeSweep;
};
WWHD_OFFSET(himo2_particle_c, mRate, 0x34);
WWHD_OFFSET(himo2_particle_c, mSpread, 0x58);
WWHD_OFFSET(himo2_particle_c, mMaxFrame, 0x5C);
WWHD_OFFSET(himo2_particle_c, mInitialVelAxis, 0x6C);
WWHD_OFFSET(himo2_particle_c, mVolumeSweep, 0x7C);
WWHD_SIZE(himo2_particle_c, 0x80);

// HD: camera-process view used by rope targeting and the boss sequences.
struct himo2_camera_c {
    u8 _00[0xDC];
    /* 0xDC */ cXyz eye;
    u8 _E8[0x248 - 0xE8];
    /* 0x248 */ u8 mCamera[1];
};
WWHD_OFFSET(himo2_camera_c, eye, 0xDC);
WWHD_OFFSET(himo2_camera_c, mCamera, 0x248);
WWHD_SIZE(himo2_camera_c, 0x249);

// Only the tww boss members used by the rope sequences are exposed here.
struct himo2_dr_c : fopAc_ac_c {
    u8 _3AC[0x526 - 0x3AC];
    /* 0x526 */ be<u8> unk_40A;
    u8 _527[9];
    /* 0x530 */ be<f32> unk_414;
    u8 _534[0x5D6 - 0x534];
    /* 0x5D6 */ be<s16> unk_4BA;
    u8 _5D8[0x0E];
    /* 0x5E6 */ be<s16> unk_4CA;
    u8 _5E8[0x40];
    /* 0x628 */ be<u8> unk_50C;
    u8 _629[3];
};
WWHD_OFFSET(himo2_dr_c, unk_40A, 0x526);
WWHD_OFFSET(himo2_dr_c, unk_414, 0x530);
WWHD_OFFSET(himo2_dr_c, unk_4BA, 0x5D6);
WWHD_OFFSET(himo2_dr_c, unk_4CA, 0x5E6);
WWHD_OFFSET(himo2_dr_c, unk_50C, 0x628);
WWHD_SIZE(himo2_dr_c, 0x62C);
struct himo2_btd_c : fopAc_ac_c {
    u8 _3AC[0x63D4 - 0x3AC];
    /* 0x63D4 */ be<u8> m6190;
    u8 _63D5[0x7059 - 0x63D5];
    /* 0x7059 */ be<u8> m6E15;
    u8 _705A[2];
};
WWHD_OFFSET(himo2_btd_c, m6190, 0x63D4);
WWHD_OFFSET(himo2_btd_c, m6E15, 0x7059);
WWHD_SIZE(himo2_btd_c, 0x705C);

enum himo2_demo_state_e {
    HIMO2_DEMO_REQUEST = -1,
    HIMO2_DEMO_INACTIVE = 0,
    HIMO2_DEMO_COIL = 2,
    HIMO2_DEMO_SET_ROCK = 3,
    HIMO2_DEMO_DROP_ROCK = 4,
    HIMO2_DEMO_CONTROL = 5,
    HIMO2_DEMO_JUMP = 6,
    HIMO2_DEMO_RESET = 7,
    HIMO2_DEMO_BOSS_FALL = 8,
    HIMO2_DEMO_BOSS_END = 9,
};

enum himo2_state_e {
    HIMO2_WAIT = 0,
    HIMO2_SPIN = 1,
    HIMO2_LAUNCH_FREE = 2,
    HIMO2_LAUNCH_TARGET = 3,
    HIMO2_HOOK_BODY = 4,
    HIMO2_PULL_BACK = 5,
    HIMO2_MAX_LENGTH = 8,
    HIMO2_REBOUND = 9,
    HIMO2_HOOKED = 10,
    HIMO2_HANG = 11,
};

struct himo2_s {
    /* 0x00 */ u8 m00[0x0C];
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ be<s16> m1C;
    /* 0x1E */ be<s16> m1E;
    /* 0x20 */ u8 m20[4];
};
WWHD_SIZE(himo2_s, 0x24);

// HD: rope line entries have a 0x10 stride; the shared line API uses 0x18.
struct himo2_line_c {
    gptr<cXyz> mpPositions;
    u8 _04[0x0C];
};
WWHD_SIZE(himo2_line_c, 0x10);
struct himo2_lineMat_c {
    u8 _00[0x130];
    /* 0x130 */ be<u32> __vtbl;
    u8 _134[0x50];
    /* 0x184 */ gptr<himo2_line_c> mpLines;
};
WWHD_OFFSET(himo2_lineMat_c, __vtbl, 0x130);
WWHD_OFFSET(himo2_lineMat_c, mpLines, 0x184);
WWHD_SIZE(himo2_lineMat_c, 0x188);

// HD: the added packet's lighting pointer is at 0xA4.
struct himo2_modelPacket_c {
    u8 _00[0xA4];
    /* 0xA4 */ gptr<dKy_tevstr_c> mpTevStr;
    u8 _A8[8];
};
WWHD_OFFSET(himo2_modelPacket_c, mpTevStr, 0xA4);
WWHD_SIZE(himo2_modelPacket_c, 0xB0);

struct himo2_class : fopAc_ac_c {
    BOOL setTargetPos(cXyz*, be<f32>*, be<f32>*);
    /* 0x03AC */ u8 _3AC[0x3B8 - 0x3AC];
    /* 0x03B8 */ be<s16> m029C;
    /* 0x03BA */ be<s16> m029E;
    /* 0x03BC */ be<s16> m02A0;
    /* 0x03BE */ be<s16> m02A2;
    /* 0x03C0 */ be<s16> m02A4;
    /* 0x03C2 */ u8 _3C2[0x3D0 - 0x3C2];
    /* 0x03D0 */ cXyz m02B4;
    /* 0x03DC */ u8 _3DC[0x3E8 - 0x3DC];
    /* 0x03E8 */ be<s32> m02CC;
    /* 0x03EC */ u8 _3EC[0x3F4 - 0x3EC];
    /* 0x03F4 */ be<u32> m02D8;
    /* 0x03F8 */ be<s32> m02DC;
    /* 0x03FC */ be<s8> m02E0;
    /* 0x03FD */ u8 _3FD[3];
    /* 0x0400 */ be<f32> m02E4;
    /* 0x0404 */ u8 _404[4];
    /* 0x0408 */ cXyz m02EC[2];
    /* 0x0420 */ u8 _420[4];
    /* 0x0424 */ be<s16> m0308;
    /* 0x0426 */ u8 _426[0x42C - 0x426];
    /* 0x042C */ himo2_s m0310[100];
    /* 0x123C */ himo2_s m1120[100];
    /* 0x204C */ himo2_lineMat_c m1F30;
    /* 0x21D4 */ be<s32> m1F6C;
    /* 0x21D8 */ be<f32> m1F70[5];
    /* 0x21EC */ cXyz m1F84;
    /* 0x21F8 */ be<s16> m1F90;
    /* 0x21FA */ be<s16> m1F92;
    /* 0x21FC */ be<s16> m1F94;
    /* 0x21FE */ u8 _21FE[2];
    /* 0x2200 */ himo2_lineMat_c m1F98;
    /* 0x2388 */ be<f32> m1FD4;
    /* 0x238C */ himo2_lineMat_c m1FD8;
    /* 0x2514 */ dCcD_Stts m2014;
    /* 0x2550 */ dCcD_Sph m2050;
    /* 0x267C */ gptr<fopAc_ac_c> m217C;
    /* 0x2680 */ be<u32> m2180;
    /* 0x2684 */ be<f32> m2184;
    /* 0x2688 */ be<f32> m2188;
    /* 0x268C */ gptr<fopAc_ac_c> m218C[100]; /* HD: 100 (GameCube 200) */
    /* 0x281C */ be<u8> m24AC;
    /* 0x281D */ u8 _281D[3];
    /* 0x2820 */ gptr<J3DModel> mpHookModel;
    /* 0x2824 */ himo2_modelPacket_c mPackets[4]; /* HD: mDoExt_J3DModelPacketS x4 */
    /* 0x2AE4 */ be<f32> m24B4;
    /* 0x2AE8 */ be<f32> m24B8;
    /* 0x2AEC */ be<s32> m24BC;
    /* 0x2AF0 */ be<s32> m24C0;
    /* 0x2AF4 */ be<f32> m24C4;
    /* 0x2AF8 */ be<s16> m24C8;
    /* 0x2AFA */ be<s16> m24CA;
    /* 0x2AFC */ cXyz m24CC;
    /* 0x2B08 */ be<s8> m24D8;
    /* 0x2B09 */ be<s8> m24D9;
    /* 0x2B0A */ u8 _2B0A[2];
    /* 0x2B0C */ cXyz m24DC;
    /* 0x2B18 */ cXyz m24E8;
    /* 0x2B24 */ be<f32> m24F4;
    /* 0x2B28 */ be<f32> m24F8;
    /* 0x2B2C */ be<f32> m24FC;
    /* 0x2B30 */ be<f32> m2500;
    /* 0x2B34 */ cXyz m2504;
    /* 0x2B40 */ be<s16> m2510;
    /* 0x2B42 */ be<s16> m2512;
    /* 0x2B44 */ be<f32> m2514;
    /* 0x2B48 */ be<f32> m2518;
    /* 0x2B4C */ be<s8> m251C;
    /* 0x2B4D */ u8 _2B4D[3];
    /* 0x2B50 */ be<f32> m2520;
    /* 0x2B54 */ cXyz m2524;
    /* 0x2B60 */ be<f32> m2530;
    /* 0x2B64 */ dBgS_AcchCir m2534;
    /* 0x2BA4 */ dBgS_ObjAcch m2574;
};
WWHD_OFFSET(himo2_class, m02CC, 0x3E8);
WWHD_OFFSET(himo2_class, m0310, 0x42C);
WWHD_OFFSET(himo2_class, m1F30, 0x204C);
WWHD_OFFSET(himo2_class, m1F98, 0x2200);
WWHD_OFFSET(himo2_class, m1FD8, 0x238C);
WWHD_OFFSET(himo2_class, m2014, 0x2514);
WWHD_OFFSET(himo2_class, m2050, 0x2550);
WWHD_OFFSET(himo2_class, m218C, 0x268C);
WWHD_OFFSET(himo2_class, m24AC, 0x281C);
WWHD_OFFSET(himo2_class, mpHookModel, 0x2820);
WWHD_OFFSET(himo2_class, m24B4, 0x2AE4);
WWHD_OFFSET(himo2_class, m2500, 0x2B30);
WWHD_OFFSET(himo2_class, m2534, 0x2B64);
WWHD_OFFSET(himo2_class, m2574, 0x2BA4);
WWHD_SIZE(himo2_class, 0x2D68);

/* himo2HIO_c (HD: vtable after the members), l_himo2HIO at 0x1046469C */
struct himo2HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    /* 0x06 */ be<s16> m0A;
    /* 0x08 */ be<s16> m0C;
    /* 0x0A */ be<s16> m0E;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<f32> m1C;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<u32> __vtbl;
};
WWHD_SIZE(himo2HIO_c, 0x24);
extern himo2HIO_c& l_himo2HIO;
extern be<f32>& rope_scale;
extern gptr<himo2_btd_c>& himo2_btd;
extern gptr<himo2_dr_c>& himo2_dr;
// HD: the debug-register root's children start at 4; each child keeps the tww layout.
struct himo2_reg_childHIO_c {
    be<u32> __vtbl;
    be<f32> mFloatRegs[30];
    be<s16> mShortRegs[10];
};
WWHD_OFFSET(himo2_reg_childHIO_c, mFloatRegs, 4);
WWHD_OFFSET(himo2_reg_childHIO_c, mShortRegs, 0x7C);
WWHD_SIZE(himo2_reg_childHIO_c, 0x90);
struct himo2_reg_HIO_c {
    u8 _00[4];
    himo2_reg_childHIO_c mChild[22];
};
WWHD_OFFSET(himo2_reg_HIO_c, mChild, 4);
WWHD_SIZE(himo2_reg_HIO_c, 0xC64);
extern himo2_reg_HIO_c& l_himo2Regs;

#define HIMO2_VTBL 0x10010EF4
#define HIMO2_SAFESTRING_VTBL 0x10010DEC

// HD: unit-local ABI bridges preserve full-word collision results and stack-copy arguments.
u32 himo2_GetOnePolyInfo(dBgS_Acch*, cBgS_PolyInfo*);
s32 himo2_GetAttributeCode(dBgS*, cBgS_PolyInfo*);
void himo2_SoundSet(cXyz*, s32, u32, s32);
void himo2_constructSphere(dCcD_Sph*);
void himo2_setupPacket(void*, s32);
void himo2_moveExternal(himo2_class*);
void himo2_destroyGroundCheck(dBgS_GndChk*, s32);
void himo2_setLinePacket(daPy_py_c*, himo2_modelPacket_c*, himo2_lineMat_c*);
void himo2_setHookPacket(daPy_py_c*, himo2_modelPacket_c*, J3DModel*);

u32 himo2_padTrigX(u32);
u32 himo2_padTrigY(u32);
u32 himo2_padTrigZ(u32);
f32 himo2_subStickY(u32);
f32 himo2_subStickX(u32);
void himo2_cameraStop(void*);
void himo2_cameraStart(void*);
void himo2_cameraTrim(void*, s32);
void himo2_cameraReset(void*, cXyz*, cXyz*);
void himo2_cameraSet(void*, cXyz*, cXyz*, s16, f32);
void himo2_customColorSet(u32, u32, f32);
void himo2_orderPotentialEvent(fopAc_ac_c*, u16, u16, u32);
void himo2_playerPosExternal(himo2_class*);
BOOL himo2_bgCheckExternal(himo2_class*);
u32 himo2_LockonTruth(u32);
u32 himo2_LockonTarget(u32, s32);
void himo2_destroyLineCheck(dBgS_LinChk*, s32);

void himo2_constructPacket(himo2_modelPacket_c*);
void himo2_destroyPacket(himo2_modelPacket_c*, s32);
void himo2_destroySphere(dCcD_Sph*, s32);
