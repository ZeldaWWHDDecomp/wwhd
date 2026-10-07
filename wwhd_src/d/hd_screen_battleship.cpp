#include "gabi.h"
using namespace gabi;
namespace hd_screen_battleship {
template<unsigned N> struct Bytes { u8 data[N]; };
void updateDisplay(u32 self) {
 WWHD_FUNC(0x02623B44, void, self);
 FrameLocal<Bytes<12>> text(8); FrameLocal<Bytes<6>> buffer(0x14);
 store<u16>(buffer.a+4,0); store<u16>(buffer.a,0);
 store<u32>(text.a,buffer.a); store<u32>(text.a+4,0x100E49F4); store<u32>(text.a+8,3);
 call<void>(0x02759D3C,text.a,0x100E4AACu,load<u32>(self+0x5C));
 call<void>(0x027046D4,load<u32>(self+0x50),text.a,0u);
}
VERIFY(0x02623B44, updateDisplay);
void reveal(u32 self,u32 first,u32 second) {
 WWHD_FUNC(0x02624134, void, self, first, second);
 u32 n=load<u32>(self+0x58);
 if(n<second) {
  u32 index=n+30, slots=load<u32>(self+0x4C);
  if(index<load<u32>(self+0x48)) slots+=index*4;
  store<u8>(load<u32>(slots)+0x50,1);
  store<u32>(self+0x58,load<u32>(self+0x58)+1);
 }
 n=load<u32>(self+0x5C);
 if(n>=first) return;
 u32 slots=load<u32>(self+0x4C);
 if(n<load<u32>(self+0x48)) slots+=n*4;
 store<u8>(load<u32>(slots)+0x50,1);
 store<u32>(self+0x5C,first);
 call<void>(0x02623B44,self);
}
VERIFY(0x02624134,reveal);
}
