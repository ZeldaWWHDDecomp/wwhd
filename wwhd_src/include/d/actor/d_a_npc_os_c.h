/* Os NPC and player actions, WWHD-specific private views. Shared actor declarations
 * keep the original tww field names and measured HD offsets. */
#pragma once
#include "d/actor/d_a_npc_os_local.h"
#include "gc_api/gc_api.h"
// HD: HIO vtables trail their fields; the child starts at offset four.
struct OsActionHioChildHD {
    be<f32> field_0x04, field_0x08, field_0x0C, field_0x10, field_0x14, field_0x18, field_0x1C, field_0x20;
    be<s16> field_0x24, field_0x26, field_0x28;
    u8 _pad[2];
    be<u32> vtable;
};
WWHD_SIZE(OsActionHioChildHD, 0x2C);
struct OsActionHioHD {
    /* 0x00 */ be<s8> mNo;
    u8 _pad[3];
    /* 0x04 */ OsActionHioChildHD mOs2;
    /* 0x30 */ dNpc_HIO_l mNpc;
    /* 0x58 */ gptr<daNpc_Os_c> field_0x5C;
    /* 0x5C */ be<f32> field_0x60;
    /* 0x60 */ be<f32> field_0x64;
    /* 0x64 */ be<f32> field_0x68;
    /* 0x68 */ be<f32> field_0x6C;
    /* 0x6C */ be<f32> field_0x70;
    /* 0x70 */ be<f32> field_0x74;
    /* 0x74 */ be<f32> field_0x78;
    /* 0x78 */ be<f32> field_0x7C;
    /* 0x7C */ be<f32> field_0x80;
    /* 0x80 */ be<f32> field_0x84;
    /* 0x84 */ be<f32> field_0x88;
    /* 0x88 */ be<f32> field_0x8C;
    /* 0x8C */ be<f32> field_0x90;
    /* 0x90 */ be<f32> field_0x94;
    /* 0x94 */ be<f32> field_0x98;
    /* 0x98 */ be<f32> field_0x9C;
    /* 0x9C */ be<f32> field_0xA0;
    /* 0xA0 */ be<f32> field_0xA4;
    /* 0xA4 */ be<f32> field_0xA8;
    /* 0xA8 */ be<f32> field_0xAC;
    /* 0xAC */ be<f32> field_0xB0;
    be<u32> vtable;
};
WWHD_SIZE(OsActionHioHD, 0xB4);
WWHD_OFFSET(OsActionHioHD, mNo, 0x00);
WWHD_OFFSET(OsActionHioHD, mOs2, 0x04);
WWHD_OFFSET(OsActionHioHD, mNpc, 0x30);
WWHD_OFFSET(OsActionHioHD, field_0x5C, 0x58);
WWHD_OFFSET(OsActionHioHD, field_0x8C, 0x88);
WWHD_OFFSET(OsActionHioHD, field_0xAC, 0xA8);
WWHD_OFFSET(OsActionHioHD, field_0xB0, 0xAC);
WWHD_OFFSET(OsActionHioChildHD, field_0x08, 0x04);
WWHD_OFFSET(OsActionHioChildHD, field_0x24, 0x20);
WWHD_OFFSET(OsActionHioChildHD, field_0x26, 0x22);
WWHD_OFFSET(OsActionHioChildHD, field_0x28, 0x24);

inline const OsActionHioHD* Os_actionHioHD() { return gabi::at<const OsActionHioHD>(0x10467E44); }
inline void Os_clearCbPlayerHD() {
    reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mpPlayerPtr[1].v = 0;
}

void os_setAnm(daNpc_Os_c* t, s32 n);
u32 os_wakeupCheck(daNpc_Os_c* t);
u32 os_finishCheck(daNpc_Os_c* t);
void os_initBrkAnm(daNpc_Os_c* t, u8 n, u32 b);
u32 os_getRestartNumber(daNpc_Os_c* t);
void os_setNpcAction(daNpc_Os_c* t, ProcFunc_l* f, void* p);
void os_setPlayerAction(daNpc_Os_c* t, ProcFunc_l* f, void* p);
void os_smokeSet(daNpc_Os_c* t, u16 id);
void os_setAttention(daNpc_Os_c* t, u32 b);
void os_walkProc(daNpc_Os_c* t, f32 spd, s16 ang);
u32 os_talk_init(daNpc_Os_c* t);
u32 os_talk(daNpc_Os_c* t);
u32 os_setAnm_brkAnm(daNpc_Os_c* t, s32 n);
u32 os_chkAttention(daNpc_Os_c* t, cXyz* pos, s16 ang);
u32 os_chkArea(daNpc_Os_c* t, cXyz* pos);
u32 os_getMsg(daNpc_Os_c* t);
void os_lookBack(daNpc_Os_c* t, s32 a, s32 b, s32 c);
s32 os_wallHitCheck(daNpc_Os_c* t);
s16 os_getStickAngY(daNpc_Os_c* t);
s8 os_getWakeupOrderEventNum(daNpc_Os_c* t);
s32 os_calcStickPos(daNpc_Os_c* t, s16 a, cXyz* p);
void os_setRestart(daNpc_Os_c* t, u32 n);
u32 os_LockonTruth(u32 att);
void Os_lookAtTargetHD(dNpc_JntCtrl_c* joint, be<s16>* outY, cXyz* target, cXyz* eye, s16 rotation,
                       s16 velocity, u32 headOnly);
f64 Os_stickValueHD(s32 port);
void Os_groundCheckDestructHD(void* check, s32 flag);
f64 Os_forwardGroundUnroundedHD(daNpc_Os_c* actor, s16 angle);
f64 Os_sqrtUnroundedHD(f32 value);
void Os_crossProductHD(cXyz* left, cXyz* output, cXyz* right);
f64 Os_wallJumpUnroundedHD(daNpc_Os_c* actor, s16 angle);
f64 Os_distanceXZSquaredHD(daNpc_Os_c* actor, fopAc_ac_c* player);
u32 Os_routeCheckHD(daNpc_Os_c* actor, f64 distance, be<s16>* angle);
u8 Os_calcTimerHD(be<u8>* timer);
s32 Os_randomValueHD(s32 minimum, s32 maximum);

// HD: explicit constructors below use this translation unit's vtables. Composition
// keeps the shared Local constructor hook from constructing the check twice.
struct OsGroundCheckHD {
    dBgS_GndChk mCheck;
};
struct OsLineCheckHD {
    dBgS_LinChk mCheck;
};
WWHD_SIZE(OsGroundCheckHD, 0x54);
WWHD_SIZE(OsLineCheckHD, 0x6C);
enum : u32 { Os_groupPassVtableHD = 0x1001F394, Os_polyInfoVtableHD = 0x1001F384 };

struct OsRouteActionsHD {
    ProcFunc_l mWaitNpc, mWaitPlayer, mWalkPlayer, mThrowNpc, mCarryNpc;
    ProcFunc_l mFinish01Npc, mSearchNpc, mFinish02Npc, mJumpNpc, mTalkNpc;
};
WWHD_SIZE(OsRouteActionsHD, 0x50);
struct OsRouteActionWordsHD {
    be<u32> mDeltaAndIndex, mFunction;
};
WWHD_SIZE(OsRouteActionWordsHD, 8);
inline void Os_copyRouteActionHD(ProcFunc_l* destination, const ProcFunc_l* source) {
    auto* out = reinterpret_cast<OsRouteActionWordsHD*>(destination);
    const auto* in = reinterpret_cast<const OsRouteActionWordsHD*>(source);
    out->mDeltaAndIndex = (u32)in->mDeltaAndIndex;
    out->mFunction = (u32)in->mFunction;
}
struct OsAnimationControlHD {
    u8 mBeforeControl[0x98];
    be<f32> mControlWord;
};
WWHD_OFFSET(OsAnimationControlHD, mControlWord, 0x98);
struct OsAttentionFlagsHD {
    u8 mBeforeFlags[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(OsAttentionFlagsHD, mFlags, 0x20);
