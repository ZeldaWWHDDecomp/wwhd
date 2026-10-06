/* Tower of the Gods glowing staircase. */
#include "d/actor/d_a_lstair.h"
static constexpr u32 MATRIX=0x1048D0CC;
static const char* arcname() { return STR(0x10014180); }
static void* lstair_getObjectRes(const char* arc,s32 index,u32 vtable) {
 gabi::Local<SafeString> key;
 key->mStringTop=gabi::ea(arc); key->__vtbl=vtable;
 auto control=dComIfG_resControl();
 // Keep the key above the callee's incoming LR-save/linkage words.
 gabi::Local<be<u32>[4]> linkage;
 return gabi::call<void*>(0x026066C4,control,key.get(),index);
}
static bool switchOn(daLStair_c* a) { s32 room=gabi::load<s8>(gabi::ea(a)+0x2FE); s32 no=a->mSwitchNo; return dComIfGs_isSwitch(no,room)!=0; }
static bool enemyGone(daLStair_c* a) { return gabi::call<void*>(0x025D98E8,gabi::load<s8>(gabi::ea(a)+0x326))==nullptr; }
void daLStair_c::setMoveBGMtx() {
 WWHD_FUNC(0x021B5D5C,void,this);
 f32 y=(current.pos.y-300.0f)+mStairYOffset;
 gabi::call(0x028E93CC,gabi::at<Mtx34>(MATRIX),(f32)current.pos.x,y,(f32)current.pos.z);
 gabi::call(0x025F1C28,gabi::at<Mtx34>(MATRIX),(s16)shape_angle.y);
 gabi::call(0x025F2518,(f32)scale.x,(f32)scale.y,(f32)scale.z);
 gabi::call(0x028E90D4,gabi::at<Mtx34>(MATRIX),&mBgMtx);
}
VERIFY(0x021B5D5C,&daLStair_c::setMoveBGMtx);
void daLStair_c::set_mtx() {
 WWHD_FUNC(0x021B5DE0,void,this);
 J3DModel_setBaseScale(mpModel,&scale);
 gabi::call(0x028E93CC,gabi::at<Mtx34>(MATRIX),(f32)current.pos.x,(f32)(current.pos.y+mStairYOffset),(f32)current.pos.z);
 gabi::call(0x025F1C28,gabi::at<Mtx34>(MATRIX),(s16)current.angle.y);
 J3DModel_setBaseTRMtx(mpModel,gabi::at<Mtx34>(MATRIX));
}
VERIFY(0x021B5DE0,&daLStair_c::set_mtx);
bool daLStair_c::_delete() {
 WWHD_FUNC(0x021B614C,bool,this);
 if(heap!=nullptr) { auto bg=dComIfG_Bgsp(); gabi::call(0x020087EC,bg,mpBgW.get()); }
 dComIfG_resDelete(&mPhase,arcname()); return true;
}
VERIFY(0x021B614C,&daLStair_c::_delete);
void daLStair_c::checkAppear() {
 WWHD_FUNC(0x021B65F8,void,this);
 bool sw=switchOn(this),gone=enemyGone(this);
 if(mSwitchNo!=255) { if(sw!=mSwitchStatus) { if(sw) appear_stair(); else disappear_stair(); } return; }
 if(gone!=mEnemyGone) { if(gone) { mAppearTimer=50; return; } disappear_stair(); }
 s8 t=mAppearTimer;
 if(t>0) mAppearTimer=t-1;
 else if(t==0) { appear_stair(); mAppearTimer=(s8)((u8)mAppearTimer-1); }
}
VERIFY(0x021B65F8,&daLStair_c::checkAppear);
void daLStair_c::moveBG() {
 WWHD_FUNC(0x021B6820,void,this);
 bool sw=switchOn(this),gone=enemyGone(this);
 f32 target=(mSwitchNo!=255?sw:gone)?0.0f:-300.0f;
 if(mAppearTimer<0 && gabi::call<u8>(0x0207A9A0,&mTimer)==0)
  gabi::call<f32>(0x0200ECD4,&mStairYOffset,target,1.0f/60.0f,10.0f,5.0f);
 setMoveBGMtx(); gabi::call(0x024F43DC,mpBgW.get());
}
VERIFY(0x021B6820,&daLStair_c::moveBG);
bool daLStair_c::_execute() {
 WWHD_FUNC(0x021B6930,bool,this);
 bool sw=switchOn(this),gone=enemyGone(this);
 checkAppear(); demoMove(); moveBG();
 mBtkAnm.play(); mBpkAnm0.play(); mBpkAnm1.play(); mBrkAnm.play();
 set_mtx(); mSwitchStatus=sw; mEnemyGone=gone; return true;
}
VERIFY(0x021B6930,&daLStair_c::_execute);
void daLStair_c::demoMove() {
 WWHD_FUNC(0x021B673C,void,this);
 if(gabi::load<u16>(gabi::ea(this)+0xF8)==2) {
  s16 idx=mEventIdx; auto ev=dComIfGp_getPEvtManager();
  if(gabi::call<s32>(0x025440C8,ev,idx)) { u32 play=dComIfGp_ea(); gabi::store<u16>(play+0x52B8,gabi::load<u16>(play+0x52B8)|8); }
 } else if(mEventState==1) {
  gabi::call<s32>(0x025D7A58,this,(s16)mEventIdx,255,65535,0,1);
  u32 a=gabi::ea(this)+0xFA; gabi::store<u16>(a,gabi::load<u16>(a)|2);
 }
 s16 idx=mEventIdx; auto ev=dComIfGp_getPEvtManager();
 if(gabi::call<s32>(0x0254407C,ev,idx)) mEventState=0;
}
VERIFY(0x021B673C,&daLStair_c::demoMove);
void daLStair_c::set_off_se() {
 WWHD_FUNC(0x021B6500,void,this);
 s32 rev=gabi::call<s32>(0x02520540,(s32)gabi::load<s8>(gabi::ea(this)+0x326));
 gabi::call(0x025E1A40,0x6957,gabi::at<cXyz>(gabi::ea(this)+0x37C),0,rev);
}
VERIFY(0x021B6500,&daLStair_c::set_off_se);
void daLStair_c::set_on_se() {
 WWHD_FUNC(0x021B6290,void,this);
 u32 stage=dComIfGp_ea()+0x5150; u32 vt=gabi::load<u32>(stage);
 u32 info=gabi::ea(gabi::call<void*>(gabi::load<u32>(vt+0x15C),gabi::at<void>(stage)));
 if((gabi::load<u8>(info+9)>>1)==5) {
  s32 rev=gabi::call<s32>(0x02520540,(s32)gabi::load<s8>(gabi::ea(this)+0x326));
  gabi::call(0x025E1A40,0x69B3,gabi::at<cXyz>(gabi::ea(this)+0x37C),0,rev);
 }
}
VERIFY(0x021B6290,&daLStair_c::set_on_se);
void daLStair_c::disappear_stair() {
 WWHD_FUNC(0x021B6548,void,this); set_off_se();
 f32 a=(s16)gabi::load<s16>(gabi::ea(this)+0x4F6),b=(s16)gabi::load<s16>(gabi::ea(this)+0x56A),c=(s16)gabi::load<s16>(gabi::ea(this)+0x482);
 mTimer=40;
 mBpkAnm0.mFrameCtrl.mFrame=a; mBpkAnm0.mFrameCtrl.mRate=-1.0f;
 mBpkAnm1.mFrameCtrl.mFrame=b; mBpkAnm1.mFrameCtrl.mRate=-1.0f;
 mBtkAnm.mFrameCtrl.mFrame=c; mBtkAnm.mFrameCtrl.mRate=-1.0f;
 mBrkAnm.mFrameCtrl.mRate=-1.0f;
}
VERIFY(0x021B6548,&daLStair_c::disappear_stair);
void daLStair_c::appear_stair() {
 WWHD_FUNC(0x021B62FC,void,this);
 gabi::Local<cXyz> center,left,right;
 // Non-leaf matrix/play callees save LR at incoming SP+4.
 gabi::Local<be<u32>[4]> linkage;
 center->x=(f32)home.pos.x; center->y=(f32)home.pos.y; center->z=(f32)home.pos.z;
 left->x=-148.0f; left->y=0.0f; left->z=0.0f;
 right->x=148.0f; right->y=0.0f; right->z=0.0f;
 gabi::call(0x028E93CC,gabi::at<Mtx34>(MATRIX),(f32)current.pos.x,(f32)(home.pos.y-300.0f),(f32)current.pos.z);
 gabi::call(0x025F1C28,gabi::at<Mtx34>(MATRIX),(s16)current.angle.y);
 gabi::call(0x028E8F64,gabi::at<Mtx34>(MATRIX),left.get(),left.get());
 gabi::call(0x028E8F64,gabi::at<Mtx34>(MATRIX),right.get(),right.get());
 center->y=(f32)center->y-300.0f;
 auto pa=dComIfGp_getParticle();
 gabi::call<void*>(0x025A847C,pa,0,0x8174,center.get(),&current.angle,0,255,0,-1,0,0,0);
 pa=dComIfGp_getParticle();
 gabi::call<void*>(0x025A847C,pa,0,0x8175,left.get(),&current.angle,0,255,0,-1,0,0,0);
 pa=dComIfGp_getParticle();
 gabi::call<void*>(0x025A847C,pa,0,0x8175,right.get(),&current.angle,0,255,0,-1,0,0,0);
 set_on_se();
 mBpkAnm1.mFrameCtrl.mFrame=0.0f; mEventState=1;
 mBtkAnm.mFrameCtrl.mFrame=0.0f; mBpkAnm0.mFrameCtrl.mFrame=0.0f;
 mBpkAnm0.mFrameCtrl.mRate=1.0f; mBtkAnm.mFrameCtrl.mRate=1.0f;
 mTimer=40; mBpkAnm1.mFrameCtrl.mRate=1.0f; mBrkAnm.mFrameCtrl.mRate=1.0f;
}
VERIFY(0x021B62FC,&daLStair_c::appear_stair);
BOOL daLStair_c::CreateHeap() {
 WWHD_FUNC(0x021B5A34,BOOL,this);
 auto modelData=(J3DModelData*)lstair_getObjectRes(arcname(),11,0x1001407C);
 if(!modelData) gabi::call(0x0273AA24,STR(0x10014100),216,STR(0x10014110));
 mpModel=mDoExt_J3DModel__create(modelData,0x80000,0x11000223);
 if(!mpModel) return 0;
 auto bck=(J3DAnmTransform*)lstair_getObjectRes(arcname(),8,0x1001407C);
 if(!bck) gabi::call(0x0273AA24,STR(0x10014100),233,STR(0x100140D0));
 if(!mBckAnm.init(modelData,bck,true,0,1.0f,0,-1,false)) return 0;
 auto btk=(J3DAnmTextureSRTKey*)lstair_getObjectRes(arcname(),21,0x1001407C);
 if(!btk) gabi::call(0x0273AA24,STR(0x10014100),244,STR(0x100140DC));
 if(!mBtkAnm.init(modelData,btk,true,2,1.0f,0,-1,false,0)) return 0;
 auto color0=lstair_getObjectRes(arcname(),14,0x1001407C);
 if(!color0) gabi::call(0x0273AA24,STR(0x10014100),255,STR(0x100140E8));
 if(!gabi::call<s32>(0x025E74FC,&mBpkAnm0,modelData,color0,1,2,1.0f,0,-1,0,0)) return 0;
 auto color1=lstair_getObjectRes(arcname(),15,0x1001407C);
 if(!color1) gabi::call(0x0273AA24,STR(0x10014100),266,STR(0x100140E8));
 if(!gabi::call<s32>(0x025E74FC,&mBpkAnm1,modelData,color1,1,2,1.0f,0,-1,0,0)) return 0;
 auto tev=lstair_getObjectRes(arcname(),18,0x1001407C);
 if(!tev) gabi::call(0x0273AA24,STR(0x10014100),278,STR(0x100140F4));
 if(!gabi::call<s32>(0x025E8154,&mBrkAnm,modelData,tev,1,2,1.0f,0,-1,0,0)) return 0;
 mpBgW=gabi::call<dBgW*>(0x024F23F4,(dBgW*)nullptr);
 if(!mpBgW) return 0;
 auto bgData=lstair_getObjectRes(arcname(),24,0x1001407C);
 return gabi::call<s32>(0x0200A030,mpBgW.get(),bgData,1,&mBgMtx)==0;
}
VERIFY(0x021B5A34,&daLStair_c::CreateHeap);
void daLStair_c::CreateInit() {
 WWHD_FUNC(0x021B5EC0,void,this);
 gabi::store<u32>(gabi::ea(this)+0x348,mpModel?gabi::ea(mpModel.get())+0xC8:0);
 fopAcM_setCullSizeBox(this,-200.0f,-300.0f,-100.0f,200.0f,100.0f,700.0f);
 s32 no=gabi::load<u8>(gabi::ea(this)+0xB3);
 s32 room=gabi::load<s8>(gabi::ea(this)+0x2FE);
 mSwitchNo=no; cullSizeFar=1.5f;
 mSwitchStatus=dComIfGs_isSwitch(no,room)!=0;
 mEnemyGone=enemyGone(this);
 auto ev=dComIfGp_getPEvtManager();
 s32 idx=gabi::call<s32>(0x02543F10,ev,STR(0x10014148),255);
 mStairYOffset=-330.0f; mAppearTimer=-1; mEventIdx=idx;
 if((mSwitchNo==255?mEnemyGone:mSwitchStatus)!=0) mStairYOffset=0.0f;
 setMoveBGMtx(); auto bg=dComIfG_Bgsp(); gabi::call<s32>(0x024EEA6C,bg,mpBgW.get(),this);
 gabi::call(0x024F43DC,mpBgW.get()); set_mtx();
}
VERIFY(0x021B5EC0,&daLStair_c::CreateInit);
s32 daLStair_c::_create() {
 WWHD_FUNC(0x021B6014,s32,this);
 u32 self=gabi::ea(this),flags=gabi::load<u32>(self+0x2E4);
 if(!(flags&8)) {
  if(self) {
   gabi::call(0x025D4ED0,(fopAc_ac_c*)this);
   gabi::store<u32>(self+0xB4,0x100140BC);
   gabi::call(0x027F2BC0,&mBckAnm,0);
   gabi::store<u32>(self+0x3FC,0x1016E54C);
   gabi::call(0x027DA984,gabi::at<void>(self+0x400));
   gabi::store<u32>(self+0x474,0); gabi::store<u32>(self+0x444,0); gabi::store<u32>(self+0x46C,0);
   gabi::store<u32>(self+0x3FC,0x10014094); gabi::store<u32>(self+0x468,0); gabi::store<u32>(self+0x470,0);
   gabi::store<u32>(self+0x434,0x1016D820);
   gabi::call(0x025E7C6C,&mBtkAnm); gabi::call(0x025E7480,&mBpkAnm0); gabi::call(0x025E7480,&mBpkAnm1); gabi::call(0x025E80D0,&mBrkAnm);
   flags=gabi::load<u32>(self+0x2E4);
  }
  gabi::store<u32>(self+0x2E4,flags|8);
 }
 s32 phase=gabi::call<s32>(0x02520460,&mPhase,arcname());
 if(phase==4) { if(!gabi::call<s32>(0x025D63E8,this,gabi::at<void>(0x021B5D58),0x2960)) return 5; CreateInit(); }
 return phase;
}
VERIFY(0x021B6014,&daLStair_c::_create);
BOOL daLStair_c::_draw() {
 WWHD_FUNC(0x021B61A8,BOOL,this);
 auto light=gabi::call<void*>(0x02555D0C);
 gabi::call(0x025626A4,light,1,&current.pos,gabi::at<void>(gabi::ea(this)+0x110));
 light=gabi::call<void*>(0x02555D0C);
 gabi::call(0x02562F5C,light,mpModel.get(),gabi::at<void>(gabi::ea(this)+0x110));
 auto data=gabi::at<void>(gabi::load<u32>(gabi::ea(mpModel.get())+0xAC));
 gabi::call(0x025E7FC4,&mBtkAnm,data,(f32)mBtkAnm.mFrameCtrl.mFrame);
 data=gabi::at<void>(gabi::load<u32>(gabi::ea(mpModel.get())+0xAC));
 gabi::call(0x025E779C,&mBpkAnm0,data,(f32)mBpkAnm0.mFrameCtrl.mFrame);
 data=gabi::at<void>(gabi::load<u32>(gabi::ea(mpModel.get())+0xAC));
 gabi::call(0x025E779C,&mBpkAnm1,data,(f32)mBpkAnm1.mFrameCtrl.mFrame);
 data=gabi::at<void>(gabi::load<u32>(gabi::ea(mpModel.get())+0xAC));
 gabi::call(0x025E83FC,&mBrkAnm,data,(f32)mBrkAnm.mFrameCtrl.mFrame);
 u32 play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D70));
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D74));
 gabi::call(0x025E2DE0,mpModel.get(),0);
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D78));
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D7C));
 return 1;
}
VERIFY(0x021B61A8,&daLStair_c::_draw);
static BOOL CheckCreateHeap(daLStair_c* a) { WWHD_FUNC(0x021B5D58,BOOL,a); return a->CreateHeap(); }
VERIFY(0x021B5D58,CheckCreateHeap);
static s32 daLStair_Create(daLStair_c* a) { WWHD_FUNC(0x021B6148,s32,a); return a->_create(); }
VERIFY(0x021B6148,daLStair_Create);
static bool daLStair_Delete(daLStair_c* a) { WWHD_FUNC(0x021B61A4,bool,a); return a->_delete(); }
VERIFY(0x021B61A4,daLStair_Delete);
static bool daLStair_Execute(daLStair_c* a) { WWHD_FUNC(0x021B69EC,bool,a); return a->_execute(); }
VERIFY(0x021B69EC,daLStair_Execute);
static BOOL daLStair_IsDelete(daLStair_c* a) { WWHD_FUNC(0x021B6A98,BOOL,a); return 1; }
VERIFY(0x021B6A98,daLStair_IsDelete);
static void LStairSafeString_destructor(void* a,s32 flags) { WWHD_FUNC(0x021B6A84,void,a,flags); if(a && (flags&1)) gabi::call(0x0273AF40,a); }
VERIFY(0x021B6A84,LStairSafeString_destructor);
static void LStair_destructor(daLStair_c* a,s32 flags) {
 WWHD_FUNC(0x021B6AA0,void,a,flags);
 if(a) { gabi::call(0x027F3628,gabi::at<void>(gabi::ea(a)+0x3FC),0); gabi::call(0x025D50BC,a,0); if(flags&1) gabi::call(0x0273AF40,a); }
}
VERIFY(0x021B6AA0,LStair_destructor);
static void LStairSafeString_terminate(void* a) { WWHD_FUNC(0x021B6B00,void,a); }
VERIFY(0x021B6B00,LStairSafeString_terminate);
static void LStair_static_init() {
 WWHD_FUNC(0x021B69F0,void);
 for(u32 off=0;off<16;off+=4) gabi::store<u32>(0x10464E5C+off,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101B90E0));
 gabi::store<f32>(0x10464E50,-3.1415927410125732f); gabi::store<f32>(0x10464E54,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<void>(0x10464E58));
 gabi::call(0x028F026C,gabi::at<void>(0x101B90EC));
 gabi::call(0x028EAB2C,gabi::at<void>(0x10464E59));
 gabi::call(0x028F026C,gabi::at<void>(0x101B90F8));
}
VERIFY(0x021B69F0,LStair_static_init);
