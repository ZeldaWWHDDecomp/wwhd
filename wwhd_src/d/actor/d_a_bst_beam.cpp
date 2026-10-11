#include "d/actor/d_a_bst_beam.h"

static BstBeamChargeEffects charge_e_name;

static void beamSound(fopAc_ac_c* actor, u32 id, bool actorCheck = true) {
    // HD: the closing sound checks the eye-position address without checking the actor.
    if ((!actorCheck || actor) && gabi::ea(&actor->eyePos)) {
        s8 room = actor->current.roomNo;
        s8 reverb = static_cast<s8>(dComIfGp_getReverb(room));
        mDoAud_seStart(id, &actor->eyePos, 0, reverb);
    }
}

/* 020E9F08 */
static void beam_attack(bst_class* actor) {
    bst_class* i_this = actor;
    BstBeamPlay* play = BstBeam_getPlay();
    s16 timer = i_this->mBombArrowDropTimer;
    fopAc_ac_c* player = play->mpPlayer;
    i_this->m2E74[0] = 10;
    i_this->m2E74[1] = 10;
    if (timer < 120) i_this->mBombArrowDropTimer = 120;
    gabi::Local<cXyz> actor_vec;
    cXyz_mi(&player->current.pos, actor_vec.get(), &actor->current.pos);
    f32 dx = actor_vec->x, dz = actor_vec->z;
    f32 dy = gabi::fadds_ppc(static_cast<f32>(actor_vec->y), i_this->m10D0);
    s16 yaw = cM_atan2s(dx, dz);
    cLib_addCalcAngleS2(&actor->shape_angle.y, yaw, 4, 0x800);
    // HD: fmadds forms the sum in double precision before rounding single.
    f32 square = static_cast<f32>(static_cast<f64>(dx) * static_cast<f64>(dx) + static_cast<f64>(gabi::fmuls_ppc(dz, dz)));
    f32 distance = std_sqrtf(square);
    s16 pitch = cM_atan2s(dy, distance);
    cLib_addCalcAngleS2(&actor->shape_angle.x, static_cast<s16>(-pitch), 4, 0x800);
    f32 height = REG0_F(11);
    cLib_addCalc2(&actor->current.pos.y, gabi::fadds_ppc(height, 400.0f), 0.05f, 30.0f);
    s16 state = i_this->mDamage;
    bool finished = false;
    switch (state) {
    case 0:
        i_this->mDamage = static_cast<s16>(state + 1);
        BstBeam_anm_init(actor, dRes_INDEX_BST_BCK_BEAM_MAE_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        beamSound(actor, JA_SE_CM_BST_MOUTH_OPEN);
        break;
    case 1: {
        if (i_this->m02B8->isStop()) {
            i_this->mDamage = static_cast<s16>(state + 1);
            BstBeam_anm_init(actor, dRes_INDEX_BST_BCK_BEAM_TAME_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->m10FC[0] = 45;
            for (u32 i = 0; i < 2; ++i) {
                u16 effect = charge_e_name.mName[i];
                play = BstBeam_getPlay();
                dPa_control_c* control = play->mpParticle;
                JPABaseEmitter* emitter = dPa_control_set(control, 0, effect, &actor->current.pos, nullptr, nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr);
                i_this->m2ED8[i] = emitter;
            }
        }
        break;
    }
    case 2: {
        beamSound(actor, JA_SE_CM_BST_BEAM_PREPARE);
        timer = i_this->m10FC[0];
        i_this->mRoomState = 5;
        if (timer == 0) {
            for (u32 i = 0; i < 2; ++i) {
                BstEmitter* emitter = bstEmitter(i_this->m2ED8[i]);
                if (emitter) {
                    emitter->becomeInvalidEmitter();
                    i_this->m2ED8[i] = nullptr;
                }
            }
            s16 current = i_this->mDamage;
            i_this->mDamage = static_cast<s16>(current + 1);
            BstBeam_anm_init(actor, dRes_INDEX_BST_BCK_BEAM_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->m10FC[0] = static_cast<s16>(REG0_S(4) + 50);
        }
        break;
    }
    case 3: {
        f32 acceleration = REG0_F(5);
        f32 target = REG0_F(6);
        cLib_addCalc2(&i_this->m10D0, target, 0.1f, gabi::fadds_ppc(acceleration, 20.0f));
        timer = i_this->m10FC[0];
        if ((timer & 3) == 0) {
            i_this->mRoomState = 6;
            // HD: beam_set is inlined and marks the HD animation matrix buffer dirty.
            for (u32 i = 0; i < 10; ++i) {
                if (i_this->m04E4[i] != 0) continue;
                i_this->m04E4[i] = 1;
                s32 joint = REG0_S(5) + BST_JNT_BUKI2_e;
                Mtx34* anmMtx = i_this->m02B8->getModel()->getAnmMtx(joint);
                Mtx34* matrix = calc_mtx();
                PSMTXCopy(anmMtx, matrix);
                gabi::Local<cXyz> vec;
                vec->x = 0.0f; vec->y = 0.0f; vec->z = 0.0f;
                MtxPosition(vec.get(), &i_this->m03B8[i]);
                pitch = actor->shape_angle.x;
                i_this->m04A8[i].x = pitch;
                yaw = actor->shape_angle.y;
                i_this->m04A8[i].y = yaw;
                i_this->m04A8[i].z = actor->shape_angle.z;
                matrix = calc_mtx();
                cMtx_YrotS(matrix, yaw);
                matrix = calc_mtx();
                pitch = i_this->m04A8[i].x;
                cMtx_XrotM(matrix, pitch);
                f32 speed = REG0_F(12);
                vec->x = 0.0f; vec->y = 0.0f; vec->z = gabi::fadds_ppc(speed, 160.0f);
                MtxPosition(vec.get(), &i_this->m0430[i]);
                i_this->m10A8[i] = 1.0f;
                break;
            }
            beamSound(actor, JA_SE_CM_BST_BEAM_FIRE);
            timer = i_this->m10FC[0];
        }
        finished = timer == 0;
        break;
    }
    }
    play = BstBeam_getPlay();
    player = play->mpPlayer;
    distance = fopAcM_searchActorDistance(actor, player);
    f32 boundary = gabi::fadds_ppc(REG0_F(9), 500.0f);
    if (distance < boundary || finished) {
        i_this->mRoomState = 8;
        i_this->mActionType = bst_class::ACTION_FLY_e;
        i_this->mDamage = 0;
        actor->speedF = 0.0f;
        beamSound(actor, JA_SE_CM_BST_MOUTH_CLOSE, false);
        s8 health = static_cast<s8>(actor->health);
        f32 wait;
        if (health == 0) wait = gabi::fadds_ppc(cM_rndF(50.0f), 100.0f);
        else if (health == 1) {
            f32 random = cM_rndF(70.0f);
            wait = gabi::fadds_ppc(random, 120.0f);
        } else if (health == 2) {
            f32 random = cM_rndF(80.0f);
            wait = gabi::fadds_ppc(random, 140.0f);
        } else {
            f32 maximum = health == 3 ? 90.0f : 100.0f;
            wait = gabi::fadds_ppc(cM_rndF(maximum), 150.0f);
        }
        i_this->m2E7E[1] = static_cast<s16>(gabi::ftoi(wait));
    }
    for (u32 i = 0; i < 2; ++i) {
        BstEmitter* emitter = bstEmitter(i_this->m2ED8[i]);
        if (!emitter) continue;
        Mtx34* anmMtx = i_this->m02B8->getModel()->getAnmMtx(0);
        JPASetRMtxTVecfromMtx(anmMtx, &emitter->mGlobalRotation, &emitter->mGlobalTranslation);
    }
}

/* WWHD glue
 data charge_e_name 10192858
 ext 025200D4 BstBeamPlay* BstBeam_getPlay()
 ext 020E47D0 void BstBeam_anm_init(bst_class*, s32, f32, u32, f32, s32)
 ext 028249B0 void JPASetRMtxTVecfromMtx(Mtx34*, Mtx34*, cXyz*)
*/
