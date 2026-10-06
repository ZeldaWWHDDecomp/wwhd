/* Sea ships/rafts, HD behavior checked against cking.rpx. */
#include "d/actor/d_a_obj_ikada.h"
namespace ikada {
void Actor::modeProc(s32 proc, s32 mode) {
    WWHD_FUNC(0x02360080, void, this, proc, mode);
    u32 entry = 0x1002B3E0;
    if (proc == 0) {
        entry += (u32)mode * 0x14;
        mCurMode = mode;
    } else if (proc == 1)
        entry += (u32)(s32)mCurMode * 0x14 + 8;
    else
        return;
    s16 index = gabi::load<s16>(entry + 2), adjust = gabi::load<s16>(entry);
    u32 self = gabi::ea(this) + (s32)adjust, target;
    if (index < 0)
        target = gabi::load<u32>(entry + 4);
    else {
        s16 offset = gabi::load<s16>(entry + 6);
        u32 vt = gabi::load<u32>(self + (s32)offset);
        target = gabi::load<u32>(vt + 8 * (s32)index + 4);
    }
    gabi::call(target, self);
}
VERIFY(0x02360080, &Actor::modeProc);
void Actor::getArg() {
    WWHD_FUNC(0x02361218, void, this);
    u32 param = gabi::load<u32>(gabi::ea(this) + 0xB0);
    u32 type = param & 15;
    s16 homeX = home.angle.x;
    mType = type;
    if (type == 4) {
        mPathId = param >> 16;
        return;
    }
    m0294 = (param >> 4) & 63;
    m0298 = (param >> 10) & 255;
    m029C = (u8)homeX;
    mPathId = (u32)(s32)homeX >> 8;
    m02A0 = (param >> 18) & 255;
}
VERIFY(0x02361218, &Actor::getArg);
void Actor::ride(fopAc_ac_c *actor) {
    WWHD_FUNC(0x02360380, void, this, actor);
    if (mType != 1)
        return;
    u32 play = gabi::ea(dComIfGp_get());
    if (gabi::ea(actor) == gabi::load<u32>(play + 0x5B34))
        mbIsLinkRiding = 1;
    if (!actor || gabi::load<s16>(gabi::ea(actor) + 8) != 0x126)
        return;
    if (gabi::call<s32>(0x020CB648, actor) > 1)
        return;
    mLinkRockAmplitude = 300;
    modeProc(0, 12);
}
VERIFY(0x02360380, &Actor::ride);
void rideCallback(void *bg, Actor *actor, fopAc_ac_c *rider) {
    WWHD_FUNC(0x02360414, void, bg, actor, rider);
    actor->ride(rider);
}
VERIFY(0x02360414, rideCallback);
void Actor::HandleRight() {
    WWHD_FUNC(0x02363F30, void, this);
    f32 speed = gabi::load<f32>(0x1046A21C);
    s16 end = mBckAnm.mFrameCtrl.mEnd;
    s32 frame = (s32)((u32)(s32)mHandleFrame + (u32)gabi::ftoi(speed));
    mHandleFrame = frame;
    if (!((f32)frame < (f32)end))
        mHandleFrame = gabi::ftoi((f32)(s16)mBckAnm.mFrameCtrl.mStart);
}
VERIFY(0x02363F30, &Actor::HandleRight);
void Actor::HandleLeft() {
    WWHD_FUNC(0x02364098, void, this);
    f32 speed = gabi::load<f32>(0x1046A21C);
    s16 start = mBckAnm.mFrameCtrl.mStart;
    s32 frame = (s32)((u32)(s32)mHandleFrame - (u32)gabi::ftoi(speed));
    mHandleFrame = frame;
    if (!((f32)frame > (f32)start))
        mHandleFrame = gabi::ftoi((f32)(s16)mBckAnm.mFrameCtrl.mEnd);
}
VERIFY(0x02364098, &Actor::HandleLeft);
void Actor::modeCraneUp() {
    WWHD_FUNC(0x02363FE0, void, this);
    if (mRopeCount > 15) {
        HandleRight();
        gabi::call(0x02363D28, this, -1, 0);
    } else
        modeProc(0, 2);
}
VERIFY(0x02363FE0, &Actor::modeCraneUp);
void Actor::modeCraneUpWaitInit() {
    WWHD_FUNC(0x02364034, void, this);
    mTimer = gabi::load<s16>(0x1046A220);
}
VERIFY(0x02364034, &Actor::modeCraneUpWaitInit);
void Actor::modeCraneUpWait() {
    WWHD_FUNC(0x02364044, void, this);
    if (!gabi::call<s32>(0x0211D2F8, &mTimer))
        modeProc(0, 6);
}
VERIFY(0x02364044, &Actor::modeCraneUpWait);
void Actor::modeCraneDownInit() {
    WWHD_FUNC(0x0236408C, void, this);
    mCraneAmplitude = 150;
}
VERIFY(0x0236408C, &Actor::modeCraneDownInit);
void Actor::modeCraneDownWaitInit() {
    WWHD_FUNC(0x02364204, void, this);
    mTimer = gabi::load<s16>(0x1046A220);
}
VERIFY(0x02364204, &Actor::modeCraneDownWaitInit);
void Actor::modeCraneDownWait() {
    WWHD_FUNC(0x02364214, void, this);
    if (!gabi::call<s32>(0x0211D2F8, &mTimer))
        modeProc(0, 3);
}
VERIFY(0x02364214, &Actor::modeCraneDownWait);
void Actor::modeCraneTurnInit() {
    WWHD_FUNC(0x0236425C, void, this);
    mCraneAmplitude = 150;
    speedF = 0.0f;
}
VERIFY(0x0236425C, &Actor::modeCraneTurnInit);
void Actor::modeCraneResetInit() {
    WWHD_FUNC(0x0236433C, void, this);
    mCraneAmplitude = 150;
    speedF = 0.0f;
}
VERIFY(0x0236433C, &Actor::modeCraneResetInit);
void Actor::modeCraneWaitInit() {
    WWHD_FUNC(0x0236440C, void, this);
    mTimer = 600;
}
VERIFY(0x0236440C, &Actor::modeCraneWaitInit);
void Actor::modeCraneWait() {
    WWHD_FUNC(0x02364418, void, this);
    if (!gabi::call<s32>(0x0211D2F8, &mTimer))
        modeProc(0, 1);
}
VERIFY(0x02364418, &Actor::modeCraneWait);
void Actor::modePathMoveInit() {
    WWHD_FUNC(0x02364460, void, this);
    mDirection = cM_rndF(1.0f) < 0.5 ? 0 : 1;
}
VERIFY(0x02364460, &Actor::modePathMoveInit);
void Actor::modePathMoveTerryInit() {
    WWHD_FUNC(0x023647C4, void, this);
    mTimer = 10;
}
VERIFY(0x023647C4, &Actor::modePathMoveTerryInit);
void Actor::modeStopTerryInit() {
    WWHD_FUNC(0x02364C60, void, this);
    mTimer = 210;
}
VERIFY(0x02364C60, &Actor::modeStopTerryInit);
void Actor::modeCraneDown() {
    WWHD_FUNC(0x02364148, void, this);
    gabi::call(0x0200F428, &mCraneAmplitude, 0, 10, 10);
    if (mCraneAmplitude < 20) {
        HandleLeft();
        if (mRopeCount < gabi::load<s16>(0x1046A21A))
            gabi::call(0x02363D28, this, 1, 0);
        else
            modeProc(0, 7);
    } else
        mCraneAnimPhase = (s16)((s32)mCraneAnimPhase + gabi::load<s16>(0x1047BD4C) + 0x1830);
}
VERIFY(0x02364148, &Actor::modeCraneDown);
void Actor::modeCraneTurn() {
    WWHD_FUNC(0x02364274, void, this);
    gabi::call(0x0200F428, &mCraneAmplitude, 0, 10, 10);
    if (mCraneAmplitude < 20) {
        HandleRight();
        s16 target = mDirection == 1 ? 0x4000 : -0x4000;
        mCraneTarget = target;
        gabi::call(0x0200F428, &mCraneAngle, target, 10, 0x100);
        if (gabi::call<s32>(0x0200FAAC, (s16)mCraneAngle, (s16)mCraneTarget) <= 0x100)
            modeProc(0, 4);
    } else
        mCraneAnimPhase = (s16)((s32)mCraneAnimPhase + gabi::load<s16>(0x1047BD4C) + 0x1830);
}
VERIFY(0x02364274, &Actor::modeCraneTurn);
void Actor::modeCraneReset() {
    WWHD_FUNC(0x02364354, void, this);
    gabi::call(0x0200F428, &mCraneAmplitude, 0, 10, 10);
    if (mCraneAmplitude < 20) {
        HandleLeft();
        mCraneTarget = 0;
        gabi::call(0x0200F428, &mCraneAngle, 0, 10, 0x100);
        if (gabi::call<s32>(0x0200FAAC, (s16)mCraneAngle, (s16)mCraneTarget) <= 0x100)
            modeProc(0, 8);
    } else
        mCraneAnimPhase = (s16)((s32)mCraneAnimPhase + gabi::load<s16>(0x1047BD4C) + 0x1830);
}
VERIFY(0x02364354, &Actor::modeCraneReset);
void Actor::modeWait() {
    WWHD_FUNC(0x02363E98, void, this);
    if (mType != 4)
        return;
    s16 target = gabi::load<s16>(0x1046A218);
    if (target) {
        if (mRopeCount > target)
            gabi::call(0x02363D28, this, -1, 0);
        else if (mRopeCount < target)
            gabi::call(0x02363D28, this, 1, 0);
    }
    if (gabi::load<s16>(0x1047BD50))
        modeProc(0, 1);
}
VERIFY(0x02363E98, &Actor::modeWait);
void Actor::modeStop() {
    WWHD_FUNC(0x02364668, void, this);
    if (gabi::load<u8>(0x1046A202))
        HandleRight();
    if (gabi::load<u8>(0x1046A201))
        HandleLeft();
    if (mPreviousMode == 8) {
        mVelocityFwdTarget = 0.0f;
        if (mPathId != 255)
            gabi::call(0x023644B0, this);
    }
    s32 count = mRopeCount;
    if (!mStopRequest) {
        if (count <= 15) {
            HandleLeft();
            gabi::call(0x02363D28, this, 1, 0);
            return;
        }
        s32 previous = mPreviousMode;
        modeProc(0, previous == 9 ? 8 : previous == 7 ? 3 : previous);
    } else if (count > 2) {
        HandleRight();
        gabi::call(0x02363D28, this, -2, 0);
        if (mRopeCount < 2)
            mRopeCount = 2;
    }
}
VERIFY(0x02364668, &Actor::modeStop);
void Actor::setCollision() {
    WWHD_FUNC(0x023647D0, void, this);
    if (mbIsLinkRiding)
        return;
    u32 play = gabi::ea(dComIfGp_get());
    if (gabi::load<u32>(play + 0x5CD8) & 0x4000)
        return;
    gabi::call(0x02018C8C, gabi::ea(this) + 0x15F0, gabi::load<f32>(0x1046A298) * (f32)scale.x);
    gabi::call(0x02018D40, gabi::ea(this) + 0x15F0, &current.pos);
    play = gabi::ea(dComIfGp_get());
    gabi::call(0x0200E240, play + 0x26A4, &mSph);
}
VERIFY(0x023647D0, &Actor::setCollision);
void Actor::pathMove() {
    WWHD_FUNC(0x023644B0, void, this);
    gabi::call(0x0200ED84, &speedF, (f32)mVelocityFwdTarget, 0.1f, 2.0f);
    gabi::call(0x02587D24, &mPathPosTarget, &mCurPathPoint, (u32)mpPath, 0x02360368, this,
               (f32)speedF);
    gabi::call(0x0200F268, &current.pos, &mPathPosTarget, gabi::load<f32>(0x1047BCD0) + 0.01f,
               (f32)speedF);
    if (speedF != 0.0f && mVelocityFwdTarget != 0.0f) {
        s32 target = gabi::call<s32>(0x0200F93C, &current.pos, &mPathPosTarget);
        gabi::call(0x0200F428, &shape_angle.y, target, 8, 0x100);
    }
}
VERIFY(0x023644B0, &Actor::pathMove);
void Actor::modePathMove() {
    WWHD_FUNC(0x02364578, void, this);
    if (gabi::load<u8>(0x1046A202))
        HandleRight();
    if (gabi::load<u8>(0x1046A201))
        HandleLeft();
    if (gabi::load<u8>(0x1046A204) || mPathId == 255)
        return;
    if (mbCraneMode) {
        gabi::Local<cXyz> delta, horizontal;
        gabi::call(0x0201ADE0, &mCurPathP1, delta.get(), &current.pos);
        horizontal->set(delta->x, 0.0f, delta->z);
        f32 length = gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, horizontal.get()));
        if (length < 50.0f) {
            mVelocityFwdTarget = 1.0f;
            pathMove();
            return;
        }
    }
    mVelocityFwdTarget = 8.0f;
    pathMove();
}
VERIFY(0x02364578, &Actor::modePathMove);
s32 Actor::checkTgHit() {
    WWHD_FUNC(0x02364854, s32, this);
    if (mbIsLinkRiding) {
        u32 player = gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5B2C);
        if (gabi::load<u32>(player + 0x3C0) & 0x20000) {
            gabi::Local<cXyz> pos, delta, horizontal;
            pos->set(gabi::load<f32>(player + 0x3E4), gabi::load<f32>(player + 0x3E8),
                     gabi::load<f32>(player + 0x3EC));
            gabi::call(0x0201ADE0, pos.get(), delta.get(), &current.pos);
            horizontal->set(delta->x, 0.0f, delta->z);
            f32 length = gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, horizontal.get()));
            if (length < 1000.0f) {
                mLinkRockAmplitude = 200;
                modeProc(0, 11);
                return 1;
            }
        }
    }
    gabi::call(0x02515E50, gabi::ea(this) + 0x1620);
    if (!gabi::call<s32>(0x0211D2F8, &mHitTimer)) {
        u32 hit = gabi::call<u32>(0x02516300, &mSph);
        gabi::call(0x02516300, &mSph);
        if (hit && gabi::load<u32>(hit + 0x10) == 0x20) {
            mHitTimer = 60;
            mLinkRockAmplitude = 300;
            modeProc(0, 12);
            return 1;
        }
    }
    return 0;
}
VERIFY(0x02364854, &Actor::checkTgHit);
s32 nodeControlCallback(void *node, s32 timing) {
    WWHD_FUNC(0x02360038, s32, node, timing);
    if (!timing) {
        u32 model = gabi::load<u32>(0x104B462C), actor = gabi::load<u32>(model + 0xB8);
        if (actor)
            gabi::call(0x0235FE84, actor, node, model);
    }
    return 1;
}
VERIFY(0x02360038, nodeControlCallback);
s32 pathMoveCallback(cXyz *pos, cXyz *point, cXyz *path, Actor *actor) {
    WWHD_FUNC(0x02360368, s32, pos, point, path, actor);
    return gabi::call<s32>(0x0236012C, actor, pos, point, path);
}
VERIFY(0x02360368, pathMoveCallback);
s32 createHeapCallback(Actor *actor) {
    WWHD_FUNC(0x02361000, s32, actor);
    return gabi::call<s32>(0x02360C64, actor);
}
VERIFY(0x02361000, createHeapCallback);
s32 Create(Actor *actor) {
    WWHD_FUNC(0x02361D54, s32, actor);
    return gabi::call<s32>(0x02361C8C, actor);
}
VERIFY(0x02361D54, Create);
s32 Delete(Actor *actor) {
    WWHD_FUNC(0x02361E3C, s32, actor);
    return gabi::call<s32>(0x02361D58, actor);
}
VERIFY(0x02361E3C, Delete);
s32 Execute(Actor *actor) {
    WWHD_FUNC(0x02363164, s32, actor);
    return gabi::call<s32>(0x02362C54, actor);
}
VERIFY(0x02363164, Execute);
s32 Draw(Actor *actor) {
    WWHD_FUNC(0x02363D24, s32, actor);
    return gabi::call<s32>(0x02363B78, actor);
}
VERIFY(0x02363D24, Draw);
void hioDestructor(void *self, s32 flags) {
    WWHD_FUNC(0x0236513C, void, self, flags);
    if (self && (flags & 1))
        gabi::call(0x0273AF40, self);
}
VERIFY(0x0236513C, hioDestructor);
cXyz *vectorConstructor(cXyz *self) {
    WWHD_FUNC(0x02365150, cXyz *, self);
    return self ? self : gabi::call<cXyz *>(0x0273AD10, 12);
}
VERIFY(0x02365150, vectorConstructor);
s32 IsDelete(Actor *actor) {
    WWHD_FUNC(0x0236517C, s32, actor);
    return 1;
}
VERIFY(0x0236517C, IsDelete);
void modeWaitInit(Actor *actor) {
    WWHD_FUNC(0x02365184, void, actor);
}
VERIFY(0x02365184, modeWaitInit);
void modeCraneUpInit(Actor *actor) {
    WWHD_FUNC(0x02365188, void, actor);
}
VERIFY(0x02365188, modeCraneUpInit);
void modeStopInit(Actor *actor) {
    WWHD_FUNC(0x0236518C, void, actor);
}
VERIFY(0x0236518C, modeStopInit);
void modeStopBombTerryInit(Actor *actor) {
    WWHD_FUNC(0x02365190, void, actor);
}
VERIFY(0x02365190, modeStopBombTerryInit);
void emptyVirtual(Actor *actor) {
    WWHD_FUNC(0x02365248, void, actor);
}
VERIFY(0x02365248, emptyVirtual);
s32 Actor::pathAdvance(cXyz *position, cXyz *previous, cXyz *next) {
    WWHD_FUNC(0x0236012C, s32, this, position, previous, next);
    u32 path = mpPath;
    s32 pointIndex = mCurPathPoint;
    s32 index = pointIndex < (s32)gabi::load<u16>(path) - 1 ? (s8)(pointIndex + 1) : 0;
    u32 points = gabi::load<u32>(path + 8);
    mbCraneMode = gabi::load<u8>(points + (u32)index * 16 + 3) != 255;
    f32 y = current.pos.y;
    mCurPathP0.copy(*previous);
    mCurPathP0.y = y;
    mCurPathP1.copy(*next);
    mCurPathP1.y = y;
    gabi::Local<cXyz> direction, delta, horizontal, delta2, horizontal2;
    gabi::call(0x0201ADE0, &mCurPathP1, direction.get(), &mCurPathP0);
    if (gabi::call<s32>(0x0201B47C, direction.get())) {
        s32 angle = gabi::call<s32>(0x020195B0, (f32)direction->x, (f32)direction->z);
        u16 difference = gabi::call<u32>(0x0200F378, &current.angle.y, angle, 8, 0x200, 8);
        f32 rate =
            (f32)speedF * __builtin_fabsf(gabi::load<f32>(0x104A44FC + 8 * (difference >> 3)));
        gabi::call(0x0200F764, position, &mCurPathP1, rate);
        gabi::call(0x0201ADE0, position, delta.get(), &mCurPathP1);
        horizontal->set(delta->x, 0.0f, delta->z);
        f32 length = gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, horizontal.get()));
        f32 threshold = rate * (gabi::load<f32>(0x1047BCD8) + 1.0f);
        if (!(length < threshold)) {
            gabi::call(0x0201ADE0, position, delta2.get(), &mCurPathP1);
            horizontal2->set(delta2->x, 0.0f, delta2->z);
            if (gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, horizontal2.get())) != 0.0f)
                return 0;
        }
        if (mbCraneMode)
            modeProc(0, 5);
    }
    return 1;
}
VERIFY(0x0236012C, &Actor::pathAdvance);
s32 Actor::draw() {
    WWHD_FUNC(0x02363B78, s32, this);
    if (gabi::load<u8>(0x1046A200))
        gabi::call(0x02363168, this);
    u32 light = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, light, 0, &current.pos, gabi::ea(this) + 0x110);
    light = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, light, mpModel.get(), gabi::ea(this) + 0x110);
    u32 play = gabi::ea(dComIfGp_get());
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
    play = gabi::ea(dComIfGp_get());
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
    u32 model = gabi::ea(mpModel.get());
    if (mType == 4) {
        gabi::call(0x025E86B8, &mBckAnm, gabi::load<u32>(model + 0xAC),
                   (f32)mBckAnm.mFrameCtrl.mFrame);
        gabi::call(0x025E2DE0, mpModel.get(), 0);
        u32 data = gabi::load<u32>(gabi::ea(mpModel.get()) + 0xAC);
        gabi::store<u32>(gabi::load<u32>(data + 8) + 0x14, 0);
    } else
        gabi::call(0x025E2DE0, mpModel.get(), 0);
    play = gabi::ea(dComIfGp_get());
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
    play = gabi::ea(dComIfGp_get());
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
    gabi::call(0x02363374, this);
    if (mType == 4) {
        if (mRopeCount >= 2) {
            gabi::call(0x025EC62C, gabi::ea(this) + 0x934, (u16)(s32)mRopeCount, 0x101CA4D8, 0,
                       gabi::ea(this) + 0x110, 5.0f);
            play = gabi::ea(dComIfGp_get());
            u32 vt = gabi::load<u32>(gabi::ea(this) + 0xA64);
            s32 index = gabi::call<s32>(gabi::load<u32>(vt + 0x14), gabi::ea(this) + 0x934);
            gabi::call(0x025EDD04, play + 0x5FB4 + (u32)index * 0x9C, gabi::ea(this) + 0x934);
        }
        light = gabi::call<u32>(0x02555D0C);
        gabi::call(0x02562F5C, light, mpRopeEnd.get(), gabi::ea(this) + 0x110);
        gabi::call(0x025E2DE0, mpRopeEnd.get(), 0);
        if (mType == 4)
            gabi::call(0x025BED80, 0x83, this, 1.0f, 1.0f, 1.0f);
    }
    return 1;
}
VERIFY(0x02363B78, &Actor::draw);
void actorDestructor(Actor *actor, s32 flags) {
    WWHD_FUNC(0x02365194, void, actor, flags);
    if (!actor)
        return;
    u32 self = gabi::ea(actor);
    gabi::call(0x02515860, self + 0x1604, 2);
    gabi::call(0x02515AE8, self + 0x14D8, 2);
    gabi::call(0x025EB8B8, self + 0x934, 2);
    gabi::call(0x02018034, self + 0x8F0, 2);
    gabi::store<u32>(self + 0x738, 0x1002B334);
    gabi::store<u32>(self + 0x72C, 0x1002B344);
    gabi::call(0x024EFD9C, self + 0x718, 0);
    gabi::call(0x027F3628, self + 0x664, 0);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
        gabi::call(0x0273AF40, actor);
}
VERIFY(0x02365194, actorDestructor);
void Actor::incRopeCount(s32 increment, s32 minimum) {
    WWHD_FUNC(0x02363D28, void, this, increment, minimum);
    s32 old = mRopeCount, newCount = (s32)((u32)old + (u32)increment);
    u32 velocity = gabi::ea(this) + 0xAC0 + (u32)old * 12;
    u32 positions = gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xAB8));
    if (newCount >= 200)
        newCount = 200;
    else if (newCount < minimum)
        newCount = minimum;
    s32 delta = (s32)((u32)newCount - (u32)old);
    u32 offset = 0x1046A2E8;
    if (delta > 0) {
        gabi::Local<cXyz> shift;
        gabi::call(0x0201AE48, offset, shift.get(), (f32)delta);
        s32 i = 0;
        old = mRopeCount;
        while (i < old) {
            gabi::call(0x028E8D88, positions, shift.get(), positions);
            old = mRopeCount;
            ++i;
            positions += 12;
        }
    } else
        positions += (u32)old * 12;
    old = mRopeCount;
    if (old < newCount) {
        gabi::Local<cXyz> result;
        for (s32 remaining = (s32)((u32)newCount - (u32)old); remaining; --remaining) {
            for (u32 i = 0; i < 12; i += 4)
                gabi::store<u32>(velocity + i, gabi::load<u32>(offset + i));
            if (mRopeCount != 0) {
                gabi::call(0x0201ADE0, positions - 12, result.get(), offset);
                for (u32 i = 0; i < 12; i += 4)
                    gabi::store<u32>(positions + i, gabi::load<u32>(gabi::ea(result.get()) + i));
            }
            positions += 12;
            velocity += 12;
        }
    }
    mRopeCount = (s16)newCount;
}
VERIFY(0x02363D28, &Actor::incRopeCount);
s32 Actor::remove() {
    WWHD_FUNC(0x02361D58, s32, this);
    gabi::call(0x025204C8, &mPhase, 0x1002B808);
    gabi::call(0x025A92C0, &mWaveRCallback);
    gabi::call(0x025A92C0, &mWaveLCallback);
    gabi::call(0x025A99B8, &mSplashCallback);
    gabi::call(0x025A9E38, &mTrackCallback);
    gabi::call(0x025A9270, &mRippleCallback);
    u32 vt = gabi::load<u32>(gabi::ea(this) + 0x5CC);
    gabi::call(gabi::load<u32>(vt + 0x44), &mFireCallback);
    vt = gabi::load<u32>(gabi::ea(this) + 0x59C);
    gabi::call(gabi::load<u32>(vt + 0x44), &mBombSmokeCallback);
    gabi::call(0x025E1B34, &mSePos);
    if (mType == 3 || mType == 1)
        gabi::call(0x0255A374, &mPLight);
    if (gabi::load<u32>(gabi::ea(this) + 0xF4)) {
        u32 bg = mpBgW;
        if (bg && gabi::load<u32>(bg) < 0x100) {
            u32 play = gabi::ea(dComIfGp_get());
            gabi::call(0x020087EC, play + 0x12A0, (u32)mpBgW);
        }
    }
    return 1;
}
VERIFY(0x02361D58, &Actor::remove);
void Actor::modeStopBombTerry() {
    WWHD_FUNC(0x02364E10, void, this);
    setCollision();
    if (!checkTgHit()) {
        mVelocityFwdTarget = 0.0f;
        if (mPathId != 255)
            pathMove();
    }
}
VERIFY(0x02364E10, &Actor::modeStopBombTerry);
void *hioConstructor(void *self) {
    WWHD_FUNC(0x02364E6C, void *, self);
    if (!self) {
        self = gabi::call<void *>(0x0273AD10, 0xAC);
        if (!self)
            return nullptr;
    }
    u32 p = gabi::ea(self);
    gabi::store<u32>(p, 0x1002B364);
    for (u32 off : {4u, 5u, 6u, 7u, 8u})
        gabi::store<u8>(p + off, 0);
    gabi::store<s16>(p + 0x1C, 0);
    gabi::store<s16>(p + 0x1E, 100);
    gabi::store<s16>(p + 0x24, 60);
    gabi::store<s16>(p + 0x94, 3000);
    gabi::store<s16>(p + 0x96, 6000);
    gabi::store<s16>(p + 0x98, 1000);
    gabi::store<s16>(p + 0x9A, 1000);
    const struct {
        u32 offset;
        f32 value;
    } defaults[] = {
        {0xC, 0},     {0x10, 0},    {0x14, 0},   {0x18, 0},   {0x20, 1.5f}, {0x28, 100},
        {0x2C, 900},  {0x30, 300},  {0x34, 660}, {0x38, 20},  {0x3C, -180}, {0x40, -320},
        {0x44, 240},  {0x48, 265},  {0x4C, -18}, {0x50, -97}, {0x54, -175}, {0x58, -0.04f},
        {0x5C, 4},    {0x60, 200},  {0x64, 300}, {0x68, 2},   {0x6C, 300},  {0x70, 2},
        {0x74, 0},    {0x78, 15},   {0x7C, -80}, {0x80, -50}, {0x84, -150}, {0x88, -40},
        {0x8C, -100}, {0x90, -350}, {0x9C, 300}, {0xA0, 12},  {0xA4, 15},   {0xA8, 30}};
    for (auto entry : defaults)
        gabi::store<f32>(p + entry.offset, entry.value);
    return self;
}
VERIFY(0x02364E6C, hioConstructor);
void staticInit() {
    WWHD_FUNC(0x0236505C, void);
    gabi::store<u32>(0x1046A19C, 0);
    gabi::store<u32>(0x1046A1A4, 0);
    gabi::store<u32>(0x1046A198, 0);
    gabi::store<u32>(0x1046A1A0, 0);
    gabi::call(0x028F026C, 0x101CA4DC);
    gabi::store<f32>(0x1046A09C, -3.1415927410125732f);
    gabi::store<f32>(0x1046A0A0, 3.1415927410125732f);
    gabi::call(0x028ED6F8, 0x1046A0A4);
    gabi::call(0x028F026C, 0x101CA4E8);
    gabi::call(0x028EAB2C, 0x1046A0A5);
    gabi::call(0x028F026C, 0x101CA4F4);
    gabi::store<f32>(0x1046A2F4, 200.0f);
    gabi::store<f32>(0x1046A2E8, 0.0f);
    gabi::store<f32>(0x1046A2EC, -10.0f);
    gabi::store<f32>(0x1046A2F8, -340.0f);
    gabi::store<f32>(0x1046A2FC, 0.0f);
    gabi::store<f32>(0x1046A2F0, 0.0f);
    gabi::call(0x02364E6C, 0x1046A1FC);
}
VERIFY(0x0236505C, staticInit);
f32 searchPlayerDistance(Actor *actor) {
    WWHD_FUNC(0x023628CC, f32, actor);
    u32 play = gabi::ea(dComIfGp_get());
    f32 distance = gabi::call<f32>(0x025D6958, actor, gabi::load<u32>(play + 0x5B2C));
    play = gabi::ea(dComIfGp_get());
    u32 ship = gabi::load<u32>(play + 0x5B3C);
    if (ship) {
        f32 other = gabi::call<f32>(0x025D6958, actor, ship);
        if (other < distance)
            return other;
    }
    return distance;
}
VERIFY(0x023628CC, searchPlayerDistance);
Actor *actorConstructor(Actor *actor) {
    WWHD_FUNC(0x02361004, Actor *, actor);
    if (!actor) {
        actor = gabi::call<Actor *>(0x0273AD10, 0x16A0);
        if (!actor)
            return nullptr;
    }
    u32 p = gabi::ea(actor);
    gabi::call(0x025D4ED0, actor);
    gabi::store<u32>(p + 0xB4, 0x1002B354);
    gabi::store<u32>(p + 0x414, 0x100521A8);
    for (u32 off : {0x450u, 0x45Cu, 0x468u})
        if (!(p + off))
            gabi::call(0x0273AD10, 12);
    gabi::store<u32>(p + 0x478, 0x100521A8);
    for (u32 off : {0x4B4u, 0x4C0u, 0x4CCu})
        if (!(p + off))
            gabi::call(0x0273AD10, 12);
    gabi::store<u32>(p + 0x4DC, 0x100521E8);
    gabi::store<u32>(p + 0x4F8, 0x10052268);
    gabi::call(0x028EFFD0, p + 0x508, 3, 12, 0x02365150);
    gabi::call(0x025A9084, p + 0x548);
    gabi::call(0x025A5894, p + 0x59C, 0, 0);
    gabi::call(0x025A5894, p + 0x5CC, 0, 0);
    gabi::call(0x027F2BC0, p + 0x654, 0);
    gabi::store<u32>(p + 0x664, 0x1016E54C);
    gabi::call(0x027DA984, p + 0x668);
    gabi::store<u32>(p + 0x6DC, 0);
    gabi::store<u32>(p + 0x6D0, 0);
    gabi::store<u32>(p + 0x664, 0x1002B2FC);
    gabi::store<u32>(p + 0x6D8, 0);
    gabi::store<u32>(p + 0x69C, 0x1016D820);
    gabi::store<u32>(p + 0x6D4, 0);
    gabi::store<u32>(p + 0x6AC, 0);
    gabi::call(0x024F0474, p + 0x718);
    gabi::store<u32>(p + 0x728, 0x1002B324);
    gabi::store<u32>(p + 0x738, 0x1002B334);
    gabi::store<u32>(p + 0x72C, 0x1002B344);
    gabi::store<u8>(p + 0x730, 1);
    gabi::call(0x024EFE94, p + 0x8DC);
    gabi::call(0x025EB82C, p + 0x934);
    gabi::call(0x025E7C6C, p + 0x1440);
    gabi::call(0x025166F0, p + 0x14D8);
    gabi::call(0x0200BD2C, p + 0x1604);
    gabi::call(0x02515DA0, p + 0x1620);
    gabi::store<u32>(p + 0x1620, 0x1004AEC0);
    gabi::store<u32>(p + 0x161C, 0x1004AE88);
    gabi::store<f32>(p + 0x1684, 1.0f);
    return actor;
}
VERIFY(0x02361004, actorConstructor);
void nodeControl(Actor *actor, void *node, J3DModel *model) {
    WWHD_FUNC(0x0235FE84, void, actor, node, model);
    u32 joint = gabi::call<u32>(0x027F7878, node);
    u32 index = gabi::load<u16>(joint + 4);
    if (index >= 4)
        gabi::call(0x0273AA24, 0x1002B3A8, 0x14C, 0x1002B3BC);
    u32 block = gabi::load<u32>(gabi::ea(model) + 0x2C), array = gabi::load<u32>(block + 0x10);
    u32 matrix = 0x1048D0CC;
    gabi::store<u16>(block + 4, gabi::load<u16>(block + 4) | 0x10);
    gabi::call(0x028E90D4, array + index * 0x30, matrix);
    u32 rotation = gabi::ea(actor) + 0x1420 + index * 6;
    gabi::call(0x025F1B48, matrix, gabi::load<s16>(rotation), gabi::load<s16>(rotation + 2),
               gabi::load<s16>(rotation + 4));
    if (index == 1) {
        s32 product =
            (s32)((u32)(s32)(s16)actor->mCraneAmplitude * (u32)(gabi::load<s16>(0x1047BD52) + 5));
        u16 phase = actor->mCraneAnimPhase;
        s16 base = actor->mCraneAngle;
        f32 sine = gabi::load<f32>(0x104A44F8 + 8 * (phase >> 3));
        s16 angle = (s16)gabi::ftoi(gabi::fmadds((f32)product, sine, (f32)base));
        gabi::call(0x025F1BF4, matrix, angle);
    }
    gabi::call(0x028E90D4, matrix, 0x104B4868);
    block = gabi::load<u32>(gabi::ea(model) + 0x2C);
    array = gabi::load<u32>(block + 0x10);
    gabi::store<u16>(block + 4, gabi::load<u16>(block + 4) | 0x10);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = gabi::load<f32>(matrix + i * 4);
    for (u32 i = 0; i < 12; ++i)
        gabi::store<f32>(array + index * 0x30 + i * 4, values[i]);
}
VERIFY(0x0235FE84, nodeControl);
void Actor::createWave() {
    WWHD_FUNC(0x02361268, void, this);
    if (!gabi::load<u32>(0x1046A2A8)) {
        gabi::store<f32>(0x1046A2B4, 1.0f);
        gabi::store<u32>(0x1046A2A8, 1);
        gabi::store<f32>(0x1046A2B0, 0.5f);
        gabi::store<f32>(0x1046A2B8, -0.3f);
    }
    if (!gabi::load<u32>(0x1046A2AC)) {
        gabi::store<f32>(0x1046A2C0, 1.0f);
        gabi::store<u32>(0x1046A2AC, 1);
        gabi::store<f32>(0x1046A2BC, -0.5f);
        gabi::store<f32>(0x1046A2C4, -0.3f);
    }
    if (!mWaveLCallback.mpEmitter) {
        u32 play = gabi::ea(dComIfGp_get());
        gabi::call(0x025A847C, gabi::load<u32>(play + 0x5AB0), 0, 0x37, &mWavePos, &mWaveRot, 0,
                   255, &mWaveLCallback, -1, 0, 0, 0);
        u32 emitter = mWaveLCallback.mpEmitter;
        if (emitter)
            for (u32 i = 0; i < 12; i += 4)
                gabi::store<f32>(emitter + 0x28 + i, gabi::load<f32>(0x1046A2B0 + i));
    }
    if (!mWaveRCallback.mpEmitter) {
        u32 play = gabi::ea(dComIfGp_get());
        gabi::call(0x025A847C, gabi::load<u32>(play + 0x5AB0), 0, 0x37, &mWavePos, &mWaveRot, 0,
                   255, &mWaveRCallback, -1, 0, 0, 0);
        u32 emitter = mWaveRCallback.mpEmitter;
        if (emitter)
            for (u32 i = 0; i < 12; i += 4)
                gabi::store<f32>(emitter + 0x28 + i, gabi::load<f32>(0x1046A2BC + i));
    }
    if (!mSplashCallback.mpEmitter) {
        u32 play = gabi::ea(dComIfGp_get());
        gabi::call(0x025A847C, gabi::load<u32>(play + 0x5AB0), 0, 0x35, &mWavePos, &mWaveRot, 0,
                   255, &mSplashCallback, -1, 0, 0, 0);
    }
    if (!mTrackCallback.mpEmitter) {
        u32 play = gabi::ea(dComIfGp_get());
        gabi::call(0x025A847C, gabi::load<u32>(play + 0x5AB0), 5, 0x36, &mTrackPos, &shape_angle, 0,
                   0, &mTrackCallback, -1, 0, 0, 0);
        u32 emitter = mTrackCallback.mpEmitter;
        if (emitter)
            for (u32 off : {0x220u, 0x23Cu, 0x240u, 0x238u, 0x228u, 0x224u})
                gabi::store<f32>(emitter + off, 1.0f);
    }
}
VERIFY(0x02361268, &Actor::createWave);
s32 Actor::create() {
    WWHD_FUNC(0x02361C8C, s32, this);
    u32 status = gabi::load<u32>(gabi::ea(this) + 0x2E4);
    if (!(status & 8)) {
        if (gabi::ea(this)) {
            actorConstructor(this);
            status = gabi::load<u32>(gabi::ea(this) + 0x2E4);
        }
        gabi::store<u32>(gabi::ea(this) + 0x2E4, status | 8);
    }
    s32 phase = gabi::call<s32>(0x02520460, &mPhase, 0x1002B808);
    if (phase == 4) {
        getArg();
        u32 size = gabi::load<u32>(0x1002B850 + (u32)(s32)mType * 4);
        if (!gabi::call<s32>(0x025D63E8, this, 0x02361000, size))
            return 5;
        gabi::call(0x023614C8, this);
    }
    return phase;
}
VERIFY(0x02361C8C, &Actor::create);
void Actor::modePathMoveTerry() {
    WWHD_FUNC(0x023649B8, void, this);
    setCollision();
    if (checkTgHit())
        return;
    bool stop = false;
    f32 distance = searchPlayerDistance(this);
    if (distance < (f32)gabi::load<s16>(0x1046A292) && !gabi::call<s32>(0x0211D2F8, &mStopTimer)) {
        mStopTimer = gabi::ftoi(cM_rndF(120.0f) + 240.0f);
        f32 voice = cM_rndF(100.0f);
        s32 room = gabi::load<s8>(gabi::ea(this) + 0x326);
        u32 sound = voice < 50.0f ? 0x6A2A : 0x6A2B;
        s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call(0x025E1A40, sound, &mSePos, 0, reverb);
    }
    s32 type = mType;
    if (type == 1) {
        mVelocityFwdTarget = gabi::load<f32>(0x1046A29C);
        u32 play = gabi::ea(dComIfGp_get());
        u32 flag = gabi::load<u32>(play + 0x5CD8);
        s16 range = gabi::load<s16>(flag & 0x10000 ? 0x1046A290 : 0x1046A296);
        if (distance < (f32)range)
            stop = true;
    } else if (type == 3) {
        mVelocityFwdTarget = gabi::load<f32>(0x1046A2A0);
        if (distance < (f32)gabi::load<s16>(0x1046A292))
            mVelocityFwdTarget = gabi::load<f32>(0x1046A2A4);
        if (distance < (f32)gabi::load<s16>(0x1046A294))
            stop = true;
    }
    if (mbIsLinkRiding) {
        mTimer = 30;
        if (mPathId != 255)
            pathMove();
        return;
    }
    if (!gabi::call<s32>(0x0211D2F8, &mTimer) && stop) {
        modeProc(0, 11);
        return;
    }
    if (mPathId != 255)
        pathMove();
}
VERIFY(0x023649B8, &Actor::modePathMoveTerry);
void Actor::modeStopTerry() {
    WWHD_FUNC(0x02364C6C, void, this);
    setCollision();
    if (checkTgHit())
        return;
    f32 distance = searchPlayerDistance(this);
    s32 type = mType;
    bool close = true;
    if (type == 1) {
        u32 play = gabi::ea(dComIfGp_get());
        u32 flag = gabi::load<u32>(play + 0x5CD8);
        close = distance < (f32)gabi::load<s16>(flag & 0x10000 ? 0x1046A290 : 0x1046A296);
    } else if (type == 3)
        close = distance < (f32)gabi::load<s16>(0x1046A294);
    if (!close) {
        modeProc(0, 10);
        return;
    }
    if (mbIsLinkRiding) {
        if (!gabi::call<s32>(0x0211D2F8, &mTimer)) {
            modeProc(0, 10);
            return;
        }
    } else
        mTimer = 300;
    mVelocityFwdTarget = 0.0f;
    if (mPathId != 255)
        pathMove();
}
VERIFY(0x02364C6C, &Actor::modeStopTerry);
s32 Actor::createHeap() {
    WWHD_FUNC(0x02360C64, s32, this);
    auto getResource = [&](u32 archive, u32 index) {
        gabi::Local<be<u32>[2]> name;
        (*name.get())[0] = archive;
        (*name.get())[1] = 0x1002B2E4;
        return gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), name.get(), index);
    };
    auto getNamedResource = [&](u32 nameAddress) {
        gabi::Local<be<u32>[2]> archive, name;
        (*archive.get())[0] = 0x1002B808;
        (*archive.get())[1] = 0x1002B2E4;
        (*name.get())[0] = nameAddress;
        (*name.get())[1] = 0x1002B2E4;
        return gabi::call<u32>(0x02606900, gabi::load<u32>(0x101F4F28), archive.get(), name.get());
    };
    u32 data = getResource(0x1002B808, gabi::load<u32>(0x1002B5C4 + (u32)(s32)mType * 4));
    if (!data)
        gabi::call(0x0273AA24, 0x1002B5EC, 0x92D, 0x1002B600);
    mpModel = gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x11000022);
    if (!mpModel)
        return 0;
    if (mType == 4) {
        u32 animation = getResource(0x1002B808, 5);
        if (!animation)
            gabi::call(0x0273AA24, 0x1002B5EC, 0x937, 0x1002B614);
        if (!gabi::call<s32>(0x025E8508, &mBckAnm, data, animation, 1, 2, 0, -1, 0, 1.0f))
            return 0;
    }
    gabi::store<u32>(gabi::ea(mpModel.get()) + 0xB8, gabi::ea(this));
    if (mType == 4) {
        u16 index = 0;
        u32 tree = gabi::call<u32>(0x027F3F94, data);
        while (index < gabi::load<u16>(tree + 8)) {
            if (index >= 1 && index <= 3) {
                u32 size = gabi::load<u32>(data + 4), joint = gabi::load<u32>(data + 8);
                if (index < size)
                    joint += index * 0x1C;
                gabi::store<u32>(joint + 8, 0x02360038);
            }
            ++index;
            tree = gabi::call<u32>(0x027F3F94, data);
        }
    }
    gabi::call(0x02360420, this);
    u32 seaData = getNamedResource(gabi::load<u32>(0x101CA4B0 + (u32)(s32)mType * 4));
    mpSeaModel = gabi::call<J3DModel *>(0x025E38E0, seaData, 0, 0x11020203);
    if (!mpSeaModel)
        return 0;
    u32 seaAnimation = getNamedResource(gabi::load<u32>(0x101CA4C4 + (u32)(s32)mType * 4));
    if (!gabi::call<s32>(0x025E7CE0, &mSeaBtk, seaData, seaAnimation, 1, 2, 0, -1, 0, 0, 1.0f))
        return 0;
    mpBgW = gabi::call<u32>(0x024F23F4, 0);
    if (!mpBgW)
        return 0;
    u32 collision = getResource(0x1002B808, gabi::load<u32>(0x1002B5D8 + (u32)(s32)mType * 4));
    if (gabi::call<s32>(0x0200A030, (u32)mpBgW, collision, 1, &mMtx))
        return 0;
    if (mType == 4) {
        u32 ropeTexture = getResource(0x1002B5B4, 0x7E);
        if (!gabi::call<s32>(0x025EBA58, gabi::ea(this) + 0x934, 1, 200, ropeTexture, 0))
            return 0;
        data = getResource(0x1002B5BC, 0x2E);
        if (!data)
            gabi::call(0x0273AA24, 0x1002B5EC, 0x989, 0x1002B600);
        mpRopeEnd = gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x11000002);
        if (!mpRopeEnd)
            return 0;
    }
    return 1;
}
VERIFY(0x02360C64, &Actor::createHeap);
void Actor::createInit() {
    WWHD_FUNC(0x023614C8, void, this);
    mInitPos.copy(current.pos);
    f32 random = cM_rndF(120.0f);
    u32 type = mType;
    u32 position[3] = {gabi::load<u32>(gabi::ea(&current.pos)),
                       gabi::load<u32>(gabi::ea(&current.pos) + 4),
                       gabi::load<u32>(gabi::ea(&current.pos) + 8)};
    u8 path = mPathId;
    mStopTimer = gabi::ftoi(random + 240.0f);
    if (type <= 2)
        gabi::store<u32>(gabi::ea(this) + 0x2E0, gabi::load<u32>(gabi::ea(this) + 0x2E0) | 0x25);
    for (u32 i = 0; i < 3; ++i)
        gabi::store<u32>(gabi::ea(&mPathPosTarget) + i * 4, position[i]);
    if (path != 255 && (mType == 4 || mType == 1 || mType == 3)) {
        mpPath = gabi::call<u32>(0x025AAF88, path, gabi::load<s8>(gabi::ea(this) + 0x326));
        if (mType == 4)
            modeProc(0, 8);
    } else
        modeProc(0, 0);
    if (mType == 1 || mType == 3)
        modeProc(0, 11);
    if (mType == 4)
        for (u32 i = 0; i < 4; ++i)
            mSvId[i] =
                gabi::call<u32>(0x025D5AA4, 0x1002B71C + i * 4, gabi::load<u32>(gabi::ea(this) + 4),
                                -1, &current.pos, gabi::load<s8>(gabi::ea(this) + 0x1C9), 0, 0, 0);
    current.pos.y = gabi::call<f32>(0x025871F8, &current.pos, &mObjAcch);
    gabi::store<u8>((u32)mpBgW + 0xBB, gabi::load<u8>(gabi::ea(this) + 0x326));
    if (mType != 4) {
        gabi::call(0x02360420, this);
        gabi::call(0x024F43DC, (u32)mpBgW);
    }
    u32 play = gabi::ea(dComIfGp_get());
    gabi::call(0x024EEA6C, play + 0x12A0, (u32)mpBgW, this);
    if (mType == 1)
        gabi::store<u32>((u32)mpBgW + 0xB0, 0x02360414);
    u32 callback = mType == 1 ? 0x024EE708 : 0x024EE658;
    gabi::store<u32>((u32)mpBgW + 0xA8, callback);
    gabi::call(0x024EFF44, &mAcchCir, 30.0f, 30.0f);
    gabi::call(0x024F06B4, &mObjAcch, &current.pos, &old.pos, this, 1, &mAcchCir, &speed, 0, 0);
    gabi::store<u32>(gabi::ea(this) + 0x740, gabi::load<u32>(gabi::ea(this) + 0x740) | 0xC);
    s32 currentType = mType;
    if (currentType == 4 || currentType == 3 || currentType == 1) {
        createWave();
        if (mType == 3 || mType == 1)
            gabi::call(0x025564B4, &mPLight);
    }
    gravity = 0.0f;
    gabi::call(0x025D6870, this, 0);
    u32 model = gabi::ea(mpModel.get());
    gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
    f32 actorScale = scale.x;
    f32 low = -1000.0f * actorScale, high = 1000.0f * actorScale;
    gabi::call(0x025D674C, this, low, -50.0f * actorScale, low, high, high, high);
    cullSizeFar = 10.0f;
    if (mType == 0 || mType == 4) {
        if (!gabi::load<u32>(0x1046A2C8)) {
            for (u32 i = 0; i < 15; ++i)
                gabi::store<f32>(0x1046A1C0 + i * 4, i == 1    ? 700.0f
                                                     : i == 12 ? 100.0f
                                                     : i == 13 ? 530.0f
                                                               : 0.0f);
            gabi::store<u32>(0x1046A2C8, 1);
        }
        mFlagPcId = gabi::call<u32>(
            0x025D5834, 0xAE, gabi::load<u32>(0x1002B374 + (u32)(s32)mType * 4), &current.pos,
            gabi::load<s8>(gabi::ea(this) + 0x1C9), &current.angle, 0, -1, 0);
        currentType = mType;
        mFlagOffset.copy(*gabi::at<cXyz>(0x1046A1C0 + (u32)currentType * 12));
        mFlagScale = gabi::load<f32>(0x1002B388 + (u32)currentType * 4);
    }
    mWave.mAnimX = (s16)gabi::ftoi(cM_rndF(32768.0f));
    mWave.mAnimZ = (s16)gabi::ftoi(cM_rndF(32768.0f));
    if (mType == 4) {
        gabi::call(0x027F4D5C, mpModel.get());
        u32 positions = gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xAB8));
        mRopeCount = 15;
        u32 modelBlock = gabi::load<u32>(gabi::ea(mpModel.get()) + 0x2C),
            array = gabi::load<u32>(modelBlock + 0x10);
        gabi::store<u16>(modelBlock + 4, gabi::load<u16>(modelBlock + 4) | 0x10);
        u32 matrix = 0x1048D0CC;
        gabi::call(0x028E90D4, array + 0x30, matrix);
        gabi::call(0x025F24E0, gabi::load<f32>(0x1046A2F4), gabi::load<f32>(0x1046A2F8),
                   gabi::load<f32>(0x1046A2FC));
        gabi::store<f32>(positions + 0xA8, gabi::load<f32>(matrix + 0xC));
        gabi::store<f32>(positions + 0xAC, gabi::load<f32>(matrix + 0x1C));
        gabi::store<f32>(positions + 0xB0, gabi::load<f32>(matrix + 0x2C));
        gabi::Local<cXyz> step, tmp, difference;
        step->set(0.0f, -1.0f, 0.0f);
        gabi::call(0x028E8E64, step.get(), step.get(), 10.0f);
        s32 count = mRopeCount;
        if (count >= 2)
            for (s32 i = count - 2; i >= 0; --i) {
                gabi::call(0x0201AD78, positions + (u32)(i + 1) * 12, tmp.get(), step.get());
                gabi::at<cXyz>(positions + (u32)i * 12)->copy(*tmp.get());
                mRopeVelocity[i].copy(*gabi::at<cXyz>(0x101FFBA8));
            }
        positions = gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xAB8));
        gabi::call(0x0201ADE0, positions, difference.get(), positions + 12);
        u16 heading = shape_angle.y;
        f32 sine = gabi::load<f32>(0x104A44F8 + 8 * (heading >> 3)),
            cosine = gabi::load<f32>(0x104A44FC + 8 * (heading >> 3));
        f32 dx = difference->x, dz = difference->z, dy = difference->y;
        f32 x = gabi::fmsubs(cosine, dx, sine * dz), z = gabi::fmadds(sine, dx, cosine * dz);
        gabi::call(0x028E93CC, matrix, gabi::load<f32>(positions), gabi::load<f32>(positions + 4),
                   gabi::load<f32>(positions + 8));
        s16 pitch = gabi::call<s32>(0x020195B0, z, dy);
        s16 yaw = shape_angle.y;
        f32 horizontal = gabi::call<f32>(0x028F4384, gabi::fmadds(dy, dy, z * z));
        s16 roll = gabi::call<s32>(0x020195B0, -x, horizontal);
        gabi::call(0x025F1B48, matrix, pitch, yaw, roll);
        gabi::call(0x025F1BF4, matrix, -0x4000);
        mRopeEndPos.set(gabi::load<f32>(matrix + 0xC), gabi::load<f32>(matrix + 0x1C),
                        gabi::load<f32>(matrix + 0x2C));
        f32 values[12];
        for (u32 i = 0; i < 12; ++i)
            values[i] = gabi::load<f32>(matrix + i * 4);
        u32 endModel = gabi::ea(mpRopeEnd.get());
        for (u32 i = 0; i < 12; ++i)
            gabi::store<f32>(endModel + 0xC8 + i * 4, values[i]);
    }
    gabi::call(0x02515F14, &mStts, 255, 255, this);
    gabi::call(0x0251677C, &mSph, 0x1002B810);
    gabi::store<u32>(gabi::ea(this) + 0x151C, gabi::ea(&mStts));
}
VERIFY(0x023614C8, &Actor::createInit);
void initializeSeaControls(Actor *actor) {
    WWHD_FUNC(0x02363168, void, actor);
    auto initialize = [](u32 guard, u32 storage, u32 name) {
        if (gabi::load<u32>(guard))
            return false;
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(storage), gabi::at<u8>(name), 4);
        return true;
    };
    s32 type = actor->mType;
    if (type == 4) {
        if (gabi::load<u32>(0x101FDA44))
            goto common;
        initialize(0x101FDA44, 0x101FEBE8, 0x1002B2C8);
        type = actor->mType;
    }
    if (type == 1) {
        if (initialize(0x101FDAC0, 0x101FEBF8, 0x1002B2CC))
            initialize(0x101FDAC0, 0x101FEBF8, 0x1002B2CC);
        initialize(0x101FDA50, 0x101FEBF4, 0x1002B2D0);
        type = actor->mType;
    }
    if (type == 3) {
        initialize(0x101FDA50, 0x101FEBF4, 0x1002B2D0);
        initialize(0x101FDAC0, 0x101FEBF8, 0x1002B2CC);
        type = actor->mType;
    }
    if (type == 1 || type == 3) {
        if (initialize(0x101FDA50, 0x101FEBF4, 0x1002B2D0))
            type = actor->mType;
    }
    if (type != 4 && type != 3 && type != 1)
        return;
common:
    if (initialize(0x101FDA50, 0x101FEBF4, 0x1002B2D0))
        type = actor->mType;
    if (type == 3 || type == 1 || type == 4)
        initialize(0x101FDA50, 0x101FEBF4, 0x1002B2D0);
}
VERIFY(0x02363168, initializeSeaControls);
void Actor::setWave() {
    WWHD_FUNC(0x02362968, void, this);
    f32 splashTarget = gabi::load<f32>(0x1046A25C), waveSpeed = gabi::load<f32>(0x1046A264),
        waveMax = gabi::load<f32>(0x1046A274), trackSpeed = gabi::load<f32>(0x1046A268);
    if (!(speedF > 2.0f)) {
        waveSpeed = 0.0f;
        splashTarget = 0.0f;
        mTrackCallback.mStop = 1;
    } else
        createWave();
    mWavePos.y = gabi::call<f32>(0x025871F8, &mWavePos, &mObjAcch);
    mWaveRot.y = shape_angle.y;
    if (mTrackCallback.mpEmitter) {
        f32 trans = gabi::load<f32>(0x1046A254), scale = gabi::load<f32>(0x1046A258);
        u32 flags = gabi::load<u32>(gabi::ea(this) + 0x740);
        mTrackCallback.mSpeed = trackSpeed;
        mTrackCallback.mIndTransY = trans;
        mTrackCallback.mWaterY = mWavePos.y;
        mTrackCallback.mIndScaleY = scale;
        mTrackCallback.mWaterFlatY =
            flags & 0x800 ? gabi::load<f32>(gabi::ea(this) + 0x8D4) : -1000000000.0f;
        mTrackCallback.mLimitSpeed = 3.0f;
    }
    u32 emitter = mWaveLCallback.mpEmitter;
    if (emitter) {
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x254, waveSpeed > 0.0f ? flags & ~1u : flags | 1u);
    }
    emitter = mWaveRCallback.mpEmitter;
    if (emitter) {
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x254, waveSpeed > 0.0f ? flags & ~1u : flags | 1u);
    }
    mWaveRCallback.mSpeed = waveSpeed;
    mWaveLCallback.mSpeed = waveSpeed;
    mWaveLCallback.mMaxSpeed = waveMax;
    mWaveRCallback.mMaxSpeed = waveMax;
    mWaveRCallback.mPitch = gabi::load<f32>(0x1046A270) + 1.0f;
    mWaveLCallback.mPitch = 1.0f - gabi::load<f32>(0x1046A270);
    f32 z1 = gabi::load<f32>(0x1046A28C), z0 = gabi::load<f32>(0x1046A280),
        y1 = gabi::load<f32>(0x1046A288), y0 = gabi::load<f32>(0x1046A27C),
        x1 = gabi::load<f32>(0x1046A284), x0 = gabi::load<f32>(0x1046A278);
    mWaveRCallback.mAnchor1.y = y1;
    mWaveLCallback.mAnchor1.x = -x1;
    mWaveLCallback.mAnchor0.x = -x0;
    mWaveLCallback.mAnchor1.y = y1;
    mWaveRCallback.mAnchor1.x = x1;
    mWaveRCallback.mAnchor1.z = z1;
    mWaveLCallback.mAnchor0.y = y0;
    mWaveRCallback.mAnchor0.x = x0;
    mWaveRCallback.mAnchor0.z = z0;
    mWaveLCallback.mAnchor0.z = z0;
    mWaveLCallback.mAnchor1.z = z1;
    mWaveRCallback.mAnchor0.y = y0;
    mWaveRCallback.mMaxDisSpeed = gabi::load<f32>(0x1046A26C);
    mWaveLCallback.mMaxDisSpeed = gabi::load<f32>(0x1046A26C);
    gabi::call(0x0200ED84, &mSplashScaleTimer, splashTarget, 0.1f, 10.0f);
    emitter = mSplashCallback.mpEmitter;
    if (emitter) {
        f32 timer = mSplashScaleTimer;
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x254, timer > 0.1f ? flags & ~1u : flags | 1u);
    }
    mSplashCallback.mSpeed = mSplashScaleTimer;
    mSplashCallback.mMaxSpeed = gabi::load<f32>(0x1046A260);
}
VERIFY(0x02362968, &Actor::setWave);
void Actor::epProc() {
    WWHD_FUNC(0x02361E40, void, this);
    if (!gabi::call<s32>(0x02556D14)) {
        gabi::call(0x025A5AC8, &mFireCallback);
        mPLight.mPower = 0.0f;
        return;
    }
    u32 debug = 0x1047B608;
    if (!mFireCallback.mpEmitter) {
        if (!gabi::load<u32>(0x1046A2CC))
            gabi::store<u32>(0x1046A2CC, 1);
        f32 scale = gabi::load<f32>(debug + 0x6F0) + 0.5f;
        gabi::store<f32>(0x1046A1B8, scale);
        gabi::store<f32>(0x1046A1B4, scale);
        gabi::store<f32>(0x1046A1BC, scale);
        u32 play = gabi::ea(dComIfGp_get());
        gabi::call(0x025A847C, gabi::load<u32>(play + 0x5AB0), 0, 0x1EA, &mFirePos, 0, 0x1046A1B4,
                   255, &mFireCallback, -1, 0, 0, 0);
        if (!mFireCallback.mpEmitter)
            return;
    }
    gabi::Local<cXyz> pos;
    pos->set(mFirePos.x, mFirePos.y + 20.0f, mFirePos.z);
    u32 play = gabi::ea(dComIfGp_get());
    gabi::call(0x025A8D40, gabi::load<u32>(play + 0x5AB0), 0x4004, pos.get(), 255, 0x101D5E98,
               0x101D5E98, 0);
    if (!gabi::call<s32>(0x0211D2F8, &mEpTimer0)) {
        f32 value = cM_rndF(gabi::load<f32>(debug + 0x10) + 5.0f);
        mEpTimer0 = (s16)gabi::ftoi(value + gabi::load<f32>(debug + 0x14));
        value = cM_rndF(gabi::load<f32>(debug + 0x20) + 4.0f);
        mGlowRadiusTarget = (value + 8.0f) + gabi::load<f32>(debug + 0x24);
    }
    if (!gabi::call<s32>(0x0211D2F8, &mEpTimer1)) {
        mEpTimer1 = (s16)gabi::ftoi(cM_rndF(6.0f) + 3.0f);
        mLightPowerTarget = cM_rndF(0.075f) + 0.75f;
    }
    gabi::call(0x0200ED84, &mGlowRadius, (f32)mGlowRadiusTarget, 1.0f,
               gabi::load<f32>(debug + 0x18) + 1.0f);
    gabi::call(0x0200ED84, &mLightPower, (f32)mLightPowerTarget, 0.4f, 0.04f);
    mPLight.mPower = 150.0f * (f32)mLightPower;
}
VERIFY(0x02361E40, &Actor::epProc);
s32 Actor::execute() {
    WWHD_FUNC(0x02362C54, s32, this);
    u32 debug = 0x1047B608;
    mWaveAnimPhase = (s16)((s32)mWaveAnimPhase + gabi::load<s16>(debug + 0x508) + 0x200);
    f32 target = gabi::load<f32>(debug + 0x49C) + 5.0f;
    if (mCurMode == 10 && speedF > 1.0f)
        target = gabi::load<f32>(debug + 0x49C) + 20.0f;
    gabi::call(0x0200ED84, &mHDWaterOffset, target, 0.1f, 1.0f);
    f32 offset = gabi::load<f32>(0x104A44F8 + 8 * ((u16)mWaveAnimPhase >> 3)) * (f32)mHDWaterOffset;
    if (mBombSmokeCallback.mpEmitter) {
        s16 yaw = (s16)((s32)(s16)shape_angle.y + (s16)mBombSmokeAngle);
        mBombSmokeRot.z = shape_angle.z;
        mBombSmokeRot.x = shape_angle.x;
        mBombSmokeRot.y = yaw;
        u32 emitter = mBombSmokeCallback.mpEmitter;
        for (u32 off : {0x220u, 0x224u, 0x228u, 0x238u, 0x23Cu, 0x240u})
            gabi::store<f32>(emitter + off, 3.0f);
        mBombSmokePos.copy(current.pos);
        mBombSmokePos.y = gabi::load<f32>(0x1046A224);
        if (!gabi::call<s32>(0x0211D2F8, &mBombSmokeTimer))
            gabi::call(0x025A5AC8, &mBombSmokeCallback);
    }
    if (mStopRequest && mCurMode != 9) {
        mPreviousMode = mCurMode;
        modeProc(0, 9);
    }
    if (mType == 3 || mType == 1)
        epProc();
    if (mType == 4) {
        gabi::call(0x023620F0, this);
        if (mType == 4) {
            mBckAnm.mFrameCtrl.mFrame = (f32)(s32)mHandleFrame;
            s16 index = gabi::load<s16>(debug + 0x740);
            if (index) {
                gabi::store<s16>(gabi::ea(this) + 0x1420 + (s32)index * 6,
                                 gabi::load<s16>(debug + 0x742));
                index = gabi::load<s16>(debug + 0x740);
                gabi::store<s16>(gabi::ea(this) + 0x1422 + (s32)index * 6,
                                 gabi::load<s16>(debug + 0x744));
                index = gabi::load<s16>(debug + 0x740);
                gabi::store<s16>(gabi::ea(this) + 0x1424 + (s32)index * 6,
                                 gabi::load<s16>(debug + 0x746));
            }
        }
    }
    modeProc(1, 13);
    current.pos.y = gabi::call<f32>(0x025871F8, &current.pos, &mObjAcch) + offset;
    gabi::call(0x02360420, this);
    gabi::call(0x027F4D5C, mpModel.get());
    s32 type = mType;
    if (type == 4 || type == 3 || type == 1) {
        u32 model = gabi::ea(mpModel.get());
        f32 s = scale.x;
        s32 culled = gabi::call<s32>(0x025D6B70, model ? model + 0xC8 : 0, -1000.0f * s, -50.0f * s,
                                     -1000.0f * s, 1000.0f * s, 1000.0f * s, 1000.0f * s);
        bool remove = !(speedF > 2.0f) || culled;
        if (!remove)
            remove = searchPlayerDistance(this) > 18000.0f;
        if (remove) {
            gabi::call(0x025A92C0, &mWaveRCallback);
            gabi::call(0x025A92C0, &mWaveLCallback);
            gabi::call(0x025A99B8, &mSplashCallback);
            mTrackCallback.mStop = 1;
        } else
            setWave();
    }
    gabi::call(0x024F43DC, (u32)mpBgW);
    if (mType == 0 || mType == 4) {
        gabi::Local<be<u32>> id;
        *id.get() = mFlagPcId;
        u32 flag = 0;
        if ((u32)mFlagPcId != 0xFFFFFFFF)
            flag = gabi::call<u32>(0x025D5218, 0x025E1234, id.get());
        if (flag) {
            gabi::store<u32>(flag + 0x1E34, gabi::ea(&mMtx));
            f32 x = gabi::load<f32>(0x1046A208), y = gabi::load<f32>(0x1046A20C),
                z = gabi::load<f32>(0x1046A210);
            if (x != 0.0f || y != 0.0f || z != 0.0f)
                mFlagOffset.set(x, y, z);
            f32 size = gabi::load<f32>(0x1046A214);
            if (size != 0.0f)
                mFlagScale = size;
            else
                size = mFlagScale;
            gabi::store<u32>(flag + 0x1E38, gabi::ea(&mFlagOffset));
            gabi::store<f32>(flag + 0x1DFC, size);
        }
    }
    type = mType;
    mbIsLinkRiding = 0;
    if (type == 1 || type == 3) {
        gabi::Local<cXyz> delta;
        gabi::call(0x0201ADE0, &current.pos, delta.get(), &old.pos);
        f32 speed = gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, delta.get())) / 30.0f;
        if (!(speed > 0.0f))
            speed = 0.0f;
        else if (!(speed < 1.0f))
            speed = 1.0f;
        s8 volume = (s8)gabi::ftoi(speed * 100.0f);
        s32 room = gabi::load<s8>(gabi::ea(this) + 0x326),
            reverb = gabi::call<s32>(0x02520540, room);
        gabi::call(0x025E1A40, 0x303D, &eyePos, volume, reverb);
    }
    return 1;
}
VERIFY(0x02362C54, &Actor::execute);
void Actor::setRopePos() {
    WWHD_FUNC(0x023620F0, void, this);
    if (!gabi::load<u32>(0x1046A2D0)) {
        gabi::store<f32>(0x1046A1A8, 0.6f);
        gabi::store<f32>(0x1046A1B0, 0.6f);
        gabi::store<u32>(0x1046A2D0, 1);
        gabi::store<f32>(0x1046A1AC, 0.6f);
    }
    s32 count = mRopeCount;
    u32 base = gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xAB8));
    s32 endIndex = count ? count - 1 : 0;
    u32 end = base + (u32)endIndex * 12;
    gabi::Local<cXyz> saved, direction, normalized, tmp, delta, scaled, divided, updated,
        velocityDelta;
    saved->copy(*gabi::at<cXyz>(end));
    u32 matrix = 0x1048D0CC;
    auto anchorMatrix = [&]() {
        u32 block = gabi::load<u32>(gabi::ea(mpModel.get()) + 0x2C),
            array = gabi::load<u32>(block + 0x10);
        gabi::store<u16>(block + 4, gabi::load<u16>(block + 4) | 0x10);
        gabi::call(0x028E90D4, array + 0x30, matrix);
        gabi::call(0x025F24E0, gabi::load<f32>(0x1046A2F4), gabi::load<f32>(0x1046A2F8),
                   gabi::load<f32>(0x1046A2FC));
    };
    anchorMatrix();
    gabi::at<cXyz>(end)->set(gabi::load<f32>(matrix + 0xC), gabi::load<f32>(matrix + 0x1C),
                             gabi::load<f32>(matrix + 0x2C));
    if (mCurMode == 1) {
        u32 endModel = gabi::ea(mpRopeEnd.get());
        u32 modelMatrix = endModel ? endModel + 0xC8 : 0;
        direction->set(gabi::load<f32>(modelMatrix + 0xC) - gabi::load<f32>(end),
                       gabi::load<f32>(modelMatrix + 0x1C) - gabi::load<f32>(end + 4),
                       gabi::load<f32>(modelMatrix + 0x2C) - gabi::load<f32>(end + 8));
        gabi::call(0x0201B3C0, direction.get(), normalized.get());
        gabi::call(0x028E8E64, direction.get(), direction.get(), 10.0f);
        count = mRopeCount;
        for (s32 i = count - 2; i >= 0; --i) {
            gabi::call(0x0201AD78, base + (u32)(i + 1) * 12, tmp.get(), direction.get());
            gabi::at<cXyz>(base + (u32)i * 12)->copy(*tmp.get());
            mRopeVelocity[i].copy(*gabi::at<cXyz>(0x101FFBA8));
        }
    } else {
        count = mRopeCount;
        for (s32 i = count - 2; i >= 0; --i) {
            u32 point = base + (u32)i * 12;
            auto velocity = &mRopeVelocity[i];
            saved->copy(*gabi::at<cXyz>(point));
            f32 water = gabi::call<f32>(0x025871F8, point, &mObjAcch);
            f32 drag = gabi::load<f32>(point + 4) < water ? 0.6f : 0.9f;
            gabi::call(0x028E8E64, velocity, velocity, drag);
            velocity->y = (f32)velocity->y - 3.0f;
            gabi::call(0x028E8D88, point, velocity, point);
            gabi::call(0x0201ADE0, point, delta.get(), point + 12);
            direction->copy(*delta.get());
            f32 length = gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, direction.get()));
            if (length < 0.01f)
                gabi::call(0x0201AD78, point + 12, updated.get(), 0x1046A2E8);
            else {
                gabi::call(0x0201AE48, direction.get(), scaled.get(), 10.0f);
                gabi::call(0x0201AEAC, scaled.get(), divided.get(), length);
                gabi::call(0x0201AD78, point + 12, updated.get(), divided.get());
            }
            gabi::at<cXyz>(point)->copy(*updated.get());
            gabi::call(0x0201ADE0, point, velocityDelta.get(), saved.get());
            gabi::call(0x0201AE48, velocityDelta.get(), delta.get(), 0.05f);
            gabi::call(0x028E8D88, velocity, delta.get(), velocity);
        }
    }
    count = mRopeCount;
    base = gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xAB8));
    f32 x, y, z, dx, dy, dz;
    if (count > 1) {
        gabi::call(0x0201ADE0, base, delta.get(), base + 12);
        x = gabi::load<f32>(base);
        dx = delta->x;
        y = gabi::load<f32>(base + 4);
        z = gabi::load<f32>(base + 8);
        dy = delta->y;
        dz = delta->z;
    } else {
        anchorMatrix();
        x = gabi::load<f32>(base);
        y = gabi::load<f32>(base + 4);
        dx = x - gabi::load<f32>(matrix + 0xC);
        dy = y - gabi::load<f32>(matrix + 0x1C);
        z = gabi::load<f32>(base + 8);
        dz = z - gabi::load<f32>(matrix + 0x2C);
    }
    u16 heading = shape_angle.y;
    f32 sine = gabi::load<f32>(0x104A44F8 + 8 * (heading >> 3)),
        cosine = gabi::load<f32>(0x104A44FC + 8 * (heading >> 3));
    f32 rx = gabi::fmsubs(cosine, dx, sine * dz), rz = gabi::fmadds(sine, dx, cosine * dz);
    gabi::call(0x028E93CC, matrix, x, y, z);
    s16 pitch = gabi::call<s32>(0x020195B0, rz, dy);
    s16 yaw = shape_angle.y;
    f32 horizontal = gabi::call<f32>(0x028F4384, gabi::fmadds(dy, dy, rz * rz));
    s16 roll = gabi::call<s32>(0x020195B0, -rx, horizontal);
    gabi::call(0x025F1B48, matrix, pitch, yaw, roll);
    gabi::call(0x025F1BF4, matrix, -0x4000);
    mRopeEndPos.set(gabi::load<f32>(matrix + 0xC), gabi::load<f32>(matrix + 0x1C),
                    gabi::load<f32>(matrix + 0x2C));
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = gabi::load<f32>(matrix + i * 4);
    u32 model = gabi::ea(mpRopeEnd.get());
    for (u32 i = 0; i < 12; ++i)
        gabi::store<f32>(model + 0xC8 + i * 4, values[i]);
    f32 firstY = gabi::load<f32>(base + 4), water = gabi::call<f32>(0x025871F8, base, &mObjAcch);
    gabi::store<f32>(base + 4, firstY);
    if (water > firstY) {
        count = mRopeCount;
        u32 crossing = base;
        for (s32 i = 0; i < count; ++i) {
            f32 pointY = i ? gabi::load<f32>(base + (u32)i * 12 + 4) : firstY;
            if (!(pointY > water))
                crossing = base + (u32)i * 12;
        }
        f32 pointX = gabi::load<f32>(crossing);
        mRipplePos.y = water;
        mRipplePos.x = pointX;
        mRipplePos.z = gabi::load<f32>(crossing + 8);
        if (!mRippleCallback.mpEmitter) {
            u32 play = gabi::ea(dComIfGp_get());
            gabi::call(0x025A847C, gabi::load<u32>(play + 0x5AB0), 5, 0x33, &mRipplePos, 0,
                       0x1046A1A8, 255, &mRippleCallback, -1, 0, 0, 0);
            if (mRippleCallback.mpEmitter) {
                u8 timer = mRippleTimer;
                mRippleCallback.mRate = 0.0f;
                if (!timer) {
                    gabi::call(0x025DAE64, &mRipplePos, 0, 1.0f, 0.7f);
                    mRippleTimer = 19;
                    return;
                }
            }
        }
    } else
        gabi::call(0x025A9270, &mRippleCallback);
    if (mRippleTimer)
        mRippleTimer = (u8)((u8)mRippleTimer - 1);
}
VERIFY(0x023620F0, &Actor::setRopePos);
void Actor::setMtx() {
    WWHD_FUNC(0x02360420, void, this);
    u32 debug = 0x1047B608, matrix = 0x1048D0CC;
    gabi::call(0x025872F4, &current.pos, &mWave, 0.0f);
    mLinkRockPhase = (s16)((s32)mLinkRockPhase + 1);
    s16 step = (s16)(gabi::load<s16>(debug + 0x748) + 5);
    gabi::call(0x0200F428, &mLinkRockAmplitude, 0, 10, step);
    s32 amplitude = mLinkRockAmplitude;
    if (amplitude <= gabi::load<s16>(debug + 0x748) + 5) {
        amplitude = 0;
        mLinkRockAmplitude = 0;
    }
    s32 product = (s32)((u32)amplitude * (u32)(gabi::load<s16>(debug + 0x74A) + 3));
    u16 wavePhase = mWaveAnimPhase;
    f32 waveSine = gabi::load<f32>(0x104A44F8 + 8 * (wavePhase >> 3));
    s16 rockPhase = mLinkRockPhase;
    s16 wavePitch = (s16)gabi::ftoi((f32)(gabi::load<s16>(debug + 0x50A) + 200) * waveSine);
    s16 waveRoll = (s16)gabi::ftoi((f32)(gabi::load<s16>(debug + 0x50C) + 60) * waveSine);
    s16 rockPitch = (s16)gabi::ftoi(
        (f32)product * gabi::load<f32>(0x104A44F8 + 8 * ((u16)((s32)rockPhase * 0x900) >> 3)));
    s16 rockRoll = (s16)gabi::ftoi(
        (f32)product * gabi::load<f32>(0x104A44F8 + 8 * ((u16)((s32)rockPhase * 0x700) >> 3)));
    if (!gabi::load<u8>(0x1046A203)) {
        shape_angle.x = (s16)((s32)mWave.mRotX + rockPitch + wavePitch);
        shape_angle.z = (s16)((s32)mWave.mRotZ + rockRoll + waveRoll);
        if (mbIsLinkRiding) {
            u32 play = gabi::ea(dComIfGp_get()), player = gabi::load<u32>(play + 0x5B34);
            gabi::Local<cXyz> delta, local;
            delta->set((f32)current.pos.x - gabi::load<f32>(player + 0x314),
                       (f32)current.pos.y - gabi::load<f32>(player + 0x318),
                       (f32)current.pos.z - gabi::load<f32>(player + 0x31C));
            gabi::call(0x025F1884, gabi::load<u32>(0x1018C7B0), (s16) - (s16)shape_angle.y);
            gabi::call(0x0200FCD8, delta.get(), local.get());
            s16 pitch = (s16)gabi::ftoi(((f32)local->z - (-200.0f)) *
                                        (gabi::load<f32>(debug + 0xA54) - 1.0f));
            s16 roll = (s16)gabi::ftoi((f32)local->x * (gabi::load<f32>(debug + 0xA58) + 2.0f));
            if (gabi::load<u32>(player + 0x3B8) & 0x02000000) {
                pitch = (s16)((s32)pitch * 2);
                roll = (s16)((s32)roll * 2);
                if (!mHDMotionMode) {
                    mHDMotionMode = 1;
                    mLinkRockAmplitude = 150;
                }
            } else
                mHDMotionMode = 0;
            s32 p = pitch, r = roll;
            if (p > 0x400)
                p = 0x400;
            else if (p < -0x400)
                p = -0x400;
            if (r > 0x480)
                r = 0x480;
            else if (r < -0x200)
                r = -0x200;
            gabi::call(0x0200F428, &mHDPitch, p, 4, 0x100);
            gabi::call(0x0200F428, &mHDRoll, r, 4, 0x100);
        } else {
            mHDMotionMode = 0;
            gabi::call(0x0200F428, &mHDPitch, 0, 0x20, 0x80);
            gabi::call(0x0200F428, &mHDRoll, 0, 0x20, 0x80);
        }
        s16 pitch = mHDPitch, x = shape_angle.x, z = shape_angle.z, roll = mHDRoll;
        shape_angle.x = (s16)((s32)x + pitch);
        shape_angle.z = (s16)((s32)z + roll);
    }
    u32 model = gabi::ea(mpModel.get());
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    gabi::store<f32>(model + 0xBC, sx);
    gabi::store<f32>(model + 0xC0, sy);
    gabi::store<f32>(model + 0xC4, sz);
    auto transform = [&]() {
        gabi::call(0x028E93CC, matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
        gabi::call(0x025F19F8, matrix, (s16)shape_angle.x, 0, (s16)shape_angle.z);
        gabi::call(0x025F1C28, matrix, (s16)shape_angle.y);
    };
    transform();
    if (mType == 4)
        gabi::call(0x025F24E0, 0.0f, 30.0f, -260.0f);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = gabi::load<f32>(matrix + i * 4);
    model = gabi::ea(mpModel.get());
    for (u32 i = 0; i < 12; ++i)
        gabi::store<f32>(model + 0xC8 + i * 4, values[i]);
    if (mType == 4) {
        transform();
        gabi::call(0x025F1C28, matrix, 0x4000);
        for (u32 i = 0; i < 4; ++i) {
            gabi::Local<cXyz> offset;
            offset->set(gabi::load<f32>(0x1046A244 + i * 4), 0.0f, 0.0f);
            gabi::Local<be<u32>> actor;
            if (gabi::call<s32>(0x025D54C4, (u32)mSvId[i], actor.get()))
                gabi::call(0x028E8F64, matrix, offset.get(), (u32)*actor.get() + 0x314);
        }
        gabi::Local<cXyz> offset;
        offset->set(gabi::load<f32>(0x1046A23C), 0.0f, 0.0f);
        gabi::call(0x028E8F64, matrix, offset.get(), &mWavePos);
        offset->x = gabi::load<f32>(0x1046A240);
        gabi::call(0x028E8F64, matrix, offset.get(), &mTrackPos);
    }
    gabi::call(0x028E90D4, matrix, &mMtx);
    s32 type = mType;
    if (type == 3 || type == 1) {
        mLightRotY = (s16)((s32)mLightRotY + 0xD0);
        mLightRotX = (s16)((s32)mLightRotX + 0x100);
        gabi::Local<cXyz> lightOffset;
        lightOffset->set(gabi::load<f32>(debug + 0x6D0), gabi::load<f32>(debug + 0x6D4) + 340.0f,
                         gabi::load<f32>(debug + 0x6D8) + 200.0f);
        model = gabi::ea(mpModel.get());
        gabi::call(0x028E90D4, model ? model + 0xC8 : 0, matrix);
        gabi::call(0x028E8F64, matrix, lightOffset.get(), &mFirePos);
        gabi::call(0x025F24E0, (f32)lightOffset->x, (f32)lightOffset->y, (f32)lightOffset->z);
        gabi::call(0x025F1C28, matrix, (s16)mLightRotY);
        gabi::call(0x025F1BF4, matrix, (s16)mLightRotX);
        f32 power = mLightPower;
        gabi::call(0x025F2518, power, power, power);
        gabi::call(0x028E90D4, matrix, &mLightMtx);
        mPLight.mPos.set(gabi::load<f32>(matrix + 0xC), gabi::load<f32>(matrix + 0x1C),
                         gabi::load<f32>(matrix + 0x2C));
        mPLight.mColorR = 600;
        mPLight.mColorG = 400;
        mPLight.mColorB = 120;
        mPLight.mFluctuation = 250.0f;
        type = mType;
        if (type == 1 || type == 3) {
            gabi::Local<cXyz> soundOffset;
            soundOffset->set(gabi::load<f32>(debug + 0x6D0) - 105.0f,
                             gabi::load<f32>(debug + 0x6D4) + 380.0f,
                             gabi::load<f32>(debug + 0x6D8) + 40.0f);
            model = gabi::ea(mpModel.get());
            gabi::call(0x028E90D4, model ? model + 0xC8 : 0, matrix);
            gabi::call(0x028E8F64, matrix, soundOffset.get(), &mSePos);
            type = mType;
        }
    }
    if (type == 4 || type == 3 || type == 1) {
        transform();
        gabi::Local<cXyz> offset;
        offset->set(0.0f, gabi::load<f32>(0x1046A228), 0.0f);
        gabi::call(0x028E8F64, matrix, offset.get(), gabi::ea(this) + 0x390);
        offset->y = gabi::load<f32>(0x1046A22C);
        gabi::call(0x028E8F64, matrix, offset.get(), &eyePos);
        if (mType != 4) {
            f32 waveHeight = gabi::load<f32>(debug + 0x49C) + 5.0f;
            if (mCurMode == 10)
                waveHeight = gabi::load<f32>(debug + 0x49C) + 20.0f;
            offset->y = gabi::load<f32>(0x1046A234);
            offset->z = gabi::load<f32>(0x1046A230) + waveHeight;
            gabi::call(0x028E8F64, matrix, offset.get(), &mWavePos);
            offset->y = 0.0f;
            offset->z = gabi::load<f32>(0x1046A238);
            gabi::call(0x028E8F64, matrix, offset.get(), &mTrackPos);
        }
    }
}
VERIFY(0x02360420, &Actor::setMtx);
void Actor::drawSea() {
    WWHD_FUNC(0x02363374, void, this);
    gabi::call(0x025E742C, &mSeaBtk);
    auto initializeQuad = [](u32 guard, u32 storage, f32 left, f32 right, f32 front, f32 back,
                             bool guardFirst) {
        if (gabi::load<u32>(guard))
            return;
        if (guardFirst)
            gabi::store<u32>(guard, 1);
        const f32 values[] = {left, 0, front, right, 0, front, right, 0, back, left, 0, back};
        for (u32 i = 0; i < 12; ++i)
            gabi::store<f32>(storage + i * 4, values[i]);
        if (!guardFirst)
            gabi::store<u32>(guard, 1);
    };
    initializeQuad(0x1046A2D4, 0x1046A0A8, -600, 600, 600, -600, false);
    initializeQuad(0x1046A2D8, 0x1046A0D8, -350, 350, 650, -400, true);
    initializeQuad(0x1046A2DC, 0x1046A108, -550, 550, 750, -750, true);
    initializeQuad(0x1046A2E0, 0x1046A138, -140, 140, 300, -400, true);
    u32 matrix = 0x1048D0CC;
    gabi::call(0x028E93CC, matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    gabi::call(0x025F1C28, matrix, (s16)shape_angle.y);
    u32 control = gabi::load<u32>(0x101CA49C + (u32)(s32)mType * 4);
    gabi::Local<cXyz[4]> points;
    for (u32 i = 0; i < 4; ++i) {
        cXyz *point = &(*points.get())[i];
        gabi::call(0x028E8F64, matrix, control + i * 12, point);
        point->y = gabi::call<f32>(0x025871F8, point, &mObjAcch);
    }
    gabi::Local<cXyz> normal, a, b, cross;
    normal->set(gabi::load<f32>(0x101FFBA8), gabi::load<f32>(0x101FFBAC),
                gabi::load<f32>(0x101FFBB0));
    for (u32 i = 0; i < 4; i += 2) {
        gabi::call(0x0201ADE0, &(*points.get())[i + 1], a.get(), &(*points.get())[i]);
        gabi::call(0x0201ADE0, &(*points.get())[(i + 2) % 4], b.get(), &(*points.get())[i + 1]);
        gabi::call(0x0201B080, a.get(), cross.get(), b.get());
        if (!gabi::call<s32>(0x0201B47C, cross.get()))
            cross->copy(*gabi::at<cXyz>(0x101FFBC0));
        gabi::call(0x028E8D88, normal.get(), cross.get(), normal.get());
        if (!gabi::call<s32>(0x0201B47C, normal.get()))
            normal->copy(*gabi::at<cXyz>(0x101FFBC0));
    }
    f32 plane = -3.4028234663852886e38f;
    for (u32 i = 0; i < 4; ++i) {
        f32 dot = gabi::call<f32>(0x028E8F44, normal.get(), &(*points.get())[i]);
        if (dot > plane)
            plane = dot;
    }
    if (!gabi::load<u32>(0x1046A2E4)) {
        gabi::store<f32>(0x1046A168, 0.0f);
        gabi::store<f32>(0x1046A16C, 0.0f);
        gabi::store<u32>(0x1046A2E4, 1);
        const f32 crossPoints[] = {100, 0, 0, -100, -100, 0, 0, 100, 0, 0};
        for (u32 i = 0; i < 10; ++i)
            gabi::store<f32>(0x1046A170 + i * 4, crossPoints[i]);
    }
    gabi::Local<cXyz[4]> projected;
    for (u32 i = 0; i < 4; ++i) {
        cXyz *point = &(*projected.get())[i];
        gabi::call(0x028E8F64, matrix, 0x1046A168 + i * 12, point);
        f32 horizontal =
            gabi::fmadds((f32)point->x, (f32)normal->x, (f32)point->z * (f32)normal->z);
        point->y = (plane - horizontal) / (f32)normal->y;
    }
    gabi::Local<cXyz> xDifference, zDifference, horizontalX, horizontalZ;
    gabi::call(0x0201ADE0, &(*projected.get())[1], xDifference.get(), &(*projected.get())[0]);
    gabi::call(0x0201ADE0, &(*projected.get())[3], zDifference.get(), &(*projected.get())[2]);
    horizontalX->set(xDifference->x, 0.0f, xDifference->z);
    f32 length = gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, horizontalX.get()));
    s32 pitch = gabi::call<s32>(0x020195B0, (f32)xDifference->y, length);
    horizontalZ->set(zDifference->x, 0.0f, zDifference->z);
    length = gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, horizontalZ.get()));
    s32 roll = gabi::call<s32>(0x020195B0, (f32)zDifference->y, length);
    f32 horizontal =
            gabi::fmadds((f32)current.pos.x, (f32)normal->x, (f32)current.pos.z * (f32)normal->z),
        height = (plane - horizontal) / (f32)normal->y;
    f32 water = gabi::call<f32>(0x025871F8, &current.pos, &mObjAcch);
    f32 y = gabi::fmadds(water - height, height < water ? 1.0f : 0.0f, height);
    gabi::call(0x028E93CC, matrix, (f32)current.pos.x, y, (f32)current.pos.z);
    gabi::call(0x025F1B48, matrix, pitch, (s16)shape_angle.y, roll);
    gabi::call(0x025F24E0, 0.0f, 3.0f, 0.0f);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = gabi::load<f32>(matrix + i * 4);
    u32 model = gabi::ea(mpSeaModel.get());
    for (u32 i = 0; i < 12; ++i)
        gabi::store<f32>(model + 0xC8 + i * 4, values[i]);
    u32 data = gabi::load<u32>(gabi::ea(mpSeaModel.get()) + 0xAC),
        material = gabi::load<u32>(data + 0x10);
    gabi::Local<u8[4]> seaColor, foamColor;
    gabi::call(0x025602F0, seaColor.get(), foamColor.get());
    gabi::Local<be<s16>[4]> color;
    u32 colorEA = gabi::ea(seaColor.get());
    (*color.get())[0] = gabi::load<u8>(colorEA);
    (*color.get())[1] = gabi::load<u8>(colorEA + 1);
    (*color.get())[2] = gabi::load<u8>(colorEA + 2);
    (*color.get())[3] = 255;
    u32 light = gabi::call<u32>(0x02555D0C);
    f32 strength = gabi::load<f32>(light + 0x10B8);
    u32 tev = gabi::load<u32>(material + 0x18), vt = gabi::load<u32>(tev + 4);
    gabi::call(gabi::load<u32>(vt + 0x24), tev, 1, color.get());
    gabi::Local<be<f32>[4]> rgba;
    for (u32 i = 0; i < 4; ++i)
        (*rgba.get())[i] = (f32)(s16)(*color.get())[i] / 255.0f;
    gabi::Local<be<f32>[4]> bright;
    gabi::call(0x0274D458, bright.get(), rgba.get(), strength);
    gabi::store<u32>(material + 0xA0, gabi::load<u32>(material + 0xA0) | 0x20);
    u32 output = gabi::call<u32>(0x027F9F0C, material + 0xA0, 5);
    f32 alpha = (f32)(s16)(*color.get())[3] / 255.0f;
    f32 r = (*bright.get())[0], g = (*bright.get())[1], bValue = (*bright.get())[2];
    gabi::store<f32>(output + 4, g);
    gabi::store<f32>(output + 8, bValue);
    gabi::store<f32>(output, r);
    gabi::store<f32>(output + 12, alpha);
    gabi::call(0x025E7FC4, &mSeaBtk, data, (f32)mSeaBtk.mFrameCtrl.mFrame);
    gabi::call(0x025E2DE0, mpSeaModel.get(), 0);
}
VERIFY(0x02363374, &Actor::drawSea);
} // namespace ikada
