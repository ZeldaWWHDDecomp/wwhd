/* Local WWHD cylinder geometry reconstruction. */
#include "SSystem/SComponent/c_xyz.h"
#include <bit>
struct Cyl_l { cXyz center; be<f32> radius,height; be<u32> vtable; };
WWHD_SIZE(Cyl_l,24);
static bool nan_value(f32 v) { return (std::bit_cast<u32>(v) << 1) > 0xFF000000u; }
void cyl_setC(Cyl_l* self,cXyz* center) {
 WWHD_FUNC(0x020182E0,void,self,center);
 f32 x=center->x;
 if(nan_value(x)) gabi::call(0x0273AA24,STR(0x10003150),80,STR(0x10003160));
 f32 y=center->y;
 if(nan_value(y)) gabi::call(0x0273AA24,STR(0x10003150),81,STR(0x1000319C));
 f32 z=center->z;
 if(nan_value(z)) gabi::call(0x0273AA24,STR(0x10003150),82,STR(0x100031D8));
 x=center->x;
 f32 low=gabi::load<f32>(0x10003148),high=gabi::load<f32>(0x1000314C);
 bool valid=low<x && x<high;
 if(valid) { y=center->y; valid=low<y && y<high; }
 if(valid) { z=center->z; valid=low<z && z<high; }
 if(!valid) {
  gabi::call(0x0273AA24,STR(0x10003150),83,STR(0x10003214));
  y=center->y; z=center->z; x=center->x;
 }
 self->center.y=y; self->center.z=z; self->center.x=x;
}
VERIFY(0x020182E0,cyl_setC);
void cyl_setH(Cyl_l* self,f32 height) {
 WWHD_FUNC(0x02018428,void,self,height);
 if(nan_value(height)) gabi::call(0x0273AA24,STR(0x10003288),95,STR(0x100032B4));
 f32 low=gabi::load<f32>(0x10003148),high=gabi::load<f32>(0x1000314C);
 if(!(low<height && height<high)) gabi::call(0x0273AA24,STR(0x10003288),96,STR(0x10003298));
 self->height=height;
}
VERIFY(0x02018428,cyl_setH);
void cyl_setR(Cyl_l* self,f32 radius) {
 WWHD_FUNC(0x020184DC,void,self,radius);
 if(nan_value(radius)) gabi::call(0x0273AA24,STR(0x100032E8),119,STR(0x10003314));
 f32 low=gabi::load<f32>(0x10003148),high=gabi::load<f32>(0x1000314C);
 if(!(low<radius && radius<high)) gabi::call(0x0273AA24,STR(0x100032E8),120,STR(0x100032F8));
 self->radius=radius;
}
VERIFY(0x020184DC,cyl_setR);
Cyl_l* cyl_ctor(Cyl_l* self) {
 WWHD_FUNC(0x02018590,Cyl_l*,self);
 if(!self) self=gabi::call<Cyl_l*>(0x0273AD10,24u);
 if(self) {
  f32 zero=gabi::load<f32>(0x1000335C);
  self->vtable=0x1000334C;
  self->height=zero; self->radius=zero;
 }
 return self;
}
VERIFY(0x02018590,cyl_ctor);
void cyl_setShape(Cyl_l* self,Cyl_l* src) {
 WWHD_FUNC(0x020185E0,void,self,src);
 gabi::Local<cXyz> center;
 center->x=(f32)src->center.x; center->y=(f32)src->center.y; center->z=(f32)src->center.z;
 cyl_setC(self,center.get());
 f32 radius=src->radius;
 cyl_setR(self,radius);
 f32 height=src->height;
 cyl_setH(self,height);
}
VERIFY(0x020185E0,cyl_setShape);
void cyl_set(Cyl_l* self,cXyz* center,f32 radius,f32 height) {
 WWHD_FUNC(0x0201864C,void,self,center,radius,height);
 cyl_setC(self,center);
 cyl_setR(self,radius);
 cyl_setH(self,height);
}
VERIFY(0x0201864C,cyl_set);
s32 cyl_crossCylinder(Cyl_l* self,Cyl_l* other,f32* distance) {
 WWHD_FUNC(0x020186C4,s32,self,other,distance);
 return gabi::call<s32>(0x02014E18,self,other,distance);
}
VERIFY(0x020186C4,cyl_crossCylinder);
s32 cyl_crossSphere(Cyl_l* self,void* sphere,cXyz* position) {
 WWHD_FUNC(0x020186C8,s32,self,sphere,position);
 gabi::Local<f32> distance;
 return gabi::call<s32>(0x02013854,self,sphere,position,distance.get());
}
VERIFY(0x020186C8,cyl_crossSphere);
void cyl_initializer() {
 WWHD_FUNC(0x020186EC,void);
 for(u32 off:{8u,0u,12u,4u}) gabi::store<u32>(0x101FF91C+off,0);
 gabi::call(0x028F026C,gabi::at<void>(0x1018C940));
 f32 a=gabi::load<f32>(0x10003364),b=gabi::load<f32>(0x10003368);
 gabi::store<f32>(0x101FF910,a); gabi::store<f32>(0x101FF914,b);
 gabi::call(0x028ED6F8,gabi::at<void>(0x101FF918));
 gabi::call(0x028F026C,gabi::at<void>(0x1018C94C));
 gabi::call(0x028EAB2C,gabi::at<void>(0x101FF919));
 gabi::call(0x028F026C,gabi::at<void>(0x1018C958));
}
VERIFY(0x020186EC,cyl_initializer);
