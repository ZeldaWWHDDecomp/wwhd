/* WWHD four sliding gate panels. Derived game code: */
#include "bindings.h"
struct daObjHami4_c : fopAc_ac_c {
    request_of_phase_process_class phase;
    gptr<J3DModel> models[4];
    gptr<dBgW> backgrounds[4];
    Mtx34 backgroundMtx[4];
    be<s32> state;
    be<f32> opening;
    BOOL CreateHeap(); void CreateInit(); void set_mtx();
    void close_stop(); void demo_wait(); void open_demo();
};
WWHD_OFFSET(daObjHami4_c,phase,0x3AC);
WWHD_OFFSET(daObjHami4_c,models,0x3B4);
WWHD_OFFSET(daObjHami4_c,backgrounds,0x3C4);
WWHD_OFFSET(daObjHami4_c,backgroundMtx,0x3D4);
WWHD_OFFSET(daObjHami4_c,state,0x494);
WWHD_SIZE(daObjHami4_c,0x49C);
static u32 PrmAbstract(daObjHami4_c* p,u32 width,u32 shift) {
    WWHD_FUNC(0x02350410,u32,p,width,shift);
    u32 parameters=p->mParameters;
    u32 mask=(width&32)?0:(1U<<(width&31));
    u32 value=(shift&32)?0:(parameters>>(shift&31));
    return value&(mask-1);
}
VERIFY(0x02350410,PrmAbstract);
static void translateOpening(daObjHami4_c* p) {
    f32 amount=p->opening;
    f32 sine=gabi::load<f32>(0x104A64F8);
    f32 cosine=gabi::load<f32>(0x104A64FC);
    f32 x=(s16)gabi::ftoi(-(amount*sine));
    f32 z=(s16)gabi::ftoi(-(amount*cosine));
    mDoMtx_stack_c::transM(x,0.0f,z);
}
BOOL daObjHami4_c::CreateHeap() {
    WWHD_FUNC(0x0234F914,BOOL,this);
    auto data=gabi::at<J3DModelData>(gabi::ea(dComIfG_getObjectRes(STR(0x10029E40),4,0x10029E04)));
    if (!data) JUT_ASSERT_fail(STR(0x10029E48),0x5F,STR(0x10029E5C));
    u32 sw=PrmAbstract(this,8,0);
    if (dComIfGs_isSwitch(sw,home.roomNo)) { state=3; opening=1500.0f; }
    else { opening=0.0f; state=0; }
    for (u32 i=0;i<4;++i) {
        J3DModel* created=mDoExt_J3DModel__create(data,0,0x11020203);
        models[i]=created;
        if (!created) return FALSE;
        s16 angle=(s16)((s32)current.angle.y+i*0x4000);
        f32 x=current.pos.x,z=current.pos.z,y=current.pos.y;
        mDoMtx_stack_c::transS(x,y,z);
        mDoMtx_stack_c::YrotM(angle);
        translateOpening(this);
        f32 sy=scale.y,sx=scale.x,sz=scale.z;
        gabi::call(0x025F2518,sx,sy,sz);
        gabi::call(0x028E90D4,mDoMtx_stack_c::get(),&backgroundMtx[i]);
        mtx_copy(gabi::at<Mtx34>(gabi::ea(models[i])+0xC8),mDoMtx_stack_c::get());
        dBgW* bg=gabi::call<dBgW*>(0x024F23F4,(void*)nullptr);
        backgrounds[i]=bg;
        if (!bg) return FALSE;
        void* resource=dComIfG_getObjectRes(STR(0x10029E40),7,0x10029E04);
        bg=backgrounds[i];
        if (gabi::call<s32>(0x0200A030,bg,resource,1,&backgroundMtx[i])) return FALSE;
    }
    return TRUE;
}
VERIFY(0x0234F914,&daObjHami4_c::CreateHeap);
static BOOL CheckCreateHeap(daObjHami4_c* p) {
    WWHD_FUNC(0x0234FC24,BOOL,p); return p->CreateHeap();
}
VERIFY(0x0234FC24,CheckCreateHeap);
void daObjHami4_c::set_mtx() {
    WWHD_FUNC(0x0234FC28,void,this);
    for (u32 i=0;i<4;++i) {
        f32 sy=scale.y;
        u32 model=gabi::ea(models[i]);
        s16 angle=(s16)((s32)current.angle.y+i*0x4000);
        f32 sx=scale.x,sz=scale.z;
        gabi::store<f32>(model+0xBC,sx); gabi::store<f32>(model+0xC0,sy); gabi::store<f32>(model+0xC4,sz);
        f32 x=current.pos.x,z=current.pos.z,y=current.pos.y;
        mDoMtx_stack_c::transS(x,y,z);
        mDoMtx_stack_c::YrotM(angle);
        translateOpening(this);
        mtx_copy(gabi::at<Mtx34>(gabi::ea(models[i])+0xC8),mDoMtx_stack_c::get());
        gabi::call(0x028E90D4,mDoMtx_stack_c::get(),&backgroundMtx[i]);
        gabi::call(0x024F43DC,(dBgW*)backgrounds[i]);
    }
}
VERIFY(0x0234FC28,&daObjHami4_c::set_mtx);
void daObjHami4_c::CreateInit() {
    WWHD_FUNC(0x0234FDE8,void,this);
    cullMtx=gabi::ea(J3DModel_getBaseTRMtx(models[0]));
    for (u32 i=0;i<4;++i) { auto bgsp=dComIfG_Bgsp(); gabi::call(0x024EEA6C,bgsp,(dBgW*)backgrounds[i],this); }
    fopAcM_setCullSizeBox(this,-10000.0f,-100.0f,-10000.0f,10000.0f,100.0f,10000.0f);
    gabi::store<f32>(gabi::ea(this)+0x364,1.0f);
    set_mtx();
}
VERIFY(0x0234FDE8,&daObjHami4_c::CreateInit);
static s32 Create(daObjHami4_c* p) {
    WWHD_FUNC(0x0234FE9C,s32,p);
    u32 condition=p->actor_condition;
    if (!(condition&8)) {
        if (gabi::ea(p)!=0) { gabi::call(0x025D4ED0,p); condition=p->actor_condition; p->__vtbl=0x10029E1C; }
        p->actor_condition=condition|8;
    }
    for (u32 i=0;i<4;++i) p->backgrounds[i]=nullptr;
    s32 status=dComIfG_resLoad(&p->phase,STR(0x10029DF4));
    if (status==4) {
        if (!fopAcM_entrySolidHeap(p,0x0234FC24,0x4900)) return 5;
        p->CreateInit();
    }
    return status;
}
VERIFY(0x0234FE9C,Create);
static BOOL Delete(daObjHami4_c* p) {
    WWHD_FUNC(0x0234FF78,BOOL,p);
    for (u32 i=0;i<4;++i) {
        u32 bg=gabi::ea(p->backgrounds[i]);
        if (bg && gabi::load<u32>(bg)<0x100) { auto bgsp=dComIfG_Bgsp(); gabi::call(0x020087EC,bgsp,(dBgW*)p->backgrounds[i]); }
    }
    dComIfG_resDelete(&p->phase,STR(0x10029DFC)); return TRUE;
}
VERIFY(0x0234FF78,Delete);
static BOOL Draw(daObjHami4_c* p) {
    WWHD_FUNC(0x0235000C,BOOL,p);
    settingTevStruct(dKy_getEnvlight(),1,&p->current.pos,&p->tevStr);
    dComIfGd_setListBG();
    for (u32 i=0;i<4;++i) { auto env=dKy_getEnvlight(); setLightTevColorType(env,p->models[i],&p->tevStr); mDoExt_modelUpdateDL(p->models[i]); }
    dComIfGd_setList(); return TRUE;
}
VERIFY(0x0235000C,Draw);
void daObjHami4_c::close_stop() {
    WWHD_FUNC(0x023500C4,void,this);
    u32 sw=PrmAbstract(this,8,0);
    if (dComIfGs_isSwitch(sw,home.roomNo)) { gabi::call(0x025D77DC,this,STR(0x10029E84),1,0xFFFF); state=1; }
}
VERIFY(0x023500C4,&daObjHami4_c::close_stop);
void daObjHami4_c::demo_wait() {
    WWHD_FUNC(0x02350140,void,this);
    if (gabi::load<u16>(gabi::ea(this)+0xF8)==2) {
        state=2; gabi::call(0x025E1988,0x806);
        s32 reverb=dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(0x69C7,&current.pos,0,reverb);
    } else gabi::call(0x025D77DC,this,STR(0x10029E90),1,0xFFFF);
}
VERIFY(0x02350140,&daObjHami4_c::demo_wait);
void daObjHami4_c::open_demo() {
    WWHD_FUNC(0x023501D4,void,this);
    f32 next=opening+10.0f;
    if (next<1500.0f) { opening=next; return; }
    opening=1500.0f; state=3;
    auto vibration=dComIfGp_getVibration();
    gabi::Local<cXyz> direction; direction->z=0.0f; direction->y=1.0f; direction->x=0.0f;
    gabi::call(0x025CB374,vibration,4,-0x21,(cXyz*)direction);
    auto play=dComIfGp_get();
    u16 flags=gabi::load<u16>(gabi::ea(play)+0x52B8);
    gabi::store<u16>(gabi::ea(play)+0x52B8,flags|8);
}
VERIFY(0x023501D4,&daObjHami4_c::open_demo);
static BOOL Execute(daObjHami4_c* p) {
    WWHD_FUNC(0x02350274,BOOL,p);
    u32 state=p->state;
    if (state==0) p->close_stop(); else if (state==1) p->demo_wait(); else if (state==2) p->open_demo();
    p->set_mtx(); return TRUE;
}
VERIFY(0x02350274,Execute);
static void sinit() { WWHD_FUNC(0x02350308,void,(u32)0); sinit_header_statics(0x10469D98,0x101C9C5C); }
VERIFY(0x02350308,sinit);
static void trivial_dt(void* p,s32 flags) { WWHD_FUNC(0x0235039C,void,p,flags); if (p && (flags&1)) operator_delete(p); }
VERIFY(0x0235039C,trivial_dt);
static BOOL IsDelete(daObjHami4_c* p) { WWHD_FUNC(0x023503B0,BOOL,p); return TRUE; }
VERIFY(0x023503B0,IsDelete);
static void destroy(daObjHami4_c* p,s32 flags) { WWHD_FUNC(0x023503B8,void,p,flags); if (p) { gabi::call(0x025D50BC,p,0); if (flags&1) operator_delete(p); } }
VERIFY(0x023503B8,destroy);
static void open_stop(daObjHami4_c* p) { WWHD_FUNC(0x0235040C,void,p); }
VERIFY(0x0235040C,open_stop);
