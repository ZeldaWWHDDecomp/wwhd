// Slot allocation caller; cache implementations remain opaque interfaces.
#include "JSystem/J3DGraph/PipelineLookupInstances.h"
using namespace j3d::pipeline;
static void* sSlotCreationCache;
static PipelineSlotDimensions_l sSlotCreationBase;
static Scalar sSlotCreationHalf;
static Scalar sSlotCreationUnsignedBoundary;
struct SlotCreationNames_l { sead::SafeText mNames[19]; };
static SlotCreationNames_l sSlotCreationNames;
u32 createPipelineModeSlot(void*,sead::SafeText*,u32,u32,u32,u32,be<u32>*,u32,u32,u32);
u32 createPipelinePlainSlot(void*,sead::SafeText*,u32,u32,u32,u32,be<u32>*,u32,u32,u32);
namespace j3d::pipeline {
namespace {
struct SlotCreationStorage_l { Scalar mInputWidth,mInputHeight,mBaseWidth,mBaseHeight,mWidth,mHeight; };
struct SlotCreationRectangle_l { Scalar mBaseWidth,mBaseHeight,mWidth,mHeight; };
u32 slotCreationUnsigned(f64 value,f64 boundary,be<u32>* scratch) {
    u32 converted;
    // fcmpu followed by bge selects the subtraction path for unordered NaNs too.
    if(!(value<boundary)) {
        f64 shifted=gabi::fsubs_fpr(value,boundary);
        converted=static_cast<u32>(gabi::ftoi(shifted));
        *scratch=converted;
        converted+=0x80000000u;
    } else {
        converted=static_cast<u32>(gabi::ftoi(value));
        *scratch=converted;
    }
    return converted;
}
}
/* 027F81C0 */
u32 PipelineSlotTable::createWord(u32 index,const PipelineSlotDimensions_l* dimensions) {
    gabi::AutoFrame frame{0x027F81C0u};
    gabi::FrameLocal<be<u32>> conversion(0x10);
    gabi::FrameLocal<SlotCreationRectangle_l> rectangle(0x18);
    u32 existing=word(index);
    if(existing!=0) return existing;
    PipelineSlot_l* slot=&mSlots[index<kSlotCount?index:0];
    SlotCreationStorage_l* storage=static_cast<SlotCreationStorage_l*>(slot->mFirstStorage.get());
    storage->mInputWidth.bits=static_cast<u32>(dimensions->mWidth.bits);
    storage->mInputHeight.bits=static_cast<u32>(dimensions->mHeight.bits);
    u32 height=dimensions->mHeight.bits;
    u32 baseWidth=sSlotCreationBase.mWidth.bits;
    u32 baseHeight=sSlotCreationBase.mHeight.bits;
    rectangle->mHeight.bits=height;
    rectangle->mBaseHeight.bits=baseHeight;
    u32 width=dimensions->mWidth.bits;
    rectangle->mBaseWidth.bits=baseWidth;
    rectangle->mWidth.bits=width;
    if(slot->mAlternateEnabled!=0) {
        f64 half=ppc_lfs_value(sSlotCreationHalf.bits);
        f64 first=gabi::fmuls_fpr(ppc_lfs_value(rectangle->mBaseWidth.bits),half);
        f64 third=gabi::fmuls_fpr(ppc_lfs_value(rectangle->mWidth.bits),half);
        f64 second=gabi::fmuls_fpr(ppc_lfs_value(rectangle->mBaseHeight.bits),half);
        rectangle->mBaseWidth.bits=ppc_stfs_bits(first);
        f64 fourth=gabi::fmuls_fpr(ppc_lfs_value(rectangle->mHeight.bits),half);
        rectangle->mWidth.bits=ppc_stfs_bits(third);
        rectangle->mBaseHeight.bits=ppc_stfs_bits(second);
        rectangle->mHeight.bits=ppc_stfs_bits(fourth);
    }
    storage=static_cast<SlotCreationStorage_l*>(slot->mFirstStorage.get());
    storage->mBaseWidth.bits=static_cast<u32>(rectangle->mBaseWidth.bits);
    storage->mBaseHeight.bits=static_cast<u32>(rectangle->mBaseHeight.bits);
    storage->mWidth.bits=static_cast<u32>(rectangle->mWidth.bits);
    storage->mHeight.bits=static_cast<u32>(rectangle->mHeight.bits);
    storage=static_cast<SlotCreationStorage_l*>(slot->mFirstStorage.get());
    void* cache=sSlotCreationCache;
    if(index>=kSlotCount) index=0;
    u8 releaseMode=slot->mReleaseMode;
    f64 boundary=ppc_lfs_value(sSlotCreationUnsignedBoundary.bits);
    f64 storedWidth=ppc_lfs_value(storage->mWidth.bits);
    u32 integerWidth=slotCreationUnsigned(storedWidth,boundary,conversion.get());
    f64 storedHeight=ppc_lfs_value(storage->mHeight.bits);
    u32 integerHeight=slotCreationUnsigned(storedHeight,boundary,conversion.get());
    be<u32>* alternate=slot->mStateEnabled!=0 ? &slot->mAlternateWord : nullptr;
    u32 mode=slot->mModeWord;
    u32 format=slot->mFormatWord;
    u32 result;
    if(releaseMode!=0) result=createPipelineModeSlot(cache,&sSlotCreationNames.mNames[index],format,integerWidth,integerHeight,1,alternate,mode,1,0);
    else result=createPipelinePlainSlot(cache,&sSlotCreationNames.mNames[index],format,integerWidth,integerHeight,1,alternate,mode,1,0);
    mSlots[index].mWord=result;
    return result;
}
}
/* WWHD glue
data sSlotCreationCache 101F8DD4
data sSlotCreationBase 104A0908
data sSlotCreationHalf 1016EA00
data sSlotCreationUnsignedBoundary 1016EA04
data sSlotCreationNames 104B494C
ext 0279FDC8 u32 createPipelineModeSlot(void*,sead::SafeText*,u32,u32,u32,u32,be<u32>*,u32,u32,u32)
ext 0279FC6C u32 createPipelinePlainSlot(void*,sead::SafeText*,u32,u32,u32,u32,be<u32>*,u32,u32,u32)
*/
