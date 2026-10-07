#include "gabi.h"
using namespace gabi;
static f32 fdivs(f32 a,f32 b) { return f32(f64(a)/f64(b)); }
namespace hd_screen_dungeon_map {
template<unsigned N> struct Bytes { u8 data[N]; };
static u32 vfn(u32 self,u32 slot) { return load<u32>(load<u32>(self+4)+slot); }
static f32 clamp(f32 x,f32 lo,f32 hi) { if(x<lo) return lo; if(x>hi) return hi; return x; }
void bounds(u32 self,u32 output) {
 WWHD_FUNC(0x0265A3BC,void,self,output);
 u32 pane=load<u32>(self+0x50); if(!pane) return;
 FrameLocal<Bytes<12>> origin(0x40);
 call<void>(0x02704DE4,origin.a,pane);
 f32 x=load<f32>(origin.a),y=load<f32>(origin.a+4);
 u32 layout=load<u32>(load<u32>(self+0x44)+4);
 f32 screenX=load<f32>(layout+0x14),screenY=load<f32>(layout+0x18);
 f32 paneX=load<f32>(pane+0x3C),paneY=load<f32>(pane+0x40);
 f32 width=f32(f64(call<u32>(0x02739594)));
 f32 height=f32(f64(call<u32>(0x027395E4)));
 f32 ratioX=fdivs(width,screenX),ratioY=fdivs(height,screenY),half=load<f32>(0x100F0254),zero=load<f32>(0x100F0250);
 x=fmuls_ppc(x,ratioX); y=fmuls_ppc(y,ratioY); paneX=fmuls_ppc(paneX,ratioX); paneY=fmuls_ppc(paneY,ratioY);
 f32 centerX=fadds_ppc(x,fmuls_ppc(width,half)),centerY=fmadds(height,half,y);
 f32 hx=fmuls_ppc(paneX,half),hy=fmuls_ppc(paneY,half);
 f32 left=clamp(fsubs_ppc(centerX,hx),zero,width),top=clamp(fsubs_ppc(centerY,hy),zero,height);
 f32 right=clamp(fadds_ppc(centerX,hx),zero,width),bottom=clamp(fadds_ppc(centerY,hy),zero,height);
 store<f32>(output+4,top); store<f32>(output+12,bottom); store<f32>(output+8,right); store<f32>(output,left);
}
VERIFY(0x0265A3BC,bounds);
void updateParts(u32 self) {
 WWHD_FUNC(0x02657790,void,self);
 call<void>(0x02656830,self); call_ptr<void>(vfn(self,0xB4),self); call_ptr<void>(vfn(self,0x8C),self);
 call<void>(0x026568CC,self); call<void>(0x026573F0,self); call<void>(0x02657440,self);
}
VERIFY(0x02657790,updateParts);
void updateMarkers(u32 self) {
 WWHD_FUNC(0x0265799C,void,self);
 u32 ui=load<u32>(load<u32>(0x101F8344)+0x218);
 if(!call<u32>(0x0268936C,ui) && load<u8>(self+0x924)) { call<void>(0x02657848,self); call<void>(0x02657914,self); }
}
VERIFY(0x0265799C,updateMarkers);
void drag(u32 self) {
 WWHD_FUNC(0x0265ADB4,void,self);
 if(!load<u32>(self+0x920)) { call<void>(0x020063C0,self+0x18,0x10492410u); return; }
 FrameLocal<Bytes<40>> event(0x20);
 u32 input=load<u32>(0x101F4FF0),stick=load<u32>(0x101F5088)+0x138;
 call<void>(0x020013C0,event.a,input+0x30);
 if(load<u32>(event.a)==0x44 && load<u32>(event.a+4)==load<u32>(load<u32>(self+0x9C)+0x10)) {
  FrameLocal<Bytes<8>> position(0x10);
  u32 x=call<u32>(0x026151A8,load<u32>(0x101F4FF0)),y=cpu->r[4];
  store<u32>(position.a,x); store<u32>(position.a+4,y);
  call<void>(0x0267D5D8,load<u32>(self+0x920),position.a);
 } else {
  f32 y=load<f32>(stick+4),x=load<f32>(stick),zero=load<f32>(0x100F0250);
  f32 squared=fmadds(x,x,fmuls_ppc(y,y)),length;
  if(squared>zero) {
   f64 inv=frsqrte(squared);
   f32 correction=fnmsubs(f32(inv*round25(inv)),squared,load<f32>(0x100F0258));
   length=fmuls_ppc(fmuls_ppc(correction,f32(inv*f64(load<f32>(0x100F0254)))),squared);
  } else length=fmuls_ppc(zero,squared);
  bool active=length>zero;
  if(active) active=load<u8>(load<u32>(0x101F8378)+0x14)!=0;
  if(active) active=call<u32>(0x02615140,load<u32>(0x101F4FF0))!=0;
  if(active) {
   u32 mode=call<u32>(0x02615140,load<u32>(0x101F4FF0));
   active=load<u32>(mode+0x10)==load<u32>(load<u32>(self+0x9C)+0x10);
  }
  if(!active) call<void>(0x020063C0,self+0x18,0x10492410u);
  else {
   f32 factor=load<f32>(0x100F0334),dx=fmuls_ppc(load<f32>(stick),factor),dy=fmuls_ppc(load<f32>(stick+4),factor);
   f32 oldX=load<f32>(self+0x3F4),oldY=load<f32>(self+0x3F8);
   store<f32>(self+0x3F4,fsubs_ppc(oldX,dx)); store<f32>(self+0x3F8,fsubs_ppc(oldY,dy));
   call<void>(0x0267D5D8,load<u32>(self+0x920),self+0x3F4);
  }
 }
 call<void>(0x02657790,self); call<void>(0x0265799C,self);
}
VERIFY(0x0265ADB4,drag);
}
