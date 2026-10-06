#include "wwhd.h"
#include "gabi.h"
namespace ky { template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);} template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);} void* ptr(u32 a){return gabi::at<void>(a);} struct Vec {f32 x,y,z;}; }
using namespace ky;
void* ky_search(u32 id){
 WWHD_FUNC(0x025DAC50,void*,id);
 return gabi::call<void*>(0x025DE50C,id);
}
VERIFY(0x025DAC50,ky_search);
// Adjacent unnamed tail wrapper included to preserve the full unit interval.
BOOL ky_wrapper(void* a){
 WWHD_FUNC(0x025DAC54,BOOL,a);
 return gabi::call<BOOL>(0x025DA958,a);
}
VERIFY(0x025DAC54,ky_wrapper);
void* ky_append(){
 WWHD_FUNC(0x025DAC58,void*);
 u32 a=gabi::call<u32>(0x02019430,-4,0x1C);
 if(a){gabi::call<void>(0x0200ECCC,ptr(a),0,0x1C);f32 one=ld<f32>(0x100579B8);st<f32>(a,0xC,one);st<f32>(a,0x14,one);st<f32>(a,0x10,one);}
 return ptr(a);
}
VERIFY(0x025DAC58,ky_append);
void* ky_make_append(s32 param,void* pos,void* scale){
 WWHD_FUNC(0x025DACB8,void*,param,pos,scale);
 u32 a=gabi::call<u32>(0x025DAC58);
 if(a){if(pos){u32 b=gabi::ea(pos);st<u32>(a,0,ld<u32>(b));st<u32>(a,4,ld<u32>(b,4));st<u32>(a,8,ld<u32>(b,8));}if(scale){u32 b=gabi::ea(scale);st<u32>(a,0xC,ld<u32>(b));st<u32>(a,0x10,ld<u32>(b,4));st<u32>(a,0x14,ld<u32>(b,8));}st<s32>(a,0x18,param);}
 return ptr(a);
}
VERIFY(0x025DACB8,ky_make_append);
u32 ky_delete(void* a){
 WWHD_FUNC(0x025DAD48,u32,a);
 return gabi::call<u32>(0x025DF944,a);
}
VERIFY(0x025DAD48,ky_delete);
u32 ky_create(s32 name,void* callback,void* data){
 WWHD_FUNC(0x025DAD4C,u32,name,callback,data);
 u32 layer=gabi::call<u32>(0x025DED64);
 return gabi::call<u32>(0x025E14A8,ptr(layer),name,callback,0,data);
}
VERIFY(0x025DAD4C,ky_create);
u32 ky_create_params(s32 name,s32 param,void* pos,void* scale,void* callback){
 WWHD_FUNC(0x025DADA4,u32,name,param,pos,scale,callback);
 u32 a=gabi::call<u32>(0x025DACB8,param,pos,scale);
 if(!a)return 0xFFFFFFFFu;
 return gabi::call<u32>(0x025DAD4C,name,callback,ptr(a));
}
VERIFY(0x025DADA4,ky_create_params);
void* ky_fast_create(s32 name,s32 param,void* pos,void* scale,void* callback){
 WWHD_FUNC(0x025DAE00,void*,name,param,pos,scale,callback);
 u32 a=gabi::call<u32>(0x025DACB8,param,pos,scale);
 if(!a)return nullptr;
 return gabi::call<void*>(0x025DFAB8,name,callback,0,ptr(a));
}
VERIFY(0x025DAE00,ky_fast_create);
u32 ky_water_pillar(void* pos,f32 xz,f32 y,s32 param){
 WWHD_FUNC(0x025DAE64,u32,pos,xz,y,param);
 u32 a=gabi::call<u32>(0x025DAC58);
 if(!a)return 0xFFFFFFFFu;
 u32 b=gabi::ea(pos);st<u32>(a,0,ld<u32>(b));st<u32>(a,4,ld<u32>(b,4));u32 z=ld<u32>(b,8);
 st<f32>(a,0xC,xz);st<u32>(a,8,z);st<f32>(a,0x14,xz);st<s32>(a,0x18,param);st<f32>(a,0x10,y);
 return gabi::call<u32>(0x025DAD4C,0x1C1,0,ptr(a));
}
VERIFY(0x025DAE64,ky_water_pillar);
s32 ky_magma_pillar(void* pos,f32 size){
 WWHD_FUNC(0x025DAF3C,s32,pos,size);
 gabi::Local<Vec> scale;st<f32>(scale.a,0,size);st<f32>(scale.a,4,size);st<f32>(scale.a,8,size);
 u32 game=gabi::call<u32>(0x025200D4),particles=ld<u32>(game,0x5AB0);
 gabi::call<void>(0x025A847C,ptr(particles),0,0x80D5,pos,0,scale.get(),0xFF,0,-1,0,0,0);
 return -1;
}
VERIFY(0x025DAF3C,ky_magma_pillar);
// Startup attribution is by adjacency, without a matched source function name.
void ky_startup(){
 WWHD_FUNC(0x025DAFBC,void);
 st<u32>(0x1048A4B4,8,0);st<u32>(0x1048A4B4,0,0);st<u32>(0x1048A4B4,12,0);st<u32>(0x1048A4B4,4,0);
 gabi::call<void>(0x028F026C,ptr(0x101F3420));
 f32 lo=ld<f32>(0x100579C4),hi=ld<f32>(0x100579C8);st<f32>(0x1048A4A8,0,lo);st<f32>(0x1048A4AC,0,hi);
 gabi::call<void>(0x028ED6F8,ptr(0x1048A4B0));gabi::call<void>(0x028F026C,ptr(0x101F342C));
 gabi::call<void>(0x028EAB2C,ptr(0x1048A4B1));gabi::call<void>(0x028F026C,ptr(0x101F3438));
}
VERIFY(0x025DAFBC,ky_startup);
