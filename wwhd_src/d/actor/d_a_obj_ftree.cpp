#include "d/actor/d_a_obj_ftree.h"
#include "bindings.h"
using daObjFtree::Act_c;
template<class T> static T read(u32 a) { return *gabi::at<be<T>>(a); }
template<class T> static void write(u32 a,T v) { *gabi::at<be<T>>(a)=v; }
static u32 offset(Act_c* a,u32 n) { return gabi::ea(a)+n; }
// The recompiled fmul selects the first NaN operand's payload before rounding.
static f32 scaledComponent(f32 component,f32 scale) {
    if(std::isnan(component)) return component;
    if(std::isnan(scale)) return scale;
    return f32(component*scale);
}
static void initTreeMap() {
    if(!read<u32>(0x101FDBD0)) { write<u32>(0x101FDBD0,1); write<u8>(0x101FEC14,10); }
}
static u32 eventSave() { return read<u32>(0x101F84DC)+0x644; }
static u32 actionTarget(u32 record,u32& actor) {
    s16 adjustment=read<s16>(record), slot=read<s16>(record+2);
    actor+=adjustment;
    if(slot<0) return read<u32>(record+4);
    u32 table=read<u32>(actor+read<s16>(record+6));
    return read<u32>(table+u32(slot)*8+4);
}

void ftree_eventInit(Act_c* actor) {
    WWHD_FUNC(0x023469BC,void,actor);
    write<s16>(offset(actor,0x4CE),-1); write<s16>(offset(actor,0x4D0),0);
}
VERIFY(0x023469BC,ftree_eventInit);

u32 ftree_checkEvent(Act_c* actor,s32 index) {
    WWHD_FUNC(0x023469D0,u32,actor,index);
    u32 play=gabi::call<u32>(0x025200D4);
    return read<u8>(play+u32(index)+0x5BBB)==0x59;
}
VERIFY(0x023469D0,ftree_checkEvent);

u32 ftree_eventSet(Act_c* actor,s32 event) {
    WWHD_FUNC(0x02346A10,u32,actor,event);
    if(read<s16>(offset(actor,0x4D0))!=0 || event==-1) return 0;
    write<s16>(offset(actor,0x4CE),s16(event)); write<s16>(offset(actor,0x4D0),1); return 1;
}
VERIFY(0x02346A10,ftree_eventSet);

u32 ftree_isBroughtId(s32 id) {
    WWHD_FUNC(0x023464B4,u32,id);
    s32 bits=gabi::call<s32>(0x025B8BB0,gabi::at<void>(eventSave()),0x9EFF);
    if((u32(bits)>>(u32(id)&7))&1) return 1;
    return gabi::call<s32>(0x025B8B94,gabi::at<void>(eventSave()),0x102)!=0;
}
VERIFY(0x023464B4,ftree_isBroughtId);

u32 ftree_isBrought(Act_c* actor) {
    WWHD_FUNC(0x02346534,u32,actor);
    initTreeMap(); s32 tree=gabi::call<s32>(0x02349DFC,actor,4,0);
    u32 id=tree<10?read<u8>(0x10029154+u32(tree)):15;
    return gabi::call<u32>(0x023464B4,id);
}
VERIFY(0x02346534,ftree_isBrought);

void ftree_getInfo(Act_c* actor,daObjFtree::SearchInfo* info) {
    WWHD_FUNC(0x02346B38,void,actor,info);
    info->brought=0; info->total=8;
    for(u32 id=0;id<8;++id) if(gabi::call<s32>(0x023464B4,id)) info->brought=u32(info->brought)+1;
}
VERIFY(0x02346B38,ftree_getInfo);

u32 ftree_isLast(Act_c* actor) {
    WWHD_FUNC(0x02346BA8,u32,actor);
    gabi::Local<daObjFtree::SearchInfo> info;
    gabi::Local<be<u32>[8]> infoLinkage;
    gabi::call<void>(0x02346B38,actor,info.get());
    if(gabi::call<s32>(0x02346534,actor)) return 0;
    return s32(u32(info->brought))>=s32(u32(info->total)-1);
}
VERIFY(0x02346BA8,ftree_isLast);

void ftree_unsetId(Act_c* actor,s32 id) {
    WWHD_FUNC(0x0234829C,void,actor,id);
    u32 bits=gabi::call<u32>(0x025B8BB0,gabi::at<void>(eventSave()),0x9EFF);
    gabi::call<void>(0x025B8AF4,gabi::at<void>(eventSave()),0x9EFF,bits&~(1u<<(u32(id)&7)));
}
VERIFY(0x0234829C,ftree_unsetId);

void ftree_setId(Act_c* actor,s32 id) {
    WWHD_FUNC(0x02347128,void,actor,id);
    u32 mask=1u<<(u32(id)&7);
    u32 bits=gabi::call<u32>(0x025B8BB0,gabi::at<void>(eventSave()),0x9EFF);
    gabi::call<void>(0x025B8AF4,gabi::at<void>(eventSave()),0x9EFF,u8(bits|mask));
    bits=gabi::call<u32>(0x025B8BB0,gabi::at<void>(eventSave()),0x9AFF);
    gabi::call<void>(0x025B8AF4,gabi::at<void>(eventSave()),0x9AFF,u8(bits|mask));
    gabi::Local<daObjFtree::SearchInfo> info;
    gabi::Local<be<u32>[8]> infoLinkage;
    gabi::call<void>(0x02346B38,actor,info.get());
    if(u32(info->brought)==u32(info->total)) gabi::call<void>(0x025B8B68,gabi::at<void>(eventSave()),0x102);
}
VERIFY(0x02347128,ftree_setId);

void ftree_setBrought(Act_c* actor) {
    WWHD_FUNC(0x023471EC,void,actor);
    initTreeMap(); s32 tree=gabi::call<s32>(0x02349DFC,actor,4,0);
    u32 id=tree<10?read<u8>(0x10029154+u32(tree)):15;
    gabi::call<void>(0x02347128,actor,id); write<u32>(offset(actor,0x7B0),1);
}
VERIFY(0x023471EC,ftree_setBrought);

void ftree_unsetBrought(Act_c* actor) {
    WWHD_FUNC(0x02348314,void,actor);
    initTreeMap(); s32 tree=gabi::call<s32>(0x02349DFC,actor,4,0);
    u32 id=tree<10?read<u8>(0x10029154+u32(tree)):15;
    gabi::call<void>(0x0234829C,actor,id); write<u32>(offset(actor,0x7B0),0);
}
VERIFY(0x02348314,ftree_unsetBrought);

u32 ftree_remove(Act_c* actor) {
    WWHD_FUNC(0x0234726C,u32,actor);
    if(read<u32>(offset(actor,0x7B0))) gabi::call<void>(0x023471EC,actor);
    gabi::call<void>(0x025204C8,gabi::at<void>(offset(actor,0x3B4)),STR(0x10029550)); return 1;
}
VERIFY(0x0234726C,ftree_remove);

u32 ftree_playJoint(Act_c* actor) {
    WWHD_FUNC(0x0234855C,u32,actor);
    return gabi::call<u32>(0x025E535C,gabi::at<void>(read<u32>(offset(actor,0x3F0))),0,0,0)!=0;
}
VERIFY(0x0234855C,ftree_playJoint);

u32 ftree_playColor(Act_c* actor) {
    WWHD_FUNC(0x02348594,u32,actor);
    return gabi::call<u32>(0x025E742C,gabi::at<void>(offset(actor,0x3F8)))!=0;
}
VERIFY(0x02348594,ftree_playColor);

u32 ftree_actionNoneInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x023485C0,u32,actor,parameter);
    actor->smallVisible=0; actor->largeVisible=0; return 1;
}
VERIFY(0x023485C0,ftree_actionNoneInit);

u32 ftree_parameters(Act_c* actor,u32 width,u32 shift) {
    WWHD_FUNC(0x02349DFC,u32,actor,width,shift);
    u32 value=read<u32>(offset(actor,0xB0));
    u32 mask=(width&32)?0:(1u<<(width&31));
    return ((shift&32)?0:value>>(shift&31))&(mask-1);
}
VERIFY(0x02349DFC,ftree_parameters);

u32 ftree_processInit(Act_c* actor,u32 mode,s32 parameter) {
    WWHD_FUNC(0x023463E4,u32,actor,mode,parameter);
    if(mode>=13) return 0;
    u32 targetActor=gabi::ea(actor), target=actionTarget(0x10029260+mode*8,targetActor);
    if(!gabi::call_ptr<u32>(target,gabi::at<void>(targetActor),parameter)) return 0;
    write<u32>(offset(actor,0x7F4),mode);
    write<s16>(offset(actor,0x80A),0); write<s16>(offset(actor,0x814),0);
    write<s16>(offset(actor,0x80C),0); write<s16>(offset(actor,0x816),0); return 1;
}
VERIFY(0x023463E4,ftree_processInit);

void ftree_processMain(Act_c* actor) {
    WWHD_FUNC(0x0234749C,void,actor);
    u32 mode=read<u32>(offset(actor,0x7F4)); if(mode>=13) return;
    u32 targetActor=gabi::ea(actor),target=actionTarget(0x1002938C+mode*8,targetActor);
    gabi::call_ptr<void>(target,gabi::at<void>(targetActor));
}
VERIFY(0x0234749C,ftree_processMain);

u32 ftree_heapCallback(Act_c* actor) {
    WWHD_FUNC(0x023463E0,u32,actor);
    return gabi::call<u32>(0x02346064,actor);
}
VERIFY(0x023463E0,ftree_heapCallback);

s32 ftree_eventCallback(Act_c* actor,s32 event) {
    WWHD_FUNC(0x02346D30,s32,actor,event);
    return gabi::call<s32>(0x02346C18,actor,event);
}
VERIFY(0x02346D30,ftree_eventCallback);

void ftree_changeMSMain(Act_c* actor) {
    WWHD_FUNC(0x023490F8,void,actor);
    gabi::call<void>(0x02349098,actor);
}
VERIFY(0x023490F8,ftree_changeMSMain);

u32 ftree_createMethod(Act_c* actor) {
    WWHD_FUNC(0x02349C2C,u32,actor);
    return gabi::call<u32>(0x02346D34,actor);
}
VERIFY(0x02349C2C,ftree_createMethod);

u32 ftree_deleteMethod(Act_c* actor) {
    WWHD_FUNC(0x02349C30,u32,actor);
    return gabi::call<u32>(0x0234726C,actor);
}
VERIFY(0x02349C30,ftree_deleteMethod);

u32 ftree_executeMethod(Act_c* actor) {
    WWHD_FUNC(0x02349C34,u32,actor);
    return gabi::call<u32>(0x02347CF0,actor);
}
VERIFY(0x02349C34,ftree_executeMethod);

u32 ftree_drawMethod(Act_c* actor) {
    WWHD_FUNC(0x02349C38,u32,actor);
    return gabi::call<u32>(0x023480C4,actor);
}
VERIFY(0x02349C38,ftree_drawMethod);

void ftree_angleHelperDtor(void* object,u32 flags) {
    WWHD_FUNC(0x02349CD0,void,object,flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02349CD0,ftree_angleHelperDtor);

void ftree_vectorHelperDtor(void* object,u32 flags) {
    WWHD_FUNC(0x02349CE4,void,object,flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02349CE4,ftree_vectorHelperDtor);

void ftree_callbackHelperDtor(void* object,u32 flags) {
    WWHD_FUNC(0x02349D14,void,object,flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02349D14,ftree_callbackHelperDtor);

void ftree_otherHelperDtor(void* object,u32 flags) {
    WWHD_FUNC(0x02349D28,void,object,flags);
    if(object && (flags&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02349D28,ftree_otherHelperDtor);

void ftree_emptyCallback() { WWHD_FUNC(0x02349CF8,void); }
VERIFY(0x02349CF8,ftree_emptyCallback);

void ftree_emptyStatic() { WWHD_FUNC(0x02349DF0,void); }
VERIFY(0x02349DF0,ftree_emptyStatic);

u32 ftree_isDelete(Act_c* actor) { WWHD_FUNC(0x02349DF4,u32,actor); return 1; }
VERIFY(0x02349DF4,ftree_isDelete);
void ftree_terminateBuffer(void* object) {
    WWHD_FUNC(0x02349CFC,void,object);
    u32 a=gabi::ea(object),base=read<u32>(a),count=read<u32>(a+8);
    write<u8>(base+count-1,0);
}
VERIFY(0x02349CFC,ftree_terminateBuffer);
void ftree_destroy(Act_c* actor,u32 flags) {
    WWHD_FUNC(0x02349D3C,void,actor,flags);
    if(!actor) return;
    gabi::call<void>(0x02515A70,gabi::at<void>(offset(actor,0x680)),2);
    gabi::call<void>(0x02515860,gabi::at<void>(offset(actor,0x644)),2);
    gabi::call<void>(0x02515A70,gabi::at<void>(offset(actor,0x510)),2);
    gabi::call<void>(0x02515860,gabi::at<void>(offset(actor,0x4D4)),2);
    write<u32>(offset(actor,0x490),0x100290D4);
    write<u32>(offset(actor,0x4B0),0x100290F4);
    write<u32>(offset(actor,0x4BC),0x100290B4);
    gabi::call<void>(0x02008DAC,gabi::at<void>(offset(actor,0x470)),0);
    gabi::call<void>(0x025D50BC,actor,0);
    if(flags&1) gabi::call<void>(0x0273AF40,actor);
}
VERIFY(0x02349D3C,ftree_destroy);

void ftree_changeSMMain(Act_c* actor) {
    WWHD_FUNC(0x02348EC0,void,actor);
    u32 joint=gabi::call<u32>(0x0234855C,actor);
    u32 color=gabi::call<u32>(0x02348594,actor);
    if(joint==1 && color==1) gabi::call<void>(0x023463E4,actor,2,0);
}
VERIFY(0x02348EC0,ftree_changeSMMain);

void ftree_changeLS2Main(Act_c* actor) {
    WWHD_FUNC(0x02349098,void,actor);
    u32 joint=gabi::call<u32>(0x0234855C,actor);
    u32 color=gabi::call<u32>(0x02348594,actor);
    if(joint==1 && color==1) gabi::call<void>(0x023463E4,actor,1,0);
}
VERIFY(0x02349098,ftree_changeLS2Main);

void ftree_changeSLMain(Act_c* actor) {
    WWHD_FUNC(0x023490FC,void,actor);
    u32 joint=gabi::call<u32>(0x0234855C,actor);
    u32 color=gabi::call<u32>(0x02348594,actor);
    if(joint==1 && color==1) gabi::call<void>(0x023463E4,actor,5,0);
}
VERIFY(0x023490FC,ftree_changeSLMain);

s32 ftree_eventCallbackBody(Act_c* actor,s32 index) {
    WWHD_FUNC(0x02346C18,s32,actor,index);
    u32 play=gabi::call<u32>(0x025200D4);
    if(read<u8>(play+u32(index)+0x5BBB)==0x59 && !gabi::call<s32>(0x02346534,actor) && gabi::call<s32>(0x02346A40,actor)) {
        s32 last=gabi::call<s32>(0x02346BA8,actor);
        u32 eventSlot=last?0x4CC:0x4CA;
        if(gabi::call<s32>(0x02346A10,actor,read<s16>(offset(actor,eventSlot)))) {
            if(last) gabi::call<void>(0x025B8B68,gabi::at<void>(eventSave()),0x102);
            write<s16>(offset(actor,0x7C4),140); return read<s16>(offset(actor,eventSlot));
        }
    }
    s32 result=-1;
    if(gabi::call<s32>(0x02346A10,actor,read<s16>(offset(actor,0x4C8)))) result=read<s16>(offset(actor,0x4C8));
    return result;
}
VERIFY(0x02346C18,ftree_eventCallbackBody);

u32 ftree_pikuSmallInit(Act_c* actor,s32 repetitions) {
    WWHD_FUNC(0x0234988C,u32,actor,repetitions);
    write<s32>(offset(actor,0x7FC),repetitions>0?repetitions:1);
    write<u32>(offset(actor,0x800),0); actor->smallVisible=1; actor->largeVisible=0;
    f32 random=gabi::call<f32>(0x020198D8,1.0f);
    f32 rate=gabi::fmadds(read<f32>(0x1002953C),random,read<f32>(0x10029540));
    return gabi::call<u32>(0x02348394,actor,6,rate,10.0f,0);
}
VERIFY(0x0234988C,ftree_pikuSmallInit);

void ftree_pikuSmallMain(Act_c* actor) {
    WWHD_FUNC(0x0234991C,void,actor);
    if(gabi::call<s32>(0x0234855C,actor)) {
        if(read<s32>(offset(actor,0x800))<read<s32>(offset(actor,0x7FC))) {
            f32 random=gabi::call<f32>(0x020198D8,1.0f);
            f32 rate=gabi::fmadds(read<f32>(0x10029544),random,read<f32>(0x10029540));
            gabi::call<void>(0x02348394,actor,6,rate,10.0f,0);
        } else gabi::call<void>(0x023463E4,actor,1,0);
        write<u32>(offset(actor,0x800),read<u32>(offset(actor,0x800))+1);
    }
    gabi::call<void>(0x02348718,actor);
}
VERIFY(0x0234991C,ftree_pikuSmallMain);

u32 ftree_pikuMediumInit(Act_c* actor,s32 duration) {
    WWHD_FUNC(0x023499CC,u32,actor,duration);
    gabi::call<void>(0x023471EC,actor);
    write<s16>(offset(actor,0x804),s16(duration));
    write<s16>(offset(actor,0x806),0); write<s16>(offset(actor,0x808),0);
    actor->smallVisible=1; actor->largeVisible=0; return 1;
}
VERIFY(0x023499CC,ftree_pikuMediumInit);

u32 ftree_pikuLargeInit(Act_c* actor,s32 duration) {
    WWHD_FUNC(0x02349AFC,u32,actor,duration);
    gabi::call<void>(0x023471EC,actor);
    write<s16>(offset(actor,0x80E),s16(duration));
    write<s16>(offset(actor,0x810),0); write<s16>(offset(actor,0x812),0);
    actor->smallVisible=0; actor->largeVisible=1; return 1;
}
VERIFY(0x02349AFC,ftree_pikuLargeInit);

void ftree_pikuMediumMain(Act_c* actor) {
    WWHD_FUNC(0x02349A20,void,actor);
    s16 duration=read<s16>(offset(actor,0x804)),elapsed=read<s16>(offset(actor,0x806));
    if(elapsed<duration || duration==-1) {
        u16 angle=read<u16>(offset(actor,0x808)); u32 table=0x104A44F8+(angle>>3)*8;
        write<s16>(offset(actor,0x80A),s16(gabi::ftoi(f32(100.0f*read<f32>(table)))));
        write<s16>(offset(actor,0x80C),s16(gabi::ftoi(f32(300.0f*read<f32>(table)))));
        write<u16>(offset(actor,0x808),u16(angle+4000));
        elapsed=read<s16>(offset(actor,0x806));
    } else {
        gabi::call<void>(0x023463E4,actor,2,0); elapsed=read<s16>(offset(actor,0x806));
    }
    write<s16>(offset(actor,0x806),s16(u16(elapsed)+1)); gabi::call<void>(0x02348A38,actor);
}
VERIFY(0x02349A20,ftree_pikuMediumMain);

void ftree_pikuLargeMain(Act_c* actor) {
    WWHD_FUNC(0x02349B50,void,actor);
    s16 duration=read<s16>(offset(actor,0x80E)),elapsed=read<s16>(offset(actor,0x810));
    if(elapsed<duration || duration==-1) {
        u16 angle=read<u16>(offset(actor,0x812)); u32 table=0x104A44F8+(angle>>3)*8;
        write<s16>(offset(actor,0x814),s16(gabi::ftoi(f32(30.0f*read<f32>(table)))));
        write<s16>(offset(actor,0x816),s16(gabi::ftoi(f32(36.0f*read<f32>(table)))));
        write<u16>(offset(actor,0x812),u16(angle+3000));
        elapsed=read<s16>(offset(actor,0x810));
    } else {
        gabi::call<void>(0x023463E4,actor,3,0); elapsed=read<s16>(offset(actor,0x810));
    }
    write<s16>(offset(actor,0x810),s16(u16(elapsed)+1)); gabi::call<void>(0x02348BF8,actor);
}
VERIFY(0x02349B50,ftree_pikuLargeMain);

u32 ftree_waitLargeInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x02348BBC,u32,actor,parameter);
    actor->largeVisible=1; actor->smallVisible=0;
    gabi::call<void>(0x023471EC,actor); return 1;
}
VERIFY(0x02348BBC,ftree_waitLargeInit);
u32 ftree_changeLargeSmallInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x02349574,u32,actor,parameter);
    actor->largeVisible=1; actor->smallVisible=0;
    write<f32>(offset(actor,0x7B8),1.0f); write<f32>(offset(actor,0x7B4),1.0f);
    gabi::call<void>(0x02348314,actor); return 1;
}
VERIFY(0x02349574,ftree_changeLargeSmallInit);
u32 ftree_changeSmallMediumInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x02348E58,u32,actor,parameter);
    gabi::call<void>(0x023471EC,actor);
    if(gabi::call<u32>(0x02348CF8,actor,0)!=1) return 0;
    gabi::call<void>(0x025E19CC,0x6A17,gabi::at<void>(offset(actor,0x314))); return 1;
}
VERIFY(0x02348E58,ftree_changeSmallMediumInit);
u32 ftree_changeMediumSmallInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x0234903C,u32,actor,parameter);
    if(!gabi::call<u32>(0x02348F20,actor,0)) return 0;
    gabi::call<void>(0x02348314,actor); return 1;
}
VERIFY(0x0234903C,ftree_changeMediumSmallInit);
void ftree_staticInit() {
    WWHD_FUNC(0x02349C3C,void);
    write<u32>(0x10469BD0,0); write<u32>(0x10469BC8,0);
    write<u32>(0x10469BD4,0); write<u32>(0x10469BCC,0);
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C9654));
    f32 minimum=read<f32>(0x10029548),maximum=read<f32>(0x1002954C);
    write<f32>(0x10469BBC,minimum); write<f32>(0x10469BC0,maximum);
    gabi::call<void>(0x028ED6F8,gabi::at<void>(0x10469BC4));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C9660));
    gabi::call<void>(0x028EAB2C,gabi::at<void>(0x10469BC5));
    gabi::call<void>(0x028F026C,gabi::at<void>(0x101C966C));
}
VERIFY(0x02349C3C,ftree_staticInit);
void ftree_waitLargeMain(Act_c* actor) {
    WWHD_FUNC(0x02348BF8,void,actor);
    u32 mode=read<u32>(offset(actor,0x7F4));
    if(mode==3) {
        if(gabi::call<s32>(0x0256019C)) { gabi::call<void>(0x023463E4,actor,12,-1); return; }
        f32 chance=gabi::call<f32>(0x020198D8,30.0f);
        if(gabi::ftoi(chance)==0) {
            f32 random=gabi::call<f32>(0x020198D8,1.0f);
            s16 duration=s16(gabi::ftoi(gabi::fmadds(80.0f,random,100.0f)));
            gabi::call<void>(0x023463E4,actor,12,duration); return;
        }
        if(read<s16>(offset(actor,0x4CE))!=-1) gabi::call<void>(0x023463E4,actor,12,-1);
    } else if(mode==12) {
        if(!gabi::call<s32>(0x0256019C) && read<s16>(offset(actor,0x80E))==-1 && read<s16>(offset(actor,0x4CE))==-1)
            gabi::call<void>(0x023463E4,actor,3,0);
    }
}
VERIFY(0x02348BF8,ftree_waitLargeMain);

u32 ftree_setJointAnimation(Act_c* actor,s32 animation,f32 rate,f32 morph,s32 endOffset) {
    WWHD_FUNC(0x02348394,u32,actor,animation,rate,morph,endOffset);
    struct Name { be<u32> string,vtable; };
    gabi::Local<Name> name;
    gabi::Local<be<u32>[8]> nameLinkage;
     name->string=0x10029550; name->vtable=0x10029044;
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),name.get(),animation);
    if(endOffset>0) endOffset=0;
    if(!resource) return 0;
    f32 end=-1.0f;
    if(endOffset) {
        u32 target=read<u32>(read<u32>(resource+4)+0x14);
        s32 frames=gabi::call_ptr<s32>(target,gabi::at<void>(resource));
        end=f32(f32(frames)+f32(endOffset));
    }
    gabi::call<void>(0x025E4A98,gabi::at<void>(read<u32>(offset(actor,0x3F0))),gabi::at<void>(resource),0,morph,rate,0.0f,end,0);
    return 1;
}
VERIFY(0x02348394,ftree_setJointAnimation);

u32 ftree_nodeEffect(Act_c* actor,void* node,s32 phase) {
    WWHD_FUNC(0x02345ADC,u32,actor,node,phase);
    if(s16(actor->effectPending)==1 && phase==0) {
        struct Vec { be<f32> x,y,z; }; struct Matrix { be<f32> values[12]; };
        gabi::Local<Vec> source,result;
        gabi::Local<be<u32>[8]> sourceLinkage;
         source->x=0.0f;source->y=0.0f;source->z=0.0f;
        u32 joint=gabi::call<u32>(0x027F7878,node);
        u16 index=read<u16>(joint+4);
        u32 model=read<u32>(read<u32>(0x104B462C)+0x2C);
        u32 matrices=read<u32>(model+0x10); u16 flags=read<u16>(model+4);
        write<u16>(model+4,flags|0x10);
        gabi::call<void>(0x028E90D4,gabi::at<void>(matrices+index*48),gabi::at<void>(0x1048D0CC));
        gabi::Local<Matrix> matrix;
        gabi::Local<be<u32>[8]> matrixLinkage;
        gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),matrix.get());
        gabi::call<void>(0x028E8F64,matrix.get(),source.get(),result.get());
        gabi::call<void>(0x025E19CC,0x6A18,gabi::at<void>(offset(actor,0x314)));
        u32 play=gabi::call<u32>(0x025200D4);
        u32 first=gabi::call<u32>(0x025A847C,gabi::at<void>(read<u32>(play+0x5AB0)),0,0x82C4,result.get(),0,0,255,0,-1,0,0,0);
        play=gabi::call<u32>(0x025200D4);
        u32 second=gabi::call<u32>(0x025A847C,gabi::at<void>(read<u32>(play+0x5AB0)),0,0x82C5,result.get(),0,0,255,0,-1,0,0,0);
        if(first && second) actor->effectPending=0;
    }
    return 1;
}
VERIFY(0x02345ADC,ftree_nodeEffect);

u32 ftree_nodeEffectCallback(void* node,s32 phase) {
    WWHD_FUNC(0x02345C40,u32,node,phase);
    if(phase!=0) return 1;
    u32 actor=read<u32>(read<u32>(0x104B462C)+0xB8);
    return gabi::call<u32>(0x02345ADC,gabi::at<Act_c>(actor),node,0);
}
VERIFY(0x02345C40,ftree_nodeEffectCallback);

u32 ftree_nodeMediumCallback(void* node,s32 phase) {
    WWHD_FUNC(0x02345C68,u32,node,phase);
    if(phase!=0) return 1;
    u32 joint=gabi::call<u32>(0x027F7878,node);
    u32 system=read<u32>(0x104B462C), actor=read<u32>(system+0xB8); u16 index=read<u16>(joint+4);
    if(!actor) return 1;
    struct Angles { be<s16> x,y,z; }; gabi::Local<Angles> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    gabi::call<void>(0x0201A478,angles.get(),read<s16>(actor+0x3AC),read<s16>(actor+0x3AE),0);
    u32 model=read<u32>(system+0x2C),matrices=read<u32>(model+0x10); u16 flags=read<u16>(model+4);
    write<u16>(model+4,flags|0x10);
    gabi::call<void>(0x028E90D4,gabi::at<void>(matrices+index*48),gabi::at<void>(0x1048D0CC));
    gabi::call<void>(0x025F1B48,gabi::at<void>(0x1048D0CC),s16(angles->x),s16(angles->y),s16(angles->z));
    model=read<u32>(system+0x2C); flags=read<u16>(model+4); write<u16>(model+4,flags|0x10);
    f32 m[12]; for(unsigned i=0;i<12;++i) m[i]=read<f32>(0x1048D0CC+i*4);
    u32 destination=read<u32>(model+0x10)+index*48;
    const unsigned order[]={3,4,2,0,5,11,8,1,6,9,7,10};
    for(unsigned i:order) write<f32>(destination+i*4,m[i]);
    gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),gabi::at<void>(0x104B4868)); return 1;
}
VERIFY(0x02345C68,ftree_nodeMediumCallback);

u32 ftree_nodeLargeCallback(void* node,s32 phase) {
    WWHD_FUNC(0x02345DA0,u32,node,phase);
    if(phase!=0) return 1;
    u32 joint=gabi::call<u32>(0x027F7878,node);
    u32 system=read<u32>(0x104B462C), actor=read<u32>(system+0xB8); u16 index=read<u16>(joint+4);
    if(!actor) return 1;
    struct Angles { be<s16> x,y,z; }; gabi::Local<Angles> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    gabi::call<void>(0x0201A478,angles.get(),0,read<s16>(actor+0x3B0),read<s16>(actor+0x3B2));
    u32 model=read<u32>(system+0x2C),matrices=read<u32>(model+0x10); u16 flags=read<u16>(model+4);
    write<u16>(model+4,flags|0x10);
    gabi::call<void>(0x028E90D4,gabi::at<void>(matrices+index*48),gabi::at<void>(0x1048D0CC));
    gabi::call<void>(0x025F1B48,gabi::at<void>(0x1048D0CC),s16(angles->x),s16(angles->y),s16(angles->z));
    model=read<u32>(system+0x2C); flags=read<u16>(model+4); write<u16>(model+4,flags|0x10);
    f32 m[12]; for(unsigned i=0;i<12;++i) m[i]=read<f32>(0x1048D0CC+i*4);
    u32 destination=read<u32>(model+0x10)+index*48;
    const unsigned order[]={3,4,2,0,5,11,8,1,6,9,7,10};
    for(unsigned i:order) write<f32>(destination+i*4,m[i]);
    gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),gabi::at<void>(0x104B4868)); return 1;
}
VERIFY(0x02345DA0,ftree_nodeLargeCallback);

u32 ftree_execute(Act_c* actor) {
    WWHD_FUNC(0x02347CF0,u32,actor);
    if(read<u32>(offset(actor,0x7F4))==0) return 1;
    gabi::call<void>(0x023472BC,actor); gabi::call<void>(0x023467DC,actor);
    gabi::call<void>(0x0234749C,actor); gabi::call<void>(0x023474F4,actor);
    gabi::call<void>(0x025D69FC,actor,7,read<f32>(offset(actor,0x640)));
    s32 mediumStep=read<u32>(offset(actor,0x7F4))==11?1:30;
    gabi::call<void>(0x0200F428,gabi::at<void>(offset(actor,0x3AC)),read<s16>(offset(actor,0x80A)),mediumStep,0x1000);
    gabi::call<void>(0x0200F428,gabi::at<void>(offset(actor,0x3AE)),read<s16>(offset(actor,0x80C)),mediumStep,0x1000);
    u32 mode=read<u32>(offset(actor,0x7F4)); s32 largeStep=(mode==12 || mode==5)?1:10;
    gabi::call<void>(0x0200F428,gabi::at<void>(offset(actor,0x3B0)),read<s16>(offset(actor,0x814)),largeStep,0x1000);
    gabi::call<void>(0x0200F428,gabi::at<void>(offset(actor,0x3B2)),read<s16>(offset(actor,0x816)),largeStep,0x1000);
    gabi::call<void>(0x023477AC,actor);
    if(read<s16>(offset(actor,0x7C0))==0) write<s16>(offset(actor,0x7C0),s16(gabi::call<s32>(0x02347924,actor)));
    if(read<s16>(offset(actor,0x7C2))==1 && gabi::call<s32>(0x02347B7C,actor)) write<s16>(offset(actor,0x7C2),0);
    s16 timer=read<s16>(offset(actor,0x7C4)); if(timer>1) write<s16>(offset(actor,0x7C4),timer-1);
    timer=read<s16>(offset(actor,0x7C2)); if(timer>1) write<s16>(offset(actor,0x7C2),timer-1);
    return 1;
}
VERIFY(0x02347CF0,ftree_execute);

u32 ftree_waterReach(Act_c* actor) {
    WWHD_FUNC(0x02346A40,u32,actor);
    struct Vec { be<f32> x,y,z; }; struct Matrix { be<f32> values[12]; };
    gabi::call<void>(0x025200D4);
    gabi::Local<Vec> source,result;
    gabi::Local<be<u32>[8]> sourceLinkage;
     source->x=0.0f; source->z=50.0f; source->y=0.0f;
    u32 play=gabi::call<u32>(0x025200D4),player=read<u32>(play+0x5B2C);
    f32 playerX=read<f32>(player+0x314),actorX=read<f32>(offset(actor,0x314)),actorZ=read<f32>(offset(actor,0x31C));
    f32 playerZ=read<f32>(player+0x31C);
    s32 angle=gabi::call<s32>(0x020195B0,f32(actorX-playerX),f32(actorZ-playerZ));
    f32 y=read<f32>(player+0x318),z=read<f32>(player+0x31C),x=read<f32>(player+0x314);
    gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
    gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),angle);
    gabi::Local<Matrix> matrix;
    gabi::Local<be<u32>[8]> matrixLinkage;
     gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),matrix.get());
    gabi::call<void>(0x028E8F64,matrix.get(),source.get(),result.get());
    f32 distance=gabi::call<f32>(0x028E8DE8,result.get(),gabi::at<void>(offset(actor,0x314)));
    f32 radius=read<f32>(offset(actor,0x640))+20.0f;
    return !(distance>f32(radius*radius));
}
VERIFY(0x02346A40,ftree_waterReach);

void ftree_talkPosition(Act_c* actor) {
    WWHD_FUNC(0x023466E8,void,actor);
    f32 x=read<f32>(offset(actor,0x314)),y=read<f32>(offset(actor,0x318)),z=read<f32>(offset(actor,0x31C));
    f32 talkY=y,attentionY=y; u32 mode=read<u32>(offset(actor,0x7F4));
    write<u32>(offset(actor,0x818),0xFFFFFFFF);
    if(mode<13) {
        u32 message=read<u32>(0x101C96C8+mode*4);
        if(message==0x149F) { talkY=f32(talkY+80.0f); attentionY=f32(attentionY+105.0f); write<u32>(offset(actor,0x39C),0x0800000A); }
        else if(message==0x14A0) { talkY=f32(talkY+90.0f); attentionY=f32(attentionY+195.0f); write<u32>(offset(actor,0x39C),0x0800000A); }
        else if(message==0x14A1) { talkY=f32(talkY+250.0f); attentionY=f32(attentionY+200.0f); write<u32>(offset(actor,0x39C),0x08000008); }
    }
    write<f32>(offset(actor,0x390),x);write<f32>(offset(actor,0x380),talkY);
    write<f32>(offset(actor,0x384),z);write<f32>(offset(actor,0x37C),x);
    write<f32>(offset(actor,0x398),z);write<f32>(offset(actor,0x394),attentionY);
}
VERIFY(0x023466E8,ftree_talkPosition);
void ftree_eventExecute(Act_c* actor) {
    WWHD_FUNC(0x023477AC,void,actor);
    s16 state=read<s16>(offset(actor,0x4D0));
    if(state==1) {
        if(read<u16>(offset(actor,0xF8))!=1) return;
        write<s16>(offset(actor,0x4D0),2);
        u32 play=gabi::call<u32>(0x025200D4),player=read<u32>(play+0x5B2C);
        struct Vec { be<f32> x,y,z; }; struct Matrix { be<f32> values[12]; };
        gabi::Local<Vec> source,result;
        gabi::Local<be<u32>[8]> sourceLinkage;
         source->x=0.0f;
        s16 angle=s16(u16(read<s16>(offset(actor,0x32A)))+9000);
        f32 z=read<f32>(offset(actor,0x31C)),x=read<f32>(offset(actor,0x314)),y=read<f32>(offset(actor,0x318));
        source->y=1.0f;source->z=108.0f;
        gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
        gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),angle);
        gabi::Local<Matrix> matrix;
        gabi::Local<be<u32>[8]> matrixLinkage;
        gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),matrix.get());
        gabi::call<void>(0x028E8F64,matrix.get(),source.get(),result.get());
        u32 target=read<u32>(read<u32>(player+0xB4)+0x114);
        gabi::call_ptr<void>(target,gabi::at<void>(player),result.get(),s16(u16(angle)+0x8000));
    } else if(state==2) {
        s16 event=read<s16>(offset(actor,0x4CE));u32 play=gabi::call<u32>(0x025200D4);
        if(gabi::call<s32>(0x025440C8,gabi::at<void>(play+0x52C4),event)) {
            play=gabi::call<u32>(0x025200D4); u16 flags=read<u16>(play+0x52B8);write<u16>(play+0x52B8,flags|8);
            gabi::call<void>(0x023469BC,actor);
        }
    }
}
VERIFY(0x023477AC,ftree_eventExecute);

void ftree_firstState(Act_c* actor) {
    WWHD_FUNC(0x023465A0,void,actor);
    if(!gabi::call<s32>(0x025B7D90,gabi::at<void>(read<u32>(0x101F84DC)+0xD4),2)) {
        gabi::call<void>(0x023463E4,actor,0,0); return;
    }
    if(gabi::call<s32>(0x025B8B94,gabi::at<void>(eventSave()),0x102)) {
        gabi::call<void>(0x023463E4,actor,3,0); write<f32>(offset(actor,0x7B4),1.0f);return;
    }
    s32 brought=gabi::call<s32>(0x02346534,actor);
    f32 size=read<f32>(0x100292C8);
    gabi::call<void>(0x023463E4,actor,brought?2:1,0);
    write<f32>(offset(actor,0x7B4),size);
}
VERIFY(0x023465A0,ftree_firstState);

void ftree_setMatrix(Act_c* actor) {
    WWHD_FUNC(0x023467DC,void,actor);
    u32 model=read<u32>(read<u32>(offset(actor,0x3F0))+0x90);
    if(!model) gabi::call<void>(0x0273AA24,STR(0x100292E4),0x92A,STR(0x100292F8));
    f32 x=read<f32>(offset(actor,0x330)),y=read<f32>(offset(actor,0x334)),z=read<f32>(offset(actor,0x338));
    write<f32>(model+0xC0,y);write<f32>(model+0xBC,x);write<f32>(model+0xC4,z);
    gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),read<f32>(offset(actor,0x314)),read<f32>(offset(actor,0x318)),read<f32>(offset(actor,0x31C)));
    gabi::call<void>(0x025F1B48,gabi::at<void>(0x1048D0CC),read<s16>(offset(actor,0x328)),read<s16>(offset(actor,0x32A)),read<s16>(offset(actor,0x32C)));
    f32 matrix[12];for(unsigned i=0;i<12;++i) matrix[i]=read<f32>(0x1048D0CC+i*4);
    const unsigned firstOrder[]={7,2,1,6,3,0,8,4,5,10,11,9};
    for(unsigned i:firstOrder) write<f32>(model+0xC8+i*4,matrix[i]);
    gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),gabi::at<void>(offset(actor,0x3C0)));
    f32 size=read<f32>(offset(actor,0x7B4));x=read<f32>(offset(actor,0x330));y=read<f32>(offset(actor,0x334));
    f32 scaledX=scaledComponent(x,size);model=read<u32>(offset(actor,0x3F4));z=read<f32>(offset(actor,0x338));
    write<f32>(model+0xBC,scaledX);write<f32>(model+0xC0,scaledComponent(y,size));write<f32>(model+0xC4,scaledComponent(z,size));
    gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),read<f32>(offset(actor,0x314)),read<f32>(offset(actor,0x318)),read<f32>(offset(actor,0x31C)));
    gabi::call<void>(0x025F1B48,gabi::at<void>(0x1048D0CC),read<s16>(offset(actor,0x328)),read<s16>(offset(actor,0x32A)),read<s16>(offset(actor,0x32C)));
    for(unsigned i=0;i<12;++i) matrix[i]=read<f32>(0x1048D0CC+i*4);
    model=read<u32>(offset(actor,0x3F4));
    const unsigned secondOrder[]={11,5,8,1,2,4,10,7,9,6,0,3};
    for(unsigned i:secondOrder) write<f32>(model+0xC8+i*4,matrix[i]);
    gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),gabi::at<void>(offset(actor,0x3C0)));
    gabi::call<void>(0x027F4D5C,gabi::at<void>(read<u32>(offset(actor,0x3F4))));
}
VERIFY(0x023467DC,ftree_setMatrix);

u32 ftree_waitSmallInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x023485D4,u32,actor,parameter);
    actor->smallVisible=1;actor->largeVisible=0;gabi::call<void>(0x02348314,actor);
    struct Name { be<u32> string,vtable; };gabi::Local<Name> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->string=0x10029550;name->vtable=0x10029044;
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),name.get(),13);
    if(!resource) return 0;
    u32 model=read<u32>(read<u32>(offset(actor,0x3F0))+0x90);
    if(!model) gabi::call<void>(0x0273AA24,STR(0x10029454),0x56D,STR(0x10029468));
    f32 rate=1.0f;
    if(!gabi::call<u32>(0x025E8154,gabi::at<void>(offset(actor,0x3F8)),gabi::at<void>(read<u32>(model+0xAC)),gabi::at<void>(resource),1,0,0,-1,1,rate,0)) return 0;
    return gabi::call<u32>(0x02348394,actor,6,rate,0.0f,0);
}
VERIFY(0x023485D4,ftree_waitSmallInit);

u32 ftree_waitMediumInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x02348884,u32,actor,parameter);
    gabi::call<void>(0x023471EC,actor); actor->smallVisible=1;actor->largeVisible=0;
    struct Name { be<u32> string,vtable; };gabi::Local<Name> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->string=0x10029550;name->vtable=0x10029044;
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),name.get(),13);
    if(!resource) return 0;
    u32 model=read<u32>(read<u32>(offset(actor,0x3F0))+0x90);
    if(!model) gabi::call<void>(0x0273AA24,STR(0x10029480),0x5A8,STR(0x10029494));
    f32 rate=-1.0f;
    if(!gabi::call<u32>(0x025E8154,gabi::at<void>(offset(actor,0x3F8)),gabi::at<void>(read<u32>(model+0xAC)),gabi::at<void>(resource),1,0,0,-1,1,rate,0)) return 0;
    if(!gabi::call<u32>(0x02348394,actor,5,rate,0.0f,0)) return 0;
    u32 morph=read<u32>(offset(actor,0x3F0));
    s16 frame=s16(gabi::ftoi(f32(f32(read<s16>(morph+0xA2))-1.0f)));
    write<f32>(morph+0x9C,f32(frame)); return 1;
}
VERIFY(0x02348884,ftree_waitMediumInit);

void ftree_waitSmallMain(Act_c* actor) {
    WWHD_FUNC(0x02348718,void,actor);
    if(read<s16>(offset(actor,0x7C4))==1) {
        s32 next=read<s16>(offset(actor,0x4CE))==read<s16>(offset(actor,0x4CC))?4:8;
        if(gabi::call<u32>(0x023463E4,actor,next,0)) { write<s16>(offset(actor,0x7C4),0);return; }
    }
    u32 hit=gabi::call<u32>(0x02516464,gabi::at<void>(offset(actor,0x680)));
    if(!read<u32>(offset(actor,0x7F0)) && hit==1 && gabi::call<u32>(0x023463E4,actor,10,1)) {
        write<u32>(offset(actor,0x7F0),0);return;
    }
    if(gabi::ftoi(gabi::call<f32>(0x020198D8,100.0f))==0 && read<u32>(offset(actor,0x7F4))==1) {
        f32 random=gabi::call<f32>(0x020198D8,1.0f);
        s16 repetitions=s16(gabi::ftoi(gabi::fmadds(4.0f,random,1.0f)));
        gabi::call<void>(0x023463E4,actor,10,repetitions);
    }
    write<u32>(offset(actor,0x7F0),hit);
}
VERIFY(0x02348718,ftree_waitSmallMain);
void ftree_waitMediumMain(Act_c* actor) {
    WWHD_FUNC(0x02348A38,void,actor);
    if(!gabi::call<u32>(0x025B61C8,gabi::at<void>(read<u32>(0x101F84DC)+0x86))) {
        gabi::Local<daObjFtree::SearchInfo> info;
        gabi::Local<be<u32>[8]> infoLinkage;
        gabi::call<void>(0x02346B38,actor,info.get());
        if(u32(info->total)!=u32(info->brought)) { gabi::call<void>(0x023463E4,actor,9,0); return; }
    }
    if(read<u32>(offset(actor,0x7F4))!=2) return;
    u32 hit=gabi::call<u32>(0x02516464,gabi::at<void>(offset(actor,0x680)));
    if(gabi::call<s32>(0x0256019C)) {
        gabi::call<void>(0x023463E4,actor,11,-1);write<u32>(offset(actor,0x7F0),hit);return;
    }
    if(!read<u32>(offset(actor,0x7F0)) && hit==1 && gabi::call<u32>(0x023463E4,actor,11,120)) {
        write<u32>(offset(actor,0x7F0),0);return;
    }
    if(gabi::ftoi(gabi::call<f32>(0x020198D8,90.0f))==0) {
        f32 random=gabi::call<f32>(0x020198D8,1.0f);
        s16 duration=s16(gabi::ftoi(gabi::fmadds(90.0f,random,30.0f)));
        gabi::call<void>(0x023463E4,actor,11,duration);
    }
    write<u32>(offset(actor,0x7F0),hit);
}
VERIFY(0x02348A38,ftree_waitMediumMain);

u32 ftree_launchHeart(Act_c* actor) {
    WWHD_FUNC(0x02347B7C,u32,actor);
    struct Vec { be<f32> x,y,z; };struct Angles { be<s16> x,y,z; };
    gabi::Local<Vec> position,scale;
    gabi::Local<be<u32>[8]> positionLinkage;
    gabi::Local<Angles> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    position->y=read<f32>(offset(actor,0x318));position->x=read<f32>(offset(actor,0x314));
    scale->y=1.0f; angles->x=read<u16>(offset(actor,0x328));angles->z=read<u16>(offset(actor,0x32C));angles->y=read<u16>(offset(actor,0x32A));
    scale->x=1.0f;scale->z=1.0f;position->z=read<f32>(offset(actor,0x31C));
    gabi::call<void>(0x025200D4);
    angles->y=s16(u16(s16(angles->y))+9000);position->y=f32(f32(position->y)+860.0f);
    u32 item=gabi::call<u32>(0x025D8AB0,position.get(),7,read<s8>(offset(actor,0x326)),angles.get(),scale.get(),-1,0,1.75f,30.0f,-2.0999999046325684f);
    if(!item) return 0;
    write<u32>(item+0x2E8,read<u32>(offset(actor,4)));
    initTreeMap();s32 tree=gabi::call<s32>(0x02349DFC,actor,4,0);
    u32 id=tree<10?read<u8>(0x10029154+u32(tree)):15;
    gabi::call<void>(0x025B8AF4,gabi::at<void>(eventSave()),0x9B07,u8(id));
    write<u32>(item+0x2E0,0x4040);write<s16>(offset(actor,0x7C0),1);return 1;
}
VERIFY(0x02347B7C,ftree_launchHeart);

void ftree_talkMain(Act_c* actor) {
    WWHD_FUNC(0x023472BC,void,actor);
    u32 messageObject=read<u32>(0x101F4B5C);
    if(read<s16>(offset(actor,0x4CE))==-1 && read<u16>(offset(actor,0xF8))==1) {
        if(read<u32>(offset(actor,0x818))==0xFFFFFFFF) {
            u32 mode=read<u32>(offset(actor,0x7F4));
            u32 message=read<u32>(0x101C96C8+mode*4);
            write<u32>(offset(actor,0x818),gabi::call<u32>(0x025F7DB0,gabi::at<void>(messageObject),message,gabi::at<void>(offset(actor,0x37C))));return;
        }
        u32 status=gabi::call<u32>(0x025F795C,gabi::at<void>(messageObject));
        if(status==14) gabi::call<void>(0x025F74D0,gabi::at<void>(messageObject),16);
        else if(status==18) {
            gabi::call<void>(0x025F74D0,gabi::at<void>(messageObject),19);
            u32 play=gabi::call<u32>(0x025200D4);write<u16>(play+0x52B8,read<u16>(play+0x52B8)|8);
            gabi::call<void>(0x023466E8,actor);
        }
        return;
    }
    u32 mode=read<u32>(offset(actor,0x7F4));if(mode>=13 || !read<u32>(0x101C96C8+mode*4)) return;
    u32 play=gabi::call<u32>(0x025200D4),player=read<u32>(play+0x5B2C);
    f32 distance=gabi::call<f32>(0x028E8DE8,gabi::at<void>(offset(actor,0x314)),gabi::at<void>(player+0x314));
    f32 radius=f32(read<f32>(offset(actor,0x640))+130.0f);if(distance>f32(radius*radius)) return;
    gabi::call<void>(0x023466E8,actor);
    play=gabi::call<u32>(0x025200D4);player=read<u32>(play+0x5B2C);u16 bits=0x20;
    if(player) {
        struct Vec { be<f32> x,y,z; };gabi::Local<Vec> difference;
        // HD keeps the result above outgoing linkage (SP+8); aligned Local storage reserves 16 bytes.
        gabi::Local<be<u32>[4]> outgoingLinkage;
        gabi::call<void>(0x0201ADE0,gabi::at<void>(offset(actor,0x314)),difference.get(),gabi::at<void>(player+0x314));
        s32 angle=gabi::call<s32>(0x020195B0,f32(difference->x),f32(difference->z));
        s32 delta=s16(u32(angle)-u16(read<s16>(player+0x32A)));
        if((delta<0?-delta:delta)<=0x4000) bits=0x21;
    }
    write<u16>(offset(actor,0xFA),read<u16>(offset(actor,0xFA))|bits);
}
VERIFY(0x023472BC,ftree_talkMain);

u32 ftree_draw(Act_c* actor) {
    WWHD_FUNC(0x023480C4,u32,actor);
    if(!read<u32>(offset(actor,0x7F4))) return 1;
    if(u8(actor->smallVisible)==1) {
        u32 environment=gabi::call<u32>(0x02555D0C);
        gabi::call<void>(0x025626A4,gabi::at<void>(environment),0,gabi::at<void>(offset(actor,0x314)),gabi::at<void>(offset(actor,0x110)));
        u32 model=read<u32>(read<u32>(offset(actor,0x3F0))+0x90);
        if(!model) gabi::call<void>(0x0273AA24,STR(0x10029428),0x9C7,STR(0x1002943C));
        environment=gabi::call<u32>(0x02555D0C);
        gabi::call<void>(0x02562F5C,gabi::at<void>(environment),gabi::at<void>(model),gabi::at<void>(offset(actor,0x110)));
        u32 play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4634,read<u32>(play+0x5D70));
        play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4638,read<u32>(play+0x5D74));
        u32 mode=read<u32>(offset(actor,0x7F4));
        if(mode==5 || mode==6) {
            write<u32>(read<u32>(model+0xAC)+0x48,0);
            gabi::call<void>(0x02347E84,actor,gabi::at<void>(read<u32>(model+0xAC)),2,read<s16>(offset(actor,0x7DE)),read<s16>(offset(actor,0x7E0)),read<s16>(offset(actor,0x7E2)));
        } else gabi::call<void>(0x025E83FC,gabi::at<void>(offset(actor,0x3F8)),gabi::at<void>(read<u32>(model+0xAC)),read<f32>(offset(actor,0x3FC)));
        gabi::call<void>(0x025E54D8,gabi::at<void>(read<u32>(offset(actor,0x3F0))));
        play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4634,read<u32>(play+0x5D78));
        play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4638,read<u32>(play+0x5D7C));
    }
    if(u8(actor->largeVisible)==1) {
        u32 environment=gabi::call<u32>(0x02555D0C);
        gabi::call<void>(0x025626A4,gabi::at<void>(environment),1,gabi::at<void>(offset(actor,0x314)),gabi::at<void>(offset(actor,0x110)));
        environment=gabi::call<u32>(0x02555D0C);
        gabi::call<void>(0x02562F5C,gabi::at<void>(environment),gabi::at<void>(read<u32>(offset(actor,0x3F4))),gabi::at<void>(offset(actor,0x110)));
        u32 play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4634,read<u32>(play+0x5D70));
        play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4638,read<u32>(play+0x5D74));
        u32 model=read<u32>(offset(actor,0x3F4));
        gabi::call<void>(0x02347E84,actor,gabi::at<void>(read<u32>(model+0xAC)),2,read<s16>(offset(actor,0x7E6)),read<s16>(offset(actor,0x7E8)),read<s16>(offset(actor,0x7EA)));
        gabi::call<void>(0x025E2DE0,gabi::at<void>(read<u32>(offset(actor,0x3F4))),0);
        play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4634,read<u32>(play+0x5D78));
        play=gabi::call<u32>(0x025200D4);write<u32>(0x104B4638,read<u32>(play+0x5D7C));
    }
    return 1;
}
VERIFY(0x023480C4,ftree_draw);

u32 ftree_GrowAnimated(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x02348CF8,u32,actor,parameter);
    actor->smallVisible=1;actor->largeVisible=0;
    struct Name { be<u32> string,vtable; };gabi::Local<Name> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->string=0x10029550;name->vtable=0x10029044;
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),name.get(),13);if(!resource) return 0;
    u32 model=read<u32>(read<u32>(offset(actor,0x3F0))+0x90);
    if(!model) gabi::call<void>(0x0273AA24,STR(0x100294AC),0x68F,STR(0x100294C0));
    if(!gabi::call<u32>(0x025E8154,gabi::at<void>(offset(actor,0x3F8)),gabi::at<void>(read<u32>(model+0xAC)),gabi::at<void>(resource),1,0,0,-1,1,1.0f,0)) return 0;
    if(!gabi::call<u32>(0x02348394,actor,5,1.0f,10.0f,0)) return 0;
    gabi::call<void>(0x023471EC,actor);gabi::call<void>(0x025E19CC,0x6A17,gabi::at<void>(offset(actor,0x314))); return 1;
}
VERIFY(0x02348CF8,ftree_GrowAnimated);

u32 ftree_ShrinkAnimated(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x02348F20,u32,actor,parameter);
    actor->smallVisible=1;actor->largeVisible=0;
    struct Name { be<u32> string,vtable; };gabi::Local<Name> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->string=0x10029550;name->vtable=0x10029044;
    u32 resource=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),name.get(),13);if(!resource) return 0;
    u32 model=read<u32>(read<u32>(offset(actor,0x3F0))+0x90);
    if(!model) gabi::call<void>(0x0273AA24,STR(0x100294D8),0x736,STR(0x100294EC));
    if(!gabi::call<u32>(0x025E8154,gabi::at<void>(offset(actor,0x3F8)),gabi::at<void>(read<u32>(model+0xAC)),gabi::at<void>(resource),1,0,0,-1,1,-1.0f,0)) return 0;
    return gabi::call<u32>(0x02348394,actor,5,read<f32>(0x100294D4),10.0f,-20);
}
VERIFY(0x02348F20,ftree_ShrinkAnimated);

u32 ftree_growLargeInit(Act_c* actor,s32 parameter) {
    WWHD_FUNC(0x0234915C,u32,actor,parameter);
    actor->largeVisible=0;actor->effectPending=1;
    write<f32>(offset(actor,0x7B8),0.0f);u32 morph=read<u32>(offset(actor,0x3F0));write<f32>(offset(actor,0x7B4),read<f32>(0x100292C8));
    actor->smallVisible=1;write<s16>(offset(actor,0x7F8),30);
    u32 model=read<u32>(morph+0x90);
    if(!model) gabi::call<void>(0x0273AA24,STR(0x10029500),0x6B7,STR(0x10029514));
    gabi::call<void>(0x02345ED8,actor,gabi::at<void>(model),2,gabi::at<void>(offset(actor,0x7CE)),gabi::at<void>(offset(actor,0x7D0)),gabi::at<void>(offset(actor,0x7D2)));
    write<u16>(offset(actor,0x7DE),read<u16>(offset(actor,0x7CE)));
    write<u16>(offset(actor,0x7E0),read<u16>(offset(actor,0x7D0)));
    write<u16>(offset(actor,0x7E2),read<u16>(offset(actor,0x7D2)));
    u16 alpha=read<u16>(offset(actor,0x7D4));write<s16>(offset(actor,0x812),0);write<u16>(offset(actor,0x7E4),alpha);return 1;
}
VERIFY(0x0234915C,ftree_growLargeInit);

void ftree_getMaterialColor(Act_c* actor,void* modelData,u32 colorSlot,be<s16>* red,be<s16>* green,be<s16>* blue) {
    WWHD_FUNC(0x02345ED8,void,actor,modelData,colorSlot,red,green,blue);
    u32 model=gabi::ea(modelData),material=read<u32>(read<u32>(read<u32>(model+0xAC)+8)+0x10);
    if(!material) return;
    u16 materialIndex=read<u16>(read<u32>(material)+0xC);
    u32 materialEntry=read<u32>(model+0x34)+materialIndex*60;
    struct Buffer { be<u32> string,vtable,size; be<u8> characters[32]; };
    gabi::Local<Buffer> buffer;
    // HD places this object at SP+0x10, above outgoing linkage saved by the formatter.
    gabi::Local<be<u32>[4]> linkage;
    buffer->size=32;buffer->characters[31]=0;buffer->characters[0]=0;
    buffer->string=gabi::ea(buffer.get())+12;buffer->vtable=0x1002908C;
    gabi::call<void>(0x02759C28,buffer.get(),STR(0x100291E8),colorSlot);
    u32 target=read<u32>(u32(buffer->vtable)+0x14);gabi::call_ptr<void>(target,buffer.get());
    u32 entry=read<u32>(materialEntry),relative=read<u32>(entry+0x38);
    u32 table=relative?entry+0x38+relative:0;
    s32 index=gabi::call<s32>(0x027DF9B0,gabi::at<void>(table),gabi::at<void>(u32(buffer->string)));
    if(index<0) return;
    entry=read<u32>(materialEntry);relative=read<u32>(entry+0x34);table=relative?entry+0x34+relative:0;
    u32 colors=read<u32>(materialEntry+0x28)+read<u16>(table+u32(index)*20+2);
    f32 value=f32(read<f32>(colors)*255.0f);
    *red=value>1.0f?s16(gabi::ftoi(value)):1;
    value=f32(read<f32>(colors+4)*255.0f);
    *green=value>1.0f?s16(gabi::ftoi(value)):1;
    value=f32(read<f32>(colors+8)*255.0f);
    *blue=value>1.0f?s16(gabi::ftoi(value)):1;
}
VERIFY(0x02345ED8,ftree_getMaterialColor);

void ftree_setMaterialColor(Act_c* actor,void* tableObject,s32 colorIndex,s32 red,s32 green,s32 blue) {
    WWHD_FUNC(0x02347E84,void,actor,tableObject,colorIndex,red,green,blue);
    u32 material=read<u32>(read<u32>(gabi::ea(tableObject)+8)+0x10);
    struct Color { be<f32> r,g,b,a; };struct Vec { be<f32> x,y,z; };
    while(material) {
        u32 block=read<u32>(material+0x18),target=read<u32>(read<u32>(block+4)+0x34);
        u32 color=gabi::call_ptr<u32>(target,gabi::at<void>(block),colorIndex);write<s16>(color,s16(red));
        block=read<u32>(material+0x18);target=read<u32>(read<u32>(block+4)+0x34);
        color=gabi::call_ptr<u32>(target,gabi::at<void>(block),colorIndex);write<s16>(color+2,s16(green));
        block=read<u32>(material+0x18);target=read<u32>(read<u32>(block+4)+0x34);
        color=gabi::call_ptr<u32>(target,gabi::at<void>(block),colorIndex);write<s16>(color+4,s16(blue));
        block=read<u32>(material+0x18);target=read<u32>(read<u32>(block+4)+0x34);
        color=gabi::call_ptr<u32>(target,gabi::at<void>(block),colorIndex);
        block=read<u32>(material+0x18);target=read<u32>(read<u32>(block+4)+0x24);
        gabi::call_ptr<void>(target,gabi::at<void>(block),colorIndex,gabi::at<void>(color));
        s16 r=read<s16>(color),b=read<s16>(color+4),g=read<s16>(color+2),a=read<s16>(color+6);
        gabi::Local<Color> floating;
        gabi::Local<be<u32>[8]> floatingLinkage;
        floating->g=f32(f32(g)/255.0f);floating->b=f32(f32(b)/255.0f);floating->a=f32(f32(a)/255.0f);floating->r=f32(f32(r)/255.0f);
        gabi::Local<Color> rgb;
        gabi::Local<be<u32>[8]> rgbLinkage;
        gabi::call<void>(0x0274D458,rgb.get(),floating.get(),1.0f);
        u32 flags=read<u32>(material+0xA0),shift=u32(colorIndex)+4;
        u32 mask=(shift&32)?0:(1u<<(shift&31));write<u32>(material+0xA0,flags|mask);
        u32 output=gabi::call<u32>(0x027F9F0C,gabi::at<void>(material+0xA0),shift);
        a=read<s16>(color+6);f32 x=rgb->r,y=rgb->g,z=rgb->b;
        write<f32>(output+4,y);write<f32>(output+8,z);write<f32>(output,x);write<f32>(output+12,f32(f32(a)/255.0f));
        material=read<u32>(material+4);
    }
}
VERIFY(0x02347E84,ftree_setMaterialColor);

void ftree_setCollision(Act_c* actor) {
    WWHD_FUNC(0x023474F4,void,actor);
    if(u8(actor->smallVisible)!=1 && u8(actor->largeVisible)!=1) return;
    s32 hit=gabi::call<s32>(0x025162A4,gabi::at<void>(offset(actor,0x680)));
    u8 joint=actor->smallVisible;
    if(hit) {
        gabi::call<void>(0x023129C4,gabi::at<void>(offset(actor,0x314)),read<s8>(offset(actor,0x326)),gabi::at<void>(offset(actor,0x680)),joint==1?8:7);
        if(joint!=1) gabi::call<void>(0x02312C8C,actor,gabi::at<void>(offset(actor,0x680)));
        struct Vec { be<f32> x,y,z; };gabi::Local<Vec> position;
        gabi::Local<be<u32>[8]> positionLinkage;
        position->y=read<f32>(offset(actor,0x318));position->z=read<f32>(offset(actor,0x31C));position->x=read<f32>(offset(actor,0x314));
        gabi::call<void>(0x0255F458,position.get(),4,read<u32>(offset(actor,4)),100);
        gabi::call<void>(0x0251621C,gabi::at<void>(offset(actor,0x680)));return;
    }
    if(joint==1) {
        struct Vec { be<f32> x,y,z; };gabi::Local<Vec> center;
        gabi::Local<be<u32>[8]> centerLinkage;
        center->x=read<f32>(offset(actor,0x314));center->y=f32(read<f32>(offset(actor,0x318))-50.0f);center->z=read<f32>(offset(actor,0x31C));
        gabi::call<void>(0x020184DC,gabi::at<void>(offset(actor,0x628)),61.0f);
        gabi::call<void>(0x02018428,gabi::at<void>(offset(actor,0x628)),96.0f);
        gabi::call<void>(0x020182E0,gabi::at<void>(offset(actor,0x628)),center.get());
        u32 play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x0200E240,gabi::at<void>(play+0x26A4),gabi::at<void>(offset(actor,0x510)));
        write<f32>(offset(actor,0x640),61.0f);
        gabi::call<void>(0x020184DC,gabi::at<void>(offset(actor,0x798)),61.0f);
        gabi::call<void>(0x02018428,gabi::at<void>(offset(actor,0x798)),96.0f);
        gabi::call<void>(0x020182E0,gabi::at<void>(offset(actor,0x798)),gabi::at<void>(offset(actor,0x314)));
        play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x0200E240,gabi::at<void>(play+0x26A4),gabi::at<void>(offset(actor,0x680)));
    } else if(u8(actor->largeVisible)==1) {
        f32 radius=f32(132.0f*read<f32>(offset(actor,0x7B4)));write<f32>(offset(actor,0x640),radius);
        gabi::call<void>(0x020184DC,gabi::at<void>(offset(actor,0x798)),radius);
        gabi::call<void>(0x02018428,gabi::at<void>(offset(actor,0x798)),f32(950.0f*read<f32>(offset(actor,0x7B4))));
        gabi::call<void>(0x020182E0,gabi::at<void>(offset(actor,0x798)),gabi::at<void>(offset(actor,0x314)));
        u32 play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x0200E240,gabi::at<void>(play+0x26A4),gabi::at<void>(offset(actor,0x680)));
    }
}
VERIFY(0x023474F4,ftree_setCollision);

u32 ftree_placeHeart(Act_c* actor) {
    WWHD_FUNC(0x02347924,u32,actor);
    if(read<s16>(offset(actor,0x4CE))!=-1) return 0;
    if(gabi::call<u32>(0x025B8B94,gabi::at<void>(eventSave()),0x102)!=1) return 0;
    if(gabi::call<u32>(0x025B8B94,gabi::at<void>(eventSave()),0x2E20)) return 0;
    initTreeMap();s32 tree=gabi::call<s32>(0x02349DFC,actor,4,0);
    u32 id=tree<10?read<u8>(0x10029154+u32(tree)):15;
    if(gabi::call<u32>(0x025B8BB0,gabi::at<void>(eventSave()),0x9B07)!=id) return 0;
    struct Vec { be<f32> x,y,z; };struct Angles { be<s16> x,y,z; };struct Matrix { be<f32> values[12]; };
    gabi::Local<Vec> source,position,scale;
    gabi::Local<be<u32>[8]> sourceLinkage;
     source->x=0.0f;
    s16 angle=s16(u16(read<s16>(offset(actor,0x32A)))+9000);
    f32 z=read<f32>(offset(actor,0x31C)),x=read<f32>(offset(actor,0x314)),y=read<f32>(offset(actor,0x318));
    source->z=310.0f;source->y=1.0f;
    gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
    gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),angle);
    gabi::Local<Matrix> matrix;
    gabi::Local<be<u32>[8]> matrixLinkage;
    gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),matrix.get());
    gabi::call<void>(0x028E8F64,matrix.get(),source.get(),position.get());
    scale->y=1.0f;scale->x=1.0f;scale->z=1.0f;
    gabi::Local<Angles> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    gabi::call<void>(0x0201A478,angles.get(),0,0,0);
    u32 item=gabi::call<u32>(0x025D8AB0,position.get(),7,read<s8>(offset(actor,0x326)),angles.get(),scale.get(),-1,0,0.0f,0.0f,-2.0999999046325684f);
    if(!item) return 0;
    write<u32>(item+0x2E8,read<u32>(offset(actor,4)));
    initTreeMap();tree=gabi::call<s32>(0x02349DFC,actor,4,0);
    id=tree<10?read<u8>(0x10029154+u32(tree)):15;
    gabi::call<void>(0x025B8AF4,gabi::at<void>(eventSave()),0x9B07,u8(id));return 1;
}
VERIFY(0x02347924,ftree_placeHeart);

void ftree_growLargeMain(Act_c* actor) {
    WWHD_FUNC(0x02349238,void,actor);
    if(u8(actor->smallVisible)!=0) {
        gabi::call<void>(0x0200ECD4,gabi::at<void>(offset(actor,0x7B8)),1.0f,read<f32>(0x100292C8),1.0f,read<f32>(0x10029528));
        f32 blend=read<f32>(offset(actor,0x7B8));
        for(unsigned i=0;i<3;++i) {
            f32 start=f32(read<s16>(offset(actor,0x7CE + i*2)));
            write<s16>(offset(actor,0x7DE + i*2),s16(gabi::ftoi(gabi::fmadds(f32(255.0f-start),blend,start))));
        }
        if(blend==1.0f) {
            s16 timer=read<s16>(offset(actor,0x7F8));
            if(timer>=0) {
                if(timer>0) { timer=s16(u16(timer)-1);write<s16>(offset(actor,0x7F8),timer); }
                if(timer==0) { actor->smallVisible=0;actor->largeVisible=1; }
            }
        }
    }
    if(u8(actor->largeVisible)==0) return;
    f32 size=read<f32>(offset(actor,0x7B4)),minimum=read<f32>(0x100292C8),blend;
    if(!(size>minimum)) blend=0.0f;
    else if(!(size<1.0f)) blend=1.0f;
    else blend=f32(f32(size-minimum)/read<f32>(0x1002952C));
    gabi::call<void>(0x0200ECD4,gabi::at<void>(offset(actor,0x7B4)),1.0f,read<f32>(0x10029530),1.0f,read<f32>(0x10029534));
    for(unsigned i=0;i<3;++i) {
        f32 end=f32(read<s16>(offset(actor,0x7D6+i*2)));
        write<s16>(offset(actor,0x7E6+i*2),s16(gabi::ftoi(gabi::fmadds(f32(end-255.0f),blend,255.0f))));
    }
    if(read<f32>(offset(actor,0x7B4))==1.0f && gabi::call<u32>(0x023463E4,actor,3,0)) write<s16>(offset(actor,0x7C2),47);
    u16 phase=read<u16>(offset(actor,0x812));u32 table=0x104A44F8+(phase>>3)*8;
    write<s16>(offset(actor,0x814),s16(gabi::ftoi(f32(30.0f*read<f32>(table)))));
    s16 next=s16(u16(read<s16>(offset(actor,0x812)))+3000);
    write<s16>(offset(actor,0x816),s16(gabi::ftoi(f32(36.0f*read<f32>(table)))));write<s16>(offset(actor,0x812),next);
}
VERIFY(0x02349238,ftree_growLargeMain);

void ftree_shrinkLargeMain(Act_c* actor) {
    WWHD_FUNC(0x023495C0,void,actor);
    f32 minimum=read<f32>(0x100292C8);
    if(u8(actor->largeVisible)!=0) {
        f32 size=read<f32>(offset(actor,0x7B4)),blend;
        if(!(size>minimum)) blend=0.0f;
        else if(!(size<1.0f)) blend=1.0f;
        else blend=f32(f32(size-minimum)/read<f32>(0x1002952C));
        gabi::call<void>(0x0200ECD4,gabi::at<void>(offset(actor,0x7B4)),minimum,read<f32>(0x10029530),1.0f,read<f32>(0x10029534));
        for(unsigned i=0;i<3;++i) {
            f32 end=f32(read<s16>(offset(actor,0x7D6+i*2)));
            write<s16>(offset(actor,0x7E6+i*2),s16(gabi::ftoi(gabi::fmadds(f32(end-255.0f),blend,255.0f))));
        }
        if(read<f32>(offset(actor,0x7B4))==minimum) { actor->smallVisible=1;actor->largeVisible=0; }
    }
    if(u8(actor->smallVisible)==0) return;
    gabi::call<void>(0x0200ECD4,gabi::at<void>(offset(actor,0x7B8)),0.0f,minimum,1.0f,read<f32>(0x10029528));
    f32 blend=read<f32>(offset(actor,0x7B8));
    for(unsigned i=0;i<3;++i) {
        f32 start=f32(read<s16>(offset(actor,0x7CE + i*2)));
        write<s16>(offset(actor,0x7DE + i*2),s16(gabi::ftoi(gabi::fmadds(f32(255.0f-start),blend,start))));
    }
    if(blend==0.0f) gabi::call<void>(0x023463E4,actor,7,0);
}
VERIFY(0x023495C0,ftree_shrinkLargeMain);

u32 ftree_createHeap(Act_c* actor) {
    WWHD_FUNC(0x02346064,u32,actor);
    struct Name { be<u32> string,vtable; };gabi::Local<Name> animationName,modelName,largeName,colorName;
    gabi::Local<be<u32>[8]> animationNameLinkage;
    animationName->string=0x10029550;animationName->vtable=0x10029044;
    u32 animation=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),animationName.get(),6);
    if(!animation) gabi::call<void>(0x0273AA24,STR(0x100291F4),0x861,STR(0x10029208));
    modelName->vtable=0x10029044;modelName->string=0x10029550;
    u32 data=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),modelName.get(),10);
    if(!data) { gabi::call<void>(0x0273AA24,STR(0x100291F4),0x867,STR(0x1002922C));return 0; }
    u32 morph=gabi::call<u32>(0x025E4F64,0,gabi::at<void>(data),0,0,gabi::at<void>(animation),0,0,-1,1.0f,1,0,0,0x11020203);
    write<u32>(offset(actor,0x3F0),morph);if(!morph) return 0;
    u32 model=read<u32>(morph+0x90);
    if(!model) gabi::call<void>(0x0273AA24,STR(0x100291F4),0x87C,STR(0x1002923C));
    else write<u32>(model+0xB8,gabi::ea(actor));
    u32 count=read<u32>(data+4),nodes=read<u32>(data+8);
    write<u32>(nodes+(count>2?0x38:0)+8,0x02345C40);
    count=read<u32>(data+4);nodes=read<u32>(data+8);write<u32>(nodes+(count>3?0x54:0)+8,0x02345C68);
    count=read<u32>(data+4);nodes=read<u32>(data+8);write<u32>(nodes+(count>4?0x70:0)+8,0x02345C68);
    count=read<u32>(data+4);nodes=read<u32>(data+8);write<u32>(nodes+(count>5?0x8C:0)+8,0x02345C68);
    largeName->vtable=0x10029044;largeName->string=0x10029550;
    u32 largeData=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),largeName.get(),9);
    if(!largeData) { gabi::call<void>(0x0273AA24,STR(0x100291F4),0x888,STR(0x10029250));return 0; }
    u32 large=gabi::call<u32>(0x025E38E0,gabi::at<void>(largeData),0,0x11020203);
    write<u32>(offset(actor,0x3F4),large);if(!large) return 0;
    write<u32>(large+0xB8,gabi::ea(actor));count=read<u32>(largeData+4);nodes=read<u32>(largeData+8);
    write<u32>(nodes+(count>1?0x1C:0)+8,0x02345DA0);
    colorName->vtable=0x10029044;colorName->string=0x10029550;
    u32 color=gabi::call<u32>(0x026066C4,gabi::at<void>(read<u32>(0x101F4F28)),colorName.get(),13);
    if(!color) { gabi::call<void>(0x0273AA24,STR(0x100291F4),0x898,STR(0x1002921C));return 0; }
    u32 initialized=gabi::call<u32>(0x025E8154,gabi::at<void>(offset(actor,0x3F8)),gabi::at<void>(data),gabi::at<void>(color),1,0,0,-1,0,1.0f,0);
    gabi::call<void>(0x02345ED8,actor,gabi::at<void>(model),2,gabi::at<void>(offset(actor,0x7C6)),gabi::at<void>(offset(actor,0x7C8)),gabi::at<void>(offset(actor,0x7CA)));
    write<u16>(offset(actor,0x7DE),read<u16>(offset(actor,0x7C6)));write<u16>(offset(actor,0x7E0),read<u16>(offset(actor,0x7C8)));
    write<u16>(offset(actor,0x7E2),read<u16>(offset(actor,0x7CA)));write<u16>(offset(actor,0x7E4),read<u16>(offset(actor,0x7CC)));
    gabi::call<void>(0x02345ED8,actor,gabi::at<void>(read<u32>(offset(actor,0x3F4))),2,gabi::at<void>(offset(actor,0x7D6)),gabi::at<void>(offset(actor,0x7D8)),gabi::at<void>(offset(actor,0x7DA)));
    write<u16>(offset(actor,0x7E6),read<u16>(offset(actor,0x7D6)));write<u16>(offset(actor,0x7E8),read<u16>(offset(actor,0x7D8)));
    write<u16>(offset(actor,0x7EA),read<u16>(offset(actor,0x7DA)));write<u16>(offset(actor,0x7EC),read<u16>(offset(actor,0x7DC)));
    return read<u32>(offset(actor,0x3F0)) && model && initialized;
}
VERIFY(0x02346064,ftree_createHeap);

u32 ftree_create(Act_c* actor) {
    WWHD_FUNC(0x02346D34,u32,actor);
    u32 condition=read<u32>(offset(actor,0x2E4));
    if(!(condition&8)) {
        if(actor) {
            gabi::call<void>(0x025D4ED0,actor);write<u32>(offset(actor,0xB4),0x10029144);
            gabi::call<void>(0x025E80D0,gabi::at<void>(offset(actor,0x3F8)));
            gabi::call<void>(0x02008E0C,gabi::at<void>(offset(actor,0x470)));
            write<u8>(offset(actor,0x4B5),0);write<u8>(offset(actor,0x4B8),0);write<u8>(offset(actor,0x4B4),1);
            write<u32>(offset(actor,0x474),offset(actor,0x4BC));write<u8>(offset(actor,0x4B7),0);write<u8>(offset(actor,0x4B6),0);
            write<u32>(offset(actor,0x480),0x10029104);write<u32>(offset(actor,0x4C0),1);write<u8>(offset(actor,0x4BA),0);
            write<u32>(offset(actor,0x4B0),0x10029134);write<u32>(offset(actor,0x490),0x10029114);write<u8>(offset(actor,0x4B9),0);
            write<u32>(offset(actor,0x470),offset(actor,0x4B0));write<u32>(offset(actor,0x4BC),0x10029124);
            gabi::call<void>(0x0200BD2C,gabi::at<void>(offset(actor,0x4D4)));
            gabi::call<void>(0x02515DA0,gabi::at<void>(offset(actor,0x4F0)));
            write<u32>(offset(actor,0x4EC),0x1004AE88);write<u32>(offset(actor,0x4F0),0x1004AEC0);
            gabi::call<void>(0x02515FB8,gabi::at<void>(offset(actor,0x510)));
            write<u32>(offset(actor,0x624),0x100015A8);write<u32>(offset(actor,0x620),0x100290A4);
            gabi::call<void>(0x02018590,gabi::at<void>(offset(actor,0x628)));
            write<u32>(offset(actor,0x54C),0x1004B108);write<u32>(offset(actor,0x624),0x1004B160);write<u32>(offset(actor,0x63C),0x1004B150);
            gabi::call<void>(0x0200BD2C,gabi::at<void>(offset(actor,0x644)));
            gabi::call<void>(0x02515DA0,gabi::at<void>(offset(actor,0x660)));
            write<u32>(offset(actor,0x65C),0x1004AE88);write<u32>(offset(actor,0x660),0x1004AEC0);
            gabi::call<void>(0x02515FB8,gabi::at<void>(offset(actor,0x680)));
            write<u32>(offset(actor,0x794),0x100015A8);write<u32>(offset(actor,0x790),0x100290A4);
            gabi::call<void>(0x02018590,gabi::at<void>(offset(actor,0x798)));
            write<u32>(offset(actor,0x7AC),0x1004B150);write<u32>(offset(actor,0x794),0x1004B160);write<u32>(offset(actor,0x6BC),0x1004B108);
            condition=read<u32>(offset(actor,0x2E4));
        }
        write<u32>(offset(actor,0x2E4),condition|8);
    }
    u32 phase=gabi::call<u32>(0x02520460,gabi::at<void>(offset(actor,0x3B4)),STR(0x10029550));
    if(phase!=4) return phase;
    if(!gabi::call<u32>(0x025D63E8,actor,0x023463E0,0)) return 5;
    initTreeMap();s32 tree=gabi::call<s32>(0x02349DFC,actor,4,0);
    write<u32>(offset(actor,0x7BC),tree<10?read<u8>(0x10029154+u32(tree)):15);write<s16>(offset(actor,0x7C0),0);
    gabi::call<void>(0x023465A0,actor);gabi::call<void>(0x023466E8,actor);
    u32 process=read<u32>(offset(actor,4));f32 y=f32(read<f32>(offset(actor,0x318))+100.0f),z=read<f32>(offset(actor,0x31C)),x=read<f32>(offset(actor,0x314));
    write<f32>(offset(actor,0x49C),z);write<u32>(offset(actor,0x478),process);write<f32>(offset(actor,0x494),x);write<f32>(offset(actor,0x498),y);
    u32 play=gabi::call<u32>(0x025200D4);
    write<f32>(offset(actor,0x4C4),gabi::call<f32>(0x02008974,gabi::at<void>(play+0x12A0),gabi::at<void>(offset(actor,0x470))));
    gabi::call<void>(0x023467DC,actor);
    u32 model=read<u32>(read<u32>(offset(actor,0x3F0))+0x90),matrix=0;
    if(!model) gabi::call<void>(0x0273AA24,STR(0x10029324),0x8DE,STR(0x10029338)); else matrix=model+0xC8;
    write<u32>(offset(actor,0x348),matrix);
    gabi::call<void>(0x025D674C,actor,-300.0f,0.0f,-300.0f,300.0f,1000.0f,300.0f);
    gabi::call<void>(0x02515F14,gabi::at<void>(offset(actor,0x4D4)),255,255,actor);
    gabi::call<void>(0x02516518,gabi::at<void>(offset(actor,0x510)),gabi::at<void>(0x100291A4));
    write<u32>(offset(actor,0x554),offset(actor,0x4D4));u32 first=read<u32>(0x101FFBA8),flags=read<u32>(offset(actor,0x5A4));
    write<u32>(offset(actor,0x5C4),first);write<u32>(offset(actor,0x5C8),read<u32>(0x101FFBAC));u32 third=read<u32>(0x101FFBB0);
    write<u32>(offset(actor,0x5A4),flags|4);write<u32>(offset(actor,0x5CC),third);
    gabi::call<void>(0x02515F14,gabi::at<void>(offset(actor,0x644)),255,255,actor);
    gabi::call<void>(0x02516518,gabi::at<void>(offset(actor,0x680)),gabi::at<void>(0x10029160));
    write<u32>(offset(actor,0x6C4),offset(actor,0x644));first=read<u32>(0x101FFBA8);flags=read<u32>(offset(actor,0x714));
    write<u32>(offset(actor,0x734),first);write<u32>(offset(actor,0x738),read<u32>(0x101FFBAC));third=read<u32>(0x101FFBB0);
    write<u32>(offset(actor,0x714),flags|4);write<u32>(offset(actor,0x73C),third);
    gabi::call<void>(0x023469BC,actor);write<u32>(offset(actor,0x104),0x023469D0);write<u32>(offset(actor,0x100),0x02346D30);
    play=gabi::call<u32>(0x025200D4);write<s16>(offset(actor,0x4C8),s16(gabi::call<s32>(0x02543F10,gabi::at<void>(play+0x52C4),STR(0x1002934C),255)));
    play=gabi::call<u32>(0x025200D4);write<s16>(offset(actor,0x4CA),s16(gabi::call<s32>(0x02543F10,gabi::at<void>(play+0x52C4),STR(0x10029360),255)));
    play=gabi::call<u32>(0x025200D4);write<s16>(offset(actor,0x4CC),s16(gabi::call<s32>(0x02543F10,gabi::at<void>(play+0x52C4),STR(0x10029374),255)));
    return phase;
}
VERIFY(0x02346D34,ftree_create);
