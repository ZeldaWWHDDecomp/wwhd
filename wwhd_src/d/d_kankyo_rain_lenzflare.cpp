/* Native HD lens flare geometry. */
#include "bindings.h"
namespace rain_lenz {
static u32 ld(u32 a){return gabi::load<u32>(a);} static f32 lf(u32 a){return gabi::load<f32>(a);}
static void sw(u32 a,u32 v){gabi::store<u32>(a,v);}static void sf(u32 a,f32 v){gabi::store<f32>(a,v);}
static void sb(u32 a,u8 v){gabi::store<u8>(a,v);}static void sh(u32 a,u16 v){gabi::store<u16>(a,v);}
static u8 lb(u32 a){return gabi::load<u8>(a);}static s16 hs(u32 a){return gabi::load<s16>(a);}
static void clear_cache(u32 p,u32 size){u32 end=p+size;for(u32 q=p;q<end;q+=32)for(u32 j=0;j<32;j+=4)sw((q&~31u)+j,0);}
static f32 sine(s32 angle){return lf(0x104A44F8+(u16(angle)>>3)*8);}
static f32 cosine(s32 angle){return lf(0x104A44F8+(u16(angle)>>3)*8+4);}
static void draw(u32 unused,u32 positions){
 WWHD_FUNC(0x02570210,void,unused,positions);
 // Live original SP08..SP19B, including Vec12, matrix48, converter16 and projection40.
 gabi::Local<u8[404]> frame;u32 b=gabi::ea(frame.get());auto sp=[&](u32 o){return b+o-8;};
 u32 packet=ld(gabi::call<u32>(0x02555D0C)+0xA38);
 u32 sun=ld(gabi::call<u32>(0x02555D0C)+0xA34);
 u32 game=gabi::call<u32>(0x025200D4);f32 falloff=lf(packet+0x124),visibility=lf(sun+0xC8);u32 camera=ld(game+0x5AF8);
 f32 tenth=lf(0x1004F588),one=lf(0x1004F550),vis2=visibility*visibility;
 sf(sp(0x1C),visibility);sf(sp(0x50),vis2);sf(sp(0x10),one);sf(sp(0x14),tenth);sf(sp(0x18),vis2);sf(sp(0x54),one-falloff);
 if(visibility<tenth)return;
 f32 zero=lf(0x1004F528);sf(sp(0x20),zero);
 f32 eased=zero;if(falloff>lf(sp(0x14)))eased=(f32)((f64)(falloff-lf(sp(0x14)))/gabi::load<f64>(0x1004FBC0));
 f32 inverse=lf(sp(0x10))-eased;sf(sp(0x44),inverse);
 f32 cube=(inverse*inverse)*inverse;eased=eased*(eased*eased);
 sf(sp(0x44),gabi::fnmsubs(lf(sp(0x44)),cube,lf(sp(0x10))));
 f32 distance=lf(0x1004F614);gabi::call(0x0256401C,camera,sp(0xBC),distance,distance);
 sb(sp(0xB8),255);sh(sp(0xC8),255);sb(sp(0x7B),40);sb(sp(0x7A),235);sb(sp(0xBB),30);
 sb(sp(0xBA),0);sh(sp(0xCA),255);sb(sp(0xB9),40);sb(sp(0x79),255);sb(sp(0x78),255);
 if(ld(gabi::call<u32>(0x025200D4)+0x5FA4)==0)return;
 u32 view=ld(gabi::call<u32>(0x025200D4)+0x5FA4);
 gabi::call(0x028E91EC,view+0x1E4,sp(0xFC));
 gabi::call(0x02563DD8,sp(0x174));gabi::call(0x025F1018,sun+0x98,sp(0x138),sp(0x174));
 sf(sp(0xF4),lf(0x1004F5F4));sf(sp(0xF0),lf(0x1004F5F0));sf(sp(0xF8),lf(sp(0x20)));
 gabi::call(0x02563F64,sp(0xF0),sp(0x138),sp(0x12C));
 s32 angle=gabi::call<s32>(0x020195B0,lf(sp(0x12C)),lf(sp(0x130)));
 f32 turns=gabi::fmadds((f32)angle,lf(0x1004F5F8),lf(0x1004F5DC))/lf(0x1004F6D8);
 s32 angleA=(s16)gabi::ftoi(gabi::fmadds(turns,lf(0x1004FBC8),lf(0x1004FBCC)));
 s32 angleB=(s16)gabi::ftoi(gabi::fmadds(turns,lf(0x1004FBD0),lf(0x1004FBD4)));
 s32 phaseA=-2038,phaseB=16747;
 if(ld(gabi::call<u32>(0x025200D4)+0x5FA4)!=0){view=ld(gabi::call<u32>(0x025200D4)+0x5FA4);gabi::call(0x028E91EC,view+0x1E4,sp(0xFC));}
 gabi::call(0x028E98C0,sp(0x144),90,lf(sp(0x20)));
 gabi::call(0x028E9108,sp(0xFC),sp(0x144),sp(0xFC));
 f32 v2=lf(sp(0x50));f32 half=lf(0x1004F57C);sf(sp(8),half);
 f32 intensity=(v2*v2)*lf(sp(0x54));sb(sp(0x79),255);sb(sp(0x78),255);sb(sp(0x7A),235);
 f32 a;if(intensity<lf(sp(8)))a=lf(0x1004F558)*lf(sp(0x20));
 else{f32 t=intensity-lf(sp(8));a=lf(0x1004F558)*(t+t);}
 sb(sp(0x7B),u8(gabi::ftoi(a)));gabi::call(0x0257015C,packet+0x9DD4,sp(0x78));
 if(lb(sp(0x7B))!=0 && lf(sp(0x54))>lf(sp(0x20))){
  f32 fall=lf(sp(0x54)),v=lf(sp(0x1C));
  f32 scale=gabi::fmadds((fall*fall)*v,lf(0x1004F740),lf(0x1004F740));
  f32 amp=(lf(0x1004F744)*v)*(lf(sp(0x10))-eased);
  for(s32 i=0;i<16;i++){
   f32 base=(f32)((i&1)?phaseA:phaseB);
   f32 oscillation=sine(gabi::ftoi((f32)((i&1)?angleA:angleB)*lf(0x1004FBE4)));
   if(oscillation<lf(sp(0x20)))oscillation=-oscillation;
   f32 h=gabi::fmadds(lf(sp(8)),oscillation,lf(sp(8)));
   f32 width=lf(0x1004FBE0)*(h+lf(sp(8)));
   s32 left=gabi::ftoi(base+width),right=gabi::ftoi(base-width);
   f32 ring=lf(0x1004F52C)*scale;
   f32 lx=sine(left)*ring,ly=cosine(left)*ring,rx=sine(right)*ring,ry=cosine(right)*ring;
   f32 radial=((lf(0x1004F5D0)*scale)*(lf(sp(0x54))+lf(0x1004F800)))*gabi::fmadds(lf(sp(8)),h,lf(sp(8)));
   radial=radial*amp;if(i&3)radial=radial*lf(0x1004FBDC);
   f32 tx=sine(gabi::ftoi(base))*radial,ty=cosine(gabi::ftoi(base))*radial;
   phaseA=(s16)(phaseA+4096);phaseB=(s16)(phaseB+7281);angleA=(s16)(angleA+4096);angleB=(s16)(angleB+7281);
   sf(sp(0x60),tx);sf(sp(0x64),ty);sf(sp(0x68),lf(sp(0x20)));
   gabi::call(0x028E8F64,sp(0xFC),sp(0x60),sp(0x6C));
   f32 py=lf(positions+4)+lf(sp(0x70)),px=lf(positions)+lf(sp(0x6C)),pz=lf(positions+8)+lf(sp(0x74));
   sf(sp(0x80),py);sf(sp(0x7C),px);sf(sp(0x84),pz);
   f64 d=gabi::call<f64>(0x028E8DE8,sp(0xBC),sp(0x7C));d=gabi::call<f64>(0x028F4384,d);
   if(d<lf(packet+0x118))sf(packet+0x118,(f32)d);else if(d>lf(packet+0x11C))sf(packet+0x11C,(f32)d);
   f32 bump;switch(i&3){case 0:bump=lf(sp(0x14));break;case 1:bump=lf(0x1004FBD8);break;case 2:bump=lf(0x1004F554);break;default:bump=lf(0x1004F598);}
   f32 gain=(lf(sp(0x18))+bump)*lf(sp(0x1C));tx=tx*gain;ty=ty*gain;
   auto transform=[&](f32 x,f32 y){sf(sp(0x60),x);sf(sp(0x64),y);sf(sp(0x68),lf(sp(0x20)));gabi::call(0x028E8F64,sp(0xFC),sp(0x60),sp(0x6C));};
   transform(lx,ly);f32 ax=lf(sun+0x98)+lf(sp(0x6C)),ay=lf(sun+0x9C)+lf(sp(0x70)),az=lf(sun+0xA0)+lf(sp(0x74));
   sf(sp(0x88),ax);sf(sp(0x8C),ay);sf(sp(0x90),az);
   transform(tx,ty);f32 bx=lf(sun+0x98)+lf(sp(0x6C)),by=lf(sun+0x9C)+lf(sp(0x70)),bz=lf(sun+0xA0)+lf(sp(0x74));
   transform(rx,ry);f32 cx=lf(sun+0x98)+lf(sp(0x6C)),cy=lf(sun+0x9C)+lf(sp(0x70)),cz=lf(sun+0xA0)+lf(sp(0x74));
   u32 table=packet+0x51D4,index=u32(i)*2+ld(table+0x4A80);u32 buf=ld(table+index*0x254);
   clear_cache(buf,0x1C0);
   index=u32(i)*2+ld(table+0x4A80);buf=ld(table+index*0x254);
   sf(buf+0x130,cx);sf(buf+0x9C,by);sf(buf+0xA0,bz);sf(buf+0x134,cy);sf(buf+0x138,cz);
   sf(buf+4,lf(sp(0x8C)));sf(buf+8,lf(sp(0x90)));sf(buf,lf(sp(0x88)));sf(buf+0x98,bx);
  }
 }
 gabi::call(0x028E98C0,sp(0x144),90,lf(packet+0x120)*lf(0x1004FB7C));
 gabi::call(0x028E9108,sp(0xFC),sp(0x144),sp(0xFC));
 f32 v=lf(sp(0x1C)),fade=lf(sp(0x44)),half2=lf(sp(8));
 f32 baseSize=gabi::fmadds(v,lf(0x1004FBE8),lf(0x1004F6DC));
 f32 unit=lf(sp(0x10)),colorTenth=lf(sp(0x14));
 f32 angleGreen=(f32)hs(sp(0xC8)),angleRed=(f32)hs(sp(0xCA));
 sf(sp(0x2C),lf(0x1004F554));sf(sp(0x10),unit-((fade-half2)+(fade-half2)));
 sf(sp(8),lf(0x1004F7FC));sf(sp(0x14),lf(0x1004F844));
 f32 invFade=unit-fade,fall=lf(sp(0x54)),fallSq=fall*fall;
 sf(sp(0x28),angleRed);sf(sp(0x24),angleGreen);sf(sp(0x20),lf(0x1004FBEC));
 bool upper=!(fade<half2);f32 zero2=lf(0x1004F528),extra=lf(0x1004F544);
 for(s32 i=0;i<9;i++){
  u32 material=packet+0x2BF0+u32(i)*0x41C,record=packet+0x130+u32(i)*0x4C0;
  sf(sp(0x18),lf(sp(0x50)));
  f32 alpha;
  if(i<3){if(i==1)alpha=(lf(0x1004F5AC)*lf(sp(0x18)))*invFade;
   else alpha=((lf(0x1004FBF0+u32(i)*4)*lf(sp(0x18)))*lf(sp(0x14)))*invFade;
  }else{
   sb(sp(0x78),u8(gabi::ftoi(lf(sp(0x24))*lf(0x1004FC14+u32(i)*4))));
   sb(sp(0x79),u8(gabi::ftoi(lf(sp(0x28))*lf(0x1004FC38+u32(i)*4))));
   f32 x=(lf(0x1004FBF0+u32(i)*4)*lf(sp(0x18)))*lf(sp(0x2C));
   alpha=x*(upper?lf(sp(0x10)):(lf(sp(0x44))+lf(sp(0x44))));
  }
  sb(sp(0x7B),u8(gabi::ftoi(alpha)));gabi::call(0x0257015C,sp(0xD0),sp(0x78));
  for(u32 k=0;k<16;k+=4)sw(material+0x168+k,ld(sp(0xD0)+k));
  gabi::call(0x0257015C,sp(0xE0),sp(0xB8));for(u32 k=0;k<16;k+=4)sw(material+0x178+k,ld(sp(0xE0)+k));
  f32 size,scaleDat=lf(0x101E9A00+u32(i)*4);
  if(i>=3)size=((scaleDat*lf(sp(0x1C)))*lf(sp(0x20)))*(unit-lf(packet+0x124));
  else{size=scaleDat*baseSize;f32 term=scaleDat*lf(sp(0x1C));if(i==0)term=term*half2;else if(i==1)term=term*colorTenth;else term=term*extra;size=i==0?gabi::fmadds(scaleDat,baseSize,term*fallSq):gabi::fmadds(term,fallSq,size);}
  f32 negative=-size;u32 pos=positions+u32(i)*12;
  auto transform=[&](f32 x,f32 y){sf(sp(0x60),x);sf(sp(0x64),y);sf(sp(0x68),zero2);gabi::call(0x028E8F64,sp(0xFC),sp(0x60),sp(0x6C));};
  transform(negative,size);f32 ax=lf(pos)+lf(sp(0x6C)),ay=lf(pos+4)+lf(sp(0x70)),az=lf(pos+8)+lf(sp(0x74));
  sf(sp(0x88),ax);sf(sp(0x8C),ay);sf(sp(0x90),az);
  transform(size,size);f32 bx=lf(pos)+lf(sp(0x6C)),by=lf(pos+4)+lf(sp(0x70)),bz=lf(pos+8)+lf(sp(0x74));
  sf(sp(0x94),bx);sf(sp(0x98),by);sf(sp(0x9C),bz);
  transform(size,negative);f32 cx=lf(pos)+lf(sp(0x6C)),cy=lf(pos+4)+lf(sp(0x70)),cz=lf(pos+8)+lf(sp(0x74));
  sf(sp(0xA0),cx);sf(sp(0xA4),cy);sf(sp(0xA8),cz);sf(sp(0xC),cz);
  transform(negative,negative);f32 dx=lf(pos)+lf(sp(0x6C)),dy=lf(pos+4)+lf(sp(0x70)),dz=lf(pos+8)+lf(sp(0x74));
  sf(sp(0xAC),dx);sf(sp(0xB0),dy);sf(sp(0xB4),dz);
  u32 index=ld(record+0x4A8),buf=ld(record+index*0x254);clear_cache(buf,0x260);
  index=ld(record+0x4A8);buf=ld(record+index*0x254);f32 uv=i<=1?unit:lf(sp(8));
  sf(buf+0x1CC,lf(sp(0xB0)));sf(buf+0x1D0,lf(sp(0xB4)));sf(buf+0x1C8,lf(sp(0xAC)));sf(buf+0x200,zero2);
  sf(buf+0x98,lf(sp(0x94)));sf(buf+0xD0,uv);sf(buf+0x9C,lf(sp(0x98)));sf(buf+0xA0,lf(sp(0x9C)));
  sf(buf+0x38,zero2);sf(buf+0xD4,zero2);sf(buf+0x3C,zero2);sf(buf+4,lf(sp(0x8C)));sf(buf+8,lf(sp(0x90)));
  sf(buf+0x204,uv);sf(buf+0x16C,uv);sf(buf+0x134,lf(sp(0xA4)));sf(buf+0x138,lf(sp(0xC)));
  sf(buf,lf(sp(0x88)));sf(buf+0x130,lf(sp(0xA0)));sf(buf+0x168,uv);
  index=ld(record+0x4A8);u32 owner=record+index*0x254;
  gabi::call(0x027B5E94,owner+4,0,ld(owner+0x150));
  sw(record+0x4A8,ld(record+0x4A8)==0?1:0);
 }
 gabi::call(0x0257EF00,packet);
}
VERIFY(0x02570210,draw);
}
