#include "wwhd.h"
#include "gabi.h"
void dBgS_SplGrpChk_empty(){WWHD_FUNC(0x024F2100,void);}
VERIFY(0x024F2100,dBgS_SplGrpChk_empty);
u32 dBgS_SplGrpChk_ctor(u32 self){
 WWHD_FUNC(0x024F2104,u32,self);
 if(!self)self=gabi::call<u32>(0x0273AD10,0x50);
 if(self){
  gabi::store<u16>(self+2,0x100);gabi::store<u32>(self+8,0xFFFFFFFF);gabi::store<u32>(self+4,0);gabi::store<u32>(self+0xC,0x10043C2C);gabi::store<u16>(self,0xFFFF);
  gabi::call<void>(0x02008B60,gabi::at<void>(self+0x10));
  gabi::store<u8>(self+0x2B,0);f32 z=gabi::load<f32>(0x10043C8C);
  gabi::store<u8>(self+0x2D,0);gabi::store<f32>(self+0x48,z);gabi::store<u8>(self+0x29,0);
  f32 f=gabi::load<f32>(0x10043C90);
  gabi::store<u8>(self+0x28,0);gabi::store<u8>(self+0x2E,0);gabi::store<u32>(self+0x20,0x10043C5C);gabi::store<u8>(self+0x2C,0);gabi::store<f32>(self+0x44,f);gabi::store<u8>(self+0x2A,0);gabi::store<u32>(self+0x4C,0);
  gabi::store<f32>(self+0x38,z);gabi::store<u32>(self+0x14,self+0x30);gabi::store<u32>(self+0x10,self+0x24);gabi::store<f32>(self+0x40,z);gabi::store<f32>(self+0x3C,z);
  gabi::store<u32>(self+0xC,0x10043C4C);gabi::store<u32>(self+0x30,0x10043C6C);gabi::store<u32>(self+0x34,0);gabi::store<u32>(self+0x24,0x10043C7C);
 }return self;
}
VERIFY(0x024F2104,dBgS_SplGrpChk_ctor);
void dBgS_SplGrpChk_Init(void* p){
 WWHD_FUNC(0x024F220C,void,p);u32 self=gabi::ea(p);
 u32 flags=gabi::load<u32>(self+0x4C)&0xFFFFFFFC;f32 height=gabi::load<f32>(self+0x3C);
 gabi::store<u32>(self+4,0);gabi::store<u32>(self+8,0xFFFFFFFF);gabi::store<f32>(self+0x48,height);gabi::store<u32>(self+0x4C,flags);gabi::store<u16>(self+2,0x100);gabi::store<u16>(self,0xFFFF);
}
VERIFY(0x024F220C,dBgS_SplGrpChk_Init);
void d_bg_s_spl_grp_chk_static_init() {
 WWHD_FUNC(0x024F2248,void);
 gabi::store<u32>(0x1046EDAC,0);gabi::store<u32>(0x1046EDA4,0);gabi::store<u32>(0x1046EDB0,0);gabi::store<u32>(0x1046EDA8,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D530C));
 f32 lo=gabi::load<f32>(0x10043C9C),hi=gabi::load<f32>(0x10043CA0);
 gabi::store<f32>(0x1046ED98,lo);gabi::store<f32>(0x1046ED9C,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1046EDA0));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D5318));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1046EDA1));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D5324));
}
VERIFY(0x024F2248,d_bg_s_spl_grp_chk_static_init);
