// Runtime storage prefix views, reconstructed from the game's field accesses only.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
#include "agl/DepthBlurContext.h"
#include "sead/RenderState.h"
#include "agl/PrimitiveShapes.h"
#include "agl/TextureResource.h"
#include "agl/DepthOfFieldDraw.h"
#include "agl/RootNode.h"
#include "port_native_draw.h"

// The depth-of-field object, its vignetting meshes and shape parameter objects (agl/DepthOfFieldDraw.h).
using agl::dof::DepthOfField;
using agl::dof::VignettingShapeParameter;
using agl::dof::VignettingMesh;
using agl::dof::VignettingVertex;
using ShapeInputsPointer = gptr<agl::PrimitiveShapeInputs>;
using ShapesPointer = gptr<agl::PrimitiveShapes>;
static ShapeInputsPointer sRuntimeShapeInputs; // primitive shape vertex inputs singleton
static ShapesPointer sRuntimeShapes; // primitive shapes singleton (quad index buffer)
static void activateRuntimeVertexBindings(agl::VertexInputSet* bindings);
static void drawRuntimeShapeIndices(u32 primitive, u32 count, u32 indexType, void* indices, s32 vertexOffset, u32 instances);
struct RuntimeMeshSegments { be<s32> mCounts[2]; };
struct RuntimeShapeRadii { be<f32> mValues[4]; };
using RegistryManagerPointer = gptr<sead::RegistryManagerState>;
static RegistryManagerPointer sRuntimeHeapManager; // heap registry manager singleton
static RuntimeMeshSegments sRuntimeMeshSegments;
static RuntimeShapeRadii sRuntimeShapeRadii;
static sead::RegistryHeap* getRuntimeAllocationHeap(sead::RegistryManagerState* manager);
static void initializeRuntimeIndexBuffer(agl::IndexBufferLayout* buffer, void* indices, u32 elementSize, u32 count);
static f64 runtimeShapeCos(f64 angle);
static f64 runtimeShapeSin(f64 angle);
static void initializeRuntimeVertexBuffer(agl::VertexBufferLayout* buffer, void* vertices, u32 stride, u32 bytes);
static void setRuntimeVertexAttribute(agl::VertexBufferLayout* buffer, u32 attribute, u32 format, u32 offset, u32 stream);
static void initializeRuntimeVertexBindings(agl::VertexInputSet* bindings, u32 streams, sead::RegistryHeap* heap);
static void bindRuntimeVertexAttribute(agl::VertexInputSet* bindings, u32 attribute, agl::VertexBufferLayout* buffer, u32 index);
static void finalizeRuntimeVertexBindings(agl::VertexInputSet* bindings);
static TextureCachePointer sRuntimeTextureCache; // dynamic texture cache singleton
// Text bytes of names and labels (guest strings bound by the data lines).
typedef be<u8> TextByte;
static TextByte sRuntimeBlurMipName;
static TextByte sRuntimeDepthMipName;
static agl::TextureCacheEntry* acquireRuntimeMipSurface(agl::DynamicTextureCache* cache, sead::SafeText* name, u32 format,
    u32 width, u32 height, u32 mipCount, u32 first, u32 second, u32 third, u32 fourth);
static void assignRuntimeMipSurface(agl::TextureSampler* destination, const agl::TextureData* source);
static ShaderHolderPointer sRuntimeShaderHolder; // shader program holder singleton
static f32 sRuntimeMinimumDepthRange;
using RuntimeCompositeColor = agl::env::ParameterColor;
static RuntimeCompositeColor sRuntimeDefaultCompositeColor;
static void constructRuntimeDrawState(sead::CompactRenderState* state);
static void applyRuntimeDrawState(sead::CompactRenderState* state);
static void clearRuntimeOutput(agl::RenderBuffer* output);
static void bindRuntimeOutputSource(agl::ColorSurface* color, agl::TextureData* source, u32 first, u32 second);
static void prepareRuntimeProgram(agl::ShaderProgram* program, u32 flags);
static void selectRuntimeShaderMode(u32 mode);
static void callRuntimeDisplayList(void* list, u32 size);
static void activateRuntimeProgram(agl::ShaderProgram* program);
static void setRuntimeVertexUniform(s32 location, u32 count, const void* values);
static void setRuntimePixelUniform(s32 location, u32 count, const void* values);
static void bindRuntimeSampler(agl::TextureSampler* sampler, agl::ProgramResource16_l* location, s32 slot, u32 flags);
static void setRuntimeOutputMip(void* runtime, agl::RenderBuffer* output, u32 mip, u32 secondaryOffset);
static u32 copyRuntimeTarget(ContextTargets* targets, u32 mode);
static void activateRuntimeOutputSampler(agl::ColorSurface* color);

using MeshDestructor = void (*)(VignettingMesh*, u32);
static u32 sDepthBlurRenderMethods;
static u32 sDepthBlurTaskMethods;
static sead::DisposerVtable sDepthBlurArchiveMethods;
static u32 sDepthBlurCategoryMethods;
static void destroyTextureArray(void* objects, u32 count, u32 stride, MeshDestructor destroy, u32 first, u32 second);
static void releaseDepthBlurArchive(agl::ResourceRecord* archive);
static void destroyDepthBlurArchiveBase(agl::ResourceRecord* archive, u32 flags);
static void destroyDepthBlurTextureBuffer(agl::TextureSampler* sampler, u32 flags);
using MeshConstructor = VignettingMesh* (*)(VignettingMesh*);
static void* allocateDepthBlurRuntimeStorage(u32 size);
static agl::parameter_tree::Value* constructParameterObject(agl::parameter_tree::Value* storage);
static void constructRuntimeParameterBase(void* parameter);
static void registerRuntimeParameter(void* parameter, sead::SafeText* name, sead::SafeText* label, sead::SafeText* description, void* archive);
static void constructRuntimeTextureBuffer(agl::TextureSampler* sampler);
static void constructRuntimeEmbeddedArchive(agl::ResourceRecord* archive);
static void constructRuntimeTextureArray(void* objects, u32 count, u32 stride, MeshConstructor construct);
static void notifyDepthBlurRuntimeChange(DepthOfField* runtime, u32 reason);
static void buildDepthBlurShapeVertices(DepthOfField* runtime, sead::RegistryHeap* heap);
static void buildDepthBlurShapeIndices(DepthOfField* runtime, sead::RegistryHeap* heap);
static void loadDepthBlurRuntimeArchive(agl::ResourceRecord* archive, void* configuration, sead::SafeText* name, void* heap);
static agl::parameter_tree::ValueVtable sRuntimeShapeMethods;
static u32 sRuntimeRootMethods;
static sead::SafeTextVtable sRuntimeStringMethods;
static u32 sRuntimeBooleanMethods;
static u32 sRuntimeIntegerMethods;
static u32 sRuntimeVector2Methods;
static TextByte sRuntimeIntegerDescription;
static TextByte sRuntimeVectorDescription;
static TextByte sRuntimeShapeTypeName;
static TextByte sRuntimeShapeRangeName;
static TextByte sRuntimeShapeScaleName;
static TextByte sRuntimeShapeTranslationName;
static TextByte sRuntimeShapeTypeLabel;
static TextByte sRuntimeShapeRangeLabel;
static TextByte sRuntimeShapeScaleLabel;
static TextByte sRuntimeShapeTranslationLabel;
static TextByte sRuntimeArchiveName;
static TextByte sRuntimeNearShapeName;
static TextByte sRuntimeFarShapeName;
static TextByte sRuntimeTaskDescription;
static TextByte sRuntimeEnableName;
static TextByte sRuntimeEnableLabel;
static TextByte sRuntimeEnableDescription;
static TextByte sRuntimeParameterObjectName;
static TextByte sRuntimeLoadArchiveName;
static void* constructRuntimeParameter(void* parameter, u32 size, const void* methods) {
    if (gabi::ea(parameter) == 0) parameter = allocateDepthBlurRuntimeStorage(size);
    if (parameter) {
        constructRuntimeParameterBase(parameter);
        static_cast<agl::env::ParameterBinding*>(parameter)->mMethods
            = reinterpret_cast<sead::VtableSlot*>(const_cast<void*>(methods));
    }
    return parameter;
}
static void initializeRuntimeString(sead::SafeText* string, TextByte* text) {
    string->mText = text;
    string->mVtable = &sRuntimeStringMethods;
}

/* 0277E7DC */
static VignettingShapeParameter* constructRuntimeShape(VignettingShapeParameter* shape, DepthOfField* owner, u32 index, sead::SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!shape) shape = static_cast<VignettingShapeParameter*>(allocateDepthBlurRuntimeStorage(sizeof(VignettingShapeParameter)));
    if (!shape) return nullptr;
    constructParameterObject(shape);
    shape->mVtable = &sRuntimeShapeMethods;
    gabi::FrameLocal<sead::SafeText> typeName(8), typeLabel(0x10), typeDescription(0x48);
    initializeRuntimeString(typeName, &sRuntimeShapeTypeName);
    initializeRuntimeString(typeLabel, &sRuntimeShapeTypeLabel);
    auto* type = static_cast<agl::parameter_value::IntegerParameter*>(constructRuntimeParameter(
        &shape->mShape.mType, 0x10, &sRuntimeIntegerMethods));
    if (type) {
        initializeRuntimeString(typeDescription, &sRuntimeIntegerDescription);
        registerRuntimeParameter(type, typeName, typeLabel, typeDescription, shape);
        type->mValue = 0;
    }
    gabi::FrameLocal<sead::SafeText> rangeName(0x18), rangeLabel(0x20), rangeDescription(0x50);
    initializeRuntimeString(rangeName, &sRuntimeShapeRangeName);
    initializeRuntimeString(rangeLabel, &sRuntimeShapeRangeLabel);
    auto* range = static_cast<agl::depth_blur_value::Vector2Parameter*>(constructRuntimeParameter(
        &shape->mShape.mRange, 0x14, &sRuntimeVector2Methods));
    if (range) {
        initializeRuntimeString(rangeDescription, &sRuntimeVectorDescription);
        registerRuntimeParameter(range, rangeName, rangeLabel, rangeDescription, shape);
        range->y = 1.0f;
        range->x = 0.25f;
    }
    gabi::FrameLocal<sead::SafeText> scaleName(0x28), scaleLabel(0x30), scaleDescription(0x58);
    initializeRuntimeString(scaleName, &sRuntimeShapeScaleName);
    initializeRuntimeString(scaleLabel, &sRuntimeShapeScaleLabel);
    auto* scale = static_cast<agl::depth_blur_value::Vector2Parameter*>(constructRuntimeParameter(
        &shape->mShape.mScale, 0x14, &sRuntimeVector2Methods));
    if (scale) {
        initializeRuntimeString(scaleDescription, &sRuntimeVectorDescription);
        registerRuntimeParameter(scale, scaleName, scaleLabel, scaleDescription, shape);
        scale->y = 1.0f;
        scale->x = 1.0f;
    }
    gabi::FrameLocal<sead::SafeText> translationName(0x38), translationLabel(0x40), translationDescription(0x60);
    initializeRuntimeString(translationName, &sRuntimeShapeTranslationName);
    initializeRuntimeString(translationLabel, &sRuntimeShapeTranslationLabel);
    auto* translation = static_cast<agl::depth_blur_value::Vector2Parameter*>(constructRuntimeParameter(
        &shape->mShape.mTranslation, 0x14, &sRuntimeVector2Methods));
    if (translation) {
        initializeRuntimeString(translationDescription, &sRuntimeVectorDescription);
        registerRuntimeParameter(translation, translationName, translationLabel, translationDescription, shape);
        translation->x = 0.0f;
        translation->y = 0.0f;
    }
    shape->mOwner = owner;
    shape->mIndex = index;
    owner->mParameterRoot.mTree.appendValue(shape, name);
    return shape;
}

/* 0277EA5C */
static DepthOfField* constructDepthBlurRuntime(DepthOfField* runtime) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!runtime) runtime = static_cast<DepthOfField*>(allocateDepthBlurRuntimeStorage(sizeof(DepthOfField)));
    if (!runtime) return nullptr;
    agl::dof::ContextHost::construct(runtime);
    gabi::FrameLocal<sead::SafeText> archiveName(8);
    initializeRuntimeString(archiveName, &sRuntimeArchiveName);
    agl::ParameterRoot::create(&runtime->mParameterRoot, archiveName, 0);
    runtime->mMethods = &sRuntimeRootMethods;
    runtime->mParameterRoot.mTree.mTable = gabi::ea(&sDepthBlurRenderMethods);
    runtime->mInputTexture = nullptr;
    runtime->mTaskMethods = &sDepthBlurTaskMethods;
    constructRuntimeTextureBuffer(&runtime->mIndirectSampler);
    runtime->mComposeMode = 0;
    runtime->mTextureScaleX = 1.0f;
    runtime->mTextureScaleY = 1.0f;
    constructRuntimeEmbeddedArchive(&runtime->mArchive);
    void* textures = runtime->mMeshes;
    if (gabi::ea(textures) == 0) textures = allocateDepthBlurRuntimeStorage(sizeof(runtime->mMeshes));
    if (textures) constructRuntimeTextureArray(textures, 2, sizeof(VignettingMesh), VignettingMesh::construct);
    runtime->mEnableNear = 1;
    runtime->mEnableFar = 1;
    constructRuntimeParameter(&runtime->mEnableParameter, 0x10, &sRuntimeBooleanMethods);
    gabi::FrameLocal<sead::SafeText> nearName(0x10);
    initializeRuntimeString(nearName, &sRuntimeNearShapeName);
    constructRuntimeShape(&runtime->mNearShape, runtime, 0, nearName);
    gabi::FrameLocal<sead::SafeText> farName(0x18);
    initializeRuntimeString(farName, &sRuntimeFarShapeName);
    constructRuntimeShape(&runtime->mFarShape, runtime, 1, farName);
    gabi::FrameLocal<sead::SafeText> taskDescription(0x20);
    initializeRuntimeString(taskDescription, &sRuntimeTaskDescription);
    agl::detail::RootNode::setNodeMeta(&runtime->mTaskMethods, taskDescription);
    return runtime;
}

/* 0277F32C */
static void initializeDepthBlurRuntime(DepthOfField* runtime, void* configuration, void* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    // The configuration word is the base's context count.
    runtime->initialize(s32(gabi::ea(configuration)), 0, static_cast<sead::RegistryHeap*>(heap));
    gabi::FrameLocal<sead::SafeText> label(8), name(0x10), description(0x18);
    initializeRuntimeString(description, &sRuntimeEnableDescription);
    initializeRuntimeString(label, &sRuntimeEnableLabel);
    initializeRuntimeString(name, &sRuntimeEnableName);
    registerRuntimeParameter(&runtime->mEnableParameter, name, label, description, runtime->mViewNames);
    runtime->mEnableParameter.mValue = 1;
    initializeRuntimeString(label, &sRuntimeParameterObjectName);
    runtime->mParameterRoot.mTree.appendValue(runtime->mViewNames, label);
    runtime->mSettings.updateShaderPrograms();
    runtime->mSettings.updateIndirectTransform();
    notifyDepthBlurRuntimeChange(runtime, 0);
    buildDepthBlurShapeVertices(runtime, static_cast<sead::RegistryHeap*>(heap));
    buildDepthBlurShapeIndices(runtime, static_cast<sead::RegistryHeap*>(heap));
    initializeRuntimeString(label, &sRuntimeLoadArchiveName);
    loadDepthBlurRuntimeArchive(&runtime->mArchive, configuration, label, heap);
}

static void writeRuntimeShapeIndex(VignettingMesh* mesh, u32 index, u32 value) {
    be<u16>* indices = mesh->mIndices;
    if (index < mesh->mIndexCapacity) indices += index;
    *indices = u16(value);
}

/* 0277F0F0 */
static void buildDepthBlurShapeIndices(DepthOfField* runtime, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 meshIndex = 0; meshIndex < 2; ++meshIndex) {
        const s32 segments = sRuntimeMeshSegments.mCounts[meshIndex];
        const s32 indexCount = s32(u32(segments) * 18);
        VignettingMesh* mesh = &runtime->mMeshes[meshIndex];
        if (indexCount > 0) {
            sead::RegistryHeap* allocationHeap = heap;
            if (!allocationHeap) allocationHeap = getRuntimeAllocationHeap(sRuntimeHeapManager);
            void* memory = gabi::call_ptr<void*>(allocationHeap->dispatch()->mAllocate.mFunction,
                allocationHeap, u32(indexCount) * 2, 4);
            if (memory) {
                mesh->mIndexCapacity = u32(indexCount);
                mesh->mIndices = static_cast<be<u16>*>(memory);
            }
        }
        u32 output = 0;
        u32 ringBase = 0;
        for (u32 band = 0; band < 3; ++band) {
            if (segments > 0) {
                const u32 nextRing = ringBase + u32(segments);
                for (s32 segment = 0; segment < segments; ++segment) {
                    const u32 nextSegment = u32((segment + 1) % segments);
                    writeRuntimeShapeIndex(mesh, output++, ringBase + u32(segment));
                    writeRuntimeShapeIndex(mesh, output++, nextRing + nextSegment);
                    writeRuntimeShapeIndex(mesh, output++, ringBase + nextSegment);
                    writeRuntimeShapeIndex(mesh, output++, ringBase + u32(segment));
                    writeRuntimeShapeIndex(mesh, output++, nextRing + u32(segment));
                    writeRuntimeShapeIndex(mesh, output++, nextRing + nextSegment);
                }
            }
            ringBase += u32(segments);
        }
        initializeRuntimeIndexBuffer(&mesh->mIndexBuffer, mesh->mIndices.get(), 4, mesh->mIndexCapacity);
        mesh->mIndexBuffer.mPrimitiveMode = 4;
    }
}

static VignettingVertex* runtimeShapeVertex(VignettingMesh* mesh, u32 index) {
    VignettingVertex* vertex = mesh->mVertices;
    if (index < mesh->mVertexCapacity) vertex += index;
    return vertex;
}

/* 0277EBF4 */
static void buildDepthBlurShapeVertices(DepthOfField* runtime, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    struct ConversionBias { be<u32> exponent, mantissa; };
    gabi::FrameLocal<ConversionBias> conversion(8);
    gabi::FrameLocal<RuntimeShapeRadii> radii(0x10);
    for (u32 i = 0; i < 4; ++i) radii->mValues[i] = sRuntimeShapeRadii.mValues[i];
    for (u32 meshIndex = 0; meshIndex < 2; ++meshIndex) {
        const s32 segments = sRuntimeMeshSegments.mCounts[meshIndex];
        const s32 vertexCount = s32(u32(segments) * 4);
        VignettingMesh* mesh = &runtime->mMeshes[meshIndex];
        if (vertexCount > 0) {
            sead::RegistryHeap* allocationHeap = heap;
            if (!allocationHeap) allocationHeap = getRuntimeAllocationHeap(sRuntimeHeapManager);
            void* memory = gabi::call_ptr<void*>(allocationHeap->dispatch()->mAllocate.mFunction,
                allocationHeap, u32(vertexCount) * 16, 4);
            if (memory) {
                mesh->mVertexCapacity = u32(vertexCount);
                mesh->mVertices = static_cast<VignettingVertex*>(memory);
            }
        }
        u32 output = 0;
        for (u32 ring = 0; ring < 4; ++ring) {
            if (meshIndex == 0) {
                if (segments > 0) {
                    conversion->mantissa = ring ^ 0x80000000;
                    conversion->exponent = 0x43300000;
                    conversion->mantissa = u32(segments) ^ 0x80000000;
                    for (s32 segment = 0; segment < segments; ++segment) {
                        conversion->exponent = 0x43300000;
                        conversion->mantissa = u32(segment) ^ 0x80000000;
                        const f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(f32(segment), 6.2831854820251465f), f32(segments));
                        VignettingVertex* position = runtimeShapeVertex(mesh, output);
                        const f64 cosine = runtimeShapeCos(gabi::fpr_from_f32(angle));
                        const f64 sine = runtimeShapeSin(gabi::fpr_from_f32(angle));
                        position->y = gabi::f32_from_fpr(sine);
                        position->x = gabi::f32_from_fpr(cosine);
                        VignettingVertex* attributes = runtimeShapeVertex(mesh, output);
                        const f32 radius = radii->mValues[ring];
                        attributes->ring = f32(ring);
                        attributes->radius = radius;
                        ++output;
                    }
                }
            } else {
                // Four corners in the original winding order, with independently reloaded attribute storage.
                static constexpr f32 x[4] = {-1.0f, -1.0f, 1.0f, 1.0f};
                static constexpr f32 y[4] = {1.0f, -1.0f, -1.0f, 1.0f};
                for (u32 corner = 0; corner < 4; ++corner) {
                    VignettingVertex* position = runtimeShapeVertex(mesh, output);
                    if (corner == 0) { position->x = x[corner]; position->y = y[corner]; }
                    else { position->y = y[corner]; position->x = x[corner]; }
                    VignettingVertex* attributes = runtimeShapeVertex(mesh, output);
                    if (corner == 0) {
                        conversion->exponent = 0x43300000;
                        conversion->mantissa = ring ^ 0x80000000;
                    }
                    const f32 radius = radii->mValues[ring];
                    if (corner == 3) { attributes->ring = f32(ring); attributes->radius = radius; }
                    else { attributes->radius = radius; attributes->ring = f32(ring); }
                    ++output;
                }
            }
        }
        initializeRuntimeVertexBuffer(&mesh->mVertexBuffer, mesh->mVertices.get(), 16, mesh->mVertexCapacity << 4);
        setRuntimeVertexAttribute(&mesh->mVertexBuffer, 0, 0x80D, 0, 0);
        setRuntimeVertexAttribute(&mesh->mVertexBuffer, 1, 0x80D, 8, 0);
        initializeRuntimeVertexBindings(&mesh->mVertexBindings, 1, heap);
        bindRuntimeVertexAttribute(&mesh->mVertexBindings, 0, &mesh->mVertexBuffer, 0);
        bindRuntimeVertexAttribute(&mesh->mVertexBindings, 1, &mesh->mVertexBuffer, 1);
        finalizeRuntimeVertexBindings(&mesh->mVertexBindings);
    }
}

static VignettingMesh* selectedRuntimeShape(DepthOfField* runtime, u32 shape) {
    return &runtime->mMeshes[shape < 2 ? shape : 0];
}
static void drawRuntimeIndexBuffer(agl::IndexBufferLayout* buffer) {
    const u32 count = buffer->mIndexCount;
#if PORT_NATIVE_DRAW
    // a depth-of-field draw as a packet of its own (port_native_draw.h)
    if (count && port_native_draw_single(PORT_NATIVE_POST, PORT_NATIVE_POST_DEPTH_BLUR, gabi::ea(buffer), 0, 0,
                                         buffer->mPrimitiveMode, count, buffer->mFormat,
                                         gabi::ea(static_cast<const void*>(buffer->mStorage)), 0, 1)) return;
#endif
    if (count) drawRuntimeShapeIndices(buffer->mPrimitiveMode, count, buffer->mFormat, buffer->mStorage, 0, 1);
}

/* 0277FD9C */
static void drawDepthBlurShape(DepthOfField* runtime, const DepthBlurContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    const u32 mode = context->mDrawMode;
    if (mode == 3) {
        agl::depth_blur_value::Parameters* parameters = context->mParameters;
        const u32 shape = parameters->usesTwoVignettingShapes()
            ? u32(parameters->mShapes[1].mType.mValue) : u32(parameters->mShapes[0].mType.mValue);
        activateRuntimeVertexBindings(&selectedRuntimeShape(runtime, shape)->mVertexBindings);
        drawRuntimeIndexBuffer(&selectedRuntimeShape(runtime, shape)->mIndexBuffer);
    } else if (mode == 2 && context->mParameters->mVignettingBlur.mValue) {
        const u32 shape = u32(context->mParameters->mShapes[0].mType.mValue);
        activateRuntimeVertexBindings(&selectedRuntimeShape(runtime, shape)->mVertexBindings);
        const u32 currentShape = u32(context->mParameters->mShapes[0].mType.mValue);
        drawRuntimeIndexBuffer(&selectedRuntimeShape(runtime, currentShape)->mIndexBuffer);
    } else {
        activateRuntimeVertexBindings(&sRuntimeShapeInputs->mInputs[7]);
        drawRuntimeIndexBuffer(&sRuntimeShapes->mIndexedShapes[1].mIndexBuffer);
    }
}

struct RuntimeConversionBias { be<u32> exponent, mantissa; };
static s32 roundedUpRuntimeLevel(f32 value, RuntimeConversionBias* conversion) {
    const s32 truncated = gabi::ftoi(value);
    conversion->exponent = 0x43300000;
    conversion->mantissa = u32(truncated) ^ 0x80000000;
    if (f32(truncated) != value && !(value < 0.0f)) return s32(u32(truncated) + 1);
    return truncated;
}
static u32 runtimeUnsignedExtent(f32 value, RuntimeConversionBias* conversion) {
    constexpr f32 signedLimit = 2147483648.0f;
    if (!(value < signedLimit)) {
        const s32 truncated = gabi::ftoi(gabi::fsubs_ppc(value, signedLimit));
        conversion->exponent = u32(truncated);
        return u32(truncated) + 0x80000000;
    }
    const s32 truncated = gabi::ftoi(value);
    conversion->exponent = u32(truncated);
    return u32(truncated);
}

/* 0277F6BC */
static void prepareDepthBlurSurfaces(DepthOfField* runtime, u32 view, u32 format, s32 width, s32 height) {
    gabi::AutoFrame frame{gabi::current_function()};
    ContextTargets* targets = runtime->mContexts;
    if (view < runtime->mContextArrayCount) targets += view;
    const u32 parameterIndex = view & u32(runtime->mViewIndexMask);
    gptr<agl::depth_blur_value::Parameters>* parameterSlot = runtime->mViewSettings;
    if (parameterIndex < runtime->mViewSettingsCount) parameterSlot += parameterIndex;
    agl::depth_blur_value::Parameters* parameters = *parameterSlot;
    agl::DynamicTextureCache* textureCache = sRuntimeTextureCache;
    const bool indirectFull = parameters->usesIndirectFromFull() != 0;
    const f32 level = parameters->mLevel.mValue;
    gabi::FrameLocal<RuntimeConversionBias> conversion(0x10);
    s32 mipCount = roundedUpRuntimeLevel(level, conversion);
    u32 textureWidth, textureHeight;
    if (indirectFull) {
        mipCount = s32(u32(mipCount) + 1);
        textureWidth = u32(width);
        textureHeight = u32(height);
    } else {
        conversion->exponent = 0x43300000;
        conversion->mantissa = u32(width) ^ 0x80000000;
        const f32 scale = parameters->mResourceScale;
        const f32 scaledWidth = gabi::fmuls_ppc(gabi::fmuls_ppc(f32(width), scale), 0.5f);
        if (mipCount < 1) mipCount = 1;
        textureWidth = runtimeUnsignedExtent(scaledWidth, conversion);
        conversion->exponent = 0x43300000;
        conversion->mantissa = u32(height) ^ 0x80000000;
        const f32 scaledHeight = gabi::fmuls_ppc(gabi::fmuls_ppc(f32(height), scale), 0.5f);
        textureHeight = runtimeUnsignedExtent(scaledHeight, conversion);
    }
    if (runtime->mSettings.mReduceDraw.mValue && format == 0x1A) format = 0x22;
    gabi::FrameLocal<sead::SafeText> blurName(0x18);
    initializeRuntimeString(blurName, &sRuntimeBlurMipName);
    agl::TextureCacheEntry* surface = acquireRuntimeMipSurface(textureCache, blurName, format,
        textureWidth, textureHeight, u32(mipCount), 0, 0, 1, 0);
    targets->mBlurSource = surface;
    if (contextSurfaceMatches(&targets->mBlurTarget, surface)) {
        void* image = surface->surface.mImage;
        void* mipmaps = surface->surface.mMipImage;
        nativeSurface(&targets->mBlurTarget)->mImage = image;
        targets->mBlurTarget.texture.surface.mImage = image;
        targets->mBlurTarget.texture.surface.mMipImage = mipmaps;
        nativeSurface(&targets->mBlurTarget)->mMipImage = mipmaps;
    } else {
        assignRuntimeMipSurface(&targets->mBlurTarget, surface);
    }
    if (!parameters->usesNearDepthBlur()) return;
    gabi::FrameLocal<sead::SafeText> depthName(0x20);
    initializeRuntimeString(depthName, &sRuntimeDepthMipName);
    const s32 depthMips = roundedUpRuntimeLevel(parameters->mDepthBlurAdd.mValue, conversion);
    surface = acquireRuntimeMipSurface(textureCache, depthName, 1, u32(width / 2), u32(height / 2),
        u32(depthMips) + 1, 0, 0, 1, 0);
    targets->mDepthSource = surface;
    if (contextSurfaceMatches(&targets->mDepthTarget, surface)) {
        void* image = surface->surface.mImage;
        void* mipmaps = surface->surface.mMipImage;
        nativeSurface(&targets->mDepthTarget)->mImage = image;
        nativeSurface(&targets->mDepthTarget)->mMipImage = mipmaps;
        targets->mDepthTarget.texture.surface.mImage = image;
        targets->mDepthTarget.texture.surface.mMipImage = mipmaps;
    } else {
        assignRuntimeMipSurface(&targets->mDepthTarget, surface);
    }
}

static u32 useRuntimeProgram(agl::ShaderProgram* program, u32 currentMode) {
    const u8 flags = program->flags;
    if (flags & 2) {
        program->flags = flags & ~2;
        prepareRuntimeProgram(program, 0);
    }
    const u32 mode = programSharedData(program)->mode;
    if (mode != currentMode) selectRuntimeShaderMode(mode);
    const u32 listSize = programDisplayList(program)->mUsed;
    if (listSize) callRuntimeDisplayList(programDisplayList(program)->mData, listSize);
    else activateRuntimeProgram(program);
    return mode;
}
static void uploadRuntimeUniform(agl::ShaderProgram* program, u32 index, u32 count, const void* values) {
    UniformLocations_l* uniform = programUniforms(program);
    if (index && program->uniformCount > index) uniform += index;
    if (uniform->mVertexRegister != -1) setRuntimeVertexUniform(uniform->mVertexRegister, count, values);
    if (uniform->mPixelRegister != -1) setRuntimePixelUniform(uniform->mPixelRegister, count, values);
}
static u32 runtimeShiftExtent(u32 extent, u32 mip) {
    const u32 shift = mip & 63;
    return shift < 32 ? extent >> shift : 0;
}

/* 0277FF88 */
static u32 drawDepthBlurMipChain(DepthOfField* runtime, const DepthBlurContext* context, u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    const bool indirectFull = context->mParameters->usesIndirectFromFull() != 0;
    ContextTargets* targets = context->mTargets;
    agl::TextureSampler* blur = &targets->mBlurTarget;
    agl::RenderBuffer* output = &targets->mMipOutput;
    gabi::FrameLocal<sead::CompactRenderState> state(0x30);
    constructRuntimeDrawState(state);
    const u8 writeAlpha = runtime->mSettings.mReduceDraw.mValue;
    state->mDepthWrite = 0;
    state->mWriteGreen = 1;
    state->mWriteBlue = 1;
    state->mDepthTest = 0;
    state->mWriteRed = 1;
    state->mWriteAlpha = writeAlpha;
    state->mBlendEnable = 0;
    applyRuntimeDrawState(state);
    clearRuntimeOutput(output);
    output->mDepth = nullptr;
    output->mColors[0] = &targets->mMipColor;
    bindRuntimeOutputSource(&targets->mMipColor, &blur->texture, 0, 0);
    agl::ShaderProgram* program = context->mParameters->mShaderPrograms[0];
    mode = useRuntimeProgram(program, mode);
    const u8 initialFlags = blur->dirty;
    blur->settings.modes[SamplerMode_MagnificationFilter] = 1;
    blur->settings.modes[SamplerMode_MinificationFilter] = 1;
    blur->settings.modes[SamplerMode_MipFilter] = 1;
    blur->dirty = initialFlags | SamplerDirty_Filters;
    u32 remaining = blur->texture.surface.mMipCount;
    u32 mip = 0;
    struct UnsignedBias { be<u32> exponent, value; };
    gabi::FrameLocal<UnsignedBias> conversion(8);
    gabi::FrameLocal<agl::depth_blur_value::Vector4> offsets(0x10), uniform(0x20);
    while (remaining) {
        if (indirectFull && mip == 0) {
            setRuntimeOutputMip(runtime, output, 0, 0);
            mode = copyRuntimeTarget(context->mTargets, mode);
            activateRuntimeOutputSampler(context->mTargets->mMipOutput.mColors[0]);
            mode = useRuntimeProgram(program, mode);
        } else {
            agl::TextureSampler* source;
            if (mip) {
                const u32 sourceMip = mip - 1;
                conversion->value = sourceMip;
                conversion->exponent = 0x43300000;
                const u8 flags = blur->dirty;
                blur->dirty = flags | SamplerDirty_Lod;
                blur->settings.scalarParameters[SamplerScalar_MinimumLod] = f32(sourceMip);
                blur->settings.scalarParameters[SamplerScalar_MaximumLod] = f32(sourceMip);
                blur->settings.scalarParameters[SamplerScalar_LodBias] = 0.0f;
                source = blur;
            } else source = &context->mTargets->mPrimary;
            bindRuntimeSampler(source, program->samplers, -1, 0);
            s32 width = s32(runtimeShiftExtent(blur->texture.surface.mWidth, mip));
            const s32 minimumWidth = blur->texture.minimumWidth;
            if (minimumWidth > width) width = minimumWidth;
            conversion->exponent = 0x43300000;
            conversion->value = u32(width);
            const f32 inverseWidth = gabi::fdivs_ppc(0.5f, f32(u32(width)));
            s32 height = s32(runtimeShiftExtent(blur->texture.surface.mHeight, mip));
            const s32 minimumHeight = blur->texture.minimumHeight;
            if (minimumHeight > height) height = minimumHeight;
            conversion->value = u32(height);
            const f32 scale = runtime->mTextureScaleX;
            offsets->x = gabi::fmuls_ppc(inverseWidth, scale);
            const f32 zeroOffset = gabi::fmuls_ppc(0.0f, scale);
            offsets->w = zeroOffset;
            offsets->z = zeroOffset;
            offsets->y = gabi::fmuls_ppc(gabi::fdivs_ppc(0.5f, f32(u32(height))), scale);
            uniform->x = offsets->x;
            uniform->y = offsets->y;
            uniform->z = offsets->z;
            uniform->w = offsets->w;
            uploadRuntimeUniform(program, 7, 4, uniform.get());
            setRuntimeOutputMip(runtime, output, mip, 0);
            drawDepthBlurShape(runtime, context);
            activateRuntimeOutputSampler(context->mTargets->mMipOutput.mColors[0]);
        }
        --remaining;
        ++mip;
    }
    const u8 samplerFlags = blur->dirty;
    const u32 levels = blur->texture.surface.mMipCount;
    blur->settings.modes[SamplerMode_MagnificationFilter] = 1;
    blur->settings.modes[SamplerMode_MinificationFilter] = 1;
    blur->settings.modes[SamplerMode_MipFilter] = 2;
    blur->dirty = samplerFlags | SamplerDirty_Filters;
    conversion->value = levels;
    conversion->exponent = 0x43300000;
    blur->dirty = samplerFlags | SamplerDirty_Filters | SamplerDirty_Lod;
    blur->settings.scalarParameters[SamplerScalar_MinimumLod] = 0.0f;
    blur->settings.scalarParameters[SamplerScalar_MaximumLod] = gabi::fsubs_ppc(f32(levels), 1.0f);
    blur->settings.scalarParameters[SamplerScalar_LodBias] = 0.0f;
    return mode;
}

static agl::ShaderProgram* selectRuntimeProgramVariant(u32 index, u32 variation) {
    agl::detail::ShaderHolder* holder = sRuntimeShaderHolder;
    u32 program = 0;
    if (u32(holder->mPrograms.mCount) > index) program = gabi::ea(holder->programSlots()[index].get());
    // Failed holder lookups preserve the original guest-null program field access.
    agl::ShaderProgramSharedData* shared = gabi::at<agl::ShaderProgramSharedData>(
        gabi::load<u32>(program + offsetof(agl::ShaderProgram, binary)));
    agl::ShaderVariantDomain* variants = &shared->variation;
    if (!variation) return variants->root;
    u32 entry = gabi::ea(variants->programs.get());
    if (variation - 1 < variants->programCount) entry += (variation - 1) * sizeof(agl::ShaderProgram);
    return gabi::at<agl::ShaderProgram>(entry);
}
static agl::ProgramResource16_l* runtimeProgramSampler(agl::ShaderProgram* program, u32 index) {
    agl::ProgramResource16_l* sampler = program->samplers;
    if (program->samplerCount > index) sampler += index;
    return sampler;
}

/* 027805A0 */
static u32 drawDepthBlurNearMipChain(DepthOfField* runtime, const DepthBlurContext* context, u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    ContextTargets* targets = context->mTargets;
    agl::TextureSampler* depth = &targets->mDepthTarget;
    agl::RenderBuffer* output = &targets->mMipOutput;
    gabi::FrameLocal<sead::CompactRenderState> state(0x60);
    constructRuntimeDrawState(state);
    state->mBlendEnable = 0;
    state->mWriteAlpha = 0;
    state->mWriteGreen = 0;
    state->mWriteBlue = 0;
    state->mWriteRed = 1;
    state->mDepthTest = 0;
    state->mDepthWrite = 0;
    applyRuntimeDrawState(state);
    clearRuntimeOutput(output);
    output->mDepth = nullptr;
    output->mColors[0] = &targets->mMipColor;
    bindRuntimeOutputSource(&targets->mMipColor, &depth->texture, 0, 0);
    const u8 flags = depth->dirty;
    depth->settings.modes[SamplerMode_MagnificationFilter] = 1;
    depth->settings.modes[SamplerMode_MinificationFilter] = 1;
    depth->dirty = flags | SamplerDirty_Filters;
    depth->settings.modes[SamplerMode_MipFilter] = 1;
    agl::ShaderProgram* program = selectRuntimeProgramVariant(45, context->mFlags);
    mode = useRuntimeProgram(program, mode);
    auto* parameters = context->mParameters.get();
    const f32 end = parameters->mFarEnd.mValue;
    const f32 start = parameters->mFarStart.mValue;
    const f32 order = gabi::fsubs_ppc(end, start);
    f32 maximum = order >= 0.0f ? end : start;
    const f32 minimum = order >= 0.0f ? start : end;
    const f32 range = gabi::fsubs_ppc(maximum, minimum);
    const f32 minimumRange = sRuntimeMinimumDepthRange;
    if (range < minimumRange) maximum = gabi::fadds_ppc(maximum, minimumRange);
    const bool perspective = context->mFlags != 0;
    const f32 farPlane = context->mFarDepth;
    const f32 nearPlane = context->mNearDepth;
    f32 depthScale, depthOffset;
    const f32 planeRatio = gabi::fdivs_ppc(nearPlane, farPlane);
    const f32 reciprocalNear = gabi::fdivs_ppc(1.0f, nearPlane);
    const f32 planeScale = gabi::fdivs_ppc(gabi::fsubs_ppc(1.0f, planeRatio), nearPlane);
    if (perspective) {
        const f32 distance = gabi::fsubs_ppc(maximum, nearPlane);
        const f32 reversedRange = gabi::fsubs_ppc(minimum, maximum);
        const f32 planeRange = gabi::fsubs_ppc(farPlane, nearPlane);
        const f32 normalizedDistance = gabi::fdivs_ppc(distance, planeRange);
        depthScale = gabi::fdivs_ppc(planeRange, reversedRange);
        const f32 offset = gabi::fmuls_ppc(normalizedDistance, depthScale);
        depthOffset = gabi::f32_from_bits(gabi::f32_bits(offset) ^ 0x80000000);
    } else {
        depthScale = gabi::fdivs_ppc(1.0f, gabi::fsubs_ppc(minimum, maximum));
        const f32 offset = gabi::fmuls_ppc(maximum, depthScale);
        depthOffset = gabi::f32_from_bits(gabi::f32_bits(offset) ^ 0x80000000);
    }
    gabi::FrameLocal<agl::depth_blur_value::Vector4> depthUniform(0x20);
    depthUniform->y = planeScale;
    depthUniform->w = depthOffset;
    depthUniform->z = depthScale;
    depthUniform->x = reciprocalNear;
    uploadRuntimeUniform(program, 6, 4, depthUniform.get());
    bindRuntimeSampler(&context->mTargets->mSecondary, runtimeProgramSampler(program, 1), -1, 0);
    struct UnsignedBias { be<u32> exponent, value; };
    gabi::FrameLocal<UnsignedBias> conversion(8);
    s32 width = depth->texture.minimumWidth;
    if (s32(depth->texture.surface.mWidth) > width) width = s32(depth->texture.surface.mWidth);
    conversion->exponent = 0x43300000;
    conversion->value = u32(width);
    s32 height = s32(depth->texture.surface.mHeight);
    const s32 minimumHeight = depth->texture.minimumHeight;
    if (minimumHeight > height) height = minimumHeight;
    const f32 inverseWidth = gabi::fdivs_ppc(0.5f, f32(u32(width)));
    conversion->value = u32(height);
    const f32 inverseHeight = gabi::fdivs_ppc(0.5f, f32(u32(height)));
    const f32 scale = runtime->mTextureScaleX;
    const f32 zeroOffset = gabi::fmuls_ppc(0.0f, scale);
    gabi::FrameLocal<agl::depth_blur_value::Vector4> offsets(0x30), initialUniform(0x50);
    offsets->w = zeroOffset;
    depthUniform->x = inverseWidth;
    offsets->x = gabi::fmuls_ppc(inverseWidth, scale);
    initialUniform->x = offsets->x;
    offsets->z = zeroOffset;
    depthUniform->w = 0.0f;
    depthUniform->y = inverseHeight;
    depthUniform->z = 0.0f;
    offsets->y = gabi::fmuls_ppc(inverseHeight, scale);
    initialUniform->y = offsets->y;
    initialUniform->z = offsets->z;
    initialUniform->w = offsets->w;
    uploadRuntimeUniform(program, 7, 4, initialUniform.get());
    setRuntimeOutputMip(runtime, output, 0, 0);
    drawDepthBlurShape(runtime, context);
    activateRuntimeOutputSampler(context->mTargets->mMipOutput.mColors[0]);
    program = context->mParameters->mShaderPrograms[1];
    mode = useRuntimeProgram(program, mode);
    const u32 levelCount = depth->texture.surface.mMipCount;
    gabi::FrameLocal<agl::depth_blur_value::Vector4> loopOffsets(0x10), loopUniform(0x40);
    for (u32 mip = 1; mip < levelCount; ++mip) {
        const u32 previous = mip - 1;
        conversion->value = previous;
        conversion->exponent = 0x43300000;
        const u8 samplerFlags = depth->dirty;
        depth->dirty = samplerFlags | SamplerDirty_Lod;
        depth->settings.scalarParameters[SamplerScalar_MinimumLod] = f32(previous);
        depth->settings.scalarParameters[SamplerScalar_MaximumLod] = f32(previous);
        depth->settings.scalarParameters[SamplerScalar_LodBias] = 0.0f;
        bindRuntimeSampler(depth, runtimeProgramSampler(program, 1), -1, 0);
        s32 mipWidth = s32(runtimeShiftExtent(depth->texture.surface.mWidth, mip));
        const s32 minimumWidth = depth->texture.minimumWidth;
        if (minimumWidth > mipWidth) mipWidth = minimumWidth;
        conversion->value = u32(mipWidth);
        conversion->exponent = 0x43300000;
        s32 mipHeight = s32(runtimeShiftExtent(depth->texture.surface.mHeight, mip));
        const s32 minimumMipHeight = depth->texture.minimumHeight;
        if (minimumMipHeight > mipHeight) mipHeight = minimumMipHeight;
        conversion->value = u32(mipHeight);
        const f32 stepX = gabi::fdivs_ppc(0.5f, f32(u32(mipWidth)));
        const f32 currentScale = runtime->mTextureScaleX;
        const f32 zero = gabi::fmuls_ppc(0.0f, currentScale);
        loopOffsets->z = zero;
        loopOffsets->x = gabi::fmuls_ppc(stepX, currentScale);
        loopUniform->x = loopOffsets->x;
        loopOffsets->w = zero;
        loopOffsets->y = gabi::fmuls_ppc(gabi::fdivs_ppc(0.5f, f32(u32(mipHeight))), currentScale);
        loopUniform->y = loopOffsets->y;
        loopUniform->z = loopOffsets->z;
        loopUniform->w = loopOffsets->w;
        uploadRuntimeUniform(program, 7, 4, loopUniform.get());
        setRuntimeOutputMip(runtime, output, mip, 0);
        drawDepthBlurShape(runtime, context);
        activateRuntimeOutputSampler(context->mTargets->mMipOutput.mColors[0]);
    }
    parameters = context->mParameters;
    const u8 finalFlags = depth->dirty;
    const f32 requestedLod = parameters->mDepthBlurAdd.mValue;
    depth->settings.modes[SamplerMode_MagnificationFilter] = 1;
    depth->settings.scalarParameters[SamplerScalar_MinimumLod] = requestedLod;
    depth->settings.scalarParameters[SamplerScalar_MaximumLod] = requestedLod;
    depth->settings.scalarParameters[SamplerScalar_LodBias] = 0.0f;
    depth->settings.modes[SamplerMode_MinificationFilter] = 1;
    depth->dirty = finalFlags | SamplerDirty_Filters | SamplerDirty_Lod;
    depth->settings.modes[SamplerMode_MipFilter] = 1;
    return mode;
}

static f32 negateRuntimeFloat(f32 value) {
    return gabi::f32_from_bits(gabi::f32_bits(value) ^ 0x80000000);
}
static f32 runtimeDepthBoundary(f32 nearPlane, f32 farPlane, f32 distance) {
    const f32 twiceNear = gabi::fadds_ppc(nearPlane, nearPlane);
    const f32 planeRange = gabi::fsubs_ppc(farPlane, nearPlane);
    const f32 planeSum = gabi::fadds_ppc(farPlane, nearPlane);
    const f32 twiceNearSquared = gabi::fmuls_ppc(twiceNear, nearPlane);
    const f32 scaledDistance = gabi::fmuls_ppc(nearPlane, distance);
    const f32 farProduct = gabi::fmuls_ppc(twiceNearSquared, farPlane);
    const f32 denominator = gabi::fmuls_ppc(scaledDistance, planeRange);
    const f32 numerator = gabi::fmsubs(scaledDistance, planeSum, farProduct);
    return gabi::fdivs_ppc(numerator, denominator);
}

/* 02780D1C */
static void uploadDepthBlurComposite(DepthOfField* runtime, const DepthBlurContext* context, agl::ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<agl::depth_blur_value::Vector4> scales(0x20), offsets(0x30);
    const f32 defaultRed = sRuntimeDefaultCompositeColor.r;
    const f32 defaultGreen = sRuntimeDefaultCompositeColor.g;
    const f32 defaultBlue = sRuntimeDefaultCompositeColor.b;
    const f32 defaultAlpha = sRuntimeDefaultCompositeColor.a;
    scales->x = defaultRed;
    scales->y = defaultGreen;
    scales->z = defaultBlue;
    offsets->x = defaultRed;
    offsets->y = defaultGreen;
    offsets->z = defaultBlue;
    offsets->w = defaultAlpha;
    scales->w = defaultAlpha;
    struct SignedBias { be<u32> exponent, value; };
    gabi::FrameLocal<SignedBias> conversion(8);
    const s32 width = context->mWidth;
    conversion->exponent = 0x43300000;
    conversion->value = u32(width) ^ 0x80000000;
    auto* parameters = context->mParameters.get();
    const s32 height = context->mHeight;
    const f32 end = parameters->mEnd.mValue;
    const f32 start = parameters->mStart.mValue;
    conversion->value = u32(height) ^ 0x80000000;
    const f32 order = gabi::fsubs_ppc(end, start);
    f32 maximum = order >= 0.0f ? end : start;
    const f32 minimum = order >= 0.0f ? start : end;
    const f32 inverseWidth = gabi::fdivs_ppc(1.0f, f32(width));
    f32 range = gabi::fsubs_ppc(maximum, minimum);
    const f32 epsilon = sRuntimeMinimumDepthRange;
    const bool perspective = context->mFlags != 0;
    const f32 inverseHeight = gabi::fdivs_ppc(1.0f, f32(height));
    if (range < epsilon) {
        maximum = gabi::fadds_ppc(maximum, epsilon);
        range = gabi::fsubs_ppc(maximum, minimum);
    }
    const f32 farMaximum = parameters->mFarMaximumDepth.mValue;
    const f32 level = parameters->mLevel.mValue;
    const f32 saturation = gabi::fsubs_ppc(1.0f, parameters->mSaturationMinimum.mValue);
    gabi::FrameLocal<agl::depth_blur_value::Vector4> uniform(0x10);
    uniform->x = level;
    uniform->z = farMaximum;
    uniform->w = saturation;
    uniform->y = level;
    uploadRuntimeUniform(program, 0, 4, uniform.get());
    const f32 textureScale = runtime->mTextureScaleY;
    uniform->w = inverseHeight;
    uniform->y = gabi::fmuls_ppc(inverseHeight, textureScale);
    uniform->z = inverseWidth;
    uniform->x = gabi::fmuls_ppc(inverseWidth, textureScale);
    uploadRuntimeUniform(program, 7, 4, uniform.get());
    if (perspective) {
        const f32 farPlane = context->mFarDepth;
        const f32 nearPlane = context->mNearDepth;
        const f32 planeRange = gabi::fsubs_ppc(farPlane, nearPlane);
        const f32 distance = gabi::fsubs_ppc(minimum, nearPlane);
        const f32 scale = gabi::fdivs_ppc(planeRange, range);
        const f32 normalized = gabi::fdivs_ppc(distance, planeRange);
        scales->x = scale;
        offsets->x = negateRuntimeFloat(gabi::fmuls_ppc(normalized, scale));
    } else {
        const f32 scale = gabi::fdivs_ppc(1.0f, range);
        scales->x = scale;
        offsets->x = negateRuntimeFloat(gabi::fmuls_ppc(minimum, scale));
    }
    f32 nearPlane = context->mNearDepth;
    f32 farPlane = context->mFarDepth;
    const f32 boundary = runtimeDepthBoundary(nearPlane, farPlane, minimum);
    parameters = context->mParameters;
    f32 nearBoundary = 0.0f;
    if (parameters->mNear.mValue) {
        const f32 nearStart = parameters->mFarStart.mValue;
        const f32 nearEnd = parameters->mFarEnd.mValue;
        const f32 nearOrder = gabi::fsubs_ppc(nearEnd, nearStart);
        f32 nearMaximum = nearOrder >= 0.0f ? nearEnd : nearStart;
        const f32 nearMinimum = nearOrder >= 0.0f ? nearStart : nearEnd;
        const f32 nearRange = gabi::fsubs_ppc(nearMaximum, nearMinimum);
        const f32 minimumRange = sRuntimeMinimumDepthRange;
        if (nearRange < minimumRange) nearMaximum = gabi::fadds_ppc(nearMaximum, minimumRange);
        const f32 reversedRange = gabi::fsubs_ppc(nearMinimum, nearMaximum);
        if (perspective) {
            const f32 distance = gabi::fsubs_ppc(nearMaximum, nearPlane);
            const f32 planeRange = gabi::fsubs_ppc(farPlane, nearPlane);
            const f32 normalized = gabi::fdivs_ppc(distance, planeRange);
            const f32 scale = gabi::fdivs_ppc(planeRange, reversedRange);
            scales->y = scale;
            offsets->y = negateRuntimeFloat(gabi::fmuls_ppc(normalized, scale));
        } else {
            const f32 scale = gabi::fdivs_ppc(1.0f, reversedRange);
            scales->y = scale;
            offsets->y = negateRuntimeFloat(gabi::fmuls_ppc(nearMaximum, scale));
        }
        nearPlane = context->mNearDepth;
        farPlane = context->mFarDepth;
        nearBoundary = runtimeDepthBoundary(nearPlane, farPlane, nearMaximum);
    }
    gabi::FrameLocal<agl::depth_blur_value::Vector4> planes(0x40);
    planes->x = gabi::fdivs_ppc(1.0f, nearPlane);
    const f32 planeRatio = gabi::fdivs_ppc(nearPlane, farPlane);
    planes->z = boundary;
    planes->w = nearBoundary;
    planes->y = gabi::fdivs_ppc(gabi::fsubs_ppc(1.0f, planeRatio), nearPlane);
    uploadRuntimeUniform(program, 6, 4, planes.get());
    parameters = context->mParameters;
    if (parameters->mColorControl.mValue) {
        const f32 first = parameters->mColorControlDepth.mValue.r;
        const f32 second = parameters->mColorControlDepth.mValue.g;
        const f32 firstOrder = gabi::fsubs_ppc(second, first);
        const f32 low = firstOrder >= 0.0f ? first : second;
        f32 high = firstOrder >= 0.0f ? second : first;
        f32 firstRange = gabi::fsubs_ppc(high, low);
        const f32 minimumRange = sRuntimeMinimumDepthRange;
        const f32 fourth = parameters->mColorControlDepth.mValue.a;
        if (firstRange < minimumRange) {
            high = gabi::fadds_ppc(high, minimumRange);
            firstRange = gabi::fsubs_ppc(high, low);
        }
        const f32 third = parameters->mColorControlDepth.mValue.b;
        const f32 secondOrder = gabi::fsubs_ppc(fourth, third);
        const f32 secondHigh = secondOrder >= 0.0f ? fourth : third;
        f32 secondLow = secondOrder >= 0.0f ? third : fourth;
        f32 secondRange = gabi::fsubs_ppc(secondLow, secondHigh);
        if (secondRange < minimumRange) {
            secondLow = gabi::fadds_ppc(secondLow, minimumRange);
            secondRange = gabi::fsubs_ppc(secondLow, secondHigh);
        }
        f32 firstScale, secondScale, firstOffset, secondOffset;
        if (perspective) {
            const f32 colorNear = context->mNearDepth;
            const f32 colorFar = context->mFarDepth;
            const f32 planeRange = gabi::fsubs_ppc(colorFar, colorNear);
            const f32 highDistance = gabi::fsubs_ppc(secondHigh, colorNear);
            const f32 normalizedFirst = gabi::fdivs_ppc(firstRange, planeRange);
            const f32 lowDistance = gabi::fsubs_ppc(low, colorNear);
            firstScale = gabi::fdivs_ppc(1.0f, normalizedFirst);
            const f32 normalizedLow = gabi::fdivs_ppc(lowDistance, planeRange);
            firstOffset = negateRuntimeFloat(gabi::fmuls_ppc(normalizedLow, firstScale));
            const f32 normalizedSecond = gabi::fdivs_ppc(secondRange, planeRange);
            secondScale = gabi::fdivs_ppc(1.0f, normalizedSecond);
            const f32 normalizedHigh = gabi::fdivs_ppc(highDistance, planeRange);
            secondOffset = negateRuntimeFloat(gabi::fmuls_ppc(normalizedHigh, secondScale));
        } else {
            firstScale = gabi::fdivs_ppc(1.0f, firstRange);
            secondScale = gabi::fdivs_ppc(1.0f, secondRange);
            firstOffset = negateRuntimeFloat(gabi::fmuls_ppc(low, firstScale));
            secondOffset = negateRuntimeFloat(gabi::fmuls_ppc(secondHigh, secondScale));
        }
        scales->z = firstScale;
        scales->w = secondScale;
        offsets->z = firstOffset;
        offsets->w = secondOffset;
        if (!parameters->mColorReverse.mValue) {
            offsets->w = 1.0f;
            scales->w = 1.0f;
        }
    }
    uploadRuntimeUniform(program, 4, 4, scales.get());
    uploadRuntimeUniform(program, 5, 4, offsets.get());
    uploadRuntimeUniform(program, 12, 4, &context->mParameters->mFarMultiplyColorParameter.mValue);
    bindRuntimeSampler(&context->mTargets->mBlurTarget, runtimeProgramSampler(program, 2), -1, 0);
    if (context->mParameters->usesNearDepthBlur())
        bindRuntimeSampler(&context->mTargets->mDepthTarget, runtimeProgramSampler(program, 3), -1, 0);
    bindRuntimeSampler(&context->mTargets->mSecondary, runtimeProgramSampler(program, 1), -1, 0);
    if (context->mParameters->usesIndirectTexture()) {
        bindRuntimeSampler(&runtime->mIndirectSampler, runtimeProgramSampler(program, 4), -1, 0);
        agl::TextureData* texture = runtime->mInputTexture;
        s32 textureWidth = texture->minimumWidth;
        if (s32(texture->surface.mWidth) > textureWidth) textureWidth = s32(texture->surface.mWidth);
        conversion->exponent = 0x43300000;
        conversion->value = u32(textureWidth);
        context->mParameters->mIndirectColor.r = f32(u32(textureWidth));
        texture = runtime->mInputTexture;
        s32 textureHeight = texture->minimumHeight;
        if (s32(texture->surface.mHeight) > textureHeight) textureHeight = s32(texture->surface.mHeight);
        conversion->value = u32(textureHeight);
        context->mParameters->mIndirectColor.g = f32(u32(textureHeight));
        const s32 outputHeight = context->mHeight;
        conversion->value = u32(outputHeight) ^ 0x80000000;
        const f32 aspectScale = gabi::fmuls_ppc(inverseWidth, f32(outputHeight));
        context->mParameters->mIndirectColor.b = gabi::fdivs_ppc(1.0f, aspectScale);
        parameters = context->mParameters;
        parameters->mIndirectColor.a = parameters->mIndirectStrength.mValue;
        uploadRuntimeUniform(program, 1, 4, &context->mParameters->mIndirectColor);
        uploadRuntimeUniform(program, 2, 4, &context->mParameters->mIndirectTransformRow0);
        uploadRuntimeUniform(program, 3, 4, &context->mParameters->mIndirectTransformRow1);
    }
}

/* 0277D290 */
static void destroyDepthBlurRuntime(DepthOfField* runtime, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!runtime) return;
    runtime->mParameterRoot.mTree.mTable = gabi::ea(&sDepthBlurRenderMethods);
    runtime->mTaskMethods = &sDepthBlurTaskMethods;
    destroyTextureArray(runtime->mMeshes, 2, sizeof(runtime->mMeshes[0]), VignettingMesh::destroy, 0, 0);
    runtime->mArchive.mVtable = &sDepthBlurArchiveMethods;
    runtime->mArchive.mInterfaceMethods = &sDepthBlurCategoryMethods;
    releaseDepthBlurArchive(&runtime->mArchive);
    destroyDepthBlurArchiveBase(&runtime->mArchive, 0);
    destroyDepthBlurTextureBuffer(&runtime->mIndirectSampler, 2);
    agl::dof::ContextHost::destroy(runtime, 0);
    if (flags & 1) operator_delete(runtime);
}

/* 0277DA88 */
static void resetDepthBlurTargets(void*, ContextTargets* targets) {
    gabi::AutoFrame frame{gabi::current_function()};
    const u8 primarySamplerFlags = targets->mPrimary.dirty;
    const u8 blurSamplerFlags = targets->mBlurTarget.dirty;
    const u8 secondarySamplerFlags = targets->mSecondary.dirty;
    const u8 combinedSamplerFlags = targets->mCombined.dirty;
    targets->mBlurTarget.settings.modes[SamplerMode_MinificationFilter] = 1;
    targets->mPrimary.dirty = primarySamplerFlags | SamplerDirty_Filters;
    targets->mPrimary.overrideEnabled = 1;
    targets->mSecondary.dirty = secondarySamplerFlags | SamplerDirty_Filters;
    targets->mCombined.settings.modes[SamplerMode_MinificationFilter] = 1;
    targets->mDepthTarget.settings.modes[SamplerMode_MipFilter] = 1;
    const u8 depthSamplerFlags = targets->mDepthTarget.dirty;
    targets->mSecondary.settings.modes[SamplerMode_MinificationFilter] = 1;
    targets->mDepthTarget.settings.modes[SamplerMode_MinificationFilter] = 1;
    targets->mSecondary.settings.modes[SamplerMode_MagnificationFilter] = 1;
    targets->mBlurTarget.settings.modes[SamplerMode_MipFilter] = 1;
    targets->mCombined.settings.modes[SamplerMode_MipFilter] = 1;
    targets->mBlurTarget.settings.modes[SamplerMode_MagnificationFilter] = 1;
    targets->mDepthTarget.settings.modes[SamplerMode_MagnificationFilter] = 1;
    targets->mCombined.settings.modes[SamplerMode_MagnificationFilter] = 1;
    targets->mPrimary.settings.modes[SamplerMode_MinificationFilter] = 1;
    targets->mPrimary.settings.modes[SamplerMode_MipFilter] = 0;
    targets->mCombined.overrideEnabled = 1;
    targets->mSecondary.settings.modes[SamplerMode_MipFilter] = 0;
    targets->mPrimary.settings.modes[SamplerMode_MagnificationFilter] = 1;
    targets->mBlurSource = nullptr;
    targets->mSecondary.overrideEnabled = 1;
    targets->mDepthSource = nullptr;
    targets->mBlurTarget.dirty = blurSamplerFlags | SamplerDirty_Filters;
    targets->mBlurTarget.overrideEnabled = 1;
    targets->mCombined.dirty = combinedSamplerFlags | SamplerDirty_Filters;
    targets->mDepthTarget.dirty = depthSamplerFlags | SamplerDirty_Filters;
}


/* WWHD glue
 data sRuntimeShapeMethods 10163024 nocheck
 data sRuntimeRootMethods 10162FEC nocheck
 data sRuntimeStringMethods 10162618 nocheck
 data sRuntimeBooleanMethods 101626F8 nocheck
 data sRuntimeIntegerMethods 10162770 nocheck
 data sRuntimeVector2Methods 10162860 nocheck
 data sRuntimeIntegerDescription 10162611 nocheck
 data sRuntimeVectorDescription 10162614 nocheck
 data sRuntimeShapeTypeName 10162F00 nocheck
 data sRuntimeShapeRangeName 10162EE8 nocheck
 data sRuntimeShapeScaleName 10162EF0 nocheck
 data sRuntimeShapeTranslationName 10162EF8 nocheck
 data sRuntimeShapeTypeLabel 10162F08 nocheck
 data sRuntimeShapeRangeLabel 10162F10 nocheck
 data sRuntimeShapeScaleLabel 10162F18 nocheck
 data sRuntimeShapeTranslationLabel 10162F24 nocheck
 data sRuntimeArchiveName 10162F30 nocheck
 data sRuntimeNearShapeName 10162F48 nocheck
 data sRuntimeFarShapeName 10162F5C nocheck
 data sRuntimeTaskDescription 10162F38 nocheck
 data sRuntimeEnableName 10162F8C nocheck
 data sRuntimeEnableLabel 10162F94 nocheck
 data sRuntimeEnableDescription 10162610 nocheck
 data sRuntimeParameterObjectName 10162F88 nocheck
 data sRuntimeLoadArchiveName 10162F9C nocheck
 data sRuntimeHeapManager 101F8B4C
 data sRuntimeMeshSegments 10162608
 data sRuntimeShapeRadii 10162F78
 data sRuntimeShapeInputs 101F8DDC
 data sRuntimeShapes 101F8DE4
 data sRuntimeTextureCache 101F8DD4
 data sRuntimeBlurMipName 10162FB4 nocheck
 data sRuntimeDepthMipName 10162FC4 nocheck
 data sRuntimeShaderHolder 101F8D6C
 data sRuntimeMinimumDepthRange 101F8C0C
 data sRuntimeDefaultCompositeColor 104A0964
 data sDepthBlurRenderMethods 10163064 nocheck
 data sDepthBlurTaskMethods 10163014 nocheck
 data sDepthBlurArchiveMethods 101626A8 nocheck
 data sDepthBlurCategoryMethods 101626B8 nocheck
 ext 028F0164 void destroyTextureArray(void* objects, u32 count, u32 stride, MeshDestructor destroy, u32 first, u32 second)
 ext 0278282C void agl::dof::VignettingMesh::destroy(VignettingMesh* mesh, u32 flags) callback
 ext 027A0998 void releaseDepthBlurArchive(agl::ResourceRecord* archive)
 ext 02752BEC void destroyDepthBlurArchiveBase(agl::ResourceRecord* archive, u32 flags)
 ext 027BE2B0 void destroyDepthBlurTextureBuffer(agl::TextureSampler* sampler, u32 flags)
 ext 02782EF8 void agl::dof::ContextHost::destroy(ContextHost* host, u32 flags)
 ext 0273AD10 void* allocateDepthBlurRuntimeStorage(u32 size)
 ext 027A84B8 agl::parameter_tree::Value* constructParameterObject(agl::parameter_tree::Value* storage)
 ext 0279E674 void constructRuntimeParameterBase(void* parameter)
 ext 0279E618 void registerRuntimeParameter(void* parameter, sead::SafeText* name, sead::SafeText* label, sead::SafeText* description, void* archive)
 ext 0279CB9C void agl::parameter_tree::Node::appendValue(Value* value, sead::SafeText* name)
 ext 02783074 agl::dof::ContextHost* agl::dof::ContextHost::construct(ContextHost* host)
 ext 0279A07C agl::ParameterRoot* agl::ParameterRoot::create(ParameterRoot* root, sead::SafeText* name, u32 context)
 ext 027BDF7C void constructRuntimeTextureBuffer(agl::TextureSampler* sampler)
 ext 027A03A0 void constructRuntimeEmbeddedArchive(agl::ResourceRecord* archive)
 ext 028EFFD0 void constructRuntimeTextureArray(void* objects, u32 count, u32 stride, MeshConstructor construct)
 ext 02782BB4 agl::dof::VignettingMesh* agl::dof::VignettingMesh::construct(VignettingMesh* mesh) callback
 ext 02785CB4 void agl::detail::RootNode::setNodeMeta(void* node, const sead::SafeText* meta)
 ext 027830E0 void agl::dof::ContextHost::initialize(s32 contextCount, u32 sharedSettings, sead::RegistryHeap* heap)
 ext 0279A6B0 void notifyDepthBlurRuntimeChange(DepthOfField* runtime, u32 reason)
 ext 028F4BE0 f64 runtimeShapeCos(f64 angle)
 ext 028F43F8 f64 runtimeShapeSin(f64 angle)
 ext 027B5CFC void initializeRuntimeVertexBuffer(agl::VertexBufferLayout* buffer, void* vertices, u32 stride, u32 bytes)
 ext 027B5D60 void setRuntimeVertexAttribute(agl::VertexBufferLayout* buffer, u32 attribute, u32 format, u32 offset, u32 stream)
 ext 027BF960 void initializeRuntimeVertexBindings(agl::VertexInputSet* bindings, u32 streams, sead::RegistryHeap* heap)
 ext 027BFB60 void bindRuntimeVertexAttribute(agl::VertexInputSet* bindings, u32 attribute, agl::VertexBufferLayout* buffer, u32 index)
 ext 027BFC74 void finalizeRuntimeVertexBindings(agl::VertexInputSet* bindings)
 ext 027BFE5C void activateRuntimeVertexBindings(agl::VertexInputSet* bindings)
 ext C0006178 void drawRuntimeShapeIndices(u32 primitive, u32 count, u32 indexType, void* indices, s32 vertexOffset, u32 instances)
 ext 0279FC6C agl::TextureCacheEntry* acquireRuntimeMipSurface(agl::DynamicTextureCache* cache, sead::SafeText* name, u32 format, u32 width, u32 height, u32 mipCount, u32 first, u32 second, u32 third, u32 fourth)
 ext 027BDEB4 void assignRuntimeMipSurface(agl::TextureSampler* destination, const agl::TextureData* source)
 ext 0274FDBC void constructRuntimeDrawState(sead::CompactRenderState* state)
 ext 0274FEB0 void applyRuntimeDrawState(sead::CompactRenderState* state)
 ext 027B954C void clearRuntimeOutput(agl::RenderBuffer* output)
 ext 025C0848 void bindRuntimeOutputSource(agl::ColorSurface* color, agl::TextureData* source, u32 first, u32 second)
 ext 027BB9E0 void prepareRuntimeProgram(agl::ShaderProgram* program, u32 flags)
 ext 027B9F68 void selectRuntimeShaderMode(u32 mode)
 ext C00060E0 void callRuntimeDisplayList(void* list, u32 size)
 ext 027BB7CC void activateRuntimeProgram(agl::ShaderProgram* program)
 ext C0006A40 void setRuntimeVertexUniform(s32 location, u32 count, const void* values)
 ext C0006908 void setRuntimePixelUniform(s32 location, u32 count, const void* values)
 ext 027BE53C void bindRuntimeSampler(agl::TextureSampler* sampler, agl::ProgramResource16_l* location, s32 slot, u32 flags)
 ext 0277FBDC void setRuntimeOutputMip(void* runtime, agl::RenderBuffer* output, u32 mip, u32 secondaryOffset)
 ext 027A9A24 u32 copyRuntimeTarget(ContextTargets* targets, u32 mode)
 ext 027B6D64 void activateRuntimeOutputSampler(agl::ColorSurface* color)
 ext 02756140 sead::RegistryHeap* getRuntimeAllocationHeap(sead::RegistryManagerState* manager)
 ext 027B54E0 void initializeRuntimeIndexBuffer(agl::IndexBufferLayout* buffer, void* indices, u32 elementSize, u32 count)
 ext 027A063C void loadDepthBlurRuntimeArchive(agl::ResourceRecord* archive, void* configuration, sead::SafeText* name, void* heap)
*/
