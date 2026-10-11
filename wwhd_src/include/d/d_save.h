/* d_save: the save information (dSv_*), WWHD layouts with GameCube names.
 *
 * HD keeps the GameCube layout. Anchors measured from verified code (offsets from dSv_info_c, which
 * dComIfGs_info() reaches at *(0x101F84DC) + 0x20): mRupee +0x4, mTime +0x24, mPlayerItem +0x3C,
 * mItemRecord.mItemRecord2.mArrowNum/mBombNum +0x69/+0x6A, mItemMax.mItemMax2 +0x6F/+0x70,
 * mBagItem.mBait +0x7E, mSavedata.mEvent +0x624, mTmp +0x1158 [v nz, kamome, npc_* event users].
 * Members between the anchors follow tww; [g] where no verified function has read them yet. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "wwhd.h"

struct dSv_player_status_a_c {
    /* 0x00 */ be<u16> mMaxLife;
    /* 0x02 */ be<u16> mLife;
    /* 0x04 */ be<u16> mRupee;          /* [v nz] */
    /* 0x06 */ be<u16> field_0x6;
    /* 0x08 */ be<u8> field_0x8;
    /* 0x09 */ be<u8> mSelectItem[5];   /* X, Y, Z button */
    /* 0x0E */ be<u8> mSelectEquip[4];  /* sword, shield, bracelet, ? */
    /* 0x12 */ be<u8> mWalletSize;
    /* 0x13 */ be<u8> mMaxMagic;
    /* 0x14 */ be<u8> mMagic;
    /* 0x15 */ be<u8> field_0x15;
    /* 0x16 */ be<u8> field_0x16;
    /* 0x17 */ u8 _17;
};
WWHD_SIZE(dSv_player_status_a_c, 0x18);

struct dSv_player_status_b_c {
    /* 0x00 */ be<u32> mDateIPL[2];     /* GameCube u64 */
    /* 0x08 */ be<f32> field_0x8;
    /* 0x0C */ be<f32> mTime;           /* [v kamome] */
    /* 0x10 */ be<u16> mDate;
    /* 0x12 */ be<s16> mTactWindAngleX;
    /* 0x14 */ be<s16> mTactWindAngleY;
    /* 0x16 */ u8 _16[2];
};
WWHD_SIZE(dSv_player_status_b_c, 0x18);

struct dSv_player_return_place_c {
    /* 0x00 */ char mName[8];
    /* 0x08 */ be<s8> mRoomNo;
    /* 0x09 */ be<u8> mPoint;
    /* 0x0A */ be<u8> unk_0xa;
    /* 0x0B */ be<u8> unk_0xb;
};
WWHD_SIZE(dSv_player_return_place_c, 0xC);

struct dSv_player_item_c {
    /* 0x00 */ be<u8> mItems[21];       /* [v nz] */
};
WWHD_SIZE(dSv_player_item_c, 0x15);

struct dSv_player_get_item_c {
    /* 0x00 */ be<u8> mItemFlags[21];
};
WWHD_SIZE(dSv_player_get_item_c, 0x15);

struct dSv_player_item_record2_c {
    /* 0x0 */ be<u8> mPictureNum;
    /* 0x1 */ be<u8> mArrowNum;          /* [v nz] */
    /* 0x2 */ be<u8> mBombNum;           /* [v nz] */
};
struct dSv_player_item_record_c {
    /* 0x0 */ be<u16> mTimer;
    /* 0x2 */ dSv_player_item_record2_c mItemRecord2;
    /* 0x5 */ be<u8> mBottleNum[3];
};
WWHD_SIZE(dSv_player_item_record_c, 0x8);

struct dSv_player_item_max2_c {
    /* 0x0 */ be<u8> mReserved1Num;
    /* 0x1 */ be<u8> mArrowNum;          /* [v nz] */
    /* 0x2 */ be<u8> mBombNum;           /* [v nz] */
};
struct dSv_player_item_max_c {
    /* 0x0 */ dSv_player_item_max2_c mItemMax2;
    /* 0x3 */ be<u8> field_0x3[5];
};
WWHD_SIZE(dSv_player_item_max_c, 0x8);

struct dSv_player_bag_item_c {
    /* 0x00 */ be<u8> mBeast[8];
    /* 0x08 */ be<u8> mBait[8];          /* [v nz: checkBaitItemEmpty this] */
    /* 0x10 */ be<u8> mReserve[8];
};
WWHD_SIZE(dSv_player_bag_item_c, 0x18);

struct dSv_player_get_bag_item_c {
    /* 0x0 */ be<u32> mReserveFlags;
    /* 0x4 */ be<u8> mBeastFlags;
    /* 0x5 */ be<u8> mBaitFlags;
    /* 0x6 */ u8 unk_0x6[0xC - 0x6];
};
WWHD_SIZE(dSv_player_get_bag_item_c, 0xC);

struct dSv_player_bag_item_record_c {
    /* 0x00 */ be<u8> mBeastNum[8];
    /* 0x08 */ be<u8> mBaitNum[8];
    /* 0x10 */ be<u8> mReserveNum[8];
};
WWHD_SIZE(dSv_player_bag_item_record_c, 0x18);

struct dSv_player_collect_c {
    /* 0x0 */ be<u8> mCollect[8];
    /* 0x8 */ be<u8> field_0x8;
    /* 0x9 */ be<u8> mTact;
    /* 0xA */ be<u8> mTriforce;
    /* 0xB */ be<u8> mSymbol;
    /* 0xC */ be<u8> field_0xc;
};
WWHD_SIZE(dSv_player_collect_c, 0xD);

struct dSv_player_map_c {
    /* 0x00 */ be<u32> field_0x0[4][4];
    /* 0x40 */ be<u8> mFmapBits[0x31];   /* dIsleIdx_COUNT_e */
    /* 0x71 */ be<u8> field_0x71[16];
    /* 0x81 */ be<u8> field_0x81;
    /* 0x82 */ u8 field_0x82[0x84 - 0x82];
};
WWHD_SIZE(dSv_player_map_c, 0x84);

struct dSv_player_info_c {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ be<u16> field_0x10;
    /* 0x12 */ be<u16> mDeathCount;
    /* 0x14 */ char mPlayerName[17];
    /* 0x25 */ char field_0x25[17];
    /* 0x36 */ char field_0x36[17];
    /* 0x47 */ be<u8> mPuzzleData[17];
    /* 0x58 */ be<u8> mClearCount;
    /* 0x59 */ be<u8> mRandomSalvagePoint;
    /* 0x5A */ u8 field_0x5a[0x5C - 0x5A];
};
WWHD_SIZE(dSv_player_info_c, 0x5C);

struct dSv_player_config_c {
    /* 0x0 */ be<u8> mRuby;
    /* 0x1 */ be<u8> mSoundMode;
    /* 0x2 */ be<u8> mAttentionType;
    /* 0x3 */ be<u8> mVibration;
    /* 0x4 */ be<u8> field_0x4;
};

struct dSv_player_priest_c {
    /* 0x0 */ cXyz field_0x0;
    /* 0xC */ be<s16> field_0xc;
    /* 0xE */ be<s8> field_0xe;
    /* 0xF */ be<u8> field_0xf;
};
WWHD_SIZE(dSv_player_priest_c, 0x10);

struct dSv_player_status_c_c {
    /* 0x00 */ dSv_player_status_a_c mRecollectStatusA;
    /* 0x18 */ dSv_player_item_c mRecollectItem;
    /* 0x2D */ dSv_player_item_record2_c mRecollectItemRecord;
    /* 0x30 */ dSv_player_item_max2_c mRecollectItemMax;
    /* 0x33 */ dSv_player_bag_item_c mRecollectBagItem;
    /* 0x4B */ dSv_player_bag_item_record_c mRecollectBagItemRecord;
    /* 0x63 */ dSv_player_collect_c mRecollectCollect;
    /* 0x70 */
};
WWHD_SIZE(dSv_player_status_c_c, 0x70);

struct dSv_player_c {
    enum { PLAYER_STATUS_C_COUNT = 4 };
    /* 0x000 */ dSv_player_status_a_c mPlayerStatusA;
    /* 0x018 */ dSv_player_status_b_c mPlayerStatusB;
    /* 0x030 */ dSv_player_return_place_c mReturnPlace;
    /* 0x03C */ dSv_player_item_c mPlayerItem;
    /* 0x051 */ dSv_player_get_item_c mGetItem;
    /* 0x066 */ dSv_player_item_record_c mItemRecord;
    /* 0x06E */ dSv_player_item_max_c mItemMax;
    /* 0x076 */ dSv_player_bag_item_c mBagItem;
    /* 0x08E */ u8 _08E[2];
    /* 0x090 */ dSv_player_get_bag_item_c mGetBagItem;
    /* 0x09C */ dSv_player_bag_item_record_c mBagItemRecord;
    /* 0x0B4 */ dSv_player_collect_c mCollect;
    /* 0x0C1 */ u8 _0C1[3];
    /* 0x0C4 */ dSv_player_map_c mMap;
    /* 0x148 */ dSv_player_info_c mInfo;
    /* 0x1A4 */ dSv_player_config_c mConfig;
    /* 0x1A9 */ u8 _1A9[3];
    /* 0x1AC */ dSv_player_priest_c mPriest;
    /* 0x1BC */ dSv_player_status_c_c mStatusC[PLAYER_STATUS_C_COUNT];
    /* 0x37C */ u8 _37C[4];
};
WWHD_OFFSET(dSv_player_c, mPlayerItem, 0x3C);
WWHD_OFFSET(dSv_player_c, mItemRecord, 0x66);
WWHD_OFFSET(dSv_player_c, mBagItem, 0x76);
WWHD_OFFSET(dSv_player_c, mGetBagItem, 0x90);
WWHD_OFFSET(dSv_player_c, mMap, 0xC4);
WWHD_OFFSET(dSv_player_c, mConfig, 0x1A4);
WWHD_OFFSET(dSv_player_c, mStatusC, 0x1BC);
WWHD_SIZE(dSv_player_c, 0x380);

struct dSv_memBit_c {
    enum { MAP, COMPASS, BOSS_KEY, STAGE_BOSS_ENEMY, STAGE_LIFE, STAGE_BOSS_DEMO };
    /* 0x00 */ be<u32> mTbox;
    /* 0x04 */ be<u32> mSwitch[4];
    /* 0x14 */ be<u32> mItem[1];
    /* 0x18 */ be<u32> mVisitedRoom[2];
    /* 0x20 */ be<u8> mKeyNum;
    /* 0x21 */ be<u8> mDungeonItem;
    /* 0x22 */ u8 _22[2];
};
WWHD_SIZE(dSv_memBit_c, 0x24);

struct dSv_ocean_c {
    /* 0x00 */ be<u16> field_0x0[50];
};
WWHD_SIZE(dSv_ocean_c, 0x64);

/* event flags (0x100 bytes); HD methods out of line [g] */
struct dSv_event_c {
    /* 0x00 */ be<u8> mFlags[0x100];
    /* 025B8B68 */ void onEventBit(u16 flag) { gabi::call(0x025B8B68, this, flag); }
    /* 025B8B94 */ BOOL isEventBit(u16 flag) { return gabi::call<BOOL>(0x025B8B94, this, flag); }
    /* 025B8AF4 */ void setEventReg(u16 reg, u8 v) { gabi::call(0x025B8AF4, this, reg, v); }
    /* 025B8BB0 */ u8 getEventReg(u16 reg) { return gabi::call<u8>(0x025B8BB0, this, reg); }
};
WWHD_SIZE(dSv_event_c, 0x100);

struct dSv_reserve_c {
    /* 0x00 */ be<u8> mReserve[0x50];
};

struct dSv_memory_c {
    /* 0x00 */ dSv_memBit_c mMembit;
};
WWHD_SIZE(dSv_memory_c, 0x24);

struct dSv_danBit_c {
    /* 0x0 */ be<s8> mStageNo;
    /* 0x1 */ be<u8> mGbaRupeeCount;
    /* 0x2 */ u8 _2[2];
    /* 0x4 */ be<u32> mSwitch[2];
};
WWHD_SIZE(dSv_danBit_c, 0xC);

struct dSv_zoneBit_c {
    /* 0x0 */ be<u16> mSwitch[3];
    /* 0x6 */ be<u16> mItem;
};
struct dSv_zoneActor_c {
    /* 0x0 */ be<u32> mActorFlags[16];
};
struct dSv_zone_c {
    /* 0x00 */ be<s8> mRoomNo;
    /* 0x01 */ u8 _01;
    /* 0x02 */ dSv_zoneBit_c mZoneBit;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ dSv_zoneActor_c mZoneActor;
};
WWHD_SIZE(dSv_zone_c, 0x4C);

struct dSv_restart_c {
    /* 0x00 */ be<s8> mRestartRoom;
    /* 0x01 */ be<s8> mOption;
    /* 0x02 */ be<s8> mOptionRoomNo;
    /* 0x03 */ u8 _03;
    /* 0x04 */ be<s16> mOptionPoint;
    /* 0x06 */ be<s16> mOptionRoomAngleY;
    /* 0x08 */ cXyz mOptionRoomPos;
    /* 0x14 */ be<s16> mStartCode;
    /* 0x16 */ be<s16> mRestartAngle;
    /* 0x18 */ cXyz mRestartPos;
    /* 0x24 */ be<u32> mRestartParam;
    /* 0x28 */ be<f32> mLastSpeedF;
    /* 0x2C */ be<u32> mLastMode;
};
WWHD_SIZE(dSv_restart_c, 0x30);

struct dSv_turnRestart_c {
    /* 0x00 */ cXyz mPosition;
    /* 0x0C */ be<u32> mParam;
    /* 0x10 */ be<s16> mAngleY;
    /* 0x12 */ be<s8> mRoomNo;
    /* 0x13 */ be<u8> field_0x13;
    /* 0x14 */ u8 field_0x14[0x24 - 0x14];
    /* 0x24 */ cXyz mShipPos;
    /* 0x30 */ be<s16> mShipAngleY;
    /* 0x32 */ u8 _32[2];
    /* 0x34 */ be<s32> mHasShip;
};
WWHD_SIZE(dSv_turnRestart_c, 0x38);

struct dSv_save_c {
    enum { STAGE_MAX = 0x10 };
    /* 0x000 */ dSv_player_c mPlayer;
    /* 0x380 */ dSv_memory_c mMemory[STAGE_MAX];
    /* 0x5C0 */ dSv_ocean_c mOcean;
    /* 0x624 */ dSv_event_c mEvent;      /* [v: *(0x101F84DC) + 0x644 in the npc/kb/ship units] */
    /* 0x724 */ dSv_reserve_c mReserve;
    /* 0x774 */ u8 _774[4];
};
WWHD_OFFSET(dSv_save_c, mEvent, 0x624);
WWHD_SIZE(dSv_save_c, 0x778);

struct dSv_info_c {
    enum { ZONE_MAX = 0x20 };
    /* 0x0000 */ dSv_save_c mSavedata;
    /* 0x0778 */ dSv_memory_c mMemory;
    /* 0x079C */ dSv_danBit_c mDan;
    /* 0x07A8 */ dSv_zone_c mZone[ZONE_MAX];
    /* 0x1128 */ dSv_restart_c mRestart;
    /* 0x1158 */ dSv_event_c mTmp;        /* [v: *(0x101F84DC) + 0x1178 in npc_photo, npc_people] */
    /* 0x1258 */ dSv_turnRestart_c mTurnRestart;
    /* 0x1290 */ be<u8> mDataNum;
    /* 0x1291 */ be<u8> mNewFile;
    /* 0x1292 */ be<u8> mNoFile;
    /* 0x1293 */ u8 _1293[5];
    /* 0x1298 */ be<u32> mMemCardCheckID[2]; /* GameCube u64 */

    /* 025BA0C0 [v mtoge] */
    BOOL isSwitch(int no, int roomNo) { return gabi::call<BOOL>(0x025BA0C0, this, no, roomNo); }
    /* 025B9E38 [v swc00] */
    void onSwitch(int no, int roomNo) { gabi::call(0x025B9E38, this, no, roomNo); }
    /* 025B9F7C [v swc00] */
    void offSwitch(int no, int roomNo) { gabi::call(0x025B9F7C, this, no, roomNo); }
    /* 025BA5D4 [g] */
    void onActor(int no, int roomNo) { gabi::call(0x025BA5D4, this, no, roomNo); }
    /* 025BA494 [v switem] */
    BOOL isItem(int no, int roomNo) { return gabi::call<BOOL>(0x025BA494, this, no, roomNo); }
};
WWHD_OFFSET(dSv_info_c, mTmp, 0x1158);
WWHD_OFFSET(dSv_info_c, mTurnRestart, 0x1258);
WWHD_SIZE(dSv_info_c, 0x12A0);
