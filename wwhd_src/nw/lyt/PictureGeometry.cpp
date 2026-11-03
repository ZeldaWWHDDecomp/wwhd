/* Picture material ownership and coordinate capacity callbacks from WWHD. */
#include "nw/lyt/PictureGeometry.h"
#include "nw/lyt/ResourceHelpers.h"
#include "nw/lyt/ResourceProvider.h"
using namespace nw::lyt;
static u8 sPictureMethods;


struct PictureResource {
    u8 prefix[0x54]; be<u8> colorBytes[16]; be<u16> materialIndex;
    be<u8> coordinateCount, reserved; LayoutTextureCoordinates coordinates[1];
};
struct PictureMaterialContext { u8 prefix[8]; gptr<be<u32>> resources; };
struct PictureBuildContext {
    u8 prefix[0x1C]; gptr<PictureMaterialContext> materials, alternateMaterials;
};
struct PictureColorConversion { be<u32> word; };

/* 02874AAC */
LayoutPicture* LayoutPicture::create(LayoutPicture* pane, void* input, void* overrideInput, void* inputContext) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* resource = static_cast<PictureResource*>(input);
    auto* alternate = static_cast<PictureResource*>(overrideInput);
    auto* context = static_cast<PictureBuildContext*>(inputContext);
    if (!pane) pane = static_cast<LayoutPicture*>(paneStateAllocate(sizeof(LayoutPicture)));
    if (!pane) return pane;
    LayoutPane::createFromResource(reinterpret_cast<LayoutPane*>(pane), resource, context);
    reinterpret_cast<LayoutPane*>(pane)->mVtable = gabi::ea(&sPictureMethods);
    pictureConstructColors(pane->colors, 4, 4, gabi::guest_fn((const void*)VertexColor::create));
    pictureCreateCoordinates(&pane->textureCoordinates);
    PictureResource* selected;
    PictureMaterialContext* materials;
    u32 count;
    if (alternate) {
        selected = alternate;
        count = selected->coordinateCount;
        materials = context->alternateMaterials;
    } else {
        selected = resource;
        count = selected->coordinateCount;
        materials = context->materials;
    }
    if (count > 3) count = 3;
    pane->ensureTextureCoordinateCapacity(count);
    for (u32 index = 0; index < 4; ++index) {
        u32 offset = index * 4;
        u32 red = selected->colorBytes[offset];
        u32 green = selected->colorBytes[offset + 1];
        u32 blue = selected->colorBytes[offset + 2];
        u32 alpha = selected->colorBytes[offset + 3];
        gabi::FrameLocal<PictureColorConversion> converted(8);
        converted->word = (red << 24) | (green << 16) | (blue << 8) | alpha;
        auto* bytes = reinterpret_cast<be<u8>*>(&pane->colors[index]);
        u32 word = converted->word;
        bytes[0] = word >> 24;
        bytes[1] = word >> 16;
        bytes[2] = word >> 8;
        bytes[3] = word;
    }
    if (count != 0 && u8(pane->textureCoordinates.capacity) != 0)
        pictureSetCoordinates(&pane->textureCoordinates, selected->coordinates, count);
    u32 materialIndex = selected->materialIndex;
    be<u32>* resources = materials->resources;
    u32 offset = resources[3 + materialIndex];
    void* materialResource = layoutAtOffset<void>(resources, offset);
    auto* material = static_cast<LayoutMaterial*>(pictureAlignedAllocate(sizeof(LayoutMaterial), 4));
    if (material) pane->material = pictureCreateMaterial(material, materialResource, materials);
    else pane->material = nullptr;
    return pane;
}

/* 02874A98 */
void LayoutPicture::reserveTextureCoordinates(u32 count) {
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)pictureReserveCoordinates), &textureCoordinates, count);
}

/* 02874AA0 */
void LayoutPicture::ensureTextureCoordinateCapacity(u32 count) {
    if (count == 0) return;
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)pictureReserveCoordinatesBinding), this, count);
}

/* 02874C64 */
void LayoutPicture::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    auto* owned = static_cast<LayoutMaterial*>((void*)material);
    reinterpret_cast<LayoutPane*>(this)->mVtable = gabi::ea(&sPictureMethods);
    if (owned && !(u8(owned->ownershipFlags) & 1)) {
        LayoutMaterialMethods* dispatch = owned->methods;
        gabi::call_ptr<void>(dispatch->destroy, owned, 2u);
        pictureRelease(owned);
        material = nullptr;
    }
    pictureClearCoordinates(&textureCoordinates);
    reinterpret_cast<LayoutPane*>(this)->destroy(0);
    if (flags & 1) pictureDelete(this);
}


struct PictureDrawMethods { u8 prefix[0x9C]; be<u32> prepare; };
struct PictureMaterialDrawMethods { u8 prefix[0x24]; be<u32> prepare; };
struct PictureMatrixOrigin { be<f32> x, y; };
struct PictureOriginWords { be<u32> x, y; };
struct PictureUniformColors { be<f32> values[16]; };
struct PictureIntegerConversion { be<u32> high, low; };
struct PictureWhiteScratch { be<u8> bytes[20]; };

/* 02874D54 */
void LayoutPicture::draw(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!(void*)material) return;
    auto* pane = reinterpret_cast<LayoutPane*>(this);
    auto* methods = paneMethods<PictureDrawMethods>(pane);
    gabi::call_ptr<void>(methods->prepare, this, context);
    gabi::FrameLocal<PictureWhiteScratch> white(0x60);
    auto* scratch = white->bytes;
    *reinterpret_cast<be<u32>*>(scratch) = 0xFFFFFFFF;
    for (u32 index = 0; index < 4; ++index) scratch[13 + index] = 0xFF;
    u32 first = *reinterpret_cast<be<u32>*>(scratch + 12);
    u32 second = *reinterpret_cast<be<u32>*>(scratch + 16);
    *reinterpret_cast<be<u32>*>(scratch + 4) = first;
    *reinterpret_cast<be<u32>*>(scratch + 8) = second;
    bool allWhite = true;
    for (u32 index = 0; index < 4; ++index) {
        u32 reference = *reinterpret_cast<be<u32>*>(scratch + 5);
        if (u32(colors[index].value) != reference) { allWhite = false; break; }
    }
    u32 alpha = pane->content()->inheritedAlpha;
    bool opaqueWhite = allWhite && alpha == 255;
    auto* currentMaterial = static_cast<LayoutMaterial*>((void*)material);
    auto* materialMethods = reinterpret_cast<PictureMaterialDrawMethods*>((LayoutMaterialMethods*)currentMaterial->methods);
    f32 matrixY = pane->content()->matrix.m[1][3];
    f32 matrixX = pane->content()->matrix.m[0][3];
    if (opaqueWhite) {
        gabi::FrameLocal<PictureMatrixOrigin> origin(0x18);
        origin->y = matrixY;
        origin->x = matrixX;
        gabi::call_ptr<void>(materialMethods->prepare, currentMaterial, context, u32(255), u32(1), u32(1), origin.get());
    } else {
        gabi::FrameLocal<PictureMatrixOrigin> origin(0x10);
        origin->y = matrixY;
        origin->x = matrixX;
        gabi::call_ptr<void>(materialMethods->prepare, currentMaterial, context, alpha, u32(0), u32(1), origin.get());
        u32 location = context->uniformLocation(13);
        if (location != 0xFFFFFFFF) {
            gabi::FrameLocal<PictureIntegerConversion> conversion(8);
            gabi::FrameLocal<PictureUniformColors> uniformColors(0x20);
            auto* bytes = reinterpret_cast<be<u8>*>(colors);
            conversion->high = 0x43300000;
            for (u32 index = 0; index < 16; ++index) {
                u32 component = bytes[index];
                conversion->low = component;
                uniformColors->values[index] = f32(component);
            }
            pictureDrawVertexUniforms(location, 16, uniformColors.get());
        }
    }
    auto origin = pane->originOffset();
    LayoutTextureCoordinates* coordinates = textureCoordinates.coordinates;
    gabi::FrameLocal<PictureOriginWords> position(0x74);
    position->x = origin.r3;
    u32 count = textureCoordinates.count;
    position->y = origin.r4;
    pictureDrawQuad(context, reinterpret_cast<const be<u32>*>(position.get()), &pane->content()->width, count, coordinates);
}

/* WWHD glue
ext C0006A40 void pictureDrawVertexUniforms(u32, u32, const void*)
ext 02880F1C void pictureDrawQuad(nw::lyt::LayoutDrawContext*, const be<u32>*, const be<f32>*, u32, nw::lyt::LayoutTextureCoordinates*)
ext 028EFFD0 void pictureConstructColors(void*, u32, u32, u32)
ext 02880CE8 void pictureCreateCoordinates(nw::lyt::TextureCoordinateStorage*)
ext 02880E24 void pictureSetCoordinates(nw::lyt::TextureCoordinateStorage*, const nw::lyt::LayoutTextureCoordinates*, u32)
ext 0287F3DC void* pictureAlignedAllocate(u32, s32)
ext 028815D8 nw::lyt::LayoutMaterial* pictureCreateMaterial(nw::lyt::LayoutMaterial*, void*, void*)
ext 0273AF40 void pictureDelete(void*)
ext 0287F3FC void pictureRelease(void*)
ext 02880D2C void pictureClearCoordinates(nw::lyt::TextureCoordinateStorage*)
ext 02880D84 void pictureReserveCoordinates(nw::lyt::TextureCoordinateStorage*, u32) callback
ext 02874A98 void pictureReserveCoordinatesBinding(nw::lyt::LayoutPicture*, u32) callback
data sPictureMethods 101803D0 nocheck
*/
