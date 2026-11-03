// HD: the GC draw pass has no shader uniform-buffer binding counterpart.
#include "JSystem/JParticle/JPADraw.h"
#include "sead/ElementAt.h"
#include "JSystem/JParticle/JPADraw_uniform.h"
#include "JSystem/JParticle/JPAEmitter.h"
using namespace JPADrawExtension;
using JPADrawViewStatePointer = gptr<JPADrawViewState_l>;
static JPADrawViewStatePointer sDrawViewState;
void JPADrawUploadUniform(JPADynamicUniform_l*,u32,const Mtx34*,const void*,const void*);
void JPADrawFlushUniform(JPADynamicUniform_l*);
void JPADrawUniformMatrixCopy(const Mtx34*,Mtx34*);
void JPADrawUniformSnapshot(Mtx34*,const Mtx34*);
void JPADrawPrepareTextureContext(JPATextureDrawContext_l*,jpa_emitter_extension::JPAShaderMaterial*);
void JPADrawBindPixelUniform(s32,void*,u32);
void JPADrawBindVertexUniform(s32,void*,u32);
void JPADrawBindGeometryUniform(s32,void*,u32);
/* 0282D094 */
void JPADraw_l::bindTextureUniforms() {
    auto emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    auto context = reinterpret_cast<JPATextureDrawContext_l*>(emitter->mGpuState.mTextureContext.get());
    auto program = gabi::at<jpa_emitter_extension::JPAShaderMaterial>(u32(emitter->mGpuState.mDrawContext));
    if (!context || !program) return;
    JPADrawPrepareTextureContext(context,program);
    auto uniform = context->mUniformBuffer.get();
    if (!uniform) return;
    auto slot = &uniform->mSlots[u32(uniform->mCurrent)];
    JPAUniformBinding_l* binding = nullptr;
    if (u32(program->mBindingCount) > 4) binding = &static_cast<JPAUniformBinding_l*>(program->mBindings.get())[4];
    s32 vertex = binding->mVertexLocation;
    u32 size = slot->mSize;
    void* data = slot->mData.get();
    s32 pixel = binding->mPixelLocation;
    s32 geometry = binding->mGeometryLocation;
    if (vertex == -1 && pixel == -1 && geometry == -1) return;
    if (pixel != -1) JPADrawBindPixelUniform(pixel,data,size);
    if (vertex != -1) JPADrawBindVertexUniform(vertex,data,size);
    if (geometry != -1) JPADrawBindGeometryUniform(geometry,data,size);
}
/* 0282D248 */
void JPADraw_l::updateUniforms(u32 parity) {
    if (u8(mUniformsUpdated[parity])) return;
    auto emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    auto dynamic = reinterpret_cast<JPADynamicUniform_l*>(emitter->mGpuState.mDynamicBuffers[parity]);
    if (u8(emitter->mGpuState.mRenderPass)) {
        auto clipboard = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
        JPADrawUniformMatrixCopy(static_cast<Mtx34*>(clipboard->mDrawMtxPtr.get()),&clipboard->mDrawMtx);
    }
    auto clipboard = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    Mtx34 matrix;
    JPADrawUniformSnapshot(&matrix,&clipboard->mDrawMtx);
    clipboard = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    JPADrawUploadUniform(dynamic,0,&matrix,clipboard->mProjectionMtx,sDrawViewState->mViewWords);
    JPADrawFlushUniform(dynamic);
    emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    auto uniform = dynamic->mBuffer.get();
    auto program = gabi::at<jpa_emitter_extension::JPAShaderMaterial>(u32(emitter->mGpuState.mDrawContext));
    auto slot = &sead::elementAt(uniform->mSlots, u32(uniform->mCurrent));
    JPAUniformBinding_l* binding = nullptr;
    if (u32(program->mBindingCount)) binding = static_cast<JPAUniformBinding_l*>(program->mBindings.get());
    s32 vertex = binding->mVertexLocation;
    u32 size = slot->mSize;
    s32 pixel = binding->mPixelLocation;
    void* data = slot->mData.get();
    s32 geometry = binding->mGeometryLocation;
    if (pixel != -1) JPADrawBindPixelUniform(pixel,data,size);
    if (vertex != -1) JPADrawBindVertexUniform(vertex,data,size);
    if (geometry != -1) JPADrawBindGeometryUniform(geometry,data,size);
    mUniformsUpdated[parity] = 1;
}
/* WWHD glue
data sDrawViewState 104B4708
ext 027FDA54 void JPADrawUploadUniform(JPADynamicUniform_l*,u32,const Mtx34*,const void*,const void*)
ext 027FE010 void JPADrawFlushUniform(JPADynamicUniform_l*)
ext 028E90D4 void JPADrawUniformMatrixCopy(const Mtx34*,Mtx34*)
ext 0282D1A8 void JPADrawUniformSnapshot(Mtx34*,const Mtx34*)
ext 027FFE54 void JPADrawPrepareTextureContext(JPATextureDrawContext_l*,jpa_emitter_extension::JPAShaderMaterial*)
ext C0006900 void JPADrawBindPixelUniform(s32,void*,u32)
ext C0006A38 void JPADrawBindVertexUniform(s32,void*,u32)
ext C00068A8 void JPADrawBindGeometryUniform(s32,void*,u32)
*/
