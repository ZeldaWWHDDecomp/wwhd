#include "wwhd.h"
#include "gabi.h"
namespace pcbase {
template<class T> T load(u32 a,u32 o=0){return gabi::load<T>(a+o);}
template<class T> void put(u32 a,u32 o,T v){gabi::store<T>(a+o,v);}
void* p(u32 a){return gabi::at<void>(a);}
}
using namespace pcbase;
BOOL pcbase_same(s32 a,s32 b){
 WWHD_FUNC(0x025DD258,BOOL,a,b);
 return a==b;
}
VERIFY(0x025DD258,pcbase_same);
u32 pcbase_type(void* type){
 WWHD_FUNC(0x025DD268,u32,type);
 u32 a=gabi::ea(type), v=load<u32>(a);
 if(!v){v=load<u32>(0x101F3908)+1;put<u32>(0x101F3908,0,v);put<u32>(a,0,v);}
 return v;
}
VERIFY(0x025DD268,pcbase_type);
u32 pcbase_id(){
 WWHD_FUNC(0x025DD290,u32);
 u32 old=load<u32>(0x101F390C), next=old;
 do {next++;} while(next==0xFFFFFFFEu || next==0xFFFFFFFFu);
 put<u32>(0x101F390C,0,next);return old;
}
VERIFY(0x025DD290,pcbase_id);
BOOL pcbase_execute(void* proc){
 WWHD_FUNC(0x025DD2B8,BOOL,proc);
 u32 a=gabi::ea(proc),saved=gabi::call<u32>(0x025DED64);
 u32 layer=load<u32>(a,0x2C);gabi::call<void>(0x025DEAB4,p(layer));
 u32 methods=load<u32>(a,0xA8);BOOL result=gabi::call<BOOL>(0x025DFCC4,p(methods),proc);
 gabi::call<void>(0x025DEAB4,p(saved));return result;
}
VERIFY(0x025DD2B8,pcbase_execute);
void pcbase_delete_append(void* proc){
 WWHD_FUNC(0x025DD314,void,proc);
 u32 a=gabi::ea(proc), data=load<u32>(a,0xAC);
 if(data){gabi::call<void>(0x0201945C,p(data));put<u32>(a,0xAC,0);}
}
VERIFY(0x025DD314,pcbase_delete_append);
BOOL pcbase_is_delete(void* proc){
 WWHD_FUNC(0x025DD354,BOOL,proc);
 u32 a=gabi::ea(proc), saved=gabi::call<u32>(0x025DED64);
 u32 layer=load<u32>(a,0x2C);gabi::call<void>(0x025DEAB4,p(layer));
 u32 methods=load<u32>(a,0xA8);BOOL result=gabi::call<BOOL>(0x025DFCCC,p(methods),proc);
 gabi::call<void>(0x025DEAB4,p(saved));return result;
}
VERIFY(0x025DD354,pcbase_is_delete);
BOOL pcbase_delete(void* proc){
 WWHD_FUNC(0x025DD3B0,BOOL,proc);
 u32 a=gabi::ea(proc),methods=load<u32>(a,0xA8);
 BOOL result=gabi::call<BOOL>(0x025DFCD4,p(methods),proc);
 if(result==1){gabi::call<void>(0x025DD314,proc);put<u32>(a,0,0);gabi::call<void>(0x0201945C,proc);}
 return result;
}
VERIFY(0x025DD3B0,pcbase_delete);
void* pcbase_create(s16 name,u32 id,void* data){
 WWHD_FUNC(0x025DD414,void*,name,id,data);
 u32 profile=gabi::call<u32>(0x025E10E0,name);
 u32 first=load<u32>(profile,0x10), second=0;
 if(!(first&3))second=load<u32>(profile,0x14);
 if((first&3)||(second&3)){
  gabi::call<void>(0x0273AA24,p(0x10057DF4),0x1E4,p(0x10057E04));
  first=load<u32>(profile,0x10);second=load<u32>(profile,0x14);
 }
 u32 size=first+second, a=gabi::call<u32>(0x02019430,-4,size);
 if(!a)return nullptr;
 gabi::call<void>(0x0201B680,p(a),size);
 u32 layer=load<u32>(profile);gabi::call<void>(0x025DF178,p(a+0x18),layer,p(a));
 gabi::call<void>(0x025DF7A8,p(a+0x34),p(a));
 gabi::call<void>(0x025DDEEC,p(a+0x4C),p(a));
 u32 list=load<u16>(profile,4);layer=load<u32>(profile);u32 priority=load<u16>(profile,6);
 gabi::call<void>(0x025E0FB0,p(a+0x68),p(a),layer,list,priority);
 put<u32>(a,4,id);put<s16>(a,0xE,name);put<u8>(a,0xC,0);put<u8>(a,0xA,0);
 u32 type=gabi::call<u32>(0x025DD268,p(0x101F3934));put<u32>(a,0,type);
 put<s16>(a,8,load<s16>(profile,8));gabi::call<void>(0x025E0C1C,p(a));
 u32 methods=load<u32>(profile,0xC);put<u32>(a,0x10,profile);put<u32>(a,0xA8,methods);
 put<u32>(a,0xAC,gabi::ea(data));put<u32>(a,0xB0,load<u32>(profile,0x18));return p(a);
}
VERIFY(0x025DD414,pcbase_create);
s32 pcbase_subcreate(void* proc){
 WWHD_FUNC(0x025DD538,s32,proc);
 u32 a=gabi::ea(proc),methods=load<u32>(a,0xA8);
 u32 phase=gabi::call<u32>(0x025DFCDC,p(methods),proc);
 if(phase==2||phase==4){gabi::call<void>(0x025DD314,proc);put<u8>(a,0xD,2);return 2;}
 if(phase<2){put<u8>(a,0xC,1);put<u8>(a,0xD,0);return 0;}
 if(phase==3){put<u8>(a,0xD,3);return 3;}
 put<u8>(a,0xD,5);return 5;
}
VERIFY(0x025DD538,pcbase_subcreate);
void* pcbase_ctor(void* self){
 WWHD_FUNC(0x025DD5F0,void*,self);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0xB8);
 if(a)put<u32>(a,0xB4,0x10057E58);return p(a);
}
VERIFY(0x025DD5F0,pcbase_ctor);
void pcbase_dtor(void* self,s32 flag){
 WWHD_FUNC(0x025DD630,void,self,flag);
 if(self&&(u32(flag)&1))gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x025DD630,pcbase_dtor);
// Startup registration associated with this unit by adjacency, not a matched name.
void pcbase_startup(){
 WWHD_FUNC(0x025DD644,void);
 put<u32>(0x1048A6AC,8,0);put<u32>(0x1048A6AC,0,0);
 put<u32>(0x1048A6AC,12,0);put<u32>(0x1048A6AC,4,0);
 gabi::call<void>(0x028F026C,p(0x101F3910));
 f32 lower=load<f32>(0x10057E4C),upper=load<f32>(0x10057E50);
 put<f32>(0x1048A6A0,0,lower);put<f32>(0x1048A6A4,0,upper);
 gabi::call<void>(0x028ED6F8,p(0x1048A6A8));gabi::call<void>(0x028F026C,p(0x101F391C));
 gabi::call<void>(0x028EAB2C,p(0x1048A6A9));gabi::call<void>(0x028F026C,p(0x101F3928));
}
VERIFY(0x025DD644,pcbase_startup);
