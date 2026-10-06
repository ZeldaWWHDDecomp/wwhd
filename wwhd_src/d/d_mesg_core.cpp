/* Retained WWHD message-controller core,025F7500..025F7980 exclusive.
 * Controller, routing, state and companion scopes are verified separately. */
#include "gabi.h"
namespace d_mesg_core_cpp {
using namespace gabi;
u32 construct(u32 self) {
 WWHD_FUNC(0x025F7500,u32,self);
 if (!self) { self=call<u32>(0x0273AD10,0x9A4u); if(!self)return 0; }
 store<u32>(self+0x10,0); call(0x025F65F4,self+0x14);
 store<u32>(self+0x970,0); store<u8>(self+0x984,0);
 f32 a=load<f32>(0x100E0C54),b=load<f32>(0x100E0C50);
 store<f32>(self+0x978,a);store<f32>(self+0x974,b);
 f32 zero=load<f32>(0x100E0AD8),one=load<f32>(0x100E0C4C);
 store<f32>(self+0x988,zero);store<u32>(self+0x950,0);
 f32 speed=load<f32>(0x100E0C58);
 store<f32>(self+0x98C,zero);store<f32>(self+0x980,speed);
 bool nullTimers=(self+0x994)==0;
 store<u32>(self+0x948,0xFFFFFFFF);store<u32>(self+0x95C,0);
 store<u32>(self+0x938,0xFFFFFFFF);store<f32>(self+0x990,zero);
 store<f32>(self+0x97C,one);store<u32>(self+0x968,0);store<u32>(self+0x958,0);
 store<u32>(self+0x964,0);store<u8>(self+0x985,0);store<u32>(self+0x960,0);
 store<u32>(self+0x96C,0);store<u32>(self+0x94C,6);store<u32>(self+0x954,0);
 if(nullTimers)call(0x0273AD10,12u);
 store<u8>(self+0x9A0,0);store<u8>(self+0x9A1,0);call(0x025F74D0,self,0u);
 if(!load<u32>(0x101FD2EC)) {
  store<u32>(0x101FD2EC,1);store<f32>(0x101FDD44,one);
  store<f32>(0x101FDD4C,one);store<f32>(0x101FDD48,one);
 }
 for(u32 i=0;i<3;i++)store<u32>(self+0x994+4*i,0);
 return self;
}
VERIFY(0x025F7500,construct);
u32 createGlobal(u32 heap) {
 WWHD_FUNC(0x025F7658,u32,heap);
 u32 result=load<u32>(0x101F4B5C);if(result)return result;
 u32 storage=call<u32>(0x0273B0D4,0x9A4u,heap,4u);
 if(storage){call(0x02752B0C,storage,heap,3u);store<u32>(storage+12,0x100E0CD0);}
 store<u32>(0x101F4B60,storage);
 result=storage?call<u32>(0x025F7500,storage):0;
 store<u32>(0x101F4B5C,result);return result;
}
VERIFY(0x025F7658,createGlobal);
void deleteStorage(u32 self,u32 flags) {
 WWHD_FUNC(0x025F76EC,void,self,flags);
 if(self&&(flags&1))call(0x0273AF40,self);
}
VERIFY(0x025F76EC,deleteStorage);
void advanceTimers(u32 self) {
 WWHD_FUNC(0x025F7700,void,self);
 for(u32 i=0;i<3;i++){
  u32 address=self+0x994+4*i;store<u32>(address,load<u32>(address)+1);
  u32 period=load<u32>(0x100E0C5C+4*i)*20;
  u32 count=load<u32>(address);
  if((s32)count>=(s32)period)store<u32>(address,count-period);
 }
}
VERIFY(0x025F7700,advanceTimers);
void update(u32 self) {
 WWHD_FUNC(0x025F77B0,void,self);
 u32 n=load<u32>(self+0x10)+1;
 u32 quotient=(u32)(((u64)0xD1B71759*n)>>32)>>13;
 store<u32>(self+0x10,n-quotient*10000);
 u32 game=call<u32>(0x025200D4);
 if(load<u8>(game+0x5BD2)){game=call<u32>(0x025200D4);store<u8>(game+0x5BD2,0);}
 game=call<u32>(0x025200D4);
 if(load<u8>(game+0x5C20)){
  u32 object=load<u32>(self+0x240);bool keep=false;
  if(!object||load<u8>(object+1)==9){
   constexpr u32 ids[]={0x80000002,0x80000025,0x80000024,0x80000027,0x8000004E,0x8000004F};
   for(u32 id:ids)if(call<u32>(0x025E1E60)==id){keep=true;break;}
  }
  if(!keep){game=call<u32>(0x025200D4);store<u8>(game+0x5C20,0);}
 }
 call(0x025F7700,self);
}
VERIFY(0x025F77B0,update);
void reset(u32 self) {
 WWHD_FUNC(0x025F78C4,void,self);
 u32 object=load<u32>(self+0x950);store<u32>(self+0x10,0);call(0x026BB330,object);
 object=load<u32>(self+0x954);call_ptr(load<u32>(load<u32>(object+4)+0x41C),object);
 object=load<u32>(self+0x958);call_ptr(load<u32>(load<u32>(object+4)+0x1EC),object);
 object=load<u32>(self+0x95C);call_ptr(load<u32>(load<u32>(object+4)+0x41C),object);
 call(0x026AD6D4,load<u32>(self+0x960));call(0x026516C0,load<u32>(self+0x964));
 call(0x025F74D0,self,0u);
}
VERIFY(0x025F78C4,reset);
u32 messageState(){WWHD_FUNC(0x025F795C,u32);return load<u8>(call<u32>(0x025200D4)+0x5BB2);}
VERIFY(0x025F795C,messageState);
}
