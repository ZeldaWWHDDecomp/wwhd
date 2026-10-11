#include "d/hd_gamepad_map.h"
using namespace gabi;
static f32 fdivs(f32 a, f32 b) { return f32(f64(a) / f64(b)); }
static f32 l_hdMapDisplayZero;
static f32 l_hdMapDisplayHalf;
static f32 l_hdMapDisplayThree;

static f32 distance(f32 squared) {
    f32 zero = l_hdMapDisplayZero;
    if (!(squared > zero))
        return fmuls_ppc(zero, squared);
    f64 inv = frsqrte(squared);
    f32 a = f32(inv * round25(inv)), half = f32(inv * f64(l_hdMapDisplayHalf));
    return fmuls_ppc(fmuls_ppc(fnmsubs(a, squared, l_hdMapDisplayThree), half), squared);
}
/* 02689BA0 */
f32 hdMapDisplayChase(HdPadPoint *current, HdPadPoint *target, f32 rate, f32 maximum, f32 minimum) {
    f32 x = current->x, tx = target->x, y, ty;
    bool same = x == tx;
    if (same) {
        y = current->y;
        ty = target->y;
        same = y == ty;
    }
    if (!same) {
        x = current->x;
        tx = target->x;
        y = current->y;
        ty = target->y;
        f32 dx = fsubs_ppc(x, tx), dy = fsubs_ppc(y, ty);
        f32 length = distance(fmadds(dx, dx, fmuls_ppc(dy, dy)));
        bool snap = length < minimum;
        if (!snap) {
            f32 step = fmuls_ppc(length, rate), sx = fmuls_ppc(dx, rate), sy = fmuls_ppc(dy, rate);
            if (step == l_hdMapDisplayZero)
                snap = true;
            else {
                if (step > maximum) {
                    f32 scale = fdivs(maximum, step);
                    sy = fmuls_ppc(sy, scale);
                    sx = fmuls_ppc(sx, scale);
                } else if (step < minimum) {
                    f32 scale = fdivs(minimum, step);
                    sy = fmuls_ppc(sy, scale);
                    sx = fmuls_ppc(sx, scale);
                }
                x = fsubs_ppc(current->x, sx);
                y = fsubs_ppc(current->y, sy);
                current->x = x;
                current->y = y;
                ty = target->y;
                tx = target->x;
            }
        }
        if (snap) {
            x = target->x;
            current->x = x;
            y = target->y;
            current->y = y;
            tx = target->x;
            ty = target->y;
        }
    }
    f32 dx = fsubs_ppc(x, tx), dy = fsubs_ppc(y, ty);
    return distance(fmadds(dx, dx, fmuls_ppc(dy, dy)));
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdMapDisplayZero 100F997C
data l_hdMapDisplayHalf 100F9A2C
data l_hdMapDisplayThree 100F9A48
*/
