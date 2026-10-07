#include "gabi.h"
using namespace gabi;
namespace hd_screen_boat_race_result {
void wait(u32 self) {
 WWHD_FUNC(0x02627714,void,self);
 u32 n=load<u32>(self+0x5C)-1; store<u32>(self+0x5C,n);
 if(!n) call<void>(0x020063C0,self+0x18,0x1048F210u);
}
VERIFY(0x02627714,wait);
void roll(u32 self) {
 WWHD_FUNC(0x02627A80,void,self);
 call_ptr<void>(load<u32>(load<u32>(self+4)+0x8C),self);
 u32 n=load<u32>(self+0x60);
 if(n==0) { call<void>(0x025E1988,0x849u); n=load<u32>(self+0x60); }
 else if(n==20 && load<u32>(self+0x58)==2) { call<void>(0x025E1988,0x860u); n=load<u32>(self+0x60); }
 store<u32>(self+0x60,n+1);
}
VERIFY(0x02627A80,roll);
}
