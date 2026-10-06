/* Local WWHD leaf-process reconstruction. */
#include "wwhd.h"
s32 leaf_priority(void* self) {
 WWHD_FUNC(0x025DF2B8,s32,self);
 return gabi::call<s32>(0x025DE460,gabi::at<void>(gabi::ea(self)+0xC2));
}
VERIFY(0x025DF2B8,leaf_priority);
s32 leaf_drawMethod(void* methods,void* process) {
 WWHD_FUNC(0x025DF2C0,s32,methods,process);
 u32 method=gabi::load<u32>(gabi::ea(methods)+0x10);
 return gabi::call<s32>(0x025DFCAC,gabi::at<void>(method),process);
}
VERIFY(0x025DF2C0,leaf_drawMethod);
s32 leaf_draw(void* self) {
 WWHD_FUNC(0x025DF2C8,s32,self);
 u32 addr=gabi::ea(self);
 if(gabi::load<u8>(addr+0xC0)!=0) return 0;
 u32 methods=gabi::load<u32>(addr+0xBC);
 return leaf_drawMethod(gabi::at<void>(methods),self);
}
VERIFY(0x025DF2C8,leaf_draw);
s32 leaf_execute(void* self) {
 WWHD_FUNC(0x025DF300,s32,self);
 u32 methods=gabi::load<u32>(gabi::ea(self)+0xBC);
 return gabi::call<s32>(0x025DFCC4,gabi::at<void>(methods),self);
}
VERIFY(0x025DF300,leaf_execute);
s32 leaf_isDelete(void* self) {
 WWHD_FUNC(0x025DF30C,s32,self);
 u32 methods=gabi::load<u32>(gabi::ea(self)+0xBC);
 return gabi::call<s32>(0x025DFCCC,gabi::at<void>(methods),self);
}
VERIFY(0x025DF30C,leaf_isDelete);
s32 leaf_delete(void* self) {
 WWHD_FUNC(0x025DF318,s32,self);
 u32 addr=gabi::ea(self),methods=gabi::load<u32>(addr+0xBC);
 s32 result=gabi::call<s32>(0x025DFCD4,gabi::at<void>(methods),self);
 if(result==1) gabi::store<u32>(addr+0xB8,0);
 return result;
}
VERIFY(0x025DF318,leaf_delete);
s32 leaf_create(void* self) {
 WWHD_FUNC(0x025DF35C,s32,self);
 u32 addr=gabi::ea(self);
 if(gabi::load<u8>(addr+0xC)==0) {
  u32 profile=gabi::load<u32>(addr+0x10);
  u32 methods=gabi::load<u32>(profile+0x1C);
  gabi::store<u32>(addr+0xBC,methods);
  u32 type=gabi::call<u32>(0x025DD268,gabi::at<void>(0x101F3BD8));
  gabi::store<u32>(addr+0xB8,type);
  s32 priority=gabi::load<s16>(profile+0x20);
  gabi::call(0x025DE470,gabi::at<void>(addr+0xC2),priority);
  gabi::store<u8>(addr+0xC0,0);
 }
 u32 methods=gabi::load<u32>(addr+0xBC);
 return gabi::call<s32>(0x025DFCDC,gabi::at<void>(methods),self);
}
VERIFY(0x025DF35C,leaf_create);
void leaf_initializer() {
 WWHD_FUNC(0x025DF3D4,void);
 for(u32 off:{8u,0u,12u,4u}) gabi::store<u32>(0x1048A850+off,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101F3BB4));
 f32 a=gabi::load<f32>(0x10058324),b=gabi::load<f32>(0x10058328);
 gabi::store<f32>(0x1048A844,a); gabi::store<f32>(0x1048A848,b);
 gabi::call(0x028ED6F8,gabi::at<void>(0x1048A84C));
 gabi::call(0x028F026C,gabi::at<void>(0x101F3BC0));
 gabi::call(0x028EAB2C,gabi::at<void>(0x1048A84D));
 gabi::call(0x028F026C,gabi::at<void>(0x101F3BCC));
}
VERIFY(0x025DF3D4,leaf_initializer);
