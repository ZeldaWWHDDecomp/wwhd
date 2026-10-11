/* Ground collision check: GameCube base layout and HD size 0x40. */
#pragma once
#include "SSystem/SComponent/c_bg_s_chk.h"
struct cBgS_PolyInfo {
    /* 0x00 */ be<u16> mPolyIndex, mBgIndex;
    /* 0x04 */ gptr<cBgW> mpBgW;
    /* 0x08 */ be<u32> mActorId;
    /* 0x0C */ be<u32> __vtbl;
};
class cBgS_GndChk : public cBgS_Chk, public cBgS_PolyInfo {
  public:
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ be<u32> mFlag;
    /* 0x34 */ be<f32> mNowY;
    /* 0x38 */ be<u32> mWallPrecheck;
    /* 0x3C */ be<u32> mGndPrecheck;
};
WWHD_SIZE(cBgS_PolyInfo, 0x10);
WWHD_OFFSET(cBgS_GndChk, m_pos, 0x24);
WWHD_OFFSET(cBgS_GndChk, mFlag, 0x30);
WWHD_OFFSET(cBgS_GndChk, mNowY, 0x34);
WWHD_SIZE(cBgS_GndChk, 0x40);
