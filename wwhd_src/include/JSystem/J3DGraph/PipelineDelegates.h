#pragma once
#include "JSystem/J3DGraph/PipelineFrameQueue.h"
namespace j3d::pipeline {
struct PipelineDelegateMethods_l { u8 mPrefix[12]; gfn mInvoke; };
struct PipelineDelegate_l : sead::instances::DelegateLayout {
    void invokeProvider();
    void invokeWork();
    void invokeCompletion();
    void invokeJob(void* context,sead::JobItem* job);
};
}
WWHD_OFFSET(j3d::pipeline::PipelineDelegateMethods_l,mInvoke,12);
WWHD_SIZE(j3d::pipeline::PipelineDelegateMethods_l,16);
WWHD_SIZE(j3d::pipeline::PipelineDelegate_l,16);
