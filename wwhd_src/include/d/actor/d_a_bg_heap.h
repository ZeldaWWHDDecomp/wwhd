#pragma once
#include "d/actor/d_a_bg.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"

struct BgHeapNameTable {
    be<u8> names[4][13];
};
struct BgHeapGuard {
    be<u32> initialized;
};
struct BgHeapPlay {
    u8 mPrefix[0x5134];
    be<u8> mStartStageName[8];
};
struct BgHeapMaterial {
    u8 mPrefix[0x24];
    gptr<void> mpMaterialAnm;
    u8 mTail[0x39C - 0x28];
};
WWHD_SIZE(BgHeapMaterial, 0x39C);
struct BgHeapModelData {
    u8 mPrefix[0xC];
    be<u32> mMaterialCount;
    gptr<BgHeapMaterial> mpMaterials;
};
struct BgHeapMaterialTable {
    u8 mPrefix[0x24];
    be<u16> mMaterialNum;
};
struct BgHeapMaterialInstance {
    u8 mPrefix[0x34];
    be<u8> mFlags;
    u8 mTail[3];
};
WWHD_SIZE(BgHeapMaterialInstance, 0x38);
struct BgHeapModel {
    u8 mPrefix[0xAC];
    gptr<J3DModelData> mpModelData;
    u8 mPadding[0x138 - 0xB0];
    be<u32> mMaterialCount;
    gptr<BgHeapMaterialInstance> mpMaterials;
};
WWHD_OFFSET(BgHeapModel, mMaterialCount, 0x138);
// HD: copy the light defaults by their exact word/byte/halfword widths, including float bits.
struct BgHeapLight {
    be<u32> mVectors[6];
    be<u8> mColor[4];
    be<u16> mSettings[4];
    be<u32> mAttenuation[8];
};
WWHD_SIZE(BgHeapLight, 0x44);
struct BgHeapTev {
    BgHeapLight mLight0;
    u8 mPadding44[0xC0 - 0x44];
    BgHeapLight mLight1;
    u8 mPadding104[0x144 - 0x104];
    BgHeapLight mLight2;
    u8 mTail[0x40];
};
WWHD_SIZE(BgHeapTev, 0x1C8);
struct BgHeapRoomStatus {
    u8 mPrefix[0x228];
    gptr<dBgW> mpBgW;
};
struct BgHeapBgW {
    enum { GLOBAL_e = 0x20 };
    u8 mPrefix[0x75];
    be<u8> mPriority;
};
// HD: retain the room table's wrapping 32-bit address calculation, including room -1.
inline BgHeapRoomStatus *BgHeap_roomStatus(s32 room) {
    return gabi::at<BgHeapRoomStatus>(daBg_roomStatus(room));
}
const char *BgHeap_setArcName(daBg_c *);
BgHeapMaterialTable *BgHeap_getMaterialTable(J3DModelData *);
BgHeapPlay *BgHeap_getPlay();
void BgHeap_tevstr_init(dKy_tevstr_c *, s8, u8);
BOOL BgHeap_btkCreate(daBg_btkAnm_c *, J3DModelData *, J3DAnmTextureSRTKey *);
BOOL BgHeap_brkCreate(daBg_brkAnm_c *, J3DModelData *, J3DAnmTevRegKey *);
daBg_SafeString *BgHeap_ss_ct(daBg_SafeString *, const char *);
BOOL BgHeap_ss_eq(daBg_SafeString *, daBg_SafeString *);
void BgHeap_ss_dt(daBg_SafeString *, s32);
