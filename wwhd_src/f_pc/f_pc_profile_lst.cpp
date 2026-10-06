#include "gabi.h"
using namespace gabi;
void fpcPfLst_sinit_hd() {
 WWHD_FUNC(0x025E1188,void);
 store<u32>(0x1048AB7C,0);store<u32>(0x1048AB74,0);store<u32>(0x1048AB80,0);store<u32>(0x1048AB78,0);
 call<void>(0x028F026C,0x101F3EBCu);
 const f32 a=load<f32>(0x10058478),b=load<f32>(0x1005847C);
 store<f32>(0x1048AB68,a);store<f32>(0x1048AB6C,b);
 call<void>(0x028ED6F8,0x1048AB70u);call<void>(0x028F026C,0x101F3EC8u);
 call<void>(0x028EAB2C,0x1048AB71u);call<void>(0x028F026C,0x101F3ED4u);
}
VERIFY(0x025E1188,fpcPfLst_sinit_hd);
