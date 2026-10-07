#include "gabi.h"
using namespace gabi;
static f32 fdivs(f32 a,f32 b) { return f32(f64(a)/f64(b)); }
namespace hd_screen_map_display {
static f32 distance(f32 squared) {
 f32 zero=load<f32>(0x100F997C);
 if(!(squared>zero)) return fmuls_ppc(zero,squared);
 f64 inv=frsqrte(squared);
 f32 a=f32(inv*round25(inv)), half=f32(inv*f64(load<f32>(0x100F9A2C)));
 return fmuls_ppc(fmuls_ppc(fnmsubs(a,squared,load<f32>(0x100F9A48)),half),squared);
}
f32 chase(u32 current,u32 target,f32 rate,f32 maximum,f32 minimum) {
 WWHD_FUNC(0x02689BA0,f32,current,target,rate,maximum,minimum);
 f32 x=load<f32>(current), tx=load<f32>(target), y,ty;
 bool same=x==tx;
 if(same) { y=load<f32>(current+4); ty=load<f32>(target+4); same=y==ty; }
 if(!same) {
  x=load<f32>(current); tx=load<f32>(target); y=load<f32>(current+4); ty=load<f32>(target+4);
  f32 dx=fsubs_ppc(x,tx),dy=fsubs_ppc(y,ty);
  f32 length=distance(fmadds(dx,dx,fmuls_ppc(dy,dy)));
  bool snap=length<minimum;
  if(!snap) {
   f32 step=fmuls_ppc(length,rate),sx=fmuls_ppc(dx,rate),sy=fmuls_ppc(dy,rate);
   if(step==load<f32>(0x100F997C)) snap=true;
   else {
    if(step>maximum) { f32 scale=fdivs(maximum,step); sy=fmuls_ppc(sy,scale); sx=fmuls_ppc(sx,scale); }
    else if(step<minimum) { f32 scale=fdivs(minimum,step); sy=fmuls_ppc(sy,scale); sx=fmuls_ppc(sx,scale); }
    x=fsubs_ppc(load<f32>(current),sx); y=fsubs_ppc(load<f32>(current+4),sy);
    store<f32>(current,x); store<f32>(current+4,y); ty=load<f32>(target+4); tx=load<f32>(target);
   }
  }
  if(snap) { x=load<f32>(target); store<f32>(current,x); y=load<f32>(target+4); store<f32>(current+4,y); tx=load<f32>(target); ty=load<f32>(target+4); }
 }
 f32 dx=fsubs_ppc(x,tx),dy=fsubs_ppc(y,ty);
 return distance(fmadds(dx,dx,fmuls_ppc(dy,dy)));
}
VERIFY(0x02689BA0,chase);
}
