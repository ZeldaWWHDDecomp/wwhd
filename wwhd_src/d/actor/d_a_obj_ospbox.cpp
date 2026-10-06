/* Ported from zeldaret/tww; HD behaviour checked against cking.rpx. */
#include "d/actor/d_a_obj_ospbox.h"
namespace daObjOspbox {
static constexpr u32 MatrixStack=0x1048D0CC, TemporaryMatrix=0x1046BB54;
static u8* ptr(Act_c* a,u32 off) { return gabi::at<u8>(gabi::ea(a)+off); }
static s32 parameter(Act_c* a,s32 width,s32 shift) {
 WWHD_FUNC(0x02380D0C,s32,a,width,shift);
 u32 value=gabi::load<u32>(gabi::ea(a)+0xB0); u32 mask=(width&32)?0:(1u<<(width&31));
 return ((shift&32)?0:(value>>(shift&31))) & (mask-1);
}
VERIFY(0x02380D0C,parameter);
s32 Act_c::Mthd_Create() {
 WWHD_FUNC(0x02380290,s32,this);
 u32 flags=gabi::load<u32>(gabi::ea(this)+0x2E4);
 if(!(flags&8)) {
  if(this) {
   dBgS_MoveBgActor::ct(this); gabi::store<u32>(gabi::ea(this)+0xB4,0x1002E2C4);
   gabi::call(0x0200BD2C,ptr(this,0x3EC)); gabi::call(0x02515DA0,ptr(this,0x408));
   gabi::store<u32>(gabi::ea(this)+0x404,0x1004AE88); gabi::store<u32>(gabi::ea(this)+0x408,0x1004AEC0);
   gabi::call(0x02515FB8,ptr(this,0x428));
   gabi::store<u32>(gabi::ea(this)+0x53C,0x100015A8); gabi::store<u32>(gabi::ea(this)+0x538,0x1002E100);
   gabi::call(0x02018590,ptr(this,0x540));
   gabi::store<u32>(gabi::ea(this)+0x464,0x1004B108); gabi::store<u32>(gabi::ea(this)+0x53C,0x1004B160);
   gabi::store<u32>(gabi::ea(this)+0x554,0x1004B150); gabi::call(0x02008E0C,ptr(this,0x558));
   gabi::store<u32>(gabi::ea(this)+0x558,gabi::ea(this)+0x598);
   gabi::store<u32>(gabi::ea(this)+0x578,0x1002E170); gabi::store<u32>(gabi::ea(this)+0x5A4,0x1002E180);
   gabi::store<u32>(gabi::ea(this)+0x5A8,1); gabi::store<u8>(gabi::ea(this)+0x59C,1);
   for(u32 off : {0x59Eu,0x59Fu,0x5A1u,0x59Du,0x5A2u}) gabi::store<u8>(gabi::ea(this)+off,0);
   gabi::store<u32>(gabi::ea(this)+0x55C,gabi::ea(this)+0x5A4); gabi::store<u32>(gabi::ea(this)+0x598,0x1002E190);
   gabi::store<u8>(gabi::ea(this)+0x5A0,0); flags=gabi::load<u32>(gabi::ea(this)+0x2E4);
   gabi::store<u32>(gabi::ea(this)+0x568,0x1002E160);
  }
  gabi::store<u32>(gabi::ea(this)+0x2E4,flags|8);
 }
 s32 phase=dComIfG_resLoad(&mPhase,STR(0x1002E278));
 if(phase==4) { phase=MoveBGCreate(STR(0x1002E278),7,0,0x8A0);
  if(phase!=4 && phase!=5) JUT_ASSERT_fail(STR(0x1002E1A0),0xC2,STR(0x1002E1B4)); }
 return phase;
}
VERIFY(0x02380290,&Act_c::Mthd_Create);
BOOL Act_c::Mthd_Delete() { WWHD_FUNC(0x02380444,BOOL,this); BOOL result=MoveBGDelete(); gabi::call(0x02520488,STR(0x1002E278)); return result; }
VERIFY(0x02380444,&Act_c::Mthd_Delete);
BOOL Act_c::CreateHeap() {
 WWHD_FUNC(0x02380480,BOOL,this);
 auto* data=(J3DModelData*)dComIfG_getObjectRes(STR(0x1002E278),4,0x1002E0E8);
 if(!data) JUT_ASSERT_fail(STR(0x1002E208),0x86,STR(0x1002E1F8));
 J3DModel* model=mDoExt_J3DModel__create(data,0x80000,0x11000022); mpModel=model; return model!=nullptr;
}
VERIFY(0x02380480,&Act_c::CreateHeap);
void Act_c::set_mtx() {
 WWHD_FUNC(0x0238051C,void,this);
 gabi::call(0x028E93CC,gabi::at<Mtx34>(MatrixStack),(f32)current.pos.x,(f32)current.pos.y,(f32)current.pos.z);
 gabi::call(0x025F1B48,gabi::at<Mtx34>(MatrixStack),(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 J3DModel_setBaseTRMtx(mpModel,gabi::at<Mtx34>(MatrixStack));
 gabi::call(0x028E90D4,gabi::at<Mtx34>(MatrixStack),gabi::at<Mtx34>(TemporaryMatrix));
}
VERIFY(0x0238051C,&Act_c::set_mtx);
void Act_c::init_mtx() { WWHD_FUNC(0x023805F0,void,this); J3DModel* model=mpModel; f32 y=scale.y,x=scale.x,z=scale.z; u32 ea=gabi::ea(model); gabi::store<f32>(ea+0xBC,x); gabi::store<f32>(ea+0xC0,y); gabi::store<f32>(ea+0xC4,z); set_mtx(); }
VERIFY(0x023805F0,&Act_c::init_mtx);
void Act_c::set_ground() {
 WWHD_FUNC(0x02380610,void,this); if(mGroundRetries<=0) return;
 gabi::store<u32>(gabi::ea(this)+0x560,gabi::load<u32>(gabi::ea(this)+4));
 f32 y=current.pos.y,x=current.pos.x,z=current.pos.z; y=y+100.0f;
 gabi::store<f32>(gabi::ea(this)+0x57C,x); gabi::store<f32>(gabi::ea(this)+0x584,z); gabi::store<f32>(gabi::ea(this)+0x580,y);
 u32 play=dComIfGp_ea(); f32 ground=gabi::call<f32>(0x02008974,gabi::at<u8>(play+0x12A0),ptr(this,0x558));
 mGroundY=ground; if(ground>-1000000000.0f) mGroundRetries=0; else mGroundRetries=(s16)mGroundRetries-1;
}
VERIFY(0x02380610,&Act_c::set_ground);
void Act_c::init_ground() { WWHD_FUNC(0x023806CC,void,this); mGroundRetries=5; set_ground(); }
VERIFY(0x023806CC,&Act_c::init_ground);
BOOL Act_c::Create() {
 WWHD_FUNC(0x023806D8,BOOL,this);
 u32 model=gabi::ea((J3DModel*)mpModel); gabi::store<u32>(gabi::ea(this)+0x348,model?model+0xC8:0); init_mtx();
 fopAcM_setCullSizeBox(this,-76.0f,-1.0f,-76.0f,76.0f,151.0f,76.0f);
 mStts.Init(255,255,this); mCyl.Set(gabi::at<dCcD_SrcCyl>(0x1002E280));
 gabi::store<u32>(gabi::ea(this)+0x46C,gabi::ea(this)+0x3EC);
 f32 x=gabi::load<f32>(gabi::ea(this)+0x390), y=current.pos.y+75.0f,z=gabi::load<f32>(gabi::ea(this)+0x398);
 u32 zero[3]={gabi::load<u32>(0x101FFBA8),gabi::load<u32>(0x101FFBAC),gabi::load<u32>(0x101FFBB0)};
 for(u32 i=0;i<3;i++) gabi::store<u32>(gabi::ea(this)+0x4DC+i*4,zero[i]);
 u32 flags=gabi::load<u32>(gabi::ea(this)+0x4BC);
 gabi::store<f32>(gabi::ea(this)+0x394,y); eyePos.y=y; eyePos.z=z; eyePos.x=x;
 gabi::store<u32>(gabi::ea(this)+0x4BC,flags|4); init_ground(); mLockTimer=2; return TRUE;
}
VERIFY(0x023806D8,&Act_c::Create);
void Act_c::make_item() {
 WWHD_FUNC(0x023807DC,void,this); s32 index=parameter(this,3,8); s16 process=gabi::load<s16>(0x1002E238+u32(index)*2);
 if(process==0x54) { s32 item=parameter(this,6,0); s8 room=home.roomNo;
  gabi::call(0x025D8120,&current.pos,item,127,room,0,&current.angle,7,0);
 } else { u32 prm=gabi::load<u32>(0x1002E248+u32(index)*4); s8 room=home.roomNo;
  gabi::call(0x025D5834,process,prm,&current.pos,room,&current.angle,0,-1,0); }
}
VERIFY(0x023807DC,&Act_c::make_item);
void Act_c::eff_break() {
 WWHD_FUNC(0x023808A0,void,this);
 if(!gabi::load<u32>(0x1046BB50)) { gabi::store<f32>(0x1046BB44,2.0f); gabi::store<f32>(0x1046BB4C,1.0f); gabi::store<f32>(0x1046BB48,2.0f); gabi::store<u32>(0x1046BB50,1); }
 gabi::Local<cXyz> pos; f32 y=current.pos.y+75.0f,z=current.pos.z,x=current.pos.x; pos->z=z; pos->x=x; pos->y=y;
 u32 play=dComIfGp_ea(); u32 particles=gabi::load<u32>(play+0x5AB0);
 u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(particles),0,0x3E6,pos.get(),0,0,255,0,-1,ptr(this,0x1A8),ptr(this,0x1A8),gabi::at<cXyz>(0x1046BB44));
 if(emitter) { gabi::store<u16>(emitter+0x60,30); gabi::store<f32>(emitter+0x6C,30.0f); }
}
VERIFY(0x023808A0,&Act_c::eff_break);
void Act_c::sound_break() {
 WWHD_FUNC(0x0238098C,void,this); u32 sound=0;
 if(gabi::load<u16>(gabi::ea(this)+0x56E)<256) { u32 play=dComIfGp_ea(); sound=gabi::call<u32>(0x024EECAC,gabi::at<u8>(play+0x12A0),ptr(this,0x56C)); }
 s8 room=current.roomNo; s32 reverb=gabi::call<s32>(0x02520540,room); mDoAud_seStart(0x69B6,&eyePos,sound,reverb);
}
VERIFY(0x0238098C,&Act_c::sound_break);
BOOL Act_c::Execute(gptr<Mtx34>* output) {
 WWHD_FUNC(0x02380A00,BOOL,this,output);
 if(mCyl.ChkTgHit()) { make_item(); eff_break(); sound_break(); fopAcM_delete(this); }
 else { u8 timer=mLockTimer; if(timer) { timer=timer-1; mLockTimer=timer; if(!timer) { u32 bg=gabi::ea((dBgW*)mpBgW); gabi::store<u8>(bg+0x6C,gabi::load<u8>(bg+0x6C)|0x80); } }
  gabi::call(0x025165A4,&mCyl,&current.pos); u32 play=dComIfGp_ea(); gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),&mCyl);
 }
 set_ground(); set_mtx(); *output=gabi::at<Mtx34>(TemporaryMatrix); return TRUE;
}
VERIFY(0x02380A00,&Act_c::Execute);
BOOL Act_c::Draw() {
 WWHD_FUNC(0x02380AF0,BOOL,this); u32 env=gabi::call<u32>(0x02555D0C);
 gabi::call(0x025626A4,gabi::at<u8>(env),0,&current.pos,&tevStr); env=gabi::call<u32>(0x02555D0C);
 J3DModel* model=mpModel; gabi::call(0x02562F5C,gabi::at<u8>(env),model,&tevStr);
 u32 play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D70));
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D74));
 model=mpModel; gabi::call(0x025E2DE0,model,0);
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D78));
 play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D7C)); return TRUE;
}
VERIFY(0x02380AF0,&Act_c::Draw);
BOOL Act_c::Delete() { WWHD_FUNC(0x02380C68,BOOL,this); return TRUE; }
VERIFY(0x02380C68,&Act_c::Delete);
static void movebg_dtor(Act_c* a,s32 flags) { WWHD_FUNC(0x0238023C,void,a,flags); if(a) { gabi::call(0x025D50BC,a,0); if(flags&1) gabi::call(0x0273AF40,a); } }
VERIFY(0x0238023C,movebg_dtor);
static s32 create(Act_c* a) { WWHD_FUNC(0x02380B88,s32,a); return a->Mthd_Create(); }
VERIFY(0x02380B88,create);
static BOOL remove(Act_c* a) { WWHD_FUNC(0x02380B8C,BOOL,a); return a->Mthd_Delete(); }
VERIFY(0x02380B8C,remove);
static BOOL execute(Act_c* a) { WWHD_FUNC(0x02380B90,BOOL,a); return a->MoveBGExecute(); }
VERIFY(0x02380B90,execute);
static BOOL draw(Act_c* a) { WWHD_FUNC(0x02380B94,BOOL,a); u32 vt=gabi::load<u32>(gabi::ea(a)+0xB4); return gabi::call<BOOL>(gabi::load<u32>(vt+0x2C),a); }
VERIFY(0x02380B94,draw);
static BOOL isDelete(Act_c* a) { WWHD_FUNC(0x02380BA4,BOOL,a); u32 vt=gabi::load<u32>(gabi::ea(a)+0xB4); return gabi::call<BOOL>(gabi::load<u32>(vt+0x3C),a); }
VERIFY(0x02380BA4,isDelete);
static void static_init() {
 WWHD_FUNC(0x02380BB4,void); for(u32 off : {8u,0u,12u,4u}) gabi::store<u32>(0x1046BB34+off,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101CC164));
 gabi::store<f32>(0x1046BB28,-3.1415927410125732f); gabi::store<f32>(0x1046BB2C,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046BB30)); gabi::call(0x028F026C,gabi::at<u8>(0x101CC170));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046BB31)); gabi::call(0x028F026C,gabi::at<u8>(0x101CC17C));
}
VERIFY(0x02380BB4,static_init);
static void empty_dtor(u8* a,s32 flags) { WWHD_FUNC(0x02380C48,void,a,flags); if(a && (flags&1)) gabi::call(0x0273AF40,a); }
VERIFY(0x02380C48,empty_dtor);
}
