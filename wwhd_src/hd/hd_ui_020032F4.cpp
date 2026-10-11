/** HD list visitor, named animation entries and layout companions. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_ui_020032F4.h"
static hdUiVisitorTable sVisitorTable;
static hdUiStringTable sBaseStringTable;
static hdUiStringTable sBufferedStringTable;
static hdUiStringTable sFixedStringTable;
static f32 sInitialRate;
static f32 sStoppedFrame;
static u32 sAdaptorTypeInitialized;
static u32 sAdaptorParentInitialized;
static u32 sAdaptorRootInitialized;
static hdUiAdaptorType sAdaptorType;
static hdUiAdaptorType sAdaptorParent;
static hdUiAdaptorType sAdaptorRoot;

/* 020032F4 */
static hdUiVisitor* constructVisitor(hdUiVisitor* i_this) {
    if (i_this == NULL) {
        i_this = (hdUiVisitor*)operator_new(sizeof(hdUiVisitor));
        if (i_this == NULL) return NULL;
    }
    i_this->field_0x0 = 0;
    i_this->mVtable = &sVisitorTable;
    return i_this;
}
/* 0200333C */
static void destroyVisitor(hdUiVisitor* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 02003350 */
static u32 visitAll(hdUiVisitor* i_this, hdUiVisitorList* list) {
    gptr<void>* element = list->mElements;
    u32 end = gabi::ea(element) + (u32)list->mCount * sizeof(gptr<void>);
    while (gabi::ea(element) != end) {
        if (i_this->visit(*element) == 0) return 0;
        element = hdUiEntryAdvanceElement(element);
        end = gabi::ea(list->mElements.get()) + (u32)list->mCount * sizeof(gptr<void>);
    }
    return 1;
}
/* 02003404 */
static void __sinit_hd_ui_visitor_cpp() {
    sinit_header_statics_rodata(0x101FF1CC, 0x1018C29C, 0x101FF1D8, 0x10000558);
}
/* 02003498 */
static u32 acceptVisitorElement() { return 1; }
/* 020034A0 */
static hdUiAnimationEntry* constructAnimationEntry(hdUiAnimationEntry* i_this, void* owner, hdUiSafeString* name, hdUiFrameController* controller) {
    if (i_this == NULL) {
        i_this = (hdUiAnimationEntry*)operator_new(sizeof(hdUiAnimationEntry));
        if (i_this == NULL) return NULL;
    }
    i_this->mOwner = owner;
    i_this->mVtable = 0x100005F8;
    i_this->mRate = sInitialRate;
    hdUiFixedString* string = i_this->nameForConstruction();
    if (string == NULL) string = (hdUiFixedString*)operator_new(sizeof(hdUiFixedString));
    if (string != NULL) {
        string->mText = string->mBuffer;
        string->mVtable = &sBaseStringTable;
        string->mCapacity = 32;
        string->mBuffer[31] = 0;
        string->mVtable = &sBufferedStringTable;
        be<u8>* buffer = string->mText;
        name->assureTermination();
        be<u8>* text = name->mText;
        s32 length = 0;
        if (*text != 0) {
            for (;;) {
                ++length;
                text = hdUiEntryAdvanceByte(text, 1);
                if (length > 0x40000) { length = 0; break; }
                if (*text == 0) break;
            }
        }
        s32 capacity = string->mCapacity;
        hdUiStringTable* sourceTable = name->mVtable;
        if (!(length < capacity)) length = capacity - 1;
        gabi::call_ptr(sourceTable->mAssureTermination.get(), name);
        hdUiEntryCopyBytes(buffer, name->mText, length, 0);
        *hdUiEntryAdvanceByte(buffer, length) = 0;
        string->mVtable = &sFixedStringTable;
    }
    i_this->mController = controller;
    return i_this;
}
/* 02003608 */
static void destroyAnimationEntry(hdUiAnimationEntry* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0200361C */
static u32 playAnimationEntry(hdUiAnimationEntry* i_this, f32 rate) {
    hdUiFrameController* controller = i_this->mController;
    i_this->mRate = rate;
    return controller->play();
}
/* 02003654 */
static u32 playAnimationEntryAlternate(hdUiAnimationEntry* i_this, f32 rate) {
    hdUiFrameController* controller = i_this->mController;
    i_this->mRate = rate;
    return controller->playAlternate();
}
/* 0200368C */
static u32 isAnimationEntryEnd(hdUiAnimationEntry* i_this) {
    hdUiFrameController* controller = i_this->mController;
    if (controller->mResource->mLoopMode == 1) return 0;
    if (!((f32)controller->mFrame == sStoppedFrame)) return 0;
    if (controller->mFlags & 1) return 0;
    return 1;
}
/* 020036D0 */
static void __sinit_hd_ui_animation_entry_cpp() {
    sinit_header_statics_rodata(0x101FF1E8, 0x1018C2C0, 0x101FF1F4, 0x100005EC);
}
/* 02003764 */
static void destroyFirstStringCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 02003778 */
static void destroySecondStringCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0200378C */
static void emptyStringCompanion() {}
/* 02003790 */
static void assureEntryStringTermination(hdUiFixedString* i_this) { i_this->terminate(); }
/* 020037A8 */
static void destroyThirdStringCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020037BC */
static void destroyFourthStringCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020037D0 */
static hdUiPartsPane* constructPartsPane(hdUiPartsPane* i_this, u32 resource, u32 option, u32 argument) {
    if (i_this == NULL) {
        i_this = (hdUiPartsPane*)operator_new(sizeof(hdUiPartsPane));
        if (i_this == NULL) return NULL;
    }
    hdUiEntryConstructPartsBase(i_this, resource, option, argument);
    i_this->mVtable = 0x10000610;
    return i_this;
}
/* 02003854 */
static void destroyPartsPane(hdUiPartsPane* i_this, s32 flags) {
    if (i_this == NULL) return;
    hdUiEntryDestroyPartsBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 020038A8 */
static void __sinit_hd_ui_parts_pane_cpp() {
    sinit_header_statics_rodata(0x101FF204, 0x1018C2E4, 0x101FF210, 0x10000608);
}
/* 0200393C */
static hdUiAdaptorType* getAdaptorType() {
    if (sAdaptorTypeInitialized != 0) return &sAdaptorType;
    u32 parentInitialized = sAdaptorParentInitialized;
    sAdaptorTypeInitialized = 1;
    if (parentInitialized != 0) {
        sAdaptorType.mParent = &sAdaptorParent;
        return &sAdaptorType;
    }
    u32 rootInitialized = sAdaptorRootInitialized;
    sAdaptorParentInitialized = 1;
    if (rootInitialized == 0) {
        sAdaptorRootInitialized = 1;
        sAdaptorRoot.mParent = NULL;
    }
    sAdaptorType.mParent = &sAdaptorParent;
    sAdaptorParent.mParent = &sAdaptorRoot;
    return &sAdaptorType;
}
/* 020039C0 */
static hdUiAdaptor* constructAdaptor(hdUiAdaptor* i_this, hdUiAdaptorTarget* target) {
    if (i_this == NULL) {
        i_this = (hdUiAdaptor*)operator_new(sizeof(hdUiAdaptor));
        if (i_this == NULL) return NULL;
    }
    i_this->mTarget = target;
    i_this->mVtable = 0x100006C8;
    return i_this;
}
/* 02003A10 */
static u32 forwardAdaptorFirst(hdUiAdaptor* i_this, u32 first, u32 fallback, u32 third) {
    hdUiAdaptorTarget* target = i_this->mTarget;
    return gabi::call_ptr<u32>(target->mVtable->mForwardFirst.get(), target, first != 0 ? first : fallback, fallback, third);
}
/* 02003A30 */
static u32 forwardAdaptorSecond(hdUiAdaptor* i_this, u32 first, u32 second, u32 third) {
    hdUiAdaptorTarget* target = i_this->mTarget;
    return gabi::call_ptr<u32>(target->mVtable->mForwardSecond.get(), target, first, second, third);
}
/* 02003A44 */
static void __sinit_hd_ui_adaptor_cpp() {
    sinit_header_statics_rodata(0x101FF220, 0x1018C308, 0x101FF22C, 0x100006C0);
}
/* 02003AD8 */
static void destroyAdaptorCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 02003AEC */
static hdUiWindowPane* constructWindowPane(hdUiWindowPane* i_this, u32 resource, u32 option, u32 argument) {
    if (i_this == NULL) {
        i_this = (hdUiWindowPane*)operator_new(sizeof(hdUiWindowPane));
        if (i_this == NULL) return NULL;
    }
    hdUiEntryConstructWindowBase(i_this, resource, option, argument);
    i_this->mVtable = 0x100006F8;
    return i_this;
}
/* 02003B70 */
static void destroyWindowPane(hdUiWindowPane* i_this, s32 flags) {
    if (i_this == NULL) return;
    hdUiEntryDestroyWindowBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 02003BC4 */
static void __sinit_hd_ui_window_pane_cpp() {
    sinit_header_statics_rodata(0x101FF23C, 0x1018C32C, 0x101FF248, 0x100006F0);
}
/* WWHD glue (tools/verify/autoglue.py): private UI constants and external base calls
 data sVisitorTable 10000560
 data sBaseStringTable 100005A0
 data sBufferedStringTable 100005B8
 data sFixedStringTable 100005D0
 data sInitialRate 10000580
 data sStoppedFrame 100005E8
 data sAdaptorTypeInitialized 101FD914
 data sAdaptorParentInitialized 101FD890
 data sAdaptorRootInitialized 101FD888
 data sAdaptorType 101FD918
 data sAdaptorParent 101FD91C
 data sAdaptorRoot 101FD8FC
 ext C0009988 void hdUiEntryCopyBytes(void*, const void*, u32, u32)
 ext 02874894 void hdUiEntryConstructPartsBase(hdUiPartsPane*, u32, u32, u32)
 ext 02874918 void hdUiEntryDestroyPartsBase(hdUiPartsPane*, u32)
 ext 02878F9C void hdUiEntryConstructWindowBase(hdUiWindowPane*, u32, u32, u32)
 ext 02879474 void hdUiEntryDestroyWindowBase(hdUiWindowPane*, u32)
*/
