// Preserve the local PPC length estimate, refinement and conditional stores.
#include "agl/EnvironmentVectorMath.h"
using namespace agl::env;
static f32 sVectorZero;
static f32 sVectorHalf;
static f32 sVectorThree;
static f32 sVectorOne;

/* 0261AAA0 */
f32 agl::env::normalizeEnvironmentVector(sead::RenderVector2* vector) {
    f32 y = vector->y;
    f32 x = vector->x;
    f32 squared = gabi::fmadds(x,x,gabi::fmuls_ppc(y,y));
    f32 zero = sVectorZero;
    f32 length;
    if (squared > zero) {
        f32 three = sVectorThree;
        f32 half = sVectorHalf;
        f64 estimate = gabi::frsqrte(squared);
        f32 squareInverse = f32(estimate * round25(estimate));
        f32 halfInverse = f32(estimate * half);
        f32 correction = gabi::fnmsubs(squareInverse,squared,three);
        f32 inverse = gabi::fmuls_ppc(correction,halfInverse);
        length = gabi::fmuls_ppc(inverse,squared);
    } else {
        length = gabi::fmuls_ppc(zero,squared);
    }
    // The return branch tests the greater-than bit being clear, including unordered NaNs.
    if (!(length > zero)) return length;
    f32 one = sVectorOne;
    y = vector->y;
    f32 inverseLength = gabi::fdivs_ppc(one,length);
    x = vector->x;
    f32 normalizedX = gabi::fmuls_ppc(x,inverseLength);
    f32 normalizedY = gabi::fmuls_ppc(y,inverseLength);
    vector->x = normalizedX;
    vector->y = normalizedY;
    return length;
}
/* WWHD glue
 data sVectorZero 100E2B08
 data sVectorHalf 100E2B0C
 data sVectorThree 100E2B10
 data sVectorOne 100E2B14
*/
