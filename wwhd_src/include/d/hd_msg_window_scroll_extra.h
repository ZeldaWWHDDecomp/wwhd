/* HD scrolling message window: game views using the canonical parts framework. */
#pragma once
#include "d/hd_msg_window_views.h"
#include "d/hd_parts_key_icon.h"
struct ScrollPane_l {
  u8 mPad0[0x20];
  union {
    be<f32> mTranslationY;
    be<u32> mTranslationYBits;
  };
  u8 mPad24[0x18];
  be<f32> mWidth;
  union {
    be<f32> mHeight;
    be<u32> mHeightBits;
  };
  be<u8> mFlags;
  be<u8> mAlpha;
  u8 mPad46[2];
  be<u32> mMatrix[12];
  u8 mPad78[0x58];
  be<u8> mAlignment;
  u8 mPadD1[0x23];
  be<u8> mTextFlags;
};
struct ScrollMessage_l {
  be<u32> mMode;
  u8 mPad4[0x648];
  gptr<const char> mpNextText;
  u8 mPad650[0x46];
  be<u8> mScrollText;
  u8 mPad697[5];
  be<u8> mNewText;
};
struct ScrollVirtuals_l {
  be<u32> mSlots[46];
};
struct ScrollPlay_l {
  u8 mPad[0x5BC5];
  be<u8> mScrollMode;
  be<u8> mScrollAlpha;
  u8 mPad5BC7[0x69];
  be<u32> mScrollMessage;
  be<u32> mScrollCounter;
};
struct ScrollStringVirtual_l {
  u8 mPad[0x14];
  be<u32> mpEnsure;
};
struct ScrollString_l {
  gptr<const char> mpText;
  gptr<ScrollStringVirtual_l> mpVtable;
};
struct ScrollRootVirtual_l {
  u8 mPad[0x5C];
  be<u32> mpFindPane;
};
/* Dimensions used by scissoring; the shared hdPartsLyt_c leaves these library fields opaque. */
struct ScrollLayoutDimensions_l {
  u8 mPad[0x14];
  be<f32> mWidth;
  union { be<f32> mHeight; be<u32> mHeightBits; };
};
struct ScrollBounds_l {
  be<f32> mLeft, mBottom, mRight, mTop;
  u8 mPad[8];
};
struct ScrollDrawContext_l {
  u8 mPad[8];
  gptr<void> mpViewport;
};
struct ScrollResource_l {
  u8 mPad[8];
  ScrollString_l mLayout;
  u8 mPad10[4];
  ScrollString_l mPane;
};
struct MsgWindowScroll_l {
  gptr<void> mpVtable;
  gptr<ScrollVirtuals_l> mpOwner;
  u8 mPad8[0x10];
  union {
    HdMsgController mState;
    struct {
      gptr<hdStateOwner> mpStateOwner;
      u8 mStatePad1C[4];
      gptr<hdStateObject> mpCurrentState;
      u8 mStatePad24[8];
      gptr<void> mpStateVtable;
      gptr<void> mpStateOwnerVtable;
      be<u32> mStateLayoutMode;
      u8 mStateTail[0xC];
    };
  };
  gptr<hdPartsLayout_c> mpAnimation;
  be<u32> mButtonCount;
  gptr<gptr<hdPartsKeyIcon_c>> mpButtons;
  be<u32> mButtonsHeld;
  be<u32> mInput;
  be<u32> mButtonsCombined;
  be<u32> mMaterial;
  be<u32> mMessageID;
  gptr<ScrollPane_l> mpRoot;
  gptr<ScrollPane_l> mpScissor;
  gptr<ScrollPane_l> mpTextPanes[3];
  be<u32> mPaneIndices[3];
  gptr<ScrollPane_l> mpMaterialPanes[3];
  gptr<ScrollPane_l> mpButtonPanes[3];
  be<u32> mScrollMode;
  be<u32> mTimer;
  union {
    be<f32> mLinePositions[3];
    be<u32> mLinePositionBits[3];
  };
  be<u32> mTextAlignment;
  be<f32> mUpperLimit;
  be<f32> mLowerLimit;
  union {
    be<f32> mLineSpacing;
    be<u32> mLineSpacingBits;
  };
  be<f32> mTarget;
  be<f32> mPosition;
  gptr<const char> mpLines[20];
  be<u32> mLineIndex;
  be<u32> mLineCount;
  ScrollBounds_l mBounds;
  gptr<ScrollMessage_l> mpMessage;
  u32 createIcons(ScrollResource_l *);
  u32 createLayout(void *, u32, u32, u32, void *);
  u32 create(u32, u32, u32);
  u32 setMessage(u32, u32, u32);
  void updateScissor();
  void update();
  void draw(ScrollDrawContext_l *, u32);
  void drawTextLayer(u32);
  void drawButtonLayer(u32);
  void drawMaterialLayer(ScrollDrawContext_l *, u32);
  void findRootPanes();
  void findTextPanes();
  void findMaterialPanes();
  void findButtonPanes();
  void clearInputAndDraw();
  void readInput();
  void fadeLines();
  void beginOpening();
  void updateInput();
  void beginScrollUp();
  void beginScrollDown();
  void updateEmpty();
  void reset();
  void close();
  void forceClose();
  void hideButton(u32);
  void hideMiddleButton();
  void setLine(u32);
  void playAnimation(u32);
  void positionLines();
  u32 canScrollDown();
  u32 canScrollUp();
  void showButton(u32);
  void showMiddleButton();
  void showScrollButtons();
  u32 buttonInEnded(u32);
  u32 middleInEnded();
  void waitButtonsIn();
  void pressButton(u32);
  void pressMiddleButton();
  u32 buttonPressEnded(u32);
  u32 middlePressEnded();
  void waitButtonsPress();
  void startMiddleButton();
  void finishMiddleButton();
  void beginClosing();
  void waitMiddleButton();
  void startCloseAnimation();
  void stopAnimation(u32, f32 frame);
  u32 backgroundLuma();
  u32 openingAlphaLimit();
  f32 openingAlpha(s32, s32);
  f32 closingAlpha(s32, s32);
};
WWHD_OFFSET(ScrollPane_l, mTranslationY, 0x20);
WWHD_OFFSET(ScrollPane_l, mFlags, 0x44);
WWHD_OFFSET(MsgWindowScroll_l, mpAnimation, 0x44);
WWHD_OFFSET(MsgWindowScroll_l, mpTextPanes, 0x6C);
WWHD_OFFSET(MsgWindowScroll_l, mpButtonPanes, 0x90);
WWHD_OFFSET(MsgWindowScroll_l, mLinePositions, 0xA4);
WWHD_OFFSET(MsgWindowScroll_l, mLineIndex, 0x118);
WWHD_OFFSET(MsgWindowScroll_l, mpMessage, 0x138);
WWHD_SIZE(MsgWindowScroll_l, 0x13C);
WWHD_OFFSET(ScrollMessage_l, mScrollText, 0x696);

struct ScrollInput_l {
  be<u32> mButtonsDown;
  u8 mPad4[4];
  be<u32> mButtonsRepeat;
  u8 mPadC[0x100];
  be<u32> mButtonsHeld;
};
struct ScrollInputPointer_l {
  gptr<ScrollInput_l> mpInput;
  u8 mPad4[0x1CC];
  be<u32> mDisplay;
};
struct ScrollInputGlobal_l {
  gptr<ScrollInputPointer_l> mpReceiver;
};
struct ScrollControl_l {
  u8 mPad[0x14];
  ScrollMessage_l mMessage;
  u8 mPad6B1[0x94C - 0x14 - sizeof(ScrollMessage_l)];
  be<u32> mMessageMode;
  be<u32> mInputDisabled;
};
struct ScrollControlGlobal_l {
  gptr<ScrollControl_l> mpControl;
};


struct ScrollMatrix_l {
  union {
    be<u32> mWords[12];
    struct {
      u8 mPad0[12];
      be<f32> mX;
      u8 mPad10[12];
      be<f32> mY;
      u8 mPad20[16];
    };
  };
};

struct ScrollSave_l {
  u8 mPad[0x644];
  u8 mEvents;
};
struct ScrollSaveGlobal_l {
  gptr<ScrollSave_l> mpSave;
};

struct ScrollHeapManager_l {
  u8 mPad[0x10];
  gptr<void> mpHeap;
};
struct ScrollHeapGlobal_l {
  gptr<ScrollHeapManager_l> mpManager;
};


struct SScrollAnimationKindsView_l {
  be<u32> mValues[2];
};

struct SScrollMaterialVisibleView_l {
  be<u8> mValues[12];
};

struct SScrollRootNamesView_l {
  ScrollString_l mValues[2];
};

struct SScrollTextNamesView_l {
  ScrollString_l mValues[3];
};

struct SScrollMaterialNamesView_l {
  ScrollString_l mValues[3];
};

struct SScrollButtonNamesView_l {
  ScrollString_l mValues[3];
};

struct SScrollStateVtableView_l {
  be<u32> mValues[2];
};

struct SScrollStateOwnerVtableView_l {
  be<u32> mValues[2];
};

struct SScrollWindowVtableView_l {
  be<u32> mValues[2];
};

struct SScrollLayoutNamesView_l {
  ScrollString_l mValues[3];
};

struct SScrollAnimationNamesView_l {
  ScrollString_l mValues[1];
};

struct SScrollIconPaneNamesView_l {
  ScrollString_l mValues[2];
};

struct SScrollIconResourceNamesView_l {
  ScrollString_l mValues[3];
};

struct SScrollInitializerWordsView_l {
  be<u32> mValues[4];
};

struct SScrollDescriptorVtableView_l {
  be<u32> mValues[2];
};

struct SScrollDescriptorOwnerView_l {
  be<u32> mValues[2];
};

using ScrollStringDestructor_l = void (*)(ScrollString_l *, u32);

WWHD_OFFSET(ScrollPane_l, mWidth, 0x3C);
WWHD_OFFSET(ScrollPane_l, mHeight, 0x40);
WWHD_OFFSET(ScrollPane_l, mMatrix, 0x48);
WWHD_OFFSET(ScrollPane_l, mAlignment, 0xD0);
WWHD_OFFSET(ScrollPane_l, mTextFlags, 0xF4);
WWHD_OFFSET(MsgWindowScroll_l, mpOwner, 4);
WWHD_OFFSET(MsgWindowScroll_l, mpStateOwner, 0x18);
WWHD_OFFSET(MsgWindowScroll_l, mpCurrentState, 0x20);
WWHD_OFFSET(MsgWindowScroll_l, mpStateVtable, 0x2C);
WWHD_OFFSET(MsgWindowScroll_l, mpStateOwnerVtable, 0x30);
WWHD_OFFSET(MsgWindowScroll_l, mStateLayoutMode, 0x34);
WWHD_OFFSET(MsgWindowScroll_l, mButtonCount, 0x48);
WWHD_OFFSET(MsgWindowScroll_l, mpButtons, 0x4C);
WWHD_OFFSET(MsgWindowScroll_l, mButtonsHeld, 0x50);
WWHD_OFFSET(MsgWindowScroll_l, mInput, 0x54);
WWHD_OFFSET(MsgWindowScroll_l, mButtonsCombined, 0x58);
WWHD_OFFSET(MsgWindowScroll_l, mMaterial, 0x5C);
WWHD_OFFSET(MsgWindowScroll_l, mMessageID, 0x60);
WWHD_OFFSET(MsgWindowScroll_l, mpRoot, 0x64);
WWHD_OFFSET(MsgWindowScroll_l, mpScissor, 0x68);
WWHD_OFFSET(MsgWindowScroll_l, mPaneIndices, 0x78);
WWHD_OFFSET(MsgWindowScroll_l, mpMaterialPanes, 0x84);
WWHD_OFFSET(MsgWindowScroll_l, mScrollMode, 0x9C);
WWHD_OFFSET(MsgWindowScroll_l, mTimer, 0xA0);
WWHD_OFFSET(MsgWindowScroll_l, mTextAlignment, 0xB0);
WWHD_OFFSET(MsgWindowScroll_l, mUpperLimit, 0xB4);
WWHD_OFFSET(MsgWindowScroll_l, mLowerLimit, 0xB8);
WWHD_OFFSET(MsgWindowScroll_l, mLineSpacing, 0xBC);
WWHD_OFFSET(MsgWindowScroll_l, mTarget, 0xC0);
WWHD_OFFSET(MsgWindowScroll_l, mPosition, 0xC4);
WWHD_OFFSET(MsgWindowScroll_l, mpLines, 0xC8);
WWHD_OFFSET(MsgWindowScroll_l, mLineCount, 0x11C);
WWHD_OFFSET(MsgWindowScroll_l, mBounds, 0x120);
WWHD_OFFSET(ScrollMessage_l, mpNextText, 0x64C);
WWHD_OFFSET(ScrollMessage_l, mNewText, 0x69C);
WWHD_OFFSET(ScrollControl_l, mMessageMode, 0x94C);
WWHD_OFFSET(ScrollControl_l, mInputDisabled, 0x950);
WWHD_OFFSET(ScrollInput_l, mButtonsHeld, 0x10C);
WWHD_OFFSET(ScrollInputPointer_l, mDisplay, 0x1D0);

/* String literals use the registered copy belonging to the active guest
 * function. */
static inline const char *scrollGuestText(const char *text) {
  return gabi::at<const char>(gabi::guest_str(text));
}
