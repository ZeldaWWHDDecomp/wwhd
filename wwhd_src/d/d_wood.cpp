/* Retained WWHD wood/bush engine TU. */
#include "gabi.h"
#include <bit>
namespace d_wood_cpp {
using gabi::load;using gabi::store;using gabi::call;
static f32 cf(u32 a){return load<f32>(a);}
static f32 add(f32 a,f32 b){return gabi::fadds_ppc(a,b);}
static void zeroPhases(u32 self){for(u32 i=0;i<2;++i)for(u32 off:{0x78u,0x7Cu,0x80u,0x84u})store<u16>(self+off+2*i,0);}
u32 animationCreate(u32 self){
 WWHD_FUNC(0x025CD26C,u32,self);
 if(!self)self=call<u32>(0x0273AD10,0x8Cu);
 if(self){call(0x028E9098,self);call(0x028E9098,self+0x30);f32 zero=cf(0x100566E4);
 store<u16>(self+0x64,0);store<f32>(self+0x74,zero);store<f32>(self+0x68,zero);store<u16>(self+0x66,0);store<u32>(self+0x60,6);store<f32>(self+0x70,zero);store<f32>(self+0x6C,zero);zeroPhases(self);store<u16>(self+0x88,0);store<u8>(self+0x8A,255);}
 return self;
}
VERIFY(0x025CD26C,animationCreate);
void play(u32 self,u32 packet){
 WWHD_FUNC(0x025CD32C,void,self,packet);
 s32 mode=load<s32>(self+0x60);if(mode==6)return;u32 record=0x100566E8+(u32)mode*8;
 s16 index=load<s16>(record+2),delta=load<s16>(record);self+=(s32)delta;u32 target;
 if(index<0)target=load<u32>(record+4);
 else{ s16 voff=load<s16>(record+6);u32 table=load<u32>(self+(s32)voff);target=load<u32>(table+(u32)(s32)index*8+4); }
 gabi::call_ptr(target,self,packet);
}
VERIFY(0x025CD32C,play);
void copyAngles(u32 self,u32 from){
 WWHD_FUNC(0x025CD380,void,self,from);
 if(self!=from)for(u32 i=0;i<2;++i)for(u32 off:{0x78u,0x7Cu,0x80u,0x84u})store<u16>(self+off+2*i,load<u16>(from+off+2*i));
}
VERIFY(0x025CD380,copyAngles);
void cutInit(u32 self,u32 unused,s16 direction){
 WWHD_FUNC(0x025CD400,void,self,unused,direction);
 zeroPhases(self);store<u16>(self+0x66,direction);f32 speed=cf(0x10056718);store<u16>(self+0x64,20);f32 zero=cf(0x100566E4);store<u8>(self+0x8A,255);store<u32>(self+0x60,0);store<f32>(self+0x74,speed);store<f32>(self+0x6C,zero);store<f32>(self+0x70,zero);
}
VERIFY(0x025CD400,cutInit);
void cut(u32 self,u32 unused){
 WWHD_FUNC(0x025CD474,void,self,unused);
 f32 speed=add(cf(self+0x74),cf(0x1005671C)),minimum=cf(0x10056720);if(speed<minimum)speed=minimum;
 f32 y=cf(self+0x6C),z=cf(self+0x70),zvel=cf(0x10056724);y=add(y,speed);store<f32>(self+0x74,speed);z=add(z,zvel);s16 pitch=load<s16>(self+0x7C);store<f32>(self+0x6C,y);store<f32>(self+0x70,z);s16 direction=load<s16>(self+0x66);store<u16>(self+0x7C,(u16)(pitch-200));call(0x025F1884,0x1048D0CCu,(s32)direction);
 z=cf(self+0x70);f32 zero=cf(0x100566E4);y=cf(self+0x6C);call(0x025F24E0,zero,y,z);call(0x025F1BF4,0x1048D0CCu,(s32)load<s16>(self+0x7C));call(0x025F1C28,0x1048D0CCu,(s32)(s16)-load<s16>(self+0x66));call(0x028E90D4,0x1048D0CCu,self);
 s16 countdown=load<s16>(self+0x64);if(countdown<20){s32 alpha=load<u8>(self+0x8A)-14;if(alpha<0)alpha=0;store<u8>(self+0x8A,(u8)alpha);countdown=load<s16>(self+0x64);}if(countdown>0)store<u16>(self+0x64,(u16)(countdown-1));
}
VERIFY(0x025CD474,cut);
void pushInit(u32 self,u32 from,s16 direction){
 WWHD_FUNC(0x025CD584,void,self,from,direction);
 copyAngles(self,from);self=gabi::cpu->r[3];store<u16>(self+0x66,direction);store<u16>(self+0x64,2);store<u8>(self+0x8A,255);store<u32>(self+0x60,1);
}
VERIFY(0x025CD584,pushInit);
void backInit(u32 self){WWHD_FUNC(0x025CD5C4,void,self);store<u16>(self+0x64,35);store<u8>(self+0x8A,255);store<u32>(self+0x60,2);}
VERIFY(0x025CD5C4,backInit);
static f32 cosine(u32 phase){return cf(0x104A44F8+((phase>>3)*8)+4);}
static void applySway(u32 self,f32 y,f32 x){
 call(0x025F1884,self,(s32)(s16)((u32)gabi::ftoi(y)+(s32)load<s16>(self+0x66)));
 call(0x025F1BF4,self,(s32)(s16)gabi::ftoi(x));
 call(0x025F1C28,self,(s32)(s16)-load<s16>(self+0x66));
}
void push(u32 self,u32 packet){
 WWHD_FUNC(0x025CD5E0,void,self,packet);
 s16 time=(s16)(load<s16>(self+0x64)-1);store<u16>(self+0x64,time);
 if(time<=0){backInit(self);return;}
 f32 y=cf(0x100566E4),x=y;
 for(u32 i=0;i<2;++i){u32 attr=0x1005681C+i*12,off=i*2;
 s16 delta=load<s16>(attr),ax=load<s16>(attr+6),step=load<s16>(attr+4),ay=load<s16>(attr+2);f32 bias=cf(attr+8);
 store<u16>(self+0x78+off,(u16)(load<s16>(self+0x78+off)+delta));
 call(0x0200F8D0,self+0x7C+off,0,(s32)step);
 call(0x0200F564,self+0x80+off,(s32)(s16)(ay/4),20);
 call(0x0200F378,self+0x84+off,(s32)ax,8,20,5);
 s16 newY=load<s16>(self+0x80+off),newX=load<s16>(self+0x84+off);
 u16 phaseY=load<u16>(self+0x78+off),phaseX=load<u16>(self+0x7C+off);
 y=gabi::fmadds(cosine(phaseY),(f32)newY,y);x=gabi::fmadds(add(cosine(phaseX),bias),(f32)newX,x);
 }applySway(self,y,x);
}
VERIFY(0x025CD5E0,push);
void toNormalInit(u32 self,u32 index){
 WWHD_FUNC(0x025CD800,void,self,index);
 if(index>=8){call(0x0273AA24,0x10056870u,0x799u,0x1005687Cu);index=0;}store<u16>(self+0x88,(u16)index);store<u8>(self+0x8A,255);store<u16>(self+0x64,20);store<u32>(self+0x60,5);
}
VERIFY(0x025CD800,toNormalInit);
s32 searchAnimation(u32 self,u32 mode){
 WWHD_FUNC(0x025CD868,s32,self,mode);
 if(mode>=6)call(0x0273AA24,0x100568E0u,0xCB5u,0x100568B4u);
 if(mode==4){u32 index=load<u32>(0x101F2F40),next=index+1;store<s32>(0x101F2F40,(s32)next%8);return (s32)index;}
 for(u32 i=8;i<72;++i)if(load<s32>(self+0x1C8D8+i*0x8C+0x60)==6)return i;
 for(u32 i=8;i<72;++i)if((s32)mode<load<s32>(self+0x1C8D8+i*0x8C+0x60))return i;
 return -1;
}
VERIFY(0x025CD868,searchAnimation);
void back(u32 self,u32 packet){
 WWHD_FUNC(0x025CD958,void,self,packet);
 s16 time=(s16)(load<s16>(self+0x64)-1);store<u16>(self+0x64,time);
 if(time<=0){s32 index=searchAnimation(packet,4);toNormalInit(self,(u32)index);return;}
 f32 y=cf(0x100566E4),x=y,t=gabi::fmuls_ppc((f32)time,cf(0x100568EC));
 for(u32 i=0;i<2;++i){u32 attr=0x1005681C+i*12,off=2*i;
 s16 targetY=(s16)gabi::ftoi(gabi::fmuls_ppc((f32)load<s16>(attr+2),t));
 s16 targetX=(s16)gabi::ftoi(gabi::fmuls_ppc((f32)load<s16>(attr+6),t));
 store<u16>(self+0x78+off,(u16)(load<s16>(self+0x78+off)+load<s16>(attr)));
 store<u16>(self+0x7C+off,(u16)(load<s16>(self+0x7C+off)+load<s16>(attr+4)));
 f32 bias=cf(attr+8);
 call(0x0200F564,self+0x80+off,(s32)targetY,20);call(0x0200F564,self+0x84+off,(s32)targetX,20);
 s16 newY=load<s16>(self+0x80+off),newX=load<s16>(self+0x84+off);
 u16 phaseY=load<u16>(self+0x78+off),phaseX=load<u16>(self+0x7C+off);
 y=gabi::fmadds(cosine(phaseY),(f32)newY,y);x=gabi::fmadds(add(cosine(phaseX),bias),(f32)newX,x);
 }applySway(self,y,x);
}
VERIFY(0x025CD958,back);
void fan(u32 self,u32 packet){WWHD_FUNC(0x025CDC20,void,self,packet);}
VERIFY(0x025CDC20,fan);
void normalInit(u32 self){
 WWHD_FUNC(0x025CDC24,void,self);store<u32>(self+0x60,4);
 for(u32 i=0;i<2;++i){store<u16>(self+0x78+2*i,(u16)(load<u32>(0x101F2F68)<<13));store<u16>(self+0x7C+2*i,(u16)(load<u32>(0x101F2F68)<<13));store<u16>(self+0x80+2*i,load<u16>(0x100567D6+12*i));store<u16>(self+0x84+2*i,load<u16>(0x100567DA+12*i));}
 store<u8>(self+0x8A,255);u32 next=load<u32>(0x101F2F68)+1;store<s32>(0x101F2F68,(s32)next%8);
}
VERIFY(0x025CDC24,normalInit);
void normal(u32 self,u32 packet){
 WWHD_FUNC(0x025CDCB4,void,self,packet);
 f32 strength=cf(self+0x68);u32 level=strength<cf(0x100568F0)?0:(strength<cf(0x100568F4)?1:2);
 f32 y=cf(0x100566E4),x=y;
 for(u32 i=0;i<2;++i){u32 attr=0x100567D4+level*24+i*12,off=i*2;
 s16 dy=load<s16>(attr),dx=load<s16>(attr+4),ay=load<s16>(attr+2),ax=load<s16>(attr+6);f32 bias=cf(attr+8);
 store<u16>(self+0x78+off,(u16)(load<s16>(self+0x78+off)+dy));store<u16>(self+0x7C+off,(u16)(load<s16>(self+0x7C+off)+dx));
 call(0x0200F564,self+0x80+off,(s32)ay,2);call(0x0200F564,self+0x84+off,(s32)ax,2);
 s16 newY=load<s16>(self+0x80+off),newX=load<s16>(self+0x84+off);u16 py=load<u16>(self+0x78+off),px=load<u16>(self+0x7C+off);
 y=gabi::fmadds(cosine(py),(f32)newY,y);x=gabi::fmadds(add(cosine(px),bias),(f32)newX,x);
 }applySway(self,y,x);
}
VERIFY(0x025CDCB4,normal);
void wind(u32 self,f32 strength,s16 direction){WWHD_FUNC(0x025CDEDC,void,self,strength,direction);store<f32>(self+0x68,strength);store<u16>(self+0x66,direction);}
VERIFY(0x025CDEDC,wind);
void toNormal(u32 self,u32 packet){
 WWHD_FUNC(0x025CDEE8,void,self,packet);
 s32 index=load<s16>(self+0x88);if((u32)index>=72)call(0x0273AA24,0x1005691Cu,0xE7u,0x100568F8u);
 u32 from=packet+0x1C8D8+(u32)index*0x8C;f32 strength=cf(from+0x68);s16 direction=load<s16>(from+0x66);
 u32 level=strength<cf(0x100568F0)?0:(strength<cf(0x100568F4)?1:2);
 call(0x0200F8D0,self+0x66,(s32)direction,3000);
 f32 y=cf(0x100566E4),x=y;
 for(u32 i=0;i<2;++i){u32 attr=0x100567D4+level*24+i*12,off=i*2;
 s16 sy=(s16)(load<s16>(attr)+3000),sx=(s16)(load<s16>(attr+4)+3000);f32 bias=cf(attr+8);
 call(0x0200F564,self+0x78+off,(s32)load<s16>(from+0x78+off),(s32)sy);
 call(0x0200F564,self+0x7C+off,(s32)load<s16>(from+0x7C+off),(s32)sx);
 call(0x0200F564,self+0x80+off,(s32)load<s16>(from+0x80+off),15);
 call(0x0200F564,self+0x84+off,(s32)load<s16>(from+0x84+off),15);
 s16 newY=load<s16>(self+0x80+off),newX=load<s16>(self+0x84+off);u16 py=load<u16>(self+0x78+off),px=load<u16>(self+0x7C+off);
 y=gabi::fmadds(cosine(py),(f32)newY,y);x=gabi::fmadds(add(cosine(px),bias),(f32)newX,x);
 }applySway(self,y,x);s16 time=load<s16>(self+0x64);if(time>0)store<u16>(self+0x64,(u16)(time-1));
}
VERIFY(0x025CDEE8,toNormal);
void clearUnit(u32 self){WWHD_FUNC(0x025CE1A8,void,self);call(0x0200ECCC,self,0u,0x248u);}
VERIFY(0x025CE1A8,clearUnit);
u32 unitCreate(u32 self){
 WWHD_FUNC(0x025CE1B4,u32,self);
 if(!self)self=call<u32>(0x0273AD10,0x248u);
 if(self){f32 values[14];u8 bytes[4];u16 shorts[4];
 for(u32 i=0;i<6;++i){values[i]=cf(0x1016E414+i*4);store<f32>(self+0x7C+i*4,values[i]);}
 for(u32 i=0;i<4;++i){bytes[i]=load<u8>(0x1016E42C+i);store<u8>(self+0x94+i,bytes[i]);}
 for(u32 i=0;i<4;++i){shorts[i]=load<u16>(0x1016E430+i*2);store<u16>(self+0x98+i*2,shorts[i]);}
 for(u32 i=6;i<14;++i){values[i]=cf(0x1016E414+0x24+(i-6)*4);store<f32>(self+0xA0+(i-6)*4,values[i]);}
 for(u32 base:{0x13Cu,0x1C0u}){
 for(u32 i=0;i<6;++i)store<f32>(self+base+i*4,values[i]);
 for(u32 i=0;i<4;++i)store<u8>(self+base+0x18+i,bytes[i]);
 for(u32 i=0;i<4;++i)store<u16>(self+base+0x1C+i*2,shorts[i]);
 for(u32 i=6;i<14;++i)store<f32>(self+base+0x24+(i-6)*4,values[i]);}
 clearUnit(self);}return self;
}
VERIFY(0x025CE1B4,unitCreate);
s32 setGround(u32 self){
 WWHD_FUNC(0x025CE360,s32,self);
 f32 y=add(cf(self+4),cf(0x10056928)),x=cf(self),z=cf(self+8);
 // Native cBgS_GndChk is 0x40 bytes; composed dBgS check includes 0x14-byte pass filters.
 gabi::Local<u8[0x54]> check;u32 a=check.a;call(0x02008E0C,a);
 store<u32>(a,a+0x40);store<u32>(a+4,a+0x4C);store<u32>(a+0x20,0x10056794);store<u32>(a+0x40,0x100567C4);store<u32>(a+0x4C,0x100567B4);
 store<u8>(a+0x44,1);for(u32 i=0x45;i<=0x4A;++i)store<u8>(a+i,0);store<u32>(a+0x50,1);store<f32>(a+0x24,x);store<f32>(a+0x28,y);store<f32>(a+0x2C,z);
 u32 game=call<u32>(0x025200D4);f32 height=call<f32>(0x02008974,game+0x12A0,a);
 store<u32>(a+0x20,0x10056764);store<u32>(a+0x4C,0x10056744);store<u32>(a+0x40,0x10056784);
 bool found=height>cf(0x1005692C);if(found)store<f32>(self+4,height);call(0x02008DAC,a,0);return found;
}
VERIFY(0x025CE360,setGround);
void setMatrix(u32 self,u32 animations){
 WWHD_FUNC(0x025CE4F0,void,self,animations);
 u32 animation=animations+load<u32>(self+0x14)*0x8C,stack=0x1048D0CC;call(0x028E90D4,animation,stack);
 for(u32 i=0;i<3;++i){u32 off=0xC+i*0x10;store<f32>(stack+off,add(cf(stack+off),cf(self+i*4)));}
 call(0x028E90D4,stack,self+0x18);call(0x028E90D4,animation+0x30,stack);
 for(u32 i=0;i<3;++i)store<f32>(stack+0xC+i*0x10,cf(self+i*4));
 call(0x028E90D4,stack,self+0x48);gabi::Local<u8[48]> product;call(0x028E9108,0x104B45F8u,stack,product.a);store<f32>(self+0x244,cf(product.a+0x2C));
}
VERIFY(0x025CE4F0,setMatrix);
static void checkAnimation(s32 index){if((u32)index>=72)call(0x0273AA24,0x10056958u,0xE7u,0x10056934u);}
static void pushHit(u32 self,u32 packet,u32 actorSlot,s32 chosen){
 call(0x025E19CC,0x6932u,self);s32 index=load<s32>(self+0x14),old=index;
 if(index<8){if(chosen!=-1){index=chosen;store<s32>(self+0x14,index);}if(index<8)return;}
 checkAnimation(index);u32 animations=packet+0x1C8D8;if(load<s32>(animations+(u32)index*0x8C+0x60)<=0)return;
 s32 angle=call<s32>(0x0200F93C,load<u32>(actorSlot)+0x314,self);s32 current=load<s32>(self+0x14);checkAnimation(current);u32 to=animations+(u32)current*0x8C;checkAnimation(old);pushInit(to,animations+(u32)old*0x8C,(s16)angle);
}
void collisionBeforeCut(u32 self,u32 packet){
 WWHD_FUNC(0x025CE5D0,void,self,packet);
 gabi::Local<u8[20]> hit;gabi::Local<u32> actor;call(0x0251694C,hit.a);u32 game=call<u32>(0x025200D4);u32 bits=call<u32>(0x025170D8,game+0x4EF8,self,actor.a,hit.a);
 s16 cooldown=load<s16>(self+0xC);if(cooldown>0){cooldown=(s16)(cooldown-1);store<u16>(self+0xC,cooldown);}
 if((bits&1)&&load<u32>(hit.a+4)&& (load<u32>(load<u32>(hit.a+4)+0x10)&0x3CC220u)){
 bits&=~1u;if(load<u32>(actor.a)&&cooldown==0){s32 chosen=searchAnimation(packet,1);store<u16>(self+0xC,20);pushHit(self,packet,actor.a,chosen);}}
 if((bits&2)&&load<u32>(actor.a)&&load<u32>(hit.a+8)&&load<u32>(load<u32>(hit.a+8)+0x44)){
 s32 chosen=searchAnimation(packet,1);u32 attacker=load<u32>(actor.a);
 if(attacker&&load<s16>(attacker+0xE)==168&&!(cf(hit.a+0xC)<cf(0x10056930))&&load<s16>(self+0xC)==0){store<u16>(self+0xC,20);pushHit(self,packet,actor.a,chosen);}}
 if(!(bits&1))return;s32 index=load<s32>(self+0x14),old=index;
 if(index<8){index=searchAnimation(packet,0);if(index==-1){index=load<s32>(self+0x14);if(index<8)return;}else{store<s32>(self+0x14,index);if(index<8)return;}}
 checkAnimation(index);u32 animations=packet+0x1C8D8;if(load<s32>(animations+(u32)index*0x8C+0x60)<=0)return;
 s32 angle=call<s32>(0x0200F93C,load<u32>(actor.a)+0x314,self);s32 current=load<s32>(self+0x14);checkAnimation(current);u32 to=animations+(u32)current*0x8C;checkAnimation(old);cutInit(to,animations+(u32)old*0x8C,(s16)angle);
 u32 drops=call<u32>(0x02555D0C);call(0x025626A4,drops,1,self,self+0x7C);
 game=call<u32>(0x025200D4);u32 emitter=call<u32>(0x025A847C,load<u32>(game+0x5AB0),0,0x3E0,self,0,0,255,0,-1,self+0x114,0,0);
 if(emitter){gabi::Local<u8[28]> desc;store<u32>(desc.a,0x10000160);store<u32>(desc.a+4,0x1005672C);f32 one=cf(0x100566E0);for(u32 i=0;i<4;++i)store<f32>(desc.a+8+i*4,one);store<u8>(desc.a+24,1);for(u32 i=25;i<28;++i)store<u8>(desc.a+i,0);
 game=call<u32>(0x025200D4);u32 manager=load<u32>(game+0x5AB0);for(u32 i=0;i<4;++i)store<u32>(desc.a+8+i*4,load<u32>(manager+0x38+i*4));call(0x0281E5A8,emitter,desc.a);}
 call(0x025E19CC,0x69D1u,self);
}
VERIFY(0x025CE5D0,collisionBeforeCut);
void unitProc(u32 self,u32 packet){
 WWHD_FUNC(0x025CEA84,void,self,packet);
 if(!(load<u32>(self+0x10)&1))return;s32 index=load<s32>(self+0x14);if(index<8)return;
 if((u32)index>=72)call(0x0273AA24,0x10056988u,0xE7u,0x10056964u);
 u32 animation=packet+0x1C8D8+(u32)index*0x8C;s32 mode=load<s32>(animation+0x60);
 if(mode==5){if(load<s16>(animation+0x64)<=0){store<s32>(self+0x14,load<s16>(animation+0x88));store<u32>(animation+0x60,6);}}
 else if(mode==0){if(load<s16>(animation+0x64)<=0){store<s32>(self+0x14,searchAnimation(packet,4));store<u32>(animation+0x60,6);store<u32>(self+0x10,load<u32>(self+0x10)|4);}}
 else if(mode==6)store<s32>(self+0x14,searchAnimation(packet,4));
}
VERIFY(0x025CEA84,unitProc);
u32 roomCreate(u32 self){
 WWHD_FUNC(0x025CEBB8,u32,self);
 if(!self)self=call<u32>(0x0273AD10,0x32Cu);
 if(self){store<u32>(self,0);store<u32>(self+4,0);store<u32>(self+8,0);call(0x0273B560,self,200u,self+12);store<u32>(self,0);}return self;
}
VERIFY(0x025CEBB8,roomCreate);
void roomEntry(u32 self,u32 unit){
 WWHD_FUNC(0x025CEC38,void,self,unit);s32 count=load<s32>(self),capacity=load<s32>(self+4);if(count<capacity){u32 array=load<u32>(self+8);store<u32>(array+(u32)count*4,unit);store<u32>(self,load<u32>(self)+1);}
}
VERIFY(0x025CEC38,roomEntry);
void roomClear(u32 self){
 WWHD_FUNC(0x025CEC64,void,self);u32 count=load<u32>(self),begin=load<u32>(self+8),end=begin+count*4;
 for(u32 p=begin;p!=end;p+=4){call(0x025E1B34,load<u32>(p));clearUnit(load<u32>(p));}store<u32>(self,0);
}
VERIFY(0x025CEC64,roomClear);
void roomDelete(u32 self,s32 room){WWHD_FUNC(0x025D0738,void,self,room);roomClear(self+0x1F038+(u32)room*0x32C);}
VERIFY(0x025D0738,roomDelete);
s32 searchUnit(u32 self){WWHD_FUNC(0x025D074C,s32,self);for(u32 i=0;i<200;++i)if(load<u32>(self+0x98+i*0x248+0x10)==0)return i;return 200;}
VERIFY(0x025D074C,searchUnit);
// Pure lfs/stfs transfers preserve the reference float representation; retain float-store footprints.
s32 putUnit(u32 self,u32 pos,u32 room){
 WWHD_FUNC(0x025D077C,s32,self,pos,room);
 if(room>=64)call(0x0273AA24,0x100569E4u,0xA17u,0x100569F0u);
 s32 index=searchUnit(self);if(index!=200){u32 unit=self+0x98+(u32)index*0x248;store<u32>(unit+0x10,1);for(u32 i=0;i<3;++i)store<f32>(unit+i*4,std::bit_cast<f32>(load<u32>(pos+i*4)));store<u32>(unit+0x14,(u32)searchAnimation(self,4));
 if(setGround(unit))roomEntry(self+0x1F038+room*0x32C,unit);else clearUnit(unit);}
 return index;
}
VERIFY(0x025D077C,putUnit);
static void bindUniforms(u32 material,u32 shape){
 u32 slots=load<u32>(material+0xC)?load<u32>(material+0x10):0;
 s16 vertex=load<s16>(slots+0xC),pixel=load<s16>(slots+0xE),geometry=load<s16>(slots+0x10);
 u32 descriptor=shape+0x10+load<u32>(shape+0x4C)*28,data=load<u32>(descriptor+4),size=load<u32>(descriptor+0xC);
 if(pixel!=-1)call(0xC0006900,(s32)pixel,size,data);
 if(vertex!=-1)call(0xC0006A38,(s32)vertex,size,data);
 if(geometry!=-1)call(0xC00068A8,(s32)geometry,size,data);
}
void drawMaterial(u32 self,u32 draw){
 WWHD_FUNC(0x025CECD8,void,self,draw);
 u32 mode=load<u32>(draw+0xC),material=0,header=self+0x4AA58;
 if((s32)mode<4){u32 capacity=load<u32>(header+4),array=load<u32>(header+8);material=load<u32>(array+(mode<capacity?mode*20:0));}
 u32 cache=call<u32>(0x027F29D4,0x104B45C0u),program=load<u32>(material);
 if(program!=load<u32>(cache+4)){
 u8 flags=load<u8>(program);u32 old=load<u32>(cache);
 if(flags&2){store<u8>(program,flags&~2);call(0x027BB9E0,program,0);}
 u32 state=load<u32>(load<u32>(program+0x7C)+0x28);if(old!=state)call(0x027B9F68,state);
 u32 size=load<u32>(program+0xC);if(size)call(0xC00060E0,load<u32>(program+4),size);else call(0x027BB7CC,program);
 store<u32>(cache,state);store<u32>(cache+4,program);
 }
 mode=load<u32>(draw+0xC);
 if(mode==0){u32 table=load<u32>(draw+0x14);if(table){u32 capacity=load<u32>(table),index=load<u32>(draw+4),shape=load<u32>(table+4)+(index<capacity?index*0x23C:0);bindUniforms(material,shape);}}
 else if(mode==1)bindUniforms(material,load<u32>(self+0x4AA68));
 else if(mode==2){bindUniforms(material,load<u32>(self+0x4AA68));u32 object=load<u32>(draw+0x30);if(object){u32 target=load<u32>(load<u32>(object+0xC)+0x2C);gabi::call_ptr(target,object,material);}call(0x027FFE54,draw,material);}
 u32 samplers=load<u32>(material+0x14)?load<u32>(material+0x18):0;call(0x027BE53C,self+0x4A468,samplers+4,0,0);
 gabi::Local<u8[0x11C]> state;call(0x02750250,state.a);u32 bits=load<u32>(state.a+0xEC);mode=load<u32>(draw+0xC);
 store<u32>(state.a+0xC,3);store<u32>(state.a+0xE4,4);store<f32>(state.a+0xE8,cf(0x100566E4));store<u32>(state.a+0xEC,(((bits&~15u)+7)&0xFFFFFF0F)+16);store<u8>(state.a+1,0);store<u8>(state.a+0xE0,1);store<u8>(state.a,1);
 call(0x0280037C,mode,state.a);call(0x02750370,state.a);
 u32 capacity=load<u32>(header+4);mode=load<u32>(draw+0xC);u32 array=load<u32>(header+8)+(mode<capacity?mode*20:0);u32 selected=load<u32>(self+0x2C494)==0?8:0;
 call(0x027BFE5C,load<u32>(array+selected+8));
 u32 roomHeader=self+0x4A72C,count=load<u32>(roomHeader),begin=load<u32>(roomHeader+8),end=begin+count*4;
 u32 transform=self+0x4AA70+(count-1)*0xA8,texture=self+0x52DB0+(count-1)*0x364;
 for(u32 item=begin;item!=end;item+=4){u32 target=load<u32>(load<u32>(texture+0xC)+0x2C);gabi::call_ptr(target,texture,material);texture-=0x364;
 target=load<u32>(load<u32>(transform+0xC)+0x2C);gabi::call_ptr(target,transform,material);transform-=0xA8;
 u32 n=load<u32>(self+0x4A444);if(n)call(0xC0006178,load<u32>(self+0x4A43C),n,load<u32>(self+0x4A438),load<u32>(self+0x4A440),0,1);
 }call(0x02750370,0x104B474Cu);
}
VERIFY(0x025CECD8,drawMaterial);
void drawMaterialThunk(u32 self,u32 draw){WWHD_FUNC(0x025CF204,void,self,draw);drawMaterial(load<u32>(self+0x98),draw);}
VERIFY(0x025CF204,drawMaterialThunk);

static void freeSlot(u32 slot){
 u32 heap=call<u32>(0x02755FEC,load<u32>(0x101F8B4C),load<u32>(slot));u32 target=load<u32>(load<u32>(heap+0xC)+0x3C);gabi::call_ptr(target,heap,load<u32>(slot));
}
static void releaseMaterialBuffers(u32 block){
 for(u32 i=0;i<2;++i)for(u32 j=0;j<2;++j){u32 part=block+i*0x4A8+j*0x254;call(0x027BF7E8,part+0x158);store<u32>(part,0);if(load<u32>(part+0x250)){freeSlot(part+0x250);store<u32>(part+0x24C,0);store<u32>(part+0x250,0);}}
 store<u32>(block+0x960,0);
}
static void destroyShapeArray(u32 row,u32 countOff,u32 ptrOff){
 u32 array=load<u32>(row+ptrOff);if(!array)return;
 s32 count=load<s32>(row+countOff);for(s32 i=0;i<count;++i){u32 object=array+(u32)i*0xF4,target=load<u32>(load<u32>(object+0xF0)+0xC);gabi::call_ptr(target,object,2);count=load<s32>(row+countOff);array=load<u32>(row+ptrOff);}
 freeSlot(row+ptrOff);store<u32>(row+countOff,0);store<u32>(row+ptrOff,0);
}
static void destroyShapeGroups(u32 header){
 u32 array=load<u32>(header+8);if(!array)return;s32 count=load<s32>(header+4);
 for(s32 i=0;i<count;++i){u32 row=array+(u32)i*20;if(row){store<u32>(row,0);destroyShapeArray(row,4,8);destroyShapeArray(row,12,16);count=load<s32>(header+4);array=load<u32>(header+8);}}
 freeSlot(header+8);store<u32>(header+4,0);store<u32>(header+8,0);
}
void packetDelete(u32 self,u32 flags){
 WWHD_FUNC(0x025D0190,void,self,flags);if(!self)return;store<u32>(self+0xC,0x10056AE8);u32 material=self+0x2BB44;
 releaseMaterialBuffers(material);
 call(0x028F0164,self+0x52DB0,200u,0x364u,0x025D2190u,0,0);
 call(0x028F0164,self+0x4AA70,200u,0xA8u,0x025D1EB4u,0,0);
 call(0x027FD764,self+0x4AA64,2);destroyShapeGroups(self+0x4AA58);
 call(0x027F13DC,self+0x4A690,0);call(0x027BE2B0,self+0x4A468,2);call(0x027B54A0,self+0x4A450,2);call(0x027B54A0,self+0x4A438,2);
 call(0x028F0164,self+0x3CB38,64u,0x364u,0x025D2190u,0,0);
 call(0x028F0164,self+0x2C4B8,400u,0xA8u,0x025D1EB4u,0,0);
 call(0x027FD764,self+0x2C4AC,2);
 if(material){releaseMaterialBuffers(material);call(0x028F0164,material,4u,0x254u,0x025D2294u,0,0);}
 destroyShapeGroups(self+0x2BB38);call(0x027F13DC,self,0);if(flags&1)call(0x0273AF40,self);
}
VERIFY(0x025D0190,packetDelete);

void calcCollision(u32 self){
 WWHD_FUNC(0x025D0850,void,self);
 s32 room=load<s8>(0x1047E6C8);if((u32)room>=64)return;
 u32 game=call<u32>(0x025200D4);f32 radius=cf(0x10056A1C);call(0x020184DC,game+0x5008,radius);call(0x02018428,game+0x5008,radius);store<u8>(game+0x5020,19);store<u8>(game+0x5021,1);
 u32 header=self+0x1F038+(u32)room*0x32C,begin=load<u32>(header+8),end=begin+load<u32>(header)*4;
 for(u32 p=begin;p!=end;p+=4){u32 unit=load<u32>(p);if(!(load<u32>(unit+0x10)&4))call(0x025CE5D0,unit,self);}
 game=call<u32>(0x025200D4);radius=cf(0x10056A20);call(0x020184DC,game+0x5008,radius);call(0x02018428,game+0x5008,radius);store<u8>(game+0x5020,18);store<u8>(game+0x5021,1);
 // The second native pass has no body; it only advances the same room-array iterator.
}
VERIFY(0x025D0850,calcCollision);
void packetCalc(u32 self){
 WWHD_FUNC(0x025D0994,void,self);
 calcCollision(self);u32 vector=call<u32>(0x0257DAA8);f32 strength=call<f32>(0x02578348);s32 angle=call<s32>(0x020195B0,cf(vector),cf(vector+8));
 u32 animation=self+0x1C8D8;for(u32 i=0;i<8;++i)wind(animation+i*0x8C,strength,(s16)angle);
 for(u32 i=0;i<72;++i)play(animation+i*0x8C,self);
 for(u32 i=0;i<200;++i)unitProc(self+0x98+i*0x248,self);
}
VERIFY(0x025D0994,packetCalc);
void copyMatrix(u32 out,u32 in){
 WWHD_FUNC(0x025D0A6C,void,out,in);f32 matrix[12];for(u32 i=0;i<12;++i)matrix[i]=cf(in+i*4);for(u32 i=0;i<12;++i)store<f32>(out+i*4,matrix[i]);
}
VERIFY(0x025D0A6C,copyMatrix);
void unpackColor(u32 out,u32 in){
 WWHD_FUNC(0x025D0B0C,void,out,in);f32 scale=cf(0x10056A24),values[4];for(u32 i=0;i<4;++i)values[i]=(f32)((f64)(f32)load<s16>(in+i*2)/(f64)scale);for(u32 i=0;i<4;++i)store<f32>(out+i*4,values[i]);
}
VERIFY(0x025D0B0C,unpackColor);
s32 compareDepth(u32 a,u32 b){WWHD_FUNC(0x025D0BD0,s32,a,b);return cf(a+0x244)<cf(b+0x244)?1:-1;}
VERIFY(0x025D0BD0,compareDepth);
void update(u32 self){
 WWHD_FUNC(0x025D15B0,void,self);
 f32 radius=cf(0x10056A64);gabi::Local<u8[12]> point;
 for(u32 i=0;i<200;++i){u32 unit=self+0x98+i*0x248;f32 x=cf(unit),y=cf(unit+4),z=cf(unit+8);if(!(load<u32>(unit+0x10)&1))continue;
 store<f32>(point.a,x);store<f32>(point.a+4,y);store<f32>(point.a+8,z);
 s32 outside=call<s32>(0x02838148,0x1048CFF0u,0x104B45F8u,point.a,radius);u32 flags=load<u32>(unit+0x10);
 if(outside)store<u32>(unit+0x10,flags|2);else{store<u32>(unit+0x10,flags&~2u);setMatrix(unit,self+0x1C8D8);}}
 u32 game=call<u32>(0x025200D4);u32 list=load<u32>(game+0x5D74);store<u32>(0x104B4638,list);call(0x027F0E04,load<u32>(0x104B4634),self,0);
 call(0x027F0E04,load<u32>(0x104B4638),self+0x4A690,0);
 game=call<u32>(0x025200D4);store<u32>(0x104B4638,load<u32>(game+0x5D7C));call(0x025D0BEC,self);
}
VERIFY(0x025D15B0,update);
void init(){
 WWHD_FUNC(0x025D1D6C,void);store<u32>(0x1048736C,0);store<u32>(0x10487364,0);store<u32>(0x10487370,0);store<u32>(0x10487368,0);call(0x028F026C,0x101F2F44u);
 f32 a=cf(0x10056AA0),b=cf(0x10056AA4);store<f32>(0x10487358,a);store<f32>(0x1048735C,b);call(0x028ED6F8,0x10487360u);call(0x028F026C,0x101F2F50u);call(0x028EAB2C,0x10487361u);call(0x028F026C,0x101F2F5Cu);
}
VERIFY(0x025D1D6C,init);
// Inline graphics companions emitted after the TU initializer, referenced by Packet arrays.
void deleteEmpty(u32 self,u32 flags){WWHD_FUNC(0x025D1E00,void,self,flags);if(self&&(flags&1))call(0x0273AF40,self);}
VERIFY(0x025D1E00,deleteEmpty);
u32 createVector4(u32 self){WWHD_FUNC(0x025D1E14,u32,self);return self?self:call<u32>(0x0273AD10,16u);}
VERIFY(0x025D1E14,createVector4);
void emptyVector4(u32 self){WWHD_FUNC(0x025D1E40,void,self);}
VERIFY(0x025D1E40,emptyVector4);
u32 createTransform(u32 self){
 WWHD_FUNC(0x025D1E44,u32,self);if(!self)self=call<u32>(0x0273AD10,0xA8u);
 if(self){call(0x027FB40C,self);store<u32>(self+0xC,0x1016EF84);call(0x028F521C,self+0x74,0x34u);if(self+0x74==0)call(0x0273AD10,0x30u);}return self;
}
VERIFY(0x025D1E44,createTransform);
void deleteTransform(u32 self,u32 flags){WWHD_FUNC(0x025D1EB4,void,self,flags);if(self){call(0x027FB528,self,0);if(flags&1)call(0x0273AF40,self);}}
VERIFY(0x025D1EB4,deleteTransform);
u32 createTextureTransform(u32 self){
 WWHD_FUNC(0x025D1F08,u32,self);if(!self)self=call<u32>(0x0273AD10,0x364u);
 if(self){call(0x027FB40C,self);store<u32>(self+0xC,0x1016EFB4);call(0x028F521C,self+0x74,0x2F0u);
 f32 zero=cf(0x10145180),one=cf(0x1014517C);for(u32 i=0;i<11;++i){for(u32 j=0;j<3;++j)store<f32>(self+0x74+i*16+j*4,zero);store<f32>(self+0x80+i*16,one);}
 for(u32 off:{0x124u,0x144u,0x164u})call(0x028EFFD0,self+off,2u,16u,0x025D1E14u);
 for(u32 off=0x184;off<=0x2D4;off+=0x30)if(self+off==0)call(0x0273AD10,0x30u);
 for(u32 off=0x304;off<=0x354;off+=0x10)if(self+off==0)call(0x0273AD10,0x10u);
 }return self;
}
VERIFY(0x025D1F08,createTextureTransform);
void deleteTextureTransform(u32 self,u32 flags){WWHD_FUNC(0x025D2190,void,self,flags);if(self){call(0x027FB528,self,0);if(flags&1)call(0x0273AF40,self);}}
VERIFY(0x025D2190,deleteTextureTransform);
void deletePacketBase(u32 self,u32 flags){WWHD_FUNC(0x025D21E4,void,self,flags);if(self){call(0x027F13DC,self,0);if(flags&1)call(0x0273AF40,self);}}
VERIFY(0x025D21E4,deletePacketBase);
u32 createMaterial(u32 self){WWHD_FUNC(0x025D2238,u32,self);if(!self)self=call<u32>(0x0273AD10,0x254u);if(self){call(0x027B5BD8,self+4);call(0x027BF734,self+0x158);store<u32>(self+0x250,0);store<u32>(self+0x24C,0);}return self;}
VERIFY(0x025D2238,createMaterial);
void deleteMaterial(u32 self,u32 flags){WWHD_FUNC(0x025D2294,void,self,flags);if(self){call(0x027BF880,self+0x158,2);call(0x027B5CBC,self+4,2);if(flags&1)call(0x0273AF40,self);}}
VERIFY(0x025D2294,deleteMaterial);
void emptyMaterial(u32 self){WWHD_FUNC(0x025D22F4,void,self);}
VERIFY(0x025D22F4,emptyMaterial);

static u32 materialInstance(u32 name){
 gabi::Local<u32[2]> desc;store<u32>(desc.a,name);store<u32>(desc.a+4,0x1005672C);
 u32 manager=call<u32>(0x027FFCBC);s32 index=call<s32>(0x027B90AC,load<u32>(manager+4),desc.a);if(index<0)return 0;
 u32 result=load<u32>(manager+12)+((u32)index<load<u32>(manager+8)?(u32)index*36:0);
 if(!load<u8>(result+0x20)){u32 resource=load<u32>(manager+4);u32 binding=(u32)index<load<u32>(resource+28)?load<u32>(resource+32)+(u32)index*132:0;call(0x02800B0C,result,binding,0);result=load<u32>(manager+12)+((u32)index<load<u32>(manager+8)?(u32)index*36:0);}return result;
}
static u32 heapAllocate(u32 bytes,u32 align){u32 heap=call<u32>(0x02756140,load<u32>(0x101F8B4C));return gabi::call_ptr<u32>(load<u32>(load<u32>(heap+12)+0x34),heap,bytes,align);}
static void createShapeGroups(u32 header,u32 material){
 for(u32 i=0;i<load<u32>(header);++i){u32 row=load<u32>(header+8)+(i<load<u32>(header+4)?i*20:0),original=load<u32>(row);store<u32>(row,0);destroyShapeArray(row,4,8);destroyShapeArray(row,12,16);store<u32>(row,original);
 for(u32 group=0;group<2;++group){u32 array=heapAllocate(0x1E8,4);for(u32 j=0;j<2;++j){u32 shape=array+j*0xF4;if(shape)call(0x027BF734,shape);}if(array){store<u32>(row+8+group*8,array);store<u32>(row+4+group*8,2);}}
 for(u32 group=0;group<2;++group)for(u32 j=0;j<2;++j){u32 shape=load<u32>(row+8+group*8)+(j<load<u32>(row+4+group*8)?j*0xF4:0);call(0x027FF530,original,shape,material+4+group*0x254+j*0x4A8,material+0x954,0);}
 }
}
static void clearVertexBuffer(u32 buffer){u32 end=buffer+0x1260;for(u32 p=buffer;p<end;p+=32)for(u32 j=0;j<32;j+=4)store<u32>((p&~31u)+j,0);}
static void fillVertices(u32 material,u32 group){
 u32 part=material+load<u32>(material+0x950)*0x254+group*0x4A8;clearVertexBuffer(load<u32>(part));
 u32 countAddress=group?0x101F13D0:0x101F13C8,position=group?0x101F2830:0x101F13D8,normal=group?0x101F29D4:0x101F1870,color=group?0x101F2B78:0x101F1D08,uv=group?0x101F2DA8:0x101F2328;
 part=material+load<u32>(material+0x950)*0x254+group*0x4A8;u32 out=load<u32>(part);
 for(u32 i=0;i<load<u32>(countAddress);++i){for(u32 j=0;j<3;++j)store<f32>(out+i*48+j*4,cf(position+i*12+j*4));for(u32 j=0;j<3;++j)store<f32>(out+i*48+12+j*4,cf(normal+i*12+j*4));for(u32 j=0;j<4;++j)store<f32>(out+i*48+32+j*4,cf(color+i*16+j*4));for(u32 j=0;j<2;++j)store<f32>(out+i*48+24+j*4,cf(uv+i*8+j*4));}
}
u32 packetCreate(u32 self){
 WWHD_FUNC(0x025CF20C,u32,self);if(!self)self=call<u32>(0x0273AD10,0x7D3D0u);if(!self)return self;
 call(0x027F1278,self);store<u32>(self+12,0x10056AE8);call(0x028EFFD0,self+0x98,200u,0x248u,0x025CE1B4u);call(0x028EFFD0,self+0x1C8D8,72u,0x8Cu,0x025CD26Cu);call(0x028EFFD0,self+0x1F038,64u,0x32Cu,0x025CEBB8u);
 u32 base=self+0x2BB38,material=base+12,r=self+0x4A438;store<u32>(base,0);u32 header=base+4;if(!header)header=call<u32>(0x0273AD10,8u);if(header){store<u32>(header+4,0);store<u32>(header,0);}
 u32 m=material;if(!m)m=call<u32>(0x0273AD10,0x968u);if(m){call(0x028EFFD0,m,4u,0x254u,0x025D2238u);store<u32>(m+0x950,0);store<u32>(m+0x960,0);store<u32>(m+0x958,0x30);store<u8>(m+0x964,0);store<u32>(m+0x954,0);for(u32 i=0;i<2;++i)for(u32 j=0;j<2;++j)store<u32>(m+i*0x254+j*0x4A8,0);}
 call(0x027FD6F4,base+0x974);call(0x028EFFD0,self+0x2C4B8,400u,0xA8u,0x025D1E44u);call(0x028EFFD0,self+0x3CB38,64u,0x364u,0x025D1F08u);
 call(0x027B5430,r);call(0x027B5430,r+24);call(0x027BDF7C,r+48);call(0x027BE6B8,r+0x1C8);call(0x027F1278,r+0x258);store<u32>(r+0x264,0x10056AB8);
 header=r+0x2F4;if(!header)header=call<u32>(0x0273AD10,0x32Cu);if(header){store<u32>(header,0);store<u32>(header+4,0);store<u32>(header+8,0);call(0x0273B560,header,200u,header+12);}
 store<u32>(r+0x620,0);header=r+0x624;if(!header)header=call<u32>(0x0273AD10,8u);if(header){store<u32>(header,0);store<u32>(header+4,0);}call(0x027FD6F4,r+0x62C);
 call(0x028EFFD0,r+0x638,200u,0xA8u,0x025D1E44u);call(0x028EFFD0,self+0x52DB0,200u,0x364u,0x025D1F08u);
 for(u32 i=0;i<8;++i)normalInit(self+0x1C8D8+i*0x8C);
 call(0x0280068C,base,materialInstance(0x100569A0),0);call(0x0280068C,r+0x620,materialInstance(0x100569AC),0);store<u32>(material+0x954,0x1013);store<u32>(material+0x95C,0x10056AA8);
 for(u32 i=0;i<2;++i)for(u32 j=0;j<2;++j){u32 part=material+i*0x254+j*0x4A8;if(!load<u32>(part)){u32 buffer=heapAllocate(0x1260,0x40);if(buffer){store<u32>(part+0x250,buffer);store<u32>(part+0x24C,98);}store<u32>(part,load<u32>(part+0x250));}call(0x027FF478,part+4,load<u32>(part),98u,material+0x954);}
 store<u32>(material+0x960,0);store<u8>(material+0x964,1);createShapeGroups(base,material);createShapeGroups(r+0x620,material);call(0x027FD838,base+0x974,1,0);call(0x027FD838,r+0x62C,1,0);
 for(u32 i=0;i<64;++i)call(0x027FB5D4,self+0x3CB38+i*0x364,0);for(u32 i=0;i<200;++i)call(0x027FB5D4,self+0x52DB0+i*0x364,0);for(u32 i=0;i<400;++i)call(0x027FB5D4,self+0x2C4B8+i*0xA8,0);for(u32 i=0;i<200;++i)call(0x027FB5D4,r+0x638+i*0xA8,0);
 call(0x027B54E0,r,0x101F2638u,4,load<u32>(0x101F13CC));store<u32>(r+4,4);call(0x027B54E0,r+24,0x101F2EC0u,4,load<u32>(0x101F13D4));store<u32>(r+28,4);
 fillVertices(material,0);fillVertices(material,1);
 // Cache both buffer pointers once per vertex, then interleave scalar float transfers as native.
 for(u32 group=0;group<2;++group){u32 index=load<u32>(material+0x950),src=material+(index+group*2)*0x254,dst=material+((index?0:1)+group*2)*0x254;for(u32 i=0;i<98;++i){u32 from=load<u32>(src),to=load<u32>(dst);for(u32 j=0;j<12;++j)store<f32>(to+i*48+j*4,std::bit_cast<f32>(load<u32>(from+i*48+j*4)));}}
 u32 part=material+load<u32>(material+0x950)*0x254+4;for(u32 i=0;i<2;++i){call(0x027B5E94,part,0,load<u32>(part+0x14C));part+=0x4A8;}store<u32>(material+0x950,load<u32>(material+0x950)?0:1);
 call(0x0274FBF8,load<u32>(0x101F8B18));gabi::Local<u32[2]> a,b;store<u32>(a.a,0x100569BC);store<u32>(a.a+4,0x1005672C);store<u32>(b.a,0x100569CC);store<u32>(b.a+4,0x1005672C);u32 resource=call<u32>(0x026124B0,load<u32>(0x101F4F7C),a.a,b.a,0);call(0x02773870,r+0x1C8,resource,0x10056994u);
 bool equal=true;for(u32 off:{0x34u,0x38u,0x3Cu,0x40u,0x44u,0x48u,0x68u,0x64u,0x4Cu})if(load<u32>(r+off)!=load<u32>(r+off+0x198)){equal=false;break;}
 if(!equal){call(0x027BDEB4,r+48,r+0x1C8);store<u32>(r+0x18C,1);store<u32>(r+0x190,1);store<u32>(r+0x194,1);store<u8>(r+0x1C0,load<u8>(r+0x1C0)|2);}else{u32 x=load<u32>(r+0x1F0);store<u32>(r+0x18C,1);u32 y=load<u32>(r+0x1F8);u8 flags=load<u8>(r+0x1C0);store<u32>(r+0x60,y);store<u32>(r+0x58,x);store<u32>(r+0x190,1);store<u32>(r+0x10C,y);store<u8>(r+0x1C0,flags|2);store<u32>(r+0x104,x);store<u32>(r+0x194,1);}
 call(0x0274FCCC,load<u32>(0x101F8B18));store<u32>(r+0x2F0,self);return self;
}
VERIFY(0x025CF20C,packetCreate);

void drawRooms(u32 self,u32 draw){
 WWHD_FUNC(0x025D16E0,void,self,draw);
 u32 mode=load<u32>(draw+0xC),material=0,header=self+0x2BB38;
 if((s32)mode<4){u32 capacity=load<u32>(header+4),array=load<u32>(header+8);material=load<u32>(array+(mode<capacity?mode*20:0));}
 u32 cache=call<u32>(0x027F29D4,0x104B45C0u),program=load<u32>(material);
 if(program!=load<u32>(cache+4)){
 u8 flags=load<u8>(program);u32 old=load<u32>(cache);
 if(flags&2){store<u8>(program,flags&~2);call(0x027BB9E0,program,0);}
 u32 state=load<u32>(load<u32>(program+0x7C)+0x28);if(old!=state)call(0x027B9F68,state);
 u32 size=load<u32>(program+0xC);if(size)call(0xC00060E0,load<u32>(program+4),size);else call(0x027BB7CC,program);
 store<u32>(cache,state);store<u32>(cache+4,program);
 }
 mode=load<u32>(draw+0xC);
 if(mode==0){u32 table=load<u32>(draw+0x14);if(table){u32 capacity=load<u32>(table),index=load<u32>(draw+4),shape=load<u32>(table+4)+(index<capacity?index*0x23C:0);bindUniforms(material,shape);}}
 else if(mode==1)bindUniforms(material,load<u32>(self+0x2C4B0));
 else if(mode==2){bindUniforms(material,load<u32>(self+0x2C4B0));u32 object=load<u32>(draw+0x30);if(object){u32 target=load<u32>(load<u32>(object+0xC)+0x2C);gabi::call_ptr(target,object,material);}}

 if(mode<=2){u32 samplers=load<u32>(material+0x14)?load<u32>(material+0x18):0;call(0x027BE53C,self+0x4A468,samplers+4,0,0);if(mode==2)call(0x027FFE54,draw,material);}
 gabi::Local<u8[0x11C]> state;call(0x02750250,state.a);mode=load<u32>(draw+0xC);store<u32>(state.a+0xC,2);store<u8>(state.a+0xE0,1);store<u32>(state.a+0xE4,4);store<f32>(state.a+0xE8,cf(0x10056A68));store<u32>(state.a+0xEC,(((load<u32>(state.a+0xEC)&~15u)+7)&0xFFFFFF0F)+16);call(0x0280037C,mode,state.a);call(0x02750370,state.a);
 u32 transformIndex=0,textureIndex=0;
 for(u32 room=0;room<64;++room){u32 roomHeader=self+0x1F038+room*0x32C,begin=load<u32>(roomHeader+8),end=begin+load<u32>(roomHeader)*4;
 if(begin!=end){u32 texture=self+0x3CB38+textureIndex*0x364;gabi::call_ptr(load<u32>(load<u32>(texture+0xC)+0x2C),texture,material);++textureIndex;}
 for(u32 ptr=begin;ptr!=end;ptr+=4){u32 unit=load<u32>(ptr);if(load<u32>(unit+0x10)&2)continue;u32 animationIndex=load<u32>(unit+0x14);if(animationIndex>=72)call(0x0273AA24,0x10056A6Cu,0xB89,0x10056A78u);
 bool simple=load<u8>(self+0x1C962+animationIndex*0x8C)==255&&!(load<u32>(load<u32>(ptr)+0x10)&4);
 u32 transform=self+0x2C4B8+transformIndex*0xA8;gabi::call_ptr(load<u32>(load<u32>(transform+0xC)+0x2C),transform,material);++transformIndex;
 if(simple){mode=load<u32>(draw+0xC);u32 row=load<u32>(header+8)+(mode<load<u32>(header+4)?mode*20:0),select=load<u32>(self+0x2C494)?0:8;call(0x027BFE5C,load<u32>(row+select+8));u32 count=load<u32>(self+0x4A444);if(count)call(0xC0006178,load<u32>(self+0x4A43C),count,load<u32>(self+0x4A438),load<u32>(self+0x4A440),0,1);
 transform=self+0x2C4B8+transformIndex*0xA8;gabi::call_ptr(load<u32>(load<u32>(transform+0xC)+0x2C),transform,material);++transformIndex;}
 mode=load<u32>(draw+0xC);u32 row=load<u32>(header+8)+(mode<load<u32>(header+4)?mode*20:0),select=load<u32>(self+0x2C494)?0:8,array=load<u32>(row+select+8),count=load<u32>(row+select+4);call(0x027BFE5C,array+(count>1?0xF4:0));count=load<u32>(self+0x4A45C);if(count)call(0xC0006178,load<u32>(self+0x4A454),count,load<u32>(self+0x4A450),load<u32>(self+0x4A458),0,1);
 }
 }call(0x02750370,0x104B474Cu);
}
VERIFY(0x025D16E0,drawRooms);

// out: the frame slot the original uses for this copy (r1+0x20 / +0x50 / +0x80).
static void copyTransform(u32 out,u32 in,u32 offset,u32 matrix){for(u32 j=0;j<12;++j)store<f32>(out+j*4,cf(in+offset+j*4));call(0x028E90D4,out,matrix+0x74);call(0x027FB678,matrix);}
// tev = light state +0x90: four s16 colour components at +0..+6, four u8 at +8..+B (025D0FA0/025D1040).
static void textureColors(u32 object,u32 tev,u32 animation,f32 alpha,f32 divisor,u32 matrixSet){
 for(u32 j=0;j<4;++j)store<f32>(object+0xB4+j*4,(f32)((f64)(f32)load<s16>(tev+j*2)/(f64)divisor));
 f32 color[4];for(u32 j=0;j<4;++j)color[j]=(f32)((f64)(f32)load<u8>(tev+8+j)/(f64)divisor);
 store<f32>(object+0xC4,color[0]);store<f32>(object+0xCC,color[2]);store<f32>(object+0xC8,color[1]);store<f32>(object+0xD0,color[3]);store<f32>(object+0xE0,alpha);
 call(0x0274D2AC,object+0xC4,cf(tev-0x90+0x24));u32 descriptor=load<u32>(matrixSet+4);for(u32 j=0;j<4;++j)store<u32>(object+0x94+j*4,load<u32>(descriptor+0x1C4+j*4));call(0x027FB678,object);
}
void prepareDraw(u32 self){
 WWHD_FUNC(0x025D0BEC,void,self);
 // The original 0x360-byte frame (r1 = entry SP - 0x360): matrix copies +0x20/+0x50/+0x80, light state +0xB0,
 // converted colour +0x278, fog colours +0x288/+0x298/+0x2A8/+0x2B8, view +0x2C8. Same layout here, so recorded
 // callee outputs land in the same objects.
 gabi::Local<u8[0x360]> frame;const u32 r1=frame.a;struct Slot{u32 a;};Slot view{r1+0x2C8},converted{r1+0x278},fog[4]={{r1+0x288},{r1+0x298},{r1+0x2A8},{r1+0x2B8}};
 copyMatrix(view.a,0x104B45F8);
 // One full 0x1C8 HD tev record includes the embedded lighting components at +0xC0/+0x144.
 u32 lightState=r1+0xB0;u32 light[3]={lightState,lightState+0xC0,lightState+0x144};for(u32 i=0;i<3;++i){for(u32 off=0;off<0x18;off+=4)store<f32>(light[i]+off,cf(0x1016E414+off));for(u32 off=0x18;off<0x1C;++off)store<u8>(light[i]+off,load<u8>(0x1016E414+off));for(u32 off=0x1C;off<0x24;off+=2)store<u16>(light[i]+off,load<u16>(0x1016E414+off));for(u32 off=0x24;off<=0x40;off+=4)store<f32>(light[i]+off,cf(0x1016E414+off));}
 call(0x0255FFF4,light[0],(s32)load<s8>(0x1047E6C8),255u);u32 env=call<u32>(0x02555D0C);call(0x025626A4,env,1,0,light[0]);call(0x0255FAF0,light[0]);
 u32 textureSet=self+0x2C4AC,transformSet=self+0x4AA64;call(0x027FDA54,textureSet,0,view.a,0x104B470Cu,load<u32>(0x104B4708)+0x240);call(0x027FDA54,transformSet,0,view.a,0x104B470Cu,load<u32>(0x104B4708)+0x240);
 for(u32 i=0;i<2;++i){u32 set=i?transformSet:textureSet;for(u32 j=0;j<2;++j){u32 descriptor=load<u32>(set+4);unpackColor(converted.a,(j?light[2]+0x1C:light[0]+0x90));call(0x0274D458,fog[i*2+j].a,converted.a,cf((j?light[2]:light[0])+0x28));for(u32 k=0;k<4;++k)store<u32>(descriptor+0x1C4+j*16+k*4,load<u32>(fog[i*2+j].a+k*4));}call(0x027FE010,set);}
 f32 fixedAlpha=cf(0x100566E0),divisor=cf(0x10056A24);u32 normalIndex=0,textureIndex=0,fadeIndex=0,instanceHeader=self+0x4A72C;store<u32>(instanceHeader,0);
 for(u32 room=0;room<64;++room){u32 header=self+0x1F038+room*0x32C,begin=load<u32>(header+8),end=begin+load<u32>(header)*4;
 if(begin!=end){call(0x0273B7B4,header,0x025D0BD0u);textureColors(self+0x3CB38+textureIndex*0x364,light[0]+0x90,0,fixedAlpha,divisor,textureSet);++textureIndex;}
 for(u32 ptr=begin;ptr!=end;ptr+=4){u32 unit=load<u32>(ptr);if(load<u32>(unit+0x10)&2)continue;u32 index=load<u32>(unit+0x14);if(index>=72){call(0x0273AA24,0x10056A30u,0xC61,0x10056A3Cu);unit=load<u32>(ptr);}u32 animation=self+0x1C8D8+index*0x8C;u8 alpha=load<u8>(animation+0x8A);u32 normalBase=self+0x2C4B8;
 if(alpha&&alpha==255&&!(load<u32>(unit+0x10)&4)){copyTransform(r1+0x50,unit,0x18,normalBase+normalIndex*0xA8);++normalIndex;unit=load<u32>(ptr);}
 else if(!alpha||(load<u32>(unit+0x10)&4)){copyTransform(r1+0x20,unit,0x48,normalBase+normalIndex*0xA8);++normalIndex;continue;}
 else{u32 texture=self+0x52DB0+fadeIndex*0x364;textureColors(texture,light[0]+0x90,animation,(f32)((f64)(f32)alpha/(f64)divisor),divisor,transformSet);unit=load<u32>(ptr);copyTransform(r1+0x80,unit,0x18,self+0x4AA70+fadeIndex*0xA8);++fadeIndex;if(load<s32>(instanceHeader)<load<s32>(instanceHeader+4)){store<u32>(load<u32>(instanceHeader+8)+load<u32>(instanceHeader)*4,load<u32>(ptr));store<u32>(instanceHeader,load<u32>(instanceHeader)+1);}unit=load<u32>(ptr);}
 copyTransform(r1+0x20,unit,0x48,normalBase+normalIndex*0xA8);++normalIndex;
 }
 }
}
VERIFY(0x025D0BEC,prepareDraw);

}
