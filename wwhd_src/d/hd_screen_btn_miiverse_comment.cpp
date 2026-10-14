/* HD-only Miiverse comment-card state and TU-owned name arrays. Private decomp source. */
#include "d/hd_screen_btn_miiverse_comment.h"
static u32 sBtnMiiverseCommentPaneNamesRegistration;
static u32 sBtnMiiverseCommentIconNamesRegistration;
static u32 sBtnMiiverseCommentWindowNamesRegistration;
static u32 sBtnMiiverseCommentTextNamesRegistration;
static u32 sBtnMiiverseCommentHeaderPiPair;
static u32 sBtnMiiverseCommentHeaderRegistration;
static u32 sBtnMiiverseCommentHeaderZeroObject;
static u32 sBtnMiiverseCommentEmptyStringRegistration;
static hdMiiverseCommentPaneNames_c sCommentPaneNames;
static hdUiSafeString sCommentResource;
struct MiiverseTinyWideString {
    gptr<be<u16>> mpText;
    gptr<void> mVtable;
    be<s32> mCapacity;
    be<u16> mBuffer[2];
};
WWHD_OFFSET(MiiverseTinyWideString, mBuffer, 0xC);
static MiiverseTinyWideString sCommentEmptyString;
static u8 sWideStringBaseVtable;
static u8 sWideStringVtable;
static hdBreakStringVtable_c sWideSafeStringVtable;
struct MiiverseWideEmptyLiteral { be<u16> mText[1]; };
static MiiverseWideEmptyLiteral sWideEmptyLiteral;
static u32 sCommentStateCounterReady;
static u32 sCommentStateCounter;
static hdPartsStateId_c sFriendState;
static hdPartsStateId_c sYeahState;
static hdPartsStateIdVtbl_c sCommentStateVtable;
static hdStateId sCommentNullState;
static hdScreenUiSystem* sCommentUiSystem;
struct MiiverseAnimationNames { hdUiSafeString mEntries[5]; };
struct MiiverseAnimationGroups { hdUiSafeString mEntries[4]; };
struct MiiverseAnimationRemap { be<u32> mIndices[4]; };
static MiiverseAnimationNames sCommentAnimations;
static MiiverseAnimationGroups sCommentGroups;
static MiiverseAnimationRemap sCommentAnimationRemap;
struct MiiverseEmptyComment { be<u16> mText[2]; };
static MiiverseEmptyComment sEmptyComment;
static hdUiStringTable sCommentStringVtable;
struct CommentNamesGuard { be<u32> mInitialized; };
static CommentNamesGuard sCommentIconsNamesGuard;
static CommentNamesGuard sCommentWindowsNamesGuard;
static CommentNamesGuard sCommentPaneNamesGuard;
static hdMiiversePosts_l* sMiiversePosts;
static u32 sCommentTextNamesReady;
static u32 sTextTypeReady;
static u32 sTextBaseTypeReady;
static u32 sPaneTypeReady;
static u32 sRootTypeReady;
static hdPartsTypeInfo_c sTextType;
static hdPartsTypeInfo_c sTextBaseType;
static hdPartsTypeInfo_c sPaneType;
static hdPartsTypeInfo_c sRootType;
static u32 sPictureTypeReady;
static u32 sPictureBaseTypeReady;
static hdPartsTypeInfo_c sPictureType;
static hdPartsTypeInfo_c sPictureBaseType;
struct MiiversePictureName { gptr<const char> mName; };
static MiiversePictureName sCommentPictureName;
static hdMiiverseSaveRoot_l* sMiiverseSaveRoot;
static u8 sCountStringVtable;
struct MiiverseCountFormat { be<u16> mChars[3]; };
static MiiverseCountFormat sCommentCountFormat;
static MiiverseCountFormat sYeahCountFormat;
static hdPartsVtbl_c sCommentVtable;
static u8 sCommentDelegateVtable;
static u8 sCommentHolderVtable;
static u8 sCommentSecondaryVtable;
static hdMiiverseCommentTextNames_c sCommentTextNames;
static hdMiiverseCommentIconNames_c sCommentIconNames;
static hdMiiverseCommentWindowNames_c sCommentWindowNames;
enum { COMMENT_STRING_DESTRUCTOR = 0x02640FAC, CURSOR_ON_ENTRY = 8, CURSOR_ON_SLOT = 3, CURSOR_OFF_ENTRY = 9, CURSOR_OFF_SLOT = 5 };
/* 0263E1D0 */
static u32 getMiiverseCommentStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0263E2D0 */
static void destroyMiiverseCommentPaneNames() { destroyMiiverseStringArray(&sCommentPaneNames, 5, sizeof(hdUiSafeString), COMMENT_STRING_DESTRUCTOR, 0, 0); }
/* 0263E2F4 */
static void destroyMiiverseCommentTextNames() { destroyMiiverseStringArray(&sCommentTextNames, 5, sizeof(hdUiSafeString), COMMENT_STRING_DESTRUCTOR, 0, 0); }
/* 0263E318 */
static void destroyMiiverseCommentIconNames() { destroyMiiverseStringArray(&sCommentIconNames, 4, sizeof(hdUiSafeString), COMMENT_STRING_DESTRUCTOR, 0, 0); }
/* 0263E33C */
static void destroyMiiverseCommentWindowNames() { destroyMiiverseStringArray(&sCommentWindowNames, 9, sizeof(hdUiSafeString), COMMENT_STRING_DESTRUCTOR, 0, 0); }
/* 0263E3D8 */
void hdBtnMiiverseComment_c::beginOnCursor() { hdAnimSet_play(mButton.mpLayout->animSet(), CURSOR_ON_ENTRY, CURSOR_ON_SLOT, 1.0f); }
/* 0263E3F4 */
void hdBtnMiiverseComment_c::executeOnCursor() { mButton.vCalcLayout(); }
/* 0263E404 */
void hdBtnMiiverseComment_c::beginOffCursor() { hdAnimSet_play(mButton.mpLayout->animSet(), CURSOR_OFF_ENTRY, CURSOR_OFF_SLOT, 1.0f); }
/* 0263E420 */
void hdBtnMiiverseComment_c::executeOffCursor() { mButton.vCalcLayout(); }
/* 0263E360 */
void hdBtnMiiverseComment_c::destroy(hdBtnMiiverseComment_c* button, s32 flags) {
    if (button == NULL) return;
    button->mButton.mStateHolder.mHolderVtable = &sCommentHolderVtable;
    button->mButton.mSecondaryVtable = &sCommentSecondaryVtable;
    destroyMiiverseTextWriter(&button->mTextWriter, 2);
    hdBasicButton_c::destroy(button, 0);
    if (flags & 1) operator_delete(button);
}

/* 0263E558 */
void hdBtnMiiverseComment_c::findPanes() {
    if (!sCommentPaneNamesGuard.mInitialized) {
        sCommentPaneNamesGuard.mInitialized = 1;
        sCommentPaneNames.mEntries[0].mVtable = &sCommentStringVtable;
        sCommentPaneNames.mEntries[0].mText.v = gabi::guest_addr("N_All_00");
        sCommentPaneNames.mEntries[1].mVtable = &sCommentStringVtable;
        sCommentPaneNames.mEntries[1].mText.v = gabi::guest_addr("N_BtnYeah_00");
        sCommentPaneNames.mEntries[2].mVtable = &sCommentStringVtable;
        sCommentPaneNames.mEntries[2].mText.v = gabi::guest_addr("N_BtnPicture_00");
        sCommentPaneNames.mEntries[3].mVtable = &sCommentStringVtable;
        sCommentPaneNames.mEntries[3].mText.v = gabi::guest_addr("N_YeahComment_00");
        sCommentPaneNames.mEntries[4].mVtable = &sCommentStringVtable;
        sCommentPaneNames.mEntries[4].mText.v = gabi::guest_addr("C_Btn_00");
        __register_global_object(gabi::ea(&sBtnMiiverseCommentPaneNamesRegistration));
    }
    hdPartsLyt_c* layout = mButton.mpLayout->lyt();
    hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    for (u32 i = 0; i < 5; ++i) {
        hdUiSafeString* name = &sCommentPaneNames.mEntries[i];
        hdMiiversePaneLookupVtbl_l* vtable = root->mVtable;
        name->assureTermination();
        hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(vtable->mFind.get(), root, name->mText.get(), 1);
        mpPanes[i] = pane;
    }
}

/* 0263E430 */
hdBtnMiiverseComment_c* hdBtnMiiverseComment_c::construct(hdBtnMiiverseComment_c* card) {
    if (card == NULL) {
        card = (hdBtnMiiverseComment_c*)operator_new(sizeof(hdBtnMiiverseComment_c));
        if (card == NULL) return NULL;
    }
    hdPartsBasicButton_construct(&card->mButton);
    card->mButton.mVtable = &sCommentVtable;
    card->mButton.mStateHolder.mHolderVtable = &sCommentHolderVtable;
    card->mButton.mSecondaryVtable = &sCommentSecondaryVtable;
    card->mPostIndex = 0;
    // GHS retains the embedded fields even when their wrapped addresses are NULL.
    if (gabi::ea(&card->mpPanes) == 0) operator_new(sizeof(card->mpPanes));
    if (gabi::ea(&card->mpTexts) == 0) operator_new(sizeof(card->mpTexts));
    if (gabi::ea(&card->mpIcons) == 0) operator_new(sizeof(card->mpIcons));
    if (gabi::ea(&card->mpWindows) == 0) operator_new(sizeof(card->mpWindows));
    if (gabi::ea(&card->mpPicture) == 0) operator_new(sizeof(card->mpPicture));
    constructMiiverseTextWriter(&card->mTextWriter);
    hdMiiverseCommentDelegate_c* delegate = &card->mTextWidthDelegate;
    if (gabi::ea(delegate) == 0) {
        delegate = (hdMiiverseCommentDelegate_c*)operator_new(sizeof(hdMiiverseCommentDelegate_c));
        if (delegate == NULL) return card;
    }
    delegate->mpOwner = card;
    delegate->mMethod.mThisDelta = 0;
    delegate->mVtable = &sCommentDelegateVtable;
    enum { TEXT_WIDTH_CALLBACK = 0x02640B38 };
    delegate->mMethod.mFunction = TEXT_WIDTH_CALLBACK;
    delegate->mMethod.mVirtualIndex = -1;
    return card;
}

/* 0263EFB0 */
u32 hdBtnMiiverseComment_c::init(void* resource, u32 mode, hdPartsDesc_c* desc) {
    if (!hdPartsBasicButton_init(&mButton, &sCommentResource, (u32)gabi::ea(resource), mode, desc)) return 0;
    findPanes();
    findTexts();
    findIcons();
    findWindows();
    findPicture();
    hdTextPane_c* comment = (hdTextPane_c*)mpTexts[2].get();
    mMaximumCommentWidth = (f32)comment->mWidth - 10.0f;
    mTextWriter.mpFont = getMiiversePaneFont(comment);
    setMiiverseWriterFontSize(&mTextWriter, comment->mFontWidth, comment->mFontHeight);
    // The recompiled reference preserves these copied payload bits.
    mTextWriter.mLineSpacingBits = *hdPartsView<be<u32>>(gabi::ea(&comment->mLineSpace));
    mTextWriter.mCharacterSpacingBits = *hdPartsView<be<u32>>(gabi::ea(&comment->mCharSpace));
    mTextWriter.mFlags = 0;
    mTextWriter.mMaximumWidth = 3.4028234663852886e+38f;
    be<u16>* empty = sEmptyComment.mText;
    u32 length = 0;
    while (*hdPartsView<be<u16>>(gabi::ea(empty) + length * 2) != 0) ++length;
    mEmptyCommentWidth = measureMiiverseText(&mTextWriter, empty, length);
    ((hdTextPane_c*)mpTexts[0].get())->setCharacterCapacity(0x33);
    ((hdTextPane_c*)mpTexts[1].get())->setCharacterCapacity(0xB);
    ((hdTextPane_c*)mpTexts[2].get())->setCharacterCapacity(0xD3);
    return 1;
}

/* 0263F118 */
f32 hdBtnMiiverseComment_c::getPictureHeight() { return ((hdMiiversePicturePane_l*)mpPicture.get())->mHeight; }
/* 026409EC */
gptr<void>* hdBtnMiiverseComment_c::getYeahPaneState() {
    return hdPartsView<gptr<void>>(gabi::ea(mpPanes[1].get()) + sizeof(hdPartsPane_c));
}
/* 026409F8 */
gptr<void>* hdBtnMiiverseComment_c::getPicturePaneState() {
    return hdPartsView<gptr<void>>(gabi::ea(mpPanes[2].get()) + sizeof(hdPartsPane_c));
}
/* 02640A04 */
void hdBtnMiiverseComment_c::setOffCursorFrame() { hdAnimSet_setFrame(mButton.mpLayout->animSet(), CURSOR_OFF_ENTRY, CURSOR_OFF_SLOT, 1.0f); }
/* 02640A20 */
u32 hdBtnMiiverseComment_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    enum { ANIMATION_COUNT = 11, GROUP_COUNT = 7, FIRST_CUSTOM_ANIMATION = 7, CUSTOM_ANIMATION_COUNT = 4 };
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), sCommentUiSystem->mHeap, 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mButton.mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, first, second, third, ANIMATION_COUNT, GROUP_COUNT, desc)) return 0;
    for (u32 i = 0; i < CUSTOM_ANIMATION_COUNT; ++i) {
        hdAnimSet* animations = mButton.mpLayout->animSet();
        u32 index = sCommentAnimationRemap.mIndices[i];
        hdUiSafeString* group = hdPartsView<hdUiSafeString>(gabi::ea(&sCommentGroups) + i * sizeof(hdUiSafeString));
        hdUiSafeString* animation = hdPartsView<hdUiSafeString>(gabi::ea(&sCommentAnimations) + index * sizeof(hdUiSafeString));
        setupMiiverseAnimation(animations, FIRST_CUSTOM_ANIMATION + i, group, animation);
    }
    return 1;
}

/* 02640B38 */
u8 hdBtnMiiverseComment_c::isTextTooWide(hdBreakString_c* text, u32 length) {
    text->assureTermination();
    f32 width = measureMiiverseText(&mTextWriter, text->mText.get(), length);
    return width > mMaximumCommentWidth;
}

/* 0263EA2C */
void hdBtnMiiverseComment_c::findIcons() {
    if (!sCommentIconsNamesGuard.mInitialized) {
        sCommentIconsNamesGuard.mInitialized = 1;
        sCommentIconNames.mEntries[0].mVtable = &sCommentStringVtable;
        sCommentIconNames.mEntries[0].mText.v = gabi::guest_addr("P_DummyTex_00");
        sCommentIconNames.mEntries[1].mVtable = &sCommentStringVtable;
        sCommentIconNames.mEntries[1].mText.v = gabi::guest_addr("MiiverseTagIcon_01");
        sCommentIconNames.mEntries[2].mVtable = &sCommentStringVtable;
        sCommentIconNames.mEntries[2].mText.v = gabi::guest_addr("P_YeahIcon_01");
        sCommentIconNames.mEntries[3].mVtable = &sCommentStringVtable;
        sCommentIconNames.mEntries[3].mText.v = gabi::guest_addr("P_CommentIcon_00");
        __register_global_object(gabi::ea(&sBtnMiiverseCommentIconNamesRegistration));
    }
    hdPartsLyt_c* layout = mButton.mpLayout->lyt();
    hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    for (u32 i = 0; i < 4; ++i) {
        hdUiSafeString* name = &sCommentIconNames.mEntries[i];
        hdMiiversePaneLookupVtbl_l* vtable = root->mVtable;
        name->assureTermination();
        hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(vtable->mFind.get(), root, name->mText.get(), 1);
        mpIcons[i] = pane;
    }
}

/* 0263EBE4 */
void hdBtnMiiverseComment_c::findWindows() {
    if (!sCommentWindowsNamesGuard.mInitialized) {
        sCommentWindowsNamesGuard.mInitialized = 1;
        sCommentWindowNames.mEntries[0].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[0].mText.v = gabi::guest_addr("W_BaseSh_00");
        sCommentWindowNames.mEntries[1].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[1].mText.v = gabi::guest_addr("W_Base_00");
        sCommentWindowNames.mEntries[2].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[2].mText.v = gabi::guest_addr("W_MiiNameBase_00");
        sCommentWindowNames.mEntries[3].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[3].mText.v = gabi::guest_addr("W_BaseSh_01");
        sCommentWindowNames.mEntries[4].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[4].mText.v = gabi::guest_addr("W_Base_01");
        sCommentWindowNames.mEntries[5].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[5].mText.v = gabi::guest_addr("W_BaseSh_02");
        sCommentWindowNames.mEntries[6].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[6].mText.v = gabi::guest_addr("W_Base_02");
        sCommentWindowNames.mEntries[7].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[7].mText.v = gabi::guest_addr("W_BaseSh_03");
        sCommentWindowNames.mEntries[8].mVtable = &sCommentStringVtable;
        sCommentWindowNames.mEntries[8].mText.v = gabi::guest_addr("W_Base_03");
        __register_global_object(gabi::ea(&sBtnMiiverseCommentWindowNamesRegistration));
    }
    hdPartsLyt_c* layout = mButton.mpLayout->lyt();
    hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    for (u32 i = 0; i < 9; ++i) {
        hdUiSafeString* name = &sCommentWindowNames.mEntries[i];
        hdMiiversePaneLookupVtbl_l* vtable = root->mVtable;
        name->assureTermination();
        hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(vtable->mFind.get(), root, name->mText.get(), 1);
        mpWindows[i] = pane;
    }
}

/* inlined into the post-dependent comment-card functions */
static hdMiiversePost_l* selectedMiiversePost(u32 index) {
    u32 base = gabi::ea(sMiiversePosts) + offsetof(hdMiiversePosts_l, mPosts);
    return hdPartsView<hdMiiversePost_l>(base + (index < 10 ? index * sizeof(hdMiiversePost_l) : 0));
}
/* 0263F124 */
void hdBtnMiiverseComment_c::setCommentCount() {
    hdMiiversePost_l* post = selectedMiiversePost(mPostIndex);
    hdMiiverseCountString_c text;
    text.mBuffer[9] = 0;
    text.mpText = &text.mBuffer[0];
    text.mCapacity = 10;
    text.mBuffer[0] = 0;
    text.mVtable = &sCountStringVtable;
    f32 count = (f32)(s32)post->mCommentCount;
    if (count > 9999.0f) count = 9999.0f;
    formatMiiverseCount(&text, &sCommentCountFormat.mChars[0], gabi::ftoi(count));
    setMiiverseText(mpTexts[4], &text, 0);
}
/* 0263F204 */
void hdBtnMiiverseComment_c::setYeahCount() {
    hdMiiversePost_l* post = selectedMiiversePost(mPostIndex);
    hdMiiverseCountString_c text;
    text.mBuffer[9] = 0;
    text.mpText = &text.mBuffer[0];
    text.mCapacity = 10;
    text.mBuffer[0] = 0;
    text.mVtable = &sCountStringVtable;
    f32 count = (f32)(s32)post->mYeahCount;
    if (count > 9999.0f) count = 9999.0f;
    formatMiiverseCount(&text, &sYeahCountFormat.mChars[0], gabi::ftoi(count));
    setMiiverseText(mpTexts[3], &text, 0);
}
/* 0263F2E4 */
u8 hdBtnMiiverseComment_c::isFriendPost() {
    if (!selectedMiiversePost(mPostIndex)->mIsFriend) return 0;
    hdSavePair_c* flags = sMiiverseSaveRoot->mHolders.getScratchPair();
    return flags->mSecondary == 1;
}
/* 0263F35C */
void hdBtnMiiverseComment_c::hideComment() {
    enum { COMMENT_ENTRY = 10, COMMENT_SLOT = 6, VISIBLE = 1 };
    hdAnimSet_setFrame(mButton.mpLayout->animSet(), COMMENT_ENTRY, COMMENT_SLOT, 1.0f);
    hdMiiversePicturePane_l* pane = (hdMiiversePicturePane_l*)mpTexts[0].get();
    pane->mFlags = u8(pane->mFlags) & ~VISIBLE;
    pane = (hdMiiversePicturePane_l*)mpTexts[2].get();
    pane->mFlags = u8(pane->mFlags) & ~VISIBLE;
    pane = (hdMiiversePicturePane_l*)mpPanes[3].get();
    pane->mFlags = u8(pane->mFlags) & ~VISIBLE;
    pane = (hdMiiversePicturePane_l*)mpIcons[0].get();
    pane->mFlags = u8(pane->mFlags) & ~VISIBLE;
    mButton.vCalcLayout();
}
/* 0263F3F4 */
void hdBtnMiiverseComment_c::showComment() {
    enum { COMMENT_ENTRY = 10, COMMENT_SLOT = 6, VISIBLE = 1 };
    hdAnimSet_setFrame(mButton.mpLayout->animSet(), COMMENT_ENTRY, COMMENT_SLOT, 0.0f);
    hdMiiversePicturePane_l* pane = (hdMiiversePicturePane_l*)mpTexts[0].get();
    pane->mFlags = (u8(pane->mFlags) & ~VISIBLE) + VISIBLE;
    pane = (hdMiiversePicturePane_l*)mpPanes[3].get();
    pane->mFlags = (u8(pane->mFlags) & ~VISIBLE) + VISIBLE;
    if (selectedMiiversePost(mPostIndex)->mHasPicture) {
        pane = (hdMiiversePicturePane_l*)mpIcons[0].get();
        u8 flags = pane->mFlags;
        pane->mFlags = flags & ~VISIBLE;
        pane->mFlags = (flags & ~VISIBLE) | VISIBLE;
    } else {
        pane = (hdMiiversePicturePane_l*)mpTexts[2].get();
        u8 flags = pane->mFlags;
        pane->mFlags = flags & ~VISIBLE;
        pane->mFlags = flags | VISIBLE;
    }
    mButton.vCalcLayout();
}
/* inlined into the comment-card runtime text/picture casts */
static hdPartsTypeInfo_c* commentTextType() {
    if (!sTextTypeReady) {
        sTextTypeReady = 1;
        if (!sTextBaseTypeReady) {
            sTextBaseTypeReady = 1;
            if (!sPaneTypeReady) {
                sPaneTypeReady = 1;
                if (!sRootTypeReady) {
                    sRootType.mParent = NULL;
                    sRootTypeReady = 1;
                }
                sPaneType.mParent = &sRootType;
            }
            sTextBaseType.mParent = &sPaneType;
        }
        sTextType.mParent = &sTextBaseType;
    }
    return &sTextType;
}
/* inlined into the comment-card picture cast */
hdPartsTypeInfo_c* miiversePictureType() {
    if (!sPictureTypeReady) {
        sPictureTypeReady = 1;
        if (!sPictureBaseTypeReady) {
            sPictureBaseTypeReady = 1;
            if (!sRootTypeReady) {
                sRootTypeReady = 1;
                sRootType.mParent = NULL;
            }
            sPictureBaseType.mParent = &sRootType;
        }
        sPictureType.mParent = &sPictureBaseType;
    }
    return &sPictureType;
}
/* inlined into the comment-card runtime text/picture casts */
hdPartsPane_c* castMiiversePane(hdPartsPane_c* pane, hdPartsTypeInfo_c* expected) {
    if (pane == NULL) return NULL;
    hdPartsTypeInfo_c* type = pane->getRuntimeTypeInfo();
    while (type != NULL) {
        if (type == expected) return pane;
        type = type->mParent;
    }
    return NULL;
}
/* 0263EE78 */
void hdBtnMiiverseComment_c::findPicture() {
    hdPartsLyt_c* layout = mButton.mpLayout->lyt();
    hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(root->mVtable->mFind.get(), root, sCommentPictureName.mName.get(), 1);
    hdPartsTypeInfo_c* type = miiversePictureType();
    mpPicture = castMiiversePane(pane, type);
}
/* 0263E73C */
void hdBtnMiiverseComment_c::findTexts() {
    if (!sCommentTextNamesReady) {
        sCommentTextNamesReady = 1;
        sCommentTextNames.mEntries[0].mVtable = &sCommentStringVtable;
        sCommentTextNames.mEntries[0].mText.v = gabi::guest_addr("T_TopicTag_00");
        sCommentTextNames.mEntries[1].mVtable = &sCommentStringVtable;
        sCommentTextNames.mEntries[1].mText.v = gabi::guest_addr("T_MiiName_00");
        sCommentTextNames.mEntries[2].mVtable = &sCommentStringVtable;
        sCommentTextNames.mEntries[2].mText.v = gabi::guest_addr("T_MiiverseComment_00");
        sCommentTextNames.mEntries[3].mVtable = &sCommentStringVtable;
        sCommentTextNames.mEntries[3].mText.v = gabi::guest_addr("T_NumComment_00");
        sCommentTextNames.mEntries[4].mVtable = &sCommentStringVtable;
        sCommentTextNames.mEntries[4].mText.v = gabi::guest_addr("T_NumYeah_00");
        __register_global_object(gabi::ea(&sBtnMiiverseCommentTextNamesRegistration));
    }
    hdPartsLyt_c* layout = mButton.mpLayout->lyt();
    hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    for (u32 i = 0; i < 5; ++i) {
        hdUiSafeString* name = &sCommentTextNames.mEntries[i];
        hdMiiversePaneLookupVtbl_l* vtable = root->mVtable;
        name->assureTermination();
        hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(vtable->mFind.get(), root, name->mText.get(), 1);
        hdPartsTypeInfo_c* expected = commentTextType();
        mpTexts[i] = castMiiversePane(pane, expected);
    }
}
/* SafeString operations inlined by the original comment-card update. */
template <u32 Capacity> struct MiiverseWideBuffer : hdBreakBuffer_c { be<u16> mBuffer[Capacity]; };
static hdBreakStringVtable_c sMiiNameBaseVtable;
static hdBreakStringVtable_c sMiiNameVtable;
static hdBreakStringVtable_c sTopicVtable;
static hdBreakStringVtable_c sCommentBaseVtable;
static hdBreakStringVtable_c sCommentFixedVtable;
static hdBreakStringVtable_c sScratchBaseVtable;
static hdBreakStringVtable_c sScratchVtable;
struct MiiverseMessageManager { gptr<void> mManager; };
static MiiverseMessageManager sMiiverseMessageManager;
struct MiiverseWideFallback { gptr<be<u16>> mText; };
static MiiverseWideFallback sMiiverseWideFallback;
struct MiiverseFallbackCharacter { be<u16> mValue; };
static MiiverseFallbackCharacter sMiiverseFallbackCharacter;

static be<u16>* miiverseWideAt(be<u16>* text, s32 index) {
    return hdPartsView<be<u16>>(gabi::ea(text) + u32(index) * sizeof(u16));
}
static s32 miiverseWideLength(be<u16>* text) {
    enum { MAXIMUM_SAFE_LENGTH = 0x40000 };
    s32 length = 0;
    while (*miiverseWideAt(text, length) != 0) {
        if (++length > MAXIMUM_SAFE_LENGTH) return 0;
    }
    return length;
}
static s32 miiverseWideLength(hdBreakString_c* string) {
    string->assureTermination();
    return miiverseWideLength(string->mText.get());
}
static u32 miiverseUnboundedWideLength(be<u16>* text) {
    u32 length = 0;
    while (*miiverseWideAt(text, length) != 0) ++length;
    return length;
}
template <u32 Capacity>
static void initializeMiiverseWideBuffer(MiiverseWideBuffer<Capacity>& string, hdBreakStringVtable_c* vtable, bool clearFirst) {
    string.mText = string.mBuffer;
    string.mVtable = vtable;
    string.mCapacity = Capacity;
    string.mBuffer[Capacity - 1] = 0;
    if (clearFirst) string.mBuffer[0] = 0;
}
static void copyMiiverseWideString(hdBreakBuffer_c* destination, be<u16>* buffer, hdBreakString_c* source, bool prepared = false) {
    s32 length = prepared ? miiverseWideLength(source->mText.get()) : miiverseWideLength(source);
    s32 capacity = destination->mCapacity;
    if (length >= capacity) length = capacity - 1;
    source->assureTermination();
    copyMiiverseWideCharacters(buffer, source->mText.get(), u32(length) * sizeof(u16), 0);
    *miiverseWideAt(buffer, length) = 0;
}
static bool miiverseContainsLineFeed(hdBreakString_c* string) {
    be<u16>* text = string->mText;
    for (u32 i = 0; i < 0x40001; ++i) {
        u16 value = *miiverseWideAt(text, i);
        if (value == 0) return false;
        if (value == '\n') return true;
    }
    return false;
}
static void miiverseAppendCharacter(hdBreakBuffer_c* destination, u16 character) {
    destination->assureTermination();
    be<u16>* text = destination->mText;
    s32 length = miiverseWideLength(text);
    if (length < (s32)destination->mCapacity - 1) {
        *miiverseWideAt(text, length) = character;
        *miiverseWideAt(text, length + 1) = 0;
    }
}
static void miiverseAppendWithoutLineFeeds(hdBreakBuffer_c* destination, hdBreakString_c* source, s32 sourceLength) {
    u32 end = u32(sourceLength) + 1;
    if (end == 0) return;
    for (s32 i = 0; u32(i) != end; ++i) {
        s32 length = miiverseWideLength(source);
        be<u16>* text = source->mText;
        if (i >= 0 && i <= length && *miiverseWideAt(text, i) == '\n') continue;
        length = miiverseWideLength(source);
        text = source->mText;
        u16 character = i < 0 || i > length ? (u16)sMiiverseFallbackCharacter.mValue : (u16)*miiverseWideAt(text, i);
        miiverseAppendCharacter(destination, character);
    }
}
static void truncateMiiverseWideString(hdBreakBuffer_c* string, s32 maximum) {
    if ((s32)string->mCapacity > maximum) *miiverseWideAt(string->mText.get(), maximum) = 0;
    else (void)miiverseWideLength(string);
}
static void appendMiiverseEllipsis(hdBreakBuffer_c* destination, be<u16>* text, s32 position) {
    hdBreakString_c* ellipsis = (hdBreakString_c*)&sCommentEmptyString;
    s32 length = miiverseWideLength(ellipsis);
    s32 remaining = (s32)destination->mCapacity - position;
    if (length >= remaining) length = remaining - 1;
    if (length > 0) {
        be<u16>* output = miiverseWideAt(text, position);
        ellipsis->assureTermination();
        copyMiiverseWideCharacters(output, ellipsis->mText.get(), u32(length) * sizeof(u16), 0);
        *miiverseWideAt(text, position + length) = 0;
    }
}

/* 0263F4F0 */
void hdBtnMiiverseComment_c::setPost(u32 index) {
    auto refreshFrame = gabi::AutoFrame(0x0263F4F0);
    mPostIndex = index;
    hdMiiversePost_l* post = selectedMiiversePost(index);
    gabi::FrameLocal<MiiverseWideBuffer<11>> nameStorage(0xAC);
    MiiverseWideBuffer<11>& name = *nameStorage;
    initializeMiiverseWideBuffer(name, &sMiiNameBaseVtable, false);
    copyMiiverseWideString(&name, name.mBuffer, &post->mMiiName);
    name.mVtable = &sMiiNameVtable;
    truncateMiiverseWideString(&name, 10);
    setMiiverseText(mpTexts[1], &name, 0);

    gabi::FrameLocal<MiiverseWideBuffer<51>> topicStorage(0x38);
    MiiverseWideBuffer<51>& topic = *topicStorage;
    initializeMiiverseWideBuffer(topic, &sTopicVtable, true);
    post->mTopic.assureTermination();
    resolveMiiverseTopic(sMiiverseMessageManager.mManager.get(), &topic, post->mTopic.mText.get());
    topic.assureTermination();
    gabi::FrameLocal<MiiverseWideBuffer<256>> scratchStorage(0x438);
    MiiverseWideBuffer<256>& scratch = *scratchStorage;
    if (miiverseContainsLineFeed(&topic)) {
        s32 length = miiverseWideLength(&topic);
        if (length > 0) {
            *miiverseWideAt(topic.mText.get(), 0) = 0;
            initializeMiiverseWideBuffer(scratch, &sScratchVtable, true);
            post->mTopic.assureTermination();
            resolveMiiverseTopic(sMiiverseMessageManager.mManager.get(), &scratch, post->mTopic.mText.get());
            length = miiverseWideLength(&scratch);
            miiverseAppendWithoutLineFeeds(&topic, &scratch, length);
        }
    }
    truncateMiiverseWideString(&topic, 50);
    setMiiverseText(mpTexts[0], &topic, 0);
    setCommentCount();
    setYeahCount();

    gabi::FrameLocal<MiiverseWideBuffer<211>> commentStorage(0x284);
    MiiverseWideBuffer<211>& comment = *commentStorage;
    initializeMiiverseWideBuffer(comment, &sCommentBaseVtable, false);
    copyMiiverseWideString(&comment, comment.mBuffer, &post->mComment);
    comment.mVtable = &sCommentFixedVtable;
    assureMiiverseFixedWideString(&comment);
    if (miiverseContainsLineFeed(&comment)) {
        s32 length = miiverseWideLength(&comment);
        if (length > 0) {
            initializeMiiverseWideBuffer(scratch, &sScratchBaseVtable, false);
            copyMiiverseWideString(&scratch, scratch.mBuffer, &post->mComment);
            *miiverseWideAt(comment.mText.get(), 0) = 0;
            scratch.mVtable = &sScratchVtable;
            length = miiverseWideLength(&scratch);
            miiverseAppendWithoutLineFeeds(&comment, &scratch, length);
        }
    }
    truncateMiiverseWideString(&comment, 200);
    gabi::FrameLocal<MiiverseWideBuffer<211>> wrappedStorage(0xD0);
    MiiverseWideBuffer<211>& wrapped = *wrappedStorage;
    initializeMiiverseWideBuffer(wrapped, &sCommentFixedVtable, true);
    comment.assureTermination();
    gabi::FrameLocal<hdBreakString_c> inputStorage(0x24);
    hdBreakString_c& input = *inputStorage;
    input.mText = comment.mText.get();
    input.mVtable = &sWideSafeStringVtable;
    gabi::FrameLocal<hdBreakOptions_c> optionsStorage(0x18);
    hdBreakOptions_c& options = *optionsStorage;
    options.mMaxLines = 0;
    options.mSkipSpaces = 1;
    options.mClassOptions = 0;
    hdTextBreak_wrap(&wrapped, &input, (hdBreakMeasure_c*)&mTextWidthDelegate, &options);
    wrapped.assureTermination();
    s32 lineCount = 0;
    s32 position = 0;
    if (miiverseContainsLineFeed(&wrapped)) {
        s32 length = miiverseWideLength(&wrapped);
        if (length > 0) {
            u32 end = u32(miiverseWideLength(&wrapped)) + 1;
            s32 previousLine = 0;
            if (end != 0) for (s32 i = 0; u32(i) != end; ++i, ++position) {
                s32 currentLength = miiverseWideLength(&wrapped);
                be<u16>* text = wrapped.mText;
                if (i < 0 || i > currentLength || *miiverseWideAt(text, i) != '\n') continue;
                ++lineCount;
                s32 earlierLine = previousLine;
                previousLine = position;
                if (lineCount < 3) continue;
                s32 cut = position - 1;
                if (cut >= (s32)wrapped.mCapacity) (void)miiverseWideLength(&wrapped);
                else *miiverseWideAt(wrapped.mText.get(), cut < 0 ? 0 : cut) = 0;
                bool tooWide = false;
                if (position >= 199) tooWide = true;
                else {
                    currentLength = miiverseWideLength(&wrapped);
                    text = wrapped.mText;
                    be<u16>* tail = earlierLine + 1 < 0 || earlierLine + 1 > currentLength ? sMiiverseWideFallback.mText.get() : miiverseWideAt(text, earlierLine + 1);
                    gabi::FrameLocal<hdBreakString_c> suffixStorage(0x10);
                    hdBreakString_c& suffix = *suffixStorage;
                    suffix.mText = tail;
                    suffix.mVtable = &sWideSafeStringVtable;
                    scratch.mText = scratch.mBuffer;
                    scratch.mCapacity = 211;
                    scratch.mVtable = &sCommentBaseVtable;
                    scratch.mBuffer[210] = 0;
                    assureMiiverseWideString(&suffix);
                    copyMiiverseWideString(&scratch, scratch.mBuffer, &suffix, true);
                    scratch.mVtable = &sCommentFixedVtable;
                    assureMiiverseFixedWideString(&scratch);
                    be<u16>* candidate = scratch.mText;
                    s32 tailLength = miiverseWideLength(candidate);
                    appendMiiverseEllipsis(&scratch, candidate, tailLength);
                    scratch.assureTermination();
                    candidate = scratch.mText;
                    f32 width = measureMiiverseText(&mTextWriter, candidate, miiverseUnboundedWideLength(candidate));
                    if (mMaximumCommentWidth < width) tooWide = true;
                }
                wrapped.assureTermination();
                text = wrapped.mText;
                currentLength = miiverseWideLength(text);
                s32 removed = 0;
                if (tooWide && currentLength > 1) {
                    s32 remaining = currentLength - 1;
                    s32 count = 1;
                    s32 tailIndex = currentLength - 1;
                    do {
                        s32 lengthNow = miiverseWideLength(&wrapped);
                        text = wrapped.mText;
                        be<u16>* tail = tailIndex < 0 || tailIndex > lengthNow ? sMiiverseWideFallback.mText.get() : miiverseWideAt(text, tailIndex);
                        gabi::FrameLocal<hdBreakString_c> suffixStorage(8);
                        hdBreakString_c& suffix = *suffixStorage;
                        suffix.mText = tail;
                        suffix.mVtable = &sWideSafeStringVtable;
                        assureMiiverseWideString(&suffix);
                        be<u16>* suffixText = suffix.mText.get();
                        f32 width = measureMiiverseText(&mTextWriter, suffixText, miiverseUnboundedWideLength(suffixText));
                        if (mEmptyCommentWidth < width) { removed = count; text = wrapped.mText; break; }
                        --remaining;
                        ++count;
                        --tailIndex;
                    } while (remaining != 0);
                    text = wrapped.mText;
                }
                s32 appendPosition = currentLength - removed;
                if (appendPosition < 0) {
                    appendPosition += miiverseWideLength(&wrapped) + 1;
                    if (appendPosition < 0) appendPosition = 0;
                }
                appendMiiverseEllipsis(&wrapped, text, appendPosition);
                break;
            }
        }
    }
    hdTextPane_c* pane = (hdTextPane_c*)mpTexts[2].get();
    ((hdMiiversePaneWidthSnapshot_l*)pane)->mWidthBits = *hdPartsView<be<u32>>(gabi::ea(&pane->mWidth));
    PaneWork_l* work = &pane->mWork;
    prepareMiiverseMessage(work, 0, 0);
    setMiiverseText(mpTexts[2], &wrapped, work);
    s32 frame;
    if (post->mHasPicture) {
        hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)mButton.mpLayout->lyt()->mpRootPane.get();
        hdMiiversePaneLookup_l* picture = gabi::call_ptr<hdMiiversePaneLookup_l*>(root->mVtable->mFind.get(), root, "P_DummyTex_00", 1);
        frame = 1;
        if (picture != NULL) setMiiversePictureTexture(picture, post->mPictureTexture, 0, 0);
    } else if (post->mIsSpecialPost) frame = lineCount + 1 <= 1 ? 7 : lineCount + 1 == 2 ? 6 : 5;
    else frame = lineCount + 1 <= 1 ? 4 : lineCount + 1 == 2 ? 3 : 2;
    hdAnimSet_setFrame(mButton.mpLayout->animSet(), 7, 2, (f32)frame);
    if (isFriendPost()) hideComment(); else showComment();
    hdAnimSet_setFrame(mButton.mpLayout->animSet(), 8, 3, post->mHasYeah ? 1.0f : 0.0f);
    hdAnimSet_setFrame(mButton.mpLayout->animSet(), 9, 5, post->mIsSpoiler ? 1.0f : 0.0f);
}

/* 02640BAC */
void hdBtnMiiverseComment_c::initializeStatics() {
    enum { MAXIMUM_SAFE_LENGTH = 0x40000, FRIEND_ENTER_INDEX = 0x4B, YEAH_ENTER_INDEX = 0x4E };
    sinit_header_statics_z(gabi::ea(&sBtnMiiverseCommentHeaderPiPair), gabi::ea(&sBtnMiiverseCommentHeaderRegistration), gabi::ea(&sBtnMiiverseCommentHeaderZeroObject));
    sCommentAnimations.mEntries[0].mVtable = &sCommentStringVtable;
    sCommentAnimations.mEntries[0].mText.v = gabi::guest_addr("G_HeightSize_00");
    sCommentAnimations.mEntries[1].mVtable = &sCommentStringVtable;
    sCommentAnimations.mEntries[1].mText.v = gabi::guest_addr("G_Friend_00");
    sCommentAnimations.mEntries[2].mVtable = &sCommentStringVtable;
    sCommentGroups.mEntries[0].mVtable = &sCommentStringVtable;
    sCommentGroups.mEntries[0].mText.v = gabi::guest_addr("HeightSize");
    sCommentGroups.mEntries[1].mVtable = &sCommentStringVtable;
    sCommentGroups.mEntries[1].mText.v = gabi::guest_addr("Friend");
    sCommentGroups.mEntries[2].mVtable = &sCommentStringVtable;
    sCommentGroups.mEntries[2].mText.v = gabi::guest_addr("Yeah");
    sCommentAnimations.mEntries[2].mText.v = gabi::guest_addr("G_Btn_00");
    sCommentAnimations.mEntries[3].mVtable = &sCommentStringVtable;
    sCommentAnimations.mEntries[3].mText.v = gabi::guest_addr("G_Yeah_00");
    sCommentEmptyString.mBuffer[1] = 0;
    sCommentEmptyString.mpText = sCommentEmptyString.mBuffer;
    sCommentEmptyString.mCapacity = 2;
    sCommentEmptyString.mVtable = &sWideStringBaseVtable;
    sCommentAnimations.mEntries[4].mVtable = &sCommentStringVtable;
    sCommentAnimations.mEntries[4].mText.v = gabi::guest_addr("G_Spoiler_00");
    sCommentGroups.mEntries[3].mVtable = &sCommentStringVtable;
    sCommentGroups.mEntries[3].mText.v = gabi::guest_addr("Spoiler");
    hdBreakString_c empty;
    empty.mText = sWideEmptyLiteral.mText;
    empty.mVtable = &sWideSafeStringVtable;
    assureMiiverseWideString(&empty);
    s32 length = 0;
    u32 cursor = gabi::ea(empty.mText.get());
    while (*hdPartsView<be<u16>>(cursor) != 0) {
        ++length;
        cursor += sizeof(u16);
        if (length > MAXIMUM_SAFE_LENGTH) { length = 0; break; }
    }
    s32 capacity = sCommentEmptyString.mCapacity;
    if (length >= capacity) length = capacity - 1;
    empty.assureTermination();
    copyMiiverseWideCharacters(sCommentEmptyString.mBuffer, empty.mText.get(), u32(length) * sizeof(u16), 0);
    *hdPartsView<be<u16>>(gabi::ea(sCommentEmptyString.mBuffer) + u32(length) * sizeof(u16)) = 0;
    sCommentEmptyString.mVtable = &sWideStringVtable;
    __register_global_object(gabi::ea(&sBtnMiiverseCommentEmptyStringRegistration));
    u32 ready = sCommentStateCounterReady;
    sCommentResource.mVtable = &sCommentStringVtable;
    sCommentResource.mText.v = gabi::guest_addr("BtnMiiverseComment_00");
    u32 number;
    if (ready) number = sCommentStateCounter + 1;
    else { number = 1; sCommentStateCounterReady = 1; }
    sFriendState.setup(number, "::StateID_Friend", &sCommentStateVtable, FRIEND_ENTER_INDEX, &sCommentNullState);
    sCommentStateCounter = number + 1;
    sYeahState.setup(number + 1, "::StateID_Yeah", &sCommentStateVtable, YEAH_ENTER_INDEX, &sCommentNullState);
}

/* WWHD glue
 data sBtnMiiverseCommentPaneNamesRegistration 101F5664 nocheck
 data sBtnMiiverseCommentIconNamesRegistration 101F567C nocheck
 data sBtnMiiverseCommentWindowNamesRegistration 101F5688 nocheck
 data sBtnMiiverseCommentTextNamesRegistration 101F5670 nocheck
 data sBtnMiiverseCommentHeaderPiPair 104907DC nocheck
 data sBtnMiiverseCommentHeaderRegistration 101F5698 nocheck
 data sBtnMiiverseCommentHeaderZeroObject 10490858 nocheck
 data sBtnMiiverseCommentEmptyStringRegistration 101F56BC nocheck
 ext 02640FC8 void assureMiiverseFixedWideString(hdBreakBuffer_c*)
 ext 025F8CDC void resolveMiiverseTopic(void*, hdBreakBuffer_c*, be<u16>*)
 ext 025F6B18 void prepareMiiverseMessage(PaneWork_l*, u32, u32)
 ext 02706A68 u32 hdTextBreak_wrap(hdBreakBuffer_c*, hdBreakString_c*, hdBreakMeasure_c*, hdBreakOptions_c*)
 data sMiiNameBaseVtable 100EAE10
 data sMiiNameVtable 100EAE28
 data sTopicVtable 100EAE58
 data sCommentBaseVtable 100EAE70
 data sCommentFixedVtable 100EAE88
 data sScratchBaseVtable 100EAEE0
 data sScratchVtable 100EAEF8
 data sMiiverseMessageManager 101F4B5C
 data sMiiverseWideFallback 104A0CE0
 data sMiiverseFallbackCharacter 10000162
 str 100EB190 "P_DummyTex_00" in 0263F4F0

 data sCommentEmptyString 104908B0
 data sWideStringBaseVtable 100EAEA0
 data sWideStringVtable 100EAEB8
 data sWideSafeStringVtable 100EADF8
 data sWideEmptyLiteral 100EADDC
 data sCommentStateCounterReady 101FD8E4
 data sCommentStateCounter 101FDD50
 data sFriendState 10490918
 data sYeahState 10490948
 data sCommentStateVtable 100EAFA8
 data sCommentNullState 101FF32C
 ext 02640FC4 void assureMiiverseWideString(hdBreakString_c*)
 ext C0009988 void copyMiiverseWideCharacters(void*, void*, u32, u32)
 str 100EB1C0 "G_HeightSize_00" in 02640BAC
 str 100EB1D0 "G_Friend_00" in 02640BAC
 str 100EB200 "G_Btn_00" in 02640BAC
 str 100EB1DC "G_Yeah_00" in 02640BAC
 str 100EB20C "G_Spoiler_00" in 02640BAC
 str 100EB21C "HeightSize" in 02640BAC
 str 100EB1B0 "Friend" in 02640BAC
 str 100EB1B8 "Yeah" in 02640BAC
 str 100EB1A8 "Spoiler" in 02640BAC
 str 100EB1E8 "BtnMiiverseComment_00" in 02640BAC
 str 100EB228 "::StateID_Friend" in 02640BAC
 str 100EB23C "::StateID_Yeah" in 02640BAC

 data sCommentUiSystem 1018C404
 data sCommentAnimations 10490868
 data sCommentGroups 10490890
 data sCommentAnimationRemap 100EAF50
 data sCommentResource 10490978
 data sEmptyComment 100EB168
 ext 02878098 PaneFont_l* getMiiversePaneFont(hdTextPane_c*)
 ext 0286E2E4 void setMiiverseWriterFontSize(hdMiiverseTextWriter_l*, f32, f32)
 ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
 data sCommentTextNamesReady 1049090C
 data sTextTypeReady 101FDD40
 data sTextBaseTypeReady 101FD8F0
 data sPaneTypeReady 101FD89C
 data sRootTypeReady 101FD888
 data sTextType 101FDD3C
 data sTextBaseType 101FD8F4
 data sPaneType 101FD8F8
 data sRootType 101FD8FC
 data sPictureTypeReady 101FD938
 data sPictureBaseTypeReady 101FD88C
 data sPictureType 101FD93C
 data sPictureBaseType 101FD940
 data sCommentPictureName 101F5694
 str 100EB050 "T_TopicTag_00"
 str 100EB060 "T_MiiName_00"
 str 100EB070 "T_MiiverseComment_00"
 str 100EB040 "T_NumComment_00"
 str 100EB088 "T_NumYeah_00"

 str 100EB098 "P_DummyTex_00" in 0263EA2C
 str 100EB0B8 "MiiverseTagIcon_01"
 str 100EB0A8 "P_YeahIcon_01"
 str 100EB0CC "P_CommentIcon_00"
 data sCommentIconsNamesGuard 10490910
 str 100EB0E0 "W_BaseSh_00"
 str 100EB110 "W_Base_00"
 str 100EB140 "W_MiiNameBase_00"
 str 100EB0EC "W_BaseSh_01"
 str 100EB11C "W_Base_01"
 str 100EB0F8 "W_BaseSh_02"
 str 100EB128 "W_Base_02"
 str 100EB104 "W_BaseSh_03"
 str 100EB134 "W_Base_03"
 data sCommentWindowsNamesGuard 10490914

 data sCommentPaneNames 104907E8
 data sMiiversePosts 1018F4AC
 data sMiiverseSaveRoot 101F84DC
 data sCountStringVtable 100EAF38
 data sCommentCountFormat 100EB17C
 data sYeahCountFormat 100EB184
 ext 02759D3C void formatMiiverseCount(hdMiiverseCountString_c*, be<u16>*, s32)
 ext 027046D4 void setMiiverseText(hdPartsPane_c*, void*, void*)
 ext 027200F4 hdSavePair_c* hdSaveHolders_c::getScratchPair()
 data sCommentPaneNamesGuard 10490908
 data sCommentStringVtable 100EADE0
 data sCommentHolderVtable 100EB4DC
 data sCommentVtable 100EB254
 data sCommentDelegateVtable 100EAED0
 ext 0286FE50 void constructMiiverseTextWriter(void*)
 ext 028706A0 f32 measureMiiverseText(void*, be<u16>*, u32)
 data sCommentSecondaryVtable 100EB4EC
 str 100EB004 "N_All_00"
 str 100EB010 "N_BtnYeah_00"
 str 100EAFF4 "N_BtnPicture_00"
 str 100EB020 "N_YeahComment_00"
 str 100EB034 "C_Btn_00"
 ext 0286FED0 void destroyMiiverseTextWriter(void*, s32)
 data sCommentTextNames 10490810
 data sCommentIconNames 10490838
 data sCommentWindowNames 104908C0
 ext 028F0164 void destroyMiiverseStringArray(void*, u32, u32, u32, u32, u32)
*/
