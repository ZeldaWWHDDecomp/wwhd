/**
 * d_a_stone2.cpp (WWHD)
 * Object - liftable/breakable stones (Stone2)
 *
 * Verified against cking.rpx, complete with the TU's weak/trailing functions.
 * Written from the WWHD code with the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_stone2.cpp) as reference for names and structure.
 */
#include "bindings.h"
#include "d/actor/d_a_stone2.h"
using daStone2::Act_c;
template<class T> struct ProtectedLocal {
 struct Frame {u8 linkage[16];T value;};gabi::Local<Frame> storage;
 T* get()const{return gabi::at<T>(storage.a+offsetof(Frame,value));}
 T* operator->()const{return get();} operator T*()const{return get();}
};
struct StoneSafeString {be<u32> name,vtable;};
static u32 checkedAttr(Act_c* a,u32 file,u32 message) {
 u32 type=a->mType;
 if(type>=5){gabi::call(0x0273AA24,STR(file),0x247,STR(message));type=a->mType;}
 return 0x1003E84C+type*0x60;
}
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 prm(Act_c* a,s32 width,s32 shift) { return gabi::call<u32>(0x024A0B14,a,width,shift); }
static void prmZ_init(Act_c* a) {
 WWHD_FUNC(0x0249E2D0,void,a);
 if(a->mPrmInitialized) return;
 s16 angle=a->home.angle.z;
 a->mPrmInitialized=1;a->home.angle.z=0;a->mPrmZ=(u16)angle;
 a->current.angle.z=0;a->shape_angle.z=0;
}
VERIFY(0x0249E2D0,prmZ_init);
static BOOL is_switch(Act_c* a) {
 WWHD_FUNC(0x0249E300,BOOL,a);
 u32 sw=prm(a,8,8);
 if(sw==255) return FALSE;
 s8 room=a->home.roomNo;u32 save=gabi::load<u32>(0x101F84DC);
 return gabi::call<u32>(0x025BA0C0,gabi::at<u8>(save+0x20),sw,room)!=0;
}
VERIFY(0x0249E300,is_switch);
static u8 chk_appear(Act_c* a) { WWHD_FUNC(0x0249E378,u8,a);return (u8)(is_switch(a)^1); }
VERIFY(0x0249E378,chk_appear);
static void on_switch(Act_c* a) {
 WWHD_FUNC(0x0249EE54,void,a);
 u32 sw=prm(a,8,8);
 if(sw!=255) {u32 save=gabi::load<u32>(0x101F84DC);s8 room=a->home.roomNo;gabi::call(0x025B9E38,gabi::at<u8>(save+0x20),sw,room);}
}
VERIFY(0x0249EE54,on_switch);
static void mode_wait_init(Act_c* a) {
 WWHD_FUNC(0x0249E6A0,void,a);
 u32 at=a->mAtFlags,co=a->mCoFlags,tg=a->mTgFlags;
 a->mAtFlags=at&~1u;a->mCoFlags=co&~1u;a->mMode=0;a->mTgFlags=tg|1u;
}
VERIFY(0x0249E6A0,mode_wait_init);
static void demo_non_init(Act_c* a) {WWHD_FUNC(0x0249E6D0,void,a);a->mDemo=0;}
VERIFY(0x0249E6D0,demo_non_init);
static BOOL damage_bg_proc(Act_c* a) {WWHD_FUNC(0x0249F174,BOOL,a);return FALSE;}
VERIFY(0x0249F174,damage_bg_proc);
static void eff_lift_smoke_end(Act_c* a) {
 WWHD_FUNC(0x0249F17C,void,a);
 if(a->mSmokeActive) {a->mSmokeActive=0;gabi::call(0x025A5F88,&a->mLiftSmoke);}
}
VERIFY(0x0249F17C,eff_lift_smoke_end);
static void eff_lift_smoke_remove(Act_c* a) {WWHD_FUNC(0x0249F958,void,a);u32 callback=gabi::ea(a)+0x7A0;u32 vtable=gabi::load<u32>(callback);gabi::call(gabi::load<u32>(vtable+0x44),gabi::at<u8>(callback));}
VERIFY(0x0249F958,eff_lift_smoke_remove);
static void demo_run_init(Act_c* a) {WWHD_FUNC(0x024A0054,void,a);a->mDemo=2;}
VERIFY(0x024A0054,demo_run_init);
static void demo_run(Act_c* a) {
 WWHD_FUNC(0x024A0128,void,a);
 s16 event=a->mEvent;u32 p=play();
 if(gabi::call<BOOL>(0x025440C8,gabi::at<u8>(p+0x52C4),event)) {
 p=play();u16 flags=gabi::load<u16>(p+0x52B8);gabi::store<u16>(p+0x52B8,flags|8);demo_non_init(a);
 }
}
VERIFY(0x024A0128,demo_run);
static void demo_req_init(Act_c* a) {
 WWHD_FUNC(0x0249F238,void,a);
 if(a->mDemo==0) {
 u8 id=(u8)(u16)a->mPrmZ;s16 event=a->mEvent;
 gabi::call(0x025D7A58,a,event,id,65535,0,1);
 u32 address=gabi::ea(a)+0xFA;u16 flags=gabi::load<u16>(address);
 a->mDemo=1;gabi::store<u16>(address,flags|2);
 }
}
VERIFY(0x0249F238,demo_req_init);
static void cam_lockoff(Act_c* a) {
 WWHD_FUNC(0x0249F2F0,void,a);
 u32 p=play(),camera=gabi::load<u32>(p+0x5AF8),id=gabi::load<u32>(gabi::ea(a)+4);
 gabi::call(0x025052BC,gabi::at<u8>(camera+0x248),id);
}
VERIFY(0x0249F2F0,cam_lockoff);
static void mode_wait(Act_c* a) {
 WWHD_FUNC(0x0249FA28,void,a);
 if(a->actor_status&0x2000) {gabi::call(0x0249F9B0,a);return;}
 u8 delay=a->mBgLockDelay;
 if(delay) {
 delay=(u8)(delay-1);a->mBgLockDelay=delay;
 if(!delay) {u32 bg=a->mpBgW;u8 flags=gabi::load<u8>(bg+0x6C);gabi::store<u8>(bg+0x6C,flags|0x80);}
 }
}
VERIFY(0x0249FA28,mode_wait);
static BOOL Delete(Act_c* a) {
 WWHD_FUNC(0x0249F968,BOOL,a);
 u32 callback=gabi::ea(a)+0x774,vtable=gabi::load<u32>(callback);
 gabi::call(gabi::load<u32>(vtable+0x44),gabi::at<u8>(callback));
 eff_lift_smoke_remove(a);return TRUE;
}
VERIFY(0x0249F968,Delete);
static BOOL Mthd_Create(Act_c* a) {WWHD_FUNC(0x024A0994,BOOL,a);return gabi::call<BOOL>(0x0249E3A0,a);}
VERIFY(0x024A0994,Mthd_Create);
static BOOL Mthd_Delete(Act_c* a) {WWHD_FUNC(0x024A0998,BOOL,a);return gabi::call<BOOL>(0x0249E528,a);}
VERIFY(0x024A0998,Mthd_Delete);
static BOOL Mthd_Execute(Act_c* a) {WWHD_FUNC(0x024A099C,BOOL,a);return gabi::call<BOOL>(0x024F1E9C,a);}
VERIFY(0x024A099C,Mthd_Execute);
static BOOL Mthd_IsDelete(Act_c* a) {WWHD_FUNC(0x024A09A0,BOOL,a);u32 vt=gabi::load<u32>(gabi::ea(a)+0xB4);return gabi::call<BOOL>(gabi::load<u32>(vt+0x2C),a);}
VERIFY(0x024A09A0,Mthd_IsDelete);
static BOOL Mthd_Draw(Act_c* a) {WWHD_FUNC(0x024A09B0,BOOL,a);u32 vt=gabi::load<u32>(gabi::ea(a)+0xB4);return gabi::call<BOOL>(gabi::load<u32>(vt+0x3C),a);}
VERIFY(0x024A09B0,Mthd_Draw);
static void staticDestructor(void* p,u32 deleting) {WWHD_FUNC(0x024A0A54,void,p,deleting);if(p&&(deleting&1))gabi::call(0x0273AF40,p);}
VERIFY(0x024A0A54,staticDestructor);
static Act_c* construct(Act_c* a) {
 WWHD_FUNC(0x0249E1AC,Act_c*,a);
 if(!a) {a=gabi::call<Act_c*>(0x0273AD10,0x7C4);if(!a)return nullptr;}
 gabi::call(0x024F1D40,a);
 u32 base=gabi::ea(a);
 gabi::store<u32>(base+0xB4,0x1003EA2C);
 gabi::call(0x024F0474,&a->mAcch);
 gabi::store<u32>(base+0x3FC,0x1003E408);
 gabi::store<u32>(base+0x40C,0x1003E418);
 gabi::store<u32>(base+0x400,0x1003E428);
 gabi::store<u8>(base+0x404,1);
 gabi::call(0x024EFE94,&a->mAcchCir);
 gabi::call(0x0200BD2C,&a->mStts);
 gabi::call(0x02515DA0,gabi::at<u8>(base+0x60C));
 gabi::store<u32>(base+0x608,0x1004AE88);
 gabi::store<u32>(base+0x60C,0x1004AEC0);
 gabi::call(0x02515FB8,&a->mAtFlags);
 gabi::store<u32>(base+0x740,0x100015A8);
 gabi::store<u32>(base+0x73C,0x1003E3F8);
 gabi::call(0x02018590,gabi::at<u8>(base+0x744));
 gabi::store<u32>(base+0x668,0x1004B108);
 gabi::store<u32>(base+0x758,0x1004B150);
 gabi::store<u32>(base+0x740,0x1004B160);
 gabi::call(0x025A5B18,&a->mBreakSmoke,1);
 gabi::call(0x025A5B18,&a->mLiftSmoke,1);
 return a;
}
VERIFY(0x0249E1AC,construct);
static void mode_carry_init(Act_c* a) {
 WWHD_FUNC(0x0249F9B0,void,a);
 u32 co=a->mCoFlags,tg=a->mTgFlags,at=a->mAtFlags;
 a->mCoFlags=co&~1u;u32 attention=gabi::load<u32>(gabi::ea(a)+0x39C);
 a->mAtFlags=at&~1u;a->mTgFlags=tg|1u;gabi::store<u32>(gabi::ea(a)+0x39C,attention&~0x10u);
 u32 p=play(),bg=a->mpBgW;gabi::call(0x020087EC,gabi::at<u8>(p+0x12A0),gabi::at<u8>(bg));
 on_switch(a);a->mMode=1;
}
VERIFY(0x0249F9B0,mode_carry_init);
static void mode_fine_init(Act_c* a) {
 WWHD_FUNC(0x0249F19C,void,a);
 u32 bg=a->mpBgW,at=a->mAtFlags,tg=a->mTgFlags,co=a->mCoFlags;
 a->mAtFlags=at&~1u;a->mTgFlags=tg&~1u;a->mCoFlags=co&~1u;
 if(gabi::load<u32>(bg)<0x100) {u32 p=play();bg=a->mpBgW;gabi::call(0x020087EC,gabi::at<u8>(p+0x12A0),gabi::at<u8>(bg));}
 if(a->actor_status&0x2000)gabi::call(0x025D9D24,a);
 u32 attention=gabi::load<u32>(gabi::ea(a)+0x39C);a->mFineTimer=1;a->mMode=3;
 gabi::store<u32>(gabi::ea(a)+0x39C,attention&~0x10u);
}
VERIFY(0x0249F19C,mode_fine_init);
static u32 checked_attr(Act_c* a) {
 u32 type=a->mType;
 if(type>=5) {gabi::call(0x0273AA24,STR(0x1003E708),0x247,STR(0x1003E718));type=a->mType;}
 return 0x1003E84C+type*0x60;
}
static void mode_drop_init(Act_c* a) {
 WWHD_FUNC(0x0249FD5C,void,a);
 u32 attr=checked_attr(a),at=a->mAtFlags,type=a->mType,co=a->mCoFlags,tg=a->mTgFlags;
 f32 forward=gabi::load<f32>(attr+0x40);
 a->mCoFlags=co|1u;a->mAtFlags=at|1u;a->mTgFlags=tg|1u;gabi::store<u8>(gabi::ea(a)+0x604,200);
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E708),0x247,STR(0x1003E718));type=a->mType;}
 attr=0x1003E84C+type*0x60;type=a->mType;f32 vertical=gabi::load<f32>(attr+0x3C);
 gabi::store<f32>(gabi::ea(a)+0x370,forward);gabi::store<f32>(gabi::ea(a)+0x340,vertical);
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E708),0x247,STR(0x1003E718));type=a->mType;}
 gabi::store<f32>(gabi::ea(a)+0x374,gabi::load<f32>(0x1003E84C+type*0x60+0x30));
 demo_req_init(a);a->mMode=2;
}
VERIFY(0x0249FD5C,mode_drop_init);
static void mode_fine(Act_c* a) {
 WWHD_FUNC(0x0249FFE0,void,a);
 s16 timer=a->mFineTimer;
 if(timer>0){timer=(s16)(timer-1);a->mFineTimer=timer;
 if(!timer&&a->mType==3){s16 event=a->mEvent;u32 p=play();if(!gabi::call<u32>(0x02544044,gabi::at<u8>(p+0x52C4),event))gabi::call(0x025E1988,0x806);}}
}
VERIFY(0x0249FFE0,mode_fine);
static void demo_req(Act_c* a) {
 WWHD_FUNC(0x024A0060,void,a);
 s16 event=a->mEvent;u32 p=play();
 if(!gabi::call<u32>(0x02544044,gabi::at<u8>(p+0x52C4),event)){demo_non_init(a);return;}
 if(gabi::load<u16>(gabi::ea(a)+0xF8)==2){demo_run_init(a);return;}
 u8 id=(u8)(u16)a->mPrmZ;event=a->mEvent;gabi::call(0x025D7A58,a,event,id,65535,0,1);
 u16 flags=gabi::load<u16>(gabi::ea(a)+0xFA);gabi::store<u16>(gabi::ea(a)+0xFA,flags|2);
}
VERIFY(0x024A0060,demo_req);
static BOOL damage_cc_proc(Act_c* a) {
 WWHD_FUNC(0x0249F0BC,BOOL,a);
 BOOL damaged=FALSE;
 if(gabi::call<u32>(0x025160DC,&a->mAtFlags)) {gabi::call(0x02516094,&a->mAtFlags);gabi::call(0x0249EEAC,a,3);damaged=TRUE;}
 else if(gabi::call<u32>(0x025162A4,&a->mAtFlags)) {
 u32 hit=gabi::call<u32>(0x02516300,&a->mAtFlags);
 if(hit&&a->mType!=3&&(gabi::load<u32>(hit+0x10)&0x20)){gabi::call(0x0249EEAC,a,7);damaged=TRUE;}
 gabi::call(0x0251621C,&a->mAtFlags);
 }
 return damaged;
}
VERIFY(0x0249F0BC,damage_cc_proc);
static BOOL damage_bg_proc_directly(Act_c* a) {
 WWHD_FUNC(0x0249F32C,BOOL,a);
 if(a->mMode!=2)return FALSE;
 u32 flags=gabi::load<u32>(gabi::ea(a)+0x414);
 bool ground=flags&0x20,wall=flags&0x10,roof=flags&0x200;
 if(ground)gabi::call(0x0249EEAC,a,2);
 else if(wall||roof)gabi::call(0x0249EEAC,a,3);
 else return FALSE;
 cam_lockoff(a);return TRUE;
}
VERIFY(0x0249F32C,damage_bg_proc_directly);
static void mode_drop(Act_c* a) {
 WWHD_FUNC(0x0249FE90,void,a);
 gabi::call(0x02312968,a,gabi::at<u8>(gabi::ea(a)+0x4C0));
 u32 type=a->mType;
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E73C),0x247,STR(0x1003E74C));type=a->mType;}
 u32 first=0x1003E84C+type*0x60,second=first;
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E73C),0x247,STR(0x1003E74C));type=a->mType;second=0x1003E84C+type*0x60;}
 f32 stream=gabi::load<f32>(first+0x44),damping=gabi::load<f32>(second+0x48);
 gabi::call(0x023123C0,a,&a->mStts,gabi::at<cXyz>(0x101FFBA8),stream,damping);
 a->shape_angle.x=(s16)((s16)a->shape_angle.x+1000);
}
VERIFY(0x0249FE90,mode_drop);
static void mode_carry(Act_c* a) {
 WWHD_FUNC(0x0249FF48,void,a);
 u32 p=play(),player=gabi::load<u32>(p+0x5B2C);
 if(gabi::load<u32>(player+0x3C0)&0x8000)a->mLifted=1;
 p=play();player=gabi::load<u32>(p+0x5B2C);
 if(gabi::load<u32>(player+0x3C0)&0x8000)gabi::call(0x0249FA68,a);
 p=play();player=gabi::load<u32>(p+0x5B2C);
 if(gabi::load<u32>(player+0x3C0)&0x20)eff_lift_smoke_end(a);
 if(!(a->actor_status&0x2000)){mode_drop_init(a);mode_drop(a);}
}
VERIFY(0x0249FF48,mode_carry);
static BOOL methodDelete(Act_c* a) {
 WWHD_FUNC(0x0249E528,BOOL,a);
 BOOL result=TRUE;
 if(a->mAppears){result=gabi::call<BOOL>(0x024F1F64,a);u32 attr=checkedAttr(a,0x1003E4B0,0x1003E4C0);u32 name=gabi::load<u32>(attr);gabi::call(0x025204C8,&a->mPhase,STR(name));}
 return result;
}
VERIFY(0x0249E528,methodDelete);
static BOOL CreateHeap(Act_c* a) {
 WWHD_FUNC(0x0249E5B8,BOOL,a);
 u32 attr=checkedAttr(a,0x1003E4F4,0x1003E504),type=a->mType,name=gabi::load<u32>(attr);
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E4F4),0x247,STR(0x1003E504));type=a->mType;attr=0x1003E84C+type*0x60;}
 s16 index=gabi::load<s16>(attr+4);u32 manager=gabi::load<u32>(0x101F4F28);
 ProtectedLocal<StoneSafeString> key;key->name=name;key->vtable=0x1003E3E0;
 u32 data=gabi::call<u32>(0x026066C4,gabi::at<u8>(manager),key.get(),index);
 if(!data)gabi::call(0x0273AA24,STR(0x1003E4F4),0x256,STR(0x1003E4E4));
 a->mpModel=gabi::call<J3DModel*>(0x025E38E0,gabi::at<u8>(data),0x80000,0x11000022);
 return a->mpModel!=nullptr;
}
VERIFY(0x0249E5B8,CreateHeap);
static void init_mtx(Act_c* a) {
 WWHD_FUNC(0x0249E89C,void,a);
 u32 model=gabi::ea((J3DModel*)a->mpModel);f32 y=a->scale.y,x=a->scale.x,z=a->scale.z;
 gabi::store<f32>(model+0xBC,x);gabi::store<f32>(model+0xC0,y);gabi::store<f32>(model+0xC4,z);
 gabi::call(0x0249E6DC,a);
}
VERIFY(0x0249E89C,init_mtx);
static void set_senv(Act_c* a,s32 sound,s32 radius) {
 WWHD_FUNC(0x0249EE10,void,a,sound,radius);
 ProtectedLocal<cXyz> position;f32 x=a->current.pos.x,y=a->current.pos.y,z=a->current.pos.z;
 position->x=x;position->y=y;position->z=z;
 u32 id=gabi::load<u32>(gabi::ea(a)+4);
 gabi::call(0x0255F458,position.get(),sound,id,radius);
}
VERIFY(0x0249EE10,set_senv);
static BOOL Draw(Act_c* a) {
 WWHD_FUNC(0x0249F8B4,BOOL,a);
 if(a->mMode!=3){
 u32 env=gabi::call<u32>(0x02555D0C);gabi::call(0x025626A4,gabi::at<u8>(env),1,&a->current.pos,&a->tevStr);
 env=gabi::call<u32>(0x02555D0C);u32 model=gabi::ea((J3DModel*)a->mpModel);gabi::call(0x02562F5C,gabi::at<u8>(env),gabi::at<u8>(model),&a->tevStr);
 u32 p=play();gabi::store<u32>(0x104B4634,gabi::load<u32>(p+0x5D70));
 p=play();gabi::store<u32>(0x104B4638,gabi::load<u32>(p+0x5D74));
 model=gabi::ea((J3DModel*)a->mpModel);gabi::call(0x025E2DE0,gabi::at<u8>(model),0);
 p=play();gabi::store<u32>(0x104B4634,gabi::load<u32>(p+0x5D78));
 p=play();gabi::store<u32>(0x104B4638,gabi::load<u32>(p+0x5D7C));
 }
 return TRUE;
}
VERIFY(0x0249F8B4,Draw);
static void member_call(Act_c* a,u32 descriptor) {
 s16 adjust=gabi::load<s16>(descriptor),slot=gabi::load<s16>(descriptor+2);
 u32 object=gabi::ea(a)+(s32)adjust,target;
 if(slot<0)target=gabi::load<u32>(descriptor+4);
 else {s16 offset=gabi::load<s16>(descriptor+6);u32 table=gabi::load<u32>(object+(s32)offset);target=gabi::load<u32>(table+(u32(slot)<<3)+4);}
 gabi::call(target,gabi::at<Act_c>(object));
}
static void demo_proc_call(Act_c* a) {WWHD_FUNC(0x0249F2A4,void,a);member_call(a,0x1003E630+(u32)(s32)a->mDemo*8);}
VERIFY(0x0249F2A4,demo_proc_call);
static void cull_set_draw(Act_c* a) {
 WWHD_FUNC(0x0249F57C,void,a);
 u32 first=checkedAttr(a,0x1003E668,0x1003E678),type=a->mType,second=first,third,fourth;
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E668),0x247,STR(0x1003E678));type=a->mType;second=0x1003E84C+type*0x60;}
 third=second;
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E668),0x247,STR(0x1003E678));type=a->mType;third=0x1003E84C+type*0x60;}
 fourth=third;
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E668),0x247,STR(0x1003E678));type=a->mType;fourth=0x1003E84C+type*0x60;}
 f32 x=gabi::load<s16>(first+0x28),y=gabi::load<s16>(second+0x2A),z=gabi::load<s16>(third+0x2C),radius=gabi::load<s16>(fourth+0x2E);
 gabi::call(0x025D6768,a,x,y,z,radius);
}
VERIFY(0x0249F57C,cull_set_draw);
static void set_mtx(Act_c* a) {
 WWHD_FUNC(0x0249E6DC,void,a);
 bool dropping=a->mMode==2;f32 x=a->current.pos.x,y,z;
 if(dropping){u32 attr=checkedAttr(a,0x1003E52C,0x1003E53C);y=a->current.pos.y;f32 height=gabi::load<f32>(attr+0x34);z=a->current.pos.z;y=y+height;}
 else {z=a->current.pos.z;y=a->current.pos.y;}
 auto matrix=gabi::at<Mtx34>(0x1048D0CC);gabi::call(0x028E93CC,matrix,x,y,z);
 s16 rx=a->shape_angle.x,ry=a->shape_angle.y,rz=a->shape_angle.z;gabi::call(0x025F1B48,matrix,rx,ry,rz);
 if(dropping){u32 attr=checkedAttr(a,0x1003E52C,0x1003E53C);f32 zero=gabi::load<f32>(0x1003E528),height=gabi::load<f32>(attr+0x34);gabi::call(0x025F24E0,zero,-height,zero);}
 f32 values[12];for(u32 i=0;i<12;i++)values[i]=gabi::load<f32>(0x1048D0CC+i*4);
 u32 model=gabi::ea((J3DModel*)a->mpModel);
 const u8 order[]={1,7,6,5,3,9,10,8,0,11,2,4};
 for(u8 i:order)gabi::store<f32>(model+0xC8+i*4,values[i]);
 gabi::call(0x028E90D4,matrix,gabi::at<Mtx34>(0x1046E0FC));
}
VERIFY(0x0249E6DC,set_mtx);
static void cull_set_move(Act_c* a) {
 WWHD_FUNC(0x0249EC74,void,a);
 u32 attr=checkedAttr(a,0x1003E5A0,0x1003E5B0),type=a->mType;
 if(gabi::load<s16>(attr+0x26)<0){a->mForceUpdate=1;return;}
 u32 slots[4];
 for(u32 i=0;i<4;i++){
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E5A0),0x247,STR(0x1003E5B0));type=a->mType;attr=0x1003E84C+type*0x60;}
 slots[i]=attr;
 }
 f32 x=gabi::load<s16>(slots[0]+0x20),y=gabi::load<s16>(slots[1]+0x22),z=gabi::load<s16>(slots[2]+0x24),radius=gabi::load<s16>(slots[3]+0x26);
 gabi::call(0x025D6768,a,x,y,z,radius);
}
VERIFY(0x0249EC74,cull_set_move);
static void staticInit() {
 WWHD_FUNC(0x024A09C0,void);
 gabi::store<u32>(0x1046E064,0);gabi::store<u32>(0x1046E05C,0);gabi::store<u32>(0x1046E068,0);gabi::store<u32>(0x1046E060,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101D1134));
 f32 first=gabi::load<f32>(0x1003E7FC),second=gabi::load<f32>(0x1003E800);
 gabi::store<f32>(0x1046E050,first);gabi::store<f32>(0x1046E054,second);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046E058));
 gabi::call(0x028F026C,gabi::at<u8>(0x101D1140));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046E059));
 gabi::call(0x028F026C,gabi::at<u8>(0x101D114C));
}
VERIFY(0x024A09C0,staticInit);
static BOOL mode_proc_call(Act_c* a) {
 WWHD_FUNC(0x0249F3D0,BOOL,a);
 member_call(a,0x1003E648+(u32)(s32)a->mMode*8);
 s32 mode=a->mMode;if(!mode)return TRUE;
 if(mode!=3){f32 y=a->current.pos.y,z=a->current.pos.z,x=a->current.pos.x;u32 p=play();gabi::call(0x024F08A8,&a->mAcch,gabi::at<u8>(p+0x12A0));
 if(a->mMode==1){a->current.pos.y=y;a->current.pos.x=x;a->current.pos.z=z;}}
 if(!damage_bg_proc_directly(a)){
 if(a->mMode!=1){u8 room=gabi::load<u8>(gabi::ea(a)+0x326);gabi::store<u8>(gabi::ea(a)+0x1C9,room);u32 p=play();u8 color=gabi::call<u8>(0x024EEEB8,gabi::at<u8>(p+0x12A0),gabi::at<u8>(gabi::ea(a)+0x4D4));gabi::store<u8>(gabi::ea(a)+0x1CA,color);}
 }else mode_fine_init(a);
 if(a->mMode==3&&a->mDemo==0&&(gabi::load<u8>(gabi::ea(a)+0x784)&1)){
 if(!a->mSmokeStarted||(gabi::load<u8>(gabi::ea(a)+0x7B0)&1))return FALSE;
 }
 return TRUE;
}
VERIFY(0x0249F3D0,mode_proc_call);
static BOOL Execute(Act_c* a,gptr<Mtx34>* output) {
 WWHD_FUNC(0x0249F6CC,BOOL,a,output);
 cull_set_move(a);
 bool active=a->mForceUpdate||a->mMode!=0;
 if(!active){u32 cull=prm(a,3,28);if(!cull||!(gabi::load<u32>(gabi::ea(a)+0x2E4)&4))active=true;else active=!gabi::call<u32>(0x025D6CE8,a);}
 if(active){s32 mode=a->mMode;a->mForceUpdate=0;
 if(mode!=3&&(damage_cc_proc(a)||damage_bg_proc(a))){eff_lift_smoke_end(a);mode_fine_init(a);demo_req_init(a);}
 demo_proc_call(a);
 if(mode_proc_call(a)){
 if(a->mMode!=3){u8 room=gabi::load<u8>(gabi::ea(a)+0x326);gabi::store<u8>(gabi::ea(a)+0x612,room);
 gabi::call(0x025165A4,&a->mAtFlags,&a->current.pos);u32 p=play();gabi::call(0x0200E240,gabi::at<u8>(p+0x26A4),&a->mAtFlags);
 if(a->mMode==2){p=play();gabi::call(0x02516C14,gabi::at<u8>(p+0x4EF8),&a->mAtFlags,3);}}
 u32 type=a->mType;f32 x=a->current.pos.x;gabi::store<f32>(gabi::ea(a)+0x390,x);
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E69C),0x247,STR(0x1003E6AC));type=a->mType;}
 f32 y=a->current.pos.y,eyeX=gabi::load<f32>(gabi::ea(a)+0x390),height=gabi::load<f32>(0x1003E84C+type*0x60+0x38),z=a->current.pos.z;y=y+height;
 gabi::store<f32>(gabi::ea(a)+0x37C,eyeX);gabi::store<f32>(gabi::ea(a)+0x398,z);gabi::store<f32>(gabi::ea(a)+0x394,y);gabi::store<f32>(gabi::ea(a)+0x384,z);gabi::store<f32>(gabi::ea(a)+0x380,y);
 }else gabi::call(0x025D57E0,a);
 }
 set_mtx(a);*output=gabi::at<Mtx34>(0x1046E0FC);cull_set_draw(a);return TRUE;
}
VERIFY(0x0249F6CC,Execute);
static BOOL methodCreate(Act_c* a) {
 WWHD_FUNC(0x0249E3A0,BOOL,a);
 u32 flags=gabi::load<u32>(gabi::ea(a)+0x2E4);
 if(!(flags&8)){if(a){construct(a);flags=gabi::load<u32>(gabi::ea(a)+0x2E4);}gabi::store<u32>(gabi::ea(a)+0x2E4,flags|8);}
 prmZ_init(a);a->mType=prm(a,3,24);u8 appears=chk_appear(a);a->mAppears=appears;
 s32 phase=5;
 if(appears){u32 attr=checkedAttr(a,0x1003E438,0x1003E448),name=gabi::load<u32>(attr);phase=gabi::call<s32>(0x02520460,&a->mPhase,STR(name));
 if(phase==4){attr=checkedAttr(a,0x1003E438,0x1003E448);u32 type=a->mType,first=attr,second;
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E438),0x247,STR(0x1003E448));type=a->mType;attr=0x1003E84C+type*0x60;}
 second=attr;
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E438),0x247,STR(0x1003E448));type=a->mType;attr=0x1003E84C+type*0x60;}
 u32 heap=gabi::load<u32>(attr+8);name=gabi::load<u32>(first);s16 index=gabi::load<s16>(second+6);
 phase=gabi::call<s32>(0x024F1D9C,a,STR(name),index,0,heap);
 if(phase!=4&&phase!=5)gabi::call(0x0273AA24,STR(0x1003E438),0x2C4,STR(0x1003E46C));
 }}
 return phase;
}
VERIFY(0x0249E3A0,methodCreate);
static BOOL Create(Act_c* a) {
 WWHD_FUNC(0x0249E8BC,BOOL,a);
 u32 base=gabi::ea(a);
 gabi::call(0x02515F14,&a->mStts,255,255,a);
 gabi::call(0x02516518,&a->mAtFlags,gabi::at<u8>(0x1003E808));
 u32 type=a->mType;gabi::store<u32>(base+0x670,base+0x5F0);
 auto attrFor=[&](u32 t){if(t>=5){gabi::call(0x0273AA24,STR(0x1003E56C),0x247,STR(0x1003E57C));t=a->mType;}return 0x1003E84C+t*0x60;};
 u32 attr=attrFor(type);f32 radius=gabi::load<s16>(attr+0x12);gabi::call(0x020184DC,gabi::at<u8>(base+0x744),radius);
 attr=attrFor(a->mType);f32 height=gabi::load<s16>(attr+0x14);gabi::call(0x02018428,gabi::at<u8>(base+0x744),height);
 for(u32 i=0;i<3;i++)gabi::store<u32>(base+0x6A8+i*4,gabi::load<u32>(0x101FFBA8+i*4));
 gabi::store<u32>(base+0x6E0,gabi::load<u32>(0x101FFBA8));gabi::store<u32>(base+0x6E4,gabi::load<u32>(0x101FFBAC));type=a->mType;gabi::store<u32>(base+0x6E8,gabi::load<u32>(0x101FFBB0));
 attr=attrFor(type);radius=gabi::load<s16>(attr+0x12);f32 wall=gabi::load<f32>(0x1003E568);gabi::call(0x024EFF44,&a->mAcchCir,wall,radius);
 gabi::call(0x024F06B4,&a->mAcch,&a->current.pos,gabi::at<cXyz>(base+0x300),a,1,&a->mAcchCir,gabi::at<cXyz>(base+0x33C),gabi::at<csXyz>(base+0x320),&a->shape_angle);
 u32 flags=gabi::load<u32>(base+0x414);type=a->mType;gabi::store<u32>(base+0x414,flags&~8u);
 attr=attrFor(type);f32 roof=gabi::load<s16>(attr+0x10);type=a->mType;gabi::store<f32>(base+0x4AC,roof);
 attr=attrFor(type);gabi::store<f32>(base+0x374,gabi::load<f32>(attr+0x30));gabi::call(0x025D6870,a,0);
 u32 p=play();gabi::call(0x024F08A8,&a->mAcch,gabi::at<u8>(p+0x12A0));
 u32 attention=gabi::load<u32>(base+0x39C);f32 x=gabi::load<f32>(base+0x2EC),y=gabi::load<f32>(base+0x2F0);
 a->current.pos.x=x;a->current.pos.y=y;flags=gabi::load<u32>(base+0x414);type=a->mType;gabi::store<u32>(base+0x39C,attention|0x10);f32 z=gabi::load<f32>(base+0x2F4);gabi::store<u32>(base+0x414,flags&~0x80u);a->current.pos.z=z;
 attr=attrFor(type);u32 status=a->actor_status;x=a->current.pos.x;type=a->mType;u8 distance=gabi::load<u8>(attr+0xC);a->actor_status=status|0x10000;gabi::store<u8>(base+0x38C,distance);gabi::store<f32>(base+0x390,x);
 attr=attrFor(type);y=a->current.pos.y;z=a->current.pos.z;f32 offset=gabi::load<f32>(attr+0x38);
 a->mSmokeStarted=0;a->mLifted=0;a->mForceUpdate=1;gabi::store<f32>(base+0x394,y+offset);gabi::store<f32>(base+0x398,z);a->mSmokeActive=0;a->mBgLockDelay=2;
 mode_wait_init(a);u8 id=(u8)(u16)a->mPrmZ;p=play();a->mEvent=gabi::call<s16>(0x02543F10,gabi::at<u8>(p+0x52C4),0,id);
 demo_non_init(a);u32 model=gabi::ea((J3DModel*)a->mpModel);gabi::store<u32>(base+0x348,model?model+0xC8:0);init_mtx(a);return TRUE;
}
VERIFY(0x0249E8BC,Create);
struct Color4 {be<u8> r,g,b,a;};
static void eff_b_break(Act_c* a,u16 effect) {
 WWHD_FUNC(0x024A02F0,void,a,effect);
 u32 base=gabi::ea(a);ProtectedLocal<Color4> color;
 color->b=(u8)gabi::load<s16>(base+0x1A4);color->g=(u8)gabi::load<s16>(base+0x1A2);
 u32 type=a->mType;color->r=(u8)gabi::load<s16>(base+0x1A0);color->a=(u8)gabi::load<s16>(base+0x1A6);
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E7AC),0x247,STR(0x1003E7BC));type=a->mType;}
 u32 scaleBits=gabi::load<u32>(0x1003E84C+type*0x60+0x58);f32 value=gabi::f32_from_bits(scaleBits);ProtectedLocal<cXyz> scale;
 // The generated first lfs/stfs preserves SNaN bits; reused FPR stores are quiet.
 gabi::store<u32>(gabi::ea(scale.get()),scaleBits);scale->y=value;scale->z=value;
 u32 p=play(),manager=gabi::load<u32>(p+0x5AB0);
 u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(manager),0,effect,&a->current.pos,0,0,255,0,-1,gabi::at<u8>(base+0x1A8),color.get(),scale.get());
 if(emitter){u32 model=gabi::ea((J3DModel*)a->mpModel);gabi::call(0x028249B0,gabi::at<Mtx34>(model?model+0xC8:0),gabi::at<u8>(emitter+0x1F0),gabi::at<u8>(emitter+0x22C));}
}
VERIFY(0x024A02F0,eff_b_break);
static void eff_m_break(Act_c* a,u16 effect,u16 frame) {
 WWHD_FUNC(0x024A018C,void,a,effect,frame);
 ProtectedLocal<StoneSafeString> first;first->vtable=0x1003E3E0;first->name=0x1003E770;
 u32 manager=gabi::load<u32>(0x101F4F28);u32 modelData=gabi::call<u32>(0x026066C4,gabi::at<u8>(manager),first.get(),0x30);
 ProtectedLocal<StoneSafeString> second;second->vtable=0x1003E3E0;manager=gabi::load<u32>(0x101F4F28);second->name=0x1003E770;
 u32 animation=gabi::call<u32>(0x026066C4,gabi::at<u8>(manager),second.get(),0x66);
 u32 attr=checkedAttr(a,0x1003E778,0x1003E788);f32 scaleValue=gabi::load<f32>(attr+0x54);ProtectedLocal<cXyz> scale;scale->x=scaleValue;scale->y=scaleValue;scale->z=scaleValue;
 u32 p=play();manager=gabi::load<u32>(p+0x5AB0);
 u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(manager),0,effect,&a->current.pos,&a->shape_angle,0,255,0,-1,0,0,scale.get());
 if(emitter){u32 model=gabi::ea((J3DModel*)a->mpModel);gabi::call(0x028249B0,gabi::at<Mtx34>(model?model+0xC8:0),gabi::at<u8>(emitter+0x1F0),gabi::at<u8>(emitter+0x22C));
 u32 object=gabi::call<u32>(0x025A3BDC,0,gabi::at<u8>(emitter),gabi::at<u8>(modelData),1,&a->tevStr,gabi::at<u8>(animation),frame,0);
 if(object){p=play();manager=gabi::load<u32>(p+0x5AB0);u32 list=gabi::load<u32>(manager+0x130);gabi::call(0x0200FE78,gabi::at<u8>(list),gabi::at<u8>(object));}}
}
VERIFY(0x024A018C,eff_m_break);
static void breakSmoke(Act_c* a,u16 modelEffect,u16 burstEffect,u16 frame,u32 offset,u32 guard,u32 heightConst,u32 dynamics,u32 dynamicsGuard,u32 dynamicsConst,u32 particle,u32 particleGuard,u32 particleConst) {
 eff_m_break(a,modelEffect,frame);eff_b_break(a,burstEffect);
 if(!gabi::load<u32>(guard)) {f32 zero=gabi::load<f32>(0x1003E528),height=gabi::load<f32>(heightConst);gabi::store<u32>(guard,1);gabi::store<f32>(offset+8,zero);gabi::store<f32>(offset,zero);gabi::store<f32>(offset+4,height);}
 u32 model=gabi::ea((J3DModel*)a->mpModel);gabi::call(0x028E90D4,gabi::at<Mtx34>(model?model+0xC8:0),gabi::at<Mtx34>(0x1048D0CC));
 gabi::call(0x028E8F64,gabi::at<Mtx34>(0x1048D0CC),gabi::at<cXyz>(offset),&a->mSmokePosition);
 u32 p=play(),manager=gabi::load<u32>(p+0x5AB0);u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(manager),2,0x2027,&a->mSmokePosition,0,0,200,&a->mBreakSmoke,-1,0,0,0);
 if(!emitter)return;
 f32 rate=gabi::load<f32>(0x1003E568);gabi::store<u32>(emitter+0x5C,1);gabi::store<f32>(emitter+0x34,rate);
 u32 dynamicsReady=gabi::load<u32>(dynamicsGuard),particleReady;f32 dx;
 if(dynamicsReady){particleReady=gabi::load<u32>(particleGuard);dx=gabi::load<f32>(dynamics);}
 else {dx=gabi::load<f32>(dynamicsConst);particleReady=gabi::load<u32>(particleGuard);gabi::store<u32>(dynamicsGuard,1);gabi::store<f32>(dynamics+8,dx);gabi::store<f32>(dynamics+4,dx);gabi::store<f32>(dynamics,dx);}
 if(!particleReady){f32 size=gabi::load<f32>(particleConst);gabi::store<u32>(particleGuard,1);gabi::store<f32>(particle,size);gabi::store<f32>(particle+8,size);gabi::store<f32>(particle+4,size);}
 gabi::store<f32>(emitter+0x220,dx);gabi::store<f32>(emitter+0x224,gabi::load<f32>(dynamics+4));gabi::store<f32>(emitter+0x228,gabi::load<f32>(dynamics+8));
 gabi::store<f32>(emitter+0x238,gabi::load<f32>(particle));gabi::store<f32>(emitter+0x23C,gabi::load<f32>(particle+4));gabi::store<f32>(emitter+0x240,gabi::load<f32>(particle+8));
}
static void eff_break_ebrock(Act_c* a){WWHD_FUNC(0x024A03F4,void,a);breakSmoke(a,0x3F4,0x3F3,2,0x1046E06C,0x1046E0D8,0x1003E7E0,0x1046E078,0x1046E0DC,0x1003E7E4,0x1046E084,0x1046E0E0,0x1003E7E8);}
VERIFY(0x024A03F4,eff_break_ebrock);
static void eff_break_ekao(Act_c* a){WWHD_FUNC(0x024A05D4,void,a);breakSmoke(a,0x3F8,0x3F7,3,0x1046E090,0x1046E0E4,0x1003E7E0,0x1046E09C,0x1046E0E8,0x1003E7EC,0x1046E0A8,0x1046E0EC,0x1003E7F0);}
VERIFY(0x024A05D4,eff_break_ekao);
static void eff_break_ebrock2(Act_c* a){WWHD_FUNC(0x024A07B4,void,a);breakSmoke(a,0x3F6,0x3F5,2,0x1046E0B4,0x1046E0F0,0x1003E7F4,0x1046E0C0,0x1046E0F4,0x1003E7F8,0x1046E0CC,0x1046E0F8,0x1003E7E4);}
VERIFY(0x024A07B4,eff_break_ebrock2);
static void damaged(Act_c* a,s32 cause) {
 WWHD_FUNC(0x0249EEAC,void,a,cause);
 u32 item=prm(a,6,0),saved=prm(a,7,16);s16 yaw=a->home.angle.y;ProtectedLocal<csXyz> angle;
 gabi::call(0x0201A478,angle.get(),0,yaw,0);s8 room=a->home.roomNo;
 gabi::call(0x025D8120,&a->current.pos,item,saved,room,0,angle.get(),cause,0);
 gabi::call(0x025D9D24,a);member_call(a,0x1003E5D4+(u32)(s32)a->mType*8);
 u32 attr=checkedAttr(a,0x1003E5FC,0x1003E60C);
 if(gabi::load<s16>(attr+0x50)>0){u32 p=play(),vibration=p+0x599C;attr=checkedAttr(a,0x1003E5FC,0x1003E60C);
 f32 zero=gabi::load<f32>(0x1003E528),one=gabi::load<f32>(0x1003E3C8);ProtectedLocal<cXyz> direction;direction->x=zero;direction->y=one;direction->z=zero;s16 strength=gabi::load<s16>(attr+0x50);
 gabi::call(0x025CB374,gabi::at<u8>(vibration),strength,-33,direction.get());}
 set_senv(a,255,10);attr=checkedAttr(a,0x1003E5FC,0x1003E60C);room=a->current.roomNo;u32 sound=gabi::load<u32>(attr+0x18);s8 reverb=gabi::call<s8>(0x02520540,room);
 gabi::call(0x025E1A40,sound,gabi::at<cXyz>(gabi::ea(a)+0x37C),0,reverb);on_switch(a);
}
VERIFY(0x0249EEAC,damaged);
template<unsigned N>struct Bytes {be<u8> data[N];};
static void eff_lift_smoke_start(Act_c* a) {
 WWHD_FUNC(0x0249FA68,void,a);
 if(a->mSmokeActive)return;
 a->mSmokeStarted=1;a->mSmokeActive=1;
 // dKy_tevstr_init clears 0x72 words, matching the HD stack slot's 0x1C8 bytes.
 ProtectedLocal<Bytes<0x1C8>> tev;u32 local=gabi::ea(tev.get()),defaults=0x1016E414;
 // HD lighting temporaries construct the same default vectors in three blocks.
 for(u32 start:{0u,0xC0u,0x144u}){
 for(u32 off=0;off<=0x40;off+=4){
 if(off==0x18){for(u32 i=0;i<4;i++)gabi::store<u8>(local+start+off+i,gabi::load<u8>(defaults+off+i));}
 else if(off==0x1C||off==0x20){gabi::store<u16>(local+start+off,gabi::load<u16>(defaults+off));gabi::store<u16>(local+start+off+2,gabi::load<u16>(defaults+off+2));}
 else gabi::store<f32>(local+start+off,gabi::load<f32>(defaults+off));
 }}
 f32 ground=gabi::load<f32>(gabi::ea(a)+0x480),invalid=gabi::load<f32>(0x1003E6D0);u8 poly=255;
 if(ground!=invalid){u32 p=play();poly=gabi::call<u8>(0x024EEEB8,gabi::at<u8>(p+0x12A0),gabi::at<u8>(gabi::ea(a)+0x4D4));}
 s8 room=a->current.roomNo;gabi::call(0x0255FFF4,tev.get(),room,poly);
 u32 env=gabi::call<u32>(0x02555D0C);gabi::call(0x025626A4,gabi::at<u8>(env),0,&a->current.pos,tev.get());
 ProtectedLocal<Color4> color;color->r=(u8)gabi::load<s16>(local+0x90);color->b=(u8)gabi::load<s16>(local+0x94);
 u32 type=a->mType;color->a=(u8)gabi::load<s16>(local+0x96);color->g=(u8)gabi::load<s16>(local+0x92);
 if(type>=5){gabi::call(0x0273AA24,STR(0x1003E6D4),0x247,STR(0x1003E6E4));type=a->mType;}
 f32 scaleValue=gabi::load<f32>(0x1003E84C+type*0x60+0x5C);ProtectedLocal<cXyz> scale;scale->x=scaleValue;scale->z=scaleValue;scale->y=scaleValue;
 u32 p=play(),manager=gabi::load<u32>(p+0x5AB0);
 gabi::call(0x025A847C,gabi::at<u8>(manager),2,0x23F2,&a->current.pos,0,scale.get(),128,&a->mLiftSmoke,-1,color.get(),gabi::at<u8>(local+0x98),0);
}
VERIFY(0x0249FA68,eff_lift_smoke_start);
/* ---- trailing weak functions of the TU (vtables 1003EA2C / 1003E3E0, demo table 1003E630) ---- */
// dBgS_MoveBgActor::IsDelete (inline virtual, emitted here; slot 1003EA68 of the Act_c vtable)
static BOOL IsDelete(Act_c* a) {WWHD_FUNC(0x024A0A68,BOOL,a);return TRUE;}
VERIFY(0x024A0A68,IsDelete);
// second virtual of the resource-name helper class (vtable 1003E3E0, after its destructor 024A0A54): empty
static void safeStringVirtual(void* p) {WWHD_FUNC(0x024A0A70,void,p);}
VERIFY(0x024A0A70,safeStringVirtual);
// daStone2::Act_c::~Act_c (deleting destructor, Act_c vtable slot 1003EA38)
static void destructor(Act_c* a,s32 flags) {
 WWHD_FUNC(0x024A0A74,void,a,flags);
 if(!a)return;
 u32 base=gabi::ea(a);
 gabi::call(0x02515A70,&a->mAtFlags,2);          // dCcD_Cyl::~dCcD_Cyl
 gabi::call(0x02515860,&a->mStts,2);             // dCcD_Stts::~dCcD_Stts
 gabi::call(0x02018034,gabi::at<u8>(base+0x5C4),2);
 gabi::store<u32>(base+0x40C,0x1003E418);
 gabi::store<u32>(base+0x400,0x1003E428);
 gabi::call(0x024EFD9C,&a->mAcch,0);              // dBgS_Acch base destructor
 gabi::call(0x025D50BC,a,0);                     // fopAc_ac_c::~fopAc_ac_c
 if(flags&1)gabi::call(0x0273AF40,a);            // operator delete
}
VERIFY(0x024A0A74,destructor);
// daStone2::Act_c::demo_non (demo table entry 0): empty
static void demo_non(Act_c* a) {WWHD_FUNC(0x024A0B10,void,a);}
VERIFY(0x024A0B10,demo_non);
// daObj::PrmAbstract<daStone2::Act_c::Prm_e>(actor, width, shift): bit field of fopAcM_GetParam
static u32 PrmAbstract(Act_c* a,s32 width,s32 shift) {
 WWHD_FUNC(0x024A0B14,u32,a,width,shift);
 u32 param=gabi::load<u32>(gabi::ea(a)+0xB0);
 // PPC slw/srw: shift amount is the low 6 bits, 32..63 gives 0
 u32 mask=((width&32)?0u:(1u<<(width&31)))-1u;
 u32 field=(shift&32)?0u:(param>>(shift&31));
 return field&mask;
}
VERIFY(0x024A0B14,PrmAbstract);
