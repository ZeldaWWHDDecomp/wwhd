/* Warp light shafts: audited WWHD translation unit. */
#include "d/actor/d_a_warpls.h"
namespace {
struct SafeName { be<u32> text,vt; };
static u32 archive(u32 type) { return gabi::load<u32>(0x101D3224+type*4); }
static void sound(daWarpls_c* actor,u32 id) {
 s32 room=(s8)actor->current.roomNo;
 s32 reverb=gabi::call<s32>(0x02520540,room);
 gabi::call(0x025E1A40,id,gabi::at<u8>(gabi::ea(actor)+0x37C),0,reverb);
}
static u32 switch_on(daWarpls_c* actor) {
 u32 save=gabi::load<u32>(0x101F84DC); s32 room=(s8)actor->home.roomNo;
 s32 sw=actor->mSwitchNo;
 return gabi::call<u32>(0x025BA0C0,gabi::at<u8>(save+0x20),sw,room);
}
static void emitter_flag(u32 emitter,u32 bit,bool on) {
 u32 flags=gabi::load<u32>(emitter+0x254);
 gabi::store<u32>(emitter+0x254,on?(flags|bit):(flags&~bit));
}
static void matrix(daWarpls_c* actor,bool initialization) {
 f32 y=actor->scale.y,z=actor->scale.z,x=actor->scale.x;
 u32 model=gabi::ea((J3DModel*)actor->mpModel);
 if(initialization) {gabi::store<f32>(model+0xC0,y);gabi::store<f32>(model+0xC4,z);gabi::store<f32>(model+0xBC,x);}
 else {gabi::store<f32>(model+0xBC,x);gabi::store<f32>(model+0xC4,z);gabi::store<f32>(model+0xC0,y);}
 f32 px=actor->current.pos.x,py=actor->current.pos.y,pz=actor->current.pos.z;
 auto* stack=gabi::at<Mtx34>(0x1048D0CC);
 gabi::call(0x028E93CC,stack,px,py,pz);
 J3DModel_setBaseTRMtx(actor->mpModel,stack);
}
static s32 heap_callback(daWarpls_c* actor) { WWHD_FUNC(0x024DBDAC,s32,actor); return actor->CreateHeap(); }
VERIFY(0x024DBDAC,heap_callback);
}
s32 daWarpls_c::CreateHeap() {
 WWHD_FUNC(0x024DBB34,s32,this);
 gabi::Local<SafeName> name;
 name->vt=0x100427BC; u32 type=mWarpType;
 name->text=archive(type);
 s32 idx=gabi::load<s16>(0x100428E0+type*2);
 u32 resource=gabi::load<u32>(0x101F4F28);
 auto* data=gabi::call<J3DModelData*>(0x026066C4,gabi::at<u8>(resource),name.get(),idx);
 if(!data) JUT_ASSERT_fail(STR(0x10042828),0xE9,STR(0x10042838));
 mpModel=mDoExt_J3DModel__create(data,0,0x11020203);
 if(!(J3DModel*)mpModel) return 0;
 type=mWarpType; mpBrkAnm=0;
 idx=gabi::load<s16>(0x100428E4+type*2);
 if(idx!=-1) {
  gabi::Local<SafeName> brk; brk->vt=0x100427BC; brk->text=archive(type);
  resource=gabi::load<u32>(0x101F4F28);
  auto* anim=gabi::call<u8*>(0x026066C4,gabi::at<u8>(resource),brk.get(),idx);
  if(!anim) JUT_ASSERT_fail(STR(0x10042828),0xF8,STR(0x10042810));
  u32 object=gabi::call<u32>(0x025E80D0,0); mpBrkAnm=object;
  if(!object) return 0;
  if(!gabi::call<s32>(0x025E8154,gabi::at<u8>(object),data,anim,1,0,1.0f,0,-1,0,0)) return 0;
 }
 type=mWarpType; mpBckAnm=0;
 idx=gabi::load<s16>(0x100428E8+type*2);
 if(idx!=-1) {
  gabi::Local<SafeName> bck; bck->vt=0x100427BC; bck->text=archive(type);
  resource=gabi::load<u32>(0x101F4F28);
  auto* anim=gabi::call<u8*>(0x026066C4,gabi::at<u8>(resource),bck.get(),idx);
  if(!anim) JUT_ASSERT_fail(STR(0x10042828),0x10B,STR(0x1004281C));
  u32 object=gabi::call<u32>(0x0273AD10,0x8C);
  if(object) {
   gabi::call(0x027F2BC0,gabi::at<u8>(object),0);
   gabi::store<u32>(object+0x10,0x1016E54C);
   gabi::call(0x027DA984,gabi::at<u8>(object+0x14));
   gabi::store<u32>(object+0x80,0);gabi::store<u32>(object+0x58,0);
   gabi::store<u32>(object+0x48,0x1016D820);gabi::store<u32>(object+0x84,0);
   gabi::store<u32>(object+0x10,0x100427D4);gabi::store<u32>(object+0x7C,0);gabi::store<u32>(object+0x88,0);
  }
  mpBckAnm=object;
  if(!object) return 0;
  if(!gabi::call<s32>(0x025E8508,gabi::at<u8>(object),data,anim,1,0,1.0f,0,-1,0)) return 0;
 }
 return 1;
}
VERIFY(0x024DBB34,&daWarpls_c::CreateHeap);
s32 daWarpls_c::distance() {
 WWHD_FUNC(0x024DBDB0,s32,this);
 u32 play=gabi::call<u32>(0x025200D4); u32 player=gabi::load<u32>(play+0x5B34);
 play=gabi::call<u32>(0x025200D4);
 if(player!=gabi::load<u32>(play+0x5B2C)) return 0;
 gabi::Local<cXyz> delta,flat;
 gabi::call(0x0201ADE0,gabi::at<cXyz>(player+0x314),delta.get(),&current.pos);
 flat->x=delta->x; flat->y=0.0f; flat->z=delta->z;
 f32 mag=gabi::call<f32>(0x028E8DD0,flat.get());
 f32 length=gabi::call<f32>(0x028F4384,mag);
 f32 limit=112.5f*(f32)scale.x;
 return length<limit;
}
VERIFY(0x024DBDB0,&daWarpls_c::distance);
s32 daWarpls_c::link() {
 WWHD_FUNC(0x024DC660,s32,this);
 u32 play=gabi::call<u32>(0x025200D4); u32 player=gabi::load<u32>(play+0x5B34);
 play=gabi::call<u32>(0x025200D4);
 if(player!=gabi::load<u32>(play+0x5B2C) || !mWarpActive || mPlayerStartedInWarp) return 0;
 gabi::Local<cXyz> delta,flat;
 gabi::call(0x0201ADE0,gabi::at<cXyz>(player+0x314),delta.get(),&current.pos);
 flat->x=delta->x; flat->y=0.0f; flat->z=delta->z;
 f32 mag=gabi::call<f32>(0x028E8DD0,flat.get());
 f32 length=gabi::call<f32>(0x028F4384,mag);
 f32 limit=112.5f*(f32)scale.x;
 return length<limit;
}
VERIFY(0x024DC660,&daWarpls_c::link);
void daWarpls_c::CreateInit() {
 WWHD_FUNC(0x024DBE68,void,this);
 gabi::Local<be<u32>[3]> events;
 (*events)[0]=0x10042874;(*events)[1]=0x10042884;(*events)[2]=0x10042894;
 gabi::Local<SafeName> siren,stage;
 siren->text=0x1004286C; siren->vt=0x100427BC;
 u32 play=gabi::call<u32>(0x025200D4);
 stage->text=play+0x5134;stage->vt=0x100427BC;
 auto invoke=[](SafeName* str) {u32 vt=str->vt;u32 fn=gabi::load<u32>(vt+0x14);gabi::call_ptr(fn,str);};
 invoke(siren.get());invoke(siren.get());u32 left=siren->text;
 invoke(stage.get());u32 right=stage->text;
 bool equal=left==right;
 if(!equal) {
  equal=true;
  for(u32 n=0;n<0x40001;n++) {u8 a=gabi::load<u8>(left+n),b=gabi::load<u8>(right+n);if(a!=b){equal=false;break;}if(!a)break;}
 }
 if(equal) {s32 room=(s8)current.roomNo;if(room==7)mEvtType=0;else if(room==17)mEvtType=1;}
 else mEvtType=2;
 u32 prm=mParameters;mSwitchNo=prm&255;mSceneNo=(prm>>8)&255;
 u32 type=mWarpType;
 if(type==0 || type==1) {
  if(type==0){scale.z=2.0f;scale.x=2.0f;}
  play=gabi::call<u32>(0x025200D4);u32 particles=gabi::load<u32>(play+0x5AB0);
  u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(particles),0,type==0?0x810B:0x8225,&current.pos,&current.angle,0,255,0,-1,0,0,0);
  mpEmitter=emitter;
  if(type==0 && emitter) {
   emitter_flag(emitter,0x40,true);emitter=mpEmitter;
   f32 y=scale.y,x=scale.x,z=scale.z;
   gabi::store<f32>(emitter+0x220,x);gabi::store<f32>(emitter+0x224,y);gabi::store<f32>(emitter+0x228,z);
  }
 }
 u32 sw=switch_on(this);
 if(sw || mSwitchNo==255) {
  u32 emitter=mpEmitter;if(emitter)emitter_flag(emitter,1,false);
  u32 brk=mpBrkAnm;if(brk)gabi::store<f32>(brk+4,(f32)gabi::load<s16>(brk+0xA));
  u32 bck=mpBckAnm;if(bck)gabi::store<f32>(bck+4,(f32)gabi::load<s16>(bck+0xA));
  mPrevSwitchState=1;mWarpActive=1;mSkipActivationEvt=1;mStartupDelayTimer=0;
 } else {
  u32 emitter=mpEmitter;if(emitter)emitter_flag(emitter,1,true);
  mWarpActive=0;mStartupDelayTimer=10;
 }
 prm=mParameters;u32 event=(prm>>16)&255;mEvtState=0;
 play=gabi::call<u32>(0x025200D4);
 mActivationEvtIdx=gabi::call<s32>(0x02543F10,gabi::at<u8>(play+0x52C4),gabi::at<char>(0),event);
 u32 name=(*events)[(u32)mEvtType];
 play=gabi::call<u32>(0x025200D4);
 mWarpingEvtIdx=gabi::call<s32>(0x02543F10,gabi::at<u8>(play+0x52C4),gabi::at<char>(name),255);
 u32 model=gabi::ea((J3DModel*)mpModel);cullMtx=model?model+0xC8:0;
 gabi::call(0x025D674C,this,-250.0f,0.0f,-250.0f,250.0f,2600.0f,250.0f);
 cullSizeFar=1.0f;matrix(this,true);
 if(distance())mPlayerStartedInWarp=1;
}
VERIFY(0x024DBE68,&daWarpls_c::CreateInit);
s32 daWarpls_c::create() {
 WWHD_FUNC(0x024DC3B8,s32,this);
 u32 condition=actor_condition;
 if(!(condition&8)) {
  if(this) {gabi::call(0x025D4ED0,this);condition=actor_condition;__vtbl=0x100427FC;}
  actor_condition=condition|8;
 }
 u32 type=(u32)mParameters>>28;mWarpType=type;
 if(type>=2){JUT_ASSERT_fail(STR(0x100428BC),0x18B,STR(0x100428A4));type=mWarpType;}
 u32 name=archive(type);
 s32 phase=gabi::call<s32>(0x02520460,&mPhs,gabi::at<char>(name));
 if(phase==4){type=mWarpType;u32 heap=gabi::load<u32>(0x100428D8+type*4);if(!gabi::call<s32>(0x025D63E8,this,0x024DBDAC,heap))return 5;CreateInit();}
 return phase;
}
VERIFY(0x024DC3B8,&daWarpls_c::create);
bool daWarpls_c::remove() {
 WWHD_FUNC(0x024DC4C0,bool,this);
 u32 emitter=mpEmitter;
 if(emitter){u32 flags=gabi::load<u32>(emitter+0x254);gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);gabi::store<u32>(emitter+0x254,flags|1);mpEmitter=0;}
 u32 name=archive(mWarpType);gabi::call(0x025204C8,&mPhs,gabi::at<char>(name));return true;
}
VERIFY(0x024DC4C0,&daWarpls_c::remove);
void daWarpls_c::effect() {
 WWHD_FUNC(0x024DC5C4,void,this);
 if(mWarpActive)return;
 u32 brk=mpBrkAnm;if(brk)gabi::store<f32>(brk,1.0f);
 u32 bck=mpBckAnm;if(bck)gabi::store<f32>(bck,1.0f);
 u32 emitter=mpEmitter;if(emitter)emitter_flag(emitter,1,false);
 sound(this,0x6967);mWarpActive=1;
}
VERIFY(0x024DC5C4,&daWarpls_c::effect);
bool daWarpls_c::demo() {
 WWHD_FUNC(0x024DC730,bool,this);
 u8 sw=switch_on(this);
 if(mPlayerStartedInWarp){if(!distance())mPlayerStartedInWarp=0;return true;}
 if(sw) {
  if(mEvtState==0 && !mSkipActivationEvt && !mWarpActive && mActivationEvtIdx!=-1)mEvtState=1;
 } else if(mSwitchNo!=255) {
  u32 brk=mpBrkAnm;if(brk)gabi::store<f32>(brk,-1.0f);
  u32 bck=mpBckAnm;if(bck)gabi::store<f32>(bck,-1.0f);
  u32 emitter=mpEmitter;if(emitter)emitter_flag(emitter,1,true);
  mWarpActive=0;return true;
 }
 if(link() && mEvtState==0)mEvtState=2;
 return true;
}
VERIFY(0x024DC730,&daWarpls_c::demo);
void daWarpls_c::checkOrder() {
 WWHD_FUNC(0x024DC880,void,this);
 u32 self=gabi::ea(this);
 if(gabi::load<u16>(self+0xF8)!=2){demo();return;}
 s32 event=mActivationEvtIdx;u32 play=gabi::call<u32>(0x025200D4);
 if(gabi::call<s32>(0x0254407C,gabi::at<u8>(play+0x52C4),event) && mEvtState==1){mEvtState=0;effect();}
 event=mActivationEvtIdx;play=gabi::call<u32>(0x025200D4);
 if(gabi::call<s32>(0x025440C8,gabi::at<u8>(play+0x52C4),event)){play=gabi::call<u32>(0x025200D4);u16 flags=gabi::load<u16>(play+0x52B8);gabi::store<u16>(play+0x52B8,flags|8);}
 event=mWarpingEvtIdx;play=gabi::call<u32>(0x025200D4);
 if(gabi::call<s32>(0x0254407C,gabi::at<u8>(play+0x52C4),event) && mEvtState==2){mEvtState=0;play=gabi::call<u32>(0x025200D4);gabi::call(0x02543714,gabi::at<u8>(play+0x52C4),&current.pos);}
 event=mWarpingEvtIdx;play=gabi::call<u32>(0x025200D4);
 if(gabi::call<s32>(0x025440C8,gabi::at<u8>(play+0x52C4),event)){s32 room=(s8)current.roomNo;u32 scene=gabi::load<u8>(self+0x3CB);gabi::call(0x02587EFC,scene,room);sound(this,0x285D);}
}
VERIFY(0x024DC880,&daWarpls_c::checkOrder);
void daWarpls_c::eventOrder() {
 WWHD_FUNC(0x024DC9D4,void,this);
 u8 sw=switch_on(this);s32 state=mEvtState;u32 self=gabi::ea(this);
 if(state==1){u32 ev=((u32)mParameters>>16)&255;s32 idx=mActivationEvtIdx;gabi::call(0x025D7A58,this,idx,ev,65535,0,1);}
 else if(state==2){
  u32 type=mEvtType;
  if(type==0){if(!distance())mEvtState=0;else{ s32 idx=mWarpingEvtIdx;gabi::call(0x025D7A58,this,idx,255,65535,0,5);}}
  else {s32 idx=mWarpingEvtIdx;gabi::call(0x025D7A58,this,idx,255,65535,0,1);}
 } else {if(mActivationEvtIdx==-1 && (mSwitchNo==255 || sw))effect();return;}
 u16 condition=gabi::load<u16>(self+0xFA);gabi::store<u16>(self+0xFA,condition|2);
}
VERIFY(0x024DC9D4,&daWarpls_c::eventOrder);
bool daWarpls_c::status() {WWHD_FUNC(0x024DCB70,bool,this);if(mWarpActive)sound(this,0x7025);return true;}
VERIFY(0x024DCB70,&daWarpls_c::status);
bool daWarpls_c::execute() {
 WWHD_FUNC(0x024DCBC8,bool,this);
 u8 sw=switch_on(this);s32 timer=mStartupDelayTimer;if(timer>0)mStartupDelayTimer=timer-1;
 checkOrder();eventOrder();status();
 u32 brk=mpBrkAnm;if(brk)gabi::call(0x025E742C,gabi::at<u8>(brk));
 u32 bck=mpBckAnm;if(bck)gabi::call(0x025E742C,gabi::at<u8>(bck));
 matrix(this,false);mPrevSwitchState=sw;return true;
}
VERIFY(0x024DCBC8,&daWarpls_c::execute);
static bool draw(daWarpls_c* actor) {
 WWHD_FUNC(0x024DC524,bool,actor);
 if(!actor->mWarpActive)return true;
 u32 env=gabi::call<u32>(0x02555D0C);gabi::call(0x025626A4,gabi::at<u8>(env),0,&actor->current.pos,&actor->tevStr);
 env=gabi::call<u32>(0x02555D0C);J3DModel* model=actor->mpModel;gabi::call(0x02562F5C,gabi::at<u8>(env),model,&actor->tevStr);
 u32 brk=actor->mpBrkAnm;
 if(brk){model=actor->mpModel;f32 frame=gabi::load<f32>(brk+4);u32 data=gabi::load<u32>(gabi::ea(model)+0xAC);gabi::call(0x025E83FC,gabi::at<u8>(brk),gabi::at<u8>(data),frame);}
 u32 bck=actor->mpBckAnm;
 if(bck){model=actor->mpModel;f32 frame=gabi::load<f32>(bck+4);u32 data=gabi::load<u32>(gabi::ea(model)+0xAC);gabi::call(0x025E86B8,gabi::at<u8>(bck),gabi::at<u8>(data),frame);}
 model=actor->mpModel;gabi::call(0x025E2DE0,model,0);return true;
}
VERIFY(0x024DC524,draw);
static s32 create_wrapper(daWarpls_c* actor){WWHD_FUNC(0x024DC4BC,s32,actor);return actor->create();}
VERIFY(0x024DC4BC,create_wrapper);
static bool delete_wrapper(daWarpls_c* actor){WWHD_FUNC(0x024DC520,bool,actor);return actor->remove();}
VERIFY(0x024DC520,delete_wrapper);
static bool execute_wrapper(daWarpls_c* actor){WWHD_FUNC(0x024DCD10,bool,actor);return actor->execute();}
VERIFY(0x024DCD10,execute_wrapper);
static bool is_delete(void* actor){WWHD_FUNC(0x024DCDBC,bool,actor);return true;}
VERIFY(0x024DCDBC,is_delete);
static void initialize_globals(){
 WWHD_FUNC(0x024DCD14,void);
 gabi::store<u32>(0x1046EBA4,0);gabi::store<u32>(0x1046EB9C,0);gabi::store<u32>(0x1046EBA8,0);gabi::store<u32>(0x1046EBA0,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101D3200));
 gabi::store<f32>(0x1046EB90,-3.1415927410125732f);gabi::store<f32>(0x1046EB94,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046EB98));gabi::call(0x028F026C,gabi::at<u8>(0x101D320C));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046EB99));gabi::call(0x028F026C,gabi::at<u8>(0x101D3218));
}
VERIFY(0x024DCD14,initialize_globals);
static void name_destructor(void* object,u32 flags){WWHD_FUNC(0x024DCDA8,void,object,flags);if(object && (flags&1))gabi::call(0x0273AF40,object);}
VERIFY(0x024DCDA8,name_destructor);
static void actor_destructor(daWarpls_c* actor,u32 flags){WWHD_FUNC(0x024DCDC4,void,actor,flags);if(actor){gabi::call(0x025D50BC,actor,0);if(flags&1)gabi::call(0x0273AF40,actor);}}
VERIFY(0x024DCDC4,actor_destructor);
static void name_assure(SafeName* object){WWHD_FUNC(0x024DCE18,void,object);}
VERIFY(0x024DCE18,name_assure);
