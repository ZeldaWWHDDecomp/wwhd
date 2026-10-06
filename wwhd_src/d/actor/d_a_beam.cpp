#include "d/actor/d_a_beam.h"
namespace {
template<class T> T read(u32 a,u32 o=0) { return *gabi::at<be<T>>(a+o); }
template<class T> void write(u32 a,u32 o,T v) { *gabi::at<be<T>>(a+o)=v; }
void* ptr(u32 a,u32 o=0) {return gabi::at<void>(a+o);}
u32 ea(const void* p) {return gabi::ea(p);}
// Keep live guest locals above the EABI linkage and four outgoing stack words.
struct CallLinkage { u8 bytes[32]; };
template<class R=void,class... A> R beamCall(u32 address,A... args) {
 gabi::Local<CallLinkage> frame;
 return gabi::call<R>(address,args...);
}
void copyWords(u32 dst,u32 src,u32 size) {for(u32 i=0;i<size;i+=4) write<u32>(dst,i,read<u32>(src,i));}
f32 sinAngle(u16 a) {return read<f32>(0x104A44F8+(a>>3)*8);}
f32 cosAngle(u16 a) {return read<f32>(0x104A44FC+(a>>3)*8);}
void invalidateEmitter(u32 p) {u32 flags=read<u32>(p,0x254);write<u32>(p,0x5C,-1);write<u32>(p,0x254,flags|1);}
void searchProc(daBeam_c* b,u32 target) {b->searchAdjustment=0;b->searchSlot=-1;b->searchTarget=target;}
bool isSwitch(daBeam_c* b,s32 sw) {u32 save=read<u32>(0x101F84DC);s8 room=b->home.roomNo;return beamCall<s32>(0x025BA0C0,ptr(save,0x20),sw,room)!=0;}
void endpoint(daBeam_c* b,cXyz* out,f32 distance) {
  u16 pitch=b->current.angle.x,yaw=b->current.angle.y;
  f32 horizontal=distance*cosAngle(pitch);
  f32 px=b->current.pos.x,py=b->current.pos.y,pz=b->current.pos.z;
  // Preserve the additive NaN payload chosen by the recompiled fused expression.
  f32 x=std::isnan(px)?px:gabi::fmadds(horizontal,sinAngle(yaw),px);
  f32 z=std::isnan(pz)?pz:gabi::fmadds(horizontal,cosAngle(yaw),pz);
  out->set(x,
           gabi::fnmsubs(distance,sinAngle(pitch),py),
           z);
}
void fade(daBeam_c* b) {
  if(b->active==1) {
    f32 frame=b->beamFrame;if(frame<5.0f) b->beamFrame=frame+1.0f;
    f32 fade=b->fadeFrame;if(fade<4.0f) b->fadeFrame=fade+1.0f;
    else {b->beamFrame=0.0f;b->fadeFrame=0.0f;b->active=0;}
  } else {b->fadeFrame=0.0f;b->beamFrame=0.0f;}
}
}
BOOL daBeam_CreateHeap(daBeam_c* b) {
 WWHD_FUNC(0x02074A4C,BOOL,b);
 struct SafeString {be<u32> text,vtable;};
 gabi::Local<SafeString> name;name->text=0x10008758;name->vtable=0x1000859C;
 u32 manager=read<u32>(0x101F4F28);
 u32 data=beamCall<u32>(0x026066C4,ptr(manager),name.get(),10);
 if(!data) beamCall(0x0273AA24,ptr(0x100086B0),304,ptr(0x100086C0));
 b->model=beamCall<u32>(0x025E38E0,ptr(data),0,0x11020203);
 if(!b->model) beamCall(0x0273AA24,ptr(0x100086B0),306,ptr(0x100086D4));
 gabi::Local<SafeString> n2;n2->text=0x10008758;n2->vtable=0x1000859C;
 b->bck=beamCall<u32>(0x026066C4,ptr(read<u32>(0x101F4F28)),n2.get(),6);
 if(!b->bck) beamCall(0x0273AA24,ptr(0x100086B0),310,ptr(0x100086E4));
 gabi::Local<SafeString> n3;n3->text=0x10008758;n3->vtable=0x1000859C;
 b->brk=beamCall<u32>(0x026066C4,ptr(read<u32>(0x101F4F28)),n3.get(),14);
 if(!b->brk) beamCall(0x0273AA24,ptr(0x100086B0),314,ptr(0x100086F4));
 gabi::Local<SafeString> n4;n4->text=0x10008758;n4->vtable=0x1000859C;
 b->btk=beamCall<u32>(0x026066C4,ptr(read<u32>(0x101F4F28)),n4.get(),18);
 if(!b->brk) beamCall(0x0273AA24,ptr(0x100086B0),318,ptr(0x100086F4));
 s32 a=beamCall<s32>(0x025E8508,ptr(ea(b),0x68C),ptr(data),ptr(b->bck),0,2,1.0f,0,-1,0);
 s32 c=beamCall<s32>(0x025E8154,ptr(ea(b),0x720),ptr(data),ptr(b->brk),0,2,1.0f,0,-1,0,0);
 s32 d=beamCall<s32>(0x025E7CE0,ptr(ea(b),0x7A0),ptr(data),ptr(b->btk),1,2,1.0f,0,-1,0,0);
 return b->model && a && c && d;
}
VERIFY(0x02074A4C,daBeam_CreateHeap);
BOOL daBeam_CheckCreateHeap(daBeam_c* b){WWHD_FUNC(0x02074C94,BOOL,b);return beamCall<BOOL>(0x02074A4C,b);}
VERIFY(0x02074C94,daBeam_CheckCreateHeap);
void daBeam_set_mtx(daBeam_c* b) {
 WWHD_FUNC(0x02075124,void,b);
 f32 distance=100.0f*(f32)b->scale.z;
 gabi::Local<cXyz> delta;beamCall(0x0201ADE0,&b->endpoint,delta.get(),&b->current.pos);
 f32 square=beamCall<f32>(0x028E8DD0,delta.get());
 f32 length=beamCall<f32>(0x028F4384,square)/distance;
 f32 width=((f32)b->beamFrame*(5.0f-(f32)b->fadeFrame))/50.0f+0.5f;
 u32 model=b->model;Vec3f scale=b->scale.get();write<f32>(model,0xC0,scale.y);write<f32>(model,0xBC,scale.x);write<f32>(model,0xC4,scale.z);
 u32 matrix=0x1048D0CC;Vec3f pos=b->current.pos.get();beamCall(0x028E93CC,ptr(matrix),pos.x,pos.y,pos.z);
 s16 x=b->current.angle.x,y=b->current.angle.y,z=b->current.angle.z;beamCall(0x025F1B48,ptr(matrix),x,y,z);
 beamCall(0x025F2518,width,width,length);
 f32 m[12];for(int i=0;i<12;++i)m[i]=read<f32>(matrix,i*4);model=b->model;for(int i=0;i<12;++i)write<f32>(model,0xC8+i*4,m[i]);
 if(b->emitter) {
  pos=b->current.pos.get();beamCall(0x028E93CC,ptr(matrix),pos.x,pos.y,pos.z);
  x=b->current.angle.x;y=b->current.angle.y;z=b->current.angle.z;beamCall(0x025F1B48,ptr(matrix),x,y,z);
  beamCall(0x025F24E0,0.0f,0.0f,(f32)b->emitterOffset);
  u32 emitter=b->emitter;beamCall(0x028249B0,ptr(matrix),ptr(emitter,0x1F0),ptr(emitter,0x22C));
  emitter=b->emitter;f32 s=b->scale.x;for(u32 offset:{0x23Cu,0x220u,0x228u,0x224u,0x240u,0x238u})write<f32>(emitter,offset,s);
 }
 if(b->floorActor) {
  pos=b->endpoint.get();beamCall(0x028E93CC,ptr(matrix),pos.x,pos.y,pos.z);
  beamCall(0x025F25CC,ptr(ea(b),0x8C4));y=b->current.angle.y;beamCall(0x025F1C28,ptr(matrix),y);
  beamCall(0x025F2518,1.0f,1.0f,(f32)b->floorScale);beamCall(0x028E90D4,ptr(matrix),ptr(ea(b),0x814));
  u32 floor=b->floorActor;write<u32>(floor,0x5AC,ea(b)+0x814);write<u8>(floor,0x4B8,1);
 }
}
VERIFY(0x02075124,daBeam_set_mtx);
BOOL daBeamDelete(daBeam_c* b) {
 WWHD_FUNC(0x02075924,BOOL,b);beamCall(0x025E1B34,&b->endpoint);beamCall(0x025204C8,&b->phase,ptr(0x10008758));
 u32 emitter=b->emitter;if(emitter)invalidateEmitter(emitter);return 1;
}
VERIFY(0x02075924,daBeamDelete);
BOOL daBeamDraw(daBeam_c* b) {
 WWHD_FUNC(0x02075F90,BOOL,b);if(!b->active)return 1;
 beamCall(0x02075124,b);
 u32 data=read<u32>(b->model,0xAC);f32 frame=read<f32>(ea(b),0x7A4);beamCall(0x025E7FC4,ptr(ea(b),0x7A0),ptr(data),frame);
 frame=(s16)gabi::ftoi(b->fadeFrame);data=read<u32>(b->model,0xAC);beamCall(0x025E83FC,ptr(ea(b),0x720),ptr(data),frame);
 frame=(s16)gabi::ftoi(b->beamFrame);data=read<u32>(b->model,0xAC);beamCall(0x025E86B8,ptr(ea(b),0x68C),ptr(data),frame);
 beamCall(0x025E2DE0,ptr(b->model),0);return 1;
}
VERIFY(0x02075F90,daBeamDraw);
void daBeam_wait_proc(daBeam_c* b) {
 WWHD_FUNC(0x02076598,void,b);u32 emitter=b->emitter;if(emitter){invalidateEmitter(emitter);b->emitter=0;}
 fade(b);if(!b->active) {b->current.angle.z=b->home.angle.z;b->current.angle.y=b->home.angle.y;b->current.angle.x=b->home.angle.x;}
}
VERIFY(0x02076598,daBeam_wait_proc);
void* daBeam_HIO_ctor(void* p) {
 WWHD_FUNC(0x0207667C,void*,p);if(!p)p=beamCall<void*>(0x0273AD10,16);if(p){write<u8>(ea(p),0,255);write<f32>(ea(p),4,0.0f);write<s16>(ea(p),8,0);write<u32>(ea(p),12,0x1000869C);}return p;
}
VERIFY(0x0207667C,daBeam_HIO_ctor);
void daBeam_sinit() {
 WWHD_FUNC(0x020766D8,void);for(u32 i=0;i<16;i+=4)write<u32>(0x104619C0,i,0);
 beamCall(0x028F026C,ptr(0x10190C84));write<f32>(0x104619A4,0,-3.1415927410125732f);write<f32>(0x104619A8,0,3.1415927410125732f);
 beamCall(0x028ED6F8,ptr(0x104619AC));beamCall(0x028F026C,ptr(0x10190C90));beamCall(0x028EAB2C,ptr(0x104619AD));beamCall(0x028F026C,ptr(0x10190C9C));beamCall<void*>(0x0207667C,ptr(0x104619B0));
}
VERIFY(0x020766D8,daBeam_sinit);
void daBeam_SafeString_dtor(void* p,u32 flags){WWHD_FUNC(0x02076778,void,p,flags);if(p && (flags&1))beamCall(0x0273AF40,p);}
VERIFY(0x02076778,daBeam_SafeString_dtor);
BOOL daBeamIsDelete(void* p){WWHD_FUNC(0x0207678C,BOOL,p);return 1;}
VERIFY(0x0207678C,daBeamIsDelete);
void daBeam_fix_search(daBeam_c* b){WWHD_FUNC(0x02076794,void,b);}
VERIFY(0x02076794,daBeam_fix_search);
void daBeam_dtor(daBeam_c* b,u32 flags) {
 WWHD_FUNC(0x02076798,void,b,flags);if(!b)return;
 write<u32>(ea(b),0x8A4,0x1000863C);write<u32>(ea(b),0x8B0,0x100085FC);write<u32>(ea(b),0x86C,0x100085C4);
 beamCall(0x02008B4C,ptr(ea(b),0x84C),0);beamCall(0x027F3628,ptr(ea(b),0x69C),0);
 beamCall(0x02515980,ptr(ea(b),0x520),2);beamCall(0x02515980,ptr(ea(b),0x3E8),2);beamCall(0x02515860,ptr(ea(b),0x3AC),2);beamCall(0x025D50BC,b,0);
 if(flags&1)beamCall(0x0273AF40,b);
}
VERIFY(0x02076798,daBeam_dtor);
void daBeam_SafeString_terminate(){WWHD_FUNC(0x0207684C,void);}
VERIFY(0x0207684C,daBeam_SafeString_terminate);
namespace {
void hitDirection(daBeam_c* b,void* at,void* other,void* tg) {
 u32 actor=ea(other);
 if(other && read<s16>(actor,8)==0xA8) {
  if(actor+0x314) {s8 room=read<s8>(actor,0x326);s32 reverb=beamCall<s32>(0x02520540,room);beamCall(0x025E1A40,0x2853,ptr(actor,0x314),0,reverb);}
  if(b->sideHit) {
   u16 yaw=b->current.angle.y;gabi::Local<cXyz> direction,delta;
   direction->set(-cosAngle(yaw),0.0f,sinAngle(yaw));
   beamCall(0x0201ADE0,ptr(actor,0x314),delta.get(),&b->current.pos);
   f32 dot=beamCall<f32>(0x028E8F44,direction.get(),delta.get());
   f32 x=direction->x,y=direction->y,z=direction->z;if(dot<0.0f){x=-x;z=-z;}
   write<f32>(ea(at),0x7C,x);write<f32>(ea(at),0x80,y);write<f32>(ea(at),0x84,z);
   write<f32>(ea(tg),0xC0,x);write<f32>(ea(tg),0xC4,y);write<f32>(ea(tg),0xC8,z);
  }
 } else if(other && read<s16>(actor,8)==0xCB) {
  u32 vtable=read<u32>(ea(tg),0x3C);u32 method=read<u32>(vtable,0x3C);beamCall(method,tg);
 }
}
}
void daBeam_AtHitCallback(daBeam_c* b,void* at,void* other,void* tg) {
 WWHD_FUNC(0x02074C98,void,b,at,other,tg);if(!beamCall<s32>(0x025D4604,other))return;
 hitDirection(b,at,other,tg);
 u32 parent=b->parentActorID;u32 id=other?read<u32>(ea(other),4):0xFFFFFFFF;
 if(id!=parent){copyWords(ea(&b->hitPosition),ea(b)+0x458,12);b->hit=1;}
}
VERIFY(0x02074C98,daBeam_AtHitCallback);
void daBeam_AtHitDummyCallback(daBeam_c* b,void* at,void* other,void* tg) {
 WWHD_FUNC(0x02074E90,void,b,at,other,tg);if(beamCall<s32>(0x025D4604,other))hitDirection(b,at,other,tg);
}
VERIFY(0x02074E90,daBeam_AtHitDummyCallback);
void daBeam_checkHitCallback(daBeam_c* b,void* at,void* other,void* tg) {
 WWHD_FUNC(0x02075078,void,b,at,other,tg);if(!beamCall<s32>(0x025D4604,other))return;
 u32 parent=b->parentActorID;u32 id=other?read<u32>(ea(other),4):0xFFFFFFFF;if(parent==id)return;
 if(other && read<s16>(ea(other),8)==0xA8 && !(read<u32>(ea(at),0x54)&1))return;
 copyWords(ea(&b->hitPosition),ea(b)+0x590,12);b->hit=1;
}
VERIFY(0x02075078,daBeam_checkHitCallback);
BOOL daBeam_checkRange(daBeam_c* b,csXyz* angle) {
 WWHD_FUNC(0x02076084,BOOL,b,angle);
 u16 yaw=b->home.angle.y;f32 sn=sinAngle(yaw),cs=cosAngle(yaw);
 gabi::Local<cXyz> forward,side,position,delta,flat;
 forward->set(sn,0.0f,cs);side->set(cs,0.0f,-sn);
 u32 play=beamCall<u32>(0x025200D4);u32 player=read<u32>(play,0x5B2C);
 position->set(read<f32>(player,0x314),read<f32>(player,0x318),read<f32>(player,0x31C));
 u32 vtable=read<u32>(player,0xB4);u32 method=read<u32>(vtable,0x24);position->y=beamCall<f32>(method,ptr(player));
 beamCall(0x0201ADE0,position.get(),delta.get(),&b->current.pos);
 f32 horizontal=beamCall<f32>(0x028E8F44,side.get(),delta.get());f32 vertical=-(f32)delta->y;
 f32 depth=beamCall<f32>(0x028E8F44,forward.get(),delta.get());
 if(!(depth<500.0f) || !(depth>0.0f) || !(std::fabs(horizontal)<150.0f) || !(std::fabs(vertical)<250.0f))return 0;
 if(angle) {
  flat->set(horizontal,0.0f,depth);f32 square=beamCall<f32>(0x028E8DD0,flat.get());f32 length=beamCall<f32>(0x028F4384,square);
  s16 pitch=beamCall<s16>(0x020195B0,vertical,length);angle->x=pitch;
  f32 x=delta->x,z=delta->z;angle->y=beamCall<s16>(0x020195B0,x,z);
 }
 return 1;
}
VERIFY(0x02076084,daBeam_checkRange);
void daBeam_move_search(daBeam_c* b) {
 WWHD_FUNC(0x02076260,void,b);gabi::Local<csXyz> angle;
 if(beamCall<BOOL>(0x02076084,b,angle.get())) {
  s16 pitch=angle->x;s32 delta=beamCall<s32>(0x0200F378,&b->current.angle.x,pitch,2,0x400,0);
  s16 yaw=angle->y;beamCall(0x0200F428,&b->current.angle.y,yaw,4,0x400);
  u32 magnitude=delta<0?0u-(u32)delta:(u32)delta;
  if((s32)magnitude<0x400 && b->active==0) {
   if(!b->emitter) {
    u32 play=beamCall<u32>(0x025200D4);u32 particles=read<u32>(play,0x5AB0);
    u32 emitter=beamCall<u32>(0x025A847C,ptr(particles),0,0x8121,&b->current.pos,ptr(0),ptr(0),255,ptr(0),-1,ptr(0),ptr(0),ptr(0));
    s16 active=b->active;b->emitter=emitter;
    if(active!=0){b->beamFrame=5.0f;b->fadeFrame=0.0f;return;}
   }
   f32 frame=b->beamFrame;b->fadeFrame=0.0f;
   if(frame<5.0f)b->beamFrame=frame+1.0f;
   else {b->fadeFrame=0.0f;b->beamFrame=5.0f;b->active=1;}
  }
 } else {
  if(b->active==0 || !((f32)b->beamFrame>0.4f)) {
   s16 yaw=b->home.angle.y;beamCall(0x0200F428,&b->current.angle.y,yaw,4,0x400);
   s16 pitch=b->home.angle.x;beamCall(0x0200F428,&b->current.angle.x,pitch,4,0x400);
  }
  u32 emitter=b->emitter;if(emitter){invalidateEmitter(emitter);b->emitter=0;}
  fade(b);
 }
}
VERIFY(0x02076260,daBeam_move_search);
s32 daBeam_CreateInit(daBeam_c* b) {
 WWHD_FUNC(0x02075384,s32,b);
 beamCall(0x02515F14,ptr(ea(b),0x3AC),255,255,b);
 u32 mode=(u32)b->mParameters>>30;
 beamCall(0x025164C0,ptr(ea(b),0x3E8),ptr(0x10190BEC));
 gabi::Local<cXyz> end;
 f32 distance=100.0f*(f32)b->scale.z;write<u32>(ea(b),0x42C,ea(b)+0x3AC);endpoint(b,end.get(),distance);
 beamCall(0x02018808,ptr(ea(b),0x500),&b->current.pos,end.get());
 u32 flags=read<u32>(ea(b),0x438);write<f32>(ea(b),0x51C,25.0f);write<u32>(ea(b),0x440,mode?0x02074E90:0x02074C98);write<u32>(ea(b),0x438,flags|6);
 beamCall(0x025164C0,ptr(ea(b),0x520),ptr(0x10190C38));
 write<u32>(ea(b),0x564,ea(b)+0x3AC);beamCall(0x02018808,ptr(ea(b),0x638),&b->current.pos,end.get());
 flags=read<u32>(ea(b),0x520);write<f32>(ea(b),0x654,25.0f);write<u32>(ea(b),0x520,flags|16);
 if(!mode)write<u32>(ea(b),0x578,0x02075078);
 copyWords(ea(b)+0x8C4,0x101E9C38,16);b->floorId=-1;b->floorActor=0;beamCall(0x02075124,b);
 u32 model=b->model;b->cullMtx=model?model+0xC8:0;
 beamCall(0x025D674C,b,-50.0f,-50.0f,0.0f,50.0f,50.0f,2000.0f);
 if(b->parentActorID==0xFFFFFFFF) {
  u8 kind=(u32)b->mParameters;kind=kind>2?2:kind;
  b->searchAdjustment=0;b->searchSlot=-1;
  u32 target=kind==0?0x02076794:0x02076260;
  if(kind<2){b->defaultAdjustment=0;b->defaultSlot=-1;b->defaultTarget=target;}b->searchTarget=target;
  u32 parameters=b->mParameters;b->scale.set(2.5f,2.5f,20.0f);
  f32 offset=read<f32>(0x1047BBD8);b->emitter=0;b->switchId=(parameters>>16)&255;b->emitterOffset=0.001f*offset;
  s32 sw=b->switchId;if(sw!=255 && isSwitch(b,sw))searchProc(b,0x02076598);
  b->sideHit=0;
 } else {
  u32 parameters=b->mParameters;b->emitterOffset=0.0f;b->emitter=0;
  b->floorParticle=1-((parameters>>28)&1);b->sideHit=(parameters>>24)&15;b->smokeParticle=1-((parameters>>29)&1);
 }
 return 4;
}
VERIFY(0x02075384,daBeam_CreateInit);
s32 daBeamCreate(daBeam_c* b) {
 WWHD_FUNC(0x02075720,s32,b);
 if(!((u32)b->actor_condition&8)) {
  if(b) {
   beamCall(0x025D4ED0,b);b->__vtbl=0x1000868C;
   beamCall(0x0200BD2C,ptr(ea(b),0x3AC));beamCall(0x02515DA0,ptr(ea(b),0x3C8));
   write<u32>(ea(b),0x3C4,0x1004AE88);write<u32>(ea(b),0x3C8,0x1004AEC0);
   beamCall(0x02515FB8,ptr(ea(b),0x3E8));write<u32>(ea(b),0x4F8,0x100085B4);write<u32>(ea(b),0x4FC,0x100015A8);beamCall(0x02018150,ptr(ea(b),0x500));
   write<u32>(ea(b),0x424,0x1004AF18);write<u32>(ea(b),0x4FC,0x1004AF70);write<u32>(ea(b),0x518,0x1004AF60);
   beamCall(0x02515FB8,ptr(ea(b),0x520));write<u32>(ea(b),0x634,0x100015A8);write<u32>(ea(b),0x630,0x100085B4);beamCall(0x02018150,ptr(ea(b),0x638));
   write<u32>(ea(b),0x634,0x1004AF70);write<u32>(ea(b),0x55C,0x1004AF18);write<u32>(ea(b),0x650,0x1004AF60);
   beamCall(0x027F2BC0,ptr(ea(b),0x68C),0);write<u32>(ea(b),0x69C,0x1016E54C);beamCall(0x027DA984,ptr(ea(b),0x6A0));
   write<u32>(ea(b),0x6E4,0);write<u32>(ea(b),0x70C,0);write<u32>(ea(b),0x69C,0x100085D4);write<u32>(ea(b),0x714,0);write<u32>(ea(b),0x6D4,0x1016D820);write<u32>(ea(b),0x710,0);write<u32>(ea(b),0x708,0);
   beamCall(0x025E80D0,ptr(ea(b),0x720));beamCall(0x025E7C6C,ptr(ea(b),0x7A0));beamCall(0x02008FEC,ptr(ea(b),0x84C));
   for(u32 off:{0x8A9u,0x8AAu,0x8AEu,0x8ADu,0x8ABu,0x8ACu})write<u8>(ea(b),off,0);
   write<u8>(ea(b),0x8A8,1);write<u32>(ea(b),0x8B4,1);write<u32>(ea(b),0x8A4,0x1000867C);write<u32>(ea(b),0x85C,0x1000864C);write<u32>(ea(b),0x84C,ea(b)+0x8A4);write<u32>(ea(b),0x850,ea(b)+0x8B0);write<u32>(ea(b),0x8B0,0x1000866C);write<u32>(ea(b),0x86C,0x1000865C);
  }
  b->actor_condition=(u32)b->actor_condition|8;
 }
 s32 phase=beamCall<s32>(0x02520460,&b->phase,ptr(0x10008758));
 if(phase==4){if(beamCall<s32>(0x025D63E8,b,ptr(0x02074C94),0x22A0))return beamCall<s32>(0x02075384,b);return 5;}return phase;
}
VERIFY(0x02075720,daBeamCreate);
BOOL daBeam_execute(daBeam_c* b) {
 WWHD_FUNC(0x02075988,BOOL,b);
 s16 slot=b->searchSlot;
 if(slot) {
  s16 adjustment=b->searchAdjustment;u32 self=ea(b)+(s32)adjustment;u32 target;
  if(slot<0)target=b->searchTarget;
  else {s16 offset=read<s16>(ea(b),0x672);u32 vtable=read<u32>(self,(s32)offset);target=read<u32>(vtable,(s32)slot*8+4);}
  beamCall(target,ptr(self));
  s32 sw=b->switchId;if(sw!=255) {
   if(isSwitch(b,sw))searchProc(b,0x02076598);
   else {u32 descriptor=read<u32>(ea(b),0x674),target=b->defaultTarget;write<u32>(ea(b),0x66C,descriptor);b->searchTarget=target;}
  }
 }
 beamCall(0x025E742C,ptr(ea(b),0x7A0));
 gabi::Local<cXyz> end,hitEnd,direction,delta,lineEnd,normal;
 endpoint(b,end.get(),100.0f*(f32)b->scale.z);
 beamCall(0x02018808,ptr(ea(b),0x638),&b->current.pos,end.get());write<f32>(ea(b),0x654,25.0f);
 u32 play=beamCall<u32>(0x025200D4);beamCall(0x0200E240,ptr(play,0x26A4),ptr(ea(b),0x520));
 if(b->active==1) {
  f32 length=100.0f*(f32)b->scale.z;
  if(b->hit==1) {beamCall(0x0201ADE0,&b->hitPosition,delta.get(),&b->current.pos);f32 square=beamCall<f32>(0x028E8DD0,delta.get());length=beamCall<f32>(0x028F4384,square);}
  endpoint(b,hitEnd.get(),length);
  beamCall(0x0201ADE0,hitEnd.get(),direction.get(),&b->current.pos);
  if(!beamCall<s32>(0x0201B47C,direction.get()))copyWords(ea(direction.get()),0x101FFBA8,12);
  beamCall(0x02018808,ptr(ea(b),0x500),&b->current.pos,hitEnd.get());copyWords(ea(b)+0x464,ea(direction.get()),12);write<f32>(ea(b),0x51C,25.0f);
  play=beamCall<u32>(0x025200D4);beamCall(0x0200E240,ptr(play,0x26A4),ptr(ea(b),0x3E8));
 }
 f32 frame=b->beamFrame;
 if(!(frame>0.0f)){b->floorActor=0;b->hit=0;b->floorId=-1;return 0;}
 endpoint(b,lineEnd.get(),(frame*20.0f)*(f32)b->scale.z);write<u8>(ea(b),0x8A0,0);
 beamCall(0x024F1AFC,ptr(ea(b),0x84C),&b->current.pos,lineEnd.get(),b);
 if(b->hit==1){b->endpoint.copy(b->hitPosition);b->floorId=-1;b->floorScale=1.0f;b->floorActor=0;}
 else {
  play=beamCall<u32>(0x025200D4);
  if(beamCall<s32>(0x02008860,ptr(play,0x12A0),ptr(ea(b),0x84C))) {
   play=beamCall<u32>(0x025200D4);u16 group=read<u16>(ea(b),0x862),triangle=read<u16>(ea(b),0x860);
   u32 plane=beamCall<u32>(0x020084C8,ptr(play,0x12A0),group,triangle);
   copyWords(ea(&b->endpoint),ea(b)+0x87C,12);
   if(plane){normal->set(read<f32>(plane),read<f32>(plane,4),read<f32>(plane,8));beamCall(0x02312548,ptr(ea(b),0x8C4),normal.get());}
   if(b->floorId==0xFFFFFFFF){u32 parameters=(!b->floorParticle?1:0)|(!b->smokeParticle?2:0);b->floorId=beamCall<u32>(0x025D5834,0xE7,parameters,&b->endpoint,-1,ptr(0),ptr(0),-1,ptr(0));}
   b->floorScale=1.0f;
  } else {b->endpoint.copy(*lineEnd.get());b->floorScale=1.0f;b->floorId=-1;b->floorActor=0;}
 }
 if(b->floorId!=0xFFFFFFFF && !b->floorActor) {gabi::Local<be<u32>> id;*id.get()=b->floorId;b->floorActor=beamCall<u32>(0x025D5218,ptr(0x025E1234),id.get());}
 s8 room=b->current.roomNo;s32 reverb=beamCall<s32>(0x02520540,room);beamCall(0x025E1A40,0x5067,&b->endpoint,0,reverb);b->hit=0;return 0;
}
VERIFY(0x02075988,daBeam_execute);
BOOL daBeamExecute(daBeam_c* b){WWHD_FUNC(0x02075F8C,BOOL,b);return beamCall<BOOL>(0x02075988,b);}
VERIFY(0x02075F8C,daBeamExecute);
