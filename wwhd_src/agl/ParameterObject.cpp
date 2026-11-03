// agl parameter object: construction with an empty name, the parameter chain (append, find, pairwise copy),
// copying another object, and applying one parameter-object resource or blending two of them; parameters the
// resources do not cover are reported to the owning parameter list and its parent. Written from WWHD's
// instructions; resource layout: AAMP format facts.
#include "agl/ParameterObject.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void moveText(void* destination, const void* source, u32 size, u32 flags);
static parameter_tree::ValueVtable sObjectMethods;
static sead::SafeTextVtable sTextBufferMethods;
static sead::SafeTextVtable sFixedTextBase64Methods;
static sead::SafeTextVtable sFixedText64Methods;
static sead::SafeText sEmptyText;

/* inlined: length of a text, 0 when it is longer than sead's limit */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > sead::kMaxTextLength) return 0;
    }
    return length;
}

/* 027A84B8 */
ParameterObject* ParameterObject::create(ParameterObject* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterObject* object = storage;
    if (object == nullptr) object = static_cast<ParameterObject*>(allocateObject(sizeof(ParameterObject)));
    if (object == nullptr) return object;
    object->mPrefix = 0;
    object->mVtable = &sObjectMethods;
    sead::FixedSafeText<64>* name = &object->mName;
    if (name == nullptr) name = static_cast<sead::FixedSafeText<64>*>(allocateObject(sizeof(sead::FixedSafeText<64>)));
    if (name != nullptr) {
        sead::SafeTextBuffer* buffer = name;
        if (buffer == nullptr) buffer = static_cast<sead::SafeTextBuffer*>(allocateObject(sizeof(sead::SafeTextBuffer)));
        if (buffer != nullptr) {
            buffer->mText = name->mStorage;
            buffer->mVtable = &sTextBufferMethods;
            buffer->mCapacity = 64;
            name->mStorage[63] = 0;
        }
        be<u8>* firstCharacter = name->mText;
        name->mVtable = &sFixedTextBase64Methods;
        firstCharacter[0] = 0;
        name->mVtable = &sFixedText64Methods;
    }
    object->mNext = nullptr;
    sead::VtableSlot* assure = &sEmptyText.mVtable->mAssureTermination;
    be<u8>* destination = object->mName.mText;
    gabi::call_ptr<void>(assure->mFunction, &sEmptyText);
    const be<u8>* source = sEmptyText.mText;
    s32 length = textLength(source);
    s32 capacity = object->mName.mCapacity;
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr<void>(assure->mFunction, &sEmptyText);
    moveText(destination, source, u32(length), 0);
    destination[length] = 0;
    return object;
}

/* 027A861C */
void ParameterObject::appendParameter(ParameterBase* parameter) {
    ParameterBase* last = firstParameter();
    if (last != nullptr) {
        for (ParameterBase* next = last->mNext; next != nullptr; next = next->mNext) last = next;
    }
    if (last != nullptr) last->mNext = parameter;
    else mPrefix = gabi::ea(parameter);
}

/* 027A8650 */
void ParameterObject::copyParameters(ParameterBase* first, ParameterBase* last, ParameterBase* source, ParameterBase* sourceLast) {
    gabi::AutoFrame frame{gabi::current_function()};
    while (first != last && source != sourceLast) {
        gabi::call_ptr<u32>(first->methods()->mCopy.mFunction, first, source);
        source = source->mNext;
        first = first->mNext;
    }
}

/* 027A86D0 */
ParameterBase* ParameterObject::findParameter(u32 nameHash) {
    for (ParameterBase* parameter = firstParameter(); parameter != nullptr; parameter = parameter->mNext) {
        if (u32(parameter->mNameHash) == nameHash) return parameter;
    }
    return nullptr;
}

/* 027A86F8 */
void ParameterObject::copy(ParameterObject* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!beginCopy()) return;
    ParameterBase* first = firstParameter();
    ParameterBase* start = source->findParameter(first->mNameHash);
    copyParameters(first, nullptr, start, nullptr);
    endCopy();
}

/* inlined into applyResource: the parameter of a resource with that name hash */
ResParameterData* ResParameterObjData::find(u32 nameHash) {
    u32 count = mParameterNum;
    ResParameterData* parameter = first();
    for (u32 index = 0; index != count; ++index) {
        if (nameHash == u32(parameter->mNameHash)) return parameter;
        parameter = parameter->next();
    }
    return nullptr;
}

/* inlined into applyResource: report a parameter no resource sets to the list and to its parent */
static void reportNotApplied(parameter_tree::Node* list, ParameterObject* object, ParameterBase* parameter,
                             ResParameterObjData* resource, s32 slot, s32 parentSlot) {
    gabi::FrameLocal<gptr<ResParameterObjData>> argument(slot);
    parameter_tree::NodeReportDispatch* methods = gabi::at<parameter_tree::NodeReportDispatch>(u32(list->mTable));
    *argument = resource;
    gabi::call_ptr<void>(methods->mParameterNotApplied.mFunction, list, object, parameter, argument.get());
    parameter_tree::Node* parent = list->mParent;
    if (parent == nullptr) return;
    parameter_tree::NodeReportDispatch* parentMethods = gabi::at<parameter_tree::NodeReportDispatch>(u32(parent->mTable));
    gabi::FrameLocal<gptr<ResParameterObjData>> parentArgument(parentSlot);
    *parentArgument = resource;
    gabi::call_ptr<void>(parentMethods->mParameterNotApplied.mFunction, parent, object, parameter, parentArgument.get());
}

/* inlined into applyResource: apply one resource parameter to a parameter */
static void applyParameter(ParameterBase* parameter, ResParameterData* resource, s32 slot) {
    gabi::FrameLocal<gptr<ParameterRecord>> argument(slot);
    *argument = static_cast<ParameterRecord*>(resource);
    parameter->applyResource(argument.get());
}

/* 027A8780 */
void ParameterObject::applyResource(u32 blend, const gptr<ResParameterObjData>* first, const gptr<ResParameterObjData>* second,
                                    parameter_tree::Node* list, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    ResParameterObjData* secondResource = *second;
    ResParameterObjData* firstResource = *first;
    if (!beginApply()) return;
    ParameterBase* parameter = firstParameter();
    if (parameter != nullptr && blend) {
        f32 firstWeight = gabi::fsubs_fpr(1.0f, weight);
        do {
            u32 nameHash = parameter->mNameHash;
            ResParameterData* firstParameter = firstResource != nullptr ? firstResource->find(nameHash) : nullptr;
            if (parameter->isFloatType()) {
                ResParameterData* secondParameter = secondResource != nullptr ? secondResource->find(nameHash) : nullptr;
                if (firstParameter != nullptr && secondParameter != nullptr) {
                    parameter->makeZero();
                    gabi::FrameLocal<gptr<ParameterRecord>> firstArgument(0x10);
                    *firstArgument = static_cast<ParameterRecord*>(firstParameter);
                    parameter->addWeightedResource(firstArgument.get(), firstWeight);
                    gabi::FrameLocal<gptr<ParameterRecord>> secondArgument(0x14);
                    *secondArgument = static_cast<ParameterRecord*>(secondParameter);
                    parameter->addWeightedResource(secondArgument.get(), weight);
                } else if (list != nullptr) {
                    parameter_tree::NodeReportDispatch* methods = gabi::at<parameter_tree::NodeReportDispatch>(u32(list->mTable));
                    gabi::FrameLocal<gptr<ResParameterObjData>> firstObject(0x18), secondObject(0x1C);
                    gabi::FrameLocal<gptr<ResParameterData>> firstFound(0x20), secondFound(0x24);
                    *secondObject = secondResource;
                    *secondFound = secondParameter;
                    *firstFound = firstParameter;
                    *firstObject = firstResource;
                    gabi::call_ptr<void>(methods->mParameterNotBlended.mFunction, list, this, parameter,
                                         firstObject.get(), secondObject.get(), firstFound.get(), secondFound.get(), weight);
                    parameter_tree::Node* parent = list->mParent;
                    if (parent != nullptr) {
                        parameter_tree::NodeReportDispatch* parentMethods = gabi::at<parameter_tree::NodeReportDispatch>(u32(parent->mTable));
                        gabi::FrameLocal<gptr<ResParameterObjData>> parentFirstObject(0x28), parentSecondObject(0x2C);
                        gabi::FrameLocal<gptr<ResParameterData>> parentFirstFound(0x30), parentSecondFound(0x34);
                        *parentFirstObject = firstResource;
                        *parentFirstFound = firstParameter;
                        *parentSecondFound = secondParameter;
                        *parentSecondObject = secondResource;
                        gabi::call_ptr<void>(parentMethods->mParameterNotBlended.mFunction, parent, this, parameter,
                                             parentFirstObject.get(), parentSecondObject.get(), parentFirstFound.get(),
                                             parentSecondFound.get(), weight);
                    }
                }
            } else if (firstParameter != nullptr) {
                applyParameter(parameter, firstParameter, 0x38);
            } else if (list != nullptr) {
                reportNotApplied(list, this, parameter, firstResource, 0x3C, 0x40);
            }
            parameter = parameter->mNext;
        } while (parameter != nullptr);
    } else {
        for (; parameter != nullptr; parameter = parameter->mNext) {
            ResParameterData* found = firstResource != nullptr ? firstResource->find(parameter->mNameHash) : nullptr;
            if (found != nullptr) {
                applyParameter(parameter, found, 0x44);
            } else if (list != nullptr) {
                reportNotApplied(list, this, parameter, firstResource, 0x48, 0x4C);
            }
        }
    }
    endApply();
}

/* 027A8BAC */
void ParameterObject::applyResourceBlend(const gptr<ResParameterObjData>* first, const gptr<ResParameterObjData>* second,
                                         parameter_tree::Node* list, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    ResParameterObjData* firstResource = *first;
    ResParameterObjData* secondResource = *second;
    gabi::FrameLocal<gptr<ResParameterObjData>> firstArgument(8), secondArgument(0xC);
    if (!(weight > 0.0) && firstResource != nullptr) {
        // Only the first resource counts: applied unblended (the weight register still holds 0.0).
        *firstArgument = firstResource;
        *secondArgument = nullptr;
        applyResource(0, firstArgument.get(), secondArgument.get(), list, 0.0);
    } else if (!(1.0 > weight) && secondResource != nullptr) {
        *firstArgument = secondResource;
        *secondArgument = nullptr;
        applyResource(0, firstArgument.get(), secondArgument.get(), list, 0.0);
    } else {
        *firstArgument = firstResource;
        *secondArgument = secondResource;
        applyResource(1, firstArgument.get(), secondArgument.get(), list, weight);
    }
}

// HD: compiler-generated and template code of this translation unit

/* 027A8C68 */
void parameter_object_text::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object) callback
ext C0009988 void moveText(void* destination, const void* source, u32 size, u32 flags)
ext 0279E904 u32 agl::ParameterBase::makeZero()
ext 0279EACC u32 agl::ParameterBase::isFloatType()
ext 0279EB28 void agl::ParameterBase::applyResource(gptr<ParameterRecord>* record)
ext 0279EC1C void agl::ParameterBase::addWeightedResource(gptr<ParameterRecord>* record, f64 weight)
data sObjectMethods 1016847C
data sTextBufferMethods 10168434
data sFixedTextBase64Methods 1016844C
data sFixedText64Methods 10168464
data sEmptyText 104A0CD8 nocheck
*/
