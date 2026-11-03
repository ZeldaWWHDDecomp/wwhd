// format facts: BFLIM footer tags and image fields; implementation reconstructed from WWHD.
/* Layout texture descriptors and the FLIM footer scan, reconstructed from WWHD. */
#include "nw/lyt/LayoutTextureDecoder.h"
using namespace nw::lyt;
struct TextureComponentDefaults { be<u32> words[54]; };
struct TextureFormatWords { be<u32> words[25]; };
static TextureComponentDefaults sTextureComponentDefaults;
static TextureFormatWords sTextureFormats;
static TextureFormatWords sTextureComponents;
/* 0287EDA0 */
void TextureResourceDecoder::initialize(TextureObject* output,const LayoutTextureParameters* parameters) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 width=parameters->width;
    u32 format=parameters->format;
    u32 height=parameters->height;
    gabi::FrameLocal<be<u32>[54]> defaults(8);
    for (u32 i=0;i<54;++i) (*defaults)[i]=static_cast<u32>(sTextureComponentDefaults.words[i]);
    output->width=width;
    output->usage=1;
    u32 components=(*defaults)[format&63];
    output->mipLevels=1;
    output->dimension=1;
    output->components=components;
    output->antiAlias=0;
    output->format=format;
    output->firstSlice=0;
    output->firstMip=0;
    output->height=height;
    output->sliceCount=1;
    output->viewMipLevels=1;
    output->tile=0;
    output->depth=1;
    output->swizzle=0;
    textureDecoderCalculate(output);
    textureDecoderInitialize(output);
    output->tile=parameters->tileMode;
    u32 swizzle=parameters->swizzle;
    output->swizzle=static_cast<u32>(output->swizzle)|(swizzle<<8);
    u32 image=parameters->image;
    u32 mipLevels=output->mipLevels;
    output->image=image;
    u32 mipmap=0;
    if (mipLevels>1) mipmap=image+static_cast<u32>(output->mipOffsets[0]);
    output->mipmaps=mipmap;
    output->components=parameters->components;
    textureDecoderInitialize(output);
}
/* 0287EEC4 */
u32 TextureResourceDecoder::decode(TextureResourceView* output,const void* data,u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 offset=*layoutAtOffset<const be<u32>>(data,size-4);
    FontFile* footer=layoutAtOffset<FontFile>(data,(offset+3)&~3u);
    LayoutImageBlock* image=nullptr;
    if (!layoutValidateResourceHeader(footer,0x464C494Du,0x02020000u,1)) return 0;
    FontBlock* previous=nullptr;
    for (s32 i=0;i<static_cast<s32>(static_cast<u32>(footer->blockCount));++i) {
        previous=layoutNextResourceBlock(footer,previous);
        if (previous->tag==0x696D6167u) image=reinterpret_cast<LayoutImageBlock*>(previous);
    }
    if (!image) return 0;
    u32 height=image->height;
    u32 flags=image->flags;
    u32 width=image->width;
    u32 format=image->format;
    gabi::FrameLocal<LayoutTextureParameters> parameters(0xC);
    parameters->height=height;
    parameters->tileMode=flags&31;
    parameters->components=sTextureComponents.words[format];
    parameters->swizzle=flags>>5;
    parameters->image=gabi::ea(data);
    parameters->width=width;
    parameters->format=sTextureFormats.words[format];
    initialize(&output->surface,parameters.get());
    u32 finalFormat=image->format;
    output->descriptorWord=1;
    output->format=finalFormat;
    output->width=width;
    output->height=height;
    return 1;
}
/* WWHD glue
data sTextureComponentDefaults 101817A4
data sTextureFormats 1018187C
data sTextureComponents 101818E0
ext C00060D0 void textureDecoderCalculate(nw::lyt::TextureObject*)
ext C0006498 void textureDecoderInitialize(nw::lyt::TextureObject*)
ext 02871158 u32 layoutValidateResourceHeader(nw::lyt::FontFile*,u32,u32,u32)
ext 028711E4 nw::lyt::FontBlock* layoutNextResourceBlock(nw::lyt::FontFile*,nw::lyt::FontBlock*)
*/
