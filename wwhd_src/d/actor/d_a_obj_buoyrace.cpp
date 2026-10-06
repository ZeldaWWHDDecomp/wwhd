/* WWHD race buoy. */
#include "d/actor/d_a_obj_buoyrace.h"
namespace daObjBuoyrace {
static f32 phase_sine(u16 angle) { return gabi::load<f32>(0x104A44F8+u32(angle>>3)*8); }
s32 Act_c::create_load() {
 WWHD_FUNC(0x0232EDE0,s32,this);
 s32 phase=dComIfG_resLoad(&mPhsKiba,STR(0x100269CC));
 if(phase==4) phase=dComIfG_resLoad(&mPhsHasi,STR(0x100269D8));
 return phase;
}
VERIFY(0x0232EDE0,&Act_c::create_load);
bool Act_c::create_heap() {
 WWHD_FUNC(0x0232EE30,bool,this);
 auto* kiba=(J3DModelData*)dComIfG_getObjectRes(STR(0x100269CC),4,0x10026898);
 if(!kiba) JUT_ASSERT_fail(STR(0x100268C0),0x77,STR(0x100268D8));
 mpModelKiba=mDoExt_J3DModel__create(kiba,0x80000,0x11000022);
 auto* hasi=(J3DModelData*)dComIfG_getObjectRes(STR(0x100269D8),3,0x10026898);
 if(!hasi) JUT_ASSERT_fail(STR(0x100268C0),0x80,STR(0x100268EC));
 J3DModel* second=mDoExt_J3DModel__create(hasi,0x80000,0x11000022);
 J3DModel* first=mpModelKiba; mpModelHasi=second;
 return first && second;
}
VERIFY(0x0232EE30,&Act_c::create_heap);
static bool heap_callback(Act_c* actor) { WWHD_FUNC(0x0232EF30,bool,actor); return actor->create_heap(); }
VERIFY(0x0232EF30,heap_callback);
void Act_c::set_water_pos() {
 WWHD_FUNC(0x0232EF34,void,this);
 f32 x=current.pos.x,z=current.pos.z;
 f32 h1=gabi::call<f32>(0x0246BA0C,x-32.0f,z-32.0f);
 x=current.pos.x; z=current.pos.z;
 f32 h2=gabi::call<f32>(0x0246BA0C,x-32.0f,z+32.0f);
 x=current.pos.x; z=current.pos.z;
 f32 h3=gabi::call<f32>(0x0246BA0C,x+32.0f,z-32.0f);
 gabi::Local<cXyz> alongZ,alongX,cross;
 alongZ->x=0.0f; alongZ->y=h2-h1; alongZ->z=32.0f;
 alongX->x=32.0f; alongX->y=h3-h1; alongX->z=0.0f;
 mMeanSeaHeight=((h1+h2)+h3)*0.3333333432674408f;
 gabi::call(0x0201B080,alongZ.get(),cross.get(),alongX.get());
 mNormal.copy(*cross);
 gabi::call(0x0201B31C,&mNormal,cross.get());
}
VERIFY(0x0232EF34,&Act_c::set_water_pos);
static s32 parameter(Act_c* actor,s32 width,s32 shift) {
 WWHD_FUNC(0x0232F918,s32,actor,width,shift);
 u32 value=actor->mParameters,mask=(width&32)?0:(1u<<(width&31));
 return ((shift&32)?0:(value>>(shift&31)))&(mask-1);
}
VERIFY(0x0232F918,parameter);
void Act_c::set_rope_pos() {
 WWHD_FUNC(0x0232F05C,void,this);
 u32 link=parentActorID,parent=0; gabi::Local<be<u32>> id; *id=link;
 if(link!=0xFFFFFFFF) parent=gabi::call<u32>(0x025D5218,0x025E1234,id.get());
 if(!parent) return;
 s32 index=parameter(this,8,0); u32 line=parameter(this,8,8);
 if(line>=4) JUT_ASSERT_fail(STR(0x10026910),0x9B,STR(0x10026930));
 u32 list=gabi::load<u32>(parent+line*0x148+0x2BD0);
 u32 point=gabi::load<u32>(list)+u32(index)*0x30;
 if(!point) JUT_ASSERT_fail(STR(0x1002694C),0x194,STR(0x10026920));
 gabi::Local<cXyz> offset; offset->x=0.0f; offset->y=610.0f; offset->z=0.0f;
 gabi::call(0x028E8F64,gabi::at<Mtx34>(0x1048D0CC),offset.get(),gabi::at<cXyz>(point));
}
VERIFY(0x0232F05C,&Act_c::set_rope_pos);
void Act_c::set_mtx() {
 WWHD_FUNC(0x0232F170,void,this);
 gabi::Local<cXyz> modelScale,direction,offset,translated;
 gabi::Local<Quaternion_l> rotation;
 u32 kiba=gabi::ea((J3DModel*)mpModelKiba);
 gabi::call(0x0201AE48,&scale,modelScale.get(),2.0f);
 f32 sx=modelScale->x,sy=modelScale->y,sz=modelScale->z;
 gabi::store<f32>(kiba+0xBC,sx); gabi::store<f32>(kiba+0xC0,sy); gabi::store<f32>(kiba+0xC4,sz);
 sx=scale.x; sz=scale.z; sy=scale.y;
 f32 hx=(sx+sx)*1.75f,hz=(sz+sz)*1.75f,hy=(sy+sy)*2.5f;
 u32 hasi=gabi::ea((J3DModel*)mpModelHasi);
 gabi::store<f32>(hasi+0xC4,hz); gabi::store<f32>(hasi+0xBC,hx); gabi::store<f32>(hasi+0xC0,hy);
 auto* stack=gabi::at<Mtx34>(0x1048D0CC);
 gabi::call(0x028E93CC,stack,(f32)current.pos.x,(f32)current.pos.y,(f32)current.pos.z);
 direction->x=(f32)mSwayX; direction->y=1.0f; direction->z=(f32)mSwayZ;
 gabi::call(0x025F24E0,0.0f,150.0f,0.0f);
 gabi::call(0x023123D8,rotation.get(),direction.get());
 gabi::call(0x025F25CC,rotation.get());
 gabi::call(0x025F1B48,stack,(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 gabi::call(0x025F24E0,0.0f,-150.0f,0.0f);
 J3DModel_setBaseTRMtx(mpModelKiba,stack);
 set_rope_pos();
 offset->x=0.0f; offset->y=290.0f; offset->z=0.0f;
 gabi::call(0x028E9044,stack,offset.get(),translated.get());
 f32 tx=translated->x,ty=translated->y,tz=translated->z;
 f32 y=gabi::load<f32>(0x1048D0CC+0x1C),x=gabi::load<f32>(0x1048D0CC+0xC);
 x=x+tx; y=y+ty; f32 z=gabi::load<f32>(0x1048D0CC+0x2C); z=z+tz;
 gabi::store<f32>(0x1048D0CC+0xC,x); gabi::store<f32>(0x1048D0CC+0x1C,y); gabi::store<f32>(0x1048D0CC+0x2C,z);
 J3DModel_setBaseTRMtx(mpModelHasi,stack);
}
VERIFY(0x0232F170,&Act_c::set_mtx);
void Act_c::init_mtx() { WWHD_FUNC(0x0232F3D4,void,this); set_mtx(); }
VERIFY(0x0232F3D4,&Act_c::init_mtx);
s32 Act_c::_create() {
 WWHD_FUNC(0x0232F3D8,s32,this);
 u32 flags=actor_condition;
 if(!(flags&8)) {
  if(gabi::ea(this)) { gabi::call(0x025D4ED0,this); flags=actor_condition; __vtbl=0x100268B0; }
  actor_condition=flags|8;
 }
 s32 phase=create_load(); if(phase!=4) return phase;
 if(!gabi::call<BOOL>(0x025D63E8,this,0x0232EF30,0x980)) return 5;
 set_water_pos(); f32 bob=gabi::fmadds(0.5f,phase_sine((u16)mPhaseAngle),-120.0f);
 current.pos.y=(f32)mMeanSeaHeight+(bob+bob);
 u32 model=gabi::ea((J3DModel*)mpModelKiba); cullMtx=model?model+0xC8:0;
 fopAcM_setCullSizeBox(this,-152.0f,-2.0f,-152.0f,152.0f,590.0f,152.0f);
 init_mtx(); return phase;
}
VERIFY(0x0232F3D8,&Act_c::_create);
bool Act_c::_delete() {
 WWHD_FUNC(0x0232F508,bool,this);
 dComIfG_resDelete(&mPhsKiba,STR(0x100269CC)); dComIfG_resDelete(&mPhsHasi,STR(0x100269D8)); return true;
}
VERIFY(0x0232F508,&Act_c::_delete);
void Act_c::afl_calc_sway() {
 WWHD_FUNC(0x0232F554,void,this);
 f32 x=(f32)mNormal.x*0.3499999940395355f,z=(f32)mNormal.z*0.3499999940395355f;
 f32 square=gabi::fmadds(x,x,z*z);
 if(square>0.09000000357627869f) { f32 length=gabi::call<f32>(0x028F4384,square); f32 ratio=0.30000001192092896f/length; z=z*ratio; x=x*ratio; }
 f32 oldX=mSwayX,vx=mSwayVelocityX,vz=mSwayVelocityZ,oldZ=mSwayZ;
 f32 dampingX=-(vx*0.03999999910593033f),dampingZ=-(vz*0.03999999910593033f);
 f32 accelX=gabi::fnmsubs(oldX-x,0.019999999552965164f,dampingX);
 f32 accelZ=gabi::fnmsubs(oldZ-z,0.019999999552965164f,dampingZ);
 vx=vx+accelX; vz=vz+accelZ;
 mSwayVelocityX=vx; mSwayVelocityZ=vz; mSwayX=oldX+vx; mSwayZ=oldZ+vz;
}
VERIFY(0x0232F554,&Act_c::afl_calc_sway);
void Act_c::afl_calc() {
 WWHD_FUNC(0x0232F650,void,this);
 f32 old=mBob,velocity=mBobVelocity;
 velocity=gabi::fmadds(old,-0.009999999776482582f,velocity)*0.9399999976158142f;
 f32 bob=old+velocity; mBobVelocity=velocity;
 if(bob>30.0f) bob=30.0f; mBob=bob;
 f32 random=gabi::call<f32>(0x02019788);
 s16 increment=(s16)gabi::ftoi(1000.0f*(random+1.0f));
 u16 phase=u16((s16)mPhaseAngle+increment);
 f32 mean=mMeanSeaHeight; mPhaseAngle=phase;
 f32 wave=gabi::fmadds(0.5f,phase_sine(phase),-120.0f);
 f32 target=(mean+(f32)mBob)+(wave+wave);
 gabi::call(0x0200F5C8,&current.pos.y,target,50.0f); afl_calc_sway();
}
VERIFY(0x0232F650,&Act_c::afl_calc);
bool Act_c::_execute() { WWHD_FUNC(0x0232F748,bool,this); set_water_pos(); afl_calc(); set_mtx(); return true; }
VERIFY(0x0232F748,&Act_c::_execute);
bool Act_c::_draw() {
 WWHD_FUNC(0x0232F788,bool,this);
 u32 env=gabi::call<u32>(0x02555D0C); gabi::call(0x025626A4,gabi::at<u8>(env),0,&current.pos,&tevStr);
 env=gabi::call<u32>(0x02555D0C); J3DModel* model=mpModelKiba; gabi::call(0x02562F5C,gabi::at<u8>(env),model,&tevStr);
 env=gabi::call<u32>(0x02555D0C); model=mpModelHasi; gabi::call(0x02562F5C,gabi::at<u8>(env),model,&tevStr);
 model=mpModelKiba; gabi::call(0x025E2DE0,model,0); model=mpModelHasi; gabi::call(0x025E2DE0,model,0); return true;
}
VERIFY(0x0232F788,&Act_c::_draw);
static s32 create(Act_c* actor) { WWHD_FUNC(0x0232F800,s32,actor); return actor->_create(); }
VERIFY(0x0232F800,create);
static bool remove(Act_c* actor) { WWHD_FUNC(0x0232F804,bool,actor); return actor->_delete(); }
VERIFY(0x0232F804,remove);
static bool execute(Act_c* actor) { WWHD_FUNC(0x0232F808,bool,actor); return actor->_execute(); }
VERIFY(0x0232F808,execute);
static bool draw(Act_c* actor) { WWHD_FUNC(0x0232F80C,bool,actor); return actor->_draw(); }
VERIFY(0x0232F80C,draw);
static void static_init() {
 WWHD_FUNC(0x0232F810,void);
 for(u32 off : {8u,0u,12u,4u}) gabi::store<u32>(0x10469418+off,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101C8858));
 gabi::store<f32>(0x1046940C,-3.1415927410125732f); gabi::store<f32>(0x10469410,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x10469414)); gabi::call(0x028F026C,gabi::at<u8>(0x101C8864));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x10469415)); gabi::call(0x028F026C,gabi::at<u8>(0x101C8870));
}
VERIFY(0x0232F810,static_init);
static void empty_destructor(u8* object,s32 flags) { WWHD_FUNC(0x0232F8A4,void,object,flags); if(object&&(flags&1)) gabi::call(0x0273AF40,object); }
VERIFY(0x0232F8A4,empty_destructor);
static void empty_inline(Act_c* actor) { WWHD_FUNC(0x0232F8B8,void,actor); }
VERIFY(0x0232F8B8,empty_inline);
static void actor_destructor(Act_c* actor,s32 flags) { WWHD_FUNC(0x0232F8BC,void,actor,flags); if(actor) { gabi::call(0x025D50BC,actor,0); if(flags&1) gabi::call(0x0273AF40,actor); } }
VERIFY(0x0232F8BC,actor_destructor);
static BOOL is_delete(Act_c* actor) { WWHD_FUNC(0x0232F910,BOOL,actor); return TRUE; }
VERIFY(0x0232F910,is_delete);
}
