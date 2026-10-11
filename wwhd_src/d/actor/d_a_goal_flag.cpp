/** Finish-line flag, cloth simulation and boat-race controller (WWHD).
 * See wwhd_src/README.md. */
#include "d/actor/d_a_goal_flag.h"

// HD: this TU owns its HIO. Shared matrix state is referenced without duplicating storage.
static daGFlag_HIO_c l_HIO;
static constexpr u32 l_hio_vtable = 0x1001037C;
static gptr<Mtx34>& l_calc_mtx = gabi::object_at<gptr<Mtx34>>(0x1018C7B0);

// HD: references to retained engine objects and this unit's constructor data.
static gptr<GoalMinigameScreens>& l_minigameScreens = gabi::object_at<gptr<GoalMinigameScreens>>(0x101F8344);
static gptr<void>& l_graphicsLock = gabi::object_at<gptr<void>>(0x101F8B18);
static gptr<void>& l_heapManager = gabi::object_at<gptr<void>>(0x101F8B4C);
static gptr<void>& l_resourceControl = gabi::object_at<gptr<void>>(0x101F4F28);
static gptr<dSv_info_c>& l_saveInfo = gabi::object_at<gptr<dSv_info_c>>(0x101F84DC);
static gptr<void>& l_opaqueDrawBuffer = gabi::object_at<gptr<void>>(0x104B4634);
static Mtx34& l_matrixStack = gabi::object_at<Mtx34>(0x1048D0CC);
static GoalJ3DSys& l_j3dSys = gabi::object_at<GoalJ3DSys>(0x104B45C0);
static GoalRenderState& l_defaultRenderState = gabi::object_at<GoalRenderState>(0x104B474C);
static be<s32>& l_minigameRupee = gabi::object_at<be<s32>>(0x101D5F18);
static be<u32>& l_minigameResult = gabi::object_at<be<u32>>(0x101D5F1C);
static be<f32>& l_zero = gabi::object_at<be<f32>>(0x10145180);
static be<f32>& l_one = gabi::object_at<be<f32>>(0x1014517C);
static be<f32>& l_minWindMagnitude = gabi::object_at<be<f32>>(0x100030B8);
static be<f32>& l_minAngle = gabi::object_at<be<f32>>(0x10464148);
static be<f32>& l_maxAngle = gabi::object_at<be<f32>>(0x1046414C);
static be<f32>& l_flagCullMax = gabi::object_at<be<f32>>(0x10464150);
static be<f32>& l_flagCullMin = gabi::object_at<be<f32>>(0x10464154);
static be<f32>& l_ropeCullMax = gabi::object_at<be<f32>>(0x10464158);
static be<f32>& l_ropeCullMin = gabi::object_at<be<f32>>(0x1046415C);
static be<u8>& l_staticByte = gabi::object_at<be<u8>>(0x10464160);
static be<u8>& l_staticByteFlag = gabi::object_at<be<u8>>(0x10464161);
static auto& l_staticDtorSlots = gabi::object_at<be<u32>[4]>(0x104641A4);
static auto& l_globalDtorRecords = gabi::object_at<be<u32>[12]>(0x101B5D1C);
static char* sub_arcname_tbl[2] = {"Gflag", "Tgflag"};
// Model data (vertex positions, texture coordinates, index lists) generated from your copy of the
// game at build time (tools/verify/assets.py, build/verify/include/assets/); never committed.
#include "assets/l_pos__d_a_goal_flag.h"
#include "assets/l_texCoord__d_a_goal_flag.h"
static auto& l_zeroVectorWords = gabi::object_at<be<u32>[3]>(0x101FFBA8);
static const f32 down_offset[4] = {0.0f, 150.0f, 225.0f, 150.0f};
static char* ActionNames[5] = {"00_dummy", "01_dummy", "02_dummy", "03_dummy", "04_dummy"};
static char* l_endEventNames[2] = {"race_goal_cam", "race_fail_cam"};
static char* l_orderEventNames[2] = {"race_goal_cam", "race_fail_cam"};
static constexpr u32 l_packetVtable = 0x1001061C, l_safeStringVtable = 0x10010334, l_actorVtable = 0x1001036C;
static const char l_clothDeleteName[] = "Cloth";
static const char arcname[] = "Cloth";
static const char l_flagArchiveName[] = "Gflag";
static const char l_boatingStage[] = "Ocean";
static const char l_flightArchiveName[] = "Tgflag";
static const char l_shaderName[] = "flag_default";
static const char l_shaderFile[] = "d_a_goal_flag.cpp";
static const char l_textureResourceAssert[] = "tmp_img != (0)";
static const char l_actorFile[] = "d_a_goal_flag.h";
static const char l_ropeIndexAssert[] = "(line >= 0) && (line < 4)";
static const char l_toonArchiveName[] = "Cloth";
static const char l_toonFile[] = "d_a_goal_flag.cpp";
static const char l_toonResourceAssert[] = "tmp_img != (0)";
static const char l_startStaff[] = "Gflag";
static const char l_startEvent[] = "race_start_cam";
static const char l_timerFile[] = "d_a_goal_flag.cpp";
static const char l_timerAssert[] = "timer != (0)";
static const char l_nextStage[] = "sea";
static const char l_endStaff[] = "Gflag";

static auto& l_startSequence = gabi::object_at<u8>(0x1048F27C);
static auto& l_endSequence = gabi::object_at<u8>(0x1049DD60);
static auto& l_matrixUniformVtable = gabi::object_at<GoalUniformVtable>(0x1016EF84);
static auto& l_materialUniformVtable = gabi::object_at<GoalUniformVtable>(0x1016EFB4);
static auto& l_vertexFormat = gabi::object_at<u8>(0x10010610);
// model data, generated from the game (tools/verify/assets.py)
#include "assets/l_flagIndices__d_a_goal_flag.h"

// Forward declarations for this translation unit's generated callbacks.
static void get_cloth_anim_sub_factor(cXyz* position, cXyz* neighbor, cXyz* correction, f32 idealDistance);
static BOOL checkCreateHeap(fopAc_ac_c* actor);
static BOOL daGoal_FlagIsDelete(void* actor);
static void goalFlag_copyMatrix(void* destination, void* source);
static void goalFlag_colorS10ToFloat(void* destination, void* source);
static void goalFlag_colorToFloat(void* destination, void* source);
static void* GoalVertexBuffer_ctor(void* buffer);
static void* GoalBufferEntry_ctor(void* entry);
static void GoalBufferEntry_dtor(void* entry, s32 flags);
static void GoalVertexBuffer_dtor(void* buffer, s32 flags);
static void daGFlag_HIO_c_dtor(daGFlag_HIO_c* hio, s32 flags);
static void daGFlag_HIO_c_callback(void* hio);
static void __sinit_d_a_goal_flag();
static void daGFlag_packet_c_dtor(void* packet, s32 flags);
static void daGoal_Flag_c_dtor(void* actor, s32 flags);
static void daGFlag_packet_c_empty(void* packet);
static void* daGFlag_packet_c_ctor(void* packet);

static void* Goal_eventManager() { return &goalFlag_gameInfo(goalFlag_playObject())->play.mEvtManager; }

static void Goal_cutEnd(s32 staff) { goalFlag_eventCutEnd(Goal_eventManager(), staff); }

static void Goal_assertRopeIndex(u32 rope) {
    if (rope >= 4)
        goalFlag_assertionFail(l_actorFile, 0x9B, l_ropeIndexAssert);
}

static void* Goal_ropePoint(void* line, s32 anchor) {
    return goalFlag_guestAddress(goalFlag_ropeLine(line)->mpPositions.get()->get() + (u32)anchor * 4);
}

template <class T> static void Goal_freeAllocation(gptr<T>* allocation) {
    void* heap = goalFlag_findAllocationHeap(l_heapManager.get(), allocation->get());
    goalFlag_heap(heap)->free(allocation->get());
}

static void Goal_releaseVertices(void* vertices) {
    for (s32 pair = 0; pair < 2; ++pair)
        for (s32 side = 0; side < 2; ++side) {
            void* buffer = &goalFlag_vertexBuffers(vertices)->mBuffers[pair][side];
            goalFlag_textureRelease(&goalFlag_vertexBuffer(buffer)->mTexture);
            bool allocated = gabi::ea(goalFlag_vertexBuffer(buffer)->mpAllocation.get()) != 0;
            goalFlag_vertexBuffer(buffer)->mpData = nullptr;
            if (allocated) {
                (void)(u32)goalFlag_vertexBuffer(buffer)->mVertexCount;
                Goal_freeAllocation(&goalFlag_vertexBuffer(buffer)->mpAllocation);
                goalFlag_vertexBuffer(buffer)->mVertexCount = 0;
                goalFlag_vertexBuffer(buffer)->mpAllocation = nullptr;
            }
        }
    goalFlag_vertexBuffers(vertices)->mFlags = 0;
}

static void Goal_releaseTextures(void* table) {
    if (!gabi::ea(goalFlag_textureTable(table)->mpPairs.get()))
        return;
    for (s32 entry = 0; entry < (s32)goalFlag_textureTable(table)->mCount; ++entry) {
        void* pair = goalFlag_guestAddress(goalFlag_textureTable(table)->mpPairs.get() + entry);
        if (!pair)
            continue;
        bool allocated = gabi::ea(goalFlag_texturePair(pair)->mpFrontTextures.get()) != 0;
        goalFlag_texturePair(pair)->mMaterialId = 0;
        if (allocated) {
            for (s32 i = 0; i < (s32)goalFlag_texturePair(pair)->mFrontCount; ++i) {
                void* texture = goalFlag_guestAddress(goalFlag_texturePair(pair)->mpFrontTextures.get() + i);
                goalFlag_texture(texture)->destroy(2);
            }
            Goal_freeAllocation(&goalFlag_texturePair(pair)->mpFrontTextures);
            goalFlag_texturePair(pair)->mFrontCount = 0;
            goalFlag_texturePair(pair)->mpFrontTextures = nullptr;
        }
        if (gabi::ea(goalFlag_texturePair(pair)->mpBackTextures.get())) {
            for (s32 i = 0; i < (s32)goalFlag_texturePair(pair)->mBackCount; ++i) {
                void* texture = goalFlag_guestAddress(goalFlag_texturePair(pair)->mpBackTextures.get() + i);
                goalFlag_texture(texture)->destroy(2);
            }
            Goal_freeAllocation(&goalFlag_texturePair(pair)->mpBackTextures);
            goalFlag_texturePair(pair)->mBackCount = 0;
            goalFlag_texturePair(pair)->mpBackTextures = nullptr;
        }
    }
    Goal_freeAllocation(&goalFlag_textureTable(table)->mpPairs);
    goalFlag_textureTable(table)->mCount = 0;
    goalFlag_textureTable(table)->mpPairs = nullptr;
}

static void Goal_materialOwnerDtor(void* owner) {
    gabi::Local<be<u32>[4]> callFrame;
    goalFlag_materialOwnerDestroy(owner, 2);
}

static void Goal_releasePacketMembers(void* packet) {
    void* vertices = goalFlag_guestAddress(&goalFlag_packet(packet)->mVertexBuffers);
    Goal_releaseVertices(vertices);
    for (s32 material = 0; material < (s32)goalFlag_packet(packet)->mShaderMaterialCount; ++material) {
        u32 data = gabi::ea(goalFlag_packet(packet)->mpShaderMaterials.get());
        if ((u32)material < (u32)goalFlag_packet(packet)->mShaderMaterialCount)
            data += (u32)material * 0x23C;
        auto* uniforms = &gabi::object_at<GoalShaderMaterial>(data).mUniforms;
        goalFlag_uniformBufferRelease(&uniforms->mBuffers[0]);
        goalFlag_uniformBufferRelease(&uniforms->mBuffers[1]);
    }
    for (GoalUniformBlock* block :
         {&goalFlag_packet(packet)->mMatrixBlock, &goalFlag_packet(packet)->mMaterialBlock})
        for (auto& buffer : block->mBuffers)
            goalFlag_uniformBufferRelease(goalFlag_guestAddress(&buffer));
    goalFlag_textureObjectDestroy(goalFlag_guestAddress(&goalFlag_packet(packet)->mToonTexture), 2);
    goalFlag_textureObjectDestroy(goalFlag_guestAddress(&goalFlag_packet(packet)->mFlagTexture), 2);
    goalFlag_indexBufferDestroy(goalFlag_guestAddress(&goalFlag_packet(packet)->mIndices), 2);
    goalFlag_uniformBlockDestroy(goalFlag_guestAddress(&goalFlag_packet(packet)->mMaterialBlock), 0);
    goalFlag_uniformBlockDestroy(goalFlag_guestAddress(&goalFlag_packet(packet)->mMatrixBlock), 0);
    Goal_materialOwnerDtor(goalFlag_guestAddress(&goalFlag_packet(packet)->mShaderMaterialCount));
    if (vertices) {
        Goal_releaseVertices(vertices);
        goalFlag_arrayDestroy(vertices, 4, 0x254, GoalVertexBuffer_dtor, 0, 0);
    }
    Goal_releaseTextures(goalFlag_guestAddress(&goalFlag_packet(packet)->mTexturePairCount));
}

static void* Goal_allocate(u32 size, u32 alignment) {
    void* heap = goalFlag_currentHeap(l_heapManager.get());
    return goalFlag_heap(heap)->allocate(size, alignment);
}

static void Goal_releaseTexturePair(void* pair) {
    auto* textures = goalFlag_texturePair(pair);
    bool allocated = textures->mpFrontTextures.get() != nullptr;
    textures->mMaterialId = 0;
    for (u32 side = 0; side < 2; ++side) {
        be<s32>* count = side ? &textures->mBackCount : &textures->mFrontCount;
        gptr<GoalTexture>* data = side ? &textures->mpBackTextures : &textures->mpFrontTextures;
        if (side ? data->get() != nullptr : allocated) {
            for (s32 i = 0; i < (s32)*count; ++i)
                goalFlag_guestAddress(data->get() + i)->destroy(2);
            Goal_freeAllocation(data);
            *count = 0;
            *data = nullptr;
        }
    }
}

static void Goal_bindUniforms(void* shader, void* material) {
    GoalUniformDescriptor* descriptor =
        goalFlag_material(material)->mUniformCount ? goalFlag_material(material)->mpUniforms.get() : nullptr;
    s16 vertex = descriptor->mVertexSlot, pixel = descriptor->mPixelSlot,
        geometry = descriptor->mGeometrySlot;
    GoalUniformBuffer* block = goalFlag_guestAddress(
        &goalFlag_uniformBlock(shader)->mBuffers[(u32)goalFlag_uniformBlock(shader)->mCurrentBuffer]);
    u32 size = block->mSize;
    void* data = block->mpData.get();
    if (pixel != -1)
        goalFlag_pixelUniformBlock(pixel, size, data);
    if (vertex != -1)
        goalFlag_vertexUniformBlock(vertex, size, data);
    if (geometry != -1)
        goalFlag_geometryUniformBlock(geometry, size, data);
}

static void Goal_loadTexture(void* packet, void* material, u32 textureIndex) {
    u32 count = goalFlag_material(material)->mTextureCount;
    GoalMaterialTexture* descriptor = nullptr;
    if (count > textureIndex)
        descriptor = goalFlag_guestAddress(goalFlag_material(material)->mpTextures.get() + textureIndex);
    goalFlag_textureBind(textureIndex ? static_cast<void*>(&goalFlag_packet(packet)->mToonTexture)
                                      : static_cast<void*>(&goalFlag_packet(packet)->mFlagTexture),
                         goalFlag_guestAddress(&descriptor->mBinding), -1, 0);
}

/* 0215DFB4 */
void daGFlag_packet_c::setTexObj(u32 archive) {
    // HD: keep the full archive index and update native resource bindings under the graphics lock.
    gabi::Local<SafeString> name;
    goalFlag_graphicsLock(l_graphicsLock.get());
    name->mStringTop = gabi::ea(sub_arcname_tbl[archive].get());
    name->__vtbl = l_safeStringVtable;
    void* resource = goalFlag_objectResource(l_resourceControl.get(), name.get(), 3);
    if (!resource)
        goalFlag_assertionFail(l_shaderFile, 0xDC, l_textureResourceAssert);
    goalFlag_textureResourceLoad(&mFlagResource, goalFlag_resource(resource)->mpTexture.get());
    goalFlag_graphicsUnlock(l_graphicsLock.get());
    const be<u32> GoalTextureDescription::* fields[] = {
        &GoalTextureDescription::mDimension, &GoalTextureDescription::mWidth,
        &GoalTextureDescription::mHeight,    &GoalTextureDescription::mDepth,
        &GoalTextureDescription::mMipLevels, &GoalTextureDescription::mFormat,
        &GoalTextureDescription::mAlignment, &GoalTextureDescription::mPitch,
        &GoalTextureDescription::mTileMode};
    bool equal = true;
    for (auto field : fields)
        if (mFlagTexture.mDescription.*field != mFlagResource.mDescription.*field) {
            equal = false;
            break;
        }
    if (!equal) {
        goalFlag_textureAssign(goalFlag_guestAddress(&mFlagTexture), goalFlag_guestAddress(&mFlagResource));
        u8 flags = (u8)mFlagTexture.mFlags | 2;
        mFlagTexture.mMipDirty = 2;
        mFlagTexture.mImageDirty = 2;
        mFlagTexture.mSamplerDirty = 2;
        mFlagTexture.mFlags = flags;
    } else {
        void* image = mFlagResource.mDescription.mpImage.get();
        void* mips = mFlagResource.mDescription.mpMipImage.get();
        mFlagTexture.mpNativeImage = image;
        u8 flags = (u8)mFlagTexture.mFlags | 2;
        mFlagTexture.mDescription.mpMipImage = mips;
        mFlagTexture.mpNativeMipImage = mips;
        mFlagTexture.mImageDirty = 2;
        mFlagTexture.mMipDirty = 2;
        mFlagTexture.mFlags = flags;
        mFlagTexture.mDescription.mpImage = image;
        mFlagTexture.mSamplerDirty = 2;
    }
}

/* 02160018 */
void daGFlag_packet_c::draw(void* drawInfo) {
    gabi::Local<GoalRenderState> renderState; // HD: native alpha-test and culling state.
    void* material = nullptr;
    s32 index = (s32)goalFlag_drawInfo(drawInfo)->mMaterialIndex;
    if (index < 4) {
        u32 data = gabi::ea(mpTexturePairs.get());
        if ((u32)index < (u32)mTexturePairCount)
            data += (u32)index * 0x14;
        material = goalFlag_texturePair(&gabi::object_at<GoalTexturePair>(data))->mpMaterial.get();
    }
    void* cache = goalFlag_shaderCache(&l_j3dSys);
    void* shader = goalFlag_material(material)->mpShader.get();
    if (gabi::ea(shader) != gabi::ea(goalFlag_cache(cache)->mpShader.get())) {
        void* oldShader = goalFlag_cache(cache)->mpNativeShader.get();
        u8 flags = (u8)goalFlag_shader(shader)->mFlags;
        if (flags & 2) {
            goalFlag_shader(shader)->mFlags = flags & ~2;
            goalFlag_shaderRefresh(shader, 0);
        }
        void* nativeShader = goalFlag_shader(shader)->mpNative->mpShader.get();
        if (oldShader != nativeShader)
            goalFlag_nativeShaderBind(nativeShader);
        void* fetch = goalFlag_shader(shader)->mpFetchShader.get();
        if (fetch)
            goalFlag_fetchShaderBind(goalFlag_shader(shader)->mpFetchData.get(), fetch);
        else
            goalFlag_shaderBind(shader);
        goalFlag_cache(cache)->mpShader = shader;
        goalFlag_cache(cache)->mpNativeShader = nativeShader;
    }
    index = (s32)goalFlag_drawInfo(drawInfo)->mMaterialIndex;
    auto upload = [&](GoalUniformBlock* block) { block->upload(material); };
    if (index == 0) {
        upload(&mMatrixBlock);
        void* pass = goalFlag_drawInfo(drawInfo)->mpPass.get();
        if (pass)
            Goal_bindUniforms(static_cast<GoalDrawPass*>(pass)->mpUniformBlock.get(), material);
        Goal_loadTexture(this, material, 0);
    } else if (index == 1 || index == 2) {
        Goal_bindUniforms(mpShaderMaterials.get(), material);
        upload(&mMaterialBlock);
        upload(&mMatrixBlock);
        if (index == 2) {
            void* extra = goalFlag_drawInfo(drawInfo)->mpExtraUniforms.get();
            if (extra)
                goalFlag_uniformBlock(extra)->upload(material);
        }
        Goal_loadTexture(this, material, 0);
        Goal_loadTexture(this, material, 1);
    }
    if (index == 2)
        goalFlag_drawInfoPrepare(drawInfo, material);
    goalFlag_renderStateConstruct(renderState.get());
    renderState->mCullMode = 0;
    s32 pass = (s32)goalFlag_drawInfo(drawInfo)->mMaterialIndex;
    renderState->mMode = 2;
    u32 flags = (u32)renderState->mFlags;
    renderState->mAlphaEnabled = 1;
    renderState->mAlphaThreshold = 0.5f;
    renderState->mAlphaFunction = 4;
    renderState->mFlags = (((flags & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 0x10;
    goalFlag_renderStateConfigure(pass, renderState.get());
    goalFlag_renderStateApply(renderState.get());
    for (u32 side = 0; side < 2; ++side) {
        if (side) {
            renderState->mCullMode = 1;
            goalFlag_renderStateApply(renderState.get());
        }
        u32 data = gabi::ea(mpTexturePairs.get()), entry = (u32)goalFlag_drawInfo(drawInfo)->mMaterialIndex;
        if (entry < (u32)mTexturePairCount)
            data += entry * 0x14;
        auto* textures = goalFlag_texturePair(&gabi::object_at<GoalTexturePair>(data));
        bool back = mVertexBuffers.mCurrentBuffer == 0;
        GoalTexture* texture = back ? textures->mpBackTextures.get() : textures->mpFrontTextures.get();
        if (side && (u32)(back ? textures->mBackCount : textures->mFrontCount) > 1)
            texture = goalFlag_guestAddress(texture + 1);
        goalFlag_textureApply(texture);
        for (u32 row = 0; row < 4; ++row) {
            u32 stride = mIndices.mStride, mode = mIndices.mMode;
            u8* indices = mIndices.mpIndices.get();
            u32 type = mIndices.mType;
            goalFlag_drawIndexed(mode, 0x12, type, goalFlag_guestAddress(indices + stride * row * 0x12), 0,
                                 1);
        }
    }
    goalFlag_renderStateApply(&l_defaultRenderState);
}

/* 0215E964 */
void daGFlag_packet_c::setBackNrm() {
    cXyz* front = mNrm[mCurrArr];
    cXyz* back = mBackNrm[mCurrArr];
    for (s32 i = 0; i < 45; ++i) {
        back->x = 0.0f;
        back->y = 0.0f;
        back->z = 0.0f;
        goalFlag_vectorSubtractPS(back, front, back);
        ++back;
        ++front;
    }
}

/* 0215E520 */
void daGFlag_packet_c::setNrmVtx(cXyz* output, s32 column, s32 row) {
    gabi::Local<cXyz> center, average, horizontal, vertical, triangle, temp, normalized;
    cXyz* positions = mDPos[mCurrArr];
    s32 index = row * 9 + column;
    center->set(positions[index].x, positions[index].y, positions[index].z);
    average->set(0, 0, 0);
    auto edge = [&](s32 neighbor, cXyz* destination) {
        goalFlag_vectorSubtract(&positions[neighbor], temp.get(), center.get());
        destination->copy(*temp);
    };
    auto accumulate = [&](cXyz* a, cXyz* b) {
        goalFlag_vectorCross(a, temp.get(), b);
        triangle->copy(*temp);
        goalFlag_vectorNormalizeZP(triangle.get(), temp.get());
        triangle->copy(*temp);
        goalFlag_vectorAdd(average.get(), triangle.get(), average.get());
    };
    if (column != 0) {
        edge(index - 1, horizontal.get());
        if (row != 0) {
            edge(index - 9, vertical.get());
            accumulate(vertical.get(), horizontal.get());
        }
        if (row != 4) {
            edge(index + 9, vertical.get());
            accumulate(horizontal.get(), vertical.get());
        }
    }
    if (column != 8) {
        edge(index + 1, horizontal.get());
        if (row != 0) {
            edge(index - 9, vertical.get());
            accumulate(horizontal.get(), vertical.get());
        }
        if (row != 4) {
            edge(index + 9, vertical.get());
            accumulate(vertical.get(), horizontal.get());
        }
    }
    goalFlag_vectorNormalizeZC(average.get(), normalized.get());
    average->copy(*normalized);
    goalFlag_matrixPush();
    s32 phase = column * -0x400 + row * 0x100;
    u16 angle = (u16)(phase + mNormalFlutterPhase);
    f32 sine = cM_ssin(angle);
    goalFlag_matrixYRotate(l_calc_mtx.get(), (s16)gabi::ftoi(512.0f * sine));
    angle = (u16)(phase + mNormalFlutterPhase);
    f32 cosine = cM_scos(angle);
    goalFlag_matrixXRotate(l_calc_mtx.get(), (s16)gabi::ftoi(512.0f * cosine));
    goalFlag_matrixPosition(average.get(), triangle.get());
    goalFlag_vectorNormalizeZP(triangle.get(), normalized.get());
    output->copy(*normalized);
    goalFlag_matrixPull();
}

/* 0215DD94 */
BOOL daGoal_Flag_c::getRacePath(u8 pathId) {
    void* path = goalFlag_roomPath(pathId, current.roomNo);
    mpRopePaths[0] = path;
    if (!path)
        return FALSE;
    s32 rope = 0;
    do {
        mNumRopeBuoys[rope] = goalFlag_path(path)->m_num;
        ++rope;
        pathId = goalFlag_path(mpRopePaths[rope - 1].get())->m_nextID;
        if (rope >= 4 || pathId == 0xFF)
            break;
        path = goalFlag_roomPath(pathId, current.roomNo);
        mpRopePaths[rope] = path;
    } while (true);
    mNumRopes = rope;
    return TRUE;
}

/* 0215F4D0 */
void daGoal_Flag_c::RopeMove() {
    gabi::Local<cXyz> from, to, sum;
    for (s32 rope = 0; rope < mNumRopes; ++rope) {
        s32 count = mNumRopeBuoys[rope];
        Goal_assertRopeIndex(rope);
        void* line = &mRopeLines[rope];
        void* end = Goal_ropePoint(line, count);
        Goal_assertRopeIndex(rope);
        void* first = Goal_ropePoint(line, 0);
        for (u32 i = 0; i < 3; ++i)
            goalFlag_vectorWords(end)->mComponents[i] = goalFlag_vectorWords(first)->mComponents[i];
        for (s32 segment = 0; segment < (s32)(mNumRopeBuoys[rope] * 4); ++segment) {
            s32 subdivision = segment & 3, anchor = segment >> 2;
            if (subdivision) {
                Goal_assertRopeIndex(rope);
                void* a = Goal_ropePoint(line, anchor);
                Goal_assertRopeIndex(rope);
                void* b = Goal_ropePoint(line, anchor + 1);
                cXyz* destination = goalFlag_guestAddress(static_cast<cXyz*>(a) + subdivision);
                f32 t = (f32)subdivision * 0.25f;
                goalFlag_vectorMultiply(a, from.get(), 1.0f - t);
                goalFlag_vectorMultiply(b, to.get(), t);
                goalFlag_vectorSum(from.get(), sum.get(), to.get());
                // lfs/stfs copy the bits (signalling NaNs stay signalling); only the sag subtraction rounds.
                goalFlag_vectorWords(destination)->mComponents[0] =
                    goalFlag_vectorWords(sum.get())->mComponents[0];
                u32 y = goalFlag_vectorWords(sum.get())->mComponents[1];
                goalFlag_vectorWords(destination)->mComponents[1] = y;
                goalFlag_vectorWords(destination)->mComponents[2] =
                    goalFlag_vectorWords(sum.get())->mComponents[2];
                destination->y = gabi::fsubs_ppc(gabi::f32_from_bits(y), (f32)down_offset[subdivision]);
            }
        }
    }
}

/* 0215DE4C */
BOOL daGoal_Flag_c::CreateBuoyRaces() {
    gabi::Local<cXyz> position;
    for (s32 rope = 0; rope < mNumRopes; ++rope) {
        void* path = mpRopePaths[rope].get();
        GoalPathPoint* points = goalFlag_pathPoints(path);
        void* line = &mRopeLines[rope];
        void* segment = goalFlag_ropeLine(line)->mpPositions.get()->get();
        s32 buoy = 0;
        for (; buoy < mNumRopeBuoys[rope]; ++buoy) {
            position->x = (f32)points->m_position.x;
            position->y = (f32)points->m_position.y;
            position->z = (f32)points->m_position.z;
            goalFlag_createChild(fpcNm_Obj_Buoyrace_e, (u32)mBsPcId, buoy | (rope << 8), position.get(),
                                 current.roomNo, 0, 0, -1, 0);
            f32 x = (f32)points->m_position.x, y = (f32)points->m_position.y + 250.0f,
                z = (f32)points->m_position.z;
            static_cast<cXyz*>(segment)->x = x;
            static_cast<cXyz*>(segment)->z = z;
            static_cast<cXyz*>(segment)->y = y;
            segment = goalFlag_guestAddress(static_cast<cXyz*>(segment) + 1);
            points = goalFlag_guestAddress(points + 1);
        }
        void* first = goalFlag_ropeLine(line)->mpPositions.get()->get();
        for (u32 component = 0; component < 3; ++component)
            goalFlag_vectorWords(segment)->mComponents[component] =
                goalFlag_vectorWords(first)->mComponents[component];
    }
    return TRUE;
}

/* 02160618 */
s32 daGoal_Flag_c::goal_check() {
    gabi::Local<cXyz> playerOffset, line, normalized, rawLine, horizontal, normal;
    u32 play = goalFlag_playObject();
    void* player = goalFlag_gameInfo(play)->play.mPlayerInfo[0].mpPlayer.get();
    goalFlag_vectorSubtract(&static_cast<fopAc_ac_c*>(player)->current.pos, playerOffset.get(),
                            &mGoalFlagPolePos[0]);
    goalFlag_vectorSubtract(&mGoalFlagPolePos[1], line.get(), &mGoalFlagPolePos[0]);
    line->y = 0.0f;
    goalFlag_vectorNormalizeZP(line.get(), normalized.get());
    line->copy(*normalized);
    playerOffset->y = 0.0f;
    goalFlag_vectorSubtract(&mGoalFlagPolePos[1], rawLine.get(), &mGoalFlagPolePos[0]);
    horizontal->set(rawLine->x, 0.0f, rawLine->z);
    f32 square = goalFlag_vectorSquareMagnitude(horizontal.get());
    f32 length = goalFlag_squareRoot(square);
    f32 along = goalFlag_vectorDot(line.get(), playerOffset.get());
    normal->set(line->z, 0.0f, -(f32)line->x);
    f32 side = goalFlag_vectorDot(normal.get(), playerOffset.get());
    s32 result = 0;
    if (along > 0.0f && along < length) {
        f32 previous = mPrevPlayerLineSide;
        if (previous > 0.0f) {
            if (!(side > 0.0f))
                result = 1;
        } else if (side > 0.0f)
            result = -1;
    }
    mPrevPlayerLineSide = side;
    return result;
}

/* 0215EA00 */
void daGoal_Flag_c::flag_move() {
    gabi::Local<cXyz> wind, normalized, factor, temp;
    u8 oldArray = mFlagPacket.mCurrArr, newArray = (oldArray ^ 1) & 1;
    s16 normalPhase = mFlagPacket.mNormalFlutterPhase, windPhase = mWindScalePhase,
        wavePhase = mFlagPacket.mFlagWavePhase;
    mFlagPacket.mCurrArr = newArray;
    windPhase = (s16)(windPhase + l_HIO.mWindScalePhaseInc);
    mWindScalePhase = windPhase;
    mFlagPacket.mNormalFlutterPhase = (s16)(normalPhase + l_HIO.mNormalFlutterPhaseInc);
    mFlagPacket.mFlagWavePhase = (s16)(wavePhase + l_HIO.mFlagWavePhaseInc);
    f32 blend = gabi::fmadds(0.5f, cM_ssin((u16)windPhase), 0.5f);
    f32 windScale = gabi::fmadds(l_HIO.mWindScale1, blend, l_HIO.mWindScale2 * (1.0f - blend));
    cXyz* previous = mFlagPacket.mPos[oldArray];
    cXyz* previousNormals = mFlagPacket.mNrm[oldArray];
    cXyz* current = mFlagPacket.mPos[newArray];
    cXyz* velocity = mFlagPacket.mVelocity;
    goalFlag_matrixYSet(l_calc_mtx.get(), (s16) - this->current.angle.y);
    void* worldWind = goalFlag_windVector();
    goalFlag_matrixPosition(worldWind, wind.get());
    f32 magnitude = goalFlag_vectorSquareMagnitude(wind.get());
    if (std::fabs(magnitude) < (f32)l_minWindMagnitude) {
        for (u32 i = 0; i < 3; ++i)
            goalFlag_vectorWords(wind.get())->mComponents[i] = l_zeroVectorWords[i];
    } else {
        goalFlag_vectorNormalize(wind.get(), normalized.get());
        wind->y = normalized->y;
        wind->z = normalized->z;
        wind->x = (f32)normalized->x * 0.2f;
        goalFlag_vectorScale(wind.get(), wind.get(), windScale);
    }
    for (s32 column = 0; column < 9; ++column)
        for (s32 row = 0; row < 5; ++row) {
            s32 index = column + row * 9;
            current[index].copy(previous[index]);
            this->get_cloth_anim_factor(factor.get(), previous, previousNormals, wind.get(), column, row);
            temp->copy(*factor);
            goalFlag_vectorAdd(&velocity[index], temp.get(), &velocity[index]);
            goalFlag_vectorScale(&velocity[index], &velocity[index], 0.85f);
            goalFlag_vectorAdd(&current[index], &velocity[index], &current[index]);
        }
    cXyz* display = mFlagPacket.mDPos[(u32)mFlagPacket.mCurrArr];
    for (s32 column = 0; column < 9; ++column)
        for (s32 row = 0; row < 5; ++row) {
            s32 index = column + row * 9;
            f32 distance = std::fabs(4.5f - (f32)column);
            f32 falloff = gabi::fnmsubs(distance, distance, 20.25f) / 20.25f;
            display[index].set(current[index].x, current[index].y, current[index].z);
            f32 sine = cM_ssin((u16)(column * 0x4000 + row * 0x2000 + mFlagPacket.mFlagWavePhase));
            display[index].z = gabi::fmadds(40.0f * falloff, sine, (f32)display[index].z);
        }
    cXyz* normals = mFlagPacket.mNrm[(u32)mFlagPacket.mCurrArr];
    for (s32 row = 0; row < 5; ++row)
        for (s32 column = 0; column < 9; ++column)
            this->mFlagPacket.setNrmVtx(&normals[row * 9 + column], column, row);
    this->mFlagPacket.setBackNrm();
    goalFlag_cacheStoreRange(mFlagPacket.mDPos[(u32)mFlagPacket.mCurrArr], 0x21C);
    goalFlag_cacheStoreRange(mFlagPacket.mNrm[(u32)mFlagPacket.mCurrArr], 0x21C);
    goalFlag_cacheStoreRange(mFlagPacket.mBackNrm[(u32)mFlagPacket.mCurrArr], 0x21C);
}

/* 0215EDF8 */
s32 daGoal_Flag_c::_create() {
    gabi::Local<SafeString> expectedStage, currentStage;
    gabi::Local<cXyz> playerOffset, line, normalized, normal;
    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (this) {
            goalFlag_actorBaseConstruct(this);
            __vtbl = l_actorVtable;
            daGFlag_packet_c_ctor(&mFlagPacket);
            goalFlag_arrayConstruct(mRopeLines, 4, 0x148, goalFlag_ropeLineConstruct);
        }
        actor_condition = actor_condition | fopAcCnd_INIT_e;
    }
    u8 variant = (u8)mParameters;
    s32 phase = goalFlag_resourceLoad(&mClothPhs, arcname);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u8 archive = (variant == 0 || variant == 255) ? 0 : 1;
    phase = goalFlag_resourceLoad(&mFlagPhs, archive ? l_flightArchiveName : l_flagArchiveName);
    scale.x = archive ? 0.98f : 1.05f;
    scale.z = 1;
    scale.y = 1;
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u8 pathId = mParameters >> 16;
    if (pathId == 255)
        return cPhs_ERROR_e;
    void* path = goalFlag_roomPath(pathId, current.roomNo);
    if (!path)
        return cPhs_ERROR_e;
    if (this->getRacePath(goalFlag_path(path)->m_nextID)) {
        if (!fopAcM_entrySolidHeap(this, checkCreateHeap, 0x10000))
            return cPhs_ERROR_e;
        this->CreateBuoyRaces();
    }
    cXyz* positions = mFlagPacket.mPos[(u32)mFlagPacket.mCurrArr];
    for (u32 vertex = 0; vertex < 45; ++vertex)
        positions[vertex].set((f32)l_pos[vertex].x, (f32)l_pos[vertex].y, (f32)l_pos[vertex].z);
    this->mFlagPacket.setTexObj(archive);
    pathId = mParameters >> 16;
    path = goalFlag_roomPath(pathId, current.roomNo);
    f32 x = current.pos.x;
    if (path) {
        GoalPathPoint* points = goalFlag_pathPoints(path);
        mGoalFlagPolePos[0].set(points[0].m_position.x, points[0].m_position.y, points[0].m_position.z);
        points = goalFlag_pathPoints(path);
        mGoalFlagPolePos[1].set(points[1].m_position.x, points[1].m_position.y, points[1].m_position.z);
    }
    goalFlag_matrixSetTranslate(&l_matrixStack, x, current.pos.y, current.pos.z);
    goalFlag_matrixYRotate(&l_matrixStack, shape_angle.y);
    goalFlag_matrixStackScale(scale.x, scale.y, scale.z);
    goalFlag_matrixCopy(&l_matrixStack, &mMtx);
    expectedStage->mStringTop = gabi::guest_addr(l_boatingStage);
    expectedStage->__vtbl = l_safeStringVtable;
    u32 play = goalFlag_playObject();
    currentStage->mStringTop = gabi::ea(goalFlag_gameInfo(play)->play.mCurStage.mName);
    currentStage->__vtbl = l_safeStringVtable;
    auto validateString = [](SafeString* string) {
        gabi::call_ptr(gabi::object_at<GoalSafeStringVtable>((u32)string->__vtbl).assureTermination.get(),
                       string);
    };
    validateString(expectedStage.get());
    validateString(expectedStage.get());
    u32 expected = expectedStage->mStringTop;
    validateString(currentStage.get());
    u32 current = currentStage->mStringTop;
    bool equal = expected == current;
    if (!equal) {
        expected = expectedStage->mStringTop;
        current = currentStage->mStringTop;
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 a = (&gabi::object_at<be<u8>>(expected))[i], b = (&gabi::object_at<be<u8>>(current))[i];
            if (a != b)
                break;
            if (a == 0) {
                equal = true;
                break;
            }
        }
    }
    bool start = false;
    if (equal) {
        play = goalFlag_playObject();
        start = (s16)goalFlag_gameInfo(play)->play.mCurStage.mPoint == 1;
    }
    if (start) {
        u32 save = gabi::ea(l_saveInfo.get());
        u16 limit = (u16)l_HIO.mTimeLimit;
        // HD: scale the full event-register result before truncation; subtract only when below the limit.
        u16 modifier = (u16)(goalFlag_eventRegister(
                                 &gabi::object_at<GoalSaveRoot>(save).mSave.mSavedata.mEvent, 0xAAFF) *
                             10);
        if (limit > modifier)
            limit -= modifier;
        s32 timer = goalFlag_timerCreate(fpcNm_TIMER_e, 2, limit, 3, 0, 0, 221.0f, 439.0f, 32.0f, 419.0f);
        mTimerProcID = timer;
        u32 singleton = gabi::ea(l_minigameScreens.get());
        void* starter = goalFlag_screens(singleton)->mpStarter.get();
        if (starter) {
            goalFlag_starter(starter)->mReset = 0;
            goalFlag_starter(starter)->mStatus = 0;
            singleton = gabi::ea(l_minigameScreens.get());
        }
        void* terminater = goalFlag_screens(singleton)->mpTerminator.get();
        if (terminater) {
            goalFlag_terminator(terminater)->mReset = 0;
            goalFlag_startSequence(goalFlag_terminator(terminater)->mSequence, &l_endSequence);
            singleton = gabi::ea(l_minigameScreens.get());
        }
        void* failure = goalFlag_screens(singleton)->mpFailure.get();
        if (failure)
            goalFlag_resetFailureSequence(failure);
        play = goalFlag_playObject();
        u16 flags = (u16)goalFlag_gameInfo(play)->play.mMinigameFlags;
        goalFlag_gameInfo(play)->play.mMiniGameType = 1;
        goalFlag_gameInfo(play)->play.mMinigameFlags = flags | 1;
        play = goalFlag_playObject();
        goalFlag_gameInfo(play)->play.mMiniGameRupee = 0;
        play = goalFlag_playObject();
        gabi::object_at<GoalGameAux>(play).mMiniGameReset = 0;
        mRaceStartState = 0;
        mActionAdjustment = 0;
        mActionDispatch = -1;
        mActionTarget = 0x021607D4;
    }
    play = goalFlag_playObject();
    void* player = goalFlag_gameInfo(play)->play.mPlayerInfo[0].mpPlayer.get();
    goalFlag_vectorSubtract(&static_cast<fopAc_ac_c*>(player)->current.pos, playerOffset.get(),
                            &mGoalFlagPolePos[0]);
    goalFlag_vectorSubtract(&mGoalFlagPolePos[1], line.get(), &mGoalFlagPolePos[0]);
    line->y = 0;
    goalFlag_vectorNormalizeZP(line.get(), normalized.get());
    line->set(normalized->x, normalized->y, normalized->z);
    normal->set(line->z, 0, -(f32)line->x);
    f32 side = goalFlag_vectorDot(normal.get(), playerOffset.get());
    mRaceEndState = 0;
    mPrevPlayerLineSide = side;
    for (u32 i = 0; i < 20; ++i)
        this->flag_move();
    mTimerEnded = 0;
    return cPhs_COMPLEATE_e;
}

/* 0215D26C */
static void get_cloth_anim_sub_factor(cXyz* position, cXyz* neighbor, cXyz* correction, f32 idealDistance) {
    gabi::Local<cXyz> delta, direction;
    goalFlag_vectorSubtract(neighbor, delta.get(), position);
    goalFlag_vectorNormalizeZP(delta.get(), direction.get());
    f32 square = goalFlag_vectorSquareMagnitude(delta.get());
    f32 length = goalFlag_squareRoot(square);
    f32 stiffness = l_HIO.mClothStiffness;
    goalFlag_vectorScale(direction.get(), direction.get(), (length - idealDistance) * stiffness);
    goalFlag_vectorAdd(correction, direction.get(), correction);
}

/* 0215E154 */
void daGoal_Flag_c::get_cloth_anim_factor(cXyz* output, cXyz* positions, cXyz* normals, cXyz* wind,
                                          s32 column, s32 row) {
    gabi::Local<cXyz> center, scaled, correction;
    s32 index = row * 9 + column;
    center->set(positions[index].x, positions[index].y, positions[index].z);
    f32 pressure = goalFlag_vectorDot(wind, &normals[index]);
    if ((row == 0 || row == 4) && (column == 0 || column == 8)) {
        if (!output)
            output = static_cast<cXyz*>(goalFlag_allocateObject(12));
        if (output)
            for (u32 i = 0; i < 3; ++i)
                static_cast<be<f32>*>(static_cast<void*>(output))[i] =
                    gabi::f32_from_bits(l_zeroVectorWords[i]);
        return;
    }
    goalFlag_vectorMultiply(&normals[index], scaled.get(), pressure);
    correction->x = scaled->x;
    correction->z = scaled->z;
    correction->y = gabi::fmadds(l_HIO.mFlagSagFactor, 0.25f * (f32)row, (f32)scaled->y);
    auto spring = [&](s32 neighbor, f32 distance) {
        get_cloth_anim_sub_factor(center.get(), &positions[neighbor], correction.get(), distance);
    };
    if (column != 0) {
        spring(index - 1, 250.0f);
        if (row != 0) {
            spring(index - 9, 120.0f);
            spring(index - 10, 277.3085021972656f);
        }
        if (row != 4) {
            spring(index + 9, 120.0f);
            spring(index + 8, 277.3085021972656f);
        }
        if (column != 8) {
            spring(index + 1, 250.0f);
            if (row != 0)
                spring(index - 8, 277.3085021972656f);
            if (row != 4)
                spring(index + 10, 277.3085021972656f);
        }
    } else {
        spring(index + 1, 250.0f);
        if (row != 0) {
            spring(index - 9, 120.0f);
            spring(index - 8, 277.3085021972656f);
        }
        if (row != 4) {
            spring(index + 9, 120.0f);
            spring(index + 10, 277.3085021972656f);
        }
    }
    if (!output)
        output = static_cast<cXyz*>(goalFlag_allocateObject(12));
    if (output)
        output->set(correction->x, correction->y, correction->z);
    return;
}

/* 0215D3C0 */
static BOOL checkCreateHeap(fopAc_ac_c* actor) { return static_cast<daGoal_Flag_c*>(actor)->CreateHeap(); }

/* 0215D304 */
BOOL daGoal_Flag_c::CreateHeap() {
    for (s32 rope = 0; rope < mNumRopes; ++rope) {
        u16 points = (u16)(mNumRopeBuoys[rope] * 4 + 1);
        if (!goalFlag_lineInit(&mRopeLines[rope], 1, points, 0))
            return FALSE;
    }
    return TRUE;
}

/* 0215F3B4 */
BOOL daGoal_Flag_c::_delete() {
    u32 parameter = mParameters;
    goalFlag_resourceDelete(&mClothPhs, l_clothDeleteName);
    char* name = sub_arcname_tbl[parameter & 1].get();
    goalFlag_resourceDelete(&mFlagPhs, name);
    u32 play = goalFlag_playObject();
    if ((u8)goalFlag_gameInfo(play)->play.mMiniGameType == 1) {
        play = goalFlag_playObject();
        if ((u8)goalFlag_gameInfo(play)->play.mMinigameResult == 1) {
            l_minigameResult = 2;
            play = goalFlag_playObject();
            l_minigameRupee = (s16)goalFlag_gameInfo(play)->play.mMiniGameRupee;
        } else {
            play = goalFlag_playObject();
            if ((u8)goalFlag_gameInfo(play)->play.mMinigameResult == 2) {
                l_minigameResult = 1;
                play = goalFlag_playObject();
                l_minigameRupee = (s16)goalFlag_gameInfo(play)->play.mMiniGameRupee;
            }
        }
        play = goalFlag_playObject();
        u16 state = (u16)goalFlag_gameInfo(play)->play.mMinigameFlags;
        goalFlag_gameInfo(play)->play.mMinigameResult = 0;
        goalFlag_gameInfo(play)->play.mMiniGameType = 0;
        goalFlag_gameInfo(play)->play.mMinigameFlags = state ^ 1;
    }
    u32 singleton = gabi::ea(l_minigameScreens.get());
    void* terminater = goalFlag_screens(singleton)->mpTerminator.get();
    if (terminater) {
        goalFlag_terminator(terminater)->mReset = 0;
        goalFlag_startSequence(goalFlag_terminator(terminater)->mSequence, &l_endSequence);
        singleton = gabi::ea(l_minigameScreens.get());
    }
    void* another = goalFlag_screens(singleton)->mpFailure.get();
    if (another)
        goalFlag_resetFailureSequence(another);
    return TRUE;
}

/* 02160788 */
s32 daGoal_Flag_c::getDemoAction(s32 staff) {
    u32 play = goalFlag_playObject();
    return goalFlag_eventActionIndex(&goalFlag_gameInfo(play)->play.mEvtManager, staff, ActionNames, 5, 0, 0);
}

/* 021607D4 */
BOOL daGoal_Flag_c::RaceStart() {
    s32 staff = goalFlag_eventStaffId(Goal_eventManager(), l_startStaff, 0, 0);
    s32 action = this->getDemoAction(staff);
    if (!mRaceStartState) {
        if (staff == -1 || action != 0)
            return TRUE;
        Goal_cutEnd(staff);
        mRaceStartState = mRaceStartState + 1;
        return TRUE;
    }
    void* timer = goalFlag_messageById((s32)mTimerProcID);
    u32 singleton = gabi::ea(l_minigameScreens.get());
    void* starter = goalFlag_screens(singleton)->mpStarter.get();
    goalFlag_playObject(); // HD retains this accessor before the starter action.
    if (action == 1 && starter) {
        goalFlag_startSequence(goalFlag_starter(starter)->mSequence, &l_startSequence);
        Goal_cutEnd(staff);
    }
    if (timer && starter && (u8)goalFlag_starter(starter)->mStatus) {
        Goal_cutEnd(staff);
        goalFlag_timerStart(timer, 7);
        if (mRaceStartState == 1) {
            goalFlag_bgmStart(0x8000000Bu);
            mRaceStartState = mRaceStartState + 1;
        }
    }
    if (goalFlag_eventEndCheck(Goal_eventManager(), l_startEvent)) {
        mActionDispatch = -1;
        mActionTarget = 0x02160954;
        mRaceStartState = 0;
        mActionAdjustment = 0;
    }
    return TRUE;
}

/* 02160954 */
BOOL daGoal_Flag_c::TimerExecute() {
    void* timer = goalFlag_messageById((s32)mTimerProcID);
    if (!timer)
        goalFlag_assertionFail(l_timerFile, 0x59B, l_timerAssert);
    s32 crossing = this->goal_check();
    if (crossing == 1) {
        if (mRaceEndState == 1) {
            if (!mTimerEnded) {
                mTimerEnded = 1;
                goalFlag_timerEnd(timer, -1);
                u32 play = goalFlag_playObject();
                mRaceEndState = (s16)goalFlag_gameInfo(play)->play.mMiniGameRupee == 0 ? 2 : 3;
            }
        } else
            mRaceEndState = 1;
    } else if (crossing == -1 && !mTimerEnded) {
        mTimerEnded = 1;
        goalFlag_timerEnd(timer, -1);
        goalFlag_soundStart(0x847);
        mRaceEndState = 2;
    }
    if (goalFlag_timerRemaining(timer) <= 0 && !mTimerEnded) {
        mTimerEnded = 1;
        goalFlag_timerEnd(timer, -1);
        if (mRaceEndState != 3) {
            mRaceEndState = 2;
            goalFlag_soundStart(0x847);
        }
    }
    s16 state = mRaceEndState;
    if (state != 3 && state != 2)
        return TRUE;
    if (eventInfo.mCommand != dEvtCmd_INDEMO_e) {
        char* name = l_orderEventNames[state != 3].get();
        goalFlag_orderOtherEvent(this, name, 1, 0xFFFF);
        eventInfo.mCondition = eventInfo.mCondition | dEvtCnd_UNK2_e;
        return TRUE;
    }
    goalFlag_timerHide(timer);
    s32 finish = 0;
    if (mRaceEndState == 3) {
        u32 play = goalFlag_playObject();
        finish = (s16)goalFlag_gameInfo(play)->play.mMiniGameRupee < l_HIO.mPerfectRupeeScore ? 1 : 2;
        play = goalFlag_playObject();
        goalFlag_gameInfo(play)->play.mMinigameResult = 1;
    } else {
        u32 play = goalFlag_playObject();
        goalFlag_gameInfo(play)->play.mMinigameResult = 2;
        goalFlag_playObject();
    }
    goalFlag_bgmStart((finish == 1 || finish == 2) ? 0x8000000Cu : 0x8000000Du);
    u32 play = goalFlag_playObject();
    goalFlag_gameInfo(play)->play.mPlayerStatus[0][0] =
        (u32)goalFlag_gameInfo(play)->play.mPlayerStatus[0][0] & ~0x00200000u;
    s16 finalState = mRaceEndState;
    u32 singleton = gabi::ea(l_minigameScreens.get());
    if (finalState == 3) {
        void* terminater = goalFlag_screens(singleton)->mpTerminator.get();
        if (terminater)
            goalFlag_finishSequence(terminater, finish);
    } else {
        void* failure = goalFlag_screens(singleton)->mpFailure.get();
        if (failure)
            goalFlag_failure(failure)->mShow = 1;
    }
    mCamFramesPassed = 0;
    mActionDispatch = -1;
    mActionTarget = 0x02160C50;
    mActionAdjustment = 0;
    mRaceStartState = 0;
    return TRUE;
}

/* 02160C50 */
BOOL daGoal_Flag_c::RaceEnd() {
    s32 id = mTimerProcID;
    mCamFramesPassed = (s16)(mCamFramesPassed + 1);
    if (id != -1) {
        void* timer = goalFlag_messageById(id);
        if (timer && goalFlag_timerDeleteCheck(timer)) {
            goalFlag_messageDelete(timer);
            mTimerProcID = -1;
        }
    }
    s32 staff = goalFlag_eventStaffId(Goal_eventManager(), l_endStaff, 0, 0);
    if (staff != -1)
        Goal_cutEnd(staff);
    char* name = l_endEventNames[mRaceEndState != 3].get();
    bool end = goalFlag_eventEndCheck(Goal_eventManager(), name) != 0;
    if (!end && mCamFramesPassed > l_HIO.mEndCamEarlyFrame) {
        end = goalFlag_triggerA(0) != 0;
        if (!end)
            end = goalFlag_triggerB(0) != 0;
        if (!end)
            end = goalFlag_triggerStart(0) != 0;
    }
    if (end)
        goalFlag_nextStage(l_nextStage, 1, 0x30, -1, 0, 1, 0, 0.0f);
    return TRUE;
}

/* 0215F748 */
BOOL daGoal_Flag_c::_execute() {
    if (mActionDispatch)
        (this->*mCurrProc)();
    this->flag_move();
    this->RopeMove();
    return TRUE;
}

/* 0215FE9C */
BOOL daGoal_Flag_c::_draw() {
    gabi::Local<be<f32>[12]> identity;
    gabi::Local<be<u8>[4]> color;
    void* light = goalFlag_environmentLight();
    void* tev = &tevStr;
    goalFlag_settingTevStruct(light, 0, &current.pos, tev);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    goalFlag_matrixTranslate(0, x, y, z);
    goalFlag_matrixYRotate(l_calc_mtx.get(), current.angle.y);
    x = scale.x;
    y = scale.y;
    z = scale.z;
    goalFlag_matrixScale(1, x, y, z);
    goalFlag_matrixIdentity(identity.get());
    goalFlag_matrixConcat(identity.get(), l_calc_mtx.get(), &mFlagPacket.mMtx);
    mFlagPacket.mpTevStr = tev;
    goalFlag_drawBufferEntry(l_opaqueDrawBuffer.get(), &mFlagPacket, 0);
    mFlagPacket.update();
    for (s32 rope = 0; rope < mNumRopes; ++rope) {
        u16 count = (u16)(mNumRopeBuoys[rope] * 4 + 1);
        (*color)[0] = (u8)l_HIO.mRopeColorR;
        (*color)[1] = (u8)l_HIO.mRopeColorG;
        (*color)[2] = (u8)l_HIO.mRopeColorB;
        (*color)[3] = 255;
        void* line = &mRopeLines[rope];
        goalFlag_lineUpdate(line, count, color.get(), 0, tev, 20.0f);
        u32 play = goalFlag_playObject();
        s32 type = goalFlag_ropeLine(line)->getMaterialType();
        goalFlag_linePacketSetMaterial(
            reinterpret_cast<GoalDrawList*>(&goalFlag_gameInfo(play)->drawlist)->mLineMaterials[(u32)type],
            line);
    }
    return TRUE;
}

/* 02160FC0 */
static BOOL daGoal_FlagIsDelete(void* actor) { return TRUE; }

/* 0215D3C4 */
void daGFlag_packet_c::init() {
    // HD: initialize the shader archive, paired vertex textures and native index buffer.
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_addr(l_shaderName);
    name->__vtbl = l_safeStringVtable;
    void* archive = goalFlag_shaderArchive(this);
    s32 index = goalFlag_shaderIndex(goalFlag_archive(archive)->mpRoot.get(), name.get());
    void* resource = nullptr;
    if (index >= 0) {
        u32 count = goalFlag_archive(archive)->mCount;
        GoalShaderEntry* data = goalFlag_archive(archive)->mpEntries.get();
        GoalShaderEntry* entry = goalFlag_guestAddress(data + ((u32)index < count ? (u32)index : 0));
        if (!(u8) static_cast<GoalShaderEntry*>(entry)->mInitialized) {
            void* root = goalFlag_archive(archive)->mpRoot.get();
            u32 max = (u32) static_cast<GoalShaderRoot*>(root)->mCount;
            void* shader =
                (u32)index < max
                    ? goalFlag_guestAddress(static_cast<GoalShaderRoot*>(root)->mpShaders.get() + (u32)index)
                    : nullptr;
            goalFlag_shaderResourceInitialize(entry, shader, 0);
            count = goalFlag_archive(archive)->mCount;
            data = goalFlag_archive(archive)->mpEntries.get();
        }
        resource = goalFlag_guestAddress(data + ((u32)index < count ? (u32)index : 0));
    }
    {
        gabi::Local<be<u32>[4]> callFrame;
        goalFlag_materialTableInitialize(&mMaterialCount, resource, 0);
    }
    mVertexBuffers.mStride = 0x13;
    mVertexBuffers.mpVertexFormat = &l_vertexFormat;
    void* vertices = goalFlag_guestAddress(&mVertexBuffers);
    for (u32 side = 0; side < 2; ++side)
        for (u32 pair = 0; pair < 2; ++pair) {
            void* buffer = &goalFlag_vertexBuffers(vertices)->mBuffers[pair][side];
            u32 vertexData = gabi::ea(goalFlag_vertexBuffer(buffer)->mpData.get());
            if (!vertexData) {
                void* allocation = Goal_allocate(0x5A0, 0x40);
                if (allocation) {
                    goalFlag_vertexBuffer(buffer)->mpAllocation = allocation;
                    goalFlag_vertexBuffer(buffer)->mVertexCount = 45;
                }
                vertexData = gabi::ea(goalFlag_vertexBuffer(buffer)->mpAllocation.get());
                goalFlag_vertexBuffer(buffer)->mpData = goalFlag_vertex(vertexData);
            }
            goalFlag_vertexBufferInitialize(&goalFlag_vertexBuffer(buffer)->mNativeVertexBuffer,
                                            goalFlag_vertex(vertexData), 45,
                                            &goalFlag_vertexBuffers(vertices)->mStride);
        }
    goalFlag_vertexBuffers(vertices)->mFlags = 0;
    goalFlag_vertexBuffers(vertices)->mInitialized = 1;
    for (u32 material = 0; material < (u32)mMaterialCount; ++material) {
        u32 data = gabi::ea(mpTexturePairs.get());
        if (material < (u32)mTexturePairCount)
            data += material * 0x14;
        void* pair = &gabi::object_at<GoalTexturePair>(data);
        u32 materialId = (u32)goalFlag_texturePair(pair)->mMaterialId;
        Goal_releaseTexturePair(pair);
        goalFlag_texturePair(pair)->mMaterialId = materialId;
        for (u32 countOffset : {4u, 12u}) {
            void* textures = Goal_allocate(0x1E8, 4);
            for (u32 i = 0; i < 2; ++i)
                if (goalFlag_guestAddress(static_cast<GoalTexture*>(textures) + i))
                    goalFlag_textureConstruct(goalFlag_guestAddress(static_cast<GoalTexture*>(textures) + i));
            if (textures) {
                if (countOffset == 4) {
                    goalFlag_texturePair(pair)->mpFrontTextures = static_cast<GoalTexture*>(textures);
                    goalFlag_texturePair(pair)->mFrontCount = 2;
                } else {
                    goalFlag_texturePair(pair)->mpBackTextures = static_cast<GoalTexture*>(textures);
                    goalFlag_texturePair(pair)->mBackCount = 2;
                }
            }
        }
        for (u32 side = 0; side < 2; ++side)
            for (u32 pairIndex = 0; pairIndex < 2; ++pairIndex) {
                u32 countOffset = 4 + side * 8;
                void* texture =
                    goalFlag_guestAddress((side ? goalFlag_texturePair(pair)->mpBackTextures
                                                : goalFlag_texturePair(pair)->mpFrontTextures)
                                              .get() +
                                          (pairIndex < (u32)(side ? goalFlag_texturePair(pair)->mBackCount
                                                                  : goalFlag_texturePair(pair)->mFrontCount)
                                               ? pairIndex
                                               : 0));
                goalFlag_texturePairInitialize(
                    materialId, texture,
                    &goalFlag_vertexBuffers(vertices)->mBuffers[pairIndex][side].mNativeVertexBuffer,
                    &goalFlag_vertexBuffers(vertices)->mStride, 0);
            }
    }
    goalFlag_shaderMaterialInitialize(goalFlag_guestAddress(&mShaderMaterialCount), 1, 0);
    goalFlag_indexBufferInitialize(goalFlag_guestAddress(&mIndices), &l_flagIndices, 4, 0x48);
    mIndices.mMode = 6;
    for (u32 side = 0; side < 2; ++side) {
        u32 active = (u32)goalFlag_vertexBuffers(vertices)->mCurrentBuffer;
        u32 destination = gabi::ea(goalFlag_vertexBuffers(vertices)->mBuffers[side][active].mpData.get());
        u32 end = destination + 0x5A0;
        for (u32 block = destination; block < end; block += sizeof(GoalCacheLine))
            for (auto& word : gabi::object_at<GoalCacheLine>(block & ~31u).mWords)
                word = 0;
        destination = gabi::ea(goalFlag_vertexBuffers(vertices)
                                   ->mBuffers[side][(u32)goalFlag_vertexBuffers(vertices)->mCurrentBuffer]
                                   .mpData.get());
        for (u32 vertex = 0; vertex < 45; ++vertex) {
            GoalVertex* target = goalFlag_vertex(destination + vertex * sizeof(GoalVertex));
            f32 x = (f32)l_pos[vertex].x, y = (f32)l_pos[vertex].y, z = (f32)l_pos[vertex].z;
            target->mPosition[0] = x;
            target->mNormal[0] = 0;
            target->mPosition[2] = z;
            target->mNormal[1] = 0;
            target->mPosition[1] = y;
            target->mNormal[2] = 0;
            target->mTexCoord[0] = (f32)l_texCoord[vertex][0];
            target->mTexCoord[1] = (f32)l_texCoord[vertex][1];
        }
    }
    u32 active = (u32)goalFlag_vertexBuffers(vertices)->mCurrentBuffer, inactive = active == 0 ? 1 : 0;
    for (u32 side = 0; side < 2; ++side) {
        for (u32 vertex = 0; vertex < 45; ++vertex) {
            GoalVertex* source = goalFlag_guestAddress(
                goalFlag_vertexBuffers(vertices)->mBuffers[side][active].mpData.get() + vertex);
            GoalVertex* target = goalFlag_guestAddress(
                goalFlag_vertexBuffers(vertices)->mBuffers[side][inactive].mpData.get() + vertex);
            for (u32 i = 0; i < 8; ++i)
                target->mComponents[i] = source->mComponents[i];
        }
        active = (u32)goalFlag_vertexBuffers(vertices)->mCurrentBuffer;
    }
    u32 flushIndex = goalFlag_vertexBuffers(vertices)->mCurrentBuffer;
    for (u32 side = 0; side < 2; ++side) {
        goalFlag_vertexBufferFlush(
            &goalFlag_vertexBuffers(vertices)->mBuffers[side][flushIndex].mNativeVertexBuffer, 0,
            goalFlag_vertexBuffers(vertices)->mBuffers[side][flushIndex].mNativeVertexBuffer.mFlushSize);
    }
    goalFlag_vertexBuffers(vertices)->mCurrentBuffer =
        (u32)goalFlag_vertexBuffers(vertices)->mCurrentBuffer == 0 ? 1 : 0;
}

/* 0215F9E4 */
void daGFlag_packet_c::update() {
    // HD: upload animated cloth vertices, lighting uniforms and the toon texture.
    gabi::Local<SafeString> name;
    gabi::Local<be<f32>[12]> matrix;
    gabi::Local<be<f32>[4]> color, scaledColor;
    goalFlag_graphicsLock(l_graphicsLock.get());
    if (!(u8)mToonLoaded) {
        name->mStringTop = gabi::guest_addr(l_toonArchiveName);
        name->__vtbl = l_safeStringVtable;
        void* resource = goalFlag_objectResource(l_resourceControl.get(), name.get(), 3);
        if (!resource)
            goalFlag_assertionFail(l_toonFile, 0x236, l_toonResourceAssert);
        goalFlag_textureResourceLoad(&mToonResource, goalFlag_resource(resource)->mpTexture.get());
        bool equal = true;
        const be<u32> GoalTextureDescription::* fields[] = {
            &GoalTextureDescription::mDimension, &GoalTextureDescription::mWidth,
            &GoalTextureDescription::mHeight,    &GoalTextureDescription::mDepth,
            &GoalTextureDescription::mMipLevels, &GoalTextureDescription::mFormat,
            &GoalTextureDescription::mAlignment, &GoalTextureDescription::mPitch,
            &GoalTextureDescription::mTileMode};
        for (auto field : fields)
            if (mToonTexture.mDescription.*field != mToonResource.mDescription.*field) {
                equal = false;
                break;
            }
        if (!equal) {
            goalFlag_textureAssign(goalFlag_guestAddress(&mToonTexture),
                                   goalFlag_guestAddress(&mToonResource));
            u8 flags = (u8)mToonTexture.mFlags | 2;
            mToonTexture.mMipDirty = 2;
            mToonTexture.mImageDirty = 2;
            mToonTexture.mSamplerDirty = 2;
            mToonTexture.mFlags = flags;
        } else {
            void* image = mToonResource.mDescription.mpImage.get();
            void* mips = mToonResource.mDescription.mpMipImage.get();
            mToonTexture.mpNativeImage = image;
            u8 flags = (u8)mToonTexture.mFlags | 2;
            mToonTexture.mDescription.mpMipImage = mips;
            mToonTexture.mpNativeMipImage = mips;
            mToonTexture.mImageDirty = 2;
            mToonTexture.mMipDirty = 2;
            mToonTexture.mFlags = flags;
            mToonTexture.mDescription.mpImage = image;
            mToonTexture.mSamplerDirty = 2;
        }
        mToonLoaded = 1;
    }
    goalFlag_graphicsUnlock(l_graphicsLock.get());
    void* vertices = goalFlag_guestAddress(&mVertexBuffers);
    for (u32 side = 0; side < 2; ++side) {
        u32 destination = gabi::ea(goalFlag_vertexBuffers(vertices)
                                       ->mBuffers[side][(u32)goalFlag_vertexBuffers(vertices)->mCurrentBuffer]
                                       .mpData.get());
        for (u32 vertex = 0; vertex < 45; ++vertex) {
            cXyz* position = &mPos[(u32)mCurrArr][vertex];
            f32 x = position->x, y = position->y, z = position->z;
            GoalVertex* target = goalFlag_vertex(destination + vertex * sizeof(GoalVertex));
            target->mPosition[0] = x;
            target->mPosition[1] = y;
            target->mPosition[2] = z;
            cXyz* normal = side ? &mBackNrm[(u32)mCurrArr][vertex] : &mNrm[(u32)mCurrArr][vertex];
            x = normal->x;
            y = normal->y;
            z = normal->z;
            target->mNormal[2] = z;
            target->mNormal[0] = x;
            target->mNormal[1] = y;
        }
    }
    u32 flushIndex = goalFlag_vertexBuffers(vertices)->mCurrentBuffer;
    for (u32 side = 0; side < 2; ++side)
        goalFlag_vertexBufferFlush(
            &goalFlag_vertexBuffers(vertices)->mBuffers[side][flushIndex].mNativeVertexBuffer, 0,
            goalFlag_vertexBuffers(vertices)->mBuffers[side][flushIndex].mNativeVertexBuffer.mFlushSize);
    goalFlag_vertexBuffers(vertices)->mCurrentBuffer =
        (u32)goalFlag_vertexBuffers(vertices)->mCurrentBuffer == 0 ? 1 : 0;
    goalFlag_setFog(mpTevStr.get());
    goalFlag_setLights(mpTevStr.get());
    goalFlag_copyMatrix(matrix.get(), &l_j3dSys.mViewMtx);
    goalFlag_materialMatrices(goalFlag_guestAddress(&mShaderMaterialCount), 0, matrix.get(),
                              &l_j3dSys.mProjection, &l_j3dSys.mpView->mProjection);
    for (u32 lightIndex = 0; lightIndex < 2; ++lightIndex) {
        void* tev = mpTevStr.get();
        u32 material = gabi::ea(mpShaderMaterials.get());
        goalFlag_colorS10ToFloat(color.get(), lightIndex ? goalFlag_tevColors(tev)->mSecondLightColor
                                                         : goalFlag_tevColors(tev)->mMaterialColor);
        tev = mpTevStr.get();
        goalFlag_colorMultiply(scaledColor.get(), color.get(),
                               (f32)(lightIndex ? goalFlag_tevColors(tev)->mSecondLightScale
                                                : goalFlag_tevColors(tev)->mMaterialScale));
        for (u32 component = 0; component < 4; ++component)
            gabi::object_at<GoalShaderMaterial>(material).mLightColors[lightIndex][component] =
                (*reinterpret_cast<be<u32>(*)[4]>(scaledColor.get()))[component];
    }
    goalFlag_materialFlush(goalFlag_guestAddress(&mShaderMaterialCount), 0);
    void* tev = mpTevStr.get();
    goalFlag_colorS10ToFloat(goalFlag_guestAddress(&mMaterialUniforms.mMaterialColor),
                             goalFlag_tevColors(tev)->mMaterialColor);
    goalFlag_colorToFloat(goalFlag_guestAddress(&mMaterialUniforms.mAmbientColor),
                          goalFlag_tevColors(tev)->mAmbientColor);
    void* currentTev = mpTevStr.get();
    goalFlag_colorScale(goalFlag_guestAddress(&mMaterialUniforms.mAmbientColor),
                        (f32)goalFlag_tevColors(currentTev)->mAmbientScale);
    if ((u8)goalFlag_tevColors(tev)->mEffectColor[3])
        goalFlag_colorToFloat(goalFlag_guestAddress(&mMaterialUniforms.mEffectColor),
                              goalFlag_tevColors(tev)->mEffectColor);
    else
        for (u32 component : {1u, 0u, 2u, 3u})
            mMaterialUniforms.mEffectColor[component] = 0;
    goalFlag_uniformBlockFlush(goalFlag_guestAddress(&mMaterialBlock));
    goalFlag_copyMatrix(matrix.get(), goalFlag_guestAddress(&mMtx));
    goalFlag_matrixCopy(matrix.get(), goalFlag_guestAddress(&mShaderMatrix));
    goalFlag_uniformBlockFlush(goalFlag_guestAddress(&mMatrixBlock));
}

// HD: compiler-generated and template code of this translation unit.

/* 0215F7CC */
static void goalFlag_copyMatrix(void* destination, void* source) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<be<f32>*>(source)[i];
    for (u32 i = 0; i < 12; ++i)
        static_cast<be<f32>*>(destination)[i] = values[i];
}

/* 0215F86C */
static void goalFlag_colorS10ToFloat(void* destination, void* source) {
    f32 values[4];
    for (u32 i = 0; i < 4; ++i)
        values[i] = (f32) static_cast<be<s16>*>(source)[i] / 255.0f;
    for (u32 i = 0; i < 4; ++i)
        static_cast<be<f32>*>(destination)[i] = values[i];
}

/* 0215F930 */
static void goalFlag_colorToFloat(void* destination, void* source) {
    f32 values[4];
    for (u32 i = 0; i < 4; ++i)
        values[i] = (f32) static_cast<be<u8>*>(source)[i] / 255.0f;
    for (u32 i = 0; i < 4; ++i)
        static_cast<be<f32>*>(destination)[i] = values[i];
}

/* 02160F24 */
static void* GoalVertexBuffer_ctor(void* buffer) {
    if (!buffer)
        buffer = goalFlag_allocateObject(0x254);
    if (buffer) {
        goalFlag_vertexBufferConstruct(&goalFlag_vertexBuffer(buffer)->mNativeVertexBuffer);
        goalFlag_textureConstruct(&goalFlag_vertexBuffer(buffer)->mTexture);
        goalFlag_vertexBuffer(buffer)->mpAllocation = nullptr;
        goalFlag_vertexBuffer(buffer)->mVertexCount = 0;
    }
    return buffer;
}

/* 02160F80 */
static void* GoalBufferEntry_ctor(void* entry) { return entry ? entry : goalFlag_allocateObject(0x10); }

/* 02160FAC */
static void GoalBufferEntry_dtor(void* entry, s32 flags) {
    if (entry && (flags & 1))
        goalFlag_deleteObject(entry);
}

/* 02160FC8 */
static void GoalVertexBuffer_dtor(void* buffer, s32 flags) {
    if (buffer) {
        goalFlag_textureDestroy(&goalFlag_vertexBuffer(buffer)->mTexture, 2);
        goalFlag_vertexBufferDestroy(&goalFlag_vertexBuffer(buffer)->mNativeVertexBuffer, 2);
        if (flags & 1)
            goalFlag_deleteObject(buffer);
    }
}

/* 021618A4 */
static void daGFlag_HIO_c_dtor(daGFlag_HIO_c* hio, s32 flags) {
    if (hio) {
        hio->m04 = 0xFF;
        hio->__vtbl = l_hio_vtable;
        if (flags & 1)
            goalFlag_deleteObject(hio);
    }
}

/* 021618CC */
static void daGFlag_HIO_c_callback(void* hio) {}

/* 02160DA4 */
static void __sinit_d_a_goal_flag() {
    for (s32 i = 3; i >= 0; --i)
        l_staticDtorSlots[i] = 0;
    goalFlag_registerGlobalObject(&l_globalDtorRecords[0]);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    goalFlag_staticByteConstruct(&l_staticByte);
    goalFlag_registerGlobalObject(&l_globalDtorRecords[3]);
    goalFlag_staticByteFlagConstruct(&l_staticByteFlag);
    goalFlag_registerGlobalObject(&l_globalDtorRecords[6]);

    l_HIO.m04 = 0xFF;
    l_ropeCullMax = 10000.0f;
    l_HIO.mDrawCelShaded = 1;
    l_HIO.__vtbl = l_hio_vtable;
    l_ropeCullMin = 10000.0f;
    l_HIO.mWindScalePhaseInc = 750;
    l_flagCullMax = 50000.0f;
    l_HIO.mNormalFlutterPhaseInc = 0x800;
    l_HIO.mFlagWavePhaseInc = 0xBD1;
    l_HIO.mTimeLimit = 240;
    l_HIO.mPerfectRupeeScore = 150;
    l_HIO.mEndCamEarlyFrame = 150;
    l_HIO.mWindScale1 = 12.0f;
    l_HIO.mRopeColorR = 255;
    l_HIO.mRopeColorG = 128;
    l_HIO.mRopeColorB = 128;
    l_HIO.m07 = 0;
    l_HIO.m06 = 0;
    l_HIO.m08 = 0;
    l_HIO.m09 = 0;
    l_HIO.mDebugFlag = 0;
    l_HIO.mWindScale2 = 4.0f;
    l_HIO.mFlagSagFactor = -1.0f;
    l_flagCullMin = 50000.0f;
    l_HIO.mClothStiffness = 0.4f;
    goalFlag_registerGlobalObject(&l_globalDtorRecords[9]);
}

/* 02161028 */
static void daGFlag_packet_c_dtor(void* packet, s32 flags) {
    if (packet) {
        goalFlag_packet(packet)->mVtable = l_packetVtable;
        Goal_releasePacketMembers(packet);
        goalFlag_packetBaseDestroy(packet, 0);
        if (flags & 1)
            goalFlag_deleteObject(packet);
    }
}

/* 02161454 */
static void daGoal_Flag_c_dtor(void* actor, s32 flags) {
    if (actor) {
        goalFlag_arrayDestroy(static_cast<daGoal_Flag_c*>(actor)->mRopeLines, 4, 0x148,
                              goalFlag_ropeLineDestroy, 0, 0);
        void* packet = goalFlag_guestAddress(&static_cast<daGoal_Flag_c*>(actor)->mFlagPacket);
        goalFlag_packet(packet)->mVtable = l_packetVtable;
        Goal_releasePacketMembers(packet);
        goalFlag_packetBaseDestroy(packet, 0);
        goalFlag_actorBaseDestroy(actor, 0);
        if (flags & 1)
            goalFlag_deleteObject(actor);
    }
}

/* 02161450 */
static void daGFlag_packet_c_empty(void* packet) {}

/* 0215D9D4 */
static void* daGFlag_packet_c_ctor(void* packet) {
    if (!packet)
        packet = goalFlag_allocateObject(0x264C);
    if (!packet)
        return nullptr;
    goalFlag_packetBaseConstruct(packet);
    goalFlag_packet(packet)->mNormalFlutterPhase = 0;
    goalFlag_packet(packet)->mMaterialCount = 0;
    goalFlag_packet(packet)->mVtable = l_packetVtable;
    void* table = goalFlag_guestAddress(&goalFlag_packet(packet)->mTexturePairCount);
    if (!table)
        table = goalFlag_allocateObject(8);
    if (table) {
        goalFlag_textureTable(table)->mpPairs = nullptr;
        goalFlag_textureTable(table)->mCount = 0;
    }
    void* vertices = goalFlag_guestAddress(&goalFlag_packet(packet)->mVertexBuffers);
    if (!vertices)
        vertices = goalFlag_allocateObject(0x968);
    if (vertices) {
        goalFlag_arrayConstruct(vertices, 4, 0x254, GoalVertexBuffer_ctor);
        goalFlag_vertexBuffers(vertices)->mCurrentBuffer = 0;
        goalFlag_vertexBuffers(vertices)->mFlags = 0;
        goalFlag_vertexBuffers(vertices)->mAlignment = 0x20;
        goalFlag_vertexBuffers(vertices)->mStride = 0;
        goalFlag_vertexBuffers(vertices)->mInitialized = 0;
        for (auto& pair : goalFlag_vertexBuffers(vertices)->mBuffers)
            for (auto& buffer : pair)
                buffer.mpData = nullptr;
    }
    goalFlag_materialOwnerConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mShaderMaterialCount));
    goalFlag_uniformBlockConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mMatrixBlock));
    goalFlag_packet(packet)->mMatrixBlock.mVtable = &l_matrixUniformVtable;
    goalFlag_memoryClear(goalFlag_guestAddress(&goalFlag_packet(packet)->mShaderMatrix), 0x34);
    if (!goalFlag_guestAddress(&goalFlag_packet(packet)->mShaderMatrix))
        goalFlag_allocateObject(0x30);
    goalFlag_uniformBlockConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mMaterialBlock));
    goalFlag_packet(packet)->mMaterialBlock.mVtable = &l_materialUniformVtable;
    goalFlag_memoryClear(goalFlag_guestAddress(&goalFlag_packet(packet)->mMaterialUniforms), 0x2F0);
    // Identity matrices and default material vectors in the shader uniform block.
    const u32 zeroComponents[] = {0,  1, 2,  4,  14, 6,  26, 34, 22, 12, 21, 33, 29, 17, 8,  13, 9,
                                  18, 5, 28, 30, 25, 20, 32, 10, 24, 16, 36, 37, 38, 40, 41, 42};
    f32 zero = (f32)l_zero, one = (f32)l_one;
    // HD: retain the compiler's uniform-default store order.
    for (u32 component : zeroComponents)
        goalFlag_packet(packet)->mMaterialUniforms.mInitialValues[component] = zero;
    for (u32 component : {11u, 7u, 31u, 15u, 35u, 23u, 27u, 3u, 19u, 39u, 43u})
        goalFlag_packet(packet)->mMaterialUniforms.mInitialValues[component] = one;
    for (auto& entries : goalFlag_packet(packet)->mMaterialUniforms.mBufferEntries)
        goalFlag_arrayConstruct(goalFlag_guestAddress(&entries), 2, 0x10, GoalBufferEntry_ctor);
    for (auto& matrix : goalFlag_packet(packet)->mMaterialUniforms.mMatrices)
        if (!goalFlag_guestAddress(&matrix))
            goalFlag_allocateObject(0x30);
    for (auto& vector : goalFlag_packet(packet)->mMaterialUniforms.mVectors)
        if (!goalFlag_guestAddress(&vector))
            goalFlag_allocateObject(0x10);
    goalFlag_packet(packet)->mFlagLoaded = 0;
    goalFlag_indexBufferConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mIndices));
    goalFlag_textureResourceConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mFlagResource));
    goalFlag_textureResourceConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mToonResource));
    goalFlag_textureObjectConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mFlagTexture));
    goalFlag_textureObjectConstruct(goalFlag_guestAddress(&goalFlag_packet(packet)->mToonTexture));
    goalFlag_packet(packet)->mToonLoaded = 0;
    goalFlag_packet(packet)->mCurrArr = 0;
    goalFlag_packetInitialize(packet);
    return packet;
}

// HD: external rope constructor/destructor callbacks need their original guest entries.
static gabi::HostFn l_ropeCtorEntry((const void*)&goalFlag_ropeLineConstruct, 0x025E9960);
static gabi::HostFn l_ropeDtorEntry((const void*)&goalFlag_ropeLineDestroy, 0x025E99E0);

/* WWHD glue
data l_pos 101B5974
data l_texCoord 101B5B90
data down_offset 100104C0
data l_flagIndices 10010394
data sub_arcname_tbl 101B594C
data ActionNames 101B5CF8
data l_endEventNames 101B5D14
data l_orderEventNames 101B5D0C
ext 025E9960 void* goalFlag_ropeLineConstruct(void*)
ext 025E99E0 void goalFlag_ropeLineDestroy(void*,s32)
str 100102FC in 0215F3B4
str 1001030C in 0215EDF8
str 10010314 in 0215EDF8
str 1001031C
str 1001032C
str 10010428
str 1001043C in 0215DFB4
str 10010450 in 0215DFB4
str 100104B0
str 100104D0
str 100104F8 in 0215F9E4
str 10010500 in 0215F9E4
str 10010514 in 0215F9E4
str 10010564 in 021607D4
str 1001056C
str 1001057C in 02160954
str 10010590
str 100105C0
str 100105C4 in 02160C50
data l_HIO 10464164
ext 027F29D4 void* goalFlag_shaderCache(void*)
ext 027BB9E0 void goalFlag_shaderRefresh(void*, s32)
ext 027B9F68 void goalFlag_nativeShaderBind(void*)
ext C00060E0 void goalFlag_fetchShaderBind(void*, void*)
ext 027BB7CC void goalFlag_shaderBind(void*)
ext 027FFE54 void goalFlag_drawInfoPrepare(void*, void*)
ext 02750250 void goalFlag_renderStateConstruct(void*)
ext 0280037C void goalFlag_renderStateConfigure(s32, void*)
ext 02750370 void goalFlag_renderStateApply(void*)
ext 027BFE5C void goalFlag_textureApply(void*)
ext C0006178 void goalFlag_drawIndexed(u32, s32, u32, void*, s32, s32)
ext 025AAF88 void * goalFlag_roomPath(u8, s8)
ext 0273AD10 void * goalFlag_allocateObject(s32)
ext 025DE50C void * goalFlag_messageById(s32)
ext 0257DAA8 void * goalFlag_windVector()
ext 02555D0C void * goalFlag_environmentLight()
ext 026066C4 void * goalFlag_objectResource(void *, SafeString *, s32)
ext 02755FEC void * goalFlag_findAllocationHeap(void *, void *)
ext 02756140 void * goalFlag_currentHeap(void *)
ext 027FFCBC void * goalFlag_shaderArchive(void *)
ext 025E9B80 s32 goalFlag_lineInit(void *, s32, u16, s32)
ext 027B5BD8 void goalFlag_vertexBufferConstruct(void *)
ext 027BF734 void goalFlag_textureConstruct(void *)
ext 0273AF40 void goalFlag_deleteObject(void *)
ext 027BF880 void goalFlag_textureDestroy(void *, s32)
ext 027B5CBC void goalFlag_vertexBufferDestroy(void *, s32)
ext 025200D4 u32 goalFlag_playObject()
ext 02542EDC s32 goalFlag_eventActionIndex(void *, s32, void *, s32, s32, s32)
ext 025D5A20 s32 goalFlag_createChild(s32, u32, s32, void *, s8, s32, s32, s32, s32)
ext 0201ADE0 void goalFlag_vectorSubtract(void *, void *, void *)
ext 0201B12C void goalFlag_vectorNormalizeZP(void *, void *)
ext 028E8DD0 f32 goalFlag_vectorSquareMagnitude(void *)
ext 028F4384 f32 goalFlag_squareRoot(f32)
ext 028E8E64 void goalFlag_vectorScale(void *, void *, f32)
ext 028E8D88 void goalFlag_vectorAdd(void *, void *, void *)
ext 028E8DAC void goalFlag_vectorSubtractPS(void *, void *, void *)
ext 028E8F44 f32 goalFlag_vectorDot(void *, void *)
ext 02543280 void goalFlag_eventCutEnd(void *, s32)
ext 02542D88 s32 goalFlag_eventStaffId(void *,const char*, s32, s32)
ext 020063C0 void goalFlag_startSequence(void *, void *)
ext 025C6250 void goalFlag_timerStart(void *, s32)
ext 025E18EC void goalFlag_bgmStart(u32)
ext 0254457C s32 goalFlag_eventEndCheck(void *,const char*)
ext 025C6278 s32 goalFlag_timerDeleteCheck(void *)
ext 025DF944 void goalFlag_messageDelete(void *)
ext 02007898 s32 goalFlag_triggerA(s32)
ext 020078BC s32 goalFlag_triggerB(s32)
ext 02007940 s32 goalFlag_triggerStart(s32)
ext 0252012C void goalFlag_nextStage(const char*, s32, s32, s32, s32, s32, s32, f32)
ext 025204C8 void goalFlag_resourceDelete(void *,const char*)
ext 026266C8 void goalFlag_resetFailureSequence(void *)
ext 0201AE48 void goalFlag_vectorMultiply(void *, void *, f32)
ext 0273AA24 void goalFlag_assertionFail(const char*, s32,const char*)
ext 0201AD78 void goalFlag_vectorSum(void *, void *, void *)
ext 025C58A8 void goalFlag_timerEnd(void *, s32)
ext 025E1988 void goalFlag_soundStart(s32)
ext 025C5814 s32 goalFlag_timerRemaining(void *)
ext 025D77DC void goalFlag_orderOtherEvent(void *, void *, s32, s32)
ext 025C577C void goalFlag_timerHide(void *)
ext 02627E14 void goalFlag_finishSequence(void *, s32)
ext 0201B080 void goalFlag_vectorCross(void *, void *, void *)
ext 0201B1E4 void goalFlag_vectorNormalizeZC(void *, void *)
ext 0200FCF0 void goalFlag_matrixPush()
ext 025F1C28 void goalFlag_matrixYRotate(void *, s16)
ext 025F1BF4 void goalFlag_matrixXRotate(void *, s16)
ext 0200FCD8 void goalFlag_matrixPosition(void *, void *)
ext 0200FD38 void goalFlag_matrixPull()
ext 025F1884 void goalFlag_matrixYSet(void *, s16)
ext 0201B084 void goalFlag_vectorNormalize(void *, void *)
ext C00088B8 void goalFlag_cacheStoreRange(void *, s32)
ext 028F026C void goalFlag_registerGlobalObject(void *)
ext 028ED6F8 void goalFlag_staticByteConstruct(void *)
ext 028EAB2C void goalFlag_staticByteFlagConstruct(void *)
ext 025626A4 void goalFlag_settingTevStruct(void *, s32, void *, void *)
ext 0200FAD8 void goalFlag_matrixTranslate(s32, f32, f32, f32)
ext 0200FC74 void goalFlag_matrixScale(s32, f32, f32, f32)
ext 028E9098 void goalFlag_matrixIdentity(void *)
ext 028E9108 void goalFlag_matrixConcat(void *, void *, void *)
ext 027F0E04 void goalFlag_drawBufferEntry(void *, void *, s32)
ext 025EA548 void goalFlag_lineUpdate(void *, u16, void *, s32, void *, f32)
ext 025EDD04 void goalFlag_linePacketSetMaterial(void *, void *)
ext 0274FBF8 void goalFlag_graphicsLock(void *)
ext 02773798 void goalFlag_textureResourceLoad(void *, void *)
ext 0274FCCC void goalFlag_graphicsUnlock(void *)
ext 027BDEB4 void goalFlag_textureAssign(void *, void *)
ext 027BF7E8 void goalFlag_textureRelease(void *)
ext 027FD764 void goalFlag_materialOwnerDestroy(void *, s32)
ext 027BEBEC void goalFlag_uniformBufferRelease(void *)
ext 027BE2B0 void goalFlag_textureObjectDestroy(void *, s32)
ext 027B54A0 void goalFlag_indexBufferDestroy(void *, s32)
ext 027FB528 void goalFlag_uniformBlockDestroy(void *, s32)
ext 028F0164 void goalFlag_arrayDestroy(void *, s32, s32, GoalArrayDtor, s32, s32)
ext 027F13DC void goalFlag_packetBaseDestroy(void *, s32)
ext 025D50BC void goalFlag_actorBaseDestroy(void *, s32)
ext 027F1278 void goalFlag_packetBaseConstruct(void *)
ext 028EFFD0 void goalFlag_arrayConstruct(void *, s32, s32, GoalArrayCtor)
ext 027FD6F4 void goalFlag_materialOwnerConstruct(void *)
ext 027FB40C void goalFlag_uniformBlockConstruct(void *)
ext 028F521C void goalFlag_memoryClear(void *, s32)
ext 027B5430 void goalFlag_indexBufferConstruct(void *)
ext 027BE6B8 void goalFlag_textureResourceConstruct(void *)
ext 027BDF7C void goalFlag_textureObjectConstruct(void *)
ext 0215D3C4 void goalFlag_packetInitialize(void *)
ext 027B90AC s32 goalFlag_shaderIndex(void *, SafeString *)
ext 02800B0C void goalFlag_shaderResourceInitialize(void *, void *, s32)
ext 0280068C void goalFlag_materialTableInitialize(void *, void *, s32)
ext 027FF478 void goalFlag_vertexBufferInitialize(void *, void *, s32, void *)
ext 027FF530 void goalFlag_texturePairInitialize(u32, void *, void *, void *, s32)
ext 027FE084 void goalFlag_shaderMaterialInitialize(void *, s32, s32)
ext 027B54E0 void goalFlag_indexBufferInitialize(void *, const void *, s32, s32)
ext 027B5E94 void goalFlag_vertexBufferFlush(void *, s32, u32)
ext 025D4ED0 void goalFlag_actorBaseConstruct(void *)
ext 02520460 s32 goalFlag_resourceLoad(void *,const char*)
ext 025D63E8 s32 goalFlag_entrySolidHeap(void *, void *, s32)
ext 028E93CC void goalFlag_matrixSetTranslate(void *, f32, f32, f32)
ext 025F2518 void goalFlag_matrixStackScale(f32, f32, f32)
ext 028E90D4 void goalFlag_matrixCopy(void *, void *)
ext 025B8BB0 u32 goalFlag_eventRegister(void *, s32)
ext 025DB468 s32 goalFlag_timerCreate(s32, s32, u16, s32, s32, s32, f32, f32, f32, f32)
ext 0255F8F4 void goalFlag_setFog(void *)
ext 0255FE90 void goalFlag_setLights(void *)
ext 027FDA54 void goalFlag_materialMatrices(void *, s32, void *, void *, void *)
ext 0274D458 void goalFlag_colorMultiply(void *, void *, f32)
ext 027FDFF4 void goalFlag_materialFlush(void *, s32)
ext 0274D2AC void goalFlag_colorScale(void *, f32)
ext 027FB678 void goalFlag_uniformBlockFlush(void *)
ext C0006900 void goalFlag_pixelUniformBlock(s16, u32, void *)
ext C0006A38 void goalFlag_vertexUniformBlock(s16, u32, void *)
ext C00068A8 void goalFlag_geometryUniformBlock(s16, u32, void *)
ext 027BE53C void goalFlag_textureBind(void *, void *, s32, s32)
*/
