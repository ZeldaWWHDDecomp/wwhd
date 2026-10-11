#ifndef D_SPLINE_PATH_H
#define D_SPLINE_PATH_H
#include "bindings.h"
#include "SSystem/SComponent/c_xyz.h"

class d2DBSplinePath {
public:
    void Init(s32 keyNum, s32 duration);
    bool Step();
    // HD: the vector return buffer is explicit and may require allocation.
    cXyz* Calc(cXyz* out, cXyz* values);
    // HD: disambiguate the scalar Calc overload for address-based verification glue.
    f32 CalcScalar(be<f32>* values);
    enum State { STOPPED = 0, STARTING = 1, RUNNING = 2, LAST = 3 };
    be<s32> mFrame;
    be<s32> mKeyNum;
    be<s32> mDuration;
    be<s32> mState;
    be<s32> mKeyNo;
    be<s32> mEnd;
    be<s32> field_0x18;
    be<f32> mStep;
    be<s32> field_0x20;
    be<f32> mCurveWeight[3];
    be<s32> mCurveKey[3];
    gptr<void> mUser;
    be<u32> __vtbl;
};
WWHD_SIZE(d2DBSplinePath, 0x44);
WWHD_OFFSET(d2DBSplinePath, mFrame, 0x00);
WWHD_OFFSET(d2DBSplinePath, mKeyNum, 0x04);
WWHD_OFFSET(d2DBSplinePath, mDuration, 0x08);
WWHD_OFFSET(d2DBSplinePath, mState, 0x0C);
WWHD_OFFSET(d2DBSplinePath, mKeyNo, 0x10);
WWHD_OFFSET(d2DBSplinePath, mEnd, 0x14);
WWHD_OFFSET(d2DBSplinePath, field_0x18, 0x18);
WWHD_OFFSET(d2DBSplinePath, mStep, 0x1C);
WWHD_OFFSET(d2DBSplinePath, field_0x20, 0x20);
WWHD_OFFSET(d2DBSplinePath, mCurveWeight, 0x24);
WWHD_OFFSET(d2DBSplinePath, mCurveKey, 0x30);
WWHD_OFFSET(d2DBSplinePath, mUser, 0x3C);
WWHD_OFFSET(d2DBSplinePath, __vtbl, 0x40);
struct SplineHeaderStatics {
    be<f32> negativePi, positivePi;
    u8 firstObject, secondObject, padding[2];
    be<u32> zero[4];
};
struct SplinePiPair { be<f32> negativePi, positivePi; };
void SplineFirstHeaderObject_ct(u8*);
void SplineSecondHeaderObject_ct(u8*);
void SplineZeroWords(void*, u32);
#endif
