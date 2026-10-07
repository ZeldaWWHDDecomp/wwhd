// Local WWHD reconstruction using the CC0 TWW JPA1 template.
#include "JSystem/JParticle/JPA_field_a.h"
namespace jpa_field_a {
using namespace gabi;
static f32 f(u32 p, u32 off) { return load<f32>(p + off); }
static void put(u32 p, u32 off, f32 v) { store<f32>(p + off, v); }
static f32 mul(f32 a, f32 b) { return fmuls_ppc(a, b); }
static f32 sub(f32 a, f32 b) { return fsubs_ppc(a, b); }
static f32 add(f32 a, f32 b) { return fadds_ppc(a, b); }
static f32 squared(f32 x, f32 y, f32 z) { return fmadds(z, z, fmadds(x, x, mul(y, y))); }
static f32 root(f32 v) { return (f32)call<f64>(0x028F37F0, (f64)v); }

f32 JPABaseField_calcFadeAffect(u32 self, u32 data, f32 time) {
    WWHD_FUNC(0x02822AC0, f32, self, data, time);
    u16 flags = load<u16>(data + 0x90);
    if (((flags & 8) && time < f(data, 0x88)) ||
        ((flags & 0x10) && !(time < f(data, 0x8C)))) return 0.0f;
    if ((flags & 0x40) && !(time < f(data, 0x84)))
        return mul(sub(f(data, 0x8C), time), f(data, 0x48));
    if ((flags & 0x20) && time < f(data, 0x80))
        return mul(sub(time, f(data, 0x88)), f(data, 0x4C));
    return 1.0f;
}
VERIFY(0x02822AC0, JPABaseField_calcFadeAffect);

void JPABaseField_calcVel(u32 self, u32 data, u32 particle) {
    WWHD_FUNC(0x02822B54, void, self, data, particle);
    u32 flags = load<u32>(particle + 0xCC);
    f32 x = f(data, 0x14), z = f(data, 0x1C), y = f(data, 0x18);
    if (!(flags & 4)) {
        f32 fade = JPABaseField_calcFadeAffect(self, data, f(particle, 0x80));
        y = mul(y, fade); x = mul(x, fade); z = mul(z, fade);
    }
    u8 type = load<u8>(data + 0x94);
    u32 offset;
    switch (type) {
    case 0: offset = 0x64; break;
    case 1: offset = 0x40; break;
    case 2: offset = 0x58; break;
    default: return;
    }
    f32 vx = f(particle, offset), vy = f(particle, offset + 4);
    vx = add(vx, x);
    f32 vz = f(particle, offset + 8);
    vy = add(vy, y); vz = add(vz, z);
    put(particle, offset, vx); put(particle, offset + 4, vy); put(particle, offset + 8, vz);
}
VERIFY(0x02822B54, JPABaseField_calcVel);

void JPABaseField_preCalc(u32 self, u32 data) {
    WWHD_FUNC(0x02822C38, void, self, data);
    f32 distance = f(data, 0x70), fadeOut = f(data, 0x84);
    f32 distanceSq = mul(distance, distance);
    f32 outDelta = sub(f(data, 0x8C), fadeOut);
    put(data, 0x44, distanceSq);
    f32 enable = f(data, 0x88);
    f32 inDelta = sub(f(data, 0x80), enable);
    put(data, 0x48, outDelta == 0.0f ? 1.0f : 1.0f / outDelta);
    put(data, 0x4C, inDelta == 0.0f ? 1.0f : 1.0f / inDelta);
}
VERIFY(0x02822C38, JPABaseField_preCalc);

bool JPABaseField_isItinRange(u32 self, u32 data, f32 distanceSq) {
    WWHD_FUNC(0x02822CA8, bool, self, data, distanceSq);
    return distanceSq < f(data, 0x44);
}
VERIFY(0x02822CA8, JPABaseField_isItinRange);

void JPABaseField_init(u32 self, u32 data, u32 particle) {
    WWHD_FUNC(0x02824504, void, self, data, particle);
}
VERIFY(0x02824504, JPABaseField_init);

void JPAGravityField_preCalc(u32 self, u32 data) {
    WWHD_FUNC(0x02822CBC, void, self, data);
    JPABaseField_preCalc(self, data);
    if (load<u16>(data + 0x90) & 2) {
        f32 y = f(data, 0x60), mag = f(data, 0x68), x = f(data, 0x5C);
        f32 vy = mul(y, mag), vx = mul(x, mag), z = f(data, 0x64);
        put(data, 0x18, vy);
        f32 vz = mul(z, mag);
        put(data, 0x14, vx); put(data, 0x1C, vz);
    } else {
        FrameLocal<be<f32>[3]> direction(8);
        call<void>(0x028E8F64, load<u32>(0x104B5948) + 0x68, data + 0x5C, direction.get());
        f32 y = f(direction.a, 4), mag = f(data, 0x68), x = f(direction.a, 0);
        f32 vy = mul(y, mag), vx = mul(x, mag), z = f(direction.a, 8);
        put(data, 0x18, vy);
        f32 vz = mul(z, mag);
        put(data, 0x14, vx); put(data, 0x1C, vz);
    }
}
VERIFY(0x02822CBC, JPAGravityField_preCalc);

// Gravity vtable 1017109C, calc slot 101710B8: tail branch, not empty blr.
void JPAGravityField_calc(u32 self, u32 data, u32 particle) {
    WWHD_FUNC(0x02822D60, void, self, data, particle);
    JPABaseField_calcVel(self, data, particle);
}
VERIFY(0x02822D60, JPAGravityField_calc);

void JPAAirField_preCalc(u32 self, u32 data) {
    WWHD_FUNC(0x02822D64, void, self, data);
    JPABaseField_preCalc(self, data);
    if (load<u16>(data + 0x90) & 2) {
        f32 mag = f(data, 0x68), y = f(data, 0x60);
        u32 xBits = load<u32>(data + 0x5C);
        f32 vy = mul(y, mag);
        store<u32>(data + 0x38, xBits);
        u32 zBits = load<u32>(data + 0x64);
        store<u32>(data + 0x3C, load<u32>(data + 0x60));
        store<u32>(data + 0x40, zBits);
        f32 vx = mul(f(data, 0x38), mag), z = f(data, 0x40);
        put(data, 0x14, vx);
        f32 vz = mul(z, mag);
        put(data, 0x18, vy); put(data, 0x1C, vz);
    } else {
        call<void>(0x028E8F64, load<u32>(0x104B5948) + 0x68, data + 0x5C, data + 0x38);
        f32 x = f(data, 0x38), mag = f(data, 0x68), y = f(data, 0x3C);
        f32 vx = mul(x, mag), z = f(data, 0x40), vy = mul(y, mag);
        put(data, 0x14, vx);
        f32 vz = mul(z, mag);
        put(data, 0x18, vy); put(data, 0x1C, vz);
    }
    if (load<u16>(data + 0x90) & 1) {
        u16 angle = (u16)ftoi(mul(f(data, 0x74), 32768.0f));
        u32 cosine = load<u32>(0x104A44FC + ((u32)angle >> 3) * 8);
        store<u32>(data + 0x2C, cosine);
        if (load<u16>(data + 0x90) & 2) {
            u32 x = load<u32>(data + 0x50), y = load<u32>(data + 0x54);
            store<u32>(data + 0x20, x);
            u32 z = load<u32>(data + 0x58);
            store<u32>(data + 0x24, y); store<u32>(data + 0x28, z);
        } else {
            call<void>(0x028E8F64, load<u32>(0x104B5948) + 0x68, data + 0x20, data + 0x50);
        }
    }
}
VERIFY(0x02822D64, JPAAirField_preCalc);

void JPAAirField_calc(u32 self, u32 data, u32 particle) {
    WWHD_FUNC(0x02822EC0, void, self, data, particle);
    u16 flags = load<u16>(data + 0x90);
    bool apply = true;
    if (flags & 1) {
        u32 pos = particle + ((flags & 2) ? 0x28 : 0x1C);
        f32 ox = f(data, 0x20), oy = f(data, 0x24);
        f32 y = sub(f(pos, 4), oy), x = sub(f(pos, 0), ox);
        f32 z = sub(f(pos, 8), f(data, 0x28));
        f32 lengthSq = squared(x, y, z);
        if (lengthSq > 0.000003814697265625f) {
            f32 length = root(lengthSq), scale = 1.0f / length;
            x = mul(x, scale); z = mul(z, scale); y = mul(y, scale);
        }
        f32 dot = fmadds(f(data, 0x38), x, mul(f(data, 0x3C), y));
        dot = fmadds(f(data, 0x40), z, dot);
        apply = !(dot < f(data, 0x2C));
    }
    if (apply) JPABaseField_calcVel(self, data, particle);
    if (load<u16>(data + 0x90) & 4) {
        f32 y = f(particle, 0x44), x = f(particle, 0x40), z = f(particle, 0x48);
        f32 length = root(squared(x, y, z)), limit = f(data, 0x6C);
        if (length > limit) {
            f32 scale = limit / length;
            x = f(particle, 0x40); y = f(particle, 0x44);
            x = mul(x, scale); z = f(particle, 0x48); y = mul(y, scale);
            put(particle, 0x40, x); z = mul(z, scale);
            put(particle, 0x44, y); put(particle, 0x48, z);
        }
    }
}
VERIFY(0x02822EC0, JPAAirField_calc);

void JPAMagnetField_preCalc(u32 self, u32 data) {
    WWHD_FUNC(0x0282308C, void, self, data);
    JPABaseField_preCalc(self, data);
    u16 flags = load<u16>(data + 0x90);
    u32 xBits = load<u32>(data + 0x50);
    if (flags & 2) {
        u32 y = load<u32>(data + 0x54), z = load<u32>(data + 0x58);
        store<u32>(data + 0x24, y); store<u32>(data + 0x28, z); store<u32>(data + 0x20, xBits);
    } else {
        u32 info = load<u32>(0x104B5948);
        put(data, 0x20, sub(f(data, 0x50), f(info, 0xD4)));
        put(data, 0x24, sub(f(data, 0x54), f(info, 0xD8)));
        put(data, 0x28, sub(f(data, 0x58), f(info, 0xDC)));
        call<void>(0x028E8F64, load<u32>(0x104B5948) + 0x68, data + 0x20, data + 0x20);
    }
}
VERIFY(0x0282308C, JPAMagnetField_preCalc);

void JPAMagnetField_calc(u32 self, u32 data, u32 particle) {
    WWHD_FUNC(0x0282312C, void, self, data, particle);
    u32 pos = particle + ((load<u16>(data + 0x90) & 2) ? 0x28 : 0x1C);
    f32 x = sub(f(data, 0x20), f(pos, 0)); put(data, 0x14, x);
    f32 y = sub(f(data, 0x24), f(pos, 4));
    f32 z0 = f(data, 0x28); put(data, 0x18, y);
    f32 z = sub(z0, f(pos, 8)); put(data, 0x1C, z);
    f32 lengthSq = squared(x, y, z), mag = f(data, 0x68);
    if (lengthSq > 0.000003814697265625f) {
        f32 length = root(lengthSq), scale = mul(1.0f / length, mag);
        x = f(data, 0x14); y = f(data, 0x18);
        x = mul(x, scale); z = f(data, 0x1C); y = mul(y, scale);
        put(data, 0x14, x); z = mul(z, scale);
        put(data, 0x18, y); put(data, 0x1C, z);
    }
    JPABaseField_calcVel(self, data, particle);
}
VERIFY(0x0282312C, JPAMagnetField_calc);

void JPANewtonField_preCalc(u32 self, u32 data) {
    WWHD_FUNC(0x02823280, void, self, data);
    JPABaseField_preCalc(self, data);
    u16 flags = load<u16>(data + 0x90);
    u32 x = load<u32>(data + 0x50);
    if (flags & 2) {
        store<u32>(data + 0x28, load<u32>(data + 0x58));
        f32 value = f(data, 0x74);
        u32 y = load<u32>(data + 0x54);
        f32 square = mul(value, value);
        store<u32>(data + 0x24, y); store<u32>(data + 0x20, x);
        put(data, 0x78, square);
    } else {
        u32 info = load<u32>(0x104B5948);
        put(data, 0x20, sub(f(data, 0x50), f(info, 0xD4)));
        put(data, 0x24, sub(f(data, 0x54), f(info, 0xD8)));
        put(data, 0x28, sub(f(data, 0x58), f(info, 0xDC)));
        call<void>(0x028E8F64, load<u32>(0x104B5948) + 0x68, data + 0x20, data + 0x20);
        f32 value = f(data, 0x74); put(data, 0x78, mul(value, value));
    }
}
VERIFY(0x02823280, JPANewtonField_preCalc);

void JPANewtonField_calc(u32 self, u32 data, u32 particle) {
    WWHD_FUNC(0x02823334, void, self, data, particle);
    bool global = load<u16>(data + 0x90) & 2;
    u32 pos = particle + (global ? 0x28 : 0x1C);
    f32 x = sub(f(data, 0x20), f(pos, 0));
    f32 y0 = f(data, 0x24); put(data, 0x14, x);
    f32 y = sub(y0, f(pos, 4));
    u16 flags = load<u16>(data + 0x90);
    f32 z0 = f(data, 0x28), mag = f(data, 0x68); put(data, 0x18, y);
    f32 z = sub(z0, f(pos, 8));
    f32 yy = global ? mul(y, y) : mul(f(data, 0x18), f(data, 0x18));
    put(data, 0x1C, z);
    f32 lengthSq = fmadds(z, z, fmadds(x, x, yy));
    if (!(flags & 0x100)) {
        f32 threshold = f(data, 0x78);
        mag = mul(mag, 10.0f);
        if (lengthSq > threshold) mag = mul(mag, threshold) / lengthSq;
    }
    if (lengthSq > 0.000003814697265625f) {
        f32 length = root(lengthSq), scale = mul(1.0f / length, mag);
        x = f(data, 0x14); y = f(data, 0x18);
        x = mul(x, scale); z = f(data, 0x1C); y = mul(y, scale);
        put(data, 0x14, x); z = mul(z, scale);
        put(data, 0x18, y); put(data, 0x1C, z);
    }
    JPABaseField_calcVel(self, data, particle);
}
VERIFY(0x02823334, JPANewtonField_calc);
} // namespace jpa_field_a
