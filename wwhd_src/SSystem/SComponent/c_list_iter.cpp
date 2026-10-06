#include "wwhd.h"
#include "gabi.h"

s32 cLsIt_Method(void* list,u32 method,u32 userData) {
 WWHD_FUNC(0x020100A0,s32,list,method,userData);
 if(gabi::load<s32>(gabi::ea(list)+8)>0)
  return gabi::call<s32>(0x02019D50,gabi::at<void>(gabi::load<u32>(gabi::ea(list))),method,userData);
 return 1;
}
VERIFY(0x020100A0,cLsIt_Method);
void* cLsIt_Judge(void* list,u32 judge,u32 userData) {
 WWHD_FUNC(0x020100BC,void*,list,judge,userData);
 if(gabi::load<s32>(gabi::ea(list)+8)>0)
  return gabi::call<void*>(0x02019DE8,gabi::at<void>(gabi::load<u32>(gabi::ea(list))),judge,userData);
 return nullptr;
}
VERIFY(0x020100BC,cLsIt_Judge);
void c_list_iter_static_init() {
 WWHD_FUNC(0x020100D8,void);
 gabi::store<u32>(0x101FF800,0);gabi::store<u32>(0x101FF7F8,0);gabi::store<u32>(0x101FF804,0);gabi::store<u32>(0x101FF7FC,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C7D8));
 f32 negativePi=gabi::load<f32>(0x10001EE0),positivePi=gabi::load<f32>(0x10001EE4);
 gabi::store<f32>(0x101FF7EC,negativePi);gabi::store<f32>(0x101FF7F0,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF7F4));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C7E4));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF7F5));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C7F0));
}
VERIFY(0x020100D8,c_list_iter_static_init);

/* ---- hosted here: a separate TU linked between c_list_iter and
 * c_m2d (name unknown): a 12-byte class of three words with an out-of-line constructor, and its
 * static initializer (header statics + one instance at 101FF824 built from (0,0,0)). ---- */
u32 hd_word3_ctor(u32 self,u32 a,u32 b,u32 c) {
 WWHD_FUNC(0x0201016C,u32,self,a,b,c);
 if(!self){self=gabi::call<u32>(0x0273AD10,12u);if(!self)return 0;}
 gabi::store<u32>(self+4,b);gabi::store<u32>(self+0,a);gabi::store<u32>(self+8,c);
 return self;
}
VERIFY(0x0201016C,hd_word3_ctor);
void hd_static_init_020101D0() {
 WWHD_FUNC(0x020101D0,void);
 gabi::store<u32>(0x101FF81C,0);gabi::store<u32>(0x101FF814,0);gabi::store<u32>(0x101FF820,0);gabi::store<u32>(0x101FF818,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C7FC));
 f32 negativePi=gabi::load<f32>(0x10001EEC),positivePi=gabi::load<f32>(0x10001EF0);
 gabi::store<f32>(0x101FF808,negativePi);gabi::store<f32>(0x101FF80C,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF810));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C808));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF811));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C814));
 gabi::call<u32>(0x0201016C,0x101FF824u,0u,0u,0u);
}
VERIFY(0x020101D0,hd_static_init_020101D0);
