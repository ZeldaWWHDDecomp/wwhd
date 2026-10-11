/** Link range 07: grab, swimming, ship and grappling-hook work views.
 * HD: the common Link layout is shared by independently verified ranges. These
 * views describe the animation heap's unchanged 0x10-byte entries locally.
 */
#pragma once
#include "d/actor/d_a_player_main.h"
#include "gc_api/f_op/f_op_actor.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
// HD: the shared collision header already provides the cylinder layout.
class cM3dGCps;
void cM3dGLin_SetStartEnd(cM3dGCps*, const cXyz*, const cXyz*);

namespace Link07 {
// HD: stage interfaces expose metadata through virtual slots in the expanded table.
struct StageVtable {
    /* 0x000 */ u8 mSlots[0x15C];
    /* 0x15C */ be<u32> mGetStageInfo;
    /* 0x160 */ u8 mRoomSlots[0x7C];
    /* 0x1DC */ be<u32> mGetFileListInfo;
};
WWHD_SIZE(StageVtable, 0x1E0);
WWHD_OFFSET(StageVtable, mGetStageInfo, 0x15C);
WWHD_OFFSET(StageVtable, mGetFileListInfo, 0x1DC);
struct StageInterface {
    gptr<StageVtable> mpVtable;
};
WWHD_SIZE(StageInterface, 4);
struct StageInfo {
    /* 0x00 */ u8 mWork[0xC];
    /* 0x0C */ be<u32> mStageType;
};
WWHD_SIZE(StageInfo, 0x10);
WWHD_OFFSET(StageInfo, mStageType, 0xC);
struct FileListInfo {
    be<u32> mFlags;
};
WWHD_SIZE(FileListInfo, 4);
struct MessagePlayState {
    /* 0x0000 */ u8 mWork[0x5C20];
    /* 0x5C20 */ be<u8> mMesgBgm;
};
WWHD_SIZE(MessagePlayState, 0x5C21);
WWHD_OFFSET(MessagePlayState, mMesgBgm, 0x5C20);

// HD: these process and resource IDs preserve the verified HD values.
enum : u32 {
    ANM_TAKTAYATSURI = 0xE8,
    ANM_TAKTCHISIN = 0xE9,
    ANM_TAKTCHUYA = 0xE6,
    ANM_TAKTFUJIN = 0xE7,
    ANM_TAKTKAZE = 0xE4,
    ANM_TAKTSIPPU = 0xE5,
    AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_STALFOS_MACE = 0x1000000,
    AT_TYPE_WATER = 0x100,
    CL_JNT_CHIN_JNT_e = 0x12,
    CL_JNT_CL_LHANDA_e = 8,
    CL_JNT_HEAD_JNT_e = 0xF,
    CL_JNT_LINK_ROOT_e = 0,
    CL_JNT_RTOE_JNT_e = 0x28,
    DIR_FORWARD = 0,
    ID_AK_JN_ELEMENTHIBANA00 = 0x2C,
    ID_AK_JN_ELEMENTKIKUZU00 = 0x2B,
    ID_AK_JN_FORESTWATER00 = 0x20D,
    ID_AK_JN_LEAFFAN00 = 0x4A,
    ID_AK_JN_LEAFPARACHUTE00 = 0x4B,
    ID_AK_JN_LEAFPARACHUTE01 = 0x4C,
    ID_AK_JN_SPILLWATER00 = 0x279,
    ID_AK_JN_TAKT01 = 0x51,
    ID_AK_JN_UCHIWAWIND00 = 0x32,
    ID_AK_SN_VOLCANICASHESFAN00 = 0x8236,
    ID_IT_JN_HM_SENKO00 = 0x214,
    ID_IT_JN_HM_SENPU00 = 0x215,
    ID_IT_JN_HM_SYOGEKI00 = 0x216,
    ID_IT_JN_HM_WP00 = 0x27C,
    ID_IT_JN_LK_GEPPU00 = 0x57,
    ID_IT_JN_WP_HAMON01 = 0x3D,
    ID_IT_JN_WP_HAMON03 = 0x3F,
    ID_IT_JN_WP_SHIBUKI = 0x40,
    UPPER_MOVE1_e = 1,
    dRes_INDEX_LKANM_BCK_ACTIONTAKTRDW_e = 8,
    dRes_INDEX_LKANM_BCK_ACTIONTAKTRUP_e = 0xB,
    dRes_INDEX_LKANM_BCK_BINDRINKING_e = 0x2B,
    dRes_INDEX_LKANM_BCK_BINDRINKPRE_e = 0x2C,
    dRes_INDEX_LKANM_BCK_BINOPENPRE_e = 0x30,
    dRes_INDEX_LKANM_BCK_FANB2A_e = 0x83,
    dRes_INDEX_LKANM_BCK_FANBA_e = 0x84,
    dRes_INDEX_LKANM_BCK_HAMSWINGBHITA_e = 0x9C,
    dRes_INDEX_LKANM_BCK_WAITTAKTLHANDD_e = 0x128,
    dRes_INDEX_LKANM_BCK_WAITTAKTLHANDL_e = 0x129,
    dRes_INDEX_LKANM_BCK_WAITTAKTLHANDR_e = 0x12A,
    dRes_INDEX_LKANM_BCK_WAITTAKTLHANDU_e = 0x12B,
    dRes_INDEX_LKANM_BCK_WAITTAKTRHANDL_e = 0x12C,
    dRes_INDEX_LKANM_BCK_WAITTAKTRHANDR_e = 0x12D,
    dRes_INDEX_LKANM_BCK_WAITTAKT_e = 0x127,
    daPyStts0_UNK4000000_e = 0x4000000,
    daPyStts1_UNK1000_e = 0x1000,
    daPyStts1_WIND_WAKER_CONDUCT_e = 1,
    fpcNm_HIMO2_e = 0x1BE,
    fpcNm_NH_e = 0x139,
    fpcNm_TORNADO_e = 0x1BB,
    fpcNm_WATER_MARK_e = 0x1D4,
};

// HD: the rope actors retain their GC fields after expanded renderer storage.
struct Rope2 {
    /* 0x000 */ u8 mWork[0x3F8];
    /* 0x3F8 */ be<u32> m02DC;
};
WWHD_SIZE(Rope2, 0x3FC);
WWHD_OFFSET(Rope2, m02DC, 0x3F8);
struct Rope3 {
    /* 0x0000 */ u8 mWork[0x1914];
    /* 0x1914 */ be<f32> m15FC;
};
WWHD_SIZE(Rope3, 0x1918);
WWHD_OFFSET(Rope3, m15FC, 0x1914);

// HD: the system root owns wind direction, tact input and learned-song state.
struct WindState {
    /* 0x00 */ u8 mWork[0x92];
    /* 0x92 */ be<u16> mAngle;
};
WWHD_SIZE(WindState, 0x94);
WWHD_OFFSET(WindState, mAngle, 0x92);
struct TactInput {
    /* 0x000 */ u8 mWork[0x16B];
    /* 0x16B */ be<u8> mTactOn;
};
WWHD_SIZE(TactInput, 0x16C);
WWHD_OFFSET(TactInput, mTactOn, 0x16B);
struct HDSystem {
    /* 0x000 */ u8 mWork[0x1C4];
    /* 0x1C4 */ gptr<WindState> mpWindState;
    /* 0x1C8 */ u8 mInputWork[0x14];
    /* 0x1DC */ gptr<TactInput> mpTactInput;
    /* 0x1E0 */ u8 mSongWork[0x61];
    /* 0x241 */ be<u8> mLearnedSongs;
};
WWHD_SIZE(HDSystem, 0x242);
WWHD_OFFSET(HDSystem, mpWindState, 0x1C4);
WWHD_OFFSET(HDSystem, mpTactInput, 0x1DC);
WWHD_OFFSET(HDSystem, mLearnedSongs, 0x241);
struct SystemPointer {
    gptr<HDSystem> mpSystem;
};
WWHD_SIZE(SystemPointer, 4);
struct MessageManager {
    /* 0x000 */ u8 mWork[0x921];
    /* 0x921 */ be<u8> mMessageSend;
};
WWHD_SIZE(MessageManager, 0x922);
WWHD_OFFSET(MessageManager, mMessageSend, 0x921);
struct MessagePointer {
    gptr<MessageManager> mpManager;
};
WWHD_SIZE(MessagePointer, 4);

// HD: capsule storage is embedded in the expanded collision objects.
struct CapsuleGeometry {
    cXyz mStart;
    cXyz mEnd;
    be<u32> mVtable;
    be<f32> mRadius;
};
WWHD_SIZE(CapsuleGeometry, 0x20);
WWHD_OFFSET(CapsuleGeometry, mRadius, 0x1C);

// HD: global particle colors follow the expanded manager work area.
struct ParticleManagerColor {
    be<u8> r, g, b;
};
WWHD_SIZE(ParticleManagerColor, 3);
struct ParticleManager {
    /* 0x00 */ u8 mWork[0xF0];
    /* 0xF0 */ ParticleManagerColor mGlobalColor;
};
WWHD_SIZE(ParticleManager, 0xF3);
WWHD_OFFSET(ParticleManager, mGlobalColor, 0xF0);

// HD: item, action and sound IDs retain the verified numeric values.
enum : u32 {
    JA_SE_LK_FAN_CHUTE_CLOSE = 0x2847,
    JA_SE_LK_FAN_CHUTE_RISE = 0x2846,
    JA_SE_LK_FAN_CHUTE_WATER = 0x2848,
    JA_SE_LK_FAN_SWING = 0x2844,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_LK_HAMMER_SWING = 0x2857,
    JA_SE_LK_JUMP_FLOWER_OUT = 0x2849,
    JA_SE_LK_ROPE_SWING_F = 0x201E,
    JA_SE_LK_SCOOP_WATER = 0x287E,
    JA_SE_LK_SPRINKLE_WATER = 0x287F,
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_WTAKT_USING = 0x205E,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_ME_ITEM_GET_S = 0x8F5,
    JA_SE_TAKT_MATCHED = 0x871,
    JA_SE_TAKT_USE_CANCEL = 0x870,
    JA_SE_WTAKT_MATCH_SIGNAL = 0x896,
    dActStts_CANCEL_e = 0x27,
    dActStts_GRAB_e = 0x11,
    dItemNo_DEKU_LEAF_e = 0x34,
    dItemNo_FIREFLY_BOTTLE_e = 0x58,
    dItemNo_FOREST_WATER_e = 0x59,
    dItemNo_GREEN_POTION_e = 0x52,
    dItemNo_HALF_SOUP_BOTTLE_e = 0x54,
    dItemNo_RED_POTION_e = 0x51,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dItemNo_SOUP_BOTTLE_e = 0x55,
    dItemNo_WATER_BOTTLE_e = 0x56,
};

// HD: sight collision data follows the expanded hookshot renderer.
struct PlayerVtable {
    /* 0x00 */ u8 mSlots[0xDC];
    /* 0xDC */ be<u32> mCheckRopeReadyAnime;
    /* 0xE0 */ u8 mReserved[4];
    /* 0xE4 */ be<u32> mVoiceStart;
};
WWHD_SIZE(PlayerVtable, 0xE8);
WWHD_OFFSET(PlayerVtable, mCheckRopeReadyAnime, 0xDC);
WWHD_OFFSET(PlayerVtable, mVoiceStart, 0xE4);
inline void voiceStart(daPy_lk_c* player, u32 voice) {
    gabi::call_ptr(gabi::at<PlayerVtable>(player->__vtbl)->mVoiceStart, player, voice);
}
struct HookshotSight {
    /* 0x00 */ be<u32> mObjHookFlg;
    /* 0x04 */ u8 mWork[0x18];
    /* 0x1C */ cXyz mObjSightCrossPos;
    /* 0x28 */ u8 mCollisionWork[0xFC];
    /* 0x124 */ u8 mSightHit[4];
};
WWHD_SIZE(HookshotSight, 0x128);
WWHD_OFFSET(HookshotSight, mObjSightCrossPos, 0x1C);
WWHD_OFFSET(HookshotSight, mSightHit, 0x124);
struct Hookshot : fopAc_ac_c {
    /* 0x3AC */ u8 mWork[0xD4FE - 0x3AC];
    /* 0xD4FE */ be<u8> mShipRideFlg;
    /* 0xD4FF */ u8 mSightPadding[0xD50C - 0xD4FF];
    /* 0xD50C */ HookshotSight mSight;
};
WWHD_SIZE(Hookshot, 0xD634);
WWHD_OFFSET(Hookshot, mSight, 0xD50C);
WWHD_OFFSET(Hookshot, mShipRideFlg, 0xD4FE);
struct Boomerang {
    /* 0x00000 */ u8 mWork[0x26274];
    /* 0x26274 */ be<u8> mCancelFlg;
};
WWHD_SIZE(Boomerang, 0x26275);
WWHD_OFFSET(Boomerang, mCancelFlg, 0x26274);
struct AttentionEntry {
    /* 0x00 */ u8 mWork[8];
    /* 0x08 */ be<s32> mType;
};
WWHD_SIZE(AttentionEntry, 0xC);
WWHD_OFFSET(AttentionEntry, mType, 8);
struct SalvageRegistry {
    /* 0x00 */ be<u32> mRegistry;
    /* 0x04 */ be<s32> mId;
};
WWHD_SIZE(SalvageRegistry, 8);

struct Attention {
    /* 0x00 */ u8 mWork[0x20];
    /* 0x20 */ be<u32> mFlags;
};
WWHD_SIZE(Attention, 0x24);
WWHD_OFFSET(Attention, mFlags, 0x20);
struct BodyModel {
    /* 0x00 */ u8 mWork[0xC8];
    /* 0xC8 */ Mtx34 mBaseTransform;
};
WWHD_SIZE(BodyModel, 0xF8);
WWHD_OFFSET(BodyModel, mBaseTransform, 0xC8);
struct BodyAnimation {
    /* 0x00 */ u8 mWork[0x90];
    /* 0x90 */ gptr<BodyModel> mpModel;
};
WWHD_SIZE(BodyAnimation, 0x94);
WWHD_OFFSET(BodyAnimation, mpModel, 0x90);

// HD: animation ratios are stored per joint after the GC ratio/animation pair.
struct FrameAnimation;
struct AnmRatioPack {
    /* 0x00 */ be<f32> mRatio;
    /* 0x04 */ gptr<FrameAnimation> mAnmTransform;
    /* 0x08 */ be<s32> mJointNum;
    /* 0x0C */ gptr<be<f32>> mpJointRatios;
};
WWHD_SIZE(AnmRatioPack, 0x10);
WWHD_OFFSET(AnmRatioPack, mJointNum, 8);
WWHD_OFFSET(AnmRatioPack, mpJointRatios, 0xC);
// HD: accessing an animation matrix also dirties the matrix-buffer flags.
struct MatrixBuffer {
    /* 0x00 */ u8 mWork[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 mFlagWork[0xA];
    /* 0x10 */ gptr<Mtx34> mpAnmMtx;
};
WWHD_SIZE(MatrixBuffer, 0x14);
WWHD_OFFSET(MatrixBuffer, mFlags, 4);
WWHD_OFFSET(MatrixBuffer, mpAnmMtx, 0x10);
struct MatrixModel {
    /* 0x00 */ u8 mWork[0x2C];
    /* 0x2C */ gptr<MatrixBuffer> mpMtxBuffer;
};
WWHD_SIZE(MatrixModel, 0x30);
WWHD_OFFSET(MatrixModel, mpMtxBuffer, 0x2C);
// HD: equipment models are separated by expanded texture-animation objects.
// HD: the blur object is separately allocated; the GC point arrays follow a larger header.
struct SwordBlur {
    /* 0x000 */ u8 mWork[0x9C];
    /* 0x09C */ be<s32> field_0x014;
    /* 0x0A0 */ u8 mPointWork[0x1C];
    /* 0x0BC */ cXyz field_0x034[60];
};
WWHD_SIZE(SwordBlur, 0x38C);
WWHD_OFFSET(SwordBlur, field_0x014, 0x9C);
WWHD_OFFSET(SwordBlur, field_0x034, 0xBC);
struct HammerGuards {
    be<u32> mSmokeInitialized;
    be<u32> mScaleInitialized;
    be<u32> mTransInitialized;
};
WWHD_SIZE(HammerGuards, 0xC);
WWHD_OFFSET(HammerGuards, mScaleInitialized, 4);
WWHD_OFFSET(HammerGuards, mTransInitialized, 8);
struct TactBrkAnimation {
    /* 0x00 */ u8 mAnimationWork[0xA];
    /* 0x0A */ be<s16> mEndFrame;
};
WWHD_SIZE(TactBrkAnimation, 0xC);
WWHD_OFFSET(TactBrkAnimation, mEndFrame, 0xA);
struct PlayerModelWork {
    /* 0x0000 */ u8 mPlayerWork[0x3AC];
    /* 0x03AC */ be<u8> mCutType;
    /* 0x03AD */ u8 mCutWork[0x4440 - 0x3AD];
    /* 0x4440 */ gptr<BodyModel> mpEquipItemModel;
    /* 0x4444 */ u8 mEquipAnimationWork[0x44D0 - 0x4444];
    /* 0x44D0 */ gptr<void> mpParachuteFanMorf;
    /* 0x44D4 */ u8 mParachuteAnimationWork[0x48F8 - 0x44D4];
    /* 0x48F8 */ TactBrkAnimation mEquipItemBrk;
    /* 0x4904 */ u8 mEquipBrkWork[0x4970 - 0x4904];
    /* 0x4970 */ gptr<BodyModel> mpBottleContentsModel;
    /* 0x4974 */ u8 mBottleAnimationWork[0x53F0 - 0x4974];
    /* 0x53F0 */ gptr<BodyModel> mpYuchw00Model;
};
WWHD_SIZE(PlayerModelWork, 0x53F4);
WWHD_OFFSET(PlayerModelWork, mCutType, 0x3AC);
WWHD_OFFSET(PlayerModelWork, mpEquipItemModel, 0x4440);
WWHD_OFFSET(PlayerModelWork, mpParachuteFanMorf, 0x44D0);
WWHD_OFFSET(PlayerModelWork, mEquipItemBrk, 0x48F8);
WWHD_OFFSET(PlayerModelWork, mpBottleContentsModel, 0x4970);
WWHD_OFFSET(PlayerModelWork, mpYuchw00Model, 0x53F0);
inline PlayerModelWork& models(daPy_lk_c* player) { return *gabi::at<PlayerModelWork>(gabi::ea(player)); }
struct AnmHeap {
    /* 0x00 */ be<u16> mIdx;
    /* 0x02 */ u8 mResourceWork[0x0E];
};
WWHD_SIZE(AnmHeap, 0x10);
WWHD_OFFSET(AnmHeap, mIdx, 0);
inline AnmHeap& upperHeap(daPy_lk_c* player, int part) {
    return *gabi::at<AnmHeap>(gabi::ea(player->m_anm_heap_upper) + part * sizeof(AnmHeap));
}
inline AnmHeap& underHeap(daPy_lk_c* player, int part) {
    return *gabi::at<AnmHeap>(gabi::ea(player->m_anm_heap_under) + part * sizeof(AnmHeap));
}

// HD: the blend calculator stores per-joint weights instead of two GC ratios.
struct JointWeights {
    /* 0x00 */ u8 mWork[0x1C];
    /* 0x1C */ gptr<be<f32>> mpUnderRatio;
    /* 0x20 */ u8 mRatioWork[0xC];
    /* 0x2C */ gptr<be<f32>> mpWeights;
};
WWHD_SIZE(JointWeights, 0x30);
WWHD_OFFSET(JointWeights, mpUnderRatio, 0x1C);
WWHD_OFFSET(JointWeights, mpWeights, 0x2C);
struct BlendModel {
    /* 0x00 */ u8 mWork[0xAC];
    /* 0xAC */ gptr<void> mpModelData;
};
WWHD_SIZE(BlendModel, 0xB0);
WWHD_OFFSET(BlendModel, mpModelData, 0xAC);
struct BlendCalc {
    /* 0x00 */ u8 mWork[0x7C];
    /* 0x7C */ gptr<JointWeights> mpJointWeights;
    /* 0x80 */ gptr<BlendModel> mpModel;
};
WWHD_SIZE(BlendCalc, 0x84);
WWHD_OFFSET(BlendCalc, mpJointWeights, 0x7C);
WWHD_OFFSET(BlendCalc, mpModel, 0x80);
// HD: Link keeps two 0x28-byte callbacks and extra talk state after the GC fields.
struct SwimTailCallback {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> mbEnd;
    /* 0x05 */ be<u8> field_0x05;
    /* 0x06 */ u8 mPadding[2];
    /* 0x08 */ be<f32> mSpeedRate;
    /* 0x0C */ be<f32> mWaterY;
    /* 0x10 */ be<f32> mWaterFlatY;
    /* 0x14 */ cXyz mPos;
    /* 0x20 */ gptr<const csXyz> field_0x20;
    /* 0x24 */ gptr<void> mpEmitter;
    void onEnd() {
        mbEnd = 1;
        field_0x20 = nullptr;
    }
};
WWHD_SIZE(SwimTailCallback, 0x28);
WWHD_OFFSET(SwimTailCallback, mbEnd, 4);
WWHD_OFFSET(SwimTailCallback, field_0x20, 0x20);
WWHD_OFFSET(SwimTailCallback, mpEmitter, 0x24);
struct TailState {
    /* 0x00 */ be<u32> mSwimTalkBlocked;
    /* 0x04 */ be<u8> mTingleTalk;
    /* 0x05 */ u8 mWork[0x17];
    /* 0x1C */ be<u8> mBoomerangFlag;
    /* 0x1D */ u8 mTail[7];
};
WWHD_SIZE(TailState, 0x24);
WWHD_OFFSET(TailState, mSwimTalkBlocked, 0);
WWHD_OFFSET(TailState, mTingleTalk, 4);
WWHD_OFFSET(TailState, mBoomerangFlag, 0x1C);
struct TransformInfo {
    /* 0x00 */ cXyz mScale;
    /* 0x0C */ csXyz mRotation;
    /* 0x12 */ u8 mPadding[2];
    /* 0x14 */ cXyz mTranslate;
};
WWHD_SIZE(TransformInfo, 0x20);
WWHD_OFFSET(TransformInfo, mTranslate, 0x14);
struct OldFrameData {
    /* 0x00 */ u8 mWork[0xC];
    /* 0x0C */ be<f32> mOldFrameRate;
    /* 0x10 */ u8 mRateWork[0xC];
    /* 0x1C */ gptr<TransformInfo> mpOldFrameTransInfo;
};
WWHD_SIZE(OldFrameData, 0x20);
WWHD_OFFSET(OldFrameData, mpOldFrameTransInfo, 0x1C);
WWHD_OFFSET(OldFrameData, mOldFrameRate, 0xC);
inline dComIfG_inf_c* gameInfo() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea()); }
inline TailState& tail(daPy_lk_c* player) { return *gabi::at<TailState>(gabi::ea(player->m8260)); }
inline SwimTailCallback& swimTail(daPy_lk_c* player, int side) {
    return *gabi::at<SwimTailCallback>(gabi::ea(player->mSwimTailEcallBack) + side * sizeof(SwimTailCallback));
}
// GameCube ship fields retained by HD; the renderer expands the gaps between them.
struct Ship : fopAc_ac_c {
    enum { MODE_PADDLE_MOVE_e = 2, MODE_READY_SECOND_e = 4, MODE_GET_OFF_FIRST_e = 5, MODE_GET_OFF_SECOND_e = 6 };
    enum { PART_WAIT_e = 0, PART_STEER_e = 1, DEMO_SALVAGE_e = 2 };
    enum : u32 { daSFLG_FLY_e = 1, daSFLG_CRANE_UP_END_e = 0x40000 };
    /* 0x03AC */ u8 mPhaseWork[8];
    /* 0x03B4 */ gptr<void> mpBodyAnm;
    /* 0x03B8 */ u8 mRenderWork[0x636 - 0x3B8];
    /* 0x0636 */ be<u8> mNextMode;
    /* 0x0637 */ be<u8> mPart;
    /* 0x0638 */ u8 mModeWork[2];
    /* 0x063A */ be<u8> m0351;
    /* 0x063B */ u8 mDemoWork[9];
    /* 0x0644 */ be<u32> mStateFlag;
    /* 0x0648 */ u8 mCraneWork[0x682 - 0x648];
    /* 0x0682 */ be<s16> mCraneBaseAngle;
    /* 0x0684 */ u8 mAngleWork[2];
    /* 0x0686 */ be<s16> mRopeCnt;
    /* 0x0688 */ u8 mMoveWork[0x6AC - 0x688];
    /* 0x06AC */ be<s32> mTactWarpPosNum;
    /* 0x06B0 */ u8 mWarpWork[0x704 - 0x6B0];
    /* 0x0704 */ be<u32> mTornadoActor;
    /* 0x0708 */ u8 mWhirlWork[4];
    /* 0x070C */ be<u32> mWhirlActor;
    /* 0x0710 */ u8 mWarpIDWork[4];
    /* 0x0714 */ be<u32> mTactWarpID;
    /* 0x0718 */ u8 mRopeWork[0x132C - 0x718];
    /* 0x132C */ cXyz m1044;
    void setPaddleMove() { mNextMode = MODE_PADDLE_MOVE_e; }
    void setReadySecond() { mNextMode = MODE_READY_SECOND_e; }
    void setGetOffFirst() { mNextMode = MODE_GET_OFF_FIRST_e; }
    void setGetOffSecond() { mNextMode = MODE_GET_OFF_SECOND_e; }
};
WWHD_SIZE(Ship, 0x1338);
WWHD_OFFSET(Ship, mpBodyAnm, 0x3B4);
WWHD_OFFSET(Ship, mNextMode, 0x636);
WWHD_OFFSET(Ship, mPart, 0x637);
WWHD_OFFSET(Ship, m0351, 0x63A);
WWHD_OFFSET(Ship, mStateFlag, 0x644);
WWHD_OFFSET(Ship, mCraneBaseAngle, 0x682);
WWHD_OFFSET(Ship, mRopeCnt, 0x686);
WWHD_OFFSET(Ship, mTornadoActor, 0x704);
WWHD_OFFSET(Ship, mWhirlActor, 0x70C);
WWHD_OFFSET(Ship, m1044, 0x132C);
WWHD_OFFSET(Ship, mTactWarpPosNum, 0x6AC);
WWHD_OFFSET(Ship, mTactWarpID, 0x714);
inline Ship* ship(u32 address) { return gabi::at<Ship>(address); }
struct SightPacket {
    /* 0x00 */ be<u32> mVtable;
    /* 0x04 */ be<u8> mDrawFlag;
    /* 0x05 */ be<u8> mLockFlag;
    /* 0x06 */ be<u8> mFrame;
    /* 0x07 */ u8 mPadding;
    /* 0x08 */ cXyz mPos;
};
WWHD_SIZE(SightPacket, 0x14);
WWHD_OFFSET(SightPacket, mDrawFlag, 4);
WWHD_OFFSET(SightPacket, mLockFlag, 5);
WWHD_OFFSET(SightPacket, mFrame, 6);
WWHD_OFFSET(SightPacket, mPos, 8);
// HD: Link's line check exposes the hit position in its line's end slot.
struct LinkLineHit {
    /* 0x00 */ u8 mCheckWork[0x14];
    /* 0x14 */ cBgS_PolyInfo mPolyInfo;
    /* 0x24 */ u8 mLineWork[0x0C];
    /* 0x30 */ cXyz mCross;
};
WWHD_SIZE(LinkLineHit, 0x3C);
WWHD_OFFSET(LinkLineHit, mPolyInfo, 0x14);
WWHD_OFFSET(LinkLineHit, mCross, 0x30);
inline LinkLineHit& lineHit(daPy_lk_c* player) { return *gabi::at<LinkLineHit>(gabi::ea(player->mLinkLinChk)); }
struct RopeScales {
    /* 0x00 */ be<u32> mDynamicsGuard;
    /* 0x04 */ be<u32> mParticleGuard;
    /* 0x08 */ cXyz mDynamics;
    /* 0x14 */ cXyz mParticle;
};
WWHD_SIZE(RopeScales, 0x20);
WWHD_OFFSET(RopeScales, mDynamics, 8);
WWHD_OFFSET(RopeScales, mParticle, 0x14);
struct RopeGuards {
    be<u32> mInitialized[5];
};
WWHD_SIZE(RopeGuards, 0x14);
struct RopeOffsets {
    cXyz mPosition[5];
};
WWHD_SIZE(RopeOffsets, 0x3C);
struct RemoveCallbackVtable {
    /* 0x00 */ u8 mSlots[0x44];
    /* 0x44 */ be<u32> mRemove;
};
WWHD_SIZE(RemoveCallbackVtable, 0x48);
WWHD_OFFSET(RemoveCallbackVtable, mRemove, 0x44);
struct SmokeCallback {
    /* 0x00 */ be<u32> mVtable;
    /* 0x04 */ u8 mWork[0x1C];
    void remove() { gabi::call_ptr(gabi::at<RemoveCallbackVtable>(mVtable)->mRemove, this); }
};
WWHD_SIZE(SmokeCallback, 0x20);
struct FollowCallback {
    /* 0x00 */ be<u32> mVtable;
    /* 0x04 */ u8 mWork[0x10];
    void remove() { gabi::call_ptr(gabi::at<RemoveCallbackVtable>(mVtable)->mRemove, this); }
};
WWHD_SIZE(FollowCallback, 0x14);
struct FootEffect {
    /* 0x00 */ SmokeCallback mSmokeCallback;
    /* 0x20 */ FollowCallback mOtherCallback;
    /* 0x34 */ cXyz mPos;
    /* 0x40 */ csXyz mAngle;
    /* 0x46 */ u8 mPadding[2];
    /* 0x48 */ be<s32> mID;
};
WWHD_SIZE(FootEffect, 0x4C);
WWHD_OFFSET(FootEffect, mOtherCallback, 0x20);
WWHD_OFFSET(FootEffect, mPos, 0x34);
WWHD_OFFSET(FootEffect, mAngle, 0x40);
WWHD_OFFSET(FootEffect, mID, 0x48);
// HD: particle colors remain byte fields in the expanded emitter.
struct ParticleColor {
    be<u8> r, g, b, a;
};
WWHD_SIZE(ParticleColor, 4);
WWHD_OFFSET(ParticleColor, g, 1);
WWHD_OFFSET(ParticleColor, b, 2);
// HD: the last particle is reached through the list node's object pointer.
struct Particle {
    /* 0x00 */ u8 mWork[0x28];
    /* 0x28 */ cXyz mGlobalPosition;
};
WWHD_SIZE(Particle, 0x34);
WWHD_OFFSET(Particle, mGlobalPosition, 0x28);
struct ParticleNode {
    gptr<Particle> mpObject;
};
WWHD_SIZE(ParticleNode, 4);
struct Emitter {
    /* 0x000 */ u8 mWork[0x34];
    /* 0x034 */ be<f32> mRate;
    /* 0x038 */ u8 mRateWork[0x20];
    /* 0x058 */ be<f32> mSpread;
    /* 0x05C */ be<u32> mMaxFrame;
    /* 0x060 */ be<s16> mLifeTime;
    /* 0x062 */ u8 mLifeWork[6];
    /* 0x068 */ be<f32> mAwayFromCenterSpeed;
    /* 0x06C */ be<f32> mAwayFromAxisSpeed;
    /* 0x070 */ be<f32> mDirectionalSpeed;
    /* 0x074 */ u8 mSweepWork[8];
    /* 0x07C */ be<f32> mVolumeSweep;
    /* 0x080 */ u8 mParticleWork[0x1B0 - 0x80];
    /* 0x1B0 */ gptr<ParticleNode> mpLastParticle;
    /* 0x1B4 */ u8 mListWork[0x1E4 - 0x1B4];
    /* 0x1E4 */ gptr<void> mpCallBack;
    /* 0x1E8 */ u8 mTransformWork[0x220 - 0x1E8];
    /* 0x220 */ cXyz mGlobalDynamicsScale;
    /* 0x22C */ u8 mScaleWork[0x0C];
    /* 0x238 */ cXyz mGlobalParticleScale;
    /* 0x244 */ ParticleColor mGlobalPrmColor;
    /* 0x248 */ ParticleColor mGlobalEnvColor;
    /* 0x24C */ u8 mStatusWork[8];
    /* 0x254 */ be<u32> mStatus;
};
WWHD_SIZE(Emitter, 0x258);
WWHD_OFFSET(Emitter, mpCallBack, 0x1E4);
WWHD_OFFSET(Emitter, mStatus, 0x254);
WWHD_OFFSET(Emitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(Emitter, mGlobalEnvColor, 0x248);
WWHD_OFFSET(Emitter, mRate, 0x34);
WWHD_OFFSET(Emitter, mSpread, 0x58);
WWHD_OFFSET(Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Emitter, mAwayFromCenterSpeed, 0x68);
WWHD_OFFSET(Emitter, mDirectionalSpeed, 0x70);
WWHD_OFFSET(Emitter, mpLastParticle, 0x1B0);
WWHD_OFFSET(Emitter, mVolumeSweep, 0x7C);
WWHD_OFFSET(Emitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
struct FanSwingCallback {
    /* 0x00 */ u8 mWork[8];
    /* 0x08 */ gptr<Emitter> mpEmitter;
};
WWHD_SIZE(FanSwingCallback, 0xC);
WWHD_OFFSET(FanSwingCallback, mpEmitter, 8);
// HD: these input fields and expression controls are absent from the GC scope procedure.
struct Pad {
    /* 0x000 */ u8 mWork[0x18];
    /* 0x018 */ be<u32> mTrigger;
    /* 0x01C */ u8 mButtonWork[0x124 - 0x1C];
    /* 0x124 */ be<u32> mButton;
};
WWHD_SIZE(Pad, 0x128);
WWHD_OFFSET(Pad, mTrigger, 0x18);
WWHD_OFFSET(Pad, mButton, 0x124);
struct PadPointer {
    gptr<Pad> mpPad;
};
WWHD_SIZE(PadPointer, 4);
struct ShipButtons {
    /* 0x00 */ be<u8> mSailStatus[2];
    /* 0x02 */ u8 mPadding[2];
    /* 0x04 */ be<u32> mFaceButton[4];
};
WWHD_SIZE(ShipButtons, 0x14);
WWHD_OFFSET(ShipButtons, mFaceButton, 4);
struct SelfiePose {
    /* 0x00 */ be<u32> mFace;
    /* 0x04 */ be<f32> mFrame;
    /* 0x08 */ be<u8> mHoldFrame;
    /* 0x09 */ u8 mPadding[3];
};
WWHD_SIZE(SelfiePose, 0x0C);
WWHD_OFFSET(SelfiePose, mFrame, 4);
WWHD_OFFSET(SelfiePose, mHoldFrame, 8);
// HD: immutable shared expression table, defined by the creation/draw range.
// This range only views it: its standalone spec does not link the table owner.
inline const SelfiePose* selfiePose(s32 index) { return gabi::at<SelfiePose>(0x10035064 + index * sizeof(SelfiePose)); }
struct CameraAngle {
    /* 0x000 */ u8 mWork[0x236];
    /* 0x236 */ be<s16> mAngleY;
};
WWHD_SIZE(CameraAngle, 0x238);
WWHD_OFFSET(CameraAngle, mAngleY, 0x236);
inline be<u8>& sailState() { return *gabi::at<be<u8>>(gabi::ea(gameInfo()->play._4A8C)); }
struct FrameFunctor {
    /* 0x00 */ be<f32> mResult;
    /* 0x04 */ be<f32> mStart;
    /* 0x08 */ be<f32> mEnd;
    /* 0x0C */ u8 mWork[4];
    /* 0x10 */ be<u32> mFunction;
    /* 0x14 */ be<u32> mArgument;
};
WWHD_SIZE(FrameFunctor, 0x18);
WWHD_OFFSET(FrameFunctor, mStart, 4);
WWHD_OFFSET(FrameFunctor, mEnd, 8);
WWHD_OFFSET(FrameFunctor, mFunction, 0x10);
WWHD_OFFSET(FrameFunctor, mArgument, 0x14);
struct FrameBlendObject {
    /* 0x00 */ gptr<FrameFunctor> mpFunctor;
    /* 0x04 */ u8 mWork[0x54];
};
WWHD_SIZE(FrameBlendObject, 0x58);
inline FrameBlendObject& frameBlend(daPy_lk_c* player, int part) {
    return *gabi::at<FrameBlendObject>(gabi::ea(player->m644) + part * sizeof(FrameBlendObject));
}
struct AnimationVtable {
    /* 0x00 */ u8 mSlots[0x14];
    /* 0x14 */ be<u32> mGetFrameMax;
};
WWHD_SIZE(AnimationVtable, 0x18);
WWHD_OFFSET(AnimationVtable, mGetFrameMax, 0x14);
struct FrameAnimation {
    /* 0x00 */ be<f32> mFrame;
    /* 0x04 */ gptr<AnimationVtable> mpVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(mpVtable->mGetFrameMax, this); }
};
WWHD_SIZE(FrameAnimation, 8);
WWHD_OFFSET(FrameAnimation, mpVtable, 4);
struct CutBrkAnimation {
    /* 0x00 */ u8 mWork[4];
    /* 0x04 */ be<f32> mFrame;
    /* 0x08 */ u8 mFrameWork[8];
    /* 0x10 */ gptr<FrameAnimation> mpAnimation;
    /* 0x14 */ u8 mAnimationWork[4];
    /* 0x18 */ gptr<FrameFunctor> mpFunctor;
};
WWHD_SIZE(CutBrkAnimation, 0x1C);
WWHD_OFFSET(CutBrkAnimation, mFrame, 4);
WWHD_OFFSET(CutBrkAnimation, mpAnimation, 0x10);
WWHD_OFFSET(CutBrkAnimation, mpFunctor, 0x18);
struct CutBtkAnimation {
    /* 0x00 */ u8 mWork[4];
    /* 0x04 */ be<f32> mFrame;
    /* 0x08 */ u8 mFrameWork[8];
    /* 0x10 */ gptr<FrameFunctor> mpFunctor;
    /* 0x14 */ u8 mFunctorWork[0x54];
    /* 0x68 */ gptr<FrameAnimation> mpAnimation;
};
WWHD_SIZE(CutBtkAnimation, 0x6C);
WWHD_OFFSET(CutBtkAnimation, mFrame, 4);
WWHD_OFFSET(CutBtkAnimation, mpFunctor, 0x10);
WWHD_OFFSET(CutBtkAnimation, mpAnimation, 0x68);
struct PlayerCutWork {
    /* 0x0000 */ u8 mPlayerWork[0x4980];
    /* 0x4980 */ CutBrkAnimation mCutfBpk;
    /* 0x499C */ u8 mTextureWork[0x58];
    /* 0x49F4 */ CutBtkAnimation mCutfBtk;
};
WWHD_SIZE(PlayerCutWork, 0x4A60);
WWHD_OFFSET(PlayerCutWork, mCutfBpk, 0x4980);
WWHD_OFFSET(PlayerCutWork, mCutfBtk, 0x49F4);
struct PlayerRopeTurnWork {
    /* 0x0000 */ u8 mPlayerWork[0x6922];
    /* 0x6922 */ be<s16> mRopeTurnSpeed;
};
WWHD_SIZE(PlayerRopeTurnWork, 0x6924);
WWHD_OFFSET(PlayerRopeTurnWork, mRopeTurnSpeed, 0x6922);
enum {
    ANM_GRABP = 0x65,
    ANM_GRABUP,
    ANM_GRABNG,
    ANM_GRABWAIT,
    ANM_GRABWAITB,
    ANM_GRABTHROW,
    ANM_GRABRE,
    daPyProc_GRAB_REBOUND_e = 0x75,
};
// HD values of the tww action, animation and item identifiers used by this range.
enum : u32 {
    ANM_BINDRINKAFTER = 0xB8,
    ANM_BINDRINKING = 0xB7,
    ANM_BINGET = 0xBE,
    ANM_BINOPENA = 0xBA,
    ANM_BINOPENB = 0xBB,
    ANM_CUTREL = 0x15,
    ANM_CUTRER = 0x27,
    ANM_HAMSWINGBEND = 0xAD,
    ANM_HAMSWINGBHIT = 0xAC,
    ANM_JATTACK = 0x2B,
    ANM_LANDDAMAST = 0x31,
    ANM_ROPECLIMB = 0x79,
    ANM_ROPEDOWN = 0x7A,
    ANM_ROPESWINGB = 0x77,
    ANM_ROPESWINGF = 0x76,
    ANM_USEFANB = 0xA2,
    ANM_USEFANB2 = 0xA3,
    ANM_WAITPUSHPULL = 0x7E,
    ANM_WAITTAKT = 0xA8,
    ANM_WALKPULL = 0x80,
    ANM_WALKPUSH = 0x7F,
    daPyProc_BOTTLE_GET_e = 0xA6,
    daPyProc_CUT_REVERSE_e = 0x5A,
    daPyProc_HAMMER_FRONT_SWING_END_e = 0x54,
    daPyProc_HAMMER_FRONT_SWING_e = 0x53,
    daPyProc_JUMP_CUT_e = 0x5B,
    daPyProc_PULL_MOVE_e = 0x34,
    daPyProc_PUSH_MOVE_e = 0x33,
    daPyProc_ROPE_DOWN_e = 0x7B,
    daPyProc_ROPE_SWING_START_e = 0x7C,
    daPyProc_ROPE_UP_e = 0x7A,
    daPyProc_TACT_PLAY_ORIGINAL_e = 0x9D,
    daPyProc_TACT_PLAY_e = 0x9B,
    daPyProc_WEAPON_FRONT_SWING_e = 0x4E,

    daPyItem_BOKO_e = 0x101,
    daPyItem_NONE_e = 0x100,
    dItemNo_BOW_e = 0x27,
    daPyStts0_UNK2_e = 0x2,
    daPyStts0_UNK400_e = 0x400,
    daPyStts0_BOW_AIM_e = 0x1000,
    daPyStts0_BOOMERANG_AIM_e = 0x80000,
    daPyStts0_HOOKSHOT_AIM_e = 0x4000,
    ID_AK_JT_ELEMENTSMOKE00 = 0x2022,
    ANM_ATNLS = 0x7,
    ANM_ATNRS = 0x8,
    ANM_DASH = 0x2,
    ANM_FREEA = 0xE1,
    ANM_FREEB = 0xE2,
    ANM_FREED = 0xE3,
    ANM_JMPEDS = 0xD,
    ANM_JMPST = 0x3C,
    ANM_MJMPC = 0x6D,
    ANM_MROLLLC = 0x70,
    ANM_MROLLRC = 0x71,
    ANM_MSTEPOVERA = 0x73,
    ANM_MSTEPOVERLAND = 0x74,
    ANM_ROPECATCH = 0x75,
    ANM_ROPEWAIT = 0x78,
    ANM_SALVLBAD = 0xD0,
    ANM_SALVLGOOD = 0xD2,
    ANM_SALVLR = 0xCC,
    ANM_SALVLWAIT = 0xCE,
    ANM_SALVRBAD = 0xCF,
    ANM_SALVRGOOD = 0xD1,
    ANM_SALVRWAIT = 0xCD,
    ANM_VJMP = 0x4C,
    ANM_WAITS = 0x0,
    ANM_WALK = 0x1,
    DIR_BACKWARD = 0x1,
    DIR_LEFT = 0x2,
    DIR_RIGHT = 0x3,
    JA_SE_INDICATOR_1 = 0x820,
    JA_SE_ITM_SUBMENU_IN_1 = 0x822,
    JA_SE_LK_BARREL_PUT_ON = 0x2829,
    JA_SE_LK_JUMP_ATTACK = 0x283C,
    JA_SE_LK_SWIM = 0x3807,
    JA_SE_LK_SW_KAZEKIRI_S = 0x2800,
    JA_SE_LK_WALK_IN_WATER = 0x2840,
    UPPER_MOVE2_e = 0x2,
    dActStts_CRUISE_e = 0x2C,
    dActStts_GET_IN_SHIP_e = 0x1C,
    dActStts_LET_GO_ROPE_e = 0x16,
    dActStts_RETURN_e = 0x7,
    dActStts_STOP_e = 0x13,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_BOOMERANG_e = 0x2D,
    dItemNo_EMPTY_BOTTLE_e = 0x50,
    dItemNo_FAIRY_BOTTLE_e = 0x57,
    dItemNo_GRAPPLING_HOOK_e = 0x25,
    dItemNo_HOOKSHOT_e = 0x2F,
    dItemNo_HYOI_PEAR_e = 0x83,
    dItemNo_MAGIC_ARMOR_e = 0x2A,
    dItemNo_SAIL_e = 0x78,
    dItemNo_TELESCOPE_e = 0x20,
    dItemNo_TINGLE_TUNER_e = 0x21,
    dItemNo_WIND_WAKER_e = 0x22,
    daPyItem_BOW_e = 0x108,
    daPyItem_DRINK_BOTTLE_e = 0x105,
    daPyItem_ESA_e = 0x107,
    daPyItem_OPEN_BOTTLE_e = 0x106,
    daPyItem_PHOTOBOX_e = 0x109,
    daPyProc_BT_JUMP_CUT_e = 0x5E,
    daPyProc_BT_ROLL_CUT_e = 0x61,
    daPyProc_BT_VERTICAL_JUMP_CUT_e = 0x63,
    daPyProc_BT_VERTICAL_JUMP_LAND_e = 0x64,
    daPyProc_ROPE_HANG_WAIT_e = 0x79,
    daPyProc_ROPE_UP_HANG_e = 0x7F,
    daPyProc_SHIP_BOOMERANG_e = 0x8B,
    daPyProc_SHIP_BOW_e = 0x8D,
    daPyProc_SHIP_GET_OFF_e = 0x90,
    daPyProc_SHIP_HOOKSHOT_e = 0x8C,
    daPyProc_SHIP_PADDLE_e = 0x89,
    daPyProc_SHIP_SCOPE_e = 0x8A,
    daPyProc_SWIM_MOVE_e = 0x37,
    daPyProc_SWIM_UP_e = 0x35,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    daPyStts0_SUBJECT_e = 0x2000,
    daPyStts0_TELESCOPE_LOOK_e = 0x200000,
    daPyStts0_UNK10_e = 0x10,
    daPyStts0_UNK800000_e = 0x800000,
    daPyStts1_PICTO_BOX_AIM_e = 0x8,
    fpcNm_BOKO_e = 0x1CF,
    fpcNm_BOMB_e = 0x126,
    fpcNm_Bomb2_e = 0x127,
};
} // namespace Link07

void Link07_clearActorKeep(daPy_actorKeep_l*);
u32 Link07_getJointTree(void*);
s32 Link07_getJointCount(u32, s32);

// HD: these bindings preserve the return precision and register widths used here.
u32 Link07_constructAngle(be<s16>*, s16);
s16 Link07_invertAngle(u32);
BOOL Link07_holdSpecialPadButton(s32);
s16 Link07_atan2sUnrounded(f64, f32);
f64 Link07_atan2Unrounded(f32, f64);
f64 Link07_getBoomerangFlyMax(u32);
BOOL Link07_setRopeTargetPos(u32, cXyz*, be<f32>*, be<f32>*);
u32 Link07_getPolyId2(dBgS*, u32);
f64 Link07_roofHeight(dBgS*, void*);
void Link07_forceCameraLock(u32, u32);
BOOL Link07_checkAttackHit(u32);
u8 Link07_getSalvageKind(u32, u32);
void Link07_startShock(u32, s32, s32, cXyz*);
BOOL Link07_orderTalkEvent(daPy_lk_c*, fopAc_ac_c*);
fopAc_ac_c* Link07_getTalkPartner(daPy_lk_c*);
void Link07_seStartSystem(u32);
void Link07_changeSwordBck(u32, u32);
void Link07_updateFrameFunctor(u32);
f64 Link07_sqrtUnrounded(f32);

s32 Link07_seenActorAngleY(daPy_lk_c*, u32);
BOOL Link07_checkScopeEnd(daPy_lk_c*);
void Link07_setShipBStatus();

f32 Link07_squareDistance(const cXyz*, const cXyz*);
void Link07_orderEvent(u32, s32, s32, s32, s32, daPy_lk_c*, u32, s32, s32);

// HD: exact call-site register widths, including unrounded beat-frame results.
void Link07_tactSetVolume(f32);
void Link07_tactReset();
void Link07_tactAmbientPlay();
void Link07_tactMelodyPlay(s32);
void Link07_taktModeMuteOff();
void Link07_tactSetStickPos(s32, s32);
void Link07_tactPlayArmSwing(s32, s32);
void Link07_taktModeMute();
void Link07_tactStopArmSwing();
void Link07_tactPlay();
void Link07_tactSetBeat(s32);
void Link07_tactArmSoundPlay(s32);
void Link07_tactMetronomePlay(s32, s32);
s32 Link07_tactJudge(s32, s32);
s32 Link07_tactFreeMStickWord(u32, s32);
s32 Link07_tactFreeCStickWord(u32, s32);
s32 Link07_cursorStateWord(u32);
u32 Link07_tactGetBeatWord();
s32 Link07_tactGetBeatSignedWord();
f64 Link07_tactGetBeatFramesUnrounded();

// HD: retain FPR precision and sign-extended angle arguments at these call sites.
f64 Link07_ropeRoofUnrounded(daPy_lk_c*, s32);
f64 Link07_groundCrossUnrounded(dBgS*, void*);
f64 Link07_squareDistanceUnrounded(const cXyz*, const cXyz*);
f64 Link07_rndFXUnrounded(f32);
f64 Link07_rndUnrounded();
s16 Link07_atan2sSecondUnrounded(f32, f64);
f64 Link07_sqrtDoubleArgument(f64);

// HD: keep full-word attribute results and sound arguments used by these routines.
s32 Link07_getAttributeCodeWord(dBgS*, u32);
BOOL Link07_checkMoveBG(dBgS*, u32);
BOOL Link07_getWaterY(const cXyz*, be<f32>*);
BOOL Link07_checkGustWind();
BOOL Link07_checkDayNightTactStop();
void Link07_setSoundPosition(const cXyz*, s32, u32, s32);
void Link07_seStartMaterial(u32, const cXyz*, u32, s32);

// HD: event and message handles retain their guest-word representation.
s32 Link07_messageSet(u32, s32, s32);
u32 Link07_cameraBody();
void Link07_endEventCamera(u32, u32);
u32 Link07_roomStatus(u32, s32);
void Link07_saveTactSongResult(u32, u32);
u32 Link07_messageStatus(u32);
void Link07_setMessageStatus(u32, s32);
BOOL Link07_eventStartCheckOld(u32, u32);
BOOL Link07_eventEndCheckOld(u32, u32);
void Link07_deleteTimerRequest(u32);
void Link07_turnRestart();
void Link07_changePlayer(daPy_lk_c*, u32);
void Link07_deleteActorByID(u32);
u32 Link07_catchTargetFromID(u32, u32);
s32 Link07_tactDirection(s32, s32);
void Link07_initOldFrameMorf(u32, f32, s32, s32);
f64 Link07_addCalcUnrounded(be<f32>*, f32, f32, f32, f32);

// HD: particle setup preserves the original GPR/FPR argument groups.
u32 Link07_setSimpleLandHD(dPa_control_c*, s32, const cXyz*, s32, f32, f32, f32, u32, be<s32>*, u32);
void Link07_setParticleMatrix(const Mtx34*, u32, u32);
void Link07_setSquall(const cXyz*, s32, s32, f32, f32, f32, f32, f32);
void Link07_setEquipBottleItemIn(u32, u32, u32);
void Link07_setEquipBottleItemEmpty(u32, u32);
void Link07_resetItemRecordTimer(u32, u32);
void Link07_orderCatchEvent(daPy_lk_c*, u32);
void Link07_createKyActor(s32, s32, const cXyz*, s32, s32);
void Link07_getAllWindVector(u32, cXyz*, be<f32>*);
void Link07_setParachuteAnimation(u32, u32, s32, f32, f32, f32, f32, s32);

// HD: these adapters preserve full-word angles, IDs and the bottle-camera argument list.
void Link07_setTurnRestart(u32, const cXyz*, s32, s32, u32, const cXyz*, s32, s32);
u32 Link07_pushPullCallback(u32, u32, daPy_lk_c*, s32, s32);
void Link07_startBottleEventCamera(u32, s32, u32, u32, be<s32>*, s32);
s16 Link07_rad2s(f32);
BOOL Link07_chaseAngle(be<s16>*, s16, s16);
void Link07_rotateZ(Mtx34*, s16);
void Link07_rotateZXY(Mtx34*, s16, s16, s16);
u32 Link07_materialSoundWord(dBgS*, u32);
s32 Link07_materialSoundSignedWord(dBgS*, u32);

// HD: ABI-sensitive Link calls keep the original full-width angles and FPR values.
void Link07_setSpeedAndAngleAtnActorHD(daPy_lk_c*);
BOOL Link07_checkSightLineUnrounded(daPy_lk_c*, f64, cXyz*);
s16 Link07_getGroundAngleWord(daPy_lk_c*, u32, s32);
BOOL Link07_initRopeSwingWord(daPy_lk_c*, u32, s32);
void Link07_endMtxFollow(daPy_mtxFollowEcallBack_c*);
void Link07_makeMtxFollowEmitter(daPy_mtxFollowEcallBack_c*, u32, u32, const cXyz*, const csXyz*);
void Link07_makeMtxFollowEmitterColor(daPy_mtxFollowEcallBack_c*, u32, u32, const cXyz*, u32, u32);
void Link07_makeMtxPosFollowEmitterColor(void*, u32, u32, const cXyz*, const csXyz*, u32, s32);
void Link07_setAttackParamWord(daPy_lk_c*, u32, s32, s32, u32, u32, u32, f32);
u32 Link07_makeFairyWord(daPy_lk_c*, u32, s32);
void Link07_setActorKeepWord(daPy_actorKeep_l*, u32);
BOOL Link07_checkDaylightPresetHD();
