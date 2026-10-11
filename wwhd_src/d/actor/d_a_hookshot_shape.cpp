// HD Hookshot renderer.
#include "d/actor/d_a_hookshot.h"
namespace {
struct CallLinkage {
    u8 bytes[32];
};
template <class R = void, class... A> R hookCall(u32 target, A... args) {
    gabi::Local<CallLinkage> linkage;
    return gabi::call<R>(target, args...);
}
namespace hookshotGpu {
template <class R = void, class... Args> R loadResource(Args... args) {
    return hookCall<R>(0x026124B0, args...);
}
template <class R = void, class... Args> R allocate(Args... args) {
    return hookCall<R>(0x0273AD10, args...);
}
template <class R = void, class... Args> R free(Args... args) {
    return hookCall<R>(0x0273AF40, args...);
}
template <class R = void, class... Args> R lockTextureCache(Args... args) {
    return hookCall<R>(0x0274FBF8, args...);
}
template <class R = void, class... Args> R unlockTextureCache(Args... args) {
    return hookCall<R>(0x0274FCCC, args...);
}
template <class R = void, class... Args> R initializeRenderState(Args... args) {
    return hookCall<R>(0x02750250, args...);
}
template <class R = void, class... Args> R applyRenderState(Args... args) {
    return hookCall<R>(0x02750370, args...);
}
template <class R = void, class... Args> R findAllocationHeap(Args... args) {
    return hookCall<R>(0x02755FEC, args...);
}
template <class R = void, class... Args> R getGpuHeap(Args... args) {
    return hookCall<R>(0x02756140, args...);
}
template <class R = void, class... Args> R loadResourceTexture(Args... args) {
    return hookCall<R>(0x02773870, args...);
}
template <class R = void, class... Args> R constructPrimitive(Args... args) {
    return hookCall<R>(0x027B5430, args...);
}
template <class R = void, class... Args> R destructPrimitive(Args... args) {
    return hookCall<R>(0x027B54A0, args...);
}
template <class R = void, class... Args> R setPrimitive(Args... args) {
    return hookCall<R>(0x027B54E0, args...);
}
template <class R = void, class... Args> R flushVertexBuffer(Args... args) {
    return hookCall<R>(0x027B5E94, args...);
}
template <class R = void, class... Args> R findMaterial(Args... args) {
    return hookCall<R>(0x027B90AC, args...);
}
template <class R = void, class... Args> R bindShaderProgram(Args... args) {
    return hookCall<R>(0x027B9F68, args...);
}
template <class R = void, class... Args> R applyShader(Args... args) {
    return hookCall<R>(0x027BB7CC, args...);
}
template <class R = void, class... Args> R updateShader(Args... args) {
    return hookCall<R>(0x027BB9E0, args...);
}
template <class R = void, class... Args> R copyTexture(Args... args) {
    return hookCall<R>(0x027BDEB4, args...);
}
template <class R = void, class... Args> R constructTexture(Args... args) {
    return hookCall<R>(0x027BDF7C, args...);
}
template <class R = void, class... Args> R destructTexture(Args... args) {
    return hookCall<R>(0x027BE2B0, args...);
}
template <class R = void, class... Args> R bindTexture(Args... args) {
    return hookCall<R>(0x027BE53C, args...);
}
template <class R = void, class... Args> R constructResourceTexture(Args... args) {
    return hookCall<R>(0x027BE6B8, args...);
}
template <class R = void, class... Args> R releaseUniformBuffer(Args... args) {
    return hookCall<R>(0x027BEBEC, args...);
}
template <class R = void, class... Args> R constructUniformBuffer(Args... args) {
    return hookCall<R>(0x027BF734, args...);
}
template <class R = void, class... Args> R resetUniformBuffer(Args... args) {
    return hookCall<R>(0x027BF7E8, args...);
}
template <class R = void, class... Args> R bindUniformBuffer(Args... args) {
    return hookCall<R>(0x027BFE5C, args...);
}
template <class R = void, class... Args> R constructPacket(Args... args) {
    return hookCall<R>(0x027F1278, args...);
}
template <class R = void, class... Args> R destructPacket(Args... args) {
    return hookCall<R>(0x027F13DC, args...);
}
template <class R = void, class... Args> R getShaderState(Args... args) {
    return hookCall<R>(0x027F29D4, args...);
}
template <class R = void, class... Args> R constructLink(Args... args) {
    return hookCall<R>(0x027FB40C, args...);
}
template <class R = void, class... Args> R destructLink(Args... args) {
    return hookCall<R>(0x027FB528, args...);
}
template <class R = void, class... Args> R setLinkIndex(Args... args) {
    return hookCall<R>(0x027FB5D4, args...);
}
template <class R = void, class... Args> R constructUniformArray(Args... args) {
    return hookCall<R>(0x027FD6F4, args...);
}
template <class R = void, class... Args> R destructUniformArray(Args... args) {
    return hookCall<R>(0x027FD764, args...);
}
template <class R = void, class... Args> R allocateUniformArray(Args... args) {
    return hookCall<R>(0x027FE084, args...);
}
template <class R = void, class... Args> R setVertexBuffer(Args... args) {
    return hookCall<R>(0x027FF478, args...);
}
template <class R = void, class... Args> R setUniformBuffer(Args... args) {
    return hookCall<R>(0x027FF530, args...);
}
template <class R = void, class... Args> R getResourceTable(Args... args) {
    return hookCall<R>(0x027FFCBC, args...);
}
template <class R = void, class... Args> R applyDrawContext(Args... args) {
    return hookCall<R>(0x027FFE54, args...);
}
template <class R = void, class... Args> R setRenderPass(Args... args) {
    return hookCall<R>(0x0280037C, args...);
}
template <class R = void, class... Args> R loadMaterialDescriptors(Args... args) {
    return hookCall<R>(0x0280068C, args...);
}
template <class R = void, class... Args> R initializeMaterialDescriptor(Args... args) {
    return hookCall<R>(0x02800B0C, args...);
}
template <class R = void, class... Args> R constructArray(Args... args) {
    return hookCall<R>(0x028EFFD0, args...);
}
template <class R = void, class... Args> R destroyArray(Args... args) {
    return hookCall<R>(0x028F0164, args...);
}
template <class R = void, class... Args> R zeroStorage(Args... args) {
    return hookCall<R>(0x028F521C, args...);
}
template <class R = void, class... Args> R GX2CallDisplayList(Args... args) {
    return hookCall<R>(0xC00060E0, args...);
}
template <class R = void, class... Args> R GX2DrawIndexedEx(Args... args) {
    return hookCall<R>(0xC0006178, args...);
}
template <class R = void, class... Args> R GX2SetGeometryUniformBlock(Args... args) {
    return hookCall<R>(0xC00068A8, args...);
}
template <class R = void, class... Args> R GX2SetPixelUniformBlock(Args... args) {
    return hookCall<R>(0xC0006900, args...);
}
template <class R = void, class... Args> R GX2SetVertexUniformBlock(Args... args) {
    return hookCall<R>(0xC0006A38, args...);
}
} // namespace hookshotGpu
u32 word(u32 p, u32 off = 0) { return gabi::load<u32>(p + off); }
void put(u32 p, u32 off, u32 value) { gabi::store<u32>(p + off, value); }
void virtualCall(u32 receiver, u32 slot, u32 argument) {
    u32 target = word(word(receiver, 0xC), slot);
    hookCall(target, receiver, argument);
}
void releaseAllocation(u32 address) {
    auto* allocation = hookshotView<HookshotAllocation>(address);
    u32 data = allocation->mData;
    u32 heap = hookshotGpu::findAllocationHeap<u32>(word(0x101F8B4C), data);
    virtualCall(heap, 0x3C, allocation->mData);
    allocation->mCount = 0;
    allocation->mData = 0;
}
void clearMaterialBuffers(u32 buffer) {
    for (u32 slot = 0; slot < 2; ++slot) {
        auto* array = hookshotView<HookshotAllocation>(buffer + 4 + slot * 8);
        u32 data = array->mData;
        if (data) {
            for (s32 i = 0; i < static_cast<s32>(array->mCount); ++i) {
                u32 element = data + u32(i) * 0xF4;
                hookCall(word(word(element, 0xF0), 0xC), element, 2);
                data = array->mData;
            }
            u32 heap = hookshotGpu::findAllocationHeap<u32>(word(0x101F8B4C), data);
            virtualCall(heap, 0x3C, array->mData);
            array->mCount = 0;
            array->mData = 0;
        }
    }
}
void resetShapeBuffers(u32 buffers) {
    auto* state = hookshotView<HookshotGpuBuffers>(buffers);
    hookshotGpu::resetUniformBuffer(buffers + 0x158);
    u32 allocation = state->mBuffers[0].mAllocation;
    state->mBuffers[0].mVertexData = 0;
    if (allocation)
        releaseAllocation(buffers + 0x24C);
    hookshotGpu::resetUniformBuffer(buffers + 0x3AC);
    allocation = state->mBuffers[1].mAllocation;
    state->mBuffers[1].mVertexData = 0;
    if (allocation)
        releaseAllocation(buffers + 0x4A0);
    state->mState = 0;
}
} // namespace

/* 02177FD4 */
static void daHookshot_shape_dt(u32 shape, s32 flags) {
    if (!shape)
        return;
    hookshotView<HookshotShapeView>(shape)->mVtable = 0x1001175C;
    u32 buffers = shape + 0xA4;
    resetShapeBuffers(buffers);
    for (s32 i = 0;
         i < (s32) static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mUniformCount); ++i) {
        u32 material = static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mUniforms);
        if (u32(i) < static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mUniformCount))
            material += u32(i) * 0x23C;
        for (u32 j = 0; j < 2; ++j)
            hookshotGpu::releaseUniformBuffer(material + 0x10 + j * 0x1C);
    }
    for (u32 start : {0x580u, 0x628u})
        for (u32 j = 0; j < 2; ++j)
            hookshotGpu::releaseUniformBuffer(shape + start + j * 0x1C);
    for (u32 i = 0; i < 300; ++i)
        for (u32 j = 0; j < 2; ++j)
            hookshotGpu::releaseUniformBuffer(shape + 0x990 + i * 0xA8 + j * 0x1C);
    hookshotGpu::destructTexture(shape + 0xCE78, 2);
    hookshotGpu::destructPrimitive(shape + 0xCE60, 2);
    hookshotGpu::destroyArray(shape + 0x980, 300, 0xA8, chainLink_dt, 0, 0);
    hookshotGpu::destructLink(shape + 0x618, 0);
    hookshotGpu::destructLink(shape + 0x570, 0);
    hookshotGpu::destructUniformArray(shape + 0x564, 2);
    if (buffers) {
        resetShapeBuffers(buffers);
        hookshotGpu::destroyArray(buffers, 2, 0x254, shapeBuf_dt, 0, 0);
    }
    u32 materials = static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntries);
    if (materials) {
        for (s32 i = 0;
             i < (s32) static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntryCount); ++i) {
            u32 buffer = materials + u32(i) * 0x14;
            if (buffer) {
                hookshotView<HookshotMaterialEntry>(buffer)->mShader = 0;
                clearMaterialBuffers(buffer);
                materials = static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntries);
            }
        }
        u32 heap = hookshotGpu::findAllocationHeap<u32>(word(0x101F8B4C), materials);
        virtualCall(heap, 0x3C, static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntries));
        hookshotView<HookshotShapeView>(shape)->mEntryCount = 0;
        hookshotView<HookshotShapeView>(shape)->mEntries = 0;
    }
    hookshotGpu::destructPacket(shape, 0);
    if (flags & 1)
        hookshotGpu::free(shape);
}

namespace {
struct RenderState {
    u8 mBase[0xC];
    be<u32> mPrimitiveMode;
    u8 mBeforeCull[0xD0];
    be<u8> mCullEnabled;
    u8 mBeforeFlags[0xB];
    be<u32> mRasterFlags;
    u8 mTail[0x2C];
};
WWHD_OFFSET(RenderState, mCullEnabled, 0xE0);
WWHD_OFFSET(RenderState, mRasterFlags, 0xEC);
WWHD_SIZE(RenderState, 0x11C);
void bindMaterialBuffer(u32 material, u32 source) {
    u32 buffer = source + 0x10 +
                 static_cast<u32>(hookshotView<HookshotMaterialView>(source)->mActiveBuffer) * 0x1C;
    u32 parameters =
        static_cast<u32>(hookshotView<HookshotMaterialView>(material)->mParameterCount)
            ? static_cast<u32>(hookshotView<HookshotMaterialView>(material)->mParameters)
            : 0;
    s16 vertex = hookshotView<HookshotShaderParameters>(parameters)->mVertexBlock;
    u32 size = hookshotView<HookshotUniformView>(buffer)->mSize,
        data = hookshotView<HookshotUniformView>(buffer)->mData;
    s16 pixel = hookshotView<HookshotShaderParameters>(parameters)->mPixelBlock;
    s16 geometry = hookshotView<HookshotShaderParameters>(parameters)->mGeometryBlock;
    if (pixel != -1)
        hookshotGpu::GX2SetPixelUniformBlock(pixel, data, size);
    if (vertex != -1)
        hookshotGpu::GX2SetVertexUniformBlock(vertex, data, size);
    if (geometry != -1)
        hookshotGpu::GX2SetGeometryUniformBlock(geometry, data, size);
}
void bindShapeTexture(u32 shape, u32 material) {
    u32 parameters = static_cast<u32>(hookshotView<HookshotMaterialView>(material)->mTextureCount)
                         ? static_cast<u32>(hookshotView<HookshotMaterialView>(material)->mTextures)
                         : 0;
    hookshotGpu::bindTexture(shape + 0xCE78, parameters + 4, -1, 0);
}
} // namespace
/* 021783F4 */
static void daHookshot_shape_drawGX2(u32 shape, u32 context) {
    s32 links = (s32)hookshotView<daHookshot_c>(
                    static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mUserArea))
                    ->mChainCnt;
    if ((s32)word(0x101B76F8) < links)
        put(0x101B76F8, 0, links);
    if (links <= 0)
        return;
    s32 index = (s32) static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mMode);
    u32 material = 0;
    if (index < 4) {
        u32 entry = static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntries);
        if (u32(index) < static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntryCount))
            entry += u32(index) * 0x14;
        material = hookshotView<HookshotMaterialEntry>(entry)->mShader;
    }
    u32 current = hookshotGpu::getShaderState<u32>(0x104B45C0);
    u32 shader = hookshotView<HookshotMaterialView>(material)->mShader;
    if (shader != static_cast<u32>(hookshotView<HookshotShaderState>(current)->mShader)) {
        u8 flags = hookshotView<HookshotShaderView>(shader)->mFlags;
        u32 previous = static_cast<u32>(hookshotView<HookshotShaderState>(current)->mProgram);
        if (flags & 2) {
            hookshotView<HookshotShaderView>(shader)->mFlags = flags & ~2u;
            hookshotGpu::updateShader(shader, 0);
        }
        u32 program = static_cast<u32>(
            hookshotView<HookshotProgramView>(hookshotView<HookshotShaderView>(shader)->mProgram)
                ->mHandle);
        if (previous != program)
            hookshotGpu::bindShaderProgram(program);
        u32 displaySize =
            static_cast<u32>(hookshotView<HookshotShaderView>(shader)->mDisplayListSize);
        if (displaySize)
            hookshotGpu::GX2CallDisplayList(
                static_cast<u32>(hookshotView<HookshotShaderView>(shader)->mDisplayList),
                displaySize);
        else
            hookshotGpu::applyShader(shader);
        hookshotView<HookshotShaderState>(current)->mProgram = program;
        hookshotView<HookshotShaderState>(current)->mShader = shader;
    }
    u32 mode = static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mMode);
    if (mode == 0) {
        u32 resource = static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mResource);
        if (resource)
            bindMaterialBuffer(
                material,
                static_cast<u32>(hookshotView<HookshotDrawResource>(resource)->mUniforms));
    } else if (mode == 1) {
        bindMaterialBuffer(material,
                           static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mUniforms));
        virtualCall(shape + 0x618, 0x2C, material);
        bindShapeTexture(shape, material);
    } else if (mode == 2) {
        u32 source =
            static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mPass) == 2
                ? static_cast<u32>(hookshotView<HookshotDrawResource>(
                                       hookshotView<HookshotRenderContext>(context)->mResource)
                                       ->mUniforms)
                : static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mUniforms);
        bindMaterialBuffer(material, source);
        virtualCall(shape + 0x618, 0x2C, material);
        u32 custom = static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mCustomLink);
        if (custom)
            virtualCall(custom, 0x2C, material);
        bindShapeTexture(shape, material);
        hookshotGpu::applyDrawContext(context, material);
    }
    gabi::Local<RenderState> state;
    hookshotGpu::initializeRenderState(state.get());
    u32 flags = state->mRasterFlags;
    state->mCullEnabled = 1;
    state->mRasterFlags = (flags & 0xFFFFFF00u) + 0x17;
    u32 selected = static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mMode),
        pass = static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mPass);
    state->mPrimitiveMode = pass == 2 ? 3 : 2;
    hookshotGpu::setRenderPass(selected, state.get());
    hookshotGpu::applyRenderState(state.get());
    if (links <= 0)
        links = 1;
    u32 link = shape + 0x980;
    for (s32 remaining = links; remaining; --remaining, link += 0xA8) {
        virtualCall(link, 0x2C, material);
        u32 entry = static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntries),
            selected = static_cast<u32>(hookshotView<HookshotRenderContext>(context)->mMode);
        if (selected < static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntryCount))
            entry += selected * 0x14;
        u32 alternate =
            static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mBuffers.mActiveBuffer) == 0
                ? 8
                : 0;
        hookshotGpu::bindUniformBuffer(word(entry + alternate, 8));
        u32 primitive = shape + 0xCE60;
        u32 count = static_cast<u32>(hookshotView<HookshotPrimitive>(primitive)->mCount);
        if (count)
            hookshotGpu::GX2DrawIndexedEx(
                static_cast<u32>(hookshotView<HookshotPrimitive>(primitive)->mIndices), count,
                static_cast<u32>(hookshotView<HookshotPrimitive>(primitive)->mType),
                static_cast<u32>(hookshotView<HookshotPrimitive>(primitive)->mFormat), 0, 1);
    }
}

namespace {
u32 allocateGpu(u32 bytes, u32 alignment) {
    u32 heap = hookshotGpu::getGpuHeap<u32>(word(0x101F8B4C));
    return hookCall<u32>(word(word(heap, 0xC), 0x34), heap, bytes, alignment);
}
void constructMaterialBuffers(u32 entry, u32 shader, u32 buffers) {
    for (u32 offset : {8u, 16u}) {
        u32 allocation = allocateGpu(0xF4, 4);
        if (allocation)
            hookshotGpu::constructUniformBuffer(allocation);
        if (allocation) {
            put(entry, offset, allocation);
            put(entry, offset - 4, 1);
        }
    }
    for (u32 i = 0; i < 2; ++i) {
        u32 part = entry + 4 + i * 8;
        u32 count = word(part), allocation = word(part, 4);
        hookshotGpu::setUniformBuffer(shader, allocation, buffers + 4 + i * 0x254, buffers + 0x4AC,
                                      0);
        (void)count; // The original tests this count but both tails perform the same call.
    }
}
f32 copiedRendererFloat(u32 bits) {
    f32 value;
    memcpy(&value, &bits, 4);
    return value;
}
void copyRendererFloats(u32 dst, u32 src, u32 count) {
    for (u32 i = 0; i < count; ++i)
        gabi::store<f32>(dst + 4 * i, copiedRendererFloat(word(src, 4 * i)));
}
} // namespace
struct InitialFloat {
    u32 offset;
    f32 value;
};
static const InitialFloat initial[] = {
    {0x68c, 0.0f}, {0x690, 0.0f}, {0x694, 0.0f}, {0x69c, 0.0f}, {0x6b8, 1.0f}, {0x6c4, 0.0f},
    {0x6a8, 1.0f}, {0x6a4, 0.0f}, {0x6f4, 0.0f}, {0x714, 0.0f}, {0x6e4, 0.0f}, {0x708, 1.0f},
    {0x6bc, 0.0f}, {0x6e0, 0.0f}, {0x710, 0.0f}, {0x700, 0.0f}, {0x6d0, 0.0f}, {0x6ac, 0.0f},
    {0x6c0, 0.0f}, {0x6c8, 1.0f}, {0x6b0, 0.0f}, {0x6d4, 0.0f}, {0x6a0, 0.0f}, {0x6fc, 0.0f},
    {0x704, 0.0f}, {0x718, 1.0f}, {0x6e8, 1.0f}, {0x6f8, 1.0f}, {0x6f0, 0.0f}, {0x6dc, 0.0f},
    {0x70c, 0.0f}, {0x6b4, 0.0f}, {0x6ec, 0.0f}, {0x698, 1.0f}, {0x6cc, 0.0f}, {0x6d8, 1.0f},
    {0x71c, 0.0f}, {0x720, 0.0f}, {0x724, 0.0f}, {0x728, 1.0f}, {0x72c, 0.0f}, {0x730, 0.0f},
    {0x734, 0.0f}, {0x738, 1.0f}};

/* 0217718C */
static u32 daHookshot_shape_ct(u32 shape) {
    if (!shape) {
        shape = hookshotGpu::allocate<u32>(0xD0A0);
        if (!shape)
            return 0;
    }
    hookshotGpu::constructPacket(shape);
    hookshotView<HookshotShapeView>(shape)->mMaterialCount = 0;
    hookshotView<HookshotShapeView>(shape)->mVtable = 0x1001175C;
    u32 header = shape + 0x9C;
    if (!header)
        header = hookshotGpu::allocate<u32>(8);
    if (header) {
        hookshotView<HookshotAllocation>(header)->mData = 0;
        hookshotView<HookshotAllocation>(header)->mCount = 0;
    }
    u32 buffers = shape + 0xA4, storage = buffers;
    if (!storage)
        storage = hookshotGpu::allocate<u32>(0x4C0);
    if (storage) {
        hookshotGpu::constructArray(storage, 2, 0x254, shapeBuf_ct);
        hookshotView<HookshotGpuBuffers>(storage)->mActiveBuffer = 0;
        hookshotView<HookshotGpuBuffers>(storage)->mAttributeCount = 0;
        hookshotView<HookshotGpuBuffers>(storage)->mState = 0;
        hookshotView<HookshotGpuBuffers>(storage)->mVertexStride = 0x20;
        hookshotView<HookshotGpuBuffers>(storage)->mUpdated = 0;
        hookshotView<HookshotGpuBuffers>(storage)->mBuffers[0].mVertexData = 0;
        hookshotView<HookshotGpuBuffers>(storage)->mBuffers[1].mVertexData = 0;
    }
    hookshotGpu::constructUniformArray(shape + 0x564);
    hookshotGpu::constructLink(shape + 0x570);
    hookshotView<HookshotShapeView>(shape)->mRootLinkVtable = 0x1016EF84;
    hookshotGpu::zeroStorage(shape + 0x5E4, 0x34);
    if (shape + 0x5E4 == 0)
        hookshotGpu::allocate<u32>(0x30);
    hookshotGpu::constructLink(shape + 0x618);
    hookshotView<HookshotShapeView>(shape)->mColorLinkVtable = 0x1016EFB4;
    hookshotGpu::zeroStorage(shape + 0x68C, 0x2F0);
    for (const auto& item : initial)
        gabi::store<f32>(shape + item.offset, item.value);
    for (u32 offset : {0x73Cu, 0x75Cu, 0x77Cu})
        hookshotGpu::constructArray(shape + offset, 2, 0x10, elem10_ct);
    for (u32 offset = 0x79C; offset <= 0x8EC; offset += 0x30)
        if (shape + offset == 0)
            hookshotGpu::allocate<u32>(0x30);
    for (u32 offset = 0x91C; offset <= 0x96C; offset += 0x10)
        if (shape + offset == 0)
            hookshotGpu::allocate<u32>(0x10);
    hookshotView<HookshotShapeView>(shape)->mRenderFlags = 0;
    hookshotGpu::constructArray(shape + 0x980, 300, 0xA8, chainLink_ct);
    u32 primitive = shape + 0xCE60, texture = shape + 0xCE78, resourceTexture = shape + 0xD010;
    hookshotGpu::constructPrimitive(primitive);
    hookshotGpu::constructTexture(texture);
    hookshotGpu::constructResourceTexture(resourceTexture);
    gabi::Local<SafeString> materialName;
    materialName->mStringTop = gabi::guest_addr("hookshot_chain");
    materialName->__vtbl = 0x10011560;
    u32 resources = hookshotGpu::getResourceTable<u32>();
    s32 index = hookshotGpu::findMaterial<s32>(
        static_cast<u32>(hookshotView<HookshotResourceTable>(resources)->mPackage),
        materialName.get());
    u32 descriptor = 0;
    if (index >= 0) {
        u32 count = static_cast<u32>(hookshotView<HookshotResourceTable>(resources)->mCount),
            data = static_cast<u32>(hookshotView<HookshotResourceTable>(resources)->mDescriptors);
        u32 candidate = data + (u32(index) < count ? u32(index) * 0x24 : 0);
        if (!gabi::load<u8>(candidate + 0x20)) {
            u32 package =
                static_cast<u32>(hookshotView<HookshotResourceTable>(resources)->mPackage);
            u32 shader =
                u32(index) < static_cast<u32>(
                                 hookshotView<HookshotResourcePackage>(package)->mShaderCount)
                    ? static_cast<u32>(hookshotView<HookshotResourcePackage>(package)->mShaders) +
                          u32(index) * 0x84
                    : 0;
            hookshotGpu::initializeMaterialDescriptor(candidate, shader, 0);
            count = static_cast<u32>(hookshotView<HookshotResourceTable>(resources)->mCount);
            data = static_cast<u32>(hookshotView<HookshotResourceTable>(resources)->mDescriptors);
        }
        descriptor = data + (u32(index) < count ? u32(index) * 0x24 : 0);
    }
    hookshotGpu::loadMaterialDescriptors(shape + 0x98, descriptor, 0);
    hookshotView<HookshotGpuBuffers>(buffers)->mAttributeCount = 0x13;
    hookshotView<HookshotGpuBuffers>(buffers)->mAttributes = 0x10011750;
    for (u32 i = 0; i < 2; ++i) {
        u32 buffer = buffers + i * 0x254;
        u32 data = static_cast<u32>(hookshotView<HookshotGpuBuffer>(buffer)->mVertexData);
        if (!data) {
            u32 allocation = allocateGpu(0x480, 0x40);
            if (allocation) {
                hookshotView<HookshotGpuBuffer>(buffer)->mAllocation = allocation;
                hookshotView<HookshotGpuBuffer>(buffer)->mAllocationCount = 0x24;
            }
            data = static_cast<u32>(hookshotView<HookshotGpuBuffer>(buffer)->mAllocation);
            hookshotView<HookshotGpuBuffer>(buffer)->mVertexData = data;
        }
        hookshotGpu::setVertexBuffer(buffer + 4, data, 0x24, buffers + 0x4AC);
    }
    hookshotView<HookshotGpuBuffers>(buffers)->mState = 0;
    hookshotView<HookshotGpuBuffers>(buffers)->mUpdated = 1;
    for (u32 i = 0; i < static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mMaterialCount);
         ++i) {
        u32 entry = static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntries);
        if (i < static_cast<u32>(hookshotView<HookshotShapeView>(shape)->mEntryCount))
            entry += i * 0x14;
        u32 shader = hookshotView<HookshotMaterialEntry>(entry)->mShader;
        hookshotView<HookshotMaterialEntry>(entry)->mShader = 0;
        clearMaterialBuffers(entry);
        hookshotView<HookshotMaterialEntry>(entry)->mShader = shader;
        constructMaterialBuffers(entry, shader, buffers);
    }
    hookshotGpu::allocateUniformArray(shape + 0x564, 1, 0);
    for (u32 i = 0; i < 300; ++i)
        hookshotGpu::setLinkIndex(shape + 0x980 + i * 0xA8, 0);
    hookshotGpu::setPrimitive(primitive, 0x101B761C, 4, word(0x101B7178));
    hookshotView<HookshotPrimitive>(primitive)->mIndices = 4;
    u32 active = static_cast<u32>(hookshotView<HookshotGpuBuffers>(buffers)->mActiveBuffer),
        buffer = buffers + active * 0x254;
    u32 data = static_cast<u32>(hookshotView<HookshotGpuBuffer>(buffer)->mVertexData),
        end = data + 0x480;
    if (data < end) {
        for (u32 p = data; p < end; p += 0x20)
            for (u32 j = 0; j < 0x20; j += 4)
                put(p & ~31u, j, 0);
        active = static_cast<u32>(hookshotView<HookshotGpuBuffers>(buffers)->mActiveBuffer);
        buffer = buffers + active * 0x254;
    }
    u32 vertices = word(0x101B7174),
        dst = static_cast<u32>(hookshotView<HookshotGpuBuffer>(buffer)->mVertexData);
    for (u32 i = 0; i < vertices; ++i) {
        copyRendererFloats(dst + i * 0x20, 0x101B719C + i * 12, 3);
        copyRendererFloats(dst + i * 0x20 + 12, 0x101B734C + i * 12, 3);
        copyRendererFloats(dst + i * 0x20 + 24, 0x101B74FC + i * 8, 2);
    }
    if (vertices) {
        active = static_cast<u32>(hookshotView<HookshotGpuBuffers>(buffers)->mActiveBuffer);
        buffer = buffers + active * 0x254;
    }
    u32 alternate = buffers + (active == 0 ? 0x254 : 0);
    for (u32 i = 0; i < 36; ++i) {
        u32 src = static_cast<u32>(hookshotView<HookshotGpuBuffer>(buffer)->mVertexData),
            target = static_cast<u32>(hookshotView<HookshotGpuBuffer>(alternate)->mVertexData);
        copyRendererFloats(target + i * 32, src + i * 32, 8);
    }
    active = static_cast<u32>(hookshotView<HookshotGpuBuffers>(buffers)->mActiveBuffer);
    buffer = buffers + active * 0x254;
    hookshotGpu::flushVertexBuffer(
        buffer + 4, 0, static_cast<u32>(hookshotView<HookshotGpuBuffer>(buffer)->mVertexDataSize));
    hookshotView<HookshotGpuBuffers>(buffers)->mActiveBuffer =
        static_cast<u32>(hookshotView<HookshotGpuBuffers>(buffers)->mActiveBuffer) == 0 ? 1 : 0;
    hookshotGpu::lockTextureCache(word(0x101F8B18));
    gabi::Local<SafeString> archiveName, textureName;
    archiveName->mStringTop = gabi::guest_addr("ProgramTexture");
    archiveName->__vtbl = 0x10011560;
    textureName->mStringTop = gabi::guest_addr("ProgramTexture.bfres");
    textureName->__vtbl = 0x10011560;
    u32 textureData =
        hookshotGpu::loadResource<u32>(word(0x101F4F7C), archiveName.get(), textureName.get(), 0);
    hookshotGpu::loadResourceTexture(resourceTexture, textureData, 0x100116CC);
    hookshotGpu::unlockTextureCache(word(0x101F8B18));
    bool equal = true;
    for (u32 off : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u})
        if (hookshotView<HookshotTextureView>(texture)->mSurfaceWords[off / 4] !=
            hookshotView<HookshotTextureView>(resourceTexture)->mSurfaceWords[off / 4]) {
            equal = false;
            break;
        }
    if (!equal) {
        hookshotGpu::copyTexture(texture, resourceTexture);
        u8 flags = hookshotView<HookshotTextureView>(texture)->mFlags;
        hookshotView<HookshotTextureView>(texture)->mImageDirty = 1;
        hookshotView<HookshotTextureView>(texture)->mSamplerDirty = 1;
        hookshotView<HookshotTextureView>(texture)->mMipDirty = 1;
        hookshotView<HookshotTextureView>(texture)->mFlags = flags | 2;
    } else {
        u8 flags = hookshotView<HookshotTextureView>(texture)->mFlags;
        u32 a = static_cast<u32>(
                hookshotView<HookshotTextureView>(resourceTexture)->mSurfaceWords[10]),
            b = static_cast<u32>(
                hookshotView<HookshotTextureView>(resourceTexture)->mSurfaceWords[12]);
        hookshotView<HookshotTextureView>(texture)->mSurfaceWords[10] = a;
        hookshotView<HookshotTextureView>(texture)->mFlags = flags | 2;
        hookshotView<HookshotTextureView>(texture)->mCachedMip = b;
        hookshotView<HookshotTextureView>(texture)->mSurfaceWords[12] = b;
        hookshotView<HookshotTextureView>(texture)->mMipDirty = 1;
        hookshotView<HookshotTextureView>(texture)->mCachedImage = a;
        hookshotView<HookshotTextureView>(texture)->mImageDirty = 1;
        hookshotView<HookshotTextureView>(texture)->mSamplerDirty = 1;
    }
    return shape;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): Hookshot renderer
str 100116D8 in 0217718C
str 100116E8 in 0217718C
str 100116F8 in 0217718C
*/
// clang-format on
