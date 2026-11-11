// GX2CalcSurfaceSizeAndAlignment model for the harness directive `mocksurfacesize` (README): the shipping port's
// own formula, runtime/third_party/cemu/gx2_surface_calc.cpp (Cemu-derived, MPL-2.0), applied to the guest's
// 0x74-byte big-endian GX2Surface. Built by verify.py with the port's Cemu flags (cemu_shim.h), separately from
// harness.cpp, which only sees the extern "C" entry point.
#include "gx2_surface_calc.h"
#include <cstdint>
#include <algorithm>
#include <cstring>

// Returns 0 after rewriting the descriptor in place, or 1 when its fields are outside what the formula models
// (the harness then aborts the input instead of letting the address library run on garbage).
// The modelled domain of a 0x74-byte descriptor (shared by mocksurfacesize and mockcopysurface): 0 = inside.
static int surface_domain(const GX2Surface& s) {
    uint32_t dim = (uint32_t)(Latte::E_DIM)s.dim, fmt = (uint32_t)(Latte::E_GX2SURFFMT)s.format;
    uint32_t tile = (uint32_t)(Latte::E_GX2TILEMODE)s.tileMode;
    if (dim > 7 || tile > 16 || (fmt & 0x3F) == 0 || (fmt & ~0xF3Fu) != 0 || s.aa > 3)
        return 1;
    // Width/height 0 are modelled: LatteAddrLib clamps each
    // level's extent to max(x >> level, 1) (LatteAddrLib.cpp:961-962, 1083, 1091), so imageSize, alignment and pitch
    // equal the 1-pixel case and the width/height fields stay 0.
    if (s.width > 16384 || s.height > 16384 || s.depth == 0 || s.depth > 2048)
        return 1;
    // The one unsafe zero case: numLevels >= 2 (0 and 1 both mean one level, gx2_surface_calc.cpp:55 and
    // adjust_level_count) where every calc_levels value is 0, i.e. width == 0 and height == 0, and for DIM_3D also
    // depth == 0 (already rejected above). adjust_level_count then returns 0, numLevels becomes 0 and imageSize,
    // alignment and pitch are never written.
    if (s.numLevels > 1 && s.width == 0 && s.height == 0 && (dim != (uint32_t)Latte::E_DIM::DIM_3D || s.depth == 0))
        return 1;
    return 0;
}

extern "C" int verify_calc_surface(uint8_t* bytes) {
    static_assert(sizeof(GX2Surface) == 0x74);
    GX2Surface s;
    memcpy(&s, bytes, sizeof s);
    if (surface_domain(s) != 0)
        return 1;
    gx2calc::CalcSurfaceSizeAndAlignment(&s);
    memcpy(bytes, &s, sizeof s);
    return 0;
}

// GX2CopySurface model for the harness directive `mockcopysurface`: the shipping port's CPU path,
// runtime/src/gfx/metal_surfaces.mm copy_surface_impl (the re-tiling loop after "CPU-produced source"), applied to the
// guest's two big-endian descriptors. The harness has no GPU, so the port's GPU-written-source branch never applies.
// Bytes per block and the block-compressed flag are the port's format_info table (runtime/src/gfx/formats.mm, colour
// branch; copysurface_test.py checks this copy against that file). Returns 0 after emitting, per element in the port's
// loop order (y outer, x inner), the source and destination byte offsets relative to *sbase / *dbase; 1 when the call
// is outside the modelled domain (a descriptor outside surface_domain, a level above 13, or a level base of 0).
extern "C" int verify_copy_bpb(uint32_t fmt, uint32_t* compressed) {
    *compressed = 0;
    switch (fmt & 0x3F) {
    case 0x01: case 0x02: return 1;
    case 0x05: case 0x06: case 0x07: case 0x08: case 0x0A: case 0x0B: case 0x0C: return 2;
    case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x15: case 0x16: case 0x19: case 0x1A: case 0x1B: return 4;
    case 0x1D: case 0x1E: case 0x1F: case 0x20: return 8;
    case 0x22: case 0x23: return 16;
    case 0x31: case 0x34: *compressed = 1; return 8;
    case 0x32: case 0x33: case 0x35: *compressed = 1; return 16;
    case 0x11: case 0x12: case 0x13: case 0x14: return 4;
    default: return 4;
    }
}

static uint32_t copy_level_address(const GX2Surface& s, uint32_t level) {
    if (level == 0) return s.imagePtr;
    if (level == 1) return s.mipPtr;
    return s.mipPtr + s.mipOffset[level - 1];
}

static uint32_t copy_element_offset(const LatteAddrLib::AddrSurfaceInfo_OUT& info, Latte::E_HWTILEMODE tm, uint32_t x, uint32_t y,
                                    uint32_t slice, uint32_t bpp, LatteAddrLib::CachedSurfaceAddrInfo* ci) {
    if (tm == Latte::E_HWTILEMODE::TM_LINEAR_GENERAL || tm == Latte::E_HWTILEMODE::TM_LINEAR_ALIGNED)
        return LatteAddrLib::ComputeSurfaceAddrFromCoordLinear(x, y, slice, 0, bpp, info.pitch, info.height, info.depth);
    if (!Latte::TM_IsMacroTiled(tm))
        return LatteAddrLib::ComputeSurfaceAddrFromCoordMicroTiled(x, y, slice, bpp, info.pitch, info.height, tm, false);
    return LatteAddrLib::ComputeSurfaceAddrFromCoordMacroTiledCached(x, y, ci);
}

extern "C" int verify_copy_surface(const uint8_t* src74, uint32_t srcMip, uint32_t srcSlice, const uint8_t* dst74,
                                   uint32_t dstMip, uint32_t dstSlice, uint32_t* sbase, uint32_t* dbase, uint32_t* bytesPerBlock,
                                   void (*emit)(void*, uint32_t, uint32_t), void* ctx) {
    GX2Surface s, d;
    memcpy(&s, src74, sizeof s);
    memcpy(&d, dst74, sizeof d);
    if (surface_domain(s) != 0 || surface_domain(d) != 0 || srcMip > 13 || dstMip > 13)
        return 1;
    *sbase = copy_level_address(s, srcMip);
    *dbase = copy_level_address(d, dstMip);
    if (*sbase == 0 || *dbase == 0)
        return 1;
    uint32_t w = std::max<uint32_t>((uint32_t)s.width >> srcMip, 1), h = std::max<uint32_t>((uint32_t)s.height >> srcMip, 1);
    uint32_t compressed;
    uint32_t bpb = verify_copy_bpb((uint32_t)(Latte::E_GX2SURFFMT)s.format, &compressed);
    *bytesPerBlock = bpb;
    LatteAddrLib::AddrSurfaceInfo_OUT si{}, di{};
    LatteAddrLib::GX2CalculateSurfaceInfo(s.format, s.width, s.height, s.depth, s.dim, s.tileMode, s.aa, srcMip, &si);
    LatteAddrLib::GX2CalculateSurfaceInfo(d.format, d.width, d.height, d.depth, d.dim, d.tileMode, d.aa, dstMip, &di);
    auto stm = (Latte::E_HWTILEMODE)si.hwTileMode, dtm = (Latte::E_HWTILEMODE)di.hwTileMode;
    uint32_t bpp = bpb * 8;
    uint32_t bw = compressed ? (w + 3) / 4 : w, bh = compressed ? (h + 3) / 4 : h;
    uint32_t sswz = s.swizzle, dswz = d.swizzle;
    LatteAddrLib::CachedSurfaceAddrInfo sci, dci;
    if (Latte::TM_IsMacroTiled(stm))
        LatteAddrLib::SetupCachedSurfaceAddrInfo(&sci, srcSlice, 0, bpp, si.pitch, si.height, si.depth, 1, stm, false, (sswz >> 8) & 1, (sswz >> 9) & 3);
    if (Latte::TM_IsMacroTiled(dtm))
        LatteAddrLib::SetupCachedSurfaceAddrInfo(&dci, dstSlice, 0, bpp, di.pitch, di.height, di.depth, 1, dtm, false, (dswz >> 8) & 1, (dswz >> 9) & 3);
    for (uint32_t y = 0; y < bh; y++)
        for (uint32_t x = 0; x < bw; x++)
            emit(ctx, copy_element_offset(si, stm, x, y, srcSlice, bpp, &sci), copy_element_offset(di, dtm, x, y, dstSlice, bpp, &dci));
    return 0;
}
