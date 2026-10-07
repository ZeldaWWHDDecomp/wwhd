#include "gabi.h"
using namespace gabi;
static f32 fdivs(f32 a,f32 b) { return f32(f64(a)/f64(b)); }
namespace hd_screen_dungeon_map_boss {
template<unsigned N> struct Bytes { u8 data[N]; };
void update(u32 self) {
 WWHD_FUNC(0x0265FCB8,void,self);
 u32 map=load<u32>(self+0x90),pane;
 if(map && (pane=load<u32>(self+0x94)) && load<u32>(map+0x4B8)) {
  u32 ui=load<u32>(load<u32>(0x101F8344)+0x218); s32 index=s32(load<u32>(ui+0x50));
  f32 zero=load<f32>(0x100F0A88),x=zero,y=zero;
  if(index>=0) { x=load<f32>(0x104926EC+u32(index)*8); y=load<f32>(0x104926F0+u32(index)*8); }
  f32 scale=load<f32>(pane+0x2C);
  FrameLocal<Bytes<8>> point(8),origin(0x10);
  store<f32>(point.a,fmuls_ppc(x,scale)); store<f32>(point.a+4,fmuls_ppc(y,scale));
  store<f32>(origin.a,zero); store<f32>(origin.a+4,zero);
  call<void>(0x026893C8,point.a,point.a,origin.a,load<f32>(map+0x4AC));
  store<f32>(self+0x70,load<f32>(point.a)); store<f32>(self+0x74,load<f32>(point.a+4));
  map=load<u32>(self+0x90); store<f32>(self+0x98,load<f32>(map+0x4E0)); store<f32>(self+0x9C,load<f32>(map+0x4E4));
 }
 call<void>(0x02006364,self+0x18);
}
VERIFY(0x0265FCB8,update);
}
