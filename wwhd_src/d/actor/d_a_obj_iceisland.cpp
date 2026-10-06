// Icy clouds around Ice Ring Isle. GC actor logic adapted to the WWHD layout.
#include "bindings.h"
#include "d/actor/d_a_obj_iceisland.h"

namespace {
using Actor = daObjIceisland_c;
static u32 ea(Actor* a) { return gabi::ea(a); }
static u8* part(Actor* a,u32 offset) { return gabi::at<u8>(ea(a)+offset); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static BOOL isSwitch(Actor* a) {
    u32 param=a->mParameters;
    s8 room=a->home.roomNo;
    u32 save=gabi::load<u32>(0x101F84DC);
    return gabi::call<BOOL>(0x025BA0C0,gabi::at<u8>(save+0x20),(param>>8)&255,room);
}
static void order(Actor* a,s16 event) { gabi::call(0x025D7A58,a,event,255,65535,0,1); }
struct ResourceName { be<u32> data,vtable; };
static u32 resource(s32 index) {
    gabi::Local<ResourceName> name;
    name->data=0x1002B220; name->vtable=0x1002B1F4;
    return gabi::call<u32>(0x026066C4,gabi::at<u8>(gabi::load<u32>(0x101F4F28)),(ResourceName*)name,index);
}
static BOOL createHeap(Actor* a) {
    WWHD_FUNC(0x0235EB00,BOOL,a);
    u32 data=resource(5);
    if(!data) gabi::call(0x0273AA24,STR(0x1002B238),0x68,STR(0x1002B228));
    a->model=gabi::call<u32>(0x025E38E0,gabi::at<u8>(data),0,0x11020203);
    u32 animation=resource(11);
    if(!animation) gabi::call(0x0273AA24,STR(0x1002B238),0x6F,STR(0x1002B250));
    s32 first=gabi::call<s32>(0x025E7CE0,part(a,0x3B8),gabi::at<u8>(data),gabi::at<u8>(animation),1,2,0,1.0f,-1,0,0);
    animation=resource(12);
    if(!animation) gabi::call(0x0273AA24,STR(0x1002B238),0x75,STR(0x1002B25C));
    s32 second=gabi::call<s32>(0x025E7CE0,part(a,0x42C),gabi::at<u8>(data),gabi::at<u8>(animation),1,2,0,1.0f,-1,0,0);
    animation=resource(8);
    if(!animation) gabi::call(0x0273AA24,STR(0x1002B238),0x7C,STR(0x1002B268));
    s32 third=gabi::call<s32>(0x025E8154,part(a,0x4A0),gabi::at<u8>(data),gabi::at<u8>(animation),1,0,0,1.0f,-1,0,0);
    u32 model=a->model;
    if(model) {
        u32 flags=gabi::load<u32>(model+0x74)&~1u;
        gabi::store<u32>(model+0x74,flags);
        gabi::call(0x027F596C,gabi::at<u8>(model),flags);
        model=a->model;
    }
    return model && first && second && third;
}
VERIFY(0x0235EB00,createHeap);
static BOOL checkCreateHeap(Actor* a) { WWHD_FUNC(0x0235ED24,BOOL,a); return gabi::call<BOOL>(0x0235EB00,a); }
VERIFY(0x0235ED24,checkCreateHeap);
static Actor* construct(Actor* a) {
    WWHD_FUNC(0x0235ED28,Actor*,a);
    if(!a) a=gabi::call<Actor*>(0x0273AD10,0x6F4);
    if(a) {
        gabi::call(0x025D4ED0,a); a->__vtbl=0x1002B20C;
        gabi::call(0x025E7C6C,part(a,0x3B8));
        gabi::call(0x025E7C6C,part(a,0x42C));
        gabi::call(0x025E80D0,part(a,0x4A0));
        // Three embedded lighting records copy the common constructor defaults.
        for(u32 base : {0x520u,0x5E0u,0x664u}) {
            for(u32 offset=0;offset<0x44;offset+=4) {
                if(offset>=0x18 && offset<0x24)
                    gabi::store<u32>(ea(a)+base+offset,gabi::load<u32>(0x1016E414+offset));
                else
                    gabi::store<f32>(ea(a)+base+offset,gabi::load<f32>(0x1016E414+offset));
            }
        }
    }
    return a;
}
VERIFY(0x0235ED28,construct);
static void setMtx(Actor* a) {
    WWHD_FUNC(0x0235EEF8,void,a);
    f32 x=a->scale.x,y=a->scale.y,z=a->scale.z;
    u32 model=a->model;
    gabi::store<f32>(model+0xBC,x); gabi::store<f32>(model+0xC0,y); gabi::store<f32>(model+0xC4,z);
    x=a->current.pos.x; y=a->current.pos.y; z=a->current.pos.z;
    gabi::call(0x028E93CC,gabi::at<u8>(0x1048D0CC),x,y,z);
    gabi::call(0x025F1C28,gabi::at<u8>(0x1048D0CC),(s16)a->current.angle.y);
    f32 matrix[12]; for(u32 i=0;i<12;i++) matrix[i]=gabi::load<f32>(0x1048D0CC+4*i);
    model=a->model;
    for(u32 i=0;i<12;i++) gabi::store<f32>(model+0xC8+4*i,matrix[i]);
}
VERIFY(0x0235EEF8,setMtx);
static u32 spawn(Actor* a,u32 id) {
    u32 controller=gabi::load<u32>(play()+0x5AB0);
    return gabi::call<u32>(0x025A847C,gabi::at<u8>(controller),0,id,part(a,0x314),part(a,0x320),0,255,0,-1,0,0,0);
}
static void particleSet(Actor* a) {
    WWHD_FUNC(0x0235EFD0,void,a);
    if(!a->emitter1) a->emitter1=spawn(a,0x81AA);
    if(!a->emitter2) a->emitter2=spawn(a,0x81AB);
}
VERIFY(0x0235EFD0,particleSet);
static void createInit(Actor* a) {
    WWHD_FUNC(0x0235F0A0,void,a);
    u32 model=a->model;
    gabi::store<u32>(ea(a)+0x348,model?model+0xC8:0);
    gabi::store<f32>(ea(a)+0x364,1.0f);
    gabi::call(0x0235EEF8,a);
    gabi::call(0x0255FFF4,part(a,0x520),(s8)a->home.roomNo,255);
    u32 light=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,gabi::at<u8>(light),1,part(a,0x314),part(a,0x520));
    a->emitter1=0; a->emitter2=0;
    if(isSwitch(a)) {
        gabi::store<f32>(ea(a)+0x4A4,(f32)gabi::load<s16>(ea(a)+0x4AA));
        u32 save=gabi::load<u32>(0x101F84DC);
        if(gabi::load<s16>(save+0x115C)==2 && (s8)a->current.roomNo==gabi::load<s8>(save+0x1148)) a->state=6;
        else a->state=3;
        a->windVolume=0;
    } else {
        gabi::call(0x0235EFD0,a); a->state=0; a->windVolume=100;
    }
    u32 p=play(); a->meltEvent=gabi::call<s16>(0x02543F10,gabi::at<u8>(p+0x52C4),STR(0x1002B280),255);
    p=play(); a->freezeEvent=gabi::call<s16>(0x02543F10,gabi::at<u8>(p+0x52C4),STR(0x1002B28C),255);
}
VERIFY(0x0235F0A0,createInit);
static s32 create(Actor* a) {
    WWHD_FUNC(0x0235F294,s32,a);
    u32 condition=a->actor_condition;
    if(!(condition&8)) {
        if(a) { gabi::call(0x0235ED28,a); condition=a->actor_condition; }
        a->actor_condition=condition|8;
    }
    s32 phase=gabi::call<s32>(0x02520460,part(a,0x3AC),STR(0x1002B1EC));
    if(phase==4) {
        if(!gabi::call<BOOL>(0x025D63E8,a,0x0235ED24,0x13D0)) return 5;
        gabi::call(0x0235F0A0,a);
    }
    return phase;
}
VERIFY(0x0235F294,create);
static BOOL remove(Actor* a) { WWHD_FUNC(0x0235F34C,BOOL,a); gabi::call(0x025204C8,part(a,0x3AC),STR(0x1002B298)); return TRUE; }
VERIFY(0x0235F34C,remove);
static BOOL draw(Actor* a) {
    WWHD_FUNC(0x0235F37C,BOOL,a);
    u32 light=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,gabi::at<u8>(light),1,part(a,0x314),part(a,0x520));
    light=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,gabi::at<u8>(light),3,part(a,0x314),part(a,0x110));
    light=gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C,gabi::at<u8>(light),gabi::at<u8>((u32)a->model),part(a,0x110));
    f32 frame=gabi::load<f32>(ea(a)+0x3BC);u32 model=a->model;
    gabi::call(0x025E8048,part(a,0x3B8),gabi::at<u8>(model),frame);
    frame=gabi::load<f32>(ea(a)+0x430);model=a->model;
    gabi::call(0x025E8048,part(a,0x42C),gabi::at<u8>(model),frame);
    model=a->model;frame=gabi::load<f32>(ea(a)+0x4A4);
    gabi::call(0x025E83FC,part(a,0x4A0),gabi::at<u8>(gabi::load<u32>(model+0xAC)),frame);
    gabi::call(0x025E2DE0,gabi::at<u8>((u32)a->model),0);return TRUE;
}
VERIFY(0x0235F37C,draw);
static void freezeMain(Actor* a) { WWHD_FUNC(0x0235F420,void,a); if(isSwitch(a)) { order(a,a->meltEvent);a->state=1; } }
VERIFY(0x0235F420,freezeMain);
static void retire(u32 emitter) { u32 flags=gabi::load<u32>(emitter+0x254);gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);gabi::store<u32>(emitter+0x254,flags|1); }
static void meltWait(Actor* a) {
    WWHD_FUNC(0x0235F498,void,a);
    if(gabi::load<u16>(ea(a)+0xF8)==2) {
        gabi::call(0x025E1988,0x806);
        u32 emitter=a->emitter1;if(emitter) { retire(emitter);a->emitter1=0; }
        emitter=a->emitter2;if(emitter) { retire(emitter);a->emitter2=0; }
        a->state=2;gabi::store<f32>(ea(a)+0x4A0,1.0f);
    } else order(a,a->meltEvent);
}
VERIFY(0x0235F498,meltWait);
static void finishDemo(Actor* a,u32 offset,u32 state) {
    gabi::call(0x025E742C,part(a,0x4A0));s16 event=gabi::load<s16>(ea(a)+offset);
    u32 p=play();if(gabi::call<BOOL>(0x025440C8,gabi::at<u8>(p+0x52C4),event)) {
        p=play();u16 flags=gabi::load<u16>(p+0x52B8);gabi::store<u16>(p+0x52B8,flags|8);a->state=state;
    }
}
static void meltDemo(Actor* a) { WWHD_FUNC(0x0235F574,void,a);finishDemo(a,0x6EC,3); }
VERIFY(0x0235F574,meltDemo);
static void meltMain(Actor* a) { WWHD_FUNC(0x0235F5E0,void,a);if(!isSwitch(a)) { order(a,a->freezeEvent);a->state=4; } }
VERIFY(0x0235F5E0,meltMain);
static void freezeWait(Actor* a) { WWHD_FUNC(0x0235F658,void,a);if(gabi::load<u16>(ea(a)+0xF8)==2) { gabi::call(0x0235EFD0,a);a->state=5;gabi::store<f32>(ea(a)+0x4A0,-1.0f); }else order(a,a->freezeEvent); }
VERIFY(0x0235F658,freezeWait);
static void freezeDemo(Actor* a) { WWHD_FUNC(0x0235F6D4,void,a);finishDemo(a,0x6EE,0); }
VERIFY(0x0235F6D4,freezeDemo);
static void failWait(Actor* a) { WWHD_FUNC(0x0235F740,void,a);if(!isSwitch(a)) { gabi::call(0x0235EFD0,a);a->state=7;gabi::store<f32>(ea(a)+0x4A0,1.0f); } }
VERIFY(0x0235F740,failWait);
static u32 failMain(Actor* a) { WWHD_FUNC(0x0235F7AC,u32,a);return gabi::call<u32>(0x025E742C,part(a,0x4A0)); }
VERIFY(0x0235F7AC,failMain);
static u8 toSrgb(u32 channel) {
    WWHD_FUNC(0x0235F7B4,u8,channel);
    f32 value=gabi::call<f32>(0x028F4560,(f32)channel/255.0f,gabi::load<f32>(0x1002B2B0));
    if(value<0) value=0;else if(value>1) value=1;
    return (u8)gabi::ftoi(value*255.0f);
}
VERIFY(0x0235F7B4,toSrgb);
static f32 toLinear(u32 channel) {
    WWHD_FUNC(0x0235F870,f32,channel);
    f32 value=gabi::call<f32>(0x028F4560,(f32)channel/255.0f,gabi::load<f32>(0x1002B2B8));
    if(value<0) value=0;else if(value>1) value=1;
    return value;
}
VERIFY(0x0235F870,toLinear);
static BOOL execute(Actor* a) {
    WWHD_FUNC(0x0235F8F0,BOOL,a);
    static constexpr u32 stateFunctions[]={0x0235F420,0x0235F498,0x0235F574,0x0235F5E0,0x0235F658,0x0235F6D4,0x0235F740,0x0235F7AC};
    u32 state=a->state;if(state<8) gabi::call(stateFunctions[state],a);
    gabi::call(0x025E742C,part(a,0x3B8));gabi::call(0x025E742C,part(a,0x42C));gabi::call(0x0235EEF8,a);
    BOOL active=isSwitch(a);
    s8 room=a->current.roomNo;s16 volume=a->windVolume;
    if(active) { if(volume>0) { volume=(s16)(volume-1);a->windVolume=volume; } }
    else if(volume<100) { volume=(s16)(volume+1);a->windVolume=volume; }
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call(0x025E1A40,0x1072,part(a,0x314),volume,reverb);
    if(a->emitter1) {
        u8 channels[3];
        for(u32 i=0;i<3;i++) channels[i]=(gabi::call<u8>(0x0235F7B4,gabi::load<u8>(ea(a)+0x5B8+i))>>1)+0x80;
        for(u32 i=0;i<3;i++) channels[i]=(u8)gabi::ftoi(gabi::call<f32>(0x0235F870,(u32)channels[i])*255.0f);
        u32 emitter=a->emitter1;for(u32 i=0;i<3;i++) gabi::store<u8>(emitter+0x244+i,channels[i]);
        emitter=a->emitter1;for(u32 i=0;i<3;i++) gabi::store<u8>(emitter+0x248+i,channels[i]);
    }
    return TRUE;
}
VERIFY(0x0235F8F0,execute);
static void staticInit() {
    WWHD_FUNC(0x0235FD7C,void,(u32)0);
    for(u32 offset : {8u,0u,12u,4u}) gabi::store<u32>(0x1046A08C+offset,0);
    gabi::call(0x028F026C,gabi::at<u8>(0x101CA428));
    gabi::store<f32>(0x1046A080,gabi::load<f32>(0x1002B2C0));gabi::store<f32>(0x1046A084,gabi::load<f32>(0x1002B2C4));
    gabi::call(0x028ED6F8,gabi::at<u8>(0x1046A088));gabi::call(0x028F026C,gabi::at<u8>(0x101CA434));
    gabi::call(0x028EAB2C,gabi::at<u8>(0x1046A089));gabi::call(0x028F026C,gabi::at<u8>(0x101CA440));
}
VERIFY(0x0235FD7C,staticInit);
static void trivialDestructor(void* a,s32 flags) { WWHD_FUNC(0x0235FE10,void,a,flags);if(a&&(flags&1)) gabi::call(0x0273AF40,a); }
VERIFY(0x0235FE10,trivialDestructor);
static BOOL isDelete(Actor* a) { WWHD_FUNC(0x0235FE24,BOOL,a);return TRUE; }
VERIFY(0x0235FE24,isDelete);
}

/* ---- leftover functions of the translation unit ---- */

/* 0235FE2C daObjIceisland_c::~daObjIceisland_c (deleting; vtable slot 1002B218): only the fopAc_ac_c base */
static void daObjIceisland_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0235FE2C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x0235FE2C, daObjIceisland_c_dt);

/* 0235FE80 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002B208, after the destructor 0235FE10 */
static void iceisland_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x0235FE80, void, p);
}
VERIFY(0x0235FE80, iceisland_SafeString_assureTermination);
