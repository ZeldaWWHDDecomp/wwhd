/* Command Melody monuments. */
#include "d/actor/d_a_obj_hsehi1.h"
static u32 save_base() { return gabi::load<u32>(0x101F84DC); }
static bool event_bit(u16 flag) { return gabi::call<s32>(0x025B8B94,gabi::at<void>(save_base()+0x644),flag)!=0; }
static void event_on(u16 flag) { gabi::call(0x025B8B68,gabi::at<void>(save_base()+0x644),flag); }
static void sound(daObj_hsh_c* a,u32 id,bool current=false) {
    s32 reverb=dComIfGp_getReverb(a->current.roomNo);
    gabi::call(0x025E1A40,id,current?&a->current.pos:&a->eyePos,0,reverb);
}
static void action_from(daObj_hsh_c* a,u32 addr,void* arg=nullptr) {
    gabi::Local<HshMember_l> ptr;
    ptr->adjust=gabi::load<s16>(addr); ptr->index=gabi::load<s16>(addr+2); ptr->target=gabi::load<u32>(addr+4);
    a->setAction(ptr,arg);
}
static u32 message_manager() { return gabi::load<u32>(0x101F4B5C); }
s16 daObj_hsh_c::XyCheckCB(s32 button) {
    WWHD_FUNC(0x0235A374,s16,this,button);
    u32 play=dComIfGp_ea();
    return gabi::load<u8>(play+(u32)button+0x5BBB)==0x22;
}
VERIFY(0x0235A374,&daObj_hsh_c::XyCheckCB);
static s16 check_cb(daObj_hsh_c* a,s32 button) { WWHD_FUNC(0x0235A3B4,s16,a,button); return a->XyCheckCB(button); }
VERIFY(0x0235A3B4,check_cb);
s16 daObj_hsh_c::XyEventCB(s32 button) {
    WWHD_FUNC(0x0235A3B8,s16,this,button);
    sound(this,0x8A7);
    u32 flags=mFlags; s16 event=mEventId[0];
    mEventSelector=0; mFlags=flags|1;
    return event;
}
VERIFY(0x0235A3B8,&daObj_hsh_c::XyEventCB);
static s16 event_cb(daObj_hsh_c* a,s32 b) { WWHD_FUNC(0x0235A418,s16,a,b); return gabi::call<s16>(0x0235A3B8,a,b); }
VERIFY(0x0235A418,event_cb);
s32 daObj_hsh_c::draw() {
    WWHD_FUNC(0x0235A5C0,s32,this);
    if(!((u32)mFlags&8)) {
        settingTevStruct(dKy_getEnvlight(),0,&current.pos,&tevStr);
        setLightTevColorType(dKy_getEnvlight(),mpModel,&tevStr);
        gabi::call(0x025E2DA8,(J3DModel*)mpModel);
    }
    return TRUE;
}
VERIFY(0x0235A5C0,&daObj_hsh_c::draw);
static s32 draw_actor(daObj_hsh_c* a) { WWHD_FUNC(0x0235A624,s32,a); return a->draw(); }
VERIFY(0x0235A624,draw_actor);
s32 daObj_hsh_c::checkCommandTalk() {
    WWHD_FUNC(0x0235A628,s32,this);
    if(gabi::load<u16>(gabi::ea(this)+0xF8)==1) {
        u32 play=dComIfGp_ea(); u32 mode=gabi::load<u8>(play+0x52B0);
        if(mode-1>3)return TRUE;
        if(mOrder==5)mOrder=-1;
    }
    return FALSE;
}
VERIFY(0x0235A628,&daObj_hsh_c::checkCommandTalk);
void daObj_hsh_c::eventEnd() {
    WWHD_FUNC(0x0235A6AC,void,this);
    dComIfGp_event_reset();
    u32 flags=mFlags; mEventSelector=-1; mFlags=flags&~1u;
}
VERIFY(0x0235A6AC,&daObj_hsh_c::eventEnd);
void daObj_hsh_c::checkOrder() {
    WWHD_FUNC(0x0235AAA0,void,this);
    if(gabi::load<u16>(gabi::ea(this)+0xF8)==1&&(u32)((s32)mOrder-3)<=2) {
        mOrder=-1;
        u32 play=dComIfGp_ea(),mode=gabi::load<u8>(play+0x52B0);
        if(mode-1>3)action_from(this,0x1002ADA0);
    }
}
VERIFY(0x0235AAA0,&daObj_hsh_c::checkOrder);
void daObj_hsh_c::emitterDelete(gptr<void>* emitter) {
    WWHD_FUNC(0x0235B2B4,void,this,emitter);
    u32 e=gabi::ea((void*)*emitter);
    if(e) {
        gabi::store<u32>(e+0x254,gabi::load<u32>(e+0x254)&~0x40u);
        e=gabi::ea((void*)*emitter); u32 flags=gabi::load<u32>(e+0x254);
        gabi::store<u32>(e+0x5C,0xFFFFFFFF); gabi::store<u32>(e+0x254,flags|1);
        *emitter=nullptr;
    }
}
VERIFY(0x0235B2B4,&daObj_hsh_c::emitterDelete);
void daObj_hsh_c::onOffDraw() {
    WWHD_FUNC(0x0235AEA8,void,this);
    u32 flags=mFlags; dBgW* bg=mpBgW; mFlags=flags|8;
    if(bg) { u32 play=dComIfGp_ea(); gabi::call(0x020087EC,gabi::at<void>(play+0x12A0),(dBgW*)mpBgW); }
}
VERIFY(0x0235AEA8,&daObj_hsh_c::onOffDraw);
void daObj_hsh_c::offOffDraw() {
    WWHD_FUNC(0x0235B3FC,void,this);
    u32 flags=mFlags; dBgW* bg=mpBgW; mFlags=flags&~8u;
    if(bg) { u32 play=dComIfGp_ea(); gabi::call(0x024EEA6C,gabi::at<void>(play+0x12A0),(dBgW*)mpBgW,this); }
}
VERIFY(0x0235B3FC,&daObj_hsh_c::offOffDraw);
void daObj_hsh_c::drawStart() { WWHD_FUNC(0x0235B450,void,this); offOffDraw(); }
VERIFY(0x0235B450,&daObj_hsh_c::drawStart);
void daObj_hsh_c::drawStop() { WWHD_FUNC(0x0235B454,void,this); onOffDraw(); emitterDelete(&mpEmitter); }
VERIFY(0x0235B454,&daObj_hsh_c::drawStop);
void daObj_hsh_c::initialDefault(s32 staff) { WWHD_FUNC(0x0235B3F0,void,this,staff); }
VERIFY(0x0235B3F0,&daObj_hsh_c::initialDefault);
s32 daObj_hsh_c::actionDefault(s32 staff) { WWHD_FUNC(0x0235B3F4,s32,this,staff); return TRUE; }
VERIFY(0x0235B3F4,&daObj_hsh_c::actionDefault);
s32 daObj_hsh_c::talk_init() {
    WWHD_FUNC(0x0235B87C,s32,this);
    u32 id=gabi::load<u32>(0x10469FE4),manager=message_manager();
    if(id==0xFFFFFFFF) {
        u32 new_id=gabi::call<u32>(0x025F7DB0,gabi::at<void>(manager),(u32)mMsgNo,&eyePos);
        gabi::store<u32>(0x10469FE4,new_id); return FALSE;
    }
    return TRUE;
}
VERIFY(0x0235B87C,&daObj_hsh_c::talk_init);
s32 daObj_hsh_c::actionMsgSetEvent(s32 staff) { WWHD_FUNC(0x0235B8E8,s32,this,staff); return talk_init(); }
VERIFY(0x0235B8E8,&daObj_hsh_c::actionMsgSetEvent);
u32 daObj_hsh_c::next_msgStatus(be<u32>* message) {
    WWHD_FUNC(0x0235B8EC,u32,this,message);
    u32 n=*message;
    if(n==0||n==0xEF3||n==(u32)mPrmMsgNo||n==0x1901)return 16;
    return 15;
}
VERIFY(0x0235B8EC,&daObj_hsh_c::next_msgStatus);
s32 daObj_hsh_c::actionMessageEvent(s32 staff) {
    WWHD_FUNC(0x0235BAD0,s32,this,staff);
    void* p=gabi::call<void*>(0x0254487C,dComIfGp_getPEvtManager(),staff,STR(0x1002AF2C),3);
    return talk(p?gabi::load<s32>(gabi::ea(p)):0);
}
VERIFY(0x0235BAD0,&daObj_hsh_c::actionMessageEvent);
s32 daObj_hsh_c::actionTactEvent(s32 staff) {
    WWHD_FUNC(0x0235BB38,s32,this,staff);
    void* p=gabi::call<void*>(0x0254487C,dComIfGp_getPEvtManager(),staff,STR(0x1002AF34),3);
    u32 mode=p?gabi::load<u32>(gabi::ea(p)):0;
    u32 play=dComIfGp_ea(),player=gabi::load<u32>(play+0x5B2C),vt=gabi::load<u32>(player+0xB4);
    u32 music=gabi::call<u32>(gabi::load<u32>(vt+0x2C),gabi::at<void>(player));
    if(music==mode)mFlags=(u32)mFlags|4;
    return talk(1);
}
VERIFY(0x0235BB38,&daObj_hsh_c::actionTactEvent);
void daObj_hsh_c::initialJudgeEvent(s32 staff) {
    WWHD_FUNC(0x0235BBD0,void,this,staff);
    u32 flags=mFlags;
    if(flags&4)mFlags=flags&~4u;
    else if(flags&2) { drawStart(); mFlags=(u32)mFlags&~2u; eventEnd(); }
}
VERIFY(0x0235BBD0,&daObj_hsh_c::initialJudgeEvent);
void daObj_hsh_c::particle_set(u16 id) {
    WWHD_FUNC(0x0235BC44,void,this,id);
    u32 play=dComIfGp_ea(),particles=gabi::load<u32>(play+0x5AB0);
    gabi::call(0x025A847C,gabi::at<void>(particles),0,id,&current.pos,&current.angle,0,255,0,-1,0,0,0);
}
VERIFY(0x0235BC44,static_cast<void(daObj_hsh_c::*)(u16)>(&daObj_hsh_c::particle_set));
void daObj_hsh_c::particle_set(gptr<void>* emitter,u16 id) {
    WWHD_FUNC(0x0235BCB4,void,this,emitter,id);
    if(!gabi::ea((void*)*emitter)) {
        u32 play=dComIfGp_ea(),particles=gabi::load<u32>(play+0x5AB0);
        void* e=gabi::call<void*>(0x025A847C,gabi::at<void>(particles),0,id,&current.pos,&current.angle,0,255,0,-1,0,0,0);
        *emitter=e;
        if(e)gabi::store<u32>(gabi::ea(e)+0x254,gabi::load<u32>(gabi::ea(e)+0x254)|0x40);
    }
}
VERIFY(0x0235BCB4,static_cast<void(daObj_hsh_c::*)(gptr<void>*,u16)>(&daObj_hsh_c::particle_set));
void daObj_hsh_c::setAttention(bool set) {
    WWHD_FUNC(0x0235BF68,void,this,set);
    if(!set)return;
    f32 y=current.pos.y,extra_eye=gabi::load<f32>(0x1046A010),extra_attn=gabi::load<f32>(0x1046A00C);
    f32 eye=y+(argument==0?90.0f:80.0f),attn=y+(argument==0?180.0f:120.0f);
    f32 z=current.pos.z,x=current.pos.x;
    eyePos.x=x;eyePos.y=eye+extra_eye;eyePos.z=z;
    gabi::store<f32>(gabi::ea(this)+0x390,x); gabi::store<f32>(gabi::ea(this)+0x394,attn+extra_attn); gabi::store<f32>(gabi::ea(this)+0x398,z);
}
VERIFY(0x0235BF68,&daObj_hsh_c::setAttention);
u32 daObj_hsh_c::getMsg() { WWHD_FUNC(0x0235C260,u32,this); return argument==0?0x1901:(u32)mPrmMsgNo; }
VERIFY(0x0235C260,&daObj_hsh_c::getMsg);
s32 daObj_hsh_c::offAction(void* arg) { WWHD_FUNC(0x0235C3AC,s32,this,arg); if(mActionMode==0)mActionMode=1; return TRUE; }
VERIFY(0x0235C3AC,&daObj_hsh_c::offAction);
s32 daObj_hsh_c::deleteAction(void* arg) {
    WWHD_FUNC(0x0235C3C8,s32,this,arg);
    if(mActionMode==0)mActionMode=1;
    else if(mActionMode!=-1)fopAcM_delete(this);
    return TRUE;
}
VERIFY(0x0235C3C8,&daObj_hsh_c::deleteAction);
static s32 invoke(daObj_hsh_c* a,u32 descriptor,void* arg,s16 index,s16 adjust) {
    u32 base=gabi::ea(a)+(s32)adjust,target;
    if(index<0)target=gabi::load<u32>(descriptor+4);
    else {
        s16 offset=gabi::load<s16>(descriptor+6);
        u32 vt=gabi::load<u32>(base+(s32)offset);
        target=gabi::load<u32>(vt+(u32)(s32)index*8+4);
    }
    return gabi::call<s32>(target,gabi::at<void>(base),arg);
}
s32 daObj_hsh_c::setAction(HshMember_l* next,void* arg) {
    WWHD_FUNC(0x0235A960,s32,this,next,arg);
    s16 new_index=next->index,old_index=mAction.index;
    s16 adjust; u32 target;
    if(old_index==new_index) {
        if(old_index==0)return TRUE;
        adjust=next->adjust; target=next->target;
        if((s16)mAction.adjust==adjust&&(u32)mAction.target==target)return TRUE;
    } else {target=next->target;adjust=next->adjust;}
    if(old_index!=0) {
        s16 old_adjust=mAction.adjust;old_index=mAction.index;
        mActionMode=-1;
        invoke(this,gabi::ea(&mAction),arg,old_index,old_adjust);
    }
    mAction.target=target;mAction.index=new_index;mAction.adjust=adjust;
    r3=0;r2=0;mActionMode=0;r0=0;r1=0;value=0;
    invoke(this,gabi::ea(&mAction),arg,new_index,adjust);
    return TRUE;
}
VERIFY(0x0235A960,&daObj_hsh_c::setAction);
void daObj_hsh_c::action(void* arg) {
    WWHD_FUNC(0x0235AB28,void,this,arg);
    s16 index=mAction.index;
    if(index==0) {speedF=0;action_from(this,0x1002AD90);index=mAction.index;}
    s16 adjust=mAction.adjust;
    invoke(this,gabi::ea(&mAction),arg,index,adjust);
}
VERIFY(0x0235AB28,&daObj_hsh_c::action);
void daObj_hsh_c::eventOrder() {
    WWHD_FUNC(0x0235ABE0,void,this);
    if((u32)mFlags&1)return;
    s32 order=mOrder;
    if(order==4||order==3) {
        s32 now=mOrder;
        u32 addr=gabi::ea(this)+0xFA;
        gabi::store<u16>(addr,gabi::load<u16>(addr)|1);
        if(now==4)gabi::call(0x025D76A8,this);
    } else if(order==5) {
        u32 addr=gabi::ea(this)+0xFA;
        gabi::store<u16>(addr,gabi::load<u16>(addr)|0x21);
        if(argument==0)gabi::call(0x0253E9B0,gabi::at<void>(gabi::ea(this)+0xF8),STR(0x1002AEB4));
    } else if(order!=-1&&order<2) {
        mEventSelector=order;
        s16 event=gabi::load<s16>(gabi::ea(this)+0x644+(u32)order*2);
        fopAcM_orderOtherEventId(this,event,255,65535,0,1);
    }
}
VERIFY(0x0235ABE0,&daObj_hsh_c::eventOrder);
s32 daObj_hsh_c::eventProc() {
    WWHD_FUNC(0x0235A6F8,s32,this);
    if(gabi::load<u16>(gabi::ea(this)+0xF8)==2&&mOrder!=-1) {mOrder=-1;mFlags=(u32)mFlags|1;}
    s32 staff=gabi::call<s32>(0x02542D88,dComIfGp_getPEvtManager(),STR(0x1002AEAC),0,0);
    u32 play=dComIfGp_ea();
    if(gabi::load<u8>(play+0x5292)!=0&&checkCommandTalk()==0) {
        if(staff!=-1) {
            s32 act=gabi::call<s32>(0x02542EDC,dComIfGp_getPEvtManager(),staff,gabi::at<void>(0x101CA25C),8,1,0);
            void* manager=dComIfGp_getPEvtManager();
            if(act==-1)gabi::call(0x02543280,manager,staff);
            else {
                s32 advanced=gabi::call<s32>(0x025447C8,manager,staff);
                u32 offset=(u32)act*8;
                if(advanced) {
                    u32 p=0x101CA1DC+offset; s16 index=gabi::load<s16>(p+2),adjust=gabi::load<s16>(p);
                    invoke(this,p,gabi::at<void>((u32)staff),index,adjust);
                }
                u32 p=0x101CA21C+offset; s16 index=gabi::load<s16>(p+2),adjust=gabi::load<s16>(p);
                if(invoke(this,p,gabi::at<void>((u32)staff),index,adjust)!=0)gabi::call(0x02543280,dComIfGp_getPEvtManager(),staff);
            }
        }
        if((u32)mFlags&1) {
            s32 selector=mEventSelector;
            s16 event=gabi::load<s16>(gabi::ea(this)+0x644+(u32)selector*2);
            if(gabi::call<s32>(0x025440C8,dComIfGp_getPEvtManager(),event))eventEnd();
            return TRUE;
        }
        if(staff!=-1)return TRUE;
    }
    return FALSE;
}
VERIFY(0x0235A6F8,&daObj_hsh_c::eventProc);
s32 daObj_hsh_c::talk(s32 mode) {
    WWHD_FUNC(0x0235B924,s32,this,mode);
    u32 manager=message_manager();
    u16 status=gabi::call<u32>(0x025F795C,gabi::at<void>(manager));
    dComIfGp_get();
    if(status==14) {
        if(mode==1) {
            if((u32)mFlags&4) {
                gabi::call(0x025F74D0,gabi::at<void>(manager),16);
                gabi::store<u8>(manager+0x921,1);
                if((u32)mMsgNo==0x5B3) {
                    s32 room=current.roomNo;u32 save=save_base();
                    gabi::call(0x025B9E38,gabi::at<void>(save+0x20),(s32)mSwitchNo,room);
                }
            } else {
                u32 play=dComIfGp_ea();
                if(gabi::load<u8>(play+0x5BD3)!=0) {
                    gabi::call(0x025F74D0,gabi::at<void>(manager),16);
                    gabi::store<u8>(manager+0x921,1); mFlags=(u32)mFlags|2;
                }
            }
        } else {
            u32 next=next_msgStatus(&mMsgNo);
            gabi::call(0x025F74D0,gabi::at<void>(manager),next);
            if(gabi::call<u32>(0x025F795C,gabi::at<void>(manager))==15)gabi::call(0x025F7DB0,gabi::at<void>(manager),(u32)mMsgNo,0);
        }
    } else if(status==21) { if(mode==2)return TRUE; }
    else if(status!=6&&status==18) {gabi::call(0x025F74D0,gabi::at<void>(manager),19);return TRUE;}
    return FALSE;
}
VERIFY(0x0235B924,&daObj_hsh_c::talk);
void daObj_hsh_c::initialMsgSetEvent(s32 staff) {
    WWHD_FUNC(0x0235B7F4,void,this,staff);
    gabi::store<u32>(0x10469FE4,0xFFFFFFFF); mMsgNo=0;
    void* p=gabi::call<void*>(0x0254487C,dComIfGp_getPEvtManager(),staff,STR(0x1002AF24),3);
    if(p) { u32 n=gabi::load<u32>(gabi::ea(p));mMsgNo=n; if(n==0x5B3)gabi::store<u8>(dComIfGp_ea()+0x5BDA,2); }
}
VERIFY(0x0235B7F4,&daObj_hsh_c::initialMsgSetEvent);
void daObj_hsh_c::initialAppearEvent(s32 staff) {
    WWHD_FUNC(0x0235BD54,void,this,staff);
    event_on(0x2B10);particle_set(0x8270);particle_set(&mpEmitter,0x8271);
    sound(this,0x6A05,true); mAppearDeleteTimer=30; action_from(this,0x1002AD90);
}
VERIFY(0x0235BD54,&daObj_hsh_c::initialAppearEvent);
s32 daObj_hsh_c::actionAppearEvent(s32 staff) {
    WWHD_FUNC(0x0235BE08,s32,this,staff);
    if(gabi::call<u8>(0x0207A9A0,&mAppearDeleteTimer)==0) {offOffDraw();emitterDelete(&mpEmitter);return TRUE;}
    return FALSE;
}
VERIFY(0x0235BE08,&daObj_hsh_c::actionAppearEvent);
void daObj_hsh_c::initialDeleteEvent(s32 staff) {
    WWHD_FUNC(0x0235BE70,void,this,staff);
    particle_set(0x8270);particle_set(&mpEmitter,0x8271);
    sound(this,0x6A05,true);mAppearDeleteTimer=60;action_from(this,0x1002ADA8);
}
VERIFY(0x0235BE70,&daObj_hsh_c::initialDeleteEvent);
s32 daObj_hsh_c::actionDeleteEvent(s32 staff) {
    WWHD_FUNC(0x0235BF0C,s32,this,staff);
    if(gabi::call<u8>(0x0207A9A0,&mAppearDeleteTimer)==0) {drawStop();return TRUE;}
    return FALSE;
}
VERIFY(0x0235BF0C,&daObj_hsh_c::actionDeleteEvent);
bool daObj_hsh_c::chkAttention(cXyz* pos,s16 angle) {
    WWHD_FUNC(0x0235C018,bool,this,pos,angle);
    u32 play=dComIfGp_ea(),player=gabi::load<u32>(play+0x5B2C);
    f32 z=gabi::load<f32>(player+0x31C)-(f32)pos->z,x=gabi::load<f32>(player+0x314)-(f32)pos->x;
    f32 dist=gabi::load<f32>(0x1046A008);s32 range=gabi::load<s16>(0x1046A014);
    f32 d=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
    s16 target=gabi::call<s16>(0x020195B0,x,z);
    if(mAttentionLatch) {dist+=40.0f;range+=0x71C;}
    s16 diff=(s16)((s32)target-angle);s32 magnitude=diff<0?-(s32)diff:(s32)diff;
    return range>magnitude&&dist>d;
}
VERIFY(0x0235C018,&daObj_hsh_c::chkAttention);
s32 daObj_hsh_c::waitAction(void* arg) {
    WWHD_FUNC(0x0235C144,s32,this,arg);
    s32 mode=mActionMode;
    if(mode==0)mActionMode=1;
    else if(mode!=-1) {
        gabi::Local<cXyz> pos;pos->x=current.pos.x;pos->y=current.pos.y;pos->z=current.pos.z;
        s32 attentive=gabi::call<s32>(0x0235C018,this,pos.get(),(s16)shape_angle.y);
        u32 flags=gabi::load<u32>(gabi::ea(this)+0x39C);
        mAttentionLatch=attentive;
        if(attentive) {
            s8 order=mOrder;u32 type=argument;
            gabi::store<u32>(gabi::ea(this)+0x39C,flags|0x20000008);
            if(order==-1)mOrder=type==0?5:3;
        } else gabi::store<u32>(gabi::ea(this)+0x39C,flags&~0x20000008u);
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0235C144,&daObj_hsh_c::waitAction);
s32 daObj_hsh_c::talkAction(void* arg) {
    WWHD_FUNC(0x0235C27C,s32,this,arg);
    s32 mode=mActionMode;
    if(mode==0) {
        gabi::store<u32>(0x10469FE4,0xFFFFFFFF);
        u32 msg=getMsg();u8 next=(u8)((u8)mActionMode+1);s8 type=argument;
        mMsgNo=msg;mActionMode=next;
        if(type==0) {u32 play=dComIfGp_ea(),player=gabi::load<u32>(play+0x5B34);gabi::store<u32>(player+0x3B8,gabi::load<u32>(player+0x3B8)|0x08000000);}
    } else if(mode!=-1) {
        if(mode==1) {if(talk_init())mActionMode=(u8)((u8)mActionMode+1);}
        else if(talk(0)) {
            action_from(this,0x1002AD90);dComIfGp_event_reset();
            if(argument==0) {u32 play=dComIfGp_ea(),player=gabi::load<u32>(play+0x5B34);gabi::store<u32>(player+0x3B8,gabi::load<u32>(player+0x3B8)&~0x08000000u);}
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0235C27C,&daObj_hsh_c::talkAction);
void daObj_hsh_c::setBaseMtx() {
    WWHD_FUNC(0x0235AC8C,void,this);
    J3DModel* model=mpModel;
    mDoMtx_stack_c::transS(current.pos.x,current.pos.y,current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(),shape_angle.y);
    J3DModel_setBaseTRMtx(model,mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(),&mMtx);
}
VERIFY(0x0235AC8C,&daObj_hsh_c::setBaseMtx);
s32 daObj_hsh_c::execute() {
    WWHD_FUNC(0x0235AD5C,s32,this);
    if(argument==0) {
        bool on=event_bit(0x2B10);u32 status=actor_status;
        actor_status=on?((status&~63u)|0x24):(status&~0x20u);
    }
    u32 play=dComIfGp_ea();mObjAcch.CrrPos(gabi::at<dBgS>(play+0x12A0));
    if(mObjAcch.GetGroundH()!=-1000000000.0f) {
        play=dComIfGp_ea();s32 room=gabi::call<s32>(0x024EF130,gabi::at<void>(play+0x12A0),gabi::at<void>(gabi::ea(this)+0x4A0));
        gabi::store<u8>(gabi::ea(this)+0x1C9,room);current.roomNo=room;
        play=dComIfGp_ea();u8 color=gabi::call<u8>(0x024EEEB8,gabi::at<void>(play+0x12A0),gabi::at<void>(gabi::ea(this)+0x4A0));
        gabi::store<u8>(gabi::ea(this)+0x1CA,color);
        u32 b=gabi::ea(this),v=gabi::load<u32>(b+0x4A8),p=gabi::load<u32>(b+0x4A4);
        gabi::store<u32>(b+0x608,v);gabi::store<u32>(b+0x604,p);
        u16 a=gabi::load<u16>(b+0x4A0),c=gabi::load<u16>(b+0x4A2);
        gabi::store<u16>(b+0x600,a);gabi::store<u16>(b+0x602,c);
    }
    if(eventProc()==0) {checkOrder();action(nullptr);}
    eventOrder();setBaseMtx();return TRUE;
}
VERIFY(0x0235AD5C,&daObj_hsh_c::execute);
static s32 execute_actor(daObj_hsh_c* a) { WWHD_FUNC(0x0235AE94,s32,a); return a->execute(); }
VERIFY(0x0235AE94,execute_actor);
static s32 isdelete_actor(daObj_hsh_c* a) { WWHD_FUNC(0x0235AE98,s32,a); return TRUE; }
VERIFY(0x0235AE98,isdelete_actor);
static s32 delete_actor(daObj_hsh_c* a) { WWHD_FUNC(0x0235AEA0,s32,a); return TRUE; }
VERIFY(0x0235AEA0,delete_actor);
s32 daObj_hsh_c::createHeap() {
    WWHD_FUNC(0x0235A41C,s32,this);
    bool second=argument!=0; const char* arc=STR(second?0x1002AE7C:0x1002AE74);
    auto* data=(J3DModelData*)dComIfG_getObjectRes(arc,4,0x1002ADF4);
    if(!data)JUT_ASSERT_fail(STR(0x1002AE84),second?0x211:0x1FB,STR(0x1002AE98));
    J3DModel* model=mDoExt_J3DModel__create(data,0,0x11020203);mpModel=model;
    if(!model)return FALSE;
    dBgW* bg=gabi::call<dBgW*>(0x024F23F4,(u32)0);mpBgW=bg;
    if(!bg)return FALSE;
    void* dzb=dComIfG_getObjectRes(arc,7,0x1002ADF4);
    return gabi::call<s32>(0x0200A030,(dBgW*)mpBgW,dzb,1,&mMtx)==0;
}
VERIFY(0x0235A41C,&daObj_hsh_c::createHeap);
static s32 check_heap(daObj_hsh_c* a) { WWHD_FUNC(0x0235A5BC,s32,a); return a->createHeap(); }
VERIFY(0x0235A5BC,check_heap);
s32 daObj_hsh_c::init() {
    WWHD_FUNC(0x0235AEF8,s32,this);
    mEventSelector=-1;mAttentionLatch=0;mOrder=-1;
    u32 prm=mParameters;mSwitchNo=prm&255;mPrmMsgNo=(prm>>8)&65535;
    mAcchCir.SetWall(30,30);
    mObjAcch.Set(&current.pos,&old.pos,this,1,&mAcchCir,&speed,nullptr,nullptr);
    setBaseMtx();u32 play=dComIfGp_ea();
    gabi::call(0x024EEA6C,gabi::at<void>(play+0x12A0),(dBgW*)mpBgW,this);
    gabi::call(0x024F43DC,(dBgW*)mpBgW);
    if(argument==0&&!event_bit(0x2B10)) {onOffDraw();action_from(this,0x1002AD98);}
    else action_from(this,0x1002AD90);
    for(u32 i=0;i<2;i++) {
        u32 name=gabi::load<u32>(0x101CA1D4+i*4);
        s16 event=gabi::call<s16>(0x02543F10,dComIfGp_getPEvtManager(),gabi::at<void>(name),255);
        mEventId[i]=event;
    }
    gabi::store<u32>(gabi::ea(this)+0x104,0x0235A3B4);
    gabi::store<u32>(gabi::ea(this)+0x100,0x0235A418);
    return TRUE;
}
VERIFY(0x0235AEF8,&daObj_hsh_c::init);
s32 daObj_hsh_c::create() {
    WWHD_FUNC(0x0235B090,s32,this);
    u32 condition=actor_condition;
    if(!(condition&8)) {
        if(gabi::ea(this)) {
            gabi::call(0x025D4ED0,this);__vtbl=0x1002AF70;
            dBgS_ObjAcch_ct(&mObjAcch,{0x1002AE1C,0x1002AE3C,0x1002AE2C});
            dBgS_AcchCir_ct(&mAcchCir);
            u32 b=gabi::ea(this);gabi::store<u32>(b+0x604,0);gabi::store<u16>(b+0x600,65535);
            gabi::store<u32>(b+0x60C,0x1002AE0C);gabi::store<u16>(b+0x602,256);gabi::store<u32>(b+0x608,0xFFFFFFFF);
            condition=actor_condition;
        }
        actor_condition=condition|8;
    }
    if(argument==0&&event_bit(0x2510))return 5;
    const char* arc=STR(argument==0?0x1002AEC8:0x1002AED0);
    s32 phase=dComIfG_resLoad(&mPhase,arc);
    if(phase==4) {
        if(!fopAcM_entrySolidHeap(this,0x0235A5BC,0x4000)) {mpBgW=nullptr;return 5;}
        cullMtx=gabi::ea(J3DModel_getBaseTRMtx(mpModel));
        if(gabi::load<s8>(0x1046A004)<0) {
            const char* name=STR(argument==0?0x1002AED8:0x1002AEE4);
            s8 no=gabi::call<s8>(0x025F0A10,name,gabi::at<void>(0x1046A004));
            gabi::store<u32>(0x1046A018,gabi::ea(this));gabi::store<s8>(0x1046A004,no);
        }
        if(init()==0)phase=5;
    }
    return phase;
}
VERIFY(0x0235B090,&daObj_hsh_c::create);
static s32 create_actor(daObj_hsh_c* a) { WWHD_FUNC(0x0235B2B0,s32,a); return a->create(); }
VERIFY(0x0235B2B0,create_actor);
void daObj_hsh_c::dt(daObj_hsh_c* a,s32 flags) {
    WWHD_FUNC(0x0235B2F0,void,a,flags);
    if(a) {
        const char* arc=STR(a->argument==0?0x1002AEF4:0x1002AEFC);a->__vtbl=0x1002AF70;
        dComIfG_resDelete(&a->mPhase,arc);
        if(a->mpBgW) {u32 play=dComIfGp_ea();gabi::call(0x020087EC,gabi::at<void>(play+0x12A0),(dBgW*)a->mpBgW);}
        a->emitterDelete(&a->mpEmitter);
        s8 child=gabi::load<s8>(0x1046A004);
        if(child>=0) {gabi::call(0x025F0A18,child);gabi::store<s8>(0x1046A004,-1);}
        u32 b=gabi::ea(a);gabi::call(0x02018034,gabi::at<void>(b+0x590),2);
        gabi::store<u32>(b+0x3D8,0x1002AE2C);gabi::store<u32>(b+0x3CC,0x1002AE3C);
        gabi::call(0x024EFD9C,&a->mObjAcch,0);gabi::call(0x025D50BC,a,0);
        if(flags&1)operator_delete(a);
    }
}
VERIFY(0x0235B2F0,daObj_hsh_c::dt);
static void* hio_ct(void* p) {
    WWHD_FUNC(0x0235C410,void*,p);
    if(!p) {p=operator_new(0x1C);if(!p)return p;}
    u32 b=gabi::ea(p);gabi::store<s8>(b,-1);gabi::store<u32>(b+0x18,0x1002AE4C);
    for(u32 i=0;i<4;i++)gabi::store<u32>(b+4+i*4,gabi::load<u32>(0x1002AF50+i*4));
    return p;
}
VERIFY(0x0235C410,hio_ct);
static void sinit() {
    WWHD_FUNC(0x0235C47C,void,(u32)0);
    for(u32 i=0;i<4;i++)gabi::store<u32>(0x10469FF4+i*4,0);
    __register_global_object(0x101CA29C);
    gabi::store<f32>(0x10469FE8,-3.1415927f);gabi::store<f32>(0x10469FEC,3.1415927f);
    gabi::call(0x028ED6F8,(u32)0x10469FF0);__register_global_object(0x101CA2A8);
    gabi::call(0x028EAB2C,(u32)0x10469FF1);__register_global_object(0x101CA2B4);
    hio_ct(gabi::at<void>(0x1046A004));
}
VERIFY(0x0235C47C,sinit);
static void hio_dt(void* p,s32 flags) { WWHD_FUNC(0x0235C51C,void,p,flags);if(p&&(flags&1))operator_delete(p); }
VERIFY(0x0235C51C,hio_dt);
static void hio_empty(void* p) { WWHD_FUNC(0x0235C530,void,p); }
VERIFY(0x0235C530,hio_empty);
static bool hsh_streq(u32 a,u32 b) {
    u8 left,right;
    do { left=gabi::load<u8>(a++); right=gabi::load<u8>(b++); } while(left==right && left!=0);
    return left==right;
}
void daObj_hsh_c::initialLinkDispEvent(s32 staff) {
    WWHD_FUNC(0x0235B48C,void,this,staff);
    /* Original buffer is sp-0x20; Local slots align to 16 bytes. */
    gabi::Local<u8[0x20]> frame;
    u32 buf=gabi::ea(frame.get());
    u32 target=gabi::call<u32>(0x0254487C,gabi::at<void>(dComIfGp_ea()+0x52C4),staff,STR(0x1002AF10),4);
    bool player=false;
    if(target) { gabi::call(0x028F040C,buf,target); player=hsh_streq(buf,0x1002AF04); }
    u32 disp=gabi::call<u32>(0x0254487C,gabi::at<void>(dComIfGp_ea()+0x52C4),staff,STR(0x1002AF18),4);
    if(!disp)return;
    gabi::call(0x028F040C,buf,disp);
    if(player) {
        u32 link=gabi::load<u32>(dComIfGp_ea()+0x5B34);
        if(hsh_streq(buf,0x1002AF20))gabi::store<u32>(link+0x3B8,gabi::load<u32>(link+0x3B8)&~0x08000000u);
        if(hsh_streq(buf,0x1002AF0C))gabi::store<u32>(link+0x3B8,gabi::load<u32>(link+0x3B8)|0x08000000u);
    } else {
        if(hsh_streq(buf,0x1002AF20))drawStart();
        if(hsh_streq(buf,0x1002AF0C))drawStop();
    }
}
VERIFY(0x0235B48C,&daObj_hsh_c::initialLinkDispEvent);
