/* WWHD double doors. Derived game code: */
#include "bindings.h"
namespace Mbdoor {
struct Act : fopAc_ac_c {
    request_of_phase_process_class phase;
    gptr<J3DModel> frame,left,right,bar;
    gptr<dBgW> background;
    be<u8> registered,side,action; u8 pad3CB;
    be<s16> barAngle,doorAngle,angularSpeed;
    be<u8> opening; u8 pad3D3;
    be<s16> adjustmentTimer; u8 pad3D6[2];
    be<s32> enemyTimer,staff;
    cXyz playerGoal,goal,goal2;
};
WWHD_OFFSET(Act,phase,0x3AC); WWHD_OFFSET(Act,frame,0x3B4);
WWHD_OFFSET(Act,background,0x3C4); WWHD_OFFSET(Act,barAngle,0x3CC);
WWHD_OFFSET(Act,staff,0x3DC); WWHD_OFFSET(Act,goal2,0x3F8);
WWHD_SIZE(Act,0x404);
static u8 getShape(Act* a) { WWHD_FUNC(0x021BF70C,u8,a); return (a->mParameters>>8)&15; }
VERIFY(0x021BF70C,getShape);
static const char* getArc(Act* a) { WWHD_FUNC(0x021BF718,const char*,a); return STR(getShape(a)==1?0x10014748:0x10014750); }
VERIFY(0x021BF718,getArc);
static u32 getFrameResource(Act* a) { WWHD_FUNC(0x021BF754,u32,a); return 6; }
VERIFY(0x021BF754,getFrameResource);
static u32 getCollisionResource(Act* a) { WWHD_FUNC(0x021BF75C,u32,a); return 10; }
VERIFY(0x021BF75C,getCollisionResource);
static u32 getLeftResource(Act* a) { WWHD_FUNC(0x021BF764,u32,a); return 4; }
VERIFY(0x021BF764,getLeftResource);
static u32 getRightResource(Act* a) { WWHD_FUNC(0x021BF76C,u32,a); return 5; }
VERIFY(0x021BF76C,getRightResource);
static u32 getBarResource(Act* a) { WWHD_FUNC(0x021BF774,u32,a); return 7; }
VERIFY(0x021BF774,getBarResource);
static f32 rightOffset(Act* a) { WWHD_FUNC(0x021BF77C,f32,a); return getShape(a)==1?300.0f:277.0f; }
VERIFY(0x021BF77C,rightOffset);
static f32 leftOffset(Act* a) { WWHD_FUNC(0x021BF7C4,f32,a); return getShape(a)==1?-300.0f:-255.0f; }
VERIFY(0x021BF7C4,leftOffset);
static f32 barOffset(Act* a) { WWHD_FUNC(0x021BF80C,f32,a); return getShape(a)==1?335.0f:300.0f; }
VERIFY(0x021BF80C,barOffset);
static void copyMatrix(J3DModel* model) {
    f32 matrix[12]; for(u32 i=0;i<12;++i) matrix[i]=gabi::load<f32>(0x1048D0CC+4*i);
    for(u32 i=0;i<12;++i) gabi::store<f32>(gabi::ea(model)+0xC8+4*i,matrix[i]);
}
static void calcMtx(Act* a) {
    WWHD_FUNC(0x021BF854,void,a);
    gabi::Local<cXyz> offset; gabi::Local<u8[16]> linkage;
    f32 x=a->current.pos.x,y=a->current.pos.y,z=a->current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(),x,y,z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(),a->home.angle.y);
    copyMatrix(a->frame);
    offset->set(0.0f,0.0f,-150.0f); PSMTXMultVec(mDoMtx_stack_c::get(),offset.get(),&a->goal);
    offset->set(0.0f,0.0f,-400.0f); PSMTXMultVec(mDoMtx_stack_c::get(),offset.get(),&a->goal2);
    if((u8)a->side==0) {
        copyMatrix(a->left);
        if((s16)a->doorAngle!=0) {
            f32 dx=rightOffset(a); mDoMtx_stack_transM(dx,0.0f,0.0f);
            mDoMtx_YrotM(mDoMtx_stack_c::get(),a->doorAngle);
            dx=rightOffset(a); mDoMtx_stack_transM(-dx,0.0f,0.0f);
        }
        offset->set(80.0f,0.0f,75.0f); PSMTXMultVec(mDoMtx_stack_c::get(),offset.get(),&a->playerGoal);
    }
    copyMatrix(a->right);
    if((u8)a->side==1) {
        if((s16)a->doorAngle!=0) {
            f32 dx=leftOffset(a); mDoMtx_stack_transM(dx,0.0f,0.0f);
            mDoMtx_YrotM(mDoMtx_stack_c::get(),(s16)-(s16)a->doorAngle);
            dx=leftOffset(a); mDoMtx_stack_transM(-dx,0.0f,0.0f);
        }
        offset->set(-80.0f,0.0f,75.0f); PSMTXMultVec(mDoMtx_stack_c::get(),offset.get(),&a->playerGoal);
        copyMatrix(a->left);
    }
    x=a->current.pos.x; y=a->current.pos.y; z=a->current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(),x,y,z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(),a->home.angle.y);
    if((s16)a->barAngle!=0) {
        f32 dx=barOffset(a); mDoMtx_stack_transM(dx,231.0f,0.0f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(),a->barAngle);
        dx=barOffset(a); mDoMtx_stack_transM(-dx,-231.0f,0.0f);
    }
    copyMatrix(a->bar);
}
VERIFY(0x021BF854,calcMtx);
static void* objectResource(const char* archive,s32 index) {
    gabi::Local<SafeString> name; gabi::Local<u8[16]> linkage;
    name->mStringTop=gabi::ea(archive); name->__vtbl=0x10014770;
    return gabi::call<void*>(0x026066C4,dComIfG_resControl(),name.get(),index);
}
static BOOL createHeap(Act* a) {
    WWHD_FUNC(0x021BFC80,BOOL,a);
    const char* archive=getArc(a); u32 index=getFrameResource(a); void* data=objectResource(archive,index);
    if(!data) JUT_ASSERT_fail(STR(0x100147D4),198,STR(0x100147E4));
    a->frame=gabi::call<J3DModel*>(0x025E38E0,data,0x80000,0x11000022);
    if(!gabi::ea(a->frame)) return FALSE;
    archive=getArc(a); index=getLeftResource(a); data=objectResource(archive,index);
    if(!data) JUT_ASSERT_fail(STR(0x100147D4),209,STR(0x100147E4));
    a->left=gabi::call<J3DModel*>(0x025E38E0,data,0x80000,0x11000022);
    if(!gabi::ea(a->left)) return FALSE;
    archive=getArc(a); index=getRightResource(a); data=objectResource(archive,index);
    if(!data) JUT_ASSERT_fail(STR(0x100147D4),220,STR(0x100147E4));
    a->right=gabi::call<J3DModel*>(0x025E38E0,data,0x80000,0x11000022);
    if(!gabi::ea(a->right)) return FALSE;
    archive=getArc(a); index=getBarResource(a); data=objectResource(archive,index);
    if(!data) JUT_ASSERT_fail(STR(0x100147D4),231,STR(0x100147E4));
    a->bar=gabi::call<J3DModel*>(0x025E38E0,data,0x80000,0x11000022);
    if(!gabi::ea(a->bar)) return FALSE;
    a->background=gabi::call<dBgW*>(0x024F23F4,(u32)0);
    if(!gabi::ea(a->background)) return FALSE;
    archive=getArc(a); index=getCollisionResource(a); data=objectResource(archive,index);
    if(!data) return FALSE;
    calcMtx(a); J3DModel* model=a->frame; dBgW* bg=a->background;
    return gabi::call<BOOL>(0x0200A030,bg,data,1,J3DModel_getBaseTRMtx(model))?FALSE:TRUE;
}
VERIFY(0x021BFC80,createHeap);
static BOOL heapCB(Act* a) { WWHD_FUNC(0x021BFED4,BOOL,a); return createHeap(a); }
VERIFY(0x021BFED4,heapCB);
static BOOL actionWait(Act* a) { WWHD_FUNC(0x021BFED8,BOOL,a); calcMtx(a); return TRUE; }
VERIFY(0x021BFED8,actionWait);
static u8 getSwitch(Act* a) { WWHD_FUNC(0x021BFEFC,u8,a); return (u8)a->mParameters; }
VERIFY(0x021BFEFC,getSwitch);
static u8 getType(Act* a) { WWHD_FUNC(0x021BFF08,u8,a); return (u16)(s16)a->home.angle.z&63; }
VERIFY(0x021BFF08,getType);
static BOOL checkUnlock(Act* a) {
    WWHD_FUNC(0x021BFF14,BOOL,a);
    u8 sw=getSwitch(a),type=getType(a);
    if(type==0) { u32 save=gabi::load<u32>(0x101F84DC); s8 room=a->current.roomNo; return gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),sw,room); }
    if(type==1 && !gabi::call<BOOL>(0x025D98E8,(s8)a->current.roomNo)) {
        s32 timer=a->enemyTimer;
        if(timer>0) { a->enemyTimer=timer-1; return FALSE; }
        if(sw!=0xFF) { u32 save=gabi::load<u32>(0x101F84DC); s8 room=a->current.roomNo; gabi::call(0x025B9E38,gabi::at<void>(save+0x20),sw,room); }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021BFF14,checkUnlock);
static void orderEvent(Act* a,u32 name) { gabi::call(0x025D77DC,a,STR(name),1,0xFFFF); }
static BOOL actionLockWait(Act* a) { WWHD_FUNC(0x021C001C,BOOL,a); if(checkUnlock(a)) { a->action=2; orderEvent(a,0x100147F8); } return TRUE; }
VERIFY(0x021C001C,actionLockWait);
static s32 getDemoAction(Act* a) { WWHD_FUNC(0x021C0078,s32,a); s32 staff=a->staff; auto manager=dComIfGp_getPEvtManager(); return gabi::call<s32>(0x02542EDC,manager,staff,gabi::at<void>(0x101BA324),9,0,0); }
VERIFY(0x021C0078,getDemoAction);
static void cutEnd(Act* a) { s32 staff=a->staff; auto manager=dComIfGp_getPEvtManager(); gabi::call(0x02543280,manager,staff); }
static void sound(Act* a,u32 id) { s8 room=a->current.roomNo; s32 reverb=dComIfGp_getReverb(room); mDoAud_seStart(id,&a->eyePos,0,reverb); }
static void setGoal(cXyz* goal) { auto manager=dComIfGp_getPEvtManager(); gabi::call(0x02543714,manager,goal); }
static void setPlayerGoal(u32 player,cXyz* goal,s16 angle) {
    u32 vt=gabi::load<u32>(player+0xB4),target=gabi::load<u32>(vt+0x114);
    gabi::call_ptr(target,gabi::at<void>(player),goal,angle);
}
static void demoProc(Act* a) {
    WWHD_FUNC(0x021C00C4,void,a);
    gabi::Local<be<s16>> angle; gabi::Local<cXyz> goal; gabi::Local<u8[16]> linkage;
    auto play=dComIfGp_get(); u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    s32 action=getDemoAction(a),staff=a->staff; auto manager=dComIfGp_getPEvtManager();
    if(gabi::call<BOOL>(0x025447C8,manager,staff)) {
        switch(action) {
        case 1: calcMtx(a); goal->copy(a->playerGoal); setGoal(goal.get()); break;
        case 2: *angle=(s16)((s16)a->current.angle.y+0x7FFF); gabi::store<s16>(player+0x422,*angle); break;
        case 4: sound(a,0x2863); a->angularSpeed=0; break;
        case 5: sound(a,0x2864); a->angularSpeed=0; a->opening=1; break;
        case 6: goal->copy(a->goal); setGoal(goal.get()); break;
        case 7: goal->copy(a->goal2); setGoal(goal.get()); break;
        case 8: {
            calcMtx(a); a->adjustmentTimer=0; s32 id=a->staff; auto ev=dComIfGp_getPEvtManager();
            u32 timer=gabi::call<u32>(0x0254487C,ev,id,STR(0x10014870),3);
            if(timer) a->adjustmentTimer=gabi::load<s32>(timer);
            break;
        }
        }
    }
    switch(action) {
    case 3: return;
    case 4: {
        s16 speed=a->angularSpeed,openingAngle=a->doorAngle;
        if(speed<250) { speed=(s16)(speed+50); a->angularSpeed=speed; }
        s32 next=(s32)openingAngle-speed;
        if(next<-0x1C71) { a->doorAngle=-0x1C71; cutEnd(a); }
        else a->doorAngle=(s16)next;
        calcMtx(a); u8 side=a->side; s16 yaw=a->current.angle.y,door=a->doorAngle;
        *angle=(s16)(yaw+0x7FFF+(side?-(s32)door:(s32)door));
        setPlayerGoal(player,&a->playerGoal,*angle); return;
    }
    case 5: {
        if(!(u8)a->opening) { cutEnd(a); return; }
        s16 speed=a->angularSpeed,bar=a->barAngle;
        if(speed<400) { speed=(s16)(speed+40); a->angularSpeed=speed; }
        s32 next=(s32)bar-speed;
        if(next<-0x3F65) { a->barAngle=-0x3F65; cutEnd(a); a->opening=0; sound(a,0x2865); }
        else a->barAngle=(s16)next;
        calcMtx(a); return;
    }
    case 8: {
        s16 yaw=a->current.angle.y; *angle=gabi::load<s16>(player+0x32A);
        gabi::call(0x0200F428,angle.get(),(s16)(yaw+0x7FFF),10,0x800);
        f32 px=gabi::load<f32>(player+0x314),gx=a->playerGoal.x,gz=a->playerGoal.z;
        f32 ax=gx*0.1f,az=gz*0.1f;
        goal->x=px; goal->y=gabi::load<f32>(player+0x318);
        f32 pz=gabi::load<f32>(player+0x31C);
        goal->x=gabi::fmadds(px,0.9f,ax); goal->z=gabi::fmadds(pz,0.9f,az);
        setPlayerGoal(player,goal.get(),*angle);
        s16 timer=a->adjustmentTimer; if(timer>0) a->adjustmentTimer=(s16)(timer-1); else cutEnd(a);
        return;
    }
    default: cutEnd(a); return;
    }
}
VERIFY(0x021C00C4,demoProc);
static BOOL actionLockOff(Act* a) {
    WWHD_FUNC(0x021C05DC,BOOL,a);
    if(gabi::load<u16>(gabi::ea(a)+0xF8)==2) {
        auto manager=dComIfGp_getPEvtManager(); a->staff=gabi::call<s32>(0x02542D88,manager,STR(0x10014878),0,0);
        demoProc(a); a->action=3;
    } else orderEvent(a,0x10014880);
    return TRUE;
}
VERIFY(0x021C05DC,actionLockOff);
static BOOL actionLockDemo(Act* a) {
    WWHD_FUNC(0x021C0668,BOOL,a);
    auto manager=dComIfGp_getPEvtManager();
    if(gabi::call<BOOL>(0x0254457C,manager,STR(0x10014894))) {
        auto play=dComIfGp_get(); u32 ea=gabi::ea(play)+0x52B8; gabi::store<u16>(ea,gabi::load<u16>(ea)|8); a->action=4;
    } else demoProc(a);
    return TRUE;
}
VERIFY(0x021C0668,actionLockDemo);
static BOOL checkArea(Act* a) {
    WWHD_FUNC(0x021C06D8,BOOL,a);
    gabi::Local<cXyz> relative; gabi::Local<u8[16]> linkage;
    auto play=dComIfGp_get(); u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    gabi::call(0x0201ADE0,gabi::at<cXyz>(player+0x314),relative.get(),&a->home.pos);
    u16 yaw=a->current.angle.y; u32 table=0x104A44F8+((u32)yaw>>3)*8;
    f32 x=relative->x,sine=gabi::load<f32>(table),z=relative->z,cosine=gabi::load<f32>(table+4);
    f32 depth=gabi::fmadds(z,cosine,x*sine); sine=gabi::load<f32>(table);
    f32 lateral=gabi::fmsubs(z,sine,x*cosine);
    if(depth>160.0f || lateral>200.0f || lateral<-200.0f) return FALSE;
    if(gabi::call<s32>(0x025D68A0,gabi::at<void>(player),a)>0x2000) return FALSE;
    a->side=lateral>0.0f?1:0; return TRUE;
}
VERIFY(0x021C06D8,checkArea);
static BOOL actionCloseWait(Act* a) {
    WWHD_FUNC(0x021C07FC,BOOL,a);
    if(gabi::load<u16>(gabi::ea(a)+0xF8)==3) {
        auto manager=dComIfGp_getPEvtManager(); a->staff=gabi::call<s32>(0x02542D88,manager,STR(0x100148B4),0,0);
        demoProc(a); a->action=5;
        auto play=dComIfGp_get(); dBgW* bg=a->background; gabi::call(0x020087EC,gabi::at<void>(gabi::ea(play)+0x12A0),bg);
        a->registered=0;
    } else if(checkArea(a)) {
        gabi::call(0x0253E9B0,gabi::at<void>(gabi::ea(a)+0xF8),STR(0x100148BC));
        u32 ea=gabi::ea(a)+0xFA; gabi::store<u16>(ea,gabi::load<u16>(ea)|4);
    }
    return TRUE;
}
VERIFY(0x021C07FC,actionCloseWait);
static BOOL actionOpen(Act* a) { WWHD_FUNC(0x021C08B0,BOOL,a); demoProc(a); return TRUE; }
VERIFY(0x021C08B0,actionOpen);
static BOOL draw(Act* a) {
    WWHD_FUNC(0x021C08D4,BOOL,a);
    auto env=dKy_getEnvlight(); gabi::call(0x025626A4,env,1,&a->current.pos,&a->tevStr);
    env=dKy_getEnvlight(); J3DModel* model=a->frame; gabi::call(0x02562F5C,env,model,&a->tevStr);
    gabi::call(0x025E2DE0,(J3DModel*)a->frame,0);
    env=dKy_getEnvlight(); model=a->left; gabi::call(0x02562F5C,env,model,&a->tevStr);
    gabi::call(0x025E2DE0,(J3DModel*)a->left,0);
    env=dKy_getEnvlight(); model=a->right; gabi::call(0x02562F5C,env,model,&a->tevStr);
    gabi::call(0x025E2DE0,(J3DModel*)a->right,0);
    env=dKy_getEnvlight(); model=a->bar; gabi::call(0x02562F5C,env,model,&a->tevStr);
    gabi::call(0x025E2DE0,(J3DModel*)a->bar,0); return TRUE;
}
VERIFY(0x021C08D4,draw);
static BOOL execute(Act* a) {
    WWHD_FUNC(0x021C0984,BOOL,a);
    if(!gabi::load<u32>(0x101FDADC)) { gabi::store<u32>(0x101FDADC,1); gabi::call(0xC000A848,gabi::at<void>(0x101FDAE0),gabi::at<void>(0x101BA2EC),24); }
    u8 demo=gabi::load<u8>(gabi::ea(a)+0x2DC); u32 actor=0;
    if(demo && demo<=32) {
        u32 manager=gabi::load<u32>(0x101D5FFC);
        if(!manager) { JUT_ASSERT_fail(STR(0x100147A8),0x23A,STR(0x10014798)); manager=gabi::load<u32>(0x101D5FFC); }
        actor=gabi::call<u32>(0x02526E70,gabi::at<void>(manager),demo);
    }
    if(actor) {
        a->side=0;
        if(gabi::load<u16>(actor+4)&8) { a->doorAngle=gabi::load<s16>(actor+0x22); a->barAngle=gabi::load<s16>(actor+0x24); }
        calcMtx(a);
    } else { u32 target=gabi::load<u32>(0x101FDAE0+4*(u8)a->action); gabi::call_ptr(target,a); }
    return TRUE;
}
VERIFY(0x021C0984,execute);
static BOOL isDelete(Act* a) { WWHD_FUNC(0x021C0A98,BOOL,a); return TRUE; }
VERIFY(0x021C0A98,isDelete);
static BOOL remove(Act* a) {
    WWHD_FUNC(0x021C0AA0,BOOL,a);
    if(gabi::ea(a->heap)) { auto play=dComIfGp_get(); dBgW* bg=a->background; gabi::call(0x020087EC,gabi::at<void>(gabi::ea(play)+0x12A0),bg); }
    const char* archive=getArc(a); dComIfG_resDelete(&a->phase,archive); return TRUE;
}
VERIFY(0x021C0AA0,remove);
static BOOL createInit(Act* a) {
    WWHD_FUNC(0x021C0AFC,BOOL,a);
    u8 sw=getSwitch(a),type=getType(a); auto play=dComIfGp_get(); dBgW* bg=a->background;
    if(gabi::call<BOOL>(0x024EEA6C,gabi::at<void>(gabi::ea(play)+0x12A0),bg,a)) JUT_ASSERT_fail(STR(0x100148D0),334,STR(0x100148CC));
    u8 room=a->current.roomNo; a->registered=1; gabi::store<u8>(gabi::ea(a)+0x1C9,room);
    bool locked=type==2;
    if(type==0 && sw!=0xFF) { u32 save=gabi::load<u32>(0x101F84DC); s8 r=a->current.roomNo; locked=!gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),sw,r); }
    if(type==1) {
        locked=sw==0xFF;
        if(!locked) { u32 save=gabi::load<u32>(0x101F84DC); s8 r=a->current.roomNo; locked=gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),sw,r)!=0; }
    }
    a->barAngle=locked?0:-0x3F65; a->doorAngle=0;
    gabi::store<u32>(gabi::ea(a)+0x39C,0x20); a->enemyTimer=30; a->action=locked?1:4;
    f32 attention=gabi::load<f32>(gabi::ea(a)+0x394),eye=a->eyePos.y;
    gabi::store<f32>(gabi::ea(a)+0x394,attention+250.0f); a->eyePos.y=eye+250.0f;
    calcMtx(a); gabi::call(0x024F43DC,(dBgW*)a->background);
    u32 status=gabi::load<u32>(gabi::ea(a)+0x2E0); gabi::store<u32>(gabi::ea(a)+0x2E0,(status&~63u)|0x2B); return TRUE;
}
VERIFY(0x021C0AFC,createInit);
static s32 create(Act* a) {
    WWHD_FUNC(0x021C0CD0,s32,a);
    const char* archive=getArc(a); s32 phase=dComIfG_resLoad(&a->phase,archive); u32 condition=a->actor_condition;
    if(!(condition&8)) {
        if(a) { gabi::call(0x025D4ED0,a); condition=a->actor_condition; a->__vtbl=0x10014788; }
        a->actor_condition=condition|8;
    }
    if(phase!=4) return phase;
    if(!gabi::call<BOOL>(0x025D63E8,a,gabi::at<void>(0x021BFED4),0x8200)) return 5;
    createInit(a); return 4;
}
VERIFY(0x021C0CD0,create);
static s32 createWrapper(Act* a) { WWHD_FUNC(0x021C0D9C,s32,a); return create(a); }
VERIFY(0x021C0D9C,createWrapper);
static void sinit() { WWHD_FUNC(0x021C0DA0,void,(u32)0); sinit_header_statics_z(0x10464F84,0x101BA348,0x10464F90); }
VERIFY(0x021C0DA0,sinit);
static void stringDtor(void* self,u32 flags) { WWHD_FUNC(0x021C0E34,void,self,flags); if(self && (flags&1)) gabi::call(0x0273AF40,self); }
VERIFY(0x021C0E34,stringDtor);
static void actorDtor(Act* self,u32 flags) { WWHD_FUNC(0x021C0E48,void,self,flags); if(self) { gabi::call(0x025D50BC,self,0); if(flags&1) gabi::call(0x0273AF40,self); } }
VERIFY(0x021C0E48,actorDtor);
static void stringTerminate(void* self) { WWHD_FUNC(0x021C0E9C,void,self); }
VERIFY(0x021C0E9C,stringTerminate);
}
