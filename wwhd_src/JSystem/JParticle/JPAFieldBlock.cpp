/** GameCube JPA1 field-block accessors; preserves HD load/store order. */
#include "bindings.h"
#include "JSystem/JParticle/JPAFieldBlock.h"

/* 02848334 */
u8 JPAFieldBlockArc::getType() { return (u32)mpData->mFlags & 0xf; }

/* 02848344 */
u32 JPAFieldBlockArc::getVelType() { return ((u32)mpData->mFlags >> 8) & 3; }

/* 02848354 */
u32 JPAFieldBlockArc::getSttFlag() { return (u32)mpData->mFlags >> 16; }

/* 02848364 */
u32 JPAFieldBlockArc::getCycle() { return mpData->mCycle; }

/* 02848370 */
u32 JPAFieldBlockArc::getID() { return 0; }

/* 02848378 */
void JPAFieldBlockArc::getPos(cXyz* out) { f32 x = mpData->mPos.x; f32 z = mpData->mPos.z; f32 y = mpData->mPos.y;
    out->z = z; out->x = x; out->y = y; }

/* 02848398 */
void JPAFieldBlockArc::getDir(cXyz* out) { f32 x = mpData->mDir.x; f32 z = mpData->mDir.z; f32 y = mpData->mDir.y;
    out->z = z; out->x = x; out->y = y; }

/* 028483B8 */
f32 JPAFieldBlockArc::getMag() { return mpData->mMag; }

/* 028483C4 */
f32 JPAFieldBlockArc::getMagRndm() { return mpData->mMagRndm; }

/* 028483D0 */
f32 JPAFieldBlockArc::getMaxDist() { return mpData->mMaxDist; }

/* 028483DC */
f32 JPAFieldBlockArc::getVal1() { return mpData->mVal1; }

/* 028483E8 */
f32 JPAFieldBlockArc::getVal2() { return mpData->mVal2; }

/* 028483F4 */
f32 JPAFieldBlockArc::getVal3() { return mpData->mVal3; }

/* 02848400 */
f32 JPAFieldBlockArc::getFadeIn() { return mpData->mFadeIn; }

/* 0284840C */
f32 JPAFieldBlockArc::getFadeOut() { return mpData->mFadeOut; }

/* 02848418 */
f32 JPAFieldBlockArc::getEnTime() { return mpData->mEnTime; }

/* 02848424 */
f32 JPAFieldBlockArc::getDisTime() { return mpData->mDisTime; }

/* WWHD glue
*/
