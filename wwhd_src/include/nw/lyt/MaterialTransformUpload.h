/* Material coordinate generators and projection transforms. */
#pragma once
#include "nw/lyt/LayoutTextureMatrix.h"
#include "nw/lyt/TextureBinding.h"
namespace nw { namespace lyt {
struct MaterialCoordinate { be<u8> kind,source; u8 padding[2]; gptr<void> projection; };
struct MaterialProjectionTransform { be<f32> x,y,width,height; be<u8> flags; u8 padding[3]; };
WWHD_SIZE(MaterialCoordinate,8); WWHD_SIZE(MaterialProjectionTransform,0x14);
} }
void materialTransformUniforms(u32,u32,const void*);
