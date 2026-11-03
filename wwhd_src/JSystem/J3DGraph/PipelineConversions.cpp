// HD pipeline matrix and color conversions, reconstructed from their own instructions.
#include "JSystem/J3DGraph/PipelineConversions.h"
#include "bindings.h"
using namespace j3d::pipeline;
namespace j3d::pipeline {
/* 027F51B4 */
void Conversions::copyMatrix(MatrixRows* destination,const MatrixRows* source) {
    // All source words are captured before the first destination store, including overlaps.
    u32 words[3][4];
    for (u32 row=0;row!=3;++row)
        for (u32 column=0;column!=4;++column)
            words[row][column]=source->mRows[row][column].bits;
    for (u32 row=0;row!=3;++row)
        for (u32 column=0;column!=4;++column)
            destination->mRows[row][column].bits=words[row][column];
}
/* 027F5254 */
void Conversions::expandByteColor(FloatColor* destination,const ByteColor* source) {
    u8 red=source->r;
    u8 green=source->g;
    f32 redValue=gabi::fdivs_ppc(static_cast<f32>(red),255.0f);
    u8 blue=source->b;
    u8 alpha=source->a;
    f32 greenValue=gabi::fdivs_ppc(static_cast<f32>(green),255.0f);
    f32 blueValue=gabi::fdivs_ppc(static_cast<f32>(blue),255.0f);
    f32 alphaValue=gabi::fdivs_ppc(static_cast<f32>(alpha),255.0f);
    destination->r.value=redValue;
    destination->g.value=greenValue;
    destination->b.value=blueValue;
    destination->a.value=alphaValue;
}
/* 027F5308 */
void Conversions::expandSignedColor(FloatColor* destination,const SignedColor* source) {
    s16 red=source->r;
    s16 blue=source->b;
    s16 green=source->g;
    f32 redValue=gabi::fdivs_ppc(static_cast<f32>(red),255.0f);
    s16 alpha=source->a;
    f32 greenValue=gabi::fdivs_ppc(static_cast<f32>(green),255.0f);
    f32 blueValue=gabi::fdivs_ppc(static_cast<f32>(blue),255.0f);
    f32 alphaValue=gabi::fdivs_ppc(static_cast<f32>(alpha),255.0f);
    destination->r.value=redValue;
    destination->g.value=greenValue;
    destination->b.value=blueValue;
    destination->a.value=alphaValue;
}
}
/* WWHD glue
*/
