// Entry completion callback: the global model may change inside the optional entry callback.
#include "JSystem/J3DGraph/PipelineEntryCursor.h"
using namespace j3d::pipeline;
static ModelPipeline_l* sPipelineCallbackModel;
namespace j3d::pipeline {
/* 027F59B4 */
void PipelineEntryCursor::advance(void*,PipelineEntryCursor* cursor) {
    gabi::AutoFrame frame{0x027F59B4u};
    be<u32>* indexAddress=cursor->mIndex;
    ModelPipeline_l* model=sPipelineCallbackModel;
    u32 index=*indexAddress;
    ModelDataPipeline_l* data=model->mModelData;
    u32 lookupIndex=static_cast<u16>(index);
    u32 count=data->mEntryCount;
    MaterialResourceEntry_l* entry=data->mEntries;
    if(lookupIndex<count) entry=&entry[lookupIndex];
    u32 callback=entry->mCallback.get();
    if(callback!=0) {
        gabi::call_ptr<void>(callback,entry,0);
        model=sPipelineCallbackModel;
    }
    PipelineJointEntryCount_l* jointCount=model->mJointEntryCount;
    u32 limit=jointCount->mCount;
    s32 next=static_cast<s32>(index+1u);
    cursor->mNextIndex=next<static_cast<s32>(limit)?next:-1;
}
}
/* WWHD glue
data sPipelineCallbackModel 104B462C
*/
