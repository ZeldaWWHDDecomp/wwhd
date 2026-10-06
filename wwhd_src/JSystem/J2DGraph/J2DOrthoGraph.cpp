#include "gabi.h"
using namespace gabi;
namespace J2DOrthoGraphHD {
void setLookat(void *self) {
 WWHD_FUNC(0x027EDE30,void,self);
 call<void>(0x028E9098,at<void>(ea(self)+0x7C));
}
VERIFY(0x027EDE30,setLookat);
void *construct(void *self,f32 x,f32 y,f32 width,f32 height,f32 nearPlane,f32 farPlane) {
 WWHD_FUNC(0x027EDE38,void*,self,x,y,width,height,nearPlane,farPlane);
 if(!self)self=call<void*>(0x0273AD10,u32(0xD4));
 if(!self)return nullptr;
 u32 a=ea(self);
 call<void>(0x027EDB54,self,x,y,width,height);
 store<u32>(a+0xB8,0x1016DEAC);
 if(a+0xBC==0)call<void*>(0x0273AD10,u32(0x10));
 f32 zero=load<f32>(0x1016DEA0);
 store<f32>(a+0xCC,nearPlane);store<f32>(a+0xC4,width);
 store<f32>(a+0xC8,height);store<f32>(a+0xC0,zero);
 store<f32>(a+0xD0,farPlane);store<f32>(a+0xBC,zero);
 call<void>(0x027EDE30,self);
 return self;
}
VERIFY(0x027EDE38,construct);
void setPort(void *self) {
 WWHD_FUNC(0x027EDF68,void,self);
 call<void>(0x027EDD50,self);
 u32 a=ea(self);
 f32 bottom=fadds_ppc(load<f32>(a+0xC8),load<f32>(0x1016DEA4));
 call<void>(0x028E9A70,at<void>(a+0x3C),load<f32>(a+0xC0),bottom,load<f32>(a+0xBC),load<f32>(a+0xC4),load<f32>(a+0xCC),load<f32>(a+0xD0));
}
VERIFY(0x027EDF68,setPort);
void scissorBounds(void *self,void *output,const void *input) {
 WWHD_FUNC(0x027EDFC0,void,self,output,input);
 u32 a=ea(self),i=ea(input),o=ea(output);
 f32 orthoX=load<f32>(a+0xBC),left=load<f32>(a),orthoY=load<f32>(a+0xC0),top=load<f32>(a+4);
 f32 xscale=f32(fsubs_ppc(load<f32>(a+8),left)/fsubs_ppc(load<f32>(a+0xC4),orthoX));
 f32 yscale=f32(fsubs_ppc(load<f32>(a+12),top)/fsubs_ppc(load<f32>(a+0xC8),orthoY));
 f32 x0=fmadds(fsubs_ppc(load<f32>(i),orthoX),xscale,left);
 f32 x1=fmadds(fsubs_ppc(load<f32>(i+8),orthoX),xscale,left);
 f32 y0=fmadds(fsubs_ppc(load<f32>(i+4),orthoY),yscale,top);
 f32 y1=fmadds(fsubs_ppc(load<f32>(i+12),orthoY),yscale,top);
 store<f32>(o,x0);store<f32>(o+8,x1);store<f32>(o+4,y0);store<f32>(o+12,y1);
}
VERIFY(0x027EDFC0,scissorBounds);
// Adjacent initializer attribution inferred from TU placement before virtual type getter.
void staticInit() {
 WWHD_FUNC(0x027EE03C,void);
 store<u32>(0x104B4530,0);store<u32>(0x104B4528,0);
 store<u32>(0x104B4534,0);store<u32>(0x104B452C,0);
 call<void>(0x028F026C,at<void>(0x101F9838));
}
VERIFY(0x027EE03C,staticInit);
u32 getGrafType(void *self) { WWHD_FUNC(0x027EE064,u32,self);return 1; }
VERIFY(0x027EE064,getGrafType);
}
