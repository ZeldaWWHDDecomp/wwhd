/* Matrix uploads and shader-state dispatch reconstructed from WWHD call and field order. */
#include "nw/lyt/LayoutDrawContext.h"
#include "port_native_draw.h"
using namespace nw::lyt;


struct DrawIdentityGuard { be<u32> ready; };
static DrawIdentityGuard sDrawProjectionGuard;
static DrawIdentityGuard sDrawAffineGuard;
static Mtx44 sDrawIdentityProjection;
static Mtx34 sDrawIdentityAffine;
static u8 sDrawContextMethods;

static void initializeDrawAffineIdentity() {
    sDrawIdentityAffine.m[0][1] = 0.0f;
    sDrawIdentityAffine.m[1][1] = 1.0f;
    sDrawAffineGuard.ready = 1;
    sDrawIdentityAffine.m[0][3] = 0.0f;
    sDrawIdentityAffine.m[0][0] = 1.0f;
    sDrawIdentityAffine.m[1][0] = 0.0f;
    sDrawIdentityAffine.m[1][2] = 0.0f;
    sDrawIdentityAffine.m[2][1] = 0.0f;
    sDrawIdentityAffine.m[2][2] = 1.0f;
    sDrawIdentityAffine.m[1][3] = 0.0f;
    sDrawIdentityAffine.m[2][3] = 0.0f;
    sDrawIdentityAffine.m[2][0] = 0.0f;
    sDrawIdentityAffine.m[0][2] = 0.0f;
}

static void copyDrawMatrixWords(void* output, const void* input, u32 count) {
    if (gabi::ea(output) == gabi::ea(input)) return;
    auto* source = static_cast<const be<u32>*>(input);
    auto* destination = static_cast<be<u32>*>(output);
    for (u32 index = 0; index < count; ++index) {
        u32 word = source[index];
        destination[index] = word;
    }
}

/* 02873D70 */
LayoutDrawContext* LayoutDrawContext::create(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!context) context = static_cast<LayoutDrawContext*>(drawContextAllocate(sizeof(LayoutDrawContext)));
    if (!context) return context;
    context->stateWordB8 = 0;
    context->flagB3 = 0;
    context->methods = gabi::ea(&sDrawContextMethods);
    context->affineUploaded = 0;
    context->flagB4 = 0;
    context->scaleY = 1.0f;
    context->stateWord = 0;
    context->scaleX = 1.0f;
    context->flagB5 = 0;
    context->program = nullptr;
    context->vertexBindingPending = 0;
    context->device = nullptr;
    drawContextClear(&context->initializedByte, 0, 1);
    if (u32(sDrawProjectionGuard.ready) == 0) {
        sDrawIdentityProjection.m[0][1] = 0.0f;
        sDrawIdentityProjection.m[1][3] = 0.0f;
        sDrawProjectionGuard.ready = 1;
        sDrawIdentityProjection.m[1][0] = 0.0f;
        sDrawIdentityProjection.m[0][0] = 1.0f;
        sDrawIdentityProjection.m[1][1] = 1.0f;
        sDrawIdentityProjection.m[2][0] = 0.0f;
        sDrawIdentityProjection.m[3][0] = 0.0f;
        sDrawIdentityProjection.m[3][2] = 0.0f;
        sDrawIdentityProjection.m[2][2] = 1.0f;
        sDrawIdentityProjection.m[3][3] = 1.0f;
        sDrawIdentityProjection.m[0][3] = 0.0f;
        sDrawIdentityProjection.m[0][2] = 0.0f;
        sDrawIdentityProjection.m[1][2] = 0.0f;
        sDrawIdentityProjection.m[2][1] = 0.0f;
        sDrawIdentityProjection.m[2][3] = 0.0f;
        sDrawIdentityProjection.m[3][1] = 0.0f;
    }
    copyDrawMatrixWords(&context->projection, &sDrawIdentityProjection, 16);
    if (u32(sDrawAffineGuard.ready) == 0) initializeDrawAffineIdentity();
    if (gabi::ea(&context->affineA) != gabi::ea(&sDrawIdentityAffine)) {
        copyDrawMatrixWords(&context->affineA, &sDrawIdentityAffine, 12);
        if (u32(sDrawAffineGuard.ready) == 0) initializeDrawAffineIdentity();
    }
    copyDrawMatrixWords(&context->affineB, &sDrawIdentityAffine, 12);
    return context;
}

/* 02873FD0 */
void LayoutDrawContext::destroy(u32 flags) {
    if (this && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)drawContextDelete), this);
}

/* 02873FE4 */
void LayoutDrawContext::setProjection(const Mtx44* matrix) {
    if (gabi::ea(this) == gabi::ea(matrix)) return;
    auto* source = reinterpret_cast<const be<u32>*>(matrix);
    auto* destination = reinterpret_cast<be<u32>*>(&projection);
    for (u32 index = 0; index < 16; ++index) {
        u32 word = source[index];
        destination[index] = word;
    }
}

/* 02874024 */
u32 LayoutDrawContext::uniformLocation(u32 index) {
    LayoutRenderDevice* renderer = device;
    LayoutShaderProgram* selected = program;
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)drawContextUniformLocation),
        &renderer->shaderSlots, selected, index);
}

/* 02874038 */
void LayoutDrawContext::uploadProjection() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 location = uniformLocation(0);
    drawContextVertexUniforms(location, 16, &projection);
}

/* 02874074 */
void LayoutDrawContext::uploadAffine() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (u8(affineUploaded) != 0) return;
    affineUploaded = 1;
    u32 location = uniformLocation(1);
#if PORT_NATIVE_DRAW
    // inside a layout quad's packet (LayoutQuadDrawing.cpp): the affine is the instance's register values
    if (wwhd_native_regs(PORT_NATIVE_LAYOUT, 0, location, 12, gabi::ea(&affineB))) return;
#endif
    drawContextVertexUniforms(location, 12, &affineB);
}

/* 028740C4 */
void LayoutDrawContext::flushVertexBinding() {
    if (u8(vertexBindingPending) == 0) return;
    LayoutRenderDevice* renderer = device;
    vertexBindingPending = 0;
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)drawContextBindVertexBuffer), renderer);
}

/* 028740E4 */
void LayoutDrawContext::selectProgram(u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    LayoutRenderDevice* renderer = device;
    LayoutShaderProgram* selected = drawContextProgramAt(&renderer->shaderSlots, index);
    renderer = device;
    program = selected;
    drawContextBindProgram(&renderer->shaderSlots, selected);
    affineUploaded = 0;
}

/* WWHD glue
data sDrawProjectionGuard 101FD308 nocheck
data sDrawAffineGuard 101FD304 nocheck
data sDrawIdentityProjection 101FE084 nocheck
data sDrawIdentityAffine 101FE0C4 nocheck
data sDrawContextMethods 101802EC nocheck
ext 0273AD10 void* drawContextAllocate(u32)
ext C000A858 void drawContextClear(void*, u32, u32)
ext 0273AF40 void drawContextDelete(void*) callback
ext 0287D850 u32 drawContextUniformLocation(nw::lyt::LayoutShaderSlots*, nw::lyt::LayoutShaderProgram*, u32) callback
ext 0287D808 nw::lyt::LayoutShaderProgram* drawContextProgramAt(nw::lyt::LayoutShaderSlots*, u32)
ext 0287D860 void drawContextBindProgram(nw::lyt::LayoutShaderSlots*, nw::lyt::LayoutShaderProgram*)
ext 0287ED48 void drawContextBindVertexBuffer(nw::lyt::LayoutRenderDevice*) callback
ext C0006A40 void drawContextVertexUniforms(u32, u32, const void*)
*/
