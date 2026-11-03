/* Curve sampling and neighboring utilities reconstructed from the WWHD binary. */
#include "agl/gx2/GpuUtilitySupport.h"
#include "sead/ElementAt.h"
#include "agl/gx2/PoseTrigTable.h"
using namespace agl_gx2_support;
static CurveRegistrationState sCurveSamplerStatic;
static CurveBasisMatrix sCurveBasis;
static u8 sCurveSamplerRegistration;
namespace agl_gx2_support {
// The original single multiply keeps a wide frA and rounds frC to 25 bits first.
f32 singleMultiplyWide(f64 first, f64 second) {
    f64 product = first * round25(second);
    return static_cast<f32>(gabi::ppc_nan2(first, second, product, gabi::ppc_dnan64()));
}

template<class T> static T* relativeTarget(const be<s32>* member) {
    u32 offset = *member;
    return offset ? gabi::at<T>(gabi::ea(member) + offset) : nullptr;
}
/* 027E640C */
void UniformMember::copySwapped(be<u32>* destination,const be<u32>* source) const {
    u32 format = mEncoding;
    if (format <= 15) {
        u32 components = (format & 3) + 1;
        for (u32 component = 0; component < components; ++component) {
            u32 word = source[component];
            destination[component] = __builtin_bswap32(word);
        }
    } else if (format <= 27) {
        u32 components = (format & 3) + 1;
        u32 rows = ((format - 16) >> 2) + 2;
        for (u32 row = 0; row < rows; ++row) {
            for (u32 component = 0; component < components; ++component) {
                u32 word = source[component];
                destination[component] = __builtin_bswap32(word);
            }
            source += components;
            destination += 4;
        }
    }
}
/* 027E6608 */
void UniformMember::copyNative(be<u32>* destination,const be<u32>* source) const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 format = mEncoding;
    if (format <= 15) {
        u32 components = (format & 3) + 1;
        utilityCopy(destination,source,components * 4);
    } else if (format <= 27) {
        u32 components = (format & 3) + 1;
        u32 rows = ((format - 16) >> 2) + 2;
        for (u32 row = 0; row < rows; ++row) {
            utilityCopy(destination,source,components * 4);
            source += components;
            destination += 4;
        }
    }
}
/* 027E67EC */
void ChannelTrack_l::evaluateCached(ChannelValue* output,const be<u16>* bindings,SampleCursor_l* cursors,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    ChannelBindingInfo_l* info = relativeTarget<ChannelBindingInfo_l>(&mBindingsRelative);
    AnimationCurve_l* curves = relativeTarget<AnimationCurve_l>(&mCurvesRelative);
    u32 count = mTargetCount;
    u32 resultIndex = 0;
    for (u32 target = 0; target < count; ++target,++info) {
        u32 binding = bindings[target];
        u32 integerCount = info->mIntegerCount;
        u32 floatCount = info->mFloatCount;
        if (binding == 0xFFFF) {
            resultIndex += integerCount + floatCount;
            continue;
        }
        ChannelValue* destination = output + resultIndex;
        AnimationCurve_l* curve = curves + resultIndex;
        resultIndex += integerCount;
        for (u32 sample = 0; sample < integerCount; ++sample) {
            u32 value = sampleChannelInteger(curve,cursors,frame);
            destination->mInteger = value;
            ++destination; ++cursors; ++curve;
        }
        destination = output + resultIndex;
        resultIndex += floatCount;
        for (u32 sample = 0; sample < floatCount; ++sample) {
            f32 value = sampleChannelFloat(curve,cursors,frame);
            destination->mFloat = value;
            ++destination; ++cursors; ++curve;
        }
    }
}
/* 027E6930 */
void ChannelTrack_l::evaluateUncached(ChannelValue* output,const be<u16>* bindings,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    ChannelBindingInfo_l* info = relativeTarget<ChannelBindingInfo_l>(&mBindingsRelative);
    AnimationCurve_l* curves = relativeTarget<AnimationCurve_l>(&mCurvesRelative);
    u32 count = mTargetCount;
    u32 resultIndex = 0;
    gabi::FrameLocal<SampleCursor_l> integerCursor(8);
    gabi::FrameLocal<SampleCursor_l> floatCursor(0x14);
    for (u32 target = 0; target < count; ++target,++info) {
        u32 binding = bindings[target];
        u32 integerCount = info->mIntegerCount;
        u32 floatCount = info->mFloatCount;
        if (binding == 0xFFFF) {
            resultIndex += integerCount + floatCount;
            continue;
        }
        ChannelValue* destination = output + resultIndex;
        AnimationCurve_l* curve = curves + resultIndex;
        resultIndex += integerCount;
        for (u32 sample = 0; sample < integerCount; ++sample) {
            integerCursor->mStartFrame = __builtin_inff();
            u32 value = sampleChannelInteger(curve,integerCursor.get(),frame);
            ++curve;
            destination->mInteger = value;
            ++destination;
        }
        destination = output + resultIndex;
        resultIndex += floatCount;
        for (u32 sample = 0; sample < floatCount; ++sample) {
            floatCursor->mStartFrame = __builtin_inff();
            f32 value = sampleChannelFloat(curve,floatCursor.get(),frame);
            ++curve;
            destination->mFloat = value;
            ++destination;
        }
    }
}
/* 027E6AA8 */
void TransformRuntime_l::evaluateEulerTransforms() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 state = mState;
    const be<f32>* frameData = mFrame;
    s32 cursorCount = mCursorCount;
    TransformResult_l* result = mResults;
    f32 frame = *frameData;
    mState = state | ResultsEvaluated;
    if (cursorCount > 0 && cursorCount <= mCursorCapacity) {
        u32 count = mBindings.mCount;
        u32 firstCursor = 0;
        for (u32 target = 0; target < count; ++target) {
            const be<u32>* entries = mBindings.mEntries;
            TransformTrack_l* tracks = mTracks;
            u32 flags = entries[target];
            TransformTrack_l* track = tracks + target;
            u32 curveCount = track->mCurveCount;
            u32 cursorOffset = firstCursor;
            firstCursor += curveCount;
            if (!(flags & SkipEvaluation)) {
                SampleCursor_l* cursors = mCursors;
                evaluateTransformCached(track,result,cursors + cursorOffset,frame);
            }
            ++result;
        }
    } else {
        u32 count = mBindings.mCount;
        for (u32 target = 0; target < count; ++target) {
            const be<u32>* entries = mBindings.mEntries;
            u32 flags = entries[target];
            if (!(flags & SkipEvaluation)) {
                TransformTrack_l* tracks = mTracks;
                evaluateTransformUncached(tracks + target,result,frame);
            }
            ++result;
        }
    }
}
/* 027E6EA8 */
void TransformRuntime_l::evaluateQuaternionTransforms() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 state = mState;
    const be<f32>* frameData = mFrame;
    s32 cursorCount = mCursorCount;
    TransformResult_l* result = mResults;
    f32 frame = *frameData;
    mState = state | ResultsEvaluated;
    if (cursorCount > 0 && cursorCount <= mCursorCapacity) {
        u32 count = mBindings.mCount;
        u32 firstCursor = 0;
        for (u32 target = 0; target < count; ++target) {
            const be<u32>* entries = mBindings.mEntries;
            TransformTrack_l* tracks = mTracks;
            u32 flags = entries[target];
            TransformTrack_l* track = tracks + target;
            u32 curveCount = track->mCurveCount;
            u32 cursorOffset = firstCursor;
            firstCursor += curveCount;
            if (!(flags & SkipEvaluation)) {
                SampleCursor_l* cursors = mCursors;
                evaluateTransformCached(track,result,cursors + cursorOffset,frame);
            }
            ++result;
        }
    } else {
        u32 count = mBindings.mCount;
        for (u32 target = 0; target < count; ++target) {
            const be<u32>* entries = mBindings.mEntries;
            u32 flags = entries[target];
            if (!(flags & SkipEvaluation)) {
                TransformTrack_l* tracks = mTracks;
                evaluateTransformUncached(tracks + target,result,frame);
            }
            ++result;
        }
    }
}
f32 angleSine(u32 code,f32 fraction) {
    const agl::gx2util::PoseTrigEntry* entry = &sPoseTrigTable.mEntries[code >> 24];
    f32 start = entry->mSin;
    f32 slope = entry->mSinSlope;
    return gabi::fmadds(slope,fraction,start);
}
f32 angleCosine(u32 code,f32 fraction) {
    const agl::gx2util::PoseTrigEntry* entry = &sPoseTrigTable.mEntries[code >> 24];
    f32 start = entry->mCos;
    f32 slope = entry->mCosSlope;
    return gabi::fmadds(slope,fraction,start);
}
/* 027E6C04 */
void TransformRuntime_l::applyEulerTransforms(SkeletonRuntime_l* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 flags = skeleton->mFlags;
    TransformBone_l* bones = skeleton->mBones;
    skeleton->mFlags = flags | 4;
    u32 state = mState;
    u32 count = mBindings.mCount;
    mState = state | ResultsEvaluated;
    const TransformResult_l* result = mResults;
    for (u32 target = 0; target < count; ++target,++result) {
        const be<u32>* entries = mBindings.mEntries;
        u32 binding = entries[target];
        if (binding & SkipApplication) continue;
        u32 resultFlags = result->mFlags;
        TransformBone_l* bone = bones + (binding & 0x7FFF);
        u32 boneFlags = bone->mFlags;
        constexpr u32 ScaleFlags = 0x0F800000;
        bone->mFlags = (boneFlags & ~ScaleFlags) | (resultFlags & ScaleFlags);
        for (u32 component = 0; component < 3; ++component) bone->mScale[component] = result->mScale[component];
        bone->mMatrix[3].mBits = result->mTranslation[0];
        bone->mMatrix[7].mBits = result->mTranslation[1];
        bone->mMatrix[11].mBits = result->mTranslation[2];
        f32 rotationX = result->mRotation[0].mValue;
        auto angleX = transformAngleIndex(gabi::fmuls_ppc(rotationX,683565248.0f));
        f32 rotationY = result->mRotation[1].mValue;
        f32 fractionX = gabi::fmuls_ppc(static_cast<f32>(angleX.r4 & 0xFFFFFF),0x1p-24f);
        f32 sineX = angleSine(angleX.r4,fractionX);
        f32 cosineX = angleCosine(angleX.r4,fractionX);
        auto angleY = transformAngleIndex(gabi::fmuls_ppc(rotationY,683565248.0f));
        f32 rotationZ = result->mRotation[2].mValue;
        f32 fractionY = gabi::fmuls_ppc(static_cast<f32>(angleY.r4 & 0xFFFFFF),0x1p-24f);
        f32 cosineY = angleCosine(angleY.r4,fractionY);
        f32 sineY = angleSine(angleY.r4,fractionY);
        auto angleZ = transformAngleIndex(gabi::fmuls_ppc(rotationZ,683565248.0f));
        f32 sineXsineY = gabi::fmuls_ppc(sineX,sineY);
        f32 sineXcosineY = gabi::fmuls_ppc(sineX,cosineY);
        f32 fractionZ = gabi::fmuls_ppc(static_cast<f32>(angleZ.r4 & 0xFFFFFF),0x1p-24f);
        f32 negativeSineY = -sineY;
        f32 sineZ = angleSine(angleZ.r4,fractionZ);
        f32 cosineZ = angleCosine(angleZ.r4,fractionZ);
        f32 cosineYsineZ = gabi::fmuls_ppc(cosineY,sineZ);
        f32 cosineXcosineZ = gabi::fmuls_ppc(cosineX,cosineZ);
        f32 cosineXsineZ = gabi::fmuls_ppc(cosineX,sineZ);
        f32 rowYy = gabi::fmadds(sineXsineY,sineZ,cosineXcosineZ);
        bone->mMatrix[4].mValue = cosineYsineZ;
        f32 rowXy = gabi::fmsubs(sineXsineY,cosineZ,cosineXsineZ);
        bone->mMatrix[8].mValue = negativeSineY;
        f32 cosineYcosineZ = gabi::fmuls_ppc(cosineY,cosineZ);
        f32 sineXsineZ = gabi::fmuls_ppc(sineX,sineZ);
        f32 sineXcosineZ = gabi::fmuls_ppc(sineX,cosineZ);
        bone->mMatrix[1].mValue = rowXy;
        f32 rowXz = gabi::fmadds(cosineXcosineZ,sineY,sineXsineZ);
        bone->mMatrix[5].mValue = rowYy;
        f32 rowYz = gabi::fmsubs(cosineXsineZ,sineY,sineXcosineZ);
        bone->mMatrix[9].mValue = sineXcosineY;
        f32 cosineXcosineY = gabi::fmuls_ppc(cosineX,cosineY);
        bone->mMatrix[2].mValue = rowXz;
        bone->mMatrix[6].mValue = rowYz;
        bone->mMatrix[10].mValue = cosineXcosineY;
        bone->mMatrix[0].mValue = cosineYcosineZ;
    }
}
/* 027E7004 */
void TransformRuntime_l::applyQuaternionTransforms(SkeletonRuntime_l* skeleton) {
    u32 flags = skeleton->mFlags;
    TransformBone_l* bones = skeleton->mBones;
    skeleton->mFlags = flags | 4;
    u32 state = mState;
    u32 count = mBindings.mCount;
    mState = state | ResultsEvaluated;
    const TransformResult_l* result = mResults;
    for (u32 target = 0; target < count; ++target,++result) {
        const be<u32>* entries = mBindings.mEntries;
        u32 binding = entries[target];
        if (binding & SkipApplication) continue;
        u32 resultFlags = result->mFlags;
        TransformBone_l* bone = bones + (binding & 0x7FFF);
        u32 boneFlags = bone->mFlags;
        constexpr u32 ScaleFlags = 0x0F800000;
        bone->mFlags = (boneFlags & ~ScaleFlags) | (resultFlags & ScaleFlags);
        for (u32 component = 0; component < 3; ++component) bone->mScale[component] = result->mScale[component];
        bone->mMatrix[3].mBits = result->mTranslation[0];
        bone->mMatrix[7].mBits = result->mTranslation[1];
        bone->mMatrix[11].mBits = result->mTranslation[2];
        f32 y = result->mRotation[1].mValue;
        f32 x = result->mRotation[0].mValue;
        f32 w = result->mRotation[3].mValue;
        f32 twiceY = gabi::fadds_ppc(y,y);
        f32 twiceX = gabi::fadds_ppc(x,x);
        f32 yy = gabi::fmuls_ppc(twiceY,y);
        f32 z = result->mRotation[2].mValue;
        f32 twiceW = gabi::fadds_ppc(w,w);
        f32 twiceZ = gabi::fadds_ppc(z,z);
        f32 withoutY = gabi::fsubs_ppc(1.0f,yy);
        f32 wy = gabi::fmuls_ppc(twiceW,y);
        f32 zz = gabi::fmuls_ppc(twiceZ,z);
        f32 wz = gabi::fmuls_ppc(twiceW,z);
        f32 diagonalX = gabi::fsubs_ppc(withoutY,zz);
        f32 xy = gabi::fmuls_ppc(twiceX,y);
        f32 xz = gabi::fmuls_ppc(twiceX,z);
        f32 rowXy = gabi::fsubs_ppc(xy,wz);
        f32 wx = gabi::fmuls_ppc(twiceW,x);
        f32 rowYx = gabi::fadds_ppc(xy,wz);
        bone->mMatrix[1].mValue = rowXy;
        f32 withoutX = gabi::fnmsubs(twiceX,x,1.0f);
        f32 yz = gabi::fmuls_ppc(twiceY,z);
        bone->mMatrix[4].mValue = rowYx;
        f32 rowXz = gabi::fadds_ppc(xz,wy);
        f32 diagonalY = gabi::fsubs_ppc(withoutX,zz);
        bone->mMatrix[0].mValue = diagonalX;
        f32 rowZx = gabi::fsubs_ppc(xz,wy);
        bone->mMatrix[2].mValue = rowXz;
        f32 diagonalZ = gabi::fsubs_ppc(withoutX,yy);
        bone->mMatrix[5].mValue = diagonalY;
        f32 rowYz = gabi::fsubs_ppc(yz,wx);
        bone->mMatrix[8].mValue = rowZx;
        f32 rowZy = gabi::fadds_ppc(yz,wx);
        bone->mMatrix[10].mValue = diagonalZ;
        bone->mMatrix[6].mValue = rowYz;
        bone->mMatrix[9].mValue = rowZy;
    }
}
/* 027E9194 */
s32 AnimationCurve_l::readByteCached(SampleCursor_l* cursor,f32 frame) {
    updateCurveCursor(this,cursor,frame);
    const be<s8>* values = relativeTarget<be<s8>>(&mValuesRelative);
    u32 index = cursor->mIndex;
    return values[index];
}
/* 027E91F0 */
s32 AnimationCurve_l::readShortCached(SampleCursor_l* cursor,f32 frame) {
    updateCurveCursor(this,cursor,frame);
    const be<s16>* values = relativeTarget<be<s16>>(&mValuesRelative);
    u32 index = cursor->mIndex;
    return sead::elementAt(values, index);
}
/* 027E924C */
u32 AnimationCurve_l::readWordCached(SampleCursor_l* cursor,f32 frame) {
    updateCurveCursor(this,cursor,frame);
    const be<u32>* values = relativeTarget<be<u32>>(&mValuesRelative);
    u32 index = cursor->mIndex;
    return sead::elementAt(values, index);
}
/* 027E9C38 */
u32 AnimationCurve_l::readWord(f32 frame) const {
    u32 start = gabi::ftoi(mStartFrame);
    u32 index = static_cast<u32>(gabi::ftoi(frame)) - start;
    const be<u32>* values = relativeTarget<be<u32>>(&mValuesRelative);
    return sead::elementAt(values, static_cast<u32>(index));
}
/* 027E9C88 */
s32 AnimationCurve_l::readShort(f32 frame) const {
    u32 start = gabi::ftoi(mStartFrame);
    u32 index = static_cast<u32>(gabi::ftoi(frame)) - start;
    const be<s16>* values = relativeTarget<be<s16>>(&mValuesRelative);
    return sead::elementAt(values, static_cast<u32>(index));
}
/* 027E9CD8 */
s32 AnimationCurve_l::readByte(f32 frame) const {
    u32 start = gabi::ftoi(mStartFrame);
    u32 index = static_cast<u32>(gabi::ftoi(frame)) - start;
    const be<s8>* values = relativeTarget<be<s8>>(&mValuesRelative);
    return values[index];
}
/* 027E9850 */
f32 AnimationCurve_l::linearFloatCached(SampleCursor_l* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    updateCurveCursor(this,cursor,frame);
    f32 begin = cursor->mStartFrame;
    f32 end = cursor->mEndFrame;
    f32 duration = gabi::fsubs_ppc(end,begin);
    f32 reciprocal = static_cast<f32>(gabi::fres(duration));
    f32 elapsed = gabi::fsubs_ppc(frame,begin);
    f32 doubledReciprocal = gabi::fadds_ppc(reciprocal,reciprocal);
    f32 correction = gabi::fmuls_ppc(duration,reciprocal);
    const be<f32>* values = relativeTarget<be<f32>>(&mValuesRelative);
    f32 refined = gabi::fnmsubs(correction,reciprocal,doubledReciprocal);
    u32 index = cursor->mIndex;
    f32 fraction = gabi::fmuls_ppc(elapsed,refined);
    f32 first = sead::elementAt(values, static_cast<u32>(index * 2));
    f32 slope = sead::elementAt(values, static_cast<u32>(index * 2 + 1));
    return gabi::fmadds(slope,fraction,first);
}
/* 027E98F8 */
f32 AnimationCurve_l::linearShortCached(SampleCursor_l* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    updateCurveCursor(this,cursor,frame);
    f32 begin = cursor->mStartFrame;
    f32 end = cursor->mEndFrame;
    f32 duration = gabi::fsubs_ppc(end,begin);
    f32 reciprocal = static_cast<f32>(gabi::fres(duration));
    f32 elapsed = gabi::fsubs_ppc(frame,begin);
    f32 doubledReciprocal = gabi::fadds_ppc(reciprocal,reciprocal);
    f32 correction = gabi::fmuls_ppc(duration,reciprocal);
    const be<s16>* values = relativeTarget<be<s16>>(&mValuesRelative);
    f32 refined = gabi::fnmsubs(correction,reciprocal,doubledReciprocal);
    u32 index = cursor->mIndex;
    f32 fraction = gabi::fmuls_ppc(elapsed,refined);
    f32 first = sead::elementAt(values, static_cast<u32>(index * 2));
    f32 slope = sead::elementAt(values, static_cast<u32>(index * 2 + 1));
    return gabi::fmadds(slope,fraction,first);
}
/* 027E99A0 */
f32 AnimationCurve_l::linearByteCached(SampleCursor_l* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    updateCurveCursor(this,cursor,frame);
    f32 begin = cursor->mStartFrame;
    f32 end = cursor->mEndFrame;
    f32 duration = gabi::fsubs_ppc(end,begin);
    f32 reciprocal = static_cast<f32>(gabi::fres(duration));
    f32 elapsed = gabi::fsubs_ppc(frame,begin);
    f32 doubledReciprocal = gabi::fadds_ppc(reciprocal,reciprocal);
    f32 correction = gabi::fmuls_ppc(duration,reciprocal);
    const be<s8>* values = relativeTarget<be<s8>>(&mValuesRelative);
    f32 refined = gabi::fnmsubs(correction,reciprocal,doubledReciprocal);
    u32 index = cursor->mIndex;
    f32 fraction = gabi::fmuls_ppc(elapsed,refined);
    f32 first = values[index * 2];
    f32 slope = values[index * 2 + 1];
    return gabi::fmadds(slope,fraction,first);
}
static f32 floatFromBits(u32 bits) {
    f32 value;
    __builtin_memcpy(&value,&bits,sizeof(value));
    return value;
}
static void storeFloatBits(be<f32>* destination,u32 bits) {
    *reinterpret_cast<be<u32>*>(destination) = bits;
}
/* 027E92A8 */
void AnimationCurve_l::locateFloatInterval(SampleCursor_l* cursor,f32 frame) const {
    const FloatWord* times = relativeTarget<FloatWord>(&mFramesRelative);
    u32 last = static_cast<u32>(mKeyCount) - 1;
    f32 lastFrame = times[last].mValue;
    if (!(frame < lastFrame)) {
        last = static_cast<u16>(last);
        cursor->mIndex = last;
        u32 bits = times[last].mBits;
        f32 start = floatFromBits(bits);
        f32 end = gabi::fadds_ppc(start,1.0f);
        storeFloatBits(&cursor->mStartFrame,bits);
        cursor->mEndFrame = end;
        return;
    }
    u32 index = cursor->mIndex;
    if (frame < times[index].mValue) {
        do { --index; } while (frame < times[index].mValue);
    } else {
        do { ++index; } while (!(times[index].mValue > frame));
        --index;
    }
    cursor->mIndex = index;
    u32 first = times[index].mBits;
    storeFloatBits(&cursor->mStartFrame,first);
    u32 second = times[index + 1].mBits;
    storeFloatBits(&cursor->mEndFrame,second);
}
/* 027E9378 */
void AnimationCurve_l::locateShortInterval(SampleCursor_l* cursor,f32 frame) const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const be<s16>* times = relativeTarget<be<s16>>(&mFramesRelative);
    s32 searchFrame = static_cast<s16>(psq_quant(gabi::fmsubs(frame,32.0f,-32768.0f),5,0) - 32768);
    u32 last = static_cast<u32>(mKeyCount) - 1;
    s32 lastFrame = times[last];
    if (searchFrame >= lastFrame) {
        last = static_cast<u16>(last);
        cursor->mIndex = last;
        f32 start = gabi::fmuls_ppc(static_cast<f32>(times[last]),0.03125f);
        f32 end = gabi::fadds_ppc(start,1.0f);
        cursor->mStartFrame = start;
        cursor->mEndFrame = end;
        return;
    }
    u32 index = cursor->mIndex;
    if (searchFrame < static_cast<s32>(times[index])) {
        do { --index; } while (searchFrame < static_cast<s32>(times[index]));
    } else {
        do { ++index; } while (static_cast<s32>(times[index]) <= searchFrame);
        --index;
    }
    cursor->mIndex = index;
    f32 start = gabi::fmuls_ppc(static_cast<f32>(times[index]),0.03125f);
    cursor->mStartFrame = start;
    f32 end = gabi::fmuls_ppc(static_cast<f32>(times[index + 1]),0.03125f);
    cursor->mEndFrame = end;
}
/* 027E94D4 */
void AnimationCurve_l::locateByteInterval(SampleCursor_l* cursor,f32 frame) const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const be<u8>* times = relativeTarget<be<u8>>(&mFramesRelative);
    s32 searchFrame = psq_quant(frame,4,0);
    u32 last = static_cast<u32>(mKeyCount) - 1;
    s32 lastFrame = times[last];
    if (searchFrame >= lastFrame) {
        last = static_cast<u16>(last);
        cursor->mIndex = last;
        f32 start = gabi::fmuls_ppc(static_cast<f32>(times[last]),1.0f);
        f32 end = gabi::fadds_ppc(start,1.0f);
        cursor->mStartFrame = start;
        cursor->mEndFrame = end;
        return;
    }
    u32 index = cursor->mIndex;
    if (searchFrame < static_cast<s32>(times[index])) {
        do { --index; } while (searchFrame < static_cast<s32>(times[index]));
    } else {
        do { ++index; } while (static_cast<s32>(times[index]) <= searchFrame);
        --index;
    }
    cursor->mIndex = index;
    f32 start = gabi::fmuls_ppc(static_cast<f32>(times[index]),1.0f);
    cursor->mStartFrame = start;
    f32 end = gabi::fmuls_ppc(static_cast<f32>(times[index + 1]),1.0f);
    cursor->mEndFrame = end;
}
/* 027E95E0 */
f32 AnimationCurve_l::cubicFloatCached(SampleCursor_l* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    updateCurveCursor(this,cursor,frame);
    f32 begin = cursor->mStartFrame;
    f32 end = cursor->mEndFrame;
    f32 duration = gabi::fsubs_ppc(end,begin);
    f32 reciprocal = static_cast<f32>(gabi::fres(duration));
    f32 elapsed = gabi::fsubs_ppc(frame,begin);
    f32 doubledReciprocal = gabi::fadds_ppc(reciprocal,reciprocal);
    f32 correction = gabi::fmuls_ppc(duration,reciprocal);
    const be<f32>* values = relativeTarget<be<f32>>(&mValuesRelative);
    f32 refined = gabi::fnmsubs(correction,reciprocal,doubledReciprocal);
    u32 index = cursor->mIndex;
    f32 fraction = gabi::fmuls_ppc(elapsed,refined);
    f32 constant = sead::elementAt(values, index * 4);
    f32 linear = sead::elementAt(values, index * 4 + 1);
    f32 quadratic = sead::elementAt(values, index * 4 + 2);
    f32 cubic = sead::elementAt(values, index * 4 + 3);
    f32 constantTerm = gabi::fmuls_ppc(constant,1.0f);
    f32 linearTerm = gabi::fmuls_ppc(linear,fraction);
    f32 quadraticTerm = gabi::fmuls_ppc(quadratic,fraction);
    f32 cubicTerm = gabi::fmuls_ppc(cubic,fraction);
    quadraticTerm = gabi::fmuls_ppc(quadraticTerm,fraction);
    cubicTerm = gabi::fmuls_ppc(cubicTerm,fraction);
    quadraticTerm = gabi::fmuls_ppc(quadraticTerm,1.0f);
    cubicTerm = gabi::fmuls_ppc(cubicTerm,fraction);
    f32 evenTerms = gabi::fadds_ppc(constantTerm,quadraticTerm);
    f32 oddTerms = gabi::fadds_ppc(linearTerm,cubicTerm);
    return gabi::fadds_ppc(evenTerms,oddTerms);
}
/* 027E96B0 */
f32 AnimationCurve_l::cubicShortCached(SampleCursor_l* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    updateCurveCursor(this,cursor,frame);
    f32 begin = cursor->mStartFrame;
    f32 end = cursor->mEndFrame;
    f32 duration = gabi::fsubs_ppc(end,begin);
    f32 reciprocal = static_cast<f32>(gabi::fres(duration));
    f32 elapsed = gabi::fsubs_ppc(frame,begin);
    f32 doubledReciprocal = gabi::fadds_ppc(reciprocal,reciprocal);
    f32 correction = gabi::fmuls_ppc(duration,reciprocal);
    const be<s16>* values = relativeTarget<be<s16>>(&mValuesRelative);
    f32 refined = gabi::fnmsubs(correction,reciprocal,doubledReciprocal);
    u32 index = cursor->mIndex;
    f32 fraction = gabi::fmuls_ppc(elapsed,refined);
    f32 constant = sead::elementAt(values, index * 4);
    f32 linear = sead::elementAt(values, index * 4 + 1);
    f32 quadratic = sead::elementAt(values, index * 4 + 2);
    f32 cubic = sead::elementAt(values, index * 4 + 3);
    f32 constantTerm = gabi::fmuls_ppc(constant,1.0f);
    f32 linearTerm = gabi::fmuls_ppc(linear,fraction);
    f32 quadraticTerm = gabi::fmuls_ppc(quadratic,fraction);
    f32 cubicTerm = gabi::fmuls_ppc(cubic,fraction);
    quadraticTerm = gabi::fmuls_ppc(quadraticTerm,fraction);
    cubicTerm = gabi::fmuls_ppc(cubicTerm,fraction);
    quadraticTerm = gabi::fmuls_ppc(quadraticTerm,1.0f);
    cubicTerm = gabi::fmuls_ppc(cubicTerm,fraction);
    f32 evenTerms = gabi::fadds_ppc(constantTerm,quadraticTerm);
    f32 oddTerms = gabi::fadds_ppc(linearTerm,cubicTerm);
    return gabi::fadds_ppc(evenTerms,oddTerms);
}
/* 027E9780 */
f32 AnimationCurve_l::cubicByteCached(SampleCursor_l* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    updateCurveCursor(this,cursor,frame);
    f32 begin = cursor->mStartFrame;
    f32 end = cursor->mEndFrame;
    f32 duration = gabi::fsubs_ppc(end,begin);
    f32 reciprocal = static_cast<f32>(gabi::fres(duration));
    f32 elapsed = gabi::fsubs_ppc(frame,begin);
    f32 doubledReciprocal = gabi::fadds_ppc(reciprocal,reciprocal);
    f32 correction = gabi::fmuls_ppc(duration,reciprocal);
    const be<s8>* values = relativeTarget<be<s8>>(&mValuesRelative);
    f32 refined = gabi::fnmsubs(correction,reciprocal,doubledReciprocal);
    u32 index = cursor->mIndex;
    f32 fraction = gabi::fmuls_ppc(elapsed,refined);
    f32 constant = values[index * 4];
    f32 linear = values[index * 4 + 1];
    f32 quadratic = values[index * 4 + 2];
    f32 cubic = values[index * 4 + 3];
    f32 constantTerm = gabi::fmuls_ppc(constant,1.0f);
    f32 linearTerm = gabi::fmuls_ppc(linear,fraction);
    f32 quadraticTerm = gabi::fmuls_ppc(quadratic,fraction);
    f32 cubicTerm = gabi::fmuls_ppc(cubic,fraction);
    quadraticTerm = gabi::fmuls_ppc(quadraticTerm,fraction);
    cubicTerm = gabi::fmuls_ppc(cubicTerm,fraction);
    quadraticTerm = gabi::fmuls_ppc(quadraticTerm,1.0f);
    cubicTerm = gabi::fmuls_ppc(cubicTerm,fraction);
    f32 evenTerms = gabi::fadds_ppc(constantTerm,quadraticTerm);
    f32 oddTerms = gabi::fadds_ppc(linearTerm,cubicTerm);
    return gabi::fadds_ppc(evenTerms,oddTerms);
}
/* 027E9A48 */
f32 AnimationCurve_l::linearFloat(f32 frame) const {
    u32 start = gabi::ftoi(mStartFrame);
    s32 wholeFrame = gabi::ftoi(frame);
    u32 index = static_cast<u32>(wholeFrame) - start;
    const be<f32>* values = relativeTarget<be<f32>>(&mValuesRelative);
    f32 fraction = gabi::fsubs_ppc(frame,static_cast<f32>(wholeFrame));
    f32 first = sead::elementAt(values, static_cast<u32>(index));
    f32 complement = gabi::fsubs_ppc(1.0f,fraction);
    f32 weighted = gabi::fmuls_ppc(first,complement);
    f32 second = sead::elementAt(values, static_cast<u32>(index + 1));
    return gabi::fmadds(second,fraction,weighted);
}
/* 027E9AE0 */
f32 AnimationCurve_l::linearShort(f32 frame) const {
    u32 start = gabi::ftoi(mStartFrame);
    s32 wholeFrame = gabi::ftoi(frame);
    u32 index = static_cast<u32>(wholeFrame) - start;
    const be<s16>* values = relativeTarget<be<s16>>(&mValuesRelative);
    f32 fraction = gabi::fsubs_ppc(frame,static_cast<f32>(wholeFrame));
    f32 second = sead::elementAt(values, static_cast<u32>(index + 1));
    f32 first = sead::elementAt(values, static_cast<u32>(index));
    f32 complement = gabi::fsubs_ppc(1.0f,fraction);
    f32 weighted = gabi::fmuls_ppc(first,complement);
    return gabi::fmadds(second,fraction,weighted);
}
/* 027E9B90 */
f32 AnimationCurve_l::linearByte(f32 frame) const {
    u32 start = gabi::ftoi(mStartFrame);
    s32 wholeFrame = gabi::ftoi(frame);
    u32 index = static_cast<u32>(wholeFrame) - start;
    const be<s8>* values = relativeTarget<be<s8>>(&mValuesRelative);
    f32 fraction = gabi::fsubs_ppc(frame,static_cast<f32>(wholeFrame));
    f32 second = values[index + 1];
    f32 first = values[index];
    f32 complement = gabi::fsubs_ppc(1.0f,fraction);
    f32 weighted = gabi::fmuls_ppc(first,complement);
    return gabi::fmadds(second,fraction,weighted);
}
/* 027E9D28 */
u32 Segment3::intersectPlane(const Plane* plane,be<f32>* fraction) const {
    f32 beginZ = mBegin.z;
    f32 endZ = mEnd.z;
    f32 beginX = mBegin.x;
    f32 beginY = mBegin.y;
    f32 deltaZ = gabi::fsubs_ppc(endZ,beginZ);
    f32 normalX = plane->mNormal.x;
    f32 normalY = plane->mNormal.y;
    f32 endX = mEnd.x;
    f32 endY = mEnd.y;
    f32 beginDotX = gabi::fmuls_ppc(normalX,beginX);
    f32 beginDotY = gabi::fmuls_ppc(normalY,beginY);
    f32 deltaX = gabi::fsubs_ppc(endX,beginX);
    f32 deltaY = gabi::fsubs_ppc(endY,beginY);
    f32 beginDotXY = gabi::fadds_ppc(beginDotX,beginDotY);
    f32 normalZ = plane->mNormal.z;
    f32 deltaDotX = gabi::fmuls_ppc(normalX,deltaX);
    f32 deltaDotY = gabi::fmuls_ppc(normalY,deltaY);
    f32 distance = plane->mDistance;
    f32 deltaDotXY = gabi::fadds_ppc(deltaDotX,deltaDotY);
    f32 beginDot = gabi::fmadds(normalZ,beginZ,beginDotXY);
    f32 directionDot = gabi::fmadds(normalZ,deltaZ,deltaDotXY);
    f32 offset = gabi::fsubs_ppc(beginDot,distance);
    f32 numerator = -offset;
    if (directionDot == 0.0f) {
        if (numerator != 0.0f) return 0;
        if (fraction) *fraction = 0.0f;
        return 3;
    }
    f32 intersection = gabi::fdivs_ppc(numerator,directionDot);
    if (intersection < 0.0f || intersection > 1.0f) return 0;
    if (fraction) *fraction = intersection;
    return 1;
}
/* 027EA1F0 */
static void __sinit_CurveBasisSupport_cpp() {
    sCurveBasis.mValues[0] = 2.0f;
    sCurveBasis.mValues[1] = -2.0f;
    sCurveBasis.mValues[2] = 1.0f;
    sCurveBasis.mValues[3] = 1.0f;
    sCurveBasis.mValues[4] = -3.0f;
    sCurveBasis.mValues[5] = 3.0f;
    sCurveBasis.mValues[6] = -2.0f;
    sCurveBasis.mValues[7] = -1.0f;
    sCurveBasis.mValues[8] = 0.0f;
    sCurveBasis.mValues[9] = 0.0f;
    sCurveBasis.mValues[10] = 1.0f;
    sCurveBasis.mValues[11] = 0.0f;
    sCurveBasis.mValues[12] = 1.0f;
    sCurveBasis.mValues[13] = 0.0f;
    sCurveBasis.mValues[14] = 0.0f;
    sCurveBasis.mValues[15] = 0.0f;
}
/* 027EA270 */
f32 CurveSamples::linear(const be<f32>* values, f32 position) const {
    if (position < 0.0f) return values[0];
    s32 last = static_cast<u32>(mScalarCount) - 1;
    f32 scaled = gabi::fmuls_ppc(position, static_cast<f32>(last));
    s32 index = gabi::ftoi(scaled);
    if (index >= last) return sead::elementAt(values, static_cast<u32>(index));
    f32 first = sead::elementAt(values, static_cast<u32>(index));
    f32 second = sead::elementAt(values, static_cast<u32>(index) + 1);
    f32 delta = gabi::fsubs_ppc(second, first);
    f32 fraction = gabi::fsubs_ppc(scaled, static_cast<f32>(index));
    return gabi::fmadds(delta, fraction, first);
}
/* 027EA32C */
f32 CurveSamples::hermite(const be<f32>* values, f32 position) const {
    u32 count = mScalarCount;
    if (count & 1) return 0.0f;
    if (position < 0.0f) return values[0];
    s32 last = (count >> 1) - 1;
    f32 scaled = gabi::fmuls_ppc(position, static_cast<f32>(last));
    s32 index = gabi::ftoi(scaled);
    f32 fraction = gabi::fsubs_ppc(scaled, static_cast<f32>(index));
    u32 scalar = static_cast<u32>(index) * 2;
    if (index >= last) return sead::elementAt(values, static_cast<u32>(scalar));
    f32 square = gabi::fmuls_ppc(fraction, fraction);
    f32 twice = gabi::fadds_ppc(fraction, fraction);
    f32 triple = gabi::fmuls_ppc(3.0f, fraction);
    f32 twiceSquare = gabi::fmuls_ppc(twice, fraction);
    f32 negativeTwice = gabi::fmuls_ppc(-2.0f, fraction);
    f32 tripleSquare = gabi::fmuls_ppc(triple, fraction);
    f32 firstTerm = gabi::fmsubs(twiceSquare, fraction, tripleSquare);
    f32 cube = gabi::fmuls_ppc(square, fraction);
    f32 negativeTwiceSquare = gabi::fmuls_ppc(negativeTwice, fraction);
    f32 secondWeight = gabi::fmadds(negativeTwiceSquare, fraction, tripleSquare);
    f32 firstTangentWeight = gabi::fsubs_ppc(cube, twiceSquare);
    f32 secondValue = sead::elementAt(values, static_cast<u32>(scalar + 2));
    f32 firstWeight = gabi::fadds_ppc(firstTerm, 1.0f);
    f32 result = gabi::fmuls_ppc(secondValue, secondWeight);
    f32 firstValue = sead::elementAt(values, static_cast<u32>(scalar));
    firstTangentWeight = gabi::fadds_ppc(firstTangentWeight, fraction);
    result = gabi::fmadds(firstValue, firstWeight, result);
    f32 firstTangent = sead::elementAt(values, static_cast<u32>(scalar + 1));
    f32 secondTangentWeight = gabi::fsubs_ppc(cube, square);
    result = gabi::fmadds(firstTangent, firstTangentWeight, result);
    f32 secondTangent = sead::elementAt(values, static_cast<u32>(scalar + 3));
    return gabi::fmadds(secondTangent, secondTangentWeight, result);
}
/* 027EA44C */
f32 CurveSamples::step(const be<f32>* values, f32 position) const {
    if (position < 0.0f) position = 0.0f;
    else if (position > 1.0f) position = 1.0f;
    s32 last = static_cast<u32>(mScalarCount) - 1;
    s32 index = gabi::ftoi(gabi::fmuls_ppc(position, static_cast<f32>(last)));
    return sead::elementAt(values, static_cast<u32>(index));
}
/* 027EA514 */
f32 CurveSamples::sine(const be<f32>* values, f32 position) const {
    f32 frequency = values[0];
    f32 phase = gabi::fmuls_ppc(position, frequency);
    phase = gabi::fmuls_ppc(phase, 6.283185482025146484375f);
    f64 wave = utilitySin(phase);
    f32 amplitude = values[1];
    return singleMultiplyWide(wave, amplitude);
}
/* 027EA55C */
f32 CurveSamples::cosine(const be<f32>* values, f32 position) const {
    f32 frequency = values[0];
    f32 phase = gabi::fmuls_ppc(position, frequency);
    phase = gabi::fmuls_ppc(phase, 6.283185482025146484375f);
    f64 wave = utilityCos(phase);
    f32 amplitude = values[1];
    return singleMultiplyWide(wave, amplitude);
}
/* 027EA5A4 */
f32 CurveSamples::squaredSine(const be<f32>* values, f32 position) const {
    f32 frequency = values[0];
    f32 phase = gabi::fmuls_ppc(position, frequency);
    phase = gabi::fmuls_ppc(phase, 6.283185482025146484375f);
    f64 wave = utilitySin(phase);
    f32 square = singleMultiplyWide(wave, wave);
    f32 amplitude = values[1];
    return gabi::fmuls_ppc(square, amplitude);
}
/* 027EA5F0 */
f32 CurveSamples::linearKeys(const be<f32>* values, f32 position) const {
    f32 firstPosition = values[0];
    u32 scalars = mScalarCount;
    u32 last = (scalars & ~1u) - 2;
    u32 count = scalars >> 1;
    if (!(position > firstPosition)) return values[1];
    f32 lastPosition = sead::elementAt(values, static_cast<u32>(last));
    if (!(position < lastPosition)) return sead::elementAt(values, static_cast<u32>(last + 1));
    for (u32 key = 0; key < count; ++key) {
        u32 scalar = key * 2;
        f32 nextPosition = sead::elementAt(values, static_cast<u32>(scalar + 2));
        if (position < nextPosition) {
            f32 beginPosition = sead::elementAt(values, static_cast<u32>(scalar));
            f32 beginValue = sead::elementAt(values, static_cast<u32>(scalar + 1));
            f32 elapsed = gabi::fsubs_ppc(position, beginPosition);
            f32 endValue = sead::elementAt(values, static_cast<u32>(scalar + 3));
            f32 duration = gabi::fsubs_ppc(nextPosition, beginPosition);
            f32 difference = gabi::fsubs_ppc(endValue, beginValue);
            f32 fraction = gabi::fdivs_ppc(elapsed, duration);
            return gabi::fmadds(difference, fraction, beginValue);
        }
    }
    return 0.0f;
}
/* 027EA68C */
f32 CurveSamples::hermiteKeys(const be<f32>* values, f32 position) const {
    u32 count = (static_cast<u32>(mScalarCount) * 0x5556u) >> 16;
    f32 firstPosition = values[0];
    u32 last = count * 3 - 3;
    if (!(position > firstPosition)) return values[1];
    f32 lastPosition = sead::elementAt(values, static_cast<u32>(last));
    if (!(position < lastPosition)) return sead::elementAt(values, static_cast<u32>(last + 1));
    for (u32 key = 0; key < count; ++key) {
        u32 scalar = key * 3;
        f32 endPosition = sead::elementAt(values, static_cast<u32>(scalar + 3));
        if (position < endPosition) {
            f32 beginPosition = sead::elementAt(values, static_cast<u32>(scalar));
            f32 elapsed = gabi::fsubs_ppc(position, beginPosition);
            f32 duration = gabi::fsubs_ppc(endPosition, beginPosition);
            f32 fraction = gabi::fdivs_ppc(elapsed, duration);
            f32 square = gabi::fmuls_ppc(fraction, fraction);
            f32 triple = gabi::fmuls_ppc(3.0f, fraction);
            f32 cube = gabi::fmuls_ppc(square, fraction);
            f32 twice = gabi::fadds_ppc(fraction, fraction);
            f32 tripleSquare = gabi::fmuls_ppc(triple, fraction);
            f32 twiceSquare = gabi::fmuls_ppc(twice, fraction);
            f32 negativeTwice = gabi::fmuls_ppc(-2.0f, fraction);
            f32 firstTerm = gabi::fmsubs(twiceSquare, fraction, tripleSquare);
            f32 negativeTwiceSquare = gabi::fmuls_ppc(negativeTwice, fraction);
            f32 firstTangentWeight = gabi::fsubs_ppc(cube, twiceSquare);
            f32 secondWeight = gabi::fmadds(negativeTwiceSquare, fraction, tripleSquare);
            f32 secondValue = sead::elementAt(values, static_cast<u32>(scalar + 4));
            f32 firstWeight = gabi::fadds_ppc(firstTerm, 1.0f);
            f32 result = gabi::fmuls_ppc(secondValue, secondWeight);
            f32 firstValue = sead::elementAt(values, static_cast<u32>(scalar + 1));
            firstTangentWeight = gabi::fadds_ppc(firstTangentWeight, fraction);
            result = gabi::fmadds(firstValue, firstWeight, result);
            f32 firstTangent = sead::elementAt(values, static_cast<u32>(scalar + 2));
            f32 secondTangentWeight = gabi::fsubs_ppc(cube, square);
            result = gabi::fmadds(firstTangent, firstTangentWeight, result);
            f32 secondTangent = sead::elementAt(values, static_cast<u32>(scalar + 5));
            return gabi::fmadds(secondTangent, secondTangentWeight, result);
        }
    }
    return 0.0f;
}
/* 027EA78C */
f32 CurveSamples::stepKeys(const be<f32>* values, f32 position) const {
    f32 firstPosition = values[0];
    u32 scalars = mScalarCount;
    u32 last = (scalars & ~1u) - 2;
    u32 count = scalars >> 1;
    if (!(position > firstPosition)) return values[1];
    f32 lastPosition = sead::elementAt(values, static_cast<u32>(last));
    if (!(position < lastPosition)) return sead::elementAt(values, static_cast<u32>(last + 1));
    for (u32 key = 0; key < count; ++key) {
        u32 scalar = key * 2;
        f32 nextPosition = sead::elementAt(values, static_cast<u32>(scalar + 2));
        if (position < nextPosition) return sead::elementAt(values, static_cast<u32>(scalar + 1));
    }
    return 0.0f;
}
/* 027EA80C */
f32 CurveSamples::zero(const be<f32>*, f32) const { return 0.0f; }
/* 027EA818 */
f32 CurveSamples::hermiteTimeKeys(const be<f32>* values, f32 position) const {
    u32 count = (static_cast<u32>(mScalarCount) * 0x5556u) >> 16;
    f32 firstPosition = values[0];
    u32 last = count * 3 - 3;
    if (!(position > firstPosition)) return values[1];
    f32 lastPosition = sead::elementAt(values, static_cast<u32>(last));
    if (!(position < lastPosition)) return sead::elementAt(values, static_cast<u32>(last + 1));
    for (u32 key = 0; key < count; ++key) {
        u32 scalar = key * 3;
        f32 endPosition = sead::elementAt(values, static_cast<u32>(scalar + 3));
        if (position < endPosition) {
            f32 beginPosition = sead::elementAt(values, static_cast<u32>(scalar));
            f32 elapsed = gabi::fsubs_ppc(position, beginPosition);
            f32 duration = gabi::fsubs_ppc(endPosition, beginPosition);
            f32 fraction = gabi::fdivs_ppc(elapsed, duration);
            f32 square = gabi::fmuls_ppc(fraction, fraction);
            f32 twice = gabi::fadds_ppc(fraction, fraction);
            f32 cube = gabi::fmuls_ppc(square, fraction);
            f32 twiceSquare = gabi::fmuls_ppc(twice, fraction);
            f32 triple = gabi::fmuls_ppc(3.0f, fraction);
            f32 firstTangentWeight = gabi::fsubs_ppc(cube, twiceSquare);
            f32 negativeTwice = gabi::fmuls_ppc(-2.0f, fraction);
            f32 tripleSquare = gabi::fmuls_ppc(triple, fraction);
            f32 negativeTwiceSquare = gabi::fmuls_ppc(negativeTwice, fraction);
            firstTangentWeight = gabi::fadds_ppc(firstTangentWeight, fraction);
            f32 firstTerm = gabi::fmsubs(twiceSquare, fraction, tripleSquare);
            f32 secondWeight = gabi::fmadds(negativeTwiceSquare, fraction, tripleSquare);
            f32 secondValue = sead::elementAt(values, static_cast<u32>(scalar + 4));
            f32 firstWeight = gabi::fadds_ppc(firstTerm, 1.0f);
            f32 firstValue = sead::elementAt(values, static_cast<u32>(scalar + 1));
            f32 result = gabi::fmuls_ppc(secondValue, secondWeight);
            f32 firstTangent = sead::elementAt(values, static_cast<u32>(scalar + 2));
            result = gabi::fmadds(firstValue, firstWeight, result);
            firstTangentWeight = gabi::fmuls_ppc(firstTangentWeight, duration);
            f32 secondTangentWeight = gabi::fsubs_ppc(cube, square);
            result = gabi::fmadds(firstTangent, firstTangentWeight, result);
            secondTangentWeight = gabi::fmuls_ppc(secondTangentWeight, duration);
            f32 secondTangent = sead::elementAt(values, static_cast<u32>(scalar + 5));
            return gabi::fmadds(secondTangent, secondTangentWeight, result);
        }
    }
    return 0.0f;
}
}
/* 027EA920 */
static void __sinit_CurveSamplerSupport_cpp() {
    gabi::AutoFrame frame{gabi::current_function()};
    sCurveSamplerStatic.mWords[2] = 0;
    sCurveSamplerStatic.mWords[0] = 0;
    sCurveSamplerStatic.mWords[3] = 0;
    sCurveSamplerStatic.mWords[1] = 0;
    curveRegisterStatic(&sCurveSamplerRegistration);
}
/* WWHD glue
ext 028F5680 Pair32 transformAngleIndex(f64)
data sCurveBasis 104A22F4
data sCurveSamplerStatic 104A2334
data sCurveSamplerRegistration 101F96F4
ext 028F026C void curveRegisterStatic(void*)
ext 027E04C8 void evaluateTransformCached(agl_gx2_support::TransformTrack_l*,agl_gx2_support::TransformResult_l*,agl_gx2_support::SampleCursor_l*,f32)
ext 027E0560 void evaluateTransformUncached(agl_gx2_support::TransformTrack_l*,agl_gx2_support::TransformResult_l*,f32)
ext 027DECC4 u32 sampleChannelInteger(agl_gx2_support::AnimationCurve_l*,agl_gx2_support::SampleCursor_l*,f32)
ext 027DEC10 f32 sampleChannelFloat(agl_gx2_support::AnimationCurve_l*,agl_gx2_support::SampleCursor_l*,f32)
ext 027E1C34 void updateCurveCursor(agl_gx2_support::AnimationCurve_l*,agl_gx2_support::SampleCursor_l*,f32)
ext C000A848 void utilityCopy(void*,const void*,u32)
ext 028F43F8 f64 utilitySin(f64)
ext 028F4BE0 f64 utilityCos(f64)
*/
