#include "gabi.h"
using namespace gabi;
namespace hd_screen_rupy_sword_counter {
template<unsigned N> struct Bytes { u8 data[N]; };
void updateDisplay(u32 self) {
 WWHD_FUNC(0x026CB2F4,void,self);
 FrameLocal<Bytes<12>> text(8); FrameLocal<Bytes<10>> buffer(0x14);
 store<u16>(buffer.a+8,0); store<u16>(buffer.a,0);
 store<u32>(text.a,buffer.a); store<u32>(text.a+4,0x10101D6C); store<u32>(text.a+8,5);
 call<void>(0x02759D3C,text.a,0x10101E28u,0xD7u,u32(load<u16>(self+0x54)));
 call<void>(0x027046D4,load<u32>(self+0x50),text.a,0u);
}
VERIFY(0x026CB2F4,updateDisplay);
void roll(u32 self) {
 WWHD_FUNC(0x026CB4A0,void,self);
 u32 game=call<u32>(0x025200D4);
 u32 shown=load<u16>(self+0x54), target=load<u16>(game+0x5CEC);
 if(shown<target) {
  u32 next=u16(shown+1); if(next>999) next=999;
  store<u16>(self+0x54,next); call<void>(0x026CB2F4,self);
 } else if(shown>target) {
  store<u16>(self+0x54,u16(shown-1)); call<void>(0x026CB2F4,self);
 }
 if(!load<u8>(0x1047B09A)) call<void>(0x020063C0,self+0x18,0x1049A3A8u);
}
VERIFY(0x026CB4A0,roll);
}
