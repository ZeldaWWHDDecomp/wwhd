#include "bindings.h"
namespace doorkey{template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);}template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);}void* p(u32 a){return gabi::at<void>(a);}struct Name{be<u32>data,vt;};}
using namespace doorkey;
s32 door_key_res_load(void* self){WWHD_FUNC(0x0252B484,s32,self);return gabi::call<s32>(0x02520460,p(gabi::ea(self)+0x94),p(0x1004C7D8));}
VERIFY(0x0252B484,door_key_res_load);
void door_key_res_delete(void* self){WWHD_FUNC(0x0252B494,void,self);gabi::call<void>(0x025204C8,p(gabi::ea(self)+0x94),p(0x1004C7DC));}
VERIFY(0x0252B494,door_key_res_delete);
void door_key_init(void* self,void* door){
 WWHD_FUNC(0x0252B4A4,void,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door);if(ld<u32>(a,4)&&ld<u8>(a)&&!ld<u8>(b,0x3BC)){
  s32 bit=gabi::call<s32>(0x0252AA14,door);if(bit<128){u32 save=ld<u32>(0x101F84DC);gabi::call<void>(0x025B9E38,p(save+0x20),bit,-1);}
  if(!ld<u8>(a,0x9D)){u32 game=gabi::call<u32>(0x025200D4);s32 count=ld<s16>(game,0x5B5C);st<u16>(game,0x5B5C,u16(count-1));}
  if(b&&b+0x37C){u8 big=ld<u8>(a,0x9D);s32 room=ld<s8>(b,0x326);u32 sound=big==1?0x69AE:0x6948;u32 reverb=gabi::call<u32>(0x02520540,room);gabi::call<void>(0x025E1A40,sound,p(b+0x37C),0,reverb);}
  st<u8>(a,0x9C,1);
 }else st<u8>(a,0x9C,0);
}
VERIFY(0x0252B4A4,door_key_init);
void door_key_disable(void* self){WWHD_FUNC(0x0252B5A8,void,self);st<u8>(gabi::ea(self),0,0);}
VERIFY(0x0252B5A8,door_key_disable);
BOOL door_key_proc(void* self){
 WWHD_FUNC(0x0252B5B4,BOOL,self);
 u32 a=gabi::ea(self);if(!ld<u8>(a,0x9C))return 1;if(!gabi::call<u32>(0x025E742C,p(a+8)))return 0;gabi::call<void>(0x0252B5A8,self);st<u8>(a,0x9C,0);return 1;
}
VERIFY(0x0252B5B4,door_key_proc);
namespace doorkey{
BOOL create(u32 a,bool big){
 u32 name=big?0x1004C7E0:0x1004C804;gabi::Local<Name> first,second;first->data=name;first->vt=0x1004C524;
 u32 data=gabi::call<u32>(0x026066C4,p(ld<u32>(0x101F4F28)),first.get(),big?8:9);
 if(!data)gabi::call<void>(0x0273AA24,p(big?0x1004C7E4:0x1004C808),big?0x35A:0x342,p(big?0x1004C7F0:0x1004C814));
 u32 model=gabi::call<u32>(0x025E38E0,p(data),0,0x11020203);st<u32>(a,4,model);if(!model)return 0;
 second->data=name;second->vt=0x1004C524;u32 animation=gabi::call<u32>(0x026066C4,p(ld<u32>(0x101F4F28)),second.get(),big?4:5);f32 rate=ld<f32>(0x1004C7C8);
 return gabi::call<u32>(0x025E8508,p(a+8),p(data),p(animation),1,0,0,-1,0,rate)!=0;
}
}
BOOL door_key_big(void* self){WWHD_FUNC(0x0252B63C,BOOL,self);return create(gabi::ea(self),true);}
VERIFY(0x0252B63C,door_key_big);
BOOL door_key_normal(void* self){WWHD_FUNC(0x0252B738,BOOL,self);return create(gabi::ea(self),false);}
VERIFY(0x0252B738,door_key_normal);
BOOL door_key_create(void* self,s32 type){WWHD_FUNC(0x0252B834,BOOL,self,type);st<u8>(gabi::ea(self),0x9D,u8(type));return gabi::call<BOOL>(type==1?0x0252B63C:0x0252B738,self);}
VERIFY(0x0252B834,door_key_create);
void door_key_enable(void* self){WWHD_FUNC(0x0252B848,void,self);st<u8>(gabi::ea(self),0,1);}
VERIFY(0x0252B848,door_key_enable);
void door_key_matrix(void* self,void* door){
 WWHD_FUNC(0x0252B854,void,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door);if(!ld<u8>(a))return;u32 matrix=0x1048D0CC;f32 x=ld<f32>(b,0x314),y=ld<f32>(b,0x318),z=ld<f32>(b,0x31C);gabi::call<void>(0x028E93CC,p(matrix),x,y,z);gabi::call<void>(0x025F1C28,p(matrix),ld<s16>(b,0x322));
 bool big=ld<u8>(a,0x9D)==1;f32 height=ld<f32>(big?0x1004C828:0x1004C698),depth=ld<f32>(0x1004C82C),zero=ld<f32>(0x1004C590);gabi::call<void>(0x025F24E0,zero,height,depth);
 f32 values[12];for(u32 i=0;i<12;++i)values[i]=ld<f32>(matrix,i*4);u32 model=ld<u32>(a,4);for(u32 i=0;i<12;++i)st<f32>(model,0xC8+i*4,values[i]);
}
VERIFY(0x0252B854,door_key_matrix);
void door_key_draw(void* self,void* door){
 WWHD_FUNC(0x0252B9D4,void,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door),model=ld<u32>(a,4),data=ld<u32>(model,0xAC);u32 env=gabi::call<u32>(0x02555D0C);model=ld<u32>(a,4);gabi::call<void>(0x02562F5C,p(env),p(model),p(b+0x110));f32 frame=ld<f32>(a,0xC);gabi::call<void>(0x025E86B8,p(a+8),p(data),frame);gabi::call<void>(0x025E2DE0,p(ld<u32>(a,4)),0);
}
VERIFY(0x0252B9D4,door_key_draw);
void* door_stop_ctor(void* self){
 WWHD_FUNC(0x0252BA44,void*,self);
 u32 a=gabi::ea(self);if(!a){a=gabi::call<u32>(0x0273AD10,12);if(!a)return nullptr;}
 f32 zero=ld<f32>(0x1004C590);st<u8>(a,0xA,0);st<f32>(a,4,zero);st<u8>(a,0xB,0);st<u8>(a,9,0);st<u32>(a,0,0);st<u8>(a,8,0);return p(a);
}
VERIFY(0x0252BA44,door_stop_ctor);
