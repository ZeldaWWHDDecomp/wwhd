// WWHD camera-framing controls; constants remain named read-only image views.
#include "agl/VolumeDraw.h"
static f32 sAlignmentFirst;
static f32 sAlignmentMiddle;
static f32 sAlignmentLast;
void composeFramedCamera(sead::ViewCamera*, const sead::RenderVector2*, const sead::RenderVector2*, u32, f64, f64);
/* 027AAA2C */
static void configureAlignedCamera(sead::ViewCamera* camera, const agl::VolumeQuadSettings_l* settings) {
    u32 flags = settings->mFlags;
    f32 first = sAlignmentFirst;
    f32 middle = sAlignmentMiddle;
    f32 last = sAlignmentLast;
    f32 horizontal;
    if (flags & 0x10) horizontal = first;
    else if (flags & 8) horizontal = last;
    else horizontal = middle;
    f32 vertical = middle;
    if (flags & 1) vertical = first;
    else if (flags & 2) vertical = last;
    return gabi::tail_ptr<void>(gabi::guest_fn((const void*)composeFramedCamera), camera,
        &settings->mFirst, &settings->mSecond, 1,
        gabi::fpr_from_f32(horizontal), gabi::fpr_from_f32(vertical));
}
static f32 sCameraDistanceLower;
static f32 sCameraDistanceUpper;
static f32 sCameraDistanceSlope;
static f32 sCameraDistanceIntercept;
static f32 sCameraIncomingScale;
static f32 sCameraDistanceScale;
static f32 sCameraBasisScale;
static f32 sCameraPitchScale;
static f32 sCameraYawScale;
static sead::ViewVector sCameraWorldUp;
f32 normalizeFramingVector(sead::ViewVector*);
void crossFramingVectors(const sead::ViewVector*, const sead::ViewVector*, sead::ViewVector*);
void makeFramingQuaternion(Quaternion*, sead::ViewVector*, f32);
void multiplyFramingQuaternions(Quaternion*, const Quaternion*, const Quaternion*);
/* 027AA2EC */
void composeFramedCamera(sead::ViewCamera* camera, const sead::RenderVector2* angles,
        const sead::RenderVector2* offset, u32 planar,
        f64 distanceAdjustment, f64 verticalAdjustment) {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::ViewCamera* output = camera;
    gabi::FrameLocal<sead::ViewVector> right(0x2C);
    gabi::FrameLocal<sead::ViewVector> up(0x190);
    gabi::FrameLocal<sead::ViewVector> forward(0x1C8);
    camera->mTransform.copyRight(right.get());
    camera->mTransform.copyUp(up.get());
    camera->mTransform.copyForward(forward.get());
    f32 target[3] = {camera->mTarget.x.value, camera->mTarget.y.value, camera->mTarget.z.value};
    gabi::FrameLocal<sead::ViewVector> direction(0x8);
    direction->z.value = gabi::fsubs_ppc(target[2], camera->mEye.z.value);
    direction->x.value = gabi::fsubs_ppc(target[0], camera->mEye.x.value);
    direction->y.value = gabi::fsubs_ppc(target[1], camera->mEye.y.value);
    f32 length = normalizeFramingVector(direction.get());
    f32 lower = sCameraDistanceLower;
    f32 delta = gabi::fsubs_ppc(length, lower);
    f32 upper = sCameraDistanceUpper;
    f32 ratio = gabi::fdivs_ppc(delta, upper);
    f32 slope = sCameraDistanceSlope;
    f32 intercept = sCameraDistanceIntercept;
    f32 mapped = gabi::fmadds(slope, ratio, intercept);
    f32 incomingScale = gabi::fmuls_fpr(distanceAdjustment, gabi::fpr_from_f32(sCameraIncomingScale));
    f32 distanceScale = gabi::fmuls_ppc(sCameraDistanceScale, mapped);
    f32 radius = gabi::fmadds(incomingScale, distanceScale, length);
    if (upper > radius) {
        if (!(radius > lower)) radius = lower;
    } else radius = upper;
    f32 basisScale = sCameraBasisScale;
    gabi::FrameLocal<sead::ViewVector> cross(0x90);
    gabi::FrameLocal<sead::ViewVector> side(0x168);
    if (planar != 0) {
        crossFramingVectors(right.get(), &sCameraWorldUp, cross.get());
        normalizeFramingVector(cross.get());
        crossFramingVectors(cross.get(), right.get(), side.get());
        normalizeFramingVector(side.get());
    }
    f32 rightValues[3] = {right->x.value, right->y.value, right->z.value};
    f32 upValues[3] = {up->x.value, up->y.value, up->z.value};
    f32 sideValues[3] = {};
    f32 crossValues[3] = {};
    if (planar != 0) {
        sideValues[0] = side->x.value; sideValues[1] = side->y.value; sideValues[2] = side->z.value;
        crossValues[0] = cross->x.value; crossValues[1] = cross->y.value; crossValues[2] = cross->z.value;
    }
    for (u32 component = 0; component < 3; ++component) {
        f32 horizontal = gabi::fmuls_ppc(rightValues[component], basisScale);
        horizontal = gabi::fmuls_ppc(horizontal, distanceScale);
        horizontal = gabi::fmuls_ppc(horizontal, offset->x);
        if (planar == 0) {
            f32 vertical = gabi::fmuls_ppc(upValues[component], basisScale);
            vertical = gabi::fmuls_ppc(vertical, distanceScale);
            vertical = gabi::fmuls_ppc(vertical, offset->y);
            if (component < 2) target[component] = gabi::fadds_ppc(vertical, gabi::fadds_ppc(horizontal, target[component]));
            else target[component] = gabi::fadds_ppc(gabi::fadds_ppc(target[component], horizontal), vertical);
        } else {
            f32 vertical = gabi::fmuls_ppc(sideValues[component], basisScale);
            vertical = gabi::fmuls_ppc(vertical, distanceScale);
            vertical = gabi::fmuls_fpr(gabi::fpr_from_f32(vertical), verticalAdjustment);
            f32 depth = gabi::fmuls_ppc(crossValues[component], basisScale);
            depth = gabi::fmuls_ppc(depth, distanceScale);
            depth = gabi::fmuls_ppc(depth, -f32(offset->y));
            if (component < 2) target[component] = gabi::fadds_ppc(depth, gabi::fadds_ppc(vertical, gabi::fadds_ppc(horizontal, target[component])));
            else target[component] = gabi::fadds_ppc(gabi::fadds_ppc(gabi::fadds_ppc(target[component], horizontal), vertical), depth);
        }
    }
    gabi::FrameLocal<Quaternion> pitch(0x1A8);
    gabi::FrameLocal<Quaternion> yaw(0x1B8);
    makeFramingQuaternion(pitch.get(), right.get(), gabi::fmuls_ppc(angles->y, sCameraPitchScale));
    makeFramingQuaternion(yaw.get(), &sCameraWorldUp, gabi::fmuls_ppc(angles->x, sCameraYawScale));
    gabi::FrameLocal<Quaternion> rotation(0x180);
    multiplyFramingQuaternions(rotation.get(), yaw.get(), pitch.get());
    f32 qz = rotation->z;
    f32 dy = direction->y.value;
    f32 dx = direction->x.value;
    f32 zy = gabi::fmuls_ppc(qz, dy);
    f32 qy = rotation->y;
    f32 qx = rotation->x;
    f32 yx = gabi::fmuls_ppc(qy, dx);
    f32 xx = gabi::fmuls_ppc(qx, dx);
    f32 dz = direction->z.value;
    f32 xz = gabi::fmuls_ppc(qx, dz);
    f32 crossX = gabi::fmsubs(qy, dz, zy);
    f32 crossY = gabi::fmsubs(qz, dx, xz);
    f32 qw = rotation->w;
    f32 crossZ = gabi::fmsubs(qx, dy, yx);
    f32 first = gabi::fmadds(qw, dx, crossX);
    f32 third = gabi::fmadds(qw, dz, crossZ);
    f32 second = gabi::fmadds(qw, dy, crossY);
    f32 fourth = gabi::fnmsubs(qy, dy, -xx);
    f32 firstY = gabi::fmuls_ppc(first, qy);
    fourth = gabi::fnmsubs(qz, dz, fourth);
    f32 secondW = gabi::fmuls_ppc(second, qw);
    f32 secondZ = gabi::fmuls_ppc(second, qz);
    f32 lastZ = gabi::fmsubs(second, qx, firstY);
    f32 lastX = gabi::fmsubs(first, qw, secondZ);
    f32 lastY = gabi::fmadds(first, qz, secondW);
    lastZ = gabi::fmadds(third, qw, lastZ);
    lastX = gabi::fmadds(third, qy, lastX);
    f32 rotatedZ = gabi::fnmsubs(fourth, qz, lastZ);
    lastY = gabi::fnmsubs(third, qx, lastY);
    f32 rotatedX = gabi::fnmsubs(fourth, qx, lastX);
    f32 rotatedY = gabi::fnmsubs(fourth, qy, lastY);
    direction->z.value = rotatedZ;
    direction->x.value = rotatedX;
    direction->y.value = rotatedY;
    normalizeFramingVector(direction.get());
    f32 eyeX = gabi::fsubs_ppc(target[0], gabi::fmuls_ppc(direction->x.value, radius));
    f32 eyeY = gabi::fsubs_ppc(target[1], gabi::fmuls_ppc(direction->y.value, radius));
    f32 eyeZ = gabi::fsubs_ppc(target[2], gabi::fmuls_ppc(direction->z.value, radius));
    crossFramingVectors(right.get(), direction.get(), cross.get());
    f32 upSign = cross->y.value >= 0.0f ? sAlignmentFirst : sAlignmentLast;
    output->mUp.z.value = sAlignmentMiddle;
    output->mUp.y.value = upSign;
    output->mUp.x.value = sAlignmentMiddle;
    normalizeFramingVector(&output->mUp);
    output->mTarget.y.value = target[1];
    output->mTarget.z.value = target[2];
    output->mEye.z.value = eyeZ;
    output->mEye.y.value = eyeY;
    output->mTarget.x.value = target[0];
    output->mEye.x.value = eyeX;
}
/* WWHD glue
 data sCameraDistanceLower 10168564
 data sCameraDistanceUpper 10168560
 data sCameraDistanceSlope 10168568
 data sCameraDistanceIntercept 1016856C
 data sCameraIncomingScale 10168570
 data sCameraDistanceScale 10168500
 data sCameraBasisScale 10168574
 data sCameraPitchScale 10168578
 data sCameraYawScale 1016857C
 data sCameraWorldUp 104A0940
 ext 0274CB08 void sead::ViewTransform::copyRight(sead::ViewVector*) const
 ext 0274CB24 void sead::ViewTransform::copyUp(sead::ViewVector*) const
 ext 0274CAEC void sead::ViewTransform::copyForward(sead::ViewVector*) const
 ext 025155D8 f32 normalizeFramingVector(sead::ViewVector*)
 ext 028E8D4C void crossFramingVectors(const sead::ViewVector*, const sead::ViewVector*, sead::ViewVector*)
 ext 02515644 void makeFramingQuaternion(Quaternion*, sead::ViewVector*, f32)
 ext 027AAF80 void multiplyFramingQuaternions(Quaternion*, const Quaternion*, const Quaternion*)
 data sAlignmentFirst 10168508
 data sAlignmentMiddle 1016854C
 data sAlignmentLast 10168550
*/
