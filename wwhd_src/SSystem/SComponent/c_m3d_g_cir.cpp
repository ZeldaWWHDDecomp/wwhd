/* Local WWHD circle geometry reconstruction. */
#include "SSystem/SComponent/c_xyz.h"
struct Cir_l { cXyz center; be<u32> vtable; be<f32> radius; };
WWHD_SIZE(Cir_l,20);
void cir_dtor(Cir_l* self,s32 flags) {
 WWHD_FUNC(0x02018034,void,self,flags);
 if(self && (flags&1)) gabi::call(0x0273AF40,self,flags);
}
VERIFY(0x02018034,cir_dtor);
Cir_l* cir_ctor(Cir_l* self) {
 WWHD_FUNC(0x02018048,Cir_l*,self);
 if(!self) self=gabi::call<Cir_l*>(0x0273AD10,20u);
 if(self) {
  gabi::call(0x0201083C,self);
  f32 zero=gabi::load<f32>(0x100030EC);
  self->vtable=0x10003100;
  self->radius=zero;
 }
 return self;
}
VERIFY(0x02018048,cir_ctor);
void cir_set(Cir_l* self,f32 x,f32 y,f32 radius,f32 z) {
 WWHD_FUNC(0x020180A8,void,self,x,y,radius,z);
 self->center.z=z;
 self->center.y=y;
 self->radius=radius;
 self->center.x=x;
}
VERIFY(0x020180A8,cir_set);
void cir_initializer() {
 WWHD_FUNC(0x020180BC,void);
 for(u32 off:{8u,0u,12u,4u}) gabi::store<u32>(0x101FF8E4+off,0);
 gabi::call(0x028F026C,gabi::at<void>(0x1018C8F8));
 f32 a=gabi::load<f32>(0x100030F4),b=gabi::load<f32>(0x100030F8);
 gabi::store<f32>(0x101FF8D8,a); gabi::store<f32>(0x101FF8DC,b);
 gabi::call(0x028ED6F8,gabi::at<void>(0x101FF8E0));
 gabi::call(0x028F026C,gabi::at<void>(0x1018C904));
 gabi::call(0x028EAB2C,gabi::at<void>(0x101FF8E1));
 gabi::call(0x028F026C,gabi::at<void>(0x1018C910));
}
VERIFY(0x020180BC,cir_initializer);
