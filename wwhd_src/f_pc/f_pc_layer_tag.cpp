#include "gabi.h"
using gabi::load; using gabi::store;
s32 fpcLyTg_ToQueue(void* tag,u32 layerID,u32 listID,u32 priority) {
 WWHD_FUNC(0x025DEF68,s32,tag,layerID,listID,priority);
 u32 a=gabi::ea(tag),layer=load<u32>(a+0x14);
 if(layerID==0xFFFFFFFFu && !layer) return 0;
 if(layerID==0xFFFFFFFFu || layerID==0xFFFFFFFDu) {
  s32 index=gabi::call<s32>(0x025DEA20,gabi::at<void>(layer),listID,tag);
  if(!index) return 0;
  store<u16>(a+0x18,listID);
  store<u16>(a+0x1A,u16(u32(index)-1u));
  return 1;
 }
 if(!layer || load<u32>(layer+0xC)!=layerID) {
  layer=gabi::ea(gabi::call<void*>(0x025DEAC0,layerID));
  store<u32>(a+0x14,layer);
 }
 s32 result=gabi::call<s32>(0x025DEA18,gabi::at<void>(layer),listID,tag,priority);
 if(!result) return 0;
 store<u16>(a+0x1A,priority);
 store<u16>(a+0x18,listID);
 return 1;
}
VERIFY(0x025DEF68,fpcLyTg_ToQueue);
s32 fpcLyTg_QueueTo(void* tag) {
 WWHD_FUNC(0x025DF084,s32,tag);
 u32 a=gabi::ea(tag),layer=load<u32>(a+0x14);
 if(gabi::call<s32>(0x025DEA28,gabi::at<void>(layer),tag)!=1) return 0;
 store<u32>(a+0x14,0);
 store<u16>(a+0x1A,0xFFFF);
 store<u16>(a+0x18,0xFFFF);
 return 1;
}
VERIFY(0x025DF084,fpcLyTg_QueueTo);
s32 fpcLyTg_Move(void* tag,u32 layerID,u32 listID,u32 priority) {
 WWHD_FUNC(0x025DF0F4,s32,tag,layerID,listID,priority);
 void* layer=gabi::call<void*>(0x025DEAC0,layerID);
 if(!gabi::ea(layer)) return 0;
 if(fpcLyTg_QueueTo(tag)!=1) return 0;
 store<u32>(gabi::ea(tag)+0x14,gabi::ea(layer));
 return fpcLyTg_ToQueue(tag,layerID,listID,priority);
}
VERIFY(0x025DF0F4,fpcLyTg_Move);
s32 fpcLyTg_Init(void* tag,u32 layerID,void* data) {
 WWHD_FUNC(0x025DF178,s32,tag,layerID,data);
 u32 a=gabi::ea(tag);
 for(u32 offset=0;offset<0x1C;offset+=4) store<u32>(a+offset,load<u32>(0x101F3B74+offset));
 gabi::call<void>(0x0201A918,tag,data);
 void* layer=gabi::call<void*>(0x025DEAC0,layerID);
 if(!gabi::ea(layer)) return 0;
 store<u32>(a+0x14,gabi::ea(layer));
 return 1;
}
VERIFY(0x025DF178,fpcLyTg_Init);
// HD math/header globals and their registration records.
void fpcLyTg_StaticInit() {
 WWHD_FUNC(0x025DF224,void);
 store<u32>(0x1048A83C,0);
 store<u32>(0x1048A834,0);
 store<u32>(0x1048A840,0);
 store<u32>(0x1048A838,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3B90));
 f32 first=load<f32>(0x10058310);
 f32 second=load<f32>(0x10058314);
 store<f32>(0x1048A828,first);
 store<f32>(0x1048A82C,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A830));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3B9C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A831));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3BA8));
}
VERIFY(0x025DF224,fpcLyTg_StaticInit);
