/* WWHD housi particle motion; native02567F68. */
#include "bindings.h"
namespace rain_housi {
static u32 ld(u32 p){return gabi::load<u32>(p);}
static f32 lf(u32 p){return gabi::load<f32>(p);}
static void sf(u32 p,f32 v){gabi::store<f32>(p,v);}
static u16 lh(u32 p){return gabi::load<u16>(p);}
static void sh(u32 p,u16 v){gabi::store<u16>(p,v);}
static void sw(u32 p,u32 v){gabi::store<u32>(p,v);}
static void sb(u32 p,u8 v){gabi::store<u8>(p,v);}
static void move(){
 WWHD_FUNC(0x02567F68,void);
 // Original live frame SP08..SPA7: vectors12, derived ground query84.
 gabi::Local<u8[160]> scratch; u32 b=gabi::ea(scratch.get());
 auto sp=[&](u32 off){return b+off-8;};
 u32 env=gabi::call<u32>(0x02555D0C), packet=ld(env+0xA78);
 u32 game=gabi::call<u32>(0x025200D4), camera=ld(game+0x5AF8);
 gabi::call<u32>(0x025200D4);
 gabi::call(0x0257DB28,sp(0x38)); gabi::call(0x02008E0C,sp(0x50));
 f32 zero=lf(0x1004F528);
 sw(sp(0x60),0x1004F3F4);sw(sp(0x70),0x1004F404);
 sw(sp(0x50),sp(0x90));sw(sp(0x54),sp(0x9C));sw(sp(0x9C),0x1004F414);
 for(u32 k=0x94;k<=0x99;k++)sb(sp(k),0);
 sw(sp(0xA0),1);sw(sp(0x90),0x1004F424);sb(sp(0x9A),0);
 if(ld(gabi::call<u32>(0x02555D0C)+0xA74)!=0 ||
    ld(gabi::call<u32>(0x02555D0C)+0xA74)!=0 || !(lf(packet+0x5E64)>zero))
  sh(packet+0x5E68,ld(gabi::call<u32>(0x02555D0C)+0xA74));
 u32 wanted=ld(gabi::call<u32>(0x02555D0C)+0xA74);
 f32 rate=lf(0x1004F554), inc=lf(0x1004F580);
 gabi::call(0x0200ECD4,packet+0x5E64,wanted?lf(0x1004F550):zero,rate,lf(0x1004F590),inc);
 s32 count=gabi::load<s16>(packet+0x5E68);
 if(count!=0){
  f32 d=lf(0x1004F6D0);gabi::call(0x0256401C,camera,sp(0x20),d,d);
  gabi::call<f32>(0x02578348);
  count=gabi::load<s16>(packet+0x5E68);
  f32 drag=lf(0x1004F588),fall=lf(0x1004F740),degrees=lf(0x1004F6D8),wind=lf(0x1004F730);
  f32 limit=lf(0x1004F728),step=lf(0x1004F734),rot=lf(0x1004F73C),force=lf(0x1004F72C);
  f32 radius=lf(0x1004F720),half=lf(0x1004F57C),floor=lf(0x1004F738),min=lf(0x1004F724);
  for(s32 i=count-1;i>=0;--i){
   u32 p=packet+0x9C+u32(i)*0x50;u8 state=gabi::load<u8>(p);f32 alpha=lf(packet+0x5E64);
   auto position=[&](){for(u32 k=0;k<12;k+=4)sf(sp(8)+k,lf(p+0x10+k)+lf(p+4+k));};
   if(state==0){
    f32 speed=gabi::call<f32>(0x020198D8,lf(0x1004F744));sh(p+0x3C,0);sf(p+0x34,speed+rate);
    f32 angle=gabi::call<f32>(0x02019918,lf(0x1004F748));sh(p+0x4C,u16(gabi::ftoi(angle)));
    for(u32 k=0;k<12;k+=4)sf(p+0x10+k,lf(sp(0x20)+k));
    sf(p+4,gabi::call<f32>(0x02019918,radius));sf(p+8,radius);
    sf(p+12,gabi::call<f32>(0x02019918,radius));sf(p+0x40,zero);sf(p+0x48,zero);
    sf(p+0x28,gabi::call<f32>(0x020198D8,degrees));sf(p+0x2C,gabi::call<f32>(0x020198D8,degrees));
    f32 zrot=gabi::call<f32>(0x020198D8,degrees),baseY=lf(p+0x14),offsetY=lf(p+8);
    u8 now=gabi::load<u8>(p);sf(p+0x30,zrot);sf(p+0x1C,zero);sf(p+0x20,zero);sf(p+0x24,zero);
    if(baseY+offsetY<lf(0x1004F74C))sf(p+8,(lf(0x1004F750)-baseY)+lf(0x1004F6B8));
    sb(p,now+1);
   }else if(state<=3){
    if(state==1){
     u32 a=u16(gabi::call<u32>(0x02019510,lf(p+0x28)));f32 wx=lf(sp(0x38)),x=lf(p+4),speed=lf(p+0x34);
     f32 sine=lf(0x104A44F8+(a>>3)*8);x=gabi::fmadds(wx*wind,speed,x);sf(p+4,x);
     f32 y=gabi::fmadds(lf(sp(0x3C))*wind,speed,lf(p+8));sf(p+8,y);
     x=gabi::fmadds(sine,speed,x);f32 z=gabi::fmadds(lf(sp(0x40))*wind,speed,lf(p+12));
     f32 ry=lf(p+0x2C);y=gabi::fnmsubs(speed,fall,y);sf(p+12,z);sf(p+8,y);sf(p+4,x);
     a=u16(gabi::call<u32>(0x02019510,ry));speed=lf(p+0x34);y=lf(p+8);
     y=gabi::fmadds(lf(0x104A44F8+(a>>3)*8),speed*half,y);f32 rz=lf(p+0x30);sf(p+8,y);
     a=u16(gabi::call<u32>(0x02019510,rz));speed=lf(p+0x34);z=lf(p+12);
     z=gabi::fmadds(lf(0x104A44F8+(a>>3)*8),speed,z);
     f32 rx=lf(p+0x28),ry2=lf(p+0x2C),rz2=lf(p+0x30);sf(p+12,z);
     sf(p+0x28,rx+rot);sf(p+0x2C,ry2+step);sf(p+0x30,rz2+inc);
    }
    position();gabi::call(0x0257E128,sp(0x2C),sp(8));
    f32 vx=lf(sp(0x2C)),vy=lf(sp(0x30)),vz=lf(sp(0x34));sf(sp(0x14),vx);sf(sp(0x18),vy);sf(sp(0x1C),vz);
    f32 oldx=lf(p+0x1C),oldy=lf(p+0x20);if(oldx<limit)sf(p+0x1C,gabi::fmadds(vx,force,oldx));
    f32 oldz=lf(p+0x24);if(oldy<limit)sf(p+0x20,gabi::fmadds(lf(sp(0x18)),force,oldy));
    if(oldz<limit)sf(p+0x24,gabi::fmadds(lf(sp(0x1C)),force,oldz));
    for(u32 k=0;k<12;k+=4)gabi::call(0x0200ECD4,p+0x1C+k,zero,rate,drag,min);
    vx=lf(p+0x1C);vy=lf(p+0x20);vz=lf(p+0x24);
    f32 x=lf(p+4)+vx,y=lf(p+8)+vy,z=lf(p+12)+vz,bx=lf(p+0x10);
    sf(p+4,x);sf(p+8,y);sf(p+12,z);sf(sp(8),bx+x);
    sf(sp(12),lf(p+0x14)+lf(p+8));sf(sp(16),lf(p+0x18)+lf(p+12));
    f32 distance=gabi::call<f32>(0x028E8DE8,sp(8),sp(0x20));distance=gabi::call<f32>(0x028F4384,distance);
    u16 delay=lh(p+0x3C);
    if(delay==0){
     if(distance>radius || lf(sp(12))<floor){
      sh(p+0x3C,10);for(u32 k=0;k<12;k+=4)sw(p+0x10+k,ld(sp(0x20)+k));
      distance=gabi::call<f32>(0x028E8DE8,sp(8),sp(0x20));distance=gabi::call<f32>(0x028F4384,distance);
      if(distance>lf(0x1004F754)){for(u32 k=0;k<12;k+=4)sf(p+4+k,gabi::call<f32>(0x02019918,radius));}
      else{f32 jitter=gabi::call<f32>(0x02019918,lf(0x1004F564));gabi::call(0x02563F0C,sp(8),sp(0x20),sp(0x14));
       f32 scale=jitter+radius;for(u32 k=0;k<12;k+=4)sf(p+4+k,lf(sp(0x14)+k)*scale);}
      sf(p+0x1C,zero);sf(p+0x20,zero);sf(p+0x24,zero);sb(p,1);
     }
    }else sh(p+0x3C,delay-1);
   }
   position();u16 angle=lh(p+0x4C)+600;sh(p+0x4C,angle);if(angle>30000)alpha=zero;
   gabi::call(0x0200ECD4,p+0x40,alpha,half,step,min);
  }
 }
 sw(sp(0x70),0x1004F404);sw(sp(0x90),0x1004F424);sw(sp(0x9C),0x1004F3E4);
 gabi::call(0x02008DAC,sp(0x50),0);
}
VERIFY(0x02567F68,move);
}
