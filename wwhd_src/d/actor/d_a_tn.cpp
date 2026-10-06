/**
 * d_a_tn.cpp (WWHD)
 * Enemy - Darknut
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tn.cpp) to the WWHD layout and verified against cking.rpx.
 * All 56 functions pass 10,000 generated inputs at seeds 1 and 7.
 */
#include "d/actor/d_a_tn.h"
#include <cmath>

static constexpr u32 TN_SAFESTRING_VTABLE = 0x10040388;

void anm_init(tn_class* actor, int animationIndex, f32 blend,
              s32 loopMode, f32 speed, int soundIndex) {
    WWHD_FUNC(0x024B91E4, void, actor, animationIndex, blend,
              loopMode, speed, soundIndex);
    if (actor->mFrozenAnimation != 0) return;
    J3DAnmTransform* animation = static_cast<J3DAnmTransform*>(
        dComIfG_getObjectRes(STR(0x100404E8), animationIndex, TN_SAFESTRING_VTABLE));
    void* sound = nullptr;
    if (soundIndex >= 0) {
        sound = dComIfG_getObjectRes(STR(0x100404E8), soundIndex, TN_SAFESTRING_VTABLE);
    }
    mDoExt_McaMorf* model = actor->mpBodyMorf;
    model->setAnm(animation, loopMode, blend, speed, 0.0f, -1.0f, sound);
}
VERIFY(0x024B91E4, anm_init);

void tate_anm_init(tn_class* actor, int animationIndex, f32 blend,
                   s32 loopMode, f32 speed) {
    WWHD_FUNC(0x024B931C, void, actor, animationIndex, blend, loopMode, speed);
    if (actor->mFrozenAnimation != 0) return;
    J3DAnmTransform* animation = static_cast<J3DAnmTransform*>(
        dComIfG_getObjectRes(STR(0x100404EB), animationIndex, TN_SAFESTRING_VTABLE));
    mDoExt_McaMorf* model = actor->mpShieldMorf;
    model->setAnm(animation, loopMode, blend, speed, 0.0f, -1.0f, nullptr);
}
VERIFY(0x024B931C, tate_anm_init);

void yoroi_anm_init(tn_class* actor, int animationIndex, f32 blend,
                    s32 loopMode, f32 speed) {
    WWHD_FUNC(0x024B93E8, void, actor, animationIndex, blend, loopMode, speed);
    if (actor->mFrozenAnimation != 0) return;
    if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1) == 0) return;
    J3DAnmTransform* animation = static_cast<J3DAnmTransform*>(
        dComIfG_getObjectRes(STR(0x100404EE), animationIndex, TN_SAFESTRING_VTABLE));
    mDoExt_McaMorf* model = actor->mpArmorMorf;
    model->setAnm(animation, loopMode, blend, speed, 0.0f, -1.0f, nullptr);
}
VERIFY(0x024B93E8, yoroi_anm_init);

BOOL daTn_IsDelete(tn_class* actor) {
    WWHD_FUNC(0x024C018C, BOOL, actor);
    return 1;
}
VERIFY(0x024C018C, daTn_IsDelete);

// HD emits this small bomb-search/state update separately from the main action.
BOOL daTn_checkNearbyBomb(tn_class* actor) {
    WWHD_FUNC(0x024BAF44, BOOL, actor);
    if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1) == 0) {
        fopAc_ac_c* bomb = gabi::call<fopAc_ac_c*>(0x024BAB8C, actor, 1);
        actor->mNearbyBomb = bomb;
        if (bomb != nullptr) return 1;
    }
    return 0;
}
VERIFY(0x024BAF44, daTn_checkNearbyBomb);

void tnSafeString_noop(void* string) {
    WWHD_FUNC(0x024C6D58, void, string);
}
VERIFY(0x024C6D58, tnSafeString_noop);

s16 get_view_H(tn_class* actor) {
    WWHD_FUNC(0x024BAA68, s16, actor);
    const bool helmet = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 2) != 0;
    return gabi::load<s16>(helmet ? 0x1046E7AA : 0x1046E7A8);
}
VERIFY(0x024BAA68, get_view_H);

// Search callbacks share the unit's bounded target list. Address arithmetic
// uses guest words, preserving the native indexing arithmetic without host UB.
static void tn_appendTarget(fopAc_ac_c* actor) {
    const s32 count = gabi::load<s32>(0x1046E718);
    if (count < 10) {
        const u32 index = static_cast<u32>(count);
        gabi::store<u32>(0x1046E718, index + 1u);
        gabi::store<u32>(0x1046E740 + index * 4u, gabi::ea(actor));
    }
}

void* s_w_sub(fopAc_ac_c* actor, void* context) {
    WWHD_FUNC(0x024BAA8C, void*, actor, context);
    if (gabi::call<BOOL>(0x025D4604, actor) == 0) return nullptr;
    if (actor == nullptr) return nullptr;
    if (gabi::load<s16>(gabi::ea(actor) + 8u) != 0x1CF) return nullptr;
    if ((static_cast<u32>(actor->actor_status) & 0x2000) != 0) return nullptr;
    tn_appendTarget(actor);
    return nullptr;
}
VERIFY(0x024BAA8C, s_w_sub);

void* s_b_sub(fopAc_ac_c* actor, void* context) {
    WWHD_FUNC(0x024BAB08, void*, actor, context);
    if (gabi::call<BOOL>(0x025D4604, actor) == 0) return nullptr;
    if (actor == nullptr) return nullptr;
    if (gabi::load<s16>(gabi::ea(actor) + 8u) != 0x126) return nullptr;
    if (gabi::call<BOOL>(0x020CB92C, actor, 0) != 0) return nullptr;
    tn_appendTarget(actor);
    return nullptr;
}
VERIFY(0x024BAB08, s_b_sub);

void* shot_s_sub(fopAc_ac_c* actor, void* context) {
    WWHD_FUNC(0x024BBAEC, void*, actor, context);
    if (gabi::call<BOOL>(0x025D4604, actor) == 0) return nullptr;
    if (actor == nullptr) return nullptr;
    if (gabi::load<s16>(gabi::ea(actor) + 8u) != 0x1BE) return nullptr;
    return actor;
}
VERIFY(0x024BBAEC, shot_s_sub);

void wait_set(tn_class* actor) {
    WWHD_FUNC(0x024BB620, void, actor);
    anm_init(actor, 0x37, 15.0f, 2, 1.0f, -1);
    yoroi_anm_init(actor, 0x65, 15.0f, 2, 1.0f);
    const f32 random = gabi::call<f32>(0x020198D8, 30.0f);
    actor->mCountDownTimers[1] = static_cast<s16>(gabi::ftoi(random + 50.0f));
}
VERIFY(0x024BB620, wait_set);

void fight_run_set(tn_class* actor) {
    WWHD_FUNC(0x024BB6E0, void, actor);
    if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1) != 0) {
        anm_init(actor, 0x3E, 5.0f, 2, 1.5f, 0x11);
        yoroi_anm_init(actor, 0x67, 5.0f, 2, 1.5f);
    } else {
        if (actor->mPathDriven != 0) {
            anm_init(actor, 0x50, 5.0f, 2, 1.0f, 0x20);
        } else {
            anm_init(actor, 0x51, 5.0f, 2, 1.0f, 0x21);
        }
        yoroi_anm_init(actor, 0x64, 5.0f, 0, 1.0f);
    }
}
VERIFY(0x024BB6E0, fight_run_set);

void d_mahi(tn_class* actor) {
    WWHD_FUNC(0x024C2DD0, void, actor);
    const s16 mode = actor->mActionMode;
    if (mode == 0) {
        actor->mActionMode = 1;
        anm_init(actor, 0x38, 3.0f, 0, 1.0f, -1);
        yoroi_anm_init(actor, 0x66, 3.0f, 0, 1.0f);
        actor->mCountDownTimers[0] = 60;
        actor->speedF = 0.0f;
        return;
    }
    if (mode != 1) return;

    const s16 timer = actor->mCountDownTimers[0];
    if (static_cast<u32>(static_cast<s32>(timer) - 10) < 31u) {
        if (timer == 40) gabi::call(0x02041CAC, actor);
        const u32 eyeAddress = gabi::ea(actor) + 0x37Cu;
        if (actor != nullptr && eyeAddress != 0) {
            const s8 room = actor->current.roomNo;
            const s32 reverb = gabi::call<s32>(0x02520540, room);
            gabi::call(0x025E1A40, 0x50BC, gabi::at<cXyz>(eyeAddress), 0, reverb);
        }
    }

    const s16 remaining = actor->mCountDownTimers[0];
    actor->speedF = 0.0f;
    if (remaining == 0) {
        actor->mAction = 0;
        gabi::call(0x024BB800, actor);
        wait_set(actor);
        actor->mActionMode = 2;
    }
}
VERIFY(0x024C2DD0, d_mahi);

void tnSafeString_destroy(void* string, int flags) {
    WWHD_FUNC(0x024C18A4, void, string, flags);
    if (string != nullptr && (static_cast<u32>(flags) & 1) != 0) {
        gabi::call(0x0273AF40, string);
    }
}
VERIFY(0x024C18A4, tnSafeString_destroy);

void* tnCutTurn_construct(void* callback) {
    WWHD_FUNC(0x024C6BAC, void*, callback);
    if (callback == nullptr) {
        callback = gabi::call<void*>(0x0273AD10, 0x10);
        if (callback == nullptr) return nullptr;
    }
    gabi::store<u32>(gabi::ea(callback), 0x10052228);
    return callback;
}
VERIFY(0x024C6BAC, tnCutTurn_construct);

void tnCutTurn_destroy(void* callback, int flags) {
    WWHD_FUNC(0x024C6BEC, void, callback, flags);
    if (callback != nullptr && (static_cast<u32>(flags) & 1) != 0) {
        gabi::call(0x0273AF40, callback);
    }
}
VERIFY(0x024C6BEC, tnCutTurn_destroy);

// HD's separate color conversion helper applies a 2.2 power and clamps.
f32 daTn_colorPower(u32 channel) {
    WWHD_FUNC(0x024BD0F0, f32, channel);
    const f32 normalized = static_cast<f32>(channel) / 255.0f;
    f32 value = gabi::call<f32>(0x028F4560, normalized, 2.2f);
    if (value < 0.0f) value = 0.0f;
    else if (value > 1.0f) value = 1.0f;
    return value;
}
VERIFY(0x024BD0F0, daTn_colorPower);

tn_enemyfire* tnEnemyFire_construct(tn_enemyfire* fire) {
    WWHD_FUNC(0x024C0898, tn_enemyfire*, fire);
    if (fire == nullptr) {
        fire = gabi::call<tn_enemyfire*>(0x0273AD10, 0x22C);
        if (fire == nullptr) return nullptr;
    }
    // The native inlined vector constructor retains its null-subobject path.
    if (gabi::ea(fire) + 0x8Cu == 0) gabi::call<void*>(0x0273AD10, 12);
    gabi::call(0x0200BD2C, &fire->mStts);
    gabi::call(0x02515DA0, gabi::at<u8>(gabi::ea(fire) + 0xBC));
    fire->mStts.__vtbl = 0x1004AE88;
    fire->mStts.__vtbl_gstts = 0x1004AEC0;
    gabi::call(0x025166F0, &fire->mSph);
    fire->mLightExpansion = 1.0f;
    return fire;
}
VERIFY(0x024C0898, tnEnemyFire_construct);

void tn_class_destroy(tn_class* actor, int flags) {
    WWHD_FUNC(0x024C6C00, void, actor, flags);
    if (actor == nullptr) return;
    const u32 base = gabi::ea(actor);

    // Embedded fire and ice collision objects precede their background checks.
    gabi::call(0x02515AE8, gabi::at<u8>(base + 0x1B08), 2);
    gabi::call(0x02515860, gabi::at<u8>(base + 0x1ACC), 2);
    gabi::store<u32>(base + 0x1888, 0x10040420);
    gabi::store<u32>(base + 0x187C, 0x10040430);
    gabi::call(0x024EFD9C, gabi::at<u8>(base + 0x1868), 0);
    gabi::call(0x02018034, gabi::at<u8>(base + 0x183C), 2);
    gabi::call(0x02515A70, gabi::at<u8>(base + 0x16E0), 2);
    gabi::call(0x02515860, gabi::at<u8>(base + 0x16A4), 2);

    // Native array cleanup: three 0x10-byte cut-turn callbacks.
    gabi::call(0x028F0164, gabi::at<u8>(base + 0x159C), 3, 0x10,
               gabi::at<void>(0x024C6BEC), 0, 0);
    gabi::call(0x02515AE8, &actor->mWepon2Sph, 2);
    gabi::call(0x02515AE8, &actor->mWeponSph, 2);
    gabi::call(0x02515AE8, &actor->mDefenceSph, 2);
    gabi::call(0x02515AE8, &actor->mHeadSph, 2);
    gabi::call(0x02515A70, &actor->mTgCyl, 2);
    gabi::call(0x02515A70, &actor->mCoCyl, 2);

    // Damage-reaction status/background subobjects and the actor base.
    gabi::call(0x02515860, gabi::at<u8>(base + 0xCE8), 2);
    gabi::store<u32>(base + 0xB00, 0x10040420);
    gabi::store<u32>(base + 0xAF4, 0x10040430);
    gabi::call(0x024EFD9C, gabi::at<u8>(base + 0xAE0), 0);
    gabi::call(0x02018034, gabi::at<u8>(base + 0xAB4), 2);
    gabi::call(0x025D50BC, actor, 0);
    if ((static_cast<u32>(flags) & 1) != 0) {
        gabi::call(0x0273AF40, actor);
    }
}
VERIFY(0x024C6C00, tn_class_destroy);

static void tn_removeParticleCallback(u32 callbackAddress) {
    const u32 table = gabi::load<u32>(callbackAddress);
    const u32 remove = gabi::load<u32>(table + 0x44u);
    gabi::call(remove, gabi::at<u8>(callbackAddress));
}

BOOL daTn_Delete(tn_class* actor) {
    WWHD_FUNC(0x024C0194, BOOL, actor);
    dComIfG_resDelete(&actor->mPhaseTkwn, STR(0x1004069C));
    dComIfG_resDelete(&actor->mPhaseTn, STR(0x100406A4));
    if (actor->mHioRegistered != 0) {
        const s8 hio = gabi::load<s8>(0x1046E778);
        gabi::store<u8>(0x101D2468, 0);
        gabi::call(0x025F0A18, hio);
    }
    const u32 base = gabi::ea(actor);
    gabi::call(0x025A9D64, gabi::at<u8>(base + 0x159C));
    gabi::call(0x025A9D64, gabi::at<u8>(base + 0x15AC));
    gabi::call(0x025A9D64, gabi::at<u8>(base + 0x15BC));
    tn_removeParticleCallback(base + 0x550);
    tn_removeParticleCallback(base + 0x570);
    tn_removeParticleCallback(base + 0xD38);
    if (actor->heap != nullptr) {
        mDoExt_McaMorf* model = actor->mpBodyMorf;
        model->stopZelAnime();
    }
    const u32 mantleId = actor->mMantPcId;
    if (mantleId != 0xFFFFu) {
        gabi::Local<be<u32>> searchId;
        *searchId = mantleId;
        fopAc_ac_c* mantle = nullptr;
        if (mantleId != 0xFFFFFFFFu) {
            mantle = gabi::call<fopAc_ac_c*>(0x025D5218,
                gabi::at<void>(0x025E1234), searchId.get());
        }
        if (mantle != nullptr) gabi::call(0x025D57E0, mantle);
    }
    if (actor->health == -128 && (static_cast<u32>(actor->actor_status) & 0x04000000u) != 0) {
        if (gabi::load<s8>(0x1046E73D) == 0) gabi::call(0x025E1928);
        else gabi::store<u8>(0x1046E73D, 0);
    }
    gabi::call(0x02041C30, &actor->mEnemyFire);
    return 1;
}
VERIFY(0x024C0194, daTn_Delete);

static void tn_constructCylinder(u32 address) {
    gabi::call(0x02515FB8, gabi::at<dCcD_Cyl>(address));
    gabi::store<u32>(address + 0x110u, 0x100403A0);
    gabi::store<u32>(address + 0x114u, 0x100015A8);
    gabi::call(0x02018590, gabi::at<u8>(address + 0x118u));
    gabi::store<u32>(address + 0x12Cu, 0x1004B150);
    gabi::store<u32>(address + 0x3Cu, 0x1004B108);
    gabi::store<u32>(address + 0x114u, 0x1004B160);
}

tn_class* tn_class_construct(tn_class* actor) {
    WWHD_FUNC(0x024C0924, tn_class*, actor);
    if (actor == nullptr) {
        actor = gabi::call<tn_class*>(0x0273AD10, 0x1C5C);
        if (actor == nullptr) return nullptr;
    }
    const u32 base = gabi::ea(actor);
    fopAc_ac_c_ct(actor);
    actor->__vtbl = 0x10040490;
    gabi::call(0x025A5B18, gabi::at<u8>(base + 0x550), 1);
    gabi::call(0x025A5B18, gabi::at<u8>(base + 0x570), 1);

    // Damage-reaction background and collision status subobjects.
    gabi::call(0x024EFE94, gabi::at<u8>(base + 0xAA0));
    gabi::call(0x024F0474, gabi::at<u8>(base + 0xAE0));
    gabi::store<u32>(base + 0xAF0, 0x10040410);
    gabi::store<u32>(base + 0xAF4, 0x10040430);
    gabi::store<u8>(base + 0xAF8, 1);
    gabi::store<u32>(base + 0xB00, 0x10040420);
    gabi::call(0x0200BD2C, gabi::at<u8>(base + 0xCE8));
    gabi::call(0x02515DA0, gabi::at<u8>(base + 0xD04));
    gabi::store<u32>(base + 0xD00, 0x1004AE88);
    gabi::store<u32>(base + 0xD04, 0x1004AEC0);
    gabi::call(0x025A5B18, gabi::at<u8>(base + 0xD38), 1);

    tn_constructCylinder(base + 0xDC4);
    tn_constructCylinder(base + 0xEF4);
    gabi::call(0x025166F0, &actor->mHeadSph);
    gabi::call(0x025166F0, &actor->mDefenceSph);
    gabi::call(0x025166F0, &actor->mWeponSph);
    gabi::call(0x025166F0, &actor->mWepon2Sph);
    gabi::call(0x028EFFD0, gabi::at<u8>(base + 0x159C), 3, 0x10,
               gabi::at<void>(0x024C6BAC));

    // Ice status/cylinder and background checks; fire is a separate constructor.
    gabi::call(0x0200BD2C, gabi::at<u8>(base + 0x16A4));
    gabi::call(0x02515DA0, gabi::at<u8>(base + 0x16C0));
    gabi::store<u32>(base + 0x16BC, 0x1004AE88);
    gabi::store<u32>(base + 0x16C0, 0x1004AEC0);
    tn_constructCylinder(base + 0x16E0);
    gabi::call(0x024EFE94, gabi::at<u8>(base + 0x1828));
    gabi::call(0x024F0474, gabi::at<u8>(base + 0x1868));
    gabi::store<u8>(base + 0x1880, 1);
    gabi::store<u32>(base + 0x1888, 0x10040420);
    gabi::store<u32>(base + 0x187C, 0x10040430);
    gabi::store<u32>(base + 0x1878, 0x10040410);
    tnEnemyFire_construct(&actor->mEnemyFire);
    return actor;
}
VERIFY(0x024C0924, tn_class_construct);

void part_draw(tn_class* actor, int frozen) {
    WWHD_FUNC(0x024BA0F0, void, actor, frozen);
    for (int index = 0; index < 3; ++index) {
        tn_p* part = &actor->mParts[index];
        if (part->mState < 0) continue;
        if (index == 0) {
            mDoExt_McaMorf* armor = actor->mpArmorMorf;
            if (frozen != 0) {
                gabi::call(0x0259138C, armor, -1, nullptr);
                continue;
            }
            J3DModel* model = armor->getModel();
            mDoExt_brkAnm* animation = actor->mParts[0].mpPartBrkAnm;
            J3DModelData* data = J3DModel_getModelData(model);
            const f32 frame = gabi::load<f32>(gabi::ea(animation) + 4u);
            mDoExt_brkAnm_entry(animation, data, frame);
            void* light = gabi::call<void*>(0x02555D0C);
            gabi::call(0x02562F5C, light, model, &actor->tevStr);
            armor = actor->mpArmorMorf;
            armor->entryDL();
            continue;
        }
        if (index == 2 && (static_cast<u8>(actor->mRemainingEquipmentPieces) & 4) == 0) continue;
        J3DModel* model = part->mpPartModel;
        if (frozen != 0) {
            gabi::call(0x02591200, model, -1, nullptr);
            continue;
        }
        mDoExt_brkAnm* animation = part->mpPartBrkAnm;
        J3DModelData* data = J3DModel_getModelData(model);
        const f32 frame = gabi::load<f32>(gabi::ea(animation) + 4u);
        mDoExt_brkAnm_entry(animation, data, frame);
        void* light = gabi::call<void*>(0x02555D0C);
        gabi::call(0x02562F5C, light, model, &actor->tevStr);
        gabi::call(0x025E2DE0, model, 0);
    }
}
VERIFY(0x024BA0F0, part_draw);

BOOL daTn_player_way_check(tn_class* actor) {
    WWHD_FUNC(0x024BB5C4, BOOL, actor);
    const u32 play = gabi::ea(dComIfGp_get());
    const u32 player = gabi::load<u32>(play + 0x5B2Cu);
    const s16 facing = actor->current.angle.y;
    const s16 playerFacing = gabi::load<s16>(player + 0x32Au);
    const s16 difference = static_cast<s16>(static_cast<s32>(facing) - playerFacing);
    const u16 magnitude = difference < 0
        ? static_cast<u16>(-static_cast<s32>(difference))
        : static_cast<u16>(difference);
    return magnitude >= 0x4000;
}
VERIFY(0x024BB5C4, daTn_player_way_check);

BOOL daTn_Draw(tn_class* actor) {
    WWHD_FUNC(0x024BA238, BOOL, actor);
    const u32 base = gabi::ea(actor);
    const s8 drawPaused = actor->mDrawPaused;
    mDoExt_McaMorf* body = actor->mpBodyMorf;
    J3DModel* model = body->getModel();
    if (drawPaused != 0 || actor->m02C1 != 0) return 1;
    if (actor->mEnemyIce.mFreezeTimer > 20) {
        gabi::call(0x0259138C, body, -1, nullptr);
        part_draw(actor, 1);
        return 1;
    }

    const u8 equipmentType = actor->mEquipmentType;
    int figure = 0xC1;
    if (equipmentType < 4) {
        figure = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 4) != 0
            ? 0xC0 : 0xBF;
    }
    gabi::call(0x025BED80, figure, actor, 1.0f, 1.0f, 1.0f);
    void* light = gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C, light, model, &actor->tevStr);
    mDoExt_brkAnm* animation = actor->mpBrkAnm;
    J3DModelData* modelData = J3DModel_getModelData(model);
    const f32 frame = gabi::load<f32>(gabi::ea(animation) + 4u);
    mDoExt_brkAnm_entry(animation, modelData, frame);

    // HD resolves the body material by name in its model-data material array.
    gabi::Local<SafeString> materialName;
    materialName->__vtbl = TN_SAFESTRING_VTABLE;
    materialName->mStringTop = 0x100405A0;
    const u32 data = gabi::ea(J3DModel_getModelData(model));
    const u32 resource = gabi::load<u32>(data);
    tnSafeString_noop(materialName.get());
    const u32 nameOffset = gabi::load<u32>(resource + 0x18u);
    const u32 nameTable = nameOffset == 0 ? 0u : resource + 0x18u + nameOffset;
    const u32 name = materialName->mStringTop;
    const s32 index = gabi::call<s32>(0x027DF9B0, gabi::at<u8>(nameTable), STR(name));
    u32 material = 0;
    if (index >= 0) {
        const u32 count = gabi::load<u32>(data + 0xCu);
        material = gabi::load<u32>(data + 0x10u);
        if (static_cast<u32>(index) < count) material += static_cast<u32>(index) * 0x39Cu;
    }
    const s16 debugHide = gabi::load<s16>(0x1047B68A);
    const u32 shape = gabi::load<u32>(material + 8u);
    bool hidden = debugHide != 0;
    if (!hidden) hidden = gabi::load<s8>(base + 0x1550) != 0;
    gabi::store<u8>(shape + 4u, hidden ? 0 : 1);
    body = actor->mpBodyMorf;
    body->entryDL();
    part_draw(actor, 0);
    return 1;
}
VERIFY(0x024BA238, daTn_Draw);

void fail(tn_class* actor) {
    WWHD_FUNC(0x024C2AAC, void, actor);
    const u32 base = gabi::ea(actor);
    gabi::store<u32>(base + 0x39Cu, 0);
    gabi::store<s16>(base + 0x508u, 5);
    const s16 mode = actor->mActionMode;
    actor->speedF = 0.0f;
    actor->speed.y = 0.0f;
    if (mode != 0) return;

    gabi::Local<cXyz> dropPosition;
    dropPosition->x = actor->current.pos.x;
    const f32 height = gabi::load<f32>(0x1046E78C) + 100.0f;
    const f32 actorY = actor->current.pos.y;
    dropPosition->y = actorY + height;
    dropPosition->z = actor->current.pos.z;
    const s16 timer = actor->mCountDownTimers[2];
    const u8 itemBits = actor->stealItemBitNo;
    gabi::call(0x025D99E8, actor, dropPosition.get(), 10, timer >= 1000 ? 1 : 0, itemBits);

    if (gabi::load<s8>(base + 0x1648u) != 0) {
        const s16 oldMode = actor->mActionMode;
        const u8 switchNo = actor->mDisableSpawnOnDeathSwitch;
        actor->mDrawPaused = 1;
        actor->mActionMode = static_cast<s16>(static_cast<s32>(oldMode) + 1);
        if (switchNo != 0) {
            const u32 save = gabi::load<u32>(0x101F84DC);
            const s8 room = actor->current.roomNo;
            gabi::call(0x025B9E38, gabi::at<u8>(save + 0x20u), switchNo, room);
        }
    } else {
        gabi::call(0x025D57E0, actor);
        const u8 switchNo = actor->mDisableSpawnOnDeathSwitch;
        if (switchNo != 0) {
            const u32 save = gabi::load<u32>(0x101F84DC);
            const s8 room = actor->current.roomNo;
            gabi::call(0x025B9E38, gabi::at<u8>(save + 0x20u), switchNo, room);
        }
    }
    const s8 homeRoom = actor->home.roomNo;
    const u32 save = gabi::load<u32>(0x101F84DC);
    const u16 actorId = actor->setID;
    gabi::call(0x025BA5D4, gabi::at<u8>(save + 0x20u), actorId, homeRoom);
    actor->mActionMode = 1;
}

VERIFY(0x024C2AAC, fail);

void aite_miru(tn_class* actor) {
    WWHD_FUNC(0x024C29CC, void, actor);
    const s16 mode = actor->mActionMode;
    if (mode == 0) {
        const s16 delay = gabi::load<s16>(0x1047B698);
        actor->speedF = 0.0f;
        actor->mCountDownTimers[1] = static_cast<s16>(static_cast<s32>(delay) + 20);
        actor->mActionMode = 1;
    } else if (mode != 1) {
        return;
    }
    const u32 base = gabi::ea(actor);
    gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x1554), 12000, 2, 6144);
    const u32 targetId = gabi::load<u32>(base + 0x1558);
    gabi::store<u8>(base + 0xCD4, 1);
    if (targetId != 0xFFFFFFFFu) {
        gabi::Local<be<u32>> searchId;
        *searchId = targetId;
        fopAc_ac_c* target = gabi::call<fopAc_ac_c*>(0x025D5218,
            gabi::at<void>(0x025E1234), searchId.get());
        if (target != nullptr) gabi::store<u32>(base + 0xCD8, gabi::ea(target));
    }
    if (actor->mCountDownTimers[1] == 0) {
        gabi::store<u32>(base + 0x1558, 0xFFFFFFFFu);
        actor->mAction = 0;
        actor->mActionMode = 0;
        gabi::call(0x024BB800, actor);
    }
}
VERIFY(0x024C29CC, aite_miru);

void defence(tn_class* actor) {
    WWHD_FUNC(0x024C2430, void, actor);
    const u32 play = gabi::ea(dComIfGp_get());
    const u32 base = gabi::ea(actor);
    const s16 targetAngle = gabi::load<s16>(base + 0x532u);
    const u32 player = gabi::load<u32>(play + 0x5B2Cu);
    gabi::store<u8>(base + 0xCD4u, 1);
    gabi::store<s16>(base + 0xA94u, targetAngle);
    gabi::call(0x0200F428, &actor->current.angle.y, targetAngle, 4, 1024);
    const s16 mode = actor->mActionMode;
    if (mode == 0) {
        const s32 pathDriven = actor->mPathDriven;
        actor->mActionMode = 1;
        const int animation = pathDriven != 0 ? 0x30 : 0x31;
        anm_init(actor, animation, 2.0f, 0, 1.0f, -1);
        tate_anm_init(actor, animation, 2.0f, 0, 1.0f);
        yoroi_anm_init(actor, 0x5E, 2.0f, 0, 1.0f);
    } else if (mode != 1) {
        return;
    }
    const s16 timer = actor->mCountDownTimers[1];
    gabi::store<u8>(base + 0x1594u, 1);
    actor->speedF = 0.0f;
    if (timer == 0 && gabi::load<u8>(player + 0x3ACu) == 0) {
        actor->mAction = 4;
        actor->mCountDownTimers[1] = 0;
        actor->mActionMode = 0;
        tate_anm_init(actor, 0x3C, 10.0f, 2, 1.0f);
    }
}
VERIFY(0x024C2430, defence);

static void tn_resumeWaiting(tn_class* actor) {
    actor->mAction = 0;
    gabi::call(0x024BB800, actor);
    wait_set(actor);
    actor->mActionMode = 2;
}

void d_sit(tn_class* actor) {
    WWHD_FUNC(0x024C2F6C, void, actor);
    const s16 mode = actor->mActionMode;
    if (mode == 0 || mode == 1) {
        if (mode == 0) {
            actor->mActionMode = 1;
            anm_init(actor, 0x34, 2.0f, 0, 1.0f, 0x0E);
            yoroi_anm_init(actor, 0x61, 2.0f, 0, 1.0f);
        }
        mDoExt_McaMorf* body = actor->mpBodyMorf;
        actor->speedF = 0.0f;
        const u8 flags = body->mFrameCtrl.mState;
        if ((flags & 1) != 0 || body->mFrameCtrl.mRate == 0.0f) tn_resumeWaiting(actor);
    } else if (mode == 10 || mode == 11) {
        if (mode == 10) {
            actor->mActionMode = 11;
            anm_init(actor, 0x37, 5.0f, 2, 2.0f, -1);
            yoroi_anm_init(actor, 0x65, 5.0f, 2, 2.0f);
            actor->mCountDownTimers[0] = 55;
        }
        const s16 timer = actor->mCountDownTimers[0];
        actor->speedF = 0.0f;
        if (timer == 0) tn_resumeWaiting(actor);
    }
}
VERIFY(0x024C2F6C, d_sit);

void d_dozou(tn_class* actor) {
    WWHD_FUNC(0x024C3118, void, actor);
    const u32 base = gabi::ea(actor);
    const s16 mode = actor->mActionMode;
    gabi::store<s16>(base + 0x508u, 5);
    gabi::store<u32>(base + 0x39Cu, 0);
    if (mode == 0 || mode == 1) {
        if (mode == 0) {
            actor->mActionMode = 1;
            gabi::call(0x02515F14, gabi::at<dCcD_Stts>(base + 0xCE8u), 0xFF, 0xFF, actor);
            if (actor->mRangeOrFrozenAnim == 0) {
                anm_init(actor, 0x37, 1.0f, 0, 0.0f, -1);
                mDoExt_McaMorf* body = actor->mpBodyMorf;
                body->mFrameCtrl.mFrame = 41.0f;
            } else {
                anm_init(actor, 0x2F, 1.0f, 0, 0.0f, 0x0C);
                mDoExt_McaMorf* body = actor->mpBodyMorf;
                body->mFrameCtrl.mFrame = 27.0f;
            }
            yoroi_anm_init(actor, 0x64, 0.0f, 0, 1.0f);
        }
        bool awaken = false;
        const u8 switchNo = actor->mEnableSpawnSwitch;
        if (switchNo != 0xFF) {
            const u32 save = gabi::load<u32>(0x101F84DC);
            const s8 room = actor->current.roomNo;
            awaken = gabi::call<BOOL>(0x025BA0C0, gabi::at<u8>(save + 0x20u), switchNo, room) != 0;
            if (!awaken && actor->mEnableSpawnSwitch != 0xFF) return;
        }
        if (!awaken) {
            const u32 save = gabi::load<u32>(0x101F84DC);
            awaken = gabi::call<BOOL>(0x025B8B94, gabi::at<u8>(save + 0x644u), 0x3802) != 0;
        }
        if (awaken) {
            mDoExt_McaMorf* body = actor->mpBodyMorf;
            body->mFrameCtrl.mRate = 1.0f;
            actor->mActionMode = 2;
        }
    } else if (mode == 2) {
        mDoExt_McaMorf* body = actor->mpBodyMorf;
        const u8 flags = body->mFrameCtrl.mState;
        if ((flags & 1) != 0 || body->mFrameCtrl.mRate == 0.0f) {
            gabi::call(0x02515F14, gabi::at<dCcD_Stts>(base + 0xCE8u), 0xF0, 0xFF, actor);
            actor->mAction = 0;
            actor->mActionMode = 0;
            gabi::store<u32>(base + 0x39Cu, 4);
        }
    }
}
VERIFY(0x024C3118, d_dozou);

BOOL daTn_player_view_check(tn_class* actor, cXyz* target, s16 heading, s16 angleLimit) {
    WWHD_FUNC(0x024BB404, BOOL, actor, target, heading, angleLimit);
    const u32 play = gabi::ea(dComIfGp_get());
    const s8 forceSearch = gabi::load<s8>(0x1046E73C);
    const u32 player = gabi::load<u32>(play + 0x5B2Cu);
    if (forceSearch != 0) return 1;
    if (gabi::call<BOOL>(0x024BB27C, actor, target) != 0) return 0;

    const f32 playerY = gabi::load<f32>(player + 0x318u);
    const f32 eyeY = actor->eyePos.y;
    const f32 heightDifference = (playerY + 50.0f) - eyeY;
    if (std::fabs(heightDifference) > gabi::load<f32>(0x1046E7AC)) return 0;

    const u32 base = gabi::ea(actor);
    const s16 actorHeading = gabi::load<s16>(base + 0x530u);
    const s16 difference = static_cast<s16>(static_cast<s32>(actorHeading) - heading);
    const u16 magnitude = difference < 0
        ? static_cast<u16>(-static_cast<s32>(difference)) : static_cast<u16>(difference);
    if (static_cast<s32>(magnitude) < angleLimit) {
        gabi::store<u8>(base + 0xCD7u, 1);
        return 1;
    }

    const s16 angle = actor->current.angle.y;
    const u32 matrix = gabi::load<u32>(0x1018C7B0);
    gabi::call(0x025F1884, gabi::at<u8>(matrix), static_cast<s16>(-static_cast<s32>(angle)));
    gabi::Local<cXyz> delta;
    delta->x = static_cast<f32>(target->x) - actor->current.pos.x;
    delta->y = static_cast<f32>(target->y) - actor->current.pos.y;
    delta->z = static_cast<f32>(target->z) - actor->current.pos.z;
    gabi::Local<cXyz> relative;
    gabi::call(0x0200FCD8, delta.get(), relative.get());
    if (std::fabs(static_cast<f32>(relative->x)) < gabi::load<f32>(0x1046E7B0) &&
        std::fabs(static_cast<f32>(relative->y)) < gabi::load<f32>(0x1046E7B4)) {
        const f32 z = relative->z;
        if (z > gabi::load<f32>(0x1046E7BC) && z < gabi::load<f32>(0x1046E7B8)) {
            gabi::store<u8>(base + 0xCD7u, 2);
            return 1;
        }
    }
    gabi::store<u8>(base + 0xCD7u, 0);
    return 0;
}
VERIFY(0x024BB404, daTn_player_view_check);

void p_lost(tn_class* actor) {
    WWHD_FUNC(0x024C18B8, void, actor);
    const u32 base = gabi::ea(actor);
    gabi::call(0x025200D4, actor);
    const s16 mode = actor->mActionMode;
    gabi::store<u8>(base + 0xCD4u, 0);
    if (mode == 0) {
        actor->mActionMode = 1;
        anm_init(actor, 0x37, 5.0f, 0, 1.0f, -1);
        yoroi_anm_init(actor, 0x65, 5.0f, 0, 1.0f);
        actor->speedF = 0.0f;
    } else if (mode == 1) {
        mDoExt_McaMorf* body = actor->mpBodyMorf;
        const u8 flags = body->mFrameCtrl.mState;
        if ((flags & 1) != 0 || body->mFrameCtrl.mRate == 0.0f) {
            actor->mAction = 0;
            gabi::call(0x024BB800, actor);
            wait_set(actor);
            actor->mActionMode = 0;
        }
    }

    mDoExt_McaMorf* body = actor->mpBodyMorf;
    // Truncating/saturating a frame to an integer and comparing it with 25
    // is equivalent to this comparison, including NaN and infinite frames.
    // This avoids an undefined host float-to-integer conversion.
    if (static_cast<f32>(body->mFrameCtrl.mFrame) >= 26.0f) {
        const s16 angleLimit = get_view_H(actor);
        const u32 target = gabi::load<u32>(base + 0xCD8u);
        const s16 heading = gabi::load<s16>(base + 0x532u);
        if (daTn_player_view_check(actor, gabi::at<cXyz>(target + 0x314u),
                                  heading, angleLimit) != 0) {
            actor->mCountDownTimers[1] = 0;
            actor->mAction = 4;
            actor->mActionMode = -10;
        }
    }
}
VERIFY(0x024C18B8, p_lost);

BOOL nodeCallBack_kata(J3DNode* node, int stage) {
    WWHD_FUNC(0x024B9FF8, BOOL, node, stage);
    if (stage != 0) return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    const u32 model = gabi::load<u32>(0x104B462C);
    const u32 actor = gabi::load<u32>(model + 0xB8u);
    const u32 bodyMorf = gabi::load<u32>(actor + 0x3E0u);
    const u32 bodyModel = gabi::load<u32>(bodyMorf + 0x90u);
    const u32 bodyBuffer = gabi::load<u32>(bodyModel + 0x2Cu);
    const u16 bodyFlags = gabi::load<u16>(bodyBuffer + 4u);
    const u32 bodyMatrices = gabi::load<u32>(bodyBuffer + 0x10u);
    const u16 jointIndex = gabi::load<u16>(gabi::ea(joint) + 4u);
    gabi::store<u16>(bodyBuffer + 4u, bodyFlags | 0x10u);

    const u32 buffer = gabi::load<u32>(model + 0x2Cu);
    const u16 flags = gabi::load<u16>(buffer + 4u);
    const u32 matrices = gabi::load<u32>(buffer + 0x10u);
    const u32 matrixOffset = static_cast<u32>(jointIndex) * 0x30u;
    gabi::store<u16>(buffer + 4u, flags | 0x10u);
    mtx_copy(gabi::at<Mtx34>(matrices + matrixOffset),
             gabi::at<Mtx34>(bodyMatrices + matrixOffset));

    const u32 refreshedBuffer = gabi::load<u32>(model + 0x2Cu);
    const u32 refreshedMatrices = gabi::load<u32>(refreshedBuffer + 0x10u);
    const u16 refreshedFlags = gabi::load<u16>(refreshedBuffer + 4u);
    gabi::store<u16>(refreshedBuffer + 4u, refreshedFlags | 0x10u);
    gabi::call(0x028E90D4, gabi::at<Mtx34>(refreshedMatrices + matrixOffset),
               gabi::at<Mtx34>(0x104B4868));
    return 1;
}
VERIFY(0x024B9FF8, nodeCallBack_kata);

BOOL nodeCallBack_mimi(J3DNode* node, int stage) {
    WWHD_FUNC(0x024B9ED0, BOOL, node, stage);
    if (stage != 0) return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    const u32 model = gabi::load<u32>(0x104B462C);
    const u32 actor = gabi::load<u32>(model + 0xB8u);
    const u16 jointIndex = gabi::load<u16>(gabi::ea(joint) + 4u);
    if (actor == 0 || (gabi::load<u8>(actor + 0x3ECu) & 2u) == 0) return 1;

    const u32 buffer = gabi::load<u32>(model + 0x2Cu);
    const u32 matrixOffset = static_cast<u32>(jointIndex) * 0x30u;
    const u16 flags = gabi::load<u16>(buffer + 4u);
    const u32 matrices = gabi::load<u32>(buffer + 0x10u);
    gabi::store<u16>(buffer + 4u, flags | 0x10u);
    const u32 stackMatrix = gabi::load<u32>(0x1018C7B0);
    gabi::call(0x028E90D4, gabi::at<Mtx34>(matrices + matrixOffset),
               gabi::at<Mtx34>(stackMatrix));
    mDoMtx_YrotM(gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), -32768);

    const u32 refreshedBuffer = gabi::load<u32>(model + 0x2Cu);
    const u16 refreshedFlags = gabi::load<u16>(refreshedBuffer + 4u);
    const u32 refreshedStack = gabi::load<u32>(0x1018C7B0);
    const u32 refreshedMatrices = gabi::load<u32>(refreshedBuffer + 0x10u);
    gabi::store<u16>(refreshedBuffer + 4u, refreshedFlags | 0x10u);
    mtx_copy(gabi::at<Mtx34>(refreshedMatrices + matrixOffset),
             gabi::at<Mtx34>(refreshedStack));
    gabi::call(0x028E90D4, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)),
               gabi::at<Mtx34>(0x104B4868));
    return 1;
}
VERIFY(0x024B9ED0, nodeCallBack_mimi);

static void tn_constructLineCheck(tn_linecheck* check) {
    cBgS_LinChk_ct(check);
    for (be<u8>& flag : check->mPassFlags) flag = 0;
    check->mGroupMask = 1;
    const u32 base = gabi::ea(check);
    gabi::store<u32>(base, base + 0x58u);
    gabi::store<u32>(base + 4u, base + 0x64u);
    check->mCheckVtable = 0x10040440;
    check->mLineVtable = 0x10040450;
    check->mPolyPassVtable = 0x10040470;
    check->mGroupPassVtable = 0x10040460;
}

static void tn_prepareLineCheckDestruction(tn_linecheck* check) {
    check->mPolyPassVtable = 0x10040470;
    check->mGroupPassVtable = 0x100403C0;
    check->mLineVtable = 0x100403B0;
}

BOOL daTn_other_bg_check(tn_class* actor, fopAc_ac_c* target) {
    WWHD_FUNC(0x024BA8F8, BOOL, actor, target);
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    BOOL blocked = 1;
    if (target != nullptr) {
        gabi::Local<cXyz> endpoint;
        endpoint->x = target->current.pos.x;
        endpoint->y = static_cast<f32>(target->current.pos.y) + gabi::load<f32>(0x100405AC);
        endpoint->z = target->current.pos.z;
        gabi::Local<cXyz> startpoint;
        startpoint->x = actor->current.pos.x;
        startpoint->y = actor->current.pos.y;
        startpoint->z = actor->current.pos.z;
        startpoint->y = actor->eyePos.y;
        dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), actor);
        const u32 play = gabi::ea(dComIfGp_get());
        blocked = cBgS_LineCross(gabi::at<dBgS>(play + 0x12A0u), check.get()) != 0;
    }
    tn_prepareLineCheckDestruction(check.get());
    cBgS_LinChk_dt(check.get(), 0);
    return blocked;
}
VERIFY(0x024BA8F8, daTn_other_bg_check);

BOOL daTn_player_bg_check(tn_class* actor, cXyz* target) {
    WWHD_FUNC(0x024BB27C, BOOL, actor, target);
    dComIfGp_get();
    if (gabi::load<s8>(0x1046E73C) != 0) return 0;
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    gabi::Local<cXyz> endpoint;
    endpoint->x = target->x;
    endpoint->y = static_cast<f32>(target->y) + gabi::load<f32>(0x100405C8);
    endpoint->z = target->z;
    gabi::Local<cXyz> startpoint;
    startpoint->x = actor->current.pos.x;
    startpoint->y = actor->current.pos.y;
    startpoint->z = actor->current.pos.z;
    startpoint->y = actor->eyePos.y;
    dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), actor);
    const u32 play = gabi::ea(dComIfGp_get());
    const BOOL blocked = cBgS_LineCross(gabi::at<dBgS>(play + 0x12A0u), check.get()) != 0;
    tn_prepareLineCheckDestruction(check.get());
    if (blocked != 0) gabi::store<u8>(gabi::ea(actor) + 0xCD7u, 0);
    cBgS_LinChk_dt(check.get(), 0);
    return blocked;
}
VERIFY(0x024BB27C, daTn_player_bg_check);

void path_check(tn_class* actor) {
    WWHD_FUNC(0x024BB800, void, actor);
    tn_path* path = actor->mpPath;
    if (path == nullptr) return;
    if (actor->mPathDriven == 0 && actor->mPathSearchEnabled == 0) return;
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    gabi::Local<cXyz> startpoint;
    const f32 height = gabi::load<f32>(0x100404F4);
    startpoint->x = actor->current.pos.x;
    startpoint->y = static_cast<f32>(actor->current.pos.y) + height;
    startpoint->z = actor->current.pos.z;
    path = actor->mpPath;
    u32 point = gabi::ea(static_cast<u8*>(path->mPoints));
    u16 count = path->mPointCount;
    gabi::Local<cXyz> endpoint;  // one frame slot, reused by every iteration
    for (u32 i = 0; i < count; ++i, point += 0x10u) {
        endpoint->x = gabi::load<f32>(point + 4u);
        endpoint->y = gabi::load<f32>(point + 8u) + height;
        endpoint->z = gabi::load<f32>(point + 0xCu);
        dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), actor);
        const u32 play = gabi::ea(dComIfGp_get());
        const BOOL blocked = cBgS_LineCross(gabi::at<dBgS>(play + 0x12A0u), check.get());
        gabi::store<u8>(0x1046E87Cu + i, static_cast<u32>(blocked) ^ 1u);
        path = actor->mpPath;
        count = path->mPointCount;
    }
    if (count != 0) point = gabi::ea(static_cast<u8*>(path->mPoints));

    f32 radius = 0.0f;
    const f32 radiusStep = gabi::load<f32>(0x100405AC);
    bool found = false;
    for (int pass = 0; pass < 100 && !found; ++pass) {
        for (u32 i = 0; i < count; ++i, point += 0x10u) {
            if (gabi::load<u8>(0x1046E87Cu + i) == 0) continue;
            const f32 dy = static_cast<f32>(actor->current.pos.y) - gabi::load<f32>(point + 8u);
            const f32 dx = static_cast<f32>(actor->current.pos.x) - gabi::load<f32>(point + 4u);
            const f32 dz = static_cast<f32>(actor->current.pos.z) - gabi::load<f32>(point + 0xCu);
            const f32 dySquared = dy * dy;
            const f32 xySquared = gabi::fmadds(dx, dx, dySquared);
            const f32 squaredDistance = gabi::fmadds(dz, dz, xySquared);
            const f32 distance = gabi::call<f32>(0x028F4384, squaredDistance);
            path = actor->mpPath;
            count = path->mPointCount;
            if (distance < radius) {
                const s8 selected = static_cast<s8>(i - static_cast<u32>(actor->mPathPointLookahead));
                actor->mPathPoint = selected;
                if (selected >= static_cast<s8>(count)) actor->mPathPoint = static_cast<s8>(count);
                else if (selected < 0) actor->mPathPoint = 0;
                actor->mActivePath = static_cast<u8>(static_cast<u32>(actor->mPathIndex) + 1u);
                found = true;
                break;
            }
        }
        radius += radiusStep;
        if (!found && pass < 99) point = gabi::ea(static_cast<u8*>(path->mPoints));
    }
    tn_prepareLineCheckDestruction(check.get());
    if (!found) actor->mActivePath = 0;
    cBgS_LinChk_dt(check.get(), 0);
}
VERIFY(0x024BB800, path_check);

BOOL search_wepon(tn_class* actor) {
    WWHD_FUNC(0x024BAFAC, BOOL, actor);
    const u32 base = gabi::ea(actor);
    if (gabi::load<s16>(base + 0x514u) != 0) return 0;
    gabi::store<u32>(0x1046E718, 0);
    for (u32 i = 0; i < 10; ++i) gabi::store<u32>(0x1046E740u + 4u * i, 0);
    gabi::call(0x025DE508, 0x024BAA8Cu, actor);
    s32 count = gabi::load<s32>(0x1046E718);
    const f32 radiusStep = gabi::load<f32>(0x100405AC);
    f32 radius = radiusStep;
    if (count <= 0) {
        actor->mWeaponActorId = 0xFFFFFFFF;
        return 0;
    }
    const f32 maxRadius = gabi::load<f32>(0x100405C0);
    u32 index = 0;
    gabi::Local<cXyz> delta;     // frame slots reused by every iteration
    gabi::Local<cXyz> relative;
    while (static_cast<s32>(index) < count) {
        const u32 candidate = gabi::load<u32>(0x1046E740u + index * 4u);
        delta->x = gabi::load<f32>(candidate + 0x314u) - actor->eyePos.x;
        delta->y = (gabi::load<f32>(candidate + 0x318u) + radiusStep) - actor->eyePos.y;
        delta->z = gabi::load<f32>(candidate + 0x31Cu) - actor->eyePos.z;
        const f32 dx = delta->x;
        const f32 dz = delta->z;
        const f32 squaredDistance = gabi::fmadds(dx, dx, dz * dz);
        const f32 distance = gabi::call<f32>(0x028F4384, squaredDistance);
        bool visible = false;
        if (distance < radius && daTn_other_bg_check(actor, gabi::at<fopAc_ac_c>(candidate)) == 0) {
            const f32 height = (gabi::load<f32>(candidate + 0x318u) + radiusStep) - actor->eyePos.y;
            if (!(std::fabs(height) > gabi::load<f32>(0x1046E7AC))) {
                const s16 heading = gabi::call<s16>(0x020195B0, dx, dz);
                const s16 actorHeading = gabi::load<s16>(base + 0x530u);
                const s16 difference = static_cast<s16>(static_cast<s32>(actorHeading) - heading);
                const u16 magnitude = difference < 0 ? static_cast<u16>(-static_cast<s32>(difference))
                                                    : static_cast<u16>(difference);
                if (magnitude < 0x1800) visible = true;
                else {
                    const s16 angle = actor->current.angle.y;
                    gabi::call(0x025F1884, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)),
                               static_cast<s16>(-static_cast<s32>(angle)));
                    gabi::call(0x0200FCD8, delta.get(), relative.get());
                    if (std::fabs(static_cast<f32>(relative->x)) < gabi::load<f32>(0x1046E7B0) &&
                        std::fabs(static_cast<f32>(relative->y)) < gabi::load<f32>(0x1046E7B4)) {
                        const f32 z = relative->z;
                        visible = z > gabi::load<f32>(0x1046E7BC) && z < gabi::load<f32>(0x1046E7B8);
                    }
                }
            }
        }
        if (visible) {
            if (candidate == 0) break;
            const u32 id = gabi::load<u32>(candidate + 4u);
            actor->mWeaponActorId = id;
            if (id == 0xFFFFFFFF) return 0;
            gabi::Local<be<u32>> searchId;
            *searchId = id;
            return gabi::call<u32>(0x025D5218, 0x025E1234u, searchId.get()) != 0;
        }
        count = gabi::load<s32>(0x1046E718);
        ++index;
        if (static_cast<s32>(index) > count) break;
        if (static_cast<s32>(index) == count) {
            radius += radiusStep;
            index = 0;
            if (radius > maxRadius) break;
        }
    }
    actor->mWeaponActorId = 0xFFFFFFFF;
    return 0;
}
VERIFY(0x024BAFAC, search_wepon);

fopAc_ac_c* search_bomb(tn_class* actor, int requireView) {
    WWHD_FUNC(0x024BAB8C, fopAc_ac_c*, actor, requireView);
    const u32 base = gabi::ea(actor);
    if ((gabi::load<u16>(base + 0x156Cu) & 0x200u) == 0) return nullptr;
    gabi::store<u32>(0x1046E718, 0);
    for (u32 i = 0; i < 10; ++i) gabi::store<u32>(0x1046E740u + 4u * i, 0);
    gabi::call(0x025DE508, 0x024BAB08u, actor);
    s32 count = gabi::load<s32>(0x1046E718);
    const f32 radiusStep = gabi::load<f32>(0x100405AC);
    f32 radius = radiusStep;
    if (count <= 0) return nullptr;
    const f32 maxRadius = gabi::load<f32>(0x100405C0);
    const f32 rangeMargin = gabi::load<f32>(0x100405C4);
    u32 index = 0;
    gabi::Local<cXyz> delta;     // frame slots reused by every iteration
    gabi::Local<cXyz> relative;
    while (static_cast<s32>(index) < count) {
        const u32 candidate = gabi::load<u32>(0x1046E740u + index * 4u);
        delta->x = gabi::load<f32>(candidate + 0x314u) - actor->current.pos.x;
        delta->y = (gabi::load<f32>(candidate + 0x318u) + radiusStep) - actor->eyePos.y;
        delta->z = gabi::load<f32>(candidate + 0x31Cu) - actor->current.pos.z;
        const f32 dx = delta->x;
        const f32 dz = delta->z;
        const f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
        if (distance < radius) {
            const f32 limit = gabi::load<f32>(base + 0x518u) + rangeMargin;
            if (!(distance > limit)) {
                const BOOL blocked = daTn_other_bg_check(actor, gabi::at<fopAc_ac_c>(candidate));
                // Native mode zero performs the line check but returns this bomb
                // without using its result. View mode additionally filters it.
                if (requireView == 0) return gabi::at<fopAc_ac_c>(candidate);
                if (blocked == 0) {
                    const f32 height = (gabi::load<f32>(candidate + 0x318u) + radiusStep) - actor->eyePos.y;
                    if (!(std::fabs(height) > gabi::load<f32>(0x1046E7AC))) {
                        const s16 heading = gabi::call<s16>(0x020195B0, dx, dz);
                        const s16 actorHeading = gabi::load<s16>(base + 0x530u);
                        const s16 difference = static_cast<s16>(static_cast<s32>(actorHeading) - heading);
                        const u16 magnitude = difference < 0 ? static_cast<u16>(-static_cast<s32>(difference))
                                                            : static_cast<u16>(difference);
                        const s16 angleLimit = get_view_H(actor);
                        if (static_cast<s32>(magnitude) < angleLimit) return gabi::at<fopAc_ac_c>(candidate);
                        const s16 angle = actor->current.angle.y;
                        gabi::call(0x025F1884, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)),
                                   static_cast<s16>(-static_cast<s32>(angle)));
                        gabi::call(0x0200FCD8, delta.get(), relative.get());
                        if (std::fabs(static_cast<f32>(relative->x)) < gabi::load<f32>(0x1046E7B0) &&
                            std::fabs(static_cast<f32>(relative->y)) < gabi::load<f32>(0x1046E7B4)) {
                            const f32 z = relative->z;
                            if (z > gabi::load<f32>(0x1046E7BC) && z < gabi::load<f32>(0x1046E7B8))
                                return gabi::at<fopAc_ac_c>(candidate);
                        }
                    }
                }
            }
        }
        count = gabi::load<s32>(0x1046E718);
        ++index;
        if (static_cast<s32>(index) > count) break;
        if (static_cast<s32>(index) == count) {
            radius += radiusStep;
            index = 0;
            if (radius > maxRadius) break;
        }
    }
    return nullptr;
}
VERIFY(0x024BAB8C, search_bomb);

void yogan_fail(tn_class* actor) {
    WWHD_FUNC(0x024C2BEC, void, actor);
    const u32 base = gabi::ea(actor);
    gabi::store<s16>(base + 0x508u, 5);
    const s16 mode = actor->mActionMode;
    gabi::store<u32>(base + 0x39Cu, 0);
    actor->speedF = 0.0f;
    if (mode != 0 && mode != 1) return;
    if (mode == 0) {
        const s32 pathDriven = actor->mPathDriven;
        actor->mActionMode = 1;
        const f32 upwardSpeed = gabi::load<f32>(0x1047B65C) + gabi::load<f32>(0x10040760);
        gabi::store<f32>(base + 0xA20u, 0.0f);
        gabi::store<f32>(base + 0xA14u, 0.0f);
        actor->speed.y = upwardSpeed;
        if (pathDriven != 0) gabi::store<u32>(base + 0xD88u, 1);
    }
    dComIfGp_particle_setSimple(0x8061, &actor->current.pos);
    dComIfGp_particle_setSimple(0x8058, &actor->current.pos);
    if ((gabi::load<u32>(base + 0x4F0u) & 3u) == 0) {
        const f32 randomAngle = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040764));
        gabi::store<s16>(base + 0x546u, static_cast<s16>(gabi::ftoi(randomAngle)));
        gabi::store<s16>(base + 0x544u, -8192);
        dComIfGp_particle_set(0x0E, gabi::at<cXyz>(base + 0x14D4u),
                              gabi::at<csXyz>(base + 0x544u));
    }
    gabi::call(0x0200F428, &actor->current.angle.x, -16384, 10, 0x200);
    if (static_cast<f32>(actor->speed.y) < 0.0f) {
        actor->mCountDownTimers[2] = 2000;
        actor->mAction = 20;
        actor->mActionMode = 0;
    }
}
VERIFY(0x024C2BEC, yogan_fail);

void smoke_set_s(tn_class* actor, f32 scale) {
    WWHD_FUNC(0x024B94C0, void, actor, scale);
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    const f32 x = actor->mSmokePosition.x;
    const f32 z = actor->mSmokePosition.z;
    const f32 y = actor->mSmokePosition.y;
    const f32 height = gabi::load<f32>(0x100404F4);
    gabi::Local<cXyz> startpoint;
    gabi::Local<cXyz> endpoint;
    startpoint->x = x;
    startpoint->y = y + height;
    startpoint->z = z;
    endpoint->x = x;
    endpoint->y = y - height;
    endpoint->z = z;
    dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), actor);
    const u32 play = gabi::ea(dComIfGp_get());
    u32 groundType = 0;
    if (cBgS_LineCross(gabi::at<dBgS>(play + 0x12A0u), check.get()) != 0) {
        const f32 hitX = check->mIntersection.x;
        const f32 hitY = check->mIntersection.y;
        const f32 hitZ = check->mIntersection.z;
        endpoint->x = hitX;
        actor->mSmokePosition.y = hitY;
        endpoint->z = hitZ;
        endpoint->y = hitY;
        const u32 refreshedPlay = gabi::ea(dComIfGp_get());
        groundType = gabi::call<u32>(0x024EF0F4, gabi::at<dBgS>(refreshedPlay + 0x12A0u),
                                     gabi::at<u8>(gabi::ea(check.get()) + 0x14u));
    } else {
        const f32 smokeY = actor->mSmokePosition.y;
        const f32 fall = gabi::load<f32>(0x100404F8);
        actor->mSmokePosition.y = smokeY - fall;
    }

    const u8 oldCounter = actor->mSmokeCounter;
    if (oldCounter == 0 || groundType == 4) {
        actor->mSmokeCounter = static_cast<u8>(static_cast<u32>(oldCounter) + 1u);
        if (groundType < 4 || groundType == 11) {
            dPa_smokeEcallBack_end(gabi::at<dPa_smokeEcallBack>(gabi::ea(actor) + 0x550u));
            const s8 room = actor->current.roomNo;
            const u8 alpha = gabi::load<u8>(0x1046E793);
            dPa_control_c* particle = dComIfGp_getParticle();
            JPABaseEmitter* emitter = dPa_control_set(particle, 2, 0x2022, &actor->mSmokePosition,
                &actor->mSmokeAngles, nullptr, alpha,
                gabi::at<dPa_levelEcallBack>(gabi::ea(actor) + 0x550u), room,
                nullptr, nullptr, nullptr);
            if (emitter != nullptr) {
                const u32 emitterBase = gabi::ea(emitter);
                gabi::store<f32>(emitterBase + 0x34u, scale);
                gabi::store<f32>(emitterBase + 0x58u, 1.0f);
                const f32 globalScale = gabi::load<f32>(0x1047B65C) + 1.0f;
                gabi::store<f32>(emitterBase + 0x228u, globalScale);
                gabi::store<f32>(emitterBase + 0x224u, globalScale);
                gabi::store<f32>(emitterBase + 0x220u, globalScale);
                const f32 particleScale = gabi::load<f32>(0x1047B63C) + 3.0f;
                gabi::store<f32>(emitterBase + 0x238u, particleScale);
                gabi::store<f32>(emitterBase + 0x23Cu, particleScale);
                gabi::store<f32>(emitterBase + 0x240u, particleScale);
            }
        } else if (groundType == 4) {
            JPABaseEmitter* emitter = dComIfGp_particle_set(0x24, &actor->mSmokePosition,
                                                          &actor->mSmokeAngles);
            if (emitter != nullptr) {
                const f32 waterScale = scale * gabi::load<f32>(0x10040504);
                gabi::store<u32>(gabi::ea(emitter) + 0x5Cu, 3);
                gabi::store<f32>(gabi::ea(emitter) + 0x34u, waterScale);
            }
        }
    }
    tn_prepareLineCheckDestruction(check.get());
    cBgS_LinChk_dt(check.get(), 0);
}
VERIFY(0x024B94C0, smoke_set_s);

BOOL nodeCallBack_P(J3DNode* node, int stage) {
    WWHD_FUNC(0x024B9C1C, BOOL, node, stage);
    if (stage != 0) return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    const u16 jointIndex = gabi::load<u16>(gabi::ea(joint) + 4u);
    if (jointIndex >= 33) JUT_ASSERT_fail(STR(0x10040564), 0x3F3, STR(0x10040570));
    const u32 model = gabi::load<u32>(0x104B462C);
    const u32 actor = gabi::load<u32>(model + 0xB8u);
    const s8 positionIndex = gabi::load<s8>(0x101D2558u + jointIndex);
    if (actor == 0) return 1;
    if (static_cast<u32>(jointIndex) - 11u < 7u &&
        (gabi::load<u8>(actor + 0x3ECu) & 4u) != 0 &&
        gabi::load<s16>(actor + 0x1682u) == 0) {
        const u32 shieldMorf = gabi::load<u32>(actor + 0x3E4u);
        const u32 shieldModel = gabi::load<u32>(shieldMorf + 0x90u);
        const u32 sourceBuffer = gabi::load<u32>(shieldModel + 0x2Cu);
        const u16 sourceFlags = gabi::load<u16>(sourceBuffer + 4u);
        const u32 sourceMatrices = gabi::load<u32>(sourceBuffer + 0x10u);
        gabi::store<u16>(sourceBuffer + 4u, sourceFlags | 0x10u);
        const u32 buffer = gabi::load<u32>(model + 0x2Cu);
        const u32 matrixOffset = static_cast<u32>(jointIndex) * 0x30u;
        const u16 flags = gabi::load<u16>(buffer + 4u);
        gabi::store<u16>(buffer + 4u, flags | 0x10u);
        const u32 matrices = gabi::load<u32>(buffer + 0x10u);
        mtx_copy(gabi::at<Mtx34>(matrices + matrixOffset), gabi::at<Mtx34>(sourceMatrices + matrixOffset));
        const u32 refreshedBuffer = gabi::load<u32>(model + 0x2Cu);
        const u32 refreshedMatrices = gabi::load<u32>(refreshedBuffer + 0x10u);
        const u16 refreshedFlags = gabi::load<u16>(refreshedBuffer + 4u);
        gabi::store<u16>(refreshedBuffer + 4u, refreshedFlags | 0x10u);
        gabi::call(0x028E90D4, gabi::at<Mtx34>(refreshedMatrices + matrixOffset),
                   gabi::at<Mtx34>(0x104B4868));
    }
    if (positionIndex >= 100) return 1;
    const u32 buffer = gabi::load<u32>(model + 0x2Cu);
    const u32 matrixOffset = static_cast<u32>(jointIndex) * 0x30u;
    const u16 flags = gabi::load<u16>(buffer + 4u);
    const u32 matrices = gabi::load<u32>(buffer + 0x10u);
    gabi::store<u16>(buffer + 4u, flags | 0x10u);
    gabi::call(0x028E90D4, gabi::at<Mtx34>(matrices + matrixOffset),
               gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)));

    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 0.0f;
    if (positionIndex == 17) {
        offset->x = gabi::load<f32>(0x10040554);
        offset->y = gabi::load<f32>(0x10040558);
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x14F8u));
    } else if (positionIndex == 16) {
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x1504u));
    } else if (positionIndex == 20) {
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0xD8Cu));
        offset->x = gabi::load<f32>(0x1004055C);
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x14ECu));
        offset->x = gabi::load<f32>(0x10040560);
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x14E0u));
    } else if (static_cast<u32>(static_cast<s32>(positionIndex)) >= 21u) {
        JUT_ASSERT_fail(STR(0x10040564), 0x420, STR(0x10040588));
    }
    const u32 positionOffset = static_cast<u32>(static_cast<s32>(positionIndex)) * 12u;
    gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x69Cu + positionOffset));
    return 1;
}
VERIFY(0x024B9C1C, nodeCallBack_P);

BOOL nodeCallBack(J3DNode* node, int stage) {
    WWHD_FUNC(0x024B97A0, BOOL, node, stage);
    if (stage != 0) return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    const u16 jointIndex = gabi::load<u16>(gabi::ea(joint) + 4u);
    if (jointIndex >= 33) JUT_ASSERT_fail(STR(0x10040518), 0x38A, STR(0x10040524));
    const u32 model = gabi::load<u32>(0x104B462C);
    const u32 actor = gabi::load<u32>(model + 0xB8u);
    const s8 positionIndex = gabi::load<s8>(0x101D2558u + jointIndex);
    if (actor == 0) return 1;
    const u32 matrixOffset = static_cast<u32>(jointIndex) * 0x30u;
    if (static_cast<u32>(jointIndex) - 11u < 7u &&
        (gabi::load<u8>(actor + 0x3ECu) & 4u) != 0 &&
        gabi::load<s16>(actor + 0x1682u) == 0) {
        const u32 shieldMorf = gabi::load<u32>(actor + 0x3E4u);
        const u32 shieldModel = gabi::load<u32>(shieldMorf + 0x90u);
        const u32 sourceBuffer = gabi::load<u32>(shieldModel + 0x2Cu);
        const u16 sourceFlags = gabi::load<u16>(sourceBuffer + 4u);
        const u32 sourceMatrices = gabi::load<u32>(sourceBuffer + 0x10u);
        gabi::store<u16>(sourceBuffer + 4u, sourceFlags | 0x10u);
        const u32 buffer = gabi::load<u32>(model + 0x2Cu);
        const u16 flags = gabi::load<u16>(buffer + 4u);
        gabi::store<u16>(buffer + 4u, flags | 0x10u);
        const u32 matrices = gabi::load<u32>(buffer + 0x10u);
        mtx_copy(gabi::at<Mtx34>(matrices + matrixOffset), gabi::at<Mtx34>(sourceMatrices + matrixOffset));
    }
    const u32 buffer = gabi::load<u32>(model + 0x2Cu);
    const u16 flags = gabi::load<u16>(buffer + 4u);
    const u32 matrices = gabi::load<u32>(buffer + 0x10u);
    gabi::store<u16>(buffer + 4u, flags | 0x10u);
    gabi::call(0x028E90D4, gabi::at<Mtx34>(matrices + matrixOffset),
               gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)));
    if (positionIndex < 100) {
        if (static_cast<u32>(static_cast<s32>(positionIndex)) >= 21u)
            JUT_ASSERT_fail(STR(0x10040518), 0x3AA, STR(0x1004053C));
        const u32 angleAddress = actor + 0x61Eu + static_cast<u32>(static_cast<s32>(positionIndex)) * 6u;
        gabi::call(0x025F1C28, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)),
                   gabi::load<s16>(angleAddress + 2u));
        gabi::call(0x025F1BF4, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)),
                   gabi::load<s16>(angleAddress));
        gabi::call(0x025F1C5C, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)),
                   gabi::load<s16>(angleAddress + 4u));
    }
    const u32 refreshedBuffer = gabi::load<u32>(model + 0x2Cu);
    const u32 stackMatrix = gabi::load<u32>(0x1018C7B0);
    const u16 refreshedFlags = gabi::load<u16>(refreshedBuffer + 4u);
    const u32 refreshedMatrices = gabi::load<u32>(refreshedBuffer + 0x10u);
    gabi::store<u16>(refreshedBuffer + 4u, refreshedFlags | 0x10u);
    mtx_copy(gabi::at<Mtx34>(refreshedMatrices + matrixOffset), gabi::at<Mtx34>(stackMatrix));
    gabi::call(0x028E90D4, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)),
               gabi::at<Mtx34>(0x104B4868));

    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 0.0f;
    if (positionIndex == 18) {
        offset->z = gabi::load<f32>(0x10040508);
        gabi::Local<cXyz> forwardPoint;
        gabi::call(0x0200FCD8, offset.get(), forwardPoint.get());
        offset->z = 0.0f;
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x37Cu));
        const f32 eyeX = gabi::load<f32>(actor + 0x37Cu);
        const f32 eyeY = gabi::load<f32>(actor + 0x380u);
        const f32 eyeZ = gabi::load<f32>(actor + 0x384u);
        gabi::store<f32>(actor + 0x394u, eyeY);
        gabi::store<f32>(actor + 0x398u, eyeZ);
        gabi::store<f32>(actor + 0x390u, eyeX);
        gabi::store<f32>(actor + 0x394u, eyeY + gabi::load<f32>(0x1046E798));
        const f32 headingX = static_cast<f32>(forwardPoint->x) - eyeX;
        const f32 headingZ = static_cast<f32>(forwardPoint->z) - eyeZ;
        const s16 heading = gabi::call<s16>(0x020195B0, headingX, headingZ);
        gabi::store<s16>(actor + 0x530u, heading);
        offset->x = gabi::load<f32>(0x1004050C);
        offset->z = gabi::load<f32>(0x10040510);
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x14D4u));
        offset->y = gabi::load<f32>(0x10040514);
    } else {
        if (positionIndex >= 100) return 1;
        if (static_cast<u32>(static_cast<s32>(positionIndex)) >= 21u)
            JUT_ASSERT_fail(STR(0x10040518), 0x3E3, STR(0x1004053C));
    }
    const u32 positionOffset = static_cast<u32>(static_cast<s32>(positionIndex)) * 12u;
    gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(actor + 0x69Cu + positionOffset));
    return 1;
}
VERIFY(0x024B97A0, nodeCallBack);

static void tn_constructGroundCheck(tn_groundcheck* check) {
    gabi::call(0x02008E0C, check);
    check->mCheckVtable = 0x100403D0;
    check->mGroundVtable = 0x100403E0;
    for (be<u8>& flag : check->mPassFlags) flag = 0;
    check->mPolyPassVtable = 0x10040400;
    check->mGroupPassVtable = 0x100403F0;
    check->mGroupMask = 1;
    const u32 base = gabi::ea(check);
    gabi::store<u32>(base, base + 0x40u);
    gabi::store<u32>(base + 4u, base + 0x4Cu);
}

static void tn_destroyGroundCheck(tn_groundcheck* check) {
    check->mGroundVtable = 0x100403E0;
    check->mPolyPassVtable = 0x10040400;
    check->mGroupPassVtable = 0x100403C0;
    gabi::call(0x02008DAC, check, 0);
}

u32 ground_4_check(tn_class* actor, int count, s16 heading, f32 radius) {
    WWHD_FUNC(0x024BA6D8, u32, actor, count, heading, radius);
    gabi::Local<tn_groundcheck> check;
    tn_constructGroundCheck(check.get());
    gabi::call(0x025F1884, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), heading);
    u32 result = 0;
    gabi::Local<cXyz> offset;
    offset->y = gabi::load<f32>(0x100404F4);
    gabi::Local<cXyz> position;
    if (count > 0) {
        const f32 noGround = gabi::load<f32>(0x100405B8);
        const f32 maximumDrop = gabi::load<f32>(0x10040508);
        const f32 fallbackHeight = gabi::load<f32>(0x100405BC);
        for (int i = 0; i < count; ++i) {
            const u32 tableOffset = static_cast<u32>(i) * 4u;
            const f32 x = gabi::load<f32>(0x101D25F8u + tableOffset);
            const f32 z = gabi::load<f32>(0x101D2608u + tableOffset);
            offset->x = x * radius;
            offset->z = z * radius;
            gabi::call(0x0200FCD8, offset.get(), position.get());
            gabi::call(0x028E8D88, position.get(), &actor->current.pos, position.get());
            check->mPosition.y = position->y;
            check->mPosition.z = position->z;
            check->mPosition.x = position->x;
            const u32 play = gabi::ea(dComIfGp_get());
            const f32 groundY = cBgS_GroundCross(gabi::at<dBgS>(play + 0x12A0u), check.get());
            const f32 actorGroundY = gabi::load<f32>(gabi::ea(actor) + 0xB74u);
            const f32 effectiveGround = groundY == noGround ? fallbackHeight : groundY;
            position->y = effectiveGround;
            if (actorGroundY - effectiveGround > maximumDrop)
                result |= gabi::load<u8>(0x101D25F4u + static_cast<u32>(i));
        }
    }
    tn_destroyGroundCheck(check.get());
    return result;
}
VERIFY(0x024BA6D8, ground_4_check);

void way_pos_check(tn_class* actor, cXyz* result) {
    WWHD_FUNC(0x024BA42C, void, actor, result);
    gabi::Local<tn_linecheck> line;
    tn_constructLineCheck(line.get());
    gabi::Local<tn_groundcheck> ground;
    tn_constructGroundCheck(ground.get());
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    const f32 height = gabi::load<f32>(0x100405AC);
    offset->y = height;
    gabi::Local<cXyz> startpoint;
    startpoint->x = actor->current.pos.x;
    startpoint->y = static_cast<f32>(actor->current.pos.y) + height;
    startpoint->z = actor->current.pos.z;
    const f32 radiusCenter = gabi::load<f32>(0x100405B4);
    const f32 radiusSpread = gabi::load<f32>(0x10040508);
    const f32 angleSpread = gabi::load<f32>(0x100405B0);
    gabi::Local<cXyz> rotated;   // frame slots reused by every attempt
    gabi::Local<cXyz> endpoint;
    for (int attempt = 0; attempt < 100; ++attempt) {
        offset->z = gabi::call<f32>(0x020198D8, radiusSpread) + radiusCenter;
        const f32 randomHeading = gabi::call<f32>(0x020198D8, angleSpread);
        gabi::call(0x0200FBA4, 0, randomHeading);
        gabi::call(0x0200FCD8, offset.get(), rotated.get());
        const f32 x = static_cast<f32>(actor->current.pos.x) + rotated->x;
        const f32 y = static_cast<f32>(actor->current.pos.y) + rotated->y;
        const f32 z = static_cast<f32>(actor->current.pos.z) + rotated->z;
        endpoint->x = x;
        endpoint->y = y;
        result->y = y;
        result->x = x;
        endpoint->z = z;
        result->z = z;
        dBgS_LinChk_Set(line.get(), startpoint.get(), endpoint.get(), actor);
        const u32 play = gabi::ea(dComIfGp_get());
        if (cBgS_LineCross(gabi::at<dBgS>(play + 0x12A0u), line.get()) == 0) {
            ground->mPosition.y = endpoint->y;
            ground->mPosition.z = endpoint->z;
            ground->mPosition.x = endpoint->x;
            const u32 refreshedPlay = gabi::ea(dComIfGp_get());
            const f32 groundY = cBgS_GroundCross(gabi::at<dBgS>(refreshedPlay + 0x12A0u), ground.get());
            const f32 actorGroundY = gabi::load<f32>(gabi::ea(actor) + 0xB74u);
            if (actorGroundY - groundY < radiusSpread) break;
        }
    }
    tn_destroyGroundCheck(ground.get());
    tn_prepareLineCheckDestruction(line.get());
    cBgS_LinChk_dt(line.get(), 0);
}
VERIFY(0x024BA42C, way_pos_check);

void tn_staticInit() {
    WWHD_FUNC(0x024C161C, void);
    // Register the native static objects and initialize the random-angle range.
    gabi::store<u32>(0x1046E770, 0);
    gabi::store<u32>(0x1046E768, 0);
    gabi::store<u32>(0x1046E774, 0);
    gabi::store<u32>(0x1046E76C, 0);
    gabi::call(0x028F026C, gabi::at<u8>(0x101D2844));
    gabi::store<f32>(0x1046E71C, gabi::load<f32>(0x10040720));
    gabi::store<f32>(0x1046E720, gabi::load<f32>(0x10040724));
    gabi::call(0x028ED6F8, gabi::at<u8>(0x1046E73E));
    gabi::call(0x028F026C, gabi::at<u8>(0x101D2850));
    gabi::call(0x028EAB2C, gabi::at<u8>(0x1046E73F));
    gabi::call(0x028F026C, gabi::at<u8>(0x101D285C));

    constexpr u32 hio = 0x1046E778;
    gabi::store<u32>(hio + 0x100u, 0x10040480);
    gabi::call(0x02552BE8, gabi::at<u8>(hio + 0xD4u));
    gabi::store<u8>(hio + 1u, 0);
    gabi::store<s16>(hio + 0x1Au, 0xB9);
    gabi::store<s16>(hio + 0x1Cu, 12);
    gabi::store<f32>(hio + 0x10u, 1.0f);
    gabi::store<f32>(hio + 0x14u, 25.0f);
    gabi::store<s16>(hio + 0x18u, 0);

    // HD HIO defaults: actor scale, eye height, detection/attack ranges,
    // movement speeds and timing values retain their native field positions.
    struct FloatDefault { u32 offset; f32 value; };
    constexpr FloatDefault firstDefaults[] = {
        {0x08, 1.0f}, {0x20, 50.0f}, {0x24, 1000.0f},
        {0x28, 400.0f}, {0x2C, 250.0f}, {0x34, 400.0f},
        {0x38, 500.0f}, {0x3C, 130.0f}, {0x40, 500.0f},
        {0x44, -125.0f}, {0x48, 20.0f},
    };
    for (const FloatDefault& field : firstDefaults) gabi::store<f32>(hio + field.offset, field.value);
    gabi::store<u8>(hio + 2u, 0);
    gabi::store<u8>(hio + 3u, 0);
    gabi::store<f32>(hio + 0x4Cu, 20.0f);
    gabi::store<s16>(hio + 6u, 50);
    gabi::store<f32>(hio + 0x50u, 70.0f);
    gabi::store<s16>(hio + 0x30u, 23000);
    gabi::store<f32>(hio + 0x54u, 90.0f);
    gabi::store<f32>(hio + 0x58u, 10.0f);
    gabi::store<s16>(hio + 0x32u, 11000);
    gabi::store<s16>(hio + 0x68u, 30);
    gabi::store<s16>(hio + 0x7Cu, 30);
    gabi::store<s16>(hio + 0x7Eu, 300);
    gabi::store<f32>(hio + 0x5Cu, 10.0f);
    gabi::store<f32>(hio + 0x60u, 70.0f);
    gabi::store<f32>(hio + 0x64u, 1.0f);
    gabi::store<f32>(hio + 0x6Cu, 75.0f);
    gabi::store<u8>(hio + 4u, 0);
    constexpr FloatDefault remainingDefaults[] = {
        {0x70, 40.0f}, {0x74, 40.0f}, {0x78, 70.0f}, {0x80, 0.9f},
        {0x84, 1.0f}, {0x88, 1.0f}, {0x8C, 1.0f}, {0x90, 1.0f},
        {0x94, 1.0f}, {0x98, 1.1f}, {0x9C, 1.0f}, {0xA0, 1.0f},
        {0xA4, 0.5f}, {0xA8, 1.0f}, {0xAC, 1.2f}, {0xB0, 1.0f}, {0xB4, 1.0f},
    };
    for (const FloatDefault& field : remainingDefaults) gabi::store<f32>(hio + field.offset, field.value);
    gabi::store<s16>(hio + 0xC0u, 14);
    gabi::store<s16>(hio + 0xC2u, 24);
    gabi::store<s16>(hio + 0xC4u, 22);
    gabi::store<s16>(hio + 0xC6u, 5);
    gabi::store<s16>(hio + 0xC8u, 100);
    gabi::store<f32>(hio + 0xB8u, 1.0f);
    gabi::store<f32>(hio + 0xBCu, 1.0f);
    gabi::store<f32>(hio + 0xCCu, 0.0f);
    gabi::store<f32>(hio + 0xD0u, 500.0f);
    gabi::store<s16>(hio + 0xCAu, 28);
}
VERIFY(0x024C161C, tn_staticInit);

static void tn_setBreakParticleColors(JPABaseEmitter* emitter, const u8* primary, const u8* environment) {
    if (emitter == nullptr) return;
    const u32 base = gabi::ea(emitter);
    for (u32 i = 0; i < 3; ++i) gabi::store<u8>(base + 0x244u + i, primary[i]);
    for (u32 i = 0; i < 3; ++i) gabi::store<u8>(base + 0x248u + i, environment[i]);
}

void yoroi_break(tn_class* actor, cXyz* position, int variant) {
    WWHD_FUNC(0x024BC070, void, actor, position, variant);
    const u32 base = gabi::ea(actor);
    const u32 eyeAddress = base + 0x37Cu;
    if (actor != nullptr && eyeAddress != 0) {
        const s8 room = actor->current.roomNo;
        const s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call(0x025E1A40, 0x586A, gabi::at<cXyz>(eyeAddress), 0, reverb);
    }
    gabi::Local<cXyz> particleScale;
    const f32 scale = gabi::load<f32>(variant == 0 ? 0x100405E8 : 0x100405EC);
    particleScale->x = scale;
    particleScale->y = scale;
    particleScale->z = scale;
    dComIfGp_particle_set(0x13, position, nullptr, particleScale.get());
    dComIfGp_particle_set(0x16, position, nullptr, particleScale.get());

    const u32 colorOffset = static_cast<u32>(actor->mArmorColorIndex) * 4u;
    const f32 divisor = gabi::load<f32>(0x100405F0);
    u8 primary[3];
    u8 environment[3];
    f32 light[3];
    for (u32 i = 0; i < 3; ++i) {
        light[i] = static_cast<f32>(gabi::load<u8>(base + 0x1A8u + i)) / divisor;
        const f32 primaryChannel = static_cast<f32>(gabi::load<u8>(0x101D2648u + colorOffset + i));
        primary[i] = static_cast<u8>(gabi::ftoi(primaryChannel * light[i]));
    }
    for (u32 i = 0; i < 3; ++i) {
        const f32 environmentChannel = static_cast<f32>(gabi::load<u8>(0x101D2660u + colorOffset + i));
        environment[i] = static_cast<u8>(gabi::ftoi(environmentChannel * light[i]));
    }
    if (variant == 0) {
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x81B4, position);
        tn_setBreakParticleColors(emitter, primary, environment);
    }
    JPABaseEmitter* emitter = dComIfGp_particle_set(0x81B5, position);
    tn_setBreakParticleColors(emitter, primary, environment);
}
VERIFY(0x024BC070, yoroi_break);

fopAc_ac_c* wepon_hit_check(tn_class* actor) {
    WWHD_FUNC(0x024BBB3C, fopAc_ac_c*, actor);
    dComIfGp_get();
    const u32 base = gabi::ea(actor);
    const u32 oldX = gabi::load<u32>(base + 0x1510u);
    actor->mWeaponCheckFlag = 0;
    gabi::store<u32>(base + 0x1528u, oldX);
    mDoExt_McaMorf* body = actor->mpBodyMorf;
    const u32 oldY = gabi::load<u32>(base + 0x1514u);
    const u32 oldZ = gabi::load<u32>(base + 0x1518u);
    gabi::store<u32>(base + 0x152Cu, oldY);
    gabi::store<u32>(base + 0x1530u, oldZ);
    const f32 animationFrame = body->mFrameCtrl.mFrame;
    const s32 attackType = actor->mAttackType;
    const s32 frame = gabi::ftoi(animationFrame);
    int firstFrame = 0;
    int lastFrame = 0;
    switch (attackType) {
    case 0: firstFrame = 19; lastFrame = 26; break;
    case 1:
        if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0) {
            firstFrame = 19; lastFrame = 28;
        } else { firstFrame = 14; lastFrame = 23; }
        break;
    case 2: firstFrame = 7; lastFrame = 9; break;
    case 3: firstFrame = 20; lastFrame = 24; break;
    case 4: firstFrame = 20; lastFrame = 26; break;
    case 5: lastFrame = 1000; break;
    }
    if (frame < firstFrame || frame > lastFrame || actor->mWeaponActivation < 0) return nullptr;
    const u32 powerTableOffset = static_cast<u32>(attackType) * 4u;
    bool footAttack = false;
    if ((static_cast<u8>(actor->mEquipmentType) & 1u) != 0) {
        const s32 matrixAttack = actor->mAttackType;
        footAttack = matrixAttack == 3;
        const u8 power = static_cast<u8>(gabi::load<u32>(0x101D2630u + powerTableOffset));
        body = actor->mpBodyMorf;
        actor->mWeponSph.SetAtAtp(power);
        actor->mWepon2Sph.SetAtAtp(power);
        const u32 model = gabi::load<u32>(gabi::ea(body) + 0x90u);
        const u32 buffer = gabi::load<u32>(model + 0x2Cu);
        const u16 flags = gabi::load<u16>(buffer + 4u);
        const u32 matrices = gabi::load<u32>(buffer + 0x10u);
        gabi::store<u16>(buffer + 4u, flags | 0x10u);
        gabi::call(0x028E90D4, gabi::at<Mtx34>(matrices + (matrixAttack == 3 ? 0xF0u : 0x450u)),
                   gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)));
    } else {
        body = actor->mpBodyMorf;
        const u8 power = static_cast<u8>(gabi::load<u32>(0x101D2618u + powerTableOffset));
        const s32 matrixAttack = actor->mAttackType;
        footAttack = matrixAttack == 3;
        actor->mWepon2Sph.SetAtAtp(power);
        actor->mWeponSph.SetAtAtp(power);
        const u32 model = gabi::load<u32>(gabi::ea(body) + 0x90u);
        const u32 buffer = gabi::load<u32>(model + 0x2Cu);
        const u16 flags = gabi::load<u16>(buffer + 4u);
        const u32 matrices = gabi::load<u32>(buffer + 0x10u);
        gabi::store<u16>(buffer + 4u, flags | 0x10u);
        gabi::call(0x028E90D4, gabi::at<Mtx34>(matrices + (matrixAttack == 3 ? 0xF0u : 0x450u)),
                   gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)));
    }
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = footAttack || actor->mAttackType == 2 ? 0.0f : gabi::load<f32>(0x100405D8);
    gabi::call(0x0200FCD8, offset.get(), &actor->mWeaponPosition);
    offset->z = static_cast<f32>(offset->z) * gabi::load<f32>(0x100405DC);
    gabi::call(0x0200FCD8, offset.get(), &actor->mWeapon2Position);
    const u8 initialized = actor->mWeaponColliderInitialized;
    actor->mWeponSph.SetAtType(8);
    actor->mWepon2Sph.SetAtType(8);
    if (initialized == 0) {
        actor->mWeaponColliderInitialized = 1;
        gabi::call(0x025167C0, &actor->mWeponSph, &actor->mWeaponPosition);
        gabi::call(0x025167C0, &actor->mWepon2Sph, &actor->mWeapon2Position);
        return nullptr;
    }
    actor->mWeponSph.SetR(gabi::load<f32>(0x1047B644) + gabi::load<f32>(0x100405E0));
    gabi::call(0x025167E4, &actor->mWeponSph, &actor->mWeaponPosition);
    gabi::call(0x025167E4, &actor->mWepon2Sph, &actor->mWeapon2Position);
    actor->mWepon2Sph.SetR(gabi::load<f32>(0x1047B648) + gabi::load<f32>(0x100405E4));
    const s32 splAttack = actor->mAttackType;
    if (splAttack <= 1 || splAttack == 4 || splAttack == 5) {
        const u8 special = splAttack <= 1 ? 6 : 7;
        actor->mWepon2Sph.SetAtSpl(special);
        actor->mWeponSph.SetAtSpl(special);
    } else {
        actor->mWeponSph.SetAtSpl(0);
        actor->mWepon2Sph.SetAtSpl(0);
    }
    u32 play = gabi::ea(dComIfGp_get());
    gabi::call(0x0200E240, gabi::at<u8>(play + 0x26A4u), &actor->mWeponSph);
    play = gabi::ea(dComIfGp_get());
    gabi::call(0x0200E240, gabi::at<u8>(play + 0x26A4u), &actor->mWepon2Sph);
    if (actor->mWeponSph.ChkAtHit() == 0 && actor->mWepon2Sph.ChkAtHit() == 0) return nullptr;
    dCcD_GObjInf* hit = actor->mWeponSph.ChkAtHit() != 0
        ? gabi::call<dCcD_GObjInf*>(0x02516178, &actor->mWeponSph)
        : gabi::call<dCcD_GObjInf*>(0x02516178, &actor->mWepon2Sph);
    if (hit == nullptr) return nullptr;
    dCcD_Stts* status = hit->mStts;
    return status == nullptr ? nullptr : static_cast<fopAc_ac_c*>(status->mp_actor);
}
VERIFY(0x024BBB3C, wepon_hit_check);

void hukki(tn_class* actor) {
    WWHD_FUNC(0x024C25F8, void, actor);
    const u32 base = gabi::ea(actor);
    const u32 play = gabi::ea(dComIfGp_get());
    const f32 joint13Z = actor->mJointPositions[13].z;
    const f32 joint10X = actor->mJointPositions[10].x;
    const f32 joint13X = actor->mJointPositions[13].x;
    const f32 joint10Z = actor->mJointPositions[10].z;
    const f32 directionX = joint10X - joint13X;
    const u32 matrix = gabi::load<u32>(0x1018C7B0);
    const f32 directionZ = joint10Z - joint13Z;
    const u32 player = gabi::load<u32>(play + 0x5B2Cu);
    const s16 heading = gabi::call<s16>(0x020195B0, directionX, directionZ);
    gabi::call(0x025F1884, gabi::at<Mtx34>(matrix), heading);

    const s16 mode = actor->mActionMode;
    const f32 zero = gabi::load<f32>(0x100404E0);
    const f32 one = gabi::load<f32>(0x100404FC);
    gabi::Local<cXyz> offset;
    offset->x = zero;
    offset->y = zero;
    if (mode == 14) {
        const s16 target = gabi::load<s16>(base + 0x532u);
        gabi::store<u8>(base + 0xCD4u, 1);
        gabi::store<s16>(base + 0xA94u, target);
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), target, 3, 0x1000);
        if (gabi::load<s16>(base + 0x4FAu) == 0) {
            actor->mAction = 5;
            actor->mActionMode = 0;
        }
        return;
    }
    if (mode != 10 && mode != 12 && mode != 13) return;
    if (mode == 10 || mode == 12) {
        anm_init(actor, mode == 10 ? 0x4E : 0x4F, zero, 0, one, mode == 10 ? 0x1E : 0x1F);
        yoroi_anm_init(actor, 0x64, gabi::load<f32>(0x10040510), 0, one);
        const s32 timer = mode == 10 ? 15 : static_cast<s32>(gabi::load<s16>(0x1047B692)) + 15;
        offset->z = gabi::load<f32>(0x100406D0);
        gabi::store<s16>(base + 0x4FCu, static_cast<s16>(timer));
        const s32 immunity = static_cast<s32>(gabi::load<s16>(0x1047BC2C)) + 35;
        gabi::store<f32>(base + 0x370u, zero);
        gabi::store<s16>(base + 0x508u, static_cast<s16>(immunity));
        actor->mActionMode = 13;
        gabi::store<s16>(base + 0xA44u, 0);
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(base + 0xA1Cu));
        const f32 currentY = gabi::load<f32>(base + 0x318u);
        const f32 heightCorrection = gabi::load<f32>(base + 0xA14u);
        const f32 oldY = gabi::load<f32>(base + 0x304u);
        gabi::store<f32>(base + 0x318u, currentY - heightCorrection);
        if (mode == 10) gabi::store<f32>(base + 0xA14u, zero);
        gabi::store<f32>(base + 0x304u, oldY - heightCorrection);
        if (mode == 12) gabi::store<f32>(base + 0xA14u, zero);
    }
    const s32 slideStart = static_cast<s32>(gabi::load<s16>(0x1047B9F2)) + 6;
    if (gabi::load<s16>(base + 0x4FCu) <= slideStart) {
        const f32 tuning = gabi::load<f32>(0x1047B984);
        const f32 baseStep = gabi::load<f32>(0x100404F4);
        const f32 stepScale = gabi::load<f32>(0x1004075C);
        const f32 stepX = (tuning + baseStep) * stepScale;
        cLib_addCalc0(gabi::at<be<f32>>(base + 0xA1Cu), one, stepX);
        const f32 stepZ = (gabi::load<f32>(0x1047B984) + baseStep) * stepScale;
        cLib_addCalc0(gabi::at<be<f32>>(base + 0xA24u), one, stepZ);
        const s32 slideEnd = static_cast<s32>(gabi::load<s16>(0x1047B9F4)) + 1;
        if (gabi::load<s16>(base + 0x4FCu) >= slideEnd) {
            offset->z = gabi::load<f32>(0x10040664);
            gabi::Local<cXyz> movement;
            gabi::call(0x0200FCD8, offset.get(), movement.get());
            const f32 x = gabi::load<f32>(base + 0x314u) + static_cast<f32>(movement->x);
            const f32 z = gabi::load<f32>(base + 0x31Cu);
            gabi::store<f32>(base + 0x314u, x);
            gabi::store<f32>(base + 0x31Cu, z + static_cast<f32>(movement->z));
        }
    }
    mDoExt_McaMorf* body = actor->mpBodyMorf;
    if ((gabi::load<u8>(gabi::ea(body) + 0xA7u) & 1u) == 0 &&
        static_cast<f32>(body->mFrameCtrl.mRate) != zero) return;
    if (!(gabi::load<f32>(player + 0x3CCu) < zero) &&
        gabi::load<f32>(base + 0x518u) < gabi::load<f32>(0x1046E7A0)) {
        actor->mActionMode = 14;
        gabi::store<s16>(base + 0x4FAu, 10);
        gabi::store<s16>(base + 0xA4Cu, 0);
    } else {
        actor->mAction = 0;
        path_check(actor);
        wait_set(actor);
        actor->mActionMode = 0;
        gabi::store<s16>(base + 0xA4Cu, 0);
    }
}
VERIFY(0x024C25F8, hukki);

static void tn_leaveWeaponSearch(tn_class* actor) {
    actor->mAction = 0;
    path_check(actor);
    wait_set(actor);
    actor->mActionMode = 2;
}

void wepon_search(tn_class* actor) {
    WWHD_FUNC(0x024C1A3C, void, actor);
    dComIfGp_get();
    const u32 base = gabi::ea(actor);
    const u32 id = actor->mWeaponActorId;
    gabi::Local<be<u32>> searchId;
    *searchId = id;
    const u32 weapon = id == 0xFFFFFFFFu ? 0
        : gabi::call<u32>(0x025D5218, 0x025E1234u, searchId.get());
    s16 mode = actor->mActionMode;
    if (mode < 2 && (weapon == 0 || (gabi::load<u32>(weapon + 0x2E0u) & 0x2000u) != 0)) {
        tn_leaveWeaponSearch(actor);
        return;
    }
    f32 distance = gabi::load<f32>(0x1004064C);
    if (weapon != 0) {
        const f32 weaponX = gabi::load<f32>(weapon + 0x314u);
        const f32 actorX = gabi::load<f32>(base + 0x314u);
        const f32 weaponZ = gabi::load<f32>(weapon + 0x31Cu);
        const f32 dx = weaponX - actorX;
        const f32 dz = weaponZ - gabi::load<f32>(base + 0x31Cu);
        const s16 heading = gabi::call<s16>(0x020195B0, dx, dz);
        const f32 squaredDistance = gabi::fmadds(dx, dx, dz * dz);
        gabi::store<s16>(base + 0xA94u, heading);
        distance = gabi::call<f32>(0x028F4384, squaredDistance);
        mode = actor->mActionMode;
    }
    const f32 zero = gabi::load<f32>(0x100404E0);
    const f32 one = gabi::load<f32>(0x100404FC);
    switch (mode) {
    case -1: {
        const f32 blend = gabi::load<f32>(0x10040500);
        actor->mActionMode = 0;
        actor->mPathSearchEnabled = 0;
        anm_init(actor, 0x36, blend, 0, one, -1);
        yoroi_anm_init(actor, 0x63, blend, 0, one);
        gabi::store<s16>(base + 0x4FAu, distance < gabi::load<f32>(0x10040748) ? 20 : 200);
        gabi::store<s16>(base + 0x506u, 5);
        return;
    }
    case 0: {
        const s16 delay = gabi::load<s16>(base + 0x506u);
        gabi::store<f32>(base + 0x370u, zero);
        if (delay != 0) return;
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u),
                   gabi::load<s16>(base + 0xA94u), 2, 0x3000);
        mDoExt_McaMorf* body = actor->mpBodyMorf;
        if ((gabi::load<u8>(gabi::ea(body) + 0xA7u) & 1u) == 0 &&
            static_cast<f32>(body->mFrameCtrl.mRate) != zero &&
            gabi::load<s16>(base + 0x4FAu) != 0) return;
        actor->mActionMode = 1;
        gabi::store<s16>(base + 0x502u, gabi::load<s16>(0x1046E7F6));
        fight_run_set(actor);
        return;
    }
    case 1: {
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), gabi::load<f32>(0x1046E7CC),
                      one, gabi::load<f32>(0x10040510));
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u),
                   gabi::load<s16>(base + 0xA94u), 4, 0x1000);
        if (distance < gabi::load<f32>(0x1047BA98) + gabi::load<f32>(0x100405D8)) {
            actor->mActionMode = 2;
            anm_init(actor, 0x4C, gabi::load<f32>(0x10040630), 0, one, 0x1D);
            gabi::store<s16>(base + 0x4FAu, 29);
            return;
        }
        if (gabi::load<s16>(base + 0x502u) == 0) {
            gabi::store<s16>(base + 0x4FAu, 0);
            actor->mPathSearchEnabled = 1;
            actor->mAction = 4;
            return;
        }
        if ((gabi::load<u32>(base + 0xB08u) & 0x30u) == 0x30u) {
            tn_leaveWeaponSearch(actor);
            gabi::store<s16>(base + 0x514u, 70);
        }
        return;
    }
    case 2: {
        gabi::store<f32>(base + 0x370u, zero);
        const s16 pickupFrame = static_cast<s16>(static_cast<s32>(gabi::load<s16>(0x1047B696)) + 18);
        s16 timer = gabi::load<s16>(base + 0x4FAu);
        if (timer == pickupFrame) {
            if (weapon != 0 && (gabi::load<u32>(weapon + 0x2E0u) & 0x2000u) == 0) {
                actor->mPathDriven = 2;
                gabi::call(0x025D9D0C, gabi::at<fopAc_ac_c>(weapon), 0);
                timer = gabi::load<s16>(base + 0x4FAu);
            } else {
                tn_leaveWeaponSearch(actor);
                timer = gabi::load<s16>(base + 0x4FAu);
            }
        }
        if (timer < 14) {
            const s16 target = gabi::load<s16>(base + 0x532u);
            gabi::store<u8>(base + 0xCD4u, 1);
            gabi::store<s16>(base + 0xA94u, target);
            gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), target, 3, 0x800);
        }
        mDoExt_McaMorf* body = actor->mpBodyMorf;
        if ((gabi::load<u8>(gabi::ea(body) + 0xA7u) & 1u) != 0 ||
            static_cast<f32>(body->mFrameCtrl.mRate) == zero) tn_leaveWeaponSearch(actor);
        return;
    }
    default: return;
    }
}
VERIFY(0x024C1A3C, wepon_search);

static void tn_playBombReactionSound(tn_class* actor, u32 sound) {
    const u32 base = gabi::ea(actor);
    if (base + 0x37Cu == 0) return;
    const s8 room = actor->current.roomNo;
    const u32 id = base == 0 ? 0xFFFFFFFFu : gabi::load<u32>(base + 4u);
    const s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call(0x025E1AA4, sound, gabi::at<cXyz>(base + 0x37Cu), id, 0, reverb);
}

void b_nige(tn_class* actor) {
    WWHD_FUNC(0x024C1ED4, void, actor);
    const u32 base = gabi::ea(actor);
    fopAc_ac_c* bomb = search_bomb(actor, 0);
    actor->mNearbyBomb = bomb;
    if (bomb == nullptr) {
        tn_leaveWeaponSearch(actor);
        return;
    }
    const u32 bombBase = gabi::ea(bomb);
    const f32 bombX = gabi::load<f32>(bombBase + 0x314u);
    const f32 actorX = gabi::load<f32>(base + 0x314u);
    const f32 actorZ = gabi::load<f32>(base + 0x31Cu);
    const f32 dx = bombX - actorX;
    const f32 dz = gabi::load<f32>(bombBase + 0x31Cu) - actorZ;
    const s16 away = gabi::call<s16>(0x020195B0, -dx, -dz);
    const s16 mode = actor->mActionMode;
    const f32 zero = gabi::load<f32>(0x100404E0);
    gabi::store<s16>(base + 0xA94u, away);
    const f32 safeDistance = gabi::load<f32>(0x1004074C);
    const f32 one = gabi::load<f32>(0x100404FC);
    bool enterWait = false;
    switch (mode) {
    case 0:
        actor->mActionMode = 1;
        anm_init(actor, 0x36, gabi::load<f32>(0x10040500), 0, one, -1);
        tn_playBombReactionSound(actor, 0x4885);
        gabi::store<s16>(base + 0x4FAu, 10);
        [[fallthrough]];
    case 1: {
        const s16 turnTarget = static_cast<s16>(static_cast<s32>(gabi::load<s16>(base + 0xA94u)) + 0x8000);
        gabi::store<f32>(base + 0x370u, zero);
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), turnTarget, 2, 0x3000);
        if (gabi::load<s16>(base + 0x4FAu) != 0) return;
        const s32 pathDriven = actor->mPathDriven;
        actor->mActionMode = 2;
        anm_init(actor, pathDriven != 0 ? 0x48 : 0x49,
                 gabi::load<f32>(0x10040630), 0, one, pathDriven != 0 ? 0x1B : 0x1C);
        const f32 speedBase = gabi::load<f32>(0x10040750);
        const f32 speed = gabi::load<f32>(0x1047B61C) + speedBase;
        const f32 verticalBase = gabi::load<f32>(0x10040754);
        gabi::store<f32>(base + 0x370u, speed);
        gabi::store<f32>(base + 0x340u, gabi::load<f32>(0x1047B620) + verticalBase);
        tn_playBombReactionSound(actor, 0x488A);
        return;
    }
    case 2: {
        if ((gabi::load<u32>(base + 0xB08u) & 0x20u) == 0) return;
        mDoExt_McaMorf* body = actor->mpBodyMorf;
        if ((gabi::load<u8>(gabi::ea(body) + 0xA7u) & 1u) == 0 &&
            static_cast<f32>(body->mFrameCtrl.mRate) != zero) return;
        const f32 squaredDistance = gabi::fmadds(dx, dx, dz * dz);
        gabi::store<f32>(base + 0x370u, zero);
        const f32 distance = gabi::call<f32>(0x028F4384, squaredDistance);
        if (distance > safeDistance) enterWait = true;
        else {
            actor->mActionMode = 3;
            fight_run_set(actor);
            return;
        }
        break;
    }
    case 3: {
        const f32 speed = gabi::load<f32>(0x1046E7CC);
        const s16 target = gabi::load<s16>(base + 0xA94u);
        gabi::store<f32>(base + 0x370u, speed);
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), target, 4, 0x1000);
        const f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
        if (!(distance > safeDistance)) return;
        enterWait = true;
        break;
    }
    case 4: {
        const s16 target = gabi::load<s16>(base + 0x532u);
        gabi::store<f32>(base + 0x370u, zero);
        gabi::store<s16>(base + 0xA94u, target);
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), target, 3, 0x1000);
        const f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
        if (distance < gabi::load<f32>(0x10040758)) actor->mActionMode = 0;
        return;
    }
    default: return;
    }
    if (enterWait) {
        const s32 pathDriven = actor->mPathDriven;
        const f32 blend = gabi::load<f32>(0x100405D0);
        actor->mActionMode = 4;
        anm_init(actor, pathDriven != 0 ? 0x35 : 0x4D, blend, 2, one, -1);
    }
}
VERIFY(0x024C1ED4, b_nige);

static mDoExt_McaMorf* tn_createResourceMorf(u32 modelIndex, u32 animationIndex, f32 speed) {
    J3DModelData* model = static_cast<J3DModelData*>(
        dComIfG_getObjectRes(STR(0x100406A7), modelIndex, TN_SAFESTRING_VTABLE));
    J3DAnmTransform* animation = static_cast<J3DAnmTransform*>(
        dComIfG_getObjectRes(STR(0x100406A7), animationIndex, TN_SAFESTRING_VTABLE));
    return mDoExt_McaMorf::create(nullptr, model, nullptr, nullptr, animation, 2, speed,
                                 0, -1, 1, nullptr, 0, 0x11020203);
}

static mDoExt_brkAnm* tn_createBrkAnimation() {
    mDoExt_brkAnm* animation = gabi::call<mDoExt_brkAnm*>(0x0273AD10, 0x78);
    if (animation != nullptr) animation = gabi::call<mDoExt_brkAnm*>(0x025E80D0, animation);
    return animation;
}

// The brk pointer is reloaded from its field after the resource lookup, as the original does.
static BOOL tn_initBrkAnimation(u32 brkField, J3DModel* model, u32 resourceIndex,
                               u8 loopMode, f32 speed) {
    void* resource = dComIfG_getObjectRes(STR(0x100406A7), resourceIndex, TN_SAFESTRING_VTABLE);
    const u32 brk = gabi::load<u32>(brkField);
    const u32 modelData = gabi::load<u32>(gabi::ea(model) + 0xACu);
    return gabi::call<BOOL>(0x025E8154, brk, gabi::at<J3DModelData>(modelData), resource,
                           1, loopMode, speed, 0, -1, 0, 0);
}

static u32 tn_jointCallbackAddress(J3DModel* model, u32 joint) {
    const u32 table = gabi::load<u32>(gabi::ea(model) + 0xACu);
    const u32 count = gabi::load<u32>(table + 4u);
    const u32 records = gabi::load<u32>(table + 8u);
    return records + (joint < count ? joint * 0x1Cu : 0u) + 8u;
}

int useHeapInit(tn_class* actor) {
    WWHD_FUNC(0x024C02FC, int, actor);
    const u32 base = gabi::ea(actor);
    J3DModelData* bodyData = static_cast<J3DModelData*>(
        dComIfG_getObjectRes(STR(0x100406A7), 0x6C, TN_SAFESTRING_VTABLE));
    J3DAnmTransform* bodyAnimation = static_cast<J3DAnmTransform*>(
        dComIfG_getObjectRes(STR(0x100406A7), 0x39, TN_SAFESTRING_VTABLE));
    const f32 speed = gabi::load<f32>(0x100404FC);
    actor->mpBodyMorf = mDoExt_McaMorf::create(nullptr, bodyData, nullptr, nullptr, bodyAnimation,
                                             2, speed, 0, -1, 1, nullptr, 0, 0x11020203);
    J3DModel* bodyModel = actor->mpBodyMorf->getModel();
    if (bodyModel == nullptr) {
        gabi::call(0x0273AA24, STR(0x100406AC), 0x1A9B, STR(0x100406B8));
    }
    gabi::store<u32>(gabi::ea(bodyModel) + 0xB8u, base);
    for (u32 joint = 0; joint < 33; ++joint) {
        const s8 slot = gabi::load<s8>(0x101D2558u + joint);
        u32 callback = 0;
        if (slot >= 0) {
            callback = (slot >= 14 && slot <= 17) || slot == 20 ? 0x024B9C1Cu : 0x024B97A0u;
        } else if (joint == 30 || joint == 32) callback = 0x024B9ED0u;
        if (callback != 0) gabi::store<u32>(tn_jointCallbackAddress(bodyModel, joint), callback);
    }
    actor->mpShieldMorf = tn_createResourceMorf(0x6C, 0x3C, speed);
    J3DModel* shieldModel = actor->mpShieldMorf->getModel();
    if (shieldModel == nullptr) return 0;
    gabi::store<u32>(gabi::ea(shieldModel) + 0xB8u, base);
    gabi::store<u32>(tn_jointCallbackAddress(shieldModel, 10), 0x024B9FF8u);
    actor->mpBrkAnm = tn_createBrkAnimation();
    if (actor->mpBrkAnm == nullptr) return 0;
    bodyModel = actor->mpBodyMorf->getModel();
    if (tn_initBrkAnimation(gabi::ea(&actor->mpBrkAnm), bodyModel, 0x73, 2, speed) == 0) return 0;
    for (u32 part = 0; part < 3; ++part) {
        J3DModel* model;
        if (part == 0) {
            actor->mpArmorMorf = tn_createResourceMorf(0x6E, 0x64, speed);
            model = actor->mpArmorMorf->getModel();
        } else {
            const bool alternateWeapon = part == 1 && (static_cast<u8>(actor->mEquipmentType) & 1u) != 0;
            const u32 index = alternateWeapon ? 0x6Bu : gabi::load<u16>(0x101D2678u + 2u * part);
            J3DModelData* data = static_cast<J3DModelData*>(
                dComIfG_getObjectRes(STR(0x100406A7), index, TN_SAFESTRING_VTABLE));
            model = mDoExt_J3DModel__create(data, 0, 0x11020203);
        }
        actor->mParts[part].mpPartModel = model;
        if (model == nullptr) return 0;
        actor->mParts[part].mpPartBrkAnm = tn_createBrkAnimation();
        if (actor->mParts[part].mpPartBrkAnm == nullptr) return 0;
        const bool alternateWeapon = part == 1 && (static_cast<u8>(actor->mEquipmentType) & 1u) != 0;
        const u32 index = alternateWeapon ? 0x72u : gabi::load<u16>(0x101D2680u + 2u * part);
        model = actor->mParts[part].mpPartModel;
        if (tn_initBrkAnimation(gabi::ea(&actor->mParts[part].mpPartBrkAnm), model, index, 0, speed) == 0) return 0;
    }
    bodyModel = actor->mpBodyMorf->getModel();
    const u32 jointHit = gabi::call<u32>(0x02552B60, bodyModel, gabi::at<u8>(0x101D257C), 10);
    gabi::store<u32>(base + 0x1C58u, jointHit);
    if (jointHit == 0) return 0;
    gabi::store<u32>(base + 0x36Cu, jointHit);
    return 4;
}
VERIFY(0x024C02FC, useHeapInit);

static void tn_selectJointMatrix(mDoExt_McaMorf* morf, u32 joint) {
    const u32 model = gabi::ea(morf->getModel());
    const u32 buffer = gabi::load<u32>(model + 0x2Cu);
    const u16 flags = gabi::load<u16>(buffer + 4u);
    const u32 matrices = gabi::load<u32>(buffer + 0x10u);
    gabi::store<u16>(buffer + 4u, flags | 0x10u);
    gabi::call(0x028E90D4, gabi::at<Mtx34>(matrices + joint * 0x30u),
               gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)));
}

static void tn_playPartImpact(tn_class* actor, u32 sound) {
    const u32 base = gabi::ea(actor);
    if (base == 0 || base + 0x37Cu == 0) return;
    const s32 reverb = gabi::call<s32>(0x02520540, static_cast<s8>(actor->current.roomNo));
    gabi::call(0x025E1A40, sound, gabi::at<cXyz>(base + 0x37Cu), 0, reverb);
}

static s16 tn_randomPartRotation(f32 magnitude) {
    return static_cast<s16>(gabi::ftoi(gabi::call<f32>(0x02019918, magnitude)));
}

static void tn_moveFlyingPart(tn_class* actor, tn_p* part, int index, tn_groundcheck* ground,
                              f32 zero, f32 horizontalBounce, f32 randomRotation) {
    const u32 partBase = gabi::ea(part);
    for (u32 i = 0; i < 3; ++i) gabi::store<u32>(partBase + 0x18u + 4u * i, gabi::load<u32>(partBase + 0xCu + 4u * i));
    gabi::call(0x028E8D88, &part->mPosition, &part->mVelocity, &part->mPosition);
    part->mVelocity.y = static_cast<f32>(part->mVelocity.y) - gabi::load<f32>(0x100405D0);
    gabi::call(0x0201A554, &part->mAngles, &part->mAngularVelocity);
    gabi::Local<cXyz> motion;
    gabi::call(0x0201ADE0, &part->mPosition, motion.get(), &part->mPreviousPosition);
    gabi::Local<cXyz> probeOffset;
    *probeOffset = *motion;
    const f32 motionSquared = gabi::call<f32>(0x028E8DD0, probeOffset.get());
    const f32 motionLength = gabi::call<f32>(0x028F4384, motionSquared);
    if (motionLength > zero) {
        gabi::Local<tn_linecheck> line;
        tn_constructLineCheck(line.get());
        const u32 matrix = gabi::load<u32>(0x1018C7B0);
        const s16 heading = gabi::call<s16>(0x020195B0, static_cast<f32>(probeOffset->x), static_cast<f32>(probeOffset->z));
        gabi::call(0x025F1884, gabi::at<Mtx34>(matrix), heading);
        probeOffset->x = zero;
        probeOffset->z = horizontalBounce;
        probeOffset->y = gabi::load<f32>(0x100405C4);
        gabi::Local<cXyz> endpoint;
        gabi::call(0x0200FCD8, probeOffset.get(), endpoint.get());
        probeOffset->x = part->mPosition.x;
        const f32 y = part->mPosition.y;
        const f32 height = gabi::load<f32>(0x100405C4);
        probeOffset->y = y;
        probeOffset->z = part->mPosition.z;
        probeOffset->y = y + height;
        gabi::call(0x028E8D88, endpoint.get(), &part->mPosition, endpoint.get());
        gabi::call(0x024F1AFC, line.get(), probeOffset.get(), endpoint.get(), actor);
        const u32 play = gabi::ea(dComIfGp_get());
        if (gabi::call<BOOL>(0x02008860, gabi::at<u8>(play + 0x12A0u), line.get()) != 0) {
            const f32 factor = gabi::load<f32>(0x1004060C);
            const f32 vx = part->mVelocity.x;
            const f32 oldX = part->mPreviousPosition.x;
            const f32 vz = part->mVelocity.z;
            const f32 oldZ = part->mPreviousPosition.z;
            part->mPosition.x = oldX;
            part->mPosition.z = oldZ;
            part->mVelocity.x = vx * factor;
            part->mVelocity.z = vz * factor;
        }
        tn_prepareLineCheckDestruction(line.get());
        gabi::call(0x02008B4C, line.get(), 0);
    }
    probeOffset->x = part->mPosition.x;
    const f32 partY = part->mPosition.y;
    const f32 probeHeight = gabi::load<f32>(0x100404F4);
    probeOffset->y = partY;
    probeOffset->z = part->mPosition.z;
    ground->mPosition.x = probeOffset->x;
    ground->mPosition.y = partY + probeHeight;
    ground->mPosition.z = probeOffset->z;
    probeOffset->y = partY + probeHeight;
    const u32 play = gabi::ea(dComIfGp_get());
    f32 groundHeight = gabi::call<f32>(0x02008974, gabi::at<u8>(play + 0x12A0u), ground);
    if (index == 0) groundHeight += gabi::load<f32>(0x10040610);
    else if (index == 1) groundHeight += gabi::load<f32>(0x1047B614);
    else if (index == 2) groundHeight += gabi::load<f32>(0x10040614);
    else if (index == 3) groundHeight += gabi::load<f32>(0x1047B61C);
    if (!(static_cast<f32>(part->mPosition.y) > groundHeight)) {
        const f32 vy = part->mVelocity.y;
        const f32 bounceThreshold = gabi::load<f32>(0x10040618);
        part->mPosition.y = groundHeight;
        if (vy < bounceThreshold) {
            if (index == 1) {
                part->mVelocity.y = horizontalBounce;
                part->mVelocity.x = gabi::call<f32>(0x02019918, horizontalBounce);
                part->mVelocity.z = gabi::call<f32>(0x02019918, horizontalBounce);
                part->mAngularVelocity.x = tn_randomPartRotation(randomRotation);
                part->mAngularVelocity.y = tn_randomPartRotation(randomRotation);
                tn_playPartImpact(actor, 0x6996);
            } else {
                const f32 vx = part->mVelocity.x;
                const f32 factor = gabi::load<f32>(0x10040504);
                const f32 vz = part->mVelocity.z;
                const f32 verticalBounce = gabi::load<f32>(0x10040510);
                part->mVelocity.y = verticalBounce;
                part->mVelocity.x = vx * factor;
                part->mVelocity.z = vz * factor;
                tn_playPartImpact(actor, 0x6997);
            }
            part->mCountDown = 20;
        } else {
            part->mAngularVelocity.z = 0;
            part->mAngularVelocity.y = 0;
            part->mVelocity.z = zero;
            part->mVelocity.y = zero;
            part->mAngularVelocity.x = 0;
            part->mVelocity.x = zero;
            gabi::call(0x0200F428, &part->mAngles.x, 0, 1, 0xC00);
            gabi::call(0x0200F428, &part->mAngles.z, 0x4000, 1, 0xC00);
        }
    }
    if (part->mCountDown == 1) {
        part->mState = -1;
        yoroi_break(actor, &part->mPosition, static_cast<u8>(index));
        if (index == 0) {
            const u32 mantleId = actor->mMantPcId;
            if (mantleId != 0xFFFFu) {
                gabi::Local<be<u32>> id;
                *id = mantleId;
                const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : gabi::call<u32>(0x025D5218, 0x025E1234u, id.get());
                if (mantle != 0) gabi::call(0x025D57E0, gabi::at<fopAc_ac_c>(mantle));
                actor->mMantPcId = 0xFFFF;
            }
        }
    }
}

void part_move(tn_class* actor, int index) {
    WWHD_FUNC(0x024BC3EC, void, actor, index);
    const u32 base = gabi::ea(actor);
    const u32 play = gabi::ea(dComIfGp_get());
    const u32 player = gabi::load<u32>(play + 0x5B2Cu);
    tn_p* part = gabi::at<tn_p>(base + 0x3F8u + static_cast<u32>(index) * 0x4Cu);
    gabi::Local<tn_groundcheck> ground;
    tn_constructGroundCheck(ground.get());
    const f32 zero = gabi::load<f32>(0x100404E0);
    gabi::Local<cXyz> offset;
    offset->x = zero;
    offset->y = zero;
    offset->z = zero;
    part->mCounter = static_cast<s16>(static_cast<s32>(part->mCounter) + 1);
    const s8 timer = part->mCountDown;
    const u32 joint = gabi::load<u32>(0x101D254Cu + static_cast<u32>(index) * 4u);
    if (timer != 0) part->mCountDown = static_cast<s8>(static_cast<s32>(timer) - 1);
    const s8 state = part->mState;
    const f32 launchSpeed = gabi::load<f32>(0x100405CC);
    const f32 randomRotation = gabi::load<f32>(0x10040604);
    const f32 horizontalBounce = gabi::load<f32>(0x10040600);
    if (state == 0) {
        tn_selectJointMatrix(actor->mpBodyMorf, joint);
        gabi::call(0x0200FCD8, offset.get(), &part->mPosition);
        const u8 detachFlags = gabi::load<u8>(base + 0x3F5u);
        if ((index == 0 && (detachFlags & 1u) != 0) ||
            (index == 1 && (detachFlags & 2u) != 0) ||
            (index == 2 && (detachFlags & 4u) != 0)) {
            part->mState = 1;
            if (index == 0) {
                part->mCountDown = 13;
                gabi::call(0x025F1884, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), static_cast<s16>(actor->current.angle.y));
                offset->x = zero;
                offset->y = zero;
                offset->z = gabi::load<f32>(0x1047B62C) + launchSpeed;
                gabi::call(0x0200FCD8, offset.get(), &part->mVelocity);
            } else part->mCountDown = 0;
        }
    } else if (state == 1) {
        tn_selectJointMatrix(actor->mpBodyMorf, joint);
        gabi::call(0x0200FCD8, offset.get(), &part->mPosition);
        if (part->mCountDown == 0) {
            tn_playBombReactionSound(actor, 0x488C);
            part->mState = 5;
            part->mCountDown = 30;
            part->mTransformMode = 1;
            part->mAngles.y = static_cast<s16>(static_cast<s32>(actor->current.angle.y) + gabi::load<s16>(0x1047B692));
            part->mAngles.z = 0x4000;
            if (index == 0) {
                const u32 status = gabi::load<u32>(base + 0x2E0u);
                const u8 equipment = actor->mRemainingEquipmentPieces;
                const u8 count = actor->m02DC;
                gabi::store<u32>(base + 0x2E0u, status | 0x200000u);
                actor->mRemainingEquipmentPieces = equipment & 0xFEu;
                actor->m02DC = static_cast<u8>(count - 1u);
            } else if (index == 1) {
                actor->mRemainingEquipmentPieces = static_cast<u8>(actor->mRemainingEquipmentPieces) & 0xFDu;
                gabi::call(0x025F1884, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(player + 0x32Au));
                offset->x = zero;
                offset->y = horizontalBounce;
                offset->z = gabi::load<f32>(0x10040608);
                gabi::call(0x0200FCD8, offset.get(), &part->mVelocity);
                part->mAngularVelocity.x = tn_randomPartRotation(randomRotation);
                part->mAngularVelocity.y = tn_randomPartRotation(randomRotation);
                actor->m02DC = static_cast<u8>(static_cast<u8>(actor->m02DC) - 1u);
            } else {
                if (index == 2) actor->mRemainingEquipmentPieces = static_cast<u8>(actor->mRemainingEquipmentPieces) & 0xFBu;
                actor->m02DC = static_cast<u8>(static_cast<u8>(actor->m02DC) - 1u);
            }
        }
    } else if (state == 5) {
        tn_moveFlyingPart(actor, part, index, ground.get(), zero, horizontalBounce, randomRotation);
    }
    J3DModel* model = index == 0 ? actor->mpArmorMorf->getModel() : static_cast<J3DModel*>(part->mpPartModel);
    const s8 transformMode = part->mTransformMode;
    if (transformMode == 1) {
        gabi::call(0x0200FAD8, 0, static_cast<f32>(part->mPosition.x), static_cast<f32>(part->mPosition.y), static_cast<f32>(part->mPosition.z));
        gabi::call(0x025F1C28, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), static_cast<s16>(part->mAngles.y));
        gabi::call(0x025F1BF4, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), static_cast<s16>(part->mAngles.x));
        gabi::call(0x025F1C5C, gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), static_cast<s16>(part->mAngles.z));
        mtx_copy(gabi::at<Mtx34>(gabi::ea(model) + 0xC8u), gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)));
    } else if (transformMode == 0) {
        tn_selectJointMatrix(actor->mpBodyMorf, joint);
        s16 wobble = gabi::load<s16>(base + 0x1552u);
        if (index == 0 && wobble != 0) {
            const u32 firstAngle = (static_cast<u32>(static_cast<s32>(wobble)) * 0x5100u & 0xFFFFu) & ~7u;
            const f32 cosine = gabi::load<f32>(0x104A44FCu + firstAngle);
            const f32 scalar = cosine * static_cast<f32>(wobble);
            const f32 factor = gabi::load<f32>(0x10040628);
            const f32 one = gabi::load<f32>(0x100404FC);
            const f32 scale = gabi::fmadds(scalar, factor, one);
            gabi::call(0x0200FC74, 1, scale, scale, scale);
            wobble = gabi::load<s16>(base + 0x1552u);
            const u32 secondAngle = (static_cast<u32>(static_cast<s32>(wobble)) * 0x3900u & 0xFFFFu) & ~7u;
            const f32 baseAngle = gabi::load<f32>(0x1004062C);
            const f32 tuning = gabi::load<f32>(0x1047B650);
            const f32 amplitude = static_cast<f32>(wobble) * (tuning + baseAngle);
            const f32 cosine2 = gabi::load<f32>(0x104A44FCu + secondAngle);
            gabi::call(0x0200FBA4, 1, cosine2 * amplitude);
            wobble = gabi::load<s16>(base + 0x1552u);
            const u32 thirdAngle = (static_cast<u32>(static_cast<s32>(wobble)) * 0x4200u & 0xFFFFu) & ~7u;
            const f32 sine = gabi::load<f32>(0x104A44F8u + thirdAngle);
            gabi::call(0x0200FB3C, 1, sine * amplitude);
        }
        mtx_copy(gabi::at<Mtx34>(gabi::ea(model) + 0xC8u), gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)));
    }
    if (index == 0) {
        actor->mpArmorMorf->calc();
        const u32 mantleId = actor->mMantPcId;
        if (mantleId != 0xFFFFu) {
            gabi::Local<be<u32>> id;
            *id = mantleId;
            const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : gabi::call<u32>(0x025D5218, 0x025E1234u, id.get());
            if (mantle != 0) {
                tn_selectJointMatrix(actor->mpArmorMorf, static_cast<u32>(static_cast<s32>(gabi::load<s16>(0x1047B9F2)) + 6));
                offset->y = gabi::load<f32>(0x1047B614) + launchSpeed;
                const f32 offsetX = gabi::load<f32>(0x1047B610);
                const f32 xConstant = gabi::load<f32>(0x10040630);
                const f32 offsetZ = gabi::load<f32>(0x1047B618);
                offset->x = offsetX + xConstant;
                offset->z = offsetZ + gabi::load<f32>(0x10040634);
                gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(mantle + 0x362Cu));
                tn_selectJointMatrix(actor->mpArmorMorf, static_cast<u32>(static_cast<s32>(gabi::load<s16>(0x1047B9F4)) + 4));
                const f32 secondX = gabi::load<f32>(0x1047B61C);
                const f32 secondConstant = gabi::load<f32>(0x10040630);
                const f32 secondY = gabi::load<f32>(0x1047B620);
                const f32 zConstant = gabi::load<f32>(0x10040638);
                offset->x = secondX + secondConstant;
                offset->y = secondY + launchSpeed;
                offset->z = gabi::load<f32>(0x1047B624) + zConstant;
                gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(mantle + 0x3638u));
                for (u32 i = 0; i < 3; ++i) gabi::store<u32>(mantle + 0x314u + 4u * i, gabi::load<u32>(base + 0x37Cu + 4u * i));
                for (u32 i = 0; i < 3; ++i) gabi::store<u16>(mantle + 0x320u + 2u * i, gabi::load<u16>(base + 0x320u + 2u * i));
            }
        }
    }
    if (index < 3) gabi::store<f32>(gabi::ea(static_cast<mDoExt_brkAnm*>(part->mpPartBrkAnm)) + 4u, static_cast<f32>(static_cast<u8>(actor->mArmorColorIndex)));
    tn_destroyGroundCheck(ground.get());
}
VERIFY(0x024BC3EC, part_move);

static bool tn_stageEquals(u32 name) {
    gabi::Local<SafeString> expected;
    gabi::Local<SafeString> stage;
    expected->mStringTop = name;
    expected->__vtbl = TN_SAFESTRING_VTABLE;
    const u32 play = gabi::ea(dComIfGp_get());
    stage->mStringTop = play + 0x5134u;
    stage->__vtbl = TN_SAFESTRING_VTABLE;
    gabi::call(gabi::load<u32>(static_cast<u32>(expected->__vtbl) + 20u), expected.get());
    gabi::call(gabi::load<u32>(static_cast<u32>(expected->__vtbl) + 20u), expected.get());
    const u32 left = expected->mStringTop;
    gabi::call(gabi::load<u32>(static_cast<u32>(stage->__vtbl) + 20u), stage.get());
    const u32 right = stage->mStringTop;
    if (left == right) return true;
    for (u32 i = 0; i < 0x40001u; ++i) {
        const u8 a = gabi::load<u8>(left + i);
        const u8 b = gabi::load<u8>(right + i);
        if (a != b) return false;
        if (a == 0) return true;
    }
    return false;
}

static void tn_initializeColliders(tn_class* actor, u8 equipment) {
    const u32 base = gabi::ea(actor);
    const u32 status = base + 0xCE8u;
    const u8 health = (equipment & 1u) != 0 ? 30 : 15;
    gabi::store<u8>(base + 0x3A1u, health);
    gabi::store<u8>(base + 0x3A0u, health);
    gabi::call(0x02515F14, gabi::at<dCcD_Stts>(status), 0xF0, 0xFF, actor);
    gabi::call(0x02516518, &actor->mCoCyl, gabi::at<u8>(0x101D27BC));
    gabi::store<u32>(base + 0xE08u, status);
    gabi::call(0x02516518, &actor->mTgCyl, gabi::at<u8>(0x101D2800));
    gabi::store<u32>(base + 0xF38u, status);
    gabi::call(0x0251677C, &actor->mHeadSph, gabi::at<u8>(0x101D2688));
    gabi::store<u32>(base + 0x1068u, status);
    gabi::call(0x0251677C, &actor->mWeponSph, gabi::at<u8>(0x101D26C8));
    gabi::store<u32>(base + 0x12C0u, status);
    gabi::call(0x0251677C, &actor->mWepon2Sph, gabi::at<u8>(0x101D2708));
    gabi::store<u32>(base + 0x13ECu, status);
    gabi::call(0x0251677C, &actor->mDefenceSph, gabi::at<u8>(0x101D2748));
    gabi::store<u32>(base + 0x1194u, status);
}

int daTn_Create(tn_class* actor) {
    WWHD_FUNC(0x024C0B0C, int, actor);
    const u32 base = gabi::ea(actor);
    u32 constructionFlags = gabi::load<u32>(base + 0x2E4u);
    if ((constructionFlags & 8u) == 0) {
        if (actor != nullptr) {
            tn_class_construct(actor);
            constructionFlags = gabi::load<u32>(base + 0x2E4u);
        }
        gabi::store<u32>(base + 0x2E4u, constructionFlags | 8u);
    }
    int phase = dComIfG_resLoad(&actor->mPhaseTkwn, STR(0x100406E8));
    if (phase != 4) return phase;
    phase = dComIfG_resLoad(&actor->mPhaseTn, STR(0x100406F0));
    if (phase != 4) return phase;
    gabi::store<u8>(base + 0x582u, 1);
    gabi::store<u8>(base + 0x2DEu, 3);
    bool specialStage = tn_stageEquals(0x100406D8);
    if (!specialStage) specialStage = tn_stageEquals(0x100406F4);
    if (!specialStage) specialStage = tn_stageEquals(0x100406FC);
    gabi::store<u8>(0x1046E73C, specialStage ? 1 : 0);
    actor->mBehaviorType = gabi::load<u32>(base + 0xB0u) & 0xFu;
    actor->mArmorColorIndex = gabi::load<u32>(base + 0xB0u) >> 4 & 0xFu;
    gabi::store<u8>(base + 0x3D9u, gabi::load<u32>(base + 0xB0u) >> 8);
    actor->mPathIndex = gabi::load<u32>(base + 0xB0u) >> 16;
    gabi::store<u8>(base + 0x3DBu, gabi::load<u32>(base + 0xB0u) >> 24);
    const u8 deathSwitch = gabi::load<u8>(base + 0x325u);
    gabi::store<u8>(base + 0x3DCu, deathSwitch);
    if (gabi::load<s16>(0x1047B688) != 0 || (deathSwitch > 0 && deathSwitch <= 0x7F)) {
        gabi::store<u32>(base + 0x2E0u, gabi::load<u32>(base + 0x2E0u) | 0x04000000u);
        gabi::store<u8>(0x1046E73C, 1);
    }
    const u8 spawnSwitch = gabi::load<u8>(base + 0x3DBu);
    if (spawnSwitch != 0xFF) {
        gabi::store<u8>(base + 0x3DDu, static_cast<u8>(spawnSwitch + 1u));
        gabi::store<u32>(base + 0x2E0u, gabi::load<u32>(base + 0x2E0u) | 0x4000u);
        gabi::store<u8>(base + 0x1670u, 10);
    }
    actor->mEquipmentType = static_cast<u16>(actor->current.angle.x) >> 5 & 7u;
    actor->current.angle.z = 0;
    actor->current.angle.x = 0;
    if (actor->mArmorColorIndex > 5) actor->mArmorColorIndex = 5;
    u8 equipment = actor->mEquipmentType;
    if (equipment > 5) { equipment = 5; actor->mEquipmentType = equipment; }
    actor->mRemainingEquipmentPieces = equipment < 2 ? 0xB : 0xF;
    u8 switchNumber = gabi::load<u8>(base + 0x3DCu);
    if (switchNumber == 0xFF) {
        gabi::store<u8>(base + 0x3DCu, 0);
    } else if (switchNumber != 0) {
        const u32 save = gabi::load<u32>(0x101F84DC);
        if (gabi::call<BOOL>(0x025BA0C0, gabi::at<u8>(save + 0x20u), switchNumber, static_cast<s8>(actor->current.roomNo)) != 0) return 5;
    }
    gabi::store<u8>(0x1046E73D, 0);
    u8 behavior = actor->mBehaviorType;
    const f32 initialGroundHeight = gabi::load<f32>(0x100406C8);
    if (behavior == 13 || behavior == 14) {
        gabi::store<u32>(base + 0x2E0u, gabi::load<u32>(base + 0x2E0u) | 0x04000000u);
        gabi::store<u8>(0x1046E73C, 1);
        if (actor->mBehaviorType == 13) actor->mActionMode = 0;
        else {
            actor->mActionMode = 10;
            const f32 y = actor->current.pos.y;
            gabi::store<u32>(base + 0x2E0u, gabi::load<u32>(base + 0x2E0u) | 0x4000u);
            actor->current.pos.y = y + initialGroundHeight;
        }
        gabi::store<u8>(base + 0x3DDu, 0);
        actor->mBehaviorType = 0;
        actor->mAction = 25;
        behavior = 0;
    } else if (behavior == 12) {
        actor->mBehaviorType = 0;
        actor->mActionMode = 20;
        actor->mAction = 25;
        behavior = 0;
    }
    if (behavior == 4) actor->mAction = 1;
    else if (behavior == 15) {
        gabi::store<u8>(base + 0x3DDu, 0);
        actor->mAction = 23;
    }
    const u32 play = gabi::ea(dComIfGp_get());
    const u32 sound = gabi::call<u32>(0x0200E814, gabi::at<u8>(play + 0x50ACu), STR(0x100406F0), 0);
    gabi::store<u32>(base + 0x3A4u, sound);
    if (gabi::call<BOOL>(0x025D63E8, actor, 0x024C02FCu, 0x29808) == 0) return 5;
    if (gabi::load<u8>(0x101D2468) == 0) {
        const u8 hio = gabi::call<u8>(0x025F0A10, STR(0x1004070C), gabi::at<u8>(0x1046E778));
        gabi::store<u8>(0x1046E778, hio);
        actor->mHioRegistered = 1;
        gabi::store<u8>(0x101D2468, 1);
    }
    const f32 minY = gabi::load<f32>(0x1004055C);
    const f32 minZ = gabi::load<f32>(0x100406CC);
    const f32 minX = gabi::load<f32>(0x1004067C);
    gabi::call(0x025D672C, actor, minX, minY, minZ);
    const f32 maxX = gabi::load<f32>(0x100406D0);
    const f32 maxYZ = gabi::load<f32>(0x10040698);
    gabi::call(0x025D673C, actor, maxX, maxYZ, maxYZ);
    const u32 bodyModel = gabi::ea(actor->mpBodyMorf->getModel());
    const f32 culling = gabi::load<f32>(0x100406D4);
    gabi::store<f32>(base + 0x3B0u, maxX);
    gabi::store<f32>(base + 0x3ACu, culling);
    gabi::store<u32>(base + 0x348u, bodyModel == 0 ? 0 : bodyModel + 0xC8u);
    gabi::store<u8>(base + 0xCD0u, 1);
    const f32 y = actor->current.pos.y;
    gabi::store<f32>(base + 0xCE4u, initialGroundHeight);
    gabi::store<f32>(base + 0xCA8u, y);
    gabi::store<s16>(base + 0xA92u, 5);
    gabi::store<u32>(base + 0x39Cu, 4);
    if (actor->mPathIndex != 0xFF) {
        actor->mpPath = gabi::call<tn_path*>(0x025AAF88, static_cast<u8>(actor->mPathIndex), static_cast<s8>(actor->current.roomNo));
        if (actor->mpPath == nullptr) return 5;
        const u8 pathIndex = actor->mPathIndex;
        actor->mPathPointLookahead = 1;
        actor->mActivePath = static_cast<u8>(pathIndex + 1u);
    }
    gabi::call(0x024F06B4, gabi::at<u8>(base + 0xAE0u), &actor->current.pos,
               gabi::at<cXyz>(base + 0x300u), actor, 1, gabi::at<u8>(base + 0xAA0u),
               gabi::at<cXyz>(base + 0x33Cu), 0, 0);
    const f32 wallRadius = gabi::load<f32>(0x100405AC);
    gabi::call(0x024EFF44, gabi::at<u8>(base + 0xAA0u), wallRadius, wallRadius);
    const u32 acchFlags = gabi::load<u32>(base + 0xB08u);
    const f32 wallHeight = gabi::load<f32>(0x100404F4);
    const u8 armorType = actor->mEquipmentType;
    gabi::store<u32>(base + 0xB08u, acchFlags & ~8u);
    gabi::store<f32>(base + 0xBA0u, wallHeight);
    tn_initializeColliders(actor, armorType);
    actor->m02DC = 3;
    gabi::Local<cXyz> weaponPosition;
    const f32 weaponHeight = gabi::load<f32>(0x1047B640);
    const f32 actorY = actor->current.pos.y;
    weaponPosition->x = actor->current.pos.x;
    weaponPosition->y = actorY + weaponHeight;
    const s8 room = actor->current.roomNo;
    weaponPosition->z = actor->current.pos.z;
    actor->mWeaponActorId = fopAcM_create(0x1CF, 3, weaponPosition.get(), room, nullptr, nullptr, -1, 0);
    actor->mPathDriven = 1;
    gabi::store<u8>(base + 0x155Cu, 1);
    u32 mantle = 0xFFFF;
    if (actor->mEquipmentType >= 4) mantle = fopAcM_create(0xC0, 0, &actor->current.pos, actor->current.roomNo, nullptr, nullptr, -1, 0);
    actor->mEnemyIce.mpActor = actor;
    actor->mMantPcId = mantle;
    const f32 iceBase = gabi::load<f32>(0x10040650);
    const f32 iceTuning = gabi::load<f32>(0x1047B628);
    const f32 iceSize = gabi::load<f32>(0x100405D4);
    const f32 iceRadius = gabi::load<f32>(0x10040508);
    const u8 itemSwitch = gabi::load<u8>(base + 0x3DCu);
    gabi::store<f32>(base + 0x1814u, iceTuning + iceBase);
    const f32 iceHeight = gabi::load<f32>(0x1047B62C);
    gabi::store<u8>(base + 0x1825u, itemSwitch);
    gabi::store<f32>(base + 0x1820u, iceSize);
    const u32 body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
    actor->mEnemyFire.mpActor = actor;
    gabi::store<u32>(base + 0x1A38u, body);
    gabi::store<f32>(base + 0x1810u, iceHeight + iceRadius);
    for (u32 i = 0; i < 10; ++i) {
        const u8 joint = gabi::load<u8>(0x101D27B0u + i);
        gabi::store<u8>(base + 0x1A3Cu + i, joint);
        const f32 radius = gabi::load<f32>(0x101D2788u + 4u * i);
        gabi::store<f32>(base + 0x1A48u + 4u * i, radius);
    }
    gabi::store<f32>(base + 0x166Cu, gabi::load<f32>(0x100404FC));
    gabi::store<u8>(base + 0x3A9u, 5);
    for (int i = 0; i < 3; ++i) gabi::call(0x024BD170, actor);
    const s16 action = actor->mAction;
    gabi::store<f32>(base + 0x166Cu, gabi::load<f32>(0x10040510));
    if (action == 23) {
        const u32 save = gabi::load<u32>(0x101F84DC);
        const BOOL set = gabi::call<BOOL>(0x025B8B94, gabi::at<u8>(save + 0x644u), 0x3802);
        const u32 status = gabi::load<u32>(base + 0x2E0u);
        gabi::store<u32>(base + 0x2E0u, set != 0 ? status & ~0x4000u : status | 0x4000u);
    }
    bool hide = false;
    if (tn_stageEquals(0x100406E0) && actor->current.roomNo == 17) hide = true;
    else if (tn_stageEquals(0x10040704)) hide = true;
    if (hide) {
        gabi::store<u32>(base + 0x2E0u, gabi::load<u32>(base + 0x2E0u) | 0x4000u);
        gabi::store<u8>(base + 0x1670u, 2);
    }
    return 4;
}
VERIFY(0x024C0B0C, daTn_Create);

static BOOL tn_eventRunning(u32 name) {
    const u32 background = gabi::ea(dComIfGp_get()) + 0x12A0u;
    const u32 manager = gabi::ea(dComIfGp_get()) + 0x52C4u;
    const s32 event = gabi::call<s32>(0x02543F10, gabi::at<u8>(manager), STR(name), 0xFF);
    return gabi::call<BOOL>(0x02544044, gabi::at<u8>(background + 0x4024u), event);
}

static s32 tn_getDemoStaff() {
    const u32 manager = gabi::ea(dComIfGp_get()) + 0x52C4u;
    return gabi::call<s32>(0x02542D88, gabi::at<u8>(manager), STR(0x1004037C), 0, 0);
}

static bool tn_staffActionEquals(s32 staff, u32 expected) {
    const u32 manager = gabi::ea(dComIfGp_get()) + 0x52C4u;
    const u32 action = gabi::call<u32>(0x02544830, gabi::at<u8>(manager), staff);
    for (u32 i = 0;; ++i) {
        const u8 a = gabi::load<u8>(action + i);
        const u8 b = gabi::load<u8>(expected + i);
        if (a != b) return false;
        if (a == 0) return true;
    }
}

static void tn_setDemoAnimationRates(tn_class* actor, mDoExt_McaMorf* body, f32 speed) {
    body->mFrameCtrl.mRate = speed;
    actor->mpArmorMorf->mFrameCtrl.mRate = speed;
    actor->mpShieldMorf->mFrameCtrl.mRate = speed;
}

static bool tn_demoAnimationStopped(mDoExt_McaMorf* body, f32 zero) {
    return (gabi::load<u8>(gabi::ea(body) + 0xA7u) & 1u) != 0 || static_cast<f32>(body->mFrameCtrl.mRate) == zero;
}

void s_demo(tn_class* actor) {
    WWHD_FUNC(0x024C3348, void, actor);
    const u32 base = gabi::ea(actor);
    gabi::store<s16>(base + 0x508u, 5);
    gabi::store<u32>(base + 0x39Cu, 0);
    mDoExt_McaMorf* body = actor->mpBodyMorf;
    const f32 zero = gabi::load<f32>(0x100404E0);
    const f32 animationFrame = body->mFrameCtrl.mFrame;
    const s16 mode = actor->mActionMode;
    const s32 frame = gabi::ftoi(animationFrame);
    const f32 one = gabi::load<f32>(0x100404FC);
    const f32 blend = gabi::load<f32>(0x10040500);
    switch (mode) {
    case 0: {
        const f32 initialBlend = gabi::load<f32>(0x10040510);
        actor->mActionMode = 1;
        anm_init(actor, 0x39, initialBlend, 2, one, -1);
        yoroi_anm_init(actor, 0x64, initialBlend, 0, one);
        [[fallthrough]];
    }
    case 1: {
        if (gabi::load<s16>(0x1047B688) == 0) {
            const s8 room = actor->current.roomNo;
            const u32 save = gabi::load<u32>(0x101F84DC);
            const u8 spawnSwitch = gabi::load<u8>(base + 0x3DBu);
            if (gabi::call<BOOL>(0x025BA0C0, gabi::at<u8>(save + 0x20u), spawnSwitch, room) == 0) return;
        }
        gabi::call(0x025E1960, 30);
        anm_init(actor, 0x54, blend, 0, one, 0x24);
        yoroi_anm_init(actor, 0x56, blend, 0, one);
        actor->mActionMode = 2;
        return;
    }
    case 2:
        if (frame == 127) { tn_playBombReactionSound(actor, 0x4885); body = actor->mpBodyMorf; }
        else if (frame == 172) { gabi::call(0x025E1918, 0x80000019u); body = actor->mpBodyMorf; }
        else if (frame == 193) { tn_playBombReactionSound(actor, 0x4888); body = actor->mpBodyMorf; }
        else if (frame == 231 || frame == 262) { tn_playBombReactionSound(actor, 0x4889); body = actor->mpBodyMorf; }
        if (tn_demoAnimationStopped(body, zero)) {
            actor->mAction = 4;
            actor->mActionMode = 0;
        }
        return;
    case 10: {
        const f32 disabledHeight = gabi::load<f32>(0x1004064C);
        const f32 verticalOffset = gabi::load<f32>(0x10040640);
        gabi::store<f32>(base + 0xCE4u, disabledHeight);
        actor->current.pos.y = gabi::load<f32>(base + 0x2F0u) + verticalOffset;
        gabi::store<f32>(base + 0x340u, zero);
        if (tn_eventRunning(0x100404A0) == 0 && tn_eventRunning(0x100404D0) == 0 && gabi::load<s8>(0x1046E73D) == 0) return;
        const s32 staff = tn_getDemoStaff();
        if (gabi::load<s8>(0x1046E73D) == 0 && (staff == -1 || !tn_staffActionEquals(staff, 0x10040380))) return;
        gabi::call(0x025E1960, 30);
        const f32 z = actor->current.pos.z;
        const f32 zOffset = gabi::load<f32>(0x100405AC);
        const f32 homeX = gabi::load<f32>(base + 0x2ECu);
        const f32 x = actor->current.pos.x;
        actor->current.pos.z = z - zOffset;
        if (homeX < zero) {
            const f32 xOffset = gabi::load<f32>(0x10040698);
            const f32 yOffset = gabi::load<f32>(0x10040770);
            const f32 homeY = gabi::load<f32>(base + 0x2F0u);
            gabi::store<u8>(base + 0x1669u, 1);
            actor->current.pos.x = x + xOffset;
            actor->current.pos.y = homeY + yOffset;
        } else {
            const f32 homeY = gabi::load<f32>(base + 0x2F0u);
            const f32 xOffset = gabi::load<f32>(0x10040774);
            const f32 yOffset = gabi::load<f32>(0x10040690);
            gabi::store<u8>(base + 0x1669u, 0);
            actor->current.pos.x = x + xOffset;
            actor->current.pos.y = homeY + yOffset;
        }
        tn_playPartImpact(actor, 0x5929);
        gabi::store<u8>(0x1046E73D, static_cast<u8>(gabi::load<u8>(0x1046E73D) + 1u));
        anm_init(actor, 0x55, blend, 0, zero, 0x25);
        yoroi_anm_init(actor, 0x57, blend, 0, zero);
        tate_anm_init(actor, 0x55, blend, 2, zero);
        actor->mActionMode = 11;
        return;
    }
    case 11: {
        if (tn_eventRunning(0x100404A0) != 0 || tn_eventRunning(0x100404D0) != 0) {
            const s32 staff = tn_getDemoStaff();
            if (staff != -1 && tn_staffActionEquals(staff, 0x10040370)) {
                actor->current.pos.x = gabi::load<f32>(base + 0x2ECu);
                const u32 play = gabi::ea(dComIfGp_get());
                const u32 player = gabi::load<u32>(play + 0x5B2Cu);
                if (player != 0) {
                    const f32 dx = gabi::load<f32>(player + 0x314u) - static_cast<f32>(actor->current.pos.x);
                    const f32 dz = gabi::load<f32>(player + 0x31Cu) - static_cast<f32>(actor->current.pos.z);
                    actor->shape_angle.y = gabi::call<s16>(0x020195B0, dx, dz);
                }
            }
        }
        if ((gabi::load<u32>(base + 0xB08u) & 0x20u) == 0) return;
        tn_setDemoAnimationRates(actor, actor->mpBodyMorf, one);
        tn_playPartImpact(actor, 0x592A);
        const s8 side = gabi::load<s8>(base + 0x1669u);
        const f32 homeX = gabi::load<f32>(base + 0x2ECu);
        actor->mActionMode = side != 0 ? 13 : 12;
        const u32 vibration = gabi::ea(dComIfGp_get()) + 0x599Cu;
        gabi::Local<cXyz> direction;
        direction->x = zero;
        direction->y = one;
        direction->z = zero;
        gabi::call(0x025CB374, gabi::at<u8>(vibration), homeX < zero ? 5 : 2, -17, direction.get());
        return;
    }
    case 12:
        if (gabi::ftoi(static_cast<f32>(body->mFrameCtrl.mFrame)) == 43) {
            if (gabi::load<s8>(0x1046E73D) == 10) actor->mActionMode = 13;
            else tn_setDemoAnimationRates(actor, body, zero);
        }
        return;
    case 13: {
        tn_setDemoAnimationRates(actor, body, one);
        body = actor->mpBodyMorf;
        if (gabi::ftoi(static_cast<f32>(body->mFrameCtrl.mFrame)) == 43) gabi::store<u8>(0x1046E73D, 10);
        if (frame == 63) tn_playBombReactionSound(actor, 0x4888);
        else if (frame == 115) tn_playBombReactionSound(actor, 0x4889);
        const s32 shieldFrame = static_cast<s32>(gabi::load<s16>(0x1047B696)) + 210;
        if (static_cast<u32>(frame) == static_cast<u32>(shieldFrame)) tate_anm_init(actor, 0x3C, gabi::load<f32>(0x10040600), 2, one);
        if (tn_demoAnimationStopped(actor->mpBodyMorf, zero)) {
            actor->mAction = 4;
            actor->mActionMode = 0;
            gabi::store<f32>(base + 0xCE4u, gabi::load<f32>(0x100406C8));
            gabi::call(0x025E1918, 0x80000019u);
        }
        return;
    }
    case 20:
        actor->mActionMode = 21;
        anm_init(actor, 0x39, one, 2, one, -1);
        yoroi_anm_init(actor, 0x64, one, 0, one);
        gabi::store<s16>(base + 0x4F8u, 30);
        return;
    case 21:
        if (gabi::load<s16>(base + 0x4F8u) == 0) {
            anm_init(actor, 0x35, gabi::load<f32>(0x100405CC), 2, one, -1);
            gabi::store<s16>(base + 0x4F8u, 30);
            actor->mActionMode = 22;
            tn_playBombReactionSound(actor, 0x4888);
        }
        return;
    case 22:
        if (gabi::load<s16>(base + 0x4F8u) == 0) {
            actor->mAction = 4;
            actor->mActionMode = 2;
            gabi::store<s16>(base + 0x4FEu, 60);
        }
        return;
    default: return;
    }
}
VERIFY(0x024C3348, s_demo);

// Movement reconstruction is still incomplete. The following helpers express
// audited native subregions; none is a standalone verification entry.
// Special airborne attack modes: 024C3EF8..024C3FD8. True tells the
// movement entry to return without ordinary action dispatch or velocity setup.
static bool tn_processAirborneAttack(tn_class* actor, f32 zero) {
    if (actor->mActionMode > -100) return false;
    const u32 base = gabi::ea(actor);
    const f32 knockback = gabi::load<f32>(base + 0xA3Cu);
    const f32 activeThreshold = gabi::load<f32>(0x100405E4);
    gabi::store<s16>(base + 0x508u, 5);
    if (!(std::fabs(knockback) > activeThreshold)) return true;
    if ((gabi::load<u32>(base + 0xB08u) & 0x20u) != 0) return true;
    gabi::call(0x02018D40, gabi::at<u8>(base + 0x1394u), gabi::at<cXyz>(base + 0x72Cu));
    gabi::call(0x02018C8C, gabi::at<u8>(base + 0x1394u), gabi::load<f32>(0x100405E0));
    const u32 weaponFlags = gabi::load<u32>(base + 0x127Cu);
    const u32 attackFlags = gabi::load<u32>(base + 0x12A8u);
    gabi::store<u8>(base + 0x12EBu, 1);
    gabi::store<u32>(base + 0x127Cu, weaponFlags & ~4u);
    gabi::store<u32>(base + 0x12A8u, attackFlags | 1u);
    const u32 collider = base + 0x127Cu;
    const u32 registration = gabi::ea(dComIfGp_get()) + 0x26A4u;
    gabi::call(0x0200E240, gabi::at<u8>(registration), gabi::at<dCcD_Sph>(collider));
    const u32 collisionManager = gabi::ea(dComIfGp_get()) + 0x4EF8u;
    gabi::call(0x02516C14, gabi::at<u8>(collisionManager), gabi::at<dCcD_Sph>(collider), 3);
    if (gabi::call<BOOL>(0x025160DC, gabi::at<dCcD_Sph>(collider)) != 0) {
        const f32 verticalSpeed = gabi::load<f32>(base + 0x340u);
        const f32 stopThreshold = gabi::load<f32>(0x1004055C);
        if (verticalSpeed < stopThreshold) {
            const f32 impulse = gabi::load<f32>(0x10040694);
            gabi::store<f32>(base + 0x340u, zero);
            gabi::store<f32>(base + 0xA38u, impulse);
        }
    }
    gabi::store<u32>(base + 0xDF0u, gabi::load<u32>(base + 0xDF0u) & ~1u);
    return true;
}

// Target selection/distance and reaction cooldown: 024C409C..024C41D0.
static void tn_updateMovementTarget(tn_class* actor, fopAc_ac_c* player) {
    const u32 base = gabi::ea(actor);
    const s16 targetTimer = gabi::load<s16>(base + 0x1584u);
    cXyz* target;
    if (targetTimer != 0) {
        gabi::store<u16>(base + 0x1584u, static_cast<u16>(static_cast<u16>(targetTimer) - 1u));
        target = gabi::at<cXyz>(base + 0x1588u);
    } else {
        target = gabi::at<cXyz>(gabi::ea(player) + 0x314u);
        gabi::store<u32>(base + 0xCD8u, gabi::ea(player));
    }
    gabi::Local<cXyz> delta;
    gabi::call(0x0201ADE0, target, delta.get(), &actor->current.pos);
    const f32 dx = delta->x;
    const f32 dy = delta->y;
    const f32 dz = delta->z;
    const f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
    gabi::store<f32>(base + 0x518u, distance);
    gabi::store<f32>(base + 0x51Cu, std::fabs(dy));
    const s16 heading = gabi::call<s16>(0x020195B0, dx, dz);
    gabi::store<s16>(base + 0x532u, heading);
    const f32 radiusOffset = gabi::load<f32>(0x10040754);
    const f32 radiusTuning = gabi::load<f32>(0x1047B61C);
    gabi::call(0x020184DC, gabi::at<u8>(base + 0xEDCu), radiusTuning + radiusOffset);
    const s8 cooldown = gabi::load<s8>(base + 0x1583u);
    if (cooldown != 0) {
        const u8 next = static_cast<u8>(static_cast<u8>(cooldown) - 1u);
        gabi::store<u8>(base + 0x1583u, next);
        if (next == 0) {
            actor->mAction = 18;
            actor->mActionMode = 0;
        }
    }
}

// Normal movement preparation 024C402C..024C409C. False skips action
// dispatch for a pending reaction while retaining the common movement tail.
static bool tn_prepareNormalMovement(tn_class* actor, fopAc_ac_c* player) {
    const u32 base = gabi::ea(actor);
    const u32 weaponFlags = gabi::load<u32>(base + 0x127Cu);
    const u32 bodyFlags = gabi::load<u32>(base + 0xDF0u);
    const s16 reaction = gabi::load<s16>(base + 0xA4Eu);
    gabi::store<u32>(base + 0x127Cu, weaponFlags | 4u);
    gabi::store<u32>(base + 0xDF0u, bodyFlags | 1u);
    if (reaction != 0 && gabi::load<s16>(base + 0xA4Cu) == 0) {
        if (reaction == 1) {
            gabi::store<s16>(base + 0x4FAu, 0);
            actor->mAction = 4;
            actor->mActionMode = 0;
        } else {
            const f32 reactionSpeed = gabi::load<f32>(0x10040618);
            const s16 heading = gabi::load<s16>(base + 0x532u);
            gabi::store<s16>(base + 0xA94u, heading);
            gabi::store<f32>(base + 0x370u, reactionSpeed);
        }
        return false;
    }
    tn_updateMovementTarget(actor, player);
    return true;
}

// Native actions with separately emitted implementations: dispatch at
// 024C4214..024C4278, implementations 024C692C..024C69E4.
// False identifies actions whose inline state machines remain to be authored.
static bool tn_dispatchSeparateMovementAction(tn_class* actor, s16 action) {
    switch (action) {
    case 8: p_lost(actor); break;
    case 9: b_nige(actor); break;
    case 10: defence(actor); break;
    case 11: hukki(actor); break;
    case 12: wepon_search(actor); break;
    case 14: aite_miru(actor); break;
    case 17: d_sit(actor); break;
    case 18: d_mahi(actor); break;
    case 20: fail(actor); break;
    case 21: yogan_fail(actor); break;
    case 23: d_dozou(actor); break;
    case 25: s_demo(actor); break;
    case 0: case 1: case 4: case 5: return false;
    default: break;
    }
    return true;
}

// Common home-range transition and horizontal velocity: 024C69E8..024C6B5C.
// The movement entry retains the zero/range constants and action value across
// action calls; they must be passed here rather than reloaded prematurely.
static void tn_finishMovement(tn_class* actor, s16 action, f32 zero, f32 rangeScale) {
    const u32 base = gabi::ea(actor);
    if (actor->mBehaviorType == 4 && action == 4) {
        gabi::Local<cXyz> homeDelta;
        gabi::call(0x0201ADE0, gabi::at<cXyz>(base + 0x2ECu), homeDelta.get(), &actor->current.pos);
        const u8 range = actor->mRangeOrFrozenAnim;
        f32 maxDistance;
        if (range != 0xFF) {
            const f32 scaledRange = static_cast<f32>(range) * rangeScale;
            maxDistance = scaledRange * gabi::load<f32>(0x100405D4);
        } else {
            maxDistance = gabi::load<f32>(0x1004078C);
        }
        const f32 squaredDistance = gabi::call<f32>(0x028E8DD0, homeDelta.get());
        const f32 distance = gabi::call<f32>(0x028F4384, squaredDistance);
        if (distance > maxDistance) {
            if (actor->mBehaviorType == 4) actor->mAction = 1;
            gabi::store<s16>(base + 0x4FAu, 0);
            gabi::store<s16>(base + 0x4FCu, 60);
            actor->mActionMode = 51;
        }
        action = actor->mAction;
    }
    gabi::Local<cXyz> forward;
    forward->y = zero;
    forward->x = zero;
    forward->z = gabi::load<f32>(base + 0x370u);
    s16 heading;
    if (action != 11 && action != 20 && gabi::load<s16>(base + 0xA4Eu) == 0) {
        const s16 currentHeading = gabi::load<s16>(base + 0x322u);
        const s16 headingOffset = gabi::load<s16>(base + 0x534u);
        gabi::store<s16>(base + 0xA46u, currentHeading);
        const s16 reloadedHeading = gabi::load<s16>(base + 0x322u);
        heading = static_cast<s16>(static_cast<u16>(reloadedHeading) + static_cast<u16>(headingOffset));
    } else {
        heading = gabi::load<s16>(base + 0xA94u);
    }
    const u32 matrix = gabi::load<u32>(0x1018C7B0);
    gabi::call(0x025F1884, gabi::at<u8>(matrix), heading);
    gabi::Local<cXyz> velocity;
    gabi::call(0x0200FCD8, forward.get(), velocity.get());
    gabi::store<f32>(base + 0x33Cu, static_cast<f32>(velocity->x));
    gabi::store<f32>(base + 0x344u, static_cast<f32>(velocity->z));
}

// Action 1: return to the home position, then resume watching the target.
// Complete inline native action region 024C4A7C..024C4F70; still unused until
// the surrounding movement entry is complete and still excluded from coverage.
static void tn_moveReturnHome(tn_class* actor, f32 zero, f32 one, f32 rangeScale, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    dComIfGp_get();
    const f32 groundOffset = gabi::load<f32>(0x100406C8);
    gabi::store<u8>(base + 0xCD4u, 0);
    u32 contact = gabi::call<u32>(0x025D9DA0, actor, groundOffset);
    const u32 mantleId = actor->mMantPcId;
    if (mantleId != 0xFFFF) {
        gabi::Local<be<u32>> id;
        *id = mantleId;
        const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : gabi::call<u32>(0x025D5218, 0x025E1234u, id.get());
        if (mantle != 0 && gabi::load<s16>(mantle + 0x365Au) != 0) contact |= 2u;
    }
    s16 mode = actor->mActionMode;
    switch (mode) {
    case 0: {
        const s32 pathDriven = actor->mPathDriven;
        actor->mActionMode = 1;
        anm_init(actor, pathDriven == 0 ? 0x3D : 0x39, gabi::load<f32>(base + 0x166Cu), 2, one, -1);
        yoroi_anm_init(actor, 0x64, gabi::load<f32>(base + 0x166Cu), 0, one);
        [[fallthrough]];
    }
    case 1: {
        const f32 vx = gabi::load<f32>(base + 0x33Cu);
        const f32 stepScale = gabi::load<f32>(0x1004075C);
        const f32 homeX = gabi::load<f32>(base + 0x2ECu);
        const f32 approachScale = gabi::load<f32>(0x10040504);
        cLib_addCalc2(&actor->current.pos.x, homeX, approachScale, vx * stepScale);
        const f32 reloadedStep = gabi::load<f32>(0x1004075C);
        const f32 vz = gabi::load<f32>(base + 0x344u);
        const f32 homeZ = gabi::load<f32>(base + 0x2F4u);
        cLib_addCalc2(&actor->current.pos.z, homeZ, gabi::load<f32>(0x10040504), vz * reloadedStep);
        const s16 homeHeading = gabi::load<s16>(base + 0x2FAu);
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), homeHeading, 2, 0x800);
        gabi::store<f32>(base + 0x370u, zero);
        break;
    }
    case 10: {
        s16 timer = gabi::load<s16>(base + 0x4FAu);
        if (timer == 30) {
            anm_init(actor, 0x36, gabi::load<f32>(0x10040500), 0, one, -1);
            timer = gabi::load<s16>(base + 0x4FAu);
        }
        if (timer == 0) {
            anm_init(actor, 0x37, rangeScale, 0, one, -1);
            actor->mActionMode = 11;
            const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x100405C4));
            const f32 minimum = gabi::load<f32>(0x100405C4);
            gabi::store<s16>(base + 0x4FEu, static_cast<s16>(gabi::ftoi(random + minimum)));
        }
        break;
    }
    case 11:
        if (gabi::load<s16>(base + 0x4FEu) == 0) actor->mActionMode = 0;
        break;
    case 20:
        if (gabi::load<s16>(base + 0x4FAu) < 10) {
            gabi::store<u8>(base + 0xCD4u, 1);
            if (gabi::load<s16>(base + 0x4FAu) == 0) {
                gabi::store<s16>(base + 0x4FAu, 0);
                actor->mAction = 4;
                actor->mActionMode = 0;
                return;
            }
        }
        break;
    case 50:
        anm_init(actor, 0x37, rangeScale, 0, one, -1);
        mode = actor->mActionMode;
        gabi::store<s16>(base + 0x4FAu, 50);
        actor->mActionMode = static_cast<s16>(static_cast<u16>(mode) + 1u);
        [[fallthrough]];
    case 51:
        gabi::store<f32>(base + 0x370u, zero);
        if (gabi::load<s16>(base + 0x4FAu) == 0) {
            fight_run_set(actor);
            mode = actor->mActionMode;
            gabi::store<s16>(base + 0x4FCu, 60);
            actor->mActionMode = static_cast<s16>(static_cast<u16>(mode) + 1u);
        }
        break;
    case 52: {
        gabi::Local<cXyz> homeDelta;
        gabi::call(0x0201ADE0, gabi::at<cXyz>(base + 0x2ECu), homeDelta.get(), &actor->current.pos);
        const f32 dx = homeDelta->x;
        const f32 dz = homeDelta->z;
        const s16 heading = gabi::call<s16>(0x020195B0, dx, dz);
        gabi::store<s16>(base + 0xA94u, heading);
        const f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
        const f32 runSpeed = gabi::load<f32>(0x1046E7C8);
        const f32 stepScale = gabi::load<f32>(0x1004075C);
        const f32 closeDistance = (runSpeed * stepScale) * acceleration;
        const s16 targetHeading = gabi::load<s16>(base + 0xA94u);
        if (distance < closeDistance) actor->mActionMode = 0;
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), targetHeading, 4, 0x1000);
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), gabi::load<f32>(0x1046E7C8), one, acceleration);
        break;
    }
    default: break;
    }
    mode = actor->mActionMode;
    if (mode < 10 && contact != 0) {
        actor->mActionMode = 10;
        const f32 random = gabi::call<f32>(0x020198D8, rangeScale);
        const f32 minimum = gabi::load<f32>(0x10040778);
        gabi::store<s16>(base + 0x4FAu, static_cast<s16>(gabi::ftoi(random + minimum)));
    }
    if (gabi::load<s16>(base + 0x4FCu) == 0 && actor->mActionMode != 20) {
        const u8 range = actor->mRangeOrFrozenAnim;
        const f32 distance = gabi::load<f32>(base + 0x518u);
        const f32 detectionRange = range == 0xFF ? gabi::load<f32>(0x1004072C) : static_cast<f32>(range) * rangeScale;
        bool visible = false;
        if (distance < detectionRange) {
            const s16 viewLimit = get_view_H(actor);
            const u32 target = gabi::load<u32>(base + 0xCD8u);
            const s16 heading = gabi::load<s16>(base + 0x532u);
            visible = daTn_player_view_check(actor, gabi::at<cXyz>(target + 0x314u), heading, viewLimit) != 0;
        }
        if (visible) {
            if (actor->mActionMode >= 50) {
                gabi::store<s16>(base + 0x4FAu, 0);
                actor->mAction = 4;
                actor->mActionMode = 0;
            } else {
                const f32 blend = gabi::load<f32>(0x10040500);
                actor->mActionMode = 20;
                anm_init(actor, 0x36, blend, 0, one, -1);
                gabi::store<s16>(base + 0x4FAu, 30);
                if (base != 0) tn_playBombReactionSound(actor, 0x4885);
            }
        }
        if (daTn_checkNearbyBomb(actor) != 0) {
            actor->mAction = 9;
            actor->mActionMode = 0;
        }
    }
    if (actor->mPathDriven == 0 && search_wepon(actor) != 0) {
        actor->mAction = 12;
        actor->mActionMode = -1;
    }
}

// Path target advancement shared by the patrol action's entry modes.
// Native subregion 024C4398..024C4588. Valid actor paths must provide the
// indexed point record; missing linked paths enter the native assertion call.
static void tn_advancePatrolTarget(tn_class* actor) {
    const u32 base = gabi::ea(actor);
    const s32 pathDriven = actor->mPathDriven;
    actor->mActionMode = 1;
    if (pathDriven == 0 && actor->mPathSearchEnabled == 0) {
        way_pos_check(actor, gabi::at<cXyz>(base + 0x520u));
        const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040664));
        gabi::store<s16>(base + 0x4FAu, static_cast<s16>(gabi::ftoi(random + gabi::load<f32>(0x100405C4))));
    } else if (static_cast<s8>(actor->mActivePath) == 0) {
        way_pos_check(actor, gabi::at<cXyz>(base + 0x520u));
    } else {
        const u8 oldPoint = gabi::load<u8>(base + 0x1575u);
        const u8 lookahead = actor->mPathPointLookahead;
        s8 pointIndex = static_cast<s8>(static_cast<u8>(oldPoint + lookahead));
        u32 path = gabi::ea(static_cast<tn_path*>(actor->mpPath));
        actor->mPathPoint = pointIndex;
        const s8 countLow = gabi::load<s8>(path + 1u);
        if (pointIndex >= countLow) {
            if ((gabi::load<u8>(path + 5u) & 1u) != 0) {
                pointIndex = 0;
                actor->mPathPoint = 0;
            } else {
                actor->mPathPointLookahead = 0xFF;
                pointIndex = static_cast<s8>(static_cast<u8>(gabi::load<u16>(path) - 2u));
                actor->mPathPoint = pointIndex;
            }
            const u16 nextPath = gabi::load<u16>(path + 2u);
            if (nextPath != 0xFFFF) {
                tn_path* linked = gabi::call<tn_path*>(0x025AAF88, nextPath, static_cast<s8>(actor->current.roomNo));
                actor->mpPath = linked;
                if (linked == nullptr) gabi::call(0x0273AA24, STR(0x100404B0), 0x7CC, STR(0x100404BC));
                pointIndex = actor->mPathPoint;
                path = gabi::ea(static_cast<tn_path*>(actor->mpPath));
            }
        } else if (pointIndex < 0) {
            path = gabi::ea(static_cast<tn_path*>(actor->mpPath));
            actor->mPathPointLookahead = 1;
            actor->mPathPoint = 1;
            pointIndex = 1;
        }
        const u32 points = gabi::load<u32>(path + 8u);
        const u32 point = points + static_cast<u32>(static_cast<s32>(pointIndex)) * 16u;
        gabi::store<f32>(base + 0x520u, gabi::load<f32>(point + 4u));
        gabi::store<f32>(base + 0x524u, gabi::load<f32>(point + 8u));
        const f32 z = gabi::load<f32>(point + 12u);
        gabi::store<s16>(base + 0x4FCu, 30);
        gabi::store<f32>(base + 0x528u, z);
        return;
    }
    gabi::store<s16>(base + 0x4FCu, 30);
}

static u32 tn_currentPatrolPoint(tn_class* actor) {
    const u32 path = gabi::ea(static_cast<tn_path*>(actor->mpPath));
    const s8 index = actor->mPathPoint;
    return gabi::load<u32>(path + 8u) + static_cast<u32>(static_cast<s32>(index)) * 16u;
}

static u32 tn_patrolContact(tn_class* actor) {
    const f32 offset = gabi::load<f32>(0x100406C8);
    u32 contact = gabi::call<u32>(0x025D9DA0, actor, offset);
    const u32 mantleId = actor->mMantPcId;
    if (mantleId != 0xFFFF) {
        gabi::Local<be<u32>> id;
        *id = mantleId;
        const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : gabi::call<u32>(0x025D5218, 0x025E1234u, id.get());
        if (mantle != 0 && gabi::load<s16>(mantle + 0x365Au) != 0) contact |= 2u;
    }
    return contact;
}

// Patrol locomotion: 024C4588..024C4780. Speed and turn limits distinguish
// ordinary wandering from path patrol; reaching selected path points pauses.
static void tn_patrolLocomotion(tn_class* actor, f32 one, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    const bool usePath = actor->mPathDriven != 0 || actor->mPathSearchEnabled != 0;
    const f32 targetX = gabi::load<f32>(base + 0x520u);
    const f32 currentX = actor->current.pos.x;
    const f32 currentZ = actor->current.pos.z;
    const f32 dx = targetX - currentX;
    const f32 dz = gabi::load<f32>(base + 0x528u) - currentZ;
    const f32 speed = gabi::load<f32>(usePath ? 0x1046E7C0 : 0x1046E7C4);
    const s32 turnLimit = usePath ? 0x400 : 0x1000;
    const s16 heading = gabi::call<s16>(0x020195B0, dx, dz);
    const s8 activePath = static_cast<s8>(actor->mActivePath);
    gabi::store<s16>(base + 0xA94u, heading);
    const bool followsPath = activePath != 0 && (actor->mPathDriven != 0 || actor->mPathSearchEnabled != 0);
    const f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
    const f32 stepScale = gabi::load<f32>(0x1004075C);
    if (followsPath) {
        const f32 arrivalDistance = (speed * stepScale) * gabi::load<f32>(0x100405CC);
        if (distance < arrivalDistance) {
            const u32 point = tn_currentPatrolPoint(actor);
            const u8 type = gabi::load<u8>(point + 3u);
            if (type == 3 || type == 7 || type == 8) {
                wait_set(actor);
                if (gabi::load<u8>(point + 3u) >= 7) {
                    const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040754));
                    gabi::store<s16>(base + 0x4FAu, static_cast<s16>(gabi::ftoi(random + gabi::load<f32>(0x10040650))));
                }
                actor->mActionMode = 2;
            } else {
                actor->mActionMode = -1;
            }
        }
    } else {
        const f32 step = speed * stepScale;
        bool wait = distance < step + step;
        if (!wait && gabi::load<s16>(base + 0x4FCu) == 0) {
            wait = (gabi::load<u32>(base + 0xB08u) & 0x10u) != 0;
            if (!wait) {
                const s16 currentHeading = gabi::load<s16>(base + 0x322u);
                wait = gabi::call<BOOL>(0x024BA6D8, actor, 1, currentHeading, gabi::load<f32>(0x10040508)) != 0;
            }
        }
        if (wait) {
            wait_set(actor);
            actor->mActionMode = 2;
        }
    }
    const s16 desiredHeading = gabi::load<s16>(base + 0xA94u);
    gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), desiredHeading, 4, turnLimit);
    cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), speed, one, acceleration);
}

// Action 0: path or free patrol, pauses and target/bomb/weapon detection.
// Native region 024C427C..024C4A7C. Not wired to a complete movement entry yet.
static void tn_movePatrol(tn_class* actor, f32 zero, f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    dComIfGp_get();
    if (actor->mBehaviorType == 4) {
        actor->mAction = 1;
        actor->mActionMode = 50;
        return;
    }
    const f32 radiusTuning = gabi::load<f32>(0x1047B648);
    const f32 radiusBase = gabi::load<f32>(0x100405E4);
    gabi::call(0x020184DC, gabi::at<u8>(base + 0xEDCu), radiusTuning + radiusBase);
    const s16 mode = actor->mActionMode;
    switch (mode) {
    case -10:
        gabi::store<s16>(base + 0x4FAu, 60);
        actor->mActionMode = -9;
        [[fallthrough]];
    case -9: {
        const s16 timer = gabi::load<s16>(base + 0x4FAu);
        if (timer == 0) actor->mActionMode = 2;
        gabi::store<f32>(base + 0x370u, zero);
        if (timer > 30) return;
        break;
    }
    case 0:
        anm_init(actor, 0x3E, blend, 2, one, 0x11);
        yoroi_anm_init(actor, 0x67, blend, 2, one);
        [[fallthrough]];
    case -1:
        tn_advancePatrolTarget(actor);
        [[fallthrough]];
    case 1:
        tn_patrolLocomotion(actor, one, acceleration);
        break;
    case 2: {
        const s16 timer = gabi::load<s16>(base + 0x4FAu);
        gabi::store<f32>(base + 0x370u, zero);
        if (timer != 0) break;
        if (static_cast<s8>(actor->mActivePath) != 0 && (actor->mPathDriven != 0 || actor->mPathSearchEnabled != 0)) {
            const u32 point = tn_currentPatrolPoint(actor);
            const u8 type = gabi::load<u8>(point + 3u);
            if (type == 7 || type == 8) {
                actor->mActionMode = 4;
                const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x100404F4));
                const f32 minimum = gabi::load<f32>(0x100404F4);
                gabi::store<s16>(base + 0x4FAu, static_cast<s16>(gabi::ftoi(random + minimum)));
                const u8 pointType = gabi::load<u8>(point + 3u);
                const u16 heading = gabi::load<u16>(base + 0xA94u);
                const u16 offset = pointType == 7 ? 0xC000 : 0x4000;
                gabi::store<u16>(base + 0xA94u, static_cast<u16>(heading + offset));
                break;
            }
        }
        const s32 pathDriven = actor->mPathDriven;
        actor->mActionMode = 0;
        if (pathDriven == 0 && actor->mPathSearchEnabled == 0 && gabi::load<s16>(base + 0x502u) == 0) actor->mPathSearchEnabled = 1;
        break;
    }
    case 4:
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), gabi::load<s16>(base + 0xA94u), 4, 0x1000);
        [[fallthrough]];
    case 3:
        gabi::store<f32>(base + 0x370u, zero);
        if (gabi::load<s16>(base + 0x4FAu) == 0) actor->mActionMode = 0;
        break;
    default: break;
    }
    const u32 contact = tn_patrolContact(actor);
    s32 pathDriven = actor->mPathDriven;
    const s8 encounter = gabi::load<s8>(0x1046E73C);
    const u32 reaction = contact + static_cast<u32>(static_cast<s32>(encounter));
    if (pathDriven != 0 || actor->mPathSearchEnabled != 0) {
        bool notice = reaction != 0;
        if (!notice) {
            const f32 distance = gabi::load<f32>(base + 0x518u);
            const f32 detectionRange = gabi::load<f32>(0x1046E79C);
            if (distance < detectionRange) {
                const s16 viewLimit = get_view_H(actor);
                const u32 target = gabi::load<u32>(base + 0xCD8u);
                const s16 heading = gabi::load<s16>(base + 0x532u);
                notice = daTn_player_view_check(actor, gabi::at<cXyz>(target + 0x314u), heading, viewLimit) != 0;
                if (!notice) pathDriven = actor->mPathDriven;
            }
        }
        if (notice) {
            actor->mAction = 4;
            actor->mActionMode = -10;
            if (base != 0) tn_playBombReactionSound(actor, 0x4885);
            pathDriven = actor->mPathDriven;
        }
    }
    if (pathDriven == 0 && search_wepon(actor) != 0) {
        actor->mAction = 12;
        actor->mActionMode = -1;
    }
    if (daTn_checkNearbyBomb(actor) != 0) {
        actor->mAction = 9;
        actor->mActionMode = 0;
    }
}

enum class tn_combat_step { checkRange, checkObstacles, beginAttack };

// Combat action locomotion modes, 024C4FCC..024C549C. The returned phase
// distinguishes ordinary range checks from the direct attack transition.
static tn_combat_step tn_combatLocomotion(tn_class* actor, f32 steering, f32 zero,
                                         f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    const s16 mode = actor->mActionMode;
    switch (mode) {
    case -10:
        if (gabi::load<s16>(base + 0x500u) == 0) {
            anm_init(actor, 0x36, gabi::load<f32>(0x10040500), 0, one, -1);
            yoroi_anm_init(actor, 0x63, gabi::load<f32>(0x10040500), 0, one);
            actor->mActionMode = -9;
            return tn_combat_step::checkObstacles;
        }
        actor->mActionMode = 0;
        gabi::store<s16>(base + 0x4FAu, 0);
        break;
    case -9:
        gabi::store<f32>(base + 0x370u, zero);
        if (tn_demoAnimationStopped(actor->mpBodyMorf, zero)) {
            actor->mActionMode = 0;
            gabi::store<s16>(base + 0x4FAu, 0);
        }
        break;
    case 0:
        if (gabi::load<s16>(base + 0x4FAu) != 0) {
            gabi::store<f32>(base + 0x370u, zero);
            break;
        }
        fight_run_set(actor);
        gabi::store<u32>(base + 0x1570u, 0);
        actor->mActionMode = 1;
        [[fallthrough]];
    case 1: {
        const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
        const f32 speed = armor ? gabi::load<f32>(0x1046E7C0) * gabi::load<f32>(0x100405D4) : gabi::load<f32>(0x1046E7C8);
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), speed, one, acceleration);
        const BOOL way = daTn_player_way_check(actor);
        const f32 distance = gabi::load<f32>(base + 0x518u);
        if (way != 0) {
            if (distance < gabi::load<f32>(0x1046E7A0)) {
                actor->mActionMode = 2;
                gabi::store<s16>(base + 0x4FEu, 40);
            }
        } else if (distance < gabi::load<f32>(0x1046E7A4)) {
            const f32 heightTuning = gabi::load<f32>(0x1047BBBC);
            const f32 heightBase = gabi::load<f32>(0x10040698);
            if (gabi::load<f32>(base + 0x51Cu) < heightTuning + heightBase) return tn_combat_step::beginAttack;
        }
        break;
    }
    case 2: {
        const f32 steeringThreshold = gabi::load<f32>(0x10040670);
        const s32 pathDriven = actor->mPathDriven;
        s16 nextMode;
        if (std::fabs(steering) > steeringThreshold) {
            anm_init(actor, pathDriven != 0 ? 0x3B : 0x53, blend, 2, one, pathDriven != 0 ? 0x10 : 0x23);
            nextMode = steering > zero ? 5 : 6;
        } else {
            anm_init(actor, pathDriven != 0 ? 0x3A : 0x52, blend, 2, one, pathDriven != 0 ? 0x0F : 0x22);
            const f32 closeRange = gabi::load<f32>(0x1046E7A4);
            const f32 distance = gabi::load<f32>(base + 0x518u);
            nextMode = distance < closeRange ? 4 : 3;
        }
        const f32 randomRange = gabi::load<f32>(0x10040600);
        actor->mActionMode = nextMode;
        const f32 random = gabi::call<f32>(0x020198D8, randomRange);
        const f32 minimum = gabi::load<f32>(0x10040600);
        gabi::store<s16>(base + 0x4FAu, static_cast<s16>(gabi::ftoi(random + minimum)));
        break;
    }
    case 3:
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), gabi::load<f32>(0x1046E7D0), one, gabi::load<f32>(0x10040600));
        if (gabi::load<s16>(base + 0x4FAu) == 0) actor->mActionMode = 2;
        break;
    case 4:
        if (gabi::load<u8>(base + 0x516u) == 2) {
            actor->mActionMode = 3;
            anm_init(actor, 0x3E, blend, 2, one, 0x11);
        } else {
            cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), -gabi::load<f32>(0x1046E7D0), one, gabi::load<f32>(0x10040600));
            if (gabi::load<s16>(base + 0x4FAu) == 0) actor->mActionMode = 2;
        }
        break;
    case 5: case 6: {
        const u8 obstacle = gabi::load<u8>(base + 0x516u);
        s16 side = mode;
        if (mode == 5 && obstacle == 4) {
            side = 6;
            actor->mActionMode = 6;
        } else if (mode == 6 && obstacle == 8) {
            side = 5;
            actor->mActionMode = 5;
        }
        gabi::store<u16>(base + 0x534u, side == 5 ? 0x4000 : 0xC000);
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), gabi::load<f32>(0x1046E7D4), one, gabi::load<f32>(0x100405C4));
        if (gabi::load<s16>(base + 0x4FAu) == 0) actor->mActionMode = 2;
        break;
    }
    case 8:
        gabi::store<f32>(base + 0x370u, zero);
        if (gabi::load<s16>(base + 0x4FAu) == 0) actor->mActionMode = 2;
        break;
    default: break;
    }
    return actor->mActionMode < 3 ? tn_combat_step::checkObstacles : tn_combat_step::checkRange;
}

static void tn_beginCombatAttack(tn_class* actor) {
    actor->mAction = 5;
    actor->mActionMode = 0;
}

// Combat range selection, 024C549C..024C55A0. True means that patrol or
// an attack has been selected and the rest of the combat action is skipped.
static bool tn_combatRangeTransition(tn_class* actor, fopAc_ac_c* player) {
    const u32 base = gabi::ea(actor);
    const f32 closeRange = gabi::load<f32>(0x1046E7A0);
    const f32 lostTargetMargin = gabi::load<f32>(0x100405C8);
    f32 distance = gabi::load<f32>(base + 0x518u);
    if (distance > closeRange + lostTargetMargin) {
        actor->mAction = 0;
        path_check(actor);
        wait_set(actor);
        actor->mActionMode = 0;
        return true;
    }
    if (actor->mPathDriven != 0) {
        const u32 playerVtable = gabi::load<u32>(gabi::ea(player) + 0xB4u);
        const u32 callback = gabi::load<u32>(playerVtable + 0x6Cu);
        if (gabi::call<BOOL>(callback, player) != 0) {
            actor->mAction = 5;
            actor->mActionMode = 10;
            return true;
        }
        const f32 attackRange = gabi::load<f32>(0x1046E7A4);
        const f32 rangeMargin = gabi::load<f32>(0x1004077C);
        distance = gabi::load<f32>(base + 0x518u);
        if (!(distance < attackRange + rangeMargin)) return false;
        const f32 reloadedMargin = gabi::load<f32>(0x1004077C);
        if (!(distance > attackRange - reloadedMargin)) return false;
    } else {
        const f32 attackRange = gabi::load<f32>(0x1046E7A4);
        const f32 rangeMargin = gabi::load<f32>(0x1004077C);
        if (!(distance < attackRange + rangeMargin)) return false;
        const f32 reloadedMargin = gabi::load<f32>(0x1004077C);
        if (!(distance > attackRange - reloadedMargin)) return false;
    }
    if (gabi::load<s16>(base + 0x4FEu) != 0) return false;
    const s16 delay = gabi::load<s16>(0x1046E7E0);
    const f32 randomRange = gabi::load<f32>(0x100404F4);
    gabi::store<s16>(base + 0x4FEu, delay);
    const f32 random = gabi::call<f32>(0x020198D8, randomRange);
    if (!(random < gabi::load<f32>(0x1046E7E4))) return false;
    const f32 heightTuning = gabi::load<f32>(0x1047BBBC);
    const f32 heightBase = gabi::load<f32>(0x10040698);
    if (!(gabi::load<f32>(base + 0x51Cu) < heightTuning + heightBase)) return false;
    tn_beginCombatAttack(actor);
    return true;
}

// Combat response to an incoming thrown object or the player's attack.
// Native region 024C55A0..024C5838; true selects a defence or jump attack.
static bool tn_combatDefensiveReaction(tn_class* actor, fopAc_ac_c* player, f32 one, f32 blend) {
    const u32 base = gabi::ea(actor);
    if (gabi::load<s16>(base + 0x50Au) != 0) return false;
    const u32 thrown = gabi::call<u32>(0x025DE508, 0x024BBAECu, actor);
    bool incoming = false;
    if (thrown != 0 && gabi::load<f32>(thrown + 0x370u) > blend) {
        gabi::Local<cXyz> delta;
        gabi::call(0x0201ADE0, gabi::at<cXyz>(thrown + 0x314u), delta.get(), &actor->eyePos);
        const f32 squaredDistance = gabi::call<f32>(0x028E8DD0, delta.get());
        const f32 distance = gabi::call<f32>(0x028F4384, squaredDistance);
        const f32 thrownSpeed = gabi::load<f32>(thrown + 0x370u);
        incoming = distance < thrownSpeed * blend;
    }
    const u32 playerAttention = gabi::ea(dComIfGp_get()) + 0x5804u;
    if (daTn_player_way_check(actor) == 0) return false;
    if (!incoming) {
        if (gabi::load<u8>(gabi::ea(player) + 0x3ACu) == 0) return false;
        if (gabi::call<BOOL>(0x024EDFCC, gabi::at<u8>(playerAttention)) == 0 &&
            (gabi::load<u32>(playerAttention + 0x20u) & 0x20000000u) == 0) return false;
        if (gabi::call<u32>(0x024EC8D0, gabi::at<u8>(playerAttention), 0) != base) return false;
    }
    const u32 frameCounter = gabi::load<u32>(base + 0x4F0u);
    if ((frameCounter & 3u) == 0) {
        const s8 pieces = actor->mRemainingEquipmentPieces;
        if ((static_cast<u8>(pieces) & 4u) != 0) {
            const f32 random = gabi::call<f32>(0x020198D8, one);
            const f32 defenceThreshold = gabi::load<f32>(0x10040780);
            if (!(random > defenceThreshold)) {
                actor->mAction = 10;
                actor->mActionMode = 0;
                const u8 playerAttack = gabi::load<u8>(gabi::ea(player) + 0x3ACu);
                gabi::store<s16>(base + 0x4FAu, playerAttack == 10 ? 30 : 15);
                if (base != 0) tn_playBombReactionSound(actor, 0x488B);
                return true;
            }
            if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0) return false;
        } else if ((static_cast<u8>(pieces) & 1u) != 0) {
            return false;
        }
    } else if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0) {
        return false;
    }
    const s32 pathDriven = actor->mPathDriven;
    actor->mAction = 5;
    actor->mActionMode = -20;
    gabi::store<s16>(base + 0x508u, 10);
    anm_init(actor, pathDriven != 0 ? 0x48 : 0x49, gabi::load<f32>(0x10040630), 0, one, pathDriven != 0 ? 0x1B : 0x1C);
    const f32 forwardTuning = gabi::load<f32>(0x1047B61C);
    const f32 forwardBase = gabi::load<f32>(0x10040750);
    const f32 verticalBase = gabi::load<f32>(0x10040754);
    gabi::store<f32>(base + 0x370u, forwardTuning + forwardBase);
    const f32 verticalTuning = gabi::load<f32>(0x1047B620);
    gabi::store<f32>(base + 0x340u, verticalTuning + verticalBase);
    tn_playBombReactionSound(actor, 0x488A);
    return true;
}

// Common combat obstacle/weapon/bomb update, 024C5838..024C58F0.
static void tn_combatObstacles(tn_class* actor) {
    const u32 base = gabi::ea(actor);
    const u32 target = gabi::load<u32>(base + 0xCD8u);
    if (daTn_player_bg_check(actor, gabi::at<cXyz>(target + 0x314u)) != 0) {
        actor->mAction = 0;
        path_check(actor);
        wait_set(actor);
        actor->mActionMode = -10;
        return;
    }
    if (actor->mPathDriven == 0 && search_wepon(actor) != 0) {
        actor->mAction = 12;
        actor->mActionMode = -1;
    }
    if (daTn_checkNearbyBomb(actor) != 0) {
        actor->mAction = 9;
        actor->mActionMode = 0;
    }
    const f32 distanceTuning = gabi::load<f32>(0x1047B98C);
    const f32 distanceBase = gabi::load<f32>(0x10040738);
    const s16 heading = gabi::load<s16>(base + 0x322u);
    const u32 obstruction = gabi::call<u32>(0x024BA6D8, actor, 4, heading, distanceTuning + distanceBase);
    gabi::store<u8>(base + 0x516u, static_cast<u8>(obstruction));
    gabi::store<s16>(base + 0x500u, 50);
}

// Complete action 4 region 024C4F70..024C58F0, assembled from the audited
// locomotion, range, defensive-reaction and obstacle phases above.
static void tn_moveCombat(tn_class* actor, f32 zero, f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    const u32 playerAddress = gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5B2Cu);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(playerAddress);
    const f32 steering = gabi::call<f32>(0x0200796C, 0);
    const s16 frozen = actor->mFrozenAnimation;
    const s16 heading = gabi::load<s16>(base + 0x532u);
    gabi::store<u8>(base + 0xCD4u, 1);
    gabi::store<s16>(base + 0xA94u, heading);
    if (frozen == 0) {
        const s16 mode = actor->mActionMode;
        if (mode != 0) gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), heading, 4, mode == 1 ? 0x800 : 0x400);
    }
    const tn_combat_step phase = tn_combatLocomotion(actor, steering, zero, one, blend, acceleration);
    if (phase == tn_combat_step::beginAttack) {
        tn_beginCombatAttack(actor);
        return;
    }
    if (phase == tn_combat_step::checkRange) {
        if (tn_combatRangeTransition(actor, player)) return;
        if (tn_combatDefensiveReaction(actor, player, one, blend)) return;
    }
    tn_combatObstacles(actor);
}

// Action 5 mode 0: native attack selection 024C5B48..024C5E48.
// The following phase must reload mAttackType, as the native caller does.
static void tn_chooseAttack(tn_class* actor, f32 one, f32 armorBlend) {
    const u32 base = gabi::ea(actor);
    const s32 pathDriven = actor->mPathDriven;
    const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x100404F4));
    if (pathDriven != 0) {
        if (random < gabi::load<f32>(0x1046E7EC)) {
            const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
            anm_init(actor, armor ? 0x2E : 0x46, armorBlend, 0, one, armor ? 0x0B : 0x19);
            yoroi_anm_init(actor, 0x5C, armorBlend, 0, one);
            actor->mAttackType = 0;
        } else {
            const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
            anm_init(actor, armor ? 0x2F : 0x47, armorBlend, 0, one, armor ? 0x0C : 0x1A);
            yoroi_anm_init(actor, 0x5D, armorBlend, 0, one);
            actor->mAttackType = 1;
            if (base != 0) tn_playBombReactionSound(actor, 0x4886);
        }
    } else {
        const f32 threshold = gabi::load<f32>(0x1046E7F0);
        if (random < threshold || (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0) {
            anm_init(actor, 0x45, gabi::load<f32>(0x10040500), 0, one, 0x18);
            actor->mAttackType = 2;
            if (base != 0) tn_playBombReactionSound(actor, 0x488F);
        } else {
            anm_init(actor, 0x43, gabi::load<f32>(0x10040500), 0, one, 0x16);
            actor->mAttackType = 3;
            if (base != 0) tn_playBombReactionSound(actor, 0x4891);
        }
    }
    actor->mWeaponColliderInitialized = 0;
    gabi::store<s16>(base + 0x4FCu, 8);
    actor->mWeaponActivation = 1;
    gabi::store<u8>(base + 0xDB7u, 0);
    actor->mActionMode = 1;
}

// Action 5 mode 1, 024C5E48..024C631C. Returns the native completion flag.
static bool tn_attackAnimation(tn_class* actor, fopAc_ac_c* player, f32 zero,
                               f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    const s32 type = actor->mAttackType;
    u32 body = gabi::load<u32>(base + 0x3E0u);
    bool decay = true;
    if (type == 3) {
        const f32 frame = gabi::load<f32>(body + 0x9Cu);
        if (!(frame < gabi::load<f32>(0x10040784) || frame > gabi::load<f32>(0x100405C4))) {
            const bool jumping = gabi::load<s8>(base + 0xDB7u) != 0;
            gabi::store<f32>(base + 0x370u, jumping
                ? gabi::load<f32>(0x1047B638) + gabi::load<f32>(0x10040754)
                : gabi::load<f32>(0x1047B634) + gabi::load<f32>(0x100405AC));
            decay = false;
        }
    } else if (type == 4) {
        const f32 frame = gabi::load<f32>(body + 0x9Cu);
        if (!(frame < gabi::load<f32>(0x10040684) || frame > gabi::load<f32>(0x10040788))) {
            gabi::store<f32>(base + 0x370u, gabi::load<f32>(0x1047B650) + gabi::load<f32>(0x100405AC));
            body = gabi::load<u32>(base + 0x3E0u);
            if (gabi::ftoi(gabi::load<f32>(body + 0x9Cu)) == 20 && base + 0x37Cu != 0)
                tn_playBombReactionSound(actor, 0x4889);
            decay = false;
        }
    }
    if (decay) cLib_addCalc0(gabi::at<be<f32>>(base + 0x370u), one,
                           gabi::load<f32>(0x1047B638) + gabi::load<f32>(0x10040600));

    f32 start = gabi::load<f32>(0x100406C8);
    f32 end = start;
    u8 attackClass = 0;
    if (actor->mWeaponActivation > 0) {
        s32 activeType = actor->mAttackType;
        body = gabi::load<u32>(base + 0x3E0u);
        if (activeType == 0) {
            const s32 frame = gabi::ftoi(gabi::load<f32>(body + 0x9Cu));
            attackClass = 2;
            start = zero;
            end = gabi::load<f32>(0x10040664);
            if (frame == 20 && base != 0 && base + 0x37Cu != 0) {
                tn_playBombReactionSound(actor, 0x4887);
                activeType = actor->mAttackType;
                body = gabi::load<u32>(base + 0x3E0u);
            }
        } else if (activeType == 1) {
            const u32 frame = static_cast<u32>(gabi::ftoi(gabi::load<f32>(body + 0x9Cu)));
            attackClass = 1;
            start = zero;
            end = gabi::load<f32>(0x10040664);
            if (frame == static_cast<u32>(static_cast<s32>(gabi::load<s16>(0x1047B68C)) + 13)) {
                gabi::store<u8>(base + 0x54Eu, 0);
                const s32 timing = static_cast<s32>(gabi::load<s16>(0x1047B68E)) + gabi::load<s16>(0x1046E794) + 14;
                gabi::store<u16>(base + 0x54Cu, static_cast<u16>(timing));
                const s32 angle = static_cast<s32>(gabi::load<s16>(base + 0x322u)) + gabi::load<s16>(0x1047B698) + 0x2000;
                gabi::store<u16>(base + 0x54Au, static_cast<u16>(angle));
                body = gabi::load<u32>(base + 0x3E0u);
            }
            if (gabi::ftoi(gabi::load<f32>(body + 0x9Cu)) == 20 && base != 0 && base + 0x37Cu != 0)
                tn_playBombReactionSound(actor, 0x4887);
            activeType = actor->mAttackType;
            body = gabi::load<u32>(base + 0x3E0u);
        }
        if (activeType == 0 || activeType == 4) {
            const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
            const s32 particleFrame = armor || activeType == 4 ? 21 : 19;
            if (gabi::ftoi(gabi::load<f32>(body + 0x9Cu)) == particleFrame) {
                const s8 room = actor->current.roomNo;
                const u32 particle = gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5AB0u);
                gabi::call(0x025A847C, gabi::at<void>(particle), 2, 0xA1B3,
                           gabi::at<cXyz>(base + 0x314u), gabi::at<csXyz>(base + 0x328u),
                           nullptr, 0xB9, gabi::at<void>(base + 0x570u), room, nullptr, nullptr, nullptr);
            }
        }
    }
    gabi::store<f32>(base + 0x3B8u, start);
    gabi::store<u8>(base + 0x3C4u, attackClass);
    gabi::store<f32>(base + 0x3BCu, end);
    gabi::store<f32>(base + 0x3B4u, gabi::load<f32>(0x1004064C));
    gabi::store<f32>(base + 0x3B4u, gabi::load<f32>(0x1046E848));
    body = gabi::load<u32>(base + 0x3E0u);
    gabi::store<f32>(base + 0x3C0u, gabi::load<f32>(body + 0x9Cu));
    fopAc_ac_c* hit = wepon_hit_check(actor);
    if (hit != nullptr && gabi::load<s16>(gabi::ea(hit) + 8u) == 0xA8) {
        const u32 playerBase = gabi::ea(player);
        const u32 vtable = gabi::load<u32>(playerBase + 0xB4u);
        if (gabi::call<s32>(gabi::load<u32>(vtable + 0x3Cu), player) != 0) {
            const s32 activeType = actor->mAttackType;
            if (activeType == 0 || activeType == 1) {
                body = gabi::load<u32>(base + 0x3E0u);
                gabi::store<f32>(body + 0x98u, gabi::load<f32>(0x100404E4));
                actor->mWeaponActivation = -1;
                body = gabi::load<u32>(base + 0x3E0u);
                gabi::call(0x025E535C, gabi::at<mDoExt_McaMorf>(body), gabi::at<cXyz>(base + 0x37Cu), 0, 0);
                anm_init(actor, 0x32, gabi::load<f32>(0x1047B620) + acceleration, 0, one, 0x0D);
                yoroi_anm_init(actor, 0x5F, gabi::load<f32>(0x1047B620) + acceleration, 0, one);
            }
        }
    }
    body = gabi::load<u32>(base + 0x3E0u);
    if (gabi::load<f32>(body + 0x9Cu) < blend) {
        const s16 heading = gabi::load<s16>(base + 0x532u);
        gabi::store<s16>(base + 0xA94u, heading);
        gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x322u), heading, 4, 0x400);
        body = gabi::load<u32>(base + 0x3E0u);
    }
    return (gabi::load<u8>(body + 0xA7u) & 1u) != 0 || gabi::load<f32>(body + 0x98u) == zero;
}

// Attack modes 10/12..18 circle the player until the player leaves its stance.
// Native 024C631C..024C6698; steering is retained from the entry query.
static bool tn_attackCircle(tn_class* actor, fopAc_ac_c* player, f32 steering,
                            bool specialStance, s16 mode, f32 zero, f32 one, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    if (mode == 10) {
        actor->mAttackType = 5;
        actor->mActionMode = 12;
        if (base != 0 && base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x4888);
        mode = 12;
    }
    if (mode == 12) {
        if (__builtin_fabsf(steering) > gabi::load<f32>(0x10040670)) {
            anm_init(actor, 0x2D, acceleration, 2, one, 0x0A);
            actor->mActionMode = steering > zero ? 15 : 16;
        } else {
            const f32 attackDistance = gabi::load<f32>(0x1046E7A4);
            const f32 distance = gabi::load<f32>(base + 0x518u);
            if (distance < attackDistance + gabi::load<f32>(0x10040664) &&
                distance > attackDistance - gabi::load<f32>(0x10040664)) {
                anm_init(actor, 0x28, acceleration, 2, one, -1);
                actor->mActionMode = 18;
            } else {
                actor->mActionMode = distance < attackDistance ? 14 : 13;
                anm_init(actor, 0x2C, acceleration, 2, one, 9);
            }
        }
        yoroi_anm_init(actor, 0x58, acceleration, 2, one);
        const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040600));
        gabi::store<u16>(base + 0x4FAu, static_cast<u16>(gabi::ftoi(random + gabi::load<f32>(0x10040600))));
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), gabi::load<f32>(0x1046E7D0), one,
                     gabi::load<f32>(0x10040600));
    } else if (mode == 13 || mode == 14) {
        const f32 speed = gabi::load<f32>(0x1046E7D0);
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), mode == 14 ? -speed : speed, one,
                     gabi::load<f32>(0x10040600));
    } else if (mode == 15 || mode == 16) {
        gabi::store<s16>(base + 0x534u, mode == 15 ? 0x4000 : -0x4000);
        cLib_addCalc2(gabi::at<be<f32>>(base + 0x370u), gabi::load<f32>(0x1046E7D4), one,
                     gabi::load<f32>(0x100405C4));
    } else {
        gabi::store<f32>(base + 0x370u, zero);
    }
    if (gabi::load<s16>(base + 0x4FAu) == 0) actor->mActionMode = 12;
    const u32 playerBase = gabi::ea(player);
    const u32 vtable = gabi::load<u32>(playerBase + 0xB4u);
    bool completed = gabi::call<s32>(gabi::load<u32>(vtable + 0x6Cu), player) == 0;
    if (specialStance) {
        actor->mActionMode = 20;
        completed = false;
        anm_init(actor, 0x29, gabi::load<f32>(0x10040630), 0, one, 6);
        yoroi_anm_init(actor, 0x59, gabi::load<f32>(0x10040630), 0, one);
        if (base != 0 && base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x4889);
    }
    return completed;
}

// Native completed-attack transition 024C6860..024C692C.
static void tn_finishAttack(tn_class* actor, f32 one, f32 blend) {
    const u32 base = gabi::ea(actor);
    if (gabi::load<f32>(base + 0x518u) < gabi::load<f32>(0x1046E7A0)) {
        const s16 limit = get_view_H(actor);
        const u32 target = gabi::load<u32>(base + 0xCD8u);
        const s16 heading = gabi::load<s16>(base + 0x532u);
        if (daTn_player_view_check(actor, gabi::at<cXyz>(target + 0x314u), heading, limit) != 0) {
            const f32 random = gabi::call<f32>(0x020198D8, one);
            if (random < gabi::load<f32>(0x10040504)) {
                actor->mAction = 4;
                actor->mActionMode = 2;
                const f32 delay = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040600));
                gabi::store<u16>(base + 0x4FEu, static_cast<u16>(gabi::ftoi(delay + blend)));
            } else actor->mActionMode = 0;
        } else {
            actor->mAction = 8;
            actor->mActionMode = 0;
        }
    } else {
        actor->mAction = 0;
        path_check(actor);
        wait_set(actor);
        actor->mActionMode = 0;
    }
}

// Complete action 5 inline region 024C58F0..024C692C.
static void tn_moveAttack(tn_class* actor, f32 zero, f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    const u32 playerBase = gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5B2Cu);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(playerBase);
    const f32 steering = gabi::call<f32>(0x0200796C, 0);
    gabi::store<u8>(base + 0xCD4u, 1);
    const u8 playerMode = gabi::load<u8>(playerBase + 0x3ACu);
    const bool specialStance = playerMode == 8 || playerMode == 9;
    const s16 mode = actor->mActionMode;
    bool completed = false;
    switch (mode) {
    case -20:
        if (tn_demoAnimationStopped(actor->mpBodyMorf, zero)) {
            const f32 random = gabi::call<f32>(0x020198D8, one);
            if (random < gabi::load<f32>(0x10040504)) {
                if (actor->mPathDriven != 0) {
                    anm_init(actor, 0x3F, gabi::load<f32>(0x10040500), 0, one, 0x12);
                    actor->mAttackType = 4;
                    if (base != 0 && base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x488A);
                } else {
                    anm_init(actor, 0x43, gabi::load<f32>(0x10040500), 0, one, 0x16);
                    actor->mAttackType = 3;
                    gabi::store<u8>(base + 0xDB7u, 1);
                }
                actor->mWeaponColliderInitialized = 0;
                const u32 contact = gabi::load<u32>(base + 0xB08u);
                gabi::store<s16>(base + 0x4FCu, 8);
                actor->mActionMode = 1;
                actor->mWeaponActivation = 1;
                if ((contact & 0x20u) != 0) gabi::store<f32>(base + 0x370u, zero);
                break;
            }
            completed = true;
        }
        if ((gabi::load<u32>(base + 0xB08u) & 0x20u) != 0) gabi::store<f32>(base + 0x370u, zero);
        break;
    case 0:
        tn_chooseAttack(actor, one, acceleration);
        [[fallthrough]];
    case 1:
        completed = tn_attackAnimation(actor, player, zero, one, blend, acceleration);
        break;
    case 10: case 12: case 13: case 14: case 15: case 16: case 18:
        completed = tn_attackCircle(actor, player, steering, specialStance, mode, zero, one, acceleration);
        break;
    case 20:
        if (!tn_demoAnimationStopped(actor->mpBodyMorf, zero)) break;
        actor->mActionMode = 21;
        anm_init(actor, 0x2A, one, 2, one, 7);
        yoroi_anm_init(actor, 0x5A, one, 2, one);
        gabi::store<s16>(base + 0x1644u, 1);
        [[fallthrough]];
    case 21: {
        gabi::store<f32>(base + 0x370u, zero);
        const u8 stance = gabi::load<u8>(playerBase + 0x3ACu);
        gabi::store<s16>(base + 0x1646u, stance == 9 ? 7 : 3);
        wepon_hit_check(actor);
        if (!specialStance) {
            actor->mActionMode = 22;
            anm_init(actor, 0x2B, one, 0, one, 8);
            yoroi_anm_init(actor, 0x5B, one, 0, one);
            if (base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x4889);
        }
        const u32 mantleId = actor->mMantPcId;
        if (mantleId != 0xFFFFu) {
            gabi::Local<be<u32>> id;
            *id = mantleId;
            const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : gabi::call<u32>(0x025D5218, 0x025E1234u, id.get());
            if (mantle != 0) {
                cLib_addCalc2(gabi::at<be<f32>>(mantle + 0x3644u), gabi::load<f32>(0x1047B628) + gabi::load<f32>(0x100405AC),
                             one, gabi::load<f32>(0x10040600));
                cLib_addCalc2(gabi::at<be<f32>>(mantle + 0x3648u), gabi::load<f32>(0x1047B62C) + gabi::load<f32>(0x1004073C),
                             one, gabi::load<f32>(0x1004066C));
            }
        }
        break;
    }
    case 22:
        completed = tn_demoAnimationStopped(actor->mpBodyMorf, zero);
        break;
    default:
        break;
    }
    gabi::store<s16>(base + 0x500u, 50);
    if (completed) tn_finishAttack(actor, one, blend);
}

// Movement owns the action dispatch and retains its constants across all calls.
void Tn_move(tn_class* actor) {
    WWHD_FUNC(0x024C3E64, void, actor);
    const u32 base = gabi::ea(actor);
    const u32 playerBase = gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5B2Cu);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(playerBase);
    gabi::store<s16>(base + 0x534u, 0);
    gabi::store<u8>(base + 0xCD4u, 0);
    gabi::call(0x0200F428, gabi::at<be<s16>>(base + 0x1554u), 0, 2, 0x800);
    const f32 zero = gabi::load<f32>(0x100404E0);
    if (tn_processAirborneAttack(actor, zero)) return;
    const f32 blend = gabi::load<f32>(0x10040510);
    if (tn_prepareNormalMovement(actor, player)) {
        const s16 action = actor->mAction;
        const f32 one = gabi::load<f32>(0x100404FC);
        const f32 acceleration = gabi::load<f32>(0x100405D0);
        switch (action) {
        case 0: tn_movePatrol(actor, zero, one, blend, acceleration); break;
        case 1: tn_moveReturnHome(actor, zero, one, blend, acceleration); break;
        case 4: tn_moveCombat(actor, zero, one, blend, acceleration); break;
        case 5: tn_moveAttack(actor, zero, one, blend, acceleration); break;
        default: tn_dispatchSeparateMovementAction(actor, action); break;
        }
    }
    tn_finishMovement(actor, static_cast<s16>(actor->mAction), zero, blend);
}
VERIFY(0x024C3E64, Tn_move);

// Complete source-only Darknut Execute proposal; no compatibility claim.
// Sole owner integrates after original-HD and ABI review.
#include <initializer_list>

namespace tn_execute_proposal {
enum class PrefixResult { Finished, ContinueAt024BD870 };
struct PrefixState { u32 playerAddress; };

// Snapshot all twelve source scalars before writing the model matrix, matching
// the native inline copies. This is not an extra guest PSMTXCopy call.
static void copyBaseMatrix(u32 source, u32 destination) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = gabi::load<f32>(source + i * 4);
    for (u32 i = 0; i < 12; ++i) gabi::store<f32>(destination + i * 4, values[i]);
}
static u32 findActor(u32 id) {
    gabi::Local<be<u32>> searchId;
    gabi::Local<u8[16]> outgoingLinkage;
    *searchId = id;
    return id == 0xFFFFFFFFu ? 0
        : gabi::call<u32>(0x025D5218, 0x025E1234u, searchId.get());
}
static void playBody(tn_class* actor) {
    gabi::call(0x025E535C, actor->mpBodyMorf.get(), &actor->eyePos, 0, 0);
}
static void playArmor(tn_class* actor) {
    gabi::call(0x025E535C, actor->mpArmorMorf.get(), nullptr, 0, 0);
}
static void playShield(tn_class* actor) {
    gabi::call(0x025E535C, actor->mpShieldMorf.get(), nullptr, 0, 0);
}
static void moveDetachedParts(tn_class* actor) {
    for (int index = 0; index < 3; ++index) {
        if (actor->mParts[index].mState >= 0)
            gabi::call(0x024BC3EC, actor, index);
    }
}
static void decrementShort(u32 address) {
    s16 timer = gabi::load<s16>(address);
    if (timer != 0) gabi::store<s16>(address, static_cast<s16>(timer - 1));
}

// Exact owned prefix begins after prologue at024BD1DC. Early exits return the
// original TRUE result. Continue enters the regular damage/movement phases
// assembled by the full wrapper below. This helper alone is not Execute.
PrefixResult executePrefix(tn_class* actor, PrefixState& state) {
    const u32 base = gabi::ea(actor);
    auto play = dComIfGp_get();
    const u8 deletionSwitch = actor->mDisableSpawnOnDeathSwitch;
    state.playerAddress = gabi::load<u32>(gabi::ea(play) + 0x5B2C);

    // HD-only-in-this-comparison prelude,024BD1E0..024BD284. Do not transplant
    // the GC unconditional4000-unit fall rule into this switch-gated branch.
    if (deletionSwitch != 0) {
        f32 floor = gabi::fsubs_ppc(actor->home.pos.y, gabi::load<f32>(0x10040640));
        if (static_cast<f32>(actor->current.pos.y) < floor) {
            u32 save = gabi::load<u32>(0x101F84DC);
            gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), deletionSwitch,
                       static_cast<s8>(actor->current.roomNo));
            save = gabi::load<u32>(0x101F84DC);
            gabi::call(0x025BA5D4, gabi::at<void>(save + 0x20),
                       static_cast<u16>(actor->setID), static_cast<s8>(actor->home.roomNo));
            gabi::call(0x025D57E0, actor);
            if (actor->mPathDriven != 0) {
                const u32 weapon = findActor(actor->mWeaponActorId);
                if (weapon != 0) gabi::call(0x025D57E0, gabi::at<void>(weapon));
            }
            return PrefixResult::Finished;
        }
    }

    // Signed byte wrap is explicit;1670 has no accepted gameplay name yet.
    const s8 statusTimer = gabi::load<s8>(base + 0x1670);
    if (statusTimer != 0) {
        const s8 next = static_cast<s8>(static_cast<u8>(statusTimer) - 1);
        gabi::store<s8>(base + 0x1670, next);
        if (next == 0) actor->actor_status = static_cast<u32>(actor->actor_status) & ~0x4000u;
    }
    const BOOL frozen = gabi::call<BOOL>(0x020402C8, &actor->mEnemyIce);
    const f32 one = gabi::load<f32>(0x100404FC);
    if (frozen != 0) {
        // Ice+6 is a signed light-shrink/state byte. Its meaning is provisional.
        if (gabi::load<s8>(base + 0x167A) == 0) {
            if (actor->mEnemyIce.mFreezeTimer == 23) {
                gabi::store<f32>(gabi::ea(actor->mpBodyMorf.get()) + 0x98, one);
                gabi::store<f32>(gabi::ea(actor->mpShieldMorf.get()) + 0x98, one);
                gabi::store<f32>(gabi::ea(actor->mpArmorMorf.get()) + 0x98, one);
                // Normal live actor/eye pointee domain; original contains the
                // redundant receiver/address guards before this sound call.
                const u32 id = gabi::load<u32>(base + 4);  // read before the reverb lookup
                const s32 reverb = gabi::call<s32>(0x02520540, static_cast<s8>(actor->current.roomNo));
                gabi::call(0x025E1AA4, 0x488A, &actor->eyePos, id, 0, reverb);
            }
            playBody(actor); playArmor(actor); playShield(actor);
        }
        const u32 model = gabi::load<u32>(gabi::ea(actor->mpBodyMorf.get()) + 0x90);
        copyBaseMatrix(0x1048D0CC, model + 0xC8);
        gabi::call(0x025E55A0, actor->mpBodyMorf.get());
        gabi::call(0x025E55A0, actor->mpShieldMorf.get());
        moveDetachedParts(actor);
        gabi::store<u8>(base + 0x3F5, 0);
        if (actor->mEnemyIce.mFreezeTimer == 1) {
            gabi::call(0x024BB800, actor);
            gabi::call(0x024BB620, actor);
            gabi::call(0x024B931C, actor, 0x3C, gabi::load<f32>(0x10040510), 2, one);
        }
        return PrefixResult::Finished;
    }

    // HD runs this block unconditionally; no GC HIO/pad pause gate is inserted.
    gabi::store<u32>(base + 0x4F0, gabi::load<u32>(base + 0x4F0) + 1);
    for (u32 offset : {0x4F8u,0x4FAu,0x4FCu,0x4FEu,0x500u}) decrementShort(base + offset);
    for (u32 offset : {0x502u,0x514u,0x1552u,0xA4Eu,0x508u,0x50Au}) decrementShort(base + offset);
    if (gabi::load<s8>(base + 0x155C) != 0) {
        const u32 weapon = findActor(actor->mWeaponActorId);
        if (weapon != 0) {
            gabi::store<u8>(base + 0x155C, 0);
            gabi::call(0x025D9D0C, gabi::at<void>(weapon), 0);
        }
    }
    const f32 zero = gabi::load<f32>(0x100404E0);
    bool attentionSuppressed;
    if (actor->m02C1 != 0) {
        bool released = gabi::load<s16>(0x1047B688) != 0;
        if (!released) {
            const u32 save = gabi::load<u32>(0x101F84DC);
            released = gabi::call<BOOL>(0x025BA0C0, gabi::at<void>(save + 0x20),
                static_cast<u8>(actor->m02C1) - 1, static_cast<s8>(actor->current.roomNo)) != 0;
        }
        if (!released) {
            gabi::store<u32>(base + 0x39C, 0);
            const u32 weapon = findActor(actor->mWeaponActorId);
            if (weapon != 0) {
                const f32 hidden = gabi::load<f32>(0x10040644);
                gabi::call(0x0200FAD8, hidden, hidden, zero, 0);
                const u32 model = gabi::load<u32>(weapon + 0x3B4);
                if (model != 0) copyBaseMatrix(gabi::load<u32>(0x1018C7B0), model + 0xC8);
            }
            if (actor->mMantPcId != 0xFFFFu) {
                const u32 mantle = findActor(actor->mMantPcId);
                if (mantle != 0) {
                    const f32 x = gabi::load<f32>(0x10040644);
                    const f32 y = gabi::load<f32>(0x100404F8);
                    const f32 z = gabi::load<f32>(0x10040648);
                    for (u32 offset : {0x362Cu,0x3638u}) {
                        gabi::store<f32>(mantle + offset, x);
                        gabi::store<f32>(mantle + offset + 4, y);
                        gabi::store<f32>(mantle + offset + 8, z);
                    }
                }
            }
            return PrefixResult::Finished;
        }
        actor->m02C1 = 0;
        attentionSuppressed = gabi::load<s8>(base + 0x1648) != 0;
    } else attentionSuppressed = gabi::load<s8>(base + 0x1648) != 0;
    if (!attentionSuppressed) {
        gabi::store<u32>(base + 0x39C, 4);
        actor->actor_status = static_cast<u32>(actor->actor_status) | 0x20;
    } else gabi::store<u32>(base + 0x39C, 0);
    if (gabi::load<s16>(base + 0x506) == 0) {
        playBody(actor); playArmor(actor);
        if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 4) != 0) playShield(actor);
    }
    return PrefixResult::ContinueAt024BD870;
}

// 024BE8DC..024BECD8: called only after the normal movement phase.
// The camera object is obtained even when the demo state is zero.
static void updateDemoCameraAndSmoke(tn_class* actor) {
    const u32 a = gabi::ea(actor);
    const u32 debug = 0x1047B608;
    const u32 hio = 0x1046E778;
    gabi::Local<cXyz> cameraCenter;
    gabi::Local<cXyz> cameraEye;
    gabi::Local<cXyz> smokeOffset;
    gabi::Local<u8[16]> outgoingLinkage;
    const u32 play = gabi::call<u32>(0x025200D4);
    const u32 cameraOwner = gabi::call<u32>(0x024F8020, gabi::at<void>(play));
    const u32 camera = gabi::call<u32>(0x024F8044, gabi::at<void>(cameraOwner));
    s8 demo = gabi::load<s8>(a + 0x1648);
    bool setCamera = demo != 0;
    if (demo == 50) {
        if (gabi::load<u16>(a + 0xF8) != 2) {
            gabi::call(0x025D7B24, actor, 2, 0xFFFF, 0);
            gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 2);
            setCamera = false;
        } else {
            gabi::call(0x02514F2C, gabi::at<void>(camera));
            gabi::call(0x02515280, gabi::at<void>(camera), 1);
            gabi::store<s8>(a + 0x1648, 51);
            const u32 currentPlay = gabi::call<u32>(0x025200D4);
            const u32 view = gabi::load<u32>(currentPlay + 0x5AF8);
            for (u32 i = 0; i < 3; ++i)
                gabi::store<u32>(a + 0x164C + 4*i, gabi::load<u32>(view + 0xDC + 4*i));
            for (u32 i = 0; i < 3; ++i)
                gabi::store<u32>(a + 0x1658 + 4*i, gabi::load<u32>(view + 0xE8 + 4*i));
            gabi::store<f32>(a + 0x1664, gabi::load<f32>(0x10040668));
            gabi::store<s16>(a + 0x164A, 0);
            demo = 51;
        }
    }
    if (demo == 51) {
        gabi::call(0x0200ED84, gabi::at<be<f32>>(a + 0x1664),
            gabi::fadds_ppc(gabi::load<f32>(debug + 0x3C), gabi::load<f32>(0x100405C4)),
            gabi::load<f32>(0x1004066C),
            gabi::fadds_ppc(gabi::load<f32>(debug + 0x40), gabi::load<f32>(0x100405EC)));
        if (gabi::load<s8>(a + 0x1668) == 0) {
            gabi::call(0x0200ED84, gabi::at<be<f32>>(a + 0x1658),
                gabi::load<f32>(a + 0x314), gabi::load<f32>(0x10040670), gabi::load<f32>(0x100404F4));
            const f32 targetY = gabi::fadds_ppc(
                gabi::fadds_ppc(gabi::load<f32>(a + 0x318), gabi::load<f32>(0x10040674)),
                gabi::load<f32>(debug + 0x38));
            gabi::call(0x0200ED84, gabi::at<be<f32>>(a + 0x165C), targetY,
                gabi::load<f32>(0x10040670), gabi::load<f32>(0x100404F4));
            gabi::call(0x0200ED84, gabi::at<be<f32>>(a + 0x1660),
                gabi::load<f32>(a + 0x31C), gabi::load<f32>(0x10040670), gabi::load<f32>(0x100404F4));
        }
        const s16 elapsed = static_cast<s16>(gabi::load<s16>(a + 0x164A) + 1);
        gabi::store<s16>(a + 0x164A, elapsed);
        if (elapsed > 150) {
            gabi::call(0x02515280, gabi::at<void>(camera), 0);
            gabi::call(0x02514F38, gabi::at<void>(camera));
            gabi::call(0x0259169C);
            const u32 currentPlay = gabi::call<u32>(0x025200D4);
            gabi::store<u16>(currentPlay + 0x52B8, gabi::load<u16>(currentPlay + 0x52B8) | 8);
            gabi::call(0x025D57E0, actor);
            setCamera = false;
        }
    }
    if (setCamera) {
        *cameraCenter = *gabi::at<cXyz>(a + 0x1658);
        *cameraEye = *gabi::at<cXyz>(a + 0x164C);
        gabi::call(0x02514F88, gabi::at<void>(camera), cameraCenter.get(), cameraEye.get(),
            0, gabi::load<f32>(a + 0x1664));
    }
    if (gabi::load<s16>(a + 0x54C) != 0) {
        const s16 remaining = static_cast<s16>(gabi::load<s16>(a + 0x54C) - 1);
        gabi::store<s16>(a + 0x54C, remaining);
        if (remaining >= gabi::load<s16>(hio + 0x1C)) {
            smokeOffset->x = gabi::load<f32>(0x100404E0);
            smokeOffset->y = gabi::load<f32>(0x100404E0);
            gabi::store<s16>(a + 0x544, 0); gabi::store<s16>(a + 0x548, 0);
            gabi::call(0x0200FAD8, 0, gabi::load<f32>(a + 0x314),
                gabi::fadds_ppc(gabi::load<f32>(a + 0x318), gabi::load<f32>(0x10040678)),
                gabi::load<f32>(a + 0x31C));
            const u8 mode = gabi::load<u8>(a + 0x54E);
            if (mode == 0 || mode == 1) {
                if (mode == 1)
                    gabi::call(0x025F1C28, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(a + 0x322));
                smokeOffset->z = mode == 0
                    ? gabi::fadds_ppc(gabi::load<f32>(debug + 0x18), gabi::load<f32>(0x1004067C))
                    : gabi::load<f32>(0x10040680);
                gabi::call(0x025F1C28, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(a + 0x54A));
                gabi::call(0x0200FCD8, smokeOffset.get(), gabi::at<cXyz>(a + 0x538));
                gabi::store<s16>(a + 0x546, gabi::load<s16>(a + 0x54A));
                gabi::call(0x024B94C0, actor, gabi::fadds_ppc(
                    gabi::load<f32>(debug + (mode == 0 ? 0x44 : 0x58)),
                    gabi::load<f32>(mode == 0 ? 0x10040500 : 0x10040684)));
                const int increment = mode == 0 ? gabi::load<s16>(debug + 0x8E) + 2000 : 0x1FA0;
                gabi::store<s16>(a + 0x54A, static_cast<s16>(gabi::load<s16>(a + 0x54A) + increment));
            }
        } else gabi::store<f32>(a + 0x53C,
            gabi::fadds_ppc(gabi::load<f32>(a + 0xCA8), gabi::load<f32>(0x10040688)));
        if (gabi::load<s16>(a + 0x54C) == 0) {
            gabi::call(0x025A5F88, gabi::at<void>(a + 0x550));
            gabi::store<u8>(a + 0x54F, 0);
        }
    }
    decrementShort(a + 0x506);
    gabi::store<u32>(a + 0x590, a);
    gabi::store<s16>(a + 0x598, 3);
}

// 024BF308..024BF610: register collision shapes after animation and fire.
// Keep the initial player snapshot: native r24 is not the refreshed damage player.
static void updateCollisionShapes(tn_class* actor, u32 initialPlayer) {
    const u32 a = gabi::ea(actor);
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> coCenter;
    gabi::Local<cXyz> headCenter;
    gabi::Local<cXyz> bodyCenter;
    gabi::Local<u8[16]> outgoingLinkage;
    gabi::call(0x0200FAD8, 0, gabi::load<f32>(a + 0x314),
        gabi::load<f32>(a + 0x318), gabi::load<f32>(a + 0x31C));
    gabi::call(0x025F1C28, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(a + 0x322));
    offset->x = gabi::load<f32>(0x100404E0);
    offset->y = gabi::load<f32>(0x100404E0);
    offset->z = gabi::load<s16>(a + 0x596) == 18
        ? gabi::fadds_ppc(gabi::load<f32>(0x1047BBC0), gabi::load<f32>(0x10040608))
        : gabi::load<f32>(0x10040610);
    gabi::call(0x0200FCD8, offset.get(), coCenter.get());
    gabi::call(0x020182E0, gabi::at<void>(a + 0xEDC), coCenter.get());
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240, gabi::at<void>(play + 0x26A4), gabi::at<void>(a + 0xDC4));
    play = gabi::call<u32>(0x025200D4);
    gabi::call(0x02516C14, gabi::at<void>(play + 0x4EF8), gabi::at<void>(a + 0xDC4), 3);
    *headCenter = *gabi::at<cXyz>(a + 0x14D4);
    *bodyCenter = actor->current.pos;
    if (gabi::load<s16>(a + 0x508) != 0) {
        headCenter->y = gabi::fsubs_ppc(headCenter->y, gabi::load<f32>(0x100404F8));
        bodyCenter->y = gabi::fsubs_ppc(bodyCenter->y, gabi::load<f32>(0x100404F8));
    }
    if (gabi::load<s16>(a + 0x596) == 10) {
        bodyCenter->y = gabi::fsubs_ppc(bodyCenter->y, gabi::load<f32>(0x100404F4));
        headCenter->y = gabi::fsubs_ppc(headCenter->y, gabi::load<f32>(0x100404F8));
    }
    gabi::call(0x020182E0, gabi::at<void>(a + 0x100C), bodyCenter.get());
    u32 bodyShield = gabi::load<u32>(a + 0xF88);
    if ((gabi::load<u8>(a + 0x3EC) & 1) != 0) bodyShield |= 1;
    else bodyShield &= ~1u;
    gabi::store<u32>(a + 0xF88, bodyShield);
    play = gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240, gabi::at<void>(play + 0x26A4), gabi::at<void>(a + 0xEF4));
    gabi::call(0x02018D40, gabi::at<void>(a + 0x113C), headCenter.get());
    if ((gabi::load<u8>(a + 0x3EC) & 2) != 0) {
        if (gabi::load<u8>(initialPlayer + 0x3AC) == 0x1F) {
            gabi::store<u32>(a + 0x103C, gabi::load<u32>(a + 0x103C) & ~1u);
        } else {
            gabi::store<u32>(a + 0x103C, gabi::load<u32>(a + 0x103C) | 1u);
            gabi::store<u8>(a + 0x10D6, 12);
            gabi::store<u32>(a + 0x10B8, gabi::load<u32>(a + 0x10B8) | 1u);
        }
    } else {
        gabi::store<u8>(a + 0x10D6, 1);
        gabi::store<u32>(a + 0x10B8, gabi::load<u32>(a + 0x10B8) & ~1u);
    }
    play = gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240, gabi::at<void>(play + 0x26A4), gabi::at<void>(a + 0x1024));
    if (gabi::load<s16>(a + 0x536) != 0) {
        const s16 remaining = static_cast<s16>(gabi::load<s16>(a + 0x536) - 1);
        gabi::store<s16>(a + 0x536, remaining);
        if (remaining == 0) {
            gabi::store<s16>(a + 0x546, gabi::load<s16>(a + 0x322));
            play = gabi::call<u32>(0x025200D4);
            gabi::call(0x025A847C, gabi::at<void>(gabi::load<u32>(play + 0x5AB0)),
                0, 0x0E, gabi::at<cXyz>(a + 0x14D4), gabi::at<csXyz>(a + 0x544),
                nullptr, 0xFF, nullptr, -1, nullptr, nullptr, 0);
        }
    }
}

// 024BF610..024BF800: sample the ground in front/right of the actor.
static void updateGroundTilt(tn_class* actor) {
    const u32 a = gabi::ea(actor);
    if ((gabi::load<u32>(a + 0xB08) & 0x20) == 0) return;
    s16 pitch = 0x7FFF, roll = 0x7FFF;
    if (gabi::load<s16>(a + 0xA44) == 0) pitch = roll = 0;
    else {
        gabi::Local<tn_groundcheck> check;
        gabi::Local<u8[16]> outgoingLinkage;
        gabi::call(0x02008E0C, check.get());
        for (int i = 0; i < 7; ++i) check->mPassFlags[i] = 0;
        check->mCheckVtable = 0x100403D0;
        gabi::store<u32>(check.a, check.a + 0x40);
        gabi::store<u32>(check.a + 4, check.a + 0x4C);
        check->mGroundVtable = 0x100403E0;
        check->mPolyPassVtable = 0x10040400;
        check->mGroupPassVtable = 0x100403F0;
        check->mGroupMask = 1;
        const f32 x = gabi::load<f32>(a + 0x314);
        const f32 z = gabi::load<f32>(a + 0x31C);
        const f32 step = gabi::load<f32>(0x100404F4);
        const f32 sampleOffset = gabi::load<f32>(0x100405AC);
        const f32 invalidHeight = gabi::load<f32>(0x100405B8);
        check->mPosition.x = x;
        check->mPosition.y = gabi::fadds_ppc(gabi::load<f32>(a + 0x318),
            gabi::fsubs_ppc(sampleOffset, gabi::load<f32>(a + 0xA14)));
        check->mPosition.z = z;
        u32 play = gabi::call<u32>(0x025200D4);
        const f32 groundY = gabi::call<f32>(0x02008974, gabi::at<void>(play + 0x12A0), check.get());
        if (groundY != invalidHeight) {
            const f32 sampleY = gabi::fadds_ppc(groundY, sampleOffset);
            const f32 forwardZ = gabi::fadds_ppc(z, step);
            check->mPosition.x = x; check->mPosition.y = sampleY; check->mPosition.z = forwardZ;
            play = gabi::call<u32>(0x025200D4);
            const f32 forwardY = gabi::call<f32>(0x02008974, gabi::at<void>(play + 0x12A0), check.get());
            if (forwardY != invalidHeight) {
                pitch = static_cast<s16>(-gabi::call<s32>(0x020195B0,
                    gabi::fsubs_ppc(forwardY, groundY), gabi::fsubs_ppc(forwardZ, z)));
                if (static_cast<u32>(static_cast<s32>(pitch) + 0x2000) >= 0x4001) pitch = 0;
            }
            const f32 rightX = gabi::fadds_ppc(x, step);
            check->mPosition.x = rightX; check->mPosition.y = sampleY; check->mPosition.z = z;
            play = gabi::call<u32>(0x025200D4);
            const f32 rightY = gabi::call<f32>(0x02008974, gabi::at<void>(play + 0x12A0), check.get());
            if (rightY != invalidHeight) {
                const s32 angle = gabi::call<s32>(0x020195B0,
                    gabi::fsubs_ppc(rightY, groundY), gabi::fsubs_ppc(rightX, x));
                roll = static_cast<s16>(angle);
                if (static_cast<u32>(angle + 0x2000) >= 0x4001) roll = 0;
            }
        }
        check->mGroundVtable = 0x100403E0;
        check->mPolyPassVtable = 0x10040400;
        check->mGroupPassVtable = 0x100403C0;
        gabi::call(0x02008DAC, check.get(), 0);
    }
    if (pitch != 0x7FFF) gabi::call(0x0200F428, gabi::at<be<s16>>(a + 0xA50), pitch, 1, 0x400);
    if (roll != 0x7FFF) gabi::call(0x0200F428, gabi::at<be<s16>>(a + 0xA54), roll, 1, 0x400);
}

// Mark joint matrices current, then perform the original guest matrix copy.
static void copyBodyJointToCalc(tn_class* actor, u32 matrixOffset) {
    const u32 morf = gabi::ea(actor->mpBodyMorf.get());
    const u32 model = gabi::load<u32>(morf + 0x90);
    const u32 matrixBlock = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(matrixBlock + 4, gabi::load<u16>(matrixBlock + 4) | 0x10);
    const u32 matrices = gabi::load<u32>(matrixBlock + 0x10);
    gabi::call(0x028E90D4, gabi::at<void>(matrices + matrixOffset),
        gabi::at<void>(gabi::load<u32>(0x1018C7B0)));
}
// 024BF800..024BFBF4: held weapon attachment and ordinary release.
static void updateHeldWeapon(tn_class* actor) {
    const u32 a = gabi::ea(actor), debug = 0x1047B608;
    if (gabi::load<u32>(a + 0xD84) == 0) return;
    gabi::Local<tn_linecheck> check;
    gabi::Local<cXyz> origin;
    gabi::Local<u8[16]> outgoingLinkage;
    u32 weapon = findActor(gabi::load<u32>(a + 0x1560));
    if (weapon != 0) {
        if ((gabi::load<u32>(weapon + 0x2E0) & 0x2000) != 0) {
            copyBodyJointToCalc(actor, 0x450);
            gabi::call(0x025F1C28, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(debug + 0x502));
            gabi::call(0x025F1BF4, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(debug + 0x504));
            gabi::call(0x025F1C5C, gabi::at<void>(gabi::load<u32>(0x1018C7B0)),
                static_cast<s16>(gabi::load<s16>(debug + 0x506) + 0x8000));
            gabi::call(0x0200FAD8, 1, gabi::load<f32>(debug + 0x4AC),
                gabi::load<f32>(debug + 0x4B0),
                gabi::fadds_ppc(gabi::load<f32>(debug + 0x4B4), gabi::load<f32>(0x1004068C)));
            const u32 weaponModel = gabi::load<u32>(weapon + 0x3B4);
            if (weaponModel != 0) copyBaseMatrix(gabi::load<u32>(0x1018C7B0), weaponModel + 0xC8);
        } else gabi::call(0x025D9D0C, gabi::at<void>(weapon), 0);
    } else if (gabi::load<s8>(a + 0x155C) == 0) gabi::store<u32>(a + 0xD84, 0);
    if (gabi::load<u32>(a + 0xD88) == 0) return;
    weapon = findActor(gabi::load<u32>(a + 0x1560));
    if (weapon != 0) {
        gabi::call(0x025D9D24, gabi::at<void>(weapon));
        if (gabi::load<u32>(a + 0xD88) != 2) {
            const f32 spin = gabi::call<f32>(0x02019918, gabi::load<f32>(0x10040690));
            gabi::store<s16>(weapon + 0x43A, static_cast<s16>(gabi::ftoi(spin)));
            const s32 baseYaw = gabi::load<s16>(a + 0x32A) + 0x8000;
            const f32 spread = gabi::call<f32>(0x02019918, gabi::load<f32>(0x10040694));
            const s32 yaw = baseYaw + static_cast<s16>(gabi::ftoi(spread));
            const f32 horizontal = gabi::fadds_ppc(
                gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040510)), gabi::load<f32>(0x10040600));
            const f32 vertical = gabi::fadds_ppc(
                gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040510)), gabi::load<f32>(0x10040600));
            gabi::store<s16>(weapon + 0x322, static_cast<s16>(yaw));
            gabi::store<f32>(weapon + 0x340, vertical);
            gabi::store<f32>(weapon + 0x370, horizontal);
        }
        gabi::call(0x02008FEC, check.get());
        for (int i = 0; i < 7; ++i) check->mPassFlags[i] = 0;
        check->mCheckVtable = 0x10040440; check->mLineVtable = 0x10040450;
        check->mPolyPassVtable = 0x10040470; check->mGroupPassVtable = 0x10040460;
        check->mGroupMask = 1;
        gabi::store<u32>(check.a, check.a + 0x58);
        gabi::store<u32>(check.a + 4, check.a + 0x64);
        gabi::call(0x024F1AFC, check.get(), &actor->eyePos, gabi::at<cXyz>(weapon + 0x314), actor);
        const u32 play = gabi::call<u32>(0x025200D4);
        if (gabi::call<int>(0x02008860, gabi::at<void>(play + 0x12A0), check.get()) != 0) {
            copyBodyJointToCalc(actor, 0x1E0);
            const u32 model = gabi::load<u32>(weapon + 0x3B4);
            if (model != 0) copyBaseMatrix(gabi::load<u32>(0x1018C7B0), model + 0xC8);
            origin->x = origin->y = origin->z = gabi::load<f32>(0x100404E0);
            gabi::call(0x0200FCD8, origin.get(), gabi::at<cXyz>(weapon + 0x314));
        }
        const s8 room = gabi::load<s8>(a + 0x326);
        const u32 actorId = gabi::load<u32>(a + 4);
        const int reverb = gabi::call<int>(0x02520540, room);
        gabi::call(0x025E1AA4, 0x488C, &actor->eyePos, actorId, 0, reverb);
        check->mPolyPassVtable = 0x10040470; check->mGroupPassVtable = 0x100403C0;
        check->mLineVtable = 0x100403B0;
        gabi::call(0x02008B4C, check.get(), 0);
    }
    gabi::store<u32>(a + 0xD88, 0); gabi::store<u32>(a + 0xD84, 0);
}

// 024BFBF4..024C0138: detached parts, spin samples/particles and lighting.
static void finishNormalEffects(tn_class* actor) {
    const u32 a = gabi::ea(actor), debug = 0x1047B608;
    moveDetachedParts(actor);
    gabi::store<u8>(a + 0x3F5, 0);
    if (gabi::load<s16>(a + 0x1644) != 0) {
        gabi::Local<cXyz> tipOffset;
        gabi::Local<cXyz> worldTip;
        gabi::Local<cXyz> relativeTip;
        gabi::Local<u8[16]> outgoingLinkage;
        tipOffset->x = tipOffset->y = gabi::load<f32>(0x100404E0);
        tipOffset->z = gabi::fadds_ppc(gabi::load<f32>(debug + 0x18), gabi::load<f32>(0x10040698));
        copyBodyJointToCalc(actor, 0x450);
        gabi::call(0x0200FCD8, tipOffset.get(), worldTip.get());
        gabi::call(0x0201ADE0, worldTip.get(), relativeTip.get(), &actor->current.pos);
        *tipOffset = *relativeTip;
        for (int i = 0; i < 10; ++i) {
            const s16 yaw = static_cast<s16>(i * (gabi::load<s16>(debug + 0x84) - 0x320));
            gabi::call(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), yaw);
            cXyz* sample = gabi::at<cXyz>(a + 0x15CC + 12*i);
            gabi::call(0x0200FCD8, tipOffset.get(), sample);
            gabi::call(0x028E8D88, sample, &actor->current.pos, sample);
        }
        // Six one-time color pairs. Preserve the unusual 2454/2456 and
        // 245C/245E channels rather than treating these as packed u16 colors.
        struct ColorInit { u32 flag, first, second; int firstValue, secondValue; };
        const ColorInit colors[] = {
            {0x1046E724,0x101D2448,0x101D2449,0x40,0x40},
            {0x1046E728,0x101D244C,0x101D244D,0xDC,0xDC},
            {0x1046E72C,0x101D2450,0x101D2451,0x78,0x78},
            {0x1046E730,0x101D2454,0x101D2456,0xC8,0x40},
            {0x1046E734,0x101D245A,0,0xDC,0},
            {0x1046E738,0x101D245C,0x101D245E,0xC8,0x78},
        };
        for (const ColorInit& color : colors) {
            if (gabi::load<u32>(color.flag) != 0) continue;
            gabi::store<u32>(color.flag, 1);
            const f32 first = gabi::call<f32>(0x024BD0F0, color.firstValue);
            gabi::store<u8>(color.first, static_cast<u8>(gabi::ftoi(
                gabi::fmuls_ppc(first, gabi::load<f32>(0x100405F0)))));
            if (color.second != 0) {
                const f32 second = gabi::call<f32>(0x024BD0F0, color.secondValue);
                gabi::store<u8>(color.second, static_cast<u8>(gabi::ftoi(
                    gabi::fmuls_ppc(second, gabi::load<f32>(0x100405F0)))));
            }
        }
        for (int i = 0; i < 3; ++i) {
            const u32 callback = a + 0x159C + 0x10*i;
            if (gabi::load<s16>(a + 0x1646) == 0) {
                gabi::call(0x025A9D64, gabi::at<void>(callback));
                continue;
            }
            if (gabi::load<s16>(a + 0x1644) == 1) {
                const bool late = gabi::load<s16>(a + 0x1646) >= 5;
                const u32 primary = late ? (i == 1 ? 0x101D2458 : 0x101D2454)
                                         : (i == 1 ? 0x101D244C : 0x101D2448);
                const u32 environment = late ? 0x101D245C : 0x101D2450;
                const s8 room = gabi::load<s8>(a + 0x326);
                const u16 effect = gabi::load<u16>(0x101D2460 + 2*i);
                const u32 play = gabi::call<u32>(0x025200D4);
                const u32 emitter = gabi::call<u32>(0x025A847C,
                    gabi::at<void>(gabi::load<u32>(play + 0x5AB0)), 0, effect,
                    &actor->current.pos, &actor->shape_angle, &actor->scale, 0xFF,
                    gabi::at<void>(callback), room, gabi::at<void>(primary),
                    gabi::at<void>(environment), 0);
                if (emitter != 0) gabi::store<f32>(emitter + 0x238,
                    gabi::fadds_ppc(gabi::load<f32>(debug + 0x4C), gabi::load<f32>(0x100405D4)));
                gabi::store<s16>(a + 0x1644, static_cast<s16>(gabi::load<s16>(a + 0x1644) + 1));
            }
            gabi::store<u32>(callback + 8, a + 0x15CC);
            gabi::store<s16>(callback + 6, 10);
        }
        if (gabi::load<s16>(a + 0x1646) == 0) gabi::store<s16>(a + 0x1644, 0);
        else decrementShort(a + 0x1646);
    }
    const u32 environment = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, gabi::at<void>(environment), 0, &actor->current.pos, gabi::at<void>(a + 0x110));
}

// 024BECD8..024BF308: generic enemy reaction result and model calculation.
static void updateDamageAnimation(tn_class* actor, u32 damagePower) {
    const u32 a = gabi::ea(actor);
    for (u32 i = 0; i < 3; ++i)
        gabi::store<u16>(a + 0x328 + 2*i, gabi::load<u16>(a + 0x320 + 2*i));
    const u32 reaction = gabi::call<u32>(0x02041F94, gabi::at<void>(a + 0x590));
    if (reaction != 0) {
        gabi::store<s16>(a + 0x54C, 1);
        gabi::store<u32>(a + 0x1558, 0xFFFFFFFFu);
        switch (reaction) {
        case 1:
            gabi::call(0x024B91E4, actor, 0x4A, 0, -1,
                gabi::load<f32>(0x100405D0), gabi::load<f32>(0x100404FC));
            if (gabi::load<u32>(a + 0xD84) != 0) {
                bool drop = gabi::load<s8>(a + 0x3A1) <= 0 || damagePower >= 4;
                if (!drop) {
                    const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x100404FC));
                    // Native bge tests !CR-less; unordered skips the drop branch.
                    drop = random < gabi::load<f32>(0x10040504);
                }
                if (drop) gabi::store<u32>(a + 0xD88, 1);
            }
            gabi::store<s16>(a + 0x596, 0);
            break;
        case 2:
            gabi::call(0x024B91E4, actor, 0x4B, 0, -1,
                gabi::load<f32>(0x100405D0), gabi::load<f32>(0x100404FC));
            break;
        case 5:
            if (gabi::load<s16>(a + 0x596) != 4 && gabi::load<s16>(a + 0x596) != 11) {
                gabi::call(0x024BB620, actor);
                gabi::store<s16>(a + 0x596, 4); gabi::store<s16>(a + 0x594, 0);
                gabi::store<s16>(a + 0x4FA, 30);
            }
            gabi::store<s16>(a + 0x536, 5); gabi::store<s16>(a + 0x544, -0x4000);
            break;
        case 10:
            gabi::call(0x024BB620, actor);
            gabi::store<s16>(a + 0x594, 2); gabi::store<s16>(a + 0x596, 0);
            gabi::call(0x024BB800, actor);
            break;
        case 20:
            gabi::store<s16>(a + 0x536, 1); gabi::store<s16>(a + 0x544, -0x4000);
            gabi::store<s16>(a + 0x54C, static_cast<s16>(gabi::load<s16>(0x1046E794) + 16));
            gabi::store<u8>(a + 0x54E, 1);
            gabi::store<s16>(a + 0xA90, gabi::load<s16>(0x1046E7F4));
            break;
        case 21:
            gabi::store<s16>(a + 0x536, 1); gabi::store<s16>(a + 0x544, 0);
            gabi::store<s16>(a + 0xA90, gabi::load<s16>(0x1046E7F4));
            break;
        case 30:
            gabi::call(0x024B91E4, actor, 0x4B, 0, -1,
                gabi::load<f32>(0x100405D0), gabi::load<f32>(0x100404FC));
            if (gabi::load<u32>(a + 0xD84) != 0) gabi::store<u32>(a + 0xD88, 1);
            gabi::store<s16>(a + 0x596, 0);
            break;
        default: break;
        }
    }
    const u32 bodyMorf = gabi::load<u32>(a + 0x3E0);
    copyBaseMatrix(gabi::load<u32>(0x1018C7B0), gabi::load<u32>(bodyMorf + 0x90) + 0xC8);
    if ((gabi::load<u8>(a + 0x3EC) & 4) != 0) {
        const u32 shieldMorf = gabi::load<u32>(a + 0x3E4);
        copyBaseMatrix(gabi::load<u32>(0x1018C7B0), gabi::load<u32>(shieldMorf + 0x90) + 0xC8);
        if ((gabi::load<u8>(a + 0x3EC) & 4) != 0)
            gabi::call(0x025E55A0, gabi::at<void>(gabi::load<u32>(a + 0x3E4)));
    }
    gabi::call(0x025E55A0, gabi::at<void>(gabi::load<u32>(a + 0x3E0)));
    gabi::call(0x02041570, gabi::at<void>(a + 0x1A2C));
}

// Native stack damage record starts at r1+0x30. Only fields explicitly
// initialized by Execute are initialized here; callees produce the others.
struct DamageInfo {
    be<u32> hitObject; be<u32> attackActor;
    be<u8> power; be<u8> hitFlag; be<u8> kind; u8 reserved0B[3];
    be<s16> angle; u8 reserved10[2]; be<u16> flags;
    be<u32> hitPosition; be<u32> soundHandle;
};
static_assert(sizeof(DamageInfo) == 0x1C);
struct DamageSetup { u32 player; int hitPart; bool defenceHit; };
// 024BD870..024BDB1C: prepare defence sphere and select head/body hit.
static DamageSetup prepareDamage(tn_class* actor, DamageInfo* info) {
    const u32 a = gabi::ea(actor);
    const u32 brk = gabi::load<u32>(a + 0x3F0);
    gabi::store<f32>(brk + 4, static_cast<f32>(gabi::load<u8>(a + 0x4DC)));
    gabi::store<f32>(a + 0x3B8, gabi::load<f32>(0x100404F4));
    gabi::store<f32>(a + 0x3BC, gabi::load<f32>(0x100404F4));
    gabi::store<f32>(a + 0x3B4, gabi::load<f32>(0x1004064C));
    gabi::store<u8>(a + 0x3C4, 0); gabi::store<f32>(a + 0x3C0, gabi::load<f32>(0x100404E0));
    const u32 play = gabi::call<u32>(0x025200D4);
    DamageSetup state{gabi::load<u32>(play + 0x5B2C), 0, false};
    info->hitPosition = 0; info->flags = 0; info->power = 0;
    gabi::call(0x02515E50, gabi::at<void>(a + 0xD04));
    gabi::Local<cXyz> origin;
    gabi::Local<cXyz> hiddenCenter;
    gabi::Local<u8[16]> outgoingLinkage;
    if (gabi::load<s8>(a + 0x1594) != 0) {
        gabi::store<s8>(a + 0x1594, static_cast<s8>(gabi::load<s8>(a + 0x1594) - 1));
        copyBodyJointToCalc(actor, 0x2D0);
        origin->x = origin->y = origin->z = gabi::load<f32>(0x100404E0);
        gabi::call(0x0200FCD8, origin.get(), gabi::at<cXyz>(a + 0x1534));
        gabi::call(0x02018C8C, gabi::at<void>(a + 0x1268), gabi::load<f32>(0x10040650));
        gabi::call(0x02018D40, gabi::at<void>(a + 0x1268), gabi::at<cXyz>(a + 0x1534));
    } else {
        *hiddenCenter = actor->current.pos;
        hiddenCenter->y = gabi::load<f32>(0x10040654);
        gabi::call(0x02018D40, gabi::at<void>(a + 0x1268), hiddenCenter.get());
    }
    const u32 currentPlay = gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240, gabi::at<void>(currentPlay + 0x26A4), gabi::at<void>(a + 0x1150));
    if (gabi::call<int>(0x025162A4, gabi::at<void>(a + 0x1150)) != 0) {
        const u32 hit = gabi::call<u32>(0x02516300, gabi::at<void>(a + 0x1150));
        gabi::call(0x02518CC8, actor, gabi::at<void>(hit), 0x40);
        gabi::call(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(a + 0x32A));
        origin->x = origin->y = gabi::load<f32>(0x100404E0);
        origin->z = gabi::load<f32>(0x10040658);
        gabi::call(0x0200FCD8, origin.get(), gabi::at<cXyz>(a + 0x9F0));
        gabi::store<f32>(a + 0xA98, gabi::load<f32>(0x10040658));
        state.defenceHit = true;
        return state;
    }
    if (gabi::load<s16>(a + 0x508) != 0) return state;
    if (gabi::call<int>(0x025162A4, gabi::at<void>(a + 0x1024)) == 0 &&
        gabi::call<int>(0x025162A4, gabi::at<void>(a + 0xEF4)) == 0) return state;
    gabi::store<s16>(a + 0x508, static_cast<s16>(gabi::load<s16>(0x1047B696) + 5));
    info->hitObject = 0;
    if (gabi::call<int>(0x025162A4, gabi::at<void>(a + 0x1024)) != 0) {
        state.hitPart = 1;
        info->hitObject = gabi::call<u32>(0x02516300, gabi::at<void>(a + 0x1024));
        info->hitPosition = a + 0x10F0;
    } else if (gabi::call<int>(0x025162A4, gabi::at<void>(a + 0xEF4)) != 0) {
        state.hitPart = 2;
        info->hitObject = gabi::call<u32>(0x02516300, gabi::at<void>(a + 0xEF4));
        info->hitPosition = a + 0xFC0;
    }
    // Native repeated collision query can leave a null hit; bypass knockback.
    if (static_cast<u32>(info->hitObject) == 0) state.defenceHit = true;
    return state;
}

static void deleteMantle(tn_class* actor) {
    const u32 a = gabi::ea(actor);
    const u32 id = gabi::load<u32>(a + 0x1598);
    if (id == 0xFFFF) return;
    const u32 mantle = findActor(id);
    if (mantle != 0) gabi::call(0x025D57E0, gabi::at<void>(mantle));
    gabi::store<u32>(a + 0x1598, 0xFFFF);
}
// 024BDB1C..024BDD88. Returns true for ice/shatter paths that bypass
// ordinary damage classification and continue with movement and zero power.
static bool applyIceOrShatter(tn_class* actor, u32 hitObject, int hitPart) {
    const u32 a = gabi::ea(actor);
    const u32 mask = gabi::load<u32>(hitObject + 0x10);
    if ((mask & 0x180000) == 0) return false;
    if ((mask & 0x80000) != 0) {
        const u8 equipment = gabi::load<u8>(a + 0x3EC);
        if (!((hitPart == 2 && (equipment & 1) == 0) ||
              (hitPart == 1 && (equipment & 2) == 0))) return false;
        gabi::store<u8>(a + 0x1680, 1);
        gabi::store<s16>(a + 0x596, 0); gabi::store<s16>(a + 0x594, -10);
        gabi::store<u8>(a + 0x3A1, static_cast<u8>(gabi::load<u8>(a + 0x3A1) + 4));
        gabi::store<s16>(a + 0x1678, static_cast<s16>(gabi::load<s16>(0x1047B690) + 60));
        const f32 blend = gabi::load<f32>(0x100405D0), speed = gabi::load<f32>(0x100404E0);
        gabi::call(0x024B91E4, actor, 0x33, 0, -1, blend, speed);
        gabi::call(0x024B931C, actor, 0x33, 0, blend, speed);
        gabi::call(0x024B93E8, actor, 0x60, 0, blend, speed);
    } else {
        gabi::store<u8>(a + 0x167A, 1);
        for (int i = 0; i < 3; ++i) {
            const u32 part = a + 0x3F8 + 0x4C*i;
            if (i <= 1 && gabi::load<s8>(part + 8) >= 0)
                gabi::call(0x024BC070, actor, gabi::at<cXyz>(part + 0xC), static_cast<u8>(i));
            gabi::store<s8>(part + 8, -1);
        }
    }
    gabi::call(0x02041C30, gabi::at<void>(a + 0x1A2C));
    gabi::store<s16>(a + 0x54C, 0);
    for (u32 offset : {0x159Cu, 0x15ACu, 0x15BCu}) gabi::call(0x025A9D64, gabi::at<void>(a + offset));
    for (u32 offset : {0x550u, 0x570u, 0xD38u}) gabi::call(0x025A5F88, gabi::at<void>(a + offset));
    if (gabi::load<u32>(a + 0xD84) != 0) gabi::store<u32>(a + 0xD88, 2);
    deleteMantle(actor);
    return true;
}

static void monsterSound(tn_class* actor, int sound) {
    const u32 a = gabi::ea(actor);
    const s8 room = gabi::load<s8>(a + 0x326);
    const u32 id = gabi::load<u32>(a + 4);
    const int reverb = gabi::call<int>(0x02520540, room);
    gabi::call(0x025E1AA4, sound, &actor->eyePos, id, 0, reverb);
}
static u16 absoluteHitYaw(tn_class* actor) {
    const u32 a = gabi::ea(actor);
    s16 difference = static_cast<s16>(gabi::load<s16>(a + 0x532) - gabi::load<s16>(a + 0x322));
    if (difference < 0) difference = static_cast<s16>(-difference);
    return static_cast<u16>(difference);
}
// 024BE3A0..024BE5B4: actual hit-power/type classification.
static int applyOrdinaryDamage(tn_class* actor, DamageInfo* info, const DamageSetup& state) {
    const u32 a = gabi::ea(actor);
    const s8 oldHealth = gabi::load<s8>(a + 0x3A1);
    gabi::call(0x02518DB0, info);
    if (info->kind == 10 || info->kind == 14) gabi::store<s8>(a + 0x3A1, 20);
    gabi::store<s8>(a + 0x3A8, gabi::load<s8>(a + 0x1550));
    info->attackActor = gabi::call<u32>(0x025192A8, actor, info);
    if (info->kind == 10 || info->kind == 14) {
        gabi::store<s8>(a + 0x3A1, oldHealth);
        if (info->kind == 14 && gabi::load<s8>(a + 0x1550) == 0) gabi::store<s8>(a + 0x1550, 1);
    }
    gabi::Local<cXyz> soundPosition;
    gabi::Local<u8[16]> outgoingLinkage;
    *soundPosition = actor->current.pos;
    gabi::store<s16>(a + 0x50A, 25);
    gabi::call(0x0255F458, soundPosition.get(), 100, gabi::load<u32>(a + 4), 5);
    if (info->kind == 12) gabi::store<s16>(a + 0x508, 10);
    gabi::store<u16>(a + 0x156C, gabi::load<u16>(a + 0x156C) | static_cast<u16>(info->flags));
    gabi::store<s8>(a + 0x1583, info->kind == 10
        ? static_cast<s8>(gabi::load<s16>(0x1047B68E) + 8) : 0);
    if (info->kind == 1) {
        if (gabi::load<u8>(state.player + 0x3AC) == 5) return 2;
        if (info->hitFlag != 0) return absoluteHitYaw(actor) > 0x4000 ? 3 : 1;
        return state.hitPart == 1 ? 4 : 5;
    }
    const int reaction = info->kind == 2 || info->hitFlag != 0 ? 7 : 4;
    gabi::call(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), static_cast<s16>(info->angle));
    return reaction;
}
// 024BE5B4..024BE8DC, also run with reaction0 when no new hit occurred.
static void updateKnockback(tn_class* actor, DamageInfo* info, int reaction) {
    const u32 a = gabi::ea(actor);
    if ((reaction >= 1 && reaction <= 5) || reaction == 7) {
        const u32 flag = reaction == 2 || reaction == 3 ? 0x40 : reaction == 5 ? 0x20 : 0x10;
        gabi::store<u32>(a + 0x9E8, gabi::load<u32>(a + 0x9E8) | flag);
        if (reaction == 2) {
            gabi::call(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018C7B0)),
                static_cast<s16>(gabi::load<s16>(a + 0x322) + 0x8000));
            gabi::store<f32>(a + 0x9EC, gabi::load<f32>(0x1004065C));
        } else {
            gabi::store<f32>(a + 0x9EC, gabi::load<f32>(reaction == 4 || reaction == 5 ? 0x10040660 : 0x1004065C));
            if (reaction != 7) gabi::call(0x025F1884,
                gabi::at<void>(gabi::load<u32>(0x1018C7B0)), gabi::load<s16>(a + 0x532));
        }
        if (reaction == 5) {
            gabi::call(0x024B91E4, actor, 0x4A, 0, -1,
                gabi::load<f32>(0x100405D0), gabi::load<f32>(0x100404FC));
            gabi::store<s16>(a + 0xA4E, 10); gabi::store<u8>(a + 0xCD2, 7);
            gabi::store<f32>(a + 0xA38, gabi::load<f32>(0x10040640));
            if (gabi::load<s16>(a + 0x536) == 0) {
                gabi::store<s16>(a + 0x536, 3); gabi::store<s16>(a + 0x544, 0);
            }
        }
    }
    if (gabi::load<u32>(a + 0x9E8) != 0) {
        gabi::Local<cXyz> offset;
        gabi::Local<u8[16]> outgoingLinkage;
        offset->x = offset->y = gabi::load<f32>(0x100404E0);
        offset->z = gabi::load<f32>(0x10040658);
        gabi::call(0x0200FCD8, offset.get(), gabi::at<cXyz>(a + 0x9F0));
        if (gabi::load<f32>(a + 0x9EC) < gabi::load<f32>(0x10040664)) {
            gabi::store<f32>(a + 0xA98, -gabi::load<f32>(0x1046E844));
        } else {
            const f32 random = gabi::call<f32>(0x020198D8, gabi::load<f32>(0x10040510));
            gabi::store<f32>(a + 0x9EC, gabi::fadds_ppc(random, gabi::load<f32>(0x10040650)));
        }
    }
    if (reaction == 0) return;
    if (gabi::load<s8>(a + 0x3A1) <= 0 && info->hitFlag != 0) {
        monsterSound(actor, 0x488E);
        if ((gabi::load<u32>(a + 0x2E0) & 0x04000000) != 0) {
            if (gabi::load<s8>(0x1046E73D) == 0) {
                gabi::store<s8>(a + 0x1648, 50); gabi::call(0x025E1928);
            } else gabi::store<s8>(0x1046E73D, 0);
        }
    } else monsterSound(actor, 0x488D);
}

static void impactParticle(tn_class* actor, int effect, csXyz* rotation, cXyz* scale) {
    const u32 a = gabi::ea(actor), play = gabi::call<u32>(0x025200D4);
    gabi::call(0x025A847C, gabi::at<void>(gabi::load<u32>(play + 0x5AB0)),
        0, effect, gabi::at<cXyz>(a + 0xFC0), rotation, scale, 0xFF,
        nullptr, -1, nullptr, nullptr, 0);
}
// True means an armored hit bypasses ordinary damage/knockback classification.
static bool applyArmoredHit(tn_class* actor, DamageInfo* info, const DamageSetup& state) {
    const u32 a = gabi::ea(actor), hit = info->hitObject;
    const u32 attackMask = gabi::load<u32>(hit + 0x10);
    const u8 equipment = gabi::load<u8>(a + 0x3EC);
    if ((attackMask & 0x40200) != 0)
        gabi::store<s16>(a + 0x1A30, static_cast<s16>(gabi::load<s16>(0x1047B68C) + ((equipment & 1) ? 40 : 100)));
    if (!((state.hitPart == 2 && (equipment & 1)) || (state.hitPart == 1 && (equipment & 2)))) return false;
    const u32 status = gabi::load<u32>(hit + 0x44);
    const u32 attacker = status == 0 ? 0 : gabi::load<u32>(status + 0xC);
    if (attacker == 0 || gabi::load<s16>(attacker + 8) != 0xA8) {
        if ((attackMask & 0x188000) == 0) gabi::call(0x02518CC8, actor, gabi::at<void>(hit), 0x40);
        return true;
    }
    if (state.hitPart == 1 && gabi::load<u8>(state.player + 0x3AC) != 5) {
        gabi::call(0x02518CC8, actor, gabi::at<void>(hit), 0x40); return true;
    }
    gabi::Local<csXyz> rotation;
    gabi::Local<cXyz> scale;
    gabi::Local<u8[16]> outgoingLinkage;
    // Helmet branch changes animation/sound state before preparing particles.
    if (state.hitPart == 1) {
        gabi::call(0x02518DB0, info); gabi::call(0x025E1FD8);
        gabi::call(0x025E1D30, gabi::at<void>(static_cast<u32>(info->soundHandle)));
        gabi::store<u8>(a + 0x3F5, gabi::load<u8>(a + 0x3F5) | 2);
        gabi::store<s16>(a + 0x596, 17); gabi::store<s16>(a + 0x508, 20);
        gabi::store<u32>(a + 0x2E0, gabi::load<u32>(a + 0x2E0) & ~0x100u);
        gabi::store<s16>(a + 0x594, 10);
        gabi::store<u8>(0x101EACB7, static_cast<u8>(gabi::load<s16>(0x1047B696) + 6));
        const int reverb = gabi::call<int>(0x02520540, gabi::load<s8>(a + 0x326));
        gabi::call(0x025E1A40, 0x58E9, &actor->eyePos, 0, reverb);
    }
    const u16 yawMagnitude = state.hitPart == 2 ? absoluteHitYaw(actor) : 0;
    rotation->x = rotation->z = 0;
    scale->x = scale->y = scale->z = gabi::load<f32>(0x10040630);
    const u32 play = gabi::call<u32>(0x025200D4);
    rotation->y = gabi::call<s16>(0x025D6894, actor, gabi::at<void>(gabi::load<u32>(play + 0x12A0 + 0x488C)));
    if (state.hitPart == 1) {
        impactParticle(actor, 0x0D, rotation.get(), scale.get());
        impactParticle(actor, 0x10, nullptr, nullptr); return true;
    }
    bool mantleBusy = false;
    if (gabi::load<u32>(a + 0x1598) != 0xFFFF) {
        const u32 mantle = findActor(gabi::load<u32>(a + 0x1598));
        mantleBusy = mantle != 0 && (gabi::load<s8>(mantle + 0x4280) != 0 || gabi::load<s16>(mantle + 0x4282) != 0);
    }
    if (!mantleBusy && yawMagnitude > 0x4000) {
        gabi::store<s16>(a + 0x596, 17);
        gabi::store<u8>(a + 0x3F5, gabi::load<u8>(a + 0x3F5) | 1);
        gabi::store<u32>(a + 0x2E0, gabi::load<u32>(a + 0x2E0) & ~0x100u);
        gabi::store<s16>(a + 0x594, 0); gabi::store<s16>(a + 0x508, 20);
        gabi::store<u8>(0x101EACB7, static_cast<u8>(gabi::load<s16>(0x1047B696) + 6));
        gabi::call(0x02518DB0, info); gabi::call(0x025E1FD8);
        gabi::call(0x025E1D30, gabi::at<void>(static_cast<u32>(info->soundHandle)));
        impactParticle(actor, 0x0D, rotation.get(), scale.get()); impactParticle(actor, 0x10, nullptr, nullptr);
        deleteMantle(actor);
        gabi::call(0x02518CC8, actor, gabi::at<void>(static_cast<u32>(info->hitObject)), 0x21);
        monsterSound(actor, 0x4892); return true;
    }
    gabi::call(0x02518CC8, actor, gabi::at<void>(static_cast<u32>(info->hitObject)), 0x40);
    impactParticle(actor, 0x0D, rotation.get(), scale.get());
    gabi::store<s16>(a + 0x1552, 15); gabi::call(0x02518DB0, info);
    const s8 health = static_cast<s8>(gabi::load<u8>(a + 0x3A1) - info->power);
    gabi::store<s8>(a + 0x3A1, health);
    if (health <= 0) {
        gabi::store<s16>(a + 0x596, 17); gabi::store<s16>(a + 0x594, 10);
        gabi::store<u8>(a + 0x3EC, gabi::load<u8>(a + 0x3EC) & ~1u);
        gabi::store<s16>(a + 0x508, 20);
        gabi::store<u32>(a + 0x2E0, gabi::load<u32>(a + 0x2E0) | 0x200000);
        gabi::store<u8>(0x101EACB7, static_cast<u8>(gabi::load<s16>(0x1047B696) + 6));
        gabi::store<s8>(a + 0x400, -1);
        gabi::call(0x024BC070, actor, gabi::at<cXyz>(a + 0x404), 0);
        gabi::store<s8>(a + 0x3A1, (gabi::load<u8>(a + 0x4DD) & 1) ? 20 : 10);
        deleteMantle(actor);
    }
    return true;
}
} // namespace tn_execute_proposal

// Complete ordinary-game proposal. The entry macro stays first; helpers have
// no entry macros, and all guest pointee temporaries are protected by linkage.
BOOL daTn_Execute(tn_class* actor) {
    WWHD_FUNC(0x024BD170, BOOL, actor);
    using namespace tn_execute_proposal;
    PrefixState prefix{};
    if (executePrefix(actor, prefix) == PrefixResult::Finished) return TRUE;
    gabi::Local<DamageInfo> info;
    gabi::Local<u8[16]> outgoingLinkage;
    const DamageSetup damage = prepareDamage(actor, info.get());
    bool bypassOrdinary = damage.defenceHit;
    int reaction = 0;
    if (!bypassOrdinary && damage.hitPart != 0) {
        if (static_cast<u32>(info->hitObject) == 0) bypassOrdinary = true;
        else {
            bypassOrdinary = applyIceOrShatter(actor, info->hitObject, damage.hitPart);
            if (!bypassOrdinary) bypassOrdinary = applyArmoredHit(actor, info.get(), damage);
            if (!bypassOrdinary) reaction = applyOrdinaryDamage(actor, info.get(), damage);
        }
    }
    if (!bypassOrdinary) updateKnockback(actor, info.get(), reaction);
    const u32 damagePower = bypassOrdinary ? 0 : info->power;
    // Source-level call integrates the owner's readable movement implementation.
    Tn_move(actor);
    updateDemoCameraAndSmoke(actor);
    updateDamageAnimation(actor, damagePower);
    updateCollisionShapes(actor, prefix.playerAddress);
    updateGroundTilt(actor);
    updateHeldWeapon(actor);
    finishNormalEffects(actor);
    return TRUE;
}
VERIFY(0x024BD170, daTn_Execute);
