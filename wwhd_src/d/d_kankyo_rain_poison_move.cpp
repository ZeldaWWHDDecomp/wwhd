// HD poison particle motion;
#include "gabi.h"
using namespace gabi;
static u32 pw(u32 a,u32 o=0){return load<u32>(a+o);}
static f32 pf(u32 a,u32 o=0){return load<f32>(a+o);}
static f32 ps(u32 a){return pf(0x104A44F8+((u16)a>>3)*8);}
static f32 pc(u32 a){return pf(0x104A44FC+((u16)a>>3)*8);}
struct PoisonFrame{u8 bytes[0x1A4];};
static void poisonTotal(u32 out,u32 base,u32 slot){for(u32 o:{0u,4u,8u})store<f32>(out+o,fadds_ppc(pf(base,o),pf(slot,4+o)));}
static f32 poisonDistance(u32 a,u32 b){f32 d=call<f32>(0x028E8DE8,a,b);return call<f32>(0x028F4384,d);}
static void poisonCopy(u32 out,u32 in){for(u32 o:{0u,4u,8u})store<u32>(out+o,pw(in,o));}
void rainPoisonMove(){
 WWHD_FUNC(0x0256CA54,void);
 Local<PoisonFrame> frame;u32 sp=frame.a-8;
 u32 packet=pw(call<u32>(0x02555D0C),0xA6C),camera=pw(call<u32>(0x025200D4),0x5AF8),player=pw(call<u32>(0x025200D4),0x5B2C),link=pw(call<u32>(0x025200D4),0x5B2C);
 call<void>(0x0257DB28,sp+0xB0);
 for(u32 o=0;o<240;o+=4)store<u32>(sp+0xBC+o,pw(0x1004F9A0+o));
 s32 room=load<s8>(0x1047E6C8);u32 env=call<u32>(0x02555D0C);if(load<u8>(env+0x10A3)!=255)room=load<u8>(call<u32>(0x02555D0C)+0x10A3);
 if(pw(0x101E99F0)!=(u32)room){store<u32>(0x101E99F0,room);for(u32 i=0;i<1000;i++)store<u8>(packet+0x98+i*0x30,0);}
 s32 pattern=room<16?(s32)pw(0x1004F960+room*4):-1;u32 data=sp+0xBC+(pattern>=0?pattern:1)*48;
 f32 baseX=pf(data),baseY=pf(data,4),baseZ=pf(data,8),rangeX=pf(data,12),rangeZ=pf(data,20),size=pf(data,24),rise=pf(data,28),rangeY=pf(data,16);
 store<f32>(sp+0x38,rise);store<f32>(sp+8,rangeY);env=call<u32>(0x02555D0C);store<u32>(env+0xA68,pattern>=0?pw(data,32):0);
 f32 drift=pf(data,40);u32 tilt=load<u16>(data+36),heading=load<u16>(data+38);u32 base=packet+0xBC18;
 store<f32>(base,baseX);store<f32>(base+8,baseZ);store<f32>(base+4,fadds_ppc(baseY,size));rangeX=fsubs_ppc(rangeX,size);rangeZ=fsubs_ppc(rangeZ,size);
 f32 phase=fadds_ppc(pf(packet,0xBC38),pf(0x1004F924));if(!(phase<pf(0x1004F8C4)))phase=fsubs_ppc(phase,pf(0x1004F8C4));store<f32>(packet+0xBC38,phase);
 store<f32>(sp+0x48,rangeX);store<f32>(sp+0x50,rangeZ);store<f32>(sp+0x1C,pf(0x1004F630));
 s32 count=pw(call<u32>(0x02555D0C),0xA68);
 if(count>0){
 const f32 zero=pf(0x1004F528),one=pf(0x1004F550),tiny=pf(0x1004F58C),fifth=pf(0x1004F554),half=pf(0x1004F57C),five=pf(0x1004F730),ten=pf(0x1004F6B8),push=pf(0x1004F92C),limit=pf(0x1004F928),decay=pf(0x1004F740),far=pf(0x1004F540);
 store<f32>(sp+0x20,fadds_ppc(size,pf(0x1004F938)));store<f32>(sp+12,fadds_ppc(size,pf(0x1004F940)));store<f32>(sp+0x3C,fsubs_ppc(pf(0x1004F930),size));store<f32>(sp+0x28,fsubs_ppc(pf(0x1004F5C0),size));store<f32>(sp+0x34,fadds_ppc(size,pf(0x1004F934)));store<f32>(sp+0x4C,pf(0x1004F558));store<f32>(sp+0x30,fadds_ppc(size,pf(0x1004F6C4)));store<f32>(sp+0x14,fsubs_ppc(pf(0x1004F93C),size));store<f32>(sp+0x10,pf(0x1004F590));store<f32>(sp+0x24,pf(0x1004F854));store<f32>(sp+0x44,fmuls_ppc(size,fifth));store<f32>(sp+0x18,pf(0x1004F81C));
 for(s32 i=0;i<count;i++){
  u32 slot=packet+0x98+i*0x30,state=load<u8>(slot);rangeZ=pf(sp,0x50);rangeX=pf(sp,0x48);rangeY=pf(sp,8);
  if(!state){store<f32>(slot+0x28,zero);
   if(pattern==2){u32 x=(u16)ftoi(call<f32>(0x02019918,pf(0x1004F944)));u32 z=(u16)ftoi(call<f32>(0x02019918,pf(0x1004F944)));store<f32>(slot+4,fmuls_ppc(ps(x),rangeX));store<f32>(slot+12,fmuls_ppc(pc(z),rangeX));store<f32>(slot+8,call<f32>(0x020198D8,rangeY));}
   else{store<f32>(slot+4,call<f32>(0x02019918,rangeX));store<f32>(slot+8,call<f32>(0x020198D8,rangeY));store<f32>(slot+12,call<f32>(0x02019918,rangeZ));}
   store<f32>(slot+0x10,zero);store<f32>(slot+0x14,zero);store<f32>(slot+0x18,zero);store<f32>(slot+0x1C,call<f32>(0x020198D8,pf(0x1004F6D8)));store<f32>(slot+0x20,call<f32>(0x020198D8,one));f32 angle=call<f32>(0x020198D8,pf(0x1004F748));u8 old=load<u8>(slot);store<f32>(slot+0x24,zero);store<u16>(slot+0x2C,(u16)ftoi(angle));store<u16>(slot+0x2E,0);store<u8>(slot,old+1);
  }
  else if(state<=2){
   f32 swirl=state==1?pf(0x1004F744):ten,up=state==1?pf(sp,0x38):five;store<f32>(sp+0x2C,swirl);store<u16>(slot+0x2C,load<s16>(slot+0x2C)+400);
   f32 random=pf(slot,0x20),dy=fmuls_ppc(up,fadds_ppc(random,half)),dx=fmuls_ppc(pf(sp,0xB0),ten),dz=fmuls_ppc(pf(sp,0xB8),ten);
   f32 maxY=fmuls_ppc(rangeY,fmadds(random,pf(sp,0x24),one));dx=fmadds(fmuls_ppc(dx,half),random,dx);dy=fmadds(fmuls_ppc(dy,half),random,dy);dz=fmadds(fmuls_ppc(dz,half),random,dz);store<f32>(sp+0x40,dz);
   call<void>(0x0201AD78,base,sp+0x74,slot+4);store<u32>(sp+0x54,pw(sp,0x74));store<f32>(sp+0x58,fadds_ppc(pf(sp,0x78),pf(0x1004F52C)));store<u32>(sp+0x5C,pw(sp,0x7C));call<void>(0x0257E128,sp+0x74,sp+0x54);
   f32 windY=pf(sp,0x78),windX=pf(sp,0x74),windZ=pf(sp,0x7C);store<f32>(sp+0xA8,windY);f32 px=fmadds(windX,push,pf(slot,0x10));store<f32>(slot+0x10,px);f32 py=fmadds(pf(sp,0xA8),push,pf(slot,0x14));f32 keptX=pf(slot,0x10),pz=pf(slot,0x18);if(py<zero)py=zero;pz=fmadds(windZ,push,pz);store<f32>(slot+0x14,py);store<f32>(slot+0x18,pz);
   if(keptX>limit||pf(slot,0x14)>limit||pz>limit)store<u16>(slot+0x2E,180);
   for(u32 o:{0x10u,0x14u,0x18u})call<void>(0x0200ECD4,slot+o,zero,fifth,decay,tiny);
   dy=fadds_ppc(dy,pf(slot,0x14));dz=fadds_ppc(pf(sp,0x40),pf(slot,0x18));dx=fadds_ppc(dx,pf(slot,0x10));f32 z=fadds_ppc(pf(slot,12),dz),x=fadds_ppc(pf(slot,4),dx),y=fadds_ppc(pf(slot,8),dy);store<f32>(slot+4,x);store<f32>(slot+8,y);store<f32>(slot+12,z);store<f32>(sp+0x40,dz);
   f32 cosine=pc(tilt),sine=ps(tilt),horizontal=fmuls_ppc(fmuls_ppc(cosine,ps(heading)),drift),vertical=fmuls_ppc(sine,drift);random=pf(slot,0x20);y=fmadds(vertical,five,y);x=fmadds(horizontal,five,x);f32 hz=fmuls_ppc(fmuls_ppc(cosine,pc(heading)),drift);u32 angle=(u16)ftoi(fmuls_ppc(random,pf(0x1004F59C)));z=fmadds(hz,five,z);store<f32>(slot+4,x);store<f32>(slot+8,y);store<f32>(slot+12,z);
   f32 amplitude=fmuls_ppc(random,pf(sp,0x2C));x=fmadds(ps(angle),amplitude,x);z=fmadds(pc(angle),amplitude,z);store<f32>(slot+4,x);store<f32>(slot+12,z);
   if(!load<s16>(slot+0x2E)){bool wrap=false;
    if(pattern==2){store<f32>(sp+0x80,fadds_ppc(pf(base),x));store<f32>(sp+0x84,pf(base,4));store<f32>(sp+0x88,fadds_ppc(pf(base,8),pf(slot,12)));f32 distance=poisonDistance(sp+0x80,base);if(!(distance<rangeX)){store<f32>(slot+4,-fmuls_ppc(pf(slot,4),pf(0x1004F948)));store<f32>(slot+12,-fmuls_ppc(pf(slot,12),pf(0x1004F948)));wrap=true;}}
    else if(pattern==4){f32 worldX=fadds_ppc(pf(base),x),worldZ=fadds_ppc(pf(base,8),z);
     if(x>rangeX){store<f32>(slot+4,-rangeX);wrap=true;}else if(x < -rangeX){store<f32>(slot+4,rangeX);wrap=true;}
     if(worldX>pf(0x1004F94C)){if(worldZ<pf(sp,0x30)){store<f32>(slot+12,fsubs_ppc(pf(sp,0x30),pf(base,8)));wrap=true;}else if(worldZ>pf(sp,0x28)){store<f32>(slot+12,fsubs_ppc(pf(sp,0x28),pf(base,8)));wrap=true;}}
     else if(worldX>pf(0x1004F950)){if(worldZ<pf(sp,0x20)){store<f32>(slot+12,fsubs_ppc(pf(sp,0x20),pf(base,8)));wrap=true;}else if(worldZ>pf(sp,0x28)){store<f32>(slot+12,fsubs_ppc(pf(sp,0x28),pf(base,8)));wrap=true;}}
     else if(worldX<pf(0x1004F954)&&worldX>pf(0x1004F958)){if(worldZ<pf(sp,12)){store<f32>(slot+12,fsubs_ppc(pf(sp,12),pf(base,8)));wrap=true;}else if(worldZ>pf(sp,0x14)){store<f32>(slot+12,fsubs_ppc(pf(sp,0x14),pf(base,8)));wrap=true;}}
     else if(pf(slot,12)>rangeZ){store<f32>(slot+12,rangeZ);wrap=true;}else if(pf(slot,12)<-rangeZ){store<f32>(slot+12,-rangeZ);wrap=true;}
     if(wrap)store<f32>(slot+0x20,call<f32>(0x020198D8,one));
    }
    else if(pattern==3){f32 worldX=fadds_ppc(pf(base),x),worldZ=fadds_ppc(pf(base,8),z);
     if(z>rangeZ){store<f32>(slot+12,-rangeZ);wrap=true;}else if(z<-rangeZ){store<f32>(slot+12,rangeZ);wrap=true;}
     if(worldZ>pf(0x1004F95C)){if(worldX<pf(sp,0x34)){store<f32>(slot+4,fsubs_ppc(pf(sp,0x34),pf(base)));wrap=true;}else if(worldX>pf(sp,0x3C)){store<f32>(slot+4,fsubs_ppc(pf(sp,0x3C),pf(base)));wrap=true;}}
     else if(pf(slot,4)>rangeX){store<f32>(slot+4,rangeX);wrap=true;}else if(pf(slot,4)<-rangeX){store<f32>(slot+4,-rangeX);wrap=true;}
     if(wrap)store<f32>(slot+0x20,call<f32>(0x020198D8,one));
    }
    else{if(x>rangeX){store<f32>(slot+4,-rangeX);wrap=true;z=pf(slot,12);}else if(x<-rangeX){store<f32>(slot+4,rangeX);wrap=true;z=pf(slot,12);}if(z>rangeZ){store<f32>(slot+12,-rangeZ);wrap=true;}else if(z<-rangeZ){store<f32>(slot+12,rangeZ);wrap=true;}}
    y=pf(slot,8);if(y>maxY&&!(pf(slot,0x24)>tiny))store<f32>(slot+8,zero);else if(y<zero&&!(pf(slot,0x24)>tiny))store<f32>(slot+8,maxY);if(wrap){store<u16>(slot+0x2E,120);store<f32>(slot+0x24,zero);}
   }
   rangeY=maxY;
  }
  poisonTotal(sp+0x54,base,slot);call<void>(0x02563F64,camera+0xDC,camera+0xE8,sp+0x8C);
  if(pattern==0){for(u32 o:{0u,4u,8u})store<f32>(sp+0x68+o,fmadds(pf(sp+0x8C,o),far,pf(camera,0xDC+o)));}else poisonCopy(sp+0x68,camera+0xDC);
  f32 distance=poisonDistance(sp+0x54,sp+0x68);if(distance<pf(sp,0x1C)){poisonCopy(base+12,sp+0x54);store<f32>(sp+0x1C,distance);}f32 ratio=distance/fadds_ppc(rangeX,rangeZ);if(ratio>one)ratio=one;
  u32 angle=call<u32>(0x02019510,pf(slot,0x1C));f32 phase=pf(slot,0x1C),y=pf(slot,8),upper=fmuls_ppc(rangeY,half);f32 newSize=fmadds(pf(sp,0x44),ps(angle),fmadds(size,ratio,size));store<f32>(slot+0x1C,fadds_ppc(phase,pf(sp,0x10)));store<f32>(slot+0x28,newSize);
  f32 verticalAlpha;if(y>upper)verticalAlpha=fsubs_ppc(rangeY,y)/upper;else{f32 lower=fmuls_ppc(rangeY,pf(0x1004F588));verticalAlpha=y<lower?y/lower:one;}
  f32 radial=one;
  if(pattern==2){poisonCopy(sp+0x98,sp+0x54);store<f32>(sp+0x9C,pf(base,4));ratio=poisonDistance(sp+0x98,base)/rangeX;if(ratio>one)ratio=one;f32 square=fmuls_ppc(ratio,ratio),cube=fmuls_ppc(square,ratio),fourth=fmuls_ppc(cube,ratio);radial=fnmsubs(ratio,fourth,one);newSize=pf(slot,0x28);}
  else{f32 edgeX=fsubs_ppc(rangeX,pf(sp,0x4C)),x=std::fabs(pf(slot,4));if(x>edgeX){f32 span=fsubs_ppc(rangeX,edgeX);radial=span>zero?fsubs_ppc(rangeX,x)/span:zero;}f32 edgeZ=fsubs_ppc(rangeZ,pf(sp,0x4C)),z=std::fabs(pf(slot,12));if(z>edgeZ){f32 span=fsubs_ppc(rangeZ,edgeZ);radial=span>zero?fmuls_ppc(radial,fsubs_ppc(rangeZ,z)/span):zero;}}
  store<f32>(slot+0x28,fmuls_ppc(newSize,radial));f32 deficit=fsubs_ppc(one,verticalAlpha);verticalAlpha=fnmsubs(deficit,deficit,one);s32 timer=load<s16>(slot+0x2E);if(timer){store<u16>(slot+0x2E,timer-1);verticalAlpha=zero;}else if(load<u8>(slot)==2)store<u8>(slot,0);
  if(call<u32>(0x02560704))verticalAlpha=zero;call<void>(0x0200ECD4,slot+0x24,fmuls_ppc(fmuls_ppc(fifth,verticalAlpha),radial),pf(sp,0x10),pf(0x1004F580),tiny);
  f32 near=pf(0x1004F52C);if(pattern==0)poisonCopy(sp+0x68,camera+0xDC);else if(pattern==3)near=pf(sp,0x18);distance=poisonDistance(sp+0x54,sp+0x68);if(distance<near){ratio=distance/pf(sp,0x18);if(ratio>one)ratio=one;store<f32>(slot+0x24,fmuls_ppc(pf(slot,0x24),fmuls_ppc(fmuls_ppc(ratio,ratio),ratio)));}
  u32 current=pw(call<u32>(0x025200D4),0x5B2C),hero=pw(call<u32>(0x025200D4),0x5B34);if(current==hero&&pf(slot,0x24)>pf(0x1004F5E8)&&!load<s16>(slot+0x2E)){store<f32>(sp+0x58,fsubs_ppc(pf(sp,0x58),pf(0x1004F564)));distance=poisonDistance(sp+0x54,player+0x314);if(distance<fmuls_ppc(pf(slot,0x28),pf(sp,0x24)))store<u32>(link+0x3C0,pw(link,0x3C0)|0x100000);}
  count=pw(call<u32>(0x02555D0C),0xA68);
 }
 }
 packet=pw(call<u32>(0x02555D0C),0xA6C);store<u32>(packet+0xBC30,pw(packet,0xBC30)+1);
}
VERIFY(0x0256CA54,rainPoisonMove);
