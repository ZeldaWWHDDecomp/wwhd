/* Drawing matrices and shader state, from constructor offsets and uniform-register imports. */
#pragma once
#include "wwhd.h"
namespace nw { namespace lyt {
struct LayoutShaderProgram { u8 prefix[0x30]; gptr<be<u32>> uniformLocations; };
struct LayoutShaderSlots { u8 prefix[0x38]; gptr<LayoutShaderProgram> programs[1]; };
struct LayoutRenderDevice { u8 prefix[0x25C]; LayoutShaderSlots shaderSlots; };
struct LayoutDrawContext {
    Mtx44 projection;
    Mtx34 affineA, affineB;
    be<f32> scaleX, scaleY;
    gptr<LayoutRenderDevice> device;
    be<u32> stateWord;
    u8 reserved[3];
    be<u8> flagB3, flagB4, flagB5, affineUploaded, vertexBindingPending;
    be<u32> stateWordB8;
    gptr<LayoutShaderProgram> program;
    be<u8> initializedByte;
    u8 padding[3];
    be<u32> methods;
    static LayoutDrawContext* create(LayoutDrawContext* context);
    void destroy(u32 flags);
    void setProjection(const Mtx44* matrix);
    u32 uniformLocation(u32 index);
    void uploadProjection();
    void uploadAffine();
    void flushVertexBinding();
    void selectProgram(u32 index);
};
WWHD_SIZE(LayoutDrawContext, 0xC8);
WWHD_OFFSET(LayoutDrawContext, affineA, 0x40);
WWHD_OFFSET(LayoutDrawContext, affineB, 0x70);
WWHD_OFFSET(LayoutDrawContext, device, 0xA8);
WWHD_OFFSET(LayoutDrawContext, affineUploaded, 0xB6);
WWHD_OFFSET(LayoutDrawContext, program, 0xBC);
WWHD_OFFSET(LayoutDrawContext, methods, 0xC4);
WWHD_OFFSET(LayoutRenderDevice, shaderSlots, 0x25C);
} }
void drawContextDelete(void* storage);
u32 drawContextUniformLocation(nw::lyt::LayoutShaderSlots*, nw::lyt::LayoutShaderProgram*, u32);
nw::lyt::LayoutShaderProgram* drawContextProgramAt(nw::lyt::LayoutShaderSlots*, u32);
void drawContextBindProgram(nw::lyt::LayoutShaderSlots*, nw::lyt::LayoutShaderProgram*);
void drawContextBindVertexBuffer(nw::lyt::LayoutRenderDevice*);
void drawContextVertexUniforms(u32 location, u32 count, const void* data);

void* drawContextAllocate(u32);
void drawContextClear(void*, u32, u32);
