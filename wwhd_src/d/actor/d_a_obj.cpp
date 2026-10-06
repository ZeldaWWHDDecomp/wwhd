/* WWHD source-only draft. Utility ownership: 02311AB8..<02312FB8.
 * Based on tww/src/d/d_a_obj.cpp; no compiler/runtime verification claimed.
 * The neighboring Nzg termination and following Adnno entry are excluded.
 * The final entry initializes this translation unit's HD header statics.
 */
#include "bindings.h"
#include "d/actor/d_a_obj.h"
#include <cmath>

namespace {
template<class T> struct ObjLocal {
    gabi::Local<T> payload;
    struct Linkage { u32 words[8]; };
    gabi::Local<Linkage> linkage;
    T* get() const { return gabi::at<T>(payload.a); }
};
inline f32 objFloat(u32 a) { return gabi::load<f32>(a); }
inline void objZero(cXyz* out) {
    const f32 x = objFloat(0x101FFBA8), y = objFloat(0x101FFBAC), z = objFloat(0x101FFBB0);
    out->x = x; out->z = z; out->y = y;
}
inline void objIdentity(daObj::ObjQuaternion* out, u32 address) {
    for (u32 i = 0; i < 4; ++i)
        gabi::store<u32>(gabi::ea(out) + i * 4, gabi::load<u32>(address + i * 4));
}
static void quatFromBase(daObj::ObjQuaternion* out, const cXyz* input, u32 baseAddress, u32 identityAddress) {
    ObjLocal<cXyz> normalized, cross;
    const f32 x = input->x, y = input->y, z = input->z;
    normalized.get()->x = x; normalized.get()->z = z; normalized.get()->y = y;
    if (!gabi::call<s32>(0x0201B47C, normalized.get())) { objIdentity(out, identityAddress); return; }
    auto* base = gabi::at<cXyz>(baseAddress);
    const f32 dot = gabi::call<f32>(0x028E8F44, base, normalized.get());
    const f32 one = objFloat(0x10024610);
    const f32 sum = gabi::fadds_ppc(dot, one);
    const f32 root = gabi::call<f32>(0x028F4384, gabi::fadds_ppc(sum, sum));
    gabi::call<void>(0x0201B080, base, cross.get(), normalized.get());
    if (!(std::fabs(root) > objFloat(0x100246D8))) { objIdentity(out, identityAddress); return; }
    const f32 reciprocal = f32(f64(one) / f64(root));
    const f32 cx = cross.get()->x, cy = cross.get()->y, cz = cross.get()->z;
    out->x = gabi::fmuls_ppc(cx, reciprocal);
    out->y = gabi::fmuls_ppc(cy, reciprocal);
    out->z = gabi::fmuls_ppc(cz, reciprocal);
    out->w = gabi::fmuls_ppc(root, objFloat(0x10024638));
}
static void hitDirection(cXyz* out, const dCcD_Cyl* cylinder) {
    const u32 c = gabi::ea(cylinder);
    gabi::call<void>(0x0201AE48, gabi::at<cXyz>(c + 0xC0), out, objFloat(0x10024614));
    const f32 square = gabi::call<f32>(0x028E8DD0, out);
    if (square < objFloat(0x100246F8)) {
        u32 table = gabi::load<u32>(c + 0x114);
        auto* center = gabi::call_ptr<cXyz*>(gabi::load<u32>(table + 0x8C), gabi::at<void>(c + 0xF8));
        out->y = objFloat(0x1002461C);
        out->x = gabi::fsubs_ppc(objFloat(c + 0xCC), f32(center->x));
        table = gabi::load<u32>(c + 0x114);
        center = gabi::call_ptr<cXyz*>(gabi::load<u32>(table + 0x8C), gabi::at<void>(c + 0xF8));
        out->z = gabi::fsubs_ppc(objFloat(c + 0xD4), f32(center->z));
    }
}

static void posMoveF_resist_acc(cXyz* out, fopAc_ac_c* actor, const cXyz* stream,
                               f32 linear, f32 quadratic) {
    WWHD_FUNC(0x02311AB8, void, out, actor, stream, linear, quadratic);
    ObjLocal<cXyz> delta, resistance;
    gabi::call<void>(0x0201ADE0, gabi::at<cXyz>(gabi::ea(actor) + 0x33C), delta.get(), stream);
    const f32 x = delta.get()->x, y = delta.get()->y, z = delta.get()->z;
    const f32 qx = gabi::fmuls_ppc(gabi::fmuls_ppc(std::fabs(x), x), quadratic);
    const f32 qy = gabi::fmuls_ppc(gabi::fmuls_ppc(std::fabs(y), y), quadratic);
    const f32 qz = gabi::fmuls_ppc(gabi::fmuls_ppc(std::fabs(z), z), quadratic);
    resistance.get()->x = gabi::fmadds(x, linear, qx);
    resistance.get()->y = gabi::fmadds(y, linear, qy);
    resistance.get()->z = gabi::fmadds(z, linear, qz);
    gabi::call<void>(0x028E8E64, resistance.get(), resistance.get(), objFloat(0x10024614));
    const f32 rx = resistance.get()->x, ry = resistance.get()->y, rz = resistance.get()->z;
    out->x = rx; out->z = rz; out->y = ry;
}
VERIFY(0x02311AB8, posMoveF_resist_acc);

static void posMoveF_grade_acc(cXyz* out, fopAc_ac_c* actor, const cXyz* normal,
                              f32 friction, f32 noGradeCos,
                              const cXyz* acceleration, const cXyz* extra) {
    WWHD_FUNC(0x02311B94, void, out, actor, normal, friction, noGradeCos, acceleration, extra);
    objZero(out);
    if (!normal) return;
    ObjLocal<cXyz> total, cross, scaled;
    total.get()->x = acceleration->x;
    total.get()->y = gabi::fadds_ppc(acceleration->y, objFloat(gabi::ea(actor) + 0x374));
    total.get()->z = acceleration->z;
    if (extra) gabi::call<void>(0x028E8D88, total.get(), extra, total.get());
    const f32 dot = gabi::call<f32>(0x028E8F44, total.get(), normal);
    if (!(dot < objFloat(0x1002461C))) return;
    if (!(f32(normal->y) > noGradeCos)) {
        gabi::call<void>(0x02017B6C, normal, total.get(), cross.get());
        gabi::call<void>(0x028E8D88, out, cross.get(), out);
    }
    gabi::call<void>(0x02017B6C, normal, gabi::at<cXyz>(gabi::ea(actor) + 0x33C), cross.get());
    gabi::call<void>(0x0201AE48, cross.get(), scaled.get(), friction);
    gabi::call<void>(0x028E8DAC, out, scaled.get(), out);
}
VERIFY(0x02311B94, posMoveF_grade_acc);
}

namespace daObj {
void HitSeStart(const cXyz* position, s32 room, const dCcD_GObjInf* object, u32 parameter) {
    WWHD_FUNC(0x023129C4, void, position, room, object, parameter);
    const s32 hitSound = gabi::call<s32>(0x0251638C, object);
    const u32 hitActor = gabi::call<u32>(0x02515BBC, gabi::at<void>(gabi::ea(object) + 0x94));
    const u32 hitObject = gabi::call<u32>(0x02516300, object);
    // HD returns without playing when any of these three values is absent.
    if (!hitSound || !hitActor || !hitObject) return;
    const u32 type = gabi::load<u32>(hitObject + 0x10);
    u32 sound;
    if (type & 0x10000u) sound = 0x2855;
    else if (type & 0x1C4000u) sound = 0x287A;
    else if (type & 0x8000u) sound = 0x287B;
    else {
        bool masterSword = false;
        if (gabi::load<s16>(hitActor + 0xE) == 0xA8) {
            const u32 equipment = gabi::load<u32>(0x101F84DC);
            const u8 sword = gabi::load<u8>(equipment + 0x2E);
            masterSword = sword == 57 || sword == 58 || sword == 62;
        }
        if (hitSound == 4) sound = 0x2833;
        else if (hitSound == 5) sound = 0x2834;
        else sound = masterSword ? 0x2805 : 0x2803;
    }
    const s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call<void>(0x025E1A40, sound, position, parameter & 0xFFu, reverb);
}
VERIFY(0x023129C4, HitSeStart);

void make_land_effect(fopAc_ac_c* actor, dBgS_GndChk* ground, f32 size) {
    WWHD_FUNC(0x02311CD8, void, actor, ground, size);
    const u32 play = gabi::call<u32>(0x025200D4);
    auto* poly = gabi::at<cBgS_PolyInfo>(ground ? gabi::ea(ground) + 0x14 : 0);
    const u32 attribute = gabi::call<u32>(0x024EF0F4, gabi::at<void>(play + 0x12A0), poly);
    const f32 spread = objFloat(0x10024610), speed = objFloat(0x10024620);
    if (attribute == 5 || attribute == 10 || attribute == 15 || attribute == 27) return;
    ObjLocal<cXyz> scale;
    auto* position = gabi::at<cXyz>(gabi::ea(actor) + 0x314);
    if (attribute != 4 && attribute != 19) {
        scale.get()->x = size; scale.get()->z = size; scale.get()->y = size;
        gabi::call<void>(0x028E8E64, scale.get(), scale.get(), objFloat(0x10024634));
        gabi::call<void>(0x025D5834, 0x1D3u, 3u, position, -1, nullptr, scale.get(), -1, nullptr);
        return;
    }
    const f32 particleSize = attribute == 19 ? gabi::fmuls_ppc(size, objFloat(0x10024624)) : size;
    scale.get()->x = particleSize; scale.get()->y = particleSize; scale.get()->z = particleSize;
    const u32 particlePlay = gabi::call<u32>(0x025200D4);
    const u32 controller = gabi::load<u32>(particlePlay + 0x5AB0);
    const u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(controller), 0u,
        attribute == 19 ? 0x23u : 0x24u, position, nullptr, scale.get(), 0xFFu, nullptr,
        -1, nullptr, nullptr, 0u);
    if (!emitter) return;
    gabi::store<f32>(emitter + 0x58, spread);
    gabi::store<u32>(emitter + 0x5C, 1);
    if (attribute == 19) {
        gabi::store<f32>(emitter + 0x34, speed);
        gabi::store<f32>(emitter + 0x70, objFloat(0x10024628));
    } else {
        gabi::store<f32>(emitter + 0x70, speed);
        gabi::store<u16>(emitter + 0x60, 20);
        const f32 zero = objFloat(0x1002461C);
        gabi::store<f32>(emitter + 0x14, zero);
        gabi::store<f32>(emitter + 0x18, objFloat(0x10024630));
        gabi::store<f32>(emitter + 0x1C, zero);
        gabi::store<f32>(emitter + 0x34, objFloat(0x1002462C));
    }
}
VERIFY(0x02311CD8, make_land_effect);

void HitEff_kikuzu(const fopAc_ac_c* actor, const dCcD_Cyl* cylinder) {
    WWHD_FUNC(0x02312C8C, void, actor, cylinder);
    if (!gabi::call<s32>(0x0251638C, cylinder)) return;
    ObjLocal<cXyz> direction;
    hitDirection(direction.get(), cylinder);
    HitEff_sub_kikuzu(gabi::at<cXyz>(gabi::ea(cylinder) + 0xCC), direction.get(),
                      gabi::at<dKy_tevstr_c>(gabi::ea(actor) + 0x110));
}
VERIFY(0x02312C8C, HitEff_kikuzu);
void HitEff_hibana(const fopAc_ac_c* actor, const dCcD_Cyl* cylinder) {
    WWHD_FUNC(0x02312E54, void, actor, cylinder);
    if (!gabi::call<s32>(0x0251638C, cylinder)) return;
    ObjLocal<cXyz> direction;
    hitDirection(direction.get(), cylinder);
    HitEff_hibana(gabi::at<cXyz>(gabi::ea(cylinder) + 0xCC), direction.get());
}
VERIFY(0x02312E54, static_cast<void(*)(const fopAc_ac_c*, const dCcD_Cyl*)>(HitEff_hibana));

void HitEff_sub_kikuzu(const cXyz* position, const cXyz* direction, const dKy_tevstr_c* tev) {
    WWHD_FUNC(0x02312B88, void, position, direction, tev);
    ObjLocal<cXyz> horizontal;
    ObjLocal<csXyz> angle;
    horizontal.get()->x = direction->x;
    horizontal.get()->z = direction->z;
    horizontal.get()->y = objFloat(0x1002461C);
    const f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
    const f32 radius = gabi::call<f32>(0x028F4384, square);
    angle.get()->x = gabi::call<s16>(0x020195B0, f32(direction->y), radius);
    angle.get()->y = gabi::call<s16>(0x020195B0, f32(direction->x), f32(direction->z));
    angle.get()->z = 0;
    const u32 play = gabi::call<u32>(0x025200D4);
    const u32 controller = gabi::load<u32>(play + 0x5AB0);
    auto* color = gabi::at<void>(gabi::ea(tev) + 0x98);
    const u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(controller), 0u, 0x2Bu,
        position, angle.get(), nullptr, 0xFFu, nullptr, -1, color, color, 0u);
    if (emitter) {
        gabi::store<f32>(emitter + 0x34, objFloat(0x100246EC));
        gabi::store<f32>(emitter + 0x58, objFloat(0x100246F0));
        gabi::store<f32>(emitter + 0x7C, objFloat(0x100246F4));
        gabi::store<u32>(emitter + 0x5C, 1);
    }
}
VERIFY(0x02312B88, HitEff_sub_kikuzu);

void HitEff_hibana(const cXyz* position, const cXyz* direction) {
    WWHD_FUNC(0x02312D6C, void, position, direction);
    ObjLocal<cXyz> horizontal;
    ObjLocal<csXyz> angle;
    horizontal.get()->x = direction->x;
    horizontal.get()->y = objFloat(0x1002461C);
    horizontal.get()->z = direction->z;
    const f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
    const f32 radius = gabi::call<f32>(0x028F4384, square);
    const s16 pitch = gabi::call<s16>(0x020195B0, f32(direction->y), radius);
    angle.get()->x = s16(u16(pitch) + 0x4000u);
    angle.get()->y = gabi::call<s16>(0x020195B0, f32(direction->x), f32(direction->z));
    angle.get()->z = 0;
    const u32 play = gabi::call<u32>(0x025200D4);
    const u32 controller = gabi::load<u32>(play + 0x5AB0);
    const u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(controller), 0u, 0x2Cu,
        position, angle.get(), nullptr, 0xFFu, nullptr, -1, nullptr, nullptr, 0u);
    if (emitter) {
        gabi::store<u32>(emitter + 0x5C, 1);
        const f32 rate = objFloat(0x10024620);
        gabi::store<f32>(emitter + 0x34, rate);
        gabi::store<f32>(emitter + 0x6C, rate);
        gabi::store<f32>(emitter + 0x70, rate);
    }
}
VERIFY(0x02312D6C, static_cast<void(*)(const cXyz*, const cXyz*)>(HitEff_hibana));

void quat_rotVec(ObjQuaternion* out, const cXyz* from, const cXyz* to) {
    WWHD_FUNC(0x023127F8, void, out, from, to);
    ObjLocal<cXyz> first, second, cross;
    const f32 tx = to->x, ty = to->y, tz = to->z;
    second.get()->x = tx; second.get()->y = ty; second.get()->z = tz;
    const f32 fx = from->x, fy = from->y, fz = from->z;
    first.get()->z = fz; first.get()->x = fx; first.get()->y = fy;
    if (!gabi::call<s32>(0x0201B47C, second.get()) ||
        !gabi::call<s32>(0x0201B47C, first.get())) {
        objIdentity(out, 0x101E9C38); return;
    }
    const f32 dot = gabi::call<f32>(0x028E8F44, first.get(), second.get());
    const f32 one = objFloat(0x10024610);
    const f32 sum = gabi::fadds_ppc(dot, one);
    const f32 root = gabi::call<f32>(0x028F4384, gabi::fadds_ppc(sum, sum));
    gabi::call<void>(0x0201B080, first.get(), cross.get(), second.get());
    if (!(std::fabs(root) > objFloat(0x100246D8))) {
        objIdentity(out, 0x101E9C38); return;
    }
    const f32 reciprocal = f32(f64(one) / f64(root));
    const f32 x = cross.get()->x, y = cross.get()->y, z = cross.get()->z;
    out->x = gabi::fmuls_ppc(x, reciprocal);
    out->y = gabi::fmuls_ppc(y, reciprocal);
    out->z = gabi::fmuls_ppc(z, reciprocal);
    out->w = gabi::fmuls_ppc(root, objFloat(0x10024638));
}
VERIFY(0x023127F8, quat_rotVec);

void quat_rotBaseY(ObjQuaternion* out, const cXyz* input) {
    WWHD_FUNC(0x023123D8, void, out, input);
    ObjLocal<cXyz> cross, normalized;
    const f32 lengthSquared = gabi::call<f32>(0x028E8DD0, input);
    const f32 epsilon = objFloat(0x100246C4);
    if (!(lengthSquared > epsilon)) { objIdentity(out, 0x100246C8); return; }
    auto* base = gabi::at<cXyz>(0x101FFBC0);
    gabi::call<void>(0x0201B080, base, cross.get(), input);
    const f32 crossSquared = gabi::call<f32>(0x028E8DD0, cross.get());
    if (!(crossSquared > epsilon)) { objIdentity(out, 0x100246C8); return; }
    gabi::call<void>(0x0201AEAC, input, normalized.get(), lengthSquared);
    const f32 dot = gabi::call<f32>(0x028E8F44, base, normalized.get());
    const f32 angle = gabi::call<f32>(0x028F4FAC, dot);
    const f32 crossLength = gabi::call<f32>(0x028F4384, crossSquared);
    const f32 reciprocal = f32(f64(objFloat(0x10024610)) / f64(crossLength));
    gabi::call<void>(0x028E8E64, cross.get(), cross.get(), reciprocal);
    gabi::call<void>(0x028E9B14, out, cross.get(), angle);
}
VERIFY(0x023123D8, quat_rotBaseY);

void quat_rotBaseY2(ObjQuaternion* out, const cXyz* input) {
    WWHD_FUNC(0x02312548, void, out, input);
    quatFromBase(out, input, 0x101FFBC0, 0x100246DC);
}
VERIFY(0x02312548, quat_rotBaseY2);
void quat_rotBaseZ(ObjQuaternion* out, const cXyz* input) {
    WWHD_FUNC(0x023126A0, void, out, input);
    quatFromBase(out, input, 0x101FFBCC, 0x101E9C38);
}
VERIFY(0x023126A0, quat_rotBaseZ);

void posMoveF_grade(fopAc_ac_c* actor, const cXyz* addVelocity, const cXyz* stream,
                     f32 linear, f32 quadratic, const cXyz* normal,
                     f32 friction, f32 noGradeCos, const cXyz* addAcceleration) {
    WWHD_FUNC(0x023121C4, void, actor, addVelocity, stream, linear, quadratic,
              normal, friction, noGradeCos, addAcceleration);
    if (!stream)
        gabi::call<void>(0x0273AA24, gabi::at<const char>(0x1002465C), 0x118,
                         gabi::at<const char>(0x1002464C));
    ObjLocal<cXyz> resistance, grade;
    posMoveF_resist_acc(resistance.get(), actor, stream, linear, quadratic);
    const f32 zero = objFloat(0x1002461C), one = objFloat(0x10024610);
    if (friction < zero || !(friction < one) || noGradeCos < zero || noGradeCos > one)
        gabi::call<void>(0x0273AA24, gabi::at<const char>(0x1002465C), 0x120,
                         gabi::at<const char>(0x10024668));
    posMoveF_grade_acc(grade.get(), actor, normal, friction, noGradeCos,
                      resistance.get(), addAcceleration);
    const u32 a = gabi::ea(actor);
    const u16 angle = gabi::load<u16>(a + 0x322);
    const u32 trig = 0x104A44F8 + (u32(angle >> 3) * 8);
    const f32 speed = objFloat(a + 0x370);
    f32 x = gabi::fadds_ppc(gabi::fmadds(speed, objFloat(trig), resistance.get()->x), grade.get()->x);
    f32 y = gabi::fadds_ppc(gabi::fadds_ppc(gabi::fadds_ppc(objFloat(a + 0x340), objFloat(a + 0x374)),
                                         resistance.get()->y), grade.get()->y);
    f32 z = gabi::fadds_ppc(gabi::fmadds(speed, objFloat(trig + 4), resistance.get()->z), grade.get()->z);
    const f32 maxFall = objFloat(a + 0x378);
    if (addAcceleration) {
        x = gabi::fadds_ppc(x, addAcceleration->x);
        y = gabi::fadds_ppc(y, addAcceleration->y);
        z = gabi::fadds_ppc(z, addAcceleration->z);
    }
    if (y < maxFall) y = maxFall;
    gabi::store<f32>(a + 0x340, y);
    gabi::store<f32>(a + 0x33C, x);
    gabi::store<f32>(a + 0x344, z);
    const f32 length = gabi::call<f32>(0x028F4384, gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
    gabi::store<f32>(a + 0x370, length);
    const s16 yaw = gabi::call<s16>(0x020195B0, x, z);
    gabi::store<u16>(a + 0x322, u16(yaw));
    gabi::call<void>(0x025D6800, actor, addVelocity);
}
VERIFY(0x023121C4, posMoveF_grade);

void posMoveF_stream(fopAc_ac_c* actor, const cXyz* addVelocity, const cXyz* stream,
                      f32 linear, f32 quadratic) {
    WWHD_FUNC(0x023123C0, void, actor, addVelocity, stream, linear, quadratic);
    const f32 zero = objFloat(0x1002461C);
    posMoveF_grade(actor, addVelocity, stream, linear, quadratic, nullptr, zero, zero, nullptr);
}
VERIFY(0x023123C0, posMoveF_stream);

cXyz* get_wind_spd(fopAc_ac_c* actor, f32 factor) {
    WWHD_FUNC(0x02311F80, cXyz*, actor, factor);
    cXyz* result = gabi::at<cXyz>(0x10468EE4);
    if (!gabi::load<u32>(0x10468EFC)) {
        gabi::store<u32>(0x10468EFC, 1);
        objZero(result);
    }
    ObjLocal<cXyz> wind, savedWind, point, combined, output;
    ObjLocal<be<f32>> pointPower;
    const f32 power = gabi::call<f32>(0x02578348);
    cXyz* direction = gabi::call<cXyz*>(0x0257DAA8);
    gabi::call<void>(0x0201AE48, direction, wind.get(), power);
    for (u32 i = 0; i < 3; ++i)
        gabi::store<u32>(gabi::ea(savedWind.get()) + i * 4,
                        gabi::load<u32>(gabi::ea(wind.get()) + i * 4));
    gabi::call<void>(0x0257DE68, gabi::at<cXyz>(gabi::ea(actor) + 0x314), point.get(), pointPower.get());
    gabi::call<void>(0x028E8E64, point.get(), point.get(), f32(*pointPower.get()));
    gabi::call<void>(0x0201AD78, savedWind.get(), combined.get(), point.get());
    const f32 scale = gabi::fmuls_ppc(factor, objFloat(0x10024638));
    gabi::call<void>(0x0201AE48, combined.get(), output.get(), scale);
    const u32 x = gabi::load<u32>(gabi::ea(output.get()));
    const u32 y = gabi::load<u32>(gabi::ea(output.get()) + 4);
    const u32 z = gabi::load<u32>(gabi::ea(output.get()) + 8);
    gabi::store<u32>(gabi::ea(result), x);
    gabi::store<u32>(gabi::ea(result) + 8, z);
    gabi::store<u32>(gabi::ea(result) + 4, y);
    return result;
}
VERIFY(0x02311F80, get_wind_spd);

cXyz* get_path_spd(cBgS_PolyInfo* poly, f32 factor) {
    WWHD_FUNC(0x023120B4, cXyz*, poly, factor);
    cXyz* result = gabi::at<cXyz>(0x10468EF0);
    if (!gabi::load<u32>(0x10468F00)) {
        objZero(result);
        gabi::store<u32>(0x10468F00, 1);
    }
    ObjLocal<be<s32>> pathPower;
    if (gabi::call<s32>(0x025AB184, poly, result, pathPower.get())) {
        if (gabi::call<s32>(0x0201B47C, result)) {
            const f32 power = f32(s32(*pathPower.get()));
            const f32 scale = gabi::fmuls_ppc(gabi::fmuls_ppc(factor, power), objFloat(0x10024648));
            gabi::call<void>(0x028E8E64, result, result, scale);
        } else {
            objZero(result);
        }
    }
    return result;
}
VERIFY(0x023120B4, get_path_spd);

void SetCurrentRoomNo(fopAc_ac_c* actor, dBgS_GndChk* ground) {
    WWHD_FUNC(0x02312968, void, actor, ground);
    const u32 play = gabi::call<u32>(0x025200D4);
    auto* poly = gabi::at<cBgS_PolyInfo>(ground ? gabi::ea(ground) + 0x14 : 0);
    const s32 room = gabi::call<s32>(0x024EF130, gabi::at<void>(play + 0x12A0), poly);
    if (room >= 0) gabi::store<u8>(gabi::ea(actor) + 0x326, u8(room));
}
VERIFY(0x02312968, SetCurrentRoomNo);
}

namespace {
static void obj_header_staticInitialize() {
    WWHD_FUNC(0x02312F24, void);
    gabi::store<u32>(0x10468EDC, 0);
    gabi::store<u32>(0x10468ED4, 0);
    gabi::store<u32>(0x10468EE0, 0);
    gabi::store<u32>(0x10468ED8, 0);
    gabi::call<void>(0x028F026C, gabi::at<void>(0x101C78C8));
    const f32 negativePi = objFloat(0x10024700), positivePi = objFloat(0x10024704);
    gabi::store<f32>(0x10468EC8, negativePi);
    gabi::store<f32>(0x10468ECC, positivePi);
    gabi::call<void>(0x028ED6F8, gabi::at<void>(0x10468ED0));
    gabi::call<void>(0x028F026C, gabi::at<void>(0x101C78D4));
    gabi::call<void>(0x028EAB2C, gabi::at<void>(0x10468ED1));
    gabi::call<void>(0x028F026C, gabi::at<void>(0x101C78E0));
}
VERIFY(0x02312F24, obj_header_staticInitialize);
}
