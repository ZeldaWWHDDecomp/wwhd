// Player NPC isolated reference implementation.
// Includes inferred per-TU initializer; excludes Pet and shared virtual targets.
#include "d/actor/d_a_player_npc.h"
#include <cmath>

namespace {
template<class T> T rd(u32 base, u32 off = 0) { return gabi::load<T>(base + off); }
template<class T> void wr(u32 base, u32 off, T value) { gabi::store<T>(base + off, value); }
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 save() { return rd<u32>(0x101F84DC); }
f32 constant(u32 address) { return rd<f32>(address); }
// Native sequential integer copies preserve floating-point payloads and alias order.
void copy3(u32 dst, u32 src) {
    wr<u32>(dst, 0, rd<u32>(src));
    wr<u32>(dst, 4, rd<u32>(src, 4));
    wr<u32>(dst, 8, rd<u32>(src, 8));
}
u32 stageVirtual(u32 slot) {
    const u32 stage = play() + 0x5150;
    const u32 target = rd<u32>(rd<u32>(stage), slot);
    return gabi::call_ptr<u32>(target, stage);
}
void assertion(u32 line, u32 expression) {
    gabi::call<void>(0x0273AA24, 0x10037EDCu, line, expression);
}
}

void* daPy_npc_JudgeForPNameAndDistance(void* actor, PlayerNpcJudge* parameter) {
    WWHD_FUNC(0x02444F74, void*, actor, parameter);
    const u32 a = gabi::ea(actor), p = gabi::ea(parameter);
    const s16 required = rd<s16>(p);
    const s16 name = a ? rd<s16>(a, 8) : 0x7FFF;
    if (name == required) {
        gabi::Local<PlayerNpcVector> relative;
        const u32 v = gabi::ea(relative.get());
        gabi::call<void>(0x0201ADE0, a + 0x314, v, p + 4);
        const f64 square = gabi::call<f64>(0x028E8DD0, v);
        const f64 distance = gabi::call<f64>(0x028F4384, square);
        if (distance < rd<f32>(p, 0x1C) && distance < rd<f32>(p, 0x20)) {
            wr<u32>(p, 0x24, a);
            wr<f32>(p, 0x20, static_cast<f32>(distance));
            copy3(p + 0x10, v);
        }
    }
    return nullptr;
}
VERIFY(0x02444F74, daPy_npc_JudgeForPNameAndDistance);

void* daPy_npc_SearchAreaByName(void* actor, s16 processName, f32 radius,
                               PlayerNpcVector* output) {
    WWHD_FUNC(0x02445018, void*, actor, processName, radius, output);
    const u32 a = gabi::ea(actor), out = gabi::ea(output);
    gabi::Local<PlayerNpcJudge> parameter;
    const u32 p = gabi::ea(parameter.get());
    const u32 y = rd<u32>(a, 0x318), x = rd<u32>(a, 0x314), z = rd<u32>(a, 0x31C);
    const f32 zero = constant(0x10037E30);
    wr<f32>(p, 0x1C, radius);
    wr<u32>(p, 0xC, z);
    wr<f32>(p, 0x10, zero);
    wr<f32>(p, 0x20, radius);
    wr<f32>(p, 0x18, zero);
    wr<s16>(p, 0, processName);
    wr<u32>(p, 0x24, 0);
    wr<f32>(p, 0x14, zero);
    wr<u32>(p, 4, x);
    wr<u32>(p, 8, y);
    gabi::call<void>(0x025D5218, 0x02444F74u, p);
    const u32 found = rd<u32>(p, 0x24);
    if (out) {
        const u32 yOut = rd<u32>(p, 0x14), xOut = rd<u32>(p, 0x10), zOut = rd<u32>(p, 0x18);
        wr<u32>(out, 0, xOut); wr<u32>(out, 8, zOut); wr<u32>(out, 4, yOut);
    }
    return gabi::at<void>(found);
}
VERIFY(0x02445018, daPy_npc_SearchAreaByName);

daPy_npc_c* daPy_npc_Construct(daPy_npc_c* actor) {
    WWHD_FUNC(0x024450B4, daPy_npc_c*, actor);
    u32 a = gabi::ea(actor);
    if (!a) { a = gabi::call<u32>(0x0273AD10, 0x608u); if (!a) return nullptr; }
    gabi::call<void>(0x023D47E8, a);
    wr<u32>(a, 0xB4, 0x10037F50);
    gabi::call<void>(0x024F0474, a + 0x43C);
    wr<u32>(a, 0x45C, 0x10037E58);
    wr<u32>(a, 0x44C, 0x10037E48);
    wr<u32>(a, 0x450, 0x10037E68);
    wr<u8>(a, 0x454, 1);
    return gabi::at<daPy_npc_c>(a);
}
VERIFY(0x024450B4, daPy_npc_Construct);

void daPy_npc_Destruct(daPy_npc_c* actor, s32 flags) {
    WWHD_FUNC(0x0244513C, void, actor, flags);
    const u32 a = gabi::ea(actor);
    if (!a) return;
    wr<u32>(a, 0x45C, 0x10037E58);
    wr<u32>(a, 0x450, 0x10037E68);
    gabi::call<void>(0x024EFD9C, a + 0x43C, 0u);
    gabi::call<void>(0x023D483C, a, 0u);
    if (static_cast<u32>(flags) & 1) gabi::call<void>(0x0273AF40, a);
}
VERIFY(0x0244513C, daPy_npc_Destruct);

s32 daPy_npc_check_initialRoom(daPy_npc_c* actor) {
    WWHD_FUNC(0x024451B4, s32, actor);
    const u32 a = gabi::ea(actor);
    if (rd<s8>(a, 0x2FE) >= 0) return 1;
    u32 game = play();
    gabi::call<void>(0x024F08A8, a + 0x43C, game + 0x12A0);
    const f32 ground = rd<f32>(a, 0x4D0), invalid = constant(0x10037E78);
    if (ground == invalid) return 0;
    game = play();
    if (gabi::call<s32>(0x024EF0BC, game + 0x12A0, a + 0x524) == 4) return 0;
    game = play();
    const s32 room = gabi::call<s32>(0x024EF130, game + 0x12A0, a + 0x524);
    if (room < 0) return 0;
    play(); // Native accessor still occurs although status storage is a fixed global.
    const u32 statusAddress = 0x1047E8E8u + static_cast<u32>(room) * 0x22Cu;
    if (!(rd<u8>(statusAddress) & 0x10)) return 0;
    wr<u8>(a, 0x2FE, static_cast<u8>(room));
    wr<u8>(a, 0x326, static_cast<u8>(room));
    return -1;
}
VERIFY(0x024451B4, daPy_npc_check_initialRoom);

s32 daPy_npc_check_moveStop(daPy_npc_c* actor) {
    WWHD_FUNC(0x024452A0, s32, actor);
    const u32 a = gabi::ea(actor);
    const s32 room = rd<s8>(a, 0x326);
    play();
    const bool loaded = (rd<u8>(0x1047E8E8u + static_cast<u32>(room) * 0x22Cu) & 0x10) != 0;
    if (loaded && room >= 0) return 0;
    if (!loaded || rd<u8>(a, 0x604) >= 30) {
        const u16 yaw = rd<u16>(a, 0x2FA);
        const u32 y = rd<u32>(a, 0x2F0);
        wr<u16>(a, 0x32A, yaw); wr<u32>(a, 0x318, y);
        const u32 z = rd<u32>(a, 0x2F4), x = rd<u32>(a, 0x2EC);
        const f32 zero = constant(0x10037E30);
        wr<u32>(a, 0x314, x); wr<f32>(a, 0x370, zero); wr<u8>(a, 0x604, 0);
        const u32 xyAngle = rd<u32>(a, 0x2F8); const u16 pitch = rd<u16>(a, 0x2F8);
        wr<u32>(a, 0x320, xyAngle); const u16 roll = rd<u16>(a, 0x2FC);
        wr<u32>(a, 0x31C, z); wr<u16>(a, 0x32C, roll);
        const u32 roomWord = rd<u32>(a, 0x2FC);
        wr<u16>(a, 0x328, pitch); wr<u32>(a, 0x324, roomWord);
    }
    const u32 player = rd<u32>(play(), 0x5B2C);
    if (rd<u16>(player, 0xF8) != 3) wr<u32>(a, 0x3BC, rd<u32>(a, 0x3BC) & ~2u);
    return 1;
}
VERIFY(0x024452A0, daPy_npc_check_moveStop);

void daPy_npc_unconditionalSetRestart(daPy_npc_c* actor, s8 option) {
    WWHD_FUNC(0x02445398, void, actor, option);
    const u32 a = gabi::ea(actor);
    gabi::call<void>(0x025B9864, save() + 0x1148, option);
    u32 state = save();
    const s8 room = rd<s8>(state, 0x114A);
    const s16 yaw = rd<s16>(state, 0x114E);
    gabi::call<void>(0x025B8880, state + 0x1CC, static_cast<u32>(static_cast<u8>(option)), state + 0x1150, yaw, room);
    state = save(); copy3(a + 0x2EC, state + 0x1150);
    state = save(); wr<s16>(a, 0x2FA, rd<s16>(state, 0x114E));
    state = save(); wr<u8>(a, 0x2FE, rd<u8>(state, 0x114A));
}
VERIFY(0x02445398, daPy_npc_unconditionalSetRestart);

void daPy_npc_setRestart(daPy_npc_c* actor, s8 option) {
    WWHD_FUNC(0x02445438, void, actor, option);
    const u32 a = gabi::ea(actor), initial = save();
    if (rd<s8>(initial, 0x1149) != option) return;
    const u32 player = rd<u32>(play(), 0x5B2C), state = save();
    const u16 command = rd<u16>(player, 0xF8);
    const s8 room = rd<s8>(a, 0x326), restartRoom = rd<s8>(state, 0x114A);
    if (command != 3 && room != restartRoom) gabi::call<void>(0x02445398, a, option);
}
VERIFY(0x02445438, daPy_npc_setRestart);

void daPy_npc_setOffsetHomePos(daPy_npc_c* actor) {
    WWHD_FUNC(0x024454C8, void, actor);
    const u32 a = gabi::ea(actor);
    if (!rd<u32>(0x1046D330)) {
        wr<u32>(0x1046D330, 0, 1);
        const f32 zero = constant(0x10037E30), offset = constant(0x10037E7C);
        wr<f32>(0x1046D324, 8, zero); wr<f32>(0x1046D324, 0, offset); wr<f32>(0x1046D324, 4, zero);
    }
    const s16 yaw = rd<s16>(a, 0x2FA);
    gabi::call<void>(0x0200F9D4, a + 0x2EC, a + 0x2EC, yaw, 0x1046D324u);
}
VERIFY(0x024454C8, daPy_npc_setOffsetHomePos);

void daPy_npc_setPointRestart(daPy_npc_c* actor, s16 point, s8 option) {
    WWHD_FUNC(0x02445518, void, actor, point, option);
    const u32 a = gabi::ea(actor);
    if (!stageVirtual(0x44)) assertion(0xAD, 0x10037EF0);
    u32 scene = stageVirtual(0x16C);
    if (!scene) assertion(0xAF, 0x10037E80);
    if (point < 0 || static_cast<s32>(point) >= rd<s32>(scene)) assertion(0xB0, 0x10037E90);
    scene = rd<u32>(scene, 4);
    if (!scene) assertion(0xB3, 0x10037F14);
    const u32 playerInfo = stageVirtual(0x44);
    u32 entry = rd<u32>(playerInfo, 4);
    const u32 sceneEntry = scene + static_cast<u32>(static_cast<s32>(point)) * 12u;
    const u8 startCode = rd<u8>(sceneEntry, 8);
    u32 i = 0;
    s32 count = static_cast<s32>(stageVirtual(0x54));
    while (static_cast<s32>(i) < count) {
        if (rd<u8>(entry, 0x1D) == startCode) break;
        entry += 0x20; ++i;
        count = static_cast<s32>(stageVirtual(0x54));
    }
    if (i == stageVirtual(0x54)) assertion(0xBE, 0x10037EB8);
    copy3(a + 0x2EC, entry + 0xC);
    const s16 entryYaw = rd<s16>(entry, 0x1A);
    wr<u8>(a, 0x2FE, 0xFF); wr<s16>(a, 0x2FA, entryYaw);
    gabi::call<void>(0x024454C8, a);
    // The native captures different words around stores; avoid a struct assignment.
    const u32 angleXY = rd<u32>(a, 0x2F8); const u16 pitch = rd<u16>(a, 0x2F8);
    const u32 roomWord = rd<u32>(a, 0x2FC), y = rd<u32>(a, 0x2F0);
    wr<u16>(a, 0x328, pitch); const u16 roll = rd<u16>(a, 0x2FC);
    wr<u32>(a, 0x304, y); const s16 yaw = rd<s16>(a, 0x2FA);
    wr<u16>(a, 0x32C, roll); wr<s16>(a, 0x32A, yaw); wr<u32>(a, 0x30C, angleXY);
    const u32 z = rd<u32>(a, 0x2F4);
    wr<u32>(a, 0x318, y); wr<u32>(a, 0x31C, z);
    const u32 x = rd<u32>(a, 0x2EC);
    wr<u32>(a, 0x308, z); wr<u32>(a, 0x314, x); wr<u32>(a, 0x324, roomWord);
    wr<u32>(a, 0x300, x); wr<u32>(a, 0x320, angleXY); wr<u32>(a, 0x310, roomWord);
    gabi::call<void>(0x025B9834, save() + 0x1148, option, a + 0x2EC, yaw, -1);
    gabi::call<void>(0x025B8880, save() + 0x1CC, static_cast<u32>(static_cast<u8>(option)), a + 0x2EC, yaw, -1);
    const u32 state = save();
    const s8 room = rd<s8>(state, 0x114A); const s16 savedYaw = rd<s16>(state, 0x114E);
    gabi::call<void>(0x025B8880, state + 0x1CC, static_cast<u32>(static_cast<u8>(option)), state + 0x1150, savedYaw, room);
    gabi::call<void>(0x025D537C, a);
}
VERIFY(0x02445518, daPy_npc_setPointRestart);

s32 daPy_npc_checkRestart(daPy_npc_c* actor, s8 option) {
    WWHD_FUNC(0x02445784, s32, actor, option);
    const u32 a = gabi::ea(actor), initial = save();
    if (rd<s8>(initial, 0x1149) != option) return 0;
    const s16 point = rd<s16>(initial, 0x114C);
    if (point >= 0) { gabi::call<void>(0x02445518, a, point, static_cast<s8>(1)); return 1; }
    const u32 x = rd<u32>(initial, 0x1150); wr<u32>(a, 0x2EC, x);
    const u32 y = rd<u32>(initial, 0x1154); wr<u32>(a, 0x2F0, y);
    const u32 z = rd<u32>(initial, 0x1158); wr<u32>(a, 0x2F4, z);
    u32 state = save(); const s16 yaw = rd<s16>(state, 0x114E); const u16 pitch = rd<u16>(a, 0x2F8);
    wr<s16>(a, 0x2FA, yaw);
    state = save(); const u32 angleXY = rd<u32>(a, 0x2F8); const u8 room = rd<u8>(state, 0x114A);
    wr<s16>(a, 0x32A, yaw); wr<u8>(a, 0x2FE, room);
    const u32 roomWord = rd<u32>(a, 0x2FC); const u16 roll = rd<u16>(a, 0x2FC);
    wr<u32>(a, 0x314, x); wr<u16>(a, 0x32C, roll); wr<u32>(a, 0x300, x);
    wr<u32>(a, 0x30C, angleXY); wr<u16>(a, 0x328, pitch); wr<u32>(a, 0x318, y);
    wr<u32>(a, 0x308, z); wr<u32>(a, 0x324, roomWord); wr<u32>(a, 0x304, y);
    wr<u32>(a, 0x310, roomWord); wr<u32>(a, 0x31C, z); wr<u32>(a, 0x320, angleXY);
    return 1;
}
VERIFY(0x02445784, daPy_npc_checkRestart);

s32 daPy_npc_initialRestartOption(daPy_npc_c* actor, s8 option, s32 shouldSave) {
    WWHD_FUNC(0x0244586C, s32, actor, option, shouldSave);
    const u32 a = gabi::ea(actor);
    if (rd<u32>(play(), 0x5B38)) return 0;
    wr<u32>(play(), 0x5B38, a);
    if (shouldSave) {
        const u32 initial = save();
        if (rd<s8>(initial, 0x1149) != option) {
            const s8 room = rd<s8>(a, 0x326); const s16 yaw = rd<s16>(a, 0x2FA);
            gabi::call<void>(0x025B9834, initial + 0x1148, option, a + 0x2EC, yaw, room);
            gabi::call<void>(0x025B8880, save() + 0x1CC, static_cast<u32>(static_cast<u8>(option)), a + 0x2EC, yaw, room);
        }
    }
    // HD-only additional nonnull callback after registering the partner.
    const u32 manager = rd<u32>(0x101F8344);
    if (manager) { const u32 object = rd<u32>(manager, 0x218); if (object) gabi::call<void>(0x02688BBC, object); }
    return 1;
}
VERIFY(0x0244586C, daPy_npc_initialRestartOption);

s32 daPy_npc_checkNowPosMove(daPy_npc_c* actor, const char* name) {
    WWHD_FUNC(0x02445950, s32, actor, name);
    const u32 a = gabi::ea(actor), n = gabi::ea(name);
    if (!rd<u8>(play(), 0x5292)) return 1;
    if (rd<u16>(a, 0xF8) == 1) return 1;
    const u32 flags = rd<u32>(a, 0x600);
    if ((flags & 2) && !(flags & 1)) return 1;
    const u32 game = play();
    if (gabi::call<s32>(0x02542D88, game + 0x52C4, n, 0u, 0u) != -1) return 1;
    return (rd<u32>(a, 0x2E0) & 0x800) ? 1 : 0;
}
VERIFY(0x02445950, daPy_npc_checkNowPosMove);

void daPy_npc_drawDamageFog(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445A00, void, actor);
    const u32 a = gabi::ea(actor);
    if (!rd<u8>(a, 0x605)) return;
    gabi::Local<PlayerNpcVector> camera;
    const u32 v = gabi::ea(camera.get());
    gabi::call<void>(0x025F1108, a + 0x314, v);
    const u32 timer = rd<u32>(0x101FF560);
    const f32 cameraZ = rd<f32>(v, 8), span = constant(0x10037F28), endOffset = constant(0x10037F2C);
    const u32 sineIndex = ((timer << 11) & 0xFFFFu) >> 3;
    const f32 base = static_cast<f32>(-static_cast<f64>(cameraZ) - static_cast<f64>(span));
    const f32 adjustment = static_cast<f32>(std::fabs(static_cast<f64>(rd<f32>(0x104A44F8u + sineIndex * 8))));
    // HD color channels are three halfwords, not GC's three bytes.
    wr<u16>(a, 0x1B4, 60); wr<u16>(a, 0x1B2, 60);
    const f32 start = gabi::fmadds(span, adjustment, base);
    wr<u16>(a, 0x1B0, 255);
    const f32 end = static_cast<f32>(start + endOffset);
    wr<f32>(a, 0x1B8, start); wr<f32>(a, 0x1BC, end);
}
VERIFY(0x02445A00, daPy_npc_drawDamageFog);

s32 daPy_npc_chkMoveBlock(daPy_npc_c* actor, PlayerNpcVector* output) {
    WWHD_FUNC(0x02445AA4, s32, actor, output);
    const u32 a = gabi::ea(actor), out = gabi::ea(output);
    gabi::Local<PlayerNpcVector> relative, velocity, normalized, cross;
    const u32 r = gabi::ea(relative.get()), v = gabi::ea(velocity.get());
    const u32 n = gabi::ea(normalized.get()), c = gabi::ea(cross.get());
    const f32 radius = constant(0x10037F2C);
    const u32 block = gabi::call<u32>(0x02445018, a, static_cast<s16>(0x2B), radius, r);
    if (!block) return 0;
    gabi::call<void>(0x0201ADE0, block + 0x314, v, block + 0x300);
    const f64 square = gabi::call<f64>(0x028E8DD0, v);
    const f64 magnitude = gabi::call<f64>(0x028F4384, square);
    if (!(magnitude > constant(0x10037F30))) return 0; // Native ble: !GT, including unordered.
    if (out) {
        const u32 y = rd<u32>(v, 4), x = rd<u32>(v), z = rd<u32>(v, 8);
        wr<u32>(out, 0, x); wr<u32>(out, 8, z); wr<u32>(out, 4, y);
    }
    gabi::call<void>(0x0201B3C0, r, n);
    gabi::call<void>(0x0201B3C0, v, n);
    const f64 dot = gabi::call<f64>(0x028E8F44, r, v);
    const f32 zero = constant(0x10037E30);
    wr<f32>(v, 4, zero); wr<f32>(r, 4, zero);
    gabi::call<void>(0x0201B080, r, c, v);
    if (!(dot < constant(0x10037F34))) {
        const f32 selfY = rd<f32>(a, 0x318), offset = constant(0x10037E7C);
        const f32 limit = static_cast<f32>(selfY + offset);
        if (!(rd<f32>(block, 0x318) > limit)) return 0;
    }
    // Native bge is !LT: unordered follows the +1 return path.
    return rd<f32>(c, 4) < zero ? -1 : 1;
}
VERIFY(0x02445AA4, daPy_npc_chkMoveBlock);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445CF4(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445CF4, s32, actor);
    return -1;
}
VERIFY(0x02445CF4, daPy_npc_virtual_02445CF4);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445CFC(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445CFC, s32, actor);
    return 0;
}
VERIFY(0x02445CFC, daPy_npc_virtual_02445CFC);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D04(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D04, s32, actor);
    return 0;
}
VERIFY(0x02445D04, daPy_npc_virtual_02445D04);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D0C(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D0C, s32, actor);
    return 0;
}
VERIFY(0x02445D0C, daPy_npc_virtual_02445D0C);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D14(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D14, s32, actor);
    return 0;
}
VERIFY(0x02445D14, daPy_npc_virtual_02445D14);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D1C(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D1C, s32, actor);
    return 0;
}
VERIFY(0x02445D1C, daPy_npc_virtual_02445D1C);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D24(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D24, s32, actor);
    return 0;
}
VERIFY(0x02445D24, daPy_npc_virtual_02445D24);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D2C(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D2C, s32, actor);
    return 0;
}
VERIFY(0x02445D2C, daPy_npc_virtual_02445D2C);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D34(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D34, s32, actor);
    return 0;
}
VERIFY(0x02445D34, daPy_npc_virtual_02445D34);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D3C(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D3C, s32, actor);
    return 0;
}
VERIFY(0x02445D3C, daPy_npc_virtual_02445D3C);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445D44(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D44, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445D44, daPy_npc_virtual_02445D44);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D48(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D48, s32, actor);
    return 0;
}
VERIFY(0x02445D48, daPy_npc_virtual_02445D48);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D50(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D50, s32, actor);
    return -1;
}
VERIFY(0x02445D50, daPy_npc_virtual_02445D50);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D58(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D58, s32, actor);
    return -1;
}
VERIFY(0x02445D58, daPy_npc_virtual_02445D58);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D60(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D60, s32, actor);
    return -1;
}
VERIFY(0x02445D60, daPy_npc_virtual_02445D60);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D68(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D68, s32, actor);
    return 0;
}
VERIFY(0x02445D68, daPy_npc_virtual_02445D68);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D70(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D70, s32, actor);
    return 0;
}
VERIFY(0x02445D70, daPy_npc_virtual_02445D70);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D78(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D78, s32, actor);
    return 0;
}
VERIFY(0x02445D78, daPy_npc_virtual_02445D78);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D80(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D80, s32, actor);
    return 0;
}
VERIFY(0x02445D80, daPy_npc_virtual_02445D80);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445D88(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D88, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445D88, daPy_npc_virtual_02445D88);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445D8C(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D8C, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445D8C, daPy_npc_virtual_02445D8C);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445D90(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D90, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445D90, daPy_npc_virtual_02445D90);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445D94(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D94, s32, actor);
    return 0;
}
VERIFY(0x02445D94, daPy_npc_virtual_02445D94);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445D9C(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445D9C, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445D9C, daPy_npc_virtual_02445D9C);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445DA0(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445DA0, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445DA0, daPy_npc_virtual_02445DA0);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445DA4(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445DA4, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445DA4, daPy_npc_virtual_02445DA4);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445DA8(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445DA8, s32, actor);
    return 0;
}
VERIFY(0x02445DA8, daPy_npc_virtual_02445DA8);

// Distinct installed virtual-table target; role naming remains provisional.
s32 daPy_npc_virtual_02445DB0(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445DB0, s32, actor);
    return 1;
}
VERIFY(0x02445DB0, daPy_npc_virtual_02445DB0);

// Distinct installed virtual-table target; role naming remains provisional.
void daPy_npc_virtual_02445DB8(daPy_npc_c* actor) {
    WWHD_FUNC(0x02445DB8, void, actor);
    // Native blr leaves the integer return register untouched.
}
VERIFY(0x02445DB8, daPy_npc_virtual_02445DB8);

// Per-TU attribution inferred from adjacent NPC constants/vtable and distinct
// header-static descriptors; prior unresolved ownership proposal remains archived.
void daPy_npc_sinit() {
    WWHD_FUNC(0x02445C30, void);
    wr<u32>(0x1046D314, 12, 0); wr<u32>(0x1046D314, 8, 0);
    wr<u32>(0x1046D314, 4, 0); wr<u32>(0x1046D314, 0, 0);
    gabi::call<void>(0x028F026C, 0x101CEF50u);
    const f32 minusPi = constant(0x10037F3C), pi = constant(0x10037F40);
    wr<f32>(0x1046D2F8, 0, minusPi); wr<f32>(0x1046D2FC, 0, pi);
    gabi::call<void>(0x028ED6F8, 0x1046D310u);
    gabi::call<void>(0x028F026C, 0x101CEF5Cu);
    gabi::call<void>(0x028EAB2C, 0x1046D311u);
    gabi::call<void>(0x028F026C, 0x101CEF68u);
    const f32 large = constant(0x10037F44), small = constant(0x10037F48);
    wr<f32>(0x1046D300, 0, large); wr<f32>(0x1046D308, 0, small);
    wr<f32>(0x1046D304, 0, large); wr<f32>(0x1046D30C, 0, small);
}
VERIFY(0x02445C30, daPy_npc_sinit);
