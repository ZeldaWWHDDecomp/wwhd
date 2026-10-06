/* Big Octo/Jabun whirlpool, WWHD implementation. */
#include "d/actor/d_a_obj_auzu.h"
namespace daObjAuzu {
static f32 attribute(const Act_c* actor,u32 offset) { return gabi::load<f32>(0x100252E0+u32((s32)actor->mType)*16+offset); }
static s32 parameter(Act_c* actor,s32 width,s32 shift) {
 WWHD_FUNC(0x0231A62C,s32,actor,width,shift);
 u32 value=actor->mParameters,mask=(width&32)?0:(1u<<(width&31));
 return ((shift&32)?0:(value>>(shift&31)))&(mask-1);
}
VERIFY(0x0231A62C,parameter);
static s32 parameter_copy(Act_c* actor,s32 width,s32 shift) {
 WWHD_FUNC(0x023197A0,s32,actor,width,shift);
 u32 value=actor->mParameters,mask=(width&32)?0:(1u<<(width&31));
 return ((shift&32)?0:(value>>(shift&31)))&(mask-1);
}
VERIFY(0x023197A0,parameter_copy);
static void color_to_float(FloatColor* output,u8* input) {
 WWHD_FUNC(0x023197BC,void,output,input);
 u32 p=gabi::ea(input); f32 r=gabi::load<u8>(p),g=gabi::load<u8>(p+1),b=gabi::load<u8>(p+2),a=gabi::load<u8>(p+3);
 f32 rf=r/255.0f,gf=g/255.0f,bf=b/255.0f,af=a/255.0f;
 output->r=rf; output->g=gf; output->b=bf; output->a=af;
}
VERIFY(0x023197BC,color_to_float);
u32 Act_c::is_exist() {
 WWHD_FUNC(0x02319870,u32,this);
 if(mType!=0) return true;
 u32 save=gabi::load<u32>(0x101F84DC);
 return gabi::call<u32>(0x025B8B94,gabi::at<u8>(save+0x644),0xA02)!=0;
}
VERIFY(0x02319870,&Act_c::is_exist);
bool Act_c::create_heap() {
 WWHD_FUNC(0x023198C8,bool,this);
 auto* modelData=(J3DModelData*)dComIfG_getObjectRes(STR(0x100252D8),4,0x10025248);
 if(!modelData) JUT_ASSERT_fail(STR(0x10025290),0xE2,STR(0x10025270));
 mpModel=mDoExt_J3DModel__create(modelData,0x80000,0x11000222);
 auto* animation=(J3DAnmTextureSRTKey*)dComIfG_getObjectRes(STR(0x100252D8),7,0x10025248);
 if(!animation) JUT_ASSERT_fail(STR(0x10025290),0xEC,STR(0x10025280));
 f32 speed=attribute(this,12);
 BOOL result=mBtk.init(modelData,animation,true,2,speed,0,-1,false,0);
 J3DModel* model=mpModel;
 if(!model || !result) return false;
 u32 ea=gabi::ea(model); u32 flags=gabi::load<u32>(ea+0x74)&~1u; gabi::store<u32>(ea+0x74,flags);
 gabi::call(0x027F596C,model,flags); return true;
}
VERIFY(0x023198C8,&Act_c::create_heap);
static bool heap_callback(Act_c* actor) { WWHD_FUNC(0x023199F4,bool,actor); return actor->create_heap(); }
VERIFY(0x023199F4,heap_callback);
void Act_c::set_mtx() {
 WWHD_FUNC(0x023199F8,void,this);
 auto* stack=gabi::at<Mtx34>(0x1048D0CC);
 gabi::call(0x028E93CC,stack,(f32)current.pos.x,(f32)current.pos.y,(f32)current.pos.z);
 gabi::call(0x025F1B48,stack,(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 J3DModel_setBaseTRMtx(mpModel,stack);
}
VERIFY(0x023199F8,&Act_c::set_mtx);
void Act_c::init_mtx() {
 WWHD_FUNC(0x02319ABC,void,this);
 f32 y=scale.y,x=scale.x,mult=attribute(this,0); u32 model=gabi::ea((J3DModel*)mpModel); x=x*mult;
 f32 z=scale.z; gabi::store<f32>(model+0xC0,y); z=z*mult;
 gabi::store<f32>(model+0xBC,x); gabi::store<f32>(model+0xC4,z); set_mtx();
}
VERIFY(0x02319ABC,&Act_c::init_mtx);
void Act_c::set_state_map() {
 WWHD_FUNC(0x02319AF8,void,this);
 if(parameter(this,1,16)==1) return;
 f32 factor=mScaleFactor; u32 status=actor_status;
 actor_status=factor>0.9990000128746033f?status|0x20:status&~0x20u;
}
VERIFY(0x02319AF8,&Act_c::set_state_map);
s32 Act_c::_create() {
 WWHD_FUNC(0x02319B70,s32,this);
 s32 type=parameter(this,4,20); u32 flags=actor_condition; mType=type;
 if(!(flags&8)) {
  if(gabi::ea(this)) { gabi::call(0x025D4ED0,this); __vtbl=0x10025260; mDoExt_btkAnm::ct(&mBtk); flags=actor_condition; }
  actor_condition=flags|8;
 }
 u32 exists=is_exist(); mExists=(u8)exists; if(!exists) return 5;
 s32 phase=dComIfG_resLoad(&mPhase,STR(0x100252D8)); if(phase!=4) return phase;
 if(!gabi::call<BOOL>(0x025D63E8,this,0x023199F4,0xD80)) return 5;
 u32 model=gabi::ea((J3DModel*)mpModel); cullMtx=model?model+0xC8:0; init_mtx();
 f32 extent=3000.0f*attribute(this,0);
 fopAcM_setCullSizeBox(this,-extent,-30.0f,-extent,extent,30.0f,extent);
 mToAppear=0; bool hidden=parameter(this,1,16)==1;
 if(!hidden) { s32 sw=parameter(this,8,0); hidden=fopAcM_isSwitch(this,sw)!=0; }
 mBgmStarted=0; mTagId=0xFFFFFFFF; mScaleFactor=hidden?0.0f:1.0f;
 set_state_map(); return phase;
}
VERIFY(0x02319B70,&Act_c::_create);
bool Act_c::_delete() { WWHD_FUNC(0x02319D48,bool,this); if(mExists) dComIfG_resDelete(&mPhase,STR(0x100252D8)); return true; }
VERIFY(0x02319D48,&Act_c::_delete);
void Act_c::bgm_start() {
 WWHD_FUNC(0x02319D84,void,this);
 if(mType==0 && !mBgmStarted) { mBgmStarted=1; gabi::call(0x025E1918,0x8000002Bu); }
}
VERIFY(0x02319D84,&Act_c::bgm_start);
void Act_c::ship_whirl() {
 WWHD_FUNC(0x02319DB4,void,this);
 u32 play=dComIfGp_ea(); u32 ship=gabi::load<u32>(play+0x5B3C);
 if(!ship || gabi::load<s16>(ship+8)!=0xA5) return;
 f32 distance=gabi::call<f32>(0x025D69AC,this,gabi::at<fopAc_ac_c>(ship));
 u32 attrs=0x100252E0+u32((s32)mType)*16; f32 radius=gabi::load<f32>(0x101C7D0C);
 f32 outer=radius*gabi::load<f32>(attrs+4); f32 inner=radius*gabi::load<f32>(attrs+8);
 outer=outer*outer; inner=inner*inner;
 if(!(distance<outer)) return;
 if(mScaleFactor>0.009999999776482582f) {
  bgm_start(); u32 id=gabi::load<u32>(gabi::ea(this)+4); s32 link=parameter(this,8,8);
  if(distance<inner) { gabi::store<u32>(ship+0x708,id); gabi::store<u16>(ship+0x69A,(u16)link); gabi::store<u8>(ship+0x63B,1); }
  else { gabi::store<u16>(ship+0x69A,(u16)link); gabi::store<u32>(ship+0x708,id); gabi::store<u8>(ship+0x63B,0); }
 } else { gabi::store<u32>(ship+0x70C,0); gabi::store<u32>(ship+0x708,0xFFFFFFFF); }
}
VERIFY(0x02319DB4,&Act_c::ship_whirl);
bool Act_c::_execute() {
 WWHD_FUNC(0x02319F4C,bool,this);
 set_mtx(); u32 play=dComIfGp_ea();
 if(gabi::load<u32>(play+0x5CD8)&0x10000) ship_whirl();
 else if(mScaleFactor>0.009999999776482582f) {
  play=dComIfGp_ea(); u32 player=gabi::load<u32>(play+0x5B34);
  gabi::call<f32>(0x025D69AC,this,gabi::at<fopAc_ac_c>(player));
  f32 distance=gabi::call<f32>(0x025D69AC,this,gabi::at<fopAc_ac_c>(player));
  f32 radius=gabi::load<f32>(0x101C7D0C)*attribute(this,4); radius=radius*radius;
  if(distance<radius) gabi::store<u32>(player+0x6A8C,gabi::load<u32>(gabi::ea(this)+4));
 }
 mDoExt_baseAnm_play(&mBtk);
 f32 target=0.0f;
 if(parameter(this,1,16)==1) { if(mToAppear) target=1.0f; }
 else { s32 sw=parameter(this,8,0); if(!fopAcM_isSwitch(this,sw)) target=1.0f; }
 gabi::call(0x0200F5C8,&mScaleFactor,target,0.019999999552965164f);
 s8 soundParam=(s8)gabi::ftoi((f32)mScaleFactor*100.0f); s32 reverb=dComIfGp_getReverb(current.roomNo);
 mDoAud_seStart(0x1068,&eyePos,soundParam,reverb);
 f32 factor=mScaleFactor;
 if(factor>0.0f) {
  u32 tag=mTagId;
  if(tag==0xFFFFFFFF) {
   f32 radius=gabi::load<f32>(0x101C7D0C); f32 mult=attribute(this,0);
   f32 inner=radius*mult,outer=(radius+500.0f)*mult;
   gabi::Local<cXyz> size; size->x=(inner*factor)/5000.0f; size->z=(outer*factor)/5000.0f;
   // GHS intentionally leaves the Y slot unspecified.
   s8 room=tevStr.mRoomNo;
   mTagId=gabi::call<u32>(0x025D5834,0x180,-1,&current.pos,room,&current.angle,size.get(),-1,0);
  } else {
   gabi::Local<be<u32>> id; *id=tag;
   u32 found=gabi::call<u32>(0x025D5218,0x025E1234,id.get());
   if(found) {
    f32 inner=gabi::load<f32>(0x101C7D0C)*attribute(this,0); inner=inner*(f32)mScaleFactor;
    gabi::store<f32>(found+0x3BC,inner);
    f32 outer=(gabi::load<f32>(0x101C7D0C)+500.0f)*attribute(this,0); outer=outer*(f32)mScaleFactor;
    gabi::store<f32>(found+0x3B8,outer);
   }
  }
 } else if(factor==0.0f && mTagId!=0xFFFFFFFF) {
  gabi::Local<be<u32>> id; *id=(u32)mTagId;
  u32 found=gabi::call<u32>(0x025D5218,0x025E1234,id.get()); if(found) gabi::call(0x025D57E0,gabi::at<fopAc_ac_c>(found));
 }
 set_state_map(); return true;
}
VERIFY(0x02319F4C,&Act_c::_execute);
static void set_material(u8* material,u32 alpha) {
 WWHD_FUNC(0x0231A2D0,void,material,alpha);
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
VERIFY(0x0231A2D0,set_material);
bool Act_c::_draw() {
 WWHD_FUNC(0x0231A470,bool,this);
 u32 env=gabi::call<u32>(0x02555D0C); gabi::call(0x025626A4,gabi::at<u8>(env),2,&current.pos,&tevStr);
 env=gabi::call<u32>(0x02555D0C); J3DModel* model=mpModel;
 gabi::call(0x02562F5C,gabi::at<u8>(env),model,&tevStr);
 model=mpModel; u32 data=gabi::load<u32>(gabi::ea(model)+0xAC); f32 frame=gabi::load<f32>(gabi::ea(this)+0x3BC);
 mBtk.entry(gabi::at<J3DModelData>(data),frame);
 u8 alpha=(u8)gabi::ftoi(255.5f*(f32)mScaleFactor); model=mpModel;
 data=gabi::load<u32>(gabi::ea(model)+0xAC); u32 joint=gabi::load<u32>(data+8);
 set_material(gabi::at<u8>(gabi::load<u32>(joint+0x10)),alpha);
 model=mpModel; gabi::call(0x025E2DE0,model,0); return true;
}
VERIFY(0x0231A470,&Act_c::_draw);
static s32 create(Act_c* actor) { WWHD_FUNC(0x0231A514,s32,actor); return actor->_create(); }
VERIFY(0x0231A514,create);
static bool remove(Act_c* actor) { WWHD_FUNC(0x0231A518,bool,actor); return actor->_delete(); }
VERIFY(0x0231A518,remove);
static bool execute(Act_c* actor) { WWHD_FUNC(0x0231A51C,bool,actor); return actor->_execute(); }
VERIFY(0x0231A51C,execute);
static bool draw(Act_c* actor) { WWHD_FUNC(0x0231A520,bool,actor); return actor->_draw(); }
VERIFY(0x0231A520,draw);
static BOOL is_delete(Act_c* actor) { WWHD_FUNC(0x0231A624,BOOL,actor); return TRUE; }
VERIFY(0x0231A624,is_delete);
static void base_destructor(Act_c* actor,s32 flags) { WWHD_FUNC(0x0231974C,void,actor,flags); if(actor) { gabi::call(0x025D50BC,actor,0); if(flags&1) gabi::call(0x0273AF40,actor); } }
VERIFY(0x0231974C,base_destructor);
static void actor_destructor(Act_c* actor,s32 flags) { WWHD_FUNC(0x0231A5D0,void,actor,flags); if(actor) { gabi::call(0x025D50BC,actor,0); if(flags&1) gabi::call(0x0273AF40,actor); } }
VERIFY(0x0231A5D0,actor_destructor);
static void empty_destructor(u8* object,s32 flags) { WWHD_FUNC(0x0231A5B8,void,object,flags); if(object&&(flags&1)) gabi::call(0x0273AF40,object); }
VERIFY(0x0231A5B8,empty_destructor);
static void empty_inline(Act_c* actor) { WWHD_FUNC(0x0231A5CC,void,actor); }
VERIFY(0x0231A5CC,empty_inline);
static void static_init() {
 WWHD_FUNC(0x0231A524,void);
 for(u32 off : {8u,0u,12u,4u}) gabi::store<u32>(0x10469130+off,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101C7CE8));
 gabi::store<f32>(0x10469124,-3.1415927410125732f); gabi::store<f32>(0x10469128,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046912C)); gabi::call(0x028F026C,gabi::at<u8>(0x101C7CF4));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046912D)); gabi::call(0x028F026C,gabi::at<u8>(0x101C7D00));
}
VERIFY(0x0231A524,static_init);
}
