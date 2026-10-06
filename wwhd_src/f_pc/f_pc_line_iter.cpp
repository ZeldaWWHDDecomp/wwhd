#include "gabi.h"
using gabi::load; using gabi::store;
s32 fpcLnIt_MethodCall(void* tag,void* filter) {
 WWHD_FUNC(0x025DF548,s32,tag,filter);
 u32 process=load<u32>(gabi::ea(tag)+0xC);
 void* layer=gabi::at<void>(load<u32>(process+0x2C));
 void* previous=gabi::call<void*>(0x025DED64);
 gabi::call<void>(0x025DEAB4,layer);
 s32 result=gabi::call<s32>(0x0201A9F4,tag,filter);
 gabi::call<void>(0x025DEAB4,previous);
 return result;
}
VERIFY(0x025DF548,fpcLnIt_MethodCall);
void fpcLnIt_Queue(u32 callback) {
 WWHD_FUNC(0x025DF5C0,void,callback);
 gabi::Local<u32[2]> filter;
 store<u32>(gabi::ea(filter.get()),callback);
 store<u32>(gabi::ea(filter.get())+4,0);
 gabi::call<void>(0x0201ABF0,gabi::at<void>(0x101F3C14),u32(0x025DF548),filter.get());
}
VERIFY(0x025DF5C0,fpcLnIt_Queue);
void fpcLnIt_StaticInit() {
 WWHD_FUNC(0x025DF600,void);
 store<u32>(0x1048A950,0);store<u32>(0x1048A948,0);
 store<u32>(0x1048A954,0);store<u32>(0x1048A94C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3C1C));
 f32 first=load<f32>(0x10058354),second=load<f32>(0x10058358);
 store<f32>(0x1048A93C,first);store<f32>(0x1048A940,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A944));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3C28));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A945));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3C34));
}
VERIFY(0x025DF600,fpcLnIt_StaticInit);
