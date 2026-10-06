#include "wwhd.h"
#include "gabi.h"
#include "bindings.h"
namespace room { template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);} template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);} void* p(u32 a){return gabi::at<void>(a);} u32 status(u32 number){return 0x1047E6CC+number*0x22C;} }
using namespace room;
BOOL room_execute(void* self){
 WWHD_FUNC(0x025B39F8,BOOL,self);
 u32 a=gabi::ea(self),number=ld<u32>(a,0xB0);gabi::call<void>(0x025200D4);u32 s=status(number);u8 flags=ld<u8>(s,0x21C);
 if(flags&4){gabi::call<void>(0x025DC8D0,self);return 1;}
 gabi::call<void>(0x025200D4);flags=ld<u8>(s,0x21C);
 if(flags&2){gabi::call<void>(0x025200D4);flags=ld<u8>(s,0x21C);st<u8>(s,0x21C,u8(flags&0xFD));gabi::call<void>(0x025200D4);flags=ld<u8>(s,0x21C);st<u8>(s,0x21C,u8(flags|1));}
 else gabi::call<void>(0x025B37EC,self);return 1;
}
VERIFY(0x025B39F8,room_execute);
BOOL room_is_delete(void* self){
 WWHD_FUNC(0x025B3A9C,BOOL,self);
 return 1;
}
VERIFY(0x025B3A9C,room_is_delete);
s32 room_phase0(void* self){
 WWHD_FUNC(0x025B3D18,s32,self);
 u32 a=gabi::ea(self),number=ld<u32>(a,0xB0),id=0xFFFFFFFFu;if(a)id=ld<u32>(a,4);
 st<u32>(0x1047E8F0+number*0x22C,0,id);gabi::call<void>(0x028F17A8,p(a+0x1E1),8,p(0x1005351C),number);return 2;
}
VERIFY(0x025B3D18,room_phase0);
s32 room_phase1(void* self){
 WWHD_FUNC(0x025B3D70,s32,self);
 s32 result=gabi::call<s32>(0x02523A04,p(gabi::ea(self)+0x1E1),0);
 if(result)return 2;gabi::call<void>(0x025C3ED0);return 5;
}
VERIFY(0x025B3D70,room_phase1);
s32 room_create(void* self){
 WWHD_FUNC(0x025B435C,s32,self);
 return gabi::call<s32>(0x02525FE4,p(gabi::ea(self)+0x1C8),p(0x101EAD1C),self);
}
VERIFY(0x025B435C,room_create);
void room_startup(){
 WWHD_FUNC(0x025B4370,void);
 st<u32>(0x1047C8D4,12,0);st<u32>(0x1047C8D4,8,0);st<u32>(0x1047C8D4,4,0);st<u32>(0x1047C8D4,0,0);gabi::call<void>(0x028F026C,p(0x101EAD30));f32 low=ld<f32>(0x100535BC),high=ld<f32>(0x100535C0);st<f32>(0x1047C8B8,0,low);st<f32>(0x1047C8BC,0,high);gabi::call<void>(0x028ED6F8,p(0x1047C8D0));gabi::call<void>(0x028F026C,p(0x101EAD3C));gabi::call<void>(0x028EAB2C,p(0x1047C8D1));gabi::call<void>(0x028F026C,p(0x101EAD48));high=ld<f32>(0x100535C4);low=ld<f32>(0x100535C8);st<f32>(0x1047C8C0,0,high);st<f32>(0x1047C8C8,0,low);st<f32>(0x1047C8C4,0,high);st<f32>(0x1047C8CC,0,low);
}
VERIFY(0x025B4370,room_startup);

namespace room {
struct View {be<u32> data,vt;};
struct Buffer {be<u32> data,vt,size;u8 bytes[32];};
bool equal(u32 first,u32 second) {
 if(first==second)return true;
 for(u32 n=0;n<0x40001;++n){u8 c=ld<u8>(first+n);if(c!=ld<u8>(second+n))return false;if(!c)return true;}
 return false;
}
u32 invoke(u32 object,u32 offset){u32 target=ld<u32>(ld<u32>(object),offset);return gabi::call_ptr<u32>(target,p(object));}
// SafeString's data precedes its vtable; each local has the real eight-byte size.
bool stageEqual(u32 text) {
 gabi::Local<View> first,second;first->data=text;first->vt=0x100534B4;
 u32 game=gabi::call<u32>(0x025200D4);second->data=game+0x5134;second->vt=0x100534B4;
 gabi::call_ptr<void>(ld<u32>(first->vt,0x14),first.get());
 gabi::call_ptr<void>(ld<u32>(first->vt,0x14),first.get());
 u32 captured=first->data;
 gabi::call_ptr<void>(ld<u32>(second->vt,0x14),second.get());
 if(captured==second->data)return true;
 return equal(first->data,second->data);
}
}
void room_object_set(void* self){
 WWHD_FUNC(0x025B37EC,void,self);
 u32 a=gabi::ea(self),number=ld<u32>(a,0xB0);gabi::call<void>(0x025200D4);
 u32 s=status(number);bool set=ld<u8>(a,0x1DF)!=0;u32 hidden=ld<u8>(s,0x21C)&8;
 if(!set){if(!hidden){gabi::call<void>(0x025D5834,0x1B7,number,0,-1,0,0,-1,0);u32 data=ld<u32>(a,0x1D0),dt=ld<u32>(a,0x1D4);gabi::call<void>(0x025C2E8C,p(data),p(dt),number);st<u8>(a,0x1DF,1);st<u8>(a,0x1E9,1);}return;}
 if(ld<u8>(a,0x1E9)){
  if(number){gabi::call<void>(0x02039E4C,p(ld<u32>(0x1018F4AC)));u32 player=ld<u32>(0x1018F450);st<u8>(player,0x30D0,0);st<u8>(player,0x30AC,0);}
  if(stageEqual(0x10053514)){for(s32 n=0;n<2;++n)gabi::call<void>(0x02037D64,p(ld<u32>(0x1018F450)),number);gabi::call<void>(0x02037230,p(ld<u32>(0x1018F450)),number);}
  st<u8>(a,0x1E9,0);
 }
 if(hidden){gabi::call<void>(0x025DEE20,p(a+0xC0),p(0x025B37C8),0);gabi::call<void>(0x025200D4);s32 zone=ld<s8>(s,0x21F);u32 save=ld<u32>(0x101F84DC);gabi::call<void>(0x025B93B0,p(save+0x7C8+u32(zone*0x4C)+2));st<u8>(a,0x1DF,0);}
}
VERIFY(0x025B37EC,room_object_set);
BOOL room_delete(void* self){
 WWHD_FUNC(0x025B3AA4,BOOL,self);
 u32 a=gabi::ea(self),number=ld<u32>(a,0xB0);
 if(ld<u8>(a,0x1DE)){u32 game=gabi::call<u32>(0x025200D4);if(!gabi::call<u32>(0x025A7EAC,p(ld<u32>(game,0x5AB0)),0,number))return 0;}
 if(ld<u8>(a,0x1E0))gabi::call<void>(0x0258F1D8,ld<u32>(a,0xB0));
 gabi::call<void>(0x02524180,p(a+0x1E1));gabi::call<void>(0x025200D4);st<u8>(status(number),0x21C,0);
 u32 game=gabi::call<u32>(0x025200D4);u32 dt=gabi::call<u32>(0x025C11DC,p(game+0x51CC),number);invoke(dt,0xC);
 game=gabi::call<u32>(0x025200D4);
 if(!ld<s8>(game,0x514C)){
  if(ld<u16>(a,0x1DC)==1)gabi::call<void>(0x025B8B68,p(ld<u32>(0x101F84DC)+0x644),0x1A80);
  if(stageEqual(0x10053518))gabi::call<void>(0x025B8B7C,p(ld<u32>(0x101F84DC)+0x1178),0x320);
 }
 if(ld<u16>(a,0x1DC)==2)gabi::call<void>(0x025B8B68,p(ld<u32>(0x101F84DC)+0x644),0x1C40);
 if(stageEqual(0x10053518))st<u8>(0x101D5F40,0,0);
 return 1;
}
VERIFY(0x025B3AA4,room_delete);
s32 room_phase2(void* self){
 WWHD_FUNC(0x025B3DBC,s32,self);
 u32 a=gabi::ea(self);s32 result=gabi::call<s32>(0x02523D08,p(a+0x1E1));
 if(result>0)return 0;if(result<0){gabi::call<void>(0x025C3ED0);return 5;}
 u32 number=ld<u32>(a,0xB0);gabi::call<void>(0x025200D4);u32 s=status(number);
 if(ld<s8>(s,0x21F)<0){u32 zone=gabi::call<u32>(0x025B9DD8,p(ld<u32>(0x101F84DC)+0x20),number);gabi::call<void>(0x025200D4);st<u8>(s,0x21F,u8(zone));}
 u32 game=gabi::call<u32>(0x025200D4);u32 dt=gabi::call<u32>(0x025C11DC,p(game+0x51CC),number);st<u32>(a,0x1D4,dt);st<u8>(dt,0x4C,u8(number));
 u32 data=gabi::call<u32>(0x0252447C,p(a+0x1E1),p(0x1005354C));st<u32>(a,0x1D0,data);
 if(data){gabi::call<void>(0x025C2DFC,p(data),p(ld<u32>(a,0x1D4)));
  if(!ld<u8>(0x1047E6B8)){game=gabi::call<u32>(0x025200D4);dt=gabi::call<u32>(0x025C11DC,p(game+0x51CC),number);u32 lbnk=invoke(dt,0x21C);
   if(lbnk){u32 banks=ld<u32>(lbnk,4);if(banks){u32 layer=gabi::call<u32>(0x025250C0,number);u32 bank=ld<u8>(banks+layer);
    if(bank!=0xFF){if(bank>=100)gabi::call<void>(0x0273AA24,p(0x1005353C),0x238,p(0x10053524));gabi::call<void>(0x028F17A8,p(0x1047E6B8),8,p(0x10053558),bank);if(!gabi::call<u32>(0x02520354,p(0x1047E6B8),0,0))st<u8>(0x1047E6B8,0,0);}
   }}
  }
 }
 return 2;
}
VERIFY(0x025B3DBC,room_phase2);
s32 room_phase3(void* self){
 WWHD_FUNC(0x025B3F90,s32,self);
 u32 a=gabi::ea(self);if(ld<u8>(0x1047E6B8)){s32 result=gabi::call<s32>(0x025203D8,p(0x1047E6B8));if(result>0)return 0;if(result<0){gabi::call<void>(0x025C3ED0);return 5;}}
 u32 number=ld<u32>(a,0xB0),game=gabi::call<u32>(0x025200D4);u32 dt=gabi::call<u32>(0x025C11DC,p(game+0x51CC),number);u32 file=invoke(dt,0x1DC);
 if(!file)gabi::call<void>(0x0273AA24,p(0x10053574),0x27D,p(0x10053564));
 u32 particle=(ld<u32>(file)>>21)&0xFF;game=gabi::call<u32>(0x025200D4);u32 ready=gabi::call<u32>(0x025A7D8C,p(ld<u32>(game,0x5AB0)),particle,p(a+0x1D8));st<u8>(a,0x1DE,u8(ready));
 gabi::Local<View> first,second;first->data=0x10053584;first->vt=0x100534B4;second->data=0x10053590;second->vt=0x100534B4;
 u32 resource=gabi::call<u32>(0x026124B0,p(ld<u32>(0x101F4F7C)),first.get(),second.get(),0);
 if(resource){u32 archive=gabi::call<u32>(0x027E2DC0,p(resource));gabi::Local<Buffer> buffer;buffer->data=gabi::ea(buffer.get())+12;buffer->vt=0x100534FC;buffer->size=32;st<u8>(gabi::ea(buffer.get()),12,0);st<u8>(gabi::ea(buffer.get()),43,0);
  gabi::call<void>(0x02759C28,buffer.get(),p(0x100535A0),particle);gabi::call_ptr<void>(ld<u32>(buffer->vt,0x14),buffer.get());
  u32 relative=ld<u32>(archive,0x4C),root=relative?archive+0x4C+relative:0;u32 entry=gabi::call<u32>(0x027DFA24,p(root),p(buffer->data));
  if(entry){relative=ld<u32>(entry);u32 contents=relative?entry+relative:0;game=gabi::call<u32>(0x025200D4);gabi::call<void>(0x025A7C30,p(ld<u32>(game,0x5AB0)),p(contents));}
 }
 gabi::call<void>(0x025B37EC,self);return 2;
}
VERIFY(0x025B3F90,room_phase3);
s32 room_phase4(void* self){
 WWHD_FUNC(0x025B419C,s32,self);
 u32 a=gabi::ea(self),game=gabi::call<u32>(0x025200D4);if(!ld<u32>(game,0x5B2C))return 0;
 u32 number=ld<u32>(a,0xB0);if(gabi::call<u32>(0x02520C0C,0x6A))st<u16>(a,0x1DC,1);
 if(stageEqual(0x100535B0)&&gabi::call<u32>(0x02520C0C,0x6B))st<u16>(a,0x1DC,2);
 if(number==0xD){gabi::call<void>(0x025B8AF4,p(ld<u32>(0x101F84DC)+0x644),0xB8FF,0);if(gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x644),0x3F02))gabi::call<void>(0x025B8B68,p(ld<u32>(0x101F84DC)+0x644),0x1580);}
 return 4;
}
VERIFY(0x025B419C,room_phase4);

/* 025B4434 this TU's sead::SafeString copy (vtable 100534B4): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void SafeString_deletingDtor_d_s_room(u32 p, u32 flags) {
    WWHD_FUNC(0x025B4434, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x025B4434, SafeString_deletingDtor_d_s_room);

/* 025B4448 this TU's sead::FixedSafeString<N>::assureTerminationImpl_ (vtables 100534CC.., 100534FC)
 *: terminates the buffer at its last byte */
static void FixedSafeString_assureTerminationImpl_d_s_room(u32 p) {
    WWHD_FUNC(0x025B4448, void, p);
    u32 buf = gabi::load<u32>(p);
    u32 size = gabi::load<u32>(p + 8);
    gabi::store<u8>(buf + size - 1, 0);
}
VERIFY(0x025B4448, FixedSafeString_assureTerminationImpl_d_s_room);

/* 025B4460 this TU's FixedSafeString copy (vtable slot 100534F0): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void FixedSafeString_deletingDtor_d_s_room_a(u32 p, u32 flags) {
    WWHD_FUNC(0x025B4460, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x025B4460, FixedSafeString_deletingDtor_d_s_room_a);

/* 025B4474 this TU's sead::SafeString copy assureTerminationImpl_ (vtable 100534B4): empty function */
static void SafeString_assureTerminationImpl_d_s_room(u32 p) {
    WWHD_FUNC(0x025B4474, void, p);
}
VERIFY(0x025B4474, SafeString_assureTerminationImpl_d_s_room);

/* 025B4478 this TU's FixedSafeString copy (vtable slot 100534D8): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void FixedSafeString_deletingDtor_d_s_room_b(u32 p, u32 flags) {
    WWHD_FUNC(0x025B4478, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x025B4478, FixedSafeString_deletingDtor_d_s_room_b);

/* 025B448C this TU's FixedSafeString copy (vtable 100534FC, slot 10053508): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void FixedSafeString_deletingDtor_d_s_room_c(u32 p, u32 flags) {
    WWHD_FUNC(0x025B448C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x025B448C, FixedSafeString_deletingDtor_d_s_room_c);
