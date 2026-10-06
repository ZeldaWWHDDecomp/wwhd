// HD HUD head helpers.
#include "gabi.h"
using namespace gabi;
void meterShow(){WWHD_FUNC(0x0259169C,void);store<u8>(call<u32>(0x025200D4)+0x5C22,1);}
VERIFY(0x0259169C,meterShow);
void meterHide(){WWHD_FUNC(0x025916C4,void);store<u8>(call<u32>(0x025200D4)+0x5C22,0);}
VERIFY(0x025916C4,meterHide);
void meterFlagOn(){WWHD_FUNC(0x025916EC,void);store<u8>(0x101EA06C,1);}
VERIFY(0x025916EC,meterFlagOn);
void meterFlagOff(){WWHD_FUNC(0x025916FC,void);store<u8>(0x101EA06C,0);}
VERIFY(0x025916FC,meterFlagOff);
void meterMenuStatus(u32 status){WWHD_FUNC(0x0259170C,void,status);store<u8>(0x101EA069,status);}
VERIFY(0x0259170C,meterMenuStatus);
u32 meterBitTest(u32 p,u32 mask){WWHD_FUNC(0x02591718,u32,p,mask);return (load<u8>(p)&mask)!=0;}
VERIFY(0x02591718,meterBitTest);
void meterBitSet(u32 p,u32 mask){WWHD_FUNC(0x0259172C,void,p,mask);store<u8>(p,load<u8>(p)|mask);}
VERIFY(0x0259172C,meterBitSet);
void meterBitClear(u32 p,u32 mask){WWHD_FUNC(0x0259173C,void,p,mask);store<u8>(p,load<u8>(p)&~mask);}
VERIFY(0x0259173C,meterBitClear);
f32 meterFadeOut(u32 state,u32 counter){
 WWHD_FUNC(0x0259174C,f32,state,counter);
 s32 mode=load<s16>(state);f32 one=load<f32>(0x100512B4);
 if(mode==8){store<u16>(state,9);store<u16>(counter,1);return one;}
 f32 result=load<f32>(0x100512B8);
 if(mode==9){s32 phase=load<s16>(counter);if(phase<6){
  result=call<f32>(0x025DB6F4,6,phase,1);
  u16 next=(u16)(load<s16>(counter)+1);result=fsubs_ppc(one,result);store<u16>(counter,next);
 }else store<u16>(state,0);}
 return result;
}
VERIFY(0x0259174C,meterFadeOut);
f32 meterFadeIn(u32 state,u32 counter){
 WWHD_FUNC(0x025917FC,f32,state,counter);
 s32 mode=load<s16>(state);
 if(mode==0){f32 zero=load<f32>(0x100512B8);store<u16>(state,7);store<u16>(counter,0);return zero;}
 f32 result=load<f32>(0x100512B4);
 if(mode==7){s32 phase=load<s16>(counter);if(phase<6){
  result=call<f32>(0x025DB6F4,6,phase,1);store<u16>(counter,(u16)(load<s16>(counter)+1));
 }else store<u16>(state,8);}
 return result;
}
VERIFY(0x025917FC,meterFadeIn);
