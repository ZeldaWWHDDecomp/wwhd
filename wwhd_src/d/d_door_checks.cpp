#include "bindings.h"
namespace doorchecks{template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);}template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);}void* p(u32 a){return gabi::at<void>(a);}struct Vec{be<f32>x,y,z;};struct Angle{be<s16>value;};struct Globe{be<f32>radius;be<s16>latitude,longitude;};}
using namespace doorchecks;
BOOL door_adjoin(void* self){
 WWHD_FUNC(0x0252AA64,BOOL,self);
 u32 front=gabi::call<u32>(0x0252A37C,self),back=gabi::call<u32>(0x0252A388,self);if(front==63||back==63)return 1;
 u32 game=gabi::call<u32>(0x025200D4);if(gabi::call<u32>(0x025C4334,p(game+0x51CC),front))return 1;
 game=gabi::call<u32>(0x025200D4);return gabi::call<u32>(0x025C4334,p(game+0x51CC),back)!=0;
}
VERIFY(0x0252AA64,door_adjoin);
u32 door_view_room(void* self){
 WWHD_FUNC(0x0252AB08,u32,self);
 u32 a=gabi::ea(self),game=gabi::call<u32>(0x025200D4),camera=ld<u32>(game,0x5FA4);gabi::Local<Vec> delta;gabi::call<void>(0x0201ADE0,p(camera+0xDC),delta.get(),p(a+0x314));
 f32 dz=delta->z,sz=ld<f32>(a,0x3B8),sx=ld<f32>(a,0x3B0),product=gabi::fmuls_ppc(dz,sz),dx=delta->x,dot=gabi::fmadds(dx,sx,product),zero=ld<f32>(0x1004C590);
 if(dot<zero)return gabi::call<u32>(0x0252A388,self);return gabi::call<u32>(0x0252A37C,self);
}
VERIFY(0x0252AB08,door_view_room);
BOOL door_front_old(void* self){
 WWHD_FUNC(0x0252AB90,BOOL,self);
 u32 a=gabi::ea(self),game=gabi::call<u32>(0x025200D4),player=ld<u32>(game,0x5B2C);gabi::Local<Vec> delta;gabi::call<void>(0x0201ADE0,p(player+0x314),delta.get(),p(a+0x314));
 gabi::Local<Globe> globe;gabi::call<void>(0x02007324,globe.get(),delta.get());gabi::Local<Angle> angle,difference;gabi::call<void>(0x020065FC,angle.get());s32 yaw=ld<s16>(a,0x322);gabi::call<void>(0x02006908,p(gabi::ea(globe.get())+6),difference.get(),yaw);angle->value=difference->value;u32 absolute=gabi::call<u32>(0x020067EC,angle.get());return absolute>=0x4000;
}
VERIFY(0x0252AB90,door_front_old);
u32 door_front_check(void* self){
 WWHD_FUNC(0x0252AC10,u32,self);
 u32 stay=u32(ld<s8>(0x1047E6C8)),front=gabi::call<u32>(0x0252A37C,self),back=gabi::call<u32>(0x0252A388,self);
 if(front==back)return gabi::call<u32>(0x0252AB90,self);if(stay==front)return 0;if(stay==back)return 1;return 2;
}
VERIFY(0x0252AC10,door_front_check);
s32 door_check_execute(void* self){
 WWHD_FUNC(0x0252AC98,s32,self);
 u32 a=gabi::ea(self),side=gabi::call<u32>(0x0252AC10,self),flags=ld<u32>(a,0x2E0);st<u8>(a,0x3BC,u8(side));if(flags&0x1000)return 1;
 u16 command=ld<u16>(a,0xF8);if(command==2||command==3)return 2;
 s32 stay=ld<s8>(0x1047E6C8),room=ld<s8>(a,0x3AC);if(room!=stay||side==2)return 0;return gabi::call<u32>(0x0252AA64,self)?2:0;
}
VERIFY(0x0252AC98,door_check_execute);
void door_start_demo(void* self){
 WWHD_FUNC(0x0252AD50,void,self);
 u32 a=gabi::ea(self),game=gabi::call<u32>(0x025200D4),player=ld<u32>(game,0x5B2C);game=gabi::call<u32>(0x025200D4);u32 staff=gabi::call<u32>(0x02542D88,p(game+0x52C4),p(0x1004C7A0),0,0);s32 angle=ld<s16>(a,0x322);st<u32>(a,0x3E4,staff);st<u16>(a,0x32A,u16(angle));
 if(!player)gabi::call<void>(0x0273AA24,p(0x1004C7B0),0x169,p(0x1004C798));
 s32 other=ld<s16>(player,0x2FA),home=ld<s16>(a,0x2FA),delta=s16(other-home);if(delta<0)delta=s16(-delta);
 if(u32(delta+0x3E7)<0x13E7){angle=ld<s16>(a,0x32A);st<u16>(a,0x32A,u16(angle+0x7FFF));}
}
VERIFY(0x0252AD50,door_start_demo);
BOOL door_check_area(void* self,f32 lateral,f32 forward,f32 distance){
 WWHD_FUNC(0x0252AE04,BOOL,self,lateral,forward,distance);
 u32 a=gabi::ea(self),game=gabi::call<u32>(0x025200D4),player=ld<u32>(game,0x5B2C);gabi::Local<Vec> delta,flat,normalized;
 gabi::call<void>(0x0201ADE0,p(player+0x314),delta.get(),p(a+0x314));f32 x=delta->x,zero=ld<f32>(0x1004C590),z=delta->z;flat->y=zero;flat->x=x;flat->z=z;
 f32 square=gabi::call<f32>(0x028E8DD0,flat.get());if(square>distance)return 0;
 gabi::call<void>(0x0201B31C,delta.get(),normalized.get());z=delta->z;f32 sz=ld<f32>(a,0x3B8),sx=ld<f32>(a,0x3B0),product=gabi::fmuls_ppc(z,sz);x=delta->x;f32 dot=gabi::fmadds(x,sx,product),project=gabi::fmuls_ppc(square,dot);project=gabi::fmuls_ppc(project,dot);if(project>forward)return 0;if(gabi::fsubs_ppc(square,project)>lateral)return 0;
 u8 side=ld<u8>(a,0x3BC);s32 yaw=ld<s16>(a,0x322),pyaw=ld<s16>(player,0x322);if(side==1)yaw=s16(yaw+0x7FFF);s32 difference=s16(yaw-pyaw);s32 absolute=difference<0?-difference:difference;return absolute>=0x5000;
}
VERIFY(0x0252AE04,door_check_area);
s32 door_draw_local(void* self){
 WWHD_FUNC(0x0252AF98,s32,self);
 u32 a=gabi::ea(self);if(!gabi::call<u32>(0x0252AA64,self)){u16 command=ld<u16>(a,0xF8);if(command!=2&&command!=3)return 0;}
 u32 front=gabi::call<u32>(0x0252A37C,self);bool boundary=front==63;if(!boundary)boundary=gabi::call<u32>(0x0252A388,self)==63;
 if(boundary){u8 stay=ld<u8>(0x1047E6C8);st<u8>(a,0x1C9,stay);st<u8>(a,0x326,stay);}
 else{u32 room=gabi::call<u32>(0x0252AB08,self);st<u8>(a,0x1C9,u8(room));st<u8>(a,0x326,u8(room));}
 u32 side=gabi::call<u32>(0x0252AC10,self);s32 from=-1;if(side!=2)from=ld<s8>(0x1047E6C8);s32 room=ld<s8>(a,0x1C9);st<u8>(a,0x3E8,u8(from));
 u32 game=gabi::call<u32>(0x025200D4);if(!gabi::call<u32>(0x025C4334,p(game+0x51CC),room))return 1;
 u32 view=gabi::call<u32>(0x0252AB08,self);if(view!=63)return 2;
 front=gabi::call<u32>(0x0252A37C,self);if(front!=63)return 1;return gabi::call<u32>(0x0252A388,self)==63?2:1;
}
VERIFY(0x0252AF98,door_draw_local);
BOOL door_draw_check(void* self,s32 mode){
 WWHD_FUNC(0x0252B0FC,BOOL,self,mode);
 u32 a=gabi::ea(self),result=gabi::call<u32>(0x0252AF98,self),flags=ld<u32>(a,0x2E0);
 if(result)flags=(flags&0xFFFFFFC0)|(mode?0x2A:0x29);else flags&=0xFFFFFFDF;
 st<u32>(a,0x2E0,flags);return result==2;
}
VERIFY(0x0252B0FC,door_draw_check);
