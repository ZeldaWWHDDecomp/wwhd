/* Picture color and material access reconstructed from original WWHD instructions.
 * This is a partial view, not the complete allocated picture object. */
#include "nw/lyt/PictureColors.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;

/* 02874D14 */
void PictureColors::setColorByte(u32 index, u32 value) {
    auto* bytes = reinterpret_cast<be<u8>*>(colors);
    auto* destination = layoutAtOffset<be<u8>>(bytes, index);
    *destination = u8(value);
}

/* 02874D2C */
bool PictureColors::hasMaterial() const {
    return material != nullptr;
}

/* 02874D3C */
void* PictureColors::materialAt(s32 index) const {
    if (index != 0) return nullptr;
    return material;
}

/* WWHD glue
*/
