// Release a nonzero surface-slot handle using its recorded release mode.
#include "JSystem/J3DGraph/PipelineLookupInstances.h"
using namespace j3d::pipeline;
static void* sPipelineSlotCache;
void releasePipelineDefaultSlot(void*,u32);
void releasePipelineAlternateSlot(void*,u32);
namespace j3d::pipeline {
/* 027F88C8 */
void PipelineSlotTable::releaseWord(u32 index) {
    gabi::AutoFrame frame{0x027F88C8u};
    u32 handle=word(index);
    if(handle==0) return;
    u32 selected=index<kSlotCount?index:0;
    u8 mode=mSlots[selected].mReleaseMode;
    void* cache=sPipelineSlotCache;
    if(mode!=0) releasePipelineAlternateSlot(cache,handle);
    else releasePipelineDefaultSlot(cache,handle);
    mSlots[selected].mWord=0;
}
}
/* WWHD glue
data sPipelineSlotCache 101F8DD4
ext 027A0040 void releasePipelineDefaultSlot(void*,u32)
ext 027A0114 void releasePipelineAlternateSlot(void*,u32)
*/
