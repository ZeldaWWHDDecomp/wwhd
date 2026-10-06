#include "wwhd.h"
#include "gabi.h"
u32 Title_Execute(u32 self) {
 WWHD_FUNC(0x025ABF00,u32,self);
 u32 reset=gabi::load<u32>(0x101F4974);
 if(gabi::load<u32>(reset)) {
  gabi::call<void>(0x025DC86C,self,5,0,5,1);
  return 1;
 }
 gabi::call<void>(0x02618094,gabi::load<u32>(0x101F5088),3);
 u32 service=gabi::load<u32>(0x101F852C);
 u32 state=gabi::load<u32>(service+0x20);
 if(state>=0xFFFFFFFD&&state<=0xFFFFFFFE) {
  gabi::call<void>(0x025F05F8,0x101D5E94);
  gabi::call<void>(0x025DC86C,self,11,0,5,1);
 }else gabi::call<void>(0x02520230,self,8);
 return 1;
}
VERIFY(0x025ABF00,Title_Execute);
u32 Title_IsDelete(u32 self) {
 WWHD_FUNC(0x025ABFCC,u32,self);
 return 1;
}
VERIFY(0x025ABFCC,Title_IsDelete);
