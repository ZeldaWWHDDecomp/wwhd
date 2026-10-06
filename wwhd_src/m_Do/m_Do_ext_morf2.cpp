#include "gabi.h"
using namespace gabi;
void ext2_setAnm(void* self,void* animation,void* secondAnimation,s32 mode,void* bas,f32 mix,f32 morf,f32 speed,f32 start,f32 end) {
 WWHD_FUNC(0x025E5D54,void,self,animation,secondAnimation,mode,bas,mix,morf,speed,start,end);
 u32 o=ea(self),a=ea(animation),b=ea(bas);f32 zero=load<f32>(0x100586DC);s16 first=(s16)ftoi(start);
 store<u32>(o+0x94,a);store<u32>(o+0x98,ea(secondAnimation));store<f32>(o+0xC0,mix);store<s16>(o+0xAC,first);store<f32>(o+0xA8,(f32)first);
 s32 last;
 if(!(end<zero))last=(s16)ftoi(end);
 else {u32 current=load<u32>(o+0x94);if(!current)last=0;else{u32 table=load<u32>(current+4),target=load<u32>(table+0x14),data=load<u32>(a+8),flags=load<u32>(data+0xC);last=call_ptr<s32>(target,at<void>(current));if(flags&4)last=(s16)(last-1);}}
 call(0x027F2BC0,at<void>(o+0xA4),last);
 if(mode<0){u32 table=load<u32>(a+4);mode=call_ptr<s32>(load<u32>(table+0xC),animation);}
 store<f32>(o+0xA4,speed);store<u8>(o+0xB2,(u8)mode);
 s16 frame=speed<zero?load<s16>(o+0xAE):first;store<f32>(o+0xA8,(f32)frame);store<s16>(o+0xB0,frame);
 call(0x025E5D1C,self,morf);u32 audio=load<u32>(o+0xC4);if(!audio)return;
 if(!b&&a){u32 data=load<u32>(a+8),relative=load<u32>(data+4),name=relative?data+4+relative:0;Local<u8[8]> str;store<u32>(ea(str.get()),name);store<u32>(ea(str.get())+4,0x10058704);b=call<u32>(0x0260702C,at<void>(load<u32>(0x101F4F28)),str.get(),at<void>(a+0xC));audio=load<u32>(o+0xC4);}
 store<u32>(audio+0x9C,b);audio=load<u32>(o+0xC4);u32 agent=load<u32>(audio+0x94),target=load<u32>(agent+0x14);
 if(!b){call_ptr(target,at<void>(audio),0,1,zero);return;}
 s32 direction=load<f32>(o+0xA4)<zero?-1:1;f32 current=(f32)load<s16>(o+0xB0);call_ptr(target,at<void>(audio),at<void>(b),direction,current);
 Local<u8[8]> str;u32 st=ea(str.get());store<u32>(st,0x10058A10);store<u32>(st+4,0x10058704);call(0x025EE5EC,str.get());call_ptr(load<u32>(load<u32>(st+4)+0x14),str.get());
 u32 nameVT=load<u32>(a+0x10),nameTarget=load<u32>(nameVT+0x14),name=load<u32>(st);call_ptr(nameTarget,at<void>(a+0xC),b,direction,nameVT,0x10058A10);u32 other=load<u32>(a+0xC);
 if(name!=other){for(u32 i=0;i<0x40001;i++){u8 left=load<u8>(name+i),right=load<u8>(other+i);if(left!=right)return;if(!left){audio=load<u32>(o+0xC4);store<u32>(audio+0x98,1);return;}}return;}
 audio=load<u32>(o+0xC4);store<u32>(audio+0x98,1);
}
VERIFY(0x025E5D54,ext2_setAnm);
void ext2_buffer(void* self) {
 WWHD_FUNC(0x025E6188,void,self);
 u32 o=ea(self),model=load<u32>(o+0x90),data=load<u32>(model+0xAC),joints=call<u32>(0x027F3F94,at<void>(data));store<u32>(o+0x6C,joints);
 Local<u8[0x14]> desc; /* game test 2026-10-05: 027E06E0/027E063C fill desc+4..+0x14 */store<u32>(ea(desc.get())+4,0);store<u32>(ea(desc.get()),0xFFFFFFFF);store<u32>(ea(desc.get())+8,0);
 load<u16>(joints+8);u32 count=load<u16>(joints+8);store<u32>(ea(desc.get()),count);
 u32 size=call<u32>(0x027E06E0,desc.get()),buffer=call<u32>(0x025E3A34,size,0x40);store<u32>(o+0x8C,buffer);call(0x027E063C,at<void>(o+0x78),desc.get(),at<void>(buffer),size);
}
VERIFY(0x025E6188,ext2_buffer);
u32 ext2_play(void* self,void* position,u32 sound,s32 reverb) {
 WWHD_FUNC(0x025E65FC,u32,self,position,sound,reverb);
 u32 o=ea(self);f32 progress=load<f32>(o+0xB4),one=load<f32>(0x100586D4);
 if(progress<one){f32 delta=load<f32>(o+0xBC);store<f32>(o+0xB8,progress);call(0x0200F5C8,at<void>(o+0xB4),one,delta);}
 call(0x027F2FC4,at<void>(o+0xA4));u32 audio=load<u32>(o+0xC4);
 if(audio&&load<u32>(audio+0x9C)&&ea(position)){f32 frame=load<f32>(o+0xA8),rate=load<f32>(o+0xA4);call(0x0201BF08,at<void>(audio),position,sound,reverb,frame,rate);}
 if(load<u8>(o+0xB3)&1)return 1;return load<f32>(o+0xA4)==load<f32>(0x100586DC)?1:0;
}
VERIFY(0x025E65FC,ext2_play);
void ext2_calc(void* self){WWHD_FUNC(0x025E66E4,void,self);u32 o=ea(self);if(!load<u32>(o+0x90))return;u32 animation=load<u32>(o+0x94);if(animation){store<u32>(animation,load<u32>(o+0xA8));animation=load<u32>(o+0x94);call(0x027F363C,self,at<void>(animation));u32 control=load<u32>(o+4);animation=load<u32>(o+0x94);u32 target=load<u32>(control+0x10),receiver=load<u32>(control+0x14);f32 second=load<f32>(control+4),third=load<f32>(control+8),first=load<f32>(animation);f32 result=call_ptr<f32>(target,at<void>(receiver),first,second,third);store<f32>(control,result);}u32 secondAnimation=load<u32>(o+0x98);if(secondAnimation)store<u32>(secondAnimation,load<u32>(o+0xA8));u32 model=load<u32>(o+0x90),data=load<u32>(model+0xAC),root=load<u32>(data+8);store<u32>(root+0x14,o);model=load<u32>(o+0x90);call(0x027F4D5C,at<void>(model));}
VERIFY(0x025E66E4,ext2_calc);
void ext2_entry(void* self){WWHD_FUNC(0x025E66D4,void,self);u32 model=load<u32>(ea(self)+0x90);if(model)call(0x025E2E5C,at<void>(model));}
VERIFY(0x025E66D4,ext2_entry);
void ext2_stop(void* self){WWHD_FUNC(0x025E6794,void,self);u32 audio=load<u32>(ea(self)+0xC4);if(audio)call(0x02801D3C,at<void>(audio));}
VERIFY(0x025E6794,ext2_stop);

void* ext2_ctor(void* self,void* data,void* cb1,void* cb2,void* animation,void* secondAnimation,s32 mode,s32 start,f32 speed){
 WWHD_FUNC(0x025E6220,void*,self,data,cb1,cb2,animation,secondAnimation,mode,start,speed);
 u32 sp=cpu->r[1];s32 end=(s32)load<u32>(sp+8);u32 enable=load<u32>(sp+0xC),b=load<u32>(sp+0x10),modelFlag=load<u32>(sp+0x14),dlFlag=load<u32>(sp+0x18),o=ea(self),d=ea(data),a=ea(animation);
 if(!o){o=call<u32>(0x0273AD10,0xD4);if(!o)return nullptr;}
 store<u32>(o,0x1016E54C);call(0x027DA984,at<void>(o+4));store<u32>(o,0x10058CE8);store<u32>(o+0x38,0x1016D820);
 for(u32 offset:{0x48u,0x6Cu,0x70u,0x74u,0x88u,0x8Cu,0x90u,0x94u,0x98u,0x9Cu,0xA0u})store<u32>(o+offset,0);
 call(0x027F2BC0,at<void>(o+0xA4),0);store<u32>(o+0xC8,0);store<u32>(o+0xCC,0);store<u32>(o+0x90,0);f32 zero=load<f32>(0x100586DC);store<f32>(o+0xB4,zero);store<u32>(o+0xC4,0);store<u32>(o+0x9C,0);store<u8>(o+0xD0,1);store<f32>(o+0xB8,zero);store<f32>(o+0xBC,zero);store<f32>(o+0xC0,zero);store<u32>(o+0xA0,0);if(!d)return at<void>(o);
 u32 model=call<u32>(0x025E38E0,data,modelFlag,dlFlag);store<u32>(o+0x90,model);if(!model)return at<void>(o);
 if(!a)call(0x0273AA24,at<void>(0x10058A1C),0x153B,at<void>(0x10058A14));call(0x027F36FC,at<void>(o),data,animation);
 bool found=false;if(!b&&a){u32 ad=load<u32>(a+8),rel=load<u32>(ad+4),name=rel?ad+4+rel:0;Local<u8[8]> str;store<u32>(ea(str.get()),name);store<u32>(ea(str.get())+4,0x10058704);b=call<u32>(0x0260702C,at<void>(load<u32>(0x101F4F28)),str.get(),at<void>(a+0xC));found=b!=0;}
 if((found||enable)&&!call<u32>(0x025E20F8)){u32 audio=call<u32>(0x0273AD10,0xA0);if(audio){call(0x028F521C,at<void>(audio),0xA0);call(0x02801DA0,at<void>(audio));store<u32>(audio+0x98,0);store<u32>(audio+0x94,0x1005871C);}store<u32>(o+0xC4,audio);if(!audio){call(0x025E5C90,at<void>(o));return at<void>(o);}}
 call(0x025E5D54,at<void>(o),animation,secondAnimation,mode,at<void>(b),zero,zero,speed,(f32)start,(f32)end);
 u32 joints=call<u32>(0x027F3F94,data),count=load<u16>(joints+8),trans=call<u32>(0x0273ADAC,count<<5);store<u32>(o+0x9C,trans);if(!trans){call(0x025E5C90,at<void>(o));return at<void>(o);}
 joints=call<u32>(0x027F3F94,data);count=load<u16>(joints+8);u32 quat=call<u32>(0x0273ADAC,count<<4);store<u32>(o+0xA0,quat);if(!quat){call(0x025E5C90,at<void>(o));return at<void>(o);}
 call(0x025E6188,at<void>(o));model=load<u32>(o+0x90);trans=load<u32>(o+0x9C);u32 md=load<u32>(model+0xAC);quat=load<u32>(o+0xA0);joints=call<u32>(0x027F3F94,at<void>(md));count=load<u16>(joints+8);
 if(count){f32 sx=load<f32>(0x1016E464),sy=load<f32>(0x1016E468),sz=load<f32>(0x1016E46C),tx=load<f32>(0x1016E478),ty=load<f32>(0x1016E47C),tz=load<f32>(0x1016E480);s16 rx=load<s16>(0x1016E470),ry=load<s16>(0x1016E472),rz=load<s16>(0x1016E474);
 for(u32 i=0;i<count;i++){store<f32>(trans,sx);store<f32>(trans+4,sy);store<f32>(trans+8,sz);store<f32>(trans+0x14,tx);store<f32>(trans+0x18,ty);store<s16>(trans+0xC,rx);store<s16>(trans+0xE,ry);store<s16>(trans+0x10,rz);store<f32>(trans+0x1C,tz);call(0x027ED2D0,(s32)rx,(s32)ry,(s32)rz,at<void>(quat));trans+=32;quat+=16;}}
 store<u32>(o+0xC8,ea(cb1));store<u32>(o+0xCC,ea(cb2));return at<void>(o);
}
VERIFY(0x025E6220,ext2_ctor);
