/* HD Picto Box capture screen and its animation states. */
#pragma once
#include "d/hd_screen_pictograph.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_input_eventmgr.h"
#include "hd/hd_jpeg_mgr.h"
struct hdPictographCamera : hdParts_c {
    gptr<hdTextPane_c> mNumberPanes[2];
    be<u8> mShutterVisible;
    u8 mReserved59[3];
    be<f32> mScale;
    HdJpegTextureMap mCapturedTexture;
    gptr<hdPartsPaneSRT_c> mpShutter;
    be<u8> mSelfieMode;
    be<u8> mSelfieDelay;
    u8 mReserved10E[2];
    static hdPictographCamera* construct(hdPictographCamera*);
    static void destroy(hdPictographCamera*,u32);
    u32 createNamedChild(hdPartsDesc_c*);
    u32 initialize(u32,u32,void*);
    void finalizeResources();
    void executeState();
    u32 isAnimationFinished();
    void requestCoverState();
    u32 loadLayout(hdUiSafeString*,u32,u32,u32,void*);
    void resetCapture();
    void bindNumberPanes();
    void initializeInAll();
    void updateInAll();
    void updateScale();
    void initializeCount();
    void setCount(s32);
    void setScaleText(f32);
    void initializeSelfieToggle();
    void updateSelfieToggle();
    void initializeConfirmMessage();
    void updateConfirmMessage();
    void updateMessage();
    void finishInquiry();
    void finishMessage();
    void initializeOutAll();
    void updateOutAll();
    void initializeInquiry();
    void bindCapturedTexture();
    void updateShutter();
    void initializeMessage();
    void calcLayout();
    void openSelfieCover();
    void updateOpenSelfieCover();
    void closeSelfieCover();
    void updateCloseSelfieCover();
    void reverseCloseSelfieCover();
    void updateReverseCloseSelfieCover();
    void reverseOpenSelfieCover();
    void updateReverseOpenSelfieCover();
    void enableSelfieMode();
    void disableSelfieMode();
    void resetAnimation();
    void enterBlank();
    void updateMessageButtons();
};
WWHD_OFFSET(hdPictographCamera,mShutterVisible,0x58);
WWHD_OFFSET(hdPictographCamera,mScale,0x5C);
WWHD_OFFSET(hdPictographCamera,mCapturedTexture,0x60);
WWHD_OFFSET(hdPictographCamera,mpShutter,0x108);
WWHD_OFFSET(hdPictographCamera,mSelfieMode,0x10C);
WWHD_SIZE(hdPictographCamera,0x110);

struct hdPictoPlayerView { u8 mReserved0[0x3B8]; be<u32> mCaptureFlags; };
WWHD_OFFSET(hdPictoPlayerView,mCaptureFlags,0x3B8);
struct hdPictoPlayView {
    u8 mReserved0[0x5B04];
    be<f32> mPictureZoom;
    u8 mReserved5B08[0x5B2C-0x5B08];
    gptr<hdPictoPlayerView> mpPlayer;
    u8 mReserved5B30[4];
    gptr<void> mpPictureController;
    u8 mReserved5B38[0x5B66-0x5B38];
    be<s16> mPendingPictureCount;
    u8 mReserved5B68[0x5BB2-0x5B68];
    be<u8> mMessageInputStatus;
    u8 mReserved5BB3[5];
    be<u8> mPrimaryButtonIcon,mSecondaryButtonIcon;
    u8 mReserved5BBA[0x5BCF-0x5BBA];
    be<u8> mMessageVariant;
    u8 mReserved5BD0[0x5BE6-0x5BD0];
    be<u8> mSelectedPhotoNumber;
    be<u8> mSelectedPhotoUsed,mPictureListMode;
    u8 mReserved5BE9;
    be<u8> mSelectedBestShot;
    be<u8> mSelectedPicture;
    be<u8> mItemOpen;
    u8 mReserved5BED[0x5C21-0x5BED];
    be<u8> mPictureInputMode;
    u8 mReserved5C22[6];
    be<u8> mPictureButtonLeft,mPictureButtonRight,mPictureButtonAux;
    be<u8> mSavedPictureButtonLeft,mSavedPictureButtonRight,mSavedPictureButtonAux;
    u8 mReserved5C2E;
    be<u8> mAuxButtonIcon;
    u8 mReserved5C30[0x5CDC-0x5C30];
    be<u32> mPlayerFlags;
};
WWHD_OFFSET(hdPictoPlayView,mPictureZoom,0x5B04);
WWHD_OFFSET(hdPictoPlayView,mpPlayer,0x5B2C);
WWHD_OFFSET(hdPictoPlayView,mPendingPictureCount,0x5B66);
WWHD_OFFSET(hdPictoPlayView,mPrimaryButtonIcon,0x5BB8);
WWHD_OFFSET(hdPictoPlayView,mSecondaryButtonIcon,0x5BB9);
WWHD_OFFSET(hdPictoPlayView,mMessageVariant,0x5BCF);
WWHD_OFFSET(hdPictoPlayView,mItemOpen,0x5BEC);
WWHD_OFFSET(hdPictoPlayView,mPictureInputMode,0x5C21);
WWHD_OFFSET(hdPictoPlayView,mAuxButtonIcon,0x5C2F);
WWHD_OFFSET(hdPictoPlayView,mPlayerFlags,0x5CDC);
struct hdPictoCoreView {
    u8 mReserved0[0x9A1]; be<u8> mCaptureMessageActive;
};
WWHD_OFFSET(hdPictoCoreView,mCaptureMessageActive,0x9A1);

struct hdPictoSaveView { u8 mReserved0[0x12C0]; be<u32> mPictureHolder; };
WWHD_OFFSET(hdPictoSaveView,mPictureHolder,0x12C0);
struct hdPictoPictureArchiveView { u8 mReserved0[0x3C030C]; be<u8> mCount; };
WWHD_OFFSET(hdPictoPictureArchiveView,mCount,0x3C030C);

struct hdPictoTextPaneTable { u8 mReserved0[0xC4]; gfn mSetText; };
WWHD_OFFSET(hdPictoTextPaneTable,mSetText,0xC4);
struct hdPictoNumberText { PaneString_l mString; be<u16> mBuffer[3]; };
WWHD_OFFSET(hdPictoNumberText,mBuffer,0xC);

struct hdPictoInputSystem { u8 mReserved0[0x18]; cking::ui::input::Event mEvent; };
struct hdPictoInputOptions { u8 mReserved0[0x14]; be<u32> mInputBlocked; };
WWHD_OFFSET(hdPictoInputSystem,mEvent,0x18);
WWHD_OFFSET(hdPictoInputOptions,mInputBlocked,0x14);
WWHD_OFFSET(hdPictoPlayView,mMessageInputStatus,0x5BB2);

struct hdPictoCaptureSurface {
    be<u32> mReserved;
    HdJpegSurface mSurface;
    be<u8> mMinimumWidth,mMinimumHeight;
};
WWHD_OFFSET(hdPictoCaptureSurface,mSurface,4);
WWHD_OFFSET(hdPictoCaptureSurface,mMinimumWidth,0x78);

WWHD_OFFSET(hdPictoPlayView,mSelectedPhotoUsed,0x5BE7);
WWHD_OFFSET(hdPictoPlayView,mPictureListMode,0x5BE8);
WWHD_OFFSET(hdPictoPlayView,mSelectedPicture,0x5BEB);

WWHD_OFFSET(hdPictoPlayView,mSelectedPhotoNumber,0x5BE6);
WWHD_OFFSET(hdPictoPlayView,mSelectedBestShot,0x5BEA);

WWHD_OFFSET(hdPictoPlayView,mpPictureController,0x5B34);
WWHD_OFFSET(hdPictoPlayView,mPictureButtonLeft,0x5C28);
WWHD_OFFSET(hdPictoPlayView,mSavedPictureButtonLeft,0x5C2B);
