#include "gabi.h"
using namespace gabi;
static f32 fdivs(f32 a,f32 b) { return f32(f64(a)/f64(b)); }
namespace hd_screen_menu_map {
static u32 vfn(u32 self,u32 slot) { return load<u32>(load<u32>(self+4)+slot); }
void drawMap(u32 self) {
 WWHD_FUNC(0x0269A6E4,void,self);
 u32 ui=load<u32>(load<u32>(0x101F8344)+0x218),map=load<u32>(self+0x11C);
 u32 icons=load<u32>(map+0x150);
 u32 cell=call<u32>(0x02680DE0,load<u32>(map+0x14C),ui+0x3C);
 u32 n=load<u32>(self+0x124);
 if(n-0x25<7) { call<void>(0x02639C54,icons); n=load<u32>(self+0x124); }
 else if(n-0x2C<7) {
  if(n==0x2C) call<void>(0x02639E4C,icons);
  call<void>(0x02639E50,icons); n=load<u32>(self+0x124);
 } else if(n-0x79<7) { call<void>(0x02639EE0,icons); n=load<u32>(self+0x124); }
 else if(n-0x80<7) { call<void>(0x02639CF0,icons); n=load<u32>(self+0x124); }
 u32 frame=load<u32>(self+0xC4); store<u32>(self+0x124,n+1);
 if(frame==30 || frame==44) { call<void>(0x02030B38,0x100FB444u); frame=load<u32>(self+0xC4); }
 else if(frame==81) { call<void>(0x025E1918,0x8000005Du); frame=load<u32>(self+0xC4); }
 else if(frame==121 || frame==128) { call<void>(0x02030B38,0x100FB430u); frame=load<u32>(self+0xC4); }
 store<u32>(self+0xC4,frame+1);
 if(call<u32>(0x02005840,load<u32>(load<u32>(self+0x44)+0xD4),6u) && call<u32>(0x02639C44,icons) && call<u32>(0x02680610,cell)) call<void>(0x020063C0,self+0x18,0x1049751Cu);
 map=load<u32>(self+0x11C); if(map) call<void>(0x02679D68,map);
 call_ptr<void>(vfn(self,0xB4),self); call_ptr<void>(vfn(self,0x8C),self);
 map=load<u32>(self+0x11C); if(map) call<void>(0x02679D78,map);
}
VERIFY(0x0269A6E4,drawMap);
}
