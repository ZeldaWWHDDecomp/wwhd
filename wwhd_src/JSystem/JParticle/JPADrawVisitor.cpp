/** JPA1 draw visitor calculation companion; merged scale/alpha visitors retain their own TU. */
#include "JSystem/JParticle/JPADrawVisitor.h"
#include "sead/ElementAt.h"
using namespace jpa_drawvisitor;
namespace jpa_drawvisitor {
static DrawParams* params(JPABaseParticle* p){return reinterpret_cast<DrawParams*>(p->mDrawParams);}
static s32 quotient(s32 a,s32 b){return !b || (a==INT32_MIN && b==-1) ? (a<0?-1:0) : a/b;}
static s32 remainder(s32 a,s32 b){return (s32)((u32)a-(u32)quotient(a,b)*(u32)b);}
static void storeColorWord(be<u32>* dst,f32 value){u32 bits;std::memcpy(&bits,&value,sizeof(bits));*dst=bits;}
using StripePipelinePtr = gptr<void>;
static StripePipelinePtr sStripePipeline;
/* 02830418 */
void dirTypeVel(JPABaseParticle* ptcl,Emitter* emtr,Vec* out){
 out->x=*reinterpret_cast<be<u32>*>(&ptcl->mVelocity[0]);out->y=*reinterpret_cast<be<u32>*>(&ptcl->mVelocity[1]);out->z=*reinterpret_cast<be<u32>*>(&ptcl->mVelocity[2]);
}
/* 02830434 */
void dirTypePos(JPABaseParticle* ptcl,Emitter* emtr,Vec* out){
 out->x=*reinterpret_cast<be<u32>*>(&ptcl->mLocalPosition[0]);out->y=*reinterpret_cast<be<u32>*>(&ptcl->mLocalPosition[1]);out->z=*reinterpret_cast<be<u32>*>(&ptcl->mLocalPosition[2]);
}
/* 02830450 */
void dirTypePosInv(JPABaseParticle* ptcl,Emitter* emtr,Vec* out){
 f32 x=ptcl->mLocalPosition[0];out->x=*reinterpret_cast<be<u32>*>(&ptcl->mLocalPosition[0]);
 f32 y=ptcl->mLocalPosition[1];out->y=*reinterpret_cast<be<u32>*>(&ptcl->mLocalPosition[1]);
 f32 z=ptcl->mLocalPosition[2];
 *reinterpret_cast<be<f32>*>(&out->x)=-x;*reinterpret_cast<be<f32>*>(&out->y)=-y;*reinterpret_cast<be<f32>*>(&out->z)=-z;
}
/* 02830480 */
void dirTypeEmtrDir(JPABaseParticle* ptcl,Emitter* emtr,Vec* out){
 s32 view=ptcl->mViewIndex;
 u32 group=JPADrawEmitterGroup(emtr);
 if(view!=-1) group=view;
 auto* position=jpa_emitter_calc::emtrInfo.mViewPosition[group];
 out->x=*reinterpret_cast<be<u32>*>(&position[0]);out->y=*reinterpret_cast<be<u32>*>(&position[1]);out->z=*reinterpret_cast<be<u32>*>(&position[2]);
}
/* 028305DC */
void rotTypeY(f32 sin,f32 cos,Matrix* out){
 out->m[0][0]=cos;
 out->m[0][3]=0.0f;
 out->m[0][1]=0.0f;
 out->m[1][0]=0.0f;
 out->m[1][3]=0.0f;
 out->m[0][2]=-sin;
 out->m[1][2]=0.0f;
 out->m[2][1]=0.0f;
 out->m[2][0]=sin;
 out->m[2][3]=0.0f;
 out->m[2][2]=cos;
 out->m[1][1]=1.0f;
}
/* 02830624 */
void rotTypeX(f32 sin,f32 cos,Matrix* out){
 out->m[2][1]=sin;
 out->m[2][2]=cos;
 out->m[1][1]=cos;
 out->m[2][3]=0.0f;
 out->m[0][2]=0.0f;
 out->m[0][3]=0.0f;
 out->m[1][3]=0.0f;
 out->m[1][2]=-sin;
 out->m[2][0]=0.0f;
 out->m[0][1]=0.0f;
 out->m[1][0]=0.0f;
 out->m[0][0]=1.0f;
}
/* 0283066C */
void rotTypeZ(f32 sin,f32 cos,Matrix* out){
 out->m[1][0]=sin;
 out->m[1][1]=cos;
 out->m[0][1]=-sin;
 out->m[2][0]=0.0f;
 out->m[1][2]=0.0f;
 out->m[0][2]=0.0f;
 out->m[2][3]=0.0f;
 out->m[2][1]=0.0f;
 out->m[1][3]=0.0f;
 out->m[0][3]=0.0f;
 out->m[0][0]=cos;
 out->m[2][2]=1.0f;
}
/* 028306B4 */
void rotTypeXYZ(f32 sin,f32 cos,Matrix* out){
 f32 a=(1.0f-cos)*0.33333298563957214f;
 f32 b=sin*0.5773500204086304f;
 f32 plus=gabi::fadds_ppc(a,b),minus=gabi::fsubs_ppc(a,b),diag=a+cos;
 out->m[2][3]=0;out->m[1][3]=0;out->m[0][3]=0;
 out->m[2][1]=plus;out->m[0][1]=minus;out->m[2][0]=minus;
 out->m[0][0]=diag;out->m[1][0]=plus;out->m[1][1]=diag;
 out->m[0][2]=plus;out->m[2][2]=diag;out->m[1][2]=minus;
}
/* 02830720 */
void rotTypeYJiggle(f32 sin,f32 cos,Matrix* out){
 out->m[0][0]=cos;out->m[2][0]=sin;
 out->m[1][1]=0.9781479835510254f;out->m[1][0]=0;out->m[0][3]=0;
 out->m[2][2]=cos*0.9781479835510254f;out->m[1][2]=-0.20791199803352356f;
 out->m[2][3]=0;out->m[1][3]=0;out->m[0][1]=-(sin*0.20791199803352356f);
 out->m[2][1]=cos*0.20791199803352356f;out->m[0][2]=-(sin*0.9781479835510254f);
}
/* 0283078C */
void basePlaneTypeXY(f32 x0,f32 x1,f32 y0,f32 y1,Matrix* out){
 out->m[1][0]=y0;out->m[2][1]=x0;out->m[0][3]=x1;out->m[0][2]=0;out->m[2][3]=0;out->m[2][0]=0;
 out->m[1][3]=y1;out->m[0][0]=x0;out->m[2][2]=y1;out->m[1][2]=x1;out->m[0][1]=y0;out->m[1][1]=0;
}
/* 028307C8 */
void basePlaneTypeXZ(f32 x0,f32 x1,f32 y0,f32 y1,Matrix* out){
 out->m[2][1]=x0;out->m[1][1]=y1;out->m[2][3]=y0;out->m[2][2]=0;out->m[1][3]=0;out->m[1][0]=0;
 out->m[1][2]=x1;out->m[0][2]=y1;out->m[0][3]=x1;out->m[2][0]=y0;out->m[0][0]=x0;out->m[0][1]=0;
}
/* 02830804 */
JPAParticleLink* nextParticle(JPAParticleLink* link){return link->mpNext;}
/* 0283080C */
JPAParticleLink* previousParticle(JPAParticleLink* link){return link->mpPrev;}
/* 02830D80 */
void JPADrawCalcColorPrm::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();u32 target=b->__vtbl->getPrmColor;s32 frame=dc->pcb->mColorAnmFrame;
 u32 value=gabi::call_ptr<u32>(target,b,frame);params(p)->mPrmColor=value;
}
/* 02830DE0 */
void JPADrawCalcColorEnv::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();u32 target=b->__vtbl->getEnvColor;s32 frame=dc->pcb->mColorAnmFrame;
 u32 value=gabi::call_ptr<u32>(target,b,frame);params(p)->mEnvColor=value;
}
/* 02830E40 */
void JPADrawCalcColorAnmFrameNormal::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();u32 target=b->__vtbl->getColorRegAnmMaxFrm;s32 age=gabi::ftoi((f32)p->mCurFrame);
 s32 max=gabi::call_ptr<s32>(target,b);
 if(age<max) dc->pcb->mColorAnmFrame=gabi::ftoi((f32)p->mCurFrame);
 else {b=dc->pbsp.get();max=gabi::call_ptr<s32>(b->__vtbl->getColorRegAnmMaxFrm,b);dc->pcb->mColorAnmFrame=max;}
}
/* 02830EF0 */
void JPADrawCalcColorAnmFrameRepeat::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();auto* vt=b->__vtbl.get();auto* clip=dc->pcb.get();
 u32 mask=gabi::call_ptr<u32>(vt->getColorLoopOffset,b);
 s32 age=gabi::ftoi((f32)p->mCurFrame);u32 target=vt->getColorRegAnmMaxFrm;
 s32 offset=(u32)params(p)->mLoopOffset&mask;b=dc->pbsp.get();s32 time=(u32)offset+(u32)age;
 s32 max=gabi::call_ptr<s32>(target,b);clip->mColorAnmFrame=remainder(time,(u32)max+1);
}
/* 02830F98 */
void JPADrawCalcColorAnmFrameReverse::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();u32 mask=gabi::call_ptr<u32>(b->__vtbl->getColorLoopOffset,b);
 b=dc->pbsp.get();u32 target=b->__vtbl->getColorRegAnmMaxFrm;s32 offset=(u32)params(p)->mLoopOffset&mask;
 s32 max=gabi::call_ptr<s32>(target,b);s32 time=(u32)offset+(u32)gabi::ftoi((f32)p->mCurFrame);
 s32 q=quotient(time,max);u32 rem=(u32)time-(u32)q*(u32)max,odd=(u32)q&1;
 dc->pcb->mColorAnmFrame=odd*(u32)max+rem-2*odd*rem;
}
/* 02831054 */
void JPADrawCalcColorAnmFrameMerge::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();u32 mask=gabi::call_ptr<u32>(b->__vtbl->getColorLoopOffset,b);
 b=dc->pbsp.get();u32 target=b->__vtbl->getColorRegAnmMaxFrm;s32 offset=(u32)params(p)->mLoopOffset&mask;
 s32 max=gabi::call_ptr<s32>(target,b);s32 span=(u32)max+1;
 s32 time=gabi::ftoi(gabi::fmadds((f32)span,(f32)p->mCurNormTime,(f32)offset));
 dc->pcb->mColorAnmFrame=remainder(time,span);
}
/* 02831134 */
void JPADrawCalcColorAnmFrameRandom::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();auto* vt=b->__vtbl.get();auto* clip=dc->pcb.get();u32 mask=gabi::call_ptr<u32>(vt->getColorLoopOffset,b);
 u32 target=vt->getColorRegAnmMaxFrm;s32 offset=(u32)params(p)->mLoopOffset&mask;b=dc->pbsp.get();
 s32 max=gabi::call_ptr<s32>(target,b);clip->mColorAnmFrame=remainder(offset,(u32)max+1);
}
/* 02830DD8 */
static void JPADrawCalcColorPrm_adjusted(JPADrawCalcColorPrm* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcColorPrm*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 02830E38 */
static void JPADrawCalcColorEnv_adjusted(JPADrawCalcColorEnv* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcColorEnv*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 02830EE8 */
static void JPADrawCalcColorAnmFrameNormal_adjusted(JPADrawCalcColorAnmFrameNormal* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcColorAnmFrameNormal*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 02830F90 */
static void JPADrawCalcColorAnmFrameRepeat_adjusted(JPADrawCalcColorAnmFrameRepeat* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcColorAnmFrameRepeat*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 0283104C */
static void JPADrawCalcColorAnmFrameReverse_adjusted(JPADrawCalcColorAnmFrameReverse* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcColorAnmFrameReverse*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 0283112C */
static void JPADrawCalcColorAnmFrameMerge_adjusted(JPADrawCalcColorAnmFrameMerge* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcColorAnmFrameMerge*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 028311C0 */
static void JPADrawCalcColorAnmFrameRandom_adjusted(JPADrawCalcColorAnmFrameRandom* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcColorAnmFrameRandom*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 028311C8 */
void JPADrawCalcTextureAnmIndexNormal::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();auto* vt=b->__vtbl.get();s32 max=gabi::call_ptr<s32>(vt->getTextureAnmKeyNum,b);
 s32 age=gabi::ftoi((f32)p->mCurFrame),idx=age;
 if((s32)((u32)max-1)<age){b=dc->pbsp.get();idx=(u32)gabi::call_ptr<s32>(b->__vtbl->getTextureAnmKeyNum,b)-1;}
 b=dc->pbsp.get();u32 index=gabi::call_ptr<u32>(vt->getTextureIndexFrame,b,(u8)idx);
 params(p)->mTexIdx=sead::elementAt(dc->pTexIdx,index);
}
/* 02831280 */
void JPADrawCalcTextureAnmIndexRepeat::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();auto* vt=b->__vtbl.get();u32 mask=gabi::call_ptr<u32>(vt->getTextureLoopOffset,b);
 s32 age=gabi::ftoi((f32)p->mCurFrame);u32 target=vt->getTextureAnmKeyNum;s32 offset=(u32)params(p)->mLoopOffset&mask;
 s32 time=(u32)offset+(u32)age;auto* object=dc->pbsp.get();s32 max=gabi::call_ptr<s32>(target,object);
 u32 index=gabi::call_ptr<u32>(vt->getTextureIndexFrame,b,(u8)remainder(time,max));params(p)->mTexIdx=sead::elementAt(dc->pTexIdx,index);
}
/* 0283132C */
void JPADrawCalcTextureAnmIndexReverse::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();u32 mask=gabi::call_ptr<u32>(b->__vtbl->getTextureLoopOffset,b);
 b=dc->pbsp.get();u32 target=b->__vtbl->getTextureAnmKeyNum;s32 offset=(u32)params(p)->mLoopOffset&mask;
 s32 max=(u32)gabi::call_ptr<s32>(target,b)-1;s32 time=(u32)offset+(u32)gabi::ftoi((f32)p->mCurFrame);
 s32 q=quotient(time,max);u32 rem=(u32)time-(u32)q*(u32)max,odd=(u32)q&1;
 b=dc->pbsp.get();u32 index=gabi::call_ptr<u32>(b->__vtbl->getTextureIndexFrame,b,(u8)(odd*(u32)max+rem-2*odd*rem));params(p)->mTexIdx=sead::elementAt(dc->pTexIdx,index);
}
/* 0283140C */
void JPADrawCalcTextureAnmIndexMerge::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();s32 max=gabi::call_ptr<s32>(b->__vtbl->getTextureAnmKeyNum,b);
 b=dc->pbsp.get();u32 mask=gabi::call_ptr<u32>(b->__vtbl->getTextureLoopOffset,b);s32 offset=(u32)params(p)->mLoopOffset&mask;
 s32 time=gabi::ftoi(gabi::fmadds((f32)max,(f32)p->mCurNormTime,(f32)offset));b=dc->pbsp.get();
 u32 index=gabi::call_ptr<u32>(b->__vtbl->getTextureIndexFrame,b,(u8)remainder(time,max));params(p)->mTexIdx=sead::elementAt(dc->pTexIdx,index);
}
/* 02831508 */
void JPADrawCalcTextureAnmIndexRandom::calc(const Context* dc,JPABaseParticle* p){
 auto* b=dc->pbsp.get();u32 mask=gabi::call_ptr<u32>(b->__vtbl->getTextureLoopOffset,b);
 b=dc->pbsp.get();auto* vt=b->__vtbl.get();s32 offset=(u32)params(p)->mLoopOffset&mask;
 s32 max=gabi::call_ptr<s32>(vt->getTextureAnmKeyNum,b);u32 index=gabi::call_ptr<u32>(vt->getTextureIndexFrame,b,(u8)remainder(offset,max));
 params(p)->mTexIdx=sead::elementAt(dc->pTexIdx,index);
}
/* 02831278 */
static void JPADrawCalcTextureAnmIndexNormal_adjusted(JPADrawCalcTextureAnmIndexNormal* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcTextureAnmIndexNormal*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 02831324 */
static void JPADrawCalcTextureAnmIndexRepeat_adjusted(JPADrawCalcTextureAnmIndexRepeat* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcTextureAnmIndexRepeat*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 02831404 */
static void JPADrawCalcTextureAnmIndexReverse_adjusted(JPADrawCalcTextureAnmIndexReverse* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcTextureAnmIndexReverse*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 02831500 */
static void JPADrawCalcTextureAnmIndexMerge_adjusted(JPADrawCalcTextureAnmIndexMerge* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcTextureAnmIndexMerge*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 02831598 */
static void JPADrawCalcTextureAnmIndexRandom_adjusted(JPADrawCalcTextureAnmIndexRandom* object,const Context* dc,JPABaseParticle* p){
 // HD: GHS secondary-base thunk adjusts the visitor subobject.
 reinterpret_cast<JPADrawCalcTextureAnmIndexRandom*>(reinterpret_cast<u8*>(object)-4)->calc(dc,p);
}
/* 028304F0 */
void dirTypePrevPtcl(JPABaseParticle* p,Emitter* emtr,Vec* out){
 f32 x=p->mGlobalPosition[0],z=p->mGlobalPosition[2],y=p->mGlobalPosition[1];
 auto* link=p->mLink.mpPrev.get();
 if(link){auto* previous=link->mpObject.get();out->x=*reinterpret_cast<be<u32>*>(&previous->mGlobalPosition[0]);f32 px=*reinterpret_cast<be<f32>*>(&out->x);
 out->y=*reinterpret_cast<be<u32>*>(&previous->mGlobalPosition[1]);f32 pz=previous->mGlobalPosition[2],py=*reinterpret_cast<be<f32>*>(&out->y);
 *reinterpret_cast<be<f32>*>(&out->x)=gabi::fsubs_ppc(px,x);*reinterpret_cast<be<f32>*>(&out->y)=gabi::fsubs_ppc(py,y);*reinterpret_cast<be<f32>*>(&out->z)=gabi::fsubs_ppc(pz,z);
 }else{emtr->calcEmitterGlobalPosition(out);f32 px=*reinterpret_cast<be<f32>*>(&out->x),py=*reinterpret_cast<be<f32>*>(&out->y),pz=*reinterpret_cast<be<f32>*>(&out->z);
 *reinterpret_cast<be<f32>*>(&out->x)=gabi::fsubs_ppc(px,x);*reinterpret_cast<be<f32>*>(&out->y)=gabi::fsubs_ppc(py,y);*reinterpret_cast<be<f32>*>(&out->z)=gabi::fsubs_ppc(pz,z);}
}
/* 02830D58 */
void JPADrawExecCallBack::exec(const Context* dc,JPABaseParticle* p){
 auto* callback=reinterpret_cast<ParticleCallback*>(p->mpCallBack2.get());auto* emitter=dc->pbe.get();
 if(callback) gabi::tail_ptr<void>(callback->__vtbl->execute,callback,emitter,p);
}
/* 028315A0 */
void JPADrawExecCallBack::draw(const Context* dc,JPABaseParticle* p){
 if(p->mHidden)return;
 auto* callback=reinterpret_cast<ParticleCallback*>(p->mpCallBack2.get());auto* emitter=dc->pbe.get();
 if(callback) gabi::tail_ptr<void>(callback->__vtbl->draw,callback,emitter,p);
}
/* 028315D4 */
void JPADrawExecLoadTexture::exec(const Context* dc){
 if(!dc->pTexIdx) JPADrawAssert("JPADrawVisitor.cpp",0x38,"pDC->pTexIdx");
}
/* 02831DEC */
void colorToFloat(ColorFloat* out,const Color* color){
 f32 r=(f32)(u8)color->r/255.0f,g=(f32)(u8)color->g/255.0f,b=(f32)(u8)color->b/255.0f,a=(f32)(u8)color->a/255.0f;
 out->rgba[0]=r;out->rgba[1]=g;out->rgba[2]=b;out->rgba[3]=a;
}
/* 02832024 */
void JPADrawExecSetPointSize::exec(const Context* dc,JPABaseParticle* p){
 s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
 f32 size=(f32)clip->mGlobalScaleX*(f32)params(p)->mScaleX;JPASetPointSize(p,(u8)gabi::ftoi(size));
}
/* 02832080 */
void JPADrawExecSetLineWidth::exec(const Context* dc,JPABaseParticle* p){}
/* 02832084 */
void JPADrawExecRegisterPrmColorAnm::exec(const Context* dc,JPABaseParticle* p){
 s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
 Color primary;
 *reinterpret_cast<be<u32>*>(&primary)=params(p)->mPrmColor;
 primary.r=((u32)(u8)primary.r*((u32)(u8)clip->mPrmColor.r+1))>>8;
 primary.g=((u32)(u8)primary.g*((u32)(u8)clip->mPrmColor.g+1))>>8;
 primary.b=((u32)(u8)primary.b*((u32)(u8)clip->mPrmColor.b+1))>>8;
 u32 alpha=((u32)(u8)primary.a*((u32)(u8)clip->mPrmColor.a+1))>>8;primary.a=(u8)gabi::ftoi((f32)alpha*(f32)params(p)->mAlphaOut);
 ColorFloat converted;
 colorToFloat(&converted,&primary);JPASetPrmColor(p,&converted);
}
/* 0283217C */
void JPADrawExecRegisterPrmAlphaAnm::exec(const Context* dc,JPABaseParticle* p){
 s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
 Color primary;
 *reinterpret_cast<be<u32>*>(&primary)=reinterpret_cast<JPAMathDraw::JPADraw*>(dc->mpDraw.get())->mPrmColor;
 primary.r=((u32)(u8)primary.r*((u32)(u8)clip->mPrmColor.r+1))>>8;
 primary.g=((u32)(u8)primary.g*((u32)(u8)clip->mPrmColor.g+1))>>8;
 primary.b=((u32)(u8)primary.b*((u32)(u8)clip->mPrmColor.b+1))>>8;
 u32 alpha=((u32)(u8)primary.a*((u32)(u8)clip->mPrmColor.a+1))>>8;primary.a=(u8)gabi::ftoi((f32)alpha*(f32)params(p)->mAlphaOut);
 ColorFloat converted;
 colorToFloat(&converted,&primary);JPASetPrmColor(p,&converted);
}
/* 02832278 */
void JPADrawExecRegisterEnvColorAnm::exec(const Context* dc,JPABaseParticle* p){
 s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
 Color environment;
 *reinterpret_cast<be<u32>*>(&environment)=params(p)->mEnvColor;
 environment.r=((u32)(u8)environment.r*((u32)(u8)clip->mEnvColor.r+1))>>8;
 environment.g=((u32)(u8)environment.g*((u32)(u8)clip->mEnvColor.g+1))>>8;
 environment.b=((u32)(u8)environment.b*((u32)(u8)clip->mEnvColor.b+1))>>8;
 ColorFloat converted;
 colorToFloat(&converted,&environment);JPASetEnvColor(p,&converted);
}
/* 0283231C */
void JPADrawExecRegisterPrmCEnv::exec(const Context* dc,JPABaseParticle* p){
 s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
 Color primary;
 *reinterpret_cast<be<u32>*>(&primary)=params(p)->mPrmColor;
 Color environment;
 *reinterpret_cast<be<u32>*>(&environment)=params(p)->mEnvColor;
 primary.r=((u32)(u8)primary.r*((u32)(u8)clip->mPrmColor.r+1))>>8;
 primary.g=((u32)(u8)primary.g*((u32)(u8)clip->mPrmColor.g+1))>>8;
 primary.b=((u32)(u8)primary.b*((u32)(u8)clip->mPrmColor.b+1))>>8;
 u32 alpha=((u32)(u8)primary.a*((u32)(u8)clip->mPrmColor.a+1))>>8;primary.a=(u8)gabi::ftoi((f32)alpha*(f32)params(p)->mAlphaOut);
 environment.r=((u32)(u8)environment.r*((u32)(u8)clip->mEnvColor.r+1))>>8;
 environment.g=((u32)(u8)environment.g*((u32)(u8)clip->mEnvColor.g+1))>>8;
 environment.b=((u32)(u8)environment.b*((u32)(u8)clip->mEnvColor.b+1))>>8;
 ColorFloat converted;
 colorToFloat(&converted,&primary);JPASetPrmColor(p,&converted);
 colorToFloat(&converted,&environment);JPASetEnvColor(p,&converted);
}
/* 0283248C */
void JPADrawExecRegisterPrmAEnv::exec(const Context* dc,JPABaseParticle* p){
 s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
 Color primary;
 *reinterpret_cast<be<u32>*>(&primary)=reinterpret_cast<JPAMathDraw::JPADraw*>(dc->mpDraw.get())->mPrmColor;
 Color environment;
 *reinterpret_cast<be<u32>*>(&environment)=params(p)->mEnvColor;
 primary.r=((u32)(u8)primary.r*((u32)(u8)clip->mPrmColor.r+1))>>8;
 primary.g=((u32)(u8)primary.g*((u32)(u8)clip->mPrmColor.g+1))>>8;
 primary.b=((u32)(u8)primary.b*((u32)(u8)clip->mPrmColor.b+1))>>8;
 u32 alpha=((u32)(u8)primary.a*((u32)(u8)clip->mPrmColor.a+1))>>8;primary.a=(u8)gabi::ftoi((f32)alpha*(f32)params(p)->mAlphaOut);
 environment.r=((u32)(u8)environment.r*((u32)(u8)clip->mEnvColor.r+1))>>8;
 environment.g=((u32)(u8)environment.g*((u32)(u8)clip->mEnvColor.g+1))>>8;
 environment.b=((u32)(u8)environment.b*((u32)(u8)clip->mEnvColor.b+1))>>8;
 ColorFloat converted;
 colorToFloat(&converted,&primary);JPASetPrmColor(p,&converted);
 colorToFloat(&converted,&environment);JPASetEnvColor(p,&converted);
}
/* 02830D78 */
static void JPADrawExecCallBack_exec_adjusted(JPADrawExecCallBack* object,const Context* dc,JPABaseParticle* p){ reinterpret_cast<JPADrawExecCallBack*>(reinterpret_cast<u8*>(object)-4)->exec(dc,p); }
/* 028315CC */
static void JPADrawExecCallBack_draw_adjusted(JPADrawExecCallBack* object,const Context* dc,JPABaseParticle* p){ reinterpret_cast<JPADrawExecCallBack*>(reinterpret_cast<u8*>(object)-4)->draw(dc,p); }
/* 02831978 */
void JPADrawExecRegisterColorEmitterPE::exec(const Context* dc){
 u32 primary=reinterpret_cast<JPAMathDraw::JPADraw*>(dc->mpDraw.get())->mPrmColor;
 u32 environment=reinterpret_cast<JPAMathDraw::JPADraw*>(dc->mpDraw.get())->mEnvColor;
 auto* clip=dc->pcb.get();
 primary=(primary&~(255u<<24))|((((primary>>24)&255)*((u32)(u8)clip->mPrmColor.r+1)>>8)<<24);
 primary=(primary&~(255u<<16))|((((primary>>16)&255)*((u32)(u8)clip->mPrmColor.g+1)>>8)<<16);
 primary=(primary&~(255u<<8))|((((primary>>8)&255)*((u32)(u8)clip->mPrmColor.b+1)>>8)<<8);
 primary=(primary&~(255u<<0))|((((primary>>0)&255)*((u32)(u8)clip->mPrmColor.a+1)>>8)<<0);
 environment=(environment&~(255u<<24))|((((environment>>24)&255)*((u32)(u8)clip->mEnvColor.r+1)>>8)<<24);
 environment=(environment&~(255u<<16))|((((environment>>16)&255)*((u32)(u8)clip->mEnvColor.g+1)>>8)<<16);
 environment=(environment&~(255u<<8))|((((environment>>8)&255)*((u32)(u8)clip->mEnvColor.b+1)>>8)<<8);
 f32 r=(f32)(primary>>24)/255.0f,g=(f32)((primary>>16)&255)/255.0f,b=(f32)((primary>>8)&255)/255.0f,a=(f32)(primary&255)/255.0f;
 auto* emitter=dc->pbe.get();
 storeColorWord(&emitter->mGpuState.mColorBits[0][0],r);
 storeColorWord(&emitter->mGpuState.mColorBits[0][1],g);
 storeColorWord(&emitter->mGpuState.mColorBits[0][2],b);
 storeColorWord(&emitter->mGpuState.mColorBits[0][3],a);
 f32 er=(f32)(environment>>24)/255.0f,eg=(f32)((environment>>16)&255)/255.0f,eb=(f32)((environment>>8)&255)/255.0f,ea=(f32)(environment&255)/255.0f;
 auto* envEmitter=dc->pbe.get();
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][0],er);
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][1],eg);
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][2],eb);
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][3],ea);
}
/* 02831B9C */
void JPADrawExecRegisterColorEmitterP::exec(const Context* dc){
 u32 primary=reinterpret_cast<JPAMathDraw::JPADraw*>(dc->mpDraw.get())->mPrmColor;
 auto* clip=dc->pcb.get();
 primary=(primary&~(255u<<24))|((((primary>>24)&255)*((u32)(u8)clip->mPrmColor.r+1)>>8)<<24);
 primary=(primary&~(255u<<16))|((((primary>>16)&255)*((u32)(u8)clip->mPrmColor.g+1)>>8)<<16);
 primary=(primary&~(255u<<8))|((((primary>>8)&255)*((u32)(u8)clip->mPrmColor.b+1)>>8)<<8);
 primary=(primary&~(255u<<0))|((((primary>>0)&255)*((u32)(u8)clip->mPrmColor.a+1)>>8)<<0);
 f32 r=(f32)(primary>>24)/255.0f,g=(f32)((primary>>16)&255)/255.0f,b=(f32)((primary>>8)&255)/255.0f,a=(f32)(primary&255)/255.0f;
 auto* emitter=dc->pbe.get();
 storeColorWord(&emitter->mGpuState.mColorBits[0][0],r);
 storeColorWord(&emitter->mGpuState.mColorBits[0][1],g);
 storeColorWord(&emitter->mGpuState.mColorBits[0][2],b);
 storeColorWord(&emitter->mGpuState.mColorBits[0][3],a);
}
/* 02831CD0 */
void JPADrawExecRegisterColorEmitterE::exec(const Context* dc){
 u32 environment=reinterpret_cast<JPAMathDraw::JPADraw*>(dc->mpDraw.get())->mEnvColor;
 auto* clip=dc->pcb.get();
 environment=(environment&~(255u<<24))|((((environment>>24)&255)*((u32)(u8)clip->mEnvColor.r+1)>>8)<<24);
 environment=(environment&~(255u<<16))|((((environment>>16)&255)*((u32)(u8)clip->mEnvColor.g+1)>>8)<<16);
 environment=(environment&~(255u<<8))|((((environment>>8)&255)*((u32)(u8)clip->mEnvColor.b+1)>>8)<<8);
 f32 er=(f32)(environment>>24)/255.0f,eg=(f32)((environment>>16)&255)/255.0f,eb=(f32)((environment>>8)&255)/255.0f,ea=(f32)(environment&255)/255.0f;
 auto* envEmitter=dc->pbe.get();
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][0],er);
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][1],eg);
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][2],eb);
 storeColorWord(&envEmitter->mGpuState.mColorBits[1][3],ea);
}
/* 02837A9C */
void JPADrawExecDraw::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 auto* particle=p;if(particle->mDrawSkipped){particle->mDrawSkipped=0;return;}
 JPAIssueDraw(p,0,500,1);
}
/* 02837AD4 */
void JPADrawExecDrawLine::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 auto* particle=p;if(particle->mDrawSkipped){particle->mDrawSkipped=0;return;}
 JPAIssueDraw(p,0,4,2);
}
/* 02837B0C */
void JPADrawExecDrawCross::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 JPAIssueDraw(p,2,500,1);
}
/* 0283196C */
void JPADrawExecGenIdtMtx::exec(const Context* dc){JPASetEmitterTextureMtx(dc->pbe.get(),nullptr);}
/* 028315F8 */
void JPADrawExecGenPrjMtx::exec(const Context* dc){
 auto* clip=dc->pcb.get();
 Matrix mtx;
 mtx.m[0][0]=(f32)clip->mProjectionMtx[0][0]*0.5f;
 mtx.m[0][1]=0.0f;
 mtx.m[0][2]=gabi::fnmsubs((f32)clip->mProjectionMtx[0][2],0.5f,-0.5f);
 mtx.m[0][3]=0.0f;mtx.m[1][0]=0.0f;
 mtx.m[1][1]=(f32)clip->mProjectionMtx[1][1]*-0.5f;
 mtx.m[1][2]=gabi::fnmsubs((f32)clip->mProjectionMtx[1][2],0.5f,-0.5f);
 mtx.m[2][0]=0.0f;mtx.m[2][1]=0.0f;mtx.m[2][2]=-1.0f;mtx.m[2][3]=0.0f;mtx.m[1][3]=0.0f;
 JPASetEmitterTextureMtx(dc->pbe.get(),&mtx);
}
/* inlined into JPADrawExecPoint::exec */
static void clearVertexCache(Vertex* vertices,u32 size){
 u32 start=gabi::ea(vertices),end=start+size;
 while(start<end){
  auto* line=&gabi::object_at<CacheLine>(start&~31u);
  for(u32 i=0;i<8;++i)line->words[i]=0;
  start+=32;
 }
}
/* 02835348 */
void JPADrawExecPoint::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 f32 z=p->mGlobalPosition[2],x=p->mGlobalPosition[0],y=p->mGlobalPosition[1];
 if(p->mNeedsVertexUpdate){
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  vertices->z=z;vertices->s=0;vertices->t=0;vertices->x=x;vertices->y=y;
 }
 JPADrawFinish(p,0);
}
/* 02832B34 */
void JPADrawExecBillBoard::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 pivotX=clip->mPivotX,globalX=clip->mGlobalScaleX,pivotY=clip->mPivotY,globalY=clip->mGlobalScaleY;
  f32 scaleX=params(p)->mScaleX,scaleY=params(p)->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX)),bottom=gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY)),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  Corners corners;
  JPAConstructArray(&corners,4,8,JPAVec2Construct);
  corners.values[1].x=right;corners.values[1].y=top;corners.values[0].x=-left;corners.values[3].x=-left;
  corners.values[2].x=right;corners.values[2].y=-bottom;corners.values[3].y=-bottom;corners.values[0].y=top;
  VecFloat position;
  position.y=p->mGlobalPosition[1];position.x=p->mGlobalPosition[0];position.z=p->mGlobalPosition[2];
  JPAMultVec(clip->mDrawMtxPtr.get(),&position,&position);
  Matrix inverse;
  JPAInverse(clip->mDrawMtxPtr.get(),&inverse);
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  VecFloat point;
  for(u32 i=0;i<4;++i){
   point.x=gabi::fadds_ppc((f32)corners.values[i].x,(f32)position.x);
   point.z=position.z;point.y=gabi::fadds_ppc((f32)corners.values[i].y,(f32)position.y);
   JPAMultVec(&inverse,&point,&point);
   vertices[i].x=point.x;vertices[i].y=point.y;vertices[i].z=point.z;
   f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];
   vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 02832D6C */
void JPADrawExecRotBillBoard::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 pivotX=clip->mPivotX,globalX=clip->mGlobalScaleX,pivotY=clip->mPivotY,globalY=clip->mGlobalScaleY;
  f32 scaleX=params(p)->mScaleX,scaleY=params(p)->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX)),bottom=gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY)),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  u32 angle=params(p)->mRotateAngle;f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
  Corners corners;
  JPAConstructArray(&corners,4,8,JPAVec2Construct);
  f32 topSin=gabi::fmuls_ppc(top,sin),leftCos=gabi::fmuls_ppc((-left),cos),topCos=gabi::fmuls_ppc(top,cos),rightCos=gabi::fmuls_ppc(right,cos),bottomCos=gabi::fmuls_ppc((-bottom),cos),leftSin=gabi::fmuls_ppc((-left),sin),rightSin=gabi::fmuls_ppc(right,sin),bottomSin=gabi::fmuls_ppc((-bottom),sin);
  corners.values[0].x=gabi::fsubs_ppc(leftCos,topSin);corners.values[0].y=gabi::fadds_ppc(topCos,leftSin);
  corners.values[2].y=gabi::fadds_ppc(bottomCos,rightSin);corners.values[1].y=gabi::fadds_ppc(topCos,rightSin);
  corners.values[1].x=gabi::fsubs_ppc(rightCos,topSin);corners.values[2].x=gabi::fsubs_ppc(rightCos,bottomSin);
  corners.values[3].y=gabi::fadds_ppc(bottomCos,leftSin);corners.values[3].x=gabi::fsubs_ppc(leftCos,bottomSin);
  VecFloat position;
  position.y=p->mGlobalPosition[1];position.x=p->mGlobalPosition[0];position.z=p->mGlobalPosition[2];
  JPAMultVec(clip->mDrawMtxPtr.get(),&position,&position);
  Matrix inverse;
  JPAInverse(clip->mDrawMtxPtr.get(),&inverse);
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  VecFloat point;
  for(u32 i=0;i<4;++i){
   point.x=gabi::fadds_ppc((f32)corners.values[i].x,(f32)position.x);
   point.z=position.z;point.y=gabi::fadds_ppc((f32)corners.values[i].y,(f32)position.y);
   JPAMultVec(&inverse,&point,&point);
   vertices[i].x=point.x;vertices[i].y=point.y;vertices[i].z=point.z;
   f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];
   vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 02833030 */
void JPADrawExecYBillBoard::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 pivotX=clip->mPivotX,globalX=clip->mGlobalScaleX,pivotY=clip->mPivotY,globalY=clip->mGlobalScaleY;
  f32 scaleX=params(p)->mScaleX,scaleY=params(p)->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX)),bottom=gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY)),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  Points corners;
  JPAConstructArray(&corners,4,12,JPAVec3Construct);
  corners.values[1].x=right;corners.values[1].y=top;corners.values[0].x=-left;corners.values[3].x=-left;
  corners.values[2].x=right;corners.values[2].y=-bottom;corners.values[3].y=-bottom;corners.values[0].y=top;
  for(u32 i=0;i<4;++i)corners.values[i].z=0;
  JPAMultVecArray(&clip->mDrawYBBMtx,&corners,&corners,4);
  VecFloat position;
  position.y=p->mGlobalPosition[1];position.x=p->mGlobalPosition[0];position.z=p->mGlobalPosition[2];
  JPAMultVecSR(clip->mDrawMtxPtr.get(),&position,&position);
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  VecFloat point;
  for(u32 i=0;i<4;++i){
   point.x=gabi::fadds_ppc((f32)corners.values[i].x,(f32)position.x);
   point.z=gabi::fadds_ppc((f32)corners.values[i].z,(f32)position.z);point.y=gabi::fadds_ppc((f32)corners.values[i].y,(f32)position.y);
   vertices[i].x=point.x;vertices[i].y=point.y;vertices[i].z=point.z;
   f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];
   vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 0283325C */
void JPADrawExecRotYBillBoard::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 pivotX=clip->mPivotX,globalX=clip->mGlobalScaleX,pivotY=clip->mPivotY,globalY=clip->mGlobalScaleY;
  f32 scaleX=params(p)->mScaleX,scaleY=params(p)->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX)),bottom=gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY)),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  u32 angle=params(p)->mRotateAngle;f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
  Points corners;
  JPAConstructArray(&corners,4,12,JPAVec3Construct);
  f32 topSin=gabi::fmuls_ppc(top,sin),leftCos=gabi::fmuls_ppc((-left),cos),topCos=gabi::fmuls_ppc(top,cos),rightCos=gabi::fmuls_ppc(right,cos),bottomCos=gabi::fmuls_ppc((-bottom),cos),leftSin=gabi::fmuls_ppc((-left),sin),rightSin=gabi::fmuls_ppc(right,sin),bottomSin=gabi::fmuls_ppc((-bottom),sin);
  corners.values[0].x=gabi::fsubs_ppc(leftCos,topSin);corners.values[0].y=gabi::fadds_ppc(topCos,leftSin);
  corners.values[2].y=gabi::fadds_ppc(bottomCos,rightSin);corners.values[1].y=gabi::fadds_ppc(topCos,rightSin);
  corners.values[1].x=gabi::fsubs_ppc(rightCos,topSin);corners.values[2].x=gabi::fsubs_ppc(rightCos,bottomSin);
  corners.values[3].y=gabi::fadds_ppc(bottomCos,leftSin);corners.values[3].x=gabi::fsubs_ppc(leftCos,bottomSin);
  for(u32 i=0;i<4;++i)corners.values[i].z=0;
  JPAMultVecArray(&clip->mDrawYBBMtx,&corners,&corners,4);
  VecFloat position;
  position.y=p->mGlobalPosition[1];position.x=p->mGlobalPosition[0];position.z=p->mGlobalPosition[2];
  JPAMultVecSR(clip->mDrawMtxPtr.get(),&position,&position);
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  VecFloat point;
  for(u32 i=0;i<4;++i){
   point.x=gabi::fadds_ppc((f32)corners.values[i].x,(f32)position.x);
   point.z=gabi::fadds_ppc((f32)corners.values[i].z,(f32)position.z);point.y=gabi::fadds_ppc((f32)corners.values[i].y,(f32)position.y);
   vertices[i].x=point.x;vertices[i].y=point.y;vertices[i].z=point.z;
   f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];
   vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 02833510 */
void JPADrawExecDirectional::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  auto* draw=params(p);auto* axis=reinterpret_cast<VecFloat*>(&draw->mAxis);
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 globalX=clip->mGlobalScaleX,pivotX=clip->mPivotX,globalY=clip->mGlobalScaleY,pivotY=clip->mPivotY;
  f32 scaleX=draw->mScaleX,scaleY=draw->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=-(gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX))),bottom=-(gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY))),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  
  Points points;
  JPAConstructArray(&points,4,12,JPAVec3Construct);
  gabi::call_ptr<void>(clip->mBasePlaneTypeFunc,&points,left,right,top,bottom);
  
  VecFloat dir;
  gabi::call_ptr<void>(clip->mDirTypeFunc,p,dc->pbe.get(),&dir);
  f32 y=dir.y,x=dir.x,z=dir.z;
  constexpr f32 epsilon=3.814697265625e-06f;
  f32 length=gabi::fmadds(z,z,gabi::fmadds(x,x,y*y));
  if(!(length>epsilon)){p->mDrawSkipped=1;return;}
  if(length>epsilon){
   f32 root=(f32)JPADrawSqrt(length),inv=1.0f/root;
   x=dir.x;y=dir.y;z=dir.z;
   x=x*inv;y=y*inv;z=z*inv;dir.x=x;dir.y=y;dir.z=z;
  }
  f32 ax=axis->x,ay=axis->y,az=axis->z;
  f32 sideX=gabi::fmsubs(ay,z,az*y),sideY=gabi::fmsubs(az,x,ax*z),sideZ=gabi::fmsubs(ax,y,ay*x);
  f32 sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(!(sideLength>epsilon)){p->mDrawSkipped=1;return;}
  sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(sideLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(sideLength);
   sideY=sideY*inv;sideZ=sideZ*inv;sideX=sideX*inv;
   x=dir.x;y=dir.y;z=dir.z;
  }
  f32 nextY=gabi::fmsubs(z,sideX,x*sideZ),nextX=gabi::fmsubs(y,sideZ,z*sideY),nextZ=gabi::fmsubs(x,sideY,y*sideX);
  f32 axisLength=gabi::fmadds(nextZ,nextZ,gabi::fmadds(nextX,nextX,nextY*nextY));
  axis->z=nextZ;axis->y=nextY;axis->x=nextX;
  if(axisLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(axisLength);
   nextX=axis->x;nextY=axis->y;nextZ=axis->z;
   nextX=nextX*inv;nextY=nextY*inv;nextZ=nextZ*inv;
   axis->x=nextX;axis->y=nextY;axis->z=nextZ;
  }
  Matrix mtx;
  mtx.m[0][0]=axis->x;mtx.m[0][1]=dir.x;mtx.m[0][2]=sideX;mtx.m[0][3]=0;
  mtx.m[1][0]=axis->y;mtx.m[1][1]=dir.y;mtx.m[1][2]=sideY;mtx.m[1][3]=0;
  mtx.m[2][0]=axis->z;mtx.m[2][1]=dir.z;mtx.m[2][2]=sideZ;mtx.m[2][3]=0;
  JPAMultVecArray(&mtx,&points,&points,4);
  f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  for(u32 i=0;i<4;++i){
   f32 x=gabi::fadds_ppc((f32)points.values[i].x,px),y=gabi::fadds_ppc((f32)points.values[i].y,py),z=gabi::fadds_ppc((f32)points.values[i].z,pz);
   vertices[i].x=x;vertices[i].y=y;vertices[i].z=z;
   f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 02833948 */
void JPADrawExecRotDirectional::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  auto* draw=params(p);auto* axis=reinterpret_cast<VecFloat*>(&draw->mAxis);
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 globalX=clip->mGlobalScaleX,pivotX=clip->mPivotX,globalY=clip->mGlobalScaleY,pivotY=clip->mPivotY;
  f32 scaleX=draw->mScaleX,scaleY=draw->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=-(gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX))),bottom=-(gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY))),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  u32 angle=draw->mRotateAngle;f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
  Points points;
  JPAConstructArray(&points,4,12,JPAVec3Construct);
  gabi::call_ptr<void>(clip->mBasePlaneTypeFunc,&points,left,right,top,bottom);
  Matrix rotation;
  gabi::call_ptr<void>(clip->mRotTypeFunc,&rotation,sin,cos);
  VecFloat dir;
  gabi::call_ptr<void>(clip->mDirTypeFunc,p,dc->pbe.get(),&dir);
  f32 y=dir.y,x=dir.x,z=dir.z;
  constexpr f32 epsilon=3.814697265625e-06f;
  f32 length=gabi::fmadds(z,z,gabi::fmadds(x,x,y*y));
  if(!(length>epsilon)){p->mDrawSkipped=1;return;}
  if(length>epsilon){
   f32 root=(f32)JPADrawSqrt(length),inv=1.0f/root;
   x=dir.x;y=dir.y;z=dir.z;
   x=x*inv;y=y*inv;z=z*inv;dir.x=x;dir.y=y;dir.z=z;
  }
  f32 ax=axis->x,ay=axis->y,az=axis->z;
  f32 sideX=gabi::fmsubs(ay,z,az*y),sideY=gabi::fmsubs(az,x,ax*z),sideZ=gabi::fmsubs(ax,y,ay*x);
  f32 sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(!(sideLength>epsilon)){p->mDrawSkipped=1;return;}
  sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(sideLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(sideLength);
   sideY=sideY*inv;sideZ=sideZ*inv;sideX=sideX*inv;
   x=dir.x;y=dir.y;z=dir.z;
  }
  f32 nextY=gabi::fmsubs(z,sideX,x*sideZ),nextX=gabi::fmsubs(y,sideZ,z*sideY),nextZ=gabi::fmsubs(x,sideY,y*sideX);
  f32 axisLength=gabi::fmadds(nextZ,nextZ,gabi::fmadds(nextX,nextX,nextY*nextY));
  axis->z=nextZ;axis->y=nextY;axis->x=nextX;
  if(axisLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(axisLength);
   nextX=axis->x;nextY=axis->y;nextZ=axis->z;
   nextX=nextX*inv;nextY=nextY*inv;nextZ=nextZ*inv;
   axis->x=nextX;axis->y=nextY;axis->z=nextZ;
  }
  Matrix mtx;
  mtx.m[0][0]=axis->x;mtx.m[0][1]=dir.x;mtx.m[0][2]=sideX;mtx.m[0][3]=0;
  mtx.m[1][0]=axis->y;mtx.m[1][1]=dir.y;mtx.m[1][2]=sideY;mtx.m[1][3]=0;
  mtx.m[2][0]=axis->z;mtx.m[2][1]=dir.z;mtx.m[2][2]=sideZ;mtx.m[2][3]=0;
  Matrix combined;
  JPAConcat(&mtx,&rotation,&combined);
  JPAMultVecArray(&combined,&points,&points,4);
  f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  for(u32 i=0;i<4;++i){
   f32 x=gabi::fadds_ppc((f32)points.values[i].x,px),y=gabi::fadds_ppc((f32)points.values[i].y,py),z=gabi::fadds_ppc((f32)points.values[i].z,pz);
   vertices[i].x=x;vertices[i].y=y;vertices[i].z=z;
   f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 02830814 */
void JPADrawExecSetTexMtx::exec(const Context* dc,JPABaseParticle* p) {
 auto* block=dc->pbsp.get();u32 target=block->__vtbl->getTilingX;
 s32 age=gabi::ftoi((f32)p->mCurFrame);
 f32 halfX=0.5f*gabi::call_ptr<f32>(target,block);
 block=dc->pbsp.get();auto* vt=block->__vtbl.get();
 f32 halfY=0.5f*gabi::call_ptr<f32>(vt->getTilingY,block);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 tick=(f32)age;
 f32 transXScroll=gabi::call_ptr<f32>(vt->getTexScrollTransX,block);
 u32 transXTarget=vt->getTexStaticTransX;block=dc->pbsp.get();
 f32 transXStatic=gabi::call_ptr<f32>(transXTarget,block);
 f32 transX=gabi::fmadds(transXScroll,tick,transXStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 transYScroll=gabi::call_ptr<f32>(vt->getTexScrollTransY,block);
 u32 transYTarget=vt->getTexStaticTransY;block=dc->pbsp.get();
 f32 transYProduct=transYScroll*tick;
 f32 transYStatic=gabi::call_ptr<f32>(transYTarget,block);
 f32 transY=gabi::fadds_ppc(transYProduct,transYStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 scaleXScroll=gabi::call_ptr<f32>(vt->getTexScrollScaleX,block);
 u32 scaleXTarget=vt->getTexStaticScaleX;block=dc->pbsp.get();
 f32 scaleXProduct=scaleXScroll*tick;
 f32 scaleXStatic=gabi::call_ptr<f32>(scaleXTarget,block);
 f32 scaleX=gabi::fadds_ppc(scaleXProduct,scaleXStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 scaleYScroll=gabi::call_ptr<f32>(vt->getTexScrollScaleY,block);
 u32 scaleYTarget=vt->getTexStaticScaleY;block=dc->pbsp.get();
 f32 scaleYProduct=scaleYScroll*tick;
 f32 scaleYStatic=gabi::call_ptr<f32>(scaleYTarget,block);
 f32 scaleY=gabi::fadds_ppc(scaleYProduct,scaleYStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 rotate=gabi::call_ptr<f32>(vt->getTexScrollRotate,block);
 u32 angle=(u16)gabi::ftoi((rotate*tick)*32768.0f);
 f32 cos=sSinCosTable.mEntries[angle>>3].mCos,sin=sSinCosTable.mEntries[angle>>3].mSin;
 f32 tx=gabi::fadds_ppc(halfX,transX),ty=gabi::fadds_ppc(halfY,transY);
 Matrix mtx;
 mtx.m[0][0]=gabi::fmuls_ppc(scaleX,cos);mtx.m[0][1]=-(gabi::fmuls_ppc(scaleX,sin));mtx.m[0][2]=0;
 mtx.m[0][3]=gabi::fmadds(scaleX,gabi::fmsubs(ty,sin,gabi::fmuls_ppc(tx,cos)),halfX);
 mtx.m[1][0]=gabi::fmuls_ppc(scaleY,sin);mtx.m[1][1]=gabi::fmuls_ppc(scaleY,cos);mtx.m[1][2]=0;
 mtx.m[1][3]=gabi::fnmsubs(scaleY,gabi::fmadds(tx,sin,gabi::fmuls_ppc(ty,cos)),halfY);
 mtx.m[2][0]=0;mtx.m[2][1]=0;mtx.m[2][2]=1;mtx.m[2][3]=0;
 JPAGpuTextureMtx(p->mpGpuState.get(),&mtx,0);
}
/* 02832604 */
void JPADrawExecSetTexMtx::exec(const Context* dc) {
 auto* block=dc->pbsp.get();u32 target=block->__vtbl->getTilingX;
 s32 age=gabi::ftoi((f32)dc->pbe->mTick);
 f32 halfX=0.5f*gabi::call_ptr<f32>(target,block);
 block=dc->pbsp.get();auto* vt=block->__vtbl.get();
 f32 halfY=0.5f*gabi::call_ptr<f32>(vt->getTilingY,block);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 tick=(f32)age;
 f32 transXScroll=gabi::call_ptr<f32>(vt->getTexScrollTransX,block);
 u32 transXTarget=vt->getTexStaticTransX;block=dc->pbsp.get();
 f32 transXStatic=gabi::call_ptr<f32>(transXTarget,block);
 f32 transX=gabi::fmadds(transXScroll,tick,transXStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 transYScroll=gabi::call_ptr<f32>(vt->getTexScrollTransY,block);
 u32 transYTarget=vt->getTexStaticTransY;block=dc->pbsp.get();
 f32 transYProduct=transYScroll*tick;
 f32 transYStatic=gabi::call_ptr<f32>(transYTarget,block);
 f32 transY=gabi::fadds_ppc(transYProduct,transYStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 scaleXScroll=gabi::call_ptr<f32>(vt->getTexScrollScaleX,block);
 u32 scaleXTarget=vt->getTexStaticScaleX;block=dc->pbsp.get();
 f32 scaleXProduct=scaleXScroll*tick;
 f32 scaleXStatic=gabi::call_ptr<f32>(scaleXTarget,block);
 f32 scaleX=gabi::fadds_ppc(scaleXProduct,scaleXStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 scaleYScroll=gabi::call_ptr<f32>(vt->getTexScrollScaleY,block);
 u32 scaleYTarget=vt->getTexStaticScaleY;block=dc->pbsp.get();
 f32 scaleYProduct=scaleYScroll*tick;
 f32 scaleYStatic=gabi::call_ptr<f32>(scaleYTarget,block);
 f32 scaleY=gabi::fadds_ppc(scaleYProduct,scaleYStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 rotate=gabi::call_ptr<f32>(vt->getTexScrollRotate,block);
 u32 angle=(u16)gabi::ftoi((rotate*tick)*32768.0f);
 f32 cos=sSinCosTable.mEntries[angle>>3].mCos,sin=sSinCosTable.mEntries[angle>>3].mSin;
 f32 tx=gabi::fadds_ppc(halfX,transX),ty=gabi::fadds_ppc(halfY,transY);
 Matrix mtx;
 mtx.m[0][0]=gabi::fmuls_ppc(scaleX,cos);mtx.m[0][1]=-(gabi::fmuls_ppc(scaleX,sin));mtx.m[0][2]=0;
 mtx.m[0][3]=gabi::fmadds(scaleX,gabi::fmsubs(ty,sin,gabi::fmuls_ppc(tx,cos)),halfX);
 mtx.m[1][0]=gabi::fmuls_ppc(scaleY,sin);mtx.m[1][1]=gabi::fmuls_ppc(scaleY,cos);mtx.m[1][2]=0;
 mtx.m[1][3]=gabi::fnmsubs(scaleY,gabi::fmadds(tx,sin,gabi::fmuls_ppc(ty,cos)),halfY);
 mtx.m[2][0]=0;mtx.m[2][1]=0;mtx.m[2][2]=1;mtx.m[2][3]=0;
 JPASetEmitterTextureMtx(dc->pbe.get(),&mtx);
}
/* 02831694 */
void JPADrawExecGenPrjTexMtx::exec(const Context* dc) {
 auto* clip=dc->pcb.get();
 Matrix projection;
 projection.m[0][0]=(f32)clip->mProjectionMtx[0][0]*0.5f;projection.m[0][1]=0;
 projection.m[0][2]=gabi::fnmsubs((f32)clip->mProjectionMtx[0][2],0.5f,-0.5f);
 projection.m[0][3]=0;projection.m[1][0]=0;projection.m[1][1]=(f32)clip->mProjectionMtx[1][1]*-0.5f;
 projection.m[1][2]=gabi::fnmsubs((f32)clip->mProjectionMtx[1][2],0.5f,-0.5f);
 projection.m[1][3]=0;projection.m[2][0]=0;projection.m[2][1]=0;projection.m[2][2]=-1;projection.m[2][3]=0;
 auto* block=dc->pbsp.get();auto* vt=block->__vtbl.get();
 f32 tick=dc->pbe->mTick;f32 halfX=0.5f,halfY=0.5f;
 f32 transXScroll=gabi::call_ptr<f32>(vt->getTexScrollTransX,block);
 u32 transXTarget=vt->getTexStaticTransX;block=dc->pbsp.get();
 f32 transXProduct=gabi::fmuls_ppc(transXScroll,tick);
 f32 transXStatic=gabi::call_ptr<f32>(transXTarget,block);
 f32 transX=gabi::fadds_ppc(transXProduct,transXStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 transYScroll=gabi::call_ptr<f32>(vt->getTexScrollTransY,block);
 u32 transYTarget=vt->getTexStaticTransY;block=dc->pbsp.get();
 f32 transYProduct=gabi::fmuls_ppc(transYScroll,tick);
 f32 transYStatic=gabi::call_ptr<f32>(transYTarget,block);
 f32 transY=gabi::fadds_ppc(transYProduct,transYStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 scaleXScroll=gabi::call_ptr<f32>(vt->getTexScrollScaleX,block);
 u32 scaleXTarget=vt->getTexStaticScaleX;block=dc->pbsp.get();
 f32 scaleXProduct=gabi::fmuls_ppc(scaleXScroll,tick);
 f32 scaleXStatic=gabi::call_ptr<f32>(scaleXTarget,block);
 f32 scaleX=gabi::fadds_ppc(scaleXProduct,scaleXStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 scaleYScroll=gabi::call_ptr<f32>(vt->getTexScrollScaleY,block);
 u32 scaleYTarget=vt->getTexStaticScaleY;block=dc->pbsp.get();
 f32 scaleYProduct=gabi::fmuls_ppc(scaleYScroll,tick);
 f32 scaleYStatic=gabi::call_ptr<f32>(scaleYTarget,block);
 f32 scaleY=gabi::fadds_ppc(scaleYProduct,scaleYStatic);
 block=dc->pbsp.get();vt=block->__vtbl.get();
 f32 rotate=gabi::call_ptr<f32>(vt->getTexScrollRotate,block);
 u32 angle=(u16)gabi::ftoi((rotate*tick)*32768.0f);
 f32 cos=sSinCosTable.mEntries[angle>>3].mCos,sin=sSinCosTable.mEntries[angle>>3].mSin;
 f32 tx=gabi::fadds_ppc(halfX,transX),ty=gabi::fadds_ppc(halfY,transY);
 Matrix mtx;
 mtx.m[0][0]=gabi::fmuls_ppc(scaleX,cos);mtx.m[0][1]=-(gabi::fmuls_ppc(scaleX,sin));mtx.m[0][2]=0;
 mtx.m[0][3]=gabi::fmadds(scaleX,gabi::fmsubs(ty,sin,gabi::fmuls_ppc(tx,cos)),halfX);
 mtx.m[1][0]=gabi::fmuls_ppc(scaleY,sin);mtx.m[1][1]=gabi::fmuls_ppc(scaleY,cos);mtx.m[1][2]=0;
 mtx.m[1][3]=gabi::fnmsubs(scaleY,gabi::fmadds(tx,sin,gabi::fmuls_ppc(ty,cos)),halfY);
 mtx.m[2][0]=0;mtx.m[2][1]=0;mtx.m[2][2]=1;mtx.m[2][3]=0;
 mtx.m[0][2]=mtx.m[0][3];mtx.m[0][3]=0;mtx.m[1][2]=mtx.m[1][3];mtx.m[1][3]=0;
 JPAConcat(&mtx,&projection,&projection);
 JPAConcat(&projection,&dc->pcb->mDrawMtx,&mtx);
 JPASetEmitterTextureMtx(dc->pbe.get(),&mtx);
}
/* 02830ADC */
static void JPADrawExecSetTexMtx_adjusted(JPADrawExecSetTexMtx* object,const Context* dc,JPABaseParticle* p){reinterpret_cast<JPADrawExecSetTexMtx*>(reinterpret_cast<u8*>(object)-4)->exec(dc,p);}
/* 02831EA0 */
void JPADrawExecRegisterColorChildPE::exec(const Context* dc){
 Color primary;
 Color environment;
 auto* block=dc->pssp.get();u32 value=gabi::call_ptr<u32>(block->__vtbl->getPrm,block);
 *reinterpret_cast<be<u32>*>(&primary)=value;
 block=dc->pssp.get();value=gabi::call_ptr<u32>(block->__vtbl->getEnv,block);
 *reinterpret_cast<be<u32>*>(&environment)=value;
 auto* clip=dc->pcb.get();
 primary.r=((u32)(u8)primary.r*((u32)(u8)clip->mPrmColor.r+1))>>8;
 primary.g=((u32)(u8)primary.g*((u32)(u8)clip->mPrmColor.g+1))>>8;
 primary.b=((u32)(u8)primary.b*((u32)(u8)clip->mPrmColor.b+1))>>8;
 primary.a=((u32)(u8)primary.a*((u32)(u8)clip->mPrmColor.a+1))>>8;
 environment.r=((u32)(u8)environment.r*((u32)(u8)clip->mEnvColor.r+1))>>8;
 environment.g=((u32)(u8)environment.g*((u32)(u8)clip->mEnvColor.g+1))>>8;
 environment.b=((u32)(u8)environment.b*((u32)(u8)clip->mEnvColor.b+1))>>8;
 ColorFloat converted;
 auto* emitter=dc->pbe.get();colorToFloat(&converted,&primary);
 emitter->mGpuState.mColorBits[0][0]=*reinterpret_cast<be<u32>*>(&converted.rgba[0]);
 emitter->mGpuState.mColorBits[0][1]=*reinterpret_cast<be<u32>*>(&converted.rgba[1]);
 emitter->mGpuState.mColorBits[0][2]=*reinterpret_cast<be<u32>*>(&converted.rgba[2]);
 emitter->mGpuState.mColorBits[0][3]=*reinterpret_cast<be<u32>*>(&converted.rgba[3]);
 emitter=dc->pbe.get();colorToFloat(&converted,&environment);
 emitter->mGpuState.mColorBits[1][0]=*reinterpret_cast<be<u32>*>(&converted.rgba[0]);
 emitter->mGpuState.mColorBits[1][1]=*reinterpret_cast<be<u32>*>(&converted.rgba[1]);
 emitter->mGpuState.mColorBits[1][2]=*reinterpret_cast<be<u32>*>(&converted.rgba[2]);
 emitter->mGpuState.mColorBits[1][3]=*reinterpret_cast<be<u32>*>(&converted.rgba[3]);
}
static bool sameDescriptor(const JPAMathDraw::JPATextureDescriptor* a,const JPAMathDraw::JPATextureDescriptor* b){
 return (u32)a->mDimension==(u32)b->mDimension && (u32)a->mWidth==(u32)b->mWidth && (u32)a->mHeight==(u32)b->mHeight && (u32)a->mDepth==(u32)b->mDepth && (u32)a->mMipLevels==(u32)b->mMipLevels && (u32)a->mFormat==(u32)b->mFormat && (u32)a->mSwizzle==(u32)b->mSwizzle && (u32)a->mTileMode==(u32)b->mTileMode && (u32)a->mAA==(u32)b->mAA;
}
/* 02830AE4 */
void JPADrawExecLoadTexture::exec(const Context* dc,JPABaseParticle* p){
 auto* shape=dc->pbsp.get();
 if(gabi::call_ptr<u32>(shape->__vtbl->textureIsEmpty,shape))return;
 shape=dc->pbsp.get();if(!gabi::call_ptr<u32>(shape->__vtbl->isEnableTextureAnm,shape))return;
 shape=dc->pbsp.get();if(gabi::call_ptr<u32>(shape->__vtbl->isEnableTextureAnmGlobal,shape))return;
 u32 index=params(p)->mTexIdx;

 auto* descriptor=static_cast<JPAMathDraw::JPATextureDescriptor*>(static_cast<::JPATextureResource*>(dc->mpTexResource.get())->mDescriptors[index].get());
 if(descriptor){
  auto* cache=p;
  u32 textureIndex=cache->mCachedTextureIndex;u32 emitterIndex=dc->pbe->mGpuState.mResourceIndex;
  if(textureIndex!=index || (u32)cache->mCachedEmitterIndex!=emitterIndex){
   auto* binding=JPAGetTextureBinding(p->mpRenderer.get(),0);
   if(!sameDescriptor(&binding->mDescriptor,descriptor))JPAApplyTexture(binding,descriptor);
   else {u32 image=descriptor->mImageAddress,mip=descriptor->mMipAddress;binding->mDescriptor.mImageAddress=image;binding->mCachedMipAddress=mip;binding->mCachedImageAddress=image;binding->mDescriptor.mMipAddress=mip;}
   emitterIndex=dc->pbe->mGpuState.mResourceIndex;cache->mCachedTextureIndex=index;cache->mCachedEmitterIndex=emitterIndex;
   auto* name=&static_cast<::JPATextureResource*>(dc->mpTexResource.get())->mNames[(s16)index];
   u32 mode=name->mMode,flags=name->mFlags;
   binding=JPAGetTextureBinding(p->mpRenderer.get(),0);
   if((u32)binding->mBinding[0]!=mode){binding=JPAGetTextureBinding(p->mpRenderer.get(),0);u32 dirty=binding->mDirtyFlags;binding->mBinding[0]=mode;binding->mDirtyFlags=dirty|2;}
   binding=JPAGetTextureBinding(p->mpRenderer.get(),0);
   if((u32)binding->mBinding[1]!=flags){binding=JPAGetTextureBinding(p->mpRenderer.get(),0);u32 dirty=binding->mDirtyFlags;binding->mBinding[1]=flags;binding->mDirtyFlags=dirty|2;}
  }
 }
}
/* 02830D50 */
static void JPADrawExecLoadTexture_adjusted(JPADrawExecLoadTexture* object,const Context* dc,JPABaseParticle* p){reinterpret_cast<JPADrawExecLoadTexture*>(reinterpret_cast<u8*>(object)-4)->exec(dc,p);}
/* 028328C0 */
void JPADrawExecLoadDefaultTexture::exec(const Context* dc){
 auto* shape=dc->pbsp.get();u32 target=shape->__vtbl->textureIsEmpty;
 u32 index=reinterpret_cast<JPAMathDraw::JPADraw*>(dc->mpDraw.get())->mTexIdx;
 if(gabi::call_ptr<u32>(target,shape))return;
 shape=dc->pbsp.get();u32 enabled=gabi::call_ptr<u32>(shape->__vtbl->isEnableTextureAnm,shape);
 if(enabled){shape=dc->pbsp.get();if(!gabi::call_ptr<u32>(shape->__vtbl->isEnableTextureAnmGlobal,shape))return;}
 auto* link=reinterpret_cast<JPAParticleList*>(dc->mpList.get())->mpHead.get();
 while(link){auto* p=link->mpObject.get();

 auto* descriptor=static_cast<JPAMathDraw::JPATextureDescriptor*>(static_cast<::JPATextureResource*>(dc->mpTexResource.get())->mDescriptors[index].get());
 if(descriptor){
  auto* cache=p;
  u32 textureIndex=cache->mCachedTextureIndex;u32 emitterIndex=dc->pbe->mGpuState.mResourceIndex;
  if(textureIndex!=index || (u32)cache->mCachedEmitterIndex!=emitterIndex){
   auto* binding=JPAGetTextureBinding(p->mpRenderer.get(),0);
   if(!sameDescriptor(&binding->mDescriptor,descriptor))JPAApplyTexture(binding,descriptor);
   else {u32 image=descriptor->mImageAddress,mip=descriptor->mMipAddress;binding->mDescriptor.mImageAddress=image;binding->mCachedMipAddress=mip;binding->mCachedImageAddress=image;binding->mDescriptor.mMipAddress=mip;}
   emitterIndex=dc->pbe->mGpuState.mResourceIndex;cache->mCachedTextureIndex=index;cache->mCachedEmitterIndex=emitterIndex;
   auto* name=&static_cast<::JPATextureResource*>(dc->mpTexResource.get())->mNames[(s16)index];
   u32 mode=name->mMode,flags=name->mFlags;
   binding=JPAGetTextureBinding(p->mpRenderer.get(),0);
   if((u32)binding->mBinding[0]!=mode){binding=JPAGetTextureBinding(p->mpRenderer.get(),0);u32 dirty=binding->mDirtyFlags;binding->mBinding[0]=mode;binding->mDirtyFlags=dirty|2;}
   binding=JPAGetTextureBinding(p->mpRenderer.get(),0);
   if((u32)binding->mBinding[1]!=flags){binding=JPAGetTextureBinding(p->mpRenderer.get(),0);u32 dirty=binding->mDirtyFlags;binding->mBinding[1]=flags;binding->mDirtyFlags=dirty|2;}
  }
 }
  link=link->mpNext.get();
 }
}
/* 02834D2C */
void JPADrawExecRotation::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  auto* draw=params(p);
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 globalX=clip->mGlobalScaleX,pivotX=clip->mPivotX,globalY=clip->mGlobalScaleY,pivotY=clip->mPivotY;
  f32 scaleX=draw->mScaleX,scaleY=draw->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=-(gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX))),bottom=-(gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY))),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  u32 angle=draw->mRotateAngle;f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
  Points points;
  JPAConstructArray(&points,4,12,JPAVec3Construct);
  gabi::call_ptr<void>(clip->mBasePlaneTypeFunc,&points,left,right,top,bottom);
  Matrix rotation;
  gabi::call_ptr<void>(clip->mRotTypeFunc,&rotation,sin,cos);
  JPAMultVecArray(&rotation,&points,&points,4);
  f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  for(u32 i=0;i<4;++i){
   f32 x=gabi::fadds_ppc((f32)points.values[i].x,px),y=gabi::fadds_ppc((f32)points.values[i].y,py),z=gabi::fadds_ppc((f32)points.values[i].z,pz);
   vertices[i].x=x;vertices[i].y=y;vertices[i].z=z;
   f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 02834F94 */
void JPADrawExecRotationCross::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  auto* draw=params(p);s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 left=-gabi::fmuls_ppc((f32)draw->mScaleX,gabi::fadds_ppc((f32)clip->mGlobalScaleX,(f32)clip->mPivotX));
  f32 right=gabi::fmuls_ppc((f32)draw->mScaleX,gabi::fsubs_ppc((f32)clip->mGlobalScaleX,(f32)clip->mPivotX));
  f32 top=gabi::fmuls_ppc((f32)draw->mScaleY,gabi::fadds_ppc((f32)clip->mGlobalScaleY,(f32)clip->mPivotY));
  f32 bottom=-gabi::fmuls_ppc((f32)draw->mScaleY,gabi::fsubs_ppc((f32)clip->mGlobalScaleY,(f32)clip->mPivotY));
  u32 angle=draw->mRotateAngle;f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
  PointsCross points;
  JPAConstructArray(&points,8,12,JPAVec3Construct);
  f32 middle=gabi::fadds_ppc(right,left)*0.5f,near=gabi::fsubs_ppc(left,right)*0.5f,far=gabi::fsubs_ppc(right,left)*0.5f;
  points.values[0].x=left;points.values[0].y=top;points.values[0].z=0;
  points.values[1].x=right;points.values[1].y=top;points.values[1].z=0;
  points.values[2].x=right;points.values[2].y=bottom;points.values[2].z=0;
  points.values[3].x=left;points.values[3].y=bottom;points.values[3].z=0;
  points.values[4].x=middle;points.values[4].y=top;points.values[4].z=far;
  points.values[5].x=middle;points.values[5].y=top;points.values[5].z=near;
  points.values[6].x=middle;points.values[6].y=bottom;points.values[6].z=near;
  points.values[7].x=middle;points.values[7].y=bottom;points.values[7].z=far;
  Matrix rotation;
  gabi::call_ptr<void>(clip->mRotTypeFunc,&rotation,sin,cos);
  JPAMultVecCrossArray(&rotation,&points,&points,8);
  f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
  auto* material=JPAGetTextureBinding(p->mpRenderer.get(),0);
  if((u32)material->mBinding[0]!=6){material=JPAGetTextureBinding(p->mpRenderer.get(),0);u32 dirty=material->mDirtyFlags;material->mBinding[0]=6;material->mBinding[1]=6;material->mDirtyFlags=dirty|2;material->mSecondaryBinding=6;}
  for(u32 plane=0;plane<2;++plane){
   auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
   auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+plane*2).mBufferData.get());clearVertexCache(vertices,0x40);
   vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+plane*2).mBufferData.get());
   for(u32 i=0;i<4;++i){
    f32 x=gabi::fadds_ppc((f32)points.values[plane*4+i].x,px),y=gabi::fadds_ppc((f32)points.values[plane*4+i].y,py),z=gabi::fadds_ppc((f32)points.values[plane*4+i].z,pz);
    vertices[i].x=x;vertices[i].y=y;vertices[i].z=z;f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];vertices[i].s=u;vertices[i].t=v;
   }
  }
 }
 JPADrawFinish(p,0);
}
/* 0283497C */
void JPADrawExecDirBillBoard::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  VecFloat dir;
  gabi::call_ptr<void>(clip->mDirTypeFunc,p,dc->pbe.get(),&dir);
  auto* camera=clip->mDrawMtxPtr.get();f32 x=dir.x,y=dir.y,z=dir.z;
  f32 cx=camera->m[2][0],cy=camera->m[2][1],cz=camera->m[2][2];
  f32 nx=gabi::fmsubs(y,cz,z*cy),ny=gabi::fmsubs(z,cx,x*cz),nz=gabi::fmsubs(x,cy,y*cx);
  f32 length=gabi::fmadds(nz,nz,gabi::fmadds(ny,ny,nx*nx));
  dir.z=nz;dir.y=ny;dir.x=nx;
  constexpr f32 epsilon=3.814697265625e-06f;
  if(!(length>epsilon)){p->mDrawSkipped=1;return;}
  length=gabi::fmadds(nz,nz,gabi::fmadds(ny,ny,nx*nx));
  if(length>epsilon){f32 inv=1.0f/(f32)JPADrawSqrt(length);x=dir.x;y=dir.y;z=dir.z;dir.x=x*inv;dir.y=y*inv;dir.z=z*inv;}
  JPAMultVecSR(clip->mDrawMtxPtr.get(),&dir,&dir);
  f32 globalX=clip->mGlobalScaleX,pivotX=clip->mPivotX,globalY=clip->mGlobalScaleY,pivotY=clip->mPivotY;
  f32 scaleX=params(p)->mScaleX,scaleY=params(p)->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX));
  f32 bottom=gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY)),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  f32 sin=dir.y,cos=dir.x;
  Corners corners;
  JPAConstructArray(&corners,4,8,JPAVec2Construct);
  corners.values[0].x=gabi::fmsubs(cos,-left,gabi::fmuls_ppc(sin,top));corners.values[0].y=gabi::fmadds(cos,top,gabi::fmuls_ppc(sin,(-left)));
  corners.values[1].x=gabi::fmsubs(cos,right,gabi::fmuls_ppc(sin,top));corners.values[1].y=gabi::fmadds(cos,top,gabi::fmuls_ppc(sin,right));
  corners.values[2].x=gabi::fmsubs(cos,right,gabi::fmuls_ppc(sin,(-bottom)));corners.values[2].y=gabi::fmadds(cos,-bottom,gabi::fmuls_ppc(sin,right));
  corners.values[3].x=gabi::fmsubs(cos,-left,gabi::fmuls_ppc(sin,(-bottom)));corners.values[3].y=gabi::fmadds(cos,-bottom,gabi::fmuls_ppc(sin,(-left)));
  VecFloat position;
  position.x=p->mGlobalPosition[0];position.z=p->mGlobalPosition[2];position.y=p->mGlobalPosition[1];
  JPAMultVec(clip->mDrawMtxPtr.get(),&position,&position);
  auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
  auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x40);
  vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
  for(u32 i=0;i<4;++i){
   f32 x=gabi::fadds_ppc((f32)corners.values[i].x,(f32)position.x),y=gabi::fadds_ppc((f32)corners.values[i].y,(f32)position.y),z=position.z;
   vertices[i].x=x;vertices[i].z=z;vertices[i].y=y;f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];vertices[i].s=u;vertices[i].t=v;
  }
 }
 JPADrawFinish(p,0);
}
/* 02833DE4 */
void JPADrawExecDirectionalCross::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  auto* draw=params(p);auto* axis=reinterpret_cast<VecFloat*>(&draw->mAxis);
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 globalX=clip->mGlobalScaleX,pivotX=clip->mPivotX,globalY=clip->mGlobalScaleY,pivotY=clip->mPivotY;
  f32 scaleX=draw->mScaleX,scaleY=draw->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=-(gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX))),bottom=-(gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY))),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  
  PointsCross points;
  JPAConstructArray(&points,8,12,JPAVec3Construct);
  f32 middle=gabi::fadds_ppc(right,left)*0.5f,near=gabi::fsubs_ppc(left,right)*0.5f,far=gabi::fsubs_ppc(right,left)*0.5f;
  points.values[0].x=left;points.values[0].y=top;points.values[0].z=0;
  points.values[1].x=right;points.values[1].y=top;points.values[1].z=0;
  points.values[2].x=right;points.values[2].y=bottom;points.values[2].z=0;
  points.values[3].x=left;points.values[3].y=bottom;points.values[3].z=0;
  points.values[4].x=middle;points.values[4].y=top;points.values[4].z=far;
  points.values[5].x=middle;points.values[5].y=top;points.values[5].z=near;
  points.values[6].x=middle;points.values[6].y=bottom;points.values[6].z=near;
  points.values[7].x=middle;points.values[7].y=bottom;points.values[7].z=far;
  
  VecFloat dir;
  gabi::call_ptr<void>(clip->mDirTypeFunc,p,dc->pbe.get(),&dir);
  f32 y=dir.y,x=dir.x,z=dir.z;
  constexpr f32 epsilon=3.814697265625e-06f;
  f32 length=gabi::fmadds(z,z,gabi::fmadds(x,x,y*y));
  if(!(length>epsilon)){p->mDrawSkipped=1;return;}
  if(length>epsilon){
   f32 root=(f32)JPADrawSqrt(length),inv=1.0f/root;
   x=dir.x;y=dir.y;z=dir.z;
   x=x*inv;y=y*inv;z=z*inv;dir.x=x;dir.y=y;dir.z=z;
  }
  f32 ax=axis->x,ay=axis->y,az=axis->z;
  f32 sideX=gabi::fmsubs(ay,z,az*y),sideY=gabi::fmsubs(az,x,ax*z),sideZ=gabi::fmsubs(ax,y,ay*x);
  f32 sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(!(sideLength>epsilon)){p->mDrawSkipped=1;return;}
  sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(sideLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(sideLength);
   sideY=sideY*inv;sideZ=sideZ*inv;sideX=sideX*inv;
   x=dir.x;y=dir.y;z=dir.z;
  }
  f32 nextY=gabi::fmsubs(z,sideX,x*sideZ),nextX=gabi::fmsubs(y,sideZ,z*sideY),nextZ=gabi::fmsubs(x,sideY,y*sideX);
  f32 axisLength=gabi::fmadds(nextZ,nextZ,gabi::fmadds(nextX,nextX,nextY*nextY));
  axis->z=nextZ;axis->y=nextY;axis->x=nextX;
  if(axisLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(axisLength);
   nextX=axis->x;nextY=axis->y;nextZ=axis->z;
   nextX=nextX*inv;nextY=nextY*inv;nextZ=nextZ*inv;
   axis->x=nextX;axis->y=nextY;axis->z=nextZ;
  }
  Matrix mtx;
  mtx.m[0][0]=axis->x;mtx.m[0][1]=dir.x;mtx.m[0][2]=sideX;mtx.m[0][3]=0;
  mtx.m[1][0]=axis->y;mtx.m[1][1]=dir.y;mtx.m[1][2]=sideY;mtx.m[1][3]=0;
  mtx.m[2][0]=axis->z;mtx.m[2][1]=dir.z;mtx.m[2][2]=sideZ;mtx.m[2][3]=0;
  JPAMultVecCrossArray(&mtx,&points,&points,8);
  f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
  auto* material=JPAGetTextureBinding(p->mpRenderer.get(),0);
  if((u32)material->mBinding[0]!=6){material=JPAGetTextureBinding(p->mpRenderer.get(),0);u32 dirty=material->mDirtyFlags;material->mBinding[0]=6;material->mBinding[1]=6;material->mDirtyFlags=dirty|2;material->mSecondaryBinding=6;}
  for(u32 plane=0;plane<2;++plane){
   auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
   auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+plane*2).mBufferData.get());clearVertexCache(vertices,0x40);
   vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+plane*2).mBufferData.get());
   for(u32 i=0;i<4;++i){
    f32 x=gabi::fadds_ppc((f32)points.values[plane*4+i].x,px),y=gabi::fadds_ppc((f32)points.values[plane*4+i].y,py),z=gabi::fadds_ppc((f32)points.values[plane*4+i].z,pz);
    vertices[i].x=x;vertices[i].y=y;vertices[i].z=z;f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];vertices[i].s=u;vertices[i].t=v;
   }
  }
 }
 JPADrawFinish(p,0);
}
/* 02834380 */
void JPADrawExecRotDirectionalCross::exec(const Context* dc,JPABaseParticle* p){
 if((u32)p->mStatus&8)return;
 if(p->mNeedsVertexUpdate){
  auto* draw=params(p);auto* axis=reinterpret_cast<VecFloat*>(&draw->mAxis);
  s32 view=p->mViewIndex;auto* clip=dc->pcb.get();if(view>=0)clip=sead::elementAt(dc->viewClip,u32(view));
  f32 globalX=clip->mGlobalScaleX,pivotX=clip->mPivotX,globalY=clip->mGlobalScaleY,pivotY=clip->mPivotY;
  f32 scaleX=draw->mScaleX,scaleY=draw->mScaleY;
  f32 right=gabi::fmuls_ppc(scaleX,gabi::fsubs_ppc(globalX,pivotX)),left=-(gabi::fmuls_ppc(scaleX,gabi::fadds_ppc(globalX,pivotX))),bottom=-(gabi::fmuls_ppc(scaleY,gabi::fsubs_ppc(globalY,pivotY))),top=gabi::fmuls_ppc(scaleY,gabi::fadds_ppc(globalY,pivotY));
  u32 angle=draw->mRotateAngle;f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
  PointsCross points;
  JPAConstructArray(&points,8,12,JPAVec3Construct);
  f32 middle=gabi::fadds_ppc(right,left)*0.5f,near=gabi::fsubs_ppc(left,right)*0.5f,far=gabi::fsubs_ppc(right,left)*0.5f;
  points.values[0].x=left;points.values[0].y=top;points.values[0].z=0;
  points.values[1].x=right;points.values[1].y=top;points.values[1].z=0;
  points.values[2].x=right;points.values[2].y=bottom;points.values[2].z=0;
  points.values[3].x=left;points.values[3].y=bottom;points.values[3].z=0;
  points.values[4].x=middle;points.values[4].y=top;points.values[4].z=far;
  points.values[5].x=middle;points.values[5].y=top;points.values[5].z=near;
  points.values[6].x=middle;points.values[6].y=bottom;points.values[6].z=near;
  points.values[7].x=middle;points.values[7].y=bottom;points.values[7].z=far;
  Matrix rotation;
  gabi::call_ptr<void>(clip->mRotTypeFunc,&rotation,sin,cos);
  VecFloat dir;
  gabi::call_ptr<void>(clip->mDirTypeFunc,p,dc->pbe.get(),&dir);
  f32 y=dir.y,x=dir.x,z=dir.z;
  constexpr f32 epsilon=3.814697265625e-06f;
  f32 length=gabi::fmadds(z,z,gabi::fmadds(x,x,y*y));
  if(!(length>epsilon)){p->mDrawSkipped=1;return;}
  if(length>epsilon){
   f32 root=(f32)JPADrawSqrt(length),inv=1.0f/root;
   x=dir.x;y=dir.y;z=dir.z;
   x=x*inv;y=y*inv;z=z*inv;dir.x=x;dir.y=y;dir.z=z;
  }
  f32 ax=axis->x,ay=axis->y,az=axis->z;
  f32 sideX=gabi::fmsubs(ay,z,az*y),sideY=gabi::fmsubs(az,x,ax*z),sideZ=gabi::fmsubs(ax,y,ay*x);
  f32 sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(!(sideLength>epsilon)){p->mDrawSkipped=1;return;}
  sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
  if(sideLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(sideLength);
   sideY=sideY*inv;sideZ=sideZ*inv;sideX=sideX*inv;
   x=dir.x;y=dir.y;z=dir.z;
  }
  f32 nextY=gabi::fmsubs(z,sideX,x*sideZ),nextX=gabi::fmsubs(y,sideZ,z*sideY),nextZ=gabi::fmsubs(x,sideY,y*sideX);
  f32 axisLength=gabi::fmadds(nextZ,nextZ,gabi::fmadds(nextX,nextX,nextY*nextY));
  axis->z=nextZ;axis->y=nextY;axis->x=nextX;
  if(axisLength>epsilon){
   f32 inv=1.0f/(f32)JPADrawSqrt(axisLength);
   nextX=axis->x;nextY=axis->y;nextZ=axis->z;
   nextX=nextX*inv;nextY=nextY*inv;nextZ=nextZ*inv;
   axis->x=nextX;axis->y=nextY;axis->z=nextZ;
  }
  Matrix mtx;
  mtx.m[0][0]=axis->x;mtx.m[0][1]=dir.x;mtx.m[0][2]=sideX;mtx.m[0][3]=0;
  mtx.m[1][0]=axis->y;mtx.m[1][1]=dir.y;mtx.m[1][2]=sideY;mtx.m[1][3]=0;
  mtx.m[2][0]=axis->z;mtx.m[2][1]=dir.z;mtx.m[2][2]=sideZ;mtx.m[2][3]=0;
  Matrix combined;
  JPAConcat(&mtx,&rotation,&combined);
  JPAMultVecCrossArray(&combined,&points,&points,8);
  f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
  auto* material=JPAGetTextureBinding(p->mpRenderer.get(),0);
  if((u32)material->mBinding[0]!=6){material=JPAGetTextureBinding(p->mpRenderer.get(),0);u32 dirty=material->mDirtyFlags;material->mBinding[0]=6;material->mBinding[1]=6;material->mDirtyFlags=dirty|2;material->mSecondaryBinding=6;}
  for(u32 plane=0;plane<2;++plane){
   auto* pool=reinterpret_cast<JPAGpuPool<4>*>(p->mVertexPool.get());
   auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+plane*2).mBufferData.get());clearVertexCache(vertices,0x40);
   vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+plane*2).mBufferData.get());
   for(u32 i=0;i<4;++i){
    f32 x=gabi::fadds_ppc((f32)points.values[plane*4+i].x,px),y=gabi::fadds_ppc((f32)points.values[plane*4+i].y,py),z=gabi::fadds_ppc((f32)points.values[plane*4+i].z,pz);
    vertices[i].x=x;vertices[i].y=y;vertices[i].z=z;f32 u=clip->mTexCoordPt[i][0],v=clip->mTexCoordPt[i][1];vertices[i].s=u;vertices[i].t=v;
   }
  }
 }
 JPADrawFinish(p,0);
}
/* 02837B2C */
void JPADrawExecStripeSubmit::exec(const Context* dc){
 auto* list=reinterpret_cast<JPAParticleList*>(dc->mpList.get());if((u32)list->mCount<2)return;
 auto* shape=dc->pbsp.get();u32 order=gabi::call_ptr<u32>(shape->__vtbl->getChildOrder,shape);
 list=reinterpret_cast<JPAParticleList*>(dc->mpList.get());
 auto* link=order?list->mpTail.get():list->mpHead.get();auto* first=link->mpObject.get();
 u32 count=0;link=order?previousParticle(link):nextParticle(link);
 while(link){if(!link->mpObject->mDeletionDelay)++count;link=order?previousParticle(link):nextParticle(link);}
 if(count<2 || first->mHidden || count*2>750)return;
 if(dc->pbe->mGpuState.mTextureCopyEnabled){
  auto* descriptor=JPAStripeDescriptor(sStripePipeline.get(),6);
  if(descriptor){
   auto* binding=JPAGetTextureBinding(first->mpRenderer.get(),0);
   if(!sameDescriptor(&binding->mDescriptor,descriptor)){JPAApplyTexture(binding,descriptor);JPAIssueDraw(first,1,(u16)(count*2),1);return;}
   u32 image=descriptor->mImageAddress,mip=descriptor->mMipAddress;binding->mDescriptor.mImageAddress=image;binding->mCachedMipAddress=mip;binding->mCachedImageAddress=image;binding->mDescriptor.mMipAddress=mip;
  }
 }
 JPAIssueDraw(first,1,(u16)(count*2),1);
}
/* 0283540C */
void JPADrawExecStripe::exec(const Context* dc){
 auto* list=reinterpret_cast<JPAParticleList*>(dc->mpList.get());if((u32)list->mCount<2)return;
 auto* shape=dc->pbsp.get();u32 order=gabi::call_ptr<u32>(shape->__vtbl->getChildOrder,shape);
 list=reinterpret_cast<JPAParticleList*>(dc->mpList.get());
 auto* start=order?list->mpTail.get():list->mpHead.get();
 auto* first=start->mpObject.get();auto* link=order?previousParticle(start):nextParticle(start);
 if(!link)return;
 u32 count=0;
 while(link){if(!link->mpObject->mDeletionDelay)++count;link=order?previousParticle(link):nextParticle(link);}
 if(count<2)return;
 f32 initialT=order?1.0f:0.0f,texStep=(order?-1.0f:1.0f)/(f32)(count-1);
 shape=dc->pbsp.get();if(gabi::call_ptr<u32>(shape->__vtbl->getChildOrder,shape))texStep=-texStep;
 if(first->mHidden)return;
 JPAPrepareStripe(first,dc->pbe.get());if(count*2>750)return;
 auto* pool=reinterpret_cast<JPAGpuPool<4>*>(first->mpSlot.get());
 auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x3A80);
 vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
 constexpr f32 epsilon=3.814697265625e-06f;
 VecFloat dir;
 f32 texT=initialT;u32 output=0;link=start;

 while(link){
  auto* p=link->mpObject.get();
  if(!p->mDeletionDelay){
   auto* clip=dc->pcb.get();auto* draw=params(p);
   u32 angle=draw->mRotateAngle;
   f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
   f32 width0=-gabi::fmuls_ppc((f32)draw->mScaleX,gabi::fadds_ppc((f32)clip->mGlobalScaleX,(f32)clip->mPivotX)),width1=gabi::fmuls_ppc((f32)draw->mScaleX,gabi::fsubs_ppc((f32)clip->mGlobalScaleX,(f32)clip->mPivotX));
   f32 sin0=width0*sin,cos0=width0*cos,sin1=width1*sin,cos1=width1*cos;
   f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
   gabi::call_ptr<void>(clip->mDirTypeFunc,p,dc->pbe.get(),&dir);

   f32 x=dir.x,y=dir.y,z=dir.z;
   f32 length=gabi::fmadds(z,z,gabi::fmadds(x,x,y*y));
   if(!(length>epsilon)){x=0;y=1;z=0;dir.x=x;dir.y=y;dir.z=z;}
   else if(length>epsilon){
    f32 inv=1.0f/(f32)JPADrawSqrt(length);x=dir.x;y=dir.y;z=dir.z;
    x=x*inv;y=y*inv;z=z*inv;dir.x=x;dir.y=y;dir.z=z;
   }
   auto* axis=reinterpret_cast<VecFloat*>(&draw->mAxis);f32 ax=axis->x,ay=axis->y,az=axis->z;
   f32 sideX=gabi::fmsubs(ay,z,az*y),sideY=gabi::fmsubs(az,x,ax*z),sideZ=gabi::fmsubs(ax,y,ay*x);
   f32 sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
   if(!(sideLength>epsilon)){sideX=1;sideY=0;sideZ=0;}
   else {
    sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
    if(sideLength>epsilon){f32 inv=1.0f/(f32)JPADrawSqrt(sideLength);sideY=sideY*inv;sideZ=sideZ*inv;sideX=sideX*inv;x=dir.x;y=dir.y;z=dir.z;}
   }
   f32 nextX=gabi::fmsubs(y,sideZ,z*sideY),nextY=gabi::fmsubs(z,sideX,x*sideZ),nextZ=gabi::fmsubs(x,sideY,y*sideX);
   axis->x=nextX;axis->y=nextY;axis->z=nextZ;
   f32 axisLength=gabi::fmadds(nextZ,nextZ,gabi::fmadds((f32)axis->x,(f32)axis->x,(f32)axis->y*(f32)axis->y));
   if(axisLength>epsilon){
    f32 inv=1.0f/(f32)JPADrawSqrt(axisLength);nextX=axis->x;nextY=axis->y;nextZ=axis->z;
    nextX=nextX*inv;nextY=nextY*inv;nextZ=nextZ*inv;axis->x=nextX;axis->y=nextY;axis->z=nextZ;
   }
   x=dir.x;y=dir.y;z=dir.z;ax=axis->x;ay=axis->y;az=axis->z;
   f32 zeroX=0.0f*x,zeroY=0.0f*y,zeroZ=0.0f*z;
   f32 vx0=gabi::fmadds(sin0,ax,gabi::fmadds(cos0,sideX,zeroX));
   f32 vy0=gabi::fmadds(sin0,ay,gabi::fmadds(cos0,sideY,zeroY));
   f32 vz0=gabi::fmadds(sin0,az,gabi::fmadds(cos0,sideZ,zeroZ));
   f32 vx1=gabi::fmadds(sin1,ax,gabi::fmadds(cos1,sideX,zeroX));
   f32 vy1=gabi::fmadds(sin1,ay,gabi::fmadds(cos1,sideY,zeroY));
   f32 vz1=gabi::fmadds(sin1,az,gabi::fmadds(cos1,sideZ,zeroZ));
   vertices[output].t=texT;vertices[output].x=gabi::fadds_ppc(vx0,px);vertices[output].s=0;vertices[output].y=gabi::fadds_ppc(vy0,py);vertices[output].z=gabi::fadds_ppc(vz0,pz);
   vertices[output+1].y=gabi::fadds_ppc(vy1,py);vertices[output+1].t=texT;vertices[output+1].z=gabi::fadds_ppc(vz1,pz);vertices[output+1].x=gabi::fadds_ppc(vx1,px);
   output+=2;texT=gabi::fadds_ppc(texT,texStep);vertices[output-1].s=1;

  }
  link=order?previousParticle(link):nextParticle(link);
 }
 JPADrawFinish(first,1);
}
/* 02835A20 */
void JPADrawExecStripeCross::exec(const Context* dc){
 auto* list=reinterpret_cast<JPAParticleList*>(dc->mpList.get());if((u32)list->mCount<2)return;
 auto* shape=dc->pbsp.get();u32 order=gabi::call_ptr<u32>(shape->__vtbl->getChildOrder,shape);
 list=reinterpret_cast<JPAParticleList*>(dc->mpList.get());
 auto* start=order?list->mpTail.get():list->mpHead.get();
 JPABaseParticle* first=nullptr;auto* link=order?previousParticle(start):nextParticle(start);
 if(!link)return;
 u32 count=0;
 while(link){if(!link->mpObject->mDeletionDelay)++count;link=order?previousParticle(link):nextParticle(link);}
 if(count<2)return;
 f32 initialT=order?1.0f:0.0f,texStep=(order?-1.0f:1.0f)/(f32)(count-1);
 shape=dc->pbsp.get();if(gabi::call_ptr<u32>(shape->__vtbl->getChildOrder,shape))texStep=-texStep;
 first=start->mpObject.get();
 auto* pool=reinterpret_cast<JPAGpuPool<4>*>(first->mpSlot.get());
 auto* vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());clearVertexCache(vertices,0x3A80);
 vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex).mBufferData.get());
 if(first->mHidden)return;
 JPAPrepareStripe(first,dc->pbe.get());if(count*2>750)return;
 constexpr f32 epsilon=3.814697265625e-06f;
 VecFloat dir;
 f32 texT=initialT;u32 output=0;link=start;

 while(link){
  auto* p=link->mpObject.get();
  if(!p->mDeletionDelay){
   auto* clip=dc->pcb.get();auto* draw=params(p);
   u32 angle=draw->mRotateAngle;
   f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
   f32 width0=-gabi::fmuls_ppc((f32)draw->mScaleX,gabi::fadds_ppc((f32)clip->mGlobalScaleX,(f32)clip->mPivotX)),width1=gabi::fmuls_ppc((f32)draw->mScaleX,gabi::fsubs_ppc((f32)clip->mGlobalScaleX,(f32)clip->mPivotX));
   f32 sin0=width0*sin,cos0=width0*cos,sin1=width1*sin,cos1=width1*cos;
   f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
   gabi::call_ptr<void>(clip->mDirTypeFunc,p,dc->pbe.get(),&dir);

   f32 x=dir.x,y=dir.y,z=dir.z;
   f32 length=gabi::fmadds(z,z,gabi::fmadds(x,x,y*y));
   if(!(length>epsilon)){x=0;y=1;z=0;dir.x=x;dir.y=y;dir.z=z;}
   else if(length>epsilon){
    f32 inv=1.0f/(f32)JPADrawSqrt(length);x=dir.x;y=dir.y;z=dir.z;
    x=x*inv;y=y*inv;z=z*inv;dir.x=x;dir.y=y;dir.z=z;
   }
   auto* axis=reinterpret_cast<VecFloat*>(&draw->mAxis);f32 ax=axis->x,ay=axis->y,az=axis->z;
   f32 sideX=gabi::fmsubs(ay,z,az*y),sideY=gabi::fmsubs(az,x,ax*z),sideZ=gabi::fmsubs(ax,y,ay*x);
   f32 sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
   if(!(sideLength>epsilon)){sideX=1;sideY=0;sideZ=0;}
   else {
    sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
    if(sideLength>epsilon){f32 inv=1.0f/(f32)JPADrawSqrt(sideLength);sideY=sideY*inv;sideZ=sideZ*inv;sideX=sideX*inv;x=dir.x;y=dir.y;z=dir.z;}
   }
   f32 nextX=gabi::fmsubs(y,sideZ,z*sideY),nextY=gabi::fmsubs(z,sideX,x*sideZ),nextZ=gabi::fmsubs(x,sideY,y*sideX);
   axis->x=nextX;axis->y=nextY;axis->z=nextZ;
   f32 axisLength=gabi::fmadds(nextZ,nextZ,gabi::fmadds((f32)axis->x,(f32)axis->x,(f32)axis->y*(f32)axis->y));
   if(axisLength>epsilon){
    f32 inv=1.0f/(f32)JPADrawSqrt(axisLength);nextX=axis->x;nextY=axis->y;nextZ=axis->z;
    nextX=nextX*inv;nextY=nextY*inv;nextZ=nextZ*inv;axis->x=nextX;axis->y=nextY;axis->z=nextZ;
   }
   x=dir.x;y=dir.y;z=dir.z;ax=axis->x;ay=axis->y;az=axis->z;
   f32 zeroX=0.0f*x,zeroY=0.0f*y,zeroZ=0.0f*z;
   f32 vx0=gabi::fmadds(sin0,ax,gabi::fmadds(cos0,sideX,zeroX));
   f32 vy0=gabi::fmadds(sin0,ay,gabi::fmadds(cos0,sideY,zeroY));
   f32 vz0=gabi::fmadds(sin0,az,gabi::fmadds(cos0,sideZ,zeroZ));
   f32 vx1=gabi::fmadds(sin1,ax,gabi::fmadds(cos1,sideX,zeroX));
   f32 vy1=gabi::fmadds(sin1,ay,gabi::fmadds(cos1,sideY,zeroY));
   f32 vz1=gabi::fmadds(sin1,az,gabi::fmadds(cos1,sideZ,zeroZ));
   vertices[output].t=texT;vertices[output].x=gabi::fadds_ppc(vx0,px);vertices[output].s=0;vertices[output].y=gabi::fadds_ppc(vy0,py);vertices[output].z=gabi::fadds_ppc(vz0,pz);
   vertices[output+1].y=gabi::fadds_ppc(vy1,py);vertices[output+1].t=texT;vertices[output+1].z=gabi::fadds_ppc(vz1,pz);vertices[output+1].x=gabi::fadds_ppc(vx1,px);
   output+=2;texT=gabi::fadds_ppc(texT,texStep);vertices[output-1].s=1;

  }
  link=order?previousParticle(link):nextParticle(link);
 }
 pool=reinterpret_cast<JPAGpuPool<4>*>(first->mpSlot.get());vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+2).mBufferData.get());clearVertexCache(vertices,0x3A80);
 vertices=static_cast<Vertex*>(sead::elementAt(pool->mBuffers, (u32)pool->mState.mSwapIndex+2).mBufferData.get());texT=initialT;output=0;link=start;

 while(link){
  auto* p=link->mpObject.get();
  if(!p->mDeletionDelay){
   auto* clip=dc->pcb.get();auto* draw=params(p);
   u32 angle=draw->mRotateAngle;
   f32 sin=sSinCosTable.mEntries[angle>>3].mSin,cos=sSinCosTable.mEntries[angle>>3].mCos;
   f32 width0=gabi::fmuls_ppc((f32)draw->mScaleY,gabi::fadds_ppc((f32)clip->mGlobalScaleY,(f32)clip->mPivotY)),width1=-gabi::fmuls_ppc((f32)draw->mScaleY,gabi::fsubs_ppc((f32)clip->mGlobalScaleY,(f32)clip->mPivotY));
   f32 oldSin=sin;sin=cos;cos=-oldSin;
   f32 sin0=width0*sin,cos0=width0*cos,sin1=width1*sin,cos1=width1*cos;
   f32 pz=p->mGlobalPosition[2],py=p->mGlobalPosition[1],px=p->mGlobalPosition[0];
   dir.x=p->mVelocity[0];dir.y=p->mVelocity[1];dir.z=p->mVelocity[2];

   f32 x=dir.x,y=dir.y,z=dir.z;
   f32 length=gabi::fmadds(z,z,gabi::fmadds(x,x,y*y));
   if(!(length>epsilon)){x=0;y=1;z=0;dir.x=x;dir.y=y;dir.z=z;}
   else if(length>epsilon){
    f32 inv=1.0f/(f32)JPADrawSqrt(length);x=dir.x;y=dir.y;z=dir.z;
    x=x*inv;y=y*inv;z=z*inv;dir.x=x;dir.y=y;dir.z=z;
   }
   auto* axis=reinterpret_cast<VecFloat*>(&draw->mAxis);f32 ax=axis->x,ay=axis->y,az=axis->z;
   f32 sideX=gabi::fmsubs(ay,z,az*y),sideY=gabi::fmsubs(az,x,ax*z),sideZ=gabi::fmsubs(ax,y,ay*x);
   f32 sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
   if(!(sideLength>epsilon)){sideX=1;sideY=0;sideZ=0;}
   else {
    sideLength=gabi::fmadds(sideZ,sideZ,gabi::fmadds(sideX,sideX,sideY*sideY));
    if(sideLength>epsilon){f32 inv=1.0f/(f32)JPADrawSqrt(sideLength);sideY=sideY*inv;sideZ=sideZ*inv;sideX=sideX*inv;x=dir.x;y=dir.y;z=dir.z;}
   }
   f32 nextX=gabi::fmsubs(y,sideZ,z*sideY),nextY=gabi::fmsubs(z,sideX,x*sideZ),nextZ=gabi::fmsubs(x,sideY,y*sideX);
   axis->x=nextX;axis->y=nextY;axis->z=nextZ;
   f32 axisLength=gabi::fmadds(nextZ,nextZ,gabi::fmadds((f32)axis->x,(f32)axis->x,(f32)axis->y*(f32)axis->y));
   if(axisLength>epsilon){
    f32 inv=1.0f/(f32)JPADrawSqrt(axisLength);nextX=axis->x;nextY=axis->y;nextZ=axis->z;
    nextX=nextX*inv;nextY=nextY*inv;nextZ=nextZ*inv;axis->x=nextX;axis->y=nextY;axis->z=nextZ;
   }
   x=dir.x;y=dir.y;z=dir.z;ax=axis->x;ay=axis->y;az=axis->z;
   f32 zeroX=0.0f*x,zeroY=0.0f*y,zeroZ=0.0f*z;
   f32 vx0=gabi::fmadds(sin0,ax,gabi::fmadds(cos0,sideX,zeroX));
   f32 vy0=gabi::fmadds(sin0,ay,gabi::fmadds(cos0,sideY,zeroY));
   f32 vz0=gabi::fmadds(sin0,az,gabi::fmadds(cos0,sideZ,zeroZ));
   f32 vx1=gabi::fmadds(sin1,ax,gabi::fmadds(cos1,sideX,zeroX));
   f32 vy1=gabi::fmadds(sin1,ay,gabi::fmadds(cos1,sideY,zeroY));
   f32 vz1=gabi::fmadds(sin1,az,gabi::fmadds(cos1,sideZ,zeroZ));
   vertices[output].t=texT;vertices[output].x=gabi::fadds_ppc(vx0,px);vertices[output].s=0;vertices[output].y=gabi::fadds_ppc(vy0,py);vertices[output].z=gabi::fadds_ppc(vz0,pz);
   vertices[output+1].y=gabi::fadds_ppc(vy1,py);vertices[output+1].t=texT;vertices[output+1].z=gabi::fadds_ppc(vz1,pz);vertices[output+1].x=gabi::fadds_ppc(vx1,px);
   output+=2;texT=gabi::fadds_ppc(texT,texStep);vertices[output-1].s=1;

  }
  link=order?previousParticle(link):nextParticle(link);
 }
 JPADrawFinish(first,1);
}
}
/* WWHD glue
 ext 028255F8 void JPAPrepareStripe(JPABaseParticle*,jpa_emitter_calc::JPABaseEmitter*)
 data sStripePipeline 101F9968
 ext 027F81A4 JPAMathDraw::JPATextureDescriptor* JPAStripeDescriptor(void*,u32)
 ext 028E8FB8 void JPAMultVecCrossArray(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::PointsCross*,jpa_drawvisitor::PointsCross*,u32)
 ext 027FE640 JPAMathDraw::JPATextureState* JPAGetTextureBinding(JPAParticleRenderer*,u32)
 ext 027BDEB4 void JPAApplyTexture(JPAMathDraw::JPATextureState*,const JPAMathDraw::JPATextureDescriptor*)
 ext 027FC80C void JPAGpuTextureMtx(JPAGpuState*,const jpa_drawvisitor::Matrix*,u32)
 ext 028F37F0 f64 JPADrawSqrt(f32)
 ext 028E9108 void JPAConcat(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::Matrix*,jpa_drawvisitor::Matrix*)
 ext 02837F9C void JPAVec3Construct(void*) callback
 ext 028E8FB8 void JPAMultVecArray(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::Points*,jpa_drawvisitor::Points*,u32)
 ext 028E9044 void JPAMultVecSR(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::VecFloat*,jpa_drawvisitor::VecFloat*)
 ext 02837FD0 void JPAVec2Construct(void*) callback
 ext 028EFFD0 void JPAConstructArray(void*,u32,u32,JPAVecCtor)
 ext 028E8F64 void JPAMultVec(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::VecFloat*,jpa_drawvisitor::VecFloat*)
 ext 028E91EC bool JPAInverse(const jpa_drawvisitor::Matrix*,jpa_drawvisitor::Matrix*)
 ext 02825158 void JPADrawFinish(JPABaseParticle*,u32)
 ext 02825284 void JPAIssueDraw(JPABaseParticle*,u32,u32,u32)
 ext 0281E1B8 void JPASetEmitterTextureMtx(jpa_emitter_calc::JPABaseEmitter*,const jpa_drawvisitor::Matrix*)
 ext 0281E950 u32 JPADrawEmitterGroup(jpa_emitter_calc::JPABaseEmitter*)
 ext 0273AA24 void JPADrawAssert(const char*,u32,const char*)
 ext 0282581C void JPASetPointSize(JPABaseParticle*,u8)
 ext 02825430 void JPASetPrmColor(JPABaseParticle*,const jpa_drawvisitor::ColorFloat*)
 ext 02825514 void JPASetEnvColor(JPABaseParticle*,const jpa_drawvisitor::ColorFloat*)
 str 10172264
 str 10172278
*/
