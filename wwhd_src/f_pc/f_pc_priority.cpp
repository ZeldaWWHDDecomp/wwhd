#include "bindings.h"
using namespace gabi;
namespace {
template<class T> T rd(u32 a,u32 o=0) { return load<T>(a+o); }
template<class T> void wr(u32 a,u32 o,T v) { store<T>(a+o,v); }
}
s32 fpcPi_IsInQueue_hd(u32 p) {
 WWHD_FUNC(0x025E0CBC,s32,p);
 return call<s32>(0x0201A71C,p);
}
VERIFY(0x025E0CBC,fpcPi_IsInQueue_hd);
s32 fpcPi_QueueTo_hd(u32 p) {
 WWHD_FUNC(0x025E0CC0,s32,p);
 call<void>(0x0201A868,p);
 call<void>(0x025DE9E0,p+0x14);
 const u32 a=rd<u32>(0x101F3E6C);wr<u32>(p,0x30,a);
 const u32 b=rd<u32>(0x101F3E70);wr<u32>(p,0x34,b);
 return 1;
}
VERIFY(0x025E0CC0,fpcPi_QueueTo_hd);
s32 fpcPi_IsNormal_hd(u32 layer,u32 list,u32 priority) {
 WWHD_FUNC(0x025E0D0C,s32,layer,list,priority);
 return layer<0xFFFFFFFEu && list<0xFFFEu && priority<0xFFFEu;
}
VERIFY(0x025E0D0C,fpcPi_IsNormal_hd);
s32 fpcPi_Change_hd(u32 p,u32 layer,u32 list,u32 priority) {
 WWHD_FUNC(0x025E0D38,s32,p,layer,list,priority);
 const u32 proc=rd<u32>(p,0xC);
 if(rd<u8>(proc,0xC)==3)return 0;
 if(!call<s32>(0x025E0D0C,layer,list,priority))return 0;
 const u32 a=rd<u32>(p,0x38),b=rd<u32>(p,0x3C);
 wr<u32>(p,0x30,a);wr<u32>(p,0x34,b);
 bool changed=false;
 if(layer!=0xFFFFFFFDu && rd<u32>(p,0x38)!=layer){changed=true;wr<u32>(p,0x30,layer);}
 if(list!=0xFFFDu && rd<u16>(p,0x3C)!=list){changed=true;wr<u16>(p,0x34,(u16)list);}
 if(priority!=0xFFFDu && rd<u16>(p,0x3E)!=priority){changed=true;wr<u16>(p,0x36,(u16)priority);}
 const u8 state=rd<u8>(proc,0xC);
 if(state==0 || state==1){
  const u32 x=rd<u32>(p,0x30),y=rd<u32>(p,0x34);
  wr<u32>(p,0x38,x);wr<u32>(p,0x3C,y);return 1;
 }
 if(!changed)return 0;
 const u32 queuedLayer=rd<u32>(p,0x30);
 if(!call<s32>(0x0201A8B4,0x1048AB40u,p))return 0;
 if(queuedLayer!=0xFFFFFFFDu){
  const u32 l=call<u32>(0x025DEAC0,queuedLayer);
  if(!call<s32>(0x025DE9E4,l,p+0x14)){call<void>(0x0201A868,p);return 0;}
 }
 return 1;
}
VERIFY(0x025E0D38,fpcPi_Change_hd);
s32 fpcPi_Handler_hd() {
 WWHD_FUNC(0x025E0EE4,s32);
 for(;;){
  const u32 tag=call<u32>(0x0201A838,0x1048AB40u);
  if(!tag)return 1;
  const u32 proc=rd<u32>(tag,0xC),p=proc+0x68;
  call<void>(0x025DE9E0,proc+0x7C);
  if(!p)return 1;
  const u32 owner=rd<u32>(p,0xC),layer=rd<u32>(p,0x30);
  const u32 line=owner+0x34;
  const u16 priority=rd<u16>(p,0x36),list=rd<u16>(p,0x34);
  if(call<s32>(0x025DF0F4,owner+0x18,layer,list,priority)!=1)return 0;
  const u16 oldList=rd<u16>(p,0x3C);
  call<void>(0x025DF738,line,oldList);
  const u32 a=rd<u32>(p,0x30),b=rd<u32>(p,0x34);
  wr<u32>(p,0x38,a);wr<u32>(p,0x3C,b);
 }
}
VERIFY(0x025E0EE4,fpcPi_Handler_hd);
s32 fpcPi_Init_hd(u32 p,u32 data,u32 layer,u32 list,u32 priority) {
 WWHD_FUNC(0x025E0FB0,s32,p,data,layer,list,priority);
 if(!call<s32>(0x025E0D0C,layer,list,priority))return 0;
 wr<u16>(p,0x3C,(u16)list);wr<u32>(p,0x30,layer);wr<u32>(p,0x38,layer);
 wr<u16>(p,0x3E,(u16)priority);wr<u16>(p,0x34,(u16)list);wr<u16>(p,0x36,(u16)priority);
 call<void>(0x0201A918,p,data);
 call<void>(0x025DFE2C,p+0x14,0x025E0CC0u,p);
 return 1;
}
VERIFY(0x025E0FB0,fpcPi_Init_hd);
void fpcPi_sinit_hd() {
 WWHD_FUNC(0x025E104C,void);
 wr<u32>(0x1048AB30,8,0);wr<u32>(0x1048AB30,0,0);wr<u32>(0x1048AB30,12,0);wr<u32>(0x1048AB30,4,0);
 call<void>(0x028F026C,0x101F3E74u);
 const f32 a=rd<f32>(0x10058454),b=rd<f32>(0x10058458);
 wr<f32>(0x1048AB24,0,a);wr<f32>(0x1048AB28,0,b);
 call<void>(0x028ED6F8,0x1048AB2Cu);call<void>(0x028F026C,0x101F3E80u);
 call<void>(0x028EAB2C,0x1048AB2Du);call<void>(0x028F026C,0x101F3E8Cu);
}
VERIFY(0x025E104C,fpcPi_sinit_hd);
