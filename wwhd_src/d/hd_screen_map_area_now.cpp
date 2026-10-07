#include "gabi.h"
using namespace gabi;
static f32 fdivs(f32 a,f32 b) { return f32(f64(a)/f64(b)); }
namespace hd_screen_map_area_now {
template<unsigned N> struct Bytes { u8 data[N]; };
u32 find(u32 cells,u32 coordinate) {
 WWHD_FUNC(0x02680DE0,u32,cells,coordinate);
 u32 x=u32(s32(s8(load<u8>(coordinate))));
 for(u32 i=0;i<49;++i) {
  u32 cell=load<u32>(cells+i*4);
  if(u32(s32(s8(load<u8>(cell+4))))!=x) continue;
  u32 y=u32(s32(s8(load<u8>(coordinate+1))));
  if(u32(s32(s8(load<u8>(cell+5))))==y) return load<u32>(cells+i*4);
 }
 return 0;
}
VERIFY(0x02680DE0,find);
u32 lookup(u32 self,u32 value) {
 WWHD_FUNC(0x02680E68,u32,self,value);
 for(u32 i=0;i<11;++i) if(load<u32>(0x100F91E0+i*4)==value) return i;
 return 0xFFFFFFFFu;
}
VERIFY(0x02680E68,lookup);
void buildGrid(u32 self) {
 WWHD_FUNC(0x026810A8,void,self);
 call<void>(0x02680E98,self); call<void>(0x02680FEC,self);
 f32 originX=load<f32>(self+0xCC),originY=load<f32>(self+0xD8);
 u32 pane=load<u32>(load<u32>(self+0xC4)+0x18);
 u32 sx=call<u32>(0x02704EA8,pane),sy=cpu->r[4];
 f32 scaleX,scaleY; memcpy(&scaleX,&sx,4); memcpy(&scaleY,&sy,4);
 f32 sizeX=fmuls_ppc(load<f32>(self+0xDC),scaleX),sizeY=fmuls_ppc(load<f32>(self+0xE0),scaleY);
 f32 offsetX=load<f32>(0x100F920C),offsetY=offsetX;
 if(load<u32>(self+0x144)==1) { offsetX=fmuls_ppc(load<f32>(self+0x148),scaleX); offsetY=fmuls_ppc(load<f32>(self+0x14C),scaleY); }
 FrameLocal<Bytes<2>> coordinate(8);
 f32 half=load<f32>(0x100F9218);
 for(u32 row=0;row<7;++row) for(u32 col=0;col<7;++col) {
  store<u8>(coordinate.a,u8(s32(col)-3)); store<u8>(coordinate.a+1,u8(s32(row)-3));
  u32 cell=call<u32>(0x02680DE0,self,coordinate.a); if(!cell) continue;
  f32 width=load<f32>(self+0xDC),height=load<f32>(self+0xE0);
  f32 x=fadds_ppc(originX,fmadds(f32(f64(col)),width,fmuls_ppc(width,half)));
  f32 y=fsubs_ppc(originY,fmadds(f32(f64(row)),height,fmuls_ppc(height,half)));
  f32 left=fnmsubs(width,half,x),top=fnmsubs(height,half,y),right=fmadds(width,half,x),bottom=fmadds(height,half,y);
  store<f32>(cell+0x14,x); store<f32>(cell+0x18,y);
  store<f32>(cell+0x1C,load<f32>(self+0xDC)); store<f32>(cell+0x20,load<f32>(self+0xE0));
  store<f32>(cell+0x24,left); store<f32>(cell+0x28,top); store<f32>(cell+0x2C,right); store<f32>(cell+0x30,bottom);
  x=fadds_ppc(fadds_ppc(originX,offsetX),fmadds(f32(f64(col)),sizeX,fmuls_ppc(sizeX,half)));
  y=fsubs_ppc(fadds_ppc(originY,offsetY),fmadds(f32(f64(row)),sizeY,fmuls_ppc(sizeY,half)));
  left=fnmsubs(sizeX,half,x); top=fnmsubs(sizeY,half,y); right=fmadds(sizeX,half,x); bottom=fmadds(sizeY,half,y);
  store<f32>(cell+0x34,x); store<f32>(cell+0x38,y); store<f32>(cell+0x3C,sizeX); store<f32>(cell+0x40,sizeY);
  store<f32>(cell+0x44,left); store<f32>(cell+0x48,top); store<f32>(cell+0x4C,right); store<f32>(cell+0x50,bottom);
  u32 id=load<u32>(cell);
  store<u32>(cell+0x54,load<u32>(self+0xE4));
  store<u32>(cell+0x58,id+load<u32>(self+0xE8)); store<u32>(cell+0x5C,id+load<u32>(self+0xEC));
  store<u32>(cell+0x60,id+load<u32>(self+0xF0)); store<u32>(cell+0x68,id+load<u32>(self+0xF8));
  store<u32>(cell+0x6C,id+load<u32>(self+0xFC)); store<u32>(cell+0x70,id+load<u32>(self+0x100));
  u32 index=call<u32>(0x02680E68,self,id);
  if(s32(index)>=0) { store<u32>(cell+0x64,index+load<u32>(self+0xF4)); store<u32>(cell+0x74,index+load<u32>(self+0x104)); }
 }
}
VERIFY(0x026810A8,buildGrid);
void countDiscovered(u32 self) {
 WWHD_FUNC(0x02681400,void,self);
 store<u32>(self+0x130,0);
 for(u32 i=0;i<49;++i) {
  u32 yes=call<u32>(0x025B8568,load<u32>(0x101F84DC)+0xE4,i);
  u32 cell=load<u32>(self+i*4); store<u8>(cell+0x97,yes?1:0);
  if(yes) store<u32>(self+0x130,load<u32>(self+0x130)+1);
 }
 if(load<u32>(self+0x130)!=load<u32>(self+0x134)) store<u8>(self+0x13C,1);
}
VERIFY(0x02681400,countDiscovered);
}
