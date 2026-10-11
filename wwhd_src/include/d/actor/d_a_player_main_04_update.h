// Link collision and frame update support. Private views of HD-only storage.
#pragma once
#include "d/actor/d_a_player_main_04_item.h"

struct LinkUpdateApi {
    static void endRipple(void*);
    static void endWaterDrop(void*);
    static void removeSwimTail(void*);
    static void endCutTurn(void*);
    static void endDamageEmitter(daPy_lk_c*);
    static void stopAnimeSound(void*);
    static void swapTexture(u32, void*);
    static void fadeOut(const GXColor*, u32, f32);
    static BOOL checkBottle(void*, u32);
    static s32 getForestWaterTimer(void*);
    static void setBottleItemIn(void*, u32, u32);
    static void setEventReg(void*, u32, u32);
    static void seStartSystem(u32);
    static void calcByteTimer(be<u8>*);
    static BOOL ChkPolySafe(dBgS*, cBgS_PolyInfo*);
    static void MoveBgCrrPos(dBgS*, cBgS_PolyInfo*, u32, cXyz*, csXyz*, csXyz*);
    static void MoveBgTransPos(dBgS*, cBgS_PolyInfo*, u32, cXyz*, csXyz*, csXyz*);
    static u32 GetGroundCode(dBgS*, cBgS_PolyInfo*);
    static u32 GetAttributeCode(dBgS*, cBgS_PolyInfo*);
    static s32 GetSpecialCode(dBgS*, cBgS_PolyInfo*);
    static cXyz* GetTriPla(dBgS*, u32, u32);
    static void RoomCheck(dBgS_GndChk*);
    static u32 GetMtrlSndId(dBgS*, cBgS_PolyInfo*);
    static u8 GetGrpSoundId(dBgS*, cBgS_PolyInfo*);
    static u32 convAttentionPId(void*, u32);
    static BOOL orderEvent(dEvt_control_c*, u32, u32, u32, u32, daPy_lk_c*, u8*, s32, u32);
    static void stopDoButtonQuake(daPy_lk_c*, s32);
    static void addCalcPosXZ(cXyz*, const cXyz*, f32, f32, f32);
    static BOOL checkDemoSound();
    static void assureSoundName(void*);
    static void setAnimSound(void*, cXyz*, f32, f32, u32, s32);
    static void setLinkGroupInfo(u32);
    static void setLinkOnBoard(u32);
    static void setLinkHp(u32, u32);
    static fopAc_ac_c* convEventPId(dEvt_control_c*, u32);
    static void decOldFrameMorfCounter(void*);
    static void getBokoTopPos(fopAc_ac_c*, cXyz*);
    static void getBokoBlurRootPos(fopAc_ac_c*, cXyz*);
    static void setEmitterMatrix(const Mtx34*, void*, cXyz*);
};

struct LinkUpdateFanSwingCallback {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u32> mFlags;
    /* 0x08 */ gptr<JPABaseEmitter> mpEmitter;
};
WWHD_OFFSET(LinkUpdateFanSwingCallback, mpEmitter, 8);
WWHD_SIZE(LinkUpdateFanSwingCallback, 0xC);

// HD: emitter callback slots and flags belong to the renderer tail.
struct LinkUpdateEmitter {
    /* 0x000 */ u8 _000[0x1E4];
    /* 0x1E4 */ gptr<void> mpEmitterCallback;
    /* 0x1E8 */ u8 _1E8[0x6C];
    /* 0x254 */ be<u32> mFlags;
};
WWHD_OFFSET(LinkUpdateEmitter, mpEmitterCallback, 0x1E4);
WWHD_OFFSET(LinkUpdateEmitter, mFlags, 0x254);
WWHD_SIZE(LinkUpdateEmitter, 0x258);

struct LinkUpdateLevelCallback {
    be<u32> __vtbl;
};
WWHD_SIZE(LinkUpdateLevelCallback, 4);
inline void LinkUpdate_removeCallback(void* callback) {
    auto* level = reinterpret_cast<LinkUpdateLevelCallback*>(callback);
    gabi::call_ptr(gabi::load<u32>(level->__vtbl + 0x44), callback);
}

struct LinkUpdateItemHeap {
    /* 0x0000 */ u8 _0000[0x4808];
    /* 0x4808 */ gptr<JKRSolidHeap> mpItemAnimeHeap;
};
WWHD_OFFSET(LinkUpdateItemHeap, mpItemAnimeHeap, 0x4808);
WWHD_SIZE(LinkUpdateItemHeap, 0x480C);

// HD: frame lifetime and event-order state have no GameCube counterparts.
struct LinkUpdateTail {
    /* 0x00 */ be<s32> mFrameCounter;
    /* 0x04 */ be<u8> mEventOrder;
    /* 0x05 */ be<u8> mEventOrderAccepted;
    /* 0x06 */ u8 _06[0x1C];
    /* 0x22 */ be<u8> mConfuseStatus;
    /* 0x23 */ u8 _23;
};
WWHD_OFFSET(LinkUpdateTail, mConfuseStatus, 0x22);
WWHD_SIZE(LinkUpdateTail, 0x24);
inline LinkUpdateTail& LinkUpdate(daPy_lk_c* player) {
    return *reinterpret_cast<LinkUpdateTail*>(player->m8260);
}

struct LinkUpdateHeavyState {
    /* 0x0000 */ u8 _0000[0x69EE];
    /* 0x69EE */ be<s16> mHeavyStateCounter;
};
WWHD_OFFSET(LinkUpdateHeavyState, mHeavyStateCounter, 0x69EE);
WWHD_SIZE(LinkUpdateHeavyState, 0x69F0);

struct LinkUpdateSoundFrame {
    be<f32> mRate, mFrame;
};
WWHD_SIZE(LinkUpdateSoundFrame, 8);
struct LinkUpdateSmokeCallback {
    be<u32> __vtbl;
    gptr<JPABaseEmitter> mpEmitter;
};
WWHD_OFFSET(LinkUpdateSmokeCallback, mpEmitter, 4);
WWHD_SIZE(LinkUpdateSmokeCallback, 8);

struct LinkUpdateModelLayout {
    u8 _0000[0x497C];
    gptr<J3DModel> mpSwordTipStabModel;
};
WWHD_OFFSET(LinkUpdateModelLayout, mpSwordTipStabModel, 0x497C);
WWHD_SIZE(LinkUpdateModelLayout, 0x4980);
inline LinkUpdateModelLayout& LinkUpdateModels(daPy_lk_c* player) {
    return gabi::object_at<LinkUpdateModelLayout>(gabi::ea(player));
}

// HD: animation sound names use an offset from their resource's name entry.
struct LinkUpdateSoundNameEntry {
    be<u32> mFlags;
    be<u32> mNameOffset;
};
struct LinkUpdateSoundResource {
    u8 _00[8];
    gptr<LinkUpdateSoundNameEntry> mpNameEntry;
};
WWHD_SIZE(LinkUpdateSoundNameEntry, 8);
WWHD_OFFSET(LinkUpdateSoundNameEntry, mNameOffset, 4);
WWHD_SIZE(LinkUpdateSoundResource, 0xC);
WWHD_OFFSET(LinkUpdateSoundResource, mpNameEntry, 8);

namespace LinkUpdateConstants {
enum : u32 {
    ModeFlg_MIDAIR = 2,
    ModeFlg_HANG = 0x20,
    ModeFlg_IN_SHIP = 0x2000,
    ModeFlg_SWIM = 0x40000,
    ModeFlg_ROPE = 0x800,
    ModeFlg_CLIMB = 0x10000,
    ModeFlg_LADDER = 0x400000,
    ModeFlg_CAUGHT = 0x10000000,
    ModeFlg_00000100 = 0x100,
    ModeFlg_02000000 = 0x2000000,
    ModeFlg_SUBJECT = 0x20000000,
    daPyStts0_SUBJECT_e = 0x2000,
    daPyStts0_UNK1000000_e = 0x1000000,
    daPyStts1_PICTO_BOX_AIM_e = 8,
    daPyStts1_UNK80_e = 0x80,

    dItemNo_GRAPPLING_HOOK_e = 0x25,
    ModeFlg_40000000 = 0x40000000,
    PlayerFlyModeMask = ModeFlg_MIDAIR | ModeFlg_HANG | ModeFlg_ROPE | ModeFlg_IN_SHIP |
                        ModeFlg_CLIMB | ModeFlg_SWIM | ModeFlg_LADDER | ModeFlg_CAUGHT,
    daPyProc_HANG_START_e = 0x2B,
    daPyProc_HANG_MOVE_e = 0x2F,
    daPyProc_CUT_F_e = 0x42,
    daPyProc_BT_VERTICAL_JUMP_CUT_e = 0x63,
    daPyProc_DAMAGE_e = 0x66,
    daPyProc_ROPE_SUBJECT_e = 0x76,
    daPyProc_ROPE_MOVE_e = 0x7D,
    daPyProc_HOOKSHOT_FLY_e = 0x85,
    daPyProc_SHIP_READY_e = 0x86,
    daPyProc_SHIP_GET_OFF_e = 0x90,
    daPyProc_FAN_SWING_e = 0x92,
    daPyProc_FAN_GLIDE_e = 0x93,
    daPyProc_BOTTLE_SWING_e = 0xA5,
    daPyProc_DEMO_TOOL_e = 0xA9,
    daPyProc_DEMO_LAVA_DAMAGE_e = 0xB0,
    daPyProc_DEMO_FREEZE_DAMAGE_e = 0xB1,
    daPyProc_DEMO_DEAD_e = 0xB2,
    daPyProc_DEMO_DOOR_OPEN_e = 0xC1,
};
}

// HD: the animation sound resource is referenced by this measured storage slot.
// The shared collider declaration overlaps it; the sound-name path uses pointer semantics.
struct LinkUpdateSoundLayout {
    u8 _0000[0x77FC];
    gptr<LinkUpdateSoundResource> mpAnimationSoundResource;
};
WWHD_OFFSET(LinkUpdateSoundLayout, mpAnimationSoundResource, 0x77FC);
WWHD_SIZE(LinkUpdateSoundLayout, 0x7800);
inline LinkUpdateSoundLayout& LinkUpdateSound(daPy_lk_c* player) {
    return gabi::object_at<LinkUpdateSoundLayout>(gabi::ea(player));
}
// GHS angle assignment loads and stores each halfword in x/y/z order.
inline void LinkUpdate_copyAngles(csXyz& dst, const csXyz& src) {
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
}
