#include "gabi.h"
using namespace gabi;
namespace hd_screen_command_a {
u32 ready(u32 self) {
 WWHD_FUNC(0x02647CC0,u32,self);
 if(!load<u8>(self+0x74)) return 0;
 if(load<u8>(call<u32>(0x025200D4)+0x5292)) return 0;
 if(load<u32>(call<u32>(0x025200D4)+0x5CD8)&0x11) { store<u32>(self+0x70,1); return 0; }
 return 1;
}
VERIFY(0x02647CC0,ready);
void wait(u32 self) {
 WWHD_FUNC(0x02647D50,void,self);
 if(!call<u32>(0x02647CC0,self)) return;
 u32 n=load<u32>(self+0x70);
 if(n) store<u32>(self+0x70,n-1);
 else call<void>(0x020063C0,self+0x18,0x1049118Cu);
}
VERIFY(0x02647D50,wait);
void waitUpdate(u32 self) {
 WWHD_FUNC(0x02647E48,void,self);
 u32 n=load<u32>(self+0x70);
 if(n) store<u32>(self+0x70,n-1);
 else if(!call<u32>(0x02647CC0,self)) call<void>(0x020063C0,self+0x18,0x104911ECu);
 call_ptr<void>(load<u32>(load<u32>(self+4)+0x8C),self);
}
VERIFY(0x02647E48,waitUpdate);
}
