/* Background query helpers: typed GameCube fields, verified HD stack layouts.
 * Local views keep the helper's temporary objects separate from caller-side bindings. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
struct FuncPolyInfo {
    be<u16> mPolyIndex, mBgIndex;
    gptr<cBgW> mpBgW;
    be<u32> mActorId, __vtbl;
};
struct FuncBgCheck {
    gptr<void> mPolyPassChk, mGrpPassChk;
    be<u32> mActorPid;
    be<u8> mSameActorChk;
    u8 padding[3];
    be<u32> __vtbl;
};
struct FuncPolyPass {
    be<u32> __vtbl;
    be<u8> mbObjThrough, mbCamThrough, mbLinkThrough, mbArrowThrough, mbBombThrough,
        mbBoomerangThrough, mbRopeThrough;
    u8 padding;
};
struct FuncGroupPass {
    enum { NORMAL_GRP = 1, WATER_GRP = 2 };
    be<u32> __vtbl, mGrp;
};
struct GroundCheck {
    /* 0x00 */ FuncBgCheck mCheck;
    /* 0x14 */ FuncPolyInfo mPolyInfo;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ be<u32> mFlag;
    /* 0x34 */ be<f32> mNowY;
    /* 0x38 */ be<u32> mWallPrecheck, mGndPrecheck;
    /* 0x40 */ FuncPolyPass mPolyPass;
    /* 0x4C */ FuncGroupPass mGrpPass;
};
struct SpecialGroupCheck {
    enum { FIND = 1, IN = 2 };
    /* 0x00 */ FuncPolyInfo mPolyInfo;
    /* 0x10 */ FuncBgCheck mCheck;
    /* 0x24 */ FuncPolyPass mPolyPass;
    /* 0x30 */ FuncGroupPass mGrpPass;
    /* 0x38 */ cXyz m_ground;
    /* 0x44 */ be<f32> m_roof;
    /* 0x48 */ be<f32> m_height;
    /* 0x4C */ be<u32> m_flags;
};
struct BackgroundSpace {
    u8 opaque;
};
struct BackgroundGameInfo {
    u8 save[0x12A0];
    BackgroundSpace mBgS;
};
struct BackgroundRoomStatus {
    u8 roomInfo[0x228];
    gptr<cBgW> mBgW;
};
struct Bounds {
    be<f32> top, bottom;
};
struct VectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(GroundCheck, 0x54);
WWHD_OFFSET(GroundCheck, mPolyInfo, 0x14);
WWHD_OFFSET(GroundCheck, m_pos, 0x24);
WWHD_OFFSET(GroundCheck, mPolyPass, 0x40);
WWHD_OFFSET(GroundCheck, mGrpPass, 0x4C);
WWHD_SIZE(SpecialGroupCheck, 0x50);
WWHD_OFFSET(SpecialGroupCheck, m_ground, 0x38);
WWHD_SIZE(BackgroundRoomStatus, 0x22C);
WWHD_OFFSET(BackgroundGameInfo, mBgS, 0x12A0);
void constructGroundCheck(GroundCheck *);
void destroyGroundCheck(GroundCheck *, u32);
BackgroundGameInfo *dComIfGp_getGameInfo();
f32 GroundCross(BackgroundSpace *, GroundCheck *);
u32 GetRoomId(BackgroundSpace *, FuncPolyInfo *);
u32 GetMtrlSndId(BackgroundSpace *, FuncPolyInfo *);
BOOL SplGrpChk(BackgroundSpace *, SpecialGroupCheck *);
void GetTopUnder(cBgW *, be<f32> *, be<f32> *);
void JUT_ASSERT_fail(const char *, s32, const char *);
BOOL daSea_ChkArea(f32, f32);
f32 daSea_calcWave(f32, f32);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
