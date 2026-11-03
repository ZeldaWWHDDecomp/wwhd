/* Quad drawing companions on the canonical layout draw context. */
#pragma once
#include "nw/lyt/LayoutRenderStorage.h"
namespace nw { namespace lyt {
struct QuadRectangleUniform { be<u32> width,height,x,y; };
struct QuadCoordinateUniform { be<u32> values[4][4]; };
struct LayoutQuadDrawing : LayoutDrawContext {
    void drawRectangle(const PaneSize* position,const PaneSize* size);
    void drawTexturedQuad(const PaneSize* position,const PaneSize* size,s32 count,const LayoutTextureCoordinates* coordinates);
    void drawNextRectangle(const PaneSize* position,const PaneSize* size);
};
WWHD_SIZE(QuadRectangleUniform,16); WWHD_SIZE(QuadCoordinateUniform,64);
WWHD_OFFSET(LayoutDrawContext,reserved,0xB0);
} }
void quadDrawingUniforms(u32 location,u32 count,const void* values);
