#include "bindings.h"
namespace doorsmoke{template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);}template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);}void* p(u32 a){return gabi::at<void>(a);}}
using namespace doorsmoke;
void* door_smoke_ctor(void* self){
 WWHD_FUNC(0x0252B190,void*,self);
 u32 a=gabi::ea(self);if(!a){a=gabi::call<u32>(0x0273AD10,0x38);if(!a)return nullptr;}
 gabi::call<void>(0x025A5B18,p(a),1);st<u8>(a,0x35,0);st<u8>(a,0x34,0);return p(a);
}
VERIFY(0x0252B190,door_smoke_ctor);
void door_smoke_init(void* self,void* actor){
 WWHD_FUNC(0x0252B1E8,void,self,actor);
 u32 a=gabi::ea(self),b=gabi::ea(actor);st<u32>(a,0x28,ld<u32>(b,0x314));st<u32>(a,0x2C,ld<u32>(b,0x318));st<u32>(a,0x30,ld<u32>(b,0x31C));st<u16>(a,0x22,u16(ld<s16>(b,0x32A)));s32 room=ld<s8>(b,0x326);
 u32 game=gabi::call<u32>(0x025200D4),particles=ld<u32>(game,0x5AB0);u32 emitter=gabi::call<u32>(0x025A847C,p(particles),0,0x2022,p(a+0x28),p(a+0x20),0,0xAA,self,room,0,0,0);
 st<u8>(a,0x34,0);st<u8>(a,0x35,0);
 if(emitter){f32 alpha=ld<f32>(0x1004C7C0),rate=ld<f32>(0x1004C7BC);st<f32>(emitter,0x58,alpha);f32 scale=ld<f32>(0x1004C7C4);st<f32>(emitter,0x34,rate);st<f32>(emitter,0x240,scale);st<f32>(emitter,0x220,scale);st<f32>(emitter,0x23C,scale);st<f32>(emitter,0x224,scale);st<f32>(emitter,0x238,scale);st<f32>(emitter,0x228,scale);}
}
VERIFY(0x0252B1E8,door_smoke_init);
void door_smoke_proc(void* self,void* actor){
 WWHD_FUNC(0x0252B2D8,void,self,actor);
 u32 a=gabi::ea(self),b=gabi::ea(actor);if(!ld<u8>(a,0x35)){st<u8>(a,0x35,1);return;}
 u32 count=ld<u8>(a,0x34);f32 sign=ld<f32>(count&1?0x1004C7C8:0x1004C7CC);f64 bias=ld<f64>(0x1004C7D0);u64 bits=0x4330000000000000ull|u32((count*20)^0x80000000u);f64 encoded;memcpy(&encoded,&bits,8);f32 amount=f32(encoded-bias),x=ld<f32>(a,0x28);st<u8>(a,0x34,u8(count+1));amount=gabi::fmuls_ppc(amount,sign);f32 sz=ld<f32>(b,0x3B8);x=gabi::fmadds(amount,sz,x);f32 z=ld<f32>(a,0x30);st<f32>(a,0x28,x);f32 sx=ld<f32>(b,0x3B0);z=gabi::fmadds(amount,sx,z);st<f32>(a,0x30,z);
}
VERIFY(0x0252B2D8,door_smoke_proc);
void door_smoke_end(void* self){WWHD_FUNC(0x0252B3CC,void,self);gabi::call<void>(0x025A5F88,self);}
VERIFY(0x0252B3CC,door_smoke_end);
void* door_key_ctor(void* self){
 WWHD_FUNC(0x0252B3D0,void*,self);
 u32 a=gabi::ea(self);if(!a){a=gabi::call<u32>(0x0273AD10,0xA0);if(!a)return nullptr;}
 st<u8>(a,0,0);st<u32>(a,4,0);gabi::call<void>(0x027F2BC0,p(a+8),0);st<u32>(a,0x18,0x1016E54C);gabi::call<void>(0x027DA984,p(a+0x1C));st<u32>(a,0x8C,0);st<u32>(a,0x88,0);st<u32>(a,0x84,0);st<u32>(a,0x90,0);st<u32>(a,0x18,0x1004C53C);st<u32>(a,0x50,0x1016D820);st<u32>(a,0x60,0);gabi::call<void>(0x028F521C,p(a+0x94),8);st<u8>(a,0x9D,0);st<u8>(a,0x9C,0);return p(a);
}
VERIFY(0x0252B3D0,door_key_ctor);
