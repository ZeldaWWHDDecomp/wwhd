#include "d/actor/d_a_obj_hcbh.h"
#include "bindings.h"
#include <cmath>
using Actor=daObjHcbh_c;
template<class T> static T read(u32 address) { return gabi::load<T>(address); }
template<class T> static void write(u32 address,T value) { gabi::store<T>(address,value); }
static void* member(Actor* actor,u32 displacement) { return gabi::at<void>(gabi::ea(actor)+displacement); }
static f32 sine(u16 angle) { return read<f32>(0x104A44F8+u32(angle>>3)*8); }
static void copyMatrix(u32 model) {
    f32 values[12];
    for(unsigned i=0;i<12;++i) values[i]=read<f32>(0x1048D0CC+i*4);
    for(unsigned i=0;i<12;++i) write<f32>(model+0xC8+i*4,values[i]);
}
static void transformPillar(Actor* actor,f32 pivot) {
    f32 x=actor->current.pos.x,y=actor->pillarHeight,z=actor->current.pos.z;
    gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
    s16 yaw=actor->breakYaw;
    gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),yaw);
    gabi::call<void>(0x025F24E0,0.0f,pivot,0.0f);
    s16 angle=actor->pillarAngle;
    gabi::call<void>(0x025F1BF4,gabi::at<void>(0x1048D0CC),angle);
    gabi::call<void>(0x025F24E0,0.0f,-pivot,0.0f);
    yaw=actor->breakYaw;
    gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),s16(-yaw));
    s16 rx=actor->shape_angle.x,ry=actor->shape_angle.y,rz=actor->shape_angle.z;
    gabi::call<void>(0x025F19F8,gabi::at<void>(0x1048D0CC),rx,ry,rz);
}
static void releaseBackground(Actor* actor,unsigned index) {
    u32 background=actor->background[index];
    if(background && read<u32>(background)<0x100) {
        u32 play=gabi::call<u32>(0x025200D4);
        background=actor->background[index];
        gabi::call<void>(0x020087EC,gabi::at<void>(play+0x12A0),gabi::at<void>(background));
    }
}

s32 hcbh_appears(Actor* actor) {
    WWHD_FUNC(0x02351C10, s32, actor);
    u32 bit=gabi::call<u32>(0x02353240,actor,8,13);
    if(bit==255) return 1;
    s8 room=actor->home.roomNo;u32 save=read<u32>(0x101F84DC);
    return gabi::call<s32>(0x025BA0C0,gabi::at<void>(save+0x20),bit,room)==0;
}
VERIFY(0x02351C10, hcbh_appears);

s32 hcbh_createHeap(Actor* actor) {
    WWHD_FUNC(0x02351C74, s32, actor);
    gabi::Local<SafeString> mainName,fragmentName,backgroundName,fragmentBackgroundName;
    mainName->mStringTop=0x1002A0C0;mainName->__vtbl=0x1002A0C8;
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),mainName.get(),8);
    if(!resource) {gabi::call<void>(0x0273AA24,STR(0x1002A200),0x1F3,STR(0x1002A1FC));return 0;}
    s32 result=1;
    actor->model=gabi::call<u32>(0x025E38E0,gabi::at<void>(resource),0x80000,0x11000022);
    for(unsigned i=0;i<4;++i) {
        s32 resourceIndex=read<s32>(0x101C9E04+i*4);
        fragmentName->mStringTop=0x1002A0C0;fragmentName->__vtbl=0x1002A0C8;
        resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),fragmentName.get(),resourceIndex);
        if(!resource) {result=0;break;}
        actor->fragmentModels[i]=gabi::call<u32>(0x025E38E0,gabi::at<void>(resource),0x80000,0x11000022);
    }
    backgroundName->mStringTop=0x1002A0C0;backgroundName->__vtbl=0x1002A0C8;
    resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),backgroundName.get(),12);
    u32 model=actor->model;
    actor->background[0]=gabi::call<u32>(0x024F2478,gabi::at<void>(resource),1,gabi::at<void>(model?model+0xC8:0));
    fragmentBackgroundName->mStringTop=0x1002A0C0;fragmentBackgroundName->__vtbl=0x1002A0C8;
    resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),fragmentBackgroundName.get(),11);
    model=actor->fragmentModels[0];
    u32 background=gabi::call<u32>(0x024F2478,gabi::at<void>(resource),1,gabi::at<void>(model?model+0xC8:0));
    actor->background[1]=background;
    if(!actor->model || !actor->fragmentModels[0] || !actor->fragmentModels[1] || !actor->fragmentModels[2] || !actor->fragmentModels[3] || !actor->background[0] || !background) result=0;
    return result;
}
VERIFY(0x02351C74, hcbh_createHeap);

s32 hcbh_heapCallback(Actor* actor) {
    WWHD_FUNC(0x02351E38, s32, actor);
    return gabi::call<s32>(0x02351C74,actor);
}
VERIFY(0x02351E38, hcbh_heapCallback);

void hcbh_setMtx(Actor* actor) {
    WWHD_FUNC(0x02351E3C, void, actor);
    f32 pivot=50.0f*sine(u16(actor->pillarAngle));
    transformPillar(actor,pivot);
    copyMatrix(actor->model);
    for(unsigned i=0;i<4;++i) {
        f32 x=actor->fragmentPositions[i].x,y=actor->fragmentPositions[i].y,z=actor->fragmentPositions[i].z;
        gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
        s16 yaw=actor->fragmentYaws[i];
        gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),yaw);
        s16 angle=actor->fragmentAngles[i];
        gabi::call<void>(0x025F1BF4,gabi::at<void>(0x1048D0CC),angle);
        yaw=actor->fragmentYaws[i];
        gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),s16(-yaw));
        s16 rx=actor->shape_angle.x,ry=actor->shape_angle.y,rz=actor->shape_angle.z;
        gabi::call<void>(0x025F19F8,gabi::at<void>(0x1048D0CC),rx,ry,rz);
        copyMatrix(actor->fragmentModels[i]);
    }
}
VERIFY(0x02351E3C, hcbh_setMtx);

void hcbh_initMtx(Actor* actor) {
    WWHD_FUNC(0x02352074, void, actor);
    u32 y=read<u32>(gabi::ea(actor)+0x334),x=read<u32>(gabi::ea(actor)+0x330);u32 model=actor->model;u32 z=read<u32>(gabi::ea(actor)+0x338);
    write<u32>(model+0xC0,y);write<u32>(model+0xBC,x);write<u32>(model+0xC4,z);
    for(unsigned i=0;i<4;++i) {
        model=actor->fragmentModels[i];z=read<u32>(gabi::ea(actor)+0x338);x=read<u32>(gabi::ea(actor)+0x330);y=read<u32>(gabi::ea(actor)+0x334);
        write<u32>(model+0xBC,x);write<u32>(model+0xC4,z);write<u32>(model+0xC0,y);
    }
    gabi::call<void>(0x02351E3C,actor);
}
VERIFY(0x02352074, hcbh_initMtx);

void hcbh_coCallback(Actor* actor, u32 object, fopAc_ac_c* other, u32 otherObject) {
    WWHD_FUNC(0x023520C0, void, actor, object, other, otherObject);
    s32 isActor=gabi::call<s32>(0x025D4604,other);
    if(isActor && other && read<s16>(gabi::ea(other)+14)==0xCA) actor->breakCondition=4;
}
VERIFY(0x023520C0, hcbh_coCallback);

void hcbh_setupBreak(Actor* actor, fopAc_ac_c* other) {
    WWHD_FUNC(0x023527A0, void, actor, other);
    if(!other) return;
    f32 otherX=other->current.pos.x,cylinderX=read<f32>(gabi::ea(actor)+0x728);
    f32 otherZ=other->current.pos.z,cylinderZ=read<f32>(gabi::ea(actor)+0x730);
    s16 yaw=gabi::call<s16>(0x020195B0,cylinderX-otherX,cylinderZ-otherZ);actor->breakYaw=yaw;
    for(unsigned i=0;i<4;++i) {
        s16 fragmentYaw=read<s16>(0x101C9E14+i*2);actor->fragmentYaws[i]=fragmentYaw;actor->fragmentAngles[i]=0;
        fragmentYaw=read<s16>(0x101C9E14+i*2);yaw=actor->breakYaw;
        u16 halfAngle=u16(u32(s32(fragmentYaw)-s32(yaw))>>1);
        actor->fragmentDelays[i]=gabi::ftoi(std::fabs(sine(halfAngle))*15.0f);
    }
}
VERIFY(0x023527A0, hcbh_setupBreak);

void hcbh_checkCollision(Actor* actor) {
    WWHD_FUNC(0x02352870, void, actor);
    if(!gabi::call<s32>(0x025162A4,member(actor,0x610))) return;
    u32 hit=gabi::call<u32>(0x02516300,member(actor,0x610));
    if(hit) {
        u32 type=read<u32>(hit+0x10);
        if(type==0x20 || type==0x10000 || type==0x4000000) {
            actor->breakCondition=type==0x20?3:(type==0x10000?2:1);
            u32 other=gabi::call<u32>(0x02515BBC,member(actor,0x6A4));
            gabi::call<void>(0x023527A0,actor,gabi::at<void>(other));
        } else if(type==8) {
            u32 other=gabi::call<u32>(0x02515BBC,member(actor,0x6A4));
            if(!other || read<s16>(other+14)!=0xBF) {gabi::call<void>(0x0251621C,member(actor,0x610));return;}
            actor->breakCondition=1;
            other=gabi::call<u32>(0x02515BBC,member(actor,0x6A4));
            gabi::call<void>(0x023527A0,actor,gabi::at<void>(other));
        }
    }
    gabi::call<void>(0x0251621C,member(actor,0x610));
}
VERIFY(0x02352870, hcbh_checkCollision);

void hcbh_particleSet(Actor* actor) {
    WWHD_FUNC(0x023529B0, void, actor);
    gabi::Local<csXyz> angle;
    s16 yaw=actor->breakYaw;gabi::call<void>(0x0201A478,angle.get(),0,yaw,0);
    u32 play=gabi::call<u32>(0x025200D4),particles=read<u32>(play+0x5AB0);
    gabi::call<void>(0x025A847C,gabi::at<void>(particles),0,0x82DD,&actor->current.pos,0,0,255,0,-1,member(actor,0x1A8),0,0);
    play=gabi::call<u32>(0x025200D4);particles=read<u32>(play+0x5AB0);
    gabi::call<void>(0x025A847C,gabi::at<void>(particles),0,0x82DE,&actor->current.pos,angle.get(),0,255,0,-1,member(actor,0x1A8),0,0);
    if(actor->breakCondition!=3) {
        play=gabi::call<u32>(0x025200D4);particles=read<u32>(play+0x5AB0);
        gabi::call<void>(0x025A847C,gabi::at<void>(particles),0,0x82DF,&actor->current.pos,angle.get(),0,255,0,-1,member(actor,0x1A8),0,0);
    }
    gabi::call<void>(0x025A5F88,member(actor,0xEC4));
    play=gabi::call<u32>(0x025200D4);particles=read<u32>(play+0x5AB0);
    gabi::call<void>(0x025A847C,gabi::at<void>(particles),2,0xA2E0,&actor->current.pos,angle.get(),0,160,member(actor,0xEC4),-1,member(actor,0x1A8),0,0);
}
VERIFY(0x023529B0, hcbh_particleSet);

void hcbh_makeItem(Actor* actor) {
    WWHD_FUNC(0x02352B04, void, actor);
    u32 item=gabi::call<u32>(0x02353240,actor,6,0);
    u32 saveBit=gabi::call<u32>(0x02353240,actor,7,6);
    gabi::Local<cXyz> position;gabi::Local<csXyz> angle;
    f32 z=actor->current.pos.z,y=actor->current.pos.y,x=actor->current.pos.x;
    position->y=y;position->z=z;position->x=x;
    gabi::call<void>(0x0201A478,angle.get(),0,0,0);
    s8 room=actor->home.roomNo;y=f32(position->y)-30.0f;position->y=y;
    gabi::call<void>(0x025D8120,position.get(),item,saveBit,room,0,angle.get(),1,0);
    u32 bit=gabi::call<u32>(0x02353240,actor,8,13);
    if(bit!=255) {u32 save=read<u32>(0x101F84DC);room=actor->home.roomNo;gabi::call<void>(0x025B9E38,gabi::at<void>(save+0x20),bit,room); }
}
VERIFY(0x02352B04, hcbh_makeItem);

void hcbh_wait(Actor* actor) {
    WWHD_FUNC(0x02352BFC, void, actor);
    gabi::call<void>(0x02352870,actor);
    if(!actor->breakCondition) {
        u32 play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x0200E240,gabi::at<void>(play+0x26A4),member(actor,0x610));return;
    }
    gabi::call<void>(0x023529B0,actor);
    s8 room=actor->current.roomNo;s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,0x6A10,&actor->current.pos,0,reverb);
    write<u32>(gabi::ea(actor)+0x6F4,0);actor->actionAdjustment=0;actor->actionIndex=-1;actor->actionTarget=0x02352CA0;
}
VERIFY(0x02352BFC, hcbh_wait);

s32 hcbh_remove(Actor* actor) {
    WWHD_FUNC(0x0235251C, s32, actor);
    if(actor->appears) {
        gabi::call<void>(0x025204C8,&actor->phase,STR(0x1002A0C0));
        if(actor->heap) {
            for(unsigned i=0;i<2;++i) if(actor->background[i]) {releaseBackground(actor,i);actor->background[i]=0;}
        }
        u32 vtable=read<u32>(gabi::ea(actor)+0xEC4),target=read<u32>(vtable+0x44);
        gabi::call<void>(target,member(actor,0xEC4));
    }
    return 1;
}
VERIFY(0x0235251C, hcbh_remove);

s32 hcbh_execute(Actor* actor) {
    WWHD_FUNC(0x023525EC, s32, actor);
    gabi::call<void>(0x02351E3C,actor);
    gabi::call<void>(0x02515E50,member(actor,0x5F0));
    u32 play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x024F08A8,member(actor,0x3D0),gabi::at<void>(play+0x12A0));
    for(unsigned i=0;i<2;++i) {
        u32 background=actor->background[i];
        if(background && read<u32>(background)<0x100) gabi::call<void>(0x024F43DC,gabi::at<void>(background));
    }
    s16 angle=actor->pillarAngle,speed=actor->pillarAngularVelocity;
    f32 height=actor->pillarHeight,velocity=actor->fallVelocity;
    angle=s16(s32(angle)+s32(speed));actor->pillarHeight=height+velocity;
    actor->pillarAngle=angle<0x4000?angle:0x4000;
    s16 index=actor->actionIndex,adjustment=actor->actionAdjustment;
    u32 self=gabi::ea(actor)+s32(adjustment),target;
    if(index<0) target=actor->actionTarget;
    else {s16 vtableOffset=read<s16>(gabi::ea(actor)+0xEEE);u32 vtable=read<u32>(self+vtableOffset);target=read<u32>(vtable+u32(index)*8+4);}
    gabi::call<void>(target,gabi::at<void>(self));return 1;
}
VERIFY(0x023525EC, hcbh_execute);

s32 hcbh_draw(Actor* actor) {
    WWHD_FUNC(0x023526FC, s32, actor);
    u32 environment=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x025626A4,gabi::at<void>(environment),1,&actor->current.pos,&actor->tevStr);
    environment=gabi::call<u32>(0x02555D0C);u32 model=actor->model;
    gabi::call<void>(0x02562F5C,gabi::at<void>(environment),gabi::at<void>(model),&actor->tevStr);
    model=actor->model;gabi::call<void>(0x025E2DE0,gabi::at<void>(model),0);
    for(unsigned i=0;i<4;++i) {
        environment=gabi::call<u32>(0x02555D0C);model=actor->fragmentModels[i];
        gabi::call<void>(0x02562F5C,gabi::at<void>(environment),gabi::at<void>(model),&actor->tevStr);
        model=actor->fragmentModels[i];gabi::call<void>(0x025E2DE0,gabi::at<void>(model),0);
    }
    return 1;
}
VERIFY(0x023526FC, hcbh_draw);

s32 hcbh_createWrapper(Actor* actor) {
    WWHD_FUNC(0x02352518, s32, actor);
    return gabi::call<s32>(0x02352120,actor);
}
VERIFY(0x02352518, hcbh_createWrapper);

s32 hcbh_deleteWrapper(Actor* actor) {
    WWHD_FUNC(0x023525E8, s32, actor);
    return gabi::call<s32>(0x0235251C,actor);
}
VERIFY(0x023525E8, hcbh_deleteWrapper);

s32 hcbh_executeWrapper(Actor* actor) {
    WWHD_FUNC(0x023526F8, s32, actor);
    return gabi::call<s32>(0x023525EC,actor);
}
VERIFY(0x023526F8, hcbh_executeWrapper);

s32 hcbh_drawWrapper(Actor* actor) {
    WWHD_FUNC(0x02352794, s32, actor);
    return gabi::call<s32>(0x023526FC,actor);
}
VERIFY(0x02352794, hcbh_drawWrapper);

s32 hcbh_isDelete(Actor* actor) {
    WWHD_FUNC(0x02352798, s32, actor);
    return 1;
}
VERIFY(0x02352798, hcbh_isDelete);

u32 hcbh_parameter(Actor* actor, u32 width, u32 shift) {
    WWHD_FUNC(0x02353240, u32, actor, width, shift);
    u32 mask=width&0x20?0:(1u<<(width&31));
    u32 value=shift&0x20?0:(u32(actor->mParameters)>>(shift&31));return value&(mask-1);
}
VERIFY(0x02353240, hcbh_parameter);

void hcbh_empty() {
    WWHD_FUNC(0x0235323C, void);

}
VERIFY(0x0235323C, hcbh_empty);

void hcbh_staticDtor(void* object, s32 flags) {
    WWHD_FUNC(0x0235316C, void, object, flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x0235316C, hcbh_staticDtor);

void hcbh_staticInit() {
    WWHD_FUNC(0x023530C8, void);
    write<u32>(0x10469E34,0);write<u32>(0x10469E2C,0);write<u32>(0x10469E38,0);write<u32>(0x10469E30,0);
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C9E1C));
    write<f32>(0x10469E1C,-3.1415927410125732f);write<f32>(0x10469E20,3.1415927410125732f);
    gabi::call<void>(0x028ED6F8,gabi::at<void>(0x10469E28));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C9E28));
    gabi::call<void>(0x028EAB2C,gabi::at<void>(0x10469E29));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C9E34));
    write<f32>(0x10469E24,589.0f);
}
VERIFY(0x023530C8, hcbh_staticInit);

void hcbh_actorDtor(Actor* actor, s32 flags) {
    WWHD_FUNC(0x02353180, void, actor, flags);
    if(!actor) return;
    gabi::call<void>(0x028F0164,member(actor,0x740),6,0x12C,gabi::at<void>(0x02515AE8),0,0);
    gabi::call<void>(0x02515A70,member(actor,0x610),2);
    gabi::call<void>(0x02515860,member(actor,0x5D4),2);
    gabi::call<void>(0x02018034,member(actor,0x5A8),2);
    write<u32>(gabi::ea(actor)+0x3F0,0x1002A100);write<u32>(gabi::ea(actor)+0x3E4,0x1002A110);
    gabi::call<void>(0x024EFD9C,member(actor,0x3D0),0);
    gabi::call<void>(0x025D50BC,actor,0);
    if(flags&1) gabi::call<void>(0x0273AF40,actor);
}
VERIFY(0x02353180, hcbh_actorDtor);

s32 hcbh_create(Actor* actor) {
    WWHD_FUNC(0x02352120, s32, actor);
    u32 condition=actor->actor_condition;
    if(!(condition&8)) {
        if(actor) {
            gabi::call<void>(0x025D4ED0,actor);actor->__vtbl=0x1002A120;
            gabi::call<void>(0x024F0474,member(actor,0x3D0));
            write<u32>(gabi::ea(actor)+0x3E0,0x1002A0F0);write<u32>(gabi::ea(actor)+0x3E4,0x1002A110);
            write<u8>(gabi::ea(actor)+0x3E8,1);write<u32>(gabi::ea(actor)+0x3F0,0x1002A100);
            gabi::call<void>(0x024EFE94,member(actor,0x594));
            gabi::call<void>(0x0200BD2C,member(actor,0x5D4));gabi::call<void>(0x02515DA0,member(actor,0x5F0));
            write<u32>(gabi::ea(actor)+0x5EC,0x1004AE88);write<u32>(gabi::ea(actor)+0x5F0,0x1004AEC0);
            gabi::call<void>(0x02515FB8,member(actor,0x610));
            write<u32>(gabi::ea(actor)+0x724,0x100015A8);write<u32>(gabi::ea(actor)+0x720,0x1002A0E0);
            gabi::call<void>(0x02018590,member(actor,0x728));
            write<u32>(gabi::ea(actor)+0x73C,0x1004B150);write<u32>(gabi::ea(actor)+0x724,0x1004B160);write<u32>(gabi::ea(actor)+0x64C,0x1004B108);
            gabi::call<void>(0x028EFFD0,member(actor,0x740),6,0x12C,gabi::at<void>(0x025166F0));
            gabi::call<void>(0x025A5B18,member(actor,0xEC4),1);
            condition=actor->actor_condition;
        }
        actor->actor_condition=condition|8;
    }
    if(read<u8>(gabi::ea(actor)+12)) {if(!actor->appears) return 5;}
    else {
        f32 height=actor->current.pos.y;actor->pillarAngle=0;actor->pillarHeight=height;
        for(unsigned i=0;i<4;++i) {
            actor->fragmentPositions[i].x=actor->current.pos.x;
            actor->fragmentPositions[i].y=actor->current.pos.y;
            actor->fragmentPositions[i].z=actor->current.pos.z;
            actor->fragmentAngles[i]=0;actor->fragmentYaws[i]=0;
        }
        s32 appears=gabi::call<s32>(0x02351C10,actor);actor->appears=appears;
        if(!appears) return 5;
    }
    s32 phase=gabi::call<s32>(0x02520460,&actor->phase,STR(0x1002A0C0));
    if(phase!=4) return phase;
    if(!gabi::call<s32>(0x025D63E8,actor,gabi::at<void>(0x02351E38),0x2D00)) return 5;
    u32 play=gabi::call<u32>(0x025200D4),background=actor->background[0];
    if(gabi::call<s32>(0x024EEA6C,gabi::at<void>(play+0x12A0),gabi::at<void>(background),actor)) return 5;
    play=gabi::call<u32>(0x025200D4);background=actor->background[1];
    if(gabi::call<s32>(0x024EEA6C,gabi::at<void>(play+0x12A0),gabi::at<void>(background),actor)) return 5;
    u32 model=actor->model;actor->cullMtx=model?model+0xC8:0;
    gabi::call<void>(0x02352074,actor);
    gabi::call<void>(0x024EFF44,member(actor,0x594),589.0f,70.0f);
    gabi::call<void>(0x024F06B4,member(actor,0x3D0),&actor->current.pos,&actor->old.pos,actor,1,member(actor,0x594),&actor->speed,&actor->current.angle,&actor->shape_angle);
    u32 flags=read<u32>(gabi::ea(actor)+0x3F8);write<f32>(gabi::ea(actor)+0x490,589.0f);write<u32>(gabi::ea(actor)+0x3F8,flags&~0x408u);
    play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x024F08A8,member(actor,0x3D0),gabi::at<void>(play+0x12A0));
    flags=read<u32>(gabi::ea(actor)+0x3F8);write<u32>(gabi::ea(actor)+0x3F8,flags&~0x80u);
    gabi::call<void>(0x02515F14,member(actor,0x5D4),255,255,actor);
    gabi::call<void>(0x02516518,member(actor,0x610),gabi::at<void>(0x1002A170));
    write<u32>(gabi::ea(actor)+0x654,gabi::ea(actor)+0x5D4);
    gabi::call<void>(0x020182E0,member(actor,0x728),&actor->current.pos);
    write<u32>(gabi::ea(actor)+0x6F4,0x023520C0);
    for(unsigned i=0;i<6;++i) {
        u32 sphere=gabi::ea(actor)+0x740+i*0x12C;
        gabi::call<void>(0x0251677C,gabi::at<void>(sphere),gabi::at<void>(0x1002A130));
        write<u32>(sphere+0x44,gabi::ea(actor)+0x5D4);
        gabi::call<void>(0x02018C8C,gabi::at<void>(sphere+0x118),70.0f);
        f32 height=actor->current.pos.y,yOffset=read<f32>(0x1002A1B4+i*12+4),x=actor->current.pos.x,z=actor->current.pos.z;
        gabi::call<void>(0x02018F3C,gabi::at<void>(sphere+0x118),x,height+yOffset,z);
    }
    write<u8>(gabi::ea(actor)+0xED5,0);actor->actionAdjustment=0;
    write<u8>(gabi::ea(actor)+0xED6,1);actor->actionTarget=0x02352BFC;actor->actionIndex=-1;
    write<u32>(gabi::ea(actor)+0xEE0,gabi::ea(actor)+0x110);
    gabi::call<void>(0x025D674C,actor,-40.0f,0.0f,-40.0f,100.0f,589.0f,100.0f);
    return phase;
}
VERIFY(0x02352120, hcbh_create);

void hcbh_fall(Actor* actor) {
    WWHD_FUNC(0x02352CA0, void, actor);
    s16 angle=actor->pillarAngle;
    if(angle<0) {actor->pillarAngle=-32768;actor->pillarAngularVelocity=0;}
    else {
        if(angle>0x2000) {releaseBackground(actor,0);releaseBackground(actor,1);angle=actor->pillarAngle;}
        f32 acceleration=gabi::fmadds(127.0f,sine(u16(angle)),4.0f);
        s16 velocity=actor->pillarAngularVelocity;
        actor->pillarAngularVelocity=s16(s32(velocity)+s32(s16(gabi::ftoi(acceleration))));
    }
    f32 homeHeight=actor->home.pos.y-10.0f,height=actor->pillarHeight;
    if(height<homeHeight) {
        gabi::call<void>(0x02352B04,actor);
        u32 play=gabi::call<u32>(0x025200D4);
        gabi::Local<cXyz> direction;direction->z=0.0f;direction->x=0.0f;direction->y=1.0f;
        gabi::call<void>(0x025CB374,gabi::at<void>(play+0x599C),8,-17,direction.get());
        s8 room=actor->current.roomNo;s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call<void>(0x025E1A40,0x6A11,&actor->current.pos,0,reverb);
        gabi::call<void>(0x025D57E0,actor);
    } else {
        f32 velocity=actor->fallVelocity;
        u16 angle=actor->pillarAngle;
        actor->fallVelocity=velocity-0.011f;
        f32 pivot=50.0f*sine(angle);
        // The original loads x after storing the velocity and reading the sine table.
        transformPillar(actor,pivot);
        for(unsigned i=0;i<6;++i) {
            gabi::Local<cXyz> center;
            f32 x=read<f32>(0x1002A1B4+i*12),z=read<f32>(0x1002A1B4+i*12+8),y=read<f32>(0x1002A1B4+i*12+4);
            center->z=z;center->y=y;center->x=x;
            gabi::call<void>(0x028E8F64,gabi::at<void>(0x1048D0CC),center.get(),center.get());
            u32 sphere=gabi::ea(actor)+0x740+i*0x12C;
            gabi::call<void>(0x025167E4,gabi::at<void>(sphere),center.get());
            u32 play=gabi::call<u32>(0x025200D4);
            gabi::call<void>(0x0200E240,gabi::at<void>(play+0x26A4),gabi::at<void>(sphere));
        }
    }
    for(unsigned i=0;i<4;++i) {
        s32 delay=actor->fragmentDelays[i];
        if(delay>0) {actor->fragmentDelays[i]=delay-1;continue;}
        f32 velocity=f32(actor->fragmentVelocities[i])-0.01f;actor->fragmentVelocities[i]=velocity;
        f32 y=f32(actor->fragmentPositions[i].y)+velocity;actor->fragmentPositions[i].y=y;
        f32 floor=f32(actor->home.pos.y)-100.0f;
        if(y<floor) actor->fragmentPositions[i].y=floor;
        u16 angle=actor->fragmentAngles[i];
        f32 acceleration=gabi::fmadds(512.0f,sine(angle),4.0f);
        s16 velocityAngle=actor->fragmentAngularVelocities[i];
        s32 sum=s32(velocityAngle)+s32(s16(gabi::ftoi(acceleration)));
        actor->fragmentAngularVelocities[i]=s16(sum);
        s16 currentAngle=actor->fragmentAngles[i];s16 next=s16(s32(currentAngle)+sum);
        actor->fragmentAngles[i]=next>0x4000?0x4000:next;
    }
}
VERIFY(0x02352CA0, hcbh_fall);
