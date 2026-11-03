/* agl::detail::TextureDataUtil: texture format conversion and surface helpers. Reconstructed from this game's
 * instructions; the table holds GX2 surface-format codes (values from the binary). */
#include "agl/TextureDataUtil.h"
using namespace agl;
using namespace agl::detail;

static void calculateSurfaceSizeAndAlignment(sead::GraphicsSurface* surface);

/* GX2 surface format of each library texture format (index 0: invalid) */
static u32 sSurfaceFormats[0x3C] = {
    0x000, 0x001, 0x101, 0x201, 0x301, 0x005, 0x105, 0x205, 0x305, 0x806,
    0x007, 0x107, 0x207, 0x307, 0x008, 0x00C, 0x00B, 0x00A, 0x10D, 0x30D,
    0x80E, 0x00F, 0x10F, 0x20F, 0x30F, 0x810, 0x816, 0x01B, 0x11B, 0x01A,
    0x11A, 0x21A, 0x31A, 0x41A, 0x019, 0x119, 0x11D, 0x31D, 0x81E, 0x01F,
    0x11F, 0x21F, 0x31F, 0x820, 0x122, 0x322, 0x823, 0x031, 0x431, 0x032,
    0x432, 0x033, 0x433, 0x034, 0x234, 0x035, 0x235, 0x005, 0x80E, 0x011,
};

/* 02786314 */
u32 TextureDataUtil::convertFromGX2Format(u32 surfaceFormat) {
    // the first match wins: some GX2 formats appear twice
    for (u32 format = 0; format < kTextureFormatCount; ++format)
        if (surfaceFormat == sSurfaceFormats[format]) return format;
    return 0;
}

/* 02786348 */
u32 TextureDataUtil::convertToGX2Format(u32 format) {
    return sSurfaceFormats[format];
}

/* 0278635C */
void TextureDataUtil::calculateSizeAndAlignment(sead::GraphicsSurface* surface) {
    calculateSurfaceSizeAndAlignment(surface);
}

/* 02786360 */
void TextureDataUtil::initializeFromSurface(TextureData* textureData, const sead::GraphicsSurface* surface) {
    textureData->copySurface(surface);
}

/* WWHD glue
data sSurfaceFormats 101F8C24
ext C00060D0 void calculateSurfaceSizeAndAlignment(sead::GraphicsSurface*)
ext 027BEA90 void agl::TextureData::copySurface(const sead::GraphicsSurface*)
*/
