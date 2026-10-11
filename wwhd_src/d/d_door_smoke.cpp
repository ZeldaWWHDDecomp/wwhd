/** Door smoke callbacks and key construction. Local only. */
#include "d/d_door_smoke.h"
static const f32 l_smokeRate;
static const f32 l_smokeSpread;
static const f32 l_smokeScale;
static const f32 l_positiveWave;
static const f32 l_negativeWave;
static const f64 l_integerBias;
enum { ID_AK_JT_ELEMENTSMOKE00 = 0x2022 };

/* 0252B190 */
dDoor_smoke_c* dDoor_smoke_ct(dDoor_smoke_c* smoke) {
    // HD: compiler constructor supports allocation and initializes the callback first.
    if (!smoke) {
        smoke = static_cast<dDoor_smoke_c*>(dDoor_alloc(0x38));
        if (!smoke) return nullptr;
    }
    dDoor_smokeCallback_ct(&smoke->mSmokeCb, 1);
    smoke->m35 = 0; smoke->m34 = 0;
    return smoke;
}
/* 0252B1E8 */
void dDoor_smoke_c::smokeInit(dDoor_info_c* door) {
    // HD: this assignment copies three words, preserving float payloads.
    mPos = door->current.pos;
    mRot.y = door->shape_angle.y;
    s32 room = door->current.roomNo;
    DoorSmokeEmitter* emtr = reinterpret_cast<DoorSmokeEmitter*>(dComIfGp_particle_set(
        ID_AK_JT_ELEMENTSMOKE00, &mPos, &mRot, nullptr, 0xAA,
        reinterpret_cast<dPa_levelEcallBack*>(&mSmokeCb), room, nullptr, nullptr, nullptr));
    m34 = 0; m35 = 0;
    if (emtr) {
        f32 spread = l_smokeSpread, rate = l_smokeRate;
        emtr->mSpread = spread;
        f32 scale = l_smokeScale;
        emtr->mRate = rate;
        emtr->mGlobalParticleScale.z = scale; emtr->mGlobalDynamicsScale.x = scale;
        emtr->mGlobalParticleScale.y = scale; emtr->mGlobalDynamicsScale.y = scale;
        emtr->mGlobalParticleScale.x = scale; emtr->mGlobalDynamicsScale.z = scale;
    }
}
/* 0252B2D8 */
void dDoor_smoke_c::smokeProc(dDoor_info_c* door) {
    if (!m35) { m35 = 1; return; }
    u32 count = m34;
    f32 sign = count & 1 ? l_positiveWave : l_negativeWave;
    // HD: retain the signed integer conversion via its double bias and fused updates.
    f64 bias = l_integerBias;
    u64 bits = 0x4330000000000000ull | u32((count * 20) ^ 0x80000000u);
    f64 encoded; memcpy(&encoded, &bits, 8);
    f32 wave = f32(encoded - bias), x = mPos.x;
    m34 = u8(count + 1);
    wave = gabi::fmuls_ppc(wave, sign);
    f32 sz = door->mAngleVec.z;
    x = gabi::fmadds(wave, sz, x);
    f32 z = mPos.z;
    mPos.x = x;
    f32 sx = door->mAngleVec.x;
    z = gabi::fmadds(wave, sx, z);
    mPos.z = z;
}
/* 0252B3CC */
void dDoor_smoke_c::smokeEnd() { mSmokeCb.end(); }
/* 0252B3D0 */
dDoor_key2_c* dDoor_key2_ct(dDoor_key2_c* key) {
    // HD: embedded animation and phase objects are substantially larger than GameCube's.
    if (!key) {
        key = static_cast<dDoor_key2_c*>(dDoor_alloc(0xA0));
        if (!key) return nullptr;
    }
    key->mbEnabled = false; key->mpModel = nullptr;
    dDoor_frameCtrl_init(&key->mBckAnim.mFrameCtrl, 0);
    key->mBckAnim.mAnm.__vtbl = 0x1016E54C;
    dDoor_animationBinding_ct(&key->mBckAnim.mAnm.mBinding);
    key->mBckAnim.mAnm.mBindingSlots[2] = 0;
    key->mBckAnim.mAnm.mBindingSlots[1] = 0;
    key->mBckAnim.mAnm.mBindingSlots[0] = 0; key->mBckAnim.mpAnm = 0;
    key->mBckAnim.mAnm.__vtbl = 0x1004C53C;
    key->mBckAnim.mAnm.mBinding.__vtbl = 0x1016D820;
    key->mBckAnim.mAnm.mBinding.mpResource = 0;
    dDoor_phase_init(&key->mPhs, 8);
    key->mbIsBossDoor = 0; key->m20 = 0;
    return key;
}

/* WWHD glue (tools/verify/autoglue.py)
data l_smokeRate 1004C7BC
data l_smokeSpread 1004C7C0
data l_smokeScale 1004C7C4
data l_positiveWave 1004C7C8
data l_negativeWave 1004C7CC
data l_integerBias 1004C7D0
ext 0273AD10 void* dDoor_alloc(u32)
ext 025A5B18 void dDoor_smokeCallback_ct(dPa_smokeEcallBack*, u32)
ext 027F2BC0 void dDoor_frameCtrl_init(J3DFrameCtrl*, u32)
ext 027DA984 void dDoor_animationBinding_ct(DoorKeyAnimationBinding*)
ext 028F521C void dDoor_phase_init(request_of_phase_process_class*, u32)
*/
