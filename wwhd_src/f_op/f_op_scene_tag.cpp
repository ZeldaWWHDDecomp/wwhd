#include "gabi.h"
using gabi::load;using gabi::store;
void fopScnTg_QueueTo(void* tag) {
 WWHD_FUNC(0x025DCFA4,void,tag);
 gabi::call<void>(0x0201A868,tag);
}
VERIFY(0x025DCFA4,fopScnTg_QueueTo);
u32 fopScnTg_ToQueue(void* tag) {
 WWHD_FUNC(0x025DCFA8,u32,tag);
 return gabi::call<u32>(0x0201A8B4,gabi::at<void>(0x101F387C),tag);
}
VERIFY(0x025DCFA8,fopScnTg_ToQueue);
void fopScnTg_Init(void* tag,void* data) {
 WWHD_FUNC(0x025DCFB8,void,tag,data);
 gabi::call<void>(0x0201A918,tag,data);
}
VERIFY(0x025DCFB8,fopScnTg_Init);
void fopScnTg_StaticInit() {
 WWHD_FUNC(0x025DCFBC,void);
 store<u32>(0x1048A644,0);store<u32>(0x1048A63C,0);
 store<u32>(0x1048A648,0);store<u32>(0x1048A640,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3858));
 f32 first=load<f32>(0x10057DBC),second=load<f32>(0x10057DC0);
 store<f32>(0x1048A630,first);store<f32>(0x1048A634,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A638));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3864));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A639));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3870));
}
VERIFY(0x025DCFBC,fopScnTg_StaticInit);
