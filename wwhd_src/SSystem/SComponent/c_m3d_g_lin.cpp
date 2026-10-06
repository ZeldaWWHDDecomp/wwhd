/* WWHD c_m3d_g_lin: the HD translation unit of cM3dGLin (line segment), linked between
 * c_m3d_g_cyl and c_m3d_g_pla. In HD the header-inline methods are emitted out of line here.
 * (decompiled from the game; see wwhd_src/README.md). */
#include "wwhd.h"
#include "gabi.h"
struct M3dGLinVec_l { be<f32> x,y,z; };
WWHD_SIZE(M3dGLinVec_l,12);
namespace c_m3d_g_lin {
/* cM3dGLin::cM3dGLin(const cXyz& start, const cXyz& end) (GHS form: allocates when this==0) */
u32 ctor(u32 self,u32 start,u32 end) {
 WWHD_FUNC(0x02018780,u32,self,start,end);
 if(!self){self=gabi::call<u32>(0x0273AD10,0x1Cu);if(!self)return 0;}
 gabi::store<u32>(self+0x18,0x10003374);
 gabi::store<u32>(self+0,gabi::load<u32>(start+0));gabi::store<u32>(self+4,gabi::load<u32>(start+4));gabi::store<u32>(self+8,gabi::load<u32>(start+8));
 gabi::store<u32>(self+0xC,gabi::load<u32>(end+0));gabi::store<u32>(self+0x10,gabi::load<u32>(end+4));gabi::store<u32>(self+0x14,gabi::load<u32>(end+8));
 return self;
}
VERIFY(0x02018780,ctor);
/* cM3dGLin::SetStartEnd(const cXyz&, const cXyz&): cXyz copy assignment (word copies) */
void SetStartEnd_cXyz(u32 self,u32 start,u32 end) {
 WWHD_FUNC(0x02018808,void,self,start,end);
 gabi::store<u32>(self+0,gabi::load<u32>(start+0));gabi::store<u32>(self+4,gabi::load<u32>(start+4));gabi::store<u32>(self+8,gabi::load<u32>(start+8));
 gabi::store<u32>(self+0xC,gabi::load<u32>(end+0));gabi::store<u32>(self+0x10,gabi::load<u32>(end+4));gabi::store<u32>(self+0x14,gabi::load<u32>(end+8));
}
VERIFY(0x02018808,SetStartEnd_cXyz);
/* cM3dGLin::SetStartEnd(const Vec&, const Vec&): cXyz::set(Vec), lfs/stfs pairs. In the recompiled
 * original a direct lfs->stfs pair keeps the bits (the host folds the single->double->single round
 * trip, so a signalling NaN is not quieted); fcopy models that: bit copy, marked as a float store. */
static void fcopy(u32 dst,u32 src) { u32 v=gabi::load<u32>(src); f32 f; memcpy(&f,&v,4); gabi::store<f32>(dst,f); }
void SetStartEnd_Vec(u32 self,u32 start,u32 end) {
 WWHD_FUNC(0x0201883C,void,self,start,end);
 fcopy(self+0,start+0);fcopy(self+4,start+4);fcopy(self+8,start+8);
 fcopy(self+0xC,end+0);fcopy(self+0x10,end+4);fcopy(self+0x14,end+8);
}
VERIFY(0x0201883C,SetStartEnd_Vec);
/* cM3dGLin::SetEnd(const cXyz&) */
void SetEnd(u32 self,u32 pos) {
 WWHD_FUNC(0x02018870,void,self,pos);
 gabi::store<u32>(self+0xC,gabi::load<u32>(pos+0));gabi::store<u32>(self+0x10,gabi::load<u32>(pos+4));gabi::store<u32>(self+0x14,gabi::load<u32>(pos+8));
}
VERIFY(0x02018870,SetEnd);
/* cM3dGLin::CalcPos(Vec* out, f32 scale) const: out = start + (end - start) * scale */
void CalcPos(u32 self,u32 out,f32 scale) {
 WWHD_FUNC(0x0201888C,void,self,out,scale);
 gabi::Local<M3dGLinVec_l> tmp;
 gabi::call<void>(0x028E8DAC,gabi::at<void>(self+0xC),gabi::at<void>(self),tmp.get());
 gabi::call<void>(0x028E8E64,tmp.get(),tmp.get(),scale);
 gabi::call<void>(0x028E8D88,tmp.get(),gabi::at<void>(self),gabi::at<void>(out));
}
VERIFY(0x0201888C,CalcPos);
/* TU static initializer (header statics only) */
void hd_static_init_0201890C() {
 WWHD_FUNC(0x0201890C,void);
 gabi::store<u32>(0x101FF940,0);gabi::store<u32>(0x101FF938,0);gabi::store<u32>(0x101FF944,0);gabi::store<u32>(0x101FF93C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C964));
 f32 negativePi=gabi::load<f32>(0x1000338C),positivePi=gabi::load<f32>(0x10003390);
 gabi::store<f32>(0x101FF92C,negativePi);gabi::store<f32>(0x101FF930,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF934));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C970));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF935));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C97C));
}
VERIFY(0x0201890C,hd_static_init_0201890C);
}
