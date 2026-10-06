/* WWHD wooden and metal bars. Derived game code: */
#include "bindings.h"
namespace Mdoor {
struct Act : fopAc_ac_c {
    u8 smoke[0x20]; request_of_phase_process_class phase;
    gptr<J3DModel> model; gptr<dBgW> background;
    be<s32> staff; be<s16> timer,event;
    be<u8> tool,action; be<u16> flags; be<f32> height;
};
WWHD_OFFSET(Act,phase,0x3CC);
WWHD_OFFSET(Act,model,0x3D4);
WWHD_OFFSET(Act,staff,0x3DC);
WWHD_OFFSET(Act,height,0x3E8);
WWHD_SIZE(Act,0x3EC);
static u8 getShape(Act* actor) { WWHD_FUNC(0x021C0EA0,u8,actor); return actor->mParameters>>24; }
VERIFY(0x021C0EA0,getShape);
static void calcMtx(Act* actor) {
    WWHD_FUNC(0x021C0EAC,void,actor);
    f32 height=actor->height,y=actor->current.pos.y,x=actor->current.pos.x,z=actor->current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(),x,y+height,z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(),actor->home.angle.y);
    f32 matrix[12]; for(u32 i=0;i<12;++i) matrix[i]=gabi::load<f32>(0x1048D0CC+i*4);
    u32 model=gabi::ea(actor->model);
    for(u32 i=0;i<12;++i) gabi::store<f32>(model+0xC8+i*4,matrix[i]);
}
VERIFY(0x021C0EAC,calcMtx);
// SafeString payload remains above every nonleaf caller linkage save.
static void* objectResource(s32 index) {
    gabi::Local<SafeString> name; gabi::Local<u8[16]> callerLinkage;
    name->__vtbl=0x100148F4; name->mStringTop=0x100149E0;
    return gabi::call<void*>(0x026066C4,dComIfG_resControl(),name.get(),index);
}
static BOOL createHeap(Act* actor) {
    WWHD_FUNC(0x021C0F70,BOOL,actor);
    bool second=getShape(actor)==1; s32 collisionIndex=second?9:8;
    void* data=objectResource(second?5:4);
    if(!data) JUT_ASSERT_fail(STR(0x1001491C),0x90,STR(0x1001492C));
    actor->model=gabi::call<J3DModel*>(0x025E38E0,data,0,0x11020203);
    if(!gabi::ea(actor->model)) return FALSE;
    actor->background=gabi::call<dBgW*>(0x024F23F4,(u32)0);
    if(!gabi::ea(actor->background)) return FALSE;
    data=objectResource(collisionIndex);
    if(!data) return FALSE;
    calcMtx(actor);
    J3DModel* model=actor->model; dBgW* background=actor->background;
    return gabi::call<BOOL>(0x0200A030,background,data,1,J3DModel_getBaseTRMtx(model))?FALSE:TRUE;
}
VERIFY(0x021C0F70,createHeap);
static BOOL heapCB(Act* actor) { WWHD_FUNC(0x021C10B8,BOOL,actor); return createHeap(actor); }
VERIFY(0x021C10B8,heapCB);
static BOOL actionWait(Act* actor) { WWHD_FUNC(0x021C10BC,BOOL,actor); return TRUE; }
VERIFY(0x021C10BC,actionWait);
static void smokeInit(Act* actor) {
    WWHD_FUNC(0x021C10C4,void,actor);
    s8 room=actor->current.roomNo; auto play=dComIfGp_get();
    u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
    u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(control),0,0x2022,&actor->current.pos,&actor->shape_angle,(u32)0,0xAA,gabi::at<void>(gabi::ea(actor)+0x3AC),room,0,0,0);
    if(emitter) {
        gabi::store<f32>(emitter+0x34,16.0f); gabi::store<f32>(emitter+0x58,0.35f);
        for(u32 i=0;i<3;++i) { gabi::store<f32>(emitter+0x220+i*4,2.0f); gabi::store<f32>(emitter+0x238+i*4,2.0f); }
    }
}
VERIFY(0x021C10C4,smokeInit);
static void debris(Act* actor) {
    dComIfGp_particle_set(0x81B2,&actor->current.pos,&actor->current.angle,nullptr,0xFF,nullptr,-1);
}
static void sound(Act* actor,u32 id,bool checkNull=false) {
    if(checkNull && (!gabi::ea(actor) || !(u32)(gabi::ea(actor)+0x37C))) return;
    s8 room=actor->current.roomNo; s32 reverb=dComIfGp_getReverb(room);
    mDoAud_seStart(id,&actor->eyePos,0,reverb);
}
static void cutEnd(Act* actor) { s32 staff=actor->staff; auto manager=dComIfGp_getPEvtManager(); gabi::call(0x02543280,manager,staff); }
static void move(Act* actor) { calcMtx(actor); gabi::call(0x024F43DC,(dBgW*)actor->background); }
static void demoProc(Act* actor) {
    WWHD_FUNC(0x021C1174,void,actor);
    s32 staff=actor->staff; auto manager=dComIfGp_getPEvtManager();
    u32 action=gabi::call<u32>(0x02542EDC,manager,staff,gabi::at<void>(0x101BA3FC),3,0,0);
    staff=actor->staff; manager=dComIfGp_getPEvtManager();
    if(gabi::call<BOOL>(0x025447C8,manager,staff)) {
        if(action==1) { u16 flags=actor->flags; actor->speedF=0.0f; actor->flags=flags|1; sound(actor,0x697C); }
        else if(action==2) { actor->speedF=0.0f; if(getShape(actor)!=1) debris(actor); sound(actor,0x697D); }
    }
    if(action==1) {
        cLib_chaseF(&actor->speedF,60.0f,6.0f);
        if(cLib_chaseF(&actor->height,0.0f,actor->speedF)) {
            cutEnd(actor); u16 flags=actor->flags;
            if(flags&1) { actor->flags=flags&0xFFFE; if(getShape(actor)!=1) debris(actor); smokeInit(actor); }
        }
        move(actor);
    } else if(action==2) {
        cLib_chaseF(&actor->speedF,30.0f,3.0f);
        if(cLib_chaseF(&actor->height,280.0f,actor->speedF)) cutEnd(actor);
        move(actor);
    } else cutEnd(actor);
}
VERIFY(0x021C1174,demoProc);
static BOOL actionDemoWait(Act* actor) {
    WWHD_FUNC(0x021C1418,BOOL,actor);
    s16 event=actor->event; auto manager=dComIfGp_getPEvtManager();
    if(gabi::call<BOOL>(0x0254407C,manager,event)) {
        actor->action=(u8)((u8)actor->action+1);
        manager=dComIfGp_getPEvtManager(); actor->staff=gabi::call<s32>(0x02542D88,manager,STR(0x10014980),0,0); demoProc(actor);
    }
    return TRUE;
}
VERIFY(0x021C1418,actionDemoWait);
static BOOL actionDemo(Act* actor) {
    WWHD_FUNC(0x021C149C,BOOL,actor);
    s16 event=actor->event; auto manager=dComIfGp_getPEvtManager();
    if(gabi::call<BOOL>(0x025440C8,manager,event)) actor->action=(u8)((u8)actor->action+1);
    else demoProc(actor);
    return TRUE;
}
VERIFY(0x021C149C,actionDemo);
static u8 getType(Act* actor) { WWHD_FUNC(0x021C1508,u8,actor); return (actor->mParameters>>8)&0xFF; }
VERIFY(0x021C1508,getType);
static void order(Act* actor) { s16 event=actor->event; u8 tool=actor->tool; gabi::call(0x025D7A58,actor,event,tool,0xFFFF,0,1); }
static BOOL actionGenocide(Act* actor) {
    WWHD_FUNC(0x021C1514,BOOL,actor);
    if(!gabi::call<u32>(0x025D98E8,(s8)actor->current.roomNo)) {
        s16 timer=actor->timer;
        if(timer>0) actor->timer=(s16)(timer-1);
        else { if(getType(actor)==1) gabi::call(0x025E1928); order(actor); actor->action=(u8)((u8)actor->action+1); }
    }
    return TRUE;
}
VERIFY(0x021C1514,actionGenocide);
static void resetEvent() { auto play=dComIfGp_get(); u32 address=gabi::ea(play)+0x52B8; gabi::store<u16>(address,gabi::load<u16>(address)|8); }
static BOOL actionOpen(Act* actor) {
    WWHD_FUNC(0x021C15B0,BOOL,actor);
    s16 event=actor->event; auto manager=dComIfGp_getPEvtManager();
    if(gabi::call<BOOL>(0x025440C8,manager,event)) { actor->action=(u8)((u8)actor->action+1); resetEvent(); return TRUE; }
    s16 timer=actor->timer;
    if(timer<0) return TRUE;
    if(timer>0) {
        timer=(s16)(timer-1); actor->timer=timer;
        if(timer) return TRUE;
        actor->speedF=0.0f; if(getShape(actor)!=1) debris(actor); sound(actor,0x697D,true);
        if((s16)actor->timer!=0) return TRUE;
    }
    cLib_chaseF(&actor->speedF,30.0f,3.0f); cLib_chaseF(&actor->height,280.0f,actor->speedF); move(actor);
    return TRUE;
}
VERIFY(0x021C15B0,actionOpen);
static u8 getSwitch(Act* actor) { WWHD_FUNC(0x021C1718,u8,actor); return (u8)actor->mParameters; }
VERIFY(0x021C1718,getSwitch);
static BOOL actionSwitch(Act* actor) {
    WWHD_FUNC(0x021C1724,BOOL,actor);
    u8 sw=getSwitch(actor); s8 room=actor->current.roomNo; u32 save=gabi::load<u32>(0x101F84DC);
    if(gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),sw,room)) { order(actor); actor->action=(u8)((u8)actor->action+1); }
    return TRUE;
}
VERIFY(0x021C1724,actionSwitch);
static BOOL actionReady(Act* actor) {
    WWHD_FUNC(0x021C17A4,BOOL,actor);
    if(gabi::load<u16>(gabi::ea(actor)+0xF8)==2) {
        auto manager=dComIfGp_getPEvtManager(); actor->staff=gabi::call<s32>(0x02542D88,manager,STR(0x10014988),0,0); demoProc(actor);
        actor->action=(u8)((u8)actor->action+1); u8 sw=getSwitch(actor);
        if(sw!=0xFF) { u32 save=gabi::load<u32>(0x101F84DC); gabi::call(0x025BA0C0,gabi::at<void>(save+0x20),sw,(s8)actor->current.roomNo); }
        if(getType(actor)==1) { u32 save=gabi::load<u32>(0x101F84DC); gabi::call(0x025B8B68,gabi::at<void>(save+0x644),0x1140); }
    } else order(actor);
    return TRUE;
}
VERIFY(0x021C17A4,actionReady);
static BOOL actionReadyOpen(Act* actor) {
    WWHD_FUNC(0x021C188C,BOOL,actor);
    if(gabi::load<u16>(gabi::ea(actor)+0xF8)==2) {
        actor->action=(u8)((u8)actor->action+1); u8 sw=getSwitch(actor);
        if(sw!=0xFF) { u32 save=gabi::load<u32>(0x101F84DC); gabi::call(0x025BA0C0,gabi::at<void>(save+0x20),sw,(s8)actor->current.roomNo); }
        actor->timer=20;
    } else order(actor);
    return TRUE;
}
VERIFY(0x021C188C,actionReadyOpen);
static BOOL actionEvent(Act* actor) {
    WWHD_FUNC(0x021C1924,BOOL,actor);
    s16 event=actor->event; auto manager=dComIfGp_getPEvtManager();
    if(gabi::call<BOOL>(0x025440C8,manager,event)) { actor->action=(u8)((u8)actor->action+1); resetEvent(); }
    else demoProc(actor);
    return TRUE;
}
VERIFY(0x021C1924,actionEvent);
static BOOL draw(Act* actor) {
    WWHD_FUNC(0x021C19A0,BOOL,actor);
    auto light=dKy_getEnvlight(); settingTevStruct(light,1,&actor->current.pos,&actor->tevStr);
    light=dKy_getEnvlight(); setLightTevColorType(light,actor->model,&actor->tevStr); mDoExt_modelUpdateDL(actor->model); return TRUE;
}
VERIFY(0x021C19A0,draw);
static void smokeEnd(Act* actor) { WWHD_FUNC(0x021C19FC,void,actor); gabi::call(0x025A5F88,gabi::at<void>(gabi::ea(actor)+0x3AC)); }
VERIFY(0x021C19FC,smokeEnd);
static BOOL execute(Act* actor) {
    WWHD_FUNC(0x021C1A04,BOOL,actor);
    smokeEnd(actor);
    if(!gabi::load<u32>(0x101FDAF8)) { gabi::store<u32>(0x101FDAF8,1); memcpy_g(gabi::at<void>(0x101FDAFC),gabi::at<void>(0x101BA39C),64); }
    u8 action=actor->action; gabi::call_ptr(gabi::load<u32>(0x101FDAFC+(u32)action*4),actor); return TRUE;
}
VERIFY(0x021C1A04,execute);
static BOOL isDelete(Act* actor) { WWHD_FUNC(0x021C1A88,BOOL,actor); return TRUE; }
VERIFY(0x021C1A88,isDelete);
static BOOL remove(Act* actor) {
    WWHD_FUNC(0x021C1A90,BOOL,actor);
    if(actor->heap) { auto space=dComIfG_Bgsp(); gabi::call(0x020087EC,space,(dBgW*)actor->background); }
    smokeEnd(actor); dComIfG_resDelete(&actor->phase,STR(0x100149E0)); return TRUE;
}
VERIFY(0x021C1A90,remove);
static u8 getTool(Act* actor) { WWHD_FUNC(0x021C1AF0,u8,actor); return (actor->mParameters>>16)&0xFF; }
VERIFY(0x021C1AF0,getTool);
static void raiseAttention(Act* actor) {
    f32 attention=gabi::load<f32>(gabi::ea(actor)+0x394),eye=actor->eyePos.y;
    gabi::store<f32>(gabi::ea(actor)+0x394,attention+150.0f); actor->eyePos.y=eye+150.0f;
}
static BOOL createInit(Act* actor) {
    WWHD_FUNC(0x021C1AFC,BOOL,actor);
    u8 sw=getSwitch(actor),type=getType(actor); auto space=dComIfG_Bgsp();
    if(gabi::call<BOOL>(0x024EEA6C,space,(dBgW*)actor->background,actor)) JUT_ASSERT_fail(STR(0x100149AC),0xEC,STR(0x1001499C));
    s8 room=actor->current.roomNo; actor->height=0.0f; actor->tevStr.mRoomNo=room; actor->tool=0xFF;
    if(type==0) {
        u32 save=gabi::load<u32>(0x101F84DC); bool done=gabi::call<BOOL>(0x025B8B94,gabi::at<void>(save+0x644),0x1101);
        if(done) actor->action=3; else { actor->height=280.0f; actor->action=1; }
        auto manager=dComIfGp_getPEvtManager(); s16 event=gabi::call<s16>(0x02543F10,manager,STR(0x100149A0),0xFF); actor->event=event;
    } else if(type==1) {
        u32 save=gabi::load<u32>(0x101F84DC);
        if(gabi::call<BOOL>(0x025B8B94,gabi::at<void>(save+0x644),0x1101)) { actor->action=7; actor->height=280.0f; }
        else {
            save=gabi::load<u32>(0x101F84DC); gabi::call(0x025B8B7C,gabi::at<void>(save+0x644),0x1140); actor->action=4;
            auto manager=dComIfGp_getPEvtManager(); s16 event=gabi::call<s16>(0x02543F10,manager,STR(0x100149BC),0xFF); actor->timer=1; actor->event=event;
        }
    } else if(type==2) {
        bool done=sw==0xFF;
        if(!done) { u32 save=gabi::load<u32>(0x101F84DC); done=gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),sw,(s8)actor->current.roomNo); }
        if(done) { actor->action=11; actor->height=280.0f; }
        else {
            actor->tool=getTool(actor); u8 tool=getTool(actor); auto manager=dComIfGp_getPEvtManager();
            s16 event=gabi::call<s16>(0x02543F10,manager,STR(0x100149BC),tool); u8 savedTool=actor->tool; actor->event=event; actor->action=savedTool==0xFF?8:12;
        }
    } else actor->action=0;
    raiseAttention(actor); move(actor); return TRUE;
}
VERIFY(0x021C1AFC,createInit);
static s32 create(Act* actor) {
    WWHD_FUNC(0x021C1E8C,s32,actor);
    s32 phase=dComIfG_resLoad(&actor->phase,STR(0x100149E0)); u32 condition=actor->actor_condition;
    if(!(condition&8)) {
        if(actor) { gabi::call(0x025D4ED0,actor); actor->__vtbl=0x1001490C; gabi::call(0x025A5B18,gabi::at<void>(gabi::ea(actor)+0x3AC),1); condition=actor->actor_condition; }
        actor->actor_condition=condition|8;
    }
    if(phase!=4) return phase;
    if(!fopAcM_entrySolidHeap(actor,0x021C10B8,0x1640)) return 5;
    createInit(actor); return 4;
}
VERIFY(0x021C1E8C,create);
static s32 createWrapper(Act* actor) { WWHD_FUNC(0x021C1F60,s32,actor); return create(actor); }
VERIFY(0x021C1F60,createWrapper);
static void sinit() { WWHD_FUNC(0x021C1F64,void,(u32)0); sinit_header_statics_z(0x10464FA0,0x101BA408,0x10464FAC); }
VERIFY(0x021C1F64,sinit);
static void stringDtor(void* p,s32 flags) { WWHD_FUNC(0x021C1FF8,void,p,flags); if(p && (flags&1)) operator_delete(p); }
VERIFY(0x021C1FF8,stringDtor);
static void actorDtor(Act* actor,s32 flags) { WWHD_FUNC(0x021C200C,void,actor,flags); if(actor) { gabi::call(0x025D50BC,actor,0); if(flags&1) operator_delete(actor); } }
VERIFY(0x021C200C,actorDtor);
static void stringTerminate(void* p) { WWHD_FUNC(0x021C2060,void,p); }
VERIFY(0x021C2060,stringTerminate);
}
