/** HD Miiverse comment button. Private binary-derived source; no GameCube counterpart. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "d/hd_parts_button.h"
#include "d/hd_screen_menu_miiverse.h"

// Float arithmetic uses the canonical raw translation words without copying the pane layout.
inline be<f32>& miiversePaneTranslationX(hdPartsPaneSRT_c* pane) {
    return reinterpret_cast<be<f32>&>(pane->mTranslationXBits);
}
inline be<f32>& miiversePaneTranslationY(hdPartsPaneSRT_c* pane) {
    return reinterpret_cast<be<f32>&>(pane->mTranslationYBits);
}
inline be<f32>& miiversePaneTranslationZ(hdPartsPaneSRT_c* pane) {
    return reinterpret_cast<be<f32>&>(pane->mTranslationZBits);
}
WWHD_OFFSET(hdPartsPaneSRT_c, mTranslationYBits, 0x20);
WWHD_OFFSET(hdPartsPaneSRT_c, mTranslationZBits, 0x24);
// Game-specific content/photo tables extend the canonical parts base.
struct hdMiiverseCommentContentVtable_l {
    hdPartsVtbl_c mBase;
    u8 mContentMethods[0x23C - sizeof(hdPartsVtbl_c)];
    gfn mCancelCursor;
    u8 mEnabledGap[4];
    gfn mSetEnabled;
};
WWHD_OFFSET(hdMiiverseCommentContentVtable_l, mCancelCursor, 0x23C);
WWHD_OFFSET(hdMiiverseCommentContentVtable_l, mSetEnabled, 0x244);
struct hdMiiverseCommentPartRequest_l;
struct hdMenuMiiverseComment_c : hdParts_c {
    be<u8> mEnabled;
    u8 mPositionPadding[3];
    be<f32> mPositionY;
    be<f32> mTargetY;
    be<f32> mScrollTarget;
    be<f32> mSavedY;
    be<u32> mArchiveIndex;
    be<u8> mContentStorage[0x88 - 0x68];
    be<u8> mFilled;
    u8 mPresentationPadding[3];
    be<f32> mPictureBaseX;
    be<f32> mPictureBaseY;
    be<f32> mPictureBaseZ;
    be<f32> mContentBaseX;
    be<f32> mContentBaseY;
    be<f32> mContentBaseZ;
    be<f32> mPicturePositionY;
    be<f32> mPictureSlideStep;
    be<u8> mSelectionPending;
    be<u8> mOpenPending;
    be<u8> mCommentRequested;
    be<u8> mPictureRequested;
    be<u8> mInteractionFlags[8];
    be<u32> mTimerHigh;
    be<u32> mTimerLow;
    be<u8> mPendingFlags[4];
    be<u32> mPendingState;
    gptr<hdPartsPaneSRT_c> mPicturePanes[4];

    f32 layoutPositionY() const;
    void setLayoutPositionY(f32);
    void restoreLayoutPositionY();
    static hdMenuMiiverseComment_c* construct(hdMenuMiiverseComment_c*);
    static void destroy(hdMenuMiiverseComment_c*, u32);
    void finalizeParts();
    void findPicturePanes();
    u32 loadLayout(void*, u32, u32, u32, void*);
    void translateLayoutY(f32);
    void setPicturePresentation(u32, f32);
    void enterIn();
    void enterOut();
    void finishOut();
    void executeIn();
    void enterInNewPost();
    void executeInNewPost();
    void enterSlideDown();
    void executeSlideDown();
    void resetToWait();
    void executeOut();
    void executePhoto();
    void beginOut();
    void beginIn();
    void beginInNewPost(f32);
    void connectAllVerticalControls(hdButtonPart_c*, hdButtonPart_c*);
    void focusContent();
    void openContent();
    void pictureGlobalTranslation(cXyz*) const;
    f32 pictureWidth() const;
    f32 pictureHeight() const;
    void clearSelection();
    void showPicturePanes();
    void showContent();
    void drawFilled(hdScreenDrawInfo*);
    void drawContent(hdScreenDrawInfo*);
    void setPictureEnabled(u32);
    void refreshContentEnabled();
    void configureContent();
    void setArchiveIndex(u32);
    void copyPresentation(hdMenuMiiverseComment_c*);
    void updateComment();
    u32 processInput();
    void executeCheckInput();
    u32 initializeParts(u32, u32, void*);
    u32 createPart(hdMiiverseCommentPartRequest_l*);
    f32 contentGlobalY() const;
    u32 canShowPhoto() const;
    void clearCommentSelection();
    void clearPictureSelection();
    u32 commentRequested() const;
    u32 pictureRequested() const;
    void connectAllAboveVariant1(hdButtonPart_c*);
    void connectAllAboveVariant2(hdButtonPart_c*);
    void connectAllAboveVariant3(hdButtonPart_c*);
    void clearContentNavigation();
    void connectCommentToPicture();
    void connectContentPair();
    void hideContent();
    void refreshContentVisibility();
    void connectUpperControl(hdButtonPart_c*);
    void connectLowerControl(hdButtonPart_c*);
    void connectPictureControl(hdButtonPart_c*);
    hdParts_c* content() const;
};
WWHD_OFFSET(hdMenuMiiverseComment_c, mEnabled, 0x50);
WWHD_OFFSET(hdMenuMiiverseComment_c, mArchiveIndex, 0x64);
WWHD_OFFSET(hdMenuMiiverseComment_c, mFilled, 0x88);
WWHD_OFFSET(hdMenuMiiverseComment_c, mSelectionPending, 0xAC);
WWHD_OFFSET(hdMenuMiiverseComment_c, mOpenPending, 0xAD);
WWHD_OFFSET(hdMenuMiiverseComment_c, mCommentRequested, 0xAE);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPictureRequested, 0xAF);
WWHD_SIZE(hdMenuMiiverseComment_c, 0xD8);

WWHD_OFFSET(hdMenuMiiverseComment_c, mSavedY, 0x60);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPositionY, 0x54);
WWHD_OFFSET(hdMenuMiiverseComment_c, mTargetY, 0x58);
WWHD_OFFSET(hdMenuMiiverseComment_c, mScrollTarget, 0x5C);
WWHD_OFFSET(hdMenuMiiverseComment_c, mContentStorage, 0x68);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPictureBaseX, 0x8C);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPictureBaseY, 0x90);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPictureBaseZ, 0x94);
WWHD_OFFSET(hdMenuMiiverseComment_c, mContentBaseX, 0x98);
WWHD_OFFSET(hdMenuMiiverseComment_c, mContentBaseY, 0x9C);
WWHD_OFFSET(hdMenuMiiverseComment_c, mContentBaseZ, 0xA0);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPicturePositionY, 0xA4);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPictureSlideStep, 0xA8);
WWHD_OFFSET(hdMenuMiiverseComment_c, mInteractionFlags, 0xB0);
WWHD_OFFSET(hdMenuMiiverseComment_c, mTimerHigh, 0xB8);
WWHD_OFFSET(hdMenuMiiverseComment_c, mTimerLow, 0xBC);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPendingFlags, 0xC0);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPendingState, 0xC4);
WWHD_OFFSET(hdMenuMiiverseComment_c, mPicturePanes, 0xC8);
