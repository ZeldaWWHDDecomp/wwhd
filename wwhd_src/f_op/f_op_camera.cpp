#include "wwhd.h"
#include "gabi.h"
namespace {
template<class T> T get(u32 p,u32 o=0) { return *gabi::at<be<T>>(p+o); }
template<class T> void put(u32 p,u32 o,T v) { *gabi::at<be<T>>(p+o)=v; }
}
s32 fopCam_Draw(u32 p) {
    WWHD_FUNC(0x025DA3C4,s32,p);
    if(gabi::call<s32>(0x025986BC)!=0) return 1;
    return gabi::call<s32>(0x025DF2C0,get<u32>(p,0x228),p);
}
VERIFY(0x025DA3C4,fopCam_Draw);
s32 fopCam_Execute(u32 p) {
    WWHD_FUNC(0x025DA418,s32,p);
    if(gabi::call<s32>(0x025986BC)!=0) return 1;
    if(gabi::call<s32>(0x025AF2A4)!=0) return 1;
    return gabi::call<s32>(0x025DFCC4,get<u32>(p,0x228),p);
}
VERIFY(0x025DA418,fopCam_Execute);
s32 fopCam_IsDelete(u32 p) {
    WWHD_FUNC(0x025DA478,s32,p);
    s32 result=gabi::call<s32>(0x025DFCCC,get<u32>(p,0x228),p);
    if(result==1) gabi::call<void>(0x025DA884,p+0x214);
    return result;
}
VERIFY(0x025DA478,fopCam_IsDelete);
s32 fopCam_Delete(u32 p) {
    WWHD_FUNC(0x025DA4CC,s32,p);
    s32 result=gabi::call<s32>(0x025DFCD4,get<u32>(p,0x228),p);
    if(result==1) gabi::call<void>(0x025DA884,p+0x214);
    return result;
}
VERIFY(0x025DA4CC,fopCam_Delete);
s32 fopCam_Create(u32 p) {
    WWHD_FUNC(0x025DA520,s32,p);
    if(get<u8>(p,0xC)==0) {
        u32 profile=get<u32>(p,0x10);
        put<u32>(p,0x228,get<u32>(profile,0x3C));
        gabi::call<void>(0x025DA888,p+0x214,p);
        u32 append=get<u32>(p,0xAC);
        if(append) put<u32>(p,0xB0,get<u32>(append));
    }
    s32 result=gabi::call<s32>(0x025DFCDC,get<u32>(p,0x228),p);
    if(result==4) {
        s32 priority=gabi::call<s32>(0x025DF2B8,p);
        gabi::call<void>(0x025DA874,p+0x214,priority);
    }
    return result;
}
VERIFY(0x025DA520,fopCam_Create);
void fopCam_Initializer() {
    WWHD_FUNC(0x025DA5B8,void);
    put<u32>(0x10487538,8,0); put<u32>(0x10487538,0,0); put<u32>(0x10487538,12,0); put<u32>(0x10487538,4,0);
    gabi::call<void>(0x028F026C,u32(0x101F3334));
    f32 a=get<f32>(0x10057950); f32 b=get<f32>(0x10057954);
    put<f32>(0x1048752C,0,a); put<f32>(0x10487530,0,b);
    gabi::call<void>(0x028ED6F8,u32(0x10487534));
    gabi::call<void>(0x028F026C,u32(0x101F3340));
    gabi::call<void>(0x028EAB2C,u32(0x10487535));
    gabi::call<void>(0x028F026C,u32(0x101F334C));
}
VERIFY(0x025DA5B8,fopCam_Initializer);
