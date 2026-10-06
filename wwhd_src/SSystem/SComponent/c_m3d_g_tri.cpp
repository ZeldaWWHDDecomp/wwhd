/* WWHD triangle geometry, raw verified implementation. */
#include "gabi.h"
using namespace gabi;
namespace tri_cpp {
struct VecLocal { be<f32> v[3]; };
static void init(u32 t) {
    call<void>(0x020189A0, at<void>(t));
    store<u32>(t+0x10, 0x10003574);
    call<void>(0x028F521C, at<void>(t+0x14), 12u);
    call<void>(0x028F521C, at<void>(t+0x20), 12u);
    call<void>(0x028F521C, at<void>(t+0x2C), 12u);
}
static void copy(u32 dst, u32 src) {
    store<u32>(dst, load<u32>(src));
    store<u32>(dst+4, load<u32>(src+4));
    store<u32>(dst+8, load<u32>(src+8));
}
static void plane(u32 t) {
    call<void>(0x02010D18, at<void>(t+0x14), at<void>(t+0x20), at<void>(t+0x2C), at<void>(t), at<void>(t+12));
}
void* tri_ctor(void* self) {
    WWHD_FUNC(0x02019040, void*, self);
    u32 t = ea(self);
    if (!t) t = call<u32>(0x0273AD10, 0x38u);
    if (t) init(t);
    return at<void>(t);
}
VERIFY(0x02019040, tri_ctor);
void* tri_ctor_pos(void* self, void* a, void* b, void* c) {
    WWHD_FUNC(0x020190B8, void*, self, a, b, c);
    u32 t = ea(self);
    if (!t) t = call<u32>(0x0273AD10, 0x38u);
    if (t) {
        init(t);
        copy(t+0x14, ea(a));
        copy(t+0x20, ea(b));
        copy(t+0x2C, ea(c));
        plane(t);
    }
    return at<void>(t);
}
VERIFY(0x020190B8, tri_ctor_pos);
void tri_up(void* self, f32 distance) {
    WWHD_FUNC(0x020191B4, void, self, distance);
    Local<VecLocal> displacement;
    call<void>(0x028E8E64, self, displacement.get(), distance);
    u32 t = ea(self);
    call<void>(0x028E8D88, at<void>(t+0x14), displacement.get(), at<void>(t+0x14));
    call<void>(0x028E8D88, at<void>(t+0x20), displacement.get(), at<void>(t+0x20));
    call<void>(0x028E8D88, at<void>(t+0x2C), displacement.get(), at<void>(t+0x2C));
    store<f32>(t+12, load<f32>(t+12)-distance);
}
VERIFY(0x020191B4, tri_up);
u32 tri_cross(void* self, void* other, void* out) {
    WWHD_FUNC(0x0201923C, u32, self, other, out);
    return call<u32>(0x020156D0, other, self, out);
}
VERIFY(0x0201923C, tri_cross);
void tri_set_pos(void* self, void* a, void* b, void* c) {
    WWHD_FUNC(0x0201924C, void, self, a, b, c);
    u32 t = ea(self);
    copy(t+0x14, ea(a));
    copy(t+0x20, ea(b));
    copy(t+0x2C, ea(c));
    plane(t);
}
VERIFY(0x0201924C, tri_set_pos);
void tri_set_normal(void* self, void* a, void* b, void* c, void* normal) {
    WWHD_FUNC(0x020192AC, void, self, a, b, c, normal);
    u32 t = ea(self);
    for (u32 i=0; i<12; i+=4) store<u32>(t+0x14+i, load<u32>(ea(a)+i));
    for (u32 i=0; i<12; i+=4) store<u32>(t+0x20+i, load<u32>(ea(b)+i));
    for (u32 i=0; i<12; i+=4) store<u32>(t+0x2C+i, load<u32>(ea(c)+i));
    call<void>(0x02018B84, self, normal);
}
VERIFY(0x020192AC, tri_set_normal);
void tri_sinit() {
    WWHD_FUNC(0x020192FC, void);
    store<u32>(0x101FF994, 0);
    store<u32>(0x101FF98C, 0);
    store<u32>(0x101FF998, 0);
    store<u32>(0x101FF990, 0);
    call<void>(0x028F026C, at<void>(0x1018C9D0));
    f32 a = load<f32>(0x1000358C);
    f32 b = load<f32>(0x10003590);
    store<f32>(0x101FF980, a);
    store<f32>(0x101FF984, b);
    call<void>(0x028ED6F8, at<void>(0x101FF988));
    call<void>(0x028F026C, at<void>(0x1018C9DC));
    call<void>(0x028EAB2C, at<void>(0x101FF989));
    call<void>(0x028F026C, at<void>(0x1018C9E8));
}
VERIFY(0x020192FC, tri_sinit);
}

/* ---- hosted here: the static initializer(s) of a separate header-static-only TU linked between c_m3d_g_tri and c_malloc (name unknown).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_02019390() {
 WWHD_FUNC(0x02019390,void);
 gabi::store<u32>(0x101FF9B0,0);gabi::store<u32>(0x101FF9A8,0);gabi::store<u32>(0x101FF9B4,0);gabi::store<u32>(0x101FF9AC,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C9F4));
 f32 negativePi=gabi::load<f32>(0x100035A4),positivePi=gabi::load<f32>(0x100035A8);
 gabi::store<f32>(0x101FF99C,negativePi);gabi::store<f32>(0x101FF9A0,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF9A4));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018CA00));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF9A5));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018CA0C));
}
VERIFY(0x02019390,hd_static_init_02019390);
