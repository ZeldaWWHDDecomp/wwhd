/** Player base and matrix-follow particle callback (WWHD).
 * Reconstructed from the GameCube source and the HD unit.
 */
#include "bindings.h"
#include "d/actor/d_a_player.h"

static void daPy_callback_execute(daPy_mtxFollowEcallBack_c* callback, void* emitter) {
    WWHD_FUNC(0x023D4528, void, callback, emitter);
    u32 e = gabi::ea(emitter);
    gabi::call(0x028249B0, callback->mpMatrix.get(), e + 0x1F0, e + 0x22C);
}
VERIFY(0x023D4528, daPy_callback_execute);

static void daPy_callback_end(daPy_mtxFollowEcallBack_c* callback) {
    WWHD_FUNC(0x023D4538, void, callback);
    u32 emitter = gabi::ea(callback->mpEmitter.get());
    if (emitter == 0) return;
    u32 flags = gabi::load<u32>(emitter + 0x254);
    gabi::store<s32>(emitter + 0x5C, -1);
    gabi::store<u32>(emitter + 0x254, flags | 1);
    emitter = gabi::ea(callback->mpEmitter.get());
    flags = gabi::load<u32>(emitter + 0x254);
    gabi::store<u32>(emitter + 0x254, flags & ~0x40u);
    emitter = gabi::ea(callback->mpEmitter.get());
    gabi::store<u32>(emitter + 0x1E4, 0);
    callback->mpEmitter = nullptr;
}
VERIFY(0x023D4538, daPy_callback_end);

static void* daPy_makeEmitter(daPy_mtxFollowEcallBack_c* callback, u32 particle,
                            void* matrix, cXyz* position, cXyz* scale) {
    WWHD_FUNC(0x023D457C, void*, callback, particle, matrix, position, scale);
    gabi::call(0x023D4538, callback);
    callback->mpMatrix = matrix;
    u32 play = dComIfGp_ea();
    u32 particles = gabi::load<u32>(play + 0x5AB0);
    return gabi::call<void*>(0x025A847C, particles, 1, particle, position, (u32)0,
                             scale, 0xFF, callback, -1, (u32)0, (u32)0, (u32)0);
}
VERIFY(0x023D457C, daPy_makeEmitter);

static void* daPy_makeEmitterColor(daPy_mtxFollowEcallBack_c* callback, u32 particle,
                                 void* matrix, cXyz* position, void* primary, void* environment) {
    WWHD_FUNC(0x023D460C, void*, callback, particle, matrix, position, primary, environment);
    gabi::call(0x023D4538, callback);
    callback->mpMatrix = matrix;
    u32 play = dComIfGp_ea();
    u32 particles = gabi::load<u32>(play + 0x5AB0);
    return gabi::call<void*>(0x025A847C, particles, 1, particle, position, (u32)0,
                             (u32)0, 0xFF, callback, -1, primary, environment, (u32)0);
}
VERIFY(0x023D460C, daPy_makeEmitterColor);

static void daPy_changePlayer(daPy_py_c* player, fopAc_ac_c* replacement) {
    WWHD_FUNC(0x023D4688, void, player, replacement);
    if (replacement == nullptr) return;
    s32 room = replacement->current.roomNo;
    s32 stay = gabi::load<s8>(0x1047E6C8);
    if ((u32)stay != (u32)room) return;
    u32 play = dComIfGp_ea();
    gabi::store<u32>(play + 0x5B2C, gabi::ea(replacement));
    play = dComIfGp_ea();
    u32 camera = gabi::load<u32>(play + 0x5AF8);
    gabi::store<u32>(camera + 0x370, gabi::ea(replacement));
    play = dComIfGp_ea();
    u32 flags = gabi::load<u32>(play + 0x5824);
    gabi::store<u32>(play + 0x5824, flags | 0x80);
}
VERIFY(0x023D4688, daPy_changePlayer);

static void daPy_setDoButtonQuake(daPy_py_c* player) {
    WWHD_FUNC(0x023D46F4, void, player);
    u32 flags = player->mNoResetFlg0;
    if (flags & 0x200000) return;
    player->mNoResetFlg0 = flags | 0x200000;
    player->mQuakeTimer = 60;
    u32 play = dComIfGp_ea();
    gabi::Local<cXyz> direction;
    direction->x = 0.0f;
    direction->y = 1.0f;
    direction->z = 0.0f;
    // HD stores the pattern in read-only data; GC used a local word.
    gabi::call(0x025CB4E0, play + 0x599C, (u32)0x100347D4, 0, 1, direction.get());
}
VERIFY(0x023D46F4, daPy_setDoButtonQuake);

static void daPy_stopDoButtonQuake(daPy_py_c* player, s32 release) {
    WWHD_FUNC(0x023D4768, void, player, release);
    s32 timer = player->mQuakeTimer;
    if (timer > 0) {
        timer = (s16)(timer - 1);
        player->mQuakeTimer = (s16)timer;
        if (timer == 0) {
            u32 play = dComIfGp_ea();
            gabi::call(0x025CB610, play + 0x599C, -1);
        }
    }
    if (release != 0 && (s16)player->mQuakeTimer == 0)
        player->mNoResetFlg0 = (u32)player->mNoResetFlg0 & ~0x200000u;
}
VERIFY(0x023D4768, daPy_stopDoButtonQuake);

static daPy_py_c* daPy_base_constructor(daPy_py_c* player) {
    WWHD_FUNC(0x023D47E8, daPy_py_c*, player);
    if (player == nullptr) player = gabi::call<daPy_py_c*>(0x0273AD10, 0x43C);
    if (player != nullptr) {
        gabi::call(0x025D4ED0, player);
        gabi::store<u32>(gabi::ea(player) + 0xB4, 0x10034830);
    }
    return player;
}
VERIFY(0x023D47E8, daPy_base_constructor);

static void daPy_base_destructor(daPy_py_c* player, u32 flags) {
    WWHD_FUNC(0x023D483C, void, player, flags);
    if (player != nullptr) {
        gabi::call(0x025D50BC, player, 0);
        if (flags & 1) gabi::call(0x0273AF40, player);
    }
}
VERIFY(0x023D483C, daPy_base_destructor);

static void daPy_objWindHitCheck(daPy_py_c* player, void* cylinder) {
    WWHD_FUNC(0x023D4890, void, player, cylinder);
    gabi::Local<cXyz> target, radial, difference, scaled, radialScaled;
    target->x = 0.0f;
    target->y = 0.0f;
    target->z = 0.0f;
    f32 maxStep = 3.0f;
    if (gabi::call<s32>(0x025162A4, cylinder) != 0) {
        u32 hit = gabi::call<u32>(0x02516300, cylinder);
        if (hit != 0 && (gabi::load<u32>(hit + 0x10) & 0x200000)) {
            u32 cyl = gabi::ea(cylinder);
            f32 y = gabi::load<f32>(cyl + 0xC4);
            f32 z = gabi::load<f32>(cyl + 0xC8);
            target->y = y;
            radial->z = z;
            f32 x = gabi::load<f32>(cyl + 0xC0);
            radial->y = 0.0f;
            target->x = x;
            radial->x = x;
            target->z = z;
            f32 speed = gabi::call<f32>(0x028E8DD0, radial.get());
            speed = gabi::call<f32>(0x028F4384, speed);
            maxStep = 1.0f;
            f32 limit = 30.0f;
            if (speed < maxStep) {
                gabi::call(0x0201ADE0, &player->current.pos, difference.get(), cyl + 0xCC);
                gabi::call(0x0201AE48, difference.get(), scaled.get(), limit);
                x = scaled->x;
                z = scaled->z;
                radialScaled->y = 0.0f;
                radialScaled->x = x;
                target->z = z;
                y = scaled->y;
                radialScaled->z = z;
                target->y = y;
                target->x = x;
                speed = gabi::call<f32>(0x028E8DD0, radialScaled.get());
                speed = gabi::call<f32>(0x028F4384, speed);
            }
            if (speed > limit)
                gabi::call(0x028E8E64, target.get(), target.get(), limit / speed);
        }
    }
    gabi::call(0x0200ECD4, &player->mWindVelocity.x, (f32)target->x, 0.5f, maxStep, 0.5f);
    gabi::call(0x0200ECD4, &player->mWindVelocity.z, (f32)target->z, 0.5f, maxStep, 0.5f);
    f32 vx = player->mWindVelocity.x;
    f32 x = player->current.pos.x;
    f32 z = player->current.pos.z;
    f32 vz = player->mWindVelocity.z;
    player->current.pos.x = x + vx;
    player->current.pos.z = z + vz;
}
VERIFY(0x023D4890, daPy_objWindHitCheck);

static void daPy_header_static() {
    WWHD_FUNC(0x023D4A54, void);
    gabi::store<u32>(0x1046CC9C, 0);
    gabi::store<u32>(0x1046CC94, 0);
    gabi::store<u32>(0x1046CCA0, 0);
    gabi::store<u32>(0x1046CC98, 0);
    gabi::call(0x028F026C, (u32)0x101CEAD8);
    gabi::store<f32>(0x1046CC88, -3.141592741012573242f);
    gabi::store<f32>(0x1046CC8C, 3.141592741012573242f);
    gabi::call(0x028ED6F8, (u32)0x1046CC90);
    gabi::call(0x028F026C, (u32)0x101CEAE4);
    gabi::call(0x028EAB2C, (u32)0x1046CC91);
    gabi::call(0x028F026C, (u32)0x101CEAF0);
}
VERIFY(0x023D4A54, daPy_header_static);

static void daPy_empty_023D4AE8(u32 self) {
    WWHD_FUNC(0x023D4AE8, void, self);
}
VERIFY(0x023D4AE8, daPy_empty_023D4AE8);

static void daPy_empty_023D4AEC(u32 self) {
    WWHD_FUNC(0x023D4AEC, void, self);
}
VERIFY(0x023D4AEC, daPy_empty_023D4AEC);

static void daPy_empty_023D4AF0(u32 self) {
    WWHD_FUNC(0x023D4AF0, void, self);
}
VERIFY(0x023D4AF0, daPy_empty_023D4AF0);

static void daPy_empty_023D4B4C(u32 self) {
    WWHD_FUNC(0x023D4B4C, void, self);
}
VERIFY(0x023D4B4C, daPy_empty_023D4B4C);

static void daPy_empty_023D4B90(u32 self) {
    WWHD_FUNC(0x023D4B90, void, self);
}
VERIFY(0x023D4B90, daPy_empty_023D4B90);

static void daPy_empty_023D4B94(u32 self) {
    WWHD_FUNC(0x023D4B94, void, self);
}
VERIFY(0x023D4B94, daPy_empty_023D4B94);

static void daPy_empty_023D4B98(u32 self) {
    WWHD_FUNC(0x023D4B98, void, self);
}
VERIFY(0x023D4B98, daPy_empty_023D4B98);

static void daPy_empty_023D4BA4(u32 self) {
    WWHD_FUNC(0x023D4BA4, void, self);
}
VERIFY(0x023D4BA4, daPy_empty_023D4BA4);

static void daPy_empty_023D4BA8(u32 self) {
    WWHD_FUNC(0x023D4BA8, void, self);
}
VERIFY(0x023D4BA8, daPy_empty_023D4BA8);

static void daPy_empty_023D4BAC(u32 self) {
    WWHD_FUNC(0x023D4BAC, void, self);
}
VERIFY(0x023D4BAC, daPy_empty_023D4BAC);

static void daPy_callback_setup(daPy_mtxFollowEcallBack_c* callback, void* emitter) {
    WWHD_FUNC(0x023D4AF4, void, callback, emitter);
    callback->mpEmitter = emitter;
}
VERIFY(0x023D4AF4, daPy_callback_setup);

static s32 daPy_default_023D4B04(u32 self) {
    WWHD_FUNC(0x023D4B04, s32, self);
    return 0;
}
VERIFY(0x023D4B04, daPy_default_023D4B04);

static s32 daPy_default_023D4B0C(u32 self) {
    WWHD_FUNC(0x023D4B0C, s32, self);
    return 0;
}
VERIFY(0x023D4B0C, daPy_default_023D4B0C);

static s32 daPy_default_023D4B14(u32 self) {
    WWHD_FUNC(0x023D4B14, s32, self);
    return 0;
}
VERIFY(0x023D4B14, daPy_default_023D4B14);

static s32 daPy_default_023D4B1C(u32 self) {
    WWHD_FUNC(0x023D4B1C, s32, self);
    return 0;
}
VERIFY(0x023D4B1C, daPy_default_023D4B1C);

static s32 daPy_default_023D4B24(u32 self) {
    WWHD_FUNC(0x023D4B24, s32, self);
    return 0;
}
VERIFY(0x023D4B24, daPy_default_023D4B24);

static s32 daPy_default_023D4B2C(u32 self) {
    WWHD_FUNC(0x023D4B2C, s32, self);
    return 0;
}
VERIFY(0x023D4B2C, daPy_default_023D4B2C);

static s32 daPy_default_023D4B34(u32 self) {
    WWHD_FUNC(0x023D4B34, s32, self);
    return 0;
}
VERIFY(0x023D4B34, daPy_default_023D4B34);

static s32 daPy_default_023D4B3C(u32 self) {
    WWHD_FUNC(0x023D4B3C, s32, self);
    return 0;
}
VERIFY(0x023D4B3C, daPy_default_023D4B3C);

static s32 daPy_default_023D4B44(u32 self) {
    WWHD_FUNC(0x023D4B44, s32, self);
    return 0;
}
VERIFY(0x023D4B44, daPy_default_023D4B44);

static s32 daPy_default_023D4B50(u32 self) {
    WWHD_FUNC(0x023D4B50, s32, self);
    return 0;
}
VERIFY(0x023D4B50, daPy_default_023D4B50);

static s32 daPy_default_023D4B70(u32 self) {
    WWHD_FUNC(0x023D4B70, s32, self);
    return 0;
}
VERIFY(0x023D4B70, daPy_default_023D4B70);

static s32 daPy_default_023D4B78(u32 self) {
    WWHD_FUNC(0x023D4B78, s32, self);
    return 0;
}
VERIFY(0x023D4B78, daPy_default_023D4B78);

static s32 daPy_default_023D4B80(u32 self) {
    WWHD_FUNC(0x023D4B80, s32, self);
    return 0;
}
VERIFY(0x023D4B80, daPy_default_023D4B80);

static s32 daPy_default_023D4B88(u32 self) {
    WWHD_FUNC(0x023D4B88, s32, self);
    return 0;
}
VERIFY(0x023D4B88, daPy_default_023D4B88);

static s32 daPy_default_023D4B9C(u32 self) {
    WWHD_FUNC(0x023D4B9C, s32, self);
    return 0;
}
VERIFY(0x023D4B9C, daPy_default_023D4B9C);

static s32 daPy_default_023D4BB0(u32 self) {
    WWHD_FUNC(0x023D4BB0, s32, self);
    return 0;
}
VERIFY(0x023D4BB0, daPy_default_023D4BB0);

static s32 daPy_default_023D4AFC(u32 self) {
    WWHD_FUNC(0x023D4AFC, s32, self);
    return -1;
}
VERIFY(0x023D4AFC, daPy_default_023D4AFC);

static s32 daPy_default_023D4B58(u32 self) {
    WWHD_FUNC(0x023D4B58, s32, self);
    return -1;
}
VERIFY(0x023D4B58, daPy_default_023D4B58);

static s32 daPy_default_023D4B60(u32 self) {
    WWHD_FUNC(0x023D4B60, s32, self);
    return -1;
}
VERIFY(0x023D4B60, daPy_default_023D4B60);

static s32 daPy_default_023D4B68(u32 self) {
    WWHD_FUNC(0x023D4B68, s32, self);
    return -1;
}
VERIFY(0x023D4B68, daPy_default_023D4B68);
