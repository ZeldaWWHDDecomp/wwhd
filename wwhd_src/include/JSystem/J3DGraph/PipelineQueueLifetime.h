#pragma once
#include "JSystem/J3DGraph/PipelineFrameQueue.h"
namespace j3d::pipeline {
struct PipelineQueueLifetime {
    static void destroyDisposer(sead::Disposer* disposer,u32 flags);
    static void destroyRegistration(sead::EmbeddedObject* object,u32 flags);
    static void releaseQueueStorage(sead::JobQueue* queue,u32 flags);
    static void releaseOpaqueStorage(void* object,u32 flags);
};
}
