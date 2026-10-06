#include "gabi.h"
#include <cmath>
using gabi::load; using gabi::store;
// Preserve the reference's addend-first NaN selection when overlapping output
// words make a payload observable; finite arithmetic still uses fmadds.
static f32 circle_fma(f32 a,f32 factor,f32 addend) {
 if(std::isnan(addend)) return gabi::ppc_qnan(addend);
 if(std::isnan(a)) return gabi::ppc_qnan(a);
 if(std::isnan(factor)) return gabi::ppc_qnan(factor);
 return gabi::fmadds(a,factor,addend);
}
void cM2d_CrossCirLin(void* circle,f32 x0,f32 y0,f32 x1,f32 y1,f32* outX,f32* outY) {
 WWHD_FUNC(0x02010284,void,circle,x0,y0,x1,y1,outX,outY);
 f32 cy=load<f32>(gabi::ea(circle)+4);
 f32 dy=gabi::fsubs_ppc(y0,cy);
 f32 cx=load<f32>(gabi::ea(circle));
 f32 dx=gabi::fsubs_ppc(x0,cx);
 f32 distance=circle_fma(dx,dx,gabi::fmuls_ppc(dy,dy));
 f32 radius=load<f32>(gabi::ea(circle)+8);
 f32 c=f32(-(f64(radius)*f64(radius)-f64(distance)));
 f32 zero=load<f32>(0x10001EFC);
 f32 dot=circle_fma(x1,dx,gabi::fmuls_ppc(y1,dy));
 f32 b=dot+dot;
 f32 a=circle_fma(x1,x1,gabi::fmuls_ppc(y1,y1));
 if(!(c<zero)) gabi::call<void>(0x0273AA24,gabi::at<void>(0x10001F0C),0x47,gabi::at<void>(0x10001F24));
 f32 epsilon=load<f32>(0x100030B8);
 f32 t=zero;
 if(std::fabs(a)<epsilon) {
  if(!(std::fabs(b)<epsilon)) t=-(c/b);
 } else {
  f32 four=load<f32>(0x10001F04);
  f32 ac=(four*a)*c;
  f32 discriminant=gabi::fmsubs(b,b,ac);
  if(std::fabs(discriminant)<epsilon) t=-(b/(a+a));
  else if(!(discriminant<zero)) {
   f32 one=load<f32>(0x10001F08);
   f32 reciprocal=one/(a+a);
   f64 root1=gabi::call<f64>(0x028F4384,discriminant);
   f32 first=gabi::fmuls_ppc(f32(root1-f64(b)),reciprocal);
   f64 root2=gabi::call<f64>(0x028F4384,discriminant);
   f64 negativeB=-f64(b);
   f32 second=gabi::fmuls_ppc(f32(std::isnan(negativeB) ? negativeB : std::isnan(root2) ? root2 : negativeB-root2),reciprocal);
   t=ppc_fsel(first-second,second,first);
  }
 }
 if(!(std::fabs(t)<epsilon)) {
  if(t<zero) gabi::call<void>(0x0273AA24,gabi::at<void>(0x10001F0C),0x8D,gabi::at<void>(0x10001F18));
  y0=std::isnan(y0) ? gabi::ppc_qnan(y0) : std::isnan(t) ? gabi::ppc_qnan(t) : gabi::fmadds(t,y1,y0);
  x0=std::isnan(x0) ? gabi::ppc_qnan(x0) : std::isnan(t) ? gabi::ppc_qnan(t) : gabi::fmadds(t,x1,x0);
 }
 store<f32>(gabi::ea(outX),x0);
 store<f32>(gabi::ea(outY),y0);
}
VERIFY(0x02010284,cM2d_CrossCirLin);
// HD math/header globals and their registration records.
void cM2d_StaticInit() {
 WWHD_FUNC(0x02010500,void);
 store<u32>(0x101FF844,0);
 store<u32>(0x101FF83C,0);
 store<u32>(0x101FF848,0);
 store<u32>(0x101FF840,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C820));
 f32 first=load<f32>(0x10001F30);
 f32 second=load<f32>(0x10001F34);
 store<f32>(0x101FF830,first);
 store<f32>(0x101FF834,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF838));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C82C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF839));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C838));
}
VERIFY(0x02010500,cM2d_StaticInit);
