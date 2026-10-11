/* Forsaken Fortress dishes, derived from the WWHD disassembly. */
#include "d/actor/d_a_obj_mshokki.h"
enum { AT_TYPE_WIND = 0x200000 };
static MshIntTable bdl_idx;
static MshIntTable se_flag;
static MshParticleTable particle_id;
static MshDataTable l_data;
static MshCylinderSource l_cyl_src;
static s32 wrappedAbs(s32 v) {
    return v < 0 ? (s32)(0u - (u32)v) : v;
}
static u32 unsignedFloat(f32 v) {
    // HD: bge after fcmpu tests !LT: an unordered value takes the high-half path.
    return v < 2147483648.0f ? (u32)gabi::ftoi(v)
                             : (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
}
/* 0237CE88 */
bool daObjMshokki_c::create_heap() {
    s32 index = (s32)bdl_idx.values[(s32)m60C];
    auto *data = (J3DModelData *)dComIfG_getObjectRes("Mshokki", index, 0x1002DADC);
    if (!data) {
        JUT_ASSERT_fail(STR(0x1002DBBC), 0x164, STR(0x1002DBB8));
        return false;
    }
    mModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    return mModel != nullptr;
}
/* 0237CF44 */
static BOOL solidHeapCB(fopAc_ac_c *base) {
    return static_cast<daObjMshokki_c *>(base)->create_heap();
}
/* 0237CF48 */
// HD: the static callback remains free for the current glue generator.
static void co_hitCallback(fopAc_ac_c *actor, dCcD_GObjInf *own, fopAc_ac_c *other,
                           dCcD_GObjInf *hit) {
    auto *self = (daObjMshokki_c *)actor;
    f32 dx = self->current.pos.x - other->current.pos.x;
    f32 dz = self->current.pos.z - other->current.pos.z;
    s16 angle = cM_atan2s(dx, dz);
    auto *p = mshPlay();
    bool aligned = gabi::ea(other) == gabi::ea(static_cast<fopAc_ac_c *>(p->mPlayer)) &&
                   (f32)wrappedAbs((s32)(s16)other->current.angle.y - angle) < 4096.0f;
    f32 speed = other->speedF;
    if (aligned && !(speed < 17.0f) && wrappedAbs((s32)((u32)self->m618 - (u32)self->m614)) > 2) {
        f32 strength = speed - 17.0f;
        // HD: ble means !GT; NaN is retained, rather than clamped to one.
        if (strength > 1.0f)
            strength = 1.0f;
        s32 turn = gabi::ftoi(6144.0f * strength);
        f32 horizontal = speed * cM_scos(turn);
        f32 vertical = gabi::fmadds(0.0f, strength, 24.0f);
        self->current.angle.y = angle;
        self->shape_angle.y = angle;
        u32 frame = self->m614;
        self->mCyl.mGObjCo.mHitCallback = 0;
        self->speedF = horizontal;
        self->speed.y = vertical;
        self->m618 = frame;
        return;
    }
    // HD: blt rejects only ordered values below six; unordered values continue.
    if (speed < 6.0f)
        return;
    f32 strength = (speed - 6.0f) / 11.0f;
    s32 type = self->m60C;
    if (strength > 1.0f)
        strength = 1.0f;
    s16 tilt = (s16)gabi::ftoi(strength * (f32)l_data.values[type].m0C);
    s16 facing = other->current.angle.y;
    self->m61C = (s16)(facing + 0x2000);
    self->shape_angle.x = tilt;
    self->shape_angle.y = facing;
}
/* 0237D17C */
void daObjMshokki_c::set_mtx() {
    J3DModel_setBaseScale(mModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM((s16)shape_angle.y);
    mDoMtx_stack_c::XrotM((s16)shape_angle.x);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
}
static u32 paramAbstract(daObjMshokki_c *self, u32 width, u32 shift);
/* 0237D260 */
cPhs_State daObjMshokki_c::_create() {
    fopAcM_ct(this, daObjMshokki_c);
    if (mInitState == 0) {
        u32 type = paramAbstract(this, PRM_ARG_W, PRM_ARG_S);
        m60C = (s32)type;
        if (type >= 3) {
            JUT_ASSERT_fail(STR(0x1002DC0C), 0x288, STR(0x1002DC08));
            m60C = 0;
        }
    }
    cPhs_State result = dComIfG_resLoad(&mPhase, "Mshokki");
    if (result != cPhs_COMPLEATE_e)
        return result;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x4C0))
        return cPhs_ERROR_e;
    mAcchCir.SetWall(50.0f, 35.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    u32 flags = mAcch.m_flags;
    mAcch.m_roof_crr_height = 50.0f;
    mAcch.m_flags = flags & ~u32(dBgS_Acch::WATER_NONE | dBgS_Acch::ROOF_NONE);
    auto *p = mshPlay();
    mAcch.CrrPos(&p->mBgS);
    mAcch.m_flags = (u32)mAcch.m_flags & ~u32(dBgS_Acch::GROUND_LANDING);
    mStts.Init(100, 255, this);
    mCyl.Set(reinterpret_cast<dCcD_SrcCyl *>(&l_cyl_src));
    mCyl.SetStts(&mStts);
    mshStartCTg(&mCyl, &current.pos);
    mCyl.SetR((f32)l_data.values[(s32)m60C].radius);
    mCyl.SetH((f32)l_data.values[(s32)m60C].height);
    mCyl.mGObjCo.mHitCallback = gabi::guest_fn((const void *)co_hitCallback);
    set_mtx();
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel));
    gravity = -6.0f;
    return result;
}
/* 0237D53C */
static cPhs_State actorCreate(daObjMshokki_c *self) {
    return self->_create();
}
/* 0237D540 */
bool daObjMshokki_c::_delete() {
    dComIfG_resDelete(&mPhase, "Mshokki");
    return true;
}
/* 0237D570 */
static bool actorDelete(daObjMshokki_c *self) {
    return self->_delete();
}
/* 0237D574 */
void daObjMshokki_c::set_se() {
    s16 delta = (s16)((s16)shape_angle.y - (s16)m61C);
    if (wrappedAbs(delta) >= 4096)
        return;
    f32 tilt = (f32)(s16)shape_angle.x;
    s32 type = m60C;
    s16 phase = (s16)gabi::ftoi((tilt / (f32)l_data.values[type].m0C) * 16384.0f);
    if (phase > 0x4000)
        phase = 0x4000;
    f32 intensity = cM_ssin(phase) * 100.0f;
    u32 sound = (u32)se_flag.values[type];
    u32 volume = unsignedFloat(intensity);
    s32 reverb = dComIfGp_getReverb((s8)current.roomNo);
    mDoAud_seStart(sound, &current.pos, volume, reverb);
}
/* 0237D6DC */
void daObjMshokki_c::break_proc() {
    u16 particle = (u16)particle_id.values[(s32)m60C];
    MshParticleController *control = mshPlay()->mParticle;
    mshParticleSet(control, 0, particle, &current.pos, &shape_angle, (u8 *)nullptr, 255,
                   (u8 *)nullptr, -1, &tevStr.mColorK0, 0, 0);
    auto *p = mshPlay();
    u32 material = p->mBgS.GetMtrlSndId(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd));
    s32 reverb = dComIfGp_getReverb((s8)current.roomNo);
    mDoAud_seStart(0x6806, &current.pos, material, reverb);
    gabi::Local<cXyz> position;
    position->set(current.pos.x, current.pos.y, current.pos.z);
    u32 id = (u32)mBsPcId;
    mshSoundSet(position.get(), 150, id, 5);
    fopAcM_delete(this);
}
/* 0237D7C8 */
bool daObjMshokki_c::checkCollision() {
    if (!mCyl.ChkTgHit())
        return false;
    auto *hit = static_cast<dCcD_GObjInf *>(mCyl.GetTgHitObj());
    bool broken = false;
    if (hit) {
        if (hit->ChkAtType(AT_TYPE_WIND)) {
            gabi::Local<cXyz> force;
            force->set((f32)mCyl.mGObjTg.mRVec.x, 0.0f, (f32)mCyl.mGObjTg.mRVec.z);
            f32 square = PSVECSquareMag(force.get());
            f32 magnitude = mshSqrt(square);
            f32 z = (f32)mCyl.mGObjTg.mRVec.z, x = (f32)mCyl.mGObjTg.mRVec.x;
            speedF = magnitude * 0.5f;
            current.angle.y = cM_atan2s(x, z);
        } else {
            break_proc();
            broken = true;
        }
    }
    mCyl.ClrTgHit();
    return broken;
}
/* 0237D89C */
bool daObjMshokki_c::_execute() {
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    auto *p = mshPlay();
    mAcch.CrrPos(&p->mBgS);
    f32 damped = (f32)speedF * 0.8f;
    speedF = damped;
    if (damped < 0.1f)
        speedF = 0.0f;
    if (mAcch.ChkGroundHit()) {
        s16 tilt = shape_angle.x;
        if (wrappedAbs(tilt) > 128) {
            shape_angle.x = (s16)(tilt - 128);
            shape_angle.y = (s16)((s16)shape_angle.y + 4096);
            set_se();
        } else
            shape_angle.x = 0;
    } else
        shape_angle.x = (s16)((s16)shape_angle.x - 3072);
    u32 flags = mAcch.m_flags;
    if (m612 == 1 && (flags & dBgS_Acch::GROUND_LANDING))
        break_proc();
    else {
        if ((flags & dBgS_Acch::GROUND_HIT) && !(flags & dBgS_Acch::GROUND_LANDING))
            m612 = 1;
        mStts.Move();
        if (!checkCollision()) {
            set_mtx();
            mshMoveCTg(&mCyl, &current.pos);
            mStts.mRoomId = (u8)(s8)current.roomNo;
            p = mshPlay();
            mshCcSet(&p->mCcS, &mCyl);
            f32 x = current.pos.x, y = current.pos.y + (f32)l_data.values[(s32)m60C].m00,
                z = current.pos.z;
            attention_info.position.x = x;
            attention_info.position.y = y;
            attention_info.position.z = z;
            eyePos.set(x, y, z);
        }
    }
    // HD: retain wrapping arithmetic for the launch cooldown counter.
    m614 = (u32)m614 + 1;
    return true;
}
/* 0237DA70 */
static bool actorExecute(daObjMshokki_c *self) {
    return self->_execute();
}
/* 0237DA74 */
bool daObjMshokki_c::_draw() {
    auto *env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    J3DModel *model = mModel;
    setLightTevColorType(env, model, &tevStr);
    mDoExt_modelUpdateDL(mModel);
    return true;
}
/* 0237DAD0 */
static bool actorDraw(daObjMshokki_c *self) {
    return self->_draw();
}
/* 0237DAD4 */
static BOOL isDelete(daObjMshokki_c *self) {
    return 1;
}
/* 0237DADC */
static void initStatics() {
    sinit_header_statics(0x1046B9DC, 0x101CBE20);
}
/* 0237DB70 */
static void trivialDestructor(void *self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}
/* 0237DB84 */
static void actorDestructor(daObjMshokki_c *self, s32 flags) {
    if (!self)
        return;
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    mshWallCylDt(&self->mAcchCir.m_cir, 2);
    static_cast<dBgS_GrpPassChk &>(self->mAcch).__vtbl = 0x1002DB14;
    static_cast<dBgS_PolyPassChk &>(self->mAcch).__vtbl = 0x1002DB24;
    mshAcchDt(&self->mAcch, 0);
    mshActorDt(self, 0);
    if (flags & 1)
        operator_delete(self);
}
/* 0237DC20 */
static void emptyVirtual(void *self) {
}
/* 0237DC24 */
static u32 paramAbstract(daObjMshokki_c *self, u32 width, u32 shift) {
    u32 mask = (width & 0x20) ? 0 : (1u << (width & 31));
    u32 value = (shift & 0x20) ? 0 : ((u32)self->mParameters >> (shift & 31));
    return value & (mask - 1);
}

daObjMshokki_c::daObjMshokki_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1002DB34;
    mshAcchCtor(&mAcch);
    static_cast<cBgS_Chk &>(mAcch).__vtbl = 0x1002DB04;
    static_cast<dBgS_GrpPassChk &>(mAcch).__vtbl = 0x1002DB14;
    static_cast<dBgS_PolyPassChk &>(mAcch).__vtbl = 0x1002DB24;
    static_cast<dBgS_PolyPassChk &>(mAcch).mbObjThrough = 1;
    dBgS_AcchCir_ct(&mAcchCir);
    dCcD_Stts_ct(&mStts);
    dCcD_Cyl_ct(&mCyl, 0x1002DAF4);
}
// clang-format off
/* WWHD glue
str 1002DAA8 "Mshokki"
data bdl_idx 101CBE00
data se_flag 101CBE0C
data particle_id 101CBE18
data l_data 1002DB44
data l_cyl_src 1002DB74
ext 024F0474 void mshAcchCtor(dBgS_ObjAcch*)
ext 02516580 void mshStartCTg(dCcD_Cyl*,cXyz*)
ext 02516680 void mshMoveCTg(dCcD_Cyl*,cXyz*)
ext 025A847C void mshParticleSet(MshParticleController*,s32,u32,cXyz*,csXyz*,void*,s32,void*,s32,GXColor*,s32,s32)
ext 0255F458 void mshSoundSet(cXyz*,s32,u32,s32)
ext 028F4384 f32 mshSqrt(f32)
ext 0200E240 void mshCcSet(u8*,dCcD_Cyl*)
ext 02018034 void mshWallCylDt(void*,s32)
ext 024EFD9C void mshAcchDt(dBgS_ObjAcch*,s32)
ext 025D50BC void mshActorDt(void*,s32)
*/
// clang-format on
