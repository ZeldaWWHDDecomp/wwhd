#include "gabi.h"
using gabi::load;using gabi::store;
void vib_Default(void* self) {
 WWHD_FUNC(0x025CB334,void,self);u32 a=gabi::ea(self);
 store<s32>(a+0x24,-1);store<s32>(a+0x70,-99);
 store<s32>(a+0x48,-1);store<s32>(a+0x44,-99);
 store<s32>(a+0x60,-1);store<s32>(a,-1);
 store<s32>(a+0x58,-99);store<s32>(a+0x7C,0);
 store<s32>(a+0x5C,-99);store<s32>(a+0x78,0);
 store<s32>(a+0x20,-99);store<s32>(a+0x74,-99);
}
VERIFY(0x025CB334,vib_Default);
u8 vib_StartShock(void* self,u32 pattern,u32 flags,void* coord) {
 WWHD_FUNC(0x025CB374,u8,self,pattern,flags,coord);u32 a=gabi::ea(self),v=gabi::ea(coord);u8 result=0;
 if(flags&0x3E) {
  store<u32>(a+0x20,0);store<u32>(a+0x10,flags);store<u32>(a,pattern);
  store<u32>(a+0x14,load<u32>(v));store<u32>(a+0x18,load<u32>(v+4));store<u32>(a+0x1C,load<u32>(v+8));
  u32 p=0x10056298+(pattern<<3);store<u32>(a+4,load<u32>(p+4));store<u32>(a+8,load<u16>(p+2));store<u32>(a+0xC,load<u16>(p));result=1;
 }
 if(flags&1) {u32 p=0x10056368+(pattern<<3);store<u32>(a+0x48,pattern);store<u32>(a+0x58,0);store<u32>(a+0x4C,load<u32>(p+4));store<u32>(a+0x50,load<u16>(p+2));result=1;}
 return result;
}
VERIFY(0x025CB374,vib_StartShock);
s32 vib_StopQuake(void* self,u32 flags) {
 WWHD_FUNC(0x025CB610,s32,self,flags);u32 a=gabi::ea(self);s32 result=0;
 if(flags&0x3E){u32 remaining=load<u32>(a+0x34)&~flags;store<u32>(a+0x34,remaining);if(!remaining)store<s32>(a+0x24,-1);store<u32>(a+0x44,0);result=1;}
 if((flags&1)&&load<s32>(a+0x60)!=-1){store<u32>(a+0x70,0);store<s32>(a+0x60,-1);result=1;}return result;
}
VERIFY(0x025CB610,vib_StopQuake);
u8 vib_CheckQuake(void* self) {
 WWHD_FUNC(0x025CB66C,u8,self);u32 a=gabi::ea(self);return load<s32>(a+0x24)!=-1||load<s32>(a+0x60)!=-1;
}
VERIFY(0x025CB66C,vib_CheckQuake);
void vib_Init(void* self) {
 WWHD_FUNC(0x025CB694,void,self);gabi::call<void>(0x025F2AAC,u32(0));gabi::call<void>(0x025F2AAC,u32(0));gabi::call<void>(0x025CB334,self);
}
VERIFY(0x025CB694,vib_Init);
void vib_Pause(void* self) {
 WWHD_FUNC(0x025CB6D4,void,self);u32 a=gabi::ea(self);if(load<s32>(a+0x7C)==-1)return;
 if(load<s32>(a+0x48)!=-1||load<s32>(a+0x60)!=-1){gabi::call<void>(0x025F2AAC,u32(0));gabi::call<void>(0x025F2AAC,u32(0));}
 s32 camera=load<s32>(a+0x24),motor=load<s32>(a+0x60);
 store<s32>(a+0x20,-99);store<s32>(a+0x48,-1);store<s32>(a,-1);store<s32>(a+0x58,-99);
 if(camera!=-1)store<u32>(a+0x44,0);if(motor!=-1)store<u32>(a+0x70,0);store<s32>(a+0x7C,-1);
}
VERIFY(0x025CB6D4,vib_Pause);
void vib_ResetWrapper(void* self) {WWHD_FUNC(0x025CC0C8,void,self);gabi::call<void>(0x025CB334,self);}
VERIFY(0x025CC0C8,vib_ResetWrapper);
void* vib_Construct(void* self) {
 WWHD_FUNC(0x025CC0CC,void*,self);
 if(!self)self=gabi::call<void*>(0x0273AD10,u32(0x84));
 if(self){store<u32>(gabi::ea(self)+0x80,0x10056530);return gabi::call<void*>(0x025CB334,self);}return self;
}
VERIFY(0x025CC0CC,vib_Construct);
void vib_Destroy(void* self,u32 flags) {
 WWHD_FUNC(0x025CC110,void,self,flags);if(self){store<u32>(gabi::ea(self)+0x80,0x10056530);gabi::call<void>(0x025CB694,self);if(flags&1)gabi::call<void>(0x0273AF40,self);}
}
VERIFY(0x025CC110,vib_Destroy);

u8 vib_StartQuake(void* self,u32 pattern,u32 flags,void* coord) {
 WWHD_FUNC(0x025CB408,u8,self,pattern,flags,coord);u32 a=gabi::ea(self),v=gabi::ea(coord);u8 result=0;
 if(flags&0x3E){store<u32>(a+0x24,pattern);store<u32>(a+0x44,0);store<u32>(a+0x34,flags);
 store<u32>(a+0x38,load<u32>(v));store<u32>(a+0x3C,load<u32>(v+4));store<u32>(a+0x40,load<u32>(v+8));
 u32 p=0x10056438+(pattern<<3);store<u32>(a+0x28,load<u32>(p+4));store<u32>(a+0x2C,load<u16>(p+2));store<u32>(a+0x30,load<u16>(p));result=1;}
 if(flags&1){u32 p=0x10056498+(pattern<<3);store<u32>(a+0x60,pattern);store<u32>(a+0x70,0);store<u32>(a+0x64,load<u32>(p+4));store<u32>(a+0x68,load<u16>(p+2));store<u32>(a+0x6C,load<u16>(0x10056438+(pattern<<3)));result=1;}return result;
}
VERIFY(0x025CB408,vib_StartQuake);
static u32 shift_left(u32 v,u32 n){return (n&32)?0:v<<(n&31);}
static u32 shift_right(u32 v,u32 n){return (n&32)?0:v>>(n&31);}
u32 vib_MakeBits(u32 pattern,u32 length,u32 desired) {
 WWHD_FUNC(0x025CB4A8,u32,pattern,length,desired);u32 masked=pattern&shift_left(~u32(0),32-length),result=masked;
 for(u32 i=length;s32(i)<s32(desired);){i+=length;result=shift_right(result,length)|masked;}return result;
}
VERIFY(0x025CB4A8,vib_MakeBits);
u8 vib_CustomQuake(void* self,void* pattern,u32 rounds,u32 flags,void* coord) {
 WWHD_FUNC(0x025CB4E0,u8,self,pattern,rounds,flags,coord);u32 a=gabi::ea(self),p=gabi::ea(pattern),v=gabi::ea(coord),length=load<u16>(p),bits=0;u8 result=0;
 if(length>32)gabi::call<void>(0x0273AA24,gabi::at<void>(0x10056504),u32(0x1B3),gabi::at<void>(0x100564FC));
 if(length){bits=u32(load<u8>(p+2))<<24;if(length>=9){bits|=u32(load<u8>(p+3))<<16;if(length>=17){bits|=u32(load<u8>(p+4))<<8;if(length>=25)bits|=load<u8>(p+5);}}}
 if(flags&0x3E){store<u32>(a+0x24,0);store<u32>(a+0x44,0);store<u32>(a+0x34,flags);
 store<u32>(a+0x38,load<u32>(v));store<u32>(a+0x3C,load<u32>(v+4));store<u32>(a+0x40,load<u32>(v+8));
 u32 extended=gabi::call<u32>(0x025CB4A8,bits,u32(load<u8>(p+1)),u32(32));store<u32>(a+0x2C,32);store<u32>(a+0x30,rounds);store<u32>(a+0x28,extended);result=1;}
 if(flags&1){store<u32>(a+0x60,0);store<u32>(a+0x70,0);u32 extended=gabi::call<u32>(0x025CB4A8,bits,u32(load<u8>(p+1)),u32(32));store<u32>(a+0x6C,rounds);store<u32>(a+0x68,32);store<u32>(a+0x64,extended);result=1;}return result;
}
VERIFY(0x025CB4E0,vib_CustomQuake);
u32 vib_RollShift(u32 pattern,u32 length,u32 frame) {
 WWHD_FUNC(0x025CB878,u32,pattern,length,frame);u32 quotient=ppc_divw(frame,length),pos=frame-quotient*length;
 return shift_right(pattern,pos)|shift_left(pattern,length-pos);
}
VERIFY(0x025CB878,vib_RollShift);
void* vib_MakeData(void* dest,u32 pattern,u32 length) {
 WWHD_FUNC(0x025CB898,void*,dest,pattern,length);u32 a=gabi::ea(dest);
 u32 state=load<u32>(load<u32>(0x101F5088)+0x1D0);
 if(!state){store<u16>(a,u16(length));store<u16>(a+2,u16(pattern>>16));store<u16>(a+0xE,0);store<u16>(a+0xC,0);store<u16>(a+6,0);store<u16>(a+0x10,0);store<u16>(a+4,u16(pattern));store<u16>(a+0xA,0);store<u16>(a+8,0);}
 else {u32 expanded=(length<<2)&0xFFFC;store<u16>(a,u16(expanded>120?120:expanded));for(u32 group=0;group<8;++group){u16 packed=0;for(u32 bit=0;bit<4;++bit)if(pattern&(u32(1)<<(31-group*4-bit)))packed|=u16(0xF000>>(bit*4));store<u16>(a+2+group*2,packed);}}return dest;
}
VERIFY(0x025CB898,vib_MakeData);

u32 vib_RandomBit(s32 rounds,s32 length) {
 WWHD_FUNC(0x025CB77C,u32,rounds,length);u32 pattern=0;if(rounds<=0)return pattern;
 f32 size=f32(length),threshold=load<f32>(0x10056514);
 for(s32 i=0;i<rounds;++i){f32 v=f32(f64(gabi::call<f32>(0x02019788))*f64(size));u32 n;
 if(v<threshold)n=u32(gabi::ftoi(v));else n=u32(gabi::ftoi(f32(f64(v)-f64(threshold))))+0x80000000;
 pattern|=(n&32)?0:(u32(0x40000000)>>(n&31));}return pattern;
}
VERIFY(0x025CB77C,vib_RandomBit);
void vib_StaticInit() {
 WWHD_FUNC(0x025CC16C,void);
 store<u32>(0x104872A0,0);store<u32>(0x10487298,0);store<u32>(0x104872A4,0);store<u32>(0x1048729C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F12D4));
 f32 first=load<f32>(0x10056524),second=load<f32>(0x10056528);
 store<f32>(0x1048728C,first);store<f32>(0x10487290,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x10487294));gabi::call<void>(0x028F026C,gabi::at<void>(0x101F12E0));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x10487295));gabi::call<void>(0x028F026C,gabi::at<void>(0x101F12EC));
}
VERIFY(0x025CC16C,vib_StaticInit);

s32 vib_Run(void* self) {
 WWHD_FUNC(0x025CBB90,s32,self);u32 a=gabi::ea(self);
 gabi::Local<u32> pattern;gabi::Local<u32[3]> norm,sum;
 store<u32>(a+0x7C,1);
 s32 option=gabi::call<s32>(0x025B8850,gabi::at<void>(load<u32>(0x101F84DC)+0x1C4));
 if(option!=1){store<s32>(a+0x60,-1);store<s32>(a+0x58,-99);store<s32>(a+0x48,-1);store<s32>(a+0x70,-99);}
 s32 shockFrame=load<s32>(a+0x20);
 if(shockFrame==0||load<s32>(a+0x44)==0){
  u32 mode=0; s32 quakeIndex=load<s32>(a+0x24);
  if(load<s32>(a)==-1)store<s32>(a+0x20,-99);else if(shockFrame>=0)mode=1;
  if(quakeIndex==-1)store<s32>(a+0x44,-99);else if(load<s32>(a+0x44)>=0)mode|=2;
  if(mode==1){u32 length=load<u32>(a+8),bits=load<u32>(a+4);
   bits|=gabi::call<u32>(0x025CB77C,load<s32>(a+0xC),length);store<u32>(gabi::ea(pattern.get()),bits);
   gabi::call<void>(0x0201B084,gabi::at<void>(a+0x14),norm.get());void* camera=gabi::call<void*>(0x024F8044);
   gabi::call<void>(0x02515298,camera,length,pattern.get(),load<u32>(a+0x10),norm.get());
  }else if(mode==2){u32 length=load<u32>(a+0x2C);
   u32 bits=gabi::call<u32>(0x025CB878,load<u32>(a+0x28),length,load<u32>(a+0x78));
   bits|=gabi::call<u32>(0x025CB77C,load<s32>(a+0x30),length);store<u32>(gabi::ea(pattern.get()),bits);
   gabi::call<void>(0x0201B084,gabi::at<void>(a+0x38),norm.get());void* camera=gabi::call<void*>(0x024F8044);
   gabi::call<void>(0x02515298,camera,length,pattern.get(),load<u32>(a+0x34),norm.get());
  }else if(mode==3){u32 frame=load<u32>(a+0x20),bits=shift_left(load<u32>(a+4),frame),length=load<u32>(a+8)-frame;
   u32 extended=gabi::call<u32>(0x025CB4A8,load<u32>(a+0x28),load<u32>(a+0x2C),length);
   bits|=gabi::call<u32>(0x025CB878,extended,length,load<u32>(a+0x78));
   s32 rounds=load<s32>(a+0x30),shockRounds=load<s32>(a+0xC);if(shockRounds>rounds)rounds=shockRounds;
   bits|=gabi::call<u32>(0x025CB77C,rounds,length);store<u32>(gabi::ea(pattern.get()),bits);
   gabi::call<void>(0x0201AD78,gabi::at<void>(a+0x14),sum.get(),gabi::at<void>(a+0x38));
   gabi::call<void>(0x0201B084,sum.get(),norm.get());void* camera=gabi::call<void*>(0x024F8044);
   u32 flags=load<u32>(a+0x34)|load<u32>(a+0x10);
   gabi::call<void>(0x02515298,camera,length,pattern.get(),flags,norm.get());store<u32>(a+0x44,0);store<u32>(a+0x20,0);
  }else{void* camera=gabi::call<void*>(0x024F8044);gabi::call<void>(0x025153D4,camera);}
 }
 s32 motorQuakeFrame=load<s32>(a+0x70);
 if(motorQuakeFrame>=900){gabi::call<void>(0x025F2AAC,u32(0));gabi::call<void>(0x025F2C88);store<s32>(a+0x70,-1);motorQuakeFrame=-1;}
 s32 motorShockFrame=load<s32>(a+0x58);
 if(motorShockFrame==0||motorQuakeFrame==0){
  u32 mode=0;s32 quakeIndex=load<s32>(a+0x60);
  if(load<s32>(a+0x48)==-1)store<s32>(a+0x58,-99);else if(motorShockFrame>=0)mode=1;
  if(quakeIndex==-1)store<s32>(a+0x70,-99);else if(load<s32>(a+0x70)>=0)mode|=2;
  if(mode==1){u32 length=load<u32>(a+0x50),rounds=load<u32>(a+0x54),bits=load<u32>(a+0x4C);
   bits|=gabi::call<u32>(0x025CB77C,rounds,length);store<u32>(a+0x5C,length);
   void* data=gabi::call<void*>(0x025CB898,gabi::at<void>(0x104872A8),bits,length);
   gabi::call<void>(0x025F2A30,u32(0),data,u32(0));
  }else if(mode==2){u32 length=load<u32>(a+0x68);
   u32 bits=gabi::call<u32>(0x025CB878,load<u32>(a+0x64),length,load<u32>(a+0x78));
   bits|=gabi::call<u32>(0x025CB77C,load<u32>(a+0x6C),length);store<u32>(a+0x74,0x7FFFFFFF);
   void* data=gabi::call<void*>(0x025CB898,gabi::at<void>(0x104872A8),bits,length);
   gabi::call<void>(0x025F2A30,u32(0),data,u32(1));
  }else if(mode==3){u32 frame=load<u32>(a+0x58),length=load<u32>(a+0x50)-frame,bits=shift_left(load<u32>(a+0x4C),frame);
   u32 extended=gabi::call<u32>(0x025CB4A8,load<u32>(a+0x64),load<u32>(a+0x68),length);
   bits|=gabi::call<u32>(0x025CB878,extended,length,load<u32>(a+0x78));
   s32 rounds=load<s32>(a+0x6C),shockRounds=load<s32>(a+0x54);if(shockRounds>rounds)rounds=shockRounds;
   bits|=gabi::call<u32>(0x025CB77C,rounds,length);
   store<u32>(a+0x74,length);store<u32>(a+0x70,0);store<u32>(a+0x5C,length);store<u32>(a+0x58,0);
   void* data=gabi::call<void*>(0x025CB898,gabi::at<void>(0x104872A8),bits,length);
   gabi::call<void>(0x025F2A30,u32(0),data,u32(0));
  }else{gabi::call<void>(0x025F2AAC,u32(0));gabi::call<void>(0x025F2C88);store<s32>(a+0x5C,-99);store<s32>(a+0x74,-99);}
 }
 u32 frame=load<u32>(a+0x20);if(s32(frame)>=-1){++frame;store<u32>(a+0x20,frame);if(s32(frame)>load<s32>(a+8)){store<u32>(a+0x20,0);store<s32>(a,-1);}}
 frame=load<u32>(a+0x58);if(s32(frame)>=-1){++frame;store<u32>(a+0x58,frame);if(s32(frame)>load<s32>(a+0x5C)){store<u32>(a+0x58,0);store<s32>(a+0x48,-1);}}
 frame=load<u32>(a+0x44);if(s32(frame)>=-1){++frame;store<u32>(a+0x44,frame);if(s32(frame)>load<s32>(a+0x2C))store<u32>(a+0x44,0);}
 frame=load<u32>(a+0x70);if(s32(frame)>=-1){++frame;store<u32>(a+0x70,frame);if(s32(frame)>load<s32>(a+0x74))store<u32>(a+0x70,0);}
 store<u32>(a+0x78,load<u32>(a+0x78)+1);return 1;
}
VERIFY(0x025CBB90,vib_Run);
