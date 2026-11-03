/* Rest-pose scale and orientation construction from resource bones. */
#include "agl/gx2/BufferRuntime.h"
#include "agl/gx2/PoseRecords.h"
#include "agl/gx2/PoseTrigTable.h"
using namespace agl::gx2util;
extern Pair32 encodePoseAngle(f32);
struct PoseTrigValues { f32 mSin, mCos; };
struct PoseAngleConversion { be<u32> mExponent; be<u32> mFraction; };
WWHD_OFFSET(PoseAngleConversion, mExponent, 0);
WWHD_OFFSET(PoseAngleConversion, mFraction, 4);
WWHD_SIZE(PoseAngleConversion, 8);
static PoseTrigValues poseTrig(f32 angle, PoseAngleConversion* conversion) {
    auto encodedPair = encodePoseAngle(gabi::fmuls_ppc(angle, 683565248.0f));
    u32 encoded = encodedPair.r4;
    conversion->mExponent = 0x43300000;
    conversion->mFraction = encoded & 0xFFFFFF;
    f32 fraction = gabi::fmuls_ppc((f32)(encoded & 0xFFFFFF), 5.960464477539063e-8f);
    PoseTrigEntry* entry = &sPoseTrigTable.mEntries[encoded >> 24];
    auto result = PoseTrigValues{};
    result.mSin = gabi::fmadds(entry->mSinSlope, fraction, entry->mSin);
    result.mCos = gabi::fmadds(entry->mCosSlope, fraction, entry->mCos);
    return result;
}

/* 027DABE0 */
void agl::gx2util::PaletteBuffer::resetValues() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 flags = (u32)mFlags | 4;
    auto resource = reinterpret_cast<PoseResource*>(mResource.get());
    auto local = static_cast<PoseLocal*>(mLocal.get());
    mFlags = flags;
    u32 count = resource->mBoneCount;
    bool euler = (flags & 0x7000) != 0;
    gabi::FrameLocal<PoseAngleConversion> conversionStorage(0x08);
    for (u32 index = 0; index < count; ++index) {
        auto bones = static_cast<PoseBoneDescriptor*>(resource->mBones.get());
        const PoseBoneDescriptor* bone = bones + index;
        PoseLocal* output = local + index;
        output->mFlags = (u32)bone->mFlags & 0x0F870000;
        for (u32 axis = 0; axis < 3; ++axis) output->mScale[axis] = *reinterpret_cast<const be<u32>*>(&bone->mScale[axis]);
        auto matrix = reinterpret_cast<be<f32>*>(&output->mMatrix);
        if (euler) {
            auto x = poseTrig(bone->mRotation[0], conversionStorage);
            auto y = poseTrig(bone->mRotation[1], conversionStorage);
            auto z = poseTrig(bone->mRotation[2], conversionStorage);
            f32 sxsy = gabi::fmuls_ppc(x.mSin, y.mSin);
            f32 sxcy = gabi::fmuls_ppc(x.mSin, y.mCos);
            f32 sxsz = gabi::fmuls_ppc(x.mSin, z.mSin);
            f32 sxcz = gabi::fmuls_ppc(x.mSin, z.mCos);
            f32 cxcz = gabi::fmuls_ppc(x.mCos, z.mCos);
            f32 cxsz = gabi::fmuls_ppc(x.mCos, z.mSin);
            matrix[4] = gabi::fmuls_ppc(y.mCos, z.mSin);
            matrix[8] = -y.mSin;
            matrix[1] = gabi::fmsubs(sxsy, z.mCos, cxsz);
            matrix[0] = gabi::fmuls_ppc(y.mCos, z.mCos);
            matrix[5] = gabi::fmadds(sxsy, z.mSin, cxcz);
            matrix[9] = sxcy;
            matrix[2] = gabi::fmadds(cxcz, y.mSin, sxsz);
            matrix[6] = gabi::fmsubs(cxsz, y.mSin, sxcz);
            matrix[10] = gabi::fmuls_ppc(x.mCos, y.mCos);
        } else {
            f32 w = bone->mRotation[3];
            f32 x = bone->mRotation[0];
            f32 y = bone->mRotation[1];
            f32 z = bone->mRotation[2];
            f32 twiceW = gabi::fadds_ppc(w, w);
            f32 twiceX = gabi::fadds_ppc(x, x);
            f32 twiceY = gabi::fadds_ppc(y, y);
            f32 wx = gabi::fmuls_ppc(twiceW, x);
            f32 yy = gabi::fmuls_ppc(twiceY, y);
            f32 xz = gabi::fmuls_ppc(twiceX, z);
            f32 wz = gabi::fmuls_ppc(twiceW, z);
            f32 wy = gabi::fmuls_ppc(twiceW, y);
            f32 oneMinusYY = gabi::fsubs_ppc(1.0f, yy);
            f32 xy = gabi::fmuls_ppc(twiceX, y);
            f32 oneMinusXX = gabi::fnmsubs(twiceX, x, 1.0f);
            f32 yz = gabi::fmuls_ppc(twiceY, z);
            f32 twiceZ = gabi::fadds_ppc(z, z);
            f32 zz = gabi::fmuls_ppc(twiceZ, z);
            matrix[1] = gabi::fsubs_ppc(xy, wz);
            matrix[2] = gabi::fadds_ppc(xz, wy);
            matrix[4] = gabi::fadds_ppc(xy, wz);
            matrix[0] = gabi::fsubs_ppc(oneMinusYY, zz);
            matrix[5] = gabi::fsubs_ppc(oneMinusXX, zz);
            matrix[6] = gabi::fsubs_ppc(yz, wx);
            matrix[8] = gabi::fsubs_ppc(xz, wy);
            matrix[9] = gabi::fadds_ppc(yz, wx);
            matrix[10] = gabi::fsubs_ppc(oneMinusXX, yy);
        }
        output->mMatrix.mRows[0].w.mBits = *reinterpret_cast<const be<u32>*>(&bone->mTranslation[0]);
        output->mMatrix.mRows[1].w.mBits = *reinterpret_cast<const be<u32>*>(&bone->mTranslation[1]);
        output->mMatrix.mRows[2].w.mBits = *reinterpret_cast<const be<u32>*>(&bone->mTranslation[2]);
        resource = reinterpret_cast<PoseResource*>(mResource.get());
    }
}

/* WWHD glue
 ext 028F5680 Pair32 encodePoseAngle(f32)
*/
