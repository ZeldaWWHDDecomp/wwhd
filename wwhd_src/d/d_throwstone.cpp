/* Cutscene stone actor, verified WWHD behavior. */
#include "d/d_throwstone.h"

const gc_tu_data gc_tu = { .safeString = 0x10056034 };
static const char M_arcname[] = "Aisi";
static u32 actorVtable;

/* 025C527C */
BOOL daThrowstone_c::CreateHeap() {
    J3DModelData* pModelData = static_cast<J3DModelData*>(
        dComIfG_getObjectRes(M_arcname, dRes_INDEX_AISI_BDL_AISI_e));
    if (pModelData == NULL) return FALSE;
    mpModel = mDoExt_J3DModel__create(pModelData, 0x80000, 0x11000002);
    return mpModel.get() != NULL;
}

/* 025C5310 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_actor) {
    return static_cast<daThrowstone_c*>(i_actor)->CreateHeap();
}

static void throwstone_SetScale(daThrowstone_c* i_this) {
    f32 x = i_this->scale.x;
    f32 z = i_this->scale.z;
    J3DModel* model = i_this->mpModel;
    f32 y = i_this->scale.y;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
}

static void throwstone_CopyMatrix(daThrowstone_c* i_this) {
    f32 values[12];
    Mtx34* matrix = mDoMtx_stack_c::get();
    for (u32 i = 0; i < 12; ++i) values[i] = matrix->m[i / 4][i % 4];
    J3DModel* model = i_this->mpModel;
    for (u32 i = 0; i < 12; ++i) model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    throwstoneMatrixCopy(matrix, &i_this->mMtx);
}

/* 025C5314 */
cPhs_State daThrowstone_c::_create() {
    cPhs_State result = dComIfG_resLoad(&mPhs, M_arcname);
    if (result != cPhs_COMPLEATE_e) return result;
    // HD: placement construction tests null, then reloads condition before setting INIT.
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            throwstoneActorConstruct(this);
            flags = actor_condition;
            __vtbl = gabi::ea(&actorVtable);
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), 0x4C0)) return cPhs_ERROR_e;
    // HD: scale belongs to the model and all three shape angles form the matrix.
    throwstone_SetScale(this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    throwstone_CopyMatrix(this);
    return result;
}

/* 025C5498 */
BOOL daThrowstone_c::_delete() {
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return TRUE;
}

/* 025C54C8 */
BOOL daThrowstone_c::_execute() {
    // HD: update the model only when demo-data activation succeeds; return TRUE.
    BOOL active = dDemo_setDemoData(this, ThrowstoneDemo_Transform | ThrowstoneDemo_Rotate |
        ThrowstoneDemo_Animation | ThrowstoneDemo_AnimationFrame, 0, 0, 0, 0, 0, 0) != 0;
    mDemoActive = active;
    if (active) {
        throwstone_SetScale(this);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::ZXYrotM(shape_angle);
        throwstone_CopyMatrix(this);
    }
    return TRUE;
}

/* 025C55E8 */
BOOL daThrowstone_c::_draw() {
    // HD: demo activation additionally gates the event-bit visibility test.
    if (mDemoActive && dComIfGs_isEventBit(ThrowstoneEvent_Visible)) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        settingTevStruct(env, 0, &current.pos, &tevStr);
        env = dKy_getEnvlight();
        setLightTevColorType(env, mpModel, &tevStr);
        mDoExt_modelUpdateDL(mpModel, 0);
    }
    return TRUE;
}

/* 025C566C */
static void __sinit_d_throwstone_cpp() {
    // HD: initialize common header statics emitted in this unit.
    sinit_header_statics_rodata(0x104871CC, 0x101EE6E8, 0x104871D8, 0x10056064);
}

/* 025C5714 */
static BOOL daThrowstoneIsDelete(void* i_this) {
    return TRUE;
}

/* 025C571C */
// HD: explicit GHS deleting-destructor entry preserves its flags argument.
static void daThrowstone_c__dt(daThrowstone_c* i_this, u32 flags) {
    if (i_this != NULL) {
        throwstoneActorDestruct(i_this, 0);
        if (flags & 1) throwstoneDeleteStorage(i_this);
    }
}

/* 025C5770 */
static void throwstoneEmptyInit() {}

/* 025C5700 */
// HD: this unit's trivial sead::SafeString deleting destructor.
static void throwstoneSafeStringDeletingDtor(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) throwstoneDeleteStorage(object);
}

/* WWHD glue
data actorVtable 1005604C
str 10056070 "Aisi" renamed
ext 025D4ED0 void throwstoneActorConstruct(daThrowstone_c*)
ext 025D50BC void throwstoneActorDestruct(daThrowstone_c*, u32)
ext 0273AF40 void throwstoneDeleteStorage(void*)
ext 02527028 u32 dDemo_setDemoData(fopAc_ac_c*, u32, u32, u32, u32, u32, u32, u32)
ext 028E90D4 void throwstoneMatrixCopy(Mtx34*, Mtx34*)
*/
