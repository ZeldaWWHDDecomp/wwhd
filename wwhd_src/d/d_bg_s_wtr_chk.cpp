#include "wwhd.h"
#include "gabi.h"
u32 dBgS_WtrChk_ctor(u32 self){
 WWHD_FUNC(0x024F22DC,u32,self);
 if(!self)self=gabi::call<u32>(0x0273AD10,0x50);
 if(self){
  gabi::call<u32>(0x024F2104,self);
  u32 flags=gabi::load<u32>(self+0x34);
  gabi::store<u32>(self+0xC,0x10043CEC);
  gabi::store<u32>(self+0x24,0x10043D1C);
  gabi::store<u32>(self+0x20,0x10043CFC);
  gabi::store<u32>(self+0x34,flags|2);
  gabi::store<u32>(self+0x30,0x10043D0C);
 }
 return self;
}
VERIFY(0x024F22DC,dBgS_WtrChk_ctor);
void d_bg_s_wtr_chk_static_init() {
 WWHD_FUNC(0x024F2360,void);
 gabi::store<u32>(0x1046EDC8,0);gabi::store<u32>(0x1046EDC0,0);gabi::store<u32>(0x1046EDCC,0);gabi::store<u32>(0x1046EDC4,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D5330));
 f32 lo=gabi::load<f32>(0x10043D34),hi=gabi::load<f32>(0x10043D38);
 gabi::store<f32>(0x1046EDB4,lo);gabi::store<f32>(0x1046EDB8,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1046EDBC));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D533C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1046EDBD));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D5348));
}
VERIFY(0x024F2360,d_bg_s_wtr_chk_static_init);
