/** Liftable and breakable stones, WWHD. */
#include "d/actor/d_a_stone2.h"
using daStone2::Act_c;
using daStone2::Attr_c;
const gc_tu_data gc_tu = {.safeString = 0x1003E3E0,
                          .objAcch = {0x1003E408, 0x1003E428, 0x1003E418},
                          .objAcchDt = {0x1003E418, 0x1003E428},
                          .headerStatics = {0x1046E050, 0x101D1134, 0x1046E05C}};
static stone2::AttrTable M_attr;
static stone2::DemoTable demo_proc;
static stone2::ModeTable mode_proc;
static stone2::EffectTable eff_break_proc;
static stone2::SourceCylinder M_cyl_src;
static Mtx34 M_tmp_mtx;
static stone2::SmokeConstants l_smokeConstants;
static f32 l_zero;
static f32 l_one;
static f32 l_wallHeight;
static f32 l_invalidGround;
static stone2::LightParameters l_lightDefaults;
static cXyz l_zeroVector;
static cXyz l_ebrockOffset;
static cXyz l_ebrockDynamics;
static cXyz l_ebrockParticle;
static cXyz l_ekaoOffset;
static cXyz l_ekaoDynamics;
static cXyz l_ekaoParticle;
static cXyz l_smallOffset;
static cXyz l_smallDynamics;
static cXyz l_smallParticle;
static u32 l_ebrockOffsetGuard;
static u32 l_ebrockDynamicsGuard;
static u32 l_ebrockParticleGuard;
static u32 l_ekaoOffsetGuard;
static u32 l_ekaoDynamicsGuard;
static u32 l_ekaoParticleGuard;
static u32 l_smallOffsetGuard;
static u32 l_smallDynamicsGuard;
static u32 l_smallParticleGuard;
static u32 PrmAbstract(Act_c*, s32, s32);
static Act_c* construct(Act_c*);
// HD: assertion text names the tww m644 member M_type. Guest indexing still wraps.
static Attr_c* checkedAttr(Act_c* a, const char* file, const char* message) {
    u32 type = a->m644;
    if (type >= Act_c::Type_Max) {
        JUT_ASSERT_fail(file, 0x247, message);
        type = a->m644;
    }
    return stone2::attributeAt(&M_attr, type);
}
static dComIfG_inf_c* play() {
    return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
}
static u32 prm(Act_c* a, s32 width, s32 shift) {
    return PrmAbstract(a, width, shift);
}
/* 0249E2D0 */
void daStone2::Act_c::prmZ_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m656)
        return;
    s16 angle = home.angle.z;
    m656 = 1;
    home.angle.z = 0;
    mPrmZ = (u16)angle;
    current.angle.z = 0;
    shape_angle.z = 0;
}
/* 0249E300 */
BOOL daStone2::Act_c::is_switch() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 sw = prm(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (sw == 255)
        return FALSE;
    s8 room = home.roomNo;
    dSv_info_c* save = dComIfGs_info();
    return stoneIsSwitch(save, sw, room) != 0;
}
/* 0249EE54 */
void daStone2::Act_c::on_switch() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 sw = prm(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (sw != 255) {
        dSv_info_c* save = dComIfGs_info();
        s8 room = home.roomNo;
        stoneOnSwitch(save, sw, room);
    }
}
/* 0249E5B8 */
BOOL daStone2::Act_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
    u32 type = m644;
    const char* name = attr->resName;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
        attr = stone2::attributeAt(&M_attr, type);
    }
    s16 index = attr->m04;
    dRes_control_c* manager = dComIfG_resControl();
    ProtectedLocal<StoneSafeString> key;
    key->name = gabi::ea(name);
    key->vtable = gc_tu.safeString;
    J3DModelData* data = stoneObjectResource(manager, key.get(), index);
    if (!data)
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x256, "model_data != NULL");
    mpModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    return mpModel != nullptr;
}
/* 0249E8BC */
BOOL daStone2::Act_c::Create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());

    mStts.Init(255, 255, this);
    mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&M_cyl_src));
    u32 type = m644;
    mCyl.mStts = &mStts;
    auto attrFor = [&](u32 t) {
        if (t >= Type_Max) {
            JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
            t = m644;
        }
        return stone2::attributeAt(&M_attr, t);
    };
    Attr_c* attr = attrFor(type);
    f32 radius = attr->m12;
    mCyl.mCyl.SetR(radius);
    attr = attrFor(m644);
    f32 height = attr->m14;
    mCyl.mCyl.SetH(height);
    stone2::words(&mCyl.mGObjAt.mVec)->x = stone2::words(&l_zeroVector)->x;
    stone2::words(&mCyl.mGObjAt.mVec)->y = stone2::words(&l_zeroVector)->y;
    stone2::words(&mCyl.mGObjAt.mVec)->z = stone2::words(&l_zeroVector)->z;
    stone2::words(&mCyl.mGObjTg.mVec)->x = stone2::words(&l_zeroVector)->x;
    stone2::words(&mCyl.mGObjTg.mVec)->y = stone2::words(&l_zeroVector)->y;
    type = m644;
    stone2::words(&mCyl.mGObjTg.mVec)->z = stone2::words(&l_zeroVector)->z;
    attr = attrFor(type);
    radius = attr->m12;
    f32 wall = (f32)l_wallHeight;
    mAcchCir.SetWall(wall, radius);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    u32 flags = mAcch.m_flags;
    type = m644;
    mAcch.m_flags = flags & ~8u;
    attr = attrFor(type);
    f32 roof = attr->m10;
    type = m644;
    mAcch.m_roof_crr_height = roof;
    attr = attrFor(type);
    gravity = attr->m30;
    stoneMovePosition(this, 0);
    dComIfG_inf_c* p = play();
    mAcch.CrrPos(&p->play.mBgS);
    u32 attention = attention_info.flags;
    f32 x = home.pos.x, y = home.pos.y;
    current.pos.x = x;
    current.pos.y = y;
    flags = mAcch.m_flags;
    type = m644;
    attention_info.flags = attention | 0x10;
    f32 z = home.pos.z;
    mAcch.m_flags = flags & ~0x80u;
    current.pos.z = z;
    attr = attrFor(type);
    u32 status = actor_status;
    x = current.pos.x;
    type = m644;
    u8 distance = attr->m0C;
    actor_status = status | 0x10000;
    attention_info.distances[4] = distance;
    attention_info.position.x = x;
    attr = attrFor(type);
    y = current.pos.y;
    z = current.pos.z;
    f32 offset = attr->m38;
    m6A8 = 0;
    m658 = 0;
    m657 = 1;
    attention_info.position.y = y + offset;
    attention_info.position.z = z;
    m6A9 = 0;
    m65A = 2;
    mode_wait_init();
    u8 id = (u8)(u16)mPrmZ;
    p = play();
    m650 = stoneEventIdx(&p->play.mEvtManager, 0, id);
    demo_non_init();
    J3DModel* model = mpModel;
    cullMtx = stone2::cullMatrixAddress(model);
    init_mtx();
    return TRUE;
}
using Color4 = GXColor;
/* 0249E378 */
u8 daStone2::Act_c::chk_appear() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return (u8)(is_switch() ^ 1);
}
/* 0249E3A0 */
BOOL daStone2::Act_c::Mthd_Create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 flags = actor_condition;
    if (!(flags & 8)) {
        if (this) {
            construct(this);
            flags = actor_condition;
        }
        actor_condition = flags | 8;
    }
    prmZ_init();
    m644 = prm(this, PRM_TYPE_W, PRM_TYPE_S);
    u8 appears = chk_appear();
    m659 = appears;
    s32 phase = 5;
    if (appears) {
        Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
        const char* name = attr->resName;
        phase = stoneResLoad(&mPhase, name);
        if (phase == cPhs_COMPLEATE_e) {
            attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
            u32 type = m644;
            Attr_c *first = attr, *second;
            if (type >= Type_Max) {
                JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
                type = m644;
                attr = stone2::attributeAt(&M_attr, type);
            }
            second = attr;
            if (type >= Type_Max) {
                JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
                type = m644;
                attr = stone2::attributeAt(&M_attr, type);
            }
            u32 heap = attr->heapSize;
            name = first->resName;
            s16 index = second->resIndex;
            phase = stoneMoveBGCreate(this, name, index, 0, heap);
            if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
                JUT_ASSERT_fail("d_a_stone2.cpp", 0x2C4,
                                "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
        }
    }
    return phase;
}
/* 0249F968 */
BOOL daStone2::Act_c::Delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    m65C.remove();
    eff_lift_smoke_remove();
    return TRUE;
}
/* 0249E528 */
BOOL daStone2::Act_c::Mthd_Delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    BOOL result = TRUE;
    if (m659) {
        result = stoneMoveBGDelete(this);
        Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
        const char* name = attr->resName;
        stoneResDelete(&mPhase, name);
    }
    return result;
}
/* 0249E6DC */
void daStone2::Act_c::set_mtx() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    bool dropping = m648 == Mode_Drop;
    f32 x = current.pos.x, y, z;
    if (dropping) {
        Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
        y = current.pos.y;
        f32 height = attr->m34;
        z = current.pos.z;
        y = y + height;
    } else {
        z = current.pos.z;
        y = current.pos.y;
    }
    auto matrix = mDoMtx_stack_c::get();
    PSMTXTrans(matrix, x, y, z);
    s16 rx = shape_angle.x, ry = shape_angle.y, rz = shape_angle.z;
    stoneRotateZXY(matrix, rx, ry, rz);
    if (dropping) {
        Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
        f32 zero = (f32)l_zero, height = attr->m34;
        stoneTranslateMatrix(zero, -height, zero);
    }
    // HD: preserve the GHS float-copy store order, including NaN quieting.
    f32 values[12];
    for (u32 i = 0; i < 12; i++)
        values[i] = stone2::matrixFloats(mDoMtx_stack_c::get())[i];
    J3DModel* model = mpModel;
    const u8 order[] = {1, 7, 6, 5, 3, 9, 10, 8, 0, 11, 2, 4};
    for (u8 i : order)
        stone2::modelMatrixFloats(model)[i] = values[i];
    PSMTXCopy(matrix, &M_tmp_mtx);
}
/* 0249E89C */
void daStone2::Act_c::init_mtx() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* model = mpModel;
    f32 y = scale.y, x = scale.x, z = scale.z;
    stone2::modelScale(model)->x = x;
    stone2::modelScale(model)->y = y;
    stone2::modelScale(model)->z = z;
    set_mtx();
}
/* 0249F57C */
void daStone2::Act_c::cull_set_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Attr_c* first = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
    u32 type = m644;
    Attr_c *second = first, *third, *fourth;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
        second = stone2::attributeAt(&M_attr, type);
    }
    third = second;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
        third = stone2::attributeAt(&M_attr, type);
    }
    fourth = third;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
        fourth = stone2::attributeAt(&M_attr, type);
    }
    f32 x = first->m28, y = second->m2A, z = third->m2C, radius = fourth->m2E;
    stoneCullSphere(this, x, y, z, radius);
}
/* 0249EC74 */
void daStone2::Act_c::cull_set_move() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
    u32 type = m644;
    if (attr->m26 < 0) {
        m657 = 1;
        return;
    }
    Attr_c* slots[4];
    for (u32 i = 0; i < 4; i++) {
        if (type >= Type_Max) {
            JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
            type = m644;
            attr = stone2::attributeAt(&M_attr, type);
        }
        slots[i] = attr;
    }
    f32 x = slots[0]->m20, y = slots[1]->m22, z = slots[2]->m24, radius = slots[3]->m26;
    stoneCullSphere(this, x, y, z, radius);
}
/* 0249EEAC */
void daStone2::Act_c::damaged(s32 cause) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 item = prm(this, PRM_ITEMNO_W, PRM_ITEMNO_S), saved = prm(this, PRM_ITEMSAVE_W, PRM_ITEMSAVE_S);
    s16 yaw = home.angle.y;
    ProtectedLocal<csXyz> angle;
    csXyz_ct(angle.get(), 0, yaw, 0);
    s8 room = home.roomNo;
    fopAcM_createItemFromTable(&current.pos, item, saved, room, 0, angle.get(), cause, 0);
    fopAcM_cancelCarryNow(this);
    (this->*stone2::procedureAt(&eff_break_proc, (u32)(s32)m644))();
    Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
    if (attr->m50 > 0) {
        dComIfG_inf_c* p = play();
        dVibration_c* vibration = &p->play.mVibration;
        attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
        f32 zero = (f32)l_zero, one = (f32)l_one;
        ProtectedLocal<cXyz> direction;
        direction->x = zero;
        direction->y = one;
        direction->z = zero;
        s16 strength = attr->m50;
        stoneShock(vibration, strength, -33, direction.get());
    }
    set_senv(255, 10);
    attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
    room = current.roomNo;
    u32 sound = attr->m18;
    s8 reverb = stoneReverb(room);
    mDoAud_seStart(sound, &eyePos, 0, reverb);
    on_switch();
}

/* 0249F0BC */
BOOL daStone2::Act_c::damage_cc_proc() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    BOOL wasDamaged = FALSE;
    if (mCyl.ChkAtHit()) {
        mCyl.ClrAtHit();
        damaged(3);
        wasDamaged = TRUE;
    } else if (mCyl.ChkTgHit()) {
        dCcD_GObjInf* hit = static_cast<dCcD_GObjInf*>(mCyl.GetTgHitObj());
        if (hit && m644 != 3 && (hit->mAtType & 0x20)) {
            damaged(7);
            wasDamaged = TRUE;
        }
        mCyl.ClrTgHit();
    }
    return wasDamaged;
}
/* 0249F174 */
BOOL daStone2::Act_c::damage_bg_proc() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return FALSE;
}
/* 0249F32C */
BOOL daStone2::Act_c::damage_bg_proc_directly() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m648 != Mode_Drop)
        return FALSE;
    u32 flags = mAcch.m_flags;
    bool ground = flags & 0x20, wall = flags & 0x10, roof = flags & 0x200;
    if (ground)
        damaged(2);
    else if (wall || roof)
        damaged(3);
    else
        return FALSE;
    cam_lockoff();
    return TRUE;
}
/* 024A018C */
void daStone2::Act_c::eff_m_break(u16 effect, u16 frame) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    ProtectedLocal<StoneSafeString> first;
    first->vtable = gc_tu.safeString;
    first->name = gabi::guest_addr("Always");
    dRes_control_c* manager = dComIfG_resControl();
    J3DModelData* modelData = stoneObjectResource(manager, first.get(), 0x30);
    ProtectedLocal<StoneSafeString> second;
    second->vtable = gc_tu.safeString;
    manager = dComIfG_resControl();
    second->name = gabi::guest_addr("Always");
    J3DModelData* animation = stoneObjectResource(manager, second.get(), 0x66);
    Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
    f32 scaleValue = attr->m54;
    ProtectedLocal<cXyz> scale;
    scale->x = scaleValue;
    scale->y = scaleValue;
    scale->z = scaleValue;
    dComIfG_inf_c* p = play();
    dPa_control_c* particles = p->play.mParticle;
    stone2::Emitter* emitter =
        stoneSetParticle(particles, 0, effect, &current.pos, &shape_angle, 0, 255, 0, -1, 0, 0, scale.get());
    if (emitter) {
        J3DModel* model = mpModel;
        stoneEmitterMatrix(model ? &model->mBaseTransformMtx : nullptr, &emitter->mGlobalRotation,
                           &emitter->mGlobalTranslation);
        stone2::ModelEmitter* object =
            stoneCreateModelEmitter(0, emitter, modelData, 1, &tevStr, animation, frame, 0);
        if (object) {
            p = play();
            particles = p->play.mParticle;
            stone2::EmitterList* list = stone2::modelEmitterList(particles);
            stoneAppendModelEmitter(list, object);
        }
    }
}
static void breakSmoke(Act_c* a, u16 modelEffect, u16 burstEffect, u16 frame, cXyz* offset, be<u32>* guard,
                       const be<f32>* heightConst, cXyz* dynamics, be<u32>* dynamicsGuard,
                       const be<f32>* dynamicsConst, cXyz* particle, be<u32>* particleGuard,
                       const be<f32>* particleConst) {
    a->eff_m_break(modelEffect, frame);
    a->eff_b_break(burstEffect);
    if (!*guard) {
        f32 zero = l_zero, height = *heightConst;
        *guard = 1;
        offset->z = zero;
        offset->x = zero;
        offset->y = height;
    }
    J3DModel* model = a->mpModel;
    PSMTXCopy(model ? &model->mBaseTransformMtx : nullptr, mDoMtx_stack_c::get());
    stoneTransformVector(mDoMtx_stack_c::get(), offset, &a->m67C);
    dComIfG_inf_c* p = play();
    dPa_control_c* manager = p->play.mParticle;
    stone2::Emitter* emitter =
        stoneSetParticle(manager, 2, 0x2027, &a->m67C, 0, 0, 200, &a->m65C, -1, 0, 0, 0);
    if (!emitter)
        return;
    f32 rate = l_wallHeight;
    emitter->mMaxFrame = 1;
    emitter->mRate = rate;
    u32 dynamicsReady = *dynamicsGuard, particleReady;
    f32 dx;
    if (dynamicsReady) {
        particleReady = *particleGuard;
        dx = dynamics->x;
    } else {
        dx = *dynamicsConst;
        particleReady = *particleGuard;
        *dynamicsGuard = 1;
        dynamics->z = dx;
        dynamics->y = dx;
        dynamics->x = dx;
    }
    if (!particleReady) {
        f32 size = *particleConst;
        *particleGuard = 1;
        particle->x = size;
        particle->z = size;
        particle->y = size;
    }
    emitter->mGlobalDynamicsScale.x = dx;
    emitter->mGlobalDynamicsScale.y = dynamics->y;
    emitter->mGlobalDynamicsScale.z = dynamics->z;
    emitter->mGlobalParticleScale.x = particle->x;
    emitter->mGlobalParticleScale.y = particle->y;
    emitter->mGlobalParticleScale.z = particle->z;
}
/* 024A02F0 */
void daStone2::Act_c::eff_b_break(u16 effect) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    ProtectedLocal<Color4> color;
    color->b = (u8)tevStr.mColorC0.b;
    color->g = (u8)tevStr.mColorC0.g;
    u32 type = m644;
    color->r = (u8)tevStr.mColorC0.r;
    color->a = (u8)tevStr.mColorC0.a;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
    }
    u32 scaleBits = stone2::attributeFloatBits(stone2::attributeAt(&M_attr, type), &Attr_c::m58);
    f32 value = gabi::f32_from_bits(scaleBits);
    ProtectedLocal<cXyz> scale;
    // HD: the first scale store preserves SNaN bits; reused FPR stores are quiet.
    stone2::words(scale.get())->x = scaleBits;
    scale->y = value;
    scale->z = value;
    dComIfG_inf_c* p = play();
    dPa_control_c* manager = p->play.mParticle;
    stone2::Emitter* emitter = stoneSetParticle(manager, 0, effect, &current.pos, 0, 0, 255, 0, -1,
                                                &tevStr.mColorK0, color.get(), scale.get());
    if (emitter) {
        J3DModel* model = mpModel;
        stoneEmitterMatrix(model ? &model->mBaseTransformMtx : nullptr, &emitter->mGlobalRotation,
                           &emitter->mGlobalTranslation);
    }
}
/* 024A03F4 */
void daStone2::Act_c::eff_break_ebrock() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    breakSmoke(this, 0x3F4, 0x3F3, 2, &l_ebrockOffset, &l_ebrockOffsetGuard, &l_smokeConstants.high,
               &l_ebrockDynamics, &l_ebrockDynamicsGuard, &l_smokeConstants.dynamicsLarge, &l_ebrockParticle,
               &l_ebrockParticleGuard, &l_smokeConstants.particleLarge);
}
/* 024A05D4 */
void daStone2::Act_c::eff_break_ekao() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    breakSmoke(this, 0x3F8, 0x3F7, 3, &l_ekaoOffset, &l_ekaoOffsetGuard, &l_smokeConstants.high,
               &l_ekaoDynamics, &l_ekaoDynamicsGuard, &l_smokeConstants.dynamicsFace, &l_ekaoParticle,
               &l_ekaoParticleGuard, &l_smokeConstants.particleFace);
}
/* 024A07B4 */
void daStone2::Act_c::eff_break_ebrock2() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    breakSmoke(this, 0x3F6, 0x3F5, 2, &l_smallOffset, &l_smallOffsetGuard, &l_smokeConstants.low,
               &l_smallDynamics, &l_smallDynamicsGuard, &l_smokeConstants.dynamicsSmall, &l_smallParticle,
               &l_smallParticleGuard, &l_smokeConstants.dynamicsLarge);
}
/* 0249FA68 */
void daStone2::Act_c::eff_lift_smoke_start() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m6A9)
        return;
    m6A8 = 1;
    m6A9 = 1;
    // HD: dKy_tevstr_init clears 0x72 words, matching the HD stack slot's 0x1C8 bytes.
    ProtectedLocal<dKy_tevstr_c> tev;
    // HD: default light objects occupy three parts of the enlarged lighting temporary.
    for (u32 start : {0u, 0xC0u, 0x144u}) {
        auto block = stone2::lightBlock(tev.get(), start);
        block->position.x = l_lightDefaults.position.x;
        block->position.y = l_lightDefaults.position.y;
        block->position.z = l_lightDefaults.position.z;
        block->direction.x = l_lightDefaults.direction.x;
        block->direction.y = l_lightDefaults.direction.y;
        block->direction.z = l_lightDefaults.direction.z;
        block->color.r = l_lightDefaults.color.r;
        block->color.g = l_lightDefaults.color.g;
        block->color.b = l_lightDefaults.color.b;
        block->color.a = l_lightDefaults.color.a;
        block->materialColor.r = l_lightDefaults.materialColor.r;
        block->materialColor.g = l_lightDefaults.materialColor.g;
        block->materialColor.b = l_lightDefaults.materialColor.b;
        block->materialColor.a = l_lightDefaults.materialColor.a;
        for (u32 i = 0; i < 8; i++)
            block->parameters[i] = l_lightDefaults.parameters[i];
    }
    f32 ground = mAcch.m_ground_h, invalid = (f32)l_invalidGround;
    u8 poly = 255;
    if (ground != invalid) {
        dComIfG_inf_c* p = play();
        poly = stonePolyColor(&p->play.mBgS, static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
    }
    s8 room = current.roomNo;
    stoneTevInit(tev.get(), room, poly);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, tev.get());
    ProtectedLocal<Color4> color;
    color->r = (u8)tev->mColorC0.r;
    color->b = (u8)tev->mColorC0.b;
    u32 type = m644;
    color->a = (u8)tev->mColorC0.a;
    color->g = (u8)tev->mColorC0.g;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
    }
    f32 scaleValue = stone2::attributeAt(&M_attr, type)->m5C;
    ProtectedLocal<cXyz> scale;
    scale->x = scaleValue;
    scale->z = scaleValue;
    scale->y = scaleValue;
    dComIfG_inf_c* p = play();
    dPa_control_c* manager = p->play.mParticle;
    stoneSetParticle(manager, 2, 0x23F2, &current.pos, 0, scale.get(), 128, &m688, -1, color.get(),
                     &tev->mColorK0, 0);
}
/* 0249F17C */
void daStone2::Act_c::eff_lift_smoke_end() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m6A9) {
        m6A9 = 0;
        m688.end();
    }
}
/* 0249F958 */
void daStone2::Act_c::eff_lift_smoke_remove() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    m688.remove();
}
/* 0249EE10 */
void daStone2::Act_c::set_senv(s32 sound, s32 radius) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    ProtectedLocal<cXyz> position;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    position->x = x;
    position->y = y;
    position->z = z;
    u32 id = mBsPcId;
    stoneSoundEnvironment(position.get(), sound, id, radius);
}
/* 0249F2F0 */
void daStone2::Act_c::cam_lockoff() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* p = play();
    stone2::Camera* camera = stone2::cameraAt((u32)p->play.mCameraInfo[0].mpCamera);
    u32 id = mBsPcId;
    stoneCameraLockOff(stone2::cameraBody(camera), id);
}
/* 0249E6A0 */
void daStone2::Act_c::mode_wait_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 at = mCyl.mObjAt.mSPrm, co = mCyl.mObjCo.mSPrm, tg = mCyl.mObjTg.mSPrm;
    mCyl.mObjAt.mSPrm = at & ~1u;
    mCyl.mObjCo.mSPrm = co & ~1u;
    m648 = Mode_Wait;
    mCyl.mObjTg.mSPrm = tg | 1u;
}
/* 0249FA28 */
void daStone2::Act_c::mode_wait() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (actor_status & 0x2000) {
        mode_carry_init();
        return;
    }
    u8 delay = m65A;
    if (delay) {
        delay = (u8)(delay - 1);
        m65A = delay;
        if (!delay) {
            stone2::Background* bg = mpBgW;
            u8 flags = bg->mFlags;
            bg->mFlags = flags | 0x80;
        }
    }
}
/* 0249F9B0 */
void daStone2::Act_c::mode_carry_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 co = mCyl.mObjCo.mSPrm, tg = mCyl.mObjTg.mSPrm, at = mCyl.mObjAt.mSPrm;
    mCyl.mObjCo.mSPrm = co & ~1u;
    u32 attention = attention_info.flags;
    mCyl.mObjAt.mSPrm = at & ~1u;
    mCyl.mObjTg.mSPrm = tg | 1u;
    attention_info.flags = attention & ~0x10u;
    dComIfG_inf_c* p = play();
    stone2::Background* bg = mpBgW;
    stoneReleaseBackground(&p->play.mBgS, bg);
    on_switch();
    m648 = Mode_Carry;
}
/* 0249FF48 */
void daStone2::Act_c::mode_carry() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* p = play();
    stone2::Player* player = reinterpret_cast<stone2::Player*>((fopAc_ac_c*)p->play.mPlayerInfo[0].mpPlayer);
    if (player->mGrabFlags & 0x8000)
        m658 = 1;
    p = play();
    player = reinterpret_cast<stone2::Player*>((fopAc_ac_c*)p->play.mPlayerInfo[0].mpPlayer);
    if (player->mGrabFlags & 0x8000)
        eff_lift_smoke_start();
    p = play();
    player = reinterpret_cast<stone2::Player*>((fopAc_ac_c*)p->play.mPlayerInfo[0].mpPlayer);
    if (player->mGrabFlags & 0x20)
        eff_lift_smoke_end();
    if (!(actor_status & 0x2000)) {
        mode_drop_init();
        mode_drop();
    }
}
/* 0249FD5C */
void daStone2::Act_c::mode_drop_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Attr_c* attr = checkedAttr(this, "d_a_stone2.cpp", "m644 >= 0 && m644 < Type_Max");
    u32 at = mCyl.mObjAt.mSPrm, type = m644, co = mCyl.mObjCo.mSPrm, tg = mCyl.mObjTg.mSPrm;
    f32 forward = attr->m40;
    mCyl.mObjCo.mSPrm = co | 1u;
    mCyl.mObjAt.mSPrm = at | 1u;
    mCyl.mObjTg.mSPrm = tg | 1u;
    mStts.m_weight = 200;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
    }
    attr = stone2::attributeAt(&M_attr, type);
    type = m644;
    f32 vertical = attr->m3C;
    speedF = forward;
    speed.y = vertical;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
    }
    gravity = stone2::attributeAt(&M_attr, type)->m30;
    demo_req_init();
    m648 = Mode_Drop;
}
/* 0249FE90 */
void daStone2::Act_c::mode_drop() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    stoneSetRoom(this, &mAcch.m_gnd);
    u32 type = m644;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
    }
    Attr_c *first = stone2::attributeAt(&M_attr, type), *second = first;
    if (type >= Type_Max) {
        JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
        type = m644;
        second = stone2::attributeAt(&M_attr, type);
    }
    f32 stream = first->m44, damping = second->m48;
    stonePosMoveStream(this, &mStts, &l_zeroVector, stream, damping);
    shape_angle.x = (s16)((s16)shape_angle.x + 1000);
}
/* 0249F19C */
void daStone2::Act_c::mode_fine_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    stone2::Background* bg = mpBgW;
    u32 at = mCyl.mObjAt.mSPrm, tg = mCyl.mObjTg.mSPrm, co = mCyl.mObjCo.mSPrm;
    mCyl.mObjAt.mSPrm = at & ~1u;
    mCyl.mObjTg.mSPrm = tg & ~1u;
    mCyl.mObjCo.mSPrm = co & ~1u;
    if (bg->m_id < 0x100) {
        dComIfG_inf_c* p = play();
        bg = mpBgW;
        stoneReleaseBackground(&p->play.mBgS, bg);
    }
    if (actor_status & 0x2000)
        fopAcM_cancelCarryNow(this);
    u32 attention = attention_info.flags;
    m654 = 1;
    m648 = Mode_Fine;
    attention_info.flags = attention & ~0x10u;
}
/* 0249FFE0 */
void daStone2::Act_c::mode_fine() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s16 timer = m654;
    if (timer > 0) {
        timer = (s16)(timer - 1);
        m654 = timer;
        if (!timer && m644 == 3) {
            s16 event = m650;
            dComIfG_inf_c* p = play();
            if (!stoneEventExists(&p->play.mEvtManager, event))
                stoneStartSystemSound(0x806);
        }
    }
}
/* 0249F3D0 */
BOOL daStone2::Act_c::mode_proc_call() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    (this->*stone2::procedureAt(&mode_proc, (u32)(s32)m648))();
    s32 mode = m648;
    if (!mode)
        return TRUE;
    if (mode != Mode_Fine) {
        f32 y = current.pos.y, z = current.pos.z, x = current.pos.x;
        dComIfG_inf_c* p = play();
        mAcch.CrrPos(&p->play.mBgS);
        if (m648 == Mode_Carry) {
            current.pos.y = y;
            current.pos.x = x;
            current.pos.z = z;
        }
    }
    if (!damage_bg_proc_directly()) {
        if (m648 != Mode_Carry) {
            u8 room = current.roomNo;
            tevStr.mRoomNo = room;
            dComIfG_inf_c* p = play();
            u8 color = stonePolyColor(&p->play.mBgS, static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
            tevStr.mEnvrIdxOverride = color;
        }
    } else
        mode_fine_init();
    if (m648 == Mode_Fine && m64C == Demo_Non && (m65C.m10 & 1)) {
        if (!m6A8 || (m688.m10 & 1))
            return FALSE;
    }
    return TRUE;
}
/* 0249E6D0 */
void daStone2::Act_c::demo_non_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    m64C = Demo_Non;
}
/* 024A0B10 */
void daStone2::Act_c::demo_non() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}
// daObj::PrmAbstract<daStone2::Act_c::Prm_e>(actor, width, shift): bit field of fopAcM_GetParam
/* 0249F238 */
void daStone2::Act_c::demo_req_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m64C == Demo_Non) {
        u8 id = (u8)(u16)mPrmZ;
        s16 event = m650;
        stoneOrderEvent(this, event, id, 65535, 0, 1);
        u16 flags = eventInfo.mCondition;
        m64C = Demo_Req;
        eventInfo.mCondition = flags | 2;
    }
}
/* 024A0060 */
void daStone2::Act_c::demo_req() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s16 event = m650;
    dComIfG_inf_c* p = play();
    if (!stoneEventExists(&p->play.mEvtManager, event)) {
        demo_non_init();
        return;
    }
    if ((u16)eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        demo_run_init();
        return;
    }
    u8 id = (u8)(u16)mPrmZ;
    event = m650;
    stoneOrderEvent(this, event, id, 65535, 0, 1);
    u16 flags = (u16)eventInfo.mCondition;
    eventInfo.mCondition = flags | 2;
}
/* 024A0054 */
void daStone2::Act_c::demo_run_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    m64C = Demo_Run;
}
/* 024A0128 */
void daStone2::Act_c::demo_run() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s16 event = m650;
    dComIfG_inf_c* p = play();
    if (stoneEventEnd(&p->play.mEvtManager, event)) {
        p = play();
        u16 flags = p->play.mEvtCtrl.mEventFlag;
        p->play.mEvtCtrl.mEventFlag = flags | 8;
        demo_non_init();
    }
}
/* 0249F2A4 */
void daStone2::Act_c::demo_proc_call() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    (this->*stone2::procedureAt(&demo_proc, (u32)(s32)m64C))();
}
/* 0249F6CC */
BOOL daStone2::Act_c::Execute(gptr<Mtx34>* output) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    cull_set_move();
    bool active = m657 || m648 != Mode_Wait;
    // HD: the actor-condition bit bypasses the culling call.
    if (!active) {
        u32 cull = prm(this, PRM_CULL_W, PRM_CULL_S);
        if (!cull || !(actor_condition & 4))
            active = true;
        else
            active = !stoneCullingCheck(this);
    }
    if (active) {
        s32 mode = m648;
        m657 = 0;
        if (mode != Mode_Fine && (damage_cc_proc() || damage_bg_proc())) {
            eff_lift_smoke_end();
            mode_fine_init();
            demo_req_init();
        }
        demo_proc_call();
        if (mode_proc_call()) {
            if (m648 != Mode_Fine) {
                u8 room = current.roomNo;
                mStts.mRoomId = room;
                stoneMoveCollision(&mCyl, &current.pos);
                dComIfG_inf_c* p = play();
                stoneRegisterCollision(&p->play.mCcS, &mCyl);
                if (m648 == Mode_Drop) {
                    p = play();
                    stoneSetMass(stone2::massManager(p), &mCyl, 3);
                }
            }
            u32 type = m644;
            f32 x = current.pos.x;
            attention_info.position.x = x;
            if (type >= Type_Max) {
                JUT_ASSERT_fail("d_a_stone2.cpp", 0x247, "m644 >= 0 && m644 < Type_Max");
                type = m644;
            }
            f32 y = current.pos.y, eyeX = attention_info.position.x,
                height = stone2::attributeAt(&M_attr, type)->m38, z = current.pos.z;
            y = y + height;
            eyePos.x = eyeX;
            attention_info.position.z = z;
            attention_info.position.y = y;
            eyePos.z = z;
            eyePos.y = y;
        } else
            stoneDeleteActor(this);
    }
    set_mtx();
    *output = &M_tmp_mtx;
    cull_set_draw();
    return TRUE;
}
/* 0249F8B4 */
BOOL daStone2::Act_c::Draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m648 != Mode_Fine) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        settingTevStruct(env, 1, &current.pos, &tevStr);
        env = dKy_getEnvlight();
        J3DModel* model = mpModel;
        setLightTevColorType(env, model, &tevStr);
        // HD: no GameCube draw_shadow call remains in this Draw entry.
        dComIfGd_setListBG();
        model = mpModel;
        mDoExt_modelUpdateDL(model, 0);
        dComIfGd_setList();
    }
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 024A0994 */
static BOOL Mthd_Create(Act_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return a->Mthd_Create();
}
/* 024A0998 */
static BOOL Mthd_Delete(Act_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return a->Mthd_Delete();
}
/* 024A099C */
static BOOL Mthd_Execute(Act_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return stoneMoveBGExecute(a);
}
/* 024A09A0 */
static BOOL Mthd_IsDelete(Act_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return stone2::dispatch(a, 0x2C);
}
/* 024A09B0 */
static BOOL Mthd_Draw(Act_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return stone2::dispatch(a, 0x3C);
}
/* 024A0A54 */
static void staticDestructor(void* p, u32 deleting) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (p && (deleting & 1))
        operator_delete(p);
}
/* 0249E1AC */
static Act_c* construct(Act_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!a) {
        a = static_cast<Act_c*>(operator_new(0x7C4));
        if (!a)
            return nullptr;
    }
    stoneMoveBGConstruct(a);

    a->__vtbl = stone2::ActorVtable;
    stoneAcchConstruct(&a->mAcch);
    a->mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    a->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    a->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    a->mAcch.mbObjThrough = 1;
    stoneAcchCirConstruct(&a->mAcchCir);
    stoneSttsConstruct(&a->mStts);
    stoneGSttsConstruct(&a->mStts.__vtbl_gstts);
    a->mStts.__vtbl = stone2::SttsVtable;
    a->mStts.__vtbl_gstts = stone2::GSttsVtable;
    stoneGObjInfConstruct(&a->mCyl);
    stone2::shape(&a->mCyl)->mVtable = stone2::BaseShapeVtable;
    stone2::shape(&a->mCyl)->mParentVtable = stone2::ShapeParentVtable;
    stoneCylConstruct(&a->mCyl.mCyl);
    a->mCyl.__vtbl_hitinf = stone2::HitInfoVtable;
    a->mCyl.mCyl.__vtbl = stone2::CylinderVtable;
    stone2::shape(&a->mCyl)->mVtable = stone2::ShapeVtable;
    stoneSmokeConstruct(&a->m65C, 1);
    stoneSmokeConstruct(&a->m688, 1);
    return a;
}
/* 024A09C0 */
static void staticInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics_rodata(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2],
                                gabi::ea(&l_smokeConstants.piNegative));
}
/* 024A0A68 */
BOOL daStone2::Act_c::IsDelete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}
/* 024A0A70 */
static void safeStringVirtual(void* p) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}
/* 024A0A74 */
static void destructor(Act_c* a, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!a)
        return;

    stoneCylDestroy(&a->mCyl, 2);   // dCcD_Cyl::~dCcD_Cyl
    stoneSttsDestroy(&a->mStts, 2); // dCcD_Stts::~dCcD_Stts
    stoneCirDestroy(&a->mAcchCir.m_cir, 2);
    a->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    a->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    stoneAcchDestroy(&a->mAcch, 0); // dBgS_Acch base destructor
    stoneActorDestroy(a, 0);        // fopAc_ac_c::~fopAc_ac_c
    if (flags & 1)
        operator_delete(a); // operator delete
}
// daStone2::Act_c::demo_non (demo table entry 0): empty
/* 024A0B14 */
static u32 PrmAbstract(Act_c* a, s32 width, s32 shift) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 param = a->mParameters;
    // PPC slw/srw: shift amount is the low 6 bits, 32..63 gives 0
    u32 mask = ((width & 32) ? 0u : (1u << (width & 31))) - 1u;
    u32 field = (shift & 32) ? 0u : (param >> (shift & 31));
    return field & mask;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data M_attr 1003E84C
data demo_proc 1003E630
data mode_proc 1003E648
data eff_break_proc 1003E5D4
data M_cyl_src 1003E808
data M_tmp_mtx 1046E0FC
data l_smokeConstants 1003E7E0
data l_zero 1003E528
data l_one 1003E3C8
data l_wallHeight 1003E568
data l_invalidGround 1003E6D0
data l_lightDefaults 1016E414
data l_zeroVector 101FFBA8
data l_ebrockOffset 1046E06C
data l_ebrockDynamics 1046E078
data l_ebrockParticle 1046E084
data l_ekaoOffset 1046E090
data l_ekaoDynamics 1046E09C
data l_ekaoParticle 1046E0A8
data l_smallOffset 1046E0B4
data l_smallDynamics 1046E0C0
data l_smallParticle 1046E0CC
data l_ebrockOffsetGuard 1046E0D8
data l_ebrockDynamicsGuard 1046E0DC
data l_ebrockParticleGuard 1046E0E0
data l_ekaoOffsetGuard 1046E0E4
data l_ekaoDynamicsGuard 1046E0E8
data l_ekaoParticleGuard 1046E0EC
data l_smallOffsetGuard 1046E0F0
data l_smallDynamicsGuard 1046E0F4
data l_smallParticleGuard 1046E0F8
str 1003E438 "d_a_stone2.cpp" in 0249E3A0
str 1003E448 "m644 >= 0 && m644 < Type_Max" renamed in 0249E3A0
str 1003E4B0 "d_a_stone2.cpp" in 0249E528
str 1003E4C0 "m644 >= 0 && m644 < Type_Max" renamed in 0249E528
str 1003E4F4 "d_a_stone2.cpp" in 0249E5B8
str 1003E504 "m644 >= 0 && m644 < Type_Max" renamed in 0249E5B8
str 1003E52C "d_a_stone2.cpp" in 0249E6DC
str 1003E53C "m644 >= 0 && m644 < Type_Max" renamed in 0249E6DC
str 1003E56C "d_a_stone2.cpp" in 0249E8BC
str 1003E57C "m644 >= 0 && m644 < Type_Max" renamed in 0249E8BC
str 1003E5A0 "d_a_stone2.cpp" in 0249EC74
str 1003E5B0 "m644 >= 0 && m644 < Type_Max" renamed in 0249EC74
str 1003E5FC "d_a_stone2.cpp" in 0249EEAC
str 1003E60C "m644 >= 0 && m644 < Type_Max" renamed in 0249EEAC
str 1003E668 "d_a_stone2.cpp" in 0249F57C
str 1003E678 "m644 >= 0 && m644 < Type_Max" renamed in 0249F57C
str 1003E69C "d_a_stone2.cpp" in 0249F6CC
str 1003E6AC "m644 >= 0 && m644 < Type_Max" renamed in 0249F6CC
str 1003E6D4 "d_a_stone2.cpp" in 0249FA68
str 1003E6E4 "m644 >= 0 && m644 < Type_Max" renamed in 0249FA68
str 1003E708 "d_a_stone2.cpp" in 0249FD5C
str 1003E718 "m644 >= 0 && m644 < Type_Max" renamed in 0249FD5C
str 1003E73C "d_a_stone2.cpp" in 0249FE90
str 1003E74C "m644 >= 0 && m644 < Type_Max" renamed in 0249FE90
str 1003E778 "d_a_stone2.cpp" in 024A018C
str 1003E788 "m644 >= 0 && m644 < Type_Max" renamed in 024A018C
str 1003E7AC "d_a_stone2.cpp" in 024A02F0
str 1003E7BC "m644 >= 0 && m644 < Type_Max" renamed in 024A02F0
str 1003E4E4 "model_data != NULL" renamed
str 1003E46C "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
str 1003E770 "Always"
ext 025BA0C0 u32 stoneIsSwitch(dSv_info_c*, u32, s32)
ext 025B9E38 void stoneOnSwitch(dSv_info_c*, u32, s32)
ext 025440C8 BOOL stoneEventEnd(dEvent_manager_c*, s32)
ext 02544044 u32 stoneEventExists(dEvent_manager_c*, s32)
ext 025D7A58 void stoneOrderEvent(fopAc_ac_c*, s32, u32, u32, u32, u32)
ext 025052BC void stoneCameraLockOff(stone2::CameraBody*, u32)
ext 024F1E9C BOOL stoneMoveBGExecute(Act_c*)
ext 024F1F64 BOOL stoneMoveBGDelete(Act_c*)
ext 024F1D40 void stoneMoveBGConstruct(Act_c*)
ext 024F1D9C s32 stoneMoveBGCreate(Act_c*, const char*, s32, u32, u32)
ext 024F0474 void stoneAcchConstruct(dBgS_ObjAcch*)
ext 024EFE94 void stoneAcchCirConstruct(dBgS_AcchCir*)
ext 0200BD2C void stoneSttsConstruct(dCcD_Stts*)
ext 02515DA0 void stoneGSttsConstruct(be<u32>*)
ext 02515FB8 void stoneGObjInfConstruct(dCcD_GObjInf*)
ext 02018590 void stoneCylConstruct(cM3dGCyl*)
ext 025A5B18 void stoneSmokeConstruct(dPa_smokeEcallBack*, u32)
ext 020087EC void stoneReleaseBackground(dBgS*, stone2::Background*)
ext 025E1988 void stoneStartSystemSound(u32)
ext 02312968 void stoneSetRoom(fopAc_ac_c*, dBgS_GndChk*)
ext 023123C0 void stonePosMoveStream(fopAc_ac_c*, dCcD_Stts*, const cXyz*, f32, f32)
ext 025204C8 void stoneResDelete(request_of_phase_process_class*, const char*)
ext 02520460 s32 stoneResLoad(request_of_phase_process_class*, const char*)
ext 026066C4 J3DModelData* stoneObjectResource(dRes_control_c*, StoneSafeString*, s32)
ext 0255F458 void stoneSoundEnvironment(cXyz*, s32, u32, s32)
ext 025D6768 void stoneCullSphere(fopAc_ac_c*, f32, f32, f32, f32)
ext 025F1B48 void stoneRotateZXY(Mtx34*, s32, s32, s32)
ext 025F24E0 void stoneTranslateMatrix(f32, f32, f32)
ext 024EEEB8 u8 stonePolyColor(dBgS*, cBgS_PolyInfo*)
ext 025D6CE8 u32 stoneCullingCheck(fopAc_ac_c*)
ext 025165A4 void stoneMoveCollision(dCcD_Cyl*, cXyz*)
ext 0200E240 void stoneRegisterCollision(cCcS*, dCcD_Cyl*)
ext 02516C14 void stoneSetMass(void*, dCcD_Cyl*, u32)
ext 025D57E0 void stoneDeleteActor(fopAc_ac_c*)
ext 025D6870 void stoneMovePosition(fopAc_ac_c*, cXyz*)
ext 02543F10 s16 stoneEventIdx(dEvent_manager_c*, const char*, u32)
ext 025A847C stone2::Emitter* stoneSetParticle(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*, u32, dPa_smokeEcallBack*, s32, const GXColor*, const GXColor*, const cXyz*)
ext 028249B0 void stoneEmitterMatrix(const Mtx34*, Mtx34*, cXyz*)
ext 025A3BDC stone2::ModelEmitter* stoneCreateModelEmitter(void*, stone2::Emitter*, J3DModelData*, u32, dKy_tevstr_c*, J3DModelData*, u32, u32)
ext 0200FE78 void stoneAppendModelEmitter(stone2::EmitterList*, stone2::ModelEmitter*)
ext 028E8F64 void stoneTransformVector(Mtx34*, cXyz*, cXyz*)
ext 025CB374 void stoneShock(dVibration_c*, s32, s32, cXyz*)
ext 02520540 s8 stoneReverb(s32)
ext 0255FFF4 void stoneTevInit(dKy_tevstr_c*, s32, u32)
ext 02515A70 void stoneCylDestroy(dCcD_Cyl*, s32)
ext 02515860 void stoneSttsDestroy(dCcD_Stts*, s32)
ext 02018034 void stoneCirDestroy(cM3dGCir_l*, s32)
ext 024EFD9C void stoneAcchDestroy(dBgS_ObjAcch*, u32)
ext 025D50BC void stoneActorDestroy(fopAc_ac_c*, u32)
*/
// clang-format on
