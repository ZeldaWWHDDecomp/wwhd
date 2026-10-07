#include "gabi.h"
using namespace gabi;
namespace hd_screen_heart {
static u32 heart(u32 self,u32 index) {
 u32 slots=load<u32>(self+0x4C);
 if(index<load<u32>(self+0x48)) slots+=index*4;
 return load<u32>(slots);
}
void refresh(u32 self) {
 WWHD_FUNC(0x0266D334,void,self);
 for(u32 i=0;i<20;++i) {
  u32 child=heart(self,i);
  u32 value=call<u32>(0x0266D308,self,i);
  call<void>(0x026F7090,child,value);
 }
}
VERIFY(0x0266D334,refresh);
void select(u32 self) {
 WWHD_FUNC(0x0266D588,void,self);
 u32 shown=load<u32>(self+0xF8)*4, max=load<u32>(self+0xF4);
 u32 amount=max>shown?shown:max;
 u32 index=amount?(amount-1)>>2:0xFFFFFFFFu;
 u32 old=load<u32>(self+0xFC);
 if(old==index) return;
 if(s32(index)>=0) { store<u32>(heart(self,index)+0x58,1); old=load<u32>(self+0xFC); }
 if(s32(old)>=0) store<u32>(heart(self,old)+0x58,2);
 store<u32>(self+0xFC,index);
}
VERIFY(0x0266D588,select);
void refill(u32 self) {
 WWHD_FUNC(0x0266D624,void,self);
 u32 timer=load<u32>(self+0xF0), save;
 if(timer) { store<u32>(self+0xF0,timer-1); save=load<u32>(0x101F84DC); }
 else {
  save=load<u32>(0x101F84DC); u32 target=load<u16>(save+0x20)>>2, shown=load<u32>(self+0xF8);
  if(shown!=target) {
   store<u32>(self+0xF0,8);
   if(target>shown) {
    call<void>(0x026F711C,heart(self,shown),1u);
    store<u32>(self+0xF8,load<u32>(self+0xF8)+1);
   } else {
    shown-=1; store<u32>(self+0xF8,shown);
    call<void>(0x026F711C,heart(self,shown),0u);
   }
   call<void>(0x0266D588,self); save=load<u32>(0x101F84DC);
  }
 }
 u32 target=load<u16>(save+0x22), shown=load<u32>(self+0xF4);
 if(shown!=target) {
  store<u32>(self+0xF4,shown<target?shown+1:shown-1);
  call<void>(0x0266D334,self); call<void>(0x0266D588,self);
 }
 if(!load<u8>(0x1047B08F)) call<void>(0x020063C0,self+0x18,0x104947F4u);
}
VERIFY(0x0266D624,refill);
}
