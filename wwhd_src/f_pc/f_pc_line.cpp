#include "wwhd.h"
#include "gabi.h"
void fpcLn_Create() {
 WWHD_FUNC(0x025DF468,void);
 for(u32 i=0;i<16;++i) gabi::call<void>(0x02010008,gabi::at<void>(0x1048A87C+i*12));
}
VERIFY(0x025DF468,fpcLn_Create);
void f_pc_line_static_init() {
 WWHD_FUNC(0x025DF4B4,void);
 gabi::store<u32>(0x1048A874,0);gabi::store<u32>(0x1048A86C,0);gabi::store<u32>(0x1048A878,0);gabi::store<u32>(0x1048A870,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3BF0));
 f32 lo=gabi::load<f32>(0x1005833C),hi=gabi::load<f32>(0x10058340);
 gabi::store<f32>(0x1048A860,lo);gabi::store<f32>(0x1048A864,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A868));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3BFC));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A869));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3C08));
}
VERIFY(0x025DF4B4,f_pc_line_static_init);
