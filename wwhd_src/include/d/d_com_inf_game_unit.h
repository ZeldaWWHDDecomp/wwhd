#pragma once
#include "bindings.h"

// HD: private supplements to the shared game-info layout. Shared accessor/header definitions
// stay unchanged; the verified entry points retain their original register widths.
namespace GameInfoHD {
enum PacketFunction : u32 {
    MagmaCalc = 0x0258C7C8,
    MagmaUpdate = 0x0258CA60,
    GrassCalc = 0x0254C6C4,
    GrassUpdate = 0x0254CF78,
    TreeCalc = 0x025C9948,
    TreeUpdate = 0x025CA6EC,
    WoodCalc = 0x025D0994,
    WoodUpdate = 0x025D15B0,
    FlowerCalc = 0x02548370,
    FlowerUpdate = 0x02548B6C,
    GrassConstructor = 0x0254ACD8,
    TreeConstructor = 0x025C7A1C,
    WoodConstructor = 0x025CF20C,
    FlowerConstructor = 0x02545A2C,
};
template <class T> inline T* view(u32 address) {
    return reinterpret_cast<T*>(PPC_MEM_BASE + address);
}
// HD: these vector fields are copied with integer loads, including NaN payload bits.
inline u32 wordBits(const be<f32>& value) {
    return *reinterpret_cast<const be<u32>*>(&value);
}
struct Matrix34 {
    be<f32> m[3][4];
};
struct Matrix44 {
    be<f32> m[4][4];
};
struct FixedSafeString32 {
    be<u32> mStringTop, __vtbl, mBufferSize;
    be<u8> mBuffer[32];
};
struct WFixedSafeString17 {
    be<u32> mStringTop, __vtbl, mBufferSize;
    be<u16> mBuffer[17];
    u8 padding[2];
};
struct StartCheck {
    be<u8> mbHasEvent;
    u8 padding;
    be<u16> mEvent;
    be<u8> mStage[8];
    be<s8> mRoomNo;
    be<u8> mStartCode;
};
struct LandingEvent {
    be<s8> roomNo;
    u8 padding;
    be<u16> event;
};
struct EntryList {
    be<s32> num;
    be<u32> m_entries;
};
struct Ship {
    u8 padding[0xE];
    be<u8> field_0xe;
    u8 padding2;
};
struct Multi {
    be<f32> mTransX, mTransY;
    be<s16> mAngle;
    be<u8> mRoomNo;
    u8 padding;
};
struct SceneList {
    be<u8> mStage[8];
    be<u8> mStart;
    be<s8> mRoom;
    u8 padding[2];
};
struct StageInfo {
    u8 padding[9];
    be<u8> mSaveTableBits;
    u8 padding2[2];
    be<u32> mStageTypeBits;
};
struct PlaySupplement {
    u8 padding[0x494A];
    be<u8> mPictureFormat, mSelectPicture, mHeapLockFlag, mNowVibration;
    be<u8> mPalLanguage, field_0x4965;
    WFixedSafeString17 mInputPassword;
    be<u8> mMesgBgm, mScopeWipeFlag, m2dShow, field_0x497a;
    u8 padding2[4];
    be<u8> hdItemFlags[6];
    u8 padding3[2];
    be<u32> mMesgCamID, mMesgCamBasicID, mMesgCamActors[10];
    dSv_player_status_c_c mPlayerInfoBuffer;
    be<u8> mPlayerInfoBufferStageNo;
};
WWHD_SIZE(Matrix34, 0x30);
WWHD_SIZE(Matrix44, 0x40);
WWHD_SIZE(EntryList, 8);
WWHD_SIZE(Ship, 0x10);
WWHD_OFFSET(Ship, field_0xe, 0xE);
WWHD_SIZE(StageInfo, 0x10);
WWHD_OFFSET(StageInfo, mSaveTableBits, 9);
WWHD_OFFSET(StageInfo, mStageTypeBits, 0xC);
WWHD_SIZE(PlaySupplement, 0x4A31);
WWHD_SIZE(FixedSafeString32, 0x2C);
WWHD_SIZE(WFixedSafeString17, 0x30);
WWHD_SIZE(StartCheck, 0xE);
WWHD_SIZE(LandingEvent, 4);
WWHD_SIZE(Multi, 0xC);
WWHD_SIZE(SceneList, 0xC);
WWHD_OFFSET(PlaySupplement, mInputPassword, 0x4950);
WWHD_OFFSET(PlaySupplement, mMesgCamID, 0x4990);
WWHD_OFFSET(PlaySupplement, mPlayerInfoBuffer, 0x49C0);
WWHD_OFFSET(PlaySupplement, mPlayerInfoBufferStageNo, 0x4A30);
} // namespace GameInfoHD

// HD: out-of-line play methods use the shared, byte-identical GameCube layout.
namespace GameInfoHD {
struct dComIfG_play_c : ::dComIfG_play_c {
    void init();
    void ct();
    void itemInit();
    void executeEvtManager();
    void createParticle();
    u32 createMagma();
    void removeMagma();
    void executeMagma();
    void drawMagma();
    u32 createGrass();
    void removeGrass();
    void executeGrass();
    void drawGrass();
    u32 createTree();
    void removeTree();
    void executeTree();
    void drawTree();
    u32 createWood();
    void removeWood();
    void executeWood();
    void drawWood();
    u32 createFlower();
    void removeFlower();
    void executeFlower();
    void drawFlower();
};
WWHD_SIZE(dComIfG_play_c, sizeof(::dComIfG_play_c));
} // namespace GameInfoHD

namespace GameInfoHD {
// HD: the global save pointer includes the platform save manager's 32-byte prefix.
struct SaveStorage {
    /* 0x0000 */ u8 mPlatformHeader[0x20];
    /* 0x0020 */ dSv_info_c mInfo;
};
WWHD_OFFSET(SaveStorage, mInfo, 0x20);
WWHD_SIZE(SaveStorage, 0x12C0);
} // namespace GameInfoHD

namespace GameInfoHD {
// HD: additional item counters and a fifth equipment byte occupy the GameCube padding.
struct ItemSupplement {
    /* 0x0000 */ u8 padding[0x48E0];
    /* 0x48E0 */ be<s16> mAdditionalItemCounts[2][8];
    /* 0x4900 */ u8 padding2[0x4920 - 0x4900];
    /* 0x4920 */ be<u8> mSelectEquip[5];
};
WWHD_OFFSET(ItemSupplement, mAdditionalItemCounts, 0x48E0);
WWHD_OFFSET(ItemSupplement, mSelectEquip, 0x4920);
WWHD_SIZE(ItemSupplement, 0x4925);
} // namespace GameInfoHD

namespace GameInfoHD {
struct FixedSafeString8 {
    be<u32> mStringTop, __vtbl, mBufferSize;
    be<u8> mBuffer[8];
};
WWHD_SIZE(FixedSafeString8, 0x14);
// HD: the inline FixedSafeString<8> between the archive sets shifts the later members.
struct PlayArchives {
    u8 padding[0x4794];
    /* 0x4794 */ be<u32> mpMsgArchive;
    /* 0x4798 */ be<u32> mpDmsgArchive;
    /* 0x479c */ be<u32> mTactMsgArchive;
    /* 0x47a0 */ be<u32> mpMenuArchive;
    /* 0x47a4 */ be<u32> mpFont0Archive;
    /* 0x47a8 */ be<u32> mpFont1Archive;
    /* 0x47ac */ be<u32> mpAnmArchive;
    /* 0x47B0 */ FixedSafeString8 mArchiveName;
    /* 0x47c4 */ be<u32> mpLkDArc;
    /* 0x47c8 */ be<u32> mpFmapArchive;
    /* 0x47cc */ be<u32> mpItemResArchive;
    /* 0x47d0 */ be<u32> mpCollectResArchive;
    /* 0x47d4 */ be<u32> mpFmapResArchive;
    /* 0x47d8 */ be<u32> mpDmapResArchive;
    /* 0x47dc */ be<u32> mpOptionResArchive;
    /* 0x47e0 */ be<u32> mpClothResArchive;
    /* 0x47e4 */ be<u32> mpSaveResArchive;
    /* 0x47e8 */ be<u32> mpItemIconArchive;
    /* 0x47ec */ be<u32> mpNameResArchive;
    /* 0x47f0 */ be<u32> mpErrorResArchive;
    /* 0x47f4 */ be<u32> mpActionIconArchive;
    /* 0x47f8 */ be<u32> mpScopeResArchive;
    /* 0x47fc */ be<u32> mpCameraResArchive;
    /* 0x4800 */ be<u32> mpSwimResArchive;
    /* 0x4804 */ be<u32> mpWindResArchive;
    /* 0x4808 */ be<u32> mpEnglishTextArchive;
    /* 0x480c */ be<u32> mpHyruleTextArchive;
};
WWHD_OFFSET(PlayArchives, mArchiveName, 0x47B0);
WWHD_OFFSET(PlayArchives, mpLkDArc, 0x47C4);
WWHD_SIZE(PlayArchives, 0x4810);
struct View {
    u8 padding[0xC];
    be<u32> __vtbl;
    u8 padding2[0x48 - 0x10];
    be<u32> mpViewMtx, mpCamera;
    be<u32> mFlags;
    u8 padding3[0x84 - 0x54];
    Matrix34 mViewMtx;
    u8 padding4[0x164 - 0xB4];
    be<u32> mpConfiguredCamera;
};
WWHD_OFFSET(View, mFlags, 0x50);
WWHD_OFFSET(View, mViewMtx, 0x84);
WWHD_OFFSET(View, mpConfiguredCamera, 0x164);
WWHD_SIZE(View, 0x168);
struct Display {
    u8 padding[0x1020];
    be<u32> mViewNum, mpViews;
};
WWHD_SIZE(Display, 0x1028);
struct DrawInfo {
    u8 padding[0x5FA4];
    be<u32> mpProjView;
};
WWHD_OFFSET(DrawInfo, mpProjView, 0x5FA4);
struct ProjectionView {
    u8 padding[0x1A4];
    Matrix44 mProjViewMtx;
};
WWHD_OFFSET(ProjectionView, mProjViewMtx, 0x1A4);
} // namespace GameInfoHD

namespace GameInfoHD {
// Inventory IDs used by this unit; names and values follow tww/d_item_data.h.
enum ItemNo : u32 {
    dItemNo_JOY_PENDANT_e = 0x1f,
    dItemNo_TELESCOPE_e = 0x20,
    dItemNo_BOW_e = 0x27,
    dItemNo_POWER_BRACELETS_e = 0x28,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_SWORD_e = 0x38,
    dItemNo_MASTER_SWORD_2_e = 0x3a,
    dItemNo_SHIELD_e = 0x3b,
    dItemNo_MIRROR_SHIELD_e = 0x3c,
    dItemNo_MASTER_SWORD_3_e = 0x3e,
    dItemNo_PIRATES_CHARM_e = 0x42,
    dItemNo_HEROS_CHARM_e = 0x43,
    dItemNo_SKULL_NECKLACE_e = 0x45,
    dItemNo_BLUE_JELLY_e = 0x4b,
    dItemNo_TRIFORCE1_e = 0x61,
    dItemNo_SONG_OF_PASSING_e = 0x72,
};
} // namespace GameInfoHD
