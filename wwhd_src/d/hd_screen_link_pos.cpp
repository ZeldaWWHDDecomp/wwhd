#include "gabi.h"
using namespace gabi;
static f32 fdivs(f32 a,f32 b) { return f32(f64(a)/f64(b)); }
namespace hd_screen_link_pos {
void grid(u32 self,u32 pane,u32 size,u32 positions) {
 WWHD_FUNC(0x02684698,void,self,pane,size,positions);
 f32 width=fdivs(load<f32>(pane+0x3C),load<f32>(0x100F93D8));
 f32 height=fdivs(load<f32>(pane+0x40),load<f32>(0x100F93D8));
 store<f32>(size,width); store<f32>(size+4,height);
 f32 half=load<f32>(0x100F93DC),left=-fmuls_ppc(load<f32>(pane+0x3C),half),top=fmuls_ppc(load<f32>(pane+0x40),half);
 for(u32 row=0;row<7;++row) for(u32 col=0;col<7;++col) {
  f32 x=fadds_ppc(left,fmadds(f32(f64(col)),width,fmuls_ppc(width,half)));
  f32 y=fsubs_ppc(top,fmadds(f32(f64(row)),height,fmuls_ppc(height,half)));
  u32 out=positions+(row*7+col)*8; store<f32>(out,x); store<f32>(out+4,y);
 }
}
VERIFY(0x02684698,grid);
}
