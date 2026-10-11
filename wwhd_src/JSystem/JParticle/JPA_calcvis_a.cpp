/** HD particle draw visitors: scale and color animation.
 * Virtual getter reloads, GHS fused arithmetic and integer word copies are intentional. */
#include "JSystem/JParticle/JPA_calcvis_a.h"
using namespace gabi;
using namespace JPA_calcvis_a;
f64 JPA_sqrt(f32);
namespace JPA_calcvis_a {
using JPAFloat = be<f32>;
static JPAFloat l_one;
static JPAFloat l_speedScale;
static JPAFloat l_unsignedBoundary;
// HD: retain divw overflow/zero behavior and 32-bit remainder arithmetic.
static s32 quotient(s32 a, s32 b) { return !b || (a == INT32_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b; }
static s32 remainder(s32 a, s32 b) { return (s32)((u32)a - (u32)quotient(a, b) * (u32)b); }
/* 028363DC */
void JPADrawExecCallBack::exec(const JPADrawContext *pDC) {
    auto *emitter = pDC->pbe.get();
    auto *callback = emitter->mpCallBack.get();
    if (callback)
        tail_ptr<void>(callback->__vtbl->draw, callback, emitter);
}

/* 028363FC */
void JPADrawCalcScaleX::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    auto *block = pDC->pesp.get();
    u32 target = block->__vtbl->getScaleInTiming;
    auto *clip = pDC->pcb.get();
    f32 timing = f32(clip->mScaleAnmTiming), in = call_ptr<f32>(target, block);
    block = pDC->pesp.get();
    auto *vt = block->__vtbl.get();
    be<f32> *dst = &params->mScaleX;
    if (timing < in) {
        f32 rate = call_ptr<f32>(vt->getIncreaseRateX, block);
        target = vt->getScaleInValueX;
        clip = pDC->pcb.get();
        timing = f32(clip->mScaleAnmTiming);
        block = pDC->pesp.get();
        f32 product = fmuls_ppc(rate, timing);
        f32 value = call_ptr<f32>(target, block);
        *dst = fmuls_ppc(f32(params->mScaleOut), fadds_ppc(product, value));
    } else {
        target = vt->getScaleOutTiming;
        clip = pDC->pcb.get();
        f32 out = call_ptr<f32>(target, block);
        timing = f32(clip->mScaleAnmTiming);
        if (timing > out) {
            block = pDC->pesp.get();
            vt = block->__vtbl.get();
            f32 rate = call_ptr<f32>(vt->getDecreaseRateX, block);
            target = vt->getScaleOutTiming;
            block = pDC->pesp.get();
            clip = pDC->pcb.get();
            out = call_ptr<f32>(target, block);
            f32 value = fmadds(rate, f32(clip->mScaleAnmTiming) - out, f32(l_one));
            *dst = f32(params->mScaleOut) * value;
        } else
            *reinterpret_cast<be<u32> *>(dst) = *reinterpret_cast<be<u32> *>(&params->mScaleOut);
    }
}

/* 0283652C */
void JPADrawCalcScaleY::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    auto *block = pDC->pesp.get();
    u32 target = block->__vtbl->getScaleInTiming;
    auto *clip = pDC->pcb.get();
    f32 timing = f32(clip->mScaleAnmTiming), in = call_ptr<f32>(target, block);
    block = pDC->pesp.get();
    auto *vt = block->__vtbl.get();
    be<f32> *dst = &params->mScaleY;
    if (timing < in) {
        f32 rate = call_ptr<f32>(vt->getIncreaseRateY, block);
        target = vt->getScaleInValueY;
        clip = pDC->pcb.get();
        timing = f32(clip->mScaleAnmTiming);
        block = pDC->pesp.get();
        f32 product = fmuls_ppc(rate, timing);
        f32 value = call_ptr<f32>(target, block);
        *dst = fmuls_ppc(f32(params->mScaleOut), fadds_ppc(product, value));
    } else {
        target = vt->getScaleOutTiming;
        clip = pDC->pcb.get();
        f32 out = call_ptr<f32>(target, block);
        timing = f32(clip->mScaleAnmTiming);
        if (timing > out) {
            block = pDC->pesp.get();
            vt = block->__vtbl.get();
            f32 rate = call_ptr<f32>(vt->getDecreaseRateY, block);
            target = vt->getScaleOutTiming;
            block = pDC->pesp.get();
            clip = pDC->pcb.get();
            out = call_ptr<f32>(target, block);
            f32 value = fmadds(rate, f32(clip->mScaleAnmTiming) - out, f32(l_one));
            *dst = f32(params->mScaleOut) * value;
        } else
            *reinterpret_cast<be<u32> *>(dst) = *reinterpret_cast<be<u32> *>(&params->mScaleOut);
    }
}

/* 0283665C */
void JPADrawCalcScaleXBySpeed::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    auto *block = pDC->pesp.get();
    u32 target = block->__vtbl->getScaleInTiming;
    auto *clip = pDC->pcb.get();
    f32 vx = f32(ptcl->mVelocity.x);
    f32 vy = f32(ptcl->mVelocity.y);
    f32 vz = f32(ptcl->mVelocity.z);
    f32 timing = f32(clip->mScaleAnmTiming), in = call_ptr<f32>(target, block);
    block = pDC->pesp.get();
    auto *vt = block->__vtbl.get();
    be<f32> *dst = &params->mScaleX;
    if (timing < in) {
        f32 rate = call_ptr<f32>(vt->getIncreaseRateX, block);
        target = vt->getScaleInValueX;
        clip = pDC->pcb.get();
        timing = f32(clip->mScaleAnmTiming);
        block = pDC->pesp.get();
        f32 product = fmuls_ppc(rate, timing);
        f32 value = call_ptr<f32>(target, block);
        *dst = fmuls_ppc(f32(params->mScaleOut), fadds_ppc(product, value));
    } else {
        target = vt->getScaleOutTiming;
        clip = pDC->pcb.get();
        f32 out = call_ptr<f32>(target, block);
        timing = f32(clip->mScaleAnmTiming);
        if (timing > out) {
            block = pDC->pesp.get();
            vt = block->__vtbl.get();
            f32 rate = call_ptr<f32>(vt->getDecreaseRateX, block);
            target = vt->getScaleOutTiming;
            block = pDC->pesp.get();
            clip = pDC->pcb.get();
            out = call_ptr<f32>(target, block);
            f32 value = fmadds(rate, f32(clip->mScaleAnmTiming) - out, f32(l_one));
            *dst = f32(params->mScaleOut) * value;
        } else
            *reinterpret_cast<be<u32> *>(dst) = *reinterpret_cast<be<u32> *>(&params->mScaleOut);
    }

    f32 square = fmadds(vz, vz, fmadds(vx, vx, vy * vy));
    f32 length = (f32)JPA_sqrt(square);
    f32 factor = length * f32(l_speedScale);
    *dst = f32(*dst) * factor;
}

/* 02836864 */
void JPADrawCalcScaleYBySpeed::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    auto *block = pDC->pesp.get();
    u32 target = block->__vtbl->getScaleInTiming;
    auto *clip = pDC->pcb.get();
    f32 vx = f32(ptcl->mVelocity.x);
    f32 vy = f32(ptcl->mVelocity.y);
    f32 vz = f32(ptcl->mVelocity.z);
    f32 timing = f32(clip->mScaleAnmTiming), in = call_ptr<f32>(target, block);
    block = pDC->pesp.get();
    auto *vt = block->__vtbl.get();
    be<f32> *dst = &params->mScaleY;
    if (timing < in) {
        f32 rate = call_ptr<f32>(vt->getIncreaseRateY, block);
        target = vt->getScaleInValueY;
        clip = pDC->pcb.get();
        timing = f32(clip->mScaleAnmTiming);
        block = pDC->pesp.get();
        f32 product = fmuls_ppc(rate, timing);
        f32 value = call_ptr<f32>(target, block);
        *dst = fmuls_ppc(f32(params->mScaleOut), fadds_ppc(product, value));
    } else {
        target = vt->getScaleOutTiming;
        clip = pDC->pcb.get();
        f32 out = call_ptr<f32>(target, block);
        timing = f32(clip->mScaleAnmTiming);
        if (timing > out) {
            block = pDC->pesp.get();
            vt = block->__vtbl.get();
            f32 rate = call_ptr<f32>(vt->getDecreaseRateY, block);
            target = vt->getScaleOutTiming;
            block = pDC->pesp.get();
            clip = pDC->pcb.get();
            out = call_ptr<f32>(target, block);
            f32 value = fmadds(rate, f32(clip->mScaleAnmTiming) - out, f32(l_one));
            *dst = f32(params->mScaleOut) * value;
        } else
            *reinterpret_cast<be<u32> *>(dst) = *reinterpret_cast<be<u32> *>(&params->mScaleOut);
    }

    f32 square = fmadds(vz, vz, fmadds(vx, vx, vy * vy));
    f32 length = (f32)JPA_sqrt(square);
    f32 factor = length * f32(l_speedScale);
    *dst = f32(*dst) * factor;
}

/* 02836A88 */
void JPADrawCalcScaleAnmTimingRepeatX::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    auto *block = pDC->pesp.get();
    s32 age = ftoi(f32(ptcl->mCurFrame));
    auto *vt = block->__vtbl.get();
    auto *clip = pDC->pcb.get();
    s32 cycle = call_ptr<s32>(vt->getAnmCycleX, block);
    s32 frame = remainder(age, cycle);
    u32 target = vt->getAnmCycleX;
    block = pDC->pesp.get();
    f32 numerator = (f32)frame;
    cycle = call_ptr<s32>(target, block);
    clip->mScaleAnmTiming = numerator / (f32)cycle;
}

/* 02836B74 */
void JPADrawCalcScaleAnmTimingRepeatY::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    auto *block = pDC->pesp.get();
    s32 age = ftoi(f32(ptcl->mCurFrame));
    auto *vt = block->__vtbl.get();
    auto *clip = pDC->pcb.get();
    s32 cycle = call_ptr<s32>(vt->getAnmCycleY, block);
    s32 frame = remainder(age, cycle);
    u32 target = vt->getAnmCycleY;
    block = pDC->pesp.get();
    f32 numerator = (f32)frame;
    cycle = call_ptr<s32>(target, block);
    clip->mScaleAnmTiming = numerator / (f32)cycle;
}

/* 02836C60 */
void JPADrawCalcScaleAnmTimingReverseX::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    auto *block = pDC->pesp.get();
    s32 age = ftoi(f32(ptcl->mCurFrame));
    s32 cycle = call_ptr<s32>(block->__vtbl->getAnmCycleX, block);
    block = pDC->pesp.get();
    auto *vt = block->__vtbl.get();
    u32 target = vt->getAnmCycleX;
    f32 odd = (f32)((u32)quotient(age, cycle) & 1);
    age = ftoi(f32(ptcl->mCurFrame));
    cycle = call_ptr<s32>(target, block);
    s32 frame = remainder(age, cycle);
    target = vt->getAnmCycleX;
    block = pDC->pesp.get();
    f32 numerator = (f32)frame;
    cycle = call_ptr<s32>(target, block);
    f32 fraction = numerator / (f32)cycle;
    // HD: the reverse animation keeps the native fused negative multiply-subtract.
    f32 result = odd + fnmsubs(odd + odd, fraction, fraction);
    pDC->pcb->mScaleAnmTiming = result;
}

/* 02836DC0 */
void JPADrawCalcScaleAnmTimingReverseY::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    auto *block = pDC->pesp.get();
    s32 age = ftoi(f32(ptcl->mCurFrame));
    s32 cycle = call_ptr<s32>(block->__vtbl->getAnmCycleY, block);
    block = pDC->pesp.get();
    auto *vt = block->__vtbl.get();
    u32 target = vt->getAnmCycleY;
    f32 odd = (f32)((u32)quotient(age, cycle) & 1);
    age = ftoi(f32(ptcl->mCurFrame));
    cycle = call_ptr<s32>(target, block);
    s32 frame = remainder(age, cycle);
    target = vt->getAnmCycleY;
    block = pDC->pesp.get();
    f32 numerator = (f32)frame;
    cycle = call_ptr<s32>(target, block);
    f32 fraction = numerator / (f32)cycle;
    // HD: the reverse animation keeps the native fused negative multiply-subtract.
    f32 result = odd + fnmsubs(odd + odd, fraction, fraction);
    pDC->pcb->mScaleAnmTiming = result;
}

/* 02836A6C */
void JPADrawCalcScaleCopyX2Y::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    *reinterpret_cast<be<u32> *>(&params->mScaleY) = *reinterpret_cast<be<u32> *>(&params->mScaleX);
}

/* 02836A78 */
void JPADrawCalcScaleAnmTimingNormal::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    auto *clip = pDC->pcb.get();
    *reinterpret_cast<be<u32> *>(&clip->mScaleAnmTiming) = *reinterpret_cast<be<u32> *>(&ptcl->mCurNormTime);
}

/* 02836F20 */
void JPADrawCalcColorPrm::calc(const JPADrawContext *pDC) {
    auto *block = pDC->pbsp.get();
    u32 target = block->__vtbl->getPrmColor;
    auto *clip = pDC->pcb.get();
    s32 frame = s32(clip->mColorAnmFrame);
    auto *draw = pDC->mpDraw.get();
    u32 color = call_ptr<u32>(target, block, frame);
    draw->mPrmColor = color;
}

/* 02836F7C */
void JPADrawCalcColorEnv::calc(const JPADrawContext *pDC) {
    auto *block = pDC->pbsp.get();
    u32 target = block->__vtbl->getEnvColor;
    auto *clip = pDC->pcb.get();
    s32 frame = s32(clip->mColorAnmFrame);
    auto *draw = pDC->mpDraw.get();
    u32 color = call_ptr<u32>(target, block, frame);
    draw->mEnvColor = color;
}

/* 02836FD8 */
void JPADrawCalcColorCopyFromEmitter::calc(const JPADrawContext *pDC, JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    params->mPrmColor = pDC->mpDraw->mPrmColor;
    params->mEnvColor = pDC->mpDraw->mEnvColor;
}

/* 02837004 */
void JPADrawCalcColorAnmFrameNormal::calc(const JPADrawContext *pDC) {
    auto *block = pDC->pbsp.get();
    auto *emitter = pDC->pbe.get();
    u32 target = block->__vtbl->getColorRegAnmMaxFrm;
    s32 tick = ftoi(f32(emitter->mCurFrame));
    s32 max = call_ptr<s32>(target, block);
    if (tick >= max) {
        block = pDC->pbsp.get();
        tick = call_ptr<s32>(block->__vtbl->getColorRegAnmMaxFrm, block);
    }
    pDC->pcb->mColorAnmFrame = (u16)tick;
}

/* 02837088 */
void JPADrawCalcColorAnmFrameRepeat::calc(const JPADrawContext *pDC) {
    f32 boundary = f32(l_unsignedBoundary), tick = f32(pDC->pbe->mCurFrame);
    u32 age = tick < boundary ? (u32)ftoi(tick) : (u32)ftoi(tick - boundary) + 0x80000000u;
    auto *block = pDC->pbsp.get();
    u32 target = block->__vtbl->getColorRegAnmMaxFrm;
    auto *clip = pDC->pcb.get();
    u32 divisor = (u32)call_ptr<s32>(target, block) + 1;
    u32 q = divisor ? age / divisor : 0;
    clip->mColorAnmFrame = (u16)(age - q * divisor);
}

/* 02837150 */
void JPADrawCalcColorAnmFrameReverse::calc(const JPADrawContext *pDC) {
    auto *block = pDC->pbsp.get();
    auto *emitter = pDC->pbe.get();
    u32 target = block->__vtbl->getColorRegAnmMaxFrm;
    s32 age = ftoi(f32(emitter->mCurFrame)), cycle = call_ptr<s32>(target, block);
    u32 q = (u32)quotient(age, cycle), odd = q & 1, frame = (u32)age - q * (u32)cycle;
    pDC->pcb->mColorAnmFrame = (u16)(frame + odd * (u32)cycle - 2 * odd * frame);
}

/* 028371D8 */
void JPADrawCalcColorAnmFrameMerge::calc(const JPADrawContext *pDC) { pDC->pcb->mColorAnmFrame = 0; }

/* 028371E8 */
void JPADrawCalcColorAnmFrameRandom::calc(const JPADrawContext *pDC) { pDC->pcb->mColorAnmFrame = 0; }

} // namespace JPA_calcvis_a
/* WWHD glue (tools/verify/autoglue.py): translation unit data
data l_one 1017223C
data l_speedScale 101722A4
data l_unsignedBoundary 101722A8
ext 028F37F0 f64 JPA_sqrt(f32)
*/
