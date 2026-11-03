// J3D HD renderer: scene uniform values (view/projection matrices, scene parameters) and their upload.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderSceneUniforms.h"
#include <atomic>

/* 027FD0A8 */
void j3d::RenderSceneUniforms::configureSceneFields(agl::RenderUniformBuffer_l* buffer, void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    defineRenderUniformFields(buffer, 30, heap);
    for (u32 index = 0; index < 4; ++index) appendRenderUniformField(buffer, 5, 4);
    appendRenderUniformField(buffer, 5, 1);
    appendRenderUniformField(buffer, 5, 4);
    for (u32 index = 0; index < 4; ++index) appendRenderUniformField(buffer, 2, 1);
    for (u32 index = 0; index < 3; ++index) appendRenderUniformField(buffer, 5, 1);
    appendRenderUniformField(buffer, 2, 1);
    for (u32 index = 0; index < 6; ++index) appendRenderUniformField(buffer, 5, 1);
    for (u32 index = 0; index < 10; ++index) appendRenderUniformField(buffer, 5, 1);
    allocateRenderUniform(buffer, heap, 1);
}

/* 027FD25C */
void j3d::RenderSceneUniforms::resetSceneValues() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < 4; ++index) resetRenderMatrix44(&mMatrices[index]);
    for (u32 component = 0; component < 4; ++component) mViewParameters.mValues[component] = 0.0f;
    resetRenderMatrix44(&mProjectionMatrix);
    mScalarParameters[0] = 1.0f;
    mScalarParameters[1] = 1.0f;
    mScalarParameters[2] = 0.5f;
    mScalarParameters[3] = 0.45f;
    for (u32 index = 0; index < 2; ++index)
        for (u32 component = 0; component < 4; ++component)
            mEnvironmentVectors[index].mValues[component] = 0.0f;
    mEnvironmentVectors[2].mValues[0] = 1.0f;
    mEnvironmentVectors[2].mValues[1] = 1.0f;
    mEnvironmentVectors[2].mValues[2] = 20.0f;
    mEnvironmentVectors[2].mValues[3] = 0.0f;
    mRange = 1.0f;
    const f32 parameters[6][4] = {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {-0.1f, -0.8f, 0.6f, 0.15f},
        {-0.5f, -0.1f, 0.5f, 0.7f},
        {-0.4f, 0.2f, 1.0f, -0.3f},
        {0.0f, 0.0f, 1.0f, 1.0f}
    };
    for (u32 index = 0; index < 6; ++index)
        for (u32 component = 0; component < 4; ++component)
            mParameters[index].mValues[component] = parameters[index][component];
    for (u32 index = 0; index < 10; ++index)
        for (u32 component = 0; component < 4; ++component)
            mExtraValues[index].mValues[component] = 0.0f;
    std::atomic_thread_fence(std::memory_order_acquire);
}

/* 027FD408 */
void j3d::RenderSceneUniforms::uploadSceneValues(agl::RenderUniformBuffer_l* buffer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    beginRenderUniformWrite(buffer);
    for (u32 index = 0; index < 4; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, index, &mMatrices[index], 0, 4);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 4, &mViewParameters, 0, 1);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 5, &mProjectionMatrix, 0, 4);
    for (u32 index = 0; index < 4; ++index) {
        gabi::FrameLocal<be<f32>> value(8 + index * 4);
        *value = mScalarParameters[index];
        writeRenderUniformField(buffer, buffer->mCpuStorage, index + 6, value.get(), 0, 1);
    }
    for (u32 index = 0; index < 3; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, index + 10, &mEnvironmentVectors[index], 0, 1);
    gabi::FrameLocal<be<f32>> range(24);
    *range = mRange;
    writeRenderUniformField(buffer, buffer->mCpuStorage, 13, range.get(), 0, 1);
    for (u32 index = 0; index < 6; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, index + 14, &mParameters[index], 0, 1);
    for (u32 index = 0; index < 10; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, index + 20, &mExtraValues[index], 0, 1);
    endRenderUniformWrite(buffer, buffer->mCpuStorage, 0);
}

/* 027FFC28 */
void j3d::RenderSceneUniforms::destroy(RenderSceneUniforms* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL) {
        RenderUniformPair::destroy(storage, 0);
        if (flags & 1) releaseRenderUniformObject(storage);
    }
}

/* WWHD glue
*/
