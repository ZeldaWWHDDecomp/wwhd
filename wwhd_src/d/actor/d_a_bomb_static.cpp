/* Bomb - static utility methods.
 * GameCube reference: tww/src/d/d_a_bomb_static.cpp. HD changes marked below. */
#include "d/actor/d_a_bomb_static.h"
const gc_tu_data gc_tu = { .headerStatics = {0x104626EC, 0x101923D8} };
static u32 PrmAbstract(const fopAc_ac_c*, u32, u32);
const int L_daBomb_Version = 1;

/* 020CB648 */
s16 BombStatic::daBomb_c::getBombRestTime() {
    _prm_chk_version();
    return mRestTime;
}

/* 020CB678 */
u8 BombStatic::daBomb_c::getBombCheck_Flag() {
    _prm_chk_version();
    return field_0x6F1;
}

/* 020CB6A8 */
void BombStatic::daBomb_c::setBombCheck_Flag() {
    _prm_chk_version();
    field_0x6F1 = 1;
}

/* 020CB6DC */
void BombStatic::daBomb_c::setBombFire_ON() {
    _prm_chk_version();
    mBombFire = 1;
}

/* 020CB710 */
void BombStatic::daBomb_c::setBombNoHit() {
    _prm_chk_version();
    // HD: load all collision flags first; set the no-hit byte before the final store.
    u32 at = mSph.mObjAt.mSPrm;
    u32 tg = mSph.mObjTg.mSPrm;
    u32 co = mSph.mObjCo.mSPrm;
    mSph.mObjAt.mSPrm = at & ~1u;
    mSph.mObjTg.mSPrm = tg & ~1u;
    field_0x6F3 = 1;
    mSph.mObjCo.mSPrm = co & ~1u;
}

/* 020CB768 */
void BombStatic::daBomb_c::setBombOffCoSet() {
    _prm_chk_version();
    mSph.mObjCo.mSPrm = mSph.mObjCo.mSPrm & ~1u;
}

/* 020CB7A0 */
void BombStatic::daBomb_c::setBombOnCoSet() {
    _prm_chk_version();
    mSph.mObjCo.mSPrm = mSph.mObjCo.mSPrm | 1u;
}

/* 020CB7D8 */
void BombStatic::daBomb_c::setBombNoEff() {
    _prm_chk_version();
    // HD: reload each emitter after clearing its callback; slots may alias the emitter.
    BombEmitter_view* first = (BombEmitter_view*)mSmoke.mpEmitter.get();
    if (first) {
        first->setEmitterCallBackPtr(nullptr);
        first = (BombEmitter_view*)mSmoke.mpEmitter.get();
        first->becomeInvalidEmitter();
    }
    // HD: capture the second pointer before clearing the first slot.
    BombEmitter_view* second = (BombEmitter_view*)mSparks.mpEmitter.get();
    mSmoke.mpEmitter = nullptr;
    if (second) {
        second->setEmitterCallBackPtr(nullptr);
        second = (BombEmitter_view*)mSparks.mpEmitter.get();
        second->becomeInvalidEmitter();
    }
    mSparks.mpEmitter = nullptr;
}

/* 020CB860 */
void BombStatic::daBomb_c::setBombRestTime(s16 time) {
    _prm_chk_version();
    mRestTime = time;
}

/* 020CB89C */
void BombStatic::daBomb_c::setNoGravityTime(s16 time) {
    _prm_chk_version();
    mNoGravityTime = time;
}

/* 020CB8D8 */
static u32 daBomb_prm_make(u32 state, u32 cheap, u32 zeroAngle) {
    // HD: GHS accepts the original word arguments without bool normalization.
    return state | ((cheap & 0xFFFFu) << 16) | ((zeroAngle & 0x7FFFu) << 17) | 0x80000000u;
}

/* 020CB92C */
bool BombStatic::daBomb_c::chk_state(u32 state) {
    _prm_chk_version();
    return prm_get_state() == state;
}

/* 020CB978 */
void BombStatic::daBomb_c::change_state(u32 state) {
    // HD: store the new state before the version-check tail call.
    mParameters = (mParameters & 0xFFFFFF00u) | state;
    _prm_chk_version();
}

/* 020CB8F0 */
u32 BombStatic::daBomb_c::prm_get_state() {
    _prm_chk_version();
    return PrmAbstract(this, PRM_STATE_W, PRM_STATE_S);
}

/* 020CB98C */
bool BombStatic::daBomb_c::get_explode_instant() {
    return field_0x784 == 1;
}

/* 020CB9A0 */
bool BombStatic::daBomb_c::prm_get_cheapEff() {
    _prm_chk_version();
    return PrmAbstract(this, PRM_CHEAP_EFF_W, PRM_CHEAP_EFF_S) != 0;
}

/* 020CB9E4 */
bool BombStatic::daBomb_c::prm_get_angXZero() {
    _prm_chk_version();
    return PrmAbstract(this, PRM_ANGXZERO_W, PRM_ANGXZERO_S) != 0;
}

/* 020CB5FC */
u32 BombStatic::daBomb_c::prm_get_version() {
    return PrmAbstract(this, PRM_VERSION_W, PRM_VERSION_S);
}

/* 020CB608 */
void BombStatic::daBomb_c::_prm_chk_version() {
    if (prm_get_version() != L_daBomb_Version) {
        JUTAssertion::showAssert("d_a_bomb_static.cpp", 0xCE, "ver == L_daBomb_Version");
    }
}

/* 020CBA28 */
void BombStatic::daBomb2::Act_c::remove_fuse_effect() {
    mSmoke.deleteCallBack();
    mSparks.deleteCallBack();
}

/* 020CBA60 */
void BombStatic::daBomb2::Act_c::set_time(s32 time) {
    mBombTimer = time;
}

/* 020CBA68 */
s32 BombStatic::daBomb2::Act_c::get_time() {
    return mBombTimer;
}

/* 020CBA70 */
u8 BombStatic::daBomb2::Act_c::chk_eat() {
    return field_0x745;
}

/* 020CBA78 */
void BombStatic::daBomb2::Act_c::set_eat() {
    field_0x745 = 1;
}

/* 020CBA84 */
void BombStatic::daBomb2::Act_c::set_no_hit() {
    u32 tg = mSph.mObjTg.mSPrm;
    u32 co = mSph.mObjCo.mSPrm;
    u32 at = mSph.mObjAt.mSPrm;
    mSph.mObjTg.mSPrm = tg & ~1u;
    mSph.mObjCo.mSPrm = co & ~1u;
    mSph.mObjAt.mSPrm = at & ~1u;
}

/* 020CBAAC */
bool BombStatic::daBomb2::Act_c::chk_explode() {
    return mState == 2;
}

/* 020CBB54 */
static u32 PrmAbstract(const fopAc_ac_c* self, u32 width, u32 shift) {
    // HD: PowerPC shift counts wrap to six bits; 32..63 produce zero.
    u32 param = self->mParameters;
    width &= 63;
    shift &= 63;
    u32 left = width < 32 ? 1u << width : 0;
    u32 right = shift < 32 ? param >> shift : 0;
    return right & (left - 1u);
}

/* 020CBAC0 */
// HD: this translation unit initializes two common header statics.
static void __sinit_d_a_bomb_static_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* WWHD glue (tools/verify/autoglue.py)
str 1000AD00 "d_a_bomb_static.cpp"
str 1000ACE8 "ver == L_daBomb_Version"
ext 020C852C void daBomb2::FuseSmokeCB_c::deleteCallBack()
ext 020C8918 void daBomb2::FuseSparksCB_c::deleteCallBack()
*/
