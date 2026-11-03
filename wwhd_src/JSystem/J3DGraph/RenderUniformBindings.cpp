// J3D HD renderer: binding a uniform pair's uploaded buffer to a shader program's block locations.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderUniformBindings.h"

/* 027FF75C */
void j3d::RenderUniformBindEntries::bindFrame(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    bindPreviousUniformBlock(uniforms, locations, RENDER_UNIFORM_SLOT_FRAME);
}

/* 027FF88C */
void j3d::RenderUniformBindEntries::bindMatrix(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    bindPreviousUniformBlock(uniforms, locations, RENDER_UNIFORM_SLOT_MATRIX);
}

/* 027FF9C0 */
void j3d::RenderUniformBindEntries::bindMaterial(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    bindPreviousUniformBlock(uniforms, locations, RENDER_UNIFORM_SLOT_MATERIAL);
}

/* 027FFB48 */
void j3d::RenderUniformBindEntries::bindScene(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    bindPreviousUniformBlock(uniforms, locations, RENDER_UNIFORM_SLOT_SCENE);
}

/* WWHD glue
 ext C0006A38 void GX2SetVertexUniformBlock(u32,u32,const void*)
 ext C0006900 void GX2SetPixelUniformBlock(u32,u32,const void*)
 ext C00068A8 void GX2SetGeometryUniformBlock(u32,u32,const void*)
*/
