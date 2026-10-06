#include "gabi.h"
using namespace gabi;
namespace rain_kazanbai {
static f32 lf(u32 p){return load<f32>(p);}
static void sf(u32 p,f32 v){store<f32>(p,v);}
static void absolutePosition(u32 out,u32 p){sf(out,fadds_ppc(lf(p+0x10),lf(p+4)));sf(out+4,fadds_ppc(lf(p+0x14),lf(p+8)));sf(out+8,fadds_ppc(lf(p+0x18),lf(p+0xC)));}
static f32 distance(u32 a,u32 b){call(0x028E8DE8,at<void>(a),at<void>(b));return call<f32>(0x028F4384);}
static void constructGround(u32 p,u32 stack,u32 other){call(0x02008E0C,at<void>(p));u32 flags=p+0x40,types=p+0x4C;store<u32>(p,flags);store<u32>(p+4,types);store<u32>(stack+4,0x1004F474);for(u32 i=5;i<=10;i++)store<u8>(flags+i,0);store<u32>(stack+0x1C,types);store<u32>(p+0x20,0x1004F484);store<u8>(flags+4,1);store<u32>(stack+0x18,0x1004F494);store<u32>(stack+0x14,0x1004F484);store<u32>(p+0x10,0x1004F474);store<u32>(p+0x50,15);store<u32>(types,0x1004F494);store<u32>(stack+0x20,0x1004F424);store<u32>(flags,0x1004F4A4);}
void move(){WWHD_FUNC(0x02569408,void);
 u32 env=call<u32>(0x02555D0C),packet=load<u32>(env+0xA50),game=call<u32>(0x025200D4),camera=load<u32>(game+0x5AF8);call(0x025200D4);
 Local<u8[312]> work;u32 s=ea(work.get()),absolute=s+0x24,center=s+0x30,wind=s+0x3C,direction=s+0x48,wind2=s+0x60,viewDirection=s+0x6C,doublePos=s+0x78,ground=s+0x90,ground2=s+0xE4;
 call(0x0257DB28,at<void>(wind));constructGround(ground,s,0);
 env=call<u32>(0x02555D0C);s32 count=load<s16>(packet+0x3760),wanted=load<s32>(env+0xA4C);bool execute;
 if(count>wanted){execute=load<s16>(packet+0x3760)!=0;}else{env=call<u32>(0x02555D0C);count=load<s16>(env+0xA4E);store<u16>(packet+0x3760,(u16)count);execute=count!=0;}
 if(execute){
  sf(ground+0x28,lf(0x1004F7E8));sf(ground+0x24,lf(0x1004F7E4));sf(ground+0x2C,lf(0x1004F7EC));game=call<u32>(0x025200D4);f32 height=call<f32>(0x02008974,at<void>(game+0x12A0),at<void>(ground));f32 radius=lf(0x1004F6D0);
  call(0x0256401C,at<void>(camera),at<void>(center),radius,radius);
  if(distance(packet+0x374C,camera+0xDC)>lf(0x1004F7D0))store<u16>(packet+0x3760,0);
  store<u32>(packet+0x374C,load<u32>(camera+0xDC));store<u32>(packet+0x3750,load<u32>(camera+0xE0));store<u32>(packet+0x3754,load<u32>(camera+0xE4));
  u32 windVector=call<u32>(0x0257DAA8);f32 power=call<f32>(0x02578348);
  store<f64>(doublePos+8,load<f64>(0x1004F530));store<f64>(doublePos,(f64)fsubs_ppc(lf(camera+0xE8),lf(camera+0xDC)));store<f64>(doublePos+16,(f64)fsubs_ppc(lf(camera+0xF0),lf(camera+0xE4)));
  call(0x02563E48,at<void>(doublePos),at<void>(viewDirection));f32 zero=lf(0x1004F528);sf(s+0x10,zero);
  f32 cross=call<f32>(0x02010CFC,zero,zero,-lf(windVector),-lf(windVector+8),lf(viewDirection),lf(viewDirection+8));sf(packet+0x375C,cross);
  f32 dot=fmadds(lf(windVector),lf(viewDirection),fmuls_ppc(lf(windVector+8),lf(viewDirection+8))),one=lf(0x1004F550),atten=fsubs_ppc(one,std::fabs(dot));s32 initial=load<s16>(packet+0x3760);
  sf(s+0xC,one);sf(s,lf(0x1004F6D8));sf(packet+0x3758,fmuls_ppc(fmuls_ppc(fmuls_ppc(atten,power),fsubs_ppc(one,std::fabs(lf(windVector+4)))),std::fabs(cross)));
  if(initial>0){
   one=lf(s+0xC);zero=lf(s+0x10);f32 half=lf(0x1004F57C),gravityBase=lf(0x1004F7F0),speedBase=lf(0x1004F72C),speedRange=lf(0x1004F7F8),rate=lf(0x1004F588),epsilon=lf(0x1004F580),angleRange=lf(s),waveStep=lf(0x1004F73C),settledRate=lf(0x1004F7F4),fade=lf(0x1004F554),gravityRange=lf(0x1004F7FC),bias=lf(0x1004F728);sf(s+8,lf(0x1004F724));bool positive=height>zero;u32 settled=0;
   for(s32 i=initial-1;i>=0;i--){
    f32 gravity=-fadds_ppc(call<f32>(0x02019918,gravityRange),gravityBase);f32 speed=fadds_ppc(call<f32>(0x020198D8,speedRange),speedBase);u32 p=packet+0x9C+(u32)i*0x38;u8 state=load<u8>(p);
    if(state==0){
     sf(p+0x28,speed);sf(p+0x24,gravity);store<u16>(p+0x34,0);sf(p+0x10,lf(center));sf(p+0x14,lf(center+4));sf(p+0x18,lf(center+8));sf(p+4,call<f32>(0x02019918,radius));sf(p+8,radius);sf(p+0xC,call<f32>(0x02019918,radius));sf(p+0x2C,zero);sf(p+0x1C,call<f32>(0x020198D8,angleRange));f32 angle=call<f32>(0x020198D8,angleRange);store<u8>(p,load<u8>(p)+1);sf(p+0x20,angle);
    }else if(state==1){
     f32 random=call<f32>(0x02019918,rate);call(0x0200ECD4,at<void>(p+0x28),fsubs_ppc(lf(p+0x28),random),half,rate,epsilon);
     f32 wx=lf(wind),ws=lf(p+0x28),px=lf(p+4),pz=lf(p+0xC);sf(p+4,fmadds(wx,ws,px));sf(p+0xC,fmadds(lf(wind+8),ws,pz));f32 gy=lf(p+0x24),wy=lf(wind+4),py=lf(p+8);f32 dy=fmadds(wy,ws,gy);f32 ax=lf(p+0x1C);sf(p+8,fadds_ppc(py,dy));u32 angle=call<u32>(0x02019510,ax);f32 x=lf(p+4);sf(p+4,fmadds(lf(0x104A44F8+((angle&0xFFFF)>>3)*8),gravityBase,x));
     angle=call<u32>(0x02019510,lf(p+0x20));f32 z=lf(p+0xC),three=lf(0x1004F800);sf(p+0xC,fmadds(lf(0x104A44F8+((angle&0xFFFF)>>3)*8),three,z));angle=call<u32>(0x02019510,lf(p+0x20));f32 az=lf(p+0x20),y=lf(p+8),sin=lf(0x104A44F8+((angle&0xFFFF)>>3)*8),oldAx=lf(p+0x1C);f32 x0=lf(p+4),bx=lf(p+0x10);sf(p+8,fadds_ppc(y,sin));sf(p+0x1C,fadds_ppc(oldAx,waveStep));sf(p+0x20,fadds_ppc(az,waveStep));sf(absolute,fadds_ppc(bx,x0));sf(absolute+4,fadds_ppc(lf(p+0x14),lf(p+8)));sf(absolute+8,fadds_ppc(lf(p+0x18),lf(p+0xC)));f32 dist=distance(absolute,center);s16 timer=load<s16>(p+0x34);
     if(!timer){if(dist>radius){sf(p+0x28,speed);sf(p+0x24,gravity);store<u16>(p+0x34,10);sf(p+0x10,lf(center));sf(p+0x14,lf(center+4));sf(p+0x18,lf(center+8));if(distance(absolute,center)>lf(0x1004F804)){sf(p+4,call<f32>(0x02019918,radius));sf(p+8,call<f32>(0x02019918,radius));sf(p+0xC,call<f32>(0x02019918,radius));store<u8>(p,1);}else{f32 random=call<f32>(0x02019918,lf(0x1004F6EC));call(0x02563F0C,at<void>(absolute),at<void>(center),at<void>(direction));f32 shell=fadds_ppc(radius,random);sf(p+4,fmuls_ppc(lf(direction),shell));sf(p+8,fmuls_ppc(lf(direction+4),shell));store<u8>(p,1);sf(p+0xC,fmuls_ppc(lf(direction+8),shell));}}}else store<u16>(p+0x34,(u16)(timer-1));
     if(positive&&lf(absolute+4)<height){if(settled<100&&(load<u32>(0x101FF558)&7)==((u32)i&7)){sf(p+0x2C,epsilon);sf(absolute+4,height);store<u16>(p+0x34,((u32)i&15)+20);store<u8>(p,2);settled++;}else store<u8>(p,0);}
    }else if(state==2){
     f32 speed=lf(p+0x28),wx=lf(wind),scaled=fmuls_ppc(speed,settledRate),x=lf(p+4),baseY=lf(p+0x14),alpha=lf(p+0x2C),z=lf(p+0xC),dy=fsubs_ppc(height,baseY);sf(p+4,fmadds(wx,scaled,x));f32 wz=lf(wind+8),delta=fsubs_ppc(one,alpha),seven=lf(0x1004F808);s16 timer=load<s16>(p+0x34);sf(p+0xC,fmadds(wz,scaled,z));sf(p+8,fnmsubs(delta,seven,dy));if(timer)store<u16>(p+0x34,(u16)(timer-1));if(!(lf(p+0x2C)>lf(0x1004F58C)))store<u8>(p,0);settled++;
    }
    absolutePosition(absolute,p);f32 d=fadds_ppc(distance(absolute,center),bias);if(!(d>=0))d=zero;f32 alpha=fsubs_ppc(one,d/radius);if(alpha>one)alpha=one;else if(!(alpha>=0))alpha=zero;
    env=call<u32>(0x02555D0C);s32 requested=load<s32>(env+0xA4C);state=load<u8>(p);if(i>=requested-1||state==2){if(state==2){if(load<s16>(p+0x34))call(0x0200ECD4,at<void>(p+0x2C),one,half,rate,lf(s+8));else call(0x0200ECD4,at<void>(p+0x2C),zero,rate,waveStep,lf(s+8));}else call(0x0200ECD4,at<void>(p+0x2C),zero,fade,rate,epsilon);}else sf(p+0x2C,alpha);
    env=call<u32>(0x02555D0C);requested=load<s32>(env+0xA4C);if(i>=requested-1&&lf(p+0x2C)<epsilon){s32 n=load<s16>(packet+0x3760);if((u32)i==(u32)(n-1))store<u16>(packet+0x3760,(u16)(n-1));}
   }
  }
  env=call<u32>(0x02555D0C);u32 settlingPacket=load<u32>(env+0xA50);call(0x025200D4);call(0x025200D4);call(0x0257DB28,at<void>(wind2));call(0x02008E0C,at<void>(ground2));u32 flags=ground2+0x40,types=ground2+0x4C;store<u32>(ground2+0x20,load<u32>(s+0x14));store<u32>(types,load<u32>(s+0x18));store<u32>(ground2+0x50,15);for(u32 j=5;j<=10;j++)store<u8>(flags+j,0);store<u32>(ground2+4,types);store<u32>(ground2,flags);store<u32>(flags,0x1004F4A4);store<u8>(flags+4,1);store<u32>(ground2+0x10,load<u32>(s+4));
  env=call<u32>(0x02555D0C);if(load<u32>(env+0xA4C)){
   sf(ground2+0x28,lf(0x1004F7E8));store<u16>(settlingPacket+0x3762,49);sf(ground2+0x24,lf(0x1004F7E4));sf(ground2+0x2C,lf(0x1004F7EC));game=call<u32>(0x025200D4);f32 groundHeight=call<f32>(0x02008974,at<void>(game+0x12A0),at<void>(ground2));f32 zero2=lf(s+0x10),range,bx,bz;if(groundHeight>zero2){range=lf(0x1004F720);bx=lf(0x1004F80C);bz=lf(0x1004F810);}else{range=lf(0x1004F81C);bx=lf(0x1004F814);bz=lf(0x1004F818);}call(0x0257DAA8);call(0x02578348);s32 n=load<s16>(settlingPacket+0x3762);
   if(n+200>=200){f32 angleRange=lf(s),drift=lf(0x1004F734),gravityRange=lf(0x1004F800),tiny=lf(0x1004F5E8),gravityBase=lf(0x1004F744),timerRange=lf(0x1004F6D4),speedRange=lf(0x1004F7F8),one2=lf(s+0xC),speedBase=lf(0x1004F72C),half=lf(0x1004F57C),fadeHeight=fsubs_ppc(groundHeight,lf(0x1004F56C)),five=lf(0x1004F730),rate=lf(0x1004F588);sf(s+4,fadeHeight);
    for(s32 i=n+200,remaining=n+1;remaining;i--,remaining--){f32 gravity=-fadds_ppc(call<f32>(0x02019918,gravityRange),gravityBase),speed=fadds_ppc(call<f32>(0x020198D8,speedRange),speedBase);u32 p=settlingPacket+0x9C+(u32)i*0x38;u8 state=load<u8>(p);if(state==0){sf(p+0x28,speed);sf(p+0x24,gravity);sf(p+0x10,bx);sf(p+0x14,zero2);sf(p+0x18,bz);sf(p+4,call<f32>(0x02019918,range));sf(p+8,groundHeight);sf(p+0xC,call<f32>(0x02019918,range));sf(p+0x2C,zero2);sf(p+0x1C,call<f32>(0x020198D8,angleRange));sf(p+0x20,call<f32>(0x020198D8,angleRange));f32 t=fadds_ppc(call<f32>(0x020198D8,timerRange),timerRange);store<u8>(p,load<u8>(p)+1);store<u16>(p+0x34,(u16)ftoi(t));}
     else if(state==1){s16 timer=load<s16>(p+0x34);f32 speed=lf(p+0x28),alpha=lf(p+0x2C);if(timer)store<u16>(p+0x34,(u16)(timer-1));f32 motion=fmuls_ppc(fmuls_ppc(speed,alpha),drift),wx=lf(wind2),x=lf(p+4),z=lf(p+0xC);sf(p+4,fmadds(wx,motion,x));f32 wz=lf(wind2+8),delta=fsubs_ppc(one2,alpha),height=lf(s+4);timer=load<s16>(p+0x34);sf(p+0xC,fmadds(wz,motion,z));sf(p+8,fnmsubs(delta,five,height));if(timer<=0){if(lf(p+0x2C)<tiny){timer=load<s16>(p+0x34);store<u8>(p,0);}if(!timer){call(0x0200ECD4,at<void>(p+0x2C),zero2,rate,drift,tiny);continue;}}call(0x0200ECD4,at<void>(p+0x2C),one2,half,rate,tiny);}
    }
   }
  }
  store<u32>(ground2+0x20,0x1004F404);store<u32>(flags,load<u32>(s+0x20));store<u32>(types,0x1004F3E4);call(0x02008DAC,at<void>(ground2),0);
 }
 store<u32>(ground+0x20,0x1004F404);store<u32>(ground+0x40,load<u32>(s+0x20));store<u32>(load<u32>(s+0x1C),0x1004F3E4);call(0x02008DAC,at<void>(ground),0);
}
}
VERIFY(0x02569408,rain_kazanbai::move);
