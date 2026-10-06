// HD HUD prefix.
#include "gabi.h"
using namespace gabi;
static u32 meterWord(u32 p,u32 n=0){return load<u32>(p+n);}
static f32 meterFloat(u32 p,u32 n=0){return load<f32>(p+n);}
static u32 meterStageData(){
 u32 receiver=call<u32>(0x025200D4)+0x5150;
 return call_ptr<u32>(meterWord(meterWord(receiver),0x15C),receiver);
}

void meterRecollectBossData(){
 WWHD_FUNC(0x0259188C,void);
 if(((meterWord(meterStageData(),0xC)>>16)&7)!=3)return;
 if(!call<u32>(0x025B9100,meterWord(0x101F84DC)+0x798,3u))return;
 const u32 types[]={3,4,6,7};
 const u32 flags[]={0x3D80,0x3D40,0x3D20,0x3D10};
 for(u32 i=0;i<4;i++){
  if((load<u8>(meterStageData()+9)>>1)!=types[i])continue;
  if(call<u32>(0x025B8B94,meterWord(0x101F84DC)+0x644,flags[i]))continue;
  call<void>(0x025221F0);
  call<void>(0x025B8B68,meterWord(0x101F84DC)+0x644,flags[i]);
  return;
 }
}
VERIFY(0x0259188C,meterRecollectBossData);

void meterSetNowHeartScale(u32 pane){
 WWHD_FUNC(0x02591A3C,void,pane);
 f32 x=fmuls_ppc(meterFloat(pane,0x24),meterFloat(0x1047AD24));
 f32 height=meterFloat(pane,0x28);
 store<f32>(pane+0x2C,x);
 store<f32>(pane+0x30,fmuls_ppc(height,meterFloat(0x1047AD24)));
 call<void>(0x025DB614,pane);
}
VERIFY(0x02591A3C,meterSetNowHeartScale);

void meterSetHeartScale(u32 pane,s32 level){
 WWHD_FUNC(0x02591A64,void,pane,level);
 f32 width=meterFloat(pane,0x24),height=meterFloat(pane,0x28);
 f32 x,y;
 if(level==4){
  x=fmuls_ppc(width,meterFloat(0x101EA030));
  y=fmuls_ppc(height,meterFloat(0x101EA034));
 }else{
  f32 scale=meterFloat(0x100512B4);
  x=fmuls_ppc(width,scale);y=fmuls_ppc(height,scale);
 }
 store<f32>(pane+0x2C,x);store<f32>(pane+0x30,y);
 call<void>(0x025DB614,pane);
}
VERIFY(0x02591A64,meterSetHeartScale);

void meterHeartScaleCountdown(u32 index){
 WWHD_FUNC(0x02591AB4,void,index);
 u32 state=0x1047B03C+index;
 if(load<u8>(state+0x53)){
  u8 count=load<u8>(state+0x4C);
  if(count)store<u8>(state+0x4C,count-1);
  else store<u8>(state+0x53,0);
 }else store<u8>(state+0x45,1);
}
VERIFY(0x02591AB4,meterHeartScaleCountdown);

void meterMaxLifeCountdown(u32 index){
 WWHD_FUNC(0x02591B00,void,index);
 u32 state=0x1047B03C+index;
 if(!load<u8>(state+0x53)){
  u8 count=load<u8>(state+0x45);
  if(count)store<u8>(state+0x45,count-1);
  else store<u8>(state+0x53,1);
 }else store<u8>(state+0x4C,1);
}
VERIFY(0x02591B00,meterMaxLifeCountdown);

void meterClearMagicFlag(){
 WWHD_FUNC(0x02592878,void);
 store<u8>(0x1047B09B,0);
}
VERIFY(0x02592878,meterClearMagicFlag);

void meterMagicLength(u32 meter,f32 scale){
 WWHD_FUNC(0x02592888,void,meter,scale);
 f32 edge=fmuls_ppc(meterFloat(meter,0xF60),meterFloat(0x100512FC));
 f32 width=meterFloat(meter,0x1120);
 f32 delta=fsubs_ppc(width,edge);
 f32 half=meterFloat(0x100512C8);
 f32 value;
 if(delta!=delta)value=ppc_qnan(delta);
 else if(edge!=edge)value=ppc_qnan(edge);
 else if(scale!=scale)value=ppc_qnan(scale);
 else value=fmadds(edge,scale,delta);
 f32 left=fmadds(value,half,meterFloat(meter,0x1100));
 store<f32>(meter+0x1128,value);store<f32>(meter+0x1118,left);
 call<void>(0x025DB614,meter+0x10FC);
 f32 origin=meterFloat(meter,0x1120),length=meterFloat(meter,0x11C8);
 value=(f32)((f64)meterFloat(meter,0x1128)+(f64)fsubs_ppc(length,origin));
 left=fmadds(value,half,meterFloat(meter,0x11A8));
 store<f32>(meter+0x11D0,value);store<f32>(meter+0x11C0,left);
 call<void>(0x025DB614,meter+0x11A4);
 length=meterFloat(meter,0x1158);origin=meterFloat(meter,0x1120);
 value=(f32)((f64)meterFloat(meter,0x1128)+(f64)fsubs_ppc(length,origin));
 left=fmadds(value,half,meterFloat(meter,0x1138));
 store<f32>(meter+0x1160,value);store<f32>(meter+0x1150,left);
 call<void>(0x025DB614,meter+0x1134);
}
VERIFY(0x02592888,meterMagicLength);

s32 meterCountStep(s32 value){
 WWHD_FUNC(0x025931A8,s32,value);
 if(value>=200)return 100;
 if(value<50)return 1;
 return 10;
}
VERIFY(0x025931A8,meterCountStep);

void meterSetMagicPosition(u32 meter){
 WWHD_FUNC(0x025931CC,void,meter);
 u8 mode=load<u8>(meterWord(0x101F84DC)+0x32);
 f32 y=meterFloat(0x100512B8),x;
 if(!mode)x=fsubs_ppc(meterFloat(meter,0x1A30),meterFloat(meter,0x19F8));
 else x=y;
 call<void>(0x025DB648,meter+0x2A24,x,y);
 call<void>(0x025DB648,meter+0x2A5C,x,y);
}
VERIFY(0x025931CC,meterSetMagicPosition);

void meterMagicTransNowInit(u32 meter){
 WWHD_FUNC(0x0259295C,void,meter);
 u16 life=load<u16>(load<u32>(0x101F84DC)+0x20);
 f32 x=fsubs_ppc(meterFloat(meter,0x1118),meterFloat(meter,0x1110));
 f32 y=meterFloat(life<=43?0x10051300:0x100512B8);
 call<void>(0x025DB648,meter+0x10FC,x,y);
 x=fsubs_ppc(meterFloat(meter,0x11C0),meterFloat(meter,0x11B8));
 call<void>(0x025DB648,meter+0x11A4,x,y);
 x=fsubs_ppc(meterFloat(meter,0x11F8),meterFloat(meter,0x11F0));
 call<void>(0x025DB648,meter+0x11DC,x,y);
 for(u32 i=0;i<8;++i){u32 pane=meter+0xF3C+0x38*i;
  f32 origin=meterFloat(pane,0x14),position=meterFloat(pane,0x1C);
  call<void>(0x025DB648,pane,fsubs_ppc(position,origin),y);
 }
}
VERIFY(0x0259295C,meterMagicTransNowInit);

void meterMagicChange(u32 meter,f32 remaining){
 WWHD_FUNC(0x02592A88,void,meter,remaining);
 f32 ratio=(f32)((f64)remaining/(f64)meterFloat(meter,0xF60));
 s16 amount=(s16)ftoi(fmuls_ppc(ratio,meterFloat(0x10051304)));
 u32 state=call<u32>(0x025200D4);store<u16>(state+0x5B62,(u16)amount);
 call<void>(0x0259295C,meter);
 f32 one=meterFloat(0x100512B4),zero=meterFloat(0x100512B8);
 for(u32 i=0;i<8;++i){u32 pane=meter+0xF3C+0x38*i;
  if(!(remaining<meterFloat(pane,0x24))){
   call<void>(0x025DB654,pane,one);
   remaining=fsubs_ppc(remaining,meterFloat(pane,0x24));
  }else if(!(remaining>zero))call<void>(0x025DB654,pane,one);
  else remaining=zero;
 }
}
VERIFY(0x02592A88,meterMagicChange);

void meterMagicInitTrans(u32 meter){
 WWHD_FUNC(0x02592BC4,void,meter);
 u16 life=load<u16>(load<u32>(0x101F84DC)+0x20);
 f32 zero=meterFloat(0x100512B8);
 f32 y=life<=43?meterFloat(0x10051300):zero;
 call<void>(0x025DB648,meter+0x10FC,zero,y);
 call<void>(0x025DB648,meter+0x11A4,zero,y);
 call<void>(0x025DB648,meter+0x11DC,zero,y);
 for(u32 i=0;i<8;++i)call<void>(0x025DB648,meter+0xF3C+i*0x38,zero,y);
}
VERIFY(0x02592BC4,meterMagicInitTrans);

void meterPaneAlphaCurve(u32 pane,u32 thresholds,f32 scale){
 WWHD_FUNC(0x02593284,void,pane,thresholds,scale);
 s32 count=load<s16>(pane+0x36),first=load<s16>(thresholds);
 f32 alpha;
 s32 last;
 if(count<first){last=load<s16>(thresholds+8);alpha=meterFloat(0x100512B8);}
 else{
  s32 second=load<s16>(thresholds+2);f32 peak=meterFloat(0x10051314);
  if(count<second){f32 t=call<f32>(0x025DB6F4,second-first,count-first,0);
   count=load<s16>(pane+0x36);last=load<s16>(thresholds+8);alpha=fmuls_ppc(peak,t);
  }else{
   s32 third=load<s16>(thresholds+4);f32 slope=meterFloat(0x10051318);
   if(count<third){f32 t=call<f32>(0x025DB6F4,third-second,count-second,0);
    last=load<s16>(thresholds+8);count=load<s16>(pane+0x36);alpha=fnmsubs(slope,t,peak);
   }else{
    s32 fourth=load<s16>(thresholds+6);
    if(count<fourth){f32 t=call<f32>(0x025DB6F4,fourth-third,count-third,0);
     count=load<s16>(pane+0x36);last=load<s16>(thresholds+8);alpha=fmadds(slope,t,meterFloat(0x1005131C));
    }else{
     last=load<s16>(thresholds+8);f32 t=call<f32>(0x025DB6F4,last-fourth,count-fourth,0);
     last=load<s16>(thresholds+8);count=load<s16>(pane+0x36);alpha=fnmsubs(peak,t,peak);
    }
   }
  }
 }
 f32 result=fmuls_ppc(alpha,scale);
 store<u16>(pane+0x36,count>=last?1:(u16)(count+1));
 store<u8>(pane+0x35,(u8)ftoi(result));
}
VERIFY(0x02593284,meterPaneAlphaCurve);

void meterGaugeInitialPosition(u32 meter){
 WWHD_FUNC(0x02593434,void,meter);
 call<void>(0x02591AB4,4);call<u32>(0x025200D4);
 bool visible=load<u8>(0x101EBFD8)!=0;f32 zero=meterFloat(0x100512B8);
 store<f32>(meter+0x2F6C,visible?meterFloat(0x100512B4):zero);
 f32 lower=meterFloat(0x10051318),mana=meterFloat(load<u32>(0x101F84DC),0x44);
 bool mode=mana<lower || !(mana<meterFloat(0x10051320));
 f32 x=-fadds_ppc(meterFloat(meter,0x1950),meterFloat(meter,0x1970));
 if(mode){store<u16>(meter+0x3016,1);store<u8>(meter+0x302E,0);store<u16>(meter+0x132A,0);}
 else{store<u16>(meter+0x132A,0);store<u8>(meter+0x302E,0);store<u16>(meter+0x3016,0);}
 call<void>(0x025DB648,meter+0x12F4,x,zero);
 call<void>(0x025DB648,meter+0x194C,x,zero);store<u8>(meter+0x3024,1);
}
VERIFY(0x02593434,meterGaugeInitialPosition);

void meterItemUseCheck(u32 meter,u32 index){
 WWHD_FUNC(0x02591E60,void,meter,index);
 u32 state=call<u32>(0x025200D4);
 bool special=load<u8>(state+index+0x5BBB)==35;
 if(!special){state=call<u32>(0x025200D4);special=load<u8>(state+index+0x5BBB)==38;}
 if(special){call<void>(0x02720144,load<u32>(0x101F84DC)+0x12C0);call<void>(0x02720144,load<u32>(0x101F84DC)+0x12C0);return;}
 for(u8 item:{39,53,54,49}){state=call<u32>(0x025200D4);if(load<u8>(state+index+0x5BBB)==item)return;}
 u32 item=load<u8>(load<u32>(0x101F84DC)+index+0x29);
 if(item<24 || item<=31 || (u32)(item-36)>=8)return;
 call<u32>(0x025200D4);
}
VERIFY(0x02591E60,meterItemUseCheck);

void meterLoadItemTexture(u32 meter,u32 index){
 WWHD_FUNC(0x02591F68,void,meter,index);
 u32 state=call<u32>(0x025200D4);
 if(load<u8>(state+index+0x5BBB)!=255){
  u32 slot=load<u8>(load<u32>(0x101F84DC)+index+0x29),resource,buffer;
  u32 address=meter+0x2F4C+index*4;
  if(slot>=24){buffer=load<u32>(address);state=call<u32>(0x025200D4);
   u32 item=load<u8>(state+index+0x5BBB);resource=load<u32>(0x101E4674+item*36+4);
  }else{
   state=call<u32>(0x025200D4);u32 item=load<u8>(state+index+0x5BBB);
   if(item==53 || item==54)item=39;
   buffer=load<u32>(address);resource=load<u32>(0x101E4674+item*36+4);
  }
  state=call<u32>(0x025200D4);u32 archive=load<u32>(state+0x5A84);
  call<void>(0x027EAA2C,buffer,0xC00,0x54494D47,resource,archive);
  call<void>(0xC00088B8,load<u32>(address),0xC00);
 }
 call<void>(0x02591E60,meter,index);
}
VERIFY(0x02591F68,meterLoadItemTexture);

static f32 meterNegativeMultiplySubtract(f32 a,f32 b,f32 c){
 if(c!=c)return -ppc_qnan(c);
 volatile f64 product=(f64)a*(f64)b;
 volatile f64 difference=product-(f64)c;
 return (f32)-difference;
}

void meterMagicTransScale(u32 meter,f32 x,f32 y,f32 scale){
 WWHD_FUNC(0x02592CD8,void,meter,x,y,scale);
 if(load<u16>(load<u32>(0x101F84DC)+0x20)<=43)y=fadds_ppc(y,meterFloat(0x10051300));
 call<void>(0x025DB654,meter+0x11A4,scale);
 call<void>(0x025DB648,meter+0x11A4,x,y);
 f32 a=meterFloat(meter,0x1148),b=meterFloat(meter,0x114C);
 store<f32>(meter+0x1150,fmuls_ppc(a,scale));store<f32>(meter+0x1154,fmuls_ppc(b,scale));
 call<void>(0x025DB654,meter+0x1134,scale);
 a=meterFloat(meter,0x1110);f32 originX=meterFloat(meter,0x11B8),originY=meterFloat(meter,0x11BC);
 f32 dx=fsubs_ppc(a,originX);b=meterFloat(meter,0x1114);
 f32 nx=fadds_ppc(fmadds(dx,scale,originX),x);
 f32 ny=fadds_ppc(fmadds(fsubs_ppc(b,originY),scale,originY),y);
 store<f32>(meter+0x1118,nx);store<f32>(meter+0x111C,ny);call<void>(0x025DB654,meter+0x10FC,scale);
 a=meterFloat(meter,0x11F0);originX=meterFloat(meter,0x11B8);originY=meterFloat(meter,0x11BC);
 dx=fsubs_ppc(a,originX);b=meterFloat(meter,0x11F4);
 nx=fadds_ppc(fmadds(dx,scale,originX),x);ny=fadds_ppc(fmadds(fsubs_ppc(b,originY),scale,originY),y);
 store<f32>(meter+0x11F8,nx);store<f32>(meter+0x11FC,ny);call<void>(0x025DB654,meter+0x11DC,scale);
 a=meterFloat(meter,0x1180);b=meterFloat(meter,0x1184);
 store<f32>(meter+0x1188,fmuls_ppc(a,scale));store<f32>(meter+0x118C,fmuls_ppc(b,scale));call<void>(0x025DB654,meter+0x116C,scale);
 f32 half=meterFloat(0x100512C8);
 for(u32 i=0;i<8;++i){u32 pane=meter+0xF3C+i*0x38;
  a=meterFloat(pane,0x14);originX=meterFloat(meter,0x11B8);
  nx=fadds_ppc(fmadds(fsubs_ppc(a,originX),scale,originX),x);
  f32 width=meterFloat(pane,0x24);b=meterFloat(pane,0x18);f32 scaledWidth=fmuls_ppc(width,scale);
  store<f32>(pane+0x1C,nx);originY=meterFloat(meter,0x11BC);
  f32 dy=fsubs_ppc(b,originY),left=meterNegativeMultiplySubtract(scaledWidth,half,nx),offset=meterFloat(pane,0x2C);
  f32 py=fmadds(dy,scale,originY);nx=fmadds(offset,half,left);ny=fadds_ppc(py,y);
  store<f32>(pane+0x1C,nx);store<f32>(pane+0x20,ny);call<void>(0x025DB614,pane);
  a=meterFloat(pane,0x24);b=meterFloat(pane,0x28);store<f32>(pane+0x2C,a);store<f32>(pane+0x30,b);
 }
}
VERIFY(0x02592CD8,meterMagicTransScale);

void meterHeartInit(u32 meter){
 WWHD_FUNC(0x02591B4C,void,meter);
 store<f32>(0x101EA038,(f32)((f64)meterFloat(meter,0x668)/(f64)meterFloat(0x100512BC)));
 f32 half=meterFloat(0x100512C8),base=meterFloat(0x100512C4);
 store<f32>(0x101EA034,half);store<f32>(0x101EA030,base);
 store<f32>(0x101EA03C,fmuls_ppc(meterFloat(meter,0x66C),meterFloat(0x100512C0)));
 f32 zero=meterFloat(0x100512B8);store<f32>(meter+0x2F74,meterFloat(0x100512B4));store<f32>(meter+0x2FC0,zero);
 store<u8>(meter+0x3022,load<u16>(meterWord(0x101F84DC)+0x20));
 store<u8>(meter+0x3023,load<u16>(meterWord(0x101F84DC)+0x22));
 store<u16>(meter+0x3018,5);store<u16>(meter+0xF3A,0);call<void>(0x025DB6E0,meter+0xF04);
 u32 maximum=load<u8>(meter+0x3022)>>2;s32 current=((s32)load<u8>(meter+0x3023)-1)/4;
 for(u32 i=0;i<20;i++){
  u32 front=meter+0x644+i*0x38,back=meter+0xAA4+i*0x38;u32 count=load<u8>(meter+0x3023),level;
  if(maximum && count && (s32)i<=current)level=(s32)i==current?(count&3):0;
  else level=i<maximum?4:5;
  store<u16>(front+0x36,level);call<void>(0x02591A64,front,level);
  call<void>(0x02591A64,back,(s32)load<s16>(front+0x36));
  call<void>(0x025DB648,front,zero,zero);call<void>(0x025DB648,back,zero,zero);
  count=load<u8>(meter+0x3023);
  if(i==(u32)(((s32)count-1)/4)&&count){
   call<void>(0x02591A3C,front);call<void>(0x02591A3C,back);
   for(u32 offset:{0xCu,0x10u,0x2Cu,0x30u,0x1Cu,0x20u})store<u32>(meter+0xF04+offset,meterWord(front,offset));
  }
 }
 u32 count=load<u8>(meter+0x3023),state=call<u32>(0x025200D4);store<u16>(state+0x5BAC,count);
}
VERIFY(0x02591B4C,meterHeartInit);

struct MeterVectorStorage {be<f32> x,y,z;};
void meterFlyGaugeMove(u32 meter,u32 active,s32 maximum,s32 current){
 WWHD_FUNC(0x02592EE8,void,meter,active,maximum,current);
 f32 zero=meterFloat(0x100512B8),alpha;
 if(active){
  f32 ratio=(f32)((f64)(f32)current/(f64)(f32)maximum);
  f32 length=fmuls_ppc(fmuls_ppc(ratio,meterFloat(0x10051310)),meterFloat(0x100512FC));
  call<void>(0x02592BC4,meter);call<void>(0x02592A88,meter,length);
  if(!load<s16>(meter+0xF72)){store<u16>(meter+0x116A,0);store<u16>(meter+0xF72,1);store<u16>(meter+0xFAA,20);}
  call<void>(0x02591B00,1);s32 counter=load<s16>(meter+0x11DA);f32 one=meterFloat(0x100512B4);
  if(counter<5){counter=(s16)(counter+1);store<u16>(meter+0x11DA,counter);alpha=call<f32>(0x025DB6F4,5,counter,0);}
  else alpha=one;
  counter=load<s16>(meter+0x116A);
  if(counter<50){
   store<u16>(meter+0x116A,counter+1);u32 state=call<u32>(0x025200D4);Local<MeterVectorStorage> point;
   call<void>(0x025F0EA4,meterWord(state,0x5B38)+0x37C,point.get());
   store<f32>(0x1047B048,(f32)point->x);store<f32>(0x1047B04C,(f32)point->y);store<f32>(0x1047B050,(f32)point->z);
   counter=load<s16>(meter+0x116A);f32 half=meterFloat(0x100512C8),blend,fade;
   if(counter>30){blend=call<f32>(0x025DB6F4,20,50-counter,0);counter=load<s16>(meter+0x116A);fade=call<f32>(0x025DB6F4,20,counter-30,0);
    f32 x=fmuls_ppc(fsubs_ppc((f32)point->x,meterFloat(meter,0x11C0)),blend),y=fmuls_ppc(fsubs_ppc((f32)point->y,meterFloat(meter,0x11C4)),blend);
    call<void>(0x02592CD8,meter,x,y,fmadds(half,fade,half));
   }else{fade=zero;blend=one;f32 x=fsubs_ppc((f32)point->x,meterFloat(meter,0x11C0)),y=fsubs_ppc((f32)point->y,meterFloat(meter,0x11C4));call<void>(0x02592CD8,meter,x,y,half);}
   store<f32>(0x1047B06C,blend);store<f32>(0x1047B070,fade);store<f32>(meter+0x2FC4,alpha);return;
  }
 }else{
  call<void>(0x02591AB4,1);s32 counter=load<s16>(meter+0x11DA);
  if(counter>0){counter=(s16)(counter-1);store<u16>(meter+0x11DA,counter);alpha=call<f32>(0x025DB6F4,5,counter,0);}
  else{store<u16>(meter+0x11DA,0);alpha=zero;call<void>(0x0259295C,meter);}
  if(load<s16>(meter+0x116A))store<u16>(meter+0x116A,0);
 }
 store<f32>(meter+0x2FC4,alpha);
}
VERIFY(0x02592EE8,meterFlyGaugeMove);

struct MeterNameStorage {be<u32> text,vtable;};
struct MeterComparisonStorage {MeterNameStorage pairs[12];};
static bool meterSceneEqual(u32 left,u32 right){
 // The actual SafeString vtable1005107C+14 points to empty0259D1B4.
 call_ptr<void>(meterWord(meterWord(left,4),0x14),left);
 call_ptr<void>(meterWord(meterWord(left,4),0x14),left);u32 saved=meterWord(left);
 call_ptr<void>(meterWord(meterWord(right,4),0x14),right);
 if(saved==meterWord(right))return true;u32 a=meterWord(left),b=meterWord(right);
 for(u32 i=0;i<0x40001;i++){u8 x=load<u8>(a+i),y=load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;
}
u32 meterXYAlpha(u32 meter,s32 item){
 WWHD_FUNC(0x02592144,u32,meter,item);
 u32 flags=meterWord(meter,0x3008);
 if((flags&0x1000)&&item!=0x25)return 1;
 if((flags&0x80)&&item!=0x20)return 1;
 if(flags&0x20000)return 1;
 if(flags&0x400){u32 state=call<u32>(0x025200D4);
  if(load<u8>(state+0x5CEA)!=1){bool exempt=false;for(s32 x:{0x78,0x20,0x22,0x25,0x2D,0x21,0x23,0x26,0x2A,0x2C,0x83,0x82,0x27,0x35,0x36,0x31,0x2F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59})if(item==x)exempt=true;if(!exempt)return 1;}
 }
 u32 state=call<u32>(0x025200D4);
 if(load<u8>(state+0x5CEA)==1&&item!=0x78&&item!=0x20)return 1;
 u32 stage=meterStageData();Local<MeterComparisonStorage> comparisons;u32 pairs=comparisons.a;
 auto scene=[&](u32 slot,u32 name){u32 left=pairs+slot*8,right=pairs+48+slot*8;store<u32>(left,name);store<u32>(left+4,0x1005107Cu);u32 game=call<u32>(0x025200D4);store<u32>(right,game+0x5134);store<u32>(right+4,0x1005107Cu);return meterSceneEqual(left,right);};
 if(((meterWord(stage,0xC)>>16)&7)==3){bool boss=false;const u32 names[]={0x100512CCu,0x100512D4u,0x100512DCu,0x100512E4u};for(u32 i=0;i<4;i++)if(scene(i,names[i])){boss=true;break;}
  if(boss&&(item==0x56||item==0x58||item==0x59))return 1;
 }
 flags=meterWord(meter,0x3008);if(!(flags&2))return 0;
 if(item==0x21)return 0;
 if(item==0x22)return scene(4,0x100512ECu)?0:1;
 if(item==0x25)return scene(5,0x100512F4u)?0:1;
 for(s32 x:{0x23,0x26,0x24,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x1F,0x2C,0x82,0x83,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x30,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E})if(item==x)return 0;
 return 1;
}
VERIFY(0x02592144,meterXYAlpha);
