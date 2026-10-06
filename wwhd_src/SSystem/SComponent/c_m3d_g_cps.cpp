/* Local WWHD capsule geometry reconstruction. */
#include "SSystem/SComponent/c_m3d_g_cps.h"
cM3dGCps* cps_ctor(cM3dGCps* self) {
  WWHD_FUNC(0x02018150, cM3dGCps*, self);
  if (!self) self = gabi::call<cM3dGCps*>(0x0273AD10, 0x20u);
  if (self) {
    f32 radius = gabi::load<f32>(0x10003124);
    self->vtable = 0x10003138;
    self->radius = radius;
  }
  return self;
}
VERIFY(0x02018150, cps_ctor);
void cps_dtor(cM3dGCps* self, u32 flags) {
  WWHD_FUNC(0x0201819C, void, self, flags);
  if (self && (flags & 1)) gabi::call(0x0273AF40, self);
}
VERIFY(0x0201819C, cps_dtor);
void cps_set(cM3dGCps* self, cXyz* start, cXyz* end, f32 radius) {
  WWHD_FUNC(0x020181B0, void, self, start, end, radius);
  gabi::call(0x02018808, self, start, end);
  self->radius = radius;
}
VERIFY(0x020181B0, cps_set);
void cps_set_shape(cM3dGCps* self, cM3dGCpsS* shape) {
  WWHD_FUNC(0x020181FC, void, self, shape);
  gabi::call(0x0201883C, self, &shape->start, &shape->end);
  f32 radius = shape->radius;
  self->radius = radius;
}
VERIFY(0x020181FC, cps_set_shape);
void cps_copy(cM3dGCps* self, cM3dGCps* source) {
  WWHD_FUNC(0x02018240, void, self, source);
  f32 radius = source->radius;
  cps_set(self, &source->start, &source->end, radius);
}
VERIFY(0x02018240, cps_copy);
void cps_initializer() {
  WWHD_FUNC(0x0201824C, void);
  for (u32 off : {8u, 0u, 12u, 4u}) gabi::store<u32>(0x101FF900 + off, 0);
  gabi::call(0x028F026C, gabi::at<void>(0x1018C91C));
  f32 a = gabi::load<f32>(0x1000312C), b = gabi::load<f32>(0x10003130);
  gabi::store<f32>(0x101FF8F4, a);
  gabi::store<f32>(0x101FF8F8, b);
  gabi::call(0x028ED6F8, gabi::at<void>(0x101FF8FC));
  gabi::call(0x028F026C, gabi::at<void>(0x1018C928));
  gabi::call(0x028EAB2C, gabi::at<void>(0x101FF8FD));
  gabi::call(0x028F026C, gabi::at<void>(0x1018C934));
}
VERIFY(0x0201824C, cps_initializer);
