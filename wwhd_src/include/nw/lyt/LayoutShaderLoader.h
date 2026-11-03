/* Layout shader-header/program loading on the merged nw::ut SDK-boundary views. */
#pragma once
#include "nw/lyt/LayoutRenderStorage.h"
namespace nw { namespace lyt {
struct LayoutShaderLoader {
    static u32 loadVertex(void* cache,gptr<nw::ut::VertexShader>* output,u32 index,const void* archive);
    static u32 loadPixel(void* cache,gptr<nw::ut::PixelShader>* output,u32 index,const void* archive);
};
} }
