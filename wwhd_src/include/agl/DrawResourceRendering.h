// Own resource draw caller prefixes.
#pragma once
#include "agl/DrawPairRendering.h"
namespace agl {
struct DrawLargeRendering_l {
    u8 mPrefix[4];
    gptr<DrawResourceConfiguration_l> mConfiguration;
    u8 mBeforeBinding[0x14];
    VertexInputSet mInputs;
    gptr<IndexBufferLayout> mIndexBuffer;
    void drawLargeResource(LargeDrawResourceRecord_l* record, u32 slot, u32 unused,
                           DrawPairRenderContext_l* context);
};
struct DrawResourceRendering_l {
    u8 mPrefix[4];
    gptr<DrawResourceConfiguration_l> mConfiguration;
    u8 mBeforeBinding[0x18];
    VertexInputSet mInputs;
    gptr<IndexBufferLayout> mIndexBuffer;
    void drawResource(DrawResourceRecord_l* record, u32 slot, u32 unused,
                      DrawPairRenderContext_l* context);
};
}
WWHD_OFFSET(agl::DrawResourceRendering_l, mInputs, 0x20);
WWHD_OFFSET(agl::DrawResourceRendering_l, mIndexBuffer, 0x114);

WWHD_OFFSET(agl::DrawLargeRendering_l, mInputs, 0x1C);
WWHD_OFFSET(agl::DrawLargeRendering_l, mIndexBuffer, 0x110);
