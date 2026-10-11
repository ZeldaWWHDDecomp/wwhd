/**
 * Textured 3D line material, following zeldaret/tww m_Do_ext names.
 * HD uses shader packets, mesh buffers and normal attributes in place of GX
 * arrays. Verified against WWHD by m_Do_ext_13. Private game-derived source.
 */
#include "m_Do/m_Do_ext_line_mat1.h"
using namespace line_mat1;
static ShaderNames l_shaderNames;
static SafeString l_packetName;
static u32 l_shaderNamesInitialized;
static u32 l_packetNameInitialized;
static DestructorRecord l_shaderNamesDestructor;
static DestructorRecord l_packetNameDestructor;
static ResourceControl* l_resourceControl;
static TextureLock* l_textureLock;
static J3DSysView l_j3dSys;
static DrawPrimitive l_drawPrimitive;
static f32 l_rendererZero;
static f32 l_rendererOne;
// HD: per-TU vtables and compiler array callbacks.
static constexpr u32 l_materialVtable = 0x10058DF0;
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

/* 025EB82C */
static void* mDoExt_3DlineMat1_c_ct(void* self) {
    auto* material = (line_mat1::mDoExt_3DlineMat1_c*)self;
    if (material == nullptr) {
        material = (line_mat1::mDoExt_3DlineMat1_c*)operator_new(sizeof(*material));
        if (material == nullptr)
            return nullptr;
    }
    texturedLineBase_ct(material);
    material->__vtbl = l_materialVtable;
    texturedLineZeroBytes(material->mMaterialCache, sizeof(material->mMaterialCache));
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

/* 025EB8B8 */
static void mDoExt_3DlineMat1_c_dtor(void* self, u32 flag) {
    auto* material = (line_mat1::mDoExt_3DlineMat1_c*)self;
    if (material == nullptr)
        return;
    material->__vtbl = l_materialVtable;
    line_mat1::mDoExt_3Dline_c* lines = material->mpLines;
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

/* 025EBA58 */
u32 line_mat1::mDoExt_3DlineMat1_c::init(u32 count, u32 points, void* texture, u32 optional) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!l_shaderNamesInitialized) {
        l_shaderNamesInitialized = 1;
        l_shaderNames.names[0].__vtbl = l_safeStringVtable;
        l_shaderNames.names[0].mStringTop = gabi::guest_addr("line_mat1");
        l_shaderNames.names[1].__vtbl = l_safeStringVtable;
        l_shaderNames.names[1].mStringTop = gabi::guest_addr("line_toon_mat1");
        __register_global_object(gabi::ea(&l_shaderNamesDestructor));
    }
    mNumLines = (u16)count;
    mMaxSegments = (u16)points;
    mpLines = (line_mat1::mDoExt_3Dline_c*)texturedLineAllocArray(
        nullptr, count, sizeof(line_mat1::mDoExt_3Dline_c), LINE_ARRAY_CONSTRUCTOR);
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
        texturedLineZeroBytes(root->mScratch, sizeof(root->mScratch));
        // HD: preserve the nullable embedded constructors, including address
        // wrapping.
        if (gabi::ea(root->mScratch) == 0)
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
    texturedLineTextureLock(l_textureLock);
    texturedLineUseImage(&mTexObj, ((TextureResource*)texture)->mpImage);
    texturedLineTextureUnlock(l_textureLock);
    texturedLineSetTexture(mpTextures, 0, &mTexObj, 0);
    TextureBinding* binding = texturedLineGetTexture(mpTextures, 0);
    u8 flags = binding->mFlags;
    binding->mWrapS = 0;
    binding->mWrapT = 0;
    binding->mFlags = flags | 2;
    return 1;
}

/* 025EC090 */
void line_mat1::mDoExt_3DlineMat1_c::setMaterial() {
    texturedLineSetLighting();
    dKy_tevstr_c* lighting = mpTevStr;
    if (lighting != nullptr)
        texturedLineSetFog(lighting);
    else
        texturedLineSetDefaultFog();
}

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

/* 025EC0E4 */
void line_mat1::mDoExt_3DlineMat1_c::draw(DrawContext* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 length = (mNumSegments * 2) & 0xFFFF;
    line_mat1::mDoExt_3Dline_c* buffer = mpLines;
    gabi::Local<RenderState> state;
    if (!l_packetNameInitialized) {
        l_packetNameInitialized = 1;
        l_packetName.__vtbl = l_safeStringVtable;
        l_packetName.mStringTop = gabi::guest_addr("line_packet_mat1");
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
            RendererManager* root = mpRenderer;
            RendererManager* controller =
                arguments->mMode == 2 ? arguments->mpController.get() : root;
            bindPacketAttributes(controller->mpEntries, packet);
            root = mpRenderer;
            root->mSecondary.apply(packet);
            root = mpRenderer;
            root->mPrimary.apply(packet);
            RenderInterface* renderer = arguments->mpRenderer;
            if (renderer != nullptr)
                renderer->apply(packet);
            texturedLinePrepareTextures(mpTextures, packet);
            texturedLinePrepareContext(arguments, packet);
        }
        texturedLineInitRenderState(state.get());
        u32 flags = state->mFlags;
        state->mPassMode = arguments->mMode == 2 ? 3 : 2;
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

using namespace line_mat1;
static f32 texturedLineZero;
static f32 texturedLineHalf;
static f32 texturedLineTexScale;
static void vectorCopy(cXyz* dst, const cXyz* src) { dst->copy(*src); }
static void positionCopy(Vertex* dst, const cXyz* src) {
    // HD: position writes are floating copies; work-vector copies preserve words.
    dst->mPosition.x = src->x;
    dst->mPosition.y = src->y;
    dst->mPosition.z = src->z;
}
static Vertex* vertex(VertexTable* table, u32 index) { return table->getVertex(index); }
static void attributes(VertexTable* table, u32 index, const cXyz* tangent, f32 x, f32 y, f32 z,
                       f32 zero, bool reverse) {
    Vertex* q = vertex(table, index);
    q->mTangent.x = tangent->x;
    q->mTangent.y = tangent->y;
    q->mTangent.z = tangent->z;
    q = vertex(table, index);
    if (reverse) {
        q->mNormal.y = y;
        q->mNormal.z = z;
        q->mNormalW = zero;
        q->mNormal.x = x;
    } else {
        q->mNormal.x = x;
        q->mNormal.y = y;
        q->mNormal.z = z;
        q->mNormalW = zero;
    }
}

/* 025EC62C */
void line_mat1::mDoExt_3DlineMat1_c::update(u32 count, const be<u32>* color, u32 taper,
                                            dKy_tevstr_c* lighting, f32 width) {
    gabi::AutoFrame frame{gabi::current_function()};
    mColor = (u32)*color;
    u32 capacity = mMaxSegments;
    mpTevStr = lighting;
    mNumSegments = (u16)count;
    if (count > capacity)
        mNumSegments = (u16)capacity;
    CameraView* camera = ((PlayView*)dComIfGp_get())->mView;
    line_mat1::mDoExt_3Dline_c* line = mpLines;
    f32 zero = texturedLineZero;
    f32 step = taper ? width / (f32)taper : zero;
    gabi::Local<LineWork> work;
    cXyz* tangent = &work->tangent;
    cXyz* cross = &work->cross;
    cXyz* normal = &work->normal;
    cXyz* temp = &work->temp;
    cXyz* eye = &work->eye;
    cXyz* out = &work->out;
    cXyz* norm = &work->norm;
    cXyz* partial = &work->partial;
    cXyz* averagePlus = &work->averagePlus;
    cXyz* averageMinus = &work->averageMinus;
    cXyz* scaled = &work->scaled;
    cXyz* norm2 = &work->norm2;
    f32 distance = zero, texScale = texturedLineTexScale;
    for (u32 i = 0; i < mNumLines; i++, ++line) {
        f32 half = texturedLineHalf, currentWidth = width;
        cXyz* point = line->mpSegments;
        MeshBuffer* root = line->mpMeshBuffer;
        VertexTable* table = root->getVertices();
        Vertex* destination;
        *temp = point[1] - point[0];
        vectorCopy(cross, temp);
        PSVECSquareMag(cross);
        f32 segment = texturedLineSquareRoot();
        vectorCopy(tangent, cross);
        vertex(table, 0)->mTexY = distance;
        vertex(table, 1)->mTexY = distance;
        distance = gabi::fmadds(segment, texScale, distance);
        *temp = point[0] - camera->mEye;
        vectorCopy(eye, temp);
        *temp = cross->outprod(*eye);
        vectorCopy(cross, temp);
        PSVECSquareMag(cross);
        f32 magnitude = texturedLineSquareRoot();
        *temp = tangent->outprod(*cross);
        vectorCopy(tangent, temp);
        f32 nx, ny, nz;
        if (texturedLineNonzero(magnitude)) {
            texturedLineNormalize(tangent, norm);
            vectorCopy(tangent, norm);
            texturedLineNormalize(cross, norm);
            nz = norm->z;
            normal->z = nz;
            nx = norm->x;
            normal->x = nx;
            ny = norm->y;
            normal->y = ny;
            PSVECScale(cross, cross, currentWidth / magnitude);
        } else {
            nz = normal->z;
            nx = normal->x;
            ny = normal->y;
        }
        destination = table->mpVertices;
        *out = point[0] + *cross;
        positionCopy(destination, out);
        attributes(table, 0, tangent, nx, ny, nz, zero, true);
        destination = vertex(table, 1);
        *out = point[0] - *cross;
        positionCopy(destination, out);
        attributes(table, 1, tangent, nx, ny, nz, zero, false);
        ++point;
        u32 index = 2;
        *temp = point[0] + *cross;
        vectorCopy(averagePlus, temp);
        *temp = point[0] - *cross;
        vectorCopy(averageMinus, temp);
        s32 remaining = (s32)mNumSegments - 2;
        for (s32 n = remaining; n > 0; n--) {
            if (n < (s32)taper)
                currentWidth = gabi::fsubs_ppc(currentWidth, step);
            *partial = point[1] - point[0];
            vectorCopy(cross, partial);
            PSVECSquareMag(cross);
            segment = texturedLineSquareRoot();
            vectorCopy(tangent, cross);
            vertex(table, index)->mTexY = distance;
            vertex(table, index + 1)->mTexY = distance;
            distance = gabi::fmadds(segment, texScale, distance);
            *partial = point[0] - camera->mEye;
            vectorCopy(eye, partial);
            *partial = cross->outprod(*eye);
            vectorCopy(cross, partial);
            PSVECSquareMag(cross);
            magnitude = texturedLineSquareRoot();
            *partial = tangent->outprod(*cross);
            vectorCopy(tangent, partial);
            if (texturedLineNonzero(magnitude)) {
                texturedLineNormalize(tangent, norm2);
                vectorCopy(tangent, norm2);
                texturedLineNormalize(cross, norm2);
                nx = norm2->x;
                ny = norm2->y;
                normal->x = nx;
                nz = norm2->z;
                normal->y = ny;
                magnitude = currentWidth / magnitude;
                normal->z = nz;
            }
            PSVECScale(cross, cross, magnitude);
            *partial = point[0] + *cross;
            PSVECAdd(averagePlus, partial, averagePlus);
            *partial = point[0] - *cross;
            PSVECAdd(averageMinus, partial, averageMinus);
            destination = vertex(table, index);
            texturedLineScale(averagePlus, scaled, half);
            positionCopy(destination, scaled);
            attributes(table, index, tangent, nx, ny, nz, zero, true);
            index++;
            destination = vertex(table, index);
            texturedLineScale(averageMinus, scaled, half);
            positionCopy(destination, scaled);
            attributes(table, index, tangent, nx, ny, nz, zero, false);
            ++point;
            index++;
            *partial = point[0] + *cross;
            vectorCopy(averagePlus, partial);
            *partial = point[0] - *cross;
            vectorCopy(averageMinus, partial);
        }
        vertex(table, index)->mTexY = distance;
        vertex(table, index + 1)->mTexY = distance;
        cXyz* last = taper ? point : averagePlus;
        destination = vertex(table, index);
        positionCopy(destination, last);
        attributes(table, index, tangent, nx, ny, nz, zero, false);
        index++;
        destination = vertex(table, index);
        positionCopy(destination, taper ? point : averageMinus);
        attributes(table, index, tangent, nx, ny, nz, zero, true);
    }
    for (u32 i = 0; i < mNumLines; i++)
        texturedLineFinalize(mpLines[i].mpMeshBuffer, 0, 1);
    texturedLineMaterialSetup(this, mpTevStr, &mColor);
}

/* 025ED1BC */
void line_mat1::mDoExt_3DlineMat1_c::updateWidths(u32 count, const be<u32>* color,
                                                  dKy_tevstr_c* lighting) {
    gabi::AutoFrame frame{gabi::current_function()};
    mColor = (u32)*color;
    u32 capacity = mMaxSegments;
    mpTevStr = lighting;
    mNumSegments = (u16)count;
    if (count > capacity)
        mNumSegments = (u16)capacity;
    CameraView* camera = ((PlayView*)dComIfGp_get())->mView;
    line_mat1::mDoExt_3Dline_c* line = mpLines;
    f32 zero = texturedLineZero;
    gabi::Local<WidthWork> work;
    cXyz* tangent = &work->tangent;
    cXyz* cross = &work->cross;
    cXyz* normal = &work->normal;
    cXyz* temp = &work->temp;
    cXyz* eye = &work->eye;
    cXyz* out = &work->out;
    cXyz* norm = &work->norm;
    cXyz* partial = &work->partial;
    cXyz* averagePlus = &work->averagePlus;
    cXyz* averageMinus = &work->averageMinus;
    cXyz* scaled = &work->scaled;
    cXyz* norm2 = &work->norm2;
    f32 distance = zero, texScale = texturedLineTexScale;
    for (u32 i = 0; i < mNumLines; i++, ++line) {
        f32 half = texturedLineHalf;
        be<u8>* widths = line->mpSize;
        cXyz* point = line->mpSegments;
        if (!widths)
            JUT_ASSERT_fail("m_Do_ext.cpp", 0x1EFE, "size_p != (0)");
        MeshBuffer* root = line->mpMeshBuffer;
        VertexTable* table = root->getVertices();
        Vertex* destination;
        vertex(table, 0)->mTexY = distance;
        vertex(table, 1)->mTexY = distance;
        *temp = point[1] - point[0];
        vectorCopy(cross, temp);
        PSVECSquareMag(cross);
        f32 segment = texturedLineSquareRoot();
        vectorCopy(tangent, cross);
        distance = gabi::fmadds(segment, texScale, distance);
        *temp = point[0] - camera->mEye;
        vectorCopy(eye, temp);
        *temp = cross->outprod(*eye);
        vectorCopy(cross, temp);
        PSVECSquareMag(cross);
        f32 magnitude = texturedLineSquareRoot();
        *temp = tangent->outprod(*cross);
        vectorCopy(tangent, temp);
        f32 nx, ny, nz;
        if (texturedLineNonzero(magnitude)) {
            texturedLineNormalize(tangent, norm);
            vectorCopy(tangent, norm);
            texturedLineNormalize(cross, norm);
            nz = norm->z;
            normal->z = nz;
            nx = norm->x;
            normal->x = nx;
            ny = norm->y;
            normal->y = ny;
            PSVECScale(cross, cross, (f32)(u8)*widths / magnitude);
        } else {
            nz = normal->z;
            nx = normal->x;
            ny = normal->y;
        }
        destination = table->mpVertices;
        *out = point[0] + *cross;
        positionCopy(destination, out);
        attributes(table, 0, tangent, nx, ny, nz, zero, true);
        destination = vertex(table, 1);
        *out = point[0] - *cross;
        positionCopy(destination, out);
        attributes(table, 1, tangent, nx, ny, nz, zero, false);
        ++point;
        widths++;
        u32 index = 2;
        *temp = point[0] + *cross;
        vectorCopy(averagePlus, temp);
        *temp = point[0] - *cross;
        vectorCopy(averageMinus, temp);
        s32 remaining = (s32)mNumSegments - 2;
        for (s32 n = remaining; n > 0; n--) {
            *partial = point[1] - point[0];
            vectorCopy(cross, partial);
            PSVECSquareMag(cross);
            segment = texturedLineSquareRoot();
            vectorCopy(tangent, cross);
            vertex(table, index)->mTexY = distance;
            vertex(table, index + 1)->mTexY = distance;
            distance = gabi::fmadds(segment, texScale, distance);
            *partial = point[0] - camera->mEye;
            vectorCopy(eye, partial);
            *partial = cross->outprod(*eye);
            vectorCopy(cross, partial);
            PSVECSquareMag(cross);
            magnitude = texturedLineSquareRoot();
            *partial = tangent->outprod(*cross);
            vectorCopy(tangent, partial);
            if (texturedLineNonzero(magnitude)) {
                texturedLineNormalize(tangent, norm2);
                vectorCopy(tangent, norm2);
                texturedLineNormalize(cross, norm2);
                nx = norm2->x;
                ny = norm2->y;
                normal->x = nx;
                nz = norm2->z;
                normal->y = ny;
                magnitude = (f32)(u8)*widths / magnitude;
                normal->z = nz;
            }
            PSVECScale(cross, cross, magnitude);
            *partial = point[0] + *cross;
            PSVECAdd(averagePlus, partial, averagePlus);
            *partial = point[0] - *cross;
            PSVECAdd(averageMinus, partial, averageMinus);
            destination = vertex(table, index);
            texturedLineScale(averagePlus, scaled, half);
            positionCopy(destination, scaled);
            attributes(table, index, tangent, nx, ny, nz, zero, true);
            index++;
            destination = vertex(table, index);
            texturedLineScale(averageMinus, scaled, half);
            positionCopy(destination, scaled);
            attributes(table, index, tangent, nx, ny, nz, zero, false);
            ++point;
            widths++;
            index++;
            *partial = point[0] + *cross;
            vectorCopy(averagePlus, partial);
            *partial = point[0] - *cross;
            vectorCopy(averageMinus, partial);
        }
        vertex(table, index)->mTexY = distance;
        vertex(table, index + 1)->mTexY = distance;
        cXyz* last = averagePlus;
        destination = vertex(table, index);
        positionCopy(destination, last);
        attributes(table, index, tangent, nx, ny, nz, zero, false);
        index++;
        destination = vertex(table, index);
        positionCopy(destination, averageMinus);
        attributes(table, index, tangent, nx, ny, nz, zero, true);
    }
    for (u32 i = 0; i < mNumLines; i++)
        texturedLineFinalize(mpLines[i].mpMeshBuffer, 0, 1);
    texturedLineMaterialSetup(this, mpTevStr, &mColor);
}

// clang-format off
/* WWHD glue: HD rendering bindings, per-TU data and original strings.
data l_shaderNames 1048CEF8
data l_packetName 1048CED0
data l_shaderNamesInitialized 1048CF20
data l_packetNameInitialized 1048CF24
data l_shaderNamesDestructor 101F477C
data l_packetNameDestructor 101F4788
data l_resourceControl 101F4F7C
data l_textureLock 101F8B18
data l_j3dSys 104B45C0
data l_drawPrimitive 104B4A84
data l_rendererZero 10145180
data l_rendererOne 1014517C
ext 025E947C void texturedLineBase_ct(void*)
ext 025E94EC void texturedLineBase_dt(void*, u32)
ext 028F521C void texturedLineZeroBytes(void*, u32)
ext 028EFFD0 void* texturedLineAllocArray(void*, u32, u32, u32)
ext 028F0164 void texturedLineDestroyArray(void*, s32, u32, u32, u32, u32)
ext 027BEBEC void texturedLineBufferDestroy(line_mat1::GraphicsBuffer*)
ext 027FB40C void texturedLineUniform_ct(line_mat1::UniformBlock*)
ext 027FB528 void texturedLineUniform_dt(line_mat1::UniformBlock*, u32)
ext 027FD6F4 void texturedLineRenderer_ct(line_mat1::RendererManager*)
ext 027FD764 void texturedLineRenderer_dt(line_mat1::RendererManager*, u32)
ext 027FE084 void texturedLineRendererInit(line_mat1::RendererManager*, u32, u32)
ext 027FE118 void texturedLineRendererPrepare(line_mat1::RendererManager*, line_mat1::DrawPacket*, u32)
ext 027FE344 line_mat1::TextureManager* texturedLineTextureManager_ct(u32)
ext 027FE4B0 void texturedLineTextureManager_dt(line_mat1::TextureManager*, u32)
ext 027FE500 void texturedLineSetTexture(line_mat1::TextureManager*, u32, line_mat1::TextureHandle*, u32)
ext 027FE5B8 void texturedLinePrepareTextures(line_mat1::TextureManager*, line_mat1::DrawPacket*)
ext 027FE640 line_mat1::TextureBinding* texturedLineGetTexture(line_mat1::TextureManager*, u32)
ext 027FFCBC line_mat1::ShaderRegistry* texturedLineGetShaderRegistry()
ext 027B90AC u32 texturedLineFindShader(line_mat1::ShaderArchive*, SafeString*)
ext 02800B0C void texturedLineInitializeShader(line_mat1::ShaderResource*, line_mat1::ShaderArchiveEntry*, u32)
ext 0280068C void texturedLineBindShader(line_mat1::ShaderBinding*, line_mat1::ShaderResource*, u32)
ext 025E8FA8 u32 texturedLineInitGeometry(line_mat1::mDoExt_3Dline_c*, u32, u32, u32)
ext 027FF078 void texturedLineInitMesh(line_mat1::MeshBuffer*, u32, u32, u32)
ext 028003E8 void texturedLineBindMesh(line_mat1::ShaderBindingEntry*, line_mat1::DrawPacket*, line_mat1::MeshBuffer*, u32)
ext 026124B0 void* texturedLineGetResource(line_mat1::ResourceControl*, SafeString*, SafeString*, u32)
ext 02773870 void texturedLineLoadTexture(line_mat1::TextureHandle*, void*, const char*)
ext 02773798 void texturedLineUseImage(line_mat1::TextureHandle*, void*)
ext 0274FBF8 void texturedLineTextureLock(line_mat1::TextureLock*)
ext 0274FCCC void texturedLineTextureUnlock(line_mat1::TextureLock*)
ext 0255F160 void texturedLineSetLighting()
ext 0255F8F4 void texturedLineSetFog(dKy_tevstr_c*)
ext 0255F84C void texturedLineSetDefaultFog()
ext 027F29D4 line_mat1::ShaderCache* texturedLineGetShaderCache(line_mat1::J3DSysView*)
ext 027BB9E0 void texturedLineInvalidateMaterial(line_mat1::ShaderMaterial*, u32)
ext 027B9F68 void texturedLineUseShader(line_mat1::ShaderProgram*)
ext 027BB7CC void texturedLineUseMaterial(line_mat1::ShaderMaterial*)
ext C00060E0 void texturedLineCallDisplayList(void*, u32)
ext C0006900 void texturedLineBindNormal(s32, void*, void*)
ext C0006A38 void texturedLineBindPosition(s32, void*, void*)
ext C00068A8 void texturedLineBindTexCoord(s32, void*, void*)
ext 02750250 void texturedLineInitRenderState(line_mat1::RenderState*)
ext 0280037C void texturedLineSetRenderState(u32, line_mat1::RenderState*)
ext 02750370 void texturedLineApplyRenderState(line_mat1::RenderState*)
ext 027BFE5C void texturedLineUseVertexBuffer(void*)
ext 027FEDB0 void texturedLineDrawMesh(line_mat1::MeshBuffer*, line_mat1::DrawPrimitive*, u32, u32)
ext 027FFE54 void texturedLinePrepareContext(line_mat1::DrawContext*, line_mat1::DrawPacket*)
data texturedLineZero 100586DC
data texturedLineHalf 100586D8
data texturedLineTexScale 10058C20
str 10058C34 "m_Do_ext.cpp"
str 10058C24 "size_p != (0)"
ext 025EA530 u32 texturedLineNonzero(f32)
ext 028F4384 f32 texturedLineSquareRoot()
ext 0201B084 void texturedLineNormalize(cXyz*, cXyz*)
ext 0201AE48 void texturedLineScale(cXyz*, cXyz*, f32)
ext 027FF1D8 void texturedLineFinalize(line_mat1::MeshBuffer*, u32, u32)
ext 025E9670 void texturedLineMaterialSetup(line_mat1::mDoExt_3DlineMat1_c*, dKy_tevstr_c*, be<u32>*)
str 10058BBC "line_mat1"
str 10058BC8 "line_toon_mat1"
str 10058BD8 "ProgramTexture"
str 10058BE8 "ProgramTexture.bfres"
str 10058C0C "line_packet_mat1"
str 10058C00 "himo_nml"
*/
