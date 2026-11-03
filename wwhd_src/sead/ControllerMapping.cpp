// WWHD mapped wrapper and buffered input aggregation from local instructions.
#include "sead/ControllerMapping.h"

static u32 sMappingTable;
static u32 sMappingBaseTable;
static MappingDefaults sDefaultMapping;
static sead::InputPair sInvalidAggregationPointer;
static sead::InputPair sZeroAggregationPointer;
void* allocateMappingObject(u32);
void copyMappingBytes(void*,const void*,u32,u32);

static u32 mappedBit(u32 bit) {
    bit &= 63;
    return bit < 32 ? 1u << bit : 0;
}

static void copyInputFloat(be<f32>* destination,const be<f32>* source) {
    *reinterpret_cast<be<u32>*>(destination) = (u32)*reinterpret_cast<const be<u32>*>(source);
}

/* 0273CDD4 */
static MappedInput* constructMappedInput(MappedInput* input) {
    if (input == NULL) input = static_cast<MappedInput*>(allocateMappingObject(sizeof(MappedInput)));
    if (input == NULL) return NULL;
    constructControllerWrapper(input);
    input->mTable = &sMappingTable;
    input->mBaseTable = gabi::ea(&sMappingBaseTable);
    copyMappingBytes(input->mMappings,sDefaultMapping.mMappings,28,0);
    return input;
}

/* 0273CE4C */
u32 MappedInput::mapButtons(u32 source) {
    s32 count = mButtonCount;
    u32 result = 0;
    for (s32 index = 0; index < count; ++index) {
        if (source & mappedBit(mMappings[index])) result |= mappedBit(index);
    }
    return result;
}

/* 0273CE94 */
void MappedInput::calc(u32 previous,u32 previousPointer) {
    sead::Controller* controller;
    bool connected = false;
    if (mEnabled != 0) {
        controller = mController;
        if (controller != NULL) {
            be<u32>* table = controller->mTable.get();
            connected = gabi::call_ptr<u32>(table[11],controller) != 0;
        }
    }
    if (connected) {
        controller = mController;
        u32 mapped = mapButtons(controller->mButtons);
        controller = mController;
        mButtons = mapped;
        copyInputFloat(&mStickX,&controller->mStickX);
        copyInputFloat(&mStickY,&controller->mStickY);
        copyInputFloat(&mCrossX,&controller->mCrossX);
        copyInputFloat(&mCrossY,&controller->mCrossY);
        copyInputFloat(&mLeftTrigger,&controller->mLeftTrigger);
        // This load remains in an FPR across the pointer-bit read, so signaling NaNs quiet.
        f32 rightTrigger = controller->mRightTrigger;
        s32 bit = mPointerBit;
        mRightTrigger = rightTrigger;
        u32 pointer = (u32)controller->mPointerState & 1;
        u32 contact = 0;
        if (bit >= 0) contact = ((u32)mButtons & mappedBit(bit)) != 0;
        updatePointer(pointer,contact,&controller->mPointerFloat);
        mapped = mapButtons(previous);
        update(mapped,previousPointer);
    } else {
        be<u32>* table = mTable.get();
        gabi::call_ptr<void>(table[11],this);
    }
    be<u32>* table = mTable.get();
    if (gabi::call_ptr<u32>(table[13],this) == 0) mIdleFrames = 0;
    else mIdleFrames = (u32)mIdleFrames + 1;
}

/* 0273D438 */
static void aggregateBufferedInput(sead::BufferedController* input) {
    input->mLeftTrigger = 0.0f;
    input->mStickY = 0.0f;
    input->mStickX = 0.0f;
    u32 buttons = 0;
    bool havePointer = false;
    bool haveStick = false;
    bool haveCross = false;
    u32 count = input->mSamples.mCount;
    gptr<sead::Controller>* entry = reinterpret_cast<gptr<sead::Controller>*>(input->mSamples.mStorage.get());
    input->mCrossX = 0.0f;
    gptr<sead::Controller>* end = entry + count;
    input->mRightTrigger = 0.0f;
    input->mCrossY = 0.0f;
    while (entry != end) {
        sead::Controller* sample = *entry;
        buttons |= (u32)sample->mButtons;
        if (!havePointer && ((u32)sample->mPointerState & 1)) {
            u32 contact = ((u32)sample->mButtons & 0x8000) != 0;
            sead::InputPair* point = &sInvalidAggregationPointer;
            if ((u32)sample->mPointerState & 1) point = &sample->mPointerFloat;
            input->updatePointer(1,contact,point);
            sample = *entry;
            havePointer = true;
        }
        if (!haveStick && ((f32)sample->mStickX != 0.0f || (f32)sample->mStickY != 0.0f)) {
            copyInputFloat(&input->mStickX,&sample->mStickX);
            copyInputFloat(&input->mStickY,&sample->mStickY);
            sample = *entry;
            haveStick = true;
        }
        if (!haveCross && ((f32)sample->mCrossX != 0.0f || (f32)sample->mCrossY != 0.0f)) {
            copyInputFloat(&input->mCrossX,&sample->mCrossX);
            copyInputFloat(&input->mCrossY,&sample->mCrossY);
            sample = *entry;
            haveCross = true;
        }
        f32 left = input->mLeftTrigger;
        f32 right = input->mRightTrigger;
        if (left == 0.0f && (f32)sample->mLeftTrigger != 0.0f) {
            copyInputFloat(&input->mLeftTrigger,&sample->mLeftTrigger);
            sample = *entry;
        }
        if (right == 0.0f && (f32)sample->mRightTrigger != 0.0f) {
            copyInputFloat(&input->mRightTrigger,&sample->mRightTrigger);
            sample = *entry;
        }
        be<u32>* table = sample->mTable.get();
        if (gabi::call_ptr<u32>(table[11],sample) != 0) input->mConnected = 1;
        ++entry;
    }
    input->mButtons = buttons;
    if (!havePointer) input->updatePointer(0,0,&sZeroAggregationPointer);
}

/* WWHD glue
 data sMappingTable 10143278
 data sMappingBaseTable 101432B0
 data sDefaultMapping 1014325C
 data sInvalidAggregationPointer 1049FFA0
 data sZeroAggregationPointer 104A0908
 ext 0273AD10 void* allocateMappingObject(u32)
 ext C0009988 void copyMappingBytes(void*, const void*, u32, u32)
*/
