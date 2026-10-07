#include "gabi.h"
using namespace gabi;
namespace hd_screen_boat_race_failed_char {
struct Pair { u8 data[8]; };
void wait(u32 self) {
 WWHD_FUNC(0x02626CD8,void,self);
 u32 n=load<u32>(self+0x58);
 if(n) {
  store<u32>(self+0x58,n-1);
  if(n-1) return;
  FrameLocal<Pair> names(8);
  store<u32>(names.a,0x100E570C); store<u32>(names.a+4,0x100E565C);
  call<void>(0x026F5258,load<u32>(self+0x54),names.a);
 } else if(!load<u8>(load<u32>(self+0x54)+0xD10)) {
  call<void>(0x020063C0,self+0x18,0x1048F0F0u);
 }
}
VERIFY(0x02626CD8,wait);
}
