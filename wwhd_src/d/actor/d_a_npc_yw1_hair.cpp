/**
 * d_a_npc_yw1_hair.cpp (WWHD)
 * NPC - Sue-Belle: daNpc_Yw1_c::setHairAngle (hair physics: head orientation, wind, a three-joint
 * spring chain and a sway wave).
 *
 * Written from the WWHD code (cking.rpx); the GameCube
 * function is a "Nonmatching" stub.
 */
#include "d/actor/d_a_npc_yw1.h"

// HD: ABI bindings for wind and squared distance.
/* 0257E1B8 dKyw_get_AllWind_vec(cXyz* pos, cXyz* dir, f32* power) */
void Yw1_getWind(cXyz* pos, cXyz* dir, be<f32>* pow);
f32 Yw1_squareDistance(const cXyz* a, const cXyz* b);

static const cXyz l_hairUp = {0.0f, 1.0f, 0.0f};
static const cXyz l_hairSide = {1.0f, 0.0f, 0.0f};

static inline s16 clamp_s16(s16 v, s16 lo, s16 hi) { return v > hi ? hi : (v < lo ? lo : v); }

/* 022FC294 */
void daNpc_Yw1_c::setHairAngle() {
    /* the hair root and tip in the head joint's frame (static cXyz, set up in __sinit) */
    Mtx34* headMtx = mpMorf->getModel()->getAnmMtx(m_head_jnt_num);
    cXyz root;
    cXyz tip;
    PSMTXMultVec(headMtx, &l_hairRoot, &root);
    PSMTXMultVec(headMtx, &l_hairTip, &tip);
    cXyz dir;
    cXyz_mi(&tip, &dir, &root);
    s16 yaw = cM_atan2s(dir.x, dir.z);
    f32 sinY = cM_ssin(yaw);
    f32 cosY = cM_scos(yaw);

    /* wind at the head */
    Mtx34* m = mpMorf->getModel()->getAnmMtx(m_head_jnt_num);
    cXyz headPos;
    headPos.x = (f32)m->m[0][3];
    headPos.y = (f32)m->m[1][3];
    headPos.z = (f32)m->m[2][3];
    cXyz windDir;
    Yw1WindPower windPower;
    Yw1_getWind(&headPos, &windDir, &windPower.mValue);
    cXyz up;
    f32 pow = windPower.mValue;
    f32 pow2 = pow * pow;
    PSMTXMultVecSR(mpMorf->getModel()->getAnmMtx(m_head_jnt_num), &l_hairUp /* (0, 1, 0) */, &up);
    cXyz side;
    PSMTXMultVecSR(mpMorf->getModel()->getAnmMtx(m_head_jnt_num), &l_hairSide /* (1, 0, 0) */,
                   &side);

    /* head orientation: pitch mHeadPitch, yaw mHeadYaw (the yaw is kept while the head points up or down) */
    s16 oldPitch = mHeadPitch;
    s16 oldYaw = mHeadYaw;
    s16 newYaw;
    if (side.y < 0.0f) {
        cXyz xz;
        xz.y = 0.0f;
        xz.x = (f32)up.x;
        xz.z = (f32)up.z;
        f32 len = std_sqrtf(PSVECSquareMag(&xz));
        mHeadPitch = cM_atan2s(up.y, -len);
        s16 y = (s16)(cM_atan2s(up.x, up.z) + 0x8000);
        newYaw = std::fabs((f32)up.y) > 0.7f ? oldYaw : y;
    } else {
        cXyz xz;
        xz.y = 0.0f;
        xz.x = (f32)up.x;
        xz.z = (f32)up.z;
        f32 len = std_sqrtf(PSVECSquareMag(&xz));
        mHeadPitch = cM_atan2s(up.y, len);
        s16 y = cM_atan2s(up.x, up.z);
        newYaw = std::fabs((f32)up.y) > 0.7f ? oldYaw : y;
    }
    s16 dPitch = (s16)(mHeadPitch - oldPitch);
    s16 dYaw = (s16)(newYaw - oldYaw);
    mHeadYaw = newYaw;
    s16 halfPitch = clamp_s16((s16)(dPitch / 2), -0x200, 0x200);
    s16 halfYaw = clamp_s16((s16)(dYaw / 2), -0x800, 0x800);

    /* the hair's previous position relative to the head, pushed by the wind */
    cXyz v;
    f32 dy = mHairPrevPos.y - headPos.y;
    v.x = mHairPrevPos.x - headPos.x;
    v.y = dy - 7.5f;
    v.z = mHairPrevPos.z - headPos.z;
    cXyz wind;
    cXyz_ml(&windDir, &wind, pow2);
    PSVECAdd(&v, &wind, &v);
    f32 vx = v.x;
    if (std::fabs(vx) < 0.01f) {
        vx = 0.0f;
        v.x = 0.0f;
    }
    f32 vz = v.z;
    if (std::fabs(vz) < 0.01f) {
        vz = 0.0f;
        v.z = 0.0f;
    }
    f32 vy = v.y;
    f32 fwd = gabi::fmadds(vz, cosY, vx * sinY);
    s16 oldA1z = mHair1Pitch;
    s16 oldA1y = mHair1Yaw;
    s16 a = cM_atan2s(-fwd, -vy);

    /* hair 1 (mHair1Pitch z, mHair1Yaw y) */
    s16 target;
    if (mHair1Pitch < 0 || (u32)(a + 0x77FF) < 0x77FF) {
        target = 0;
    } else {
        target = (u32)(a + 0x77FF) >= 0xF000 ? (s16)0x7800 : a;
    }
    cLib_addCalcAngleS2(&mHair1Pitch, target, 5, 0x400);
    mHair1Pitch = (s16)(mHair1Pitch + (mHair1PitchVelocity + halfPitch));
    f32 vy2 = v.y;
    f32 len2 = std_sqrtf(gabi::fmadds(fwd, fwd, vy2 * vy2));
    f32 vz2 = v.z;
    f32 vx2 = v.x;
    f32 lat = gabi::fmsubs(vx2, cosY, vz2 * sinY);
    s16 b = clamp_s16(cM_atan2s(-lat, len2), -0x3800, 0x3800);
    cLib_addCalcAngleS2(&mHair1Yaw, b, 5, 0x400);

    s16 d1z = (s16)(mHair1Pitch - oldA1z);
    s16 a1y = (s16)(mHair1Yaw + (mHair1YawVelocity - halfYaw));
    s16 d1y = (s16)(a1y - oldA1y);
    mHair1Yaw = a1y;
    mHair1PitchVelocity = (s16)gabi::ftoi((f32)d1z * 0.2f);
    mHair1YawVelocity = (s16)gabi::ftoi((f32)d1y * 0.2f);

    /* hair 2 (mHair2Pitch z, mHair2Yaw y) follows hair 1 */
    s16 a2z = (s16)(mHair2Pitch - d1z);
    s16 a2y = (s16)(mHair2Yaw - d1y);
    mHair2Pitch = a2z;
    mHair2Yaw = a2y;
    cLib_addCalcAngleS2(&mHair2Pitch, 0, 5, 0x400);
    cLib_addCalcAngleS2(&mHair2Yaw, 0, 5, 0x400);
    s16 n2z = (s16)(mHair2Pitch + (mHair2PitchVelocity + halfPitch));
    s16 d2z = (s16)(n2z - a2z);
    s16 n2y = (s16)(mHair2Yaw + (mHair2YawVelocity - halfYaw));
    s16 d2y = (s16)(n2y - a2y);
    mHair2Pitch = n2z;
    mHair2Yaw = n2y;
    mHair2PitchVelocity = (s16)gabi::ftoi((f32)d2z * 0.2f);
    mHair2YawVelocity = (s16)gabi::ftoi((f32)d2y * 0.2f);

    /* hair 3 (mHair3Pitch z, mHair3Yaw y) follows hair 2 */
    s16 a3z = (s16)(mHair3Pitch - d2z);
    s16 a3y = (s16)(mHair3Yaw - d2y);
    mHair3Pitch = a3z;
    mHair3Yaw = a3y;
    cLib_addCalcAngleS2(&mHair3Pitch, 0, 5, 0x400);
    cLib_addCalcAngleS2(&mHair3Yaw, 0, 5, 0x400);
    s16 n3z = (s16)(mHair3Pitch + (mHair3PitchVelocity + halfPitch));
    s16 d3z = (s16)(n3z - a3z);
    s16 n3y = (s16)(mHair3Yaw + (mHair3YawVelocity - halfYaw));
    s16 d3y = (s16)(n3y - a3y);
    mHair3Pitch = n3z;
    mHair3Yaw = n3y;
    mHair3PitchVelocity = (s16)gabi::ftoi((f32)d3z * 0.2f);
    mHair3YawVelocity = (s16)gabi::ftoi((f32)d3y * 0.2f);

    /* sway wave: faster and wider with the head's speed and the wind */
    f32 dist = std_sqrtf(Yw1_squareDistance(&mHairPrevPos, &headPos));
    f32 k = gabi::fmadds(dist, 0.65f, pow2) * 0.25f;
    if (k > 1.0f) {
        k = 1.0f;
    }
    s16 inc = (s16)gabi::ftoi(gabi::fmadds(k, 4096.0f, 1500.0f));
    s16 wave = (s16)(mHairWave + inc);
    mHairWave = wave;
    mHair1Wave = (s16)gabi::ftoi(k * 2280.0f * cM_scos(wave));
    mHair2Wave = (s16)gabi::ftoi(
        k * 3908.0f * cM_scos((s16)gabi::ftoi(gabi::fnmsubs(3.0f, (f32)inc, (f32)wave))));
    mHair3Wave = (s16)gabi::ftoi(
        k * 7568.0f * cM_scos((s16)gabi::ftoi(gabi::fnmsubs(6.0f, (f32)inc, (f32)wave))));
    mHairPrevPos.copy(headPos);
}

/* WWHD glue
data l_hairUp 10023758
data l_hairSide 10023764
ext 0257E1B8 void Yw1_getWind(cXyz* pos, cXyz* dir, be<f32>* pow)
ext 028E8DE8 f32 Yw1_squareDistance(const cXyz* a, const cXyz* b)
*/
