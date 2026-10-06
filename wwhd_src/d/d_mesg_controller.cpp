// HD message controller, proven control vtable subset. Full unit inventory pending.
#include "gabi.h"
using namespace gabi;
u32 messageControlCtor(u32 p){
 WWHD_FUNC(0x025F7424,u32,p);
 if(!p){p=call<u32>(0x0273AD10,0x74u);if(!p)return 0;}
 call<void>(0x028427B8,p);
 store<u32>(p+0x3C,0);store<u32>(p+0x40,0);store<u32>(p+0x38,0x100E0CB8u);
 store<u32>(p+0x54,0);store<u32>(p+0x58,0);
 s32 n=load<s16>(0x1047AFA0u);float z=load<float>(0x100E0C4Cu);store<u32>(p+0x5C,n);
 n=load<s16>(0x1047AFA0u);
 store<float>(p+0x44,z);store<u32>(p+0x60,n);store<u32>(p+0x6C,0x1E6u);
 store<float>(p+0x48,z);store<u8>(p+0x71,0);store<float>(p+0x4C,z);
 store<u32>(p+0x68,0);store<u32>(p+0x64,0);store<u8>(p+0x70,0);store<float>(p+0x50,z);
 return p;
}
VERIFY(0x025F7424,messageControlCtor);
void messageControlMode(u32 p,u32 mode){
 WWHD_FUNC(0x025F74D0,void,p,mode);store<u8>(call<u32>(0x025200D4)+0x5BB2,mode);
}
VERIFY(0x025F74D0,messageControlMode);
void messageControlDtor(u32 p,u32 flags){
 WWHD_FUNC(0x025F9070,void,p,flags);if(p){call<void>(0x02842828,p,0u);if(flags&1)call<void>(0x0273AF40,p);}
}
VERIFY(0x025F9070,messageControlDtor);
