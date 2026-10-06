/* Wind Temple elevator. */
#include "d/actor/d_a_obj_hbrf1.h"
using daObjHbrf1::Act_c;
static u32 PrmAbstract(fopAc_ac_c* a,s32 width,s32 shift) {
    WWHD_FUNC(0x02351BF4,u32,a,width,shift);
    u32 w=(u32)width&63,s=(u32)shift&63;
    return (s<32?(u32)a->mParameters>>s:0)&((w<32?1u<<w:0)-1);
}
VERIFY(0x02351BF4,PrmAbstract);
static s32 prm(Act_c* a,s32 width,s32 shift) { return PrmAbstract(a,width,shift); }
static bool sw(Act_c* a) { s32 n=prm(a,8,0); return dComIfGs_isSwitch(n,a->home.roomNo)!=0; }
static bool event_bit(u16 bit) {
    u32 save=gabi::load<u32>(0x101F84DC);
    return gabi::call<s32>(0x025B8B94,gabi::at<void>(save+0x644),bit)!=0;
}
static void event_on(u16 bit) {
    u32 save=gabi::load<u32>(0x101F84DC);
    gabi::call(0x025B8B68,gabi::at<void>(save+0x644),bit);
}
static void order(Act_c* a) { fopAcM_orderOtherEventId(a,a->mEventIdx,255,65535,0,1); }
static void sound(Act_c* a,u32 id) {
    s32 reverb=dComIfGp_getReverb(a->current.roomNo);
    gabi::call(0x025E1A40,id,&a->eyePos,0,reverb);
}
s32 Act_c::Mthd_Create() {
    WWHD_FUNC(0x02350DBC,s32,this);
    u32 condition=actor_condition;
    if(!(condition&8)) {
        if(gabi::ea(this)!=0) {
            dBgS_MoveBgActor::ct(this);
            condition=actor_condition;
            __vtbl=0x1002A048;
        }
        actor_condition=condition|8;
    }
    s32 phase=dComIfG_resLoad(&mPhs,STR(0x1002A040));
    if(phase==4) {
        phase=MoveBGCreate(STR(0x1002A040),7,0x024EE76C,0);
        if(phase!=4&&phase!=5)JUT_ASSERT_fail(STR(0x10029F90),0x93,STR(0x10029FA4));
    }
    return phase;
}
VERIFY(0x02350DBC,&Act_c::Mthd_Create);
s32 Act_c::Mthd_Delete() {
    WWHD_FUNC(0x02350E90,s32,this);
    s32 result=MoveBGDelete();
    dComIfG_resDelete(&mPhs,STR(0x1002A040));
    return result;
}
VERIFY(0x02350E90,&Act_c::Mthd_Delete);
s32 Act_c::CreateHeap() {
    WWHD_FUNC(0x02350EDC,s32,this);
    auto* data=(J3DModelData*)dComIfG_getObjectRes(STR(0x1002A040),4,0x10029F78);
    if(!data)JUT_ASSERT_fail(STR(0x10029FF8),0x5E,STR(0x10029FE8));
    mpModel=mDoExt_J3DModel__create(data,0,0x11020203);
    return mpModel!=nullptr;
}
VERIFY(0x02350EDC,&Act_c::CreateHeap);
void Act_c::set_mtx() {
    WWHD_FUNC(0x02350F78,void,this);
    mDoMtx_stack_c::transS(current.pos.x,current.pos.y,current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(),shape_angle.x,shape_angle.y,shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel,mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(),gabi::at<Mtx34>(0x10469DEC));
}
VERIFY(0x02350F78,&Act_c::set_mtx);
void Act_c::init_mtx() {
    WWHD_FUNC(0x0235104C,void,this);
    J3DModel* model=mpModel;
    f32 x=scale.x,y=scale.y,z=scale.z;
    gabi::store<f32>(gabi::ea(model)+0xBC,x);
    gabi::store<f32>(gabi::ea(model)+0xC0,y);
    gabi::store<f32>(gabi::ea(model)+0xC4,z);
    set_mtx();
}
VERIFY(0x0235104C,&Act_c::init_mtx);
s32 Act_c::Create() {
    WWHD_FUNC(0x0235106C,s32,this);
    cullMtx=gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    s32 type=prm(this,1,8); bool raised=false;
    if(type==0) { if(!sw(this))raised=true; else type=prm(this,1,8); }
    if(!raised&&type==1)raised=sw(this);
    if(raised) {
        mMode=4;
        current.pos.y=(f32)current.pos.y+750.0f;
    } else mMode=0;
    init_mtx();
    fopAcM_setCullSizeBox(this,-800.0f,-1000.0f,-800.0f,800.0f,1000.0f,800.0f);
    mYOffset=10.0f;
    s16 index=gabi::call<s16>(0x02543F10,dComIfGp_getPEvtManager(),STR(0x1002A024),255);
    eyePos.copy(home.pos);
    mEventIdx=index;
    mEventActive=0;
    return TRUE;
}
VERIFY(0x0235106C,&Act_c::Create);
void Act_c::down_stop() {
    WWHD_FUNC(0x02351278,void,this);
    s32 type=prm(this,1,8); bool trigger=false;
    if(type==0) { if(!sw(this))trigger=true; else type=prm(this,1,8); }
    if(!trigger&&type!=0)trigger=sw(this);
    if(trigger) {
        s32 event=prm(this,8,16); bool first=false;
        if(event==0) { if(!event_bit(0x1540))first=true; else event=prm(this,8,16); }
        if(first) {
            event_on(0x1540); order(this); mMode=1; mEventActive=1;
        } else if(event==1&&!event_bit(0x1510)) {
            event_on(0x1510); order(this); mEventActive=1; mMode=1;
        } else mMode=3;
    }
}
VERIFY(0x02351278,&Act_c::down_stop);
void Act_c::up_stop() {
    WWHD_FUNC(0x02351580,void,this);
    s32 type=prm(this,1,8); bool trigger=false;
    if(type==0) { if(sw(this))trigger=true; else type=prm(this,1,8); }
    if(!trigger&&type!=0)trigger=!sw(this);
    if(trigger) {
        s32 event=prm(this,8,16); bool first=false;
        if(event==0) { if(!event_bit(0x1520))first=true; else event=prm(this,8,16); }
        if(first) {
            event_on(0x1520); order(this); mMode=5; mEventActive=1;
        } else if(event==1&&!event_bit(0x1508)) {
            event_on(0x1508); order(this); mEventActive=1; mMode=5;
        } else mMode=7;
    }
}
VERIFY(0x02351580,&Act_c::up_stop);
void Act_c::up_demo_wait() {
    WWHD_FUNC(0x02351430,void,this);
    if(gabi::load<u16>(gabi::ea(this)+0xF8)==2) { mTimer=30; mMode=2; }
}
VERIFY(0x02351430,&Act_c::up_demo_wait);
void Act_c::down_demo_wait() {
    WWHD_FUNC(0x02351740,void,this);
    if(gabi::load<u16>(gabi::ea(this)+0xF8)==2) { mTimer=30; mMode=6; }
    else order(this);
}
VERIFY(0x02351740,&Act_c::down_demo_wait);
void Act_c::up_demo_timer() {
    WWHD_FUNC(0x02351450,void,this);
    s32 timer=(s32)((u32)mTimer-1); mTimer=timer;
    if(timer==0)mMode=3;
}
VERIFY(0x02351450,&Act_c::up_demo_timer);
void Act_c::down_demo_timer() {
    WWHD_FUNC(0x0235177C,void,this);
    s32 timer=(s32)((u32)mTimer-1); mTimer=timer;
    if(timer==0)mMode=7;
}
VERIFY(0x0235177C,&Act_c::down_demo_timer);
void Act_c::up_demo() {
    WWHD_FUNC(0x0235146C,void,this);
    f32 next=(f32)current.pos.y+(f32)mYOffset,target=(f32)home.pos.y+750.0f;
    current.pos.y=next;
    if(!(next<target)) {
        current.pos.y=target;
        if(mEventActive!=0) { dComIfGp_event_reset(); mEventActive=0; }
        mMode=4;
        sound(this,0x69C9);
        gabi::Local<cXyz> direction; direction->x=0; direction->y=1; direction->z=0;
        dComIfGp_getVibration_StartShock(4,-33,direction);
    } else sound(this,0x61C8);
}
VERIFY(0x0235146C,&Act_c::up_demo);
void Act_c::down_demo() {
    WWHD_FUNC(0x02351798,void,this);
    f32 next=(f32)current.pos.y-(f32)mYOffset,target=home.pos.y;
    current.pos.y=next;
    if(!(next>target)) {
        current.pos.y=target;
        if(mEventActive!=0) { dComIfGp_event_reset(); mEventActive=0; }
        mMode=0;
        sound(this,0x69C9);
        gabi::Local<cXyz> direction; direction->x=0; direction->y=1; direction->z=0;
        dComIfGp_getVibration_StartShock(4,-33,direction);
    } else sound(this,0x61C8);
}
VERIFY(0x02351798,&Act_c::down_demo);
s32 Act_c::Execute(Mtx34** matrix) {
    WWHD_FUNC(0x0235189C,s32,this,matrix);
    switch((s32)mMode) {
    case 0: down_stop(); break; case 1: up_demo_wait(); break;
    case 2: up_demo_timer(); break; case 3: up_demo(); break;
    case 4: up_stop(); break; case 5: down_demo_wait(); break;
    case 6: down_demo_timer(); break; case 7: down_demo(); break;
    }
    set_mtx();
    gabi::store<u32>(gabi::ea(matrix),0x10469DEC);
    return TRUE;
}
VERIFY(0x0235189C,&Act_c::Execute);
s32 Act_c::Draw() {
    WWHD_FUNC(0x02351A20,s32,this);
    settingTevStruct(dKy_getEnvlight(),1,&current.pos,&tevStr);
    setLightTevColorType(dKy_getEnvlight(),mpModel,&tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x02351A20,&Act_c::Draw);
s32 Act_c::Delete() { WWHD_FUNC(0x02351B98,s32,this); return TRUE; }
VERIFY(0x02351B98,&Act_c::Delete);
static s32 method_Create(Act_c* a) { WWHD_FUNC(0x02351AB8,s32,a); return a->Mthd_Create(); }
VERIFY(0x02351AB8,method_Create);
static s32 method_Delete(Act_c* a) { WWHD_FUNC(0x02351ABC,s32,a); return a->Mthd_Delete(); }
VERIFY(0x02351ABC,method_Delete);
static s32 method_Execute(Act_c* a) { WWHD_FUNC(0x02351AC0,s32,a); return a->MoveBGExecute(); }
VERIFY(0x02351AC0,method_Execute);
static s32 method_Draw(Act_c* a) { WWHD_FUNC(0x02351AC4,s32,a); return a->Draw_v(); }
VERIFY(0x02351AC4,method_Draw);
static s32 method_IsDelete(Act_c* a) { WWHD_FUNC(0x02351AD4,s32,a); return a->IsDelete_v(); }
VERIFY(0x02351AD4,method_IsDelete);
static void sinit() {
    WWHD_FUNC(0x02351AE4,void,(u32)0);
    for(u32 i=0;i<4;i++)gabi::store<u32>(0x10469DDC+i*4,0);
    __register_global_object(0x101C9D70);
    gabi::store<f32>(0x10469DD0,-3.1415927f); gabi::store<f32>(0x10469DD4,3.1415927f);
    gabi::call(0x028ED6F8,(u32)0x10469DD8); __register_global_object(0x101C9D7C);
    gabi::call(0x028EAB2C,(u32)0x10469DD9); __register_global_object(0x101C9D88);
}
VERIFY(0x02351AE4,sinit);
static void trivial_dt(void* a,s32 flags) { WWHD_FUNC(0x02351B78,void,a,flags); if(a&&(flags&1))operator_delete(a); }
VERIFY(0x02351B78,trivial_dt);
static s32 base_IsDelete(void* a) { WWHD_FUNC(0x02351B8C,s32,a); return TRUE; }
VERIFY(0x02351B8C,base_IsDelete);
static void base_empty(void* a) { WWHD_FUNC(0x02351B94,void,a); }
VERIFY(0x02351B94,base_empty);
static void actor_dt(Act_c* a,s32 flags) {
    WWHD_FUNC(0x02351BA0,void,a,flags);
    if(a) { gabi::call(0x025D50BC,a,0); if(flags&1)operator_delete(a); }
}
VERIFY(0x02351BA0,actor_dt);
