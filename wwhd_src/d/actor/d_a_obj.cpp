/* Object utility functions: movement resistance, wind/path velocity, quaternion rotation,
 * collision sound and hit particles. GameCube names follow tww/src/d/d_a_obj.cpp.
 * Verified against the private WWHD image.
 * HD: native vector/math calls, particle layout and header initialization differ from GameCube.
 */
#include "d/actor/d_a_obj.h"
#include "bindings.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo_wether.h"
#include <cmath>

using namespace gabi;
using namespace objHD;
using namespace daObj;

static f32 sOne;
static f32 sNegativeOne;
static f32 sZero;
static f32 sParticleSpeed;
static f32 sWaterScale;
static f32 sWaterDirectionalSpeed;
static f32 sGrassRate;
static f32 sGrassHeight;
static f32 sSmokeScale;
static f32 sHalf;
static f32 sPathPowerScale;
static f32 sQuaternionEpsilon;
static f32 sQuaternionRootEpsilon;
static f32 sWoodRate;
static f32 sWoodSpread;
static f32 sWoodSweep;
static f32 sHitDirectionEpsilon;
static f32 sNegativePi;
static f32 sPositivePi;
static QuaternionWords sIdentityY;
static QuaternionWords sIdentityY2;
static QuaternionWords sIdentityZ;
static cXyz sBaseY;
static cXyz sBaseZ;
static cXyz sTotalSpeed;
static cXyz sPathSpeed;
static u32 sTotalSpeedGuard;
static u32 sPathSpeedGuard;
static SinCosTable sSinCos;
static HeaderStatics sHeaderStatics;
static HeaderVector sHeaderVector0;
static HeaderVector sHeaderVector1;
static HeaderVector sHeaderVector2;
extern s32 objNormalize(cXyz*);
extern f32 objSqrt(f32);
extern void objCross(const cXyz*, cXyz*, const cXyz*);
extern void objSubtract(const cXyz*, cXyz*, const cXyz*);
extern void objCrawVec(const cXyz*, const cXyz*, cXyz*);
extern void objScale(const cXyz*, cXyz*, f32);
extern void objPSSubtract(const cXyz*, const cXyz*, cXyz*);
extern void objAdd(const cXyz*, cXyz*, const cXyz*);
extern void objDivide(const cXyz*, cXyz*, f32);
extern f32 objAcos(f32);
extern void objQuatRotAxis(daObj::ObjQuaternion*, cXyz*, f32);
extern s32 objGetHitSound(const dCcD_GObjInf*);
extern fopAc_ac_c* objGetHitActor(const dCcD_GObjTg*);
extern dCcD_GObjInf* objGetHitObject(const dCcD_GObjInf*);
extern u32 objGetAttribute(dBgS*, cBgS_PolyInfo*);
extern s32 objGetRoom(dBgS*, cBgS_PolyInfo*);
extern void objLandSmoke(u32, u32, cXyz*, s32, void*, cXyz*, s32, void*);
extern Emitter* objParticleSet(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*,
                               u32, void*, s32, const void*, const void*, u32);
extern void objPointWind(cXyz*, cXyz*, be<f32>*);
extern s32 objPathVector(cBgS_PolyInfo*, cXyz*, be<s32>*);
extern void objRegisterGlobalObject(HeaderVector*);
extern void objHeaderAngleConstruct(u8*);
extern void objHeaderByteConstruct(u8*);

namespace {
// HD: the verified wrappers reserve a vector slot and 32 bytes of linkage scratch.
template <class T> struct ObjLocal {
    gabi::Stack<T> payload;
    gabi::Stack<Linkage> linkage;
    T* get() const {
        return payload.get();
    }
};
inline void objZero(cXyz* out) {
    const f32 x = cXyz::Zero.x, y = cXyz::Zero.y, z = cXyz::Zero.z;
    out->x = x;
    out->z = z;
    out->y = y;
}
inline void objIdentity(daObj::ObjQuaternion* out, const QuaternionWords* identity) {
    QuaternionWords* result = (QuaternionWords*)out;
    for (u32 i = 0; i < 4; ++i)
        result->mWord[i] = identity->mWord[i];
}
/* inlined into quat_rotBaseY2 and quat_rotBaseZ */
static void quatFromBase(daObj::ObjQuaternion* out, const cXyz* input, const cXyz* base,
                         const QuaternionWords* identity) {
    ObjLocal<cXyz> normalized, cross;
    const f32 x = input->x, y = input->y, z = input->z;
    normalized.get()->x = x;
    normalized.get()->z = z;
    normalized.get()->y = y;
    if (!objNormalize(normalized.get())) {
        objIdentity(out, identity);
        return;
    }
    const f32 dot = PSVECDotProduct(base, normalized.get());
    const f32 one = sOne;
    const f32 sum = gabi::fadds_ppc(dot, one);
    const f32 root = objSqrt(gabi::fadds_ppc(sum, sum));
    objCross(base, cross.get(), normalized.get());
    if (!(std::fabs(root) > sQuaternionRootEpsilon)) {
        objIdentity(out, identity);
        return;
    }
    const f32 reciprocal = f32(f64(one) / f64(root));
    const f32 cx = cross.get()->x, cy = cross.get()->y, cz = cross.get()->z;
    out->x = gabi::fmuls_ppc(cx, reciprocal);
    out->y = gabi::fmuls_ppc(cy, reciprocal);
    out->z = gabi::fmuls_ppc(cz, reciprocal);
    out->w = gabi::fmuls_ppc(root, sHalf);
}
/* inlined into HitEff_kikuzu and HitEff_hibana */
static void hitDirection(cXyz* out, const dCcD_Cyl* cylinder) {
    const dCcD_Cyl* c = cylinder;
    objScale(&c->mGObjTg.mRVec, out, sNegativeOne);
    const f32 square = PSVECSquareMag(out);
    if (square < sHitDirectionEpsilon) {
        CylinderShape* shape = (CylinderShape*)c->mShapeAttr;
        auto* center = call_ptr<cXyz*>(shape->__vtbl->getCoCP.get(), shape);
        out->y = sZero;
        out->x = gabi::fsubs_ppc((f32)c->mGObjTg.mHitPos.x, f32(center->x));
        center = call_ptr<cXyz*>(shape->__vtbl->getCoCP.get(), shape);
        out->z = gabi::fsubs_ppc((f32)c->mGObjTg.mHitPos.z, f32(center->z));
    }
}
} // namespace

namespace daObj {

/* 02311CD8 */
void make_land_effect(fopAc_ac_c* i_actor, dBgS_GndChk* chk, f32 scale) {
    dComIfG_inf_c* play = (dComIfG_inf_c*)dComIfGp_get();
    cBgS_PolyInfo* poly = chk ? static_cast<cBgS_PolyInfo*>(chk) : nullptr;
    const u32 attribute = objGetAttribute(&play->play.mBgS, poly);
    const f32 spread = sOne, speed = sParticleSpeed;
    if (attribute == dBgS_Attr_GIANT_FLOWER_e || attribute == dBgS_Attr_CARPET_e || attribute == dBgS_Attr_ICE_e || attribute == dBgS_Attr_UNK1B_e)
        return;
    ObjLocal<cXyz> scaleV;
    auto* position = &i_actor->current.pos;
    if (attribute != dBgS_Attr_GRASS_e && attribute != dBgS_Attr_WATER_e) {
        scaleV.get()->x = scale;
        scaleV.get()->z = scale;
        scaleV.get()->y = scale;
        PSVECScale(scaleV.get(), scaleV.get(), sSmokeScale);
        objLandSmoke(OBJECT_EFFECT_PROCESS, LAND_SMOKE, position, -1, nullptr, scaleV.get(), -1, nullptr);
        return;
    }
    const f32 particleSize = attribute == dBgS_Attr_WATER_e ? gabi::fmuls_ppc(scale, sWaterScale) : scale;
    scaleV.get()->x = particleSize;
    scaleV.get()->y = particleSize;
    scaleV.get()->z = particleSize;
    dComIfG_inf_c* particlePlay = (dComIfG_inf_c*)dComIfGp_get();
    dPa_control_c* controller = particlePlay->play.mParticle;
    Emitter* emitter =
        objParticleSet(controller, 0u, attribute == dBgS_Attr_WATER_e ? ID_AK_JN_ELEMENTSHIBUKI00 : ID_AK_JN_ELEMENTKUSA00, position, nullptr,
                       scaleV.get(), 0xFFu, nullptr, -1, nullptr, nullptr, 0u);
    if (!emitter)
        return;
    emitter->mSpread = spread;
    emitter->mMaxFrame = 1;
    if (attribute == dBgS_Attr_WATER_e) {
        emitter->mRate = speed;
        emitter->mInitialVelDir = sWaterDirectionalSpeed;
    } else {
        emitter->mInitialVelDir = speed;
        emitter->mLifeTime = 20;
        const f32 zero = sZero;
        emitter->mTranslation.x = zero;
        emitter->mTranslation.y = sGrassHeight;
        emitter->mTranslation.z = zero;
        emitter->mRate = sGrassRate;
    }
}

/* 02311F80 */
cXyz* get_wind_spd(fopAc_ac_c* i_actor, f32 param_2) {
    cXyz* result = &sTotalSpeed;
    if (!sTotalSpeedGuard) {
        sTotalSpeedGuard = 1;
        objZero(result);
    }
    ObjLocal<cXyz> wind, savedWind, point, combined, output;
    ObjLocal<be<f32>> pointPower;
    const f32 power = dKyw_get_wind_pow();
    cXyz* direction = dKyw_get_wind_vec();
    objScale(direction, wind.get(), power);
    savedWind.get()->copy(*wind.get());
    objPointWind(&i_actor->current.pos, point.get(), pointPower.get());
    PSVECScale(point.get(), point.get(), f32(*pointPower.get()));
    objAdd(savedWind.get(), combined.get(), point.get());
    const f32 scale = gabi::fmuls_ppc(param_2, sHalf);
    objScale(combined.get(), output.get(), scale);
    result->copy(*output.get());
    return result;
}

/* 023120B4 */
cXyz* get_path_spd(cBgS_PolyInfo* poly, f32 param_2) {
    cXyz* result = &sPathSpeed;
    if (!sPathSpeedGuard) {
        objZero(result);
        sPathSpeedGuard = 1;
    }
    ObjLocal<be<s32>> pathPower;
    if (objPathVector(poly, result, pathPower.get())) {
        if (objNormalize(result)) {
            const f32 power = f32(s32(*pathPower.get()));
            const f32 scale = gabi::fmuls_ppc(gabi::fmuls_ppc(param_2, power), sPathPowerScale);
            PSVECScale(result, result, scale);
        } else {
            objZero(result);
        }
    }
    return result;
}

/* 023123C0 */
void posMoveF_stream(fopAc_ac_c* i_actor, const cXyz* addVelocity, const cXyz* stream, f32 linear,
                     f32 quadratic) {
    const f32 zero = sZero;
    posMoveF_grade(i_actor, addVelocity, stream, linear, quadratic, nullptr, zero, zero, nullptr);
}

namespace {

/* 02311AB8 */
static void posMoveF_resist_acc(cXyz* pDst, fopAc_ac_c* i_actor, const cXyz* stream, f32 linear,
                                f32 quadratic) {
    ObjLocal<cXyz> delta, resistance;
    objSubtract(&i_actor->speed, delta.get(), stream);
    const f32 x = delta.get()->x, y = delta.get()->y, z = delta.get()->z;
    const f32 qx = gabi::fmuls_ppc(gabi::fmuls_ppc(std::fabs(x), x), quadratic);
    const f32 qy = gabi::fmuls_ppc(gabi::fmuls_ppc(std::fabs(y), y), quadratic);
    const f32 qz = gabi::fmuls_ppc(gabi::fmuls_ppc(std::fabs(z), z), quadratic);
    resistance.get()->x = gabi::fmadds(x, linear, qx);
    resistance.get()->y = gabi::fmadds(y, linear, qy);
    resistance.get()->z = gabi::fmadds(z, linear, qz);
    PSVECScale(resistance.get(), resistance.get(), sNegativeOne);
    const f32 rx = resistance.get()->x, ry = resistance.get()->y, rz = resistance.get()->z;
    pDst->x = rx;
    pDst->z = rz;
    pDst->y = ry;
}

/* 02311B94 */
static void posMoveF_grade_acc(cXyz* pDst, fopAc_ac_c* i_actor, const cXyz* normal, f32 friction,
                               f32 noGradeCos, const cXyz* acceleration, const cXyz* extra) {
    objZero(pDst);
    if (!normal)
        return;
    ObjLocal<cXyz> total, cross, scaled;
    total.get()->x = acceleration->x;
    total.get()->y = gabi::fadds_ppc(acceleration->y, (f32)i_actor->gravity);
    total.get()->z = acceleration->z;
    if (extra)
        PSVECAdd(total.get(), extra, total.get());
    const f32 dot = PSVECDotProduct(total.get(), normal);
    if (!(dot < sZero))
        return;
    if (!(f32(normal->y) > noGradeCos)) {
        objCrawVec(normal, total.get(), cross.get());
        PSVECAdd(pDst, cross.get(), pDst);
    }
    objCrawVec(normal, &i_actor->speed, cross.get());
    objScale(cross.get(), scaled.get(), friction);
    objPSSubtract(pDst, scaled.get(), pDst);
}

} // namespace

/* 023121C4 */
void posMoveF_grade(fopAc_ac_c* i_actor, const cXyz* addVelocity, const cXyz* stream, f32 linear,
                    f32 quadratic, const cXyz* normal, f32 friction, f32 noGradeCos,
                    const cXyz* addAcceleration) {
    if (!stream)
        JUTAssertion::showAssert("d_a_obj.cpp", 0x118, "stream_spd != 0");
    ObjLocal<cXyz> resistance, grade;
    posMoveF_resist_acc(resistance.get(), i_actor, stream, linear, quadratic);
    const f32 zero = sZero, one = sOne;
    if (friction < zero || !(friction < one) || noGradeCos < zero || noGradeCos > one)
        JUTAssertion::showAssert("d_a_obj.cpp", 0x120,
                                 "(friction >= 0.0f) && (friction < 1.0f) && (no_grade_cos >= "
                                 "0.0f) && (no_grade_cos <= 1.0f)");
    posMoveF_grade_acc(grade.get(), i_actor, normal, friction, noGradeCos, resistance.get(),
                       addAcceleration);
    const u16 angle = i_actor->current.angle.y;
    const SinCos* trig = &sSinCos.mValues[angle >> 3];
    const f32 speed = (f32)i_actor->speedF;
    f32 x =
        gabi::fadds_ppc(gabi::fmadds(speed, (f32)trig->mSin, resistance.get()->x), grade.get()->x);
    f32 y =
        gabi::fadds_ppc(gabi::fadds_ppc(gabi::fadds_ppc((f32)i_actor->speed.y, (f32)i_actor->gravity),
                                        resistance.get()->y),
                        grade.get()->y);
    f32 z =
        gabi::fadds_ppc(gabi::fmadds(speed, (f32)trig->mCos, resistance.get()->z), grade.get()->z);
    const f32 maxFall = (f32)i_actor->maxFallSpeed;
    if (addAcceleration) {
        x = gabi::fadds_ppc(x, addAcceleration->x);
        y = gabi::fadds_ppc(y, addAcceleration->y);
        z = gabi::fadds_ppc(z, addAcceleration->z);
    }
    if (y < maxFall)
        y = maxFall;
    i_actor->speed.y = y;
    i_actor->speed.x = x;
    i_actor->speed.z = z;
    const f32 length = objSqrt(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
    i_actor->speedF = length;
    const s16 yaw = cM_atan2s(x, z);
    i_actor->current.angle.y = u16(yaw);
    fopAcM_posMove(i_actor, addVelocity);
}

/* 023123D8 */
void quat_rotBaseY(ObjQuaternion* quat, const cXyz* param_1) {
    ObjLocal<cXyz> cross, normalized;
    const f32 lengthSquared = PSVECSquareMag(param_1);
    const f32 epsilon = sQuaternionEpsilon;
    if (!(lengthSquared > epsilon)) {
        objIdentity(quat, &sIdentityY);
        return;
    }
    const cXyz* base = &sBaseY;
    objCross(base, cross.get(), param_1);
    const f32 crossSquared = PSVECSquareMag(cross.get());
    if (!(crossSquared > epsilon)) {
        objIdentity(quat, &sIdentityY);
        return;
    }
    objDivide(param_1, normalized.get(), lengthSquared);
    const f32 dot = PSVECDotProduct(base, normalized.get());
    const f32 angle = objAcos(dot);
    const f32 crossLength = objSqrt(crossSquared);
    const f32 reciprocal = f32(f64(sOne) / f64(crossLength));
    PSVECScale(cross.get(), cross.get(), reciprocal);
    objQuatRotAxis(quat, cross.get(), angle);
}

/* 02312548 */
void quat_rotBaseY2(ObjQuaternion* quat, const cXyz* param_1) {
    quatFromBase(quat, param_1, &sBaseY, &sIdentityY2);
}

/* 023126A0 */
void quat_rotBaseZ(ObjQuaternion* quat, const cXyz* param_1) {
    quatFromBase(quat, param_1, &sBaseZ, &sIdentityZ);
}

/* 023127F8 */
void quat_rotVec(ObjQuaternion* quat, const cXyz* from, const cXyz* to) {
    ObjLocal<cXyz> first, second, cross;
    const f32 tx = to->x, ty = to->y, tz = to->z;
    second.get()->x = tx;
    second.get()->y = ty;
    second.get()->z = tz;
    const f32 fx = from->x, fy = from->y, fz = from->z;
    first.get()->z = fz;
    first.get()->x = fx;
    first.get()->y = fy;
    if (!objNormalize(second.get()) || !objNormalize(first.get())) {
        objIdentity(quat, &sIdentityZ);
        return;
    }
    const f32 dot = PSVECDotProduct(first.get(), second.get());
    const f32 one = sOne;
    const f32 sum = gabi::fadds_ppc(dot, one);
    const f32 root = objSqrt(gabi::fadds_ppc(sum, sum));
    objCross(first.get(), cross.get(), second.get());
    if (!(std::fabs(root) > sQuaternionRootEpsilon)) {
        objIdentity(quat, &sIdentityZ);
        return;
    }
    const f32 reciprocal = f32(f64(one) / f64(root));
    const f32 x = cross.get()->x, y = cross.get()->y, z = cross.get()->z;
    quat->x = gabi::fmuls_ppc(x, reciprocal);
    quat->y = gabi::fmuls_ppc(y, reciprocal);
    quat->z = gabi::fmuls_ppc(z, reciprocal);
    quat->w = gabi::fmuls_ppc(root, sHalf);
}

/* 02312968 */
void SetCurrentRoomNo(fopAc_ac_c* i_actor, dBgS_GndChk* chk) {
    dComIfG_inf_c* play = (dComIfG_inf_c*)dComIfGp_get();
    cBgS_PolyInfo* poly = chk ? static_cast<cBgS_PolyInfo*>(chk) : nullptr;
    const s32 room = objGetRoom(&play->play.mBgS, poly);
    if (room >= 0)
        i_actor->current.roomNo = u8(room);
}

/* 023129C4 */
void HitSeStart(const cXyz* position, s32 room, const dCcD_GObjInf* object, u32 parameter) {
    const s32 hitSound = objGetHitSound(object);
    const fopAc_ac_c* hitActor = objGetHitActor(&object->mGObjTg);
    const dCcD_GObjInf* hitObject = objGetHitObject(object);
    // HD: returns without playing when any of these three values is absent.
    if (!hitSound || !hitActor || !hitObject)
        return;
    const u32 type = (u32)hitObject->mAtType;
    u32 sound;
    if (type & AT_TYPE_SKULL_HAMMER)
        sound = JA_SE_LK_HAMMER_HIT;
    else if (type & AT_TYPE_ARROWS)
        sound = JA_SE_LK_ARROW_REBOUND;
    else if (type & AT_TYPE_HOOKSHOT)
        sound = JA_SE_LK_HS_REBOUND;
    else {
        bool masterSword = false;
        if ((s16)hitActor->mProfName == fpcNm_PLAYER_e) {
            const u8 sword = dComIfGs_getSelectEquip(0);
            masterSword = sword == dItemNo_MASTER_SWORD_1_e || sword == dItemNo_MASTER_SWORD_2_e || sword == dItemNo_MASTER_SWORD_3_e;
        }
        if (hitSound == dCcG_SE_WOOD)
            sound = JA_SE_LK_W_WEP_HIT;
        else if (hitSound == dCcG_SE_METAL)
            sound = JA_SE_LK_MS_WEP_HIT;
        else
            sound = masterSword ? JA_SE_LK_SW_HIT_M : JA_SE_LK_SW_HIT_S;
    }
    const s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(sound, const_cast<cXyz*>(position), parameter & 0xFFu, reverb);
}

/* 02312B88 */
void HitEff_sub_kikuzu(const cXyz* position, const cXyz* direction, const dKy_tevstr_c* tev) {
    ObjLocal<cXyz> horizontal;
    ObjLocal<csXyz> angle;
    horizontal.get()->x = direction->x;
    horizontal.get()->z = direction->z;
    horizontal.get()->y = sZero;
    const f32 square = PSVECSquareMag(horizontal.get());
    const f32 radius = objSqrt(square);
    angle.get()->x = cM_atan2s(f32(direction->y), radius);
    angle.get()->y = cM_atan2s(f32(direction->x), f32(direction->z));
    angle.get()->z = 0;
    dComIfG_inf_c* play = (dComIfG_inf_c*)dComIfGp_get();
    dPa_control_c* controller = play->play.mParticle;
    const GXColor* color = &tev->mColorK0;
    Emitter* emitter = objParticleSet(controller, 0u, ID_AK_JN_ELEMENTKIKUZU00, position, angle.get(), nullptr, 0xFFu,
                                      nullptr, -1, color, color, 0u);
    if (emitter) {
        emitter->mRate = sWoodRate;
        emitter->mSpread = sWoodSpread;
        emitter->mVolumeSweep = sWoodSweep;
        emitter->mMaxFrame = 1;
    }
}

/* 02312C8C */
void HitEff_kikuzu(const fopAc_ac_c* i_actor, const dCcD_Cyl* i_cyl) {
    if (!objGetHitSound(i_cyl))
        return;
    ObjLocal<cXyz> direction;
    hitDirection(direction.get(), i_cyl);
    HitEff_sub_kikuzu(&i_cyl->mGObjTg.mHitPos, direction.get(), &i_actor->tevStr);
}

// HD: explicit registrations preserve the two overloaded GameCube signatures.
void HitEff_hibana(const cXyz* position, const cXyz* direction) {
    WWHD_FUNC(0x02312D6C, void, position, direction);
    ObjLocal<cXyz> horizontal;
    ObjLocal<csXyz> angle;
    horizontal.get()->x = direction->x;
    horizontal.get()->y = sZero;
    horizontal.get()->z = direction->z;
    const f32 square = PSVECSquareMag(horizontal.get());
    const f32 radius = objSqrt(square);
    const s16 pitch = cM_atan2s(f32(direction->y), radius);
    angle.get()->x = s16(u16(pitch) + 0x4000u);
    angle.get()->y = cM_atan2s(f32(direction->x), f32(direction->z));
    angle.get()->z = 0;
    dComIfG_inf_c* play = (dComIfG_inf_c*)dComIfGp_get();
    dPa_control_c* controller = play->play.mParticle;
    Emitter* emitter = objParticleSet(controller, 0u, ID_AK_JN_ELEMENTHIBANA00, position, angle.get(), nullptr, 0xFFu,
                                      nullptr, -1, nullptr, nullptr, 0u);
    if (emitter) {
        emitter->mMaxFrame = 1;
        const f32 rate = sParticleSpeed;
        emitter->mRate = rate;
        emitter->mInitialVelAxis = rate;
        emitter->mInitialVelDir = rate;
    }
}
VERIFY(0x02312D6C, static_cast<void(*)(const cXyz*, const cXyz*)>(HitEff_hibana));

// HD: explicit registrations preserve the two overloaded GameCube signatures.
void HitEff_hibana(const fopAc_ac_c* i_actor, const dCcD_Cyl* i_cyl) {
    WWHD_FUNC(0x02312E54, void, i_actor, i_cyl);
    if (!objGetHitSound(i_cyl))
        return;
    ObjLocal<cXyz> direction;
    hitDirection(direction.get(), i_cyl);
    HitEff_hibana(&i_cyl->mGObjTg.mHitPos, direction.get());
}
VERIFY(0x02312E54, static_cast<void(*)(const fopAc_ac_c*, const dCcD_Cyl*)>(HitEff_hibana));

} // namespace daObj

// HD: compiler-generated header initialization and global destructor registration.
namespace {
/* 02312F24 */
static void obj_header_staticInitialize() {
    sHeaderStatics.mZero[2] = 0;
    sHeaderStatics.mZero[0] = 0;
    sHeaderStatics.mZero[3] = 0;
    sHeaderStatics.mZero[1] = 0;
    objRegisterGlobalObject(&sHeaderVector0);
    const f32 negativePi = sNegativePi, positivePi = sPositivePi;
    sHeaderStatics.mNegativePi = negativePi;
    sHeaderStatics.mPositivePi = positivePi;
    objHeaderAngleConstruct(&sHeaderStatics.mAngleTag);
    objRegisterGlobalObject(&sHeaderVector1);
    objHeaderByteConstruct(&sHeaderStatics.mByteTag);
    objRegisterGlobalObject(&sHeaderVector2);
}
} // namespace

/* WWHD glue
str 1002464C
str 1002465C
str 10024668
data sOne 10024610
data sNegativeOne 10024614
data sZero 1002461C
data sParticleSpeed 10024620
data sWaterScale 10024624
data sWaterDirectionalSpeed 10024628
data sGrassRate 1002462C
data sGrassHeight 10024630
data sSmokeScale 10024634
data sHalf 10024638
data sPathPowerScale 10024648
data sQuaternionEpsilon 100246C4
data sQuaternionRootEpsilon 100246D8
data sWoodRate 100246EC
data sWoodSpread 100246F0
data sWoodSweep 100246F4
data sHitDirectionEpsilon 100246F8
data sNegativePi 10024700
data sPositivePi 10024704
data sIdentityY 100246C8
data sIdentityY2 100246DC
data sIdentityZ 101E9C38
data sBaseY 101FFBC0
data sBaseZ 101FFBCC
data sTotalSpeed 10468EE4
data sPathSpeed 10468EF0
data sTotalSpeedGuard 10468EFC
data sPathSpeedGuard 10468F00
data sSinCos 104A44F8
data sHeaderStatics 10468EC8
data sHeaderVector0 101C78C8
data sHeaderVector1 101C78D4
data sHeaderVector2 101C78E0
ext 0201B47C s32 objNormalize(cXyz*)
ext 028F4384 f32 objSqrt(f32)
ext 0201B080 void objCross(const cXyz*, cXyz*, const cXyz*)
ext 0201ADE0 void objSubtract(const cXyz*, cXyz*, const cXyz*)
ext 02017B6C void objCrawVec(const cXyz*, const cXyz*, cXyz*)
ext 0201AE48 void objScale(const cXyz*, cXyz*, f32)
ext 028E8DAC void objPSSubtract(const cXyz*, const cXyz*, cXyz*)
ext 0201AD78 void objAdd(const cXyz*, cXyz*, const cXyz*)
ext 0201AEAC void objDivide(const cXyz*, cXyz*, f32)
ext 028F4FAC f32 objAcos(f32)
ext 028E9B14 void objQuatRotAxis(daObj::ObjQuaternion*, cXyz*, f32)
ext 0251638C s32 objGetHitSound(const dCcD_GObjInf*)
ext 02515BBC fopAc_ac_c* objGetHitActor(const dCcD_GObjTg*)
ext 02516300 dCcD_GObjInf* objGetHitObject(const dCcD_GObjInf*)
ext 024EF0F4 u32 objGetAttribute(dBgS*, cBgS_PolyInfo*)
ext 024EF130 s32 objGetRoom(dBgS*, cBgS_PolyInfo*)
ext 025D5834 void objLandSmoke(u32, u32, cXyz*, s32, void*, cXyz*, s32, void*)
ext 025A847C Emitter* objParticleSet(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*, u32, void*, s32, const void*, const void*, u32)
ext 0257DE68 void objPointWind(cXyz*, cXyz*, be<f32>*)
ext 025AB184 s32 objPathVector(cBgS_PolyInfo*, cXyz*, be<s32>*)
ext 028F026C void objRegisterGlobalObject(HeaderVector*)
ext 028ED6F8 void objHeaderAngleConstruct(u8*)
ext 028EAB2C void objHeaderByteConstruct(u8*)
*/
