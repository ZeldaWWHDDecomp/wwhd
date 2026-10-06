#include "d/actor/d_a_warpfout.h"
#include "bindings.h"
// Reserve the outgoing linkage below the caller, including real nested helpers.
template<class R = void, class... A>
static R warpfoutCall(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call<R>(target, args...);
}
template<class R = void, class... A>
static R warpfoutCallPtr(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call_ptr<R>(target, args...);
}
#undef WWHD_FUNC
#define WWHD_FUNC(addr, R, ...) \
    if (gabi::Activation::nested()) return warpfoutCall<R>(addr __VA_OPT__(, ) __VA_ARGS__); \
    gabi::Activation wwhd_activation_

void warpfoutOrderNoop(daWarpfout_c* a) {
    WWHD_FUNC(0x024D8198,void,a);
}
VERIFY(0x024D8198,warpfoutOrderNoop);
void warpfoutCreateInit(daWarpfout_c* a) {
    WWHD_FUNC(0x024D819C,void,a);
    a->mInitFlag=-1;
}
VERIFY(0x024D819C,warpfoutCreateInit);
s32 warpfoutCreate(daWarpfout_c* a) {
    WWHD_FUNC(0x024D81A8,s32,a);
    u32 condition=a->actor_condition;
    if (!(condition&8)) {
        if (a) { warpfoutCall<void>(0x025D4ED0,a); a->__vtbl=0x10042474; condition=a->actor_condition; }
        a->actor_condition=condition|8;
    }
    warpfoutCreateInit(a); return 4;
}
VERIFY(0x024D81A8,warpfoutCreate);
s32 warpfoutCreateWrapper(daWarpfout_c* a) {
    WWHD_FUNC(0x024D8210,s32,a);
    return warpfoutCreate(a);
}
VERIFY(0x024D8210,warpfoutCreateWrapper);
s32 warpfoutDelete(daWarpfout_c* a) {
    WWHD_FUNC(0x024D8214,s32,a);
    return 1;
}
VERIFY(0x024D8214,warpfoutDelete);
s32 warpfoutDraw(daWarpfout_c* a) {
    WWHD_FUNC(0x024D821C,s32,a);
    return 1;
}
VERIFY(0x024D821C,warpfoutDraw);
static u32 warpfoutPlay() { return warpfoutCall<u32>(0x025200D4); }
// HD stores its two-part member pointers as adjustment, virtual index, target/offset.
static s32 warpfoutDispatch(u32 entry,daWarpfout_c* a,s32 staff) {
    s16 virtualIndex=gabi::load<s16>(entry+2);
    s16 adjustment=gabi::load<s16>(entry);
    u32 object=gabi::ea(a)+s32(adjustment);
    u32 target;
    if (virtualIndex<0) target=gabi::load<u32>(entry+4);
    else {
        s16 vtableOffset=gabi::load<s16>(entry+6);
        u32 table=gabi::load<u32>(object+s32(vtableOffset));
        target=gabi::load<u32>(table+u32(s32(virtualIndex))*8+4);
    }
    return warpfoutCallPtr<s32>(target,gabi::at<daWarpfout_c>(object),staff);
}
void warpfoutDemo(daWarpfout_c* a) {
    WWHD_FUNC(0x024D8224,void,a);
    u32 play=warpfoutPlay();
    a->mStaffId=warpfoutCall<s32>(0x02542D88,play+0x52C4,STR(0x10042484),0,0);
    play=warpfoutPlay();
    if (!gabi::load<u8>(play+0x5292) || gabi::load<u16>(gabi::ea(a)+0xF8)==1) return;
    s32 staff=a->mStaffId; if (staff==-1) return;
    play=warpfoutPlay();
    s32 action=warpfoutCall<s32>(0x02542EDC,play+0x52C4,staff,0x101D2FB4,5,0,0);
    staff=a->mStaffId;
    if (action==-1) { play=warpfoutPlay(); warpfoutCall<void>(0x02543280,play+0x52C4,staff); return; }
    play=warpfoutPlay();
    s32 advance=warpfoutCall<s32>(0x025447C8,play+0x52C4,staff);
    u32 offset=u32(action)*8;
    if (advance) warpfoutDispatch(0x101D2F44+offset,a,a->mStaffId);
    s32 done=warpfoutDispatch(0x101D2F6C+offset,a,a->mStaffId);
    if (done) { staff=a->mStaffId; play=warpfoutPlay(); warpfoutCall<void>(0x02543280,play+0x52C4,staff); }
}
VERIFY(0x024D8224,warpfoutDemo);
s32 warpfoutExecute(daWarpfout_c* a) {
    WWHD_FUNC(0x024D83E8,s32,a);
    warpfoutDemo(a);
    return 1;
}
VERIFY(0x024D83E8,warpfoutExecute);
s32 warpfoutExecuteWrapper(daWarpfout_c* a) {
    WWHD_FUNC(0x024D840C,s32,a);
    return warpfoutExecute(a);
}
VERIFY(0x024D840C,warpfoutExecuteWrapper);
u32 warpfoutInitWarp1(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D8410,u32,a,staff);
    a->mTimer=5; return warpfoutCall<u32>(0x025E1988,0x2887);
}
VERIFY(0x024D8410,warpfoutInitWarp1);
// The HD small-structure ABI returns x/y in r3 and z/padding in r4.
u32 warpfoutEffectAngle(daWarpfout_c* a) {
    WWHD_FUNC(0x024D8420,u32,a);
    u32 play=warpfoutPlay(); u32 camera=gabi::load<u32>(play+0x5AF8);
    s16 x=warpfoutCall<s16>(0x024F8008,camera);
    play=warpfoutPlay(); camera=gabi::load<u32>(play+0x5AF8);
    s16 y=warpfoutCall<s16>(0x024F8000,camera);
    gabi::Local<be<u16>[4]> angle;
    gabi::store<u16>(angle.a+4,0);
    u32 second=gabi::load<u32>(angle.a+4);
    gabi::store<u16>(angle.a,u16(x));
    gabi::store<u16>(angle.a+2,u16(u32(s32(y))+0x8000));
    gabi::cpu->r[4]=second;
    return gabi::load<u32>(angle.a);
}
VERIFY(0x024D8420,warpfoutEffectAngle);
static void warpfoutParticle(u32 particle,u32 id,cXyz* pos,csXyz* angle) {
    warpfoutCall<void>(0x025A847C,particle,0,id,pos,angle,0,0xFF,0,-1,0,0,0);
}
void warpfoutWind(daWarpfout_c* a,cXyz* position,s16 offset) {
    WWHD_FUNC(0x024D8480,void,a,position,offset);
    u32 xy=warpfoutEffectAngle(a); u32 second=gabi::cpu->r[4];
    gabi::Local<be<u32>[2]> angle;
    gabi::store<u32>(angle.a+4,second);
    s16 z=gabi::load<s16>(angle.a+4);
    gabi::store<u32>(angle.a,xy);
    gabi::store<u16>(angle.a+4,u16(s32(z)+s32(offset)));
    u32 play=warpfoutPlay(); u32 particle=gabi::load<u32>(play+0x5AB0);
    warpfoutParticle(particle,0x830F,position,gabi::at<csXyz>(angle.a));
}
VERIFY(0x024D8480,warpfoutWind);
static void warpfoutRaisedEffect(daWarpfout_c* a,f32 height,s16 angle) {
    u32 play=warpfoutPlay(); u32 player=gabi::load<u32>(play+0x5B34);
    f32 y=gabi::load<f32>(player+0x318);
    f32 x=gabi::load<f32>(player+0x314);
    y=y+height;
    f32 z=gabi::load<f32>(player+0x31C);
    gabi::Local<cXyz> position;
    position->x=x; position->y=y; position->z=z;
    warpfoutWind(a,position,angle);
}
void warpfoutInitWarp2(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D850C,void,a,staff);
    warpfoutRaisedEffect(a,60.0f,0x38E);
    a->mTimer=5;
}
VERIFY(0x024D850C,warpfoutInitWarp2);
void warpfoutInitWarp3(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D8578,void,a,staff);
    warpfoutRaisedEffect(a,80.0f,0x71C);
    a->mTimer=5;
}
VERIFY(0x024D8578,warpfoutInitWarp3);
void warpfoutInitWarp4(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D85E4,void,a,staff);
    u32 play=warpfoutPlay(); u32 player=gabi::load<u32>(play+0x5B34);
    for (s32 count=6;count;--count) {
        play=warpfoutPlay(); u32 particle=gabi::load<u32>(play+0x5AB0);
        warpfoutParticle(particle,0x830E,gabi::at<cXyz>(player+0x314),nullptr);
    }
}
VERIFY(0x024D85E4,warpfoutInitWarp4);
s32 warpfoutActWarp1(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D8664,s32,a,staff);
    if (s32(a->mTimer)==4) warpfoutRaisedEffect(a,40.0f,0);
    return warpfoutCall<s32>(0x0211D2F8,&a->mTimer)==0;
}
VERIFY(0x024D8664,warpfoutActWarp1);
s32 warpfoutActWarp2(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D86E4,s32,a,staff);
    return warpfoutCall<s32>(0x0211D2F8,&a->mTimer)==0;
}
VERIFY(0x024D86E4,warpfoutActWarp2);
s32 warpfoutActWarp3(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D8710,s32,a,staff);
    return warpfoutCall<s32>(0x0211D2F8,&a->mTimer)==0;
}
VERIFY(0x024D8710,warpfoutActWarp3);
s32 warpfoutActEnd(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D873C,s32,a,staff);
    warpfoutCall<void>(0x025D57E0,a);
    return 1;
}
VERIFY(0x024D873C,warpfoutActEnd);
void warpfoutStaticInit() {
    WWHD_FUNC(0x024D8760,void);
    gabi::store<u32>(0x1046EB40,0); gabi::store<u32>(0x1046EB38,0);
    gabi::store<u32>(0x1046EB44,0); gabi::store<u32>(0x1046EB3C,0);
    warpfoutCall<void>(0x028F026C,0x101D2FC8);
    gabi::store<f32>(0x1046EB2C,-3.1415927410125732f); gabi::store<f32>(0x1046EB30,3.1415927410125732f);
    warpfoutCall<void>(0x028ED6F8,0x1046EB34); warpfoutCall<void>(0x028F026C,0x101D2FD4);
    warpfoutCall<void>(0x028EAB2C,0x1046EB35); warpfoutCall<void>(0x028F026C,0x101D2FE0);
}
VERIFY(0x024D8760,warpfoutStaticInit);
s32 warpfoutIsDelete(daWarpfout_c* a) {
    WWHD_FUNC(0x024D87F4,s32,a);
    return 1;
}
VERIFY(0x024D87F4,warpfoutIsDelete);
s32 warpfoutActWarp4(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D87FC,s32,a,staff);
    return 1;
}
VERIFY(0x024D87FC,warpfoutActWarp4);
void warpfoutInitEnd(daWarpfout_c* a,s32 staff) {
    WWHD_FUNC(0x024D8804,void,a,staff);
}
VERIFY(0x024D8804,warpfoutInitEnd);
void warpfoutDestructor(daWarpfout_c* a,s32 flags) {
    WWHD_FUNC(0x024D8808,void,a,flags);
    if (a) { warpfoutCall<void>(0x025D50BC,a,0); if (u32(flags)&1) warpfoutCall<void>(0x0273AF40,a); }
}
VERIFY(0x024D8808,warpfoutDestructor);
