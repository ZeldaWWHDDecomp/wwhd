#include "gabi.h"
using gabi::load; using gabi::store;
void* fopScnIt_Judge(u32 callback,void* data) {
 WWHD_FUNC(0x025DC73C,void*,callback,data);
 gabi::Local<u32[2]> filter;
 store<u32>(gabi::ea(filter.get()),callback);
 store<u32>(gabi::ea(filter.get())+4,gabi::ea(data));
 return gabi::call<void*>(0x020100BC,gabi::at<void>(0x101F387C),u32(0x0201AA08),filter.get());
}
VERIFY(0x025DC73C,fopScnIt_Judge);
void fopScnIt_StaticInit() {
 WWHD_FUNC(0x025DC778,void);
 store<u32>(0x1048A5D4,0);store<u32>(0x1048A5CC,0);
 store<u32>(0x1048A5D8,0);store<u32>(0x1048A5D0,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3770));
 f32 first=load<f32>(0x10057D54),second=load<f32>(0x10057D58);
 store<f32>(0x1048A5C0,first);store<f32>(0x1048A5C4,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A5C8));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F377C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A5C9));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3788));
}
VERIFY(0x025DC778,fopScnIt_StaticInit);
