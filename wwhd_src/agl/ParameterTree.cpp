// Parameter name copies and intrusive registration, reconstructed from assigned WWHD code.
#include "bindings.h"
#include "agl/ParameterTree.h"
void copyParameterNameBytes(void*, const void*, u32, u32);
u32 hashParameterName(const sead::SafeText*);
void* allocateParameterTreeObject(u32);
agl::parameter_tree::MatchRecord* parameterValueInputAt(const gptr<agl::parameter_tree::InputRecord_l>*, u32);
agl::parameter_tree::InputRecord_l* parameterChildInputAt(const gptr<agl::parameter_tree::InputRecord_l>*, u32);
void blendParameterValue(agl::parameter_tree::Value*, u32,
    const gptr<agl::parameter_tree::MatchRecord>*, const gptr<agl::parameter_tree::MatchRecord>*,
    agl::parameter_tree::Node*, f64);
static u32 sParameterNodeTable;
static u32 sParameterBufferBaseTable;
static u32 sParameterBufferTable;
static u32 sParameterFixedNameTable;
static s32 boundedParameterNameLength(const be<u8>* text) {
    s32 length = 0;
    if (*text != 0) {
        do {
            ++length; ++text;
            if (length > sead::kMaxTextLength) return 0;
        } while (*text != 0);
    }
    return length;
}
/* 0279C950 */
void agl::parameter_tree::Node::setName(sead::SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* destination = mName.mText;
    name->assureTermination();
    s32 length = boundedParameterNameLength(name->mText);
    s32 capacity = mName.mCapacity;
    if (length >= capacity) length = capacity - 1;
    name->assureTermination();
    copyParameterNameBytes(destination, name->mText, (u32)length, 0);
    destination[length] = 0;
    mNameHash = hashParameterName(name);
}
/* 0279CB24 */
void agl::parameter_tree::Node::appendChild(Node* child, sead::SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    child->setName(name);
    Node* last = mChildren;
    if (last != NULL) {
        Node* next = last;
        do { last = next; next = last->mNextSibling; } while (next != NULL);
    }
    if (last != NULL) last->mNextSibling = child;
    else mChildren = child;
    child->mParent = this;
}
/* 0279CB9C */
void agl::parameter_tree::Node::appendValue(Value* value, sead::SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* destination = value->mName.mText;
    name->assureTermination();
    s32 length = boundedParameterNameLength(name->mText);
    s32 capacity = value->mName.mCapacity;
    if (length >= capacity) length = capacity - 1;
    name->assureTermination();
    copyParameterNameBytes(destination, name->mText, (u32)length, 0);
    destination[length] = 0;
    value->mNameHash = hashParameterName(&value->mName);
    Value* last = mValues;
    if (last != NULL) {
        Value* next = last;
        do { last = next; next = last->mNext; } while (next != NULL);
    }
    if (last != NULL) last->mNext = value;
    else mValues = value;
}
/* 0279CCAC */
agl::parameter_tree::Value* agl::parameter_tree::Node::findValue(const gptr<MatchRecord>* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    MatchRecord* selected = *record;
    if (selected == NULL) return NULL;
    Value* value = mValues;
    while (value != NULL) {
        gabi::FrameLocal<gptr<MatchRecord>> selectedSlot(8);
        *selectedSlot = selected;
        if (gabi::call_ptr<u32>(value->mVtable->mMatchesRecord, value, selectedSlot.get()) != 0) return value;
        value = value->mNext;
    }
    return NULL;
}
/* 0279CA34 */
agl::parameter_tree::Node* agl::parameter_tree::Node::create(Node* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (node == NULL) node = (Node*)allocateParameterTreeObject(sizeof(Node));
    if (node != NULL) {
        node->mValues = NULL; node->mChildren = NULL;
        node->mTable = gabi::ea(&sParameterNodeTable);
        sead::FixedSafeText<64>* name = gabi::at<sead::FixedSafeText<64>>(gabi::ea(node) + (u32)offsetof(Node, mName));
        if (name == NULL) name = (sead::FixedSafeText<64>*)allocateParameterTreeObject(sizeof(sead::FixedSafeText<64>));
        if (name != NULL) {
            sead::SafeTextBuffer* base = name;
            if (base == NULL) base = (sead::SafeTextBuffer*)allocateParameterTreeObject(sizeof(sead::SafeTextBuffer));
            if (base != NULL) {
                base->mText = gabi::at<be<u8>>(gabi::ea(name) + (u32)offsetof(sead::FixedSafeText<64>, mStorage));
                base->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterBufferBaseTable);
                base->mCapacity = 64;
                name->mStorage[63] = 0;
            }
            be<u8>* text = name->mText;
            name->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterBufferTable);
            *text = 0;
            name->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterFixedNameTable);
        }
        node->mNextSibling = NULL; node->mParent = NULL;
        node->setName(&sFileDeviceEmptyText);
    }
    return node;
}
/* 0279CD28 */
agl::parameter_tree::MatchRecord* agl::parameter_tree::Node::findValueInput(
        const gptr<InputRecord_l>* input, Value* value) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<InputRecord_l>> source(12);
    *source = *input;
    if (*source == NULL) return NULL;
    u32 count = (*source)->mValueCount;
    MatchRecord* current = NULL;
    if (count != 0) current = parameterValueInputAt(source.get(), 0);
    gabi::FrameLocal<be<u32>> firstIndex(16), endIndex(24);
    gabi::FrameLocal<gptr<MatchRecord>> firstRecord(20);
    *firstIndex = 0; *firstRecord = current;
    count = (*source)->mValueCount;
    if (count != 0) {
        parameterValueInputAt(source.get(), count - 1);
        count = (*source)->mValueCount;
    }
    *endIndex = count;
    for (u32 index = 0; index != count; ++index) {
        ValueVtable* table = value->mVtable;
        gabi::FrameLocal<gptr<MatchRecord>> recordSlot(8);
        *recordSlot = current;
        if (gabi::call_ptr<u32>(table->mMatchesRecord, value, recordSlot.get()) != 0) return current;
        u32 step = current->mPrefix;
        current = gabi::at<MatchRecord>(gabi::ea(current) + step);
    }
    return NULL;
}
/* 0279CE84 */
agl::parameter_tree::Node* agl::parameter_tree::Node::findChild(const gptr<MatchRecord>* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    MatchRecord* selected = *record;
    if (selected == NULL) return NULL;
    Node* child = mChildren;
    while (child != NULL) {
        auto* table = gabi::at<NodeMatchDispatch>(child->mTable);
        gabi::FrameLocal<gptr<MatchRecord>> recordSlot(8);
        *recordSlot = selected;
        if (gabi::call_ptr<u32>(table->mMatchesRecord, child, recordSlot.get()) != 0) return child;
        child = child->mNextSibling;
    }
    return NULL;
}
/* 0279CF00 */
agl::parameter_tree::InputRecord_l* agl::parameter_tree::Node::findChildInput(
        const gptr<InputRecord_l>* input, Node* child) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<InputRecord_l>> source(12);
    *source = *input;
    if (*source == NULL) return NULL;
    u32 count = (*source)->mChildCount;
    InputRecord_l* current = NULL;
    if (count != 0) current = parameterChildInputAt(source.get(), 0);
    gabi::FrameLocal<be<u32>> firstIndex(16), endIndex(24);
    gabi::FrameLocal<gptr<InputRecord_l>> firstRecord(20);
    *firstIndex = 0; *firstRecord = current;
    count = (*source)->mChildCount;
    if (count != 0) {
        parameterChildInputAt(source.get(), count - 1);
        count = (*source)->mChildCount;
    }
    *endIndex = count;
    for (u32 index = 0; index != count; ++index) {
        auto* table = gabi::at<NodeMatchDispatch>(child->mTable);
        gabi::FrameLocal<gptr<InputRecord_l>> recordSlot(8);
        *recordSlot = current;
        if (gabi::call_ptr<u32>(table->mMatchesRecord, child, recordSlot.get()) != 0) return current;
        u32 step = current->mByteStep;
        current = gabi::at<InputRecord_l>(gabi::ea(current) + step);
    }
    return NULL;
}
/* 0279D05C */
void agl::parameter_tree::Node::applySecondValues(u32 blend, const gptr<InputRecord_l>* second, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<InputRecord_l>> source(24);
    *source = *second;
    if (*source == NULL) return;
    u32 count = (*source)->mValueCount;
    MatchRecord* current = NULL;
    if (count != 0) current = parameterValueInputAt(source.get(), 0);
    gabi::FrameLocal<be<u32>> firstIndex(28), endIndex(36);
    gabi::FrameLocal<gptr<MatchRecord>> firstRecord(32);
    *firstIndex = 0; *firstRecord = current;
    count = (*source)->mValueCount;
    if (count != 0) {
        parameterValueInputAt(source.get(), count - 1);
        count = (*source)->mValueCount;
    }
    *endIndex = count;
    for (u32 index = 0; index != count; ++index) {
        MatchRecord* selected = current;
        gabi::FrameLocal<gptr<MatchRecord>> searchRecord(12);
        *searchRecord = selected;
        Value* value = findValue(searchRecord.get());
        if (value != NULL) {
            gabi::FrameLocal<be<u32>> scratch(8);
            gabi::FrameLocal<gptr<MatchRecord>> first(16), secondRecord(20);
            *scratch = 0; *secondRecord = selected; *first = NULL;
            blendParameterValue(value, blend, first.get(), secondRecord.get(), this, weight);
        }
        u32 step = current->mPrefix;
        current = gabi::at<MatchRecord>(gabi::ea(current) + step);
    }
}
/* 0279D198 */
void agl::parameter_tree::Node::applySecondChildren(u32 blend, const gptr<InputRecord_l>* second, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<InputRecord_l>> source(24);
    *source = *second;
    if (*source == NULL) return;
    u32 count = (*source)->mChildCount;
    InputRecord_l* current = NULL;
    if (count != 0) current = parameterChildInputAt(source.get(), 0);
    gabi::FrameLocal<be<u32>> firstIndex(28), endIndex(36);
    gabi::FrameLocal<gptr<InputRecord_l>> firstRecord(32);
    *firstIndex = 0; *firstRecord = current;
    count = (*source)->mChildCount;
    if (count != 0) {
        parameterChildInputAt(source.get(), count - 1);
        count = (*source)->mChildCount;
    }
    *endIndex = count;
    for (u32 index = 0; index != count; ++index) {
        InputRecord_l* selected = current;
        gabi::FrameLocal<gptr<InputRecord_l>> searchRecord(12);
        *searchRecord = selected;
        Node* child = findChild(reinterpret_cast<const gptr<MatchRecord>*>(searchRecord.get()));
        if (child != NULL) {
            gabi::FrameLocal<be<u32>> scratch(8);
            gabi::FrameLocal<gptr<InputRecord_l>> first(16), secondRecord(20);
            *scratch = 0; *secondRecord = selected; *first = NULL;
            child->blendRecords(blend, first.get(), secondRecord.get(), weight);
        }
        u32 step = current->mByteStep;
        current = gabi::at<InputRecord_l>(gabi::ea(current) + step);
    }
}
/* 0279D2D0 */
void agl::parameter_tree::Node::blendRecords(u32 blend, const gptr<InputRecord_l>* firstInput,
                                           const gptr<InputRecord_l>* secondInput, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<InputRecord_l>> source(8);
    *source = *firstInput;
    InputRecord_l* second = *secondInput;
    auto* table = gabi::at<NodeApplyDispatch>(mTable);
    if (gabi::call_ptr<u32>(table->mBeforeApply, this) == 0) return;
    if (*source != NULL) {
        u32 count = (*source)->mValueCount;
        MatchRecord* current = NULL;
        if (count != 0) current = parameterValueInputAt(source.get(), 0);
        gabi::FrameLocal<be<u32>> firstIndex(52), endIndex(76);
        gabi::FrameLocal<gptr<MatchRecord>> firstRecord(56);
        *firstIndex = 0; *firstRecord = current;
        count = (*source)->mValueCount;
        if (count != 0) {
            parameterValueInputAt(source.get(), count - 1);
            count = (*source)->mValueCount;
        }
        *endIndex = count;
        for (u32 index = 0; index != count; ++index) {
            MatchRecord* selected = current;
            Value* value = NULL;
            if (selected != NULL) {
                gabi::FrameLocal<gptr<MatchRecord>> searchRecord(12);
                *searchRecord = selected;
                value = findValue(searchRecord.get());
            }
            if (value != NULL) {
                gabi::FrameLocal<gptr<InputRecord_l>> otherInput(16);
                *otherInput = second;
                MatchRecord* matching = findValueInput(otherInput.get(), value);
                gabi::FrameLocal<gptr<MatchRecord>> firstRecordSlot(20), secondRecordSlot(24);
                *secondRecordSlot = matching; *firstRecordSlot = selected;
                blendParameterValue(value, blend, firstRecordSlot.get(), secondRecordSlot.get(), this, weight);
            } else {
                gabi::FrameLocal<gptr<InputRecord_l>> otherInput(28);
                *otherInput = second;
                applySecondValues(blend, otherInput.get(), weight);
            }
            u32 step = current->mPrefix;
            current = gabi::at<MatchRecord>(gabi::ea(current) + step);
        }
    } else {
        gabi::FrameLocal<gptr<InputRecord_l>> otherInput(68);
        *otherInput = second;
        applySecondValues(blend, otherInput.get(), weight);
    }
    if (*source != NULL) {
        u32 count = (*source)->mChildCount;
        InputRecord_l* current = NULL;
        if (count != 0) current = parameterChildInputAt(source.get(), 0);
        gabi::FrameLocal<gptr<InputRecord_l>> firstRecord(64);
        gabi::FrameLocal<be<u32>> firstIndex(60), endIndex(84);
        *firstRecord = current; *firstIndex = 0;
        count = (*source)->mChildCount;
        if (count != 0) {
            parameterChildInputAt(source.get(), count - 1);
            count = (*source)->mChildCount;
        }
        *endIndex = count;
        for (u32 index = 0; index != count; ++index) {
            InputRecord_l* selected = current;
            Node* child = NULL;
            if (selected != NULL) {
                gabi::FrameLocal<gptr<InputRecord_l>> searchRecord(32);
                *searchRecord = selected;
                child = findChild(reinterpret_cast<const gptr<MatchRecord>*>(searchRecord.get()));
            }
            if (child != NULL) {
                gabi::FrameLocal<gptr<InputRecord_l>> otherInput(36);
                *otherInput = second;
                InputRecord_l* matching = findChildInput(otherInput.get(), child);
                gabi::FrameLocal<gptr<InputRecord_l>> firstRecordSlot(40), secondRecordSlot(44);
                *secondRecordSlot = matching; *firstRecordSlot = selected;
                child->blendRecords(blend, firstRecordSlot.get(), secondRecordSlot.get(), weight);
            } else {
                gabi::FrameLocal<gptr<InputRecord_l>> otherInput(48);
                *otherInput = second;
                applySecondChildren(blend, otherInput.get(), weight);
            }
            u32 step = current->mByteStep;
            current = gabi::at<InputRecord_l>(gabi::ea(current) + step);
        }
    } else {
        gabi::FrameLocal<gptr<InputRecord_l>> otherInput(72);
        *otherInput = second;
        applySecondChildren(blend, otherInput.get(), weight);
    }
    table = gabi::at<NodeApplyDispatch>(mTable);
    gabi::call_ptr<void>(table->mAfterApply, this);
}
/* 0279D5E0 */
void agl::parameter_tree::Node::applyRecord(const gptr<InputRecord_l>* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    InputRecord_l* selected = *record;
    gabi::FrameLocal<be<u32>> conversionScratch(8);
    gabi::FrameLocal<gptr<InputRecord_l>> first(12);
    gabi::FrameLocal<gptr<InputRecord_l>> second(16);
    *conversionScratch = 0; *first = selected; *second = NULL;
    blendRecords(0, first.get(), second.get(), 0.0);
}
/* 0279D624 */
void agl::parameter_tree::Node::applyBlend(const gptr<InputRecord_l>* firstInput,
                                         const gptr<InputRecord_l>* secondInput, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    InputRecord_l* secondValue = *secondInput;
    InputRecord_l* firstValue = *firstInput;
    gabi::FrameLocal<gptr<InputRecord_l>> first(8);
    gabi::FrameLocal<gptr<InputRecord_l>> second(12);
    if (!(weight > 0.0) && firstValue != NULL) {
        *first = firstValue; *second = NULL;
        blendRecords(0, first.get(), second.get(), 0.0);
    } else if (!(1.0 > weight) && secondValue != NULL) {
        *first = secondValue; *second = NULL;
        blendRecords(0, first.get(), second.get(), 0.0);
    } else {
        *first = firstValue; *second = secondValue;
        blendRecords(1, first.get(), second.get(), weight);
    }
}
/* WWHD glue
 ext 027A8780 void blendParameterValue(agl::parameter_tree::Value*, u32, const gptr<agl::parameter_tree::MatchRecord>*, const gptr<agl::parameter_tree::MatchRecord>*, agl::parameter_tree::Node*, f64)
 ext 027A7424 agl::parameter_tree::MatchRecord* parameterValueInputAt(const gptr<agl::parameter_tree::InputRecord_l>*, u32)
 ext 027A73F4 agl::parameter_tree::InputRecord_l* parameterChildInputAt(const gptr<agl::parameter_tree::InputRecord_l>*, u32)
 data sParameterNodeTable 101678F8
 data sParameterBufferBaseTable 10167880
 data sParameterBufferTable 10167898
 data sParameterFixedNameTable 101678B0
 ext 0273AD10 void* allocateParameterTreeObject(u32)
 ext C0009988 void copyParameterNameBytes(void*, const void*, u32, u32)
 ext 0279E57C u32 hashParameterName(const sead::SafeText*)
*/
