#pragma once
#include "d/actor/d_a_bst.h"

struct BstBeamPlay { u8 mPrefix[0x5AB0]; gptr<dPa_control_c> mpParticle; u8 mBeforePlayer[0x78]; gptr<fopAc_ac_c> mpPlayer; };
struct BstBeamChargeEffects { be<u16> mName[2]; };
WWHD_OFFSET(BstBeamPlay, mpPlayer, 0x5B2C);
WWHD_SIZE(BstBeamChargeEffects, 4);
BstBeamPlay* BstBeam_getPlay();
void BstBeam_anm_init(bst_class*, s32, f32, u32, f32, s32);
void JPASetRMtxTVecfromMtx(Mtx34*, Mtx34*, cXyz*);
enum { dRes_INDEX_BST_BCK_BEAM_e = 6, dRes_INDEX_BST_BCK_BEAM_MAE_e = 7,
       dRes_INDEX_BST_BCK_BEAM_TAME_e = 8, BST_JNT_BUKI2_e = 3 };
enum { JA_SE_CM_BST_MOUTH_OPEN = 0x58DE, JA_SE_CM_BST_BEAM_PREPARE = 0x50DF,
       JA_SE_CM_BST_BEAM_FIRE = 0x58E0, JA_SE_CM_BST_MOUTH_CLOSE = 0x58E1 };
