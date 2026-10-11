/** Capsule-driven point wind, with tww fields and the HD guest ABI. */
#pragma once
#include "bindings.h"

namespace PointWind {
// HD: local views avoid changing the shared raw capsule and environment declarations.
struct cM3dGCpsS { cXyz mStart, mEnd; be<f32> mRadius; };
struct WIND_INFLUENCE {
    cXyz mPos, mDir;
    be<f32> mRadius, mStrength, field_0x20;
    be<s32> mRegistIdx;
    u8 mbConstant, padding[3];
};
}
class dPointWind_c {
public:
    void set_pwind_init(PointWind::cM3dGCpsS* pCps);
    void set_pwind_move();
    void set_pwind_delete();
    gptr<PointWind::cM3dGCpsS> mpCps;
    PointWind::WIND_INFLUENCE mWind;
};
WWHD_SIZE(PointWind::cM3dGCpsS, 0x1C);
WWHD_SIZE(PointWind::WIND_INFLUENCE, 0x2C);
WWHD_SIZE(dPointWind_c, 0x30);
WWHD_OFFSET(dPointWind_c, mWind, 4);
WWHD_OFFSET(dPointWind_c, mWind.mRadius, 0x1C);
WWHD_OFFSET(dPointWind_c, mWind.field_0x20, 0x24);

// HD: retain SafeString's emitted virtual calls, including the repeated termination call.
struct PointWindSafeStringVtable {
    u8 prefix[0x14];
    gabi::gfn assureTerminationImpl;
};
struct PointWindSafeString {
    gptr<const be<char>> mStringTop;
    gptr<const PointWindSafeStringVtable> __vtbl;
    void assureTermination() {
        gabi::call_ptr(__vtbl->assureTerminationImpl.get(), this);
    }
};
struct PointWindPlayView { u8 prefix[0x5134]; be<char> mStartStageName[8]; };
struct PointWindVectorWords { be<u32> x, y, z; };
struct PointWindHeaderStatics {
    be<f32> negativePi, positivePi;
    u8 firstObject, secondObject, padding[2];
    be<u32> zero[4];
};
WWHD_SIZE(PointWindSafeString, 8);
WWHD_OFFSET(PointWindSafeStringVtable, assureTerminationImpl, 0x14);
WWHD_OFFSET(PointWindPlayView, mStartStageName, 0x5134);
void dKyw_pwind_cylinder_set(PointWind::WIND_INFLUENCE*);
void dKyw_pntwind_cut(PointWind::WIND_INFLUENCE*);
void dKyr_get_vectle_calc(cXyz*, cXyz*, cXyz*);
f32 PSVECSquareDistance(cXyz*, cXyz*);
f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32);
void PointWindFirstHeaderObject_ct(u8*);
void PointWindSecondHeaderObject_ct(u8*);
