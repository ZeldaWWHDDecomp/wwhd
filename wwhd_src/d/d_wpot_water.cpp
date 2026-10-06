#include "bindings.h"
static void callback_dtor(u32 p,u32 flags) {WWHD_FUNC(0x025D3950,void,p,flags);if(p&&(flags&1))gabi::call(0x0273AF40,p,flags);}
VERIFY(0x025D3950,callback_dtor);
static void packet_noop(u32 p) {WWHD_FUNC(0x025D3964,void,p);}
VERIFY(0x025D3964,packet_noop);
static s32 draw(u32 p) {WWHD_FUNC(0x025D3968,s32,p);u32 game=gabi::call<u32>(0x025200D4);gabi::call(0x027F0E04,gabi::load<u32>(game+0x5D7C),p+0x100,0);return 1;}
VERIFY(0x025D3968,draw);
static s32 execute(u32 p) {WWHD_FUNC(0x025D39A8,s32,p);gabi::call(0x028E93CC,0x1048D0CC,gabi::load<f32>(p+0xE0),gabi::fadds_ppc(gabi::load<f32>(p+0xE4),gabi::load<f32>(0x10056D84)),gabi::load<f32>(p+0xE8));f32 scale=gabi::load<f32>(0x10056D88);gabi::call(0x025F2518,scale,gabi::load<f32>(0x10056D8C),scale);gabi::call(0x028E9108,0x104B45F8,0x1048D0CC,p+0x198);u32 em=gabi::load<u32>(p+0xFC);if((gabi::load<u32>(em+0x254)&8)&&gabi::load<u32>(em+0x1B4)+gabi::load<u32>(em+0x1C0)==0){gabi::store<u32>(em+0x254,gabi::load<u32>(em+0x254)&~0x40u);gabi::call(0x025DAD48,p);}return 1;}
VERIFY(0x025D39A8,execute);
static s32 is_delete(u32 p) {WWHD_FUNC(0x025D3A60,s32,p);return 1;}
VERIFY(0x025D3A60,is_delete);
static s32 remove(u32 p) {WWHD_FUNC(0x025D3A68,s32,p);if(p){gabi::call(0x027F13DC,p+0x100,0);gabi::call(0x025DD630,p,0);}return 1;}
VERIFY(0x025D3A68,remove);
struct Ground {u8 bytes[84];};
static void ground_setup(u32 a,bool lava) {
 gabi::call(0x02008E0C,a);gabi::store<u32>(a,a+0x40);gabi::store<u32>(a+4,a+0x4C);
 gabi::store<u32>(a+0x10,lava?0x10056D44:0x10056CC4);gabi::store<u32>(a+0x20,lava?0x10056D54:0x10056CD4);
 for(u32 i=0;i<7;i++)gabi::store<u8>(a+0x44+i,0);if(lava)gabi::store<u8>(a+0x44,1);
 gabi::store<u32>(a+0x40,lava?0x10056D74:0x10056CF4);gabi::store<u32>(a+0x4C,lava?0x10056D64:0x10056CE4);gabi::store<u32>(a+0x50,lava?4:1);
}
static void ground_destroy(u32 lava,u32 ground) {for(u32 a:{lava,ground}){gabi::store<u32>(a+0x20,0x10056CD4);gabi::store<u32>(a+0x40,0x10056CF4);gabi::store<u32>(a+0x4C,0x10056CB4);gabi::call(0x02008DAC,a,0);}}
static s32 create(u32 p) {
 WWHD_FUNC(0x025D3AB0,s32,p);
 gabi::Local<Ground> ground;ground_setup(ground.a,false);gabi::Local<Ground> lava;ground_setup(lava.a,true);
 f32 sentinel=gabi::load<f32>(0x10056D90),step=gabi::load<f32>(0x10056D94),up=gabi::load<f32>(0x10056D8C),raise=gabi::load<f32>(0x10056D98);
 for(u32 i=0;i<50;i++) {
  u32 trig=0x104A44F8+(((u16)(i*10000))>>3)*8;f32 r=gabi::fmuls_ppc((f32)i,step);
  f32 x=gabi::fmadds(r,gabi::load<f32>(trig+4),gabi::load<f32>(p+0xE0)),y=gabi::fadds_ppc(gabi::load<f32>(p+0xE4),up),z=gabi::fmadds(r,gabi::load<f32>(trig),gabi::load<f32>(p+0xE8));
  gabi::store<f32>(lava.a+0x24,x);gabi::store<f32>(lava.a+0x28,y);gabi::store<f32>(lava.a+0x2C,z);
  u32 game=gabi::call<u32>(0x025200D4);f64 lavaY=gabi::call<f64>(0x02008974,game+0x12A0,lava.get());
  if(lavaY==sentinel)continue;
  gabi::store<f32>(ground.a+0x24,x);gabi::store<f32>(ground.a+0x28,y);gabi::store<f32>(ground.a+0x2C,z);
  game=gabi::call<u32>(0x025200D4);f64 gndY=gabi::call<f64>(0x02008974,game+0x12A0,ground.get());
  if(gabi::load<u16>(lava.a+0x14)==65535||gabi::load<u16>(lava.a+0x16)==256)continue;
  game=gabi::call<u32>(0x025200D4);if(gabi::call<s32>(0x024EF0F4,game+0x12A0,lava.a+0x14)!=6||!(lavaY>gndY))continue;
  gabi::Local<gabi::be<f32>[3]> spawn;gabi::store<f32>(spawn.a,x);gabi::store<f32>(spawn.a+4,(f32)(lavaY+raise));gabi::store<f32>(spawn.a+8,z);
  gabi::call(0x025D5834,0x2A,0,spawn.get(),gabi::load<u32>(p+0xF8),0,0,~0u,0);break;
 }
 gabi::store<f32>(ground.a+0x24,gabi::load<f32>(p+0xE0));gabi::store<f32>(ground.a+0x28,gabi::fadds_ppc(gabi::load<f32>(p+0xE4),up));gabi::store<f32>(ground.a+0x2C,gabi::load<f32>(p+0xE8));
 u32 game=gabi::call<u32>(0x025200D4);f64 gy=gabi::call<f64>(0x02008974,game+0x12A0,ground.get());gabi::store<f32>(p+0xE4,(f32)gy);
 if(gy==sentinel){ground_destroy(lava.a,ground.a);return 5;}
 gabi::Local<gabi::be<f32>[3]> pos;gabi::store<f32>(pos.a,gabi::load<f32>(p+0xE0));gabi::store<f32>(pos.a+4,(f32)gy);gabi::store<f32>(pos.a+8,gabi::load<f32>(p+0xE8));
 gabi::call(0x025D5834,0xB7,0,pos.get(),gabi::load<u32>(p+0xF8),0,0,~0u,0);
 if(p){gabi::call(0x025DD5F0,p);gabi::store<u32>(p+0xB4,0x10056CA4);gabi::call(0x027F1278,p+0x100);gabi::store<u32>(p+0x10C,0x1004CC38);}
 gabi::Local<gabi::be<f32>[3]> splash;gabi::store<f32>(splash.a,gabi::load<f32>(p+0xE0));gabi::store<f32>(splash.a+4,gabi::fadds_ppc(gabi::load<f32>(p+0xE4),gabi::load<f32>(0x10056DA8)));gabi::store<f32>(splash.a+8,gabi::load<f32>(p+0xE8));
 game=gabi::call<u32>(0x025200D4);gabi::call(0x025A847C,gabi::load<u32>(game+0x5AB0),0,0x8083,splash.get(),0,0,255,0,~0u,0,0,0);
 game=gabi::call<u32>(0x025200D4);gabi::call(0x025A847C,gabi::load<u32>(game+0x5AB0),0,0x8084,p+0xE0,0,0,255,0,~0u,0,0,0);
 game=gabi::call<u32>(0x025200D4);u32 emitter=gabi::call<u32>(0x025A847C,gabi::load<u32>(game+0x5AB0),0,0x8086,p+0xE0,0,0,0xAA,0x104873AC,~0u,0,0,0);gabi::store<u32>(p+0xFC,emitter);
 if(emitter)gabi::store<u32>(emitter+0x254,gabi::load<u32>(emitter+0x254)|0x40);
 ground_destroy(lava.a,ground.a);return emitter?4:5;
}
VERIFY(0x025D3AB0,create);
struct DrawState {u8 bytes[284];};
static void emitter_draw(u32 cb,u32 em) {
 WWHD_FUNC(0x025D4124,void,cb,em);if(!(gabi::load<u8>(em+0x18E)&1))return;
 gabi::Local<DrawState> state;u32 a=state.a;gabi::call(0x02750250,state.get());u32 flags=gabi::load<u32>(a+0xC),mode=gabi::load<u32>(a+0xEC);
 gabi::store<u32>(a+0xE4,7);gabi::store<f32>(a+0xE8,gabi::load<f32>(0x10056DAC));gabi::store<u32>(a+4,3);gabi::store<u32>(a+0xC,flags|3);
 gabi::store<u8>(a,0);gabi::store<u8>(a+1,0);gabi::store<u32>(a+0x10,6);gabi::store<u8>(a+0xE0,1);gabi::store<u32>(a+0x14,6);
 gabi::store<u32>(a+0x1C,7);gabi::store<u32>(a+0x2C,6);gabi::store<u32>(a+0x18,7);gabi::store<u32>(a+0x28,6);gabi::store<u32>(a+0x30,7);gabi::store<u32>(a+0xEC,(((mode&~15u)+8)&~0xF0u)+0x80);gabi::store<u32>(a+0x34,7);
 gabi::call(0x027505B4,state.get());gabi::call(0x02750520,state.get());gabi::call(0x02750534,state.get());gabi::call(0x027505DC,state.get());
}
VERIFY(0x025D4124,emitter_draw);
static void initializer() {WWHD_FUNC(0x025D41F0,void,(u32)0);for(u32 i=0;i<4;i++)gabi::store<u32>(0x1048739C+i*4,0);gabi::call(0x028F026C,0x101F2FE0);gabi::store<f32>(0x10487390,gabi::load<f32>(0x10056DB4));gabi::store<f32>(0x10487394,gabi::load<f32>(0x10056DB8));gabi::call(0x028ED6F8,0x10487398);gabi::call(0x028F026C,0x101F2FEC);gabi::call(0x028EAB2C,0x10487399);gabi::call(0x028F026C,0x101F2FF8);gabi::store<u32>(0x104873AC,0x10056DC0);gabi::call(0x028F026C,0x101F3004);}
VERIFY(0x025D41F0,initializer);
static void callback_empty_A0(u32 p) {WWHD_FUNC(0x025D42A0,void,p);}
VERIFY(0x025D42A0,callback_empty_A0);
static void callback_empty_A4(u32 p) {WWHD_FUNC(0x025D42A4,void,p);}
VERIFY(0x025D42A4,callback_empty_A4);
static void callback_delete(u32 p,u32 flags) {WWHD_FUNC(0x025D42A8,void,p,flags);if(p&&(flags&1))gabi::call(0x0273AF40,p,flags);}
VERIFY(0x025D42A8,callback_delete);
static void callback_empty_BC(u32 p) {WWHD_FUNC(0x025D42BC,void,p);}
VERIFY(0x025D42BC,callback_empty_BC);
static void callback_empty_C0(u32 p) {WWHD_FUNC(0x025D42C0,void,p);}
VERIFY(0x025D42C0,callback_empty_C0);
