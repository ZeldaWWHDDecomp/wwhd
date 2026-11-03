// Initial slot formats and allocation order from own 027F7F08 jump table.
#include "JSystem/J3DGraph/PipelineLookupInstances.h"
#include "sead/HeapRegistry.h"
using namespace j3d::pipeline;
void* allocatePipelineSlotStorage(u32,sead::RegistryHeap*,s32);
void* constructPipelineFirstSlotStorage(void*);
void* constructPipelineSecondSlotStorage(void*);
namespace j3d::pipeline {
/* 027F7F08 */
void PipelineSlotTable::initializeSlots(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F7F08u};
    for(u32 index=0;index!=kSlotCount;++index) {
        PipelineSlot_l* slot=&mSlots[index];
        void* first=allocatePipelineSlotStorage(0x40,heap,4);
        if(first!=nullptr) first=constructPipelineFirstSlotStorage(first);
        slot->mFirstStorage=first;
        void* second=allocatePipelineSlotStorage(0x148,heap,4);
        if(second!=nullptr) second=constructPipelineSecondSlotStorage(second);
        slot->mStateEnabled=0;
        slot->mWord=0;
        slot->mSecondStorage=second;
        slot->mAlternateWord=0;
        bool release=(index>=3 && index<=4) || (index>=6 && index<=11) ||
                     (index>=13 && index<=14) || (index>=17 && index<=18);
        slot->mReleaseMode=release?1:0;
        switch(index) {
        case 0:
            slot->mAlternateEnabled=0;slot->mFormatWord=9;slot->mModeWord=1;break;
        case 1:
            slot->mAlternateEnabled=1;slot->mFormatWord=9;slot->mModeWord=0;break;
        case 2:
            slot->mFormatWord=0x3A;slot->mModeWord=0;
            slot->mAlternateEnabled=1;slot->mStateEnabled=1;break;
        case 3:case 5:
            slot->mAlternateEnabled=0;slot->mFormatWord=0x22;slot->mModeWord=0;break;
        case 4:case 13:
            slot->mAlternateEnabled=1;slot->mFormatWord=0x22;slot->mModeWord=1;break;
        case 6:case 11:case 14:
            slot->mAlternateEnabled=0;slot->mFormatWord=0x22;slot->mModeWord=1;break;
        case 7:case 8:case 9:case 17:case 18:
            slot->mAlternateEnabled=0;slot->mFormatWord=0x1D;slot->mModeWord=1;break;
        case 10:
            slot->mAlternateEnabled=0;slot->mFormatWord=0x3A;slot->mModeWord=1;break;
        case 12:
            slot->mAlternateEnabled=0;slot->mFormatWord=1;slot->mModeWord=0;break;
        case 15:
            slot->mModeWord=0;slot->mFormatWord=0x22;slot->mAlternateEnabled=1;break;
        case 16:
            slot->mAlternateEnabled=1;slot->mFormatWord=1;slot->mModeWord=0;break;
        }
    }
}
}
/* WWHD glue
ext 0273B050 void* allocatePipelineSlotStorage(u32,sead::RegistryHeap*,s32)
ext 027B95B4 void* constructPipelineFirstSlotStorage(void*)
ext 027B6CB8 void* constructPipelineSecondSlotStorage(void*)
*/
