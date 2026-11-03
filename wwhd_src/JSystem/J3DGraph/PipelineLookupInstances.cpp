// HD pipeline bounded slot lookups and per-TU inline descriptor/text operations.
#include "JSystem/J3DGraph/PipelineLookupInstances.h"
#include "bindings.h"
using namespace j3d::pipeline;
namespace j3d::pipeline {
/* 027F7AE4 */
u32 PipelineOptionReference::flagValue() const {
    constexpr u32 kSelectedFlag=4;
    u32 flags=mDescriptor->mFlags;
    return (flags&kSelectedFlag)!=0?2u:0u;
}
/* 027F7B00 */
s32 PipelineOptionReference::selection() const { return mDescriptor->mSelection; }
/* 027F81A4 */
u32 PipelineSlotTable::word(u32 index) const {
    const PipelineSlot_l* slot=&mSlots[0];
    if(index<kSlotCount) slot=&mSlots[index];
    return slot->mWord;
}
/* 027F8E5C */
u32 PipelineSlotTable::alternateWord(u32 index) const {
    const PipelineSlot_l* slot=&mSlots[0];
    if(index<kSlotCount) slot=&mSlots[index];
    return slot->mAlternateWord;
}
/* 027F76E4 */
void PipelineTextTermination::ensureLastByte(sead::SafeTextBuffer* text) {
    be<u8>* storage=text->mText;
    u32 capacity=static_cast<u32>(static_cast<s32>(text->mCapacity));
    *gabi::at<be<u8>>(gabi::ea(storage)+capacity-1)=0;
}
/* 027F7B0C */
void PipelineBufferTermination::ensureLastByte(sead::SafeTextBuffer* text) {
    be<u8>* storage=text->mText;
    u32 capacity=static_cast<u32>(static_cast<s32>(text->mCapacity));
    *gabi::at<be<u8>>(gabi::ea(storage)+capacity-1)=0;
}
}
/* WWHD glue
*/
