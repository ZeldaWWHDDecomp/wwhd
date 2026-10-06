#include "wwhd.h"
#include "gabi.h"
namespace {
template<class T>T get(u32 p,u32 o=0){return *gabi::at<be<T>>(p+o);}
template<class T>void put(u32 p,u32 o,T x){*gabi::at<be<T>>(p+o)=x;}
}
void registerInit(u32 p){
 WWHD_FUNC(0x028457C0,void,p);
 put<u32>(p,0x2C,0);put<u16>(p,0xC,0xF0);put<u16>(p,0xE,12);put<u16>(p,0x14,1);put<u32>(p,0x20,0);put<u16>(p,0x16,0x7FFF);put<u16>(p,0x1A,0x40);
 put<u16>(p,2,0);put<u16>(p,0x12,1);put<u16>(p,4,0);put<u16>(p,0x18,0x4000);put<u16>(p,6,0);put<u32>(p,0x28,0);put<u16>(p,8,0);put<u16>(p,0x10,0);put<u16>(p,0xA,0);put<u16>(p,0,0);put<u32>(p,0x24,0);
}
VERIFY(0x028457C0,registerInit);
u8 registerBank(u32 p){WWHD_FUNC(0x02845828,u8,p);return u8(get<u16>(p,0xC)>>8);}
VERIFY(0x02845828,registerBank);
u8 registerProgram(u32 p){WWHD_FUNC(0x02845834,u8,p);return get<u8>(p,0xD);}
VERIFY(0x02845834,registerProgram);
void registerInherit(u32 p,u32 source){
 WWHD_FUNC(0x02845840,void,p,source);
 put<u16>(p,2,0);put<u16>(p,4,0);put<u16>(p,6,0);put<u16>(p,8,0);put<u16>(p,0xA,0);put<u16>(p,0,0);
 put<u16>(p,0xC,get<u16>(source,0xC));put<u16>(p,0xE,get<u16>(source,0xE));put<u16>(p,0x1A,get<u16>(source,0x1A));
 for(u32 i=0;i<5;++i)put<u16>(p,0x10+2*i,get<u16>(source,0x10+2*i));
 put<u32>(p,0x28,0);put<u32>(p,0x24,0);put<u32>(p,0x2C,0);put<u32>(p,0x20,0);
}
VERIFY(0x02845840,registerInherit);
void registerInitializer(){WWHD_FUNC(0x028458AC,void);put<u32>(0x104B6654,8,0);put<u32>(0x104B6654,0,0);put<u32>(0x104B6654,12,0);put<u32>(0x104B6654,4,0);gabi::call<void>(0x028F026C,u32(0x101FC6FC));}
VERIFY(0x028458AC,registerInitializer);
