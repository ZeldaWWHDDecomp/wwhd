#include "d/d_ev_camera_4.h"
#include "gc_api/d/d_com_inf_game.h"

namespace event_camera_lists {
static EventString fixedCenters;
static EventString fixedEyes;
static EventString fixedFovys;
static EventString fixedTimer;
static EventString relativeMask;
static EventString relativeDefault;
static EventString relativeActor;
static EventString fixedFile;
static EventString fixedCentersAssert;
static EventString fixedEyesAssert;
static EventString fixedFovysAssert;
static EventString splineCenters;
static EventString splineEyes;
static EventString splineFovys;
static EventString splineTimer;
static EventString splineFile;
static EventString splineCentersAssert;
static EventString splineEyesAssert;
static EventString splineFovysAssert;
static f32 cameraCollisionRadius;

static s32 substanceNum(dCamera_c* camera, const char* name) {
    s32 staff = camera->mEventData.mStaffIdx;
    return dComIfGp_evmng_getMySubstanceNum(staff, name);
}
static void* substanceP(dCamera_c* camera, const char* name, s32 type) {
    s32 staff = camera->mEventData.mStaffIdx;
    return dComIfGp_evmng_getMySubstanceP(staff, name, type);
}
static void copyPoint(cXyz* destination, const cXyz* source) {
    // HD: preserve the interleaved integer loads and stores, including NaN payloads.
    auto* out = reinterpret_cast<VectorWords*>(destination);
    auto* in = reinterpret_cast<const VectorWords*>(source);
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}
static void SkipSmoother(dCamera_c* camera) {
    camera->m102 = 1;
    camera->m101 = 1;
    camera->m100 = 1;
}
/* inlined into fixedFramesEvCamera */
static void setViewFromWork(dCamera_c* camera, FixedFramesWork* work) {
    cXyz direction;
    copyPoint(&camera->mViewCache.mCenter, &work->mCenter);
    copyPoint(&camera->mViewCache.mEye, &work->mEye);
    camera->mViewCache.mFovy = work->mFovy;
    cameraListSubtract(&camera->mViewCache.mEye, &direction, &camera->mViewCache.mCenter);
    cSGlobe_Val(&camera->mViewCache.mDirection, &direction);
}

/* 0253A268 */
u32 fixedFramesEvCamera(u32 i_this) {
    auto* camera = gabi::at<dCamera_c>(i_this);
    auto* work = reinterpret_cast<FixedFramesWork*>(camera->mWork);
    if (camera->m11C == 0) {
        work->mFrameCount = 9999;
        s32 count = substanceNum(camera, fixedCenters.text);
        if (count == 0)
            return 1;
        auto* points = static_cast<cXyz*>(substanceP(camera, fixedCenters.text, 1));
        s32 previousCount = work->mFrameCount;
        work->mpCenters = points;
        if (previousCount > count)
            work->mFrameCount = count;
        count = substanceNum(camera, fixedEyes.text);
        if (count == 0)
            return 1;
        points = static_cast<cXyz*>(substanceP(camera, fixedEyes.text, 1));
        previousCount = work->mFrameCount;
        work->mpEyes = points;
        if (previousCount > count)
            work->mFrameCount = count;
        count = substanceNum(camera, fixedFovys.text);
        if (count == 0)
            return 1;
        auto* fovys = static_cast<be<f32>*>(substanceP(camera, fixedFovys.text, 0));
        previousCount = work->mFrameCount;
        work->mpFovys = fovys;
        if (previousCount > count)
            work->mFrameCount = count;
        work->mTimerSet =
            static_cast<u8>(cameraListGetEvIntData(camera, &work->mTimer, fixedTimer.text, 1));
        cameraListGetEvStringData(camera, reinterpret_cast<char*>(work->mRelUseMask),
                                  relativeMask.text, relativeDefault.text);
        auto* actor = cameraListGetEvActor(camera, relativeActor.text);
        auto* centers = work->mpCenters.get();
        work->mpRelActor = actor;
        if (!centers)
            cameraListAssert(fixedFile.text, 0xF06, fixedCentersAssert.text);
        if (!work->mpEyes)
            cameraListAssert(fixedFile.text, 0xF07, fixedEyesAssert.text);
        if (!work->mpFovys)
            cameraListAssert(fixedFile.text, 0xF08, fixedFovysAssert.text);
        cXyz center;
        cXyz eye;
        cXyz relative;
        cXyz collisionCenter;
        cXyz collisionEye;
        for (s32 i = 0; i < work->mFrameCount; ++i) {
            copyPoint(&center, work->mpCenters.get() + i);
            copyPoint(&eye, work->mpEyes.get() + i);
            actor = work->mpRelActor.get();
            if (actor && work->mRelUseMask[0] == 'o') {
                camera->relationalPos(&relative, actor, &center);
                copyPoint(&work->mCenter, &relative);
            } else {
                copyPoint(&work->mCenter, &center);
            }
            actor = work->mpRelActor.get();
            if (actor && work->mRelUseMask[1] == 'o') {
                camera->relationalPos(&relative, actor, &eye);
                copyPoint(&work->mEye, &relative);
            } else {
                copyPoint(&work->mEye, &eye);
            }
            work->mFovy = work->mpFovys.get()[i];
            if (cameraListLineBGCheck(camera, &work->mCenter, &work->mEye, 0x8F))
                continue;
            collisionCenter.x = work->mCenter.x;
            collisionCenter.y = work->mCenter.y;
            collisionCenter.z = work->mCenter.z;
            collisionEye.x = work->mEye.x;
            collisionEye.y = work->mEye.y;
            collisionEye.z = work->mEye.z;
            auto* player = camera->mpPlayerActor.get();
            actor = work->mpRelActor.get();
            auto* collision = dComIfG_Ccsp();
            if (!cameraListChkCamera(collision, &collisionCenter, &collisionEye,
                                     cameraCollisionRadius, player, actor))
                break;
        }
        SkipSmoother(camera);
    }
    setViewFromWork(camera, work);
    if (work->mTimerSet && camera->m11C < work->mTimer)
        return 0;
    return 1;
}

/* 0253A704 */
u32 bSplineEvCamera(u32 i_this) {
    auto* camera = gabi::at<dCamera_c>(i_this);
    auto* work = reinterpret_cast<BSplineWork*>(camera->mWork);
    auto* spline = reinterpret_cast<BSplinePathView*>(camera->mEventData.mSpline2DPath);
    if (camera->m11C == 0) {
        work->mKeyNum = 9999;
        s32 count = substanceNum(camera, splineCenters.text);
        if (count == 0)
            return 1;
        auto* points = static_cast<cXyz*>(substanceP(camera, splineCenters.text, 1));
        work->mpCenters = points;
        if (!points)
            cameraListAssert(splineFile.text, 0xF3E, splineCentersAssert.text);
        if (work->mKeyNum > count)
            work->mKeyNum = count;
        count = substanceNum(camera, splineEyes.text);
        if (count == 0)
            return 1;
        points = static_cast<cXyz*>(substanceP(camera, splineEyes.text, 1));
        work->mpEyes = points;
        if (!points)
            cameraListAssert(splineFile.text, 0xF48, splineEyesAssert.text);
        if (work->mKeyNum > count)
            work->mKeyNum = count;
        count = substanceNum(camera, splineFovys.text);
        if (count == 0)
            return 1;
        auto* fovys = static_cast<be<f32>*>(substanceP(camera, splineFovys.text, 0));
        work->mpFovys = fovys;
        if (!fovys)
            cameraListAssert(splineFile.text, 0xF52, splineFovysAssert.text);
        if (work->mKeyNum > count)
            work->mKeyNum = count;
        if (!cameraListGetEvIntDataRequired(camera, &work->mTimer, splineTimer.text))
            return 1;
        cameraListSplineInit(spline, work->mKeyNum, work->mTimer);
        SkipSmoother(camera);
    }
    if (!cameraListSplineStep(spline))
        return 1;
    cXyz position;
    cameraListSplineCalcXyz(spline, &position, work->mpCenters.get());
    copyPoint(&camera->mViewCache.mCenter, &position);
    cameraListSplineCalcXyz(spline, &position, work->mpEyes.get());
    copyPoint(&camera->mViewCache.mEye, &position);
    f32 fovy = cameraListSplineCalcFloat(spline, work->mpFovys.get());
    camera->mViewCache.mFovy = fovy;
    cameraListSubtract(&camera->mViewCache.mEye, &position, &camera->mViewCache.mCenter);
    cSGlobe_Val(&camera->mViewCache.mDirection, &position);
    return spline->mState == SPLINE_DONE ? 1 : 0;
}
} // namespace event_camera_lists
using namespace event_camera_lists;

/* WWHD glue (tools/verify/autoglue.py)
data fixedCenters 1004D3D4
data fixedEyes 1004D3EC
data fixedFovys 1004D3DC
data fixedTimer 1004D3E4
data relativeMask 1004D438
data relativeDefault 1004D3F4
data relativeActor 1004D444
data fixedFile 1004D3F8
data fixedCentersAssert 1004D408
data fixedEyesAssert 1004D418
data fixedFovysAssert 1004D428
data splineCenters 1004D450
data splineEyes 1004D468
data splineFovys 1004D458
data splineTimer 1004D460
data splineFile 1004D470
data splineCentersAssert 1004D480
data splineEyesAssert 1004D490
data splineFovysAssert 1004D4A0
data cameraCollisionRadius 1004CF80
ext 0273AA24 u32 cameraListAssert(const char*, s32, const char*)
ext 02530634 u32 cameraListGetEvIntData(dCamera_c*, be<u32>*, const char*, s32)
ext 02530418 u32 cameraListGetEvIntDataRequired(dCamera_c*, be<u32>*, const char*)
ext 02530998 u32 cameraListGetEvStringData(dCamera_c*, char*, const char*, const char*)
ext 02530B84 fopAc_ac_c* cameraListGetEvActor(dCamera_c*, const char*)
ext 024FCBE8 bool cameraListLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32)
ext 025187D8 bool cameraListChkCamera(void*, cXyz*, cXyz*, f32, fopAc_ac_c*, fopAc_ac_c*)
ext 025C0B18 void cameraListSplineInit(BSplinePathView*, u32, u32)
ext 025C0C80 u32 cameraListSplineStep(BSplinePathView*)
ext 025C0E38 void cameraListSplineCalcXyz(BSplinePathView*, cXyz*, cXyz*)
ext 025C0F30 f32 cameraListSplineCalcFloat(BSplinePathView*, be<f32>*)
ext 0250242C void dCamera_c::relationalPos(cXyz*, fopAc_ac_c*, cXyz*)
ext 0201ADE0 void cameraListSubtract(cXyz*, cXyz*, cXyz*)
*/
