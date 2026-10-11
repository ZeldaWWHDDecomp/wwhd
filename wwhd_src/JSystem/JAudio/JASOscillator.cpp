/**
 * JAS audio envelope oscillator. GameCube names and WWHD layout, verified against
 * the original by unit JASOscillator.
 */
#include "JSystem/JAudio/JASOscillator.h"
#include <cmath>

using JASystem::TOscillator;
static f32 l_zero;
static f32 l_valueScale;
static f32 l_rateDivisor;
static f32 l_timeDivisor;
static f64 l_zeroDouble;
static f32 l_one;
static f32 l_tableIntervals;
static f32 l_unsignedBoundary;
struct ReleaseTables {
    gptr<be<f32>> table[4];
};
static ReleaseTables l_releaseTables;
static s16 oscTableForceStop[6] = {0, 15, 0, 15, 0, 0};

extern f32 oscillatorDacRate();
extern f32 oscillatorCalcNative(TOscillator*, be<s16>*);
extern void oscillatorUpdateNative(TOscillator*);

/* 02816448 */
void TOscillator::init() {
    // Copy the constant's word, preserving the original lfs/stfs payload.
    const be<f32>& zero = *reinterpret_cast<const be<f32>*>(&l_zero);
    field_0x6 = 0;
    mOsc = nullptr;
    mState = STOP;
    field_0x5 = 0;
    mDirectRelease = 0;
    mReleaseRate = zero;
    mPhase = zero;
    mTargetPhase = zero;
    mPhaseChangeRate = zero;
    mInitialReleaseRate = zero;
}

/* 02816480 */
f32 TOscillator::calc(be<s16>* i_table) {
    f32 zero = l_zero;
    // HD: unordered rates enter the loop too.
    while (!(mReleaseRate > zero)) {
        u32 idx = field_0x6;
        mPhase = mTargetPhase;
        if (mState == DIRECT_RELEASE) {
            mState = STOP;
            break;
        }
        s32 envMode = i_table[idx * 3];
        s32 envTime = i_table[idx * 3 + 1];
        s32 envValue = i_table[idx * 3 + 2];
        if (envMode == JUMP) {
            field_0x6 = (u16)envValue;
            continue;
        }
        if (envMode == END) {
            mState = STOP;
            break;
        }
        if (envMode == SUSTAIN) {
            mState = HOLD;
            const Osc_* osc = mOsc;
            return fmadds(mPhase, osc->field_0x10, osc->field_0x14);
        }
        field_0x5 = (u8)envMode;
        f32 scale = l_valueScale;
        if (envTime == 0) {
            mTargetPhase = fmuls_ppc((f32)envValue, scale);
            field_0x6 = (u16)(field_0x6 + 1);
            continue;
        }
        // HD: the fixed divisors include the update interval.
        f32 rate = oscillatorDacRate();
        f32 duration = fmuls_ppc((f32)((f32)(rate / l_rateDivisor) / l_timeDivisor), (f32)envTime);
        u8 curve = field_0x5;
        u16 next = field_0x6;
        f32 phase = mPhase;
        f32 target = fmuls_ppc((f32)envValue, scale);
        mReleaseRate = duration;
        mInitialReleaseRate = duration;
        f32 change = fsubs_ppc(target, phase);
        mTargetPhase = target;
        if (!curve)
            change = (f32)(change / mReleaseRate);
        mPhaseChangeRate = change;
        field_0x6 = (u16)(next + 1);
    }
    const Osc_* osc = mOsc;
    if (osc->field_0x10 == zero)
        return osc->field_0x14;
    f32 target = mTargetPhase;
    f32 newPhase;
    // GHS keeps the original unsuffixed-zero comparison in double precision.
    if ((f64)mInitialReleaseRate == l_zeroDouble) {
        mPhase = mTargetPhase;
        newPhase = target;
    } else {
        f32 change = mPhaseChangeRate;
        u8 curve = field_0x5;
        if (curve >= 1 && curve <= 3 && change != zero) {
            be<f32>* table = l_releaseTables.table[curve];
            f32 one = l_one;
            f32 ratio = (f32)(mReleaseRate / mInitialReleaseRate);
            f32 fIdx = fmuls_ppc(change < zero ? fsubs_ppc(one, ratio) : ratio, l_tableIntervals);
            u32 idx;
            f32 prop;
            // GHS's unsigned conversion retains wrapping for out-of-range inputs.
            if (fIdx < l_unsignedBoundary) {
                idx = (u32)ftoi(fIdx);
                prop = fsubs_ppc(fIdx, (f32)idx);
            } else {
                u32 raw = (u32)ftoi(fsubs_ppc(fIdx, l_unsignedBoundary));
                idx = raw + 0x80000000u;
                prop = fsubs_ppc(fIdx, (f32)idx);
            }
            if (idx >= 16) {
                idx = 15;
                prop = one;
            }
            f32 first = table[idx];
            f32 second = table[idx + 1];
            f32 valAbs =
                std::fabs(fmuls_ppc(fmadds(fsubs_ppc(second, first), prop, first), change));
            if (change < zero)
                newPhase = fadds_ppc(target, valAbs);
            else
                newPhase = fsubs_ppc(target, fsubs_ppc(change, valAbs));
        } else {
            newPhase = fnmsubs(change, mReleaseRate, target);
        }
        mPhase = newPhase;
    }
    osc = mOsc;
    return fmadds(newPhase, osc->field_0x10, osc->field_0x14);
}

/* 028168F8 */
void TOscillator::getOffset() {
    u8 state = mState;
    if (state == STOP || state == HOLD)
        return;
    f32 one = l_one;
    const Osc_* osc = mOsc;
    be<s16>* table;
    if (state == RELEASE) {
        table = osc->rel_table;
    } else if (state == FORCE_STOP) {
        mReleaseRate = fsubs_ppc(mReleaseRate, one);
        oscillatorCalcNative(this, reinterpret_cast<be<s16>*>(oscTableForceStop));
        return;
    } else {
        table = osc->table;
    }
    if (table != nullptr || state == DIRECT_RELEASE) {
        mReleaseRate = fsubs_ppc(mReleaseRate, osc->field_0x4);
        oscillatorCalcNative(this, table);
    } else {
        mPhase = one;
    }
}

/* 02816980 */
void TOscillator::initStart(const Osc_* osc) {
    if (osc == nullptr) {
        mState = STOP;
        return;
    }
    mOsc = osc;
    mDirectRelease = 0;
    mState = ATTACK;
    f32 zero = l_zero;
    if (!osc->table) {
        mPhase = zero;
        return;
    }
    mReleaseRate = zero;
    mTargetPhase = zero;
    field_0x6 = 0;
    mReleaseRate = fsubs_ppc(zero, mOsc->field_0x4);
    oscillatorUpdateNative(this);
}

/* 028169E0 */
f32 TOscillator::getValue() {
    if (!mState)
        return l_one;
    const Osc_* osc = mOsc;
    return fmadds(mPhase, osc->field_0x10, osc->field_0x14);
}

/* 02816A10 */
u32 TOscillator::release() {
    if (mState == FORCE_STOP)
        return 0;
    f32 zero = l_zero;
    const Osc_* osc = mOsc;
    u32 direct = mDirectRelease;
    if (osc->table != osc->rel_table) {
        mReleaseRate = zero;
        mTargetPhase = mPhase;
        field_0x6 = 0;
    }
    if (!direct) {
        if (mOsc->rel_table) {
            mState = RELEASE;
            return 1;
        }
        mDirectRelease = 16;
    }
    direct = mDirectRelease;
    mState = DIRECT_RELEASE;
    field_0x5 = (u8)((direct >> 14) & 3);
    f32 count = (f32)(direct & 0x3FFF);
    f32 rate = oscillatorDacRate();
    f32 duration = fmuls_ppc(count, (f32)((f32)(rate / l_rateDivisor) / l_timeDivisor));
    f32 one = l_one;
    if (duration < one)
        duration = one;
    u8 curve = field_0x5;
    mReleaseRate = duration;
    mInitialReleaseRate = duration;
    mTargetPhase = zero;
    f32 phase = mPhase;
    mPhaseChangeRate = curve ? -phase : -(f32)(phase / mReleaseRate);
    return 1;
}

/* WWHD glue
data l_zero 101703F8
data l_valueScale 101703FC
data l_rateDivisor 10170408
data l_timeDivisor 1017040C
data l_zeroDouble 10170410
data l_one 10170418
data l_tableIntervals 1017041C
data l_unsignedBoundary 10170420
data l_releaseTables 1017042C
data oscTableForceStop 10170444
ext 02813AD4 f32 oscillatorDacRate()
ext 02816480 f32 oscillatorCalcNative(TOscillator*, be<s16>*)
ext 028168F8 void oscillatorUpdateNative(TOscillator*)
*/
