#pragma once
#include "sead/FileDevice.h"
namespace j3d::pipeline {
// Only the shared buffered-text prefix is established for this method-table copy.
struct PipelineSecondaryTextBufferPrefix_l : sead::SafeTextBuffer {
    static void destroy(PipelineSecondaryTextBufferPrefix_l*,u32);
};
struct PipelineSurfaceNameText_l : sead::SafeText {
    static void destroy(PipelineSurfaceNameText_l*,u32);
    static void assureTermination(PipelineSurfaceNameText_l*);
};
struct PipelineTaskNameText_l : sead::SafeText {
    static void assureTermination(PipelineTaskNameText_l*);
};
struct PipelineValueNameText_l : sead::SafeText {
    static void destroy(PipelineValueNameText_l* object,u32 flags);
    static void assureTermination(PipelineValueNameText_l* object);
};
struct PipelineBaseText_l : sead::SafeText {
    static void destroy(PipelineBaseText_l* object,u32 flags);
};
struct PipelineBaseTextBuffer_l : sead::SafeTextBuffer {
    static void destroy(PipelineBaseTextBuffer_l* object,u32 flags);
};
struct PipelineFormattedText_l : sead::FixedSafeText<128> {
    static void destroy(PipelineFormattedText_l* object,u32 flags);
    // Guest variadic entry adapter: trailing arguments remain in the GHS register bank.
    static PipelineFormattedText_l* create(PipelineFormattedText_l* object,const char* format);
};
}
WWHD_SIZE(j3d::pipeline::PipelineFormattedText_l,0x8C);

WWHD_SIZE(j3d::pipeline::PipelineValueNameText_l,8);
WWHD_SIZE(j3d::pipeline::PipelineBaseText_l,8);
WWHD_SIZE(j3d::pipeline::PipelineBaseTextBuffer_l,12);
