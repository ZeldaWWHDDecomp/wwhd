#include "gabi.h"
#include <cmath>
using namespace gabi;

void* box_ctor(void* self) {
    WWHD_FUNC(0x02010594, void*, self);
    if (!self) self = call<void*>(0x0273AD10, 20);
    if (self) {
        store<u32>(ea(self)+16, 0x10001F50);
        call<void>(0x028F521C, self, 8);
        call<void>(0x028F521C, at<void>(ea(self)+8), 8);
    }
    return self;
}
VERIFY(0x02010594, box_ctor);

void box_dtor(void* self, s32 flags) {
    WWHD_FUNC(0x020105F8, void, self, flags);
    if (self && (flags & 1)) call<void>(0x0273AF40, self);
}
VERIFY(0x020105F8, box_dtor);

void box_set(void* self, void* low, void* high) {
    WWHD_FUNC(0x0201060C, void, self, low, high);
    store<u32>(ea(self), load<u32>(ea(low)));
    store<u32>(ea(self)+4, load<u32>(ea(low)+4));
    store<u32>(ea(self)+8, load<u32>(ea(high)));
    store<u32>(ea(self)+12, load<u32>(ea(high)+4));
}
VERIFY(0x0201060C, box_set);

f32 box_get_len(void* self, void* point) {
    WWHD_FUNC(0x02010630, f32, self, point);
    f32 x = load<f32>(ea(point));
    f32 x0 = load<f32>(ea(self));
    if (x0 < x && x < load<f32>(ea(self)+8)) {
        f32 y0 = load<f32>(ea(self)+4);
        f32 y = load<f32>(ea(point)+4);
        f32 y1 = load<f32>(ea(self)+12);
        if (y0 < y && y < y1) return load<f32>(0x10001F3C);
        f32 a = std::fabs(y0-y);
        f32 b = std::fabs(y1-y);
        return a > b ? b : a;
    }
    f32 y0 = load<f32>(ea(self)+4);
    f32 y = load<f32>(ea(point)+4);
    if (y0 < y && y < load<f32>(ea(self)+12)) {
        f32 x1 = load<f32>(ea(self)+8);
        f32 a = std::fabs(x0-x);
        f32 b = std::fabs(x1-x);
        return a > b ? b : a;
    }
    f32 cx = x < x0 ? x0 : load<f32>(ea(self)+8);
    f32 cy = y < y0 ? y0 : load<f32>(ea(self)+12);
    f32 squared = call<f32>(0x020109E8, x, y, cx, cy);
    return call<f32>(0x028F4384, squared);
}
VERIFY(0x02010630, box_get_len);

void box_sinit() {
    WWHD_FUNC(0x020107A8, void);
    store<u32>(0x101FF860, 0);
    store<u32>(0x101FF858, 0);
    store<u32>(0x101FF864, 0);
    store<u32>(0x101FF85C, 0);
    call<void>(0x028F026C, at<void>(0x1018C844));
    f32 a = load<f32>(0x10001F44);
    f32 b = load<f32>(0x10001F48);
    store<f32>(0x101FF84C, a);
    store<f32>(0x101FF850, b);
    call<void>(0x028ED6F8, at<void>(0x101FF854));
    call<void>(0x028F026C, at<void>(0x1018C850));
    call<void>(0x028EAB2C, at<void>(0x101FF855));
    call<void>(0x028F026C, at<void>(0x1018C85C));
}
VERIFY(0x020107A8, box_sinit);
/* ---- hosted here: the separate TU linked between c_m2d_g_box and
 * c_m3d (probably c_m2d_g_cir by link order): cM2dGCir::cM2dGCir() (GHS form, allocates when
 * this==0; 0x10 bytes, vtable at 0xC) and its static initializer. ---- */
u32 hd_cM2dGCir_ctor(u32 self) {
 WWHD_FUNC(0x0201083C,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,0x10u);if(!self)return 0;}
 f32 zero=gabi::load<f32>(0x10001F74);
 gabi::store<u32>(self+0xC,0x10001F64);
 gabi::store<f32>(self+0,zero);gabi::store<f32>(self+8,zero);gabi::store<f32>(self+4,zero);
 return self;
}
VERIFY(0x0201083C,hd_cM2dGCir_ctor);
void hd_static_init_02010890() {
 WWHD_FUNC(0x02010890,void);
 gabi::store<u32>(0x101FF87C,0);gabi::store<u32>(0x101FF874,0);gabi::store<u32>(0x101FF880,0);gabi::store<u32>(0x101FF878,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C868));
 f32 negativePi=gabi::load<f32>(0x10001F7C),positivePi=gabi::load<f32>(0x10001F80);
 gabi::store<f32>(0x101FF868,negativePi);gabi::store<f32>(0x101FF86C,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF870));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C874));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF871));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C880));
}
VERIFY(0x02010890,hd_static_init_02010890);
