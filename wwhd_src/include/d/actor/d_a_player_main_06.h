/** Link demo and ladder range. HD keeps the shared Link layout; private views describe
 * animation/item blocks without changing declarations used by other ranges. */
#pragma once
#include "d/actor/d_a_player_main.h"
#include "gc_api/d/d_com_inf_game.h"
#include "JSystem/J3DGraph.h"


struct PlayerDemoAnmHeap {
    be<u16> mIdx;
    u8 padding[0xE];
};
WWHD_SIZE(PlayerDemoAnmHeap, 0x10);
struct PlayerDemoOldFrame {
    u8 beforeRate[0xC];
    be<f32> mOldFrameRate;
    u8 beforeTransform[0xC];
    gptr<void> mpOldFrameTransInfo;
};
WWHD_OFFSET(PlayerDemoOldFrame, mOldFrameRate, 0xC);
WWHD_OFFSET(PlayerDemoOldFrame, mpOldFrameTransInfo, 0x1C);
WWHD_SIZE(PlayerDemoOldFrame, 0x20);
struct PlayerDemoTransform {
    u8 beforeTranslation[0x14];
    cXyz mTranslate;
};
WWHD_OFFSET(PlayerDemoTransform, mTranslate, 0x14);
WWHD_SIZE(PlayerDemoTransform, 0x20);
// HD: the animation's maximum frame is obtained virtually and kept in the full register.
struct PlayerDemoAnimation {
    be<f32> mFrame;
    be<u32> mVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(gabi::load<u32>(mVtable + 0x14), this); }
};
WWHD_SIZE(PlayerDemoAnimation, 8);
struct PlayerDemoShip {
    u8 beforeLinkSit[0x636];
    be<u8> mLinkSit;
    u8 beforeFlags[0x644 - 0x637];
    be<u32> mFlags;
};
WWHD_OFFSET(PlayerDemoShip, mLinkSit, 0x636);
WWHD_OFFSET(PlayerDemoShip, mFlags, 0x644);
WWHD_SIZE(PlayerDemoShip, 0x648);
struct PlayerDemoBottle : fopAc_ac_c {
    u8 beforeThrowState[0x787 - sizeof(fopAc_ac_c)];
    be<u8> mThrowState;
};
WWHD_OFFSET(PlayerDemoBottle, mThrowState, 0x787);
WWHD_SIZE(PlayerDemoBottle, 0x788);

// HD: sparse typed views of fields which remain opaque in the shared Link layout.
struct PlayerDemoView {
    u8 padding0[0xE88];
    /* 0x0E88 */ gptr<J3DModel> mpShmsModel;
    /* 0x0E8C */ mDoExt_bckAnm mAtngshaBck;
    u8 padding2[0x352C];
    /* 0x4444 */ mDoExt_bckAnm mSwordAnim;
    u8 padding3[0x4A0];
    /* 0x4970 */ gptr<J3DModel> mpBottleContentsModel;
    u8 padding4[0xEA8];
    /* 0x581C */ gptr<PlayerDemoAnimation> mpLetterAnimation;
    u8 padding5[0x20];
    /* 0x5840 */ be<s32> mBlendCount;
    /* 0x5844 */ gptr<be<f32>> mpBlendWeights;
    /* 0x5848 */ PlayerDemoAnmHeap mAnmHeapUnder[2];
    /* 0x5868 */ PlayerDemoAnmHeap mAnmHeapUpper[3];
    u8 padding9[0xD48];
    /* 0x65E0 */ PlayerDemoAnmHeap mTexScrollHeap;
    u8 padding10[0x1C78];
    /* 0x8268 */ gptr<PlayerDemoBottle> mHDThrowActor;
    /* 0x826C */ be<s16> mHDThrowAngle;
    u8 padding12[0x6];
    /* 0x8274 */ be<f32> mHDThrowHeight;
};
WWHD_OFFSET(PlayerDemoView, mpShmsModel, 0xE88);
WWHD_OFFSET(PlayerDemoView, mAtngshaBck, 0xE8C);
WWHD_OFFSET(PlayerDemoView, mSwordAnim, 0x4444);
WWHD_OFFSET(PlayerDemoView, mpBottleContentsModel, 0x4970);
WWHD_OFFSET(PlayerDemoView, mpLetterAnimation, 0x581C);
WWHD_OFFSET(PlayerDemoView, mBlendCount, 0x5840);
WWHD_OFFSET(PlayerDemoView, mpBlendWeights, 0x5844);
WWHD_OFFSET(PlayerDemoView, mAnmHeapUnder, 0x5848);
WWHD_OFFSET(PlayerDemoView, mAnmHeapUpper, 0x5868);
WWHD_OFFSET(PlayerDemoView, mTexScrollHeap, 0x65E0);
WWHD_OFFSET(PlayerDemoView, mHDThrowActor, 0x8268);
WWHD_OFFSET(PlayerDemoView, mHDThrowAngle, 0x826C);
WWHD_OFFSET(PlayerDemoView, mHDThrowHeight, 0x8274);
WWHD_SIZE(PlayerDemoView, 0x8278);
inline PlayerDemoView& playerDemoView(daPy_lk_c* player) {
    return *gabi::at<PlayerDemoView>(gabi::ea(player));
}
inline PlayerDemoOldFrame* playerDemoOldFrame(u32 address) { return gabi::at<PlayerDemoOldFrame>(address); }

// HD: private bindings retain the original full-register arguments and return widths.
void cLib_addCalcPos(cXyz*, cXyz*, f32, f32, f32);
void playerDemoStartTextInput(u32, s32);
void playerDemoShowItem(fopAc_ac_c*);
void playerDemoClearActorKeep(daPy_actorKeep_l*);
u32 playerDemoGetCameraBody();
void playerDemoForceLockOff(u32, u32);
void playerDemoStartCamera(u32);
void playerDemoSetCamera(u32, cXyz*, cXyz*);
void playerDemoResetCamera(u32, cXyz*, cXyz*);
void playerDemoEndEventCamera(u32, u32);
void dKy_Itemgetcol_chg_on();
s32 dKyw_get_tactwind_dir();
void playerDemoEmptyBottle(u32, u32);
void playerDemoOnTact(u32, u32);
fopAc_ac_c* fopAcM_fastCreate(s32, u32, cXyz*, s32, csXyz*, void*, s32, s32, s32);
fopAc_ac_c* fopAcM_getItemEventPartner(fopAc_ac_c*);
fopAc_ac_c* fopAcM_getEventPartner(fopAc_ac_c*);
void fopAcM_setCarryNow(fopAc_ac_c*, s32);
void mDoAud_subBgmStart(u32);
void playerDemoSystemSound(u32);
void playerDemoInitBck(mDoExt_bckAnm*, u32, u32, s32, s32, f32, s32, s32, s32);
void playerDemoChangeBckOnly(mDoExt_bckAnm*, u32);
f64 playerDemoVectorMagnitudeRegister(cXyz*);
inline u8 playerDemoParamByte(daPy_lk_c* player) { return reinterpret_cast<be<u8>*>(&player->mDemo)[0xB]; }
inline f32 playerDemoFloatSelect(f32 test, f32 positive, f32 negative) {
    return test >= 0.0f ? positive : negative;
}
// HD: integer channel preserves signaling NaN bits through the GHS lfs/stfs copy.
inline void playerDemoCopyFloatBits(u32 destination, u32 source) {
    gmem_stf32(destination, gmem_ld32(source));
}
inline void playerDemoCopyFloatBits(be<f32>* destination, be<f32>* source) {
    playerDemoCopyFloatBits(gabi::ea(destination), gabi::ea(source));
}
f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32);

namespace player_demo {
enum : u32 {
    ANM_BINOPENB = 0xBB,
    ANM_BINOPENPRE = 0xB9,
    ANM_BOXOPENSLINK = 0xD4,
    ANM_COMBO_LINK = 0xDD,
    ANM_GETLETTER = 0xC6,
    ANM_GRABUP = 0x66,
    ANM_GRABWAIT = 0x68,
    ANM_KOSHIKAKE = 0xDC,
    ANM_LADDERDWEDL = 0x89,
    ANM_LADDERDWEDR = 0x88,
    ANM_LAVADAM = 0x61,
    ANM_LINK_FREEZ = 0xC8,
    ANM_LINK_MOGAKI = 0xC9,
    ANM_POWUP = 0xDB,
    ANM_POWUPWAIT = 0xDA,
    ANM_PRESENTATIONAWAIT = 0xD9,
    ANM_RISE = 0xC0,
    ANM_SLIPICE = 0xA9,
    ANM_SURPRISEDB = 0xBF,
    ANM_SURPRISEDWAIT = 0xD8,
    ANM_TAKTDGE = 0xCA,
    ANM_USETCEIVER = 0xC1,
    ANM_WAITAUCTION = 0xE0,
    ANM_WAITLETTER = 0xC7,
    ANM_WAITS = 0,
    ANM_WARPIN = 0xD6,
    ANM_WARPOUT = 0xD7,
    ANM_WARPOUTFIRST = 0xDF,
    ANM_WINDL = 0xB3,
    ANM_WINDR = 0xB4,
    CL_JNT_BODY_CHN_e = 2,
    CL_JNT_LINK_ROOT_e = 0,
    JA_BGM_GET_SONG = 0x80000027u,
    JA_BGM_ITEM_GET = 0x80000002u,
    JA_SE_FT_LADDER_CLIMB_D = 0x3818,
    JA_SE_LK_V_GN_LAST_ATTACK = 0x188B,
    JA_SE_LK_V_TURN_INTO_GN = 0x188A,
    JA_SE_LK_W_DAIZA_ATTACH = 0x2870,
    JA_SE_TALK_START = 0x80A,
    UPPER_MOVE2_e = 2,
    dItemNo_FAIRY_BOTTLE_e = 0x57,
    dRes_INDEX_LINK_BCK_COMBO_TATE_e = 0xC,
    dRes_INDEX_LKANM_BCK_COMBO_KEN_e = 0x3B,
    dRes_INDEX_LKANM__LASTCOMBO_POS_e = 0x28A,
    daPyFlg0_CUT_AT_FLG = 0x40,
    daPyItem_BOKO_e = 0x101,
    daPyItem_NONE_e = 0x100,
    daPyItem_SWORD_e = 0x103,
    daPyProc_DEMO_AGB_USE_e = 0xCA,
    daPyProc_DEMO_BOSS_WARP_e = 0xC9,
    daPyProc_DEMO_BOTTLE_OPEN_FAIRY_e = 0xD1,
    daPyProc_DEMO_GET_DANCE_e = 0xD0,
    daPyProc_DEMO_HAND_UP_e = 0xD9,
    daPyProc_DEMO_ICE_SLIP_e = 0xDA,
    daPyProc_DEMO_LAST_COMBO_e = 0xD8,
    daPyProc_DEMO_LETTER_OPEN_e = 0xCC,
    daPyProc_DEMO_LETTER_READ_e = 0xCD,
    daPyProc_DEMO_LOOK_TURN_e = 0xCB,
    daPyProc_DEMO_OPEN_SALVAGE_TREASURE_e = 0xD3,
    daPyProc_DEMO_POWER_UP_WAIT_e = 0xD5,
    daPyProc_DEMO_POWER_UP_e = 0xD6,
    daPyProc_DEMO_REDEAD_CATCH_e = 0xCF,
    daPyProc_DEMO_REDEAD_STOP_e = 0xCE,
    daPyProc_DEMO_SHIP_SIT_e = 0xD7,
    daPyProc_DEMO_SLIGHT_SURPRISED_e = 0xC7,
    daPyProc_DEMO_SMILE_e = 0xC8,
    daPyProc_DEMO_STAND_ITEM_PUT_e = 0xC5,
    daPyProc_DEMO_SURPRISED_WAIT_e = 0xD4,
    daPyProc_DEMO_VORCANO_FAIL_e = 0xC6,
    daPyProc_DEMO_WARP_SHORT_e = 0xD2,
    daPyProc_DEMO_WIND_CHANGE_e = 0xC4,
    daPyProc_LADDER_DOWN_END_e = 0x3B,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    daPyStts0_UNK2000000_e = 0x2000000,
    ANM_PRESENTATIONA = 0xB2,
    ModeFlg_00000080 = 0x80,
    ModeFlg_00000100 = 0x100,
    ModeFlg_00000400 = 0x400,
    dRes_INDEX_LKANM_BTK_TEDL_e = 0x181,
    dRes_INDEX_LKANM_BTK_TEDR_e = 0x182,
    dRes_INDEX_LKANM_BTK_TEDW_e = 0x183,
    fpcNm_Demo_Item_e = 0x101,
    fpcNm_ITEM_e = 0xFF,
    fpcNm_NPC_FA1_e = 0x168,
    fpcNm_STANDITEM_e = 0x1CE,
};
} // namespace player_demo

struct PlayerDemoCamera {
    u8 beforeAngle[0x236];
    be<s16> mAngleY;
    u8 beforeBody[0x248 - 0x238];
    u8 mBody[0x264 - 0x248];
    cXyz mEye;
    u8 beforeEyeOffset[0x7C0 - 0x270];
    cXyz mEyeOffset;
};
WWHD_OFFSET(PlayerDemoCamera, mAngleY, 0x236);
WWHD_OFFSET(PlayerDemoCamera, mBody, 0x248);
WWHD_OFFSET(PlayerDemoCamera, mEye, 0x264);
WWHD_OFFSET(PlayerDemoCamera, mEyeOffset, 0x7C0);
WWHD_SIZE(PlayerDemoCamera, 0x7CC);
inline PlayerDemoCamera* playerDemoCamera(s32 index) {
    return reinterpret_cast<PlayerDemoCamera*>(dComIfGp_getCamera(index));
}
void playerDemoAddVector(const cXyz*, cXyz*, const cXyz*);
inline void playerDemoCameraEye(u32 address, cXyz* result) {
    PlayerDemoCamera* camera = gabi::at<PlayerDemoCamera>(address);
    playerDemoAddVector(&camera->mEye, result, &camera->mEyeOffset);
}
struct PlayerDemoBottleTuning {
    u8 beforeHeight[0x1180];
    be<f32> mCameraHeight;
    be<f32> mCameraStep;
    be<f32> mCameraDistance;
    be<f32> mCameraDrop;
    u8 beforeAngleStep[0x11F4 - 0x1190];
    be<s16> mCameraAngleStep;
};
WWHD_OFFSET(PlayerDemoBottleTuning, mCameraHeight, 0x1180);
WWHD_OFFSET(PlayerDemoBottleTuning, mCameraStep, 0x1184);
WWHD_OFFSET(PlayerDemoBottleTuning, mCameraDistance, 0x1188);
WWHD_OFFSET(PlayerDemoBottleTuning, mCameraDrop, 0x118C);
WWHD_OFFSET(PlayerDemoBottleTuning, mCameraAngleStep, 0x11F4);
WWHD_SIZE(PlayerDemoBottleTuning, 0x11F6);
struct PlayerDemoPlayView {
    u8 beforeMessageBgm[0x5C20];
    be<u8> mMesgBgm;
};
WWHD_OFFSET(PlayerDemoPlayView, mMesgBgm, 0x5C20);
WWHD_SIZE(PlayerDemoPlayView, 0x5C21);
inline void playerDemoMessageBgmOn() { gabi::at<PlayerDemoPlayView>(dComIfGp_ea())->mMesgBgm = 1; }
inline u8 playerDemoGetItem() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mGetItemNo; }
inline PlayerDemoBottle* playerDemoBottle(u32 address) { return gabi::at<PlayerDemoBottle>(address); }
// HD: retain the polygon assignment's original halfword/word store order, excluding its vtable.
inline void playerDemoCopyPoly(cBgS_PolyInfo* destination, cBgS_PolyInfo* source) {
    destination->mPolyIndex = source->mPolyIndex;
    destination->mActorId = source->mActorId;
    destination->mBgIndex = source->mBgIndex;
    destination->mpBgW = source->mpBgW;
}

inline dBgS_LinChk* playerDemoLineCheck(daPy_lk_c* player) {
    return reinterpret_cast<dBgS_LinChk*>(player->mLinkLinChk);
}
inline cBgS_PolyInfo* playerDemoLinePoly(daPy_lk_c* player) {
    return static_cast<cBgS_PolyInfo*>(playerDemoLineCheck(player));
}
inline cBgS_PolyInfo* playerDemoPolyInfo(daPy_lk_c* player) {
    return reinterpret_cast<cBgS_PolyInfo*>(player->mPolyInfo);
}
BOOL playerDemoChkPolySafe(dBgS*, cBgS_PolyInfo*);
BOOL playerDemoChkMoveBG(dBgS*, cBgS_PolyInfo*);
void playerDemoMoveBgTransPos(dBgS*, cBgS_PolyInfo*, bool, cXyz*, csXyz*, csXyz*);
s32 playerDemoGetWallCode(dBgS*, cBgS_PolyInfo*);
void playerDemoSetActorKeep(daPy_actorKeep_l*, fopAc_ac_c*);
void playerDemoMakeEmitter(daPy_mtxFollowEcallBack_c*, u32, Mtx34*, cXyz*, void*);
BOOL playerDemoFinishLetter(daPy_lk_c*);
inline void playerDemoActorSound(fopAc_ac_c* actor, u32 sound, u32 material) {
    // HD: GHS tests the computed current-position address, including guest-address wraparound.
    if (gabi::ea(actor) + 0x314 != 0) {
        s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
        mDoAud_seStart(sound, &actor->current.pos, material, reverb);
    }
}
struct PlayerDemoVtable {
    u8 beforeVoice[0xE4];
    be<u32> mVoiceStart;
};
WWHD_OFFSET(PlayerDemoVtable, mVoiceStart, 0xE4);
WWHD_SIZE(PlayerDemoVtable, 0xE8);
inline void playerDemoVoiceStart(daPy_lk_c* player, u32 sound) {
    // HD: keep the virtual call and full sound register, rather than calling Link's implementation directly.
    gabi::call_ptr(gabi::at<PlayerDemoVtable>(player->__vtbl)->mVoiceStart, player, sound);
}

struct PlayerDemoLineCheckVtables {
    dBgS_LinChk_vt mConstruct;
    u32 mGroupDestructor;
    u32 mPolyDestructor;
};
extern const PlayerDemoLineCheckVtables l_demoLineCheckVtables;
inline void playerDemoConstructLineCheck(dBgS_LinChk* check) {
    dBgS_LinChk_ct(check, l_demoLineCheckVtables.mConstruct, false);
}
inline void playerDemoDestroyLineCheck(dBgS_LinChk* check) {
    static_cast<dBgS_PolyPassChk*>(check)->__vtbl = l_demoLineCheckVtables.mConstruct.v58;
    static_cast<dBgS_GrpPassChk*>(check)->__vtbl = l_demoLineCheckVtables.mGroupDestructor;
    static_cast<cBgS_PolyInfo*>(check)->__vtbl = l_demoLineCheckVtables.mPolyDestructor;
    cBgS_LinChk_dt(check, 0);
}

// HD: private views cover only the opaque Link animation and foot blocks used here.
#include "gc_api/f_op/f_op_actor.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
struct PlayerMoveAnimation {
    be<f32> mFrame;
};
WWHD_SIZE(PlayerMoveAnimation, 4);
struct PlayerMoveAnmRatio {
    be<f32> mRatio;
    gptr<PlayerMoveAnimation> mpAnmTransform;
    u8 padding[8];
};
WWHD_OFFSET(PlayerMoveAnmRatio, mpAnmTransform, 4);
WWHD_SIZE(PlayerMoveAnmRatio, 0x10);
struct PlayerMoveCalcPack {
    u8 beforeCalc[0x84];
    gptr<struct PlayerMoveCalcObject> mpCalc;
};
WWHD_OFFSET(PlayerMoveCalcPack, mpCalc, 0x84);
WWHD_SIZE(PlayerMoveCalcPack, 0x88);
struct PlayerMoveJointTransform {
    u8 beforeTranslate[0x10];
    cXyz mTranslate;
};
WWHD_OFFSET(PlayerMoveJointTransform, mTranslate, 0x10);
WWHD_SIZE(PlayerMoveJointTransform, 0x1C);
struct PlayerMoveCalcObject {
    u8 beforeTransform[0x2C];
    gptr<PlayerMoveJointTransform> mpTransform;
    u8 beforeFlags[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(PlayerMoveCalcObject, mpTransform, 0x2C);
WWHD_OFFSET(PlayerMoveCalcObject, mFlags, 0x50);
WWHD_SIZE(PlayerMoveCalcObject, 0x54);
struct PlayerMoveFoot {
    u8 beforePoint[0x18];
    cXyz field_0x018;
    u8 afterPoint[0x118-0x24];
};
WWHD_OFFSET(PlayerMoveFoot, field_0x018, 0x18);
WWHD_SIZE(PlayerMoveFoot, 0x118);
struct PlayerMoveSwBlur {
    u8 beforeBuffer[0xAC];
    gptr<cXyz> mpPosBuffer;
};
WWHD_OFFSET(PlayerMoveSwBlur, mpPosBuffer, 0xAC);
WWHD_SIZE(PlayerMoveSwBlur, 0xB0);
struct PlayerMoveMabaState {
    be<u8> m_maba_flg;
    be<u8> m_maba_timer;
};
WWHD_SIZE(PlayerMoveMabaState, 2);
struct PlayerMovePlay {
    u8 beforeMode[0x5292];
    be<u8> mEventMode;
    u8 beforeArrow[0x5BCB-0x5293];
    be<u8> mAdvanceDirection;
};
WWHD_OFFSET(PlayerMovePlay, mEventMode, 0x5292);
WWHD_OFFSET(PlayerMovePlay, mAdvanceDirection, 0x5BCB);
WWHD_SIZE(PlayerMovePlay, 0x5BCC);
inline PlayerMovePlay* playerMovePlay() { return gabi::at<PlayerMovePlay>(dComIfGp_ea()); }
inline PlayerMoveAnmRatio* playerMoveRatio(daPy_lk_c* player, s32 i) { return reinterpret_cast<PlayerMoveAnmRatio*>(player->mAnmRatioUnder)+i; }
inline PlayerMoveFoot* playerMoveFoot(daPy_lk_c* player, s32 i) { return reinterpret_cast<PlayerMoveFoot*>(player->mFootData)+i; }
inline dBgS_GndChk* playerMoveGroundCheck(daPy_lk_c* player) { return reinterpret_cast<dBgS_GndChk*>(player->mGndChk); }
inline cBgS_PolyInfo* playerMoveGroundPoly(daPy_lk_c* player) { return static_cast<cBgS_PolyInfo*>(playerMoveGroundCheck(player)); }
inline dBgS_RoofChk* playerMoveRoof(daPy_lk_c* player) { return reinterpret_cast<dBgS_RoofChk*>(player->mRoofChk); }
// HD: this Link check reads the cross at +0x30, unlike the generic check layout.
struct PlayerMoveLineCross {
    u8 beforeCross[0x30];
    cXyz mCross;
};
WWHD_OFFSET(PlayerMoveLineCross, mCross, 0x30);
WWHD_SIZE(PlayerMoveLineCross, 0x3C);
inline cXyz& playerMoveLineCross(daPy_lk_c* player) { return reinterpret_cast<PlayerMoveLineCross*>(player->mLinkLinChk)->mCross; }
inline be<s16>& playerMoveAngle(s16* angle) { return *reinterpret_cast<be<s16>*>(angle); }
inline Mtx34* playerMoveBaseMatrix(J3DModel* model) { return model->getBaseTRMtx(); }

f64 playerMoveHangSpeedRegister(daPy_lk_c*);
f64 playerMoveClimbSpeedRegister(daPy_lk_c*);
BOOL playerMoveSingleAnimeRegister(daPy_lk_c*, int, f64, f32, int, f32);
BOOL playerMoveCrawlInitRegister(daPy_lk_c*, s32, s32);
cM3dGPla* playerMoveGetTriPla(dBgS*, u32, u32);
f64 playerMoveGroundCross(dBgS*, dBgS_GndChk*);
f64 playerMoveRoofCheck(dBgS*, dBgS_RoofChk*);
s32 playerMoveAttributeCode(dBgS*, cBgS_PolyInfo*);
void playerMoveScaleVector(const cXyz*, cXyz*, f32);
void playerMoveAddVector(const cXyz*, const cXyz*, cXyz*);
void playerMoveSubtractVector(const cXyz*, const cXyz*, cXyz*);
s16 cM_rad2s(f32);
BOOL cM3d_Len2dSqPntAndSegLine(f32, f32, f32, f32, f32, f32, be<f32>*, be<f32>*, f32*);
void playerMoveSetWallR(dBgS_AcchCir*, f32);
f64 playerMoveSquareMagnitudeRegister(cXyz*);
f64 playerMoveSqrtRegister(f64);
BOOL playerMoveCameraChangeModeOK(u32, s32);
u32 playerMoveAllocateAligned(u32, s32);
void playerMoveRestoreHeap(J3DModel*);
void fopAcM_setStageLayer(fopAc_ac_c*);
void playerMoveOnEventBit(void*, u32);

inline cM3dGPla* playerMoveTriPla(cBgS_PolyInfo* poly) { return playerMoveGetTriPla(dComIfG_Bgsp(), (u32)poly->mBgIndex, (u32)poly->mPolyIndex); }
inline BOOL playerMoveChkPolySafe(cBgS_PolyInfo* poly) { return playerDemoChkPolySafe(dComIfG_Bgsp(), poly); }
inline BOOL playerMoveChkMoveBG(cBgS_PolyInfo* poly) { return playerDemoChkMoveBG(dComIfG_Bgsp(), poly); }
inline void playerMoveMoveBgTransPos(cBgS_PolyInfo* poly, bool enable, cXyz* pos, csXyz* angle, csXyz* shape) { playerDemoMoveBgTransPos(dComIfG_Bgsp(), poly, enable, pos, angle, shape); }
inline s32 playerMoveGetWallCode(cBgS_PolyInfo* poly) { return playerDemoGetWallCode(dComIfG_Bgsp(), poly); }
inline s32 playerMoveGetAttributeCode(cBgS_PolyInfo* poly) { return playerMoveAttributeCode(dComIfG_Bgsp(), poly); }

namespace player_move {
enum : u32 {
    dSv_event_flag_UNK_4020 = 0x4020,
    dRes_INDEX_LKANM_BCK_REST_e = 0xD7,
    dCamAttnStts_00001000_e = 0x1000,
    dCamAttnStts_00000080_e = 0x80,
    ModeFlg_04000000 = 0x4000000,
    ModeFlg_00000001 = 1,
    ModeFlg_00000100 = 0x100,
    ModeFlg_00000400 = 0x400,
    ANM_FCLIMBSLIDERUP = 0x8D,
    ANM_FCLIMBSLIDELUP = 0x8C,
    ANM_HANGMOVER = 0x53,
    ANM_HANGMOVEL = 0x52,
    dBgS_Attr_UNK1B_e = 0x1B,
    ANM_GRABNG = 0x67,
    ANM_GRABUP = 0x66,
    ANM_HANGUP = 0x51,
    ANM_LIE = 0x40,
    ANM_VJMPCHA = 0x4D,
    ANM_VJMPCL = 0x4F,
    ANM_WALL = 0x42,
    ANM_WALLDW = 0x43,
    ANM_WALLPL = 0x48,
    ANM_WALLPLDW = 0x4A,
    ANM_WALLPR = 0x49,
    ANM_WALLPRDW = 0x4B,
    ANM_WALLWL = 0x44,
    ANM_WALLWLDW = 0x46,
    ANM_WALLWR = 0x45,
    ANM_WALLWRDW = 0x47,
    DIR_BACKWARD = 1,
    DIR_FORWARD = 0,
    DIR_LEFT = 2,
    DIR_RIGHT = 3,
    ModeFlg_HANG = 0x20,
    dActStts_BLANK_e = 0,
    dActStts_CROUCH_e = 0xF,
    dActStts_LET_GO_e = 6,
    dActStts_SIDLE_e = 0x10,
    daPyFlg0_UNK10000 = 0x10000,
    daPyFlg0_UNK80000000 = 0x80000000,
    daPyFlg1_VINE_CATCH = 0x2000000,
    daPyProc_CLIMB_DOWN_START_e = 0x3E,
    daPyProc_CLIMB_MOVE_SIDE_e = 0x40,
    daPyProc_CLIMB_MOVE_UP_DOWN_e = 0x3F,
    daPyProc_CRAWL_AUTO_MOVE_e = 0x11,
    daPyProc_CRAWL_END_e = 0x12,
    daPyProc_CRAWL_MOVE_e = 0x10,
    daPyProc_FALL_e = 0x27,
    daPyProc_GRAB_MISS_e = 0x70,
    daPyProc_GRAB_UP_e = 0x6F,
    daPyProc_HANG_CLIMB_e = 0x30,
    daPyProc_HANG_MOVE_e = 0x2F,
    daPyProc_HANG_UP_e = 0x2D,
    daPyProc_HANG_WAIT_e = 0x2E,
    daPyProc_WHIDE_MOVE_e = 0x15,
    daPyProc_WHIDE_PEEP_e = 0x16,
    daPyProc_WHIDE_READY_e = 0x13,
    daPyProc_WHIDE_WAIT_e = 0x14,
    daPyRFlg0_CRAWL_AUTO_MOVE = 0x1000,
    daPyRFlg0_GRAB_UP_START = 0x8000,
    daPyRFlg0_SUBJECT_ACCEPT = 0x4000000,
    daPyStts0_CRAWL_e = 0x8000000,
    daPyStts0_HANG_e = 0x100,
    daPyStts0_SUBJECT_e = 0x2000,
    daPyStts0_UNK1_e = 1,
    daPyStts0_UNK200_e = 0x200,
    daPyStts0_UNK20_e = 0x20,
    daPyStts0_UNK40_e = 0x40,
    daPyStts1_UNK10000_e = 0x10000,
    daPyStts1_UNK40000_e = 0x40000,
    fpcNm_Stone2_e = 0x1C7,
};
} // namespace player_move
