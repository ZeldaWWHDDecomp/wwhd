/* m_Do_graphic: WWHD graphics state TU025F03C4..025F0943,17entries.
 * Includes Before/After draw, heap initialization,
 * display wrappers, blur/fade/monotone state and adjacent header-static initializer.
 * GC renderer/capture routines are absent from this HD body. External calls mocked. */
#include "bindings.h"
namespace m_Do_graphic_cpp {
static u32 ld(u32 a){return gabi::load<u32>(a);}
static u8 lb(u32 a){return gabi::load<u8>(a);}
static s16 lh(u32 a){return gabi::load<s16>(a);}
static f32 lf(u32 a){return gabi::load<f32>(a);}
static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static void sb(u32 a,u8 v){gabi::store<u8>(a,v);}
static void sf(u32 a,f32 v){gabi::store<f32>(a,v);}
static BOOL BeforeOfDraw(){
    WWHD_FUNC(0x025F03C4,BOOL,(u32)0);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0252F264,play+0x5D30);
    return TRUE;
}
VERIFY(0x025F03C4,BeforeOfDraw);
static BOOL AfterOfDraw(){
    WWHD_FUNC(0x025F03F0,BOOL,(u32)0);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0252E388,play+0x60EC);
    return TRUE;
}
VERIFY(0x025F03F0,AfterOfDraw);
static void createHeap(){
    WWHD_FUNC(0x025F041C,void,(u32)0);
    u32 parent=gabi::call<u32>(0x027EC230);
    u32 first=gabi::call<u32>(0x027EC464,0x10000,parent,0);
    st(0x101F4808,first);
    if(!first) gabi::call(0x0273AA24,0x10058EF4,0xFB,0x10058EAC);
    u32 second=gabi::call<u32>(0x027EC464,0x10000,parent,0);
    st(0x101F480C,second);
    if(!second) gabi::call(0x0273AA24,0x10058EF4,0xFD,0x10058EBC);
    st(ld(0x101F4808)+0x10,0x10058ECC);
    st(ld(0x101F480C)+0x10,0x10058EE0);
    sb(0x101F4824,0);
}
VERIFY(0x025F041C,createHeap);
static void create(){
    WWHD_FUNC(0x025F04EC,void,(u32)0);
    gabi::call(0x025F041C);
    gabi::call(0x027F0148,0x400);
    sb(0x101F4825,0);
    sb(0x101F4827,0);
    u32 color=ld(0x101D5E90);
    st(0x101F4818,color);
    st(0x101F481C,color);
}
VERIFY(0x025F04EC,create);
static BOOL Create(){
    WWHD_FUNC(0x025F0550,BOOL,(u32)0);
    u32 heap=gabi::call<u32>(0x025E35BC,0,0,0);
    if(!heap) gabi::call(0x0273AA24,0x10058F14,0xD1C,0x10058F08);
    st(heap+0x10,0x10058F28);
    gabi::call(0x025F04EC);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0252ED28,play+0x5D30);
    gabi::call(0x025E3678,heap);
    gabi::call(0x025E37D8);
    return TRUE;
}
VERIFY(0x025F0550,Create);
static u32 displayControl(u32 first,u32 second){
    WWHD_FUNC(0x025F05D0,u32,first,second);
    return gabi::call<u32>(0x02728CB8,ld(0x101F86B4),first,second);
}
VERIFY(0x025F05D0,displayControl);
static void displayControlOne(u32 first){
    WWHD_FUNC(0x025F05E8,void,first);
    gabi::call(0x02728CD4,ld(0x101F86B4),first);
}
VERIFY(0x025F05E8,displayControlOne);
static void setDisplayColor(u32 color){
    WWHD_FUNC(0x025F05F8,void,color);
    u32 display=ld(0x101F86B4);
    u32 value=ld(color);
    u32 fader=ld(display+0x440);
    st(fader+8,value);
}
VERIFY(0x025F05F8,setDisplayColor);
static void onBlurMatrix(u32 matrix){
    WWHD_FUNC(0x025F0634,void,matrix);
    sb(0x101F4825,1);
    gabi::call(0x028E90D4,matrix,0x1048CF84);
}
VERIFY(0x025F0634,onBlurMatrix);
static void onBlur(){
    WWHD_FUNC(0x025F064C,void,(u32)0);
    gabi::call(0x025F0634,0x101F48F0);
}
VERIFY(0x025F064C,onBlur);
static void fadeOut(f32 speed,u32 color){
    WWHD_FUNC(0x025F0658,void,speed,color);
    sb(0x101F4827,1);
    sf(0x101F4814,speed);
    u32 value=ld(color);
    st(0x101F481C,value);
    sf(0x101F4810,speed>=0.0f?0.0f:1.0f);
}
VERIFY(0x025F0658,fadeOut);
static void fadeOutDefault(f32 speed){
    WWHD_FUNC(0x025F069C,void,speed);
    gabi::call(0x025F0658,speed,0x101D5E90);
}
VERIFY(0x025F069C,fadeOutDefault);
static void calcFade(){
    WWHD_FUNC(0x025F06A8,void,(u32)0);
    if(lb(0x101F4827)) {
        f32 speed=lf(0x101F4814);
        f32 rate=gabi::fadds_ppc(lf(0x101F4810),speed);
        if(rate<0.0f){rate=0.0f;sf(0x101F4810,rate);sb(0x101F4827,0);}
        else if(rate>1.0f){rate=1.0f;sf(0x101F4810,rate);}
        else sf(0x101F4810,rate);
        u8 alpha=(u8)gabi::ftoi(gabi::fmuls_ppc(255.0f,rate));
        sb(0x101F481F,alpha);
        if(!alpha) return;
    } else {
        u32 play=gabi::call<u32>(0x025200D4);
        if(lb(play+0x62F1)==255){sb(0x101F481F,0);return;}
        sb(0x101F481C,0);sb(0x101F481D,0);sb(0x101F481E,0);
        play=gabi::call<u32>(0x025200D4);
        u8 alpha=255-lb(play+0x62F1);
        sb(0x101F481F,alpha);
        if(!alpha) return;
    }
    gabi::call(0x027291FC,ld(0x101F86B4));
}
VERIFY(0x025F06A8,calcFade);
static void onMonotone(){
    WWHD_FUNC(0x025F0820,void,(u32)0);
    sb(0x101F4829,1);
}
VERIFY(0x025F0820,onMonotone);
static void offMonotone(){
    WWHD_FUNC(0x025F0830,void,(u32)0);
    sb(0x101F4829,0);
}
VERIFY(0x025F0830,offMonotone);
static void calcMonotone(){
    WWHD_FUNC(0x025F0840,void,(u32)0);
    s16 speed=lh(0x101F4822);
    s32 target=speed<0?400:-600;
    s32 step=(s16)(speed<0?-speed:speed);
    s32 complete=gabi::call<s32>(0x0200F564,0x101F4820,target,step);
    if(complete && lh(0x101F4822)>0) gabi::call(0x025F0830);
}
VERIFY(0x025F0840,calcMonotone);
static void sinit(){
    WWHD_FUNC(0x025F08B0,void,(u32)0);
    for(int i=0;i<4;++i) st(0x1048CF74+4*i,0);
    gabi::call(0x028F026C,0x101F47E4);
    sf(0x1048CF68,lf(0x10058F58));
    sf(0x1048CF6C,lf(0x10058F5C));
    gabi::call(0x028ED6F8,0x1048CF72);
    gabi::call(0x028F026C,0x101F47F0);
    gabi::call(0x028EAB2C,0x1048CF73);
    gabi::call(0x028F026C,0x101F47FC);
}
VERIFY(0x025F08B0,sinit);

/* 025F0944 graphics interface table entry 1018C4AC (probably mDoGph_BlankingON, empty as on GameCube): empty function */
static void mDoGph_interface_empty_025F0944() {
    WWHD_FUNC(0x025F0944, void);
}
VERIFY(0x025F0944, mDoGph_interface_empty_025F0944);

/* 025F0948 graphics interface table entry 1018C4B0 (probably mDoGph_BlankingOFF, empty as on GameCube): empty function */
static void mDoGph_interface_empty_025F0948() {
    WWHD_FUNC(0x025F0948, void);
}
VERIFY(0x025F0948, mDoGph_interface_empty_025F0948);

/* 025F094C graphics interface table entry 1018C4A8 (probably mDoGph_Create): returns 1 */
static u32 mDoGph_interface_025F094C() {
    WWHD_FUNC(0x025F094C, u32);
    return 1;
}
VERIFY(0x025F094C, mDoGph_interface_025F094C);

/* 025F0954 this TU's sead::SafeString copy (vtable 10058E94 in this TU's rodata): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void SafeString_deletingDtor_m_Do_graphic(u32 p, u32 flags) {
    WWHD_FUNC(0x025F0954, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x025F0954, SafeString_deletingDtor_m_Do_graphic);

/* 025F0968 empty HD graphics hook called by the logo scene (d_s_logo phase_2): empty function */
static void mDoGph_empty_025F0968() {
    WWHD_FUNC(0x025F0968, void);
}
VERIFY(0x025F0968, mDoGph_empty_025F0968);

/* 025F096C empty HD graphics hook called by the logo/menu/name/play scenes: empty function */
static void mDoGph_empty_025F096C() {
    WWHD_FUNC(0x025F096C, void);
}
VERIFY(0x025F096C, mDoGph_empty_025F096C);

/* 025F0970 this TU's sead::SafeString copy assureTerminationImpl_ (vtable 10058E94): empty function */
static void SafeString_assureTerminationImpl_m_Do_graphic(u32 p) {
    WWHD_FUNC(0x025F0970, void, p);
}
VERIFY(0x025F0970, SafeString_assureTerminationImpl_m_Do_graphic);

}
