#include "gabi.h"
using gabi::load; using gabi::store;
s32 cTgIt_MethodCall(void* tag,void* filter) {
 WWHD_FUNC(0x0201A9F4,s32,tag,filter);
 u32 function=load<u32>(gabi::ea(filter));
 void* userData=gabi::at<void>(load<u32>(gabi::ea(filter)+4));
 void* tagData=gabi::at<void>(load<u32>(gabi::ea(tag)+0xC));
 return gabi::call_ptr<s32>(function,tagData,userData);
}
VERIFY(0x0201A9F4,cTgIt_MethodCall);
void* cTgIt_JudgeFilter(void* tag,void* filter) {
 WWHD_FUNC(0x0201AA08,void*,tag,filter);
 u32 function=load<u32>(gabi::ea(filter));
 void* userData=gabi::at<void>(load<u32>(gabi::ea(filter)+4));
 void* tagData=gabi::at<void>(load<u32>(gabi::ea(tag)+0xC));
 return gabi::call_ptr<void*>(function,tagData,userData);
}
VERIFY(0x0201AA08,cTgIt_JudgeFilter);
// HD math/header globals and their registration records.
void cTgIt_StaticInit() {
 WWHD_FUNC(0x0201AA1C,void);
 store<u32>(0x101FFB4C,0);
 store<u32>(0x101FFB44,0);
 store<u32>(0x101FFB50,0);
 store<u32>(0x101FFB48,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3F4));
 f32 first=load<f32>(0x100036D0);
 f32 second=load<f32>(0x100036D4);
 store<f32>(0x101FFB38,first);
 store<f32>(0x101FFB3C,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFB40));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D400));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFB41));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D40C));
}
VERIFY(0x0201AA1C,cTgIt_StaticInit);
