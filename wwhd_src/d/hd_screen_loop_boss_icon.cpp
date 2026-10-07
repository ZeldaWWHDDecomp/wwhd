#include "gabi.h"
using namespace gabi;
namespace hd_screen_loop_boss_icon {
static u32 asUnsigned(f32 value,f32 threshold) {
 if(value<threshold) return u32(ftoi(value));
 return u32(ftoi(fsubs_ppc(value,threshold)))+0x80000000u;
}
void update(u32 self) {
 WWHD_FUNC(0x0267468C,void,self);
 f32 rate=load<f32>(0x100F7200), threshold=load<f32>(0x100F7210);
 u32 n=load<u32>(self+0x50)-1; store<u32>(self+0x50,n);
 if(!n) {
  f32 random=call<f32>(0x020198D8,load<f32>(0x100F7208));
  store<u32>(self+0x50,asUnsigned(fadds_ppc(random,load<f32>(0x100F720C)),threshold));
  call<void>(0x020053E4,load<u32>(load<u32>(self+0x44)+0xD4),0u,0u,rate);
 }
 n=load<u32>(self+0x54)-1; store<u32>(self+0x54,n);
 if(!n) {
  f32 random=call<f32>(0x020198D8,load<f32>(0x100F7214));
  store<u32>(self+0x54,asUnsigned(fadds_ppc(fadds_ppc(random,random),load<f32>(0x100F7218)),threshold));
  call<void>(0x020053E4,load<u32>(load<u32>(self+0x44)+0xD4),1u,1u,rate);
 }
 call_ptr<void>(load<u32>(load<u32>(self+4)+0x8C),self);
}
VERIFY(0x0267468C,update);
}
