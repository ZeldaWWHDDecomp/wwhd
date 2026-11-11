/** Squid-Hunt board. HD uses the native HUD and two models for each ship
 * length. */
#pragma once
#include "d/d_seafightgame.h"
#include "f_op/f_op_actor.h"
enum BoardResource {
  BDL_SHIP2 = 4,
  BDL_SHIP3,
  BDL_SHIP4,
  BDL_HIT,
  BDL_BOARD,
  BDL_CURSOR,
  BDL_MISS
};

struct J3DModel;
struct MgBoardJudgeTypeHD {
  be<s16> mType;
};
struct MgBoardStickControlHD {
  u8 mParameters[0x24];
  be<u32> mVtable;
};
struct MgBoardShipHD {
  be<u8> mPositions[4][2];
  be<u8> mLength;
  be<u8> mLife;  /* was mHitCount (misnamed): the view of dSeaFightGame_ship_data::mLife (0x9): put_ship 025BB198 sets the ship size, attack 025BB470 subtracts 1 per hit and sinks the ship at 0; it counts the remaining cells, not hits */
  be<u8> mState;
  be<u8> mX;
  be<u8> mY;
  be<u8> mLengthX;  /* was mDamage (misnamed): the view of dSeaFightGame_ship_data::mLengthX (0xD): put_ship 025BB198 stores the ship size when it lies along x, else 0; no damage role */
  be<u8> mLengthY;  /* was mVertical (misnamed): the view of dSeaFightGame_ship_data::mLengthY (0xE): put_ship 025BB198 stores the ship size when it lies along y, else 0; the board draw turns the model by -0x8000 while it is non-zero (one name per field) */
};
struct MgBoardPadHD {
  u8 mReserved[0x18];
  be<u32> mHeldButtons;
  u8 mReservedHeld[4];
  be<u32> mRepeatButtons;
  u8 mReservedRepeat[0x124 - 0x24];
  be<u32> mDpadButtons;
};
struct MgBoardHudCommandHD {
  u8 mData[0x30];
};
struct MgBoardHudHD {
  u8 mReserved[0x18];
  MgBoardHudCommandHD mCommand;
};
struct MgBoardHudManagerHD {
  u8 mReserved[0x1F8];
  gptr<MgBoardHudHD> mpScreen;
};
struct MgBoardArchiveHD {
  be<u32> mWords[2];
  be<u8> mTerminator;
};
struct MgBoardSafeStringHD {
  gptr<MgBoardArchiveHD> mpText;
  be<u32> mVtable;
};
struct MgBoardHeaderStaticsHD {
  be<f32> mMinAngle, mMaxAngle, mFarDistance, mFarLimit, mNearDistance,
      mNearLimit;
  be<u8> mRandomState, mMathState;
  u8 mPadding[2];
  be<u32> mObject[4];
};
struct MgBoardRegistrationHD {
  u8 mData[12];
};
struct MgBoardRegistrationTableHD {
  MgBoardRegistrationHD mEntries[3];
};
struct MgBoardGridHD {
  cXyz mPositions[8][8];
};
struct MgBoardBulletGridHD {
  cXyz mPositions[3][8];
};
struct MgBoardShipGridHD {
  cXyz mPositions[3];
};
struct daMgBoard_c : fopAc_ac_c {
  /* 0x3AC */ request_of_phase_process_class mPhase;
  /* 0x3B4 */ gptr<J3DModel> mpBoardModel, mpCursorModel;
  /* 0x3BC */ gptr<J3DModel> mpHitModel[20];
  /* 0x40C */ gptr<J3DModel> mpMissModel[32];
  /* 0x48C */ gptr<J3DModel> mpShip2Model[2], mpShip3Model[2], mpShip4Model[2];
  u8 mReservedModels[0x584 - 0x4A4];
  // HD: cursor coordinates are signed; the native input path clamps them after
  // each move.
  /* 0x584 */ be<s8> mBoardPosX, mBoardPosY;
  u8 mReservedCursor[2];
  /* 0x588 */ be<s32> mLastFirePosX, mLastFirePosY;
  /* 0x590 */ be<u16> mGameFlags;
  u8 mReservedFlags[2];
  be<u32> mGameResult;
  /* 0x598 */ dSeaFightGame_info_c mSeaFightGame;
  u8 mReservedGame[4];
  /* 0x69C */ be<s32> mMissModelCount, mHitModelCount;
  /* 0x6A4 */ be<s16> mMinigameStartIdx, mMinigameEndIdx;
  /* 0x6A8 */ MgBoardStickControlHD mStickControl;
  /* 0x6D0 */ cXyz mNPCPos;
  /* 0x6DC */ be<s32> mState;
  /* 0x6E0 */ be<u8> mbDraw, mbStartGame, mbEndGame, mbForceEnd, mTimer;
  u8 mReservedEnd[3];
  gptr<J3DModel> &shipModel(u32 group, u32 index) {
    if (group == 0)
      return mpShip2Model[index];
    if (group == 1)
      return mpShip3Model[index];
    return mpShip4Model[index];
  }
  enum State { STATE_INIT, STATE_WAIT, STATE_START, STATE_PLAY, STATE_END };
  BOOL CreateHeap();
  void MiniGameInit();
  void CreateInit();
  s32 _create();
  BOOL _delete();
  BOOL _draw();
  void ResetGame();
  void StartInput();
  u32 CursorMove();
  BOOL MinigameMain();
  BOOL execGameMain();
  void EndInput();
  BOOL _execute();
  bool checkClearGame();
  u8 getScore();
  void reqStartGame();
  bool checkEndGame();
  void setGInfoDraw();
  void clrGInfoDraw();
  void set_mtx();
};
WWHD_SIZE(MgBoardShipHD, 15);
WWHD_SIZE(MgBoardStickControlHD, 0x28);
WWHD_OFFSET(MgBoardPadHD, mDpadButtons, 0x124);
WWHD_OFFSET(MgBoardHudManagerHD, mpScreen, 0x1F8);
WWHD_SIZE(MgBoardArchiveHD, 9);
WWHD_SIZE(MgBoardSafeStringHD, 8);
WWHD_SIZE(MgBoardHeaderStaticsHD, 0x2C);
WWHD_OFFSET(daMgBoard_c, mpBoardModel, 0x3B4);
WWHD_OFFSET(daMgBoard_c, mBoardPosX, 0x584);
WWHD_OFFSET(daMgBoard_c, mSeaFightGame, 0x598);
WWHD_OFFSET(daMgBoard_c, mMissModelCount, 0x69C);
WWHD_OFFSET(daMgBoard_c, mStickControl, 0x6A8);
WWHD_OFFSET(daMgBoard_c, mNPCPos, 0x6D0);
WWHD_OFFSET(daMgBoard_c, mState, 0x6DC);
WWHD_SIZE(daMgBoard_c, 0x6E8);

WWHD_SIZE(MgBoardJudgeTypeHD, 2);
WWHD_SIZE(MgBoardPadHD, 0x128);
WWHD_OFFSET(MgBoardShipHD, mLength, 8);
WWHD_OFFSET(MgBoardShipHD, mX, 11);
WWHD_OFFSET(MgBoardShipHD, mLengthY, 14);
WWHD_SIZE(MgBoardHudCommandHD, 0x30);
WWHD_SIZE(MgBoardHudHD, 0x48);
WWHD_SIZE(MgBoardHudManagerHD, 0x1FC);
WWHD_SIZE(MgBoardRegistrationHD, 12);
WWHD_SIZE(MgBoardRegistrationTableHD, 36);
WWHD_SIZE(MgBoardGridHD, 0x300);
WWHD_SIZE(MgBoardBulletGridHD, 0x120);
WWHD_SIZE(MgBoardShipGridHD, 0x24);
