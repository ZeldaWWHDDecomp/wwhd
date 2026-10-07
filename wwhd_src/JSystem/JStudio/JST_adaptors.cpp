// Scoped WWHD JStudio adaptors, reconstructed against CC0 TWW templates.
// HD call slots, offsets and unsigned-duration conversions from the binary.
#include "gabi.h"
namespace jst_adaptors {
using namespace gabi;
template<unsigned N> struct Bytes { u8 data[N]; };
static u32 vfn(u32 object, u32 slot) { return load<u32>(load<u32>(object) + slot); }
static void getVec(u32 self, u32 out, u32 indices) { call<void>(0x0283A1E0, self, out, indices); }
static void getColor(u32 self, u32 out, u32 indices) { call<void>(0x0283A318, self, out, indices); }
static void copyWords(u32 dst, u32 src, unsigned count) {
    for (unsigned i = 0; i < count; ++i) store<u32>(dst + i * 4, load<u32>(src + i * 4));
}

void Sound_update(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x0283DD48, void, self, object, frames);
    FrameLocal<Bytes<12>> result(8);
    u32 control = load<u32>(object + 0x10), position = self + 0xF8;
    getVec(self, position, 0x10172F6C);
    if (load<u8>(control + 0x74)) {
        call<void>(0x028E8F64, control + 0x98, position, result.a);
        copyWords(position, result.a, 3);
    }
}
VERIFY(0x0283DD48, Sound_update);

// GHS unsigned conversions: f32 paths subtract at single precision, f64 paths at double precision.
static u32 duration32(f32 value) {
    if (value < 2147483648.0f) return static_cast<u32>(ftoi(value));
    return static_cast<u32>(ftoi(value - 2147483648.0f)) + 0x80000000u;
}
static u32 duration64(f64 value) {
    if (value < 2147483648.0) return static_cast<u32>(ftoi(value));
    return static_cast<u32>(ftoi(value - 2147483648.0)) + 0x80000000u;
}


void Sound_beginFade(u32 self, f32 value, u32 adaptor) {
    WWHD_FUNC(0x0283DE84, void, self, value, adaptor);
    u32 id = load<u32>(adaptor + 0xF0);
    u32 sound = load<u32>(adaptor + 0xEC);
    if (id & 0xC0000000) {
        if (sound) call<void>(0x0280B160, sound, duration32(value));
    } else {
        if (sound && (id & 0xC0000C00)) call<void>(0x0280B1D8, sound, u32(0));
        u32 fade = duration32(value);
        u32 position = load<u32>(adaptor + 0xF4);
        u32 basic = load<u32>(adaptor + 0xE8);
        call<void>(0x02802848, basic, id, adaptor + 0xEC, position, fade, u32(0), u32(4));
    }
}
VERIFY(0x0283DE84, Sound_beginFade);

void Sound_endFade(u32 self, f32 value, u32 adaptor) {
    WWHD_FUNC(0x0283E020, void, self, value, adaptor);
    u32 sound = load<u32>(adaptor + 0xEC);
    if (sound) call<void>(0x0280B1D8, sound, duration32(value));
}
VERIFY(0x0283E020, Sound_endFade);

void Particle_update(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x0283EA48, void, self, object, frames);
    u32 duration = load<u32>(self + 0x1B8);
    if (!duration) return;
    u32 elapsed = load<u32>(self + 0x1BC);
    if (elapsed >= duration) return;
    duration = load<u32>(self + 0x1B8);
    elapsed += frames;
    store<u32>(self + 0x1BC, elapsed);
    if (elapsed < duration) return;
    if (load<u32>(self + 0x1B4) != 2) store<u32>(self + 0x1B4, 0);
    store<u32>(self + 0x1BC, 0);
    store<u32>(self + 0x1B8, 0);
}
VERIFY(0x0283EA48, Particle_update);

void Particle_beginFade(u32 self, f32 value, u32 adaptor) {
    WWHD_FUNC(0x0283EB98, void, self, value, adaptor);
    FrameLocal<Bytes<12>> position(8);
    u32 emitter = load<u32>(adaptor + 0x1A0);
    u32 id = load<u32>(adaptor + 0x1B0);
    if (emitter) call<void>(0x0282215C, load<u32>(adaptor + 0x19C), emitter, u32(0));
    if (!load<u32>(0x101FE03C)) {
        store<u32>(0x101FE03C, 1);
        call<void>(0xC000A848, u32(0x101FE040), u32(0x1017344C), u32(12));
    }
    u32 x = load<u32>(0x101FE040);
    u32 z = load<u32>(0x101FE048);
    store<u32>(position.a, x);
    u32 y = load<u32>(0x101FE044);
    u32 manager = load<u32>(adaptor + 0x19C);
    store<u32>(position.a + 8, z);
    store<u32>(position.a + 4, y);
    emitter = call<u32>(0x02821448, manager, position.a, u32(id & 0xFFFF), id >> 24, (id >> 16) & 0xFF, adaptor + 0x1A4, u32(0));
    store<u32>(adaptor + 0x1A0, emitter);
    if (emitter) {
        store<u32>(emitter + 0x254, load<u32>(emitter + 0x254) | 0x40);
        store<u32>(adaptor + 0x1B4, 1);
        store<u32>(adaptor + 0x1B8, duration32(value));
        store<u32>(adaptor + 0x1BC, 0);
    }
}
VERIFY(0x0283EB98, Particle_beginFade);

void Particle_endFade(u32 self, f32 value, u32 adaptor) {
    WWHD_FUNC(0x0283ECE8, void, self, value, adaptor);
    if (!load<u32>(adaptor + 0x1A0)) return;
    if (load<u32>(adaptor + 0x1B4) == 1 && load<u32>(adaptor + 0x1BC)) {
        u32 duration = load<u32>(adaptor + 0x1B8);
        u32 elapsed = load<u32>(adaptor + 0x1BC);
        f64 ratio = static_cast<f64>(duration) / static_cast<f64>(elapsed);
        f64 total = static_cast<f64>(value) * ratio;
        store<u32>(adaptor + 0x1B4, 2);
        // The second conversion uses the saved ratio and original float input.
        u32 totalFrames = duration64(total);
        f64 remaining = static_cast<f64>(value) * (ratio - 1.0);
        store<u32>(adaptor + 0x1B8, totalFrames);
        store<u32>(adaptor + 0x1BC, duration64(remaining));
    } else {
        store<u32>(adaptor + 0x1B4, 2);
        store<u32>(adaptor + 0x1B8, duration32(value));
        store<u32>(adaptor + 0x1BC, 0);
    }
}
VERIFY(0x0283ECE8, Particle_endFade);

void Actor_update(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x0283FF60, void, self, object, frames);
    FrameLocal<Bytes<0x40>> storage(8);
    u32 f = storage.a - 8;
    u32 control = load<u32>(object + 0x10);
    getVec(self, f + 0x20, 0x10172EB8);
    getVec(self, f + 0x2C, 0x10172EC4);
    getVec(self, f + 0x38, 0x10172ED0);
    u32 sr = f + 0x20;
    if (load<u8>(control + 0x74)) {
        call<void>(0x028E8F64, control + 0x98, f + 0x20, f + 8);
        store<u32>(f + 0x14, load<u32>(f + 0x2C));
        f32 y = load<f32>(f + 0x30);
        f32 yaw = load<f32>(control + 0x90);
        u32 actor = load<u32>(self + 0x128);
        f32 result = y + yaw;
        store<u32>(f + 0x1C, load<u32>(f + 0x34));
        store<f32>(f + 0x18, result);
        call_ptr<void>(vfn(actor, 0x74), actor, f + 8);
        sr = f + 8;
    } else {
        u32 actor = load<u32>(self + 0x128);
        call_ptr<void>(vfn(actor, 0x74), actor, sr);
    }
    u32 actor = load<u32>(self + 0x128);
    call_ptr<void>(vfn(actor, 0x94), actor, sr + 12);
    actor = load<u32>(self + 0x128);
    call_ptr<void>(vfn(actor, 0x84), actor, f + 0x38);
}
VERIFY(0x0283FF60, Actor_update);

void Ambient_update(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x02840AC0, void, self, object, frames);
    FrameLocal<Bytes<8>> color(8);
    getColor(self, color.a, 0x10172E68);
    u32 target = load<u32>(self + 0x60);
    u32 fn = vfn(target, 0x74);
    store<u32>(color.a + 4, load<u32>(color.a));
    call_ptr<void>(fn, target, color.a + 4);
}
VERIFY(0x02840AC0, Ambient_update);

void Camera_update(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x02840F24, void, self, object, frames);
    FrameLocal<Bytes<0x30>> storage(8);
    u32 f = storage.a - 8;
    u32 control = load<u32>(object + 0x10);
    getVec(self, f + 8, 0x10172EE8);
    getVec(self, f + 0x14, 0x10172EF4);
    u32 vectors = f + 8;
    if (load<u8>(control + 0x74)) {
        call<void>(0x028E8F64, control + 0x98, f + 8, f + 0x20);
        call<void>(0x028E8F64, control + 0x98, f + 0x14, f + 0x2C);
        vectors = f + 0x20;
    }
    u32 target = load<u32>(self + 0xEC);
    call_ptr<void>(vfn(target, 0xE4), target, vectors);
    target = load<u32>(self + 0xEC);
    call_ptr<void>(vfn(target, 0x104), target, vectors + 12);
}
VERIFY(0x02840F24, Camera_update);

void Fog_update(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x02841678, void, self, object, frames);
    FrameLocal<Bytes<8>> color(8);
    getColor(self, color.a, 0x10172E78);
    u32 target = load<u32>(self + 0x88);
    u32 fn = vfn(target, 0xA4);
    store<u32>(color.a + 4, load<u32>(color.a));
    call_ptr<void>(fn, target, color.a + 4);
}
VERIFY(0x02841678, Fog_update);

void Light_update(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x02841D7C, void, self, object, frames);
    FrameLocal<Bytes<0x48>> storage(8);
    u32 f = storage.a - 8;
    u32 control = load<u32>(object + 0x10);
    getColor(self, f + 8, 0x10172E88);
    u32 target = load<u32>(self + 0x114);
    u32 fn = vfn(target, 0x94);
    store<u32>(f + 0x24, load<u32>(f + 8));
    call_ptr<void>(fn, target, f + 0x24);
    getVec(self, f + 0xC, 0x10172F18);
    u32 mode = load<u32>(self + 0x118);
    if (mode == 1) {
        u32 values = load<u32>(self);
        f32 radians = load<f32>(0x10173990);
        f32 pitch = load<f32>(values + 0xDC) * radians;
        f32 yaw = load<f32>(values + 0xC8);
        f64 cp = call<f64>(0x028F4BE0, pitch);
        f64 sp = call<f64>(0x028F43F8, pitch);
        f32 yawRad = yaw * radians;
        f64 sy = call<f64>(0x028F43F8, yawRad);
        store<f32>(f + 0x18, static_cast<f32>(cp * sy));
        store<f32>(f + 0x1C, static_cast<f32>(sp));
        f64 cy = call<f64>(0x028F4BE0, yawRad);
        store<f32>(f + 0x20, static_cast<f32>(cp * cy));
    } else if (mode == 2) {
        getVec(self, f + 0x28, 0x10172F24);
        call<void>(0x028E8DAC, f + 0x28, f + 0xC, f + 0x18);
    }
    u32 vectors = f + 0xC;
    if (load<u8>(control + 0x74)) {
        call<void>(0x028E8F64, control + 0x98, f + 0xC, f + 0x34);
        call<void>(0x028E9044, control + 0x98, f + 0x18, f + 0x40);
        vectors = f + 0x34;
    }
    target = load<u32>(self + 0x114);
    call_ptr<void>(vfn(target, 0x84), target, vectors);
    target = load<u32>(self + 0x114);
    call_ptr<void>(vfn(target, 0xC4), target, vectors + 12);
}
VERIFY(0x02841D7C, Light_update);

void Particle_callback(u32 self, u32 emitter) {
    WWHD_FUNC(0x0283EE58, void, self, emitter);
    FrameLocal<Bytes<0xC0>> storage(8);
    u32 f = storage.a - 8;
    u32 status = load<u32>(emitter + 0x254);
    if ((status & 8) && load<u32>(emitter + 0x1B4) + load<u32>(emitter + 0x1C0) == 0) {
        store<u32>(load<u32>(self + 4) + 0x1B4, 0);
        store<u32>(load<u32>(self + 4) + 0x1B8, 0);
        store<u32>(load<u32>(self + 4) + 0x1BC, 0);
        u32 adaptor = load<u32>(self + 4);
        call<void>(0x0282215C, load<u32>(adaptor + 0x19C), emitter, u32(0));
        store<u32>(load<u32>(self + 4) + 0x1A0, 0);
        return;
    }
    u32 adaptor = load<u32>(self + 4);
    u32 duration = load<u32>(adaptor + 0x1B8);
    u32 mode = load<u32>(adaptor + 0x1B4);
    f64 alpha = 1.0;
    if (mode == 1 && duration) {
        alpha = static_cast<f64>(load<u32>(adaptor + 0x1BC)) / static_cast<f64>(duration);
    } else if (mode == 2) {
        if (!duration) {
            status |= 1;
            store<u32>(emitter + 0x254, status);
            store<u32>(emitter + 0x5C, 1);
            alpha = 0.0;
        } else {
            u32 elapsed = load<u32>(adaptor + 0x1BC);
            alpha = static_cast<f64>(duration - elapsed) / static_cast<f64>(duration);
        }
    }
    u32 object = load<u32>(self + 8);
    u32 control = load<u32>(object + 0x10);
    store<u32>(emitter + 0x254, status | 4);
    getVec(load<u32>(self + 4), f + 0x20, 0x10172F30);
    getVec(load<u32>(self + 4), f + 0x2C, 0x10172F3C);
    getVec(load<u32>(self + 4), f + 0x14, 0x10172F48);
    adaptor = load<u32>(self + 4);
    if (!load<u8>(adaptor + 0x1CC)) {
        u32 sr = f + 0x20;
        if (load<u8>(control + 0x74)) {
            call<void>(0x028E8F64, control + 0x98, f + 0x20, f + 0x38);
            store<u32>(f + 0x44, load<u32>(f + 0x2C));
            store<u32>(f + 0x4C, load<u32>(f + 0x34));
            f32 yaw = load<f32>(control + 0x90);
            f32 rotation = load<f32>(f + 0x30);
            store<f32>(f + 0x48, rotation + yaw);
            sr = f + 0x38;
        }
        u8 group = load<u8>(emitter + 0x262);
        copyWords(emitter + 0x22C, sr, 3);
        if (group >= 7) store<f32>(emitter + 0x230, -load<f32>(emitter + 0x230));
        f64 rx = static_cast<f64>(load<f32>(sr + 12)) / 360.0;
        f64 ry = static_cast<f64>(load<f32>(sr + 16)) / 360.0;
        s16 ax = static_cast<s16>(ftoi(rx * 65536.0));
        s16 ay = static_cast<s16>(ftoi(ry * 65536.0));
        f64 rz = static_cast<f64>(load<f32>(sr + 20)) / 360.0;
        s16 az = static_cast<s16>(ftoi(rz * 65536.0));
        call<void>(0x028245AC, ax, ay, az, emitter + 0x1F0);
        u32 sx = load<u32>(f + 0x14), sy = load<u32>(f + 0x18);
        store<u32>(emitter + 0x220, sx);
        u32 sz = load<u32>(f + 0x1C);
        store<u32>(emitter + 0x224, sy);
        store<u32>(emitter + 0x228, sz);
        store<u32>(emitter + 0x238, sx);
        store<u32>(emitter + 0x23C, sy);
        store<u32>(emitter + 0x240, sz);
    } else {
        u32 parent = load<u32>(adaptor + 0x1C4);
        if (!parent) return;
        u32 fn = vfn(parent, 0x64);
        u32 node = load<u32>(adaptor + 0x1C8);
        if (!call_ptr<u32>(fn, parent, node, f + 0x38)) return;
        call<void>(0x02839C28, f + 0x68, f + 0x14, f + 0x2C, f + 0x20);
        call<void>(0x028E9108, f + 0x38, f + 0x68, f + 0x98);
        call<void>(0x02824890, f + 0x98, emitter + 0x1F0, emitter + 0x220, emitter + 0x22C);
    }
    getColor(load<u32>(self + 4), f + 0x10, 0x10172E98);
    u8 colorAlpha = load<u8>(f + 0x13);
    u8 green = load<u8>(f + 0x11), blue = load<u8>(f + 0x12), red = load<u8>(f + 0x10);
    alpha *= static_cast<f64>(colorAlpha);
    store<u8>(emitter + 0x244, red);
    store<u8>(emitter + 0x245, green);
    store<u8>(emitter + 0x246, blue);
    u8 outAlpha = 255;
    if (alpha < 255.0) outAlpha = static_cast<u8>(ftoi(alpha));
    store<u8>(emitter + 0x247, outAlpha);
    getColor(load<u32>(self + 4), f + 0x10, 0x10172EA8);
    red = load<u8>(f + 0x10);
    blue = load<u8>(f + 0x12);
    store<u8>(emitter + 0x248, red);
    green = load<u8>(f + 0x11);
    status = load<u32>(emitter + 0x254);
    store<u8>(emitter + 0x249, green);
    store<u8>(emitter + 0x24A, blue);
    store<u32>(emitter + 0x254, status & ~4u);
}
VERIFY(0x0283EE58, Particle_callback);
}
