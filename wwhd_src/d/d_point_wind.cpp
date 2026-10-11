/** Point-wind utility, WWHD. */
#include "d/d_point_wind.h"

// HD: guest bytes for the stage name "sea".
static const char l_sea[4] = {0x73, 0x65, 0x61, 0};
static const PointWindSafeStringVtable l_safeStringVtable;
static s8 l_stayNo;
static PointWindHeaderStatics l_headerStatics;

/* 025AB3F8 */
void dPointWind_c::set_pwind_init(PointWind::cM3dGCpsS* capsule) {
    mpCps = capsule;
    PointWind::cM3dGCpsS* cps = capsule;
    mWind.mPos.x = f32(cps->mStart.x);
    mWind.mPos.y = f32(cps->mStart.y);
    mWind.mPos.z = f32(cps->mStart.z);
    mWind.mDir.x = f32(cps->mEnd.x);
    mWind.mDir.y = f32(cps->mEnd.y);
    mWind.mDir.z = f32(cps->mEnd.z);
    f32 radius = f32(cps->mRadius);
    mWind.field_0x20 = 0.0f;
    mWind.mRadius = radius;
    mWind.mStrength = 1.0f;
    dKyw_pwind_cylinder_set(&mWind);
}

/* 025AB454 */
void dPointWind_c::set_pwind_move() {
    cXyz p0;
    cXyz p1;
    cXyz direction;
    cXyz sum;
    cXyz scaled;
    PointWindSafeString sea;
    PointWindSafeString stage;
    PointWind::cM3dGCpsS* cps = mpCps;
    p0.x = f32(cps->mStart.x);
    p0.y = f32(cps->mStart.y);
    p0.z = f32(cps->mStart.z);
    p1.x = f32(cps->mEnd.x);
    p1.y = f32(cps->mEnd.y);
    p1.z = f32(cps->mEnd.z);
    f32 radScale = 1.8f;
    bool animate = true;
    dKyr_get_vectle_calc(&p0, &p1, &direction);
    sea.mStringTop = reinterpret_cast<const be<char>*>(l_sea);
    sea.__vtbl = &l_safeStringVtable;
    bool special = false;
    PointWindPlayView* play = reinterpret_cast<PointWindPlayView*>(dComIfGp_get());
    stage.__vtbl = &l_safeStringVtable;
    stage.mStringTop = play->mStartStageName;
    sea.assureTermination();
    sea.assureTermination();
    // HD: fetch the stage virtual target before reading the sea string pointer.
    u32 terminate = stage.__vtbl->assureTerminationImpl.get();
    const be<char>* seaTop = sea.mStringTop;
    gabi::call_ptr(terminate, &stage);
    const be<char>* stageTop = stage.mStringTop;
    bool equal = seaTop == stageTop;
    if (!equal) {
        const be<char>* a = sea.mStringTop;
        const be<char>* b = stage.mStringTop;
        // HD: retain SafeString's bounded byte comparison.
        for (u32 count = 0x40001; count != 0; --count) {
            u32 x = u8(*a);
            u32 y = u8(*b);
            if (x != y) {
                break;
            }
            if (x == 0) {
                equal = true;
                break;
            }
            ++a;
            ++b;
        }
    }
    if (equal && l_stayNo == 4) {
        special = true;
    }
    if (special) {
        radScale = 11.0f;
        animate = false;
        cXyz_ml(&direction, &scaled, -100.0f);
        cXyz_pl(&p0, &sum, &scaled);
        // HD: integer word copies preserve NaN payloads and the emitted x/z/y store order.
        const PointWindVectorWords* sumWords = reinterpret_cast<const PointWindVectorWords*>(&sum);
        PointWindVectorWords* posWords = reinterpret_cast<PointWindVectorWords*>(&mWind.mPos);
        u32 x = sumWords->x;
        u32 z = sumWords->z;
        posWords->x = x;
        u32 y = sumWords->y;
        posWords->z = z;
        posWords->y = y;
    }
    const PointWindVectorWords* directionWords = reinterpret_cast<const PointWindVectorWords*>(&direction);
    PointWindVectorWords* dirWords = reinterpret_cast<PointWindVectorWords*>(&mWind.mDir);
    u32 dy = directionWords->y;
    cps = mpCps;
    dirWords->y = dy;
    u32 dx = directionWords->x;
    u32 dz = directionWords->z;
    dirWords->x = dx;
    dirWords->z = dz;
    f32 radius = f32(cps->mRadius) * radScale;
    mWind.field_0x20 = 0.0f;
    mWind.mRadius = radius;
    if (animate) {
        f32 currentRadius = mWind.mRadius;
        f32 half = currentRadius * 0.5f;
        cps = mpCps;
        f32 end = f32(cps->mEnd.x);
        cLib_addCalc(&mWind.mPos.x, end, 0.1f, currentRadius, half);
        currentRadius = mWind.mRadius;
        cps = mpCps;
        half = currentRadius * 0.5f;
        end = f32(cps->mEnd.y);
        cLib_addCalc(&mWind.mPos.y, end, 0.1f, currentRadius, half);
        currentRadius = mWind.mRadius;
        cps = mpCps;
        half = currentRadius * 0.5f;
        end = f32(cps->mEnd.z);
        cLib_addCalc(&mWind.mPos.z, end, 0.1f, currentRadius, half);
        f32 distance = PSVECSquareDistance(&mWind.mPos, &p1);
        distance = std_sqrtf(distance);
        if (distance < (f32)mWind.mRadius) {
            cps = mpCps;
            mWind.mPos.x = f32(cps->mStart.x);
            mWind.mPos.y = f32(cps->mStart.y);
            mWind.mPos.z = f32(cps->mStart.z);
        }
    }
}

/* 025AB710 */
void dPointWind_c::set_pwind_delete() {
    dKyw_pntwind_cut(&mWind);
}

/* 025AB718 */
static void __sinit_d_point_wind_cpp() {
    // HD: keep common-header initialization and constructor registrations in order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101EA5A8);
    l_headerStatics.negativePi = -3.141592741012573242f;
    l_headerStatics.positivePi = 3.141592741012573242f;
    PointWindFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101EA5B4);
    PointWindSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101EA5C0);
}

/* 025AB7AC */
static void SafeString_dtor(PointWindSafeString* string, u32 flags) {
    if (string != nullptr && (flags & 1)) {
        operator_delete(string);
    }
}

/* 025AB7C0 */
static void SafeString_assureTerminationImpl(PointWindSafeString* string) {
}

/* WWHD glue (tools/verify/autoglue.py):
data l_sea 10052558
data l_safeStringVtable 10052530
data l_stayNo 1047E6C8
data l_headerStatics 1047B344
ext 0257DE5C void dKyw_pwind_cylinder_set(PointWind::WIND_INFLUENCE*)
ext 0257D1B8 void dKyw_pntwind_cut(PointWind::WIND_INFLUENCE*)
ext 02563F64 void dKyr_get_vectle_calc(cXyz*, cXyz*, cXyz*)
ext 028E8DE8 f32 PSVECSquareDistance(cXyz*, cXyz*)
ext 0200ECD4 f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32)
ext 028ED6F8 void PointWindFirstHeaderObject_ct(u8*)
ext 028EAB2C void PointWindSecondHeaderObject_ct(u8*)
*/
