/* HD shortcut, swim timer and telescope screen views. Private reconstruction. */
#pragma once
#include "wwhd.h"
struct HdHudState { u8 mOpaque; };
struct HdHudStateController { u8 mOpaque[0x2C]; };
struct HdHudAnimation { u8 mPad[0xD4]; gptr<void> mPlayer; };
struct HdHudPane { u8 mPad[0x1C]; be<u32> mXBits; be<f32> mY; be<u32> mZBits; u8 mPad28[0x1C]; be<u8> mFlags; };
struct HdHudShortcut {
 u8 mPad[0x18]; HdHudStateController mState; u8 mPad44[0x48]; be<u32> mTextOffset;
 u8 mPad90[4]; gptr<HdHudState> mNextState; u8 mPad98[0x3B]; be<u8> mActive;
};
struct HdHudSwim {
 u8 mPad0[4]; gptr<gfn> mVTable; u8 mPad8[0x10]; HdHudStateController mState;
 gptr<HdHudAnimation> mAnimation; u8 mPad48[8]; gptr<HdHudPane> mPanes[6];
 be<f32> mPaneY[6]; be<f32> mDistance; be<f32> mRatio; be<f32> mBlinkRange;
 be<u32> mBlinkFrame; be<u32> mBlinkPeriod; be<u32> mSoundCooldown; be<u32> mFlashDelay;
 u8 mPad9C[4]; be<u8> mFlashing[6]; be<u8> mPadA6; be<u8> mBlinking;
};
struct HdHudTelescope { u8 mPad[0x18]; HdHudStateController mState; u8 mPad44[0x18]; be<u32> mFrame; };
struct HdHudActor { u8 mPad[0x3B8]; be<u32> mStatus; };
struct HdHudPlay {
 u8 mPad[0x5B2C]; gptr<HdHudActor> mActor; u8 mPad5B30[0x1C]; be<s32> mSwimTime;
 be<s32> mSwimTimeMax; u8 mPad5B54[0x5C]; be<u8> mSwimTimerRunning; be<u8> mPad5BB1;
 be<u8> mTelescopeMode; be<u8> mTelescopeNextMode;
};
struct HdHudModes { be<u32> mModes[16]; };
enum HdHudScriptOffset { HD_HUD_COMMAND_SIZE = 0x28 };
enum HdHudPaneFlags { HD_HUD_PANE_TRANSFORM_DIRTY = 0x10 };
enum HdHudTelescopeFlags { HD_HUD_TELESCOPE_ACTOR_STATUS = 0x80000 };
WWHD_OFFSET(HdHudShortcut,mTextOffset,0x8C);
WWHD_OFFSET(HdHudShortcut,mActive,0xD3);
WWHD_OFFSET(HdHudSwim,mPanes,0x50);
WWHD_OFFSET(HdHudSwim,mRatio,0x84);
WWHD_OFFSET(HdHudSwim,mBlinking,0xA7);
WWHD_OFFSET(HdHudPlay,mSwimTime,0x5B4C);
WWHD_OFFSET(HdHudPlay,mSwimTimerRunning,0x5BB0);
WWHD_OFFSET(HdHudTelescope,mFrame,0x5C);
HdHudPlay* hdHudGetPlay();
void hdHudChangeState(HdHudStateController*,HdHudState*);
