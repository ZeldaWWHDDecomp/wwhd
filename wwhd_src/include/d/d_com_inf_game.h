/* d_com_inf_game: the global game info, WWHD. 
 *
 * HD: g_dComIfG_gameInfo.play is a function-local static (0x1046F0B0) behind the accessor
 * dComIfGp_get() (025200D4, called before every access); the save info is reached through the
 * pointer at 0x101F84DC (+0x20).
 *
 * dComIfGp_get() returns g_dComIfG_gameInfo (dComIfG_inf_c): the GameCube order, save info (0x12A0)
 * then play, then the draw list. dComIfG_play_c member offsets (play-relative; GameCube -> WWHD): the
 * same up to mStageData, -8 from mRoomCtrl, -4 from mAttention, -0x14/-0x18 around the particle
 * and player pointers, -0x16 at mMesgSendButton, +0x10 from mPlayerStatus. Measured from calls
 * whose `this` is play+OFF (tools/verify README: "playoffs") and verified accessors; [v] = used
 * by a verified function. The PLAY_* constants (absolute, from dComIfGp_get()) are the members'
 * offsets; accessors keep calling dComIfGp_get() at every use (HD inlines). */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "f_op/f_op_actor.h"
#include "d/d_save.h"
#include "d/d_bg_s.h"
#include "d/d_event.h"
#include "d/d_vibration.h"
#include "wwhd.h"

inline u8* dComIfGp_get() { return gabi::call<u8*>(0x025200D4); }
inline u32 dComIfGp_ea() { return gabi::ea(dComIfGp_get()); }

/* dStage_startStage_c / dStage_nextStage_c (unchanged from GameCube) */
struct dStage_startStage_c {
    /* 0x0 */ char mName[8];
    /* 0x8 */ be<s16> mPoint;
    /* 0xA */ be<s8> mRoomNo;
    /* 0xB */ be<s8> mLayer;
};
WWHD_SIZE(dStage_startStage_c, 0xC);
struct dStage_nextStage_c : dStage_startStage_c {
    /* 0xC */ be<s8> enable;
    /* 0xD */ be<u8> wipe;
};
WWHD_SIZE(dStage_nextStage_c, 0xE);

/* dComIfG_camera_info_class (0x34) */
struct dComIfG_camera_info_class {
    /* 0x00 */ be<u32> mpCamera;               /* camera_class* */
    /* 0x04 */ be<s8> mDlstWindowIdx;
    /* 0x05 */ be<s8> mCamP1Id;
    /* 0x06 */ be<s8> mCamP2Id;
    /* 0x07 */ u8 field_0x07;
    /* 0x08 */ be<u32> mCameraAttentionStatus;  /* [v ship, arrow_lighteff] */
    /* 0x0C */ u8 _0C[0x34 - 0x0C];
};
WWHD_SIZE(dComIfG_camera_info_class, 0x34);

/* __d_timer_info_c (0x14) */
struct __d_timer_info_c {
    /* 0x00 */ be<u32> mTimerPtr;               /* dTimer_c* [v npc_bmsw] */
    /* 0x04 */ be<s32> mTimerNowTimeMs;
    /* 0x08 */ be<s32> mTimerLimitTimeMs;
    /* 0x0C */ be<s32> mTimerMode;
    /* 0x10 */ be<u16> mTimerType;
    /* 0x12 */ u8 _12[2];
};
WWHD_SIZE(__d_timer_info_c, 0x14);

/* Member offsets: [v] read or written by verified code at this offset (the local accessors of the
 * unit headers, gathered for the shared accessors); [i] inferred from the GameCube order between [v] neighbours. */
struct dComIfG_play_c {
    /* 0x0000 */ dBgS mBgS;                       /* [v] */
    /* 0x0001 */ u8 _0001[0x1404 - 0x0001];
    /* 0x1404 */ cCcS mCcS;                       /* dCcS (cCcS::Set this) [v kamome]; its dCcMassS_Mng at +0x2854 */
    /* 0x1405 */ u8 _1405[0x3E94 - 0x1405];       /* ... cDT_NamePTbl at 0x3E0C (cDT::GetIndex this) */
    /* 0x3E94 */ dStage_startStage_c mCurStage;   /* [v name, point, layer] */
    /* 0x3EA0 */ dStage_nextStage_c mNextStage;
    /* 0x3EAE */ u8 field_0x3EAE[2];
    /* 0x3EB0 */ u8 mStageData[0x3F2C - 0x3EB0];  /* dStage_stageDt_c (HD 0x7C, GameCube 0x84; vtable first) [v kamome] */
    /* 0x3F2C */ dStage_roomControl_c mRoomCtrl;  /* GameCube 0x3F34 */
    /* 0x3F2D */ u8 _3F2D[3];
    /* 0x3F30 */ dEvt_control_c mEvtCtrl;         /* GameCube 0x3F38 */
    /* 0x4024 */ dEvent_manager_c mEvtManager;    /* GameCube 0x402C */
    /* 0x4025 */ u8 _4025[0x4564 - 0x4025];
    /* 0x4564 */ dAttention_c mAttention;         /* GameCube 0x4568; its flags word at +0x20 [v mknjd, ji1] */
    /* 0x4565 */ u8 _4565[0x46FC - 0x4565];
    /* 0x46FC */ dVibration_c mVibration;         /* GameCube 0x4700 */
    /* 0x46FD */ u8 _46FD[0x4780 - 0x46FD];
    /* 0x4780 */ dDetect_c mDetect;               /* GameCube 0x4784 */
    /* 0x4781 */ u8 _4781[0x4810 - 0x4781];       /* archives, ARAM blocks */
    /* 0x4810 */ gptr<dPa_control_c> mParticle;   /* GameCube 0x4824 [v lamp] */
    /* 0x4814 */ be<u32> mpMagmaPacket;           /* GameCube 0x482C [v magma] (HD: no mDemo pointer before the packets) */
    /* 0x4818 */ be<u32> mpGrassPacket;           /* [v grass] */
    /* 0x481C */ be<u32> mpTreePacket;            /* [v grass] */
    /* 0x4820 */ be<u32> mpWoodPacket;            /* [v obj_wood] */
    /* 0x4824 */ be<u32> mpFlowerPacket;          /* [v grass] */
    /* 0x4828 */ be<s8> mLkDArcIdx;               /* [i] */
    /* 0x4829 */ be<u8> mDlstWindowNum;           /* [i] */
    /* 0x482A */ be<u16> mStatus;                 /* [i] */
    /* 0x482C */ u8 mDlstWindow[0x2C];            /* dDlst_window_c [1] [i] */
    /* 0x4858 */ dComIfG_camera_info_class mCameraInfo[1]; /* GameCube 0x4870 [v ship, bk_exec] */
    /* 0x488C */ struct {
        /* 0x0 */ gptr<fopAc_ac_c> mpPlayer;
        /* 0x4 */ be<s8> mCameraID;
        /* 0x5 */ u8 _5[3];
    } mPlayerInfo[1];                             /* GameCube 0x48A4 [v kamome, bk_exec] */
    /* 0x4894 */ gptr<fopAc_ac_c> mpPlayerPtr[3]; /* 0: Link, 1: partner, 2: ship [v mknjd, hr, tornado] */
    /* 0x48A0 */ be<f32> mItemScopeWipeScale;     /* [i] */
    /* 0x48A4 */ be<f32> mItemLifeCount;          /* [v d_item] */
    /* 0x48A8 */ be<s32> mItemRupeeCount;         /* GameCube 0x48C0 [v nz, d_item] */
    /* 0x48AC */ be<s32> mAirMeter;               /* [i] */
    /* 0x48B0 */ be<s32> mItemTimeMax;            /* [i] */
    /* 0x48B4 */ be<u32> mNpcNameMessageID;       /* [i] */
    /* 0x48B8 */ be<u32> mItemNameMessageID;      /* [i] */
    /* 0x48BC */ be<s16> mItemKeyNumCount;        /* [v d_item] */
    /* 0x48BE */ be<s16> mItemMaxLifeCount;       /* [v d_item] */
    /* 0x48C0 */ be<s16> mItemMagicCount;         /* [v d_item, jbo] */
    /* 0x48C2 */ be<s16> mItemNowMagicCount;      /* [i] */
    /* 0x48C4 */ be<s16> mItemMaxMagicCount;      /* [v d_item] */
    /* 0x48C6 */ be<s16> mItemPictureNumCount;    /* [i] */
    /* 0x48C8 */ be<s16> mItemArrowNumCount;      /* [v d_item] */
    /* 0x48CA */ be<s16> field_0x48e2;
    /* 0x48CC */ be<s16> mItemBombNumCount;       /* [v d_item] */
    /* 0x48CE */ be<s16> field_0x48e6;
    /* 0x48D0 */ be<s16> mItemBeastNumCounts[8];  /* [v d_item] */
    /* 0x48E0 */ u8 field_0x48F8[0x4900 - 0x48E0];
    /* 0x4900 */ be<s16> mMsgCountNumber;         /* [v ji1, bmcon1] */
    /* 0x4902 */ be<s16> mMsgSetNumber;           /* [i] */
    /* 0x4904 */ be<s16> mMessageRupee;           /* GameCube 0x491C [v nz] */
    /* 0x4906 */ be<s16> mAuctionRupee;           /* [i] */
    /* 0x4908 */ be<s16> mAuctionGauge;           /* [i] */
    /* 0x490A */ be<s16> mItemTimer;              /* [v npc_people] */
    /* 0x490C */ be<u16> mItemNowLife;            /* [i] */
    /* 0x490E */ be<s16> mItemNowRupee;           /* [i] */
    /* 0x4910 */ be<u8> mItemSwimTimerStatus;     /* [i] */
    /* 0x4911 */ be<u8> mItemScopeWipeTimer;      /* [i] */
    /* 0x4912 */ be<u8> mMesgStatus;              /* [i] */
    /* 0x4913 */ be<u8> mScopeMesgStatus;         /* [v mn, npc_people] */
    /* 0x4914 */ be<u8> field_0x492c;
    /* 0x4915 */ be<u8> mRStatus;                 /* [i] */
    /* 0x4916 */ be<u8> mAStatus;                 /* [i] */
    /* 0x4917 */ be<u8> mDoStatus;                /* [v ji1] */
    /* 0x4918 */ be<u8> mRStatusForce;            /* [i] */
    /* 0x4919 */ be<u8> mAStatusForce;            /* [i] */
    /* 0x491A */ be<u8> mDoStatusForce;           /* [i] */
    /* 0x491B */ be<u8> mSelectItem[5];           /* HD: five item buttons (GameCube [4]) [v nz, d_item] */
    /* 0x4920 */ be<u8> mSelectEquip[4];          /* [i] */
    /* 0x4924 */ u8 _4924;                        /* HD */
    /* 0x4925 */ be<u8> mMesgAnime;               /* GameCube 0x493B [v npc_* (getMesgAnimeAttrInfo)] */
    /* 0x4926 */ be<u8> mMesgAnimeTagInfo;        /* [v ko1] */
    /* 0x4927 */ be<u8> mItemSlot;                /* [v d_item] */
    /* 0x4928 */ be<u8> mItemNo;                  /* [v d_item] */
    /* 0x4929 */ be<u8> field_0x493f;
    /* 0x492A */ be<u8> field_0x4940;
    /* 0x492B */ be<u8> mDirection;               /* [i] */
    /* 0x492C */ be<u8> mButtonMode;              /* [i] */
    /* 0x492D */ be<u8> mButtonInfo[2];           /* [i] */
    /* 0x492F */ be<u8> mScopeType;               /* [i] */
    /* 0x4930 */ be<u8> mOperateWind;             /* [i] */
    /* 0x4931 */ be<u8> mMetronome;               /* [i] */
    /* 0x4932 */ be<u8> mMesgSendButton;          /* GameCube 0x4948 [v nz] */
    /* 0x4933 */ be<u8> mMesgCancelButton;        /* [v hr] */
    /* 0x4934 */ be<u8> field_0x494a[6];
    /* 0x493A */ be<u8> mMelodyNum;               /* [v hr, tt] */
    /* 0x493B */ be<u8> mFmapOpen;                /* [i] */
    /* 0x493C */ be<u8> mNameOpen;                /* [i] */
    /* 0x493D */ be<u8> field_0x4953;
    /* 0x493E */ be<u8> field_0x4954;
    /* 0x493F */ be<u8> mStartItemTimer;          /* [v npc_people] */
    /* 0x4940 */ be<u8> mFwaterTimer;             /* [i] */
    /* 0x4941 */ be<u8> mPlacenameIndex;          /* [i] */
    /* 0x4942 */ be<u8> mPlacenameState;          /* [i] */
    /* 0x4943 */ be<u8> mGameoverStatus;          /* [i] */
    /* 0x4944 */ be<u8> mMenuCollect;             /* [i] */
    /* 0x4945 */ be<u8> mPictureFlag;             /* [i] */
    /* 0x4946 */ be<u8> mPictureResult;           /* [v npc_photo, npc_people] */
    /* 0x4947 */ be<u8> mPictureResultDetail;     /* [v npc_people] */
    /* 0x4948 */ be<u8> mPictureStatus;           /* [i] */
    /* 0x4949 */ be<u8> mGetPictureNum;           /* [i] */
    /* 0x494A */ u8 _494A[0x4A38 - 0x494A];       /* HD grew here by 0x26 (not located yet) */
    /* 0x4A38 */ be<u32> mPlayerStatus[1][4];     /* GameCube 0x4A28 [v kamome, swtact, kanban] */
    /* 0x4A48 */ be<u16> mMinigameFlags;          /* [v ji1] */
    /* 0x4A4A */ be<u8> mMiniGameType;            /* [v ji1, kmon] */
    /* 0x4A4B */ u8 _4A4B;
    /* 0x4A4C */ be<s16> mMiniGameRupee;          /* [v ji1] */
    /* 0x4A4E */ be<u8> mMinigameResult;          /* [v ji1] */
    /* 0x4A4F */ u8 _4A4F;
    /* 0x4A50 */ __d_timer_info_c mTimerInfo;     /* [v npc_bmsw] */
    /* 0x4A64 */ be<u32> mCurrentWindow;          /* [i] */
    /* 0x4A68 */ be<u32> mCurrentView;            /* [i] */
    /* 0x4A6C */ be<u32> mCurrentViewport;        /* [i] */
    /* 0x4A70 */ be<u32> mCurrentGrafPort;        /* [i] */
    /* 0x4A74 */ be<u8> mShipId;                  /* [i] */
    /* 0x4A75 */ be<u8> mShipRoomId;              /* [i] */
    /* 0x4A76 */ be<u8> mIkadaShipBeforeRoomId;   /* [i] */
    /* 0x4A77 */ be<u8> mIkadaShipId;             /* [i] */
    /* 0x4A78 */ cXyz mIkadaLinkPos;              /* [i] */
    /* 0x4A84 */ be<u32> mpItemTable;             /* ItemTableList*; GameCube 0x4A74 [v switem] */
    /* 0x4A88 */ be<u32> mpFmapData;              /* [i] */
    /* 0x4A8C */ u8 _4A8C[0x4A90 - 0x4A8C];

    fopAc_ac_c* getPlayer(int idx) { return mPlayerInfo[idx].mpPlayer; }
    fopAc_ac_c* getPlayerPtr(int idx) { return mpPlayerPtr[idx]; }
    dEvt_control_c& getEvent() { return mEvtCtrl; }
};
WWHD_OFFSET(dComIfG_play_c, mCcS, 0x1404);
WWHD_OFFSET(dComIfG_play_c, mStageData, 0x3EB0);
WWHD_OFFSET(dComIfG_play_c, mRoomCtrl, 0x3F2C);
WWHD_OFFSET(dComIfG_play_c, mEvtCtrl, 0x3F30);
WWHD_OFFSET(dComIfG_play_c, mEvtManager, 0x4024);
WWHD_OFFSET(dComIfG_play_c, mAttention, 0x4564);
WWHD_OFFSET(dComIfG_play_c, mVibration, 0x46FC);
WWHD_OFFSET(dComIfG_play_c, mDetect, 0x4780);
WWHD_OFFSET(dComIfG_play_c, mParticle, 0x4810);
WWHD_OFFSET(dComIfG_play_c, mPlayerInfo, 0x488C);
WWHD_OFFSET(dComIfG_play_c, mpPlayerPtr, 0x4894);
WWHD_OFFSET(dComIfG_play_c, mItemRupeeCount, 0x48A8);
WWHD_OFFSET(dComIfG_play_c, mMessageRupee, 0x4904);
WWHD_OFFSET(dComIfG_play_c, mSelectItem, 0x491B);
WWHD_OFFSET(dComIfG_play_c, mMesgSendButton, 0x4932);
WWHD_OFFSET(dComIfG_play_c, mPlayerStatus, 0x4A38);
WWHD_OFFSET(dComIfG_play_c, mpItemTable, 0x4A84);
WWHD_OFFSET(dComIfG_play_c, mCurStage, 0x3E94);
WWHD_OFFSET(dComIfG_play_c, mpMagmaPacket, 0x4814);
WWHD_OFFSET(dComIfG_play_c, mCameraInfo, 0x4858);
WWHD_OFFSET(dComIfG_play_c, mItemLifeCount, 0x48A4);
WWHD_OFFSET(dComIfG_play_c, mItemKeyNumCount, 0x48BC);
WWHD_OFFSET(dComIfG_play_c, mItemBeastNumCounts, 0x48D0);
WWHD_OFFSET(dComIfG_play_c, mMsgCountNumber, 0x4900);
WWHD_OFFSET(dComIfG_play_c, mItemTimer, 0x490A);
WWHD_OFFSET(dComIfG_play_c, mScopeMesgStatus, 0x4913);
WWHD_OFFSET(dComIfG_play_c, mDoStatus, 0x4917);
WWHD_OFFSET(dComIfG_play_c, mMesgAnime, 0x4925);
WWHD_OFFSET(dComIfG_play_c, mItemSlot, 0x4927);
WWHD_OFFSET(dComIfG_play_c, mMesgCancelButton, 0x4933);
WWHD_OFFSET(dComIfG_play_c, mMelodyNum, 0x493A);
WWHD_OFFSET(dComIfG_play_c, mStartItemTimer, 0x493F);
WWHD_OFFSET(dComIfG_play_c, mPictureResult, 0x4946);
WWHD_OFFSET(dComIfG_play_c, mMinigameFlags, 0x4A48);
WWHD_OFFSET(dComIfG_play_c, mMiniGameRupee, 0x4A4C);
WWHD_OFFSET(dComIfG_play_c, mTimerInfo, 0x4A50);
WWHD_SIZE(dComIfG_play_c, 0x4A90);

/* g_dComIfG_gameInfo (GameCube dComIfG_inf_c): HD a function-local static behind dComIfGp_get() */
struct dComIfG_inf_c {
    /* 0x0000 */ dSv_info_c save;        /* HD: the verified code reaches the save info through *(0x101F84DC) */
    /* 0x12A0 */ dComIfG_play_c play;
    /* 0x5D30 */ dDlst_list_c drawlist;  /* GameCube 0x5D1C; opa/xlu lists at +0x40.. (bindings.h dComIfGd_*) */
};
WWHD_OFFSET(dComIfG_inf_c, play, 0x12A0);
WWHD_OFFSET(dComIfG_inf_c, drawlist, 0x5D30);
#define PLAY_OFF(member) ((u32)offsetof(dComIfG_inf_c, play.member))

/* absolute offsets from dComIfGp_get() (the raw units' names) */
enum : u32 {
    PLAY_BGS = PLAY_OFF(mBgS),                 /* 0x12A0 */
    PLAY_CCS = PLAY_OFF(mCcS),                 /* 0x26A4 */
    PLAY_CCMASS = PLAY_OFF(mCcS) + 0x2854,     /* 0x4EF8 dCcMassS_Mng (in dCcS) */
    PLAY_NAMETBL = 0x50AC,                     /* cDT_NamePTbl (cDT::GetIndex this) */
    PLAY_STAGEDATA = PLAY_OFF(mStageData),     /* 0x5150 */
    PLAY_ROOMCTRL = PLAY_OFF(mRoomCtrl),       /* 0x51CC */
    PLAY_EVTCTRL = PLAY_OFF(mEvtCtrl),         /* 0x51D0 */
    PLAY_EVTMANAGER = PLAY_OFF(mEvtManager),   /* 0x52C4 */
    PLAY_ATTENTION = PLAY_OFF(mAttention),     /* 0x5804 */
    PLAY_VIBRATION = PLAY_OFF(mVibration),     /* 0x599C */
    PLAY_DETECT = PLAY_OFF(mDetect),           /* 0x5A20 */
    PLAY_PARTICLE = PLAY_OFF(mParticle),       /* 0x5AB0 */
    PLAY_PLAYER = PLAY_OFF(mPlayerInfo),       /* 0x5B2C */
    PLAY_PLAYERPTR = PLAY_OFF(mpPlayerPtr),    /* 0x5B34 */
    PLAY_PLAYER_STATUS0 = PLAY_OFF(mPlayerStatus), /* 0x5CD8 */
    PLAY_DLST = (u32)offsetof(dComIfG_inf_c, drawlist), /* 0x5D30 */
};
static_assert(PLAY_BGS == 0x12A0 && PLAY_CCS == 0x26A4 && PLAY_CCMASS == 0x4EF8 && PLAY_STAGEDATA == 0x5150 &&
              PLAY_ROOMCTRL == 0x51CC && PLAY_EVTCTRL == 0x51D0 && PLAY_EVTMANAGER == 0x52C4 &&
              PLAY_ATTENTION == 0x5804 && PLAY_VIBRATION == 0x599C && PLAY_DETECT == 0x5A20 &&
              PLAY_PARTICLE == 0x5AB0 && PLAY_PLAYER == 0x5B2C && PLAY_PLAYERPTR == 0x5B34 &&
              PLAY_PLAYER_STATUS0 == 0x5CD8 && PLAY_DLST == 0x5D30, "play offsets");

inline dBgS* dComIfG_Bgsp() { return gabi::at<dBgS>(dComIfGp_ea() + PLAY_BGS); }
inline cCcS* dComIfG_Ccsp() { return gabi::at<cCcS>(dComIfGp_ea() + PLAY_CCS); }
inline dEvent_manager_c* dComIfGp_getPEvtManager() { return gabi::at<dEvent_manager_c>(dComIfGp_ea() + PLAY_EVTMANAGER); }
inline dEvt_control_c* dComIfGp_getEvent() { return gabi::at<dEvt_control_c>(dComIfGp_ea() + PLAY_EVTCTRL); }
inline dAttention_c* dComIfGp_getAttention() { return gabi::at<dAttention_c>(dComIfGp_ea() + PLAY_ATTENTION); }
inline dVibration_c* dComIfGp_getVibration() { return gabi::at<dVibration_c>(dComIfGp_ea() + PLAY_VIBRATION); }
inline dPa_control_c* dComIfGp_getParticle() { return gabi::at<dPa_control_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PARTICLE)); }
inline fopAc_ac_c* dComIfGp_getPlayer(int i) { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER + 8 * i)); }
inline fopAc_ac_c* dComIfGp_getLinkPlayer() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR)); }

/* 0200E240 cCcS::Set(cCcD_Obj*) [v kamome] */
inline void cCcS_Set(cCcS* ccs, void* obj) { gabi::call(0x0200E240, ccs, obj); }
inline void dComIfG_Ccsp_Set(void* obj) { cCcS_Set(dComIfG_Ccsp(), obj); }

/* ---- save info (HD: through the pointer at 0x101F84DC) ---- */
/* offset of a dSv_info_c member from *(0x101F84DC) */
#define SAVE_OFF(member) ((u32)(0x20 + offsetof(dSv_info_c, member)))
inline dSv_info_c* dComIfGs_info() { return gabi::at<dSv_info_c>(gabi::load<u32>(0x101F84DC) + 0x20); }
/* 025BA0C0 dSv_info_c::isSwitch [v mtoge] */
inline BOOL dComIfGs_isSwitch(s32 no, s32 roomNo) { return dComIfGs_info()->isSwitch(no, roomNo); }
/* 025B9E38 dSv_info_c::onSwitch [v swc00] */
inline void dComIfGs_onSwitch(s32 no, s32 roomNo) { dComIfGs_info()->onSwitch(no, roomNo); }
/* 025B9F7C dSv_info_c::offSwitch [v swc00] */
inline void dComIfGs_offSwitch(s32 no, s32 roomNo) { dComIfGs_info()->offSwitch(no, roomNo); }
/* 025BA5D4 dSv_info_c::onActor [g] */
inline void dComIfGs_onActor(s32 no, s32 roomNo) { dComIfGs_info()->onActor(no, roomNo); }

/* ---- resources ---- */
inline cPhs_State dComIfG_resLoad(request_of_phase_process_class* p, const char* arc) { return gabi::call<cPhs_State>(0x02520460, p, arc); }
inline BOOL dComIfG_resDelete(request_of_phase_process_class* p, const char* arc) { return gabi::call<BOOL>(0x025204C8, p, arc); }
/* HD: resource names are passed as sead::SafeString temporaries {const char* mStringTop; vtable}
 * (GHS places the vtable pointer after the members). Each translation unit has its own copy of
 * the SafeString vtable, so the caller passes its TU's vtable address. */
struct SafeString {
    be<u32> mStringTop;
    be<u32> __vtbl;
};
inline dRes_control_c* dComIfG_resControl() { return gabi::at<dRes_control_c>(gabi::load<u32>(0x101F4F28)); }
/* 026066C4 HD: dRes_control_c::getRes(const sead::SafeString& arc, s32 index) [v] */
inline void* dComIfG_getObjectRes(const char* arc, s32 index, u32 safestring_vtbl) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_addr(arc); /* a guest string, or a host literal of a GameCube-style unit */
    key->__vtbl = safestring_vtbl;
    return gabi::call<void*>(0x026066C4, dComIfG_resControl(), key.get(), index);
}

/* ---- events (dEvent_manager_c at play+0x52C4) [g] ---- */
inline s32 dComIfGp_evmng_getMyStaffId(const char* name, fopAc_ac_c* actor, s32 tag) {
    return gabi::call<s32>(0x02542D88, dComIfGp_getPEvtManager(), name, actor, tag);
}
inline void dComIfGp_evmng_cutEnd(s32 staffId) { gabi::call(0x02543280, dComIfGp_getPEvtManager(), staffId); }
inline BOOL dComIfGp_evmng_endCheck(s16 eventIdx) { return gabi::call<BOOL>(0x025440C8, dComIfGp_getPEvtManager(), eventIdx); }
inline BOOL dComIfGp_evmng_startCheck(s16 eventIdx) { return gabi::call<BOOL>(0x0254407C, dComIfGp_getPEvtManager(), eventIdx); }
inline s16 dComIfGp_evmng_getEventIdx(const char* name, u8 evNo) {
    return gabi::call<s16>(0x02543F10, dComIfGp_getPEvtManager(), name, evNo);
}
inline BOOL dComIfGp_evmng_getIsAddvance(s32 staffId) { return gabi::call<BOOL>(0x025447C8, dComIfGp_getPEvtManager(), staffId); }
inline void* dComIfGp_evmng_getMySubstanceP(s32 staffId, const char* name, s32 type) {
    return gabi::call<void*>(0x0254487C, dComIfGp_getPEvtManager(), staffId, name, type);
}

/* 02520540 dComIfGp_getReverb(roomNo) [v] */
inline s32 dComIfGp_getReverb(s32 roomNo) { return gabi::call<s32>(0x02520540, roomNo); }
