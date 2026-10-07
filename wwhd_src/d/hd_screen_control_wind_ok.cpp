#include "gabi.h"
using namespace gabi;
namespace hd_screen_control_wind_ok {
struct Event { u8 data[40]; };
void wait(u32 self) {
 WWHD_FUNC(0x0264F09C,void,self);
 FrameLocal<Event> event(8);
 call<void>(0x020013C0,event.a,load<u32>(0x101F4FF0)+0x30);
 u32 n=load<u8>(self+0x50), flag=load<u8>(self+0x55);
 if(n) { n=u8(n-1); store<u8>(self+0x50,n); }
 if(flag || !n || call<u32>(0x026F620C,load<u32>(event.a))) {
  store<u8>(self+0x54,1);
  call<void>(0x020063C0,self+0x18,0x10491C00u);
 }
}
VERIFY(0x0264F09C,wait);
}
