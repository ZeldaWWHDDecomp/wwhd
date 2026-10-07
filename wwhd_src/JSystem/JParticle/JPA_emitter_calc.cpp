#include "JSystem/JParticle/JPA_emitter_calc.h"
#include <climits>
#include <cstring>
namespace {
constexpr u32 info = 0x104B5730;
template <class T> T rd(u32 p) {
    return gabi::load<T>(p);
}
template <class T> void wr(u32 p, T x) {
    gabi::store<T>(p, x);
}
u32 vfn(u32 p, u32 slot) {
    return rd<u32>(rd<u32>(p) + slot);
}
u32 randomNext(JPAEmitterCalc_l *e) {
    u32 s = (u32)e->randomSeed * 0x19660du + 0x3c6ef35fu;
    e->randomSeed = s;
    return s;
}
f32 randomFloat(JPAEmitterCalc_l *e) {
    u32 bits = 0x3f800000u | (randomNext(e) >> 9);
    f32 f;
    std::memcpy(&f, &bits, 4);
    return f - 1.0f;
}
s16 randomShort(JPAEmitterCalc_l *e) {
    return (s16)(randomNext(e) >> 16);
}
f32 sinAngle(u16 angle) {
    return rd<f32>(0x104A44F8 + (angle >> 3) * 8);
}
f32 cosAngle(u16 angle) {
    return rd<f32>(0x104A44FC + (angle >> 3) * 8);
}
s32 divide(s32 a, s32 b) {
    return !b || (a == INT_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b;
}
void volumeResult(f32 x, f32 y, f32 z) {
    f32 sx = rd<f32>(info + 0xc8), sy = rd<f32>(info + 0xcc), sz = rd<f32>(info + 0xd0);
    wr<f32>(info + 0x134, x);
    wr<f32>(info + 0x138, y);
    wr<f32>(info + 0x13c, z);
    wr<f32>(info + 0x140, gabi::fmuls_ppc(x, sx));
    wr<f32>(info + 0x144, gabi::fmuls_ppc(y, sy));
    wr<f32>(info + 0x148, gabi::fmuls_ppc(z, sz));
    wr<f32>(info + 0x14c, x);
    wr<f32>(info + 0x150, 0.0f);
    wr<f32>(info + 0x154, z);
}
void emitterCallback(JPAEmitterCalc_l *e, u32 slot) {
    u32 cb = e->emitterCallback;
    if (cb)
        gabi::call<void>(vfn(cb, slot), gabi::at<void>(cb), e);
}
} // namespace
bool JPA_doStartFrameProcess(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x028200C4, bool, e);
    f32 time = e->time;
    if (!(time < (f32)(s16)e->startFrame))
        return true;
    if (!((u32)e->status & 2)) {
        f32 next = time + 1.0f;
        e->time = next < 0.0f ? 0.0f : next;
    }
    return false;
}
VERIFY(0x028200C4, JPA_doStartFrameProcess);
bool JPA_doTerminationProcess(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x0282014C, bool, e);
    u8 delay = e->deleteCountdown;
    if (delay) {
        delay--;
        e->deleteCountdown = delay;
        return delay == 0;
    }
    s32 max = e->maxFrame;
    bool empty = false;
    if (max < 0) {
        e->status = (u32)e->status | 8;
        u32 base = gabi::ea(e);
        empty = rd<u32>(base + 0x1b4) + rd<u32>(base + 0x1c0) == 0;
    } else if (max > 0 && !((f32)e->tick < (f32)max)) {
        u32 status = (u32)e->status | 8;
        e->status = status;
        if (status & 0x40)
            return false;
        u32 base = gabi::ea(e);
        empty = rd<u32>(base + 0x1b4) + rd<u32>(base + 0x1c0) == 0;
    }
    if (empty && !(u8)e->deleteCountdown) {
        e->deleteCountdown = 2;
        return false;
    }
    return empty;
}
VERIFY(0x0282014C, JPA_doTerminationProcess);
void JPA_emitterCalc(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x0281FE40, void, e);
    wr<u32>(info + 0x15c, 0);
    if ((u32)e->status & 2) {
        emitterCallback(e, 0x1c);
        emitterCallback(e, 0x24);
        return;
    }
    if ((u8)e->deleteCountdown)
        return;
    u8 clear = e->clearCountdown;
    if (clear) {
        e->clearCountdown = --clear;
        if (!clear)
            gabi::call<void>(0x0281DE68, e, 0);
        return;
    }
    gabi::call<void>(0x0281F42C, e);
    emitterCallback(e, 0x1c);
    gabi::call<void>(0x0281F688, e);
    gabi::call<void>(0x0282DC48, gabi::at<void>(gabi::ea(e) + 0x9c));
    gabi::call<void>(0x02824294, gabi::at<void>(gabi::ea(e) + 0x19c));
    if (!((u32)e->status & 8))
        gabi::call<void>(0x0281F878, e);
    emitterCallback(e, 0x24);
    gabi::call<void>(0x0281FBD8, e);
    gabi::call<void>(0x0281FD14, e);
    f32 tick = (f32)e->tick + 1.0f;
    e->tick = tick < 0.0f ? 0.0f : tick;
}
VERIFY(0x0281FE40, JPA_emitterCalc);
void JPA_managerCalc(void *manager, u8 group) {
    WWHD_FUNC(0x0282167C, void, manager, group);
    if (group >= 16)
        gabi::call<void>(0x0273AA24, gabi::at<void>(0x10170E84), 0x108, gabi::at<void>(0x10170E9C));
    u32 link = rd<u32>(gabi::ea(manager) + 0x50 + 12 * group);
    while (link) {
        auto *emitter = gabi::at<JPAEmitterCalc_l>(rd<u32>(link));
        link = rd<u32>(link + 0xc);
        if (JPA_doStartFrameProcess(emitter)) {
            if (JPA_doTerminationProcess(emitter))
                gabi::call<void>(0x028215F8, manager, emitter);
            else
                JPA_emitterCalc(emitter);
        }
    }
}
VERIFY(0x0282167C, JPA_managerCalc);
void JPA_calcCreateParticles(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x0281F878, void, e);
    if ((u32)e->status & 0x20) {
        s32 count = 0;
        if ((u32)e->dataFlags & 2) {
            u32 div = e->divisions;
            count = (u8)e->volumeType == 1 ? (s32)((div + 1) * (div - 1) * 4 + 6) : (s32)div;
            wr<u32>(info + 0x160, 0);
        } else {
            f32 r = randomFloat(e);
            r = (r + r) - 1.0f;
            f32 increment =
                gabi::fmuls_ppc((f32)e->rate, gabi::fmadds((f32)e->rateRandom, r, 1.0f));
            f32 counter = gabi::fadds_ppc((f32)e->emitCount, increment);
            if (!(counter < 1.0f)) {
                count = gabi::ftoi(counter);
                e->emitCount = counter - (f32)count;
            } else {
                e->emitCount = counter;
                if (increment > 0.0f && ((u32)e->status & 0x10))
                    count = 1;
            }
        }
        wr<s32>(info + 0x15c, count);
        if ((u32)e->status & 1)
            count = 0;
        for (s32 i = 0; i < count; i++)
            if (!gabi::call<void *>(0x0281DCB8, e))
                break;
    }
    f32 limit = (f32)((u8)e->rateStep + 1);
    f32 timer = (f32)e->rateStepTimer + 1.0f;
    e->rateStepTimer = timer;
    if (!(timer < limit)) {
        e->rateStepTimer = timer - limit;
        e->status = ((u32)e->status | 0x20) & ~0x10u;
    } else
        e->status = (u32)e->status & ~0x30u;
}
VERIFY(0x0281F878, JPA_calcCreateParticles);
void JPA_calcKey(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x0281F42C, void, e);
    u32 data = e->dataLink;
    for (u32 i = 0; i < rd<u8>(data + 0x22); i++) {
        u32 key = rd<u32>(rd<u32>(data + 0x14) + 4 * i);
        u32 target = vfn(key, 0x2c);
        f32 tick = e->tick;
        u32 ptr = gabi::call<u32>(target, gabi::at<void>(key));
        u32 number = gabi::call<u32>(vfn(key, 0x24), gabi::at<void>(key));
        if (gabi::call<u32>(vfn(key, 0x1c), gabi::at<void>(key))) {
            s32 end = (s32)((u32)gabi::ftoi(rd<f32>(ptr + number * 16 - 16)) + 1);
            u32 loops = (u32)divide(gabi::ftoi(tick), end);
            tick -= (f32)(s32)(loops * (u32)end);
        }
        f32 value = gabi::call<f32>(0x028249D8, number, gabi::at<void>(ptr), tick);
        u32 id = gabi::call<u32>(vfn(key, 0x14), gabi::at<void>(key));
        switch (id) {
        case 0:
            e->rate = value;
            break;
        case 1:
            e->volumeSize = (u16)gabi::ftoi(value);
            break;
        case 2:
            e->volumeSweep = value;
            break;
        case 3:
            e->volumeMinRadius = value;
            break;
        case 4:
            e->lifetime = (s16)gabi::ftoi(value);
            break;
        case 5:
            e->moment = value;
            break;
        case 6:
            e->velocityOmni = value;
            break;
        case 7:
            e->velocityAxis = value;
            break;
        case 8:
            e->velocityDirection = value;
            break;
        case 9:
            e->spread = value;
            break;
        case 10:
            wr<f32>(gabi::ea(e) + 0x180, value);
            break;
        }
        data = e->dataLink;
    }
}
VERIFY(0x0281F42C, JPA_calcKey);
void JPA_calcEmitterGlobalPosition(JPAEmitterCalc_l *e, void *dst) {
    WWHD_FUNC(0x0281DBEC, void, e, dst);
    gabi::Local<be<f32>[12]> matrix;
    gabi::call<void>(0x028E945C, matrix.get(), (f32)e->globalScale[0], (f32)e->globalScale[1],
                     (f32)e->globalScale[2]);
    gabi::call<void>(0x028E9108, e->globalRotation, matrix.get(), matrix.get());
    for (int i = 0; i < 3; i++)
        wr<u32>(gabi::ea(matrix.get()) + i * 16 + 12, rd<u32>(gabi::ea(e) + 0x22c + i * 4));
    gabi::call<void>(0x028E8F64, matrix.get(), e->emitterTranslation, dst);
}
VERIFY(0x0281DBEC, JPA_calcEmitterGlobalPosition);
void JPA_calcEmitterInfo(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x0281F688, void, e);
    wr<u32>(info + 0x168, 0);
    wr<u32>(info + 0x164, 0);
    wr<u32>(info + 0x16c, 1);
    wr<u32>(info + 4, gabi::ea(e));
    wr<u32>(info + 0x170, (u16)e->divisions * 2 + 1);
    wr<f32>(info + 0x158, (f32)(u16)e->volumeSize);
    gabi::Local<be<f32>[12]> scale, rotation, matrix;
    gabi::call<void>(0x028E945C, scale.get(), (f32)e->emitterScale[0], (f32)e->emitterScale[1],
                     (f32)e->emitterScale[2]);
    s16 x = ((s32)(s16)e->emitterRotation[0] * 0x4000) / 90;
    s16 y = ((s32)(s16)e->emitterRotation[1] * 0x4000) / 90;
    s16 z = ((s32)(s16)e->emitterRotation[2] * 0x4000) / 90;
    gabi::call<void>(0x028245AC, x, y, z, rotation.get());
    gabi::call<void>(0x028E945C, matrix.get(), (f32)e->globalScale[0], (f32)e->globalScale[1],
                     (f32)e->globalScale[2]);
    gabi::call<void>(0x028E9108, e->globalRotation, matrix.get(), matrix.get());
    for (int i = 0; i < 3; i++)
        wr<u32>(gabi::ea(matrix.get()) + i * 16 + 12, rd<u32>(gabi::ea(e) + 0x22c + i * 4));
    gabi::call<void>(0x028E90D4, e->globalRotation, gabi::at<void>(info + 0x68));
    gabi::call<void>(0x028E9108, e->globalRotation, rotation.get(), gabi::at<void>(info + 0x38));
    gabi::call<void>(0x028E9108, gabi::at<void>(info + 0x38), scale.get(),
                     gabi::at<void>(info + 8));
    gabi::call<void>(0x0282466C, e->emitterDirection, gabi::at<void>(info + 0x98));
    for (int i = 0; i < 3; i++)
        wr<f32>(info + 0xc8 + 4 * i,
                gabi::fmuls_ppc((f32)e->emitterScale[i], (f32)e->globalScale[i]));
    for (int i = 0; i < 3; i++)
        wr<u32>(info + 0xd4 + 4 * i, rd<u32>(gabi::ea(e) + 0x14 + 4 * i));
    for (int i = 0; i < 3; i++)
        wr<u32>(info + 0xec + 4 * i, rd<u32>(gabi::ea(e) + 0x220 + 4 * i));
    gabi::call<void>(0x028E8F64, matrix.get(), e->emitterTranslation, gabi::at<void>(info + 0xe0));
}
VERIFY(0x0281F688, JPA_calcEmitterInfo);
void JPA_calcVolumePoint(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x02820244, void, e);
    wr<f32>(info + 0x13c, 0.0f);
    wr<f32>(info + 0x138, 0.0f);
    wr<f32>(info + 0x134, 0.0f);
    f32 x = randomFloat(e) - 0.5f, y = randomFloat(e) - 0.5f, z = randomFloat(e) - 0.5f;
    wr<f32>(info + 0x140, x);
    wr<f32>(info + 0x144, y);
    wr<f32>(info + 0x148, z);
    x = randomFloat(e) - 0.5f;
    z = randomFloat(e) - 0.5f;
    wr<f32>(info + 0x14c, x);
    wr<f32>(info + 0x150, 0.0f);
    wr<f32>(info + 0x154, z);
}
VERIFY(0x02820244, JPA_calcVolumePoint);
void JPA_calcVolumeLine(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x02820354, void, e);
    f32 z;
    if ((u32)e->dataFlags & 2) {
        u32 idx = rd<u32>(info + 0x160);
        f32 count = (f32)rd<s32>(info + 0x15c);
        f32 size = rd<f32>(info + 0x158);
        wr<u32>(info + 0x160, idx + 1);
        z = gabi::fmuls_ppc(size, ((f32)(s32)idx / (count - 1.0f)) - 0.5f);
    } else {
        f32 r = randomFloat(e) - 0.5f;
        z = gabi::fmuls_ppc(r, rd<f32>(info + 0x158));
    }
    volumeResult(0.0f, 0.0f, z);
}
VERIFY(0x02820354, JPA_calcVolumeLine);
void JPA_calcVolumeCube(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x02820688, void, e);
    f32 x = randomFloat(e) - 0.5f, y = randomFloat(e) - 0.5f, z = randomFloat(e) - 0.5f;
    f32 size = rd<f32>(info + 0x158);
    volumeResult(gabi::fmuls_ppc(size, x), gabi::fmuls_ppc(size, y), gabi::fmuls_ppc(size, z));
}
VERIFY(0x02820688, JPA_calcVolumeCube);
void JPA_calcVolumeCircle(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x028204A0, void, e);
    s16 angle;
    if ((u32)e->dataFlags & 2) {
        u32 idx = rd<u32>(info + 0x160);
        s16 step = divide((s32)(idx << 16), rd<s32>(info + 0x15c));
        f32 sweep = e->volumeSweep;
        wr<u32>(info + 0x160, idx + 1);
        angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)step, sweep));
    } else {
        s16 r = randomShort(e);
        angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)r, (f32)e->volumeSweep));
    }
    f32 r = randomFloat(e), min = e->volumeMinRadius;
    if ((u32)e->dataFlags & 1)
        r = (f32)(1.0 - (double)r * r);
    f32 radius = gabi::fmuls_ppc(gabi::fmadds(r, 1.0f - min, min), rd<f32>(info + 0x158));
    volumeResult(gabi::fmuls_ppc(sinAngle(angle), radius), 0.0f,
                 gabi::fmuls_ppc(cosAngle(angle), radius));
}
VERIFY(0x028204A0, JPA_calcVolumeCircle);
void JPA_calcVolumeSphere(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x02820778, void, e);
    s16 x, angle;
    if ((u32)e->dataFlags & 2) {
        u32 a = rd<u32>(info + 0x164), maximum = rd<u32>(info + 0x16c);
        u32 latitude = rd<u32>(info + 0x168), div = rd<u32>(info + 0x170);
        u16 step = divide((s32)(a << 16), (s32)maximum);
        x = (s16)((u32)divide((s32)(latitude << 15), (s32)(div - 1)) + 0x4000);
        angle = (s16)gabi::ftoi(gabi::fmadds((f32)step, (f32)e->volumeSweep, 32768.0f));
        if (++a == maximum) {
            latitude = rd<u32>(info + 0x168) + 1;
            div = rd<u32>(info + 0x170);
            wr<u32>(info + 0x164, 0);
            wr<u32>(info + 0x168, latitude);
            wr<u32>(info + 0x16c, (s32)(latitude * 2) < (s32)div
                                      ? maximum + (maximum == 1 ? 3 : 4)
                                      : (maximum == 4 ? 1 : maximum - 4));
        } else
            wr<u32>(info + 0x164, a);
    } else {
        s16 r = randomShort(e);
        s16 r2 = randomShort(e);
        x = r >> 1;
        angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)r2, (f32)e->volumeSweep));
    }
    f32 r = randomFloat(e), min = e->volumeMinRadius;
    if ((u32)e->dataFlags & 1) {
        f32 square = r * r;
        r = (f32)(1.0 - (double)square * r);
    }
    f32 radius = gabi::fmuls_ppc(gabi::fmadds(r, 1.0f - min, min), rd<f32>(info + 0x158));
    f32 side = gabi::fmuls_ppc(radius, cosAngle(x));
    volumeResult(gabi::fmuls_ppc(side, sinAngle(angle)), -gabi::fmuls_ppc(radius, sinAngle(x)),
                 gabi::fmuls_ppc(side, cosAngle(angle)));
}
VERIFY(0x02820778, JPA_calcVolumeSphere);
void JPA_calcVolumeCylinder(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x02820A84, void, e);
    s16 rangle = randomShort(e);
    s16 angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)rangle, (f32)e->volumeSweep));
    f32 r = randomFloat(e), min = e->volumeMinRadius;
    if ((u32)e->dataFlags & 1)
        r = (f32)(1.0 - (double)r * r);
    f32 radius = gabi::fmuls_ppc(gabi::fmadds(r, 1.0f - min, min), rd<f32>(info + 0x158));
    f32 y = randomFloat(e);
    y = (y + y) - 1.0f;
    volumeResult(gabi::fmuls_ppc(radius, sinAngle(angle)),
                 gabi::fmuls_ppc(rd<f32>(info + 0x158), y),
                 gabi::fmuls_ppc(radius, cosAngle(angle)));
}
VERIFY(0x02820A84, JPA_calcVolumeCylinder);
void JPA_calcVolumeTorus(JPAEmitterCalc_l *e) {
    WWHD_FUNC(0x02820BE0, void, e);
    s16 r = randomShort(e);
    s16 a = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)r, (f32)e->volumeSweep));
    u16 b = randomShort(e);
    f32 size = rd<f32>(info + 0x158);
    f32 radius = gabi::fmuls_ppc((f32)e->volumeMinRadius, size);
    f32 sa = sinAngle(a), ca = cosAngle(a), sb = sinAngle(b), cb = cosAngle(b);
    f32 axisX = gabi::fmuls_ppc(gabi::fmuls_ppc(radius, sa), cb);
    f32 axisY = gabi::fmuls_ppc(radius, sb);
    f32 axisZ = gabi::fmuls_ppc(gabi::fmuls_ppc(radius, ca), cb);
    wr<f32>(info + 0x14c, axisX);
    wr<f32>(info + 0x150, axisY);
    wr<f32>(info + 0x154, axisZ);
    f32 x = gabi::fmadds(size, sa, axisX), z = gabi::fmadds(size, ca, axisZ);
    wr<f32>(info + 0x134, x);
    wr<f32>(info + 0x138, axisY);
    wr<f32>(info + 0x13c, z);
    wr<f32>(info + 0x140, gabi::fmuls_ppc(x, rd<f32>(info + 0xc8)));
    wr<f32>(info + 0x144, gabi::fmuls_ppc(axisY, rd<f32>(info + 0xcc)));
    wr<f32>(info + 0x148, gabi::fmuls_ppc(z, rd<f32>(info + 0xd0)));
}
VERIFY(0x02820BE0, JPA_calcVolumeTorus);
