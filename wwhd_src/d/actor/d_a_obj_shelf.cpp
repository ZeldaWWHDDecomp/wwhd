/* Wooden shelf: GameCube source ported against WWHD disassembly.
 */
#include "d/actor/d_a_obj_shelf.h"
using daObjShelf::Act_c;
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
const gc_tu_data gc_tu = {.safeString = 0x1002F2B8};
static const char M_arcname[] = "Otana";
static Mtx34 M_tmp_mtx;
enum { SHELF_VTBL=0x1002F3C0, MoveBGProc_Trans=0x024EE76C };
using ActorPointer = gptr<fopAc_ac_c>;
using daObjShelf::ShelfModeEntry;
BOOL fopAcM_SearchByName(s32, ActorPointer*);
BOOL chk_quake(dDetect_c*, const cXyz*);

/* 0238E830 */
static u32 PrmAbstract(fopAc_ac_c* actor, s32 width, s32 shift) {
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 param = actor->mParameters;
    return (sh < 32 ? param >> sh : 0) & ((wd < 32 ? 1u << wd : 0) - 1);
}

/* 0238DDD8 */
cPhs_State Act_c::Mthd_Create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            dBgS_MoveBgActor::ct(this);
            condition = actor_condition;
            __vtbl = SHELF_VTBL;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        phase = MoveBGCreate(M_arcname, dRes_INDEX_OTANA_DZB_OTANA_e, MoveBGProc_Trans, 0xB00);
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            JUT_ASSERT_fail("d_a_obj_shelf.cpp", 0x15A, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase;
}
/* 0238DEAC */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname);
    return result;
}
/* 0238DEF8 */
BOOL Act_c::CreateHeap() {
    auto* data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_OTANA_BDL_OTANA_e, gc_tu.safeString);
    if (!data)
        JUT_ASSERT_fail("d_a_obj_shelf.cpp", 0x12C, "model_data != 0");
    J3DModel* model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}
/* 0238DF94 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}
/* 0238E068 */
void Act_c::init_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
/* 0238E088 */
void Act_c::mode_wait_init() {
    mMode = Mode_WAIT_e;
}
/* 0238E094 */
BOOL Act_c::Create() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    init_mtx();
    fopAcM_setCullSizeBox(this, -110.0f, -55.0f, -1.0f, 110.0f, 20.0f, 110.0f);
    mode_wait_init();
    return TRUE;
}
/* 0238E118 */
BOOL Act_c::Execute(gptr<Mtx34>* matrix) {
    // HD: GHS stores the named member-function descriptors in eight bytes.
    static ShelfModeEntry mode_proc[] = {
        {&Act_c::mode_wait}, {&Act_c::mode_vib},
        {&Act_c::mode_rot}, {&Act_c::mode_fell}
    };
    (this->*mode_proc[mMode].proc)();
    set_mtx();
    *matrix = &M_tmp_mtx;
    return TRUE;
}
/* 0238E1CC */
BOOL Act_c::Draw() {
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, TEV_TYPE_BG0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    J3DModel* model = mpModel;
    setLightTevColorType(env, model, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
/* 0238E264 */
void Act_c::hold_event() {
    ActorPointer npc;
    if (fopAcM_SearchByName(fpcNm_NPC_BA1_e, &npc)) {
        fopAc_ac_c* found = npc;
        if (found)
            reinterpret_cast<daObjShelf::GrannyEventView*>(found)->holdEventOn();
    }
}
/* 0238E2A8 */
void Act_c::mode_vib_init() {
    mTimer = 4;
    mVibY = 0;
    mVibX = 0;
    mVibZ = 0;
    mMode = Mode_VIB_e;
}
/* 0238E2CC */
void Act_c::mode_rot_init3() {
    mCurBounce = 6;
    mRotSpeed = -2500.0f;
    mTimer = 0;
    mTargetAngle = 0;
    m2e7 = 1;
    mMode = Mode_ROT_e;
}
/* 0238E300 */
void Act_c::mode_wait() {
    if (!chk_quake(&reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mDetect, &current.pos))
        return;
    if (PrmAbstract(this, PRM_GROUNDMA_W, PRM_GROUNDMA_S)) {
        if (!dComIfGs_isEventBit(GrannyRecoveredEvent)) {
            hold_event();
            mode_rot_init3();
            return;
        }
    }
    mode_vib_init();
}
/* 0238E394 */
void Act_c::mode_rot_init() {
    mCurBounce = 6;
    mRotSpeed = 0.0f;
    mTimer = 0;
    mTargetAngle = 0x4000;
    m2e7 = 0;
    mMode = Mode_ROT_e;
}
/* 0238E3C8 */
void Act_c::mode_vib() {
    s16 timer = (s16)((s16)mTimer - 1);
    mTimer = timer;
    if (timer <= 0) {
        current.pos.y = home.pos.y;
        shape_angle.x = home.angle.x;
        shape_angle.z = home.angle.z;
        mode_rot_init();
    } else {
        f32 homeY = home.pos.y;
        s16 vibY = (s16)((s16)mVibY + 30000);
        s16 vibX = (s16)((s16)mVibX + 30500);
        s16 vibZ = (s16)((s16)mVibZ + 29000);
        mVibY = vibY;
        current.pos.y = gabi::fmadds(cM_ssin(vibY), 4.0f, homeY);
        mVibX = vibX;
        mVibZ = vibZ;
        shape_angle.x = (s16)((u32)(s32)home.angle.x + (u32)gabi::ftoi(cM_ssin(vibX) * 200.0f));
        shape_angle.z = (s16)((u32)(s32)home.angle.z + (u32)gabi::ftoi(cM_ssin(vibZ) * 200.0f));
    }
}
/* 0238E4DC */
void Act_c::mode_rot_init2() {
    mCurBounce = 6;
    mRotSpeed = -3500.0f;
    mTimer = 0;
    mTargetAngle = 0x4000;
    m2e7 = 0;
    mMode = Mode_ROT_e;
}
/* 0238E510 */
void Act_c::mode_fell_init() {
    mMode = Mode_FELL_e;
}
/* 0238E51C */
void Act_c::mode_rot() {
    s16 timer = mTimer;
    if (timer > 0) {
        mTimer = (s16)(timer - 1);
        return;
    }
    f32 speed = (f32)mRotSpeed + 600.0f;
    speed = gabi::fnmsubs(speed, 0.02f, speed);
    s16 angle = (s16)((u32)(s32)shape_angle.x + (u32)gabi::ftoi(speed));
    s16 target = mTargetAngle;
    mRotSpeed = speed;
    shape_angle.x = angle;
    if (angle > target) {
        s8 bounce = (s8)((u8)mCurBounce - 1);
        mCurBounce = bounce;
        target = mTargetAngle;
        if (bounce <= 0) {
            shape_angle.x = target;
            if (m2e7)
                mode_wait_init();
            else
                mode_fell_init();
        } else {
            s16 overshoot = (s16)((s16)shape_angle.x - target);
            shape_angle.x = (s16)(target - gabi::ftoi((f32)overshoot * 0.5f));
            mRotSpeed = (f32)mRotSpeed * -0.5f;
            if (bounce == 5) {
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(JA_SE_OBJ_RACK_BREAK, &eyePos, 0, reverb);
            }
        }
    }
}
/* 0238E6AC */
void Act_c::mode_fell() {
    if (chk_quake(&reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mDetect, &current.pos))
        mode_rot_init2();
}

/* 0238E6F4 */
static cPhs_State Mthd_Create(Act_c* self) {
    return self->Mthd_Create();
}
/* 0238E6F8 */
static BOOL Mthd_Delete(Act_c* self) {
    return self->Mthd_Delete();
}
/* 0238E6FC */
static BOOL Mthd_Execute(Act_c* self) {
    return self->MoveBGExecute();
}
/* 0238E700 */
static BOOL Mthd_Draw(Act_c* self) {
    return self->Draw_v();
}
/* 0238E710 */
static BOOL Mthd_IsDelete(Act_c* self) {
    return self->IsDelete_v();
}
// HD: compiler-generated and template code of this translation unit.
/* 0238E720 */
static void initStatics() {
    sinit_header_statics(0x1046BE24, 0x101CCABC);
}
/* 0238E7B4 */
static void trivialDestructor(void* self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}
/* 0238E7D4 */
static BOOL Delete(Act_c* self) {
    return TRUE;
}
/* 0238E7DC */
static void actorDestructor(Act_c* self, s32 flags) {
    if (self) {
        __dt(static_cast<fopAc_ac_c*>(self));
        if (flags & 1)
            operator_delete(self);
    }
}

/* Per-TU inline copies: matcher incorrectly labels IsDelete as d_a_fan. */
/* 0238E7C8 */
static BOOL IsDelete(Act_c* self) {
    return TRUE;
}
/* 0238E7D0 */
static void emptyVirtual(Act_c* self) {
}

/* WWHD glue
str 1002F3B8
str 1002F2D4 in 0238DDD8
str 1002F2E8
str 1002F33C in 0238DEF8
str 1002F32C
ext 025D5578 BOOL fopAcM_SearchByName(s32, ActorPointer*)
ext 02529CE8 BOOL chk_quake(dDetect_c*, const cXyz*)
data M_tmp_mtx 1046BE40
data mode_proc 1002F364
*/
