#pragma once
#include "wwhd.h"
namespace j3d::pipeline {
struct StaticWords_l { be<u32> mWords[4]; };
struct ExitRegistration_l { be<u32> mWords[3]; };
struct PipelineStaticState {
    static void initializeModelState();
    static void initializeResourceState();
    static void initializeValueState();
    static void initializeDrawState();
    static void initializeQueueState();
    static void initializeSurfaceNames();
};
}
WWHD_SIZE(j3d::pipeline::StaticWords_l,16);
WWHD_SIZE(j3d::pipeline::ExitRegistration_l,12);
