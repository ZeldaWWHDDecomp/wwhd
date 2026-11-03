// Small HD pipeline constructors, including allocation-on-NULL entry forms.
#include "JSystem/J3DGraph/PipelineInlineState.h"
#include "bindings.h"
using namespace j3d::pipeline;
void* allocatePipelineStorage(u32);
void releasePipelineStorage(void*);
static PipelineVectorPair sDefaultPipelineVectorPair;
namespace j3d::pipeline {
/* 027F74D4 */
PipelineFlagState* PipelineFlagState::create(PipelineFlagState* state) {
    gabi::AutoFrame frame{0x027F74D4u};
    if(state==nullptr) state=static_cast<PipelineFlagState*>(allocatePipelineStorage(sizeof(PipelineFlagState)));
    if(state!=nullptr) { state->mFlag=0; state->mWord=0; }
    return state;
}
/* 027F7514 */
PipelineVectorPair* PipelineVectorPair::create(PipelineVectorPair* state) {
    gabi::AutoFrame frame{0x027F7514u};
    if(state==nullptr) state=static_cast<PipelineVectorPair*>(allocatePipelineStorage(sizeof(PipelineVectorPair)));
    if(state!=nullptr) {
        for(u32 vector=0;vector!=2;++vector)
            for(u32 component=0;component!=3;++component) {
                u32 bits=sDefaultPipelineVectorPair.mVectors[vector][component].bits;
                state->mVectors[vector][component].bits=bits;
            }
        state->mFlag=sDefaultPipelineVectorPair.mFlag;
    }
    return state;
}
/* 027F7654 */
PipelineStorageRecord* PipelineStorageRecord::create(PipelineStorageRecord* state) {
    gabi::AutoFrame frame{0x027F7654u};
    if(state==nullptr) state=static_cast<PipelineStorageRecord*>(allocatePipelineStorage(sizeof(PipelineStorageRecord)));
    return state;
}
}
/* WWHD glue
data sDefaultPipelineVectorPair 1016E394
ext 0273AD10 void* allocatePipelineStorage(u32)
ext 0273AF40 void releasePipelineStorage(void*)
*/
