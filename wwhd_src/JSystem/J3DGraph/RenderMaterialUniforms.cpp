// J3D HD renderer: material uniform values (colours, texture matrices, transforms) and their upload.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderMaterialUniforms.h"
#include <atomic>

static j3d::RenderWideWords sDefaultMaterialValue;
static j3d::RenderWideWords sDefaultMaterialOffset;

/* 027FBB58 */
void j3d::RenderMaterialUniforms::setTransform(u32 index, const RenderUniformRow* linear, const RenderUniformRow* offset) {
    if (index >= 3) return;
    for (u32 component = 0; component < 4; ++component)
        mTransforms[index].mLinear.mValues[component] = linear->mValues[component];
    for (u32 component = 0; component < 4; ++component)
        mTransforms[index].mOffset.mValues[component] = offset->mValues[component];
}

/* 027FBC3C */
void j3d::RenderMaterialUniforms::configureMaterialFields(agl::RenderUniformBuffer_l* buffer, void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    defineRenderUniformFields(buffer, 28, heap);
    for (u32 field = 0; field < 11; ++field) appendRenderUniformField(buffer, 5, 1);
    for (u32 field = 0; field < 3; ++field) appendRenderUniformField(buffer, 5, 2);
    for (u32 field = 0; field < 8; ++field) appendRenderUniformField(buffer, 5, 3);
    for (u32 field = 0; field < 6; ++field) appendRenderUniformField(buffer, 5, 1);
    allocateRenderUniform(buffer, heap, 1);
}

/* 027FBE4C */
void j3d::RenderMaterialUniforms::resetMaterialValues() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < 11; ++index) {
        const RenderWideWords* initial = (index == 2 || index == 3)
            ? &sDefaultMaterialOffset : &sDefaultMaterialValue;
        for (u32 word = 0; word < 4; ++word)
            mMaterialValues[index].mWords[word] = initial->mWords[word];
    }
    for (u32 index = 0; index < 3; ++index)
        for (u32 row = 0; row < 2; ++row)
            for (u32 column = 0; column < 4; ++column)
                mTextureRows[index][row].mValues[column] = row == column ? 1.0f : 0.0f;
    for (u32 index = 0; index < 8; ++index) resetRenderMatrix34(&mTextureMatrices[index]);
    for (u32 index = 0; index < 3; ++index) {
        for (u32 component = 0; component < 4; ++component)
            mTransforms[index].mLinear.mValues[component] = (component == 0 || component == 3) ? 1.0f : 0.0f;
        for (u32 component = 0; component < 4; ++component)
            mTransforms[index].mOffset.mValues[component] = 0.0f;
    }
    // isync at the end of the reset (an acquire fence).
    std::atomic_thread_fence(std::memory_order_acquire);
}

/* 027FC118 */
void j3d::RenderMaterialUniforms::uploadMaterialValues(agl::RenderUniformBuffer_l* buffer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    beginRenderUniformWrite(buffer);
    for (u32 index = 0; index < 11; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, index, &mMaterialValues[index], 0, 1);
    // Fields11..13 have their own update path; this entry does not upload them.
    for (u32 index = 0; index < 8; ++index)
        writeRenderUniformField(buffer, buffer->mCpuStorage, index + 14, &mTextureMatrices[index], 0, 3);
    for (u32 index = 0; index < 3; ++index) {
        writeRenderUniformField(buffer, buffer->mCpuStorage, index * 2 + 22, &mTransforms[index].mLinear, 0, 1);
        writeRenderUniformField(buffer, buffer->mCpuStorage, index * 2 + 23, &mTransforms[index].mOffset, 0, 1);
    }
    endRenderUniformWrite(buffer, buffer->mCpuStorage, 0);
}

/* 027FC420 */
void j3d::RenderMutableMaterialUniforms::setPairValue(const RenderWideWords* value, u32 index) {
    if (index >= 2) return;
    for (u32 word = 0; word < 4; ++word)
        mMaterialValues[index + 2].mWords[word] = value->mWords[word];
    mDirtyMask = mDirtyMask | ((u64)1 << (index + 2));
}

/* 027FC500 */
void j3d::RenderMutableMaterialUniforms::setTripletValue(const RenderWideWords* value, u32 index) {
    if (index >= 3) return;
    for (u32 word = 0; word < 4; ++word)
        mMaterialValues[index + 4].mWords[word] = value->mWords[word];
    mDirtyMask = mDirtyMask | ((u64)1 << (index + 4));
}

/* 027FC650 */
void j3d::RenderMutableMaterialUniforms::setQuartetValue(const RenderWideWords* value, u32 index) {
    if (index >= 4) return;
    for (u32 word = 0; word < 4; ++word)
        mMaterialValues[index + 7].mWords[word] = value->mWords[word];
    mDirtyMask = mDirtyMask | ((u64)1 << (index + 7));
}

/* 027FC80C */
void j3d::RenderMutableMaterialUniforms::setTextureMatrix(const Mtx34* matrix, u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    // Slot3 is explicitly a no-op in this translation unit's dispatch table.
    if (index >= 8 || index == 3) return;
    copyRenderMatrix34(matrix, &mTextureMatrices[index]);
    mDirtyMask = mDirtyMask | ((u64)1 << (index + 14));
}

/* 027FCAB0 */
void j3d::RenderMutableMaterialUniforms::setTransformDirty(u32 index, const RenderUniformRow* linear, const RenderUniformRow* offset) {
    if (index >= 3) return;
    for (u32 component = 0; component < 4; ++component)
        mTransforms[index].mLinear.mValues[component] = linear->mValues[component];
    for (u32 component = 0; component < 4; ++component)
        mTransforms[index].mOffset.mValues[component] = offset->mValues[component];
    mDirtyMask = mDirtyMask | ((u64)1 << (index + 11));
}

/* 027FCC60 */
void j3d::RenderMutableMaterialUniforms::uploadDirtyMaterialValues(agl::RenderUniformBuffer_l* buffer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    beginRenderUniformWrite(buffer);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 0, &mMaterialValues[0], 0, 1);
    writeRenderUniformField(buffer, buffer->mCpuStorage, 1, &mMaterialValues[1], 0, 1);
    u32 dirty = (u32)mDirtyMask;
    for (u32 field = 2; field < 11; ++field) {
        if (dirty & (1u << field)) {
            writeRenderUniformField(buffer, buffer->mCpuStorage, field, &mMaterialValues[field], 0, 1);
            dirty = (u32)mDirtyMask;
        }
    }
    for (u32 index = 0; index < 8; ++index) {
        u32 field = index + 14;
        if (dirty & (1u << field)) {
            writeRenderUniformField(buffer, buffer->mCpuStorage, field, &mTextureMatrices[index], 0, 3);
            dirty = (u32)mDirtyMask;
        }
    }
    if (dirty & (1u << 11)) {
        writeRenderUniformField(buffer, buffer->mCpuStorage, 22, &mTransforms[0].mLinear, 0, 1);
        writeRenderUniformField(buffer, buffer->mCpuStorage, 23, &mTransforms[0].mOffset, 0, 1);
        dirty = (u32)mDirtyMask;
    }
    if (dirty & (1u << 12)) {
        writeRenderUniformField(buffer, buffer->mCpuStorage, 24, &mTransforms[1].mLinear, 0, 1);
        writeRenderUniformField(buffer, buffer->mCpuStorage, 25, &mTransforms[1].mOffset, 0, 1);
        dirty = (u32)mDirtyMask;
        // Original quirk: the third pair checks bit12 again, not bit13.
        if (dirty & (1u << 12)) {
            writeRenderUniformField(buffer, buffer->mCpuStorage, 26, &mTransforms[2].mLinear, 0, 1);
            writeRenderUniformField(buffer, buffer->mCpuStorage, 27, &mTransforms[2].mOffset, 0, 1);
        }
    }
    endRenderUniformWrite(buffer, buffer->mCpuStorage, 0);
}

/* WWHD glue
 ext 028E90D4 void copyRenderMatrix34(const Mtx34*,Mtx34*)
 data sDefaultMaterialValue 104A01EC
 data sDefaultMaterialOffset 104A01CC
*/
