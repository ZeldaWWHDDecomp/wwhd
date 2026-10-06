#include "gabi.h"
using gabi::load; using gabi::store;
u32 cTg_IsUse(void* tag) {
 WWHD_FUNC(0x0201A71C,u32,tag);
 return load<u8>(gabi::ea(tag)+0x10);
}
VERIFY(0x0201A71C,cTg_IsUse);
s32 cTg_SingleCutFromTree(void* tag) {
 WWHD_FUNC(0x0201A724,s32,tag);
 if(load<u8>(gabi::ea(tag)+0x10)!=1) return 0;
 store<u8>(gabi::ea(tag)+0x10,0);
 gabi::call<void>(0x0201AAB0,tag);
 return 1;
}
VERIFY(0x0201A724,cTg_SingleCutFromTree);
s32 cTg_SingleCut(void* tag) {
 WWHD_FUNC(0x0201A868,s32,tag);
 if(load<u8>(gabi::ea(tag)+0x10)!=1) return 0;
 store<u8>(gabi::ea(tag)+0x10,0);
 gabi::call<void>(0x0200FDF4,tag);
 return 1;
}
VERIFY(0x0201A868,cTg_SingleCut);
s32 cTg_AdditionToTree(void* tree,s32 listIndex,void* tag) {
 WWHD_FUNC(0x0201A770,s32,tree,listIndex,tag);
 if(load<u8>(gabi::ea(tag)+0x10)) return 0;
 s32 result=gabi::call<s32>(0x0201AAB4,tree,listIndex,tag);
 if(!result) return 0;
 store<u8>(gabi::ea(tag)+0x10,1);
 return result;
}
VERIFY(0x0201A770,cTg_AdditionToTree);
s32 cTg_InsertToTree(void* tree,s32 listIndex,void* tag,s32 index) {
 WWHD_FUNC(0x0201A7D4,s32,tree,listIndex,tag,index);
 if(load<u8>(gabi::ea(tag)+0x10)) return 0;
 s32 result=gabi::call<s32>(0x0201AADC,tree,listIndex,tag,index);
 if(!result) return 0;
 store<u8>(gabi::ea(tag)+0x10,1);
 return result;
}
VERIFY(0x0201A7D4,cTg_InsertToTree);
s32 cTg_Addition(void* list,void* tag) {
 WWHD_FUNC(0x0201A8B4,s32,list,tag);
 if(load<u8>(gabi::ea(tag)+0x10)) return 0;
 s32 result=gabi::call<s32>(0x0200FE78,list,tag);
 if(!result) return 0;
 store<u8>(gabi::ea(tag)+0x10,1);
 return result;
}
VERIFY(0x0201A8B4,cTg_Addition);
void* cTg_GetFirst(void* list) {
 WWHD_FUNC(0x0201A838,void*,list);
 void* tag=gabi::call<void*>(0x0200FFB0,list);
 if(gabi::ea(tag)) store<u8>(gabi::ea(tag)+0x10,0);
 return tag;
}
VERIFY(0x0201A838,cTg_GetFirst);
void cTg_Create(void* tag,void* data) {
 WWHD_FUNC(0x0201A918,void,tag,data);
 gabi::call<void>(0x02019CA8,tag,nullptr);
 store<u32>(gabi::ea(tag)+0xC,gabi::ea(data));
 store<u8>(gabi::ea(tag)+0x10,0);
}
VERIFY(0x0201A918,cTg_Create);
// HD math/header globals and their registration records.
void cTg_StaticInit() {
 WWHD_FUNC(0x0201A960,void);
 store<u32>(0x101FFB30,0);
 store<u32>(0x101FFB28,0);
 store<u32>(0x101FFB34,0);
 store<u32>(0x101FFB2C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3D0));
 f32 first=load<f32>(0x100036C8);
 f32 second=load<f32>(0x100036CC);
 store<f32>(0x101FFB1C,first);
 store<f32>(0x101FFB20,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFB24));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3DC));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFB25));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3E8));
}
VERIFY(0x0201A960,cTg_StaticInit);
