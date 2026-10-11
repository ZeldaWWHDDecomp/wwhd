#include "d/actor/d_a_bst_paa.h"
#include <cmath>

// Gohdan's two hands approach Link, clap together, then withdraw.
static BstPaaAnimations paa_bck_d;
static BstPaaHands hand;

/* 020E888C */
static void paa_attack(bst_class *actor) {
    bst_class *i_this = actor;
    BstPaaPlay *play = BstPaa_getPlay();
    fopAc_ac_c *player = play->mpPlayer;
    bool finish = false;
    bool move = true;
    cLib_addCalcAngleS2(&actor->shape_angle.x, 0, 10, 0x200);
    cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 10, 0x400);
    f32 scale = 1.0f;
    f32 ten = 10.0f;
    cLib_addCalc2(&i_this->m1118, -40.0f, scale, ten);
    s16 state = static_cast<s16>(i_this->mDamage);
    f32 forty = 40.0f;
    gabi::Local<cXyz> vector, intermediate, transformed;
    cXyz *target = &i_this->mTargetPos;
    cXyz *position = &actor->current.pos;
    auto distanceToTarget = [&]() {
        cXyz_mi(target, intermediate.get(), position);
        vector->copy(*intermediate);
        f32 square = PSVECSquareMag(vector.get());
        return std_sqrtf(square);
    };
    auto rotation = [&]() {
        s16 angle = static_cast<s16>(i_this->m111C);
        Mtx34 *matrix = calc_mtx();
        cMtx_YrotS(matrix, angle);
    };
    switch ((u32)(s32)state) {
    case 0: {
        f32 zero = 0.0f;
        i_this->mDamage = (s16)(state + 1);
        i_this->m10EC.y = zero;
        s16 playerFacing = static_cast<s16>(player->shape_angle.y);
        u8 part = i_this->mBstPartType;
        i_this->m111C = playerFacing;
        u16 animation = paa_bck_d.mAnimation[part];
        BstPaa_anm_init(actor, animation, ten, 2, scale, -1);
        s16 duration = REG0_S(1);
        i_this->m10FC[0] = (s16)(duration + 80);
        [[fallthrough]];
    }
    case 1: {
        f32 distance = distanceToTarget();
        f32 range = gabi::fadds_ppc(REG0_F(12), 200.0f);
        f32 acceleration = REG0_F(13);
        f32 increment = gabi::fadds_ppc(acceleration, 2.0f);
        if (distance > range)
            cLib_addCalc2(&actor->speedF, forty, scale, increment);
        else
            cLib_addCalc0(&actor->speedF, scale, increment);
        f32 radius = gabi::fadds_ppc(REG0_F(5), 8000.0f);
        i_this->m10EC.x = radius;
        rotation();
        bool left = (u8)i_this->mBstPartType == bst_class::Type_LEFT_HAND_e;
        f32 offsetX = REG0_F(2);
        f32 lateral = gabi::fadds_ppc(offsetX, 700.0f);
        if (left) {
            vector->y = REG0_F(3);
            vector->z = REG0_F(4);
            vector->x = -lateral;
        } else {
            vector->z = REG0_F(4);
            vector->y = REG0_F(3);
            vector->x = lateral;
        }
        MtxPosition(vector.get(), transformed.get());
        cXyz_pl(&player->current.pos, intermediate.get(), transformed.get());
        s16 savedFacing = static_cast<s16>(i_this->m111C);
        target->copy(*intermediate);
        cLib_addCalcAngleS2(&actor->shape_angle.y, (s16)(savedFacing + (left ? 0x4000 : -0x4000)), 10, 0x800);
        if (static_cast<s16>(i_this->m10FC[0]) == 0) {
            s16 current = static_cast<s16>(i_this->mDamage);
            i_this->mDamage = (s16)(current + 1);
            // HD: word copies preserve the player's floating-point payloads.
            BstPaaPositionWords *dst = reinterpret_cast<BstPaaPositionWords *>(target);
            BstPaaPositionWords *src = reinterpret_cast<BstPaaPositionWords *>(&player->current.pos);
            for (u32 i = 0; i < 3; ++i)
                dst->mWord[i] = static_cast<u32>(src->mWord[i]);
            BstPaaPlay *currentPlay = BstPaa_getPlay();
            fopAc_ac_c *currentPlayer = currentPlay->mpPlayer;
            s16 facing = BstPaa_searchActorAngleY(actor, currentPlayer);
            actor->current.angle.y = facing;
        }
        break;
    }
    case 2: {
        f32 speed = gabi::fadds_ppc(REG0_F(14), 140.0f);
        f32 step = gabi::fadds_ppc(REG0_F(15), ten);
        cLib_addCalc2(&actor->speedF, speed, scale, step);
        f32 radius = gabi::fadds_ppc(REG0_F(5), 10000.0f);
        i_this->m10EC.x = radius;
        f32 distance = distanceToTarget();
        f32 range = gabi::fadds_ppc(REG0_F(16), 200.0f);
        if (distance < range) {
            s16 current = static_cast<s16>(i_this->mDamage);
            i_this->mDamage = (s16)(current + 1);
            s16 duration = REG0_S(7);
            i_this->m10FC[0] = (s16)(duration + 30);
        }
        break;
    }
    case 3: {
        move = false;
        s16 timer = static_cast<s16>(i_this->m10FC[0]);
        if (timer >= 28) {
            if (timer == 28) {
                if ((u8)i_this->mBstPartType == bst_class::Type_LEFT_HAND_e) {
                    if (actor && gabi::ea(position)) {
                        s8 room = static_cast<s8>(actor->current.roomNo);
                        s8 reverb = static_cast<s8>(dComIfGp_getReverb(room));
                        mDoAud_seStart(JA_SE_CM_BST_HAND_CLAP, position, 0, reverb);
                    }
                    gabi::Local<csXyz> angles;
                    angles->x = static_cast<s16>(actor->shape_angle.x);
                    s16 yaw = static_cast<s16>(actor->shape_angle.y);
                    angles->y = yaw;
                    angles->y = (s16)(yaw + 0x4000);
                    angles->z = static_cast<s16>(actor->shape_angle.z);
                    BstPaaPlay *currentPlay = BstPaa_getPlay();
                    dPa_control_c *particles = currentPlay->mpParticle;
                    dPa_control_set(particles, 4, 0xC1D7, target, angles.get(), nullptr, 255, nullptr, -1, 0, nullptr,
                                    nullptr);
                    s8 room = static_cast<s8>(actor->current.roomNo);
                    currentPlay = BstPaa_getPlay();
                    particles = currentPlay->mpParticle;
                    dPa_control_set(particles, 2, 0xA1DA, target, angles.get(), nullptr, 0xB9,
                                    reinterpret_cast<dPa_levelEcallBack *>(&i_this->mPa_smokeEcallBack), room, 0,
                                    nullptr, nullptr);
                }
                s16 duration = REG0_S(8);
                i_this->m112A = (s16)(duration + 5);
            }
            BstPaa_set_hand_AT(actor, 1);
        }
        bool left = (u8)i_this->mBstPartType == bst_class::Type_LEFT_HAND_e;
        s16 savedFacing = static_cast<s16>(i_this->m111C);
        cLib_addCalcAngleS2(&actor->shape_angle.y, (s16)(savedFacing + (left ? 0x4000 : -0x4000)), 1, 0x1000);
        f32 maxX = std::fabs(static_cast<f32>(actor->speed.x));
        f32 targetX = target->x;
        cLib_addCalc2(&actor->current.pos.x, targetX, scale, maxX);
        f32 maxZ = std::fabs(static_cast<f32>(actor->speed.z));
        f32 targetZ = target->z;
        cLib_addCalc2(&actor->current.pos.z, targetZ, scale, maxZ);
        if (static_cast<s16>(i_this->m10FC[0]) == 0) {
            s16 current = static_cast<s16>(i_this->mDamage);
            f32 zero = 0.0f;
            i_this->mDamage = (s16)(current + 1);
            actor->speedF = zero;
            s16 duration = REG0_S(3);
            s16 facing = static_cast<s16>(actor->current.angle.y);
            s16 saved = static_cast<s16>(i_this->m111C);
            i_this->m10FC[0] = (s16)(duration + 30);
            actor->current.angle.y = (s16)(facing - 0x8000);
            Mtx34 *matrix = calc_mtx();
            cMtx_YrotS(matrix, saved);
            bool nowLeft = (u8)i_this->mBstPartType == bst_class::Type_LEFT_HAND_e;
            f32 offsetX = REG0_F(2);
            f32 lateral = gabi::fadds_ppc(offsetX, 4000.0f);
            if (nowLeft) {
                vector->y = REG0_F(3);
                vector->z = REG0_F(4);
                vector->x = -lateral;
            } else {
                vector->z = REG0_F(4);
                vector->y = REG0_F(3);
                vector->x = lateral;
            }
            MtxPosition(vector.get(), transformed.get());
            PSVECAdd(target, transformed.get(), target);
        }
        break;
    }
    case 4: {
        f32 acceleration = gabi::fadds_ppc(REG0_F(13), scale);
        cLib_addCalc2(&actor->speedF, forty, scale, acceleration);
        f32 radius = gabi::fadds_ppc(REG0_F(5), 2000.0f);
        s16 timer = static_cast<s16>(i_this->m10FC[0]);
        i_this->m10EC.x = radius;
        finish = timer == 0;
        break;
    }
    default:
        break;
    }
    if (move)
        BstPaa_pos_move(actor, 0);
    f32 floor = gabi::fadds_ppc(REG0_F(4), 30.0f);
    if (!(static_cast<f32>(actor->current.pos.y) > floor))
        actor->current.pos.y = floor;
    if (!finish && !i_this->mAcch.ChkWallHit())
        return;
    f32 wait = 100.0f;
    i_this->mActionType = bst_class::ACTION_FLY_e;
    i_this->mDamage = 0;
    f32 random = cM_rndF(wait);
    i_this->mStateTimer = (s16)gabi::ftoi(gabi::fadds_ppc(random, wait));
    if (finish)
        return;
    u8 part = i_this->mBstPartType;
    u32 otherIndex = 2u - part;
    if (otherIndex >= 2)
        return;
    gptr<bst_class> *slot = &hand.mHand[otherIndex];
    bst_class *other = *slot;
    if (static_cast<s16>(other->mActionType) != bst_class::ACTION_PAA_ATTACK_e)
        return;
    other->mActionType = bst_class::ACTION_FLY_e;
    other = *slot;
    other->mDamage = 0;
    other = *slot;
    random = cM_rndF(wait);
    other->mStateTimer = (s16)gabi::ftoi(gabi::fadds_ppc(random, wait));
}
/* WWHD glue
 data paa_bck_d 101928D0
 data hand 10462978
 ext 025200D4 BstPaaPlay* BstPaa_getPlay()
 ext 020E47D0 void BstPaa_anm_init(bst_class*, s32, f32, u32, f32, s32)
 ext 025D6894 s16 BstPaa_searchActorAngleY(fopAc_ac_c*, fopAc_ac_c*)
 ext 020E474C void BstPaa_set_hand_AT(bst_class*, u8)
 ext 020E4CD0 void BstPaa_pos_move(bst_class*, s32)
*/
