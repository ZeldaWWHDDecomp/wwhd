#include "bindings.h"
namespace {
static u32 ld(u32 a){return gabi::load<u32>(a);}
static s16 sh(u32 a){return gabi::load<s16>(a);}
static u8 lb(u32 a){return gabi::load<u8>(a);}
static f32 lf(u32 a){return gabi::load<f32>(a);}
static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static void ss(u32 a,u16 v){gabi::store<u16>(a,v);}
static void sb(u32 a,u8 v){gabi::store<u8>(a,v);}
static void sf(u32 a,f32 v){gabi::store<f32>(a,v);}
static void phase(u32 self,u32 f){ss(self+0xD0,0);ss(self+0xD2,0xFFFF);st(self+0xD4,f);}
static BOOL Execute(u32 self){
    WWHD_FUNC(0x025A2514,BOOL,self);
    s16 index=sh(self+0xD2);u32 receiver=self+(s32)sh(self+0xD0);
    u32 target=index<0?ld(self+0xD4):ld(ld(receiver+(s32)sh(self+0xD6))+(u32)(s32)index*8+4);
    return gabi::call_ptr<BOOL>(target,receiver);
}
VERIFY(0x025A2514,Execute);
static BOOL Delete(u32 self){
    WWHD_FUNC(0x025A2558,BOOL,self);
    if(self){gabi::call(0x0252CCFC,self+0x14C,0);gabi::call(0x0252CCFC,self+0xDC,0);gabi::call(0x0252CCFC,self+0xD8,0);gabi::call(0x025DD630,self,0);}
    f32 value=lf(0x1005192C);u32 renderer=ld(0x101F9968);sb(0x101D616C,0);sf(0x101D6160,value);
    gabi::call(0x027F88C8,renderer,4);gabi::call(0x02728EE0,ld(0x101F86B4));return TRUE;
}
VERIFY(0x025A2558,Delete);
static u32 constructor(u32 self){
    WWHD_FUNC(0x025A25F0,u32,self);
    if(!self)self=gabi::call<u32>(0x0273AD10,0x1C0);
    if(self){gabi::call(0x025DD5F0,self);st(self+0xB4,0x1005191C);gabi::call(0x0252CCBC,self+0xD8);st(self+0xD8,0x10051940);
        gabi::call(0x0252CCBC,self+0xDC);st(self+0xDC,0x1004CBC0);gabi::call(0x0252CCBC,self+0x14C);
        phase(self,0x025A26D8);st(self+0x14C,0x1004CBC0);gabi::call(0x02728DE8,ld(0x101F86B4),self);
    }return self;
}
VERIFY(0x025A25F0,constructor);
static u32 Create(u32 self){
    WWHD_FUNC(0x025A26AC,u32,self);
    if(self)gabi::call(0x025A25F0,self);return 4;
}
VERIFY(0x025A26AC,Create);
static BOOL firstSnap(u32 self){
    WWHD_FUNC(0x025A26D8,BOOL,self);
    if(lb(self+0x1BC)){phase(self,0x025A2750);gabi::call(0x025DBDDC,self);sb(gabi::call<u32>(0x025200D4)+0x5AC9,0);
        f32 a=lf(0x1047BA0C),b=lf(0x10051930);gabi::call(0x0252F590,-gabi::fadds_ppc(a,b));}
    return TRUE;
}
VERIFY(0x025A26D8,firstSnap);
static BOOL fadeOut(u32 self){
    WWHD_FUNC(0x025A2750,BOOL,self);
    sb(gabi::call<u32>(0x025200D4)+0x5AC9,0);
    if(lb(0x101D616C)&&lf(0x101D6160)==lf(0x10051934)&&gabi::call<u32>(0x025DBDC4,self)){
        gabi::call(0x025DBD74);phase(self,0x025A27E4);
    }return TRUE;
}
VERIFY(0x025A2750,fadeOut);
static BOOL nextSnap(u32 self){
    WWHD_FUNC(0x025A27E4,BOOL,self);
    ss(self+0xD2,0xFFFF);st(self+0xD4,0x025A2850);sb(self+0x1BC,0);ss(self+0xD0,0);
    u32 play=gabi::call<u32>(0x025200D4);f32 a=lf(0x10051930);sb(play+0x5AC9,1);f32 b=lf(0x1047BA0C);
    gabi::call(0x0252F590,gabi::fadds_ppc(a,b));return TRUE;
}
VERIFY(0x025A27E4,nextSnap);
static BOOL fadeIn(u32 self){
    WWHD_FUNC(0x025A2850,BOOL,self);
    gabi::call(0x025DBD24);sb(gabi::call<u32>(0x025200D4)+0x5AC9,0);
    if(!lb(0x101D616C)){gabi::call(0x025DBDDC,self);sb(gabi::call<u32>(0x025200D4)+0x5AC9,1);}
    return TRUE;
}
VERIFY(0x025A2850,fadeIn);
static void nativeDraw(u32 self,u32 context){
    WWHD_FUNC(0x025A28B0,void,self,context);
    u8 first=lb(self+0x1BC);u32 renderer=ld(0x101F9968);
    if(!first){gabi::call(0x027F84A4,renderer,4,context);sb(self+0x1BC,1);return;}
    gabi::call(0x027F8974,renderer,4,context,1);
}
VERIFY(0x025A28B0,nativeDraw);
static void sinit(){
    WWHD_FUNC(0x025A291C,void,(u32)0);
    st(0x1047B130,0);st(0x1047B128,0);st(0x1047B134,0);st(0x1047B12C,0);
    gabi::call(0x028F026C,0x101EA34C);f32 a=lf(0x10051938),b=lf(0x1005193C);sf(0x1047B11C,a);sf(0x1047B120,b);
    gabi::call(0x028ED6F8,0x1047B124);gabi::call(0x028F026C,0x101EA358);gabi::call(0x028EAB2C,0x1047B125);gabi::call(0x028F026C,0x101EA364);
}
VERIFY(0x025A291C,sinit);
static BOOL Draw(){
    WWHD_FUNC(0x025A29B0,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x025A29B0,Draw);
static BOOL IsDelete(){
    WWHD_FUNC(0x025A29B8,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x025A29B8,IsDelete);
static void packetStub(){
    WWHD_FUNC(0x025A29C0,void,(u32)0);
}
VERIFY(0x025A29C0,packetStub);
}
