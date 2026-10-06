#include "d/actor/d_a_obj_ashut.h"
#include "bindings.h"
#include <cmath>
using daObjAshut::Act_c;
template<class T> static T read(u32 address) { return *gabi::at<be<T>>(address); }
template<class T> static void write(u32 address,T value) { *gabi::at<be<T>>(address)=value; }
static u32 offset(Act_c* actor,u32 displacement) { return gabi::ea(actor)+displacement; }
static bool isSwitch(Act_c* actor) {
    u32 bit=gabi::call<u32>(0x023197A0,actor,8,0);
    u32 save=read<u32>(0x101F84DC)+0x20;
    s8 room=actor->home.roomNo;
    return gabi::call<s32>(0x025BA0C0,gabi::at<void>(save),bit,room)!=0;
}

void ashut_actorDtor(Act_c* actor, s32 flags) {
    WWHD_FUNC(0x0231974C, void, actor, flags);
    if(actor) { gabi::call<void>(0x025D50BC,actor,0); if(flags & 1) gabi::call<void>(0x0273AF40,actor); }
}
VERIFY(0x0231974C, ashut_actorDtor);

s32 ashut_create(Act_c* actor) {
    WWHD_FUNC(0x02318A74, s32, actor);
    u32 condition=actor->actor_condition;
    if(!(condition&8)) {
        if(actor) { gabi::call<void>(0x024F1D40,actor); actor->__vtbl=0x100251E0; condition=actor->actor_condition; }
        actor->actor_condition=condition|8;
    }
    s32 phase=gabi::call<s32>(0x02520460,&actor->phase,STR(0x100251D8));
    if(phase==4) {
        phase=gabi::call<s32>(0x024F1D9C,actor,STR(0x100251D8),7,0,0x760);
        if(phase!=4 && phase!=5) gabi::call<void>(0x0273AA24,STR(0x1002508C),0x137,STR(0x100250A0));
        if(isSwitch(actor) && read<u32>(actor->background)<0x100) {
            u32 play=gabi::call<u32>(0x025200D4); u32 bg=actor->background;
            gabi::call<void>(0x020087EC,gabi::at<void>(play+0x12A0),gabi::at<void>(bg));
        }
    }
    return phase;
}
VERIFY(0x02318A74, ashut_create);

s32 ashut_remove(Act_c* actor) {
    WWHD_FUNC(0x02318B98, s32, actor);
    s32 result=gabi::call<s32>(0x024F1F64,actor);
    gabi::call<void>(0x025204C8,&actor->phase,STR(0x100251D8)); return result;
}
VERIFY(0x02318B98, ashut_remove);

s32 ashut_createHeap(Act_c* actor) {
    WWHD_FUNC(0x02318BE4, s32, actor);
    gabi::Local<SafeString> name; name->mStringTop=0x100251D8; name->__vtbl=0x10025074;
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),name.get(),4);
    if(!resource) gabi::call<void>(0x0273AA24,STR(0x100250E4),0xF9,STR(0x100250F8));
    u32 model=gabi::call<u32>(0x025E38E0,gabi::at<void>(resource),0x80000,0x11000022);
    actor->model=model; return model!=0;
}
VERIFY(0x02318BE4, ashut_createHeap);

void ashut_setMtx(Act_c* actor) {
    WWHD_FUNC(0x02318C80, void, actor);
    u32 stack=0x1048D0CC;
    f32 x=actor->current.pos.x,y=actor->current.pos.y,z=actor->current.pos.z;
    gabi::call<void>(0x028E93CC,gabi::at<void>(stack),x,y,z);
    s16 rx=actor->shape_angle.x,ry=actor->shape_angle.y,rz=actor->shape_angle.z;
    gabi::call<void>(0x025F1B48,gabi::at<void>(stack),rx,ry,rz);
    gabi::call<void>(0x028E90D4,gabi::at<void>(stack),gabi::at<void>(0x104690F4));
    f32 values[12];
    values[7]=read<f32>(stack+28)+f32(actor->height); write<f32>(stack+28,values[7]);
    u32 model=actor->model;
    for(unsigned i=0;i<12;++i) if(i!=7) values[i]=read<f32>(stack+i*4);
    for(unsigned i=0;i<12;++i) write<f32>(model+0xC8+i*4,values[i]);
}
VERIFY(0x02318C80, ashut_setMtx);

void ashut_initMtx(Act_c* actor) {
    WWHD_FUNC(0x02318D60, void, actor);
    u32 model=actor->model,base=gabi::ea(actor);
    u32 y=read<u32>(base+0x334),x=read<u32>(base+0x330),z=read<u32>(base+0x338);
    write<u32>(model+0xBC,x);write<u32>(model+0xC0,y);write<u32>(model+0xC4,z);
    gabi::call<void>(0x02318C80,actor);
}
VERIFY(0x02318D60, ashut_initMtx);

void ashut_upperInit(Act_c* actor) {
    WWHD_FUNC(0x02318D80, void, actor);
    actor->mode=0; actor->height=250.0f;
}
VERIFY(0x02318D80, ashut_upperInit);

void ashut_lowerInit(Act_c* actor) {
    WWHD_FUNC(0x02318D98, void, actor);
    actor->mode=2; actor->height=0.0f;
}
VERIFY(0x02318D98, ashut_lowerInit);

s32 ashut_createInit(Act_c* actor) {
    WWHD_FUNC(0x02318DB0, s32, actor);
    write<u8>(u32(actor->background)+0x75,1);
    u32 model=actor->model; actor->cullMtx=model?model+0xC8:0;
    gabi::call<void>(0x02318D60,actor);
    gabi::call<void>(0x025D674C,actor,-80.0f,-5.0f,-30.0f,80.0f,250.0f,30.0f);
    actor->demoStarted=0;
    u8 event=gabi::call<u32>(0x023197A0,actor,8,8);
    u32 play=gabi::call<u32>(0x025200D4);
    actor->eventIndex=gabi::call<s32>(0x02543F10,gabi::at<void>(play+0x52C4),0,event);
    actor->requestedMode=5;
    if(isSwitch(actor)) gabi::call<void>(0x02318D80,actor); else gabi::call<void>(0x02318D98,actor);
    return 1;
}
VERIFY(0x02318DB0, ashut_createInit);

s32 ashut_execute(Act_c* actor, u32 output) {
    WWHD_FUNC(0x02318ED0, s32, actor, output);
    u32 entry=0x10025124+u32(actor->mode)*8;
    s16 adjustment=read<s16>(entry),index=read<s16>(entry+2);
    u32 target,thisAddress=offset(actor,adjustment);
    if(index<0) target=read<u32>(entry+4);
    else { s16 vtableOffset=read<s16>(entry+6); u32 vtable=read<u32>(thisAddress+vtableOffset); target=read<u32>(vtable+u32(index)*8+4); }
    gabi::call<void>(target,gabi::at<void>(thisAddress));
    f32 height=actor->height,y=actor->current.pos.y,z=actor->current.pos.z,x=actor->current.pos.x;
    actor->eyePos.z=z;actor->eyePos.x=x;actor->eyePos.y=gabi::fadds_ppc(y,height);
    gabi::call<void>(0x02318C80,actor);write<u32>(output,0x104690F4);return 1;
}
VERIFY(0x02318ED0, ashut_execute);

s32 ashut_draw(Act_c* actor) {
    WWHD_FUNC(0x02318FC4, s32, actor);
    u32 environment=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x025626A4,gabi::at<void>(environment),1,&actor->current.pos,&actor->tevStr);
    environment=gabi::call<u32>(0x02555D0C);u32 model=actor->model;
    gabi::call<void>(0x02562F5C,gabi::at<void>(environment),gabi::at<void>(model),&actor->tevStr);
    model=actor->model;gabi::call<void>(0x025E2DE0,gabi::at<void>(model),0);return 1;
}
VERIFY(0x02318FC4, ashut_draw);

u8 ashut_safeArea(Act_c* actor) {
    WWHD_FUNC(0x02319020, u8, actor);
    s16 angle=-s16(actor->shape_angle.y);
    u32 play=gabi::call<u32>(0x025200D4),player=read<u32>(play+0x5B2C);
    gabi::Local<cXyz> difference,local;
    gabi::call<void>(0x0201ADE0,gabi::at<void>(player+0x314),difference.get(),&actor->current.pos);
    gabi::call<void>(0x025F1884,gabi::at<void>(0x1048D0CC),angle);
    gabi::call<void>(0x028E9044,gabi::at<void>(0x1048D0CC),difference.get(),local.get());
    return (std::fabs(f32(local->y))<100.0f) && (std::fabs(f32(local->z))<55.0f) && (std::fabs(f32(local->x))<105.0f);
}
VERIFY(0x02319020, ashut_safeArea);

void ashut_downInit(Act_c* actor) {
    WWHD_FUNC(0x023190E8, void, actor);
    s8 room=actor->current.roomNo;actor->bounceCount=5;actor->velocity=0.0f;
     actor->mode=1;
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,27038,&actor->eyePos,0,reverb);
}
VERIFY(0x023190E8, ashut_downInit);

void ashut_upInit(Act_c* actor) {
    WWHD_FUNC(0x0231914C, void, actor);
    s8 room=actor->current.roomNo;actor->bounceCount=5;actor->velocity=0.0f;
    actor->delay=14; actor->mode=3;
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,27037,&actor->eyePos,0,reverb);
}
VERIFY(0x0231914C, ashut_upInit);

void ashut_demoInit(Act_c* actor, s32 mode) {
    WWHD_FUNC(0x023191B8, void, actor, mode);
    if(mode!=1 && mode!=3) gabi::call<void>(0x0273AA24,STR(0x10025158),0x221,STR(0x1002516C));
    if(actor->demoStarted) gabi::call<void>(mode==1?0x023190E8:0x0231914C,actor);
    else {actor->requestedMode=mode;actor->mode=4;}
}
VERIFY(0x023191B8, ashut_demoInit);

void ashut_upper(Act_c* actor) {
    WWHD_FUNC(0x02319270, void, actor);
    if(!isSwitch(actor) && !gabi::call<u8>(0x02319020,actor)) {
        if(read<u32>(actor->background)>=0x100) {u32 play=gabi::call<u32>(0x025200D4);u32 bg=actor->background;gabi::call<void>(0x024EEA6C,gabi::at<void>(play+0x12A0),gabi::at<void>(bg),actor);}
        gabi::call<void>(0x023191B8,actor,1);
    }
}
VERIFY(0x02319270, ashut_upper);

void ashut_down(Act_c* actor) {
    WWHD_FUNC(0x02319308, void, actor);
    f32 velocity=(f32(actor->velocity)-2.5f)*0.95f,height=f32(actor->height)+velocity;
    actor->velocity=velocity;actor->height=height;
    if(height>0.0f) return;
    u8 count=actor->bounceCount;
    if(!count) {gabi::call<void>(0x02318D98,actor);return;}
    height=f32(actor->height)*-0.5f;velocity=f32(actor->velocity)*-0.5f;
    actor->height=height;actor->bounceCount=count-1;actor->velocity=velocity;
    if(velocity>7.0f) actor->velocity=7.0f;
}
VERIFY(0x02319308, ashut_down);

void ashut_lower(Act_c* actor) {
    WWHD_FUNC(0x02319394, void, actor);
    if(isSwitch(actor)) gabi::call<void>(0x023191B8,actor,3);
}
VERIFY(0x02319394, ashut_lower);

void ashut_up(Act_c* actor) {
    WWHD_FUNC(0x023193F8, void, actor);
    s16 delay=actor->delay;if(delay>0) actor->delay=delay-1;
    f32 velocity=f32(actor->velocity)+0.8f;delay=actor->delay;actor->velocity=velocity;
    velocity*=1.0f-((delay>0 && delay<4)?0.4f:0.1f);
    f32 height=f32(actor->height)+velocity;actor->height=height;actor->velocity=velocity;
    if(height>150.0f && read<u32>(actor->background)<0x100) {
        u32 play=gabi::call<u32>(0x025200D4);u32 bg=actor->background;
        gabi::call<void>(0x020087EC,gabi::at<void>(play+0x12A0),gabi::at<void>(bg));height=actor->height;
    }
    if(height<250.0f) return;
    u8 count=actor->bounceCount;
    if(!count) {gabi::call<void>(0x02318D80,actor);return;}
    height=actor->height;velocity=f32(actor->velocity)*-0.6f;
    height=gabi::fmadds(height-250.0f,-0.6f,250.0f);
    actor->bounceCount=count-1;actor->velocity=velocity;actor->height=height;
    if(velocity<-4.0f) actor->velocity=-4.0f;
}
VERIFY(0x023193F8, ashut_up);

void ashut_demo(Act_c* actor) {
    WWHD_FUNC(0x0231956C, void, actor);
    s16 event=actor->eventIndex;u32 play=gabi::call<u32>(0x025200D4);
    u32 data=gabi::call<u32>(0x02544044,gabi::at<void>(play+0x52C4),event);
    if(data && read<u16>(offset(actor,0xF8))!=2) {
        u8 id=gabi::call<u32>(0x023197A0,actor,8,8);event=actor->eventIndex;
        gabi::call<void>(0x025D7A58,actor,event,id,0xFFFF,0,1);
        write<u16>(offset(actor,0xFA),read<u16>(offset(actor,0xFA))|2);return;
    }
    if(data) actor->demoStarted=1;
    s32 mode=actor->requestedMode;gabi::call<void>(mode==1?0x023190E8:0x0231914C,actor);
}
VERIFY(0x0231956C, ashut_demo);

s32 ashut_createWrapper(Act_c* actor) {
    WWHD_FUNC(0x02319664, s32, actor);
    return gabi::call<s32>(0x02318A74,actor);
}
VERIFY(0x02319664, ashut_createWrapper);

s32 ashut_deleteWrapper(Act_c* actor) {
    WWHD_FUNC(0x02319668, s32, actor);
    return gabi::call<s32>(0x02318B98,actor);
}
VERIFY(0x02319668, ashut_deleteWrapper);

s32 ashut_executeWrapper(Act_c* actor) {
    WWHD_FUNC(0x0231966C, s32, actor);
    return gabi::call<s32>(0x024F1E9C,actor);
}
VERIFY(0x0231966C, ashut_executeWrapper);

s32 ashut_drawWrapper(Act_c* actor) {
    WWHD_FUNC(0x02319670, s32, actor);
    u32 target=read<u32>(u32(actor->__vtbl)+44);return gabi::call<s32>(target,actor);
}
VERIFY(0x02319670, ashut_drawWrapper);

s32 ashut_isDeleteWrapper(Act_c* actor) {
    WWHD_FUNC(0x02319680, s32, actor);
    u32 target=read<u32>(u32(actor->__vtbl)+60);return gabi::call<s32>(target,actor);
}
VERIFY(0x02319680, ashut_isDeleteWrapper);

void ashut_staticInit() {
    WWHD_FUNC(0x02319690, void);
    write<u32>(0x104690EC,0);write<u32>(0x104690E4,0);write<u32>(0x104690F0,0);write<u32>(0x104690E8,0);
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C7C74));
    write<f32>(0x104690D8,-3.1415927410125732f);write<f32>(0x104690DC,3.1415927410125732f);
    gabi::call<void>(0x028ED6F8,gabi::at<void>(0x104690E0));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C7C80));
    gabi::call<void>(0x028EAB2C,gabi::at<void>(0x104690E1));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C7C8C));
}
VERIFY(0x02319690, ashut_staticInit);

void ashut_staticDtor(void* object, s32 flags) {
    WWHD_FUNC(0x02319724, void, object, flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02319724, ashut_staticDtor);

s32 ashut_isDelete(Act_c* actor) {
    WWHD_FUNC(0x02319738, s32, actor);
    return 1;
}
VERIFY(0x02319738, ashut_isDelete);

s32 ashut_delete(Act_c* actor) {
    WWHD_FUNC(0x02319744, s32, actor);
    return 1;
}
VERIFY(0x02319744, ashut_delete);

u32 ashut_parameter(Act_c* actor, u32 width, u32 shift) {
    WWHD_FUNC(0x023197A0, u32, actor, width, shift);
    u32 mask=width&0x20?0:(1u<<(width&31));
    u32 value=shift&0x20?0:(u32(actor->mParameters)>>(shift&31));return value&(mask-1);
}
VERIFY(0x023197A0, ashut_parameter);

/* ---- leftover functions of the translation unit ---- */

/* 02319740 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10025088, after the destructor 02319724 */
static void ashut_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02319740, void, p);
}
VERIFY(0x02319740, ashut_SafeString_assureTermination);
