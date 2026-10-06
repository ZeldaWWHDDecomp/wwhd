/* f_pc_manager: HD management loop, lifecycle wrappers and layer lookup.
 * Full025DF904..025DFCA8 TU; mistaken legacy DD868 map is
 * owned by create_req (the manager alias was folded into its ID lookup).
 * Graphics/DVD error message code from GC is absent; HD adds its render object tail.
 */
#include "bindings.h"
namespace f_pc_manager_cpp {
static u32 ld(u32 a) {return gabi::load<u32>(a);}
static u8 lb(u32 a) {return gabi::load<u8>(a);}
static void st(u32 a,u32 v) {gabi::store<u32>(a,v);}
static BOOL Draw(u32 proc) {
    WWHD_FUNC(0x025DF904,BOOL,proc);
    return gabi::call<BOOL>(0x025DE2CC,proc);
}
VERIFY(0x025DF904,Draw);
static BOOL DrawIterater(u32 callback) {
    WWHD_FUNC(0x025DF908,BOOL,callback);
    u32 layer=gabi::call<u32>(0x025DEAA8);
    return gabi::call<BOOL>(0x025DED70,layer,callback,0);
}
VERIFY(0x025DF908,DrawIterater);
static BOOL Execute(u32 proc) {
    WWHD_FUNC(0x025DF940,BOOL,proc);
    return gabi::call<BOOL>(0x025DE58C,proc);
}
VERIFY(0x025DF940,Execute);
static BOOL Delete(u32 proc) {
    WWHD_FUNC(0x025DF944,BOOL,proc);
    return gabi::call<BOOL>(0x025DE1B8,proc);
}
VERIFY(0x025DF944,Delete);
static void Management(u32 before,u32 after) {
    WWHD_FUNC(0x025DF948,void,before,after);
    gabi::call(0x0200FAC4);
    gabi::call(0x025F2B08);
    u32 play=gabi::call<u32>(0x025200D4);
    if(lb(play+0x5AC9)) {
        play=gabi::call<u32>(0x025200D4);
        s32 slot=(s8)lb(play+0x5AF4);
        u32 current=play+0x5ACC;
        play=gabi::call<u32>(0x025200D4);
        u32 active=ld(play+slot*52+0x5AF8);
        if(active) {
            play=gabi::call<u32>(0x025200D4);
            st(play+0x5D04,current);
            play=gabi::call<u32>(0x025200D4);
            st(play+0x5D08,active);
            play=gabi::call<u32>(0x025200D4);
            st(play+0x5D0C,current);
        }
    }
    gabi::call(0x025DE024);
    if(!gabi::call<BOOL>(0x025E0EE4)) gabi::call(0x0273AA24,0x10058398,0x256,0x10058394);
    if(!gabi::call<BOOL>(0x025DDCEC)) gabi::call(0x0273AA24,0x10058398,0x25A,0x10058394);
    if(before) gabi::call_ptr(before);
    gabi::call(0x025DE788,0x025DF940);
    gabi::call(0x025DE37C,0x025DF908,0x025DF904);
    if(after) gabi::call_ptr(after);
    u32 graphics=ld(0x101F8344);
    if(graphics) gabi::call(0x02715310,graphics);
}
VERIFY(0x025DF948,Management);
static void Init() {
    WWHD_FUNC(0x025DFA7C,void,(u32)0);
    gabi::call(0x025DEBF0,0x1048A9BC,0,0x1048A9E8,10);
    gabi::call(0x025DF468);
}
VERIFY(0x025DFA7C,Init);
static u32 FastCreate(u32 type,u32 callback,u32 createData,u32 data) {
    WWHD_FUNC(0x025DFAB8,u32,type,callback,createData,data);
    u32 layer=gabi::call<u32>(0x025DED64);
    return gabi::call<u32>(0x025DE884,layer,type,callback,createData,data);
}
VERIFY(0x025DFAB8,FastCreate);
static s32 IsPause(u32 proc,u32 flag) {
    WWHD_FUNC(0x025DFB1C,s32,proc,flag);
    return gabi::call<s32>(0x025E0B20,proc,flag);
}
VERIFY(0x025DFB1C,IsPause);
static void PauseEnable(u32 proc,u32 flag) {
    WWHD_FUNC(0x025DFB20,void,proc,flag);
    gabi::call(0x025E0B38,proc,flag);
}
VERIFY(0x025DFB20,PauseEnable);
static void PauseDisable(u32 proc,u32 flag) {
    WWHD_FUNC(0x025DFB24,void,proc,flag);
    gabi::call(0x025E0BA8,proc,flag);
}
VERIFY(0x025DFB24,PauseDisable);
static u32 JudgeInLayer(u32 id,u32 callback,u32 data) {
    WWHD_FUNC(0x025DFB28,u32,id,callback,data);
    u32 layer=gabi::call<u32>(0x025DEAC0,id);
    if(!layer) return 0;
    u32 result=gabi::call<u32>(0x025DD780,id,callback,data);
    if(!result) result=gabi::call<u32>(0x025DEE20,layer,callback,data);
    return result;
}
VERIFY(0x025DFB28,JudgeInLayer);
static void sinit() {
    WWHD_FUNC(0x025DFBE8,void,(u32)0);
    for(int i=0;i<4;++i) st(0x1048A9AC+4*i,0);
    gabi::call(0x028F026C,0x101F3C88);
    gabi::store<f32>(0x1048A990,gabi::load<f32>(0x100583B4));
    gabi::store<f32>(0x1048A994,gabi::load<f32>(0x100583B8));
    gabi::call(0x028ED6F8,0x1048A9A8);
    gabi::call(0x028F026C,0x101F3C94);
    gabi::call(0x028EAB2C,0x1048A9A9);
    gabi::call(0x028F026C,0x101F3CA0);
    f32 first=gabi::load<f32>(0x100583BC);
    f32 second=gabi::load<f32>(0x100583C0);
    gabi::store<f32>(0x1048A998,first);
    gabi::store<f32>(0x1048A9A0,second);
    gabi::store<f32>(0x1048A99C,first);
    gabi::store<f32>(0x1048A9A4,second);
}
VERIFY(0x025DFBE8,sinit);
}
