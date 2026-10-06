#include "wwhd.h"
#include "gabi.h"
// HD method tag:20-byte create_tag, callback+14, callback data+18; size1C.
s32 fpcMtdTg_Do(void* tag) {
 WWHD_FUNC(0x025DFE14,s32,tag);
 u32 a=gabi::ea(tag),target=gabi::load<u32>(a+0x14),data=gabi::load<u32>(a+0x18);
 return gabi::call<s32>(target,gabi::at<void>(data));
}
VERIFY(0x025DFE14,fpcMtdTg_Do);
s32 fpcMtdTg_ToMethodQ(void* list,void* tag) {
 WWHD_FUNC(0x025DFE24,s32,list,tag);
 return gabi::call<s32>(0x0201A8B4,list,tag);
}
VERIFY(0x025DFE24,fpcMtdTg_ToMethodQ);
void fpcMtdTg_MethodQTo(void* tag) {
 WWHD_FUNC(0x025DFE28,void,tag);
 gabi::call<s32>(0x0201A868,tag);
}
VERIFY(0x025DFE28,fpcMtdTg_MethodQTo);
s32 fpcMtdTg_Init(void* tag,u32 method,u32 data) {
 WWHD_FUNC(0x025DFE2C,s32,tag,method,data);
 gabi::call<void>(0x0201A918,tag,tag);
 gabi::store<u32>(gabi::ea(tag)+0x14,method);gabi::store<u32>(gabi::ea(tag)+0x18,data);
 return 1;
}
VERIFY(0x025DFE2C,fpcMtdTg_Init);
void f_pc_method_tag_static_init() {
 WWHD_FUNC(0x025DFE80,void);
 gabi::store<u32>(0x1048AAAC,0);gabi::store<u32>(0x1048AAA4,0);gabi::store<u32>(0x1048AAB0,0);gabi::store<u32>(0x1048AAA8,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3CF4));
 f32 negativePi=gabi::load<f32>(0x100583D8),positivePi=gabi::load<f32>(0x100583DC);
 gabi::store<f32>(0x1048AA98,negativePi);gabi::store<f32>(0x1048AA9C,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048AAA0));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3D00));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048AAA1));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3D0C));
}
VERIFY(0x025DFE80,f_pc_method_tag_static_init);
