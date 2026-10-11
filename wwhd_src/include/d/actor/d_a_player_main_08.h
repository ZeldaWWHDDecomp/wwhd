/* Link weapon, food, sword and inline copies of the main translation unit.
 * HD: private views refine the opaque blocks of the shared Link layout without changing it. */
#pragma once
#include "d/actor/d_a_player_main_readable.h"
namespace Link08 {
enum {
    daPyProc_FRONT_ROLL_e = 0x1E,
    daPyProc_BOTTLE_SWING_e = 0xA5,
    daPyProc_SWIM_WAIT_e = 0x36,
    daPyProc_GRAB_READY_e = 0x6E,
    daPyProc_GRAB_MISS_e = 0x70,
    dItemNo_BOOMERANG_e = 0x2D,
    CL_JNT_CL_LHANDA_e = 0x8,
    CL_JNT_CL_RHANDA_e = 0xC,
    CL_JNT_HEAD_JNT_e = 0xF,
    UNDER_MOVE0_e = 0x0,
    UPPER_MOVE2_e = 0x2,
    ModeFlg_MIDAIR = 0x2,
    ModeFlg_HANG = 0x20,
    ModeFlg_ROPE = 0x800,
    ModeFlg_IN_SHIP = 0x2000,
    ModeFlg_CLIMB = 0x10000,
    ModeFlg_SWIM = 0x40000,
    ModeFlg_LADDER = 0x400000,
    ModeFlg_CAUGHT = 0x10000000,

    ANM_CUTREL = 0x15,
    ANM_CUTRER = 0x27,
    ANM_CUTTURNB = 0x2A,
    ANM_CUTTURNPWFB = 0x19,
    ANM_HAMSWINGBEND = 0xAD,
    ANM_JATTACK = 0x2B,
    ANM_JATTACKLAND = 0x2F,
    ANM_JMPEDS = 0xD,
    ANM_WAITQ = 0x9D,
    ID_AK_JN_CHARGEPOWER00 = 0x29,
    ID_AK_JN_CHARGEPOWER01 = 0x2A,
    ID_AK_JN_ELEMENTSHIBUKI00 = 0x23,
    ID_IT_JN_PIYOPIYO00 = 0x27A,
    JA_SE_CM_MD_PIYO = 0x50BC,
    JA_SE_LK_ESA_THROW = 0x2824,
    JA_SE_LK_HYOI_SET = 0x2885,
    JA_SE_LK_KAITENGIRI = 0x282B,
    JA_SE_LK_SWORD_CHARGE = 0x203B,
    JA_SE_LK_SW_KAZEKIRI_S = 0x2800,
    JA_SE_LK_V_KAITEN_S = 0x1059,
    JA_SE_PRE_TAKT = 0x8A7,
    dItemNo_HOOKSHOT_e = 0x2F,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dItemNo_SWORD_e = 0x38,
    dItemNo_WIND_WAKER_e = 0x22,
    daPyFlg1_SOUP_POWER_UP = 0x8000,
    daPyItem_BOKO_e = 0x101,
    daPyItem_NONE_e = 0x100,
    daPyItem_SWORD_e = 0x103,
    daPyProc_CUT_ROLL_END_e = 0x57,
    daPyProc_CUT_ROLL_e = 0x56,
    daPyProc_CUT_TURN_MOVE_e = 0x59,
    daPyProc_CUT_TURN_e = 0x55,
    daPyProc_DEMO_DEAD_e = 0xB2,
    daPyProc_JUMP_CUT_LAND_e = 0x5C,
    daPyProc_SHIP_BOOMERANG_e = 0x8B,
    daPyProc_TACT_PLAY_e = 0x9B,
    daPyProc_TACT_WAIT_e = 0x9A,
    daPyProc_WEAPON_FRONT_SWING_END_e = 0x4F,
    daPyStts0_SPIN_ATTACK_e = 0x40000000,
    daPyStts1_UNK20000_e = 0x20000,
    fpcNm_ESA_e = 0xDD,
};
}

BOOL Link08_checkNextMode(daPy_lk_c*, s32);
BOOL Link08_procWeaponFrontSwing_init(daPy_lk_c*);
void Link08_setShapeAngleToAtnActor(daPy_lk_c*);
void Link08_commonProcInit(daPy_lk_c*, u32);
void Link08_setSingleMoveAnime(daPy_lk_c*, u32, f32, f32, s32, f32);
void Link08_setEnemyWeaponAtParam(daPy_lk_c*, s32);
BOOL Link08_changeCutReverseProc(daPy_lk_c*, u32);
void Link08_seStartSwordCut(daPy_lk_c*, u32);
void Link08_deleteEquipItem(daPy_lk_c*, s32);
void Link08_setShipRidePosUseItem(daPy_lk_c*);
void Link08_endDemoMode(daPy_lk_c*);
void Link08_setDamagePoint(daPy_lk_c*, f32);
u32 Link08_loadTextureAnimeResource(daPy_lk_c*, u32, s32);
void Link08_setTextureAnimeResource(daPy_lk_c*, u32, s32);
u32 Link08_loadTextureScrollResource(daPy_lk_c*, u32, s32);
void Link08_setTextureScrollResource(daPy_lk_c*, u32, s32);
void Link08_setDemoTextureAnime(daPy_lk_c*, u32, u32, s32, u32);
void Link08_setSpecialBattle(daPy_lk_c*, s32);
BOOL Link08_changeSpecialBattle(daPy_lk_c*);
void Link08_setTextureAnime(daPy_lk_c*, u32, s32);
BOOL Link08_changeLandProc(daPy_lk_c*, f32);
void Link08_setFallVoice(daPy_lk_c*);
void Link08_mtxFollow_end(void*);
JPABaseEmitter* Link08_mtxFollow_makeEmitter(void*, u32, Mtx34*, cXyz*, cXyz*);
void Link08_endFlameDamageEmitter(daPy_lk_c*);
BOOL Link08_procFrontRollCrash_init(daPy_lk_c*);
void Link08_procWait_init(daPy_lk_c*);
BOOL Link08_changeSlideProc(daPy_lk_c*);
s32 Link08_getDirectionFromAngle(daPy_lk_c*, s32);
void Link08_setNormalSpeedF(daPy_lk_c*, f32, f32, f32, f32);
void Link08_initSeAnime(daPy_lk_c*);
void Link08_procCutTurn_init(daPy_lk_c*, s32);
f64 Link08_getBlurTopRate(daPy_lk_c*);
s32 Link08_getSwordBlurColor(daPy_lk_c*);
void Link08_setJumpCutAtParam(daPy_lk_c*);
void Link08_setHammerQuake(daPy_lk_c*, u32, u32, s32);
BOOL Link08_checkHeavyStateOn(daPy_lk_c*);
void Link08_setAtCpsSpl(daPy_lk_c*, s32);
void Link08_resetFootEffect(daPy_lk_c*);
BOOL Link08_checkBoomerangAnime(daPy_lk_c*);

struct Link08ItemBlock {
    u8 padding[0x4440 - 0xD90];
    gptr<J3DModel> mpEquipItemModel;
};
WWHD_OFFSET(Link08ItemBlock, mpEquipItemModel, 0x36B0);
struct Link08AnmRatioPack {
    be<f32> mRatio;
    be<u32> mpAnmTransform;
    u8 padding[8];
};
WWHD_OFFSET(Link08AnmRatioPack, mpAnmTransform, 4);
WWHD_SIZE(Link08AnmRatioPack, 0x10);
struct Link08AnmHeap {
    be<u16> mIdx, field_0x2, field_0x4, field_0x6;
    gptr<void> m_buffer;
    gptr<JKRSolidHeap> mpAnimeHeap;
};
WWHD_SIZE(Link08AnmHeap, 0x10);
struct Link08PolyInfo {
    be<u16> mPolyIndex, mBgIndex;
    u8 padding[0xC];
};
WWHD_SIZE(Link08PolyInfo, 0x10);
struct Link08LineCheck {
    u8 padding[0x14];
    Link08PolyInfo mPolyInfo;
};
WWHD_OFFSET(Link08LineCheck, mPolyInfo, 0x14);
inline Link08ItemBlock& link08ItemBlock(daPy_lk_c* player) {
    return *reinterpret_cast<Link08ItemBlock*>(player->mD90);
}
inline Link08AnmRatioPack* link08UpperRatios(daPy_lk_c* player) {
    return reinterpret_cast<Link08AnmRatioPack*>(player->mAnmRatioUpper);
}
inline be<u8>& link08JumpCutFollowUp(daPy_lk_c* player) {
    return *reinterpret_cast<be<u8>*>(player->m69E8);
}
inline Link08LineCheck& link08LineCheck(daPy_lk_c* player) {
    return *reinterpret_cast<Link08LineCheck*>(player->mLinkLinChk);
}

// The owner source d_a_player_main.cpp defines the TU-wide curse object once.
extern be<u16>& l_curseState;
void Link08_setBaitItemEmpty(dSv_player_bag_item_c*, u32);

inline void link08SetReserveBaitEmpty(u32 btn) {
    Link08_setBaitItemEmpty(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, btn);
}
f64 Link08_addCalcDouble(be<f32>*, f32, f32, f32, f32);
u32 Link08_getCameraBody();
void Link08_endEventCamera(u32, u32);
f32 Link08_addCalc(be<f32>*, f32, f32, f32, f32);
f64 Link08_sqrt(f64);
f64 Link08_squareMagnitude(cXyz*);
s16 Link08_atan2s(f64, f64);
fopAc_ac_c* Link08_fastCreate(u32, u32, cXyz*, s32, csXyz*, void*, s32, s32, s32);
void Link08_updateAnimationEvaluator(u32);
void Link08_endSmokeCallback(void*);
u32 Link08_getTrianglePlane(dBgS*, u32, u32);
void Link08_startSystemSound(u32);
void Link08_initSwordBlur(u32, Mtx34*, s32, f64, s32);
void Link08_clearActorKeep(daPy_actorKeep_l*);
BOOL Link08_compulsoryEvent(u32, fopAc_ac_c*, s32, u32);
f64 Link08_getTactBeatFrames();
BOOL Link08_getWaterY(cXyz*, be<f32>*);
void Link08_getSeaColor(GXColor*, GXColor*);
u32 Link08_getLinkVoiceVowel(u32);
void Link08_startLinkVoice(u32, cXyz*, u32, s32);
void Link08_matrixToRotation(Mtx34*, csXyz*);
void Link08_initializeHeaderByte(u32);
void Link08_initializeHeaderFlag(u32);
void Link08_constructGroundCheck(u32);
void Link08_initializeFrameControl(u32, s32);
void Link08_constructVertexBufferObject(u32);
void Link08_constructVertexAttributeBinding(u32);
void Link08_constructSmokeCallback(u32, s32);
void Link08_constructFollowCallback(u32, s32, s32);
void Link08_constructCollisionInfo(u32);
void Link08_constructCapsuleGeometry(u32);
void Link08_destroyVertexAttributeBinding(u32, s32);
void Link08_destroyVertexBufferObject(u32, s32);
void Link08_destroyBackgroundCheck(u32, s32);
void Link08_releaseVertexAttributeBinding(u32);
u32 Link08_findHeapFromRoot(u32, u32);
void Link08_releaseShapeBinding(u32);
void Link08_destroyShapeBindingCollection(u32, s32);
void Link08_destroyDisplayList(u32, s32);
void Link08_destroyShaderBinding(u32, s32);
void Link08_destroyShapeEntryArray(u32, s32);
void Link08_destroyArray(u32, u32, u32, u32, s32, s32);
void Link08_destroyRenderPacketBase(u32, s32);
void Link08_destroyPlaneGeometry(u32, s32);
BOOL Link08_checkGrabBarrelSearch(daPy_lk_c*, s32);
u32 Link08_allocate(u32);
void Link08_deallocate(u32);

// HD: the shared model and actor headers leave these fields opaque.
struct Link08MtxBuffer {
    u8 padding0[4];
    be<u16> mFlags;
    u8 padding6[0xA];
    gptr<Mtx34> mpAnmMtx;
};
WWHD_OFFSET(Link08MtxBuffer, mpAnmMtx, 0x10);
struct Link08Model {
    u8 padding[0x2C];
    gptr<Link08MtxBuffer> mpMtxBuffer;
};
WWHD_OFFSET(Link08Model, mpMtxBuffer, 0x2C);
struct Link08Boko {
    u8 padding0[0x432];
    be<u8> mSetThrow;
    u8 padding433[9];
    be<s16> mFlameTimer;
    u8 padding43E[4];
    be<s16> mThrowAngle;
};
WWHD_OFFSET(Link08Boko, mSetThrow, 0x432);
WWHD_OFFSET(Link08Boko, mFlameTimer, 0x43C);
WWHD_OFFSET(Link08Boko, mThrowAngle, 0x442);
struct Link08CutTurnCallback {
    be<u32> mVtable;
    be<u8> mAlpha;
    u8 padding[0xB];
};
WWHD_SIZE(Link08CutTurnCallback, 0x10);
inline Link08CutTurnCallback& link08CutTurnCallback(void* callback) {
    return *reinterpret_cast<Link08CutTurnCallback*>(callback);
}
// The shared SDK exposes these bytes without the guest-storage wrapper.
inline be<u8>& link08CutType(daPy_lk_c* player) {
    return *reinterpret_cast<be<u8>*>(&player->mCutType);
}
inline be<u8>& link08CutCount(daPy_lk_c* player) {
    return *reinterpret_cast<be<u8>*>(&player->mCutCount);
}

// HD: animation objects replace the GameCube BPK/BTK frame setters.
struct Link08AnimationEvaluator {
    be<f32> mValue, mA, mB;
    u8 padding[4];
    gfn mEvaluate;
    gptr<void> mpContext;
};
WWHD_OFFSET(Link08AnimationEvaluator, mEvaluate, 0x10);
WWHD_OFFSET(Link08AnimationEvaluator, mpContext, 0x14);
WWHD_SIZE(Link08AnimationEvaluator, 0x18);
struct Link08CutAnimations {
    u8 padding0[0x4984];
    be<f32> mBpkFrame;
    u8 padding4988[8];
    gptr<be<f32>> mpBpkFrame;
    u8 padding4994[4];
    gptr<Link08AnimationEvaluator> mpBpkEvaluator;
    u8 padding499C[0x5C];
    be<f32> mBtkFrame;
    u8 padding49FC[8];
    gptr<Link08AnimationEvaluator> mpBtkEvaluator;
    u8 padding4A08[0x54];
    gptr<be<f32>> mpBtkFrame;
};
WWHD_OFFSET(Link08CutAnimations, mBpkFrame, 0x4984);
WWHD_OFFSET(Link08CutAnimations, mpBpkFrame, 0x4990);
WWHD_OFFSET(Link08CutAnimations, mpBpkEvaluator, 0x4998);
WWHD_OFFSET(Link08CutAnimations, mBtkFrame, 0x49F8);
WWHD_OFFSET(Link08CutAnimations, mpBtkEvaluator, 0x4A04);
WWHD_OFFSET(Link08CutAnimations, mpBtkFrame, 0x4A5C);

// HD: emitter fields used by the rolled-cut and item-water effects.
struct Link08Emitter {
    u8 padding0[0x14];
    cXyz mTranslation;
    u8 padding20[8];
    cXyz mDirection;
    be<f32> mRate;
    u8 padding38[0x20];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    be<s16> mLifeTime;
    u8 padding62[2];
    be<s16> mVolumeSize;
    u8 padding66[6];
    be<f32> mAwayFromAxisSpeed, mDirectionalSpeed;
    u8 padding74[0x1C4];
    cXyz mGlobalParticleScale;
    u8 padding244[0x10];
    be<u32> mFlags;
};
WWHD_OFFSET(Link08Emitter, mTranslation, 0x14);
WWHD_OFFSET(Link08Emitter, mDirection, 0x28);
WWHD_OFFSET(Link08Emitter, mRate, 0x34);
WWHD_OFFSET(Link08Emitter, mSpread, 0x58);
WWHD_OFFSET(Link08Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Link08Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Link08Emitter, mVolumeSize, 0x64);
WWHD_OFFSET(Link08Emitter, mAwayFromAxisSpeed, 0x6C);
WWHD_OFFSET(Link08Emitter, mDirectionalSpeed, 0x70);
WWHD_OFFSET(Link08Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Link08Emitter, mFlags, 0x254);
WWHD_SIZE(Link08Emitter, 0x258);

struct Link08VectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(Link08VectorWords, 0xC);
struct Link08AngleWords {
    be<u16> x, y, z;
};
WWHD_SIZE(Link08AngleWords, 6);
inline Link08VectorWords& link08VectorWords(cXyz& vector) {
    return *reinterpret_cast<Link08VectorWords*>(&vector);
}
inline const Link08VectorWords& link08VectorWords(const cXyz& vector) {
    return *reinterpret_cast<const Link08VectorWords*>(&vector);
}
inline Link08AngleWords& link08AngleWords(csXyz& angle) {
    return *reinterpret_cast<Link08AngleWords*>(&angle);
}
inline void link08CopyVectorWords(cXyz* destination, const cXyz* source) {
    Link08VectorWords& to = link08VectorWords(*destination);
    const Link08VectorWords& from = link08VectorWords(*source);
    to.x = from.x;
    to.y = from.y;
    to.z = from.z;
}

// HD: the hookshot's mode and carried-actor data moved into its expanded layout.
struct Link08Hookshot {
    u8 padding0[0xB0];
    be<u32> mMode;
    u8 paddingB4[0xD6B4];
    cXyz mCarryOffset;
    be<u32> mCarryActorID;
};
WWHD_OFFSET(Link08Hookshot, mMode, 0xB0);
WWHD_OFFSET(Link08Hookshot, mCarryOffset, 0xD768);
WWHD_OFFSET(Link08Hookshot, mCarryActorID, 0xD774);
struct Link08TrianglePlane {
    cXyz mNormal;
    be<f32> mD;
};
WWHD_SIZE(Link08TrianglePlane, 0x10);
inline Link08AnmHeap* link08UpperHeaps(daPy_lk_c* player) {
    return reinterpret_cast<Link08AnmHeap*>(player->m_anm_heap_upper);
}

inline void link08CutEnd(daPy_lk_c* player) {
    dComIfGp_evmng_cutEnd(player->mStaffIdx);
}
inline void link08BokoUpperEnd(daPy_lk_c* player) {
    if (player->mEquipItem == Link08::daPyItem_BOKO_e &&
        link08UpperRatios(player)[0].mpAnmTransform != 0) {
        player->mFrameCtrlUpper[0].setFrame((f32)player->mFrameCtrlUpper[0].getEnd() - 0.001f);
        // HD: the animation object's frame is its first word.
        *gabi::at<be<f32>>(link08UpperRatios(player)[0].mpAnmTransform) =
            player->mFrameCtrlUpper[0].getFrame();
    }
}
inline void link08CutHoldFlag(daPy_lk_c* player) {
    if ((player->mItemButton & 2) && player->m34C5 == 0)
        player->mNoResetFlg0 = player->mNoResetFlg0 | 4;
    else
        player->mNoResetFlg0 = player->mNoResetFlg0 & ~4u;
}
struct Link08VirtualTable {
    u8 padding0[0x3C];
    gfn mCheckPlayerGuard;
    u8 padding40[0xA4];
    gfn mVoiceStart;
    u8 paddingE8[0x2C];
    gfn mSetPlayerPosAndAngle;
};
WWHD_OFFSET(Link08VirtualTable, mCheckPlayerGuard, 0x3C);
WWHD_OFFSET(Link08VirtualTable, mVoiceStart, 0xE4);
WWHD_OFFSET(Link08VirtualTable, mSetPlayerPosAndAngle, 0x114);
inline Link08VirtualTable* link08VirtualTable(daPy_lk_c* player) {
    return gabi::at<Link08VirtualTable>(player->__vtbl);
}
inline void link08VoiceStart(daPy_lk_c* player, u32 id) {
    gabi::call_ptr(link08VirtualTable(player)->mVoiceStart.get(), player, id);
}

// HD: compiler-generated resource and callback storage.
struct Link08HeaderObject {
    u8 padding0[8];
    be<u8> mInitialized;
    u8 padding9[0x17];
    be<u32> mResource;
};
WWHD_OFFSET(Link08HeaderObject, mInitialized, 8);
WWHD_OFFSET(Link08HeaderObject, mResource, 0x20);
WWHD_SIZE(Link08HeaderObject, 0x24);
struct Link08AnimationState {
    be<f32> mFrame;
    be<u32> mResource, mEvaluator, mContext;
};
WWHD_SIZE(Link08AnimationState, 0x10);
struct Link08VertexBuffer {
    be<u32> mpBuffer;
    u8 mBufferObject[0x154];
    u8 mAttributeBinding[0xF4];
    be<u32> mVertexCount, mpAllocation;
};
WWHD_OFFSET(Link08VertexBuffer, mBufferObject, 4);
WWHD_OFFSET(Link08VertexBuffer, mAttributeBinding, 0x158);
WWHD_OFFSET(Link08VertexBuffer, mVertexCount, 0x24C);
WWHD_OFFSET(Link08VertexBuffer, mpAllocation, 0x250);
WWHD_SIZE(Link08VertexBuffer, 0x254);
struct Link08VtableObject {
    be<u32> mVtable;
};
WWHD_SIZE(Link08VtableObject, 4);
struct Link08BufferedString {
    be<u32> mpBuffer, mVtable, mCapacity;
};
WWHD_SIZE(Link08BufferedString, 0xC);
struct Link08AnimationInfo {
    u8 padding0[0xC];
    be<u32> mFlags;
    u8 padding10[2];
    be<s16> mEndFrame;
};
WWHD_OFFSET(Link08AnimationInfo, mFlags, 0xC);
WWHD_OFFSET(Link08AnimationInfo, mEndFrame, 0x12);
struct Link08AnimationInfoOwner {
    u8 padding0[8];
    gptr<Link08AnimationInfo> mpInfo;
};
WWHD_OFFSET(Link08AnimationInfoOwner, mpInfo, 8);
struct Link08FanSwingCallback {
    be<u32> mVtable, mState, mpEmitter;
};
WWHD_SIZE(Link08FanSwingCallback, 0xC);

struct Link08ResourceName {
    be<u32> mStringTop, mVtable;
};
WWHD_SIZE(Link08ResourceName, 8);
struct Link08CylinderSource {
    u8 padding[0x30];
    cXyz mCenter;
    be<f32> mRadius, mHeight;
};
WWHD_OFFSET(Link08CylinderSource, mRadius, 0x3C);
WWHD_OFFSET(Link08CylinderSource, mHeight, 0x40);
WWHD_SIZE(Link08CylinderSource, 0x44);
struct Link08CapsuleSource {
    u8 padding[0x30];
    cXyz mStart, mEnd;
    be<f32> mRadius;
};
WWHD_OFFSET(Link08CapsuleSource, mRadius, 0x48);
WWHD_SIZE(Link08CapsuleSource, 0x4C);

struct Link08StaticGroundCheck {
    be<u32> mVtable;
    u8 padding[4];
    dBgS_GndChk mGroundCheck;
};
WWHD_OFFSET(Link08StaticGroundCheck, mGroundCheck, 8);
WWHD_SIZE(Link08StaticGroundCheck, 0x5C);
struct Link08HeaderZeroState {
    be<u32> mWords[4];
};
WWHD_SIZE(Link08HeaderZeroState, 0x10);
struct Link08HeaderAngleLimits {
    be<f32> mMinimum, mMaximum;
};
WWHD_SIZE(Link08HeaderAngleLimits, 8);
struct Link08DestructorRecord {
    u8 mStorage[0xC];
};
WWHD_SIZE(Link08DestructorRecord, 0xC);

struct Link08HeapVirtualTable {
    u8 padding[0x3C];
    gfn mFree;
};
WWHD_OFFSET(Link08HeapVirtualTable, mFree, 0x3C);
struct Link08Heap {
    u8 padding[0xC];
    gptr<Link08HeapVirtualTable> mpVirtualTable;
};
WWHD_OFFSET(Link08Heap, mpVirtualTable, 0xC);
struct Link08ShapeEntries {
    be<u32> mCount, mpEntries;
};
WWHD_SIZE(Link08ShapeEntries, 8);
struct Link08SwordBlur {
    u8 padding0[0xC];
    be<u32> mVtable;
    u8 padding10[0x11DD0 - 0x10];
    be<u32> mCurrentMaterial;
    u8 padding11DD4[4];
    Link08ShapeEntries mEntries;
};
WWHD_OFFSET(Link08SwordBlur, mVtable, 0xC);
WWHD_OFFSET(Link08SwordBlur, mCurrentMaterial, 0x11DD0);
WWHD_OFFSET(Link08SwordBlur, mEntries, 0x11DD8);

struct Link08CapsuleShapeAttributes {
    u8 padding[0x18];
    be<u32> mAabbVtable, mVtable;
};
WWHD_SIZE(Link08CapsuleShapeAttributes, 0x20);
struct Link08CapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> mVtable;
    be<f32> mRadius;
};
WWHD_SIZE(Link08CapsuleGeometry, 0x20);
struct Link08CapsuleCollision : dCcD_GObjInf {
    Link08CapsuleShapeAttributes mShapeAttr;
    Link08CapsuleGeometry mCps;
};
WWHD_OFFSET(Link08CapsuleCollision, mShapeAttr, 0xF8);
WWHD_OFFSET(Link08CapsuleCollision, mCps, 0x118);
WWHD_SIZE(Link08CapsuleCollision, 0x138);
struct Link08CapsuleVtables {
    u32 mShapeBase, mAabb, mHitInfo, mCapsule, mShape;
};
struct Link08MovementDefaults {
    be<f32> mSwimWallHeight, mLadderLineOffset, mCrawlWallRadius, mSwimWallRadius,
        mRopeLengthOffset;
};
WWHD_SIZE(Link08MovementDefaults, 0x14);
struct Link08HeaderDistances {
    be<f32> mLargeDistance0, mLargeDistance1, mSmallDistance0, mSmallDistance1;
};
WWHD_SIZE(Link08HeaderDistances, 0x10);
struct Link08ModelMatrices {
    u8 padding[0xC8];
    Mtx34 mBaseTRMtx;
};
WWHD_OFFSET(Link08ModelMatrices, mBaseTRMtx, 0xC8);
inline Mtx34* link08BaseTRMtx(J3DModel* model) {
    return model ? &reinterpret_cast<Link08ModelMatrices*>(model)->mBaseTRMtx : nullptr;
}

// HD: named inline views retain the shared split-TU entry-point wrappers.
struct Link08Player : daPy_lk_c {
    f32 getGroundY();
    Mtx34* getLeftHandMatrix();
    Mtx34* getRightHandMatrix();
    u32 checkPlayerFly();
    BOOL checkMidairOrSwimWaitHD();
    BOOL checkFrontRoll();
    BOOL checkBottleSwing();
    BOOL checkCutCharge();
    f32 getBaseAnimeFrameRate();
    f32 getBaseAnimeFrame();
    u32 getItemID();
    u32 getThrowBoomerangID();
    u32 getGrabActorID();
    BOOL checkGrabBarrel();
    BOOL checkRopeTag();
    BOOL checkRopeReadyAnime();
    Mtx34* getModelJointMtx(s32 jnt);
    void onFrollCrashFlg(u32 v);
};
WWHD_SIZE(Link08Player, sizeof(daPy_lk_c));
