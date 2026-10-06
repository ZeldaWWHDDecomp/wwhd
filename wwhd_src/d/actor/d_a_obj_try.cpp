#include "d/actor/d_a_obj_try.h"
#include "bindings.h"
#include <cmath>
using Actor=daObjTry_c;
template<class T> static T read(u32 address) {return gabi::load<T>(address);}
template<class T> static void write(u32 address,T value) {gabi::store<T>(address,value);}
static void* member(Actor* actor,u32 offset) {return gabi::at<void>(gabi::ea(actor)+offset);}
static u32 checkedType(Actor* actor,u32 file,u32 message) {
    u32 type=actor->type;
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(file),0x3CE,STR(message));type=actor->type;}
    return type;
}
static void copyMatrix(u32 model) {
    u32 words[12];
    for(unsigned i=0;i<12;++i) words[i]=read<u32>(0x1048D0CC+i*4);
    for(unsigned i=0;i<12;++i) write<u32>(model+0xC8+i*4,words[i]);
}

void try_setMatrix(Actor* actor) {
    WWHD_FUNC(0x023ABA4C,void,actor);
    f32 x=actor->current.pos.x,z=actor->current.pos.z,y=actor->current.pos.y;
    gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
    s16 rx=actor->shape_angle.x,rz=actor->shape_angle.z,ry=actor->shape_angle.y;
    gabi::call<void>(0x025F1B48,gabi::at<void>(0x1048D0CC),rx,ry,rz);
    copyMatrix(actor->model);
}
VERIFY(0x023ABA4C,try_setMatrix);
s32 try_delete(Actor* actor) {
    WWHD_FUNC(0x023ABF90,s32,actor);
    if(actor->appears) {
        u32 vtable=read<u32>(gabi::ea(actor)+0x7D0),target=read<u32>(vtable+0x44);
        gabi::call<void>(target,member(actor,0x7D0));
        u32 emitter=actor->emitter;
        if(emitter) {u32 flags=read<u32>(emitter+0x254);write<u32>(emitter+0x5C,0xFFFFFFFFu);write<u32>(emitter+0x254,flags|1);}
        u32 type=actor->type;
        if(type==5 || type==6) {
            u32 save=read<u32>(0x101F84DC);
            gabi::call<void>(0x025B8B7C,gabi::at<void>(save+0x1178),type==5?0x108:0x110);
        }
        write<u32>(gabi::ea(actor)+0x368,0);
        gabi::call<void>(0x02520488,STR(0x100321B0));
    }
    return 1;
}
VERIFY(0x023ABF90,try_delete);
void try_waterSplash(Actor* actor) {
    WWHD_FUNC(0x023AC3D4,void,actor);
    gabi::Local<cXyz> position;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    u32 x=read<u32>(gabi::ea(actor)+0x314),z=read<u32>(gabi::ea(actor)+0x31C),water=read<u32>(gabi::ea(actor)+0x5EC);
    write<u32>(gabi::ea(position.get())+4,water);write<u32>(gabi::ea(position.get()),x);write<u32>(gabi::ea(position.get())+8,z);
    gabi::call<void>(0x025DAE64,position.get(),0,1.0f,0.75f);
}
VERIFY(0x023AC3D4,try_waterSplash);
void try_staticInit() {
    WWHD_FUNC(0x023AE798,void);
    write<u32>(0x1046C7A8,0);write<u32>(0x1046C7A0,0);write<u32>(0x1046C7AC,0);write<u32>(0x1046C7A4,0);
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101CD73C));
    write<u32>(0x1046C794,read<u32>(0x100321A8));write<u32>(0x1046C798,read<u32>(0x100321AC));
    gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1046C79C));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101CD748));
    gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1046C79D));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101CD754));
}
VERIFY(0x023AE798,try_staticInit);
void try_destructor(Actor* actor,s32 flags) {
    WWHD_FUNC(0x023AE844,void,actor,flags);
    if(!actor) return;
    gabi::call<void>(0x02515A70,member(actor,0x670),2);
    gabi::call<void>(0x02515860,member(actor,0x634),2);
    gabi::call<void>(0x02018034,member(actor,0x608),2);
    write<u32>(gabi::ea(actor)+0x450,0x10031CC0);write<u32>(gabi::ea(actor)+0x444,0x10031CD0);
    gabi::call<void>(0x024EFD9C,member(actor,0x430),0);
    gabi::call<void>(0x025D50BC,actor,0);
    if(flags&1) gabi::call<void>(0x0273AF40,actor);
}
VERIFY(0x023AE844,try_destructor);
void try_damaged(Actor* actor) {WWHD_FUNC(0x023AC7E8,void,actor);gabi::call<void>(0x025D9D24,actor);}
VERIFY(0x023AC7E8,try_damaged);
void try_initCollision(Actor* actor) {
    WWHD_FUNC(0x023AB8A0,void,actor);
    u32 type=checkedType(actor,0x10031D88,0x10031D98);
    u8 weight=read<u8>(0x100321FC+type*0x78+12);
    gabi::call<void>(0x02515F14,member(actor,0x634),weight,255,actor);
    gabi::call<void>(0x02516518,member(actor,0x670),gabi::at<void>(0x100321B8));
    type=actor->type;write<u32>(gabi::ea(actor)+0x6B4,gabi::ea(actor)+0x634);
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031D88),0x3CE,STR(0x10031D98));type=actor->type;}
    f32 radius=f32(read<u8>(0x100321FC+type*0x78+0x4B));
    gabi::call<void>(0x020184DC,member(actor,0x788),radius);
    type=checkedType(actor,0x10031D88,0x10031D98);
    f32 height=f32(read<u8>(0x100321FC+type*0x78+0x4C));
    gabi::call<void>(0x02018428,member(actor,0x788),height);
    write<u32>(gabi::ea(actor)+0x6EC,read<u32>(0x101FFBA8));write<u32>(gabi::ea(actor)+0x6F0,read<u32>(0x101FFBAC));write<u32>(gabi::ea(actor)+0x6F4,read<u32>(0x101FFBB0));
    u32 x=read<u32>(0x101FFBA8),flags=read<u32>(gabi::ea(actor)+0x704);write<u32>(gabi::ea(actor)+0x724,x);
    write<u32>(gabi::ea(actor)+0x728,read<u32>(0x101FFBAC));u32 z=read<u32>(0x101FFBB0);
    write<u32>(gabi::ea(actor)+0x704,flags|1);write<u32>(gabi::ea(actor)+0x72C,z);
}
VERIFY(0x023AB8A0,try_initCollision);
s32 try_playerNearby(Actor* actor) {
    WWHD_FUNC(0x023AD0E0,s32,actor);
    gabi::Local<cXyz> difference,horizontal;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    u32 play=gabi::call<u32>(0x025200D4),player=read<u32>(play+0x5B2C);
    gabi::call<void>(0x0201ADE0,gabi::at<void>(player+0x314),difference.get(),&actor->current.pos);
    u32 x=read<u32>(gabi::ea(difference.get()));write<u32>(gabi::ea(horizontal.get()),x);
    u32 z=read<u32>(gabi::ea(difference.get())+8);write<u32>(gabi::ea(horizontal.get())+4,0);write<u32>(gabi::ea(horizontal.get())+8,z);
    f64 square=gabi::call<f64>(0x028E8DD0,horizontal.get());
    f64 distance=gabi::call<f64>(0x028F4384,square);
    return !(distance>190.0);
}
VERIFY(0x023AD0E0,try_playerNearby);
void try_vibration(Actor* actor) {
    WWHD_FUNC(0x023AD160,void,actor);
    u32 nearby=gabi::call<u32>(0x023AD0E0,actor)&1,play=gabi::call<u32>(0x025200D4);
    gabi::Local<cXyz> direction;direction->z=0.0f;direction->y=1.0f;direction->x=0.0f;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    gabi::call<void>(0x025CB374,gabi::at<void>(play+0x599C),nearby+1,1,direction.get());
}
VERIFY(0x023AD160,try_vibration);
s32 try_createHeap(Actor* actor) {
    WWHD_FUNC(0x023AB6B0,s32,actor);
    gabi::Local<SafeString> modelName,animationName;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    u32 type=checkedType(actor,0x10031D30,0x10031D50);
    modelName->__vtbl=0x10031C18;modelName->mStringTop=0x100321B0;
    s16 modelIndex=read<s16>(0x100321FC+type*0x78+0x46);
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),modelName.get(),modelIndex);
    if(!resource) gabi::call<void>(0x0273AA24,STR(0x10031D30),0x3F2,STR(0x10031D40));
    u32 model=gabi::call<u32>(0x025E38E0,gabi::at<void>(resource),0,0x11020203);
    type=actor->type;actor->model=model;s32 animationReady=1;
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031D30),0x3CE,STR(0x10031D50));type=actor->type;}
    s16 animationIndex=read<s16>(0x100321FC+type*0x78+0x48);
    if(animationIndex>=0) {
        if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031D30),0x3CE,STR(0x10031D50));type=actor->type;}
        animationName->mStringTop=0x100321B0;animationName->__vtbl=0x10031C18;
        animationIndex=read<s16>(0x100321FC+type*0x78+0x48);
        u32 animation=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),animationName.get(),animationIndex);
        if(!animation) gabi::call<void>(0x0273AA24,STR(0x10031D30),0x3FC,STR(0x10031D74));
        animationReady=gabi::call<s32>(0x025E8154,member(actor,0x3B8),gabi::at<void>(resource),gabi::at<void>(animation),1,2,0,-1,0,0,1.0f);
        if(!gabi::call<u32>(0x02333CBC,actor,1,31)) {
            s16 frame=read<s16>(gabi::ea(actor)+0x3C2);write<f32>(gabi::ea(actor)+0x3BC,f32(frame));
        }
    }
    return actor->model && animationReady?1:0;
}
VERIFY(0x023AB6B0,try_createHeap);
void try_drop(Actor* actor) {
    WWHD_FUNC(0x023AE438,void,actor);
    gabi::call<void>(0x02312968,actor,member(actor,0x504));
    gabi::call<void>(0x023ADBFC,actor);
    gabi::Local<f32> gravity,drag,stream;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    gabi::call<void>(0x023ADCD8,actor,gravity.get(),drag.get(),stream.get());
    u32 gravityBits=read<u32>(gabi::ea(gravity.get()));f32 streamValue=read<f32>(gabi::ea(stream.get())),dragValue=read<f32>(gabi::ea(drag.get()));
    write<u32>(gabi::ea(actor)+0x374,gravityBits);
    gabi::call<void>(0x023123C0,actor,member(actor,0x634),gabi::at<void>(0x101FFBA8),dragValue,streamValue);
}
VERIFY(0x023AE438,try_drop);
void try_carry(Actor* actor) {
    WWHD_FUNC(0x023AE4A8,void,actor);
    s16 timer=read<s16>(gabi::ea(actor)+0x7AC);if(timer>0) write<s16>(gabi::ea(actor)+0x7AC,s16(timer-1));
    u32 status=actor->actor_status;write<u32>(gabi::ea(actor)+0x340,0);
    if(!(status&0x2000)) {
        f32 forward=read<f32>(gabi::ea(actor)+0x370);
        if(forward>0.0f) {gabi::call<void>(0x023AE2D4,actor);gabi::call<void>(0x023AE438,actor);}
        else {gabi::call<void>(0x02312968,actor,member(actor,0x504));write<u8>(gabi::ea(actor)+0x7B2,2);gabi::call<void>(0x023ABB30,actor);}
    }
}
VERIFY(0x023AE4A8,try_carry);
void try_restartInit(Actor* actor) {
    WWHD_FUNC(0x023AC678,void,actor);
    u32 collider=read<u32>(gabi::ea(actor)+0x670),contact=read<u32>(gabi::ea(actor)+0x69C),hit=read<u32>(gabi::ea(actor)+0x688);
    write<u32>(gabi::ea(actor)+0x370,0);u32 acch=read<u32>(gabi::ea(actor)+0x458),homeX=read<u32>(gabi::ea(actor)+0x2EC);
    write<u32>(gabi::ea(actor)+0x670,collider&~1u);u32 type=actor->type;
    write<u32>(gabi::ea(actor)+0x688,hit&~1u);write<u32>(gabi::ea(actor)+0x69C,contact|1);
    write<s16>(gabi::ea(actor)+0x7AC,110);write<u32>(gabi::ea(actor)+0x458,(((acch|12)&~2u)|0x400)&~0x2000u);
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031F18),0x3CE,STR(0x10031F28));type=actor->type;}
    s16 homeYaw=read<s16>(gabi::ea(actor)+0x2FA);u8 offset=read<u8>(0x100321FC+type*0x78+0x4D);
    s16 homeXAngle=read<s16>(gabi::ea(actor)+0x2F8);u32 status=read<u32>(gabi::ea(actor)+0x39C);
    write<u32>(gabi::ea(actor)+0x314,homeX);u32 x=read<u32>(gabi::ea(actor)+0x314);
    s16 homeZAngle=read<s16>(gabi::ea(actor)+0x2FC);f32 homeY=read<f32>(gabi::ea(actor)+0x2F0),vertical=-10.0f-f32(offset);
    s16 yaw=s16(s32(homeYaw)-32768);write<s16>(gabi::ea(actor)+0x322,yaw);write<u32>(gabi::ea(actor)+0x300,x);
    u32 homeZ=read<u32>(gabi::ea(actor)+0x2F4);write<s16>(gabi::ea(actor)+0x30C,homeXAngle);write<u32>(gabi::ea(actor)+0x39C,status&~0x10u);
    write<s16>(gabi::ea(actor)+0x328,homeXAngle);write<s16>(gabi::ea(actor)+0x320,homeXAngle);
    write<f32>(gabi::ea(actor)+0x318,homeY+vertical);write<u32>(gabi::ea(actor)+0x31C,homeZ);u32 z=read<u32>(gabi::ea(actor)+0x31C);
    write<s16>(gabi::ea(actor)+0x30E,yaw);write<s16>(gabi::ea(actor)+0x310,homeZAngle);write<s16>(gabi::ea(actor)+0x32A,yaw);write<s16>(gabi::ea(actor)+0x32C,homeZAngle);
    u32 y=read<u32>(gabi::ea(actor)+0x318);actor->mode=0;write<u32>(gabi::ea(actor)+0x304,y);write<s16>(gabi::ea(actor)+0x324,homeZAngle);write<u32>(gabi::ea(actor)+0x308,z);
}
VERIFY(0x023AC678,try_restartInit);
void try_sinkInit(Actor* actor) {
    WWHD_FUNC(0x023AC424,void,actor);
    u32 hit=read<u32>(gabi::ea(actor)+0x688),collider=read<u32>(gabi::ea(actor)+0x670),acch=read<u32>(gabi::ea(actor)+0x458),contact=read<u32>(gabi::ea(actor)+0x69C),type=actor->type;
    write<u32>(gabi::ea(actor)+0x688,hit|1);write<u32>(gabi::ea(actor)+0x670,collider&~1u);write<u32>(gabi::ea(actor)+0x69C,contact|1);write<u32>(gabi::ea(actor)+0x458,((acch|8)&0xFFFFFBF9u)|0x2000);
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031EA8),0x3CE,STR(0x10031EB8));type=actor->type;}
    u32 attributes=0x100321FC+type*0x78,second=attributes;
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031EA8),0x3CE,STR(0x10031EB8));type=actor->type;second=0x100321FC+type*0x78;}
    f32 forward=read<f32>(gabi::ea(actor)+0x370),gravity=read<f32>(attributes),square=forward*forward,extra=read<f32>(second+0x24),vertical=read<f32>(gabi::ea(actor)+0x340);
    write<f32>(gabi::ea(actor)+0x374,gravity+extra);
    f64 speed=gabi::call<f64>(0x028F4384,gabi::fmadds(vertical,vertical,square));
    type=checkedType(actor,0x10031EA8,0x10031EB8);f32 limit=read<f32>(0x100321FC+type*0x78+0x30);
    if(speed>f64(limit)) {
        if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031EA8),0x3CE,STR(0x10031EB8));type=actor->type;}
        limit=read<f32>(0x100321FC+type*0x78+0x30);f32 scale=f32(f64(limit)/speed);
        gabi::call<void>(0x028E8E64,member(actor,0x33C),member(actor,0x33C),scale);
        forward=read<f32>(gabi::ea(actor)+0x370);write<f32>(gabi::ea(actor)+0x370,forward*scale);
    }
    u32 status=read<u32>(gabi::ea(actor)+0x39C);actor->mode=4;write<u32>(gabi::ea(actor)+0x39C,status&~0x10u);
}
VERIFY(0x023AC424,try_sinkInit);
f64 try_waterHeight(Actor* actor) {
    WWHD_FUNC(0x023AE544,f64,actor);
    struct Check {u8 storage[0x50];};gabi::Local<Check> check;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    gabi::call<void>(0x024F22DC,check.get());
    f64 height=actor->current.pos.y;
    if(gabi::call<s32>(0x024F15A4,&actor->current.pos,check.get(),100.0f)) height=read<f32>(gabi::ea(check.get())+0x48);
    write<u32>(gabi::ea(check.get())+0x20,0x10031C50);write<u32>(gabi::ea(check.get())+0x24,0x10031C70);write<u32>(gabi::ea(check.get())+0x30,0x10031C40);
    gabi::call<void>(0x02008B4C,gabi::at<void>(gabi::ea(check.get())+0x10),0);
    return height;
}
VERIFY(0x023AE544,try_waterHeight);
s32 try_draw(Actor* actor) {
    WWHD_FUNC(0x023AD980,s32,actor);
    u32 environment=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x025626A4,gabi::at<void>(environment),0,&actor->current.pos,&actor->tevStr);
    environment=gabi::call<u32>(0x02555D0C);u32 model=actor->model;
    gabi::call<void>(0x02562F5C,gabi::at<void>(environment),gabi::at<void>(model),&actor->tevStr);
    u32 type=checkedType(actor,0x10032050,0x10032060);
    if(read<s16>(0x10032244+type*0x78)>=0) {
        model=actor->model;f32 frame=read<f32>(gabi::ea(actor)+0x3BC);u32 modelData=read<u32>(model+0xAC);
        gabi::call<void>(0x025E83FC,member(actor,0x3B8),gabi::at<void>(modelData),frame);
    }
    model=actor->model;gabi::call<void>(0x025E2DE0,gabi::at<void>(model),0);return 1;
}
VERIFY(0x023AD980,try_draw);
s32 try_execute(Actor* actor) {
    WWHD_FUNC(0x023AD758,s32,actor);
    gabi::call<void>(0x023AC050,actor);
    bool skip=false;
    if(!read<u8>(gabi::ea(actor)+0x7B1) && actor->mode==1) {
        u32 flags=read<u32>(gabi::ea(actor)+0x458);
        if((flags&0x20) && !(flags&0x80)) {
            u32 type=checkedType(actor,0x1003201C,0x1003202C);
            if(!read<u8>(0x100321FC+type*0x78+0x75) && (actor->actor_condition&4)) skip=gabi::call<s32>(0x025D6CE8,actor)!=0;
        }
    }
    if(!skip) {
        write<u8>(gabi::ea(actor)+0x7B1,0);
        bool damaged=gabi::call<s32>(0x023AC0B4,actor)!=0;
        if(!damaged) damaged=gabi::call<s32>(0x023AC7EC,actor)!=0;
        if(!damaged) {
            u8 delay=read<u8>(gabi::ea(actor)+0x7B2);if(delay) write<u8>(gabi::ea(actor)+0x7B2,u8(delay-1));
            write<u8>(gabi::ea(actor)+0x7CA,0);
            if(!gabi::call<s32>(0x023AD358,actor)) damaged=true;
            else {
                gabi::call<void>(0x023ABA4C,actor);
                write<u8>(gabi::ea(actor)+0x656,read<u8>(gabi::ea(actor)+0x326));
                gabi::call<void>(0x025165A4,member(actor,0x670),&actor->current.pos);
                u32 play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x0200E240,gabi::at<void>(play+0x26A4),member(actor,0x670));
                u32 mode=actor->mode;
                if(mode==3 || mode==4 || read<u8>(gabi::ea(actor)+0x7B2)) {play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x02516C14,gabi::at<void>(play+0x4EF8),member(actor,0x670),3);}
                u32 type=actor->type;write<u32>(gabi::ea(actor)+0x390,read<u32>(gabi::ea(actor)+0x314));
                if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x1003201C),0x3CE,STR(0x1003202C));type=actor->type;}
                f32 y=actor->current.pos.y;mode=actor->mode;u32 finalType=actor->type,z=read<u32>(gabi::ea(actor)+0x31C);f32 offset=read<f32>(0x100321FC+type*0x78+8);
                write<u32>(gabi::ea(actor)+0x7A8,mode);y+=offset;u32 x=read<u32>(gabi::ea(actor)+0x390);
                write<u32>(gabi::ea(actor)+0x398,z);write<u32>(gabi::ea(actor)+0x37C,x);write<f32>(gabi::ea(actor)+0x380,y);write<u32>(gabi::ea(actor)+0x384,z);write<f32>(gabi::ea(actor)+0x394,y);
                type=finalType;
            }
        }
        if(damaged) gabi::call<void>(0x025D57E0,actor);
    }
    u32 type=checkedType(actor,0x1003201C,0x1003202C);
    if(!read<u8>(0x100321FC+type*0x78+0x74)) actor->correctEnabled=0;
    actor->correctImmediate=0;gabi::call<void>(0x023ABA2C,actor);return 1;
}
VERIFY(0x023AD758,try_execute);
s32 try_damageCollision(Actor* actor) {
    WWHD_FUNC(0x023AC0B4,s32,actor);
    if(gabi::call<s32>(0x025160DC,member(actor,0x670))) {
        gabi::call<void>(0x02516094,member(actor,0x670));
        f32 forward=read<f32>(gabi::ea(actor)+0x370);write<f32>(gabi::ea(actor)+0x370,forward*0.3f);return 0;
    }
    if(gabi::call<s32>(0x025162A4,member(actor,0x670))) {
        gabi::call<u32>(0x02516300,member(actor,0x670));
        u32 type=checkedType(actor,0x10031E08,0x10031E18);s8 room=actor->current.roomNo;
        u32 sound=read<u32>(0x100321FC+type*0x78+0x5C);
        gabi::call<void>(0x023129C4,member(actor,0x37C),room,member(actor,0x670),sound);
        type=checkedType(actor,0x10031E08,0x10031E18);u32 first=0x100321FC+type*0x78;
        type=checkedType(actor,0x10031E08,0x10031E18);u32 second=0x100321FC+type*0x78;
        u8 context=read<u8>(second+0x43),soundEnvironment=read<u8>(first+0x42);
        gabi::call<void>(0x023AC070,actor,soundEnvironment,context);
        gabi::call<void>(0x02312E54,actor,member(actor,0x670));gabi::call<void>(0x0251621C,member(actor,0x670));
    }
    return 0;
}
VERIFY(0x023AC0B4,try_damageCollision);
void try_waterSound(Actor* actor) {
    WWHD_FUNC(0x023AC280,void,actor);
    u32 material=19;
    for(unsigned i=0;i<2;++i) {
        u32 polygon=gabi::ea(actor)+(i==0?0x5A4:0x518);
        if(read<u16>(polygon+2)<256) {u32 play=gabi::call<u32>(0x025200D4);material=gabi::call<u32>(0x024EECAC,gabi::at<void>(play+0x12A0),gabi::at<void>(polygon));break;}
    }
    u32 type=checkedType(actor,0x10031E70,0x10031E80);s8 room=actor->current.roomNo;
    u32 sound=read<u32>(0x100321FC+type*0x78+0x58);s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,sound,member(actor,0x37C),material,reverb);
    type=checkedType(actor,0x10031E70,0x10031E80);u32 first=0x100321FC+type*0x78;
    type=checkedType(actor,0x10031E70,0x10031E80);u32 second=0x100321FC+type*0x78;
    u8 context=read<u8>(second+0x45),environment=read<u8>(first+0x44);
    gabi::call<void>(0x023AC070,actor,environment,context);
}
VERIFY(0x023AC280,try_waterSound);
s32 try_create(Actor* actor) {
    WWHD_FUNC(0x023ABB7C,s32,actor);
    u32 condition=actor->actor_condition;
    if(!(condition&8)) {
        if(actor) {
            gabi::call<void>(0x025D4ED0,actor);write<u32>(gabi::ea(actor)+0xB4,0x10031CE0);
            gabi::call<void>(0x025E80D0,member(actor,0x3B8));gabi::call<void>(0x024F0474,member(actor,0x430));
            write<u8>(gabi::ea(actor)+0x448,1);write<u32>(gabi::ea(actor)+0x440,0x10031CB0);write<u32>(gabi::ea(actor)+0x450,0x10031CC0);write<u32>(gabi::ea(actor)+0x444,0x10031CD0);
            gabi::call<void>(0x024EFE94,member(actor,0x5F4));gabi::call<void>(0x0200BD2C,member(actor,0x634));gabi::call<void>(0x02515DA0,member(actor,0x650));
            write<u32>(gabi::ea(actor)+0x64C,0x1004AE88);write<u32>(gabi::ea(actor)+0x650,0x1004AEC0);
            gabi::call<void>(0x02515FB8,member(actor,0x670));write<u32>(gabi::ea(actor)+0x784,0x100015A8);write<u32>(gabi::ea(actor)+0x780,0x10031C30);
            gabi::call<void>(0x02018590,member(actor,0x788));write<u32>(gabi::ea(actor)+0x6AC,0x1004B108);write<u32>(gabi::ea(actor)+0x79C,0x1004B150);write<u32>(gabi::ea(actor)+0x784,0x1004B160);
            gabi::call<void>(0x025A5894,member(actor,0x7D0),0,0);actor->emitter=0;condition=actor->actor_condition;
        }
        actor->actor_condition=condition|8;
    }
    actor->type=gabi::call<u32>(0x02333CBC,actor,4,0);
    s32 appears=gabi::call<s32>(0x023AB584,actor);actor->appears=u8(appears);if(!appears) return 5;
    s32 phase=gabi::call<s32>(0x02520460,&actor->phase,STR(0x100321B0));if(phase!=4) return phase;
    u32 type=checkedType(actor,0x10031DCC,0x10031DDC),heapSize=read<u32>(0x100321FC+type*0x78+0x60);
    if(!gabi::call<s32>(0x025D63E8,actor,gabi::at<void>(0x023AB89C),heapSize)) return 5;
    type=checkedType(actor,0x10031DCC,0x10031DDC);f32 radius=f32(read<u8>(0x100321FC+type*0x78+0x4B));
    gabi::call<void>(0x024EFF44,member(actor,0x5F4),30.0f,radius);
    gabi::call<void>(0x024F06B4,member(actor,0x430),&actor->current.pos,member(actor,0x300),actor,1,member(actor,0x5F4),member(actor,0x33C),member(actor,0x320),member(actor,0x328));
    u32 flags=read<u32>(gabi::ea(actor)+0x458);type=actor->type;write<u32>(gabi::ea(actor)+0x458,flags&~0x408u);
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031DCC),0x3CE,STR(0x10031DDC));type=actor->type;}
    write<f32>(gabi::ea(actor)+0x4F0,f32(read<u8>(0x100321FC+type*0x78+0x4D)));
    gabi::call<void>(0x023AB8A0,actor);u32 model=actor->model;actor->cullMtx=model?model+0xC8:0;
    gabi::call<void>(0x023ABA2C,actor);type=checkedType(actor,0x10031DCC,0x10031DDC);
    u32 actualType=actor->type,gravity=read<u32>(0x100321FC+type*0x78);write<u32>(gabi::ea(actor)+0x374,gravity);
    if(actualType==5 || actualType==6) actor->actor_status=u32(actor->actor_status)|0x02000000u;
    gabi::call<void>(0x025D6870,actor,0);u32 play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x024F08A8,member(actor,0x430),gabi::at<void>(play+0x12A0));
    u32 x=read<u32>(gabi::ea(actor)+0x314),acchFlags=read<u32>(gabi::ea(actor)+0x458);type=actor->type;write<u32>(gabi::ea(actor)+0x390,x);u32 status=read<u32>(gabi::ea(actor)+0x39C);
    write<s16>(gabi::ea(actor)+0x7AE,20);write<u8>(gabi::ea(actor)+0x38C,23);write<u8>(gabi::ea(actor)+0x7B0,1);write<u32>(gabi::ea(actor)+0x458,acchFlags&~0x80u);write<u32>(gabi::ea(actor)+0x39C,status|0x10);
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031DCC),0x3CE,STR(0x10031DDC));type=actor->type;}
    f32 y=actor->current.pos.y;u32 z=read<u32>(gabi::ea(actor)+0x31C);f32 offset=read<f32>(0x100321FC+type*0x78+8);
    write<u8>(gabi::ea(actor)+0x7B1,1);write<s16>(gabi::ea(actor)+0x7AC,0);write<u8>(gabi::ea(actor)+0x7B2,0);write<f32>(gabi::ea(actor)+0x394,y+offset);write<u32>(gabi::ea(actor)+0x398,z);write<u32>(gabi::ea(actor)+0x7B4,0);actor->restartDelay=0;
    gabi::call<void>(0x023ABB10,actor);actor->bingoActive=0;actor->correctEnabled=0;actor->correctImmediate=0;write<u8>(gabi::ea(actor)+0x7CA,1);write<u8>(gabi::ea(actor)+0x7CB,0);write<u8>(gabi::ea(actor)+0x7CE,0);
    gabi::call<void>(0x023ABB30,actor);write<u32>(gabi::ea(actor)+0x368,u32(actor->model));return phase;
}
VERIFY(0x023ABB7C,try_create);

void* try_searchSameType(Actor* candidate,Actor* actor) {
    WWHD_FUNC(0x023AB4BC,void*,candidate,actor);
    if(!candidate || !gabi::call<s32>(0x025D4604,candidate)) return nullptr;
    if(!candidate || read<s16>(gabi::ea(candidate)+8)!=0x1CA || candidate==actor) return nullptr;
    u32 otherType=gabi::call<u32>(0x02333CBC,candidate,4,0);
    u32 type=gabi::call<u32>(0x02333CBC,actor,4,0);
    if(otherType!=type || !candidate->appears) return nullptr;
    return candidate;
}
VERIFY(0x023AB4BC,try_searchSameType);
s32 try_appears(Actor* actor) {
    WWHD_FUNC(0x023AB584,s32,actor);
    u32 type=actor->type; s32 result=1;
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031CF4),0x3CE,STR(0x10031D04));type=actor->type;}
    if(read<u8>(0x100321FC+type*0x78+0x70)) {
        bool inverse=gabi::call<u32>(0x02333CBC,actor,1,31)!=0;
        u32 bit=gabi::call<u32>(0x02333CBC,actor,8,8);
        u32 save=read<u32>(0x101F84DC);s8 room=actor->home.roomNo;
        bool on=gabi::call<s32>(0x025BA0C0,gabi::at<void>(save+0x20),bit,room)!=0;
        type=actor->type;if(on!=inverse) result=0;
    }
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031CF4),0x3CE,STR(0x10031D04));type=actor->type;}
    if(read<u8>(0x100321FC+type*0x78+0x71) && gabi::call<u32>(0x025D5218,gabi::at<void>(0x023AB4BC),actor)) result=0;
    return result;
}
VERIFY(0x023AB584,try_appears);
u8 try_inWater(Actor* actor) {
    WWHD_FUNC(0x023AC1EC,u8,actor);
    if(!(read<u32>(gabi::ea(actor)+0x458)&0x800)) return 0;
    u32 type=actor->type;
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031E3C),0x3CE,STR(0x10031E4C));type=actor->type;}
    f32 height=actor->current.pos.y;f32 offset=read<f32>(0x10032200+type*0x78);
    f32 top=height+offset,water=read<f32>(gabi::ea(actor)+0x5EC);
    return water>top;
}
VERIFY(0x023AC1EC,try_inWater);
u8 try_fullySubmerged(Actor* actor) {
    WWHD_FUNC(0x023AC5B8,u8,actor);
    if(!(read<u32>(gabi::ea(actor)+0x458)&0x800)) return 0;
    u32 type=actor->type;
    if(type>=13) {gabi::call<void>(0x0273AA24,STR(0x10031EE0),0x3CE,STR(0x10031EF0));type=actor->type;}
    f32 offset=f32(read<u8>(0x100321FC+type*0x78+0x4D)),height=actor->current.pos.y;
    f32 top=(height+offset)+50.0f,water=read<f32>(gabi::ea(actor)+0x5EC);
    return water>top;
}
VERIFY(0x023AC5B8,try_fullySubmerged);
void try_carryInit(Actor* actor) {
    WWHD_FUNC(0x023AC9D0,void,actor);
    u32 hitFlags=read<u32>(gabi::ea(actor)+0x688),acchFlags=read<u32>(gabi::ea(actor)+0x458),contactFlags=read<u32>(gabi::ea(actor)+0x69C);
    write<s16>(gabi::ea(actor)+0x7AC,8);
    u32 status=read<u32>(gabi::ea(actor)+0x39C),colliderFlags=read<u32>(gabi::ea(actor)+0x670);
    write<u32>(gabi::ea(actor)+0x458,acchFlags&0xFFFFDBF1u);write<u32>(gabi::ea(actor)+0x688,hitFlags|1);
    write<u32>(gabi::ea(actor)+0x670,colliderFlags&~1u);actor->mode=2;
    write<u32>(gabi::ea(actor)+0x39C,status&~0x10u);write<u32>(gabi::ea(actor)+0x69C,contactFlags&~1u);
}
VERIFY(0x023AC9D0,try_carryInit);
f64 try_gamma(u32 intensity) {
    WWHD_FUNC(0x023ACA24,f64,intensity);
    f32 fraction=f32(intensity)/255.0f;
    f64 value=gabi::call<f64>(0x028F4560,f64(fraction),f64(2.2f));
    if(value<0.0) value=0.0; else if(value>1.0) value=1.0;
    return value;
}
VERIFY(0x023ACA24,try_gamma);

void try_cullDraw(Actor* actor) {
    WWHD_FUNC(0x023ABA2C,void,actor);
    gabi::call<void>(0x025D6768,actor,0.0f,65.0f,0.0f,100.0f);
}
VERIFY(0x023ABA2C,try_cullDraw);
void try_cullMove(Actor* actor) {
    WWHD_FUNC(0x023AC050,void,actor);
    gabi::call<void>(0x025D6768,actor,0.0f,65.0f,0.0f,300.0f);
}
VERIFY(0x023AC050,try_cullMove);
void try_initMatrix(Actor* actor) {
    WWHD_FUNC(0x023ABB10,void,actor);
    u32 model=actor->model,y=read<u32>(gabi::ea(actor)+0x334),x=read<u32>(gabi::ea(actor)+0x330),z=read<u32>(gabi::ea(actor)+0x338);
    write<u32>(model+0xBC,x);write<u32>(model+0xC0,y);write<u32>(model+0xC4,z);
    gabi::call<void>(0x023ABA4C,actor);
}
VERIFY(0x023ABB10,try_initMatrix);
void try_waitInit(Actor* actor) {
    WWHD_FUNC(0x023ABB30,void,actor);
    u32 hitFlags=read<u32>(gabi::ea(actor)+0x688),acchFlags=read<u32>(gabi::ea(actor)+0x458);
    u32 colliderFlags=read<u32>(gabi::ea(actor)+0x670);
    write<u32>(gabi::ea(actor)+0x458,acchFlags&0xFFFFDBF1u);
    u32 contactFlags=read<u32>(gabi::ea(actor)+0x69C);
    actor->mode=1;write<u32>(gabi::ea(actor)+0x670,colliderFlags&~1u);
    write<u32>(gabi::ea(actor)+0x370,0);write<u32>(gabi::ea(actor)+0x688,hitFlags|1);write<u32>(gabi::ea(actor)+0x69C,contactFlags|1);
}
VERIFY(0x023ABB30,try_waitInit);
void try_soundEnvironment(Actor* actor,s32 sound,s32 context) {
    WWHD_FUNC(0x023AC070,void,actor,sound,context);
    gabi::Local<cXyz> position;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    u32 x=read<u32>(gabi::ea(actor)+0x314),y=read<u32>(gabi::ea(actor)+0x318);
    write<u32>(gabi::ea(position.get()),x);u32 z=read<u32>(gabi::ea(actor)+0x31C),id=read<u32>(gabi::ea(actor)+4);
    write<u32>(gabi::ea(position.get())+4,y);write<u32>(gabi::ea(position.get())+8,z);
    gabi::call<void>(0x0255F458,position.get(),sound,id,context);
}
VERIFY(0x023AC070,try_soundEnvironment);
void try_clearBingo(Actor* actor) {
    WWHD_FUNC(0x023AD02C,void,actor);
    if(actor->bingoActive) {
        gabi::call<void>(0x025A5AC8,member(actor,0x7D0));
        u32 emitter=actor->emitter;
        if(emitter) {u32 flags=read<u32>(emitter+0x254);write<u32>(emitter+0x5C,0xFFFFFFFFu);write<u32>(emitter+0x254,flags|1);actor->emitter=0;}
        actor->bingoActive=0;
    }
}
VERIFY(0x023AD02C,try_clearBingo);
void try_cameraUnlock(Actor* actor) {
    WWHD_FUNC(0x023AD094,void,actor);
    u32 play=gabi::call<u32>(0x025200D4),camera=read<u32>(play+0x5AF8),id=read<u32>(gabi::ea(actor)+4);
    gabi::call<void>(0x025052BC,gabi::at<void>(camera+0x248),id);
}
VERIFY(0x023AD094,try_cameraUnlock);
void try_landSmoke(Actor* actor) {
    WWHD_FUNC(0x023AD0D0,void,actor);
    gabi::call<void>(0x02311CD8,actor,member(actor,0x504),1.0f);
}
VERIFY(0x023AD0D0,try_landSmoke);
void try_saveSwitchParameter(Actor* actor,s32 bit) {
    WWHD_FUNC(0x023AD348,void,actor,bit);
    u32 parameters=actor->mParameters;
    actor->mParameters=(parameters&~0xFF00u)|((u32(bit)<<8)&0xFF00u);
}
VERIFY(0x023AD348,try_saveSwitchParameter);
s32 try_heapCallback(Actor* actor) {
    WWHD_FUNC(0x023AB89C,s32,actor);return gabi::call<s32>(0x023AB6B0,actor);
}
VERIFY(0x023AB89C,try_heapCallback);
s32 try_damageBackground(Actor* actor) {
    WWHD_FUNC(0x023AC7EC,s32,actor);
    s32 stay=s8(read<u8>(0x1047E6C8));
    bool grounded=(read<u32>(gabi::ea(actor)+0x458)&0x20)!=0;
    s32 room=s8(actor->home.roomNo);
    s32 inWater=gabi::call<s32>(0x023AC1EC,actor);
    u32 mode=actor->mode;
    bool submerged=false;
    if(mode==1) {
        if(inWater) {
            gabi::call<void>(0x023AC280,actor);
            if(read<f32>(gabi::ea(actor)+0x340)!=0.0f) {
                gabi::call<void>(0x023AC3D4,actor);
                write<u8>(gabi::ea(actor)+0x7CF,1);
            } else write<u8>(gabi::ea(actor)+0x7CF,0);
            gabi::call<void>(0x023AC424,actor);
        }
    } else if(mode==3) {
        if(!grounded) {
            if(inWater) {
                gabi::call<void>(0x023AC280,actor);
                gabi::call<void>(0x023AC3D4,actor);
                write<u8>(gabi::ea(actor)+0x7CF,1);
                gabi::call<void>(0x023AC424,actor);
            }
            return 0;
        }
        gabi::call<void>(0x023ABB30,actor);
    } else if(mode==4) {
        if(gabi::call<s32>(0x023AC5B8,actor)) {
            submerged=true;
            if(!read<u8>(gabi::ea(actor)+0x7CF)) gabi::call<void>(0x023AC3D4,actor);
        } else if(!inWater) gabi::call<void>(0x023ABB30,actor);
    }
    bool dangerousGround=false;
    if(grounded) {
        u32 play=gabi::call<u32>(0x025200D4);
        dangerousGround=gabi::call<s32>(0x024EF0BC,gabi::at<void>(play+0x12A0),member(actor,0x518))==4;
    }
    if(!dangerousGround && !submerged) return 0;
    if(room==stay) {
        u32 type=checkedType(actor,0x10031F4C,0x10031F5C);
        if(read<u8>(0x100321FC+type*0x78+0x71) || !read<u8>(gabi::ea(actor)+0x7CB)) {
            gabi::call<void>(0x023AC678,actor);
            return 0;
        }
    }
    gabi::call<void>(0x023AC7E8,actor);
    return 1;
}
VERIFY(0x023AC7EC,try_damageBackground);

s32 try_bound(Actor* actor) {
    WWHD_FUNC(0x023ADBFC,s32,actor);
    u32 flags=read<u32>(gabi::ea(actor)+0x458);
    s32 grounded=(flags>>5)&1;
    if(flags&0x10) {
        s32 normalYaw=read<s16>(gabi::ea(actor)+0x630);
        f32 forward=read<f32>(gabi::ea(actor)+0x370);
        s32 yaw=read<s16>(gabi::ea(actor)+0x322);
        flags=read<u32>(gabi::ea(actor)+0x458);
        write<f32>(gabi::ea(actor)+0x370,forward*0.3f);
        write<u16>(gabi::ea(actor)+0x322,u16(normalYaw*2-(yaw+0x8000)));
    }
    if(flags&0x80) {
        f32 bounce=std::fabs(read<f32>(gabi::ea(actor)+0x7B4)*-0.6f);
        if(bounce>15.0f) {
            write<f32>(gabi::ea(actor)+0x340,15.0f);
            return 0;
        }
    } else if(flags&0x20) {
        gabi::call<void>(0x0200ECD4,member(actor,0x370),0.0f,0.5f,5.0f,1.0f);
    }
    return grounded;
}
VERIFY(0x023ADBFC,try_bound);

void try_restart(Actor* actor) {
    WWHD_FUNC(0x023ADA30,void,actor);
    s16 timer=s16(u16(read<s16>(gabi::ea(actor)+0x7AC))-1);
    write<s16>(gabi::ea(actor)+0x7AC,timer);
    if(timer<80) write<u8>(0x101CD761,1);
    write<s16>(gabi::ea(actor)+0x7AE,20);
    timer=read<s16>(gabi::ea(actor)+0x7AC);
    write<u8>(gabi::ea(actor)+0x7B0,1);
    if(timer==0) {
        s16 yaw=read<s16>(gabi::ea(actor)+0x2FA);
        u32 y=read<u32>(gabi::ea(actor)+0x2F0);
        write<s16>(gabi::ea(actor)+0x32A,yaw);
        write<s16>(gabi::ea(actor)+0x322,yaw);
        write<u32>(gabi::ea(actor)+0x318,y);
        gabi::call<void>(0x023ABB30,actor);
    } else if(timer<50) {
        u32 angle=u16(gabi::ftoi(f32(timer)*655.36f));
        f32 factor=(1.0f-read<f32>(0x104A44FC+((angle>>3)<<3)))*0.5f;
        u32 type=checkedType(actor,0x10032090,0x100320A0);
        f32 height=f32(read<u8>(0x100321FC+type*0x78+0x4D));
        s32 rotation=gabi::ftoi(factor*-32768.0f);
        s32 homeYaw=read<s16>(gabi::ea(actor)+0x2FA);
        f32 homeY=read<f32>(gabi::ea(actor)+0x2F0);
        u16 yaw=u16(u32(rotation)+u32(homeYaw));
        f32 y=gabi::fmadds(-10.0f-height,factor,homeY);
        write<u16>(gabi::ea(actor)+0x32A,yaw);
        write<f32>(gabi::ea(actor)+0x318,y);
        write<u16>(gabi::ea(actor)+0x322,yaw);
    }
}
VERIFY(0x023ADA30,try_restart);

void try_damageBackgroundDirect(Actor* actor) {
    WWHD_FUNC(0x023AD1C0,void,actor);
    u32 mode=actor->mode,flags=read<u32>(gabi::ea(actor)+0x458);
    bool grounded=(flags&0x20)!=0;
    if(mode==3) {
        bool unlock=(flags&0x200)!=0;
        if(!unlock) unlock=gabi::call<s32>(0x023AC1EC,actor)!=0 || grounded;
        if(unlock) gabi::call<void>(0x023AD094,actor);
    }
    s16 timer=read<s16>(gabi::ea(actor)+0x7AE);
    if(timer>0) {write<s16>(gabi::ea(actor)+0x7AE,timer-1);return;}
    if(!grounded) {write<u8>(gabi::ea(actor)+0x7B0,0);return;}
    if(read<u8>(gabi::ea(actor)+0x7B0)) return;
    mode=actor->mode;
    if(mode!=1 && mode!=3) return;
    u32 type=checkedType(actor,0x10031F8C,0x10031F9C);
    u32 sound=read<u32>(0x100321FC+type*0x78+0x54);
    u32 play=gabi::call<u32>(0x025200D4);
    s32 material=gabi::call<s32>(0x024EECAC,gabi::at<void>(play+0x12A0),member(actor,0x518));
    s32 reverb=gabi::call<s32>(0x02520540,s32(s8(actor->current.roomNo)));
    gabi::call<void>(0x025E1A40,sound,member(actor,0x37C),material,reverb);
    if(read<u32>(gabi::ea(actor)+0x7A8)!=4) gabi::call<void>(0x023AD0D0,actor);
    write<u8>(gabi::ea(actor)+0x7B0,1);
    write<s16>(gabi::ea(actor)+0x7AE,20);
    gabi::call<void>(0x023AD160,actor);
}
VERIFY(0x023AD1C0,try_damageBackgroundDirect);

void try_sink(Actor* actor) {
    WWHD_FUNC(0x023AE5E4,void,actor);
    f64 water=gabi::call<f64>(0x023AE544,actor);
    f32 ground=read<f32>(gabi::ea(actor)+0x4C4);
    gabi::call<void>(0x02312968,actor,member(actor,0x504));
    u32 type;
    if(read<f32>(gabi::ea(actor)+0x340)!=0.0f) {
        type=checkedType(actor,0x10032174,0x10032184);
        f32 y=read<f32>(gabi::ea(actor)+0x318),offset=read<f32>(0x100321FC+type*0x78+4);
        if(f64(y+offset)>water) {
            if(type>=13) type=checkedType(actor,0x10032174,0x10032184);
            offset=read<f32>(0x100321FC+type*0x78+4);
            f32 surface=f32(water-f64(offset));
            if(surface>ground) {
                if(type>=13) type=checkedType(actor,0x10032174,0x10032184);
                offset=read<f32>(0x100321FC+type*0x78+4);
                f32 newY=f32(water-f64(offset));
                type=actor->type;
                write<f32>(gabi::ea(actor)+0x318,newY);
            }
        }
    } else type=actor->type;
    if(type>=13) type=checkedType(actor,0x10032174,0x10032184);
    u32 firstRow=0x100321FC+type*0x78;
    if(type>=13) type=checkedType(actor,0x10032174,0x10032184);
    f32 drag=read<f32>(firstRow+0x28),stream=read<f32>(0x100321FC+type*0x78+0x2C);
    gabi::call<void>(0x023123C0,actor,member(actor,0x634),gabi::at<void>(0x101FFBA8),drag,stream);
}
VERIFY(0x023AE5E4,try_sink);

void try_dropInit(Actor* actor) {
    WWHD_FUNC(0x023AE2D4,void,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 type=actor->type;
    u32 player=read<u32>(play+0x5B2C);
    if(type>=13) type=checkedType(actor,0x10032140,0x10032150);
    u32 firstRow=0x100321FC+type*0x78;
    if(type>=13) type=checkedType(actor,0x10032140,0x10032150);
    u32 secondRow=0x100321FC+type*0x78;
    u32 flags=read<u32>(gabi::ea(actor)+0x458),cylinder=read<u32>(gabi::ea(actor)+0x670);
    f32 playerSpeed=read<f32>(player+0x370);
    type=actor->type;
    u32 target=read<u32>(gabi::ea(actor)+0x69C);
    f32 multiplier=read<f32>(secondRow+0x18),constant=read<f32>(firstRow+0x14);
    u32 status=read<u32>(gabi::ea(actor)+0x39C);
    write<u32>(gabi::ea(actor)+0x69C,target|1);
    u32 collision=read<u32>(gabi::ea(actor)+0x688);
    f32 forward=gabi::fmadds(playerSpeed,multiplier,constant);
    write<u32>(gabi::ea(actor)+0x458,(flags&~0x40Eu)|0x2000);
    write<u32>(gabi::ea(actor)+0x39C,status&~0x10u);
    write<u32>(gabi::ea(actor)+0x688,collision|1);
    write<u32>(gabi::ea(actor)+0x670,cylinder|1);
    if(type>=13) type=checkedType(actor,0x10032140,0x10032150);
    u32 vertical=read<u32>(0x100321FC+type*0x78+0x10);
    type=actor->type;
    write<f32>(gabi::ea(actor)+0x370,forward);
    write<u32>(gabi::ea(actor)+0x340,vertical);
    if(type>=13) type=checkedType(actor,0x10032140,0x10032150);
    u32 gravity=read<u32>(0x100321FC+type*0x78);
    actor->mode=3;
    write<u32>(gabi::ea(actor)+0x374,gravity);
}
VERIFY(0x023AE2D4,try_dropInit);

void try_calcDrop(Actor* actor,f32* gravity,f32* drag,f32* stream) {
    WWHD_FUNC(0x023ADCD8,void,actor,gravity,drag,stream);
    u32 flags=read<u32>(gabi::ea(actor)+0x458),type=actor->type;
    if(flags&0x800) {
        f32 depth=read<f32>(gabi::ea(actor)+0x318)-read<f32>(gabi::ea(actor)+0x5EC);
        f32 submerged=0.0f;
        if(depth<0.0f) {
            if(type>=13) type=checkedType(actor,0x100320D0,0x100320E0);
            f32 height=f32(read<u8>(0x100321FC+type*0x78+0x4D));
            submerged=0.5f;
            if(depth>-height) {
                if(type>=13) type=checkedType(actor,0x100320D0,0x100320E0);
                height=f32(read<u8>(0x100321FC+type*0x78+0x4D));
                submerged=-(depth*(0.5f/height));
            }
        }
        f32 above=1.0f-submerged;
        if(type>=13) type=checkedType(actor,0x100320D0,0x100320E0);
        u32 firstRow=0x100321FC+type*0x78;
        if(type>=13) type=checkedType(actor,0x100320D0,0x100320E0);
        f32 air=read<f32>(0x100321FC+type*0x78+0x1C),water=read<f32>(firstRow+0x28);
        write<f32>(gabi::ea(drag),gabi::fmadds(submerged,water,above*air));
        type=checkedType(actor,0x100320D0,0x100320E0);
        firstRow=0x100321FC+type*0x78;
        if(type>=13) type=checkedType(actor,0x100320D0,0x100320E0);
        air=read<f32>(0x100321FC+type*0x78+0x20);water=read<f32>(firstRow+0x2C);
        write<f32>(gabi::ea(stream),gabi::fmadds(submerged,water,above*air));
        type=checkedType(actor,0x100320D0,0x100320E0);
        firstRow=0x100321FC+type*0x78;
        if(type>=13) type=checkedType(actor,0x100320D0,0x100320E0);
        f32 buoyancy=read<f32>(firstRow+0x24),base=read<f32>(0x100321FC+type*0x78);
        write<f32>(gabi::ea(gravity),gabi::fmadds(submerged,buoyancy,base));
    } else {
        if(type>=13) type=checkedType(actor,0x100320D0,0x100320E0);
        write<u32>(gabi::ea(drag),read<u32>(0x100321FC+type*0x78+0x1C));
        type=checkedType(actor,0x100320D0,0x100320E0);
        write<u32>(gabi::ea(stream),read<u32>(0x100321FC+type*0x78+0x20));
        type=checkedType(actor,0x100320D0,0x100320E0);
        write<u32>(gabi::ea(gravity),read<u32>(0x100321FC+type*0x78));
    }
}
VERIFY(0x023ADCD8,try_calcDrop);

void try_wait(Actor* actor) {
    WWHD_FUNC(0x023ADFE4,void,actor);
    u32 type=checkedType(actor,0x1003210C,0x1003211C);
    bool corrected=read<u8>(0x100321FC+type*0x78+0x74) && actor->correctEnabled;
    if(type>=13) type=checkedType(actor,0x1003210C,0x1003211C);
    bool chase=read<u8>(0x100321FC+type*0x78+0x73) && actor->correctImmediate;
    if(chase) {
        u32 y=read<u32>(gabi::ea(actor)+0x318);
        gabi::call<void>(0x0200F62C,member(actor,0x314),member(actor,0x7B8),10.0f);
        gabi::Local<cXyz> target,current;
        gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
        write<u32>(gabi::ea(target.get()),read<u32>(gabi::ea(actor)+0x7B8));
        write<u32>(gabi::ea(target.get())+4,0);
        write<u32>(gabi::ea(current.get()),read<u32>(gabi::ea(actor)+0x314));
        write<u32>(gabi::ea(actor)+0x318,y);
        write<u32>(gabi::ea(current.get())+4,0);
        write<u32>(gabi::ea(current.get())+8,read<u32>(gabi::ea(actor)+0x31C));
        write<u32>(gabi::ea(target.get())+8,read<u32>(gabi::ea(actor)+0x7C0));
        f64 distance=gabi::call<f64>(0x028E8DE8,target.get(),current.get());
        if(distance<25.0) {
            u32 x=read<u32>(gabi::ea(actor)+0x7B8),z=read<u32>(gabi::ea(actor)+0x7C0);
            write<u32>(gabi::ea(actor)+0x314,x);write<u32>(gabi::ea(actor)+0x31C,z);
            distance=gabi::call<f64>(0x028E8DE8,member(actor,0x7B8),member(actor,0x314));
            if(distance<1.0) write<u8>(0x101CD760,1);
        }
        s32 yaw=read<s16>(gabi::ea(actor)+0x32A),targetYaw=read<s16>(gabi::ea(actor)+0x7C4);
        s16 goal=s16(u16((u32(yaw-targetYaw+0x2000)&0xC000)+u32(targetYaw)));
        write<s16>(gabi::ea(actor)+0x7C6,goal);
        gabi::call<void>(0x0200F378,member(actor,0x32A),goal,3,0x1800,0x800);
    } else if(!corrected) gabi::call<s32>(0x023ADBFC,actor);
    bool grounded=(read<u32>(gabi::ea(actor)+0x458)&0x20)!=0;
    bool useStream=false;
    if(grounded && !corrected) {
        u32 status=read<u32>(gabi::ea(actor)+0x39C);
        u8 immediate=actor->correctImmediate;
        write<u32>(gabi::ea(actor)+0x39C,status|0x10);
        useStream=immediate!=0;
    } else {
        write<u32>(gabi::ea(actor)+0x39C,read<u32>(gabi::ea(actor)+0x39C)&~0x10u);
        if(corrected) write<u8>(gabi::ea(actor)+0x7CA,1);
        if(grounded) useStream=corrected || actor->correctImmediate;
    }
    void* movement=useStream?gabi::at<void>(0x101FFBA8):member(actor,0x634);
    u32 flags=read<u32>(gabi::ea(actor)+0x458);
    if(!(flags&0x20)) {
        gabi::Local<f32> gravity,drag,stream;
        gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
        gabi::call<void>(0x023ADCD8,actor,gravity.get(),drag.get(),stream.get());
        u32 g=read<u32>(gabi::ea(gravity.get()));
        f32 d=read<f32>(gabi::ea(drag.get())),v=read<f32>(gabi::ea(stream.get()));
        write<u32>(gabi::ea(actor)+0x374,g);
        gabi::call<void>(0x023123C0,actor,movement,gabi::at<void>(0x101FFBA8),d,v);
    } else {
        type=checkedType(actor,0x1003210C,0x1003211C);
        write<u32>(gabi::ea(actor)+0x374,read<u32>(0x100321FC+type*0x78));
        gabi::call<void>(0x025D6870,actor,movement);
    }
}
VERIFY(0x023ADFE4,try_wait);

s32 try_modeCall(Actor* actor) {
    WWHD_FUNC(0x023AD358,s32,actor);
    if((u32(actor->actor_status)&0x2000) && u32(actor->mode)!=2) gabi::call<void>(0x023AC9D0,actor);
    write<u8>(0x101CD760,0);write<u8>(0x101CD761,0);
    u32 dispatch=0x10031FC0+u32(actor->mode)*8;
    s32 slot=read<s16>(dispatch+2),adjustment=read<s16>(dispatch);
    u32 self=gabi::ea(actor)+u32(adjustment),target;
    if(slot<0) target=read<u32>(dispatch+4);
    else {
        s32 vtableOffset=read<s16>(dispatch+6);
        u32 vtable=read<u32>(self+u32(vtableOffset));
        target=read<u32>(vtable+u32(slot)*8+4);
    }
    gabi::call<void>(target,gabi::at<void>(self));
    u32 type=checkedType(actor,0x10031FE8,0x10031FF8);
    if(read<s16>(0x100321FC+type*0x78+0x48)>=0) {
        u8 bingo=read<u8>(0x101CD760),restart=read<u8>(0x101CD761);
        if(bingo || restart) gabi::call<void>(0x023ACAA4,actor,bingo,restart);
        else gabi::call<void>(0x023AD02C,actor);
        bingo=read<u8>(0x101CD760);
        bool animate=bingo!=0;
        if(!animate) animate=read<u8>(0x101CD761)!=0 || u32(actor->mode)==2;
        if(animate) {
            s32 room=s8(actor->current.roomNo);
            write<u32>(gabi::ea(actor)+0x3B8,0x3F800000);write<u8>(gabi::ea(actor)+0x3C6,2);
            s32 reverb=gabi::call<s32>(0x02520540,room);
            gabi::call<void>(0x025E1A40,0x61D5,member(actor,0x37C),0,reverb);
        } else write<u8>(gabi::ea(actor)+0x3C6,0);
        type=actor->type;
        if(type==5 || type==6) {
            u32 save=read<u32>(0x101F84DC);bool carrying=u32(actor->mode)==2;
            gabi::call<void>(carrying?0x025B8B68:0x025B8B7C,gabi::at<void>(save+0x1178),type==5?0x108:0x110);
        }
        gabi::call<void>(0x025E742C,member(actor,0x3B8));
    }
    u32 vy=read<u32>(gabi::ea(actor)+0x340),y=read<u32>(gabi::ea(actor)+0x318);
    write<u32>(gabi::ea(actor)+0x7B4,vy);
    u32 vx=read<u32>(gabi::ea(actor)+0x33C),vz=read<u32>(gabi::ea(actor)+0x344),z=read<u32>(gabi::ea(actor)+0x31C),x=read<u32>(gabi::ea(actor)+0x314);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x024F08A8,member(actor,0x430),gabi::at<void>(play+0x12A0));
    play=gabi::call<u32>(0x025200D4);
    s32 moving=gabi::call<s32>(0x024EEABC,gabi::at<void>(play+0x12A0),member(actor,0x518));
    u32 mode=actor->mode;
    if(moving) write<u8>(gabi::ea(actor)+0x7B1,1);
    if(mode==0 || mode==2) {
        write<u32>(gabi::ea(actor)+0x314,x);write<u32>(gabi::ea(actor)+0x340,vy);write<u32>(gabi::ea(actor)+0x344,vz);
        write<u32>(gabi::ea(actor)+0x33C,vx);write<u32>(gabi::ea(actor)+0x31C,z);write<u32>(gabi::ea(actor)+0x318,y);
    }
    gabi::call<void>(0x023AD1C0,actor);
    if(u32(actor->mode)!=2) {
        write<u8>(gabi::ea(actor)+0x1C9,u8(actor->current.roomNo));
        play=gabi::call<u32>(0x025200D4);
        u32 color=gabi::call<u32>(0x024EEEB8,gabi::at<void>(play+0x12A0),member(actor,0x518));
        write<u8>(gabi::ea(actor)+0x1CA,u8(color));
    }
    if(s8(actor->current.roomNo)!=s8(actor->home.roomNo)) {
        type=actor->type;write<u8>(gabi::ea(actor)+0x7CB,1);
        if(type>=13) type=checkedType(actor,0x10031FE8,0x10031FF8);
        if(read<u8>(0x100321FC+type*0x78+0x72)) {
            if(gabi::call<u32>(0x02333CBC,actor,8,8)!=255) gabi::call<void>(0x023AD348,actor,255);
        }
    }
    play=gabi::call<u32>(0x025200D4);
    if(read<u8>(play+0x5292)) write<u8>(gabi::ea(actor)+0x7CA,1);
    return 1;
}
VERIFY(0x023AD358,try_modeCall);

static u8 gammaByte(u32 intensity) {
    f64 corrected=gabi::call<f64>(0x023ACA24,intensity);
    return u8(gabi::ftoi(f32(corrected*255.0)));
}
static void initializeParticleColors(u32 guard,u32 colors,u32 r,u32 g,u32 b,u32 er,u32 eg,u32 eb) {
    if(read<u32>(guard)) return;
    write<u32>(guard,1);
    write<u8>(colors,gammaByte(r));write<u8>(colors+1,gammaByte(g));write<u8>(colors+2,gammaByte(b));
    write<u8>(colors+4,gammaByte(er));write<u8>(colors+5,gammaByte(eg));write<u8>(colors+6,gammaByte(eb));
}
void try_bingo(Actor* actor,s32 sound,s32 atHome) {
    WWHD_FUNC(0x023ACAA4,void,actor,sound,atHome);
    if(actor->bingoActive) {
        s32 reverb=gabi::call<s32>(0x02520540,s32(s8(actor->current.roomNo)));
        gabi::call<void>(0x025E1A40,0x7026,member(actor,0x37C),0,reverb);
        return;
    }
    initializeParticleColors(0x1046C7B0,0x101CD71C,12,24,72,72,12,24);
    initializeParticleColors(0x1046C7B4,0x101CD724,66,73,202,103,62,202);
    initializeParticleColors(0x1046C7B8,0x101CD72C,2,12,56,56,2,12);
    initializeParticleColors(0x1046C7BC,0x101CD734,8,62,27,42,62,98);
    u32 colorOffset=u32(u32(actor->type)!=5)*4;
    u32 primary=0x101CD71C+colorOffset,environment=0x101CD724+colorOffset;
    u32 play=gabi::call<u32>(0x025200D4),particles=read<u32>(play+0x5AB0);
    gabi::call<u32>(0x025A847C,gabi::at<void>(particles),0,0x818E,member(actor,0x314),member(actor,0x328),0,255,member(actor,0x7D0),-1,gabi::at<void>(primary),gabi::at<void>(environment),0);
    gabi::Local<csXyz> rotation;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    s16 yaw=read<s16>(gabi::ea(actor)+0x7C6);
    gabi::call<void>(0x0201A478,rotation.get(),0,yaw,0);
    void* position=atHome?member(actor,0x2EC):member(actor,0x314);
    play=gabi::call<u32>(0x025200D4);particles=read<u32>(play+0x5AB0);
    u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(particles),0,0x818F,position,rotation.get(),0,255,0,-1,gabi::at<void>(0x101CD72C+colorOffset),gabi::at<void>(0x101CD734+colorOffset),0);
    actor->emitter=emitter;
    if(sound) {
        s32 reverb=gabi::call<s32>(0x02520540,s32(s8(actor->current.roomNo)));
        gabi::call<void>(0x025E1A40,0x695F,member(actor,0x37C),0,reverb);
    }
    actor->bingoActive=1;
}
VERIFY(0x023ACAA4,try_bingo);

s32 try_createWrapper(Actor* actor) {WWHD_FUNC(0x023AE788,s32,actor);return gabi::call<s32>(0x023ABB7C,actor);}
VERIFY(0x023AE788,try_createWrapper);
s32 try_deleteWrapper(Actor* actor) {WWHD_FUNC(0x023AE78C,s32,actor);return gabi::call<s32>(0x023ABF90,actor);}
VERIFY(0x023AE78C,try_deleteWrapper);
s32 try_executeWrapper(Actor* actor) {WWHD_FUNC(0x023AE790,s32,actor);return gabi::call<s32>(0x023AD758,actor);}
VERIFY(0x023AE790,try_executeWrapper);
s32 try_drawWrapper(Actor* actor) {WWHD_FUNC(0x023AE794,s32,actor);return gabi::call<s32>(0x023AD980,actor);}
VERIFY(0x023AE794,try_drawWrapper);
void try_staticDestructor(void* object,s32 flags) {
    WWHD_FUNC(0x023AE82C,void,object,flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x023AE82C,try_staticDestructor);
void try_empty(void* object) {WWHD_FUNC(0x023AE840,void,object);}
VERIFY(0x023AE840,try_empty);
s32 try_isDelete(Actor* actor) {WWHD_FUNC(0x023AE8E0,s32,actor);return 1;}
VERIFY(0x023AE8E0,try_isDelete);
