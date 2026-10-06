/* WWHD Dragon Roost water platform. Derived game code: */
#include "bindings.h"

struct daObjGryw00_c : dBgS_MoveBgActor {
    request_of_phase_process_class phase;
    gptr<J3DModel> model;
    Mtx34 waterMtx;
    mDoExt_btkAnm btk;
    mDoExt_bckAnm bck;
    be<s32> activationSwitch;
    be<s16> actionDelta, actionIndex;
    be<u32> actionTarget;
    be<s32> hidden;
    be<f32> waterLevel, waterMax, waterStep;
    be<f32> waveAmplitude, waveMax, waveStep;
    be<f32> finalWaterY;
    be<s32> playSound, geyserRemaining;
    gptr<void> emitters[2];
    be<s32> particleRetireRemaining;
    s32 Mthd_Create(); s32 Mthd_Delete();
    BOOL CreateHeap(); BOOL Create(); BOOL Execute(be<u32>*); BOOL Draw(); BOOL Delete();
    f32 get_draw_water_lv(daObjGryw00_c*);
    BOOL setup_high_water_level_btk_anm();
    void particle_set(); void particle_move(); void particle_delete(); void set_se();
    void switch_wait_act_proc(); void spread_water_face_act_proc();
    void water_level_move_wait_act_proc(); void anime_loop_start_wait_act_proc();
    void high_water_level_act_proc();
};
WWHD_OFFSET(daObjGryw00_c, phase, 0x3E0);
WWHD_OFFSET(daObjGryw00_c, btk, 0x41C);
WWHD_OFFSET(daObjGryw00_c, bck, 0x490);
WWHD_OFFSET(daObjGryw00_c, actionTarget, 0x524);
WWHD_OFFSET(daObjGryw00_c, emitters, 0x550);
WWHD_SIZE(daObjGryw00_c, 0x55C);
static const char* arcName() { return STR(0x10029938); }
static void changeAction(daObjGryw00_c* p, u32 target) {
    p->actionDelta=0; p->actionIndex=-1; p->actionTarget=target;
}
static void* objectResource(s32 index) {
    return dComIfG_getObjectRes(arcName(),index,0x10029940);
}
static void soundCurrent(daObjGryw00_c* p,u32 sound) {
    s8 room=p->current.roomNo;
    s32 reverb=dComIfGp_getReverb(room);
    mDoAud_seStart(sound,&p->current.pos,0,reverb);
}
static u32 PrmAbstract(daObjGryw00_c* p,u32 width,u32 shift) {
    WWHD_FUNC(0x0234D390,u32,p,width,shift);
    u32 parameters=p->mParameters;
    u32 mask=(width&32) ? 0 : (1U<<(width&31));
    u32 value=(shift&32) ? 0 : (parameters>>(shift&31));
    return value&(mask-1);
}
VERIFY(0x0234D390,PrmAbstract);
s32 daObjGryw00_c::Mthd_Create() {
    WWHD_FUNC(0x0234C420,s32,this);
    u32 condition=actor_condition;
    if (!(condition&8)) {
        if (gabi::ea(this)!=0) {
            dBgS_MoveBgActor::ct(this);
            __vtbl=0x10029A50;
            mDoExt_btkAnm::ct(&btk);
            gabi::call(0x027F2BC0,&bck,0);
            gabi::store<u32>(gabi::ea(this)+0x4A0,0x1016E54C);
            gabi::call(0x027DA984,gabi::at<void>(gabi::ea(this)+0x4A4));
            gabi::store<u32>(gabi::ea(this)+0x518,0);
            gabi::store<u32>(gabi::ea(this)+0x514,0);
            gabi::store<u32>(gabi::ea(this)+0x4A0,0x10029958);
            gabi::store<u32>(gabi::ea(this)+0x50C,0);
            gabi::store<u32>(gabi::ea(this)+0x4D8,0x1016D820);
            gabi::store<u32>(gabi::ea(this)+0x510,0);
            condition=actor_condition;
            gabi::store<u32>(gabi::ea(this)+0x4E8,0);
        }
        actor_condition=condition|8;
    }
    s32 result=dComIfG_resLoad(&phase,arcName());
    if (result==4) {
        result=MoveBGCreate(arcName(),15,0x024EE76C,0x9A0);
        if (result!=4 && result!=5)
            JUT_ASSERT_fail(STR(0x10029980),0x209,STR(0x10029994));
    }
    return result;
}
VERIFY(0x0234C420,&daObjGryw00_c::Mthd_Create);
s32 daObjGryw00_c::Mthd_Delete() {
    WWHD_FUNC(0x0234C550,s32,this);
    s32 result=MoveBGDelete();
    dComIfG_resDelete(&phase,arcName());
    return result;
}
VERIFY(0x0234C550,&daObjGryw00_c::Mthd_Delete);
f32 daObjGryw00_c::get_draw_water_lv(daObjGryw00_c* p) {
    WWHD_FUNC(0x0234C5C4,f32,this,p);
    f32 value=p->home.pos.y+p->waterLevel;
    return value+p->waveAmplitude;
}
VERIFY(0x0234C5C4,&daObjGryw00_c::get_draw_water_lv);
BOOL daObjGryw00_c::CreateHeap() {
    WWHD_FUNC(0x0234C5DC,BOOL,this);
    auto data=gabi::at<J3DModelData>(gabi::ea(objectResource(9)));
    auto texture=gabi::at<J3DAnmTextureSRTKey>(gabi::ea(objectResource(12)));
    auto transform=gabi::at<J3DAnmTransform>(gabi::ea(objectResource(6)));
    if (!data || !texture || !transform) {
        JUT_ASSERT_fail(STR(0x100299E0),0x199,STR(0x100299DC));
        return FALSE;
    }
    J3DModel* created=mDoExt_J3DModel__create(data,0x80000,0x11000222);
    model=created;
    if (!created) return FALSE;
    J3DModelData* modelData=J3DModel_getModelData(created);
    BOOL textureReady=btk.init(modelData,texture,true,0,0.0f,0,400,false,0);
    modelData=J3DModel_getModelData(model);
    BOOL transformReady=bck.init(modelData,transform,true,0,0.0f,0,-1,false);
    if (!model || !textureReady || !transformReady) return FALSE;
    return TRUE;
}
VERIFY(0x0234C5DC,&daObjGryw00_c::CreateHeap);
BOOL daObjGryw00_c::setup_high_water_level_btk_anm() {
    WWHD_FUNC(0x0234C768,BOOL,this);
    auto texture=gabi::at<J3DAnmTextureSRTKey>(gabi::ea(objectResource(12)));
    if (!texture) JUT_ASSERT_fail(STR(0x100299F8),0xDF,STR(0x10029A0C));
    J3DModelData* data=J3DModel_getModelData(model);
    return btk.init(data,texture,true,2,1.0f,401,-1,true,0) ? TRUE : FALSE;
}
VERIFY(0x0234C768,&daObjGryw00_c::setup_high_water_level_btk_anm);
BOOL daObjGryw00_c::Create() {
    WWHD_FUNC(0x0234C838,BOOL,this);
    u32 sw=PrmAbstract(this,8,0);
    f32 finalY=home.pos.y+510.0f;
    activationSwitch=sw; finalWaterY=finalY;
    bool activated=false;
    if (sw!=255) activated=dComIfGs_isSwitch(sw,home.roomNo)==TRUE;
    if (activated) {
        setup_high_water_level_btk_anm();
        hidden=0; waterLevel=510.0f; bck.mFrameCtrl.mFrame=299.0f;
        waveMax=-10.0f; waterMax=510.0f; waveAmplitude=-10.0f;
        changeAction(this,0x0234D260);
    } else {
        hidden=1; waterMax=-5.0f; waveAmplitude=0.0f; waveMax=0.0f;
        changeAction(this,0x0234D024); waterLevel=-5.0f;
    }
    f32 x=scale.x,y=scale.y,z=scale.z;
    u32 m=gabi::ea(model);
    gabi::store<f32>(m+0xBC,x); gabi::store<f32>(m+0xC4,z); gabi::store<f32>(m+0xC0,y);
    f32 level=home.pos.y+waterLevel;
    f32 drawY=level+waveAmplitude;
    f32 homeZ=home.pos.z,homeX=home.pos.x;
    mDoMtx_stack_c::transS(homeX,drawY,homeZ);
    mtx_copy(gabi::at<Mtx34>(gabi::ea(model)+0xC8),mDoMtx_stack_c::get());
    cullMtx=gabi::ea(J3DModel_getBaseTRMtx(model));
    particleRetireRemaining=0;
    return TRUE;
}
VERIFY(0x0234C838,&daObjGryw00_c::Create);
void daObjGryw00_c::particle_move() {
    WWHD_FUNC(0x0234CAE8,void,this);
    f32 y=home.pos.y+waterLevel,z=home.pos.z,x=home.pos.x;
    for (u32 i=0;i<2;++i) {
        u32 emitter=gabi::ea(emitters[i]);
        if (!emitter) continue;
        u8 type=gabi::load<u8>(emitter+0x262);
        gabi::store<f32>(emitter+0x22C,x); gabi::store<f32>(emitter+0x234,z); gabi::store<f32>(emitter+0x230,y);
        if (type>=7) gabi::store<f32>(emitter+0x230,-gabi::load<f32>(emitter+0x230));
    }
}
VERIFY(0x0234CAE8,&daObjGryw00_c::particle_move);
void daObjGryw00_c::set_se() {
    WWHD_FUNC(0x0234CB60,void,this);
    if (playSound!=TRUE) return;
    f32 water=waterLevel;
    s32 remaining=geyserRemaining;
    if (!(water<510.0f)) playSound=FALSE;
    if (remaining>0) { geyserRemaining=remaining-1; soundCurrent(this,0x704C); }
    soundCurrent(this,0x704B); soundCurrent(this,0x704D);
}
VERIFY(0x0234CB60,&daObjGryw00_c::set_se);
BOOL daObjGryw00_c::Execute(be<u32>* outMtx) {
    WWHD_FUNC(0x0234CC24,BOOL,this,outMtx);
    f32 wave=waveAmplitude+waveStep;
    f32 water=waterLevel,limit=waveMax,step=waterStep;
    waveAmplitude=wave;
    f32 waterLimit=waterMax;
    if (wave<limit) { waveStep=0.0f; waveAmplitude=limit; }
    water=water+step; waterLevel=water;
    if (water>waterLimit) { waterStep=0.0f; waterLevel=waterLimit; }
    f32 homeX=home.pos.x;
    f32 drawY=get_draw_water_lv(this);
    f32 homeZ=home.pos.z;
    mDoMtx_stack_c::transS(homeX,drawY,homeZ);
    mtx_copy(gabi::at<Mtx34>(gabi::ea(model)+0xC8),mDoMtx_stack_c::get());
    f32 waterY=home.pos.y+waterLevel;
    homeZ=home.pos.z; homeX=home.pos.x;
    mDoMtx_stack_c::transS(homeX,waterY,homeZ);
    f32 scaleZ=scale.z,scaleX=scale.x,scaleY=scale.y;
    gabi::call(0x025F2518,scaleX,scaleY,scaleZ);
    gabi::call(0x028E90D4,mDoMtx_stack_c::get(),&waterMtx);
    *outMtx=gabi::ea(&waterMtx);
    particle_move(); set_se(); btk.play(); bck.play();
    ptmf_call(gabi::ea(&actionDelta),this);
    return TRUE;
}
VERIFY(0x0234CC24,&daObjGryw00_c::Execute);
BOOL daObjGryw00_c::Draw() {
    WWHD_FUNC(0x0234CDE4,BOOL,this);
    if (!hidden) {
        settingTevStruct(dKy_getEnvlight(),2,&current.pos,&tevStr);
        auto light=dKy_getEnvlight(); setLightTevColorType(light,model,&tevStr);
        J3DModel* m=model; f32 frame=btk.getFrame();
        btk.entry(J3DModel_getModelData(m),frame);
        m=model; frame=bck.getFrame();
        bck.entry(J3DModel_getModelData(m),frame);
        mDoExt_modelUpdateDL(model);
    }
    return TRUE;
}
VERIFY(0x0234CDE4,&daObjGryw00_c::Draw);
void daObjGryw00_c::particle_delete() {
    WWHD_FUNC(0x0234CE74,void,this);
    for (u32 i=0;i<2;++i) {
        u32 emitter=gabi::ea(emitters[i]);
        if (!emitter) continue;
        u32 flags=gabi::load<u32>(emitter+0x254);
        gabi::store<s32>(emitter+0x5C,-1);
        gabi::store<u32>(emitter+0x254,flags|1);
        emitters[i]=nullptr;
    }
}
VERIFY(0x0234CE74,&daObjGryw00_c::particle_delete);
BOOL daObjGryw00_c::Delete() {
    WWHD_FUNC(0x0234CEC4,BOOL,this);
    particle_delete(); return TRUE;
}
VERIFY(0x0234CEC4,&daObjGryw00_c::Delete);
void daObjGryw00_c::particle_set() {
    WWHD_FUNC(0x0234CEE8,void,this);
    gabi::Local<cXyz> homePosition;
    homePosition->x=home.pos.x; homePosition->y=home.pos.y; homePosition->z=home.pos.z;
    GXColor* tint=gabi::at<GXColor>(gabi::ea(this)+0x1A8);
    dComIfGp_particle_set(0x8295,homePosition,nullptr,nullptr,255,nullptr,-1,nullptr,tint);
    dComIfGp_particle_set(0x8297,homePosition,nullptr,nullptr,255,nullptr,-1,nullptr,tint);
    emitters[0]=dComIfGp_particle_set(0x8296,homePosition,nullptr,nullptr,255,nullptr,-1,nullptr,tint);
    emitters[1]=dComIfGp_particle_set(0x8298,homePosition,nullptr,nullptr,255,nullptr,-1,nullptr,tint);
}
VERIFY(0x0234CEE8,&daObjGryw00_c::particle_set);
void daObjGryw00_c::switch_wait_act_proc() {
    WWHD_FUNC(0x0234D024,void,this);
    s32 sw=activationSwitch;
    if (sw==255 || dComIfGs_isSwitch(sw,home.roomNo)!=TRUE) return;
    waveMax=-10.0f; waveStep=-0.10526315867900848f;
    bck.mFrameCtrl.mRate=1.0f; btk.mFrameCtrl.mRate=1.0f;
    waterMax=5.0f; waterStep=0.10526315867900848f;
    particle_set(); hidden=0; playSound=TRUE; geyserRemaining=310;
    changeAction(this,0x0234D0F0);
}
VERIFY(0x0234D024,&daObjGryw00_c::switch_wait_act_proc);
void daObjGryw00_c::spread_water_face_act_proc() {
    WWHD_FUNC(0x0234D0F0,void,this);
    if (gabi::call<BOOL>(0x027F2BF8,&btk,95.0f)) changeAction(this,0x0234D14C);
}
VERIFY(0x0234D0F0,&daObjGryw00_c::spread_water_face_act_proc);
void daObjGryw00_c::water_level_move_wait_act_proc() {
    WWHD_FUNC(0x0234D14C,void,this);
    if (gabi::call<BOOL>(0x027F2BF8,&btk,299.0f)) {
        waterMax=510.0f; bck.mFrameCtrl.mRate=0.0f;
        waterStep=1.6777408123016357f; bck.mFrameCtrl.mFrame=299.0f;
        changeAction(this,0x0234D1EC);
    }
}
VERIFY(0x0234D14C,&daObjGryw00_c::water_level_move_wait_act_proc);
void daObjGryw00_c::anime_loop_start_wait_act_proc() {
    WWHD_FUNC(0x0234D1EC,void,this);
    if (!(btk.mFrameCtrl.mState&1) && btk.mFrameCtrl.mRate!=0.0f) return;
    particleRetireRemaining=100;
    setup_high_water_level_btk_anm(); changeAction(this,0x0234D260);
}
VERIFY(0x0234D1EC,&daObjGryw00_c::anime_loop_start_wait_act_proc);
void daObjGryw00_c::high_water_level_act_proc() {
    WWHD_FUNC(0x0234D260,void,this);
    s32 remaining=particleRetireRemaining;
    if (!remaining) return;
    remaining=(s32)((u32)remaining-1); particleRetireRemaining=remaining;
    if (!remaining) particle_delete();
}
VERIFY(0x0234D260,&daObjGryw00_c::high_water_level_act_proc);
static void sinit() {
    WWHD_FUNC(0x0234D27C,void,(u32)0);
    sinit_header_statics(0x10469CC8,0x101C9A48);
}
VERIFY(0x0234D27C,sinit);
static void trivial_dt(void* p,s32 flags) {
    WWHD_FUNC(0x0234D310,void,p,flags);
    if (p && (flags&1)) operator_delete(p);
}
VERIFY(0x0234D310,trivial_dt);
static BOOL isDelete(daObjGryw00_c* p) {
    WWHD_FUNC(0x0234D324,BOOL,p); return TRUE;
}
VERIFY(0x0234D324,isDelete);
static void destroy(daObjGryw00_c* p,s32 flags) {
    WWHD_FUNC(0x0234D32C,void,p,flags);
    if (p) {
        gabi::call(0x027F3628,gabi::at<void>(gabi::ea(p)+0x4A0),0);
        gabi::call(0x025D50BC,p,0);
        if (flags&1) operator_delete(p);
    }
}
VERIFY(0x0234D32C,destroy);
static s32 daObjGryw00_Create(daObjGryw00_c* p) {
    WWHD_FUNC(0x0234C54C,s32,p); return p->Mthd_Create();
}
VERIFY(0x0234C54C,daObjGryw00_Create);
static s32 daObjGryw00_Delete(daObjGryw00_c* p) {
    WWHD_FUNC(0x0234C59C,s32,p); return p->Mthd_Delete();
}
VERIFY(0x0234C59C,daObjGryw00_Delete);
static BOOL daObjGryw00_Execute(daObjGryw00_c* p) {
    WWHD_FUNC(0x0234C5A0,BOOL,p); return p->MoveBGExecute();
}
VERIFY(0x0234C5A0,daObjGryw00_Execute);
static BOOL daObjGryw00_Draw(daObjGryw00_c* p) {
    WWHD_FUNC(0x0234C5A4,BOOL,p); return p->Draw_v();
}
VERIFY(0x0234C5A4,daObjGryw00_Draw);
static BOOL daObjGryw00_IsDelete(daObjGryw00_c* p) {
    WWHD_FUNC(0x0234C5B4,BOOL,p); return p->IsDelete_v();
}
VERIFY(0x0234C5B4,daObjGryw00_IsDelete);

/* ---- leftover functions of the translation unit ---- */

/* 0234D38C sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10029954, after the destructor 0234D310 */
static void gryw00_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x0234D38C, void, p);
}
VERIFY(0x0234D38C, gryw00_SafeString_assureTermination);
