// Gohdan hand attacks (hana/harai/kumi): tables and bindings; the actor is bst_class (d_a_bst.h).
#pragma once
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "d/d_bg_s.h"
#include "gc_api/d/d_item.h"
#include "d/actor/d_a_bst.h"
#include "d/d_com_inf_game.h"
struct BstAttackPositionWords {
    be<u32> mWord[3];
};


struct BstAttackJointBlock {
    u8 _0[4];
    be<u16> mFlags;
    u8 _6[10];
    gptr<Mtx34> mpAnmMtx;
};
struct BstAttackModel {
    u8 _0[0x2C];
    gptr<BstAttackJointBlock> mpJointBlock;
};
struct BstAttackSave {
    u8 _0[0x89];
    be<u8> mArrowNum;
};
struct BstAttackAnimations {
    be<u16> mAnimation[3];
};
struct BstAttackSmoke {
    be<u16> mEffect[2];
};
struct BstAttackHands {
    gptr<bst_class> mHand[2];
};
WWHD_OFFSET(BstAttackJointBlock, mpAnmMtx, 0x10);
WWHD_OFFSET(BstAttackModel, mpJointBlock, 0x2C);
WWHD_OFFSET(BstAttackSave, mArrowNum, 0x89);
inline BstAttackPositionWords* bstAttackWords(cXyz* position) {
    return reinterpret_cast<BstAttackPositionWords*>(position);
}
// Mapped references share the original TU data; they define no additional static objects.
inline BstAttackHands* bstAttackHands() {
    return gabi::at<BstAttackHands>(0x10462978);
}
inline BstAttackAnimations* bstFlyAnimations() {
    return gabi::at<BstAttackAnimations>(0x10192878);
}
inline BstAttackAnimations* bstKumiAnimations() {
    return gabi::at<BstAttackAnimations>(0x101928D8);
}
inline BstAttackSmoke* bstItemSmoke() {
    return gabi::at<BstAttackSmoke>(0x1019285C);
}
inline BstAttackSave* bstAttackSave() {
    return *gabi::at<gptr<BstAttackSave>>(0x101F84DC);
}
inline void BstAttack_anm_init(bst_class* actor, s32 animation, f32 morph, u32 mode, f32 speed,
                               s32 sound) {
    gabi::call(0x020E47D0, actor, animation, morph, mode, speed, sound);
}
inline void BstAttack_set_hand_CO(bst_class* actor, u8 enabled) {
    gabi::call(0x020E4790, actor, enabled);
}
inline void BstAttack_set_hand_AT(bst_class* actor, u8 enabled) {
    gabi::call(0x020E474C, actor, enabled);
}
inline void BstAttack_pos_move(bst_class* actor, s32 mode) {
    gabi::call(0x020E4CD0, actor, mode);
}
// HD: this call consumes and returns single precision; std_sqrtf returns f64.
inline f32 BstAttack_sqrtf(f32 square) {
    return gabi::call<f32>(0x028F4384, square);
}

// HD: these bindings retain the original register widths and call-site order.

enum BstAttackConstants {
    SearchByIDCallback = 0x025E1234,
    J3DCalcAnmMtx = 0x10,
    dItemNo_BOMB_5_e = 0xB
};

using BstAttackProcessID = be<u32>;

namespace bst_attack_names {
enum Resource { dRes_INDEX_BST_BCK_HANA_DEMO_e = 20 };
enum Joint { BST_JNT_HEAD_e = 0, BST_JNT_HANAL_e = 6, BST_JNT_HANAR_e = 7 };
enum Sound {
    JA_SE_CM_BST_HAND_SHAKE = 0x58DA,
    JA_SE_CM_BST_HAND_RELEASE = 0x58DB,
    JA_SE_CM_BST_HAND_STRAIN = 0x58E2,
    JA_SE_OBJ_BOMB_EXPLODE = 0x6901,
    JA_SE_CM_BST_ITEM_OUT_NOSE = 0x6987,
    JA_SE_CM_BST_2_HAND_ATTACK = 0x6993,
};
enum Particle { ID_AK_SP_BSTSHOCKWAVEH00 = 0xC1D6, ID_AK_ST_BSTATTACK2HSMOKE00 = 0xA1D9 };
} // namespace bst_attack_names
WWHD_SIZE(BstAttackPositionWords, 0xC);
WWHD_SIZE(BstAttackJointBlock, 0x14);
WWHD_SIZE(BstAttackModel, 0x30);
WWHD_SIZE(BstAttackSave, 0x8A);
WWHD_SIZE(BstAttackAnimations, 6);
WWHD_SIZE(BstAttackSmoke, 4);
WWHD_SIZE(BstAttackHands, 8);
