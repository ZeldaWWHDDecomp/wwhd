/* HD menu pane controller utilities; game binary-derived implementation. */
#include "d/hd_menu_pane_control.h"
#include "d/hd_parts.h"

/* The previous menu-map TU installs these query delegates in its named states. */
/* 0268AE7C */
static u32 queryMenuBlankState() { return 0; }

/* 0268AE8C */
static u32 queryMenuDisplayMenuMapState() { return 0; }

/* 0268AE9C */
static u32 queryMenuDisplayDungeonMapState() { return 0; }

/* 0268AEA8 */
static u32 queryMenuInAllMenuMapState() { return 0; }

/* 0268AEB4 */
static u32 queryMenuInAllDungeonMapState() { return 0; }

/* 0268AEC0 */
static u32 queryMenuOutAllMenuMapState() { return 0; }

/* 0268AECC */
static u32 queryMenuOutAllDungeonMapState() { return 0; }

/* 0268AED4 */
static u32 queryMenuChangeToMenuMapState() { return 0; }

/* 0268AEDC */
static u32 queryMenuChangeToDungeonMapState() { return 0; }

struct hdMenuSortKey_l {
    u8 _padding[2];
    be<s8> mPriority;
};
WWHD_OFFSET(hdMenuSortKey_l, mPriority, 2);

/* 0268AEE4 */
static s32 compareMenuPriority(const hdMenuSortKey_l* left, const hdMenuSortKey_l* right) {
    s32 rightPriority = right->mPriority;
    return s32(left->mPriority) - rightPriority;
}

/* 0268AEFC */
static s32 menuStateNumber(const hdStateId* state) { return state->mId; }

/* 0268AFD0 */
static hdStateObject* createMenuPartsState(hdPartsStateHolder_c* holder, hdPartsStateId_c* state) {
    holder->mState.mpStateId = state;
    return reinterpret_cast<hdStateObject*>(&holder->mState);
}

/* 0268AFDC */
static void clearMenuPartsState(hdPartsStateHolder_c*, gptr<hdStateObject>* state) {
    *state = NULL;
}

/* 0268AFE8 */
static hdPartsStateId_c* getMenuPartsStateId(hdPartsState_c* state) { return state->mpStateId; }

/* 0268AFF0 */
static void enterMenuPartsState(hdPartsState_c* state) {
    hdPartsStateId_c* id = state->mpStateId;
    hdParts_c* owner = state->mpParts;
    id->vEnter(owner);
}

/* 0268B00C */
static void executeMenuPartsState(hdPartsState_c* state) {
    hdPartsStateId_c* id = state->mpStateId;
    hdParts_c* owner = state->mpParts;
    id->vExecute(owner);
}

/* 0268B028 */
static void exitMenuPartsState(hdPartsState_c* state) {
    hdPartsStateId_c* id = state->mpStateId;
    hdParts_c* owner = state->mpParts;
    id->vExit(owner);
    state->mpStateId = NULL;
}

/* 0268B1BC */
static void destroyMenuMapPart(void*, hdParts_c* child) {
    if (child != NULL) {
        child->vFinalize();
        child->vDelete(3);
    }
}

/* 0268B214 */
static void destroyDungeonMapPart(void*, hdParts_c* child) {
    if (child != NULL) {
        child->vFinalize();
        child->vDelete(3);
    }
}

struct hdMenuHeapManager_l {
    u8 _padding[0x10];
    gptr<void> mpHeap;
};
WWHD_OFFSET(hdMenuHeapManager_l, mpHeap, 0x10);
struct hdMenuStaticValues_l {
    be<f32> mNegativePi;
    be<f32> mPositivePi;
    hdMenuPaneVector mLargeBounds;
    hdMenuPaneVector mSmallBounds;
    be<u8> mRandom;
    be<u8> mUtility;
    u8 _padding[2];
    be<u32> mStorage[4];
};
WWHD_OFFSET(hdMenuStaticValues_l, mRandom, 0x18);
WWHD_OFFSET(hdMenuStaticValues_l, mStorage, 0x1C);
struct hdMenuStaticRegistration_l { u8 mStorage[12]; };
static hdMenuHeapManager_l* sMenuHeapManager;
static hdMenuStaticValues_l sMenuStaticValues;
static hdMenuStaticRegistration_l sMenuStorageRegistration;
static hdMenuStaticRegistration_l sMenuRandomRegistration;
static hdMenuStaticRegistration_l sMenuUtilityRegistration;
extern void* allocateMenuChild(u32, void*, u32);
extern hdParts_c* constructMenuMapChild(hdParts_c*);
extern hdParts_c* constructDungeonMapChild(hdParts_c*);
extern void createMenuMapChild(hdParts_c*, void*, void*, u32);
extern void createDungeonMapChild(hdParts_c*, void*, void*, u32);
extern void registerMenuStatic(hdMenuStaticRegistration_l*);
extern void constructMenuRandom(be<u8>*);
extern void constructMenuUtility(be<u8>*);

/* 0268B0CC */
static void makeMenuMapChild(void*, gptr<hdParts_c>* output, void* first, void* second) {
    hdParts_c* child = static_cast<hdParts_c*>(allocateMenuChild(0x12C, sMenuHeapManager->mpHeap.get(), 4));
    if (child != NULL) child = constructMenuMapChild(child);
    *output = child;
    createMenuMapChild(child, first, second, 0);
}

/* 0268B144 */
static void makeDungeonMapChild(void*, gptr<hdParts_c>* output, void* first, void* second) {
    hdParts_c* child = static_cast<hdParts_c*>(allocateMenuChild(0x928, sMenuHeapManager->mpHeap.get(), 4));
    if (child != NULL) child = constructDungeonMapChild(child);
    *output = child;
    createDungeonMapChild(child, first, second, 0);
}

/* 0268B664 */
static void initializeMenuPaneStatics() {
    sMenuStaticValues.mStorage[3] = 0;
    sMenuStaticValues.mStorage[2] = 0;
    sMenuStaticValues.mStorage[1] = 0;
    sMenuStaticValues.mStorage[0] = 0;
    registerMenuStatic(&sMenuStorageRegistration);
    sMenuStaticValues.mNegativePi = -3.1415927410125732f;
    sMenuStaticValues.mPositivePi = 3.1415927410125732f;
    constructMenuRandom(&sMenuStaticValues.mRandom);
    registerMenuStatic(&sMenuRandomRegistration);
    constructMenuUtility(&sMenuStaticValues.mUtility);
    registerMenuStatic(&sMenuUtilityRegistration);
    sMenuStaticValues.mLargeBounds.x = 50000.0f;
    sMenuStaticValues.mSmallBounds.x = 10000.0f;
    sMenuStaticValues.mLargeBounds.y = 50000.0f;
    sMenuStaticValues.mSmallBounds.y = 10000.0f;
}

extern void* paneControlAllocate(u32);
extern void paneControlDelete(void*);
extern f32 fadeMenuPaneIn(hdMenuPane_l*);
extern f32 fadeMenuPaneOut(hdMenuPane_l*);

static void constructMenuPaneVector(hdMenuPaneVector* vector) {
    if (gabi::ea(vector) == 0) {
        vector = static_cast<hdMenuPaneVector*>(paneControlAllocate(sizeof(hdMenuPaneVector)));
        if (vector == NULL) return;
    }
    vector->y = 0.0f;
    vector->x = 0.0f;
}

/* 0268B26C */
hdMenuPaneControl* hdMenuPaneControl::construct(hdMenuPaneControl* self) {
    if (self == NULL) {
        self = static_cast<hdMenuPaneControl*>(paneControlAllocate(sizeof(hdMenuPaneControl)));
        if (self == NULL) return NULL;
    }
    self->mpRotationPane = NULL;
    self->mpPane = NULL;
    constructMenuPaneVector(&self->mPosition);
    constructMenuPaneVector(&self->mPositionOffset);
    constructMenuPaneVector(&self->mBasePosition);
    constructMenuPaneVector(&self->mBaseOffset);
    self->mFlags = 0;
    self->mLastVisible = 0;
    self->mRotation = 0.0f;
    self->mScale = 1.0f;
    self->mScaleFactor = 1.0f;
    self->mAlpha = 0;
    self->mVisible = 0;
    self->mDuration = 10;
    self->mRotationPaneVisible = 0;
    self->mRotationOffset = 0.0f;
    self->mFading = 0;
    return self;
}

/* 0268B400 */
void hdMenuPaneControl::destroy(u32 flags) {
    if (this != NULL) {
        reset();
        if ((flags & 1) != 0) paneControlDelete(this);
    }
}

/* 0268B434 */
void hdMenuPaneControl::update() {
    hdMenuPane_l* pane = mpPane;
    if (pane != NULL) {
        f32 x = f32(mBasePosition.x) + f32(mBaseOffset.x);
        f32 y = f32(mBasePosition.y) + f32(mBaseOffset.y);
        u8 flags = pane->mFlags;
        pane->mTranslationY = y;
        pane->mTranslationZ = 0.0f;
        pane->mFlags = flags | 0x10;
        pane->mTranslationX = x;
        f32 scale = f32(mScale) * f32(mScaleFactor);
        pane = mpPane;
        flags = pane->mFlags;
        pane->mScaleY = scale;
        pane->mScaleX = scale;
        pane->mFlags = flags | 0x10;
        u8 visible = mVisible;
        if (visible != u8(mLastVisible)) {
            visible = mVisible;
            mFading = 1;
            pane = mpPane;
            pane->mAlpha = visible != 0 ? 0 : 255;
            visible = mVisible;
        }
        u8 fading = mFading;
        mLastVisible = visible;
        pane = mpPane;
        if (fading != 0) {
            f32 alpha;
            if (mVisible != 0) {
                alpha = fadeMenuPaneIn(pane);
                if (!(1.0f > alpha)) mFading = 0;
            } else {
                alpha = fadeMenuPaneOut(pane);
                if (!(alpha > 0.0f)) mFading = 0;
            }
            u8 opacity = u8(gabi::ftoi(alpha * 255.0f));
            pane = mpPane;
            mAlpha = opacity;
            pane->mAlpha = opacity;
        } else {
            flags = u8(pane->mFlags) & 0xFE;
            pane->mFlags = flags;
            pane->mFlags = flags | u8(mVisible);
            u8 opacity = mAlpha;
            pane = mpPane;
            pane->mAlpha = opacity;
        }
    }
    pane = mpRotationPane;
    if (pane == NULL) return;
    f32 x = pane->mRotationX;
    f32 rotation = mRotation;
    f32 y = pane->mRotationY;
    f32 z = pane->mRotationZ;
    (void)z;
    f32 offset = mRotationOffset;
    pane->mRotationX = x;
    u8 flags = pane->mFlags;
    pane->mRotationY = y;
    pane->mRotationZ = rotation + offset;
    pane->mFlags = flags | 0x10;
    pane = mpRotationPane;
    if (pane != mpPane.get()) {
        if (mRotationPaneVisible != 0) fadeMenuPaneIn(pane);
        else fadeMenuPaneOut(pane);
    }
}

/* 0268B070 */
static u32 approachMenuValue(be<f32>* value, f32 target, f32 step) {
    f32 current = *value;
    if (current < target) {
        f32 next = current + step;
        if (!(target > next) || next < current) {
            *value = target;
            return 1;
        }
        *value = next;
        return 0;
    }
    if (current > target) {
        f32 next = current - step;
        if (!(next > target) || current < next) {
            *value = target;
            return 1;
        }
        *value = next;
        return 0;
    }
    return 1;
}

/* 0268B398 */
void hdMenuPaneControl::reset() {
    mFading = 0;
    mPositionOffset.x = 0.0f;
    mScale = 1.0f;
    mBasePosition.y = 0.0f;
    mPosition.y = 0.0f;
    mPositionOffset.y = 0.0f;
    mAlpha = 0;
    mPosition.x = 0.0f;
    mRotationOffset = 0.0f;
    mpPane = NULL;
    mRotation = 0.0f;
    mFlags = 0;
    mBaseOffset.x = 0.0f;
    mLastVisible = 0;
    mDuration = 10;
    mVisible = 0;
    mBasePosition.x = 0.0f;
    mBaseOffset.y = 0.0f;
    mScaleFactor = 1.0f;
}

/* WWHD glue (tools/verify/autoglue.py): menu pane bindings
 data sMenuHeapManager 1018C404
 data sMenuStaticValues 10496564
 data sMenuStorageRegistration 101F6308
 data sMenuRandomRegistration 101F6314
 data sMenuUtilityRegistration 101F6320
 ext 0273B050 void* allocateMenuChild(u32,void*,u32)
 ext 0269C6BC hdParts_c* constructMenuMapChild(hdParts_c*)
 ext 0265B2D8 hdParts_c* constructDungeonMapChild(hdParts_c*)
 ext 0269D098 void createMenuMapChild(hdParts_c*,void*,void*,u32)
 ext 0265C2A8 void createDungeonMapChild(hdParts_c*,void*,void*,u32)
 ext 028F026C void registerMenuStatic(hdMenuStaticRegistration_l*)
 ext 028ED6F8 void constructMenuRandom(be<u8>*)
 ext 028EAB2C void constructMenuUtility(be<u8>*)
 ext 0273AD10 void* paneControlAllocate(u32)
 ext 0273AF40 void paneControlDelete(void*)
 ext 026899D4 f32 fadeMenuPaneIn(hdMenuPane_l*)
 ext 02689AB4 f32 fadeMenuPaneOut(hdMenuPane_l*)
*/
