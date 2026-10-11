/**
 * JPA_calcvis_b.cpp - Alpha, flicker and texture animation visitors.
 * WWHD verified against the original, with tww names.
 */
#include "JSystem/JParticle/JPA_calcvis_b.h"
namespace jpa_calcvis_b {
static const f32 c_zero  =  0.0f;
static const f32 c_one  =  1.0f;
static const f32 c_half  =  0.5f;
static const f32 c_two  =  2.0f;
// HD: read-only sine/cosine ROM table; the verifier supplies its original bytes.
static SinCosEntry sincosTable[8192]  =  {};

static f32 sine(f32 angle) {
    u32 index  =  (u16)ftoi(angle);
    return sincosTable[index >> 3].sin;
}
static f32 phase(JPABaseParticle* ptcl) {
    // HD: conversion and the shift wrap at 32 bits before converting back to float.
    s32 shifted  =  (s32)((u32)ftoi(ptcl->mCurFrame) << 14);
    return (f32)shifted * ptcl->mDrawParams.mAlphaWaveRandom;
}
static void alpha(JPABaseParticle* ptcl, f32 factor) {
    f32 value  =  ptcl->mDrawParams.mAlphaOut * factor;
    f32 zero  =  c_zero;
    ptcl->mDrawParams.mAlphaOut  =  value;
    // HD: unordered values skip the clamp.
    if (value < zero) ptcl->mDrawParams.mAlphaOut  =  zero;
}
static s32 divide(s32 a, s32 b) {
    if (!b || (a == INT32_MIN && b == -1)) return 0;
    return a / b;
}
static void texture(const JPADrawContext* pDC, u32 index) {
    be<u16>* table  =  pDC->pTexIdx;
    JPADraw* draw  =  pDC->mpDraw;
    draw->mTexIdx  =  (u16)table[index];
}
static f32 getAlphaWaveParam1(const JPADrawContext* pDC) {
    JPAExtraShape* object  =  pDC->pesp;
    u32 target  =  object->__vtbl->getAlphaWaveParam1;
    return jpaCall<f32>(target, object);
}
static f32 getAlphaWaveParam2(const JPADrawContext* pDC) {
    JPAExtraShape* object  =  pDC->pesp;
    u32 target  =  object->__vtbl->getAlphaWaveParam2;
    return jpaCall<f32>(target, object);
}
static f32 getAlphaWaveParam3(const JPADrawContext* pDC) {
    JPAExtraShape* object  =  pDC->pesp;
    u32 target  =  object->__vtbl->getAlphaWaveParam3;
    return jpaCall<f32>(target, object);
}

// HD: cached vtable and target reads are kept separate from subsequent object reloads.
/* 028371F8 */
void JPADrawCalcAlpha::calc(const JPADrawContext* pDC, JPABaseParticle* ptcl) {
    JPAExtraShape* object = pDC->pesp;
    u32 target = object->__vtbl->getAlphaInTiming;
    f32 time = ptcl->mCurNormTime;
    f32 inTime = jpaCall<f32>(target, object);
    object = pDC->pesp;
    ExtraShapeVtable* vt = object->__vtbl;
    if (time<inTime) {
        f32 rate = jpaCall<f32>(vt->getAlphaIncreaseRate, object);
        target = vt->getAlphaInValue;
        object = pDC->pesp;
        f32 scaled = rate*time;
        f32 value = jpaCall<f32>(target, object);
        ptcl->mDrawParams.mAlphaOut  =  scaled+value;
    } else {
        f32 outTime = jpaCall<f32>(vt->getAlphaOutTiming, object);
        object = pDC->pesp;
        vt = object->__vtbl;
        if (time>outTime) {
            f32 rate = jpaCall<f32>(vt->getAlphaDecreaseRate, object);
            target = vt->getAlphaOutTiming;
            object = pDC->pesp;
            f32 end = jpaCall<f32>(target, object);
            target = vt->getAlphaBaseValue;
            object = pDC->pesp;
            f32 delta = time-end;
            f32 base = jpaCall<f32>(target, object);
            ptcl->mDrawParams.mAlphaOut  =  fmadds(rate, delta, base);
        } else ptcl->mDrawParams.mAlphaOut  =  jpaCall<f32>(vt->getAlphaBaseValue, object);
    }
}
/* 02837320 */
void JPADrawCalcAlphaFlickNrmSin::calc(const JPADrawContext* pDC, JPABaseParticle* ptcl) {
    f32 theta = phase(ptcl), one = c_one;
    f32 param1 = getAlphaWaveParam1(pDC);
    f32 wave = sine(theta*(one-param1));
    f32 param3 = getAlphaWaveParam3(pDC);
    f32 half = c_half;
    f32 amplitude = ((wave-one)*half)*param3;
    alpha(ptcl, fmadds(amplitude, ptcl->mDrawParams.mAlphaWaveRandom, one));
}
/* 02837468 */
void JPADrawCalcAlphaFlickAddSin::calc(const JPADrawContext* pDC, JPABaseParticle* ptcl) {
    f32 theta = phase(ptcl), one = c_one;
    f32 param1 = getAlphaWaveParam1(pDC), wave1 = sine(theta*(one-param1));
    f32 param2 = getAlphaWaveParam2(pDC), wave2 = sine(theta*(one-param2));
    f32 param3 = getAlphaWaveParam3(pDC), two = c_two, half = c_half;
    f32 amplitude = (((wave1+wave2)-two)*half)*param3;
    alpha(ptcl, fmadds(amplitude, ptcl->mDrawParams.mAlphaWaveRandom, two)*half);
}
/* 02837614 */
void JPADrawCalcAlphaFlickMultSin::calc(const JPADrawContext* pDC, JPABaseParticle* ptcl) {
    f32 theta = phase(ptcl), param3 = getAlphaWaveParam3(pDC);
    f32 half = c_half;
    f32 amplitude = (half*param3)*ptcl->mDrawParams.mAlphaWaveRandom, one = c_one;
    f32 param1 = getAlphaWaveParam1(pDC), wave1 = sine(theta*(one-param1));
    f32 param2 = getAlphaWaveParam2(pDC), wave2 = sine(theta*(one-param2));
    alpha(ptcl, fmadds(wave1-one, amplitude, one)*fmadds(wave2-one, amplitude, one));
}
/* 028377C8 */
void JPADrawCalcTextureAnmIndexNormal::calc(const JPADrawContext* pDC) {
    JPABaseShape* object = pDC->pbsp;
    JPABaseEmitter* emitter = pDC->pbe;
    BaseShapeVtable* vt = object->__vtbl;
    s32 tick = ftoi(emitter->mCurrentFrame);
    s32 count = jpaCall<s32>(vt->getTextureAnmKeyNum, object);
    if ((s32)((u32)count-1)<tick) {
        object = pDC->pbsp;
        count = jpaCall<s32>(object->__vtbl->getTextureAnmKeyNum, object);
        tick = (s32)((u32)count-1);
    }
    u32 target = vt->getTextureIndexFrame;
    object = pDC->pbsp;
    texture(pDC, jpaCall<u32>(target, object, (u8)tick));
}
/* 0283787C */
void JPADrawCalcTextureAnmIndexRepeat::calc(const JPADrawContext* pDC) {
    JPABaseEmitter* emitter = pDC->pbe;
    JPABaseShape* object = pDC->pbsp;
    BaseShapeVtable* vt = object->__vtbl;
    s32 tick = ftoi(emitter->mCurrentFrame);
    s32 count = jpaCall<s32>(vt->getTextureAnmKeyNum, object), q = divide(tick, count);
    u32 target = vt->getTextureIndexFrame, frame = (u32)tick-(u32)q*(u32)count;
    texture(pDC, jpaCall<u32>(target, object, (u8)frame));
}
/* 02837920 */
void JPADrawCalcTextureAnmIndexReverse::calc(const JPADrawContext* pDC) {
    JPABaseShape* object = pDC->pbsp;
    JPABaseEmitter* emitter = pDC->pbe;
    BaseShapeVtable* vt = object->__vtbl;
    s32 tick = ftoi(emitter->mCurrentFrame);
    s32 count = jpaCall<s32>(vt->getTextureAnmKeyNum, object);
    s32 maxFrame = (s32)((u32)count-1), q = divide(tick, maxFrame);
    object = pDC->pbsp;
    u32 rem = (u32)tick-(u32)q*(u32)maxFrame, odd = (u32)q&1;
    u32 target = object->__vtbl->getTextureIndexFrame, frame = rem+odd*(u32)maxFrame-2*(odd*rem);
    texture(pDC, jpaCall<u32>(target, object, (u8)frame));
}
/* 028379D0 */
void JPADrawCalcTextureAnmIndexMerge::calc(const JPADrawContext* pDC) {
    JPABaseShape* object = pDC->pbsp;
    u32 index = jpaCall<u32>(object->__vtbl->getTextureIndex, object);
    pDC->mpDraw->mTexIdx  =  (u16)index;
}
/* 02837A14 */
void JPADrawCalcTextureAnmIndexRandom::calc(const JPADrawContext* pDC) {
    JPABaseShape* object = pDC->pbsp;
    u32 index = jpaCall<u32>(object->__vtbl->getTextureIndex, object);
    pDC->mpDraw->mTexIdx  =  (u16)index;
}
/* 02837A58 */
void JPADrawCalcChildAlphaOut::calc(const JPADrawContext* pDC, JPABaseParticle* ptcl) {
    ptcl->mDrawParams.mAlphaOut  =  c_one-ptcl->mCurNormTime;
}
/* 02837A70 */
void JPADrawCalcChildScaleOut::calc(const JPADrawContext* pDC, JPABaseParticle* ptcl) {
    f32 time = ptcl->mCurNormTime, one = c_one, scale = ptcl->mDrawParams.mScaleOut;
    f32 fade = one-time, random = ptcl->mDrawParams.mAlphaWaveRandom;
    ptcl->mDrawParams.mScaleX  =  scale*fade;
    ptcl->mDrawParams.mScaleY  =  random*fade;
}

} // namespace jpa_calcvis_b
using namespace jpa_calcvis_b;

/* WWHD glue (tools/verify/autoglue.py)
data c_zero 10172238
data c_one 1017223C
data c_half 10172254
data c_two 101722AC
data sincosTable 104A44F8 nocheck
*/
