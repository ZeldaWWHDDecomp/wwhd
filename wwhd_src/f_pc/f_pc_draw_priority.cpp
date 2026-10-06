#include "wwhd.h"
#include "gabi.h"
// HD draw priority is a signed halfword at+0; preserve full r4 through Init.
s16 fpcDwPi_Get(void* priority) {
 WWHD_FUNC(0x025DE460,s16,priority);
 return gabi::load<s16>(gabi::ea(priority));
}
VERIFY(0x025DE460,fpcDwPi_Get);
void fpcDwPi_Set(void* priority,u32 value) {
 WWHD_FUNC(0x025DE468,void,priority,value);
 gabi::store<u16>(gabi::ea(priority),u16(value));
}
VERIFY(0x025DE468,fpcDwPi_Set);
void fpcDwPi_Init(void* priority,u32 value) {
 WWHD_FUNC(0x025DE470,void,priority,value);
 fpcDwPi_Set(priority,value);
}
VERIFY(0x025DE470,fpcDwPi_Init);
void f_pc_draw_priority_static_init() {
 WWHD_FUNC(0x025DE474,void);
 gabi::store<u32>(0x1048A7B0,0);gabi::store<u32>(0x1048A7A8,0);gabi::store<u32>(0x1048A7B4,0);gabi::store<u32>(0x1048A7AC,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3A70));
 f32 negativePi=gabi::load<f32>(0x100582C8),positivePi=gabi::load<f32>(0x100582CC);
 gabi::store<f32>(0x1048A79C,negativePi);gabi::store<f32>(0x1048A7A0,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A7A4));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3A7C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A7A5));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3A88));
}
VERIFY(0x025DE474,f_pc_draw_priority_static_init);
