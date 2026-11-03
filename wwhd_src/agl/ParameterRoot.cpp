// Parameter-root name and path construction reconstructed from assigned WWHD instructions.
#include "bindings.h"
#include "agl/ParameterRoot.h"
void* allocateParameterRootObject(u32);
void copyParameterRootBytes(void*, const void*, u32, u32);
static u32 sParameterRootTable;
static u32 sParameterRootTextTable;
static u32 sParameterRootBufferBaseTable;
static u32 sParameterRootNameBaseTable;
static u32 sParameterRootNameTable;
static u32 sParameterRootPathBaseTable;
static u32 sParameterRootPathTable;
static s32 parameterRootNameLength(const be<u8>* text) {
    s32 length = 0;
    if (*text != 0) {
        do {
            ++length; ++text;
            if (length > sead::kMaxTextLength) return 0;
        } while (*text != 0);
    }
    return length;
}
template <u32 Capacity>
static sead::FixedSafeText<Capacity>* constructRootText(sead::FixedSafeText<Capacity>* text) {
    if (text == NULL) text = (sead::FixedSafeText<Capacity>*)allocateParameterRootObject(sizeof(sead::FixedSafeText<Capacity>));
    if (text != NULL) {
        sead::SafeTextBuffer* base = text;
        if (base == NULL) base = (sead::SafeTextBuffer*)allocateParameterRootObject(sizeof(sead::SafeTextBuffer));
        if (base != NULL) {
            base->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterRootBufferBaseTable);
            base->mText = gabi::at<be<u8>>(gabi::ea(text) + offsetof(sead::FixedSafeText<Capacity>, mStorage));
            base->mCapacity = Capacity;
            text->mStorage[Capacity - 1] = 0;
        }
    }
    return text;
}
/* 0279A07C */
agl::ParameterRoot* agl::ParameterRoot::create(ParameterRoot* root, sead::SafeText* name, u32 context) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (root == NULL) root = (ParameterRoot*)allocateParameterRootObject(sizeof(ParameterRoot));
    if (root == NULL) return root;
    parameter_tree::Node::create(&root->mTree);
    root->mTree.mTable = gabi::ea(&sParameterRootTable);
    auto* display = constructRootText(gabi::at<sead::FixedSafeText<64>>(gabi::ea(root) + offsetof(ParameterRoot, mDisplayName)));
    if (display != NULL) {
        be<u8>* storage = display->mText;
        display->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterRootNameBaseTable);
        *storage = 0;
        display->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterRootNameTable);
    }
    root->mFlags = 0;
    auto* path = constructRootText(gabi::at<sead::FixedSafeText<256>>(gabi::ea(root) + offsetof(ParameterRoot, mPath)));
    if (path != NULL) {
        path->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterRootPathBaseTable);
        sead::SafeTextVtable* emptyTable = sFileDeviceEmptyText.mVtable;
        be<u8>* destination = path->mText;
        gabi::call_ptr<void>(emptyTable->mAssureTermination.mFunction, &sFileDeviceEmptyText);
        be<u8>* source = sFileDeviceEmptyText.mText;
        s32 length = parameterRootNameLength(source);
        s32 capacity = path->mCapacity;
        if (length >= capacity) length = capacity - 1;
        // The second dispatch keeps the first table and source pointer across the call.
        gabi::call_ptr<void>(emptyTable->mAssureTermination.mFunction, &sFileDeviceEmptyText);
        copyParameterRootBytes(destination, source, (u32)length, 0);
        destination[length] = 0;
        path->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterRootPathTable);
    }
    root->mState = 0;
    be<u8>* destination = root->mDisplayName.mText;
    name->assureTermination();
    s32 length = parameterRootNameLength(name->mText);
    s32 capacity = root->mDisplayName.mCapacity;
    if (length >= capacity) length = capacity - 1;
    name->assureTermination();
    copyParameterRootBytes(destination, name->mText, (u32)length, 0);
    destination[length] = 0;
    root->mContext = context;
    gabi::FrameLocal<sead::SafeText> rootName(8);
    rootName->mText = gabi::at<be<u8>>(gabi::guest_str("param_root"));
    rootName->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sParameterRootTextTable);
    root->mTree.setName(rootName.get());
    return root;
}
/* 0279A308 */
void agl::ParameterRoot::applyInput(const gptr<ParameterRootInput_l>* input) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterRootInput_l* record = *input;
    u32 state = record->mState;
    u32 context = mContext;
    mState = state;
    if (context != (u32)record->mContext) {
        gabi::FrameLocal<gptr<ParameterRootInput_l>> recordSlot(12);
        *recordSlot = record;
        auto* table = gabi::at<ParameterRootDispatch>(mTree.mTable);
        gabi::call_ptr<void>(table->mContextMismatch, this, recordSlot.get());
    }
    parameter_tree::InputRecord_l* payload = gabi::at<parameter_tree::InputRecord_l>(
        gabi::ea(record) + (u32)record->mPayloadOffset + sizeof(ParameterRootInput_l));
    gabi::FrameLocal<gptr<parameter_tree::InputRecord_l>> scratch(8), payloadSlot(16);
    *scratch = payload; *payloadSlot = payload;
    mTree.applyRecord(payloadSlot.get());
}
/* 0279A390 */
void agl::ParameterRoot::applyInputs(const gptr<ParameterRootInput_l>* firstInput,
                                    const gptr<ParameterRootInput_l>* secondInput, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterRootInput_l* first = *firstInput;
    ParameterRootInput_l* second = *secondInput;
    u32 context = mContext;
    if (context != (u32)first->mContext) {
        gabi::FrameLocal<gptr<ParameterRootInput_l>> firstSlot(12);
        *firstSlot = first;
        auto* table = gabi::at<ParameterRootDispatch>(mTree.mTable);
        gabi::call_ptr<void>(table->mContextMismatch, this, firstSlot.get());
        context = mContext;
    }
    if (context != (u32)second->mContext) {
        gabi::FrameLocal<gptr<ParameterRootInput_l>> secondSlot(16);
        *secondSlot = second;
        auto* table = gabi::at<ParameterRootDispatch>(mTree.mTable);
        gabi::call_ptr<void>(table->mContextMismatch, this, secondSlot.get());
    }
    parameter_tree::InputRecord_l* firstPayload = gabi::at<parameter_tree::InputRecord_l>(
        gabi::ea(first) + (u32)first->mPayloadOffset + sizeof(ParameterRootInput_l));
    parameter_tree::InputRecord_l* secondPayload = gabi::at<parameter_tree::InputRecord_l>(
        gabi::ea(second) + (u32)second->mPayloadOffset + sizeof(ParameterRootInput_l));
    gabi::FrameLocal<gptr<parameter_tree::InputRecord_l>> firstPayloadSlot(20), scratch(8), secondPayloadSlot(24);
    *firstPayloadSlot = firstPayload; *scratch = secondPayload; *secondPayloadSlot = secondPayload;
    mTree.applyBlend(firstPayloadSlot.get(), secondPayloadSlot.get(), weight);
}
/* 0279A950 */
u32 agl::ParameterRoot::constantOneHook() { return 1; }
/* 0279A958 */
void agl::ParameterRoot::onContextMismatch(const gptr<ParameterRootInput_l>* input) {}
/* 0279A560 */
u32 agl::ParameterRoot::treeConstantOneHook() { return 1; }
/* 0279A568 */
void agl::ParameterRoot::treeEmptyHook() {}
/* 0279A56C */
u32 agl::ParameterRoot::treeMatchesRecord(const gptr<parameter_tree::MatchRecord>* record) {
    const parameter_tree::MatchRecord* selected = *record;
    u32 ownHash = mTree.mNameHash;
    return ownHash == (u32)selected->mSecondaryValue;
}
/* 0279A588 */
void agl::ParameterRoot::treeTrailingEmptyHook() {}
/* 0279A58C */
void agl::ParameterRoot::treeAlternateTrailingEmptyHook() {}
/* 0279A590 */
void agl::ParameterRoot::treeFinalEmptyHook() {}
/* WWHD glue
 data sParameterRootTable 10167474
 data sParameterRootTextTable 10167378
 data sParameterRootBufferBaseTable 10167390
 data sParameterRootNameBaseTable 101673A8
 data sParameterRootNameTable 101673C0
 data sParameterRootPathBaseTable 101673D8
 data sParameterRootPathTable 101673F0
 str 10167468 "param_root"
 ext 0273AD10 void* allocateParameterRootObject(u32)
 ext C0009988 void copyParameterRootBytes(void*, const void*, u32, u32)
*/
