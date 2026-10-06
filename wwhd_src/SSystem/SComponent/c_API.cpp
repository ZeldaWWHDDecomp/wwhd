#include "gabi.h"
using gabi::load; using gabi::store;
void cAPI_StaticInit() {
 WWHD_FUNC(0x02007ECC,void);
 store<u32>(0x101FF3C8,0);store<u32>(0x101FF3C0,0);
 store<u32>(0x101FF3CC,0);store<u32>(0x101FF3C4,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C4FC));
 f32 first=load<f32>(0x10000B68),second=load<f32>(0x10000B6C);
 store<f32>(0x101FF3B4,first);store<f32>(0x101FF3B8,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF3BC));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C508));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF3BD));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C514));
}
VERIFY(0x02007ECC,cAPI_StaticInit);
