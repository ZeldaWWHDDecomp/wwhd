/* Full WWHD sliding puzzle actor, derived from zeldaret/tww. */
#include "d/actor/d_a_obj_apzl.h"
void daObjApzl_c::swap_piece(u8 first, u8 second) {
  WWHD_FUNC(0x02316DA0, void, this, first, second);
  u8 value = mPiecePos[second], old = mPiecePos[first];
  mPiecePos[first] = value;
  mPiecePos[second] = old;
}
VERIFY(0x02316DA0, &daObjApzl_c::swap_piece);
u8 daObjApzl_c::search_piece(u8 piece) {
  WWHD_FUNC(0x02316DB8, u8, this, piece);
  for (u32 i = 0;; ++i)
    if (gabi::load<u8>(gabi::ea(this) + 0xB92 + i) == piece)
      return (u8)i;
}
VERIFY(0x02316DB8, &daObjApzl_c::search_piece);
BOOL daObjApzl_c::check_clear() {
  WWHD_FUNC(0x02317014, BOOL, this);
  for (u32 i = 0; i < 16; ++i)
    if ((u8)mPiecePos[i] != (u8)i)
      return FALSE;
  return TRUE;
}
VERIFY(0x02317014, &daObjApzl_c::check_clear);
void daObjApzl_c::check_arrow_draw() {
  WWHD_FUNC(0x02318598, void, this);
  for (u32 i = 0; i < 4; ++i)
    mDrawArrow[i] = 0;
  if ((u8)mState != 3 || (u8)mMoveTimer != 0)
    return;
  if ((getblank() & 3) != 3)
    mDrawArrow[1] = 1;
  if ((getblank() & 3) != 0)
    mDrawArrow[3] = 1;
  if ((getblank() & 12) != 12)
    mDrawArrow[0] = 1;
  if ((getblank() & 12) != 0)
    mDrawArrow[2] = 1;
}
VERIFY(0x02318598, &daObjApzl_c::check_arrow_draw);
BOOL solidHeapCB(daObjApzl_c *p) {
  WWHD_FUNC(0x02316D9C, BOOL, p);
  return p->CreateHeap();
}
VERIFY(0x02316D9C, solidHeapCB);
s32 daObjApzl_c::_create() {
  WWHD_FUNC(0x02317878, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(base + 0xB4, 0x10024B14);
      gabi::call(0x028EFFD0, &mScoreboardBtpAnm[0], 16, 0x74, 0x025E7820);
      flags = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10024AF4));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, 0x02316D9C, 0x1460))
      return 5;
    CreateInit();
  }
  return phase;
}
VERIFY(0x02317878, &daObjApzl_c::_create);
BOOL daObjApzl_c::_delete() {
  WWHD_FUNC(0x02317954, BOOL, this);
  u32 controller = gabi::ea((u8 *)stick);
  if (controller) {
    u32 vt = gabi::load<u32>(controller + 0x24);
    gabi::call(gabi::load<u32>(vt + 0xC), gabi::at<u8>(controller), 3);
  }
  gabi::call(0x025204C8, &mPhase, STR(0x10024FE4));
  return TRUE;
}
VERIFY(0x02317954, &daObjApzl_c::_delete);
BOOL IsDelete(daObjApzl_c *p) {
  WWHD_FUNC(0x02318A14, BOOL, p);
  return TRUE;
}
VERIFY(0x02318A14, IsDelete);
void destruct(daObjApzl_c *p, s32 flags) {
  WWHD_FUNC(0x02318A1C, void, p, flags);
  if (p) {
    gabi::call(0x025D50BC, p, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, p);
  }
}
VERIFY(0x02318A1C, destruct);
void emptyVirtual() { WWHD_FUNC(0x02318A70, void); }
VERIFY(0x02318A70, emptyVirtual);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x02318A00, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x02318A00, deleteStatic);

u32 daObjApzl_c::getMsg() {
  WWHD_FUNC(0x02317B4C, u32, this);
  if ((u8)mShownRewardMessage)
    return 0x1BC5;
  if ((u8)mGaveReward)
    return 0x1BD0;
  if ((u8)mGameCleared)
    return 0x1BCF;
  return (u8)mGameStarted ? 0x1BCD : 0x1BC6;
}
VERIFY(0x02317B4C, &daObjApzl_c::getMsg);
u16 daObjApzl_c::next_msgStatus(u32 *msg) {
  WWHD_FUNC(0x02317BA0, u16, this, msg);
  u32 id = gabi::load<u32>(gabi::ea(msg)),
      message = gabi::load<u32>(0x101F4B5C);
  auto assign = [&](u32 n) { gabi::store<u32>(gabi::ea(msg), n); };
  switch (id) {
  case 0x1BC6:
    assign(gabi::load<s32>(message + 0x948) ? 0x1BC8 : 0x1BC7);
    return 15;
  case 0x1BC7:
    assign(gabi::load<s32>(message + 0x948) ? 0x1BC9 : 0x1BCA);
    return 15;
  case 0x1BCA:
    assign(0x1BCB);
    return 15;
  case 0x1BCB:
    assign(gabi::load<s32>(message + 0x948) ? 0x1BCC : 0x1BC9);
    return 15;
  case 0x1BCC:
    assign(0x1BCA);
    return 15;
  case 0x1BCD:
    if (gabi::load<s32>(message + 0x948)) {
      assign(0x1BCE);
      return 15;
    }
    return 16;
  case 0x1BC9:
  case 0x1BCF:
    return 16;
  case 0x1BD0:
    mShownRewardMessage = 1;
    return 16;
  default:
    mQuitGame = 1;
    return 16;
  }
}
VERIFY(0x02317BA0, &daObjApzl_c::next_msgStatus);
u16 daObjApzl_c::talk(s32 mode) {
  WWHD_FUNC(0x02317CB0, u16, this, mode);
  u32 current = mCurrMsgId;
  u32 message = gabi::load<u32>(0x101F4B5C);
  u16 result = 255;
  if (current == 0xFFFFFFFF) {
    u32 id;
    if (mode == 1) {
      id = getMsg();
      mMsgNo = id;
    } else
      id = mMsgNo;
    u32 next = gabi::call<u32>(0x025F7DB0, gabi::at<u8>(message), id, &eyePos);
    mCurrMsgId = next;
    if (next != 0xFFFFFFFF)
      mMessageStarted = 0;
  } else if ((u8)mMessageStarted) {
    result = (u16)gabi::call<u32>(0x025F795C, gabi::at<u8>(message));
    if (result == 14) {
      u16 status = next_msgStatus((u32 *)&mMsgNo);
      gabi::call(0x025F74D0, gabi::at<u8>(message), status);
      if (gabi::call<s32>(0x025F795C, gabi::at<u8>(message)) == 15) {
        u32 id = mMsgNo;
        gabi::call(0x025F7DB0, gabi::at<u8>(message), id, 0);
      }
    } else if (result == 18) {
      gabi::call(0x025F74D0, gabi::at<u8>(message), 19);
      mCurrMsgId = 0xFFFFFFFF;
    }
  } else
    mMessageStarted = 1;
  return result;
}
VERIFY(0x02317CB0, &daObjApzl_c::talk);

BOOL daObjApzl_c::CreateHeap() {
  WWHD_FUNC(0x02316AB0, BOOL, this);
  for (u32 i = 0; i < 16; ++i) {
    s16 override = gabi::load<s16>(0x1047C80A);
    if (override > 0) {
      override = gabi::load<s16>(0x1047C80A);
      s32 puzzle = override - 1;
      u32 save = gabi::load<u32>(0x101F84DC);
      if (puzzle >= 16)
        gabi::store<s16>(0x1047C80A, 0);
      gabi::store<u8>(save + 0x1BF, (u8)puzzle);
    }
    u32 save = gabi::load<u32>(0x101F84DC);
    u8 puzzle = gabi::load<u8>(save + 0x1BF);
    if (puzzle >= 16)
      puzzle = 0;
    u32 resource = gabi::load<u32>(0x10024B24 + ((u32)puzzle * 16 + i) * 4);
    gabi::Local<SafeString> key;
    // Reserve caller linkage space below the SafeString passed to actual callees.
    gabi::Local<be<u32>> keyLinkage;
    key->mStringTop = 0x10024F30;
    key->__vtbl = 0x10024AFC;
    void *data = gabi::call<void *>(0x026066C4,
                                    gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                                    key.get(), resource);
    if (!data)
      gabi::call(0x0273AA24, STR(0x10024F48), 0x324, STR(0x10024F5C));
    mpPieceModel[i] =
        gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x37441422);
    if (!mpPieceModel[i])
      return FALSE;
  }
  gabi::Local<SafeString> arrows;
  gabi::Local<be<u32>> arrowsLinkage;
  arrows->mStringTop = 0x10024F30;
  arrows->__vtbl = 0x10024AFC;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), arrows.get(), 7);
  if (!data)
    gabi::call(0x0273AA24, STR(0x10024F48), 0x340, STR(0x10024F5C));
  for (u32 i = 0; i < 4; ++i) {
    mpArrowModel[i] = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
    if (!mpArrowModel[i])
      return FALSE;
  }
  gabi::Local<SafeString> scores;
  gabi::Local<be<u32>> scoresLinkage;
  scores->mStringTop = 0x10024F30;
  scores->__vtbl = 0x10024AFC;
  data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), scores.get(), 10);
  if (!data)
    gabi::call(0x0273AA24, STR(0x10024F48), 0x34B, STR(0x10024F5C));
  gabi::Local<SafeString> animationName;
  gabi::Local<be<u32>> animationNameLinkage;
  animationName->mStringTop = 0x10024F30;
  animationName->__vtbl = 0x10024AFC;
  void *animation =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         animationName.get(), 0x10F);
  if (!animation)
    gabi::call(0x0273AA24, STR(0x10024F48), 0x350, STR(0x10024F38));
  for (u32 i = 0; i < 16; ++i) {
    mpScoreboardModel[i] =
        gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
    if (!mpScoreboardModel[i])
      return FALSE;
    if (!gabi::call<s32>(0x025E789C, &mScoreboardBtpAnm[i], data, animation, 1,
                         0, 1.0f, 0, -1, 0, 0))
      return FALSE;
  }
  stick = gabi::call<u8 *>(0x0258861C, 0, 60, 30, 0, 0, 0.9f, 0.5f, 0, 0);
  if (!stick)
    gabi::call(0x0273AA24, STR(0x10024F48), 0x361, STR(0x10024F70));
  return TRUE;
}
VERIFY(0x02316AB0, &daObjApzl_c::CreateHeap);

void daObjApzl_c::save_piece() {
  WWHD_FUNC(0x0231839C, void, this);
  for (u32 i = 0; i < 16; ++i) {
    u32 save = gabi::load<u32>(0x101F84DC);
    u8 position = mPiecePos[i];
    gabi::store<u8>(save + 0x1AF + i, position);
  }
}
VERIFY(0x0231839C, &daObjApzl_c::save_piece);
void daObjApzl_c::randamize_piece() {
  WWHD_FUNC(0x02316DDC, void, this);
  for (u32 i = 0; i < 16; ++i)
    mPiecePos[i] = (u8)i;
  for (;;) {
    for (u32 step = 0; step < 10000; ++step) {
      u32 direction = (u32)gabi::ftoi(gabi::call<f32>(0x020198D8, 4.0f)) & 3;
      u8 blank = mBlankIdx, position = mPiecePos[blank];
      if (direction == 1) {
        if ((position & 3) != 3) {
          u8 piece = search_piece((u8)(position + 1));
          swap_piece(blank, piece);
        }
      } else if (direction == 3) {
        if ((position & 3) != 0) {
          u8 piece = search_piece((u8)(position - 1));
          swap_piece(blank, piece);
        }
      } else if (direction == 0) {
        if ((position & 12) != 12) {
          u8 piece = search_piece((u8)(position + 4));
          swap_piece(blank, piece);
        }
      } else if ((position & 12) != 0) {
        u8 piece = search_piece((u8)(position - 4));
        swap_piece(blank, piece);
      }
    }
    u8 matches = 0;
    for (u32 i = 0; i < 16; ++i)
      if ((u8)mPiecePos[i] == (u8)i)
        ++matches;
    if (matches < 3)
      break;
  }
  for (u32 step = 0; step < 3; ++step) {
    u8 blank = mBlankIdx, position = mPiecePos[blank];
    if ((position & 3) != 3) {
      u8 piece = search_piece((u8)(position + 1));
      swap_piece(blank, piece);
      blank = mBlankIdx;
      position = mPiecePos[blank];
    }
    if ((position & 12) != 0) {
      u8 piece = search_piece((u8)(position - 4));
      swap_piece(blank, piece);
    }
  }
}
VERIFY(0x02316DDC, &daObjApzl_c::randamize_piece);

void staticInitialize() {
  WWHD_FUNC(0x0231896C, void);
  gabi::store<u32>(0x104690D0, 0);
  gabi::store<u32>(0x104690C8, 0);
  gabi::store<u32>(0x104690D4, 0);
  gabi::store<u32>(0x104690CC, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7C00));
  gabi::store<f32>(0x104690BC, -3.1415927410125732f);
  gabi::store<f32>(0x104690C0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x104690C4));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7C0C));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x104690C5));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7C18));
}
VERIFY(0x0231896C, staticInitialize);

BOOL daObjApzl_c::move_piece() {
  WWHD_FUNC(0x023183CC, BOOL, this);
  gabi::call(0x0258873C, (u8 *)stick);
  u8 timer = mMoveTimer;
  if (timer) {
    mMoveTimer = (u8)(timer - 1);
    return FALSE;
  }
  u32 controller = gabi::load<u32>(0x101F5088);
  u32 repeated = gabi::load<u32>(controller + 0x20),
      restrict = gabi::load<u32>(controller + 0x124),
      pressed = gabi::load<u32>(controller + 0x18);
  u32 keys = pressed | repeated;
  if (restrict & 0xF00000)
    keys &= 0xF00000;
  bool swapped = false;
  u8 blank = mBlankIdx, position = mPiecePos[blank];
  if ((position & 3) != 3 && (keys & 0x440000)) {
    mMoveDirection = 1;
    position = getblank();
    u8 piece = search_piece((u8)(position + 1));
    blank = mBlankIdx;
    mSwappedPieceIdx = piece;
    position = mPiecePos[blank];
    swapped = true;
  }
  if ((position & 3) != 0 && (keys & 0x880000)) {
    mMoveDirection = 3;
    position = getblank();
    u8 piece = search_piece((u8)(position - 1));
    blank = mBlankIdx;
    mSwappedPieceIdx = piece;
    position = mPiecePos[blank];
    swapped = true;
  }
  if ((position & 12) != 12 && (keys & 0x110000)) {
    mMoveDirection = 0;
    position = getblank();
    u8 piece = search_piece((u8)(position + 4));
    blank = mBlankIdx;
    mSwappedPieceIdx = piece;
    position = mPiecePos[blank];
    swapped = true;
  }
  if ((position & 12) != 0 && (keys & 0x220000)) {
    mMoveDirection = 2;
    position = getblank();
    u8 piece = search_piece((u8)(position - 4));
    blank = mBlankIdx;
    mSwappedPieceIdx = piece;
    swap_piece(blank, piece);
    mMoveTimer = check_clear() ? 40 : 5;
    return TRUE;
  }
  if (swapped) {
    u8 piece = mSwappedPieceIdx;
    swap_piece(blank, piece);
    mMoveTimer = check_clear() ? 40 : 5;
  }
  return swapped;
}
VERIFY(0x023183CC, &daObjApzl_c::move_piece);

BOOL daObjApzl_c::_draw() {
  WWHD_FUNC(0x023179B0, BOOL, this);
  u8 type = mType;
  u32 play = gabi::call<u32>(0x025200D4), list = gabi::load<u32>(play + 0x5D70);
  gabi::store<u32>(0x104B4634, list);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  for (u32 i = 0; i < 16; ++i)
    if (type || i != (u8)mBlankIdx || (u8)mShowBlankPiece)
      gabi::call(0x025E2DE0, (J3DModel *)mpPieceModel[i], 0);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  if (!type) {
    for (u32 i = 0; i < 4; ++i)
      if ((u8)mDrawArrow[i])
        gabi::call(0x025E2DE0, (J3DModel *)mpArrowModel[i], 0);
    for (u32 i = 0; i < 16; ++i) {
      f32 frame = mScoreboardBtpAnm[i].frame;
      u32 model = gabi::ea((J3DModel *)mpScoreboardModel[i]);
      void *data = gabi::at<u8>(gabi::load<u32>(model + 0xAC));
      gabi::call(0x025E7B3C, &mScoreboardBtpAnm[i], data,
                 (s16)gabi::ftoi(frame));
      gabi::call(0x025E2DE0, (J3DModel *)mpScoreboardModel[i], 0);
      model = gabi::ea((J3DModel *)mpScoreboardModel[i]);
      u32 md = gabi::load<u32>(model + 0xAC);
      gabi::store<u32>(md + 0x38, 0);
    }
  }
  return TRUE;
}
VERIFY(0x023179B0, &daObjApzl_c::_draw);
void daObjApzl_c::CreateInit() {
  WWHD_FUNC(0x023175B8, void, this);
  u32 base = gabi::ea(this);
  mType = (u8)(gabi::load<u32>(base + 0xB0) >> 8);
  u32 save = gabi::load<u32>(0x101F84DC);
  u8 puzzle = gabi::load<u8>(save + 0x1BF);
  if (puzzle >= 16)
    puzzle = 0;
  mPuzzleNo = puzzle;
  for (u32 i = 0; i < 16; ++i) {
    mScoreboardBtpAnm[i].frame = i < puzzle ? 1.0f : 0.0f;
    puzzle = mPuzzleNo;
  }
  u32 model = gabi::ea((J3DModel *)mpPieceModel[0]);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, this, -600.0f, -600.0f, -600.0f, 600.0f, 600.0f,
             600.0f);
  u8 type = mType;
  mShowBlankPiece = 0;
  mBlankIdx = 3;
  gabi::store<f32>(base + 0x364, 1.0f);
  if (!type)
    randamize_piece();
  else
    for (u32 i = 0; i < 16; ++i)
      mPiecePos[i] = (u8)i;
  u32 play = gabi::call<u32>(0x025200D4);
  mEventIdx[0] = gabi::call<s32>(0x02543F10, gabi::at<u8>(play + 0x52C4),
                                 STR(0x10024FBC), 255);
  play = gabi::call<u32>(0x025200D4);
  mEventIdx[1] = gabi::call<s32>(0x02543F10, gabi::at<u8>(play + 0x52C4),
                                 STR(0x10024FC8), 255);
  play = gabi::call<u32>(0x025200D4);
  s32 rewardEvent = gabi::call<s32>(0x02543F10, gabi::at<u8>(play + 0x52C4),
                                    STR(0x10024FD4), 255);
  type = mType;
  u32 y = gabi::load<u32>(base + 0x318);
  s16 talkEvent = mEventIdx[0];
  mEventIdx[2] = rewardEvent;
  gabi::store<s16>(base + 0xFC, talkEvent);
  if (!type) {
    u32 flags = gabi::load<u32>(base + 0x39C);
    gabi::store<u32>(base + 0x39C, flags | 0x20000008);
  }
  gabi::store<u32>(base + 0x394, y);
  mMessageStarted = 0;
  mState = 0;
  mMoveTimer = 0;
  mQuitGame = 0;
  u32 x = gabi::load<u32>(base + 0x314);
  mGameCleared = 0;
  gabi::store<u8>(base + 0x389, 40);
  mShownRewardMessage = 0;
  u32 z = gabi::load<u32>(base + 0x31C);
  mCurrMsgId = 0xFFFFFFFF;
  mPlayedStartSound = 0;
  gabi::store<u32>(base + 0x398, z);
  mGaveReward = 0;
  mGameStarted = 0;
  mSwappedPieceIdx = 0;
  gabi::store<u32>(base + 0x390, x);
  mMoveDirection = 0;
  for (u32 i = 0; i < 4; ++i)
    mDrawArrow[i] = 0;
  mGivenRupeeCount = 0;
  set_mtx();
}
VERIFY(0x023175B8, &daObjApzl_c::CreateInit);

static inline void apzlCopyMatrix(u32 model) {
  f32 values[12];
  for (u32 j = 0; j < 12; ++j)
    values[j] = gabi::load<f32>(0x1048D0CC + j * 4);
  for (u32 j = 0; j < 12; ++j)
    gabi::store<f32>(model + 0xC8 + j * 4, values[j]);
}
void daObjApzl_c::set_mtx() {
  WWHD_FUNC(0x0231704C, void, this);
  constexpr u32 matrix = 0x1048D0CC;
  for (u32 i = 0; i < 16; ++i) {
    u32 model = gabi::ea((J3DModel *)mpPieceModel[i]);
    f32 z = scale.z, x = scale.x, y = scale.y;
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC0, y);
    gabi::store<f32>(model + 0xC4, z);
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    gabi::call(0x028E93CC, gabi::at<u8>(matrix), x, y, z);
    s16 angle = current.angle.y;
    gabi::call(0x025F1C28, gabi::at<u8>(matrix), angle);
    u8 position = mPiecePos[i];
    x = gabi::fmadds((f32)(position % 4), 40.0f, -60.0f);
    y = -gabi::fmadds((f32)(position / 4), 40.0f, -60.0f);
    gabi::call(0x025F24E0, x, y, 0.0f);
    if (i == (u8)mSwappedPieceIdx) {
      u8 timer = mMoveTimer;
      if (timer) {
        BOOL clear = check_clear();
        u8 direction = mMoveDirection;
        f32 distance = (clear ? 1.0f : 8.0f) * (f32)timer;
        if (!direction)
          gabi::call(0x025F24E0, 0.0f, -distance, 0.0f);
        else if (direction == 1)
          gabi::call(0x025F24E0, distance, 0.0f, 0.0f);
        else if (direction == 2)
          gabi::call(0x025F24E0, 0.0f, distance, 0.0f);
        else
          gabi::call(0x025F24E0, -distance, 0.0f, 0.0f);
      }
    }
    model = gabi::ea((J3DModel *)mpPieceModel[i]);
    apzlCopyMatrix(model);
  }
  for (u32 i = 0; i < 4; ++i) {
    u32 model = gabi::ea((J3DModel *)mpArrowModel[i]);
    f32 z = scale.z, x = scale.x, y = scale.y;
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC0, y);
    gabi::store<f32>(model + 0xC4, z);
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    gabi::call(0x028E93CC, gabi::at<u8>(matrix), x, y, z);
    s16 angle = current.angle.y;
    gabi::call(0x025F1C28, gabi::at<u8>(matrix), angle);
    u8 position = getblank();
    x = gabi::fmadds((f32)(position % 4), 40.0f, -60.0f);
    y = -gabi::fmadds((f32)(position / 4), 40.0f, -60.0f);
    gabi::call(0x025F24E0, x, y, 0.0f);
    gabi::call(0x025F1C5C, gabi::at<u8>(matrix), (s16)(i * 0x4000));
    gabi::call(0x025F24E0, 0.0f, -15.0f, 0.0f);
    model = gabi::ea((J3DModel *)mpArrowModel[i]);
    apzlCopyMatrix(model);
  }
  for (u32 i = 0; i < 16; ++i) {
    f32 y = scale.y, x = scale.x;
    u32 model = gabi::ea((J3DModel *)mpScoreboardModel[i]);
    f32 z = scale.z;
    gabi::store<f32>(model + 0xC0, y);
    gabi::store<f32>(model + 0xC4, z);
    gabi::store<f32>(model + 0xBC, x);
    y = gabi::fmadds(28.6200008392334f, -(f32)((i % 8) + 1), 10.0f);
    z = 28.6200008392334f * (f32)((s32)(i / 8) - 2);
    gabi::call(0x028E93CC, gabi::at<u8>(matrix), 0.0f, y, z);
    model = gabi::ea((J3DModel *)mpScoreboardModel[i]);
    apzlCopyMatrix(model);
  }
}
VERIFY(0x0231704C, &daObjApzl_c::set_mtx);

BOOL daObjApzl_c::_execute() {
  WWHD_FUNC(0x02318640, BOOL, this);
  if (!(u8)mType) {
    u32 base = gabi::ea(this);
    switch ((u8)mState) {
    case 0: {
      u16 condition = gabi::load<u16>(base + 0xFA),
          command = gabi::load<u16>(base + 0xF8);
      gabi::store<u16>(base + 0xFA, condition | 1);
      if (command == 1)
        mState = 1;
      break;
    }
    case 1: {
      privateCut();
      s16 event = mEventIdx[0];
      u32 play = gabi::call<u32>(0x025200D4);
      if (gabi::call<s32>(0x025440C8, gabi::at<u8>(play + 0x52C4), event)) {
        if ((u8)mQuitGame) {
          play = gabi::call<u32>(0x025200D4);
          u16 flags = gabi::load<u16>(play + 0x52B8);
          gabi::store<u16>(play + 0x52B8, flags | 8);
          mGameStarted = 0;
          mState = 0;
          mQuitGame = 0;
          save_piece();
        } else
          mState = 2;
      }
      break;
    }
    case 2: {
      if (!(u8)mPlayedStartSound) {
        gabi::call(0x025E1988, 0x8F2);
        mPlayedStartSound = 1;
      }
      s16 event = mEventIdx[1];
      gabi::call(0x025D7874, this, event, 0, 65535);
      mState = 3;
      break;
    }
    case 3: {
      privateCut();
      if (move_piece())
        gabi::call(0x025E1988, 0x83D);
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::store<u8>(play + 0x5BBA, 0);
      play = gabi::call<u32>(0x025200D4);
      gabi::store<u8>(play + 0x5BB9, 39);
      if (check_clear() && !(u8)mMoveTimer) {
        mShowBlankPiece = 1;
        mGameCleared = 1;
        play = gabi::call<u32>(0x025200D4);
        gabi::Local<cXyz> direction;
        direction->y = 1.0f;
        direction->x = 0.0f;
        direction->z = 0.0f;
        gabi::call(0x025CB374, gabi::at<u8>(play + 0x599C), 4, 1,
                   direction.get());
        mState = 4;
      } else if (gabi::call<s32>(0x020076E0, 0)) {
        gabi::call(0x025E1988, 0x8F3);
        mState = 4;
      }
      break;
    }
    case 4: {
      u8 cleared = mGameCleared;
      mQuitGame = 0;
      if (cleared) {
        u32 save = gabi::load<u32>(0x101F84DC);
        u8 count = (u8)(gabi::load<u8>(save + 0x1BF) + 1), puzzle = mPuzzleNo;
        if (count >= 16)
          count = 0;
        gabi::store<f32>(base + 0x448 + (u32)puzzle * 0x74, 1.0f);
        save = gabi::load<u32>(0x101F84DC);
        gabi::store<u8>(save + 0x1BF, count);
        s16 event = mEventIdx[2];
        gabi::call(0x025D7874, this, event, 0, 65535);
        mState = 5;
      } else {
        s16 event = mEventIdx[0];
        gabi::call(0x025D7874, this, event, 0, 65535);
        mState = 1;
      }
      [[fallthrough]];
    }
    case 5: {
      privateCut();
      s16 event = mEventIdx[2];
      u32 play = gabi::call<u32>(0x025200D4);
      if (gabi::call<s32>(0x025440C8, gabi::at<u8>(play + 0x52C4), event)) {
        play = gabi::call<u32>(0x025200D4);
        u16 flags = gabi::load<u16>(play + 0x52B8);
        gabi::store<u16>(play + 0x52B8, flags | 8);
        mState = 0;
        mGameStarted = 0;
        mQuitGame = 0;
      }
      break;
    }
    }
    check_arrow_draw();
  }
  set_mtx();
  return TRUE;
}
VERIFY(0x02318640, &daObjApzl_c::_execute);

void daObjApzl_c::privateCut() {
  WWHD_FUNC(0x02317DE4, void, this);
  u32 play = gabi::call<u32>(0x025200D4);
  s32 staff = gabi::call<s32>(0x02542D88, gabi::at<u8>(play + 0x52C4),
                              STR(0x10025008), 0, 0);
  if (staff == -1)
    return;
  play = gabi::call<u32>(0x025200D4);
  s8 action = gabi::call<s32>(0x02542EDC, gabi::at<u8>(play + 0x52C4), staff,
                              gabi::at<u8>(0x101C7BE0), 8, 1, 0);
  mActIdx = action;
  play = gabi::call<u32>(0x025200D4);
  if (action == -1) {
    gabi::call(0x02543280, gabi::at<u8>(play + 0x52C4), staff);
    return;
  }
  if (gabi::call<s32>(0x025447C8, gabi::at<u8>(play + 0x52C4), staff)) {
    action = mActIdx;
    switch (action) {
    case 2:
      mGameStarted = 1;
      break;
    case 3:
      mRewardTimer = 150;
      mGaveReward = 1;
      break;
    case 5:
      gabi::call(0x025E1988, 0x8F4);
      break;
    case 7: {
      for (s32 i = 0; i < (s32)mGivenRupeeCount; ++i) {
        gabi::Local<be<u32>> id;
        id.get()[0] = mRupeeIds[i];
        u32 item = 0;
        if ((u32)id.get()[0] != 0xFFFFFFFF)
          item = gabi::call<u32>(0x025D5218, 0x025E1234, id.get());
        if (item) {
          gabi::store<s16>(item + 0x320, 0);
          gabi::store<s16>(item + 0x322, 0);
          gabi::store<s16>(item + 0x324, 0);
        }
      }
      break;
    }
    }
  }
  action = mActIdx;
  bool finish = false;
  switch (action) {
  case 1:
    finish = talk(1) == 18;
    break;
  case 3: {
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(play + 0x5BBA, 0);
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(play + 0x5BB9, 0);
    s32 timer = mRewardTimer;
    if (!timer) {
      finish = true;
      break;
    }
    timer = (s32)((u32)timer - 1);
    mRewardTimer = timer;
    if ((u32)timer & 2)
      break;
    s32 count = mGivenRupeeCount;
    if (count >= 30)
      break;
    gabi::Local<csXyz> angle;
    gabi::Local<cXyz> position;
    gabi::Local<cXyz> linkage;
    s16 ax = current.angle.x, ay = current.angle.y, az = current.angle.z;
    angle->z = az;
    angle->x = ax;
    position->x = 0;
    angle->y = ay;
    position->y = 100.0f;
    position->z = 735.0f;
    s16 rx = (s16)gabi::ftoi(gabi::call<f32>(0x02019918, 4000.0f));
    angle->x = (s16)(ax + rx);
    s16 ry = (s16)gabi::ftoi(gabi::call<f32>(0x02019918, 6000.0f));
    ay = angle->y;
    u8 puzzle = mPuzzleNo;
    count = mGivenRupeeCount;
    angle->z = 0;
    angle->y = (s16)(ay + ry - 0x4000);
    u32 itemNo;
    if (puzzle == 15) {
      u8 reward = gabi::load<u8>(0x101C7C54 + (u32)count);
      itemNo = reward == 0   ? 1
               : reward == 1 ? 2
               : reward == 2 ? 3
               : reward == 3 ? 4
                             : 5;
    } else
      itemNo = count % 6 == 0 ? 2 : 1;
    f32 velocity = gabi::call<f32>(0x020198D8, 15.0f) + 5.0f;
    f32 height = gabi::call<f32>(0x020198D8, 15.0f) + 5.0f;
    s8 room = gabi::load<s8>(gabi::ea(this) + 0x326);
    u32 item =
        gabi::call<u32>(0x025D8AB0, position.get(), itemNo, room, angle.get(),
                        0, -1, 0, velocity, height, -2.0999999046325684f);
    if (item) {
      u32 flags = gabi::load<u32>(item + 0x2E0);
      gabi::store<u32>(item + 0x2E0, flags | 0x4000);
      gabi::call(0x0218084C, gabi::at<u8>(item), 450);
    }
    count = mGivenRupeeCount;
    u32 id = item ? gabi::load<u32>(item + 4) : 0xFFFFFFFF;
    gabi::store<u32>(gabi::ea(this) + 0xBB8 + (u32)count * 4, id);
    count = mGivenRupeeCount;
    mGaveReward = 1;
    mGivenRupeeCount = (s32)((u32)count + 1);
    mRewardTimer = 60;
    return;
  }
  case 4:
    return;
  case 5:
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(play + 0x5BBA, 0);
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(play + 0x5BB9, 0);
    finish = !gabi::call<s32>(0x025E1B24, 0x8F4);
    break;
  case 6:
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(play + 0x5BBA, 25);
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(play + 0x5BB9, 0);
    finish = gabi::call<s32>(0x020076BC, 0) != 0;
    break;
  default:
    finish = true;
    break;
  }
  if (finish) {
    play = gabi::call<u32>(0x025200D4);
    gabi::call(0x02543280, gabi::at<u8>(play + 0x52C4), staff);
  }
}
VERIFY(0x02317DE4, &daObjApzl_c::privateCut);
