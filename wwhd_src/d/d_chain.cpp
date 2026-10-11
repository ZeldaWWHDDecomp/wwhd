/** Chain rendering packet, GameCube names with HD GPU-resource state.
 * Every verified function and memory effect retained.
 */
#include "d/d_chain.h"
using namespace chainDetail;
static u32 l_heapManager;
static u32 l_indexData;
static u32 l_vertexCount;
static u32 l_graphicsContext;
static u32 l_resourceManager;
static u32 l_staticLower;
static u32 l_staticUpper;
static f32 l_packetZero;
static f32 l_packetOne;
static f32 l_segmentAdjustment;
static f32 l_segmentBase;
static f32 l_zero;
static f32 l_colorDenominator;
static u8 l_safeStringVtable;
static u8 l_shapeName;
static u8 l_shaderName;
static u8 l_shapeArchive;
static u8 l_shapeFilename;
static u8 l_vertexFormatName;
static u8 l_chainVtable;
static u8 l_segmentPacketVtable;
static u8 l_uniformPacketVtable;
static u8 l_positions;
static u8 l_normals;
static u8 l_texCoords;
static u8 l_indices;
static u8 l_staticDestructor0;
static u8 l_staticDestructor1;
static u8 l_staticDestructor2;
static u32 l_staticRange;
static u32 l_staticRangeEnd;
static u8 l_staticAngle;
static u8 l_staticFlag;
static u32 l_staticWord0;
static u32 l_staticWord1;
static u32 l_staticWord2;
static u32 l_staticWord3;
static u8 l_matrixStack;
static u8 l_graphics;

// HD: release the two GPU allocations owned by an inlined shader resource pair.
static void chain_ReleaseResources(u32 resource) {
    for (u32 offset : {0u, 0x254u}) {
        u32 item = resource + offset;
        auto* resourceItem = gabi::at<ChainResourceHD>(item);
        chain_releaseShaderBlock(item + 0x158);
        u32 allocation = resourceItem->mpAllocation.get();
        resourceItem->mpData = 0;
        if (allocation) {
            u32 heap = chain_findContainingHeap(l_heapManager, allocation);
            u32 table = gabi::at<ChainHeapHD>(heap)->mVtable.get();
            gabi::call_ptr<void>(gabi::at<ChainHeapVtableHD>(table)->mFree.get(), heap,
                                 resourceItem->mpAllocation.get());
            resourceItem->mCount = 0;
            resourceItem->mpAllocation = 0;
        }
    }
    gabi::at<ChainResourcesHD>(resource)->mDirty = 0;
}
static void chain_FreeHeapField(u32 object, u32 offset) {
    u32 heap = chain_findContainingHeap(l_heapManager, gabi::at<be<u32>>(object + offset)->get());
    u32 table = gabi::at<ChainHeapHD>(heap)->mVtable.get();
    gabi::call_ptr<void>(gabi::at<ChainHeapVtableHD>(table)->mFree.get(), heap,
                         gabi::at<be<u32>>(object + offset)->get());
}
/* 02519814 */
// HD: GHS constructor entry accepts null storage and returns the allocated packet.
u32 dChain_packet_construct(u32 self, u32 count, u32 tev, f32 scale) {
    gabi::Local<ChainString> names, archive, filename;
    if (!self) {
        self = chain_operatorNew(0xB32C);
        if (!self)
            return 0;
    }
    auto* chain = gabi::at<dChain_packet_c>(self);
    chain_packetBaseConstruct(self);
    chain->mPt = 0;
    chain->mShaderCount = 0;
    u32 string = self + 0xB4;
    chain->mVtable = gabi::ea(&l_chainVtable);
    if (!string)
        string = chain_operatorNew(8);
    if (string) {
        gabi::at<ChainString>(string)->mVtable = 0;
        gabi::at<ChainString>(string)->mpText = 0;
    }
    chain_packetArrayConstruct(self + 0xBC);
    chain_constructArray(self + 0xC8, 256, 0xA8,
                         gabi::guest_fn((const void*)&chain_InlinePacketCtor));
    u32 packet = self + 0xA8C8;
    auto* uniformPacket = gabi::at<ChainDrawPacketHD>(packet);
    chain_drawPacketConstruct(packet);
    uniformPacket->mVtable = gabi::ea(&l_uniformPacketVtable);
    chain_clearMemory(packet + 0x74, 0x2F0);
    f32 zero = l_packetZero, one = l_packetOne;
    uniformPacket->mUniforms[0] = zero;
    uniformPacket->mUniforms[1] = zero;
    uniformPacket->mUniforms[2] = zero;
    uniformPacket->mUniforms[33] = zero;
    uniformPacket->mUniforms[3] = one;
    uniformPacket->mUniforms[28] = zero;
    uniformPacket->mUniforms[7] = one;
    uniformPacket->mUniforms[21] = zero;
    uniformPacket->mUniforms[31] = one;
    uniformPacket->mUniforms[34] = zero;
    uniformPacket->mUniforms[19] = one;
    uniformPacket->mUniforms[13] = zero;
    uniformPacket->mUniforms[22] = zero;
    uniformPacket->mUniforms[11] = one;
    uniformPacket->mUniforms[12] = zero;
    uniformPacket->mUniforms[6] = zero;
    uniformPacket->mUniforms[38] = zero;
    uniformPacket->mUniforms[24] = zero;
    gabi::at<ChainOwnerHD>(packet)->mPt = zero;
    uniformPacket->mUniforms[29] = zero;
    uniformPacket->mUniforms[23] = one;
    uniformPacket->mUniforms[37] = zero;
    uniformPacket->mUniforms[15] = one;
    uniformPacket->mUniforms[30] = zero;
    uniformPacket->mUniforms[9] = zero;
    uniformPacket->mUniforms[16] = zero;
    uniformPacket->mUniforms[8] = zero;
    uniformPacket->mUniforms[32] = zero;
    uniformPacket->mUniforms[39] = one;
    uniformPacket->mUniforms[10] = zero;
    uniformPacket->mUniforms[35] = one;
    uniformPacket->mUniforms[18] = zero;
    uniformPacket->mUniforms[40] = zero;
    uniformPacket->mUniforms[25] = zero;
    uniformPacket->mUniforms[5] = zero;
    uniformPacket->mUniforms[20] = zero;
    uniformPacket->mUniforms[41] = zero;
    uniformPacket->mUniforms[36] = zero;
    uniformPacket->mUniforms[27] = one;
    uniformPacket->mUniforms[26] = zero;
    uniformPacket->mUniforms[42] = zero;
    uniformPacket->mUniforms[4] = zero;
    uniformPacket->mUniforms[17] = zero;
    uniformPacket->mUniforms[43] = one;
    for (u32 off : {0x124u, 0x144u, 0x164u})
        chain_constructArray(packet + off, 2, 16,
                             gabi::guest_fn((const void*)&chain_InlineColorCtor));
    for (u32 off = 0x184; off <= 0x2D4; off += 0x30)
        if (!(packet + off))
            chain_operatorNew(0x30);
    for (u32 off = 0x304; off <= 0x354; off += 0x10)
        if (!(packet + off))
            chain_operatorNew(0x10);
    u32 resources = self + 0xAC2C, actual = resources;
    auto* resourcePair = gabi::at<ChainResourcesHD>(resources);
    if (!actual)
        actual = chain_operatorNew(0x4C0);
    if (actual) {
        auto* actualPair = gabi::at<ChainResourcesHD>(actual);
        chain_constructArray(actual, 2, 0x254,
                             gabi::guest_fn((const void*)&chain_InlineResourceCtor));
        actualPair->mActive = 0;
        actualPair->mFormat = 0;
        actualPair->mDirty = 0;
        actualPair->mAlignment = 0x20;
        actualPair->mReady = 0;
        gabi::at<ChainResourceHD>(actual)->mpData = 0;
        actualPair->mResource[1].mpData = 0;
    }
    u32 indices = self + 0xB0EC, shape = self + 0xB104, temporary = self + 0xB29C;
    auto* indexBuffer = gabi::at<ChainIndexBufferHD>(indices);
    auto* shapeInfo = gabi::at<ChainShapeHD>(shape);
    auto* temporaryInfo = gabi::at<ChainTemporaryShapeHD>(temporary);
    chain_indexBufferConstruct(indices);
    chain_shapeConstruct(shape);
    chain_temporaryShapeConstruct(temporary);
    chain->mpTevStr = tev;
    chain->mNum = count;
    chain->mScale.z = scale;
    chain->mScale.y = scale;
    chain->mScale.x = scale;
    u32 points = chain_constructArray(0, count, 12, 0);
    chain->mPt = points;
    names->mpText = gabi::ea(&l_shaderName);
    names->mVtable = gabi::ea(&l_safeStringVtable);
    u32 manager = chain_shaderManagerInstance();
    auto* shaderManager = gabi::at<ChainShaderManagerHD>(manager);
    s32 found = chain_findShader(shaderManager->mpArchive.get(), names.a);
    u32 chosen = 0;
    if (found >= 0) {
        u32 total = shaderManager->mShaderCount.get(), objects = shaderManager->mpShaders.get();
        u32 item = objects + ((u32)found < total ? (u32)found * 36 : 0);
        if (!gabi::at<ChainShaderObjectHD>(item)->mLoaded.get()) {
            u32 file = shaderManager->mpArchive.get();
            u32 limit = gabi::at<ChainShaderArchiveHD>(file)->mCount.get(), data = 0;
            if ((u32)found < limit)
                data = gabi::at<ChainShaderArchiveHD>(file)->mpEntries.get() + (u32)found * 0x84;
            chain_shaderObjectInitialize(item, data, 0);
            total = shaderManager->mShaderCount.get();
            objects = shaderManager->mpShaders.get();
        }
        chosen = objects + ((u32)found < total ? (u32)found * 36 : 0);
    }
    chain_setupShaderEntries(self + 0xB0, chosen, 0);
    resourcePair->mFormat = 0x13;
    resourcePair->mpName = gabi::ea(&l_vertexFormatName);
    for (u32 offset : {0u, 0x254u}) {
        u32 item = resources + offset;
        auto* resourceItem = gabi::at<ChainResourceHD>(item);
        if (!resourceItem->mpData.get()) {
            u32 heap = chain_currentHeap(l_heapManager);
            u32 table = gabi::at<ChainHeapHD>(heap)->mVtable.get();
            u32 allocation = gabi::call_ptr<u32>(gabi::at<ChainHeapVtableHD>(table)->mAlloc.get(),
                                                 heap, 0x480, 0x40);
            if (allocation) {
                resourceItem->mpAllocation = allocation;
                resourceItem->mCount = 0x24;
            }
            resourceItem->mpData = resourceItem->mpAllocation.get();
        }
        chain_setupVertexBuffer(item + 4, resourceItem->mpData.get(), 0x24, resources + 0x4AC);
    }
    resourcePair->mDirty = 0;
    resourcePair->mReady = 1;
    u32 index = 0;
    while (index < chain->mShaderCount.get()) {
        u32 entry = chain->mpShaderEntries.get();
        if (index < chain->mShaderCapacity.get())
            entry += index * 20;
        u32 saved = gabi::at<ChainShaderEntryHD>(entry)->mpShader.get();
        gabi::at<ChainShaderEntryHD>(entry)->mpShader = 0;
        for (u32 field : {4u, 12u}) {
            u32 data = gabi::at<ChainBindingArrayHD>(entry + field)->mpData.get();
            if (data) {
                u32 j = 0, offset = 0;
                while ((s32)j < (s32)gabi::at<ChainBindingArrayHD>(entry + field)->mCount.get()) {
                    u32 object = data + offset,
                        table = gabi::at<ChainShaderBlockHD>(object)->mVtable.get();
                    gabi::call_ptr<void>(gabi::at<ChainPacketVtableHD>(table)->mDestroy.get(),
                                         object, 2);
                    ++j;
                    offset += 0xF4;
                    data = gabi::at<ChainBindingArrayHD>(entry + field)->mpData.get();
                }
                chain_FreeHeapField(entry, field + 4);
                gabi::at<ChainBindingArrayHD>(entry + field)->mCount = 0;
                gabi::at<ChainBindingArrayHD>(entry + field)->mpData = 0;
            }
        }
        gabi::at<ChainShaderEntryHD>(entry)->mpShader = saved;
        for (u32 field : {4u, 12u}) {
            u32 heap = chain_currentHeap(l_heapManager);
            u32 table = gabi::at<ChainHeapHD>(heap)->mVtable.get();
            u32 allocation = gabi::call_ptr<u32>(gabi::at<ChainHeapVtableHD>(table)->mAlloc.get(),
                                                 heap, 0xF4, 4);
            if (allocation)
                chain_shaderBlockConstruct(allocation);
            if (allocation) {
                gabi::at<ChainBindingArrayHD>(entry + field)->mpData = allocation;
                gabi::at<ChainBindingArrayHD>(entry + field)->mCount = 1;
            }
        }
        for (u32 j = 0; j < 2; ++j)
            chain_setupShaderBlock(saved,
                                   gabi::at<ChainBindingArrayHD>(entry + 4 + j * 8)->mpData.get(),
                                   resources + 4 + j * 0x254, resources + 0x4AC, 0);
        ++index;
    }
    chain_resizePacketArray(self + 0xBC, 1, 0);
    chain_drawPacketSetup(packet, 0);
    for (u32 j = 0; j < 256; ++j)
        chain_drawPacketSetup(self + 0xC8 + j * 0xA8, 0);
    chain_setupIndexBuffer(indices, gabi::ea(&l_indices), 4, l_indexData);
    indexBuffer->mIndexType = 4;
    u32 active = resourcePair->mActive.get();
    u32 selected = resources + active * 0x254,
        data = gabi::at<ChainResourceHD>(selected)->mpData.get(), end = data + 0x480;
    if (data < end) {
        do {
            for (u32 k = 0; k < 32; ++k)
                gabi::at<be<u8>>((data & ~31u) + k)->set(0);
            data += 32;
        } while (data < end);
        active = resourcePair->mActive.get();
        selected = resources + active * 0x254;
    }
    u32 vertices = l_vertexCount;
    data = gabi::at<ChainResourceHD>(selected)->mpData.get();
    for (u32 j = 0; j < vertices; ++j) {
        for (u32 k = 0; k < 3; ++k)
            gabi::at<ChainVertexDataHD>(data + j * sizeof(ChainVertexDataHD))->mPosition[k] =
                gabi::at<ChainVectorArrayHD>(gabi::ea(&l_positions) + j * sizeof(ChainVector))
                    ->mComponent[k]
                    .get();
        for (u32 k = 0; k < 3; ++k)
            gabi::at<ChainVertexDataHD>(data + j * sizeof(ChainVertexDataHD))->mNormal[k] =
                gabi::at<ChainVectorArrayHD>(gabi::ea(&l_normals) + j * sizeof(ChainVector))
                    ->mComponent[k]
                    .get();
        for (u32 k = 0; k < 2; ++k)
            gabi::at<ChainVertexDataHD>(data + j * sizeof(ChainVertexDataHD))->mTexCoord[k] =
                gabi::at<ChainTexCoordHD>(gabi::ea(&l_texCoords) + j * sizeof(ChainTexCoordHD))
                    ->mComponent[k]
                    .get();
        vertices = l_vertexCount;
    }
    if (vertices) {
        active = resourcePair->mActive.get();
        selected = resources + active * 0x254;
    }
    u32 other = resources + (!active ? 0x254 : 0);
    for (u32 j = 0; j < 36; ++j) {
        u32 from = gabi::at<ChainResourceHD>(selected)->mpData.get(),
            to = gabi::at<ChainResourceHD>(other)->mpData.get();
        for (u32 k = 0; k < 8; ++k)
            gabi::at<ChainFloatWordsHD>(to + j * sizeof(ChainVertexDataHD))->mValue[k] =
                gabi::at<ChainFloatWordsHD>(from + j * sizeof(ChainVertexDataHD))->mValue[k].get();
    }
    active = resourcePair->mActive.get();
    selected = resources + active * 0x254;
    chain_invalidateVertexBuffer(selected + 4, 0,
                                 gabi::at<ChainVertexSizeHD>(selected)->mSize.get());
    active = resourcePair->mActive.get();
    resourcePair->mActive = !active;
    chain_enterGraphicsContext(l_graphicsContext);
    archive->mVtable = gabi::ea(&l_safeStringVtable);
    filename->mVtable = gabi::ea(&l_safeStringVtable);
    filename->mpText = gabi::ea(&l_shapeFilename);
    archive->mpText = gabi::ea(&l_shapeArchive);
    u32 loaded = chain_loadResource(l_resourceManager, archive.a, filename.a, 0);
    chain_readShapeResource(temporary, loaded, gabi::ea(&l_shapeName));
    bool equal = true;
    for (u32 off : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u}) {
        if (shapeInfo->mLayoutWords[off / sizeof(u32)].get() !=
            temporaryInfo->mLayoutWords[off / sizeof(u32)].get()) {
            equal = false;
            break;
        }
    }
    if (!equal)
        chain_copyShape(shape, temporary);
    else {
        u32 last = temporaryInfo->mLayoutWords[12].get(),
            first = temporaryInfo->mLayoutWords[10].get();
        shapeInfo->mpLastBinding = last;
        shapeInfo->mLayoutWords[10] = first;
        shapeInfo->mpFirstBinding = first;
        shapeInfo->mLayoutWords[12] = last;
    }
    u8 flags = shapeInfo->mFlags.get();
    shapeInfo->mDirtyNormal = 1;
    shapeInfo->mDirtyPosition = 1;
    shapeInfo->mDirtyTexCoord = 1;
    shapeInfo->mFlags = flags | SHAPE_DIRTY;
    chain_leaveGraphicsContext(l_graphicsContext);
    return self;
}

/* 0251A324 */
void dChain_packet_c::destroy(u32 flags) {
    u32 self = gabi::ea(this);
    if (!self)
        return;
    this->mVtable = gabi::ea(&l_chainVtable);
    u32 resources = self + 0xAC2C;
    chain_ReleaseResources(resources);
    u32 index = 0, offset = 0;
    while ((s32)index < (s32)this->mPacketCount.get()) {
        u32 count = this->mPacketCount.get();
        u32 entry = this->mpPackets.get();
        if (index < count)
            entry += offset;
        chain_releasePacketResource(entry + 0x10);
        chain_releasePacketResource(entry + 0x2C);
        ++index;
        offset += 0x23C;
    }
    chain_releasePacketResource(self + 0xA8D8);
    chain_releasePacketResource(self + 0xA8F4);
    for (u32 i = 0; i < 256; ++i) {
        chain_releasePacketResource(self + 0xD8 + i * 0xA8);
        chain_releasePacketResource(self + 0xF4 + i * 0xA8);
    }
    u32 points = this->mPt.get();
    if (points) {
        chain_destroyArray(points, -1, 12, 0, 1, 0);
        this->mPt = 0;
    }
    chain_shapeDestroy(self + 0xB104, 2);
    chain_indexBufferDestroy(self + 0xB0EC, 2);
    if (resources) {
        chain_ReleaseResources(resources);
        chain_destroyArray(resources, 2, 0x254,
                           gabi::guest_fn((const void*)&chain_InlineResourceDtor), 0, 0);
    }
    chain_drawPacketDestroy(self + 0xA8C8, 0);
    chain_destroyArray(self + 0xC8, 256, 0xA8, gabi::guest_fn((const void*)&chain_InlinePacketDtor),
                       0, 0);
    chain_packetArrayDestroy(self + 0xBC, 2);
    u32 entries = this->mpShaderEntries.get();
    if (entries) {
        index = 0;
        offset = 0;
        u32 count = this->mShaderCapacity.get();
        while ((s32)index < (s32)count) {
            u32 entry = entries + offset;
            if (entry) {
                gabi::at<ChainShaderEntryHD>(entry)->mpShader = 0;
                for (u32 field : {4u, 12u}) {
                    u32 data = gabi::at<ChainBindingArrayHD>(entry + field)->mpData.get();
                    if (!data)
                        continue;
                    u32 j = 0, position = 0;
                    while ((s32)j <
                           (s32)gabi::at<ChainBindingArrayHD>(entry + field)->mCount.get()) {
                        u32 object = data + position;
                        u32 table = gabi::at<ChainShaderBlockHD>(object)->mVtable.get();
                        gabi::call_ptr<void>(gabi::at<ChainPacketVtableHD>(table)->mDestroy.get(),
                                             object, 2);
                        ++j;
                        position += 0xF4;
                        data = gabi::at<ChainBindingArrayHD>(entry + field)->mpData.get();
                    }
                    chain_FreeHeapField(entry, field + 4);
                    gabi::at<ChainBindingArrayHD>(entry + field)->mCount = 0;
                    gabi::at<ChainBindingArrayHD>(entry + field)->mpData = 0;
                }
                count = this->mShaderCapacity.get();
                entries = this->mpShaderEntries.get();
            }
            ++index;
            offset += 20;
        }
        chain_FreeHeapField(self, 0xB8);
        this->mShaderCapacity = 0;
        this->mpShaderEntries = 0;
    }
    chain_packetBaseDestroy(self, 0);
    if (flags & DELETE_STORAGE)
        chain_operatorDelete(self);
}

static void chain_BindAttributes(u32 shader, u32 vertex) {
    u32 selected = vertex + 0x10 + gabi::at<ChainVertexHD>(vertex)->mActive.get() * 0x1C;
    u32 attributes = gabi::at<ChainShaderHD>(shader)->mAttributeCount.get()
                         ? gabi::at<ChainShaderHD>(shader)->mpAttributes.get()
                         : 0;
    s32 first = gabi::at<ChainAttributeLocationsHD>(attributes)->mPosition.get();
    u32 stride = gabi::at<ChainVertexBufferHD>(selected)->mStride.get(),
        buffer = gabi::at<ChainVertexBufferHD>(selected)->mpBuffer.get();
    s32 second = gabi::at<ChainAttributeLocationsHD>(attributes)->mNormal.get();
    s32 third = gabi::at<ChainAttributeLocationsHD>(attributes)->mTexCoord.get();
    if (second != -1)
        chain_bindNormalBuffer(second, buffer, stride);
    if (first != -1)
        chain_bindPositionBuffer(first, buffer, stride);
    if (third != -1)
        chain_bindTexCoordBuffer(third, buffer, stride);
}
/* 0251AE90 */
// HD: draw builds per-link GPU packets; GameCube emitted display lists directly.
void dChain_packet_c::draw() {
    u32 self = gabi::ea(this);
    gabi::Local<ChainMatrix> view, matrix;
    gabi::Local<ChainColor> color, first, second, quat;
    gabi::Local<ChainVector> pos, dir, step;
    const u32 graphics = gabi::ea(&l_graphics);
    chain_copyViewMatrix(view.a, graphics + 0x38);
    chain_setTevFog(this->mpTevStr.get());
    u32 global = gabi::at<ChainGraphicsHD>(graphics)->mpCamera.get();
    chain_preparePacketArray(self + 0xBC, 0, view.a, graphics + 0x14C, global + 0x240);
    u32 tev = this->mpTevStr.get();
    u32 packet = this->mpPackets.get();
    chain_colorToFloat(color.a, tev + 0x90);
    tev = this->mpTevStr.get();
    f32 brightness = gabi::at<ChainTevHD>(tev)->mBrightness.get();
    chain_scaleColor(first.a, color.a, brightness);
    for (u32 i = 0; i < 4; ++i)
        gabi::at<ChainColorWordsHD>(packet + 0x1C4)->mValue[i] =
            reinterpret_cast<ChainColorWordsHD*>(first.get())->mValue[i].get();
    tev = this->mpTevStr.get();
    packet = this->mpPackets.get();
    chain_colorToFloat(color.a, tev + 0x160);
    tev = this->mpTevStr.get();
    brightness = gabi::at<ChainTevHD>(tev)->mLightBrightness.get();
    chain_scaleColor(second.a, color.a, brightness);
    for (u32 i = 0; i < 4; ++i)
        gabi::at<ChainColorWordsHD>(packet + 0x1D4)->mValue[i] =
            reinterpret_cast<ChainColorWordsHD*>(second.get())->mValue[i].get();
    chain_updatePacketArray(self + 0xBC);
    chain_resetTevFog();
    tev = this->mpTevStr.get();
    f32 r = (f32)gabi::at<ChainTevHD>(tev)->mColorC0[0].get(),
        b = (f32)gabi::at<ChainTevHD>(tev)->mColorC0[2].get();
    f32 g = (f32)gabi::at<ChainTevHD>(tev)->mColorC0[1].get(),
        a = (f32)gabi::at<ChainTevHD>(tev)->mColorC0[3].get();
    f32 denominator = l_colorDenominator;
    u32 shader = self + 0xA8C8;
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[16] = r / denominator;
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[17] = g / denominator;
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[18] = b / denominator;
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[19] = a / denominator;
    r = (f32)gabi::at<ChainTevHD>(tev)->mColorK0[0].get();
    g = (f32)gabi::at<ChainTevHD>(tev)->mColorK0[1].get();
    b = (f32)gabi::at<ChainTevHD>(tev)->mColorK0[2].get();
    a = (f32)gabi::at<ChainTevHD>(tev)->mColorK0[3].get();
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[21] = g / denominator;
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[22] = b / denominator;
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[20] = r / denominator;
    gabi::at<ChainDrawPacketHD>(shader)->mUniforms[23] = a / denominator;
    tev = this->mpTevStr.get();
    f32 alpha = gabi::at<ChainTevHD>(tev)->mAlpha.get();
    chain_scaleAlpha(shader + 0xC4, alpha);
    packet = this->mpPackets.get();
    for (u32 i = 0; i < 4; ++i)
        gabi::at<ChainColorWordsHD>(shader + 0x94)->mValue[i] =
            gabi::at<ChainColorWordsHD>(packet + 0x1C4)->mValue[i].get();
    chain_updateDrawPacket(shader);
    u32 points = this->mPt.get();
    u32 count = this->mNum.get();
    f32 x = gabi::at<ChainVector>(points)->x.get();
    f32 adjustment = l_segmentAdjustment;
    f32 base = l_segmentBase;
    pos->x = x;
    f32 y = gabi::at<ChainVector>(points)->y.get();
    f32 segment = (adjustment + base) * this->mScale.z.get();
    pos->y = y;
    pos->z = gabi::at<ChainVector>(points)->z.get();
    u32 index = 0, packetOffset = 0;
    s16 angle = 0;
    if ((s32)index >= (s32)(count - 1))
        return;
    f32 zero = l_zero;
    u32 next = points + 12;
    const u32 stackMatrix = gabi::ea(&l_matrixStack);
    do {
        chain_vectorSubtract(next, dir.a, pos.a);
        f32 squared = chain_vectorMagnitudeSquared(dir.a);
        f32 length = chain_sqrtf(squared);
        if (chain_vectorNormalizeRS(dir.a)) {
            chain_quatRotBaseZ(quat.a, dir.a);
            while (length > zero) {
                f32 px = pos->x.get(), pz = pos->z.get(), py = pos->y.get();
                chain_matrixTranslate(stackMatrix, px, py, pz);
                chain_matrixQuatM(quat.a);
                chain_matrixZrotM(stackMatrix, (s32)angle);
                f32 sz = this->mScale.z.get(), sy = this->mScale.y.get(), sx = this->mScale.x.get();
                chain_matrixScaleM(sx, sy, sz);
                f32 values[12];
                for (u32 i = 0; i < 12; ++i)
                    values[i] = gabi::at<ChainMatrix>(stackMatrix)->m[i].get();
                for (u32 i = 0; i < 12; ++i)
                    matrix->m[i] = values[i];
                u32 entry = self + 0xC8 + packetOffset;
                chain_matrixConcat(matrix.a, entry + 0x74);
                chain_updateDrawPacket(entry);
                length -= segment;
                packetOffset += 0xA8;
                chain_vectorScale(dir.a, step.a, segment);
                chain_vectorAdd(pos.a, step.a, pos.a);
                angle = (s16)(u16)((u16)angle + 0x4000);
            }
        }
        ++index;
        count = this->mNum.get();
        next += 12;
    } while ((s32)index < (s32)(count - 1));
}

/* 0251A2CC */
u32 dChain_packet_create(u32 count, u32 tev, f32 scale) {
    u32 packet = chain_constructPacket(0, count, tev, scale);
    if (packet && !gabi::at<ChainOwnerHD>(packet)->mPt.get()) {
        u32 vtable = gabi::at<ChainDrawPacketHD>(packet)->mVtable.get();
        u32 destructor = gabi::at<ChainPacketVtableHD>(vtable)->mDestroy.get();
        gabi::call_ptr<void>(destructor, packet, 3);
        return 0;
    }
    return packet;
}

/* 0251A74C */
// HD: shader pass rendering uses the packet state prepared by draw().
void dChain_packet_c::drawHD(u32 pass) {
    u32 self = gabi::ea(this);
    gabi::Local<ChainRenderState> state;
    gabi::Local<ChainVector> pos, dir, step;
    gabi::Local<ChainColor> quat;
    u32 mode = gabi::at<ChainPassHD>(pass)->mMode.get(), shader = 0;
    if ((s32)mode < PASS_COUNT) {
        u32 count = this->mShaderCapacity.get(), entries = this->mpShaderEntries.get();
        if (mode < count)
            entries += mode * 20;
        shader = gabi::at<ChainShaderEntryHD>(entries)->mpShader.get();
    }
    u32 graphics = chain_getGraphicsContext(gabi::ea(&l_graphics));
    u32 program = gabi::at<ChainShaderHD>(shader)->mpProgram.get(),
        current = gabi::at<ChainGraphicsHD>(graphics)->mpCurrentProgram.get();
    if (program != current) {
        u8 flags = gabi::at<ChainProgramHD>(program)->mFlags.get();
        u32 oldBinary = gabi::at<ChainGraphicsHD>(graphics)->mpCurrentBinary.get();
        if (flags & PROGRAM_DIRTY) {
            gabi::at<ChainProgramHD>(program)->mFlags = flags & ~u32(PROGRAM_DIRTY);
            chain_updateShaderProgram(program, 0);
        }
        u32 resource = gabi::at<ChainProgramHD>(program)->mpResource.get();
        u32 binary = gabi::at<ChainProgramResourceHD>(resource)->mpBinary.get();
        if (oldBinary != binary)
            chain_bindShaderBinary(binary);
        u32 size = gabi::at<ChainProgramHD>(program)->mInstructionSize.get();
        if (size) {
            chain_bindShaderInstructions(gabi::at<ChainProgramHD>(program)->mpInstructions.get(),
                                         size);
            gabi::at<ChainGraphicsHD>(graphics)->mpCurrentBinary = binary;
            gabi::at<ChainGraphicsHD>(graphics)->mpCurrentProgram = program;
        } else {
            chain_activateShaderProgram(program);
            gabi::at<ChainGraphicsHD>(graphics)->mpCurrentProgram = program;
            gabi::at<ChainGraphicsHD>(graphics)->mpCurrentBinary = binary;
        }
    }
    mode = gabi::at<ChainPassHD>(pass)->mMode.get();
    if (mode == PASS_SHAPE) {
        u32 shape = gabi::at<ChainPassHD>(pass)->mpShape.get();
        if (shape)
            chain_BindAttributes(shader, gabi::at<ChainShapeHD>(shape)->mLayoutWords[1].get());
    } else if (mode == PASS_UNIFORM || mode == PASS_UNIFORM_ADDITIONAL) {
        chain_BindAttributes(shader, this->mpPackets.get());
        u32 packet = self + 0xA8C8;
        u32 table = gabi::at<ChainDrawPacketHD>(packet)->mVtable.get();
        gabi::call_ptr<void>(gabi::at<ChainPacketVtableHD>(table)->mBindShader.get(), packet,
                             shader);
        if (mode == PASS_UNIFORM_ADDITIONAL) {
            u32 additional = gabi::at<ChainPassHD>(pass)->mpAdditionalPacket.get();
            if (additional) {
                table = gabi::at<ChainDrawPacketHD>(additional)->mVtable.get();
                gabi::call_ptr<void>(gabi::at<ChainPacketVtableHD>(table)->mBindShader.get(),
                                     additional, shader);
            }
        }
        u32 data = gabi::at<ChainShaderHD>(shader)->mUniformCount.get()
                       ? gabi::at<ChainShaderHD>(shader)->mpUniforms.get()
                       : 0;
        chain_bindShape(self + 0xB104, data + 4, -1, 0);
        if (mode == PASS_UNIFORM_ADDITIONAL)
            chain_bindAdditionalPass(pass, shader);
    }
    chain_renderStateRead(state.a);
    u32 word = state->mBlendFlags.get();
    state->mCullMode = 2;
    state->mDepthTest = 1;
    state->mBlendFlags = ((((word & 0xFFFFFFF0u) + 7) & 0xFFFFFF0Fu) + 16);
    chain_configurePass(gabi::at<ChainPassHD>(pass)->mMode.get(), state.a);
    chain_renderStateApply(state.a);
    u32 count = this->mNum.get(), points = this->mPt.get();
    f32 x = gabi::at<ChainVector>(points)->x.get(), adjustment = l_segmentAdjustment;
    f32 base = l_segmentBase;
    pos->x = x;
    f32 y = gabi::at<ChainVector>(points)->y.get();
    f32 segment = (adjustment + base) * this->mScale.z.get();
    pos->y = y;
    pos->z = gabi::at<ChainVector>(points)->z.get();
    u32 index = 0, offset = 0;
    if ((s32)index >= (s32)(count - 1))
        return;
    u32 next = points + 12;
    f32 zero = l_zero;
    do {
        chain_vectorSubtract(next, dir.a, pos.a);
        f32 squared = chain_vectorMagnitudeSquared(dir.a);
        f32 length = chain_sqrtf(squared);
        if (chain_vectorNormalizeRS(dir.a)) {
            chain_quatRotBaseZ(quat.a, dir.a);
            if (length > zero) {
                u32 packet = self + 0xC8 + offset;
                u32 indices = self + 0xB0EC, resources = self + 0xAC2C;
                do {
                    u32 table = gabi::at<ChainDrawPacketHD>(packet)->mVtable.get();
                    gabi::call_ptr<void>(gabi::at<ChainPacketVtableHD>(table)->mBindShader.get(),
                                         packet, shader);
                    mode = gabi::at<ChainPassHD>(pass)->mMode.get();
                    u32 size = this->mShaderCapacity.get();
                    offset += 0xA8;
                    packet += 0xA8;
                    u32 entries = this->mpShaderEntries.get();
                    if (mode < size)
                        entries += mode * 20;
                    u32 selector = gabi::at<ChainResourcesHD>(resources)->mActive.get() ? 0 : 8;
                    chain_bindUniformBlock(
                        gabi::at<ChainBindingArrayHD>(entries + selector + 4)->mpData.get());
                    u32 number = gabi::at<ChainIndexBufferHD>(indices)->mNumIndices.get();
                    if (number)
                        chain_drawIndexed(
                            gabi::at<ChainIndexBufferHD>(indices)->mIndexType.get(), number,
                            gabi::at<ChainIndexBufferHD>(indices)->mPrimitive.get(),
                            gabi::at<ChainIndexBufferHD>(indices)->mpIndices.get(), 0, 1);
                    length -= segment;
                    chain_vectorScale(dir.a, step.a, segment);
                    chain_vectorAdd(pos.a, step.a, pos.a);
                } while (length > zero);
            }
        }
        count = this->mNum.get();
        ++index;
        next += 12;
    } while ((s32)index < (s32)(count - 1));
}

/* 0251AD2C */
void chain_MatrixCopy(u32 dest, u32 source) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = gabi::at<ChainMatrix>(source)->m[i].get();
    for (u32 i = 0; i < 12; ++i)
        gabi::at<ChainMatrix>(dest)->m[i] = values[i];
}

/* 0251ADCC */
void chain_ColorToFloat(u32 dest, u32 source) {
    auto* sourceColor = gabi::at<ChainColorS10HD>(source);
    auto* destColor = gabi::at<ChainColor>(dest);
    f32 r = (f32)sourceColor->r.get();
    f32 b = (f32)sourceColor->b.get();
    f32 g = (f32)sourceColor->g.get();
    f32 a = (f32)sourceColor->a.get();
    f32 denominator = l_colorDenominator;
    destColor->r = r / denominator;
    destColor->g = g / denominator;
    destColor->b = b / denominator;
    destColor->a = a / denominator;
}

// HD: compiler-generated initialization and inlined SDK helpers.
/* 0251B304 */
void chain_StaticInit() {
    l_staticWord2 = 0;
    l_staticWord0 = 0;
    l_staticWord3 = 0;
    l_staticWord1 = 0;
    chain_registerStaticDestructor(gabi::ea(&l_staticDestructor0));
    u32 lower = l_staticLower, upper = l_staticUpper;
    l_staticRange = lower;
    l_staticRangeEnd = upper;
    chain_initializeStaticAngle(gabi::ea(&l_staticAngle));
    chain_registerStaticDestructor(gabi::ea(&l_staticDestructor1));
    chain_initializeStaticFlag(gabi::ea(&l_staticFlag));
    chain_registerStaticDestructor(gabi::ea(&l_staticDestructor2));
}

/* 0251B398 */
void chain_InlineDelete(u32 self, u32 flags) {
    if (self && (flags & DELETE_STORAGE))
        chain_operatorDelete(self);
}

/* 0251B3AC */
u32 chain_InlinePacketCtor(u32 self) {
    if (!self)
        self = chain_operatorNew(0xA8);
    if (self) {
        chain_drawPacketConstruct(self);
        gabi::at<ChainDrawPacketHD>(self)->mVtable = gabi::ea(&l_segmentPacketVtable);
        chain_clearMemory(self + 0x74, 0x34);
        if (self + 0x74 == 0)
            chain_operatorNew(0x30);
    }
    return self;
}

/* 0251B41C */
u32 chain_InlineColorCtor(u32 self) { return self ? self : chain_operatorNew(0x10); }

/* 0251B448 */
u32 chain_InlineResourceCtor(u32 self) {
    if (!self)
        self = chain_operatorNew(0x254);
    if (self) {
        chain_vertexBufferConstruct(self + 4);
        chain_shaderBlockConstruct(self + 0x158);
        gabi::at<ChainResourceHD>(self)->mpAllocation = 0;
        gabi::at<ChainResourceHD>(self)->mCount = 0;
    }
    return self;
}

/* 0251B4A4 */
void chain_InlineResourceDtor(u32 self, u32 flags) {
    if (self) {
        chain_shaderBlockDestroy(self + 0x158, 2);
        chain_vertexBufferDestroy(self + 4, 2);
        if (flags & DELETE_STORAGE)
            chain_operatorDelete(self);
    }
}

/* 0251B504 */
void chain_InlinePacketDtor(u32 self, u32 flags) {
    if (self) {
        chain_drawPacketDestroy(self, 0);
        if (flags & DELETE_STORAGE)
            chain_operatorDelete(self);
    }
}

/* 0251B558 */
void chain_InlineEmpty0(u32 self) {}
/* 0251B55C */
void chain_InlineEmpty1(u32 self) {}

/* WWHD glue
data l_heapManager 101F8B4C
data l_indexData 101D5828
data l_vertexCount 101D5824
data l_graphicsContext 101F8B18
data l_resourceManager 101F4F7C
data l_staticLower 1004B4FC
data l_staticUpper 1004B500
data l_packetZero 10145180
data l_packetOne 1014517C
data l_segmentAdjustment 1047BBDC
data l_segmentBase 1004B4DC
data l_zero 1004B4E0
data l_colorDenominator 1004B4E4
data l_safeStringVtable 1004B484
data l_shapeName 1004B49C
data l_shaderName 1004B4A8
data l_shapeArchive 1004B4B4
data l_shapeFilename 1004B4C4
data l_vertexFormatName 1004B508
data l_chainVtable 1004B514
data l_segmentPacketVtable 1016EF84
data l_uniformPacketVtable 1016EFB4
data l_positions 101D582C
data l_normals 101D59DC
data l_texCoords 101D5B8C
data l_indices 101D5CAC
data l_staticDestructor0 101D5D3C
data l_staticDestructor1 101D5D48
data l_staticDestructor2 101D5D54
data l_staticRange 1046F038
data l_staticRangeEnd 1046F03C
data l_staticAngle 1046F040
data l_staticFlag 1046F041
data l_staticWord0 1046F044
data l_staticWord1 1046F048
data l_staticWord2 1046F04C
data l_staticWord3 1046F050
data l_matrixStack 1048D0CC
data l_graphics 104B45C0
ext 02519814 u32 chain_constructPacket(u32, u32, u32, f32)
ext 027BF7E8 void chain_releaseShaderBlock(u32)
ext 02755FEC u32 chain_findContainingHeap(u32, u32)
ext 0273AD10 u32 chain_operatorNew(u32)
ext 027F1278 void chain_packetBaseConstruct(u32)
ext 027FD6F4 void chain_packetArrayConstruct(u32)
ext 028EFFD0 u32 chain_constructArray(u32, u32, u32, u32)
ext 027FB40C void chain_drawPacketConstruct(u32)
ext 028F521C void chain_clearMemory(u32, u32)
ext 027B5430 void chain_indexBufferConstruct(u32)
ext 027BDF7C void chain_shapeConstruct(u32)
ext 027BE6B8 void chain_temporaryShapeConstruct(u32)
ext 027FFCBC u32 chain_shaderManagerInstance()
ext 027B90AC s32 chain_findShader(u32, u32)
ext 02800B0C void chain_shaderObjectInitialize(u32, u32, u32)
ext 0280068C void chain_setupShaderEntries(u32, u32, u32)
ext 02756140 u32 chain_currentHeap(u32)
ext 027FF478 void chain_setupVertexBuffer(u32, u32, u32, u32)
ext 027BF734 void chain_shaderBlockConstruct(u32)
ext 027FF530 void chain_setupShaderBlock(u32, u32, u32, u32, u32)
ext 027FD838 void chain_resizePacketArray(u32, u32, u32)
ext 027FB5D4 void chain_drawPacketSetup(u32, u32)
ext 027B54E0 void chain_setupIndexBuffer(u32, u32, u32, u32)
ext 027B5E94 void chain_invalidateVertexBuffer(u32, u32, u32)
ext 0274FBF8 void chain_enterGraphicsContext(u32)
ext 026124B0 u32 chain_loadResource(u32, u32, u32, u32)
ext 02773870 void chain_readShapeResource(u32, u32, u32)
ext 027BDEB4 void chain_copyShape(u32, u32)
ext 0274FCCC void chain_leaveGraphicsContext(u32)
ext 027BEBEC void chain_releasePacketResource(u32)
ext 028F0164 void chain_destroyArray(u32, u32, u32, u32, u32, u32)
ext 027BE2B0 void chain_shapeDestroy(u32, u32)
ext 027B54A0 void chain_indexBufferDestroy(u32, u32)
ext 027FB528 void chain_drawPacketDestroy(u32, u32)
ext 027FD764 void chain_packetArrayDestroy(u32, u32)
ext 027F13DC void chain_packetBaseDestroy(u32, u32)
ext 0273AF40 void chain_operatorDelete(u32)
ext C0006900 void chain_bindNormalBuffer(u32, u32, u32)
ext C0006A38 void chain_bindPositionBuffer(u32, u32, u32)
ext C00068A8 void chain_bindTexCoordBuffer(u32, u32, u32)
ext 027F29D4 u32 chain_getGraphicsContext(u32)
ext 027BB9E0 void chain_updateShaderProgram(u32, u32)
ext 027B9F68 void chain_bindShaderBinary(u32)
ext C00060E0 void chain_bindShaderInstructions(u32, u32)
ext 027BB7CC void chain_activateShaderProgram(u32)
ext 027BE53C void chain_bindShape(u32, u32, u32, u32)
ext 027FFE54 void chain_bindAdditionalPass(u32, u32)
ext 02750250 void chain_renderStateRead(u32)
ext 0280037C void chain_configurePass(u32, u32)
ext 02750370 void chain_renderStateApply(u32)
ext 0201ADE0 void chain_vectorSubtract(u32, u32, u32)
ext 028E8DD0 f32 chain_vectorMagnitudeSquared(u32)
ext 028F4384 f32 chain_sqrtf(f32)
ext 0201B47C BOOL chain_vectorNormalizeRS(u32)
ext 023126A0 void chain_quatRotBaseZ(u32, u32)
ext 027BFE5C void chain_bindUniformBlock(u32)
ext C0006178 void chain_drawIndexed(u32, u32, u32, u32, u32, u32)
ext 0201AE48 void chain_vectorScale(u32, u32, f32)
ext 028E8D88 void chain_vectorAdd(u32, u32, u32)
ext 0251AD2C void chain_copyViewMatrix(u32, u32)
ext 0255F8F4 void chain_setTevFog(u32)
ext 027FDA54 void chain_preparePacketArray(u32, u32, u32, u32, u32)
ext 0251ADCC void chain_colorToFloat(u32, u32)
ext 0274D458 void chain_scaleColor(u32, u32, f32)
ext 027FE010 void chain_updatePacketArray(u32)
ext 0255F84C void chain_resetTevFog()
ext 0274D2AC void chain_scaleAlpha(u32, f32)
ext 027FB678 void chain_updateDrawPacket(u32)
ext 028E93CC void chain_matrixTranslate(u32, f32, f32, f32)
ext 025F25CC void chain_matrixQuatM(u32)
ext 025F1C5C void chain_matrixZrotM(u32, u32)
ext 025F2518 void chain_matrixScaleM(f32, f32, f32)
ext 028E90D4 void chain_matrixConcat(u32, u32)
ext 028F026C void chain_registerStaticDestructor(u32)
ext 028ED6F8 void chain_initializeStaticAngle(u32)
ext 028EAB2C void chain_initializeStaticFlag(u32)
ext 027B5BD8 void chain_vertexBufferConstruct(u32)
ext 027BF880 void chain_shaderBlockDestroy(u32, u32)
ext 027B5CBC void chain_vertexBufferDestroy(u32, u32)
*/
