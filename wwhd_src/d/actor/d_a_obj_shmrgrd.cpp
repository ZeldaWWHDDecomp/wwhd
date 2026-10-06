/* Spiked Skull Hammer switch. Derived game source;
 */
#include "d/actor/d_a_obj_shmrgrd.h"
static constexpr u32 list_address=0x101CCB54,damage_address=0x101CCB58,dir_address=0x101CCB5C;
static void* member(daObjShmrgrd_c* a,u32 offset) { return gabi::at<void>(gabi::ea(a)+offset);
  }
static void sound(daObjShmrgrd_c* a,u32 id) { s32 reverb=dComIfGp_getReverb(a->current.roomNo);
  gabi::call(0x025E1A40,id,&a->eyePos,0,reverb);
  }
static daObjShmrgrd_c* search_target_next(daObjShmrgrd_c* a,daObjShmrgrd_c* target) {
 WWHD_FUNC(0x0238EEF4,daObjShmrgrd_c*,a,target);
 auto cur=gabi::at<daObjShmrgrd_c>(gabi::load<u32>(list_address));
 while(cur) { auto next=(daObjShmrgrd_c*)cur->mpNext;
  if(next==target)return cur;
  cur=next;
  }
 return nullptr;
}
VERIFY(0x0238EEF4,search_target_next);
void daObjShmrgrd_c::register_list() {
 WWHD_FUNC(0x0238EF28,void,this);
  mpNext=nullptr;
 if(gabi::load<u32>(list_address)==0)gabi::store<u32>(list_address,gabi::ea(this));
 else { auto last=gabi::call<daObjShmrgrd_c*>(0x0238EEF4,this,nullptr);
  if(last)last->mpNext=this;
  }
}
VERIFY(0x0238EF28,&daObjShmrgrd_c::register_list);
void daObjShmrgrd_c::leave_list() {
 WWHD_FUNC(0x0238F254,void,this);
 auto prev=gabi::call<daObjShmrgrd_c*>(0x0238EEF4,this,this);
  auto next=(daObjShmrgrd_c*)mpNext;
 if(prev)prev->mpNext=next;
 else if(!next)gabi::store<u32>(list_address,0);
 else if(gabi::ea(this)==gabi::load<u32>(list_address))gabi::store<u32>(list_address,gabi::ea(next));
}
VERIFY(0x0238F254,&daObjShmrgrd_c::leave_list);
void daObjShmrgrd_c::mode_lower_init() {
 WWHD_FUNC(0x0238EE5C,void,this);
  u32 flags=gabi::load<u32>(gabi::ea(this)+0x39C);
  mMode=2;
  gabi::store<u32>(gabi::ea(this)+0x39C,flags&~1u);
  mTargetHFrac=0;
}
VERIFY(0x0238EE5C,&daObjShmrgrd_c::mode_lower_init);
void daObjShmrgrd_c::mode_upper_init() {
 WWHD_FUNC(0x0238EE80,void,this);
  gabi::call(0x02018428,member(this,0x640),112.5f);
  gabi::call(0x02018428,member(this,0x4D4),112.5f);
  mMode=0;
  mTargetHFrac=1;
}
VERIFY(0x0238EE80,&daObjShmrgrd_c::mode_upper_init);
void daObjShmrgrd_c::mode_u_l_init() {
 WWHD_FUNC(0x0238FC84,void,this);
  sound(this,0x69B2);
  mMode=1;
  mTargetHFrac=0;
}
VERIFY(0x0238FC84,&daObjShmrgrd_c::mode_u_l_init);
void daObjShmrgrd_c::crush_start() { WWHD_FUNC(0x0238F4F0,void,this);
  mCrushState=1;
  }
VERIFY(0x0238F4F0,&daObjShmrgrd_c::crush_start);
void daObjShmrgrd_c::crush_proc() {
 WWHD_FUNC(0x0238F790,void,this);
 if(mCrushState==1) { mCrushTimer=18;
  mCrushState=2;
  mScaleY=0.2f;
  }
 else if(mCrushState==2) { s16 timer=(s16)((s16)mCrushTimer-1);
  mCrushTimer=timer;
  if(timer>0)return;
  mCrushState=0;
  mScaleY=1;
  }
}
VERIFY(0x0238F790,&daObjShmrgrd_c::crush_proc);
void daObjShmrgrd_c::vib_proc() {
 WWHD_FUNC(0x0238F734,void,this);
 f32 z=mAngleZ,x=mAngleX,sz=mAngleSpeedZ,sx=mAngleSpeedX;
 f32 nz=sz+gabi::fnmsubs(sz,0.18f,-(z*0.99f)), nx=sx+gabi::fnmsubs(sx,0.18f,-(x*0.99f));
 mAngleSpeedZ=nz;
  mAngleSpeedX=nx;
  mAngleZ=z+nz;
  mAngleX=x+nx;
}
VERIFY(0x0238F734,&daObjShmrgrd_c::vib_proc);
void daObjShmrgrd_c::calc_top_pos() {
 WWHD_FUNC(0x0238F6D8,void,this);
 f32 target=mTargetHFrac,cur=mCurHFrac,speed=mVSpeed;
 f32 next=gabi::fnmsubs(cur-target,0.8f,speed);
  next=gabi::fnmsubs(next,0.55f,next);
 f32 height=cur+next;
  if(height-1.0f>=0)height=1;
 mVSpeed=next;
  mCurHFrac=height;
  mTopPos=(1.0f-height)*-85.0f;
}
VERIFY(0x0238F6D8,&daObjShmrgrd_c::calc_top_pos);
void daObjShmrgrd_c::vib_start(s16 dir,f32 mag) {
 WWHD_FUNC(0x0238FC50,void,this,dir,mag);
 u32 table=0x104A44F8+((u32)(u16)dir>>3)*8;
 mAngleSpeedZ=gabi::load<f32>(table+4)*mag;
  mAngleSpeedX=gabi::load<f32>(table)*mag;
}
VERIFY(0x0238FC50,&daObjShmrgrd_c::vib_start);
s32 daObjShmrgrd_c::check_player_angle(fopAc_ac_c* actor) {
 WWHD_FUNC(0x0238F360,s32,this,actor);
 s32 angle=gabi::call<s32>(0x0200F93C,&actor->current.pos,&current.pos);
  s16 delta=(s16)((s16)actor->shape_angle.y-angle);
 return (u32)((s32)delta+0x1FFF)<0x3FFF;
}
VERIFY(0x0238F360,&daObjShmrgrd_c::check_player_angle);
static s32 prm(daObjShmrgrd_c* a,s32 width,s32 shift) { WWHD_FUNC(0x02390008,s32,a,width,shift);
  u32 w=(u32)width&63, sh=(u32)shift&63;
  u32 mask=(w<32?1u<<w:0)-1;
  return (sh<32?(u32)a->mParameters>>sh:0)&mask;
  }
VERIFY(0x02390008,prm);
void daObjShmrgrd_c::mode_upper() {
 WWHD_FUNC(0x0238FCE0,void,this);
 s32 damage=gabi::load<s32>(damage_address);
 if(damage==1) { gabi::call(0x0238FC84,this);
  return;
  }
 s32 index=gabi::call<s32>(0x02390008,this,8,0);
 if(index!=255) { s8 room=home.roomNo;
  u32 save=gabi::load<u32>(0x101F84DC);
  if(gabi::call<s32>(0x025BA0C0,gabi::at<void>(save+0x20),index,room)!=0) { gabi::call(0x0238FC84,this);
  return;
  } damage=gabi::load<s32>(damage_address);
  }
 if(damage==2) { s16 dir=gabi::load<s16>(dir_address);
  gabi::call(0x0238FC50,this,dir,1.0f);
  sound(this,0x69B8);
  }
}
VERIFY(0x0238FCE0,&daObjShmrgrd_c::mode_upper);
void daObjShmrgrd_c::mode_u_l() {
 WWHD_FUNC(0x0238FDDC,void,this);
 if((f32)mCurHFrac>0)return;
 s32 index=gabi::call<s32>(0x02390008,this,8,0);
 if(index!=255) { s8 room=home.roomNo;
  u32 save=gabi::load<u32>(0x101F84DC);
  gabi::call(0x025B9E38,gabi::at<void>(save+0x20),index,room);
  }
 gabi::call(0x0238EE5C,this);
}
VERIFY(0x0238FDDC,&daObjShmrgrd_c::mode_u_l);
void daObjShmrgrd_c::init_mtx() {
 WWHD_FUNC(0x0238EE3C,void,this);
  u32 model=gabi::ea((J3DModel*)mpModel);
  f32 x=scale.x,y=scale.y,z=scale.z;
  gabi::store<f32>(model+0xBC,x);
  gabi::store<f32>(model+0xC0,y);
  gabi::store<f32>(model+0xC4,z);
  gabi::call(0x0238ED78,this);
}
VERIFY(0x0238EE3C,&daObjShmrgrd_c::init_mtx);
void daObjShmrgrd_c::set_mtx() {
 WWHD_FUNC(0x0238ED78,void,this);
 auto matrix=gabi::at<Mtx34>(0x1048D0CC);
  f32 x=current.pos.x,y=current.pos.y,z=current.pos.z;
 gabi::call(0x028E93CC,matrix,x,y,z);
  gabi::call(0x025F1B48,matrix,(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
 f32 values[12];
  for(u32 i=0;i<12;i++)values[i]=gabi::load<f32>(0x1048D0CC+i*4);
  u32 model=gabi::ea((J3DModel*)mpModel);
  for(u32 i=0;i<12;i++)gabi::store<f32>(model+0xC8+i*4,values[i]);
}
VERIFY(0x0238ED78,&daObjShmrgrd_c::set_mtx);
static void empty_lower(daObjShmrgrd_c* a) { WWHD_FUNC(0x0238FFF8,void,a);
  }
VERIFY(0x0238FFF8,empty_lower);
static void empty_callback(void* a) { WWHD_FUNC(0x0238FFFC,void,a);
  }
VERIFY(0x0238FFFC,empty_callback);
static s32 is_delete(daObjShmrgrd_c* a) { WWHD_FUNC(0x02390000,s32,a);
  return 1;
  }
VERIFY(0x02390000,is_delete);
static void global_destructor(void* a,s32 flags) { WWHD_FUNC(0x0238FF30,void,a,flags);
  if(a&&((u32)flags&1))gabi::call(0x0273AF40,a);
  }
VERIFY(0x0238FF30,global_destructor);
static void actor_destructor(daObjShmrgrd_c* a,s32 flags) {
 WWHD_FUNC(0x0238FF44,void,a,flags);
  if(!a)return;
 gabi::call(0x02515860,member(a,0x930),2);
  gabi::call(0x02515A70,member(a,0x800),2);
 gabi::call(0x02515860,member(a,0x7C4),2);
  gabi::call(0x02515A70,member(a,0x694),2);
 gabi::call(0x02515860,member(a,0x658),2);
  gabi::call(0x02515A70,member(a,0x528),2);
 gabi::call(0x02515860,member(a,0x4EC),2);
  gabi::call(0x02515A70,member(a,0x3BC),2);
 gabi::call(0x025D50BC,a,0);
  if((u32)flags&1)gabi::call(0x0273AF40,a);
}
VERIFY(0x0238FF44,actor_destructor);
static void get_at_vector(cXyz* out,cXyz* other) {
 WWHD_FUNC(0x0238E84C,void,out,other);
 u32 play=gabi::call<u32>(0x025200D4), player=gabi::load<u32>(play+0x5B2C);
  gabi::Local<cXyz> dir;
 gabi::call(0x0201ADE0,gabi::at<cXyz>(player+0x314),dir.get(),other);
 out->x=gabi::load<f32>(0x101FFBA8);
  out->y=gabi::load<f32>(0x101FFBAC);
  out->z=gabi::load<f32>(0x101FFBB0);
 if(gabi::call<s32>(0x0201B47C,dir.get())!=0) { f32 y=dir->y,x=dir->x,z=dir->z;
  out->x=x;
  out->z=z;
  out->y=y;
  }
}
VERIFY(0x0238E84C,get_at_vector);
static void set_damage(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238F3B4,void,a);
 u32 play=gabi::call<u32>(0x025200D4);
  u32 player=gabi::load<u32>(play+0x5B2C);
  u8 cut=gabi::load<u8>(player+0x3AC);
 gabi::store<s32>(damage_address,0);
  gabi::store<s16>(dir_address,0);
 gabi::call(0x02515E50,member(a,0x674));
 if(!gabi::call<s32>(0x025162A4,member(a,0x528)))return;
 u32 hit=gabi::call<u32>(0x02516300,member(a,0x528));
  auto actor=gabi::call<fopAc_ac_c*>(0x02515BBC,member(a,0x5BC));
 if((gabi::load<u32>(hit+0x10)&0x10000)&&actor&&gabi::load<s16>(gabi::ea(actor)+0xE)==0xA8) {
  s32 front=gabi::call<s32>(0x0238F360,a,actor);
  if(front&&(cut==0x12||cut==0x13))gabi::store<s32>(damage_address,1);
  else if(cut==0x11) { f32 x=gabi::load<f32>(gabi::ea(a)+0x5E8),z=gabi::load<f32>(gabi::ea(a)+0x5F0);
  s16 direction=gabi::call<s16>(0x020195B0,x,z);
  gabi::store<s16>(dir_address,direction);
  gabi::store<s32>(damage_address,2);
  }
 }
 gabi::call(0x023129C4,&a->eyePos,(s8)a->current.roomNo,member(a,0x528),0x11);
 gabi::call(0x02312E54,a,member(a,0x528));
  gabi::call(0x0251621C,member(a,0x528));
}
VERIFY(0x0238F3B4,set_damage);
static daObjShmrgrd_c* search_gap(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238F7F0,daObjShmrgrd_c*,a);
 auto node=gabi::at<daObjShmrgrd_c>(gabi::load<u32>(list_address));
  gabi::Local<cXyz> delta,flat,vec;
 while(node) {
  gabi::call(0x0201ADE0,&node->current.pos,delta.get(),&a->current.pos);
  s32 mode=node->mMode;
  f32 x=delta->x,z=delta->z;
  if(mode==0) { flat->x=x;
  flat->y=0;
  flat->z=z;
  f32 square=gabi::call<f32>(0x028E8DD0,flat.get());
  f32 mag=gabi::call<f32>(0x028F4384,square);
   if(!(mag>165)) { gabi::call(0x025F1884,gabi::at<Mtx34>(0x1048D0CC),(s16)a->shape_angle.y);
  gabi::call(0x028E8F64,gabi::at<Mtx34>(0x1048D0CC),gabi::at<cXyz>(0x101FFBCC),vec.get());
    f32 dot=gabi::fmadds((f32)vec->x,x,(f32)vec->z*z);
  if(dot>0)return node;
   }
  }
  node=(daObjShmrgrd_c*)node->mpNext;
 }
 return nullptr;
}
VERIFY(0x0238F7F0,search_gap);
static void set_gap_co(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238F928,void,a);
 auto gap=gabi::call<daObjShmrgrd_c*>(0x0238F7F0,a);
  if(!gap)return;
 gabi::Local<cXyz> delta,pos,at;
 gabi::call(0x0201ADE0,&gap->current.pos,delta.get(),&a->current.pos);
  pos->y=delta->y;
  pos->x=delta->x;
  pos->z=delta->z;
 gabi::call(0x028E8E64,pos.get(),pos.get(),0.5f);
  gabi::call(0x028E8D88,pos.get(),&a->current.pos,pos.get());
 gabi::call(0x020182E0,member(a,0x918),pos.get());
  gabi::call(0x0238E84C,at.get(),pos.get());
 f32 y=at->y,x=at->x;
  gabi::store<f32>(gabi::ea(a)+0x880,y);
  f32 z=at->z;
  gabi::store<f32>(gabi::ea(a)+0x87C,x);
  gabi::store<f32>(gabi::ea(a)+0x884,z);
 u32 play=gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),member(a,0x800));
}
VERIFY(0x0238F928,set_gap_co);
static daObjShmrgrd_c* actor_constructor(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238E8DC,daObjShmrgrd_c*,a);
 if(!a) { a=gabi::call<daObjShmrgrd_c*>(0x0273AD10,0x9F0);
  if(!a)return nullptr;
  }
 fopAc_ac_c_ct(a);
  a->__vtbl=0x1002F458;
 for(u32 i=0;i<4;i++) {
  u32 off=0x3BC+i*0x16C;
  gabi::call(0x02515FB8,member(a,off));
  gabi::store<u32>(gabi::ea(a)+off+0x114,0x100015A8);
  gabi::store<u32>(gabi::ea(a)+off+0x110,0x1002F448);
  gabi::call(0x02018590,member(a,off+0x118));
  if(i==1||i==2) { gabi::store<u32>(gabi::ea(a)+off+0x12C,0x1004B150);
  gabi::store<u32>(gabi::ea(a)+off+0x114,0x1004B160);
  gabi::store<u32>(gabi::ea(a)+off+0x3C,0x1004B108);
  }
  else { gabi::store<u32>(gabi::ea(a)+off+0x114,0x1004B160);
  gabi::store<u32>(gabi::ea(a)+off+0x3C,0x1004B108);
  gabi::store<u32>(gabi::ea(a)+off+0x12C,0x1004B150);
  }
  gabi::call(0x0200BD2C,member(a,off+0x130));
  gabi::call(0x02515DA0,member(a,off+0x14C));
  gabi::store<u32>(gabi::ea(a)+off+0x148,0x1004AE88);
  gabi::store<u32>(gabi::ea(a)+off+0x14C,0x1004AEC0);
 }
 gabi::call(0x025A5B18,member(a,0x99C),1);
 gabi::store<u8>(gabi::ea(a)+0x9B2,0xA0);
  gabi::store<u8>(gabi::ea(a)+0x9B4,0x80);
  gabi::store<u8>(gabi::ea(a)+0x9B3,0xA0);
  a->mpNext=nullptr;
  gabi::store<u8>(gabi::ea(a)+0x9B5,0xFF);
 return a;
}
VERIFY(0x0238E8DC,actor_constructor);
static s32 create_heap(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238EC78,s32,a);
  gabi::Local<be<u32>[2]> request1,request2;
 (*request1)[0]=0x1002F510;
  (*request1)[1]=0x1002F430;
 u32 data=gabi::call<u32>(0x026066C4,gabi::at<void>(gabi::load<u32>(0x101F4F28)),request1.get(),4);
 if(!data) { gabi::call(0x0273AA24,gabi::at<void>(0x1002F478),0x21A,gabi::at<void>(0x1002F48C));
  return 0;
  }
 auto model=gabi::call<J3DModel*>(0x025E38E0,gabi::at<void>(data),0,0x11020203);
  a->mpModel=model;
  if(!model)return 0;
 u32 count=gabi::load<u32>(data+4),joint=gabi::load<u32>(data+8);
  if(count>2)joint+=0x38;
  gabi::store<u32>(joint+8,0x0238EA88);
 u32 loaded=gabi::ea((J3DModel*)a->mpModel);
  gabi::store<u32>(loaded+0xB8,gabi::ea(a));
 (*request2)[0]=0x1002F510;
  (*request2)[1]=0x1002F430;
 u32 bg=gabi::call<u32>(0x026066C4,gabi::at<void>(gabi::load<u32>(0x101F4F28)),request2.get(),7);
 auto world=gabi::call<dBgW*>(0x024F2478,gabi::at<void>(bg),1,&a->mMtx);
  a->mpBgW=world;
  return world!=nullptr;
}
VERIFY(0x0238EC78,create_heap);
static s32 heap_callback(daObjShmrgrd_c* a) { WWHD_FUNC(0x0238ED74,s32,a);
  return gabi::call<s32>(0x0238EC78,a);
  }
VERIFY(0x0238ED74,heap_callback);
static bool delete_actor(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238F2CC,bool,a);
 if(a->heap) { auto bg=(dBgW*)a->mpBgW;
  if(bg&&gabi::load<u32>(gabi::ea(bg))<0x100) { u32 play=gabi::call<u32>(0x025200D4);
  gabi::call(0x020087EC,gabi::at<void>(play+0x12A0),(dBgW*)a->mpBgW);
  a->mpBgW=nullptr;
  } }
 gabi::call(0x0238F254,a);
  u32 vtable=gabi::load<u32>(gabi::ea(a)+0x99C);
  gabi::call(gabi::load<u32>(vtable+0x44),member(a,0x99C));
 gabi::call(0x025204C8,&a->mPhase,gabi::at<void>(0x1002F510));
  return true;
}
VERIFY(0x0238F2CC,delete_actor);
static s32 draw_actor(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238FBB8,s32,a);
 u32 light=gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4,gabi::at<void>(light),0,&a->current.pos,&a->tevStr);
 light=gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C,gabi::at<void>(light),(J3DModel*)a->mpModel,&a->tevStr);
 u32 play=gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D70));
 play=gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D74));
  gabi::call(0x025E2DE0,(J3DModel*)a->mpModel,0);
 play=gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D78));
 play=gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D7C));
  return 1;
}
VERIFY(0x0238FBB8,draw_actor);
static s32 create_actor(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238EF8C,s32,a);
 u32 condition=a->actor_condition;
  if(!(condition&8)) { if(a) { gabi::call(0x0238E8DC,a);
  condition=a->actor_condition;
  } a->actor_condition=condition|8;
  }
 s32 phase=gabi::call<s32>(0x02520460,&a->mPhase,gabi::at<void>(0x1002F510));
  if(phase!=4)return phase;
 if(!gabi::call<s32>(0x025D63E8,a,gabi::at<void>(0x0238ED74),0x12A0))return 5;
 u32 model=gabi::ea((J3DModel*)a->mpModel);
  a->cullMtx=model?model+0xC8:0;
  gabi::call(0x0238EE3C,a);
 gabi::call(0x025D674C,a,-90.0f,0.0f,-90.0f,90.0f,120.0f,90.0f);
 gabi::call(0x02515F14,member(a,0x4EC),255,255,a);
  gabi::call(0x02516518,member(a,0x3BC),gabi::at<void>(0x101CCBB0));
  gabi::store<u32>(gabi::ea(a)+0x400,gabi::ea(a)+0x4EC);
 gabi::call(0x02515F14,member(a,0x658),255,255,a);
  gabi::call(0x02516518,member(a,0x528),gabi::at<void>(0x101CCBF4));
 u32 flags=gabi::load<u32>(gabi::ea(a)+0x5BC);
  gabi::store<u32>(gabi::ea(a)+0x56C,gabi::ea(a)+0x658);
  gabi::store<u32>(gabi::ea(a)+0x5BC,flags|4);
 gabi::call(0x02515F14,member(a,0x7C4),255,255,a);
  gabi::call(0x02516518,member(a,0x694),gabi::at<void>(0x1002F518));
  gabi::store<u32>(gabi::ea(a)+0x6D8,gabi::ea(a)+0x7C4);
 gabi::call(0x02515F14,member(a,0x930),255,255,a);
  gabi::call(0x02516518,member(a,0x800),gabi::at<void>(0x1002F55C));
 flags=gabi::load<u32>(gabi::ea(a)+0x39C);
  a->mVSpeed=0;
  a->mAngleZ=0;
  a->mAngleX=0;
  a->mScaleY=1;
  a->mUnused=0;
  a->mCrushState=0;
  a->mAngleSpeedX=0;
  a->mAngleSpeedZ=0;
 gabi::store<u8>(gabi::ea(a)+0x388,0x2D);
  gabi::store<u32>(gabi::ea(a)+0x844,gabi::ea(a)+0x930);
  gabi::store<u32>(gabi::ea(a)+0x39C,flags|1);
  a->mCrushTimer=0;
  a->mTargetHFrac=0;
 s32 sw=gabi::call<s32>(0x02390008,a,8,0);
  bool set=false;
 if(sw!=255) { s8 room=a->home.roomNo;
  u32 save=gabi::load<u32>(0x101F84DC);
  set=gabi::call<s32>(0x025BA0C0,gabi::at<void>(save+0x20),sw,room)!=0;
  }
 if(set) { a->mCurHFrac=0;
  a->mTopPos=-85;
  gabi::call(0x0238EE5C,a);
  } else { a->mCurHFrac=1;
  a->mTopPos=0;
  gabi::call(0x0238EE80,a);
  }
 gabi::call(0x0238EF28,a);
  model=gabi::ea((J3DModel*)a->mpModel);
  gabi::call(0x028E90D4,gabi::at<Mtx34>(model?model+0xC8:0),&a->mMtx);
 u32 play=gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C,gabi::at<void>(play+0x12A0),(dBgW*)a->mpBgW,a);
  return 4;
}
VERIFY(0x0238EF8C,create_actor);
static s32 execute_actor(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238F9F4,s32,a);
 gabi::call(0x0238F3B4,a);
  if(gabi::load<s32>(damage_address)==1) { gabi::call(0x0238F4F0,a);
  gabi::call(0x0238F4FC,a);
  }
 u32 entry=0x1002F4E4+(u32)a->mMode*8;
  s16 index=gabi::load<s16>(entry+2),adjust=gabi::load<s16>(entry);
 u32 actor=gabi::ea(a)+(s32)adjust,target;
 if(index<0)target=gabi::load<u32>(entry+4);
 else { s16 offset=gabi::load<s16>(entry+6);
  u32 table=gabi::load<u32>(actor+(s32)offset);
  target=gabi::load<u32>(table+(u32)(u16)index*8+4);
  }
 gabi::call(target,gabi::at<daObjShmrgrd_c>(actor));
  gabi::call(0x0238F6D8,a);
  gabi::call(0x0238F734,a);
  gabi::call(0x0238F790,a);
  gabi::call(0x0238ED78,a);
 if(a->mMode==0) {
  gabi::call(0x020182E0,member(a,0x640),&a->current.pos);
  u32 play=gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),member(a,0x528));
  gabi::call(0x020182E0,member(a,0x4D4),&a->current.pos);
  play=gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),member(a,0x3BC));
  gabi::Local<cXyz> at;
  gabi::call(0x0238E84C,at.get(),&a->current.pos);
  f32 y=at->y,z=at->z,x=at->x;
  gabi::store<f32>(gabi::ea(a)+0x714,y);
  gabi::store<f32>(gabi::ea(a)+0x718,z);
  gabi::store<f32>(gabi::ea(a)+0x710,x);
  gabi::call(0x020182E0,member(a,0x7AC),&a->current.pos);
  play=gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),member(a,0x694));
  gabi::call(0x0238F928,a);
 }
 f32 height=(f32)a->current.pos.y+(f32)a->mTopPos;
  gabi::store<f32>(gabi::ea(a)+0x394,height+125);
  a->eyePos.y=height+75;
 if(a->heap) { auto bg=(dBgW*)a->mpBgW;
  if(bg&&gabi::load<u32>(gabi::ea(bg))<0x100)gabi::call(0x024F43DC,bg);
  } return 1;
}
VERIFY(0x0238F9F4,execute_actor);
static s32 create_method(daObjShmrgrd_c* a) { WWHD_FUNC(0x0238FE54,s32,a);
  return gabi::call<s32>(0x0238EF8C,a);
  }
VERIFY(0x0238FE54,create_method);
static bool delete_method(daObjShmrgrd_c* a) { WWHD_FUNC(0x0238FE58,bool,a);
  return gabi::call<bool>(0x0238F2CC,a);
  }
VERIFY(0x0238FE58,delete_method);
static s32 execute_method(daObjShmrgrd_c* a) { WWHD_FUNC(0x0238FE5C,s32,a);
  return gabi::call<s32>(0x0238F9F4,a);
  }
VERIFY(0x0238FE5C,execute_method);
static s32 draw_method(daObjShmrgrd_c* a) { WWHD_FUNC(0x0238FE60,s32,a);
  return gabi::call<s32>(0x0238FBB8,a);
  }
VERIFY(0x0238FE60,draw_method);
static void effects_crush(daObjShmrgrd_c* a) {
 WWHD_FUNC(0x0238F4FC,void,a);
 if(a->mMode==0) {
  u32 play=gabi::call<u32>(0x025200D4);
  u32 manager=gabi::load<u32>(play+0x5AB0);
  gabi::call(0x025A847C,gabi::at<void>(manager),0,0x81B7,&a->current.pos,0,0,255,0,-1,0,0,0);
  play=gabi::call<u32>(0x025200D4);
  manager=gabi::load<u32>(play+0x5AB0);
  gabi::call(0x025A847C,gabi::at<void>(manager),0,0x81B8,&a->current.pos,0,0,255,0,-1,0,0,0);
 }
 if(!gabi::load<u32>(0x1046BEA4)) { gabi::store<f32>(0x1046BE8C,1.5f);
  gabi::store<f32>(0x1046BE94,1);
  gabi::store<f32>(0x1046BE90,1.5f);
  gabi::store<u32>(0x1046BEA4,1);
  }
 u32 play=gabi::call<u32>(0x025200D4),manager=gabi::load<u32>(play+0x5AB0);
 u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(manager),2,0x2027,&a->current.pos,0,0,200,member(a,0x99C),-1,0,0,gabi::at<cXyz>(0x1046BE8C));
 if(!emitter)return;
 gabi::store<u32>(emitter+0x5C,1);
  gabi::store<u16>(emitter+0x60,45);
  gabi::store<f32>(emitter+0x34,30);
  gabi::store<f32>(emitter+0x68,0);
  gabi::store<f32>(emitter+0x6C,38);
 if(!gabi::load<u32>(0x1046BEA8)) { gabi::store<u32>(0x1046BEA8,1);
  gabi::store<f32>(0x1046BEA0,0);
  gabi::store<f32>(0x1046BE9C,15);
  gabi::store<f32>(0x1046BE98,0);
  }
 gabi::store<f32>(emitter+0x14,gabi::load<f32>(0x1046BE98));
  gabi::store<f32>(emitter+0x18,gabi::load<f32>(0x1046BE9C));
  gabi::store<f32>(emitter+0x1C,gabi::load<f32>(0x1046BEA0));
}
VERIFY(0x0238F4FC,effects_crush);
static s32 joint_callback(void* joint,s32 phase) {
 WWHD_FUNC(0x0238EA88,s32,joint,phase);
  if(phase!=0)return 1;
 u32 model=gabi::load<u32>(0x104B462C),actor=gabi::load<u32>(model+0xB8);
 u32 node=gabi::call<u32>(0x027F7878,joint);
  u16 index=gabi::load<u16>(node+4);
  u32 matrices=gabi::load<u32>(model+0x2C);
 u32 base=gabi::load<u32>(matrices+0x10);
  u16 flags=gabi::load<u16>(matrices+4);
  gabi::store<u16>(matrices+4,flags|0x10);
 gabi::call(0x028E90D4,gabi::at<Mtx34>(base+(u32)index*0x30),gabi::at<Mtx34>(0x1048D0CC));
 f32 scale=gabi::load<f32>(actor+0x978);
  gabi::call(0x025F24E0,0.0f,20.0f*(1.0f-scale),0.0f);
 gabi::call(0x025F2518,1.0f,gabi::load<f32>(actor+0x978),1.0f);
  gabi::call(0x025F24E0,0.0f,gabi::load<f32>(actor+0x998),0.0f);
 gabi::Local<cXyz> axis;
  gabi::Local<be<f32>[4]> quat;
 axis->y=1;
  axis->x=gabi::load<f32>(actor+0x980);
  axis->z=gabi::load<f32>(actor+0x97C);
 gabi::call(0x02312548,quat.get(),axis.get());
  gabi::call(0x025F24E0,0.0f,-20.0f,0.0f);
  gabi::call(0x025F25CC,quat.get());
  gabi::call(0x025F24E0,0.0f,20.0f,0.0f);
 matrices=gabi::load<u32>(model+0x2C);
  flags=gabi::load<u16>(matrices+4);
  base=gabi::load<u32>(matrices+0x10);
  gabi::store<u16>(matrices+4,flags|0x10);
 f32 values[12];
  for(u32 i=0;i<12;i++)values[i]=gabi::load<f32>(0x1048D0CC+i*4);
  for(u32 i=0;i<12;i++)gabi::store<f32>(base+(u32)index*0x30+i*4,values[i]);
 gabi::call(0x028E90D4,gabi::at<Mtx34>(0x1048D0CC),gabi::at<Mtx34>(0x104B4868));
  return 1;
}
VERIFY(0x0238EA88,joint_callback);
static void initialize_globals() {
 WWHD_FUNC(0x0238FE64,void);
 gabi::store<u32>(0x1046BE88,0);
  gabi::store<u32>(0x1046BE84,0);
  gabi::store<u32>(0x1046BE80,0);
  gabi::store<u32>(0x1046BE7C,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101CCB30));
  gabi::store<f32>(0x1046BE70,-3.1415927410125732f);
  gabi::store<f32>(0x1046BE74,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<void>(0x1046BE78));
  gabi::call(0x028F026C,gabi::at<void>(0x101CCB3C));
  gabi::call(0x028EAB2C,gabi::at<void>(0x1046BE79));
  gabi::call(0x028F026C,gabi::at<void>(0x101CCB48));
 gabi::store<f32>(0x101CCC30,50);
  gabi::store<f32>(0x101CCC34,112.5f);
  gabi::store<f32>(0x101CCBEC,79);
  gabi::store<f32>(0x101CCBF0,112.5f);
}
VERIFY(0x0238FE64,initialize_globals);
