/* Quad uniform uploads and draw dispatch from WWHD instructions. */
#include "nw/lyt/LayoutQuadDrawing.h"
using namespace nw::lyt;
// These loads quiet signaling NaNs in the generated original before the uniform stores.
static u32 readQuadFloatBits(const be<f32>& value) {
    f32 converted=value;
    u32 bits;
    memcpy(&bits,&converted,sizeof(bits));
    return bits;
}
/* 02880E90 */
void LayoutQuadDrawing::drawRectangle(const PaneSize* position,const PaneSize* size) {
    gabi::AutoFrame frame{gabi::current_function()};
    flushVertexBinding();
    u32 location=uniformLocation(9);
    gabi::FrameLocal<QuadRectangleUniform> rectangle(8);
    u32 height=readQuadFloatBits(size->height);
    u32 width=readQuadFloatBits(size->width);
    rectangle->height=height;
    rectangle->width=width;
    u32 x=readQuadFloatBits(position->width);
    u32 y=readQuadFloatBits(position->height);
    rectangle->x=x;
    rectangle->y=y;
    quadDrawingUniforms(location,4,rectangle.get());
    uploadAffine();
    reinterpret_cast<LayoutRenderStorage*>(device.get())->drawQuad();
}
/* 02880F1C */
void LayoutQuadDrawing::drawTexturedQuad(const PaneSize* position,const PaneSize* size,s32 count,const LayoutTextureCoordinates* coordinates) {
    gabi::AutoFrame frame{gabi::current_function()};
    flushVertexBinding();
    gabi::FrameLocal<QuadCoordinateUniform> coordinateUniform(8);
    if (count && coordinates) {
        auto* slots=reinterpret_cast<const be<s8>*>(reserved);
        for (u32 channel=0;channel<3;++channel) {
            s32 index=slots[channel];
            if (index<0 || index>=count) continue;
            const LayoutTextureCoordinates* source=coordinates+index;
            for (u32 vertex=0;vertex<4;++vertex) {
                coordinateUniform->values[vertex][0]=readQuadFloatBits(source->values[vertex][0]);
                coordinateUniform->values[vertex][1]=readQuadFloatBits(source->values[vertex][1]);
                coordinateUniform->values[vertex][2]=0;
                coordinateUniform->values[vertex][3]=0x3F800000;
            }
            u32 location=uniformLocation(channel+14);
            quadDrawingUniforms(location,16,coordinateUniform.get());
        }
    }
    u32 location=uniformLocation(9);
    gabi::FrameLocal<QuadRectangleUniform> rectangle(0x48);
    u32 y=readQuadFloatBits(position->height);
    u32 height=readQuadFloatBits(size->height);
    rectangle->y=y;
    rectangle->height=height;
    u32 width=readQuadFloatBits(size->width);
    u32 x=readQuadFloatBits(position->width);
    rectangle->width=width;
    rectangle->x=x;
    quadDrawingUniforms(location,4,rectangle.get());
    uploadAffine();
    reinterpret_cast<LayoutRenderStorage*>(device.get())->drawQuad();
}
/* 028810B0 */
void LayoutQuadDrawing::drawNextRectangle(const PaneSize* position,const PaneSize* size) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 location=uniformLocation(9);
    gabi::FrameLocal<QuadRectangleUniform> rectangle(8);
    u32 height=readQuadFloatBits(size->height);
    u32 width=readQuadFloatBits(size->width);
    rectangle->height=height;
    rectangle->width=width;
    u32 x=readQuadFloatBits(position->width);
    u32 y=readQuadFloatBits(position->height);
    rectangle->x=x;
    rectangle->y=y;
    quadDrawingUniforms(location,4,rectangle.get());
    reinterpret_cast<LayoutRenderStorage*>(device.get())->drawQuad();
}
/* WWHD glue
ext 02874024 u32 nw::lyt::LayoutDrawContext::uniformLocation(u32)
ext 02874074 void nw::lyt::LayoutDrawContext::uploadAffine()
ext 028740C4 void nw::lyt::LayoutDrawContext::flushVertexBinding()
ext C0006A40 void quadDrawingUniforms(u32,u32,const void*)
*/
