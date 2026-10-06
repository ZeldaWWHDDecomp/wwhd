#include "wwhd.h"
#include "gabi.h"
namespace dvd { template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);} template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);} void* p(u32 a){return gabi::at<void>(a);} struct Path {u8 bytes[0x101];}; }
using namespace dvd;
void dvd_path(void* dest,void* input){
 WWHD_FUNC(0x025E21A8,void,dest,input);
 gabi::Local<Path> local;gabi::call<void>(0x0284B7BC,local.get(),input);gabi::call<void>(0xC000A848,dest,local.get(),0x101);
}
VERIFY(0x025E21A8,dvd_path);
s32 dvd_command_cb(void* address){
 WWHD_FUNC(0x025E21E8,s32,address);
 u32 a=ld<u32>(gabi::ea(address)),vt=ld<u32>(a,0x10),target=ld<u32>(vt,0x14);
 s32 result=gabi::call_ptr<s32>(target,p(a),gabi::cpu->r[4],gabi::cpu->r[5],gabi::cpu->r[6],gabi::cpu->r[7],gabi::cpu->r[8],gabi::cpu->r[9],gabi::cpu->r[10]);
 if(result!=1)gabi::call<void>(0x025F2710,p(0x10058518));return 0;
}
VERIFY(0x025E21E8,dvd_command_cb);
void* dvd_command_ctor(void* self){
 WWHD_FUNC(0x025E2234,void*,self);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0x14);
 if(a){st<u8>(a,0xC,0);st<u32>(a,0x10,0x10058688);gabi::call<void>(0x02019C94,p(a));}return p(a);
}
VERIFY(0x025E2234,dvd_command_ctor);
void* dvd_callback_ctor(void* self,void* callback,void* user){
 WWHD_FUNC(0x025E2290,void*,self,callback,user);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0x20);
 if(a){gabi::call<void>(0x025E2234,p(a));st<u32>(a,0x14,gabi::ea(callback));st<u32>(a,0x18,gabi::ea(user));st<u32>(a,0x10,0x100586A0);st<u32>(a,0x1C,0);}return p(a);
}
VERIFY(0x025E2290,dvd_callback_ctor);
void dvd_kick(void* self){
 WWHD_FUNC(0x025E230C,void,self);
 u32 a=ld<u32>(0x1048CE3C),vt=ld<u32>(a,0xC),target=ld<u32>(vt,0x1C);
 gabi::call_ptr<void>(target,p(a),1,1,gabi::cpu->r[6],gabi::cpu->r[7],gabi::cpu->r[8],gabi::cpu->r[9],gabi::cpu->r[10]);
}
VERIFY(0x025E230C,dvd_kick);
void dvd_add(void* self,void* command){
 WWHD_FUNC(0x025E232C,void,self,command);
 u32 a=gabi::ea(self);gabi::call<void>(0xC0009E40,p(a+0x58));gabi::call<void>(0x0200FE78,p(a+0x4C),command);gabi::call<void>(0xC000A1C8,p(a+0x58));gabi::call<void>(0x025E230C,self);
}
VERIFY(0x025E232C,dvd_add);
void* dvd_callback_create(void* callback,void* user){
 WWHD_FUNC(0x025E2384,void*,callback,user);
 u32 heap=gabi::call<u32>(0x025EE060),a=gabi::call<u32>(0x0273B050,0x20,p(heap),-4);
 if(!a)return nullptr;
 a=gabi::call<u32>(0x025E2290,p(a),callback,user);if(a)gabi::call<void>(0x025E232C,p(0x1048CE40),p(a));return p(a);
}
VERIFY(0x025E2384,dvd_callback_create);
BOOL dvd_callback_execute(void* self){
 WWHD_FUNC(0x025E2404,BOOL,self);
 u32 a=gabi::ea(self),target=ld<u32>(a,0x14),user=ld<u32>(a,0x18);
 u32 result=gabi::call_ptr<u32>(target,p(user),gabi::cpu->r[4],gabi::cpu->r[5],gabi::cpu->r[6],gabi::cpu->r[7],gabi::cpu->r[8],gabi::cpu->r[9],gabi::cpu->r[10]);st<u32>(a,0x1C,result);st<u8>(a,0xC,1);return result!=0;
}
VERIFY(0x025E2404,dvd_callback_execute);
void* dvd_ram_ctor(void* self,s32 direction){
 WWHD_FUNC(0x025E2450,void*,self,direction);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0x230);
 if(a){gabi::call<void>(0x025E2234,p(a));st<u8>(a,0x14,u8(direction));st<u32>(a,0x10,0x100586B8);st<u8>(a,0x15,0);st<u32>(a,0x120,0);st<u32>(a,0x118,0);st<u32>(a,0x11C,0);
 u32 b=a+0x124;if(!b)b=gabi::call<u32>(0x0273AD10,0x10C);
 if(b){u32 c=b;if(!c)c=gabi::call<u32>(0x0273AD10,0xC);if(c){st<u32>(c,0,b+0xC);st<u32>(c,4,0x10058574);st<u32>(c,8,0x100);st<u8>(b,0x10B,0);}u32 data=ld<u32>(b);st<u32>(b,4,0x1005859C);st<u8>(data,0,0);st<u32>(b,4,0x100585B4);}
 if(direction==0)st<u8>(a,0x14,ld<u8>(0x1048CEC4));}
 return p(a);
}
VERIFY(0x025E2450,dvd_ram_ctor);
BOOL dvd_ram_execute(void* self){
 WWHD_FUNC(0x025E26A8,BOOL,self);
 u32 a=gabi::ea(self),heap=ld<u32>(a,0x120);if(!heap)heap=gabi::call<u32>(0x025E3268);
 u32 direction=ld<u8>(a,0x14)==0?1:2;gabi::Local<Path> local;gabi::call<void>(0xC000A848,local.get(),p(a+0x15),0x101);
 u32 data=gabi::call<u32>(0x027EBB2C,local.get(),0,1,0,p(heap),direction,0,0,0);st<u32>(a,0x118,data);
 if(data){u32 size=gabi::call<u32>(0x027EC134,p(heap),p(data));st<u32>(a,0x11C,size);}u32 result=ld<u32>(a,0x118);st<u8>(a,0xC,1);return result!=0;
}
VERIFY(0x025E26A8,dvd_ram_execute);
void* dvd_param_ctor(void* self){
 WWHD_FUNC(0x025E276C,void*,self);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0x84);
 if(a){gabi::call<void>(0x028F521C,p(a),0x3C);gabi::call<void>(0x028F521C,p(a+0x3C),0x10);gabi::call<void>(0x028F521C,p(a+0x4C),0xC);gabi::call<void>(0x028F521C,p(a+0x58),0x2C);gabi::call<void>(0xC0009D68,p(a+0x58));gabi::call<void>(0x02010008,p(a+0x4C));}return p(a);
}
VERIFY(0x025E276C,dvd_param_ctor);
void dvd_cut(void* self,void* command){
 WWHD_FUNC(0x025E27EC,void,self,command);
 u32 a=gabi::ea(self);gabi::call<void>(0xC0009E40,p(a+0x58));gabi::call<void>(0x0200FDF4,command);gabi::call<void>(0xC000A1C8,p(a+0x58));gabi::call<void>(0x025E230C,self);
}
VERIFY(0x025E27EC,dvd_cut);
void* dvd_first(void* self){
 WWHD_FUNC(0x025E2840,void*,self);
 return p(ld<u32>(gabi::ea(self),0x4C));
}
VERIFY(0x025E2840,dvd_first);
void dvd_drain(void* self){
 WWHD_FUNC(0x025E2848,void,self);
 gabi::Local<u32> command;u32 next=gabi::call<u32>(0x025E2840,self);st<u32>(command.a,0,next);
 while(next){gabi::call<void>(0x025E27EC,self,p(next));gabi::call<void>(0x025E21E8,command.get());next=gabi::call<u32>(0x025E2840,self);st<u32>(command.a,0,next);}
}
VERIFY(0x025E2848,dvd_drain);
void dvd_dtor_25E29FC(void* self,s32 flag){
 WWHD_FUNC(0x025E29FC,void,self,flag);
 if(self&&(u32(flag)&1))gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x025E29FC,dvd_dtor_25E29FC);
void dvd_dtor_25E2A28(void* self,s32 flag){
 WWHD_FUNC(0x025E2A28,void,self,flag);
 if(self&&(u32(flag)&1))gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x025E2A28,dvd_dtor_25E2A28);
void dvd_dtor_25E2A3C(void* self,s32 flag){
 WWHD_FUNC(0x025E2A3C,void,self,flag);
 if(self&&(u32(flag)&1))gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x025E2A3C,dvd_dtor_25E2A3C);
void dvd_dtor_25E2A54(void* self,s32 flag){
 WWHD_FUNC(0x025E2A54,void,self,flag);
 if(self&&(u32(flag)&1))gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x025E2A54,dvd_dtor_25E2A54);
void dvd_dtor_25E2A78(void* self,s32 flag){
 WWHD_FUNC(0x025E2A78,void,self,flag);
 if(self&&(u32(flag)&1))gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x025E2A78,dvd_dtor_25E2A78);
void dvd_terminate(void* self){
 WWHD_FUNC(0x025E2A10,void,self);
 u32 a=gabi::ea(self),data=ld<u32>(a),size=ld<u32>(a,8);st<u8>(data+size-1,0,0);
}
VERIFY(0x025E2A10,dvd_terminate);
void dvd_view(void* self){
 WWHD_FUNC(0x025E2A50,void,self);
}
VERIFY(0x025E2A50,dvd_view);
s32 dvd_thread_entry(void* self){
 WWHD_FUNC(0x025E2A68,s32,self);
 u32 a=gabi::ea(self),vt=ld<u32>(a,0xC),target=ld<u32>(vt,0x4C);
 return gabi::call_ptr<s32>(target,self,gabi::cpu->r[4],gabi::cpu->r[5],gabi::cpu->r[6],gabi::cpu->r[7],gabi::cpu->r[8],gabi::cpu->r[9],gabi::cpu->r[10]);
}
VERIFY(0x025E2A68,dvd_thread_entry);
void dvd_global_drain(){
 WWHD_FUNC(0x025E2A8C,void);
 gabi::call<void>(0x025E2848,p(0x1048CE40));
}
VERIFY(0x025E2A8C,dvd_global_drain);
void dvd_thread_dtor(void* self,s32 flag){
 WWHD_FUNC(0x025E2A98,void,self,flag);
 u32 a=gabi::ea(self);if(a){st<u32>(a,0x18,0x1005858C);gabi::call<void>(0x027609AC,self,0);if(u32(flag)&1)gabi::call<void>(0x0273AF40,self);}
}
VERIFY(0x025E2A98,dvd_thread_dtor);
struct DvdView {u32 data,vt;};
void* dvd_ram_create(void* input,s32 direction,void* heap){
 WWHD_FUNC(0x025E255C,void*,input,direction,heap);
 u32 h=gabi::call<u32>(0x025EE060),a=gabi::call<u32>(0x0273B050,0x230,p(h),-4);
 if(!a)return nullptr;
 a=gabi::call<u32>(0x025E2450,p(a),direction);if(!a)return nullptr;
 gabi::Local<DvdView> view;st<u32>(view.a,0,gabi::ea(input));st<u32>(view.a,4,0x1005855C);
 u32 buffer=ld<u32>(a,0x124);gabi::call<void>(0x025E2A50,view.get());
 u32 cursor=ld<u32>(view.a);s32 length=0;
 if(ld<u8>(cursor)){do{length++;cursor++;if(length>0x40000){length=0;break;}}while(ld<u8>(cursor));}
 s32 capacity=ld<s32>(a,0x12C);u32 vt=ld<u32>(view.a,4);if(length>=capacity)length=s32(u32(capacity)-1);
 u32 target=ld<u32>(vt,0x14);
 gabi::call_ptr<void>(target,view.get(),gabi::cpu->r[4],gabi::cpu->r[5],gabi::cpu->r[6],gabi::cpu->r[7],gabi::cpu->r[8],gabi::cpu->r[9],gabi::cpu->r[10]);
 u32 data=ld<u32>(view.a);gabi::call<void>(0xC0009988,p(buffer),p(data),length,0);st<u8>(buffer+u32(length),0,0);
 gabi::call<void>(0x025E21A8,p(a+0x15),input);
 if(!ld<u8>(a,0x15)){u32 table=ld<u32>(a,0x10);st<u8>(a,0xC,1);target=ld<u32>(table,0xC);gabi::call_ptr<void>(target,p(a),3,gabi::cpu->r[5],gabi::cpu->r[6],gabi::cpu->r[7],0,1,table);return nullptr;}
 st<u32>(a,0x120,gabi::ea(heap));gabi::call<void>(0x025E232C,p(0x1048CE40),p(a));return p(a);
}
VERIFY(0x025E255C,dvd_ram_create);
void dvd_thread_create(){
 WWHD_FUNC(0x025E28B0,void);
 u32 heap=ld<u32>(0x101F8B9C),a=gabi::call<u32>(0x0273AD10,0x94);
 if(a){gabi::Local<DvdView> view;st<u32>(view.a,0,0x10058674);st<u32>(view.a,4,0x1005855C);gabi::call<void>(0x027607C0,p(a),view.get(),0,p(heap),0,0x7FFFFFFF,0x5000,0x20);st<u32>(a,0xC,0x100585CC);}
 st<u32>(0x1048CE3C,0,a);u32 vt=ld<u32>(a,0xC),target=ld<u32>(vt,0x2C);
 gabi::call_ptr<void>(target,p(a),gabi::cpu->r[4],gabi::cpu->r[5],gabi::cpu->r[6],gabi::cpu->r[7],gabi::cpu->r[8],gabi::cpu->r[9],vt);
}
VERIFY(0x025E28B0,dvd_thread_create);
// Adjacent startup initializer; attribution by ordering and param-constructor call.
void dvd_startup(){
 WWHD_FUNC(0x025E295C,void);
 st<u32>(0x1048CE2C,8,0);st<u32>(0x1048CE2C,0,0);st<u32>(0x1048CE2C,12,0);st<u32>(0x1048CE2C,4,0);gabi::call<void>(0x028F026C,p(0x101F472C));
 f32 lo=ld<f32>(0x10058680),hi=ld<f32>(0x10058684);st<f32>(0x1048CE20,0,lo);st<f32>(0x1048CE24,0,hi);
 gabi::call<void>(0x028ED6F8,p(0x1048CE28));gabi::call<void>(0x028F026C,p(0x101F4738));gabi::call<void>(0x028EAB2C,p(0x1048CE29));gabi::call<void>(0x028F026C,p(0x101F4744));gabi::call<void>(0x025E276C,p(0x1048CE40));
}
VERIFY(0x025E295C,dvd_startup);
