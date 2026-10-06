/* HD event data: full26-entry scope40508..429A3; neighboring initializers excluded. */
#include "bindings.h"
namespace d_event_data_cpp {
static u32 ld(u32 a){return gabi::load<u32>(a);} static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static u32 play(){return gabi::call<u32>(0x025200D4);}
static u32 data(u32 staff,u32 name,u32 type){u32 p=play();return gabi::call<u32>(0x0254487C,p+0x52C4,staff,name,type);}
static void cutEnd(u32 staff){u32 p=play();gabi::call(0x02543280,p+0x52C4,staff);}
static u32 flagMax(u32 self,s32 flag){
 WWHD_FUNC(0x02540778,u32,self,flag);
 if(flag==-1)return 1;
 if(flag>=10240){gabi::call(0x0273AA24,0x1004D908u,0xE7u,0x1004D8F4u);return 1;}return 0;
}
VERIFY(0x02540778,flagMax);
static u32 flagCheck(u32 self,u32 flag){
 WWHD_FUNC(0x025407D4,u32,self,flag);
 if(gabi::call<u32>(0x02540778,self,flag))return 0;
 return (ld(self+((flag>>5)*4))>>(flag&31))&1;
}
VERIFY(0x025407D4,flagCheck);
static u32 flagSet(u32 self,u32 flag){
 WWHD_FUNC(0x02540848,u32,self,flag);
 if(gabi::call<u32>(0x02540778,self,flag))return 0;
 u32 word=flag>>5;if(word>=320)return 0;
 st(self+word*4,ld(self+word*4)|(1u<<(flag&31)));return 1;
}
VERIFY(0x02540848,flagSet);
static void flagInit(u32 self){
 WWHD_FUNC(0x025408CC,void,self);
 for(u32 i=0;i<320;++i)st(self+4*i,0);
}
VERIFY(0x025408CC,flagInit);
static u32 startCheck(u32 self){
 WWHD_FUNC(0x025408E8,u32,self);
 for(u32 i=0;i<3;++i){u32 flag=ld(self+0x28+4*i);if(flag==0xFFFFFFFF)return i?1:0xFFFFFFFF;
  if(!gabi::call<u32>(0x025407D4,play()+0x5300,flag))return 0;
 }return 1;
}
VERIFY(0x025408E8,startCheck);
static void staffInit(u32 self){
 WWHD_FUNC(0x025409B4,void,self);
 u32 type=ld(self+0x2C);gabi::store<u8>(self+0x47,0);st(self+0x3C,0xFFFFFFFF);
 gabi::store<u16>(self+0x42,0);gabi::store<u8>(self+0x45,0);gabi::store<u16>(self+0x40,0);
 u32 first=ld(self+0x30);gabi::store<u8>(self+0x44,0);gabi::store<u8>(self+0x46,2);st(self+0x38,first);
 if(type==2)st(play()+0x52E4,1);
}
VERIFY(0x025409B4,staffInit);
static void advance(u32 self,u32 cut){
 WWHD_FUNC(0x02540A18,void,self,cut);
 st(self+0x38,cut);gabi::store<u8>(self+0x45,0);st(self+0x3C,0xFFFFFFFF);
 gabi::store<u8>(self+0x47,0);gabi::store<u8>(self+0x46,1);gabi::store<u16>(self+0x42,0);
 gabi::store<u16>(self+0x40,0);gabi::store<u8>(self+0x44,0);
}
VERIFY(0x02540A18,advance);
static void waitStart(u32 self,u32 staff){
 WWHD_FUNC(0x02540A48,void,self,staff);
 u32 timer=data(staff,0x1004D91C,3);gabi::store<u16>(self+0x42,timer?gabi::load<u16>(timer+2):0);
}
VERIFY(0x02540A48,waitStart);
static void waitProc(u32 self,u32 staff){
 WWHD_FUNC(0x02540AAC,void,self,staff);
 s16 timer=gabi::load<s16>(self+0x42);if(timer>0)gabi::store<u16>(self+0x42,(u16)(timer-1));else cutEnd(staff);
}
VERIFY(0x02540AAC,waitProc);
static u32 finishCheck(u32 self){
 WWHD_FUNC(0x02542624,u32,self);
 for(u32 i=0;i<3;++i){u32 flag=ld(self+0x88+4*i);if(flag==0xFFFFFFFF)return 1;
 if(!gabi::call<u32>(0x025407D4,play()+0x5300,flag))return 0;}return 1;
}
VERIFY(0x02542624,finishCheck);
static void specialStaff(u32 self,u32 staff){
 WWHD_FUNC(0x025426C4,void,self,staff);
 for(s32 i=0;i<(s32)ld(self+0x7C);++i){u32 idx=ld(self+0x2C+4*(u32)i);gabi::call(0x025424B8,staff+idx*80);}
}
VERIFY(0x025426C4,specialStaff);
static void baseInit(u32 self){
 WWHD_FUNC(0x0254273C,void,self);
 st(self+0x14,0);st(self+0x10,0);st(self+0x1C,0);st(self+0x18,0);st(self+4,0);st(self,0);st(self+0xC,0);st(self+8,0);
}
VERIFY(0x0254273C,baseInit);
static u32 advanceLocal(u32 self,u32 staff){
 WWHD_FUNC(0x02542764,u32,self,staff);
 u32 cuts=ld(self+0xC);u32 cut=cuts+ld(staff+0x38)*80;u32 next=ld(cut+0x3C);
 if(next!=0xFFFFFFFF){u32 status=gabi::call<u32>(0x025408E8,cuts+next*80);
  if(status==0xFFFFFFFF){u32 flag=ld(cut+0x34);if(gabi::call<u32>(0x025407D4,play()+0x5300,flag)){gabi::call(0x02540A18,staff,ld(cut+0x3C));return 1;}}
  else if(status==1){u32 flag=ld(cut+0x34);gabi::call(0x02540848,play()+0x5300,flag);gabi::call(0x02540A18,staff,ld(cut+0x3C));return 1;}
 }
 gabi::store<u8>(staff+0x46,gabi::load<u8>(staff+0x46)>1?1:0);return 0;
}
VERIFY(0x02542764,advanceLocal);
static void baseAdvance(u32 self,u32 evt){
 WWHD_FUNC(0x02542878,void,self,evt);
 for(s32 i=0;i<(s32)ld(evt+0x7C);++i){u32 idx=ld(evt+0x2C+4*(u32)i);u32 staff=ld(self+8);gabi::call(0x02542764,self,staff+idx*80);}
}
VERIFY(0x02542878,baseAdvance);
static void initialize(){
 WWHD_FUNC(0x025428F8,void,(u32)0);
 st(0x104758EC,0);st(0x104758E4,0);st(0x104758F0,0);st(0x104758E8,0);
 gabi::call(0x028F026C,0x101D6380u);
 gabi::store<f32>(0x104758D8,gabi::load<f32>(0x1004DCE0));gabi::store<f32>(0x104758DC,gabi::load<f32>(0x1004DCE4));
 gabi::call(0x028ED6F8,0x104758E0u);gabi::call(0x028F026C,0x101D638Cu);gabi::call(0x028EAB2C,0x104758E1u);gabi::call(0x028F026C,0x101D6398u);
}
VERIFY(0x025428F8,initialize);
static void packageDestructor(u32 self,u32 flags){
 WWHD_FUNC(0x0254298C,void,self,flags);
 if(self&&(flags&1))gabi::call(0x0273AF40,self);
}
VERIFY(0x0254298C,packageDestructor);
static void packageMessage(){WWHD_FUNC(0x025429A0,void,(u32)0);}
VERIFY(0x025429A0,packageMessage);
static bool stringEq(u32 a,u32 b){
 for(u32 i=0;;++i){u8 x=gabi::load<u8>(a+i),y=gabi::load<u8>(b+i);if(x!=y)return false;if(!x)return true;}
}
static u32 nextStage(u32 staff,u32 wipe){
 WWHD_FUNC(0x02540508,u32,staff,wipe);
 u32 room=0,layer=0xFFFFFFFF,mode=0;
 u32 stage=data(staff,0x1004D888,4);if(!stage)return 0;
 u32 start=data(staff,0x1004D8D4,3);
 u32 p=data(staff,0x1004D8A4,3);if(p)room=ld(p);
 p=data(staff,0x1004D890,3);if(p)layer=ld(p);
 u32 hour=data(staff,0x1004D8AC,0);
 p=data(staff,0x1004D8B4,3);if(p)mode=ld(p);
 p=data(staff,0x1004D8BC,3);if(p)wipe=ld(p);
 if(!start){gabi::call(0x0273AA24,0x1004D8E0u,0xA3u,0x1004D8A0u);return 1;}
 if(hour){f32 h=gabi::load<f32>(hour);f32 k=gabi::load<f32>(0x1004D880);gabi::call(0x02560728,gabi::fmuls_ppc(k,h));}
 f32 zero=gabi::load<f32>(0x1004D884);
 if(stringEq(0x1004D8C4,stage))gabi::call(0x0252012C,0x1004D8CCu,0u,0u,8u,0u,1u,0u,zero);
 else if(stringEq(0x1004D898,stage))gabi::call(0x0252012C,0x1004D8C4u,0u,0u,8u,0u,1u,0u,zero);
 else gabi::call(0x0252012C,stage,(s32)gabi::load<s16>(start+2),(s32)(s8)room,(s32)(s8)layer,mode,1u,(s32)(s8)wipe,zero);
 return 1;
}
VERIFY(0x02540508,nextStage);
struct StringRef {be<u32> text,vt;};
static bool boundedStringEq(u32 a,u32 b){
 for(u32 i=0;i<0x40001;++i){u8 x=gabi::load<u8>(a+i),y=gabi::load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;
}
static void package(u32 self){
 WWHD_FUNC(0x02540B0C,void,self);
 u32 staff=gabi::call<u32>(0x02542D88,play()+0x52C4,0x1004D944u,0u,0u);
 if(staff==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004D994u,0x412u,0x1004D94Cu);return;}
 u32 action=gabi::call<u32>(0x02542EDC,play()+0x52C4,staff,0x101D62C4u,3u,0u,0u);
 u32 advanceFlag=gabi::call<u32>(0x025447C8,play()+0x52C4,staff);
 f32 zero=gabi::load<f32>(0x1004D884);
 if(advanceFlag){
  if(action==0)gabi::call(0x02540A48,self,staff);
  else if(action==1||action==2){
   u32 filename=data(staff,0x1004D9A8,4);if(!filename)gabi::call(0x0273AA24,0x1004D994u,0x422u,0x1004D950u);
   u32 offset=data(staff,0x1004D970,1),angle=data(staff,0x1004D9B4,0);
   f32 rotation=angle?gabi::load<f32>(angle):zero;u32 resource=0;
   if(gabi::load<u8>(0x1047E6B8)){
    gabi::Local<StringRef> archive,name;st(archive.a,0x1047E6B8);st(archive.a+4,0x1004D928);st(name.a,filename);st(name.a+4,0x1004D928);
    resource=gabi::call<u32>(0x02606900,ld(0x101F4F28),archive.get(),name.get());
   }
   if(!resource){resource=gabi::call<u32>(0x0252447C,0x1004D958u,filename);if(!resource)gabi::call(0x0273AA24,0x1004D994u,0x44Fu,0x1004D97Cu);}
   gabi::call(0x02528CEC,resource,offset,rotation);
   u32 p=play();gabi::store<f32>(p+0x52B4,gabi::load<f32>(0x1004D940));
   u32 start=data(staff,0x1004D988,3);if(start)gabi::call(0x025B8B68,ld(0x101F84DC)+0x644,(u32)gabi::load<u16>(start+2));
  }
 }
 if(action==0){gabi::call(0x02540AAC,self,staff);return;}
 if(action==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004D994u,0x488u,0x1004D94Cu);cutEnd(staff);return;}
 if(action!=1||gabi::load<s16>(self+0x40))return;
 u32 state=ld(0x101D6010);
 if(state==2){
  if(gabi::call<u32>(0x02540508,staff,5u)){gabi::store<u16>(self+0x40,1);state=ld(0x101D6010);}
  else{
   gabi::Local<StringRef> a,b;st(a.a,0x1004D968);st(a.a+4,0x1004D928);
   u32 p=play();st(b.a,p+0x5134);st(b.a+4,0x1004D928);
   gabi::call_ptr(ld(ld(a.a+4)+0x14),a.get());gabi::call_ptr(ld(ld(a.a+4)+0x14),a.get());
   u32 text=ld(a.a);gabi::call_ptr(ld(ld(b.a+4)+0x14),b.get());
   if(text==ld(b.a)||boundedStringEq(ld(a.a),ld(b.a))){
    gabi::call(0x0252012C,0x1004D960u,0u,44u,0u,0u,1u,0u,zero);gabi::store<u16>(self+0x40,1);
   }else gabi::call(0x02528AC4);
   state=ld(0x101D6010);
  }
 }
 if(!state)cutEnd(staff);
}
VERIFY(0x02540B0C,package);
static u32 staffId(u32 name){return gabi::call<u32>(0x02542D88,play()+0x52C4,name,0u,0u);}
static u32 actionId(u32 staff,u32 table,u32 count){return gabi::call<u32>(0x02542EDC,play()+0x52C4,staff,table,count,0u,0u);}
static u32 isAdvance(u32 staff){return gabi::call<u32>(0x025447C8,play()+0x52C4,staff);}
static void timekeeper(u32 self){
 WWHD_FUNC(0x02541664,void,self);
 u32 staff=staffId(0x1004DAB4);if(staff==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004DAC0u,0x49Du,0x1004DAA0u);return;}
 u32 action=actionId(staff,0x101D6304,2);
 if(isAdvance(staff)&&action==0){u32 p=data(staff,0x1004DAA4,3);if(!p)gabi::call(0x0273AA24,0x1004DAC0u,0x4AEu,0x1004DAACu);gabi::store<u16>(self+0x42,(u16)ld(p));}
 if(action==0){s16 timer=gabi::load<s16>(self+0x42);if(timer>0)gabi::store<u16>(self+0x42,(u16)(timer-1));else cutEnd(staff);}
 else if(action==1)cutEnd(staff);
 else if(action==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004DAC0u,0x4CBu,0x1004DAA0u);cutEnd(staff);}
}
VERIFY(0x02541664,timekeeper);
static void light(u32 self){
 WWHD_FUNC(0x025422AC,void,self);
 u32 staff=staffId(0x1004DC98);if(staff==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004DCACu,0x191u,0x1004DCA0u);return;}
 u32 action=actionId(staff,0x101D6374,3);
 if(isAdvance(staff)){
  f32 multiplier=gabi::load<f32>(0x1004D880);
  if(action==1){
   u32 p=data(staff,0x1004DCA4,0);if(p)gabi::call(0x0256076C,gabi::fmuls_ppc(multiplier,gabi::load<f32>(p)));
   p=data(staff,0x1004DC90,3);if(p&&ld(p)==0){gabi::call(0x025607E0);cutEnd(staff);return;}
  }else if(action==2){
   u32 p=data(staff,0x1004DCA4,0);
   if(p){f32 hour=gabi::load<f32>(p),speed=gabi::load<f32>(0x1004DC84),current=gabi::load<f32>(ld(0x101F84DC)+0x44);
    f32 value=gabi::fmadds(current,speed,hour);f64 day=gabi::load<f64>(0x1004DC88);
    while(!(value<day))value=(f32)((f64)value-day);
    gabi::call(0x0256076C,gabi::fmuls_ppc(multiplier,value));
   }
  }
 }
 cutEnd(staff);
}
VERIFY(0x025422AC,light);
static void specialDispatcher(u32 self){
 WWHD_FUNC(0x025424B8,void,self);
 switch(ld(self+0x2C)){
 case 1:cutEnd(ld(self+0x24));break;
 case 4:gabi::call(0x02541664,self);break;
 case 6:gabi::call(0x02540FB8,self);break;
 case 7:gabi::call(0x02541DF8,self);break;
 case 8:gabi::call(0x02541B40,self);break;
 case 9:gabi::call(0x025422AC,self);break;
 case 11:gabi::call(0x02540B0C,self);break;
 case 12:gabi::call(0x02541828,self);cutEnd(ld(self+0x24));break;
 default:break;
 }
}
VERIFY(0x025424B8,specialDispatcher);
struct VecWords{be<u32> x,y,z;};struct AngleWords{be<u16> x,y,z;};
static void rawCopy(u32 to,u32 from,u32 n){for(u32 i=0;i<n;i+=4)st(to+i,ld(from+i));}
static void create(u32 self){
 WWHD_FUNC(0x02541828,void,self);
 u32 staff=staffId(0x1004DAE8);if(staff==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004DB14u,0x2F3u,0x1004DAF8u);return;}
 u32 action=actionId(staff,0x101D630C,2);if(!isAdvance(staff)||action!=1)return;
 u32 name=data(staff,0x1004DB28,4);if(!name)gabi::call(0x0273AA24,0x1004DB14u,0x303u,0x1004DB0Cu);
 u32 info=gabi::call<u32>(0x025C109C,name);if(!info)gabi::call(0x0273AA24,0x1004DB14u,0x305u,0x1004DB34u);
 u32 p=data(staff,0x1004DAF0,3),params=p?ld(p):0xFFFFFFFF;
 gabi::Local<VecWords> pos,scale;gabi::Local<AngleWords> angle;
 p=data(staff,0x1004DAF4,1);if(p)rawCopy(pos.a,p,12);else rawCopy(pos.a,ld(play()+0x5B2C)+0x314,12);
 p=data(staff,0x1004DAFC,3);for(u32 i=0;i<3;++i)gabi::store<u16>(angle.a+i*2,p?gabi::load<u16>(p+2+i*4):0);
 p=data(staff,0x1004DB04,1);if(p)rawCopy(scale.a,p,12);else{f32 one=gabi::load<f32>(0x1004D9D8);for(u32 i=0;i<3;++i)gabi::store<f32>(scale.a+i*4,one);}
 s32 room=gabi::load<s8>(0x1047E6C8),layer=gabi::load<s8>(info+0xA),profile=gabi::load<s16>(info+8);
 gabi::call(0x025D5834,profile,params,pos.get(),room,angle.get(),scale.get(),layer,0u);
}
VERIFY(0x02541828,create);
static void sound(u32 self){
 WWHD_FUNC(0x02541B40,void,self);
 u32 staff=staffId(0x1004DB5C);if(staff==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004DB78u,0x274u,0x1004DB64u);return;}
 u32 action=actionId(staff,0x101D6314,8);
 if(isAdvance(staff)){
  switch(action){
  case 0:case 4:gabi::call(0x02540A48,self,staff);break;
  case 1:gabi::call(0x025E1944);break;
  case 2:gabi::call(0x025E1934,0xC0000004u);break;
  case 3:gabi::call(0x025E1934,0xC0000005u);break;
  case 5:gabi::call(0x025E1988,0x806u);break;
  case 6:{u32 p=data(staff,0x1004DB50,3);if(p){u32 choice=ld(p);if((s32)choice>6){gabi::call(0x0273AA24,0x1004DB78u,0x2B3u,0x1004DB64u);choice=ld(p);}gabi::call(0x025E1FE4,ld(0x101D6334+choice*4));}
   p=data(staff,0x1004DB68,3);if(p)gabi::call(0x025E1FF4,p);
   p=data(staff,0x1004DB58,3);if(p)gabi::call(0x025E2000,p);break;}
  case 7:{u32 p=data(staff,0x1004DB70,3);if(p)gabi::call(0x025E1904,ld(p));break;}
  default:break;
  }
 }
 if(action==0){gabi::call(0x02540AAC,self,staff);return;}
 if(action!=4||(u32)(s32)gabi::load<s16>(self+0x42)<=ld(0x101D600C))cutEnd(staff);
}
VERIFY(0x02541B40,sound);
struct Color{u8 r,g,b,a;};
static void director(u32 self){
 WWHD_FUNC(0x02540FB8,void,self);
 u32 player=ld(play()+0x5B34),staff=staffId(0x1004DA20);
 if(staff==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004DA2Cu,0x350u,0x1004D9E4u);return;}
 u32 action=actionId(staff,0x101D62D4,9);u32 advanceFlag=isAdvance(staff);
 f32 zero=gabi::load<f32>(0x1004D884),one=gabi::load<f32>(0x1004D9D8);
 if(advanceFlag){switch(action){
 case 0:gabi::call(0x02540A48,self,staff);break;
 case 1:gabi::call(0x02540508,staff,0u);break;
 case 2:{u32 rate=data(staff,0x1004DA00,0),col=data(staff,0x1004D9E8,3);
  if(!rate)gabi::call(0x0273AA24,0x1004DA2Cu,0x363u,0x1004DA08u);
  if(gabi::load<f32>(rate)>zero)gabi::store<f32>(0x101F4810,zero);
  if(col){gabi::Local<Color> c;for(u32 i=0;i<4;++i)gabi::store<u8>(c.a+i,(u8)ld(col+4*i));gabi::call(0x025F0658,c.get(),gabi::load<f32>(rate));}
  else gabi::call(0x025F069C,gabi::load<f32>(rate));break;}
 case 3:{u32 p=data(staff,0x1004DA10,3),i=p?ld(p):0;gabi::call(0x025E18EC,ld(0x101D62F8+i*4));break;}
 case 4:{gabi::call(0x02540A48,self,staff);if(!gabi::load<s16>(self+0x42))gabi::call(0x0273AA24,0x1004DA2Cu,0x38Bu,0x1004D9E4u);
  u32 strength=data(staff,0x1004D9DC,3),type=data(staff,0x1004DA18,3);if(!strength||!type)gabi::call(0x0273AA24,0x1004DA2Cu,0x38Fu,0x1004D9E4u);
  u32 p=play();gabi::Local<VecWords> dir;gabi::store<f32>(dir.a+8,zero);gabi::store<f32>(dir.a+4,one);gabi::store<f32>(dir.a,zero);
  gabi::call(0x025CB4E0,p+0x599C,strength,0u,ld(type),dir.get());break;}
 case 5:{u32 p=data(staff,0x1004D9F0,3);if(p&&(s32)ld(p)<=0)gabi::call(0x025E1988,ld(0x101D62D0));break;}
 case 6:{u32 rate=data(staff,0x1004DA00,0);if(!rate)gabi::call(0x0273AA24,0x1004DA2Cu,0x3A4u,0x1004DA08u);
  gabi::call(0x0252F590,-gabi::load<f32>(rate));gabi::store<u16>(self+0x40,gabi::load<f32>(rate)>zero?0:1);
  u32 p=data(staff,0x1004D9F8,3);if(p&&!ld(p))gabi::call(0x025E1988,0x8DAu);break;}
 case 7:st(player+0x3B8,ld(player+0x3B8)&0xF7FFFFFF);break;
 case 8:st(player+0x3B8,ld(player+0x3B8)|0x08000000);break;
 default:break;
 }}
 if(action==0){gabi::call(0x02540AAC,self,staff);return;}
 if(action==1)return;
 if(action==2){
  if(!gabi::load<u8>(0x101F4827)){cutEnd(staff);return;}
  if(gabi::load<f32>(0x101F4810)<one)return;
  u32 rate=data(staff,0x1004DA00,0);if(!rate)gabi::call(0x0273AA24,0x1004DA2Cu,0x3D8u,0x1004DA08u);
  if(gabi::load<f32>(rate)>zero)cutEnd(staff);return;
 }
 if(action==4){s16 timer=gabi::load<s16>(self+0x42);if(timer>0){timer=(s16)(timer-1);gabi::store<u16>(self+0x42,(u16)timer);if(!timer)gabi::call(0x025CB610,play()+0x599C,0xFFFFFFFFu);}else cutEnd(staff);return;}
 if(action==6){f32 fade=gabi::load<f32>(0x101D6160);if(gabi::load<s16>(self+0x40)){if(fade==zero)cutEnd(staff);}else if(!(fade<one))cutEnd(staff);return;}
 cutEnd(staff);
}
VERIFY(0x02540FB8,director);
static void message(u32 self){
 WWHD_FUNC(0x02541DF8,void,self);
 u32 manager=ld(0x101F4B5C),staff=staffId(0x1004DBF4);
 if(staff==0xFFFFFFFF){gabi::call(0x0273AA24,0x1004DC18u,0x1EAu,0x1004DBFCu);return;}
 u32 action=actionId(staff,0x101D6350,9);
 if(isAdvance(staff)){switch(action){
 case 0:gabi::call(0x02540A48,self,staff);break;
 case 1:{st(0x104758F4,0xFFFFFFFF);u32 p=data(staff,0x1004DC00,3);if(!p)gabi::call(0x0273AA24,0x1004DC18u,0x1F9u,0x1004DC08u);st(0x104758F8,ld(p));gabi::store<u16>(self+0x40,0);break;}
 case 3:case 5:gabi::call(0x025F74D0,manager,16u);break;
 case 4:{gabi::call(0x025F74D0,manager,15u);u32 p=data(staff,0x1004DC00,3);if(!p)gabi::call(0x0273AA24,0x1004DC18u,0x204u,0x1004DC08u);u32 id=ld(p);st(0x104758F8,id);gabi::call(0x025F7DB0,manager,id,0u);break;}
 case 7:{u32 p=data(staff,0x1004DC10,3);if(!p)gabi::call(0x0273AA24,0x1004DC18u,0x20Au,0x1004DC08u);u8 choice=gabi::load<u8>(p+3);u32 game=play();gabi::store<u8>(game+0x5BE1,choice);gabi::store<u8>(game+0x5BE2,2);gabi::call(0x02540A48,self,staff);break;}
 case 8:gabi::store<u8>(play()+0x5BE2,1);break;
 default:break;
 }}
 switch(action){
 case 0:case 7:gabi::call(0x02540AAC,self,staff);return;
 case 1:{u32 phase=(u32)(s32)gabi::load<s16>(self+0x40);
  if(phase==0){u32 id=gabi::call<u32>(0x025F7DB0,manager,ld(0x104758F8),0u);st(0x104758F4,id);if(id==0xFFFFFFFF)return;phase=(u32)(s32)gabi::load<s16>(self+0x40);}
  else if(phase==2){cutEnd(staff);return;}else if(phase!=1)return;
  gabi::store<u16>(self+0x40,(u16)(phase+1));return;}
 case 2:if(gabi::call<u32>(0x025F795C,manager)==14)cutEnd(staff);return;
 case 3:case 6:
  if(ld(0x104758F4)==0xFFFFFFFF){cutEnd(staff);return;}
  if(gabi::call<u32>(0x025F795C,manager)==18){gabi::call(0x025F74D0,manager,19u);st(0x104758F4,0xFFFFFFFF);cutEnd(staff);}return;
 case 4:cutEnd(staff);return;
 case 5:{u32 state=gabi::call<u32>(0x025F795C,manager);if(state>=17&&state<=18)cutEnd(staff);return;}
 case 8:if(!gabi::load<u8>(play()+0x5BE2))cutEnd(staff);return;
 default:cutEnd(staff);return;
 }
}
VERIFY(0x02541DF8,message);
}
