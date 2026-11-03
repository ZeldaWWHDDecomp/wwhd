/* Layout texture footer and GX2 descriptor setup, reconstructed from binary loads and calls. */
#pragma once
#include "nw/lyt/LayoutGpuResources.h"
#include "nw/lyt/ResourceHelpers.h"
namespace nw { namespace lyt {
struct LayoutTextureParameters { be<u32> image,width,height,format,tileMode,swizzle,components; };
struct LayoutImageBlock { be<u32> tag,size; be<u16> width,height,alignment; be<u8> format,flags; };
struct TextureResourceDecoder {
    static void initialize(TextureObject* output,const LayoutTextureParameters* parameters);
    static u32 decode(TextureResourceView* output,const void* data,u32 size);
};
WWHD_SIZE(LayoutTextureParameters,0x1C); WWHD_SIZE(LayoutImageBlock,0x10);
WWHD_OFFSET(LayoutImageBlock,format,0xE);
} }
void textureDecoderCalculate(nw::lyt::TextureObject* texture);
void textureDecoderInitialize(nw::lyt::TextureObject* texture);
