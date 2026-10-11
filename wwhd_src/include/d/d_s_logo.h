/* Logo scene: scene_class is four bytes larger than GC, placing mPhs at 0x1C8.
 * Only the scene prefix accessed by these verified functions is modeled. The other layouts are
 * partial HD views; no allocation uses their sizes. Free adapters keep the verified register ABI
 * of HD library/static calls, including calls with no explicit heap argument. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
WWHD_SIZE(SafeString, 8);
u32 dScnLogo_getSystemControlHD();
void dScnLogo_setSystemControlEnabledHD(u32, u32);
void dScnLogo_createMiiverseBottleMgrHD();
void dScnLogo_initializeMiiverseBottleMgrHD(u32);
void dScnLogo_startMiiverseBottleMgrHD(u32);
void dScnLogo_createPlatformMgrHD();
void dScnLogo_initializePlatformMgrHD(u32);
void dScnLogo_startPlatformMgrHD(u32);
u32 dScnLogo_getSystemHeapHD();
u32 dScnLogo_getUIHeapHD();
u32 dComIfGp_getGameInfoHD();
u32 dComIfG_setObjectResHD(u32, u32, u32);
s32 dComIfG_syncObjectResHD(u32);
u32 dComIfG_syncAllObjectResHD();
void dComIfGp_createParticleHD(u32);
u32 dComLbG_PhaseHandlerHD(u32, u32, u32);
void mDoExt_getMesgFontHD();
void mDoExt_getRubyFontHD();
void dPa_control_createCommonHD(u32, u32);
void dADM_SetDataHD(u32, u32);
u32 mDoAud_checkFirstWavesHD();
void mDoExt_setSafeGameHeapSizeHD();
void mDoExt_setSafeZeldaHeapSizeHD();
void mDoExt_setSafeArchiveHeapSizeHD();
u32 mDoExt_getCommandHeapHD();
void mDoExt_setSafeCommandHeapSizeHD();
void mDoGph_startFadeInHD(u32, u32);
void mDoGph_waitBlankingHD(u32);
void mDoGph_setTickRateHD(u32);
u32 dRes_control_getResHD(u32, SafeString*, u32);
u32 dScnLogo_loadResourceHD(u32, SafeString*, SafeString*, u32);
void dScnLogo_createSwkbdMgrHD();
void dScnLogo_initializeSwkbdMgrHD(u32);
void dScnLogo_initializeMessageDataHD(u32);
void dScnLogo_initializeMessageControlHD();
void dScnLogo_beginSaveSessionHD(u32, u32);
u32 dScnLogo_openSaveFileHD(u32);
void JUTAssertion_showAssertHD(u32, u32, u32);
void operator_deleteHD(u32);
u32 dScnLogo_createHeapHD(u32, SafeString*, u32, u32, u32);
u32 dScnLogo_getHeapHeaderSizeHD(u32);
u32 dScnLogo_findArchiveEntryHD(u32, u32);
u32 dScnLogo_getResourceFileHD();
void dScnLogo_dumpHeapHD(u32);
void dScnLogo_initializeAngleTableHD(u32);
void dScnLogo_initializeMathTableHD(u32);
void dScnLogo_registerGlobalObjectHD(u32);
u32 OSGetSystemInfoHD();

// HD: partial layouts for the HD-only bootstrap objects touched by this scene.
#include "f_op/f_op_scene.h"
#include "SSystem/SComponent/c_phase.h"
struct dScnLogo_c : scene_class {
    /* 0x1C8 */ request_of_phase_process_class mPhs;
};
WWHD_OFFSET(dScnLogo_c, mPhs, 0x1C8);
WWHD_SIZE(dScnLogo_c, 0x1D0);
struct dScnLogo_HeapVtableHD {
    u8 _00[0x2C];
    /* 0x2C */ gfn destroy;
    u8 _30[0x6C - 0x30];
    /* 0x6C */ gfn dump;
};
WWHD_OFFSET(dScnLogo_HeapVtableHD, destroy, 0x2C);
WWHD_OFFSET(dScnLogo_HeapVtableHD, dump, 0x6C);
WWHD_SIZE(dScnLogo_HeapVtableHD, 0x70);
struct dScnLogo_HeapHD {
    u8 _00[0xC];
    /* 0xC */ gptr<dScnLogo_HeapVtableHD> mVtable;
};
WWHD_OFFSET(dScnLogo_HeapHD, mVtable, 0xC);
WWHD_SIZE(dScnLogo_HeapHD, 0x10);
struct dScnLogo_ResourceControlHD {
    u8 _0000[0x2040];
    /* 0x2040 */ gptr<dScnLogo_HeapHD> mHeap;
};
WWHD_OFFSET(dScnLogo_ResourceControlHD, mHeap, 0x2040);
WWHD_SIZE(dScnLogo_ResourceControlHD, 0x2044);
struct dScnLogo_SaveMgrHD {
    u8 _00[0x1C];
    /* 0x1C */ be<u32> mThreadState;
    /* 0x20 */ be<u32> mResult;
};
WWHD_OFFSET(dScnLogo_SaveMgrHD, mThreadState, 0x1C);
WWHD_OFFSET(dScnLogo_SaveMgrHD, mResult, 0x20);
WWHD_SIZE(dScnLogo_SaveMgrHD, 0x24);
struct dScnLogo_GameInfoHD {
    u8 _0000[0x12A0];
    /* 0x12A0 */ u8 mPlay[0x5098 - 0x12A0];
    /* 0x5098 */ u8 mActorData[0x5AB0 - 0x5098];
    /* 0x5AB0 */ be<u32> mParticle;
    u8 _5AB4[0x5AC9 - 0x5AB4];
    /* 0x5AC9 */ be<u8> mWindowNum;
    u8 _5ACA[0x5BE5 - 0x5ACA];
    /* 0x5BE5 */ be<u8> mPictureFlag;
    u8 _5BE6[0x5D24 - 0x5BE6];
    /* 0x5D24 */ be<u32> mpItemTable;
    /* 0x5D28 */ be<u32> mpFmapData;
};
WWHD_OFFSET(dScnLogo_GameInfoHD, mPlay, 0x12A0);
WWHD_OFFSET(dScnLogo_GameInfoHD, mActorData, 0x5098);
WWHD_OFFSET(dScnLogo_GameInfoHD, mParticle, 0x5AB0);
WWHD_OFFSET(dScnLogo_GameInfoHD, mWindowNum, 0x5AC9);
WWHD_OFFSET(dScnLogo_GameInfoHD, mPictureFlag, 0x5BE5);
WWHD_OFFSET(dScnLogo_GameInfoHD, mpItemTable, 0x5D24);
WWHD_OFFSET(dScnLogo_GameInfoHD, mpFmapData, 0x5D28);
WWHD_SIZE(dScnLogo_GameInfoHD, 0x5D2C);
struct dScnLogo_ResourceFileHD {
    u8 _00[0x4C];
    /* 0x4C */ be<u32> mFileDictionaryOffset;
};
WWHD_OFFSET(dScnLogo_ResourceFileHD, mFileDictionaryOffset, 0x4C);
WWHD_SIZE(dScnLogo_ResourceFileHD, 0x50);

// HD: the two System texture indices used by this scene's resource manager.
enum { dRes_INDEX_SYSTEM_BTI_TOON_e = 3, dRes_INDEX_SYSTEM_BTI_TOONEX_e = 4 };
struct dScnLogo_ResetStateHD {
    /* 0x0 */ be<u32> mReset;
    /* 0x4 */ be<u32> mResetPrepare;
};
WWHD_OFFSET(dScnLogo_ResetStateHD, mReset, 0);
WWHD_OFFSET(dScnLogo_ResetStateHD, mResetPrepare, 4);
WWHD_SIZE(dScnLogo_ResetStateHD, 8);
