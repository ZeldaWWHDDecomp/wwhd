/* WWHD Nintendo Gallery hatch. */
#include "d/actor/d_a_obj_ohatch.h"
const gc_tu_data gc_tu = {.safeString = 0x1002DF2C};
enum {
    dRes_INDEX_OHATCH_BDL_OHATCH_e = 4,
    dRes_INDEX_OHATCH_DZB_OH_CLS_e = 7,
    dRes_INDEX_OHATCH_DZB_OH_OPN_e = 8
};
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;
static OhatchHeaderZeros l_headerZeros;
static void setAction(daObjOhatch_c *actor, HatchAction action) {
    actor->mAction.delta = 0;
    actor->mAction.index = -1;
    actor->mAction.target = (u32)action;
}
/* 0237FCBC */
static s32 parameter(daObjOhatch_c *actor, s32 width, s32 shift) {
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 value = actor->mParameters;
    return ((shift & 32) ? 0 : (value >> (shift & 31))) & (mask - 1);
}
/* 0237F07C */
bool daObjOhatch_c::create_heap() {
    auto *data = (J3DModelData *)dComIfG_getObjectRes("Ohatch", dRes_INDEX_OHATCH_BDL_OHATCH_e);
    if (!data) {
        JUT_ASSERT_fail(STR(0x1002DF58), 0x137, STR(0x1002DF54));
        return false;
    }
    mModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    void *closed = dComIfG_getObjectRes("Ohatch", dRes_INDEX_OHATCH_DZB_OH_CLS_e);
    mClosedBg = dBgW_NewSet((cBgD_t *)closed, dBgW::MOVE_BG_e, &mBgMatrix);
    void *opened = dComIfG_getObjectRes("Ohatch", dRes_INDEX_OHATCH_DZB_OH_OPN_e);
    dBgW *openBg = dBgW_NewSet((cBgD_t *)opened, dBgW::MOVE_BG_e, &mBgMatrix);
    J3DModel *model = mModel;
    mOpenBg = openBg;
    return model != nullptr && (dBgW *)mClosedBg != nullptr && openBg != nullptr;
}
/* 0237F188 */
static BOOL solidHeapCB(fopAc_ac_c *actor) {
    return ((daObjOhatch_c *)actor)->create_heap();
}
/* 0237F18C */
void daObjOhatch_c::init_mtx() {
    J3DModel_setBaseScale(mModel, &scale);
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x, (s16)shape_angle.y,
                   (s16)shape_angle.z);
    mDoMtx_stack_push();
    mDoMtx_stack_c::transM(0.0f, 25.0f, -190.0f);
    mDoMtx_stack_c::XrotM((s16)mHingeAngle);
    mDoMtx_stack_c::transM(0.0f, -25.0f, 190.0f);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
    mDoMtx_stack_pop();
    mDoMtx_stack_c::scaleM((f32)scale.x, (f32)scale.y, (f32)scale.z);
    MTXCopy(mDoMtx_stack_c::get(), &mBgMatrix);
}
/* 0237F2EC */
s32 daObjOhatch_c::_create() {
    fopAcM_ct(this, daObjOhatch_c);
    s32 phase = dComIfG_resLoad(&mPhase, "Ohatch");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0xB90))
        return cPhs_ERROR_e;
    J3DModel *model = mModel;
    cullMtx = model ? gabi::ea(J3DModel_getBaseTRMtx(model)) : 0;
    s32 sw = parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    mSwitch = sw;
    bool opened = sw != 255 && fopAcM_isSwitch(this, sw) == 1;
    if (opened) {
        setAction(this, HatchAction::WaitOpen);
        mHingeAngle = -0x4000;
    } else
        setAction(this, HatchAction::WaitClosed);
    auto *bgsp = dComIfG_Bgsp();
    dBgW *bg = opened ? (dBgW *)mOpenBg : (dBgW *)mClosedBg;
    if (bgsp->Regist(bg, this))
        phase = cPhs_ERROR_e;
    init_mtx();
    return phase;
}
/* 0237F47C */
bool daObjOhatch_c::_delete() {
    dComIfG_resDelete(&mPhase, "Ohatch");
    if ((JKRSolidHeap *)heap) {
        dBgW *bg = mClosedBg;
        if (bg) {
            if (bg->ChkUsed()) {
                auto *bgsp = dComIfG_Bgsp();
                bg = mClosedBg;
                bgsp->Release(bg);
            }
            mClosedBg = nullptr;
        }
        bg = mOpenBg;
        if (bg) {
            if (bg->ChkUsed()) {
                auto *bgsp = dComIfG_Bgsp();
                bg = mOpenBg;
                bgsp->Release(bg);
            }
            mOpenBg = nullptr;
        }
    }
    return true;
}
/* 0237F52C */
void daObjOhatch_c::set_mtx() {
    f32 amplitude = (s16)mVibrationAmplitude;
    u16 phase = mVibrationPhase;
    f32 wave = cM_ssin(phase);
    f32 height = mTremorHeight, z = current.pos.z, y = current.pos.y;
    s16 angle = (s16)((s16)mHingeAngle + (s16)gabi::ftoi(amplitude * wave));
    y = y + height;
    f32 x = current.pos.x;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x, (s16)shape_angle.y,
                   (s16)shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, 25.0f, -190.0f);
    mDoMtx_stack_c::XrotM(angle);
    mDoMtx_stack_c::transM(0.0f, -25.0f, 190.0f);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
}
/* 0237F6BC */
bool daObjOhatch_c::_execute() {
    dBgW *bg = mClosedBg;
    if (bg && bg->ChkUsed())
        bg->Move();
    bg = mOpenBg;
    if (bg && bg->ChkUsed())
        bg->Move();
    set_mtx();
    // GHS pointer-to-member descriptor supports both direct and virtual actions.
    ptmf_call(gabi::ea(&mAction), this);
    return true;
}
/* 0237F770 */
bool daObjOhatch_c::_draw() {
    auto *env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    J3DModel *model = mModel;
    setLightTevColorType(env, model, &tevStr);
    model = mModel;
    mDoExt_modelUpdateDL(model, 0);
    return true;
}
/* 0237F7D8 */
void daObjOhatch_c::close_wait_act_proc() {
    s32 sw = mSwitch;
    if (sw == 255 || fopAcM_isSwitch(this, sw) != 1)
        return;
    // HD: existence uses the manager from the first play accessor, before the event-index lookup.
    void *existenceManager = ohatchEventManager(dComIfGp_get());
    s32 event = ohatchGetEventIdx(dComIfGp_getPEvtManager(), STR(0x1002DF9C), 255);
    if (!ohatchGetEventData(existenceManager, event))
        return;
    s32 staff = dComIfGp_evmng_getMyStaffId(STR(0x1002DF8C), nullptr, 0);
    if (staff == -1)
        return;
    u32 cut = ohatchGetCutName(dComIfGp_getPEvtManager(), staff);
    if (!ohatchCutIsOpen(cut, STR(0x1002DF94)))
        return;
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(0x6A01, &current.pos, 0, reverb);
    dBgW *bg = mClosedBg;
    if (bg && bg->ChkUsed()) {
        auto *bgsp = dComIfG_Bgsp();
        bg = mClosedBg;
        bgsp->Release(bg);
    }
    bg = mOpenBg;
    if (bg && !bg->ChkUsed()) {
        auto *bgsp = dComIfG_Bgsp();
        bg = mOpenBg;
        bgsp->Regist(bg, this);
    }
    auto *vibration = dComIfGp_getVibration();
    gabi::Local<cXyz> up;
    up->x = 0.0f;
    up->z = 0.0f;
    up->y = 1.0f;
    vibration->StartShock(3, 1, up.get());
    setAction(this, HatchAction::Tremor);
}
/* 0237F990 */
void daObjOhatch_c::tremor_act_proc() {
    s32 phase = (s32)((u32)(s32)mTremorPhase + 0x2000);
    mTremorPhase = phase;
    if (phase > 0x20000) {
        mTremorHeight = 0.0f;
        setAction(this, HatchAction::Open);
    } else {
        f32 wave = cM_ssin((u16)phase);
        // HD: negate unordered NaNs as well as negative sine values.
        if (!(wave >= 0.0f))
            wave = -wave;
        mTremorHeight = wave * 6.0f;
    }
}
/* 0237FA0C */
void daObjOhatch_c::open_act_proc() {
    s16 angle = mHingeAngle;
    f32 fraction = (f32)angle * -0.00006103515625f;
    u16 phase = (u16)gabi::ftoi(fraction * 32768.0f);
    f32 wave = cM_ssin(phase);
    f32 speed = (wave + 1.0f) * 0.5f;
    s16 next = (s16)(angle + (s16)gabi::ftoi(-4096.0f * speed));
    if (next > -0x4000) {
        mHingeAngle = next;
        return;
    }
    mHingeAngle = -0x4000;
    ohatchSeStart(0x806);
    mVibrationAmplitude = 0x400;
    auto *vibration = dComIfGp_getVibration();
    gabi::Local<cXyz> up;
    up->y = 1.0f;
    up->x = 0.0f;
    up->z = 0.0f;
    vibration->StartShock(6, 1, up.get());
    setAction(this, HatchAction::Vibrate);
}
/* 0237FB74 */
void daObjOhatch_c::vibrate_act_proc() {
    s16 amplitude = (s16)((s16)mVibrationAmplitude - 64);
    mVibrationAmplitude = amplitude;
    if (amplitude < 0)
        setAction(this, HatchAction::WaitOpen);
    else
        mVibrationPhase = (s16)((s16)mVibrationPhase + 0x2000);
}
/* 0237FC60 */
void daObjOhatch_c::open_wait_act_proc() {
}
/* 0237F478 */
static s32 create(daObjOhatch_c *actor) {
    return actor->_create();
}
/* 0237F528 */
static bool remove(daObjOhatch_c *actor) {
    return actor->_delete();
}
/* 0237F76C */
static bool execute(daObjOhatch_c *actor) {
    return actor->_execute();
}
/* 0237F7CC */
static bool draw(daObjOhatch_c *actor) {
    return actor->_draw();
}
/* 0237F7D0 */
static BOOL is_delete(daObjOhatch_c *actor) {
    return TRUE;
}
/* 0237F028 */
static void base_destructor(daObjOhatch_c *actor, s32 flags) {
    if (actor) {
        ohatchActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0237FC64 */
static void actor_destructor(daObjOhatch_c *actor, s32 flags) {
    if (actor) {
        ohatchActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0237FC4C */
static void empty_destructor(u8 *object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 0237FBB8 */
static void static_init() {

    // HD: preserve the translation unit's header-static zeroing order.
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(0x101CC09C);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    ohatchInitA(&l_initA);
    __register_global_object(0x101CC0A8);
    ohatchInitB(&l_initB);
    __register_global_object(0x101CC0B4);
}
/* 0237FCB8 */
static void empty_inline(daObjOhatch_c *actor) {
}

daObjOhatch_c::daObjOhatch_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
str 1002DF24 "Ohatch"
data l_minAngle 1046BAC0
data l_maxAngle 1046BAC4
data l_initA 1046BAC8
data l_initB 1046BAC9
data l_headerZeros 1046BACC
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
ext 02543F10 s32 ohatchGetEventIdx(void*, const char*, s32)
ext 02544044 u32 ohatchGetEventData(void*, s32)
ext 02544830 u32 ohatchGetCutName(void*, s32)
ext 025E1988 void ohatchSeStart(u32)
ext 025D50BC void ohatchActorDt(void*, s32)
ext 028ED6F8 void ohatchInitA(void*)
ext 028EAB2C void ohatchInitB(void*)
*/
