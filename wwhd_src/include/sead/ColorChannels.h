#pragma once
#include "sead/ViewTransform.h"
namespace sead {
struct ColorChannels {
    ViewScalar r, g, b, a;
    void clamp();
    void add(const ColorChannels*);
    void subtract(const ColorChannels*);
    void multiply(const ColorChannels*);
    void scale(f32);
    void divide(f32);
    bool equals(const ColorChannels*) const;
};
WWHD_SIZE(ColorChannels,16);
WWHD_OFFSET(ColorChannels,r,0);
WWHD_OFFSET(ColorChannels,g,4);
WWHD_OFFSET(ColorChannels,b,8);
WWHD_OFFSET(ColorChannels,a,12);
void initializeColorPresets();
void addColors(ColorChannels*,const ColorChannels*,const ColorChannels*);
void subtractColors(ColorChannels*,const ColorChannels*,const ColorChannels*);
void multiplyColors(ColorChannels*,const ColorChannels*,const ColorChannels*);
void scaleColor(ColorChannels*,const ColorChannels*,f32);
}
