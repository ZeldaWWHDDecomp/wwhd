// Named hierarchy item construction and comparison reconstructed from assigned WWHD bodies.
#include "bindings.h"
#include "agl/HierarchyNode.h"
void* allocateHierarchyItem(u32);
void copyHierarchyItemBytes(void*, const void*, u32, u32);
void assureHierarchyItemEmptyText(sead::SafeText*);
sead::SafeText* hierarchyNameReference();
void setHierarchyFolderText(agl::HierarchyItem*, const sead::SafeText*);
static u32 sHierarchyItemTable;
static u32 sHierarchyItemPlainTextTable;
static u32 sHierarchyItemBufferBaseTable;
static u32 sHierarchyItemNameBaseTable;
static u32 sHierarchyItemNameTable;
static u32 sHierarchyItemDisplayBaseTable;
static u32 sHierarchyItemDisplayTable;
static u8 sHierarchyItemInitialName = 0;
static u8 sHierarchyItemResetDisplay = 0;
static s32 hierarchyItemTextLength(const be<u8>* text) {
    s32 length = 0;
    if (*text != 0) {
        do {
            ++length; ++text;
            if (length > sead::kMaxTextLength) return 0;
        } while (*text != 0);
    }
    return length;
}
/* 0279B9CC */
agl::HierarchyItem* agl::HierarchyItem::create(HierarchyItem* item) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (item == NULL) item = (HierarchyItem*)allocateHierarchyItem(sizeof(HierarchyItem));
    if (item == NULL) return item;
    item->mSecondWord = 0;
    item->mTable = reinterpret_cast<HierarchyItemDispatch*>(&sHierarchyItemTable);
    gabi::FrameLocal<sead::SafeText> empty(8);
    empty->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemPlainTextTable);
    empty->mText = gabi::at<be<u8>>(gabi::ea(&sHierarchyItemInitialName));
    auto* name = gabi::at<sead::FixedSafeText<32>>(gabi::ea(item) + offsetof(HierarchyItem, mName));
    if (name == NULL) name = (sead::FixedSafeText<32>*)allocateHierarchyItem(sizeof(sead::FixedSafeText<32>));
    if (name != NULL) {
        sead::SafeTextBuffer* base = name;
        if (base == NULL) base = (sead::SafeTextBuffer*)allocateHierarchyItem(sizeof(sead::SafeTextBuffer));
        if (base != NULL) {
            base->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemBufferBaseTable);
            base->mText = gabi::at<be<u8>>(gabi::ea(name) + offsetof(sead::FixedSafeText<32>, mStorage));
            base->mCapacity = 32;
            name->mStorage[31] = 0;
        }
        name->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemNameBaseTable);
        be<u8>* destination = name->mText;
        empty->assureTermination();
        s32 length = hierarchyItemTextLength(empty->mText);
        s32 capacity = name->mCapacity;
        if (length >= capacity) length = capacity - 1;
        empty->assureTermination();
        copyHierarchyItemBytes(destination, empty->mText, (u32)length, 0);
        destination[length] = 0;
        name->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemNameTable);
    }
    auto* display = gabi::at<sead::FixedSafeText<256>>(gabi::ea(item) + offsetof(HierarchyItem, mDisplay));
    if (display == NULL) display = (sead::FixedSafeText<256>*)allocateHierarchyItem(sizeof(sead::FixedSafeText<256>));
    if (display != NULL) {
        sead::SafeTextBuffer* base = display;
        if (base == NULL) base = (sead::SafeTextBuffer*)allocateHierarchyItem(sizeof(sead::SafeTextBuffer));
        if (base != NULL) {
            base->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemBufferBaseTable);
            base->mText = gabi::at<be<u8>>(gabi::ea(display) + offsetof(sead::FixedSafeText<256>, mStorage));
            base->mCapacity = 256;
            display->mStorage[255] = 0;
        }
        be<u8>* text = display->mText;
        display->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemDisplayBaseTable);
        *text = 0;
        display->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemDisplayTable);
    }
    return item;
}
/* 0279BB98 */
void agl::HierarchyItem::resetDisplay(u32 first, u32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> empty(8);
    empty->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemPlainTextTable);
    empty->mText = gabi::at<be<u8>>(gabi::ea(&sHierarchyItemResetDisplay));
    be<u8>* destination = mDisplay.mText;
    assureHierarchyItemEmptyText(empty.get());
    s32 length = hierarchyItemTextLength(empty->mText);
    s32 capacity = mDisplay.mCapacity;
    if (length >= capacity) length = capacity - 1;
    empty->assureTermination();
    copyHierarchyItemBytes(destination, empty->mText, (u32)length, 0);
    destination[length] = 0;
    mFirstWord = first;
    mSecondWord = second;
}
/* 0279C00C */
void agl::HierarchyItem::setName(sead::SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* destination = mName.mText;
    name->assureTermination();
    s32 length = hierarchyItemTextLength(name->mText);
    s32 capacity = mName.mCapacity;
    if (length >= capacity) length = capacity - 1;
    name->assureTermination();
    copyHierarchyItemBytes(destination, name->mText, (u32)length, 0);
    destination[length] = 0;
    sead::SafeText* reference = hierarchyNameReference();
    name->assureTermination();
    name->assureTermination();
    be<u8>* cached = name->mText;
    reference->assureTermination();
    bool same = cached == (be<u8>*)reference->mText;
    if (!same) {
        be<u8>* first = name->mText;
        be<u8>* second = reference->mText;
        for (u32 index = 0; index <= (u32)sead::kMaxTextLength; ++index) {
            u8 left = first[index];
            u8 right = second[index];
            if (left != right) break;
            if (left == 0) { same = true; break; }
        }
    }
    if (same) {
        gabi::FrameLocal<sead::SafeText> icon(8);
        icon->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemPlainTextTable);
        icon->mText = gabi::at<be<u8>>(gabi::guest_str("Icon=FOLDER_RED"));
        setHierarchyFolderText(this, icon.get());
    } else {
        gabi::FrameLocal<sead::SafeText> icon(16);
        icon->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sHierarchyItemPlainTextTable);
        icon->mText = gabi::at<be<u8>>(gabi::guest_str("Icon=FOLDER_GREEN"));
        setHierarchyFolderText(this, icon.get());
    }
}
/* 0279C1B4 */
s32 agl::compareHierarchyItemNames(HierarchyItem* first, HierarchyItem* other) {
    gabi::AutoFrame frame{gabi::current_function()};
    first->mName.assureTermination();
    first->mName.assureTermination();
    be<u8>* ownText = first->mName.mText;
    other->mName.assureTermination();
    if (ownText == (be<u8>*)other->mName.mText) return 1;
    be<u8>* otherText = other->mName.mText;
    for (u32 index = 0; index < (u32)sead::kMaxTextLength; ++index) {
        u8 otherByte = otherText[index];
        if (otherByte == 0) return 1;
        be<u8>* text = first->mName.mText;
        u8 ownByte = text[index];
        if (ownByte == 0 || otherByte > ownByte) return -1;
        if (otherByte < ownByte) return 1;
    }
    return 1;
}
/* WWHD glue
 str 101677DC "Icon=FOLDER_RED"
 str 101677EC "Icon=FOLDER_GREEN"
 ext 027AE62C sead::SafeText* hierarchyNameReference()
 ext 02785CB4 void setHierarchyFolderText(agl::HierarchyItem*, const sead::SafeText*)
 data sHierarchyItemTable 10167840
 data sHierarchyItemPlainTextTable 101676E0
 data sHierarchyItemBufferBaseTable 101676F8
 data sHierarchyItemNameBaseTable 10167760
 data sHierarchyItemNameTable 10167778
 data sHierarchyItemDisplayBaseTable 10167730
 data sHierarchyItemDisplayTable 10167748
 data sHierarchyItemInitialName 101677D8
 data sHierarchyItemResetDisplay 101677D9
 ext 0273AD10 void* allocateHierarchyItem(u32)
 ext C0009988 void copyHierarchyItemBytes(void*, const void*, u32, u32)
 ext 0279C764 void assureHierarchyItemEmptyText(sead::SafeText*)
*/
