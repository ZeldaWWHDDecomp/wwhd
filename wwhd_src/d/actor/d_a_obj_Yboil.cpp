/* Volcano boiling lava: private port of the actual HD translation unit. */
#include "d/actor/d_a_obj_Yboil.h"
static u8* ptr(u32 a) { return gabi::at<u8>(a); }
BOOL daObjYboil_c::CreateHeap() {
 WWHD_FUNC(0x023BC3D8,BOOL,this);
 const u32 indices[]={6,9,15,12},lines[]={0x5C,0x60,0x64,0x68};
 const u32 expressions[]={0x10033994,0x100339A8,0x100339B4,0x100339C0};
 u32 resources[4];
 for(u32 j=0;j<4;j++) {
  resources[j]=gabi::ea(dComIfG_getObjectRes(STR(0x10033978),indices[j],0x10033924));
  if(!resources[j]) JUT_ASSERT_fail(STR(0x10033980),lines[j],STR(expressions[j]));
 }
 for(u32 i=0;i<50;i++) {
  mModels[i]=mDoExt_J3DModel__create((J3DModelData*)ptr(resources[0]),0x80000,0x11000222);
  if(!mModels[i]) return 0;
  if(!mBck[i].init((J3DModelData*)ptr(resources[0]),(J3DAnmTransform*)ptr(resources[1]),true,0,1.0f,0,-1,false)) return 0;
  if(!mBtk[i].init((J3DModelData*)ptr(resources[0]),(J3DAnmTextureSRTKey*)ptr(resources[2]),true,0,1.0f,0,-1,false,0)) return 0;
  if(!gabi::call<BOOL>(0x025E8154,&mBrk[i],ptr(resources[0]),ptr(resources[3]),true,0,1.0f,(s16)0,(s16)-1,false,0)) return 0;
 }
 return 1;
}
VERIFY(0x023BC3D8,&daObjYboil_c::CreateHeap);
static BOOL yboilHeapCB(daObjYboil_c* self) {
 WWHD_FUNC(0x023BC624,BOOL,self); return gabi::call<BOOL>(0x023BC3D8,self);
}
VERIFY(0x023BC624,yboilHeapCB);
void daObjYboil_c::pos_reset(s32 i) {
 WWHD_FUNC(0x023BC628,void,this,i);
 f32 spread=(f32)mScaleMax-(f32)mScaleMin;
 f32 size=cM_rndF(spread)+(f32)mScaleMin;
 mScales[i].x=size; mScales[i].y=size; mScales[i].z=size;
 f32 distance=cM_rndF(1400.0f)+1300.0f;
 u16 angle=(u16)gabi::ftoi(cM_rndF(65536.0f));
 mPositions[i].x=gabi::fmadds(distance,cM_ssin(angle),current.pos.x);
 mPositions[i].y=current.pos.y;
 mPositions[i].z=gabi::fmadds(distance,cM_scos(angle),current.pos.z);
}
VERIFY(0x023BC628,&daObjYboil_c::pos_reset);
void daObjYboil_c::set_mtx() {
 WWHD_FUNC(0x023BC718,void,this);
 for(u32 i=0;i<50;i++) {
  J3DModel_setBaseScale(mModels[i],&mScales[i]);
  mDoMtx_stack_c::transS(mPositions[i].x,mPositions[i].y,mPositions[i].z);
  J3DModel_setBaseTRMtx(mModels[i],mDoMtx_stack_c::get());
 }
}
VERIFY(0x023BC718,&daObjYboil_c::set_mtx);
void daObjYboil_c::CreateInit() {
 WWHD_FUNC(0x023BC800,void,this);
 mScaleMin=0.5f; mScaleMax=1.0f;
 for(u32 i=0;i<50;i++) {
  pos_reset(i);
  cullMtx=gabi::ea(J3DModel_getBaseTRMtx(mModels[i]));
  fopAcM_setCullSizeBox(this,-3000.0f,-0.0f,-3000.0f,3000.0f,200.0f,3000.0f);
  cullSizeFar=1.0f;
  f32 frame=(f32)gabi::ftoi(cM_rndF(29.999900817871094f));
  mBck[i].mFrameCtrl.mFrame=frame; mBtk[i].mFrameCtrl.mFrame=frame; mBrk[i].mFrameCtrl.mFrame=frame;
  mTimers[i]=0;
 }
 set_mtx();
}
VERIFY(0x023BC800,&daObjYboil_c::CreateInit);
static s32 yboilCreate(daObjYboil_c* self) {
 WWHD_FUNC(0x023BC9BC,s32,self);
 if(!(self->actor_condition&8)) {
  if(self) {
   gabi::call(0x025D4ED0,self); gabi::store<u32>(gabi::ea(self)+0xB4,0x10033964);
   gabi::call(0x028EFFD0,&self->mBck[0],50,0x8C,0x023BD294);
   gabi::call(0x028EFFD0,&self->mBtk[0],50,0x74,0x025E7C6C);
   gabi::call(0x028EFFD0,&self->mBrk[0],50,0x78,0x025E80D0);
  }
  self->actor_condition=(u32)self->actor_condition|8;
 }
 if(gabi::call<BOOL>(0x025B8B94,ptr(gabi::load<u32>(0x101F84DC)+0x644),0x1902)) return 3;
 s32 result=gabi::call<s32>(0x02520460,&self->mPhase,STR(0x1003391C));
 if(result==4) {
  if(!gabi::call<BOOL>(0x025D63E8,self,0x023BC624,0x14500)) return 5;
  self->CreateInit();
 }
 return result;
}
VERIFY(0x023BC9BC,yboilCreate);
static BOOL yboilDelete(daObjYboil_c* self) {
 WWHD_FUNC(0x023BCB04,BOOL,self);
 if(gabi::load<u8>(gabi::ea(self)+0xD)!=3) gabi::call(0x025204C8,&self->mPhase,STR(0x100339F8));
 return 1;
}
VERIFY(0x023BCB04,yboilDelete);
struct YboilByteColor { be<u8> rgba[4]; };
struct YboilFloatColor { be<f32> r,g,b,a; };
static void yboilNormalize(YboilFloatColor* out,u8* color) {
 WWHD_FUNC(0x023BCB40,void,out,color);
 u32 source=gabi::ea(color),dest=gabi::ea(out);
 f32 r=gabi::load<u8>(source),g=gabi::load<u8>(source+1),b=gabi::load<u8>(source+2),a=gabi::load<u8>(source+3);
 gabi::store<f32>(dest,r/255.0f);gabi::store<f32>(dest+4,g/255.0f);
 gabi::store<f32>(dest+8,b/255.0f);gabi::store<f32>(dest+12,a/255.0f);
}
VERIFY(0x023BCB40,yboilNormalize);
static u32 colorGet(u32 material,u32 slot) {
 u32 tev=gabi::load<u32>(material+0x18);
 u32 target=gabi::load<u32>(gabi::load<u32>(tev+4)+slot);
 return gabi::call_ptr<u32>(target,ptr(tev),0);
}
static void colorSet(u32 material,u32 slot,u32 color) {
 u32 tev=gabi::load<u32>(material+0x18);
 u32 target=gabi::load<u32>(gabi::load<u32>(tev+4)+slot);
 gabi::call_ptr(target,ptr(tev),0,ptr(color));
}
static void renderList(u32 first,u32 second) {
 u32 play=gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4634,gabi::load<u32>(play+first));
 play=gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4638,gabi::load<u32>(play+second));
}
static BOOL yboilDraw(daObjYboil_c* self) {
 WWHD_FUNC(0x023BCBF4,BOOL,self);
 renderList(0x5D70,0x5D74);
 u32 env=gabi::call<u32>(0x02555D0C);
 gabi::call(0x025626A4,ptr(env),1,&self->current.pos,ptr(gabi::ea(self)+0x110));
 for(u32 i=0;i<50;i++) {
  env=gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C,ptr(env),(J3DModel*)self->mModels[i],ptr(gabi::ea(self)+0x110));
  u32 model=gabi::ea(self->mModels[i]);
  u32 data=gabi::load<u32>(model+0xAC);
  u32 material=gabi::load<u32>(gabi::load<u32>(data+8)+0x10);
  while(material) {
   gabi::Local<YboilByteColor> ambient,diffuse;
   gabi::Local<YboilFloatColor> input,converted;
   gabi::call(0x025602F0,ambient.get(),diffuse.get());
   for(u32 component=0;component<3;component++) {
    u32 destination=colorGet(material,0x34);
    u16 value=ambient->rgba[component];
    gabi::store<u16>(destination+component*2,value);
   }
   u32 color=colorGet(material,0x34);
   colorSet(material,0x24,color);
   input->r=(f32)gabi::load<s16>(color)/255.0f;
   input->g=(f32)gabi::load<s16>(color+2)/255.0f;
   input->b=(f32)gabi::load<s16>(color+4)/255.0f;
   input->a=(f32)gabi::load<s16>(color+6)/255.0f;
   gabi::call(0x0274D458,converted.get(),input.get(),1.0f);
   gabi::store<u32>(material+0xA0,gabi::load<u32>(material+0xA0)|0x10);
   u32 output=gabi::call<u32>(0x027F9F0C,ptr(material+0xA0),4);
   f32 alpha=(f32)gabi::load<s16>(color+6)/255.0f;
   // The generated HD green load/store preserves its signaling payload;
   // red and blue round through double FPRs. Aliased color buffers expose this.
   u32 rgb=gabi::ea(converted.get());
   f32 red=converted->r,blue=converted->b;
   u32 green=gabi::load<u32>(rgb+4);
   gabi::store<u32>(output+4,green);gabi::store<f32>(output+8,blue);
   gabi::store<f32>(output,red);gabi::store<f32>(output+12,alpha);
   for(u32 component=0;component<3;component++) {
    u32 destination=colorGet(material,0x4C);
    u8 value=diffuse->rgba[component];
    gabi::store<u8>(destination+component,value);
   }
   color=colorGet(material,0x4C);
   colorSet(material,0x3C,color);
   yboilNormalize(input.get(),ptr(color));
   gabi::call(0x0274D458,converted.get(),input.get(),1.0f);
   gabi::store<u32>(material+0xA0,gabi::load<u32>(material+0xA0)|0x80);
   output=gabi::call<u32>(0x027F9F0C,ptr(material+0xA0),7);
   alpha=(f32)gabi::load<u8>(color+3)/255.0f;
   // This second HD conversion rounds RGB through double FPRs.
   f32 redFloat=converted->r,greenFloat=converted->g,blueFloat=converted->b;
   gabi::store<f32>(output+4,greenFloat);gabi::store<f32>(output+8,blueFloat);
   gabi::store<f32>(output,redFloat);gabi::store<f32>(output+12,alpha);
   material=gabi::load<u32>(material+4);
  }
  data=gabi::load<u32>(gabi::ea(self->mModels[i])+0xAC);
  self->mBck[i].entry((J3DModelData*)ptr(data),self->mBck[i].mFrameCtrl.mFrame);
  data=gabi::load<u32>(gabi::ea(self->mModels[i])+0xAC);
  self->mBtk[i].entry((J3DModelData*)ptr(data),self->mBtk[i].mFrameCtrl.mFrame);
  data=gabi::load<u32>(gabi::ea(self->mModels[i])+0xAC);
  gabi::call(0x025E83FC,&self->mBrk[i],ptr(data),(f32)self->mBrk[i].mFrameCtrl.mFrame);
  gabi::call(0x025E2DE0,(J3DModel*)self->mModels[i],0);
 }
 renderList(0x5D78,0x5D7C);
 return 1;
}
VERIFY(0x023BCBF4,yboilDraw);
static u32 yboilParameter(daObjYboil_c* self,u32 width,u32 shift) {
 WWHD_FUNC(0x023BD3F8,u32,self,width,shift);
 u32 mask=width&32?0:1u<<(width&31);
 u32 value=shift&32?0:(u32)self->mParameters>>(shift&31);
 return value&(mask-1);
}
VERIFY(0x023BD3F8,yboilParameter);
static BOOL yboilExecute(daObjYboil_c* self) {
 WWHD_FUNC(0x023BD02C,BOOL,self);
 for(u32 i=0;i<50;i++) {
  if(self->mTimers[i]==0) {
   self->mBck[i].play();self->mBtk[i].play();self->mBrk[i].play();
   if(self->mBck[i].mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || self->mBck[i].mFrameCtrl.mRate==0.0f) {
    self->mBck[i].mFrameCtrl.mFrame=0;self->mBtk[i].mFrameCtrl.mFrame=0;self->mBrk[i].mFrameCtrl.mFrame=0;
    self->mBck[i].mFrameCtrl.mRate=1;self->mBtk[i].mFrameCtrl.mRate=1;self->mBrk[i].mFrameCtrl.mRate=1;
    self->mTimers[i]=(u32)gabi::ftoi(cM_rndF(3.9999001026153564f))+1;
   }
  } else {
   u32 sw=yboilParameter(self,8,8);
   u32 save=gabi::load<u32>(0x101F84DC);
   if(!gabi::call<BOOL>(0x025BA0C0,ptr(save+0x20),sw,(s32)(s8)self->home.roomNo)) {
    u32 timer=(u32)self->mTimers[i]-1;
    self->mTimers[i]=timer;
    if(timer==0) self->pos_reset(i);
   }
  }
 }
 self->set_mtx();return 1;
}
VERIFY(0x023BD02C,yboilExecute);
static void yboilStaticInit() {
 WWHD_FUNC(0x023BD1EC,void);
 for(u32 i=0;i<4;i++) gabi::store<u32>(0x1046CA5C+i*4,0);
 gabi::call(0x028F026C,ptr(0x101CDF7C));
 gabi::store<f32>(0x1046CA50,-3.1415927410125732f);gabi::store<f32>(0x1046CA54,3.1415927410125732f);
 gabi::call(0x028ED6F8,ptr(0x1046CA58));gabi::call(0x028F026C,ptr(0x101CDF88));
 gabi::call(0x028EAB2C,ptr(0x1046CA59));gabi::call(0x028F026C,ptr(0x101CDF94));
}
VERIFY(0x023BD1EC,yboilStaticInit);
static void yboilTrivialDtor(u8* self,u32 flags) {
 WWHD_FUNC(0x023BD280,void,self,flags); if(self && (flags&1)) gabi::call(0x0273AF40,self);
}
VERIFY(0x023BD280,yboilTrivialDtor);
static u8* yboilBckCtor(u8* self) {
 WWHD_FUNC(0x023BD294,u8*,self);
 if(!self) { self=gabi::call<u8*>(0x0273AD10,0x8C); if(!self) return nullptr; }
 gabi::call(0x027F2BC0,self,0);
 u32 a=gabi::ea(self);gabi::store<u32>(a+0x10,0x1016E54C);
 gabi::call(0x027DA984,ptr(a+0x14));
 gabi::store<u32>(a+0x88,0);gabi::store<u32>(a+0x48,0x1016D820);
 gabi::store<u32>(a+0x58,0);gabi::store<u32>(a+0x84,0);
 gabi::store<u32>(a+0x10,0x1003393C);gabi::store<u32>(a+0x80,0);gabi::store<u32>(a+0x7C,0);
 return self;
}
VERIFY(0x023BD294,yboilBckCtor);
static BOOL yboilIsDelete(daObjYboil_c* self) {
 WWHD_FUNC(0x023BD324,BOOL,self);return 1;
}
VERIFY(0x023BD324,yboilIsDelete);
static void yboilBckDtor(u8* self,u32 flags) {
 WWHD_FUNC(0x023BD32C,void,self,flags);
 if(self) { gabi::call(0x027F3628,ptr(gabi::ea(self)+0x10),0);if(flags&1) gabi::call(0x0273AF40,self); }
}
VERIFY(0x023BD32C,yboilBckDtor);
static void yboilActorDtor(daObjYboil_c* self,u32 flags) {
 WWHD_FUNC(0x023BD380,void,self,flags);
 if(self) {
  gabi::call(0x028F0164,&self->mBck[0],50,0x8C,0x023BD32C,0,0);
  gabi::call(0x025D50BC,self,0);if(flags&1) gabi::call(0x0273AF40,self);
 }
}
VERIFY(0x023BD380,yboilActorDtor);
static void yboilEmpty(u8* self) { WWHD_FUNC(0x023BD3F4,void,self); }
VERIFY(0x023BD3F4,yboilEmpty);
