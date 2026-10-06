#include "wwhd.h"
#include "gabi.h"
namespace {
template<class T> T get(u32 p,u32 o=0) { return *gabi::at<be<T>>(p+o); }
template<class T> void put(u32 p,u32 o,T v) { *gabi::at<be<T>>(p+o)=v; }
}
void dBgS_LinChk_Set(u32 p,u32 start,u32 end,u32 actor) {
    WWHD_FUNC(0x024F1AFC,void,p,start,end,actor);
    u32 id=actor ? get<u32>(actor,4) : 0xFFFFFFFF;
    gabi::call<void>(0x0200909C,p,start,end,id);
}
VERIFY(0x024F1AFC,dBgS_LinChk_Set);
void dBgS_LinChk_Initializer() {
    WWHD_FUNC(0x024F1B1C,void);
    put<u32>(0x1046ED50,8,0); put<u32>(0x1046ED50,0,0); put<u32>(0x1046ED50,12,0); put<u32>(0x1046ED50,4,0);
    gabi::call<void>(0x028F026C,u32(0x101D5294));
    f32 a=get<f32>(0x10043B98); f32 b=get<f32>(0x10043B9C);
    put<f32>(0x1046ED44,0,a); put<f32>(0x1046ED48,0,b);
    gabi::call<void>(0x028ED6F8,u32(0x1046ED4C));
    gabi::call<void>(0x028F026C,u32(0x101D52A0));
    gabi::call<void>(0x028EAB2C,u32(0x1046ED4D));
    gabi::call<void>(0x028F026C,u32(0x101D52AC));
}
VERIFY(0x024F1B1C,dBgS_LinChk_Initializer);
