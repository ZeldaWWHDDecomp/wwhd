// Value bank allocation and callback-guarded copies reconstructed from assigned WWHD bodies.
#include "bindings.h"
#include "agl/ParameterValueBank.h"
void* allocateParameterValueBank(u32);
void constructBankValue(agl::parameter_tree::Value*);
void copyBankValue(agl::parameter_tree::Value*, const agl::parameter_tree::Value*);
sead::RegistryHeap* findBankValueHeap(sead::RegistryManagerState*, void*);
sead::RegistryHeap* currentBankValueHeap(sead::RegistryManagerState*);
static u32 sParameterValueBankTable;
/* 0279A594 */
void agl::ParameterValueBank::copyValue(u32 destinationIndex, u32 sourceIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterValueBankDispatch* table = mDispatch;
    parameter_tree::Value* source = mValues.mData;
    if (sourceIndex < (u32)mValues.mCount)
        source = gabi::at<parameter_tree::Value>(gabi::ea(source) + sourceIndex * sizeof(parameter_tree::Value));
    if (gabi::call_ptr<u32>(table->mBeforeCopy, this, destinationIndex, source, 0u, 0.0) == 0) return;
    u32 count = mValues.mCount;
    parameter_tree::Value* data = mValues.mData;
    parameter_tree::Value* destination = data;
    if (destinationIndex < count)
        destination = gabi::at<parameter_tree::Value>(gabi::ea(data) + destinationIndex * sizeof(parameter_tree::Value));
    source = data;
    if (sourceIndex < count)
        source = gabi::at<parameter_tree::Value>(gabi::ea(data) + sourceIndex * sizeof(parameter_tree::Value));
    copyBankValue(destination, source);
    table = mDispatch;
    source = mValues.mData;
    if (sourceIndex < (u32)mValues.mCount)
        source = gabi::at<parameter_tree::Value>(gabi::ea(source) + sourceIndex * sizeof(parameter_tree::Value));
    gabi::call_ptr<void>(table->mAfterCopy, this, destinationIndex, source, 0u, 0.0);
}
/* 0279A6B0 */
void agl::ParameterValueBank::copyToOtherValues(u32 source) {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u8)mSingleValue != 0) return;
    for (s32 index = 0; index < (s32)mActiveCount; ++index) {
        if ((u32)index != source) copyValue((u32)index, source);
    }
}
/* 0279A730 */
agl::ParameterValueBank* agl::ParameterValueBank::create(ParameterValueBank* bank) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (bank == NULL) bank = (ParameterValueBank*)allocateParameterValueBank(sizeof(ParameterValueBank));
    if (bank == NULL) return bank;
    bank->mDispatch = reinterpret_cast<ParameterValueBankDispatch*>(&sParameterValueBankTable);
    be<u32>* state = gabi::at<be<u32>>(gabi::ea(bank));
    if (state == NULL) state = (be<u32>*)allocateParameterValueBank(sizeof(be<u32>));
    if (state != NULL) *state = 0;
    bank->mCurrent = 0;
    bank->mCount = 0;
    bank->mSingleValue = 1;
    bank->mActiveCount = 0;
    auto* values = gabi::at<ParameterValueBuffer>(gabi::ea(bank) + offsetof(ParameterValueBank, mValues));
    if (values == NULL) values = (ParameterValueBuffer*)allocateParameterValueBank(sizeof(ParameterValueBuffer));
    if (values != NULL) { values->mData = NULL; values->mCount = 0; }
    return bank;
}
/* 0279A7D8 */
void agl::ParameterValueBank::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    parameter_tree::Value* data = mValues.mData;
    mDispatch = reinterpret_cast<ParameterValueBankDispatch*>(&sParameterValueBankTable);
    if (data != NULL) {
        sead::RegistryHeap* heap = findBankValueHeap(sRegistryManager, data);
        void* storage = mValues.mData;
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage);
        mValues.mCount = 0;
        mValues.mData = NULL;
    }
    if ((flags & 1) != 0) operator_delete(this);
}
/* 0279A86C */
void agl::ParameterValueBank::initialize(u32 count, u32 singleValue, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mCount = count;
    mSingleValue = (u8)singleValue;
    mState = 0xFFFFFFFF;
    u32 allocationCount;
    if (singleValue != 0) {
        allocationCount = 1;
        mActiveCount = 1;
        mCurrent = 0;
    } else {
        allocationCount = count;
        mActiveCount = count;
        mCurrent = 0xFFFFFFFF;
        if ((s32)count <= 0) return;
    }
    if (heap == NULL) heap = currentBankValueHeap(sRegistryManager);
    auto* data = gabi::call_ptr<parameter_tree::Value*>(heap->dispatch()->mAllocate.mFunction,
        heap, allocationCount * sizeof(parameter_tree::Value), 4u);
    u32 remaining = (s32)allocationCount > 0 ? allocationCount : 1;
    u32 offset = 0;
    do {
        auto* value = gabi::at<parameter_tree::Value>(gabi::ea(data) + offset);
        if (value != NULL) constructBankValue(value);
        offset += sizeof(parameter_tree::Value);
    } while (--remaining != 0);
    if (data != NULL) { mValues.mData = data; mValues.mCount = allocationCount; }
}
/* WWHD glue
 data sParameterValueBankTable 101674E0
 ext 0273AD10 void* allocateParameterValueBank(u32)
 ext 027A84B8 void constructBankValue(agl::parameter_tree::Value*)
 ext 027A86F8 void copyBankValue(agl::parameter_tree::Value*, const agl::parameter_tree::Value*)
 ext 02755FEC sead::RegistryHeap* findBankValueHeap(sead::RegistryManagerState*, void*)
 ext 02756140 sead::RegistryHeap* currentBankValueHeap(sead::RegistryManagerState*)
*/
