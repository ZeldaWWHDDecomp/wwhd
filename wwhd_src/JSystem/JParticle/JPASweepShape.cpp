/** JPA1 sweep shape, with HD cached gamma-converted colors. */
#include "bindings.h"
#include "JSystem/JParticle/JPASweepShape.h"
static u32 sVtable;
static JPASweepShapeStaticBlock sStaticBlock;
static u32 sStaticRegister;
/* 028484E8 */
f64 JPASweepColorComponent(u32 component) {
    f32 normalized = (f32)component / 255.0f;
    f64 result = JPASweepPow(normalized,2.200000047683716);
    if (result < 0.0) result = 0.0;
    else if (result > 1.0) result = 1.0;
    return result;
}
/* 02848568 */
void JPASweepConvertColor(GXColor* color) {
    color->r = (u8)gabi::ftoi((f32)(JPASweepColorComponent(color->r) * 255.0));
    color->g = (u8)gabi::ftoi((f32)(JPASweepColorComponent(color->g) * 255.0));
    color->b = (u8)gabi::ftoi((f32)(JPASweepColorComponent(color->b) * 255.0));
}
/* 02848610 */
static JPASweepShapeArc* JPASweepShapeArc__ct(JPASweepShapeArc* object,const u8* data) {
    if (object == NULL) object = (JPASweepShapeArc*)operator_new(sizeof(JPASweepShapeArc));
    if (object != NULL) {
        object->mpData = (const JPASweepShapeData*)(data + 0xc);
        object->mVtable = &sVtable;
        object->mPrm = object->mpData->mPrm;
        object->mEnv = object->mpData->mEnv;
        JPASweepConvertColor(&object->mPrm);
        JPASweepConvertColor(&object->mEnv);
    }
    return object;
}
/* 028486A0 */
static void __sinit_JPASweepShape_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JPASweepShapeConstructRegister(&sStaticRegister);
}

/* 028486C8 */
u8 JPASweepShapeArc::getType() { return (u32)mpData->mFlags & 15; }

/* 028486D8 */
u8 JPASweepShapeArc::getDirType() { return ((u32)mpData->mFlags >> 4) & 7; }

/* 028486E8 */
u8 JPASweepShapeArc::getRotType() { return ((u32)mpData->mFlags >> 7) & 7; }

/* 028486F8 */
u8 JPASweepShapeArc::getBasePlaneType() { return ((u32)mpData->mFlags >> 10) & 1; }

/* 02848708 */
s16 JPASweepShapeArc::getLife() { return mpData->mLife; }

/* 02848714 */
s16 JPASweepShapeArc::getRate() { return mpData->mRate; }

/* 02848720 */
f32 JPASweepShapeArc::getTiming() { return mpData->mTiming; }

/* 0284872C */
u32 JPASweepShapeArc::getStep() { return mpData->mStep; }

/* 02848738 */
f32 JPASweepShapeArc::getPosRndm() { return mpData->mPosRndm; }

/* 02848744 */
f32 JPASweepShapeArc::getVelInfRate() { return mpData->mVelInfRate; }

/* 02848750 */
f32 JPASweepShapeArc::getBaseVel() { return mpData->mBaseVel; }

/* 0284875C */
f32 JPASweepShapeArc::getBaseVelRndm() { return mpData->mBaseVelRndm; }

/* 02848768 */
f32 JPASweepShapeArc::getGravity() { return mpData->mGravity; }

/* 02848774 */
u32 JPASweepShapeArc::isEnableField() { return (u32)mpData->mFlags & 0x200000; }

/* 02848784 */
u32 JPASweepShapeArc::isEnableDrawParent() { return (u32)mpData->mFlags & 0x80000; }

/* 02848794 */
u32 JPASweepShapeArc::isEnableScaleOut() { return (u32)mpData->mFlags & 0x400000; }

/* 028487A4 */
u32 JPASweepShapeArc::isEnableAlphaOut() { return (u32)mpData->mFlags & 0x800000; }

/* 028487B4 */
u32 JPASweepShapeArc::isEnableRotate() { return (u32)mpData->mFlags & 0x1000000; }

/* 028487C4 */
u32 JPASweepShapeArc::isInheritedScale() { return (u32)mpData->mFlags & 0x10000; }

/* 028487D4 */
u32 JPASweepShapeArc::isInheritedAlpha() { return (u32)mpData->mFlags & 0x20000; }

/* 028487E4 */
u32 JPASweepShapeArc::isInheritedRGB() { return (u32)mpData->mFlags & 0x40000; }

/* 028487F4 */
u8 JPASweepShapeArc::getTextureIndex() { return mpData->mTextureIndex; }

/* 02848800 */
f32 JPASweepShapeArc::getScaleX() { return mpData->mScaleX; }

/* 0284880C */
f32 JPASweepShapeArc::getScaleY() { return mpData->mScaleY; }

/* 02848818 */
u32 JPASweepShapeArc::getPrm() { return ((u32)mPrm.r << 24) | ((u32)mPrm.g << 16) | ((u32)mPrm.b << 8) | (u8)mPrm.a; }

/* 02848838 */
u32 JPASweepShapeArc::getEnv() { return ((u32)mEnv.r << 24) | ((u32)mEnv.g << 16) | ((u32)mEnv.b << 8) | (u8)mEnv.a; }

/* 02848858 */
u8 JPASweepShapeArc::getPrmAlpha() { return mpData->mPrm.a; }

/* 02848864 */
u8 JPASweepShapeArc::getEnvAlpha() { return mpData->mEnv.a; }

/* 02848870 */
f32 JPASweepShapeArc::getRotateSpeed() { return mpData->mRotateSpeed; }

/* 0284887C */
f32 JPASweepShapeArc::getInheritScale() { return mpData->mInheritScale; }

/* 02848888 */
f32 JPASweepShapeArc::getInheritAlpha() { return mpData->mInheritAlpha; }

/* 02848894 */
f32 JPASweepShapeArc::getInheritRGB() { return mpData->mInheritRGB; }

/* WWHD glue
 data sVtable 1017AFC0
 data sStaticBlock 104B66D4
 data sStaticRegister 101FC75C
 ext 028F026C void JPASweepShapeConstructRegister(void*)
 ext 028F4560 f64 JPASweepPow(f64,f64)
*/
