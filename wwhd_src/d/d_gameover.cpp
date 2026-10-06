/* Game-over engine; method table 101E1CE4. Adjacent initializer attribution qualified. */
#include "bindings.h"
namespace gameover_cpp {
static void capture(){WWHD_FUNC(0x02549990,void,(u32)0);}

VERIFY(0x02549990,capture);
static void draw(){WWHD_FUNC(0x02549994,void,(u32)0);}

VERIFY(0x02549994,draw);
static u32 isDelete(){WWHD_FUNC(0x02549998,u32,(u32)0);return 1;}

VERIFY(0x02549998,isDelete);
static u32 execute(u32 self){
 WWHD_FUNC(0x025499A0,u32,self);
 u8 state=gabi::load<u8>(self+0x110);
 if(state==3){u32 gp=gabi::load<u32>(0x101F8344);gabi::store<u8>(gp+0x248,1);gabi::store<u8>(self+0x110,4);u32 save=gabi::load<u32>(0x101F84DC);gabi::store<u16>(save+0x22,12);return 1;}
 if(state==4){
  u32 gp=gabi::load<u32>(0x101F8344),window=gabi::load<u32>(gp+0x1BC);
  u32 a=gabi::call<u32>(0x02006478,window+0x18);
  u32 target=gabi::load<u32>(gabi::load<u32>(a+8)+0x14);
  u32 node=gabi::load<u32>(0x1049DD68);
  u32 current=gabi::call_ptr<u32>(target,a);
  u32 expected=gabi::call_ptr<u32>(gabi::load<u32>(node+0x14),0x1049DD60u);
  if(current==expected || gabi::load<u8>(window+0xD1)!=0){
   u8 choice=gabi::load<u8>(window+0xD5);u32 play=gabi::call<u32>(0x025200D4);
   gabi::store<u8>(play+0x5BE3,choice==0?3:2);gabi::call(0x0259170C,0u);gabi::store<u8>(self+0x110,6);
  }return 1;
 }
 if(state==5||state==6||gabi::load<u8>(self+0x111)==0)return 1;
 s16 timer=gabi::load<s16>(self+0x10C);u32 gp=gabi::load<u32>(0x101F8344);
 if(timer==90){gabi::call(0x027170D4,gp);gabi::store<u16>(self+0x10C,(u16)(gabi::load<s16>(self+0x10C)-1));}
 else if(gabi::call<s32>(0x027170F8,gp)!=0)gabi::store<u8>(self+0x110,3);
 return 1;
}

VERIFY(0x025499A0,execute);
static u32 executeWrapper(u32 self){WWHD_FUNC(0x02549B14,u32,self);return gabi::call<u32>(0x025499A0,self);}

VERIFY(0x02549B14,executeWrapper);
static u32 drawWrapper(){WWHD_FUNC(0x02549B18,u32,(u32)0);return 1;}

VERIFY(0x02549B18,drawWrapper);
static u32 deleteWrapper(){WWHD_FUNC(0x02549B20,u32,(u32)0);return 1;}

VERIFY(0x02549B20,deleteWrapper);
static u32 create(u32 self){
 WWHD_FUNC(0x02549B28,u32,self);
 if(gabi::call<s32>(0x025986BC)!=0)return 0;
 u32 play=gabi::call<u32>(0x025200D4);
 if(gabi::load<u8>(play+0x5BEC)!=0){play=gabi::call<u32>(0x025200D4);if(gabi::load<u8>(play+0x5BEC)!=4)return 0;}
 play=gabi::call<u32>(0x025200D4);if(gabi::load<u8>(play+0x5BB2)!=0)return 0;
 u32 save=gabi::load<u32>(0x101F84DC);u16 deaths=gabi::load<u16>(save+0x17A);if(deaths<9999)gabi::store<u16>(save+0x17A,deaths+1);
 gabi::store<u16>(self+0x10C,90);s16 delay=gabi::load<s16>(0x1047AF1E);
 gabi::store<u8>(self+0x110,0);gabi::store<u16>(self+0x10E,delay);gabi::store<u8>(self+0x111,0);return 4;
}

VERIFY(0x02549B28,create);
static u32 createWrapper(u32 self){WWHD_FUNC(0x02549BE8,u32,self);return gabi::call<u32>(0x02549B28,self);}

VERIFY(0x02549BE8,createWrapper);
static u32 checkDelete(u32 self){WWHD_FUNC(0x02549BEC,u32,self);return gabi::load<u8>(self+0x110)==6;}

VERIFY(0x02549BEC,checkDelete);
static void backAlpha(f64 alpha){
 WWHD_FUNC(0x02549C00,void,alpha);
 gabi::call(0x0266A064,gabi::load<u32>(gabi::load<u32>(0x101F8344)+0x1E8));
 u8 value=(u8)gabi::ftoi((f32)(alpha*(f64)gabi::load<f32>(0x1004E1D8)));
 u32 play=gabi::call<u32>(0x025200D4);gabi::store<u8>(play+0x62F1,value);
}

VERIFY(0x02549C00,backAlpha);
static void initialize(){
 WWHD_FUNC(0x02549C74,void,(u32)0);
 gabi::store<u32>(0x10475984,0);gabi::store<u32>(0x10475980,0);gabi::store<u32>(0x1047597C,0);gabi::store<u32>(0x10475978,0);
 gabi::call(0x028F026C,0x101E1CF8u);
 f32 a=gabi::load<f32>(0x1004E1E0),b=gabi::load<f32>(0x1004E1E4);gabi::store<f32>(0x1047595C,a);gabi::store<f32>(0x10475960,b);
 gabi::call(0x028ED6F8,0x10475974u);gabi::call(0x028F026C,0x101E1D04u);gabi::call(0x028EAB2C,0x10475975u);gabi::call(0x028F026C,0x101E1D10u);
 a=gabi::load<f32>(0x1004E1E8);b=gabi::load<f32>(0x1004E1EC);gabi::store<f32>(0x10475964,a);gabi::store<f32>(0x1047596C,b);gabi::store<f32>(0x10475968,a);gabi::store<f32>(0x10475970,b);
}
VERIFY(0x02549C74,initialize);

}
