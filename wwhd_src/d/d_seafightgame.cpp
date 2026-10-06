/* Local WWHD sea-fight minigame reconstruction. */
#include "wwhd.h"
s32 sea_check(void* self,s32 x,s32 y,s32 size,s32 direction) {
 WWHD_FUNC(0x025BB0E0,s32,self,x,y,size,direction);
 if(size<=0) return 1;
 u32 addr=gabi::ea(self)+(u32)x*8+(u32)y;
 if(direction==0) {
  if(x>7) return 0;
  for(s32 i=0;i<size;i++) {
   if(y>7 || gabi::load<u8>(addr)>100) return 0;
   addr++; y=(s32)((u32)y+1);
  }
 } else {
  if(y>7) return 0;
  for(s32 i=0;i<size;i++) {
   if(x>7 || gabi::load<u8>(addr)>100) return 0;
   addr+=8; x=(s32)((u32)x+1);
  }
 }
 return 1;
}
VERIFY(0x025BB0E0,sea_check);
s32 sea_put(void* self,u32 ship,u32 size) {
 WWHD_FUNC(0x025BB198,s32,self,ship,size);
 f32 gridScale=gabi::load<f32>(0x10054568),directionScale=gabi::load<f32>(0x1005456C);
 u32 x,y,direction;
 do {
  s16 dir=(s16)gabi::ftoi(gabi::call<f32>(0x020198D8,directionScale));
  direction=(u8)(dir%2);
  x=(u8)gabi::ftoi(gabi::call<f32>(0x020198D8,gridScale));
  y=(u8)gabi::ftoi(gabi::call<f32>(0x020198D8,gridScale));
 } while(!sea_check(self,x,y,(s32)size,direction));
 u32 addr=gabi::ea(self),record=addr+0x40+ship*15;
 if(direction==0) {
  for(u32 i=0;i<size && (s32)size>0;i++) {
   gabi::store<u8>(record+i*2,x);
   gabi::store<u8>(record+i*2+1,y+i);
   gabi::store<u8>(addr+x*8+y+i,ship+102);
  }
  gabi::store<u8>(record+0xD,0);
  gabi::store<u8>(record+0xB,x);
  gabi::store<u8>(record+0xC,y);
  gabi::store<u8>(record+0xE,size);
 } else {
  for(u32 i=0;i<size && (s32)size>0;i++) {
   gabi::store<u8>(record+i*2,x+i);
   gabi::store<u8>(record+i*2+1,y);
   gabi::store<u8>(addr+(x+i)*8+y,ship+102);
  }
  gabi::store<u8>(record+0xD,size);
  gabi::store<u8>(record+0xB,x);
  gabi::store<u8>(record+0xC,y);
  gabi::store<u8>(record+0xE,0);
 }
 gabi::store<u8>(record+8,size);
 gabi::store<u8>(record+9,size);
 return 1;
}
VERIFY(0x025BB198,sea_put);
s32 sea_init(void* self,s32 bullets,u32 scenario) {
 WWHD_FUNC(0x025BB358,s32,self,bullets,scenario);
 u32 addr=gabi::ea(self);
 for(u32 i=0;i<64;i++) gabi::store<u8>(addr+i,0);
 gabi::store<u8>(addr+0x7D,bullets);
 gabi::store<u8>(addr+0x7E,0);
 if(scenario==1) {
  gabi::store<u8>(addr+0x7C,1); sea_put(self,0,2);
 } else if(scenario==2) {
  gabi::store<u8>(addr+0x7C,2); sea_put(self,0,2); sea_put(self,1,3);
 } else if(scenario==3) {
  gabi::store<u8>(addr+0x7C,3); sea_put(self,0,2); sea_put(self,1,3); sea_put(self,2,4);
 }
 gabi::store<u32>(addr+0x80,0);
 return 1;
}
VERIFY(0x025BB358,sea_init);
s32 sea_attack(void* self,u32 x,u32 y) {
 WWHD_FUNC(0x025BB470,s32,self,x,y);
 u32 addr=gabi::ea(self),cell=addr+x*8+y;
 u32 value=gabi::load<u8>(cell);
 s32 result=-1;
 if(value==0) {
  gabi::store<u8>(cell,1);
 } else if(value>100) {
  u32 ship=value-102;
  if(ship>=4) {
   gabi::call(0x0273AA24,gabi::at<void>(0x10054570),0xAF,gabi::at<void>(0x10054584));
   return -1;
  }
  result=ship;
  u32 record=addr+ship*15;
  u8 remaining=gabi::load<u8>(record+0x49)-1;
  gabi::store<u8>(record+0x49,remaining);
  if(remaining==0) {
   u8 alive=gabi::load<u8>(addr+0x7C);
   u32 dead=gabi::load<u32>(addr+0x80);
   gabi::store<u8>(addr+0x7C,alive-1);
   gabi::store<u32>(addr+0x80,dead+1);
  }
  gabi::store<u8>(cell,3);
 } else return -2;
 u8 bullets=gabi::load<u8>(addr+0x7D),score=gabi::load<u8>(addr+0x7E);
 gabi::store<u8>(addr+0x7D,bullets-1);
 gabi::store<u8>(addr+0x7E,score+1);
 return result;
}
VERIFY(0x025BB470,sea_attack);
void sea_initializer() {
 WWHD_FUNC(0x025BB568,void);
 gabi::store<u32>(0x1047C978,0); gabi::store<u32>(0x1047C970,0);
 gabi::store<u32>(0x1047C97C,0); gabi::store<u32>(0x1047C974,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101EAE80));
 f32 zero=gabi::load<f32>(0x100545B4),one=gabi::load<f32>(0x100545B8);
 gabi::store<f32>(0x1047C964,zero); gabi::store<f32>(0x1047C968,one);
 gabi::call(0x028ED6F8,gabi::at<void>(0x1047C96C));
 gabi::call(0x028F026C,gabi::at<void>(0x101EAE8C));
 gabi::call(0x028EAB2C,gabi::at<void>(0x1047C96D));
 gabi::call(0x028F026C,gabi::at<void>(0x101EAE98));
}
VERIFY(0x025BB568,sea_initializer);
