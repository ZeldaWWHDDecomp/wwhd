/* HD progress display update; original behaviour. Binary-derived. */
#include "gabi.h"
using namespace gabi;
namespace hd_ui_02702A20 {
void updateProgress(u32 self) {
 WWHD_FUNC(0x02702D50,void,self);
 u32 save=load<u32>(0x101F84DC),flags=load<u32>(0x1047B074);
 u32 target=load<u8>(save+0x33),old=load<u32>(self+0x50);
 if(flags&0x40000) target=32;
 bool changed=old!=target;
 if(changed) store<u32>(self+0x50,target);
 u32 play=call<u32>(0x025200D4);
 u32 actual=u32(s32(load<s16>(play+0x5B62))),shown=load<u32>(self+0x54);
 if(actual>shown) { store<u32>(self+0x54,shown+1);call<void>(0x02702BB8,self); }
 else if(actual<shown) { store<u32>(self+0x54,shown-1);call<void>(0x02702BB8,self); }
 else if(changed) call<void>(0x02702BB8,self);
}
VERIFY(0x02702D50,updateProgress);
}
