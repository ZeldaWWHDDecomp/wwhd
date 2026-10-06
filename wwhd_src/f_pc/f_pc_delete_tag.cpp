#include "gabi.h"
using gabi::load; using gabi::store;
s32 fpcDtTg_IsEmpty() {
 WWHD_FUNC(0x025DDE18,s32);
 return load<u32>(0x101F3A24)==0;
}
VERIFY(0x025DDE18,fpcDtTg_IsEmpty);
u32 fpcDtTg_ToDeleteQ(void* tag) {
 WWHD_FUNC(0x025DDE2C,u32,tag);
 store<u16>(gabi::ea(tag)+0x18,1);
 return gabi::call<u32>(0x0201A8B4,gabi::at<void>(0x101F3A1C),tag);
}
VERIFY(0x025DDE2C,fpcDtTg_ToDeleteQ);
s32 fpcDtTg_Do(void* tag,u32 callback) {
 WWHD_FUNC(0x025DDE44,s32,tag,callback);
 u32 a=gabi::ea(tag); s16 timer=load<s16>(a+0x18);
 if(timer>0) { store<u16>(a+0x18,u16(u32(timer)-1)); return 0; }
 gabi::call<void>(0x0201A868,tag);
 void* data=gabi::at<void>(load<u32>(a+0xC));
 if(gabi::call_ptr<s32>(callback,data)==0) { fpcDtTg_ToDeleteQ(tag); return 0; }
 return 1;
}
VERIFY(0x025DDE44,fpcDtTg_Do);
s32 fpcDtTg_Init(void* tag,void* data) {
 WWHD_FUNC(0x025DDEEC,s32,tag,data);
 gabi::call<void>(0x0201A918,tag,data); return 1;
}
VERIFY(0x025DDEEC,fpcDtTg_Init);
void fpcDtTg_StaticInit() {
 WWHD_FUNC(0x025DDF10,void);
 store<u32>(0x1048A75C,0);store<u32>(0x1048A754,0);
 store<u32>(0x1048A760,0);store<u32>(0x1048A758,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F39F8));
 f32 first=load<f32>(0x10058274),second=load<f32>(0x10058278);
 store<f32>(0x1048A748,first);store<f32>(0x1048A74C,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A750));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3A04));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A751));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3A10));
}
VERIFY(0x025DDF10,fpcDtTg_StaticInit);
