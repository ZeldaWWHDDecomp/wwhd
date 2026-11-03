// Manager update modes and the original observation-only name walk.
#include "agl/EnvironmentManager.h"
using namespace agl::env;
// The update calls pass the manager's dirty bit 8 as a second argument word.
void updateEnvironmentManagerModeZero(agl::HierarchyNode*,u32);
void updateEnvironmentManagerModeTwo(agl::HierarchyNode*,u32);
void sortEnvironmentManagerSetClass(EnvironmentReferenceList*,u32);
/* 0276B3E0 */
void EnvironmentManager::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 referenceCount = u32(mReferences.mReferences.mCount);
    EnvironmentReference* reference = mReferences.mReferences.mData;
    EnvironmentReference* referenceEnd = reference + referenceCount;
    while (gabi::ea(reference) != gabi::ea(referenceEnd)) ++reference;
    u32 iconCount = u32(mIcons.mCount);
    EnvironmentClassIcon* icon = mIcons.mData;
    EnvironmentClassIcon* iconEnd = icon + iconCount;
    while (gabi::ea(icon) != gabi::ea(iconEnd)) ++icon;
    mParameters.rebuildEntries();
    u32 mode = mMode;
    if (mode == 0) {
        updateEnvironmentManagerModeZero(&mParameters,(u32(mDirtyFlags) >> 8) & 1);
        return;
    }
    if (mode == 2) {
        updateEnvironmentManagerModeTwo(&mParameters,(u32(mDirtyFlags) >> 8) & 1);
        return;
    }
    if (mode != 1) return;
    EnvironmentClassIcon* firstIcon = mIcons.mData;
    iconEnd = firstIcon + u32(mIcons.mCount);
    for (icon = firstIcon; gabi::ea(icon) != gabi::ea(iconEnd); ++icon) {
        u32 typeId = (gabi::ea(icon) - gabi::ea(firstIcon)) / sizeof(EnvironmentClassIcon);
        u32 rangeCount = u32(mReferences.mRanges.mCount);
        EnvironmentReferenceRange* range = mReferences.mRanges.mData;
        if (typeId < rangeCount) range += typeId;
        if (range->mCount == 0) continue;
        sortEnvironmentManagerSetClass(&mEnvironmentSet.mReferences,typeId);
        rangeCount = u32(mEnvironmentSet.mReferences.mRanges.mCount);
        range = mEnvironmentSet.mReferences.mRanges.mData;
        if (typeId < rangeCount) range += typeId;
        u32 count = u32(mEnvironmentSet.mReferences.mReferences.mCount);
        u32 beginIndex = range->mFirst;
        if (beginIndex > count) beginIndex = count;
        EnvironmentReference* data = mEnvironmentSet.mReferences.mReferences.mData;
        EnvironmentReference* begin = data + beginIndex;
        u32 endIndex = u32(range->mFirst) + u32(range->mCount);
        if (endIndex > count) endIndex = count;
        EnvironmentReference* end = data + endIndex;
        for (reference = begin; gabi::ea(reference) != gabi::ea(end); ++reference) {
            EnvironmentObject* object = reference->mObject;
            if (object == NULL) continue;
            s32 selectedIndex = mParameters.mSelection;
            if (selectedIndex == -1) continue;
            u32 entryCount = u32(mParameters.mItems.mCount);
            agl::HierarchyItem* selected = mParameters.mItems.mData;
            if (u32(selectedIndex) < entryCount) selected += selectedIndex;
            // The object's hierarchy leaf interface: its group query answers the object's name parameter.
            agl::HierarchyLeafPrefix* leaf = reinterpret_cast<agl::HierarchyLeafPrefix*>(&object->mParameterIoMethods);
            sead::SafeText* objectName = gabi::call_ptr<sead::SafeText*>(leaf->mTable->mGroup,leaf);
            selected->mName.assureTermination();
            selected->mName.assureTermination();
            const be<u8>* selectedText = selected->mName.mText;
            objectName->assureTermination();
            if (gabi::ea(selectedText) == gabi::ea(static_cast<be<u8>*>(objectName->mText))) continue;
            const be<u8>* left = selected->mName.mText;
            const be<u8>* right = objectName->mText;
            // The result is unused, but the original still performs the bounded reads and callbacks.
            for (u32 index = 0; index <= u32(sead::kMaxTextLength); ++index) {
                u32 leftCharacter = left[index];
                u32 rightCharacter = right[index];
                if (leftCharacter != rightCharacter || leftCharacter == 0) break;
            }
        }
    }
}
/* WWHD glue
 ext 0279C2AC void agl::HierarchyNode::rebuildEntries()
 ext 0279C494 void updateEnvironmentManagerModeZero(agl::HierarchyNode*,u32)
 ext 0279C5D0 void updateEnvironmentManagerModeTwo(agl::HierarchyNode*,u32)
 ext 0276F814 void sortEnvironmentManagerSetClass(EnvironmentReferenceList*,u32)
*/
