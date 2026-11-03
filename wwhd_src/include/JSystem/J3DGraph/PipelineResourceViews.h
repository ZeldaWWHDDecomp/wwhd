#pragma once
#include "wwhd.h"
namespace j3d::pipeline {
struct RelativeResourceHeader_l {
    u8 mPrefix[12];
    be<u32> mRelativeData;
};
struct PipelineResourceView {
    gptr<RelativeResourceHeader_l> mResource;
    RelativeResourceHeader_l* resource() const;
    void* relativeData() const;
};
struct PipelineWordReference {
    be<u32> mWord;
    void setWord(u32);
    u32 word() const;
};
}
WWHD_OFFSET(j3d::pipeline::RelativeResourceHeader_l,mRelativeData,12);
WWHD_SIZE(j3d::pipeline::RelativeResourceHeader_l,16);
WWHD_OFFSET(j3d::pipeline::PipelineResourceView,mResource,0);
WWHD_SIZE(j3d::pipeline::PipelineResourceView,4);
WWHD_OFFSET(j3d::pipeline::PipelineWordReference,mWord,0);
WWHD_SIZE(j3d::pipeline::PipelineWordReference,4);
