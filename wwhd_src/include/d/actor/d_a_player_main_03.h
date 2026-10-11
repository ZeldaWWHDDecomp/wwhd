/* Link action initialization and input/demo handling, WWHD range 03.
 *
 * The canonical daPy_lk_c layout is shared by every Link range. These private views expose
 * the HD animation blend records, equipment animation and message-bottle tail used here.
 * GameCube animation ratio packs grew to 0x10; the message-bottle state has no GameCube counterpart.
 */
#pragma once
#include "d/actor/d_a_player_main.h"
#include "gc_api/d/d_com_inf_game.h"

struct LinkActionAnmHeap {
    /* 0x00 */ be<u16> mIdx;
    /* 0x02 */ be<u16> field_0x2;
    /* 0x04 */ be<s16> field_0x4;
    /* 0x06 */ be<s16> field_0x6;
    /* 0x08 */ gptr<void> m_buffer;
    /* 0x0C */ gptr<JKRSolidHeap> mpAnimeHeap;
};
WWHD_SIZE(LinkActionAnmHeap, 0x10);

struct LinkActionAnmRatio {
    /* 0x00 */ be<f32> mRatio;
    /* 0x04 */ gptr<be<f32>> mpAnmFrame;
    /* 0x08 */ be<s32> mJointCount;
    /* 0x0C */ gptr<be<f32>> mpJointWeights;
};
WWHD_SIZE(LinkActionAnmRatio, 0x10);

struct LinkActionTexture {
    be<u32> mWords[9];
};
WWHD_SIZE(LinkActionTexture, 0x24);
struct LinkActionLayout {
    /* 0x0000 */ u8 _0000[0x48C];
    /* 0x048C */ LinkActionTexture mTextures[10];
    /* 0x05F4 */ u8 _05F4[0xB14 - 0x5F4];
    /* 0x0B14 */ u8 _gndCheck[0x24];
    /* 0x0B38 */ cXyz mGndChkPos;
    /* 0x0B44 */ u8 _0B44[0x4440 - 0xB44];
    /* 0x4440 */ gptr<J3DModel> mpEquipItemModel;
    /* 0x4444 */ mDoExt_bckAnm mSwordAnim;
    /* 0x44D0 */ u8 _44D0[0x57F8 - 0x44D0];
    /* 0x57F8 */ LinkActionAnmRatio mAnmRatioUnder[2];
    /* 0x5818 */ LinkActionAnmRatio mAnmRatioUpper[3];
    /* 0x5848 */ LinkActionAnmHeap m_anm_heap_under[2];
    /* 0x5868 */ LinkActionAnmHeap m_anm_heap_upper[3];
    /* 0x5898 */ u8 _5898[0x8260 - 0x5898];
    // HD: message-bottle input blocking and camera-follow state.
    /* 0x8260 */ be<u32> mItemButtonsBlocked;
    /* 0x8264 */ be<u8> mTingleDemoOrdered;
    /* 0x8265 */ be<u8> mBottleThrowPending;
    /* 0x8266 */ u8 _8266[2];
    /* 0x8268 */ be<u32> mThrowActor;
    /* 0x826C */ be<s16> mThrowAngle;
    /* 0x826E */ u8 _826E[2];
    /* 0x8270 */ cXyz mThrowPos;
    /* 0x827C */ u8 _827C[8];
};
WWHD_SIZE(LinkActionLayout, 0x8284);
WWHD_OFFSET(LinkActionLayout, mpEquipItemModel, 0x4440);
WWHD_OFFSET(LinkActionLayout, mAnmRatioUnder, 0x57F8);
WWHD_OFFSET(LinkActionLayout, m_anm_heap_upper, 0x5868);
WWHD_OFFSET(LinkActionLayout, mItemButtonsBlocked, 0x8260);

inline LinkActionLayout& LinkAction(daPy_lk_c* player) {
    return gabi::object_at<LinkActionLayout>(gabi::ea(player));
}

// HD: only the ship controls read by Link's boarding actions are exposed here.
struct LinkActionShip {
    u8 _000[0x636];
    /* 0x636 */ be<u8> mMode;
    /* 0x637 */ be<u8> mPart;
    u8 _638[0x682 - 0x638];
    /* 0x682 */ be<s16> mCraneBaseAngle;
    u8 _684[2];
    /* 0x686 */ be<s16> mRopeCnt;
};
WWHD_SIZE(LinkActionShip, 0x688);
WWHD_OFFSET(LinkActionShip, mMode, 0x636);
WWHD_OFFSET(LinkActionShip, mCraneBaseAngle, 0x682);

struct LinkActionThrownItem {
    u8 _000[0x787];
    /* 0x787 */ be<u8> mMode;
};
WWHD_SIZE(LinkActionThrownItem, 0x788);

// These copies deliberately use words: GHS does not quiet signaling NaNs while moving positions.
struct LinkActionActorWords {
    u8 _000[0x300];
    /* 0x300 */ be<u32> mOldPos[3];
    u8 _30C[8];
    /* 0x314 */ be<u32> mCurrentPos[3];
};
WWHD_SIZE(LinkActionActorWords, 0x320);
WWHD_OFFSET(LinkActionActorWords, mCurrentPos, 0x314);

struct LinkActionStageInfo {
    u8 _00[0xC];
    /* 0x0C */ be<u32> mStageType;
};
WWHD_SIZE(LinkActionStageInfo, 0x10);

struct LinkActionAttention {
    u8 _00[0x20];
    /* 0x20 */ be<u32> mFlags;
};
WWHD_SIZE(LinkActionAttention, 0x24);

// HD: this collision path tests the halfword at +0xE, separately from the process name at +8.
struct LinkActionDamageActor {
    u8 _00[0xE];
    /* 0x0E */ be<s16> mDamageActorType;
};
WWHD_SIZE(LinkActionDamageActor, 0x10);

// Values verified for the HD action range; names follow the GameCube enums.
namespace LinkActionConstants {
enum : u32 {
    daPyProc_HAMMER_SIDE_SWING_e = 0x51,
    ANM_HAMSWINGA = 0xAA,
    dRes_INDEX_LKANM__HAMMERSIDE_POS_e = 0x289,
    dRes_INDEX_LKANM_BCK_HAMSWINGAA_e = 0x99,
    daPyProc_HAMMER_FRONT_SWING_READY_e = 0x52,
    ANM_HAMSWINGBPRE = 0xAB,
    dRes_INDEX_LKANM_BCK_HAMSWINGBPREA_e = 0x9E,
    daPyProc_CALL_e = 0x2,
    ANM_YOBU = 0xC2,
    daPyItem_BOKO_e = 0x101,
    daPyProc_CROUCH_e = 0xE,
    ANM_CROUCH = 0x3F,
    daPyProc_CROUCH_DEFENSE_e = 0xC,
    ANM_DIFENCE = 0x16,
    daPyStts1_UNK80000_e = 0x80000,
    daPyProc_SIDE_STEP_e = 0xA,
    daPyProc_BACK_JUMP_e = 0x22,
    ANM_ROLLB = 0x3D,
    daPyProc_FRONT_ROLL_e = 0x1E,
    ANM_ROLLF = 0x32,
    daPyProc_SUBJECTIVITY_e = 0x1,
    dItemNo_HYOI_PEAR_e = 0x83,
    dRes_INDEX_LINK_BDL_HYOINOMI_e = 0x1F,
    daPyProc_FOOD_SET_e = 0xA8,
    ANM_SETHYOINOMI = 0xC5,
    daPyProc_FOOD_THROW_e = 0xA7,
    JA_SE_ITEM_TARGET_OUT = 0x883,
    ANM_ESAMAKI = 0xC4,
    dItemNo_SOUP_BOTTLE_e = 0x55,
    dItemNo_HALF_SOUP_BOTTLE_e = 0x54,
    daPyProc_BOTTLE_DRINK_e = 0xA3,
    ANM_BINDRINKPRE = 0xB6,
    daPyStts1_UNK2000_e = 0x2000,
    daPyProc_BOTTLE_OPEN_e = 0xA4,
    ANM_BINOPENPRE = 0xB9,
    daPyStts1_UNK4000_e = 0x4000,
    dItemNo_FAIRY_BOTTLE_e = 0x57,
    daPyProc_NOT_USE_e = 0xA2,
    ANM_PRESENTATIONB = 0xB5,
    daPyStts1_UNK800_e = 0x800,
    dItemNo_BOOMERANG_e = 0x2D,
    dItemNo_MAGIC_ARMOR_e = 0x2A,
    dItemNo_TINGLE_TUNER_e = 0x21,
    dItemNo_IRON_BOOTS_e = 0x29,
    dItemNo_SAIL_e = 0x78,
    dItemNo_EMPTY_BOTTLE_e = 0x50,
    dItemNo_NONE_e = 0xFF,
    dItemNo_BOMB_BAG_e = 0x31,
    daPyItem_SWORD_e = 0x103,
    daPyFlg1_EQUIP_DRAGON_SHIELD = 0x1,
    daPyFlg0_EQUIP_HEAVY_BOOTS = 0x2000000,
    daPyItem_DRINK_BOTTLE_e = 0x105,
    daPyRFlg0_UNK80 = 0x80,
    dItemNo_TELESCOPE_e = 0x20,
    dItemNo_DEKU_LEAF_e = 0x34,
    dItemNo_HOOKSHOT_e = 0x2F,
    dItemNo_GRAPPLING_HOOK_e = 0x25,
    dItemNo_WIND_WAKER_e = 0x22,
    daPyFlg0_UNK4 = 0x4,
    dItemNo_SKULL_HAMMER_e = 0x33,
    daPyFlg1_NPC_CALL_COMMAND = 0x2,
    daPyRFlg0_SUBJECT_ACCEPT = 0x4000000,
    fpcNm_BOOMERANG_e = 0x1B0,
    daPyProc_ATN_ACTOR_WAIT_e = 0x8,
    daPyProc_ATN_ACTOR_MOVE_e = 0x9,
    daPyProc_ATN_MOVE_e = 0x7,
    daPyProc_WAIT_TURN_e = 0x17,
    ANM_ROT = 0x14,
    daPyProc_MOVE_TURN_e = 0x18,
    daPyProc_SLIP_e = 0x19,
    ANM_SLIP = 0x34,
    JA_SE_LK_RUN_SLIP = 0x280D,
    daPyProc_MOVE_e = 0x6,
    daPyProc_VOMIT_WAIT_e = 0x97,
    ANM_VOMITJMP = 0xA7,
    daPyProc_VOMIT_READY_e = 0x96,
    ANM_MSTEPOVER = 0x72,
    fpcNm_JBO_e = 0xD5,
    daPyFlg1_FORCE_VOMIT_JUMP = 0x10,
    daPyFlg1_FORCE_VOMIT_JUMP_SHORT = 0x10000,
    dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e = 0xF4,
    ANM_VOYAGE1 = 0x7D,
    dRes_INDEX_LKANM_BCK_FN_HAIR1_e = 0x89,
    daPyProc_SHIP_CANNON_e = 0x8E,
    daPyStts1_UNK4_e = 0x4,
    daPyProc_SHIP_CRANE_e = 0x8F,
    daPyStts1_UNK2_e = 0x2,
    daPyProc_SHIP_STEER_e = 0x88,
    daPyProc_SHIP_PADDLE_e = 0x89,
    daPyProc_LARGE_DAMAGE_e = 0x68,
    daPyItem_UNK104_e = 0x104,
    daPyItem_UNK10A_e = 0x10A,
    daPyProc_DEMO_BOSS_WARP_e = 0xC9,
    ANM_WAITS = 0x0,
    daPyProc_DEMO_TOOL_e = 0xA9,
    ANM_WALK = 0x1,
    daPyProc_DEMO_TALK_e = 0xAA,
    ANM_TALKA = 0x1B,
    JA_SE_TALK_START = 0x80A,
    JA_SE_LK_MGC_SHIELD_DEF = 0x2886,
    daPyRFlg0_UNK80000 = 0x80000,
    CL_JNT_CENTER_e = 0x1,
    ID_AK_SN_BKMHAKIDASHIHOUSHI00 = 0x80F6,
    JA_SE_LK_DAMAGE_LARGE = 0x282E,
    daPyFlg1_FREEZE_STATE = 0x800,
    daPyFlg1_UNK40000 = 0x40000,
    JA_SE_LK_FREEZE = 0x2878,
    ID_IT_SN_LK_FREEZSMOKE00 = 0x827E,
    ID_IT_SN_LK_FREEZKIRA00 = 0x827F,
    daPyProc_LARGE_DAMAGE_UP_e = 0x69,
    JA_SE_LK_FALL_DOWN = 0x3811,
    ANM_DAMFB = 0x5C,
    ANM_DAMFL = 0x59,
    ANM_DAMFR = 0x5A,
    ANM_DAMFLUP = 0x5D,
    ANM_DAMFRUP = 0x5E,
    daPyProc_SHIP_GET_OFF_e = 0x90,
    daPyProc_FALL_e = 0x27,
    ANM_JMPEDS = 0xD,
    daPyProc_SLOW_FALL_e = 0x28,
    ANM_MSTEPOVER_JMPED = 0xD3,
    daPyProc_DEMO_LOOK_WAIT_e = 0xBE,
    daPyProc_GRAB_WAIT_e = 0x73,
    daPyFlg1_SHIP_TACT = 0x1000,
    daPyProc_BACK_JUMP_LAND_e = 0x23,
    daPyProc_LAND_e = 0x25,
};
}

// HD input bindings retain the verified return register widths.
f32 CPad_GET_STICK_VALUE(u32 pad);
s16 CPad_GET_STICK_ANGLE(u32 pad);
BOOL CPad_CHECK_TRIG_B(u32 pad);
BOOL CPad_CHECK_TRIG_A(u32 pad);
BOOL CPad_CHECK_TRIG_X(u32 pad);
BOOL CPad_CHECK_TRIG_Y(u32 pad);
BOOL CPad_CHECK_TRIG_Z(u32 pad);
BOOL CPad_CHECK_TRIG_L(u32 pad);
BOOL CPad_CHECK_R_LOCK_TRIG(u32 pad);
BOOL CPad_CHECK_HOLD_A(u32 pad);
BOOL CPad_CHECK_HOLD_B(u32 pad);
BOOL CPad_CHECK_HOLD_X(u32 pad);
BOOL CPad_CHECK_HOLD_Y(u32 pad);
BOOL CPad_CHECK_HOLD_Z(u32 pad);
BOOL CPad_CHECK_HOLD_L(u32 pad);
BOOL CPad_CHECK_R_LOCK_BUTTON(u32 pad);

inline dComIfG_play_c& LinkActionPlay(u32 info) {
    return gabi::object_at<dComIfG_inf_c>(info).play;
}
inline dComIfG_play_c& LinkActionPlay() { return LinkActionPlay(dComIfGp_ea()); }
inline dSv_info_c& LinkActionSave() { return gabi::object_at<dSv_info_c>(dComIfGs_ea() + 0x20); }

struct LinkActionBlendRatio {
    /* 0x00 */ gptr<be<f32>> mpJointWeights;
    u8 _04[0xC];
};
WWHD_SIZE(LinkActionBlendRatio, 0x10);
struct LinkActionBlendData {
    u8 _00[0xC];
    /* 0x0C */ LinkActionBlendRatio mRatios[3];
};
WWHD_SIZE(LinkActionBlendData, 0x3C);
struct LinkActionBlendCalc {
    u8 _00[0x7C];
    /* 0x7C */ gptr<LinkActionBlendData> mpBlendData;
    /* 0x80 */ gptr<J3DModel> mpModel;
};
WWHD_SIZE(LinkActionBlendCalc, 0x84);
struct LinkActionVectorWords {
    be<u32> x;
    be<u32> y;
    be<u32> z;
};
WWHD_SIZE(LinkActionVectorWords, 0xC);

struct LinkActionDemoActor {
    u8 _00[4];
    /* 0x04 */ be<u16> mEnable;
    u8 _06[2];
    /* 0x08 */ cXyz mTranslation;
    u8 _14[0xC];
    /* 0x20 */ csXyz mRotation;
    u8 _26[2];
    /* 0x28 */ be<s32> mShape;
    /* 0x2C */ be<s32> mAnimation;
    u8 _30[0x18];
    /* 0x48 */ gptr<J3DModel> mpModel;
};
WWHD_SIZE(LinkActionDemoActor, 0x4C);
WWHD_OFFSET(LinkActionDemoActor, mRotation, 0x20);
WWHD_OFFSET(LinkActionDemoActor, mpModel, 0x48);
struct LinkActionTextureList {
    /* 0x00 */ be<u32> mCount;
    u8 _04[4];
    /* 0x08 */ gptr<gptr<LinkActionTexture>> mpEntries;
};
WWHD_SIZE(LinkActionTextureList, 0xC);

WWHD_OFFSET(LinkActionLayout, mTextures, 0x48C);
WWHD_OFFSET(LinkActionTextureList, mpEntries, 8);

struct LinkActionVtable {
    u8 _00[0x3C];
    /* 0x03C */ gabi::gfn mCheckPlayerGuard;
    u8 _040[0xE4 - 0x40];
    /* 0x0E4 */ gabi::gfn mVoiceStart;
    u8 _0E8[0x114 - 0xE8];
    /* 0x114 */ gabi::gfn mSetPlayerPosAndAngle;
    u8 _118[0x134 - 0x118];
    /* 0x134 */ gabi::gfn mChangeTextureAnime;
};
WWHD_SIZE(LinkActionVtable, 0x138);
WWHD_OFFSET(LinkActionVtable, mVoiceStart, 0xE4);
WWHD_OFFSET(LinkActionVtable, mSetPlayerPosAndAngle, 0x114);
inline LinkActionVtable* LinkActionVirtuals(daPy_lk_c* player) {
    return gabi::at<LinkActionVtable>(player->__vtbl);
}
inline BOOL LinkActionCheckPlayerGuard(daPy_lk_c* player) {
    return gabi::call_ptr<BOOL>(LinkActionVirtuals(player)->mCheckPlayerGuard.get(), player);
}
inline void LinkActionVoiceStart(daPy_lk_c* player, u32 voice) {
    gabi::call_ptr(LinkActionVirtuals(player)->mVoiceStart.get(), player, voice);
}
inline void LinkActionSetPlayerPosAndAngle(daPy_lk_c* player, u32 position, s16 angle) {
    gabi::call_ptr(LinkActionVirtuals(player)->mSetPlayerPosAndAngle.get(), player, position,
                   (s32)angle);
}
inline void LinkActionChangeTextureAnime(daPy_lk_c* player, s32 btp, s32 btk, s32 frame) {
    gabi::call_ptr(LinkActionVirtuals(player)->mChangeTextureAnime.get(), player, btp, btk, frame);
}
struct LinkActionBoomerang {
    u8 _00000[0x26274];
    /* 0x26274 */ be<u8> mCancel;
};
WWHD_SIZE(LinkActionBoomerang, 0x26275);
struct LinkActionAttentionEntry {
    u8 _00[8];
    /* 0x08 */ be<s32> mType;
};
WWHD_SIZE(LinkActionAttentionEntry, 0xC);
inline u32 LinkActionDemoObject() { return *gabi::at<be<u32>>(0x101D5FFC); }

struct LinkActionApi {
    static void makeEmitter(void*, u32, u32, cXyz*, void*);
    static u32 resolveJointTree(u32);
    static s32 countJoints(u32, s32);
    static void assertionFailure(u32, u32, u32);
    static void fadeOut(u32, f32);
    static void setCarryNow(u32, s32);
    static void startEventCameraMode(u32, u32, u32, u32, u32, u32);
    static void createItem(cXyz*, s32, s32, s32, s32, u32, u32, u32);
    static void setLetterWritingMode(u32, s32);
    static u32 getLetterWriter();
    static void setParticle(u32, u32, u32, cXyz*, u32, u32, u32, u32, s32, u32, u32, u32);

    static f64 groundCross(void*, void*);
    static s32 distanceAngleS(s32, s32);
    static s32 atan2sWide(f32, f32);
    static void changePlayer(void*, void*);
    static void actorKeepSetData(void*, u32);
    static BOOL procVomitReadyWide(void*, s32, f64);
    static void shipInitStartPos(u32, cXyz*, s32);
    static u32 convCatchPId(u32, u32);
    static u32 getCameraBody();
    static void forceCameraLockOff(u32, u32);
    static void stopCamera(u32);
    static void startCamera(u32);
    static void setCamera(u32, cXyz*, cXyz*, f32, s32);
    static void resetCamera(u32);
    static void forceCameraLockOn(u32, u32);
    static u32 getCollisionActor(u32);
    static BOOL checkGetItemNum(u32);
    static u32 getDemoActor(u32, u32);
    static void endEventCamera(u32, u32);
    static BOOL orderEvent(u32, u32, u32, u32, u32, void*, u32, s32, u32);
    static BOOL startCheckOld(u32, u32);
    static u32 getMyNowCutName(u32, s32);
    static u32 getEventGoal(u32);
    static BOOL checkStartDemo(u32);
    static BOOL checkItemGet(u32, s32);
    static BOOL isDaizaItem(u32);
    static BOOL isEmono(u32);
    static BOOL isEsa(u32);
    static void setEquipBottleItemEmpty(u32, int);
    static s32 searchActorAngleY(void*, u32);
    static f64 searchActorDistanceXZ(void*, u32);
    static void orderZHintEvent(void*, u32);
    static void orderDoorEvent(void*, u32);
    static void orderTreasureEvent(void*, u32);
    static u32 getTalkEventPartner(void*);
    static u32 fastCreateItem2(cXyz*, s32, s32, s32, u32, u32, u32, u32);
    static s32 isOverlapPeek();
    static void seStartSystem(u32);
    static void taktModeMuteOff();
    static void setCurrentHeap(u32);
    static void initOldFrameMorf(u32, f32, s32, s32);
    static BOOL isHeroMode(u32);
    static u32 getHeroModeManager(u32);
    static void copyTextureList(u32, u32);
    static f64 squareMagnitude(cXyz*);
    static void constructArray(void*, u32, u32, u32);
    static f64 sqrtWide(f64);
};

struct LinkDemoEntry {
    be<s16> d;
    be<s16> i;
    be<u32> f;
};

struct LinkActionSafeString {
    be<u32> mStr;
    be<u32> __vtbl;
};

struct LinkActionTextureArray {
    LinkActionTexture mTextures[10];
};

using LinkDemoProc = BOOL (daPy_lk_c::*)();

WWHD_SIZE(LinkDemoEntry, 8);
WWHD_SIZE(LinkActionSafeString, 8);
WWHD_SIZE(LinkActionTextureArray, 0x168);

struct LinkActionCatch {
    u8 _00[8];
    /* 0x08 */ be<u32> mPId;
};
WWHD_SIZE(LinkActionCatch, 0xC);
inline LinkActionCatch* LinkActionCatchInfo(u32 info) {
    return gabi::at<LinkActionCatch>(info + 0x5928);
}
// HD: these actions reload the mutable matrix-stack pointer, rather than using the fixed stack.
inline Mtx34* LinkActionMatrixStack() { return gabi::at<gptr<Mtx34>>(0x1018C7B0)->get(); }

// HD: the motion/damage calls preserve the original argument count and return width.
struct LinkMotionApi {
    static fopAc_ac_c *fastCreate(u32, u32, cXyz *, s32, csXyz *, void *, s32, u32, u32);
    static u32 createAppend(u32, u32, u32, u32, u32, u32);
    static void bgmStop(u32);
    static void bgmStart(u32);
    static BOOL checkBottle(u32, u32);
    static void setBottleItemIn(u32, u32, u32);
    static u32 getTriPla(dBgS *, u32, u32);
    static s32 getWallCode(dBgS *, u32);
    static u32 getMySubstanceP(u32, s32, u32, s32);
    static s32 rad2s(f32);
    static void createWpillar(cXyz *, f32, f32, u32);
    static BOOL chkPolySafe(dBgS *, u32);
    static s32 getAttributeCode(dBgS *, u32);
    static u32 getTgHitGObj(dCcD_GObjInf *);
    static BOOL chkTgHit(dCcD_GObjInf *);
    static BOOL chkAtHit(dCcD_GObjInf *);
    static u32 getAtHitGObj(dCcD_GObjInf *);
    static f64 randomWide();
    static u32 getTgHitObjSe(dCcD_GObjInf *);
    static void clearTg(dCcD_Stts *);
    static void clearAt(dCcD_Stts *);
    static void setAimActor(u32, u32);
    static void clearActorKeep(void *);
    static BOOL checkPass(J3DFrameCtrl *, f32);
    static void updateJointFlags(u32, s32);
    static void updateMaterialFlags(u32, u32, u32, u32);
    static void jointCB0(daPy_lk_c *, s32);
    static BOOL getPolyRoomPathVec(u32, cXyz *, be<s32> *);
    static void normalizeZP(cXyz *, cXyz *);
    static void addCalcPosXZ(cXyz *, cXyz *, f32, f32, f32);
    static void concatMatrix(Mtx34 *, Mtx34 *, Mtx34 *);
    static void multVec(Mtx34 *, cXyz *, cXyz *);
    static s32 getGroundAngle(daPy_lk_c *, u32, s32);
    static f64 swimTimerRateWide(daPy_lk_c *);
    static void addCalc(be<f32> *, f32, f32, f32, f32);
    static void addVector(cXyz *, cXyz *, cXyz *);
    static void setNextStage(u32, s32, s32, s32, f32, s32, u32, u32);
    static BOOL isEventBit(u32, u32);
    static BOOL chkPlayerId(s32, s32);
    static BOOL checkSeaLandingEvent(s32);
    static void changeScene(s32, f32, s32, s32);
    static void restartRoom(u32, s32);
    static void calcUnderBlend(u32, J3DModel *);
    static void subtractVector(cXyz *, cXyz *, cXyz *);
    static u32 judgeActor(u32, be<u32> *);
    static BOOL seaChkArea(f32, f32);
    static f64 seaCalcWave(f32, f32);
    static s32 getRoomId(dBgS *, u32);
    static u32 getPolyColor(dBgS *, u32);
    static BOOL chkMoveBG(dBgS *, u32);
    static s32 getLinkNo(dBgS *, u32);
    static u32 getActorPointer(dBgS *, u32);
    static s32 getExitId(dBgS *, u32);
    static BOOL changeSceneExitId(u32, f32, s32, s32);
    static void onEventBit(u32, u32);
    static void translateMatrix(Mtx34 *, f32, f32, f32);
    static void rotateMatrixY(Mtx34 *, s32);
    static void copyMatrix(Mtx34 *, Mtx34 *);
    static void inverseMatrix(Mtx34 *, Mtx34 *);
    static void multVecSR(Mtx34 *, cXyz *, cXyz *);
    static void rotateMatrixZXY(Mtx34 *, s32, s32, s32);
    static void translateStack(f32, f32, f32);

    static s32 atan2sYWide(f32, f64);
    static s32 atan2sXWide(f64, f32);
    static void startEventCamera(u32, u32, u32, u32);
    static u32 setParticle(u32, u32, u32, cXyz *, u32, u32, u32, u32, s32, u32, u32, u32);
};

// HD: the two foot records retain the GameCube layout inside the larger player object.
struct LinkMotionFootData {
    /* 0x000 */ be<u8> field_0x000;
    /* 0x001 */ be<u8> field_0x001;
    /* 0x002 */ be<s16> field_0x002;
    /* 0x004 */ be<s16> field_0x004;
    /* 0x006 */ be<s16> field_0x006;
    /* 0x008 */ be<s16> field_0x008;
    /* 0x00A */ be<s16> field_0x00A;
    /* 0x00C */ cXyz field_0x00C;
    /* 0x018 */ cXyz field_0x018;
    /* 0x024 */ cXyz field_0x024;
    /* 0x030 */ be<f32> field_0x030;
    /* 0x034 */ dBgS_GndChk field_0x034;
    /* 0x088 */ Mtx34 field_0x088[3];
};
WWHD_OFFSET(LinkMotionFootData, field_0x00C, 0xC);
WWHD_OFFSET(LinkMotionFootData, field_0x018, 0x18);
WWHD_OFFSET(LinkMotionFootData, field_0x024, 0x24);
WWHD_OFFSET(LinkMotionFootData, field_0x030, 0x30);
WWHD_OFFSET(LinkMotionFootData, field_0x034, 0x34);
WWHD_OFFSET(LinkMotionFootData, field_0x088, 0x88);
WWHD_SIZE(LinkMotionFootData, 0x118);
struct LinkMotionWater {
    u8 _00[0x48];
    /* 0x48 */ be<f32> m_height;
    u8 _4C[4];
};
WWHD_OFFSET(LinkMotionWater, m_height, 0x48);
WWHD_SIZE(LinkMotionWater, 0x50);
inline dBgS_GndChk &linkMotionGroundCheck(void *check) {
    return *reinterpret_cast<dBgS_GndChk *>(check);
}
inline LinkActionAnmHeap &linkMotionAnimeHeap(void *heap) {
    return *reinterpret_cast<LinkActionAnmHeap *>(heap);
}

// HD: private views expose fields missing from the shared caller-facing declarations.
struct LinkMotionEmitter {
    u8 _000[0x34];
    /* 0x034 */ be<f32> mRate;
    u8 _038[0x220 - 0x38];
    /* 0x220 */ cXyz mGlobalDynamicsScale;
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ cXyz mGlobalParticleScale;
};
WWHD_OFFSET(LinkMotionEmitter, mRate, 0x34);
WWHD_OFFSET(LinkMotionEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(LinkMotionEmitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(LinkMotionEmitter, 0x244);
struct LinkMotionDamageCallback {
    be<u32> __vtbl;
    gptr<LinkMotionEmitter> mpEmitter;
    be<u32> mJoint;
};
WWHD_OFFSET(LinkMotionDamageCallback, mpEmitter, 4);
WWHD_SIZE(LinkMotionDamageCallback, 0xC);
struct LinkMotionLine {
    u8 _00[0x14];
    /* 0x14 */ cBgS_PolyInfo mPolyInfo;
    u8 _24[0xC];
    /* 0x30 */ cXyz mCross;
    u8 _3C[0x6C - 0x3C];
};
WWHD_OFFSET(LinkMotionLine, mPolyInfo, 0x14);
WWHD_OFFSET(LinkMotionLine, mCross, 0x30);
WWHD_SIZE(LinkMotionLine, 0x6C);
struct LinkMotionQueries {
    u8 _000[0x9D0];
    /* 0x9D0 */ LinkMotionLine mLinkLinChk;
    u8 _A3C[0xB14 - 0xA3C];
    /* 0xB14 */ dBgS_GndChk mGndChk;
    u8 _B68[0xC8C - 0xB68];
    /* 0xC8C */ dBgS_GndChk mLavaGndChk;
    /* 0xCE0 */ cBgS_PolyInfo mPolyInfo;
};
WWHD_OFFSET(LinkMotionQueries, mLinkLinChk, 0x9D0);
WWHD_OFFSET(LinkMotionQueries, mGndChk, 0xB14);
WWHD_OFFSET(LinkMotionQueries, mLavaGndChk, 0xC8C);
WWHD_OFFSET(LinkMotionQueries, mPolyInfo, 0xCE0);
WWHD_SIZE(LinkMotionQueries, 0xCF0);
inline LinkMotionQueries &linkMotionQueries(u32 player) {
    return *gabi::at<LinkMotionQueries>(player);
}
struct LinkMotionShip {
    u8 _000[0x644];
    /* 0x644 */ be<u32> mStateFlg;
    u8 _648[0x704 - 0x648];
    /* 0x704 */ gptr<fopAc_ac_c> mTornadoActor;
    u8 _708[4];
    /* 0x70C */ gptr<fopAc_ac_c> mWhirlActor;
};
WWHD_OFFSET(LinkMotionShip, mStateFlg, 0x644);
WWHD_OFFSET(LinkMotionShip, mTornadoActor, 0x704);
WWHD_OFFSET(LinkMotionShip, mWhirlActor, 0x70C);
WWHD_SIZE(LinkMotionShip, 0x710);
struct LinkMotionTransform {
    cXyz mScale;
    csXyz mRotation;
    u8 _12[2];
    cXyz mTranslate;
};
WWHD_OFFSET(LinkMotionTransform, mTranslate, 0x14);
WWHD_SIZE(LinkMotionTransform, 0x20);
// HD: the blended root record has a shorter rotation prefix than J3DTransformInfo.
struct LinkMotionRootTransform {
    u8 _00[0x10];
    cXyz mTranslate;
};
WWHD_OFFSET(LinkMotionRootTransform, mTranslate, 0x10);
WWHD_SIZE(LinkMotionRootTransform, 0x1C);
struct LinkMotionOldFrame {
    be<u8> mOldFrameFlg;
    u8 _01[0x1C - 1];
    gptr<LinkMotionTransform> mOldFrameTransInfo;
};
WWHD_OFFSET(LinkMotionOldFrame, mOldFrameTransInfo, 0x1C);
WWHD_SIZE(LinkMotionOldFrame, 0x20);
// These objects belong to other Link/particle units; only borrow their mapped storage.
struct LinkMotionGlobals {
    static be<u16> &damageType() { return gabi::object_at<be<u16>>(0x101CEF16); }
    static be<s16> &damageTimer() { return gabi::object_at<be<s16>>(0x101CEF14); }
    static be<u8> &particleStatus() { return gabi::object_at<be<u8>>(0x101EA4E4); }
    static cXyz *crawlFront() { return gabi::at<cXyz>(0x101CEBF0); }
    static cXyz *crawlBack() { return gabi::at<cXyz>(0x101CEBFC); }
};

#include "SSystem/SComponent/c_m3d_g_pla.h"

struct LinkMotionJointTree {
    u8 _00[8];
    be<u16> mJointCount;
};
WWHD_OFFSET(LinkMotionJointTree, mJointCount, 8);
WWHD_SIZE(LinkMotionJointTree, 0xA);
struct LinkMotionJoint {
    u8 _00[8];
    be<u32> mCallBack;
    u8 _0C[8];
    be<u32> mpMtxCalc;
    u8 _18[4];
};
WWHD_OFFSET(LinkMotionJoint, mCallBack, 8);
WWHD_OFFSET(LinkMotionJoint, mpMtxCalc, 0x14);
WWHD_SIZE(LinkMotionJoint, 0x1C);
struct LinkMotionMtxBuffer {
    u8 _00[0x2E];
    be<u16> mCurrentMtxCalc;
    be<u32> mpMtxCalc;
};
WWHD_OFFSET(LinkMotionMtxBuffer, mCurrentMtxCalc, 0x2E);
WWHD_OFFSET(LinkMotionMtxBuffer, mpMtxCalc, 0x30);
WWHD_SIZE(LinkMotionMtxBuffer, 0x34);
struct LinkMotionBlendEntry {
    be<f32> mRatio;
    be<u32> mpAnmTransform;
    be<s32> mJointCount;
    gptr<be<f32>> mpJointWeights;
};
WWHD_OFFSET(LinkMotionBlendEntry, mpAnmTransform, 4);
WWHD_OFFSET(LinkMotionBlendEntry, mpJointWeights, 0xC);
WWHD_SIZE(LinkMotionBlendEntry, 0x10);
struct LinkMotionBlendData {
    LinkMotionBlendEntry mEntries[3];
};
WWHD_SIZE(LinkMotionBlendData, 0x30);
struct LinkMotionJointFlags {
    u8 mFlags[0x68];
};
WWHD_SIZE(LinkMotionJointFlags, 0x68);
struct LinkMotionBlendCalc {
    u8 _00[0x7C];
    gptr<LinkMotionBlendData> mpBlendData;
    gptr<J3DModel> mpModel;
    gptr<LinkMotionJointFlags> mpJointFlags;
    u8 _88[0xA4 - 0x88];
    gptr<LinkMotionRootTransform> mpRootTransform;
};
WWHD_OFFSET(LinkMotionBlendCalc, mpBlendData, 0x7C);
WWHD_OFFSET(LinkMotionBlendCalc, mpJointFlags, 0x84);
WWHD_OFFSET(LinkMotionBlendCalc, mpRootTransform, 0xA4);
WWHD_SIZE(LinkMotionBlendCalc, 0xA8);
inline LinkMotionBlendCalc &linkMotionBlend(u32 calc) {
    return *gabi::at<LinkMotionBlendCalc>(calc);
}
struct LinkMotionAnmVtable {
    u8 _00[0x14];
    be<u32> getFrameMax;
};
WWHD_OFFSET(LinkMotionAnmVtable, getFrameMax, 0x14);
WWHD_SIZE(LinkMotionAnmVtable, 0x18);
struct LinkMotionAnm {
    u8 _00[4];
    gptr<LinkMotionAnmVtable> __vtbl;
};
WWHD_OFFSET(LinkMotionAnm, __vtbl, 4);
WWHD_SIZE(LinkMotionAnm, 8);

struct LinkMotionStageInfo {
    u8 _00[0xC];
    be<u32> mStageFlags;
};
WWHD_OFFSET(LinkMotionStageInfo, mStageFlags, 0xC);
WWHD_SIZE(LinkMotionStageInfo, 0x10);
inline be<s32> &linkMotionShipRaceResult() {
    return gabi::object_at<be<s32>>(0x101D5F1C);
}
inline cXyz *linkMotionBaseX() {
    return gabi::at<cXyz>(0x101FFBB4);
}
inline cXyz *linkMotionBaseZ() {
    return gabi::at<cXyz>(0x101FFBCC);
}
inline u32 linkMotionSingleRippleCallback() {
    return 0x1047B2E4;
}

namespace LinkMotionConstants {
enum : u32 {
    daPyRFlg0_FAIRY_USE = 0x02000000,
    daPyStts0_UNK20000000_e = 0x20000000,
    daPyStts0_SWIM_e = 0x100000,
    ID_IT_JN_WP_HAMON03 = 0x3F,
    daPyProc_DEMO_DEAD_e = 0xB2,
    ModeFlg_SWIM = 0x40000,
    daPyFlg0_UNK100 = 0x100,
    ModeFlg_DAMAGE = 8,
    daPyProc_AUTO_JUMP_e = 0x24,
    ANM_JMPST = 0x3C,
    daPyFlg0_UNK20000 = 0x20000,
    daPyProc_CLIMB_DOWN_START_e = 0x3E,
    ANM_LADDERDWST = 0x87,
    daPyStts1_UNK10000_e = 0x10000,
    daPyProc_HANG_FALL_START_e = 0x2C,
    ANM_HANGING = 0x50,
    daPyStts0_HANG_e = 0x100,
    daPyFlg0_HOVER_BOOTS = 0x1000,
    daPyProc_FRONT_ROLL_e = 0x1E,
    daPyProc_SWIM_UP_e = 0x35,
    daPyFlg0_UNK4000 = 0x4000,
    daPyProc_SWIM_WAIT_e = 0x36,
    ANM_SWIMWAIT = 0x82,
    ANM_SWIMP = 0x81,
    ID_IT_JN_WP_SHIBUKI = 0x40,
    ID_IT_JN_WP_HAMON01 = 0x3D,
    daPyFlg0_UNK80 = 0x80,
    ModeFlg_MIDAIR = 2,
    JA_SE_LK_INTO_WATER = 0x3808,
    daPyProc_DEMO_FREEZE_DAMAGE_e = 0xB1,
    ID_AK_JN_CCTHUNDER00 = 0x3ED,
    ID_AK_JN_CCTHUNDER01 = 0x3EE,
    ID_AK_JN_BODYABLAZE00 = 0x3F1,
    daPyProc_ELEC_DAMAGE_e = 0x6C,
    ANM_DAMBIRI = 0xCB,
    JA_SE_LK_ELEC_PARALYSED = 0x287C,
    daPyProc_DAMAGE_e = 0x66,
    JA_SE_LK_DAMAGE_NORMAL = 0x282D,
    daPyProc_MOVE_e = 6,
    dRes_INDEX_LKANM_BCK_DAMDASH_e = 0x64,
    daPyProc_CROUCH_DEFENSE_SLIP_e = 0xD,
    ANM_DIFENCEA = 0x17,
    dRes_INDEX_LKANM_BCK_ATNGAHAMA_e = 0x18,
    daPyStts1_UNK80000_e = 0x80000,
    daPyProc_GUARD_SLIP_e = 0x6D,
    daPyProc_POLY_DAMAGE_e = 0x67,
    ANM_DAM = 0x54,
    daPyFlg0_UNK1000000 = 0x01000000,
    daPyProc_SHIP_READY_e = 0x86,
    daPyFlg1_UNK4 = 4,
    ModeFlg_HANG = 0x20,
    AT_TYPE_BOMB = 0x20,
    ModeFlg_ROPE = 0x800,
    AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_WATER = 0x100,
    daPyRFlg0_UNK80000 = 0x80000,
    daPyItem_BOKO_e = 0x101,
    ModeFlg_IN_SHIP = 0x2000,
    fpcNm_NZ_e = 0xC6,
    JA_SE_OBJ_COL_N_BDY_MPLT = 0x6843,
    JA_SE_OBJ_COL_SWS_NMTLP = 0x6817,
    JA_SE_OBJ_COL_NWHP_NMTL = 0x683C,
    JA_SE_OBJ_COL_SWM_NSWL = 0x6815,
    daPyProc_CROUCH_DEFENSE_e = 0xC,
    daPyFlg0_UNK20 = 0x20,
    dRes_INDEX_LKANM_BCK_BOOMCATCH_e = 0x33,
    JA_SE_LK_BOOM_CATCH = 0x2815,
    daPyStts0_BOOMERANG_WAIT_e = 0x400000,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    daPyItem_SWORD_e = 0x103,
    dRes_INDEX_LKANM_BCK_REST_e = 0xD7,
    dRes_INDEX_LKANM_BCK_TAKE_e = 0x103,
    dRes_INDEX_LKANM_BCK_TAKEBOTH_e = 0x104,
    JA_SE_LK_SW_PUTIN_S = 0x280A,
    JA_SE_LK_ITEM_TAKEOUT = 0x2810,
    daPyFlg1_UNK4000000 = 0x04000000,
    daPyRFlg0_UNK80 = 0x80,
    daPyProc_DEMO_STAND_ITEM_PUT_e = 0xC5,
    JA_SE_LK_SW_PULLOUT_S = 0x2807,
    dRes_INDEX_LKANM_BCK_ATNLS_e = 0x22,
    dRes_INDEX_LKANM_BCK_ATNRS_e = 0x24,
    fopAc_ENEMY_e = 2,
    daPyProc_DEMO_TOOL_e = 0xA9,
    dRes_INDEX_LKANM_BTK_TABEKOBE_e = 0x15D,
    daPyProc_FAN_GLIDE_e = 0x93,
    daPyProc_SWIM_MOVE_e = 0x37,
    daPyProc_CUT_ROLL_e = 0x56,
    daPyRFlg0_CRAWL_AUTO_MOVE = 0x1000,
    dIsleRoom_BoatingCourse_e = 0x30,
    JA_SE_FORCE_BACK = 0x8B3,
    dStageType_SEA_e = 7,
    daPyFlg0_DEKU_SP_RETURN_FLG = 0x10,
    daPyFlg1_UNK10000000 = 0x10000000,
    daPyProc_HANG_MOVE_e = 0x2F,
    daPyFlg0_UNK80000000 = 0x80000000,
    daPyProc_LAVA_DAMAGE_e = 0x6B,
    ANM_LAVADAM = 0x61,
    ID_AK_SN_HIDARUMAFIRE = 0x8078,
    JA_SE_LK_FALL_MAGMA = 0x3810,
    daPyStts0_UNK10000000_e = 0x10000000,
    daPyProc_DEMO_LAVA_DAMAGE_e = 0xB0,
    ModeFlg_HOOKSHOT = 0x200,
    ModeFlg_CRAWL = 0x01000000,
    daPyFlg1_UNK100000 = 0x00100000,
    JA_SE_LK_MAZE_WARP_IN = 0x288B,
    ModeFlg_CLIMB = 0x10000,
};
}
