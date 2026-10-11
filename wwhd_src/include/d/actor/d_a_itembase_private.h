/* Private item-base HD views. GameCube item/animation names are retained;
 * HD adds BFRES texture records, material packets and the player's frame
 * helper.  Shared daItemBase_c declarations remain
 * unchanged. */
#pragma once
#include "d/actor/d_a_itembase.h"

namespace item_base {
using namespace gabi;
enum {
  dItemNo_ARROW_20_e = 0x11,
  dItemNo_ARROW_30_e = 0x12,
  dItemNo_SMALL_KEY_e = 0x15,
  dItemNo_VHO_HD_e = 0x77,
  dItemNo_BOMB_BAG_e = 0x31,
  dItemNo_SKULL_HAMMER_e = 0x33,
};
template <class T> static T *view(u32 address) { return gabi::at<T>(address); }
template <class T, class U> static T *view(U *pointer) {
  return view<T>(gabi::ea(pointer));
}
// GHS address addition wraps before a subsequent null-pointer test.
template <class T> static T *element(T *base, u32 index) {
  return view<T>(gabi::ea(base) + index * (u32)sizeof(T));
}
using WordHD = be<u32>;
using ByteHD = be<u8>;
struct dItem_data_item_resource {
  /* 0x00 */ gptr<const char> mArcname;
  /* 0x04 */ u8 reserved[4];
  /* 0x08 */ be<s16> mBmdIdx, mSrtIdx, mSrtIdx2, mTevIdx, mTevIdx2, mBckIdx;
  /* 0x14 */ be<s8> mTevFrm;
  /* 0x15 */ u8 tail[0x24 - 0x15];
};
struct dItem_data_field_item_res {
  /* 0x00 */ gptr<const char> mArc;
  /* 0x04 */ be<s16> mBmdIdx, mSrtIdx, mSrtIdx2, mTevIdx, mTevIdx2, mBckIdx;
  /* 0x10 */ u8 reserved[8];
  /* 0x18 */ be<u16> mHeapSize;
  /* 0x1A */ u8 tail[2];
};
struct dItem_data_item_info {
  /* 0x00 */ be<u8> mShadowSize, mCollisionH, mCollisionR, mFlag;
};
struct ItemResourcesHD {
  dItem_data_item_resource items[256];
};
struct FieldItemResourcesHD {
  dItem_data_field_item_res items[256];
};
struct ItemInfosHD {
  dItem_data_item_info items[256];
};
WWHD_OFFSET(dItem_data_item_resource, mBmdIdx, 8);
WWHD_OFFSET(dItem_data_item_resource, mTevFrm, 0x14);
WWHD_SIZE(dItem_data_item_resource, 0x24);
WWHD_OFFSET(dItem_data_field_item_res, mBmdIdx, 4);
WWHD_OFFSET(dItem_data_field_item_res, mHeapSize, 0x18);
WWHD_SIZE(dItem_data_field_item_res, 0x1C);
WWHD_OFFSET(dItem_data_item_info, mCollisionH, 1);
WWHD_OFFSET(dItem_data_item_info, mCollisionR, 2);
WWHD_SIZE(dItem_data_item_info, 4);
WWHD_SIZE(ItemResourcesHD, 0x2400);
WWHD_SIZE(FieldItemResourcesHD, 0x1C00);
WWHD_SIZE(ItemInfosHD, 0x400);

struct FrameHelperHD {
  /* 0x00 */ be<f32> frame, start, end;
  /* 0x0C */ u8 reserved[4];
  /* 0x10 */ be<u32> normalize;
  /* 0x14 */ be<u32> argument;
};
struct PlayerBombHD {
  /* 0x0000 */ u8 prefix[0x44D8];
  /* 0x44D8 */ be<f32> frame;
  /* 0x44DC */ u8 reserved[8];
  /* 0x44E4 */ gptr<be<f32>> anmFrame;
  /* 0x44E8 */ u8 gap[0xC];
  /* 0x44F4 */ gptr<FrameHelperHD> helper;
};
WWHD_OFFSET(PlayerBombHD, frame, 0x44D8);
WWHD_OFFSET(PlayerBombHD, anmFrame, 0x44E4);
WWHD_OFFSET(PlayerBombHD, helper, 0x44F4);
WWHD_SIZE(PlayerBombHD, 0x44F8);
WWHD_OFFSET(FrameHelperHD, frame, 0);
WWHD_OFFSET(FrameHelperHD, start, 4);
WWHD_OFFSET(FrameHelperHD, end, 8);
WWHD_OFFSET(FrameHelperHD, normalize, 0x10);
WWHD_OFFSET(FrameHelperHD, argument, 0x14);
WWHD_SIZE(FrameHelperHD, 0x18);
struct PlayDrawListsHD {
  /* 0x0000 */ u8 prefix[0x5B34];
  /* 0x5B34 */ gptr<PlayerBombHD> player;
  /* 0x5B38 */ u8 gap[0x5D58 - 0x5B38];
  /* 0x5D58 */ WordHD opaP1;
  /* 0x5D5C */ u8 gapP1[4];
  /* 0x5D60 */ WordHD xluP1;
  /* 0x5D64 */ u8 gapMask[0x20];
  /* 0x5D84 */ WordHD opaMaskOff, xluMaskOff;
  /* 0x5D8C */ u8 gapRupee[8];
  /* 0x5D94 */ WordHD xluRupee;
};
WWHD_OFFSET(PlayDrawListsHD, player, PLAY_PLAYERPTR);
WWHD_OFFSET(PlayDrawListsHD, opaP1, 0x5D58);
WWHD_OFFSET(PlayDrawListsHD, xluP1, 0x5D60);
WWHD_OFFSET(PlayDrawListsHD, opaMaskOff, 0x5D84);
WWHD_OFFSET(PlayDrawListsHD, xluMaskOff, 0x5D88);
WWHD_OFFSET(PlayDrawListsHD, xluRupee, 0x5D94);
WWHD_SIZE(PlayDrawListsHD, 0x5D98);
static PlayDrawListsHD *play() { return view<PlayDrawListsHD>(dComIfGp_ea()); }
struct JointHD {
  /* 0x00 */ u8 prefix[0x14];
  /* 0x14 */ WordHD mMtxCalc;
};
WWHD_OFFSET(JointHD, mMtxCalc, 0x14);
WWHD_SIZE(JointHD, 0x18);
struct SafeStringVtableHD {
  WordHD slots[6];
};
struct SafeStringHD {
  /* 0x00 */ gptr<const be<u8>> string;
  /* 0x04 */ gptr<SafeStringVtableHD> vtable;
};
WWHD_OFFSET(SafeStringHD, vtable, 4);
WWHD_SIZE(SafeStringHD, 8);
WWHD_SIZE(SafeStringVtableHD, 0x18);
union TextureEntryHD {
  WordHD words[9];
  struct {
    /* 0x00 */ u8 reserved[2];
    /* 0x02 */ be<u16> height, width;
    /* 0x06 */ u8 gap[2];
    /* 0x08 */ be<u8> format;
    /* 0x09 */ u8 gapImage[3];
    /* 0x0C */ WordHD imageOffset;
    /* 0x10 */ u8 gapPalette[0xC];
    /* 0x1C */ WordHD paletteOffset;
    /* 0x20 */ WordHD texture;
  };
};
struct TextureTableHD {
  /* 0x00 */ be<u16> count;
  /* 0x02 */ u8 reserved[2];
  /* 0x04 */ gptr<TextureEntryHD> entries;
  /* 0x08 */ WordHD maskHigh, maskLow, maskUpperHigh, maskUpperLow;
  /* 0x18 */ WordHD oneHigh, oneLow;
};
struct TextureResourceHD {
  /* 0x00 */ u8 prefix[8];
  /* 0x08 */ WordHD height, width;
};
struct ResourceFileHD {
  /* 0x00 */ u8 prefix[0x24];
  /* 0x24 */ be<s32> texturesOffset;
};
struct MaterialTableHD {
  /* 0x00 */ u8 prefix[0x18];
  /* 0x18 */ be<s32> namesOffset;
};
struct MaterialDescriptorHD {
  /* 0x00 */ u8 prefix[0xC];
  /* 0x0C */ be<u16> index;
};
struct MaterialHD {
  /* 0x00 */ gptr<MaterialDescriptorHD> descriptor;
  /* 0x04 */ u8 tail[0x39C - 4];
};
struct MaterialPacketHD {
  /* 0x00 */ u8 prefix[0x30];
  /* 0x30 */ WordHD flags;
  /* 0x34 */ u8 tail[4];
};
// HD: a missing packet still updates the flags slot relative to its zero base.
// Form the field address explicitly so C++ cannot discard the null branch.
static WordHD *packetFlagsSlot(MaterialPacketHD *packet) {
  return view<WordHD>(gabi::ea(packet) + offsetof(MaterialPacketHD, flags));
}
struct ModelDataHD : J3DModelData {
  /* 0x14 */ u8 gap[0x30 - 0x14];
  /* 0x30 */ gptr<TextureTableHD> textureTable;
  /* 0x34 */ WordHD textureNames;
};
struct ModelFlagsHD {
  /* 0x00 */ u8 prefix[0x74];
  /* 0x74 */ WordHD flags;
};
struct ModelPacketsHD : J3DModel {
  /* 0xF8 */ u8 gap[0x138 - 0xF8];
  /* 0x138 */ WordHD count;
  /* 0x13C */ gptr<MaterialPacketHD> packets;
};
WWHD_OFFSET(TextureEntryHD, height, 2);
WWHD_OFFSET(TextureEntryHD, width, 4);
WWHD_OFFSET(TextureEntryHD, format, 8);
WWHD_OFFSET(TextureEntryHD, imageOffset, 0xC);
WWHD_OFFSET(TextureEntryHD, paletteOffset, 0x1C);
WWHD_OFFSET(TextureEntryHD, texture, 0x20);
WWHD_SIZE(TextureEntryHD, 0x24);
WWHD_OFFSET(TextureTableHD, entries, 4);
WWHD_OFFSET(TextureTableHD, maskHigh, 8);
WWHD_OFFSET(TextureTableHD, oneHigh, 0x18);
WWHD_SIZE(TextureTableHD, 0x20);
WWHD_OFFSET(TextureResourceHD, height, 8);
WWHD_SIZE(TextureResourceHD, 0x10);
WWHD_OFFSET(ResourceFileHD, texturesOffset, 0x24);
WWHD_SIZE(ResourceFileHD, 0x28);
WWHD_OFFSET(MaterialTableHD, namesOffset, 0x18);
WWHD_SIZE(MaterialTableHD, 0x1C);
WWHD_OFFSET(MaterialDescriptorHD, index, 0xC);
WWHD_SIZE(MaterialDescriptorHD, 0xE);
WWHD_SIZE(MaterialHD, 0x39C);
WWHD_OFFSET(MaterialPacketHD, flags, 0x30);
WWHD_SIZE(MaterialPacketHD, 0x38);
WWHD_OFFSET(ModelDataHD, textureTable, 0x30);
WWHD_SIZE(ModelDataHD, 0x38);
WWHD_OFFSET(ModelFlagsHD, flags, 0x74);
WWHD_SIZE(ModelFlagsHD, 0x78);
WWHD_OFFSET(ModelPacketsHD, count, 0x138);
WWHD_SIZE(ModelPacketsHD, 0x140);
struct HeapFrameHD {
  /* 0x00 */ u8 prefix[0x14];
  /* 0x14 */ SafeStringHD textureName, candidate, resourceName, fileName;
  /* 0x34 */ TextureEntryHD texture;
  /* 0x58 */ SafeStringHD materialName;
  /* 0x60 */ u8 tail[0xE8 - 0x60];
};
WWHD_OFFSET(HeapFrameHD, textureName, 0x14);
WWHD_OFFSET(HeapFrameHD, texture, 0x34);
WWHD_OFFSET(HeapFrameHD, materialName, 0x58);
WWHD_SIZE(HeapFrameHD, 0xE8);
struct BckAnmHD {
  /* 0x00 */ u8 frameControl[0x10];
  /* 0x10 */ WordHD vtable;
  /* 0x14 */ u8 frameHelper[0x34];
  /* 0x48 */ WordHD helperVtable;
  /* 0x4C */ u8 gap[0xC];
  /* 0x58 */ WordHD helperState;
  /* 0x5C */ u8 gapState[0x20];
  /* 0x7C */ WordHD state, resource, model, flags;
};
WWHD_OFFSET(BckAnmHD, vtable, 0x10);
WWHD_OFFSET(BckAnmHD, frameHelper, 0x14);
WWHD_OFFSET(BckAnmHD, helperVtable, 0x48);
WWHD_OFFSET(BckAnmHD, helperState, 0x58);
WWHD_OFFSET(BckAnmHD, state, 0x7C);
WWHD_SIZE(BckAnmHD, 0x8C);
static u32 relativeAddress(be<s32> *offset) {
  s32 value = *offset;
  return value != 0 ? gabi::ea(offset) + value : 0;
}
} // namespace item_base

// HD: these private adapters retain the verified native parameter widths.
static void itemBaseSpecular(cXyz *, dKy_tevstr_c *, J3DModel *, f32);
static void itemBaseNormalizeFrame(gptr<item_base::FrameHelperHD> *);
static void itemBaseAssureTermination(u32);
static u32 itemBaseTextureName(u32, u32);
static u32 itemBaseResourceFile(u32, item_base::SafeStringHD *,
                                item_base::SafeStringHD *, int);
static u32 itemBaseTexture(u32, const char *);
static s32 itemBaseMaterialIndex(u32, u32);
static void itemBasePacketFlags(u32, u32);
static mDoExt_btkAnm *itemBaseNewBtk(int);
static mDoExt_brkAnm *itemBaseNewBrk(int);
static BOOL itemBaseBtkInit(u32, J3DModelData *, void *, s32, s32, f32, s16,
                            s16, bool, s32);
static BOOL itemBaseBrkInit(u32, J3DModelData *, void *, s32, s32, f32, s16,
                            s16, bool, s32);
static void itemBaseFrameInit(u32, int);
static void itemBaseFrameHelperInit(u8 *);
