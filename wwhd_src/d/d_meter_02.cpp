// Qualified HD HUD trailing range.
#include "gabi.h"
using namespace gabi;
static u32 tailWord(u32 p,u32 n=0){return load<u32>(p+n);}
static f32 tailFloat(u32 p,u32 n=0){return load<f32>(p+n);}
static u32 tailStage(){u32 p=call<u32>(0x025200D4)+0x5150;return call_ptr<u32>(tailWord(tailWord(p),0x15C),p);}
struct TailNameStorage {be<u32> text,vtable;};
struct TailSceneStorage {TailNameStorage left,right;};
static bool tailSceneEquals(u32 name){
 Local<TailSceneStorage> pair;pair->left.text=name;pair->left.vtable=0x1005107Cu;
 u32 state=call<u32>(0x025200D4);pair->right.text=state+0x5134;pair->right.vtable=0x1005107Cu;
 u32 left=pair.a,right=pair.a+8;
 // Real1005107C+14 points to empty0259D1B4 and reads no argument registers.
 call_ptr<void>(tailWord(tailWord(left,4),0x14),left);call_ptr<void>(tailWord(tailWord(left,4),0x14),left);u32 old=tailWord(left);
 call_ptr<void>(tailWord(tailWord(right,4),0x14),right);if(old==tailWord(right))return true;
 u32 a=tailWord(left),b=tailWord(right);for(u32 i=0;i<0x40001;i++){u8 x=load<u8>(a+i),y=load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;
}
void meterTailCounter6(u32 meter){
 WWHD_FUNC(0x0259C0EC,void,meter);
 u32 flags=tailWord(meter,0x3008);bool hide=true;
 bool input=false;if(!(flags&0x4000)&&(flags&0x40)){u32 state=call<u32>(0x025200D4);if(load<u8>(state+0x5292)){state=call<u32>(0x025200D4);input=(load<u16>(state+0x52A6)&0x200)!=0;}}
 if(!(flags&0x4000)&&!input&&load<u8>(0x101EA073)!=4){
  flags=tailWord(meter,0x3008);
  if(!(flags&0x013E00A0)&&!load<u8>(0x101EA06C)&&!(flags&0x02C00000)){
   hide=tailSceneEquals(0x10051074u)&&load<u8>(call<u32>(0x025200D4)+0x5CEA)==6;
  }
 }
 if(hide||(tailWord(meter,0x3008)&0x18))call<void>(0x02591AB4,6);else call<void>(0x02591B00,6);
}
VERIFY(0x0259C0EC,meterTailCounter6);
void meterTailPaneAlpha(u32 meter){
 WWHD_FUNC(0x0259C2BC,void,meter);f32 hearts=tailFloat(meter,0x2FC0),magic=tailFloat(meter,0x2FC4);
 for(u32 i=0;i<20;i++){call<void>(0x025DB690,meter+0x644+i*0x38,hearts);call<void>(0x025DB690,meter+0xAA4+i*0x38,hearts);}
 store<u8>(meter+0xF39,(u8)ftoi(fmuls_ppc((f32)load<u8>(meter+0xF39),hearts)));
 for(u32 i=0;i<8;i++)call<void>(0x025DB690,meter+0xF3C+i*0x38,magic);
 for(u32 p:{0x10FCu,0x11A4u,0x11DCu})call<void>(0x025DB690,meter+p,magic);
}
VERIFY(0x0259C2BC,meterTailPaneAlpha);
void meterTailMagicVisibility(u32 meter){
 WWHD_FUNC(0x0259C3CC,void,meter);u32 visible=0;u32 state=call<u32>(0x025200D4);
 if(load<u8>(state+0x5BD1)&&!(tailWord(meter,0x3008)&0x00800038)){
  state=call<u32>(0x025200D4);if(load<u16>(state+0x5BAC)&&!call<u32>(0x027161BC))visible=1;
 }
 store<u8>(0x1047B097,visible);
}
VERIFY(0x0259C3CC,meterTailMagicVisibility);
void meterTailCounter4(u32 meter){
 WWHD_FUNC(0x0259CCA8,void,meter);u32 flags=tailWord(meter,0x3008);bool reset=(flags&0x4000)!=0;
 if(!reset){if(flags&0x40){u32 state=call<u32>(0x025200D4);if(load<u8>(state+0x5292)){state=call<u32>(0x025200D4);reset=(load<u16>(state+0x52A6)&0x100)!=0;}flags=tailWord(meter,0x3008);}
  reset=reset||(flags&0x02600000)||(flags&0x009201B8);
  if(!reset){u32 state=call<u32>(0x025200D4);reset=load<u8>(state+0x5CEA)==1;if(!reset){state=call<u32>(0x025200D4);reset=load<u8>(state+0x5CEA)==8;}}
 }
 if(reset){call<void>(0x02593434,meter);return;}
 if((tailWord(meter,0x3008)&0x400)&&(load<u8>(tailStage()+9)>>1)!=5)call<void>(0x02591B00,4);else call<void>(0x02591AB4,4);
}
VERIFY(0x0259CCA8,meterTailCounter4);
void meterTailCounter5(u32 meter){
 WWHD_FUNC(0x0259CDC4,void,meter);
 if(!tailWord(0x1047A9BC)){store<u32>(0x1047A9BC,1);store<u8>(0x1047A9C8,call<u32>(0x02526C9C));}
 if(!tailWord(0x1047A9C0))store<u32>(0x1047A9C0,1);
 u32 state=call<u32>(0x025200D4);bool shown=(tailWord(state,0x5CD8)&0x1000)!=0;
 if(shown){const u32 mask=0x002040C0u|0x00800120u|0x00120018u;shown=!(tailWord(meter,0x3008)&mask);if(shown)shown=load<u8>(call<u32>(0x025200D4)+0x5CEA)!=8;}
 if(shown)call<void>(0x02591B00,5);else call<void>(0x02591AB4,5);
}
VERIFY(0x0259CDC4,meterTailCounter5);
void meterTailPhotoState(u32 meter){
 WWHD_FUNC(0x0259CEA0,void,meter);u32 state=call<u32>(0x025200D4);
 if(load<u8>(state+0x5BEC))return;
 state=call<u32>(0x025200D4);call<void>(0x02540024,state+0x51D0);
 state=call<u32>(0x025200D4);u32 mode=load<u8>(state+0x5BE8);bool photo=mode==2;
 if(!photo){state=call<u32>(0x025200D4);photo=load<u8>(state+0x5BE8)==3;}
 if(photo){store<u16>(meter+0x3040,89);store<u8>(0x101EACB7,24);}
 u32 index=load<u16>(meter+0x3040);if((index==99||index==89||index==98)&&tailWord(meter,0x303C)==0xFFFFFFFF)store<u16>(meter+0x3040,0);
}
VERIFY(0x0259CEA0,meterTailPhotoState);

void meterTailRupeeMove(u32 meter){
 WWHD_FUNC(0x0259C454,void,meter);
 u32 state=tailWord(0x101F84DC);if((u32)(s32)load<s16>(meter+0x1A62)!=load<u8>(state+0x32)){call<void>(0x025931CC,meter);store<u16>(meter+0x1A62,load<u8>(tailWord(0x101F84DC)+0x32));}
 u32 game=call<u32>(0x025200D4);u32 pending;
 if(tailWord(game,0x5B48)){
  game=call<u32>(0x025200D4);state=tailWord(0x101F84DC);u32 delta=tailWord(game,0x5B48),mode=load<u8>(state+0x32);s32 total=(s32)((u32)load<u16>(state+0x24)+delta);
  u32 cap=mode==0?500:(mode==1?1000:5000),current=load<u16>(meter+0x300C);u32 value=total>(s32)cap?cap:(total<0?0:(u32)total);
  store<u32>(meter+0x3000,value-current);store<u16>(tailWord(0x101F84DC)+0x24,value);
  store<u32>(call<u32>(0x025200D4)+0x5B48,0);pending=tailWord(meter,0x3000);
  if(pending>=5)store<u8>(0x101EA076,1);
 }else pending=tailWord(meter,0x3000);
 if(pending){
  bool increase=(s32)pending>0;u32 step=call<u32>(0x025931A8,increase?pending:0u-pending);
  u32 current=load<u16>(meter+0x300C);pending=tailWord(meter,0x3000);
  current=increase?current+step:current-step;pending=increase?pending-step:pending+step;
  store<u16>(meter+0x300C,current);store<u32>(meter+0x3000,pending);
  if(load<u8>(0x101EA076)){
   if(!pending){call<void>(0x025E1988,increase?0x83Au:0x83Cu);store<u8>(0x101EA076,0);store<u8>(0x101EA075,0);}
   else if(!load<u8>(0x101EA075)){call<void>(0x025E1988,increase?0x839u:0x83Bu);store<u8>(0x101EA075,1);}
   else store<u8>(0x101EA075,0);
  }
  current=load<u16>(meter+0x300C);store<u16>(call<u32>(0x025200D4)+0x5BAE,current);
 }
 u32 flags=tailWord(meter,0x3008);bool hide=(flags&0x4000)!=0;
 if(!hide){if(flags&0x40){game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);hide=(load<u16>(game+0x52A6)&0x80)!=0;}flags=tailWord(meter,0x3008);}
  hide=hide||(flags&0x00F200B8);
  if(!hide){game=call<u32>(0x025200D4);hide=load<u8>(game+0x5CEA)==1;if(!hide){game=call<u32>(0x025200D4);hide=load<u8>(game+0x5CEA)==6;}}
 }
 call<void>(hide?0x02591AB4u:0x02591B00u,2);
 f32 alpha=call<f32>(hide?0x0259174Cu:0x025917FCu,meter+0x2A5A,meter+0x1A2A);
 u32 emitter=tailWord(meter,0x2F24);if(emitter){u32 bits=tailWord(emitter,0x254);store<u32>(emitter+0x254,hide?bits|4u:bits&~4u);}
 for(u32 i=0;i<4;i++){call<void>(0x025DB690,meter+0x19F4+i*0x38,alpha);call<void>(0x025DB690,meter+0x1B44+i*0x38,alpha);}
 call<void>(0x025DB690,meter+0x2A24,alpha);call<void>(0x025DB690,meter+0x2A5C,alpha);store<f32>(meter+0x3030,alpha);
}
VERIFY(0x0259C454,meterTailRupeeMove);

struct TailThresholdStorage {be<s16> values[5];};
void meterTailCounter3(u32 meter){
 WWHD_FUNC(0x0259C7FC,void,meter);
 if(load<u8>(tailStage()+9)&1){
  u32 game=call<u32>(0x025200D4);
  if(load<s16>(game+0x5B5C)){
   game=call<u32>(0x025200D4);u32 state=tailWord(0x101F84DC);s32 total=(s16)(load<u8>(state+0x7B8)+load<s16>(game+0x5B5C));u32 current=load<u8>(meter+0x3021);total=total>99?99:(total<0?0:total);
   store<u16>(meter+0x3012,(u32)total-current);store<u8>(tailWord(0x101F84DC)+0x7B8,total);store<u16>(call<u32>(0x025200D4)+0x5B5C,0);
  }
  s32 pending=load<s16>(meter+0x3012);
  if(pending>0){store<u16>(meter+0x3012,pending-1);store<u8>(meter+0x3021,load<u8>(meter+0x3021)+1);}
  else if(pending<0){store<u8>(meter+0x3021,load<u8>(meter+0x3021)-1);store<u16>(meter+0x3012,pending+1);}
 }
 Local<TailThresholdStorage> thresholds;const s16 values[]={30,40,45,50,58};for(u32 i=0;i<5;i++)thresholds->values[i]=values[i];
 u32 flags=tailWord(meter,0x3008);bool hide=(flags&0x4000)!=0;
 if(!hide){if(flags&0x40){u32 game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);hide=(load<u16>(game+0x52A6)&0x40)!=0;}flags=tailWord(meter,0x3008);}
  hide=hide||(flags&0x00F204B8);if(!hide)hide=!(load<u8>(tailStage()+9)&1);
 }
 call<void>(hide?0x02591AB4u:0x02591B00u,3);
 f32 alpha=call<f32>(hide?0x0259174Cu:0x025917FCu,meter+0x1CCA,meter+0x1B0A);
 for(u32 i=0;i<2;i++){call<void>(0x025DB690,meter+0x1AD4+i*0x38,alpha);call<void>(0x025DB690,meter+0x1C24+i*0x38,alpha);}
 call<void>(0x025DB690,meter+0x1C94,alpha);
 if(load<s16>(meter+0x19F2)==1){
  u32 config=0x1047AD14;f32 rnd=call<f32>(0x02019918,(f32)load<s16>(config+0x9A));s16 threshold=(s16)ftoi(fadds_ppc((f32)load<s16>(config+0x90),rnd));store<u16>(meter+0x2FDE,threshold);
  for(u32 i=0;i<3;i++){rnd=call<f32>(0x02019918,(f32)load<s16>(config+0x92+i*2));s32 base=threshold+load<s16>(config+0x88+i*2);threshold=(s16)ftoi(fadds_ppc((f32)base,rnd));store<u16>(meter+0x2FE0+i*2,threshold);}
  f32 base=(f32)(threshold+load<s16>(config+0x8E));rnd=call<f32>(0x02019918,(f32)load<s16>(config+0x98));store<u16>(meter+0x2FE6,(s16)ftoi(fadds_ppc(base,rnd)));
 }
 call<void>(0x02593284,meter+0x1984,thresholds.get(),alpha);call<void>(0x02593284,meter+0x19BC,meter+0x2FDE,alpha);store<f32>(meter+0x3034,alpha);
}
VERIFY(0x0259C7FC,meterTailCounter3);

void meterTailActionIndicator(u32 meter){
 WWHD_FUNC(0x0259A5B0,void,meter);
 u32 game=call<u32>(0x025200D4);if(load<u8>(game+0x5BB8)){u32 mode=load<u8>(call<u32>(0x025200D4)+0x5BB8);store<u8>(call<u32>(0x025200D4)+0x5BB5,mode);}
 if(load<s16>(meter+0x1D02)){
  game=call<u32>(0x025200D4);if(load<u8>(game+0x5BB5)){game=call<u32>(0x025200D4);if(load<u8>(game+0x5BB5)!=7)call<u32>(0x025200D4);call<u32>(0x025200D4);}
  u32 mode=load<u8>(call<u32>(0x025200D4)+0x5BB5);store<u16>(meter+0x1D02,0);store<u16>(meter+0x2712,1);store<u8>(meter+0x3020,mode);
 }
 game=call<u32>(0x025200D4);if(load<u8>(meter+0x3020)!=load<u8>(game+0x5BB5))store<u16>(meter+0x1D02,1);
 Local<be<u8>> state;*state.get()=load<u8>(0x1047B07A);u32 flags=tailWord(meter,0x3008),mode=0;bool hide=(flags&0x4000)!=0;
 if(!hide){if(flags&0x40){game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);hide=(load<u16>(game+0x52A6)&8)!=0;}flags=tailWord(meter,0x3008);}
  hide=hide||(flags&0x100);if(!hide){game=call<u32>(0x025200D4);hide=load<u8>(game+0x5BB5)==62;}
 }
 if(!hide){flags=tailWord(meter,0x3008);
  if(flags&0x20000){game=call<u32>(0x025200D4);hide=load<u8>(game+0x5BE8)==2;mode=2;}
  else if(flags&0x40000)mode=4;
  else{
   if(load<u8>(0x101EA069)){
    game=call<u32>(0x025200D4);
    if(load<u8>(game+0x5BE8)!=2){u32 m=load<u8>(0x101EA06A);hide=m==1||(load<u8>(0x101EA067)==1&&m==0);}
    if(!hide&&load<u8>(0x101EA069)){u32 m=load<u8>(0x101EA06A);hide=m==2||(load<u8>(0x101EA067)==2&&m==0);}
    flags=tailWord(meter,0x3008);
   }
   hide=hide||(flags&0x800000);
   if(!hide&&(flags&0x20)){hide=(load<u16>(tailStage()+0xA)&3)==1;flags=tailWord(meter,0x3008);}
   hide=hide||(flags&0x01780080);
   if(!hide&&(flags&0x20)){u32 value=load<u8>(0x1047B09F);if(value<=3)mode=3;else if(value>=7&&value<=10)mode=5;else hide=true;}
  }
 }
 if(hide)mode=1;
 if(load<s16>(0x101EA062)!=(s32)mode){call<void>(0x0259173C,meter+0x302A,8);if(mode==1)call<void>(0x0259173C,state.get(),8);}
 store<u16>(0x101EA062,mode);
 if(load<u8>(meter+0x302A)!=15){
  if(!call<u32>(0x02591718,meter+0x302A,8)){
   if(mode!=1)call<void>(0x0259172C,state.get(),8);
   s32 count=load<s16>(0x101EA060);if(count>=5){store<u16>(0x101EA060,5);call<void>(0x0259172C,meter+0x302A,8);}else store<u16>(0x101EA060,count+1);
  }
 }else{ s32 count=load<s16>(0x101EA060);store<u16>(0x101EA060,count>0?count-1:0); }
 if((u8)*state.get()!=load<u8>(0x1047B07A))store<u8>(0x1047B07A,(u8)*state.get());
 flags=tailWord(meter,0x3008);hide=(flags&0x4000)!=0;
 if(!hide){if(flags&0x40){game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);hide=(load<u16>(game+0x52A6)&8)!=0;}flags=tailWord(meter,0x3008);}
  hide=hide||(flags&0x01FC03B8);
 }
 s32 count=load<s16>(0x101EA064);
 if(hide){store<u16>(0x101EA064,count<5?count+1:5);store<u8>(meter+0x3038,load<u8>(meter+0x3038)|16);count=load<s16>(0x101EA064);}
 else if(flags&0xA000){count=count>3?(s16)(count-1):(count<3?(s16)(count+1):3);store<u16>(0x101EA064,count);}
 else{store<u16>(0x101EA064,count<5?count+1:5);store<u8>(meter+0x3028,load<u8>(meter+0x3028)|16);store<u8>(meter+0x3038,load<u8>(meter+0x3038)|16);count=load<s16>(0x101EA064);}
 f32 alpha=call<f32>(0x025DB6F4,5,count,0);game=call<u32>(0x025200D4);
 if(!load<u8>(game+0x5BB5))call<void>(0x025DB6E0,meter+0x1CCC);else call<void>(0x025DB690,meter+0x1CCC,alpha);
 if((tailWord(meter,0x3008)&0x20)&&!(load<u16>(tailStage()+0xA)&3))alpha=fsubs_ppc(tailFloat(0x100512B4),alpha);
 call<void>(0x025DB690,meter+0x26A4,alpha);call<void>(0x025DB690,meter+0x26DC,alpha);store<u8>(call<u32>(0x025200D4)+0x5BB8,0);
}
VERIFY(0x0259A5B0,meterTailActionIndicator);
void meterTailMagicGauge(u32 meter){
 WWHD_FUNC(0x0259B698,void,meter);
 if(tailWord(meter,0x3008)&0x40000){
  u32 player=tailWord(call<u32>(0x025200D4),0x5B38);Local<be<u16>> id;id->set(0x14E);
  if(player==call<u32>(0x025D5218,0x025E121C,id.a)){u32 active=load<u8>(0x101BDC2D);u32 maximum=call<u32>(0x022271B8);call<void>(0x02592EE8,meter,active,maximum,load<s16>(0x101BDC28));}
  else{player=tailWord(call<u32>(0x025200D4),0x5B38);id->set(0x16F);
   if(player==call<u32>(0x025D5218,0x025E121C,id.a)){u32 active=load<u8>(0x101D5F3E);u32 maximum=call<u32>(0x02526C94);call<void>(0x02592EE8,meter,active,maximum,load<s16>(0x101D5F38));}
   else call<void>(0x02592EE8,meter,0,1,1);
  }
  if(load<s16>(meter+0xFE2)==1)store<u16>(meter+0xFE2,0);store<u16>(meter+0x108A,1);return;
 }
 f32 divisor=tailFloat(0x10051348),length=tailFloat(0x100512B4);
 if(load<s16>(meter+0xF72)==1){store<u16>(meter+0xF72,0);f32 zero=tailFloat(0x100512B8);call<void>(0x02592CD8,meter,zero,zero,length);call<void>(0x02592BC4,meter);call<void>(0x02592A88,meter,((f32)load<u16>(meter+0x301A)/divisor));}
 u32 game=call<u32>(0x025200D4);
 if(load<s16>(game+0x5B64)){game=call<u32>(0x025200D4);u32 save=tailWord(0x101F84DC);s32 cap=(s16)(load<u8>(save+0x33)+load<s16>(game+0x5B64));if(cap>32)cap=32;store<u8>(save+0x33,cap);store<u16>(call<u32>(0x025200D4)+0x5B64,0);}
 u32 save=tailWord(0x101F84DC);u32 displayed=load<u8>(meter+0x301C),capacity=load<u8>(save+0x33);bool finalLength=false;
 if(displayed<capacity){displayed=(u8)(displayed+1);store<u8>(meter+0x301C,displayed);call<void>(0x02592888,meter,fmuls_ppc((f32)displayed,tailFloat(0x100512C0)));save=tailWord(0x101F84DC);capacity=load<u8>(save+0x33);finalLength=load<u8>(meter+0x301C)==capacity;}
 else if(displayed>capacity||load<s16>(meter+0xFE2)==0){if(displayed>capacity){displayed=(u8)(displayed-1);store<u8>(meter+0x301C,displayed);}call<void>(0x02592888,meter,fmuls_ppc((f32)displayed,tailFloat(0x100512C0)));save=tailWord(0x101F84DC);capacity=load<u8>(save+0x33);finalLength=load<u8>(meter+0x301C)==capacity;}
 if(finalLength){if(capacity<=16)length=tailFloat(0x100512C8);call<void>(0x02592888,meter,length);store<u16>(meter+0xFE2,1);save=tailWord(0x101F84DC);capacity=load<u8>(save+0x33);}
 if(capacity){game=call<u32>(0x025200D4);if(load<s16>(game+0x5B60)){game=call<u32>(0x025200D4);save=tailWord(0x101F84DC);s32 value=(s16)(load<u8>(save+0x34)+load<s16>(game+0x5B60));if(value<0)value=0;store<u8>(save+0x34,value);store<u16>(call<u32>(0x025200D4)+0x5B60,0);}save=tailWord(0x101F84DC);capacity=load<u8>(save+0x33);}
 if(load<u8>(save+0x34)>capacity)store<u8>(save+0x34,capacity);
 bool blocked=call<u32>(0x025DBE38)!=0;
 if(!blocked&&(tailWord(meter,0x3008)&0x40)){
  game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);if(load<u16>(game+0x52A6)&0x20){game=call<u32>(0x025200D4);if(call<u32>(0x025449B0,game+0x52C4)){game=call<u32>(0x025200D4);blocked=!(tailWord(game,0x5CDC)&0x2000);}}}
 }
 if(blocked)store<u16>(meter+0x108A,1);
 else{
  save=tailWord(0x101F84DC);f32 scaled=fmuls_ppc(fmuls_ppc((f32)load<u8>(save+0x34),tailFloat(meter,0xF60)),tailFloat(0x10051310));u32 desired=(u16)ftoi(scaled),shown=load<u16>(meter+0x301A);
  if(desired!=shown){
   if(desired>shown){shown=desired-shown<625?desired:(u16)(shown+625);store<u16>(meter+0x301A,shown);if(shown%1250==0){call<void>(0x025E1A04,0x87D,0,shown/1250);shown=load<u16>(meter+0x301A);}}
   else{if(shown-desired<625){store<u16>(meter+0x301A,0);call<void>(0x025E1988,0x87E);shown=load<u16>(meter+0x301A);}else{shown=(u16)(shown-625);store<u16>(meter+0x301A,shown);if(shown%1250==0){call<void>(0x025E1988,0x87E);shown=load<u16>(meter+0x301A);}}if(!shown){call<void>(0x025E1988,0x881);shown=load<u16>(meter+0x301A);}}
   call<void>(0x02592A88,meter,((f32)shown/divisor));store<u16>(meter+0x108A,1);
  }else if(load<s16>(meter+0x108A)){call<void>(0x02592A88,meter,((f32)shown/divisor));store<u16>(meter+0x108A,0);}
 }
 bool pulse=false;
 if(!(tailWord(meter,0x3008)&0x00800118)){
  game=call<u32>(0x025200D4);pulse=(load<u8>(game+0x5BEF)&1)!=0;
  if(!pulse){game=call<u32>(0x025200D4);pulse=call<u32>(0x0244363C,tailWord(game,0x5B34))!=0;}
  if(!pulse){game=call<u32>(0x025200D4);pulse=(tailWord(game,0x5CDC)&0x20)!=0;}
  if(!pulse){game=call<u32>(0x025200D4);if((tailWord(game,0x5CD8)&0x1000)&&call<u32>(0x02526C9C)){game=call<u32>(0x025200D4);u32 actor=tailWord(game,0x5B2C);pulse=call_ptr<s32>(tailWord(tailWord(actor,0xB4),0xAC),actor)!=-1;}}
 }
 if(pulse){if(!load<s16>(meter+0x1052))store<u16>(meter+0x1052,1);s32 timer=load<s16>(meter+0x101A);s32 value;
  if(timer>=29){store<u16>(meter+0x101A,0);value=0;}else{timer=(s16)(timer+1);store<u16>(meter+0x101A,timer);value=timer<15?timer:30-timer;}
  call<f32>(0x025DB6F4,15,value,0);game=call<u32>(0x025200D4);store<u8>(game+0x5BEF,load<u8>(game+0x5BEF)&0xFE);store<u8>(0x1047B098,1);
 }else{if(load<s16>(meter+0x1052)==1)store<u16>(meter+0x1052,0);store<u8>(0x1047B098,0);}
 save=tailWord(0x101F84DC);bool hide=load<u8>(save+0x33)==0;u32 flags=tailWord(meter,0x3008);
 if(!hide)hide=(flags&0x4000)!=0;
 if(!hide&&(flags&0x40)){game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);if(load<u16>(game+0x52A6)&0x20){game=call<u32>(0x025200D4);hide=!(tailWord(game,0x5CDC)&0x2000);}}}
 if(!hide)hide=load<u8>(0x101EA073)==4;
 if(!hide){flags=tailWord(meter,0x3008);hide=(flags&0x013A00A0)||load<u8>(0x101EA06C)||(flags&0x00C00000);}
 if(!hide)hide=tailSceneEquals(0x1005106Cu)&&load<u8>(call<u32>(0x025200D4)+0x5CEA)==6;
 s32 timer;
 if(hide){call<void>(0x02591AB4,1);timer=load<s16>(meter+0x1132);timer=timer>0?(s16)(timer-1):0;}
 else if(tailWord(meter,0x3008)&0x18){call<void>(0x02591AB4,1);timer=load<s16>(meter+0x1132);timer=timer>3?(s16)(timer-1):(timer<3?(s16)(timer+1):3);}
 else{call<void>(0x02591B00,1);timer=load<s16>(meter+0x1132);timer=timer<5?(s16)(timer+1):5;}
 store<u16>(meter+0x1132,timer);f32 alpha=call<f32>(0x025DB6F4,5,timer,0);store<f32>(meter+0x2FC4,alpha);if(load<s16>(meter+0x1212))store<u16>(meter+0x1212,0);
}
VERIFY(0x0259B698,meterTailMagicGauge);
struct TailVector {be<f32> x,y,z;};
void meterTailEnemy(u32 meter){
 WWHD_FUNC(0x0259ACD4,void,meter);
 u32 attention=call<u32>(0x025200D4)+0x5804,stage=tailStage();
 if(!stage)call<void>(0x0273AA24,0x10051130,0x1D6F,0x10051140);
 f32 zero=tailFloat(0x100512B8),one=tailFloat(0x100512B4);u32 event=load<u8>(0x101EA069);bool track=false;
 if(attention){
  if(!event){
   bool eligible=!load<u8>(call<u32>(0x025200D4)+0x5292);
   if(eligible)eligible=call<u32>(0x024EC8D0,attention,0)!=0;
   if(eligible){u32 a=call<u32>(0x024EC8D0,call<u32>(0x025200D4)+0x5804,0);eligible=(f32)load<s8>(a+0x3A0)>zero;}
   if(eligible)eligible=call<u32>(0x025B7A2C,tailWord(0x101F84DC)+0xD4,4,1)!=0;
   if(eligible)eligible=call<u32>(0x024EDFCC,attention)!=0;
   if(eligible)eligible=load<u8>(call<u32>(0x024EC8D0,attention,0)+0x2DA)==2;
   for(s16 excluded:{0xEA,0xEB,0xEE,0xD3,0xF3,0xF4,0xF5,0xF6}){if(!eligible)break;u32 actor=call<u32>(0x024EC8D0,attention,0);if(actor&&load<s16>(actor+8)==excluded)eligible=false;}
   if(eligible){u32 actor=call<u32>(0x024EC8D0,attention,0);if(actor&&load<s16>(actor+8)==0xF0){actor=call<u32>(0x024EC8D0,attention,0);if(!(tailWord(actor,0xB0)&15))eligible=false;}}
   if(eligible){u32 actor=call<u32>(0x024EC8D0,attention,0);if(actor&&load<s16>(actor+8)==0xD9)eligible=false;}
   if(eligible){u32 actor=call<u32>(0x024EC8D0,call<u32>(0x025200D4)+0x5804,0);Local<TailVector> position;call<void>(0x025F0EA4,actor+0x390,position.a);
    f32 ratio=(f32)load<s8>(actor+0x3A1)/(f32)load<s8>(actor+0x3A0);if(ratio<zero)ratio=zero;else if(ratio>one)ratio=one;
    store<f32>(0x1047B03C,position->x.get());store<f32>(0x1047B040,position->y.get());store<f32>(0x1047B044,position->z.get());event=load<u8>(0x101EA069);store<u8>(0x1047B09C,1);store<f32>(0x1047B068,ratio);
   }else{event=load<u8>(0x101EA069);store<u8>(0x1047B09C,0);}
  }else store<u8>(0x1047B09C,0);
 }
 if(!event&&call<u32>(0x025B7A2C,tailWord(0x101F84DC)+0xD4,4,1)){
  if(!load<u8>(call<u32>(0x025200D4)+0x5292)&&!(tailWord(meter,0x3008)&0x20080)){
   bool environment=((tailWord(stage,0xC)>>16)&7)==3;
   if(!environment){Local<be<u16>> name;name->set(0xEE);environment=call<u32>(0x025D5218,0x025E121C,name.a)!=0;}
   if(environment){
    if(call<u32>(0x025B9100,tailWord(0x101F84DC)+0x798,5)&&!call<u32>(0x025B9100,tailWord(0x101F84DC)+0x798,3))track=true;
    else{for(u32 name:{0x1005104Cu,0x10051054u,0x1005105Cu,0x10051064u})if(tailSceneEquals(name)){track=true;break;}}
   }
  }
 }
 if(!track){call<void>(0x02592878,meter);return;}
 u32 actor=0;
 for(u16 kind:{0xEA,0xEB,0xEE,0xF0,0xD9,0xD3}){Local<be<u16>> first;first->set(kind);actor=call<u32>(0x025D5218,0x025E121C,first.a);if(actor){Local<be<u16>> second;second->set(kind);actor=call<u32>(0x025D5218,0x025E121C,second.a);break;}}
 if(!actor){call<void>(0x02592878,meter);return;}
 s32 maximum=load<s8>(actor+0x3A0);f32 ratio;
 if(maximum){ratio=(f32)load<s8>(actor+0x3A1)/(f32)maximum;if(ratio<zero)ratio=zero;else if(ratio>one)ratio=one;}
 else ratio=tailFloat(0x1047AD20);
 store<f32>(0x1047B064,ratio);store<u8>(0x1047B09B,1);
}
VERIFY(0x0259ACD4,meterTailEnemy);
static void tailMaskBoth(u32 meter,u8 mask){u8 a=load<u8>(meter+0x3028),b=load<u8>(meter+0x3038);store<u8>(meter+0x3028,a|mask);store<u8>(meter+0x3038,b|mask);}
void meterTailBowLight(u32 meter){
 WWHD_FUNC(0x02599584,void,meter);
 u32 game=call<u32>(0x025200D4);if(load<u8>(game+0x5C2F)!=77){u32 previous=load<u8>(call<u32>(0x025200D4)+0x5C2F);store<u8>(call<u32>(0x025200D4)+0x5C2E,previous);}
 bool unchanged=true,particles=false;
 for(u32 i=0;i<3;i++){game=call<u32>(0x025200D4);if(load<u8>(meter+0x3025+i)!=load<u8>(game+0x5BBB+i)){
  call<void>(0x02591F68,meter,i);unchanged=false;game=call<u32>(0x025200D4);bool special=load<u8>(game+0x5BBB+i)==53;
  if(!special){game=call<u32>(0x025200D4);special=load<u8>(game+0x5BBB+i)==54;}if(!special)store<u16>(meter+0x235A+i*0x38,0);
  store<u8>(meter+0x3025+i,load<u8>(call<u32>(0x025200D4)+0x5BBB+i));
 }}
 if(unchanged){bool update=false;u32 save;
  if(!tailWord(0x1047A9B4)){save=tailWord(0x101F84DC);store<u32>(0x1047A9B4,1);store<u8>(0x1047A9C6,load<u8>(save+0x8F));}
  if(!tailWord(0x1047A9B8)){save=tailWord(0x101F84DC);store<u32>(0x1047A9B8,1);store<u8>(0x1047A9C7,load<u8>(save+0x90));}
  game=call<u32>(0x025200D4);if(load<s16>(game+0x5B66)){u32 block=call<u32>(0x02720144,tailWord(0x101F84DC)+0x12C0);s32 value=load<u8>(block+0x3C030C);value=(s16)(value+load<s16>(call<u32>(0x025200D4)+0x5B66));store<u16>(call<u32>(0x025200D4)+0x5B66,0);save=tailWord(0x101F84DC);if(value<0)value=0;if(value>12)value=12;block=call<u32>(0x02720144,save+0x12C0);call<void>(0x027262B0,block,(u8)value);update=true;}
  bool obtained=call<u32>(0x02520C0C,39)!=0;if(!obtained)obtained=call<u32>(0x02520C0C,53)!=0;if(!obtained)obtained=call<u32>(0x02520C0C,54)!=0;
  bool change=false;if(obtained)change=load<s16>(call<u32>(0x025200D4)+0x5B68)!=0;
  if(!change){save=tailWord(0x101F84DC);change=load<u8>(0x1047A9C6)!=load<u8>(save+0x8F);}
  if(change){if(load<u8>(0x1047AD76))store<u16>(call<u32>(0x025200D4)+0x5B68,0);game=call<u32>(0x025200D4);save=tailWord(0x101F84DC);s32 value=(s16)(load<u8>(save+0x89)+load<s16>(game+0x5B68));store<u16>(call<u32>(0x025200D4)+0x5B68,0);save=tailWord(0x101F84DC);u32 cap=load<u8>(save+0x8F);if(value<0)value=0;if(value>(s32)cap)value=cap;store<u8>(save+0x89,value);store<u8>(0x1047A9C6,load<u8>(tailWord(0x101F84DC)+0x8F));update=true;}
  change=false;if(call<u32>(0x02520C0C,49))change=load<s16>(call<u32>(0x025200D4)+0x5B6C)!=0;
  if(!change){save=tailWord(0x101F84DC);change=load<u8>(0x1047A9C7)!=load<u8>(save+0x90);}
  if(change){if(load<u8>(0x1047AD77))store<u16>(call<u32>(0x025200D4)+0x5B6C,0);game=call<u32>(0x025200D4);save=tailWord(0x101F84DC);s32 value=(s16)(load<u8>(save+0x8A)+load<s16>(game+0x5B6C));store<u16>(call<u32>(0x025200D4)+0x5B6C,0);save=tailWord(0x101F84DC);u32 cap=load<u8>(save+0x90);if(value<0)value=0;if(value>(s32)cap)value=cap;store<u8>(save+0x8A,value);store<u8>(0x1047A9C7,load<u8>(tailWord(0x101F84DC)+0x90));update=true;}
  for(u32 i=0;i<8;i++){game=call<u32>(0x025200D4);if(load<s16>(game+0x5B70+2*i)){
   game=call<u32>(0x025200D4);save=tailWord(0x101F84DC);s32 value=(s16)(load<u8>(save+0xBC+i)+load<s16>(game+0x5B70+2*i));store<u16>(call<u32>(0x025200D4)+0x5B70+2*i,0);
   if(value<=0){store<u8>(tailWord(0x101F84DC)+0xBC+i,0);for(u32 j=0;j<3;j++){call<u32>(0x025200D4);call<u32>(0x025DB5AC,24+i);}u32 item=call<u32>(0x025DB5AC,24+i);call<void>(0x025B6308,tailWord(0x101F84DC)+0x96,item);}
   else{save=tailWord(0x101F84DC);if(value>99)value=99;store<u8>(save+0xBC+i,value);}update=true;
  }save=tailWord(0x101F84DC);u8 cap=load<u8>(save+0xC4+i);if(load<u8>(0x1047A98C+i)!=cap){store<u8>(0x1047A98C+i,cap);update=true;}}
  for(u32 i=0;i<3;i++)if(load<u8>(call<u32>(0x025200D4)+0x5BBB+i)==130)update=true;
  if(update)for(u32 i=0;i<3;i++)call<void>(0x02591E60,meter,i);
 }
 Local<be<u8>> state;state->set(load<u8>(0x1047B07A));u32 flags=tailWord(meter,0x3008),mode=0;bool hide=(flags&0x4000)!=0;
 if(!hide&&(flags&0x40)){game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);hide=(load<u16>(game+0x52A6)&1)!=0;}flags=tailWord(meter,0x3008);}
 if(!hide)hide=(flags&0x100)!=0;
 if(!hide&&load<u8>(0x101EA069)){u32 action=load<u8>(0x101EA06A);hide=action==2||(load<u8>(0x101EA067)==2&&action==0);}
 if(!hide)hide=(flags&0x800000)!=0;
 if(!hide&&(flags&0x20)){hide=(load<u16>(tailStage()+0xA)&3)==1;flags=tailWord(meter,0x3008);}
 if(!hide)hide=(flags&0x00300080)!=0;
 if(!hide&&(flags&0x20000)){hide=load<u8>(call<u32>(0x025200D4)+0x5BE8)==2;flags=tailWord(meter,0x3008);}
 if(!hide)hide=(flags&0x004C0000)!=0;
 if(!hide){bool eventMode=false;if(load<u8>(0x101EA069)){u32 action=load<u8>(0x101EA06A);eventMode=action==1||(load<u8>(0x101EA067)==1&&action==0);}
  if(eventMode){if(load<u8>(0x101EA072))hide=true;else mode=2;}
  else if(flags&0x20){u32 item=load<u8>(0x1047B09F);if(item<=3)mode=3;else if(item>=7&&item<=10)mode=4;else hide=true;}
 }
 if(hide)mode=1;
 if(load<s16>(0x101EA054)!=(s32)mode){call<void>(0x0259173C,meter+0x302A,4);if(mode==1)call<void>(0x0259173C,state.a,4);}store<u16>(0x101EA054,mode);
 bool paneDone=load<u8>(meter+0x302A)==15;
 if(!paneDone&&!call<u32>(0x02591718,meter+0x302A,4)){
  if(mode!=1)call<void>(0x0259172C,state.a,4);s32 count=load<s16>(0x101EA052);if(count>=5){store<u16>(0x101EA052,5);call<void>(0x0259172C,meter+0x302A,4);}else store<u16>(0x101EA052,count+1);
 }else if(paneDone){ s32 count=load<s16>(0x101EA052);store<u16>(0x101EA052,count>0?count-1:0);}
 if(state->get()!=load<u8>(0x1047B07A))store<u8>(0x1047B07A,state->get());
 f32 zero=tailFloat(0x100512B8),height=tailFloat(0x10051500),width=tailFloat(0x10051504),one=tailFloat(0x100512B4),minimum=tailFloat(0x10051508),pulseDepth=tailFloat(0x1005150C);
 for(u32 i=0;i<3;i++){
  game=call<u32>(0x025200D4);bool bow=load<u8>(game+0x5BBB+i)==53;if(!bow)bow=load<u8>(call<u32>(0x025200D4)+0x5BBB+i)==54;
  if(bow){store<u16>(meter+0x235A,load<s16>(meter+0x235A)+1);u32 pane=meter+0x2324;u32 pulsePane=pane+i*0x38;s32 timer=load<s16>(pulsePane+0x36),phase;
   if(timer>=120){store<u16>(pulsePane+0x36,0);phase=0;}else phase=timer<60?timer:120-timer;
   f32 weight=call<f32>(0x025DB6F4,60,phase,0),base=(f32)load<u8>(pane+0x34);u8 alpha=(u8)ftoi(fnmsubs(fsubs_ppc(base,minimum),weight,base));f32 scale=fnmsubs(pulseDepth,weight,one);
   for(u32 j=0;j<3;j++){u32 p=pulsePane+j*0xA8;call<void>(0x025DB654,p,scale);store<u8>(p+0x35,alpha);}
  }else if(load<u8>(call<u32>(0x025200D4)+0x5BBB+i)==89){Local<TailVector> position;position->x=fsubs_ppc(tailFloat(meter,0x20A0+i*0x38),width);position->y=fsubs_ppc(tailFloat(meter,0x20A4+i*0x38),height);position->z=zero;particles=true;
   for(u32 j=0;j<2;j++){u32 slot=meter+0x2F30+j*4,emitter=tailWord(slot);if(emitter){f32 y=position->y.get();if(load<u8>(emitter+0x262)>=7)y=-y;store<f32>(emitter+0x22C,position->x.get());store<f32>(emitter+0x230,y);store<f32>(emitter+0x234,position->z.get());emitter=tailWord(slot);store<u32>(emitter+0x254,tailWord(emitter,0x254)&~4u);emitter=tailWord(slot);store<u8>(emitter+0x247,load<u8>(meter+0x2084+i*0x38+0x35));}
    else{game=call<u32>(0x025200D4);u32 p=call<u32>(0x025A847C,tailWord(game,0x5AB0),9,0x2D,position.a,0,0,255,0,-1,0,0,0);store<u32>(slot,p);}
   }
  }
 }
 if(!particles)for(u32 j=0;j<2;j++){u32 slot=meter+0x2F30+j*4,emitter=tailWord(slot);if(emitter){store<u32>(emitter+0x5C,0xFFFFFFFF);store<u32>(emitter+0x254,tailWord(emitter,0x254)|1);emitter=tailWord(slot);store<u32>(emitter+0x254,tailWord(emitter,0x254)&~0x40u);store<u32>(slot,0);}}
 flags=tailWord(meter,0x3008);hide=(flags&0x4000)!=0;
 if(!hide&&(flags&0x40)){game=call<u32>(0x025200D4);if(load<u8>(game+0x5292)){game=call<u32>(0x025200D4);hide=(load<u16>(game+0x52A6)&1)!=0;}flags=tailWord(meter,0x3008);}
 if(!hide)hide=(flags&0x00800110)!=0;
 if(!hide&&(flags&0x20)){hide=(load<u16>(tailStage()+0xA)&3)==1;flags=tailWord(meter,0x3008);}
 if(!hide)hide=(flags&0x007C0080)!=0;
 if(!hide){
  if(flags&8){for(u32 i=0;i<3;i++){u32 p=0x101EA058+i*2;s32 count=load<s16>(p);store<u16>(p,count<5?count+1:5);store<u8>(meter+0x3038,load<u8>(meter+0x3038)|load<u8>(0x10051510+i));}store<u8>(meter+0x3038,load<u8>(meter+0x3038)|0x40);}
  else if(flags&0x1AA00){for(u32 i=0;i<3;i++){u32 p=0x101EA058+i*2;s32 count=load<s16>(p);store<u16>(p,count>3?count-1:(count<3?count+1:3));}}
  else{
   bool stageZero=false;if(flags&0x20){stageZero=!(load<u16>(tailStage()+0xA)&3);if(!stageZero)flags=tailWord(meter,0x3008);}
   if(stageZero){s32 count=load<s16>(0x101EA058);store<u16>(0x101EA058,count>0?count-1:0);count=load<s16>(0x101EA05A);store<u16>(0x101EA05A,count<5?count+1:5);tailMaskBoth(meter,4);count=load<s16>(0x101EA05C);store<u16>(0x101EA05C,count>0?count-1:0);}
   else if(flags&0x01000000)tailMaskBoth(meter,0x20);
   else if(flags&0x20000)tailMaskBoth(meter,4);
   else{for(u32 i=0;i<3;i++){u32 item=load<u8>(call<u32>(0x025200D4)+0x5BBB+i);bool dim=call<u32>(0x02592144,meter,item)!=0;u32 p=0x101EA058+i*2;s32 count=load<s16>(p);if(dim)store<u16>(p,count>3?count-1:(count<3?count+1:3));else{store<u16>(p,count<5?count+1:5);tailMaskBoth(meter,i==0?4:(i==1?8:0x20));}}
    if(!call<u32>(0x02592144,meter,0x22))tailMaskBoth(meter,0x40);
   }
  }
 }
 store<u8>(call<u32>(0x025200D4)+0x5C2F,77);
}
VERIFY(0x02599584,meterTailBowLight);
