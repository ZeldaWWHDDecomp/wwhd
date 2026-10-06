#include "gabi.h"
using namespace gabi;
namespace ext_complete {
void blend(void* dst,void* anim,void* weights){
 WWHD_FUNC(0x025EE884,void,dst,anim,weights);
 u32 d=ea(dst),a=ea(anim),w=ea(weights);
 store<u32>(a+0x50,load<u32>(a+0x50)|8);
 u32 record=load<u32>(a+0x2C);
 store<u32>(d+8,load<u32>(d+8)|8);
 u16 count=load<u16>(a+0x40);u32 out=load<u32>(d);
 if(!count)return;
 f32 one=load<f32>(0x100586D4);
 for(u32 i=0;i<count;i++,record+=0x30){
  u32 flag=load<u32>(load<u32>(a+0x38)+i*4);
  if(flag&0x80000000)continue;
  f32 z=load<f32>(record+0x28),y=load<f32>(record+0x24);
  f32 zz=z+z,yy=y+y,z2=fmuls_ppc(zz,z),q=load<f32>(record+0x2C);
  f32 c=fnmsubs(yy,y,one),x=load<f32>(record+0x20),qq=q+q,xx=x+x;
  f32 m00=c-z2,yz=fmuls_ppc(qq,y),c2=fnmsubs(xx,x,one);
  f32 m02=fmadds(xx,z,yz),xz=fmuls_ppc(xx,y),weight=load<f32>(w+i*4),qz=fmuls_ppc(qq,z);
  f32 m11=c2-z2,m01=xz-qz,xy=fmuls_ppc(yy,z),m10=xz+qz,m12=fnmsubs(qq,x,xy);
  u32 p=out+(flag&0x7FFF)*56;
  f32 old20=load<f32>(p+0x20),old24=load<f32>(p+0x24),old28=load<f32>(p+0x28),old2C=load<f32>(p+0x2C);
  store<f32>(p+0x20,fmadds(m00,weight,old20));store<f32>(p+0x24,fmadds(m01,weight,old24));
  f32 old30=load<f32>(p+0x30),old34=load<f32>(p+0x34);
  store<f32>(p+0x28,fmadds(m02,weight,old28));store<f32>(p+0x2C,fmadds(m10,weight,old2C));
  f32 old4=load<f32>(p+4),old8=load<f32>(p+8);
  store<f32>(p+0x30,fmadds(m11,weight,old30));store<f32>(p+0x34,fmadds(m12,weight,old34));
  f32 t4=load<f32>(record+4),t8=load<f32>(record+8);weight=load<f32>(w+i*4);
  f32 old1C=load<f32>(p+0x1C),tC=load<f32>(record+0xC),t10=load<f32>(record+0x10),oldC=load<f32>(p+0xC),old10=load<f32>(p+0x10);
  store<f32>(p+4,fmadds(t4,weight,old4));store<f32>(p+8,fmadds(t8,weight,old8));
  f32 t14=load<f32>(record+0x14),t18=load<f32>(record+0x18),old14=load<f32>(p+0x14),old18=load<f32>(p+0x18);
  store<f32>(p+0xC,fmadds(tC,weight,oldC));store<f32>(p+0x10,fmadds(t10,weight,old10));
  u32 oldflags=load<u32>(p),newflags=load<u32>(record);
  store<f32>(p+0x14,fmadds(t14,weight,old14));store<f32>(p+0x18,fmadds(t18,weight,old18));
  store<f32>(p+0x1C,old1C+weight);store<u32>(p,oldflags|~newflags);
 }
}
VERIFY(0x025EE884,blend);
void destroySolid(void* heap){WWHD_FUNC(0x025F0148,void,heap);u32 h=ea(heap);if(!h)return;u32 parent=call<u32>(0x027EC124,heap);u32 kind=call<u32>(0x027EC23C,at<void>(parent));if(kind==0x534C4944)call(0x025E3868,at<void>(parent));else call_ptr(load<u32>(load<u32>(parent+0xC)+0x3C),at<void>(parent),heap);}
VERIFY(0x025F0148,destroySolid);
void* heapScope(void* self,u32 tag,u32 size,u32 align){WWHD_FUNC(0x025F01D8,void*,self,tag,size,align);u32 s=ea(self);if(!s){s=call<u32>(0x0273AD10,4);if(!s)return at<void>(s);}if(!align)align=0x100;u32 h=call<u32>(0x025E35BC,size,0,align);store<u32>(s,h);if(h&&tag)store<u32>(h+0x10,tag);return at<void>(s);}
VERIFY(0x025F01D8,heapScope);
void heapScopeDtor(void* self,u32 flags){WWHD_FUNC(0x025F0270,void,self,flags);u32 s=ea(self);if(!s)return;if(load<u32>(s)){call(0x025E37D8,self);call(0x025E3678,at<void>(load<u32>(s)));}if(flags&1)call(0x0273AF40,self);}
VERIFY(0x025F0270,heapScopeDtor);
void heapScopeDestroy(void* self){WWHD_FUNC(0x025F02D0,void,self);u32 s=ea(self);if(load<u32>(s)){call(0x025E37D8,self);call(0x025E3868,at<void>(load<u32>(s)));store<u32>(s,0);}}
VERIFY(0x025F02D0,heapScopeDestroy);
void startup(){WWHD_FUNC(0x025F0318,void);store<u32>(0x1048CF60,0);store<u32>(0x1048CF58,0);store<u32>(0x1048CF64,0);store<u32>(0x1048CF5C,0);call(0x028F026C,at<void>(0x101F47BC));f32 a=load<f32>(0x10058E84),b=load<f32>(0x10058E88);store<f32>(0x1048CF4C,a);store<f32>(0x1048CF50,b);call(0x028ED6F8,at<void>(0x1048CF54));call(0x028F026C,at<void>(0x101F47C8));call(0x028EAB2C,at<void>(0x1048CF55));call(0x028F026C,at<void>(0x101F47D4));}
VERIFY(0x025F0318,startup);
}
