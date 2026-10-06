// Barrel actor: GameCube behavior adapted to the original HD functions.
#include "d/actor/d_a_obj_barrel.h"
using daObjBarrel::Act_c;
static void collision_mode(Act_c *a, bool carry = false) {
    a->mCyl.mObjAt.mSPrm &= ~1u;
    a->mCyl.mObjTg.mSPrm |= 1u;
    if (carry)
        a->mCyl.mObjCo.mSPrm &= ~1u;
    else
        a->mCyl.mObjCo.mSPrm |= 1u;
}
static void clear_carry_attention(Act_c *a) { *gabi::at<be<u32>>(gabi::ea(a) + 0x39C) &= ~0x10u; }
static bool create_heap(Act_c *a) {
    WWHD_FUNC(0x0231C2AC, bool, a);
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = 0x100256E4;
    (*name)[1] = 0x10025558;
    auto *model = gabi::call<J3DModelData *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                                             name.get(), 3);
    if (!model)
        gabi::call(0x0273AA24, STR(0x100255D0), 0x17A, STR(0x100255C0));
    a->mpModel = mDoExt_J3DModel__create(model, 0x80000, 0x11000022);
    return a->mpModel != nullptr;
}
VERIFY(0x0231C2AC, create_heap);
static bool solid_heap(Act_c *a) {
    WWHD_FUNC(0x0231C35C, bool, a);
    return create_heap(a);
}
VERIFY(0x0231C35C, solid_heap);
static void cull_set_draw(Act_c *a) {
    WWHD_FUNC(0x0231C360, void, a);
    gabi::call(0x025D6768, a, 0.f, 50.f, 0.f, 75.f);
}
VERIFY(0x0231C360, cull_set_draw);
static void cull_set_move(Act_c *a) {
    WWHD_FUNC(0x0231CA9C, void, a);
    gabi::call(0x025D6768, a, 0.f, 50.f, 0.f, 300.f);
}
VERIFY(0x0231CA9C, cull_set_move);
static void set_mtx(Act_c *a) {
    WWHD_FUNC(0x0231C380, void, a);
    auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
    gabi::call(0x028E93CC, matrix, (f32)a->current.pos.x, (f32)a->current.pos.y,
               (f32)a->current.pos.z);
    u32 mode = a->mMode;
    if (mode <= 6) {
        bool vibrate = mode >= 2 && mode <= 4;
        f32 height =
            vibrate
                ? 50.f
                : gabi::fmadds(5.f, gabi::load<f32>(0x104A44FC + ((u16)a->shape_angle.z >> 3) * 8),
                               45.f);
        gabi::call(0x025F24E0, 0.f, height, 0.f);
        gabi::call(0x025F1B48, matrix, 0, (s16)a->shape_angle.y, (s16)a->shape_angle.z);
        gabi::call(0x025F1C28, matrix, (s16)a->mRotationAngle);
        gabi::call(0x025F1BF4, matrix, (s16)a->mVibrationAngle);
        gabi::call(0x025F1C28, matrix, (s16) - (s16)a->mRotationAngle);
        if (!vibrate)
            gabi::call(0x025F1C28, matrix, (s16)a->mWalkAngle);
        gabi::call(0x025F24E0, 0.f, -50.f, 0.f);
    }
    u32 model = gabi::ea((J3DModel *)a->mpModel);
    for (u32 i = 0; i < 12; i++)
        gabi::store<f32>(model + 0xC8 + i * 4, gabi::load<f32>(0x1048D0CC + i * 4));
}
VERIFY(0x0231C380, set_mtx);
static void init_mtx(Act_c *a) {
    WWHD_FUNC(0x0231C5D4, void, a);
    u32 model = gabi::ea((J3DModel *)a->mpModel);
    gabi::store<f32>(model + 0xBC, a->scale.x);
    gabi::store<f32>(model + 0xC0, a->scale.y);
    gabi::store<f32>(model + 0xC4, a->scale.z);
    set_mtx(a);
}
VERIFY(0x0231C5D4, init_mtx);
static void mode_wait_init(Act_c *a) {
    WWHD_FUNC(0x0231C5F4, void, a);
    collision_mode(a);
    a->speedF = 0.f;
    a->gravity = -6.f;
    a->mStts.Init(200, 255, a);
    a->mMode = 0;
}
VERIFY(0x0231C5F4, mode_wait_init);
static bool actor_delete(Act_c *a) {
    WWHD_FUNC(0x0231CA6C, bool, a);
    dComIfG_resDelete(&a->mPhase, STR(0x100256E4));
    return true;
}
VERIFY(0x0231CA6C, actor_delete);
static u32 get_se_map_hit(Act_c *a) {
    WWHD_FUNC(0x0231CABC, u32, a);
    return 11;
}
VERIFY(0x0231CABC, get_se_map_hit);
static void set_senv(Act_c *a, s32 p1, s32 p2) {
    WWHD_FUNC(0x0231CBE0, void, a, p1, p2);
    gabi::Local<cXyz> position;
    *position = a->current.pos;
    gabi::call(0x0255F458, position.get(), p1, gabi::load<u32>(gabi::ea(a) + 4), p2);
}
VERIFY(0x0231CBE0, set_senv);
static void mode_jump_init(Act_c *a) {
    WWHD_FUNC(0x0231CC9C, void, a);
    collision_mode(a);
    clear_carry_attention(a);
    a->shape_angle.y = 0;
    a->shape_angle.z = 0;
    a->mVibrationAngle = 0;
    a->mRotationAngle = 0;
    a->mWalkAngle = 0;
    a->gravity = -6.f;
    a->speed.y = 30.f;
    a->mMode = 5;
}
VERIFY(0x0231CC9C, mode_jump_init);
static void mode_walk_init(Act_c *a) {
    WWHD_FUNC(0x0231CD08, void, a);
    collision_mode(a);
    clear_carry_attention(a);
    a->gravity = -6.f;
    a->mMode = 6;
}
VERIFY(0x0231CD08, mode_walk_init);
static bool chk_sink_water(Act_c *a) {
    WWHD_FUNC(0x0231D0F0, bool, a);
    return a->mAcch.ChkWaterHit() &&
           gabi::load<f32>(gabi::ea(a) + 0x574) > (f32)a->current.pos.y + 50.f;
}
VERIFY(0x0231D0F0, chk_sink_water);
static bool chk_sinkdown_water(Act_c *a) {
    WWHD_FUNC(0x0231D25C, bool, a);
    return a->mAcch.ChkWaterHit() &&
           gabi::load<f32>(gabi::ea(a) + 0x574) > ((f32)a->current.pos.y + 100.f) + 50.f;
}
VERIFY(0x0231D25C, chk_sinkdown_water);
static void eff_hit_water_splash(Act_c *a) {
    WWHD_FUNC(0x0231D20C, void, a);
    gabi::Local<cXyz> position;
    position->x = a->current.pos.x;
    position->y = gabi::load<f32>(gabi::ea(a) + 0x574);
    position->z = a->current.pos.z;
    fopKyM_createWpillar(position.get(), 1.f, 0.75f, 0);
}
VERIFY(0x0231D20C, eff_hit_water_splash);
static void mode_vib0_init(Act_c *a) {
    WWHD_FUNC(0x0231D344, void, a);
    collision_mode(a);
    clear_carry_attention(a);
    a->gravity = -14.f;
    a->speed.y = 80.f;
    a->shape_angle.y = 0;
    a->shape_angle.z = 0;
    a->mVibrationAngle = 3500;
    a->mRotationAngle = 0;
    a->mWalkAngle = 0;
    a->mTimer = 5;
    a->mMode = 2;
}
VERIFY(0x0231D344, mode_vib0_init);
static void eff_land_smoke(Act_c *a) {
    WWHD_FUNC(0x0231D5B4, void, a);
    gabi::call(0x02311CD8, a, gabi::at<u8>(gabi::ea(a) + 0x48C), 1.f);
}
VERIFY(0x0231D5B4, eff_land_smoke);
static void mode_vib1_init(Act_c *a) {
    WWHD_FUNC(0x0231E08C, void, a);
    a->gravity = -1.f;
    a->mTimer = 12;
    a->mMode = 3;
}
VERIFY(0x0231E08C, mode_vib1_init);
static void mode_vib2_init(Act_c *a) {
    WWHD_FUNC(0x0231E0F0, void, a);
    a->gravity = -12.f;
    a->mTimer = 20;
    a->mMode = 4;
}
VERIFY(0x0231E0F0, mode_vib2_init);
static void global_init() {
    WWHD_FUNC(0x0231E504, void);
    for (u32 i = 0; i < 4; i++)
        gabi::store<u32>(0x104691EC + i * 4, 0);
    gabi::call(0x028F026C, gabi::at<u8>(0x101C7E8C));
    gabi::store<f32>(0x104691E0, -3.1415927410125732f);
    gabi::store<f32>(0x104691E4, 3.1415927410125732f);
    gabi::call(0x028ED6F8, gabi::at<u8>(0x104691E8));
    gabi::call(0x028F026C, gabi::at<u8>(0x101C7E98));
    gabi::call(0x028EAB2C, gabi::at<u8>(0x104691E9));
    gabi::call(0x028F026C, gabi::at<u8>(0x101C7EA4));
}
VERIFY(0x0231E504, global_init);
static void static_dtor(void *object, u32 flags) {
    WWHD_FUNC(0x0231E598, void, object, flags);
    if (object && (flags & 1))
        gabi::call(0x0273AF40, object);
}
VERIFY(0x0231E598, static_dtor);
static void empty_dtor(void *object, u32 flags) { WWHD_FUNC(0x0231E5AC, void, object, flags); }
VERIFY(0x0231E5AC, empty_dtor);
static void actor_dtor(Act_c *a, u32 flags) {
    WWHD_FUNC(0x0231E5B0, void, a, flags);
    if (a) {
        gabi::call(0x02515A70, &a->mCyl, 2);
        gabi::call(0x02515860, &a->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(a) + 0x590), 2);
        gabi::store<u32>(gabi::ea(a) + 0x3D8, 0x10025590);
        gabi::store<u32>(gabi::ea(a) + 0x3CC, 0x100255A0);
        gabi::call(0x024EFD9C, &a->mAcch, 0);
        gabi::call(0x025D50BC, a, 0);
        if (flags & 1)
            gabi::call(0x0273AF40, a);
    }
}
VERIFY(0x0231E5B0, actor_dtor);
static bool is_delete(Act_c *a) {
    WWHD_FUNC(0x0231E64C, bool, a);
    return true;
}
VERIFY(0x0231E64C, is_delete);
static u32 parameter(Act_c *a, u32 width, u32 shift) {
    WWHD_FUNC(0x0231E654, u32, a, width, shift);
    u32 shifted = (shift & 63) >= 32 ? 0 : (u32)a->mParameters >> (shift & 31);
    u32 limit = (width & 63) >= 32 ? 0 : 1u << (width & 31);
    return shifted & (limit - 1u);
}
VERIFY(0x0231E654, parameter);
static void eff_break(Act_c *a) {
    WWHD_FUNC(0x0231CAC4, void, a);
    gabi::Local<cXyz> position;
    position->x = a->current.pos.x;
    position->y = (f32)a->current.pos.y + 50.f;
    position->z = a->current.pos.z;
    u32 play = gabi::ea(dComIfGp_get());
    auto *emitter =
        gabi::call<u8 *>(0x025A847C, gabi::at<u8>(gabi::load<u32>(play + 0x5AB0)), 0, 0x3E5,
                         position.get(), nullptr, nullptr, 255, nullptr, -1,
                         gabi::at<u8>(gabi::ea(a) + 0x1A8), gabi::at<u8>(gabi::ea(a) + 0x1A8), 0);
    if (emitter) {
        if (!gabi::load<u32>(0x104691FC)) {
            gabi::store<u32>(0x104691FC, 1);
            gabi::store<f32>(0x10469200, 1.f);
            gabi::store<f32>(0x10469204, 0.8f);
            gabi::store<f32>(0x10469208, 1.f);
        }
        gabi::store<f32>(gabi::ea(emitter) + 8, gabi::load<f32>(0x10469200));
        gabi::store<f32>(gabi::ea(emitter) + 12, gabi::load<f32>(0x10469204));
        gabi::store<f32>(gabi::ea(emitter) + 16, gabi::load<f32>(0x10469208));
    }
    gabi::call(0x025D5834, 0x1D3, 0, position.get(), -1, nullptr, nullptr, -1, nullptr);
}
VERIFY(0x0231CAC4, eff_break);
static void damaged(Act_c *a, s32 animate) {
    WWHD_FUNC(0x0231CC24, void, a, animate);
    gabi::call(0x025D9D24, a);
    if (animate) {
        eff_break(a);
        s32 reverb = gabi::call<s32>(0x02520540, (s8)a->current.roomNo);
        gabi::call(0x025E1A40, 0x6805, &a->eyePos, 0, reverb);
        set_senv(a, 150, 5);
    }
}
VERIFY(0x0231CC24, damaged);
static void se_fall_water(Act_c *a) {
    WWHD_FUNC(0x0231D128, void, a);
    u32 sound = 0x13;
    u32 polygons[2] = {gabi::ea(a) + 0x52C, gabi::ea(a) + 0x4A0};
    for (u32 polygon : polygons) {
        if (gabi::load<u16>(polygon + 2) < 256) {
            auto *play = dComIfGp_get();
            sound = gabi::call<u32>(0x024EECAC, gabi::at<u8>(gabi::ea(play) + 0x12A0),
                                    gabi::at<u8>(polygon));
            break;
        }
    }
    s32 reverb = gabi::call<s32>(0x02520540, (s8)a->current.roomNo);
    gabi::call(0x025E1A40, 0x6919, &a->eyePos, sound, reverb);
    set_senv(a, 125, 5);
}
VERIFY(0x0231D128, se_fall_water);
static bool damage_bg_proc(Act_c *a) {
    WWHD_FUNC(0x0231D2A0, bool, a);
    bool sunk = chk_sink_water(a);
    bool broken = false;
    if (((s32)a->mMode == 0 || (s32)a->mMode == 5 || (s32)a->mMode == 6) && sunk) {
        if (!a->mSunk) {
            se_fall_water(a);
            eff_hit_water_splash(a);
            a->mSunk = 1;
        }
        if (chk_sinkdown_water(a)) {
            damaged(a, 0);
            broken = true;
        }
    }
    return broken;
}
VERIFY(0x0231D2A0, damage_bg_proc);
static bool damage_bg_proc_directly(Act_c *a) {
    WWHD_FUNC(0x0231D5C4, bool, a);
    bool landing = a->mAcch.ChkGroundLanding();
    bool ground = a->mAcch.ChkGroundHit();
    bool broken = false;
    if ((s32)a->mMode == 0 || (s32)a->mMode == 5 || (s32)a->mMode == 6) {
        if (landing && ((f32)a->mLastGroundY - (f32)a->current.pos.y) > 200.f) {
            damaged(a, 1);
            broken = true;
        }
        if (ground)
            a->mLastGroundY = a->current.pos.y;
    }
    if ((s8)a->mInitTimer > 0)
        a->mInitTimer = (s8)a->mInitTimer - 1;
    else {
        if (a->mAcch.ChkGroundHit()) {
            if (!a->mOnGround && (s32)a->mMode != 1) {
                auto *play = dComIfGp_get();
                u32 material = gabi::call<u32>(0x024EECAC, gabi::at<u8>(gabi::ea(play) + 0x12A0),
                                               gabi::at<u8>(gabi::ea(a) + 0x4A0));
                s32 reverb = gabi::call<s32>(0x02520540, (s8)a->current.roomNo);
                gabi::call(0x025E1A40, 0x282A, &a->eyePos, material, reverb);
                a->mOnGround = 1;
            }
        } else
            a->mOnGround = 0;
        if (landing && ((s32)a->mMode != 1 || (s32)a->mTimer <= 0))
            eff_land_smoke(a);
    }
    return broken;
}
VERIFY(0x0231D5C4, damage_bg_proc_directly);
static bool actor_draw(Act_c *a) {
    WWHD_FUNC(0x0231DAE0, bool, a);
    auto *light = dKy_getEnvlight();
    settingTevStruct(light, 0, &a->current.pos, &a->tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, a->mpModel, &a->tevStr);
    gabi::call(0x025E2DE0, (J3DModel *)a->mpModel, 0);
    return true;
}
VERIFY(0x0231DAE0, actor_draw);
static void mode_carry(Act_c *a) {
    WWHD_FUNC(0x0231DE48, void, a);
    s16 angle = a->mCarryAngle;
    if ((s32)a->mTimer > 0)
        a->mTimer = (s32)a->mTimer - 1;
    gabi::call(0x0200F8D0, &a->shape_angle.z, angle, 0x1000);
    if (!((u32)a->actor_status & 0x102000)) {
        a->mLastGroundY = a->current.pos.y;
        gabi::call(0x02312968, a, gabi::at<u8>(gabi::ea(a) + 0x48C));
        a->mVibrationAngle = 0;
        a->mRotationAngle = 0;
        mode_wait_init(a);
    }
}
VERIFY(0x0231DE48, mode_carry);
static void vib_pos_ang(Act_c *a) {
    WWHD_FUNC(0x0231DED0, void, a);
    f32 factor = a->mAcch.ChkGroundHit() ? 0.8f : 1.f;
    f32 angle = gabi::fmadds((f32)(s16)a->mVibrationAngle, 8.544921729480848e-05f, 0.3f);
    angle = ((16000.f * angle) * 0.1f) * factor;
    gabi::call(0x025D6870, a, &a->mStts.m_cc_move);
    a->shape_angle.y = (s16)a->shape_angle.y + (s16)gabi::ftoi(angle);
    a->mRotationAngle = (s16)a->mRotationAngle + (s16)gabi::ftoi(14400.f * factor);
}
VERIFY(0x0231DED0, vib_pos_ang);
static void mode_vib0(Act_c *a) {
    WWHD_FUNC(0x0231E0AC, void, a);
    vib_pos_ang(a);
    a->mTimer = (u32)a->mTimer - 1u;
    if ((s32)a->mTimer <= 0)
        mode_vib1_init(a);
}
VERIFY(0x0231E0AC, mode_vib0);
static void mode_vib1(Act_c *a) {
    WWHD_FUNC(0x0231E110, void, a);
    vib_pos_ang(a);
    a->mTimer = (u32)a->mTimer - 1u;
    if ((s32)a->mTimer <= 0)
        mode_vib2_init(a);
}
VERIFY(0x0231E110, mode_vib1);
static void mode_vib2(Act_c *a) {
    WWHD_FUNC(0x0231E154, void, a);
    gabi::call(0x0200F8D0, &a->mVibrationAngle, 0, 150);
    vib_pos_ang(a);
    a->mTimer = (u32)a->mTimer - 1u;
    if ((s32)a->mTimer <= 0) {
        a->shape_angle.x = 0;
        a->mVibrationAngle = 0;
        a->mRotationAngle = 0;
        mode_wait_init(a);
    }
}
VERIFY(0x0231E154, mode_vib2);
static bool draw_wrapper(Act_c *a) {
    WWHD_FUNC(0x0231E500, bool, a);
    return actor_draw(a);
}
VERIFY(0x0231E500, draw_wrapper);
static void set_walk_rot(Act_c *a) {
    WWHD_FUNC(0x0231DB3C, void, a);
    gabi::Local<cXyz> delta;
    gabi::call(0x0201ADE0, &a->current.pos, delta.get(), &a->old.pos);
    gabi::Local<cXyz> horizontal;
    horizontal->x = delta->x;
    horizontal->y = 0.f;
    horizontal->z = delta->z;
    f32 magnitude = gabi::call<f32>(0x028E8DD0, horizontal.get());
    magnitude = gabi::call<f32>(0x028F4384, magnitude);
    s32 target = gabi::call<s32>(0x020195B0, (f32)delta->x, (f32)delta->z);
    a->current.angle.y = target;
    bool negative = target < 0;
    if (negative)
        target = (s16)(target - 0x8000);
    if (magnitude > 5.f || ((s32)a->mMode == 0 && magnitude > 2.5f))
        gabi::call(0x0200F8D0, &a->shape_angle.y, target, 0x600);
    f32 cosine = gabi::load<f32>(0x104A44FC + ((u16)a->shape_angle.z >> 3) * 8);
    f32 roll = (magnitude / (6.28f * gabi::fmadds(5.f, cosine, 45.f))) * 65535.f;
    s16 rotation = gabi::ftoi(roll * 3.f);
    s16 walk = gabi::ftoi(roll);
    if (negative) {
        a->mRotationAngle = (s16)a->mRotationAngle + rotation;
        a->mWalkAngle = (s16)a->mWalkAngle + walk;
    } else {
        a->mRotationAngle = (s16)a->mRotationAngle - rotation;
        a->mWalkAngle = (s16)a->mWalkAngle - walk;
    }
    f32 factor = magnitude / 30.f;
    if (factor > 1.f)
        factor = 1.f;
    a->mVibrationAngle = gabi::ftoi(2048.f * factor);
}
VERIFY(0x0231DB3C, set_walk_rot);
static void mode_wait(Act_c *a) {
    WWHD_FUNC(0x0231DD40, void, a);
    gabi::call(0x025D6870, a, &a->mStts.m_cc_move);
    gabi::call(0x0200F8D0, &a->shape_angle.z, (s16)a->shape_angle.z > 0x1000 ? 0x4000 : 0, 0xC00);
    set_walk_rot(a);
    if (a->mAcch.ChkGroundHit()) {
        *gabi::at<be<u32>>(gabi::ea(a) + 0x39C) |= 0x10u;
        if ((s16)a->shape_angle.z == 0x4000) {
            auto *play = dComIfGp_get();
            auto *normal = gabi::call<cXyz *>(0x020084C8, gabi::at<u8>(gabi::ea(play) + 0x12A0),
                                              gabi::load<u16>(gabi::ea(a) + 0x4A2),
                                              gabi::load<u16>(gabi::ea(a) + 0x4A0));
            if (normal &&
                (std::fabs((f32)normal->x) > 0.001f || std::fabs((f32)normal->z) > 0.001f))
                mode_walk_init(a);
            a->mStts.m_weight = 140;
        } else
            a->mStts.m_weight = 200;
    } else
        clear_carry_attention(a);
}
VERIFY(0x0231DD40, mode_wait);
static void mode_jump(Act_c *a) {
    WWHD_FUNC(0x0231E1BC, void, a);
    gabi::call(0x02312968, a, gabi::at<u8>(gabi::ea(a) + 0x48C));
    if (a->mAcch.ChkGroundLanding())
        mode_walk_init(a);
    else {
        gabi::call(0x023123C0, a, &a->mStts.m_cc_move, &a->mMove, 0.006f, 0.001f);
        gabi::call(0x0200F8D0, &a->shape_angle.z, 0x4000, 0xC00);
        set_walk_rot(a);
    }
}
VERIFY(0x0231E1BC, mode_jump);
static void mode_walk(Act_c *a) {
    WWHD_FUNC(0x0231E248, void, a);
    gabi::call(0x02312968, a, gabi::at<u8>(gabi::ea(a) + 0x48C));
    if (a->mAcch.ChkGroundHit()) {
        auto *play = dComIfGp_get();
        auto *normal = gabi::call<cXyz *>(0x020084C8, gabi::at<u8>(gabi::ea(play) + 0x12A0),
                                          gabi::load<u16>(gabi::ea(a) + 0x4A2),
                                          gabi::load<u16>(gabi::ea(a) + 0x4A0));
        gabi::Local<cXyz> acceleration;
        *acceleration = *gabi::at<cXyz>(0x101FFBA8);
        f32 friction = 0.f, cosine = 0.f;
        if (normal) {
            acceleration->x = normal->x;
            acceleration->y = 0.f;
            acceleration->z = normal->z;
            friction = 0.1f;
            cosine = gabi::load<f32>(0x104A4F44);
        }
        gabi::call(0x023121C4, a, &a->mStts.m_cc_move, &a->mMove, 0.006f, 0.001f, normal, friction,
                   cosine, acceleration.get());
        gabi::call(0x0200F8D0, &a->shape_angle.z, 0x4000, 0xC00);
        set_walk_rot(a);
        *gabi::at<be<u32>>(gabi::ea(a) + 0x39C) |= 0x10u;
        if (normal) {
            gabi::Local<cXyz> horizontal;
            horizontal->x = a->speed.x;
            horizontal->y = 0.f;
            horizontal->z = a->speed.z;
            f32 magnitude = gabi::call<f32>(0x028E8DD0, horizontal.get());
            if (magnitude < 0.1f && !(std::fabs((f32)normal->x) < 0.001f) &&
                !(std::fabs((f32)normal->z) < 0.001f))
                mode_wait_init(a);
        }
    } else {
        gabi::call(0x023123C0, a, &a->mStts.m_cc_move, &a->mMove, 0.006f, 0.001f);
        gabi::call(0x0200F8D0, &a->shape_angle.z, 0x4000, 0xC00);
        set_walk_rot(a);
    }
}
VERIFY(0x0231E248, mode_walk);
static bool stage_matches(u32 literal) {
    gabi::Local<be<u32>[2]> expected, stage;
    (*expected)[0] = literal;
    (*expected)[1] = 0x10025558;
    (*stage)[0] = gabi::ea(dComIfGp_get()) + 0x5134;
    (*stage)[1] = 0x10025558;
    u32 ensure = gabi::load<u32>(0x1002556C);
    gabi::call(ensure, expected.get());
    gabi::call(ensure, expected.get());
    u32 left = (*expected)[0];
    gabi::call(ensure, stage.get());
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
static void mode_carry_init(Act_c *a) {
    WWHD_FUNC(0x0231D3BC, void, a);
    collision_mode(a, true);
    clear_carry_attention(a);
    a->mMode = 1;
    if (stage_matches(0x10025644) || stage_matches(0x1002564C))
        gabi::call(0x025B8B68, gabi::at<u8>(gabi::load<u32>(0x101F84DC) + 0x644), 0x401);
    a->mTimer = 15;
}
VERIFY(0x0231D3BC, mode_carry_init);
static bool mode_proc_call(Act_c *a) {
    WWHD_FUNC(0x0231D724, bool, a);
    if (((u32)a->actor_status & 0x102000) && (s32)a->mMode != 1)
        mode_carry_init(a);
    u32 entry = 0x10025660 + (u32)a->mMode * 8;
    u32 object = gabi::ea(a) + (s32)gabi::load<s16>(entry);
    s16 index = gabi::load<s16>(entry + 2);
    u32 target;
    if (index < 0)
        target = gabi::load<u32>(entry + 4);
    else
        target =
            gabi::load<u32>(gabi::load<u32>(object + gabi::load<s16>(entry + 6)) + index * 8 + 4);
    gabi::call(target, gabi::at<Act_c>(object));
    f32 y = a->current.pos.y, z = a->current.pos.z, x = a->current.pos.x;
    auto *play = dComIfGp_get();
    a->mAcch.CrrPos(gabi::at<dBgS>(gabi::ea(play) + 0x12A0));
    play = dComIfGp_get();
    if (gabi::call<s32>(0x024EEABC, gabi::at<u8>(gabi::ea(play) + 0x12A0),
                        gabi::at<u8>(gabi::ea(a) + 0x4A0)))
        a->mForceExec = 1;
    if ((s32)a->mMode == 1) {
        a->current.pos.y = y;
        a->current.pos.x = x;
        a->current.pos.z = z;
    }
    if (damage_bg_proc_directly(a))
        return false;
    if ((s32)a->mMode != 1) {
        a->tevStr.mRoomNo = a->current.roomNo;
        play = dComIfGp_get();
        gabi::store<u8>(gabi::ea(a) + 0x1CA,
                        gabi::call<u32>(0x024EEEB8, gabi::at<u8>(gabi::ea(play) + 0x12A0),
                                        gabi::at<u8>(gabi::ea(a) + 0x4A0)));
    }
    gabi::call(0x028E8E64, &a->mMove, &a->mMove, 0.95f);
    if (gabi::call<f32>(0x028E8DD0, &a->mMove) < 0.1f) {
        a->mMove.x = 0.f;
        a->mMove.z = 0.f;
        a->mMove.y = 0.f;
    }
    return true;
}
VERIFY(0x0231D724, mode_proc_call);
static s32 actor_create(Act_c *a) {
    WWHD_FUNC(0x0231C674, s32, a);
    if (!((u32)a->actor_condition & 8)) {
        if (a) {
            fopAc_ac_c_ct(a);
            a->__vtbl = 0x100255B0;
            gabi::call(0x024F0474, &a->mAcch);
            gabi::store<u8>(gabi::ea(a) + 0x3D0, 1);
            gabi::store<u32>(gabi::ea(a) + 0x3C8, 0x10025580);
            gabi::store<u32>(gabi::ea(a) + 0x3D8, 0x10025590);
            gabi::store<u32>(gabi::ea(a) + 0x3CC, 0x100255A0);
            gabi::call(0x024EFE94, &a->mAcchCir);
            dCcD_Stts_ct(&a->mStts);
            dCcD_Cyl_ct(&a->mCyl, 0x10025570);
        }
        a->actor_condition |= 8u;
    }
    s32 phase = gabi::call<s32>(0x02520460, &a->mPhase, STR(0x100256E4));
    if (phase != 4)
        return phase;
    if (!gabi::call<s32>(0x025D63E8, a, 0x0231C35C, 0x820))
        return 5;
    a->mAcchCir.SetWall(30.f, 50.f);
    a->mAcch.Set(&a->current.pos, &a->old.pos, a, 1, &a->mAcchCir, &a->speed, &a->current.angle,
                 &a->shape_angle);
    a->mAcch.m_flags &= ~1024u;
    u32 model = gabi::ea((J3DModel *)a->mpModel);
    a->cullMtx = model ? model + 0xC8 : 0;
    cull_set_draw(a);
    if (stage_matches(0x10025614)) {
        auto *play = dComIfGp_get();
        u32 accessor = gabi::load<u32>(gabi::ea(play) + 0x5150);
        u32 info = gabi::call<u32>(gabi::load<u32>(accessor + 0x15C),
                                   gabi::at<u8>(gabi::ea(play) + 0x5150));
        f32 far = gabi::load<u16>(info + 0x12);
        if (far > 1.f)
            a->cullSizeFar = 8000.f / far;
    }
    a->mStts.Init(200, 255, a);
    a->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x100256F0));
    *gabi::at<be<u32>>(gabi::ea(a) + 0x39C) |= 0x10u;
    a->mCyl.SetStts(&a->mStts);
    a->gravity = -6.f;
    gabi::store<f32>(gabi::ea(a) + 0x390, a->current.pos.x);
    gabi::store<f32>(gabi::ea(a) + 0x394, (f32)a->current.pos.y + 50.f);
    gabi::store<f32>(gabi::ea(a) + 0x398, a->current.pos.z);
    a->mForceExec = 1;
    gabi::store<u8>(gabi::ea(a) + 0x38C, 9);
    gabi::call(0x025D6870, a, nullptr);
    auto *play = dComIfGp_get();
    a->mAcch.CrrPos(gabi::at<dBgS>(gabi::ea(play) + 0x12A0));
    a->mOnGround = 1;
    a->mInitTimer = 20;
    a->mSunk = 0;
    a->mAcch.m_flags &= ~128u;
    a->mLastGroundY = a->current.pos.y;
    init_mtx(a);
    a->mMove = *gabi::at<cXyz>(0x101FFBA8);
    mode_wait_init(a);
    return phase;
}
VERIFY(0x0231C674, actor_create);
static bool damage_cc_proc(Act_c *a) {
    WWHD_FUNC(0x0231CD50, bool, a);
    if (a->mCyl.ChkAtHit()) {
        damaged(a, 1);
        a->mCyl.ClrAtHit();
        return true;
    }
    if (!a->mCyl.ChkTgHit())
        return false;
    void *hit = a->mCyl.GetTgHitObj();
    u32 sound = get_se_map_hit(a);
    if (hit) {
        u32 type = gabi::load<u32>(gabi::ea(hit) + 0x10);
        if ((type & 0x10028) ||
            ((gabi::load<u32>(gabi::ea(hit)) & 4) && ((u8)a->mStts.mAtSpl & 3))) {
            damaged(a, 1);
            a->mCyl.ClrTgHit();
            return true;
        }
        if (((s32)a->mMode == 0 || (s32)a->mMode == 6) && (type & 0x200000)) {
            gabi::Local<cXyz> wind, normal;
            *wind = *gabi::at<cXyz>(gabi::ea(a) + 0x6B8);
            f32 magnitude = gabi::call<f32>(0x028E8DD0, wind.get());
            if (magnitude > 31684.f) {
                f32 root = gabi::call<f32>(0x028F4384, magnitude);
                gabi::call(0x028E8E64, wind.get(), wind.get(), 178.f / root);
            }
            u32 shape =
                gabi::call<u32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(hit) + 0x3C) + 0x2C), hit);
            *normal = *gabi::at<cXyz>(0x101FFBA8);
            f32 strength = 1.f;
            u32 callback = gabi::load<u32>(gabi::load<u32>(shape + 0x1C) + 0x9C);
            if (gabi::call<s32>(callback, gabi::at<u8>(shape), &a->current.pos, normal.get())) {
                gabi::call(0x028E8E64, normal.get(), normal.get(), 25.f);
                gabi::call<f32>(0x028E8DD0, &a->mMove);
                auto *actor = gabi::call<fopAc_ac_c *>(0x02515BBC, &a->mCyl.mGObjTg);
                if (actor && gabi::load<s16>(gabi::ea(actor) + 0xE) == 0xA8) {
                    s32 angle = gabi::call<s32>(0x020195B0, (f32)normal->x, (f32)normal->z);
                    u16 difference = (s16)actor->shape_angle.y - angle;
                    f32 cosine = gabi::load<f32>(0x104A44FC + (difference >> 3) * 8);
                    if (cosine > 0.f)
                        strength = (cosine + cosine) + 1.f;
                }
            }
            f32 ratio = (0.01f - magnitude) >= 0.f ? 0.f : 0.5f;
            gabi::Local<cXyz> windPart, normalPart, scaledNormal, result;
            gabi::call(0x0201AE48, wind.get(), windPart.get(), ratio);
            gabi::call(0x0201AE48, normal.get(), normalPart.get(), 1.f - ratio);
            gabi::call(0x0201AE48, normalPart.get(), scaledNormal.get(), strength);
            gabi::call(0x0201AD78, windPart.get(), result.get(), scaledNormal.get());
            a->mMove = *result;
            if ((s32)a->mMode == 0) {
                if ((s16)a->shape_angle.z == 0)
                    mode_jump_init(a);
                else
                    mode_walk_init(a);
            }
        }
    }
    gabi::call(0x023129C4, &a->eyePos, (s8)a->current.roomNo, &a->mCyl, sound);
    set_senv(a, 100, 4);
    gabi::call(0x02312C8C, a, &a->mCyl);
    a->mCyl.ClrTgHit();
    return false;
}
VERIFY(0x0231CD50, damage_cc_proc);
static bool actor_execute(Act_c *a) {
    WWHD_FUNC(0x0231D944, bool, a);
    cull_set_move(a);
    bool execute = a->mForceExec || (s32)a->mMode != 0 || !a->mAcch.ChkGroundHit() ||
                   a->mAcch.ChkGroundLanding();
    if (!execute) {
        execute = !parameter(a, 3, 28) || !((u32)a->actor_condition & 4) ||
                  !gabi::call<s32>(0x025D6CE8, a);
    }
    if (execute) {
        a->mForceExec = 0;
        bool broken = true;
        if (!damage_cc_proc(a) && !damage_bg_proc(a)) {
            s32 mode = a->mMode;
            if (mode != 2 && mode != 3 && mode != 4 && mode != 5 && mode != 6 &&
                !((u32)a->actor_status & 0x2000) && (s16)a->shape_angle.x != 0)
                mode_vib0_init(a);
            if (mode_proc_call(a)) {
                broken = false;
                set_mtx(a);
                a->mStts.mRoomId = a->current.roomNo;
                gabi::call(0x020182E0, &a->mCyl.mCyl, &a->current.pos);
                auto *play = dComIfGp_get();
                gabi::call(0x0200E240, gabi::at<u8>(gabi::ea(play) + 0x26A4), &a->mCyl);
                a->eyePos.x = a->current.pos.x;
                a->eyePos.y = (f32)a->current.pos.y + 50.f;
                a->eyePos.z = a->current.pos.z;
                *gabi::at<cXyz>(gabi::ea(a) + 0x390) = a->eyePos;
            }
        }
        if (broken)
            fopAcM_delete(a);
    }
    cull_set_draw(a);
    return true;
}
VERIFY(0x0231D944, actor_execute);
static s32 create_wrapper(Act_c *a) {
    WWHD_FUNC(0x0231E4F4, s32, a);
    return actor_create(a);
}
VERIFY(0x0231E4F4, create_wrapper);
static bool delete_wrapper(Act_c *a) {
    WWHD_FUNC(0x0231E4F8, bool, a);
    return actor_delete(a);
}
VERIFY(0x0231E4F8, delete_wrapper);
static bool execute_wrapper(Act_c *a) {
    WWHD_FUNC(0x0231E4FC, bool, a);
    return actor_execute(a);
}
VERIFY(0x0231E4FC, execute_wrapper);
