/* Component interpolation, chase helpers and matrix stack. GameCube names and verified HD
 * call order, register widths and NaN behavior. */
#include "SSystem/SComponent/c_lib.h"
#include <cmath>
using namespace gabi;

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

// HD: integer parameters stay full register width; HD truncates only explicit halfword operations.
// HD: PPC signed division by zero/overflow returns a sign-derived sentinel.
static s32 divideSignedPPC(s32 a, s32 b) {
    return !b || (a == INT32_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b;
}

/* 0200F378 */
s16 cLib_addCalcAngleS(be<s16>* pValue, s32 target, s32 scale, s32 maxStep, s32 minStep) {

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
    s32 x = *pValue, step = (s16)divideSignedPPC((s16)((u32)target - (u32)x), scale);
    if (step > maxStep)
        step = maxStep;
    else if (step < (s32)(0u - (u32)maxStep))
        step = (s32)(0u - (u32)maxStep);
    *pValue = (s16)((u32)x + (u32)step);
}

/* 0200F474 */
s32 cLib_addCalcAngleL(be<s32>* pValue, s32 target, s32 scale, s32 maxStep, s32 minStep) {

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
    f32 zero = (f32)zeroValueHD, x = *pValue;
    if (step == zero)
        return x == target;
    if (x > target)
        step = -step;
    f32 next = fadds_ppc(x, step), prod = fmuls_ppc(fsubs_ppc(next, target), step);
    if (prod < zero) {
        *pValue = next;
        return 0;
    }
    *pValue = target;
    return 1;
}

/* 0200F62C */
s32 cLib_chasePos(cXyz* pValue, cXyz* target, f32 step) {
    if (step == (f32)zeroValueHD)
        return cXyz_equalHD(pValue, target) != 0;
    cXyz delta;
    cXyz scaled;
    cXyz_subtractHD(pValue, &delta, target);
    f32 d = vectorLengthHD(&delta);
    if (std::fabs(d) < (f32)zeroThresholdHD || !(d > step)) {
        copyPositionWordsHD(pValue, target);
        return 1;
    }
    cXyz_scaleResultHD(&delta, &scaled, step / d);
    PSVECSubtract(pValue, &scaled, pValue);
    return 0;
}

/* 0200F764 */
s32 cLib_chasePosXZ(cXyz* pValue, cXyz* target, f32 step) {
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
    if (std::fabs(d) < epsilon || !(d > step)) {
        copyPositionWordsHD(pValue, target);
        return 1;
    }
    cXyz_scaleResultHD(&delta, &scaled, step / d);
    PSVECSubtract(pValue, &scaled, pValue);
    return 0;
}

/* 0200F8D0 */
s32 cLib_chaseAngleS(be<s16>* pValue, s32 target, s32 step) {
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
