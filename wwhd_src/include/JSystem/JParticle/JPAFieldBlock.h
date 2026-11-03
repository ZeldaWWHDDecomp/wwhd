#pragma once
#include "wwhd.h"
#include "SSystem/SComponent/c_xyz.h"
struct JPAFieldBlockData {
    be<u32> mFlags;
    be<f32> mMag;
    be<f32> mMagRndm;
    be<f32> mMaxDist;
    cXyz mPos;
    cXyz mDir;
    be<f32> mVal1;
    be<f32> mVal2;
    be<f32> mVal3;
    be<f32> mFadeIn;
    be<f32> mFadeOut;
    be<f32> mEnTime;
    be<f32> mDisTime;
    be<u8> mCycle;
    u8 mPadding[3];
};
class JPAFieldBlockArc {
public:
    u8 getType();
    u32 getVelType();
    u32 getSttFlag();
    u32 getCycle();
    u32 getID();
    void getPos(cXyz* out);
    void getDir(cXyz* out);
    f32 getMag();
    f32 getMagRndm();
    f32 getMaxDist();
    f32 getVal1();
    f32 getVal2();
    f32 getVal3();
    f32 getFadeIn();
    f32 getFadeOut();
    f32 getEnTime();
    f32 getDisTime();
    gptr<void> mVtable;
    gptr<const JPAFieldBlockData> mpData;
};
WWHD_SIZE(JPAFieldBlockData,0x48);
WWHD_OFFSET(JPAFieldBlockData,mPos,0x10);
WWHD_OFFSET(JPAFieldBlockData,mDir,0x1c);
WWHD_OFFSET(JPAFieldBlockData,mCycle,0x44);
WWHD_OFFSET(JPAFieldBlockArc,mpData,4);
WWHD_SIZE(JPAFieldBlockArc,8);
