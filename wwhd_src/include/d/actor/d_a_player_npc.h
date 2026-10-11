// Player NPC: GameCube base members grow by 0x11C in HD; allocation 0x608.
#pragma once
#include "bindings.h"
#include "d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"
struct PlayerNpcVector { be<u32> x, y, z; };
WWHD_SIZE(PlayerNpcVector, 12);
struct judge_parm_c {
    be<s16> mProcName; u8 padding[2];
    PlayerNpcVector mAreaCenter, mActorRelPos;
    be<f32> mAreaRadius, mMinDist;
    be<u32> mpActor;
};
WWHD_SIZE(judge_parm_c, 0x28);
WWHD_OFFSET(judge_parm_c, mAreaCenter, 4);
WWHD_OFFSET(judge_parm_c, mActorRelPos, 0x10);
WWHD_OFFSET(judge_parm_c, mAreaRadius, 0x1C);
WWHD_OFFSET(judge_parm_c, mMinDist, 0x20);
WWHD_OFFSET(judge_parm_c, mpActor, 0x24);
enum PlayerNpcProcessName { fpcNm_Obj_Movebox_e = 0x2B };
struct daPy_npc_c : daPy_py_c {
    /* 0x43C */ dBgS_ObjAcch mAcch;
    /* 0x600 */ be<u32> m4E4;
    /* 0x604 */ be<u8> m4E8;
    /* 0x605 */ be<u8> mDamageFogTimer;
    /* 0x606 */ u8 padding[2];
    s32 check_initialRoom();
    s32 check_moveStop();
    void unconditionalSetRestart(s8 option);
    void setRestart(s8 option);
    void setOffsetHomePos();
    void setPointRestart(s16 point, s8 option);
    s32 checkRestart(s8 option);
    s32 initialRestartOption(s8 option, s32 shouldSave);
    s32 checkNowPosMove(const char* name);
    void drawDamageFog();
    s32 chkMoveBlock(PlayerNpcVector* output);
    s32 getTactMusic();
    s32 getTactTimerCancel();
    s32 checkPlayerGuard();
    s32 getGrabMissActor();
    s32 checkPlayerFly();
    s32 checkFrontRoll();
    s32 checkBottleSwing();
    s32 checkCutCharge();
    s32 getBokoFlamePos();
    s32 checkTactWait();
    void setTactZev();
    s32 checkComboCutTurn();
    s32 getItemID();
    s32 getThrowBoomerangID();
    s32 getGrabActorID();
    s32 checkGrabBarrel();
    s32 checkPlayerNoDraw();
    s32 checkRopeTag();
    s32 checkRopeReadyAnime();
    void voiceStart();
    void setOutPower();
    void onFrollCrashFlg();
    s32 setHookshotCarryOffset();
    void setPlayerPosAndAnglePosition();
    void setPlayerPosAndAngleRotation();
    void setPlayerPosAndAngleMatrix();
    s32 setThrowDamage();
    s32 isTagCheckOK();
    void setMessageAnimation();
};
WWHD_SIZE(daPy_npc_c, 0x608);
WWHD_OFFSET(daPy_npc_c, mAcch, 0x43C);
WWHD_OFFSET(daPy_npc_c, m4E4, 0x600);
WWHD_OFFSET(daPy_npc_c, m4E8, 0x604);
WWHD_OFFSET(daPy_npc_c, mDamageFogTimer, 0x605);
// HD: raw word views retain packed place copies and signaling-NaN payloads.
template<class T, class Field> inline be<T>& npcWord(Field& field) {
    return *gabi::at<be<T>>(gabi::ea(&field));
}
inline daPy_npc_c* npcActor(u32 a) { return gabi::at<daPy_npc_c>(a); }
inline PlayerNpcVector* npcVector(u32 a) { return gabi::at<PlayerNpcVector>(a); }
inline judge_parm_c* npcJudge(u32 a) { return gabi::at<judge_parm_c>(a); }
inline dSv_info_c* npcSaveView(u32 a) { return gabi::at<dSv_info_c>(a + 0x20); }
inline dSv_info_c* npcSave() { return dComIfGs_info(); }
inline dComIfG_inf_c* npcGame(u32 a) { return gabi::at<dComIfG_inf_c>(a); }
inline dComIfG_inf_c* npcPlay() { return npcGame(dComIfGp_ea()); }
struct PlayerNpcStage {
    gptr<be<u32>> vtable;
    u32 getPlayer() { return gabi::call_ptr<u32>(vtable.get()[0x44/4], this); }
    u32 getPlayerNum() { return gabi::call_ptr<u32>(vtable.get()[0x54/4], this); }
    u32 getSclsInfo() { return gabi::call_ptr<u32>(vtable.get()[0x16C/4], this); }
};
WWHD_SIZE(PlayerNpcStage, 4);
inline PlayerNpcStage* npcStage() { return gabi::at<PlayerNpcStage>(gabi::ea(&npcPlay()->play.mStageData)); }
struct PlayerNpcScene { be<s32> num; be<u32> entries; };
WWHD_SIZE(PlayerNpcScene, 8);
struct PlayerNpcSceneEntry { u8 name[8]; be<u8> mStart; u8 padding[3]; };
WWHD_SIZE(PlayerNpcSceneEntry, 12);
WWHD_OFFSET(PlayerNpcSceneEntry, mStart, 8);
struct PlayerNpcPlayerInfo { be<s32> num; be<u32> entries; };
WWHD_SIZE(PlayerNpcPlayerInfo, 8);
struct PlayerNpcPlayerEntry {
    u8 name[8]; be<u32> parameters; PlayerNpcVector position;
    csXyz angle; u8 padding[2];
    be<u8>& startCode() { return *gabi::at<be<u8>>(gabi::ea(&angle.z)+1); }
};
WWHD_SIZE(PlayerNpcPlayerEntry, 0x20);
WWHD_OFFSET(PlayerNpcPlayerEntry, position, 0xC);
WWHD_OFFSET(PlayerNpcPlayerEntry, angle, 0x18);
struct PlayerNpcPartnerManager { u8 padding[0x218]; be<u32> mpPartner; };
WWHD_SIZE(PlayerNpcPartnerManager, 0x21C);
WWHD_OFFSET(PlayerNpcPartnerManager, mpPartner, 0x218);
struct PlayerNpcRoomStatus { be<u8> mFlags; u8 padding[0x22B]; };
WWHD_SIZE(PlayerNpcRoomStatus, 0x22C);
struct PlayerNpcSine { be<f32> sin, cos; };
WWHD_SIZE(PlayerNpcSine, 8);
inline PlayerNpcRoomStatus* npcRoomStatus(u32 a) { return gabi::at<PlayerNpcRoomStatus>(a); }
inline PlayerNpcRoomStatus* npcRoomStatusAt(s32 room) { return npcRoomStatus(0x1047E8E8u + static_cast<u32>(room)*sizeof(PlayerNpcRoomStatus)); }
inline PlayerNpcSine* npcSine(u32 index) { return gabi::at<PlayerNpcSine>(0x104A44F8u + index*sizeof(PlayerNpcSine)); }
inline PlayerNpcScene* npcScene(u32 a) { return gabi::at<PlayerNpcScene>(a); }
inline PlayerNpcSceneEntry* npcSceneEntry(u32 a) { return gabi::at<PlayerNpcSceneEntry>(a); }
inline PlayerNpcPlayerInfo* npcPlayerInfo(u32 a) { return gabi::at<PlayerNpcPlayerInfo>(a); }
inline PlayerNpcPlayerEntry* npcPlayerEntry(u32 a) { return gabi::at<PlayerNpcPlayerEntry>(a); }
inline PlayerNpcPartnerManager* npcPartnerManager(u32 a) { return gabi::at<PlayerNpcPartnerManager>(a); }
void npcAssertion(const char*, u32, const char*);
inline void npcAssert(u32 line, const char* expression) { npcAssertion("d_a_player_npc.cpp", line, expression); }
inline void* npcGuestPointer(u32 a) { return gabi::at<void>(a); }
void* daPy_npc_JudgeForPNameAndDistance(void*, judge_parm_c*);
void* daPy_npc_SearchAreaByName(void*, s16, f32, PlayerNpcVector*);
daPy_npc_c* daPy_npc_Construct(daPy_npc_c*);
void daPy_npc_Destruct(daPy_npc_c*, s32);
// HD: named slice bindings preserve full guest argument and return-register widths.
void npcVectorSubtract(u32, u32, u32);
f64 npcVectorSquareMag(u32);
f64 npcSqrt(f64);
void npcActorJudge(u32, u32);
u32 npcAllocate(u32);
void npcPlayerConstruct(u32);
void npcAcchConstruct(u32);
void npcAcchDestroy(u32, u32);
void npcPlayerDestroy(u32, u32);
void npcDelete(u32);
void npcAcchCrrPos(u32, u32);
s32 npcGroundCode(u32, u32);
s32 npcGroundRoom(u32, u32);
void npcSetRestartOption(u32, s8);
void npcSetPlayerPriest(u32, u32, u32, s16, s32);
void npcUnconditionalRestartSlice(u32, s8);
void npcOffsetPos(u32, u32, s16, u32);
void npcOffsetHomeSlice(u32);
void npcSaveRestart(u32, s8, u32, s16, s32);
void npcSetStageLayer(u32);
void npcPointRestartSlice(u32, s16, s8);
void npcNotifyPartner(u32);
s32 npcMyStaffId(u32, u32, u32, u32);
void npcPosToCamera(u32, u32);
u32 npcSearchAreaSlice(u32, s16, f32, u32);
void npcNormalize(u32, u32);
f64 npcDot(u32, u32);
void npcCross(u32, u32, u32);
void npcRegisterGlobal(u32);
void npcInitHeaderFlag(u32);
void npcInitHeaderByte(u32);
