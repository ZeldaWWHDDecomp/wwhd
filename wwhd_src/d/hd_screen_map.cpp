#include "gabi.h"
using namespace gabi;
static f32 fdivs(f32 a,f32 b) { return f32(f64(a)/f64(b)); }
namespace hd_screen_map {
template<unsigned N> struct Bytes { u8 data[N]; };
static u32 vfn(u32 self,u32 slot) { return load<u32>(load<u32>(self+4)+slot); }
void zoom(u32 self) {
 WWHD_FUNC(0x0267C34C,void,self);
 f32 t=call<f32>(0x0267BF30,self,u32(load<u8>(self+0x4BC)),load<f32>(self+0x4E8),load<f32>(self+0x4EC));
 f32 scale=load<f32>(self+0x4EC),x=-fmuls_ppc(load<f32>(self+0x4F8),scale),y=-fmuls_ppc(load<f32>(self+0x4FC),scale);
 store<f32>(self+0x4F8,x); store<f32>(self+0x4FC,y);
 call<void>(0x0267C068,self,self+0x4F0,self+0x4F8,u32(load<u8>(self+0x4BC)));
 call<void>(0x0267C188,self,u32(load<u8>(self+0x4BC)),load<f32>(self+0x504),load<f32>(self+0x508),t);
 call_ptr<void>(vfn(self,0x22C),self);
 if(!(load<f32>(0x100F8D18)>t)) {
  u32 map=load<u32>(load<u32>(0x101F8344)+0x218);
  u32 mode=call<u32>(0x02655CE8,load<u32>(map+0x2C));
  call<void>(0x020063C0,self+0x18,mode?0x104960F0u:0x10496060u);
 }
 call_ptr<void>(vfn(self,0x8C),self);
}
VERIFY(0x0267C34C,zoom);
void setPosition(u32 self,u32 position,u32 index) {
 WWHD_FUNC(0x0267C6C4,void,self,position,index);
 u32 cell=call_ptr<u32>(vfn(self,0x1C4),self,index);
 if(!cell) return;
 store<f32>(cell+0x20,load<f32>(position)); store<f32>(cell+0x24,load<f32>(position+4));
 FrameLocal<Bytes<8>> target(8),current(0x10);
 f32 scale=load<f32>(self+0x4EC);
 store<f32>(target.a,-fmuls_ppc(load<f32>(position),scale)); store<f32>(target.a+4,-fmuls_ppc(load<f32>(position+4),scale));
 store<u32>(current.a,load<u32>(cell+0x18)); store<u32>(current.a+4,load<u32>(cell+0x1C));
 call<f32>(0x02689BA0,current.a,target.a,load<f32>(0x100F8D30),load<f32>(0x100F8D34),load<f32>(0x100F8D18));
 store<f32>(cell+0x18,load<f32>(target.a)); store<f32>(cell+0x1C,load<f32>(target.a+4));
}
VERIFY(0x0267C6C4,setPosition);
}
