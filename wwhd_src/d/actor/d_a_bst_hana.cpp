// Gohdan dispenses recovery items; verified HD nose animation and effects.
#include "d/actor/d_a_bst_hana.h"
#include "d/d_com_inf_game.h"
#include <cmath>

using namespace bst_attack_names;

// Gohdan moves toward the room center and dispenses arrows or bombs from its nose.
/* 020EACE0 */
static void hana_demo(bst_class* actor) {
    bst_class* i_this = actor;
    auto position = &i_this->current.pos;
    f32 x = position->x;
    f32 z = position->z;
    i_this->mState = 10;
    s16 angle = cM_atan2s(-x, -z);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, angle, 10, 0x400);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 10, 0x400);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, 0, 10, 0x400);
    f32 heightConstant = 400.0f;
    f32 tuningHeight = REG0_F(11);
    f32 maxStep = 30.0f;
    f32 height = gabi::fadds_ppc(tuningHeight, heightConstant);
    cLib_addCalc2(&i_this->current.pos.y, height, 0.05f, maxStep);
    s16 state = static_cast<s16>(i_this->mDamage);
    f32 zero = 0.0f;
    if (state == 0) {
        f32 animationSpeed = 1.0f;
        i_this->speedF = zero;
        f32 transition = 5.0f;
        i_this->mDamage = (s16)(state + 1);
        i_this->speed.y = zero;
        i_this->m10FC[0] = 40;
        BstAttack_anm_init(actor, dRes_INDEX_BST_BCK_HANA_DEMO_e, transition,
                           J3DFrameCtrl::EMode_NONE, animationSpeed, -1);
        i_this->m2E9A = 1;
        if (gabi::ea(position)) {
            s8 room = static_cast<s8>(i_this->current.roomNo);
            s8 reverb = dComIfGp_getReverb(room);
            mDoAud_seStart(JA_SE_CM_BST_HAND_STRAIN, position, 0, reverb);
        }
    } else if (state != 1) {
        return;
    }

    cXyz horizontal;
    cXyz itemPosition;
    // HD: interleaved word copies preserve floating-point payloads.
    bstAttackWords(&horizontal)->mWord[0] =
        static_cast<u32>(bstAttackWords(&actor->current.pos)->mWord[0]);
    bstAttackWords(&horizontal)->mWord[1] =
        static_cast<u32>(bstAttackWords(&actor->current.pos)->mWord[1]);
    u32 positionZ = static_cast<u32>(bstAttackWords(&actor->current.pos)->mWord[2]);
    horizontal.y = zero;
    bstAttackWords(&horizontal)->mWord[2] = positionZ;
    f32 square = PSVECSquareMag(&horizontal);
    f32 radius = std_sqrtf(square);
    f32 limitConstant = 1350.0f;
    f32 limit = gabi::fadds_ppc(REG0_F(9), limitConstant);
    if (radius > limit) {
        f32 ratio = limit / radius;
        f32 scale = 0.2f;
        f32 targetX = gabi::fmuls_ppc((f32)horizontal.x, ratio);
        cLib_addCalc2(&i_this->current.pos.x, targetX, scale, maxStep);
        f32 targetZ = gabi::fmuls_ppc((f32)horizontal.z, ratio);
        cLib_addCalc2(&i_this->current.pos.z, targetZ, scale, maxStep);
    }
    mDoExt_McaMorf* morph = i_this->m02B8;
    s32 frame = gabi::ftoi(morph->getFrame());
    if (frame > 26) {
        u32 id = static_cast<u32>(i_this->mCreatedItemId);
        fopAc_ac_c* item = nullptr;
        BstAttackProcessID lookup;
        lookup = id;
        if (id != 0xFFFFFFFF) {
            item = fopAcIt_Judge(SearchByIDCallback, &lookup);
        }
        if (item) {
            u32 flags = static_cast<u32>(item->actor_status);
            item->actor_status = flags | fopAcStts_UNK4000_e;
        }
        morph = i_this->m02B8;
    }
    frame = gabi::ftoi(morph->getFrame());
    if (frame != 26)
        return;

    // HD: fetch the save pointer before the model, then read its arrow count.
    BstAttackSave* save = bstAttackSave();
    BstAttackModel* model = reinterpret_cast<BstAttackModel*>(morph->mpModel.get());
    u8 arrows = static_cast<u8>(save->mArrowNum);
    BstAttackJointBlock* jointBlock = model->mpJointBlock;
    u16 flags = static_cast<u16>(jointBlock->mFlags);
    u32 itemNumber;
    if (arrows == 0) {
        Mtx34* matrices = jointBlock->mpAnmMtx;
        jointBlock->mFlags = flags | J3DCalcAnmMtx;
        Mtx34* matrix = calc_mtx();
        itemNumber = dItemNo_ARROW_10_e;
        PSMTXCopy(&matrices[BST_JNT_HANAL_e], matrix);
        i_this->m2E9E = 0;
    } else {
        Mtx34* matrices = jointBlock->mpAnmMtx;
        jointBlock->mFlags = flags | J3DCalcAnmMtx;
        Mtx34* matrix = calc_mtx();
        itemNumber = dItemNo_BOMB_5_e;
        PSMTXCopy(&matrices[BST_JNT_HANAR_e], matrix);
        i_this->m2E9E = 1;
    }
    f32 offsetX = REG0_F(2);
    f32 sixty = 60.0f;
    f32 offsetY = REG0_F(3);
    f32 offsetZ = REG0_F(4);
    horizontal.y = offsetY;
    horizontal.z = offsetZ;
    horizontal.x = gabi::fadds_ppc(offsetX, sixty);
    auto eye = &actor->eyePos;
    if (gabi::ea(eye)) {
        s8 room = static_cast<s8>(i_this->current.roomNo);
        s8 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(JA_SE_OBJ_BOMB_EXPLODE, eye, 0, reverb);
    }
    MtxPosition(&horizontal, &itemPosition);
    s8 room = static_cast<s8>(i_this->current.roomNo);
    u32 item = fopAcM_createItem(&itemPosition, itemNumber, -1, room, 0, nullptr, 0xB, nullptr);
    i_this->mCreatedItemId = item;
    if (gabi::ea(eye)) {
        s8 currentRoom = static_cast<s8>(i_this->current.roomNo);
        s8 reverb = dComIfGp_getReverb(currentRoom);
        mDoAud_seStart(JA_SE_CM_BST_ITEM_OUT_NOSE, eye, 0, reverb);
    }
    morph = i_this->m02B8;
    model = reinterpret_cast<BstAttackModel*>(morph->mpModel.get());
    jointBlock = model->mpJointBlock;
    flags = static_cast<u16>(jointBlock->mFlags);
    Mtx34* matrices = jointBlock->mpAnmMtx;
    jointBlock->mFlags = flags | J3DCalcAnmMtx;
    Mtx34* matrix = calc_mtx();
    PSMTXCopy(matrices, matrix);
    horizontal.x = zero;
    horizontal.y = zero;
    horizontal.z = zero;
    MtxPosition(&horizontal, &itemPosition);
    s8 side = static_cast<s8>(i_this->m2E9E);
    s8 smokeRoom = static_cast<s8>(i_this->current.roomNo);
    u16 effect = bstItemSmoke()->mEffect[side];
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    dPa_control_c* particles = play->play.mParticle.get();
    particles->set(2, effect, &itemPosition, &i_this->shape_angle, nullptr, 0xB9,
                   reinterpret_cast<dPa_levelEcallBack*>(&i_this->mPa_smokeEcallBack), smokeRoom, 0,
                   nullptr, nullptr);
}

/* WWHD glue (tools/verify/autoglue.py)
 */
