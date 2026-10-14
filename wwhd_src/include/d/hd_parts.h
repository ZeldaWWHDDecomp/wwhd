/* HD: base class of the UI parts and parts screens (hd_parts.cpp): a layout screen, a state machine
 * (states Blank and Display), the child parts the layout lists (built by a factory from the parts class
 * names "BasicButton", "Heart", "InOutLoop", "KeyIcon", "Loop", "NoControl", "NoDecideButton",
 * "OneTime", "ProgressBar", "Valid") and resource archives attached to a multi-archive accessor.
 * No GameCube counterpart.
 *
 * Names: the states and parts class names are HD strings; every class, member, method and static name
 * here is ours (descriptive). "probably" marks a role read from its use only. Shared views reused:
 * hdStateMachine / hdStateId / hdStateOwner (hd/hd_sys_02006294.h), hdUiSafeString / hdUiVisitorList
 * (hd/hd_ui_020032F4.h), hdAnimSet (hd/hd_ui_02004A3C.h), hdScreenHeap / hdScreenUiSystem
 * (hd/hd_ui_020021FC.h). */
#pragma once
#include "bindings.h"
#include "hd/hd_sys_02006294.h"
#include "hd/hd_ui_020032F4.h"
#include "hd/hd_ui_02004A3C.h"
#include "d/hd_parts_lyt.h"
#include "hd/hd_ui_020021FC.h"

struct hdParts_c;
typedef be<u8> hdPartsChar_t; /* a guest string character */

/* ---- states ---- */

/* a GHS pointer to member function as the state delegates store it: `this` adjustment, then either a
 * virtual index (>= 0) with the vtable offset in the low half of mFunction, or (index < 0) a function */
struct hdPartsPtmf_c {
    /* 0x00 */ be<s16> mThisDelta;
    /* 0x02 */ be<s16> mVirtualIndex;
    /* 0x04 */ be<u32> mFunction;
    void setVirtual(s16 index) {
        mThisDelta = 0;
        mVirtualIndex = index;
        mFunction = PARTS_VTABLE_OFFSET;
    }
    void clear() {
        mThisDelta = 0;
        mVirtualIndex = 0;
        mFunction = 0;
    }
    enum { PARTS_VTABLE_OFFSET = 4 }; /* the parts' vtable pointer is at +4 */
};
WWHD_SIZE(hdPartsPtmf_c, 8);
WWHD_OFFSET(hdPartsPtmf_c, mFunction, 4);

/* a state id with its name and its enter / execute / exit delegates (0x30); the per-TU statics
 * ::StateID_* of every parts class */
struct hdParts_c;
struct hdPartsStateIdVtbl_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gfn mGetKey; /* probably the number the state machine compares */
    /* 0x18 */ u8 _18[0xC];
    /* 0x24 */ gfn mEnter;   /* the delegates, called with the parts */
    /* 0x28 */ u8 _28[4];
    /* 0x2C */ gfn mExecute;
    /* 0x30 */ u8 _30[4];
    /* 0x34 */ gfn mExit;
};
WWHD_OFFSET(hdPartsStateIdVtbl_c, mGetKey, 0x14);
WWHD_OFFSET(hdPartsStateIdVtbl_c, mEnter, 0x24);
WWHD_OFFSET(hdPartsStateIdVtbl_c, mExecute, 0x2C);
WWHD_OFFSET(hdPartsStateIdVtbl_c, mExit, 0x34);
struct hdPartsStateId_c {
    /* 0x00 */ be<s32> mNumber;
    /* 0x04 */ gptr<const char> mName;
    /* 0x08 */ gptr<hdPartsStateIdVtbl_c> mVtable;
    /* 0x0C */ hdPartsPtmf_c mEnter;
    /* 0x14 */ hdPartsPtmf_c mExecute;
    /* 0x1C */ hdPartsPtmf_c mExit;
    /* 0x24 */ hdPartsPtmf_c mUnused;
    /* 0x2C */ gptr<hdStateId> mpNullStateId; /* probably */

    /* the static initialiser's setup of one state: number, name and the three virtual delegates */
    void setup(s32 number, const char* name, hdPartsStateIdVtbl_c* vtable, s16 enterIndex, hdStateId* nullStateId) {
        mNumber = number;
        mName.v = gabi::guest_addr(name);
        mVtable = vtable;
        mEnter.setVirtual(enterIndex);
        mExecute.setVirtual(enterIndex + 1);
        mExit.setVirtual(enterIndex + 2);
        mUnused.clear();
        mpNullStateId = nullStateId;
    }
    u32 getKey() { return gabi::call_ptr<u32>(mVtable->mGetKey.get(), this); }
    void vEnter(hdParts_c* parts) { gabi::call_ptr(mVtable->mEnter.get(), this, parts); }
    void vExecute(hdParts_c* parts) { gabi::call_ptr(mVtable->mExecute.get(), this, parts); }
    void vExit(hdParts_c* parts) { gabi::call_ptr(mVtable->mExit.get(), this, parts); }
};
WWHD_OFFSET(hdPartsStateId_c, mName, 4);
WWHD_OFFSET(hdPartsStateId_c, mVtable, 8);
WWHD_OFFSET(hdPartsStateId_c, mEnter, 0xC);
WWHD_OFFSET(hdPartsStateId_c, mExecute, 0x14);
WWHD_OFFSET(hdPartsStateId_c, mExit, 0x1C);
WWHD_OFFSET(hdPartsStateId_c, mUnused, 0x24);
WWHD_OFFSET(hdPartsStateId_c, mpNullStateId, 0x2C);
WWHD_SIZE(hdPartsStateId_c, 0x30);

/* the state numbers are handed out by a counter shared by all TUs (guarded static) */
struct hdPartsStateCounter_c {
    be<u32> mValue;
};

/* the state machine's owner and current state in one object (0x28, at hdParts_c+8): its first vtable
 * is the owner interface (createState / onStateChange), its second (+4) the state interface
 * (getStateId / enter / execute / exit) that forwards to the state id's delegates on the parts */
struct hdPartsState_c {
    /* 0x00 */ gptr<void> mVtable;
    /* 0x04 */ gptr<hdParts_c> mpParts;
    /* 0x08 */ gptr<hdPartsStateId_c> mpStateId;

    hdPartsStateId_c* getStateId();
    void enter();
    void execute();
    void exit();
};
WWHD_OFFSET(hdPartsState_c, mpStateId, 8);
WWHD_SIZE(hdPartsState_c, 0xC);
struct hdPartsStateHolder_c {
    /* 0x00 */ gptr<void> mOwnerVtable;
    /* 0x04 */ hdPartsState_c mState;
    /* 0x10 */ hdStateMachine mMachine;
    /* 0x24 */ gptr<void> mHolderVtable;

    hdStateObject* createState(hdPartsStateId_c* stateId);
    void onStateChange(gptr<hdStateObject>* state);
};
WWHD_OFFSET(hdPartsStateHolder_c, mState, 4);
WWHD_OFFSET(hdPartsStateHolder_c, mMachine, 0x10);
WWHD_OFFSET(hdPartsStateHolder_c, mHolderVtable, 0x24);
WWHD_SIZE(hdPartsStateHolder_c, 0x28);

/* ---- the parts' layout screen: hdPartsLayout_c (0x110, FW-layout_mgr) derives from hdLayoutScreen
 * (hd/hd_ui_020021FC.h); its +4 object is the parts' NW4F layout (hdPartsLyt_c), the animation set is
 * at +0xD4 and the vtable at +0xE0. Defined here, after hdPartsResAccessor_c (its +0xE4 member). ---- */
struct hdPartsLayoutVtbl_c {
    /* 0x00 */ u8 _00[0x0C];
    /* 0x0C */ gfn mDelete;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ gfn mLoad;
    /* 0x18 */ u8 _18[4];
    /* 0x1C */ gfn mRelease;
    /* 0x20 */ u8 _20[4];
    /* 0x24 */ gfn mCalc;
    /* 0x28 */ u8 _28[4];
    /* 0x2C */ gfn mUpdate; /* probably */
    /* 0x30 */ u8 _30[4];
    /* 0x34 */ gfn mDraw;
};
WWHD_OFFSET(hdPartsLayoutVtbl_c, mDelete, 0xC);
WWHD_OFFSET(hdPartsLayoutVtbl_c, mLoad, 0x14);
WWHD_OFFSET(hdPartsLayoutVtbl_c, mRelease, 0x1C);
WWHD_OFFSET(hdPartsLayoutVtbl_c, mCalc, 0x24);
WWHD_OFFSET(hdPartsLayoutVtbl_c, mUpdate, 0x2C);
WWHD_OFFSET(hdPartsLayoutVtbl_c, mDraw, 0x34);
struct hdPartsLayout_c;

/* animation set calls (hd_ui_02004A3C: entry / slot indices) */
inline void hdAnimSet_play(hdAnimSet* set, u32 entry, u32 slot, f32 rate) { gabi::call(0x020053E4, set, entry, slot, rate); }
inline void hdAnimSet_playAlternate(hdAnimSet* set, u32 entry, u32 slot, f32 rate) { gabi::call(0x02005488, set, entry, slot, rate); }
inline void hdAnimSet_setFrame(hdAnimSet* set, u32 entry, u32 slot, f32 frame) { gabi::call(0x0200552C, set, entry, slot, frame); }
inline void hdAnimSet_operationC4(hdAnimSet* set, u32 entry, u32 slot) { gabi::call(0x020055E0, set, entry, slot); }
inline void hdAnimSet_operationCC(hdAnimSet* set, u32 entry, u32 slot) { gabi::call(0x02005674, set, entry, slot); }
inline u32 hdAnimSet_isEnd(hdAnimSet* set, u32 entry) { return gabi::call<u32>(0x02005840, set, entry); }

/* ---- resource archives (library objects, called by address) ---- */
struct hdPartsResAccessorVtbl_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gfn mDelete;
};
/* the multi-archive resource accessor (0x2C, constructed by 02702F98) */
struct hdPartsResAccessor_c {
    /* 0x00 */ gptr<hdPartsResAccessorVtbl_c> mVtable;
    /* 0x04 */ u8 _04[0x2C - 4];
    static hdPartsResAccessor_c* construct(hdPartsResAccessor_c* i_this) { return gabi::call<hdPartsResAccessor_c*>(0x02702F98, i_this); }
    void destroy(s32 flags) { gabi::call_ptr(mVtable->mDelete.get(), this, flags); }
};
WWHD_SIZE(hdPartsResAccessor_c, 0x2C);
struct hdPartsLayout_c : hdLayoutScreen {
    /* 0x0E4 */ hdPartsResAccessor_c mResourceBinding;

    hdPartsLyt_c* lyt() { return gabi::at<hdPartsLyt_c>(mAccessor.v.get()); }
    hdAnimSet* animSet() { return gabi::at<hdAnimSet>(mAnimationSet.v.get()); }
    hdPartsLayoutVtbl_c* vtbl() { return gabi::at<hdPartsLayoutVtbl_c>(mVtable.v.get()); }

    static hdPartsLayout_c* construct(hdPartsLayout_c* i_this) { return gabi::call<hdPartsLayout_c*>(0x026FADB0, i_this); }
    u32 prepare(u32 first, u32 second, void* resource, void* block, u32 option, u32 flags, void* archive);
    u32 load(hdScreenString* name, u32 kind);
    /* virtual calls (through the vtable at +0xE0) */
    u32 vLoad(void* name, u32 arg1, u32 arg2, u32 arg3, u32 animCount, u32 groupCount, void* desc) {
        return gabi::call_ptr<u32>(vtbl()->mLoad.get(), this, name, arg1, arg2, arg3, animCount, groupCount, desc);
    }
    void release() { gabi::call_ptr(vtbl()->mRelease.get(), this); }
    void destroy(s32 flags) { gabi::call_ptr(vtbl()->mDelete.get(), this, flags); }
    void calc() { gabi::call_ptr(vtbl()->mCalc.get(), this); }
    void update() { gabi::call_ptr(vtbl()->mUpdate.get(), this); }
    void draw(void* info) { gabi::call_ptr(vtbl()->mDraw.get(), this, info); }
};
WWHD_OFFSET(hdPartsLayout_c, mAccessor, 4);
WWHD_OFFSET(hdPartsLayout_c, mAnimationSet, 0xD4);
WWHD_OFFSET(hdPartsLayout_c, mVtable, 0xE0);
WWHD_OFFSET(hdPartsLayout_c, mResourceBinding, 0xE4);
WWHD_SIZE(hdPartsLayout_c, 0x110);
/* one archive link (0x64): two list words, then an archive accessor */
struct hdPartsArcLink_c {
    /* 0x00 */ be<u32> mLink[2];
    /* 0x08 */ u8 mAccessor[0x64 - 8];
};
WWHD_OFFSET(hdPartsArcLink_c, mAccessor, 8);
WWHD_SIZE(hdPartsArcLink_c, 0x64);
struct hdPartsArcLinkArray_c {
    /* 0x00 */ be<s32> mNum;
    /* 0x04 */ be<s32> mMax;
    /* 0x08 */ gptr<gptr<hdPartsArcLink_c>> mpLinks;
    gptr<hdPartsArcLink_c>* slot(u32 index) { return hdPartsView<gptr<hdPartsArcLink_c>>(gabi::ea(mpLinks.get()) + index * 4); }
    /* sead PtrArray access: NULL out of range */
    hdPartsArcLink_c* at(s32 index) { return u32(index) < u32(mNum) ? (hdPartsArcLink_c*)*slot(index) : NULL; }
};
WWHD_SIZE(hdPartsArcLinkArray_c, 0xC);
inline void hdParts_constructArcAccessor(void* accessor) { gabi::call(0x02873A40, accessor); }
inline void hdParts_destroyArcAccessor(void* accessor, s32 flags) { gabi::call(0x02873AC0, accessor, flags); }
inline void hdParts_setArcLink(hdPartsArcLink_c* link, void* archive, const char* rootDir) { gabi::call(0x028733DC, link, archive, rootDir); }
inline void hdParts_attachArcLink(hdPartsResAccessor_c* accessor, hdPartsArcLink_c* link) { gabi::call(0x02873604, accessor, link); }
inline void hdParts_detachAllArcLinks(hdPartsResAccessor_c* accessor) { gabi::call(0x02873548, accessor); }
inline void hdParts_allocArcLinks(hdPartsArcLinkArray_c* array, u32 count, hdScreenHeap* heap, u32 alignment) { gabi::call(0x0273B584, array, count, heap, alignment); }
inline void hdParts_freeArcLinks(hdPartsArcLinkArray_c* array) { gabi::call(0x0273B5DC, array); }
/* 026FBB24 (FW-layout_mgr): the archive the layout resource manager holds under `name` */
inline void* hdParts_findArchive(void* resMgr, hdUiSafeString* name) { return gabi::call<void*>(0x026FBB24, resMgr, name); }
/* 025F36D8: the font manager uses the accessor's archives too */
inline void hdParts_setFontResAccessor(void* fontMgr, hdPartsResAccessor_c* accessor) { gabi::call(0x025F36D8, fontMgr, accessor); }

/* ---- heaps and the UI base classes (called by address) ---- */
inline void* hdParts_new(u32 size, hdScreenHeap* heap, u32 alignment) { return gabi::call<void*>(0x0273B050, size, heap, alignment); }
inline hdScreenHeap* hdParts_getCurrentHeap(void* heapMgr) { return gabi::call<hdScreenHeap*>(0x02756140, heapMgr); }
inline hdScreenHeap* hdParts_findContainHeap(void* heapMgr, void* memory) { return gabi::call<hdScreenHeap*>(0x02755FEC, heapMgr, memory); }
inline void hdParts_constructVisitor(hdParts_c* parts) { gabi::call(0x020032F4, parts); }
inline void hdParts_destroyVisitor(hdParts_c* parts, s32 flags) { gabi::call(0x0200333C, parts, flags); }
inline u32 hdParts_visitAll(hdParts_c* parts, hdPartsDescList_c* list) { return gabi::call<u32>(0x02003350, parts, list); }
inline void hdParts_createStateMachine(hdStateMachine* machine, hdPartsStateHolder_c* owner, hdStateId* initial) {
    gabi::call(0x020062E8, machine, owner, initial);
}
inline void hdParts_executeStateMachine(hdStateMachine* machine) { gabi::call(0x02006364, machine); }
inline void hdParts_changeState(hdStateMachine* machine, hdPartsStateId_c* stateId) { gabi::call(0x020063C0, machine, stateId); }
inline hdPartsStateId_c* hdParts_getCurrentStateId(hdStateMachine* machine) { return gabi::call<hdPartsStateId_c*>(0x02006478, machine); }

/* ---- the parts ---- */

/* the child parts (sead Buffer: count, data) */
struct hdPartsChildArray_c {
    /* 0x00 */ be<s32> mNum;
    /* 0x04 */ gptr<gptr<hdParts_c>> mpData;
    /* element `index` of the data, 32-bit address arithmetic */
    gptr<hdParts_c>* slot(u32 index) { return hdPartsView<gptr<hdParts_c>>(gabi::ea(mpData.get()) + index * 4); }
    /* HD: sead Buffer access: an index out of range uses element 0 */
    gptr<hdParts_c>* safeSlot(u32 index) { return slot(index < u32(mNum) ? index : 0); }
};
WWHD_SIZE(hdPartsChildArray_c, 8);

/* vtable of hdParts_c (GHS: 8-byte entries, function word at +4); derived parts append their state
 * delegates from +0xCC */
struct hdPartsVtbl_c {
    /* 0x00 */ u8 _00[0x0C];
    /* 0x0C */ gfn mDelete;
    /* 0x10 */ u8 _10[0x44];
    /* 0x54 */ gfn mFinalize;
    /* 0x58 */ u8 _58[4];
    /* 0x5C */ gfn mExecuteState;
    /* 0x60 */ u8 _60[0xC];
    /* 0x6C */ gfn mDraw;
    /* 0x70 */ u8 _70[0xC];
    /* 0x7C */ gfn mLoad;
    /* 0x80 */ u8 _80[4];
    /* 0x84 */ gfn mDestroyLayout;
    /* 0x88 */ u8 _88[4];
    /* 0x8C */ gfn mCalcLayout;
    /* 0x90 */ u8 _90[4];
    /* 0x94 */ gfn mUpdateLayout;
    /* 0x98 */ u8 _98[4];
    /* 0x9C */ gfn mDrawLayout;
    /* 0xA0 */ u8 _A0[4];
    /* 0xA4 */ gfn mCreateChildren;
    /* 0xA8 */ u8 _A8[4];
    /* 0xAC */ gfn mDeleteChildren;
    /* 0xB0 */ u8 _B0[4];
    /* 0xB4 */ gfn mExecuteChildren;
    /* 0xB8 */ u8 _B8[0xC];
    /* 0xC4 */ gfn mDrawChildren;
};
WWHD_OFFSET(hdPartsVtbl_c, mDelete, 0xC);
WWHD_OFFSET(hdPartsVtbl_c, mFinalize, 0x54);
WWHD_OFFSET(hdPartsVtbl_c, mExecuteState, 0x5C);
WWHD_OFFSET(hdPartsVtbl_c, mDraw, 0x6C);
WWHD_OFFSET(hdPartsVtbl_c, mLoad, 0x7C);
WWHD_OFFSET(hdPartsVtbl_c, mDestroyLayout, 0x84);
WWHD_OFFSET(hdPartsVtbl_c, mCalcLayout, 0x8C);
WWHD_OFFSET(hdPartsVtbl_c, mUpdateLayout, 0x94);
WWHD_OFFSET(hdPartsVtbl_c, mDrawLayout, 0x9C);
WWHD_OFFSET(hdPartsVtbl_c, mCreateChildren, 0xA4);
WWHD_OFFSET(hdPartsVtbl_c, mDeleteChildren, 0xAC);
WWHD_OFFSET(hdPartsVtbl_c, mExecuteChildren, 0xB4);
WWHD_OFFSET(hdPartsVtbl_c, mDrawChildren, 0xC4);

struct hdParts_c {
    /* 0x00 */ be<u32> mVisitorWord;         /* hdUiVisitor base (020032F4) */
    /* 0x04 */ gptr<hdPartsVtbl_c> mVtable;
    /* 0x08 */ hdPartsStateHolder_c mStateHolder;
    /* 0x30 */ gptr<void> mSecondaryVtable;
    /* 0x34 */ gptr<hdPartsResAccessor_c> mpResAccessor;
    /* 0x38 */ hdPartsArcLinkArray_c mArcLinks;
    /* 0x44 */ gptr<hdPartsLayout_c> mpLayout;
    /* 0x48 */ hdPartsChildArray_c mChildren;

    static hdParts_c* construct(hdParts_c* i_this);
    static void destroy(hdParts_c* i_this, s32 flags);
    void displayExecute();
    void finalize();
    void executeState();
    void draw(void* info);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void destroyLayout();
    void calcLayout();
    void updateLayout();
    void drawLayout(void* info);
    u32 createChildren();
    void deleteChildren();
    void executeChildren();
    void drawChildren(void* info);
    void attachArchives(hdUiSafeString* names, u32 count);
    void detachArchives();
    u32 createChild(u32 index, hdPartsDesc_c* desc);

    hdStateMachine* stateMachine() { return &mStateHolder.mMachine; }
    hdAnimSet* animSet() { return mpLayout->animSet(); }
    void changeState(hdPartsStateId_c* stateId) { hdParts_changeState(stateMachine(), stateId); }
    /* the first state, entered directly (no previous state to leave) */
    void startState(hdPartsStateId_c* stateId) {
        hdStateOwner* owner = mStateHolder.mMachine.mpOwner;
        hdStateObject* state = owner->createState((hdStateId*)stateId);
        mStateHolder.mMachine.mpCurrentState = state;
        state->enter();
    }
    gptr<hdParts_c>* childSlot(u32 index) { return mChildren.safeSlot(index); }
    hdParts_c* child(u32 index) { return *childSlot(index); }

    /* virtual calls (through the vtable) */
    void vDelete(s32 flags) { gabi::call_ptr(mVtable->mDelete.get(), this, flags); }
    void vFinalize() { gabi::call_ptr(mVtable->mFinalize.get(), this); }
    void vExecuteState() { gabi::call_ptr(mVtable->mExecuteState.get(), this); }
    void vDraw(void* info) { gabi::call_ptr(mVtable->mDraw.get(), this, info); }
    u32 vLoad(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
        return gabi::call_ptr<u32>(mVtable->mLoad.get(), this, name, arg1, arg2, arg3, desc);
    }
    void vDestroyLayout() { gabi::call_ptr(mVtable->mDestroyLayout.get(), this); }
    void vCalcLayout() { gabi::call_ptr(mVtable->mCalcLayout.get(), this); }
    void vDrawLayout(void* info) { gabi::call_ptr(mVtable->mDrawLayout.get(), this, info); }
    void vDeleteChildren() { gabi::call_ptr(mVtable->mDeleteChildren.get(), this); }
    void vExecuteChildren() { gabi::call_ptr(mVtable->mExecuteChildren.get(), this); }
    void vDrawChildren(void* info) { gabi::call_ptr(mVtable->mDrawChildren.get(), this, info); }
};
WWHD_OFFSET(hdParts_c, mVtable, 4);
WWHD_OFFSET(hdParts_c, mStateHolder, 8);
WWHD_OFFSET(hdParts_c, mSecondaryVtable, 0x30);
WWHD_OFFSET(hdParts_c, mpResAccessor, 0x34);
WWHD_OFFSET(hdParts_c, mArcLinks, 0x38);
WWHD_OFFSET(hdParts_c, mpLayout, 0x44);
WWHD_OFFSET(hdParts_c, mChildren, 0x48);
WWHD_SIZE(hdParts_c, 0x50);

/* a static SafeString of a literal (static initialisers) */
inline void hdParts_setSafeString(hdUiSafeString* string, hdUiStringTable* vtable, const char* text) {
    string->mVtable = vtable;
    string->mText.v = gabi::guest_addr(text);
}

/* 026FA16C (hd_parts.cpp): this TU's copy of the SafeString termination (empty for literals) */
void hdParts_assureTermination(hdUiSafeString* string);

/* the other parts classes the factory builds (other FW units; constructor and init by address) */
inline hdParts_c* hdPartsBasicButton_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x026F1C3C, p); }
inline u32 hdPartsBasicButton_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x026F1D04, p, res, a, b, desc); }
inline hdParts_c* hdPartsHeart_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x026F6EBC, p); }
inline u32 hdPartsHeart_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x026F6F48, p, res, a, b, desc); }
inline hdParts_c* hdPartsLoop_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x026FC1AC, p); }
inline u32 hdPartsLoop_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x026FC220, p, res, a, b, desc); }
inline hdParts_c* hdPartsNoControl_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x02700ADC, p); }
inline u32 hdPartsNoControl_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x02700B48, p, res, a, b, desc); }
inline hdParts_c* hdPartsNoDecideButton_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x02701718, p); }
inline u32 hdPartsNoDecideButton_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x0270178C, p, res, a, b, desc); }
inline hdParts_c* hdPartsOneTime_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x027025E0, p); }
inline u32 hdPartsOneTime_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x02702674, p, res, a, b, desc); }
inline hdParts_c* hdPartsProgressBar_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x02702B40, p); }
inline u32 hdPartsProgressBar_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x02702CCC, p, res, a, b, desc); }
inline hdParts_c* hdPartsValid_construct(hdParts_c* p) { return gabi::call<hdParts_c*>(0x027074A4, p); }
inline u32 hdPartsValid_init(hdParts_c* p, void* res, u32 a, u32 b, hdPartsDesc_c* desc) { return gabi::call<u32>(0x02707518, p, res, a, b, desc); }
