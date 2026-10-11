// Metal bars for the Pirate Ship rope minigame. Verified WWHD behavior.
#include "d/actor/d_a_obj_ashut.h"
#include "JSystem/J3DGraph.h"
#include <cmath>
using daObjAshut::Act_c;
// HD: tww provides names and method layout; its method bodies are nonmatching.
static Mtx34 sMatrixStack;
static Mtx34 sBackgroundMatrix;
static void* sResourceManager;
static AshutSaveData* sSaveData;
static AshutActionTable sModeActions;
static AshutHeaderStatics sHeaderStatics;
static AshutDestructorRecord sDestructorRecords;
static bool isSwitch(Act_c* actor) {
    u32 bit = actor->prm_get(8, 0);
    AshutSaveData* save = sSaveData.get();
    s8 room = actor->home.roomNo;
    return ashutCheckSwitch(&save->mSwitch, bit, room) != 0;
}

static AshutBackground* background(Act_c* actor) {
    return static_cast<AshutBackground*>(static_cast<void*>(actor->mpBgW.get()));
}

/* 0231974C */
void ashut_actorDtor(Act_c* actor, s32 flags) {
    if(actor) {
        ashutDestroyActor(actor,0);
        if(flags & 1) ashutDeleteObject(actor);
    }
}

/* 02318A74 */
s32 daObjAshut::Act_c::Mthd_Create() {
    u32 condition=actor_condition;
    if(!(condition&8)) {
        if(this) {
            ashutConstructMoveBg(this);
            __vtbl=0x100251E0;
            condition=actor_condition;
        }
        actor_condition=condition|8;
    }
    s32 phase=ashutLoadResource(&this->phase,"Ashut");
    if(phase==cPhs_COMPLEATE_e) {
        phase=ashutCreateMoveBg(this,"Ashut",7,0,0x760);
        if(phase!=cPhs_COMPLEATE_e && phase!=cPhs_ERROR_e) ashutAssert("d_a_obj_ashut.cpp",0x137,"(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
        if(isSwitch(this) && mpBgW->m_id<0x100) {
            AshutGameInfo* play=ashutGetGameInfo();
            dBgW* bg=mpBgW;
            ashutReleaseBackground(&play->mBgS,bg);
        }
    }
    return phase;
}

/* 02318B98 */
s32 daObjAshut::Act_c::Mthd_Delete() {
    s32 result=ashutDeleteMoveBg(this);
    ashutDeleteResource(&phase,"Ashut");
    return result;
}

/* 02318BE4 */
s32 daObjAshut::Act_c::CreateHeap() {
    gabi::Local<SafeString> name;
    name->mStringTop=gabi::guest_addr("Ashut");
    name->__vtbl=0x10025074;
    void* resource=ashutGetModelResource(sResourceManager,name.get(),4);
    if(!resource) ashutAssert("d_a_obj_ashut.cpp",0xF9,"mdl_data != 0");
    J3DModel* model=ashutCreateModel(resource,0x80000,0x11000022);
    this->model=model;
    return model!=0;
}

/* 02318C80 */
void daObjAshut::Act_c::set_mtx() {
    Mtx34* stack=&sMatrixStack;
    f32 x=current.pos.x,y=current.pos.y,z=current.pos.z;
    ashutTranslateMatrix(stack,x,y,z);
    s16 rx=shape_angle.x,ry=shape_angle.y,rz=shape_angle.z;
    ashutRotateMatrix(stack,rx,ry,rz);
    ashutCopyMatrix(stack,&sBackgroundMatrix);
    f32 values[12];
    values[7]=stack->m[1][3]+f32(height);
    stack->m[1][3]=values[7];
    J3DModel* model=this->model;
    for(unsigned i=0;i<12;++i) if(i!=7) values[i]=stack->m[i/4][i%4];
    for(unsigned i=0;i<12;++i) model->mBaseTransformMtx.m[i/4][i%4]=values[i];
}

/* 02318D60 */
void daObjAshut::Act_c::init_mtx() {
    J3DModel* model=this->model;
    // HD: scale copies are words; matrix copies below pass through float registers.
    AshutVectorWords* source=reinterpret_cast<AshutVectorWords*>(&scale);
    AshutVectorWords* destination=reinterpret_cast<AshutVectorWords*>(&model->mBaseScale);
    u32 y=source->y,x=source->x,z=source->z;
    destination->x=x;
    destination->y=y;
    destination->z=z;
    set_mtx();
}

/* 02318D80 */
void daObjAshut::Act_c::mode_upper_init() {
    mode=MODE_UPPER;
    height=250.0f;
}

/* 02318D98 */
void daObjAshut::Act_c::mode_lower_init() {
    mode=MODE_LOWER;
    height=0.0f;
}

/* 02318DB0 */
s32 daObjAshut::Act_c::Create() {
    background(this)->mStickWall=1;
    J3DModel* model=this->model;
    cullMtx=model?gabi::ea(&model->mBaseTransformMtx):0;
    init_mtx();
    ashutSetCullBox(this,-80.0f,-5.0f,-30.0f,80.0f,250.0f,30.0f);
    demoStarted=0;
    u8 event=prm_get(8,8);
    AshutGameInfo* play=ashutGetGameInfo();
    eventIndex=ashutGetEventIndex(&play->mEvtManager,0,event);
    requestedMode=MODE_NONE;
    if(isSwitch(this)) mode_upper_init();
    else mode_lower_init();
    return 1;
}

/* 02318ED0 */
s32 daObjAshut::Act_c::Execute(be<u32>* output) {
    (this->*sModeActions.mEntries[u32(mode)])();
    f32 height=this->height,y=current.pos.y,z=current.pos.z,x=current.pos.x;
    eyePos.z=z;
    eyePos.x=x;
    eyePos.y=gabi::fadds_ppc(y,height);
    set_mtx();
    *output=gabi::ea(&sBackgroundMatrix);
    return 1;
}

/* 02318FC4 */
s32 daObjAshut::Act_c::Draw() {
    void* environment=ashutGetEnvironment();
    ashutSetLight(environment,1,&current.pos,&tevStr);
    environment=ashutGetEnvironment();
    J3DModel* model=this->model;
    ashutApplyLight(environment,model,&tevStr);
    model=this->model;
    ashutUpdateModel(model,0);
    return 1;
}

/* 02319020 */
u8 daObjAshut::Act_c::chk_safe_area() {
    s16 angle=-s16(shape_angle.y);
    AshutGameInfo* play=ashutGetGameInfo();
    fopAc_ac_c* player=play->mpPlayer;
    gabi::Local<cXyz> difference,local;
    ashutSubtractVector(&player->current.pos,difference.get(),&current.pos);
    ashutRotateY(&sMatrixStack,angle);
    ashutTransformVector(&sMatrixStack,difference.get(),local.get());
    return (std::fabs(f32(local->y))<100.0f) && (std::fabs(f32(local->z))<55.0f) && (std::fabs(f32(local->x))<105.0f);
}

/* 023190E8 */
void daObjAshut::Act_c::mode_u_l_init() {
    s8 room=current.roomNo;
    bounceCount=5;
    velocity=0.0f;
    mode=MODE_U_L;
    s32 reverb=ashutGetReverb(room);
    ashutStartSound(27038,&eyePos,0,reverb);
}

/* 0231914C */
void daObjAshut::Act_c::mode_l_u_init() {
    s8 room=current.roomNo;
    bounceCount=5;
    velocity=0.0f;
    delay=14;
    mode=MODE_L_U;
    s32 reverb=ashutGetReverb(room);
    ashutStartSound(27037,&eyePos,0,reverb);
}

/* 023191B8 */
void daObjAshut::Act_c::mode_demoreq_init(s32 mode) {
    if(mode!=MODE_U_L && mode!=MODE_L_U) ashutAssert("d_a_obj_ashut.cpp",0x221,"(i_demo_next == Mode_U_L) || (i_demo_next == Mode_L_U)");
    if(demoStarted) {
        if (mode == MODE_U_L) mode_u_l_init();
        else mode_l_u_init();
    }
    else {
        requestedMode=mode;
        this->mode=MODE_DEMOREQ;
    }
}

/* 02319270 */
void daObjAshut::Act_c::mode_upper() {
    if(!isSwitch(this) && !chk_safe_area()) {
        if(mpBgW->m_id>=0x100) {
            AshutGameInfo* play=ashutGetGameInfo();
            dBgW* bg=mpBgW;
            ashutRegisterBackground(&play->mBgS,bg,this);
        }
        mode_demoreq_init(MODE_U_L);
    }
}

/* 02319308 */
void daObjAshut::Act_c::mode_u_l() {
    f32 velocity=(f32(this->velocity)-2.5f)*0.95f,height=f32(this->height)+velocity;
    this->velocity=velocity;
    this->height=height;
    if(height>0.0f) return;
    u8 count=bounceCount;
    if(!count) {
        mode_lower_init();
        return;
    }
    height=f32(this->height)*-0.5f;
    velocity=f32(this->velocity)*-0.5f;
    this->height=height;
    bounceCount=count-1;
    this->velocity=velocity;
    if(velocity>7.0f) this->velocity=7.0f;
}

/* 02319394 */
void daObjAshut::Act_c::mode_lower() {
    if(isSwitch(this)) mode_demoreq_init(MODE_L_U);
}

/* 023193F8 */
void daObjAshut::Act_c::mode_l_u() {
    s16 delay=this->delay;
    if(delay>0) this->delay=delay-1;
    f32 velocity=f32(this->velocity)+0.8f;
    delay=this->delay;
    this->velocity=velocity;
    velocity*=1.0f-((delay>0 && delay<4)?0.4f:0.1f);
    f32 height=f32(this->height)+velocity;
    this->height=height;
    this->velocity=velocity;
    if(height>150.0f && mpBgW->m_id<0x100) {
        AshutGameInfo* play=ashutGetGameInfo();
        dBgW* bg=mpBgW;
        ashutReleaseBackground(&play->mBgS,bg);
        height=this->height;
    }
    if(height<250.0f) return;
    u8 count=bounceCount;
    if(!count) {
        mode_upper_init();
        return;
    }
    height=this->height;
    velocity=f32(this->velocity)*-0.6f;
    height=gabi::fmadds(height-250.0f,-0.6f,250.0f);
    bounceCount=count-1;
    this->velocity=velocity;
    this->height=height;
    if(velocity<-4.0f) this->velocity=-4.0f;
}

/* 0231956C */
void daObjAshut::Act_c::mode_demoreq() {
    s16 event=eventIndex;
    AshutGameInfo* play=ashutGetGameInfo();
    u32 data=ashutGetEventData(&play->mEvtManager,event);
    if(data && eventInfo.mCommand!=dEvtCmd_INDEMO_e) {
        u8 id=prm_get(8,8);
        event=eventIndex;
        ashutOrderEvent(this,event,id,0xFFFF,0,1);
        eventInfo.mCondition=(u16)eventInfo.mCondition|2;
        return;
    }
    if(data) demoStarted=1;
    s32 mode=requestedMode;
    if (mode == MODE_U_L) mode_u_l_init();
    else mode_l_u_init();
}

/* 02319664 */
s32 ashut_createWrapper(Act_c* actor) {
    return actor->Mthd_Create();
}

/* 02319668 */
s32 ashut_deleteWrapper(Act_c* actor) {
    return actor->Mthd_Delete();
}

/* 0231966C */
s32 ashut_executeWrapper(Act_c* actor) {
    return ashutExecuteMoveBg(actor);
}

/* 02319670 */
s32 ashut_drawWrapper(Act_c* actor) {
    return actor->Draw_v();
}

/* 02319680 */
s32 ashut_isDeleteWrapper(Act_c* actor) {
    return actor->IsDelete_v();
}

/* 02319690 */
void ashut_staticInit() {
    sHeaderStatics.mZero[2]=0;
    sHeaderStatics.mZero[0]=0;
    sHeaderStatics.mZero[3]=0;
    sHeaderStatics.mZero[1]=0;
    ashutRegisterGlobal(&sDestructorRecords.mRecords[0]);
    sHeaderStatics.mMinAngle=-3.1415927410125732f;
    sHeaderStatics.mMaxAngle=3.1415927410125732f;
    ashutInitializeHeaderByte(&sHeaderStatics.mHeaderByte);
    ashutRegisterGlobal(&sDestructorRecords.mRecords[1]);
    ashutInitializeHeaderFlag(&sHeaderStatics.mHeaderFlag);
    ashutRegisterGlobal(&sDestructorRecords.mRecords[2]);
}

/* 02319724 */
void ashut_staticDtor(void* object, s32 flags) {
    if(object && (flags&1)) ashutDeleteObject(object);
}

/* 02319738 */
s32 daObjAshut::Act_c::IsDelete() {
    return 1;
}

/* 02319744 */
s32 daObjAshut::Act_c::Delete() {
    return 1;
}

/* 023197A0 */
u32 daObjAshut::Act_c::prm_get(u32 width, u32 shift) {
    u32 mask=width&0x20?0:(1u<<(width&31));
    u32 value=shift&0x20?0:(u32(mParameters)>>(shift&31));
    return value&(mask-1);
}

/* ---- leftover functions of the translation unit ---- */
/* 02319740 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10025088, after the destructor 02319724 */
/* 02319740 */
static void ashut_SafeString_assureTermination(void* p) {
}

/* WWHD glue
str 100251D8
str 1002508C in 02318A74
str 100250A0
str 100250E4 in 02318BE4
str 100250F8
str 10025158 in 023191B8
str 1002516C
data sMatrixStack 1048D0CC
data sBackgroundMatrix 104690F4
data sResourceManager 101F4F28
data sSaveData 101F84DC
data sModeActions 10025124
data sHeaderStatics 104690D8
data sDestructorRecords 101C7C74
ext 025BA0C0 s32 ashutCheckSwitch(void*, u32, s8)
ext 025D50BC void ashutDestroyActor(Act_c*, s32)
ext 0273AF40 void ashutDeleteObject(void*)
ext 024F1D40 void ashutConstructMoveBg(Act_c*)
ext 02520460 s32 ashutLoadResource(request_of_phase_process_class*, const char*)
ext 024F1D9C s32 ashutCreateMoveBg(Act_c*, const char*, u32, u32, u32)
ext 0273AA24 void ashutAssert(const char*, s32, const char*)
ext 025200D4 AshutGameInfo* ashutGetGameInfo()
ext 020087EC void ashutReleaseBackground(void*, void*)
ext 024F1F64 s32 ashutDeleteMoveBg(Act_c*)
ext 025204C8 void ashutDeleteResource(request_of_phase_process_class*, const char*)
ext 026066C4 void* ashutGetModelResource(void*, SafeString*, s32)
ext 025E38E0 J3DModel* ashutCreateModel(void*, u32, u32)
ext 028E93CC void ashutTranslateMatrix(void*, f32, f32, f32)
ext 025F1B48 void ashutRotateMatrix(void*, s16, s16, s16)
ext 028E90D4 void ashutCopyMatrix(void*, void*)
ext 025D674C void ashutSetCullBox(Act_c*, f32, f32, f32, f32, f32, f32)
ext 02543F10 s32 ashutGetEventIndex(void*, u32, u32)
ext 02555D0C void* ashutGetEnvironment()
ext 025626A4 void ashutSetLight(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void ashutApplyLight(void*, void*, dKy_tevstr_c*)
ext 025E2DE0 void ashutUpdateModel(void*, u32)
ext 0201ADE0 void ashutSubtractVector(void*, cXyz*, cXyz*)
ext 025F1884 void ashutRotateY(void*, s16)
ext 028E9044 void ashutTransformVector(void*, cXyz*, cXyz*)
ext 02520540 s32 ashutGetReverb(s8)
ext 025E1A40 void ashutStartSound(u32, cXyz*, u32, s32)
ext 024EEA6C void ashutRegisterBackground(void*, void*, Act_c*)
ext 02544044 u32 ashutGetEventData(void*, s16)
ext 025D7A58 void ashutOrderEvent(Act_c*, s16, u8, u32, u32, u32)
ext 024F1E9C s32 ashutExecuteMoveBg(Act_c*)
ext 028F026C void ashutRegisterGlobal(void*)
ext 028ED6F8 void ashutInitializeHeaderByte(void*)
ext 028EAB2C void ashutInitializeHeaderFlag(void*)
*/
