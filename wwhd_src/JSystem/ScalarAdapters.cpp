// Shared HD scalar helper; behavior reconstructed from local instructions.
#include "bindings.h"

/* 027382E8 */
static f64 clampNativeRange(f64 value,f64 minimum,f64 maximum) {
    // fmr preserves the selected FPR, including its payload and signed zero.
    if (!(value >= minimum)) return minimum;
    if (value <= maximum) return value;
    return maximum;
}

/* WWHD glue
*/
