#include "gabi.h"
using namespace gabi;
void ext_setMorf(void* self,f32 duration) {
 WWHD_FUNC(0x025E4A54,void,self,duration);
 u32 o=ea(self);f32 previous=load<f32>(o+0xB4),zero=load<f32>(0x100586DC),one=load<f32>(0x100586D4);
 if(previous<zero||!(duration>zero)){store<f32>(o+0xB0,one);store<f32>(o+0xB4,one);return;}
 f32 delta=one/duration;store<f32>(o+0xB0,zero);store<f32>(o+0xB4,zero);store<f32>(o+0xB8,delta);
}
VERIFY(0x025E4A54,ext_setMorf);
void ext_4ECC(void* self) {
 WWHD_FUNC(0x025E4ECC,void,self);
 u32 o=ea(self),model=load<u32>(o+0x90),data=load<u32>(model+0xAC),joints=call<u32>(0x027F3F94,at<void>(data));store<u32>(o+0x6C,joints);
 Local<u8[0x14]> desc; /* game test 2026-10-05: 027E06E0/027E063C fill desc+4..+0x14 */store<u32>(ea(desc.get())+4,0);store<u32>(ea(desc.get()),0xFFFFFFFF);store<u32>(ea(desc.get())+8,0);
 load<u16>(joints+8);u32 count=load<u16>(joints+8);store<u32>(ea(desc.get()),count);
 u32 size=call<u32>(0x027E06E0,desc.get()),buffer=call<u32>(0x025E3A34,size,0x40);store<u32>(o+0x8C,buffer);call(0x027E063C,at<void>(o+0x78),desc.get(),at<void>(buffer),size);
}
VERIFY(0x025E4ECC,ext_4ECC);
u32 ext_play(void* self,void* position,u32 sound,s32 reverb) {
 WWHD_FUNC(0x025E535C,u32,self,position,sound,reverb);
 u32 o=ea(self);f32 progress=load<f32>(o+0xB0),one=load<f32>(0x100586D4);
 if(progress<one){f32 delta=load<f32>(o+0xB8);store<f32>(o+0xB4,progress);call(0x0200F5C8,at<void>(o+0xB0),one,delta);}
 call(0x027F2FC4,at<void>(o+0x98));u32 audio=load<u32>(o+0xBC);
 if(audio&&load<u32>(audio+0x9C)&&ea(position)){f32 frame=load<f32>(o+0x9C),rate=load<f32>(o+0x98);call(0x0201BF08,at<void>(audio),position,sound,reverb,frame,rate);}
 if(load<u8>(o+0xA7)&1)return 1;return load<f32>(o+0x98)==load<f32>(0x100586DC)?1:0;
}
VERIFY(0x025E535C,ext_play);
void ext_5434(void* self) {
 WWHD_FUNC(0x025E5434,void,self);
 u32 o=ea(self),model=load<u32>(o+0x90);if(!model)return;
 u32 anim=load<u32>(o+0x94);
 if(anim){u32 frameBits=load<u32>(o+0x9C);store<u32>(anim,frameBits);anim=load<u32>(o+0x94);call(0x027F363C,self,at<void>(anim));u32 control=load<u32>(o+4);anim=load<u32>(o+0x94);u32 target=load<u32>(control+0x10);f32 second=load<f32>(control+4);u32 receiver=load<u32>(control+0x14);f32 third=load<f32>(control+8),first=load<f32>(anim);f32 result=call_ptr<f32>(target,at<void>(receiver),first,second,third);store<f32>(control,result);model=load<u32>(o+0x90);}
 u32 data=load<u32>(model+0xAC),root=load<u32>(data+8);store<u32>(root+0x14,o);model=load<u32>(o+0x90);
 call(0x025E2DA8,at<void>(model));
 f32 progress=load<f32>(o+0xB0);store<f32>(o+0xB4,progress);}
VERIFY(0x025E5434,ext_5434);
void ext_54D8(void* self) {
 WWHD_FUNC(0x025E54D8,void,self);
 u32 o=ea(self),model=load<u32>(o+0x90);if(!model)return;
 u32 anim=load<u32>(o+0x94);
 if(anim){u32 frameBits=load<u32>(o+0x9C);store<u32>(anim,frameBits);anim=load<u32>(o+0x94);call(0x027F363C,self,at<void>(anim));u32 control=load<u32>(o+4);anim=load<u32>(o+0x94);u32 target=load<u32>(control+0x10);f32 second=load<f32>(control+4);u32 receiver=load<u32>(control+0x14);f32 third=load<f32>(control+8),first=load<f32>(anim);f32 result=call_ptr<f32>(target,at<void>(receiver),first,second,third);store<f32>(control,result);model=load<u32>(o+0x90);}
 u32 data=load<u32>(model+0xAC),root=load<u32>(data+8);store<u32>(root+0x14,o);model=load<u32>(o+0x90);
 call(0x025E2DE0,at<void>(model),0);
 f32 progress=load<f32>(o+0xB0);store<f32>(o+0xB4,progress);}
VERIFY(0x025E54D8,ext_54D8);
void ext_55A0(void* self) {
 WWHD_FUNC(0x025E55A0,void,self);
 u32 o=ea(self),model=load<u32>(o+0x90);if(!model)return;
 u32 anim=load<u32>(o+0x94);
 if(anim){u32 frameBits=load<u32>(o+0x9C);store<u32>(anim,frameBits);anim=load<u32>(o+0x94);call(0x027F363C,self,at<void>(anim));u32 control=load<u32>(o+4);anim=load<u32>(o+0x94);u32 target=load<u32>(control+0x10);f32 second=load<f32>(control+4);u32 receiver=load<u32>(control+0x14);f32 third=load<f32>(control+8),first=load<f32>(anim);f32 result=call_ptr<f32>(target,at<void>(receiver),first,second,third);store<f32>(control,result);model=load<u32>(o+0x90);}
 u32 data=load<u32>(model+0xAC),root=load<u32>(data+8);store<u32>(root+0x14,o);model=load<u32>(o+0x90);
 call(0x027F4D5C,at<void>(model));
 }
VERIFY(0x025E55A0,ext_55A0);
void ext_5580(void* self){WWHD_FUNC(0x025E5580,void,self);u32 target=load<u32>(ea(self)+0x90);if(target)call(0x025E2E24,at<void>(target));}
VERIFY(0x025E5580,ext_5580);
void ext_5590(void* self){WWHD_FUNC(0x025E5590,void,self);u32 target=load<u32>(ea(self)+0x90);if(target)call(0x025E2E5C,at<void>(target));}
VERIFY(0x025E5590,ext_5590);
void ext_563C(void* self){WWHD_FUNC(0x025E563C,void,self);u32 target=load<u32>(ea(self)+0xBC);if(target)call(0x02801D3C,at<void>(target));}
VERIFY(0x025E563C,ext_563C);

void ext_setAnm(void* self,void* animation,s32 mode,void* bas,f32 morf,f32 speed,f32 start,f32 end) {
 WWHD_FUNC(0x025E4A98,void,self,animation,mode,bas,morf,speed,start,end);
 u32 o=ea(self),a=ea(animation),b=ea(bas);f32 zero=load<f32>(0x100586DC);s16 first=(s16)ftoi(start);
 store<u32>(o+0x94,a);store<s16>(o+0xA0,first);store<f32>(o+0x9C,(f32)first);
 s32 last;
 if(!(end<zero))last=(s16)ftoi(end);
 else {u32 current=load<u32>(o+0x94);if(!current)last=0;else{u32 table=load<u32>(current+4),target=load<u32>(table+0x14),data=load<u32>(a+8),flags=load<u32>(data+0xC);last=call_ptr<s32>(target,at<void>(current));if(flags&4)last=(s16)(last-1);}}
 call(0x027F2BC0,at<void>(o+0x98),last);
 if(a&&mode<0){u32 table=load<u32>(a+4);mode=call_ptr<s32>(load<u32>(table+0xC),animation);}
 store<f32>(o+0x98,speed);store<u8>(o+0xA6,(u8)mode);
 s16 frame=speed<zero?load<s16>(o+0xA2):first;store<f32>(o+0x9C,(f32)frame);store<s16>(o+0xA4,frame);
 call(0x025E4A54,self,morf);u32 audio=load<u32>(o+0xBC);if(!audio)return;
 if(!b&&a){u32 data=load<u32>(a+8),relative=load<u32>(data+4),name=relative?data+4+relative:0;Local<u8[8]> str;store<u32>(ea(str.get()),name);store<u32>(ea(str.get())+4,0x10058704);b=call<u32>(0x0260702C,at<void>(load<u32>(0x101F4F28)),str.get(),at<void>(a+0xC));audio=load<u32>(o+0xBC);}
 store<u32>(audio+0x9C,b);audio=load<u32>(o+0xBC);u32 agent=load<u32>(audio+0x94),target=load<u32>(agent+0x14);
 if(!b){call_ptr(target,at<void>(audio),0,1,zero);return;}
 s32 direction=load<f32>(o+0x98)<zero?-1:1;f32 current=(f32)load<s16>(o+0xA4);call_ptr(target,at<void>(audio),at<void>(b),direction,current);
 Local<u8[8]> str;u32 st=ea(str.get());store<u32>(st,0x10058A08);store<u32>(st+4,0x10058704);call(0x025EE5EC,str.get());call_ptr(load<u32>(load<u32>(st+4)+0x14),str.get());
 u32 name=load<u32>(st);call_ptr(load<u32>(load<u32>(a+0x10)+0x14),at<void>(a+0xC));u32 other=load<u32>(a+0xC);
 if(name!=other){for(u32 i=0;i<0x40001;i++){u8 left=load<u8>(name+i),right=load<u8>(other+i);if(left!=right)return;if(!left){audio=load<u32>(o+0xBC);store<u32>(audio+0x98,1);return;}}return;}
 audio=load<u32>(o+0xBC);store<u32>(audio+0x98,1);
}
VERIFY(0x025E4A98,ext_setAnm);

void* ext_morfCtor(void* self,void* data,void* cb1,void* cb2,void* animation,s32 mode,s32 start,s32 end,f32 speed){
 WWHD_FUNC(0x025E4F64,void*,self,data,cb1,cb2,animation,mode,start,end,speed);
 u32 entrySP=cpu->r[1],enable=load<u32>(entrySP+8),b=load<u32>(entrySP+0xC),modelFlag=load<u32>(entrySP+0x10),dlFlag=load<u32>(entrySP+0x14);
 u32 o=ea(self),d=ea(data),a=ea(animation);if(!o){o=call<u32>(0x0273AD10,0xC8);if(!o)return nullptr;}
 store<u32>(o,0x1016E54C);call(0x027DA984,at<void>(o+4));store<u32>(o,0x10058CC0);store<u32>(o+0x38,0x1016D820);
 store<u32>(o+0x48,0);store<u32>(o+0x6C,0);store<u32>(o+0x70,0);store<u32>(o+0x74,0);store<u32>(o+0x88,0);store<u32>(o+0x8C,0);store<u32>(o+0x90,0);store<u32>(o+0x94,0);call(0x027F2BC0,at<void>(o+0x98),0);
 for(u32 offset: {0xC0u,0xC4u,0x90u,0x94u,0xBCu,0xA8u,0xACu})store<u32>(o+offset,0);
 f32 zero=load<f32>(0x100586DC);store<f32>(o+0xB0,zero);store<f32>(o+0xB4,zero);store<f32>(o+0xB8,zero);if(!d)return at<void>(o);
 u32 model=call<u32>(0x025E38E0,data,modelFlag,dlFlag);store<u32>(o+0x90,model);if(!model)return at<void>(o);
 call(0x027F36FC,at<void>(o),data,animation);
 bool found=false;
 if(!b&&a){u32 ad=load<u32>(a+8),rel=load<u32>(ad+4),name=rel?ad+4+rel:0;Local<u8[8]> str;store<u32>(ea(str.get()),name);store<u32>(ea(str.get())+4,0x10058704);b=call<u32>(0x0260702C,at<void>(load<u32>(0x101F4F28)),str.get(),at<void>(a+0xC));found=b!=0;}
 bool failed=false;
 if((found||enable)&&!call<u32>(0x025E20F8)){u32 audio=call<u32>(0x0273AD10,0xA0);if(audio){call(0x028F521C,at<void>(audio),0xA0);call(0x02801DA0,at<void>(audio));store<u32>(audio+0x98,0);store<u32>(audio+0x94,0x1005871C);}store<u32>(o+0xBC,audio);if(!audio)failed=true;}
 if(!failed){call(0x025E4A98,at<void>(o),animation,mode,at<void>(b),zero,speed,(f32)start,(f32)end);store<f32>(o+0xB4,load<f32>(0x10058A0C));
 u32 joints=call<u32>(0x027F3F94,data),count=load<u16>(joints+8),trans=call<u32>(0x0273ADAC,count<<5);store<u32>(o+0xA8,trans);if(!trans)failed=true;
 if(!failed){joints=call<u32>(0x027F3F94,data);count=load<u16>(joints+8);u32 quat=call<u32>(0x0273ADAC,count<<4);store<u32>(o+0xAC,quat);if(!quat)failed=true;
 if(!failed){call(0x025E4ECC,at<void>(o));model=load<u32>(o+0x90);trans=load<u32>(o+0xA8);u32 md=load<u32>(model+0xAC);quat=load<u32>(o+0xAC);joints=call<u32>(0x027F3F94,at<void>(md));count=load<u16>(joints+8);
 if(count){f32 sx=load<f32>(0x1016E464),sy=load<f32>(0x1016E468),sz=load<f32>(0x1016E46C),tx=load<f32>(0x1016E478),ty=load<f32>(0x1016E47C),tz=load<f32>(0x1016E480);s16 rx=load<s16>(0x1016E470),ry=load<s16>(0x1016E472),rz=load<s16>(0x1016E474);
 for(u32 i=0;i<count;i++){store<f32>(trans,sx);store<f32>(trans+4,sy);store<f32>(trans+8,sz);store<f32>(trans+0x14,tx);store<f32>(trans+0x18,ty);store<f32>(trans+0x1C,tz);store<s16>(trans+0xC,rx);store<s16>(trans+0xE,ry);store<s16>(trans+0x10,rz);call(0x027ED2D0,(s32)rx,(s32)ry,(s32)rz,at<void>(quat));trans+=32;quat+=16;}}
 store<u32>(o+0xC0,ea(cb1));store<u32>(o+0xC4,ea(cb2));return at<void>(o);}}}
 u32 audio=load<u32>(o+0xBC);if(audio){call(0x02801D3C,at<void>(audio));store<u32>(o+0xBC,0);}u32 trans=load<u32>(o+0xA8),quat=load<u32>(o+0xAC);if(trans)store<u32>(o+0xA8,0);if(quat)store<u32>(o+0xAC,0);model=load<u32>(o+0x90);if(model){call_ptr(load<u32>(load<u32>(model+0xC)+0xC),at<void>(model),2);store<u32>(o+0x90,0);}return at<void>(o);
}
VERIFY(0x025E4F64,ext_morfCtor);
