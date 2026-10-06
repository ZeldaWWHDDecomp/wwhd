/* Opening scene HD TU025AEBCC..025AF20F:14entries incl inline stubs.
 * HD native UI uses singleton101F8318. */
#include "bindings.h"
namespace d_s_open_cpp {
static u32 ld(u32 a){return gabi::load<u32>(a);}
static s16 lh(u32 a){return gabi::load<s16>(a);}
static u8 lb(u32 a){return gabi::load<u8>(a);}
static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static void sb(u32 a,u8 v){gabi::store<u8>(a,v);}
static BOOL IsDelete(){
    WWHD_FUNC(0x025AF208,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x025AF208,IsDelete);
static void packetDelete(u32 self,u32 flags){
    WWHD_FUNC(0x025AEBCC,void,self,flags);
    if(self && (flags&1))gabi::call(0x0273AF40,self);
}
VERIFY(0x025AEBCC,packetDelete);
static void packetStub(){
    WWHD_FUNC(0x025AEBE0,void,(u32)0);
}
VERIFY(0x025AEBE0,packetStub);
static BOOL draw(u32 self){
    WWHD_FUNC(0x025AEBE4,BOOL,self);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 window=play+0x5ACC;
    play=gabi::call<u32>(0x025200D4);st(play+0x5F9C,window);
    play=gabi::call<u32>(0x025200D4);st(play+0x5FA0,window);
    u32 tag=gabi::call<u32>(0x025DA7AC);
    while(tag){gabi::call(0x025DF904,ld(tag+12));tag=gabi::call<u32>(0x025DA7D0,tag);}
    u32 packet=ld(self+0x1D4);
    play=gabi::call<u32>(0x025200D4);
    u32 list=play+0x5D30;
    gabi::call(0x0252CDC0,list,list+0x1DC,list+0x1E0,packet);
    return TRUE;
}
VERIFY(0x025AEBE4,draw);
static BOOL Draw(u32 self){
    WWHD_FUNC(0x025AEC70,BOOL,self);
    return gabi::call<BOOL>(0x025AEBE4,self);
}
VERIFY(0x025AEC70,Draw);
static void changeGameScene(u32 self){
    WWHD_FUNC(0x025AEC74,void,self);
    if(gabi::call<BOOL>(0x025DBE00))return;
    if(lh(self+8)==13){gabi::call(0x02520230,self,8);return;}
    if(gabi::call<BOOL>(0x025DC86C,self,7,0,5,1)){
        st(ld(0x101F84DC)+0x116C,0);
        u32 play=gabi::call<u32>(0x025200D4),stage=play+0x5140;
        play=gabi::call<u32>(0x025200D4);s32 room=(s8)lb(play+0x514A);
        play=gabi::call<u32>(0x025200D4);s32 layer=(s8)lb(play+0x514B);
        gabi::call(0x025E17CC,stage,room,layer);
    }
}
VERIFY(0x025AEC74,changeGameScene);
static BOOL execute(u32 self){
    WWHD_FUNC(0x025AED3C,BOOL,self);
    gabi::call(0x0270CA94,ld(0x101F8318));
    u32 count=ld(self+0x1DC);
    if((s32)count<90){
        ++count;st(self+0x1DC,count);
        if(count==40)gabi::call(0x025E1934,0xC0000024);
        else if((s32)count>=90)gabi::call(0x025E1944);
    }
    s16 name=lh(self+8);u32 skip=0;
    if(name==12 && (s32)ld(ld(0x101F8318))>0){
        BOOL pressed=gabi::call<BOOL>(0x02007940,0);
        name=lh(self+8);if(pressed)skip=1;
    }
    if(name==12 && gabi::call<BOOL>(0x02007940,0)){
        u32 ui=ld(0x101F8318),proc=ld(ui+0x94);
        st(ui,300);gabi::call(0x026BF3E8,proc);
    }
    if(!gabi::call<BOOL>(0x025DBE00) && !gabi::call<BOOL>(0x025202F8,self)){
        u32 buttons=0;
        if(lh(self+8)==13){
            if(gabi::call<BOOL>(0x02007898,0) || gabi::call<BOOL>(0x020078BC,0) || gabi::call<BOOL>(0x02007940,0) || gabi::call<BOOL>(0x020078E8,0) || gabi::call<BOOL>(0x02007914,0)) buttons=1;
            else {
                u32 pad=ld(0x101F5088);
                if((ld(pad+0x18)&0x1000) || (ld(ld(pad))&0x8000))buttons=1;
            }
        }
        if(skip|buttons){sb(self+0x1D8,1);gabi::call(0x025E1904,20);}
        if(lb(self+0x1D8) && !gabi::call<BOOL>(0x025E20F0))gabi::call(0x025AEC74,self);
    }
    u32 ui=ld(0x101F8318);
    if(lb(ld(ui+0x94)+0x88))gabi::call(0x025AEC74,self);
    return TRUE;
}
VERIFY(0x025AED3C,execute);
static BOOL Execute(u32 self){
    WWHD_FUNC(0x025AEF38,BOOL,self);
    return gabi::call<BOOL>(0x025AED3C,self);
}
VERIFY(0x025AEF38,Execute);
static void destructor(u32 self,u32 flags){
    WWHD_FUNC(0x025AEF3C,void,self,flags);
    if(!self)return;
    u32 proc=ld(self+0x1D4);
    if(proc)gabi::call_ptr(ld(ld(proc)+12),proc,3);
    u32 heap=ld(self+0x1D0);
    if(heap)gabi::call(0x025E3868,heap);
    gabi::call(0x025204C8,self+0x1C8,0x10052B1C);
    u32 play=gabi::call<u32>(0x025200D4);sb(play+0x5AC9,0);
    gabi::call(0x0270CB3C,ld(0x101F8318));
    gabi::call(0x0270CE94);
    sb(0x101EA834,0);
    gabi::call(0x025DD630,self,2);
    if(flags&1)gabi::call(0x0273AF40,self);
}
VERIFY(0x025AEF3C,destructor);
static BOOL Delete(u32 self){
    WWHD_FUNC(0x025AEFFC,BOOL,self);
    gabi::call(0x025AEF3C,self,2);return TRUE;
}
VERIFY(0x025AEFFC,Delete);
static u32 constructor(u32 self){
    WWHD_FUNC(0x025AF024,u32,self);
    if(!self)self=gabi::call<u32>(0x0273AD10,0x1E0);
    if(self){
        gabi::call(0x025DD5F0,self);
        gabi::call(0x028F521C,self+0x1C8,8);
        st(self+0x1DC,0);st(self+0x1D0,0);sb(self+0x1D8,0);st(self+0x1D4,0);
    }
    return self;
}
VERIFY(0x025AF024,constructor);
static s32 create(u32 self){
    WWHD_FUNC(0x025AF08C,s32,self);
    u8 phase=lb(0x101EA834);
    if(phase>1){
        st(self+0x1DC,0);
        u32 play=gabi::call<u32>(0x025200D4);sb(play+0x514C,0);
        gabi::call(0x0270CD94,0);
        gabi::call(0x0270C940,ld(0x101F8318));
        sb(0x101EA834,0);return 4;
    }
    u32 result=phase==0?gabi::call<u32>(0x026FBB8C,ld(0x101F7274),2):gabi::call<u32>(0x026FBB7C,ld(0x101F7274));
    if(result)sb(0x101EA834,lb(0x101EA834)+1);
    return 0;
}
VERIFY(0x025AF08C,create);
static s32 Create(u32 self){
    WWHD_FUNC(0x025AF138,s32,self);
    if(self)gabi::call(0x025AF024,self);
    return gabi::call<s32>(0x025AF08C,self);
}
VERIFY(0x025AF138,Create);
static void sinit(){
    WWHD_FUNC(0x025AF174,void,(u32)0);
    st(0x1047B3F4,0);st(0x1047B3EC,0);st(0x1047B3F8,0);st(0x1047B3F0,0);
    gabi::call(0x028F026C,0x101EA810);
    f32 a=gabi::load<f32>(0x10052B2C),b=gabi::load<f32>(0x10052B30);
    gabi::store<f32>(0x1047B3E0,a);gabi::store<f32>(0x1047B3E4,b);
    gabi::call(0x028ED6F8,0x1047B3E8);gabi::call(0x028F026C,0x101EA81C);
    gabi::call(0x028EAB2C,0x1047B3E9);gabi::call(0x028F026C,0x101EA828);
}
VERIFY(0x025AF174,sinit);
}
