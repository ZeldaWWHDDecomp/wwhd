#include "gabi.h"
using namespace gabi;
u32 fpcSch_JudgeForPName_hd(u32 p,u32 data) {
 WWHD_FUNC(0x025E121C,u32,p,data);
 const s16 name=load<s16>(p+8),want=load<s16>(data);
 return name==want ? p : 0;
}
VERIFY(0x025E121C,fpcSch_JudgeForPName_hd);
u32 fpcSch_JudgeByID_hd(u32 p,u32 data) {
 WWHD_FUNC(0x025E1234,u32,p,data);
 const u32 id=load<u32>(p+4),want=load<u32>(data);
 return id==want ? p : 0;
}
VERIFY(0x025E1234,fpcSch_JudgeByID_hd);
void fpcSch_sinit_hd() {
 WWHD_FUNC(0x025E124C,void);
 store<u32>(0x1048AB98,0);store<u32>(0x1048AB90,0);store<u32>(0x1048AB9C,0);store<u32>(0x1048AB94,0);
 call<void>(0x028F026C,0x101F4674u);
 const f32 a=load<f32>(0x1005848C),b=load<f32>(0x10058490);
 store<f32>(0x1048AB84,a);store<f32>(0x1048AB88,b);
 call<void>(0x028ED6F8,0x1048AB8Cu);call<void>(0x028F026C,0x101F4680u);
 call<void>(0x028EAB2C,0x1048AB8Du);call<void>(0x028F026C,0x101F468Cu);
}
VERIFY(0x025E124C,fpcSch_sinit_hd);
