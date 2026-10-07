#include "gabi.h"
using namespace gabi;
namespace hd_screen_command_guide {
u32 flag(u32 self) { WWHD_FUNC(0x026489F8,u32,self); return load<u8>(0x1047B07A); }
VERIFY(0x026489F8,flag);
static u32 vfn(u32 self,u32 slot) { return load<u32>(load<u32>(self+4)+slot); }
void wait(u32 self) {
 WWHD_FUNC(0x02648A04,void,self);
 if(call_ptr<u32>(vfn(self,0x14C),self)) {
  u32 n=load<u32>(self+0xD0);
  if(n) {
   store<u32>(self+0xD0,n-1);
   if(n==1) {
    store<u8>(self+0xDC,call<u32>(0x026489F8,self));
    store<u8>(self+0xDD,call_ptr<u32>(vfn(self,0x14C),self));
    call<void>(0x020063C0,self+0x18,0x10491670u);
   }
  } else store<u32>(self+0xD0,3);
 } else store<u32>(self+0xD0,0);
 call_ptr<void>(vfn(self,0xB4),self);
 call_ptr<void>(vfn(self,0x8C),self);
}
VERIFY(0x02648A04,wait);
}
