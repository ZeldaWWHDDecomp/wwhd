// HD wind lines and inlined seabird update.
#include "gabi.h"
using namespace gabi;
static u32 windWord(u32 a,u32 o=0){return load<u32>(a+o);}
static f32 windFloat(u32 a,u32 o=0){return load<f32>(a+o);}
static f32 windSin(u32 angle){return windFloat(0x104A44F8+((u16)angle>>3)*8);}
static f32 windCos(u32 angle){return windFloat(0x104A44FC+((u16)angle>>3)*8);}
// The caller owns vectors12, SafeString pairs8 and the derived ground query84.
// Preserve their nonoverlapping original frame positions, including base-subobject aliases.
struct WindScratch {u8 bytes[0x158];};
static void windInvalidate(u32 slot){u32 p=windWord(slot);call<void>(0x0281DE68,p,1);p=windWord(slot);u32 flags=windWord(p,0x254);store<u32>(p+0x5C,0xFFFFFFFF);store<u32>(p+0x254,flags|1);store<u32>(slot,0);}
static void windScale(u32 emitter,f32 scale){for(u32 o:{0x220u,0x224u,0x228u,0x238u,0x23Cu,0x240u})store<f32>(emitter+o,scale);}
static void windTranslate(u32 emitter,u32 position){f32 x=windFloat(position),y=windFloat(position,4),z=windFloat(position,8);store<f32>(emitter+0x22C,x);u32 flip=load<u8>(emitter+0x262);store<f32>(emitter+0x230,y);store<f32>(emitter+0x234,z);if(flip>=7)store<f32>(emitter+0x230,-windFloat(emitter,0x230));}
static void windTotal(u32 slot,u32 pos){store<f32>(pos,fadds_ppc(windFloat(slot,4),windFloat(slot,0x10)));store<f32>(pos+4,fadds_ppc(windFloat(slot,8),windFloat(slot,0x14)));store<f32>(pos+8,fadds_ppc(windFloat(slot,0xC),windFloat(slot,0x18)));}
void rainWindMove(){
 WWHD_FUNC(0x025642AC,void);
 Local<WindScratch> scratch;u32 sp=scratch.a-8;
 u32 env=call<u32>(0x02555D0C),packet=windWord(env,0xAAC);
 u32 camera=windWord(call<u32>(0x025200D4),0x5AF8),player=windWord(call<u32>(0x025200D4),0x5B2C);
 call<void>(0x02008E0C,sp+0x108);
 store<u32>(sp+0x44,sp+0x148);store<u32>(sp+0x4C,sp+0x154);
 for(u32 o:{0x14Du,0x14Eu,0x14Fu,0x150u,0x151u,0x152u})store<u8>(sp+o,0);
 store<u32>(sp+0x154,0x1004F494);store<u32>(sp+0x10C,sp+0x154);store<u32>(sp+0x118,0x1004F474);store<u32>(sp+0x128,0x1004F484);store<u32>(sp+0x108,sp+0x148);store<u32>(sp+0x50,0x1004F404);store<u32>(sp+0x158,15);store<u32>(sp+0x58,0x1004F3E4);store<u8>(sp+0x14C,1);store<u32>(sp+0x54,0x1004F424);store<u32>(sp+0x148,0x1004F4A4);
 u32 direction=call<u32>(0x0257DAA8);f32 power=call<f32>(0x02578348);
 f32 windX=windFloat(direction),windZ=windFloat(direction,8),windY=windFloat(direction,4);
 call<u32>(0x025200D4);s32 count=windWord(env,0xAA8);u32 particlesRoot=windWord(0x1047B2D4);u32 particles=windWord(particlesRoot,0x24)-windWord(particlesRoot,8);
 f32 range=windFloat(0x1004F540),spawnDistance=windFloat(0x1004F53C),offsetY=range,scale=windFloat(0x1004F550);
 store<f32>(sp+0x28,windFloat(0x1004F548));store<f32>(sp+0x30,windFloat(0x1004F544));store<f32>(sp+0x34,scale);store<f32>(sp+0x18,range);store<f32>(sp+0x48,windFloat(0x1004F554));store<f32>(sp+0x1C,windFloat(0x1004F54C));
 f32 distance=call<f32>(0x028E8DE8,packet+0x758,camera+0xDC);distance=call<f32>(0x028F4384,distance);distance/=windFloat(0x1004F558);if(distance>windFloat(sp,0x34))distance=windFloat(sp,0x34);
 f32 speed=fmadds(windFloat(0x1004F560),distance,windFloat(0x1004F55C));
 store<u32>(packet+0x758,windWord(camera,0xDC));store<u32>(packet+0x75C,windWord(camera,0xE0));store<u32>(packet+0x760,windWord(camera,0xE4));store<f32>(sp+0x38,speed);
 if(windWord(call<u32>(0x025200D4),0x5CDC)&0x20)count=10;
 f32 custom=windFloat(env,0xA20),zero=windFloat(0x1004F528);u32 oldCustom=load<u8>(packet+0x764);store<f32>(sp+0x2C,zero);store<f32>(sp+0x24,windFloat(0x1004F52C));bool reset=false;
 if(custom>zero){range=windFloat(0x1004F568);offsetY=windFloat(sp,0x24);spawnDistance=windFloat(0x1004F564);scale=windFloat(0x1004F578);count=9;store<f32>(sp+0x1C,offsetY);store<f32>(sp+0x38,windFloat(0x1004F56C));store<f32>(sp+0x18,windFloat(0x1004F574));store<f32>(sp+0x48,windFloat(sp,0x2C));store<f32>(sp+0x28,range);store<f32>(sp+0x30,windFloat(0x1004F570));if(!oldCustom){store<u8>(packet+0x764,1);reset=true;}}
 else if(oldCustom){store<u8>(packet+0x764,0);reset=true;}
 if(reset)for(u32 i=0;i<30;i++){u32 slot=packet+i*0x34;if(windWord(slot))windInvalidate(slot);store<u32>(slot+0x24,0);}
 call<void>(0x0256401C,camera,sp+0xC8,spawnDistance,spawnDistance);
 store<f32>(sp+0x5C,fmadds(windX,windX,fmuls_ppc(windZ,windZ)));
 const f32 half=windFloat(0x1004F57C),epsilon=windFloat(0x1004F580),fifth=windFloat(0x1004F554),third=windFloat(0x1004F584),tenth=windFloat(0x1004F588),fourth=windFloat(0x1004F598),angleMaximum=windFloat(0x1004F59C),height=windFloat(0x1004F594);
 store<f32>(sp+0x40,windFloat(0x1004F58C));store<f32>(sp+0x3C,windFloat(0x1004F590));f32 lineZero=windFloat(sp,0x2C),one=windFloat(sp,0x34);bool enough=!(power<third);
 for(s32 i=0;i<30;i++){
  u32 slot=packet+i*0x34,state=windWord(slot,0x24);if(i>=count&&!state){if(windWord(slot))windInvalidate(slot);continue;}
  if(!state){if(!enough||particles>1500)continue;u32 tick=windWord(0x101FF558);if(((tick>>4)&7)==((u32)i&3)&&!load<u8>(packet+0x764))continue;
   store<f32>(slot+0x1C,lineZero);store<f32>(slot+0x20,lineZero);store<u16>(slot+0x2C,0);
   u32 base=load<u8>(packet+0x764)==1?player+0x314:sp+0xC8;f32 x=windFloat(base),y=windFloat(base,4),z=windFloat(base,8);store<f32>(slot+4,x);store<f32>(slot+8,fadds_ppc(y,offsetY));store<f32>(slot+0xC,z);
   store<f32>(slot+0x10,call<f32>(0x02019918,range));store<f32>(slot+0x14,call<f32>(0x02019918,range));store<f32>(slot+0x18,call<f32>(0x02019918,range));f32 random=call<f32>(0x020198D8,one);f32 spread=fmadds(windFloat(sp,0x28),random,windFloat(sp,0x28));
   x=fnmsubs(windX,spread,windFloat(slot,0x10));y=fnmsubs(windY,spread,windFloat(slot,0x14));z=fnmsubs(windZ,spread,windFloat(slot,0x18));store<f32>(slot+0x10,x);store<f32>(slot+0x14,y);store<f32>(slot+0x18,z);
   random=call<f32>(0x020198D8,angleMaximum);store<u16>(slot+0x2C,(u16)ftoi(random));windTotal(slot,sp+0x68);
   if(load<u8>(packet+0x764)!=1){store<u32>(sp+0x12C,windWord(sp+0x68));store<f32>(sp+0x130,fadds_ppc(windFloat(sp,0x6C),height));store<u32>(sp+0x134,windWord(sp+0x70));u32 game=call<u32>(0x025200D4);f32 ground=call<f32>(0x02008974,game+0x12A0,sp+0x108);
    if(windFloat(sp,0x6C)<ground){f32 raised=fadds_ppc(ground,offsetY);random=call<f32>(0x020198D8,offsetY);store<f32>(slot+0x14,fsubs_ppc(fadds_ppc(raised,random),windFloat(slot,8)));}
   }
   windTotal(slot,sp+0x68);u32 game=call<u32>(0x025200D4);u32 emitter=call<u32>(0x025A847C,windWord(game,0x5AB0),0,0x31,sp+0x68,0,0,255,0,-1,0,0,0);store<u32>(slot,emitter);if(emitter){store<u8>(emitter+0x247,0);windScale(windWord(slot),scale);store<u32>(slot+0x24,1);}
   f32 horizontal=call<f32>(0x028F4384,windFloat(sp,0x5C));store<f32>(sp+0x20,horizontal);u32 angle=call<u32>(0x020195B0,windX,windZ);store<u16>(slot+0x30,angle);angle=call<u32>(0x020195B0,windY,windFloat(sp,0x20));store<u32>(slot+0x28,0);store<u16>(slot+0x2E,angle);random=call<f32>(0x020198D8,one);store<u8>(slot+0x32,!(random>windFloat(sp,0x48)));continue;
  }
  if(state==3){if(!load<u8>(packet+0x764))store<u32>(slot+0x24,0);continue;}if(state>3)continue;
  s32 oldAngle=load<s16>(slot+0x2C),oldPitch=load<s16>(slot+0x2E);u16 phase=(u16)ftoi(fmadds(windFloat(sp,0x18),power,(f32)oldAngle));f32 deficit=fsubs_ppc(one,power);f32 sway=windFloat(sp,0x1C);sway=fnmsubs(fmuls_ppc(sway,fifth),deficit,sway);store<u16>(slot+0x2C,phase);
  store<u16>(slot+0x2E,(u16)ftoi(fmadds(windSin(phase),sway,(f32)oldPitch)));s32 yaw=load<s16>(slot+0x30);store<u16>(slot+0x30,(u16)ftoi(i&1?fmadds(windSin(load<u16>(slot+0x2C)),sway,(f32)yaw):fnmsubs(windSin(load<u16>(slot+0x2C)),sway,(f32)yaw)));
  if(windFloat(slot,0x1C)>fourth&&load<u8>(slot+0x32)==1){s32 add=3600+i*200;u32 counter=windWord(slot,0x28)+add;store<u32>(slot+0x28,counter);store<u16>(slot+0x2E,load<s16>(slot+0x2E)+add);if((s32)counter>60535)store<u8>(slot+0x32,0);}
  else{f32 horizontal=call<f32>(0x028F4384,windFloat(sp,0x5C));store<f32>(sp+0x20,horizontal);u32 targetYaw=call<u32>(0x020195B0,windX,windZ);u32 targetPitch=call<u32>(0x020195B0,windY,windFloat(sp,0x20));call<void>(0x0200F378,slot+0x2E,targetPitch,10,1000,1);call<void>(0x0200F378,slot+0x30,targetYaw,10,1000,1);}
  u32 pitch=load<u16>(slot+0x2E),heading=load<u16>(slot+0x30);f32 moveX=fmuls_ppc(windCos(pitch),windSin(heading));store<f32>(sp+0x74,moveX);store<f32>(sp+0x78,windSin(load<u16>(slot+0x2E)));store<f32>(sp+0x7C,fmuls_ppc(windCos(load<u16>(slot+0x2E)),windCos(load<u16>(slot+0x30))));
  f32 fraction=(f32)load<s16>(slot+0x2C)/angleMaximum;f32 baseSpeed=windFloat(sp,0x38);f32 effective=fnmsubs(fmuls_ppc(baseSpeed,fifth),deficit,baseSpeed);if(fraction>one)fraction=one;else fraction=fraction>=0?fraction:lineZero;
  effective=fmadds(fmuls_ppc(effective,third),fraction,effective);
  store<f32>(slot+0x10,fmadds(moveX,effective,windFloat(slot,0x10)));store<f32>(slot+0x14,fmadds(windFloat(sp,0x78),effective,windFloat(slot,0x14)));store<f32>(slot+0x18,fmadds(windFloat(sp,0x7C),effective,windFloat(slot,0x18)));windTotal(slot,sp+0x68);windTranslate(windWord(slot),sp+0x68);
  f32 step=fmadds(fmuls_ppc(windFloat(sp,0x30),tenth),(f32)(i/30),windFloat(sp,0x30));f32 distanceToEye=call<f32>(0x028E8DE8,sp+0x68,camera+0xDC);distanceToEye=call<f32>(0x028F4384,distanceToEye);distanceToEye/=windFloat(sp,0x24);if(distanceToEye>one)distanceToEye=one;
  u32 red=load<u8>(env+0xB70),green=load<u8>(env+0xB71),blue=load<u8>(env+0xB72);f32 color=fmuls_ppc((f32)(red+green+blue),windFloat(0x1004F5A8))/windFloat(0x1004F5AC);color=fmuls_ppc(color,color);f32 alpha=fmuls_ppc(power,fmuls_ppc(distanceToEye,color));store<f32>(sp+0x20,alpha);if(alpha<half)store<f32>(sp+0x20,half);
  call<void>(0x0201AE48,sp+0x74,sp+0xF4,windFloat(0x1004F54C));call<void>(0x0201AD78,sp+0x68,sp+0xD4,sp+0xF4);store<u32>(sp+0x12C,windWord(sp+0xD4));store<f32>(sp+0x130,fadds_ppc(windFloat(sp,0xD8),height));store<u32>(sp+0x134,windWord(sp+0xDC));
  u32 game=call<u32>(0x025200D4);f32 ground=call<f32>(0x02008974,game+0x12A0,sp+0x108);if(ground>windFloat(sp,0x6C)){step=fmadds((f32)i,epsilon,fifth);store<u32>(slot+0x24,2);}
  store<u8>(windWord(slot)+0x247,(u8)ftoi(fmuls_ppc(fmuls_ppc(windFloat(slot,0x20),windFloat(0x1004F5AC)),windFloat(sp,0x20))));
  if(windWord(slot,0x24)==1){call<void>(0x0200ECD4,slot+0x1C,one,third,fmuls_ppc(step,tenth),epsilon);f32 timer=windFloat(slot,0x1C);if(!(timer<one)){store<u32>(slot+0x24,2);timer=windFloat(slot,0x1C);}if(timer>half)call<void>(0x0200ECD4,slot+0x20,one,half,windFloat(sp,0x3C),windFloat(sp,0x40));}
  else{call<void>(0x0200ECD4,slot+0x1C,lineZero,fourth,fmuls_ppc(step,fmadds((f32)i,epsilon,tenth)),epsilon);f32 timer=windFloat(slot,0x1C);if(!(timer>lineZero)){windInvalidate(slot);store<u32>(slot+0x24,0);u32 custom=load<u8>(packet+0x764);timer=windFloat(slot,0x1C);if(custom==1)store<u32>(slot+0x24,4);}if(timer<half)call<void>(0x0200ECD4,slot+0x20,lineZero,half,windFloat(sp,0x3C),windFloat(sp,0x40));}
 }
 // dKyr_kamome_move is inlined in this HD entry.
 env=call<u32>(0x02555D0C);packet=windWord(env,0xAAC);camera=windWord(call<u32>(0x025200D4),0x5AF8);call<u32>(0x0257DAA8);
 store<u32>(sp+0xEC,0x1004F3A8);store<u32>(sp+0xF0,0x1004F3AC);u32 game=call<u32>(0x025200D4);store<u32>(sp+0x100,game+0x5134);store<u32>(sp+0x104,0x1004F3AC);
 // Actual1004F3AC+14 target0257770C is empty and reads no argument registers.
 call_ptr<void>(windWord(windWord(sp+0xF0),0x14),sp+0xEC);call_ptr<void>(windWord(windWord(sp+0xF0),0x14),sp+0xEC);u32 name=windWord(sp+0xEC);call_ptr<void>(windWord(windWord(sp+0x104),0x14),sp+0x100);
 bool sea=name==windWord(sp+0x100);if(!sea){u32 a=windWord(sp+0xEC),b=windWord(sp+0x100);for(u32 j=0;j<0x40001;j++){u8 left=load<u8>(a+j),right=load<u8>(b+j);if(left!=right)break;if(!left){sea=true;break;}}}
 bool spawn=false;
 if(sea){bool bad=false;u32 e=call<u32>(0x02555D0C);if(load<u8>(e+0x108D)==1){e=call<u32>(0x02555D0C);bad=windFloat(e,0xFC8)>windFloat(sp,0x2C);}
  if(!bad){e=call<u32>(0x02555D0C);if(load<u8>(e+0x108C)==1){e=call<u32>(0x02555D0C);bad=windFloat(e,0xFC8)<windFloat(sp,0x34);}}
  if(!bad){e=call<u32>(0x02555D0C);if(load<u8>(e+0x108D)==2){e=call<u32>(0x02555D0C);bad=windFloat(e,0xFC8)>windFloat(sp,0x2C);}}
  if(!bad){e=call<u32>(0x02555D0C);if(load<u8>(e+0x108C)==2){e=call<u32>(0x02555D0C);bad=windFloat(e,0xFC8)<windFloat(sp,0x34);}}
  spawn=!bad;
 }
 f32 birdZero=windFloat(sp,0x2C),birdOne=windFloat(sp,0x34),negativeMaximum=windFloat(0x1004F5C8),radius=windFloat(0x1004F5B0),vertical=windFloat(0x1004F5C0),maximum=windFloat(0x1004F558),minimum=windFloat(0x1004F55C),negativeMinimum=windFloat(0x1004F5C4),fadeFactor=windFloat(0x1004F588),fadeStep=windFloat(0x1004F5BC),fadeMinimum=windFloat(0x1004F5B8),birdScale=windFloat(0x1004F5B4),birdDelay=windFloat(0x1004F5D4),resetDelay=windFloat(0x1004F5CC),resetSpread=windFloat(0x1004F5D0),angleRange=windFloat(0x1004F59C);
 for(u32 i=0;i<2;i++){
  u32 bird=packet+0x618+i*0x20;u32 state=load<u8>(bird+0x1E);
  if(state==0){if(spawn){s32 timer=load<s16>(bird+0x1C);if(!timer){store<u16>(bird+0x10,(u16)ftoi(call<f32>(0x02019918,angleRange)));f32 r=call<f32>(0x02019918,angleRange);u32 angle=load<u16>(bird+0x10);store<u16>(bird+0x12,(u16)ftoi(r));f32 x=fmadds(windSin(angle),radius,windFloat(camera,0xDC)),z=fmadds(windCos(angle),radius,windFloat(camera,0xE4));store<f32>(bird+8,windFloat(0x1004F5D8));store<f32>(bird+4,x);store<f32>(bird+0xC,z);
    store<f32>(bird+0x14,call<f32>(0x02019918,birdOne));store<f32>(bird+0x18,birdZero);r=call<f32>(0x020198D8,windFloat(0x1004F5DC));store<u16>(bird+0x1C,(u16)ftoi(fadds_ppc(r,birdDelay)));game=call<u32>(0x025200D4);u32 emitter=call<u32>(0x025A847C,windWord(game,0x5AB0),0,0x429,bird+4,0,0,255,0,-1,0,0,0);u8 old=load<u8>(bird+0x1E);store<u32>(bird,emitter);store<u8>(bird+0x1E,old+1);
   }else store<u16>(bird+0x1C,timer-1);}}
  else if(state==1&&windWord(bird)){
   f32 old=windFloat(bird,0x14),r=call<f32>(0x02019918,birdOne),speed=fadds_ppc(old,r);if(old<birdZero){if(speed<negativeMaximum)speed=negativeMaximum;else if(speed>negativeMinimum)speed=negativeMinimum;}else{if(speed>maximum)speed=maximum;else if(speed<minimum)speed=minimum;}store<f32>(bird+0x14,speed);
   store<f32>(sp+0x98,fmuls_ppc(windSin(load<u16>(bird+0x10)),radius));store<f32>(sp+0x9C,std::fabs(fmuls_ppc(windSin(load<u16>(bird+0x12)),vertical)));store<f32>(sp+0xA0,fmuls_ppc(windCos(load<u16>(bird+0x10)),radius));
   s32 angle=load<s16>(bird+0x10);u16 heading=(u16)ftoi(fmadds(windFloat(bird,0x14),windFloat(bird,0x18),(f32)angle));s32 pitch=load<s16>(bird+0x12);store<u16>(bird+0x10,heading);store<u16>(bird+0x12,pitch+15);
   f32 x=fmuls_ppc(windSin(heading),radius),y=std::fabs(fmuls_ppc(windSin(load<u16>(bird+0x12)),vertical)),z=fmuls_ppc(windCos(load<u16>(bird+0x10)),radius);store<f32>(sp+0x8C,x);store<f32>(sp+0x90,y);store<f32>(sp+0x94,z);y=fadds_ppc(y,windFloat(0x1004F5E0));x=fadds_ppc(windFloat(camera,0xDC),x);z=fadds_ppc(windFloat(camera,0xE4),z);store<f32>(bird+8,y);store<f32>(bird+4,x);store<f32>(bird+0xC,z);store<f32>(sp+0xB0,x);store<f32>(sp+0xB4,y);store<f32>(sp+0xB8,z);windTranslate(windWord(bird),sp+0xB0);
   if(load<s16>(bird+0x1C)&&spawn){call<void>(0x0200ECD4,bird+0x18,birdOne,fadeFactor,fadeStep,fadeMinimum);store<u16>(bird+0x1C,load<s16>(bird+0x1C)-1);}
   else{call<void>(0x0200ECD4,bird+0x18,birdZero,fadeFactor,fadeStep,fadeMinimum);if(windFloat(bird,0x18)<windFloat(0x1004F58C))store<u8>(bird+0x1E,load<u8>(bird+0x1E)+1);}
   call<void>(0x02563F64,sp+0x98,sp+0x8C,sp+0xE0);u32 rotation=call<u32>(0x020195B0,windFloat(sp,0xE0),windFloat(sp,0xE8));call<void>(0x028245AC,0,rotation,0,windWord(bird)+0x1F0);
  }
  else if(state==2){windInvalidate(bird);store<u8>(bird+0x1E,0);f32 r=call<f32>(0x020198D8,resetSpread);store<u16>(bird+0x1C,(u16)ftoi(fadds_ppc(r,resetDelay)));}
  if(windWord(bird)){
   game=call<u32>(0x025200D4);if(windWord(game,0x5FA4)){game=call<u32>(0x025200D4);u32 view=windWord(game,0x5FA4);f32 fov=windFloat(view,0xD4)/windFloat(0x1004F5E4);f32 scale=windFloat(bird,0x18);if(!(fov<birdOne))fov=birdOne;windScale(windWord(bird),fmuls_ppc(fmuls_ppc(scale,fov),birdScale));}
   u32 emitter=windWord(bird);u32 red=load<u8>(call<u32>(0x02555D0C)+0xB70);u32 green=load<u8>(call<u32>(0x02555D0C)+0xB71);u32 blue=load<u8>(call<u32>(0x02555D0C)+0xB72);store<u8>(emitter+0x244,red);store<u8>(emitter+0x245,green);store<u8>(emitter+0x246,blue);
  }
 }
 store<u32>(sp+0x128,windWord(sp,0x50));u32 first=windWord(sp,0x44),second=windWord(sp,0x4C),a=windWord(sp,0x54),b=windWord(sp,0x58);store<u32>(first,a);store<u32>(second,b);call<void>(0x02008DAC,sp+0x108,0);
}
VERIFY(0x025642AC,rainWindMove);
