/**
 * d_a_npc_yw1_hair.cpp (WWHD)
 * NPC - Sue-Belle: daNpc_Yw1_c::setHairAngle (hair physics: head orientation, wind, a three-joint
 * spring chain and a sway wave).
 *
 * Written from the WWHD code (cking.rpx); the GameCube
 * function is a "Nonmatching" stub.
 */
#include "d/actor/d_a_npc_yw1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0257E1B8 dKyw_get_AllWind_vec(cXyz* pos, cXyz* dir, f32* power) */
static inline void dKyw_get_AllWind_vec(cXyz* pos, cXyz* dir, be<f32>* pow) { gabi::call(0x0257E1B8, pos, dir, pow); }
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }

static inline s16 clamp_s16(s16 v, s16 lo, s16 hi) { return v > hi ? hi : (v < lo ? lo : v); }

/* 022FC294 */
void daNpc_Yw1_c::setHairAngle() {
    WWHD_FUNC(0x022FC294, void, this);
    /* the hair root and tip in the head joint's frame (static cXyz, set up in __sinit) */
    Mtx34* headMtx = J3DModel_getAnmMtx(mpMorf->getModel(), m_head_jnt_num);
    gabi::Local<cXyz> root;
    gabi::Local<cXyz> tip;
    PSMTXMultVec(headMtx, gabi::at<cXyz>(0x101C6DC0), root);
    PSMTXMultVec(headMtx, gabi::at<cXyz>(0x101C6DB4), tip);
    gabi::Local<cXyz> dir;
    cXyz_mi(tip, dir, root);
    s16 yaw = cM_atan2s(dir->x, dir->z);
    f32 sinY = cM_ssin(yaw);
    f32 cosY = cM_scos(yaw);

    /* wind at the head */
    Mtx34* m = J3DModel_getAnmMtx(mpMorf->getModel(), m_head_jnt_num);
    gabi::Local<cXyz> headPos;
    headPos->x = (f32)m->m[0][3];
    headPos->y = (f32)m->m[1][3];
    headPos->z = (f32)m->m[2][3];
    gabi::Local<cXyz> windDir;
    gabi::Local<be<f32>> windPow;
    dKyw_get_AllWind_vec(headPos, windDir, windPow);
    gabi::Local<cXyz> up;
    f32 pow = *windPow;
    f32 pow2 = pow * pow;
    PSMTXMultVecSR(J3DModel_getAnmMtx(mpMorf->getModel(), m_head_jnt_num), gabi::at<cXyz>(0x10023758) /* (0, 1, 0) */, up);
    gabi::Local<cXyz> side;
    PSMTXMultVecSR(J3DModel_getAnmMtx(mpMorf->getModel(), m_head_jnt_num), gabi::at<cXyz>(0x10023764) /* (1, 0, 0) */, side);

    /* head orientation: pitch m8F4, yaw m8F6 (the yaw is kept while the head points up or down) */
    s16 oldPitch = m8F4;
    s16 oldYaw = m8F6;
    s16 newYaw;
    if (side->y < 0.0f) {
        gabi::Local<cXyz> xz;
        xz->y = 0.0f;
        xz->x = (f32)up->x;
        xz->z = (f32)up->z;
        f32 len = std_sqrtf(PSVECSquareMag(xz));
        m8F4 = cM_atan2s(up->y, -len);
        s16 y = (s16)(cM_atan2s(up->x, up->z) + 0x8000);
        newYaw = std::fabs((f32)up->y) > 0.7f ? oldYaw : y;
    } else {
        gabi::Local<cXyz> xz;
        xz->y = 0.0f;
        xz->x = (f32)up->x;
        xz->z = (f32)up->z;
        f32 len = std_sqrtf(PSVECSquareMag(xz));
        m8F4 = cM_atan2s(up->y, len);
        s16 y = cM_atan2s(up->x, up->z);
        newYaw = std::fabs((f32)up->y) > 0.7f ? oldYaw : y;
    }
    s16 dPitch = (s16)(m8F4 - oldPitch);
    s16 dYaw = (s16)(newYaw - oldYaw);
    m8F6 = newYaw;
    s16 halfPitch = clamp_s16((s16)(dPitch / 2), -0x200, 0x200);
    s16 halfYaw = clamp_s16((s16)(dYaw / 2), -0x800, 0x800);

    /* the hair's previous position relative to the head, pushed by the wind */
    gabi::Local<cXyz> v;
    f32 dy = mHairPrevPos.y - headPos->y;
    v->x = mHairPrevPos.x - headPos->x;
    v->y = dy - 7.5f;
    v->z = mHairPrevPos.z - headPos->z;
    gabi::Local<cXyz> wind;
    cXyz_ml(windDir, wind, pow2);
    PSVECAdd(v, wind, v);
    f32 vx = v->x;
    if (std::fabs(vx) < 0.01f) {
        vx = 0.0f;
        v->x = 0.0f;
    }
    f32 vz = v->z;
    if (std::fabs(vz) < 0.01f) {
        vz = 0.0f;
        v->z = 0.0f;
    }
    f32 vy = v->y;
    f32 fwd = gabi::fmadds(vz, cosY, vx * sinY);
    s16 oldA1z = m8F8;
    s16 oldA1y = m8FA;
    s16 a = cM_atan2s(-fwd, -vy);

    /* hair 1 (m8F8 z, m8FA y) */
    s16 target;
    if (m8F8 < 0 || (u32)(a + 0x77FF) < 0x77FF) {
        target = 0;
    } else {
        target = (u32)(a + 0x77FF) >= 0xF000 ? (s16)0x7800 : a;
    }
    cLib_addCalcAngleS2(&m8F8, target, 5, 0x400);
    m8F8 = (s16)(m8F8 + (m914 + halfPitch));
    f32 vy2 = v->y;
    f32 len2 = std_sqrtf(gabi::fmadds(fwd, fwd, vy2 * vy2));
    f32 vz2 = v->z;
    f32 vx2 = v->x;
    f32 lat = gabi::fmsubs(vx2, cosY, vz2 * sinY);
    s16 b = clamp_s16(cM_atan2s(-lat, len2), -0x3800, 0x3800);
    cLib_addCalcAngleS2(&m8FA, b, 5, 0x400);

    s16 d1z = (s16)(m8F8 - oldA1z);
    s16 a1y = (s16)(m8FA + (m916 - halfYaw));
    s16 d1y = (s16)(a1y - oldA1y);
    m8FA = a1y;
    m914 = (s16)gabi::ftoi((f32)d1z * 0.2f);
    m916 = (s16)gabi::ftoi((f32)d1y * 0.2f);

    /* hair 2 (m918 z, m91A y) follows hair 1 */
    s16 a2z = (s16)(m918 - d1z);
    s16 a2y = (s16)(m91A - d1y);
    m918 = a2z;
    m91A = a2y;
    cLib_addCalcAngleS2(&m918, 0, 5, 0x400);
    cLib_addCalcAngleS2(&m91A, 0, 5, 0x400);
    s16 n2z = (s16)(m918 + (m91C + halfPitch));
    s16 d2z = (s16)(n2z - a2z);
    s16 n2y = (s16)(m91A + (m91E - halfYaw));
    s16 d2y = (s16)(n2y - a2y);
    m918 = n2z;
    m91A = n2y;
    m91C = (s16)gabi::ftoi((f32)d2z * 0.2f);
    m91E = (s16)gabi::ftoi((f32)d2y * 0.2f);

    /* hair 3 (m920 z, m922 y) follows hair 2 */
    s16 a3z = (s16)(m920 - d2z);
    s16 a3y = (s16)(m922 - d2y);
    m920 = a3z;
    m922 = a3y;
    cLib_addCalcAngleS2(&m920, 0, 5, 0x400);
    cLib_addCalcAngleS2(&m922, 0, 5, 0x400);
    s16 n3z = (s16)(m920 + (m924 + halfPitch));
    s16 d3z = (s16)(n3z - a3z);
    s16 n3y = (s16)(m922 + (m926 - halfYaw));
    s16 d3y = (s16)(n3y - a3y);
    m920 = n3z;
    m922 = n3y;
    m924 = (s16)gabi::ftoi((f32)d3z * 0.2f);
    m926 = (s16)gabi::ftoi((f32)d3y * 0.2f);

    /* sway wave: faster and wider with the head's speed and the wind */
    f32 dist = std_sqrtf(PSVECSquareDistance(&mHairPrevPos, headPos));
    f32 k = gabi::fmadds(dist, 0.65f, pow2) * 0.25f;
    if (k > 1.0f) {
        k = 1.0f;
    }
    s16 inc = (s16)gabi::ftoi(gabi::fmadds(k, 4096.0f, 1500.0f));
    s16 wave = (s16)(mHairWave + inc);
    mHairWave = wave;
    m92A = (s16)gabi::ftoi(k * 2280.0f * cM_scos(wave));
    m92C = (s16)gabi::ftoi(k * 3908.0f * cM_scos((s16)gabi::ftoi(gabi::fnmsubs(3.0f, (f32)inc, (f32)wave))));
    m92E = (s16)gabi::ftoi(k * 7568.0f * cM_scos((s16)gabi::ftoi(gabi::fnmsubs(6.0f, (f32)inc, (f32)wave))));
    mHairPrevPos.copy(*headPos);
}
VERIFY(0x022FC294, &daNpc_Yw1_c::setHairAngle);
