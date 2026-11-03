// Component color operations reconstructed from WWHD; scalar ordering preserves aliasing.
#include "sead/ColorChannels.h"
namespace sead {}
using namespace sead;
static ColorChannels sBlack;
static u32 sPackedBlack;
static ColorChannels sGray;
static u32 sPackedGray;
static ColorChannels sWhite;
static u32 sPackedWhite;
static ColorChannels sRed;
static u32 sPackedRed;
static ColorChannels sGreen;
static u32 sPackedGreen;
static ColorChannels sBlue;
static u32 sPackedBlue;
static ColorChannels sYellow;
static u32 sPackedYellow;
static ColorChannels sMagenta;
static u32 sPackedMagenta;
static ColorChannels sCyan;
static u32 sPackedCyan;

/* 0274D1E0 */
void sead::ColorChannels::add(const ColorChannels* other) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 red = gabi::fadds_ppc(r.value, other->r.value);
    f32 green = g.value;
    r.value = red;
    green = gabi::fadds_ppc(green, other->g.value);
    f32 blue = b.value;
    g.value = green;
    blue = gabi::fadds_ppc(blue, other->b.value);
    f32 alpha = a.value;
    b.value = blue;
    alpha = gabi::fadds_ppc(alpha, other->a.value);
    a.value = alpha;
}
/* 0274D224 */
void sead::ColorChannels::subtract(const ColorChannels* other) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 red = gabi::fsubs_ppc(r.value, other->r.value);
    f32 green = g.value;
    r.value = red;
    green = gabi::fsubs_ppc(green, other->g.value);
    f32 blue = b.value;
    g.value = green;
    blue = gabi::fsubs_ppc(blue, other->b.value);
    f32 alpha = a.value;
    b.value = blue;
    alpha = gabi::fsubs_ppc(alpha, other->a.value);
    a.value = alpha;
}
/* 0274D268 */
void sead::ColorChannels::multiply(const ColorChannels* other) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 red = gabi::fmuls_ppc(r.value, other->r.value);
    f32 green = g.value;
    r.value = red;
    green = gabi::fmuls_ppc(green, other->g.value);
    f32 blue = b.value;
    g.value = green;
    blue = gabi::fmuls_ppc(blue, other->b.value);
    f32 alpha = a.value;
    b.value = blue;
    alpha = gabi::fmuls_ppc(alpha, other->a.value);
    a.value = alpha;
}
/* 0274D2AC */
void sead::ColorChannels::scale(f32 factor) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 red = r.value;
    f32 green = g.value;
    red = gabi::fmuls_ppc(red, factor);
    f32 blue = b.value;
    green = gabi::fmuls_ppc(green, factor);
    r.value = red;
    f32 alpha = a.value;
    blue = gabi::fmuls_ppc(blue, factor);
    g.value = green;
    alpha = gabi::fmuls_ppc(alpha, factor);
    b.value = blue;
    a.value = alpha;
}
/* 0274D2E0 */
void sead::ColorChannels::divide(f32 divisor) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 red = r.value;
    f32 green = g.value;
    red = gabi::fdivs_ppc(red, divisor);
    green = gabi::fdivs_ppc(green, divisor);
    f32 blue = b.value;
    blue = gabi::fdivs_ppc(blue, divisor);
    g.value = green;
    f32 alpha = a.value;
    alpha = gabi::fdivs_ppc(alpha, divisor);
    b.value = blue;
    r.value = red;
    a.value = alpha;
}
/* 0274D4C0 */
bool sead::ColorChannels::equals(const ColorChannels* other) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    return r.value == other->r.value && g.value == other->g.value &&
           b.value == other->b.value && a.value == other->a.value;
}
/* 0274D314 */
void sead::addColors(ColorChannels* result, const ColorChannels* left, const ColorChannels* right) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<ColorChannels> value(8);
    u32 green = left->g.bits;
    u32 alpha = left->a.bits;
    u32 blue = left->b.bits;
    u32 red = left->r.bits;
    value->g.bits = green;
    value->a.bits = alpha;
    value->b.bits = blue;
    value->r.bits = red;
    value->add(right);
    red = value->r.bits;
    green = value->g.bits;
    result->r.bits = red;
    alpha = value->a.bits;
    blue = value->b.bits;
    result->g.bits = green;
    result->b.bits = blue;
    result->a.bits = alpha;
}
/* 0274D380 */
void sead::subtractColors(ColorChannels* result, const ColorChannels* left, const ColorChannels* right) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<ColorChannels> value(8);
    u32 green = left->g.bits;
    u32 alpha = left->a.bits;
    u32 blue = left->b.bits;
    u32 red = left->r.bits;
    value->g.bits = green;
    value->a.bits = alpha;
    value->b.bits = blue;
    value->r.bits = red;
    value->subtract(right);
    red = value->r.bits;
    green = value->g.bits;
    result->r.bits = red;
    alpha = value->a.bits;
    blue = value->b.bits;
    result->g.bits = green;
    result->b.bits = blue;
    result->a.bits = alpha;
}
/* 0274D3EC */
void sead::multiplyColors(ColorChannels* result, const ColorChannels* left, const ColorChannels* right) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<ColorChannels> value(8);
    u32 green = left->g.bits;
    u32 alpha = left->a.bits;
    u32 blue = left->b.bits;
    u32 red = left->r.bits;
    value->g.bits = green;
    value->a.bits = alpha;
    value->b.bits = blue;
    value->r.bits = red;
    value->multiply(right);
    red = value->r.bits;
    green = value->g.bits;
    result->r.bits = red;
    alpha = value->a.bits;
    blue = value->b.bits;
    result->g.bits = green;
    result->b.bits = blue;
    result->a.bits = alpha;
}
/* 0274D458 */
void sead::scaleColor(ColorChannels* result, const ColorChannels* input, f32 factor) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<ColorChannels> value(8);
    u32 green = input->g.bits;
    u32 red = input->r.bits;
    value->g.bits = green;
    value->r.bits = red;
    u32 alpha = input->a.bits;
    u32 blue = input->b.bits;
    value->a.bits = alpha;
    value->b.bits = blue;
    value->scale(factor);
    blue = value->b.bits;
    alpha = value->a.bits;
    red = value->r.bits;
    green = value->g.bits;
    result->r.bits = red;
    result->g.bits = green;
    result->a.bits = alpha;
    result->b.bits = blue;
}
/* 0274D11C */
void sead::ColorChannels::clamp() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 red=r.value;
    f32 green=g.value;
    bool redLow=red<0.0f;
    f32 blue;
    bool greenLow;
    if(redLow) {
        greenLow=green<0.0f;
        blue=b.value;
        r.value=0.0f;
    } else {
        if(red>1.0f) red=1.0f;
        greenLow=green<0.0f;
        r.value=red;
        blue=b.value;
    }
    f32 alpha;
    bool blueLow;
    if(greenLow) {
        blueLow=blue<0.0f;
        alpha=a.value;
        g.value=0.0f;
    } else {
        if(green>1.0f) green=1.0f;
        blueLow=blue<0.0f;
        g.value=green;
        alpha=a.value;
    }
    bool alphaLow;
    if(blueLow) {
        alphaLow=alpha<0.0f;
        b.value=0.0f;
    } else {
        if(blue>1.0f) blue=1.0f;
        alphaLow=alpha<0.0f;
        b.value=blue;
    }
    if(alphaLow) a.value=0.0f;
    else {
        if(alpha>1.0f) alpha=1.0f;
        a.value=alpha;
    }
}

/* 0274D510 */
void sead::initializeColorPresets() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    sBlack.r.value=0.0f; sBlack.g.value=0.0f; sBlack.b.value=0.0f; sBlack.a.value=1.0f;
    sGray.r.value=0.5f; sGray.g.value=0.5f; sGray.b.value=0.5f; sGray.a.value=1.0f;
    sWhite.r.value=1.0f; sWhite.g.value=1.0f; sWhite.b.value=1.0f; sWhite.a.value=1.0f;
    sRed.r.value=1.0f; sRed.g.value=0.0f; sRed.b.value=0.0f; sRed.a.value=1.0f;
    sGreen.r.value=0.0f; sGreen.g.value=1.0f; sGreen.b.value=0.0f; sGreen.a.value=1.0f;
    sBlue.r.value=0.0f; sBlue.g.value=0.0f; sBlue.b.value=1.0f; sBlue.a.value=1.0f;
    sYellow.r.value=1.0f; sYellow.g.value=1.0f; sYellow.b.value=0.0f; sYellow.a.value=1.0f;
    sMagenta.r.value=1.0f; sMagenta.g.value=0.0f; sMagenta.b.value=1.0f; sMagenta.a.value=1.0f;
    sCyan.r.value=0.0f; sCyan.g.value=1.0f; sCyan.b.value=1.0f; sCyan.a.value=1.0f;
    sPackedBlack=0x000000FF;
    sPackedGray=0x808080FF;
    sPackedWhite=0xFFFFFFFF;
    sPackedRed=0xFF0000FF;
    sPackedGreen=0x00FF00FF;
    sPackedBlue=0x0000FFFF;
    sPackedYellow=0xFFFF00FF;
    sPackedMagenta=0xFF00FFFF;
    sPackedCyan=0x00FFFFFF;
}

/* WWHD glue
data sBlack 104A01CC
data sPackedBlack 104A025C
data sGray 104A01DC
data sPackedGray 104A0260
data sWhite 104A01EC
data sPackedWhite 104A0264
data sRed 104A01FC
data sPackedRed 104A0268
data sGreen 104A020C
data sPackedGreen 104A026C
data sBlue 104A021C
data sPackedBlue 104A0270
data sYellow 104A022C
data sPackedYellow 104A0274
data sMagenta 104A023C
data sPackedMagenta 104A0278
data sCyan 104A024C
data sPackedCyan 104A027C

*/
