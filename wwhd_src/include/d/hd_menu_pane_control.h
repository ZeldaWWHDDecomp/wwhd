/* HD menu pane positioning and visibility controller; binary-observed layout. */
#pragma once
#include "bindings.h"

/* Narrow library pane view: only data accessed by this game controller. */
struct hdMenuPane_l {
    u8 _header[0x1C];
    be<f32> mTranslationX;
    be<f32> mTranslationY;
    be<f32> mTranslationZ;
    be<f32> mRotationX;
    be<f32> mRotationY;
    be<f32> mRotationZ;
    be<f32> mScaleX;
    be<f32> mScaleY;
    u8 _transformTail[8];
    be<u8> mFlags;
    be<u8> mAlpha;
};
WWHD_OFFSET(hdMenuPane_l, mTranslationX, 0x1C);
WWHD_OFFSET(hdMenuPane_l, mRotationX, 0x28);
WWHD_OFFSET(hdMenuPane_l, mScaleX, 0x34);
WWHD_OFFSET(hdMenuPane_l, mFlags, 0x44);
WWHD_OFFSET(hdMenuPane_l, mAlpha, 0x45);
struct hdMenuPaneVector {
    be<f32> x;
    be<f32> y;
};
WWHD_SIZE(hdMenuPaneVector, 8);

struct hdMenuPaneControl {
    gptr<hdMenuPane_l> mpPane;
    gptr<hdMenuPane_l> mpRotationPane;
    hdMenuPaneVector mPosition;
    hdMenuPaneVector mPositionOffset;
    hdMenuPaneVector mBasePosition;
    hdMenuPaneVector mBaseOffset;
    be<f32> mScale;
    be<f32> mScaleFactor;
    be<f32> mRotation;
    be<f32> mRotationOffset;
    be<u8> mAlpha;
    be<u8> mDuration;
    be<u8> mFlags;
    be<u8> mVisible;
    be<u8> mLastVisible;
    be<u8> mRotationPaneVisible;
    be<u8> mFading;
    u8 _padding;

    static hdMenuPaneControl* construct(hdMenuPaneControl* self);
    void destroy(u32 flags);
    void reset();
    void update();
};
WWHD_SIZE(hdMenuPaneControl, 0x40);
WWHD_OFFSET(hdMenuPaneControl, mpPane, 0);
WWHD_OFFSET(hdMenuPaneControl, mpRotationPane, 4);
WWHD_OFFSET(hdMenuPaneControl, mPosition, 8);
WWHD_OFFSET(hdMenuPaneControl, mPositionOffset, 0x10);
WWHD_OFFSET(hdMenuPaneControl, mBasePosition, 0x18);
WWHD_OFFSET(hdMenuPaneControl, mBaseOffset, 0x20);
WWHD_OFFSET(hdMenuPaneControl, mScale, 0x28);
WWHD_OFFSET(hdMenuPaneControl, mRotation, 0x30);
WWHD_OFFSET(hdMenuPaneControl, mAlpha, 0x38);
WWHD_OFFSET(hdMenuPaneControl, mVisible, 0x3B);
WWHD_OFFSET(hdMenuPaneControl, mFading, 0x3E);
