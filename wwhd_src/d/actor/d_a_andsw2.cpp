#include "d/actor/d_a_andsw2.h"
#include "bindings.h"
// Reserve the outgoing linkage below the caller, including real nested helpers.
template<class R = void, class... A>
static R andsw2Call(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call<R>(target, args...);
}
template<class R = void, class... A>
static R andsw2CallPtr(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call_ptr<R>(target, args...);
}
#undef WWHD_FUNC
#define WWHD_FUNC(addr, R, ...) \
    if (gabi::Activation::nested()) return andsw2Call<R>(addr __VA_OPT__(, ) __VA_ARGS__); \
    gabi::Activation wwhd_activation_

enum { ACT_ON_ALL, ACT_TIMER, ACT_ORDER, ACT_EVENT, ACT_OFF, ACT_WAIT };
u8 andsw2GetSwbit2(daAndsw2_c* a) {
    WWHD_FUNC(0x020519EC,u8,a);
    return u32(a->mParameters)>>24;
}
VERIFY(0x020519EC,andsw2GetSwbit2);
u8 andsw2GetSwbit(daAndsw2_c* a) {
    WWHD_FUNC(0x020519F8,u8,a);
    return u32(a->mParameters)>>16;
}
VERIFY(0x020519F8,andsw2GetSwbit);
u8 andsw2GetTopSw(daAndsw2_c* a) {
    WWHD_FUNC(0x02051A04,u8,a);
    u8 sw=andsw2GetSwbit2(a);
    if (sw!=0xFF) return sw;
    sw=andsw2GetSwbit(a);
    return sw!=0xFF ? u8(sw+1) : u8(0xFF);
}
VERIFY(0x02051A04,andsw2GetTopSw);
u8 andsw2GetNum(daAndsw2_c* a) {
    WWHD_FUNC(0x02051A54,u8,a);
    return u32(a->mParameters);
}
VERIFY(0x02051A54,andsw2GetNum);
BOOL andsw2ChkAllSw2(daAndsw2_c* a) {
    WWHD_FUNC(0x02051A60,BOOL,a);
    s32 top=andsw2GetTopSw(a);
    s32 remaining=andsw2GetNum(a);
    if (remaining==0xFF || remaining==0 || top==0xFF) return FALSE;
    for (s32 i=0; remaining>0; i++,remaining--) {
        u32 save=gabi::load<u32>(0x101F84DC);
        s32 room=a->current.roomNo;
        if (!andsw2Call<s32>(0x025BA0C0,save+0x20,top+i,room)) return FALSE;
    }
    return TRUE;
}
VERIFY(0x02051A60,andsw2ChkAllSw2);
u8 andsw2GetTimer(daAndsw2_c* a) {
    WWHD_FUNC(0x02051B0C,u8,a);
    return u16(a->home.angle.z);
}
VERIFY(0x02051B0C,andsw2GetTimer);
u8 andsw2GetType(daAndsw2_c* a) {
    WWHD_FUNC(0x02051B18,u8,a);
    return u32(a->mParameters)>>8;
}
VERIFY(0x02051B18,andsw2GetType);
BOOL andsw2ActionOnAll(daAndsw2_c* a) {
    WWHD_FUNC(0x02051B24,BOOL,a);
    if (andsw2ChkAllSw2(a)) {
        u8 timer=andsw2GetTimer(a);
        if (timer!=0xFF) {
            a->mAction=ACT_TIMER;
            a->mTimer=s16(u32(timer)*15);
        } else if (s16(a->mEventIdx)!=-1) {
            a->mAction=ACT_ORDER;
        } else {
            u8 sw=andsw2GetSwbit(a);
            u32 save=gabi::load<u32>(0x101F84DC);
            s32 room=a->current.roomNo;
            andsw2Call<void>(0x025B9E38,save+0x20,sw,room);
            u8 type=andsw2GetType(a);
            a->mAction=type==1 ? ACT_OFF : ACT_WAIT;
        }
    }
    return TRUE;
}
VERIFY(0x02051B24,andsw2ActionOnAll);
BOOL andsw2ActionTimer(daAndsw2_c* a) {
    WWHD_FUNC(0x02051BD8,BOOL,a);
    if (andsw2GetType(a)==1 && !andsw2ChkAllSw2(a)) {
        a->mAction=ACT_ON_ALL;
    } else if (s16(a->mTimer)>0) {
        a->mTimer=s16(s16(a->mTimer)-1);
    } else if (s16(a->mEventIdx)!=-1) {
        a->mAction=ACT_ORDER;
    } else {
        u8 sw=andsw2GetSwbit(a);
        u32 save=gabi::load<u32>(0x101F84DC);
        s32 room=a->current.roomNo;
        andsw2Call<void>(0x025B9E38,save+0x20,sw,room);
        if (andsw2GetType(a)==1) a->mAction=ACT_WAIT;
    }
    return TRUE;
}
VERIFY(0x02051BD8,andsw2ActionTimer);
u8 andsw2GetEventNo(daAndsw2_c* a) {
    WWHD_FUNC(0x02051CA4,u8,a);
    return u16(a->home.angle.x);
}
VERIFY(0x02051CA4,andsw2GetEventNo);
BOOL andsw2ActionOrder(daAndsw2_c* a) {
    WWHD_FUNC(0x02051CB0,BOOL,a);
    if (gabi::load<u16>(gabi::ea(a)+0xF8)==2) {
        a->mAction=ACT_EVENT;
        u8 sw=andsw2GetSwbit(a);
        u32 save=gabi::load<u32>(0x101F84DC);
        s32 room=a->current.roomNo;
        andsw2Call<void>(0x025B9E38,save+0x20,sw,room);
    } else if (andsw2GetType(a)==1 && !andsw2ChkAllSw2(a)) {
        a->mAction=ACT_ON_ALL;
    } else {
        u8 ev=andsw2GetEventNo(a);
        s32 idx=a->mEventIdx;
        andsw2Call<void>(0x025D7A58,a,idx,ev,0xFFFF,0,1);
    }
    return TRUE;
}
VERIFY(0x02051CB0,andsw2ActionOrder);
BOOL andsw2ActionEvent(daAndsw2_c* a) {
    WWHD_FUNC(0x02051D6C,BOOL,a);
    s32 idx=a->mEventIdx;
    u32 play=andsw2Call<u32>(0x025200D4);
    if (andsw2Call<s32>(0x025440C8,play+0x52C4,idx)) {
        u8 type=andsw2GetType(a);
        a->mAction=type==1 ? ACT_OFF : ACT_WAIT;
        play=andsw2Call<u32>(0x025200D4);
        u16 state=gabi::load<u16>(play+0x52B8);
        gabi::store<u16>(play+0x52B8,state|8);
    }
    return TRUE;
}
VERIFY(0x02051D6C,andsw2ActionEvent);
BOOL andsw2ActionOff(daAndsw2_c* a) {
    WWHD_FUNC(0x02051DE8,BOOL,a);
    if (!andsw2ChkAllSw2(a)) {
        a->mAction=ACT_ON_ALL;
        u8 sw=andsw2GetSwbit(a);
        u32 save=gabi::load<u32>(0x101F84DC);
        s32 room=a->current.roomNo;
        andsw2Call<void>(0x025B9F7C,save+0x20,sw,room);
    }
    return TRUE;
}
VERIFY(0x02051DE8,andsw2ActionOff);
BOOL andsw2ActionWait(daAndsw2_c* a) {
    WWHD_FUNC(0x02051E4C,BOOL,a);
    return TRUE;
}
VERIFY(0x02051E4C,andsw2ActionWait);
BOOL andsw2Execute(daAndsw2_c* a) {
    WWHD_FUNC(0x02051E54,BOOL,a);
    if (!gabi::load<u32>(0x101FD9F4)) {
        gabi::store<u32>(0x101FD9F4,1);
        andsw2Call<void>(0xC000A848,0x101FD9F8,0x1018FD18,24);
    }
    u8 action=a->mAction;
    u32 fn=gabi::load<u32>(0x101FD9F8+u32(action)*4);
    andsw2CallPtr<void>(fn,a);
    return TRUE;
}
VERIFY(0x02051E54,andsw2Execute);
BOOL andsw2IsDelete(daAndsw2_c* a) {
    WWHD_FUNC(0x02051ED4,BOOL,a);
    return TRUE;
}
VERIFY(0x02051ED4,andsw2IsDelete);
BOOL andsw2Delete(daAndsw2_c* a) {
    WWHD_FUNC(0x02051EDC,BOOL,a);
    return TRUE;
}
VERIFY(0x02051EDC,andsw2Delete);
s32 andsw2Create(daAndsw2_c* a) {
    WWHD_FUNC(0x02051EE4,s32,a);
    u8 sw=andsw2GetSwbit(a);
    if (!(u32(a->actor_condition)&8)) {
        if (a) {
            andsw2Call<void>(0x025D4ED0,a);
            a->__vtbl=0x100071D4;
        }
        a->actor_condition=u32(a->actor_condition)|8;
    }
    u8 type=andsw2GetType(a);
    if (type==0) {
        BOOL on=FALSE;
        if (sw!=0xFF) {
            u32 save=gabi::load<u32>(0x101F84DC);
            s32 room=a->current.roomNo;
            on=andsw2Call<s32>(0x025BA0C0,save+0x20,sw,room);
        }
        a->mAction=(sw==0xFF || on) ? ACT_WAIT : ACT_ON_ALL;
        u8 ev=andsw2GetEventNo(a);
        u32 play=andsw2Call<u32>(0x025200D4);
        a->mEventIdx=s16(andsw2Call<s32>(0x02543F10,play+0x52C4,0,ev));
    } else if (type==1) {
        if (sw==0xFF) a->mAction=ACT_WAIT;
        else {
            s32 room=a->current.roomNo;
            u32 save=gabi::load<u32>(0x101F84DC);
            BOOL on=andsw2Call<s32>(0x025BA0C0,save+0x20,sw,room);
            a->mAction=on ? ACT_OFF : ACT_ON_ALL;
        }
        u8 ev=andsw2GetEventNo(a);
        u32 play=andsw2Call<u32>(0x025200D4);
        a->mEventIdx=s16(andsw2Call<s32>(0x02543F10,play+0x52C4,0,ev));
    } else a->mAction=ACT_WAIT;
    a->shape_angle.z=0;
    a->current.angle.z=0;
    a->current.angle.x=0;
    a->shape_angle.x=0;
    return 4;
}
VERIFY(0x02051EE4,andsw2Create);
void andsw2StaticInit() {
    WWHD_FUNC(0x020520CC,void);
    gabi::store<u32>(0x10461464,0);
    gabi::store<u32>(0x1046145C,0);
    gabi::store<u32>(0x10461468,0);
    gabi::store<u32>(0x10461460,0);
    andsw2Call<void>(0x028F026C,0x1018FD50);
    gabi::store<f32>(0x10461450,-3.1415927410125732f);
    gabi::store<f32>(0x10461454,3.1415927410125732f);
    andsw2Call<void>(0x028ED6F8,0x10461458);
    andsw2Call<void>(0x028F026C,0x1018FD5C);
    andsw2Call<void>(0x028EAB2C,0x10461459);
    andsw2Call<void>(0x028F026C,0x1018FD68);
}
VERIFY(0x020520CC,andsw2StaticInit);
BOOL andsw2Draw(daAndsw2_c* a) {
    WWHD_FUNC(0x02052160,BOOL,a);
    return TRUE;
}
VERIFY(0x02052160,andsw2Draw);
void andsw2Destructor(daAndsw2_c* a,u32 flags) {
    WWHD_FUNC(0x02052168,void,a,flags);
    if (!a) return;
    andsw2Call<void>(0x025D50BC,a,0);
    if (flags&1) andsw2Call<void>(0x0273AF40,a);
}
VERIFY(0x02052168,andsw2Destructor);
