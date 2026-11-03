// Own normalization caller, preserving ordered/unordered comparisons of both axis tests.
#include "agl/DrawResourceCleanup.h"
void normalizeDrawResourceDirection(sead::RenderVector3* direction);
namespace agl {
struct DrawResourceScaleTable_l { be<f32> mValues[3]; };
static sead::RenderVector3 sDirectionDefaultAxis;
static DrawResourceScaleTable_l sDirectionScaleTable;
static f32 sDirectionConservativeScale;
/* 027CCEB8 */
void agl::DrawResourceDirectionState_l::prepareDirection(DrawResourceRecord_l* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> negativeAxis(8);
    gabi::FrameLocal<sead::RenderVector3> copiedAxis(0x14);
    normalizeDrawResourceDirection(&record->mDirection);
    f32 directionX = record->mDirection.x;
    f32 axisX = sDirectionDefaultAxis.x;
    constexpr f32 lower = -9.999999747378752e-05f;
    constexpr f32 upper = 9.999999747378752e-05f;
    f32 difference = gabi::fsubs_ppc(directionX, axisX);
    f32 axisY = sDirectionDefaultAxis.y;
    f32 axisZ = sDirectionDefaultAxis.z;
    bool perturb = false;
    f32 signInput = 0.0f;
    if (!(lower > difference) && !(difference > upper)) {
        f32 directionY = record->mDirection.y;
        f32 differenceY = gabi::fsubs_ppc(directionY, axisY);
        if (!(lower > differenceY) && !(differenceY > upper)) {
            f32 directionZ = record->mDirection.z;
            f32 differenceZ = gabi::fsubs_ppc(directionZ, axisZ);
            if (!(lower > differenceZ) && !(differenceZ > upper)) {
                perturb = true;
                signInput = directionY;
            }
        }
    }
    if (!perturb) {
        negativeAxis->x = -axisX;
        copiedAxis->x = negativeAxis->x;
        negativeAxis->y = -axisY;
        negativeAxis->z = -axisZ;
        difference = gabi::fsubs_ppc(f32(record->mDirection.x), f32(copiedAxis->x));
        copiedAxis->y = negativeAxis->y;
        copiedAxis->z = negativeAxis->z;
        if (!(lower > difference) && !(difference > upper)) {
            f32 directionY = record->mDirection.y;
            f32 differenceY = gabi::fsubs_ppc(directionY, f32(copiedAxis->y));
            if (!(lower > differenceY) && !(differenceY > upper)) {
                f32 differenceZ = gabi::fsubs_ppc(f32(record->mDirection.z), f32(copiedAxis->z));
                if (!(lower > differenceZ) && !(differenceZ > upper)) {
                    perturb = true;
                    signInput = directionY;
                }
            }
        }
    }
    if (perturb) {
        record->mDirection.z = upper;
        record->mDirection.x = upper;
        record->mDirection.y = signInput >= 0.0f ? 1.0f : -1.0f;
        normalizeDrawResourceDirection(&record->mDirection);
    }
    if (!(record->mFalloff > 0.0f))
        record->mFalloff = 0.0010000000474974513f;
    if (!(record->mScale > 0.0f))
        record->mScale = 0.0010000000474974513f;
    DrawResourceConfiguration_l* configuration = mConfiguration;
    u32 flags = configuration->mFlags;
    f32 factor = flags & 0x20 ? f32(sDirectionConservativeScale) : 1.0f;
    u32 index = configuration->mFormatIndex;
    f32 scale = sDirectionScaleTable.mValues[index];
    mScale = gabi::fmuls_ppc(factor, scale);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
}
/* WWHD glue
data sDirectionDefaultAxis 104A0940
data sDirectionScaleTable 101F9620
data sDirectionConservativeScale 101F9610
ext 025155D8 void normalizeDrawResourceDirection(sead::RenderVector3* direction)
*/
