/* Blend pose finalization reconstructed from the assigned WWHD functions. */
#include "agl/gx2/BlendPoseSupport.h"
using namespace blend_pose_support;
using agl_gx2_support::TransformBone_l;
namespace blend_pose_support {
static f32 readTransformScalar(const be<u32>* word) {
    return *reinterpret_cast<const be<f32>*>(word);
}
/* 027E7164 */
void PoseBlend_l::accumulateQuaternionAxes(agl_gx2_support::TransformRuntime_l* animation,f32 weight) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    animation->mState = static_cast<u32>(animation->mState) | 8;
    const agl_gx2_support::TransformResult_l* result = animation->mResults;
    mState = static_cast<u32>(mState) | 8;
    u32 count = animation->mBindings.mCount;
    BlendPose_l* data = mData;
    for (u32 target = 0; target < count; ++target,++result) {
        const be<u32>* entries = animation->mBindings.mEntries;
        u32 binding = entries[target];
        if (binding & agl_gx2_support::SkipApplication) continue;
        f32 w = result->mRotation[3].mValue;
        f32 x = result->mRotation[0].mValue;
        f32 twiceW = gabi::fadds_ppc(w,w);
        f32 z = result->mRotation[2].mValue;
        f32 twiceX = gabi::fadds_ppc(x,x);
        f32 y = result->mRotation[1].mValue;
        f32 twiceZ = gabi::fadds_ppc(z,z);
        f32 twiceY = gabi::fadds_ppc(y,y);
        f32 xy = gabi::fmuls_ppc(twiceX,y);
        f32 wz = gabi::fmuls_ppc(twiceW,z);
        f32 wy = gabi::fmuls_ppc(twiceW,y);
        f32 withoutY = gabi::fnmsubs(twiceY,y,1.0f);
        f32 withoutX = gabi::fnmsubs(twiceX,x,1.0f);
        BlendPose_l* pose = data + (binding & 0x7FFF);
        f32 zz = gabi::fmuls_ppc(twiceZ,z);
        f32 rowYx = gabi::fadds_ppc(xy,wz);
        f32 rowXz = gabi::fmadds(twiceX,z,wy);
        f32 rowYy = gabi::fsubs_ppc(withoutX,zz);
        f32 rowXx = gabi::fsubs_ppc(withoutY,zz);
        f32 rowXy = gabi::fsubs_ppc(xy,wz);
        f32 yz = gabi::fmuls_ppc(twiceY,z);
        f32 rowYz = gabi::fnmsubs(twiceW,x,yz);
        f32 oldOrientation[6];
        oldOrientation[0] = pose->mOrientation[0].mValue;
        oldOrientation[1] = pose->mOrientation[1].mValue;
        oldOrientation[2] = pose->mOrientation[2].mValue;
        oldOrientation[3] = pose->mOrientation[3].mValue;
        pose->mOrientation[0].mValue = gabi::fmadds(rowXx,weight,oldOrientation[0]);
        pose->mOrientation[1].mValue = gabi::fmadds(rowXy,weight,oldOrientation[1]);
        oldOrientation[4] = pose->mOrientation[4].mValue;
        oldOrientation[5] = pose->mOrientation[5].mValue;
        pose->mOrientation[2].mValue = gabi::fmadds(rowXz,weight,oldOrientation[2]);
        pose->mOrientation[3].mValue = gabi::fmadds(rowYx,weight,oldOrientation[3]);
        pose->mOrientation[4].mValue = gabi::fmadds(rowYy,weight,oldOrientation[4]);
        pose->mOrientation[5].mValue = gabi::fmadds(rowYz,weight,oldOrientation[5]);
        f32 scaleX = readTransformScalar(&result->mScale[0]);
        f32 scaleY = readTransformScalar(&result->mScale[1]);
        f32 oldScaleX = pose->mScale[0].mValue;
        f32 oldScaleY = pose->mScale[1].mValue;
        f32 scaledX = gabi::fmadds(scaleX,weight,oldScaleX);
        f32 scaledY = gabi::fmadds(scaleY,weight,oldScaleY);
        f32 oldScaleZ = pose->mScale[2].mValue;
        f32 oldTranslationX = pose->mTranslation[0].mValue;
        pose->mScale[0].mValue = scaledX;
        pose->mScale[1].mValue = scaledY;
        f32 scaleZ = readTransformScalar(&result->mScale[2]);
        f32 translationX = readTransformScalar(&result->mTranslation[0]);
        f32 scaledZ = gabi::fmadds(scaleZ,weight,oldScaleZ);
        f32 translatedX = gabi::fmadds(translationX,weight,oldTranslationX);
        f32 oldTranslationY = pose->mTranslation[1].mValue;
        f32 oldTranslationZ = pose->mTranslation[2].mValue;
        pose->mScale[2].mValue = scaledZ;
        pose->mTranslation[0].mValue = translatedX;
        f32 translationY = readTransformScalar(&result->mTranslation[1]);
        f32 translationZ = readTransformScalar(&result->mTranslation[2]);
        f32 oldWeight = pose->mWeight;
        f32 translatedY = gabi::fmadds(translationY,weight,oldTranslationY);
        f32 translatedZ = gabi::fmadds(translationZ,weight,oldTranslationZ);
        u32 oldFlags = pose->mFlags;
        f32 newWeight = gabi::fadds_ppc(oldWeight,weight);
        pose->mTranslation[1].mValue = translatedY;
        pose->mTranslation[2].mValue = translatedZ;
        u32 flags = result->mFlags;
        pose->mWeight = newWeight;
        pose->mFlags = oldFlags | ~flags;
    }
}
/* 027E72CC */
void PoseBlend_l::accumulateEulerAxes(agl_gx2_support::TransformRuntime_l* animation,f32 weight) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    animation->mState = static_cast<u32>(animation->mState) | 8;
    const agl_gx2_support::TransformResult_l* result = animation->mResults;
    mState = static_cast<u32>(mState) | 8;
    u32 count = animation->mBindings.mCount;
    BlendPose_l* data = mData;
    for (u32 target = 0; target < count; ++target,++result) {
        const be<u32>* entries = animation->mBindings.mEntries;
        u32 binding = entries[target];
        if (binding & agl_gx2_support::SkipApplication) continue;
        BlendPose_l* pose = data + (binding & 0x7FFF);
        f32 rotationX = result->mRotation[0].mValue;
        auto angleX = transformAngleIndex(gabi::fmuls_ppc(rotationX,683565248.0f));
        f32 rotationY = result->mRotation[1].mValue;
        f32 fractionX = gabi::fmuls_ppc(static_cast<f32>((angleX.r4 >> 8) & 0xFFFF),1.52590218931436538696e-5f);
        f32 sineX = agl_gx2_support::angleSine(angleX.r4,fractionX);
        f32 cosineX = agl_gx2_support::angleCosine(angleX.r4,fractionX);
        auto angleY = transformAngleIndex(gabi::fmuls_ppc(rotationY,683565248.0f));
        f32 rotationZ = result->mRotation[2].mValue;
        f32 fractionY = gabi::fmuls_ppc(static_cast<f32>((angleY.r4 >> 8) & 0xFFFF),1.52590218931436538696e-5f);
        f32 sineY = agl_gx2_support::angleSine(angleY.r4,fractionY);
        f32 cosineY = agl_gx2_support::angleCosine(angleY.r4,fractionY);
        auto angleZ = transformAngleIndex(gabi::fmuls_ppc(rotationZ,683565248.0f));
        f32 fractionZ = gabi::fmuls_ppc(static_cast<f32>((angleZ.r4 >> 8) & 0xFFFF),1.52590218931436538696e-5f);
        f32 sineZ = agl_gx2_support::angleSine(angleZ.r4,fractionZ);
        f32 cosineZ = agl_gx2_support::angleCosine(angleZ.r4,fractionZ);
        f32 rowYx = gabi::fmuls_ppc(sineZ,cosineY);
        f32 rowXx = gabi::fmuls_ppc(cosineZ,cosineY);
        f32 cosineXsineZ = gabi::fmuls_ppc(cosineX,sineZ);
        f32 sineXcosineZ = gabi::fmuls_ppc(sineX,cosineZ);
        f32 rowXy = gabi::fmsubs(sineXcosineZ,sineY,cosineXsineZ);
        f32 rowYz = gabi::fmsubs(cosineXsineZ,sineY,sineXcosineZ);
        f32 sineXsineZ = gabi::fmuls_ppc(sineX,sineZ);
        f32 cosineXcosineZ = gabi::fmuls_ppc(cosineX,cosineZ);
        f32 rowXz = gabi::fmadds(cosineXcosineZ,sineY,sineXsineZ);
        f32 rowYy = gabi::fmadds(sineXsineZ,sineY,cosineXcosineZ);
        f32 oldOrientation[6];
        oldOrientation[0] = pose->mOrientation[0].mValue;
        oldOrientation[1] = pose->mOrientation[1].mValue;
        oldOrientation[2] = pose->mOrientation[2].mValue;
        oldOrientation[3] = pose->mOrientation[3].mValue;
        pose->mOrientation[0].mValue = gabi::fmadds(rowXx,weight,oldOrientation[0]);
        pose->mOrientation[1].mValue = gabi::fmadds(rowXy,weight,oldOrientation[1]);
        oldOrientation[4] = pose->mOrientation[4].mValue;
        oldOrientation[5] = pose->mOrientation[5].mValue;
        pose->mOrientation[2].mValue = gabi::fmadds(rowXz,weight,oldOrientation[2]);
        pose->mOrientation[3].mValue = gabi::fmadds(rowYx,weight,oldOrientation[3]);
        pose->mOrientation[4].mValue = gabi::fmadds(rowYy,weight,oldOrientation[4]);
        pose->mOrientation[5].mValue = gabi::fmadds(rowYz,weight,oldOrientation[5]);
        f32 scaleX = readTransformScalar(&result->mScale[0]);
        f32 scaleY = readTransformScalar(&result->mScale[1]);
        f32 oldScaleX = pose->mScale[0].mValue;
        f32 oldScaleY = pose->mScale[1].mValue;
        f32 scaledX = gabi::fmadds(scaleX,weight,oldScaleX);
        f32 scaledY = gabi::fmadds(scaleY,weight,oldScaleY);
        f32 oldScaleZ = pose->mScale[2].mValue;
        f32 oldTranslationX = pose->mTranslation[0].mValue;
        pose->mScale[0].mValue = scaledX;
        pose->mScale[1].mValue = scaledY;
        f32 scaleZ = readTransformScalar(&result->mScale[2]);
        f32 translationX = readTransformScalar(&result->mTranslation[0]);
        f32 scaledZ = gabi::fmadds(scaleZ,weight,oldScaleZ);
        f32 translatedX = gabi::fmadds(translationX,weight,oldTranslationX);
        f32 oldTranslationY = pose->mTranslation[1].mValue;
        f32 oldTranslationZ = pose->mTranslation[2].mValue;
        pose->mScale[2].mValue = scaledZ;
        pose->mTranslation[0].mValue = translatedX;
        f32 translationY = readTransformScalar(&result->mTranslation[1]);
        f32 translationZ = readTransformScalar(&result->mTranslation[2]);
        f32 oldWeight = pose->mWeight;
        f32 translatedY = gabi::fmadds(translationY,weight,oldTranslationY);
        f32 translatedZ = gabi::fmadds(translationZ,weight,oldTranslationZ);
        u32 oldFlags = pose->mFlags;
        f32 newWeight = gabi::fadds_ppc(oldWeight,weight);
        pose->mTranslation[1].mValue = translatedY;
        pose->mTranslation[2].mValue = translatedZ;
        u32 flags = result->mFlags;
        pose->mWeight = newWeight;
        pose->mFlags = oldFlags | ~flags;
    }
}
static u32 blendAngleUnsigned(f32 value) {
    if (value < 2147483648.0f) return static_cast<u32>(gabi::ftoi(value));
    return static_cast<u32>(gabi::ftoi(gabi::fsubs_ppc(value,2147483648.0f))) + 0x80000000u;
}
/* 027E7538 */
void PoseBlend_l::accumulateQuaternion(agl_gx2_support::TransformRuntime_l* animation,f32 weight) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    animation->mState = static_cast<u32>(animation->mState) | 8;
    const agl_gx2_support::TransformResult_l* result = animation->mResults;
    mState = static_cast<u32>(mState) | 8;
    u32 count = animation->mBindings.mCount;
    BlendPose_l* data = mData;
    for (u32 target = 0; target < count; ++target,++result) {
        const be<u32>* entries = animation->mBindings.mEntries;
        u32 binding = entries[target];
        if (binding & agl_gx2_support::SkipApplication) continue;
        BlendPose_l* pose = data + (binding & 0x7FFF);
        f32 oldWeight = pose->mWeight;
        if (oldWeight == 0.0f) {
            for (u32 component = 0; component < 4; ++component) pose->mOrientation[component].mBits = result->mRotation[component].mBits;
        } else {
            f32 totalWeight = gabi::fadds_ppc(oldWeight,weight);
            f32 estimate = static_cast<f32>(gabi::fres(totalWeight));
            f32 doubled = gabi::fadds_ppc(estimate,estimate);
            f32 product = gabi::fmuls_ppc(totalWeight,estimate);
            f32 inverse = gabi::fnmsubs(product,estimate,doubled);
            f32 factor = gabi::fmuls_ppc(weight,inverse);
            f32 incomingX = result->mRotation[0].mValue;
            f32 incomingY = result->mRotation[1].mValue;
            f32 oldX = pose->mOrientation[0].mValue;
            f32 oldY = pose->mOrientation[1].mValue;
            f32 dotX = gabi::fmuls_ppc(oldX,incomingX);
            f32 dotY = gabi::fmuls_ppc(oldY,incomingY);
            f32 oldZ = pose->mOrientation[2].mValue;
            f32 oldW = pose->mOrientation[3].mValue;
            f32 incomingZ = result->mRotation[2].mValue;
            f32 incomingW = result->mRotation[3].mValue;
            dotX = gabi::fmadds(oldZ,incomingZ,dotX);
            dotY = gabi::fmadds(oldW,incomingW,dotY);
            f32 dot = gabi::fadds_ppc(dotX,dotY);
            f32 magnitude = __builtin_fabsf(dot);
            f32 sign = static_cast<f32>(ppc_fsel(dot,-1.0,1.0));
            f32 oldFactor;
            f32 incomingFactor;
            bool linear = magnitude > 0.999989986419677734375f;
            if (linear) {
                incomingFactor = gabi::fmuls_ppc(sign,factor);
                oldFactor = gabi::fsubs_ppc(1.0f,factor);
            } else {
                f64 angle = blendAcos(static_cast<f64>(magnitude));
                f32 scaled = agl_gx2_support::singleMultiplyWide(angle,683565248.0f);
                auto converted = transformAngleIndex(scaled);
                u32 wholeAngle = converted.r4;
                f32 fraction = gabi::fmuls_ppc(static_cast<f32>(wholeAngle & 0xFFFFFF),0x1p-24f);
                f32 sine = agl_gx2_support::angleSine(wholeAngle,fraction);
                f32 part = gabi::fmuls_ppc(factor,static_cast<f32>(wholeAngle));
                f32 sineEstimate = static_cast<f32>(gabi::fres(sine));
                f32 sineDoubled = gabi::fadds_ppc(sineEstimate,sineEstimate);
                f32 sineProduct = gabi::fmuls_ppc(sine,sineEstimate);
                f32 inverseSine = gabi::fnmsubs(sineProduct,sineEstimate,sineDoubled);
                u32 partAngle = blendAngleUnsigned(part);
                u32 remainder = wholeAngle - partAngle;
                f32 remainderFraction = gabi::fmuls_ppc(static_cast<f32>(remainder & 0xFFFFFF),0x1p-24f);
                f32 partFraction = gabi::fmuls_ppc(static_cast<f32>(partAngle & 0xFFFFFF),0x1p-24f);
                f32 remainderSine = agl_gx2_support::angleSine(remainder,remainderFraction);
                f32 partSine = agl_gx2_support::angleSine(partAngle,partFraction);
                oldFactor = gabi::fmuls_ppc(remainderSine,inverseSine);
                f32 unsignedFactor = gabi::fmuls_ppc(partSine,inverseSine);
                incomingFactor = gabi::fmuls_ppc(sign,unsignedFactor);
                incomingX = result->mRotation[0].mValue;
                incomingY = result->mRotation[1].mValue;
                oldX = pose->mOrientation[0].mValue;
                oldY = pose->mOrientation[1].mValue;
            }
            f32 weightedX = gabi::fmuls_ppc(incomingX,incomingFactor);
            f32 weightedY = gabi::fmuls_ppc(incomingY,incomingFactor);
            f32 newX = gabi::fmadds(oldX,oldFactor,weightedX);
            f32 newY = gabi::fmadds(oldY,oldFactor,weightedY);
            pose->mOrientation[0].mValue = newX;
            pose->mOrientation[1].mValue = newY;
            incomingZ = result->mRotation[2].mValue;
            incomingW = result->mRotation[3].mValue;
            oldZ = pose->mOrientation[2].mValue;
            oldW = pose->mOrientation[3].mValue;
            f32 weightedZ = gabi::fmuls_ppc(incomingZ,incomingFactor);
            f32 weightedW = gabi::fmuls_ppc(incomingW,incomingFactor);
            pose->mOrientation[2].mValue = gabi::fmadds(oldZ,oldFactor,weightedZ);
            pose->mOrientation[3].mValue = gabi::fmadds(oldW,oldFactor,weightedW);
        }
        f32 scaleX = readTransformScalar(&result->mScale[0]);
        f32 scaleY = readTransformScalar(&result->mScale[1]);
        f32 oldScaleX = pose->mScale[0].mValue;
        f32 oldScaleY = pose->mScale[1].mValue;
        f32 scaledX = gabi::fmadds(scaleX,weight,oldScaleX);
        f32 scaledY = gabi::fmadds(scaleY,weight,oldScaleY);
        f32 oldScaleZ = pose->mScale[2].mValue;
        f32 oldTranslationX = pose->mTranslation[0].mValue;
        pose->mScale[0].mValue = scaledX;
        pose->mScale[1].mValue = scaledY;
        f32 scaleZ = readTransformScalar(&result->mScale[2]);
        f32 translationX = readTransformScalar(&result->mTranslation[0]);
        f32 scaledZ = gabi::fmadds(scaleZ,weight,oldScaleZ);
        f32 translatedX = gabi::fmadds(translationX,weight,oldTranslationX);
        f32 oldTranslationY = pose->mTranslation[1].mValue;
        f32 oldTranslationZ = pose->mTranslation[2].mValue;
        pose->mScale[2].mValue = scaledZ;
        pose->mTranslation[0].mValue = translatedX;
        f32 translationY = readTransformScalar(&result->mTranslation[1]);
        f32 translationZ = readTransformScalar(&result->mTranslation[2]);
        f32 currentWeight = pose->mWeight;
        f32 translatedY = gabi::fmadds(translationY,weight,oldTranslationY);
        f32 translatedZ = gabi::fmadds(translationZ,weight,oldTranslationZ);
        u32 oldFlags = pose->mFlags;
        f32 newWeight = gabi::fadds_ppc(currentWeight,weight);
        pose->mTranslation[1].mValue = translatedY;
        pose->mTranslation[2].mValue = translatedZ;
        u32 flags = result->mFlags;
        pose->mWeight = newWeight;
        pose->mFlags = oldFlags | ~flags;
    }
}
struct EulerQuaternion_l { be<f32> mValues[4]; };
static f32 quaternionRoot(f32 square) {
    if (square==0.0f) return 0.0f;
    f32 estimate=static_cast<f32>(gabi::frsqrte(square));
    f32 half=gabi::fmuls_ppc(0.5f,square);
    f32 product=gabi::fmuls_ppc(half,estimate);
    f32 error=gabi::fnmsubs(product,estimate,1.5f);
    estimate=gabi::fmuls_ppc(estimate,error);
    return gabi::fmuls_ppc(square,estimate);
}
static void eulerQuaternion(f32* output,f32 sx,f32 cx,f32 sy,f32 cy,f32 sz,f32 cz) {
    f32 m21=gabi::fmuls_ppc(sx,cy);
    f32 sxsy=gabi::fmuls_ppc(sx,sy);
    f32 m22=gabi::fmuls_ppc(cx,cy);
    f32 cxcz=gabi::fmuls_ppc(cx,cz);
    f32 m00=gabi::fmuls_ppc(cy,cz);
    f32 m11=gabi::fmadds(sxsy,sz,cxcz);
    f32 cxsz=gabi::fmuls_ppc(cx,sz);
    f32 trace=gabi::fadds_ppc(m00,m11);
    f32 sxcz=gabi::fmuls_ppc(sx,cz);
    trace=gabi::fadds_ppc(trace,m22);
    f32 m12=gabi::fmsubs(cxsz,sy,sxcz);
    f32 m01=gabi::fmsubs(sxsy,cz,cxsz);
    f32 m20=-sy;
    f32 sxsz=gabi::fmuls_ppc(sx,sz);
    f32 m02=gabi::fmadds(cxcz,sy,sxsz);
    f32 m10=gabi::fmuls_ppc(cy,sz);
    u32 largest;
    f32 diagonal;
    if (!(trace<0.0f)) { largest=3; diagonal=trace; }
    else {
        diagonal=gabi::fsubs_ppc(gabi::fadds_ppc(m00,m00),trace);
        if (!(diagonal<0.0f)) largest=0;
        else {
            diagonal=gabi::fsubs_ppc(gabi::fadds_ppc(m11,m11),trace);
            if (!(diagonal<0.0f)) largest=1;
            else { largest=2; diagonal=gabi::fsubs_ppc(gabi::fadds_ppc(m22,m22),trace); }
        }
    }
    f32 square=gabi::fmadds(diagonal,0.25f,0.25f);
    f32 root=quaternionRoot(square);
    f32 divisor=gabi::fmuls_ppc(root,4.0f);
    f32 estimate=static_cast<f32>(gabi::fres(divisor));
    f32 doubled=gabi::fadds_ppc(estimate,estimate);
    f32 product=gabi::fmuls_ppc(divisor,estimate);
    f32 inverse=gabi::fnmsubs(product,estimate,doubled);
    output[largest]=root;
    if(largest==3) {
        output[0]=gabi::fmuls_ppc(gabi::fsubs_ppc(m21,m12),inverse);
        output[1]=gabi::fmuls_ppc(gabi::fsubs_ppc(m02,m20),inverse);
        output[2]=gabi::fmuls_ppc(gabi::fsubs_ppc(m10,m01),inverse);
    } else if(largest==0) {
        output[3]=gabi::fmuls_ppc(gabi::fsubs_ppc(m21,m12),inverse);
        output[1]=gabi::fmuls_ppc(gabi::fadds_ppc(m01,m10),inverse);
        output[2]=gabi::fmuls_ppc(gabi::fadds_ppc(m02,m20),inverse);
    } else if(largest==1) {
        output[3]=gabi::fmuls_ppc(gabi::fsubs_ppc(m02,m20),inverse);
        output[2]=gabi::fmuls_ppc(gabi::fadds_ppc(m12,m21),inverse);
        output[0]=gabi::fmuls_ppc(gabi::fadds_ppc(m10,m01),inverse);
    } else {
        output[3]=gabi::fmuls_ppc(gabi::fsubs_ppc(m10,m01),inverse);
        output[0]=gabi::fmuls_ppc(gabi::fadds_ppc(m20,m02),inverse);
        output[1]=gabi::fmuls_ppc(gabi::fadds_ppc(m21,m12),inverse);
    }
}
/* 027E7A14 */
void PoseBlend_l::accumulateEulerQuaternion(agl_gx2_support::TransformRuntime_l* animation,f32 weight) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    animation->mState = static_cast<u32>(animation->mState) | 8;
    const agl_gx2_support::TransformResult_l* result = animation->mResults;
    mState = static_cast<u32>(mState) | 8;
    u32 count = animation->mBindings.mCount;
    BlendPose_l* data = mData;
    for (u32 target = 0; target < count; ++target,++result) {
        const be<u32>* entries = animation->mBindings.mEntries;
        u32 binding = entries[target];
        if (binding & agl_gx2_support::SkipApplication) continue;
        BlendPose_l* pose = data + (binding & 0x7FFF);
        f32 angles[3];
        u32 codes[3];
        f32 sine[3],cosine[3];
        for (u32 axis=0;axis<3;++axis) {
            angles[axis]=result->mRotation[axis].mValue;
            f32 scaled=gabi::fmuls_ppc(angles[axis],683565248.0f);
            codes[axis]=transformAngleIndex(scaled).r4;
            f32 fraction=gabi::fmuls_ppc(static_cast<f32>(codes[axis]&0xFFFFFF),0x1p-24f);
            sine[axis]=agl_gx2_support::angleSine(codes[axis],fraction);
            cosine[axis]=agl_gx2_support::angleCosine(codes[axis],fraction);
        }
        f32 quaternion[4];
        eulerQuaternion(quaternion,sine[0],cosine[0],sine[1],cosine[1],sine[2],cosine[2]);
        f32 oldWeight = pose->mWeight;
        if (oldWeight == 0.0f) {
            for (u32 component = 0; component < 4; ++component) pose->mOrientation[component].mValue = quaternion[component];
        } else {
            gabi::FrameLocal<EulerQuaternion_l> incoming(0x10);
            for(u32 component=0;component<4;++component) incoming->mValues[component]=quaternion[component];
            f32 totalWeight = gabi::fadds_ppc(oldWeight,weight);
            f32 estimate = static_cast<f32>(gabi::fres(totalWeight));
            f32 doubled = gabi::fadds_ppc(estimate,estimate);
            f32 product = gabi::fmuls_ppc(totalWeight,estimate);
            f32 inverse = gabi::fnmsubs(product,estimate,doubled);
            f32 factor = gabi::fmuls_ppc(weight,inverse);
            f32 incomingX = incoming->mValues[0];
            f32 incomingY = incoming->mValues[1];
            f32 oldX = pose->mOrientation[0].mValue;
            f32 oldY = pose->mOrientation[1].mValue;
            f32 dotX = gabi::fmuls_ppc(oldX,incomingX);
            f32 dotY = gabi::fmuls_ppc(oldY,incomingY);
            f32 oldZ = pose->mOrientation[2].mValue;
            f32 oldW = pose->mOrientation[3].mValue;
            f32 incomingZ = incoming->mValues[2];
            f32 incomingW = incoming->mValues[3];
            dotX = gabi::fmadds(oldZ,incomingZ,dotX);
            dotY = gabi::fmadds(oldW,incomingW,dotY);
            f32 dot = gabi::fadds_ppc(dotX,dotY);
            f32 magnitude = __builtin_fabsf(dot);
            f32 sign = static_cast<f32>(ppc_fsel(dot,-1.0,1.0));
            f32 oldFactor;
            f32 incomingFactor;
            bool linear = magnitude > 0.999989986419677734375f;
            if (linear) {
                incomingFactor = gabi::fmuls_ppc(sign,factor);
                oldFactor = gabi::fsubs_ppc(1.0f,factor);
            } else {
                f64 angle = blendAcos(static_cast<f64>(magnitude));
                f32 scaled = agl_gx2_support::singleMultiplyWide(angle,683565248.0f);
                auto converted = transformAngleIndex(scaled);
                u32 wholeAngle = converted.r4;
                f32 fraction = gabi::fmuls_ppc(static_cast<f32>(wholeAngle & 0xFFFFFF),0x1p-24f);
                f32 sine = agl_gx2_support::angleSine(wholeAngle,fraction);
                f32 part = gabi::fmuls_ppc(factor,static_cast<f32>(wholeAngle));
                f32 sineEstimate = static_cast<f32>(gabi::fres(sine));
                f32 sineDoubled = gabi::fadds_ppc(sineEstimate,sineEstimate);
                f32 sineProduct = gabi::fmuls_ppc(sine,sineEstimate);
                f32 inverseSine = gabi::fnmsubs(sineProduct,sineEstimate,sineDoubled);
                u32 partAngle = blendAngleUnsigned(part);
                u32 remainder = wholeAngle - partAngle;
                f32 remainderFraction = gabi::fmuls_ppc(static_cast<f32>(remainder & 0xFFFFFF),0x1p-24f);
                f32 partFraction = gabi::fmuls_ppc(static_cast<f32>(partAngle & 0xFFFFFF),0x1p-24f);
                f32 remainderSine = agl_gx2_support::angleSine(remainder,remainderFraction);
                f32 partSine = agl_gx2_support::angleSine(partAngle,partFraction);
                oldFactor = gabi::fmuls_ppc(remainderSine,inverseSine);
                f32 unsignedFactor = gabi::fmuls_ppc(partSine,inverseSine);
                incomingFactor = gabi::fmuls_ppc(sign,unsignedFactor);
                oldX = pose->mOrientation[0].mValue;
                oldY = pose->mOrientation[1].mValue;
            }
            f32 weightedX = gabi::fmuls_ppc(incomingX,incomingFactor);
            f32 weightedY = gabi::fmuls_ppc(incomingY,incomingFactor);
            f32 newX = gabi::fmadds(oldX,oldFactor,weightedX);
            f32 newY = gabi::fmadds(oldY,oldFactor,weightedY);
            pose->mOrientation[0].mValue = newX;
            pose->mOrientation[1].mValue = newY;
            incomingZ = incoming->mValues[2];
            incomingW = incoming->mValues[3];
            oldZ = pose->mOrientation[2].mValue;
            oldW = pose->mOrientation[3].mValue;
            f32 weightedZ = gabi::fmuls_ppc(incomingZ,incomingFactor);
            f32 weightedW = gabi::fmuls_ppc(incomingW,incomingFactor);
            pose->mOrientation[2].mValue = gabi::fmadds(oldZ,oldFactor,weightedZ);
            pose->mOrientation[3].mValue = gabi::fmadds(oldW,oldFactor,weightedW);
        }
        f32 scaleX = readTransformScalar(&result->mScale[0]);
        f32 scaleY = readTransformScalar(&result->mScale[1]);
        f32 oldScaleX = pose->mScale[0].mValue;
        f32 oldScaleY = pose->mScale[1].mValue;
        f32 scaledX = gabi::fmadds(scaleX,weight,oldScaleX);
        f32 scaledY = gabi::fmadds(scaleY,weight,oldScaleY);
        f32 oldScaleZ = pose->mScale[2].mValue;
        f32 oldTranslationX = pose->mTranslation[0].mValue;
        pose->mScale[0].mValue = scaledX;
        pose->mScale[1].mValue = scaledY;
        f32 scaleZ = readTransformScalar(&result->mScale[2]);
        f32 translationX = readTransformScalar(&result->mTranslation[0]);
        f32 scaledZ = gabi::fmadds(scaleZ,weight,oldScaleZ);
        f32 translatedX = gabi::fmadds(translationX,weight,oldTranslationX);
        f32 oldTranslationY = pose->mTranslation[1].mValue;
        f32 oldTranslationZ = pose->mTranslation[2].mValue;
        pose->mScale[2].mValue = scaledZ;
        pose->mTranslation[0].mValue = translatedX;
        f32 translationY = readTransformScalar(&result->mTranslation[1]);
        f32 translationZ = readTransformScalar(&result->mTranslation[2]);
        f32 currentWeight = pose->mWeight;
        f32 translatedY = gabi::fmadds(translationY,weight,oldTranslationY);
        f32 translatedZ = gabi::fmadds(translationZ,weight,oldTranslationZ);
        u32 oldFlags = pose->mFlags;
        f32 newWeight = gabi::fadds_ppc(currentWeight,weight);
        pose->mTranslation[1].mValue = translatedY;
        pose->mTranslation[2].mValue = translatedZ;
        u32 flags = result->mFlags;
        pose->mWeight = newWeight;
        pose->mFlags = oldFlags | ~flags;
    }
}
/* 027E89FC */
void PoseBlend_l::applyAxesNormalized(SkeletonRuntime_l* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mState = static_cast<u32>(mState) | 8;
    u32 skeletonFlags = skeleton->mFlags;
    BlendPose_l* pose = mData;
    skeleton->mFlags = skeletonFlags | 4;
    u32 count = mCapacity;
    TransformBone_l* bone = skeleton->mBones;
    for (u32 index = 0; index < count; ++index,++pose,++bone) {
        f32 weight = pose->mWeight;
        if (__builtin_fabsf(weight) < 0.001f) continue;
        f32 difference = gabi::fsubs_ppc(weight,1.0f);
        bool unitWeight = __builtin_fabsf(difference) < 0.001f;
        u32 poseFlags = pose->mFlags;
        if (!unitWeight) {
            f32 estimate = static_cast<f32>(gabi::fres(weight));
            f32 doubled = gabi::fadds_ppc(estimate,estimate);
            f32 error = gabi::fmuls_ppc(weight,estimate);
            f32 inverse = gabi::fnmsubs(error,estimate,doubled);
            f32 values[6];
            for (u32 component = 0; component < 3; ++component) values[component] = pose->mScale[component].mValue;
            for (u32 component = 0; component < 3; ++component) values[component + 3] = pose->mTranslation[component].mValue;
            for (u32 component = 0; component < 3; ++component) pose->mScale[component].mValue = gabi::fmuls_ppc(values[component],inverse);
            for (u32 component = 0; component < 3; ++component) pose->mTranslation[component].mValue = gabi::fmuls_ppc(values[component + 3],inverse);
        }
        constexpr u32 ScaleFlags = 0x0F800000;
        u32 boneFlags = bone->mFlags;
        bone->mFlags = (boneFlags & ~ScaleFlags) | (~poseFlags & ScaleFlags);
        for (u32 component = 0; component < 3; ++component) bone->mScale[component] = pose->mScale[component].mBits;
        bone->mMatrix[3].mBits = pose->mTranslation[0].mBits;
        bone->mMatrix[7].mBits = pose->mTranslation[1].mBits;
        bone->mMatrix[11].mBits = pose->mTranslation[2].mBits;
        f32 firstZ = pose->mOrientation[2].mValue;
        f32 secondY = pose->mOrientation[4].mValue;
        f32 firstY = pose->mOrientation[1].mValue;
        f32 productX = gabi::fmuls_ppc(firstZ,secondY);
        f32 secondZ = pose->mOrientation[5].mValue;
        f32 crossX = gabi::fmsubs(firstY,secondZ,productX);
        f32 firstX = pose->mOrientation[0].mValue;
        f32 secondX = pose->mOrientation[3].mValue;
        f32 productY = gabi::fmuls_ppc(firstX,secondZ);
        f32 crossY = gabi::fmsubs(firstZ,secondX,productY);
        firstY = pose->mOrientation[1].mValue;
        f32 productZ = gabi::fmuls_ppc(firstY,secondX);
        secondY = pose->mOrientation[4].mValue;
        f32 crossZ = gabi::fmsubs(firstX,secondY,productZ);
        secondX = pose->mOrientation[3].mValue;
        secondY = pose->mOrientation[4].mValue;
        f32 crossSquareX = gabi::fmuls_ppc(crossX,crossX);
        f32 crossSquareY = gabi::fmuls_ppc(crossY,crossY);
        f32 secondSquareX = gabi::fmuls_ppc(secondX,secondX);
        f32 secondSquareY = gabi::fmuls_ppc(secondY,secondY);
        secondZ = pose->mOrientation[5].mValue;
        f32 secondLength = gabi::fadds_ppc(secondSquareX,secondSquareY);
        secondLength = gabi::fmadds(secondZ,secondZ,secondLength);
        f32 crossLength = gabi::fadds_ppc(crossSquareX,crossSquareY);
        crossLength = gabi::fmadds(crossZ,crossZ,crossLength);
        if (secondLength != 0.0f && crossLength != 0.0f) {
            f32 secondEstimate = static_cast<f32>(gabi::frsqrte(secondLength));
            f32 crossEstimate = static_cast<f32>(gabi::frsqrte(crossLength));
            f32 secondHalf = gabi::fmuls_ppc(secondLength,0.5f);
            f32 crossHalf = gabi::fmuls_ppc(crossLength,0.5f);
            f32 secondProduct = gabi::fmuls_ppc(secondHalf,secondEstimate);
            f32 crossProduct = gabi::fmuls_ppc(crossHalf,crossEstimate);
            f32 secondError = gabi::fnmsubs(secondProduct,secondEstimate,1.5f);
            f32 crossError = gabi::fnmsubs(crossProduct,crossEstimate,1.5f);
            f32 secondInverse = gabi::fmuls_ppc(secondEstimate,secondError);
            f32 crossInverse = gabi::fmuls_ppc(crossEstimate,crossError);
            secondX = pose->mOrientation[3].mValue;
            secondY = pose->mOrientation[4].mValue;
            f32 normalizedSecondX = gabi::fmuls_ppc(secondX,secondInverse);
            f32 normalizedSecondY = gabi::fmuls_ppc(secondY,secondInverse);
            bone->mMatrix[4].mValue = normalizedSecondX;
            bone->mMatrix[5].mValue = normalizedSecondY;
            secondZ = pose->mOrientation[5].mValue;
            f32 normalizedSecondZ = gabi::fmuls_ppc(secondZ,secondInverse);
            bone->mMatrix[6].mValue = normalizedSecondZ;
            f32 normalizedCrossX = gabi::fmuls_ppc(crossX,crossInverse);
            f32 normalizedCrossY = gabi::fmuls_ppc(crossY,crossInverse);
            bone->mMatrix[8].mValue = normalizedCrossX;
            bone->mMatrix[9].mValue = normalizedCrossY;
            f32 normalizedCrossZ = gabi::fmuls_ppc(crossZ,crossInverse);
            f32 storedCrossY = bone->mMatrix[9].mValue;
            f32 rowProductX = gabi::fmuls_ppc(normalizedSecondZ,storedCrossY);
            f32 storedSecondY = bone->mMatrix[5].mValue;
            f32 rowX = gabi::fmsubs(storedSecondY,normalizedCrossZ,rowProductX);
            bone->mMatrix[10].mValue = normalizedCrossZ;
            f32 storedSecondX = bone->mMatrix[4].mValue;
            f32 storedCrossX = bone->mMatrix[8].mValue;
            f32 rowProductY = gabi::fmuls_ppc(storedSecondX,normalizedCrossZ);
            f32 rowY = gabi::fmsubs(normalizedSecondZ,storedCrossX,rowProductY);
            storedSecondY = bone->mMatrix[5].mValue;
            storedCrossY = bone->mMatrix[9].mValue;
            f32 rowProductZ = gabi::fmuls_ppc(storedSecondY,storedCrossX);
            f32 rowZ = gabi::fmsubs(storedSecondX,storedCrossY,rowProductZ);
            bone->mMatrix[0].mValue = rowX;
            bone->mMatrix[1].mValue = rowY;
            bone->mMatrix[2].mValue = rowZ;
        }
        count = mCapacity;
    }
}
/* 027E8C4C */
void PoseBlend_l::applyAxes(SkeletonRuntime_l* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mState = static_cast<u32>(mState) | 8;
    u32 skeletonFlags = skeleton->mFlags;
    BlendPose_l* pose = mData;
    skeleton->mFlags = skeletonFlags | 4;
    u32 count = mCapacity;
    TransformBone_l* bone = skeleton->mBones;
    for (u32 index = 0; index < count; ++index,++pose,++bone) {
        f32 weight = pose->mWeight;
        if (__builtin_fabsf(weight) < 0.001f) continue;
        u32 poseFlags = pose->mFlags;
        constexpr u32 ScaleFlags = 0x0F800000;
        u32 boneFlags = bone->mFlags;
        bone->mFlags = (boneFlags & ~ScaleFlags) | (~poseFlags & ScaleFlags);
        for (u32 component = 0; component < 3; ++component) bone->mScale[component] = pose->mScale[component].mBits;
        bone->mMatrix[3].mBits = pose->mTranslation[0].mBits;
        bone->mMatrix[7].mBits = pose->mTranslation[1].mBits;
        bone->mMatrix[11].mBits = pose->mTranslation[2].mBits;
        f32 firstZ = pose->mOrientation[2].mValue;
        f32 secondY = pose->mOrientation[4].mValue;
        f32 firstY = pose->mOrientation[1].mValue;
        f32 productX = gabi::fmuls_ppc(firstZ,secondY);
        f32 secondZ = pose->mOrientation[5].mValue;
        f32 crossX = gabi::fmsubs(firstY,secondZ,productX);
        f32 firstX = pose->mOrientation[0].mValue;
        f32 secondX = pose->mOrientation[3].mValue;
        f32 productY = gabi::fmuls_ppc(firstX,secondZ);
        f32 crossY = gabi::fmsubs(firstZ,secondX,productY);
        firstY = pose->mOrientation[1].mValue;
        f32 productZ = gabi::fmuls_ppc(firstY,secondX);
        secondY = pose->mOrientation[4].mValue;
        f32 crossZ = gabi::fmsubs(firstX,secondY,productZ);
        secondX = pose->mOrientation[3].mValue;
        secondY = pose->mOrientation[4].mValue;
        f32 crossSquareX = gabi::fmuls_ppc(crossX,crossX);
        f32 crossSquareY = gabi::fmuls_ppc(crossY,crossY);
        f32 secondSquareX = gabi::fmuls_ppc(secondX,secondX);
        f32 secondSquareY = gabi::fmuls_ppc(secondY,secondY);
        secondZ = pose->mOrientation[5].mValue;
        f32 secondLength = gabi::fadds_ppc(secondSquareX,secondSquareY);
        secondLength = gabi::fmadds(secondZ,secondZ,secondLength);
        f32 crossLength = gabi::fadds_ppc(crossSquareX,crossSquareY);
        crossLength = gabi::fmadds(crossZ,crossZ,crossLength);
        if (secondLength != 0.0f && crossLength != 0.0f) {
            f32 secondEstimate = static_cast<f32>(gabi::frsqrte(secondLength));
            f32 crossEstimate = static_cast<f32>(gabi::frsqrte(crossLength));
            f32 secondHalf = gabi::fmuls_ppc(secondLength,0.5f);
            f32 crossHalf = gabi::fmuls_ppc(crossLength,0.5f);
            f32 secondProduct = gabi::fmuls_ppc(secondHalf,secondEstimate);
            f32 crossProduct = gabi::fmuls_ppc(crossHalf,crossEstimate);
            f32 secondError = gabi::fnmsubs(secondProduct,secondEstimate,1.5f);
            f32 crossError = gabi::fnmsubs(crossProduct,crossEstimate,1.5f);
            f32 secondInverse = gabi::fmuls_ppc(secondEstimate,secondError);
            f32 crossInverse = gabi::fmuls_ppc(crossEstimate,crossError);
            secondX = pose->mOrientation[3].mValue;
            secondY = pose->mOrientation[4].mValue;
            f32 normalizedSecondX = gabi::fmuls_ppc(secondX,secondInverse);
            f32 normalizedSecondY = gabi::fmuls_ppc(secondY,secondInverse);
            bone->mMatrix[4].mValue = normalizedSecondX;
            bone->mMatrix[5].mValue = normalizedSecondY;
            secondZ = pose->mOrientation[5].mValue;
            f32 normalizedSecondZ = gabi::fmuls_ppc(secondZ,secondInverse);
            bone->mMatrix[6].mValue = normalizedSecondZ;
            f32 normalizedCrossX = gabi::fmuls_ppc(crossX,crossInverse);
            f32 normalizedCrossY = gabi::fmuls_ppc(crossY,crossInverse);
            bone->mMatrix[8].mValue = normalizedCrossX;
            bone->mMatrix[9].mValue = normalizedCrossY;
            f32 normalizedCrossZ = gabi::fmuls_ppc(crossZ,crossInverse);
            f32 storedCrossY = bone->mMatrix[9].mValue;
            f32 rowProductX = gabi::fmuls_ppc(normalizedSecondZ,storedCrossY);
            f32 storedSecondY = bone->mMatrix[5].mValue;
            f32 rowX = gabi::fmsubs(storedSecondY,normalizedCrossZ,rowProductX);
            bone->mMatrix[10].mValue = normalizedCrossZ;
            f32 storedSecondX = bone->mMatrix[4].mValue;
            f32 storedCrossX = bone->mMatrix[8].mValue;
            f32 rowProductY = gabi::fmuls_ppc(storedSecondX,normalizedCrossZ);
            f32 rowY = gabi::fmsubs(normalizedSecondZ,storedCrossX,rowProductY);
            storedSecondY = bone->mMatrix[5].mValue;
            storedCrossY = bone->mMatrix[9].mValue;
            f32 rowProductZ = gabi::fmuls_ppc(storedSecondY,storedCrossX);
            f32 rowZ = gabi::fmsubs(storedSecondX,storedCrossY,rowProductZ);
            bone->mMatrix[0].mValue = rowX;
            bone->mMatrix[1].mValue = rowY;
            bone->mMatrix[2].mValue = rowZ;
        }
        count = mCapacity;
    }
}
/* 027E8E54 */
void PoseBlend_l::applyQuaternionNormalized(SkeletonRuntime_l* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mState = static_cast<u32>(mState) | 8;
    u32 skeletonFlags = skeleton->mFlags;
    BlendPose_l* pose = mData;
    skeleton->mFlags = skeletonFlags | 4;
    u32 count = mCapacity;
    TransformBone_l* bone = skeleton->mBones;
    for (u32 index = 0; index < count; ++index,++pose,++bone) {
        f32 weight = pose->mWeight;
        if (__builtin_fabsf(weight) < 0.001f) continue;
        f32 difference = gabi::fsubs_ppc(weight,1.0f);
        bool unitWeight = __builtin_fabsf(difference) < 0.001f;
        u32 poseFlags = pose->mFlags;
        if (!unitWeight) {
            f32 estimate = static_cast<f32>(gabi::fres(weight));
            f32 doubled = gabi::fadds_ppc(estimate,estimate);
            f32 error = gabi::fmuls_ppc(weight,estimate);
            f32 inverse = gabi::fnmsubs(error,estimate,doubled);
            f32 values[6];
            for (u32 component = 0; component < 3; ++component) values[component] = pose->mScale[component].mValue;
            for (u32 component = 0; component < 3; ++component) values[component + 3] = pose->mTranslation[component].mValue;
            for (u32 component = 0; component < 3; ++component) pose->mScale[component].mValue = gabi::fmuls_ppc(values[component],inverse);
            for (u32 component = 0; component < 3; ++component) pose->mTranslation[component].mValue = gabi::fmuls_ppc(values[component + 3],inverse);
        }
        constexpr u32 ScaleFlags = 0x0F800000;
        u32 boneFlags = bone->mFlags;
        bone->mFlags = (boneFlags & ~ScaleFlags) | (~poseFlags & ScaleFlags);
        for (u32 component = 0; component < 3; ++component) bone->mScale[component] = pose->mScale[component].mBits;
        bone->mMatrix[3].mBits = pose->mTranslation[0].mBits;
        bone->mMatrix[7].mBits = pose->mTranslation[1].mBits;
        bone->mMatrix[11].mBits = pose->mTranslation[2].mBits;
        f32 x = pose->mOrientation[0].mValue;
        f32 y = pose->mOrientation[1].mValue;
        f32 w = pose->mOrientation[3].mValue;
        f32 twiceX = gabi::fadds_ppc(x,x);
        f32 twiceY = gabi::fadds_ppc(y,y);
        f32 twiceW = gabi::fadds_ppc(w,w);
        f32 yy = gabi::fmuls_ppc(twiceY,y);
        f32 z = pose->mOrientation[2].mValue;
        f32 twiceZ = gabi::fadds_ppc(z,z);
        f32 withoutY = gabi::fsubs_ppc(1.0f,yy);
        f32 xy = gabi::fmuls_ppc(twiceX,y);
        f32 zz = gabi::fmuls_ppc(twiceZ,z);
        f32 wy = gabi::fmuls_ppc(twiceW,y);
        f32 diagonalX = gabi::fsubs_ppc(withoutY,zz);
        f32 xz = gabi::fmuls_ppc(twiceX,z);
        f32 wz = gabi::fmuls_ppc(twiceW,z);
        f32 rowXz = gabi::fadds_ppc(xz,wy);
        f32 rowYx = gabi::fadds_ppc(xy,wz);
        f32 wx = gabi::fmuls_ppc(twiceW,x);
        f32 rowXy = gabi::fsubs_ppc(xy,wz);
        f32 withoutX = gabi::fnmsubs(twiceX,x,1.0f);
        f32 yz = gabi::fmuls_ppc(twiceY,z);
        f32 rowZx = gabi::fsubs_ppc(xz,wy);
        f32 diagonalY = gabi::fsubs_ppc(withoutX,zz);
        f32 rowYz = gabi::fsubs_ppc(yz,wx);
        f32 rowZy = gabi::fadds_ppc(yz,wx);
        f32 diagonalZ = gabi::fsubs_ppc(withoutX,yy);
        bone->mMatrix[4].mValue = rowYx;
        bone->mMatrix[1].mValue = rowXy;
        bone->mMatrix[0].mValue = diagonalX;
        bone->mMatrix[6].mValue = rowYz;
        bone->mMatrix[5].mValue = diagonalY;
        bone->mMatrix[2].mValue = rowXz;
        bone->mMatrix[8].mValue = rowZx;
        bone->mMatrix[9].mValue = rowZy;
        bone->mMatrix[10].mValue = diagonalZ;
        count = mCapacity;
    }
}
/* 027E9020 */
void PoseBlend_l::applyQuaternion(SkeletonRuntime_l* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mState = static_cast<u32>(mState) | 8;
    u32 skeletonFlags = skeleton->mFlags;
    BlendPose_l* pose = mData;
    skeleton->mFlags = skeletonFlags | 4;
    u32 count = mCapacity;
    TransformBone_l* bone = skeleton->mBones;
    for (u32 index = 0; index < count; ++index,++pose,++bone) {
        f32 weight = pose->mWeight;
        if (__builtin_fabsf(weight) < 0.001f) continue;
        u32 poseFlags = pose->mFlags;
        constexpr u32 ScaleFlags = 0x0F800000;
        u32 boneFlags = bone->mFlags;
        bone->mFlags = (boneFlags & ~ScaleFlags) | (~poseFlags & ScaleFlags);
        for (u32 component = 0; component < 3; ++component) bone->mScale[component] = pose->mScale[component].mBits;
        bone->mMatrix[3].mBits = pose->mTranslation[0].mBits;
        bone->mMatrix[7].mBits = pose->mTranslation[1].mBits;
        bone->mMatrix[11].mBits = pose->mTranslation[2].mBits;
        f32 x = pose->mOrientation[0].mValue;
        f32 y = pose->mOrientation[1].mValue;
        f32 w = pose->mOrientation[3].mValue;
        f32 twiceX = gabi::fadds_ppc(x,x);
        f32 twiceY = gabi::fadds_ppc(y,y);
        f32 twiceW = gabi::fadds_ppc(w,w);
        f32 yy = gabi::fmuls_ppc(twiceY,y);
        f32 z = pose->mOrientation[2].mValue;
        f32 twiceZ = gabi::fadds_ppc(z,z);
        f32 withoutY = gabi::fsubs_ppc(1.0f,yy);
        f32 xy = gabi::fmuls_ppc(twiceX,y);
        f32 zz = gabi::fmuls_ppc(twiceZ,z);
        f32 wy = gabi::fmuls_ppc(twiceW,y);
        f32 diagonalX = gabi::fsubs_ppc(withoutY,zz);
        f32 xz = gabi::fmuls_ppc(twiceX,z);
        f32 wz = gabi::fmuls_ppc(twiceW,z);
        f32 rowXz = gabi::fadds_ppc(xz,wy);
        f32 rowYx = gabi::fadds_ppc(xy,wz);
        f32 wx = gabi::fmuls_ppc(twiceW,x);
        f32 rowXy = gabi::fsubs_ppc(xy,wz);
        f32 withoutX = gabi::fnmsubs(twiceX,x,1.0f);
        f32 yz = gabi::fmuls_ppc(twiceY,z);
        f32 rowZx = gabi::fsubs_ppc(xz,wy);
        f32 diagonalY = gabi::fsubs_ppc(withoutX,zz);
        f32 rowYz = gabi::fsubs_ppc(yz,wx);
        f32 rowZy = gabi::fadds_ppc(yz,wx);
        f32 diagonalZ = gabi::fsubs_ppc(withoutX,yy);
        bone->mMatrix[4].mValue = rowYx;
        bone->mMatrix[0].mValue = diagonalX;
        bone->mMatrix[2].mValue = rowXz;
        bone->mMatrix[1].mValue = rowXy;
        bone->mMatrix[8].mValue = rowZx;
        bone->mMatrix[5].mValue = diagonalY;
        bone->mMatrix[6].mValue = rowYz;
        bone->mMatrix[9].mValue = rowZy;
        bone->mMatrix[10].mValue = diagonalZ;
        count = mCapacity;
    }
}
}
/* WWHD glue
ext 028F4FAC f64 blendAcos(f64)
*/
