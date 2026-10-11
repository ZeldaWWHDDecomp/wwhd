/* Room scenes. HD adds archive-name storage and reload state after the GameCube fields.
 * Only verified fields of room/stage/archive objects are exposed. */
#pragma once
#include "f_op/f_op_scene.h"
#include "f_pc/f_pc_node.h"
#include "SSystem/SComponent/c_phase.h"
#include "d/d_com_lib_game.h"
#include "gc_api/JSystem/JUTAssert.h"

struct RoomDt;
struct room_of_scene_class : scene_class {
    /* 0x1C8 */ request_of_phase_process_class mPhs;
    /* 0x1D0 */ gptr<void> mpRoomData;
    /* 0x1D4 */ gptr<RoomDt> mpRoomDt;
    /* 0x1D8 */ gptr<void> sceneCommand;
    /* 0x1DC */ be<u16> mPearlState;
    /* 0x1DE */ be<u8> mbHasRoomParticle;
    /* 0x1DF */ be<u8> mbReLoaded;
    /* 0x1E0 */ be<u8> mbSetMap;
    /* 0x1E1 */ char mArcName[8];
    /* 0x1E9 */ be<u8> mNotifyPlayer;
    u8 mPadding[2];
};
WWHD_OFFSET(room_of_scene_class, mPhs, 0x1C8);
WWHD_OFFSET(room_of_scene_class, mpRoomData, 0x1D0);
WWHD_OFFSET(room_of_scene_class, mpRoomDt, 0x1D4);
WWHD_OFFSET(room_of_scene_class, sceneCommand, 0x1D8);
WWHD_OFFSET(room_of_scene_class, mPearlState, 0x1DC);
WWHD_OFFSET(room_of_scene_class, mbHasRoomParticle, 0x1DE);
WWHD_OFFSET(room_of_scene_class, mbReLoaded, 0x1DF);
WWHD_OFFSET(room_of_scene_class, mbSetMap, 0x1E0);
WWHD_OFFSET(room_of_scene_class, mArcName, 0x1E1);
WWHD_OFFSET(room_of_scene_class, mNotifyPlayer, 0x1E9);
WWHD_SIZE(room_of_scene_class, 0x1EC);
struct RoomStatus {
    u8 mRoomState[0x21C];
    be<u8> mFlags;
    u8 mFlagPadding[2];
    be<s8> mZoneNo;
    u8 mZonePadding[4];
    be<u32> mProcID;
    u8 mPadding[4];
};
WWHD_OFFSET(RoomStatus, mFlags, 0x21C);
WWHD_OFFSET(RoomStatus, mZoneNo, 0x21F);
WWHD_OFFSET(RoomStatus, mProcID, 0x224);
WWHD_SIZE(RoomStatus, 0x22C);
struct RoomStatusTable { RoomStatus entries[64]; };
struct RoomControl { u8 mOpaque; };
struct RoomPlayView {
    u8 mPlayState[0x5134];
    char mStartStageName[0x18];
    be<s8> mEnableNextStage;
    u8 mStageState[0x51CC - 0x514D];
    RoomControl mRoomControl;
    u8 mRoomState[0x5AB0 - 0x51CD];
    gptr<dPa_control_c> mpParticle;
    u8 mParticleState[0x5B2C - 0x5AB4];
    gptr<fopAc_ac_c> mpPlayer;
};
WWHD_OFFSET(RoomPlayView, mStartStageName, 0x5134);
WWHD_OFFSET(RoomPlayView, mEnableNextStage, 0x514C);
WWHD_OFFSET(RoomPlayView, mRoomControl, 0x51CC);
WWHD_OFFSET(RoomPlayView, mpParticle, 0x5AB0);
WWHD_OFFSET(RoomPlayView, mpPlayer, 0x5B2C);
WWHD_SIZE(RoomPlayView, 0x5B30);
inline RoomPlayView* roomPlay() { return reinterpret_cast<RoomPlayView*>(dComIfGp_get()); }

struct RoomSafeStringMethods { u8 mMethods[0x14]; gfn mpAssureTermination; };
WWHD_OFFSET(RoomSafeStringMethods, mpAssureTermination, 0x14);
WWHD_SIZE(RoomSafeStringMethods, 0x18);
struct RoomSafeString {
    gptr<const char> mpString;
    gptr<RoomSafeStringMethods> mpMethods;
    void setString(const char* text) { mpString.v = gabi::guest_addr(text); }
    void assureTermination() { gabi::call_ptr<void>(mpMethods->mpAssureTermination.get(), this); }
};
WWHD_OFFSET(RoomSafeString, mpMethods, 4);
WWHD_SIZE(RoomSafeString, 8);
struct RoomFixedSafeString {
    gptr<char> mpString;
    gptr<RoomSafeStringMethods> mpMethods;
    be<u32> mSize;
    be<u8> mBuffer[32];
    void assureTermination() { gabi::call_ptr<void>(mpMethods->mpAssureTermination.get(), this); }
};
WWHD_OFFSET(RoomFixedSafeString, mSize, 8);
WWHD_OFFSET(RoomFixedSafeString, mBuffer, 0xC);
WWHD_SIZE(RoomFixedSafeString, 0x2C);
struct RoomLbnk { be<u32> mCount; gptr<be<u8>> mEntries; };
WWHD_OFFSET(RoomLbnk, mEntries, 4);
WWHD_SIZE(RoomLbnk, 8);
struct RoomFileList { be<u32> mFlags; };
WWHD_SIZE(RoomFileList, 4);
struct RoomDtMethods {
    u8 mDestructorMethods[0xC];
    gfn mpInit;
    u8 mStageMethods[0x1DC - 0x10];
    gfn mpGetFileListInfo;
    u8 mRoomMethods[0x21C - 0x1E0];
    gfn mpGetLbnk;
};
WWHD_OFFSET(RoomDtMethods, mpInit, 0xC);
WWHD_OFFSET(RoomDtMethods, mpGetFileListInfo, 0x1DC);
WWHD_OFFSET(RoomDtMethods, mpGetLbnk, 0x21C);
WWHD_SIZE(RoomDtMethods, 0x220);
struct RoomDt {
    gptr<RoomDtMethods> mpMethods;
    u8 mStageData[0x4C - 4];
    be<u8> mRoomNo;
    u8 mPadding[3];
    u32 init() { return gabi::call_ptr<u32>(mpMethods->mpInit.get(), this); }
    RoomLbnk* getLbnk() { return gabi::call_ptr<RoomLbnk*>(mpMethods->mpGetLbnk.get(), this); }
    RoomFileList* getFileListInfo() { return gabi::call_ptr<RoomFileList*>(mpMethods->mpGetFileListInfo.get(), this); }
};
WWHD_OFFSET(RoomDt, mRoomNo, 0x4C);
WWHD_SIZE(RoomDt, 0x50);
struct RoomPlayerState {
    u8 mPlayerState[0x30AC];
    be<u8> mRoomTransitionState;
    u8 mRoomState[0x30D0 - 0x30AD];
    be<u8> mRoomReloadState;
};
WWHD_OFFSET(RoomPlayerState, mRoomTransitionState, 0x30AC);
WWHD_OFFSET(RoomPlayerState, mRoomReloadState, 0x30D0);
WWHD_SIZE(RoomPlayerState, 0x30D1);
using RoomPlayerPtr = gptr<RoomPlayerState>;
using RoomObjectPtr = gptr<void>;
// HD archive offsets are self-relative 32-bit values; addition must wrap in guest space.
struct RoomRelativePointer {
    be<u32> mOffset;
    void* get() { u32 offset = mOffset; return offset ? gabi::at<void>(gabi::ea(this) + offset) : nullptr; }
};
WWHD_SIZE(RoomRelativePointer, 4);
struct RoomArchive { u8 mArchiveState[0x4C]; RoomRelativePointer mRoot; };
WWHD_OFFSET(RoomArchive, mRoot, 0x4C);
WWHD_SIZE(RoomArchive, 0x50);
struct RoomDemoArcName { be<u8> text[8]; };
struct RoomPhaseTable { cPhs__Handler entries[5]; };
struct RoomHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    be<f32> firstRange[2];
    be<f32> secondRange[2];
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
WWHD_OFFSET(RoomHeaderStatics, firstObject, 0x18);
WWHD_OFFSET(RoomHeaderStatics, zero, 0x1C);
WWHD_SIZE(RoomHeaderStatics, 0x2C);
struct RoomConstants { be<f32> negativePi; be<f32> positivePi; be<f32> firstRange; be<f32> secondRange; };
WWHD_SIZE(RoomConstants, 0x10);

enum RoomFlag { RoomFlag_ACTIVE = 1, RoomFlag_RELOAD = 2, RoomFlag_DELETE = 4, RoomFlag_HIDDEN = 8 };
enum { Room_BG_PROCESS = 0x1B7, Room_DRAGON_ROOST = 13, Room_NO_DEMO_BANK = 0xFF,
       Room_ITEM_PEARL_DIN = 0x6A, Room_ITEM_PEARL_FARORE = 0x6B };
enum RoomEvent { RoomEvent_DIN_LEAVE = 0x1A80, RoomEvent_FARORE_LEAVE = 0x1C40,
                 RoomEvent_DRI_REGISTER = 0xB8FF, RoomEvent_DRI_COMPLETE = 0x3F02,
                 RoomEvent_DRI_ENABLE = 0x1580, RoomEvent_SEA_TALK_TMP = 0x320 };
void fopScnM_DeleteReq(room_of_scene_class*);
void Room_formatArchiveName(char*, u32, const char*, u32);
BOOL dComIfG_setStageRes(char*, u32);
s32 dComIfG_syncStageRes(char*);
void dComIfG_deleteStageRes(char*);
void* dComIfG_getStageRes(char*, const char*);
void dStage_escapeRestart();
void Room_createBackground(u32,u32,u32,s32,u32,u32,s32,u32);
void dStage_dt_c_roomReLoader(void*, RoomDt*, u32);
void dStage_dt_c_roomLoader(void*, RoomDt*);
void RoomAudio_sceneChange(void*);
void RoomPlayer_enterRoom(RoomPlayerState*, u32);
void RoomPlayer_finishRoom(RoomPlayerState*, u32);
void* Room_layerJudge(void*, u32, u32);
void dSv_zoneBit_clearRoomSwitch(dSv_zoneBit_c*);
u32 dPa_control_removeRoomScene(dPa_control_c*, u32, u32);
void dComIfGp_map_deleteImage(u32);
RoomDt* dStage_roomControl_getStatusRoomDt(RoomControl*, u32);
u32 dSv_info_createZone(dSv_info_c*, u32);
u32 dComIfG_play_getLayerNo(u32);
u32 dComIfG_setObjectRes(char*,u32,u32);
s32 dComIfG_syncObjectRes(char*);
u32 dPa_control_readScene(dPa_control_c*,u32,gptr<void>*);
void* RoomResourceManager_getResource(void*, RoomSafeString*, RoomSafeString*, u32);
RoomArchive* RoomResource_getArchive(void*);
void RoomFixedSafeString_format(RoomFixedSafeString*, const char*, u32);
RoomRelativePointer* RoomArchive_findResource(void*, const char*);
void dPa_control_createRoomScene(dPa_control_c*,void*);
void RoomFirstHeaderObject_ct(u8*);
void RoomSecondHeaderObject_ct(u8*);

void Room_objectSetCheck(room_of_scene_class*);
enum : u32 { Room_deleteJudge = 0x025B37C8 };

void dSv_event_offEventBit(dSv_event_c*, u32);
