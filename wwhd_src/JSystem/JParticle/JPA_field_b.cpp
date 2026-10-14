/** JPAField.cpp — vortex, convection, random, drag and spin fields.
 * GameCube names with verified WWHD arithmetic and guest frames. */
#include "JSystem/JParticle/JPA_field_b.h"
using namespace gabi;
static const f32 l_one;
static const f32 l_zero;
static const f32 l_epsilon;
static const f32 l_half;
static JPAFieldEmitterSlot l_emitterInfo;
// Native snapshots preserve the binary's load order without introducing guest temporaries.
struct JPAFieldVector {
    f32 x, y, z;
};
static JPAFieldVector readVec(const cXyz *p) {
    return {p->x, p->y, p->z};
}
static void writeVec(cXyz *p, JPAFieldVector v) {
    p->x = v.x;
    p->y = v.y;
    p->z = v.z;
}
static f32 square(JPAFieldVector v) {
    return fmadds(v.z, v.z, fmadds(v.x, v.x, v.y * v.y));
}
static f32 dot(JPAFieldVector a, JPAFieldVector b) {
    return fmadds(a.z, b.z, fmadds(a.x, b.x, a.y * b.y));
}
static JPAFieldVector scale(JPAFieldVector v, f32 s) {
    return {v.x * s, v.y * s, v.z * s};
}
static JPAFieldVector cross(JPAFieldVector a, JPAFieldVector b) {
    return {fmsubs(a.y, b.z, a.z * b.y), fmsubs(a.z, b.x, a.x * b.z), fmsubs(a.x, b.y, a.y * b.x)};
}
static f32 root(f32 s) {
    return (f32)JPAField_sqrt(s);
}
static void normalizeAt(cXyz *p, f32 epsilon, f32 one) {
    f32 s = square(readVec(p));
    // HD: unordered values skip normalization, as does equality with epsilon.
    if (s > epsilon) {
        f32 inv = one / root(s);
        auto v = readVec(p);
        writeVec(p, scale(v, inv));
    }
}
static void basePre(JPABaseField *self, JPAFieldData *data) {
    JPAField_basePre(self, data);
}
static void baseVel(JPABaseField *self, JPAFieldData *data, JPABaseParticle *particle) {
    JPAField_baseVel(self, data, particle);
}
static void transform(cXyz *src, cXyz *dst) {
    JPAFieldEmitterInfo *info = l_emitterInfo.mpInfo;
    PSMTXMultVec(&info->mEmitterGlobalRot, src, dst);
}
static f32 randomFloat() {
    JPAFieldEmitterInfo *info = l_emitterInfo.mpInfo;
    JPAFieldEmitter *emitter = info->mpCurEmitter;
    u32 seed = (u32)emitter->mRandomSeed * 0x19660D + 0x3C6EF35F;
    emitter->mRandomSeed = seed;
    return std::bit_cast<f32>((seed >> 9) | 0x3F800000);
}
/* 0282356C */
void JPAVortexField::preCalc(JPAFieldData *data) {
    basePre(this, data);
    transform(&data->mDir, &data->mWork0);
    f32 epsilon = l_epsilon, one = l_one;
    normalizeAt(&data->mWork0, epsilon, one);
    f32 z = (f32)data->mPos.z;
    f32 s = z * z;
    data->mVal1 = s;
    data->mVal2 = one / s;
}
/* 02823640 */
void JPAVortexField::calc(JPAFieldData *data, JPABaseParticle *particle) {
    auto axis = readVec(&data->mWork0), pos = readVec(&particle->mLocalPosition);
    f32 d = dot(axis, pos);
    auto force = JPAFieldVector{fnmsubs(axis.x, d, pos.x), fnmsubs(axis.y, d, pos.y),
                                fnmsubs(axis.z, d, pos.z)};
    f32 sq = square(force), power = sq, limit = (f32)data->mVal1;
    if (power > limit)
        power = limit;
    f32 temp = power * (f32)data->mVal2, one = l_one;
    f32 speed = fmadds(one - temp, (f32)data->mMag, temp * (f32)data->mMagRndm);
    f32 epsilon = l_epsilon;
    if (sq > epsilon) {
        force = scale(force, one / root(sq));
        axis = readVec(&data->mWork0);
    }
    auto vel = scale(cross(force, axis), speed);
    writeVec(&data->mVel, vel);
    baseVel(this, data, particle);
}
/* 02824508 */
u32 JPAVortexField::isItinRange(JPAFieldData *data, f32 distance) {
    return 1;
}
/* 028237D4 */
void JPAConvectionField::preCalc(JPAFieldData *data) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    FrameLocal<cXyz> axis(8);
    basePre(this, data);
    auto direction = readVec(&data->mDir), position = readVec(&data->mPos);
    auto a = cross(position, direction), p = cross(a, direction);
    writeVec(axis.get(), a);
    writeVec(&data->mPos, p);
    f32 epsilon = l_epsilon, one = l_one;
    normalizeAt(&data->mPos, epsilon, one);
    transform(&data->mPos, &data->mWork0);
    transform(&data->mDir, &data->mWork1);
    transform(axis.get(), &data->mWork2);
    normalizeAt(&data->mWork0, epsilon, one);
    normalizeAt(&data->mWork1, epsilon, one);
    normalizeAt(&data->mWork2, epsilon, one);
}
/* 02823A28 */
void JPAConvectionField::calc(JPAFieldData *data, JPABaseParticle *particle) {
    auto pos = readVec(&particle->mLocalPosition), a = readVec(&data->mWork0),
         b = readVec(&data->mWork2);
    f32 da = dot(a, pos), db = dot(b, pos);
    auto n = JPAFieldVector{fmadds(b.x, db, a.x * da), fmadds(b.y, db, a.y * da),
                            fmadds(b.z, db, a.z * da)};
    f32 length = (f32)data->mVal1, epsilon = l_epsilon, zero = l_zero, one = l_one;
    f32 s = square(n);
    auto n2 = JPAFieldVector{};
    if (s > epsilon) {
        f32 k = (one / root(s)) * length;
        n2 = scale(n, k);
        pos = readVec(&particle->mLocalPosition);
    } else
        n2 = JPAFieldVector{zero, zero, zero};
    auto delta = JPAFieldVector{pos.x - n2.x, pos.y - n2.y, pos.z - n2.z};
    auto axis = cross(readVec(&data->mWork1), n2), vel = cross(axis, delta);
    writeVec(&data->mVel, vel);
    f32 mag = (f32)data->mMag;
    s = square(vel);
    if (s > epsilon) {
        f32 k = (one / root(s)) * mag;
        writeVec(&data->mVel, scale(readVec(&data->mVel), k));
    }
    f32 extra = (f32)data->mVal2;
    if (extra != zero) {
        s = square(delta);
        if (s > epsilon)
            delta = scale(delta, (one / root(s)) * extra);
        vel = readVec(&data->mVel);
        writeVec(&data->mVel, {vel.x + delta.x, vel.y + delta.y, vel.z + delta.z});
    }
    baseVel(this, data, particle);
}
/* 02824510 */
u32 JPAConvectionField::isItinRange(JPAFieldData *data, f32 distance) {
    return 1;
}
/* 02823CDC */
void JPARandomField::calc(JPAFieldData *data, JPABaseParticle *particle) {
    s32 age = ftoi((f32)particle->mCurFrame);
    if (age != 0) {
        u32 cycle = (u8)data->mCycle;
        if (!cycle || age % (s32)cycle != 0)
            return;
    }
    f32 x = randomFloat(), y = randomFloat(), z = randomFloat();
    f32 one = l_one, half = l_half, mag = (f32)data->mMag;
    writeVec(&data->mVel,
             {((x - one) - half) * mag, ((y - one) - half) * mag, ((z - one) - half) * mag});
    baseVel(this, data, particle);
}
/* 02823E14 */
void JPADragField::init(JPAFieldData *data, JPABaseParticle *particle) {
    f32 random = randomFloat(), one = l_one, half = l_half;
    f32 drag = fmadds((f32)data->mMagRndm, (random - one) - half, (f32)data->mMag);
    if (drag > one)
        drag = one;
    particle->mFieldDrag = drag;
}
/* 02823E8C */
void JPADragField::calc(JPAFieldData *data, JPABaseParticle *particle) {
    if ((u32)particle->mStatus & 4) {
        f32 drag = (f32)particle->mDrag, fieldDrag = (f32)particle->mFieldDrag;
        particle->mDrag = drag * fieldDrag;
    } else {
        f32 affect = JPAField_calcFade(this, data, (f32)particle->mCurNormTime);
        f32 one = l_one, fieldDrag = (f32)particle->mFieldDrag;
        f32 factor = fnmsubs(affect, one - fieldDrag, one), drag = (f32)particle->mDrag;
        particle->mDrag = drag * factor;
    }
}
/* 02823EF0 */
void JPASpinField::preCalc(JPAFieldData *data) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    FrameLocal<JPAFieldMatrix> matrix(8);
    basePre(this, data);
    transform(&data->mDir, &data->mPos);
    f32 sq = square(readVec(&data->mPos)), epsilon = l_epsilon;
    if (sq > epsilon) {
        f32 r = root(sq), one = l_one;
        writeVec(&data->mPos, scale(readVec(&data->mPos), one / r));
    }
    JPAField_RotAxis((Mtx34 *)matrix.get(), &data->mPos, (f32)data->mMag);
    // Pure FPR copies preserve the original single-precision bit patterns.
    for (u32 column = 0; column < 3; ++column)
        for (u32 row = 0; row < 3; ++row)
            data->mWorkWords[column][row] = (u32)matrix->m[row][column];
}
/* 02823FF0 */
void JPASpinField::calc(JPAFieldData *data, JPABaseParticle *particle) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    FrameLocal<JPAFieldMatrix> matrix(8);
    FrameLocal<cXyz> position(0x38);
    for (u32 column = 0; column < 3; ++column)
        for (u32 row = 0; row < 3; ++row)
            matrix->m[row][column] = (u32)data->mWorkWords[column][row];
    u32 zero = std::bit_cast<u32>((f32)l_zero);
    matrix->m[0][3] = zero;
    matrix->m[1][3] = zero;
    matrix->m[2][3] = zero;
    PSMTXMultVecSR((Mtx34 *)matrix.get(), &particle->mLocalPosition, position.get());
    auto next = readVec(position.get()), old = readVec(&particle->mLocalPosition);
    writeVec(&data->mVel, {next.x - old.x, next.y - old.y, next.z - old.z});
    baseVel(this, data, particle);
}
/* 02824518 */
u32 JPASpinField::isItinRange(JPAFieldData *data, f32 distance) {
    return 1;
}
/* 02824230 */
void JPAFieldManager::init(JPABaseParticle *particle) {
    for (JPAFieldLink *link = mList.mHead; link; link = link->mNext) {
        JPAFieldData *data = link->mObject;
        JPABaseField *field = data->mpBaseField;
        u32 target = field->__vtbl->mInit;
        call_ptr<void>(target, field, data, particle);
    }
}
/* 02824294 */
void JPAFieldManager::preCalc() {
    for (JPAFieldLink *link = mList.mHead; link; link = link->mNext) {
        JPAFieldData *data = link->mObject;
        JPABaseField *field = data->mpBaseField;
        u32 target = field->__vtbl->mPreCalc;
        call_ptr<void>(target, field, data);
    }
}
/* 028242E8 */
void JPAFieldManager::calc(JPABaseParticle *particle) {
    for (JPAFieldLink *link = mList.mHead; link; link = link->mNext) {
        JPAFieldData *data = link->mObject;
        if (data->mSttFlag & 0x80) {
            auto pos = readVec(&particle->mGlobalPosition), origin = readVec(&data->mPos);
            JPABaseField *field = data->mpBaseField;
            u32 target = field->__vtbl->mIsItinRange;
            auto delta = JPAFieldVector{pos.x - origin.x, pos.y - origin.y, pos.z - origin.z};
            if (!call_ptr<u32>(target, field, data, square(delta)))
                continue;
        }
        JPABaseField *field = data->mpBaseField;
        u32 target = field->__vtbl->mCalc;
        call_ptr<void>(target, field, data, particle);
    }
}
/* WWHD glue
ext 02822C38 void JPAField_basePre(JPABaseField*,JPAFieldData*)
ext 02822B54 void JPAField_baseVel(JPABaseField*,JPAFieldData*,JPABaseParticle*)
ext 02822AC0 f32 JPAField_calcFade(JPABaseField*,JPAFieldData*,f32)
ext 028F37F0 f64 JPAField_sqrt(f32)
ext 028E9838 void JPAField_RotAxis(Mtx34*,cXyz*,f32)
data l_one 10171078
data l_zero 1017107C
data l_epsilon 10171084
data l_half 1017108C
data l_emitterInfo 104B5948
*/
