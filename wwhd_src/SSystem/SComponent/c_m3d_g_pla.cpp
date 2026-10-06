#include "gabi.h"
#include <cmath>
using namespace gabi;

void* pla_ctor(void* self) {
    WWHD_FUNC(0x020189A0, void*, self);
    if (!self) self = call<void*>(0x0273AD10, 20);
    if (self) {
        f32 d = load<f32>(0x100033AC);
        store<u32>(ea(self)+16, 0x1000339C);
        store<f32>(ea(self)+12, d);
    }
    return self;
}
VERIFY(0x020189A0, pla_ctor);

void* pla_ctor_nd(void* self, void* normal, f32 d) {
    WWHD_FUNC(0x020189EC, void*, self, normal, d);
    if (!self) self = call<void*>(0x0273AD10, 20);
    if (self) {
        f32 initial = load<f32>(0x100033AC);
        store<u32>(ea(self)+16, 0x1000339C);
        store<f32>(ea(self)+12, initial);
        store<u32>(ea(self), load<u32>(ea(normal)));
        store<u32>(ea(self)+4, load<u32>(ea(normal)+4));
        u32 z = load<u32>(ea(normal)+8);
        store<f32>(ea(self)+12, d);
        store<u32>(ea(self)+8, z);
    }
    return self;
}
VERIFY(0x020189EC, pla_ctor_nd);

void pla_setup_np0(void* self, void* normal, void* point) {
    WWHD_FUNC(0x02018A7C, void, self, normal, point);
    store<f32>(ea(self), load<f32>(ea(normal)));
    store<f32>(ea(self)+4, load<f32>(ea(normal)+4));
    store<f32>(ea(self)+8, load<f32>(ea(normal)+8));
    call<void>(0x028E8EF0, self, self);
    f32 d = call<f32>(0x028E8F44, self, point);
    store<f32>(ea(self)+12, -d);
}
VERIFY(0x02018A7C, pla_setup_np0);

u32 pla_cross_y(void* self, void* point, void* out) {
    WWHD_FUNC(0x02018AE4, u32, self, point, out);
    f32 y = load<f32>(ea(self)+4);
    if (std::fabs(y) < load<f32>(0x100030B8)) return 0;
    f32 nx = load<f32>(ea(self));
    f32 px = load<f32>(ea(point));
    f32 product = nx * px;
    f32 nz = load<f32>(ea(self)+8);
    f32 pz = load<f32>(ea(point)+8);
    f32 numerator = fnmsubs(nz, pz, -product);
    f32 d = load<f32>(ea(self)+12);
    store<f32>(ea(out), (numerator-d)/y);
    return 1;
}
VERIFY(0x02018AE4, pla_cross_y);

u32 pla_cross_y_no_d(void* self, void* point, void* out) {
    WWHD_FUNC(0x02018B38, u32, self, point, out);
    f32 y = load<f32>(ea(self)+4);
    if (std::fabs(y) < load<f32>(0x100030B8)) return 0;
    f32 px = load<f32>(ea(point));
    f32 nx = load<f32>(ea(self));
    f32 product = nx * px;
    f32 nz = load<f32>(ea(self)+8);
    f32 pz = load<f32>(ea(point)+8);
    store<f32>(ea(out), fnmsubs(nz, pz, -product)/y);
    return 1;
}
VERIFY(0x02018B38, pla_cross_y_no_d);

void pla_set(void* self, void* other) {
    WWHD_FUNC(0x02018B84, void, self, other);
    store<u32>(ea(self), load<u32>(ea(other)));
    store<u32>(ea(self)+4, load<u32>(ea(other)+4));
    store<u32>(ea(self)+8, load<u32>(ea(other)+8));
    store<f32>(ea(self)+12, load<f32>(ea(other)+12));
}
VERIFY(0x02018B84, pla_set);

s16 pla_calc_angle_xz(void* self, void* x, void* z) {
    WWHD_FUNC(0x02018BA8, s16, self, x, z);
    return call<s16>(0x02017264, self, x, z);
}
VERIFY(0x02018BA8, pla_calc_angle_xz);

void pla_sinit() {
    WWHD_FUNC(0x02018BAC, void);
    store<u32>(0x101FF95C, 0);
    store<u32>(0x101FF954, 0);
    store<u32>(0x101FF960, 0);
    store<u32>(0x101FF958, 0);
    call<void>(0x028F026C, at<void>(0x1018C988));
    f32 a = load<f32>(0x100033B4);
    f32 b = load<f32>(0x100033B8);
    store<f32>(0x101FF948, a);
    store<f32>(0x101FF94C, b);
    call<void>(0x028ED6F8, at<void>(0x101FF950));
    call<void>(0x028F026C, at<void>(0x1018C994));
    call<void>(0x028EAB2C, at<void>(0x101FF951));
    call<void>(0x028F026C, at<void>(0x1018C9A0));
}
VERIFY(0x02018BAC, pla_sinit);
