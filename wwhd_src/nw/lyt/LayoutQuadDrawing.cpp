/* Quad uniform uploads and draw dispatch from WWHD instructions. */
#include "nw/lyt/LayoutQuadDrawing.h"
#include "port_native_draw.h"
using namespace nw::lyt;
#if PORT_NATIVE_DRAW
// native draw packets (port_native_draw.h): each quad is an instance of the layout packet. Its rectangle, affine and
// texture-coordinate uploads (uniform registers of the selected program) are the instance's register values, the
// quad (LayoutRenderStorage::drawQuad) its mesh. The instance register ranges are the whole uniform register files
// (vertex and pixel): the pane's other per-quad values (material and vertex colours, texture matrices) stay the
// material's and pane's own calls, which the port then counts as instance state too; the program, textures and
// blend stay outside (a change lowers the packet again).
static bool nativeLayoutBegin(LayoutQuadDrawing* context) {
    auto* storage=reinterpret_cast<LayoutRenderStorage*>(context->device.get());
    LayoutShaderProgram* program=context->program;
    if (!storage || !program ||
        !wwhd_native_begin(PORT_NATIVE_LAYOUT,0,0,gabi::ea(program),gabi::ea(program),0)) return false;
    wwhd_native_declare_regs(PORT_NATIVE_LAYOUT,0,0,0x400);
    wwhd_native_declare_regs(PORT_NATIVE_LAYOUT,1,0,0x400);
    // the material's textures (units 0-2), blend and the quad buffer bind: its own calls between quads
    for (u32 unit=0;unit<3;++unit) wwhd_native_declare_check(PORT_NATIVE_LAYOUT,0,unit);
    wwhd_native_declare_check(PORT_NATIVE_LAYOUT,1,0);
    wwhd_native_declare_check(PORT_NATIVE_LAYOUT,2,0);
    wwhd_native_mesh(PORT_NATIVE_LAYOUT,0,0x13,4,4,gabi::ea(storage->quadIndices.get()));
    return wwhd_native_commit(PORT_NATIVE_LAYOUT)!=0;
}
#define QUAD_BEGIN() const bool native=nativeLayoutBegin(this)
#define QUAD_UNIFORMS(l,n,v) \
    if (!(native && wwhd_native_regs(PORT_NATIVE_LAYOUT,0,l,n,gabi::ea(v)))) quadDrawingUniforms(l,n,v)
#define QUAD_END() if (native) wwhd_native_end(PORT_NATIVE_LAYOUT)
#else
#define QUAD_BEGIN()
#define QUAD_UNIFORMS(l,n,v) quadDrawingUniforms(l,n,v)
#define QUAD_END()
#endif
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
    QUAD_BEGIN();
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
    QUAD_UNIFORMS(location,4,rectangle.get());
    uploadAffine();
    reinterpret_cast<LayoutRenderStorage*>(device.get())->drawQuad();
    QUAD_END();
}
/* 02880F1C */
void LayoutQuadDrawing::drawTexturedQuad(const PaneSize* position,const PaneSize* size,s32 count,const LayoutTextureCoordinates* coordinates) {
    gabi::AutoFrame frame{gabi::current_function()};
    flushVertexBinding();
    QUAD_BEGIN();
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
            QUAD_UNIFORMS(location,16,coordinateUniform.get());
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
    QUAD_UNIFORMS(location,4,rectangle.get());
    uploadAffine();
    reinterpret_cast<LayoutRenderStorage*>(device.get())->drawQuad();
    QUAD_END();
}
/* 028810B0 */
void LayoutQuadDrawing::drawNextRectangle(const PaneSize* position,const PaneSize* size) {
    gabi::AutoFrame frame{gabi::current_function()};
    QUAD_BEGIN();
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
    QUAD_UNIFORMS(location,4,rectangle.get());
    reinterpret_cast<LayoutRenderStorage*>(device.get())->drawQuad();
    QUAD_END();
}
#undef QUAD_BEGIN
#undef QUAD_UNIFORMS
#undef QUAD_END
/* WWHD glue
ext 02874024 u32 nw::lyt::LayoutDrawContext::uniformLocation(u32)
ext 02874074 void nw::lyt::LayoutDrawContext::uploadAffine()
ext 028740C4 void nw::lyt::LayoutDrawContext::flushVertexBinding()
ext C0006A40 void quadDrawingUniforms(u32,u32,const void*)
*/
