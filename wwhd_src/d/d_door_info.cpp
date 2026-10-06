#include "bindings.h"
namespace doorinfo {
template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);}template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);}void* p(u32 a){return gabi::at<void>(a);}struct Vec{be<f32>x,y,z;};
u32 status(u32 n){return 0x1047E6CC+n*0x22C;}u32 trig(u32 angle){return 0x104A44F8+((angle&0xFFFF)>>3)*8;}
}
using namespace doorinfo;
/* 0252A244 dDoor_info_c::dDoor_info_c: the out-of-line
 * inline constructor the door actors call (daDoor10/12, daKddoor, daKnob00 create): allocates
 * 0x3EC bytes when this == NULL, runs the fopAc_ac_c constructor, stores the HD vtable
 * (1004C564) and clears the door state (room numbers, side flag, two zeroed blocks, demo index). */
u32 dDoor_info_ct(u32 self){
 WWHD_FUNC(0x0252A244,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,u32(0x3EC));if(!self)return 0;}
 gabi::call<void>(0x025D4ED0,self);
 st<u8>(self,0x3AE,0);st<u8>(self,0x3BD,0);st<u32>(self,0xB4,0x1004C564);st<u8>(self,0x3AD,0);st<u8>(self,0x3BC,0);st<u8>(self,0x3AC,0);
 gabi::call<void>(0x028F521C,self+0x3BE,u32(0x18));
 gabi::call<void>(0x028F521C,self+0x3D6,u32(0xC));
 st<u32>(self,0x3E4,0);st<u8>(self,0x3E8,0);st<u8>(self,0x3E2,0);
 return self;
}
VERIFY(0x0252A244,dDoor_info_ct);
void door_open_demo(void* self,s32 mode){
 WWHD_FUNC(0x0252A2DC,void,self,mode);
 u32 a=gabi::ea(self);if(ld<u8>(a,0x3E2)!=9)st<u8>(0x1047A964,0,1);
 u8 side=ld<u8>(a,0x3BC);s32 angle=ld<s16>(a,0x322);st<u16>(a,0x32A,u16(angle));if(side==1)st<u16>(a,0x32A,u16(angle+0x7FFF));
 u32 game=gabi::call<u32>(0x025200D4);u32 index=gabi::call<u32>(0x02542D88,p(game+0x52C4),p(0x1004C574),0,0);st<u32>(a,0x3E4,index);
 if(mode){game=gabi::call<u32>(0x025200D4);u16 flags=ld<u16>(game,0x52B8);st<u16>(game,0x52B8,u16(flags|2));}
}
VERIFY(0x0252A2DC,door_open_demo);
u32 door_front_room(void* self){WWHD_FUNC(0x0252A37C,u32,self);return u32(ld<s16>(gabi::ea(self),0x2F8))&63;}
VERIFY(0x0252A37C,door_front_room);
u32 door_back_room(void* self){WWHD_FUNC(0x0252A388,u32,self);return (u32(ld<s16>(gabi::ea(self),0x2F8))>>6)&63;}
VERIFY(0x0252A388,door_back_room);
u32 door_aux_room(void* self){WWHD_FUNC(0x0252A394,u32,self);return u32(ld<s16>(gabi::ea(self),0x2FC))&63;}
VERIFY(0x0252A394,door_aux_room);
void door_open_init(void* self,s32 mode){
 WWHD_FUNC(0x0252A3A0,void,self,mode);
 u32 a=gabi::ea(self),first,second;
 if(!ld<u8>(a,0x3BC)){first=gabi::call<u32>(0x0252A37C,self);st<u8>(a,0x3AD,u8(first));second=gabi::call<u32>(0x0252A388,self);}
 else{first=gabi::call<u32>(0x0252A388,self);st<u8>(a,0x3AD,u8(first));second=gabi::call<u32>(0x0252A37C,self);}
 first=ld<u8>(a,0x3AD);st<u8>(a,0x3AE,u8(second));
 if(first!=second&&first!=63&&second!=63){gabi::call<void>(0x025200D4);u32 s=status(second);u8 flags=ld<u8>(s,0x21C);st<u8>(s,0x21C,u8(flags&0xF7));u32 env=gabi::call<u32>(0x02555D0C);st<u8>(env,0x10A3,ld<u8>(a,0x3AE));}
 if(mode){u32 room=gabi::call<u32>(0x0252A394,self);if(room!=63)gabi::call<void>(0x025C23E0,room,ld<u8>(a,0x3AE));}
}
VERIFY(0x0252A3A0,door_open_init);
void door_open_proc(void* self){
 WWHD_FUNC(0x0252A48C,void,self);
 u32 a=gabi::ea(self),game=gabi::call<u32>(0x025200D4);s32 angle=ld<s16>(a,0x32A);f32 distance=ld<f32>(0x1004C584);u32 player=ld<u32>(game,0x5B2C);f32 x=ld<f32>(a,0x314),z=ld<f32>(a,0x31C);u32 t=trig(u32(angle+0x7FFF));f32 sn=ld<f32>(t),cs=ld<f32>(t,4);
 f32 gx=gabi::fmadds(sn,distance,x);f32 px=ld<f32>(player,0x314),ratio=ld<f32>(0x1004C58C);f32 gz=gabi::fmadds(cs,distance,z);gabi::Local<Vec> out;out->x=px;f32 mixx=gabi::fmuls_ppc(gx,ratio);f32 py=ld<f32>(player,0x318),other=ld<f32>(0x1004C588);f32 mixz=gabi::fmuls_ppc(gz,ratio);out->y=py;f32 pz=ld<f32>(player,0x31C);out->x=gabi::fmadds(px,other,mixx);out->z=gabi::fmadds(pz,other,mixz);
 u32 vt=ld<u32>(player,0xB4),target=ld<u32>(vt,0x114);s32 yaw=ld<s16>(player,0x322);gabi::call_ptr<void>(target,p(player),out.get(),yaw);
}
VERIFY(0x0252A48C,door_open_proc);
void door_close_end(void* self){
 WWHD_FUNC(0x0252A550,void,self);
 u32 a=gabi::ea(self);u32 back=ld<u8>(a,0x3AE),front=ld<u8>(a,0x3AD);
 if(front!=back&&front!=63&&back!=63){gabi::call<void>(0x025200D4);u32 s=status(front);st<u8>(s,0x21C,u8(ld<u8>(s,0x21C)|8));}
 u32 game=gabi::call<u32>(0x025200D4),player=ld<u32>(game,0x5B2C);gabi::Local<Vec> delta,out;gabi::call<void>(0x0201ADE0,p(player+0x314),delta.get(),p(a+0x314));
 f32 dz=delta->z,sz=ld<f32>(a,0x3B8),dx=delta->x;f32 product=gabi::fmuls_ppc(dz,sz);f32 sx=ld<f32>(a,0x3B0),negative=ld<f32>(0x1004C594);f32 dot=gabi::fmadds(dx,sx,product);f32 positive=ld<f32>(0x1004C598),zero=ld<f32>(0x1004C590);f32 distance=dot>=0?negative:positive;
 f32 x=ld<f32>(a,0x314),z=ld<f32>(a,0x31C),y=ld<f32>(a,0x318);out->y=y;out->x=gabi::fnmsubs(distance,sx,x);out->z=gabi::fnmsubs(distance,sz,z);
 s32 room,yaw;u32 save;
 if(dot>zero){room=ld<s8>(player,0x326);save=ld<u32>(0x101F84DC);yaw=ld<s16>(a,0x322);}
 else{yaw=s16(ld<s16>(a,0x322)+0x8000);room=ld<s8>(player,0x326);save=ld<u32>(0x101F84DC);}
 gabi::call<void>(0x025B9810,p(save+0x1148),out.get(),yaw,room);
}
VERIFY(0x0252A550,door_close_end);
s32 door_demo_action(void* self){
 WWHD_FUNC(0x0252A684,s32,self);
 u32 index=ld<u32>(gabi::ea(self),0x3E4);u32 game=gabi::call<u32>(0x025200D4);return gabi::call<s32>(0x02542EDC,p(game+0x52C4),index,p(0x101D6060),0x16,0,0);
}
VERIFY(0x0252A684,door_demo_action);
void door_set_goal(void* self){
 WWHD_FUNC(0x0252A6D0,void,self);
 u32 a=gabi::ea(self),game=gabi::call<u32>(0x025200D4);s32 angle=ld<s16>(a,0x32A);f32 z=ld<f32>(a,0x31C);u32 player=ld<u32>(game,0x5B2C),t=trig(u32(angle+0x7FFF));f32 px=ld<f32>(player,0x314),far=ld<f32>(0x1004C688),near=ld<f32>(0x1004C68C),x=ld<f32>(a,0x314),sn=ld<f32>(t),cs=ld<f32>(t,4);
 gabi::Local<Vec> out;out->x=px;f32 firstx=gabi::fmadds(sn,far,px),py=ld<f32>(player,0x318),firstz=gabi::fmadds(cs,near,z),ratio=ld<f32>(0x1004C694),secondx=gabi::fmadds(sn,near,x);out->y=py;
 f32 mixz=gabi::fmuls_ppc(firstz,ratio),pz=ld<f32>(player,0x31C),mixx=gabi::fmuls_ppc(secondx,ratio),other=ld<f32>(0x1004C690),secondz=gabi::fmadds(cs,far,pz);out->x=gabi::fmadds(firstx,other,mixx);out->z=gabi::fmadds(secondz,other,mixz);
 game=gabi::call<u32>(0x025200D4);gabi::call<void>(0x02543714,p(game+0x52C4),out.get());
}
VERIFY(0x0252A6D0,door_set_goal);
void door_player_angle(void* self,s32 flip){
 WWHD_FUNC(0x0252A798,void,self,flip);
 s32 angle=ld<s16>(gabi::ea(self),0x32A);u32 game=gabi::call<u32>(0x025200D4),player=ld<u32>(game,0x5B34);if(flip)angle=s16(angle+0x7FFF);st<u16>(player,0x422,u16(angle));
}
VERIFY(0x0252A798,door_player_angle);
void door_set_position(void* self,void* position,s32 angle){
 WWHD_FUNC(0x0252A7E8,void,self,position,angle);
 u32 a=gabi::ea(self),source=gabi::ea(position);u16 type=ld<u16>(a,0xF8);if(type==2||type==3)return;
 if(source){st<u32>(a,0x314,ld<u32>(source));st<u32>(a,0x318,ld<u32>(source,4));f32 offset=ld<f32>(0x1004C698),y=ld<f32>(a,0x318),x=ld<f32>(a,0x314);u32 zbits=ld<u32>(source,8);y=gabi::fadds_ppc(y,offset);st<f32>(a,0x390,x);st<u32>(a,0x31C,zbits);f32 z=ld<f32>(a,0x31C);st<f32>(a,0x394,y);st<f32>(a,0x398,z);st<f32>(a,0x380,y);st<f32>(a,0x384,z);st<f32>(a,0x37C,x);}
 st<u16>(a,0x322,u16(angle));st<u16>(a,0x32A,u16(angle));u32 t=trig(u32(angle));f32 sn=ld<f32>(t),cs=ld<f32>(t,4),zero=ld<f32>(0x1004C590);st<f32>(a,0x3B0,sn);st<f32>(a,0x3B4,zero);st<f32>(a,0x3B8,cs);
}
VERIFY(0x0252A7E8,door_set_position);
u32 door_event_no(void* self){WWHD_FUNC(0x0252A898,u32,self);return (ld<u32>(gabi::ea(self),0xB0)>>12)&255;}
VERIFY(0x0252A898,door_event_no);
void door_event_ids(void* self,u32 type){
 WWHD_FUNC(0x0252A8A4,void,self,type);
 u32 a=gabi::ea(self);for(u32 n=0;n<12;++n){u32 no=gabi::call<u32>(0x0252A898,self),game=gabi::call<u32>(0x025200D4);u32 map=gabi::call<u32>(0x02544708,p(game+0x52C4),no,n);st<u8>(a,0x3D6+n,u8(map));u32 name=ld<u32>(0x101D60B8+n*4);game=gabi::call<u32>(0x025200D4);u32 id=gabi::call<u32>(0x02543F10,p(game+0x52C4),p(name),map);st<u16>(a,0x3BE + n*2,u16(id));}
 if(type==1||type==2){u32 name=type==1?0x1004C69C:0x1004C6B4;u32 map=ld<u8>(a,0x3D8),game=gabi::call<u32>(0x025200D4),id=gabi::call<u32>(0x02543F10,p(game+0x52C4),p(name),map);st<u16>(a,0x3C2,u16(id));map=ld<u8>(a,0x3D9);game=gabi::call<u32>(0x025200D4);id=gabi::call<u32>(0x02543F10,p(game+0x52C4),p(name),map);st<u16>(a,0x3C4,u16(id));}
}
VERIFY(0x0252A8A4,door_event_ids);
void door_init_proc(void* self,u32 type){
 WWHD_FUNC(0x0252A9E0,void,self,type);
 u32 a=gabi::ea(self),t=trig(ld<u16>(a,0x2FA));f32 zero=ld<f32>(0x1004C590),sn=ld<f32>(t),cs=ld<f32>(t,4);st<f32>(a,0x3B4,zero);st<f32>(a,0x3B8,cs);st<f32>(a,0x3B0,sn);gabi::call<void>(0x0252A8A4,self,type);
}
VERIFY(0x0252A9E0,door_init_proc);
u32 door_switch(void* self){WWHD_FUNC(0x0252AA14,u32,self);return ld<u8>(gabi::ea(self),0xB3);}
VERIFY(0x0252AA14,door_switch);
u32 door_switch2(void* self){WWHD_FUNC(0x0252AA20,u32,self);return (ld<u32>(gabi::ea(self),0xB0)>>20)&255;}
VERIFY(0x0252AA20,door_switch2);
u32 door_type(void* self){WWHD_FUNC(0x0252AA2C,u32,self);return (ld<u32>(gabi::ea(self),0xB0)>>8)&15;}
VERIFY(0x0252AA2C,door_type);
void door_set_type(void* self,u32 type){WWHD_FUNC(0x0252AA38,void,self,type);if(type<16){u32 a=gabi::ea(self),old=ld<u32>(a,0xB0);st<u32>(a,0xB0,(old&0xFFFFF0FF)|(type<<8));}}
VERIFY(0x0252AA38,door_set_type);
u32 door_arg1(void* self){WWHD_FUNC(0x0252AA58,u32,self);return (u32(ld<s16>(gabi::ea(self),0x2FC))>>8)&255;}
VERIFY(0x0252AA58,door_arg1);
