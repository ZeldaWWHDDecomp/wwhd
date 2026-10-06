#include "wwhd.h"
#include "gabi.h"
namespace {
template<class T> T get(u32 p,u32 o=0) { return *gabi::at<be<T>>(p+o); }
template<class T> void put(u32 p,u32 o,T v) { *gabi::at<be<T>>(p+o)=v; }
}
void screenCalcMtx(u32 p) {
    WWHD_FUNC(0x0259CF48,void,p);
    u32 target=get<u32>(get<u32>(p,0xC8),0x64);
    f32 y=get<f32>(p,0xC),x=get<f32>(p,8);
    gabi::call_ptr<void>(target,p,x,y);
}
VERIFY(0x0259CF48,screenCalcMtx);
u32 myScreenCreatePane(u32 p,u32 header,u32 stream,u32 parent) {
    WWHD_FUNC(0x0259D008,u32,p,header,stream,parent);
    if(get<u32>(header)!=0x50494331) return gabi::call<u32>(0x027F0898,p,header,stream,parent);
    u32 picture=gabi::call<u32>(0x0273AD10,u32(0x138));
    if(picture) {
        gabi::call<void>(0x027EF590,picture,parent,stream);
        put<u8>(picture,0x134,0); put<u32>(picture,0xC8,0x10057C88);
    }
    return picture;
}
VERIFY(0x0259D008,myScreenCreatePane);
void myPictureDestructor(u32 p,u32 flags) {
    WWHD_FUNC(0x0259D0B8,void,p,flags);
    if(p) { gabi::call<void>(0x027F021C,p,u32(0)); if(flags&1) gabi::call<void>(0x0273AF40,p); }
}
VERIFY(0x0259D0B8,myPictureDestructor);
