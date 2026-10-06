/* WWHD Nintendo Gallery hatch. */
#include "d/actor/d_a_obj_ohatch.h"
static constexpr u32 MatrixStack=0x1048D0CC;
static f32 sin_angle(u16 angle) { return gabi::load<f32>(0x104A44F8+(angle>>3)*8); }
static void action(daObjOhatch_c* actor,u32 address) {
 actor->mActionDelta=0; actor->mActionIndex=-1; actor->mActionTarget=address;
}
static bool used(dBgW* bg) { return gabi::load<u32>(gabi::ea(bg))<256; }
static s32 parameter(daObjOhatch_c* actor,s32 width,s32 shift) {
 WWHD_FUNC(0x0237FCBC,s32,actor,width,shift);
 u32 mask=(width&32)?0:(1u<<(width&31));
 u32 value=actor->mParameters;
 return ((shift&32)?0:(value>>(shift&31)))&(mask-1);
}
VERIFY(0x0237FCBC,parameter);
bool daObjOhatch_c::create_heap() {
 WWHD_FUNC(0x0237F07C,bool,this);
 auto* data=(J3DModelData*)dComIfG_getObjectRes(STR(0x1002DF24),4,0x1002DF2C);
 if(!data) { JUT_ASSERT_fail(STR(0x1002DF58),0x137,STR(0x1002DF54)); return false; }
 mModel=mDoExt_J3DModel__create(data,0x80000,0x11000022);
 void* closed=dComIfG_getObjectRes(STR(0x1002DF24),7,0x1002DF2C);
 mClosedBg=gabi::call<dBgW*>(0x024F2478,closed,1,&mBgMatrix);
 void* opened=dComIfG_getObjectRes(STR(0x1002DF24),8,0x1002DF2C);
 dBgW* openBg=gabi::call<dBgW*>(0x024F2478,opened,1,&mBgMatrix);
 J3DModel* model=mModel; mOpenBg=openBg;
 return model!=nullptr && (dBgW*)mClosedBg!=nullptr && openBg!=nullptr;
}
VERIFY(0x0237F07C,&daObjOhatch_c::create_heap);
static bool solidHeapCB(daObjOhatch_c* actor) {
 WWHD_FUNC(0x0237F188,bool,actor); return actor->create_heap();
}
VERIFY(0x0237F188,solidHeapCB);
void daObjOhatch_c::init_mtx() {
 WWHD_FUNC(0x0237F18C,void,this);
 f32 x=scale.x,y=scale.y,z=scale.z; u32 model=gabi::ea((J3DModel*)mModel);
 gabi::store<f32>(model+0xBC,x); gabi::store<f32>(model+0xC0,y); gabi::store<f32>(model+0xC4,z);
 gabi::call(0x028E93CC,gabi::at<Mtx34>(MatrixStack),(f32)current.pos.x,(f32)current.pos.y,(f32)current.pos.z);
 gabi::call(0x025F19F8,gabi::at<Mtx34>(MatrixStack),(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 mDoMtx_stack_push();
 gabi::call(0x025F24E0,0.0f,25.0f,-190.0f);
 gabi::call(0x025F1BF4,gabi::at<Mtx34>(MatrixStack),(s16)mHingeAngle);
 gabi::call(0x025F24E0,0.0f,-25.0f,190.0f);
 J3DModel_setBaseTRMtx(mModel,gabi::at<Mtx34>(MatrixStack));
 mDoMtx_stack_pop();
 gabi::call(0x025F2518,(f32)scale.x,(f32)scale.y,(f32)scale.z);
 gabi::call(0x028E90D4,gabi::at<Mtx34>(MatrixStack),&mBgMatrix);
}
VERIFY(0x0237F18C,&daObjOhatch_c::init_mtx);
s32 daObjOhatch_c::_create() {
 WWHD_FUNC(0x0237F2EC,s32,this);
 u32 flags=actor_condition;
 if(!(flags&8)) {
  if(gabi::ea(this)) { gabi::call(0x025D4ED0,this); flags=actor_condition; __vtbl=0x1002DF44; }
  actor_condition=flags|8;
 }
 s32 phase=dComIfG_resLoad(&mPhase,STR(0x1002DF24));
 if(phase!=4) return phase;
 if(!gabi::call<BOOL>(0x025D63E8,this,0x0237F188,0xB90)) return 5;
 u32 model=gabi::ea((J3DModel*)mModel); cullMtx=model?model+0xC8:0;
 s32 sw=parameter(this,8,0); mSwitch=sw;
 bool opened=sw!=255 && fopAcM_isSwitch(this,sw)==1;
 if(opened) { action(this,0x0237FC60); mHingeAngle=-0x4000; }
 else action(this,0x0237F7D8);
 u32 play=dComIfGp_ea(); dBgW* bg=opened?(dBgW*)mOpenBg:(dBgW*)mClosedBg;
 if(gabi::call<s32>(0x024EEA6C,gabi::at<u8>(play+0x12A0),bg,this)) phase=5;
 init_mtx(); return phase;
}
VERIFY(0x0237F2EC,&daObjOhatch_c::_create);
bool daObjOhatch_c::_delete() {
 WWHD_FUNC(0x0237F47C,bool,this);
 dComIfG_resDelete(&mPhase,STR(0x1002DF24));
 if((JKRSolidHeap*)heap) {
  dBgW* bg=mClosedBg;
  if(bg) { if(used(bg)) { u32 play=dComIfGp_ea(); bg=mClosedBg; gabi::call(0x020087EC,gabi::at<u8>(play+0x12A0),bg); } mClosedBg=nullptr; }
  bg=mOpenBg;
  if(bg) { if(used(bg)) { u32 play=dComIfGp_ea(); bg=mOpenBg; gabi::call(0x020087EC,gabi::at<u8>(play+0x12A0),bg); } mOpenBg=nullptr; }
 }
 return true;
}
VERIFY(0x0237F47C,&daObjOhatch_c::_delete);
void daObjOhatch_c::set_mtx() {
 WWHD_FUNC(0x0237F52C,void,this);
 f32 amplitude=(s16)mVibrationAmplitude; u16 phase=mVibrationPhase;
 f32 wave=sin_angle(phase); f32 height=mTremorHeight,z=current.pos.z,y=current.pos.y;
 s16 angle=(s16)((s16)mHingeAngle+(s16)gabi::ftoi(amplitude*wave));
 y=y+height; f32 x=current.pos.x;
 gabi::call(0x028E93CC,gabi::at<Mtx34>(MatrixStack),x,y,z);
 gabi::call(0x025F19F8,gabi::at<Mtx34>(MatrixStack),(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 gabi::call(0x025F24E0,0.0f,25.0f,-190.0f);
 gabi::call(0x025F1BF4,gabi::at<Mtx34>(MatrixStack),angle);
 gabi::call(0x025F24E0,0.0f,-25.0f,190.0f);
 J3DModel_setBaseTRMtx(mModel,gabi::at<Mtx34>(MatrixStack));
}
VERIFY(0x0237F52C,&daObjOhatch_c::set_mtx);
bool daObjOhatch_c::_execute() {
 WWHD_FUNC(0x0237F6BC,bool,this);
 dBgW* bg=mClosedBg; if(bg && used(bg)) gabi::call(0x024F43DC,bg);
 bg=mOpenBg; if(bg && used(bg)) gabi::call(0x024F43DC,bg);
 set_mtx();
 // GHS pointer-to-member descriptor supports both direct and virtual actions.
 ptmf_call(gabi::ea(this)+0x3F8,this);
 return true;
}
VERIFY(0x0237F6BC,&daObjOhatch_c::_execute);
bool daObjOhatch_c::_draw() {
 WWHD_FUNC(0x0237F770,bool,this);
 u32 env=gabi::call<u32>(0x02555D0C);
 gabi::call(0x025626A4,gabi::at<u8>(env),1,&current.pos,&tevStr);
 env=gabi::call<u32>(0x02555D0C); J3DModel* model=mModel;
 gabi::call(0x02562F5C,gabi::at<u8>(env),model,&tevStr);
 model=mModel; gabi::call(0x025E2DE0,model,0); return true;
}
VERIFY(0x0237F770,&daObjOhatch_c::_draw);
void daObjOhatch_c::close_wait_act_proc() {
 WWHD_FUNC(0x0237F7D8,void,this);
 s32 sw=mSwitch; if(sw==255 || fopAcM_isSwitch(this,sw)!=1) return;
 u32 bgBase=dComIfGp_ea()+0x12A0;
 u32 play=dComIfGp_ea(); s32 event=gabi::call<s32>(0x02543F10,gabi::at<u8>(play+0x52C4),STR(0x1002DF9C),255);
 if(!gabi::call<u32>(0x02544044,gabi::at<u8>(bgBase+0x4024),event)) return;
 play=dComIfGp_ea(); s32 staff=gabi::call<s32>(0x02542D88,gabi::at<u8>(play+0x52C4),STR(0x1002DF8C),0,0);
 if(staff==-1) return;
 play=dComIfGp_ea(); u32 cut=gabi::call<u32>(0x02544830,gabi::at<u8>(play+0x52C4),staff);
 u32 name=0x1002DF94; u8 left,right;
 do { left=gabi::load<u8>(cut++); right=gabi::load<u8>(name++); } while(left==right && left);
 if(left!=right) return;
 s32 reverb=dComIfGp_getReverb(current.roomNo);
 mDoAud_seStart(0x6A01,&current.pos,0,reverb);
 dBgW* bg=mClosedBg;
 if(bg && used(bg)) { play=dComIfGp_ea(); bg=mClosedBg; gabi::call(0x020087EC,gabi::at<u8>(play+0x12A0),bg); }
 bg=mOpenBg;
 if(bg && !used(bg)) { play=dComIfGp_ea(); bg=mOpenBg; gabi::call(0x024EEA6C,gabi::at<u8>(play+0x12A0),bg,this); }
 play=dComIfGp_ea(); gabi::Local<cXyz> up; up->x=0.0f; up->z=0.0f; up->y=1.0f;
 gabi::call(0x025CB374,gabi::at<u8>(play+0x599C),3,1,up.get());
 action(this,0x0237F990);
}
VERIFY(0x0237F7D8,&daObjOhatch_c::close_wait_act_proc);
void daObjOhatch_c::tremor_act_proc() {
 WWHD_FUNC(0x0237F990,void,this);
 s32 phase=(s32)((u32)(s32)mTremorPhase+0x2000); mTremorPhase=phase;
 if(phase>0x20000) { mTremorHeight=0.0f; action(this,0x0237FA0C); }
 else { f32 wave=sin_angle((u16)phase); if(!(wave>=0.0f)) wave=-wave; mTremorHeight=wave*6.0f; }
}
VERIFY(0x0237F990,&daObjOhatch_c::tremor_act_proc);
void daObjOhatch_c::open_act_proc() {
 WWHD_FUNC(0x0237FA0C,void,this);
 s16 angle=mHingeAngle;
 f32 fraction=(f32)angle*-0.00006103515625f;
 u16 phase=(u16)gabi::ftoi(fraction*32768.0f);
 f32 wave=sin_angle(phase); f32 speed=(wave+1.0f)*0.5f;
 s16 next=(s16)(angle+(s16)gabi::ftoi(-4096.0f*speed));
 if(next>-0x4000) { mHingeAngle=next; return; }
 mHingeAngle=-0x4000; gabi::call(0x025E1988,0x806);
 mVibrationAmplitude=0x400;
 u32 play=dComIfGp_ea(); gabi::Local<cXyz> up; up->y=1.0f; up->x=0.0f; up->z=0.0f;
 gabi::call(0x025CB374,gabi::at<u8>(play+0x599C),6,1,up.get());
 action(this,0x0237FB74);
}
VERIFY(0x0237FA0C,&daObjOhatch_c::open_act_proc);
void daObjOhatch_c::vibrate_act_proc() {
 WWHD_FUNC(0x0237FB74,void,this);
 s16 amplitude=(s16)((s16)mVibrationAmplitude-64); mVibrationAmplitude=amplitude;
 if(amplitude<0) action(this,0x0237FC60);
 else mVibrationPhase=(s16)((s16)mVibrationPhase+0x2000);
}
VERIFY(0x0237FB74,&daObjOhatch_c::vibrate_act_proc);
void daObjOhatch_c::open_wait_act_proc() { WWHD_FUNC(0x0237FC60,void,this); }
VERIFY(0x0237FC60,&daObjOhatch_c::open_wait_act_proc);
static s32 create(daObjOhatch_c* actor) { WWHD_FUNC(0x0237F478,s32,actor); return actor->_create(); }
VERIFY(0x0237F478,create);
static bool remove(daObjOhatch_c* actor) { WWHD_FUNC(0x0237F528,bool,actor); return actor->_delete(); }
VERIFY(0x0237F528,remove);
static bool execute(daObjOhatch_c* actor) { WWHD_FUNC(0x0237F76C,bool,actor); return actor->_execute(); }
VERIFY(0x0237F76C,execute);
static bool draw(daObjOhatch_c* actor) { WWHD_FUNC(0x0237F7CC,bool,actor); return actor->_draw(); }
VERIFY(0x0237F7CC,draw);
static BOOL is_delete(daObjOhatch_c* actor) { WWHD_FUNC(0x0237F7D0,BOOL,actor); return TRUE; }
VERIFY(0x0237F7D0,is_delete);
static void base_destructor(daObjOhatch_c* actor,s32 flags) {
 WWHD_FUNC(0x0237F028,void,actor,flags);
 if(actor) { gabi::call(0x025D50BC,actor,0); if(flags&1) gabi::call(0x0273AF40,actor); }
}
VERIFY(0x0237F028,base_destructor);
static void actor_destructor(daObjOhatch_c* actor,s32 flags) {
 WWHD_FUNC(0x0237FC64,void,actor,flags);
 if(actor) { gabi::call(0x025D50BC,actor,0); if(flags&1) gabi::call(0x0273AF40,actor); }
}
VERIFY(0x0237FC64,actor_destructor);
static void empty_destructor(u8* object,s32 flags) {
 WWHD_FUNC(0x0237FC4C,void,object,flags);
 if(object && (flags&1)) gabi::call(0x0273AF40,object);
}
VERIFY(0x0237FC4C,empty_destructor);
static void static_init() {
 WWHD_FUNC(0x0237FBB8,void);
 for(u32 off : {8u,0u,12u,4u}) gabi::store<u32>(0x1046BACC+off,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101CC09C));
 gabi::store<f32>(0x1046BAC0,-3.1415927410125732f); gabi::store<f32>(0x1046BAC4,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046BAC8));
 gabi::call(0x028F026C,gabi::at<u8>(0x101CC0A8));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046BAC9));
 gabi::call(0x028F026C,gabi::at<u8>(0x101CC0B4));
}
VERIFY(0x0237FBB8,static_init);
static void empty_inline(daObjOhatch_c* actor) { WWHD_FUNC(0x0237FCB8,void,actor); }
VERIFY(0x0237FCB8,empty_inline);
