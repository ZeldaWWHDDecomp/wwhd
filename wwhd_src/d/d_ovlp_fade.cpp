#include "bindings.h"
namespace {
static u32 ld(u32 a){return gabi::load<u32>(a);}
static s16 sh(u32 a){return gabi::load<s16>(a);}
static u8 lb(u32 a){return gabi::load<u8>(a);}
static f32 lf(u32 a){return gabi::load<f32>(a);}
static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static void sf(u32 a,f32 v){gabi::store<f32>(a,v);}
struct SafeString {be<u32> ptr,vtable;};
static bool same(u32 a,u32 b){if(a==b)return true;for(u32 i=0;i<0x40001;i++){u8 x=lb(a+i),y=lb(b+i);if(x!=y)return false;if(!x)return true;}return false;}
static bool demoName(u32 name){
    gabi::Local<SafeString> left,right;u32 a=gabi::ea(left.get()),b=gabi::ea(right.get());st(b+4,0x10051884);st(a+4,0x10051884);st(a,name);st(b,0x1047E6B8);
    gabi::call_ptr(0x025A1BCC,a);gabi::call_ptr(ld(ld(a+4)+20),a);u32 saved=ld(a);
    gabi::call_ptr(ld(ld(b+4)+20),b);return same(saved,ld(b));
}
static BOOL fadeOut(u32 self){
    WWHD_FUNC(0x025A17A0,BOOL,self);
    if(!ld(self+0xD0)){
        if(self&&sh(self+14)==2){if(!gabi::call<u32>(0x02728CB8,ld(0x101F86B4),1,0))return TRUE;st(self+0xD0,1);}
        else {u32 mode=demoName(0x1005189C)?1:(demoName(0x100518A4)?2:0);
            if(gabi::call<u32>(0x02728CB8,ld(0x101F86B4),26,mode))return TRUE;
            st(self+0xD0,26);
        }
    }
    gabi::call(0x025DBD74);u32 count=ld(self+0xD0)-1;st(self+0xD0,count);
    if(!count){gabi::call(0x025DBDDC,self);st(self+0xD0,ld(self+0xD0)+1);}return TRUE;
}
VERIFY(0x025A17A0,fadeOut);
static BOOL wait(u32 self){
    WWHD_FUNC(0x025A19E0,BOOL,self);
    if(gabi::call<u32>(0x025DBDC4,self)){st(0x101EA1D4,0x025A17A0);gabi::call(0x025DBD74);}return TRUE;
}
VERIFY(0x025A19E0,wait);
static BOOL fadeIn(u32 self){
    WWHD_FUNC(0x025A1A20,BOOL,self);
    u32 count=ld(self+0xD4);
    if(!count){count=26;st(self+0xD4,count);
        if(self&&(sh(self+14)==0||sh(self+14)==3)){gabi::call(0x025F05E8,26);count=ld(self+0xD4);}}
    st(self+0xD4,count-1);
    if(count==1){st(0x101EA1D4,0x025A19E0);gabi::call(0x025F05E8,1);gabi::call(0x02728D5C,ld(0x101F86B4));gabi::call(0x025DBDDC,self);}return TRUE;
}
VERIFY(0x025A1A20,fadeIn);
static BOOL Execute(u32 self){
    WWHD_FUNC(0x025A1ACC,BOOL,self);
    gabi::call_ptr(ld(0x101EA1D4),self);return TRUE;
}
VERIFY(0x025A1ACC,Execute);
static BOOL Draw(){
    WWHD_FUNC(0x025A1798,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x025A1798,Draw);
static BOOL IsDelete(){
    WWHD_FUNC(0x025A1AFC,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x025A1AFC,IsDelete);
static BOOL Delete(){
    WWHD_FUNC(0x025A1B04,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x025A1B04,Delete);
static u32 Create(){
    WWHD_FUNC(0x025A1B0C,u32,(u32)0);
    st(0x101EA1D4,0x025A1A20);return 4;
}
VERIFY(0x025A1B0C,Create);
static void sinit(){
    WWHD_FUNC(0x025A1B24,void,(u32)0);
    st(0x1047B0F8,0);st(0x1047B0F0,0);st(0x1047B0FC,0);st(0x1047B0F4,0);
    gabi::call(0x028F026C,0x101EA1EC);f32 a=lf(0x100518B4),b=lf(0x100518B8);sf(0x1047B0E4,a);sf(0x1047B0E8,b);
    gabi::call(0x028ED6F8,0x1047B0EC);gabi::call(0x028F026C,0x101EA1F8);gabi::call(0x028EAB2C,0x1047B0ED);gabi::call(0x028F026C,0x101EA204);
}
VERIFY(0x025A1B24,sinit);
static void stringDtor(u32 self,u32 flags){
    WWHD_FUNC(0x025A1BB8,void,self,flags);
    if(self&&(flags&1))gabi::call(0x0273AF40,self);
}
VERIFY(0x025A1BB8,stringDtor);
static void stringStub(){
    WWHD_FUNC(0x025A1BCC,void,(u32)0);
}
VERIFY(0x025A1BCC,stringStub);
}
