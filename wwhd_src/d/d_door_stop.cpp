#include "bindings.h"
namespace doorstop{template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);}template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);}void* p(u32 a){return gabi::at<void>(a);}}
using namespace doorstop;
void door_stop_matrix(void* self,void* door){
 WWHD_FUNC(0x0252BA9C,void,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door);if(!ld<u8>(a,8)||!ld<u32>(a))return;u32 matrix=0x1048D0CC;f32 y=ld<f32>(b,0x318),offset=ld<f32>(a,4),x=ld<f32>(b,0x314),z=ld<f32>(b,0x31C);y=gabi::fadds_ppc(y,offset);gabi::call<void>(0x028E93CC,p(matrix),x,y,z);gabi::call<void>(0x025F1C28,p(matrix),ld<s16>(b,0x322));if(ld<u8>(a,9)==1)gabi::call<void>(0x025F1C28,p(matrix),0x7FFF);
 f32 values[12];for(u32 i=0;i<12;++i)values[i]=ld<f32>(matrix,i*4);u32 model=ld<u32>(a);for(u32 i=0;i<12;++i)st<f32>(model,0xC8+i*4,values[i]);
}
VERIFY(0x0252BA9C,door_stop_matrix);
void door_stop_close_init(void* self,void* door){
 WWHD_FUNC(0x0252BB9C,void,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door);f32 height=ld<f32>(0x1004C830),zero=ld<f32>(0x1004C590);st<f32>(a,4,height);st<f32>(b,0x370,zero);
 if(b&&b+0x37C){u32 arg=gabi::call<u32>(0x0252AA58,door);s32 room=ld<s8>(b,0x326);u32 sound=arg==0x11?0x697C:0x6906,reverb=gabi::call<u32>(0x02520540,room);gabi::call<void>(0x025E1A40,sound,p(b+0x37C),0,reverb);}
 st<u8>(a,0xB,1);
}
VERIFY(0x0252BB9C,door_stop_close_init);
s32 door_stop_close_proc(void* self,void* door){
 WWHD_FUNC(0x0252BC3C,s32,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door);if(!ld<u8>(a,0xB))return 1;f32 target=ld<f32>(0x1004C834),step=ld<f32>(0x1004C838);gabi::call<void>(0x0200F5C8,p(b+0x370),target,step);step=ld<f32>(b,0x370);target=ld<f32>(0x1004C590);if(!gabi::call<u32>(0x0200F5C8,p(a+4),target,step))return 0;st<u8>(a,0xB,0);return 2;
}
VERIFY(0x0252BC3C,door_stop_close_proc);
void door_stop_open_init(void* self,void* door){
 WWHD_FUNC(0x0252BCF4,void,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door);f32 zero=ld<f32>(0x1004C590);st<f32>(a,4,zero);st<f32>(b,0x370,zero);
 if(b&&b+0x37C){u32 arg=gabi::call<u32>(0x0252AA58,door);s32 room=ld<s8>(b,0x326);u32 sound=arg==0x11?0x697D:0x6906,reverb=gabi::call<u32>(0x02520540,room);gabi::call<void>(0x025E1A40,sound,p(b+0x37C),0,reverb);}
 st<u8>(a,0xB,1);
}
VERIFY(0x0252BCF4,door_stop_open_init);
s32 door_stop_open_proc(void* self,void* door){
 WWHD_FUNC(0x0252BD8C,s32,self,door);
 u32 a=gabi::ea(self),b=gabi::ea(door);if(!ld<u8>(a,0xB))return 1;f32 target=ld<f32>(0x1004C83C),step=ld<f32>(0x1004C840);gabi::call<void>(0x0200F5C8,p(b+0x370),target,step);step=ld<f32>(b,0x370);target=ld<f32>(0x1004C830);if(!gabi::call<u32>(0x0200F5C8,p(a+4),target,step))return 0;st<u8>(a,0xB,0);st<u8>(a,8,0);return 2;
}
VERIFY(0x0252BD8C,door_stop_open_proc);
BOOL door_stop_create(void* self){
 WWHD_FUNC(0x0252BE48,BOOL,self);
 u32 a=gabi::ea(self),data=gabi::call<u32>(0x0252447C,p(0x1004C844),p(0x1004C84C));if(!data)return 1;u32 model=gabi::call<u32>(0x025E38E0,p(data),0,0x11020203);st<u32>(a,0,model);return model!=0;
}
VERIFY(0x0252BE48,door_stop_create);
