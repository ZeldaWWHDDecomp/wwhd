/** HD layout screen loader, text/picture panes and child holder. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_ui_020021FC.h"
static u32 sScreenHolderTypeInitialized;
static u32 sScreenHolderParentInitialized;
static u32 sScreenRootTypeInitialized;
static u32 sScreenPaneOwnerTypeInitialized;
static u32 sScreenPaneOwnerParentInitialized;
static u32 sScreenTypeInitialized;
static u32 sScreenParentInitialized;
static hdScreenType sScreenHolderType;
static hdScreenType sScreenHolderParent;
static hdScreenType sScreenRootType;
static hdScreenType sScreenPaneOwnerType;
static hdScreenType sScreenPaneOwnerParent;
static hdScreenType sScreenType;
static hdScreenType sScreenParent;
static hdScreenUiSystem* sScreenUiSystem;
static void* sScreenHeapManager;
static hdScreenStringTable sScreenFixedStringTable;
static hdScreenStringTable sScreenSafeStringTable;
static hdScreenDeleteTable sScreenHolderTable;
static hdScreenTable sLayoutScreenTable;
/* 020021FC */
static hdScreenTextBox* constructScreenTextBox(hdScreenTextBox* i_this, u32 resource, u32 option, u32 argument) {
    if (i_this == NULL) {
        i_this = (hdScreenTextBox*)operator_new(sizeof(hdScreenTextBox));
        if (i_this == NULL) return NULL;
    }
    hdScreenConstructTextBoxBase(i_this, resource, option, argument);
    i_this->mVtable = 0x100002C8;
    return i_this;
}
/* 02002280 */
static hdScreenTextBox* constructScreenTextBoxResource(hdScreenTextBox* i_this, u32 resource) {
    if (i_this == NULL) {
        i_this = (hdScreenTextBox*)operator_new(sizeof(hdScreenTextBox));
        if (i_this == NULL) return NULL;
    }
    hdScreenConstructTextBoxResource(i_this, resource);
    i_this->mVtable = 0x100002C8;
    return i_this;
}
/* 020022E4 */
static void destroyScreenTextBox(hdScreenTextBox* i_this, s32 flags) {
    if (i_this == NULL) return;
    hdScreenDestroyTextBoxBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 02002338 */
static void __sinit_hd_text_box_cpp() {
    sinit_header_statics_rodata(0x101FF15C, 0x1018C20C, 0x101FF168, 0x100002C0);
}
/* 020023CC */
static hdScreenType* getScreenHolderType() {
    if (sScreenHolderTypeInitialized != 0) return &sScreenHolderType;
    u32 parentInitialized = sScreenHolderParentInitialized;
    sScreenHolderTypeInitialized = 1;
    if (parentInitialized != 0) { sScreenHolderType.mParent = &sScreenHolderParent; return &sScreenHolderType; }
    u32 rootInitialized = sScreenRootTypeInitialized;
    sScreenHolderParentInitialized = 1;
    if (rootInitialized == 0) { sScreenRootTypeInitialized = 1; sScreenRootType.mParent = NULL; }
    sScreenHolderType.mParent = &sScreenHolderParent;
    sScreenHolderParent.mParent = &sScreenRootType;
    return &sScreenHolderType;
}
/* 02002450 */
static hdScreenHolder* constructScreenHolder(hdScreenHolder* i_this, void* owner, void* layout) {
    if (i_this == NULL) {
        i_this = (hdScreenHolder*)operator_new(sizeof(hdScreenHolder));
        if (i_this == NULL) return NULL;
    }
    i_this->mLayout = layout;
    i_this->mOwner = owner;
    i_this->mVtable = &sScreenHolderTable;
    return i_this;
}
/* 020024B0 */
static void destroyScreenHolder(hdScreenHolder* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
static hdScreenPaneOwner* screenPaneOwnerCast(hdScreenPaneOwner* object) {
    if (object == NULL) return NULL;
    hdScreenType* type = gabi::call_ptr<hdScreenType*>(object->mVtable->mGetRuntimeType.get(), object);
    if (type == NULL) return NULL;
    for (; type != NULL; type = type->mParent) {
        if (type == &sScreenPaneOwnerType) return object;
    }
    return NULL;
}
/* 020024C4 */
static void attachScreenHolder(hdScreenHolder* i_this, void* argument, hdScreenPaneOwner* object) {
    if (sScreenPaneOwnerTypeInitialized == 0) {
        u32 parentInitialized = sScreenPaneOwnerParentInitialized;
        sScreenPaneOwnerTypeInitialized = 1;
        if (parentInitialized == 0) {
            sScreenPaneOwnerParentInitialized = 1;
            sScreenPaneOwnerParent.mParent = NULL;
        }
        sScreenPaneOwnerType.mParent = &sScreenPaneOwnerParent;
    }
    hdScreenPaneOwner* owner = screenPaneOwnerCast(object);
    void* target = hdScreenReadTarget(owner);
    if (target == NULL) return;
    hdScreenHeap* heap = sScreenUiSystem->mHeap;
    void* child = hdScreenAllocate(0x3C, heap, 4);
    if (child != NULL) child = hdScreenConstructChild(child, argument, owner, i_this->mOwner);
    hdScreenAddChild(target, child);
}
/* 02002604 */
static void __sinit_hd_holder_cpp() {
    sinit_header_statics_rodata(0x101FF178, 0x1018C230, 0x101FF184, 0x100003A0);
}
/* 02002698 */
static hdScreenPicture* constructScreenPicture(hdScreenPicture* i_this, u32 resource, u32 option, u32 argument) {
    if (i_this == NULL) {
        i_this = (hdScreenPicture*)operator_new(sizeof(hdScreenPicture));
        if (i_this == NULL) return NULL;
    }
    hdScreenConstructPictureBase(i_this, resource, option, argument);
    i_this->mVtable = 0x100003C8;
    return i_this;
}
/* 0200271C */
static void destroyScreenPicture(hdScreenPicture* i_this, s32 flags) {
    if (i_this == NULL) return;
    hdScreenDestroyPictureBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 02002770 */
static void __sinit_hd_picture_cpp() {
    sinit_header_statics_rodata(0x101FF194, 0x1018C254, 0x101FF1A0, 0x100003C0);
}
/* 02002804 */
static hdScreenType* getLayoutScreenType() {
    if (sScreenTypeInitialized != 0) return &sScreenType;
    u32 parentInitialized = sScreenParentInitialized;
    sScreenTypeInitialized = 1;
    if (parentInitialized != 0) { sScreenType.mParent = &sScreenParent; return &sScreenType; }
    u32 rootInitialized = sScreenRootTypeInitialized;
    sScreenParentInitialized = 1;
    if (rootInitialized == 0) { sScreenRootTypeInitialized = 1; sScreenRootType.mParent = NULL; }
    sScreenType.mParent = &sScreenParent;
    sScreenParent.mParent = &sScreenRootType;
    return &sScreenType;
}
/* 02002888 */
static hdLayoutScreen* constructLayoutScreen(hdLayoutScreen* i_this) {
    if (i_this == NULL) {
        i_this = (hdLayoutScreen*)operator_new(sizeof(hdLayoutScreen));
        if (i_this == NULL) return NULL;
    }
    i_this->mHolder = NULL;
    i_this->mAccessor = NULL;
    i_this->mVtable = &sLayoutScreenTable;
    i_this->field_0x0 = 0;
    hdScreenConstructLytObject(i_this->lytObject());
    hdScreenPaneArray* panes = i_this->panesForConstruction();
    i_this->mAnimationSet = NULL;
    if (panes == NULL) panes = (hdScreenPaneArray*)operator_new(sizeof(hdScreenPaneArray));
    if (panes != NULL) { panes->mData = NULL; panes->mCount = 0; }
    return i_this;
}
/* 02002918 */
static void destroyLayoutScreen(hdLayoutScreen* i_this, s32 flags) {
    if (i_this == NULL) return;
    hdScreenDestroyLytObject(i_this->lytObject(), 2);
    if (flags & 1) operator_delete(i_this);
}
/* 0200296C */
static void setLayoutScreenResource(hdLayoutScreen* i_this, void* source) {
    if (source == NULL) return;
    void* resource = hdScreenConvertResource(source);
    hdScreenSetLytResource(i_this->lytObject(), resource);
}
/* 020029AC */
static void setLayoutScreenBlock(hdLayoutScreen* i_this, be<u32>* source) {
    if (source == NULL) return;
    for (u32 i = 0; i < 12; ++i) i_this->mLytObject.mBlock[i] = source[i];
}
/* 020029EC */
static void setupLayoutScreen(hdLayoutScreen* i_this, void* resource, be<u32>* block) {
    i_this->mLytObject.mSystemWord = sScreenUiSystem->mSystemWord;
    hdScreenSetResource(i_this, resource);
    hdScreenSetBlock(i_this, block);
}
/* 02002A40 */
static s32 buildLayoutScreen(hdLayoutScreen* i_this, u32 argument, u32 option, void* resource, be<u32>* block, u32 first, u32 second, hdScreenArchive* archive) {
    (void)argument; (void)option;
    if (archive == NULL) {
        if (gabi::call_ptr<u32>(i_this->mVtable->mInitialize.get(), i_this) == 0) return 0;
        if (gabi::call_ptr<u32>(i_this->mVtable->mInitializeOptions.get(), i_this, first, second) == 0) return 0;
        hdScreenSetup(i_this, resource, block);
        return 1;
    }
    if (archive->mAvailable == 0) { hdScreenSetup(i_this, resource, block); return 1; }
    if (gabi::call_ptr<u32>(i_this->mVtable->mInitializeArchive.get(), i_this, archive) == 0) return 0;
    if (gabi::call_ptr<u32>(i_this->mVtable->mInitializeOptions.get(), i_this, first, second) == 0) return 0;
    gabi::call_ptr(i_this->mVtable->mFinishArchiveFirst.get(), i_this, archive);
    gabi::call_ptr(i_this->mVtable->mFinishArchiveSecond.get(), i_this, archive);
    hdScreenSetup(i_this, resource, block);
    return 1;
}
/* 02002B84 */
static void releaseLayoutScreen(hdLayoutScreen* i_this) {
    s32 count = i_this->mPanes.mCount;
    if (count > 0 && i_this->mPanes.mData != NULL) {
        gptr<void>* data = i_this->mPanes.mData;
        hdScreenHeap* heap = hdScreenFindHeap(sScreenHeapManager, data);
        heap->free(i_this->mPanes.mData);
        i_this->mPanes.mCount = 0;
        i_this->mPanes.mData = NULL;
    }
    if (i_this->mAnimationSet != NULL) {
        hdScreenReleaseAnimationSet(i_this->mAnimationSet);
        hdScreenAnimationSet* set = i_this->mAnimationSet;
        if (set != NULL) set->destroy(3);
        i_this->mAnimationSet = NULL;
    }
    hdScreenHolder* holder = i_this->mHolder;
    if (holder != NULL) { holder->destroy(3); i_this->mHolder = NULL; }
    hdScreenAccessor* accessor = i_this->mAccessor;
    if (hdScreenAccessorBusy(accessor) == 0 && accessor != NULL) {
        accessor->destroy(3);
        i_this->mAccessor = NULL;
    }
}
/* 02002C90 */
static void drawLayoutScreen(hdLayoutScreen* i_this) {
    hdScreenAccessor* accessor = i_this->mAccessor;
    if (hdScreenAccessorBusy(accessor) != 0) return;
    accessor->prepareDraw();
    accessor = i_this->mAccessor;
    accessor->draw(i_this->lytObject());
    accessor = i_this->mAccessor;
    accessor->finishDraw();
}
/* 02002D0C */
static u32 calcLayoutScreen(hdLayoutScreen* i_this) {
    return gabi::call_ptr<u32>(i_this->mVtable->mCalc.get(), i_this, i_this->lytObject());
}
/* 02002D20 */
static void drawLayoutScreenWithInfo(hdLayoutScreen* i_this, void* info) {
    hdScreenAccessor* accessor = i_this->mAccessor;
    if (hdScreenAccessorBusy(accessor) != 0) return;
    hdScreenDrawInfo* drawInfo = sScreenUiSystem->mDrawInfo;
    drawInfo->field_0x0 = 0;
    drawInfo->field_0x1 = 0;
    drawInfo = sScreenUiSystem->mDrawInfo;
    drawInfo->mMode = 2;
    hdScreenPrepareDrawInfo(sScreenUiSystem->mDrawInfo);
    accessor = i_this->mAccessor;
    accessor->drawWithInfo(info);
}
/* 02002DB4 */
static u32 loadLayoutScreen(hdLayoutScreen* i_this, hdScreenString* name, u32 argument) {
    hdScreenHeap* heap = sScreenUiSystem->mHeap;
    hdScreenAccessor* accessor = (hdScreenAccessor*)hdScreenAllocate(0x248, heap, 4);
    if (accessor != NULL) accessor = hdScreenConstructAccessor(accessor);
    i_this->mAccessor = accessor;
    if (accessor == NULL) return 0;
    hdScreenFixedString buffer;
    hdScreenStringTable* nameTable = name->mVtable;
    buffer.mBuffer[63] = 0;
    buffer.mBuffer[0] = 0;
    buffer.mCapacity = 64;
    buffer.mText = buffer.mBuffer;
    buffer.mVtable = &sScreenFixedStringTable;
    gabi::call_ptr(nameTable->mAssureTermination.get(), name);
    hdScreenFormatName(&buffer, "%s.bflyt", name->mText);
    hdScreenHolder* holder = (hdScreenHolder*)hdScreenAllocate(sizeof(hdScreenHolder), heap, 4);
    if (holder != NULL) holder = hdScreenConstructHolder(holder, i_this, i_this->mAccessor);
    i_this->mHolder = holder;
    if (holder == NULL) return 0;
    buffer.assureTermination();
    return hdScreenLoadLayout(i_this->mAccessor, (const char*)buffer.mText.get(), argument, i_this->mHolder, 0);
}
/* 02002F04 */
static u32 getScreenPair(hdScreenPair* output, hdScreenPairSource* source) {
    u32 second = source->mSecond;
    output->mSecond = second;
    if (second == 0) return 0;
    output->mFirst = source->mFirst;
    return 1;
}
/* 02002F30 */
static u32 createScreenAnimationSet(hdLayoutScreen* i_this, u32 entryCount, u32 slotCount) {
    if (entryCount == 0 || slotCount == 0) return 1;
    hdScreenAnimationSet* set = (hdScreenAnimationSet*)hdScreenAllocate(sizeof(hdScreenAnimationSet), sScreenUiSystem->mHeap, 4);
    if (set != NULL) set = hdScreenConstructAnimationSet(set);
    i_this->mAnimationSet = set;
    if (set == NULL) return 0;
    return hdScreenInitializeAnimationSet(set, i_this->mAccessor, entryCount, slotCount);
}
/* 02002FE4 */
static void bindScreenPanes(hdLayoutScreen* i_this, hdScreenResource* resource) {
    u32 count = resource->mNames.mPaneCount;
    if (count == 0) return;
    hdScreenHeap* heap = sScreenUiSystem->mHeap;
    if (heap == NULL) heap = hdScreenGetCurrentHeap(sScreenHeapManager);
    void* allocation = heap->allocate(count << 2);
    if (allocation != NULL) {
        i_this->mPanes.mData = (gptr<void>*)allocation;
        i_this->mPanes.mCount = count;
    }
    s32 tableCount = i_this->mPanes.mCount;
    for (s32 i = 0; i < tableCount; ++i) *hdScreenPaneArray::element(i_this->mPanes.mData, i) = NULL;
    u32 remaining = count > 1 ? count : 1;
    for (u32 i = 0; remaining != 0; --remaining, ++i) {
        const be<u8>* name = (const be<u8>*)hdScreenPaneName(&resource->mNames, i);
        if (name == NULL || *name == 0) continue;
        hdScreenResourceObject* object = i_this->mAccessor->mObject;
        u32 size = i_this->mPanes.mCount;
        u32 target = object->mVtable->mFindPane.get();
        gptr<void>* data = i_this->mPanes.mData;
        gptr<void>* slot = hdScreenPaneArray::element(data, i < size ? i : 0);
        void* pane = gabi::call_ptr<void*>(target, object, (const char*)name, 1);
        *slot = pane;
    }
}
/* 02003130 */
static void bindScreenAnimations(hdLayoutScreen* i_this, hdScreenResource* resource) {
    u32 count = resource->mNames.mAnimationCount;
    if (count == 0) return;
    u32 remaining = count > 1 ? count : 1;
    hdScreenString name;
    for (u32 i = 0; remaining != 0; --remaining, ++i) {
        const be<u8>* text = (const be<u8>*)hdScreenAnimationName(&resource->mNames, i);
        if (text == NULL || *text == 0) continue;
        hdScreenAnimationSet* set = i_this->mAnimationSet;
        name.mVtable = &sScreenSafeStringTable;
        name.mText = (const char*)text;
        hdScreenBindAnimation(set, i, &name);
    }
}
// HD: compiler-generated and template code of these translation units.
/* 020031F4 */
static void __sinit_hd_layout_screen_cpp() {
    sinit_header_statics_rodata(0x101FF1B0, 0x1018C278, 0x101FF1BC, 0x100004EC);
}
/* 02003288 */
static void destroyScreenCompanionFirst(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0200329C */
static void assureScreenStringNone() {}
/* 020032A0 */
static void terminateScreenString(hdScreenFixedString* i_this) { i_this->terminate(); }
/* 020032B8 */
static void destroyScreenCompanionSecond(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020032CC */
static void destroyScreenCompanionThird(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020032E0 */
static void destroyScreenCompanionFourth(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* WWHD glue (tools/verify/autoglue.py): HD screen constants and external layout calls
 data sScreenHolderTypeInitialized 101FD8F0
 data sScreenHolderParentInitialized 101FD89C
 data sScreenRootTypeInitialized 101FD888
 data sScreenPaneOwnerTypeInitialized 101FD8C4
 data sScreenPaneOwnerParentInitialized 101FD894
 data sScreenTypeInitialized 101FD908
 data sScreenParentInitialized 101FD898
 data sScreenHolderType 101FD8F4
 data sScreenHolderParent 101FD8F8
 data sScreenRootType 101FD8FC
 data sScreenPaneOwnerType 101FD900
 data sScreenPaneOwnerParent 101FD904
 data sScreenType 101FD90C
 data sScreenParent 101FD910
 data sScreenUiSystem 1018C404
 data sScreenHeapManager 101F8B4C
 data sScreenFixedStringTable 100004C8
 data sScreenSafeStringTable 10000480
 data sScreenHolderTable 100003A8
 data sLayoutScreenTable 100004F8
 str 100004E0 "%s.bflyt"
 ext 0287795C void hdScreenConstructTextBoxBase(hdScreenTextBox*, u32, u32, u32)
 ext 02877DB8 void hdScreenConstructTextBoxResource(hdScreenTextBox*, u32)
 ext 02877FE8 void hdScreenDestroyTextBoxBase(hdScreenTextBox*, u32)
 ext 02874AAC void hdScreenConstructPictureBase(hdScreenPicture*, u32, u32, u32)
 ext 02874C64 void hdScreenDestroyPictureBase(hdScreenPicture*, u32)
 ext 0273B050 void* hdScreenAllocate(u32, hdScreenHeap*, u32)
 ext 02005978 void* hdScreenConstructChild(void*, void*, hdScreenPaneOwner*, void*)
 ext 020042A4 void hdScreenAddChild(void*, void*)
 ext 02873D70 void hdScreenConstructLytObject(hdScreenLytObject*)
 ext 02873FD0 void hdScreenDestroyLytObject(hdScreenLytObject*, u32)
 ext 0274D83C void* hdScreenConvertResource(void*)
 ext 02873FE4 void hdScreenSetLytResource(hdScreenLytObject*, void*)
 ext 0200296C void hdScreenSetResource(hdLayoutScreen*, void*)
 ext 020029AC void hdScreenSetBlock(hdLayoutScreen*, be<u32>*)
 ext 020029EC void hdScreenSetup(hdLayoutScreen*, void*, be<u32>*)
 ext 02755FEC hdScreenHeap* hdScreenFindHeap(void*, void*)
 ext 02756140 hdScreenHeap* hdScreenGetCurrentHeap(void*)
 ext 02004CAC void hdScreenReleaseAnimationSet(hdScreenAnimationSet*)
 ext 0286DFB4 void hdScreenPrepareDrawInfo(hdScreenDrawInfo*)
 ext 02003D84 hdScreenAccessor* hdScreenConstructAccessor(hdScreenAccessor*)
 ext 02759C28 void hdScreenFormatName(hdScreenFixedString*, const char*, const char*)
 ext 02002450 hdScreenHolder* hdScreenConstructHolder(hdScreenHolder*, void*, hdScreenAccessor*)
 ext 0287F640 u32 hdScreenLoadLayout(hdScreenAccessor*, const char*, u32, hdScreenHolder*, u32)
 ext 02004AC0 hdScreenAnimationSet* hdScreenConstructAnimationSet(hdScreenAnimationSet*)
 ext 02004B70 u32 hdScreenInitializeAnimationSet(hdScreenAnimationSet*, hdScreenAccessor*, u32, u32)
 ext 02873D24 const char* hdScreenPaneName(hdScreenNameTable*, u32)
 ext 02873D48 const char* hdScreenAnimationName(hdScreenNameTable*, u32)
 ext 02004F50 void hdScreenBindAnimation(hdScreenAnimationSet*, u32, hdScreenString*)
*/
