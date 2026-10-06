/* WWHD standard create request.
 * Nine request/phase bodies025E12E0..025E14A8 plus trailing SDK initializer025E1538.
 * The GC load phase is absent in HD; process creation failure no longer frees the module.
 * HD request is0x60: layer44,phase48,name50,append54,callback58,callbackData5C.
 */
#include "bindings.h"
namespace stdcreate {
using gabi::load;using gabi::store;
static s32 create(u32 q) {
 WWHD_FUNC(0x025E12E0,s32,q);
 gabi::call(0x025DEAB4,load<u32>(q+0x44));
 s16 name=load<s16>(q+0x50);u32 data=load<u32>(q+0x54),id=load<u32>(q+0x3C);
 u32 process=gabi::call<u32>(0x025DD414,name,id,data);
 store<u32>(q+0x40,process);
 if(process==0)return 5;
 store<u32>(process+0x14,q);return 2;
}
VERIFY(0x025E12E0,create);
static s32 subcreate(u32 q){
 WWHD_FUNC(0x025E134C,s32,q);
 gabi::call(0x025DEAB4,load<u32>(q+0x44));
 return gabi::call<s32>(0x025DD538,load<u32>(q+0x40));
}
VERIFY(0x025E134C,subcreate);
static s32 complete(u32 q){
 WWHD_FUNC(0x025E1384,s32,q);
 u32 process=load<u32>(q+0x40);
 if(gabi::call<s32>(0x025DD258,load<u32>(0x101F3D60),load<u32>(process+0xB8))==1)
  if(gabi::call<s32>(0x025DEA6C,process+0xC0)==1)return 0;
 return 2;
}
VERIFY(0x025E1384,complete);
static s32 post(u32 q){
 WWHD_FUNC(0x025E13F0,s32,q);
 u32 cb=load<u32>(q+0x58);
 if(cb && gabi::call_ptr<s32>(cb,load<u32>(q+0x40),load<u32>(q+0x5C))==0)return 0;
 return 2;
}
VERIFY(0x025E13F0,post);
static s32 done(u32 q){WWHD_FUNC(0x025E144C,s32,q);return 2;}
VERIFY(0x025E144C,done);
static s32 handler(u32 q){
 WWHD_FUNC(0x025E1454,s32,q);
 s32 state;
 do {state=gabi::call<s32>(0x02019F74,q+0x48,q);}while(state==2);
 return state;
}
VERIFY(0x025E1454,handler);
static s32 remove(u32 q){WWHD_FUNC(0x025E1498,s32,q);return 1;}
VERIFY(0x025E1498,remove);
static s32 cancel(u32 q){WWHD_FUNC(0x025E14A0,s32,q);return 1;}
VERIFY(0x025E14A0,cancel);
static u32 request(u32 layer,s32 name,u32 cb,u32 cbData,u32 append){
 WWHD_FUNC(0x025E14A8,u32,layer,name,cb,cbData,append);
 if(name>=32767)return 0xFFFFFFFF;
 u32 q=gabi::call<u32>(0x025DDAD4,layer,0x60,0x101F46B0);
 if(q==0)return 0xFFFFFFFF;
 gabi::call(0x02019F0C,q+0x48,0x101F4698);
 store<s16>(q+0x50,(s16)name);store<u32>(q+0x54,append);
 u32 id=load<u32>(q+0x3C);
 store<u32>(q+0x5C,cbData);store<u32>(q+0x58,cb);return id;
}
VERIFY(0x025E14A8,request);
static void init(){
 WWHD_FUNC(0x025E1538,void);
 store<u32>(0x1048ABB4,0);store<u32>(0x1048ABAC,0);
 store<u32>(0x1048ABB8,0);store<u32>(0x1048ABB0,0);
 gabi::call(0x028F026C,0x101F46BC);
 f32 a=load<f32>(0x100584A4),b=load<f32>(0x100584A8);
 store<f32>(0x1048ABA0,a);store<f32>(0x1048ABA4,b);
 gabi::call(0x028ED6F8,0x1048ABA8);
 gabi::call(0x028F026C,0x101F46C8);
 gabi::call(0x028EAB2C,0x1048ABA9);
 gabi::call(0x028F026C,0x101F46D4);
}
VERIFY(0x025E1538,init);
}
