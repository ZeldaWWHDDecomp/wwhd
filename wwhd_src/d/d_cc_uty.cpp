/* WWHD collision attack utilities. */
#include "gabi.h"
namespace d_cc_uty_cpp {
using gabi::load; using gabi::store; using gabi::call;
u16 cutBit() {
 WWHD_FUNC(0x02518B28,u16);
 u32 play=call<u32>(0x025200D4), player=load<u32>(play+0x5B2C);
 u8 cut=load<u8>(player+0x3AC);
 if(cut>=1 && cut<=7) return 1u<<(cut-1);
 if(cut==8 || cut==9) return 0x80;
 return cut==10?0x100:0;
}
VERIFY(0x02518B28,cutBit);
static u32 sound(u32 obj,bool critical) {
 if(load<u32>(obj+0x10)&0x01010000) return 0x2855;
 switch(load<u8>(obj+0x6C)) {
 case 4:return critical?0x2835:0x2833;
 case 5:return critical?0x2836:0x2834;
 case 7:return 0x2879; case 8:return 0x286F;
 default:return critical?0x2806:0x2803;
 }
}
u32 atSound(u32 obj) {
 WWHD_FUNC(0x02518BE8,u32,obj);
 u32 info=call<u32>(0x025157AC,obj);
 if(!info) {call(0x0273AA24,0x1004B450u,0xABu,0x1004B448u);return 0x2803;}
 return sound(info,false);
}
VERIFY(0x02518BE8,atSound);
static void startSound(u32 actor,u32 id,u32 arg) {
 u32 pos=actor+0x37C;
 if(pos) {u32 reverb=call<u32>(0x02520540,(s32)load<s8>(actor+0x326));call(0x025E1A40,id,pos,arg,reverb);}
}
void defenseSound(u32 actor,u32 obj,u32 arg) {
 WWHD_FUNC(0x02518CC8,void,actor,obj,arg);
 u32 id=atSound(obj);
 if(actor) startSound(actor,id,arg);
}
VERIFY(0x02518CC8,defenseSound);
void defenseSoundAt(u32 actor,u32 pos,u32 obj,u32 arg) {
 WWHD_FUNC(0x02518D40,void,actor,pos,obj,arg);
 u32 reverb=call<u32>(0x02520540,(s32)load<s8>(actor+0x326));
 u32 id=atSound(obj);call(0x025E1A40,id,pos,arg,reverb);
}
VERIFY(0x02518D40,defenseSoundAt);
u32 powerCheck(u32 at) {
 WWHD_FUNC(0x02518DB0,u32,at);
 u32 play=call<u32>(0x025200D4),obj=load<u32>(at),player=load<u32>(play+0x5B2C);
 if(!obj)return 0;
 u32 status=load<u32>(obj+0x44),actor=status?load<u32>(status+0xC):0;
 store<u8>(at+8,0); obj=load<u32>(at);store<u8>(at+0xA,0xC);store<u32>(at+4,actor);
 u32 flags=load<u32>(obj+0x10);
 if(flags&0x100){store<u8>(at+0xA,4);return load<u32>(at+4);}
 flags=load<u32>(obj+0x10);
 if(flags&0x200000){store<u8>(at+0xA,8);return load<u32>(at+4);}
 flags=load<u32>(obj+0x10);
 if(flags&0x20000){store<u8>(at+0xA,5);return load<u32>(at+4);}
 flags=load<u32>(obj+0x10);actor=load<u32>(at+4);
 if(flags&0x40200)store<u8>(at+0xA,5);
 if(actor) {
  obj=load<u32>(at);actor=load<u32>(at+4);store<u8>(at+8,load<u8>(obj+0x14));
  if(actor) {
   s16 name=load<s16>(actor+8);
   if(name==168) {
    obj=load<u32>(at);
    if(load<u32>(obj+0x10)&0x10000)store<u8>(at+0xA,9);
    else {store<u8>(at+0xA,1);u16 bit=cutBit();store<u16>(at+0x12,bit);
     u8 count=load<u8>(player+0x3AD);u32 hit;
     if(count==3)hit=1;else if(count==4)hit=4;else {u8 cut=load<u8>(player+0x3AC);hit=cut==10?2:(cut==5||cut==15||cut==16)?3:0;}
     store<u32>(at+0x18,hit);
    }
   } else if(name==294) {u32 state=call<u32>(0x020CB92C,actor,8u);store<u16>(at+0x12,0x200);store<u8>(at+0xA,state?13:2);}
   else if(name==295){store<u8>(at+0xA,2);store<u16>(at+0x12,0x200);}
   else if(name==446){store<u8>(at+0xA,14);store<u16>(at+0x12,0x400);}
   else if(name==432){store<u8>(at+0xA,10);store<u16>(at+0x12,0x800);}
   else if(name==472)store<u16>(at+0x12,0x1000);
   else if(name==238)store<u8>(at+0xA,11);
   else if(name==453){u32 type=call<u32>(0x02048038,actor,4u,0x18u);store<u8>(at+0xA,type==2?4:3);}
   else if(name==188){u8 damage=load<u8>(at+8);store<u8>(at+0xA,7);if(damage>2)store<u8>(at+8,2);}
   else if(name==216){store<u8>(at+0xA,6);store<u8>(at+8,4);}
  }
 }
 actor=load<u32>(at+4);store<u8>(at+9,0);
 if(actor && load<s16>(actor+8)==168) {
  u32 info=call<u32>(0x025157AC,load<u32>(at));
  if(load<u8>(info+0x6F)==1){store<u8>(at+9,1);return load<u32>(at+4);}
  return load<u32>(at+4);
 }
 if(load<u8>(at+8)>=2){store<u8>(at+9,1);return load<u32>(at+4);}
 return actor;
}
VERIFY(0x02518DB0,powerCheck);
u32 attackCheck(u32 target,u32 at) {
 WWHD_FUNC(0x025192A8,u32,target,at);
 u32 play=call<u32>(0x025200D4),player=load<u32>(play+0x5B2C);
 store<u32>(at+0x18,0);u32 actor=powerCheck(at);store<u32>(at+4,actor);
 if(!actor)return actor;
 call(0x025E1FD8,actor);
 actor=load<u32>(at+4);call(0x028E8DD0,actor+0x33C);
 // The magnitude helper consumes the vector helper's preserved f1 result.
 f32 magnitude=call<f32>(0x028F4384,(f64)gabi::cpu->f[1].ps0);
 f32 limit=load<f32>(0x1004B460);actor=load<u32>(at+4);f32 x,z;
 if(magnitude>limit){z=load<f32>(actor+0x344);x=load<f32>(actor+0x33C);}
 else {f32 tx=load<f32>(target+0x314),ax=load<f32>(actor+0x314),az=load<f32>(actor+0x31C);x=gabi::fsubs_ppc(tx,ax);f32 tz=load<f32>(target+0x31C);z=gabi::fsubs_ppc(tz,az);}
 u16 angle=call<u16>(0x020195B0,(f64)-x,(f64)-z);store<u16>(at+0xE,angle);
 u32 obj=load<u32>(at);u32 flags=load<u32>(obj+0x10);actor=load<u32>(at+4);
 if((flags&0x8000)&&(load<u32>(target+0x2E0)&0x380000))store<u8>(at+8,0);
 if(actor && load<s16>(actor+8)==446 && load<s8>(target+0x3A9)!=0) {
  s8 left=load<s8>(target+0x3A9);s32 room=load<s8>(target+0x326);u8 bit=load<u8>(target+0x3A8);u32 table=load<u32>(target+0x3A4);
  store<u8>(target+0x3A9,(u8)(left-1));call(0x025D9000,target+0x314,table,room,0u,(u32)bit);
  store<u8>(target+0x3A8,load<u8>(target+0x3A8)+1);store<u8>(at+8,0);
 } else {
  u8 damage=load<u8>(at+8);s8 health=load<s8>(target+0x3A1);
  if((s8)damage>0){health=(s8)(health-damage);store<u8>(target+0x3A1,(u8)health);
   u8 cut=load<u8>(player+0x3AC);if((cut==8||cut==9)&&load<u8>(player+0x69E8)!=0){health=(s8)(health-load<u8>(at+8));store<u8>(target+0x3A1,(u8)health);}
  }
 }
 s8 pause=6;
 if(load<s8>(target+0x3A1)<=0) {
  store<u8>(at+9,1);startSound(target,0x2828,0);pause=(s8)(load<s16>(0x1047B696)+6);
  u32 pos=load<u32>(at+0x14);
  if(pos) {
   play=call<u32>(0x025200D4);u32 particle=load<u32>(play+0x5AB0);
   call(0x025A847C,particle,0u,0x10u,pos,0u,0u,0xFFu,0u,0xFFFFFFFFu,0u,0u,0u);
   gabi::Local<unsigned char[6]> rotation;gabi::Local<unsigned char[12]> scale;
   f32 value=load<f32>(0x1004B468);store<u16>(rotation.a+4,0);store<f32>(scale.a,value);store<f32>(scale.a+8,value);store<u16>(rotation.a,0);store<f32>(scale.a+4,value);
   play=call<u32>(0x025200D4);u32 p=load<u32>(play+0x5B2C);u16 a=call<u16>(0x025D6894,target,p);store<u16>(rotation.a+2,a);
   pos=load<u32>(at+0x14);play=call<u32>(0x025200D4);particle=load<u32>(play+0x5AB0);
   call(0x025A847C,particle,0u,0xDu,pos,rotation.a,scale.a,0xFFu,0u,0xFFFFFFFFu,0u,0u,0u);
  }
 } else {
  u32 arg=target && load<s16>(target+8)==190?0x33:0x20;
  if(load<u8>(at+9)) {
   u32 info=call<u32>(0x025157AC,load<u32>(at));u32 id=sound(info,true);startSound(target,id,arg);
   if(load<u8>(at+0xA)==9)goto finish;
   pause=(s8)(load<s16>(0x1047B694)+4);
  } else {
   u32 id;
   if(target && load<s16>(target+8)==216 && load<u8>(target+0x570)==2){id=0x693A;arg=0;}
   else id=atSound(load<u32>(at));
   startSound(target,id,arg);pause=(s8)(load<s16>(0x1047B692)+1);
  }
 }
 if(load<u8>(at+0xA)==1)store<u8>(0x101EACB7,(u8)pause);
finish:
 if(load<u8>(at+8)!=0)call(0x025E1D30,load<u32>(at+0x18));
 return load<u32>(at+4);
}
VERIFY(0x025192A8,attackCheck);
void init(){
 WWHD_FUNC(0x02519780,void);
 store<u32>(0x1046F030,0);store<u32>(0x1046F028,0);store<u32>(0x1046F034,0);store<u32>(0x1046F02C,0);call(0x028F026C,0x101D5800u);
 f32 a=load<f32>(0x1004B474),b=load<f32>(0x1004B478);store<f32>(0x1046F01C,a);store<f32>(0x1046F020,b);
 call(0x028ED6F8,0x1046F024u);call(0x028F026C,0x101D580Cu);call(0x028EAB2C,0x1046F025u);call(0x028F026C,0x101D5818u);
}
VERIFY(0x02519780,init);
}
