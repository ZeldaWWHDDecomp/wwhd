/* Poe: GameCube field names with verified HD offsets. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_cc_d.h"

// HD: embedded damage helpers keep their verified constructor subobjects.
struct PwEnemyIce {
    be<u32> mpActor;
    be<s16> mTimer;
    be<u8> mState;
    u8 _7;
    be<f32> mRadius;
    u8 _C[2];
    be<s16> mFreezeTimer;
    u8 _10[0x20];
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    be<f32> mScale;
    be<f32> mHeight;
    u8 _1A4[8];
    be<f32> mAlpha;
    u8 _1B0[4];
    dBgS_AcchCir mAcchCir;
    dBgS_ObjAcch mAcch;
};
WWHD_SIZE(PwEnemyIce, 0x3B8);
WWHD_OFFSET(PwEnemyIce, mStts, 0x30);
WWHD_OFFSET(PwEnemyIce, mCyl, 0x6C);
WWHD_OFFSET(PwEnemyIce, mAcchCir, 0x1B4);
WWHD_OFFSET(PwEnemyIce, mAcch, 0x1F4);
struct PwEnemyFire {
    be<u32> mpActor;
    be<s16> mTimer;
    u8 _6[6];
    be<u32> mpMorf;
    be<u8> mColors[10];
    u8 _1A[2];
    be<f32> mSizes[10];
    u8 _44[0x48];
    u8 mInlineObject[0x14];
    dCcD_Stts mStts;
    dCcD_Sph mSph;
    u8 _208[0x20];
    be<f32> mScale;
};
WWHD_SIZE(PwEnemyFire, 0x22C);
WWHD_OFFSET(PwEnemyFire, mStts, 0xA0);
WWHD_OFFSET(PwEnemyFire, mSph, 0xDC);
struct PwFollowCallback {
    be<u32> __vtbl;
    be<u32> mpEmitter;
    u8 _8[0xC];
};
WWHD_SIZE(PwFollowCallback, 0x14);
// HD: enemy base and lighting grow; enemy fire appends one float.
struct pw_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u32> mpMorf;
    /* 0x3D4 */ u8 _3D4[4];
    /* 0x3D8 */ be<u32> m2BC;
    /* 0x3DC */ be<u32> m2C0;
    /* 0x3E0 */ be<u32> m2C4;
    /* 0x3E4 */ be<u32> m2C8;
    /* 0x3E8 */ cXyz m2CC;
    /* 0x3F4 */ cXyz m2D8;
    /* 0x400 */ cXyz m2E4;
    /* 0x40C */ cXyz m2F0;
    /* 0x418 */ cXyz m2FC;
    /* 0x424 */ cXyz m308;
    /* 0x430 */ cXyz m314;
    /* 0x43C */ cXyz m320;
    /* 0x448 */ csXyz m32C[2];
    /* 0x454 */ be<u8> mBehaviorType;
    /* 0x455 */ be<u8> mHoversAtInitialYPos;
    /* 0x456 */ be<u8> mNoticeRangeParam;
    /* 0x457 */ be<u8> mColorIndex;
    /* 0x458 */ be<u8> mHitByWhat;
    /* 0x459 */ be<u8> _459;
    /* 0x45A */ be<u8> m33E;
    /* 0x45B */ be<u8> m33F;
    /* 0x45C */ be<u8> m340;
    /* 0x45D */ be<u8> m341;
    /* 0x45E */ be<u8> m342;
    /* 0x45F */ be<u8> m343;
    /* 0x460 */ be<u8> m344;
    /* 0x461 */ be<u8> m345;
    /* 0x462 */ be<s16> m346;
    /* 0x464 */ be<u32> mpPath;
    /* 0x468 */ u8 _468[0xC];
    /* 0x474 */ be<s8> mPathPntIdx;
    /* 0x475 */ be<u8> mPathIndex;
    /* 0x476 */ u8 _476[0xC];
    /* 0x482 */ be<s16> mAction;
    /* 0x484 */ be<s16> mMode;
    /* 0x486 */ u8 _486[2];
    /* 0x488 */ be<u32> mJalhallaID;
    /* 0x48C */ be<u32> mKanteraID;
    /* 0x490 */ be<s32> mBckIdx;
    /* 0x494 */ be<s16> m378;
    /* 0x496 */ be<s16> m37A;
    /* 0x498 */ be<s16> m37C;
    /* 0x49A */ be<s16> m37E;
    /* 0x49C */ be<s16> m380;
    /* 0x49E */ be<s16> m382;
    /* 0x4A0 */ be<s16> m384;
    /* 0x4A2 */ be<s16> m386;
    /* 0x4A4 */ be<s16> m388;
    /* 0x4A6 */ be<s16> m38A;
    /* 0x4A8 */ be<s16> m38C;
    /* 0x4AA */ be<s16> m38E;
    /* 0x4AC */ be<s16> m390;
    /* 0x4AE */ be<s16> m392;
    /* 0x4B0 */ be<s16> mFloatPhase;
    /* 0x4B2 */ be<s16> mFloatPhaseStep;
    /* 0x4B4 */ be<s16> m398;
    /* 0x4B6 */ be<s16> m39A;
    /* 0x4B8 */ be<s16> m39C;
    /* 0x4BA */ be<s16> m39E;
    /* 0x4BC */ be<f32> mCorrectionOffsetY;
    /* 0x4C0 */ be<f32> m3A4;
    /* 0x4C4 */ be<f32> mNoticeRange;
    /* 0x4C8 */ be<f32> m3AC;
    /* 0x4CC */ be<f32> m3B0;
    /* 0x4D0 */ u8 _4D0[4];
    /* 0x4D4 */ be<f32> mLightRadius;
    /* 0x4D8 */ dBgS_AcchCir mAcchCir;
    /* 0x518 */ dBgS_ObjAcch mAcch;
    // HD: retain the four-byte gap after the shared 0x1C4 Acch layout.
    u8 _6DC[4];
    /* 0x6E0 */ PwFollowCallback m5C4;
    /* 0x6F4 */ actor_place mActorPlace;
    /* 0x708 */ dCcD_Stts mStts;
    /* 0x744 */ dCcD_Cyl mCyl;
    /* 0x874 */ dCcD_Sph mSph;
    /* 0x9A0 */ PwEnemyIce mEnemyIce;
    /* 0xD58 */ PwEnemyFire mEnemyFire;
    /* 0xF84 */ u8 mInvisibleModel[8];
};
WWHD_SIZE(pw_class, 0xF8C);
WWHD_OFFSET(pw_class, mPhase, 0x3C8);
WWHD_OFFSET(pw_class, mpMorf, 0x3D0);
WWHD_OFFSET(pw_class, _3D4, 0x3D4);
WWHD_OFFSET(pw_class, m2BC, 0x3D8);
WWHD_OFFSET(pw_class, m2C0, 0x3DC);
WWHD_OFFSET(pw_class, m2C4, 0x3E0);
WWHD_OFFSET(pw_class, m2C8, 0x3E4);
WWHD_OFFSET(pw_class, m2CC, 0x3E8);
WWHD_OFFSET(pw_class, m2D8, 0x3F4);
WWHD_OFFSET(pw_class, m2E4, 0x400);
WWHD_OFFSET(pw_class, m2F0, 0x40C);
WWHD_OFFSET(pw_class, m2FC, 0x418);
WWHD_OFFSET(pw_class, m308, 0x424);
WWHD_OFFSET(pw_class, m314, 0x430);
WWHD_OFFSET(pw_class, m320, 0x43C);
WWHD_OFFSET(pw_class, m32C, 0x448);
WWHD_OFFSET(pw_class, mBehaviorType, 0x454);
WWHD_OFFSET(pw_class, mHoversAtInitialYPos, 0x455);
WWHD_OFFSET(pw_class, mNoticeRangeParam, 0x456);
WWHD_OFFSET(pw_class, mColorIndex, 0x457);
WWHD_OFFSET(pw_class, mHitByWhat, 0x458);
WWHD_OFFSET(pw_class, _459, 0x459);
WWHD_OFFSET(pw_class, m33E, 0x45A);
WWHD_OFFSET(pw_class, m33F, 0x45B);
WWHD_OFFSET(pw_class, m340, 0x45C);
WWHD_OFFSET(pw_class, m341, 0x45D);
WWHD_OFFSET(pw_class, m342, 0x45E);
WWHD_OFFSET(pw_class, m343, 0x45F);
WWHD_OFFSET(pw_class, m344, 0x460);
WWHD_OFFSET(pw_class, m345, 0x461);
WWHD_OFFSET(pw_class, m346, 0x462);
WWHD_OFFSET(pw_class, mpPath, 0x464);
WWHD_OFFSET(pw_class, _468, 0x468);
WWHD_OFFSET(pw_class, mPathPntIdx, 0x474);
WWHD_OFFSET(pw_class, mPathIndex, 0x475);
WWHD_OFFSET(pw_class, _476, 0x476);
WWHD_OFFSET(pw_class, mAction, 0x482);
WWHD_OFFSET(pw_class, mMode, 0x484);
WWHD_OFFSET(pw_class, _486, 0x486);
WWHD_OFFSET(pw_class, mJalhallaID, 0x488);
WWHD_OFFSET(pw_class, mKanteraID, 0x48C);
WWHD_OFFSET(pw_class, mBckIdx, 0x490);
WWHD_OFFSET(pw_class, m378, 0x494);
WWHD_OFFSET(pw_class, m37A, 0x496);
WWHD_OFFSET(pw_class, m37C, 0x498);
WWHD_OFFSET(pw_class, m37E, 0x49A);
WWHD_OFFSET(pw_class, m380, 0x49C);
WWHD_OFFSET(pw_class, m382, 0x49E);
WWHD_OFFSET(pw_class, m384, 0x4A0);
WWHD_OFFSET(pw_class, m386, 0x4A2);
WWHD_OFFSET(pw_class, m388, 0x4A4);
WWHD_OFFSET(pw_class, m38A, 0x4A6);
WWHD_OFFSET(pw_class, m38C, 0x4A8);
WWHD_OFFSET(pw_class, m38E, 0x4AA);
WWHD_OFFSET(pw_class, m390, 0x4AC);
WWHD_OFFSET(pw_class, m392, 0x4AE);
WWHD_OFFSET(pw_class, mFloatPhase, 0x4B0);
WWHD_OFFSET(pw_class, mFloatPhaseStep, 0x4B2);
WWHD_OFFSET(pw_class, m398, 0x4B4);
WWHD_OFFSET(pw_class, m39A, 0x4B6);
WWHD_OFFSET(pw_class, m39C, 0x4B8);
WWHD_OFFSET(pw_class, m39E, 0x4BA);
WWHD_OFFSET(pw_class, mCorrectionOffsetY, 0x4BC);
WWHD_OFFSET(pw_class, m3A4, 0x4C0);
WWHD_OFFSET(pw_class, mNoticeRange, 0x4C4);
WWHD_OFFSET(pw_class, m3AC, 0x4C8);
WWHD_OFFSET(pw_class, m3B0, 0x4CC);
WWHD_OFFSET(pw_class, _4D0, 0x4D0);
WWHD_OFFSET(pw_class, mLightRadius, 0x4D4);
WWHD_OFFSET(pw_class, mAcchCir, 0x4D8);
WWHD_OFFSET(pw_class, mAcch, 0x518);
WWHD_OFFSET(pw_class, m5C4, 0x6E0);
WWHD_OFFSET(pw_class, mActorPlace, 0x6F4);
WWHD_OFFSET(pw_class, mStts, 0x708);
WWHD_OFFSET(pw_class, mCyl, 0x744);
WWHD_OFFSET(pw_class, mSph, 0x874);
WWHD_OFFSET(pw_class, mEnemyIce, 0x9A0);
WWHD_OFFSET(pw_class, mEnemyFire, 0xD58);
WWHD_OFFSET(pw_class, mInvisibleModel, 0xF84);

// HD guest ABI bindings; calls retain their original register and reload order.
s32 Pw_cBgS_ChkPolySafe(u32, u32);
s32 Pw_cBgS_LineCross(u32, u32);
void Pw_cBgS_LinChk_dt(u32, u32);
void Pw_cBgS_LinChk_ct(u32);
void Pw_cCcD_Stts_ct(u32);
void Pw_cCcS_Set(u32, u32);
u32 Pw_cDT_NamePTbl_GetIndex(u32, u32, u32);
void Pw_cLib_addCalc2(u32, f32, f32, f32);
void Pw_cLib_addCalc0(u32, f32, f32);
void Pw_cLib_addCalcAngleS2(u32, u32, u32, u32);
void Pw_MtxPosition(u32, u32);
void Pw_dBgS_AcchCir_dt(u32, u32);
void Pw_cM3dGCyl_SetC(u32, u32);
void Pw_cM3dGCyl_SetH(u32, f32);
void Pw_cM3dGCyl_SetR(u32, f32);
void Pw_cM3dGCyl_ct(u32);
void Pw_cM3dGSph_SetR(u32, f32);
void Pw_cM3dGSph_SetC(u32, u32);
s32 Pw_cM_atan2s(f32, f32);
f32 Pw_cM_rndF(f32);
f32 Pw_cM_rndFX(f32);
void Pw_cXyz__pl(u32, u32, u32);
s32 Pw_enemy_ice(u32);
void Pw_enemy_fire(u32);
void Pw_enemy_fire_remove(u32);
void Pw_enemy_piyo_set(u32);
void Pw_draw_SUB(u32);
void Pw_GXColor_to_float(u32, u32);
void Pw_anm_init(u32, u32, f32, u32, f32, u32);
void Pw_kantera_break(u32);
s32 Pw_Big_pow_gattai_check(u32);
s32 Pw_Big_pow_down_check(u32);
void Pw_BG_check(u32);
s32 Pw_Line_check(u32, u32, u32);
void Pw_alpha_anime(u32);
void Pw_fuwafuwa_calc(u32);
s32 Pw_kougen_hani_check(u32, u32);
void Pw_kyori_sub(u32);
s32 Pw_hani_check(u32);
void Pw_next_dousa_check(u32);
void Pw_move_sound(u32);
void Pw_first_mode_change(u32);
void Pw_enemy_fire_ct(u32);
void Pw_pw_class_ct(u32);
void Pw_body_atari_check(u32);
void Pw_kantera_atari_check(u32);
void Pw_kantera_calc(u32);
void Pw_action_demo(u32);
void Pw_action_torituku(u32);
void Pw_action_big_demo(u32);
s32 Pw_dBgS_GetMtrlSndId(u32, u32);
s32 Pw_dBgS_GetGroundCode(u32, u32);
void Pw_dBgS_Acch_dt(u32, u32);
void Pw_dBgS_AcchCir_dBgS_AcchCir(u32);
void Pw_dBgS_AcchCir_SetWall(u32, f32, f32);
void Pw_dBgS_Acch_dBgS_Acch(u32);
void Pw_dBgS_Acch_Set(u32, u32, u32, u32, u32, u32, u32, u32, u32);
void Pw_dBgS_Acch_CrrPos(u32, u32);
void Pw_dBgS_LinChk_Set(u32, u32, u32, u32);
void Pw_dCamera_c_Stop(u32);
void Pw_dCamera_c_Start(u32);
void Pw_dCamera_c_SetTrimSize(u32, u32);
void Pw_dCcD_Stts_dt(u32, u32);
void Pw_dCcD_GObjInf_dt(u32, u32);
void Pw_dCcD_Sph_dt(u32, u32);
u32 Pw_dCcD_GAtTgCoCommonBase_GetAc(u32);
void Pw_dCcD_GStts_dCcD_GStts(u32);
void Pw_dCcD_GStts_Move(u32);
void Pw_dCcD_Stts_Init(u32, u32, u32, u32);
void Pw_dCcD_GObjInf_dCcD_GObjInf(u32);
s32 Pw_dCcD_GObjInf_ChkAtHit(u32);
void Pw_dCcD_GObjInf_ClrTgHit(u32);
s32 Pw_dCcD_GObjInf_ChkTgHit(u32);
u32 Pw_dCcD_GObjInf_GetTgHitObj(u32);
void Pw_dCcD_Cyl_Set(u32, u32);
void Pw_dCcD_Sph_ct(u32);
void Pw_dCcD_Sph_Set(u32, u32);
void Pw_cc_at_check(u32, u32);
u32 Pw_dComIfGp_get();
s32 Pw_dComIfG_resLoad(u32, u32);
void Pw_dComIfG_resDelete(u32, u32);
s32 Pw_dComIfGp_getReverb(u32);
s32 Pw_dDetect_c_chk_light(u32, u32);
u32 Pw_dKy_getEnvlight();
void Pw_dScnKy_env_light_c_settingTevStruct(u32, u32, u32, u32);
void Pw_dScnKy_env_light_setLightTevColorType_HD(u32, u32, u32);
void Pw_dKyr_get_vectle_calc(u32, u32, u32);
void Pw_dMat_ice_c_entryDL(u32, u32, u32);
void Pw_dPa_followEcallBack_dPa_followEcallBack(u32, u32, u32);
void Pw_dPa_followEcallBack_end(u32);
u32 Pw_dPa_control_c_set(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
u32 Pw_dPath_GetRoomPath(u32, u32);
void Pw_dSv_info_c_onActor(u32, u32, u32);
void Pw_dSnap_RegistFig(u32, u32, f32, f32, f32);
void Pw_dVibration_c_StopQuake(u32, u32);
void Pw_fopAc_ac_c_fopAc_ac_c(u32);
void Pw_fopAc_ac_c_fopAc_ac_c(u32, u32);
u32 Pw_fopAcIt_Judge(u32, u32);
s32 Pw_fopAcM_SearchByID(u32, u32);
void Pw_fopAcM_delete(u32);
u32 Pw_fopAcM_create(u32, u32, u32, u32, u32, u32, u32, u32);
s32 Pw_fopAcM_entrySolidHeap(u32, u32, u32);
void Pw_fopAcM_setCullSizeBox(u32, f32, f32, f32, f32, f32, f32);
void Pw_fopAcM_posMove(u32, u32);
s32 Pw_fopAcM_searchActorAngleY(u32, u32);
f32 Pw_fopAcM_searchActorDistance(u32, u32);
void Pw_fopAcM_orderPotentialEvent(u32, u32, u32, u32);
void Pw_fopAcM_createDisappear(u32, u32, u32, u32, u32);
void Pw_fopAcM_setGbaName(u32, u32, u32, u32);
void Pw_cc_seStart(u32, u32, u32, u32);
void Pw_fopAcM_monsSeStart_HD(u32, u32, u32, u32, u32);
void Pw_mDoExt_McaMorf_setAnm(u32, u32, u32, f32, f32, f32, f32, u32);
u32 Pw_mDoExt_McaMorf_mDoExt_McaMorf(u32, u32, u32, u32, u32, u32, f32, u32, u32, u32, u32, u32, u32);
void Pw_mDoExt_McaMorf_play(u32, u32, u32, u32);
void Pw_mDoExt_McaMorf_entryDL(u32);
void Pw_mDoExt_McaMorf_calc(u32);
void Pw_mDoExt_McaMorf_stopZelAnime(u32);
void Pw_mDoExt_baseAnm_play(u32);
u32 Pw_mDoExt_btpAnm_ct(u32);
s32 Pw_mDoExt_btpAnm_init(u32, u32, u32, u32, u32, f32, u32, u32, u32, u32);
void Pw_mDoExt_btpAnm_entry(u32, u32, u32);
u32 Pw_mDoExt_brkAnm_ct(u32);
s32 Pw_mDoExt_brkAnm_init(u32, u32, u32, u32, u32, f32, u32, u32, u32, u32);
void Pw_mDoExt_brkAnm_entry(u32, u32, f32);
void Pw_mDoExt_invisibleModel_ct(u32);
void Pw_mDoExt_invisibleModel_dt(u32, u32);
s32 Pw_mDoExt_invisibleModel_create(u32, u32);
void Pw_mDoMtx_YrotS(u32, u32);
void Pw_mDoMtx_XrotM(u32, u32);
void Pw_mDoMtx_YrotM(u32, u32);
void Pw_mDoMtx_ZrotM(u32, u32);
u32 Pw_dRes_control_c_getRes(u32, u32, u32);
u32 Pw_operator_new(u32);
void Pw_dl(u32);
void Pw_GXColor4f_scaleRGB(u32, u32, f32);
s32 Pw_J3DFrameCtrl_checkPass(u32, f32);
u32 Pw_J3DMaterial_getShapePacketModelData_HD(u32);
u32 Pw_J3DModelData_getJointTree_HD(u32);
void Pw_J3DModel_setModelXlu_HD(u32, u32);
u32 Pw_J3DJoint_getKind_HD(u32);
u32 Pw_J3DMaterial_getColorReg_HD(u32, u32);
void Pw_JPASetRMtxTVecfromMtx(u32, u32, u32);
void Pw_PSVECAdd(u32, u32, u32);
void Pw_PSMTXCopy(u32, u32);
void Pw_PSMTXTrans(u32, f32, f32, f32);
void Pw_GHS_header_ct(u32);
void Pw_GHS_section_ct(u32);
void Pw_register_global_object(u32);
f32 Pw_sqrtf_HD(f32);

// HD: partial view; only offsets read by this unit are exposed.
struct PwPlayView {
    u8 _0[0x52B8];
    /* 0x52B8 */ be<u16> mCollisionFlags;
    u8 _52BA[0x7F6];
    /* 0x5AB0 */ be<u32> mpParticle;
    u8 _5AB4[0x44];
    /* 0x5AF8 */ be<u32> mpCamera;
    u8 _5AFC[0x30];
    /* 0x5B2C */ be<u32> mpPlayer;
    /* 0x5B30 */ be<s8> mCameraId;
    u8 _5B31[0x3];
    /* 0x5B34 */ be<u32> mpControlPlayer;
    u8 _5B38[0x1A4];
    /* 0x5CDC */ be<u32> mButtonHold;
    u8 _5CE0[0x98];
    /* 0x5D78 */ be<u32> mpOpa;
    /* 0x5D7C */ be<u32> mpXlu;
    u8 _5D80[0x4];
    /* 0x5D84 */ be<u32> mpMaskOffOpa;
    /* 0x5D88 */ be<u32> mpMaskOffXlu;
};
WWHD_OFFSET(PwPlayView, mCollisionFlags, 0x52B8);
WWHD_OFFSET(PwPlayView, mpParticle, 0x5AB0);
WWHD_OFFSET(PwPlayView, mpCamera, 0x5AF8);
WWHD_OFFSET(PwPlayView, mpPlayer, 0x5B2C);
WWHD_OFFSET(PwPlayView, mCameraId, 0x5B30);
WWHD_OFFSET(PwPlayView, mpControlPlayer, 0x5B34);
WWHD_OFFSET(PwPlayView, mButtonHold, 0x5CDC);
WWHD_OFFSET(PwPlayView, mpOpa, 0x5D78);
WWHD_OFFSET(PwPlayView, mpXlu, 0x5D7C);
WWHD_OFFSET(PwPlayView, mpMaskOffOpa, 0x5D84);
WWHD_OFFSET(PwPlayView, mpMaskOffXlu, 0x5D88);

// HD: partial view; only offsets read by this unit are exposed.
struct PwMaterialView {
    u8 _0[0x18];
    /* 0x18 */ be<u32> mpColorBlock;
    u8 _1C[0x84];
    /* 0xA0 */ be<u32> mChanged;
};
WWHD_OFFSET(PwMaterialView, mpColorBlock, 0x18);
WWHD_OFFSET(PwMaterialView, mChanged, 0xA0);

// HD: partial view; only offsets read by this unit are exposed.

// HD: partial view; only offsets read by this unit are exposed.
struct PwJointTreeView {
    u8 _0[0x8];
    /* 0x8 */ be<u16> mJointNum;
    u8 _A[0x1A];
    /* 0x24 */ be<u16> mMaterialNum;
};
WWHD_OFFSET(PwJointTreeView, mJointNum, 0x8);
WWHD_OFFSET(PwJointTreeView, mMaterialNum, 0x24);

// HD: partial view; only offsets read by this unit are exposed.
struct PwColorBlockView {
    u8 _0[0x4];
    /* 0x4 */ be<u32> __vtbl;
};
WWHD_OFFSET(PwColorBlockView, __vtbl, 0x4);

// HD: partial view; only offsets read by this unit are exposed.

// HD: partial view; only offsets read by this unit are exposed.
struct PwFloatColor {
    /* 0x0 */ be<f32> r;
    /* 0x4 */ be<f32> g;
    /* 0x8 */ be<f32> b;
    /* 0xC */ be<f32> a;
};
WWHD_OFFSET(PwFloatColor, r, 0x0);
WWHD_OFFSET(PwFloatColor, g, 0x4);
WWHD_OFFSET(PwFloatColor, b, 0x8);
WWHD_OFFSET(PwFloatColor, a, 0xC);

// HD: partial view; only offsets read by this unit are exposed.
struct PwDebugRegisters {
    u8 _0[0xC];
    /* 0xC */ be<f32> mApproachSpeed;
    /* 0x10 */ be<f32> mJumpSpeed;
    /* 0x14 */ be<f32> mJumpRandomSpeed;
    /* 0x18 */ be<f32> mGravity;
    /* 0x1C */ be<f32> mRetreatSpeed;
    /* 0x20 */ be<f32> mRetreatRandomSpeed;
    /* 0x24 */ be<f32> mMergeSpeed;
    /* 0x28 */ be<f32> mMergeHeight;
    /* 0x2C */ be<f32> mMergeDistance;
    u8 _30[0x5E];
    /* 0x8E */ be<s16> mStunTime;
    u8 _90[0x430];
    /* 0x4C0 */ be<f32> mBodyRadius;
    u8 _4C4[0x6C];
    /* 0x530 */ be<f32> mDemoDistance;
    /* 0x534 */ be<f32> mDemoApproachSpeed;
    /* 0x538 */ be<f32> mDemoTurnLimit;
    /* 0x53C */ be<f32> mDemoTurnStep;
    /* 0x540 */ be<f32> mDemoSpeed;
    /* 0x544 */ be<f32> mDemoTurnSpeed;
    /* 0x548 */ be<f32> mDemoFastTurnSpeed;
    /* 0x54C */ be<f32> mDemoRandomTimer;
};
WWHD_OFFSET(PwDebugRegisters, mApproachSpeed, 0xC);
WWHD_OFFSET(PwDebugRegisters, mJumpSpeed, 0x10);
WWHD_OFFSET(PwDebugRegisters, mJumpRandomSpeed, 0x14);
WWHD_OFFSET(PwDebugRegisters, mGravity, 0x18);
WWHD_OFFSET(PwDebugRegisters, mRetreatSpeed, 0x1C);
WWHD_OFFSET(PwDebugRegisters, mRetreatRandomSpeed, 0x20);
WWHD_OFFSET(PwDebugRegisters, mMergeSpeed, 0x24);
WWHD_OFFSET(PwDebugRegisters, mMergeHeight, 0x28);
WWHD_OFFSET(PwDebugRegisters, mMergeDistance, 0x2C);
WWHD_OFFSET(PwDebugRegisters, mStunTime, 0x8E);
WWHD_OFFSET(PwDebugRegisters, mBodyRadius, 0x4C0);
WWHD_OFFSET(PwDebugRegisters, mDemoDistance, 0x530);
WWHD_OFFSET(PwDebugRegisters, mDemoApproachSpeed, 0x534);
WWHD_OFFSET(PwDebugRegisters, mDemoTurnLimit, 0x538);
WWHD_OFFSET(PwDebugRegisters, mDemoTurnStep, 0x53C);
WWHD_OFFSET(PwDebugRegisters, mDemoSpeed, 0x540);
WWHD_OFFSET(PwDebugRegisters, mDemoTurnSpeed, 0x544);
WWHD_OFFSET(PwDebugRegisters, mDemoFastTurnSpeed, 0x548);
WWHD_OFFSET(PwDebugRegisters, mDemoRandomTimer, 0x54C);

// HD: partial view; only offsets read by this unit are exposed.
struct PwJalhallaView {
    u8 _0[0x505];
    /* 0x505 */ be<u8> mMergeState;
    /* 0x506 */ be<u8> mAction;
    u8 _507;
    u8 _508[0xA];
    /* 0x512 */ be<s16> mMode;
    u8 _514[0x4E];
    /* 0x562 */ be<s16> mKnockdown;
    u8 _564[0x6];
    /* 0x56A */ be<s16> mMergeAllowed;
    u8 _56C[0x12];
    /* 0x57E */ be<s16> mPoeCount;
    /* 0x580 */ be<s16> mPoeMerged;
};
WWHD_OFFSET(PwJalhallaView, mMergeState, 0x505);
WWHD_OFFSET(PwJalhallaView, mAction, 0x506);
WWHD_OFFSET(PwJalhallaView, mMode, 0x512);
WWHD_OFFSET(PwJalhallaView, mKnockdown, 0x562);
WWHD_OFFSET(PwJalhallaView, mMergeAllowed, 0x56A);
WWHD_OFFSET(PwJalhallaView, mPoeCount, 0x57E);
WWHD_OFFSET(PwJalhallaView, mPoeMerged, 0x580);

// HD: partial view; only offsets read by this unit are exposed.
struct PwPathView {
    /* 0x0 */ be<u16> mNum;
    u8 _2[0x6];
    /* 0x8 */ be<u32> mpPoints;
};
WWHD_OFFSET(PwPathView, mNum, 0x0);
WWHD_OFFSET(PwPathView, mpPoints, 0x8);

// HD: partial view; only offsets read by this unit are exposed.
struct PwPathPoint {
    u8 _0[0x4];
    /* 0x4 */ be<f32> x;
    u8 _8[0x4];
    /* 0xC */ be<f32> z;
};
WWHD_OFFSET(PwPathPoint, x, 0x4);
WWHD_OFFSET(PwPathPoint, z, 0xC);

struct PwJointNumber {
    u8 _0[4];
    be<u16> mJointNo;
};
WWHD_OFFSET(PwJointNumber, mJointNo, 4);

inline GXColor* pwViewGXColor(u32 address) { return gabi::at<GXColor>(address); }

inline J3DFrameCtrl* pwViewJ3DFrameCtrl(u32 address) { return gabi::at<J3DFrameCtrl>(address); }

inline J3DModel* pwViewJ3DModel(u32 address) { return gabi::at<J3DModel>(address); }

inline J3DModelData* pwViewJ3DModelData(u32 address) { return gabi::at<J3DModelData>(address); }

inline J3DMtxBuffer_hd* pwViewJ3DMtxBuffer_hd(u32 address) { return gabi::at<J3DMtxBuffer_hd>(address); }

inline PwDebugRegisters* pwViewPwDebugRegisters(u32 address) { return gabi::at<PwDebugRegisters>(address); }

inline PwFloatColor* pwViewPwFloatColor(u32 address) { return gabi::at<PwFloatColor>(address); }

inline PwJalhallaView* pwViewPwJalhallaView(u32 address) { return gabi::at<PwJalhallaView>(address); }

inline PwJointNumber* pwViewPwJointNumber(u32 address) { return gabi::at<PwJointNumber>(address); }

inline PwJointTreeView* pwViewPwJointTreeView(u32 address) { return gabi::at<PwJointTreeView>(address); }

inline PwMaterialView* pwViewPwMaterialView(u32 address) { return gabi::at<PwMaterialView>(address); }

inline PwPathPoint* pwViewPwPathPoint(u32 address) { return gabi::at<PwPathPoint>(address); }

inline PwPathView* pwViewPwPathView(u32 address) { return gabi::at<PwPathView>(address); }

inline PwPlayView* pwViewPwPlayView(u32 address) { return gabi::at<PwPlayView>(address); }

inline cXyz* pwViewcXyz(u32 address) { return gabi::at<cXyz>(address); }

inline fopAc_ac_c* pwViewfopAc_ac_c(u32 address) { return gabi::at<fopAc_ac_c>(address); }

inline mDoExt_McaMorf* pwViewmDoExt_McaMorf(u32 address) { return gabi::at<mDoExt_McaMorf>(address); }

struct PwShapeAttribute {
    u8 _0[0x18];
    be<u32> mBaseVtable;
    be<u32> mVtable;
};
WWHD_SIZE(PwShapeAttribute, 0x20);
struct PwAttackInfo {
    be<u32> mpCollider;
    u8 _4[0x10];
    be<u32> mSound;
    u8 _18[4];
};
WWHD_SIZE(PwAttackInfo, 0x1C);
struct PwPlayerActionView {
    u8 _0[0x3AC];
    be<u8> mCutType;
};
inline be<s16>& pwDamageTimer(pw_class* actor, u32 index) {
    return *gabi::at<be<s16>>(gabi::ea(actor) + offsetof(pw_class, m384) + index * sizeof(s16));
}
inline be<s16>& pwTimer(pw_class* actor, u32 index) {
    return *gabi::at<be<s16>>(gabi::ea(actor) + offsetof(pw_class, m378) + index * sizeof(s16));
}
inline be<u32>& pwRegisterAnimation(pw_class* actor, u32 index) {
    return *gabi::at<be<u32>>(gabi::ea(actor) + offsetof(pw_class, m2C0) + index * sizeof(u32));
}

struct PwLanternTail {
    u8 _0[0x3D4];
    csXyz mJointRot[2];
    csXyz mJointBaseRot;
    csXyz m2CA;
    csXyz mJointBaseRotTarget;
    u8 _3F2[2];
    cXyz mBonPos;
    u8 _400[0x84];
    be<u8> mState;
};
WWHD_OFFSET(PwLanternTail, mState, 0x484);

inline PwEnemyFire* pwViewPwEnemyFire(u32 address) { return gabi::at<PwEnemyFire>(address); }

inline PwLanternTail* pwViewPwLanternTail(u32 address) { return gabi::at<PwLanternTail>(address); }

inline PwPlayerActionView* pwViewPwPlayerActionView(u32 address) {
    return gabi::at<PwPlayerActionView>(address);
}

inline PwShapeAttribute* pwViewPwShapeAttribute(u32 address) { return gabi::at<PwShapeAttribute>(address); }

inline daPy_py_c* pwViewdaPy_py_c(u32 address) { return gabi::at<daPy_py_c>(address); }

inline be<f32>& pwMatrixElement(u32 matrix, u32 index) {
    return *gabi::at<be<f32>>(matrix + index * sizeof(f32));
}
inline be<u32>& pwVectorWord(const cXyz* vector, u32 index) {
    return *gabi::at<be<u32>>(gabi::ea(vector) + index * sizeof(u32));
}
inline be<u16>& pwAngleWord(const csXyz* angles, u32 index) {
    return *gabi::at<be<u16>>(gabi::ea(angles) + index * sizeof(u16));
}
struct PwMaterialAnimationSlots {
    u8 _0[0x38];
    be<u32> mpBtp;
    u8 _3C[0xC];
    be<u32> mpBrk;
};
struct PwColorBlockVtable {
    u8 _0[0x3C];
    be<u32> mSetKonst;
    u8 _40[0xC];
    be<u32> mGetKonst;
};
struct PwFollowVtable {
    u8 _0[0x44];
    be<u32> mEnd;
};
inline u32 pwGetKonstColor(u32 block, s32 index) {
    u32 table = gabi::at<PwColorBlockView>(block)->__vtbl;
    return gabi::call_ptr<u32>(gabi::at<PwColorBlockVtable>(table)->mGetKonst, block, index);
}
inline void pwSetKonstColor(u32 block, s32 index, u32 color) {
    u32 table = gabi::at<PwColorBlockView>(block)->__vtbl;
    gabi::call_ptr(gabi::at<PwColorBlockVtable>(table)->mSetKonst, block, index, color);
}
inline void pwEndFollow(u32 table, u32 callback) {
    gabi::call_ptr<void>(gabi::at<PwFollowVtable>(table)->mEnd, callback);
}

inline J3DJoint* pwViewJ3DJoint(u32 address) { return gabi::at<J3DJoint>(address); }

inline dCcD_GObjInf* pwViewdCcD_GObjInf(u32 address) { return gabi::at<dCcD_GObjInf>(address); }

inline pw_class* pwViewpw_class(u32 address) { return gabi::at<pw_class>(address); }

inline PwMaterialAnimationSlots* pwViewPwMaterialAnimationSlots(u32 address) {
    return gabi::at<PwMaterialAnimationSlots>(address);
}

// Resource indices are unchanged from the GameCube Pw archive.
enum dRes_INDEX_PW {
    /* BAS  */
    dRes_INDEX_PW_BAS_ATTACK1_e = 0x7,
    dRes_INDEX_PW_BAS_NIGERU1_e = 0x8,
    /* BCK  */
    dRes_INDEX_PW_BCK_ANSIN1_e = 0xB,
    dRes_INDEX_PW_BCK_ATTACK1_e = 0xC,
    dRes_INDEX_PW_BCK_BUNRETU1_e = 0xD,
    dRes_INDEX_PW_BCK_DAMAGE1_e = 0xE,
    dRes_INDEX_PW_BCK_DAMAGE_K1_e = 0xF,
    dRes_INDEX_PW_BCK_DERUA1_e = 0x10,
    dRes_INDEX_PW_BCK_DERUB1_e = 0x11,
    dRes_INDEX_PW_BCK_DERUB2_e = 0x12,
    dRes_INDEX_PW_BCK_DOKI1_e = 0x13,
    dRes_INDEX_PW_BCK_IDOU1_e = 0x14,
    dRes_INDEX_PW_BCK_IDOU2_e = 0x15,
    dRes_INDEX_PW_BCK_IKIGIRE1_e = 0x16,
    dRes_INDEX_PW_BCK_JITTAIKA1_e = 0x17,
    dRes_INDEX_PW_BCK_KIDUKU1_e = 0x18,
    dRes_INDEX_PW_BCK_KIDUKU2_e = 0x19,
    dRes_INDEX_PW_BCK_KIZETU1_e = 0x1A,
    dRes_INDEX_PW_BCK_NIGERU1_e = 0x1B,
    dRes_INDEX_PW_BCK_PRESS1_e = 0x1C,
    dRes_INDEX_PW_BCK_SIRIMOTI1_e = 0x1D,
    dRes_INDEX_PW_BCK_SYOUTEN1_e = 0x1E,
    dRes_INDEX_PW_BCK_TORITUKI1_e = 0x1F,
    dRes_INDEX_PW_BCK_TORITUKI2_e = 0x20,
    dRes_INDEX_PW_BCK_TORITUKI3_e = 0x21,
    dRes_INDEX_PW_BCK_TORITUKI_WAIT1_e = 0x22,
    dRes_INDEX_PW_BCK_WAIT1_e = 0x23,
    dRes_INDEX_PW_BCK_WAIT2_e = 0x24,
    /* BDLM */
    dRes_INDEX_PW_BDL_PW_e = 0x27,
    /* BRK  */
    dRes_INDEX_PW_BRK_DEFAULT_e = 0x2A,
    dRes_INDEX_PW_BRK_HIT_e = 0x2B,
    dRes_INDEX_PW_BRK_JITTAIKA1_e = 0x2C,
    /* BTP  */
    dRes_INDEX_PW_BTP_IROGAE1_e = 0x2F,
};

void Pw_invisibleEntry(u32);
void Pw_invisibleEntryMaskOff(u32);

enum PwAction {
    PW_ACTION_DOUSA = 0,
    PW_ACTION_KOUGEKI = 1,
    PW_ACTION_ITAI = 2,
    PW_ACTION_DEMO = 3,
    PW_ACTION_TORITUKU = 4,
    PW_ACTION_BIG_DEMO = 5,
};

// HD: addresses of this translation unit's compiler data and external judge callback.
constexpr u32 PW_ACTOR_VTABLE = 0x1003841C;
constexpr u32 PW_STATUS_VTABLE = 0x1004AE88;
constexpr u32 PW_GAME_STATUS_VTABLE = 0x1004AEC0;
constexpr u32 PW_CYLINDER_ATTR_VTABLE = 0x1003837C;
constexpr u32 PW_GEOMETRY_ATTR_VTABLE = 0x100015A8;
constexpr u32 PW_HIT_INFO_VTABLE = 0x1004B108;
constexpr u32 PW_CYLINDER_SHAPE_VTABLE = 0x1004B160;
constexpr u32 PW_CYLINDER_GEOMETRY_VTABLE = 0x1004B150;
constexpr u32 PW_LINE_CHECK_VTABLE = 0x100383DC;
constexpr u32 PW_LINE_POLYGON_VTABLE = 0x100383EC;
constexpr u32 PW_LINE_POLYGON_BASE_VTABLE = 0x1003838C;
constexpr u32 PW_LINE_GROUP_BASE_VTABLE = 0x1003839C;
constexpr u32 PW_LINE_PASS_VTABLE = 0x1003840C;
constexpr u32 PW_LINE_GROUP_VTABLE = 0x100383FC;
constexpr u32 PW_JUDGE_BY_ID = 0x025E1234;
constexpr u32 PW_HEADER_SECTION = 0x1046D3F0;
constexpr u32 PW_HEADER_OBJECT = 0x1046D3F1;
constexpr u32 PW_HEADER_REGISTRATION = 0x101CF180;
constexpr u32 PW_SECTION_REGISTRATION = 0x101CF18C;
constexpr u32 PW_OBJECT_REGISTRATION = 0x101CF198;

// HD: explicit 32-bit addition retains guest-address wrapping, including nullable member addresses.
inline u32 pwAddress(pw_class* actor, u32 offset) { return gabi::ea(actor) + offset; }
WWHD_OFFSET(pw_class, tevStr, 0x110);
WWHD_OFFSET(pw_class, old.pos, 0x300);
WWHD_OFFSET(pw_class, current.pos, 0x314);
WWHD_OFFSET(pw_class, current.pos.y, 0x318);
WWHD_OFFSET(pw_class, current.pos.z, 0x31C);
WWHD_OFFSET(pw_class, current.angle.y, 0x322);
WWHD_OFFSET(pw_class, shape_angle, 0x328);
WWHD_OFFSET(pw_class, shape_angle.y, 0x32A);
WWHD_OFFSET(pw_class, speed, 0x33C);
WWHD_OFFSET(pw_class, speedF, 0x370);
WWHD_OFFSET(pw_class, eyePos, 0x37C);
WWHD_OFFSET(pw_class, attention_info.position, 0x390);
WWHD_OFFSET(pw_class, mPhase, 0x3C8);
WWHD_OFFSET(pw_class, m2CC, 0x3E8);
WWHD_OFFSET(pw_class, m2D8, 0x3F4);
WWHD_OFFSET(pw_class, m308, 0x424);
WWHD_OFFSET(pw_class, mFloatPhaseStep, 0x4B2);
WWHD_OFFSET(pw_class, m39A, 0x4B6);
WWHD_OFFSET(pw_class, m39C, 0x4B8);
WWHD_OFFSET(pw_class, m39E, 0x4BA);
WWHD_OFFSET(pw_class, m3A4, 0x4C0);
WWHD_OFFSET(pw_class, m3B0, 0x4CC);
WWHD_OFFSET(pw_class, mLightRadius, 0x4D4);
WWHD_OFFSET(pw_class, mAcchCir, 0x4D8);
WWHD_OFFSET(pw_class, mAcchCir.m_cir, 0x4EC);
WWHD_OFFSET(pw_class, mAcch, 0x518);
WWHD_OFFSET(pw_class, mAcch.m_gnd.mPolyIndex, 0x600);
WWHD_OFFSET(pw_class, m5C4, 0x6E0);
WWHD_OFFSET(pw_class, mActorPlace.pos, 0x6F4);
WWHD_OFFSET(pw_class, mActorPlace.angle, 0x700);
WWHD_OFFSET(pw_class, mStts, 0x708);
WWHD_OFFSET(pw_class, mStts.__vtbl_gstts, 0x724);
WWHD_OFFSET(pw_class, mCyl, 0x744);
WWHD_OFFSET(pw_class, mCyl.mGObjAt, 0x794);
WWHD_OFFSET(pw_class, mCyl.mCyl, 0x85C);
WWHD_OFFSET(pw_class, mSph, 0x874);
WWHD_OFFSET(pw_class, mSph.mSph, 0x98C);
WWHD_OFFSET(pw_class, mEnemyIce, 0x9A0);
WWHD_OFFSET(pw_class, mEnemyIce.mStts, 0x9D0);
WWHD_OFFSET(pw_class, mEnemyIce.mStts.__vtbl_gstts, 0x9EC);
WWHD_OFFSET(pw_class, mEnemyIce.mCyl, 0xA0C);
WWHD_OFFSET(pw_class, mEnemyIce.mCyl.mCyl, 0xB24);
WWHD_OFFSET(pw_class, mEnemyIce.mAcchCir, 0xB54);
WWHD_OFFSET(pw_class, mEnemyIce.mAcchCir.m_cir, 0xB68);
WWHD_OFFSET(pw_class, mEnemyIce.mAcch, 0xB94);
WWHD_OFFSET(pw_class, mEnemyFire, 0xD58);
WWHD_OFFSET(pw_class, mEnemyFire.mStts, 0xDF8);
WWHD_OFFSET(pw_class, mEnemyFire.mSph, 0xE34);
WWHD_OFFSET(pw_class, mInvisibleModel, 0xF84);
