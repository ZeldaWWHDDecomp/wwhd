/** Graphics state: tww member names at WWHD static-data addresses.
 * HD: native display objects replace the GameCube renderer. Partial object views only. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"

struct mDoGph_HeapHD {
    u8 padding[0x10];
    gptr<const char> mName;
    // HD: string literals stored as pointers must use their mapped guest address.
    void setName(const char *name) {
        mName = gabi::at<const char>(gabi::guest_addr(name));
    }
};
union mDoGph_Color {
    be<u32> word;
    GXColor rgba;
};
WWHD_SIZE(mDoGph_Color, 4);
struct mDoGph_StateHD {
    gptr<mDoGph_HeapHD> mHeap[2];
    be<f32> mFadeRate;
    be<f32> mFadeSpeed;
    mDoGph_Color mBackColor;
    mDoGph_Color mFadeColor;
    be<s16> mMonotoneRate;
    be<s16> mMonotoneRateSpeed;
    be<u8> mCurrentHeap;
    be<u8> mBlureFlag;
    be<u8> mBlureRate;
    be<u8> mFade;
    be<u8> mAutoForcus;
    be<u8> mMonotone;
};
WWHD_OFFSET(mDoGph_StateHD, mFadeRate, 8);
WWHD_OFFSET(mDoGph_StateHD, mFadeColor, 0x14);
WWHD_OFFSET(mDoGph_StateHD, mMonotoneRate, 0x18);
WWHD_OFFSET(mDoGph_StateHD, mFade, 0x1f);
struct mDoGph_FaderHD {
    u8 padding[8];
    mDoGph_Color mColor;
};
struct mDoGph_DisplayHD {
    u8 padding[0x440];
    gptr<mDoGph_FaderHD> mFader;
};
struct mDoGph_GameInfoHD {
    u8 padding[0x5d30];
    u8 mDrawList[0x60ec - 0x5d30];
    u8 m2DList[0x62f1 - 0x60ec];
    be<u8> mFadeBrightness;
};
struct mDoGph_HeaderStaticsHD {
    be<f32> mMinAngle;
    be<f32> mMaxAngle;
    u8 padding[2];
    be<u8> mAngleTable;
    be<u8> mMathTable;
    be<u32> mZero[4];
};
WWHD_OFFSET(mDoGph_HeaderStaticsHD, mAngleTable, 0xa);
WWHD_OFFSET(mDoGph_HeaderStaticsHD, mZero, 0xc);
struct mDoGph_gInf_c {
    static void createHeap();
    static void create();
    static void onBlure(const Mtx34 *);
    static void onBlure();
    static void fadeOut(f32, const mDoGph_Color &);
    static void fadeOut(f32);
    static void calcFade();
    static void onMonotone();
    static void offMonotone();
    static void calcMonotone();
};
// HD: these free entry adapters retain the no-this ABI of the GC static methods.
// Autoglue currently treats every qualified definition as an instance method.
void mDoGph_createHeap();
void mDoGph_create();
void mDoGph_onBlureMatrix(const Mtx34 *);
void mDoGph_fadeOut(f32, const mDoGph_Color *);
void mDoGph_offMonotone();
void *mDoGph_getParentHeap();
mDoGph_HeapHD *mDoGph_createHeapHD(u32, void *, u32);
void mDoGph_initializeBuffers(u32);
mDoGph_HeapHD *mDoGph_createGameHeap(u32, u32, u32);
void mDoGph_adjustGameHeap(mDoGph_HeapHD *);
void mDoGph_restoreCurrentHeap();
mDoGph_GameInfoHD *mDoGph_getGameInfo();
void mDoGph_drawListBefore(void *);
void mDoGph_drawListAfter(void *);
void mDoGph_drawListInitialize(void *);
u32 mDoGph_displayControlHD(mDoGph_DisplayHD *, u32, u32);
void mDoGph_displayControlOneHD(mDoGph_DisplayHD *, u32);
void mDoGph_applyFade(mDoGph_DisplayHD *);
void mDoGph_initializeAngle(be<u8> *);
void mDoGph_initializeMath(be<u8> *);
s32 mDoGph_chaseMonotone(be<s16> *, s32, s32);
