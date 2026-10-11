// Gohdan sweeps a hand around the room; verified HD attack state machine.
#include "d/actor/d_a_bst_hana.h"
#include "d/d_com_inf_game.h"
#include <cmath>

using namespace bst_attack_names;

// Gohdan's sweeping hand attack; verified against the original HD routine.
/* 020E9AA0 */
static void harai_attack(bst_class* actor) {
    bst_class* i_this = actor;

    dComIfGp_get(); // HD: retain the otherwise unused play lookup.
    u8 moveMode = 0;
    BstAttack_set_hand_CO(actor, 1);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 10, 0x200);
    s16 facing = static_cast<s16>(i_this->current.angle.y);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, facing, 4, 0x1000);

    const bool rightHand = (u8)i_this->mBstPartType == bst_class::Type_RIGHT_HAND_e;
    f32 fifty = 50.0f;
    s16 roll = rightHand ? 0x4000 : -0x4000;
    i_this->mFacingDir = roll;
    f32 tilt = gabi::fadds_ppc(REG0_F(11), fifty);
    if (rightHand)
        tilt = -tilt;
    cLib_addCalcAngleS2(&i_this->shape_angle.z, roll, 8, 0x400);
    f32 scale = 1.0f;
    f32 maxStep = 10.0f;
    cLib_addCalc2(&i_this->m1110, tilt, scale, maxStep);

    s16 state = static_cast<s16>(i_this->mDamage);
    f32 speed = 60.0f;
    f32 zero = 0.0f;
    switch ((u32)(s32)state) {
    case 0: {
        i_this->m10EC.y = zero;
        i_this->speedF = zero;
        i_this->mDamage = (s16)(state + 1);
        s16 timer = REG0_S(5);
        i_this->m10FC[0] = (s16)(timer + 20);
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
        s16 playerAngle = fopAcM_searchActorAngleY(actor, player);
        u8 part = i_this->mBstPartType;
        i_this->current.angle.y = playerAngle;
        u16 animation = bstFlyAnimations()->mAnimation[part];
        BstAttack_anm_init(actor, animation, maxStep, J3DFrameCtrl::EMode_LOOP, scale, -1);
        [[fallthrough]];
    }
    case 1: {
        f32 acceleration = gabi::fadds_ppc(REG0_F(13), 2.0f);
        moveMode = 1;
        cLib_addCalc2(&i_this->speedF, speed, scale, acceleration);
        f32 radius = gabi::fadds_ppc(REG0_F(5), 2000.0f);
        s16 timer = static_cast<s16>(i_this->m10FC[0]);
        i_this->m10EC.x = radius;
        if (timer == 0) {
            s16 currentState = static_cast<s16>(i_this->mDamage);
            i_this->mDamage = (s16)(currentState + 1);
            s16 duration = REG0_S(6);
            i_this->current.angle.x = 0;
            i_this->m10FC[0] = (s16)(duration + 60);
        }
        break;
    }
    case 2: {
        s16 turn = (s16)(REG0_S(4) + 350);
        u8 part = i_this->mBstPartType;
        moveMode = 1;
        s16 currentAngle = static_cast<s16>(i_this->current.angle.y);
        u32 x = static_cast<u32>(bstAttackWords(&actor->current.pos)->mWord[0]);
        i_this->current.angle.y =
            (s16)(part == bst_class::Type_RIGHT_HAND_e ? currentAngle + turn
                                                            : currentAngle - turn);
        cXyz horizontal;
        bstAttackWords(&horizontal)->mWord[0] = x;
        u32 y = static_cast<u32>(bstAttackWords(&actor->current.pos)->mWord[1]);
        s16 timer = static_cast<s16>(i_this->m10FC[0]);
        bstAttackWords(&horizontal)->mWord[1] = y;
        u32 z = static_cast<u32>(bstAttackWords(&actor->current.pos)->mWord[2]);
        horizontal.y = zero;
        bstAttackWords(&horizontal)->mWord[2] = z;
        bool finish = timer == 0;
        if (!finish) {
            f32 square = PSVECSquareMag(&horizontal);
            f32 radius = std_sqrtf(square);
            f32 boundary = 1700.0f;
            finish = radius > boundary;
        }
        if (finish) {
            s16 currentState = static_cast<s16>(i_this->mDamage);
            i_this->current.angle.x = 0;
            i_this->m10FC[0] = 30;
            i_this->mDamage = (s16)(currentState + 1);
        }
        break;
    }
    case 3: {
        f32 deceleration = gabi::fadds_ppc(REG0_F(13), 3.0f);
        moveMode = 1;
        cLib_addCalc0(&i_this->speedF, scale, deceleration);
        if (static_cast<s16>(i_this->m10FC[0]) == 0) {
            i_this->mActionType = bst_class::ACTION_FLY_e;
            i_this->mDamage = 0;
            f32 random = cM_rndF(speed);
            i_this->mStateTimer = (s16)gabi::ftoi(gabi::fadds_ppc(random, speed));
        }
        break;
    }
    default:
        break;
    }
    i_this->current.angle.x = 0;
    f32 height = gabi::fadds_ppc(REG0_F(7), 180.0f);
    f32 heightScale = 0.2f;
    cLib_addCalc2(&i_this->current.pos.y, height, heightScale, maxStep);
    BstAttack_pos_move(actor, moveMode);
}

/* WWHD glue (tools/verify/autoglue.py)
 */
