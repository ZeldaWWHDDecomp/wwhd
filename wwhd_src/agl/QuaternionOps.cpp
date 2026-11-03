// WWHD quaternion leaves. Arithmetic follows the original operation and operand order.
#include "agl/QuaternionOps.h"

static f32 sQuaternionZero;
static f32 sQuaternionOne;
void* allocateAglStorage(u32 size);

/* 027AAF00 */
Quaternion* createQuaternionStorage(Quaternion* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = (Quaternion*)allocateAglStorage(sizeof(Quaternion));
    return object;
}

/* 027AAF2C */
Quaternion* createIdentityQuaternion(Quaternion* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = (Quaternion*)allocateAglStorage(sizeof(Quaternion));
    if (object != NULL) {
        f32 zero = sQuaternionZero;
        object->y = zero;
        object->x = zero;
        object->z = zero;
        object->w = sQuaternionOne;
    }
    return object;
}

/* 027AAF80 */
static void multiplyQuaternion(Quaternion* output, const Quaternion* left, const Quaternion* right) {
    f32 ax = left->x;
    f32 bw = right->w;
    f32 aw = left->w;
    f32 xw = gabi::fmuls_ppc(ax, bw);
    f32 bx = right->x;
    f32 by = right->y;
    f32 xx = gabi::fmuls_ppc(ax, bx);
    f32 x = gabi::fmadds(aw, bx, xw);
    f32 bz = right->z;
    f32 xy = gabi::fmuls_ppc(ax, by);
    f32 xz = gabi::fmuls_ppc(ax, bz);
    f32 w = gabi::fmsubs(aw, bw, xx);
    f32 z = gabi::fmadds(aw, bz, xy);
    f32 ay = left->y;
    f32 y = gabi::fmsubs(aw, by, xz);
    w = gabi::fnmsubs(ay, by, w);
    x = gabi::fmadds(ay, bz, x);
    f32 az = left->z;
    y = gabi::fmadds(ay, bw, y);
    w = gabi::fnmsubs(az, bz, w);
    x = gabi::fnmsubs(az, by, x);
    z = gabi::fnmsubs(ay, bx, z);
    output->w = w;
    y = gabi::fmadds(az, bx, y);
    output->x = x;
    z = gabi::fmadds(az, bw, z);
    output->y = y;
    output->z = z;
}
/* WWHD glue
 data sQuaternionZero 10145180
 data sQuaternionOne 1014517C
 ext 0273AD10 void* allocateAglStorage(u32)
*/
