// Gohdan clasps both hands around Link, then drops them together.
#include "d/actor/d_a_bst_hana.h"
#include "d/d_com_inf_game.h"
#include <cmath>

using namespace bst_attack_names;

/* inlined into kumi_attack */
static void kumiSound(fopAc_ac_c* actor, u32 sound, bool checkActor = false) {
    if ((!checkActor || actor) && gabi::ea(&actor->current.pos)) {
        s8 room = static_cast<s8>(actor->current.roomNo);
        s8 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(sound, &actor->current.pos, 0, reverb);
    }
}

/* inlined into kumi_attack */
static f32 kumiDistance(cXyz* first, cXyz* second) {
    cXyz result;
    cXyz difference;
    cXyz_mi(first, &result, second);
    difference.copy(result);
    f32 square = PSVECSquareMag(&difference);
    return BstAttack_sqrtf(square);
}

/* 020E90F0 */
static void kumi_attack(bst_class* actor) {
    bst_class* i_this = actor;

    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 10, 0x200);
    f32 one = 1.0f, ten = 10.0f;
    f32 thirty = 30.0f;
    cLib_addCalc2(&i_this->m1110, thirty, one, ten);
    f32 target = -80.0f;
    cLib_addCalc2(&i_this->m1118, target, one, ten);
    thirty = 30.0f;
    target = -200.0f;
    cLib_addCalc2(&i_this->m1114, target, one, thirty);
    s16 state = static_cast<s16>(i_this->mDamage);
    f32 zero = 0.0f, hundred = 100.0f;
    bool move = true, success = false;
    switch (state) {
    case 0: {
        u8 part = static_cast<u8>(i_this->mBstPartType);
        i_this->m10EC.y = zero;
        i_this->mDamage = (s16)(state + 1);
        s16 yaw = static_cast<s16>(player->shape_angle.y);
        i_this->m111C = yaw;
        u16 animation = bstKumiAnimations()->mAnimation[part];
        BstAttack_anm_init(actor, animation, ten, J3DFrameCtrl::EMode_LOOP, one, -1);
        s16 duration = REG0_S(1);
        i_this->m10FC[0] = (s16)(duration + 70);
        [[fallthrough]];
    }
    case 1: {
        f32 distance = kumiDistance(&i_this->mTargetPos, &actor->current.pos);
        f32 boundary = REG0_F(12) + 200.0f;
        f32 acceleration = REG0_F(13);
        if (distance > boundary) {
            f32 two = 2.0f, forty = 40.0f;
            cLib_addCalc2(&i_this->speedF, forty, one, acceleration + two);
        } else {
            f32 two = 2.0f;
            cLib_addCalc0(&i_this->speedF, one, acceleration + two);
        }
        f32 turn = REG0_F(5) + 8000.0f;
        s16 yaw = static_cast<s16>(i_this->m111C);
        i_this->m10EC.x = turn;
        Mtx34* matrix = calc_mtx();
        cMtx_YrotS(matrix, yaw);
        u8 part = static_cast<u8>(i_this->mBstPartType);
        f32 x = REG0_F(2) + 500.0f;
        if (part == bst_class::Type_LEFT_HAND_e)
            x = -x;
        f32 y = REG0_F(3) + 400.0f;
        cXyz local;
        cXyz transformed;
        cXyz result;
        local.x = x;
        local.z = zero;
        local.y = y;
        MtxPosition(&local, &transformed);
        cXyz_pl(&player->current.pos, &result, &transformed);
        yaw = static_cast<s16>(i_this->m111C);
        i_this->mTargetPos.copy(result);
        cLib_addCalcAngleS2(
            &i_this->shape_angle.y,
            (s16)(yaw + (part == bst_class::Type_LEFT_HAND_e ? 0x4000 : -0x4000)), 10, 0x800);
        if (static_cast<s16>(i_this->m10FC[0]) == 0) {
            s16 current = static_cast<s16>(i_this->mDamage);
            f32 fiveHundred = 500.0f;
            i_this->mDamage = (s16)(current + 1);
            f32 px = static_cast<f32>(player->current.pos.x);
            i_this->mTargetPos.x = px;
            f32 py = static_cast<f32>(player->current.pos.y);
            i_this->mTargetPos.y = py;
            f32 pz = static_cast<f32>(player->current.pos.z);
            i_this->mTargetPos.z = pz;
            f32 height = REG0_F(3) + fiveHundred;
            i_this->mTargetPos.y = py + height;
            play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            fopAc_ac_c* currentPlayer = play->play.mPlayerInfo[0].mpPlayer;
            s16 angle = fopAcM_searchActorAngleY(actor, currentPlayer);
            i_this->current.angle.y = angle;
        }
        break;
    }
    case 2: {
        f32 speed = REG0_F(14), acceleration = REG0_F(15);
        f32 eight = 8.0f;
        cLib_addCalc2(&i_this->speedF, speed + hundred, one, acceleration + eight);
        f32 turn = REG0_F(5) + 10000.0f;
        i_this->m10EC.x = turn;
        f32 distance = kumiDistance(&i_this->mTargetPos, &actor->current.pos);
        f32 boundary = REG0_F(16) + hundred;
        if (distance < boundary) {
            s16 current = static_cast<s16>(i_this->mDamage);
            i_this->mDamage = (s16)(current + 1);
        }
        break;
    }
    case 3: {
        u8 part = static_cast<u8>(i_this->mBstPartType);
        s16 yaw = static_cast<s16>(i_this->m111C);
        cLib_addCalcAngleS2(
            &i_this->shape_angle.y,
            (s16)(yaw + (part == bst_class::Type_LEFT_HAND_e ? 0x4000 : -0x4000)), 1, 0x1000);
        f32 speed = static_cast<f32>(i_this->speed.x),
            goal = static_cast<f32>(i_this->mTargetPos.x);
        cLib_addCalc2(&i_this->current.pos.x, goal, one, std::fabs(speed));
        speed = static_cast<f32>(i_this->speed.y);
        goal = static_cast<f32>(i_this->mTargetPos.y);
        cLib_addCalc2(&i_this->current.pos.y, goal, one, std::fabs(speed) + ten);
        speed = static_cast<f32>(i_this->speed.z);
        goal = static_cast<f32>(i_this->mTargetPos.z);
        cLib_addCalc2(&i_this->current.pos.z, goal, one, std::fabs(speed));
        bst_class* left = bstAttackHands()->mHand[0];
        bst_class* right = bstAttackHands()->mHand[1];
        f32 distance = kumiDistance(&left->current.pos, &right->current.pos);
        if (distance < one) {
            s16 current = static_cast<s16>(i_this->mDamage);
            i_this->mDamage = (s16)(current + 1);
            s16 duration = REG0_S(3);
            i_this->m10FC[0] = (s16)(duration + 10);
            kumiSound(actor, JA_SE_CM_BST_HAND_SHAKE);
            i_this->speed.y = zero;
        }
        move = false;
        break;
    }
    case 4: {
        f32 velocity = static_cast<f32>(i_this->speed.y),
            y = static_cast<f32>(i_this->current.pos.y);
        s16 timer = static_cast<s16>(i_this->m10FC[0]);
        i_this->current.pos.y = y + velocity;
        if (timer > 1) {
            f32 acceleration = REG0_F(14), current = static_cast<f32>(i_this->speed.y);
            u8 part = static_cast<u8>(i_this->mBstPartType);
            i_this->speed.y = current + (acceleration + one);
            s16 roll = part == bst_class::Type_RIGHT_HAND_e ? -0x1000 : 0x1000;
            i_this->mFacingDir = roll;
            cLib_addCalcAngleS2(&i_this->shape_angle.z, roll, 10, 0x200);
        } else {
            u8 part = static_cast<u8>(i_this->mBstPartType);
            s16 roll = part == bst_class::Type_RIGHT_HAND_e ? 0x4000 : -0x4000;
            i_this->mFacingDir = roll;
            cLib_addCalcAngleS2(&i_this->shape_angle.z, roll, 1, 0x1000);
            BstAttack_set_hand_AT(actor, 1);
            f32 acceleration = REG0_F(3) + 60.0f;
            velocity = static_cast<f32>(i_this->speed.y);
            i_this->speed.y = velocity - acceleration;
            f32 floor = REG0_F(4) + 150.0f;
            y = static_cast<f32>(i_this->current.pos.y);
            // HD: unordered heights take the landing path.
            if (!(y > floor)) {
                i_this->current.pos.y = floor;
                part = static_cast<u8>(i_this->mBstPartType);
                i_this->speed.y = zero;
                if (part == bst_class::Type_LEFT_HAND_e) {
                    cXyz impact;
                    cXyz impulse;
                    impact.x = static_cast<f32>(i_this->mTargetPos.x);
                    impact.y = one;
                    impact.z = static_cast<f32>(i_this->mTargetPos.z);
                    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
                    dPa_control_c* particles = play->play.mParticle.get();
                    particles->set(4, ID_AK_SP_BSTSHOCKWAVEH00, &impact, nullptr, nullptr, 255,
                                   nullptr, -1, 0, nullptr, nullptr);
                    s8 room = static_cast<s8>(i_this->current.roomNo);
                    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
                    particles = play->play.mParticle.get();
                    particles->set(
                        2, ID_AK_ST_BSTATTACK2HSMOKE00, &impact, nullptr, nullptr, 185,
                        reinterpret_cast<dPa_levelEcallBack*>(&i_this->mPa_smokeEcallBack), room, 0,
                        nullptr, nullptr);
                    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
                    s32 strength = REG0_S(2) + 5;
                    impulse.x = zero;
                    impulse.y = one;
                    impulse.z = zero;
                    play->play.mVibration.StartShock(strength, -33, &impulse);
                    kumiSound(actor, JA_SE_CM_BST_2_HAND_ATTACK);
                }
                s16 current = static_cast<s16>(i_this->mDamage), duration = REG0_S(7);
                i_this->mDamage = (s16)(current + 1);
                i_this->m112A = (s16)(duration + 10);
                i_this->m10FC[0] = 40;
            }
        }
        move = false;
        break;
    }
    case 5:
        if (static_cast<s16>(i_this->m10FC[0]) == 0)
            success = true;
        move = false;
        break;
    }
    if (move)
        BstAttack_pos_move(actor, 0);
    bool collision = false;
    if (!success)
        collision = (i_this->mAcch.ChkWallHit()) != 0;
    if (!success && !collision)
        return;
    s16 yaw = static_cast<s16>(i_this->current.angle.y);
    i_this->speedF = zero;
    i_this->mActionType = bst_class::ACTION_FLY_e;
    i_this->mDamage = 0;
    i_this->current.angle.y = (s16)(yaw - 0x8000);
    f32 random = cM_rndF(hundred);
    i_this->mStateTimer = (s16)gabi::ftoi(random + hundred);
    i_this->mState = 40;
    kumiSound(actor, JA_SE_CM_BST_HAND_RELEASE);
    if (success)
        return;
    u32 other = 2u - static_cast<u8>(i_this->mBstPartType);
    // HD: malformed part values do not index the partner table.
    if (other >= 2)
        return;
    gptr<bst_class>* partner = &bstAttackHands()->mHand[other];
    bst_class* hand = *partner;
    if (static_cast<s16>(hand->mActionType) != bst_class::ACTION_KUMI_ATTACK_e)
        return;
    // HD: each write/call may replace the partner pointer; reload the slot.
    hand->mActionType = bst_class::ACTION_FLY_e;
    hand = *partner;
    hand->mDamage = 0;
    hand = *partner;
    hand->speedF = zero;
    hand = *partner;
    yaw = static_cast<s16>(hand->current.angle.y);
    hand->current.angle.y = (s16)(yaw - 0x8000);
    hand = *partner;
    random = cM_rndF(hundred);
    hand->mStateTimer = (s16)gabi::ftoi(random + hundred);
    hand = *partner;
    hand->mState = 40;
    hand = *partner;
    kumiSound(hand, JA_SE_CM_BST_HAND_RELEASE, true);
}

/* WWHD glue (tools/verify/autoglue.py)
 */
