#include "gabi.h"
using gabi::load; using gabi::store;
void cAPIGph_BeforeOfDraw() {
 WWHD_FUNC(0x02007E18,void);
 gabi::call_ptr<void>(load<u32>(0x1018C4A0));
}
VERIFY(0x02007E18,cAPIGph_BeforeOfDraw);
void cAPIGph_AfterOfDraw() {
 WWHD_FUNC(0x02007E28,void);
 gabi::call_ptr<void>(load<u32>(0x1018C4A4));
}
VERIFY(0x02007E28,cAPIGph_AfterOfDraw);
void cAPIGph_StaticInit() {
 WWHD_FUNC(0x02007E38,void);
 store<u32>(0x101FF3AC,0);store<u32>(0x101FF3A4,0);
 store<u32>(0x101FF3B0,0);store<u32>(0x101FF3A8,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C4D8));
 f32 first=load<f32>(0x10000B5C),second=load<f32>(0x10000B60);
 store<f32>(0x101FF398,first);store<f32>(0x101FF39C,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF3A0));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C4E4));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF3A1));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C4F0));
}
VERIFY(0x02007E38,cAPIGph_StaticInit);
