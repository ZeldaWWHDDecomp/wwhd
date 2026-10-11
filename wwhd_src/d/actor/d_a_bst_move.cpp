// Gohdan movement and HD-inlined action dispatch; verified against the original.
#include "d/actor/d_a_bst_move.h"

#include <cmath>
using namespace bst_move_names;

/* inlined into daBst_move */
static void bstFlyAnimator(bst_class* i_this, be<u16>* table, bool brk, s32 loop, f32 one,
                           u32 resourceName = BST_FLY_ARCHIVE) {
    u8 part = (u8)i_this->mBstPartType;
    u32 resources = (u32)bstMoveResourceTable();
    u32 morph = (u32)i_this->m02B8.v, model = gabi::ea((J3DModel*)bstMoveMorf(morph)->mpModel);
    u16 id = (u16)table[part];
    SafeString name;
    name.mStringTop = resourceName;
    name.__vtbl = BST_ARCHIVE_VTABLE;
    void* resource = BstMove_getResource(bstMoveOpaque(resources), &name, id);
    u32 data = gabi::ea((J3DModelData*)bstMoveModel(model)->mModelData),
        controller = (u32)*bstMoveAnimatorField(i_this, brk);
    (brk ? BstMove_initBrk : BstMove_initBtk)(bstMoveOpaque(controller), bstMoveOpaque(data),
                                              resource, 1, loop, 0, -1, 1, one, 0);
}
/* inlined into daBst_move */
static void bstDispatchSound(bst_class* i_this, u32 sound) {
    if (gabi::ea(&i_this->eyePos)) {
        s8 room = (s8)i_this->current.roomNo, reverb = BstMove_reverb(room);
        BstMove_startSound(sound, &i_this->eyePos, 0, reverb);
    }
}
/* inlined into daBst_move */
static void bstFixedAnimator(bst_class* i_this, u32 id, bool brk, f32 one) {
    u32 morph = (u32)i_this->m02B8.v, resources = (u32)bstMoveResourceTable(),
        model = gabi::ea((J3DModel*)bstMoveMorf(morph)->mpModel);
    SafeString name;
    name.mStringTop = BST_HEAD_ARCHIVE;
    name.__vtbl = BST_ARCHIVE_VTABLE;
    void* resource = BstMove_getResource(bstMoveOpaque(resources), &name, id);
    u32 controller = (u32)*bstMoveAnimatorField(i_this, brk),
        data = gabi::ea((J3DModelData*)bstMoveModel(model)->mModelData);
    (brk ? BstMove_initBrk : BstMove_initBtk)(bstMoveOpaque(controller), bstMoveOpaque(data),
                                              resource, 1, 0, 0, -1, 1, one, 0);
}
/* inlined into daBst_move */
static u32 bstHeldBomb(bst_class* i_this) {
    BstMoveID id;
    id = (u32)i_this->mBombId;
    if ((u32)id == 0xFFFFFFFF)
        return 0;
    return BstMove_searchActor(bstMoveSearchCallback(), &id);
}
/* inlined into daBst_move */
static void bstRetireEmitter(gptr<JPABaseEmitter>* slot) {
    BstEmitter* emitter = bstEmitter(*slot);
    if (emitter) {
        emitter->becomeInvalidEmitter();
        *slot = nullptr;
    }
}
/* inlined into daBst_move */
static void bstDownSound(bst_class* i_this, u32 sound) {
    if (gabi::ea(&i_this->current.pos)) {
        s8 room = (s8)i_this->current.roomNo;
        s8 reverb = BstMove_reverb(room);
        BstMove_startSound(sound, &i_this->current.pos, 0, reverb);
    }
}
/* inlined into daBst_move */
static void bstDownApproach(bst_class* i_this, u32 axis, f32 half) {
    f32 velocity = (f32)*bstMoveAxis(&i_this->speed, axis),
        target = (f32)*bstMoveAxis(&i_this->mTargetPos, axis);
    BstMove_approach(bstMoveAxis(&i_this->current.pos, axis), target, half, std::fabs(velocity));
}
/* inlined into daBst_move */
static f32 bstDownDistance(bst_class* i_this) {
    cXyz delta;
    cXyz copy;
    // GHS keeps the hidden-result temporary before the word copy.
    BstMove_vectorSubtractResult(&i_this->mTargetPos, &delta, &i_this->current.pos);
    copy.copy(delta);
    f32 square = BstMove_magnitudeSquared(&copy);
    return BstMove_sqrt(square);
}
/* inlined into daBst_move */
static void stay(bst_class* actor, f32 zero, f32 one, f32 ten) {
    bst_class* i_this = actor;
    s16 state = (s16)i_this->mDamage;
    i_this->attention_info.flags = 0;
    i_this->mState = 10;
    if (state == 0 || state == 2) {
        u8 part = (u8)i_this->mBstPartType;
        f32 two = (f32)bstMoveConstants().mTwo;
        u16 animation = (u16)(state == 0 ? bstMove_sleep_anm() : bstMove_start_anm())[part];
        BstMove_anmInit(actor, animation, two, state == 0 ? 2 : 0, one, -1);
        state = (s16)i_this->mDamage;
        i_this->mDamage = (s16)(state + 1);
        return;
    }
    if (state != 4 && state != 5)
        return;
    if (state == 4) {
        cXyz local;
        cXyz transformed;
        cXyz sum;
        local.x = zero;
        local.y = zero;
        u8 part = (u8)i_this->mBstPartType;
        if (part) {
            u16 animation = (u16)bstMove_start2_anm()[part];
            BstMove_anmInit(actor, animation, ten, 0, one, -1);
            local.z = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[11],
                                      (f32)bstMoveConstants().mFourHundred);
        } else
            local.z = (f32)bstMoveConstants().mTenThousand;
        part = (u8)i_this->mBstPartType;
        u16 effect = (u16)bstMove_start_smoke()[part];
        s8 room = (s8)i_this->current.roomNo;
        void* play = BstMove_play();
        u32 control =
            gabi::ea((dPa_control_c*)reinterpret_cast<dComIfG_inf_c*>(play)->play.mParticle);
        BstMove_setParticle(bstMoveOpaque(control), 2, effect, &i_this->current.pos,
                            &i_this->current.angle, nullptr, 185, &i_this->mPa_smokeEcallBack, room,
                            0, nullptr, nullptr);
        play = BstMove_play();
        s16 strength = (s16)bstMoveTuning().mRegS[2];
        cXyz impulse;
        impulse.z = zero;
        impulse.y = one;
        impulse.x = zero;
        BstMove_startShock(&reinterpret_cast<dComIfG_inf_c*>(play)->play.mVibration, strength + 3,
                           -33, &impulse);
        f32 shake =
            gabi::fadds_ppc((f32)bstMoveTuning().mRegF[10], (f32)bstMoveConstants().mFifteen);
        u32 head = (u32)bstMoveGlobals().boss;
        bstMoveActor(head)->m2ECC = shake;
        state = (s16)i_this->mDamage;
        i_this->mDamage = (s16)(state + 1);
        u32 matrix = (u32)bstMoveCalcMatrix();
        s16 yaw = (s16)i_this->current.angle.y;
        BstMove_rotateY(bstMoveOpaque(matrix), yaw);
        BstMove_transformVector(&local, &transformed);
        BstMove_vectorAddResult(&i_this->current.pos, &sum, &transformed);
        actor->mTargetPos.copy(sum);
        i_this->m10FC[3] = 15;
    }
    if ((u8)i_this->mBstPartType != 0 && (s16)i_this->m10FC[3] != 0) {
        for (u32 i = 0; i < 15; ++i) {
            u32 morph = (u32)i_this->m02B8.v,
                model = gabi::ea((J3DModel*)bstMoveMorf(morph)->mpModel),
                block = gabi::ea((J3DMtxBuffer_hd*)bstMoveModel(model)->mpMtxBuffer);
            u16 flags = (u16)bstMoveMatrixBuffer(block)->mFlags;
            u32 matrices = (u32)bstMoveMatrixBuffer(block)->mpAnmMtx;
            bstMoveMatrixBuffer(block)->mFlags = flags | 0x10;
            u32 matrix = (u32)bstMoveCalcMatrix();
            BstMove_matrixCopy(&bstMoveMatrix(matrices)[i + 2], bstMoveOpaque(matrix));
            cXyz origin;
            cXyz position;
            origin.x = zero;
            origin.y = zero;
            origin.z = zero;
            BstMove_transformVector(&origin, &position);
            void* play = BstMove_play();
            u32 control =
                gabi::ea((dPa_control_c*)reinterpret_cast<dComIfG_inf_c*>(play)->play.mParticle);
            BstMove_setSimpleParticle(bstMoveOpaque(control), 0x81D5, &position, 255,
                                      bstMoveParticleColor(), bstMoveParticleColor(), 0);
        }
    }
    f32 target = (f32)i_this->mTargetPos.x, ratio = (f32)bstMoveConstants().mPositionChaseRatio;
    BstMove_approach(&i_this->current.pos.x, target, ratio, ten);
    ratio = (f32)bstMoveConstants().mPositionChaseRatio;
    target = (f32)i_this->mTargetPos.z;
    BstMove_approach(&i_this->current.pos.z, target, ratio, ten);
}

/* inlined into daBst_move */
static void fly(bst_class* actor, f32 zero, f32 one, f32 half, f32 ten) {
    bst_class* i_this = actor;
    void* play = BstMove_play();
    u8 part = (u8)i_this->mBstPartType;
    u32 player =
        gabi::ea((fopAc_ac_c*)reinterpret_cast<dComIfG_inf_c*>(play)->play.mPlayerInfo[0].mpPlayer);
    s16 timer = (s16)i_this->mState;
    if (part && (s16)i_this->m10D8 == 6)
        *reinterpret_cast<be<u8>*>(&i_this->health) = 4;
    i_this->m10D8 = 1;
    if (timer == 0)
        BstMove_setHandCo(actor, 1);
    u32 head = (u32)bstMoveGlobals().boss;
    bool face = (s8)bstMoveActor(head)->m2E9A != 0;
    if (!face && ((s16)i_this->mUpdateLastFacingDirIfMultipleOf32 & 31) == 0)
        face = BstMove_random(one) < half;
    if (face)
        i_this->mLastCheckedPlayerFacingDir = (s16)bstMoveActor(player)->shape_angle.y;
    s16 state = (s16)i_this->mDamage;
    if (state == 0) {
        part = (u8)i_this->mBstPartType;
        u16 animation = (u16)bstMove_fly_anm()[part];
        BstMove_anmInit(actor, animation, ten, 2, one, -1);
        bstFlyAnimator(i_this, bstMove_damage_brk(), true, 0, one);
        s16 frame = (s16)bstMoveTuning().mRegS[8];
        u32 controller = (u32)i_this->mpTevRegAnimator.v;
        bstMoveFrame(controller)->mFrame = (f32)((s32)frame + 29);
        state = (s16)i_this->mDamage;
        i_this->m02D4 = 1;
        i_this->m02C4 = 1;
        i_this->mDamage = (s16)(state + 1);
    } else {
        if (state == 1) {
            bstFlyAnimator(i_this, bstMove_fly_brk(), true, 2, one);
            bstFlyAnimator(i_this, bstMove_fly_btk(), false, 2, one);
            state = (s16)i_this->mDamage;
            i_this->m10EC.y = zero;
            i_this->mDamage = (s16)(state + 1);
            state = 2;
        }
        if (state == 2) {
            s16 yaw = (s16)i_this->mLastCheckedPlayerFacingDir;
            u32 matrix = (u32)bstMoveCalcMatrix();
            BstMove_rotateY(bstMoveOpaque(matrix), yaw);
            f32 height = (f32)bstMoveTuning().mRegF[3],
                eightHundred = (f32)bstMoveConstants().mEightHundred,
                twoHundred = (f32)bstMoveConstants().mTwoHundred;
            f32 depth = (f32)bstMoveTuning().mRegF[4];
            cXyz local;
            cXyz transformed;
            cXyz sum;
            local.y = gabi::fadds_ppc(height, twoHundred);
            part = (u8)i_this->mBstPartType;
            if (!part) {
                local.x = zero;
                local.z = gabi::fadds_ppc(depth, (f32)bstMoveConstants().mThousand);
                head = (u32)bstMoveGlobals().boss;
                if ((s8)bstMoveActor(head)->m2E7C == 11)
                    local.y = gabi::fadds_ppc(height, (f32)bstMoveConstants().mThreeFifty);
            } else {
                local.z = gabi::fadds_ppc(depth, eightHundred);
                f32 side = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[2],
                                           (f32)bstMoveConstants().mEightHundred);
                if (part == 1)
                    local.x = -side;
                else if (part == 2)
                    local.x = side;
            }
            BstMove_transformVector(&local, &transformed);
            BstMove_vectorAddResult(&bstMoveActor(player)->current.pos, &sum, &transformed);
            f32 targetX = sum.x;
            part = (u8)i_this->mBstPartType;
            i_this->mTargetPos.x = targetX;
            f32 targetY = sum.y, targetZ = sum.z;
            i_this->mTargetPos.y = targetY;
            i_this->mTargetPos.z = targetZ;
            if (!part) {
                local.z = targetZ;
                local.y = zero;
                local.x = targetX;
                f32 square = BstMove_magnitudeSquared(&local), radius = BstMove_sqrt(square);
                f32 limit = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[9],
                                            (f32)bstMoveConstants().mArenaRadius);
                if (radius > limit) {
                    f32 ratio = limit / radius;
                    targetX = (f32)i_this->mTargetPos.x;
                    targetZ = (f32)i_this->mTargetPos.z;
                    i_this->mTargetPos.x = gabi::fmuls_ppc(targetX, ratio);
                    i_this->mTargetPos.z = gabi::fmuls_ppc(targetZ, ratio);
                }
            }
            BstMove_vectorSubtractResult(&i_this->mTargetPos, &sum, &i_this->current.pos);
            local.copy(sum);
            f32 square = BstMove_magnitudeSquared(&local), distance = BstMove_sqrt(square);
            f32 boundary = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[12],
                                           (f32)bstMoveConstants().mThreeHundred);
            if (distance > boundary) {
                part = (u8)i_this->mBstPartType;
                bool headFlight = false;
                if (!part) {
                    head = (u32)bstMoveGlobals().boss;
                    headFlight = (s8)bstMoveActor(head)->m2E7C == 11;
                }
                f32 speed = headFlight ? ten : (f32)bstMoveConstants().mThirty;
                f32 acceleration =
                    gabi::fadds_ppc((f32)bstMoveTuning().mRegF[13], (f32)bstMoveConstants().mTwo);
                BstMove_approach(&i_this->speedF, speed, one, acceleration);
            } else {
                f32 decay = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[14], one);
                BstMove_approachZero(&i_this->speedF, one, decay);
            }
            f32 turn = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[5],
                                       (f32)bstMoveConstants().mFifteenHundred);
            i_this->m10EC.x = turn;
            play = BstMove_play();
            u32 currentPlayer = gabi::ea(
                (fopAc_ac_c*)reinterpret_cast<dComIfG_inf_c*>(play)->play.mPlayerInfo[0].mpPlayer);
            yaw = BstMove_targetAngleY(actor, bstMoveOpaque(currentPlayer));
            BstMove_approachAngle(&i_this->shape_angle.y, yaw, 10, 0x400);
            part = (u8)i_this->mBstPartType;
            bool headFlight = false;
            if (!part) {
                head = (u32)bstMoveGlobals().boss;
                headFlight = (s8)bstMoveActor(head)->m2E7C == 11;
            }
            BstMove_approachAngle(&i_this->shape_angle.x, headFlight ? 0 : 3000, 10, 0x100);
            BstMove_approachAngle(&i_this->shape_angle.z, 0, 8, 0x200);
            f32 shake =
                gabi::fadds_ppc((f32)bstMoveTuning().mRegF[11], (f32)bstMoveConstants().mFifty);
            f32 acceleration = (f32)bstMoveConstants().mTwo;
            BstMove_approach(&i_this->m10EC.z, shake, one, acceleration);
        }
    }
    BstMove_moveFly(actor, 0);
    if ((u8)i_this->mBstPartType == 0 && (s16)i_this->mBombArrowDropTimer == 0) {
        u32 save = (u32)bstMoveSaveTable();
        if ((u8)bstMoveSave(save)->mArrowNum == 0 || (u8)bstMoveSave(save)->mBombNum == 0) {
            i_this->mBombArrowDropTimer = 600;
            i_this->mActionType = ACTION_HANA_DEMO_e;
            i_this->mDamage = 0;
        }
    }
}

/* inlined into daBst_move */
static void down_attack(bst_class* actor, f32 zero, f32 one, f32 half, f32 ten) {
    bst_class* i_this = actor;
    u32 play = gabi::ea(BstMove_play());
    u32 player = gabi::ea((fopAc_ac_c*)bstMoveInfo(play)->play.mPlayerInfo[0].mpPlayer);
    bool normalEnd = false, abort = false;
    BstMove_approachAngle(&i_this->shape_angle.x, 0, 10, 0x200);
    s16 state = (s16)i_this->mDamage;
    if (state == 0) {
        i_this->m10EC.y = zero;
        i_this->mDamage = (s16)(state + 1);
        f32 x = (f32)bstMoveActor(player)->current.pos.x;
        i_this->mTargetPos.x = x;
        f32 y = (f32)bstMoveActor(player)->current.pos.y;
        i_this->mTargetPos.y = y;
        f32 z = (f32)bstMoveActor(player)->current.pos.z,
            threeHundred = (f32)bstMoveConstants().mThreeHundred;
        i_this->mTargetPos.z = z;
        i_this->m10FC[0] = 100;
        f32 height = (f32)bstMoveTuning().mRegF[7], targetY = (f32)i_this->mTargetPos.y;
        i_this->mTargetPos.y = gabi::fadds_ppc(targetY, gabi::fadds_ppc(height, threeHundred));
        state = 1;
    }
    if (state == 1) {
        f32 step = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[13], one);
        BstMove_approach(&i_this->speedF, (f32)bstMoveConstants().mForty, one, step);
        f32 turn =
            gabi::fadds_ppc((f32)bstMoveTuning().mRegF[5], (f32)bstMoveConstants().mTwoThousand);
        i_this->m10EC.x = turn;
        f32 distance = bstDownDistance(i_this),
            boundary = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[12],
                                       (f32)bstMoveConstants().mTwoHundred);
        if (distance < boundary || (s16)i_this->m10FC[0] == 0)
            i_this->mDamage = (s16)((s16)i_this->mDamage + 1);
        BstMove_approachAngle(&i_this->shape_angle.y, (s16)i_this->current.angle.y, 4, 0x400);
        BstMove_moveFly(actor, 0);
    } else if (state == 2) {
        bstDownApproach(i_this, 0, half);
        bstDownApproach(i_this, 4, half);
        bstDownApproach(i_this, 8, half);
        BstMove_approachAngle(&i_this->shape_angle.y, (s16)i_this->current.angle.y, 4, 0x4000);
        f32 distance = bstDownDistance(i_this),
            boundary =
                gabi::fadds_ppc((f32)bstMoveTuning().mRegF[13], (f32)bstMoveConstants().mFifty);
        if (distance < boundary) {
            u8 part = (u8)i_this->mBstPartType;
            u16 animation = (u16)bstMove_down_anm()[part];
            BstMove_anmInit(actor, animation, ten, 2, one, -1);
            i_this->mDamage = (s16)((s16)i_this->mDamage + 1);
            i_this->speed.y = zero;
            i_this->m10FC[0] = 30;
            s16 debug = (s16)bstMoveTuning().mRegS[0], yaw = (s16)i_this->shape_angle.y;
            part = (u8)i_this->mBstPartType;
            i_this->m2E6E = (s16)(yaw + (debug == 1 ? 0x4000 : -0x4000));
            i_this->mFacingDir = part == 2 ? 0x4000 : -0x4000;
            bstDownSound(i_this, 0x58D8);
        }
    } else if (state == 3) {
        BstMove_approach(&i_this->m1114, (f32)bstMoveConstants().mMinusTwoHundred, one,
                         (f32)bstMoveConstants().mTwentyFive);
        bstDownApproach(i_this, 0, half);
        bstDownApproach(i_this, 8, half);
        BstMove_approachAngle(&i_this->shape_angle.z, (s16)i_this->mFacingDir, 4, 0x1000);
        BstMove_approachAngle(&i_this->shape_angle.y, (s16)i_this->m2E6E, 4, 0x1000);
        f32 y = (f32)i_this->current.pos.y, velocity = (f32)i_this->speed.y;
        s16 timer = (s16)i_this->m10FC[0];
        i_this->current.pos.y = gabi::fadds_ppc(y, velocity);
        if (timer) {
            f32 acceleration = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[14], half);
            velocity = (f32)i_this->speed.y;
            u32 flags = (u32)i_this->mAcch.m_flags;
            i_this->speed.y = gabi::fadds_ppc(velocity, acceleration);
            abort = (flags & 0x10) != 0;
        } else {
            BstMove_setHandAt(actor, 1);
            f32 acceleration =
                gabi::fadds_ppc((f32)bstMoveTuning().mRegF[3], (f32)bstMoveConstants().mSixty);
            velocity = (f32)i_this->speed.y;
            i_this->speed.y = gabi::fsubs_ppc(velocity, acceleration);
            f32 floor =
                gabi::fadds_ppc((f32)bstMoveTuning().mRegF[4], (f32)bstMoveConstants().mOneFifty);
            y = (f32)i_this->current.pos.y;
            if (!(y > floor)) {
                i_this->current.pos.y = floor;
                i_this->speed.y = zero;
                play = gabi::ea(BstMove_play());
                s16 strength = (s16)bstMoveTuning().mRegS[2];
                cXyz impulse;
                impulse.x = zero;
                impulse.y = one;
                impulse.z = zero;
                BstMove_startShock(&bstMoveInfo(play)->play.mVibration, strength + 5, -33,
                                   &impulse);
                i_this->m112A = (s16)((s16)bstMoveTuning().mRegS[7] + 10);
                bstDownSound(i_this, 0x6992);
                i_this->mDamage = (s16)((s16)i_this->mDamage + 1);
                i_this->m10FC[0] = 30;
                cXyz position;
                position.x = (f32)i_this->current.pos.x;
                position.y = (f32)i_this->current.pos.y;
                position.z = (f32)i_this->current.pos.z;
                position.y = one;
                play = gabi::ea(BstMove_play());
                u32 control = gabi::ea((dPa_control_c*)bstMoveInfo(play)->play.mParticle);
                BstMove_setParticle(bstMoveOpaque(control), 4, 0xC1D6, &position, nullptr, nullptr,
                                    255, nullptr, -1, 0, nullptr, nullptr);
                s8 room = (s8)i_this->current.roomNo;
                play = gabi::ea(BstMove_play());
                control = gabi::ea((dPa_control_c*)bstMoveInfo(play)->play.mParticle);
                BstMove_setParticle(bstMoveOpaque(control), 2, 0xA1D8, &position, nullptr, nullptr,
                                    185, &i_this->mPa_smokeEcallBack, room, 0, nullptr, nullptr);
            }
        }
    } else if (state == 4) {
        BstMove_approach(&i_this->m1114, (f32)bstMoveConstants().mMinusTwoHundred, one,
                         (f32)bstMoveConstants().mTwentyFive);
        if ((s16)i_this->m10FC[0] == 0) {
            normalEnd = true;
            abort = true;
        }
    } else
        BstMove_moveFly(actor, 0);
    if (!normalEnd)
        abort = abort || ((u32)i_this->mAcch.m_flags & 0x10) != 0;
    if (abort) {
        i_this->mActionType = ACTION_FLY_e;
        i_this->mDamage = 0;
        if (i_this)
            bstDownSound(i_this, 0x58D9);
        if (!normalEnd) {
            u8 part = (u8)i_this->mBstPartType;
            u32 index = 2u - part;
            if (index < 2) {
                be<u32>* otherHand = &bstMoveGlobals().hand[index];
                u32 other = *otherHand;
                if ((s16)bstMoveActor(other)->mActionType == ACTION_DOWN_ATTACK_e) {
                    bstMoveActor(other)->mActionType = ACTION_FLY_e;
                    other = *otherHand;
                    bstMoveActor(other)->mDamage = 0;
                    other = *otherHand;
                    bstMoveActor(other)->speedF = zero;
                }
            }
        }
    }
}

/* inlined into daBst_move */
static void sleep(bst_class* actor, f32 zero, f32 one, f32 ten) {
    bst_class* i_this = actor;
    i_this->m10D8 = 6;
    BstMove_setHandCo(actor, 1);
    i_this->attention_info.flags = 0;
    f32 fifty = (f32)bstMoveConstants().mFifty;
    i_this->mState = 10;
    f32 shake = (f32)bstMoveTuning().mRegF[11], two = (f32)bstMoveConstants().mTwo;
    BstMove_approach(&i_this->m10EC.z, gabi::fadds_ppc(shake, fifty), one, two);
    s16 state = (s16)i_this->mDamage;
    if (state == 0) {
        f32 twenty = (f32)bstMoveConstants().mTwenty;
        u8 part = (u8)i_this->mBstPartType;
        u16 animation = (u16)bstMove_sleep_start_anm()[part];
        BstMove_anmInit(actor, animation, twenty, 2, one, -1);
        bstFlyAnimator(i_this, bstMove_sleep_brk(), true, 0, one, BST_SLEEP_ARCHIVE);
        bstFlyAnimator(i_this, bstMove_sleep_btk(), false, 0, one, BST_SLEEP_ARCHIVE);
        state = (s16)i_this->mDamage;
        i_this->mDamage = (s16)(state + 1);
        bstDispatchSound(i_this, 0x58DC);
        state = 1;
    }
    if (state == 1) {
        if ((s16)i_this->m10FC[0] == 0) {
            f32 x = BstMove_randomSymmetric((f32)bstMoveConstants().mFifteenHundred);
            i_this->mTargetPos.x = x;
            f32 y = BstMove_random((f32)bstMoveConstants().mTwoHundred);
            f32 sixHundred = (f32)bstMoveConstants().mSixHundred;
            u8 part = (u8)i_this->mBstPartType;
            y = gabi::fadds_ppc(y, sixHundred);
            i_this->mTargetPos.y = y;
            if (part == 1)
                i_this->mTargetPos.y = gabi::fadds_ppc(y, (f32)bstMoveConstants().mThreeFifty);
            f32 z = BstMove_randomSymmetric((f32)bstMoveConstants().mFifteenHundred);
            i_this->mTargetPos.z = z;
            f32 duration = BstMove_random((f32)bstMoveConstants().mThirty);
            i_this->m10FC[0] = (s16)gabi::ftoi(duration);
        }
        f32 turn =
            gabi::fadds_ppc((f32)bstMoveTuning().mRegF[5], (f32)bstMoveConstants().mFiveHundred);
        i_this->m10EC.x = turn;
        f32 speed = (f32)bstMoveTuning().mRegF[6];
        s16 duration = (s16)i_this->m10FC[1];
        i_this->speedF = gabi::fadds_ppc(speed, ten);
        if (duration == 0) {
            i_this->mActionType = ACTION_FLY_e;
            i_this->mDamage = 0;
            bstDispatchSound(i_this, 0x58D5);
        }
    }
    BstMove_moveFly(actor, 0);
}

/* inlined into daBst_move */
static void damage(bst_class* actor, f32 zero, f32 one) {
    bst_class* i_this = actor;
    s16 state = (s16)i_this->mDamage;
    i_this->mState = 10;
    if (state == 0) {
        u8 part = (u8)i_this->mBstPartType;
        if (part) {
            u16 animation = (u16)bstMove_damage_anm()[part];
            BstMove_anmInit(actor, animation, one, 0, one, -1);
        }
        bstFlyAnimator(i_this, bstMove_damage_brk(), true, 0, one, BST_DAMAGE_ARCHIVE);
        bstFlyAnimator(i_this, bstMove_damage_btk(), false, 0, one, BST_DAMAGE_ARCHIVE);
        state = (s16)i_this->mDamage;
        i_this->mDamage = (s16)(state + 1);
        i_this->m10FC[0] = 30;
    } else if (state == 1 && (s16)i_this->m10FC[0] == 0) {
        u8 part = (u8)i_this->mBstPartType;
        if (part && (s8)i_this->health <= 0) {
            part = (u8)i_this->mBstPartType;
            i_this->mActionType = ACTION_SLEEP_e;
            i_this->m10FC[1] = 300;
            u32 hand = (u32)bstMoveGlobals().hand[part == 1 ? 0 : 1];
            if ((s16)bstMoveActor(hand)->mActionType != ACTION_SLEEP_e)
                BstMove_bgmStart(part == 1 ? 0x80000120 : 0x80000121);
        } else {
            i_this->speedF = zero;
            i_this->mActionType = ACTION_FLY_e;
        }
        i_this->mDamage = 0;
    }
}
/* inlined into daBst_move */
static void head_damage(bst_class* actor, f32 zero, f32 one) {
    bst_class* i_this = actor;
    i_this->mState = 10;
    i_this->m2E74[0] = 10;
    i_this->m2E74[1] = 10;
    BstMove_approachAngle(&i_this->shape_angle.x, 0, 8, 0x200);
    BstMove_approachAngle(&i_this->shape_angle.z, 0, 8, 0x200);
    BstMove_approachZero(&i_this->m10EC.z, one, (f32)bstMoveConstants().mFive);
    s16 state = (s16)i_this->mDamage;
    s16 entryState = state;
    bool processBomb = state == 4;
    if (state == 0) {
        i_this->speedF = zero;
        i_this->speed.y = zero;
        i_this->mDamage = (s16)(state + 1);
        i_this->m10FC[0] = 40;
        bstFixedAnimator(i_this, dRes_INDEX_BST_BRK_BST_END_e, true, one);
        bstFixedAnimator(i_this, dRes_INDEX_BST_BTK_BST_END_e, false, one);
        bstDispatchSound(i_this, 0x58DD);
        BstMove_bgmStart(0x80000123);
        state = 1;
    }
    if (state == 1) {
        cXyz horizontal;
        horizontal.x = (f32)i_this->current.pos.x;
        horizontal.y = zero;
        horizontal.z = (f32)i_this->current.pos.z;
        f32 square = BstMove_magnitudeSquared(&horizontal), radius = BstMove_sqrt(square);
        f32 limit =
            gabi::fadds_ppc((f32)bstMoveTuning().mRegF[9], (f32)bstMoveConstants().mArenaRadius);
        if (radius > limit) {
            f32 ratio = limit / radius, step = (f32)bstMoveConstants().mThirty,
                target = gabi::fmuls_ppc((f32)horizontal.x, ratio);
            BstMove_approach(&i_this->current.pos.x, target, (f32)bstMoveConstants().mBoundaryRatio,
                             step);
            target = gabi::fmuls_ppc((f32)horizontal.z, ratio);
            BstMove_approach(&i_this->current.pos.z, target, (f32)bstMoveConstants().mBoundaryRatio,
                             (f32)bstMoveConstants().mThirty);
        }
        if ((s16)i_this->m10FC[0] == 0) {
            f32 y = (f32)i_this->current.pos.y, speed = (f32)i_this->speed.y;
            y = gabi::fadds_ppc(y, speed);
            f32 five = (f32)bstMoveConstants().mFive;
            i_this->current.pos.y = y;
            f32 acceleration = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[13], five);
            speed = gabi::fsubs_ppc(speed, acceleration);
            i_this->speed.y = speed;
            f32 floor = (f32)bstMoveTuning().mRegF[14];
            if (!(y > floor)) {
                i_this->current.pos.y = floor;
                void* play = BstMove_play();
                s32 strength = (s16)bstMoveTuning().mRegS[2] + 7;
                cXyz impulse;
                impulse.z = zero;
                impulse.y = one;
                impulse.x = zero;
                BstMove_startShock(&reinterpret_cast<dComIfG_inf_c*>(play)->play.mVibration,
                                   strength, -33, &impulse);
                s8 room = (s8)i_this->current.roomNo;
                play = BstMove_play();
                u32 control = gabi::ea(
                    (dPa_control_c*)reinterpret_cast<dComIfG_inf_c*>(play)->play.mParticle);
                BstMove_setParticle(bstMoveOpaque(control), 2, 0xA1DB, &i_this->current.pos,
                                    nullptr, nullptr, 185, &i_this->mPa_smokeEcallBack, room, 0,
                                    nullptr, nullptr);
                i_this->m112A = (s16)((s16)bstMoveTuning().mRegS[7] + 4);
                bstDispatchSound(i_this, 0x698A);
                bstDispatchSound(i_this, 0x58DE);
                BstMove_anmInit(actor, dRes_INDEX_BST_BCK_KUTI_OPEN_e, one, 0, one, -1);
                state = (s16)i_this->mDamage;
                i_this->speed.y = zero;
                i_this->mDamage = (s16)(state + 1);
            }
        }
    } else if (state == 2) {
        u32 morph = (u32)i_this->m02B8.v;
        if (((u8)bstMoveMorf(morph)->mFrameCtrl.mState & 1) ||
            (f32)bstMoveMorf(morph)->mFrameCtrl.mRate == zero) {
            BstMove_anmInit(actor, dRes_INDEX_BST_BCK_KUTI_OPEN_LOOP_e, one, 2, one, -1);
            state = (s16)i_this->mDamage;
            i_this->m10FC[0] = 300;
            i_this->mDamage = (s16)(state + 1);
        }
    } else if (state == 3) {
        u32 bomb = 0;
        if (BstMove_checkCoHit(&i_this->mHandHurtCyl)) {
            u32 hit = BstMove_getCoHitObject(&i_this->mHandHurtCyl);
            if (hit) {
                u32 info = (u32)bstMoveCollisionObject(hit)->mpGObjInf;
                if (info)
                    bomb = (u32)bstMoveCollisionInfo(info)->mpActor;
            }
        }
        bool captured = false;
        if (bomb && (s16)bstMoveActor(bomb)->mProcName == 0x126 &&
            !BstMove_bombCheckExplode(bstMoveOpaque(bomb))) {
            if (BstMove_bombGetRestTime(bstMoveOpaque(bomb)) > 1) {
                i_this->mBombId = (u32)bstMoveActor(bomb)->mBsPcId;
                BstMove_bombSetCarry(bstMoveOpaque(bomb));
                BstMove_bombChangeState(bstMoveOpaque(bomb), 2);
                BstMove_bombSetNoHit(bstMoveOpaque(bomb));
                BstMove_bombSetRestTime(bstMoveOpaque(bomb), 200);
                state = (s16)i_this->mDamage;
                i_this->mDamage = (s16)(state + 1);
                i_this->m10FC[0] = 10;
                captured = true;
            }
        }
        if (captured)
            processBomb = true;
        else if ((s16)i_this->m10FC[0] == 0) {
            i_this->mActionType = ACTION_HEAD_HUKKI_e;
            i_this->mDamage = 0;
            BstMove_bgmStart(0x80000122);
        }
    }
    bool checkExplosion = entryState == 5;
    if (processBomb) {
        u32 bomb = bstHeldBomb(i_this);
        if (bomb) {
            bstMoveActor(bomb)->gravity = zero;
            bstMoveActor(bomb)->speedF = zero;
            bstMoveActor(bomb)->speed.x = zero;
            bstMoveActor(bomb)->speed.y = zero;
            bstMoveActor(bomb)->speed.z = zero;
            bstMoveActor(bomb)->current.angle.x = 0;
            bstMoveActor(bomb)->current.angle.y = 0;
            bstMoveActor(bomb)->current.angle.z = 0;
            bstMoveActor(bomb)->shape_angle.x = 0;
            bstMoveActor(bomb)->shape_angle.y = 0;
            bstMoveActor(bomb)->shape_angle.z = 0;
            f32 target = (f32)i_this->current.pos.x, step = (f32)bstMoveConstants().mThirty;
            BstMove_approach(&bstMoveActor(bomb)->current.pos.x, target, one, step);
            target = gabi::fadds_ppc((f32)i_this->current.pos.y, (f32)bstMoveConstants().mNinety);
            target = gabi::fadds_ppc(target, (f32)bstMoveTuning().mRegF[11]);
            step = (f32)bstMoveConstants().mThirty;
            BstMove_approach(&bstMoveActor(bomb)->current.pos.y, target, one, step);
            target = (f32)i_this->current.pos.z;
            step = (f32)bstMoveConstants().mThirty;
            BstMove_approach(&bstMoveActor(bomb)->current.pos.z, target, one, step);
        }
        if ((s16)i_this->m10FC[0] == 0) {
            BstMove_anmInit(actor, dRes_INDEX_BST_BCK_BOM_NOMI_e, one, 0, one, -1);
            state = (s16)i_this->mDamage;
            i_this->mDamage = (s16)(state + 1);
            void* play = BstMove_play();
            u32 control =
                gabi::ea((dPa_control_c*)reinterpret_cast<dComIfG_inf_c*>(play)->play.mParticle);
            u32 emitter =
                BstMove_setParticle(bstMoveOpaque(control), 0, 0x81E3, &i_this->current.pos,
                                    nullptr, nullptr, 255, nullptr, -1, 0, nullptr, nullptr);
            i_this->m2EE0[0].v = emitter;
            if (i_this)
                bstDispatchSound(i_this, 0x58E1);
            u32 morph = (u32)i_this->m02B8.v;
            i_this->m10FC[0] = 60;
            checkExplosion = true;
        }
    }
    if (entryState == 5) {
        s16 timer = (s16)i_this->m10FC[0];
        if (timer == 50 && i_this) {
            bstDispatchSound(i_this, 0x58E3);
            timer = (s16)i_this->m10FC[0];
        }
        if (timer == 30 && i_this)
            bstDispatchSound(i_this, 0x698B);
    }
    if (checkExplosion) {
        u32 morph = (u32)i_this->m02B8.v;
        if (((u8)bstMoveMorf(morph)->mFrameCtrl.mState & 1) ||
            (f32)bstMoveMorf(morph)->mFrameCtrl.mRate == zero) {
            bstRetireEmitter(&i_this->m2EE0[0]);
            u32 bomb = bstHeldBomb(i_this);
            if (bomb)
                BstMove_deleteActor(bstMoveOpaque(bomb));
            s8 health = (s8)((u8) * reinterpret_cast<be<u8>*>(&i_this->health) - 1);
            i_this->health = health;
            if (health <= 0 || (u8)bstMoveGlobals().m6 != 0) {
                void* play = BstMove_play();
                // HD: clear the pending life-item count on the death transition.
                reinterpret_cast<dComIfG_inf_c*>(play)->play.mItemLifeCount = zero;
                i_this->mActionType = ACTION_END_DEMO_e;
                i_this->mDamage = 0;
                i_this->m2E9A = 50;
                BstMove_anmInit(actor, dRes_INDEX_BST_BCK_BST_DEAD_e, one, 0, one, -1);
                bstMoveActor((u32)bstMoveGlobals().hand[0])->mActionType = ACTION_END_DEMO_e;
                bstMoveActor((u32)bstMoveGlobals().hand[0])->mDamage = 10;
                bstMoveActor((u32)bstMoveGlobals().hand[1])->mActionType = ACTION_END_DEMO_e;
                bstMoveActor((u32)bstMoveGlobals().hand[1])->mDamage = 10;
                bstDispatchSound(i_this, 0x2828);
                BstMove_bgmStop(30);
                BstMove_streamBgmStart(0xC0000019);
            } else {
                BstMove_anmInit(actor, dRes_INDEX_BST_BCK_BOM_BAKUHATU_e, one, 0, one, -1);
                state = (s16)i_this->mDamage;
                i_this->mDamage = (s16)(state + 1);
            }
            bstDispatchSound(i_this, 0x698C);
            for (u32 i = 0; i < 4; ++i) {
                u16 effect = (u16)bstMoveBombEffects()[i];
                void* play = BstMove_play();
                u32 control = gabi::ea(
                    (dPa_control_c*)reinterpret_cast<dComIfG_inf_c*>(play)->play.mParticle);
                u32 emitter =
                    BstMove_setParticle(bstMoveOpaque(control), 0, effect, &i_this->current.pos,
                                        nullptr, nullptr, 255, nullptr, -1, 0, nullptr, nullptr);
                i_this->m2EE0[i + 1].v = emitter;
            }
            i_this->m10FC[3] = 81;
        }
    } else if (entryState == 6) {
        u32 morph = (u32)i_this->m02B8.v;
        s32 frame = gabi::ftoi((f32)bstMoveMorf(morph)->mFrameCtrl.mFrame);
        if (frame == 8 && i_this) {
            bstDispatchSound(i_this, 0x698D);
            morph = (u32)i_this->m02B8.v;
        }
        if (((u8)bstMoveMorf(morph)->mFrameCtrl.mState & 1) ||
            (f32)bstMoveMorf(morph)->mFrameCtrl.mRate == zero) {
            state = (s16)i_this->mDamage;
            i_this->m10FC[0] = 30;
            i_this->mDamage = (s16)(state + 1);
            BstMove_bgmStart(0x80000023);
        }
    } else if (entryState == 7 && (s16)i_this->m10FC[0] == 0) {
        i_this->mActionType = ACTION_HEAD_HUKKI_e;
        i_this->mDamage = 0;
    }
    for (u32 i = 0; i < 5; ++i) {
        BstEmitter* emitter = bstEmitter(i_this->m2EE0[i]);
        if (!emitter)
            continue;
        if (i > 0 && (s16)i_this->m10FC[3] == 1) {
            emitter->becomeInvalidEmitter();
            i_this->m2EE0[i] = nullptr;
        } else {
            u32 morph = (u32)i_this->m02B8.v,
                model = gabi::ea((J3DModel*)bstMoveMorf(morph)->mpModel),
                block = gabi::ea((J3DMtxBuffer_hd*)bstMoveModel(model)->mpMtxBuffer);
            u16 flags = (u16)bstMoveMatrixBuffer(block)->mFlags;
            u32 matrices = (u32)bstMoveMatrixBuffer(block)->mpAnmMtx;
            bstMoveMatrixBuffer(block)->mFlags = flags | 0x10;
            BstMove_setEmitterMatrix(bstMoveOpaque(matrices),
                                     &emitter->mGlobalRotation, &emitter->mGlobalTranslation);
        }
    }
}

/* inlined into daBst_move */
static void head_hukki(bst_class* actor, f32 zero, f32 one) {
    bst_class* i_this = actor;
    s16 state = (s16)i_this->mDamage;
    if (state == 0) {
        i_this->mDamage = (s16)(state + 1);
        s16 duration = (s16)bstMoveTuning().mRegS[0];
        i_this->m10F8 = zero;
        i_this->m10EC.y = zero;
        i_this->m10FC[0] = (s16)(duration + 40);
        state = 1;
    }
    if (state == 1) {
        f32 strength = (f32)bstMoveTuning().mRegF[4],
            fourThousand = (f32)bstMoveConstants().mFourThousand,
            hundred = (f32)bstMoveConstants().mHundred;
        f32 step = (f32)bstMoveTuning().mRegF[5];
        BstMove_approach(&i_this->m10F8, gabi::fadds_ppc(strength, fourThousand), one,
                         gabi::fadds_ppc(step, hundred));
        if ((s16)i_this->m10FC[0] == 0) {
            state = (s16)i_this->mDamage;
            i_this->mDamage = (s16)(state + 1);
            i_this->m10FC[0] = (s16)((s16)bstMoveTuning().mRegS[1] + 80);
        }
    } else if (state == 2) {
        f32 step = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[6], (f32)bstMoveConstants().mFifty);
        BstMove_approachZero(&i_this->m10F8, one, step);
        if ((s16)i_this->m10FC[0] == 0) {
            i_this->speedF = zero;
            i_this->m10EC.y = zero;
            i_this->mActionType = ACTION_FLY_e;
            i_this->mDamage = 0;
            i_this->mEyeHealth[1] = 2;
            i_this->mEyeHealth[0] = 2;
        }
    }
    s16 frequency = (s16)bstMoveTuning().mRegS[6],
        frame = (s16)i_this->mUpdateLastFacingDirIfMultipleOf32;
    f32 strength = (f32)i_this->m10F8,
        sine = (f32)bstMoveTrig()[((u16)(frame * (frequency + 700)) >> 3)].mSin;
    i_this->shape_angle.x = (s16)gabi::ftoi(gabi::fmuls_ppc(sine, strength));
    frequency = (s16)bstMoveTuning().mRegS[6];
    frame = (s16)i_this->mUpdateLastFacingDirIfMultipleOf32;
    strength = (f32)i_this->m10F8;
    sine = (f32)bstMoveTrig()[((u16)(frame * (frequency + 600)) >> 3)].mSin;
    f32 fourHundred = (f32)bstMoveConstants().mFourHundred, forty = (f32)bstMoveConstants().mForty;
    s16 roll = (s16)gabi::ftoi(gabi::fmuls_ppc(sine, strength));
    f32 progress = (f32)i_this->m10EC.y;
    i_this->shape_angle.z = roll;
    f32 height = (f32)bstMoveTuning().mRegF[11];
    f32 step = gabi::fmuls_ppc(forty, progress),
        ratio = (f32)bstMoveConstants().mPositionChaseRatio;
    BstMove_approach(&i_this->current.pos.y, gabi::fadds_ppc(height, fourHundred), ratio, step);
    f32 shake = gabi::fadds_ppc((f32)bstMoveTuning().mRegF[11], (f32)bstMoveConstants().mFifty);
    step = (f32)bstMoveConstants().mTwo;
    BstMove_approach(&i_this->m10EC.z, shake, one, step);
    BstMove_approach(&i_this->m10EC.y, one, one, (f32)bstMoveConstants().mProgressStep);
}

/* 020EB4BC */
void daBst_move(bst_class* actor) {

    bst_class* i_this = actor;
    u8 part = (u8)i_this->mBstPartType;
    f32 zero = (f32)bstMoveConstants().mZero;
    if (part) {
        u32 head = (u32)bstMoveGlobals().boss;
        s8 phase = (s8)bstMoveActor(head)->m2E9A;
        if ((u32)((s32)phase - 1) < 9) {
            s8 action = (s8)bstMoveActor(head)->m2E7C;
            i_this->mActionType = action == 11 ? ACTION_FLY_e : ACTION_SLEEP_e;
            i_this->mDamage = 0;
            i_this->speedF = zero;
            return;
        }
    }
    // HD: the following CO call has no arguments in the verified dispatch.
    BstMove_setHandAt(actor, 0);
    BstMove_setHandCoNoArgs();
    u32 flags = (u32) * reinterpret_cast<be<u32>*>(&i_this->mHeadHurtCyl);
    f32 one = (f32)bstMoveConstants().mOne;
    s16 action = (s16)i_this->mActionType;
    *reinterpret_cast<be<u32>*>(&i_this->mHeadHurtCyl) = flags & ~1u;
    f32 half = (f32)bstMoveConstants().mHalf, ten = (f32)bstMoveConstants().mTen;
    bool movement = false;
    switch (action) {
    case ACTION_STAY_e:
        stay(actor, zero, one, ten);
        movement = true;
        break;
    case ACTION_FLY_e:
        fly(actor, zero, one, half, ten);
        movement = true;
        break;
    case ACTION_DAMAGE_e:
        damage(actor, zero, one);
        break;
    case ACTION_SLEEP_e:
        sleep(actor, zero, one, ten);
        break;
    case ACTION_HEAD_DAMAGE_e:
        head_damage(actor, zero, one);
        break;
    case ACTION_HEAD_HUKKI_e:
        head_hukki(actor, zero, one);
        break;
    case ACTION_DOWN_ATTACK_e:
        down_attack(actor, zero, one, half, ten);
        movement = true;
        break;
    case ACTION_PAA_ATTACK_e:
        BstMove_paaAttack(actor);
        movement = true;
        break;
    case ACTION_KUMI_ATTACK_e:
        BstMove_kumiAttack(actor);
        movement = true;
        break;
    case ACTION_HARAI_ATTACK_e:
        BstMove_haraiAttack(actor);
        movement = true;
        break;
    case ACTION_BEAM_ATTACK_e:
        BstMove_beamAttack(actor);
        break;
    case ACTION_HANA_DEMO_e:
        BstMove_startDemo(actor);
        break;
    case ACTION_END_DEMO_e:
        BstMove_endDemo(actor);
        break;
    default:
        break;
    }
    if (movement) {
        cXyz delta;
        BstMove_vectorSubtractResult(&i_this->current.pos, &delta, &i_this->old.pos);
        f32 square = BstMove_magnitudeSquared(&delta), distance = BstMove_sqrt(square);
        f32 scaled = gabi::fmuls_ppc(distance, (f32)bstMoveConstants().mSoundDistanceScale),
            limit = (f32)bstMoveConstants().mUnsignedFloatSplit;
        u32 intensity = !(scaled < limit)
                            ? (u32)gabi::ftoi(gabi::fsubs_ppc(scaled, limit)) + 0x80000000u
                            : (u32)gabi::ftoi(scaled);
        part = (u8)i_this->mBstPartType;
        if (intensity > 100)
            intensity = 100;
        BstMove_soundSe(actor, part ? 0x7036 : 0x7038, intensity);
    }
    BstMove_damageCheck(actor);
    if (gabi::ea(&i_this->mStts.m_cc_move)) {
        f32 offset = (f32)i_this->mStts.m_cc_move.x, x = (f32)i_this->current.pos.x;
        x = gabi::fadds_ppc(x, offset);
        f32 y = (f32)i_this->current.pos.y;
        i_this->current.pos.x = x;
        offset = (f32)i_this->mStts.m_cc_move.y;
        i_this->current.pos.y = gabi::fadds_ppc(y, offset);
        offset = (f32)i_this->mStts.m_cc_move.z;
        f32 z = (f32)i_this->current.pos.z;
        i_this->current.pos.z = gabi::fadds_ppc(z, offset);
    }
    f32 recoil = (f32)i_this->mHeadHurtSpeed, threshold = (f32)bstMoveConstants().mRecoilCutoff;
    if (recoil > threshold) {
        cXyz local;
        cXyz delta;
        local.x = zero;
        local.y = zero;
        local.z = recoil;
        u32 matrix = (u32)bstMoveCalcMatrix();
        s16 yaw = (s16)i_this->mHurtRecoilAngle1;
        BstMove_rotateY(bstMoveOpaque(matrix), yaw);
        matrix = (u32)bstMoveCalcMatrix();
        s16 pitch = (s16)i_this->mHurtRecoilAngle2;
        BstMove_rotateXMultiply(bstMoveOpaque(matrix), pitch);
        BstMove_transformVector(&local, &delta);
        BstMove_vectorAdd(&i_this->current.pos, &delta, &i_this->current.pos);
        BstMove_approachZero(&i_this->mHeadHurtSpeed, one, (f32)bstMoveConstants().mRecoilDecay);
    }
}

/* WWHD glue
ext 026066C4 void* BstMove_getResource(void*, SafeString*, u32)
ext 025E8154 void BstMove_initBrk(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32)
ext 025E7CE0 void BstMove_initBtk(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32)
ext 025200D4 void* BstMove_play()
ext 020E4790 void BstMove_setHandCo(bst_class*, s32)
ext 020198D8 f32 BstMove_random(f32)
ext 02019918 f32 BstMove_randomSymmetric(f32)
ext 020E47D0 void BstMove_anmInit(bst_class*, s32, f32, s32, f32, s32)
ext 025F1884 void BstMove_rotateY(void*, s16)
ext 025F1BF4 void BstMove_rotateXMultiply(void*, s16)
ext 0200FCD8 void BstMove_transformVector(cXyz*, cXyz*)
ext 0201AD78 void BstMove_vectorAddResult(cXyz*, cXyz*, cXyz*)
ext 0201ADE0 void BstMove_vectorSubtractResult(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f32 BstMove_magnitudeSquared(cXyz*)
ext 028F4384 f32 BstMove_sqrt(f32)
ext 0200ED84 void BstMove_approach(be<f32>*, f32, f32, f32)
ext 0200EDC8 void BstMove_approachZero(be<f32>*, f32, f32)
ext 025D6894 s16 BstMove_targetAngleY(bst_class*, void*)
ext 0200F428 void BstMove_approachAngle(be<s16>*, s32, s32, s32)
ext 020E4CD0 void BstMove_moveFly(bst_class*, s32)
ext 02520540 s8 BstMove_reverb(s8)
ext 025E1A40 void BstMove_startSound(u32, cXyz*, s32, s8)
ext 025E18EC void BstMove_bgmStart(u32)
ext 025D5218 u32 BstMove_searchActor(void*, BstMoveID*)
ext 025CB374 void BstMove_startShock(void*, s32, s32, cXyz*)
ext 025A847C u32 BstMove_setParticle(void*, s32, u32, cXyz*, void*, void*, s32, void*, s32, s32, void*, void*)
ext 02516464 BOOL BstMove_checkCoHit(dCcD_Cyl*)
ext 025163BC u32 BstMove_getCoHitObject(dCcD_Cyl*)
ext 020CB678 BOOL BstMove_bombCheckExplode(void*)
ext 020CB648 s32 BstMove_bombGetRestTime(void*)
ext 020CB6A8 void BstMove_bombSetCarry(void*)
ext 020CB978 void BstMove_bombChangeState(void*, s32)
ext 020CB710 void BstMove_bombSetNoHit(void*)
ext 020CB860 void BstMove_bombSetRestTime(void*, s32)
ext 025D57E0 void BstMove_deleteActor(void*)
ext 025E1904 void BstMove_bgmStop(s32)
ext 025E1934 void BstMove_streamBgmStart(u32)
ext 028249B0 void BstMove_setEmitterMatrix(void*, Mtx34*, cXyz*)
ext 028E90D4 void BstMove_matrixCopy(void*, void*)
ext 025A8D40 void BstMove_setSimpleParticle(void*, u32, cXyz*, s32, GXColor*, GXColor*, s32)
ext 020E474C void BstMove_setHandAt(bst_class*, s32)
ext 020E888C void BstMove_paaAttack(bst_class*)
ext 020E90F0 void BstMove_kumiAttack(bst_class*)
ext 020E9AA0 void BstMove_haraiAttack(bst_class*)
ext 020E9F08 void BstMove_beamAttack(bst_class*)
ext 020EACE0 void BstMove_startDemo(bst_class*)
ext 020EB174 void BstMove_endDemo(bst_class*)
ext 020E880C void BstMove_soundSe(bst_class*, u32, u32)
ext 020EA6B4 void BstMove_damageCheck(bst_class*)
ext 028E8D88 void BstMove_vectorAdd(cXyz*, cXyz*, cXyz*)
ext 020E4790 void BstMove_setHandCoNoArgs()
*/
