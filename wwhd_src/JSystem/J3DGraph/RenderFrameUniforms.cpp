// J3D HD renderer: frame and matrix uniform values, reset and upload into their uniform buffers.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderUniformPair.h"

static j3d::RenderWideWords sDefaultWideValue;

/* 027FB780 */
void j3d::RenderFrameUniforms::resetFrameValues() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    resetRenderMatrix34(&mPrimaryMatrix);
    resetRenderMatrix44(&mSecondaryMatrix);
    for (u32 index = 0; index < 8; ++index) {
        mVectors[index].mX = 0.0f;
        mVectors[index].mY = 0.0f;
        mVectors[index].mZ = 0.0f;
        for (u32 word = 0; word < 4; ++word)
            mWideValues[index].mWords[word] = sDefaultWideValue.mWords[word];
    }
    for (u32 index = 0; index < 2; ++index)
        for (u32 word = 0; word < 4; ++word)
            mExtraValues[index].mWords[word] = sDefaultWideValue.mWords[word];
    for (u32 word = 0; word < 4; ++word)
        mReferenceValue.mWords[word] = sDefaultWideValue.mWords[word];
    mSecondLimit = 1000000.0f;
    mFirstLimit = 1000000.0f;
    resetRenderMatrix44(&mFinalMatrix);
}

/* 027FB880 */
void j3d::RenderFrameUniforms::uploadFrameValues(agl::RenderUniformBuffer_l* buffer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    beginRenderUniformWrite(buffer);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 0, &mPrimaryMatrix, 0, 3);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 1, &mSecondaryMatrix, 0, 4);
    for (u32 index = 0; index < 8; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, 2, &mVectors[index], index, 1);
    for (u32 index = 0; index < 8; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, 3, &mWideValues[index], index, 1);
    for (u32 index = 0; index < 2; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, 4, &mExtraValues[index], index, 1);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 5, &mReferenceValue, 0, 1);
    gabi::FrameLocal<be<f32>> firstLimit(8);
    *firstLimit = mFirstLimit;
    writeRenderUniformField(buffer, buffer->mCpuStorage, 6, firstLimit.get(), 0, 1);
    gabi::FrameLocal<be<f32>> secondLimit(12);
    *secondLimit = mSecondLimit;
    writeRenderUniformField(buffer, buffer->mCpuStorage, 7, secondLimit.get(), 0, 1);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 8, &mFinalMatrix, 0, 4);
    endRenderUniformWrite(buffer, buffer->mCpuStorage, 0);
}

/* 027FBA94 */
void j3d::RenderMatrixUniforms::resetMatrixValues() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    resetRenderMatrix34(&mMatrix);
    mValue = 0;
}

/* 027FBACC */
void j3d::RenderMatrixUniforms::uploadMatrixValues(agl::RenderUniformBuffer_l* buffer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    beginRenderUniformWrite(buffer);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 0, &mMatrix, 0, 3);
    gabi::FrameLocal<be<u32>> value(8);
    *value = mValue;
    writeRenderUniformField(buffer, buffer->mCpuStorage, 1, value.get(), 0, 1);
    endRenderUniformWrite(buffer, buffer->mCpuStorage, 0);
}

/* WWHD glue
 data sDefaultWideValue 104A01CC
 ext 028E9098 void resetRenderMatrix34(Mtx34*)
 ext 028E892C void resetRenderMatrix44(Mtx44*)
 ext 027BEF54 void beginRenderUniformWrite(agl::RenderUniformBuffer_l*)
 ext 027BF048 void writeRenderUniformField(agl::RenderUniformBuffer_l*,void*,u32,const void*,u32,u32)
 ext 027BEF80 void endRenderUniformWrite(agl::RenderUniformBuffer_l*,void*,u32)
*/
