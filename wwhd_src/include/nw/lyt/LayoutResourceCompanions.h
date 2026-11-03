/* Resource helpers adjacent to the merged pane classes; layouts checked against WWHD instructions. */
#pragma once
#include "nw/lyt/LayoutGpuResources.h"
#include "nw/lyt/ResourceHelpers.h"
namespace nw { namespace lyt {
struct TextureCoordinateArray : TextureCoordinateStorage {
    static TextureCoordinateArray* create(TextureCoordinateArray*);
    void clear();
    void reserve(u32 count);
    void assign(const LayoutTextureCoordinates* source,u32 count);
};
struct MaterialResourceHeader;
struct MaterialResourceSet;
struct MaterialGpu : LayoutMaterial {
    u32 programIndex() const { return *layoutAtOffset<be<u32>>(this,0x14); }
    void setProgramIndex(u32 index) { layoutAtOffset<be<u32>>(this,0x14)->set(index); }
    static MaterialGpu* createFromResource(MaterialGpu*,const MaterialResourceHeader*,const MaterialResourceSet*);
    static MaterialGpu* copyFrom(MaterialGpu*,const MaterialGpu*);
    void clearCounts(be<u32>* flags);
    MaterialGpu* initialize();
    u32 reserve(u32 textures,u32 transforms,u32 coordinates,u32 stages,u32 alpha,u32 blends,u32 indirect,u8 indirectTransforms,u8 projection);
    void destroy(u32 flags);
    void bindTextures(LayoutDrawContext* context);
    void applyRenderState();
    void uploadPixelUniforms(LayoutDrawContext* context);
    void uploadTransforms(LayoutDrawContext* context,const PaneSize* size);
    void apply(LayoutDrawContext* context,u32 alpha,u32 variant,u32 reset,const PaneSize* size);
};
RuntimeTypeNode* windowRuntimeType();
RuntimeTypeNode* materialRuntimeType();
ShaderLocation* createPixelUniformLocation(ShaderLocation* location);
WWHD_SIZE(TextureCoordinateArray,8);
WWHD_SIZE(MaterialGpu,0x3C);
WWHD_OFFSET(LayoutMaterial,resource,0x14);
} }
