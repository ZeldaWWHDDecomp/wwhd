/* WWHD hammer switch; */
#include "d/actor/d_a_obj_swhammer.h"
using daObjSwhammer::Act_c;
#define ARC STR(0x1002FAC8)
#define TMP_MTX gabi::at<Mtx34>(0x1046BFD8)
#define DAMAGE 0x101CCE48
#define DAMAGE_DIR 0x101CCE4C
static u32 PrmAbstract(fopAc_ac_c* a,s32 width,s32 shift) {
 WWHD_FUNC(0x02393CB4,u32,a,width,shift);
 u32 w=(u32)width&63,s=(u32)shift&63;
 return (s<32?(u32)a->mParameters>>s:0)&((w<32?1u<<w:0)-1);
}
VERIFY(0x02393CB4,PrmAbstract);
static bool is_switch(Act_c* a) { s32 sw=PrmAbstract(a,8,8); return fopAcM_isSwitch(a,sw)!=0; }
static void change_switch(Act_c* a,u32 target,s32 shift) {
 s32 sw=PrmAbstract(a,8,shift); u32 save=gabi::load<u32>(0x101F84DC); s32 room=a->home.roomNo;
 gabi::call(target,gabi::at<void>(save+0x20),sw,room);
}
void Act_c::mode_upper_init() {
 WWHD_FUNC(0x02392F08,void,this);
 gabi::call(0x02018428,&mCylTg.mCyl,112.5f); gabi::call(0x02018428,&mCylCo.mCyl,112.5f);
 u32 flags=gabi::load<u32>(gabi::ea(this)+0x39C); mMode=0; gabi::store<u32>(gabi::ea(this)+0x39C,flags|1); mTargetHFrac=1; mChanging=0;
}
VERIFY(0x02392F08,&Act_c::mode_upper_init);
void Act_c::mode_lower_init() {
 WWHD_FUNC(0x02392E94,void,this);
 gabi::call(0x02018428,&mCylTg.mCyl,gabi::load<f32>(0x1046BFA0)); gabi::call(0x02018428,&mCylCo.mCyl,gabi::load<f32>(0x1046BFA0));
 u32 flags=gabi::load<u32>(gabi::ea(this)+0x39C); mMode=2; mTargetHFrac=0; gabi::store<u32>(gabi::ea(this)+0x39C,flags&~1u); mChanging=0;
}
VERIFY(0x02392E94,&Act_c::mode_lower_init);
void Act_c::mode_u_l_init() {
 WWHD_FUNC(0x02393858,void,this); fopAcM_seStart(this,0x69B2,0); mMode=1; mTargetHFrac=0;
}
VERIFY(0x02393858,&Act_c::mode_u_l_init);
void Act_c::mode_l_u_init() { WWHD_FUNC(0x02393A58,void,this); mMode=3; mTargetHFrac=1; }
VERIFY(0x02393A58,&Act_c::mode_l_u_init);
void Act_c::mode_upper() {
 WWHD_FUNC(0x023938B4,void,this);
 if(gabi::load<s32>(DAMAGE)==1) { mChanging=1; mode_u_l_init(); }
 else if(is_switch(this))mode_u_l_init();
 else if(gabi::load<s32>(DAMAGE)==2) { vib_start(gabi::load<s16>(DAMAGE_DIR),1); fopAcM_seStart(this,0x69B8,0); }
}
VERIFY(0x023938B4,&Act_c::mode_upper);
void Act_c::mode_lower() {
 WWHD_FUNC(0x02393A70,void,this);
 if(!is_switch(this)) { fopAcM_seStart(this,0x69B7,0); mode_l_u_init(); }
}
VERIFY(0x02393A70,&Act_c::mode_lower);
void Act_c::mode_u_l() {
 WWHD_FUNC(0x023939AC,void,this);
 if(!(mCurHFrac>0)) { change_switch(this,0x025B9E38,8); if(mChanging)change_switch(this,0x025B9F7C,0); mode_lower_init(); }
}
VERIFY(0x023939AC,&Act_c::mode_u_l);
void Act_c::mode_l_u() { WWHD_FUNC(0x02393AF0,void,this); if(!(mCurHFrac<1))mode_upper_init(); }
VERIFY(0x02393AF0,&Act_c::mode_l_u);
void Act_c::vib_start(s16 dir,f32 mag) {
 WWHD_FUNC(0x02393824,void,this,dir,mag); mAngleSpeedZ=cM_scos(dir)*mag; mAngleSpeedX=cM_ssin(dir)*mag;
}
VERIFY(0x02393824,&Act_c::vib_start);
void Act_c::vib_proc() {
 WWHD_FUNC(0x023934E4,void,this);
 f32 z=mAngleZ,x=mAngleX,vz=mAngleSpeedZ,vx=mAngleSpeedX;
 f32 az=gabi::fnmsubs(vz,0.18f,-(z*0.99f)),ax=gabi::fnmsubs(vx,0.18f,-(x*0.99f));
 vz+=az; vx+=ax; mAngleSpeedZ=vz; mAngleSpeedX=vx; mAngleZ=z+vz; mAngleX=x+vx;
}
VERIFY(0x023934E4,&Act_c::vib_proc);
void Act_c::crush_start() { WWHD_FUNC(0x023932A0,void,this); mCrushState=1; }
VERIFY(0x023932A0,&Act_c::crush_start);
void Act_c::crush_proc() {
 WWHD_FUNC(0x02393540,void,this);
 if(mCrushState==1) { mCrushState=2; mScaleY=0.2f; mCrushTimer=18; return; }
 if(mCrushState==2) { s16 timer=(s16)((s16)mCrushTimer-1); mCrushTimer=timer; if(timer<=0) {mCrushState=0;mScaleYSpeed=0;} return; }
 f32 scale=mScaleY, velocity=mScaleYSpeed; f32 accel=gabi::fnmsubs(velocity,0.45f,-((scale-1.0f)*0.8f));
 velocity+=accel; mScaleYSpeed=velocity; mScaleY=scale+velocity;
}
VERIFY(0x02393540,&Act_c::crush_proc);
void Act_c::calc_top_pos() {
 WWHD_FUNC(0x02393488,void,this);
 f32 frac=mCurHFrac,diff=frac-(f32)mTargetHFrac;
 f32 velocity=gabi::fnmsubs(diff,0.8f,mVSpeed); velocity=gabi::fnmsubs(velocity,0.55f,velocity);
 frac+=velocity; frac=(frac-1.0f)>=0?1.0f:frac; mVSpeed=velocity; mCurHFrac=frac; mTopPos=(1-frac)*-60.0f;
}
VERIFY(0x02393488,&Act_c::calc_top_pos);
s32 Act_c::Draw() {
 WWHD_FUNC(0x02393798,s32,this);
 settingTevStruct(dKy_getEnvlight(),0,&current.pos,&tevStr); setLightTevColorType(dKy_getEnvlight(),mpModel,&tevStr); mDoExt_modelUpdateDL(mpModel); return TRUE;
}
VERIFY(0x02393798,&Act_c::Draw);
s32 Act_c::Delete() {
 WWHD_FUNC(0x023937F4,s32,this);
 u32 callback=gabi::ea(&mSmokeCb),vt=gabi::load<u32>(callback); gabi::call(gabi::load<u32>(vt+0x44),gabi::at<void>(callback)); return TRUE;
}
VERIFY(0x023937F4,&Act_c::Delete);
static s32 method_Create(Act_c* a) { WWHD_FUNC(0x02393B08,s32,a); return a->_create(); }
VERIFY(0x02393B08,method_Create);
static s32 method_Delete(Act_c* a) { WWHD_FUNC(0x02393B0C,s32,a); return gabi::call<s32>(0x02392A70,a); }
VERIFY(0x02393B0C,method_Delete);
static s32 method_Execute(Act_c* a) { WWHD_FUNC(0x02393B10,s32,a); return a->MoveBGExecute(); }
VERIFY(0x02393B10,method_Execute);
static s32 method_Draw(Act_c* a) { WWHD_FUNC(0x02393B14,s32,a); return a->Draw_v(); }
VERIFY(0x02393B14,method_Draw);
static s32 method_IsDelete(Act_c* a) { WWHD_FUNC(0x02393B24,s32,a); return a->IsDelete_v(); }
VERIFY(0x02393B24,method_IsDelete);
Act_c* Act_c::ct(Act_c* a) {
 WWHD_FUNC(0x02392880,Act_c*,a);
 if(!a) { a=(Act_c*)operator_new(0x718); if(!a)return a; }
 dBgS_MoveBgActor::ct(a); a->__vtbl=0x1002FAD0;
 dCcD_Cyl_ct(&a->mCylCo,0x1002F9BC); dCcD_Stts_ct(&a->mSttsCo);
 dCcD_Cyl_ct(&a->mCylTg,0x1002F9BC); dCcD_Stts_ct(&a->mSttsTg);
 gabi::call(0x025A5B18,&a->mSmokeCb,1);
 u32 b=gabi::ea(a); gabi::store<u8>(b+0x70B,160); gabi::store<u8>(b+0x70A,160); gabi::store<u8>(b+0x70D,255); gabi::store<u8>(b+0x70C,128);
 return a;
}
VERIFY(0x02392880,Act_c::ct);
s32 Act_c::_create() {
 WWHD_FUNC(0x023929AC,s32,this);
 if(!((u32)actor_condition&8)) { if(gabi::ea(this)!=0)ct(this); actor_condition=(u32)actor_condition|8; }
 s32 phase=dComIfG_resLoad(&mPhs,ARC);
 if(phase==4) { phase=MoveBGCreate(ARC,7,0,0xA80); if(phase!=4&&phase!=5)JUT_ASSERT_fail(STR(0x1002F9CC),0x1E8,STR(0x1002F9E4)); }
 return phase;
}
VERIFY(0x023929AC,&Act_c::_create);
bool Act_c::_delete() { WWHD_FUNC(0x02392A70,bool,this); s32 result=MoveBGDelete(); dComIfG_resDelete(&mPhs,ARC); return result!=0; }
VERIFY(0x02392A70,&Act_c::_delete);
s32 Act_c::CreateHeap() {
 WWHD_FUNC(0x02392CB0,s32,this);
 auto* data=(J3DModelData*)dComIfG_getObjectRes(ARC,4,0x1002F9A4);
 if(!data)JUT_ASSERT_fail(STR(0x1002FA34),0x18B,STR(0x1002FA4C));
 J3DModel* model=mDoExt_J3DModel__create(data,0x80000,0x11000022); mpModel=model;
 if(model) { u32 b=gabi::ea(data),n=gabi::load<u32>(b+4),joint=gabi::load<u32>(b+8); if(n>1)joint+=0x1C; gabi::store<u32>(joint+8,0x02392AC0); gabi::store<u32>(gabi::ea((J3DModel*)mpModel)+0xB8,gabi::ea(this)); model=mpModel; }
 return model!=nullptr;
}
VERIFY(0x02392CB0,&Act_c::CreateHeap);
void Act_c::set_mtx() {
 WWHD_FUNC(0x02392D80,void,this);
 mDoMtx_stack_c::transS(current.pos.x,current.pos.y,current.pos.z); mDoMtx_ZXYrotM(mDoMtx_stack_c::get(),shape_angle.x,shape_angle.y,shape_angle.z); J3DModel_setBaseTRMtx(mpModel,mDoMtx_stack_c::get());
}
VERIFY(0x02392D80,&Act_c::set_mtx);
void Act_c::init_mtx() {
 WWHD_FUNC(0x02392E44,void,this);
 f32 x=scale.x,y=scale.y; J3DModel* model=mpModel; f32 z=scale.z;
 gabi::store<f32>(gabi::ea(model)+0xBC,x); gabi::store<f32>(gabi::ea(model)+0xC0,y); gabi::store<f32>(gabi::ea(model)+0xC4,z);
 set_mtx(); PSMTXCopy(mDoMtx_stack_c::get(),gabi::at<Mtx34>(0x1046BFD8));
}
VERIFY(0x02392E44,&Act_c::init_mtx);
s32 Act_c::Create() {
 WWHD_FUNC(0x02392F8C,s32,this);
 cullMtx=gabi::ea(J3DModel_getBaseTRMtx(mpModel)); init_mtx();
 gabi::call(0x025D6768,this,0.0f,30.0f,0.0f,120.0f);
 mSttsCo.Init(255,255,this); mCylCo.Set(gabi::at<dCcD_SrcCyl>(0x101CCEA0)); mCylCo.SetStts(&mSttsCo);
 mSttsTg.Init(255,255,this); mCylTg.Set(gabi::at<dCcD_SrcCyl>(0x101CCEE4));
 mScaleYSpeed=0; mVSpeed=0; mCrushTimer=0; u32 flags=gabi::load<u32>(gabi::ea(this)+0x5EC);
 mAngleSpeedZ=0; mCylTg.SetStts(&mSttsTg); mTargetHFrac=0; mScaleY=1; mAngleX=0; mAngleZ=0;
 gabi::store<u32>(gabi::ea(this)+0x5EC,flags|4); mAngleSpeedX=0; mCrushState=0;
 if(is_switch(this)) { mCurHFrac=0; mTopPos=-60; mode_lower_init(); }
 else { mCurHFrac=1; mTopPos=0; mode_upper_init(); }
 f32 y=(f32)current.pos.y+(f32)mTopPos+120;
 gabi::store<u8>(gabi::ea(this)+0x388,45); gabi::store<f32>(gabi::ea(this)+0x394,y); eyePos.y=y; return TRUE;
}
VERIFY(0x02392F8C,&Act_c::Create);
static void sinit() {
 WWHD_FUNC(0x02393B34,void,(u32)0);
 for(u32 i=0;i<4;i++)gabi::store<u32>(0x1046BFA8+i*4,0);
 __register_global_object(0x101CCE24); gabi::store<f32>(0x1046BF98,-3.1415927f); gabi::store<f32>(0x1046BF9C,3.1415927f);
 gabi::call(0x028ED6F8,(u32)0x1046BFA4); __register_global_object(0x101CCE30); gabi::call(0x028EAB2C,(u32)0x1046BFA5); __register_global_object(0x101CCE3C);
 gabi::store<f32>(0x101CCEA0+0x3C,100); gabi::store<f32>(0x1046BFA0,52.5f);
 gabi::store<f32>(0x101CCEE4+0x3C,50); gabi::store<f32>(0x101CCEE4+0x40,112.5f); gabi::store<f32>(0x101CCEA0+0x40,112.5f);
}
VERIFY(0x02393B34,sinit);
static void trivial_dt(void* a,s32 flags) { WWHD_FUNC(0x02393C10,void,a,flags); if(a&&(flags&1))operator_delete(a); }
VERIFY(0x02393C10,trivial_dt);
void Act_c::set_damage() {
 WWHD_FUNC(0x02393178,void,this);
 u32 play=dComIfGp_ea(),player=gabi::load<u32>(play+0x5B2C); u8 cut=gabi::load<u8>(player+0x3AC);
 gabi::store<u32>(DAMAGE,0); gabi::store<u16>(DAMAGE_DIR,0);
 if(gabi::call<s32>(0x025162A4,&mCylTg)) {
 void* hit=gabi::call<void*>(0x02516300,&mCylTg);
 void* actor=gabi::call<void*>(0x02515BBC,gabi::at<void>(gabi::ea(this)+0x5EC));
 if((gabi::load<u32>(gabi::ea(hit)+0x10)&0x10000) && actor && gabi::load<s16>(gabi::ea(actor)+0xE)==168) {
 if(cut==18||cut==19)gabi::store<u32>(DAMAGE,1);
 else if(cut==17) { f32 x=gabi::load<f32>(gabi::ea(this)+0x618),z=gabi::load<f32>(gabi::ea(this)+0x620); s16 dir=gabi::call<s16>(0x020195B0,x,z); gabi::store<s16>(DAMAGE_DIR,dir); gabi::store<u32>(DAMAGE,2); }
 }
 gabi::call(0x023129C4,&eyePos,(s32)current.roomNo,&mCylTg,17);
 gabi::call(0x02312E54,this,&mCylTg); gabi::call(0x0251621C,&mCylTg);
 }
 gabi::call(0x02515E50,gabi::at<void>(gabi::ea(this)+0x6A4));
}
VERIFY(0x02393178,&Act_c::set_damage);
static void member_dispatch(Act_c* a,u32 entry) {
 u32 base=gabi::ea(a)+(s16)gabi::load<s16>(entry); s16 index=gabi::load<s16>(entry+2); u32 target;
 if(index<0)target=gabi::load<u32>(entry+4);
 else {s16 offset=gabi::load<s16>(entry+6); u32 vt=gabi::load<u32>(base+offset);target=gabi::load<u32>(vt+(u32)index*8+4);}
 gabi::call(target,gabi::at<void>(base));
}
s32 Act_c::Execute(Mtx34** matrix) {
 WWHD_FUNC(0x023935E4,s32,this,matrix);
 set_damage(); if(gabi::load<s32>(DAMAGE)==1) {crush_start();eff_crush();}
 member_dispatch(this,0x1002FA90+(u32)mMode*8);
 calc_top_pos();vib_proc();crush_proc();set_mtx();
 gabi::store<u32>(gabi::ea(matrix),0x1046BFD8);
 gabi::call(0x020182E0,&mCylTg.mCyl,&current.pos); gabi::call(0x020182E0,&mCylCo.mCyl,&current.pos);
 u32 play=dComIfGp_ea();gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),&mCylTg);
 play=dComIfGp_ea();gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),&mCylCo);
 f32 y=(f32)current.pos.y+(f32)mTopPos+120; eyePos.y=y;gabi::store<f32>(gabi::ea(this)+0x394,y);return TRUE;
}
VERIFY(0x023935E4,&Act_c::Execute);
struct Quaternion_l {be<f32> x,y,z,w;};
static BOOL jnodeCB(void* node,s32 timing) {
 WWHD_FUNC(0x02392AC0,BOOL,node,timing);
 if(timing==0) {
 u32 model=gabi::load<u32>(0x104B462C),actor=gabi::load<u32>(model+0xB8);
 void* joint=gabi::call<void*>(0x027F7878,node); u16 index=gabi::load<u16>(gabi::ea(joint)+4);
 u32 block=gabi::load<u32>(model+0x2C),matrices=gabi::load<u32>(block+0x10);gabi::store<u16>(block+4,gabi::load<u16>(block+4)|16);
 PSMTXCopy(gabi::at<Mtx34>(matrices+index*0x30),mDoMtx_stack_c::get());
 f32 scale=gabi::load<f32>(actor+0x6D0);
 gabi::call(0x025F24E0,0.0f,20.0f*(1.0f-scale),0.0f);
 scale=gabi::load<f32>(actor+0x6D0);mDoMtx_stack_c::scaleM(1,scale,1);
 gabi::call(0x025F24E0,0.0f,gabi::load<f32>(actor+0x6F0),0.0f);
 gabi::Local<cXyz> axis; f32 x=gabi::load<f32>(actor+0x6D8),z=gabi::load<f32>(actor+0x6D4);axis->y=1;axis->x=x;axis->z=z;
 gabi::Local<Quaternion_l> quat;gabi::call(0x02312548,(Quaternion_l*)quat,(cXyz*)axis);
 gabi::call(0x025F24E0,0.0f,-20.0f,0.0f);gabi::call(0x025F25CC,(Quaternion_l*)quat);gabi::call(0x025F24E0,0.0f,20.0f,0.0f);
 block=gabi::load<u32>(model+0x2C);matrices=gabi::load<u32>(block+0x10);gabi::store<u16>(block+4,gabi::load<u16>(block+4)|16);
 mtx_copy(gabi::at<Mtx34>(matrices+index*0x30),mDoMtx_stack_c::get());PSMTXCopy(mDoMtx_stack_c::get(),gabi::at<Mtx34>(0x104B4868));
 }
 return TRUE;
}
VERIFY(0x02392AC0,jnodeCB);
void Act_c::eff_crush() {
 WWHD_FUNC(0x023932AC,void,this);
 if(mMode==0) { dComIfGp_particle_set(0x81B7,&current.pos);dComIfGp_particle_set(0x81B8,&current.pos); }
 if(!gabi::load<u32>(0x1046BFD0)) {
 gabi::store<f32>(0x1046BFB8,1.5f);gabi::store<f32>(0x1046BFC0,1);gabi::store<f32>(0x1046BFBC,1.5f);gabi::store<u32>(0x1046BFD0,1);
 }
 u32 play=dComIfGp_ea(),particles=gabi::load<u32>(play+0x5AB0);
 void* emitter=gabi::call<void*>(0x025A847C,gabi::at<void>(particles),2,0x2027,&current.pos,(u32)0,(u32)0,200,&mSmokeCb,-1,(u32)0,(u32)0,gabi::at<cXyz>(0x1046BFB8));
 if(emitter) {
 u32 b=gabi::ea(emitter);gabi::store<u32>(b+0x5C,1);gabi::store<u16>(b+0x60,45);gabi::store<f32>(b+0x34,30);gabi::store<f32>(b+0x68,0);gabi::store<f32>(b+0x6C,38);
 if(!gabi::load<u32>(0x1046BFD4)) {
 gabi::store<u32>(0x1046BFD4,1);gabi::store<f32>(0x1046BFCC,0);gabi::store<f32>(0x1046BFC8,15);gabi::store<f32>(0x1046BFC4,0);
 }
 gabi::store<f32>(b+0x14,gabi::load<f32>(0x1046BFC4));gabi::store<f32>(b+0x18,gabi::load<f32>(0x1046BFC8));gabi::store<f32>(b+0x1C,gabi::load<f32>(0x1046BFCC));
 }
}
VERIFY(0x023932AC,&Act_c::eff_crush);
static void actor_dt(Act_c* a,s32 flags) {
 WWHD_FUNC(0x02393C30,void,a,flags);
 if(a) {gabi::call(0x02515860,&a->mSttsTg,2);gabi::call(0x02515A70,&a->mCylTg,2);gabi::call(0x02515860,&a->mSttsCo,2);gabi::call(0x02515A70,&a->mCylCo,2);gabi::call(0x025D50BC,a,0);if(flags&1)operator_delete(a);}
}
VERIFY(0x02393C30,actor_dt);
static BOOL base_IsDelete(void* a) {WWHD_FUNC(0x02393C24,BOOL,a);return TRUE;}
VERIFY(0x02393C24,base_IsDelete);
static void base_empty(void* a) {WWHD_FUNC(0x02393C2C,void,a);}
VERIFY(0x02393C2C,base_empty);
