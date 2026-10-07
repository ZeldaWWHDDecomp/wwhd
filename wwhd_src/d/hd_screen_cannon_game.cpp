#include "gabi.h"
using namespace gabi;
namespace hd_screen_cannon_game {
void reveal(u32 self,u32 target) {
 WWHD_FUNC(0x0264679C,void,self,target);
 u32 n=load<u32>(self+0x5C);
 if(n>=target) return;
 u32 index=n+10, slots=load<u32>(self+0x4C);
 if(index<load<u32>(self+0x48)) slots+=index*4;
 store<u8>(load<u32>(slots)+0x50,1);
 store<u32>(self+0x5C,load<u32>(self+0x5C)+1);
}
VERIFY(0x0264679C,reveal);
}
