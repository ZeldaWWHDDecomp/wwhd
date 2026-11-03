#pragma once
#include "JSystem/J3DGraph/PipelineFrameQueue.h"
#include "sead/Foundation.h"
#include "sead/WorkerQueueSupport.h"
namespace j3d::pipeline {
// Exact local storage pending correction of the shared options word currently labelled as a heap.
struct PipelineWorkerSettings_l {
    u8 mBeforeArgument[0x18];
    be<u32> mWorkerArgument;
    u8 mTail[4];
};
}
WWHD_OFFSET(j3d::pipeline::PipelineWorkerSettings_l,mWorkerArgument,0x18);
WWHD_SIZE(j3d::pipeline::PipelineWorkerSettings_l,0x20);
