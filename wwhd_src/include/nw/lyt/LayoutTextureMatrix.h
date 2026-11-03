/* Two-row texture-coordinate matrix and its material transform input. */
#pragma once
#include "nw/lyt/MaterialGpuStorage.h"
namespace nw { namespace lyt {
struct LayoutTextureMatrix {
    be<f32> horizontalScale,horizontalCross,horizontalOffset;
    be<f32> verticalCross,verticalScale,verticalOffset;
    void configure(const MaterialTextureTransform* transform);
    void configureIndirect(const PaneSize* scale,f32 angle);
};
WWHD_SIZE(LayoutTextureMatrix,0x18);
} }
Pair32 textureMatrixConvertAngle(f64 angle);
