/* Component interpolation, chase helpers and matrix stack. GameCube names and verified HD
 * call order, register widths and NaN behavior. */
#include "SSystem/SComponent/c_lib.h"
#include "true60.h"
#include "SSystem/SComponent/c_lib_callsig_t60.h"
#include <cmath>
#include <unordered_map>
using namespace gabi;

// true60 (the per-step smoothing rule and 1.6): the smoothing and chase primitives advance by the running code's step length g_dt (1
// outside converted code: the original arithmetic, unchanged). For dt != 1 a ratio r becomes 1 - (1 - r)^dt and a per-step
// amount (maximum, minimum, chase step) is scaled by dt.
#define T60_SMOOTH(scale, maxStep, minStep)                    \
    if (t60::dt() != 1.0f) {                                    \
        scale = t60_ratio(scale, t60::dt());                    \
        maxStep = fmuls_ppc(maxStep, t60::dt());                \
        minStep = fmuls_ppc(minStep, t60::dt());                \
    }
#define T60_SMOOTH2(scale, maxStep)                             \
    if (t60::dt() != 1.0f) {                                    \
        scale = t60_ratio(scale, t60::dt());                    \
        maxStep = fmuls_ppc(maxStep, t60::dt());                \
    }
#define T60_STEP(step)                                          \
    if (t60::dt() != 1.0f)                                      \
        step = fmuls_ppc(step, t60::dt());

/* true60: a converted chase's "arrived" test on the step that ends the frame
 * also takes a remainder within a relative 1e-6 of the step (the N steps' rounding: two half steps of a distance equal to
 * the frame's step left ~1e-24, so B arrived one frame late; wind_tag's path points). dt 1 / earlier steps: 0. */
static inline f32 t60ArriveSlack(f32 step) {
    return (t60::dt() != 1.0f && t60::tick30()) ? std::fabs(step) * 1.0e-6f : 0.0f;
}

using Float = be<f32>;
using MatrixStack = Mtx34[10];
using MatrixPointer = gptr<Mtx34>;
using SinCosTable = cLib_sinCosHD[8192];
using RuntimeObjects = cLib_runtimeObjectHD[3];
static Float zeroValueHD;
static Float zeroThresholdHD;
static MatrixPointer calc_mtx;
static MatrixStack mtx;
static SinCosTable sinCosTable;
static cLib_runtimeConstantsHD runtimeConstants;
static cLib_runtimeStateHD runtimeState;
static RuntimeObjects runtimeObjects;

// HD: preserve the single-precision square-root ABI and explicit callee payloads.

static void copyPositionWordsHD(cXyz* dst, cXyz* src) { dst->copy(*src); }

static void copyTemporaryWordsHD(cXyz* dst, cXyz* src) {
    cLib_vectorWordsHD* out = reinterpret_cast<cLib_vectorWordsHD*>(dst);
    cLib_vectorWordsHD* in = reinterpret_cast<cLib_vectorWordsHD*>(src);
    u32 y = in->y, z = in->z, x = in->x;
    out->y = y;
    out->z = z;
    out->x = x;
}

static f32 vectorLengthHD(cXyz* v) {
    f32 q = PSVECSquareMag(v);
    return cLib_sqrtfHD(q);
}

static f32 vectorDistanceHD(cXyz* a, cXyz* b) {
    f32 q = PSVECSquareDistance(a, b);
    return cLib_sqrtfHD(q);
}

static void flattenXZHD(cXyz* d, cXyz* s, f32 zero) {
    f32 x = s->x, z = s->z;
    d->x = x;
    d->y = zero;
    d->z = z;
}

/* 0200ECCC */
void cLib_memSet(void* ptr, u32 value, u32 size) { cLib_memsetHD(ptr, value & 255, size); }

// HD: unordered comparisons and explicit PPC arithmetic preserve NaN payloads.
/* 0200ECD4 */
f32 cLib_addCalc(be<f32>* pValue, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    T60_SMOOTH(scale, maxStep, minStep);


    f32 x = *pValue;
    if (x == target)
        return std::fabs(fsubs_ppc(target, x));

    f32 diff = fsubs_ppc(target, x), step = fmuls_ppc(diff, scale);

    if (!(step < minStep) || !(step > -minStep)) {
        if (step > maxStep)
            step = maxStep;
        if (step < -maxStep)
            step = -maxStep;
        x = fadds_ppc(x, step);
        *pValue = x;
        return std::fabs(fsubs_ppc(target, x));
    }

    if (step > (f32)zeroValueHD) {
        x = fadds_ppc(x, minStep);
        if (x > target)
            x = target;
    }

    else {
        x = fadds_ppc(x, -minStep);
        if (x < target)
            x = target;
    }

    *pValue = x;
    return std::fabs(fsubs_ppc(target, x));
}

/* 0200ED84 */
void cLib_addCalc2(be<f32>* pValue, f32 target, f32 scale, f32 maxStep) {
    T60_SMOOTH2(scale, maxStep);

    f32 x = *pValue;
    if (x == target)
        return;
    f32 step = fmuls_ppc(fsubs_ppc(target, x), scale);
    if (step > maxStep) {
        *pValue = fadds_ppc(x, maxStep);
        return;
    }
    if (step < -maxStep)
        step = -maxStep;
    *pValue = fadds_ppc(x, step);
}

/* 0200EDC8 */
void cLib_addCalc0(be<f32>* pValue, f32 scale, f32 maxStep) {
    T60_SMOOTH2(scale, maxStep);

    f32 x = *pValue, step = fmuls_ppc(x, scale);
    if (step > maxStep) {
        *pValue = fsubs_ppc(x, maxStep);
        return;
    }
    if (step < -maxStep)
        step = -maxStep;
    *pValue = fsubs_ppc(x, step);
}

/* 0200EE00 */
f32 cLib_addCalcPos(cXyz* pValue, cXyz* target, f32 scale, f32 maxStep, f32 minStep) {
    T60_SMOOTH(scale, maxStep, minStep);


    if (cXyz_notEqualHD(pValue, target) != 0) {
        cXyz delta;
        cXyz step;
        cXyz_subtractHD(pValue, &delta, target);
        copyTemporaryWordsHD(&step, &delta);
        f32 d = vectorLengthHD(&step);

        if (!(d < minStep)) {
            f32 scaled = fmuls_ppc(d, scale);
            PSVECScale(&step, &step, scale);
            if (!(std::fabs(scaled) < (f32)zeroThresholdHD)) {
                if (scaled > maxStep || scaled < minStep) {
                    if (!(scaled > maxStep))
                        maxStep = minStep;
                    PSVECScale(&step, &step, maxStep / scaled);
                }
                PSVECSubtract(pValue, &step, pValue);
                return vectorDistanceHD(pValue, target);
            }
        }

        copyPositionWordsHD(pValue, target);
    }

    return vectorDistanceHD(pValue, target);
}

/* 0200EF78 */
f32 cLib_addCalcPosXZ(cXyz* pValue, cXyz* target, f32 scale, f32 maxStep, f32 minStep) {
    T60_SMOOTH(scale, maxStep, minStep);


    f32 zero = (f32)zeroValueHD;

    if (!(pValue->x == target->x && pValue->z == target->z)) {
        cXyz delta;
        cXyz flat;
        cXyz_subtractHD(pValue, &delta, target);
        flattenXZHD(&flat, &delta, zero);
        f32 d = vectorLengthHD(&flat);

        if (!(d < minStep)) {
            f32 scaled = fmuls_ppc(d, scale);
            PSVECScale(&delta, &delta, scale);
            if (!(std::fabs(scaled) < (f32)zeroThresholdHD)) {
                if (scaled > maxStep || scaled < minStep) {
                    if (!(scaled > maxStep))
                        maxStep = minStep;
                    PSVECScale(&delta, &delta, maxStep / scaled);
                }
                f32 x = pValue->x, a = delta.x, b = delta.z;
                f32 nx = fsubs_ppc(x, a);
                f32 z = pValue->z, nz = fsubs_ppc(z, b);
                pValue->x = nx;
                pValue->z = nz;
                cXyz remaining;
                cXyz restflat;
                cXyz_subtractHD(pValue, &remaining, target);
                f32 rz = remaining.z, rx = remaining.x;
                restflat.z = rz;
                restflat.x = rx;
                restflat.y = zero;
                return vectorLengthHD(&restflat);
            }
        }

        // A target snap copies components without arithmetic, preserving payload bits.
        cLib_vectorWordsHD* dst = reinterpret_cast<cLib_vectorWordsHD*>(pValue);
        cLib_vectorWordsHD* src = reinterpret_cast<cLib_vectorWordsHD*>(target);
        dst->x = (u32)src->x;
        dst->z = (u32)src->z;
    }

    cXyz remaining;

    cXyz flat;
    cXyz_subtractHD(pValue, &remaining, target);
    flattenXZHD(&flat, &remaining, zero);
    return vectorLengthHD(&flat);
}

/* 0200F164 */
void cLib_addCalcPos2(cXyz* pValue, cXyz* target, f32 scale, f32 maxStep) {
    T60_SMOOTH2(scale, maxStep);


    if (cXyz_notEqualHD(pValue, target) == 0)
        return;
    cXyz delta;
    cXyz scaled;
    cXyz step;
    cXyz normal;
    cXyz_subtractHD(pValue, &delta, target);
    cXyz_scaleResultHD(&delta, &scaled, scale);
    copyTemporaryWordsHD(&step, &scaled);
    f32 d = vectorLengthHD(&step);
    if (d > maxStep) {
        cXyz_normalizeResultHD(&step, &normal);
        copyTemporaryWordsHD(&step, &normal);
        PSVECScale(&step, &step, maxStep);
    }
    PSVECSubtract(pValue, &step, pValue);
}

/* 0200F268 */
void cLib_addCalcPosXZ2(cXyz* pValue, cXyz* target, f32 scale, f32 maxStep) {
    T60_SMOOTH2(scale, maxStep);


    if (pValue->x == target->x && pValue->z == target->z)
        return;
    cXyz delta;
    cXyz step;
    cXyz flat;
    cXyz_subtractHD(pValue, &delta, target);
    cXyz_scaleResultHD(&delta, &step, scale);
    flattenXZHD(&flat, &step, (f32)zeroValueHD);
    f32 d = vectorLengthHD(&flat);
    if (std::fabs(d) < (f32)zeroThresholdHD)
        return;
    if (d > maxStep)
        PSVECScale(&step, &step, maxStep / d);
    f32 x = pValue->x, a = step.x, z = pValue->z;
    f32 nx = fsubs_ppc(x, a), b = step.z, nz = fsubs_ppc(z, b);
    pValue->x = nx;
    pValue->z = nz;
}


// true60: the integer variants carry the 30 Hz frame's remainder. Per value (keyed by its
// guest address) a host side entry holds the frame's start value; in-between sub-steps move toward the target but never
// past what the original one step from the start reaches, and the tick30 sub-step writes exactly the original one-step
// result from the start ("the full step gets the rest"). The frame tag is g_Counter.mCounter0 (c_counter.cpp, data
// 101FF558), which counts 30 Hz frames and only changes after the tick30 step's process executes.
// A value whose frame entry begins after the frame's first step (smoothing that starts mid-frame, e.g. procWaitTurn begun
// on the first step and smoothing from its first run on the tick30 step; or a value written by someone else since) has
// only part of the frame left: its sub-steps, the tick30 one included, all take the in-between form (a step's share,
// never past the original one step from the start), so the value does not get the whole frame's step on its last
// sub-step; the tick30 sub-step then ends the entry and the next frame starts exact again.
namespace {
struct T60IntEntry {
    u32 addr, frame;
    s32 start, last;
    u32 k;
    bool partial; /* begun after the frame's first step */
    u32 sig = 0;  /* the opening call's parameters (c_lib_callsig_t60.h; addCalcAngleS) */
    u32 step = 0; /* the step counter of the entry's last call */
    u32 sig2 = 0;  /* another call site's parameters met within the frame */
    u32 site = 0;  /* the opening call's site (t60cs_site) */
    u32 site2 = 0; /* another call site met within the frame */
};
constexpr u32 kT60IntSlots = 2048, kT60IntProbes = 8, kT60FrameCounter = 0x101FF558;
T60IntEntry t60IntTable[kT60IntSlots];
static_assert((kT60IntSlots & (kT60IntSlots - 1)) == 0, "a power of two");
constexpr u32 kT60IntShift = 32 - __builtin_ctz(kT60IntSlots); /* the hash's top log2(slots) bits */
/* Overflow (all probes of a value's hash taken by this frame's entries): the entry goes to a host map instead, so the
 * value keeps the exact frame split. (Before 2026-11-16 a full table returned no entry and every sub-step then wrote the
 * whole frame's step: the value moved N times too fast.) Cleared when the frame tag changes; counted in
 * wwhd_t60_ismooth_overflow. Values that begin an entry per frame are counted for wwhd_t60_ismooth_peak (true60.h). */
std::unordered_map<u32, T60IntEntry> t60IntOverflow;
u32 t60IntOverflowFrame = ~0u, t60IntUseFrame = ~0u, t60IntUse = 0;

void t60IntCountUse(u32 frame) {
    if (frame != t60IntUseFrame) {
        t60IntUseFrame = frame;
        t60IntUse = 0;
    }
    if (++t60IntUse > wwhd_t60_ismooth_peak)
        wwhd_t60_ismooth_peak = t60IntUse;
}

/* the frame's entry for the value at p (current value cur); never nullptr */
T60IntEntry* t60IntEntry(const void* p, s32 cur) {
    u32 a = gabi::ea(p), frame = *gabi::at<be<u32>>(kT60FrameCounter);
    u32 h = ((a >> 1) * 2654435761u) >> kT60IntShift;
    T60IntEntry* free = nullptr;
    for (u32 i = 0; i < kT60IntProbes; i++) {
        T60IntEntry* e = &t60IntTable[(h + i) & (kT60IntSlots - 1)];
        if (e->addr == a && e->frame == frame) {
            if (e->last != cur) /* written by someone else since: the frame starts again here */
                *e = T60IntEntry{a, frame, cur, cur, 0, !t60::first_step()};
            return e;
        }
        if (!free && (e->addr == 0 || e->frame != frame || e->addr == a))
            free = e;
    }
    if (t60IntOverflowFrame != frame) {
        t60IntOverflowFrame = frame;
        t60IntOverflow.clear();
    }
    if (!t60IntOverflow.empty()) { /* (a value that overflowed earlier this frame keeps its overflow entry) */
        auto it = t60IntOverflow.find(a);
        if (it != t60IntOverflow.end() && it->second.addr == a && it->second.frame == frame) {
            T60IntEntry* e = &it->second;
            if (e->last != cur)
                *e = T60IntEntry{a, frame, cur, cur, 0, !t60::first_step()};
            return e;
        }
    }
    t60IntCountUse(frame);
    if (!free) {
        wwhd_t60_ismooth_overflow++;
        free = &t60IntOverflow[a]; /* (element addresses stay valid while the map grows) */
    }
    *free = T60IntEntry{a, frame, cur, cur, 0, !t60::first_step()};
    return free;
}

/* the sub-step writes the original one step from the frame's start: the tick30 sub-step of a frame the entry covers from
 * its first step */
bool t60PinFull(const T60IntEntry* e) { return t60::tick30() && !e->partial; }

/* ceil(|v|) with the sign of v, as an integer step */
s64 t60AwayFromZero(double v) { return v >= 0 ? (s64)std::ceil(v) : -(s64)std::ceil(-v); }

/* clamp an offset o (from the frame's start) into the segment [0, S] */
s64 t60ClampOffset(s64 o, s64 S) {
    if (S >= 0)
        return o < 0 ? 0 : (o > S ? S : o);
    return o > 0 ? 0 : (o < S ? S : o);
}

/* split of a frame's displacement S over N sub-steps: the offset after in-between sub-step k (rounded away from 0) */
s64 t60SplitOffset(s64 S, u32 k, u32 n) {
    s64 a = S < 0 ? -S : S, o = (a * (s64)(k + 1) + (s64)n - 1) / (s64)n;
    return S < 0 ? -o : o;
}

/* the original one-step results as value functions (same arithmetic as the dt = 1 code below) */
s32 divideSignedPPC_t60(s32 a, s32 b) { return !b || (a == INT32_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b; }

s16 t60AngleSFull(s32 x, s32 target, s32 scale, s32 maxStep, s32 minStep, s16* ret) {
    s16 diff = (s16)((u32)target - (u32)x);
    if (x == target) {
        *ret = diff;
        return (s16)x;
    }
    s32 step = (s16)divideSignedPPC_t60(diff, scale);
    if (step > minStep || step < (s32)(0u - (u32)minStep)) {
        if (step > maxStep)
            step = maxStep;
        if (step < (s32)(0u - (u32)maxStep))
            step = (s16)(0u - (u32)maxStep);
        x = (s32)((u32)x + (u32)step);
        *ret = (s16)((u32)target - (u32)x);
        return (s16)x;
    }
    if (diff >= 0) {
        x = (s16)((u32)x + (u32)minStep);
        s16 rem = (s16)((u32)target - (u32)x);
        if (rem > 0) {
            *ret = rem;
            return (s16)x;
        }
    } else {
        x = (s16)((u32)x - (u32)minStep);
        s16 rem = (s16)((u32)target - (u32)x);
        if (rem < 0) {
            *ret = rem;
            return (s16)x;
        }
    }
    *ret = 0;
    return (s16)target;
}

s16 t60AngleS2Full(s32 x, s32 target, s32 scale, s32 maxStep) {
    s32 step = (s16)divideSignedPPC_t60((s16)((u32)target - (u32)x), scale);
    if (step > maxStep)
        step = maxStep;
    else if (step < (s32)(0u - (u32)maxStep))
        step = (s32)(0u - (u32)maxStep);
    return (s16)((u32)x + (u32)step);
}

s32 t60AngleLFull(u32 x, s32 target, s32 scale, s32 maxStep, s32 minStep) {
    s32 diff = (s32)((u32)target - x);
    if (x == (u32)target)
        return (s32)x;
    s32 step = divideSignedPPC_t60(diff, scale), nmin = (s32)(0u - (u32)minStep), nmax = (s32)(0u - (u32)maxStep);
    if (step > minStep || step < nmin) {
        if (step > maxStep)
            step = maxStep;
        if (step < nmax)
            step = nmax;
        return (s32)(x + (u32)step);
    }
    u32 next = diff >= 0 ? x + (u32)minStep : x - (u32)minStep;
    s32 rem = (s32)((u32)target - next);
    return ((diff >= 0 && rem > 0) || (diff < 0 && rem < 0)) ? (s32)next : target;
}

/* chaseS / chaseUC / chaseAngleS: the new value (wrapped to the field by the caller) and the return value; *write false:
 * the original does not store (step 0) */
s32 t60ChaseFull(s32 x, s32 target, s32 step, int kind, s32* ret, bool* write) {
    *write = step != 0;
    if (step == 0) {
        *ret = x == target;
        return x;
    }
    if (kind == 2 ? (s16)((u32)x - (u32)target) > 0 : x > target)
        step = (s16)(0u - (u32)step);
    x = (s16)((u32)x + (u32)step);
    s32 prod = kind == 2 ? (s32)((u32)(s32)(s16)((u32)x - (u32)target) * (u32)(s32)step)
                         : (s32)((u32)((u32)x - (u32)target) * (u32)(s32)step);
    if (prod < 0) {
        *ret = 0;
        return x;
    }
    *ret = 1;
    return target;
}

/* the in-between sub-step's new value of an integer smoothing: dynamic step `move` from cur, the frame's offset clamped
 * to the original one-step displacement S from start (wrapped differences for angles: bits 16 or 32) */
s64 t60Wrap(s64 v, int bits) { return bits == 16 ? (s64)(s16)v : bits == 32 ? (s64)(s32)v : v; }
}  // namespace

// HD: integer parameters stay full register width; HD truncates only explicit halfword operations.
// HD: PPC signed division by zero/overflow returns a sign-derived sentinel.
static s32 divideSignedPPC(s32 a, s32 b) {
    return !b || (a == INT32_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b;
}


/* true60: chaseUC (kind 0), chaseS (1), chaseAngleS (2) on an in-between or tick30 sub-step: the frame's step
 * split as ceil((k+1)*S/N) from the start, the tick30 sub-step the original one step from the start */
template <class T> static s32 t60Chase(be<T>* pValue, s32 target, s32 step, int kind) {
    s32 cur = *pValue;
    T60IntEntry* e = t60IntEntry(pValue, cur);
    s32 start = e ? e->start : cur, ret;
    bool write;
    s32 full = t60ChaseFull(start, target, step, kind, &ret, &write);
    if (t60PinFull(e)) {
        if (e)
            e->addr = 0;
        if (write || start != cur)
            *pValue = (T)full;
        if (t60::tick30())
            return ret;
        return step == 0 ? cur == target : (T)full == (T)target;
    }
    s64 S = kind == 2 ? (s64)(s16)((u32)(T)full - (u32)(T)start) : (s64)(T)full - (s64)(T)start;
    s64 o = t60SplitOffset(S, e->k, t60::steps());
    T nx = (T)((u32)start + (u32)o);
    e->k++;
    e->last = (s32)nx;
    if (t60::tick30())
        e->addr = 0; /* partial frame (see t60IntEntry) ends here */
    if (nx != (T)cur)
        *pValue = nx;
    return step == 0 ? cur == target : (o == S && write && ret);
}

/* 0200F378 */
s16 cLib_addCalcAngleS(be<s16>* pValue, s32 target, s32 scale, s32 maxStep, s32 minStep) {
    if (t60::dt() != 1.0f) {
        s32 cur = *pValue;
        T60IntEntry* e = t60IntEntry(pValue, cur);
        t60cs_check(e, t60cs_sig(scale, maxStep, minStep), cur, T60CS_SITE()); /* true60: another call site's frame */
        s32 start = e ? e->start : cur;
        s16 ret;
        s16 full = t60AngleSFull(start, target, scale, maxStep, minStep, &ret);
        if (t60PinFull(e)) {
            if (e)
                e->addr = 0;
            if (start != target || start != cur) /* the original stores unless the value is on the target */
                *pValue = full;
            return ret;
        }
        double dt = t60::dt();
        s64 diff = (s16)((u32)target - (u32)cur), S = (s16)((u32)full - (u32)start), move = 0;
        if (diff != 0) {
            double mv = scale >= 2 ? (double)diff * t60_ratio(1.0f / (float)scale, (float)dt) : (double)diff;
            double mx = (double)maxStep * dt, mn = (double)minStep * dt;
            if (mv > mx) mv = mx;
            if (mv < -mx) mv = -mx;
            if (mv > -mn && mv < mn) mv = diff > 0 ? mn : -mn;
            move = t60AwayFromZero(mv);
        }
        s64 o = t60ClampOffset((s16)((u32)cur + (u32)move - (u32)start), S);
        s16 nx = (s16)((u32)start + (u32)o);
        e->k++;
        e->last = nx;
        if (t60::tick30())
            e->addr = 0; /* partial frame (see t60IntEntry) ends here */
        if (nx != cur)
            *pValue = nx;
        return (s16)((u32)target - (u32)nx);
    }

    s32 x = *pValue;
    s16 diff = (s16)((u32)target - (u32)x);
    if (x == target)
        return diff;
    s32 step = (s16)divideSignedPPC(diff, scale);

    if (step > minStep || step < (s32)(0u - (u32)minStep)) {
        if (step > maxStep)
            step = maxStep;
        if (step < (s32)(0u - (u32)maxStep))
            step = (s16)(0u - (u32)maxStep);
        x = (s32)((u32)x + (u32)step);
        *pValue = (s16)x;
        return (s16)((u32)target - (u32)x);
    }

    if (diff >= 0) {
        x = (s16)((u32)x + (u32)minStep);
        s16 rem = (s16)((u32)target - (u32)x);
        if (rem > 0) {
            *pValue = (s16)x;
            return rem;
        }
    }

    else {
        x = (s16)((u32)x - (u32)minStep);
        s16 rem = (s16)((u32)target - (u32)x);
        if (rem < 0) {
            *pValue = (s16)x;
            return rem;
        }
    }

    *pValue = target;
    return 0;
}

/* 0200F428 */
void cLib_addCalcAngleS2(be<s16>* pValue, s32 target, s32 scale, s32 maxStep) {
    if (t60::dt() != 1.0f) {
        s32 cur = *pValue;
        T60IntEntry* e = t60IntEntry(pValue, cur);
        s32 start = e ? e->start : cur;
        s16 full = t60AngleS2Full(start, target, scale, maxStep);
        if (t60PinFull(e)) {
            if (e)
                e->addr = 0;
            *pValue = full; /* the original always stores */
            return;
        }
        double dt = t60::dt();
        s64 diff = (s16)((u32)target - (u32)cur), S = (s16)((u32)full - (u32)start);
        double mv = scale >= 2 ? (double)diff * t60_ratio(1.0f / (float)scale, (float)dt) : (double)diff;
        double mx = (double)maxStep * dt;
        if (mv > mx) mv = mx;
        if (mv < -mx) mv = -mx;
        s64 o = t60ClampOffset((s16)((u32)cur + (u32)t60AwayFromZero(mv) - (u32)start), S);
        s16 nx = (s16)((u32)start + (u32)o);
        e->k++;
        e->last = nx;
        if (t60::tick30())
            e->addr = 0; /* partial frame (see t60IntEntry) ends here */
        *pValue = nx;
        return;
    }
    s32 x = *pValue, step = (s16)divideSignedPPC((s16)((u32)target - (u32)x), scale);
    if (step > maxStep)
        step = maxStep;
    else if (step < (s32)(0u - (u32)maxStep))
        step = (s32)(0u - (u32)maxStep);
    *pValue = (s16)((u32)x + (u32)step);
}

/* 0200F474 */
s32 cLib_addCalcAngleL(be<s32>* pValue, s32 target, s32 scale, s32 maxStep, s32 minStep) {
    if (t60::dt() != 1.0f) {
        s32 cur = *pValue;
        T60IntEntry* e = t60IntEntry(pValue, cur);
        s32 start = e ? e->start : cur;
        s32 diff = (s32)((u32)target - (u32)cur);
        if (diff == 0 && start == cur) { /* the original returns before any store */
            if (t60::tick30() && e)
                e->addr = 0;
            return diff;
        }
        s32 full = t60AngleLFull((u32)start, target, scale, maxStep, minStep);
        if (t60PinFull(e)) {
            if (e)
                e->addr = 0;
            *pValue = full;
            return t60::tick30() ? (s32)((u32)target - (u32)start) : diff;
        }
        double dt = t60::dt();
        s64 S = (s32)((u32)full - (u32)start), move = 0;
        if (diff != 0) {
            double mv = scale >= 2 ? (double)diff * t60_ratio(1.0f / (float)scale, (float)dt) : (double)diff;
            double mx = (double)maxStep * dt, mn = (double)minStep * dt;
            if (mv > mx) mv = mx;
            if (mv < -mx) mv = -mx;
            if (mv > -mn && mv < mn) mv = diff > 0 ? mn : -mn;
            move = t60AwayFromZero(mv);
        }
        s64 o = t60ClampOffset((s32)((u32)cur + (u32)move - (u32)start), S);
        s32 nx = (s32)((u32)start + (u32)o);
        e->k++;
        e->last = nx;
        if (t60::tick30())
            e->addr = 0; /* partial frame (see t60IntEntry) ends here */
        if (nx != cur)
            *pValue = nx;
        return diff;
    }

    u32 x = (u32)(s32)*pValue;
    s32 diff = (s32)((u32)target - x);
    if (x == (u32)target)
        return diff;
    s32 step = divideSignedPPC(diff, scale), nmin = (s32)(0u - (u32)minStep),
        nmax = (s32)(0u - (u32)maxStep);

    if (step > minStep || step < nmin) {
        if (step > maxStep)
            step = maxStep;
        if (step < nmax)
            step = nmax;
        *pValue = (s32)(x + (u32)step);
    }

    else {
        u32 next = diff >= 0 ? x + (u32)minStep : x - (u32)minStep;
        s32 rem = (s32)((u32)target - next);
        *pValue = ((diff >= 0 && rem > 0) || (diff < 0 && rem < 0)) ? (s32)next : target;
    }

    return diff;
}

/* 0200F4FC */
s32 cLib_chaseUC(be<u8>* pValue, s32 target, s32 step) {
    if (t60::dt() != 1.0f)
        return t60Chase(pValue, target, step, 0);
    s32 x = *pValue;
    if (step == 0)
        return x == target;
    if (x > target)
        step = (s16)(0u - (u32)step);
    x = (s16)((u32)x + (u32)step);
    s32 prod = (s32)((u32)((u32)x - (u32)target) * (u32)(s32)step);
    if (prod < 0) {
        *pValue = (u8)x;
        return 0;
    }
    *pValue = target;
    return 1;
}

/* 0200F564 */
s32 cLib_chaseS(be<s16>* pValue, s32 target, s32 step) {
    if (t60::dt() != 1.0f)
        return t60Chase(pValue, target, step, 1);
    s32 x = *pValue;
    if (step == 0)
        return x == target;
    if (x > target)
        step = (s16)(0u - (u32)step);
    x = (s16)((u32)x + (u32)step);
    s32 prod = (s32)((u32)((u32)x - (u32)target) * (u32)(s32)step);
    if (prod < 0) {
        *pValue = (s16)x;
        return 0;
    }
    *pValue = target;
    return 1;
}

/* 0200F5C8 */
s32 cLib_chaseF(be<f32>* pValue, f32 target, f32 step) {
    T60_STEP(step);

    f32 zero = (f32)zeroValueHD, x = *pValue;
    if (step == zero)
        return x == target;
    if (x > target)
        step = -step;
    f32 next = fadds_ppc(x, step), prod = fmuls_ppc(fsubs_ppc(next, target), step);
    if (prod < zero && !(std::fabs(fsubs_ppc(next, target)) <= t60ArriveSlack(step))) {
        *pValue = next;
        return 0;
    }
    *pValue = target;
    return 1;
}

/* 0200F62C */
s32 cLib_chasePos(cXyz* pValue, cXyz* target, f32 step) {
    T60_STEP(step);

    if (step == (f32)zeroValueHD)
        return cXyz_equalHD(pValue, target) != 0;
    cXyz delta;
    cXyz scaled;
    cXyz_subtractHD(pValue, &delta, target);
    f32 d = vectorLengthHD(&delta);
    if (std::fabs(d) < (f32)zeroThresholdHD || !(d > step + t60ArriveSlack(step))) {
        copyPositionWordsHD(pValue, target);
        return 1;
    }
    cXyz_scaleResultHD(&delta, &scaled, step / d);
    PSVECSubtract(pValue, &scaled, pValue);
    return 0;
}

/* 0200F764 */
s32 cLib_chasePosXZ(cXyz* pValue, cXyz* target, f32 step) {
    T60_STEP(step);

    cXyz delta;
    cXyz flat;
    cXyz scaled;
    cXyz_subtractHD(pValue, &delta, target);
    f32 x = delta.x, zero = (f32)zeroValueHD, z = delta.z;
    delta.y = zero;
    flat.y = zero;
    flat.x = x;
    flat.z = z;
    f32 d = vectorLengthHD(&flat), epsilon = (f32)zeroThresholdHD;
    if (step == zero)
        return std::fabs(d) < epsilon;
    if (std::fabs(d) < epsilon || !(d > step + t60ArriveSlack(step))) {
        copyPositionWordsHD(pValue, target);
        return 1;
    }
    cXyz_scaleResultHD(&delta, &scaled, step / d);
    PSVECSubtract(pValue, &scaled, pValue);
    return 0;
}

/* 0200F8D0 */
s32 cLib_chaseAngleS(be<s16>* pValue, s32 target, s32 step) {
    if (t60::dt() != 1.0f)
        return t60Chase(pValue, target, step, 2);
    s32 x = *pValue;
    if (step == 0)
        return x == target;
    if ((s16)((u32)x - (u32)target) > 0)
        step = (s16)(0u - (u32)step);
    x = (s16)((u32)x + (u32)step);
    s32 prod = (s32)((u32)(s32)(s16)((u32)x - (u32)target) * (u32)(s32)step);
    if (prod < 0) {
        *pValue = (s16)x;
        return 0;
    }
    *pValue = target;
    return 1;
}

/* 0200F93C */
s16 cLib_targetAngleY(cXyz* pValue, cXyz* target) {
    f32 x = pValue->x, tx = target->x, z = pValue->z;
    f32 dx = fsubs_ppc(tx, x), tz = target->z, dz = fsubs_ppc(tz, z);
    return cM_atan2s(dx, dz);
}

// HD: extra target-angle entrypoint shares the GameCube calculation.
/* 0200F958 */
s16 cLib_targetAngleY2HD(cXyz* pValue, cXyz* target) {
    f32 x = pValue->x, tx = target->x, z = pValue->z;
    f32 dx = fsubs_ppc(tx, x), tz = target->z, dz = fsubs_ppc(tz, z);
    return cM_atan2s(dx, dz);
}

/* 0200F974 */
s16 cLib_targetAngleX(cXyz* pValue, cXyz* target) {
    cXyz delta;
    cXyz flat;
    cXyz_subtractHD(target, &delta, pValue);
    f32 x = delta.x, zero = (f32)zeroValueHD;
    flat.x = x;
    f32 z = delta.z;
    flat.y = zero;
    flat.z = z;
    f32 d = vectorLengthHD(&flat);
    f32 y = delta.y;
    return cM_atan2s(y, d);
}

static void offsetPositionHD(cXyz* d, cXyz* base, u16 angle, cXyz* o) {
    const cLib_sinCosHD& sample = sinCosTable[angle >> 3];
    f32 oz = o->z, s = (f32)sample.sine, ox = o->x;
    f32 zs = fmuls_ppc(oz, s), c = (f32)sample.cosine, dx = fmadds(ox, c, zs), bx = base->x;
    d->x = fadds_ppc(bx, dx);
    f32 by = base->y, oy = o->y;
    d->y = fadds_ppc(by, oy);
    ox = o->x;
    oz = o->z;
    f32 xs = fmuls_ppc(ox, s), dz = fmsubs(oz, c, xs), bz = base->z;
    d->z = fadds_ppc(bz, dz);
}

/* 0200F9D4 */
void cLib_offsetPos(cXyz* d, cXyz* base, s16 angle, cXyz* o) {
    offsetPositionHD(d, base, (u16)angle, o);
}

// HD: extra position-offset entrypoint shares the GameCube calculation.
/* 0200FA40 */
void cLib_offsetPos2HD(cXyz* d, cXyz* base, s16 angle, cXyz* o) {
    offsetPositionHD(d, base, (u16)angle, o);
}

/* 0200FAAC */
s32 cLib_distanceAngleS(s16 a, s16 b) {
    s32 d = (s16)(a - b);
    return d < 0 ? -d : d;
}

// GameCube MtxInit resets the current matrix-stack pointer.
/* 0200FAC4 */
void MtxInit() { calc_mtx = &mtx[0]; }

// HD: concat retains its complete incoming register instead of narrowing to u8.
/* 0200FAD8 */
void MtxTrans(f32 x, f32 y, f32 z, u32 concat) {
    if (concat == 0) {
        MTXTrans(calc_mtx.get(), x, y, z);
    } else {
        Mtx34 temporary;
        MTXTrans(&temporary, x, y, z);
        Mtx34* current = calc_mtx.get();
        MTXConcat(current, &temporary, current);
    }
}
/* 0200FB3C */
void MtxRotX(f32 angle, u32 concat) {
    if (concat == 0) {
        MTXRotRad(calc_mtx.get(), 'X', angle);
    } else {
        Mtx34 temporary;
        MTXRotRad(&temporary, 'X', angle);
        Mtx34* current = calc_mtx.get();
        MTXConcat(current, &temporary, current);
    }
}
/* 0200FBA4 */
void MtxRotY(f32 angle, u32 concat) {
    if (concat == 0) {
        MTXRotRad(calc_mtx.get(), 'Y', angle);
    } else {
        Mtx34 temporary;
        MTXRotRad(&temporary, 'Y', angle);
        Mtx34* current = calc_mtx.get();
        MTXConcat(current, &temporary, current);
    }
}
/* 0200FC0C */
void MtxRotZ(f32 angle, u32 concat) {
    if (concat == 0) {
        MTXRotRad(calc_mtx.get(), 'Z', angle);
    } else {
        Mtx34 temporary;
        MTXRotRad(&temporary, 'Z', angle);
        Mtx34* current = calc_mtx.get();
        MTXConcat(current, &temporary, current);
    }
}
/* 0200FC74 */
void MtxScale(f32 x, f32 y, f32 z, u32 concat) {
    if (concat == 0) {
        MTXScale(calc_mtx.get(), x, y, z);
    } else {
        Mtx34 temporary;
        MTXScale(&temporary, x, y, z);
        Mtx34* current = calc_mtx.get();
        MTXConcat(current, &temporary, current);
    }
}
/* 0200FCD8 */
void MtxPosition(cXyz* src, cXyz* dest) { MTXMultVec(calc_mtx.get(), src, dest); }
/* 0200FCF0 */
void MtxPush() {
    Mtx34 temporary;
    MTXCopy(calc_mtx.get(), &temporary);
    Mtx34* next = calc_mtx.get() + 1;
    calc_mtx = next;
    MTXCopy(&temporary, next);
}
// HD: this version does not return the old matrix pointer.
/* 0200FD38 */
void MtxPull() { calc_mtx = calc_mtx.get() - 1; }
// HD: compiler-generated initialization of this translation unit.
/* 0200FD4C */
void __sinit_c_lib_cpp() {
    runtimeState.zero[2] = 0;
    runtimeState.zero[0] = 0;
    runtimeState.zero[3] = 0;
    runtimeState.zero[1] = 0;
    cLib_runtimeObjectInitHD(&runtimeObjects[0]);
    f32 a = (f32)runtimeConstants.lower, b = (f32)runtimeConstants.upper;
    runtimeState.lower = a;
    runtimeState.upper = b;
    cLib_runtimeFlagAInitHD(&runtimeState.flagA);
    cLib_runtimeObjectInitHD(&runtimeObjects[1]);
    cLib_runtimeFlagBInitHD(&runtimeState.flagB);
    cLib_runtimeObjectInitHD(&runtimeObjects[2]);
}

// clang-format off
/* WWHD glue
data zeroValueHD 10001EBC
data zeroThresholdHD 10001EC0
data calc_mtx 1018C7B0
data mtx 101FF5F0
data sinCosTable 104A44F8
data runtimeConstants 10001ED0
data runtimeState 101FF5D4
data runtimeObjects 1018C78C
ext C0009990 void cLib_memsetHD(void*,u32,u32)
ext 028E8DD0 f32 PSVECSquareMag(const cXyz*)
ext 028F4384 f32 cLib_sqrtfHD(f32)
ext 028E8DE8 f32 PSVECSquareDistance(const cXyz*,const cXyz*)
ext 0201AFD8 s32 cXyz_notEqualHD(const cXyz*,const cXyz*)
ext 0201AF98 s32 cXyz_equalHD(const cXyz*,const cXyz*)
ext 0201ADE0 void cXyz_subtractHD(const cXyz*,cXyz*,const cXyz*)
ext 028E8E64 void PSVECScale(const cXyz*,cXyz*,f32)
ext 028E8DAC void PSVECSubtract(const cXyz*,const cXyz*,cXyz*)
ext 0201AE48 void cXyz_scaleResultHD(const cXyz*,cXyz*,f32)
ext 0201B12C void cXyz_normalizeResultHD(const cXyz*,cXyz*)
ext 020195B0 s16 cM_atan2s(f32,f32)
ext 028E93CC void MTXTrans(Mtx34*,f32,f32,f32)
ext 028E9108 void MTXConcat(const Mtx34*,const Mtx34*,Mtx34*)
ext 028E98C0 void MTXRotRad(Mtx34*,u32,f32)
ext 028E945C void MTXScale(Mtx34*,f32,f32,f32)
ext 028E8F64 void MTXMultVec(const Mtx34*,const cXyz*,cXyz*)
ext 028E90D4 void MTXCopy(const Mtx34*,Mtx34*)
ext 028F026C void cLib_runtimeObjectInitHD(cLib_runtimeObjectHD*)
ext 028ED6F8 void cLib_runtimeFlagAInitHD(be<u8>*)
ext 028EAB2C void cLib_runtimeFlagBInitHD(be<u8>*)
*/
// clang-format on
