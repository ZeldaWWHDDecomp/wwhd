/* Special-group collision check: GameCube names and HD layout.
 * Local base views keep this unit independent of caller-only dBgS_WtrChk bindings. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"

struct SplPolyInfo {
    /* 0x00 */ be<u16> mPolyIndex;
    /* 0x02 */ be<u16> mBgIndex;
    /* 0x04 */ gptr<cBgW> mpBgW;
    /* 0x08 */ be<u32> mActorId;
    /* 0x0C */ be<u32> __vtbl;
};
struct SplBgCheck {
    /* 0x00 */ gptr<void> mPolyPassChk;
    /* 0x04 */ gptr<void> mGrpPassChk;
    /* 0x08 */ be<u32> mActorPid;
    /* 0x0C */ be<u8> mSameActorChk;
    u8 padding[3];
    /* 0x10 */ be<u32> __vtbl;
};
struct SplPolyPassCheck {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> mbObjThrough, mbCamThrough, mbLinkThrough, mbArrowThrough;
    /* 0x08 */ be<u8> mbBombThrough, mbBoomerangThrough, mbRopeThrough;
    u8 padding;
};
struct SplGroupPassCheck {
    enum { NORMAL_GRP = 1, WATER_GRP = 2 };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u32> mGrp;
};
class dBgS_SplGrpChk {
  public:
    enum { FIND = 1, IN = 2 };
    void Init();
    /* 0x00 */ SplPolyInfo mPolyInfo;
    /* 0x10 */ SplBgCheck mCheck;
    /* 0x24 */ SplPolyPassCheck mPolyPass;
    /* 0x30 */ SplGroupPassCheck mGrpPass;
    /* 0x38 */ cXyz m_ground;
    /* 0x44 */ be<f32> m_roof;
    /* 0x48 */ be<f32> m_height;
    /* 0x4C */ be<u32> m_flags;
};
WWHD_SIZE(SplPolyInfo, 0x10);
WWHD_SIZE(SplBgCheck, 0x14);
WWHD_SIZE(SplPolyPassCheck, 0xC);
WWHD_SIZE(SplGroupPassCheck, 8);
WWHD_OFFSET(dBgS_SplGrpChk, mCheck, 0x10);
WWHD_OFFSET(dBgS_SplGrpChk, mPolyPass, 0x24);
WWHD_OFFSET(dBgS_SplGrpChk, mGrpPass, 0x30);
WWHD_OFFSET(dBgS_SplGrpChk, m_ground, 0x38);
WWHD_OFFSET(dBgS_SplGrpChk, m_roof, 0x44);
WWHD_OFFSET(dBgS_SplGrpChk, m_height, 0x48);
WWHD_OFFSET(dBgS_SplGrpChk, m_flags, 0x4C);
WWHD_SIZE(dBgS_SplGrpChk, 0x50);
void *operator_new(u32);
void constructBgCheck(SplBgCheck *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
dBgS_SplGrpChk *constructSpecialGroupCheck(dBgS_SplGrpChk *);
