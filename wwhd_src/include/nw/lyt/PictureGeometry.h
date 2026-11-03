/* Picture geometry extends the existing constructor-proven inline color view. */
#pragma once
#include "nw/lyt/PictureColors.h"
#include "nw/lyt/PaneState.h"
#include "nw/lyt/LayoutMaterial.h"
#include "nw/lyt/LayoutDrawContext.h"
namespace nw { namespace lyt {
struct LayoutTextureCoordinates { be<f32> values[4][2]; };
struct TextureCoordinateStorage {
    be<u8> capacity, count;
    u8 padding[2];
    gptr<LayoutTextureCoordinates> coordinates;
};
struct LayoutPicture : PictureColors {
    TextureCoordinateStorage textureCoordinates;
    static LayoutPicture* create(LayoutPicture*, void* resource, void* alternate, void* context);
    void destroy(u32 flags);
    void draw(LayoutDrawContext* context);
    void reserveTextureCoordinates(u32 count);
    void ensureTextureCoordinateCapacity(u32 count);
};
WWHD_SIZE(TextureCoordinateStorage, 8);
WWHD_SIZE(LayoutTextureCoordinates, 0x20);
WWHD_SIZE(LayoutPicture, 0xC0);
WWHD_OFFSET(LayoutPicture, textureCoordinates, 0xB8);
} }
void pictureDelete(void* storage);
void pictureRelease(void* storage);
void pictureClearCoordinates(nw::lyt::TextureCoordinateStorage*);
void pictureReserveCoordinates(nw::lyt::TextureCoordinateStorage*, u32 count);
void pictureReserveCoordinatesBinding(nw::lyt::LayoutPicture*, u32 count);

void pictureConstructColors(void*, u32 stride, u32 count, u32 constructor);
void pictureCreateCoordinates(nw::lyt::TextureCoordinateStorage*);
void pictureSetCoordinates(nw::lyt::TextureCoordinateStorage*, const nw::lyt::LayoutTextureCoordinates*, u32);
void* pictureAlignedAllocate(u32, s32);
nw::lyt::LayoutMaterial* pictureCreateMaterial(nw::lyt::LayoutMaterial*, void*, void*);

void pictureDrawVertexUniforms(u32, u32, const void*);
void pictureDrawQuad(nw::lyt::LayoutDrawContext*, const be<u32>*, const be<f32>*, u32, nw::lyt::LayoutTextureCoordinates*);
