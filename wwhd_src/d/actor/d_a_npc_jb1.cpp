#include "d/actor/d_a_npc_jb1.h"
using gabi::call;
namespace {
template<class T> T read(const void* p,u32 offset) { return gabi::load<T>(gabi::ea(p)+offset); }
template<class T> void write(void* p,u32 offset,T value) { gabi::store<T>(gabi::ea(p)+offset,value); }
void* member(void* p,u32 offset) { return gabi::at<void>(gabi::ea(p)+offset); }
void* play() { return call<void*>(0x025200D4); }
}
void Jb_checkOrder(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02248128,void,actor);
    if(read<u16>(actor,0xF8)!=1) return;
    s8 order=actor->eventOrder;
    if(order==1 || order==2) { actor->eventOrder=0; actor->talking=1; }
}
VERIFY(0x02248128,Jb_checkOrder);
void Jb_eventOrder(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x0224840C,void,actor);
    s8 order=actor->eventOrder;
    if(order!=1 && order!=2) return;
    order=actor->eventOrder;
    write<u16>(actor,0xFA,read<u16>(actor,0xFA)|1);
    if(order==1) call<void>(0x025D76A8,actor);
}
VERIFY(0x0224840C,Jb_eventOrder);
s32 Jb_isEventEntry(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02248240,s32,actor);
    u32 name=read<u32>(actor,0xC9C);
    void* state=play();
    return call<s32>(0x02542D88,member(state,0x52C4),name,0,0);
}
VERIFY(0x02248240,Jb_isEventEntry);
void Jb_eventActionInit(daNpc_Jb1_c* actor,s32 staff) {
    WWHD_FUNC(0x02248280,void,actor,staff);
    void* state=play();
    void* number=call<void*>(0x0254487C,member(state,0x52C4),staff,STR(0x1001ADA4),3);
    if(number) actor->actionNumber=read<u32>(number,0);
}
VERIFY(0x02248280,Jb_eventActionInit);
BOOL Jb_eventAction(daNpc_Jb1_c* actor) { WWHD_FUNC(0x022482E0,BOOL,actor); return 1; }
VERIFY(0x022482E0,Jb_eventAction);
BOOL Jb_isDelete(daNpc_Jb1_c* actor) { WWHD_FUNC(0x02248764,BOOL,actor); return 1; }
VERIFY(0x02248764,Jb_isDelete);
BOOL Jb_charDecide(daNpc_Jb1_c* actor,s32 character) {
    WWHD_FUNC(0x022478D8,BOOL,actor,character);
    actor->characterKind=0; actor->characterIndex=0; return 1;
}
VERIFY(0x022478D8,Jb_charDecide);
s32 Jb_animationResource(daNpc_Jb1_c* actor,s32 animation) {
    WWHD_FUNC(0x0224876C,s32,actor,animation);
    return gabi::load<s32>(0x1001ADC0+u32(animation)*4);
}
VERIFY(0x0224876C,Jb_animationResource);
BOOL Jb_setAnimation(daNpc_Jb1_c* actor,JbAnimationParameters* parameters) {
    WWHD_FUNC(0x02248780,BOOL,actor,parameters);
    s8 animation=parameters->animation;
    if(s8(actor->animationNumber)==animation) return 1;
    actor->animationNumber=animation;
    s32 resource=Jb_animationResource(actor,animation);
    // The original keeps the parameter pointer across the resource lookup, then
    // reads the parameters and actor's current morph controller afterwards.
    s32 loop=parameters->loopMode;
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    f32 speed=parameters->speed, blend=parameters->morph;
    call<void>(0x0259D24C,morph,loop,blend,speed,resource,-1,STR(0x1001ADC8));
    actor->animationWrapped=0;
    actor->previousFrame=gabi::load<f32>(0x1001AD74);
    actor->animationChanged=0;
    return 1;
}
VERIFY(0x02248780,Jb_setAnimation);
BOOL Jb_selectAnimation(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02248814,BOOL,actor);
    u32 address=0x101BE9D4+u32(s32(s8(actor->animationState)))*16;
    if(gabi::load<s8>(address)>=0) Jb_setAnimation(actor,gabi::at<JbAnimationParameters>(address));
    return 1;
}
VERIFY(0x02248814,Jb_selectAnimation);
BOOL Jb_selectAttribute(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x0224885C,BOOL,actor);
    return Jb_setAnimation(actor,gabi::at<JbAnimationParameters>(0x101BE9F4+u32(u8(actor->animationAttribute))*16));
}
VERIFY(0x0224885C,Jb_selectAttribute);
void Jb_changeAttribute(daNpc_Jb1_c* actor,u8 attribute) {
    WWHD_FUNC(0x02248874,void,actor,attribute);
    if(attribute!=0 || u8(actor->animationAttribute)==0) return;
    actor->animationAttribute=0; Jb_selectAttribute(actor);
}
VERIFY(0x02248874,Jb_changeAttribute);
void Jb_animationAttribute(daNpc_Jb1_c* actor,u16 status) {
    WWHD_FUNC(0x02248894,void,actor,status);
    if(status==14) { actor->attributePhase=0; return; }
    if(status!=6) return;
    if(s8(actor->attributePhase)==0) {
        actor->animationAttribute=255;
        void* state=play();
        u8 attribute=read<u8>(state,0x5BC5);
        Jb_changeAttribute(actor,attribute);
        actor->attributePhase=u8(actor->attributePhase)+1;
    }
    void* state=play(); u8 tag=read<u8>(state,0x5BC6);
    state=play(); write<u8>(state,0x5BC6,255);
    if(tag!=255 && u8(actor->animationTag)!=tag) actor->animationTag=tag;
}
VERIFY(0x02248894,Jb_animationAttribute);
u16 Jb_nextMessage(daNpc_Jb1_c* actor,u32* message) { WWHD_FUNC(0x0224896C,u16,actor,message); return 16; }
VERIFY(0x0224896C,Jb_nextMessage);
u32 Jb_getMessage(daNpc_Jb1_c* actor) { WWHD_FUNC(0x02248974,u32,actor); return 0; }
VERIFY(0x02248974,Jb_getMessage);
BOOL Jb_checkAttention(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x0224897C,BOOL,actor);
    void* state=play(); void* attention=member(state,0x5804);
    BOOL locked=call<BOOL>(0x024EDFCC,attention);
    void* target=call<void*>(locked?0x024EC8D0:0x024EE464,attention,0);
    return actor==target;
}
VERIFY(0x0224897C,Jb_checkAttention);
BOOL Jb_setState(daNpc_Jb1_c* actor,s8 state) {
    WWHD_FUNC(0x02248A04,BOOL,actor,state);
    actor->animationState=state; return Jb_selectAnimation(actor);
}
VERIFY(0x02248A04,Jb_setState);
BOOL Jb_waitAction(daNpc_Jb1_c* actor,void* argument) {
    WWHD_FUNC(0x02248A0C,BOOL,actor,argument);
    s8 phase=actor->actionPhase;
    if(phase==0) { Jb_setState(actor,1); actor->actionPhase=u8(actor->actionPhase)+1; }
    else if(u32(s32(phase))<=3) {
        BOOL attention=Jb_checkAttention(actor);
        s8 state=actor->animationState;
        actor->hasAttention=attention;
        if(state==1) actor->waitResult=1;
    }
    return 1;
}
VERIFY(0x02248A0C,Jb_waitAction);
void Jb_HioDelete(void* object,s32 flags) {
    WWHD_FUNC(0x02248BA4,void,object,flags);
    if(object && (flags&1)) call<void>(0x0273AF40,object);
}
VERIFY(0x02248BA4,Jb_HioDelete);
void Jb_emptyVirtual(daNpc_Jb1_c* actor) { WWHD_FUNC(0x02248C60,void,actor); }
VERIFY(0x02248C60,Jb_emptyVirtual);
void Jb_privateCut(daNpc_Jb1_c* actor,s32 staff) {
    WWHD_FUNC(0x022482E8,void,actor,staff);
    if(staff==-1) return;
    void* state=play();
    s8 index=call<s32>(0x02542EDC,member(state,0x52C4),staff,gabi::at<void>(0x101BE9D0),1,1,0);
    actor->unknownD44=index;
    state=play();
    if(index==-1) { call<void>(0x02543280,member(state,0x52C4),staff); return; }
    BOOL advanced=call<BOOL>(0x025447C8,member(state,0x52C4),staff);
    if(advanced && s8(actor->unknownD44)==0) Jb_eventActionInit(actor,staff);
    if(s8(actor->unknownD44)==0 && !Jb_eventAction(actor)) return;
    state=play(); call<void>(0x02543280,member(state,0x52C4),staff);
}
VERIFY(0x022482E8,Jb_privateCut);
void Jb_eventProcess(daNpc_Jb1_c* actor,s32 staff) {
    WWHD_FUNC(0x022483BC,void,actor,staff);
    if(!call<BOOL>(0x0259F858,member(actor,0xC9C))) Jb_privateCut(actor,staff);
}
VERIFY(0x022483BC,Jb_eventProcess);
u8 Jb_demo(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02248168,u8,actor);
    if(read<u8>(actor,0x2DC)==0) {
        u8 active=actor->demoActive;
        if(active) { actor->demoActive=0; active=0; }
        return active;
    }
    u8 id=read<u8>(actor,0x2DC); actor->demoActive=1;
    if(id && id<=32) {
        void* demo=gabi::at<void>(gabi::load<u32>(0x101D5FFC));
        if(!demo) {
            call<void>(0x0273AA24,STR(0x1001ACD0),0x23A,STR(0x1001ACC0));
            demo=gabi::at<void>(gabi::load<u32>(0x101D5FFC));
        }
        call<void>(0x02526E70,demo,id);
    }
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    call<void>(0x02527028,actor,0x6A,morph,STR(0x1001AD9F),0,0,0,0);
    return actor->demoActive;
}
VERIFY(0x02248168,Jb_demo);
BOOL Jb_delete(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02248098,BOOL,actor);
    call<void>(0x025204C8,member(actor,0x7DC),STR(0x1001AD9C));
    call<void>(0x0255A374,member(actor,0xB64));
    if(read<u32>(actor,0xF4)) {
        void* morph=gabi::at<void>(read<u32>(actor,0x44C));
        if(morph) call<void>(0x025E563C,morph);
    }
    s32 users=gabi::load<s32>(0x104671FC);
    if(users>=0) {
        users=s32(u32(users)-1); gabi::store<s32>(0x104671FC,users);
        if(users<0) call<void>(0x025F0A18,gabi::load<s8>(0x104671F8));
    }
    return 1;
}
VERIFY(0x02248098,Jb_delete);
BOOL Jb_deleteEntry(daNpc_Jb1_c* actor) { WWHD_FUNC(0x02248124,BOOL,actor); return Jb_delete(actor); }
VERIFY(0x02248124,Jb_deleteEntry);
void Jb_playColorAnimation(daNpc_Jb1_c* actor,void* animation,be<s16>* frame) {
    WWHD_FUNC(0x02247A58,void,actor,animation,frame);
    if(!animation) return;
    s16 next=s16(u16(s16(*frame))+1); *frame=next;
    u32 table=read<u32>(animation,4);
    u32 target=gabi::load<u32>(table+0x14);
    s32 maximum=call<s32>(target,animation);
    if(next>=maximum) *frame=0;
}
VERIFY(0x02247A58,Jb_playColorAnimation);
void Jb_destroy(daNpc_Jb1_c* actor,s32 flags) {
    WWHD_FUNC(0x02248BB8,void,actor,flags);
    if(!actor) return;
    call<void>(0x027F3628,member(actor,0x9E8),0);
    call<void>(0x02515A70,member(actor,0x690),2);
    call<void>(0x02515860,member(actor,0x654),2);
    call<void>(0x02018034,member(actor,0x628),2);
    write<u32>(actor,0x470,0x1001AC90); write<u32>(actor,0x464,0x1001ACA0);
    call<void>(0x024EFD9C,member(actor,0x450),0);
    call<void>(0x025D50BC,actor,0);
    if(flags&1) call<void>(0x0273AF40,actor);
}
VERIFY(0x02248BB8,Jb_destroy);
struct JbActionDescriptor { be<s16> adjustment, selector; be<u32> target; };
WWHD_SIZE(JbActionDescriptor,8);
namespace {
void dispatchAction(daNpc_Jb1_c* actor,void* argument) {
    s16 selector=read<s16>(actor,0xC8E);
    s16 adjustment=read<s16>(actor,0xC8C);
    void* self=member(actor,u32(s32(adjustment)));
    u32 target;
    if(selector<0) target=read<u32>(actor,0xC90);
    else {
        s16 vtableOffset=read<s16>(actor,0xC92);
        u32 table=read<u32>(self,u32(s32(vtableOffset)));
        target=gabi::load<u32>(table+u32(s32(selector))*8+4);
    }
    call<void>(target,self,argument);
}
}
BOOL Jb_setAction(daNpc_Jb1_c* actor,JbActionDescriptor* action,void* argument) {
    WWHD_FUNC(0x022478EC,BOOL,actor,action,argument);
    s16 oldSelector=read<s16>(actor,0xC8E), selector=action->selector;
    s16 adjustment=action->adjustment; u32 target=action->target;
    bool unchanged=oldSelector==selector && (oldSelector==0 ||
        (read<s16>(actor,0xC8C)==adjustment && read<u32>(actor,0xC90)==target));
    if(unchanged) return 1;
    if(oldSelector!=0) { actor->actionPhase=9; dispatchAction(actor,argument); }
    write<u32>(actor,0xC90,target); write<s16>(actor,0xC8C,adjustment);
    write<s16>(actor,0xC8E,selector); actor->actionPhase=0;
    dispatchAction(actor,argument); return 1;
}
VERIFY(0x022478EC,Jb_setAction);
BOOL Jb_initCharacter(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02247A18,BOOL,actor);
    gabi::Local<JbActionDescriptor> action;
    action->adjustment=gabi::load<s16>(0x1001AC38);
    action->selector=gabi::load<s16>(0x1001AC3A);
    action->target=gabi::load<u32>(0x1001AC3C);
    Jb_setAction(actor,action.get(),nullptr); return 1;
}
VERIFY(0x02247A18,Jb_initCharacter);
BOOL Jb_createInit(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02247E84,BOOL,actor);
    f32 zero=gabi::load<f32>(0x1001AD74);
    write<f32>(actor,0x374,zero);
    call<void>(0x0259F814,member(actor,0xC9C),STR(0x1001AD88),actor);
    s8 character=actor->characterKind; actor->animationNumber=2;
    if(character!=0 || !Jb_initCharacter(actor)) return 0;
    u16 x=read<u16>(actor,0x320), z=read<u16>(actor,0x324), y=read<u16>(actor,0x322);
    write<u16>(actor,0x32C,z); write<u16>(actor,0x32A,y); write<u16>(actor,0x328,x);
    call<void>(0x025564B4,member(actor,0xB64));
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    call<void>(0x025E4A54,morph,zero);
    call<void>(0x02247B7C,actor); return 1;
}
VERIFY(0x02247E84,Jb_createInit);
BOOL Jb_execute(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02248444,BOOL,actor);
    if(read<u8>(actor,0xD3B)==0) {
        u16 y=read<u16>(actor,0x322); u32 z=read<u32>(actor,0x31C);
        write<u16>(actor,0xD1A,y);
        u16 x=read<u16>(actor,0x320); u32 py=read<u32>(actor,0x318);
        write<u32>(actor,0xD14,z);
        u16 rz=read<u16>(actor,0x324); u32 px=read<u32>(actor,0x314);
        write<u16>(actor,0xD1C,rz); write<u32>(actor,0xD0C,px);
        write<u32>(actor,0xD10,py); write<u8>(actor,0xD3B,1); write<u16>(actor,0xD18,x);
    }
    if(read<u8>(actor,0xD38) && read<u8>(actor,0x2DC)==0) return 1;
    write<u8>(actor,0xD38,0); Jb_checkOrder(actor);
    if(!Jb_demo(actor)) {
        void* state=play(); s32 staff=-1;
        if(read<u8>(state,0x5292) && read<u16>(actor,0xF8)!=1) staff=Jb_isEventEntry(actor);
        if(staff>=0) Jb_eventProcess(actor,staff); else dispatchAction(actor,nullptr);
        call<void>(0x025D6870,actor,member(actor,0x654));
        if(read<u8>(actor,0xD39)==0) {
            u16 z=read<u16>(actor,0x324), y=read<u16>(actor,0x322);
            write<u16>(actor,0x32C,z); u16 x=read<u16>(actor,0x320);
            write<u16>(actor,0x32A,y); write<u16>(actor,0x328,x);
        }
    }
    Jb_eventOrder(actor); call<void>(0x02247B7C,actor); return 1;
}
VERIFY(0x02248444,Jb_execute);
BOOL Jb_executeEntry(daNpc_Jb1_c* actor) { WWHD_FUNC(0x022485C0,BOOL,actor); return Jb_execute(actor); }
VERIFY(0x022485C0,Jb_executeEntry);
BOOL Jb_draw(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x022485C4,BOOL,actor);
    void* lightModel=gabi::at<void>(read<u32>(actor,0xC08));
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    u8 hidden=read<u8>(actor,0xD38);
    void* model=gabi::at<void>(read<u32>(morph,0x90));
    void* lightData=gabi::at<void>(read<u32>(lightModel,0xAC));
    void* modelData=gabi::at<void>(read<u32>(model,0xAC));
    if(hidden || read<u8>(actor,0xD3A)) return 1;
    void* environment=call<void*>(0x02555D0C);
    call<void>(0x025626A4,environment,0,member(actor,0x314),member(actor,0x110));
    environment=call<void*>(0x02555D0C);
    call<void>(0x02562F5C,environment,model,member(actor,0x110));
    call<void>(0x025E83FC,member(actor,0xB8C),modelData,f32(read<s16>(actor,0xC04)));
    morph=gabi::at<void>(read<u32>(actor,0x44C)); call<void>(0x025E5590,morph);
    write<u32>(modelData,0x48,0);
    call<void>(0x025E83FC,member(actor,0xC10),lightData,f32(read<s16>(actor,0xC88)));
    lightModel=gabi::at<void>(read<u32>(actor,0xC08)); call<void>(0x025E2E5C,lightModel);
    write<u32>(lightData,0x48,0);
    if(gabi::load<u8>(0x10467204)) {
        if(gabi::load<u32>(0x101FDB64)==0) {
            gabi::store<u32>(0x101FDB64,1); call<void>(0xC000A848,gabi::at<void>(0x101FEC00),gabi::at<void>(0x1001AC40),4);
        }
        if(gabi::load<u32>(0x101FDA50)==0) {
            gabi::store<u32>(0x101FDA50,1); call<void>(0xC000A848,gabi::at<void>(0x101FEBF4),gabi::at<void>(0x1001AC44),4);
        }
        if(gabi::load<u32>(0x101FDAC0)==0) {
            gabi::store<u32>(0x101FDAC0,1); call<void>(0xC000A848,gabi::at<void>(0x101FEBF8),gabi::at<void>(0x1001AC48),4);
        }
    }
    return 1;
}
VERIFY(0x022485C4,Jb_draw);
BOOL Jb_drawEntry(daNpc_Jb1_c* actor) { WWHD_FUNC(0x02248760,BOOL,actor); return Jb_draw(actor); }
VERIFY(0x02248760,Jb_drawEntry);

daNpc_Jb1_c* Jb_construct(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x0224769C,daNpc_Jb1_c*,actor);
    if(!actor) { actor=call<daNpc_Jb1_c*>(0x0273AD10,0xD50); if(!actor) return nullptr; }
    call<void>(0x025A1458,actor);
    write<u32>(actor,0xB4,0x1001ADE8);
    // The HD NPC base default record initializes vector, angle, animation and
    // gaze-controller fields before constructing the embedded controllers.
    f32 default00=gabi::load<f32>(0x1016E414+0x0);
    write<f32>(actor,0x7f4,default00);
    f32 default04=gabi::load<f32>(0x1016E414+0x4);
    write<f32>(actor,0x7f8,default04);
    f32 default08=gabi::load<f32>(0x1016E414+0x8);
    write<f32>(actor,0x7fc,default08);
    f32 default0C=gabi::load<f32>(0x1016E414+0xC);
    write<f32>(actor,0x800,default0C);
    f32 default10=gabi::load<f32>(0x1016E414+0x10);
    write<f32>(actor,0x804,default10);
    f32 default14=gabi::load<f32>(0x1016E414+0x14);
    write<f32>(actor,0x808,default14);
    u8 default18=gabi::load<u8>(0x1016E414+0x18);
    write<u8>(actor,0x80c,default18);
    u8 default19=gabi::load<u8>(0x1016E414+0x19);
    write<u8>(actor,0x80d,default19);
    u8 default1A=gabi::load<u8>(0x1016E414+0x1A);
    write<u8>(actor,0x80e,default1A);
    u8 default1B=gabi::load<u8>(0x1016E414+0x1B);
    write<u8>(actor,0x80f,default1B);
    s16 default1C=gabi::load<s16>(0x1016E414+0x1C);
    write<s16>(actor,0x810,default1C);
    s16 default1E=gabi::load<s16>(0x1016E414+0x1E);
    write<s16>(actor,0x812,default1E);
    s16 default20=gabi::load<s16>(0x1016E414+0x20);
    write<s16>(actor,0x814,default20);
    s16 default22=gabi::load<s16>(0x1016E414+0x22);
    write<s16>(actor,0x816,default22);
    f32 default24=gabi::load<f32>(0x1016E414+0x24);
    write<f32>(actor,0x818,default24);
    f32 default28=gabi::load<f32>(0x1016E414+0x28);
    write<f32>(actor,0x81c,default28);
    f32 default2C=gabi::load<f32>(0x1016E414+0x2C);
    write<f32>(actor,0x820,default2C);
    f32 default30=gabi::load<f32>(0x1016E414+0x30);
    write<f32>(actor,0x824,default30);
    f32 default34=gabi::load<f32>(0x1016E414+0x34);
    write<f32>(actor,0x828,default34);
    f32 default38=gabi::load<f32>(0x1016E414+0x38);
    write<f32>(actor,0x82c,default38);
    f32 default3C=gabi::load<f32>(0x1016E414+0x3C);
    write<f32>(actor,0x830,default3C);
    f32 default40=gabi::load<f32>(0x1016E414+0x40);
    write<s16>(actor,0x956,default1E);
    write<f32>(actor,0x8f4,default40);
    write<f32>(actor,0x964,default2C);
    write<f32>(actor,0x834,default40);
    write<f32>(actor,0x8b8,default04);
    write<u8>(actor,0x8ce,default1A);
    write<s16>(actor,0x8d0,default1C);
    write<f32>(actor,0x8f0,default3C);
    write<s16>(actor,0x958,default20);
    write<f32>(actor,0x944,default0C);
    write<f32>(actor,0x960,default28);
    write<u8>(actor,0x8cc,default18);
    write<f32>(actor,0x8dc,default28);
    write<f32>(actor,0x8c8,default14);
    write<f32>(actor,0x8e0,default2C);
    write<f32>(actor,0x968,default30);
    write<f32>(actor,0x978,default40);
    write<f32>(actor,0x8d8,default24);
    write<s16>(actor,0x954,default1C);
    write<f32>(actor,0x8c4,default10);
    write<f32>(actor,0x8c0,default0C);
    write<s16>(actor,0x95a,default22);
    write<f32>(actor,0x8bc,default08);
    write<f32>(actor,0x948,default10);
    write<f32>(actor,0x96c,default34);
    write<f32>(actor,0x95c,default24);
    write<u8>(actor,0x953,default1B);
    write<s16>(actor,0x8d4,default20);
    write<f32>(actor,0x8e8,default34);
    write<u8>(actor,0x951,default19);
    write<s16>(actor,0x8d6,default22);
    write<f32>(actor,0x94c,default14);
    write<f32>(actor,0x970,default38);
    write<s16>(actor,0x8d2,default1E);
    write<u8>(actor,0x952,default1A);
    write<f32>(actor,0x93c,default04);
    write<f32>(actor,0x8b4,default00);
    write<f32>(actor,0x938,default00);
    write<u8>(actor,0x8cf,default1B);
    write<u8>(actor,0x950,default18);
    write<f32>(actor,0x974,default3C);
    write<f32>(actor,0x8e4,default30);
    write<u8>(actor,0x8cd,default19);
    write<f32>(actor,0x940,default08);
    write<f32>(actor,0x8ec,default38);
    call<void>(0x027F2BC0,member(actor,0x9D8),0);
    write<u32>(actor,0x9E8,0x1016E54C);
    call<void>(0x027DA984,member(actor,0x9EC));
    write<u32>(actor,0xA20,0x1016D820);
    for(u32 offset : {0xA30u,0xA54u,0xA58u,0xA60u,0xA5Cu}) write<u32>(actor,offset,0);
    write<u32>(actor,0x9E8,0x1001AC68);
    call<void>(0x025E7C6C,member(actor,0xA6C));
    call<void>(0x025E80D0,member(actor,0xAE8));
    write<f32>(actor,0xB84,gabi::load<f32>(0x1001ACDC));
    call<void>(0x025E80D0,member(actor,0xB8C));
    call<void>(0x025E80D0,member(actor,0xC10));
    call<void>(0x0259F740,member(actor,0xC9C));
    return actor;
}
VERIFY(0x0224769C,Jb_construct);
void* Jb_hioConstruct(void* object) {
    WWHD_FUNC(0x02248A98,void*,object);
    if(!object) { object=call<void*>(0x0273AD10,0x20); if(!object) return nullptr; }
    write<u32>(object,0x1C,0x1001ACB0);
    call<void>(0xC000A848,member(object,8),gabi::at<void>(0x101BEA04),0x14);
    write<u8>(object,0,255); write<s32>(object,4,-1); return object;
}
VERIFY(0x02248A98,Jb_hioConstruct);
void Jb_staticInit() {
    WWHD_FUNC(0x02248B04,void);
    gabi::store<u32>(0x10467220,0); gabi::store<u32>(0x10467218,0);
    gabi::store<u32>(0x10467224,0); gabi::store<u32>(0x1046721C,0);
    call<void>(0x028F026C,gabi::at<void>(0x101BEA18));
    f32 minimum=gabi::load<f32>(0x1001ADE0), maximum=gabi::load<f32>(0x1001ADE4);
    gabi::store<f32>(0x104671EC,minimum); gabi::store<f32>(0x104671F0,maximum);
    call<void>(0x028ED6F8,gabi::at<void>(0x104671F4));
    call<void>(0x028F026C,gabi::at<void>(0x101BEA24));
    call<void>(0x028EAB2C,gabi::at<void>(0x104671F5));
    call<void>(0x028F026C,gabi::at<void>(0x101BEA30));
    Jb_hioConstruct(gabi::at<void>(0x104671F8));
}
VERIFY(0x02248B04,Jb_staticInit);
struct JbVector { be<f32> x,y,z; };
void Jb_setAttention(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02247AC4,void,actor);
    gabi::Local<JbVector> offset;
    f32 x=read<f32>(actor,0x314);
    f32 zero=gabi::load<f32>(0x1001AD74);
    f32 y=read<f32>(actor,0x318), height=gabi::load<f32>(0x1001AD78);
    offset->x=zero; offset->y=height; offset->z=gabi::load<f32>(0x1001AD7C);
    f32 z=read<f32>(actor,0x31C);
    void* matrix=gabi::at<void>(0x1048D0CC);
    call<void>(0x028E93CC,matrix,x,y,z);
    s16 angle=read<s16>(actor,0x322); call<void>(0x025F1C28,matrix,angle);
    call<void>(0x028E8F64,matrix,offset.get(),member(actor,0xD20));
    y=read<f32>(actor,0xD24); height=gabi::load<f32>(0x10467200);
    write<f32>(actor,0x380,y); f32 raised=y+height;
    x=read<f32>(actor,0xD20); z=read<f32>(actor,0xD28);
    write<f32>(actor,0x394,raised); write<f32>(actor,0x37C,x);
    write<f32>(actor,0x398,z); write<f32>(actor,0x390,x); write<f32>(actor,0x384,z);
}
VERIFY(0x02247AC4,Jb_setAttention);
BOOL Jb_createHeap(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x022475E0,BOOL,actor);
    if(!call<void*>(0x02247260,actor)) return 0;
    if(!call<BOOL>(0x022474D8,actor)) return 0;
    f32 zero=gabi::load<f32>(0x1001AD74);
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    void* model=gabi::at<void>(read<u32>(morph,0x90));
    write<u32>(model,0xB8,0);
    call<void>(0x024EFF44,member(actor,0x614),zero,zero);
    call<void>(0x024F06B4,member(actor,0x450),member(actor,0x314),member(actor,0x300),actor,1,member(actor,0x614),member(actor,0x33C),0,0);
    return 1;
}
VERIFY(0x022475E0,Jb_createHeap);
BOOL Jb_checkCreateHeap(daNpc_Jb1_c* actor) { WWHD_FUNC(0x02247698,BOOL,actor); return Jb_createHeap(actor); }
VERIFY(0x02247698,Jb_checkCreateHeap);
s32 Jb_create(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02247F64,s32,actor);
    u32 flags=read<u32>(actor,0x2E4);
    if(!(flags&8)) {
        if(actor) { Jb_construct(actor); flags=read<u32>(actor,0x2E4); }
        write<u32>(actor,0x2E4,flags|8);
    }
    s32 phase=call<s32>(0x02520460,member(actor,0x7DC),STR(0x1001AD8C));
    if(phase!=4) return phase;
    if(!Jb_charDecide(actor,read<u8>(actor,0xB3))) return 5;
    s32 users=gabi::load<s32>(0x104671FC);
    if(users<0) {
        s32 child=call<s32>(0x025F0A10,STR(0x1001AD90),gabi::at<void>(0x104671F8));
        users=gabi::load<s32>(0x104671FC); gabi::store<u8>(0x104671F8,u8(child));
    }
    u32 size=gabi::load<u32>(0x101BE9CC);
    gabi::store<u32>(0x104671FC,u32(users)+1);
    if(!call<BOOL>(0x025D63E8,actor,0x02247698,size)) return 5;
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    u32 model=read<u32>(morph,0x90);
    write<u32>(actor,0x348,model?model+0xC8:0);
    return Jb_createInit(actor)?phase:5;
}
VERIFY(0x02247F64,Jb_create);
s32 Jb_createEntry(daNpc_Jb1_c* actor) { WWHD_FUNC(0x02248094,s32,actor); return Jb_create(actor); }
VERIFY(0x02248094,Jb_createEntry);
struct JbSafeString { be<u32> text,vtable; };
namespace {
void* resource(JbSafeString* archive,s32 id) {
    void* control=gabi::at<void>(gabi::load<u32>(0x101F4F28));
    return call<void*>(0x026067F4,control,archive,id);
}
void assertion(u32 file,s32 line,u32 expression) { call<void>(0x0273AA24,STR(file),line,STR(expression)); }
}
void* Jb_createAnimation(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02247260,void*,actor);
    gabi::Local<JbSafeString> archive1,archive2,archive3;
    archive1->vtable=0x1001AC50; archive1->text=0x1001ACE8;
    void* data=resource(archive1.get(),2);
    if(!data) assertion(0x1001ACEC,0x469,0x1001ACFC);
    archive2->vtable=0x1001AC50; archive2->text=0x1001ACE8;
    void* animation=resource(archive2.get(),1);
    f32 speed=gabi::load<f32>(0x1001ACDC);
    void* morph=call<void*>(0x025E4F64,nullptr,data,nullptr,nullptr,animation,2,speed,0,-1,1,nullptr,0x80000,0x11020022);
    write<u32>(actor,0x44C,gabi::ea(morph));
    if(!morph) return nullptr;
    if(read<u32>(morph,0x90)==0) {
        u32 table=read<u32>(morph,0); u32 destroy=gabi::load<u32>(table+12);
        call<void>(destroy,morph,3); write<u32>(actor,0x44C,0); return nullptr;
    }
    archive3->vtable=0x1001AC50; archive3->text=0x1001ACE8;
    animation=resource(archive3.get(),3); write<u32>(actor,0xB88,gabi::ea(animation));
    if(!animation) { assertion(0x1001ACEC,0x47E,0x1001AD10); animation=gabi::at<void>(read<u32>(actor,0xB88)); }
    morph=gabi::at<void>(read<u32>(actor,0x44C));
    void* model=gabi::at<void>(read<u32>(morph,0x90));
    void* modelData=gabi::at<void>(read<u32>(model,0xAC));
    BOOL initialized=call<s32>(0x025E8154,member(actor,0xB8C),modelData,animation,1,2,speed,0,-1,0)!=0;
    if(!initialized) { write<u32>(actor,0x44C,0); return nullptr; }
    morph=gabi::at<void>(read<u32>(actor,0x44C)); write<u16>(actor,0xC04,0);
    model=gabi::at<void>(read<u32>(morph,0x90)); modelData=gabi::at<void>(read<u32>(model,0xAC));
    void* names=call<void*>(0x027F68FC,modelData);
    u32 offset=read<u32>(names,16); u32 nameTable=offset?gabi::ea(names)+16+offset:0;
    s8 joint=call<s32>(0x027DF9B0,gabi::at<void>(nameTable),STR(0x1001ACE0));
    write<s8>(actor,0x7E4,joint);
    if(joint<0) assertion(0x1001ACEC,0x48D,0x1001AD24);
    return data;
}
VERIFY(0x02247260,Jb_createAnimation);
BOOL Jb_createLight(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x022474D8,BOOL,actor);
    gabi::Local<JbSafeString> archive1,archive2;
    archive1->vtable=0x1001AC50; archive1->text=0x1001AD38;
    void* data=resource(archive1.get(),9);
    void* model=call<void*>(0x025E38E0,data,0,0x11020203);
    write<u32>(actor,0xC08,gabi::ea(model));
    if(!model) assertion(0x1001AD3C,0x4D8,0x1001AD4C);
    archive2->vtable=0x1001AC50; archive2->text=0x1001AD38;
    void* animation=resource(archive2.get(),8); write<u32>(actor,0xC0C,gabi::ea(animation));
    if(!animation) { assertion(0x1001AD3C,0x4DD,0x1001AD60); animation=gabi::at<void>(read<u32>(actor,0xC0C)); }
    model=gabi::at<void>(read<u32>(actor,0xC08)); data=gabi::at<void>(read<u32>(model,0xAC));
    f32 speed=gabi::load<f32>(0x1001ACDC);
    BOOL initialized=call<s32>(0x025E8154,member(actor,0xC10),data,animation,1,2,speed,0,-1,0)!=0;
    if(initialized) write<u16>(actor,0xC88,0);
    return initialized;
}
VERIFY(0x022474D8,Jb_createLight);
namespace {
// Load all twelve matrix components before any store, preserving overlap.
void copyBaseMatrix(void* model,void* source) {
    f32 values[12];
    for(u32 i=0;i<12;++i) values[i]=read<f32>(source,4*i);
    for(u32 i=0;i<12;++i) write<f32>(model,0xC8+4*i,values[i]);
}
void* jointMatrix(daNpc_Jb1_c* actor) {
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    s8 index=read<s8>(actor,0x7E4);
    void* model=gabi::at<void>(read<u32>(morph,0x90));
    void* buffer=gabi::at<void>(read<u32>(model,0x2C));
    u32 matrices=read<u32>(buffer,0x10);
    u16 flags=read<u16>(buffer,4); write<u16>(buffer,4,flags|0x10);
    return gabi::at<void>(matrices+u32(s32(index))*48);
}
}
void Jb_setMatrix(daNpc_Jb1_c* actor) {
    WWHD_FUNC(0x02247B7C,void,actor);
    void* animation=gabi::at<void>(read<u32>(actor,0xB88));
    f32 zero=gabi::load<f32>(0x1001AD74), offsetY=gabi::load<f32>(0x1001AD80);
    gabi::Local<JbVector> offset;
    offset->z=zero; offset->y=offsetY; offset->x=zero;
    Jb_playColorAnimation(actor,animation,gabi::at<be<s16>>(gabi::ea(actor)+0xC04));
    animation=gabi::at<void>(read<u32>(actor,0xC0C));
    Jb_playColorAnimation(actor,animation,gabi::at<be<s16>>(gabi::ea(actor)+0xC88));
    if(u8(actor->demoActive)==0) {
        void* morph=gabi::at<void>(read<u32>(actor,0x44C));
        u8 wrapped=call<s32>(0x025E535C,morph,member(actor,0x37C),0,0);
        morph=gabi::at<void>(read<u32>(actor,0x44C)); f32 old=actor->previousFrame;
        actor->animationWrapped=wrapped;
        f32 frame=read<f32>(morph,0x9C);
        if(frame<old) { morph=gabi::at<void>(read<u32>(actor,0x44C)); actor->animationWrapped=1; }
        actor->previousFrame=read<f32>(morph,0x9C);
        void* state=play(); call<void>(0x024F08A8,member(actor,0x450),member(state,0x12A0));
    }
    void* state=play();
    u8 room=call<s32>(0x024EF130,member(state,0x12A0),member(actor,0x538)); write<u8>(actor,0x1C9,room);
    state=play();
    u8 color=call<s32>(0x024EEEB8,member(state,0x12A0),member(actor,0x538)); write<u8>(actor,0x1CA,color);
    void* matrix=gabi::at<void>(0x1048D0CC);
    f32 y=read<f32>(actor,0x318), x=read<f32>(actor,0x314), z=read<f32>(actor,0x31C);
    call<void>(0x028E93CC,matrix,x,y,z);
    s16 angle=read<s16>(actor,0x322); call<void>(0x025F1C28,matrix,angle);
    void* morph=gabi::at<void>(read<u32>(actor,0x44C));
    void* model=gabi::at<void>(read<u32>(morph,0x90));
    copyBaseMatrix(model,matrix);
    morph=gabi::at<void>(read<u32>(actor,0x44C)); call<void>(0x025E55A0,morph);
    void* joint=jointMatrix(actor); call<void>(0x028E90D4,joint,matrix);
    call<void>(0x028E8F64,matrix,offset.get(),member(actor,0x7E8));
    joint=jointMatrix(actor); call<void>(0x028E90D4,joint,matrix);
    call<void>(0x025F24E0,zero,offsetY,zero);
    f32 scale=gabi::load<f32>(0x1001AD84); call<void>(0x025F2518,scale,scale,scale);
    // The second matrix snapshot precedes the light-model pointer read.
    f32 values[12]; for(u32 i=0;i<12;++i) values[i]=read<f32>(matrix,i*4);
    model=gabi::at<void>(read<u32>(actor,0xC08));
    for(u32 i=0;i<12;++i) write<f32>(model,0xC8+i*4,values[i]);
    model=gabi::at<void>(read<u32>(actor,0xC08)); call<void>(0x027F4D5C,model);
    u32 py=read<u32>(actor,0x7EC),pz=read<u32>(actor,0x7F0),px=read<u32>(actor,0x7E8);
    write<u32>(actor,0xB6C,pz); write<u32>(actor,0xB68,py); write<u32>(actor,0xB64,px);
    write<s16>(actor,0xB70,gabi::load<s16>(0x10467206));
    write<s16>(actor,0xB72,gabi::load<s16>(0x10467208));
    write<s16>(actor,0xB74,gabi::load<s16>(0x1046720A));
    write<f32>(actor,0xB78,gabi::load<f32>(0x1046720C));
    write<f32>(actor,0xB7C,gabi::load<f32>(0x10467210));
    Jb_setAttention(actor);
}
VERIFY(0x02247B7C,Jb_setMatrix);
