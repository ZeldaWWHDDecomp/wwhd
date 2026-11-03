// agl parameter selection: construction, owner link, destruction and index lookup of a text parameter that
// names an entry of its owner's list, plus the String32 parameter accessors and the text-class virtuals this
// translation unit emits. Written from WWHD's instructions.
#include "agl/ParameterSelection.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static ParameterBase* constructParameterBase(ParameterBase* parameter);
static void moveText(void* destination, const void* source, u32 size, u32 flags);
static sead::VtableSlot sTextParameterMethods;
static sead::VtableSlot sSelectionMethods;
static sead::SafeTextVtable sTextBufferMethods;
static sead::SafeTextVtable sFixedTextBase32Methods;
static sead::SafeTextVtable sFixedText32Methods;
static sead::SafeText sEmptyText;

/* inlined: length of a text, 0 when it is longer than sead's limit */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > sead::kMaxTextLength) return 0;
    }
    return length;
}

/* inlined: text equality as sead compares (same pointer, or equal up to the terminator within the limit) */
static bool textsEqual(const be<u8>* text, const be<u8>* other) {
    for (s32 index = 0; index <= sead::kMaxTextLength; ++index) {
        u8 character = text[index];
        if (character != other[index]) return false;
        if (character == 0) return true;
    }
    return false;
}

/* inlined: copy the empty text into the value (assured before and after the length scan) */
static void copyEmptyText(be<u8>* destination, const be<s32>* capacity, sead::VtableSlot* assure, const be<u8>* source,
                          bool assureFirst) {
    if (assureFirst) gabi::call_ptr<void>(assure->mFunction, &sEmptyText);
    s32 length = textLength(source);
    s32 maximum = *capacity;
    if (length >= maximum) length = maximum - 1;
    gabi::call_ptr<void>(assure->mFunction, &sEmptyText);
    moveText(destination, source, u32(length), 0);
    destination[length] = 0;
}

/* 027A7E6C */
ParameterSelection* ParameterSelection::create(ParameterSelection* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterSelection* selection = storage;
    if (selection == nullptr) selection = static_cast<ParameterSelection*>(allocateObject(sizeof(ParameterSelection)));
    if (selection == nullptr) return selection;
    TextParameter32* parameter = selection;
    if (parameter == nullptr) parameter = static_cast<TextParameter32*>(allocateObject(sizeof(TextParameter32)));
    if (parameter != nullptr) {
        constructParameterBase(parameter);
        parameter->mMethods = &sTextParameterMethods;
        sead::SafeTextBuffer* value = &parameter->mValue;
        if (value == nullptr) value = static_cast<sead::SafeTextBuffer*>(allocateObject(sizeof(sead::SafeTextBuffer)));
        if (value != nullptr) {
            value->mText = parameter->mValue.mStorage;
            value->mVtable = &sTextBufferMethods;
            value->mCapacity = 32;
            parameter->mValue.mStorage[31] = 0;
        }
        be<u8>* firstCharacter = parameter->mValue.mText;
        parameter->mValue.mVtable = &sFixedTextBase32Methods;
        firstCharacter[0] = 0;
        parameter->mValue.mVtable = &sFixedText32Methods;
    }
    selection->mLink.mPrevious = nullptr;
    selection->mListener = nullptr;
    selection->mIndex = Selection_None;
    selection->mMethods = &sSelectionMethods;
    selection->mLink.mNext = nullptr;
    selection->mOwner = nullptr;
    be<u8>* destination = selection->mValue.mText;
    sead::VtableSlot* assure = &sEmptyText.mVtable->mAssureTermination;
    gabi::call_ptr<void>(assure->mFunction, &sEmptyText);
    copyEmptyText(destination, &selection->mValue.mCapacity, assure, sEmptyText.mText, false);
    return selection;
}

/* 027A7FF4 */
void ParameterSelection::setOwner(HierarchyNode* owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    HierarchyNode* previous = mOwner;
    if (owner == previous) return;
    if (previous != nullptr) {
        gabi::at<sead::Link>(gabi::ea(this) + u32(s32(previous->mLinkedEntries.mLinkOffset)))->detach();
        previous->mLinkedEntries.mList.mCount = previous->mLinkedEntries.mList.mCount - 1;
    }
    mOwner = owner;
    if (owner != nullptr) {
        u32 offset = u32(s32(owner->mLinkedEntries.mLinkOffset));
        owner->mLinkedEntries.mList.insertBefore(gabi::at<sead::Link>(gabi::ea(this) + offset));
        owner->mLinkedEntries.mList.mCount = owner->mLinkedEntries.mList.mCount + 1;
    }
}

/* 027A8084 */
void ParameterSelection::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mMethods = &sSelectionMethods;
    if (mLink.mNext != nullptr || mLink.mPrevious != nullptr) setOwner(nullptr);
    if (flags & 1) releaseObject(this);
}

/* 027A80FC */
void ParameterSelection::updateIndex() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mOwner == nullptr) return;
    mValue.assureTermination();
    mValue.assureTermination();
    sead::VtableSlot* assure = &sEmptyText.mVtable->mAssureTermination;
    const be<u8>* text = mValue.mText;
    gabi::call_ptr<void>(assure->mFunction, &sEmptyText);
    const be<u8>* empty = sEmptyText.mText;
    bool isEmpty = text == empty;
    if (!isEmpty) isEmpty = textsEqual(mValue.mText, empty);
    if (isEmpty) {
        // GHS reloads the value's text here; the empty text read above is copied (normalising the value).
        be<u8>* destination = mValue.mText;
        mIndex = Selection_None;
        copyEmptyText(destination, &mValue.mCapacity, assure, empty, true);
    } else {
        mIndex = Selection_NotFound;
        s32 remaining = entryCount();
        for (s32 index = 0; remaining > 0; --remaining, ++index) {
            sead::SafeText* name = entryName(index);
            mValue.assureTermination();
            mValue.assureTermination();
            const be<u8>* value = mValue.mText;
            name->assureTermination();
            if (value == name->mText.get() || textsEqual(mValue.mText, name->mText)) {
                mIndex = index;
                break;
            }
        }
    }
    ParameterSelectionListener* listener = mListener;
    if (listener != nullptr) gabi::call_ptr<void>(listener->mMethods->mSelectionUpdated.mFunction, listener, this);
}

/* 027A8424 */
void TextParameter32::onResourceApplied(const be<u8>* data, u32 size) {}

/* 027A8428 */
u32 TextParameter32::valueKind() const { return ParameterType_String32; }

/* 027A8430 */
const be<u8>* TextParameter32::constValue() {
    gabi::AutoFrame frame{gabi::current_function()};
    mValue.assureTermination();
    return mValue.mText;
}

/* 027A8470 */
be<u8>* TextParameter32::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    mValue.assureTermination();
    return mValue.mText;
}

/* 027A84B0 */
u32 TextParameter32::valueBytes() const { return sizeof(mValue.mStorage); }

// HD: compiler-generated and template code of this translation unit

/* 027A837C */
void parameter_selection_text::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A8390 */
void parameter_selection_text::FixedTextBase32::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A83A4 */
void parameter_selection_text::Text::assureTerminationImpl() {}

/* 027A83A8 */
void parameter_selection_text::TextBuffer::assureTerminationImpl() {
    mText[static_cast<s32>(mCapacity) - 1] = 0;
}

/* 027A83C0 */
void parameter_selection_text::TextBuffer::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A83D4 */
void parameter_selection_text::FixedText32::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A83E8 */
void parameter_selection_text::FixedTextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A83FC */
void parameter_selection_text::FixedTextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A8410 */
void parameter_selection_text::FixedTextC::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object) callback
ext 0279E674 agl::ParameterBase* constructParameterBase(ParameterBase* parameter)
ext C0009988 void moveText(void* destination, const void* source, u32 size, u32 flags)
ext 0273B2F0 sead::Link* sead::Link::insertBefore(Link* node)
ext 0273B310 sead::Link* sead::Link::detach()
data sTextParameterMethods 10168314
data sSelectionMethods 1016838C
data sTextBufferMethods 10168284
data sFixedTextBase32Methods 1016829C
data sFixedText32Methods 101682B4
data sEmptyText 104A0CD8 nocheck
*/
