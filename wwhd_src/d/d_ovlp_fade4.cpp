#include "bindings.h"
namespace {
static u32 ld(u32 a){return gabi::load<u32>(a);}
static s16 sh(u32 a){return gabi::load<s16>(a);}
static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static void ss(u32 a,u16 v){gabi::store<u16>(a,v);}
static u8 lb(u32 a){return gabi::load<u8>(a);}
static void sb(u32 a,u8 v){gabi::store<u8>(a,v);}
static void sf(u32 a,f32 v){gabi::store<f32>(a,v);}
static f32 lf(u32 a){return gabi::load<f32>(a);}
static void phase(u32 self,u32 f){ss(self+0xD0,0);ss(self+0xD2,0xFFFF);st(self+0xD4,f);}
static u32 spriteCtor(u32 self){
    WWHD_FUNC(0x025A2B88,u32,self);
    if(!self)self=gabi::call<u32>(0x0273AD10,0x6C);
    if(self){gabi::call(0x0252CCBC,self);st(self,0x100519C0);gabi::call(0x028F521C,self+4,64);
        f32 zero=lf(0x10051974);st(self+0x44,0);sf(self+0x5C,zero);st(self+0x4C,0);sf(self+0x60,zero);st(self+0x50,0);sf(self+0x54,zero);sf(self+0x58,zero);
        st(self+0x48,0);gabi::call(0x028F521C,self+0x64,4);sf(self+0x68,zero);
    }return self;
}
VERIFY(0x025A2B88,spriteCtor);
static u32 effectCtor(u32 self){
    WWHD_FUNC(0x025A2C3C,u32,self);
    if(!self)self=gabi::call<u32>(0x0273AD10,0x178);
    if(self){gabi::call(0x0252CCBC,self);f32 zero=lf(0x10051974);sb(self+8,0);sf(self+4,zero);st(self,0x100519D8);
        gabi::call(0x028F521C,self+12,12);gabi::call(0x0252CCBC,self+12);st(self+12,0x100519A8);gabi::call(0x025A2B88,self+0x18);
        gabi::call(0x028F521C,self+0x84,12);gabi::call(0x0252CCBC,self+0x84);st(self+0x84,0x100519A8);gabi::call(0x025A2B88,self+0x90);
        gabi::call(0x028F521C,self+0xFC,12);gabi::call(0x0252CCBC,self+0xFC);st(self+0xFC,0x100519A8);gabi::call(0x025A2B88,self+0x108);
        ss(self+0x176,0);ss(self+0x174,0);
    }return self;
}
VERIFY(0x025A2C3C,effectCtor);
static u32 sceneCtor(u32 self){
    WWHD_FUNC(0x025A2D24,u32,self);
    if(!self)self=gabi::call<u32>(0x0273AD10,0x370);
    if(self){gabi::call(0x025DD5F0,self);st(self+0xB4,0x10051960);
        gabi::call(0x028F521C,self+0xD0,8);gabi::call(0x028F521C,self+0xD8,8);
        gabi::call(0x028F521C,self+0xE0,12);gabi::call(0x0252CCBC,self+0xE0);st(self+0xE0,0x100519A8);
        gabi::call(0x028F521C,self+0xEC,12);gabi::call(0x0252CCBC,self+0xEC);st(self+0xEC,0x100519A8);
        gabi::call(0x025A2B88,self+0xF8);gabi::call(0x025A2B88,self+0x164);gabi::call(0x025A2C3C,self+0x1D0);
        sb(self+0x348,1);f32 zero=lf(0x10051974);st(self+0x36C,0);sf(self+0x354,zero);sf(self+0x350,zero);sf(self+0x364,zero);
        st(self+0xD4,0x025A32E8);ss(self+0xDA,0xFFFF);st(self+0xDC,0x025A36D8);ss(self+0xD0,0);
        sb(self+0x35C,0);sf(self+0x358,zero);ss(self+0x360,0);sb(self+0x34A,0);sb(self+0x368,0);sb(self+0x349,0);ss(self+0x35E,0);sb(self+0x35D,0);ss(self+0x34C,0);ss(self+0xD2,0xFFFF);ss(self+0xD8,0);
        sb(gabi::call<u32>(0x025200D4)+0x5C22,0);sb(self+0x34A,39);ss(self+0x34C,10);sf(self+0x350,zero);sb(self+0x349,0);
    }return self;
}
VERIFY(0x025A2D24,sceneCtor);
static u32 wait(u32 self){
    WWHD_FUNC(0x025A32E8,u32,self);
    s16 time=sh(self+0x34C);if(time>0){ss(self+0x34C,time-1);return self; /* original: r3 still self */}
    phase(self,0x025A3014);return gabi::call<u32>(0x02728D6C,ld(0x101F86B4),self);
}
VERIFY(0x025A32E8,wait);
static void effectDraw(u32 self){
    WWHD_FUNC(0x025A2EC0,void,self);
    s32 t=gabi::ftoi(gabi::fmuls_ppc(lf(0x10051978),lf(self+4)));
    ss(self+0x174,(u32)t*-16u+320);ss(self+0x176,(u32)t*-12u+240);
    if(lb(self+8)){sb(self+8,0);return;}
    if(lb(gabi::call<u32>(0x025200D4)+0x5AC9))return;
    s32 w=320,h=240;
    while(w-16>=gabi::load<u16>(self+0x174)&&h-12>=gabi::load<u16>(self+0x176)){
        s32 dw=w-16,dh=h-12;st(self+0xDC,w/2);st(self+0xE0,h/2);ss(self+0x104,dw);ss(self+0x106,dh);sf(self+0xEC,(f32)dw);sf(self+0xF0,(f32)dh);w=dw;h=dh;
    }
    f32 zero=lf(0x10051974);st(self+0x154,w/2);sf(self+0x15C,zero);sf(self+0x160,zero);st(self+0x158,h/2);
}
VERIFY(0x025A2EC0,effectDraw);
struct Texture {u8 bytes[0x198];};
struct Scale {be<f32> x,y;};
static void nativeDraw(u32 self,u32 context){
    WWHD_FUNC(0x025A332C,void,self,context);
    f32 effect=lf(0x10051990),one=lf(0x10051988);u32 renderer=ld(0x101F9968);
    bool initial=sh(self+0xDA)==-1&&sh(self+0xD8)==0&&ld(self+0xDC)==0x025A36D8;
    if(lb(self+0x348)){gabi::call(0x027F84A4,renderer,4,context);sb(self+0x348,0);if(initial)return;}
    gabi::call(0x0274D690,ld(context+8));gabi::call(0x0274F964,ld(context+0x1C),ld(context+8));
    u32 image=gabi::call<u32>(0x027F81A4,renderer,4);
    gabi::Local<Texture> texture;u32 ptr=gabi::ea(texture.get());gabi::call(0x027BE114,ptr,image);
    sb(ptr+0x190,lb(ptr+0x190)|4);u32 size=ld(context+8);
    f32 denom=gabi::fadds_ppc((f32)ld(self+0x36C),one);
    st(ptr+0x150,0);st(ptr+0x154,0);st(ptr+0x158,0);
    gabi::Local<Scale> scale;u32 sp=gabi::ea(scale.get());sf(sp,(f32)(lf(size+0x10)/denom));sf(sp+4,(f32)(lf(size+0x14)/denom));
    u32 output=gabi::call<u32>(0x027F29D4,0x104B45C0);
    u32 result=gabi::call<u32>(0x0272CF9C,ld(0x101F8710),ptr,sp,ld(output),(f32)ld(self+0x36C),effect);
    output=gabi::call<u32>(0x027F29D4,0x104B45C0);st(output,result);st(output+4,0);
    gabi::call(0x027BE2B0,ptr,2);
}
VERIFY(0x025A332C,nativeDraw);
static void firstSnap(u32 self){
    WWHD_FUNC(0x025A3014,void,self);
    if(!lb(self+0x348)&&!gabi::call<u32>(0x02055B64,self+0x34C)){
        phase(self,0x025A3098);gabi::call(0x025DBDDC,self);
        sb(gabi::call<u32>(0x025200D4)+0x5AC9,0);st(self+0x36C,0);ss(self+0x34C,15);
    }
}
VERIFY(0x025A3014,firstSnap);
static void fadeOut(u32 self){
    WWHD_FUNC(0x025A3098,void,self);
    sb(gabi::call<u32>(0x025200D4)+0x5AC9,0);
    if(!gabi::call<u32>(0x02055B64,self+0x34C)&&ld(self+0x36C)>=45&&gabi::call<u32>(0x025DBDC4,self)){
        gabi::call(0x025DBD74);sb(self+0x349,0);phase(self,0x025A3168);
        sb(gabi::call<u32>(0x025200D4)+0x5C22,0);st(self+0x36C,45);return;
    }
    if(!sh(self+0x34C)&&ld(self+0x36C)<45)st(self+0x36C,ld(self+0x36C)+1);
}
VERIFY(0x025A3098,fadeOut);
static void nextSnap(u32 self){
    WWHD_FUNC(0x025A3168,void,self);
    phase(self,0x025A3224);ss(self+0xD8,0);ss(self+0xDA,0xFFFF);sb(self+0x348,1);st(self+0xDC,0x025A36DC);
    sb(gabi::call<u32>(0x025200D4)+0x5AC9,1);
    s8 time=gabi::load<s8>(self+0x34A);f32 a=lf(0x10051988),b=lf(0x1005198C);
    ss(self+0x34C,(u16)(s16)time);sf(self+0x1D4,a);sf(self+0x364,(f32)(b/(f32)time));
}
VERIFY(0x025A3168,nextSnap);
static void fadeIn(u32 self){
    WWHD_FUNC(0x025A3224,void,self);
    if(!gabi::call<u32>(0x02055B64,self+0x34C)&&!ld(self+0x36C)){
        if(!lb(self+0x368)){gabi::call(0x025DBD74);sb(self+0x368,1);return;}
        gabi::call(0x025DBDDC,self);sb(gabi::call<u32>(0x025200D4)+0x5AC9,1);sb(gabi::call<u32>(0x025200D4)+0x5C22,1);return;
    }
    sb(gabi::call<u32>(0x025200D4)+0x5AC9,0);gabi::call(0x025DBD24);
    u32 n=ld(self+0x36C);if(n)st(self+0x36C,n-1);
}
VERIFY(0x025A3224,fadeIn);
static void invoke(u32 self,u32 off){
    s16 index=sh(self+off+2),adjust=sh(self+off);u32 receiver=self+(s32)adjust;
    u32 target=index<0?ld(self+off+4):ld(ld(receiver+(s32)sh(self+off+6))+(u32)(s32)index*8+4);
    gabi::call_ptr(target,receiver);
}
static BOOL Draw(u32 self){
    WWHD_FUNC(0x025A29C4,BOOL,self);
    if(sh(self+0xDA))invoke(self,0xD8);
    return TRUE;
}
VERIFY(0x025A29C4,Draw);
static BOOL Execute(u32 self){
    WWHD_FUNC(0x025A2A34,BOOL,self);
    invoke(self,0xD0);return TRUE;
}
VERIFY(0x025A2A34,Execute);
static BOOL Delete(u32 self){
    WWHD_FUNC(0x025A2AA0,BOOL,self);
    if(self){
        for(u32 off:{0x2D8u,0x2CCu,0x260u,0x254u,0x1E8u,0x1DCu,0x1D0u,0x164u,0xF8u,0xECu,0xE0u})gabi::call(0x0252CCFC,self+off,0);
        gabi::call(0x025DD630,self,0);
    }
    gabi::call(0x027F88C8,ld(0x101F9968),4);
    gabi::call(0x02728EE0,ld(0x101F86B4));return TRUE;
}
VERIFY(0x025A2AA0,Delete);
static u32 Create(u32 self){
    WWHD_FUNC(0x025A2E94,u32,self);
    if(self)gabi::call(0x025A2D24,self);return 4;
}
VERIFY(0x025A2E94,Create);
static void sinit(){
    WWHD_FUNC(0x025A3634,void,(u32)0);
    st(0x1047B14C,0);st(0x1047B144,0);st(0x1047B150,0);st(0x1047B148,0);
    gabi::call(0x028F026C,0x101EA3AC);
    f32 a=gabi::load<f32>(0x100519A0),b=gabi::load<f32>(0x100519A4);
    gabi::store<f32>(0x1047B138,a);gabi::store<f32>(0x1047B13C,b);
    gabi::call(0x028ED6F8,0x1047B140);gabi::call(0x028F026C,0x101EA3B8);
    gabi::call(0x028EAB2C,0x1047B141);gabi::call(0x028F026C,0x101EA3C4);
}
VERIFY(0x025A3634,sinit);
static BOOL IsDelete(){
    WWHD_FUNC(0x025A36C8,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x025A36C8,IsDelete);
static void inline0(){
    WWHD_FUNC(0x025A36D0,void,(u32)0);
}
VERIFY(0x025A36D0,inline0);
static void inline1(){
    WWHD_FUNC(0x025A36D4,void,(u32)0);
}
VERIFY(0x025A36D4,inline1);
static void inline2(){
    WWHD_FUNC(0x025A36D8,void,(u32)0);
}
VERIFY(0x025A36D8,inline2);
static void inline3(){
    WWHD_FUNC(0x025A36DC,void,(u32)0);
}
VERIFY(0x025A36DC,inline3);
}
