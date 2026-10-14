/* HD two-dimensional cursor chase, companion to the verified swipe detector. */
#include "d/hd_input_math.h"
static f32 sCursorZero;
static f32 sCursorHalf;
static f32 sCursorThree;
static f32 sCursorOne;
/* 0261AB28 */
u32 hdCursorChase(HdCursorPoint* cursor, HdCursorPoint* target, f32 step) {
    const f32 dy = gabi::fsubs_ppc(target->y, cursor->y);
    const f32 dx = gabi::fsubs_ppc(target->x, cursor->x);
    const f32 squared = gabi::fmadds(dx, dx, gabi::fmuls_ppc(dy, dy));
    f32 inverseLength = sCursorZero;
    if (!(squared <= inverseLength)) {
        const f64 estimate = gabi::frsqrte(squared);
        const f32 correction = gabi::fnmsubs(f32(estimate * round25(estimate)), squared, sCursorThree);
        inverseLength = gabi::fmuls_ppc(correction, f32(estimate * round25(sCursorHalf)));
    }
    const f32 length = gabi::fmuls_ppc(inverseLength, squared);
    if (!(length > step)) {
        reinterpret_cast<be<u32>*>(&cursor->x)[0] = reinterpret_cast<be<u32>*>(&target->x)[0];
        reinterpret_cast<be<u32>*>(&cursor->y)[0] = reinterpret_cast<be<u32>*>(&target->y)[0];
        return 1;
    }
    const f32 reciprocal = sCursorOne / length;
    const f32 nextX = gabi::fmadds(step, gabi::fmuls_ppc(dx, reciprocal), cursor->x);
    const f32 nextY = gabi::fmadds(step, gabi::fmuls_ppc(dy, reciprocal), cursor->y);
    cursor->x = nextX;
    cursor->y = nextY;
    return 0;
}
/* WWHD glue: cursor constants from its own TU.
data sCursorZero 100E2B08
data sCursorHalf 100E2B0C
data sCursorThree 100E2B10
data sCursorOne 100E2B14
*/
