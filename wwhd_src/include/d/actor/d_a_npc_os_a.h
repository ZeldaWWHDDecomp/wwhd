/* Os event, movement and rendering helpers. WWHD keeps the shared Os actor layout;
 * private views below describe the HD-only globals without changing the other parts. */
#pragma once
#include "d/actor/d_a_npc_os_local.h"
#include "gc_api/gc_api.h"

// HD: the lighting assignment mixes float, halfword and word copies; keep those widths.
struct OsLightParametersHD {
    cXyz mPosition;
    cXyz mDirection;
    GXColor mColor;
    GXColorS10 mColorS10;
    be<f32> mCosAtten[3];
    be<f32> mDistAtten[3];
    be<f32> mCutoff;
    be<f32> mReferenceBrightness;
};
WWHD_SIZE(OsLightParametersHD, 0x44);
struct OsWordVectorHD {
    be<u32> x, y, z;
};
struct OsSmokeLightingHD {
    OsLightParametersHD mLight;
    u8 mLightPadding[0x40];
    OsWordVectorHD mLightPosWorld;
    GXColorS10 mColorC0;
    be<u32> mColorK0, mColorK1;
    GXColorS10 mFogColor;
    be<f32> mFogStartZ, mFogEndZ, mColpatBlend;
    be<u8> mInitTimer, mEnvrIdxCurr, mEnvrIdxPrev, mColpatCurr, mColpatPrev;
    be<u8> mRoomNo, mEnvrIdxOverride, mLightMode, mInitType;
    u8 mPadding[3];
    OsLightParametersHD mCachedLight;
    u8 mCachedLightPadding[0x40];
    OsLightParametersHD mCachedLight2;
    u8 mCachedLight2Padding[0x40];
};
WWHD_OFFSET(OsSmokeLightingHD, mLightPosWorld, 0x84);
WWHD_OFFSET(OsSmokeLightingHD, mColorC0, 0x90);
WWHD_OFFSET(OsSmokeLightingHD, mColorK0, 0x98);
WWHD_OFFSET(OsSmokeLightingHD, mFogColor, 0xA0);
WWHD_OFFSET(OsSmokeLightingHD, mRoomNo, 0xB9);
WWHD_OFFSET(OsSmokeLightingHD, mCachedLight, 0xC0);
WWHD_OFFSET(OsSmokeLightingHD, mCachedLight2, 0x144);
WWHD_SIZE(OsSmokeLightingHD, 0x1C8);

struct OsWaterScaleStaticsHD {
    be<u32> mSplashGuard, mRippleGuard;
    cXyz mSplashScale, mRippleScale;
};
WWHD_OFFSET(OsWaterScaleStaticsHD, mSplashScale, 8);
WWHD_OFFSET(OsWaterScaleStaticsHD, mRippleScale, 0x14);
WWHD_SIZE(OsWaterScaleStaticsHD, 0x20);
struct OsEmitterScaleHD {
    u8 mPrefix[0x220];
    cXyz mGlobalScale;
    u8 mMiddle[0x0C];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(OsEmitterScaleHD, mGlobalScale, 0x220);
WWHD_OFFSET(OsEmitterScaleHD, mGlobalParticleScale, 0x238);
WWHD_SIZE(OsEmitterScaleHD, 0x244);
struct OsJointHioHD {
    u8 mPrefix[0x30];
    dNpc_HIO_l mNpc;
};
WWHD_OFFSET(OsJointHioHD, mNpc, 0x30);

BOOL Os_triggerReturnHD(s32 port);
void Os_initBrkHD(daNpc_Os_c* actor, u32 animation, u32 loop);
void Os_endEmitterHD(daNpc_Os_infiniteEcallBack_l* callback);
void Os_orderEventHD(daNpc_Os_c* actor, s32 event, u32 mapTool, u32 flags, s32 priority, s32 mode);
BOOL Os_startCheckHD(dEvent_manager_c* manager, const char* name);
BOOL Os_endCheckNameHD(dEvent_manager_c* manager, const char* name);
void Os_controlSoundHD(u32 sound);
s32 Os_getActIndexHD(dEvent_manager_c* manager, s32 staff, gptr<char>* names, s32 count, s32 flag, s32 extra);
void Os_cutEndHD(dEvent_manager_c* manager, s32 staff);
BOOL Os_getAdvanceHD(dEvent_manager_c* manager, s32 staff);
BOOL Os_endCheckHD(dEvent_manager_c* manager, s32 event);
void Os_enterBrkHD(mDoExt_brkAnm_os* animation, J3DModelData* data, f32 frame);
BOOL Os_initialRoomCheckHD(daNpc_Os_c* actor);
void Os_initialRestartHD(daNpc_Os_c* actor, s32 restart, s32 flag);
void Os_setWallRadiusHD(dBgS_AcchCir* circle, f32 radius);
fopAc_ac_c* Os_searchPedestalHD(const char* name, s32 room, s32 flag);
BOOL Os_isCreatingHD(u32 processId);
BOOL Os_moveStopCheckHD(daNpc_Os_c* actor);
BOOL Os_polygonSafeHD(dBgS* background, cBgS_PolyInfo_l* polygon);
BOOL Os_isMovingBackgroundHD(dBgS* background, cBgS_PolyInfo_l* polygon);
void Os_correctMovingBackgroundHD(dBgS* background, cBgS_PolyInfo_l* polygon, s32 flag, cXyz* pos, s32 angle,
                                  s32 extra);
void Os_setJointParamsHD(dNpc_JntCtrl_c* joint, s32 backX, s32 backY, s32 minBackX, s32 minBackY, s32 headX,
                         s32 headY, s32 minHeadX, s32 minHeadY, s32 step);
BOOL Os_positionMoveCheckHD(daNpc_Os_c* actor, const char* name);
void Os_movePositionHD(daNpc_Os_c* actor, dCcD_Stts* status);
s32 Os_groundRoomHD(dBgS* background, cBgS_PolyInfo_l* polygon);
u32 Os_groundColorHD(dBgS* background, cBgS_PolyInfo_l* polygon);
s32 Os_groundCodeHD(dBgS* background, cBgS_PolyInfo_l* polygon);
u32 Os_trianglePlaneHD(dBgS* background, u32 backgroundIndex, u32 polygonIndex);
void Os_restartLinkHD(fopAc_ac_c* player, s32 mode, s32 room, f32 speed, s32 flag);

struct OsCutNamesHD {
    gptr<char> mNames[12];
};
struct OsNamesHD {
    gptr<char> mNames[3];
};
inline void Os_clearCbPlayerHD() {
    reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mpPlayerPtr[1].v = 0;
}

struct OsRoomStatusHD {
    be<u8> mStatus;
    u8 mRoomData[0x22B];
};
WWHD_SIZE(OsRoomStatusHD, 0x22C);
struct OsPlacementWordsHD {
    OsWordVectorHD mPosition;
    be<u32> mAnglesXY, mAngleZAndRoom;
};
WWHD_OFFSET(OsPlacementWordsHD, mAnglesXY, 0x0C);
WWHD_SIZE(OsPlacementWordsHD, 0x14);
struct OsAnimatedModelDataHD : J3DModelData {
    u8 mAnimationPrefix[0x48 - sizeof(J3DModelData)];
    be<u32> mpBrk;
};
WWHD_OFFSET(OsAnimatedModelDataHD, mpBrk, 0x48);
struct OsCattleRoomHD {
    be<s8> mRoomNo;
};

// HD: compact GHS action descriptors keep the original eight-byte copy and dispatch ABI.
struct OsActionWordsHD {
    be<u32> mDeltaAndIndex, mFunction;
};
WWHD_SIZE(OsActionWordsHD, 8);
struct OsActionDescriptorsHD {
    ProcFunc_l mWaitNpc, mWaitPlayer;
    u8 mBeforeCarry[0x10];
    ProcFunc_l mCarry;
    u8 mBeforeTalk[0x20];
    ProcFunc_l mTalk;
};
WWHD_OFFSET(OsActionDescriptorsHD, mCarry, 0x20);
WWHD_OFFSET(OsActionDescriptorsHD, mTalk, 0x48);
WWHD_SIZE(OsActionDescriptorsHD, 0x50);
struct OsEventActionsHD {
    gabi::ptmf<BOOL (daNpc_Os_c::*)(s32)> mActions[12];
};
inline void Os_copyActionHD(ProcFunc_l* dst, const ProcFunc_l* src) {
    auto* out = reinterpret_cast<OsActionWordsHD*>(dst);
    const auto* in = reinterpret_cast<const OsActionWordsHD*>(src);
    out->mDeltaAndIndex = (u32)in->mDeltaAndIndex;
    out->mFunction = (u32)in->mFunction;
}
template <class A> inline BOOL Os_callActionHD(daNpc_Os_c* actor, ProcFunc_l* descriptor, A arg) {
    auto* action = reinterpret_cast<gabi::ptmf<BOOL (daNpc_Os_c::*)(A)>*>(descriptor);
    return (actor->**action)(arg);
}
enum OsActionTargetHD : u32 {
    OS_CARRY_ACTION_HD = 0x022AECD4,
    OS_THROW_ACTION_HD = 0x022AEF70,
    OS_SEARCH_ACTION_HD = 0x022AF8D4,
    OS_WALK_PLAYER_ACTION_HD = 0x022AFF94,
};

WWHD_OFFSET(OsLightParametersHD, mDirection, 0x0C);
WWHD_OFFSET(OsLightParametersHD, mColor, 0x18);
WWHD_OFFSET(OsLightParametersHD, mColorS10, 0x1C);
WWHD_OFFSET(OsLightParametersHD, mCosAtten, 0x24);
WWHD_OFFSET(OsLightParametersHD, mDistAtten, 0x30);
WWHD_OFFSET(OsLightParametersHD, mCutoff, 0x3C);
WWHD_SIZE(OsWordVectorHD, 0x0C);
WWHD_SIZE(OsCutNamesHD, 0x30);
WWHD_SIZE(OsNamesHD, 0x0C);
WWHD_SIZE(OsEventActionsHD, 0x60);
WWHD_SIZE(OsCattleRoomHD, 1);
WWHD_SIZE(OsJointHioHD, 0x58);

static void daNpc_Os_infiniteEcallBack_makeEmitter(daNpc_Os_infiniteEcallBack_l* cb, u16 id, const cXyz* pos,
                                                   const csXyz* angle, const cXyz* scale);

namespace os_a {
enum {
    dActStts_RETURN_e = 7,
    dActStts_HIDDEN_e = 0x3E,
    ID_AK_SN_OTOMOBEAM00 = 0x826E,
    ID_AK_SN_OTOMOBEAM01 = 0x826F,
    ID_IT_JN_WP_SHIBUKI = 0x40,
    ID_IT_JN_WP_HAMON01 = 0x3D,
    JA_SE_CTRL_NPC_TO_LINK = 0x886,
    JA_SE_OBJ_OSTATUE_LIGHT_ST = 0x5900,
    JA_SE_OBJ_OSTATUE_LIGHT_SUS = 0x7046,
};
}
