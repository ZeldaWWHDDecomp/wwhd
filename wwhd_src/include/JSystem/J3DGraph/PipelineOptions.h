#pragma once
#include "JSystem/J3DGraph/PipelineLookupInstances.h"
namespace sead { struct RegistryHeap; }
namespace j3d::pipeline {
struct PipelineOptionNameText_l : sead::SafeText {
    static void destroy(PipelineOptionNameText_l*,u32);
    static void assureTermination(PipelineOptionNameText_l*);
};
struct PipelineOptionBaseText_l : sead::SafeText { static void destroy(PipelineOptionBaseText_l*,u32); };
struct PipelineOptionBaseBuffer_l : sead::SafeTextBuffer { static void destroy(PipelineOptionBaseBuffer_l*,u32); };
struct PipelineOptionText_l : sead::FixedSafeText<32> { static void destroy(PipelineOptionText_l*,u32); };
struct PipelineNamedOption_l : PipelineOptionReference {
    PipelineOptionText_l mName;
    static PipelineNamedOption_l* create(OptionDescriptor_l*,sead::SafeText*,sead::RegistryHeap*);
};
struct PipelineScalarBinding_l {
    be<f32> mScalar; be<u32> mMethods,mWord8; gptr<void> mSource; be<u32> mWord10;
    static PipelineScalarBinding_l* create(void*,sead::RegistryHeap*);
};
}
WWHD_SIZE(j3d::pipeline::PipelineOptionNameText_l,8);
WWHD_SIZE(j3d::pipeline::PipelineOptionBaseText_l,8);
WWHD_SIZE(j3d::pipeline::PipelineOptionBaseBuffer_l,12);
WWHD_SIZE(j3d::pipeline::PipelineOptionText_l,0x2C);
WWHD_OFFSET(j3d::pipeline::PipelineNamedOption_l,mName,0xC);
WWHD_SIZE(j3d::pipeline::PipelineNamedOption_l,0x38);
WWHD_OFFSET(j3d::pipeline::PipelineScalarBinding_l,mSource,0xC);
WWHD_SIZE(j3d::pipeline::PipelineScalarBinding_l,0x14);
