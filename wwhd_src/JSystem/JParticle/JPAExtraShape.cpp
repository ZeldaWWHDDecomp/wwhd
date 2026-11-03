/** JPA1 extra-shape archive getters and interpolation rates. */
#include "bindings.h"
#include "JSystem/JParticle/JPAExtraShape.h"
static u32 sVtable;
static JPAExtraShapeStaticBlock sStaticBlock;
static u32 sStaticRegister;
/* 02847F18 */
static JPAExtraShapeArc* JPAExtraShapeArc__ct(JPAExtraShapeArc* object,const u8* data) {
 if (!object) object=(JPAExtraShapeArc*)operator_new(sizeof(JPAExtraShapeArc));
 if (object) {
  object->mVtable=&sVtable;
  object->mpData=(const JPAExtraShapeData*)(data+0xC);
  f32 in=object->mpData->mAlphaInTiming;
  object->mAlphaIncreaseRate=in!=0.0f ? ((f32)object->mpData->mAlphaBaseValue-(f32)object->mpData->mAlphaInValue)/in : 1.0f;
  f32 out=object->mpData->mAlphaOutTiming;
  object->mAlphaDecreaseRate=out!=1.0f ? ((f32)object->mpData->mAlphaOutValue-(f32)object->mpData->mAlphaBaseValue)/(1.0f-out) : 1.0f;
  in=object->mpData->mScaleInTiming;
  if(in!=0.0f) {
   object->mIncreaseRateX=(1.0f-(f32)object->mpData->mScaleInValueX)/in;
   object->mIncreaseRateY=(1.0f-(f32)object->mpData->mScaleInValueY)/(f32)object->mpData->mScaleInTiming;
  } else {object->mIncreaseRateY=1.0f;object->mIncreaseRateX=1.0f;}
  out=object->mpData->mScaleOutTiming;
  if(out!=1.0f) {
   object->mDecreaseRateX=((f32)object->mpData->mScaleOutValueX-1.0f)/(1.0f-out);
   object->mDecreaseRateY=((f32)object->mpData->mScaleOutValueY-1.0f)/(1.0f-(f32)object->mpData->mScaleOutTiming);
  } else {object->mDecreaseRateX=1.0f;object->mDecreaseRateY=1.0f;}
 }
 return object;
}
/* 02848090 */
static void __sinit_JPAExtraShape_cpp() {
 sStaticBlock.words[2]=0;sStaticBlock.words[0]=0;sStaticBlock.words[3]=0;sStaticBlock.words[1]=0;
 JPAExtraShapeRegister(&sStaticRegister);
}
/* 028480B8 */
u32 JPAExtraShapeArc::isEnableScale() { return ((u32)mpData->mFlags & 0x100); }

/* 028480C8 */
u32 JPAExtraShapeArc::isDiffXY() { return ((u32)mpData->mFlags & 0x200); }

/* 028480D8 */
u32 JPAExtraShapeArc::isEnableScaleAnmX() { return ((u32)mpData->mFlags & 0x800); }

/* 028480E8 */
u32 JPAExtraShapeArc::isEnableScaleAnmY() { return ((u32)mpData->mFlags & 0x400); }

/* 028480F8 */
u32 JPAExtraShapeArc::isEnableScaleBySpeedX() { return ((u32)mpData->mFlags & 0x2000); }

/* 02848108 */
u32 JPAExtraShapeArc::isEnableScaleBySpeedY() { return ((u32)mpData->mFlags & 0x1000); }

/* 02848118 */
f32 JPAExtraShapeArc::getScaleInTiming() { return mpData->mScaleInTiming; }

/* 02848124 */
f32 JPAExtraShapeArc::getScaleOutTiming() { return mpData->mScaleOutTiming; }

/* 02848130 */
f32 JPAExtraShapeArc::getScaleInValueX() { return mpData->mScaleInValueX; }

/* 0284813C */
f32 JPAExtraShapeArc::getScaleInValueY() { return mpData->mScaleInValueY; }

/* 02848148 */
u8 JPAExtraShapeArc::getAnmTypeX() { return ((u32)mpData->mFlags >> 18) & 0x01; }

/* 02848158 */
u8 JPAExtraShapeArc::getAnmTypeY() { return ((u32)mpData->mFlags >> 19) & 0x01; }

/* 02848168 */
s32 JPAExtraShapeArc::getAnmCycleX() { return mpData->mAnmCycleX; }

/* 02848174 */
s32 JPAExtraShapeArc::getAnmCycleY() { return mpData->mAnmCycleY; }

/* 02848180 */
f32 JPAExtraShapeArc::getIncreaseRateX() { return mIncreaseRateX; }

/* 02848188 */
f32 JPAExtraShapeArc::getIncreaseRateY() { return mIncreaseRateY; }

/* 02848190 */
f32 JPAExtraShapeArc::getDecreaseRateX() { return mDecreaseRateX; }

/* 02848198 */
f32 JPAExtraShapeArc::getDecreaseRateY() { return mDecreaseRateY; }

/* 028481A0 */
u8 JPAExtraShapeArc::getPivotX() { return ((u32)mpData->mFlags >> 14) & 0x03; }

/* 028481B0 */
u8 JPAExtraShapeArc::getPivotY() { return ((u32)mpData->mFlags >> 16) & 0x03; }

/* 028481C0 */
f32 JPAExtraShapeArc::getRandomScale() { return mpData->mRandomScale; }

/* 028481CC */
u32 JPAExtraShapeArc::isEnableAlpha() { return ((u32)mpData->mFlags & 0x01); }

/* 028481DC */
u32 JPAExtraShapeArc::isEnableSinWave() { return ((u32)mpData->mFlags & 0x02); }

/* 028481EC */
f32 JPAExtraShapeArc::getAlphaInTiming() { return mpData->mAlphaInTiming; }

/* 028481F8 */
f32 JPAExtraShapeArc::getAlphaOutTiming() { return mpData->mAlphaOutTiming; }

/* 02848204 */
f32 JPAExtraShapeArc::getAlphaInValue() { return mpData->mAlphaInValue; }

/* 02848210 */
f32 JPAExtraShapeArc::getAlphaBaseValue() { return mpData->mAlphaBaseValue; }

/* 0284821C */
u8 JPAExtraShapeArc::getAlphaWaveType() { return ((u32)mpData->mFlags >> 2) & 0x03; }

/* 0284822C */
f32 JPAExtraShapeArc::getAlphaWaveParam1() { return mpData->mAlphaWaveParam1; }

/* 02848238 */
f32 JPAExtraShapeArc::getAlphaWaveParam2() { return mpData->mAlphaWaveParam2; }

/* 02848244 */
f32 JPAExtraShapeArc::getAlphaWaveParam3() { return mpData->mAlphaWaveParam3; }

/* 02848250 */
f32 JPAExtraShapeArc::getAlphaWaveRandom() { return mpData->mAlphaWaveRandom; }

/* 0284825C */
f32 JPAExtraShapeArc::getAlphaIncreaseRate() { return mAlphaIncreaseRate; }

/* 02848264 */
f32 JPAExtraShapeArc::getAlphaDecreaseRate() { return mAlphaDecreaseRate; }

/* 0284826C */
u32 JPAExtraShapeArc::isEnableRotate() { return ((u32)mpData->mFlags & 0x01000000); }

/* 0284827C */
f32 JPAExtraShapeArc::getRotateAngle() { return mpData->mRotateAngle; }

/* 02848288 */
f32 JPAExtraShapeArc::getRotateSpeed() { return mpData->mRotateSpeed; }

/* 02848294 */
f32 JPAExtraShapeArc::getRotateRandomAngle() { return mpData->mRotateRandomAngle; }

/* 028482A0 */
f32 JPAExtraShapeArc::getRotateRandomSpeed() { return mpData->mRotateRandomSpeed; }

/* 028482AC */
f32 JPAExtraShapeArc::getRotateDirection() { return mpData->mRotateDirection; }
/* WWHD glue
 data sVtable 1017AD88
 data sStaticBlock 104B66A4
 data sStaticRegister 101FC738
 ext 028F026C void JPAExtraShapeRegister(void*)
*/
