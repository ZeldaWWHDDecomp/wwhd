/* Earth Temple face statue. */
#include "d/actor/d_a_obj_Vds.h"
using daObjVds::Act_c;
const gc_tu_data gc_tu = {.safeString = 0x10032928};
struct VdsGlowTexture {
    u8 bytes[0x20];
};
struct VdsGlowRenderer {
    gptr<void> pointer;
};
static VdsGlowTexture glowTexture;
static VdsGlowRenderer glowRenderer;
static char event_name[4] = {86, 100, 115, 0};

// HD: private views cover fields absent from the shared headers.
struct VdsBgW : dBgW {
    u8 _04[0xA8 - 4];
    be<u32> mFlags;
};
struct VdsSwitch : fopAc_ac_c {
    u8 _3AC[0x1128 - 0x3AC];
    be<f32> mStrength;
};
struct VdsColorS16 {
    be<s16> rgba[4];
};
struct VdsColorF32 {
    be<f32> rgba[4];
};
struct VdsAnmVtable {
    u8 _00[0x14];
    be<u32> getFrameMax;
};
struct VdsAnmView {
    u8 _00[4];
    gptr<VdsAnmVtable> vtable;
};
static s32 getFrameMax(J3DAnmTevRegKey* animation) {
    auto* view = reinterpret_cast<VdsAnmView*>(animation);
    return gabi::call_ptr<s32>(view->vtable->getFrameMax, animation);
}
using InitProcess = gabi::ptmf<BOOL (Act_c::*)()>;
using MainProcess = gabi::ptmf<void (Act_c::*)()>;
struct InitProcesses {
    InitProcess entries[Act_c::PROCESS_COUNT];
};
struct MainProcesses {
    MainProcess entries[Act_c::PROCESS_COUNT];
};
static InitProcesses init_processes;
static MainProcesses main_processes;
struct VdsHeaderStorage {
    be<u32> words[4];
};
struct VdsFloat {
    be<f32> value;
};
struct VdsByte {
    u8 value;
};
struct VdsRegistration {
    u8 bytes[12];
};
static VdsHeaderStorage headerStorage;
static VdsFloat minAngle;
static VdsFloat maxAngle;
static VdsByte headerFlag0;
static VdsByte headerFlag1;
static VdsRegistration registration0;
static VdsRegistration registration1;
static VdsRegistration registration2;

static Act_c* Act_construct(Act_c* actor);
static BOOL solidHeapCB(fopAc_ac_c* actor);
static void* ds_search_switchCB(void* candidate, void* actor);
static LIGHT_INFLUENCE* light_construct(LIGHT_INFLUENCE* light);
// HD: private external declarations retain guest calls to this unit, including stubbed callees.
extern Act_c* Vds_construct(Act_c*);
extern void Vds_set_mtx(Act_c*);
extern BOOL Vds_create_heap(Act_c*);
extern BOOL Vds_process_init(Act_c*, s32);
extern void Vds_set_first_process(Act_c*);
extern void Vds_Event_init(Act_c*);
extern void Vds_Event_exe(Act_c*);
extern BOOL Vds_PlayLoopJointAnimation(Act_c*);
extern void Vds_delete_point_light(Act_c*);
extern void* Vds_search_switchCB(Act_c*, fopAc_ac_c*);
extern void Vds_create_point_light(Act_c*, s32, cXyz*);
extern void Vds_execute_point_light(Act_c*);
extern void Vds_process_common(Act_c*);
extern void Vds_process_main(Act_c*);
extern BOOL Vds_SetLoopJointAnimation(Act_c*, J3DAnmTransform*, J3DAnmTransform*, f32, f32);
extern s32 Vds_create(Act_c*);
extern BOOL Vds_delete(Act_c*);
extern BOOL Vds_execute(Act_c*);
extern BOOL Vds_draw(Act_c*);
extern u32 Vds_getParam(Act_c*, s32, s32);
using LightConstructor = LIGHT_INFLUENCE* (*)(LIGHT_INFLUENCE*);
extern void constructArray(void*, u32, u32, LightConstructor);
extern void Vds_setAnm(mDoExt_McaMorf*, J3DAnmTransform*, s32, s32, f32, f32, f32, f32);
extern void Vds_drawGlow(void*, cXyz*, VdsColorF32*, s32, void*, s32, s32, s32, f32, f32, f32);
extern void constructHeader0(void*);
extern void constructHeader1(void*);
extern void registerHeader(void*);
extern void* operatorNew(u32);
extern void operatorDelete(void*);
extern void Vds_color(VdsColorF32*, VdsColorS16*);

/* 023AEE1C */
static Act_c* Act_construct(Act_c* actor) {
    if (!actor)
        actor = static_cast<Act_c*>(operatorNew(sizeof(Act_c)));
    if (actor) {
        fopAc_ac_c_ct(actor);
        actor->__vtbl = 0x10032940;
        mDoExt_brkAnm::ct(&actor->mBrk0);
        mDoExt_brkAnm::ct(&actor->mBrk1);
        constructArray(actor->mLights, 2, sizeof(LIGHT_INFLUENCE), light_construct);
    }
    return actor;
}

/* 023AEE98 */
void Act_c::set_mtx() {
    J3DModel* model = M_anm0->getModel();
    f32 z = scale.z, x = scale.x, y = scale.y;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    model = M_anm0->getModel();
    model->setBaseTRMtx(mDoMtx_stack_c::get());
    model = M_anm1->getModel();
    model->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &mBgMtx);
}

/* 023AEFF4 */
BOOL Act_c::create_heap() {
    auto* mdl_data0 = static_cast<J3DModelData*>(dComIfG_getObjectRes("Vds", 10));
    if (!mdl_data0)
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x359, "mdl_data0 != 0");
    M_bck_data0 = static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Vds", 6));
    if (!M_bck_data0)
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x35D, "M_bck_data0 != 0");
    if (mdl_data0 && M_bck_data0)
        M_anm0 = mDoExt_McaMorf::create(nullptr, mdl_data0, nullptr, nullptr, M_bck_data0, 0, 1.0f, 0, -1, 1, nullptr,
                                        0, 0x11020203);
    if (!M_anm0)
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x36A, "M_anm0 != 0");
    auto* mdl_data1 = static_cast<J3DModelData*>(dComIfG_getObjectRes("Vds", 11));
    if (!mdl_data1)
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x36E, "mdl_data1 != 0");
    M_bck_data1 = static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Vds", 7));
    if (!M_bck_data1)
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x372, "M_bck_data1 != 0");
    if (mdl_data1 && M_bck_data1)
        M_anm1 = mDoExt_McaMorf::create(nullptr, mdl_data1, nullptr, nullptr, M_bck_data1, 0, 1.0f, 0, -1, 1, nullptr,
                                        0, 0x11020203);
    if (!M_anm1)
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x37F, "M_anm1 != 0");
    M_brk_data0 = static_cast<J3DAnmTevRegKey*>(dComIfG_getObjectRes("Vds", 14));
    J3DAnmTevRegKey* color = M_brk_data0;
    if (!color) {
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x384, "M_brk_data0 != 0");
        color = M_brk_data0;
    }
    s32 brk0 = mBrk0.init(mdl_data0, color, true, 0, 1.0f, 0, -1, false, 0);
    M_brk_data1 = static_cast<J3DAnmTevRegKey*>(dComIfG_getObjectRes("Vds", 15));
    color = M_brk_data1;
    if (!color) {
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x391, "M_brk_data1 != 0");
        color = M_brk_data1;
    }
    s32 brk1 = mBrk1.init(mdl_data1, color, true, 0, 1.0f, 0, -1, false, 0);
    Vds_set_mtx(this);
    auto* bgw_data = static_cast<cBgD_t*>(dComIfG_getObjectRes("Vds", 18));
    if (!bgw_data)
        JUT_ASSERT_fail("d_a_obj_Vds.cpp", 0x3A7, "bgw_data != 0");
    else {
        mpBgW = new_dBgW();
        dBgW* bg = mpBgW;
        if (bg)
            bg->Set(bgw_data, dBgW::MOVE_BG_e, &mBgMtx);
    }
    return M_bck_data0 && M_anm0 && M_anm0->getModel() && M_bck_data1 && M_anm1 && M_anm1->getModel() && mpBgW &&
           M_brk_data0 && M_brk_data1 && brk0 && brk1;
}

// HD: free callback because autoglue does not yet support static-member callbacks.
/* 023AF400 */
static BOOL solidHeapCB(fopAc_ac_c* actor) {
    return Vds_create_heap(static_cast<Act_c*>(actor));
}

/* 023AF404 */
BOOL Act_c::process_init(s32 process) {
    if ((u32)process >= PROCESS_COUNT || !(this->*init_processes.entries[process])())
        return FALSE;
    mProcess = process;
    return TRUE;
}

/* 023AF4BC */
void Act_c::set_first_process() {
    s32 sw = Vds_getParam(this, 8, 0);
    // HD: this statue checks its home room, not the current-room accessor.
    s32 enabled = fopAcM_isSwitch(this, sw);
    Vds_process_init(this, enabled != 0);
}

/* 023AF51C */
void Act_c::Event_init() {
    mEvent = -1;
    mEventState = EVENT_NONE;
}

/* 023AF530 */
s32 Act_c::_create() {
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this)
            Vds_construct(this);
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    s32 phase = dComIfG_resLoad(&mPhase, "Vds");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0))
        return cPhs_ERROR_e;
    Vds_set_first_process(this);
    fopAcM_SetMtx(this, M_anm0->getModel()->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -2000.0f, -2000.0f, -2000.0f, 2000.0f, 2000.0f, 2000.0f);
    dBgS* background = dComIfG_Bgsp();
    dBgW* bg = mpBgW;
    background->Regist(bg, this);
    static_cast<VdsBgW*>(static_cast<dBgW*>(mpBgW))->mFlags = 0;
    Vds_Event_init(this);
    // HD: duplicate archive/event text is registered per function in the glue block.
    mEventIndex = dComIfGp_evmng_getEventIdx(reinterpret_cast<const char*>(event_name), 255);
    for (auto& id : mSwitchActor)
        id = -1;
    return phase;
}

/* 023AF674 */
void Act_c::delete_point_light() {
    for (auto& light : mLights)
        dKy_plight_cut(&light);
}

/* 023AF6BC */
BOOL Act_c::_delete() {
    dBgW* bg = mpBgW;
    if (heap && bg && bg->ChkUsed()) {
        dBgS* background = dComIfG_Bgsp();
        bg = mpBgW;
        background->Release(bg);
    }
    Vds_delete_point_light(this);
    dComIfG_resDelete(&mPhase, "Vds");
    return TRUE;
}

/* 023AF734 */
void Act_c::Event_exe() {
    s32 state = mEventState;
    if (state == EVENT_ORDER) {
        if (eventInfo.checkCommandDemoAccrpt())
            mEventState = EVENT_RUNNING;
        else {
            fopAcM_orderOtherEventId(this, mEvent, 255, 65535, 0, 1);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
        }
    } else if (state == EVENT_RUNNING) {
        s32 event = mEvent;
        if (dComIfGp_evmng_endCheck(event)) {
            dComIfGp_event_reset();
            Vds_Event_init(this);
        }
    }
}

/* 023AF834 */
BOOL Act_c::PlayLoopJointAnimation() {
    M_anm0->play(nullptr, 0, 0);
    M_anm1->play(nullptr, 0, 0);
    return TRUE;
}

/* 023AF888 */
void* Act_c::search_switchCB(fopAc_ac_c* candidate) {
    if (fopAc_IsActor(candidate) && candidate && fopAcM_GetName(candidate) == 30) {
        for (auto& id : mSwitchActor)
            if (id == -1) {
                id = fopAcM_GetID(candidate);
                break;
            }
    }
    return nullptr;
}

// HD: same callback ABI as tww; global placement avoids the namespace-function autoglue gap.
/* 023AF924 */
static void* ds_search_switchCB(void* candidate, void* actor) {
    return Vds_search_switchCB(static_cast<Act_c*>(actor), static_cast<fopAc_ac_c*>(candidate));
}

/* 023AF934 */
void Act_c::create_point_light(s32 index, cXyz* pos) {
    u32 i = (u32)index & 1;
    auto& light = mLights[i];
    light.mPos.copy(*pos);
    mLightPosition[i].copy(*pos);
    light.mColorR = 600;
    light.mColorG = 360;
    light.mColorB = 80;
    light.mPower = 0;
    light.mFluctuation = 0;
    dKy_plight_set(&light);
}

/* 023AF9A8 */
void Act_c::execute_point_light() {
    for (u32 i = 0; i < 2; i++) {
        auto& light = mLights[i];
        light.mPower = (f32)mStrength[i] * 700.0f;
        light.mPos.copy(mLightPosition[i]);
        light.mColorB = 80;
        light.mColorG = 360;
        light.mColorR = 600;
    }
}

/* 023AFA08 */
void Act_c::process_common() {
    u32 state = mLightStage;
    if (state == LIGHT_SEARCH) {
        for (auto& id : mSwitchActor)
            id = -1;
        fopAcM_Search(ds_search_switchCB, this);
        s32 id0 = mSwitchActor[0], id1 = mSwitchActor[1];
        if (id0 == -1 || id1 == -1)
            return;
        fopAc_ac_c* first = fopAcM_SearchByID(id0);
        fopAc_ac_c* second = fopAcM_SearchByID(mSwitchActor[1]);
        if (!first || !second)
            return;
        s16 difference = (s16)((s32)first->shape_angle.y - (s32)(s16)shape_angle.y);
        if (difference >= 0) {
            id0 = mSwitchActor[0];
            id1 = mSwitchActor[1];
            mSwitchActor[0] = id1;
            mSwitchActor[1] = id0;
        }
        mLightStage = LIGHT_CREATE;
    } else if (state == LIGHT_CREATE) {
        fopAc_ac_c* first = fopAcM_SearchByID(mSwitchActor[0]);
        fopAc_ac_c* second = fopAcM_SearchByID(mSwitchActor[1]);
        if (first && second) {
            Vds_create_point_light(this, 0, &first->current.pos);
            Vds_create_point_light(this, 1, &second->current.pos);
            mLightStage = LIGHT_EXECUTE;
        }
    } else if (state == LIGHT_EXECUTE)
        Vds_execute_point_light(this);
}

/* 023AFBF4 */
void Act_c::process_main() {
    u32 state = mProcess;
    if (state < PROCESS_COUNT)
        (this->*main_processes.entries[state])();
}

/* 023AFC4C */
BOOL Act_c::_execute() {
    Vds_Event_exe(this);
    Vds_PlayLoopJointAnimation(this);
    f32 first = mStrength[0];
    bool full = !(first < 1.0f) && !((f32)mStrength[1] < 1.0f);
    if (!full)
        first = (f32)(first * 0.25f);
    s32 frame = getFrameMax(M_brk_data0);
    mBrk0.setFrame((f32)(first * (f32)(s32)((u32)frame - 1)));
    J3DAnmTevRegKey* color = M_brk_data1;
    // HD: load the virtual target before loading the second light strength.
    auto* view = reinterpret_cast<VdsAnmView*>(color);
    u32 target = view->vtable->getFrameMax;
    f32 second = 0;
    if (!full)
        second = (f32)((f32)mStrength[1] * 0.25f);
    frame = gabi::call_ptr<s32>(target, color);
    if (full)
        second = mStrength[1];
    mBrk1.setFrame((f32)(second * (f32)(s32)((u32)frame - 1)));
    Vds_process_common(this);
    Vds_process_main(this);
    Vds_set_mtx(this);
    dBgW* bg = mpBgW;
    if (bg)
        bg->Move();
    return TRUE;
}

/* 023AFE48 */
static VdsColorF32* color_convert(VdsColorF32* out, VdsColorS16* input) {
    f32 rgba[4];
    for (u32 i = 0; i < 4; i++)
        rgba[i] = (f32)((f32)input->rgba[i] / 255.0f);
    for (u32 i = 0; i < 4; i++)
        out->rgba[i] = rgba[i];
    return out;
}

/* 023AFF0C */
BOOL Act_c::_draw() {
    dKy_tevstr_c* tev = &tevStr;
    settingTevStruct(dKy_getEnvlight(), 1, &current.pos, tev);
    mDoExt_McaMorf* morf = M_anm0;
    dScnKy_env_light_c* environment = dKy_getEnvlight();
    setLightTevColorType(environment, morf->getModel(), tev);
    morf = M_anm1;
    environment = dKy_getEnvlight();
    setLightTevColorType(environment, morf->getModel(), tev);
    morf = M_anm0;
    J3DModel* model = morf->getModel();
    mBrk0.entry(model->getModelData(), mBrk0.getFrame());
    morf = M_anm1;
    model = morf->getModel();
    mBrk1.entry(model->getModelData(), mBrk1.getFrame());
    M_anm0->updateDL();
    M_anm1->updateDL();
    for (u32 i = 0; i < 2; i++)
        if ((f32)mStrength[i] > 0.0f) {
            gabi::Local<cXyz> position;
            position->copy(mLights[i].mPos);
            gabi::Local<VdsColorF32> color;
            Vds_color(color.get(), reinterpret_cast<VdsColorS16*>(&mLights[i].mColorR));
            f32 strength = mStrength[i];
            Vds_drawGlow((void*)glowRenderer.pointer, position.get(), color.get(), 1, &glowTexture, 1, 0, 0,
                         (f32)(400.0f * strength), strength, 0.0f);
        }
    return TRUE;
}

/* 023B0080 */
BOOL Act_c::SetLoopJointAnimation(J3DAnmTransform* first, J3DAnmTransform* second, f32 speed, f32 morph) {
    // HD: this callsite supplies a fourth integer word before the floating-point arguments.
    Vds_setAnm(M_anm0, first, J3DFrameCtrl::EMode_LOOP, 0, morph, speed, 0.0f, -1.0f);
    Vds_setAnm(M_anm1, second, J3DFrameCtrl::EMode_LOOP, 0, morph, speed, 0.0f, -1.0f);
    mLoopAnimation = 1;
    return TRUE;
}

/* 023B016C */
BOOL Act_c::process_off_init() {
    J3DAnmTevRegKey* color = M_brk_data0;
    J3DModel* model = M_anm0->getModel();
    if (!mBrk0.init(model->getModelData(), color, true, 0, 1.0f, 0, -1, true, 0))
        return FALSE;
    mBrk0.setPlaySpeed(0.0f);
    color = M_brk_data1;
    model = M_anm1->getModel();
    if (!mBrk1.init(model->getModelData(), color, true, 0, 1.0f, 0, -1, true, 0))
        return FALSE;
    mBrk1.setPlaySpeed(0.0f);
    return TRUE;
}

/* 023B0280 */
void Act_c::process_off_main() {
    for (u32 i = 0; i < 2; i++) {
        auto* sw = static_cast<VdsSwitch*>(fopAcM_SearchByID(mSwitchActor[i]));
        f32 strength = sw ? (f32)sw->mStrength : 0.0f;
        mStrength[i] = (f32)(strength * strength);
    }
    s32 sw = Vds_getParam(this, 8, 0);
    if (fopAcM_isSwitch(this, sw))
        Vds_process_init(this, PROCESS_ON);
}

/* 023B037C */
BOOL Act_c::process_on_init() {
    J3DAnmTransform* first = M_bck_data0;
    J3DAnmTransform* second = M_bck_data1;
    if (!Vds_SetLoopJointAnimation(this, first, second, 1.0f, 0.0f))
        return FALSE;
    for (auto& strength : mStrength)
        strength = 1.0f;
    return TRUE;
}

/* 023B0420 */
static s32 Mthd_Create(Act_c* actor) {
    return Vds_create(actor);
}
/* 023B0424 */
static BOOL Mthd_Delete(Act_c* actor) {
    return Vds_delete(actor);
}
/* 023B0428 */
static BOOL Mthd_Execute(Act_c* actor) {
    return Vds_execute(actor);
}
/* 023B042C */
static BOOL Mthd_Draw(Act_c* actor) {
    return Vds_draw(actor);
}

/* 023B0430 */
static void __sinit_d_a_obj_Vds_cpp() {
    headerStorage.words[2] = 0;
    headerStorage.words[0] = 0;
    headerStorage.words[3] = 0;
    headerStorage.words[1] = 0;
    registerHeader(&registration0);
    minAngle.value = -3.1415927410125732f;
    maxAngle.value = 3.1415927410125732f;
    constructHeader0(&headerFlag0);
    registerHeader(&registration1);
    constructHeader1(&headerFlag1);
    registerHeader(&registration2);
}

/* 023B04C4 */
static void simple_destruct(void* object, s32 flags) {
    if (object && (flags & 1))
        operatorDelete(object);
}
/* 023B04D8 */
static LIGHT_INFLUENCE* light_construct(LIGHT_INFLUENCE* light) {
    if (!light)
        light = static_cast<LIGHT_INFLUENCE*>(operatorNew(sizeof(LIGHT_INFLUENCE)));
    if (light)
        light->mHD20 = 1.0f;
    return light;
}
/* 023B0518 */
static void light_destruct(void* object) {}
/* 023B051C */
static void Act_destruct(Act_c* actor, s32 flags) {
    if (actor) {
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operatorDelete(actor);
    }
}
/* 023B0570 */
void Act_c::process_on_main() {}
/* 023B0574 */
static BOOL Mthd_IsDelete(Act_c* actor) {
    return TRUE;
}

/* WWHD glue
str 10032A58 "Vds"
str 10032950 "d_a_obj_Vds.cpp"
str 10032988 "mdl_data0 != 0"
str 10032998 "M_bck_data0 != 0"
str 10032960 "M_anm0 != 0"
str 100329AC "mdl_data1 != 0"
str 100329BC "M_bck_data1 != 0"
str 1003296C "M_anm1 != 0"
str 100329D0 "M_brk_data0 != 0"
str 100329E4 "M_brk_data1 != 0"
str 10032978 "bgw_data != 0"
data event_name 10032A10
data glowTexture 104A01CC
data glowRenderer 101F8A20
data init_processes 100329F8
data main_processes 10032A1C
data headerStorage 1046C818
data minAngle 1046C80C
data maxAngle 1046C810
data headerFlag0 1046C814
data headerFlag1 1046C815
data registration0 101CD828
data registration1 101CD834
data registration2 101CD840
ext 023AEE1C Act_c* Vds_construct(Act_c*)
ext 023AEE98 void Vds_set_mtx(Act_c*)
ext 023AEFF4 BOOL Vds_create_heap(Act_c*)
ext 023AF404 BOOL Vds_process_init(Act_c*, s32)
ext 023AF4BC void Vds_set_first_process(Act_c*)
ext 023AF51C void Vds_Event_init(Act_c*)
ext 023AF734 void Vds_Event_exe(Act_c*)
ext 023AF834 BOOL Vds_PlayLoopJointAnimation(Act_c*)
ext 023AF674 void Vds_delete_point_light(Act_c*)
ext 023AF888 void* Vds_search_switchCB(Act_c*, fopAc_ac_c*)
ext 023AF934 void Vds_create_point_light(Act_c*, s32, cXyz*)
ext 023AF9A8 void Vds_execute_point_light(Act_c*)
ext 023AFA08 void Vds_process_common(Act_c*)
ext 023AFBF4 void Vds_process_main(Act_c*)
ext 023B0080 BOOL Vds_SetLoopJointAnimation(Act_c*, J3DAnmTransform*, J3DAnmTransform*, f32, f32)
ext 023AF530 s32 Vds_create(Act_c*)
ext 023AF6BC BOOL Vds_delete(Act_c*)
ext 023AFC4C BOOL Vds_execute(Act_c*)
ext 023AFF0C BOOL Vds_draw(Act_c*)
ext 02349DFC u32 Vds_getParam(Act_c*, s32, s32)
ext 028EFFD0 void constructArray(void*, u32, u32, LightConstructor)
ext 025E4A98 void Vds_setAnm(mDoExt_McaMorf*, J3DAnmTransform*, s32, s32, f32, f32, f32, f32)
ext 0273862C void Vds_drawGlow(void*, cXyz*, VdsColorF32*, s32, void*, s32, s32, s32, f32, f32, f32)
ext 028ED6F8 void constructHeader0(void*)
ext 028EAB2C void constructHeader1(void*)
ext 028F026C void registerHeader(void*)
ext 0273AD10 void* operatorNew(u32)
ext 0273AF40 void operatorDelete(void*)
ext 023AFE48 void Vds_color(VdsColorF32*, VdsColorS16*)
*/
