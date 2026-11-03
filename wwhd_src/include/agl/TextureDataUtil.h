/* agl::detail::TextureDataUtil: conversion between the library's texture formats and GX2 surface formats, and the
 * surface helpers forwarded to GX2 and to agl::TextureData. From this game's instructions. */
#pragma once
#include "agl/TextureData.h"

namespace agl { namespace detail {

/* number of library texture formats (entries of the GX2 format table) */
static constexpr u32 kTextureFormatCount = 0x3C;

struct TextureDataUtil {
    static u32 convertFromGX2Format(u32 surfaceFormat);
    static u32 convertToGX2Format(u32 format);
    static void calculateSizeAndAlignment(sead::GraphicsSurface* surface);
    static void initializeFromSurface(TextureData* textureData, const sead::GraphicsSurface* surface);
};

} }
