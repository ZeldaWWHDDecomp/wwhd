#pragma once
#include "sead/ColorChannels.h"
namespace j3d::pipeline {
struct PipelineParameterSet_l;
struct PipelineColorTable_l {
    be<u32> mFlags;
    sead::ColorChannels mColors[11];
    static sead::ColorChannels* color(PipelineColorTable_l* table,u32 index);
    void applyParameters(PipelineParameterSet_l* parameters);
};
}
WWHD_OFFSET(j3d::pipeline::PipelineColorTable_l,mColors,4);
WWHD_SIZE(j3d::pipeline::PipelineColorTable_l,0xB4);
