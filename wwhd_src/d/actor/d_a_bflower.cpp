// Bomb flower.
#include "d/actor/d_a_bflower.h"
namespace {
static u8* ptr(u32 a) { return gabi::at<u8>(a); }
template<class T> static T read(u32 a) { return gabi::load<T>(a); }
template<class T> static void write(u32 a,T v) { gabi::store<T>(a,v); }
static u32 play() {
 // Reserve the guest linkage area before a non-leaf callee saves LR.
 gabi::Local<be<u32>[4]> linkage;
 return gabi::call<u32>(0x025200D4);
}
static u32 env() { return gabi::call<u32>(0x02555D0C); }
static u32 resource(s32 index) {
 gabi::Local<be<u32>[2]> name;
 (*name)[0]=0x10008848; (*name)[1]=0x10008764;
 u32 control=read<u32>(0x101F4F28);
 // Keep the SafeString above the callee's linkage words.
 gabi::Local<be<u32>[4]> linkage;
 return gabi::call<u32>(0x026066C4,ptr(control),name.get(),index);
}
static u32 checkedResource(s32 index,s32 line,u32 expression) {
 u32 p=resource(index);
 if(!p) gabi::call(0x0273AA24,ptr(0x100087D4),line,ptr(expression));
 return p;
}
static s32 initBck(u32 a,u32 model,u32 anim,s32 mode,s32 modify) {
 return gabi::call<s32>(0x025E8508,ptr(a),ptr(model),ptr(anim),1,mode,1.0f,0,-1,modify);
}
static s32 initBrk(u32 a,u32 model,u32 anim) {
 return gabi::call<s32>(0x025E8154,ptr(a),ptr(model),ptr(anim),1,0,1.0f,0,-1,0,0);
}
static void translate(daBFlower_c* p) {
 f32 x=p->current.pos.x,y=p->current.pos.y,z=p->current.pos.z;
 gabi::call(0x028E93CC,ptr(0x1048D0CC),x,y,z);
}
static void rotate(daBFlower_c* p) {
 s16 x=p->current.angle.x,y=p->current.angle.y,z=p->current.angle.z;
 gabi::call(0x025F1B48,ptr(0x1048D0CC),x,y,z);
}
static void copyMatrix(u32 model) {
 f32 matrix[12];
 for(int i=0;i<12;++i) matrix[i]=read<f32>(0x1048D0CC+i*4);
 for(int i=0;i<12;++i) write<f32>(model+0xC8+i*4,matrix[i]);
}
static f32 distanceXZ(u32 other,daBFlower_c* p) {
 gabi::Local<cXyz> delta,flat;
 gabi::call(0x0201ADE0,ptr(other+0x314),delta.get(),&p->current.pos);
 f32 x=delta->x,z=delta->z;
 flat->x=x; flat->y=0.0f; flat->z=z;
 f32 square=gabi::call<f32>(0x028E8DD0,flat.get());
 return gabi::call<f32>(0x028F4384,square);
}
static u32 grabId(u32 player) {
 u32 vtable=read<u32>(player+0xB4),target=read<u32>(vtable+0xBC);
 return gabi::call<u32>(target,ptr(player));
}
static void sound(daBFlower_c* p,s32 id) {
 s8 room=p->current.roomNo;
 s32 reverb=gabi::call<s32>(0x02520540,room);
 gabi::call(0x025E1A40,id,&p->eyePos,0,reverb);
}
static u32 createBomb(daBFlower_c* p,u32 parameter) {
 s8 room=p->current.roomNo;
 return gabi::call<u32>(0x025D5928,0x127,parameter,&p->current.pos,room,&p->current.angle,ptr(0),-1,0,0);
}
}
BOOL daBFlower_c::CreateHeap() {
 WWHD_FUNC(0x02076850,BOOL,this);
 u32 a=gabi::ea(this),parameters=mParameters;
 mSwitchNo=(parameters>>8)&255; mState=(parameters>>4)&15;
 s8 room=home.roomNo; s32 sw=mSwitchNo;
 u32 save=read<u32>(0x101F84DC);
 if(gabi::call<s32>(0x025BA0C0,ptr(save+0x20),sw,room)) mState=0;
 u32 model=checkedResource(11,434,0x100087FC);
 mpModel=ptr(gabi::call<u32>(0x025E38E0,ptr(model),0x80000,0x11000022));
 if(!mpModel) return FALSE;
 u32 anim=checkedResource(5,451,0x100087E4);
 if(!initBck(a+0x650,model,anim,1,0)) return FALSE;
 write<f32>(a+0x654,(f32)read<s16>(a+0x65A));
 anim=checkedResource(15,473,0x100087F0);
 if(!initBrk(a+0x6DC,model,anim)) return FALSE;
 write<f32>(a+0x6DC,0.0f);
 model=checkedResource(12,487,0x100087FC);
 mpModel2=ptr(gabi::call<u32>(0x025E38E0,ptr(model),0x80000,0x11000022));
 if(!mpModel2) return FALSE;
 anim=checkedResource(8,503,0x100087E4);
 if(!initBck(a+0x75C,model,anim,0,0)) return FALSE;
 write<f32>(a+0x75C,0.0f);
 anim=checkedResource(17,520,0x100087F0);
 if(!initBrk(a+0x7E8,model,anim)) return FALSE;
 write<f32>(a+0x7E8,0.0f);
 if(mState==0) {
  f32 b=(f32)read<s16>(a+0x6E6),c=(f32)read<s16>(a+0x7F2),d=(f32)read<s16>(a+0x766);
  write<f32>(a+0x7EC,c); write<f32>(a+0x6E0,b); write<f32>(a+0x760,d);
 } else if(mState==1) {
  write<f32>(a+0x760,0.0f);write<f32>(a+0x7EC,0.0f);write<f32>(a+0x6E0,0.0f);
 }
 return TRUE;
}
VERIFY(0x02076850,&daBFlower_c::CreateHeap);
void daBFlower_c::set_mtx() {
 WWHD_FUNC(0x02076C34,void,this);
 f32 x=scale.x,y=scale.y,z=scale.z;u32 model=gabi::ea((u8*)mpModel);
 write<f32>(model+0xBC,x);write<f32>(model+0xC0,y);write<f32>(model+0xC4,z);
 translate(this);rotate(this);copyMatrix(gabi::ea((u8*)mpModel));
 x=mBombScale.x;y=mBombScale.y;z=mBombScale.z;model=gabi::ea((u8*)mpModel2);
 write<f32>(model+0xBC,x);write<f32>(model+0xC4,z);write<f32>(model+0xC0,y);
 copyMatrix(gabi::ea((u8*)mpModel2));
}
VERIFY(0x02076C34,&daBFlower_c::set_mtx);
void daBFlower_c::CreateInit() {
 WWHD_FUNC(0x02076D94,void,this);
 u32 a=gabi::ea(this),model=gabi::ea((u8*)mpModel);cullMtx=model?model+0xC8:0;
 gabi::call(0x025D674C,this,-50.0f,0.0f,-50.0f,50.0f,100.0f,50.0f);
 gabi::call(0x02515F14,ptr(a+0x3B8),255,255,this);
 gabi::call(0x02516518,ptr(a+0x3F4),ptr(0x10190D88));
 u8 state=mState;write<u32>(a+0x438,a+0x3B8);
 gabi::call(0x0251677C,ptr(a+0x524),ptr(state?0x10190D18:0x10190CD8));
 current.angle.x=home.angle.x;current.angle.y=home.angle.y;
 write<u32>(a+0x568,a+0x3B8);current.angle.z=home.angle.z;
 set_mtx();eyePos.x=0.0f;eyePos.y=50.0f;eyePos.z=0.0f;
 translate(this);rotate(this);gabi::call(0x028E8F64,ptr(0x1048D0CC),&eyePos,&eyePos);
 u32 x=read<u32>(a+0x37C),y=read<u32>(a+0x380),z=read<u32>(a+0x384);
 write<u32>(a+0x390,x);write<u32>(a+0x394,y);write<u32>(a+0x398,z);
 mGrabbable=current.angle.x!=0;mpBombActor=nullptr;mAvailable=1;mGrowing=0;mBombScale.copy(scale);
 f32 r=gabi::call<f32>(0x020198D8,150.0f);mAnimation=-1;mAnimTimer=gabi::ftoi(r+60.0f);
}
VERIFY(0x02076D94,&daBFlower_c::CreateInit);
s32 daBFlower_c::init_bck_anm(s16 index) {
 WWHD_FUNC(0x02077530,s32,this,index);
 if(mAnimation!=-1) return 1;
 mAnimation=index;u32 model=resource(11),anim=resource(index);
 return initBck(gabi::ea(this)+0x650,model,anim,1,1);
}
VERIFY(0x02077530,&daBFlower_c::init_bck_anm);
void daBFlower_c::animPlay() {
 WWHD_FUNC(0x02077280,void,this);
 u32 a=gabi::ea(this);
 if(gabi::call<s32>(0x025E742C,ptr(a+0x650))) mAnimation=-1;
 gabi::call(0x025E742C,ptr(a+0x6DC));
}
VERIFY(0x02077280,&daBFlower_c::animPlay);
void daBFlower_c::setCollision() {
 WWHD_FUNC(0x020772C8,void,this);u32 a=gabi::ea(this);
 if(mState==1) {
  gabi::call(0x020182E0,ptr(a+0x50C),&current.pos);
  u32 p=play();gabi::call(0x0200E240,ptr(p+0x26A4),ptr(a+0x3F4));
  gabi::call(0x02018D40,ptr(a+0x63C),&current.pos);
  p=play();gabi::call(0x0200E240,ptr(p+0x26A4),ptr(a+0x524));
 } else if(mAvailable) {
  gabi::Local<cXyz> center;center->x=0.0f;center->y=30.0f;center->z=0.0f;
  translate(this);rotate(this);gabi::call(0x028E8F64,ptr(0x1048D0CC),center.get(),center.get());
  gabi::call(0x02018D40,ptr(a+0x63C),center.get());
  u32 p=play();gabi::call(0x0200E240,ptr(p+0x26A4),ptr(a+0x524));
 }
}
VERIFY(0x020772C8,&daBFlower_c::setCollision);
bool daBFlower_c::_draw() {
 WWHD_FUNC(0x0207717C,bool,this);u32 a=gabi::ea(this),e=env();
 gabi::call(0x025626A4,ptr(e),0,&current.pos,&tevStr);
 e=env();u32 model=gabi::ea((u8*)mpModel);gabi::call(0x02562F5C,ptr(e),ptr(model),&tevStr);
 model=gabi::ea((u8*)mpModel);u32 data=read<u32>(model+0xAC);f32 frame=read<f32>(a+0x654);
 gabi::call(0x025E86B8,ptr(a+0x650),ptr(data),frame);
 model=gabi::ea((u8*)mpModel);data=read<u32>(model+0xAC);frame=read<f32>(a+0x6E0);
 gabi::call(0x025E83FC,ptr(a+0x6DC),ptr(data),frame);
 gabi::call(0x025E2DE0,(u8*)mpModel,0);
 model=gabi::ea((u8*)mpModel);data=read<u32>(model+0xAC);write<u32>(read<u32>(data+8)+0x14,0);
 if(mAvailable) {
  e=env();model=gabi::ea((u8*)mpModel2);gabi::call(0x02562F5C,ptr(e),ptr(model),&tevStr);
  model=gabi::ea((u8*)mpModel2);data=read<u32>(model+0xAC);frame=read<f32>(a+0x760);
  gabi::call(0x025E86B8,ptr(a+0x75C),ptr(data),frame);
  model=gabi::ea((u8*)mpModel2);data=read<u32>(model+0xAC);frame=read<f32>(a+0x7EC);
  gabi::call(0x025E83FC,ptr(a+0x7E8),ptr(data),frame);
  gabi::call(0x025E2DE0,(u8*)mpModel2,0);
  model=gabi::ea((u8*)mpModel2);data=read<u32>(model+0xAC);write<u32>(read<u32>(data+8)+0x14,0);
 }
 return true;
}
VERIFY(0x0207717C,&daBFlower_c::_draw);
bool daBFlower_c::_execute() {
 WWHD_FUNC(0x020773C4,bool,this);
 u32 p=play(),player=read<u32>(p+0x5B2C);
 u32 table=0x10190D58+(u8)mState*8;
 s16 adjust=read<s16>(table),virtualIndex=read<s16>(table+2);
 u32 receiver=gabi::ea(this)+(s32)adjust,target;
 if(virtualIndex<0) target=read<u32>(table+4);
 else {
  s16 slot=read<s16>(table+6);u32 vtable=read<u32>(receiver+(s32)slot);
  target=read<u32>(vtable+(s32)virtualIndex*8+4);
 }
 gabi::call(target,ptr(receiver));
 animPlay();set_mtx();setCollision();
 mPrevPlayerDist=distanceXZ(player,this);mPrevGrabActorID=grabId(player);
 return true;
}
VERIFY(0x020773C4,&daBFlower_c::_execute);
BOOL daBFlower_c::actLive() {
 WWHD_FUNC(0x020775F8,BOOL,this);
 u32 a=gabi::ea(this),p=play(),player=read<u32>(p+0x5B2C);
 f32 distance=distanceXZ(player,this);
 gabi::call<f32>(0x0200ECD4,&mBombScale.x,1.0f,0.05f,0.1f,0.05f);
 gabi::call<f32>(0x0200ECD4,&mBombScale.y,1.0f,0.05f,0.1f,0.05f);
 f32 growing=gabi::call<f32>(0x0200ECD4,&mBombScale.z,1.0f,0.05f,0.1f,0.05f);
 if(mGrowing && growing==0.0f) init_bck_anm(6);
 if(distance<50.0f) {
  f32 change=std::fabs((f32)mPrevPlayerDist-distance);
  if(change>2.0f) {
   f32 y=current.pos.y,py=read<f32>(player+0x318);
   if(std::fabs(py-y)<10.0f) init_bck_anm(6);
  }
 }
 if(distance<200.0f) {
  u32 grabbed=grabId(player),previous=mPrevGrabActorID;
  if(grabbed!=previous && grabId(player)==0xFFFFFFFF) {
   previous=mPrevGrabActorID;gabi::Local<be<u32>> id;*id=previous;
   u32 actor=0;
   if(previous!=0xFFFFFFFF) actor=gabi::call<u32>(0x025D5218,ptr(0x025E1234),id.get());
   if(actor && distanceXZ(actor,this)<70.0f) init_bck_anm(6);
  }
 }
 gabi::call(0x025E742C,ptr(a+0x7E8));
 s32 finished=gabi::call<s32>(0x025E742C,ptr(a+0x75C));
 s32 grabbable=mGrabbable;
 if(finished) mCarryReady=1;
 u32 bomb=gabi::ea((u8*)mpBombActor);
 if(grabbable==1) mCarryReady=0;
 u32 id=bomb?read<u32>(bomb+4):0xFFFFFFFF;
 if(gabi::call<s32>(0x025DE564,id)) {
  u32 flags=read<u32>(a+0x39C);mCarryReady=0;write<u32>(a+0x39C,flags&~0x10u);
 } else {
  if(!mAvailable) {
   mBombScale.z=0.0f;mBombScale.x=0.0f;mBombScale.y=0.0f;sound(this,0x6A0F);
  }
  u8 ready=mCarryReady;mAvailable=1;
  u32 flags=read<u32>(a+0x39C);write<u32>(a+0x39C,ready?flags|0x10:flags&~0x10u);
 }
 if(mAvailable && growing==0.0f) {
  if(gabi::call<s32>(0x025162A4,ptr(a+0x524))) {
   u32 hit=gabi::call<u32>(0x02516300,ptr(a+0x524));
   if(hit) {
    u32 type=read<u32>(hit+0x10);
    if(type&0x20) {mpBombActor=ptr(createBomb(this,0x100));mAvailable=0;}
    else if(type&0xFF1DFEFF) {
     u32 parameter=mGrabbable==1?0x101:1;
     mpBombActor=ptr(createBomb(this,parameter));mAvailable=0;
    }
   }
  }
  gabi::call(0x0251621C,ptr(a+0x524));
 }
 if((u32)actor_status&0x2000) {
  if(mCarryReady) {
   mAvailable=0;mpBombActor=ptr(createBomb(this,2));
   gabi::call(0x025D9D24,this);bomb=gabi::ea((u8*)mpBombActor);
   if(bomb) {
    gabi::call(0x025D9D0C,ptr(bomb),0);
    p=play();u32 link=read<u32>(p+0x5B34);bomb=gabi::ea((u8*)mpBombActor);
    gabi::call(0x023DE638,ptr(link+0x65A0),ptr(bomb));
   }
   init_bck_anm(6);
  }
 }
 mGrowing=mAvailable && growing>0.0f;
 return TRUE;
}
VERIFY(0x020775F8,&daBFlower_c::actLive);
BOOL daBFlower_c::actDead() {
 WWHD_FUNC(0x02077AE8,BOOL,this);
 u32 a=gabi::ea(this),p=play(),player=read<u32>(p+0x5B2C);
 f32 distance=distanceXZ(player,this);s32 timer=mAnimTimer;
 if(timer>0) {timer=(s32)((u32)timer-1);mAnimTimer=timer;}
 if(distance<50.0f && std::fabs((f32)mPrevPlayerDist-distance)>2.0f) {
  init_bck_anm(5);timer=mAnimTimer;
 }
 if(!timer) {
  f32 r=gabi::call<f32>(0x020198D8,100.0f);mAnimTimer=gabi::ftoi(r+100.0f);init_bck_anm(5);
 }
 write<f32>(a+0x6DC,0.0f);
 if(gabi::call<s32>(0x025162A4,ptr(a+0x3F4))) {
  u32 hit=gabi::call<u32>(0x02516300,ptr(a+0x3F4));
  if(hit && (read<u32>(hit+0x10)&0x100)) {
   sound(this,0x69EA);s8 room=home.roomNo;mState=0;mRecoveryTimer=70;
   u32 save=read<u32>(0x101F84DC);s32 sw=mSwitchNo;
   gabi::call(0x025B9E38,ptr(save+0x20),sw,room);init_bck_anm(6);
   write<f32>(a+0x7E8,1.0f);write<f32>(a+0x75C,1.0f);write<f32>(a+0x6DC,1.0f);
   gabi::call(0x0251677C,ptr(a+0x524),ptr(0x10190CD8));
  }
 }
 u32 flags=read<u32>(a+0x39C);mCarryReady=0;mAvailable=1;write<u32>(a+0x39C,flags&~0x10u);
 return TRUE;
}
VERIFY(0x02077AE8,&daBFlower_c::actDead);
s32 daBFlower_c::_create() {
 WWHD_FUNC(0x02076F80,s32,this);u32 a=gabi::ea(this);
 if(!((u32)actor_condition&8)) {
  if(a) {
   gabi::call(0x025D4ED0,this);__vtbl=0x100087B4;
   gabi::call(0x0200BD2C,ptr(a+0x3B8));gabi::call(0x02515DA0,ptr(a+0x3D4));
   write<u32>(a+0x3D0,0x1004AE88);write<u32>(a+0x3D4,0x1004AEC0);
   gabi::call(0x02515FB8,ptr(a+0x3F4));
   write<u32>(a+0x508,0x100015A8);write<u32>(a+0x504,0x1000877C);
   gabi::call(0x02018590,ptr(a+0x50C));
   write<u32>(a+0x430,0x1004B108);write<u32>(a+0x508,0x1004B160);write<u32>(a+0x520,0x1004B150);
   gabi::call(0x025166F0,ptr(a+0x524));gabi::call(0x027F2BC0,ptr(a+0x650),0);
   write<u32>(a+0x660,0x1016E54C);gabi::call(0x027DA984,ptr(a+0x664));
   write<u32>(a+0x6CC,0);write<u32>(a+0x660,0x1000878C);write<u32>(a+0x6D0,0);write<u32>(a+0x6A8,0);
   write<u32>(a+0x698,0x1016D820);write<u32>(a+0x6D8,0);write<u32>(a+0x6D4,0);
   gabi::call(0x025E80D0,ptr(a+0x6DC));gabi::call(0x027F2BC0,ptr(a+0x75C),0);
   write<u32>(a+0x76C,0x1016E54C);gabi::call(0x027DA984,ptr(a+0x770));
   write<u32>(a+0x7A4,0x1016D820);write<u32>(a+0x7B4,0);write<u32>(a+0x7E4,0);
   write<u32>(a+0x7D8,0);write<u32>(a+0x7E0,0);write<u32>(a+0x76C,0x1000878C);write<u32>(a+0x7DC,0);
   gabi::call(0x025E80D0,ptr(a+0x7E8));
  }
  actor_condition=(u32)actor_condition|8;
 }
 s32 phase=gabi::call<s32>(0x02520460,&mPhs,ptr(0x10008848));
 if(phase==4) {
  if(!gabi::call<s32>(0x025D63E8,this,ptr(0x02076C30),0xFC0)) return 5;
  CreateInit();
 }
 return phase;
}
VERIFY(0x02076F80,&daBFlower_c::_create);
static BOOL CheckCreateHeap(daBFlower_c* p) {WWHD_FUNC(0x02076C30,BOOL,p);return p->CreateHeap();}
VERIFY(0x02076C30,CheckCreateHeap);
static s32 daBFlower_Create(daBFlower_c* p) {WWHD_FUNC(0x02077148,s32,p);return p->_create();}
VERIFY(0x02077148,daBFlower_Create);
static BOOL daBFlower_Delete(daBFlower_c* p) {WWHD_FUNC(0x0207714C,BOOL,p);gabi::call(0x025204C8,&p->mPhs,ptr(0x10008848));return TRUE;}
VERIFY(0x0207714C,daBFlower_Delete);
static BOOL daBFlower_Draw(daBFlower_c* p) {WWHD_FUNC(0x0207727C,BOOL,p);return p->_draw();}
VERIFY(0x0207727C,daBFlower_Draw);
static BOOL daBFlower_Execute(daBFlower_c* p) {WWHD_FUNC(0x0207752C,BOOL,p);return p->_execute();}
VERIFY(0x0207752C,daBFlower_Execute);
static BOOL daBFlower_IsDelete(daBFlower_c* p) {WWHD_FUNC(0x02077D94,BOOL,p);return TRUE;}
VERIFY(0x02077D94,daBFlower_IsDelete);
static void bflower_sinit() {
 WWHD_FUNC(0x02077CD4,void);
 write<u32>(0x104619E8,0);write<u32>(0x104619E4,0);write<u32>(0x104619E0,0);write<u32>(0x104619DC,0);
 gabi::call(0x028F026C,ptr(0x10190DCC));
 write<f32>(0x104619D0,-3.1415927410125732f);write<f32>(0x104619D4,3.1415927410125732f);
 gabi::call(0x028ED6F8,ptr(0x104619D8));gabi::call(0x028F026C,ptr(0x10190DD8));
 gabi::call(0x028EAB2C,ptr(0x104619D9));gabi::call(0x028F026C,ptr(0x10190DE4));
 write<f32>(0x104619EC,0.0f);write<f32>(0x104619F0,0.0f);write<f32>(0x104619F4,0.0f);
}
VERIFY(0x02077CD4,bflower_sinit);
static void bflower_string_delete(u8* p,s32 flags) {WWHD_FUNC(0x02077D80,void,p,flags);if(p && (flags&1)) gabi::call(0x0273AF40,p);}
VERIFY(0x02077D80,bflower_string_delete);
static void bflower_delete(u8* p,s32 flags) {
 WWHD_FUNC(0x02077D9C,void,p,flags);u32 a=gabi::ea(p);
 if(a) {
  gabi::call(0x027F3628,ptr(a+0x76C),0);gabi::call(0x027F3628,ptr(a+0x660),0);
  gabi::call(0x02515AE8,ptr(a+0x524),2);gabi::call(0x02515A70,ptr(a+0x3F4),2);
  gabi::call(0x02515860,ptr(a+0x3B8),2);gabi::call(0x025D50BC,p,0);
  if(flags&1) gabi::call(0x0273AF40,p);
 }
}
VERIFY(0x02077D9C,bflower_delete);
static void bflower_string_terminate(u8* p) {WWHD_FUNC(0x02077E2C,void,p);}
VERIFY(0x02077E2C,bflower_string_terminate);
