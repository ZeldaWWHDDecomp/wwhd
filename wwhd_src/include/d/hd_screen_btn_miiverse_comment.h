/* HD-only Miiverse comment card and its bound panes. */
#pragma once
#include "d/hd_screen_btn_miiverse_picture.h"
#include "d/hd_save_holders.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_text_break.h"
struct hdMiiverseCommentPaneNames_c { hdUiSafeString mEntries[5]; };
struct hdMiiverseCommentTextNames_c { hdUiSafeString mEntries[5]; };
struct hdMiiverseCommentWindowNames_c { hdUiSafeString mEntries[9]; };
struct hdMiiverseCommentIconNames_c { hdUiSafeString mEntries[4]; };
struct hdMiiverseCommentDelegate_c {
    gptr<void> mVtable;
    gptr<void> mpOwner;
    hdPartsPtmf_c mMethod;
};
WWHD_SIZE(hdMiiverseCommentDelegate_c, 0x10);
struct hdMiiverseTextWriter_l {
    u8 mPrefix[0x28];
    gptr<PaneFont_l> mpFont;
    u8 mFontSettings[8];
    be<f32> mMaximumWidth;
    be<u32> mCharacterSpacingBits;
    be<u32> mLineSpacingBits;
    u8 mReserved[4];
    be<u32> mFlags;
    u8 mTail[4];
};
WWHD_OFFSET(hdMiiverseTextWriter_l, mpFont, 0x28);
WWHD_OFFSET(hdMiiverseTextWriter_l, mMaximumWidth, 0x34);
WWHD_OFFSET(hdMiiverseTextWriter_l, mCharacterSpacingBits, 0x38);
WWHD_OFFSET(hdMiiverseTextWriter_l, mLineSpacingBits, 0x3C);
WWHD_OFFSET(hdMiiverseTextWriter_l, mFlags, 0x44);
WWHD_SIZE(hdMiiverseTextWriter_l, 0x4C);
struct hdBtnMiiverseComment_c : hdBasicButton_c {
    be<u32> mPostIndex;
    gptr<hdPartsPane_c> mpPanes[5];
    gptr<hdPartsPane_c> mpTexts[5];
    gptr<hdPartsPane_c> mpIcons[4];
    gptr<hdPartsPane_c> mpWindows[9];
    gptr<hdPartsPane_c> mpPicture;
    hdMiiverseTextWriter_l mTextWriter;
    hdMiiverseCommentDelegate_c mTextWidthDelegate;
    be<f32> mMaximumCommentWidth;
    be<f32> mEmptyCommentWidth;
    static void initializeStatics();
    u32 init(void*, u32, hdPartsDesc_c*);
    u32 loadLayout(void*, u32, u32, u32, void*);
    void setPost(u32);
    void beginOnCursor();
    void executeOnCursor();
    void beginOffCursor();
    void executeOffCursor();
    u32 getStateNumber();
    static hdBtnMiiverseComment_c* construct(hdBtnMiiverseComment_c*);
    u8 isTextTooWide(hdBreakString_c*, u32);
    gptr<void>* getYeahPaneState();
    gptr<void>* getPicturePaneState();
    f32 getPictureHeight();
    void setOffCursorFrame();
    void setCommentCount();
    void setYeahCount();
    u8 isFriendPost();
    void showComment();
    void hideComment();
    void findPanes();
    void findTexts();
    void findPicture();
    void findIcons();
    void findWindows();
    static void destroy(hdBtnMiiverseComment_c*, s32);
};
WWHD_OFFSET(hdBtnMiiverseComment_c, mPostIndex, 0x8C);
WWHD_OFFSET(hdBtnMiiverseComment_c, mpPanes, 0x90);
WWHD_OFFSET(hdBtnMiiverseComment_c, mpTexts, 0xA4);
WWHD_OFFSET(hdBtnMiiverseComment_c, mpIcons, 0xB8);
WWHD_OFFSET(hdBtnMiiverseComment_c, mpWindows, 0xC8);
WWHD_OFFSET(hdBtnMiiverseComment_c, mpPicture, 0xEC);
WWHD_OFFSET(hdBtnMiiverseComment_c, mTextWriter, 0xF0);
WWHD_OFFSET(hdBtnMiiverseComment_c, mTextWidthDelegate, 0x13C);
WWHD_OFFSET(hdBtnMiiverseComment_c, mMaximumCommentWidth, 0x14C);
WWHD_OFFSET(hdBtnMiiverseComment_c, mEmptyCommentWidth, 0x150);
WWHD_SIZE(hdBtnMiiverseComment_c, 0x154);
void destroyMiiverseStringArray(void*, u32, u32, u32, u32, u32);

void destroyMiiverseTextWriter(void*, s32);

void constructMiiverseTextWriter(void*);
f32 measureMiiverseText(void*, be<u16>*, u32);

struct hdMiiversePost_l {
    hdBreakString_c mMiiName;
    u8 mNameDetails[0x1C];
    hdBreakString_c mTopic;
    u8 mTopicDetails[0x6C];
    hdBreakString_c mComment;
    u8 mCommentDetails[0x1AC];
    be<u8> mHasPicture;
    be<u8> mIsSpoiler;
    u8 mAlign[2];
    be<s32> mCommentCount;
    be<s32> mYeahCount;
    be<u8> mHasYeah;
    be<u8> mIsSpecialPost;
    u8 mDetails[0x31C - 0x25A];
    u8 mPictureTexture[0x3E5 - 0x31C];
    be<u8> mIsFriend;
    u8 mTail[6];
};
WWHD_OFFSET(hdMiiversePost_l, mMiiName, 0);
WWHD_OFFSET(hdMiiversePost_l, mTopic, 0x24);
WWHD_OFFSET(hdMiiversePost_l, mComment, 0x98);
WWHD_OFFSET(hdMiiversePost_l, mHasPicture, 0x24C);
WWHD_OFFSET(hdMiiversePost_l, mIsSpoiler, 0x24D);
WWHD_OFFSET(hdMiiversePost_l, mHasYeah, 0x258);
WWHD_OFFSET(hdMiiversePost_l, mIsSpecialPost, 0x259);
WWHD_OFFSET(hdMiiversePost_l, mCommentCount, 0x250);
WWHD_OFFSET(hdMiiversePost_l, mYeahCount, 0x254);
WWHD_OFFSET(hdMiiversePost_l, mPictureTexture, 0x31C);
WWHD_OFFSET(hdMiiversePost_l, mIsFriend, 0x3E5);
WWHD_SIZE(hdMiiversePost_l, 0x3EC);
struct hdMiiversePosts_l { u8 mPrefix[0x14]; hdMiiversePost_l mPosts[10]; };
WWHD_OFFSET(hdMiiversePosts_l, mPosts, 0x14);
struct hdMiiverseSaveRoot_l { u8 mPrefix[0x12C0]; hdSaveHolders_c mHolders; };
WWHD_OFFSET(hdMiiverseSaveRoot_l, mHolders, 0x12C0);
struct hdMiiverseCountString_c {
    gptr<be<u16>> mpText;
    gptr<void> mVtable;
    be<u32> mCapacity;
    be<u16> mBuffer[10];
};
WWHD_OFFSET(hdMiiverseCountString_c, mCapacity, 8);
WWHD_OFFSET(hdMiiverseCountString_c, mBuffer, 0xC);
WWHD_SIZE(hdMiiverseCountString_c, 0x20);
void formatMiiverseCount(hdMiiverseCountString_c*, be<u16>*, s32);
void setMiiverseText(hdPartsPane_c*, void*, void*);

PaneFont_l* getMiiversePaneFont(hdTextPane_c*);
void setMiiverseWriterFontSize(hdMiiverseTextWriter_l*, f32, f32);

void assureMiiverseWideString(hdBreakString_c*);
void copyMiiverseWideCharacters(void*, void*, u32, u32);

void assureMiiverseFixedWideString(hdBreakBuffer_c*);
void resolveMiiverseTopic(void*, hdBreakBuffer_c*, be<u16>*);
void prepareMiiverseMessage(PaneWork_l*, u32, u32);

struct hdMiiversePaneWidthSnapshot_l { u8 mPrefix[0x8C4]; be<u32> mWidthBits; };
WWHD_OFFSET(hdMiiversePaneWidthSnapshot_l, mWidthBits, 0x8C4);
