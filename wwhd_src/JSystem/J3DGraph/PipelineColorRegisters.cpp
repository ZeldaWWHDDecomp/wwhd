// HD pipeline color-register copies retain their halfword and packed-word access order.
#include "JSystem/J3DGraph/PipelineColorRegisters.h"
using namespace j3d::pipeline;
namespace j3d::pipeline {
/* 027F75B0 */
void PipelineColorRegisters::setSignedColor(u32 index,const SignedColor* color) {
    SignedColor* destination=&mSignedColors[index];
    s16 red=color->r;
    destination->r=red;
    s16 green=color->g;
    destination->g=green;
    s16 blue=color->b;
    destination->b=blue;
    s16 alpha=color->a;
    destination->a=alpha;
}
/* 027F75DC */
SignedColor* PipelineColorRegisters::signedColor(u32 index) { return &mSignedColors[index]; }
/* 027F75EC */
void PipelineColorRegisters::setByteColor(u32 index,const ByteColor* color) {
    // GHS loads the complete four-byte value before writing any destination byte.
    const auto* source=reinterpret_cast<const ByteColorSlot_l*>(color);
    u32 bits=source->mBits;
    mByteColors[index].mBits=bits;
}
/* 027F7604 */
ByteColor* PipelineColorRegisters::byteColor(u32 index) { return &mByteColors[index].mColor; }
}
/* WWHD glue
*/
