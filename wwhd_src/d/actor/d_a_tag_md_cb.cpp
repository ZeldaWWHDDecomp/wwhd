// Medli/Makar event and warp trigger. Derived game source:
#include "bindings.h"
#include "d/actor/d_a_tag_md_cb.h"
namespace {
// A reference-return helper uses the guest endian wrapper rather than native storage.
template<class T> be<T>& field(u32 address) { return *gabi::at<be<T>>(address); }
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 events() { return play()+0x52C4; }
u32 save() { return field<u32>(0x101F84DC); }
BOOL eventBit(u16 bit) { return gabi::call<BOOL>(0x025B8B94,save()+0x644,bit); }
void onEventBit(u16 bit) { gabi::call(0x025B8B68,save()+0x644,bit); }
BOOL switchBit(daTag_MdCb_c* actor,u32 bit,bool roomArgument=true) {
    s8 room=actor->current.roomNo;
    u32 state=save()+0x20;
    return gabi::call<BOOL>(0x025BA0C0,state,bit,room);
}
fopAc_ac_c* linkPlayer() { return gabi::at<fopAc_ac_c>(field<u32>(play()+0x5B34)); }
s32 invokeAction(u32 entry,void* self,u32 argument) {
    s16 index=field<s16>(entry+2), delta=field<s16>(entry);
    u32 adjusted=gabi::ea(self)+(s32)delta;
    u32 target;
    if(index<0) target=field<u32>(entry+4);
    else { s16 offset=field<s16>(entry+6); u32 table=field<u32>(adjusted+(s32)offset); target=field<u32>(table+(u32)(s32)index*8+4); }
    return gabi::call_ptr<s32>(target,gabi::at<void>(adjusted),argument);
}
void copyAction(MdCbAction* destination,u32 source) { u32 a=field<u32>(source),b=field<u32>(source+4); field<u32>(gabi::ea(destination))=a; field<u32>(gabi::ea(destination)+4)=b; }
}
BOOL daTag_MdCb_c::checkCommandTalk() { WWHD_FUNC(0x024AE6E8,BOOL,this); return field<u16>(gabi::ea(this)+0xF8)==1; }
VERIFY(0x024AE6E8,&daTag_MdCb_c::checkCommandTalk);
s32 daTag_MdCb_c::getMyStaffId() { WWHD_FUNC(0x024AE694,s32,this); u32 name=field<u32>(0x101D1D64+(u32)(s32)(s8)argument*4); return gabi::call<s32>(0x02542D88,events(),name,0,0); }
VERIFY(0x024AE694,&daTag_MdCb_c::getMyStaffId);
void daTag_MdCb_c::eventEnd() { WWHD_FUNC(0x024AE6FC,void,this); u32 p=play(); field<u16>(p+0x52B8)=(u16)field<u16>(p+0x52B8)|8; u32 flags=mFlags; mEvent=-1; mFlags=flags&~1u; }
VERIFY(0x024AE6FC,&daTag_MdCb_c::eventEnd);
BOOL daTag_MdCb_c::checkAreaIn(fopAc_ac_c* other) {
    WWHD_FUNC(0x024AE968,BOOL,this,other);
    if(!other) return FALSE;
    f32 distance=gabi::call<f32>(0x025D69AC,this,other);
    f32 radius=scale.x; f32 otherY=other->current.pos.y, ownY=current.pos.y;
    f32 square=gabi::fmuls_ppc(radius,radius), difference=gabi::fsubs_ppc(otherY,ownY);
    if(!(distance<square) || difference< -10.0f || !(difference< (f32)scale.y)) return FALSE;
    mInArea=1; return TRUE;
}
VERIFY(0x024AE968,&daTag_MdCb_c::checkAreaIn);
BOOL daTag_MdCb_c::checkTimer() { WWHD_FUNC(0x024AE9FC,BOOL,this); return gabi::call<s16>(0x02055B64,&mTimer)==0; }
VERIFY(0x024AE9FC,&daTag_MdCb_c::checkTimer);
BOOL daTag_MdCb_c::checkEventFinish() { WWHD_FUNC(0x024AF18C,BOOL,this); switch((s8)argument) {
case 2:return eventBit(0x3310); case 3:return eventBit(0x4180); case 4:return eventBit(0x3308); case 5:return eventBit(0x3304); case 6:return eventBit(0x3302); case 7:return eventBit(0x3301); case 8:return eventBit(0x3480); case 9:return eventBit(0x3440); case 10:return eventBit(0x3420); case 11:return eventBit(0x3410); default:return FALSE; } }
VERIFY(0x024AF18C,&daTag_MdCb_c::checkEventFinish);
BOOL daTag_MdCb_c::setAction(MdCbAction* next,void* argument) {
    WWHD_FUNC(0x024AEFA0,BOOL,this,next,argument);
    s16 index=next->index, oldIndex=mAction.index;
    if(oldIndex==index && oldIndex==0) return TRUE;
    s16 delta=next->delta; u32 target=next->target;
    if(oldIndex==index && (s16)mAction.delta==delta && (u32)mAction.target==target) return TRUE;
    if(oldIndex!=0) { mActionState=-1; invokeAction(gabi::ea(&mAction),this,gabi::ea(argument)); }
    mAction.target=target; mAction.index=index; mAction.delta=delta;
    mCounterD=0; mCounterC=0; mActionState=0; mCounterA=0; mCounterB=0; mValue=0.0f;
    invokeAction(gabi::ea(&mAction),this,gabi::ea(argument)); return TRUE;
}
VERIFY(0x024AEFA0,&daTag_MdCb_c::setAction);
void daTag_MdCb_c::action(void* argument) { WWHD_FUNC(0x024AF0E0,void,this,argument); if((s16)mAction.index==0) { gabi::Local<MdCbAction> initial; copyAction(initial,0x1003F928); setAction(initial,nullptr); } invokeAction(gabi::ea(&mAction),this,gabi::ea(argument)); }
VERIFY(0x024AF0E0,&daTag_MdCb_c::action);
BOOL daTag_MdCb_c::warpAction(void* argument) { WWHD_FUNC(0x024AFD10,BOOL,this,argument); if(mActionState==0) mActionState=1; else if(mActionState!=-1 && this->argument<=1) { u16 bit=field<u16>(0x1003F940+(u32)(s32)(s8)this->argument*2); onEventBit(bit); } return TRUE; }
VERIFY(0x024AFD10,&daTag_MdCb_c::warpAction);
void daTag_MdCb_c::eventOrder() { WWHD_FUNC(0x024AF2CC,void,this); if(checkEventFinish()) { mPendingEvent=-1; return; } s8 pending=mPendingEvent; if(pending!=-1 && pending<7) { mEvent=pending; s16 id=field<s16>(gabi::ea(this)+0x3D4+(u32)(s32)pending*2); gabi::call(0x025D7A58,this,id,0xFF,0xFFFF,0,4); } }
VERIFY(0x024AF2CC,&daTag_MdCb_c::eventOrder);
BOOL daTag_MdCb_c::init() {
    WWHD_FUNC(0x024AF468,BOOL,this);
    u32 parameter=mParameters; u8 lowX=field<u8>(gabi::ea(this)+0x321);
    mMessage=parameter&0xFFFF; mSwitchCount=parameter>>24; mSwitchStart=(parameter>>16)&0xFF; mSwitch=lowX;
    for(u32 i=0;i<7;i++) { u32 name=field<u32>(0x101D1E30+i*4); s32 id=gabi::call<s32>(0x02543F10,events(),name,0xFF); mEvents[i]=id; }
    gabi::Local<MdCbAction> initial; copyAction(initial,argument>1?0x1003F920:0x1003F928); setAction(initial,nullptr);
    mPendingEvent=-1; s8 kind=argument; mReserved=0; mFoundSwitch=0;
    if(kind==8 || kind==11) { kind=argument; mTimer=0; }
    if(kind==3) { mTimer=60; scale.x=2250.0f; } else mTimer=field<s16>(0x1046E5B6);
    return TRUE;
}
VERIFY(0x024AF468,&daTag_MdCb_c::init);
static s32 daTag_MdCb_create(daTag_MdCb_c* actor) {
    WWHD_FUNC(0x024AF5D0,s32,actor);
    u32 flags=actor->actor_condition; if(!(flags&8)) { if(actor) { fopAc_ac_c_ct(actor); flags=actor->actor_condition; actor->__vtbl=0x1003FA70; } actor->actor_condition=flags|8; }
    s8 kind=actor->argument;
    if(kind<=1) { u16 bit=field<u16>(0x1003F940+(u32)(s32)kind*2); if(eventBit(bit)) return 5; }
    else { if(kind==11) gabi::call(0x025B8B7C,save()+0x644,0x3410); if(actor->checkEventFinish()) return 5; }
    if(field<s8>(0x1046E5A8)<0) { s32 index=gabi::call<s32>(0x025F0A10,0x1003FA2C,0x1046E5A8); field<s8>(0x1046E5A8)=index; field<u32>(0x1046E5C0)=gabi::ea(actor); field<u32>(0x101D1D94)=1; }
    else field<u32>(0x101D1D94)=(u32)field<u32>(0x101D1D94)+1;
    if(actor->argument>1) gabi::call(0x028E8E64,&actor->scale,&actor->scale,50.0f);
    return actor->init()?4:5;
}
VERIFY(0x024AF5D0,daTag_MdCb_create);
BOOL daTag_MdCb_c::execute() {
    WWHD_FUNC(0x024AF360,BOOL,this);
    s8 kind=argument; mWasInArea=(u8)mInArea; mInArea=0;
    if(kind<=1) { scale.x=field<f32>(0x1046E5B0); scale.y=field<f32>(0x1046E5AC); }
    if(!eventProc()) { for(u32 i=0;i<7;i++) { u32 name=field<u32>(0x101D1E30+i*4); s32 id=gabi::call<s32>(0x02543F10,events(),name,0xFF); mEvents[i]=id; } if(checkCondition()) action(nullptr); eventOrder(); }
    return TRUE;
}
VERIFY(0x024AF360,&daTag_MdCb_c::execute);
BOOL daTag_MdCb_c::eventProc() {
    WWHD_FUNC(0x024AE748,BOOL,this);
    if(field<u16>(gabi::ea(this)+0xF8)!=2) return FALSE;
    if(mPendingEvent!=-1) { u32 flags=mFlags; mPendingEvent=-1; mFlags=flags|1; }
    s32 staff=getMyStaffId(); if(!field<u8>(play()+0x5292) || checkCommandTalk()) return FALSE;
    if(staff!=-1) {
        s32 actionIndex=gabi::call<s32>(0x02542EDC,events(),staff,0x101D1DF8,6,1,0);
        u32 manager=events();
        if(actionIndex==-1) gabi::call(0x02543280,manager,staff);
        else {
            BOOL advance=gabi::call<BOOL>(0x025447C8,manager,staff);
            u32 offset=(u32)actionIndex*8;
            if(advance) invokeAction(0x101D1D98+offset,this,staff);
            if(invokeAction(0x101D1DC8+offset,this,staff)) gabi::call(0x02543280,events(),staff);
        }
    }
    s16 eventId=field<s16>(gabi::ea(this)+0x3D4+(u32)(s32)(s8)mEvent*2);
    if(gabi::call<BOOL>(0x025440C8,events(),eventId)) eventEnd();
    return TRUE;
}
VERIFY(0x024AE748,&daTag_MdCb_c::eventProc);
BOOL daTag_MdCb_c::talk_init() { WWHD_FUNC(0x024AF974,BOOL,this); u32 id=field<u32>(0x1046E598); u32 manager=field<u32>(0x101F4B5C); if(id==0xFFFFFFFF) { u32 message=mCurrentMessage; field<u32>(0x1046E598)=gabi::call<u32>(0x025F7DB0,manager,message,&eyePos); return FALSE; } return TRUE; }
VERIFY(0x024AF974,&daTag_MdCb_c::talk_init);
u16 daTag_MdCb_c::next_msgStatus(be<u32>* message) {
    WWHD_FUNC(0x024AF9E4,u16,this,message);
    u32 number=*message;
    switch(number) {
    case 0x19F0:onEventBit(0x3320); mTimer=field<s16>(0x1046E5B4);break;
    case 0x19F1:onEventBit(0x3310);onEventBit(0x4001);mTimer=field<s16>(0x1046E5B4);break;
    case 0x19F2:case 0x19F4:mTimer=field<s16>(0x1046E5B4);break;
    case 0x19F3:onEventBit(0x3304);break;case 0x19F5:onEventBit(0x3301);break;
    case 0x1530:onEventBit(0x3480);break;case 0x1531:onEventBit(0x3440);break;
    case 0x1533:*message=0x1535;return 0xF;case 0x1534:onEventBit(0x3420);break;case 0x1535:onEventBit(0x3410);break;
    } return 0x10;
}
VERIFY(0x024AF9E4,&daTag_MdCb_c::next_msgStatus);
BOOL daTag_MdCb_c::talk() {
    WWHD_FUNC(0x024AFB68,BOOL,this);
    u32 manager=field<u32>(0x101F4B5C); u16 status=gabi::call<u32>(0x025F795C,manager); play();
    if(status==0xE) { u16 next=next_msgStatus(&mCurrentMessage); gabi::call(0x025F74D0,manager,next); if(gabi::call<s32>(0x025F795C,manager)==0xF) { u32 message=mCurrentMessage; gabi::call(0x025F7DB0,manager,message,0); } }
    else if(status!=0x15) {
        if(status==6) { s8 kind=mEvent; if(kind==0 || kind==2 || kind==3 || kind==5 || kind==6) { u32 actor=field<u32>(play()+0x5B38); if(actor) { u32 p=play(); u32 table=field<u32>(actor+0xB4); u32 target=field<u32>(table+0x154); u8 animation=field<u8>(p+0x5BC5); gabi::call_ptr(target,actor,animation); } } }
        else if(status==0x12) { gabi::call(0x025F74D0,manager,0x13); return TRUE; }
    } return FALSE;
}
VERIFY(0x024AFB68,&daTag_MdCb_c::talk);
void daTag_MdCb_c::initialInitEvent(s32 staff) { WWHD_FUNC(0x024AF7F8,void,this,staff); u32 actor=field<u32>(play()+0x5B38); u32 control=play()+0x51D0; field<u32>(control+0xCC)=gabi::call<u32>(0x0253F124,control,actor); }
VERIFY(0x024AF7F8,&daTag_MdCb_c::initialInitEvent);
void daTag_MdCb_c::initialMsgSetEvent(s32 staff) {
    WWHD_FUNC(0x024AF844,void,this,staff);
    field<u32>(0x1046E598)=0xFFFFFFFF;
    u32 substance=gabi::call<u32>(0x0254487C,events(),staff,0x1003FA44,3);
    u32 number=substance?(u32)field<u32>(substance):(u32)mMessage; mCurrentMessage=number;
    if(number==0x19F3 || number==0x1531) {
        u32 actor=field<u32>(play()+0x5B38); if(actor) {
            u32 sound=mCurrentMessage==0x19F3?0x4993:0x4992; u32 p=play();
            gabi::Local<cXyz> direction; direction->x=0;direction->y=1;direction->z=0;
            gabi::call(0x025CB374,p+0x599C,4,-0x21,direction.get());
            s8 room=field<s8>(actor+0x326); s32 reverb=gabi::call<s32>(0x02520540,room);
            if(actor && actor+0x314) { u32 id=field<u32>(actor+4); gabi::call(0x025E1A7C,sound,actor+0x314,id,reverb); }
        }
    }
}
VERIFY(0x024AF844,&daTag_MdCb_c::initialMsgSetEvent);
void daTag_MdCb_c::initialPlayerOffDrow(s32 staff) { WWHD_FUNC(0x024AFCB0,void,this,staff); u32 actor=field<u32>(play()+0x5B34); field<u32>(actor+0x3B8)=(u32)field<u32>(actor+0x3B8)|0x08000000; }
VERIFY(0x024AFCB0,&daTag_MdCb_c::initialPlayerOffDrow);
void daTag_MdCb_c::initialPlayerOnDrow(s32 staff) { WWHD_FUNC(0x024AFCE0,void,this,staff); u32 actor=field<u32>(play()+0x5B34); field<u32>(actor+0x3B8)=(u32)field<u32>(actor+0x3B8)&~0x08000000u; }
VERIFY(0x024AFCE0,&daTag_MdCb_c::initialPlayerOnDrow);
BOOL daTag_MdCb_c::messageAction(void* parameter) {
    WWHD_FUNC(0x024AFD80,BOOL,this,parameter);
    if(mActionState==0) { mActionState=1; return TRUE; }
    if(mActionState==-1 || argument<=1 || checkEventFinish()) return TRUE;
    u32 actor=field<u32>(play()+0x5B38); if(!actor) return TRUE;
    u32 table=field<u32>(actor+0xB4),target=field<u32>(table+0x14C);
    if(!gabi::call_ptr<BOOL>(target,actor)) return TRUE;
    s16 name=field<s16>(actor+8);
    if(name==0x16F) {
        if(argument==5) mPendingEvent=2;
        else if((u32)field<u32>(actor+0x2E0)&0x2000) { if(!((u32)field<u32>(actor+0x420C)&0x800)) mPendingEvent=1; }
        else mPendingEvent=0;
    } else if(field<s16>(actor+8)==0x14E) {
        s8 kind=argument;
        if(kind==9) mPendingEvent=5;
        else if(kind==11) mPendingEvent=6;
        else if((u32)field<u32>(actor+0x2E0)&0x2000) { if(!((u16)field<u16>(0x101BDC2A)&1)) mPendingEvent=4; }
        else mPendingEvent=3;
    }
    return TRUE;
}
VERIFY(0x024AFD80,&daTag_MdCb_c::messageAction);
BOOL daTag_MdCb_c::checkCondition() {
    WWHD_FUNC(0x024AEA28,BOOL,this);
    fopAc_ac_c* other=gabi::at<fopAc_ac_c>(field<u32>(play()+0x5B38));
    if(!other) return FALSE;
    s8 room=other->current.roomNo; if(room==-1 || room!=(s8)current.roomNo) return FALSE;
    s8 kind=argument;
    if(kind<=1) return checkAreaIn(other)?TRUE:FALSE;
    if(kind==2) {
        if(eventBit(0x3310) || !switchBit(this,mSwitch)) return FALSE;
        u32 p=play(); u32 player=field<u32>(p+0x5B2C);
        f32 distance=gabi::call<f32>(0x025D69AC,other,player);
        return !(distance<250000.0f)?FALSE:checkTimer();
    }
    if(kind==3) {
        if(switchBit(this,mSwitch,false)) { onEventBit(0x4180);onEventBit(0x3310); }
        if(eventBit(0x4180) || !eventBit(0x3310)) return FALSE;
        if(eventBit(0x4001)) return checkTimer();
        if(checkAreaIn(linkPlayer()) || checkAreaIn(other)) return checkTimer();
    } else if(kind==4) {
        if(switchBit(this,mSwitch,false)) onEventBit(0x3308);
        if(switchBit(this,mSwitchStart) && !eventBit(0x3308) && checkAreaIn(other)) return checkTimer();
    } else if(kind==5) { if(eventBit(0x3404) && checkAreaIn(linkPlayer())) return TRUE; }
    else if(kind==6) {
        if(!gabi::call<BOOL>(0x025B7A2C,save()+0xD4,1,1)) return FALSE;
        if(switchBit(this,mSwitch)) onEventBit(0x3302);
        if(eventBit(0x3302)) return FALSE;
        u32 count=mSwitchCount,index=mSwitchStart,i=0;
        while(i<count) { if(switchBit(this,index)) { mFoundSwitch=1; break; } count=mSwitchCount; ++i;++index; }
        if(checkAreaIn(other) && mFoundSwitch) return checkTimer();
    } else if(kind==7) {
        if(switchBit(this,mSwitch,false)) onEventBit(0x3301);
        u32 count=mSwitchCount,index=mSwitchStart,i=0;
        while(i<count) { if(!switchBit(this,index)) break; count=mSwitchCount; ++i;++index; }
        if(!eventBit(0x3301) && i==(u32)mSwitchCount && checkAreaIn(other)) return checkTimer();
    } else if(kind==8) {
        if(switchBit(this,mSwitch,false)) onEventBit(0x3480);
        if(!eventBit(0x3480) && checkAreaIn(other)) return checkTimer();
    } else if(kind==9) { if(eventBit(0x3408) && checkAreaIn(linkPlayer())) return TRUE; }
    else if(kind==10) {
        if(!gabi::call<BOOL>(0x02520C0C,0x2F)) return FALSE;
        u32 count=mSwitchCount,index=mSwitchStart,i=0;
        while(i<count) { if(switchBit(this,index)) return checkTimer(); count=mSwitchCount; ++i;++index; }
    } else if(kind==11) {
        if(switchBit(this,mSwitch,false)) onEventBit(0x3410);
        if(!eventBit(0x3410) && eventBit(0x3440) && checkAreaIn(linkPlayer())) return TRUE;
    }
    return FALSE;
}
VERIFY(0x024AEA28,&daTag_MdCb_c::checkCondition);
static BOOL daTag_MdCb_Draw(daTag_MdCb_c* actor) { WWHD_FUNC(0x024AE68C,BOOL,actor);return TRUE; }
VERIFY(0x024AE68C,daTag_MdCb_Draw);
static BOOL daTag_MdCb_Execute(daTag_MdCb_c* actor) { WWHD_FUNC(0x024AF434,BOOL,actor);actor->execute();return TRUE; }
VERIFY(0x024AF434,daTag_MdCb_Execute);
static BOOL daTag_MdCb_IsDelete(daTag_MdCb_c* actor) { WWHD_FUNC(0x024AF458,BOOL,actor);return TRUE; }
VERIFY(0x024AF458,daTag_MdCb_IsDelete);
// HD profile Delete is empty. Virtual destruction still releases HIO ownership.
static BOOL daTag_MdCb_Delete(daTag_MdCb_c* actor) { WWHD_FUNC(0x024AF460,BOOL,actor);return TRUE; }
VERIFY(0x024AF460,daTag_MdCb_Delete);
static s32 daTag_MdCb_Create(daTag_MdCb_c* actor) { WWHD_FUNC(0x024AF748,s32,actor);return daTag_MdCb_create(actor); }
VERIFY(0x024AF748,daTag_MdCb_Create);
static void initialDefault(daTag_MdCb_c* actor,s32 staff) { WWHD_FUNC(0x024AF7EC,void,actor,staff); }
VERIFY(0x024AF7EC,initialDefault);
static BOOL actionDefault(daTag_MdCb_c* actor,s32 staff) { WWHD_FUNC(0x024AF7F0,BOOL,actor,staff);return TRUE; }
VERIFY(0x024AF7F0,actionDefault);
static BOOL actionMsgSetEvent(daTag_MdCb_c* actor,s32 staff) { WWHD_FUNC(0x024AF9E0,BOOL,actor,staff);return actor->talk_init(); }
VERIFY(0x024AF9E0,actionMsgSetEvent);
static BOOL actionMessageEvent(daTag_MdCb_c* actor,s32 staff) { WWHD_FUNC(0x024AFCAC,BOOL,actor,staff);return actor->talk(); }
VERIFY(0x024AFCAC,actionMessageEvent);
static void daTag_MdCb_destructor(daTag_MdCb_c* actor,s32 flags) {
    WWHD_FUNC(0x024AF74C,void,actor,flags);
    if(!actor) return;
    actor->__vtbl=0x1003FA70;
    s32 count=field<s32>(0x101D1D94);
    bool release=true;
    if(count) { count=(s32)((u32)count-1);field<s32>(0x101D1D94)=count;release=count<=0; }
    if(release) { s8 child=field<s8>(0x1046E5A8); if(child>=0) { gabi::call(0x025F0A18,child);field<s8>(0x1046E5A8)=-1; } }
    gabi::call(0x025D50BC,actor,0);
    if(flags&1) gabi::call(0x0273AF40,actor);
}
VERIFY(0x024AF74C,daTag_MdCb_destructor);
static void* daTag_MdCb_HIO_ctor(void* object) {
    WWHD_FUNC(0x024AFF08,void*,object);
    if(!object) object=gabi::call<void*>(0x0273AD10,0x20);
    if(object) { u32 base=gabi::ea(object);field<s8>(base)=-1;field<u32>(base+0x1C)=0x1003F954;for(u32 i=0;i<5;i++) field<u32>(base+4+i*4)=field<u32>(0x1003FA50+i*4); }
    return object;
}
VERIFY(0x024AFF08,daTag_MdCb_HIO_ctor);
static void __sinit_d_a_tag_md_cb_cpp() {
    WWHD_FUNC(0x024AFF7C,void,(u32)0);
    field<u32>(0x1046E5D0)=0;field<u32>(0x1046E5C8)=0;field<u32>(0x1046E5D4)=0;field<u32>(0x1046E5CC)=0;
    gabi::call(0x028F026C,0x101D1E4C);
    field<f32>(0x1046E59C)=-3.1415927410125732f;field<f32>(0x1046E5A0)=3.1415927410125732f;
    gabi::call(0x028ED6F8,0x1046E5A4);gabi::call(0x028F026C,0x101D1E58);
    gabi::call(0x028EAB2C,0x1046E5A5);gabi::call(0x028F026C,0x101D1E64);
    daTag_MdCb_HIO_ctor(gabi::at<void>(0x1046E5A8));
}
VERIFY(0x024AFF7C,__sinit_d_a_tag_md_cb_cpp);
