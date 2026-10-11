/** Barrel actor: tww state machine on the verified WWHD layout.
 * HD: SafeString calls, full-register returns, float copies and guest Local order
 * retain the original behavior. The draw path has no GameCube cutoff or shadow. */
#include "d/actor/d_a_obj_barrel.h"
using daObjBarrel::Act_c;

const gc_tu_data gc_tu = {
    .safeString = 0x10025558,
    .objAcch = {0x10025580, 0x100255A0, 0x10025590},
    .objAcchDt = {0x10025590, 0x100255A0},
    .headerStatics = {0x104691E0, 0x101C7E8C, 0x104691EC},
};
static const char M_arcname[] = "Ktaru_01";
static BarrelResPointer l_resControl;
static BarrelStringVtable l_stringVtable;
static BarrelWord l_scaleInitialized;
static cXyz l_emitterScale;
static BarrelModeTable mode_proc;
static dCcD_SrcCyl M_cyl_src;

static void collision_mode(Act_c* a, bool carry = false) {
    a->mCyl.mObjAt.mSPrm &= ~1u;
    a->mCyl.mObjTg.mSPrm |= 1u;
    if (carry)
        a->mCyl.mObjCo.mSPrm &= ~1u;
    else
        a->mCyl.mObjCo.mSPrm |= 1u;
}
static void clear_carry_attention(Act_c* a) {
    a->attention_info.flags &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
}
/* 0231C2AC */
bool daObjBarrel::Act_c::create_heap() {
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = gabi::guest_addr(M_arcname);
    (*name)[1] = gc_tu.safeString;
    auto* model = Barrel_getRes(l_resControl.get(), name.get(), dRes_INDEX_KTARU_01_BDL_KTARU_01_e);
    if (!model)
        Barrel_assert("d_a_obj_barrel.cpp", 0x17A, "mdl_data != 0");
    this->mpModel = mDoExt_J3DModel__create(model, 0x80000, 0x11000022);
    return this->mpModel != nullptr;
}
/* 0231C35C */
static bool solidHeapCB(Act_c* a) { return a->create_heap(); }
/* 0231C360 */
void daObjBarrel::Act_c::cull_set_draw() { Barrel_setCullSphere(this, 0.f, 50.f, 0.f, 75.f); }
/* 0231CA9C */
void daObjBarrel::Act_c::cull_set_move() { Barrel_setCullSphere(this, 0.f, 50.f, 0.f, 300.f); }
/* 0231C380 */
void daObjBarrel::Act_c::set_mtx() {
    auto* matrix = mDoMtx_stack_c::get();
    Barrel_transS(matrix, (f32)this->current.pos.x, (f32)this->current.pos.y,
                  (f32)this->current.pos.z);
    u32 mode = this->mMode;
    if (mode <= MODE_WALK) {
        bool vibrate = mode >= MODE_VIB0 && mode <= MODE_VIB2;
        f32 height = vibrate ? 50.f : gabi::fmadds(5.f, cM_scos(this->shape_angle.z), 45.f);
        Barrel_transM(0.f, height, 0.f);
        Barrel_ZXYrotM(matrix, 0, (s16)this->shape_angle.y, (s16)this->shape_angle.z);
        Barrel_YrotM(matrix, (s16)this->m612);
        Barrel_XrotM(matrix, (s16)this->m610);
        Barrel_YrotM(matrix, (s16) - (s16)this->m612);
        if (!vibrate)
            Barrel_YrotM(matrix, (s16)this->m630);
        Barrel_transM(0.f, -50.f, 0.f);
    }
    J3DModel* model = this->mpModel;
    // HD: float loads/stores preserve the original matrix copy's NaN behavior.
    for (u32 i = 0; i < 12; i++)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix->m[i / 4][i % 4];
}
/* 0231C5D4 */
void daObjBarrel::Act_c::init_mtx() {
    J3DModel* model = this->mpModel;
    model->mBaseScale.x = this->scale.x;
    model->mBaseScale.y = this->scale.y;
    model->mBaseScale.z = this->scale.z;
    this->set_mtx();
}
/* 0231C5F4 */
void daObjBarrel::Act_c::mode_wait_init() {
    collision_mode(this);
    this->speedF = 0.f;
    this->gravity = -6.f;
    this->mStts.Init(200, 255, this);
    this->mMode = MODE_WAIT;
}
/* 0231CA6C */
bool daObjBarrel::Act_c::_delete() {
    dComIfG_resDelete(&this->mPhs, M_arcname);
    return true;
}
/* 0231CABC */
u32 daObjBarrel::Act_c::get_se_map_hit() { return 11; }
/* 0231CBE0 */
void daObjBarrel::Act_c::set_senv(s32 p1, s32 p2) {
    gabi::Local<cXyz> position;
    *position = this->current.pos;
    Barrel_Sound_set(position.get(), p1, (u32)this->mBsPcId, p2);
}
/* 0231CC9C */
void daObjBarrel::Act_c::mode_jump_init() {
    collision_mode(this);
    clear_carry_attention(this);
    this->shape_angle.y = 0;
    this->shape_angle.z = 0;
    this->m610 = 0;
    this->m612 = 0;
    this->m630 = 0;
    this->gravity = -6.f;
    this->speed.y = 30.f;
    this->mMode = MODE_JUMP;
}
/* 0231CD08 */
void daObjBarrel::Act_c::mode_walk_init() {
    collision_mode(this);
    clear_carry_attention(this);
    this->gravity = -6.f;
    this->mMode = MODE_WALK;
}
/* 0231D0F0 */
bool daObjBarrel::Act_c::chk_sink_water() {
    return this->mAcch.ChkWaterHit() &&
           (f32)this->mAcch.m_wtr.m_height > (f32)this->current.pos.y + 50.f;
}
/* 0231D25C */
bool daObjBarrel::Act_c::chk_sinkdown_water() {
    return this->mAcch.ChkWaterHit() &&
           (f32)this->mAcch.m_wtr.m_height > ((f32)this->current.pos.y + 100.f) + 50.f;
}
/* 0231D20C */
void daObjBarrel::Act_c::eff_hit_water_splash() {
    gabi::Local<cXyz> position;
    position->x = this->current.pos.x;
    position->y = (f32)this->mAcch.m_wtr.m_height;
    position->z = this->current.pos.z;
    fopKyM_createWpillar(position.get(), 1.f, 0.75f, 0);
}
/* 0231D344 */
void daObjBarrel::Act_c::mode_vib0_init() {
    collision_mode(this);
    clear_carry_attention(this);
    this->gravity = -14.f;
    this->speed.y = 80.f;
    this->shape_angle.y = 0;
    this->shape_angle.z = 0;
    this->m610 = 3500;
    this->m612 = 0;
    this->m630 = 0;
    this->mTimer = 5;
    this->mMode = MODE_VIB0;
}
/* 0231D5B4 */
void daObjBarrel::Act_c::eff_land_smoke() {
    Barrel_make_land_effect(this, &this->mAcch.m_gnd, 1.f);
}
/* 0231E08C */
void daObjBarrel::Act_c::mode_vib1_init() {
    this->gravity = -1.f;
    this->mTimer = 12;
    this->mMode = MODE_VIB1;
}
/* 0231E0F0 */
void daObjBarrel::Act_c::mode_vib2_init() {
    this->gravity = -12.f;
    this->mTimer = 20;
    this->mMode = MODE_VIB2;
}
// HD: compiler-generated common-header initialization and destructors.
/* 0231E504 */
static void __sinit_d_a_obj_barrel_cpp() {
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
}
/* 0231E598 */
static void BarrelHeaderStatic__dt(void* object, u32 flags) {
    if (object && (flags & 1))
        Barrel_operator_delete(object);
}
/* 0231E5AC */
static void BarrelHeaderEmpty__dt(void* object, u32 flags) {}
/* 0231E5B0 */
static void Act_c__dt(Act_c* a, u32 flags) {
    if (a) {
        Barrel_Cyl_dt(&a->mCyl, 2);
        Barrel_Stts_dt(&a->mStts, 2);
        Barrel_AcchCirShape_dt(&a->mAcchCir.m_cir, 2);
        a->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
        a->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        Barrel_Acch_dt(&a->mAcch, 0);
        Barrel_actor_dt(a, 0);
        if (flags & 1)
            Barrel_operator_delete(a);
    }
}
/* 0231E64C */
bool daObjBarrel_Method_IsDelete(Act_c* a) { return true; }
/* 0231E654 */
static u32 PrmAbstract(Act_c* a, u32 width, u32 shift) {
    u32 shifted = (shift & 63) >= 32 ? 0 : (u32)a->mParameters >> (shift & 31);
    u32 limit = (width & 63) >= 32 ? 0 : 1u << (width & 31);
    return shifted & (limit - 1u);
}
/* 0231CAC4 */
void daObjBarrel::Act_c::eff_break() {
    gabi::Local<cXyz> position;
    position->x = this->current.pos.x;
    position->y = (f32)this->current.pos.y + 50.f;
    position->z = this->current.pos.z;
    auto* play = dComIfGp_get();
    auto* emitter = Barrel_particle_set(
        Barrel_gameInfo(play)->play.mParticle.get(), 0, BARREL_PARTICLE_BREAK, position.get(),
        nullptr, nullptr, 255, nullptr, -1, &this->tevStr.mColorK0, &this->tevStr.mColorK0, 0);
    if (emitter) {
        if (!(u32)l_scaleInitialized) {
            l_scaleInitialized = 1;
            l_emitterScale.x = 1.f;
            l_emitterScale.y = 0.8f;
            l_emitterScale.z = 1.f;
        }
        emitter->mScale.x = l_emitterScale.x;
        emitter->mScale.y = l_emitterScale.y;
        emitter->mScale.z = l_emitterScale.z;
    }
    Barrel_createSmoke(BARREL_SMOKE_PROCESS, 0, position.get(), -1, nullptr, nullptr, -1, nullptr);
}
/* 0231CC24 */
void daObjBarrel::Act_c::damaged(s32 animate) {
    Barrel_cancelCarryNow(this);
    if (animate) {
        this->eff_break();
        s32 reverb = Barrel_getReverb((s8)this->current.roomNo);
        Barrel_seStart(BARREL_SE_BREAK, &this->eyePos, 0, reverb);
        this->set_senv(150, 5);
    }
}
/* 0231D128 */
void daObjBarrel::Act_c::se_fall_water() {
    u32 sound = 0x13;
    cBgS_PolyInfo* polygons[2] = {
        reinterpret_cast<cBgS_PolyInfo*>(&this->mAcch.m_wtr),
        static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd),
    };
    for (cBgS_PolyInfo* polygon : polygons) {
        if (polygon->mBgIndex < 256) {
            auto* play = dComIfGp_get();
            sound = Barrel_GetMtrlSndId(&Barrel_gameInfo(play)->play.mBgS, polygon);
            break;
        }
    }
    s32 reverb = Barrel_getReverb((s8)this->current.roomNo);
    Barrel_seStart(BARREL_SE_WATER, &this->eyePos, sound, reverb);
    this->set_senv(125, 5);
}
/* 0231D2A0 */
bool daObjBarrel::Act_c::damage_bg_proc() {
    bool sunk = this->chk_sink_water();
    bool broken = false;
    if (((s32)this->mMode == MODE_WAIT || (s32)this->mMode == MODE_JUMP ||
         (s32)this->mMode == MODE_WALK) &&
        sunk) {
        if (!this->mSunk) {
            this->se_fall_water();
            this->eff_hit_water_splash();
            this->mSunk = 1;
        }
        if (this->chk_sinkdown_water()) {
            this->damaged(0);
            broken = true;
        }
    }
    return broken;
}
/* 0231D5C4 */
bool daObjBarrel::Act_c::damage_bg_proc_directly() {
    bool landing = this->mAcch.ChkGroundLanding();
    bool ground = this->mAcch.ChkGroundHit();
    bool broken = false;
    if ((s32)this->mMode == MODE_WAIT || (s32)this->mMode == MODE_JUMP ||
        (s32)this->mMode == MODE_WALK) {
        if (landing && ((f32)this->mLastGroundY - (f32)this->current.pos.y) > 200.f) {
            this->damaged(1);
            broken = true;
        }
        if (ground)
            this->mLastGroundY = this->current.pos.y;
    }
    if ((s8)this->mInitTimer > 0)
        this->mInitTimer = (s8)this->mInitTimer - 1;
    else {
        if (this->mAcch.ChkGroundHit()) {
            if (!this->mOnGround && (s32)this->mMode != MODE_CARRY) {
                auto* play = dComIfGp_get();
                u32 material = Barrel_GetMtrlSndId(&Barrel_gameInfo(play)->play.mBgS,
                                                   static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd));
                s32 reverb = Barrel_getReverb((s8)this->current.roomNo);
                Barrel_seStart(BARREL_SE_TAKE_OFF, &this->eyePos, material, reverb);
                this->mOnGround = 1;
            }
        } else
            this->mOnGround = 0;
        if (landing && ((s32)this->mMode != MODE_CARRY || (s32)this->mTimer <= 0))
            this->eff_land_smoke();
    }
    return broken;
}
/* 0231DAE0 */
bool daObjBarrel::Act_c::_draw() {
    auto* light = dKy_getEnvlight();
    settingTevStruct(light, 0, &this->current.pos, &this->tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, this->mpModel, &this->tevStr);
    Barrel_modelUpdateDL((J3DModel*)this->mpModel, 0);
    return true;
}
/* 0231DE48 */
void daObjBarrel::Act_c::mode_carry() {
    s16 angle = this->m61C;
    if ((s32)this->mTimer > 0)
        this->mTimer = (s32)this->mTimer - 1;
    Barrel_chaseAngleS(&this->shape_angle.z, angle, 0x1000);
    if (!((u32)this->actor_status & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e))) {
        this->mLastGroundY = this->current.pos.y;
        Barrel_SetCurrentRoomNo(this, &this->mAcch.m_gnd);
        this->m610 = 0;
        this->m612 = 0;
        this->mode_wait_init();
    }
}
/* 0231DED0 */
void daObjBarrel::Act_c::vib_pos_ang() {
    f32 factor = this->mAcch.ChkGroundHit() ? 0.8f : 1.f;
    f32 angle = gabi::fmadds((f32)(s16)this->m610, 8.544921729480848e-05f, 0.3f);
    angle = ((16000.f * angle) * 0.1f) * factor;
    Barrel_posMoveF(this, &this->mStts.m_cc_move);
    this->shape_angle.y = (s16)this->shape_angle.y + (s16)gabi::ftoi(angle);
    this->m612 = (s16)this->m612 + (s16)gabi::ftoi(14400.f * factor);
}
/* 0231E0AC */
void daObjBarrel::Act_c::mode_vib0() {
    this->vib_pos_ang();
    this->mTimer = (u32)this->mTimer - 1u;
    if ((s32)this->mTimer <= 0)
        this->mode_vib1_init();
}
/* 0231E110 */
void daObjBarrel::Act_c::mode_vib1() {
    this->vib_pos_ang();
    this->mTimer = (u32)this->mTimer - 1u;
    if ((s32)this->mTimer <= 0)
        this->mode_vib2_init();
}
/* 0231E154 */
void daObjBarrel::Act_c::mode_vib2() {
    Barrel_chaseAngleS(&this->m610, 0, 150);
    this->vib_pos_ang();
    this->mTimer = (u32)this->mTimer - 1u;
    if ((s32)this->mTimer <= 0) {
        this->shape_angle.x = 0;
        this->m610 = 0;
        this->m612 = 0;
        this->mode_wait_init();
    }
}
/* 0231E500 */
bool daObjBarrel_Method_Draw(Act_c* a) { return a->_draw(); }
/* 0231DB3C */
void daObjBarrel::Act_c::set_walk_rot() {
    gabi::Local<cXyz> delta;
    Barrel_xyzSubtract(&this->current.pos, delta.get(), &this->old.pos);
    gabi::Local<cXyz> horizontal;
    horizontal->x = delta->x;
    horizontal->y = 0.f;
    horizontal->z = delta->z;
    f32 magnitude = Barrel_xyzAbs2(horizontal.get());
    magnitude = Barrel_sqrtf(magnitude);
    // HD: the caller keeps atan2s's full return register until the explicit s16 casts.
    s32 target = Barrel_atan2s((f32)delta->x, (f32)delta->z);
    this->current.angle.y = target;
    bool negative = target < 0;
    if (negative)
        target = (s16)(target - 0x8000);
    if (magnitude > 5.f || ((s32)this->mMode == MODE_WAIT && magnitude > 2.5f))
        Barrel_chaseAngleS(&this->shape_angle.y, target, 0x600);
    f32 cosine = cM_scos(this->shape_angle.z);
    f32 roll = (magnitude / (6.28f * gabi::fmadds(5.f, cosine, 45.f))) * 65535.f;
    s16 rotation = gabi::ftoi(roll * 3.f);
    s16 walk = gabi::ftoi(roll);
    if (negative) {
        this->m612 = (s16)this->m612 + rotation;
        this->m630 = (s16)this->m630 + walk;
    } else {
        this->m612 = (s16)this->m612 - rotation;
        this->m630 = (s16)this->m630 - walk;
    }
    f32 factor = magnitude / 30.f;
    if (factor > 1.f)
        factor = 1.f;
    this->m610 = gabi::ftoi(2048.f * factor);
}
/* 0231DD40 */
void daObjBarrel::Act_c::mode_wait() {
    Barrel_posMoveF(this, &this->mStts.m_cc_move);
    Barrel_chaseAngleS(&this->shape_angle.z, (s16)this->shape_angle.z > 0x1000 ? 0x4000 : 0, 0xC00);
    this->set_walk_rot();
    if (this->mAcch.ChkGroundHit()) {
        this->attention_info.flags |= fopAc_Attn_ACTION_CARRY_e;
        if ((s16)this->shape_angle.z == 0x4000) {
            auto* play = dComIfGp_get();
            auto* normal =
                Barrel_GetTriNormal(&Barrel_gameInfo(play)->play.mBgS,
                                    static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)->mBgIndex,
                                    static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)->mPolyIndex);
            if (normal &&
                (std::fabs((f32)normal->x) > 0.001f || std::fabs((f32)normal->z) > 0.001f))
                this->mode_walk_init();
            this->mStts.m_weight = 140;
        } else
            this->mStts.m_weight = 200;
    } else
        clear_carry_attention(this);
}
/* 0231E1BC */
void daObjBarrel::Act_c::mode_jump() {
    Barrel_SetCurrentRoomNo(this, &this->mAcch.m_gnd);
    if (this->mAcch.ChkGroundLanding())
        this->mode_walk_init();
    else {
        Barrel_posMoveF_stream(this, &this->mStts.m_cc_move, &this->mMove, 0.006f, 0.001f);
        Barrel_chaseAngleS(&this->shape_angle.z, 0x4000, 0xC00);
        this->set_walk_rot();
    }
}
/* 0231E248 */
void daObjBarrel::Act_c::mode_walk() {
    Barrel_SetCurrentRoomNo(this, &this->mAcch.m_gnd);
    if (this->mAcch.ChkGroundHit()) {
        auto* play = dComIfGp_get();
        auto* normal =
            Barrel_GetTriNormal(&Barrel_gameInfo(play)->play.mBgS,
                                static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)->mBgIndex,
                                static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)->mPolyIndex);
        gabi::Local<cXyz> acceleration;
        *acceleration = cXyz::Zero;
        f32 friction = 0.f, cosine = 0.f;
        if (normal) {
            acceleration->x = normal->x;
            acceleration->y = 0.f;
            acceleration->z = normal->z;
            friction = 0.1f;
            cosine = cM_scos(0xA4F);
        }
        Barrel_posMoveF_grade(this, &this->mStts.m_cc_move, &this->mMove, 0.006f, 0.001f, normal,
                              friction, cosine, acceleration.get());
        Barrel_chaseAngleS(&this->shape_angle.z, 0x4000, 0xC00);
        this->set_walk_rot();
        this->attention_info.flags |= fopAc_Attn_ACTION_CARRY_e;
        if (normal) {
            gabi::Local<cXyz> horizontal;
            horizontal->x = this->speed.x;
            horizontal->y = 0.f;
            horizontal->z = this->speed.z;
            f32 magnitude = Barrel_xyzAbs2(horizontal.get());
            if (magnitude < 0.1f && !(std::fabs((f32)normal->x) < 0.001f) &&
                !(std::fabs((f32)normal->z) < 0.001f))
                this->mode_wait_init();
        }
    } else {
        Barrel_posMoveF_stream(this, &this->mStts.m_cc_move, &this->mMove, 0.006f, 0.001f);
        Barrel_chaseAngleS(&this->shape_angle.z, 0x4000, 0xC00);
        this->set_walk_rot();
    }
}
/* inlined into Act_c::_create and Act_c::mode_carry_init */
// HD: both strings use native eight-byte stack slots and repeated termination calls.
static bool stage_matches(const char* literal) {
    gabi::Local<be<u32>[2]> expected, stage;
    (*expected)[0] = gabi::guest_addr(literal);
    (*expected)[1] = gc_tu.safeString;
    (*stage)[0] = gabi::ea(Barrel_gameInfo(dComIfGp_get())->play.mCurStage.mName);
    (*stage)[1] = gc_tu.safeString;
    u32 ensure = l_stringVtable.mAssureTermination.get();
    Barrel_assureTermination(ensure, expected.get());
    Barrel_assureTermination(ensure, expected.get());
    u32 left = (*expected)[0];
    Barrel_assureTermination(ensure, stage.get());
    u32 right = (*stage)[0];
    if (left == right)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 value = gabi::load<u8>(left + i);
        if (value != gabi::load<u8>(right + i))
            return false;
        if (!value)
            return true;
    }
    return false;
}
/* 0231D3BC */
void daObjBarrel::Act_c::mode_carry_init() {
    collision_mode(this, true);
    clear_carry_attention(this);
    this->mMode = MODE_CARRY;
    if (stage_matches("majroom") || stage_matches("MajyuE"))
        Barrel_onEventBit(&dComIfGs_info()->mSavedata.mEvent, 0x401);
    this->mTimer = 15;
}
/* 0231D724 */
bool daObjBarrel::Act_c::mode_proc_call() {
    if (((u32)this->actor_status & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e)) &&
        (s32)this->mMode != MODE_CARRY)
        this->mode_carry_init();
    (this->*mode_proc.mProc[(u32)this->mMode])();
    f32 y = this->current.pos.y, z = this->current.pos.z, x = this->current.pos.x;
    auto* play = dComIfGp_get();
    this->mAcch.CrrPos(&Barrel_gameInfo(play)->play.mBgS);
    play = dComIfGp_get();
    if (Barrel_ChkMoveBG(&Barrel_gameInfo(play)->play.mBgS,
                         static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)))
        this->mForceExec = 1;
    if ((s32)this->mMode == MODE_CARRY) {
        this->current.pos.y = y;
        this->current.pos.x = x;
        this->current.pos.z = z;
    }
    if (this->damage_bg_proc_directly())
        return false;
    if ((s32)this->mMode != MODE_CARRY) {
        this->tevStr.mRoomNo = this->current.roomNo;
        play = dComIfGp_get();
        this->tevStr.mEnvrIdxOverride = (u8)(Barrel_GetPolyColor(
            &Barrel_gameInfo(play)->play.mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)));
    }
    Barrel_xyzScale(&this->mMove, &this->mMove, 0.95f);
    if (Barrel_xyzAbs2(&this->mMove) < 0.1f) {
        this->mMove.x = 0.f;
        this->mMove.z = 0.f;
        this->mMove.y = 0.f;
    }
    return true;
}
/* 0231C674 */
s32 daObjBarrel::Act_c::_create() {
    if (!((u32)this->actor_condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            this->__vtbl = BARREL_ACTOR_VTABLE;
            dBgS_ObjAcch_ct(&this->mAcch, gc_tu.objAcch);
            Barrel_AcchCir_ct(&this->mAcchCir);
            dCcD_Stts_ct(&this->mStts);
            dCcD_Cyl_ct(&this->mCyl, BARREL_AAB_VTABLE);
        }
        this->actor_condition |= fopAcCnd_INIT_e;
    }
    s32 phase = Barrel_resLoad(&this->mPhs, M_arcname);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!Barrel_entrySolidHeap(this, gabi::guest_fn((const void*)solidHeapCB), 0x820))
        return cPhs_ERROR_e;
    this->mAcchCir.SetWall(30.f, 50.f);
    this->mAcch.Set(&this->current.pos, &this->old.pos, this, 1, &this->mAcchCir, &this->speed,
                    &this->current.angle, &this->shape_angle);
    this->mAcch.m_flags &= ~dBgS_Acch::WATER_NONE;
    J3DModel* model = this->mpModel;
    this->cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    this->cull_set_draw();
    if (stage_matches("sea")) {
        auto* play = dComIfGp_get();
        auto* stage = reinterpret_cast<BarrelStageDt*>(Barrel_gameInfo(play)->play.mStageData);
        BarrelStageInfo* info = stage->getStagInfo();
        f32 far = info->mCullPoint;
        if (far > 1.f)
            this->cullSizeFar = 8000.f / far;
    }
    this->mStts.Init(200, 255, this);
    this->mCyl.Set(&M_cyl_src);
    this->attention_info.flags |= fopAc_Attn_ACTION_CARRY_e;
    this->mCyl.SetStts(&this->mStts);
    this->gravity = -6.f;
    this->attention_info.position.x = this->current.pos.x;
    this->attention_info.position.y = (f32)this->current.pos.y + 50.f;
    this->attention_info.position.z = this->current.pos.z;
    this->mForceExec = 1;
    this->attention_info.distances[fopAc_Attn_TYPE_CARRY_e] = 9;
    Barrel_posMoveF(this, nullptr);
    auto* play = dComIfGp_get();
    this->mAcch.CrrPos(&Barrel_gameInfo(play)->play.mBgS);
    this->mOnGround = 1;
    this->mInitTimer = 20;
    this->mSunk = 0;
    this->mAcch.m_flags &= ~dBgS_Acch::GROUND_LANDING;
    this->mLastGroundY = this->current.pos.y;
    this->init_mtx();
    this->mMove = cXyz::Zero;
    this->mode_wait_init();
    return phase;
}
/* 0231CD50 */
bool daObjBarrel::Act_c::damage_cc_proc() {
    if (this->mCyl.ChkAtHit()) {
        this->damaged(1);
        this->mCyl.ClrAtHit();
        return true;
    }
    if (!this->mCyl.ChkTgHit())
        return false;
    auto* hit = static_cast<dCcD_GObjInf*>(this->mCyl.GetTgHitObj());
    u32 sound = this->get_se_map_hit();
    if (hit) {
        u32 type = (u32)hit->mAtType;
        if ((type & (BARREL_AT_BOMB | BARREL_AT_UNKNOWN8 | BARREL_AT_SKULL_HAMMER)) ||
            (((u32)hit->mObjAt.mSPrm & 4) && ((u8)this->mStts.mAtSpl & 3))) {
            this->damaged(1);
            this->mCyl.ClrTgHit();
            return true;
        }
        if (((s32)this->mMode == MODE_WAIT || (s32)this->mMode == MODE_WALK) &&
            (type & BARREL_AT_WIND)) {
            gabi::Local<cXyz> wind, normal;
            *wind = this->mCyl.mGObjTg.mRVec;
            f32 magnitude = Barrel_xyzAbs2(wind.get());
            if (magnitude > 31684.f) {
                f32 root = Barrel_sqrtf(magnitude);
                Barrel_xyzScale(wind.get(), wind.get(), 178.f / root);
            }
            BarrelShape* shape = Barrel_getShapeAttr(hit);
            *normal = cXyz::Zero;
            f32 strength = 1.f;
            if (shape->getNVec(&this->current.pos, normal.get())) {
                Barrel_xyzScale(normal.get(), normal.get(), 25.f);
                Barrel_xyzAbs2(&this->mMove);
                auto* actor = Barrel_GetAc(&this->mCyl.mGObjTg);
                if (actor && (s16)actor->mProfName == 0xA8) {
                    s32 angle = Barrel_atan2s((f32)normal->x, (f32)normal->z);
                    u16 difference = (s16)actor->shape_angle.y - angle;
                    f32 cosine = cM_scos(difference);
                    if (cosine > 0.f)
                        strength = (cosine + cosine) + 1.f;
                }
            }
            f32 ratio = (0.01f - magnitude) >= 0.f ? 0.f : 0.5f;
            gabi::Local<cXyz> windPart, normalPart, scaledNormal, result;
            Barrel_xyzMultiply(wind.get(), windPart.get(), ratio);
            Barrel_xyzMultiply(normal.get(), normalPart.get(), 1.f - ratio);
            Barrel_xyzMultiply(normalPart.get(), scaledNormal.get(), strength);
            Barrel_xyzAdd(windPart.get(), result.get(), scaledNormal.get());
            this->mMove = *result;
            if ((s32)this->mMode == MODE_WAIT) {
                if ((s16)this->shape_angle.z == 0)
                    this->mode_jump_init();
                else
                    this->mode_walk_init();
            }
        }
    }
    Barrel_HitSeStart(&this->eyePos, (s8)this->current.roomNo, &this->mCyl, sound);
    this->set_senv(100, 4);
    Barrel_HitEff_kikuzu(this, &this->mCyl);
    this->mCyl.ClrTgHit();
    return false;
}
/* 0231D944 */
bool daObjBarrel::Act_c::_execute() {
    this->cull_set_move();
    bool execute = this->mForceExec || (s32)this->mMode != MODE_WAIT ||
                   !this->mAcch.ChkGroundHit() || this->mAcch.ChkGroundLanding();
    if (!execute) {
        execute = !PrmAbstract(this, PRM_CULL_W, PRM_CULL_S) ||
                  !((u32)this->actor_condition & fopAcCnd_NODRAW_e) || !Barrel_cullingCheck(this);
    }
    if (execute) {
        this->mForceExec = 0;
        bool broken = true;
        if (!this->damage_cc_proc() && !this->damage_bg_proc()) {
            s32 mode = this->mMode;
            if (mode != MODE_VIB0 && mode != MODE_VIB1 && mode != MODE_VIB2 && mode != MODE_JUMP &&
                mode != MODE_WALK && !((u32)this->actor_status & fopAcStts_CARRY_e) &&
                (s16)this->shape_angle.x != 0)
                this->mode_vib0_init();
            if (this->mode_proc_call()) {
                broken = false;
                this->set_mtx();
                this->mStts.mRoomId = this->current.roomNo;
                Barrel_Cyl_SetC(&this->mCyl.mCyl, &this->current.pos);
                auto* play = dComIfGp_get();
                Barrel_Cc_Set(&Barrel_gameInfo(play)->play.mCcS, &this->mCyl);
                this->eyePos.x = this->current.pos.x;
                this->eyePos.y = (f32)this->current.pos.y + 50.f;
                this->eyePos.z = this->current.pos.z;
                this->attention_info.position = this->eyePos;
            }
        }
        if (broken)
            fopAcM_delete(this);
    }
    this->cull_set_draw();
    return true;
}
/* 0231E4F4 */
s32 daObjBarrel_Method_Create(Act_c* a) { return a->_create(); }
/* 0231E4F8 */
bool daObjBarrel_Method_Delete(Act_c* a) { return a->_delete(); }
/* 0231E4FC */
bool daObjBarrel_Method_Execute(Act_c* a) { return a->_execute(); }
/* WWHD glue (tools/verify/autoglue.py): barrel translation unit
data l_resControl 101F4F28
data l_stringVtable 10025558
data l_scaleInitialized 104691FC
data l_emitterScale 10469200
data mode_proc 10025660
data M_cyl_src 100256F0
str 100256E4 "Ktaru_01"
str 100255D0 "d_a_obj_barrel.cpp"
str 100255C0 "mdl_data != 0"
str 10025644 "majroom"
str 1002564C "MajyuE"
str 10025614 "sea"
ext 026066C4 J3DModelData* Barrel_getRes(void*, void*, s32)
ext 0273AA24 void Barrel_assert(const char*, s32, const char*)
ext 025D6768 void Barrel_setCullSphere(fopAc_ac_c*, f32, f32, f32, f32)
ext 028E93CC void Barrel_transS(Mtx34*, f32, f32, f32)
ext 025F24E0 void Barrel_transM(f32, f32, f32)
ext 025F1B48 void Barrel_ZXYrotM(Mtx34*, s32, s16, s16)
ext 025F1C28 void Barrel_YrotM(Mtx34*, s16)
ext 025F1BF4 void Barrel_XrotM(Mtx34*, s16)
ext 0255F458 void Barrel_Sound_set(cXyz*, s32, u32, s32)
ext 02311CD8 void Barrel_make_land_effect(fopAc_ac_c*, dBgS_GndChk*, f32)
ext 0273AF40 void Barrel_operator_delete(void*)
ext 02515A70 void Barrel_Cyl_dt(dCcD_Cyl*, s32)
ext 02515860 void Barrel_Stts_dt(dCcD_Stts*, s32)
ext 02018034 void Barrel_AcchCirShape_dt(void*, s32)
ext 024EFD9C void Barrel_Acch_dt(dBgS_Acch*, s32)
ext 025D50BC void Barrel_actor_dt(fopAc_ac_c*, s32)
ext 025A847C BarrelEmitter* Barrel_particle_set(dPa_control_c*, s32, s32, cXyz*, csXyz*, cXyz*, s32, void*, s32, GXColor*, GXColor*, s32)
ext 025D5834 void Barrel_createSmoke(s32, s32, cXyz*, s32, csXyz*, cXyz*, s32, void*)
ext 025D9D24 void Barrel_cancelCarryNow(fopAc_ac_c*)
ext 02520540 s32 Barrel_getReverb(s8)
ext 025E1A40 void Barrel_seStart(u32, cXyz*, u32, s32)
ext 024EECAC u32 Barrel_GetMtrlSndId(dBgS*, cBgS_PolyInfo*)
ext 025E2DE0 void Barrel_modelUpdateDL(J3DModel*, s32)
ext 0200F8D0 void Barrel_chaseAngleS(be<s16>*, s32, s32)
ext 02312968 void Barrel_SetCurrentRoomNo(fopAc_ac_c*, dBgS_GndChk*)
ext 025D6870 void Barrel_posMoveF(fopAc_ac_c*, cXyz*)
ext 0201ADE0 void Barrel_xyzSubtract(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f32 Barrel_xyzAbs2(cXyz*)
ext 028F4384 f32 Barrel_sqrtf(f32)
ext 020195B0 s32 Barrel_atan2s(f32, f32)
ext 020084C8 cXyz* Barrel_GetTriNormal(dBgS*, u16, u16)
ext 023123C0 void Barrel_posMoveF_stream(fopAc_ac_c*, cXyz*, cXyz*, f32, f32)
ext 023121C4 void Barrel_posMoveF_grade(fopAc_ac_c*, cXyz*, cXyz*, f32, f32, cXyz*, f32, f32, cXyz*)
ext 025B8B68 void Barrel_onEventBit(void*, s32)
ext 024EEABC s32 Barrel_ChkMoveBG(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u32 Barrel_GetPolyColor(dBgS*, cBgS_PolyInfo*)
ext 024EFE94 void Barrel_AcchCir_ct(dBgS_AcchCir*)
ext 02520460 s32 Barrel_resLoad(request_of_phase_process_class*, const char*)
ext 025D63E8 s32 Barrel_entrySolidHeap(fopAc_ac_c*, u32, u32)
ext 028E8E64 void Barrel_xyzScale(cXyz*, cXyz*, f32)
ext 02515BBC fopAc_ac_c* Barrel_GetAc(dCcD_GObjTg*)
ext 0201AE48 void Barrel_xyzMultiply(cXyz*, cXyz*, f32)
ext 0201AD78 void Barrel_xyzAdd(cXyz*, cXyz*, cXyz*)
ext 023129C4 void Barrel_HitSeStart(cXyz*, s8, dCcD_Cyl*, u32)
ext 02312C8C void Barrel_HitEff_kikuzu(fopAc_ac_c*, dCcD_Cyl*)
ext 025D6CE8 s32 Barrel_cullingCheck(fopAc_ac_c*)
ext 020182E0 void Barrel_Cyl_SetC(cM3dGCyl*, cXyz*)
ext 0200E240 void Barrel_Cc_Set(cCcS*, dCcD_Cyl*)
*/
