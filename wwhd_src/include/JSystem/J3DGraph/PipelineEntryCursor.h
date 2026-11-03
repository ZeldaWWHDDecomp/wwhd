#pragma once
#include "JSystem/J3DGraph/ModelPipeline.h"
namespace j3d::pipeline {
struct PipelineEntryCursor {
    gptr<be<u32>> mIndex;
    be<s32> mNextIndex;
    static void advance(void* receiver,PipelineEntryCursor* cursor);
};
}
WWHD_OFFSET(j3d::pipeline::PipelineEntryCursor,mNextIndex,4);
WWHD_SIZE(j3d::pipeline::PipelineEntryCursor,8);
