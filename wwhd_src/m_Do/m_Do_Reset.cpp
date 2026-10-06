// TU attribution inferred from HD adjacency (after the m_Do_audio companions 20F0/20F8/2100; after the printf initializer 28C8, before vibration 29F0); not proven by symbols or strings
#include "gabi.h"
using namespace gabi;
void mDoReset_sinit_hd() {
 WWHD_FUNC(0x025F295C,void);
 store<u32>(0x1048D554,0);store<u32>(0x1048D54C,0);store<u32>(0x1048D558,0);store<u32>(0x1048D550,0);
 call<void>(0x028F026C,0x101F4950u);
 const f32 a=load<f32>(0x100590DC),b=load<f32>(0x100590E0);
 store<f32>(0x1048D540,a);store<f32>(0x1048D544,b);
 call<void>(0x028ED6F8,0x1048D548u);call<void>(0x028F026C,0x101F495Cu);
 call<void>(0x028EAB2C,0x1048D549u);call<void>(0x028F026C,0x101F4968u);
}
VERIFY(0x025F295C,mDoReset_sinit_hd);
