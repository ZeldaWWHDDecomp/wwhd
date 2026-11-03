// GX2CalcSurfaceSizeAndAlignment model for the harness directive `mocksurfacesize` (README): the shipping port's
// own formula, runtime/third_party/cemu/gx2_surface_calc.cpp (Cemu-derived, MPL-2.0), applied to the guest's
// 0x74-byte big-endian GX2Surface. Built by verify.py with the port's Cemu flags (cemu_shim.h), separately from
// harness.cpp, which only sees the extern "C" entry point.
#include "gx2_surface_calc.h"
#include <cstdint>
#include <cstring>

// Returns 0 after rewriting the descriptor in place, or 1 when its fields are outside what the formula models
// (the harness then aborts the input instead of letting the address library run on garbage).
extern "C" int verify_calc_surface(uint8_t* bytes) {
    static_assert(sizeof(GX2Surface) == 0x74);
    GX2Surface s;
    memcpy(&s, bytes, sizeof s);
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
    gx2calc::CalcSurfaceSizeAndAlignment(&s);
    memcpy(bytes, &s, sizeof s);
    return 0;
}
