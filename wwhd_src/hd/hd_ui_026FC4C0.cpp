/* Two HD wait-screen timers; original behaviour, binary-derived. */
#include "gabi.h"
using namespace gabi;
namespace hd_ui_026FC4C0 {
void updateFirst(u32 self) {
 WWHD_FUNC(0x026FC584,void,self);
 u32 timer=load<u32>(self+0x8C)-1;
 store<u32>(self+0x8C,timer);
 if(s32(timer)>=0) return;
 s32 index=load<s8>(call<u32>(0x025200D4)+0x5B30);
 u32 play=call<u32>(0x025200D4);
 if(!(load<u32>(play+u32(index*0x34)+0x5B00)&4) && !call<u32>(0x025DBB40)) return;
 u32 value=load<u32>(call<u32>(0x025200D4)+0x5C30);
 store<u32>(self+0x88,value);
 call<void>(0x020063C0,self+0x18,0x1049DECCu);
}
VERIFY(0x026FC584,updateFirst);
void updateSecond(u32 self) {
 WWHD_FUNC(0x026FC764,void,self);
 u32 timer=load<u32>(self+0x8C)-1;
 store<u32>(self+0x8C,timer);
 if(s32(timer)>=0) return;
 s32 index=load<s8>(call<u32>(0x025200D4)+0x5B30);
 u32 play=call<u32>(0x025200D4);
 if(!(load<u32>(play+u32(index*0x34)+0x5B00)&4) && !call<u32>(0x025DBB40)) return;
 u32 value=load<u32>(call<u32>(0x025200D4)+0x5C30);
 store<u32>(self+0x88,value);
 call<void>(0x020063C0,self+0x18,0x1049DEFCu);
}
VERIFY(0x026FC764,updateSecond);
}
