#include "gabi.h"
using namespace gabi;
namespace hd_screen_dungeon_key {
struct Indices { u8 data[8]; };
static u32 asUnsigned(f32 value) {
 if(value<2147483648.0f) return u32(ftoi(value));
 return u32(ftoi(fsubs_ppc(value,2147483648.0f)))+0x80000000u;
}
static u32 anim(u32 self) { return load<u32>(load<u32>(self+0x44)+0xD4); }
void sparkle(u32 self) {
 WWHD_FUNC(0x02653C18,void,self);
 u32 timer=load<u32>(self+0x5C);
 if(timer) store<u32>(self+0x5C,timer-1);
 else {
  FrameLocal<Indices> indices(0x10); u32 count=0;
  for(u32 i=0;i<2;++i) {
   if(!load<u32>(self+0x50+i*4)) { store<u32>(indices.a+count*4,i); ++count; }
  }
  if(count) {
   f32 random=call<f32>(0x020198D8,f32(f64(count)));
   u32 index=load<u32>(indices.a+asUnsigned(random)*4), field=index<2?self+0x50+index*4:self+0x50;
   store<u32>(field,1);
   call<void>(0x020053E4,anim(self),index+1,load<u32>(0x100EFDC4+(index+1)*4),load<f32>(0x100EFE68));
   random=call<f32>(0x020198D8,f32(f64(s32(load<u32>(self+0x64)))));
   f32 base=f32(f64(s32(load<u32>(self+0x60))));
   store<u32>(self+0x5C,u32(ftoi(fadds_ppc(random,base))));
  }
 }
 for(u32 i=0;i<2;++i) {
  if(load<u32>(self+0x50+i*4) && call<u32>(0x02005840,anim(self),i+1)) store<u32>(self+0x50+i*4,0);
 }
}
VERIFY(0x02653C18,sparkle);
}
