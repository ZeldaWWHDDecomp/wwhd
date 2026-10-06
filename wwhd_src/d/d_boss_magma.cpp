/* Retained WWHD boss magma TU; */
#include "gabi.h"
namespace d_boss_magma_cpp {
using gabi::load;using gabi::store;using gabi::call;
static f32 cf(u32 p){return load<f32>(p);}
static f32 add(f32 a,f32 b){return gabi::fadds_ppc(a,b);}
static f32 mul(f32 a,f32 b){return gabi::fmuls_ppc(a,b);}
u32 findBoss(u32 actor,u32 unused){
 WWHD_FUNC(0x024F6098,u32,actor,unused);
 return call<u32>(0x025D4604,actor)&&actor&&load<s16>(actor+8)==234?actor:0;
}
VERIFY(0x024F6098,findBoss);
void calc(u32 self,f32 input,u8 unused,s32 unused2){
 WWHD_FUNC(0x024F60E8,void,self,input,unused,unused2);
 u32 boss=load<u32>(0x101D53E4);f32 amplitude=cf(0x10044654);
 if(boss){if(!(cf(boss+0x70C8)>cf(0x10044658))){store<f32>(self+0xD54,cf(0x1004465C));return;}
  f32 z=cf(self+0xD58),x=cf(self+0xD50);f32 radius=call<f32>(0x028F4384,gabi::fmadds(x,x,mul(z,z)));
  f32 diff=std::fabs(gabi::fsubs_ppc(cf(boss+0x70C0),radius));
  if(diff<cf(0x10044660)){
   f32 adjustment=cf(0x1047B624),base=cf(self+0xD60);f32 target=add(add(base,add(adjustment,amplitude)),cf(0x10044664));
   f32 rate=cf(0x10044668),max=cf(0x10044670),factor=cf(0x1004466C);
   call(0x0200ED84,self+0xD54,gabi::fnmsubs(diff,rate,target),factor,max);
   f32 step=cf(0x10044674),scale=cf(0x10044678);call(0x0200ED84,self+0xD5C,scale,step,step);
   store<u16>(self+0xD64,0x4000);store<u16>(self+0xDD0,1000);return;
  }
 }
 s16 wave=load<s16>(self+0xD64);u16 phase=(u16)wave;
 if(wave<0){u32 vtable=load<u32>(self+0xDCC),fn=load<u32>(vtable+0x24);gabi::call_ptr(fn,self,0u,(s32)-1,input);store<u16>(self+0xD64,0);phase=0;}
 f32 adjustment=cf(0x1047B624);s16 nextwave=load<s16>(self+0xD64);f32 base=cf(self+0xD60);s16 speed=load<s16>(self+0xDD0);
 f32 sine=cf(0x104A44F8+((phase>>3)<<3));f32 y=gabi::fmadds(sine,add(adjustment,amplitude),base);
 store<u16>(self+0xD64,(u16)(nextwave+speed));store<f32>(self+0xD54,y);
}
VERIFY(0x024F60E8,calc);
void update(u32 self){
 WWHD_FUNC(0x024F62F8,void,self);
 f32 y=cf(self+0xD54),z=cf(self+0xD58),x=cf(self+0xD50);call(0x028E93CC,0x1048D0CCu,x,y,z);
 f32 scale=cf(self+0xD5C),one=cf(0x1004467C);call(0x025F2518,scale,one,scale);
 call(0x028E90D4,0x1048D0CCu,self+0xD98);call(0x028E9108,0x104B45F8u,self+0xD98,self+0xD68);call(0x02589D30,self);
}
VERIFY(0x024F62F8,update);
void setup(u32 self,f32 input,u8 unused,s32 unused2){
 WWHD_FUNC(0x024F637C,void,self,input,unused,unused2);
 f32 x=call<f32>(0x02019918,cf(0x10044680));f32 radius2=cf(0x10044688),zero=cf(0x10044684);store<f32>(self+0xD50,x);store<f32>(self+0xD54,zero);
 f32 maxz=call<f32>(0x028F4384,gabi::fnmsubs(x,x,radius2));f32 z=call<f32>(0x02019918,maxz);f32 half=cf(0x1004466C);store<f32>(self+0xD58,z);
 f32 variation=call<f32>(0x020198D8,add(cf(0x1047B640),half));f32 scale=add(add(cf(0x1047B63C),half),variation);f32 baseoffset=cf(0x1004468C);store<f32>(self+0xD5C,scale);
 f32 base=add(add(add(cf(0x1047B628),baseoffset),input),cf(0x10044690));store<f32>(self+0xD60,base);
 u32 boss=call<u32>(0x025DE508,0x024F6098u,0u);store<u32>(0x101D53E4,boss);
 if(boss){
  if(load<u8>(boss+0x70CC)==1){f32 vz=cf(self+0xD58),vx=cf(self+0xD50);f32 dist=call<f32>(0x028F4384,gabi::fmadds(vx,vx,mul(vz,vz)));
   f32 target=add(cf(0x1047B650),cf(0x10044694));f32 height=cf(self+0xD60);
   if(dist<target){f32 factor=add(cf(0x1047B654),cf(0x10044698));height=gabi::fmadds(gabi::fsubs_ppc(target,dist),factor,height);store<f32>(self+0xD60,height);}
   boss=load<u32>(0x101D53E4);store<f32>(self+0xD60,add(height,cf(boss+0x70C8)));
  } else {f32 delta=cf(boss+0x70C8),height=cf(self+0xD60);// Reference retains the boss NaN payload when both operands are NaNs.
   store<f32>(self+0xD60,add(delta,height));}
 }
 f32 speed=call<f32>(0x020198D8,cf(0x1004469C));speed=add(speed,cf(0x100446A0));f32 wavebound=cf(0x100446A8);store<u16>(self+0xDD0,(u16)gabi::ftoi(speed));
 f32 wave=call<f32>(0x020198D8,wavebound);store<u16>(self+0xD64,(u16)gabi::ftoi(mul(cf(0x100446A4),wave)));
}
VERIFY(0x024F637C,setup);
void init(){
 WWHD_FUNC(0x024F6570,void);
 store<u32>(0x1046EE54,0);store<u32>(0x1046EE4C,0);store<u32>(0x1046EE58,0);store<u32>(0x1046EE50,0);call(0x028F026C,0x101D53E8u);
 f32 a=cf(0x100446B0),b=cf(0x100446B4);store<f32>(0x1046EE40,a);store<f32>(0x1046EE44,b);call(0x028ED6F8,0x1046EE48u);call(0x028F026C,0x101D53F4u);call(0x028EAB2C,0x1046EE49u);call(0x028F026C,0x101D5400u);
}
VERIFY(0x024F6570,init);
void memberDestroy(u32 self,u32 flags){
 WWHD_FUNC(0x024F6604,void,self,flags);
 if(self){call(0x027BF880,self+0x158,2u);call(0x027B5CBC,self+4,2u);if(flags&1)call(0x0273AF40,self);}
}
VERIFY(0x024F6604,memberDestroy);
// HD mesh ownership shared by both retained deleting-destructor copies.
static void releaseField(u32 address){u32 heap=call<u32>(0x02755FEC,load<u32>(0x101F8B4C),load<u32>(address));u32 vtable=load<u32>(heap+0xC),fn=load<u32>(vtable+0x3C);gabi::call_ptr(fn,heap,load<u32>(address));}
static void clearMeshPair(u32 p){
 call(0x027BF7E8,p+0x158);u32 data=load<u32>(p+0x250);store<u32>(p,0);
 if(data){releaseField(p+0x250);store<u32>(p+0x24C,0);store<u32>(p+0x250,0);}
 call(0x027BF7E8,p+0x3AC);data=load<u32>(p+0x4A4);store<u32>(p+0x254,0);
 if(data){releaseField(p+0x4A4);store<u32>(p+0x4A0,0);store<u32>(p+0x4A4,0);}store<u32>(p+0x4B8,0);
}
static void destroyMesh(u32 self,u32 flags){
 if(!self)return;
 store<u32>(self+0xDCC,0x1004462C);clearMeshPair(self+0xC);
 for(s32 i=0,offset=0;i<load<s32>(self+0x4E4);++i,offset+=0x23C){u32 data=load<u32>(self+0x4E8);if((u32)i<load<u32>(self+0x4E4))data+=offset;call(0x027BEBEC,data+0x10);call(0x027BEBEC,data+0x2C);}
 for(u32 i=0;i<2;++i)call(0x027BEBEC,self+0x500+i*0x1C);
 for(u32 i=0;i<2;++i)call(0x027BEBEC,self+0x5A8+i*0x1C);
 call(0x027BE2B0,self+0xBB8,2u);call(0x027BE2B0,self+0xA20,2u);call(0x027FB528,self+0x598,0u);call(0x027FB528,self+0x4F0,0u);call(0x027FD764,self+0x4E4,2u);call(0x027B54A0,self+0x4CC,2u);
 u32 p=self+0xC;if(p){clearMeshPair(p);call(0x028F0164,p,2u,0x254u,0x024F6604u,0u,0u);}
 u32 data=load<u32>(self+8);if(data){
  for(s32 i=0,off=0;i<load<s32>(self+4);++i,off+=0x14){u32 entry=data+off;if(entry){store<u32>(entry,0);
   for(u32 arrayOff: {8u,0x10u}){u32 arr=load<u32>(entry+arrayOff);if(arr){
    for(s32 j=0,offset=0;j<load<s32>(entry+arrayOff-4);++j,offset+=0xF4){u32 object=arr+offset,vt=load<u32>(object+0xF0),fn=load<u32>(vt+0xC);gabi::call_ptr(fn,object,2u);arr=load<u32>(entry+arrayOff);}
    releaseField(entry+arrayOff);store<u32>(entry+arrayOff-4,0);store<u32>(entry+arrayOff,0);
   }}data=load<u32>(self+8);
  }}releaseField(self+8);store<u32>(self+4,0);store<u32>(self+8,0);
 }
 if(flags&1)call(0x0273AF40,self);
}
void baseDestroy(u32 self,u32 flags){WWHD_FUNC(0x024F6664,void,self,flags);destroyMesh(self,flags);}
VERIFY(0x024F6664,baseDestroy);
void bossDestroy(u32 self,u32 flags){WWHD_FUNC(0x024F6A28,void,self,flags);destroyMesh(self,flags);}
VERIFY(0x024F6A28,bossDestroy);
}
