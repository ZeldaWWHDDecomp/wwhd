/**
 * d_a_dai_item.cpp (WWHD)
 * Object - Stand item (Windfall display-stand items: flowers, flags, pinwheel, idols, statues)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww) and checked against WWHD.
 */
#include "d/actor/d_a_dai_item.h"
namespace {
using Actor = daStandItem_c;
template<class T> T read(Actor* a, u32 offset) { return gabi::load<T>(gabi::ea(a)+offset); }
template<class T> void write(Actor* a, u32 offset, T value) { gabi::store<T>(gabi::ea(a)+offset,value); }
u8* part(Actor* a,u32 offset) { return gabi::at<u8>(gabi::ea(a)+offset); }
void invokeMember(Actor* a,u32 descriptor) {
 s32 index=gabi::load<s16>(descriptor+2);
 u32 object=gabi::ea(a)+(s32)gabi::load<s16>(descriptor);
 u32 target;
 if(index<0) target=gabi::load<u32>(descriptor+4);
 else {s32 offset=gabi::load<s16>(descriptor+6);u32 table=gabi::load<u32>(object+offset);target=gabi::load<u32>(table+(u32)index*8+4);}
 gabi::call_ptr(target,gabi::at<Actor>(object));
}
}
void mode_carry_init(daStandItem_c* a) {
 WWHD_FUNC(0x02116E2C,void,a);
 write<f32>(a,0x370,0.0f);
 u32 x=gabi::load<u32>(0x101FFBA8), flags=read<u32>(a,0x42C);
 write<u32>(a,0x33C,x);
 u32 y=gabi::load<u32>(0x101FFBAC);flags&=~1u;
 u32 attention=read<u32>(a,0x39C);
 write<u32>(a,0x340,y);
 u32 z=gabi::load<u32>(0x101FFBB0);
 write<u32>(a,0x42C,flags);write<u32>(a,0x344,z);
 a->mMode=0;write<u32>(a,0x39C,attention&~0x10u);
}
VERIFY(0x02116E2C,mode_carry_init);
void mode_wait_init(daStandItem_c* a) {
 WWHD_FUNC(0x02116E78,void,a);
 u32 flags=read<u32>(a,0x42C);
 write<f32>(a,0x344,0.0f);write<f32>(a,0x340,0.0f);write<f32>(a,0x33C,0.0f);write<f32>(a,0x370,0.0f);
 a->mMode=1;write<u32>(a,0x42C,flags|1u);
}
VERIFY(0x02116E78,mode_wait_init);
void mode_drop_init(daStandItem_c* a) { WWHD_FUNC(0x02117AF0,void,a);a->mMode=2; }
VERIFY(0x02117AF0,mode_drop_init);
void mode_carry(daStandItem_c* a) { WWHD_FUNC(0x02117AFC,void,a);if(!(read<u32>(a,0x2E0)&0x2000u)) mode_drop_init(a); }
VERIFY(0x02117AFC,mode_carry);
void mode_wait(daStandItem_c* a) { WWHD_FUNC(0x021187E8,void,a); }
VERIFY(0x021187E8,mode_wait);
void mode_drop(daStandItem_c* a) {
 WWHD_FUNC(0x02117B0C,void,a);
 gabi::call(0x025D6870,a,(void*)nullptr);
 u32 play=gabi::call<u32>(0x025200D4);
 gabi::call(0x024F08A8,part(a,0x530),gabi::at<u8>(play+0x12A0));
 if(read<u32>(a,0x558)&0x20u) {
  s32 reverb=gabi::call<s32>(0x02520540,(s32)read<s8>(a,0x326));
  gabi::call(0x025E1A40,0x2870u,part(a,0x37C),0,reverb);mode_wait_init(a);
 }
}
VERIFY(0x02117B0C,mode_drop);
void execAction(daStandItem_c* a) {
 WWHD_FUNC(0x02117960,void,a);
 u8 carry=(read<u32>(a,0x2E0)>>13)&1;
 if(carry&&!a->mCarry)mode_carry_init(a);
 if(a->mCarry&&!carry)mode_wait_init(a);
 invokeMember(a,0x101B3EF8+(u32)a->mMode*8u);a->mCarry=carry;
}
VERIFY(0x02117960,execAction);
void itemProc(daStandItem_c* a) {
 WWHD_FUNC(0x02117A24,void,a);
 if(a->mMode!=1)return;
 u32 descriptor=0x101B3E98+(u32)a->mItemType*8u;
 if(gabi::load<s16>(descriptor+2))invokeMember(a,descriptor);
}
VERIFY(0x02117A24,itemProc);
BOOL execute(daStandItem_c* a) {
 WWHD_FUNC(0x02117A84,BOOL,a);
 a->mTimer=(s32)((u32)a->mTimer+1u);execAction(a);itemProc(a);
 gabi::call(0x02116EA8,a);
 u32 cloth=read<u32>(a,0x7B8);
 if(cloth){u32 table=gabi::load<u32>(cloth+12);gabi::call_ptr(gabi::load<u32>(table+0x3C),gabi::at<u8>(cloth));}
 return 1;
}
VERIFY(0x02117A84,execute);
BOOL CheckCreateHeap(daStandItem_c* a) { WWHD_FUNC(0x02116E28,BOOL,a);return gabi::call<BOOL>(0x02116B10,a); }
VERIFY(0x02116E28,CheckCreateHeap);
BOOL daStandItem_Create(daStandItem_c* a) { WWHD_FUNC(0x021177A8,BOOL,a);return gabi::call<BOOL>(0x021175A8,a); }
VERIFY(0x021177A8,daStandItem_Create);
BOOL daStandItem_Delete(daStandItem_c* a) { WWHD_FUNC(0x02117878,BOOL,a);return gabi::call<BOOL>(0x021177AC,a); }
VERIFY(0x02117878,daStandItem_Delete);
BOOL daStandItem_Draw(daStandItem_c* a) { WWHD_FUNC(0x0211795C,BOOL,a);return gabi::call<BOOL>(0x0211787C,a); }
VERIFY(0x0211795C,daStandItem_Draw);
BOOL daStandItem_Execute(daStandItem_c* a) { WWHD_FUNC(0x02117AEC,BOOL,a);return gabi::call<BOOL>(0x02117A84,a); }
VERIFY(0x02117AEC,daStandItem_Execute);
BOOL daStandItem_IsDelete(daStandItem_c* a) { WWHD_FUNC(0x021187E0,BOOL,a);return 1; }
VERIFY(0x021187E0,daStandItem_IsDelete);
BOOL actionFobj03(daStandItem_c* a) { WWHD_FUNC(0x021187EC,BOOL,a);return 1; }
VERIFY(0x021187EC,actionFobj03);
BOOL actionFobj04(daStandItem_c* a) { WWHD_FUNC(0x021187F4,BOOL,a);return 1; }
VERIFY(0x021187F4,actionFobj04);
BOOL actionFobj00(daStandItem_c* a) { WWHD_FUNC(0x02117E64,BOOL,a);gabi::call(0x02117B80,a);return 1; }
VERIFY(0x02117E64,actionFobj00);
BOOL actionFobj01(daStandItem_c* a) { WWHD_FUNC(0x02117E88,BOOL,a);gabi::call(0x02117B80,a);return 1; }
VERIFY(0x02117E88,actionFobj01);
BOOL actionFobj02(daStandItem_c* a) { WWHD_FUNC(0x02117EAC,BOOL,a);gabi::call(0x02117B80,a);return 1; }
VERIFY(0x02117EAC,actionFobj02);
BOOL actionFobj08(daStandItem_c* a) { WWHD_FUNC(0x021180B4,BOOL,a);gabi::call(0x02117B80,a);return 1; }
VERIFY(0x021180B4,actionFobj08);
BOOL actionFobj10(daStandItem_c* a) { WWHD_FUNC(0x021186E4,BOOL,a);gabi::call(0x02117B80,a);return 1; }
VERIFY(0x021186E4,actionFobj10);
BOOL actionFobj11(daStandItem_c* a) { WWHD_FUNC(0x02118708,BOOL,a);a->mBckSpeed=1.0f;gabi::call(0x02117B80,a);return 1; }
VERIFY(0x02118708,actionFobj11);
void emptyDestructor(daStandItem_c* a) { WWHD_FUNC(0x02118898,void,a); }
VERIFY(0x02118898,emptyDestructor);
void globalDeletingDestructor(void* object,u32 flags) { WWHD_FUNC(0x021187CC,void,object,flags);if(object&&(flags&1))gabi::call(0x0273AF40,object); }
VERIFY(0x021187CC,globalDeletingDestructor);

namespace {
void invalidateEmitter(Actor* a,u32 offset,bool immortal=false) {
 u32 emitter=read<u32>(a,offset);
 if(!emitter)return;
 if(immortal) {gabi::store<u32>(emitter+0x254,gabi::load<u32>(emitter+0x254)&~0x40u);emitter=read<u32>(a,offset);}
 u32 flags=gabi::load<u32>(emitter+0x254);
 gabi::store<s32>(emitter+0x5C,-1);gabi::store<u32>(emitter+0x254,flags|1u);write<u32>(a,offset,0);
}
s16 randomTimer(s16 lo,s16 hi) {
 s16 mid=(s16)(((s32)lo+hi)/2),width=(s16)(((s32)hi-lo)/2);
 f32 deviation=gabi::call<f32>(0x02019918,(f32)width);
 return (s16)gabi::ftoi((f32)mid+deviation);
}
void restartAnimation(Actor* a) {
 gabi::store<f32>(read<u32>(a,0x3C0),1.0f);
 gabi::store<f32>(read<u32>(a,0x3C0)+4,0.0f);
}
}
BOOL deleteItem(daStandItem_c* a) {
 WWHD_FUNC(0x021177AC,BOOL,a);
 invalidateEmitter(a,0x7B0);invalidateEmitter(a,0x7AC);invalidateEmitter(a,0x7B4,true);
 gabi::call(0x025204C8,part(a,0x3AC),STR(0x1000CE28));
 gabi::call(0x025204C8,part(a,0x3B4),STR(0x1000CE00));return 1;
}
VERIFY(0x021177AC,deleteItem);
BOOL drawItem(daStandItem_c* a) {
 WWHD_FUNC(0x0211787C,BOOL,a);
 u32 light=gabi::call<u32>(0x02555D0C);
 gabi::call(0x025626A4,gabi::at<u8>(light),0,part(a,0x314),part(a,0x110));
 light=gabi::call<u32>(0x02555D0C);
 gabi::call(0x02562F5C,gabi::at<u8>(light),gabi::at<u8>(read<u32>(a,0x3BC)),part(a,0x110));
 u8 item=a->mItemNo;
 if(item==0x92) {
  u32 model=read<u32>(a,0x3BC),data=gabi::load<u32>(model+0xAC),joints=gabi::load<u32>(data+8);
  gabi::store<u32>(joints+0x14,0);item=a->mItemNo;
 }else{
  u32 animation=read<u32>(a,0x3C0);
  if(animation){u32 model=read<u32>(a,0x3BC),data=gabi::load<u32>(model+0xAC);f32 frame=gabi::load<f32>(animation+4);
   gabi::call(0x025E86B8,gabi::at<u8>(animation),gabi::at<u8>(data),frame);item=a->mItemNo;}
 }
 if(item==0x97)gabi::call(0x0252CA4C,part(a,0x37C),part(a,0x110),gabi::at<u8>(read<u32>(a,0x3BC)),1.0f);
 gabi::call(0x025E2DE0,gabi::at<u8>(read<u32>(a,0x3BC)),0);
 u32 cloth=read<u32>(a,0x7B8);
 if(cloth){u32 table=gabi::load<u32>(cloth+12);gabi::call_ptr(gabi::load<u32>(table+0x44),gabi::at<u8>(cloth));}
 return 1;
}
VERIFY(0x0211787C,drawItem);
void animTest(daStandItem_c* a) {
 WWHD_FUNC(0x02117B80,void,a);
 u32 index=(u32)a->mItemType*2u;
 s16 stopMin=gabi::load<s16>(0x1000CEA8+index),animMax=gabi::load<s16>(0x1000CE90+index);
 s16 animMin=gabi::load<s16>(0x1000CE78+index),stopMax=gabi::load<s16>(0x1000CEC0+index);
 s16 play=a->mBckPlayTimer;a->mbBckDidPlay=0;
 if(play>0) {
  u32 animation=read<u32>(a,0x3C0);a->mBckPlayTimer=(s16)(play-1);
  if(animation){gabi::call(0x025E742C,gabi::at<u8>(animation));a->mbBckDidPlay=1;}
  if(stopMax==0){u32 animationNow=read<u32>(a,0x3C0);a->mBckPlayTimer=16;gabi::store<f32>(animationNow,1.0f);}
  if(a->mBckPlayTimer<15&&stopMax!=0){
   gabi::call(0x0200ECD4,&a->mBckSpeed,0.0f,0.1f,0.1f,0.05f);
   f32 speed=a->mBckSpeed;gabi::store<f32>(read<u32>(a,0x3C0),speed);
  }
  if(a->mBckSpeed==0.0f||a->mBckPlayTimer==0){
   gabi::store<f32>(read<u32>(a,0x3C0),0.0f);a->mBckStopTimer=randomTimer(stopMin,stopMax);
  }
 }else if(a->mBckStopTimer>0){
  s16 remaining=(s16)(a->mBckStopTimer-1);
  if(animMax==0){a->mBckStopTimer=1;return;}
  a->mBckStopTimer=remaining;
  if(!remaining){s16 timer=randomTimer(animMin,animMax);a->mBckSpeed=1.0f;u32 animation=read<u32>(a,0x3C0);a->mBckPlayTimer=timer;if(animation)gabi::store<f32>(animation,1.0f);}
 }
}
VERIFY(0x02117B80,animTest);
void animTestForOneTime(daStandItem_c* a) {
 WWHD_FUNC(0x021180D8,void,a);
 u32 index=(u32)a->mItemType*2u;
 s16 animMin=gabi::load<s16>(0x1000CE78+index),animMax=gabi::load<s16>(0x1000CE90+index);
 s16 stopMin=gabi::load<s16>(0x1000CEA8+index),stopMax=gabi::load<s16>(0x1000CEC0+index);
 a->mbBckDidPlay=0;BOOL stopped=gabi::call<BOOL>(0x025E742C,gabi::at<u8>(read<u32>(a,0x3C0)));
 if(a->mBckStopTimer>0){
  a->mBckStopTimer=(s16)(a->mBckStopTimer-1);
  if(a->mBckStopTimer==0){restartAnimation(a);a->mBckPlayTimer=randomTimer(animMin,animMax);}
 }else if(a->mBckPlayTimer>0){
  s16 remaining=(s16)(a->mBckPlayTimer-1);
  if(remaining>0){a->mBckPlayTimer=remaining;if(stopped)restartAnimation(a);}
  else if(remaining==0){
   if(stopped){a->mBckPlayTimer=0;a->mBckStopTimer=randomTimer(stopMin,stopMax);}
   else {a->mbBckDidPlay=1;a->mBckPlayTimer=1;return;}
  }else a->mBckPlayTimer=remaining;
 }
 if(!stopped)a->mbBckDidPlay=1;
 else if(stopMax==0)restartAnimation(a);
}
VERIFY(0x021180D8,animTestForOneTime);
BOOL actionFobj05(daStandItem_c* a) { WWHD_FUNC(0x02117ED0,BOOL,a);f32 wind=gabi::call<f32>(0x02578348);u32 animation=read<u32>(a,0x3C0);if(animation&&wind>0.0f)gabi::call(0x025E742C,gabi::at<u8>(animation));return 1; }
VERIFY(0x02117ED0,actionFobj05);
BOOL actionFobj07(daStandItem_c* a) { WWHD_FUNC(0x02118064,BOOL,a);f32 wind=gabi::call<f32>(0x02578348);u32 animation=read<u32>(a,0x3C0);if(animation&&wind>0.0f)gabi::call(0x025E742C,gabi::at<u8>(animation));return 1; }
VERIFY(0x02118064,actionFobj07);
void actorDeletingDestructor(daStandItem_c* a,u32 flags) {
 WWHD_FUNC(0x021187FC,void,a,flags);
 if(a){
  gabi::call(0x02018034,part(a,0x708),2);
  write<u32>(a,0x550,0x1000CCFC);write<u32>(a,0x544,0x1000CD0C);
  gabi::call(0x024EFD9C,part(a,0x530),0);gabi::call(0x02515A70,part(a,0x400),2);
  gabi::call(0x02515860,part(a,0x3C4),2);gabi::call(0x025D50BC,a,0);
  if(flags&1)gabi::call(0x0273AF40,a);
 }
}
VERIFY(0x021187FC,actorDeletingDestructor);
void staticInitialize() {
 WWHD_FUNC(0x02118738,void);
 gabi::store<u32>(0x10462FC0,0);gabi::store<u32>(0x10462FB8,0);gabi::store<u32>(0x10462FC4,0);gabi::store<u32>(0x10462FBC,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101B3F10));
 gabi::store<f32>(0x10462FAC,gabi::load<f32>(0x1000CE20));gabi::store<f32>(0x10462FB0,gabi::load<f32>(0x1000CE24));
 gabi::call(0x028ED6F8,gabi::at<u8>(0x10462FB4));gabi::call(0x028F026C,gabi::at<u8>(0x101B3F1C));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x10462FB5));gabi::call(0x028F026C,gabi::at<u8>(0x101B3F28));
}
VERIFY(0x02118738,staticInitialize);

BOOL actionFobj06(daStandItem_c* a) {
 WWHD_FUNC(0x02117F20,BOOL,a);
 gabi::Local<cXyz> wind,horizontal,zero;
 gabi::call(0x0257E34C,wind.get(),part(a,0x314));
 horizontal->set(wind->x,0.0f,wind->z);
 f32 squared=gabi::call<f32>(0x028E8DD0,horizontal.get());
 f32 strength=gabi::call<f32>(0x028F4384,squared);
 zero->set(0.0f,0.0f,0.0f);
 s16 angle=gabi::call<s16>(0x0200F93C,zero.get(),wind.get());
 gabi::call(0x0200FAAC,angle,read<s16>(a,0x322));
 f32 spin=(f32)a->mSpinPower+__builtin_fabsf(strength);
 if(spin>4.0f)spin=4.0f;
 a->mSpinPower=spin;a->mRotationSpeed=(s16)gabi::ftoi(1536.0f*spin);
 f32 maxStep=gabi::call<f32>(0x02578348)+0.3f;
 f32 minStep=gabi::call<f32>(0x02578348)+0.1f;
 gabi::call(0x0200ECD4,&a->mSpinPower,0.0f,0.08f,maxStep,minStep);
 a->mRotation=(s16)((u16)a->mRotation+(u16)a->mRotationSpeed);return 1;
}
VERIFY(0x02117F20,actionFobj06);
void set_mtx(daStandItem_c* a) {
 WWHD_FUNC(0x02116EA8,void,a);
 f32 sx=read<f32>(a,0x330),sz=read<f32>(a,0x338),sy=read<f32>(a,0x334);u32 model=read<u32>(a,0x3BC);
 gabi::store<f32>(model+0xBC,sx);gabi::store<f32>(model+0xC0,sy);gabi::store<f32>(model+0xC4,sz);
 u8* matrix=gabi::at<u8>(0x1048D0CC);
 gabi::call(0x028E93CC,matrix,read<f32>(a,0x314),read<f32>(a,0x318),read<f32>(a,0x31C));
 gabi::call(0x025F1B48,matrix,read<s16>(a,0x320),read<s16>(a,0x322),read<s16>(a,0x324));
 f32 values[12];for(u32 i=0;i<12;++i)values[i]=gabi::load<f32>(0x1048D0CC+i*4);
 model=read<u32>(a,0x3BC);for(u32 i=0;i<12;++i)gabi::store<f32>(model+0xC8+i*4,values[i]);
 if(!read<u32>(a,0x7B8))return;
 struct Offsets {cXyz entries[4];};gabi::Local<Offsets> offsets;
 f32 tweak=gabi::load<f32>(0x1047BBEC),normal=tweak+180.0f,tall=tweak+195.0f;
 for(u32 i=0;i<4;++i)offsets->entries[i].set(0.0f,i==2?tall:normal,0.0f);
 gabi::Local<cXyz> position,wind,copiedWind;
 u8 clothType=read<u8>(a,0x7BC);
 gabi::call(0x0201AD78,part(a,0x314),position.get(),gabi::at<cXyz>(offsets.a+(u32)clothType*12));
 gabi::call(0x0257E34C,wind.get(),position.get());copiedWind->copy(*wind.get());
 f32 windSq=gabi::call<f32>(0x028E8DD0,copiedWind.get());f32 strength=gabi::call<f32>(0x028F4384,windSq);
 f32 currentSq=gabi::call<f32>(0x028E8DD0,part(a,0x7C0));f32 currentStrength=gabi::call<f32>(0x028F4384,currentSq);
 if(strength-currentStrength>0.25f){
  f32 z=copiedWind->z,y=copiedWind->y;write<f32>(a,0x7C8,z);f32 x=copiedWind->x;write<f32>(a,0x7C4,y);write<f32>(a,0x7C0,x);
  s16 angle=gabi::call<s16>(0x020195B0,x,z);gabi::call(0x0200F428,part(a,0x7BE),angle,4,0x1000);
 }else{
  gabi::call(0x0200F164,part(a,0x7C0),copiedWind.get(),0.1f,0.1f);
  s16 angle=gabi::call<s16>(0x020195B0,read<f32>(a,0x7C0),read<f32>(a,0x7C8));gabi::call(0x0200F428,part(a,0x7BE),angle,16,0x400);
 }
 u32 offset=offsets.a+(u32)read<u8>(a,0x7BC)*12;
 gabi::call(0x025F24E0,gabi::load<f32>(offset),gabi::load<f32>(offset+4),gabi::load<f32>(offset+8));
 gabi::call(0x025F1C28,matrix,(s16)(read<s16>(a,0x7BE)-read<s16>(a,0x322)));
 gabi::call(0x0251B638,gabi::at<u8>(read<u32>(a,0x7B8)),matrix);
}
VERIFY(0x02116EA8,set_mtx);
namespace {
u32 createParticle(Actor* a,u32 effect,bool colored) {
 s32 room=colored?(s32)read<s8>(a,0x326):-1;
 u32 play=gabi::call<u32>(0x025200D4),control=gabi::load<u32>(play+0x5AB0);
 return gabi::call<u32>(0x025A847C,gabi::at<u8>(control),0,effect,part(a,0x314),part(a,0x320),0,0xFF,0,room,colored?part(a,0x1A8):nullptr,0,0);
}
void setEmitterMatrix(Actor* a,u32 emitter,u32 matrixOffset) {gabi::call(0x028249B0,part(a,matrixOffset),gabi::at<u8>(emitter+0x1F0),gabi::at<u8>(emitter+0x22C));}
}
BOOL actionFobj09(daStandItem_c* a) {
 WWHD_FUNC(0x02118390,BOOL,a);
 u32 play=gabi::call<u32>(0x025200D4),link=gabi::load<u32>(play+0x5B34);
 gabi::Local<cXyz> delta,horizontal;
 gabi::call(0x0201ADE0,gabi::at<u8>(link+0x314),delta.get(),part(a,0x314));
 horizontal->set(delta->x,0.0f,delta->z);
 f32 sq=gabi::call<f32>(0x028E8DD0,horizontal.get()),distance=gabi::call<f32>(0x028F4384,sq);
 animTestForOneTime(a);
 if(!a->mbBckDidPlay){
  if(distance<500.0f){
   invalidateEmitter(a,0x7B0);invalidateEmitter(a,0x7B4,true);
   u32 emitter=read<u32>(a,0x7AC);
   if(!emitter)write<u32>(a,0x7AC,createParticle(a,0x82B3,true));else setEmitterMatrix(a,emitter,0x77C);
  }else {invalidateEmitter(a,0x7AC);invalidateEmitter(a,0x7B0);invalidateEmitter(a,0x7B4,true);}
 }else{
  if(!a->mPreviousBckDidPlay){
   invalidateEmitter(a,0x7AC);
   if(!read<u32>(a,0x7B0))write<u32>(a,0x7B0,createParticle(a,0x82B4,true));
   u32 emitter=read<u32>(a,0x7B4);
   if(!emitter){emitter=createParticle(a,0x82B5,false);write<u32>(a,0x7B4,emitter);}
   if(emitter)gabi::store<u32>(emitter+0x254,gabi::load<u32>(emitter+0x254)|0x40u);
  }
  u32 first=read<u32>(a,0x7B0);if(first)setEmitterMatrix(a,first,0x77C);
  u32 second=read<u32>(a,0x7B4);if(second)setEmitterMatrix(a,second,0x74C);
 }
 a->mPreviousBckDidPlay=(u8)a->mbBckDidPlay;return 1;
}
VERIFY(0x02118390,actionFobj09);

BOOL CreateHeap(daStandItem_c* a) {
 WWHD_FUNC(0x02116B10,BOOL,a);
 s32 modelIndex=gabi::load<s16>(0x1000CE30+(u32)a->mItemType*2);
 u32 data=gabi::ea(dComIfG_getObjectRes(STR(0x1000CE28),modelIndex,0x1000CC9C));
 if(!data)gabi::call(0x0273AA24,STR(0x1000CD7C),0x239,STR(0x1000CD90));
 bool statue=a->mItemNo==0x97;
 u32 model=gabi::call<u32>(0x025E38E0,gabi::at<u8>(data),statue?0u:0x80000u,statue?0x11020203u:0x11000022u);
 write<u32>(a,0x3BC,model);if(!model)return 0;
 s32 animationIndex=gabi::load<s16>(0x1000CE48+(u32)a->mItemType*2);
 if(animationIndex!=-1){
  u32 animationData=gabi::ea(dComIfG_getObjectRes(STR(0x1000CE28),animationIndex,0x1000CC9C));
  if(!animationData)gabi::call(0x0273AA24,STR(0x1000CD7C),0x250,STR(0x1000CD70));
  u32 animation=gabi::call<u32>(0x0273AD10,0x8C);
  if(animation){
   gabi::call(0x027F2BC0,gabi::at<u8>(animation),0);
   gabi::store<u32>(animation+0x10,0x1016E54C);gabi::call(0x027DA984,gabi::at<u8>(animation+0x14));
   gabi::store<u32>(animation+0x80,0);gabi::store<u32>(animation+0x58,0);gabi::store<u32>(animation+0x48,0x1016D820);
   gabi::store<u32>(animation+0x84,0);gabi::store<u32>(animation+0x10,0x1000CCC4);gabi::store<u32>(animation+0x7C,0);gabi::store<u32>(animation+0x88,0);
  }
  write<u32>(a,0x3C0,animation);if(!animation)return 0;
  u32 mode=gabi::load<u32>(0x1000CD40+(u32)a->mItemType*4);
  if(!gabi::call<BOOL>(0x025E8508,gabi::at<u8>(animation),gabi::at<u8>(data),gabi::at<u8>(animationData),1,mode,1.0f,0,-1,0))return 0;
  gabi::store<f32>(read<u32>(a,0x3C0),0.0f);
 }
 u8 item=a->mItemNo;
 if((item>=0x8F&&item<=0x91)||item==0x93){
  u32 type=(item>=0x8F&&item<=0x91)?gabi::load<u8>(0x1000CCAD+item):3;
  write<u8>(a,0x7BC,(u8)type);
  u32 texture=gabi::ea(dComIfG_getObjectRes(STR(0x1000CE28),(s32)(32+type),0x1000CC9C));
  u32 toon=gabi::ea(dComIfG_getObjectRes(STR(0x1000CD34),3,0x1000CC9C));
  u32 actualType=read<u8>(a,0x7BC);
  struct Factories { be<u32> entries[4]; };gabi::Local<Factories> factories;
  factories->entries[0]=0x0251BFA4;factories->entries[1]=0x0251C134;factories->entries[2]=0x0251C2D0;factories->entries[3]=0x0251C478;
  u32 positions=gabi::load<u32>(0x101B3E68+actualType*4);
  u32 cloth=gabi::call_ptr<u32>(gabi::load<u32>(factories.a+actualType*4),gabi::at<u8>(texture),gabi::at<u8>(toon),part(a,0x110),gabi::at<u8>(positions));
  write<u32>(a,0x7B8,cloth);if(!cloth)return 0;
 }else write<u32>(a,0x7B8,0);
 return 1;
}
VERIFY(0x02116B10,CreateHeap);
namespace {
void attachJointCallback(Actor* a,u32 name) {
 u32 model=read<u32>(a,0x3BC),data=gabi::load<u32>(model+0xAC);
 u32 table=gabi::call<u32>(0x027F68FC,gabi::at<u8>(data));
 u32 relative=gabi::load<u32>(table+0x10),names=relative?table+0x10+relative:0;
 s32 index=gabi::call<s32>(0x027DF9B0,gabi::at<u8>(names),STR(name));
 if(index>=0){
  model=read<u32>(a,0x3BC);data=gabi::load<u32>(model+0xAC);
  u32 count=gabi::load<u32>(data+4),joints=gabi::load<u32>(data+8),slot=(u16)index;
  if(slot<count)joints+=slot*0x1C;
  gabi::store<u32>(joints+8,0x02526580);
 }
}
}
void CreateInit(daStandItem_c* a) {
 WWHD_FUNC(0x02117168,void,a);
 u32 model=read<u32>(a,0x3BC);write<u32>(a,0x348,model?model+0xC8:0);
 gabi::call(0x025D674C,a,-100.0f,-0.0f,-100.0f,100.0f,300.0f,100.0f);
 gabi::call(0x024EFF44,part(a,0x6F4),30.0f,30.0f);
 gabi::call(0x024F06B4,part(a,0x530),part(a,0x314),part(a,0x300),a,1,part(a,0x6F4),part(a,0x33C),0,0);
 if(read<u32>(a,0x2E0)&0x2000u)mode_carry_init(a);else mode_wait_init(a);
 a->mCarry=0;u32 wind=gabi::call<u32>(0x0257DAA8);
 write<u32>(a,0x7C0,gabi::load<u32>(wind));write<u32>(a,0x7C4,gabi::load<u32>(wind+4));write<u32>(a,0x7C8,gabi::load<u32>(wind+8));
 s16 angle=gabi::call<s16>(0x020195B0,read<f32>(a,0x7C0),read<f32>(a,0x7C8));write<s16>(a,0x7BE,angle);set_mtx(a);
 model=read<u32>(a,0x3BC);gabi::store<u32>(model+0xB8,0);
 u8 item=a->mItemNo;
 if(item==0x92||item==0x95){
  if(item==0x92)attachJointCallback(a,0x1000CDD0);
  else {attachJointCallback(a,0x1000CDD4);attachJointCallback(a,0x1000CDE0);}
  model=read<u32>(a,0x3BC);gabi::store<u32>(model+0xB8,gabi::ea(a));
  gabi::call(0x027F4D5C,gabi::at<u8>(read<u32>(a,0x3BC)));
 }
 f32 windPower=gabi::call<f32>(0x02578348);u32 index=(u32)a->mItemType*2;a->mWindPower=windPower;
 s16 animMin=gabi::load<s16>(0x1000CE78+index),animMax=gabi::load<s16>(0x1000CE90+index),stopMin=gabi::load<s16>(0x1000CEA8+index),stopMax=gabi::load<s16>(0x1000CEC0+index);
 s16 mid=(s16)(((s32)animMin+animMax)/2),width=(s16)(((s32)animMax-animMin)/2);
 a->mBckPlayTimer=0;a->mBckStopTimer=randomTimer(stopMin,stopMax);
 if(stopMax==0){f32 deviation=gabi::call<f32>(0x02019918,(f32)width);a->mBckPlayTimer=(s16)gabi::ftoi((f32)mid+deviation);}
 write<u32>(a,0x7AC,0);write<u32>(a,0x7B0,0);write<u32>(a,0x7B4,0);
 u32 light=gabi::call<u32>(0x02555D0C);gabi::call(0x025626A4,gabi::at<u8>(light),0,part(a,0x314),part(a,0x110));
}
VERIFY(0x02117168,CreateInit);
s32 createItem(daStandItem_c* a) {
 WWHD_FUNC(0x021175A8,s32,a);
 u32 status=read<u32>(a,0x2E4);
 if(!(status&8)){
  if(a){
   gabi::call(0x025D4ED0,a);write<u32>(a,0xB4,0x1000CD1C);
   gabi::call(0x0200BD2C,part(a,0x3C4));gabi::call(0x02515DA0,part(a,0x3E0));
   write<u32>(a,0x3DC,0x1004AE88);write<u32>(a,0x3E0,0x1004AEC0);
   gabi::call(0x02515FB8,part(a,0x400));write<u32>(a,0x514,0x100015A8);write<u32>(a,0x510,0x1000CCB4);
   gabi::call(0x02018590,part(a,0x518));write<u32>(a,0x43C,0x1004B108);write<u32>(a,0x514,0x1004B160);write<u32>(a,0x52C,0x1004B150);
   gabi::call(0x024F0474,part(a,0x530));write<u32>(a,0x540,0x1000CCEC);write<u32>(a,0x550,0x1000CCFC);write<u32>(a,0x544,0x1000CD0C);write<u8>(a,0x548,1);
   gabi::call(0x024EFE94,part(a,0x6F4));status=read<u32>(a,0x2E4);
  }
  write<u32>(a,0x2E4,status|8u);
 }
 u8 item=read<u8>(a,0xB3);a->mItemNo=item;
 a->mItemType=(item>=0x8D&&item<=0x97)?gabi::load<u8>(0x1000CD67+item):0;
 s32 result=gabi::call<s32>(0x02520460,part(a,0x3AC),STR(0x1000CE28));if(result!=4)return result;
 s32 clothResult=gabi::call<s32>(0x02520460,part(a,0x3B4),STR(0x1000CDEC));if(clothResult!=4)return clothResult;
 u32 size=gabi::load<u16>(0x1000CE60+(u32)a->mItemType*2);
 if(!gabi::call<BOOL>(0x025D63E8,a,gabi::at<u8>(0x02116E28),size))return 5;
 CreateInit(a);return result;
}
VERIFY(0x021175A8,createItem);
