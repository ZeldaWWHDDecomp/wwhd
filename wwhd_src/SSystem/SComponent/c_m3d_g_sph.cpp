#include "gabi.h"
#include <cmath>
using namespace gabi;
struct SphVectorLocal { be<f32> v[3]; };

void* sph_ctor(void* self) {
    WWHD_FUNC(0x02018C40, void*, self);
    if (!self) self = call<void*>(0x0273AD10, 20);
    if (self) {
        f32 radius = load<f32>(0x100033D4);
        store<u32>(ea(self)+16, 0x100033C4);
        store<f32>(ea(self)+12, radius);
    }
    return self;
}
VERIFY(0x02018C40, sph_ctor);

void sph_set_r(void* self, f32 radius) {
    WWHD_FUNC(0x02018C8C, void, self, radius);
    if (std::isnan(radius)) call<void>(0x0273AA24, at<void>(0x100033E0), 66, at<void>(0x1000340C));
    if (!(load<f32>(0x100033D8) < radius && radius < load<f32>(0x100033DC)))
        call<void>(0x0273AA24, at<void>(0x100033E0), 67, at<void>(0x100033F0));
    store<f32>(ea(self)+12, radius);
}
VERIFY(0x02018C8C, sph_set_r);

void sph_set_c(void* self, void* center) {
    WWHD_FUNC(0x02018D40, void, self, center);
    u32 p = ea(center);
    if (std::isnan(load<f32>(p))) call<void>(0x0273AA24, at<void>(0x10003440), 31, at<void>(0x10003450));
    if (std::isnan(load<f32>(p+4))) call<void>(0x0273AA24, at<void>(0x10003440), 32, at<void>(0x10003488));
    if (std::isnan(load<f32>(p+8))) call<void>(0x0273AA24, at<void>(0x10003440), 33, at<void>(0x100034C0));
    f32 x = load<f32>(p);
    f32 lower = load<f32>(0x100033D8);
    f32 y, z;
    bool valid = false;
    if (lower < x) {
        f32 upper = load<f32>(0x100033DC);
        if (x < upper) {
            y = load<f32>(p+4);
            if (lower < y && y < upper) {
                z = load<f32>(p+8);
                valid = lower < z && z < upper;
            }
        }
    }
    if (!valid) {
        call<void>(0x0273AA24, at<void>(0x10003440), 34, at<void>(0x100034F8));
        y = load<f32>(p+4);
        z = load<f32>(p+8);
        x = load<f32>(p);
    }
    store<f32>(ea(self)+4, y);
    store<f32>(ea(self)+8, z);
    store<f32>(ea(self), x);
}
VERIFY(0x02018D40, sph_set_c);

void sph_set(void* self, void* center, f32 radius) {
    WWHD_FUNC(0x02018E88, void, self, center, radius);
    sph_set_c(self, center);
    sph_set_r(self, radius);
}
VERIFY(0x02018E88, sph_set);

void sph_set_src(void* self, void* source) {
    WWHD_FUNC(0x02018EDC, void, self, source);
    Local<SphVectorLocal> center;
    f32 x = load<f32>(ea(source));
    f32 y = load<f32>(ea(source)+4);
    f32 z = load<f32>(ea(source)+8);
    center.get()->v[0] = x;
    center.get()->v[1] = y;
    center.get()->v[2] = z;
    sph_set_c(self, center.get());
    f32 radius = load<f32>(ea(source)+12);
    sph_set_r(self, radius);
}
VERIFY(0x02018EDC, sph_set_src);

void sph_set_xyz(void* self, f32 x, f32 y, f32 z) {
    WWHD_FUNC(0x02018F3C, void, self, x, y, z);
    Local<SphVectorLocal> center;
    center.get()->v[2] = z;
    center.get()->v[1] = y;
    center.get()->v[0] = x;
    sph_set_c(self, center.get());
}
VERIFY(0x02018F3C, sph_set_xyz);

u32 sph_cross_cyl(void* self, void* other, void* out) {
    WWHD_FUNC(0x02018F6C, u32, self, other, out);
    Local<be<f32>> distance;
    return call<u32>(0x02013854, other, self, out, distance.get());
}
VERIFY(0x02018F6C, sph_cross_cyl);

u32 sph_cross_sph(void* self, void* other, void* out) {
    WWHD_FUNC(0x02018F9C, u32, self, other, out);
    return call<u32>(0x02014010, other, self, out);
}
VERIFY(0x02018F9C, sph_cross_sph);

void sph_sinit() {
    WWHD_FUNC(0x02018FAC, void);
    store<u32>(0x101FF978, 0);
    store<u32>(0x101FF970, 0);
    store<u32>(0x101FF97C, 0);
    store<u32>(0x101FF974, 0);
    call<void>(0x028F026C, at<void>(0x1018C9AC));
    f32 a = load<f32>(0x10003564);
    f32 b = load<f32>(0x10003568);
    store<f32>(0x101FF964, a);
    store<f32>(0x101FF968, b);
    call<void>(0x028ED6F8, at<void>(0x101FF96C));
    call<void>(0x028F026C, at<void>(0x1018C9B8));
    call<void>(0x028EAB2C, at<void>(0x101FF96D));
    call<void>(0x028F026C, at<void>(0x1018C9C4));
}
VERIFY(0x02018FAC, sph_sinit);
