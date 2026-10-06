// HD logo scene startup and phases.
#include "gabi.h"
using namespace gabi;
static u32 U(u32 p,u32 n=0){return load<u32>(p+n);}
struct LogoName { be<u32> a,b; };
static u32 relative(u32 p){u32 n=U(p);return n?p+n:0;}
u32 logoStartup(u32 p){
 WWHD_FUNC(0x025ABFD4,u32,p);Local<LogoName> left,right;left->b=0x100526DCu;right->b=0x100526DCu;right->a=0x10052708u;left->a=0x100526F4u;
 if(call<u32>(0x026124B0,U(0x101F4F7Cu),left.get(),right.get(),0u)){
  u32 archive=call<u32>(0x027E2DC0),folder=relative(archive+0x4C),entry=call<u32>(0x027DFA24,folder,0x10052714u),data=relative(entry);
  u32 g=call<u32>(0x025200D4);store<u32>(g+0x5D24,data);
  folder=relative(archive+0x4C);entry=call<u32>(0x027DFA24,folder,0x100526FCu);data=relative(entry);g=call<u32>(0x025200D4);store<u32>(g+0x5D28,data);
  folder=relative(archive+0x4C);entry=call<u32>(0x027DFA24,folder,0x10052724u);data=relative(entry);g=call<u32>(0x025200D4);call<void>(0x025ABD4C,g+0x5098,data);
 }
 right->b=0x100526DCu;left->b=0x100526DCu;right->a=0x10052734u;left->a=0x10052740u;
 if(call<u32>(0x026124B0,U(0x101F4F7Cu),right.get(),left.get(),0u)){
  u32 archive=call<u32>(0x027E2DC0),folder=relative(archive+0x4C),entry=call<u32>(0x027DFA24,folder,0x10052750u),data=relative(entry),g=call<u32>(0x025200D4);call<void>(0x025A7838,U(g,0x5AB0),data);
 }
 call<void>(0x02590708);call<void>(0x025907FC);u32 g=call<u32>(0x025200D4);store<u8>(g+0x5AC9,0);
 for(u32 n=0;n<3;++n){g=call<u32>(0x025200D4);store<u8>(g+0x5BE5,(u8)(load<u8>(g+0x5BE5)&~(1u<<n)));}
 call<void>(0x025E2FF0);call<void>(0x025E30AC);call<void>(0x025E3184);call<void>(0x025E3274);call<void>(0x02708748,U(0x101F8294u));call<void>(0x027089F8);return 1;
}
VERIFY(0x025ABFD4,logoStartup);
u32 logoPhase0(u32 p){
 WWHD_FUNC(0x025AC214,u32,p);u32 control=call<u32>(0x02035D78);call<void>(0x02035ED4,control,0u);
 if(!U(0x1018F504u)){call<void>(0x0203E8C4);call<void>(0x0203B88C);call<void>(0x0203B984,U(0x1018F504u));call<void>(0x0203BBB0,U(0x1018F504u));}
 if(!U(0x1018F450u)){u32 owner=call<u32>(0x0275320C,4u)+0x30D4;Local<LogoName> name;name->b=0x100526DCu;name->a=0x1005276Cu;
  u32 heap=call<u32>(0x0203E8C4),resource=call<u32>(0x02753004,owner,name.get(),heap,1u,0u);
  call<void>(0x02036EEC);call<void>(0x02036F8C,U(0x1018F450u));call<void>(0x02036FD4,U(0x1018F450u));call_ptr<void>(U(U(resource,12),0x2C),resource);
 }
 if(load<u8>(0x101F4704u)&&call<u32>(0x025E18AC))return 0;if(!U(0x101F4F54u))return 0;
 if(!U(0x101F50D8u)){u32 owner=call<u32>(0x0275320C,4u)+0x1C8;Local<LogoName> name;name->b=0x100526DCu;name->a=0x10052790u;
  u32 heap=call<u32>(0x0203E8E8),resource=call<u32>(0x02753004,owner,name.get(),heap,1u,0u);
  call<void>(0x026193D4);call<void>(0x026195C4,U(0x101F50D8u));call_ptr<void>(U(U(resource,12),0x2C),resource);
 }
 u32 heap=call<u32>(0x025E3268);if(call<u32>(0x02520354,0x10052764u,0u,0u)!=1)call<void>(0x0273AA24,0x100527ACu,0x238u,0x1005275Cu);
 call<void>(0x027EC3D4,heap);return 2;
}
VERIFY(0x025AC214,logoPhase0);
u32 logoPhase1(u32 p){
 WWHD_FUNC(0x025AC3F8,u32,p);if(call<u32>(0x02522F50))return 0;call<void>(0x025E3268);
 Local<LogoName> first;first->b=0x100526DCu;first->a=0x100527BCu;u32 a=call<u32>(0x026066C4,U(0x101F4F28u),first.get(),3u);
 if(!a)call<void>(0x0273AA24,0x100527C4u,0x25Au,0x100527D4u);
 u32 owner=U(0x101F4F28u);Local<LogoName> second;second->b=0x100526DCu;second->a=0x100527BCu;store<u32>(0x10475800u,a);
 u32 b=call<u32>(0x026066C4,owner,second.get(),4u);if(!b)call<void>(0x0273AA24,0x100527C4u,0x25Fu,0x100527D4u);store<u32>(0x10475804u,b);return 2;
}
VERIFY(0x025AC3F8,logoPhase1);
u32 logoPhase2(u32 p){
 WWHD_FUNC(0x025AC4D4,u32,p);s32 result=call<s32>(0x025203D8,0x100527F8u);if(result!=0){if(result<0)call<void>(0x0273AA24,0x10052800u,0x281u,0x100527E8u);return 0;}
 call<void>(0x025E3268);u32 g=call<u32>(0x025200D4);call<void>(0x02524BB0,g+0x12A0);
 for(u32 n=0;n<25;++n){if(call<u32>(0x02520354,U(0x101EA604u,n*4),0u,0u)!=1)call<void>(0x0273AA24,0x10052800u,0x2B6u,0x100527F0u);}
 u32 clock=call<u32>(0xC0009C80u),ticks=U(clock);u32 rate=(u32)(((u64)(ticks>>2)*0x88888889u)>>37);
 call<void>(0x025F096C,rate);call<void>(0x025F0968,0u);call<void>(0x025F05D0,30u,0u);
 store<u32>(U(0x101F4974u),0);store<u32>(U(0x101F4974u)+4,0);store<u32>(U(0x101F4974u)+4,0);call<void>(0x02721A18,U(0x101F852Cu),0u);return 2;
}
VERIFY(0x025AC4D4,logoPhase2);
u32 logoPhase3(u32 p){
 WWHD_FUNC(0x025AC620,u32,p);if(call<u32>(0x02522F50))return 0;u32 state=U(0x101F852Cu);if(U(state,0x1C)==1)return 0;
 u32 manager=U(0x101F4F28u),heap=U(manager,0x2040),vt=U(heap,12);call_ptr<void>(U(vt,0x6C),heap);
 state=U(0x101F852Cu);if(U(state,0x20)==0xFFFFFFFEu && !call<u32>(0x02722894,state))return 0;
 u32 control=call<u32>(0x02035D78);call<void>(0x02035ED4,control,1u);return 4;
}
VERIFY(0x025AC620,logoPhase3);
u32 logoCreate(u32 p){WWHD_FUNC(0x025AC6C8,u32,p);return call<u32>(0x02525FE4,p+0x1C8,0x101EA668u,p);}
VERIFY(0x025AC6C8,logoCreate);
void logoStaticInit(){WWHD_FUNC(0x025AC6DC,void);store<u32>(0x1047B390u,0);store<u32>(0x1047B388u,0);store<u32>(0x1047B394u,0);store<u32>(0x1047B38Cu,0);call<void>(0x028F026C,0x101EA678u);f32 a=load<f32>(0x100528B4u),b=load<f32>(0x100528B8u);store<f32>(0x1047B37Cu,a);store<f32>(0x1047B380u,b);call<void>(0x028ED6F8,0x1047B384u);call<void>(0x028F026C,0x101EA684u);call<void>(0x028EAB2C,0x1047B385u);call<void>(0x028F026C,0x101EA690u);}
VERIFY(0x025AC6DC,logoStaticInit);
u32 logoIsDelete(u32 p){WWHD_FUNC(0x025AC770,u32,p);return 1;}
VERIFY(0x025AC770,logoIsDelete);
void logoEmptyDtor(u32 p,u32 flags){WWHD_FUNC(0x025AC778,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025AC778,logoEmptyDtor);
void logoEmpty(u32 p){WWHD_FUNC(0x025AC78C,void,p);}
VERIFY(0x025AC78C,logoEmpty);
