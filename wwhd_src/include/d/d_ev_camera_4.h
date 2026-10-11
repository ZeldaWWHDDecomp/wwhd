#pragma once
#include "d/d_camera.h"

// HD: these two event-work variants are not described by the GameCube header.
namespace event_camera_lists {
struct FixedFramesWork {
    be<u8> mTimerSet;
    u8 _01[3];
    cXyz mCenter;
    cXyz mEye;
    gptr<cXyz> mpEyes;
    gptr<cXyz> mpCenters;
    gptr<be<f32>> mpFovys;
    be<f32> mFovy;
    gptr<fopAc_ac_c> mpRelActor;
    be<char> mRelUseMask[4];
    be<u32> mTimer;
    be<s32> mFrameCount;
};
WWHD_SIZE(FixedFramesWork, 0x3C);
WWHD_OFFSET(FixedFramesWork, mpCenters, 0x20);
WWHD_OFFSET(FixedFramesWork, mFrameCount, 0x38);
struct BSplineWork {
    gptr<cXyz> mpCenters;
    gptr<cXyz> mpEyes;
    gptr<be<f32>> mpFovys;
    be<u32> mTimer;
    be<s32> mKeyNum;
};
WWHD_SIZE(BSplineWork, 0x14);
enum SplineState : u32 { SPLINE_DONE = 3 };
struct BSplinePathView {
    be<s32> mFrame;
    be<s32> mKeyNum;
    be<s32> mDuration;
    be<u32> mState;
};
WWHD_SIZE(BSplinePathView, 0x10);
struct VectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(VectorWords, 0xC);
struct EventString {
    char text[16];
};

} // namespace event_camera_lists
using namespace event_camera_lists;

u32 cameraListAssert(const char*, s32, const char*);
u32 cameraListGetEvIntData(dCamera_c*, be<u32>*, const char*, s32);
u32 cameraListGetEvIntDataRequired(dCamera_c*, be<u32>*, const char*);
u32 cameraListGetEvStringData(dCamera_c*, char*, const char*, const char*);
fopAc_ac_c* cameraListGetEvActor(dCamera_c*, const char*);
bool cameraListLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32);
bool cameraListChkCamera(void*, cXyz*, cXyz*, f32, fopAc_ac_c*, fopAc_ac_c*);
void cameraListSplineInit(BSplinePathView*, u32, u32);
u32 cameraListSplineStep(BSplinePathView*);
void cameraListSplineCalcXyz(BSplinePathView*, cXyz*, cXyz*);
f32 cameraListSplineCalcFloat(BSplinePathView*, be<f32>*);
void cameraListSubtract(cXyz*, cXyz*, cXyz*);
