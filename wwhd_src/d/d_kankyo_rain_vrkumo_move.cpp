// HD cloud particle motion;
#include "gabi.h"
using namespace gabi;
static u32 cw(u32 a,u32 o=0){return load<u32>(a+o);}
static f32 cf(u32 a,u32 o=0){return load<f32>(a+o);}
static f32 cs(u32 a){return cf(0x104A44F8+((u16)a>>3)*8);}
static f32 cc(u32 a){return cf(0x104A44FC+((u16)a>>3)*8);}
struct CloudFrame{u8 bytes[0xE4];};
// Every SafeString has its real8 bytes; the resolved virtual is the empty0257770C.
static bool cloudStage(u32 sp,u32 literal,u32 left,u32 right){
 store<u32>(sp+left,literal);store<u32>(sp+left+4,0x1004F3AC);
 u32 play=call<u32>(0x025200D4);store<u32>(sp+right,play+0x5134);store<u32>(sp+right+4,0x1004F3AC);
 call_ptr<void>(cw(cw(sp+left+4),0x14),sp+left);call_ptr<void>(cw(cw(sp+left+4),0x14),sp+left);
 u32 a=cw(sp+left);call_ptr<void>(cw(cw(sp+right+4),0x14),sp+right);u32 b=cw(sp+right);
 if(a==b)return true;a=cw(sp+left);b=cw(sp+right);
 for(u32 i=0;i<0x40001;i++){u8 x=load<u8>(a+i),y=load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;
}
static void cloudVector(u32 target,u32 slot){store<u32>(target,cw(slot,4));store<f32>(target+4,0);store<u32>(target+8,cw(slot,12));}
void rainCloudMove(){
 WWHD_FUNC(0x0256DDF8,void);
 Local<CloudFrame> frame;u32 sp=frame.a-8;
 call<void>(0x0257DB28,sp+0x4C);u32 packet=cw(call<u32>(0x02555D0C),0xA94),camera=cw(call<u32>(0x025200D4),0x5AF8);
 f32 strength=cf(call<u32>(0x02555D0C),0xA90),zero=cf(0x1004F528),one=cf(0x1004F550);
 f32 vertical=fmadds(cf(0x1004FAE0),strength,cf(0x1004F7E8)),edge=fmadds(zero,strength,cf(0x1004FAE4));
 f32 maximum=fmadds(cf(0x1004FAE8),strength,one),speed=fmadds(cf(0x1004FAD8),strength,cf(0x1004FAD4)),height=fmadds(cf(0x1004FADC),strength,cf(0x1004F720));
 store<f32>(sp+0x1C,vertical);store<f32>(sp+0x10,maximum);
 bool dragon=cloudStage(sp,0x1004FB24,0x6C,0xA4);
 if(dragon){store<f32>(sp+0x50,zero);height=cf(0x1004F5D4);store<f32>(sp+0x54,zero);store<f32>(sp+0x4C,cf(0x1004FAEC));}
 else if(!cloudStage(sp,0x1004FB44,0x74,0xAC)){
  u32 play=call<u32>(0x025200D4),info=call_ptr<u32>(cw(cw(play+0x5150),0x15C),play+0x5150);
  if(((cw(info,12)>>16)&7)==2){s32 offset=0;
   if(cloudStage(sp,0x1004FB4C,0x7C,0xB4))offset=0x4000;
   else if(cloudStage(sp,0x1004FB54,0x84,0xBC))offset=-0x4000;
   else if(cloudStage(sp,0x1004FB2C,0x8C,0xC4))offset=0x7FFF;
   else if(cloudStage(sp,0x1004FB5C,0x94,0xCC))offset=-0x4000;
   else if(cloudStage(sp,0x1004FB34,0x9C,0xD4))offset=0x4000;
   u32 save=cw(0x101F84DC);u32 x=0,y=(u16)offset;
   if(load<s16>(save+0x4A)!=-1||load<s16>(save+0x4C)!=-1){x=load<u16>(call<u32>(0x02555D0C)+0xA24);y=(u16)(load<s16>(call<u32>(0x02555D0C)+0xA26)+offset);}
   f32 scale=cf(0x1004F740),sy=cs(x),cx=cc(x),cy=cc(y),sx=cs(y);
   store<f32>(sp+0x50,fmuls_ppc(sy,scale));store<f32>(sp+0x4C,fmuls_ppc(fmuls_ppc(cx,cy),scale));store<f32>(sp+0x54,fmuls_ppc(fmuls_ppc(cx,sx),scale));
  }
 }
 if(cw(call<u32>(0x025200D4),0x5FA4)){
  s32 stay=load<s8>(0x1047E6C8);f32 sea=zero;
  if(stay>=0){u32 play=call<u32>(0x025200D4),room=call<u32>(0x025C11DC,play+0x51CC,stay);u32 file=call_ptr<u32>(cw(cw(room),0x1DC),room);if(file)sea=cf(file,4);}
  if(cloudStage(sp,0x1004FB3C,0x64,0xDC)){s32 room=load<s8>(0x1047E6C8);if(room==17||room==18)sea=cf(0x1004FAF0);}
  if(cloudStage(sp,0x1004FB64,0x64,0xE4))sea=cf(0x1004FAF4);
  height=fnmsubs(fsubs_ppc(cf(camera,0xE0),sea),cf(0x1004FAF8),height);
 }
 f32 fadeEdge=cf(0x1004FAFC),epsilon=cf(0x1004F580),alphaStep=cf(0x1004F588),smallStep=cf(0x1004FB08),half=cf(0x1004F57C),angleMax=cf(0x1004F59C),radius=cf(0x1004FB00),hundred=cf(0x1004F558),respawn=cf(0x1004FB04),alphaFactor=cf(0x1004F554),spawnMax=cf(0x1004FB0C),speedRange=cf(0x1004FAD4),low=cf(0x1004F590),randomHeight=cf(0x1004F584),heightStep=cf(0x1004F7CC);
 f32 edgeSpan=fsubs_ppc(edge,fadeEdge);bool noSpan=!(edgeSpan>zero);
 store<f32>(sp+12,epsilon);store<f32>(sp+8,smallStep);store<f32>(sp+0x20,alphaStep);store<f32>(sp+0x14,half);store<f32>(sp+0x18,heightStep);
 store<f32>(packet+0x11D4,fadds_ppc(cf(packet,0x11D4),hundred));
 for(s32 i=0;i<100;i++){
  u32 slot=packet+0xA4+i*0x2C;u32 state=load<u8>(slot);f32 fraction=(f32)i/hundred;
  if(state==0){u32 angle=(u16)ftoi(call<f32>(0x020198D8,angleMax));f32 distance=call<f32>(0x020198D8,spawnMax);
   if(distance>radius)distance=(f32)((f64)call<f32>(0x020198D8,cf(0x1004F720))+load<f64>(0x1004FB10));
   u32 a=call<u32>(0x02019510,(f32)angle);store<f32>(slot+4,fmuls_ppc(cs(a),distance));store<f32>(slot+8,zero);
   a=call<u32>(0x02019510,(f32)angle);store<f32>(slot+12,fmuls_ppc(cc(a),distance));
   f32 random=call<f32>(0x02019918,randomHeight);store<f32>(slot+0x1C,fmuls_ppc(randomHeight,random));store<f32>(slot+0x20,zero);
   random=call<f32>(0x020198D8,speedRange);u8 old=load<u8>(slot);store<u8>(slot,old+1);store<f32>(slot+0x28,fadds_ppc(random,cf(sp,0x14)));state=1;
  }
  if(state==1){cloudVector(sp+0x58,slot);f32 distance=call<f32>(0x028E8DD0,sp+0x58);distance=call<f32>(0x028F4384,distance);
   if(distance>radius){distance=call<f32>(0x028E8DD0,sp+0x58);distance=call<f32>(0x028F4384,distance);
    if(distance>cf(0x1004FB20)){store<f32>(slot+4,call<f32>(0x02019918,respawn));store<f32>(slot+12,call<f32>(0x02019918,respawn));}
    else{f32 x=cf(slot,4),z=cf(slot,12);store<f32>(slot+4,-x);store<f32>(slot+12,-z);}store<f32>(slot+0x20,zero);
   }
   if(!(cf(slot,0x20)>zero)){f32 advance=fmadds(fraction,speed,speed);store<f32>(slot+4,fmadds(cf(sp,0x4C),advance,cf(slot,4)));f32 dz=fmuls_ppc(cf(sp,0x54),advance);store<f32>(slot+12,fadds_ppc(cf(slot,12),dz));}
   else{f32 advance=fmuls_ppc(speed,cf(slot,0x28)),x=cf(slot,4),wx=cf(sp,0x4C),falloff=cf(slot,0x24);store<f32>(slot+4,fmadds(fmuls_ppc(wx,advance),falloff,x));f32 dz=fmuls_ppc(fmuls_ppc(cf(sp,0x54),advance),falloff);store<f32>(slot+12,fadds_ppc(cf(slot,12),dz));}
  }
  cloudVector(sp+0x40,slot);f32 distance=call<f32>(0x028E8DD0,sp+0x40);distance=call<f32>(0x028F4384,distance);f32 ratio=distance/radius;
  f32 remaining=fsubs_ppc(one,ratio);if(!(remaining>=0))remaining=zero;f32 shape=fsubs_ppc(one,remaining);shape=fnmsubs(fmuls_ppc(shape,shape),shape,one);
  f32 base=fmadds(cf(sp,0x1C),shape,height);store<f32>(slot+8,fmadds(fraction,cf(sp,0x18),base));if(ratio>one)ratio=one;
  f32 square=fmuls_ppc(ratio,ratio),cube=fmuls_ppc(square,ratio),fourth=fmuls_ppc(cube,ratio),fifth=fmuls_ppc(fourth,ratio);store<f32>(slot+0x24,fnmsubs(fifth,ratio,one));
  if(cloudStage(sp,0x1004FB24,0x30,0x38)){store<f32>(slot+0x20,one);continue;}
  f32 step=cf(sp,0x20),target;u32 env=call<u32>(0x02555D0C);s32 count=cw(env,0xA8C);
  if(i>=count){step=cf(sp,8);target=zero;}
  else{f32 falloff=cf(slot,0x24);if(falloff<alphaFactor){target=!(falloff<low)?fsubs_ppc(falloff,low)/cf(0x1004F544):zero;}
   else{env=call<u32>(0x02555D0C);if(i>= (s32)cw(env,0xA8C)){falloff=cf(slot,0x24);target=!(falloff<low)?fsubs_ppc(falloff,low)/cf(0x1004F544):zero;}else target=cf(sp,0x10);}}
  if(shape>edge)target=zero;else if(shape>fadeEdge){if(noSpan)target=zero;else target=fmuls_ppc(target,fsubs_ppc(edge,shape)/edgeSpan);}
  call<void>(0x0200ECD4,slot+0x20,target,alphaFactor,step,cf(sp,12));
 }
}
VERIFY(0x0256DDF8,rainCloudMove);
