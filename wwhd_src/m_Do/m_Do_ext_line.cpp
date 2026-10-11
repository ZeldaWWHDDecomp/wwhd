/**
 * 3D line geometry, lighting and untextured material, following zeldaret/tww m_Do_ext names.
 * HD uses shader packets, mesh buffers and normal attributes in place of GX
 * arrays. Verified against WWHD by m_Do_ext_11. Private game-derived source.
 */
#include "m_Do/m_Do_ext_line.h"
using namespace line_mat0;
static ShaderNames l_shaderNames;
static SafeString l_packetName;
static u32 l_shaderNamesInitialized;
static u32 l_packetNameInitialized;
static DestructorRecord l_shaderNamesDestructor;
static DestructorRecord l_packetNameDestructor;
static ResourceControl* l_resourceControl;
static J3DSysView l_j3dSys;
static DrawPrimitive l_drawPrimitive;
static f32 l_rendererZero;
static f32 l_rendererOne;
// HD: per-TU vtables and compiler array callbacks.
static constexpr u32 l_materialVtable = 0x10058DC0;
static constexpr u32 l_safeStringVtable = 0x10058704;
static constexpr u32 l_primaryVtable = 0x1016EF84;
static constexpr u32 l_secondaryVtable = 0x1016EFB4;
enum {
    LINE_ARRAY_CONSTRUCTOR = 0x025E8F58,
    LINE_ARRAY_DESTRUCTOR = 0x025E91C8,
    SHADER_ARRAY_CONSTRUCTOR = 0x025EE4E8,
    SHADER_ARRAY_DESTRUCTOR = 0x025EE34C,
    PARAMETER_ARRAY_CONSTRUCTOR = 0x025EE558,
};

static f32 l_lineZero;
static f32 l_lineOne;
static f32 l_tangentPositive;
static f32 l_tangentNegative;
static f32 l_colorDivisor;
static f32 l_threshold;
static HeapRegistry* l_heapRegistry;
static Mtx34 l_viewMatrix;
// HD: deleting destructors install the translation unit's own vtables.
static constexpr u32 l_lineVtable = 0x10058E50;
static constexpr u32 l_baseVtable = 0x1005876C;

// HD: bind the three shader attributes in their original normal/position/UV
// order.
static void bindPacketAttributes(RendererRecord* entry, DrawPacket* packet) {
    GraphicsBuffer* record = &entry->mBuffers[entry->mCurArr];
    ShaderInfo* info = packet->getShaderInfo();
    s32 position = info->mPositionIndex;
    s32 normal = info->mNormalIndex;
    s32 texCoord = info->mTexCoordIndex;
    if (position != -1 || normal != -1 || texCoord != -1) {
        void* vertices = record->mpVertices;
        void* data = record->mpData;
        if (normal != -1)
            texturedLineBindNormal(normal, data, vertices);
        if (position != -1)
            texturedLineBindPosition(position, data, vertices);
        if (texCoord != -1)
            texturedLineBindTexCoord(texCoord, data, vertices);
    }
}

/* 025E8FA8 */
u32 line_mat0::mDoExt_3Dline_c::init(u32 count, u32 optional) {
    mpSegments = (cXyz*)texturedLineAllocArray(nullptr, count, sizeof(cXyz), 0);
    if (!mpSegments)
        return 0;
    if (optional) {
        mpSize = (be<u8>*)lineAllocateBytes(count);
        if (!mpSize)
            return 0;
    } else {
        mpSize = nullptr;
    }
    MeshBuffer* mesh = lineCreateMesh(0);
    u32 doubled = count * 2;
    mpMeshBuffer = mesh;
    if (mesh == nullptr)
        return 0;
    lineAllocateMesh(mesh, 1, doubled, 0);
    mesh = mpMeshBuffer;
    VertexTable* table = mesh->getVertices();
    if ((s32)doubled > 0) {
        for (u32 i = 0; i < doubled; i++) {
            Vertex* vertex = table->getVertex(i);
            vertex->mTangent.z = l_lineZero;
            vertex->mTangent.y = l_lineZero;
            vertex = table->getVertex(i);
            vertex->mTexX = (i & 1) ? l_lineZero : l_lineOne;
            vertex = table->getVertex(i);
            vertex->mTangent.x = (i & 1) ? l_tangentNegative : l_tangentPositive;
            vertex = table->getVertex(i);
            vertex->mTangent.y = l_lineOne;
            vertex = table->getVertex(i);
            vertex->mTexY = l_lineOne;
        }
        mesh = mpMeshBuffer;
    }
    lineUploadMesh(mesh);
    return 1;
}

/* 025E91C8 */
static void mDoExt_3Dline_c_dtor(void* self, u32 flag) {
    auto* line = (line_mat0::mDoExt_3Dline_c*)self;
    if (line == nullptr)
        return;
    line->__vtbl = l_lineVtable;
    cXyz* segments = line->mpSegments;
    if (segments != nullptr) {
        texturedLineDestroyArray(segments, -1, sizeof(cXyz), 0, 1, 0);
        line->mpSegments = nullptr;
    }
    be<u8>* sizes = line->mpSize;
    if (sizes != nullptr) {
        lineFreeBytes(sizes);
        line->mpSize = nullptr;
    }
    MeshBuffer* root = line->mpMeshBuffer;
    if (root != nullptr) {
        for (u32 i = 0; i < (u32)root->mNumBuffers; i++) {
            for (u32 array = 0; array < 2; array++) {
                MeshRecord* entry = root->mBuffers[array].getEntry(i);
                lineReleaseBuffer(&entry->mBuffer);
                entry = root->mBuffers[array].getEntry(i);
                Vertex* buffer = entry->mVertices.mpVertices;
                if (buffer != nullptr) {
                    HeapManager* manager = lineFindHeap(l_heapRegistry, buffer);
                    manager->free(entry->mVertices.mpVertices);
                    entry->mVertices.mCount = 0;
                    entry->mVertices.mpVertices = nullptr;
                }
            }
        }
        for (u32 array = 0; array < 2; array++) {
            MeshRecord* base = root->mBuffers[array].mpEntries;
            if (base != nullptr) {
                u32 i = 0;
                u32 count = root->mBuffers[array].mCount;
                while ((s32)i < (s32)count) {
                    MeshRecord* entry = &base[i];
                    // HD: test the wrapped guest address, including a wrapped NULL entry.
                    if (gabi::ea(entry) != 0) {
                        lineBuffer_dt(&entry->mBuffer, 2);
                        lineResource_dt(&entry->mResource, 2);
                        count = root->mBuffers[array].mCount;
                        base = root->mBuffers[array].mpEntries;
                    }
                    i++;
                }
                HeapManager* manager = lineFindHeap(l_heapRegistry, base);
                manager->free(root->mBuffers[array].mpEntries);
                root->mBuffers[array].mCount = 0;
                root->mBuffers[array].mpEntries = nullptr;
            }
        }
        root->mReady = 0;
        root->mNumBuffers = 0;
        root->mCapacity = 0;
        lineMesh_dt(line->mpMeshBuffer, 3);
        line->mpMeshBuffer = nullptr;
    }
    if (flag & 1)
        operator_delete(self);
}

/* 025E947C */
static void* mDoExt_3DlineMat_c_ct(void* self) {
    auto* material = (line_mat0::mDoExt_3DlineMat0_c*)self;
    if (material == nullptr) {
        material = (line_mat0::mDoExt_3DlineMat0_c*)operator_new(0x134);
        if (material == nullptr)
            return nullptr;
    }
    material->mpRenderer = nullptr;
    material->__vtbl = l_baseVtable;
    material->mpTextures = nullptr;
    material->mpNextLineMat = nullptr;
    material->mpShaders = nullptr;
    lineTexture_ct(&material->mTexObj);
    lineTexture_ct(&material->mToonTexObj);
    return material;
}

/* 025E94EC */
static void mDoExt_3DlineMat_c_dtor(void* self, u32 flag) {
    auto* material = (line_mat0::mDoExt_3DlineMat0_c*)self;
    if (material == nullptr)
        return;
    material->__vtbl = l_baseVtable;
    if (material->mpShaders)
        lineAssert("m_Do_ext.cpp", 0x19C8, "mShaderAssignSet == (0)");
    if (material->mpRenderer)
        lineAssert("m_Do_ext.cpp", 0x19C9, "mUBOSet == (0)");
    if (material->mpTextures)
        lineAssert("m_Do_ext.cpp", 0x19CA, "mSamplerSet == (0)");
    if (flag & 1)
        operator_delete(self);
}

/* 025E95AC */
static void lineColorS10ToFloat(Float4* output, GXColorS10* input) {
    f32 red = (f32)input->r;
    f32 blue = (f32)input->b;
    f32 green = (f32)input->g;
    f32 alpha = (f32)input->a;
    f32 divisor = l_colorDivisor;
    output->values[0] = red / divisor;
    output->values[1] = green / divisor;
    output->values[2] = blue / divisor;
    output->values[3] = alpha / divisor;
}

/* 025E9670 */
static void mDoExt_3DlineMat_c_setColor(void* self, void* lighting, void* color) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* material = (line_mat0::mDoExt_3DlineMat0_c*)self;
    auto* tev = (LightingView*)lighting;
    auto* gxColor = (GXColor*)color;
    RendererManager* root = material->mpRenderer;
    auto* info = (MaterialUniforms*)&root->mTransforms;
    root->mMatrixDirty = 0;
    lineMatrixIdentity(&root->mScratchMatrix);
    f32 zero = l_lineZero;
    f32 one = l_lineOne;
    f32 divisor = l_colorDivisor;
    info->mRows[0].values[1] = one;
    info->mRows[2].values[0] = zero;
    info->mRows[0].values[2] = one;
    info->mRows[0].values[3] = one;
    info->mRows[2].values[1] = zero;
    info->mRows[2].values[2] = zero;
    info->mRows[0].values[0] = one;
    info->mRows[2].values[3] = zero;
    if (tev != nullptr) {
        f32 red = (f32)tev->mColorC0.r;
        f32 green = (f32)tev->mColorC0.g;
        f32 blue = (f32)tev->mColorC0.b;
        f32 alpha = (f32)tev->mColorC0.a;
        info->mColorC0.values[1] = green;
        info->mColorC0.values[0] = red;
        info->mColorC0.values[2] = blue;
        info->mColorC0.values[3] = alpha;
        lineDivideColor(&info->mColorC0, divisor);
        red = (f32)tev->mColorK0.r;
        green = (f32)tev->mColorK0.g;
        blue = (f32)tev->mColorK0.b;
        alpha = (f32)tev->mColorK0.a;
        info->mColorK0.values[0] = red;
        info->mColorK0.values[1] = green;
        info->mColorK0.values[2] = blue;
        info->mColorK0.values[3] = alpha;
        lineScaleColor(&info->mColorK0, tev->mColorK0Scale);
        lineDivideColor(&info->mColorK0, divisor);
    }
    f32 red = (f32)gxColor->r;
    f32 green = (f32)gxColor->g;
    f32 blue = (f32)gxColor->b;
    f32 alpha = (f32)gxColor->a;
    info->mColor.values[0] = red;
    info->mColor.values[1] = green;
    info->mColor.values[2] = blue;
    info->mColor.values[3] = alpha;
    lineDivideColor(&info->mColor, divisor);
    if (tev != nullptr) {
        gabi::Local<LightingWork> work;
        texturedLineSetFog((dKy_tevstr_c*)lighting);
        lineBuildLightingMatrix(&work->matrix, &l_viewMatrix, &l_j3dSys);
        // HD: the matrix helper leaves the lighting source in live GPR5.
        LightingSource* source = gabi::at<LightingSource>(gabi::cpu->r[5]);
        for (u32 i = 0; i < 16; i++)
            work->lights.words[i] = (u32)source->mLights.words[i];
        LightExtra* extra = source->mpExtra;
        lineApplyLighting(material->mpRenderer, 0, &work->matrix, &work->lights, &extra->mExtra);
        RendererRecord* record = material->mpRenderer->mpEntries;
        lineColorS10ToFloat(&work->color, &tev->mColorC0);
        lineMixColor(&record->mAmbient, &work->color, tev->mAmbientScale);
        lineColorS10ToFloat(&work->color, &tev->mAmbientColor);
        lineMixColor(&record->mFog, &work->color, tev->mFogScale);
        texturedLineSetDefaultFog();
    }
    lineCommitRenderer(material->mpRenderer, 0);
}

/* 025E9960 */
static void* mDoExt_3DlineMat0_c_ct(void* self) {
    auto* material = (line_mat0::mDoExt_3DlineMat0_c*)self;
    if (material == nullptr) {
        material = (line_mat0::mDoExt_3DlineMat0_c*)operator_new(sizeof(*material));
        if (material == nullptr)
            return nullptr;
    }
    texturedLineBase_ct(material);
    material->__vtbl = l_materialVtable;
    texturedLineZeroBytes(&material->mColor, sizeof(material->mColor));
    material->mCurArr = 0;
    material->mFlags = 0;
    material->mNumLines = 0;
    material->mNumSegments = 0;
    material->mpLines = nullptr;
    material->mMaxSegments = 0;
    material->mpTevStr = nullptr;
    return material;
}

/* 025E99E0 */
static void mDoExt_3DlineMat0_c_dtor(void* self, u32 flag) {
    auto* material = (line_mat0::mDoExt_3DlineMat0_c*)self;
    if (material == nullptr)
        return;
    material->__vtbl = l_materialVtable;
    line_mat0::mDoExt_3Dline_c* lines = material->mpLines;
    if (lines != nullptr) {
        texturedLineDestroyArray(lines, -1, sizeof(*lines), LINE_ARRAY_DESTRUCTOR, 1, 0);
        material->mpLines = nullptr;
    }
    ShaderBinding* shaders = material->mpShaders;
    if (shaders != nullptr) {
        texturedLineDestroyArray(shaders, -1, sizeof(*shaders), SHADER_ARRAY_DESTRUCTOR, 1, 0);
        material->mpShaders = nullptr;
    }
    RendererManager* root = material->mpRenderer;
    if (root != nullptr) {
        for (u32 i = 0; (s32)i < (s32)(u32)root->mCount; i++) {
            GraphicsBuffer* buffers = root->getEntry(i)->mBuffers;
            texturedLineBufferDestroy(&buffers[0]);
            texturedLineBufferDestroy(&buffers[1]);
        }
        texturedLineBufferDestroy(&root->mPrimary.mBuffers[0]);
        texturedLineBufferDestroy(&root->mPrimary.mBuffers[1]);
        texturedLineBufferDestroy(&root->mSecondary.mBuffers[0]);
        texturedLineBufferDestroy(&root->mSecondary.mBuffers[1]);
        root = material->mpRenderer;
        if (root != nullptr) {
            texturedLineUniform_dt(&root->mSecondary, 0);
            texturedLineUniform_dt(&root->mPrimary, 0);
            texturedLineRenderer_dt(root, 2);
            operator_delete(root);
        }
        material->mpRenderer = nullptr;
    }
    TextureManager* textures = material->mpTextures;
    if (textures != nullptr) {
        texturedLineTextureManager_dt(textures, 3);
        material->mpTextures = nullptr;
    }
    texturedLineBase_dt(self, 0);
    if (flag & 1)
        operator_delete(self);
}

/* 025E9B80 */
u32 line_mat0::mDoExt_3DlineMat0_c::init(u32 count, u32 points, u32 optional) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!l_shaderNamesInitialized) {
        l_shaderNamesInitialized = 1;
        l_shaderNames.names[0].__vtbl = l_safeStringVtable;
        l_shaderNames.names[0].mStringTop = gabi::guest_addr("line_mat");
        l_shaderNames.names[1].__vtbl = l_safeStringVtable;
        l_shaderNames.names[1].mStringTop = gabi::guest_addr("line_toon_mat");
        __register_global_object(gabi::ea(&l_shaderNamesDestructor));
    }
    mNumLines = (u16)count;
    mMaxSegments = (u16)points;
    mpLines = (line_mat0::mDoExt_3Dline_c*)texturedLineAllocArray(
        nullptr, count, sizeof(line_mat0::mDoExt_3Dline_c), LINE_ARRAY_CONSTRUCTOR);
    ShaderBinding* shaders = (ShaderBinding*)texturedLineAllocArray(
        nullptr, count, sizeof(ShaderBinding), SHADER_ARRAY_CONSTRUCTOR);
    mpShaders = shaders;
    if (!mpLines || shaders == nullptr)
        return 0;
    if ((s32)count > 0) {
        for (u32 i = 0; i < count; i++) {
            ShaderRegistry* registry = texturedLineGetShaderRegistry();
            u32 id = texturedLineFindShader(registry->mpArchive, &l_shaderNames.names[1]);
            ShaderResource* resource = nullptr;
            if ((s32)id >= 0) {
                u32 capacity = registry->mCount;
                ShaderResource* resources = registry->mpResources;
                ShaderResource* entry = id < capacity ? &resources[id] : resources;
                if (!entry->mInitialized) {
                    ShaderArchive* owner = registry->mpArchive;
                    u32 total = owner->mCount;
                    ShaderArchiveEntry* extra = nullptr;
                    entry = id < capacity ? &resources[id] : resources;
                    if (id < total)
                        extra = &owner->mpEntries[id];
                    texturedLineInitializeShader(entry, extra, 0);
                    capacity = registry->mCount;
                    resources = registry->mpResources;
                }
                resource = id < capacity ? &resources[id] : resources;
            }
            texturedLineBindShader(&mpShaders[i], resource, 0);
            if (!texturedLineInitGeometry(&mpLines[i], points, optional, 0))
                return 0;
            texturedLineInitMesh(mpLines[i].mpMeshBuffer, 0, 1, 0);
            ShaderBinding* table = &mpShaders[i];
            MeshBuffer* buffer = mpLines[i].mpMeshBuffer;
            for (u32 j = 0; j < (u32)table->mCount; j++) {
                ShaderBindingEntry* entry = table->getEntry(j);
                texturedLineBindMesh(entry, entry->mpPacket, buffer, 0);
            }
        }
    }
    mpNextLineMat = nullptr;
    mCurArr = 0;
    RendererManager* root = (RendererManager*)operator_new(sizeof(RendererManager));
    if (root != nullptr) {
        texturedLineRenderer_ct(root);
        texturedLineUniform_ct(&root->mPrimary);
        root->mPrimary.__vtbl = (RenderVtable*)gabi::at<void>(l_primaryVtable);
        texturedLineZeroBytes(&root->mScratchMatrix,
                              sizeof(root->mScratchMatrix) + sizeof(root->mMatrixDirty));
        // HD: preserve the nullable embedded constructors, including address
        // wrapping.
        if (gabi::ea(&root->mScratchMatrix) == 0)
            operator_new(sizeof(Mtx34));
        texturedLineUniform_ct(&root->mSecondary);
        root->mSecondary.__vtbl = (RenderVtable*)gabi::at<void>(l_secondaryVtable);
        texturedLineZeroBytes(&root->mTransforms, sizeof(root->mTransforms));
        root->mTransforms.mBaseMatrices[0].m[0][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[0][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[0][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[1][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[2][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[1].m[0][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[1][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[0].m[1][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[0][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[2][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[2][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[1][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[1].m[0][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[2][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[2][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[1][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[1][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[2][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[0][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[0][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[0].m[2][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[1][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[1][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[1][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[1][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[2][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[1].m[2][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[2].m[0][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[2].m[0][1] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[2][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[2][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[2][2] = l_rendererZero;
        root->mTransforms.mBaseMatrices[2].m[0][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[0].m[0][3] = l_rendererOne;
        root->mTransforms.mBaseMatrices[1].m[1][0] = l_rendererZero;
        root->mTransforms.mBaseMatrices[1].m[1][3] = l_rendererOne;
        root->mTransforms.mDefaultVectors[0].values[0] = l_rendererZero;
        root->mTransforms.mDefaultVectors[0].values[1] = l_rendererZero;
        root->mTransforms.mDefaultVectors[0].values[2] = l_rendererZero;
        root->mTransforms.mDefaultVectors[0].values[3] = l_rendererOne;
        root->mTransforms.mDefaultVectors[1].values[0] = l_rendererZero;
        root->mTransforms.mDefaultVectors[1].values[1] = l_rendererZero;
        root->mTransforms.mDefaultVectors[1].values[2] = l_rendererZero;
        root->mTransforms.mDefaultVectors[1].values[3] = l_rendererOne;
        for (u32 i = 0; i < 3; i++)
            texturedLineAllocArray(root->mTransforms.mParameterPairs[i], 2, sizeof(Float4),
                                   PARAMETER_ARRAY_CONSTRUCTOR);
        for (u32 i = 0; i < 8; i++)
            if (gabi::ea(&root->mTransforms.mExtraMatrices[i]) == 0)
                operator_new(sizeof(Mtx34));
        for (u32 i = 0; i < 6; i++)
            if (gabi::ea(&root->mTransforms.mExtraVectors[i]) == 0)
                operator_new(sizeof(Float4));
        root->mFlags = 0;
    }
    mpRenderer = root;
    texturedLineRendererInit(root, 1, 0);
    TextureManager* textures = texturedLineTextureManager_ct(0);
    mpTextures = textures;
    gabi::Local<ResourceNames> names;
    names->archive.__vtbl = l_safeStringVtable;
    names->resource.__vtbl = l_safeStringVtable;
    names->resource.mStringTop = gabi::guest_addr("ProgramTexture.bfres");
    names->archive.mStringTop = gabi::guest_addr("ProgramTexture");
    void* resource =
        texturedLineGetResource(l_resourceControl, &names->archive, &names->resource, 0);
    texturedLineLoadTexture(&mToonTexObj, resource, "himo_nml");
    texturedLineSetTexture(mpTextures, 1, &mToonTexObj, 0);
    return 1;
}

/* 025EA154 */
void line_mat0::mDoExt_3DlineMat0_c::setMaterial() {
    texturedLineSetLighting();
    dKy_tevstr_c* lighting = mpTevStr;
    if (lighting != nullptr)
        texturedLineSetFog(lighting);
    else
        texturedLineSetDefaultFog();
}

/* 025EA1A8 */
void line_mat0::mDoExt_3DlineMat0_c::draw(DrawContext* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 length = (mNumSegments * 2) & 0xFFFF;
    line_mat0::mDoExt_3Dline_c* buffer = mpLines;
    gabi::Local<RenderState> state;
    if (!l_packetNameInitialized) {
        l_packetNameInitialized = 1;
        l_packetName.__vtbl = l_safeStringVtable;
        l_packetName.mStringTop = gabi::guest_addr("line_packet_mat0");
        __register_global_object(gabi::ea(&l_packetNameDestructor));
    }
    for (u32 i = 0; i < (u32)mNumLines; i++, buffer++) {
        u32 kind = arguments->mPass;
        DrawPacket* packet = nullptr;
        if ((s32)kind < 4)
            packet = mpShaders[i].getEntry(kind)->mpPacket;
        ShaderCache* cache = texturedLineGetShaderCache(&l_j3dSys);
        ShaderMaterial* material = packet->mpMaterial;
        if (material != cache->mpMaterial.get()) {
            u8 flags = material->mFlags;
            ShaderProgram* previous = cache->mpShaderProgram;
            if (flags & 2) {
                material->mFlags = flags & ~2;
                texturedLineInvalidateMaterial(material, 0);
            }
            ShaderProgram* program = material->mpParameters->mpShaderProgram;
            if (previous != program)
                texturedLineUseShader(program);
            u32 bytes = material->mDisplayListSize;
            if (bytes) {
                texturedLineCallDisplayList(material->mpDisplayList, bytes);
                cache->mpShaderProgram = program;
                cache->mpMaterial = material;
            } else {
                texturedLineUseMaterial(material);
                cache->mpMaterial = material;
                cache->mpShaderProgram = program;
            }
        }
        kind = arguments->mPass;
        if (kind == 0) {
            RendererManager* root = mpRenderer;
            root->mPrimary.apply(packet);
            RendererManager* controller = arguments->mpController;
            if (controller != nullptr) {
                u32 index = arguments->mMode;
                bindPacketAttributes(controller->getEntry(index), packet);
            }
        } else if (kind == 1) {
            texturedLineRendererPrepare(mpRenderer, packet, 0);
            texturedLinePrepareTextures(mpTextures, packet);
        } else if (kind == 2) {
            texturedLineRendererPrepare(mpRenderer, packet, 0);
            RenderInterface* renderer = arguments->mpRenderer;
            if (renderer != nullptr)
                renderer->apply(packet);
            texturedLinePrepareTextures(mpTextures, packet);
            texturedLinePrepareContext(arguments, packet);
        }
        texturedLineInitRenderState(state.get());
        u32 flags = state->mFlags;
        state->mPassMode = 2;
        state->mDrawMode = 2;
        state->mFlags = (((flags & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 0x10;
        texturedLineSetRenderState(arguments->mPass, state.get());
        texturedLineApplyRenderState(state.get());
        ShaderBinding* table = &mpShaders[i];
        u32 capacity = table->mCapacity;
        u32 pass = arguments->mPass;
        ShaderBindingEntry* entry = table->mpEntries;
        if (pass < capacity)
            entry += pass;
        u32 bit = buffer->mpMeshBuffer->mCurArr;
        u32 offset = ((bit == 0 ? 32 : __builtin_clz(bit)) >> 2) & 0x3FFFFFF8;
        texturedLineUseVertexBuffer(entry->mVertexBindings[offset / sizeof(gptr<void>)]);
        texturedLineDrawMesh(buffer->mpMeshBuffer, &l_drawPrimitive, 1, length);
    }
    mCurArr = mCurArr ^ 1;
}

using namespace line_mat0;

/* 025EA530 */
static u32 mDoExt_3Dline_nonzero(f32 value) { return value > l_threshold; }

// clang-format off
/* WWHD glue: HD rendering bindings, per-TU data and original strings.
data l_shaderNames 1048CEE8
data l_packetName 1048CEC8
data l_shaderNamesInitialized 1048CF18
data l_packetNameInitialized 1048CF1C
data l_shaderNamesDestructor 101F4764
data l_packetNameDestructor 101F4770
data l_resourceControl 101F4F7C
data l_j3dSys 104B45C0
data l_drawPrimitive 104B4A84
data l_rendererZero 10145180
data l_rendererOne 1014517C
ext 025E947C void texturedLineBase_ct(void*)
ext 025E94EC void texturedLineBase_dt(void*, u32)
ext 028F521C void texturedLineZeroBytes(void*, u32)
ext 028EFFD0 void* texturedLineAllocArray(void*, u32, u32, u32)
ext 028F0164 void texturedLineDestroyArray(void*, s32, u32, u32, u32, u32)
ext 027BEBEC void texturedLineBufferDestroy(line_mat0::GraphicsBuffer*)
ext 027FB40C void texturedLineUniform_ct(line_mat0::UniformBlock*)
ext 027FB528 void texturedLineUniform_dt(line_mat0::UniformBlock*, u32)
ext 027FD6F4 void texturedLineRenderer_ct(line_mat0::RendererManager*)
ext 027FD764 void texturedLineRenderer_dt(line_mat0::RendererManager*, u32)
ext 027FE084 void texturedLineRendererInit(line_mat0::RendererManager*, u32, u32)
ext 027FE118 void texturedLineRendererPrepare(line_mat0::RendererManager*, line_mat0::DrawPacket*, u32)
ext 027FE344 line_mat0::TextureManager* texturedLineTextureManager_ct(u32)
ext 027FE4B0 void texturedLineTextureManager_dt(line_mat0::TextureManager*, u32)
ext 027FE500 void texturedLineSetTexture(line_mat0::TextureManager*, u32, line_mat0::TextureHandle*, u32)
ext 027FE5B8 void texturedLinePrepareTextures(line_mat0::TextureManager*, line_mat0::DrawPacket*)
ext 027FE640 line_mat0::TextureBinding* texturedLineGetTexture(line_mat0::TextureManager*, u32)
ext 027FFCBC line_mat0::ShaderRegistry* texturedLineGetShaderRegistry()
ext 027B90AC u32 texturedLineFindShader(line_mat0::ShaderArchive*, SafeString*)
ext 02800B0C void texturedLineInitializeShader(line_mat0::ShaderResource*, line_mat0::ShaderArchiveEntry*, u32)
ext 0280068C void texturedLineBindShader(line_mat0::ShaderBinding*, line_mat0::ShaderResource*, u32)
ext 025E8FA8 u32 texturedLineInitGeometry(line_mat0::mDoExt_3Dline_c*, u32, u32, u32)
ext 027FF078 void texturedLineInitMesh(line_mat0::MeshBuffer*, u32, u32, u32)
ext 028003E8 void texturedLineBindMesh(line_mat0::ShaderBindingEntry*, line_mat0::DrawPacket*, line_mat0::MeshBuffer*, u32)
ext 026124B0 void* texturedLineGetResource(line_mat0::ResourceControl*, SafeString*, SafeString*, u32)
ext 02773870 void texturedLineLoadTexture(line_mat0::TextureHandle*, void*, const char*)
ext 02773798 void texturedLineUseImage(line_mat0::TextureHandle*, void*)
ext 0274FBF8 void texturedLineTextureLock(line_mat0::TextureLock*)
ext 0274FCCC void texturedLineTextureUnlock(line_mat0::TextureLock*)
ext 0255F160 void texturedLineSetLighting()
ext 0255F8F4 void texturedLineSetFog(dKy_tevstr_c*)
ext 0255F84C void texturedLineSetDefaultFog()
ext 027F29D4 line_mat0::ShaderCache* texturedLineGetShaderCache(line_mat0::J3DSysView*)
ext 027BB9E0 void texturedLineInvalidateMaterial(line_mat0::ShaderMaterial*, u32)
ext 027B9F68 void texturedLineUseShader(line_mat0::ShaderProgram*)
ext 027BB7CC void texturedLineUseMaterial(line_mat0::ShaderMaterial*)
ext C00060E0 void texturedLineCallDisplayList(void*, u32)
ext C0006900 void texturedLineBindNormal(s32, void*, void*)
ext C0006A38 void texturedLineBindPosition(s32, void*, void*)
ext C00068A8 void texturedLineBindTexCoord(s32, void*, void*)
ext 02750250 void texturedLineInitRenderState(line_mat0::RenderState*)
ext 0280037C void texturedLineSetRenderState(u32, line_mat0::RenderState*)
ext 02750370 void texturedLineApplyRenderState(line_mat0::RenderState*)
ext 027BFE5C void texturedLineUseVertexBuffer(void*)
ext 027FEDB0 void texturedLineDrawMesh(line_mat0::MeshBuffer*, line_mat0::DrawPrimitive*, u32, u32)
ext 027FFE54 void texturedLinePrepareContext(line_mat0::DrawContext*, line_mat0::DrawPacket*)
str 10058B44 "line_mat"
str 10058B34 "line_toon_mat"
str 10058B50 "ProgramTexture"
str 10058B60 "ProgramTexture.bfres"
str 10058B84 "line_packet_mat0"
str 10058B78 "himo_nml"
ext 0273ADAC void* lineAllocateBytes(u32)
ext 0273AFC8 void lineFreeBytes(void*)
ext 027FE64C line_mat0::MeshBuffer* lineCreateMesh(u32)
ext 027FE6BC void lineAllocateMesh(line_mat0::MeshBuffer*,u32,u32,u32)
ext 027FF260 void lineUploadMesh(line_mat0::MeshBuffer*)
ext 027BF7E8 void lineReleaseBuffer(line_mat0::BufferState*)
ext 027BF880 void lineBuffer_dt(line_mat0::BufferState*,u32)
ext 027B5CBC void lineResource_dt(line_mat0::VertexResource*,u32)
ext 02755FEC line_mat0::HeapManager* lineFindHeap(line_mat0::HeapRegistry*,void*)
ext 027FEB60 void lineMesh_dt(line_mat0::MeshBuffer*,u32)
ext 027BE6B8 void lineTexture_ct(line_mat0::TextureHandle*)
ext 0273AA24 void lineAssert(const char*,u32,const char*)
ext 028E9098 void lineMatrixIdentity(Mtx34*)
ext 0274D2E0 void lineDivideColor(line_mat0::Float4*,f32)
ext 0274D2AC void lineScaleColor(line_mat0::Float4*,f32)
ext 025E2B54 void lineBuildLightingMatrix(Mtx34*,Mtx34*,line_mat0::J3DSysView*)
ext 027FDA54 void lineApplyLighting(line_mat0::RendererManager*,u32,Mtx34*,line_mat0::LightWords*,void*)
ext 0274D458 void lineMixColor(line_mat0::Float4*,line_mat0::Float4*,f32)
ext 027FE0DC void lineCommitRenderer(line_mat0::RendererManager*,u32)
data l_lineZero 100586DC
data l_lineOne 100586D4
data l_tangentPositive 10058ADC
data l_tangentNegative 10058AE0
data l_colorDivisor 10058B30
data l_threshold 10058B98
data l_heapRegistry 101F8B4C
data l_viewMatrix 104B45F8
str 10058AFC "m_Do_ext.cpp"
str 10058AE4 "mShaderAssignSet == (0)"
str 10058B0C "mUBOSet == (0)"
str 10058B1C "mSamplerSet == (0)"
*/
