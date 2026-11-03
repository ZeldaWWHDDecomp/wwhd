// Resource endpoints, class/object selection and parameter application in original callback order.
#include "agl/EnvironmentManager.h"
#include "agl/EnvironmentBlendResource.h"
#include <atomic>
using namespace agl::env;
typedef be<u32> EnvironmentBlendWord;
typedef be<u8> EnvironmentBlendTextByte;
static EnvironmentBlendWord sBlendNameReady;
static EnvironmentBlendWord sBlendGroupReady;
static EnvironmentBlendWord sBlendNameHash;
static EnvironmentBlendWord sBlendGroupHash;
static sead::SafeTextVtable sBlendTextMethods;
static EnvironmentBlendTextByte sBlendNameKey;
static EnvironmentBlendTextByte sBlendGroupKey;
static f32 sBlendZero;
static f32 sBlendOne;
u32 hashEnvironmentBlendKey(const sead::SafeText*);
agl::parameter_tree::InputRecord_l* environmentBlendChild(EnvironmentBlendListReference*,u32);
EnvironmentObjectRecord* environmentBlendObject(EnvironmentBlendListReference*,u32);
sead::SafeText* environmentBlendFallbackName();
void applySingleEnvironmentBlend(EnvironmentObject*,u32,const EnvironmentResourceReference*,const EnvironmentResourceReference*,u32,f64);
void applyPairedEnvironmentBlend(EnvironmentObject*,const EnvironmentResourceReference*,const EnvironmentResourceReference*,u32,f64);
namespace {
static agl::parameter_tree::InputRecord_l* blendRoot(agl::ParameterRootInput_l* resource) {
    // The original reads the root offset even when the selected resource word is zero.
    u32 rootOffset = lowMemoryView(resource)->mPayloadOffset;
    return relativeNode<agl::parameter_tree::InputRecord_l>(resource, rootOffset + sizeof(agl::ParameterRootInput_l));
}
static agl::parameter_tree::InputRecord_l* blendChildren(EnvironmentBlendListReference* reference,u32& count) {
    agl::parameter_tree::InputRecord_l* first = NULL;
    count = reference->mList->mChildCount;
    if (count != 0) {
        first = environmentBlendChild(reference,0);
        count = reference->mList->mChildCount;
    }
    if (count != 0) {
        environmentBlendChild(reference,count - 1);
        count = reference->mList->mChildCount;
    }
    return first;
}
static EnvironmentObjectRecord* blendObjects(EnvironmentBlendListReference* reference,u32& count) {
    EnvironmentObjectRecord* first = NULL;
    count = reference->mList->mValueCount;
    if (count != 0) {
        first = environmentBlendObject(reference,0);
        count = reference->mList->mValueCount;
    }
    if (count != 0) {
        environmentBlendObject(reference,count - 1);
        count = reference->mList->mValueCount;
    }
    return first;
}
static EnvironmentSetResourceEntry* blendParameter(EnvironmentObjectRecord* object,u32 count,u32 nameHash) {
    EnvironmentSetResourceEntry* parameter = object->firstParameter();
    for (u32 index = 0; index != count; ++index) {
        if (u32(parameter->mNameHash) == nameHash) return parameter;
        parameter = parameter->next();
    }
    return NULL;
}
static be<u8>* blendParameterText(EnvironmentSetResourceEntry* parameter) {
    return parameter->text();
}
static bool blendTextEqual(sead::SafeText* left,sead::SafeText* right) {
    left->assureTermination();
    left->assureTermination();
    const be<u8>* cachedLeft = left->mText;
    right->assureTermination();
    if (gabi::ea(cachedLeft) == gabi::ea(static_cast<be<u8>*>(right->mText))) return true;
    const be<u8>* leftText = left->mText;
    const be<u8>* rightText = right->mText;
    for (u32 index = 0; index <= u32(sead::kMaxTextLength); ++index) {
        u32 character = leftText[index];
        if (character != u32(rightText[index])) return false;
        if (character == 0) return true;
    }
    return false;
}
static void blendSingle(EnvironmentObject* object,EnvironmentObjectRecord* resource,f32 zero,u32 referenceOffset) {
    gabi::FrameLocal<EnvironmentResourceReference> first(referenceOffset);
    gabi::FrameLocal<EnvironmentResourceReference> second(referenceOffset + 4);
    first->mObject = reinterpret_cast<EnvironmentResourceObject*>(resource);
    second->mObject = NULL;
    applySingleEnvironmentBlend(object,0,first.get(),second.get(),0,zero);
}
}
/* 0276B710 */
void EnvironmentManager::blendResources(const EnvironmentArchiveReference* first,const EnvironmentArchiveReference* second,s32 selectedEntry,f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ParameterRootInput_l* firstResource = first->mArchive;
    agl::ParameterRootInput_l* secondResource = second->mArchive;
    // The original keeps incoming f1 in a double-width FPR through comparisons and calls.
    f32 zero = sBlendZero;
    gabi::FrameLocal<EnvironmentArchiveReference> from(0xC);
    gabi::FrameLocal<EnvironmentArchiveReference> to(0x1C);
    to->mArchive = secondResource;
    if (!(weight > zero)) {
        from->mArchive = firstResource;
        to->mArchive = firstResource;
    } else if (sBlendOne > weight) {
        from->mArchive = firstResource;
    } else {
        from->mArchive = secondResource;
    }
    gabi::FrameLocal<sead::SafeText> key(0xB8);
    if (sBlendNameReady == 0) {
        sBlendNameReady = 1;
        key->mVtable = &sBlendTextMethods;
        key->mText = &sBlendNameKey;
        sBlendNameHash = hashEnvironmentBlendKey(key.get());
    }
    if (sBlendGroupReady == 0) {
        sBlendGroupReady = 1;
        key->mVtable = &sBlendTextMethods;
        key->mText = &sBlendGroupKey;
        sBlendGroupHash = hashEnvironmentBlendKey(key.get());
    }
    gabi::FrameLocal<EnvironmentBlendListReference> root(0xB4);
    root->mList = blendRoot(from->mArchive);
    u32 childCount;
    agl::parameter_tree::InputRecord_l* child = blendChildren(root.get(),childCount);
    for (u32 childIndex = 0; childIndex != childCount; ++childIndex) {
        EnvironmentClassIcon* registeredClass = static_cast<EnvironmentClassIcon*>(static_cast<agl::parameter_tree::Node*>(mIo.mTree.mChildren));
        u32 typeId = 0;
        if (registeredClass != NULL) {
            u32 childName = child->mNameHash;
            while (registeredClass != NULL && u32(registeredClass->mNameHash) != childName) {
                registeredClass = registeredClass->nextClass();
                ++typeId;
            }
        }
        if (registeredClass != NULL) {
            s32 lastFallbackIndex = -1;
            gabi::FrameLocal<EnvironmentBlendListReference> childReference(0x70);
            childReference->mList = child;
            u32 objectCount;
            EnvironmentObjectRecord* resource = blendObjects(childReference.get(),objectCount);
            for (u32 objectIndex = 0; objectIndex != objectCount; ++objectIndex) {
                u32 parameterCount = resource->mParameterCount;
                EnvironmentSetResourceEntry* groupParameter = blendParameter(resource,parameterCount,sBlendGroupHash);
                EnvironmentSetResourceEntry* nameParameter = blendParameter(resource,parameterCount,sBlendNameHash);
                if (groupParameter != NULL && nameParameter != NULL) {
                    bool groupAccepted = true;
                    if (selectedEntry != -1) {
                        gabi::FrameLocal<sead::SafeText> group(0x68);
                        group->mText = blendParameterText(groupParameter);
                        group->mVtable = &sBlendTextMethods;
                        agl::HierarchyItem* selected = mParameters.mItems.mData;
                        if (u32(selectedEntry) < u32(mParameters.mItems.mCount)) selected += selectedEntry;
                        groupAccepted = blendTextEqual(&selected->mName,group.get());
                    }
                    if (groupAccepted) {
                        gabi::FrameLocal<sead::SafeText> name(0x30);
                        name->mText = blendParameterText(nameParameter);
                        name->mVtable = &sBlendTextMethods;
                        u32 rangeCount = u32(mReferences.mRanges.mCount);
                        const EnvironmentReferenceRange* range = mReferences.mRanges.mData;
                        if (typeId < rangeCount) range += typeId;
                        u32 referenceCount = u32(mReferences.mReferences.mCount);
                        EnvironmentReference* data = mReferences.mReferences.mData;
                        u32 begin = range->mFirst;
                        if (begin > referenceCount) begin = referenceCount;
                        u32 end = u32(range->mFirst) + u32(range->mCount);
                        if (end > referenceCount) end = referenceCount;
                        EnvironmentObject* selectedObject = NULL;
                        for (EnvironmentReference* reference = data + begin; gabi::ea(reference) != gabi::ea(data + end); ++reference) {
                            EnvironmentObject* object = reference->mObject;
                            if ((u32(object->mFlags) & 1) == 0) continue;
                            u32 referenceIndex = (gabi::ea(reference) - gabi::ea(data)) / sizeof(EnvironmentReference);
                            if (selectedObject == NULL && lastFallbackIndex < s32(referenceIndex)) {
                                sead::SafeText* fallbackName = gabi::call_ptr<sead::SafeText*>(object->mMethods->mNameText.mFunction,object);
                                sead::SafeText* defaultName = environmentBlendFallbackName();
                                if (blendTextEqual(fallbackName,defaultName)) {
                                    lastFallbackIndex = s32(referenceIndex);
                                    selectedObject = object;
                                }
                            }
                            sead::SafeText* objectName = gabi::call_ptr<sead::SafeText*>(object->mMethods->mGroupText.mFunction,object);
                            if (blendTextEqual(objectName,name.get())) { selectedObject = object; break; }
                        }
                        if (selectedObject != NULL) {
                            if (gabi::ea(static_cast<agl::ParameterRootInput_l*>(from->mArchive)) == gabi::ea(static_cast<agl::ParameterRootInput_l*>(to->mArchive))) {
                                blendSingle(selectedObject,resource,zero,0x94);
                            } else {
                                gabi::FrameLocal<EnvironmentBlendListReference> otherRoot(0x4C);
                                otherRoot->mList = blendRoot(to->mArchive);
                                u32 otherChildCount;
                                agl::parameter_tree::InputRecord_l* otherChild = blendChildren(otherRoot.get(),otherChildCount);
                                agl::parameter_tree::InputRecord_l* matchingChild = NULL;
                                if (otherChildCount != 0) {
                                    u32 childName = childReference->mList->mNameHash;
                                    for (u32 index = 0; index != otherChildCount; ++index) {
                                        if (u32(otherChild->mNameHash) == childName) { matchingChild = otherChild; break; }
                                        otherChild = nextBlendList(otherChild);
                                    }
                                }
                                if (matchingChild == NULL) {
                                    blendSingle(selectedObject,resource,zero,0x9C);
                                } else {
                                    gabi::FrameLocal<EnvironmentBlendListReference> matching(0x40);
                                    matching->mList = matchingChild;
                                    u32 otherObjectCount;
                                    EnvironmentObjectRecord* otherObject = blendObjects(matching.get(),otherObjectCount);
                                    for (u32 index = 0; index != otherObjectCount; ++index) {
                                        u32 otherParameterCount = otherObject->mParameterCount;
                                        EnvironmentSetResourceEntry* otherNameParameter = blendParameter(otherObject,otherParameterCount,sBlendNameHash);
                                        gabi::FrameLocal<sead::SafeText> otherName(0x38);
                                        otherName->mText = blendParameterText(otherNameParameter);
                                        otherName->mVtable = &sBlendTextMethods;
                                        if (blendTextEqual(name.get(),otherName.get())) {
                                            gabi::FrameLocal<EnvironmentResourceReference> left(0x44);
                                            gabi::FrameLocal<EnvironmentResourceReference> right(0x48);
                                            left->mObject = reinterpret_cast<EnvironmentResourceObject*>(resource);
                                            right->mObject = reinterpret_cast<EnvironmentResourceObject*>(otherObject);
                                            applyPairedEnvironmentBlend(selectedObject,left.get(),right.get(),0,weight);
                                            break;
                                        }
                                        otherObject = otherObject->next();
                                    }
                                }
                            }
                        }
                    }
                }
                resource = resource->next();
            }
        }
        child = nextBlendList(child);
    }
    mDirtyFlags = u32(mDirtyFlags) | 1;
    std::atomic_thread_fence(std::memory_order_acquire);
}
/* WWHD glue
 data sBlendNameReady 104A144C
 data sBlendGroupReady 104A1450
 data sBlendNameHash 104A1454
 data sBlendGroupHash 104A1458
 data sBlendTextMethods 1015FB48
 data sBlendNameKey 1015FC5C
 data sBlendGroupKey 1015FC54
 data sBlendZero 1015FC4C
 data sBlendOne 1015FC50
 ext 0279E57C u32 hashEnvironmentBlendKey(const sead::SafeText*)
 ext 027A73F4 agl::parameter_tree::InputRecord_l* environmentBlendChild(EnvironmentBlendListReference*,u32)
 ext 027A7424 EnvironmentObjectRecord* environmentBlendObject(EnvironmentBlendListReference*,u32)
 ext 027AE62C sead::SafeText* environmentBlendFallbackName()
 ext 027A8780 void applySingleEnvironmentBlend(EnvironmentObject*,u32,const EnvironmentResourceReference*,const EnvironmentResourceReference*,u32,f64)
 ext 027A8BAC void applyPairedEnvironmentBlend(EnvironmentObject*,const EnvironmentResourceReference*,const EnvironmentResourceReference*,u32,f64)
*/
