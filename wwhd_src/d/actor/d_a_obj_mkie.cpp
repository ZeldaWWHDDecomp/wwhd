/* WWHD light-sensitive breakable statue. Derived game code: */
#include "bindings.h"
struct Brk_l { J3DFrameCtrl ctrl; u8 tail[0x68]; };
struct Mkie : dBgS_MoveBgActor {
    request_of_phase_process_class phase;
    gptr<J3DModel> baseModel,vanishModel;
    Brk_l brk;
    dCcD_Tri triangles[8]; dCcD_Stts status[8];
    be<s32> mode,type;
    be<u8> ccDirty,removeStatue,eventPlaying,animationDone;
    be<s16> timer,lightCount,eventIndex;
    be<u8> active,broken;
    s32 Mthd_Create(); BOOL Mthd_Delete(); BOOL CreateHeap(); BOOL Create(); BOOL Execute(be<u32>*); BOOL Draw();
    void set_mtx(); void init_mtx(); void set_cc_pos(); void init_cc(); BOOL chk_light();
    void eff_break(); void sound_melt(); void sound_break(); void vib_break();
    void mode_wait(); void mode_demoWait_init(); void mode_demo_init(); void mode_demoWait(); void mode_demo(); void mode_proc_call();
};
WWHD_OFFSET(Mkie,baseModel,0x3E8); WWHD_OFFSET(Mkie,brk,0x3F0);
WWHD_OFFSET(Mkie,triangles,0x468); WWHD_OFFSET(Mkie,status,0xEE8);
WWHD_OFFSET(Mkie,mode,0x10C8); WWHD_SIZE(Mkie,0x10DC);
static u32 PrmAbstract(Mkie* p,u32 width,u32 shift) {
    WWHD_FUNC(0x0237167C,u32,p,width,shift);
    u32 parameters=p->mParameters;
    u32 mask=(width&32)?0:(1U<<(width&31));
    u32 value=(shift&32)?0:(parameters>>(shift&31));
    return value&(mask-1);
}
VERIFY(0x0237167C,PrmAbstract);
static u32 attribute(Mkie* p,u32 file,u32 expression) {
    u32 type=p->type;
    if (type>=2) { JUT_ASSERT_fail(STR(file),0x183,STR(expression)); type=p->type; }
    return 0x101CABF0+type*0x128;
}
s32 Mkie::Mthd_Create() {
    WWHD_FUNC(0x0236FFCC,s32,this);
    s32 phaseState=5; u32 condition=actor_condition;
    if (!(condition&8)) {
        if (gabi::ea(this)!=0) {
            dBgS_MoveBgActor::ct(this); __vtbl=0x1002C744;
            gabi::call(0x025E80D0,&brk);
            gabi::call(0x028EFFD0,&triangles[0],8,0x150,0x02371548);
            gabi::call(0x028EFFD0,&status[0],8,0x3C,0x023714E0);
            condition=actor_condition;
        }
        actor_condition=condition|8;
    }
    u32 sw=PrmAbstract(this,8,16);
    active=dComIfGs_isSwitch(sw,home.roomNo)==0;
    if (!active) return phaseState;
    phaseState=dComIfG_resLoad(&phase,STR(0x1002C6E8));
    if (phaseState==4) {
        type=PrmAbstract(this,1,8);
        u32 a=attribute(this,0x1002C4C0,0x1002C4D4);
        f32 size=gabi::load<f32>(a+4);
        scale.y=size; scale.x=size; scale.z=size;
        phaseState=MoveBGCreate(STR(0x1002C6E8),15,0x024EE76C,0x1B20);
        if (phaseState!=4 && phaseState!=5) JUT_ASSERT_fail(STR(0x1002C4C0),0x217,STR(0x1002C4FC));
    }
    return phaseState;
}
VERIFY(0x0236FFCC,&Mkie::Mthd_Create);
BOOL Mkie::Mthd_Delete() {
    WWHD_FUNC(0x02370170,BOOL,this);
    BOOL result=TRUE;
    if (active) { result=MoveBGDelete(); dComIfG_resDelete(&phase,STR(0x1002C6E8)); }
    return result;
}
VERIFY(0x02370170,&Mkie::Mthd_Delete);
BOOL Mkie::CreateHeap() {
    WWHD_FUNC(0x023701D0,BOOL,this);
    auto baseData=gabi::at<J3DModelData>(gabi::ea(dComIfG_getObjectRes(STR(0x1002C6E8),6,0x1002C480)));
    if (!baseData) JUT_ASSERT_fail(STR(0x1002C564),0x194,STR(0x1002C540));
    baseModel=mDoExt_J3DModel__create(baseData,0x80000,0x11000022);
    auto vanishData=gabi::at<J3DModelData>(gabi::ea(dComIfG_getObjectRes(STR(0x1002C6E8),9,0x1002C480)));
    if (!vanishData) JUT_ASSERT_fail(STR(0x1002C564),0x19D,STR(0x1002C550));
    vanishModel=mDoExt_J3DModel__create(vanishData,0x80000,0x11000022);
    void* animation=dComIfG_getObjectRes(STR(0x1002C6E8),12,0x1002C480);
    if (!animation) JUT_ASSERT_fail(STR(0x1002C564),0x1A7,STR(0x1002C578));
    s32 ready=gabi::call<s32>(0x025E8154,&brk,vanishData,animation,1,0,1.0f,0,-1,0,0);
    if (!baseModel || !vanishModel || !ready) return FALSE;
    return TRUE;
}
VERIFY(0x023701D0,&Mkie::CreateHeap);
void Mkie::set_mtx() {
    WWHD_FUNC(0x02370338,void,this);
    f32 x=current.pos.x,z=current.pos.z,y=current.pos.y;
    mDoMtx_stack_c::transS(x,y,z);
    s16 rx=shape_angle.x,rz=shape_angle.z,ry=shape_angle.y;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(),rx,ry,rz);
    mtx_copy(gabi::at<Mtx34>(gabi::ea(baseModel)+0xC8),mDoMtx_stack_c::get());
    mtx_copy(gabi::at<Mtx34>(gabi::ea(vanishModel)+0xC8),mDoMtx_stack_c::get());
    x=scale.x; y=scale.y; z=scale.z;
    gabi::call(0x025F2518,x,y,z);
    gabi::call(0x028E90D4,mDoMtx_stack_c::get(),gabi::at<Mtx34>(0x1046A558));
}
VERIFY(0x02370338,&Mkie::set_mtx);
void Mkie::init_mtx() {
    WWHD_FUNC(0x02370480,void,this);
    u32 m=gabi::ea(baseModel); f32 y=scale.y,x=scale.x,z=scale.z;
    gabi::store<f32>(m+0xBC,x); gabi::store<f32>(m+0xC0,y); gabi::store<f32>(m+0xC4,z);
    m=gabi::ea(vanishModel); x=scale.x; z=scale.z; y=scale.y;
    gabi::store<f32>(m+0xC4,z); gabi::store<f32>(m+0xBC,x); gabi::store<f32>(m+0xC0,y);
    set_mtx();
}
VERIFY(0x02370480,&Mkie::init_mtx);
void Mkie::set_cc_pos() {
    WWHD_FUNC(0x023704BC,void,this);
    gabi::Local<cXyz[3]> source,destination;
    for (u32 i=0;i<8;++i) {
        for (u32 vertex=0;vertex<3;++vertex) {
            u32 a=attribute(this,0x1002C588,0x1002C59C)+8+i*36+vertex*12;
            auto v=gabi::at<cXyz>(source.a+vertex*12);
            v->x=gabi::load<f32>(a); v->y=gabi::load<f32>(a+4); v->z=gabi::load<f32>(a+8);
        }
        auto matrix=J3DModel_getBaseTRMtx(baseModel);
        gabi::call(0x028E8FB8,matrix,gabi::at<cXyz>(source.a),gabi::at<cXyz>(destination.a),3);
        gabi::call(0x0201924C,gabi::at<void>(gabi::ea(this)+0x580+i*0x150),gabi::at<cXyz>(destination.a),gabi::at<cXyz>(destination.a+12),gabi::at<cXyz>(destination.a+24));
    }
}
VERIFY(0x023704BC,&Mkie::set_cc_pos);
void Mkie::init_cc() {
    WWHD_FUNC(0x02370618,void,this);
    for (u32 i=0;i<8;++i) {
        status[i].Init(255,255,this);
        gabi::call(0x0251650C,&triangles[i],gabi::at<dCcD_SrcTri>(0x1002C6F0));
        u32 tri=gabi::ea(&triangles[i]),flags=gabi::load<u32>(tri+0x94);
        gabi::store<u32>(tri+0x44,gabi::ea(&status[i]));
        gabi::store<u32>(tri+0x94,flags|4);
    }
    set_cc_pos();
}
VERIFY(0x02370618,&Mkie::init_cc);
BOOL Mkie::Create() {
    WWHD_FUNC(0x02370698,BOOL,this);
    cullMtx=gabi::ea(J3DModel_getBaseTRMtx(baseModel)); init_mtx();
    u32 box[6]; for (u32 i=0;i<6;++i) box[i]=attribute(this,0x1002C5D4,0x1002C5E8);
    f32 x0=-80.0f*gabi::load<f32>(box[0]+4),x1=80.0f*gabi::load<f32>(box[3]+4);
    f32 y0=-gabi::load<f32>(box[1]+4),z0=-80.0f*gabi::load<f32>(box[2]+4);
    f32 y1=230.0f*gabi::load<f32>(box[4]+4),z1=80.0f*gabi::load<f32>(box[5]+4);
    fopAcM_setCullSizeBox(this,x0,y0,z0,x1,y1,z1); init_cc();
    if (PrmAbstract(this,1,13)) { u32 value=actor_status; gbaName=0; actor_status=value&0xFFFFFFC0; }
    u32 attributeType=type;
    ccDirty=0; animationDone=0;
    if (attributeType>=2) { JUT_ASSERT_fail(STR(0x1002C5D4),0x183,STR(0x1002C5E8)); attributeType=type; }
    u32 a=0x101CABF0+attributeType*0x128;
    f32 y=current.pos.y,z=gabi::load<f32>(gabi::ea(this)+0x398),x=gabi::load<f32>(gabi::ea(this)+0x390);
    f32 attentionY=gabi::fmadds(130.0f,gabi::load<f32>(a+4),y);
    eyePos.z=z; eyePos.x=x; gabi::store<f32>(gabi::ea(this)+0x394,attentionY); eyePos.y=attentionY;
    a=attribute(this,0x1002C5D4,0x1002C5E8);
    const char* name=gabi::at<char>(gabi::load<u32>(a));
    u8 event=PrmAbstract(this,8,0); auto manager=dComIfGp_getPEvtManager();
    eventIndex=gabi::call<s32>(0x02543F10,manager,name,event); return TRUE;
}
VERIFY(0x02370698,&Mkie::Create);
void Mkie::mode_proc_call() {
    WWHD_FUNC(0x02370914,void,this);
    u32 value=mode; ptmf_call(0x1002C610+value*8,this);
}
VERIFY(0x02370914,&Mkie::mode_proc_call);
BOOL Mkie::Execute(be<u32>* outMtx) {
    WWHD_FUNC(0x02370960,BOOL,this,outMtx);
    gabi::store<u32>(gabi::ea(this)+0x390,gabi::load<u32>(gabi::ea(this)+0x314));
    u32 a=attribute(this,0x1002C628,0x1002C63C);
    f32 z=current.pos.z,x=gabi::load<f32>(gabi::ea(this)+0x390),y=current.pos.y;
    f32 height=gabi::fmadds(130.0f,gabi::load<f32>(a+4),y);
    eyePos.z=z; gabi::store<f32>(gabi::ea(this)+0x398,z); eyePos.x=x;
    gabi::store<f32>(gabi::ea(this)+0x394,height); eyePos.y=height;
    mode_proc_call(); if (removeStatue) fopAcM_delete(this);
    ccDirty=0; set_mtx(); *outMtx=0x1046A558; return TRUE;
}
VERIFY(0x02370960,&Mkie::Execute);
BOOL Mkie::Draw() {
    WWHD_FUNC(0x02370A3C,BOOL,this);
    if (!animationDone) {
        bool vanish=mode==2 && timer==0;
        settingTevStruct(dKy_getEnvlight(),0,&current.pos,&tevStr);
        auto env=dKy_getEnvlight(); setLightTevColorType(env,vanish?(J3DModel*)vanishModel:(J3DModel*)baseModel,&tevStr);
        if (vanish) { auto model=(J3DModel*)vanishModel; f32 frame=brk.ctrl.mFrame; mDoExt_brkAnm_entry((mDoExt_brkAnm*)&brk,J3DModel_getModelData(model),frame); }
        dComIfGd_setListBG(); mDoExt_modelUpdateDL(vanish?(J3DModel*)vanishModel:(J3DModel*)baseModel); dComIfGd_setList();
    }
    return TRUE;
}
VERIFY(0x02370A3C,&Mkie::Draw);
BOOL Mkie::chk_light() {
    WWHD_FUNC(0x02370B64,BOOL,this);
    auto play=dComIfGp_get();
    if (gabi::call<s32>(0x0252A038,gabi::at<void>(gabi::ea(play)+0x5A20),&eyePos)) return TRUE;
    BOOL hit=FALSE;
    for (u32 i=0;i<8;++i) if (gabi::call<s32>(0x025162A4,&triangles[i])) { gabi::call(0x0251621C,&triangles[i]); hit=TRUE; }
    return hit;
}
VERIFY(0x02370B64,&Mkie::chk_light);
void Mkie::eff_break() {
    WWHD_FUNC(0x02370C00,void,this);
    gabi::Local<GXColor> color;
    color->g=(u8)gabi::load<s16>(gabi::ea(this)+0x1A2); color->b=(u8)gabi::load<s16>(gabi::ea(this)+0x1A4);
    color->a=(u8)gabi::load<s16>(gabi::ea(this)+0x1A6); color->r=(u8)gabi::load<s16>(gabi::ea(this)+0x1A0);
    dComIfGp_particle_set(0x81A0,&current.pos,&shape_angle,&scale,255,nullptr,-1,gabi::at<GXColor>(gabi::ea(this)+0x1A8),(GXColor*)color);
}
VERIFY(0x02370C00,&Mkie::eff_break);
void Mkie::sound_melt() { WWHD_FUNC(0x02370C90,void,this); s32 reverb=dComIfGp_getReverb(current.roomNo); mDoAud_seStart(0x69C2,&eyePos,0,reverb); }
VERIFY(0x02370C90,&Mkie::sound_melt);
void Mkie::sound_break() {
    WWHD_FUNC(0x02370CD8,void,this);
    u32 value=type; s8 room=current.roomNo;
    u32 sound=gabi::load<u32>(0x1002C664+value*4);
    s32 reverb=dComIfGp_getReverb(room); mDoAud_seStart(sound,&eyePos,0,reverb);
}
VERIFY(0x02370CD8,&Mkie::sound_break);
void Mkie::vib_break() {
    WWHD_FUNC(0x02370D38,void,this);
    auto vibration=dComIfGp_getVibration(); gabi::Local<cXyz> direction;
    direction->z=0.0f; direction->y=1.0f; direction->x=0.0f;
    gabi::call(0x025CB374,vibration,4,-33,(cXyz*)direction);
}
VERIFY(0x02370D38,&Mkie::vib_break);
void Mkie::mode_demoWait_init() { WWHD_FUNC(0x02370D88,void,this); eventPlaying=0; mode=1; }
VERIFY(0x02370D88,&Mkie::mode_demoWait_init);
void Mkie::mode_wait() {
    WWHD_FUNC(0x02370D9C,void,this);
    if (chk_light()) { s16 count=(s16)((s32)lightCount+1); lightCount=count; if (count>20) { mode_demoWait_init(); return; } }
    else lightCount=0;
    if (ccDirty) set_cc_pos();
    if (!(actor_condition&4)) for (u32 i=0;i<8;++i) { auto system=dComIfG_Ccsp(); gabi::call(0x0200E240,system,&triangles[i]); }
}
VERIFY(0x02370D9C,&Mkie::mode_wait);
void Mkie::mode_demo_init() {
    WWHD_FUNC(0x02370E6C,void,this); mode=2; timer=10;
    if (!PrmAbstract(this,1,12)) gabi::call(0x025E1988,0x806);
    broken=0; animationDone=0;
}
VERIFY(0x02370E6C,&Mkie::mode_demo_init);
void Mkie::mode_demoWait() {
    WWHD_FUNC(0x02370ECC,void,this);
    s16 index=eventIndex;
    if (gabi::call<u32>(0x02544044,dComIfGp_getPEvtManager(),index)) {
        if (gabi::load<u16>(gabi::ea(this)+0xF8)==2) { eventPlaying=1; mode_demo_init(); }
        else {
            u8 event=PrmAbstract(this,8,0); index=eventIndex;
            fopAcM_orderOtherEventId(this,index,event,0xFFFF,0,1);
            u16 condition=gabi::load<u16>(gabi::ea(this)+0xFA); gabi::store<u16>(gabi::ea(this)+0xFA,condition|2);
        }
    } else mode_demo_init();
}
VERIFY(0x02370ECC,&Mkie::mode_demoWait);
void Mkie::mode_demo() {
    WWHD_FUNC(0x02370F98,void,this);
    s16 count=timer; bool play=count==0;
    if (count>0) {
        count=(s16)(count-1); timer=count;
        if (count==0) { eff_break(); sound_melt(); u32 sw=PrmAbstract(this,8,16); dComIfGs_onSwitch(sw,home.roomNo); play=timer==0; }
    }
    if (play) {
        u32 done=gabi::call<u32>(0x025E742C,&brk); u8 wasBroken=broken;
        animationDone=done!=0;
        if (!wasBroken && done) { sound_break(); vib_break(); broken=1; }
    }
    if (eventPlaying) {
        s16 index=eventIndex;
        if (gabi::call<s32>(0x025440C8,dComIfGp_getPEvtManager(),index)) { dComIfGp_event_reset(); s16 remaining=timer; eventPlaying=0; if (remaining!=0) return; }
        else if (eventPlaying) return;
    }
    if (timer==0 && animationDone) removeStatue=1;
}
VERIFY(0x02370F98,&Mkie::mode_demo);
static s32 CreateWrapper(Mkie* p) { WWHD_FUNC(0x023710EC,s32,p); return p->Mthd_Create(); }
VERIFY(0x023710EC,CreateWrapper);
static BOOL DeleteWrapper(Mkie* p) { WWHD_FUNC(0x023710F0,BOOL,p); return p->Mthd_Delete(); }
VERIFY(0x023710F0,DeleteWrapper);
static BOOL ExecuteWrapper(Mkie* p) { WWHD_FUNC(0x023710F4,BOOL,p); return p->MoveBGExecute(); }
VERIFY(0x023710F4,ExecuteWrapper);
static BOOL DrawWrapper(Mkie* p) { WWHD_FUNC(0x023710F8,BOOL,p); return p->Draw_v(); }
VERIFY(0x023710F8,DrawWrapper);
static BOOL IsDeleteWrapper(Mkie* p) { WWHD_FUNC(0x02371108,BOOL,p); return p->IsDelete_v(); }
VERIFY(0x02371108,IsDeleteWrapper);
static void sinit() {
    WWHD_FUNC(0x02371118,void,(u32)0);
    sinit_header_statics(0x1046A53C,0x101CAB7C);
    static const f32 vertices[2][8][9]={
      {{-30,190,40,-65,50,75,65,50,75},{-30,190,40,65,50,75,30,190,40},
       {38,190,20,75,50,65,75,50,-65},{38,190,20,75,50,-65,38,190,-40},
       {30,190,-60,65,50,-75,-65,50,-75},{30,190,-60,-65,50,-75,-30,190,-60},
       {-38,190,-40,-75,50,-65,-75,50,65},{-38,190,-40,-75,50,65,-38,190,20}},
      {{-90,570,170,-195,150,225,195,150,225},{-90,570,170,195,150,225,90,570,170},
       {120,570,60,225,150,195,225,150,-195},{120,570,60,225,150,-195,120,570,-120},
       {90,570,-180,195,150,-225,-195,150,-225},{90,570,-180,-195,150,-225,-90,570,-180},
       {-120,570,-120,-225,150,-195,-225,150,195},{-120,570,-120,-225,150,195,-120,570,60}}
    };
    for (u32 type=0;type<2;++type) for (u32 i=0;i<8;++i) for (u32 coordinate=0;coordinate<9;++coordinate)
        gabi::store<f32>(0x101CABF8+type*0x128+i*36+coordinate*4,vertices[type][i][coordinate]);
}
VERIFY(0x02371118,sinit);
static void trivial_dt(void* p,s32 flags) { WWHD_FUNC(0x023714CC,void,p,flags); if (p && (flags&1)) operator_delete(p); }
VERIFY(0x023714CC,trivial_dt);
static dCcD_Stts* status_ct(dCcD_Stts* p) {
    WWHD_FUNC(0x023714E0,dCcD_Stts*,p);
    if (!p) p=gabi::call<dCcD_Stts*>(0x0273AD10,0x3C);
    if (p) {
        gabi::call(0x0200BD2C,p); gabi::call(0x02515DA0,gabi::at<void>(gabi::ea(p)+0x1C));
        gabi::store<u32>(gabi::ea(p)+0x18,0x1004AE88); gabi::store<u32>(gabi::ea(p)+0x1C,0x1004AEC0);
    }
    return p;
}
VERIFY(0x023714E0,status_ct);
static dCcD_Tri* triangle_ct(dCcD_Tri* p) {
    WWHD_FUNC(0x02371548,dCcD_Tri*,p);
    if (!p) p=gabi::call<dCcD_Tri*>(0x0273AD10,0x150);
    if (p) {
        u32 a=gabi::ea(p); gabi::call(0x02515FB8,p);
        gabi::store<u32>(a+0x114,0x100015A8); gabi::store<u32>(a+0x110,0x1002C498);
        gabi::call(0x02019040,gabi::at<void>(a+0x118));
        gabi::store<u32>(a+0x3C,0x1004B010); gabi::store<u32>(a+0x128,0x1004B058); gabi::store<u32>(a+0x114,0x1004B068);
    }
    return p;
}
VERIFY(0x02371548,triangle_ct);
static BOOL IsDelete(Mkie* p) { WWHD_FUNC(0x023715D4,BOOL,p); return TRUE; }
VERIFY(0x023715D4,IsDelete);
static void empty_virtual(Mkie* p) { WWHD_FUNC(0x023715DC,void,p); }
VERIFY(0x023715DC,empty_virtual);
static BOOL Delete(Mkie* p) { WWHD_FUNC(0x023715E0,BOOL,p); return TRUE; }
VERIFY(0x023715E0,Delete);
static void destroy(Mkie* p,s32 flags) {
    WWHD_FUNC(0x023715E8,void,p,flags);
    if (p) {
        gabi::call(0x028F0164,&p->status[0],8,0x3C,0x02515860,0,0);
        gabi::call(0x028F0164,&p->triangles[0],8,0x150,0x025159F8,0,0);
        gabi::call(0x025D50BC,p,0); if (flags&1) operator_delete(p);
    }
}
VERIFY(0x023715E8,destroy);
