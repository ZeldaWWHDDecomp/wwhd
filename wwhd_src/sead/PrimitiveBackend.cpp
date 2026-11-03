// WWHD primitive backend setup, reconstructed from the local game's instructions.
#include "sead/PrimitiveBackend.h"

static u32 sPrimitiveBackendTable;
void* allocateBackendObject(u32 size);

/* 0275181C */
sead::PrimitiveBackend* sead::PrimitiveBackend::create(PrimitiveBackend* object) {
    if (object == NULL) object = (PrimitiveBackend*)allocateBackendObject(sizeof(PrimitiveBackend));
    if (object == NULL) return object;
    object->mTable = gabi::ea(&sPrimitiveBackendTable);
    // GHS emitted null-allocation checks for the two in-place matrix subobjects.
    if (gabi::ea(&object->mModel) == 0) allocateBackendObject(sizeof(RenderMatrix34));
    if (gabi::ea(&object->mProjection) == 0) allocateBackendObject(sizeof(RenderMatrix44));
    return object;
}

static sead::StreamTemplate_l sStreamTemplate;
struct LineIndexTemplate_l { be<u16> mIndices[4]; };
static LineIndexTemplate_l sLineIndexTemplate;

u32 vertexShaderObjectSize(u32 index, const void* archive);
u32 vertexShaderProgramSize(u32 index, const void* archive);
u32 pixelShaderObjectSize(u32 index, const void* archive);
u32 pixelShaderProgramSize(u32 index, const void* archive);
void loadVertexShader(sead::VertexShader_l* shader, void* program, u32 index, const void* archive);
void loadPixelShader(sead::PixelShader_l* shader, void* program, u32 index, const void* archive);
void invalidatePrimitiveBuffer(u32 flags, void* buffer, u32 size);
u32 fetchShaderSize(u32 streams, u32 type, u32 tessellation);
void initializeFetchShader(void* shader, void* program, u32 count, sead::VertexStream_l* streams, u32 type, u32 tessellation);
void generatePrimitiveQuad(void* vertices, void* indices);
void generatePrimitiveBox(void* vertices, void* indices);
void generatePrimitiveCube(void* vertices, void* indices);
void generatePrimitiveCylinder(void* vertices, void* indices);
void generatePrimitiveSphere(void* vertices, void* indices, u32 rings, u32 sides);
void generatePrimitiveDisk(void* vertices, void* indices, u32 sides);
void generatePrimitiveHemisphere(void* vertices, void* indices, u32 sides);
void copyPrimitiveIndices(void* destination, const void* source, u32 size);
void initializePrimitiveSampler(void* sampler, u32 clamp, u32 filter);

static bool primitiveShaderNameMatches(const be<u8>* text, const char* name) {
    u32 offset = 0;
    while (true) {
        u32 actual = text[offset];
        u32 expected = (u8)name[offset];
        if (actual != expected) return false;
        if (actual == 0) return true;
        ++offset;
    }
}

static u32 findPrimitiveUniform(u32 count, sead::ShaderUniform_l* uniforms, const char* name) {
    for (u32 index = 0; index < count; ++index) {
        sead::ShaderUniform_l* uniform = &uniforms[index];
        if (primitiveShaderNameMatches(uniform->mName, name)) {
            if (uniform != NULL) return uniform->mLocation;
            return ~0u;
        }
    }
    return ~0u;
}

static u32 findPrimitiveSampler(u32 count, sead::ShaderSampler_l* samplers, const char* name) {
    for (u32 index = 0; index < count; ++index) {
        sead::ShaderSampler_l* sampler = &samplers[index];
        if (primitiveShaderNameMatches(sampler->mName, name)) {
            if (sampler != NULL) return sampler->mLocation;
            return ~0u;
        }
    }
    return ~0u;
}

static u32 findPrimitiveAttribute(u32 count, sead::ShaderAttribute_l* attributes, const char* name) {
    for (u32 index = 0; index < count; ++index) {
        sead::ShaderAttribute_l* attribute = &attributes[index];
        if (primitiveShaderNameMatches(attribute->mName, name)) {
            if (attribute != NULL) return attribute->mLocation;
            return ~0u;
        }
    }
    return ~0u;
}

/* 02751888 */
void sead::PrimitiveBackend::initialize(AllocationHeap* heap, const void* shaderArchive) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto resources = reinterpret_cast<PrimitiveShaderResources_l*>(mShaderResources);
    auto samplerStorage = reinterpret_cast<PrimitiveSamplerStorage_l*>(mSampler);
    auto basicMeshes = reinterpret_cast<PrimitiveBasicMeshes_l*>(mFilledMeshState);
    auto roundMeshes = reinterpret_cast<PrimitiveRoundMeshes_l*>(mWireMeshState);
    auto largeHemisphere = reinterpret_cast<PrimitiveGeometry_l*>(mTrailingStorage);
    u32 objectSize = vertexShaderObjectSize(0, shaderArchive);
    u32 programSize = vertexShaderProgramSize(0, shaderArchive);
    mVertexShader = (VertexShader_l*)gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, objectSize, 1);
    void* program = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, programSize, 0x100);
    loadVertexShader((VertexShader_l*)mVertexShader.get(), program, 0, shaderArchive);
    VertexShader_l* vertex = (VertexShader_l*)mVertexShader.get();
    invalidatePrimitiveBuffer(0x48, vertex->mProgram, vertex->mProgramSize);
    objectSize = pixelShaderObjectSize(0, shaderArchive);
    programSize = pixelShaderProgramSize(0, shaderArchive);
    mPixelShader = (PixelShader_l*)gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, objectSize, 1);
    program = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, programSize, 0x100);
    loadPixelShader((PixelShader_l*)mPixelShader.get(), program, 0, shaderArchive);
    PixelShader_l* pixel = (PixelShader_l*)mPixelShader.get();
    invalidatePrimitiveBuffer(0x48, pixel->mProgram, pixel->mProgramSize);
    vertex = (VertexShader_l*)mVertexShader.get();
    mProjectionRegister = findPrimitiveUniform(vertex->mUniformCount, vertex->mUniforms, "wvp");
    vertex = (VertexShader_l*)mVertexShader.get();
    mModelRegister = findPrimitiveUniform(vertex->mUniformCount, vertex->mUniforms, "user");
    vertex = (VertexShader_l*)mVertexShader.get();
    mTopColorRegister = findPrimitiveUniform(vertex->mUniformCount, vertex->mUniforms, "color0");
    vertex = (VertexShader_l*)mVertexShader.get();
    mBottomColorRegister = findPrimitiveUniform(vertex->mUniformCount, vertex->mUniforms, "color1");
    pixel = (PixelShader_l*)mPixelShader.get();
    mTextureRegister = findPrimitiveUniform(pixel->mUniformCount, pixel->mUniforms, "rate");
    pixel = (PixelShader_l*)mPixelShader.get();
    mTextureSlot = findPrimitiveSampler(pixel->mSamplerCount, pixel->mSamplers, "texture0");
    vertex = (VertexShader_l*)mVertexShader.get();
    resources->mPositionAttribute = findPrimitiveAttribute(vertex->mAttributeCount, vertex->mAttributes, "Vertex");
    vertex = (VertexShader_l*)mVertexShader.get();
    resources->mTexCoordAttribute = findPrimitiveAttribute(vertex->mAttributeCount, vertex->mAttributes, "TexCoord0");
    vertex = (VertexShader_l*)mVertexShader.get();
    resources->mColorAttribute = findPrimitiveAttribute(vertex->mAttributeCount, vertex->mAttributes, "ColorRate");

    gabi::FrameLocal<StreamTemplate_l> streamDefaults(8);
    for (u32 word = 0; word < 20; ++word) streamDefaults->mWords[word] = (u32)sStreamTemplate.mWords[word];
    resources->mStreams[0].mLocation = resources->mPositionAttribute;
    resources->mStreams[0].mIndexType = 0;
    resources->mStreams[0].mFormat = 0x811;
    resources->mStreams[0].mEndian = 3;
    resources->mStreams[0].mDivisor = 0;
    resources->mStreams[0].mComponentMask = (u32)streamDefaults->mWords[17];
    resources->mStreams[0].mOffset = 0;
    resources->mStreams[0].mBuffer = 0;
    for (u32 word = 0; word < 20; ++word) streamDefaults->mWords[word] = (u32)sStreamTemplate.mWords[word];
    resources->mStreams[1].mLocation = resources->mTexCoordAttribute;
    resources->mStreams[1].mIndexType = 0;
    resources->mStreams[1].mEndian = 3;
    resources->mStreams[1].mComponentMask = (u32)streamDefaults->mWords[13];
    resources->mStreams[1].mOffset = 12;
    resources->mStreams[1].mBuffer = 0;
    resources->mStreams[1].mFormat = 0x80D;
    resources->mStreams[1].mDivisor = 0;
    for (u32 word = 0; word < 20; ++word) streamDefaults->mWords[word] = (u32)sStreamTemplate.mWords[word];
    resources->mStreams[2].mLocation = resources->mColorAttribute;
    resources->mStreams[2].mFormat = 0x813;
    resources->mStreams[2].mOffset = 20;
    resources->mStreams[2].mEndian = 3;
    resources->mStreams[2].mDivisor = 0;
    resources->mStreams[2].mComponentMask = (u32)streamDefaults->mWords[19];
    resources->mStreams[2].mIndexType = 0;
    resources->mStreams[2].mBuffer = 0;
    u32 fetchSize = fetchShaderSize(3, 0, 0);
    resources->mFetchProgram = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, fetchSize, 0x100);
    initializeFetchShader(&mFetchShader, resources->mFetchProgram, 3, resources->mStreams, 0, 0);
    fetchSize = fetchShaderSize(3, 0, 0);
    invalidatePrimitiveBuffer(0x48, resources->mFetchProgram, fetchSize);

    samplerStorage->mQuad.mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x90, 0x40);
    samplerStorage->mQuad.mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0xC, 0x20);
    generatePrimitiveQuad(samplerStorage->mQuad.mVertices, samplerStorage->mQuad.mIndices);
    PrimitiveVertex_l* vertices = (PrimitiveVertex_l*)samplerStorage->mQuad.mVertices.get();
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;
    vertices = (PrimitiveVertex_l*)samplerStorage->mQuad.mVertices.get();
    vertices[1].v = 0.0f;
    vertices[1].u = 1.0f;
    vertices = (PrimitiveVertex_l*)samplerStorage->mQuad.mVertices.get();
    vertices[2].u = 0.0f;
    vertices[2].v = 1.0f;
    vertices = (PrimitiveVertex_l*)samplerStorage->mQuad.mVertices.get();
    vertices[3].v = 1.0f;
    vertices[3].u = 1.0f;
    invalidatePrimitiveBuffer(0x41, samplerStorage->mQuad.mVertices, 0x90);
    invalidatePrimitiveBuffer(0x41, samplerStorage->mQuad.mIndices, 0xC);
    samplerStorage->mLineIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 8, 0x20);
    copyPrimitiveIndices(samplerStorage->mLineIndices, &sLineIndexTemplate, 8);
    invalidatePrimitiveBuffer(0x41, samplerStorage->mLineIndices, 8);
    basicMeshes->mBox.mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x90, 0x40);
    basicMeshes->mBox.mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0xC, 0x20);
    generatePrimitiveBox(basicMeshes->mBox.mVertices, basicMeshes->mBox.mIndices);
    invalidatePrimitiveBuffer(0x41, basicMeshes->mBox.mVertices, 0x90);
    invalidatePrimitiveBuffer(0x41, basicMeshes->mBox.mIndices, 0xC);
    basicMeshes->mCube.mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x120, 0x40);
    basicMeshes->mCube.mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x48, 0x20);
    generatePrimitiveCube(basicMeshes->mCube.mVertices, basicMeshes->mCube.mIndices);
    invalidatePrimitiveBuffer(0x41, basicMeshes->mCube.mVertices, 0x120);
    invalidatePrimitiveBuffer(0x41, basicMeshes->mCube.mIndices, 0x48);
    basicMeshes->mCylinder.mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x120, 0x40);
    basicMeshes->mCylinder.mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x22, 0x20);
    generatePrimitiveCylinder(basicMeshes->mCylinder.mVertices, basicMeshes->mCylinder.mIndices);
    invalidatePrimitiveBuffer(0x41, basicMeshes->mCylinder.mVertices, 0x120);
    invalidatePrimitiveBuffer(0x41, basicMeshes->mCylinder.mIndices, 0x22);
    mFilledVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x4C8, 0x40);
    mFilledIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x180, 0x20);
    generatePrimitiveSphere(mFilledVertices, mFilledIndices, 8, 4);
    invalidatePrimitiveBuffer(0x41, mFilledVertices, 0x4C8);
    invalidatePrimitiveBuffer(0x41, mFilledIndices, 0x180);
    roundMeshes->mSphereLarge.mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x1248, 0x40);
    roundMeshes->mSphereLarge.mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x600, 0x20);
    generatePrimitiveSphere(roundMeshes->mSphereLarge.mVertices, roundMeshes->mSphereLarge.mIndices, 16, 8);
    invalidatePrimitiveBuffer(0x41, roundMeshes->mSphereLarge.mVertices, 0x1248);
    invalidatePrimitiveBuffer(0x41, roundMeshes->mSphereLarge.mIndices, 0x600);
    roundMeshes->mDiskSmall.mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x264, 0x40);
    roundMeshes->mDiskSmall.mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x60, 0x20);
    generatePrimitiveDisk(roundMeshes->mDiskSmall.mVertices, roundMeshes->mDiskSmall.mIndices, 16);
    invalidatePrimitiveBuffer(0x41, roundMeshes->mDiskSmall.mVertices, 0x264);
    invalidatePrimitiveBuffer(0x41, roundMeshes->mDiskSmall.mIndices, 0x60);
    roundMeshes->mDiskLarge.mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x4A4, 0x40);
    roundMeshes->mDiskLarge.mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0xC0, 0x20);
    generatePrimitiveDisk(roundMeshes->mDiskLarge.mVertices, roundMeshes->mDiskLarge.mIndices, 32);
    invalidatePrimitiveBuffer(0x41, roundMeshes->mDiskLarge.mVertices, 0x4A4);
    invalidatePrimitiveBuffer(0x41, roundMeshes->mDiskLarge.mIndices, 0xC0);
    roundMeshes->mRingSmallIndices = (be<u16>*)gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 32, 0x20);
    for (u32 index = 0; index < 16; ++index) roundMeshes->mRingSmallIndices[index] = (u16)index;
    invalidatePrimitiveBuffer(0x41, roundMeshes->mRingSmallIndices, 32);
    roundMeshes->mRingLargeIndices = (be<u16>*)gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 64, 0x20);
    for (u32 index = 0; index < 32; ++index) roundMeshes->mRingLargeIndices[index] = (u16)index;
    invalidatePrimitiveBuffer(0x41, roundMeshes->mRingLargeIndices, 64);
    mWireVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x4C8, 0x40);
    mWireIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x180, 0x20);
    generatePrimitiveHemisphere(mWireVertices, mWireIndices, 16);
    invalidatePrimitiveBuffer(0x41, mWireVertices, 0x4C8);
    invalidatePrimitiveBuffer(0x41, mWireIndices, 0x180);
    largeHemisphere->mVertices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x948, 0x40);
    largeHemisphere->mIndices = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, 0x300, 0x20);
    generatePrimitiveHemisphere(largeHemisphere->mVertices, largeHemisphere->mIndices, 32);
    invalidatePrimitiveBuffer(0x41, largeHemisphere->mVertices, 0x948);
    invalidatePrimitiveBuffer(0x41, largeHemisphere->mIndices, 0x300);
    initializePrimitiveSampler(mSampler, 2, 1);
}
/* WWHD glue
 data sPrimitiveBackendTable 10145890
 ext 0273AD10 void* allocateBackendObject(u32)
 data sStreamTemplate 101457DC
 data sLineIndexTemplate 10145834
 str 1014583C "wvp"
 str 10145840 "user"
 str 10145848 "color0"
 str 10145850 "color1"
 str 10145858 "rate"
 str 10145880 "texture0"
 str 10145860 "Vertex"
 str 10145868 "TexCoord0"
 str 10145874 "ColorRate"
 ext 028E8518 u32 vertexShaderObjectSize(u32,const void*)
 ext 028E8538 u32 vertexShaderProgramSize(u32,const void*)
 ext 028E8528 u32 pixelShaderObjectSize(u32,const void*)
 ext 028E8548 u32 pixelShaderProgramSize(u32,const void*)
 ext 028E8624 void loadVertexShader(sead::VertexShader_l*,void*,u32,const void*)
 ext 028E87A8 void loadPixelShader(sead::PixelShader_l*,void*,u32,const void*)
 ext 02768AA8 void generatePrimitiveQuad(void*,void*)
 ext 027694B0 void generatePrimitiveBox(void*,void*)
 ext 02768BD4 void generatePrimitiveCube(void*,void*)
 ext 02768DA0 void generatePrimitiveCylinder(void*,void*)
 ext 02768DF0 void generatePrimitiveSphere(void*,void*,u32,u32)
 ext 027692BC void generatePrimitiveDisk(void*,void*,u32)
 ext 02769594 void generatePrimitiveHemisphere(void*,void*,u32)
 ext C00064A8 void invalidatePrimitiveBuffer(u32,void*,u32)
 ext C00060B8 u32 fetchShaderSize(u32,u32,u32)
 ext C0006400 void initializeFetchShader(void*,void*,u32,sead::VertexStream_l*,u32,u32)
 ext C0006438 void initializePrimitiveSampler(void*,u32,u32)
 ext C000A848 void copyPrimitiveIndices(void*,const void*,u32)
*/
