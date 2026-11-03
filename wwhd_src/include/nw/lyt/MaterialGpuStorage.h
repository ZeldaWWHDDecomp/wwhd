/* Material parameter storage records, reconstructed from allocation strides and initializers. */
#pragma once
#include "nw/lyt/LayoutResourceCompanions.h"
#include "nw/lyt/ResourceHelpers.h"
namespace nw { namespace lyt {
struct MaterialTextureTransform { be<f32> translateX,translateY,rotation,scaleX,scaleY; };
struct MaterialAlphaTest { be<u8> function; u8 padding[3]; be<f32> reference; };
struct MaterialBlend { be<u32> value; };
struct MaterialIndirect { be<f32> rotation,scaleX,scaleY; };
struct MaterialProjection { be<u8> bytes[8]; };
WWHD_SIZE(MaterialTextureTransform,0x14); WWHD_SIZE(MaterialAlphaTest,8);
WWHD_SIZE(MaterialBlend,4); WWHD_SIZE(MaterialIndirect,0xC); WWHD_SIZE(MaterialProjection,8);
} }
void materialStorageDelete(void* object);
void materialStorageClear(void* storage,u32 value,u32 size);

nw::lyt::LayoutMaterialMethods* materialGpuMethodTable();
