/** Player base and matrix-follow particle callback (WWHD).
 * Reconstructed from the GameCube source and the HD unit.
 */
#include "bindings.h"
#include "d/actor/d_a_player_readable.h"
using PlayerBase = player_detail::daPy_py_c;
using namespace player_detail;
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
using GuestByte = be<u8>;
struct HeaderObject {
    GuestWord words[4];
};
static HeaderObject l_zeroObject;
static GuestFloat l_negativePi;
static GuestFloat l_positivePi;
static GuestByte l_firstObject;
static GuestByte l_secondObject;
static GuestWord l_zeroDestructor;
static GuestWord l_firstDestructor;
static GuestWord l_secondDestructor;
static GuestWord l_playerVtable;
static GuestWord l_quakePattern;

/* 023D4528 */
void player_detail::daPy_mtxFollowEcallBack_c::execute(Emitter *emitter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    JPASetRMtxTVecfromMtx(mpMtx.get(), &emitter->mGlobalRotation, &emitter->mGlobalTranslation);
}

/* 023D4538 */
void player_detail::daPy_mtxFollowEcallBack_c::end() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!mpEmitter) {
        return;
    }
    u32 flags = mpEmitter->mFlags;
    mpEmitter->mMaxFrame = -1;
    mpEmitter->mFlags = flags | 1;
    flags = mpEmitter->mFlags;
    mpEmitter->mFlags = flags & ~0x40u;
    mpEmitter->mpCallBack = nullptr;
    mpEmitter = nullptr;
}

// HD: retain the full register-width particle ID; GC declares u16.
/* 023D457C */
void *player_detail::daPy_mtxFollowEcallBack_c::makeEmitter(u32 particle, void *matrix,
                                                            cXyz *position, cXyz *scale) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    end();
    mpMtx = matrix;
    auto particles = player_play()->play.mParticle.get();
    return player_particle_set(particles, 1, particle, position, (u32)0, scale, 0xFF, this, -1,
                               nullptr, nullptr, (u32)0);
}

/* 023D460C */
void *player_detail::daPy_mtxFollowEcallBack_c::makeEmitterColor(u32 particle, void *matrix,
                                                                 cXyz *position, void *primary,
                                                                 void *environment) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    end();
    mpMtx = matrix;
    auto particles = player_play()->play.mParticle.get();
    return player_particle_set(particles, 1, particle, position, (u32)0, nullptr, 0xFF, this, -1,
                               primary, environment, (u32)0);
}

/* 023D4688 */
void player_detail::daPy_py_c::changePlayer(fopAc_ac_c *replacement) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (replacement == nullptr)
        return;
    s32 room = replacement->current.roomNo;
    s32 stay = dComIfGp_roomControl_getStayNo();
    if ((u32)stay != (u32)room)
        return;
    player_play()->play.mPlayerInfo[0].mpPlayer = replacement;
    auto camera = reinterpret_cast<Camera *>(dComIfGp_getCamera(0));
    camera->mpPlayerActor = replacement;
    auto attention = reinterpret_cast<Attention *>(&player_play()->play.mAttention);
    attention->mFlags = (u32)attention->mFlags | 0x80;
}

/* 023D46F4 */
void player_detail::daPy_py_c::setDoButtonQuake() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 flags = this->mNoResetFlg0;
    if (flags & daPyFlg0_UNK200000)
        return;
    this->mNoResetFlg0 = flags | daPyFlg0_UNK200000;
    this->mQuakeTimer = 60;
    auto vibration = &player_play()->play.mVibration;
    gabi::Local<cXyz> direction;
    direction->x = 0.0f;
    direction->y = 1.0f;
    direction->z = 0.0f;
    // HD: stores the pattern in read-only data; GC used a local word.
    player_StartQuake(vibration, &l_quakePattern, 0, 1, direction.get());
}

/* 023D4768 */
void player_detail::daPy_py_c::stopDoButtonQuake(s32 release) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s32 timer = this->mQuakeTimer;
    if (timer > 0) {
        timer = (s16)(timer - 1);
        this->mQuakeTimer = (s16)timer;
        if (timer == 0) {
            auto vibration = &player_play()->play.mVibration;
            player_StopQuake(vibration, -1);
        }
    }
    if (release != 0 && (s16)this->mQuakeTimer == 0)
        this->mNoResetFlg0 = (u32)this->mNoResetFlg0 & ~(u32)daPyFlg0_UNK200000;
}

// HD: nullable allocation and the explicit guest vtable are part of this constructor ABI.
/* 023D47E8 */
static PlayerBase *daPy_py_c__ct(PlayerBase *player) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (player == nullptr)
        player = static_cast<PlayerBase *>(operator_new(sizeof(PlayerBase)));
    if (player != nullptr) {
        fopAc_ac_c_ct(player);
        player->__vtbl = gabi::ea(&l_playerVtable);
    }
    return player;
}

// HD: deleting-destructor flag bit 0 controls release of the guest allocation.
/* 023D483C */
static void daPy_py_c__dt(PlayerBase *player, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (player != nullptr) {
        player_actor_destructor(player, 0);
        if (flags & 1)
            operator_delete(player);
    }
}

// HD: mWindVelocity is the shared layout name for tww field_0x2f8.
// Keep the five guest temporaries and float rounding order of the original expansion.
/* 023D4890 */
void player_detail::daPy_py_c::objWindHitCheck(dCcD_Cyl *cylinder) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> target, radial, difference, scaled, radialScaled;
    target->x = 0.0f;
    target->y = 0.0f;
    target->z = 0.0f;
    f32 maxStep = 3.0f;
    if (cylinder->ChkTgHit() != 0) {
        auto hit = static_cast<dCcD_GObjInf *>(cylinder->GetTgHitObj());
        if (hit != nullptr && ((u32)hit->mAtType & AT_TYPE_WIND)) {
            f32 y = (f32)cylinder->mGObjTg.mRVec.y;
            f32 z = (f32)cylinder->mGObjTg.mRVec.z;
            target->y = y;
            radial->z = z;
            f32 x = (f32)cylinder->mGObjTg.mRVec.x;
            radial->y = 0.0f;
            target->x = x;
            radial->x = x;
            target->z = z;
            f32 speed = PSVECSquareMag(radial.get());
            speed = std_sqrtf(speed);
            maxStep = 1.0f;
            f32 limit = 30.0f;
            if (speed < maxStep) {
                cXyz_mi(&current.pos, difference.get(), cylinder->GetTgHitPosP());
                cXyz_ml(difference.get(), scaled.get(), limit);
                x = scaled->x;
                z = scaled->z;
                radialScaled->y = 0.0f;
                radialScaled->x = x;
                target->z = z;
                y = scaled->y;
                radialScaled->z = z;
                target->y = y;
                target->x = x;
                speed = PSVECSquareMag(radialScaled.get());
                speed = std_sqrtf(speed);
            }
            if (speed > limit)
                PSVECScale(target.get(), target.get(), limit / speed);
        }
    }
    cLib_addCalc(&this->mWindVelocity.x, (f32)target->x, 0.5f, maxStep, 0.5f);
    cLib_addCalc(&this->mWindVelocity.z, (f32)target->z, 0.5f, maxStep, 0.5f);
    f32 vx = this->mWindVelocity.x;
    f32 x = this->current.pos.x;
    f32 z = this->current.pos.z;
    f32 vz = this->mWindVelocity.z;
    this->current.pos.x = x + vx;
    this->current.pos.z = z + vz;
}

/* 023D4A54 */
static void daPy_header_static() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    l_zeroObject.words[2] = 0;
    l_zeroObject.words[0] = 0;
    l_zeroObject.words[3] = 0;
    l_zeroObject.words[1] = 0;
    __register_global_object(gabi::ea(&l_zeroDestructor));
    l_negativePi = -3.141592741012573242f;
    l_positivePi = 3.141592741012573242f;
    player_firstStaticConstructor(&l_firstObject);
    __register_global_object(gabi::ea(&l_firstDestructor));
    player_secondStaticConstructor(&l_secondObject);
    __register_global_object(gabi::ea(&l_secondDestructor));
}

/* 023D4AE8 */
void player_detail::daPy_mtxFollowEcallBack_c::executeAfter(Emitter *emitter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4AEC */
void player_detail::daPy_mtxFollowEcallBack_c::draw(Emitter *emitter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// HD: an additional empty callback hook follows draw in the emitter vtable.
/* 023D4AF0 */
void player_detail::daPy_mtxFollowEcallBack_c::endHook() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4B4C */
void player_detail::daPy_py_c::onDekuSpReturnFlg(u8 mode) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4B90 */
void player_detail::daPy_py_c::voiceStart(u32 sound) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4B94 */
void player_detail::daPy_py_c::setOutPower(f32 power, s16 angle, int type) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4B98 */
void player_detail::daPy_py_c::onFrollCrashFlg(u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4BA4 */
void player_detail::daPy_py_c::setPlayerPosAndAngle(cXyz *pos, s16 angle) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// GC overload names use suffixes here to keep autoglue registration unambiguous.
/* 023D4BA8 */
void player_detail::daPy_py_c::setPlayerPosAndAngleAngles(cXyz *pos, csXyz *angle) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4BAC */
void player_detail::daPy_py_c::setPlayerPosAndAngleMatrix(Mtx34 *matrix) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 023D4AF4 */
void player_detail::daPy_mtxFollowEcallBack_c::setup(Emitter *emitter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mpEmitter = emitter;
}

/* 023D4B04 */
s32 player_detail::daPy_py_c::getTactTimerCancel() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B0C */
s32 player_detail::daPy_py_c::checkPlayerGuard() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B14 */
fopAc_ac_c *player_detail::daPy_py_c::getGrabMissActor() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return nullptr;
}

/* 023D4B1C */
s32 player_detail::daPy_py_c::checkPlayerFly() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B24 */
s32 player_detail::daPy_py_c::checkFrontRoll() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B2C */
s32 player_detail::daPy_py_c::checkBottleSwing() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B34 */
s32 player_detail::daPy_py_c::checkCutCharge() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B3C */
s32 player_detail::daPy_py_c::getBokoFlamePos(cXyz *position) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B44 */
s32 player_detail::daPy_py_c::checkTactWait() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B50 */
s32 player_detail::daPy_py_c::checkComboCutTurn() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B70 */
s32 player_detail::daPy_py_c::checkGrabBarrel() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B78 */
s32 player_detail::daPy_py_c::checkPlayerNoDraw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B80 */
s32 player_detail::daPy_py_c::checkRopeTag() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B88 */
s32 player_detail::daPy_py_c::checkRopeReadyAnime() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4B9C */
s32 player_detail::daPy_py_c::setHookshotCarryOffset(u32 actorID, const cXyz *offset) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4BB0 */
s32 player_detail::daPy_py_c::setThrowDamage(cXyz *position, s16 angle, f32 speed, f32 speedY,
                                             int type) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 023D4AFC */
s32 player_detail::daPy_py_c::getTactMusic() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return -1;
}

/* 023D4B58 */
s32 player_detail::daPy_py_c::getItemID() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return -1;
}

/* 023D4B60 */
s32 player_detail::daPy_py_c::getThrowBoomerangID() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return -1;
}

/* 023D4B68 */
s32 player_detail::daPy_py_c::getGrabActorID() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return -1;
}

// clang-format off
/* WWHD glue:
data l_zeroObject 1046CC94
data l_negativePi 1046CC88
data l_positivePi 1046CC8C
data l_firstObject 1046CC90
data l_secondObject 1046CC91
data l_zeroDestructor 101CEAD8
data l_firstDestructor 101CEAE4
data l_secondDestructor 101CEAF0
data l_playerVtable 10034830
data l_quakePattern 100347D4
ext 028249B0 void JPASetRMtxTVecfromMtx(void*, void*, void*)
ext 025A847C void* player_particle_set(void*, u32, u32, cXyz*, u32, cXyz*, u32, void*, s32, void*, void*, u32)
ext 025CB4E0 void player_StartQuake(void*, void*, s32, s32, cXyz*)
ext 025CB610 void player_StopQuake(void*, s32)
ext 025D50BC void player_actor_destructor(fopAc_ac_c*, u32)
ext 0200ECD4 void cLib_addCalc(be<f32>*, f32, f32, f32, f32)
ext 028ED6F8 void player_firstStaticConstructor(void*)
ext 028EAB2C void player_secondStaticConstructor(void*)
*/
