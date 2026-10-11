/** HD: TV/GamePad controller routing and touch-pointer layouts; no GameCube counterpart. */
#pragma once
#include "wwhd.h"
struct hdControllerRttiTable { u8 _opaque[0xC]; gfn mIsA; };
struct hdInputGame { u8 _opaque00[0x12C0]; be<u8> mOptions; };
struct hdInputState {
    /* 0x000 */ be<u32> mHold; be<u32> mTrigger; be<u32> mRelease; be<u32> mPointerFlags;
    /* 0x010 */ gptr<hdControllerRttiTable> mVtable;
    u8 _opaque14[0xA1];
    /* 0x0B5 */ be<u8> mPlatformType;
    u8 _opaqueB6[0x56];
    /* 0x10C */ be<u32> mRepeat;
    /* 0x110 */ be<f32> mPointerX; be<f32> mPointerY;
};
struct hdController : hdInputState {
    u8 _opaque118[0x40];
    /* 0x158 */ gptr<hdControllerRttiTable> mControllerVtable;
    u8 _opaque15C[0x978];
    /* 0xAD4 */ be<s32> mConnectedCount;
    // HD: controller type lies in the opaque platform status block.
    u8 platformType() const { return mPlatformType; }
};
struct hdDrcStatus { u8 _opaque00[0x5C]; be<u8> mStatus; u8 _opaque5D[0xEA3]; be<s32> mConnectedCount; u8 _opaqueF04[4]; };
struct hdDrcController { u8 _opaque00[0x1118]; hdDrcStatus mChannels[4]; };
struct hdPointerController : hdInputState {
    u32 controllerType();
    void pointerUpdate(u32 a, u32 b);
    u8 _opaque118[0x24];
    /* 0x13C */ gptr<hdControllerRttiTable> mVtableSecond;
    u8 _opaque140[4];
    /* 0x144 */ be<u8> mTouchActive;
    u8 _opaque145[0x2B];
    /* 0x170 */ gptr<hdControllerRttiTable> mVtableThird;
    u8 _opaque174[0x20];
    /* 0x194 */ be<s32> mPointerPixelX; be<s32> mPointerPixelY;
    /* 0x19C */ be<f32> mCalibratedX; be<f32> mCalibratedY;
    /* 0x1A4 */ be<f32> mOffsetX; be<f32> mOffsetY; be<f32> mScaleX; be<f32> mScaleY;
    /* 0x1B4 */ be<u32> mControllerType;
};
struct hdInputMatrix { be<u32> mBits[9]; };
struct hdInputVector { be<u32> mBits[3]; };
struct hdInputPosition { be<f32> mX; be<f32> mY; };
struct hdInputOrientation {
    /* 0x00 */ hdInputVector mBasis[3];
    /* 0x24 */ hdInputMatrix mCurrent;
    /* 0x48 */ gptr<hdInputMatrix> mSource;
    /* 0x4C */ hdInputMatrix mCalibration;
    /* 0x70 */ hdInputMatrix mInverse;
};
struct hdInputCursor { u8 _opaque[4]; be<u32> mState; };
struct hdInputManagerTable { u8 _opaque[0xC]; gfn mDestroy; };
struct hdInputDisposer { u8 _opaque[0xC]; gptr<hdInputManagerTable> mVtable; };
struct hdInputManager {
    u32 isTvMode();
    void calc();
    void init(u32 unused, void* heap);
    u32 hasPad();
    u32 hasDrc();
    u32 setMode(u32 mode);
    u32 isGamePadMode();
    u32 isAlternateGamePadMode();
    u32 isCombinedMode();
    u32 isFree(u32 mask);
    u32 cursorState();
    void calibrate();
    void resetPointer(u32 arg);
    void pointerParams(u32 a, u32 b);
    void pointerDefault();
    void pointerParamsMasked(u32 a, u32 b, u32 c);
    void pointerSlow(u32 a);
    /* 0x000 */ gptr<hdPointerController> mInput;
    /* 0x004 */ gptr<hdInputManagerTable> mVtable;
    /* 0x008 */ hdInputDisposer mDisposer;
    /* 0x018 */ hdPointerController mPointer;
    /* 0x1D0 */ be<u32> mMode;
    /* 0x1D4 */ gptr<hdInputCursor> mCursor;
    /* 0x1D8 */ hdInputOrientation mOrientation;
    /* 0x26C */ be<u8> mPointerEnabled; be<u8> mRecalibrationTimer; u8 _pad26E[2];
    // HD: embedded addresses must retain unsigned guest wrapping, including a null allocation.
    hdPointerController* pointer() { return gabi::at<hdPointerController>(gabi::ea(this) + offsetof(hdInputManager,mPointer)); }
    hdInputDisposer* disposer() { return gabi::at<hdInputDisposer>(gabi::ea(this) + offsetof(hdInputManager,mDisposer)); }
};
struct hdInputControllerTypeMap { be<u32> mTypes[256]; };
struct hdInputStaticBlock { be<u32> mWords[4]; };
struct hdInputStaticRange { be<f32> mMin; be<f32> mMax; be<u8> mFirst; be<u8> mSecond; u8 _pad[2]; };
enum hdControllerMode { INPUT_MODE_TV=0, INPUT_MODE_GAMEPAD=1, INPUT_MODE_GAMEPAD_ALT=2, INPUT_MODE_BOTH=3, INPUT_MODE_NONE=4 };
enum hdControllerId { INPUT_CONTROLLER_DRC=7, INPUT_CONTROLLER_PAD=8, INPUT_CONTROLLER_COMBINED=9 };
enum hdPointerFlag { INPUT_POINTER_VALID=1, INPUT_POINTER_FLAG_2=2, INPUT_POINTER_FLAG_4=4, INPUT_BUTTON_TOUCH=0x8000 };
WWHD_SIZE(hdInputState,0x118);
WWHD_OFFSET(hdController,mControllerVtable,0x158); WWHD_OFFSET(hdController,mConnectedCount,0xAD4);
WWHD_SIZE(hdDrcStatus,0xF08); WWHD_OFFSET(hdDrcStatus,mConnectedCount,0xF00);
WWHD_OFFSET(hdPointerController,mVtableSecond,0x13C); WWHD_OFFSET(hdPointerController,mVtableThird,0x170);
WWHD_OFFSET(hdPointerController,mCalibratedX,0x19C); WWHD_OFFSET(hdPointerController,mControllerType,0x1B4); WWHD_SIZE(hdPointerController,0x1B8);
WWHD_SIZE(hdInputMatrix,0x24); WWHD_SIZE(hdInputVector,0xC); WWHD_SIZE(hdInputPosition,8); WWHD_SIZE(hdInputOrientation,0x94);
WWHD_OFFSET(hdInputManager,mPointer,0x18); WWHD_OFFSET(hdInputManager,mMode,0x1D0); WWHD_OFFSET(hdInputManager,mOrientation,0x1D8); WWHD_OFFSET(hdInputManager,mPointerEnabled,0x26C); WWHD_SIZE(hdInputManager,0x270);
hdInputManager* hdInputAllocateSingleton(u32 size, void* heap, u32 alignment);
void hdInputConstructBase(hdInputManager* manager, hdInputManager* owner);
void hdInputSetPointerBounds(hdPointerController* pointer, f32 x, f32 y, f32 width, f32 height);
void hdInputConstructDisposer(hdInputDisposer* disposer, void* heap, u32 flags);
hdController* hdInputControllerByOrder(void* manager, u32 id, u32 index);
hdController* hdInputControllerById(void* manager, u32 id);
void hdInputAttachController(hdController* manager, hdController* controller);
void hdInputSetPointerController(hdPointerController* pointer, hdController* controller, u32 enabled);
void hdInputSetPointerParameters(hdPointerController* pointer, u32 mask, u32 delay, u32 interval);
hdInputCursor* hdInputAllocateCursor(u32 size, void* heap, u32 alignment);
hdInputCursor* hdInputConstructCursor(hdInputCursor* cursor);
void hdInputPlatformUpdate(u32 channel);
void hdInputResetPointer(hdPointerController* pointer, u32 enabled);
void hdInputClearControllers(hdController* controller);
void hdInputUpdateOrientation(hdInputOrientation* orientation);
void hdInputUpdateCursor(hdInputCursor* cursor, hdInputPosition* position);
void hdInputSetDrcOutput(u32 enabled);
void hdInputCopyMatrix(hdInputMatrix* out, hdInputMatrix* source);
void hdInputInvertMatrix(hdInputMatrix* out, hdInputMatrix* source);
void* hdInputGetOptions(void* options);
u32 hdInputGyroEnabled(void* options);
void hdInputMultiplyMatrix(hdInputMatrix* out, hdInputMatrix* left, hdInputMatrix* right);
void hdInputClearPointer(hdPointerController* pointer);
void hdInputDestroyDisposer(hdInputDisposer* disposer, u32 flags);
void hdInputConstructRegister(void* object);
void hdInputConstructFirstFlag(be<u8>* flag);
void hdInputConstructSecondFlag(be<u8>* flag);
void hdInputDestroyPointerBase(hdPointerController* pointer, u32 flags);
void hdInputDestroyManagerBase(hdInputManager* manager, u32 flags);
void hdInputPointerSecondBaseOperation(hdPointerController* pointer, u32 value);
u32 hdInputPlatformControllerType(u32 channel);
void hdInputConstructPointerBase(hdPointerController* pointer);
void hdInputUpdatePointerBase(hdPointerController* pointer, u32 first, u32 second);

// HD: secondary-base thunk subtraction wraps in the 32-bit guest address space.
inline hdPointerController* hdInputPointerFromSecondBase(void* base) { return gabi::at<hdPointerController>(gabi::ea(base) - 0x130); }

// HD: this read is also made through a null controller after an unsuccessful RTTI cast.
inline s32 hdInputControllerConnectedCount(hdController* controller) { return (s32)*gabi::at<be<s32>>(gabi::ea(controller) + offsetof(hdController,mConnectedCount)); }

WWHD_SIZE(hdControllerRttiTable,0x10); WWHD_SIZE(hdController,0xAD8);
WWHD_OFFSET(hdInputState,mPlatformType,0xB5); WWHD_OFFSET(hdInputState,mRepeat,0x10C); WWHD_OFFSET(hdInputState,mPointerX,0x110);
WWHD_OFFSET(hdDrcController,mChannels,0x1118); WWHD_SIZE(hdDrcController,0x4D38);
WWHD_OFFSET(hdInputOrientation,mCurrent,0x24); WWHD_OFFSET(hdInputOrientation,mSource,0x48); WWHD_OFFSET(hdInputOrientation,mCalibration,0x4C); WWHD_OFFSET(hdInputOrientation,mInverse,0x70);
WWHD_SIZE(hdInputCursor,8); WWHD_SIZE(hdInputManagerTable,0x10); WWHD_SIZE(hdInputDisposer,0x10); WWHD_SIZE(hdInputStaticBlock,0x10); WWHD_SIZE(hdInputStaticRange,0xC); WWHD_SIZE(hdInputControllerTypeMap,0x400); WWHD_SIZE(hdInputGame,0x12C1);
