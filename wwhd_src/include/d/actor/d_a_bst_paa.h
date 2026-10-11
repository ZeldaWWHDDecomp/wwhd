#pragma once
#include "d/actor/d_a_bst.h"
struct BstPaaPositionWords {
    be<u32> mWord[3];
};

struct BstPaaPlay {
    u8 mPrefix[0x5AB0];
    gptr<dPa_control_c> mpParticle;
    u8 mBeforePlayer[0x78];
    gptr<fopAc_ac_c> mpPlayer;
};
struct BstPaaAnimations {
    be<u16> mAnimation[3];
};
struct BstPaaHands {
    gptr<bst_class> mHand[2];
};
WWHD_OFFSET(BstPaaPlay, mpPlayer, 0x5B2C);
BstPaaPlay *BstPaa_getPlay();
void BstPaa_anm_init(bst_class *, s32, f32, u32, f32, s32);
s16 BstPaa_searchActorAngleY(fopAc_ac_c *, fopAc_ac_c *);
void BstPaa_set_hand_AT(bst_class *, u8);
void BstPaa_pos_move(bst_class *, s32);
enum { JA_SE_CM_BST_HAND_CLAP = 0x6994 };
