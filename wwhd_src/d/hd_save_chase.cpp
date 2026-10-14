/** HD save helpers; local decomp */
#include "d/hd_save_chase.h"
static u32 sSaveChaseHeaderStatics;
static u32 sSaveChaseHeaderDescriptors;
static u32 sSaveChaseHeaderZero;
static u32 sSaveChasePiConstants;

/* 0271F634 */
static BOOL chaseSaveValue(be<s32>* value, s32 target, s32 step) {
    s32 current = *value;
    s32 delta = s32(u32(target) - u32(current));
    if (delta > step) { *value = s32(u32(current) + u32(step)); return FALSE; }
    if (delta < s32(0u - u32(step))) { *value = s32(u32(current) - u32(step)); return FALSE; }
    *value = target;
    return TRUE;
}

/* 0271F680 */
static BOOL chaseSaveValueScaled(be<s32>* value, s32 target, u32 distance, s32 maximum, s32 minimum, f32 rate) {
    s32 step = gabi::ftoi(f32(distance) * rate);
    if (step < minimum) step = minimum;
    else if (step > maximum) step = maximum;
    return chaseSaveValue(value, target, step);
}

/* 0271F6F4 */
static BOOL chaseSaveValueRate(be<s32>* value, be<s32>* target, be<s32>* maximum, be<s32>* minimum, f32 rate) {
    s32 goal = *target;
    s32 delta = s32(u32(goal) - u32(s32(*value)));
    u32 distance = delta < 0 ? 0u - u32(delta) : u32(delta);
    return chaseSaveValueScaled(value, goal, distance, *maximum, *minimum, rate);
}

/* 0271F738 */
static BOOL chaseSaveValueRef(be<s32>* value, be<s32>* target, be<s32>* step) {
    return chaseSaveValue(value, *target, *step);
}

/* 0271F744 */
static void __sinit_hd_save_chase_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSaveChaseHeaderStatics), gabi::ea(&sSaveChaseHeaderDescriptors), gabi::ea(&sSaveChaseHeaderZero), gabi::ea(&sSaveChasePiConstants));
}

/* WWHD glue
data sSaveChaseHeaderStatics 1049F6DC nocheck
data sSaveChaseHeaderDescriptors 101F8404 nocheck
data sSaveChaseHeaderZero 1049F6E8 nocheck
data sSaveChasePiConstants 10140720 nocheck
*/
