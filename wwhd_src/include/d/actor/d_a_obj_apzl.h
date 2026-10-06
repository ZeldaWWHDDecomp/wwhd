#pragma once
#include "bindings.h"
struct ApzlBtp_l {
  be<f32> speed, frame;
  be<s16> startFrame, endFrame;
  u8 opaque[0x68];
};
WWHD_SIZE(ApzlBtp_l, 0x74);
struct daObjApzl_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mpPieceModel[16], mpArrowModel[4], mpScoreboardModel[16];
  ApzlBtp_l mScoreboardBtpAnm[16];
  be<u8> mPuzzleNo;
  u8 padB85[3];
  gptr<u8> stick;
  be<u8> mType, mBlankIdx, mSwappedPieceIdx, mMoveTimer, mMoveDirection,
      mShowBlankPiece;
  be<u8> mPiecePos[16], mState, mDrawArrow[4];
  u8 padBA7;
  be<s16> mEventIdx[3];
  be<s8> mActIdx;
  be<u8> mQuitGame, mGameStarted, mGameCleared, mGaveReward,
      mShownRewardMessage;
  be<u8> mPlayedStartSound;
  u8 padBB5[3];
  be<u32> mRupeeIds[30];
  be<s32> mGivenRupeeCount;
  be<u32> mMsgNo, mCurrMsgId;
  be<u8> mMessageStarted;
  u8 padC3D[3];
  be<s32> mRewardTimer;
  BOOL CreateHeap();
  void randamize_piece();
  BOOL check_clear();
  void set_mtx();
  void CreateInit();
  s32 _create();
  BOOL _delete();
  BOOL _draw();
  BOOL _execute();
  u32 getMsg();
  u16 next_msgStatus(u32 *);
  u16 talk(s32);
  void privateCut();
  void save_piece();
  BOOL move_piece();
  void check_arrow_draw();
  void swap_piece(u8, u8);
  u8 search_piece(u8);
  u8 getblank() { return mPiecePos[(u8)mBlankIdx]; }
};
WWHD_OFFSET(daObjApzl_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjApzl_c, mpPieceModel, 0x3B4);
WWHD_OFFSET(daObjApzl_c, mpArrowModel, 0x3F4);
WWHD_OFFSET(daObjApzl_c, mpScoreboardModel, 0x404);
WWHD_OFFSET(daObjApzl_c, mScoreboardBtpAnm, 0x444);
WWHD_OFFSET(daObjApzl_c, mPuzzleNo, 0xB84);
WWHD_OFFSET(daObjApzl_c, stick, 0xB88);
WWHD_OFFSET(daObjApzl_c, mPiecePos, 0xB92);
WWHD_OFFSET(daObjApzl_c, mState, 0xBA2);
WWHD_OFFSET(daObjApzl_c, mEventIdx, 0xBA8);
WWHD_OFFSET(daObjApzl_c, mRupeeIds, 0xBB8);
WWHD_OFFSET(daObjApzl_c, mMessageStarted, 0xC3C);
WWHD_SIZE(daObjApzl_c, 0xC44);
