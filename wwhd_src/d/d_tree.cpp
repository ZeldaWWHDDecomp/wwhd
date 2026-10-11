/* Tree collision, animation and HD native rendering. Verified against the original unit. */
#include "d/d_tree.h"
#include <cmath>
using namespace gabi;
static const f32 compilerPiMin = -3.1415927410125732f;
static const f32 compilerPiMax = 3.1415927410125732f;
static const f32 alphaThreshold = 0.5f;
static TreeHeaderStatics l_headerStatics;
static TreeHeaderStatics l_compilerStatics;
static cXyz l_particleOffset;
static cXyz l_scale;
static u32 l_particleOffsetReady;
static u32 l_scaleReady;
static u8 l_CutSoundFlag;
#define l_sincosTable (gabi::at<TreeTrig>(0x104A44F8))
static s8 l_stayRoom;
static TreeCounter g_Counter;
static TreeRenderGlobals treeRenderGlobals;
static TreeClipper treeClipper;
static f32 treeOriginalFar;
static u32 l_tevDefaults[17] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xBF800000,
                                0x00000000, 0xFFFFFFFF, 0x00FF00FF, 0x00FF00FF, 0x3F800000,
                                0x3F800000, 0x3F800000, 0x00000000, 0x00000000, 0x3F800000,
                                0x00000000, 0x00000000};
static TreeVertexCounts l_vertexCounts[4] = {{28, 84}, {18, 48}, {120, 324}, {34, 108}};
static u32 treeHeap;
static u32 treeTextureCache;
static u32 treeResourceManager;

static const f32 tiny = 0.000003814697265625f;
static const f32 groundRaise = 50.0f;
static const f32 groundMinimum = -1000000000.0f;
static const f32 treeZero = 0.0f;
static const f32 damping = 0.89999997615814208984375f;
static const f32 maximumSwing = 4000.0f;
static const f32 bombRange = 200.0f;
static const f32 gravity = -2.0f;
static const f32 initialCutSpeed = 12.0f;
static const f32 treeOne = 1.0f;
static const f32 swingStep = 40.0f;
static const f32 leafThreshold = 400.0f;
static const f32 leafHeight = 203.0f;
static const f32 terminalVelocity = -40.0f;
static const f32 groundOffset = -5.0f;
static const f32 cutTargetSpeed = 1500.0f;
static const f32 smokeHeight = 150.0f;
static const f32 smokeScale = 1.60000002384185791015625f;
static const f32 smokeRate = 20.0f;
static const f32 smokeSpeed = 15.0f;
static const f32 windAmplitude = 100.0f;
static const f32 windFrequency = 1000.0f;
static const f32 colorDenominator = 255.0f;
static const f32 clipFarScale = 1.81818187236785888671875f;
static const f32 clipRadius = 800.0f;
static const f32 animationRandomRange = 7.0f;
static const f32 piMin = -3.1415927410125732421875f;
static const f32 piMax = 3.1415927410125732421875f;
static const f32 nativeOne = 1.0f;
static const f32 nativeZero = 0.0f;
// HD: integer loads and stores preserve signaling-NaN payloads in matrix/vector copies.
static u32 floatBits(const be<f32>& value) { return load<u32>(ea(&value)); }

static void storeFloatBits(be<f32>& value, u32 bits) { store<u32>(ea(&value), bits); }

static bool tree_ensureAnm(u32 a) {
    s32 index = (s8)(at<dTree_data_c>(a)->mAnimIdx);
    if (index >= 8)
        return true;
    u32 game = getGameInfo();
    index = (s8)(at<dTree_data_c>(a)->mAnimIdx);
    u32 p = (at<TreeGameView>(game)->mpTree.v) + (u32)index * 0x84;
    game = getGameInfo();
    s32 next = newAnimation(at<void>((at<TreeGameView>(game)->mpTree.v)),
                            (s32)(s16)(u16)(at<dTree_anm_c>(p + 10908)->field_0x02));
    if (next < 0)
        return false;
    at<dTree_data_c>(a)->mAnimIdx = next;
    return true;
}

/* inlined into dTree_data_c::animation */
static void tree_particleState(u32 emitter, f32 one) {
    TreeParticleState state;
    state.mAllocator = 0x100560B4;
    state.mName = 0x10000160;
    for (u32 i = 8; i < 24; i += 4)
        state.mParameters[(i - 8) / 4] = one;
    state.mFlags[0] = 1;
    state.mFlags[1] = 0;
    state.mFlags[2] = 0;
    state.mFlags[3] = 0;
    u32 game = getGameInfo(), p = (at<TreeGameView>(game)->mpParticle.v);
    for (u32 i = 0; i < 4; i++)
        storeFloatBits(state.mParameters[i], at<be<u32>>(p + 0x20)[i]);
    setParticleState(at<void>(emitter), &state);
}

static void tree_freeField(be<u32>* allocation) {
    u32 heap = findAllocationHeap(at<void>(treeHeap), at<void>(allocation->get()));
    call_ptr<void>(at<TreeHeap>(heap)->__vtbl->free, at<void>(heap), at<void>(allocation->get()));
}

static void tree_releaseCollisions(u32 base) {
    for (u32 i = 0; i < 5; i++)
        for (u32 j = 0; j < 2; j++) {
            u32 p = base + i * 0x4A8 + j * 0x254;
            releaseShape(at<void>(p + 0x158));
            at<TreeCollisionBuffer>(p)->mpVertices.v = 0;
            if ((at<TreeCollisionBuffer>(p)->mpAllocation.v)) {
                tree_freeField(&at<TreeCollisionBuffer>(p)->mpAllocation.v);
                at<TreeCollisionBuffer>(p)->mCapacity = 0;
                at<TreeCollisionBuffer>(p)->mpAllocation.v = 0;
            }
        }
}

static void tree_releaseShapes(u32 list) {
    u32 array = (at<TreeMaterialList>(list)->mpRecords.v);
    if (!array)
        return;
    for (s32 i = 0; i < (s32)(at<TreeMaterialList>(list)->mCapacity); i++) {
        u32 record = array + (u32)i * 20;
        if (!record)
            continue;
        at<TreeMaterialRecord>(record)->mpMaterial.v = 0;
        for (u32 off : {4u, 12u}) {
            u32 p = (u32)at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v;
            if (!p)
                continue;
            for (s32 j = 0;
                 j < (s32)(u32)at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mCount; j++) {
                u32 object = p + (u32)j * 244;
                call_ptr<void>(at<TreeNativeShape>(object)->__vtbl->destroy, at<void>(object), 2);
                p = (u32)at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v;
            }
            tree_freeField(&at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v);
            at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mCount = 0;
            at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v = 0;
        }
        array = (at<TreeMaterialList>(list)->mpRecords.v);
    }
    tree_freeField(&at<TreeMaterialList>(list)->mpRecords.v);
    at<TreeMaterialList>(list)->mCapacity = 0;
    at<TreeMaterialList>(list)->mpRecords.v = 0;
}

static u32 tree_bindMaterial(u32 a, u32 draw, TreeMaterialList* records,
                             TreeUniformContainer* resource) {
    u32 material = 0, type = (at<TreeDrawArguments>(draw)->mMaterialType);
    if ((s32)type < 4) {
        u32 table = (u32)records->mpRecords.v;
        if (type < (u32)records->mCapacity)
            table += type * 20;
        material = at<TreeMaterialRecord>(table)->mpMaterial.v;
    }
    u32 cache = getMaterialCache(&treeRenderGlobals);
    u32 shader = (at<TreeMaterial>(material)->mpShader.v),
        old = (at<TreeMaterialCache>(cache)->mpTexture.v);
    if (shader != (at<TreeMaterialCache>(cache)->mpShader.v)) {
        if ((at<TreeShader>(shader)->mFlags) & 2) {
            at<TreeShader>(shader)->mFlags = (at<TreeShader>(shader)->mFlags) & 253;
            updateShader(at<void>(shader), 0);
        }
        u32 texture =
            at<TreeTextureBindings>(at<TreeShader>(shader)->mpTextureBindings.v)->mpTexture.v;
        if (old != texture)
            bindShaderTexture(at<void>(texture));
        u32 length = (at<TreeShader>(shader)->mDisplayListSize);
        if (length)
            GX2CallDisplayList(at<void>((at<TreeShader>(shader)->mpDisplayList.v)), length);
        else
            bindShader(at<void>(shader));
        at<TreeMaterialCache>(cache)->mpTexture.v = texture;
        at<TreeMaterialCache>(cache)->mpShader.v = shader;
    }
    type = (at<TreeDrawArguments>(draw)->mMaterialType);
    bool uniform = false;
    u32 data = 0;
    if (type == 0) {
        u32 catalog = (at<TreeDrawArguments>(draw)->mpCatalog.v);
        if (catalog) {
            u32 object = (at<TreeResourceList>(catalog)->mpResources.v),
                idx = (at<TreeDrawArguments>(draw)->mResourceIndex);
            if (idx < (at<TreeResourceList>(catalog)->mCount))
                object += idx * 572;
            data = object + 16 + (at<TreeUniformResource>(object)->mUniformIndex) * 28;
            uniform = true;
        }
    } else if (type == 1 || type == 2) {
        u32 object = (u32)resource->mpUniform.v;
        data = object + 16 + (at<TreeUniformResource>(object)->mUniformIndex) * 28;
        uniform = true;
    }
    if (uniform) {
        u32 metadata = (at<TreeMaterial>(material)->mMetadataCount)
                           ? (at<TreeMaterial>(material)->mpMetadata.v)
                           : 0;
        s32 vs = (s16)(at<TreeUniformMetadata>(metadata)->mVertexRegister),
            ps = (s16)(at<TreeUniformMetadata>(metadata)->mPixelRegister),
            gs = (s16)(at<TreeUniformMetadata>(metadata)->mGeometryRegister);
        u32 size = (at<TreeUniformData>(data)->mSize), ptr = (at<TreeUniformData>(data)->mpData.v);
        if (ps != -1)
            GX2SetPixelUniformReg(ps, size, at<void>(ptr));
        if (vs != -1)
            GX2SetVertexUniformReg(vs, size, at<void>(ptr));
        if (gs != -1)
            GX2SetGeometryUniformReg(gs, size, at<void>(ptr));
    }
    if (type == 2) {
        u32 obj = (at<TreeDrawArguments>(draw)->mpDrawUniform.v);
        if (obj)
            call_ptr<void>(at<TreeDrawBase>(obj)->__vtbl->applyMaterial, at<void>(obj),
                           at<void>(material));
        applyDrawMaterial(at<void>(draw), at<void>(material));
    }
    u32 texture =
        (at<TreeMaterial>(material)->mTextureCount) ? (at<TreeMaterial>(material)->mpTexture.v) : 0;
    bindTextureSampler(&at<TreePacketState>(a)->mRenderer.mTextureState, at<void>(texture + 4), 0,
                       0);
    return material;
}

/* inlined into TreePacketState::draw and drawShadow */
static void tree_renderState(u32 draw, bool shadows) {
    TreeRenderState state;
    initializeRenderState(&state);
    state.mMode = shadows ? 3 : 2;
    if (shadows) {
        state.mDepthEnable = 1;
        state.mDepthWrite = 0;
    }
    state.mBlendEnable = 1;
    state.mBlendOperation = 4;
    state.mAlphaReference = (shadows ? treeZero : alphaThreshold);
    state.mBlendFactors = ((((state.mBlendFactors) & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 16;
    setMaterialRenderState((at<TreeDrawArguments>(draw)->mMaterialType), &state);
    applyRenderState(&state);
}

static u32 tree_shapeRecord(u32 a, u32 draw, TreeMaterialList* records) {
    u32 table = (u32)records->mpRecords.v, type = (at<TreeDrawArguments>(draw)->mMaterialType);
    if (type < (u32)records->mCapacity)
        table += type * 20;
    return table;
}

static void tree_setShape(u32 record, u32 flag, u32 index, u32 stride) {
    TreeShapeArray* shapes = &at<TreeMaterialRecord>(record)->mShapes[flag == 0 ? 1 : 0];
    u32 ptr = (u32)shapes->mpShapes.v;
    if ((u32)shapes->mCount > index)
        ptr += stride;
    bindShape(at<void>(ptr));
}

static void tree_drawIndexed(u32 p) {
    u32 count = (at<TreeIndexBuffer>(p)->mCount);
    if (count)
        GX2DrawIndexed((at<TreeIndexBuffer>(p)->mIndexType), count,
                       (at<TreeIndexBuffer>(p)->mPrimitive),
                       at<void>((at<TreeIndexBuffer>(p)->mpIndices.v)), 0, 1);
}

/* inlined into TreePacketState::prepareRender */
static void tree_matrixUniform(u32 object, u32 source) {
    TreeMtx m;
    u32 bits[12];
    for (u32 i = 0; i < 12; i++)
        bits[i] = at<be<u32>>(source)[i].get();
    for (u32 i = 0; i < 12; i++)
        at<be<u32>>(ea(&m))[i] = bits[i];
    PSMTXCopy(&m, at<void>(object + 0x74));
    updateDrawUniform(at<void>(object));
}

/* inlined into TreePacketState::prepareRender */
static void tree_colorUniform(u32 container, u32 tev) {
    TreeVec4 color;
    TreeVec4 out;
    for (u32 i = 0; i < 2; i++) {
        u32 dst = (at<TreeUniformContainer>(container)->mpUniform.v);
        convertColor(&color, at<void>(tev + (i ? 0x160 : 0x90)));
        scaleColorCopy(
            &out, &color,
            (i ? (f32)at<TreeTev>(tev)->mColorC1Scale : (f32)at<TreeTev>(tev)->mColorC0Scale));
        for (u32 j = 0; j < 4; j++)
            at<TreeUniformColors>(dst)->mColors[i][j] = floatBits(out.a[j]);
    }
    updateColorUniform(at<void>(container));
}

static void tree_lightingUniform(u32 object, u32 tev, u32 container, f32 alpha) {
    f32 denominator = colorDenominator;
    for (u32 i = 0; i < 4; i++)
        at<TreeLargeDraw>(object)->mColorC0[i] =
            (f32)((f32)(s16)at<TreeTev>(tev)->mColorC0[i] / denominator);
    for (u32 i = 0; i < 4; i++)
        at<TreeLargeDraw>(object)->mColorK0[i] =
            (f32)((f32)(u8)at<TreeTev>(tev)->mColorK0[i] / denominator);
    at<TreeLargeDraw>(object)->mAlpha = alpha;
    scaleColor(at<void>(object + 0xC4), (at<TreeTev>(tev)->mColorK0Scale));
    u32 data = (at<TreeUniformContainer>(container)->mpUniform.v);
    for (u32 i = 0; i < 4; i++)
        storeFloatBits(at<TreeLargeDraw>(object)->mAmbientColor[i],
                       at<TreeUniformColors>(data)->mColors[0][i]);
    updateDrawUniform(at<void>(object));
}

static void tree_constructList(u32 p) {
    if (!p)
        p = allocateObject(0x10C);
    if (p) {
        at<dTree_room_c>(p)->mCount = 0;
        at<dTree_room_c>(p)->mCapacity = 0;
        at<dTree_room_c>(p)->mpData.v = 0;
        initializeRoomList(at<void>(p), 64, at<void>(p + 12));
    }
}

static void tree_constructPair(u32 p) {
    if (!p)
        p = allocateObject(8);
    if (p) {
        at<TreeShapeArray>(p)->mpShapes.v = 0;
        at<TreeShapeArray>(p)->mCount = 0;
    }
}

/* inlined into Tree_packetCtor */
static u32 tree_lookupMaterial(u32 name) {
    TreeName n;
    n.text = name;
    n.allocator = 0x100560B4;
    u32 catalog = getMaterialCatalog();
    s32 index = findMaterialIndex(at<void>((at<TreeMaterialCatalog>(catalog)->mpResource.v)), &n);
    if (index < 0)
        return 0;
    u32 count = (at<TreeMaterialCatalog>(catalog)->mCount),
        table = (at<TreeMaterialCatalog>(catalog)->mpMaterials.v);
    u32 selected = table + ((u32)index < count ? (u32)index * 36 : 0);
    if (!(at<TreeMaterial>(selected)->mLoaded)) {
        u32 resource = (at<TreeMaterialCatalog>(catalog)->mpResource.v),
            ptr = (u32)index < (at<TreeMaterialResource>(resource)->mCount)
                      ? (at<TreeMaterialResource>(resource)->mpMaterials.v) + (u32)index * 132
                      : 0;
        createMaterial(at<void>(selected), at<void>(ptr), 0);
        count = (at<TreeMaterialCatalog>(catalog)->mCount);
        table = (at<TreeMaterialCatalog>(catalog)->mpMaterials.v);
    }
    return table + ((u32)index < count ? (u32)index * 36 : 0);
}

static u32 tree_heapAllocate(u32 size, u32 alignment) {
    u32 heap = getCurrentHeap(at<void>(treeHeap));
    return call_ptr<u32>(at<TreeHeap>(heap)->__vtbl->allocate, at<void>(heap), size, alignment);
}

static void tree_constructShapes(u32 list, u32 col) {
    for (u32 i = 0; i < (at<TreeMaterialList>(list)->mCount); i++) {
        u32 record = (at<TreeMaterialList>(list)->mpRecords.v) +
                     (i < (at<TreeMaterialList>(list)->mCapacity) ? i * 20 : 0),
            material = (at<TreeMaterialRecord>(record)->mpMaterial.v);
        at<TreeMaterialRecord>(record)->mpMaterial.v = 0;
        for (u32 off : {4u, 12u}) {
            u32 p = (u32)at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v;
            if (p) {
                for (s32 j = 0;
                     j < (s32)(u32)at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mCount;
                     j++) {
                    u32 object = p + (u32)j * 244;
                    call_ptr<void>(at<TreeNativeShape>(object)->__vtbl->destroy, at<void>(object),
                                   2);
                    p = (u32)at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v;
                }
                tree_freeField(&at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v);
                at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mCount = 0;
                at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v = 0;
            }
        }
        at<TreeMaterialRecord>(record)->mpMaterial.v = material;
        for (u32 off : {4u, 12u}) {
            u32 p = tree_heapAllocate(0x4C4, 4);
            for (u32 j = 0; j < 5; j++) {
                u32 object = p + j * 244;
                if (object)
                    constructShape(at<void>(object));
            }
            if (p) {
                at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mpShapes.v = p;
                at<TreeMaterialRecord>(record)->mShapes[(off - 4) / 8].mCount = 5;
            }
        }
        for (u32 j = 0; j < 2; j++)
            for (u32 k = 0; k < 5; k++) {
                u32 pair = record + 4 + j * 8,
                    p = (at<TreeShapeArray>(pair)->mpShapes.v) +
                        (k < (at<TreeShapeArray>(pair)->mCount) ? k * 244 : 0);
                createTreeShape(at<void>(material), at<void>(p),
                                at<void>(col + 4 + j * 596 + k * 1192), at<void>(col + 0x174C), 0);
            }
    }
}

static void tree_copyVertexGroup(u32 dst, u32 src, u32 count) {
    u32 words[4];
    for (u32 i = 0; i < count; i++)
        words[i] = at<be<u32>>(src)[i].get();
    for (u32 i = 0; i < count; i++)
        at<be<u32>>(dst)[i] = words[i];
}

// HD: ground probing keeps the extra triangle query but omits the GameCube shadow matrix.
/* 025C6350 */
f64 Tree_checkGroundY(u32 ignored, cXyz* pos) {

    TreeGround g;
    constructGroundCheck(&g);
    f32 raise = groundRaise;
    u32 poly = ea(&g) + 0x40, pass = ea(&g) + 0x4C;
    g.mFlags[1] = 0;
    f32 y = fadds_ppc((pos->y), raise);
    g.mpPolyPass.v = poly;
    storeFloatBits(g.mPosition.x, floatBits(pos->x));
    g.mPosition.y = y;
    g.mPolyFlags[1] = 0;
    g.mPolyFlags[0] = 0;
    g.mGroupPassVtable = 0x100560FC;
    pos->y = y;
    g.mGroundVtable = 0x100560DC;
    g.mFlags[0] = 0;
    g.mVtable = 0x100560EC;
    g.mPolyFlags[2] = 0;
    g.mpGroupPass.v = pass;
    g.mFlags[2] = 0;
    storeFloatBits(g.mPosition.z, floatBits(pos->z));
    g.mPolyFlags[3] = 0;
    g.mPolyPassVtable = 0x1005610C;
    g.mGroup = 1;
    u32 game = getGameInfo();
    f64 h = groundCross(&at<TreeGameView>(game)->mBackground[0], &g);
    y = fsubs_ppc((pos->y), raise);
    f32 threshold = groundMinimum;
    pos->y = y;
    if (h > threshold) {
        game = getGameInfo();
        getGroundTriangle(&at<TreeGameView>(game)->mBackground[0], (g.mPolygonIndex),
                          (g.mBackgroundIndex));
    } else
        h = y;
    g.mVtable = 0x100560EC;
    g.mPolyPassVtable = 0x1005610C;
    g.mGroupPassVtable = 0x100560CC;
    destroyGroundCheck(&g, 0);
    return h;
}

// HD: preserve callback reloads and the original single-precision force arithmetic.
/* 025C65CC */
void dTree_data_c::WorkCo(TreeActorView* actor, u32 unused, u32 room) {
    u32 actorAddr = ea(actor);
    if ((field_0x001))
        return;
    s32 index = (s8)(mAnimIdx);
    if (index < 8) {
        u32 game = getGameInfo();
        index = (s8)(mAnimIdx);
        u32 anm = (at<TreeGameView>(game)->mpTree.v) + (u32)index * 0x84;
        game = getGameInfo();
        s32 next = newAnimation(at<void>((at<TreeGameView>(game)->mpTree.v)),
                                (s32)(s16)(u16)at<dTree_anm_c>(anm + 0x2A9C)->field_0x02);
        if (next < 0)
            return;
        mAnimIdx = next;
    }
    f32 dx = fsubs_ppc((mPos.x), (actor->mPosition.x)),
        dz = fsubs_ppc((mPos.z), (actor->mPosition.z));
    cXyz vel;
    vel.x = dx;
    vel.y = treeZero;
    vel.z = dz;
    PSVECSquareMag(&vel);
    u32 game = getGameInfo();
    index = (s8)(mAnimIdx);
    u32 anm = (at<TreeGameView>(game)->mpTree.v) + 0x2A9C + (u32)index * 0x84;
    s32 angle = cM_atan2s(dx, dz);
    u32 diff = ((s32)(s16)(actor->mAngleY) - angle) & 0xFFFF;
    f32 cos = (f32)l_sincosTable[diff >> 3].mCos;
    if (cos > damping && actorAddr && (s16)(actor->mName) == TreeProcess_Player)
        call_ptr<void>(actor->__vtbl->setCollisionMode, actor, 7);
    f32 speed = std::fabs(fmuls_ppc((actor->mSpeedF), groundRaise)), minimum = maximumSwing;
    speed = fsubs_ppc(speed, minimum) >= 0 ? minimum : speed;
    f32 strength = fmuls_ppc(speed, cos);
    if (std::fabs((at<dTree_anm_c>(anm)->mAnimTimer)) < std::fabs(strength)) {
        at<dTree_anm_c>(anm)->field_0x0a = angle;
        at<dTree_anm_c>(anm)->mAnimTimer = strength;
    }
}

// HD: magnitude stays double through the tiny-vector comparison and bomb fallback.
/* 025C6784 */
void dTree_data_c::WorkAt_NoCutAnim(TreeActorView* actor, u32 unused, u32 room, u32 inf,
                                    TreeAttackView* hit) {
    u32 actorAddr = ea(actor);
    u32 a = ea(this);
    if ((field_0x001) || !tree_ensureAnm(a))
        return;
    u32 obj = dCcD_GetGObjInf(hit);
    cXyz vel;
    cXyz xz;
    storeFloatBits(vel.x, floatBits(at<TreeAttackView>(obj)->mAttackVector.x));
    storeFloatBits(vel.y, floatBits(at<TreeAttackView>(obj)->mAttackVector.y));
    storeFloatBits(vel.z, floatBits(at<TreeAttackView>(obj)->mAttackVector.z));
    f32 zero = treeZero;
    storeFloatBits(xz.x, floatBits(vel.x));
    xz.y = zero;
    storeFloatBits(xz.z, floatBits(vel.z));
    f32 square = PSVECSquareMag(&xz);
    f64 mag = squareRoot(square);
    if (std::fabs(mag) < tiny && actorAddr) {
        s16 name = (s16)(actor->mName);
        if (name == TreeProcess_Player || name == TreeProcess_Bomb ||
            ((s16)(actor->mName) == TreeProcess_Bomb2)) {
            f32 dx = fsubs_ppc((mPos.x), (actor->mPosition.x)),
                dz = fsubs_ppc((mPos.z), (actor->mPosition.z)),
                dy = fsubs_ppc((mPos.y), (actor->mPosition.y));
            vel.x = dx;
            vel.y = dy;
            vel.z = dz;
            xz.x = dx;
            xz.y = zero;
            xz.z = dz;
            square = PSVECSquareMag(&xz);
            f64 distance = squareRoot(square);
            if (name == TreeProcess_Player) {
                mag = distance;
                u32 game = getGameInfo();
                u32 anm =
                    (at<TreeGameView>(game)->mpTree.v) + (u32)(s32)(s8)(mAnimIdx) * 0x84 + 0x2A9C;
                s32 angle = cM_atan2s((vel.x), (vel.z));
                f32 strength = fmuls_ppc((f32)mag, groundRaise), limit = maximumSwing;
                strength = fsubs_ppc(strength, limit) >= 0 ? limit : strength;
                if (std::fabs((at<dTree_anm_c>(anm)->mAnimTimer)) < std::fabs(strength)) {
                    at<dTree_anm_c>(anm)->field_0x0a = angle;
                    at<dTree_anm_c>(anm)->mAnimTimer = strength;
                }
                return;
            }
            mag = fsubs_ppc(bombRange, (f32)distance);
            mag = mag >= 0 ? mag : zero;
            PSVECNormalize(&vel, &vel);
            PSVECScale(&vel, &vel, (f32)mag);
        }
    }
    u32 game = getGameInfo();
    u32 anm = (at<TreeGameView>(game)->mpTree.v) + (u32)(s32)(s8)(mAnimIdx) * 0x84 + 0x2A9C;
    s32 angle = cM_atan2s((vel.x), (vel.z));
    f32 strength = fmuls_ppc((f32)mag, groundRaise), limit = maximumSwing;
    strength = fsubs_ppc(strength, limit) >= 0 ? limit : strength;
    if (std::fabs((at<dTree_anm_c>(anm)->mAnimTimer)) < std::fabs(strength)) {
        at<dTree_anm_c>(anm)->field_0x0a = angle;
        at<dTree_anm_c>(anm)->mAnimTimer = strength;
    }
}

/* 025C6A58 */
void dTree_data_c::WorkAt(TreeActorView* actor, u32 flags, u32 room, TreeMassHitInfo* info) {

    u32 a = ea(this);
    u32 hit = (info->mpHitObject.v);
    if (hit && ((at<TreeAttackView>(hit)->mAttackType) & 0x003CC220)) {
        workAttackNoCut(at<void>(a), actor, flags, room, info, at<void>(hit));
        return;
    }
    f32 dx = fsubs_ppc((mPos.x), (actor->mPosition.x)),
        dz = fsubs_ppc((mPos.z), (actor->mPosition.z)), zero = treeZero;
    cXyz v;
    v.x = dx;
    v.y = zero;
    v.z = dz;
    PSVECSquareMag(&v);
    cM_atan2s(dx, dz);
    if ((field_0x001) || !tree_ensureAnm(a))
        return;
    field_0x001 = 1;
    u32 game = getGameInfo(),
        anm = (at<TreeGameView>(game)->mpTree.v) + (u32)(s32)(s8)(mAnimIdx) * 0x84;
    game = getGameInfo();
    u32 sword = (s32)(s16)(u16)at<TreeGameView>(game)->mpTree->mPlayerSwordMoveAngY;
    at<dTree_anm_c>(anm + 10908)->mAnimTimer = zero;
    at<dTree_anm_c>(anm + 10908)->field_0x0a = sword;
    at<dTree_anm_c>(anm + 10908)->field_0x10 = gravity;
    at<dTree_anm_c>(anm + 10908)->field_0x14 = initialCutSpeed;
    if (!(u8)l_CutSoundFlag) {
        l_CutSoundFlag = 1;
        u32 reverb = getReverb(room);
        startObjectSound(0x6917, at<void>(a + 4), 0, reverb);
    }
    game = getGameInfo();
    cXyz shock;
    shock.z = zero;
    shock.x = zero;
    shock.y = treeOne;
    startShock(&at<TreeGameView>(game)->mVibration[0], 4, -33, &shock);
}

/* 025C6C8C */
s32 dTree_data_c::hitCheck(u32 unused, u32 room) {
    u32 a = ea(this);
    Local<be<u32>> actor;
    TreeMassHitInfo hit;
    *actor.get() = 0;
    constructMassHitInfo(&hit);
    u32 game = getGameInfo();
    u32 flags =
        checkMass(&at<TreeGameView>(game)->mMassSystem[0], at<void>(a + 4), actor.get(), &hit);
    if (flags && actor.get()->get()) {
        if (flags & 2)
            workCollision(at<void>(a), at<void>(actor.get()->get()), flags, room);
        if (flags & 1)
            workAttack(at<void>(a), at<void>(actor.get()->get()), flags, room, &hit);
    }
    return 0;
}

// HD: native particle slots, color state and emitter fields replace the GameCube setters.
/* 025C6D40 */
void dTree_data_c::animation(u32 room) {
    u32 a = ea(this);
    u32 game = getGameInfo(),
        anm = (at<TreeGameView>(game)->mpTree.v) + 0x2A9C + (u32)(s32)(s8)(mAnimIdx) * 0x84;
    f32 zero = treeZero, one = treeOne, threshold = swingStep;
    if (!(field_0x001)) {
        s32 angle = (s16)(u16)(at<dTree_anm_c>(anm)->field_0x08);
        f32 restoring = -(f32)angle,
            timer = fmuls_ppc(fadds_ppc((at<dTree_anm_c>(anm)->mAnimTimer), restoring), damping);
        s16 next = (s16)(angle + (s16)ftoi(timer));
        at<dTree_anm_c>(anm)->mAnimTimer = timer;
        f32 product = fmuls_ppc((f32)next, restoring);
        at<dTree_anm_c>(anm)->field_0x08 = next;
        if (!(product < zero)) {
            f32 amount = std::fabs((at<dTree_anm_c>(anm)->mAnimTimer));
            if (amount < threshold) {
                u32 idx = (u16)(at<dTree_anm_c>(anm)->field_0x02) >> 13;
                at<dTree_anm_c>(anm)->field_0x08 = 0;
                at<dTree_anm_c>(anm)->mAnimTimer = zero;
                game = getGameInfo();
                u32 base = (at<TreeGameView>(game)->mpTree.v) + idx * 0x84 + 0x2A9C;
                if (chaseAngle(at<void>(anm + 4),
                               (s32)(s16)(u16)(at<dTree_anm_c>(base)->field_0x04), 800) &&
                    chaseAngle(at<void>(anm + 6),
                               (s32)(s16)(u16)(at<dTree_anm_c>(base)->field_0x06), 800)) {
                    game = getGameInfo();
                    at<TreeGameView>(game)->mpTree->mAnm[(u32)(s32)(s8)mAnimIdx].mState = 0;
                    mAnimIdx = idx;
                }
                next = (s16)(u16)(at<dTree_anm_c>(anm)->field_0x08);
            } else if (amount > leafThreshold) {
                cXyz pos;
                f32 yy = fadds_ppc((mPos.y), leafHeight), zz = (mPos.z), xx = (mPos.x);
                pos.z = zz;
                pos.x = xx;
                pos.y = yy;
                getGameInfo();
                u32 color = 0x1047E6CC + room * 556 + 0xEC;
                game = getGameInfo();
                setSimpleParticle(at<void>((at<TreeGameView>(game)->mpParticle.v)), 0x3DC, &pos,
                                  255, at<void>(color), at<void>(color), 0);
                game = getGameInfo();
                u32 slot =
                    getSimpleParticleSlot(at<void>((at<TreeGameView>(game)->mpParticle.v)), 0x3DC);
                if (slot && (at<TreeParticleSlot>(slot)->mpEmitter.v))
                    tree_particleState((at<TreeParticleSlot>(slot)->mpEmitter.v), one);
                u32 reverb = getReverb(room);
                startObjectSound(0x6932, at<void>(a + 4), 0, reverb);
                next = (s16)(u16)(at<dTree_anm_c>(anm)->field_0x08);
            }
        }
        at<dTree_anm_c>(anm)->field_0x04 = next;
        return;
    }
    // HD: zero-extend the angle before indexing the sine/cosine table.
    u32 trig = 0x104A44F8 + ((u16)(at<dTree_anm_c>(anm)->field_0x0a) >> 3) * 8;
    f32 velocity = (at<dTree_anm_c>(anm)->field_0x10),
        vert = fadds_ppc((at<dTree_anm_c>(anm)->field_0x14), gravity);
    f32 dx = fmuls_ppc(velocity, (f32)at<TreeTrig>(trig)->mSin),
        dz = fmuls_ppc(velocity, (f32)at<TreeTrig>(trig)->mCos);
    if (vert < terminalVelocity)
        vert = terminalVelocity;
    f32 x = fadds_ppc((at<dTree_anm_c>(anm)->field_0x18), dx),
        y = fadds_ppc((at<dTree_anm_c>(anm)->field_0x1c), vert),
        z = fadds_ppc((at<dTree_anm_c>(anm)->field_0x20), dz);
    at<dTree_anm_c>(anm)->field_0x14 = vert;
    at<dTree_anm_c>(anm)->field_0x18 = x;
    at<dTree_anm_c>(anm)->field_0x20 = z;
    if (y < groundOffset) {
        y = groundOffset;
        at<dTree_anm_c>(anm)->field_0x10 = zero;
    }
    at<dTree_anm_c>(anm)->field_0x1c = y;
    chaseFloat(at<void>(anm + 12), cutTargetSpeed, threshold);
    if ((s16)(u16)(at<dTree_anm_c>(anm)->field_0x04) != 13600) {
        if (!chaseAngle(at<void>(anm + 4), 13600,
                        (s32)(s16)ftoi((at<dTree_anm_c>(anm)->mAnimTimer))))
            return;
        if (!(u32)l_particleOffsetReady) {
            l_particleOffset.x = zero;
            l_particleOffsetReady = 1;
            l_particleOffset.y = smokeHeight;
            l_particleOffset.z = zero;
        }
        if (!(u32)l_scaleReady) {
            l_scaleReady = 1;
            f32 scale = smokeScale;
            l_scale.x = scale;
            l_scale.y = scale;
            l_scale.z = scale;
        }
        cXyz pos;
        PSMTXMultVec(at<void>(anm + 36), &l_particleOffset, &pos);
        game = getGameInfo();
        u32 emitter = createParticle(at<void>((at<TreeGameView>(game)->mpParticle.v)), 2, 0x2022,
                                     &pos, 0, &l_scale, 0xAA, at<void>(0x10487250), (s32)(s8)room,
                                     at<void>(0), at<void>(0), 0);
        if (emitter) {
            at<TreeEmitter>(emitter)->mMaxFrame = 1;
            at<TreeEmitter>(emitter)->mLifeTime = 60;
            at<TreeEmitter>(emitter)->mRate = smokeRate;
            at<TreeEmitter>(emitter)->mSpread = one;
            at<TreeEmitter>(emitter)->mDirectionalSpeed = smokeSpeed;
        }
        pos.y = fsubs_ppc((pos.y), groundRaise);
        getGameInfo();
        u32 color = 0x1047E6CC + room * 556 + 0xEC;
        game = getGameInfo();
        emitter =
            createParticle(at<void>((at<TreeGameView>(game)->mpParticle.v)), 0, 0x3DF, &pos, 0,
                           at<void>(0), 255, at<void>(0), -1, at<void>(color), at<void>(color), 0);
        if (emitter)
            tree_particleState(emitter, one);
        return;
    }
    if ((at<dTree_anm_c>(anm)->field_0x10) == zero) {
        mState = (mState) | 8;
        PSMTXCopy(at<void>(anm + 36), at<void>(a + 0x70));
        field_0x070.a[3] = fadds_ppc((mPos.x), (at<dTree_anm_c>(anm)->field_0x18));
        field_0x070.a[7] = fadds_ppc((mPos.y), (at<dTree_anm_c>(anm)->field_0x1c));
        field_0x070.a[11] = fadds_ppc((mPos.z), (at<dTree_anm_c>(anm)->field_0x20));
        game = getGameInfo();
        at<TreeGameView>(game)->mpTree->mAnm[(u32)(s32)(s8)mAnimIdx].mState = 0;
        mAnimIdx = (u16)(at<dTree_anm_c>(anm)->field_0x02) >> 13;
    }
}

/* 025C738C */
void dTree_room_c::newData(dTree_data_c* p) {
    u32 pAddr = ea(p);
    u32 n = (mCount);
    if ((s32)n >= (s32)(mCapacity))
        return;
    mpData.get()[n].v = pAddr;
    mCount = (mCount) + 1;
}

/* 025C73B8 */
void dTree_room_c::deleteData() {
    u32 start = (mpData.v), end = start + (mCount) * 4;
    for (u32 p = start; p != end; p += 4) {
        at<dTree_data_c>(at<be<u32>>(p)->get())->mState = 0;
        deleteObjectSound(at<void>(at<be<u32>>(p)->get() + 4));
    }
    mCount = 0;
}

// HD: construct native buffered geometry, materials, uniforms and the program texture.
/* 025C7A1C */
void* Tree_packetCtor(void* ptr) {
    u32 a = ea(ptr);
    if (!a)
        a = allocateObject(0x2DE90);
    if (!a)
        return nullptr;
    constructPacketBase(at<void>(a));
    at<TreePacketState>(a)->__vtbl = 0x10056260;
    at<TreePacketState>(a)->mNextIdx = 0;
    constructArray(&at<TreePacketState>(a)->mData, 64, 168,
                   gabi::guest_fn((const void*)Tree_dataCtor));
    constructArray(&at<TreePacketState>(a)->mAnm, 72, 132,
                   gabi::guest_fn((const void*)Tree_anmCtor));
    constructArray(&at<TreePacketState>(a)->mRoom, 64, 268,
                   gabi::guest_fn((const void*)Tree_roomCtor));
    at<TreePacketState>(a)->mPlayerCutFlg = 0;
    at<TreePacketState>(a)->mPlayerSwordAngY = 0;
    at<TreePacketState>(a)->mPlayerSwordMoveAngY = 0;
    at<TreePacketState>(a)->mMaterials.mCount = 0;
    tree_constructPair(a + 0x92D4);
    u32 col = a + 0x92DC, allocatedCol = col;
    if (!allocatedCol)
        allocatedCol = allocateObject(0x1760);
    if (allocatedCol) {
        constructArray(at<void>(allocatedCol), 10, 596,
                       gabi::guest_fn((const void*)Tree_collisionCtor));
        at<TreeCollisionManager>(allocatedCol)->mActiveBuffer = 0;
        at<TreeCollisionManager>(allocatedCol)->mCount = 0;
        at<TreeCollisionManager>(allocatedCol)->mVertexStride = 48;
        at<TreeCollisionManager>(allocatedCol)->mDirty = 0;
        at<TreeCollisionManager>(allocatedCol)->mVertexFormat = 0;
        for (u32 j = 0; j < 2; j++)
            for (u32 i = 0; i < 5; i++)
                at<TreeCollisionManager>(allocatedCol)->mBuffers[i][j].mpVertices.v = 0;
    }
    constructUniformContainer(&at<TreePacketState>(a)->mUniformContainer);
    constructArray(&at<TreePacketState>(a)->mSmallDraw, 128, 168,
                   gabi::guest_fn((const void*)Tree_drawSmallCtor));
    constructArray(&at<TreePacketState>(a)->mLargeDraw, 64, 868,
                   gabi::guest_fn((const void*)Tree_drawLargeCtor));
    u32 render = a + 0x1D748;
    for (u32 off : {0u, 24u, 48u, 72u})
        constructIndexBuffer(at<void>(render + off));
    constructTextureState(&at<TreeRenderer>(render)->mTextureState);
    constructTexture(&at<TreeRenderer>(render)->mTexture);
    zeroMemory(&at<TreeRenderer>(render)->mShadowPacket, 0x9C);
    constructPacketBase(&at<TreeRenderer>(render)->mShadowPacket);
    at<TreeRenderer>(render)->mShadowPacket.__vtbl = 0x10056230;
    tree_constructList(render + 0x324);
    at<TreeRenderer>(render)->mMaterials.mCount = 0;
    tree_constructPair(render + 0x434);
    constructUniformContainer(&at<TreeRenderer>(render)->mUniformContainer);
    constructArray(&at<TreeRenderer>(render)->mSmallDraw, 64, 168,
                   gabi::guest_fn((const void*)Tree_drawSmallCtor));
    constructArray(&at<TreeRenderer>(render)->mLargeDraw, 64, 868,
                   gabi::guest_fn((const void*)Tree_drawLargeCtor));
    for (u32 i = 0; i < 64; i++)
        at<TreePacketState>(a)->mData[i].mState = 0;
    for (u32 i = 0; i < 72; i++)
        at<TreePacketState>(a)->mAnm[i].mState = 0;
    for (u32 i = 0; i < 8; i++)
        setAnimation(at<void>(a), i, (s32)(s16)(i * 0x2000));
    u32 selected = tree_lookupMaterial(gabi::guest_addr("tree_draw"));
    createMaterialList(&at<TreePacketState>(a)->mMaterials, at<void>(selected), 0);
    selected = tree_lookupMaterial(gabi::guest_addr("tree_draw_xlu"));
    createMaterialList(&at<TreeRenderer>(render)->mMaterials, at<void>(selected), 0);
    at<TreeCollisionManager>(col)->mVertexFormat = 0x1013;
    at<TreeCollisionManager>(col)->mpAttributes.v = 0x10056220;
    for (u32 j = 0; j < 2; j++)
        for (u32 i = 0; i < 5; i++) {
            u32 object = col + j * 596 + i * 1192,
                p = (at<TreeCollisionBuffer>(object)->mpVertices.v);
            if (!p) {
                u32 allocation = tree_heapAllocate(0x1800, 64);
                if (allocation) {
                    at<TreeCollisionBuffer>(object)->mpAllocation.v = allocation;
                    at<TreeCollisionBuffer>(object)->mCapacity = 128;
                }
                p = (at<TreeCollisionBuffer>(object)->mpAllocation.v);
                at<TreeCollisionBuffer>(object)->mpVertices.v = p;
            }
            createTreeVertexBuffer(at<void>(object + 4), at<void>(p), 128, at<void>(col + 0x174C));
        }
    at<TreeCollisionManager>(col)->mCount = 0;
    at<TreeCollisionManager>(col)->mDirty = 1;
    tree_constructShapes(a + 0x92D0, col);
    tree_constructShapes(render + 0x430, col);
    setUniformContainerSlots(&at<TreePacketState>(a)->mUniformContainer, 1, 0);
    setUniformContainerSlots(&at<TreeRenderer>(render)->mUniformContainer, 1, 0);
    for (u32 i = 0; i < 64; i++)
        setUniformSlot(at<void>(a + 0xFE48 + i * 868), 0);
    for (u32 i = 0; i < 64; i++)
        setUniformSlot(at<void>(render + 0x2E48 + i * 868), 0);
    for (u32 i = 0; i < 128; i++)
        setUniformSlot(at<void>(a + 0xAA48 + i * 168), 0);
    for (u32 i = 0; i < 64; i++)
        setUniformSlot(at<void>(render + 0x448 + i * 168), 0);
    u32 indexSources[4] = {0x101EEDD8, 0x101EF1E0, 0x101F08C0, 0x101F11A8};
    for (u32 i = 0; i < 4; i++) {
        createIndexBuffer(at<void>(render + i * 24), at<void>(indexSources[i]), 4,
                          (u32)l_vertexCounts[i].mIndexCount);
        at<TreeRenderer>(render)->mIndexBuffers[i].mIndexType = 4;
    }
    u32 slot = col + (at<TreeCollisionManager>(col)->mActiveBuffer) * 596;
    u32 sources[4][4] = {{0x101EE898, 0x101EE9E8, 0x101EECF8, 0x101EEB38},
                         {0x101EEE80, 0x101EEF58, 0x101EF150, 0x101EF030},
                         {0x101EF240, 0x101EF7E0, 0x101F0500, 0x101EFD80},
                         {0x101F0B48, 0x101F0CE0, 0x101F1098, 0x101F0E78}};
    for (u32 part = 0; part < 4; part++) {
        u32 offset = (part + 1) * 1192,
            dst = (u32)at<TreeCollisionBuffer>(slot + offset)->mpVertices.v, end = dst + 0x1800;
        if (dst < end) {
            for (u32 p = dst; p < end; p += 32)
                for (u32 i = 0; i < 8; i++)
                    at<be<u32>>(p & ~31u)[i] = 0;
            col = a + 0x92DC;
            slot = col + (at<TreeCollisionManager>(col)->mActiveBuffer) * 596;
        }
        dst = (u32)at<TreeCollisionBuffer>(slot + offset)->mpVertices.v;
        for (u32 i = 0; i < (u32)l_vertexCounts[part].mVertexCount; i++) {
            u32 vertex = dst + i * 48;
            tree_copyVertexGroup(vertex, sources[part][0] + i * 12, 3);
            tree_copyVertexGroup(vertex + 12, sources[part][1] + i * 12, 3);
            tree_copyVertexGroup(vertex + 32, sources[part][3] + i * 16, 4);
            tree_copyVertexGroup(vertex + 24, sources[part][2] + i * 8, 2);
        }
    }
    u32 flag = (at<TreeCollisionManager>(col)->mActiveBuffer), other = flag ? 0 : 1;
    for (u32 part = 0; part < 5; part++) {
        u32 sourceObj = col + (flag + part * 2) * 596, destObj = col + (other + part * 2) * 596;
        for (u32 i = 0; i < 128; i++) {
            u32 src = (at<TreeCollisionBuffer>(sourceObj)->mpVertices.v) + i * 48,
                dst = (at<TreeCollisionBuffer>(destObj)->mpVertices.v) + i * 48;
            for (u32 j = 0; j < 12; j++)
                at<be<u32>>(dst)[j] = at<be<u32>>(src)[j];
        }
        flag = (at<TreeCollisionManager>(col)->mActiveBuffer);
    }
    slot = col + flag * 596;
    for (u32 i = 0; i < 5; i++) {
        u32 object = slot + 4 + i * 1192;
        invalidateVertexBuffer(at<void>(object), 0, (u32)at<TreeVertexBuffer>(object)->mSize);
    }
    at<TreeCollisionManager>(col)->mActiveBuffer =
        (at<TreeCollisionManager>(col)->mActiveBuffer) ? 0 : 1;
    lockTextureCache(at<void>(treeTextureCache));
    TreeName nameA;
    TreeName nameB;
    nameA.text = gabi::guest_addr("ProgramTexture");
    nameA.allocator = 0x100560B4;
    nameB.text = gabi::guest_addr("ProgramTexture.bfres");
    nameB.allocator = 0x100560B4;
    u32 texture = getTextureResource(at<void>(treeResourceManager), &nameA, &nameB, 0);
    createTexture(&at<TreeRenderer>(render)->mTexture, at<void>(texture), at<void>(0x10056174));
    bool same = true;
    for (u32 i = 0; i < 6; i++)
        if ((u32)at<TreeRenderer>(render)->mTextureState.mComparisonHeader[i] !=
            (u32)at<TreeRenderer>(render)->mTexture.mComparisonHeader[i]) {
            same = false;
            break;
        }
    if (same)
        same = (at<TreeRenderer>(render)->mTextureState.mHeight) ==
                   (at<TreeRenderer>(render)->mTexture.mHeight) &&
               (at<TreeRenderer>(render)->mTextureState.mDimension) ==
                   (at<TreeRenderer>(render)->mTexture.mDimension) &&
               (at<TreeRenderer>(render)->mTextureState.mFormat) ==
                   (at<TreeRenderer>(render)->mTexture.mFormat);
    if (!same)
        copyTextureState(&at<TreeRenderer>(render)->mTextureState,
                         &at<TreeRenderer>(render)->mTexture);
    else {
        at<TreeRenderer>(render)->mTextureState.mImageData =
            (at<TreeRenderer>(render)->mTexture.mImageData);
        at<TreeRenderer>(render)->mTextureState.mCachedImageData =
            (at<TreeRenderer>(render)->mTexture.mImageData);
        at<TreeRenderer>(render)->mTextureState.mMipData =
            (at<TreeRenderer>(render)->mTexture.mMipData);
        at<TreeRenderer>(render)->mTextureState.mCachedMipData =
            (at<TreeRenderer>(render)->mTexture.mMipData);
    }
    at<TreeRenderer>(render)->mTextureState.mFilter[0] = 1;
    at<TreeRenderer>(render)->mTextureState.mFilter[1] = 1;
    at<TreeRenderer>(render)->mTextureState.mFilter[2] = 1;
    at<TreeRenderer>(render)->mTextureState.mFlags =
        (at<TreeRenderer>(render)->mTextureState.mFlags) | 2;
    unlockTextureCache(at<void>(treeTextureCache));
    at<TreeRenderer>(render)->mShadowPacket.mpTree.v = a;
    return at<void>(a);
}

/* 025CAD3C */
void* Tree_roomCtor(void* p) {
    u32 a = ea(p);
    if (!a)
        a = allocateObject(0x10C);
    if (!a)
        return nullptr;
    u32 dst = a;
    if (!dst)
        dst = allocateObject(0x10C);
    if (dst) {
        at<dTree_room_c>(dst)->mCount = 0;
        at<dTree_room_c>(dst)->mCapacity = 0;
        at<dTree_room_c>(dst)->mpData.v = 0;
        initializeRoomList(at<void>(dst), 0x40, at<void>(dst + 12));
    }
    at<dTree_room_c>(a)->mCount = 0;
    return at<void>(a);
}

/* 025CAD00 */
void* Tree_anmCtor(void* p) {
    u32 a = ea(p);
    if (!a)
        a = allocateObject(0x84);
    if (a)
        at<dTree_anm_c>(a)->mState = 0;
    return at<void>(a);
}

/* 025C64C4 */
void* Tree_dataCtor(void* p) {
    u32 a = ea(p);
    if (!a)
        a = allocateObject(0xA8);
    if (!a)
        return nullptr;
    at<dTree_data_c>(a)->field_0x003 = 0;
    at<dTree_data_c>(a)->mAnimIdx = 0;
    at<dTree_data_c>(a)->mState = 0;
    at<dTree_data_c>(a)->field_0x001 = 0;
    zeroMemory(at<void>(a + 0x10), 0x30);
    zeroMemory(at<void>(a + 0x40), 0x30);
    zeroMemory(at<void>(a + 0x70), 0x30);
    at<dTree_data_c>(a)->mNativeFlags = 0;
    at<dTree_data_c>(a)->mViewDepth = treeZero;
    at<dTree_data_c>(a)->mState = 0;
    return at<void>(a);
}

// HD: preserve the extra draw callback argument, including the integer buffer-offset form.
/* 025C91FC */
void TreePacketState::draw(u32 draw) {
    u32 a = ea(this);
    u32 material = tree_bindMaterial(a, draw, at<TreeMaterialList>(a + 0x92D0),
                                     at<TreeUniformContainer>(a + 0xAA3C));
    tree_renderState(draw, false);
    u32 small = 0, large = 0;
    for (u32 room = 0; room < 64; room++) {
        u32 list = a + 0x4FBC + room * 268, start = (at<dTree_room_c>(list)->mpData.v),
            end = start + (at<dTree_room_c>(list)->mCount) * 4;
        if (start != end) {
            u32 object = a + 0xFE48 + large * 868;
            call_ptr<void>(at<TreeDrawBase>(object)->__vtbl->applyMaterial, at<void>(object),
                           at<void>(material));
            large++;
        }
        for (u32 p = start; p != end; p += 4) {
            u32 data = at<be<u32>>(p)->get();
            if ((at<dTree_data_c>(data)->mState) & TreeState_Clipped)
                continue;
            u32 status = 0x101EE88C + (at<dTree_data_c>(data)->field_0x001) * 3;
            u8 alpha = (at<dTree_data_c>(data)->field_0x003);
            u32 object = a + 0xAA48 + small * 168;
            call_ptr<void>(at<TreeDrawBase>(object)->__vtbl->applyMaterial, at<void>(object),
                           at<void>(material), alpha == 255 ? data : small * 168);
            small++;
            if (alpha == 255) {
                u8 first = at<TreeModelStatus>(status)->mTrunk;
                if (first == 3) {
                    u32 record = tree_shapeRecord(a, draw, at<TreeMaterialList>(a + 0x92D0));
                    tree_setShape(record, (mCollision.mActiveBuffer), 1, 244);
                    tree_drawIndexed(a + 0x1D748);
                }
                u32 record = tree_shapeRecord(a, draw, at<TreeMaterialList>(a + 0x92D0));
                tree_setShape(record, (mCollision.mActiveBuffer), 3, 0x2DC);
                tree_drawIndexed(a + 0x1D778);
                object = a + 0xAA48 + small * 168;
                call_ptr<void>(at<TreeDrawBase>(object)->__vtbl->applyMaterial, at<void>(object),
                               at<void>(material), small * 168);
                small++;
            }
            u8 last = at<TreeModelStatus>(status)->mStump;
            if (last == 2) {
                u32 record = tree_shapeRecord(a, draw, at<TreeMaterialList>(a + 0x92D0));
                tree_setShape(record, (mCollision.mActiveBuffer), 4, 0x3D0);
                tree_drawIndexed(a + 0x1D790);
            } else if (last == 4) {
                u32 record = tree_shapeRecord(a, draw, at<TreeMaterialList>(a + 0x92D0));
                tree_setShape(record, (mCollision.mActiveBuffer), 2, 0x1E8);
                tree_drawIndexed(a + 0x1D760);
            }
        }
    }
    applyRenderState(at<void>(0x104B474C));
}

// HD: bounded room arrays and the current player sword state replace the linked-list walk.
/* 025C9948 */
void TreePacketState::calc() {
    u32 a = ea(this);
    u32 base = a + 0x2A9C;
    f32 amp = windAmplitude, rate = windFrequency, speed = maximumSwing;
    for (u32 i = 0; i < 8; i++) {
        u32 p = base + i * 132;
        f32 frame = (f32)((u32)g_Counter.mTimer + i * 250);
        u16 theta = (u16)ftoi(fmuls_ppc(frame, speed));
        // HD: zero-extend the angle before indexing the sine/cosine table.
        u32 trig = 0x104A44F8 + (theta >> 3) * 8;
        f32 initial = fadds_ppc((f32)(s16)(u16)(at<dTree_anm_c>(p)->field_0x02), amp);
        f32 heading = fmadds(amp, (f32)at<TreeTrig>(trig)->mCos, initial);
        at<dTree_anm_c>(p)->field_0x06 = ftoi(heading);
        frame = (f32)((u32)g_Counter.mTimer + i * 250);
        theta = (u16)ftoi(fmuls_ppc(frame, rate));
        trig = 0x104A44F8 + (theta >> 3) * 8;
        at<dTree_anm_c>(p)->field_0x04 = ftoi(fmadds(amp, (f32)at<TreeTrig>(trig)->mCos, amp));
    }
    s32 room = (s8)(u8)l_stayRoom;
    u32 list = base + (u32)room * 268 + 0x2520;
    u32 start = (at<dTree_room_c>(list)->mpData.v),
        end = start + (at<dTree_room_c>(list)->mCount) * 4;
    if (start == end)
        return;
    u32 game = getGameInfo(), player = (at<TreeGameView>(game)->mpPlayer.v);
    at<TreePacketState>(base + -10908)->mPlayerCutFlg =
        ((at<TreeActorView>(player)->mNoResetFlg0) & 64) != 0;
    cXyz sword;
    cXyz delta;
    delta.x = (at<TreeActorView>(player)->mSwordTopPos.x);
    sword.x = (at<TreeActorView>(player)->mSwordTopPos.x);
    delta.y = (at<TreeActorView>(player)->mSwordTopPos.y);
    sword.y = (at<TreeActorView>(player)->mSwordTopPos.y);
    delta.z = (at<TreeActorView>(player)->mSwordTopPos.z);
    sword.z = (at<TreeActorView>(player)->mSwordTopPos.z);
    subtractVector(&sword, &delta, at<void>(player + 0x314));
    s32 angle = cM_atan2s((delta.x), (delta.z));
    at<TreePacketState>(base + -10908)->mPlayerSwordAngY = angle;
    subtractVector(at<void>(base + 0x6828), &delta, &sword);
    angle = cM_atan2s((delta.x), (delta.z));
    for (u32 i = 0; i < 3; i++)
        at<be<u32>>(ea(&mPlayerSwordTop))[i] = at<be<u32>>(ea(&sword))[i];
    at<TreePacketState>(base + -10908)->mPlayerSwordMoveAngY = angle;
    l_CutSoundFlag = 0;
    game = getGameInfo();
    setMassRadius(&at<TreeGameView>(game)->mMassCylinder[0], swingStep);
    setMassHeight(&at<TreeGameView>(game)->mMassCylinder[0], bombRange);
    at<TreeGameView>(game)->mMassType = 19;
    at<TreeGameView>(game)->mMassPriority = 1;
    for (u32 p = start; p != end; p += 4) {
        u32 data = at<be<u32>>(p)->get();
        if (!((at<dTree_data_c>(data)->mState) & TreeState_Fallen)) {
            hitCheckCall(at<void>(data), at<void>(player), room);
            data = at<be<u32>>(p)->get();
        }
        if ((s8)(at<dTree_data_c>(data)->mAnimIdx) >= 8)
            animateTree(at<void>(data), room);
    }
}

// HD: matrices remain in world space; a separate view depth orders native draw records.
/* 025CA6EC */
void TreePacketState::update() {
    u32 a = ea(this);
    u32 base = a + 0x2A9C;
    for (u32 i = 0; i < 72; i++) {
        u32 anmData = base + i * 0x84;
        rotateY(at<void>(0x1048D0CC), (s32)(s16)(u16)(at<dTree_anm_c>(anmData)->field_0x0a));
        concatenateXRotation(at<void>(0x1048D0CC),
                             (s32)(s16)(u16)(at<dTree_anm_c>(anmData)->field_0x04));
        concatenateYRotation(at<void>(0x1048D0CC),
                             (s32)(s16)((s16)(u16)(at<dTree_anm_c>(anmData)->field_0x06) -
                                        (s16)(u16)(at<dTree_anm_c>(anmData)->field_0x0a)));
        PSMTXCopy(at<void>(0x1048D0CC), at<void>(anmData + 36));
        rotateY(at<void>(0x1048D0CC), (s32)(s16)(u16)(at<dTree_anm_c>(anmData)->field_0x0a));
        concatenateXRotation(at<void>(0x1048D0CC),
                             (s32)(s16)(u16)(at<dTree_anm_c>(anmData)->field_0x08));
        concatenateYRotation(at<void>(0x1048D0CC),
                             (s32)(s16)((s16)(u16)(at<dTree_anm_c>(anmData)->field_0x02) -
                                        (s16)(u16)(at<dTree_anm_c>(anmData)->field_0x0a)));
        PSMTXCopy(at<void>(0x1048D0CC), at<void>(anmData + 84));
    }
    treeClipper.mFar = fmuls_ppc(treeOriginalFar, clipFarScale);
    calculateClipFrustum(&treeClipper);
    f32 radius = clipRadius;
    u32 grounded = 0;
    TreeMtx m;
    cXyz pos;
    for (u32 i = 0; i < 64; i++) {
        u32 data = a + 0x9C + i * 0xA8;
        if (!((at<dTree_data_c>(data)->mState) & TreeState_Active))
            continue;
        if (((at<dTree_data_c>(data)->mState) & TreeState_NeedsGround) && grounded < 4) {
            f64 y = checkGroundYCall(at<void>(data), at<void>(data + 4));
            at<dTree_data_c>(data)->mPos.y = (f32)y;
            at<dTree_data_c>(data)->mState = (at<dTree_data_c>(data)->mState) & 239;
            grounded++;
        }
        for (u32 j = 0; j < 3; j++)
            at<be<u32>>(ea(&pos))[j] = at<be<u32>>(ea(&at<dTree_data_c>(data)->mPos))[j];
        u32 clipped = clipTreeSphere(&treeClipper, at<void>(0x104B45F8), &pos, radius);
        u32 state = (at<dTree_data_c>(data)->mState);
        if (clipped) {
            at<dTree_data_c>(data)->mState = state | 4;
            continue;
        }
        u32 anim = base + (u32)(s32)(s8)(at<dTree_data_c>(data)->mAnimIdx) * 0x84;
        at<dTree_data_c>(data)->mState = state & 251;
        if ((state & 251) & 8) {
            PSMTXCopy(at<void>(data + 0x70), at<void>(data + 16));
            chaseAlpha(at<void>(data + 3), 0, 8);
        } else {
            at<dTree_anm_c>(anim)->field_0x24.a[3] =
                fadds_ppc((at<dTree_data_c>(data)->mPos.x), (at<dTree_anm_c>(anim)->field_0x18));
            at<dTree_anm_c>(anim)->field_0x24.a[7] =
                fadds_ppc((at<dTree_data_c>(data)->mPos.y), (at<dTree_anm_c>(anim)->field_0x1c));
            at<dTree_anm_c>(anim)->field_0x24.a[11] =
                fadds_ppc((at<dTree_data_c>(data)->mPos.z), (at<dTree_anm_c>(anim)->field_0x20));
            PSMTXCopy(at<void>(anim + 36), at<void>(data + 16));
        }
        storeFloatBits(at<dTree_anm_c>(anim)->field_0x54.a[3],
                       floatBits(at<dTree_data_c>(data)->mPos.x));
        storeFloatBits(at<dTree_anm_c>(anim)->field_0x54.a[7],
                       floatBits(at<dTree_data_c>(data)->mPos.y));
        storeFloatBits(at<dTree_anm_c>(anim)->field_0x54.a[11],
                       floatBits(at<dTree_data_c>(data)->mPos.z));
        PSMTXCopy(at<void>(anim + 84), at<void>(data + 64));
        PSMTXConcat(at<void>(0x104B45F8), at<void>(anim + 84), &m);
        storeFloatBits(at<dTree_data_c>(data)->mViewDepth, floatBits(m.a[11]));
    }
    storeFloatBits(treeClipper.mFar, floatBits(*at<be<f32>>(ea(&treeOriginalFar))));
    calculateClipFrustum(&treeClipper);
    u32 game = getGameInfo();
    treeRenderGlobals.mpXluBuffer.v = at<TreeGameView>(game)->mpXluListBG.v;
    entryImmediate(at<void>(treeRenderGlobals.mpOpaqueBuffer.v), at<void>(a), 0);
    entryImmediate(at<void>(treeRenderGlobals.mpXluBuffer.v),
                   &at<TreePacketState>(a)->mRenderer.mShadowPacket, 0);
    game = getGameInfo();
    treeRenderGlobals.mpXluBuffer.v = at<TreeGameView>(game)->mpXluList.v;
    prepareTreeRender(at<void>(a));
}

// HD: keep the ground result in double until storing the position; preserve random conversion.
/* 025CA9F8 */
void TreePacketState::setData(dTree_data_c* data, u32 next, cXyz* pos, u32 state, u32 room) {

    u32 a = ea(this);
    u32 peek = fopOvlpM_IsPeek();
    f64 y;
    if (peek) {
        y = checkGroundYCall(data, pos);
        data->mState = TreeState_Active | TreeState_Clipped;
    } else {
        y = (pos->y);
        data->mState = TreeState_Active | TreeState_Clipped | TreeState_NeedsGround;
    }
    data->field_0x001 = state;
    data->field_0x003 = state == 1 ? 0 : 255;
    f64 r = cM_rndF(animationRandomRange);
    data->mAnimIdx = ftoi(r);
    f32 x = (pos->x), z = (pos->z);
    data->mPos.x = x;
    data->mPos.y = (f32)y;
    data->mPos.z = z;
    addRoomData(at<void>(a + 0x4FBC + room * 0x10C), data);
    mNextIdx = next;
}

/* 025CAAF4 */
u32 TreePacketState::newData(cXyz* pos, u32 state, u32 room) {

    u32 a = ea(this);
    if (room >= 64)
        assertion(at<void>(gabi::guest_addr("d_tree.cpp")), 0xCAA,
                  at<void>(gabi::guest_addr("0 <= i_roomNo && i_roomNo < 64")));
    u32 start = (mNextIdx), base = a + 0x9C;
    for (u32 i = start; i < 64; i++) {
        u32 p = base + i * 0xA8;
        if (!((at<dTree_data_c>(p)->mState) & TreeState_Active)) {
            setTreeData(at<void>(a), at<void>(p), i, pos, state, room);
            return p;
        }
    }
    for (u32 i = 0; i < start; i++) {
        u32 p = base + i * 0xA8;
        if (!((at<dTree_data_c>(p)->mState) & TreeState_Active)) {
            setTreeData(at<void>(a), at<void>(p), i, pos, state, room);
            return p;
        }
    }
    return 0;
}

/* 025C6558 */
s32 TreePacketState::newAnm(u32 angle) {
    u32 a = ea(this);
    f32 z = treeZero;
    for (u32 i = 8; i < 72; i++) {
        u32 p = a + 0x2A9C + i * 0x84;
        if ((at<dTree_anm_c>(p)->mState))
            continue;
        at<dTree_anm_c>(p)->field_0x08 = 0;
        at<dTree_anm_c>(p)->mState = 1;
        at<dTree_anm_c>(p)->field_0x1c = z;
        at<dTree_anm_c>(p)->field_0x04 = 0;
        at<dTree_anm_c>(p)->field_0x20 = z;
        at<dTree_anm_c>(p)->field_0x06 = angle;
        at<dTree_anm_c>(p)->mAnimTimer = z;
        at<dTree_anm_c>(p)->field_0x10 = z;
        at<dTree_anm_c>(p)->field_0x14 = z;
        at<dTree_anm_c>(p)->field_0x0a = 0;
        at<dTree_anm_c>(p)->field_0x02 = angle;
        at<dTree_anm_c>(p)->field_0x18 = z;
        return i;
    }
    return -1;
}

/* 025C79D0 */
void TreePacketState::setAnm(u32 i, u32 angle) {
    u32 a = ea(this);
    u32 p = a + 0x2A9C + i * 0x84;
    f32 z = treeZero;
    at<dTree_anm_c>(p)->mState = 1;
    at<dTree_anm_c>(p)->field_0x04 = 0;
    at<dTree_anm_c>(p)->field_0x06 = angle;
    at<dTree_anm_c>(p)->field_0x14 = z;
    at<dTree_anm_c>(p)->field_0x08 = 0;
    at<dTree_anm_c>(p)->field_0x0a = 0;
    at<dTree_anm_c>(p)->field_0x1c = z;
    at<dTree_anm_c>(p)->field_0x10 = z;
    at<dTree_anm_c>(p)->field_0x02 = angle;
    at<dTree_anm_c>(p)->field_0x18 = z;
    at<dTree_anm_c>(p)->field_0x20 = z;
    at<dTree_anm_c>(p)->mAnimTimer = z;
}

// HD: compiler-generated and native rendering code of this translation unit.

/* 025C79C8 */
void Tree_drawThunk(u32 a, u32 draw) {
    drawTreeShadow(at<void>((at<TreeShadowPacket>(a)->mpTree.v)), at<void>(draw));
}

/* 025CACD8 */
void Tree_anmDelete(u32 p, u32 flags) {
    if (p && (flags & 1))
        deleteObject(at<void>(p));
}

/* 025CACEC */
void Tree_dataDelete(u32 p, u32 flags) {
    if (p && (flags & 1))
        deleteObject(at<void>(p));
}

/* 025CADBC */
void Tree_roomDtor(u32 a, u32 flags) {
    if (a) {
        destroyPacketBase(at<void>(a), 0);
        if (flags & 1)
            deleteObject(at<void>(a));
    }
}

/* 025CAE10 */
void* Tree_collisionCtor(void* p) {
    u32 a = ea(p);
    if (!a)
        a = allocateObject(0x254);
    if (!a)
        return nullptr;
    constructVertexBuffer(at<void>(a + 4));
    constructShape(at<void>(a + 0x158));
    at<TreeCollisionBuffer>(a)->mpAllocation.v = 0;
    at<TreeCollisionBuffer>(a)->mCapacity = 0;
    return at<void>(a);
}

/* 025CAC14 */
void Tree_staticInit() {
    for (u32 o : {8u, 0u, 12u, 4u})
        l_headerStatics.mZero[o / 4] = 0;
    registerGlobalObject(at<void>(0x101F1280));
    l_headerStatics.mPiMin = piMin;
    l_headerStatics.mPiMax = piMax;
    initializeHeaderByte(at<void>(0x1048721C));
    registerGlobalObject(at<void>(0x101F128C));
    initializeHeaderFlag(at<void>(0x1048721D));
    registerGlobalObject(at<void>(0x101F1298));
    constructSmokeCallback(at<void>(0x10487250), 1, 1, 0, 1);
    registerGlobalObject(at<void>(0x101F12A4));
}

/* 025CB298 */
void Tree_empty298() {}

/* 025CB29C */
void Tree_empty29C() {}

/* 025C9BE8 */
void Tree_copyMtx(TreeMtx* dst, TreeMtx* src) {
    u32 dstAddr = ea(dst);
    u32 srcAddr = ea(src);
    u32 words[12];
    for (u32 i = 0; i < 12; i++)
        words[i] = at<be<u32>>(srcAddr)[i].get();
    for (u32 i = 0; i < 12; i++)
        at<be<u32>>(dstAddr)[i] = words[i];
}

/* 025C9C88 */
void Tree_short4ToFloat4(TreeVec4* dst, TreeShort4* src) {
    u32 dstAddr = ea(dst);
    s32 x = (s16)(u16)src->a[0], y = (s16)(u16)src->a[1], z = (s16)(u16)src->a[2],
        w = (s16)(u16)src->a[3];
    f32 divisor = colorDenominator;
    f32 xx = (f32)x / divisor, yy = (f32)y / divisor, zz = (f32)z / divisor, ww = (f32)w / divisor;
    at<TreeVec4>(dstAddr)->a[0] = xx;
    at<TreeVec4>(dstAddr)->a[1] = yy;
    at<TreeVec4>(dstAddr)->a[2] = zz;
    at<TreeVec4>(dstAddr)->a[3] = ww;
}

// HD: ties and unordered depth comparisons both return the original negative result.
/* 025C9D4C */
s32 Tree_sortCompare(dTree_data_c* a, dTree_data_c* b) {

    return (a->mViewDepth) < (b->mViewDepth) ? 1 : -1;
}

/* 025CAE6C */
void* Tree_drawSmallCtor(void* ptr) {
    u32 a = ea(ptr);
    if (!a)
        a = allocateObject(0xA8);
    if (!a)
        return nullptr;
    constructDrawUniform(at<void>(a));
    at<TreeSmallDraw>(a)->mBase.__vtbl.v = 0x1016EF84;
    zeroMemory(at<void>(a + 0x74), 0x34);
    if (a + 0x74 == 0)
        allocateObject(0x30);
    return at<void>(a);
}

/* 025CAEDC */
void* Tree_vec4Ctor(void* ptr) {
    u32 a = ea(ptr);
    if (!a)
        a = allocateObject(0x10);
    return at<void>(a);
}

// HD: retain the ordered identity/default stores and nullable subobject allocation guards.
/* 025CAF08 */
void* Tree_drawLargeCtor(void* ptr) {
    u32 a = ea(ptr);
    if (!a)
        a = allocateObject(0x364);
    if (!a)
        return nullptr;
    constructDrawUniform(at<void>(a));
    at<TreeLargeDraw>(a)->mBase.__vtbl.v = 0x1016EFB4;
    zeroMemory(at<void>(a + 0x74), 0x2F0);
    f32 z = nativeZero, one = nativeOne;
    for (u32 o :
         {0x74u, 0x78u, 0x7Cu, 0x84u, 0xACu, 0x8Cu,  0xDCu,  0xFCu,  0xCCu,  0xA4u,  0xC8u,
          0xF8u, 0xE8u, 0xB8u, 0x94u, 0xA8u, 0x98u,  0xBCu,  0x88u,  0xE4u,  0xECu,  0xD8u,
          0xC4u, 0xF4u, 0x9Cu, 0xD4u, 0xB4u, 0x104u, 0x108u, 0x10Cu, 0x114u, 0x118u, 0x11Cu})
        at<be<f32>>(a + 0x74)[(o - 0x74) / 4] = z;
    for (u32 o : {0x80u, 0x90u, 0xA0u, 0xB0u, 0xC0u, 0xD0u, 0xE0u, 0xF0u, 0x100u, 0x110u, 0x120u})
        at<be<f32>>(a + 0x74)[(o - 0x74) / 4] = one;
    for (u32 o : {0x124u, 0x144u, 0x164u})
        constructArray(at<void>(a + o), 2, 0x10, gabi::guest_fn((const void*)Tree_vec4Ctor));
    for (u32 o : {0x110u, 0x140u, 0x170u, 0x1A0u, 0x1D0u, 0x200u, 0x230u, 0x260u})
        if (a + 0x74 + o == 0)
            allocateObject(0x30);
    for (u32 o : {0x290u, 0x2A0u, 0x2B0u, 0x2C0u, 0x2D0u, 0x2E0u})
        if (a + 0x74 + o == 0)
            allocateObject(0x10);
    return at<void>(a);
}

/* 025CB190 */
void Tree_drawSmallDtor(u32 a, u32 flags) {
    if (a) {
        destroyDrawUniform(at<void>(a), 0);
        if (flags & 1)
            deleteObject(at<void>(a));
    }
}

/* 025CB1E4 */
void Tree_drawLargeDtor(u32 a, u32 flags) {
    if (a) {
        destroyDrawUniform(at<void>(a), 0);
        if (flags & 1)
            deleteObject(at<void>(a));
    }
}

/* 025CB238 */
void Tree_collisionDtor(u32 a, u32 flags) {
    if (a) {
        destroyShape(at<void>(a + 0x158), 2);
        destroyVertexBuffer(at<void>(a + 4), 2);
        if (flags & 1)
            deleteObject(at<void>(a));
    }
}

// HD: release native allocations and retain GHS array destruction order and callback identities.
/* 025C8C34 */
void Tree_packetDtor(u32 a, u32 flags) {
    if (!a)
        return;
    at<TreePacketState>(a)->__vtbl = 0x10056260;
    u32 col = a + 0x92DC;
    tree_releaseCollisions(col);
    at<TreeCollisionManager>(col)->mCount = 0;
    u32 render = a + 0x1D748;
    destroyArray(&at<TreeRenderer>(render)->mLargeDraw, 64, 0x364,
                 gabi::guest_fn((const void*)Tree_drawSmallDtor), 0, 0);
    destroyArray(&at<TreeRenderer>(render)->mSmallDraw, 64, 0xA8,
                 gabi::guest_fn((const void*)Tree_drawLargeDtor), 0, 0);
    destroyUniformContainer(&at<TreeRenderer>(render)->mUniformContainer, 2);
    tree_releaseShapes(render + 0x430);
    destroyPacketBase(&at<TreeRenderer>(render)->mShadowPacket, 0);
    destroyTextureState(&at<TreeRenderer>(render)->mTextureState, 2);
    for (u32 off : {0x48u, 0x30u, 0x18u, 0u})
        destroyIndexBuffer(at<void>(render + off), 2);
    destroyArray(&at<TreePacketState>(a)->mLargeDraw, 64, 0x364,
                 gabi::guest_fn((const void*)Tree_drawSmallDtor), 0, 0);
    destroyArray(&at<TreePacketState>(a)->mSmallDraw, 128, 0xA8,
                 gabi::guest_fn((const void*)Tree_drawLargeDtor), 0, 0);
    destroyUniformContainer(&at<TreePacketState>(a)->mUniformContainer, 2);
    if (col) {
        tree_releaseCollisions(col);
        at<TreeCollisionManager>(col)->mCount = 0;
        destroyArray(at<void>(col), 10, 0x254, gabi::guest_fn((const void*)Tree_collisionDtor), 0,
                     0);
    }
    tree_releaseShapes(a + 0x92D0);
    destroyPacketBase(at<void>(a), 0);
    if (flags & 1)
        deleteObject(at<void>(a));
}

// HD: native material caches, uniforms, shapes and indexed drawing replace GX display lists.
/* 025C743C */
void TreePacketState::drawShadow(u32 draw) {
    u32 a = ea(this);
    u32 material = tree_bindMaterial(a, draw, at<TreeMaterialList>(a + 0x1DB78),
                                     at<TreeUniformContainer>(a + 0x1DB84));
    tree_renderState(draw, true);
    u32 list = a + 0x1DA6C, start = (at<dTree_room_c>(list)->mpData.v),
        end = start + (at<dTree_room_c>(list)->mCount) * 4;
    if (start == end) {
        applyRenderState(at<void>(0x104B474C));
        return;
    }
    u32 count = (at<dTree_room_c>(list)->mCount) - 1;
    u32 small = a + 0x1DB90 + count * 168, large = a + 0x20590 + count * 868;
    for (u32 p = end - 4; p != start - 4; p -= 4) {
        call_ptr<void>(at<TreeDrawBase>(large)->__vtbl->applyMaterial, at<void>(large),
                       at<void>(material));
        large -= 868;
        call_ptr<void>(at<TreeDrawBase>(small)->__vtbl->applyMaterial, at<void>(small),
                       at<void>(material));
        small -= 168;
        u32 record = tree_shapeRecord(a, draw, at<TreeMaterialList>(a + 0x1DB78));
        tree_setShape(record, (mCollision.mActiveBuffer), 1, 244);
        tree_drawIndexed(a + 0x1D748);
        record = tree_shapeRecord(a, draw, at<TreeMaterialList>(a + 0x1DB78));
        tree_setShape(record, (mCollision.mActiveBuffer), 3, 0x2DC);
        tree_drawIndexed(a + 0x1D778);
    }
    applyRenderState(at<void>(0x104B474C));
}

// HD: native uniform preparation sorts each room and builds a separate translucent-shadow list.
/* 025C9D68 */
void TreePacketState::prepareRender() {
    u32 a = ea(this);
    TreeMtx view;
    TreeTev tev;
    copyMatrixWords(&view, at<void>(0x104B45F8));
    u32 defaults[17];
    for (u32 i = 0; i < 17; i++)
        defaults[i] = (u32)l_tevDefaults[i];
    for (u32 offset : {0u, 0xC0u, 0x144u})
        for (u32 i = 0; i < 17; i++)
            at<be<u32>>(ea(&tev) + offset)[i] = defaults[i];
    initializeTev(&tev, (s32)(s8)(u8)l_stayRoom, 255);
    u32 light = getEnvironmentLight();
    settingTevStruct(at<void>(light), 1, 0, &tev);
    updateTevColors(&tev);
    u32 normal = a + 0xAA3C, shadow = a + 0x1DB84;
    for (u32 container : {normal, shadow})
        setViewUniform(at<void>(container), 0, &view, at<void>(0x104B470C),
                       at<void>(treeRenderGlobals.mpView.v + 0x240));
    tree_colorUniform(normal, ea(&tev));
    tree_colorUniform(shadow, ea(&tev));
    u32 small = 0, large = 0, shadowLarge = 0, shadowSmall = 0, listShadow = a + 0x1DA6C;
    at<dTree_room_c>(listShadow)->mCount = 0;
    f32 one = treeOne, denominator = colorDenominator;
    for (u32 room = 0; room < 64; room++) {
        u32 list = a + 0x4FBC + room * 268, start = (at<dTree_room_c>(list)->mpData.v),
            end = start + (at<dTree_room_c>(list)->mCount) * 4;
        if (start != end) {
            sortRoomList(at<void>(list), at<void>(gabi::guest_fn((const void*)Tree_sortCompare)));
            tree_lightingUniform(a + 0xFE48 + large, ea(&tev), normal, one);
            large += 868;
        }
        for (u32 p = start; p != end; p += 4) {
            u32 data = at<be<u32>>(p)->get();
            if ((at<dTree_data_c>(data)->mState) & TreeState_Clipped)
                continue;
            u8 alpha = (at<dTree_data_c>(data)->field_0x003);
            if (alpha == 0) {
                tree_matrixUniform(a + 0xAA48 + small * 168, data + 0x40);
                small++;
                continue;
            }
            if (alpha == 255) {
                tree_matrixUniform(a + 0xAA48 + small * 168, data + 0x10);
                small++;
            } else {
                tree_lightingUniform(
                    a + 0x20590 + shadowLarge, ea(&tev), shadow,
                    (f32)((f32)(u8)at<dTree_data_c>(at<be<u32>>(p)->get())->field_0x003 /
                          denominator));
                tree_matrixUniform(a + 0x1DB90 + shadowSmall, data + 0x10);
                shadowLarge += 868;
                shadowSmall += 168;
                u32 n = (at<dTree_room_c>(listShadow)->mCount);
                if ((s32)n < (s32)(at<dTree_room_c>(listShadow)->mCapacity)) {
                    at<dTree_room_c>(listShadow)->mpData.get()[n].v = at<be<u32>>(p)->get();
                    at<dTree_room_c>(listShadow)->mCount =
                        (at<dTree_room_c>(listShadow)->mCount) + 1;
                }
            }
            data = at<be<u32>>(p)->get();
            tree_matrixUniform(a + 0xAA48 + small * 168, data + 0x40);
            small++;
        }
    }
}

// HD: compiler-generated header statics of this translation unit.
/* 025CB2A0 */
static void __sinit_d_tree_cpp() {
    l_compilerStatics.mZero[2] = 0;
    l_compilerStatics.mZero[0] = 0;
    l_compilerStatics.mZero[3] = 0;
    l_compilerStatics.mZero[1] = 0;
    registerGlobalObject(at<void>(0x101F12B0));
    f32 lo = compilerPiMin, hi = compilerPiMax;
    l_compilerStatics.mPiMin = lo;
    l_compilerStatics.mPiMax = hi;
    initializeHeaderByte(&l_compilerStatics.mByte);
    registerGlobalObject(at<void>(0x101F12BC));
    initializeHeaderFlag(&l_compilerStatics.mFlag);
    registerGlobalObject(at<void>(0x101F12C8));
}

/* WWHD glue
data compilerPiMin 10056290
data compilerPiMax 10056294
data alphaThreshold 100561C4
str 10056180
str 1005618C
str 1005619C
str 100561AC
str 100561E8
str 100561F4
data l_headerStatics 10487214
data l_compilerStatics 10487270
data l_particleOffset 10487230
data l_scale 1048723C
data l_particleOffsetReady 10487248
data l_scaleReady 1048724C
data l_CutSoundFlag 101EE894
data l_stayRoom 1047E6C8
data g_Counter 101FF558
data treeRenderGlobals 104B45C0
data treeClipper 1048CFF0
data treeOriginalFar 1048D04C
data l_tevDefaults 1016E414
data l_vertexCounts 101EE86C
data treeHeap 101F8B4C
data treeTextureCache 101F8B18
data treeResourceManager 101F4F7C
data tiny 100030B8
data groundRaise 1005611C
data groundMinimum 10056120
data treeZero 10056124
data damping 10056128
data maximumSwing 1005612C
data bombRange 10056130
data gravity 10056134
data initialCutSpeed 10056138
data treeOne 1005613C
data swingStep 10056140
data leafThreshold 10056150
data leafHeight 10056154
data terminalVelocity 10056158
data groundOffset 1005615C
data cutTargetSpeed 10056160
data smokeHeight 10056164
data smokeScale 10056168
data smokeRate 1005616C
data smokeSpeed 10056170
data windAmplitude 100561C8
data windFrequency 100561CC
data colorDenominator 100561D8
data clipFarScale 100561DC
data clipRadius 100561E0
data animationRandomRange 100561E4
data piMin 10056218
data piMax 1005621C
data nativeOne 1014517C
data nativeZero 10145180
ext 020084C8 void getGroundTriangle(void*,u32,u32)
ext 02008974 f64 groundCross(void*,void*)
ext 02008DAC void destroyGroundCheck(void*,u32)
ext 02008E0C void constructGroundCheck(void*)
ext 0200F4FC void chaseAlpha(void*,u32,u32)
ext 0200F5C8 void chaseFloat(void*,f32,f32)
ext 0200F8D0 u32 chaseAngle(void*,u32,u32)
ext 02018428 void setMassHeight(void*,f32)
ext 020184DC void setMassRadius(void*,f32)
ext 020195B0 s32 cM_atan2s(f32,f32)
ext 020198D8 f64 cM_rndF(f32)
ext 0201ADE0 void subtractVector(void*,void*,void*)
ext 025157AC u32 dCcD_GetGObjInf(void*)
ext 0251694C void constructMassHitInfo(void*)
ext 025170D8 u32 checkMass(void*,void*,void*,void*)
ext 025200D4 u32 getGameInfo()
ext 02520540 u32 getReverb(u32)
ext 02555D0C u32 getEnvironmentLight()
ext 0255FAF0 void updateTevColors(void*)
ext 0255FFF4 void initializeTev(void*,u32,u32)
ext 025626A4 void settingTevStruct(void*,u32,u32,void*)
ext 025A5C04 void constructSmokeCallback(void*,u32,u32,u32,u32)
ext 025A847C u32 createParticle(void*,u32,u32,void*,u32,void*,u32,void*,u32,void*,void*,u32)
ext 025A8D10 u32 getSimpleParticleSlot(void*,u32)
ext 025A8D40 void setSimpleParticle(void*,u32,void*,u32,void*,void*,u32)
ext 025C6350 f64 checkGroundYCall(void*,void*)
ext 025C6558 s32 newAnimation(void*,u32)
ext 025C65CC void workCollision(void*,void*,u32,u32)
ext 025C6784 void workAttackNoCut(void*,void*,u32,u32,void*,void*)
ext 025C6A58 void workAttack(void*,void*,u32,u32,void*)
ext 025C6C8C void hitCheckCall(void*,void*,u32)
ext 025C6D40 void animateTree(void*,u32)
ext 025C738C void addRoomData(void*,void*)
ext 025C743C void drawTreeShadow(void*,void*)
ext 025C79D0 void setAnimation(void*,u32,u32)
ext 025C9BE8 void copyMatrixWords(void*,void*)
ext 025C9C88 void convertColor(void*,void*)
ext 025C9D68 void prepareTreeRender(void*)
ext 025CA9F8 void setTreeData(void*,void*,u32,void*,u32,u32)
ext 025CB374 void startShock(void*,u32,u32,void*)
ext 025DBE00 u32 fopOvlpM_IsPeek()
ext 025E1A40 void startObjectSound(u32,void*,u32,u32)
ext 025E1B34 void deleteObjectSound(void*)
ext 025F1884 void rotateY(void*,u32)
ext 025F1BF4 void concatenateXRotation(void*,u32)
ext 025F1C28 void concatenateYRotation(void*,u32)
ext 026124B0 u32 getTextureResource(void*,void*,void*,u32)
ext 0273AA24 void assertion(void*,u32,void*)
ext 0273AD10 u32 allocateObject(u32)
ext 0273AF40 void deleteObject(void*)
ext 0273B560 void initializeRoomList(void*,u32,void*)
ext 0273B7B4 void sortRoomList(void*,void*)
ext 0274D2AC void scaleColor(void*,f32)
ext 0274D458 void scaleColorCopy(void*,void*,f32)
ext 0274FBF8 void lockTextureCache(void*)
ext 0274FCCC void unlockTextureCache(void*)
ext 02750250 void initializeRenderState(void*)
ext 02750370 void applyRenderState(void*)
ext 02755FEC u32 findAllocationHeap(void*,void*)
ext 02756140 u32 getCurrentHeap(void*)
ext 02773870 void createTexture(void*,void*,void*)
ext 027B5430 void constructIndexBuffer(void*)
ext 027B54A0 void destroyIndexBuffer(void*,u32)
ext 027B54E0 void createIndexBuffer(void*,void*,u32,u32)
ext 027B5BD8 void constructVertexBuffer(void*)
ext 027B5CBC void destroyVertexBuffer(void*,u32)
ext 027B5E94 void invalidateVertexBuffer(void*,u32,u32)
ext 027B90AC s32 findMaterialIndex(void*,void*)
ext 027B9F68 void bindShaderTexture(void*)
ext 027BB7CC void bindShader(void*)
ext 027BB9E0 void updateShader(void*,u32)
ext 027BDEB4 void copyTextureState(void*,void*)
ext 027BDF7C void constructTextureState(void*)
ext 027BE2B0 void destroyTextureState(void*,u32)
ext 027BE53C void bindTextureSampler(void*,void*,u32,u32)
ext 027BE6B8 void constructTexture(void*)
ext 027BF734 void constructShape(void*)
ext 027BF7E8 void releaseShape(void*)
ext 027BF880 void destroyShape(void*,u32)
ext 027BFE5C void bindShape(void*)
ext 027F0E04 void entryImmediate(void*,void*,u32)
ext 027F1278 void constructPacketBase(void*)
ext 027F13DC void destroyPacketBase(void*,u32)
ext 027F29D4 u32 getMaterialCache(void*)
ext 027FB40C void constructDrawUniform(void*)
ext 027FB528 void destroyDrawUniform(void*,u32)
ext 027FB5D4 void setUniformSlot(void*,u32)
ext 027FB678 void updateDrawUniform(void*)
ext 027FD6F4 void constructUniformContainer(void*)
ext 027FD764 void destroyUniformContainer(void*,u32)
ext 027FD838 void setUniformContainerSlots(void*,u32,u32)
ext 027FDA54 void setViewUniform(void*,u32,void*,void*,void*)
ext 027FE010 void updateColorUniform(void*)
ext 027FF478 void createTreeVertexBuffer(void*,void*,u32,void*)
ext 027FF530 void createTreeShape(void*,void*,void*,void*,u32)
ext 027FFCBC u32 getMaterialCatalog()
ext 027FFE54 void applyDrawMaterial(void*,void*)
ext 0280037C void setMaterialRenderState(u32,void*)
ext 0280068C void createMaterialList(void*,void*,u32)
ext 02800B0C void createMaterial(void*,void*,u32)
ext 0281E5A8 void setParticleState(void*,void*)
ext 0283801C void calculateClipFrustum(void*)
ext 02838148 u32 clipTreeSphere(void*,void*,void*,f32)
ext 028E8DD0 f32 PSVECSquareMag(void*)
ext 028E8E64 void PSVECScale(void*,void*,f32)
ext 028E8EF0 void PSVECNormalize(void*,void*)
ext 028E8F64 void PSMTXMultVec(void*,void*,void*)
ext 028E90D4 void PSMTXCopy(void*,void*)
ext 028E9108 void PSMTXConcat(void*,void*,void*)
ext 028EAB2C void initializeHeaderFlag(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EFFD0 void constructArray(void*,u32,u32,u32)
ext 028F0164 void destroyArray(void*,u32,u32,u32,u32,u32)
ext 028F026C void registerGlobalObject(void*)
ext 028F4384 f64 squareRoot(f32)
ext 028F521C void zeroMemory(void*,u32)
ext C00060E0 void GX2CallDisplayList(void*,u32)
ext C0006178 void GX2DrawIndexed(u32,u32,u32,void*,u32,u32)
ext C00068A8 void GX2SetGeometryUniformReg(u32,u32,void*)
ext C0006900 void GX2SetPixelUniformReg(u32,u32,void*)
ext C0006A38 void GX2SetVertexUniformReg(u32,u32,void*)
*/
