/* HD scalable-font manager (sead singleton, 0x15B0 bytes, instance 101F4BD8). It owns the scalable font (+0x3C)
 * and its glyph texture cache (+0x64). Both are nw::font engine objects (NW-FONT range, replaced by the port's
 * font module); this class only calls their entry points. hdFontMgr_c (hd_font_mgr) registers the scalable font
 * as font #5 ("CafeStd"). A state machine (Wait / InProgress / Finish) drives the per-frame cache updates.
 * Roles and init-argument fields follow the font interface map (font-sort/interface.md §1, §5).
 * Reconstructed from the game's instructions. */
#pragma once
#include "d/hd_state.h"
enum hdSFontProgress { HD_SFONT_IDLE, HD_SFONT_WORKING, HD_SFONT_DONE };
enum hdSFontMessage { HD_SFONT_MSG_UPDATE = 0x75706474 };  // 'updt'
struct ScalableFontStorage { u8 mOpaque[0x28]; };    // the scalable font (engine object, opaque)
struct GlyphCacheStorage { u8 mOpaque[0x1524]; };    // its glyph texture cache (engine object, opaque)
/* font initialize argument (defaults from 028B289C) */
struct ScalableFontInitArg {
 gptr<GlyphCacheStorage> mGlyphCache;
 be<u32> mFontSize;              // the game passes 25; width/height/ascent derive from it
 be<u16> mFace;
 be<u8> mSpaceReplacement;       // set: replacement character U+3000, glyph mode 0
 be<u8> mModeSelect;             // otherwise glyph mode 1 when set, else 2 (the game sets it)
};
/* glyph cache initialize argument (defaults from 028B4230) */
struct GlyphCacheInitArg {
 be<u32> mSheetWidth;            // texture sheet: the game passes 1024 x 2048
 be<u32> mSheetHeight;
 gptr<HdStateVTableObject> mAllocator;
 gptr<be<u32>> mFontData;        // OS shared font (OSGetSharedData type 2): pointer
 gptr<be<u32>> mFontDataSize;    // and size
 be<u32> mReserved14;
 be<u32> mReserved18;
 gptr<be<u32>> mZeroWord;        // the game passes a zero word
 be<u32> mFontCount;             // one font
 be<u32> mReserved24;            // default 0x80000
 be<u32> mReserved28;            // default 0xC800
 be<u32> mReserved2C;            // default 0x400; the game passes 0x1400 (role not identified)
 be<u8> mReserved30;
 u8 mPad31[3];
};
/* the frame of initScalableFont() */
struct hdSFontInitFrame {
 ScalableFontInitArg mFontArg; be<u32> mFontDataSize; be<u32> mZeroWord; GlyphCacheInitArg mCacheArg;
};
struct hdScalableFontMgr_c {
 gptr<gfn> mVTable; gptr<gfn> mObjectVTable; gptr<hdScalableFontMgr_c> mSelf; be<u32> mReserved;
 HdStateMachine mState; gptr<gfn> mTaskVTable; HdStateDisposer mDisposer;
 gptr<void> mHeap; ScalableFontStorage mFont; GlyphCacheStorage mGlyphCache;
 gptr<HdStateVTableObject> mAllocator; be<u32> mFontData; HdStateDelegate mUpdateDelegate;
 gptr<HdStateObject> mObject; be<u32> mProgress; be<u8> mFlag; u8 mPad15A9[3]; be<u32> mBusyCount;

 void clearFlag();
 u32 isIdle();
 void updateGlyphCache();
 void setFlag();
 void initScalableFont();
 void init(void* heap);
 void execute();
 u32 requestCacheUpdateA(void* text, u32 a, u32 b, u32 c, u32 d);
 u32 requestCacheUpdateB();
 void onMessage(void* object, u32 message);
};
struct hdSFontSingleton { gptr<hdScalableFontMgr_c> mInstance; };
WWHD_SIZE(hdScalableFontMgr_c,0x15B0);
WWHD_OFFSET(hdScalableFontMgr_c,mState,0x10);
WWHD_OFFSET(hdScalableFontMgr_c,mDisposer,0x28);
WWHD_OFFSET(hdScalableFontMgr_c,mFont,0x3C);
WWHD_OFFSET(hdScalableFontMgr_c,mGlyphCache,0x64);
WWHD_OFFSET(hdScalableFontMgr_c,mAllocator,0x1588);
WWHD_OFFSET(hdScalableFontMgr_c,mFontData,0x158C);
WWHD_OFFSET(hdScalableFontMgr_c,mUpdateDelegate,0x1590);
WWHD_OFFSET(hdScalableFontMgr_c,mProgress,0x15A4);
WWHD_OFFSET(hdScalableFontMgr_c,mBusyCount,0x15AC);
WWHD_SIZE(ScalableFontInitArg,0xC);
WWHD_SIZE(GlyphCacheInitArg,0x34);
WWHD_SIZE(hdSFontInitFrame,0x48);
