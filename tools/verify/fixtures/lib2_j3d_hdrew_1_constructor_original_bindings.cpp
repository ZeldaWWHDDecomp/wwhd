// Original-call bindings for the direct shipped-resource constructor fixture.
// Canonical autoglue ext wrappers route dependencies to the original guest functions.
#include "JSystem/J3DGraph/ModelDataPipeline.h"
#include "sead/HeapRegistry.h"
#include "bindings.h"
using j3d::pipeline::PipelineStorageRecord;
using j3d::pipeline::PipelineVectorPair;
using j3d::pipeline::PipelineFlagState;
using sead::CompactRenderState;
/* WWHD glue
ext 0273AD10 void* allocatePipelineStorage(u32)
ext 028EFFD0 void constructPipelineArray(void*,u32,u32,u32)
ext 0274FDBC sead::CompactRenderState* CompactRenderState::create(sead::CompactRenderState*)
ext 027F7654 j3d::pipeline::PipelineStorageRecord* PipelineStorageRecord::create(j3d::pipeline::PipelineStorageRecord*) callback
ext 027F7514 j3d::pipeline::PipelineVectorPair* PipelineVectorPair::create(j3d::pipeline::PipelineVectorPair*) callback
ext 027F74D4 j3d::pipeline::PipelineFlagState* PipelineFlagState::create(j3d::pipeline::PipelineFlagState*) callback
*/
