/* Fire Mountain exterior, WWHD. */
#include "d/actor/d_a_obj_volcano.h"
namespace daObjVolcano {
static void color_to_float(FloatColor* output,u8* input) {
 WWHD_FUNC(0x023B3B94,void,output,input);
 u32 p=gabi::ea(input); f32 r=gabi::load<u8>(p),g=gabi::load<u8>(p+1),b=gabi::load<u8>(p+2),a=gabi::load<u8>(p+3);
 output->r=r/255.0f; output->g=g/255.0f; output->b=b/255.0f; output->a=a/255.0f;
}
VERIFY(0x023B3B94,color_to_float);
static s32 parameter(Act_c* actor,s32 width,s32 shift) {
 WWHD_FUNC(0x023B587C,s32,actor,width,shift);
 u32 value=actor->mParameters,mask=(width&32)?0:(1u<<(width&31));
 return ((shift&32)?0:(value>>(shift&31)))&(mask-1);
}
VERIFY(0x023B587C,parameter);
s32 Act_c::Mthd_Create() {
 WWHD_FUNC(0x023B3C48,s32,this);
 u32 flags=actor_condition;
 if(!(flags&8)) {
  if(gabi::ea(this)) {
   dBgS_MoveBgActor::ct(this); __vtbl=0x100330B0;
   mDoExt_btkAnm::ct(&mBtk); mDoExt_btkAnm::ct(&mFireBtk); gabi::call(0x025E80D0,&mFireBrk);
   dCcD_Stts_ct(&mStts); dCcD_Cyl_ct(&mCyl,0x10032F78); flags=actor_condition;
  }
  actor_condition=flags|8;
 }
 s32 phase=dComIfG_resLoad(&mPhase,STR(0x100330A8));
 if(phase==4) {
  phase=MoveBGCreate(STR(0x100330A8),0x11,0,0xAED0);
  if(phase!=4 && phase!=5) JUT_ASSERT_fail(STR(0x10032F88),0x142,STR(0x10032F9C));
 }
 return phase;
}
VERIFY(0x023B3C48,&Act_c::Mthd_Create);
BOOL Act_c::Mthd_Delete() {
 WWHD_FUNC(0x023B3DA8,BOOL,this);
 BOOL result=MoveBGDelete(); gabi::call(0x025E1B34,&mFirePos); dComIfG_resDelete(&mPhase,STR(0x100330A8)); return result;
}
VERIFY(0x023B3DA8,&Act_c::Mthd_Delete);
BOOL Act_c::CreateHeap() {
 WWHD_FUNC(0x023B3DFC,BOOL,this);
 auto* data=(J3DModelData*)dComIfG_getObjectRes(STR(0x100330A8),6,0x10032F60);
 if(!data) JUT_ASSERT_fail(STR(0x10032FF0),0xBF,STR(0x10032FE0));
 mpModel=mDoExt_J3DModel__create(data,0x80000,0x11000222);
 auto* texture=(J3DAnmTextureSRTKey*)dComIfG_getObjectRes(STR(0x100330A8),13,0x10032F60);
 if(!texture) JUT_ASSERT_fail(STR(0x10032FF0),0xCA,STR(0x10033004));
 BOOL first=mBtk.init(data,texture,true,2,1.0f,0,-1,false,0);
 auto* fireData=(J3DModelData*)dComIfG_getObjectRes(STR(0x100330A8),7,0x10032F60);
 if(!fireData) JUT_ASSERT_fail(STR(0x10032FF0),0xE1,STR(0x10033010));
 mpFireModel=mDoExt_J3DModel__create(fireData,0x80000,0x11000222);
 texture=(J3DAnmTextureSRTKey*)dComIfG_getObjectRes(STR(0x100330A8),14,0x10032F60);
 if(!texture) JUT_ASSERT_fail(STR(0x10032FF0),0xEC,STR(0x10033028));
 BOOL second=mFireBtk.init(fireData,texture,true,2,1.0f,0,-1,false,0);
 u32 colors=gabi::ea(dComIfG_getObjectRes(STR(0x100330A8),10,0x10032F60));
 if(!colors) JUT_ASSERT_fail(STR(0x10032FF0),0xF3,STR(0x10033034));
 BOOL third=gabi::call<BOOL>(0x025E8154,&mFireBrk,fireData,gabi::at<u8>(colors),true,0,1.0f,0,-1,false,0);
 mFireVisible=1; return mpModel && mpFireModel && first && second && third;
}
VERIFY(0x023B3DFC,&Act_c::CreateHeap);
void Act_c::set_mtx() {
 WWHD_FUNC(0x023B4060,void,this);
 auto* stack=gabi::at<Mtx34>(0x1048D0CC);
 gabi::call(0x028E93CC,stack,(f32)current.pos.x,(f32)current.pos.y,(f32)current.pos.z);
 gabi::call(0x025F1B48,stack,(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 J3DModel_setBaseTRMtx(mpModel,stack);
 gabi::call(0x028E90D4,stack,gabi::at<Mtx34>(0x1046C904));
 gabi::call(0x028E93CC,stack,(f32)mFirePos.x,(f32)mFirePos.y,(f32)mFirePos.z);
 gabi::call(0x025F1B48,stack,(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 J3DModel_setBaseTRMtx(mpFireModel,stack);
}
VERIFY(0x023B4060,&Act_c::set_mtx);
void Act_c::init_mtx() {
 WWHD_FUNC(0x023B41C0,void,this);
 u32 model=gabi::ea((J3DModel*)mpModel); f32 y=scale.y,x=scale.x,z=scale.z;
 gabi::store<f32>(model+0xBC,x); gabi::store<f32>(model+0xC0,y); gabi::store<f32>(model+0xC4,z);
 model=gabi::ea((J3DModel*)mpFireModel); x=scale.x; z=scale.z; y=scale.y;
 gabi::store<f32>(model+0xC4,z); gabi::store<f32>(model+0xBC,x); gabi::store<f32>(model+0xC0,y); set_mtx();
}
VERIFY(0x023B41C0,&Act_c::init_mtx);
void Act_c::StopFire() {
 WWHD_FUNC(0x023B41FC,void,this);
 dBgW* bg=mpBgW;
 if(bg && dBgW_ChkUsed(bg)) { u32 play=dComIfGp_ea(); bg=mpBgW; gabi::call(0x020087EC,gabi::at<u8>(play+0x12A0),bg); }
 for(u32 i=0;i<10;i++) {
  u32 emitter=gabi::ea((u8*)mEmitters[i]);
  if(emitter) { u32 flags=gabi::load<u32>(emitter+0x254); gabi::store<u32>(emitter+0x5C,0xFFFFFFFF); gabi::store<u32>(emitter+0x254,flags|1); }
 }
}
VERIFY(0x023B41FC,&Act_c::StopFire);
static u32 particle(Act_c* actor,u32 id,cXyz* pos,cXyz* size) {
 u32 play=dComIfGp_ea(),control=gabi::load<u32>(play+0x5AB0);
 return gabi::call<u32>(0x025A847C,gabi::at<u8>(control),0,id,pos,&actor->current.angle,size,255,0,-1,0,0,0);
}
void Act_c::StartFire() {
 WWHD_FUNC(0x023B4284,void,this);
 mFireVisible=1; gabi::store<f32>(gabi::ea(&mFireBrk),1.0f); gabi::store<f32>(gabi::ea(&mFireBrk)+4,0.0f); mTimer=0;
 dBgW* bg=mpBgW;
 if(bg && !dBgW_ChkUsed(bg)) { u32 play=dComIfGp_ea(); bg=mpBgW; gabi::call(0x024EEA6C,gabi::at<u8>(play+0x12A0),bg,this); }
 mEmitters[0]=gabi::at<u8>(particle(this,0x805A,&mHeadPos,&mParticleScale));
 mEmitters[1]=gabi::at<u8>(particle(this,0x805B,&mMiddlePos,&mParticleScale));
 mEmitters[2]=gabi::at<u8>(particle(this,0x805C,&mFootPos,&mParticleScale));
 for(u32 i=3;i<10;i++) mEmitters[i]=gabi::at<u8>(particle(this,0x8194+i-3,&current.pos,nullptr));
}
VERIFY(0x023B4284,&Act_c::StartFire);
BOOL Act_c::Create() {
 WWHD_FUNC(0x023B456C,BOOL,this);
 u32 model=gabi::ea((J3DModel*)mpModel); cullMtx=model?model+0xC8:0; init_mtx();
 fopAcM_setCullSizeBox(this,-1600.0f,-500.0f,-1800.0f,1700.0f,2500.0f,1900.0f);
 mStts.Init(255,255,this); mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101CDBA4));
 mFirePos.copy(current.pos); gabi::call(0x020182E0,&mCyl.mCyl,&mFirePos);
 mHeight=gabi::load<f32>(gabi::ea(this)+0x6DC);
 f32 y=current.pos.y,headY=y+5000.0f,footY=y+2400.0f,x=current.pos.x;
 mMiddlePos.y=headY; mHeadPos.x=x; mFootPos.x=x;
 mRadius=gabi::load<f32>(gabi::ea(this)+0x6D8);
 f32 z=current.pos.z; mParticleScale.y=2.0f;
 mMiddlePos.z=z; mFootPos.z=z; mHeadPos.z=z; mMiddlePos.x=x;
 mCyl.SetStts(&mStts); mHeadPos.y=headY; mParticleScale.z=2.0f; mParticleScale.x=2.0f; mFootPos.y=footY;
 for(u32 i=0;i<10;i++) mEmitters[i]=nullptr;
 s32 sw=parameter(this,8,8); bool frozen=fopAcM_isSwitch(this,sw)!=0;
 f32 homeY=home.pos.y;
 if(frozen) {
  mFirePos.y=homeY-2000.0f; StopFire(); mOpacity=0.0f;
  u32 save=gabi::load<u32>(0x101F84DC); s16 start=gabi::load<s16>(save+0x115C);
  if(start==2 && current.roomNo==gabi::load<s8>(save+0x1148)) mState=6; else mState=3;
 } else { mFirePos.y=homeY+2000.0f; StartFire(); mState=0; mOpacity=1.0f; }
 u32 play=dComIfGp_ea(); mFreezeEvent=gabi::call<s16>(0x02543F10,gabi::at<u8>(play+0x52C4),STR(0x1003306C),255);
 play=dComIfGp_ea(); mFireEvent=gabi::call<s16>(0x02543F10,gabi::at<u8>(play+0x52C4),STR(0x1003307C),255); return TRUE;
}
VERIFY(0x023B456C,&Act_c::Create);
static void eruption_sound(Act_c* actor) {
 s32 reverb=dComIfGp_getReverb(actor->current.roomNo); mDoAud_seStart(0x61CD,&actor->mFirePos,0,reverb);
 reverb=dComIfGp_getReverb(actor->current.roomNo); mDoAud_seStart(0x1073,&actor->current.pos,0,reverb);
}
static void order(Act_c* actor,s16 event) { gabi::call(0x025D7A58,actor,event,255,65535,0,1); }
static void finish_event(Act_c* actor,s32 state,s16 event) {
 u32 play=dComIfGp_ea(); BOOL done=gabi::call<BOOL>(0x025440C8,gabi::at<u8>(play+0x52C4),event);
 if(done) { play=dComIfGp_ea(); u16 flags=gabi::load<u16>(play+0x52B8); gabi::store<u16>(play+0x52B8,flags|8); actor->mState=state; }
}
void Act_c::fire_main() {
 WWHD_FUNC(0x023B4834,void,this);
 eruption_sound(this);
 if(mCyl.ChkTgHit()) { void* hit=mCyl.GetTgHitObj(); if(hit && cCcD_Obj_ChkAtType(hit,0x80000)) { order(this,(s16)mFreezeEvent); mState=1; } }
 mDoExt_baseAnm_play(&mFireBtk); gabi::call(0x0200F5C8,&mOpacity,1.0f,0.01666666753590107f);
}
VERIFY(0x023B4834,&Act_c::fire_main);
void Act_c::freeze_demo_wait() {
 WWHD_FUNC(0x023B4910,void,this);
 if(gabi::load<u16>(gabi::ea(this)+0xF8)==2) {
  s32 sw=parameter(this,8,8); dComIfGs_onSwitch(sw,home.roomNo); mTimer=0; StopFire();
  for(u32 i=0;i<3;i++) particle(this,0x8191+i,&mFirePos,&scale);
  mState=2;
 } else order(this,(s16)mFreezeEvent);
}
VERIFY(0x023B4910,&Act_c::freeze_demo_wait);
void Act_c::freeze_demo_main() {
 WWHD_FUNC(0x023B4A88,void,this);
 mTimer=u32((s32)mTimer)+1; gabi::call(0x0200F5C8,&mOpacity,0.0f,0.01666666753590107f);
 s32 timer=mTimer;
 if(timer==10) { s32 reverb=dComIfGp_getReverb(current.roomNo); mDoAud_seStart(0x69CE,&mFirePos,0,reverb); }
 else if(timer==150) { s32 reverb=dComIfGp_getReverb(current.roomNo); mDoAud_seStart(0x69CF,&mFirePos,0,reverb); gabi::call(0x025E1988,0x806); mFireVisible=0; }
 else if(timer<=10) mDoExt_baseAnm_play(&mFireBtk);
 mDoExt_baseAnm_play(&mFireBrk); finish_event(this,3,(s16)mFreezeEvent);
}
VERIFY(0x023B4A88,&Act_c::freeze_demo_main);
void Act_c::freeze_main() {
 WWHD_FUNC(0x023B4BDC,void,this);
 s32 sw=parameter(this,8,8);
 if(!fopAcM_isSwitch(this,sw)) { order(this,(s16)mFireEvent); f32 y=home.pos.y; mState=4; mFirePos.y=y-2000.0f; }
}
VERIFY(0x023B4BDC,&Act_c::freeze_main);
void Act_c::fire_demo_wait() {
 WWHD_FUNC(0x023B4C70,void,this);
 if(gabi::load<u16>(gabi::ea(this)+0xF8)==2) { StartFire(); mState=5; } else order(this,(s16)mFireEvent);
}
VERIFY(0x023B4C70,&Act_c::fire_demo_wait);
void Act_c::fire_demo_main() {
 WWHD_FUNC(0x023B4CE0,void,this);
 eruption_sound(this);
 f32 currentY=current.pos.y,fireY=mFirePos.y; s16 event=mFireEvent;
 if(fireY<currentY+2300.0f) mFirePos.y=fireY+100.0f;
 finish_event(this,0,event); mDoExt_baseAnm_play(&mFireBtk); gabi::call(0x0200F5C8,&mOpacity,1.0f,0.01666666753590107f);
}
VERIFY(0x023B4CE0,&Act_c::fire_demo_main);
void Act_c::fail_demo_wait() {
 WWHD_FUNC(0x023B4DD0,void,this);
 s32 sw=parameter(this,8,8); if(!fopAcM_isSwitch(this,sw)) { StartFire(); mState=7; }
}
VERIFY(0x023B4DD0,&Act_c::fail_demo_wait);
void Act_c::fail_demo_main() {
 WWHD_FUNC(0x023B4E38,void,this);
 eruption_sound(this); f32 y=current.pos.y,fireY=mFirePos.y;
 if(fireY<y+2300.0f) mFirePos.y=fireY+100.0f;
 mDoExt_baseAnm_play(&mFireBtk); gabi::call(0x0200F5C8,&mOpacity,1.0f,0.01666666753590107f);
}
VERIFY(0x023B4E38,&Act_c::fail_demo_main);
BOOL Act_c::Execute(gptr<Mtx34>* output) {
 WWHD_FUNC(0x023B4EEC,BOOL,this,output);
 gabi::call(0x020182E0,&mCyl.mCyl,&mFirePos); gabi::call(0x02018428,&mCyl.mCyl,(f32)mHeight); gabi::call(0x020184DC,&mCyl.mCyl,(f32)mRadius);
 u32 play=dComIfGp_ea(); gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),&mCyl);
 switch((u32)mState) {
  case 0:fire_main();break; case 1:freeze_demo_wait();break; case 2:freeze_demo_main();break;
  case 3:freeze_main();break; case 4:fire_demo_wait();break; case 5:fire_demo_main();break;
  case 6:fail_demo_wait();break; case 7:fail_demo_main();break;
 }
 mDoExt_baseAnm_play(&mBtk); u32 env=gabi::call<u32>(0x02555D0C);
 u8 er=gabi::load<u8>(env+0xB7C),eg=gabi::load<u8>(env+0xB7D),eb=gabi::load<u8>(env+0xB7E);
 u8 pr=gabi::load<u8>(env+0xB80),pg=gabi::load<u8>(env+0xB81),pb=gabi::load<u8>(env+0xB82);
 for(u32 i=3;i<10;i++) {
  u32 p=gabi::ea((u8*)mEmitters[i]); if(!p) continue;
  gabi::store<u8>(p+0x248,er); gabi::store<u8>(p+0x249,eg); gabi::store<u8>(p+0x24A,eb);
  p=gabi::ea((u8*)mEmitters[i]); gabi::store<u8>(p+0x245,pg); gabi::store<u8>(p+0x244,pr); gabi::store<u8>(p+0x246,pb);
 }
 set_mtx(); *output=gabi::at<Mtx34>(0x1046C904); return TRUE;
}
VERIFY(0x023B4EEC,&Act_c::Execute);
static void set_material(u8* material,u32 alpha) {
 WWHD_FUNC(0x023B5414,void,material,alpha);
 u32 mat=gabi::ea(material);
 while(mat) {
  u32 shape=gabi::load<u32>(mat+8); gabi::store<u8>(shape+4,alpha?1:0);
  if(alpha) {
   u32 tev=gabi::load<u32>(mat+0x18),vt=gabi::load<u32>(tev+4);
   u32 color=gabi::call<u32>(gabi::load<u32>(vt+0x4C),gabi::at<u8>(tev),3); gabi::store<u8>(color+3,(u8)alpha);
   tev=gabi::load<u32>(mat+0x18); vt=gabi::load<u32>(tev+4);
   color=gabi::call<u32>(gabi::load<u32>(vt+0x4C),gabi::at<u8>(tev),3);
   tev=gabi::load<u32>(mat+0x18); vt=gabi::load<u32>(tev+4);
   gabi::call(gabi::load<u32>(vt+0x3C),gabi::at<u8>(tev),3,gabi::at<u8>(color));
   gabi::Local<FloatColor> input,output;
   color_to_float(input.get(),gabi::at<u8>(color));
   gabi::call(0x0274D458,output.get(),input.get(),1.0f);
   gabi::store<u32>(mat+0xA0,gabi::load<u32>(mat+0xA0)|0x400);
   u32 dest=gabi::call<u32>(0x027F9F0C,gabi::at<u8>(mat+0xA0),10);
   f32 opacity=gabi::load<u8>(color+3),r=output->r,g=output->g,b=output->b;
   gabi::store<f32>(dest,r); gabi::store<f32>(dest+4,g); gabi::store<f32>(dest+8,b); gabi::store<f32>(dest+12,opacity/255.0f);
  }
  mat=gabi::load<u32>(mat+4);
 }
}
VERIFY(0x023B5414,set_material);
BOOL Act_c::Draw() {
 WWHD_FUNC(0x023B55B4,BOOL,this);
 u32 env=gabi::call<u32>(0x02555D0C); gabi::call(0x025626A4,gabi::at<u8>(env),2,&current.pos,&tevStr);
 env=gabi::call<u32>(0x02555D0C); J3DModel* model=mpModel; gabi::call(0x02562F5C,gabi::at<u8>(env),model,&tevStr);
 env=gabi::call<u32>(0x02555D0C); model=mpFireModel; gabi::call(0x02562F5C,gabi::at<u8>(env),model,&tevStr);
 model=mpModel; u32 data=gabi::load<u32>(gabi::ea(model)+0xAC); f32 frame=gabi::load<f32>(gabi::ea(&mBtk)+4); mBtk.entry(gabi::at<J3DModelData>(data),frame);
 u32 play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D70));
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D74));
 u8 alpha=(u8)gabi::ftoi(255.5f*(f32)mOpacity); model=mpModel; data=gabi::load<u32>(gabi::ea(model)+0xAC);
 u32 count=gabi::load<u32>(data+4),joint=gabi::load<u32>(data+8); if(count>2) joint+=0x38;
 set_material(gabi::at<u8>(gabi::load<u32>(joint+0x10)),alpha);
 count=gabi::load<u32>(data+4); joint=gabi::load<u32>(data+8); if(count>1) joint+=0x1C;
 set_material(gabi::at<u8>(gabi::load<u32>(joint+0x10)),alpha);
 model=mpModel; gabi::call(0x025E2DE0,model,0);
 if(mFireVisible) {
  model=mpFireModel; frame=gabi::load<f32>(gabi::ea(&mFireBtk)+4); data=gabi::load<u32>(gabi::ea(model)+0xAC); mFireBtk.entry(gabi::at<J3DModelData>(data),frame);
  model=mpFireModel; frame=gabi::load<f32>(gabi::ea(&mFireBrk)+4); data=gabi::load<u32>(gabi::ea(model)+0xAC); gabi::call(0x025E83FC,&mFireBrk,gabi::at<J3DModelData>(data),frame);
  model=mpFireModel; gabi::call(0x025E2DE0,model,0);
 }
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D78));
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D7C)); return TRUE;
}
VERIFY(0x023B55B4,&Act_c::Draw);
static s32 create(Act_c* actor) { WWHD_FUNC(0x023B5728,s32,actor); return actor->Mthd_Create(); }
VERIFY(0x023B5728,create);
static BOOL remove(Act_c* actor) { WWHD_FUNC(0x023B572C,BOOL,actor); return actor->Mthd_Delete(); }
VERIFY(0x023B572C,remove);
static BOOL execute(Act_c* actor) { WWHD_FUNC(0x023B5730,BOOL,actor); return actor->MoveBGExecute(); }
VERIFY(0x023B5730,execute);
static BOOL draw(Act_c* actor) { WWHD_FUNC(0x023B5734,BOOL,actor); return actor->Draw_v(); }
VERIFY(0x023B5734,draw);
static BOOL is_delete(Act_c* actor) { WWHD_FUNC(0x023B5744,BOOL,actor); return actor->IsDelete_v(); }
VERIFY(0x023B5744,is_delete);
static void static_init() {
 WWHD_FUNC(0x023B5754,void);
 for(u32 off : {8u,0u,12u,4u}) gabi::store<u32>(0x1046C8F4+off,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101CDBE8));
 gabi::store<f32>(0x1046C8E8,-3.1415927410125732f); gabi::store<f32>(0x1046C8EC,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046C8F0)); gabi::call(0x028F026C,gabi::at<u8>(0x101CDBF4));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046C8F1)); gabi::call(0x028F026C,gabi::at<u8>(0x101CDC00));
}
VERIFY(0x023B5754,static_init);
static void empty_destructor(u8* object,s32 flags) { WWHD_FUNC(0x023B57E8,void,object,flags); if(object&&(flags&1)) gabi::call(0x0273AF40,object); }
VERIFY(0x023B57E8,empty_destructor);
static BOOL movebg_is_delete(Act_c* actor) { WWHD_FUNC(0x023B57FC,BOOL,actor); return TRUE; }
VERIFY(0x023B57FC,movebg_is_delete);
static void empty_inline(Act_c* actor) { WWHD_FUNC(0x023B5804,void,actor); }
VERIFY(0x023B5804,empty_inline);
BOOL Act_c::Delete() { WWHD_FUNC(0x023B5808,BOOL,this); return TRUE; }
VERIFY(0x023B5808,&Act_c::Delete);
static void actor_destructor(Act_c* actor,s32 flags) {
 WWHD_FUNC(0x023B5810,void,actor,flags);
 if(actor) { dCcD_Cyl_dt(&actor->mCyl,2); dCcD_Stts_dt(&actor->mStts,2); gabi::call(0x025D50BC,actor,0); if(flags&1) gabi::call(0x0273AF40,actor); }
}
VERIFY(0x023B5810,actor_destructor);
}
