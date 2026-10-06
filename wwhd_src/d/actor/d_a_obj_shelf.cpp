/* Wooden shelf: GameCube source ported against WWHD disassembly.
 */
#include "d/actor/d_a_obj_shelf.h"
using daObjShelf::Act_c;
#define M_arcname STR(0x1002F3B8)
#define M_tmp_mtx gabi::at<Mtx34>(0x1046BE40)

static u32 PrmAbstract(fopAc_ac_c* actor, s32 width, s32 shift) {
    WWHD_FUNC(0x0238E830, u32, actor, width, shift);
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 param = actor->mParameters;
    return (sh < 32 ? param >> sh : 0) & ((wd < 32 ? 1u << wd : 0) - 1);
}
VERIFY(0x0238E830, PrmAbstract);

cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0238DDD8, cPhs_State, this);
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            dBgS_MoveBgActor::ct(this);
            condition = actor_condition;
            __vtbl = 0x1002F3C0;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        phase = MoveBGCreate(M_arcname, 7, 0x024EE76C, 0xB00);
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            JUT_ASSERT_fail(STR(0x1002F2D4), 0x15A, STR(0x1002F2E8));
    }
    return phase;
}
VERIFY(0x0238DDD8, &Act_c::Mthd_Create);
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0238DEAC, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname);
    return result;
}
VERIFY(0x0238DEAC, &Act_c::Mthd_Delete);
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0238DEF8, BOOL, this);
    auto* data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, 4, 0x1002F2B8);
    if (!data)
        JUT_ASSERT_fail(STR(0x1002F33C), 0x12C, STR(0x1002F32C));
    J3DModel* model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}
VERIFY(0x0238DEF8, &Act_c::CreateHeap);
void Act_c::set_mtx() {
    WWHD_FUNC(0x0238DF94, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x0238DF94, &Act_c::set_mtx);
void Act_c::init_mtx() {
    WWHD_FUNC(0x0238E068, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x0238E068, &Act_c::init_mtx);
void Act_c::mode_wait_init() {
    WWHD_FUNC(0x0238E088, void, this);
    mMode = 0;
}
VERIFY(0x0238E088, &Act_c::mode_wait_init);
BOOL Act_c::Create() {
    WWHD_FUNC(0x0238E094, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    init_mtx();
    fopAcM_setCullSizeBox(this, -110.0f, -55.0f, -1.0f, 110.0f, 20.0f, 110.0f);
    mode_wait_init();
    return TRUE;
}
VERIFY(0x0238E094, &Act_c::Create);
BOOL Act_c::Execute(Mtx34** matrix) {
    WWHD_FUNC(0x0238E118, BOOL, this, matrix);
    // HD member-function table: this adjustment, virtual index, direct target.
    u32 entry = 0x1002F364 + (u32)mMode * 8;
    s16 index = gabi::load<s16>(entry + 2);
    s16 adjust = gabi::load<s16>(entry);
    u32 object = gabi::ea(this) + adjust;
    u32 target;
    if (index < 0) {
        target = gabi::load<u32>(entry + 4);
    } else {
        s16 vtableOffset = gabi::load<s16>(entry + 6);
        u32 vtable = gabi::load<u32>(object + vtableOffset);
        target = gabi::load<u32>(vtable + index * 8 + 4);
    }
    gabi::call_ptr<void>(target, gabi::at<Act_c>(object));
    set_mtx();
    gabi::store<u32>(gabi::ea(matrix), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x0238E118, &Act_c::Execute);
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0238E1CC, BOOL, this);
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
VERIFY(0x0238E1CC, &Act_c::Draw);
void Act_c::hold_event() {
    WWHD_FUNC(0x0238E264, void, this);
    gabi::Local<gptr<fopAc_ac_c>> npc;
    if (gabi::call<BOOL>(0x025D5578, 0x14F, npc.get())) {
        fopAc_ac_c* found = *npc;
        if (found)
            gabi::store<u8>(gabi::ea(found) + 0x969, 1);
    }
}
VERIFY(0x0238E264, &Act_c::hold_event);
void Act_c::mode_vib_init() {
    WWHD_FUNC(0x0238E2A8, void, this);
    mTimer = 4;
    mVibY = 0;
    mVibX = 0;
    mVibZ = 0;
    mMode = 1;
}
VERIFY(0x0238E2A8, &Act_c::mode_vib_init);
void Act_c::mode_rot_init3() {
    WWHD_FUNC(0x0238E2CC, void, this);
    mCurBounce = 6;
    mRotSpeed = -2500.0f;
    mTimer = 0;
    mTargetAngle = 0;
    mReturnToWait = 1;
    mMode = 2;
}
VERIFY(0x0238E2CC, &Act_c::mode_rot_init3);
void Act_c::mode_wait() {
    WWHD_FUNC(0x0238E300, void, this);
    u32 play = gabi::call<u32>(0x025200D4);
    if (!gabi::call<BOOL>(0x02529CE8, gabi::at<u8>(play + 0x5A20), &current.pos))
        return;
    if (PrmAbstract(this, 1, 0)) {
        u32 save = gabi::load<u32>(0x101F84DC);
        if (!gabi::call<BOOL>(0x025B8B94, gabi::at<u8>(save + 0x644), 1)) {
            hold_event();
            mode_rot_init3();
            return;
        }
    }
    mode_vib_init();
}
VERIFY(0x0238E300, &Act_c::mode_wait);
void Act_c::mode_rot_init() {
    WWHD_FUNC(0x0238E394, void, this);
    mCurBounce = 6;
    mRotSpeed = 0.0f;
    mTimer = 0;
    mTargetAngle = 0x4000;
    mReturnToWait = 0;
    mMode = 2;
}
VERIFY(0x0238E394, &Act_c::mode_rot_init);
void Act_c::mode_vib() {
    WWHD_FUNC(0x0238E3C8, void, this);
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
VERIFY(0x0238E3C8, &Act_c::mode_vib);
void Act_c::mode_rot_init2() {
    WWHD_FUNC(0x0238E4DC, void, this);
    mCurBounce = 6;
    mRotSpeed = -3500.0f;
    mTimer = 0;
    mTargetAngle = 0x4000;
    mReturnToWait = 0;
    mMode = 2;
}
VERIFY(0x0238E4DC, &Act_c::mode_rot_init2);
void Act_c::mode_fell_init() {
    WWHD_FUNC(0x0238E510, void, this);
    mMode = 3;
}
VERIFY(0x0238E510, &Act_c::mode_fell_init);
void Act_c::mode_rot() {
    WWHD_FUNC(0x0238E51C, void, this);
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
            if (mReturnToWait)
                mode_wait_init();
            else
                mode_fell_init();
        } else {
            s16 overshoot = (s16)((s16)shape_angle.x - target);
            shape_angle.x = (s16)(target - gabi::ftoi((f32)overshoot * 0.5f));
            mRotSpeed = (f32)mRotSpeed * -0.5f;
            if (bounce == 5) {
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                gabi::call(0x025E1A40, 0x696D, &eyePos, 0, reverb);
            }
        }
    }
}
VERIFY(0x0238E51C, &Act_c::mode_rot);
void Act_c::mode_fell() {
    WWHD_FUNC(0x0238E6AC, void, this);
    u32 play = gabi::call<u32>(0x025200D4);
    if (gabi::call<BOOL>(0x02529CE8, gabi::at<u8>(play + 0x5A20), &current.pos))
        mode_rot_init2();
}
VERIFY(0x0238E6AC, &Act_c::mode_fell);

static cPhs_State Mthd_Create(Act_c* self) {
    WWHD_FUNC(0x0238E6F4, cPhs_State, self);
    return self->Mthd_Create();
}
VERIFY(0x0238E6F4, Mthd_Create);
static BOOL Mthd_Delete(Act_c* self) {
    WWHD_FUNC(0x0238E6F8, BOOL, self);
    return self->Mthd_Delete();
}
VERIFY(0x0238E6F8, Mthd_Delete);
static BOOL Mthd_Execute(Act_c* self) {
    WWHD_FUNC(0x0238E6FC, BOOL, self);
    return self->MoveBGExecute();
}
VERIFY(0x0238E6FC, Mthd_Execute);
static BOOL Mthd_Draw(Act_c* self) {
    WWHD_FUNC(0x0238E700, BOOL, self);
    return self->Draw_v();
}
VERIFY(0x0238E700, Mthd_Draw);
static BOOL Mthd_IsDelete(Act_c* self) {
    WWHD_FUNC(0x0238E710, BOOL, self);
    return self->IsDelete_v();
}
VERIFY(0x0238E710, Mthd_IsDelete);
static void initStatics() {
    WWHD_FUNC(0x0238E720, void);
    sinit_header_statics(0x1046BE24, 0x101CCABC);
}
VERIFY(0x0238E720, initStatics);
static void trivialDestructor(void* self, s32 flags) {
    WWHD_FUNC(0x0238E7B4, void, self, flags);
    if (self && (flags & 1))
        operator_delete(self);
}
VERIFY(0x0238E7B4, trivialDestructor);
static BOOL Delete(Act_c* self) {
    WWHD_FUNC(0x0238E7D4, BOOL, self);
    return TRUE;
}
VERIFY(0x0238E7D4, Delete);
static void actorDestructor(Act_c* self, s32 flags) {
    WWHD_FUNC(0x0238E7DC, void, self, flags);
    if (self) {
        gabi::call(0x025D50BC, self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}
VERIFY(0x0238E7DC, actorDestructor);

/* Per-TU inline copies: matcher incorrectly labels IsDelete as d_a_fan. */
static BOOL IsDelete(Act_c* self) {
    WWHD_FUNC(0x0238E7C8, BOOL, self);
    return TRUE;
}
VERIFY(0x0238E7C8, IsDelete);
static void emptyVirtual(Act_c* self) {
    WWHD_FUNC(0x0238E7D0, void, self);
}
VERIFY(0x0238E7D0, emptyVirtual);
