/** WindowPart layouts and named character-animation configurations. */
#include "d/hd_parts_window.h"
void hdWindow_getPaneVector(hdWindowVector_l*, hdWindowPane_l*);
static hdWindowTreeNode_l* hdWindow_findConfig(hdWindowPart_c* part, hdUiSafeString* name);
void hdWindow_destroyArray(void*);
hdWindowTreeNode_l* hdWindow_findTree(hdWindowTree_l*, hdWindowTreeNode_l*, hdUiSafeString*);
void hdWindow_setConfig(hdWindowPart_c*, hdWindowConfig_l*);
void hdWindow_begin(hdWindowPart_c*);
void hdWindow_measurePane(hdWindowPane_l*, hdWindowMetrics_l*);
void hdWindow_measureCharacter(hdWindowPane_l*, hdWindowMetrics_l*);
u32 hdWindow_random(void*);
static void* sWindowRandom;
/* 026F2FD4 */
static be<u32>* hdWindow_getObject(gptr<be<u32>>* object) {
    return *object;
}
/* 026F30D4 */
void hdWindowConfig_l::destroy(hdWindowConfig_l* config, u32 flags) {
    if (!config)
        return;
    if (config->mpAnimators)
        hdWindow_destroyArray(&config->mAnimatorCount);
    if (flags & 1)
        operator_delete(config);
}
/* 026F3130 */
static void hdWindow_deleteConfig(void*, hdUiSafeString*, hdWindowConfig_l* config) {
    if (config)
        hdWindowConfig_l::destroy(config, 3);
}
/* 026F3140 */
static hdWindowSettings_l* hdWindowSettings_l__ct(hdWindowSettings_l* settings) {
    if (!settings) {
        settings = static_cast<hdWindowSettings_l*>(operator_new(sizeof(hdWindowSettings_l)));
        if (!settings)
            return NULL;
    }
    settings->mFixedWidth = 0;
    settings->mLeft = 0;
    settings->mFontScale = 0;
    settings->mMaximumJitter = 50.0f;
    settings->mWidth = 30.0f;
    settings->mRandomize = 0;
    settings->mCenter = 0.5f;
    settings->mAlignment = 0;
    settings->mMinimumJitter = 5.0f;
    settings->mRight = 0;
    settings->mSpacing = 5.0f;
    return settings;
}
/* 026F35AC */
void hdWindowPart_c::readMetrics(hdWindowPane_l* pane) {
    mSettings.mAlignment = u8(mpTextPane->mAlignment) & 3;
    mSettings.mFontScale = mpTextPane->mFontScale;
    hdWindowMetrics_l metrics;
    hdWindow_measurePane(pane, &metrics);
    f32 top = metrics.mTop, bottom = metrics.mBottom;
    f32 center = (top + bottom) * 0.5f;
    mSettings.mLeft = metrics.mLeft;
    mSettings.mRight = metrics.mRight;
    mSettings.mCenter = center;
}
/* 026F3D38 */
f32 hdWindowPart_c::characterWidth(hdWindowPane_l* pane) {
    if (mSettings.mFixedWidth)
        return mSettings.mWidth;
    hdWindowMetrics_l metrics;
    hdWindow_measureCharacter(pane, &metrics);
    return f32(metrics.mRight) - f32(metrics.mLeft);
}
/* 026F3D90 */
f32 hdWindowPart_c::lineWidth(s32 first, s32 count) {
    f32 width = 0;
    s32 end = s32(u32(first) + u32(count));
    if (first < end) {
        u32 remaining = count;
        do {
            hdWindowPane_l* character = NULL;
            if (u32(first) < mCharacters.mCount)
                character = mCharacters.mpItems[u32(first)];
            f32 value = characterWidth(character);
            f32 spacing = mSettings.mSpacing;
            width = width + (value + spacing);
            first = s32(u32(first) + 1);
        } while (--remaining);
    }
    if (count > 0)
        width = width - f32(mSettings.mSpacing);
    return width;
}
/* 026F3E44 */
f32 hdWindowPart_c::lineStart(f32 width) {
    switch (static_cast<u32>(mSettings.mAlignment)) {
    case 0: {
        f32 left = mSettings.mLeft, right = mSettings.mRight;
        return gabi::fmadds((right - left) - width, 0.5f, left);
    }
    case 1:
        return mSettings.mLeft;
    case 2:
        return f32(mSettings.mRight) - width;
    default:
        return 0;
    }
}
/* 026F3EA0 */
f32 hdWindowPart_c::randomize(f32 value) {
    if (!mSettings.mRandomize)
        return value;
    u32 random = hdWindow_random(sWindowRandom);
    f32 fraction = f32(random) * 2.3283064365386963e-10f;
    f32 minimum = mSettings.mMinimumJitter, maximum = mSettings.mMaximumJitter;
    return gabi::fmadds(fraction, maximum - minimum, value + minimum);
}
static hdWindowTreeNode_l* hdWindow_findConfig(hdWindowPart_c* part, hdUiSafeString* name) {
    return hdWindow_findTree(&part->mConfigs, part->mConfigs.mpRoot, name);
}
/* 026F5258 */
void hdWindowPart_c::selectConfig(hdUiSafeString* name) {
    hdWindowTreeNode_l* node = hdWindow_findConfig(this, name);
    if (node && u32(gabi::ea(node) + offsetof(hdWindowTreeNode_l, mpConfig))) {
        hdWindow_setConfig(this, node->mpConfig);
        hdWindow_begin(this);
    }
}
/* 026F52B4 */
void hdWindowPart_c::stop() {
    hdWindowConfig_l* config = mpConfig;
    if (config) {
        u32 count = config->mAnimatorCount;
        gptr<hdWindowAnimator_l>* items = config->mpAnimators;
        for (u32 i = 0; i < count; ++i) {
            hdWindowAnimator_l* animator = items[i];
            gabi::call_ptr<void>(animator->mVtable->mSetEnabled.get(), animator, 0);
        }
    }
    mpCurrentAnimator = NULL;
    mpConfig = NULL;
    mScheduling = 0;
}
/* 026F5344 */
void hdWindowPart_c::reset(hdUiSafeString* name) {
    selectConfig(name);
    hdWindowPane_l* pane = mpPane;
    gabi::call_ptr<void>(pane->mVtable->mUpdate.get(), pane);
    pane = mpPane;
    gabi::call_ptr<void>(pane->mVtable->mSetTransform.get(), pane, &mTransform, 0);
    stop();
}
/* 026F5410 */
BOOL hdWindowPart_c::isAnimatorFlagged(hdUiSafeString* name, u32 index) {
    hdWindowTreeNode_l* node = hdWindow_findConfig(this, name);
    if (!node || !u32(gabi::ea(node) + offsetof(hdWindowTreeNode_l, mpConfig)))
        return FALSE;
    hdWindowConfig_l* config = node->mpConfig;
    hdWindowAnimator_l* animator = NULL;
    if (index < config->mAnimatorCount)
        animator = config->mpAnimators[index];
    // Failed array lookup deliberately reads the flag through guest page zero.
    return (u32(hdPartsView(animator)->mFlags) & 1) != 0;
}
/* 026F549C */
BOOL hdWindowPart_c::isLastAnimatorFlagged(hdUiSafeString* name) {
    u32 count = mpTextPane->mCharacterCount;
    hdWindowTreeNode_l* node = hdWindow_findConfig(this, name);
    if (!node || !u32(gabi::ea(node) + offsetof(hdWindowTreeNode_l, mpConfig)))
        return FALSE;
    hdWindowConfig_l* config = node->mpConfig;
    hdWindowAnimator_l* animator = NULL;
    if (u32(count - 1) < config->mAnimatorCount)
        animator = config->mpAnimators[count - 1];
    // Failed array lookup deliberately reads the flag through guest page zero.
    return (u32(hdPartsView(animator)->mFlags) & 1) != 0;
}
/* 026F5534 */
BOOL hdWindowPart_c::hasRunningAnimator(hdUiSafeString* name) {
    hdWindowTreeNode_l* node = hdWindow_findConfig(this, name);
    if (!node || !u32(gabi::ea(node) + offsetof(hdWindowTreeNode_l, mpConfig)))
        return FALSE;
    hdWindowConfig_l* config = node->mpConfig;
    u32 count = hdPartsView(config)->mAnimatorCount;
    gptr<hdWindowAnimator_l>* item = hdPartsView(config)->mpAnimators;
    u32 endAddress = u32(gabi::ea(item) + u32(count * sizeof(*item)));
    while (gabi::ea(item) != endAddress) {
        hdWindowAnimator_l* animator = *hdPartsView(item);
        if (hdPartsView(animator)->mProgress != 0.0f)
            return TRUE;
        item = gabi::at<gptr<hdWindowAnimator_l>>(u32(gabi::ea(item) + sizeof(*item)));
    }
    return FALSE;
}
/* 026F55DC */
void hdWindowPart_c::copyConfig(hdUiSafeString* name, hdWindowConfig_l* source) {
    hdWindowTreeNode_l* node = hdWindow_findConfig(this, name);
    if (!node || !u32(gabi::ea(node) + offsetof(hdWindowTreeNode_l, mpConfig)))
        return;
    node->mpConfig->mDuration = source->mDuration;
    node->mpConfig->mExtraItems = source->mExtraItems;
    node->mpConfig->mOrder = source->mOrder;
    node->mpConfig->mMode = source->mMode;
    node->mpConfig->mResetLayout = source->mResetLayout;
    node->mpConfig->mFinishEnabled = source->mFinishEnabled;
}
u32 hdWindow_frameCount(hdWindowAnimator_l*);
void hdWindow_resetLayout(hdWindowPart_c*);
f32 hdWindow_randomFloat(f32);
static s32 divideWindow(s32 numerator, s32 denominator) {
    if (!denominator || (numerator == s32(0x80000000u) && denominator == -1))
        return numerator < 0 ? -1 : 0;
    return numerator / denominator;
}
/* 026F47D0 */
void hdWindowPart_c::updateItem(s32 index) {
    if (index < 0 || index >= mItemCount)
        return;
    hdWindowConfig_l* config = mpConfig;
    hdWindowAnimator_l* animator = NULL;
    if (u32(index) < config->mAnimatorCount)
        animator = config->mpAnimators[u32(index)];
    if (animator->mProgress == 0.0f) {
        u32 frames = hdWindow_frameCount(animator);
        f32 frame = animator->mFrame;
        if (frame != f32(frames))
            gabi::call_ptr<void>(animator->mVtable->mPlay.get(), animator, 1.0f);
    }
    mpCurrentAnimator = animator;
}
/* 026F4E48 */
s32 hdWindowPart_c::intervalDivisor() {
    hdWindowConfig_l* config = mpConfig;
    u32 mode = config->mMode;
    s32 result = 1;
    if (mode >= 1 && mode <= 2) {
        u32 remaining = mRemaining, total = 0;
        while (remaining) {
            total += remaining * remaining;
            --remaining;
        }
        s32 duration = config->mDuration;
        if (duration <= 0)
            duration = 1;
        result = divideWindow(s32(total), duration);
        if (result <= 0)
            result = 1;
    }
    return result;
}
/* 026F4EA8 */
void hdWindowPart_c::setConfig(hdWindowConfig_l* config) {
    hdWindowConfig_l* previous = mpConfig;
    if (previous) {
        u32 count = previous->mAnimatorCount;
        gptr<hdWindowAnimator_l>* items = previous->mpAnimators;
        for (u32 i = 0; i < count; ++i) {
            hdWindowAnimator_l* animator = items[i];
            gabi::call_ptr<void>(animator->mVtable->mSetEnabled.get(), animator, 0);
        }
    }
    s32 count = mItemCount;
    mpConfig = config;
    mRemaining = count;
    mTime = 0;
    mDelay = 0;
    s32 divisor = intervalDivisor();
    mScheduling = 1;
    mIntervalDivisor = divisor;
    mpCurrentAnimator = NULL;
    if (mpConfig->mResetLayout)
        hdWindow_resetLayout(this);
    hdWindowConfig_l* current = mpConfig;
    u32 animatorCount = current->mAnimatorCount;
    gptr<hdWindowAnimator_l>* items = current->mpAnimators;
    for (u32 i = 0; i < animatorCount; ++i) {
        hdWindowAnimator_l* animator = items[i];
        gabi::call_ptr<void>(animator->mVtable->mSetEnabled.get(), animator, 1);
        animator = items[i];
        gabi::call_ptr<void>(animator->mVtable->mFinish.get(), animator);
    }
}
/* 026F50AC */
void hdWindowPart_c::shuffleOrder() {
    s32 count = mItemCount;
    if (!count || !mpConfig || mpConfig->mOrder != WINDOW_ITEMS_CONFIGURED)
        return;
    for (s32 i = 0; i < mItemCount; ++i)
        mpOrder[u32(i)] = -1;
    s32 visible = 0;
    for (s32 i = 0; i < mItemCount; ++i) {
        hdWindowPane_l* pane = NULL;
        if (u32(i) < mCharacters.mCount)
            pane = mCharacters.mpItems[u32(i)];
        be<u16>* text = hdPartsView(pane)->mText;
        if (*text != 0x20) {
            mpOrder[u32(visible)] = i;
            ++visible;
        }
    }
    for (s32 pass = 0; pass < s32(u32(mItemCount) * 10); ++pass) {
        s32 first = gabi::ftoi(hdWindow_randomFloat(f32(visible)));
        if (first >= visible)
            first = s32(u32(visible) - 1);
        s32 second = gabi::ftoi(hdWindow_randomFloat(f32(visible)));
        if (second >= visible)
            second = s32(u32(visible) - 1);
        s32 a = mpOrder[u32(first)], b = mpOrder[u32(second)];
        mpOrder[u32(first)] = b;
        mpOrder[u32(second)] = a;
    }
}
static hdHeartInitLists_l sWindowInitLists;
static hdHeartInitAngles_l sWindowInitAngles;
static hdHeartAngleLimits_l sWindowAngleLimits;
static hdHeartInitRecord_l sWindowFirstRecord;
static hdHeartInitRecord_l sWindowSecondRecord;
static hdHeartInitRecord_l sWindowThirdRecord;
void hdWindow_registerStatic(hdHeartInitRecord_l*);
void hdWindow_initFlag(be<u8>*);
void hdWindow_initByte(be<u8>*);
/* 026F5668 */
static void hdWindow_staticInit() {
    sWindowInitLists.mThird = 0;
    sWindowInitLists.mFirst = 0;
    sWindowInitLists.mFourth = 0;
    sWindowInitLists.mSecond = 0;
    hdWindow_registerStatic(&sWindowFirstRecord);
    f32 minimum = sWindowAngleLimits.mMinimum, maximum = sWindowAngleLimits.mMaximum;
    sWindowInitAngles.mMinimum = minimum;
    sWindowInitAngles.mMaximum = maximum;
    hdWindow_initFlag(&sWindowInitAngles.mFlag);
    hdWindow_registerStatic(&sWindowSecondRecord);
    hdWindow_initByte(&sWindowInitAngles.mByte);
    hdWindow_registerStatic(&sWindowThirdRecord);
}
struct hdWindowMemberDescriptor_l {
    be<u32> mFirst, mSecond;
};
struct hdWindowDelegate_l {
    gptr<void> mVtable, mpReceiver;
    hdWindowMemberDescriptor_l mMethod;
};
struct hdWindowDeleteContext_l {
    gptr<hdWindowPart_c> mpPart;
};
struct hdWindowDeleteReceiver_l {
    gptr<hdWindowDeleteContext_l> mpContext;
};
static hdWindowMemberDescriptor_l sWindowDeleteMethod;
static hdWindowMemberDescriptor_l sWindowNoopMethod;
static u32 sWindowVtable;
void hdWindow_constructDeleteDelegate(hdWindowDelegate_l*, hdWindowDeleteReceiver_l*,
                                      hdWindowMemberDescriptor_l*);
void hdWindow_constructTreeDelegate(hdWindowDelegate_l*, hdWindowTree_l*, hdWindowMemberDescriptor_l*);
void hdWindow_traverseDelete(hdWindowTreeNode_l*, hdWindowDelegate_l*);
void hdWindow_traverseTree(hdWindowTreeNode_l*, hdWindowDelegate_l*);
void hdWindow_constructTransform(void*);
void hdWindow_destroyTransform(void*, u32);
void hdWindow_freePane(hdWindowPane_l*);
static void* sWindowHeapManager;
hdScreenHeap* hdWindow_getHeap(void*, void*);
static void clearWindowArray(hdWindowPaneArray_l* array) {
    if (!array)
        array = static_cast<hdWindowPaneArray_l*>(operator_new(sizeof(hdWindowPaneArray_l)));
    if (array) {
        array->mCapacity = 0;
        array->mCount = 0;
        array->mpItems = NULL;
    }
}
/* 026F31CC */
static hdWindowPart_c* hdWindowPart_c__ct(hdWindowPart_c* part, f32 spacing) {
    if (!part) {
        part = static_cast<hdWindowPart_c*>(operator_new(sizeof(hdWindowPart_c)));
        if (!part)
            return NULL;
    }
    part->mpTextPane = NULL;
    part->mVtable = &sWindowVtable;
    part->mpResource = NULL;
    part->mpPane = NULL;
    part->mpRoot = NULL;
    struct Buffer_l {
        be<u32> mType;
        gptr<void> mpData;
    };
    Buffer_l* buffer = gabi::at<Buffer_l>(u32(gabi::ea(part) + offsetof(hdWindowPart_c, mLineCount)));
    if (!buffer)
        buffer = static_cast<Buffer_l*>(operator_new(sizeof(Buffer_l)));
    if (buffer) {
        buffer->mpData = NULL;
        buffer->mType = 0;
    }
    clearWindowArray(gabi::at<hdWindowPaneArray_l>(u32(gabi::ea(part) + offsetof(hdWindowPart_c, mPanes))));
    clearWindowArray(
        gabi::at<hdWindowPaneArray_l>(u32(gabi::ea(part) + offsetof(hdWindowPart_c, mCharacters))));
    hdWindowSettings_l__ct(
        gabi::at<hdWindowSettings_l>(u32(gabi::ea(part) + offsetof(hdWindowPart_c, mSettings))));
    hdWindowTree_l* tree = gabi::at<hdWindowTree_l>(u32(gabi::ea(part) + offsetof(hdWindowPart_c, mConfigs)));
    if (!tree)
        tree = static_cast<hdWindowTree_l*>(operator_new(sizeof(hdWindowTree_l)));
    if (tree) {
        tree->mpRoot = NULL;
        tree->mCount = 0;
        tree->mCapacity = 32;
        hdWindowTreeNode_l* first = &tree->mNodes[0];
        tree->mpFreeHead = first;
        for (u32 i = 0; i < 31; ++i)
            tree->mNodes[i].mpLeft = &tree->mNodes[i + 1];
        tree->mNodes[31].mpLeft = NULL;
        tree->mpNodes = first;
    }
    part->mpOrder = NULL;
    part->mRemaining = 0;
    part->mItemCount = 0;
    part->mpCurrentAnimator = NULL;
    part->mDelay = 0;
    part->mScheduling = 0;
    part->mIntervalDivisor = 1;
    part->mpConfig = NULL;
    part->mTime = 0;
    hdWindow_constructTransform(&part->mTransform);
    part->mTransformFlags = 0;
    part->mSettings.mSpacing = spacing;
    part->mReserved = 0;
    return part;
}
struct hdWindowClearLocals_l {
    hdWindowDeleteContext_l mContext;
    hdWindowDeleteReceiver_l mReceiver;
    hdWindowMemberDescriptor_l mDeleteMethod, mNoopMethod;
    hdWindowDelegate_l mDeleteDelegate, mTreeDelegate;
};
WWHD_OFFSET(hdWindowClearLocals_l, mReceiver, 4);
WWHD_OFFSET(hdWindowClearLocals_l, mDeleteMethod, 8);
WWHD_OFFSET(hdWindowClearLocals_l, mNoopMethod, 0x10);
WWHD_OFFSET(hdWindowClearLocals_l, mDeleteDelegate, 0x18);
WWHD_OFFSET(hdWindowClearLocals_l, mTreeDelegate, 0x28);
/* 026F335C */
void hdWindowPart_c::clearConfigs() {
    // The delegate's receiver points four bytes back to its enclosing context.
    hdWindowClearLocals_l locals;
    locals.mContext.mpPart = this;
    locals.mReceiver.mpContext = &locals.mContext;
    locals.mDeleteMethod.mFirst = sWindowDeleteMethod.mFirst;
    locals.mDeleteMethod.mSecond = sWindowDeleteMethod.mSecond;
    hdWindow_constructDeleteDelegate(&locals.mDeleteDelegate, &locals.mReceiver, &locals.mDeleteMethod);
    hdWindowTreeNode_l* root = mConfigs.mpRoot;
    if (root)
        hdWindow_traverseDelete(root, &locals.mDeleteDelegate);
    locals.mNoopMethod.mFirst = sWindowNoopMethod.mFirst;
    locals.mNoopMethod.mSecond = sWindowNoopMethod.mSecond;
    hdWindow_constructTreeDelegate(&locals.mTreeDelegate, &mConfigs, &locals.mNoopMethod);
    root = mConfigs.mpRoot;
    if (root)
        hdWindow_traverseTree(root, &locals.mTreeDelegate);
    u32 capacity = mConfigs.mCapacity;
    hdWindowTreeNode_l* nodes = mConfigs.mpNodes;
    mConfigs.mCount = 0;
    mConfigs.mpFreeHead = nodes;
    mConfigs.mpRoot = NULL;
    s32 remaining = s32(capacity - 1);
    if (remaining > 0) {
        u32 address = gabi::ea(nodes);
        do {
            hdWindowTreeNode_l* node = gabi::at<hdWindowTreeNode_l>(address);
            address += sizeof(hdWindowTreeNode_l);
            hdPartsView(node)->mpLeft.v = address;
        } while (--remaining);
    }
    hdWindowTreeNode_l* last = gabi::at<hdWindowTreeNode_l>(
        gabi::ea(nodes) + capacity * sizeof(hdWindowTreeNode_l) - sizeof(hdWindowTreeNode_l));
    hdPartsView(last)->mpLeft = NULL;
    mConfigs.mpNodes = nodes;
}
/* 026F344C */
void hdWindowPart_c::clear() {
    if (mpOrder) {
        operator_delete(mpOrder);
        mpOrder = NULL;
    }
    s32 remaining = mPanes.mCount;
    if (remaining > 0) {
        u32 index = 0;
        do {
            if (index < mPanes.mCount) {
                hdWindowPane_l* pane = mPanes.mpItems[index];
                if (pane) {
                    gabi::call_ptr<void>(pane->mVtable->mDestroy.get(), pane, 2);
                    hdWindow_freePane(pane);
                }
            }
            ++index;
        } while (--remaining);
    }
    hdWindow_destroyArray(&mPanes);
    if (mpLineLengths) {
        hdScreenHeap* heap = hdWindow_getHeap(sWindowHeapManager, static_cast<void*>(mpLineLengths));
        gabi::call_ptr<void>(heap->mVtable->mFree.get(), heap, static_cast<void*>(mpLineLengths));
        mLineCount = 0;
        mpLineLengths = NULL;
    }
    hdWindow_destroyArray(&mCharacters);
    clearConfigs();
}
/* 026F3544 */
void hdWindowPart_c::destroy(hdWindowPart_c* part, u32 flags) {
    if (!part)
        return;
    part->mVtable = &sWindowVtable;
    part->clear();
    hdWindow_destroyTransform(&part->mTransform, 2);
    if (flags & 1)
        operator_delete(part);
}
static u32 sWindowBufferVtable;
static u32 sWindowStringVtable;
static u32 sWindowConfigNameVtable;
/* 026F42F0 */
static hdWindowConfig_l* hdWindow_baseConfigConstruct(hdWindowConfig_l* config) {
    if (!config) {
        config = static_cast<hdWindowConfig_l*>(operator_new(0x60));
        if (!config)
            return NULL;
    }
    config->mName.mText = &config->mName.mBuffer[0];
    config->mName.mVtable.v = gabi::ea(&sWindowBufferVtable);
    config->mName.mCapacity = 64;
    config->mName.mBuffer[63] = 0;
    config->mName.mVtable.v = gabi::ea(&sWindowStringVtable);
    *config->mName.mText = 0;
    config->mName.mVtable.v = gabi::ea(&sWindowConfigNameVtable);
    config->mExtraItems = 0;
    config->mMode = 0;
    config->mDuration = 60;
    config->mOrder = 0;
    config->mFinishEnabled = 1;
    config->mResetLayout = 1;
    *config->mName.mText = 0;
    return config;
}
/* 026F43DC */
static hdWindowConfig_l* hdWindow_configConstruct(hdWindowConfig_l* config) {
    if (!config) {
        config = static_cast<hdWindowConfig_l*>(operator_new(sizeof(hdWindowConfig_l)));
        if (!config)
            return NULL;
    }
    hdWindow_baseConfigConstruct(config);
    struct Animators_l {
        be<u32> mCount, mCapacity;
        gptr<gptr<hdWindowAnimator_l>> mpItems;
    };
    Animators_l* animators =
        gabi::at<Animators_l>(u32(gabi::ea(config) + offsetof(hdWindowConfig_l, mAnimatorCount)));
    if (!animators)
        animators = static_cast<Animators_l*>(operator_new(sizeof(Animators_l)));
    if (animators) {
        animators->mCount = 0;
        animators->mpItems = NULL;
        animators->mCapacity = 0;
    }
    return config;
}
/* 026F53A8 */
static void hdWindow_characterVector(hdWindowVector_l* output, hdWindowPart_c* part, u32 index) {
    hdWindowPane_l* pane = NULL;
    if (index < part->mCharacters.mCount)
        pane = part->mCharacters.mpItems[index];
    hdWindowVector_l vector;
    hdWindow_getPaneVector(&vector, pane);
    output->mX = vector.mX;
    output->mY = vector.mY;
    output->mZ = vector.mZ;
}
void hdWindow_countText(void*, be<u32>*, be<u32>*, be<u16>*, u32);
static s32 textLength16(be<u16>* text) {
    s32 count = 0;
    if (*text) {
        do {
            ++count;
            if (count > 0x40000)
                return 0;
            ++text;
        } while (*text);
    }
    return count;
}
static void assureWindowText(hdWindowText_l* text) {
    gabi::call_ptr<void>(text->mVtable->mAssureTermination.get(), text);
}
/* 026F3624 */
static u32 hdWindow_measureText(hdWindowPart_c* part, hdWindowText_l* text) {
    struct Count_l {
        be<u32> mValue;
    };
    Count_l total;
    total.mValue = 0;
    void* font = part->mpTextPane->mpFont;
    assureWindowText(text);
    be<u16>* characters = text->mText;
    assureWindowText(text);
    s32 length = textLength16(text->mText);
    hdWindow_countText(font, NULL, &total.mValue, characters, u32(length) * 2);
    return total.mValue;
}
static u32 sWindowDelimiterVtable;
static u32 sWindowIteratorVtable;
static u32 sWindowTokenVtable;
struct hdWindowToken_l {
    hdWindowText_l mText;
    be<u32> mCapacity;
    be<u16> mBuffer[256];
};
struct hdWindowTextIterator_l {
    gptr<hdWindowText_l> mText;
    be<s32> mIndex;
    gptr<void> mVtable;
};
struct hdWindowDelimiter_l {
    hdWindowText_l mText;
    be<u32> mCount;
    be<u16> mNewline, mTerminator;
};
WWHD_OFFSET(hdWindowDelimiter_l, mCount, 8);
WWHD_OFFSET(hdWindowDelimiter_l, mNewline, 0xC);
struct hdWindowTextCount_l {
    be<u32> mValue;
};
/* 026F36F4 */
static u32 hdWindow_splitText(hdWindowPart_c* part, hdWindowLineCounts_l* output, hdWindowText_l* text) {
    u32 line = 0, total = 0;
    s32 position = 0;
    assureWindowText(text);
    s32 end = textLength16(text->mText) + 1;
    hdWindowTextIterator_l iterator;
    iterator.mText = text;
    iterator.mIndex = 0;
    iterator.mVtable = &sWindowIteratorVtable;
    hdWindowDelimiter_l delimiter;
    delimiter.mNewline = 10;
    delimiter.mTerminator = 0;
    delimiter.mText.mText = &delimiter.mNewline;
    delimiter.mText.mVtable.v = gabi::ea(&sWindowDelimiterVtable);
    hdWindowToken_l token;
    token.mBuffer[0] = 0;
    token.mBuffer[255] = 0;
    token.mText.mText = &token.mBuffer[0];
    token.mText.mVtable.v = gabi::ea(&sWindowTokenVtable);
    token.mCapacity = 256;
    void* font = part->mpTextPane->mpFont;
    hdWindowText_l* current = text;
    while (current != text || position != end) {
        assureWindowText(text);
        s32 length = textLength16(text->mText);
        if (position < 0 || position > length) {
            delimiter.mCount = 0;
            assureWindowText(&token.mText);
            hdWindow_countText(font, &delimiter.mCount, NULL, token.mText.mText, 0);
        } else {
            assureWindowText(&token.mText);
            be<u16>* destination = token.mText.mText;
            u32 tokenLength = 0;
            for (;;) {
                current = iterator.mText;
                be<u16>* source = current->mText;
                u16 character = source[u32(position)];
                ++position;
                if (!character)
                    break;
                assureWindowText(&delimiter.mText);
                be<u16>* scan = delimiter.mText.mText;
                bool isDelimiter = false;
                for (u32 i = 0; i < 0x40001; ++i) {
                    u16 candidate = *scan;
                    if (!candidate)
                        break;
                    if (candidate == character) {
                        isDelimiter = true;
                        break;
                    }
                    ++scan;
                }
                if (isDelimiter)
                    break;
                *destination = character;
                ++destination;
                ++tokenLength;
            }
            *destination = 0;
            iterator.mIndex = position;
            delimiter.mCount = 0;
            assureWindowText(&token.mText);
            hdWindow_countText(font, &delimiter.mCount, NULL, token.mText.mText, tokenLength * 2);
        }
        be<u32>* counts = output->mpCounts;
        if (line < output->mCapacity)
            counts += line;
        *counts = delimiter.mCount;
        total += u32(delimiter.mCount);
        position = iterator.mIndex;
        ++line;
        current = iterator.mText;
    }
    return total;
}
hdScreenHeap* hdWindow_currentHeap(void*);
void hdWindow_allocateArray(void*, u32, void*, u32);
void hdWindow_createPanes(hdWindowPart_c*, hdWindowPaneArray_l*, hdWindowPaneArray_l*, void*);
u32 hdWindow_mapCharacters(void*, hdWindowPaneArray_l*, be<u16>*, u32);
void* hdWindow_allocateOrder(u32, void*, u32);
/* 026F3BE4 */
void hdWindowPart_c::createCharacters(void* heapArgument) {
    hdWindowText_l text;
    text.mText = mpTextPane->mText;
    text.mVtable.v = gabi::ea(&sWindowDelimiterVtable);
    s32 count = hdWindow_measureText(this, &text);
    if (count > 0) {
        hdScreenHeap* heap = static_cast<hdScreenHeap*>(heapArgument);
        if (!heap)
            heap = hdWindow_currentHeap(sWindowHeapManager);
        be<u32>* lines = gabi::call_ptr<be<u32>*>(heap->mVtable->mAllocate.get(), heap, u32(count) * 4, 4);
        if (lines) {
            mpLineLengths = lines;
            mLineCount = count;
        }
    }
    hdWindowLineCounts_l* lines =
        gabi::at<hdWindowLineCounts_l>(u32(gabi::ea(this) + offsetof(hdWindowPart_c, mLineCount)));
    u32 characters = hdWindow_splitText(this, lines, &text);
    hdWindow_allocateArray(&mPanes, characters, heapArgument, 4);
    hdWindow_allocateArray(&mCharacters, characters, heapArgument, 4);
    hdWindow_createPanes(this, &mCharacters, &mPanes, mpResource);
    hdWindowPane_l* pane = mpTextPane;
    u32 bytes = u16(pane->mCharacterCount) * 2;
    u32 mapped = hdWindow_mapCharacters(pane->mpFont, &mCharacters, pane->mText, bytes);
    mItemCount = mapped;
    if (mapped) {
        if (mpOrder) {
            operator_delete(mpOrder);
            mapped = mItemCount;
        }
        mpOrder = static_cast<be<s32>*>(hdWindow_allocateOrder(mapped * 4, heapArgument, 4));
    }
}
struct hdWindowPaneTransform_l {
    u8 mPrefix[0x1C];
    be<f32> mX, mY, mZ;
    u8 mStorage[0x44 - 0x28];
    be<u8> mFlags;
};
/* 026F3F60 */
void hdWindowPart_c::positionCharacters() {
    u32 count = mLineCount;
    be<u32>* line = mpLineLengths;
    u32 end = gabi::ea(line) + count * 4;
    u32 index = 0;
    f32 y = mSettings.mCenter;
    while (gabi::ea(line) != end) {
        f32 width = lineWidth(index, *line);
        f32 x = lineStart(width);
        f32 height = 0;
        s32 current = 0;
        while (current < s32(*line)) {
            hdWindowPane_l* character = NULL;
            if (index < mCharacters.mCount)
                character = mCharacters.mpItems[index];
            hdWindowMetrics_l bounds;
            hdWindow_measureCharacter(character, &bounds);
            f32 glyphHeight = std::fabs(f32(bounds.mBottom) - f32(bounds.mTop));
            f32 glyphWidth = std::fabs(f32(bounds.mRight) - f32(bounds.mLeft));
            f32 center = gabi::fmadds(glyphWidth, 0.5f, x);
            f32 position = randomize(y);
            hdWindowPaneTransform_l* transform = hdPartsView(character)->mpParentTransform;
            u8 flags = transform->mFlags;
            transform->mZ = 0;
            transform->mY = position;
            height = (glyphHeight - height) >= 0 ? glyphHeight : height;
            transform->mFlags = flags | 0x10;
            transform->mX = center;
            f32 advance = characterWidth(character);
            f32 spacing = mSettings.mSpacing;
            x = x + (advance + spacing);
            ++current;
            ++index;
        }
        f32 spacing = mSettings.mFontScale;
        ++line;
        y = y - (height + spacing);
    }
}
static s32 textLength8(be<u8>* text) {
    s32 count = 0;
    if (*text) {
        do {
            ++count;
            if (count > 0x40000)
                return 0;
            ++text;
        } while (*text);
    }
    return count;
}
static void assureWindowName(hdUiSafeString* name) {
    gabi::call_ptr<void>(name->mVtable->mAssureTermination.get(), name);
}
hdWindowAnimator_l* hdWindow_createAnimator(hdWindowPane_l*, be<u8>*, hdWindowPane_l*, u32);
void hdWindow_copyName(be<u8>*, be<u8>*, u32, u32);
void* hdWindow_allocate(u32, void*, u32);
hdWindowTreeNode_l* hdWindow_insertTree(hdWindowTree_l*, hdWindowTreeNode_l*, hdWindowTreeNode_l*);
static u32 sWindowNodeValueVtable;
static u32 sWindowNodeStringVtable;
/* 026F444C */
static void hdWindow_loadConfig(hdWindowConfig_l* config, hdWindowPane_l* pane,
                                hdWindowPaneArray_l* characters, hdUiSafeString* name, void* heap) {
    hdWindow_allocateArray(&config->mAnimatorCount, characters->mCount, heap, 4);
    u32 count = characters->mCount;
    gptr<hdWindowPane_l>* items = characters->mpItems;
    for (u32 i = 0; i < count; ++i) {
        assureWindowName(name);
        hdWindowAnimator_l* animator = hdWindow_createAnimator(pane, name->mText, items[i], 0);
        s32 size = config->mAnimatorCount, capacity = config->mAnimatorCapacity;
        if (size < capacity) {
            config->mpAnimators[u32(size)] = animator;
            ++config->mAnimatorCount;
        }
    }
    be<u8>* destination = config->mName.mText;
    assureWindowName(name);
    s32 length = textLength8(name->mText);
    s32 capacity = config->mName.mCapacity;
    if (length >= capacity)
        length = s32(u32(capacity) - 1);
    assureWindowName(name);
    hdWindow_copyName(destination, name->mText, length, 0);
    destination[u32(length)] = 0;
}
/* 026F459C */
hdWindowConfig_l* hdWindowPart_c::createConfig(hdUiSafeString* name, void* heap) {
    hdWindowTreeNode_l* node = hdWindow_findConfig(this, name);
    if (node && u32(gabi::ea(node) + offsetof(hdWindowTreeNode_l, mpConfig)))
        return node->mpConfig;
    hdWindowConfig_l* config =
        static_cast<hdWindowConfig_l*>(hdWindow_allocate(sizeof(hdWindowConfig_l), heap, 4));
    if (config)
        config = hdWindow_configConstruct(config);
    hdWindow_loadConfig(config, mpPane, &mPanes, name, heap);
    if (s32(mConfigs.mCount) < s32(mConfigs.mCapacity)) {
        node = mConfigs.mpFreeHead;
        if (node)
            mConfigs.mpFreeHead = node->mpLeft;
        if (node) {
            node->mpConfig = config;
            node->mpLeft = NULL;
            node->mpOwner = &mConfigs;
            node->mValueVtable = gabi::ea(&sWindowNodeValueVtable);
            node->mNameBuffer[64] = 0;
            node->mKey.mText.v = gabi::guest_addr("");
            node->mIsRed = 1;
            node->mKey.mVtable.v = gabi::ea(&sWindowNodeStringVtable);
            node->mpRight = NULL;
            assureWindowName(name);
            s32 length = textLength8(name->mText);
            if (length >= 65)
                length = 64;
            assureWindowName(name);
            hdWindow_copyName(&node->mNameBuffer[0], name->mText, length, 0);
            node->mNameBuffer[u32(length)] = 0;
            node->mKey.mText = &node->mNameBuffer[0];
        }
        ++mConfigs.mCount;
        mConfigs.mpRoot = hdWindow_insertTree(&mConfigs, mConfigs.mpRoot, node);
        mConfigs.mpRoot->mIsRed = 0;
    } else {
        node = hdWindow_findConfig(this, name);
        if (node && u32(gabi::ea(node) + offsetof(hdWindowTreeNode_l, mpConfig)))
            node->mpConfig = config;
    }
    return config;
}
struct hdWindowGlyph_l {
    u8 mPrefix[0xC];
    gptr<hdWindowPane_l> mpParent;
    u8 mStorage[0xB8 - 0x10];
    be<f32> mWidth, mHeight;
    u8 mToFlags[0xF4 - 0xC0];
    be<u8> mFlags;
    u8 mToSize[0x104 - 0xF5];
    be<f32> mResourceWidth, mResourceHeight;
    u8 mTail[0xB68 - 0x10C];
};
void* hdWindow_paneAllocate(u32, u32);
hdWindowPane_l* hdWindow_constructContainer(hdWindowPane_l*);
hdWindowPane_l* hdWindow_constructGroup(hdWindowPane_l*);
hdWindowGlyph_l* hdWindow_copyGlyph(hdWindowGlyph_l*, void*);
void hdWindow_destroyGlyph(hdWindowGlyph_l*, u32);
void* hdWindow_getFont(hdWindowPane_l*);
void hdWindow_setFont(hdWindowGlyph_l*, void*);
void hdWindow_attach(void*, void*);
void hdWindow_addPane(hdWindowPane_l*, hdWindowGlyph_l*);
/* 026F39BC */
static void hdWindow_createCharacterPanes(hdWindowPart_c* part, hdWindowPaneArray_l* characters,
                                          hdWindowPaneArray_l* containers, hdWindowGlyph_l* source) {
    while (s32(characters->mCount) < s32(characters->mCapacity)) {
        hdWindowPane_l* container = static_cast<hdWindowPane_l*>(hdWindow_paneAllocate(0x34, 4));
        if (container)
            container = hdWindow_constructContainer(container);
        hdWindowPane_l* group = static_cast<hdWindowPane_l*>(hdWindow_paneAllocate(0xA4, 4));
        if (group)
            group = hdWindow_constructGroup(group);
        hdWindowGlyph_l temporary;
        hdWindowGlyph_l* copied = hdWindow_copyGlyph(&temporary, source);
        hdWindowGlyph_l* glyph = static_cast<hdWindowGlyph_l*>(hdWindow_paneAllocate(0xB68, 4));
        if (glyph)
            glyph = hdWindow_copyGlyph(glyph, copied);
        hdWindow_destroyGlyph(&temporary, 0);
        bool same = glyph->mWidth == glyph->mResourceWidth && glyph->mHeight == glyph->mResourceHeight;
        u8 flags = glyph->mFlags;
        glyph->mFlags = flags | (!same ? 0x20 : 0);
        if (!same) {
            glyph->mWidth = glyph->mResourceWidth;
            glyph->mHeight = glyph->mResourceHeight;
        }
        void* font = hdWindow_getFont(part->mpTextPane);
        hdWindow_setFont(glyph, font);
        hdWindow_attach(source->mpParent, group);
        hdWindow_attach(group, glyph);
        hdWindow_addPane(container, glyph);
        s32 size = characters->mCount, capacity = characters->mCapacity;
        if (size < capacity) {
            characters->mpItems[u32(size)].v = gabi::ea(glyph);
            ++characters->mCount;
        }
        size = containers->mCount;
        capacity = containers->mCapacity;
        if (size < capacity) {
            containers->mpItems[u32(size)] = container;
            ++containers->mCount;
        }
    }
}
void hdWindow_createCharacters(hdWindowPart_c*, void*);
struct hdWindowResource_l {
    u8 mPrefix[0x24];
    u8 mPaneNames[0xC];
};
struct hdWindowTransformHeap_l {
    u8 mPrefix[0x14];
    gptr<void> mpTransformHeap;
};
WWHD_OFFSET(hdWindowTransformHeap_l, mpTransformHeap, 0x14);
struct hdWindowMatrix_l {
    be<u32> mElements[12];
};
static hdScreenUiSystem* sWindowPartHeapManager;
be<u8>* hdWindow_getResourceName(void*, u32);
void* hdWindow_getArchiveResource(void*);
void hdWindow_loadTransform(hdWindowTransform_l*, void*);
/* 026F4174 */
static void hdWindow_load(hdWindowPart_c* part, hdWindowResource_l* resource, hdPartsLyt_c* layout,
                          void* archive, hdWindowMatrix_l* matrix) {
    part->mpPane.v = gabi::ea(layout);
    hdButtonPaneVtable_l* rootVtable =
        reinterpret_cast<hdButtonPaneVtable_l*>(layout->mpRootPane->mVtable.get());
    be<u8>* name = hdWindow_getResourceName(resource->mPaneNames, 0);
    hdWindowPane_l* pane = gabi::call_ptr<hdWindowPane_l*>(
        rootVtable->mFindPane.get(), static_cast<hdPartsPane_c*>(layout->mpRootPane), name, 1);
    if (!pane)
        return;
    rootVtable = reinterpret_cast<hdButtonPaneVtable_l*>(layout->mpRootPane->mVtable.get());
    name = hdWindow_getResourceName(resource->mPaneNames, 0);
    part->mpTextPane = gabi::call_ptr<hdWindowPane_l*>(
        rootVtable->mFindPane.get(), static_cast<hdPartsPane_c*>(layout->mpRootPane), name, 1);
    if (!part->mpTextPane)
        return;
    rootVtable = reinterpret_cast<hdButtonPaneVtable_l*>(layout->mpRootPane->mVtable.get());
    name = hdWindow_getResourceName(resource->mPaneNames, 1);
    part->mpResource = gabi::call_ptr<void*>(rootVtable->mFindPane.get(),
                                             static_cast<hdPartsPane_c*>(layout->mpRootPane), name, 1);
    if (!part->mpResource)
        return;
    part->readMetrics(pane);
    hdWindow_createCharacters(part, sWindowPartHeapManager->mHeap);
    hdWindow_resetLayout(part);
    hdWindowPane_l* text = part->mpTextPane;
    text->mFlags = u8(text->mFlags) & ~1;
    hdWindowPane_l* glyph = static_cast<hdWindowPane_l*>(part->mpResource.get());
    glyph->mFlags = u8(glyph->mFlags) & ~1;
    part->mTransform.mpHeap =
        hdPartsView<hdWindowTransformHeap_l>(gabi::ea(sWindowPartHeapManager))->mpTransformHeap;
    void* data = hdWindow_getArchiveResource(archive);
    hdWindow_loadTransform(&part->mTransform, data);
    for (u32 i = 0; i < 12; ++i)
        part->mTransform.mMatrix[i] = matrix->mElements[i];
}
/* WWHD glue
 data sWindowPartHeapManager 1018C404
 ext 02873D24 be<u8>* hdWindow_getResourceName(void*,u32)
 ext 0274D83C void* hdWindow_getArchiveResource(void*)
 ext 02873FE4 void hdWindow_loadTransform(hdWindowTransform_l*,void*)
 ext 026F3BE4 void hdWindow_createCharacters(hdWindowPart_c*,void*)

 ext 0287F3DC void* hdWindow_paneAllocate(u32,u32)
 ext 02874484 hdWindowPane_l* hdWindow_constructContainer(hdWindowPane_l*)
 ext 02875A10 hdWindowPane_l* hdWindow_constructGroup(hdWindowPane_l*)
 ext 0270324C hdWindowGlyph_l* hdWindow_copyGlyph(hdWindowGlyph_l*,void*)
 ext 020022E4 void hdWindow_destroyGlyph(hdWindowGlyph_l*,u32)
 ext 02878098 void* hdWindow_getFont(hdWindowPane_l*)
 ext 02877668 void hdWindow_setFont(hdWindowGlyph_l*,void*)
 ext 02876524 void hdWindow_attach(void*,void*)
 ext 02874524 void hdWindow_addPane(hdWindowPane_l*,hdWindowGlyph_l*)

 data sWindowNodeValueVtable 10109A5C
 data sWindowNodeStringVtable 10109974
 str 10000160 ""
 ext 0287F740 hdWindowAnimator_l* hdWindow_createAnimator(hdWindowPane_l*,be<u8>*,hdWindowPane_l*,u32)
 ext C0009988 void hdWindow_copyName(be<u8>*,be<u8>*,u32,u32)
 ext 0273B050 void* hdWindow_allocate(u32,void*,u32)
 ext 0261401C hdWindowTreeNode_l* hdWindow_insertTree(hdWindowTree_l*,hdWindowTreeNode_l*,hdWindowTreeNode_l*)

 ext 02756140 hdScreenHeap* hdWindow_currentHeap(void*)
 ext 0273B584 void hdWindow_allocateArray(void*,u32,void*,u32)
 ext 026F39BC void hdWindow_createPanes(hdWindowPart_c*,hdWindowPaneArray_l*,hdWindowPaneArray_l*,void*)
 ext 02602834 u32 hdWindow_mapCharacters(void*,hdWindowPaneArray_l*,be<u16>*,u32)
 ext 0273B0D4 void* hdWindow_allocateOrder(u32,void*,u32)

 data sWindowDelimiterVtable 101099C4
 data sWindowIteratorVtable 10109A4C
 data sWindowTokenVtable 10109A24

 ext 02602778 void hdWindow_countText(void*,be<u32>*,be<u32>*,be<u16>*,u32)

 data sWindowBufferVtable 1010998C
 data sWindowStringVtable 101099DC
 data sWindowConfigNameVtable 101099F4
 ext 02704D2C void hdWindow_getPaneVector(hdWindowVector_l*,hdWindowPane_l*)

 data sWindowDeleteMethod 10109968
 data sWindowNoopMethod 10109960
 data sWindowVtable 10109AD0
 data sWindowHeapManager 101F8B4C
 ext 026F591C void hdWindow_constructDeleteDelegate(hdWindowDelegate_l*,hdWindowDeleteReceiver_l*,hdWindowMemberDescriptor_l*)
 ext 026F5988 void hdWindow_constructTreeDelegate(hdWindowDelegate_l*,hdWindowTree_l*,hdWindowMemberDescriptor_l*)
 ext 026F586C void hdWindow_traverseDelete(hdWindowTreeNode_l*,hdWindowDelegate_l*)
 ext 026F57BC void hdWindow_traverseTree(hdWindowTreeNode_l*,hdWindowDelegate_l*)
 ext 02873D70 void hdWindow_constructTransform(void*)
 ext 02873FD0 void hdWindow_destroyTransform(void*,u32)
 ext 0287F3FC void hdWindow_freePane(hdWindowPane_l*)
 ext 02755FEC hdScreenHeap* hdWindow_getHeap(void*,void*)

 data sWindowInitLists 1049DA54
 data sWindowInitAngles 1049DA48
 data sWindowAngleLimits 10109AC8
 data sWindowFirstRecord 101F712C
 data sWindowSecondRecord 101F7138
 data sWindowThirdRecord 101F7144
 ext 028F026C void hdWindow_registerStatic(hdHeartInitRecord_l*)
 ext 028ED6F8 void hdWindow_initFlag(be<u8>*)
 ext 028EAB2C void hdWindow_initByte(be<u8>*)
 ext 028713B8 u32 hdWindow_frameCount(hdWindowAnimator_l*)
 ext 026F3F60 void hdWindow_resetLayout(hdWindowPart_c*)
 ext 020198D8 f32 hdWindow_randomFloat(f32)

 data sWindowRandom 101F8B60
 ext 0273B5DC void hdWindow_destroyArray(void*)
 ext 02613F20 hdWindowTreeNode_l* hdWindow_findTree(hdWindowTree_l*,hdWindowTreeNode_l*,hdUiSafeString*)
 ext 026F4EA8 void hdWindow_setConfig(hdWindowPart_c*,hdWindowConfig_l*)
 ext 026F50AC void hdWindow_begin(hdWindowPart_c*)
 ext 02876368 void hdWindow_measurePane(hdWindowPane_l*,hdWindowMetrics_l*)
 ext 028782A8 void hdWindow_measureCharacter(hdWindowPane_l*,hdWindowMetrics_l*)
 ext 0275CEE4 u32 hdWindow_random(void*)
*/
