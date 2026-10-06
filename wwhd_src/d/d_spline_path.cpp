#include "gabi.h"
#include "SSystem/SComponent/c_xyz.h"
using gabi::load; using gabi::store;
// HD d2DBSplinePath: unchanged 0x44-byte layout; Calc(Vec*) exposes
// its output/optional-allocation argument explicitly in the verified ABI.
struct SplinePath {
 be<s32> frame,keyCount,duration,state,keyNo,end;
 be<f32> reserved18,step,reserved20,weights[3];
 be<s32> keys[3]; gptr<void> user; be<u32> vtable;
};
static_assert(sizeof(SplinePath)==0x44);
static s32 splineAdd(s32 x,s32 n) { return (s32)((u32)x+(u32)n); }
void splineInit(SplinePath* p,s32 keys,s32 duration) {
 WWHD_FUNC(0x025C0B18,void,p,keys,duration);
 p->frame=0; p->keyCount=keys; p->state=1; p->keyNo=0;
 p->end=duration; p->duration=duration;
 f32 numerator=keys<2?1.0f:(f32)splineAdd(keys,-2);
 s32 end=p->end;
 f32 denominator=end==0?1.0f:(f32)splineAdd(end,-1);
 f32 step=numerator/denominator;
 p->user=nullptr; p->step=step;
}
VERIFY(0x025C0B18,splineInit);
SplinePath* splineConstruct(SplinePath* p) {
 WWHD_FUNC(0x025C0BD0,SplinePath*,p);
 if(!p) p=gabi::call<SplinePath*>(0x0273AD10,0x44);
 if(p) {
  p->keyCount=0; p->end=0; p->vtable=0x100552F4;
  p->reserved18=0.0f; p->reserved20=0.0f; p->state=0;
  p->frame=0; p->duration=0; p->keyNo=0; p->step=0.0f;
  gabi::call(0x028F521C,p->weights,12);
  gabi::call(0x028F521C,p->keys,12);
  p->user=nullptr; splineInit(p,0,0);
 }
 return p;
}
VERIFY(0x025C0BD0,splineConstruct);
bool splineStep(SplinePath* p) {
 WWHD_FUNC(0x025C0C80,bool,p);
 s32 duration=p->duration,frame=p->frame,state;
 if(frame>=duration) { state=0; p->state=0; }
 else {
  state=p->state;
  if(state==1) { p->state=2; state=2; frame=p->frame; }
  if(state==2) {
   s32 end=p->end;
   if(frame>splineAdd(end,-1)) { state=0; p->state=0; }
   else {
    f32 t=gabi::fmuls_ppc(p->step,(f32)frame);
    s32 key=gabi::ftoi(t);
    duration=p->duration; frame=p->frame;
    p->keyNo=key; t=gabi::fsubs_ppc(t,(f32)key);
    if(frame==splineAdd(duration,-1)) p->state=3;
    end=p->end;
    if(frame==splineAdd(end,-1)) { p->state=3; t=1.0f; }
    s32 count=p->keyCount; state=p->state; frame=p->frame;
    f32 tSquared=gabi::fmuls_ppc(t,t);
    key=p->keyNo; f32 inverse=gabi::fsubs_ppc(1.0f,t);
    s32 last=splineAdd(count,-1),next=splineAdd(key,1),second=splineAdd(key,2);
    f32 inverseSquared=gabi::fmuls_ppc(inverse,inverse);
    p->keys[0]=key<last?key:last;
    p->keys[1]=last>=next?next:last;
    f32 weight0=gabi::fmuls_ppc(inverseSquared,0.5f);
    p->weights[0]=weight0; p->frame=splineAdd(frame,1);
    p->keys[2]=second<last?second:last;
    p->weights[1]=gabi::fmadds(t,inverse,0.5f);
    p->weights[2]=gabi::fmuls_ppc(tSquared,0.5f);
   }
  }
 }
 return state==2 || state==3;
}
VERIFY(0x025C0C80,splineStep);
static u32 splineElement(void* values,s32 index,u32 stride) { return gabi::ea(values)+(u32)index*stride; }
cXyz* splineCalcVector(SplinePath* p,cXyz* out,cXyz* values) {
 WWHD_FUNC(0x025C0E38,cXyz*,p,out,values);
 s32 k1=p->keys[1],k0=p->keys[0];
 f32 w1=p->weights[1]; u32 a1=splineElement(values,k1,12);
 u32 a0=splineElement(values,k0,12); f32 x1=load<f32>(a1);
 s32 k2=p->keys[2]; f32 x0=load<f32>(a0),w0=p->weights[0];
 f32 x01=gabi::fmadds(x0,w0,gabi::fmuls_ppc(x1,w1));
 f32 z1=load<f32>(a1+8),y1=load<f32>(a1+4);
 u32 a2=splineElement(values,k2,12); f32 y0=load<f32>(a0+4);
 f32 z01=gabi::fmuls_ppc(z1,w1),y01=gabi::fmuls_ppc(y1,w1);
 f32 x2=load<f32>(a2),z0=load<f32>(a0+8);
 y01=gabi::fmadds(y0,w0,y01); f32 w2=p->weights[2];
 z01=gabi::fmadds(z0,w0,z01); f32 y2=load<f32>(a2+4);
 f32 x=gabi::fmadds(x2,w2,x01); f32 z2=load<f32>(a2+8);
 f32 y=gabi::fmadds(y2,w2,y01),z=gabi::fmadds(z2,w2,z01);
 if(!out) out=gabi::call<cXyz*>(0x0273AD10,12);
 if(out) { out->y=y; out->x=x; out->z=z; }
 return out;
}
VERIFY(0x025C0E38,splineCalcVector);
f32 splineCalcScalar(SplinePath* p,f32* values) {
 WWHD_FUNC(0x025C0F30,f32,p,values);
 s32 k0=p->keys[0],k1=p->keys[1]; f32 w1=p->weights[1]; s32 k2=p->keys[2];
 f32 x0=load<f32>(splineElement(values,k0,4)); f32 x1=load<f32>(splineElement(values,k1,4));
 f32 w0=p->weights[0],t=gabi::fmuls_ppc(x1,w1);
 f32 x2=load<f32>(splineElement(values,k2,4)); t=gabi::fmadds(x0,w0,t);
 f32 w2=p->weights[2]; return gabi::fmadds(x2,w2,t);
}
VERIFY(0x025C0F30,splineCalcScalar);
// Adjacent HD TU initializer, attributed by contiguous spline/stage boundary.
void splineStaticInit() {
 WWHD_FUNC(0x025C0F70,void);
 store<u32>(0x1047D98C,0); store<u32>(0x1047D984,0);
 store<u32>(0x1047D990,0); store<u32>(0x1047D988,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101EBAB0));
 f32 low=load<f32>(0x10055310),high=load<f32>(0x10055314);
 store<f32>(0x1047D978,low); store<f32>(0x1047D97C,high);
 gabi::call(0x028ED6F8,gabi::at<void>(0x1047D980));
 gabi::call(0x028F026C,gabi::at<void>(0x101EBABC));
 gabi::call(0x028EAB2C,gabi::at<void>(0x1047D981));
 gabi::call(0x028F026C,gabi::at<void>(0x101EBAC8));
}
VERIFY(0x025C0F70,splineStaticInit);
