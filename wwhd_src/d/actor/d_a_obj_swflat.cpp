// WWHD Tower of the Gods statue floor switch. Binary-derived source: private.
#include "bindings.h"
#include "d/actor/d_a_obj_swflat.h"
using daObjSwflat::Act_c;
static constexpr u32 HIO=0x1046BF4C, MTX=0x1046BF68;
static const char* arc() { return STR(0x1002F928); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static void set_mtx(Act_c* a) {
 WWHD_FUNC(0x02391AC4,void,a);
 auto mtx=gabi::at<Mtx34>(0x1048D0CC);
 f32 x=a->current.pos.x,z=a->current.pos.z,y=a->current.pos.y;
 gabi::call(0x028E93CC,mtx,x,y,z);
 s16 rx=a->shape_angle.x,rz=a->shape_angle.z,ry=a->shape_angle.y;
 gabi::call(0x025F1B48,mtx,rx,ry,rz);
 f32 values[12]; for(u32 i=0;i<12;i++) values[i]=gabi::load<f32>(0x1048D0CC+4*i);
 u32 model=gabi::ea((J3DModel*)a->mpModel);
 for(u32 i=0;i<12;i++) gabi::store<f32>(model+0xC8+4*i,values[i]);
 gabi::call(0x028E90D4,mtx,gabi::at<Mtx34>(MTX));
}
VERIFY(0x02391AC4,set_mtx);
static void init_mtx(Act_c* a) {
 WWHD_FUNC(0x02391B98,void,a); // Matcher calls this PrmAbstract, but it copies the model scale.
 u32 model=gabi::ea((J3DModel*)a->mpModel);
 f32 y=a->scale.y,x=a->scale.x,z=a->scale.z;
 gabi::store<f32>(model+0xBC,x);gabi::store<f32>(model+0xC0,y);gabi::store<f32>(model+0xC4,z);
 set_mtx(a);
}
VERIFY(0x02391B98,init_mtx);
static u32 PrmAbstract(Act_c* a,s32 width,s32 shift) {
 WWHD_FUNC(0x02392864,u32,a,width,shift);
 u32 value=a->mParameters,sh=(u32)shift&63,w=(u32)width&63;
 u32 bits=sh<32?value>>sh:0; return bits&((w<32?1u<<w:0)-1);
}
VERIFY(0x02392864,PrmAbstract);
static BOOL CreateHeap(Act_c* a) {
 WWHD_FUNC(0x023919B8,BOOL,a);
 auto data=dComIfG_getObjectRes(arc(),5,0x1002F84C); if(!data) return FALSE;
 a->mpModel=mDoExt_J3DModel__create((J3DModelData*)data,0,0x11020203);
 auto animation=dComIfG_getObjectRes(arc(),8,0x1002F84C); if(!animation) return FALSE;
 u32 brk=gabi::call<u32>(0x025E80D0,0);a->mpBrk=brk;if(!brk) return FALSE;
 BOOL result=gabi::call<BOOL>(0x025E8154,gabi::at<void>(brk),data,animation,1,0,1.0f,0,-1,0,0);
 return a->mpModel!=nullptr&&result!=0;
}
VERIFY(0x023919B8,CreateHeap);
static s32 Mthd_Create(Act_c* a) {
 WWHD_FUNC(0x023917A0,s32,a);
 u32 condition=a->actor_condition;
 if(!(condition&8)) {
 if(a) {
 dBgS_MoveBgActor::ct(a); a->__vtbl=0x1002F930;
 gabi::call(0x0200BD2C,a->mStts);
 gabi::call(0x02515DA0,gabi::at<u8>(gabi::ea(a)+0x40C));
 gabi::store<u32>(gabi::ea(a)+0x408,0x1004AE88);gabi::store<u32>(gabi::ea(a)+0x40C,0x1004AEC0);
 gabi::call(0x02515FB8,a->mCyl);
 gabi::store<u32>(gabi::ea(a)+0x540,0x100015A8);gabi::store<u32>(gabi::ea(a)+0x53C,0x1002F864);
 gabi::call(0x02018590,gabi::at<u8>(gabi::ea(a)+0x544));
 gabi::store<u32>(gabi::ea(a)+0x468,0x1004B108);gabi::store<u32>(gabi::ea(a)+0x540,0x1004B160);
 condition=a->actor_condition;gabi::store<u32>(gabi::ea(a)+0x558,0x1004B150);
 }
 a->actor_condition=condition|8;
 }
 s32 phase=dComIfG_resLoad(&a->mPhase,arc());
 if(phase==4) {
 phase=gabi::call<s32>(0x024F1D9C,a,arc(),11,0,-1);
 if(phase!=4&&phase!=5) JUT_ASSERT_fail(STR(0x1002F894),0x182,STR(0x1002F8A8));
 }
 return phase;
}
VERIFY(0x023917A0,Mthd_Create);
static void invalidateEmitter(u32 emitter) {
 u32 flags=gabi::load<u32>(emitter+0x254);
 gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);gabi::store<u32>(emitter+0x254,flags|1);
}
static BOOL Mthd_Delete(Act_c* a) {
 WWHD_FUNC(0x023918E4,BOOL,a);
 if(a->heap) {
 u32 inactive=a->mpInactiveEmitter;
 if(inactive) { invalidateEmitter(inactive);a->mpInactiveEmitter=0; }
 u32 active=a->mpActiveEmitter;
 if(active) { invalidateEmitter(active);a->mpActiveEmitter=0; }
 }
 s8 child=gabi::load<s8>(HIO);
 if(child>=0) { s32 count=gabi::load<s32>(HIO+4)-1;gabi::store<s32>(HIO+4,count);
 if(!count) { gabi::call(0x025F0A18,child);gabi::store<s8>(HIO,-1); }
 }
 BOOL result=a->MoveBGDelete(); dComIfG_resDelete(&a->mPhase,arc());return result;
}
VERIFY(0x023918E4,Mthd_Delete);
static BOOL Draw(Act_c* a) {
 WWHD_FUNC(0x02392658,BOOL,a);
 settingTevStruct(dKy_getEnvlight(),1,&a->current.pos,&a->tevStr);
 auto env=dKy_getEnvlight();auto model=(J3DModel*)a->mpModel;setLightTevColorType(env,model,&a->tevStr);
 model=a->mpModel;u32 brk=a->mpBrk,data=gabi::load<u32>(gabi::ea(model)+0xAC);f32 frame=gabi::load<f32>(brk+4);
 gabi::call(0x025E83FC,gabi::at<u8>(brk),gabi::at<u8>(data),frame);
 u32 p=play();gabi::store<u32>(0x104B4634,gabi::load<u32>(p+0x5D70));
 p=play();gabi::store<u32>(0x104B4638,gabi::load<u32>(p+0x5D74));
 model=a->mpModel;gabi::call(0x025E2DE0,model,0);
 p=play();gabi::store<u32>(0x104B4634,gabi::load<u32>(p+0x5D78));
 p=play();gabi::store<u32>(0x104B4638,gabi::load<u32>(p+0x5D7C));return TRUE;
}
VERIFY(0x02392658,Draw);
static s32 CreateWrapper(Act_c* a) { WWHD_FUNC(0x02392704,s32,a);return Mthd_Create(a); }
VERIFY(0x02392704,CreateWrapper);
static BOOL DeleteWrapper(Act_c* a) { WWHD_FUNC(0x02392708,BOOL,a);return Mthd_Delete(a); }
VERIFY(0x02392708,DeleteWrapper);
static BOOL ExecuteWrapper(Act_c* a) { WWHD_FUNC(0x0239270C,BOOL,a);return a->MoveBGExecute(); }
VERIFY(0x0239270C,ExecuteWrapper);
static BOOL DrawWrapper(Act_c* a) { WWHD_FUNC(0x02392710,BOOL,a);return a->Draw_v(); }
VERIFY(0x02392710,DrawWrapper);
static BOOL IsDeleteWrapper(Act_c* a) { WWHD_FUNC(0x02392720,BOOL,a);return a->IsDelete_v(); }
VERIFY(0x02392720,IsDeleteWrapper);
static void sinit() { WWHD_FUNC(0x02392730,void,(u32)0);sinit_header_statics(0x1046BF30,0x101CCDB0);gabi::call(0x02391724,gabi::at<u8>(HIO)); }
VERIFY(0x02392730,sinit);
static void trivialDestructor(void* a,s32 flags) { WWHD_FUNC(0x023927D0,void,a,flags);if(a&&(flags&1)) operator_delete(a); }
VERIFY(0x023927D0,trivialDestructor);
static void emptyVirtual(void* a) { WWHD_FUNC(0x023927E4,void,a); }
VERIFY(0x023927E4,emptyVirtual);
static BOOL Delete(Act_c* a) { WWHD_FUNC(0x023927E8,BOOL,a);return TRUE; }
VERIFY(0x023927E8,Delete);
static BOOL IsDelete(Act_c* a) { WWHD_FUNC(0x023927F0,BOOL,a);return TRUE; }
VERIFY(0x023927F0,IsDelete);
static void destructor(Act_c* a,s32 flags) {
 WWHD_FUNC(0x023927F8,void,a,flags);
 if(a) { gabi::call(0x02515A70,a->mCyl,2);gabi::call(0x02515860,a->mStts,2);gabi::call(0x025D50BC,a,0);if(flags&1) operator_delete(a); }
}
VERIFY(0x023927F8,destructor);
static void* HIOConstructor(void* a) {
 WWHD_FUNC(0x02391724,void*,a);
 if(!a) { a=gabi::call<void*>(0x0273AD10,0x1C);if(!a) return a; }
 u32 p=gabi::ea(a);gabi::store<f32>(p+8,10.0f);gabi::store<u32>(p+4,0);
 gabi::store<s16>(p+0x10,30);gabi::store<u32>(p+0x18,0x1002F874);
 gabi::store<s16>(p+0x12,30);gabi::store<s8>(p,-1);gabi::store<u8>(p+0x15,0);gabi::store<u8>(p+0x14,0);gabi::store<f32>(p+0xC,20.0f);return a;
}
VERIFY(0x02391724,HIOConstructor);
static BOOL isSwitch(Act_c* a,u8 sw) {
 u32 save=gabi::load<u32>(0x101F84DC);s8 room=a->home.roomNo;
 return gabi::call<BOOL>(0x025BA0C0,gabi::at<u8>(save+0x20),sw,room);
}
static void onSwitch(Act_c* a) {
 s8 room=a->home.roomNo;u32 save=gabi::load<u32>(0x101F84DC);u8 sw=a->mSwitch;
 gabi::call(0x025B9E38,gabi::at<u8>(save+0x20),sw,room);
}
static void offSwitch(Act_c* a,u8 sw) {
 u32 save=gabi::load<u32>(0x101F84DC);s8 room=a->home.roomNo;
 gabi::call(0x025B9F7C,gabi::at<u8>(save+0x20),sw,room);
}
static u32 spawnEmitter(Act_c* a,u16 id) {
 u32 p=play();u32 particles=gabi::load<u32>(p+0x5AB0);
 return gabi::call<u32>(0x025A847C,gabi::at<u8>(particles),0,id,&a->mEffectPos,&a->shape_angle,0,255,0,-1,0,0,0);
}
static BOOL Create(Act_c* a) {
 WWHD_FUNC(0x02391BB8,BOOL,a);
 auto model=(J3DModel*)a->mpModel;a->cullMtx=model?gabi::ea(model)+0xC8:0;
 init_mtx(a);gabi::call(0x025D6768,a,0.0f,0.0f,0.0f,90.0f);
 gabi::call(0x02515F14,a->mStts,255,255,a);
 gabi::call(0x02516518,a->mCyl,gabi::at<u8>(0x101CCD6C));
 gabi::store<u32>(gabi::ea(a)+0x470,gabi::ea(a)+0x3F0);
 a->mType=PrmAbstract(a,2,0);a->mSwitch=PrmAbstract(a,8,8);a->mSwitch2=PrmAbstract(a,8,16);
 u32 brk=a->mpBrk;a->mFrame=0;s16 end=gabi::load<s16>(brk+0xA);
 // GHS copies position with integer words and takes the low signed half of fctiwz.
 u32 x=gabi::load<u32>(gabi::ea(a)+0x314),y=gabi::load<u32>(gabi::ea(a)+0x318),z=gabi::load<u32>(gabi::ea(a)+0x31C);
 gabi::store<u32>(gabi::ea(a)+0x564,x);gabi::store<u32>(gabi::ea(a)+0x568,y);gabi::store<u32>(gabi::ea(a)+0x56C,z);
 a->mLocked=0;a->mPressEndFrame=(s16)gabi::ftoi((f32)end-5.0f);
 if(gabi::load<u8>(HIO+0x15)==1) {
 u8 sw=a->mSwitch;if(sw!=255) offSwitch(a,sw);
 sw=a->mSwitch2;if(sw!=255) offSwitch(a,sw);
 }
 u8 type=a->mType,cachedSw=a->mSwitch;BOOL active;
 if(type==2&&isSwitch(a,a->mSwitch2)) { a->mType=0;active=isSwitch(a,a->mSwitch2); }
 else { if(type==2) a->mType=1;active=isSwitch(a,cachedSw); }
 if(active) {
 a->mWasPressed=1;a->mPressed=1;
 if(a->mType!=0) {
 a->mpActiveEmitter=spawnEmitter(a,0x8164);
 a->mInactiveAlpha=0;a->mActiveAlpha=250;a->mEventTimer=0;a->mEventState=0;a->mLockTimer=0;a->mpInactiveEmitter=0;
 } else {
 a->mActiveAlpha=0;a->mLocked=1;a->mpActiveEmitter=0;
 brk=a->mpBrk;gabi::store<f32>(brk+4,(f32)gabi::load<s16>(brk+0xA));
 a->mpInactiveEmitter=0;a->mInactiveAlpha=0;a->mEventState=0;a->mEventTimer=0;a->mLockTimer=0;
 }
 } else {
 a->mWasPressed=0;a->mPressed=0;
 a->mpInactiveEmitter=spawnEmitter(a,0x8163);
 a->mActiveAlpha=0;a->mInactiveAlpha=250;a->mEventTimer=0;a->mEventState=0;a->mLockTimer=0;a->mpActiveEmitter=0;
 }
 if(gabi::load<s8>(HIO)<0) { s32 child=gabi::call<s32>(0x025F0A10,STR(0x1002F904),gabi::at<u8>(HIO));gabi::store<s8>(HIO,child); }
 s32 count=gabi::load<s32>(HIO+4);gabi::store<s32>(HIO+4,count+1);return TRUE;
}
VERIFY(0x02391BB8,Create);
static void setFrame(Act_c* a,s16 frame) { u32 brk=a->mpBrk;a->mFrame=frame;gabi::store<f32>(brk+4,(f32)frame); }
static void setEmitterAlpha(u32 emitter,u8 alpha) { gabi::store<u8>(emitter+0x247,alpha); }
static BOOL Execute(Act_c* a,Mtx34** output) {
 WWHD_FUNC(0x02391F58,BOOL,a,output);
 BOOL statue=FALSE,contact=FALSE,spawned=FALSE;
 gabi::call(0x020182E0,gabi::at<u8>(gabi::ea(a)+0x544),&a->current.pos);
 if(gabi::load<u8>(HIO+0x14)) {
 gabi::call(0x020184DC,gabi::at<u8>(gabi::ea(a)+0x544),gabi::load<f32>(HIO+8));
 gabi::call(0x02018428,gabi::at<u8>(gabi::ea(a)+0x544),gabi::load<f32>(HIO+12));
 }
 u32 p=play();gabi::call(0x0200E240,gabi::at<u8>(p+0x26A4),a->mCyl);
 if(gabi::call<BOOL>(0x02516464,a->mCyl)) {
 u32 actor=gabi::call<u32>(0x02515BBC,gabi::at<u8>(gabi::ea(a)+0x508));
 if(actor&&(gabi::load<u32>(actor+0x2E0)&0x400)) {
 contact=TRUE;
 if(gabi::load<s16>(actor+0xE)==0xA8) statue=TRUE;
 else if(gabi::load<s16>(actor+0xE)==0x1C5) gabi::store<u8>(actor+0x79E,1);
 }
 }
 if(a->mEventTimer==0) {
 if(contact) { a->mPressed=1;gabi::call(0x020184DC,gabi::at<u8>(gabi::ea(a)+0x544),15.0f); }
 else if(a->mType==1) { a->mPressed=0;gabi::call(0x020184DC,gabi::at<u8>(gabi::ea(a)+0x544),10.0f); }
 }
 if(PrmAbstract(a,2,0)==2&&isSwitch(a,a->mSwitch2)) {
 u8 locked=a->mLocked;a->mType=0;
 if(!locked) { a->mLocked=1;a->mLockTimer=gabi::load<s16>(HIO+0x12); }
 }
 if(a->mPressed==1) {
 if(!a->mWasPressed) gabi::call(0x025E19CC,0x6960,&a->current.pos);
 if(!a->mLocked) gabi::call(0x025E19CC,0x7027,&a->current.pos);
 s16 frame=a->mFrame,end=a->mPressEndFrame;if(frame<end) setFrame(a,(s16)(frame+1));
 if(!statue) onSwitch(a);
 if(!a->mLocked) {
 if(!a->mpActiveEmitter) {
 a->mpActiveEmitter=spawnEmitter(a,0x8164);a->mActiveAlpha=250;spawned=TRUE;
 if(PrmAbstract(a,2,0)==0) { a->mLocked=1;a->mLockTimer=gabi::load<s16>(HIO+0x12); }
 } else if(a->mActiveAlpha<250) {
 u8 alpha=(u8)a->mActiveAlpha+25;a->mActiveAlpha=alpha;setEmitterAlpha(a->mpActiveEmitter,alpha);onSwitch(a);
 }
 }
 if(a->mpInactiveEmitter) {
 u8 alpha=(u8)a->mInactiveAlpha-25;a->mInactiveAlpha=alpha;setEmitterAlpha(a->mpInactiveEmitter,alpha);
 if(a->mInactiveAlpha==0) { invalidateEmitter(a->mpInactiveEmitter);a->mpInactiveEmitter=0; }
 }
 } else {
 s16 frame=a->mFrame;if(frame>0) setFrame(a,(s16)(frame-1));
 offSwitch(a,a->mSwitch);
 if(!a->mpInactiveEmitter) { a->mpInactiveEmitter=spawnEmitter(a,0x8163);a->mInactiveAlpha=250; }
 else if(a->mInactiveAlpha<250) { u8 alpha=(u8)a->mInactiveAlpha+25;a->mInactiveAlpha=alpha;setEmitterAlpha(a->mpInactiveEmitter,alpha); }
 if(a->mpActiveEmitter) {
 u8 alpha=(u8)a->mActiveAlpha-25;a->mActiveAlpha=alpha;setEmitterAlpha(a->mpActiveEmitter,alpha);
 if(a->mActiveAlpha==0) { invalidateEmitter(a->mpActiveEmitter);a->mpActiveEmitter=0; }
 }
 }
 switch((u8)a->mEventState) {
 case 0:
 a->mEventTimer=0;
 if(spawned&&statue) { a->mEventState=1;a->mEventTimer=30; } break;
 case 1:
 if(gabi::load<u16>(gabi::ea(a)+0xF8)==2) {
 a->mEventState=2;
 if(gabi::load<u8>(HIO+0x14)) a->mEventTimer=gabi::load<s16>(HIO+0x10);
 } else { gabi::call(0x025D7B24,a,1,0,0);u16 condition=gabi::load<u16>(gabi::ea(a)+0xFA);gabi::store<u16>(gabi::ea(a)+0xFA,condition|2); }
 break;
 case 2: {
 s16 timer=(s16)a->mEventTimer-1;a->mEventTimer=timer;
 if(timer<=1) { p=play();gabi::store<u16>(p+0x52B8,gabi::load<u16>(p+0x52B8)|8);a->mEventState=0;onSwitch(a); } break;
 }
 }
 if(a->mLocked) {
 a->mPressed=1;
 s16 lockTimer=a->mLockTimer;
 if(lockTimer!=0) { a->mLockTimer=lockTimer-1;set_mtx(a);gabi::store<u32>(gabi::ea(output),MTX);a->mWasPressed=a->mPressed;return TRUE; }
 if(a->mpActiveEmitter) {
 u8 alpha=(u8)a->mActiveAlpha-25;a->mActiveAlpha=alpha;setEmitterAlpha(a->mpActiveEmitter,alpha);
 if(a->mActiveAlpha==0) { invalidateEmitter(a->mpActiveEmitter);a->mpActiveEmitter=0; }
 }
 if(a->mpInactiveEmitter) {
 u8 alpha=(u8)a->mInactiveAlpha-25;a->mInactiveAlpha=alpha;setEmitterAlpha(a->mpInactiveEmitter,alpha);
 if(a->mInactiveAlpha==0) { invalidateEmitter(a->mpInactiveEmitter);a->mpInactiveEmitter=0; }
 }
 s16 end=a->mPressEndFrame,frame=a->mFrame;
 if((s32)frame<(s32)end+5) setFrame(a,(s16)(frame+1));
 }
 set_mtx(a);gabi::store<u32>(gabi::ea(output),MTX);a->mWasPressed=a->mPressed;return TRUE;
}
VERIFY(0x02391F58,Execute);
