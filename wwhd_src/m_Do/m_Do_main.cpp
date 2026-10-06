/* Full retained HD main TU: LOAD_COPYDATE, init, single frame, initializer.
 * Periodic heap checker025F1654 belongs m_Do_machine. */
#include "bindings.h"
namespace m_Do_main_cpp {
static u32 ld(u32 a){return gabi::load<u32>(a);}
static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static void sb(u32 a,u8 v){gabi::store<u8>(a,v);}
static s32 LOAD_COPYDATE(){
    WWHD_FUNC(0x025F1658,s32,(u32)0);
    return 1;
}
VERIFY(0x025F1658,LOAD_COPYDATE);
static void main01(){
    WWHD_FUNC(0x025F1660,void,(u32)0);
    st(0x1048D090,0);
    st(0x1048D09C,0xFFFFFFFF);
    st(0x1048D098,0);
    st(0x101F4974,0x1048D090);
    st(0x1048D094,0);
    u32 game=gabi::call<u32>(0x025200D4);
    gabi::call(0x02520020,game);
    sb(0x101F48C0,1);
    gabi::call(0x025F12F8);
    u32 heap=gabi::call<u32>(0x025E2FBC);
    u32 previous=1;
    if(heap) previous=gabi::call<u32>(0x02756170,ld(0x101F8B4C),heap);
    gabi::call(0x025F0A08);
    gabi::call(0x025F0550);
    gabi::call(0x025F2D70);
    gabi::call(0x025E2384,0x025F1658,0);
    gabi::call(0x025D4320);
    sb(0x1048D07C,0);
    if(previous!=1) gabi::call(0x02756170,ld(0x101F8B4C),previous);
}
VERIFY(0x025F1660,main01);
static void frame(){
    WWHD_FUNC(0x025F172C,void,(u32)0);
    u32 count=ld(0x1048D0A8)+1;
    u8 period=gabi::load<u8>(0x1048D0AC);
    st(0x1048D0A8,count);
    if(period!=0 && count%period==0) gabi::call(0x025F1654);
    gabi::call(0x025F2D74);
    gabi::call(0x025E15E0);
    gabi::call(0x025D42EC);
}
VERIFY(0x025F172C,frame);
static void initializer(){
    WWHD_FUNC(0x025F1788,void,(u32)0);
    st(0x1048D088,0);
    st(0x1048D080,0);
    st(0x1048D08C,0);
    st(0x1048D084,0);
    gabi::call(0x028F026C,0x101F489C);
    f32 a=gabi::load<f32>(0x10059038);
    f32 b=gabi::load<f32>(0x1005903C);
    gabi::store<f32>(0x1048D074,a);
    gabi::store<f32>(0x1048D078,b);
    gabi::call(0x028ED6F8,0x1048D07D);
    gabi::call(0x028F026C,0x101F48A8);
    gabi::call(0x028EAB2C,0x1048D07E);
    gabi::call(0x028F026C,0x101F48B4);
}
VERIFY(0x025F1788,initializer);
}
