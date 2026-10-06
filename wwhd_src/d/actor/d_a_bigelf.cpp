/* WWHD Great Fairy actor. Derived game code: */
#include "d/actor/d_a_bigelf.h"
namespace Bigelf {
static u32 getType(Actor* actor) {
    WWHD_FUNC(0x020948E0,u32,actor);
    return gabi::load<u8>(gabi::ea(actor)+0xB3);
}
VERIFY(0x020948E0,getType);
static u32 getEventFlag(Actor* actor) {
    WWHD_FUNC(0x02094ED4,u32,actor);
    u32 type=getType(actor);
    return type<=6?gabi::load<u16>(0x10009214+2*type):0;
}
VERIFY(0x02094ED4,getEventFlag);
static void makeFa1S(Actor* actor) {
    WWHD_FUNC(0x02094F20,void,actor);
    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    position->x=actor->current.pos.x;
    position->y=(f32)((f64)(f32)actor->current.pos.y+(f64)(f32)actor->fairyHeight);
    position->z=actor->current.pos.z;
    angle->x=actor->current.angle.x;
    angle->y=actor->current.angle.y;
    angle->z=actor->current.angle.z;
    for(u32 i=0;i<10;++i) {
        gabi::call<u32>(0x025D5834,0x168,4,position.get(),(s32)(s8)actor->current.roomNo,angle.get(),0,-1,0);
        angle->y=(s16)((u32)(s32)(s16)angle->y+10000u);
    }
}
VERIFY(0x02094F20,makeFa1S);
static void makeFa1(Actor* actor) {
    WWHD_FUNC(0x02094FC8,void,actor);
    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    angle->x=actor->current.angle.x;
    angle->y=actor->current.angle.y;
    angle->z=actor->current.angle.z;
    position->x=actor->current.pos.x;
    position->y=(f32)((f64)(f32)actor->current.pos.y+(f64)gabi::load<f32>(0x10009224));
    position->z=actor->current.pos.z;
    actor->fairy=gabi::call<u32>(0x025D5834,0x168,6,position.get(),(s32)(s8)actor->current.roomNo,angle.get(),0,-1,0);
}
VERIFY(0x02094FC8,makeFa1);
static Actor* construct(Actor* actor) {
    WWHD_FUNC(0x02094E50,Actor*,actor);
    if(!actor) {
        actor=gabi::call<Actor*>(0x0273AD10,0x644);
        if(!actor) return nullptr;
    }
    gabi::call<void>(0x025D4ED0,actor);
    actor->__vtbl=0x10009144;
    u32 address=gabi::ea(actor);
    gabi::call<void>(0x025E80D0,gabi::at<u8>(address+0x3B8));
    gabi::call<void>(0x025E7C6C,gabi::at<u8>(address+0x430));
    gabi::call<void>(0x025E80D0,gabi::at<u8>(address+0x4A8));
    gabi::call<void>(0x0259DAA0,gabi::at<u8>(address+0x524));
    f32 value=gabi::load<f32>(0x10009170);
    gabi::store<f32>(address+0x5D4,value);
    gabi::store<f32>(address+0x5B0,value);
    return actor;
}
VERIFY(0x02094E50,construct);
static u32 getSwbit(Actor* actor) {
    WWHD_FUNC(0x020958E0,u32,actor);
    return ((u32)actor->mParameters>>8)&255;
}
VERIFY(0x020958E0,getSwbit);
static u32 getSwbit2(Actor* actor) {
    WWHD_FUNC(0x020958EC,u32,actor);
    return ((u32)actor->mParameters>>16)&255;
}
VERIFY(0x020958EC,getSwbit2);
static u32 getMsg(Actor* actor) {
    WWHD_FUNC(0x02095BC8,u32,actor);
    u32 message=actor->message;
    if(message==12014 && gabi::load<u8>(gabi::load<u32>(0x101F84DC)+0x68)==39) {
        message=12015; actor->message=message;
    }
    return message;
}
VERIFY(0x02095BC8,getMsg);
static void talkInit(Actor* actor) {
    WWHD_FUNC(0x02095BF8,void,actor);
    actor->talking=0; actor->messageState=255;
}
VERIFY(0x02095BF8,talkInit);
static void lightProc(Actor* actor) {
    WWHD_FUNC(0x020965D0,void,actor); actor->lightState=0;
}
VERIFY(0x020965D0,lightProc);
static void demoInitFlDelete(Actor* actor) {
    WWHD_FUNC(0x0209675C,void,actor);
    actor->demoCounter=0; actor->lightState=255;
}
VERIFY(0x0209675C,demoInitFlDelete);
static void darkInit(Actor* actor) {
    WWHD_FUNC(0x02096770,void,actor);
    if(getType(actor)!=6) gabi::store<u8>(gabi::ea(actor)+0x5E0,1);
}
VERIFY(0x02096770,darkInit);
static void lightEnd(Actor* actor) {
    WWHD_FUNC(0x02096ECC,void,actor);
    u32 address=gabi::ea(actor);
    if(gabi::load<u8>(address+0x58E)) {
        gabi::store<u8>(address+0x58E,0);
        gabi::call<void>(0x0255A374,gabi::at<u8>(address+0x590));
    }
}
VERIFY(0x02096ECC,lightEnd);
static BOOL IsDelete(Actor* actor) {
    WWHD_FUNC(0x020958D8,BOOL,actor); return 1;
}
VERIFY(0x020958D8,IsDelete);
static void trivialDestructor(void* object,u32 flags) {
    WWHD_FUNC(0x02097FD8,void,object,flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02097FD8,trivialDestructor);
static void actorDestructor(Actor* actor,u32 flags) {
    WWHD_FUNC(0x02097FEC,void,actor,flags);
    if(actor) {
        gabi::call<void>(0x025D50BC,actor,0);
        if(flags&1) gabi::call<void>(0x0273AF40,actor);
    }
}
VERIFY(0x02097FEC,actorDestructor);
static void emptyVirtual() { WWHD_FUNC(0x02098040,void); }
VERIFY(0x02098040,emptyVirtual);
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 save() { return gabi::load<u32>(0x101F84DC); }
static void* bytes(u32 address) { return gabi::at<u8>(address); }
static f32 constant(u32 address) { return gabi::load<f32>(address); }
static void setAnm(Actor*,s32);
static u32 talk(Actor*);
static void demoProc(Actor*);
static void demoInitWait(Actor*);
static void lightInit(Actor* actor,cXyz* position) {
    WWHD_FUNC(0x020965DC,void,actor,position);
    u32 a=gabi::ea(actor),p=gabi::ea(position);
    u32 x=gabi::load<u32>(p); gabi::store<u32>(a+0x590,x);
    u32 y=gabi::load<u32>(p+4); u8 registered=actor->lightRegistered;
    gabi::store<u32>(a+0x594,y);
    u32 z=gabi::load<u32>(p+8);
    gabi::store<u32>(a+0x5B4,x); gabi::store<u32>(a+0x598,z);
    gabi::store<u32>(a+0x5B8,y); gabi::store<u32>(a+0x5BC,z);
    if(registered) return;
    z=gabi::load<u32>(a+0x598); y=gabi::load<u32>(a+0x594);
    f32 zero=constant(0x10009164);
    actor->lightTargetColor[2]=255;
    actor->lightPower=zero; actor->lightTargetFluctuation=zero;
    actor->lightFluctuation=zero; actor->lightTargetPower=zero;
    gabi::store<u32>(a+0x5BC,z); actor->lightTargetColor[0]=255;
    actor->lightColor[2]=255; actor->lightRegistered=1;
    actor->lightTargetColor[1]=255; actor->lightColor[1]=255; actor->lightColor[0]=255;
    x=gabi::load<u32>(a+0x590);
    gabi::store<u32>(a+0x5B8,y); gabi::store<u32>(a+0x5B4,x);
    gabi::call<void>(0x0255A2B8,&actor->lightPosition);
}
VERIFY(0x020965DC,lightInit);
static void demoInitFlDmMd(Actor* actor) {
    WWHD_FUNC(0x020967A4,void,actor);
    darkInit(actor);
    u16 flags=actor->stateBits;
    f32 one=constant(0x10009170);
    actor->darkGain=one; actor->darkTarget=one;
    actor->stateBits=(flags&0xFF7F)|0x600;
}
VERIFY(0x020967A4,demoInitFlDmMd);
static void darkEnd(Actor* actor) {
    WWHD_FUNC(0x0209641C,void,actor);
    f32 one=constant(0x10009170); actor->darkActive=0;
    gabi::call<void>(0x0255A3D8,one); gabi::call<void>(0x0255A418,one); gabi::call<void>(0x0255A51C,one);
}
VERIFY(0x0209641C,darkEnd);
static void darkProc(Actor* actor) {
    WWHD_FUNC(0x0209744C,void,actor);
    if(actor->darkActive) {
        f32 target=actor->darkTarget;
        f32 maximum=constant(0x10009170),rate=constant(0x1000924C);
        gabi::call<void>(0x0200ED84,&actor->darkGain,target,rate,maximum);
        f32 gain=actor->darkGain;
        f32 scale=constant(0x10009360),base=constant(0x10009364);
        gabi::call<void>(0x0255A3D8,gabi::fmadds(scale,gain,base));
        gain=actor->darkGain; scale=constant(0x10009368); base=constant(0x1000936C);
        f32 factor=gabi::fmadds(scale,gain,base);
        gabi::call<void>(0x0255A418,factor); gabi::call<void>(0x0255A51C,factor);
    }
}
VERIFY(0x0209744C,darkProc);
static void demoProcCom(Actor* actor) {
    WWHD_FUNC(0x020974F8,void,actor); darkProc(actor);
}
VERIFY(0x020974F8,demoProcCom);
static void setAttention(Actor* actor,BOOL force) {
    WWHD_FUNC(0x02095D70,void,actor,force);
    s16 delay=actor->attentionDelay;
    if(delay>0) { actor->attentionDelay=(s16)(delay-1); return; }
    if(!force && (u8)actor->attentionSetCount>=2) return;
    f32 x=actor->attentionPosition.x,y=actor->attentionPosition.y;
    f32 height=constant(0x1000922C);
    gabi::store<f32>(gabi::ea(actor)+0x390,x);
    y=(f32)((f64)y+(f64)height);
    f32 z=actor->attentionPosition.z,eyeZ=actor->headPosition.z;
    gabi::store<f32>(gabi::ea(actor)+0x394,y);
    gabi::store<f32>(gabi::ea(actor)+0x398,z);
    actor->eyePos.z=eyeZ;
    f32 eyeX=actor->headPosition.x,eyeY=actor->headPosition.y;
    actor->eyePos.x=eyeX; actor->eyePos.y=eyeY;
}
VERIFY(0x02095D70,setAttention);
static void lookBack(Actor* actor) {
    WWHD_FUNC(0x02095DDC,void,actor);
    u32 p=play(); u16 flags=actor->stateBits;
    u32 player=gabi::load<u32>(p+0x5B34);
    if(!(flags&0x20)) {
        s16 yaw=gabi::call<s16>(0x025D6894,actor,bytes(player));
        actor->shape_angle.y=yaw; actor->current.angle.y=yaw;
    }
}
VERIFY(0x02095DDC,lookBack);
static s32 getNowEventAction(Actor* actor) {
    WWHD_FUNC(0x02095F14,s32,actor);
    s32 staff=actor->staffID;
    return gabi::call<s32>(0x02542EDC,bytes(play()+0x52C4),staff,bytes(0x101912FC),11,0,1);
}
VERIFY(0x02095F14,getNowEventAction);
static void demoInitWait(Actor* actor) {
    WWHD_FUNC(0x02096034,void,actor);
    s32 staff=actor->staffID;
    u32 timer=gabi::call<u32>(0x0254487C,bytes(play()+0x52C4),staff,bytes(0x100092EC),3);
    actor->demoCounter=timer?gabi::load<s16>(timer+2):0;
    setAnm(actor,0);
}
VERIFY(0x02096034,demoInitWait);
static u32 demoProcWait(Actor* actor) {
    WWHD_FUNC(0x0209689C,u32,actor);
    s16 count=actor->demoCounter;
    if(count>0) actor->demoCounter=(s16)(count-1);
    else {
        s32 staff=actor->staffID;
        gabi::call<void>(0x02543280,bytes(play()+0x52C4),staff);
    }
    return 0;
}
VERIFY(0x0209689C,demoProcWait);
static u32 demoProcFlLink(Actor* actor) {
    WWHD_FUNC(0x02096EA8,u32,actor); demoProcWait(actor); return 1;
}
VERIFY(0x02096EA8,demoProcFlLink);
static u32 findActor(u32 id) {
    gabi::Local<be<u32>> argument; *argument=id;
    if(id==0xFFFFFFFF) return 0;
    return gabi::call<u32>(0x025D5218,bytes(0x025E1234),argument.get());
}
static void demoInitFa1(Actor* actor) {
    WWHD_FUNC(0x020960A4,void,actor);
    u32 fairy=findActor(actor->fairy);
    if(fairy) {
        gabi::call<void>(0x02234CB8,bytes(fairy));
        s32 reverb=gabi::call<s32>(0x02520540,(s32)(s8)actor->current.roomNo);
        gabi::call<void>(0x025E1A40,0x594A,&actor->eyePos,0,reverb);
    }
}
VERIFY(0x020960A4,demoInitFa1);
static u32 demoProcFa1(Actor* actor) {
    WWHD_FUNC(0x020968F4,u32,actor);
    u32 fairy=findActor(actor->fairy);
    if(fairy) {
        f32 y=actor->current.pos.y;
        f32 height=constant(0x100092F4),rate=constant(0x10009330);
        f32 target=(f32)((f64)y+(f64)height),max=constant(0x10009224);
        gabi::call<void>(0x0200ED84,bytes(fairy+0x318),target,rate,max);
    }
    s32 staff=actor->staffID;
    gabi::call<void>(0x02543280,bytes(play()+0x52C4),staff); return 1;
}
VERIFY(0x020968F4,demoProcFa1);
static u32 demoProcTalk(Actor* actor) {
    WWHD_FUNC(0x02096ACC,u32,actor);
    u32 status=talk(actor);
    if(status==0x12 || status==0xFE) {
        s32 staff=actor->staffID;
        gabi::call<void>(0x02543280,bytes(play()+0x52C4),staff);
    }
    return 1;
}
VERIFY(0x02096ACC,demoProcTalk);
static u32 hunt(Actor* actor) {
    WWHD_FUNC(0x02095E28,u32,actor);
    u32 fairy=findActor(actor->fairy);
    u32 player=gabi::load<u32>(play()+0x5B34);
    if(!fairy) { actor->state=3; return 0; }
    f32 distance=gabi::call<f32>(0x025D6958,actor,bytes(player));
    f32 limit=constant(0x10009268);
    if(distance<limit) {
        actor->state=1;
        u32 event=gabi::call<u32>(0x02543F10,bytes(play()+0x52C4),bytes(0x1000926C),255);
        actor->eventID=(s16)event;
        gabi::call<void>(0x025D7A58,actor,event,255,0xFFFF,0,1);
    }
    return 1;
}
VERIFY(0x02095E28,hunt);
static BOOL remove(Actor* actor) {
    WWHD_FUNC(0x02095374,BOOL,actor);
    gabi::call<void>(0x025204C8,&actor->phase,bytes(0x10009238));
    u32 animator=actor->animator;
    if(animator) gabi::call<void>(0x025E563C,bytes(animator));
    return 1;
}
VERIFY(0x02095374,remove);
static BOOL profileDelete(Actor* actor) { WWHD_FUNC(0x020953C0,BOOL,actor); return remove(actor); }
VERIFY(0x020953C0,profileDelete);
static BOOL octSearch(Actor* actor) {
    WWHD_FUNC(0x02097B10,BOOL,actor);
    u32 oct=gabi::call<u32>(0x025D9F38,bytes(0x10009390),0,0);
    if(oct) {
        actor->octID=gabi::load<u32>(oct+4); actor->state=5; actor->octTimer=10;
    }
    return 1;
}
VERIFY(0x02097B10,octSearch);
static void setAnm(Actor* actor,s32 animation) {
    WWHD_FUNC(0x020958F8,void,actor,animation);
    f32 speed=constant(0x10009170),morph=constant(0x10009258);
    f32 end=constant(0x1000925C),start=constant(0x10009164);
    s32 mode=-1;
    if((u32)animation>=1 && (u32)animation<3) morph=start;
    else if(animation==3) { speed=end; end=constant(0x10009224); mode=3; }
    s32 current=(s8)actor->animation;
    if(current==1) morph=start;
    if((u32)animation==(u32)current || animation==-1) return;
    actor->previousFrame=start; actor->animation=(s8)animation;
    actor->animationFinished=0;
    s32 resource=gabi::load<s32>(0x10009154+4u*(u32)(s32)(s8)animation);
    u32 manager=gabi::load<u32>(0x101F4F28);
    struct ResourceName { be<u32> text,vtable; };
    gabi::Local<ResourceName> name;
    name->vtable=0x1000912C; name->text=0x10009260;
    u32 data=gabi::call<u32>(0x026066C4,bytes(manager),name.get(),resource);
    u32 animator=actor->animator;
    gabi::call<void>(0x025E4A98,bytes(animator),bytes(data),mode,0,morph,speed,start,end);
}
VERIFY(0x020958F8,setAnm);
static void setAnmStatus(Actor* actor) { WWHD_FUNC(0x02095A88,void,actor); setAnm(actor,0); }
VERIFY(0x02095A88,setAnmStatus);
static u32 nextMessageStatus(Actor* actor,be<u32>* message) {
    WWHD_FUNC(0x02095A90,u32,actor,message);
    u32 id=*message;
    if(id==12008) {
        *message=12009;
        u32 type=getType(actor); u8 item;
        if(type<=1) item=gabi::load<u8>(save()+0x32)?0xAC:0xAB;
        else if(type<=3) item=gabi::load<u8>(save()+0x90)>30?0xAE:0xAD;
        else if(type<=5) item=gabi::load<u8>(save()+0x8F)>30?0xB0:0xAF;
        else item=4;
        actor->givenItem=item; gabi::store<u8>(play()+0x52A4,item);
        return 15;
    }
    if(id==12011 || id==12012 || id==12015 || id==12016) { *message=id+1; return 15; }
    if(id==12017) { *message=12014; return 15; }
    return 16;
}
VERIFY(0x02095A90,nextMessageStatus);
static u32 talk(Actor* actor) {
    WWHD_FUNC(0x02095C0C,u32,actor);
    u32 manager=gabi::load<u32>(0x101F4B5C);
    s32 phase=(s8)actor->talking;
    if(!phase) {
        gabi::store<u32>(0x104622E0,0xFFFFFFFF);
        actor->currentMessage=getMsg(actor); actor->talking=1; return 255;
    }
    if(phase==-1) return 255;
    if(gabi::load<u32>(0x104622E0)==0xFFFFFFFF) {
        u32 message=actor->currentMessage;
        u32 handle=gabi::call<u32>(0x025F7DB0,bytes(manager),message,&actor->eyePos);
        gabi::store<u32>(0x104622E0,handle); return 255;
    }
    if(!((u16)actor->stateBits&4)) { play(); phase=(s8)actor->talking; }
    if(phase==1) { actor->talking=2; return 255; }
    if(phase!=2) return 255;
    u16 status=(u16)gabi::call<u32>(0x025F795C,bytes(manager));
    if(status==14) {
        u32 next=nextMessageStatus(actor,&actor->currentMessage);
        gabi::call<void>(0x025F74D0,bytes(manager),next);
        if(gabi::call<u32>(0x025F795C,bytes(manager))==15) {
            u32 message=actor->currentMessage;
            gabi::call<void>(0x025F7DB0,bytes(manager),message,0);
        }
    } else if(status==18) {
        gabi::call<void>(0x025F74D0,bytes(manager),19); actor->talking=-1;
    }
    return status;
}
VERIFY(0x02095C0C,talk);
static u32 particle(u16 effect,cXyz* position,csXyz* angle=nullptr,cXyz* scale=nullptr) {
    u32 manager=gabi::load<u32>(play()+0x5AB0);
    return gabi::call<u32>(0x025A847C,bytes(manager),0,effect,position,angle,scale,255,0,-1,0,0,0);
}
static void sound(Actor* actor,u32 id) {
    s32 reverb=gabi::call<s32>(0x02520540,(s32)(s8)actor->current.roomNo);
    gabi::call<void>(0x025E1A40,id,&actor->eyePos,0,reverb);
}
static void cutEnd(Actor* actor) {
    s32 staff=actor->staffID;
    gabi::call<void>(0x02543280,bytes(play()+0x52C4),staff);
}
static u16 colorEffect(Actor* actor,u32 table) {
    return gabi::load<u16>(table+2u*(u32)(s32)(s8)actor->colorType);
}
static void retireEmitter(u32 emitter) {
    u32 flags=gabi::load<u32>(emitter+0x254);
    gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);
    gabi::store<u32>(emitter+0x254,flags|1);
}
static void demoInitCom(Actor* actor) {
    WWHD_FUNC(0x02095F60,void,actor);
    u16 flags=actor->stateBits; s32 staff=actor->staffID;
    actor->stateBits=flags|1;
    if(!gabi::call<u32>(0x0254487C,bytes(play()+0x52C4),staff,bytes(0x100092E4),3)) return;
    u32 ship=gabi::load<u32>(play()+0x5B3C);
    if(ship) {
        gabi::Local<cXyz> offset,position;
        f32 zero=constant(0x10009164),distance=constant(0x100092E0);
        offset->y=zero; offset->z=distance; offset->x=zero;
        gabi::call<void>(0x025DA124,actor,offset.get(),position.get());
        position->y=gabi::load<f32>(ship+0x318);
        u32 yaw=gabi::call<u32>(0x0200F93C,&actor->current.pos,position.get());
        gabi::call<void>(0x024832E0,bytes(ship),position.get(),(s32)(s16)(yaw+0x4000u));
    }
    actor->stateBits=(u16)actor->stateBits|0x20;
}
VERIFY(0x02095F60,demoInitCom);
static void demoInitAppear(Actor* actor) {
    WWHD_FUNC(0x0209611C,void,actor);
    gabi::Local<cXyz> position,direction;
    u32 fairy=findActor(actor->fairy);
    if(fairy) {
        u32 event=play()+0x51D0;
        u32 process=gabi::call<u32>(0x0253F124,bytes(event),actor);
        gabi::store<u32>(event+0xCC,process);
        gabi::call<void>(0x025D57E0,bytes(fairy));
    }
    f32 x=actor->current.pos.x,y=actor->current.pos.y;
    f32 height=constant(0x100092F4),z=actor->current.pos.z;
    position->x=x; position->z=z; position->y=(f32)((f64)y+(f64)height);
    u32 p=play(); f32 zero=constant(0x10009164),one=constant(0x10009170);
    direction->z=zero; direction->y=one; direction->x=zero;
    gabi::call<void>(0x025CB374,bytes(p+0x599C),5,-33,direction.get());
    u16 effect=colorEffect(actor,0x10191328);
    particle(effect,position.get(),nullptr,&actor->scale);
    effect=colorEffect(actor,0x10191330);
    particle(effect,position.get(),nullptr,&actor->scale);
    if(getType(actor)==6) actor->attentionDelay=15;
    actor->flowerSpeed=zero; actor->lightState=15;
    if(getType(actor)==6) { sound(actor,0x594D); gabi::call<void>(0x025E18EC,0x80000053u); }
    actor->stateBits=(u16)actor->stateBits|0x100;
}
VERIFY(0x0209611C,demoInitAppear);
static void demoInitTalk(Actor* actor) {
    WWHD_FUNC(0x02096328,void,actor);
    talkInit(actor);
    s32 staff=actor->staffID;
    u32 message=gabi::call<u32>(0x0254487C,bytes(play()+0x52C4),staff,bytes(0x10009300),3);
    if(!message) gabi::call<void>(0x0273AA24,bytes(0x10009310),0x333,bytes(0x10009308));
    u32 id=gabi::load<u32>(message); actor->message=id;
    if(id==12010) {
        f32 rupees=(f32)gabi::load<u16>(save()+0x20);
        u32 p=play(); f32 old=gabi::load<f32>(p+0x5B44);
        gabi::store<f32>(p+0x5B44,(f32)((f64)old+(f64)rupees));
        u8 count=gabi::load<u8>(save()+0x33);
        p=play(); s16 previous=gabi::load<s16>(p+0x5B60);
        gabi::store<u16>(p+0x5B60,(u16)((u32)(s32)previous+count));
    }
}
VERIFY(0x02096328,demoInitTalk);
static void demoInitExit(Actor* actor) {
    WWHD_FUNC(0x02096478,void,actor);
    gabi::Local<cXyz> position,scale;
    u32 emitter=actor->appearEmitter;
    if(emitter) { retireEmitter(emitter); actor->appearEmitter=0; }
    u16 flags=actor->stateBits;
    f32 y=actor->current.pos.y,height=constant(0x10009320),one=constant(0x10009170);
    actor->demoCounter=0; f32 x=actor->current.pos.x;
    actor->disappearTarget=one; scale->y=one;
    position->x=x; position->y=(f32)((f64)y+(f64)height);
    scale->x=one; actor->disappearScale=one; scale->z=one;
    actor->stateBits=flags|0x10;
    f32 z=actor->current.pos.z; actor->fairyHeight=height; position->z=z;
    particle(0x272,position.get(),nullptr,scale.get());
    s32 room=(s8)actor->current.roomNo;
    f32 zero=constant(0x10009164);
    u32 a=gabi::ea(actor);
    gabi::store<u16>(a+0x1B0,255); gabi::store<f32>(a+0x1B8,zero);
    gabi::store<u16>(a+0x1B4,255); f32 power=constant(0x10009324);
    gabi::store<u16>(a+0x1B2,255); gabi::store<f32>(a+0x1BC,power);
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,0x594B,&actor->eyePos,0,reverb);
    sound(actor,0x492D); darkEnd(actor);
}
VERIFY(0x02096478,demoInitExit);
static void demoInitFlLink(Actor* actor) {
    WWHD_FUNC(0x0209666C,void,actor);
    gabi::Local<cXyz> offset,position;
    u32 player=gabi::load<u32>(play()+0x5B34);
    f32 zero=constant(0x10009164),radius=constant(0x1000932C),height=constant(0x10009328);
    offset->z=radius; offset->x=zero; offset->y=height;
    gabi::call<void>(0x025DA124,bytes(player),offset.get(),position.get());
    u16 effect=colorEffect(actor,0x10009118);
    actor->linkEmitter=particle(effect,position.get(),&actor->shape_angle);
    lightInit(actor,position.get()); actor->lightRadius=radius; demoInitWait(actor);
}
VERIFY(0x0209666C,demoInitFlLink);
static void demoInitFlDmAf(Actor* actor) {
    WWHD_FUNC(0x020967E8,void,actor);
    gabi::Local<cXyz> position;
    f32 y=actor->handPosition.y,x=actor->handPosition.x,height=constant(0x10009168);
    position->x=x; position->y=(f32)((f64)y+(f64)height);
    f32 z=actor->handPosition.z;
    u16 effect=colorEffect(actor,0x10009118); position->z=z;
    actor->changeEmitter=particle(effect,position.get(),&actor->shape_angle);
    gabi::call<void>(0x025E1988,0x7853);
}
VERIFY(0x020967E8,demoInitFlDmAf);
static u32 demoProcAppear(Actor* actor) {
    WWHD_FUNC(0x02096988,u32,actor);
    u8 timer=actor->lightState;
    if(timer) {
        timer=(u8)(timer-1); actor->lightState=timer;
        if(timer) return 1;
        actor->stateBits=(u16)actor->stateBits&0xFFFD; setAnm(actor,1);
        if(!actor->appearEmitter) actor->appearEmitter=particle(0x8346,&actor->current.pos);
        return 1;
    }
    u16 flags=actor->stateBits;
    if(flags&0x100) {
        f32 frame=gabi::load<f32>((u32)actor->animator+0x9C),threshold=constant(0x10009334);
        if(!(frame<threshold)) {
            s32 room=(s8)actor->current.roomNo; actor->stateBits=flags&0xFEFF;
            s32 reverb=gabi::call<s32>(0x02520540,room);
            gabi::call<void>(0x025E1A40,0x492A,&actor->eyePos,0,reverb);
        }
    }
    if(actor->animationFinished) { setAnm(actor,0); cutEnd(actor); }
    f32 scale=actor->scale.x; actor->scale.z=scale; actor->scale.y=scale; return 1;
}
VERIFY(0x02096988,demoProcAppear);
static u32 demoProcExit(Actor* actor) {
    WWHD_FUNC(0x02096B20,u32,actor);
    f32 ten=constant(0x10009338); s16 timer=actor->demoCounter;
    f32 one=constant(0x10009170),maximum=constant(0x10009244);
    actor->demoCounter=(s16)((u32)(s32)timer+1);
    gabi::call<void>(0x0200ED84,bytes(gabi::ea(actor)+0x1BC),ten,one,maximum);
    timer=actor->demoCounter; f32 rate=constant(0x1000924C);
    if(timer<70) {
        f32 step=constant(0x10009254);
        gabi::call<void>(0x0200EDC8,&actor->disappearScale,rate,step);
        gabi::call<void>(0x0200EDC8,&actor->disappearTarget,rate,step);
        return 1;
    }
    if(timer==70) { sound(actor,0x5905); if(getType(actor)==6) gabi::call<void>(0x025E1904,45); }
    f32 target=constant(0x1000933C);
    gabi::call<void>(0x0200ED84,&actor->disappearTarget,target,rate,one);
    gabi::call<void>(0x0200EDC8,&actor->disappearScale,rate,constant(0x10009250));
    if((s16)actor->demoCounter>=90) {
        cutEnd(actor); actor->stateBits=((u16)actor->stateBits|2)&0xFFEF;
        if(getType(actor)!=6) makeFa1S(actor);
        actor->flowerSpeed=one;
    }
    return 1;
}
VERIFY(0x02096B20,demoProcExit);
static u32 demoProcFlDmBf(Actor* actor) {
    WWHD_FUNC(0x0209709C,u32,actor);
    if(!((u16)actor->stateBits&1)) { cutEnd(actor); return 1; }
    if(actor->animationFinished) {
        setAnm(actor,2); cutEnd(actor); actor->stateBits=(u16)actor->stateBits&0xFFFE;
    }
    return 1;
}
VERIFY(0x0209709C,demoProcFlDmBf);
static u32 demoProcFlDmAf(Actor* actor) {
    WWHD_FUNC(0x02097380,u32,actor);
    if(actor->animationFinished || !((u16)actor->stateBits&1)) {
        cutEnd(actor); actor->stateBits=(u16)actor->stateBits&0xFFFE;
        u32 emitter=actor->changeEmitter;
        if(emitter) { retireEmitter(emitter); actor->changeEmitter=0; }
    } else {
        f32 low=constant(0x10009340),frame=gabi::load<f32>((u32)actor->animator+0x9C);
        if(!(frame<low)) {
            f32 high=constant(0x10009344);
            if(!(frame>high)) {
                actor->stateBits=(u16)actor->stateBits|8u;
            }
        }
    }
    return 1;
}
VERIFY(0x02097380,demoProcFlDmAf);
static u32 demoProcFlDemo(Actor* actor) {
    WWHD_FUNC(0x02096CA8,u32,actor);
    gabi::Local<cXyz> position;
    if(actor->animationFinished) {
        u16 flags=actor->stateBits;
        if(flags&1) {
            s32 animation=actor->animation;
            if(animation==0) setAnm(actor,2);
            else if(animation==2) { actor->stateBits=flags&0xFFFE; cutEnd(actor); }
            else return 1;
        } else cutEnd(actor);
    }
    if((s8)actor->animation!=2) return 1;
    f32 low=constant(0x10009340),frame=gabi::load<f32>((u32)actor->animator+0x9C);
    u8 state=actor->lightState;
    if(!(frame<low)) {
        f32 high=constant(0x10009344);
        if(!(frame>high)) actor->stateBits=(u16)actor->stateBits|8;
    }
    if(state==0) {
        f32 y=actor->handPosition.y,height=constant(0x10009168),threshold=constant(0x10009348);
        f32 x=actor->handPosition.x,z=actor->handPosition.z;
        position->y=(f32)((f64)y+(f64)height); position->z=z; position->x=x;
        if(frame<threshold) return 1;
        actor->lightState=state+1;
        u16 effect=colorEffect(actor,0x10009118);
        actor->changeEmitter=particle(effect,position.get(),&actor->shape_angle);
        gabi::call<void>(0x025E1988,0x7853);
    } else if(state==1 && !((u16)actor->stateBits&1) && actor->changeEmitter) {
        u32 emitter=actor->changeEmitter;
        actor->lightState=state+1; retireEmitter(emitter); actor->changeEmitter=0;
    }
    return 1;
}
VERIFY(0x02096CA8,demoProcFlDemo);
static u32 demoProcFlDelete(Actor* actor) {
    WWHD_FUNC(0x02096EEC,u32,actor);
    s16 count=(s16)((u32)(s32)(s16)actor->demoCounter+1);
    actor->demoCounter=count;
    if(count<27) return 1;
    if(count==27) {
        u32 p=play(); u32 player=gabi::load<u32>(p+0x5B34);
        u16 effect=colorEffect(actor,0x10009120);
        particle(effect,gabi::at<cXyz>(player+0x314));
        count=actor->demoCounter; actor->darkTarget=constant(0x10009170);
        if(count<27) return 1;
    }
    u32 emitter=actor->linkEmitter; f32 zero=constant(0x10009164);
    if(emitter) {
        u8 alpha=actor->lightState;
        if(alpha>10) {
            alpha=(u8)(alpha-10); emitter=actor->linkEmitter;
            actor->lightState=alpha; gabi::store<u8>(emitter+0x247,alpha); return 1;
        }
        emitter=actor->linkEmitter; actor->lightState=0; gabi::store<u8>(emitter+0x247,0);
        emitter=actor->linkEmitter; retireEmitter(emitter);
        actor->linkEmitter=0; actor->lightRadius=zero; return 1;
    }
    f32 power=actor->lightPower;
    if(power>zero) return 1;
    lightEnd(actor); cutEnd(actor);
    u16 flags=actor->stateBits;
    if(flags&1) {
        actor->stateBits=flags&0xFFFE;
        if(getType(actor)==6) {
            u32 p=play(); s16 count=gabi::load<s16>(p+0x5B64);
            gabi::store<u16>(p+0x5B64,(u16)((u32)(s32)count+32));
            p=play(); count=gabi::load<s16>(p+0x5B60);
            gabi::store<u16>(p+0x5B60,(u16)((u32)(s32)count+32));
        }
    }
    return 1;
}
VERIFY(0x02096EEC,demoProcFlDelete);
static u32 demoProcFlDmMd(Actor* actor) {
    WWHD_FUNC(0x02097130,u32,actor);
    gabi::Local<cXyz> position;
    u16 flags=actor->stateBits;
    f32 frame=gabi::load<f32>((u32)actor->animator+0x9C);
    if(!(flags&1)) { cutEnd(actor); return 1; }
    f32 begin=constant(0x1000934C),lightFrame=constant(0x10009340);
    if(!(frame<begin)) {
        if(frame<lightFrame) {
            f32 difference=(f32)((f64)lightFrame-(f64)frame);
            f32 rate=constant(0x10009254);
            flags=actor->stateBits; actor->darkTarget=(f32)((f64)difference*(f64)rate);
        } else { f32 zero=constant(0x10009164); flags=actor->stateBits; actor->darkTarget=zero; }
    }
    if(!(flags&0x80) && !(frame<lightFrame)) {
        u16 yaw=(u16)actor->shape_angle.y;
        f32 z=actor->handPosition.z,distance=constant(0x10009350),x=actor->handPosition.x;
        u32 table=0x104A44F8+8u*(yaw>>3);
        actor->stateBits=flags|0x80;
        f32 sine=gabi::load<f32>(table),y=actor->handPosition.y,cosine=gabi::load<f32>(table+4);
        position->y=y; position->x=gabi::fmadds(sine,distance,x); position->z=gabi::fmadds(cosine,distance,z);
        lightInit(actor,position.get());
        f32 radius=constant(0x10009328);
        s32 room=(s8)actor->current.roomNo; actor->lightRadius=radius;
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call<void>(0x025E1A40,0x6A2F,&actor->eyePos,0,reverb);
        flags=actor->stateBits;
    }
    if(flags&0x200) {
        f32 threshold=constant(0x10009354);
        if(!(frame<threshold)) {
            s32 room=(s8)actor->current.roomNo; actor->stateBits=flags&0xFDFF;
            s32 reverb=gabi::call<s32>(0x02520540,room);
            gabi::call<void>(0x025E1A40,0x492B,&actor->eyePos,0,reverb);
            flags=actor->stateBits;
        }
    }
    if(flags&0x400) {
        f32 threshold=constant(0x10009358);
        if(!(frame<threshold)) {
            s32 room=(s8)actor->current.roomNo; actor->stateBits=flags&0xFBFF;
            s32 reverb=gabi::call<s32>(0x02520540,room);
            gabi::call<void>(0x025E1A40,0x492C,&actor->eyePos,0,reverb);
        }
    }
    f32 threshold=constant(0x1000935C);
    if(!(frame<threshold)) { cutEnd(actor); actor->stateBits=(u16)actor->stateBits&0xFFFE; }
    if(!(frame<lightFrame)) {
        f32 end=constant(0x10009344);
        if(!(frame>end)) actor->stateBits=(u16)actor->stateBits|8;
    }
    return 1;
}
VERIFY(0x02097130,demoProcFlDmMd);
static void demoProc(Actor* actor) {
    WWHD_FUNC(0x020974FC,void,actor);
    s32 action=getNowEventAction(actor),staff=actor->staffID;
    if(gabi::call<BOOL>(0x025447C8,bytes(play()+0x52C4),staff)) {
        demoInitCom(actor);
        switch(action) {
        case 0:demoInitWait(actor);break;
        case 1:demoInitFa1(actor);break;
        case 2:demoInitAppear(actor);break;
        case 3:demoInitTalk(actor);break;
        case 4:demoInitExit(actor);break;
        case 5:lightProc(actor);break;
        case 6:demoInitFlLink(actor);break;
        case 7:demoInitFlDelete(actor);break;
        case 9:demoInitFlDmMd(actor);break;
        case 10:demoInitFlDmAf(actor);break;
        }
    }
    switch(action) {
    case 0:demoProcWait(actor);break;
    case 1:demoProcFa1(actor);break;
    case 2:demoProcAppear(actor);break;
    case 3:demoProcTalk(actor);break;
    case 4:demoProcExit(actor);break;
    case 5:demoProcFlDemo(actor);break;
    case 6:demoProcFlLink(actor);break;
    case 7:demoProcFlDelete(actor);break;
    case 8:demoProcFlDmBf(actor);break;
    case 9:demoProcFlDmMd(actor);break;
    case 10:demoProcFlDmAf(actor);break;
    default:cutEnd(actor);break;
    }
    demoProcCom(actor);
}
VERIFY(0x020974FC,demoProc);
static u32 ready(Actor* actor) {
    WWHD_FUNC(0x020977AC,u32,actor);
    findActor(actor->fairy);
    if(gabi::load<u16>(gabi::ea(actor)+0xF8)==2) {
        actor->state=2;
        u32 staff=gabi::call<u32>(0x02542D88,bytes(play()+0x52C4),bytes(0x10009370),0,0);
        actor->staffID=staff; actor->flowerSpeed=constant(0x10009170); demoProc(actor);
    } else gabi::call<void>(0x025D7A58,actor,(s32)(s16)actor->eventID,255,0xFFFF,0,1);
    return 1;
}
VERIFY(0x020977AC,ready);
static u32 event(Actor* actor) {
    WWHD_FUNC(0x02097868,u32,actor);
    s32 id=(s16)actor->eventID;
    if(gabi::call<BOOL>(0x025440C8,bytes(play()+0x52C4),id)) {
        u32 flag=getEventFlag(actor);
        gabi::call<void>(0x025B8B68,bytes(save()+0x644),flag);
        actor->state=3;
        u32 p=play(); u16 flags=gabi::load<u16>(p+0x52B8); gabi::store<u16>(p+0x52B8,flags|8);
        if(getType(actor)==6) {
            u32 bit=getSwbit2(actor);
            if(bit!=255) gabi::call<void>(0x025B9E38,bytes(save()+0x20),bit,(s32)(s8)actor->current.roomNo);
        }
    } else demoProc(actor);
    return 1;
}
VERIFY(0x02097868,event);
static u32 oct(Actor* actor) {
    WWHD_FUNC(0x02097930,u32,actor);
    gabi::Local<cXyz> origin,offset;
    u32 oct=findActor(actor->octID),a=gabi::ea(actor);
    if(oct) {
        u32 x=gabi::load<u32>(oct+0x314); gabi::store<u32>(a+0x314,x);
        u32 y=gabi::load<u32>(oct+0x318); f32 homeY=actor->home.pos.y;
        gabi::store<u32>(a+0x318,y); u32 z=gabi::load<u32>(oct+0x31C);
        gabi::store<u32>(a+0x37C,x); gabi::store<u32>(a+0x31C,z);
        f32 currentZ=actor->current.pos.z,currentX=actor->current.pos.x;
        actor->current.pos.y=homeY; y=gabi::load<u32>(a+0x318);
        gabi::store<f32>(a+0x390,currentX); gabi::store<u32>(a+0x380,y);
        gabi::store<f32>(a+0x398,currentZ); gabi::store<u32>(a+0x384,z); gabi::store<f32>(a+0x394,homeY);
        s32 yaw=gabi::load<s16>(oct+0x32A);
        f32 offsetX=constant(0x10009378),offsetZ=constant(0x1000937C),zero=constant(0x10009164);
        offset->z=offsetZ; origin->x=zero; origin->y=zero; origin->z=zero;
        actor->current.angle.y=(s16)yaw; offset->x=offsetX; offset->y=zero;
        u32 extra=gabi::call<u32>(0x0200F93C,origin.get(),offset.get());
        u16 flags=actor->stateBits;
        actor->shape_angle.y=(s16)((u32)yaw+extra);
        actor->current.angle.y=(s16)((u32)yaw+extra); actor->stateBits=flags|0x20;
    } else actor->stateBits=(u16)actor->stateBits&0xFFDF;
    u32 bit=getSwbit(actor);
    if(!gabi::call<BOOL>(0x025BA0C0,bytes(save()+0x20),bit,(s32)(s8)actor->current.roomNo)) return 1;
    s16 timer=actor->octTimer;
    if(timer>0) { actor->octTimer=(s16)(timer-1); return 1; }
    u32 player=gabi::load<u32>(play()+0x5B34); actor->state=1;
    u32 event=gabi::call<u32>(0x02543F10,bytes(play()+0x52C4),bytes(0x10009380),255);
    actor->eventID=(s16)event;
    gabi::call<void>(0x025D7970,actor,bytes(player),event,0,0xFFFF);
    u16 flags=actor->stateBits; actor->octTimer=30; actor->stateBits=flags|0x40;
    return 1;
}
VERIFY(0x02097930,oct);
static void octDelete(Actor* actor) {
    WWHD_FUNC(0x02097B7C,void,actor);
    gabi::Local<cXyz> offset,position;
    u32 oct=findActor(actor->octID),p=play();
    u16 flags=actor->stateBits; u32 ship=gabi::load<u32>(p+0x5B3C);
    if(!(flags&0x40)) return;
    s16 timer=actor->octTimer;
    if(timer>0) { actor->octTimer=(s16)(timer-1); return; }
    if(oct) {
        if(ship) {
            f32 zero=constant(0x10009164),distance=constant(0x10009398);
            offset->y=zero; offset->z=distance; offset->x=zero;
            gabi::call<void>(0x025DA124,bytes(oct),offset.get(),position.get());
            position->y=gabi::load<f32>(ship+0x318);
            u32 yaw=gabi::call<u32>(0x0200F93C,&actor->current.pos,position.get());
            gabi::call<void>(0x024832E0,bytes(ship),position.get(),(s32)(s16)(yaw+0x4000));
        }
        gabi::call<void>(0x025D57E0,bytes(oct)); flags=actor->stateBits;
    }
    actor->stateBits=flags&0xFFBF;
}
VERIFY(0x02097B7C,octDelete);
static BOOL waitAction(Actor* actor,void* argument) {
    WWHD_FUNC(0x02097C98,BOOL,actor,argument);
    s32 phase=actor->actionPhase;
    if(!phase) {
        u32 flag=getEventFlag(actor);
        u32 done=gabi::call<u32>(0x025B8B94,bytes(save()+0x644),flag);
        if(done) actor->state=3;
        else if(getType(actor)==6) {
            u32 bit=getSwbit(actor);
            if(gabi::call<BOOL>(0x025BA0C0,bytes(save()+0x20),bit,(s32)(s8)actor->current.roomNo)) {
                bit=getSwbit2(actor);
                if(bit!=255) gabi::call<void>(0x025B9E38,bytes(save()+0x20),bit,(s32)(s8)actor->current.roomNo);
                actor->state=3;
            } else actor->state=4;
        } else actor->state=0;
        setAnmStatus(actor); actor->actionPhase=(s8)((u8)actor->actionPhase+1); return 1;
    }
    if(phase==-1) return 1;
    s32 state=(s8)actor->state; u32 attention=0;
    switch(state) {
    case 0:attention=hunt(actor);break;
    case 1:attention=ready(actor);break;
    case 2:attention=event(actor);break;
    case 3:lookBack(actor); setAttention(actor,1);break;
    case 4:attention=octSearch(actor);break;
    case 5:attention=oct(actor);break;
    }
    if(state!=3) { lookBack(actor); setAttention(actor,attention); }
    if((u16)actor->stateBits&2) {
        u32 a=gabi::ea(actor),z=gabi::load<u32>(a+0x31C),y=gabi::load<u32>(a+0x318);
        gabi::store<u32>(a+0x398,z); gabi::store<u32>(a+0x380,y);
        u32 x=gabi::load<u32>(a+0x314);
        gabi::store<u32>(a+0x394,y); gabi::store<u32>(a+0x37C,x);
        gabi::store<u32>(a+0x384,z); gabi::store<u32>(a+0x390,x);
    }
    octDelete(actor); return 1;
}
VERIFY(0x02097C98,waitAction);
static void staticInit() {
    WWHD_FUNC(0x02097F44,void);
    gabi::store<u32>(0x104622F8,0); gabi::store<u32>(0x104622F0,0);
    gabi::store<u32>(0x104622FC,0); gabi::store<u32>(0x104622F4,0);
    gabi::call<void>(0x028F026C,bytes(0x10191338));
    gabi::store<f32>(0x104622E4,constant(0x100093A0));
    gabi::store<f32>(0x104622E8,constant(0x100093A4));
    gabi::call<void>(0x028ED6F8,bytes(0x104622EC));
    gabi::call<void>(0x028F026C,bytes(0x10191344));
    gabi::call<void>(0x028EAB2C,bytes(0x104622ED));
    gabi::call<void>(0x028F026C,bytes(0x10191350));
}
VERIFY(0x02097F44,staticInit);
static void copyMatrix(u32 source,u32 destination) {
    f32 values[12];
    for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(source+4*i);
    for(u32 i=0;i<12;++i) gabi::store<f32>(destination+4*i,values[i]);
}
static void commitJointMatrix(u32 model,u16 joint) {
    gabi::call<void>(0x028E90D4,bytes(gabi::load<u32>(0x1018C7B0)),bytes(0x104B4868));
    u32 buffer=gabi::load<u32>(model+0x2C);
    u16 flags=gabi::load<u16>(buffer+4);
    u32 current=gabi::load<u32>(0x1018C7B0);
    gabi::store<u16>(buffer+4,flags|0x10);
    u32 matrices=gabi::load<u32>(buffer+0x10);
    copyMatrix(current,matrices+48u*joint);
}
static BOOL nodeCallback(Actor* actor,void* node) {
    WWHD_FUNC(0x020945CC,BOOL,actor,node);
    gabi::Local<cXyz> offset,position;
    u32 model=gabi::load<u32>(0x104B462C);
    u32 joint=gabi::call<u32>(0x027F7878,node);
    u16 index=gabi::load<u16>(joint+4);
    u32 buffer=gabi::load<u32>(model+0x2C);
    u16 flags=gabi::load<u16>(buffer+4);
    u32 matrices=gabi::load<u32>(buffer+0x10);
    gabi::store<u16>(buffer+4,flags|0x10);
    u32 matrix=gabi::load<u32>(0x1018C7B0);
    gabi::call<void>(0x028E90D4,bytes(matrices+48u*index),bytes(matrix));
    s32 head=actor->jointController.mHeadJntNum;
    f32 zero=constant(0x10009164);
    if((u32)index==(u32)head) {
        s32 target=actor->animation==0?(s32)(s16)actor->jointController.mAngles[0][0]:0;
        gabi::call<void>(0x0200F378,&actor->headTurn,target,8,0x400,0x100);
        s32 yaw=actor->headTurn;
        gabi::call<void>(0x025F1C5C,bytes(gabi::load<u32>(0x1018C7B0)),(s32)(s16)(0u-(u32)yaw));
        offset->x=zero; offset->y=zero; offset->z=zero;
        gabi::call<void>(0x0200FCD8,offset.get(),position.get());
        f32 x=position->x,y=position->y,z=position->z;
        actor->attentionPosition.x=x; actor->attentionPosition.z=z; actor->attentionPosition.y=y;
        offset->x=constant(0x10009168); offset->z=zero; offset->y=constant(0x1000916C);
        gabi::call<void>(0x0200FCD8,offset.get(),position.get());
        z=position->z; y=position->y; x=position->x;
        u8 count=actor->attentionSetCount;
        actor->headPosition.x=x; actor->headPosition.y=y; actor->headPosition.z=z;
        if(count!=255) actor->attentionSetCount=(u8)(count+1);
    } else if((u32)index!=(u32)(s32)(s8)actor->jointController.mBackboneJntNum && (u32)index==(u32)(s32)(s8)actor->handJoint) {
        offset->x=zero; offset->y=zero; offset->z=zero;
        gabi::call<void>(0x0200FCD8,offset.get(),&actor->handPosition);
    }
    commitJointMatrix(model,index); return 1;
}
VERIFY(0x020945CC,nodeCallback);
static BOOL nodeCallbackWrapper(void* node,s32 phase) {
    WWHD_FUNC(0x0209487C,BOOL,node,phase);
    if(!phase) {
        u32 model=gabi::load<u32>(0x104B462C);
        u32 actor=gabi::load<u32>(model+0xB8);
        gabi::call<void>(0x027F7878,node);
        if(actor) nodeCallback(gabi::at<Actor>(actor),node);
    }
    return 1;
}
VERIFY(0x0209487C,nodeCallbackWrapper);
struct ResourceName { be<u32> text,vtable; };
static u32 resource(gabi::Local<ResourceName>& name,s32 id) {
    name->text=0x1000917C; name->vtable=0x1000912C;
    return gabi::call<u32>(0x026066C4,bytes(gabi::load<u32>(0x101F4F28)),name.get(),id);
}
static void assertion(u32 line,u32 condition) {
    gabi::call<void>(0x0273AA24,bytes(0x100091BC),line,bytes(condition));
}
static s8 namedJoint(u32 data,u32 name) {
    u32 table=gabi::call<u32>(0x027F68FC,bytes(data));
    u32 offset=gabi::load<u32>(table+0x10);
    u32 names=offset?table+0x10+offset:0;
    return (s8)gabi::call<u32>(0x027DF9B0,bytes(names),bytes(name));
}
static BOOL createHeap(Actor* actor) {
    WWHD_FUNC(0x020948EC,BOOL,actor);
    gabi::Local<ResourceName> name[6];
    u32 data=resource(name[0],11);
    if(!data) assertion(0x7F9,0x100091A0);
    u32 animation=resource(name[1],8);
    f32 one=constant(0x10009170);
    u32 morf=gabi::call<u32>(0x025E4F64,0,bytes(data),0,0,bytes(animation),2,0,-1,1,0,0x80000u,0x11000222u,one);
    actor->animator=morf;
    if(!morf || !gabi::load<u32>(morf+0x90)) return 0;
    animation=resource(name[2],15);
    if(!gabi::call<BOOL>(0x025E8154,&actor->brk,bytes(data),bytes(animation),1,0,0,-1,0,0,one)) return 0;
    animation=resource(name[3],19);
    if(!gabi::call<BOOL>(0x025E7CE0,&actor->btk,bytes(data),bytes(animation),1,2,0,-1,0,0,one)) return 0;
    f32 z=actor->current.pos.z;
    morf=actor->animator; f32 x=actor->current.pos.x,y=actor->current.pos.y;
    u32 model=gabi::load<u32>(morf+0x90);
    gabi::call<void>(0x028E93CC,bytes(0x1048D0CC),x,y,z);
    gabi::call<void>(0x025F1C28,bytes(0x1048D0CC),(s32)(s16)actor->current.angle.y);
    copyMatrix(0x1048D0CC,model+0xC8);
    gabi::call<void>(0x025E55A0,bytes((u32)actor->animator));
    s8 head=namedJoint(data,0x10009184); actor->jointController.mHeadJntNum=head;
    if(head<0) assertion(0x84F,0x100091CC);
    s8 backbone=namedJoint(data,0x100091E8);
    head=actor->jointController.mHeadJntNum; actor->jointController.mBackboneJntNum=backbone;
    if(head<0) { assertion(0x858,0x100091CC); backbone=actor->jointController.mBackboneJntNum; }
    if(backbone<0) assertion(0x859,0x100091F4);
    s8 hand=namedJoint(data,0x1000918C); actor->handJoint=hand;
    if(hand<0) assertion(0x861,0x100091AC);
    u16 index=0;
    u32 joints=gabi::call<u32>(0x027F3F94,bytes(data));
    while(index<gabi::load<u16>(joints+8)) {
        if((u32)index==(u32)(s32)(s8)actor->jointController.mHeadJntNum ||
           (u32)index==(u32)(s32)(s8)actor->jointController.mBackboneJntNum ||
           (u32)index==(u32)(s32)(s8)actor->handJoint) {
            u32 model=gabi::load<u32>((u32)actor->animator+0x90);
            u32 modelData=gabi::load<u32>(model+0xAC),count=gabi::load<u32>(modelData+4);
            u32 array=gabi::load<u32>(modelData+8);
            if(index<count) array+=28u*index;
            gabi::store<u32>(array+8,0x0209487C);
        }
        index=(u16)(index+1); joints=gabi::call<u32>(0x027F3F94,bytes(data));
    }
    model=gabi::load<u32>((u32)actor->animator+0x90); gabi::store<u32>(model+0xB8,gabi::ea(actor));
    u32 flowerData=resource(name[4],12);
    if(!flowerData) assertion(0x876,0x10009194);
    actor->flowerModel=gabi::call<u32>(0x025E38E0,bytes(flowerData),0x80000u,0x01000000u);
    if(!actor->flowerModel) return 0;
    animation=resource(name[5],16);
    if(!gabi::call<BOOL>(0x025E8154,&actor->flowerBrk,bytes(flowerData),bytes(animation),1,0,0,-1,0,0,one)) return 0;
    actor->colorType=0;
    u32 type=getType(actor);
    if(type>=2 && type<=6) {
        f32 frame;
        if(type<=3) frame=one;
        else if(type<6) frame=constant(0x10009174);
        else frame=constant(0x10009178);
        if(type<=3) {
            gabi::store<f32>(gabi::ea(actor)+0x3BC,frame); gabi::store<f32>(gabi::ea(actor)+0x4AC,frame); actor->colorType=1;
        } else {
            actor->colorType=type<6?2:3;
            gabi::store<f32>(gabi::ea(actor)+0x3BC,frame); gabi::store<f32>(gabi::ea(actor)+0x4AC,frame);
        }
    }
    f32 zero=constant(0x10009164);
    actor->flowerFrame=one; actor->lightRadius=zero; actor->flowerSpeed=one; return 1;
}
VERIFY(0x020948EC,createHeap);
static BOOL heapCallback(Actor* actor) { WWHD_FUNC(0x02094E4C,BOOL,actor); return createHeap(actor); }
VERIFY(0x02094E4C,heapCallback);
static void invokeAction(Actor* actor) {
    s32 adjustment=actor->actionAdjustment,dispatch=actor->actionDispatch;
    u32 receiver=gabi::ea(actor)+(u32)adjustment;
    u32 target;
    if(dispatch<0) target=actor->actionTarget;
    else {
        s32 offset=(s16)(u16)(u32)actor->actionTarget;
        u32 table=gabi::load<u32>(receiver+(u32)offset);
        target=gabi::load<u32>(table+8u*(u32)dispatch+4);
    }
    gabi::call_ptr<u32>(target,bytes(receiver),0);
}
static BOOL init(Actor* actor) {
    WWHD_FUNC(0x02095058,BOOL,actor);
    if(!actor->initialized) {
        s16 dispatch=actor->actionDispatch;
        bool current=dispatch==-1 && (s16)actor->actionAdjustment==0 && (u32)actor->actionTarget==0x02097C98;
        if(!current) {
            if(dispatch!=0) { actor->actionPhase=-1; invokeAction(actor); }
            actor->actionPhase=0; actor->actionTarget=0x02097C98;
            actor->actionDispatch=-1; actor->actionAdjustment=0;
            gabi::call_ptr<u32>((u32)actor->actionTarget,actor,0);
        }
    }
    f32 x=actor->current.pos.x,y=actor->home.pos.y,height=constant(0x10009228);
    actor->fairy=0xFFFFFFFF;
    y=(f32)((f64)y+(f64)height); f32 eyeHeight=constant(0x10009224);
    u32 a=gabi::ea(actor);
    gabi::store<f32>(a+0x390,x); actor->eyePos.x=x; actor->current.pos.y=y;
    f32 eyeY=(f32)((f64)y+(f64)eyeHeight),z=actor->current.pos.z,attentionHeight=constant(0x1000922C);
    actor->eyePos.y=eyeY;
    f32 attentionY=(f32)((f64)eyeY+(f64)attentionHeight);
    actor->headPosition.y=eyeY; actor->attentionPosition.y=eyeY;
    gabi::store<f32>(a+0x394,attentionY);
    actor->headPosition.x=x; gabi::store<f32>(a+0x398,z); actor->eyePos.z=z;
    actor->headPosition.z=z; actor->attentionPosition.x=x; actor->attentionPosition.z=z;
    if(getType(actor)!=6) {
        u32 flag=getEventFlag(actor);
        if(gabi::call<BOOL>(0x025B8B94,bytes(save()+0x644),flag)) makeFa1S(actor);
        else makeFa1(actor);
    }
    actor->stateBits=(u16)actor->stateBits|2; return 1;
}
VERIFY(0x02095058,init);
static s32 create(Actor* actor) {
    WWHD_FUNC(0x0209524C,s32,actor);
    u32 condition=actor->actor_condition;
    if(!(condition&8)) {
        if(actor) { construct(actor); condition=actor->actor_condition; }
        actor->actor_condition=condition|8;
    }
    s32 phase=gabi::call<s32>(0x02520460,&actor->phase,bytes(0x10009230));
    if(phase==4) {
        if(gabi::load<s16>(gabi::ea(actor)+8)!=369) return 5;
        actor->initialized=0;
        if(!gabi::call<BOOL>(0x025D63E8,actor,bytes(0x02094E4C),0xB7B0)) { actor->animator=0; return 5; }
        u32 model=gabi::load<u32>((u32)actor->animator+0x90);
        actor->cullMtx=model?model+0xC8:0;
        if(!init(actor)) { actor->animator=0; return 5; }
    }
    return phase;
}
VERIFY(0x0209524C,create);
static s32 profileCreate(Actor* actor) { WWHD_FUNC(0x02095370,s32,actor); return create(actor); }
VERIFY(0x02095370,profileCreate);
static BOOL execute(Actor* actor) {
    WWHD_FUNC(0x020953C4,BOOL,actor);
    gabi::call<void>(0x0259E08C,&actor->jointController,0,0,0,0,4000,9000,-2000,-4000,0x1000);
    u16 flags=actor->stateBits;
    if(!(flags&0x10)) {
        u32 done=gabi::call<u32>(0x025E535C,bytes((u32)actor->animator),&actor->eyePos,0,0);
        u32 morf=actor->animator; f32 previous=actor->previousFrame;
        actor->animationFinished=(s8)done;
        f32 frame=gabi::load<f32>(morf+0x9C);
        if(frame<previous && (s8)actor->animation!=3) {
            morf=actor->animator; actor->animationFinished=1;
        }
        frame=gabi::load<f32>(morf+0x9C); flags=actor->stateBits;
        actor->previousFrame=frame;
    }
    s32 adjustment=actor->actionAdjustment,dispatch=actor->actionDispatch;
    actor->stateBits=flags&0xFFF7;
    u32 receiver=gabi::ea(actor)+(u32)adjustment,target;
    if(dispatch<0) target=actor->actionTarget;
    else {
        s32 offset=(s16)(u16)(u32)actor->actionTarget;
        u32 table=gabi::load<u32>(receiver+(u32)offset);
        target=gabi::load<u32>(table+8u*(u32)dispatch+4);
    }
    gabi::call_ptr<u32>(target,bytes(receiver),0);
    s16 yaw=actor->current.angle.y; f32 z=actor->current.pos.z,x=actor->current.pos.x,y=actor->current.pos.y;
    u32 morf=actor->animator; s8 room=actor->current.roomNo;
    actor->shape_angle.y=yaw; gabi::store<u8>(gabi::ea(actor)+0x1C9,(u8)room);
    u32 model=gabi::load<u32>(morf+0x90);
    gabi::call<void>(0x028E93CC,bytes(0x1048D0CC),x,y,z);
    gabi::call<void>(0x025F1C28,bytes(0x1048D0CC),(s32)(s16)actor->current.angle.y);
    f32 sy=actor->scale.y,sx=actor->scale.x,sz=actor->scale.z;
    gabi::store<f32>(model+0xBC,sx); gabi::store<f32>(model+0xC0,sy); gabi::store<f32>(model+0xC4,sz);
    flags=actor->stateBits; f32 zero=constant(0x10009164);
    if(flags&0x10) {
        gabi::call<void>(0x025F24E0,zero,(f32)actor->fairyHeight,zero);
        f32 scale=actor->disappearScale,height=actor->disappearTarget;
        gabi::call<void>(0x025F2518,scale,height,scale);
        height=actor->fairyHeight;
        gabi::call<void>(0x025F24E0,zero,-(f64)height,zero);
    }
    copyMatrix(0x1048D0CC,model+0xC8);
    gabi::call<void>(0x025E55A0,bytes((u32)actor->animator));
    gabi::call<void>(0x025E742C,&actor->btk);
    f32 radius=actor->lightRadius,one=constant(0x10009170),rate=constant(0x10009240),maximum=constant(0x10009244);
    gabi::call<void>(0x0200ECD4,&actor->lightPower,radius,rate,maximum,one);
    radius=actor->lightRadius;
    if(radius>zero) {
        f32 power=actor->lightPower;
        f32 fraction=(f32)((f64)power/(f64)radius),color=constant(0x10009248);
        f32 r=(f32)((f64)color*(f64)fraction);
        f32 secondRadius=actor->lightRadius;
        fraction=(f32)((f64)power/(f64)secondRadius);
        f32 gb=(f32)((f64)color*(f64)fraction);
        actor->lightColor[0]=(u8)gabi::ftoi(r);
        actor->lightColor[1]=(u8)gabi::ftoi(gb); actor->lightColor[2]=(u8)gabi::ftoi(gb);
    }
    if(getType(actor)!=6) gabi::call<void>(0x0255FDA4,4,0,(f32)actor->flowerFrame);
    f32 speed=actor->flowerSpeed,minimum=constant(0x10009250),step=constant(0x10009254),factor=constant(0x1000924C);
    gabi::call<void>(0x0200ECD4,&actor->flowerFrame,speed,factor,minimum,step); return 1;
}
VERIFY(0x020953C4,execute);
static BOOL profileExecute(Actor* actor) { WWHD_FUNC(0x02095734,BOOL,actor); return execute(actor); }
VERIFY(0x02095734,profileExecute);
static BOOL draw(Actor* actor) {
    WWHD_FUNC(0x02095738,BOOL,actor);
    u32 morf=actor->animator,flower=actor->flowerModel,model=gabi::load<u32>(morf+0x90);
    u16 flags=actor->stateBits;
    u32 data=gabi::load<u32>(model+0xAC),flowerData=gabi::load<u32>(flower+0xAC);
    if(flags&2) return 0;
    if(!(flags&0x10)) {
        u32 environment=gabi::call<u32>(0x02555D0C);
        gabi::call<void>(0x025626A4,bytes(environment),0,&actor->current.pos,&actor->tevStr);
    }
    u32 environment=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x02562F5C,bytes(environment),bytes(model),&actor->tevStr);
    environment=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x02562F5C,bytes(environment),bytes((u32)actor->flowerModel),&actor->tevStr);
    f32 frame=gabi::load<f32>(gabi::ea(actor)+0x3BC);
    gabi::call<void>(0x025E83FC,&actor->brk,bytes(data),frame);
    frame=gabi::load<f32>(gabi::ea(actor)+0x434);
    gabi::call<void>(0x025E7FC4,&actor->btk,bytes(data),frame);
    gabi::call<void>(0x025E5580,bytes((u32)actor->animator));
    if((u16)actor->stateBits&8) {
        frame=gabi::load<f32>(gabi::ea(actor)+0x4AC);
        gabi::call<void>(0x025E83FC,&actor->flowerBrk,bytes(flowerData),frame);
        s32 hand=actor->handJoint;
        u32 buffer=gabi::load<u32>(model+0x2C),matrices=gabi::load<u32>(buffer+0x10);
        u16 state=gabi::load<u16>(buffer+4);
        flower=actor->flowerModel;
        gabi::store<u16>(buffer+4,state|0x10);
        copyMatrix(matrices+(u32)hand*48u,flower+0xC8);
        gabi::call<void>(0x025E2DE0,bytes((u32)actor->flowerModel),0);
    }
    return 1;
}
VERIFY(0x02095738,draw);
static BOOL profileDraw(Actor* actor) { WWHD_FUNC(0x020958D4,BOOL,actor); return draw(actor); }
VERIFY(0x020958D4,profileDraw);
}
