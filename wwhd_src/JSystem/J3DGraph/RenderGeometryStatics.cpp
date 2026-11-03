// J3D HD renderer: shared primitive statics (sequential index buffer, unit quad) and the vertex constructor.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderGeometry.h"

struct RenderQuadVertices_l { j3d::RenderVertex_l mVertices[4]; };
struct RenderQuadIndices_l { be<u32> mIndices[4]; };
static j3d::RenderStaticRegistry sPrimitiveRegistry;
static u32 sPrimitiveRegistration;
static u32 sSequentialIndexRegistration;
static u32 sQuadVertexRegistration;
static u32 sQuadIndexRegistration;
static agl::IndexBufferLayout_l sSequentialIndexDescriptor;
static agl::IndexBufferLayout_l sQuadIndexDescriptor;
static agl::VertexBufferLayout_l sQuadVertexDescriptor;
static RenderQuadVertices_l sQuadVertices;
static RenderQuadIndices_l sQuadIndices;
using RenderIndexWord = be<u32>;
static RenderIndexWord* sSequentialIndices;
void* allocateRenderPrimitiveArray(u32,void*,s32);
void constructRenderIndexDescriptor(agl::IndexBufferLayout_l*);
void configureRenderIndexDescriptor(agl::IndexBufferLayout_l*,void*,u32,u32);
using RenderVertexConstruct = void (*)(j3d::RenderVertex_l*);
void constructRenderVertexArray(void*,u32,u32,RenderVertexConstruct);
void initializeRenderVertex(j3d::RenderVertex_l*);

/* 0280025C */
j3d::RenderVertex_l* j3d::RenderVertex_l::construct(RenderVertex_l* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) storage = static_cast<RenderVertex_l*>(allocateRenderGeometryObject(sizeof(RenderVertex_l)));
    if (storage == NULL) return NULL;
    // Component constructors allocate only if their own address is NULL;
    // they establish no values in a complete, non-wrapping vertex object.
    // Member offsets/sizes of the 13 components after the first (whose NULL test is dead here).
    const u32 offsets[13] = {0xC,0x18,0x28,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78,0x88};
    const u32 sizes[13] = {12,16,16,8,8,8,8,8,8,8,8,16,16};
    for (u32 component = 0; component < 13; ++component)
        if (j3d::memberWrapsToNull(storage, offsets[component])) allocateRenderGeometryObject(sizes[component]);
    return storage;
}

/* 028001B4 */
static void initializePrimitiveStatics() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sPrimitiveRegistry.mWords[2] = 0;
    sPrimitiveRegistry.mWords[0] = 0;
    sPrimitiveRegistry.mWords[3] = 0;
    sPrimitiveRegistry.mWords[1] = 0;
    registerRenderStatics(&sPrimitiveRegistration);
    constructRenderIndexDescriptor(&sSequentialIndexDescriptor);
    registerRenderStatics(&sSequentialIndexRegistration);
    constructRenderVertexArray(&sQuadVertices, 4, sizeof(j3d::RenderVertex_l), initializeRenderVertex);
    constructRenderVertexDescriptor(&sQuadVertexDescriptor);
    registerRenderStatics(&sQuadVertexRegistration);
    constructRenderIndexDescriptor(&sQuadIndexDescriptor);
    registerRenderStatics(&sQuadIndexRegistration);
}

/* 027FFCC8 */
static void preparePrimitiveStatics(void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sSequentialIndices = static_cast<be<u32>*>(allocateRenderPrimitiveArray(40000, heap, 4));
    for (u32 index = 0; index < 10000; ++index) sSequentialIndices[index] = index;
    configureRenderIndexDescriptor(&sSequentialIndexDescriptor, sSequentialIndices, GX2_INDEX_TYPE_U32, 10000);
    const f32 positions[4][2] = {{-0.5f,0.5f},{0.5f,0.5f},{0.5f,-0.5f},{-0.5f,-0.5f}};
    const f32 coordinates[4][2] = {{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f}};
    for (u32 vertex = 0; vertex < 4; ++vertex) {
        auto& value = sQuadVertices.mVertices[vertex];
        value.mLeadingValues[0] = positions[vertex][0];
        value.mLeadingValues[1] = positions[vertex][1];
        value.mLeadingValues[2] = 0.0f;
        value.mLeadingValues[3] = 0.0f;
        value.mLeadingValues[4] = 0.0f;
        value.mLeadingValues[5] = 1.0f;
        value.mTrailingValues[0] = coordinates[vertex][0];
        value.mTrailingValues[1] = coordinates[vertex][1];
    }
    sSequentialIndexDescriptor.mPrimitiveMode = GX2_PRIMITIVE_MODE_TRIANGLE_STRIP;
    j3d::RenderGeometryEntries::configureVertex(&sQuadVertexDescriptor, &sQuadVertices, 4);
    sQuadIndices.mIndices[0] = 0;
    sQuadIndices.mIndices[1] = 3;
    sQuadIndices.mIndices[3] = 2;
    sQuadIndices.mIndices[2] = 1;
    configureRenderIndexDescriptor(&sQuadIndexDescriptor, &sQuadIndices, GX2_INDEX_TYPE_U32, 4);
    sQuadIndexDescriptor.mPrimitiveMode = GX2_PRIMITIVE_MODE_TRIANGLE_STRIP;
}

/* WWHD glue
 data sPrimitiveRegistry 104B4A74
 data sPrimitiveRegistration 101F9A0C
 data sSequentialIndexRegistration 101F9A18
 data sQuadVertexRegistration 101F9A24
 data sQuadIndexRegistration 101F9A30
 data sSequentialIndexDescriptor 104B4A84
 data sQuadVertices 104B4A9C
 data sQuadIndexDescriptor 104B4CFC
 data sQuadIndices 104B4D14
 data sSequentialIndices 104B4D2C
 data sQuadVertexDescriptor 104B4D30
 ext 0273B0D4 void* allocateRenderPrimitiveArray(u32,void*,s32)
 ext 027B5430 void constructRenderIndexDescriptor(agl::IndexBufferLayout_l*)
 ext 027B54E0 void configureRenderIndexDescriptor(agl::IndexBufferLayout_l*,void*,u32,u32)
 ext 028EFFD0 void constructRenderVertexArray(void*,u32,u32,RenderVertexConstruct)
 ext 0280025C void initializeRenderVertex(j3d::RenderVertex_l*) callback
*/
