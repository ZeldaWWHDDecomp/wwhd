// agl depth of field (the draw half of the translation unit): per-context render buffers, the compose,
// vignetting and reduced passes, the multi-context parameter host and the TU's template instances.
// Layouts come from WWHD's own constructors, destructors and field accesses. The setup half of this TU
// (AGL#4) owns the parameter object (depth_blur_value::Parameters), the draw argument (DepthBlurContext)
// and the per-context target set (ContextTargets); this header owns the host, the depth-of-field object,
// its vignetting meshes and shape parameter objects, which both halves use.
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/RenderState.h"
#include "sead/Viewport.h"
#include "sead/RuntimeFormatSupport.h"
#include "agl/DepthBlurContext.h"
#include "agl/ParameterTree.h"
#include "agl/ParameterRoot.h"
#include "agl/VertexBufferLayout.h"
#include "agl/IndexBufferLayout.h"
#include "agl/VertexInput.h"
#include "agl/TextureResource.h"
#include "agl/CascadeFormattedLabel.h"

namespace agl { namespace dof {
using depth_blur_value::Parameters;
using depth_blur_value::VignettingShape;

// The texture-matrix helpers of the indirect texture (sead::Matrix34f rows).
void makeRotation(sead::RenderMatrix34* matrix, const sead::RenderVector3* rotation);
void scaleColumns(sead::RenderMatrix34* destination, const sead::RenderMatrix34* source, const sead::RenderVector3* scale);
void translateLocal(sead::RenderMatrix34* destination, const sead::RenderMatrix34* source, const sead::RenderVector3* translation);

// The vignetting shape parameter object (AGL#4 builds it; it reports edits back to its owner).
struct DepthOfField;
struct VignettingShapeParameter : parameter_tree::Value {
    VignettingShape mShape;
    gptr<DepthOfField> mOwner;
    be<u32> mIndex;
    u32 postRead();
};

// One vertex of a vignetting shape mesh: position, then the ring radius and ring index.
struct VignettingVertex { be<f32> x, y, radius, ring; };
// A vignetting shape's mesh: vertex and index storage plus the vertex buffer, vertex input and index buffer.
struct VignettingMesh {
    be<u32> mVertexCapacity;
    gptr<VignettingVertex> mVertices;
    be<u32> mIndexCapacity;
    gptr<be<u16>> mIndices;
    VertexBufferLayout mVertexBuffer;
    VertexInputSet mVertexBindings;
    IndexBufferLayout mIndexBuffer;
    static VignettingMesh* construct(VignettingMesh* mesh);
    static void destroy(VignettingMesh* mesh, u32 flags);
};

// The multi-context, multi-view parameter host the depth of field derives from (sizeof 0x334).
struct ContextHost {
    be<u32> mViewMask;
    be<s32> mContextCount;          // requested by the base initialisation
    be<s32> mViewCount;
    be<u32> mReserved;
    be<u32> mViewIndexMask;
    be<u32> mViewNameCount;
    gptr<parameter_tree::Value> mViewNames;
    gptr<const void> mMethods;
    Parameters mSettings;           // the first view's parameters
    be<u32> mContextArrayCount;
    gptr<ContextTargets> mContexts;
    be<u32> mViewSettingsCount;
    gptr<gptr<Parameters>> mViewSettings;
    be<u32> mExtraSettingsCount;
    gptr<Parameters> mExtraSettings;
    ContextTargets* context(u32 index) const {
        ContextTargets* contexts = mContexts;
        if (index < mContextArrayCount) contexts += index;
        return contexts;
    }
    Parameters* viewSettings(u32 index) const {
        gptr<Parameters>* slot = mViewSettings;
        u32 view = index & mViewIndexMask;
        if (view < mViewSettingsCount) slot += view;
        return *slot;
    }
    Parameters* firstViewSettings() const { return mViewSettings.get()[0]; }
    static ContextHost* construct(ContextHost* host);
    static void destroy(ContextHost* host, u32 flags);
    void initialize(s32 contextCount, u32 sharedSettings, sead::RegistryHeap* heap);
    u32 isApplicable() const;
    void postApply();
};

// The depth-of-field object (0xEE0 bytes; constructed by AGL#4's 0277EA5C).
struct DepthOfField : ContextHost {
    enum ComposeMode { cCompose_Final, cCompose_DepthMask, cCompose_Combined };
    ParameterRoot mParameterRoot;   // the object's parameter root; its tree table is this class's method table
    gptr<void> mTaskMethods;
    gptr<TextureData> mInputTexture;
    TextureSampler mIndirectSampler;
    be<s32> mComposeMode;
    be<f32> mTextureScaleX, mTextureScaleY;
    ResourceRecord mArchive;
    VignettingMesh mMeshes[2];
    be<u8> mEnableNear, mOtherEnable, mEnableFar;
    u8 mPadding;
    parameter_value::BooleanParameter mEnableParameter;
    VignettingShapeParameter mNearShape, mFarShape;

    u32 draw(s32 contextIndex, RenderBuffer* renderBuffer, u32 mode, f32 nearZ, f32 farZ);
    u32 draw(s32 contextIndex, RenderBuffer* renderBuffer, void* depth, u32 viewDepth, u32 mode, f32 nearZ, f32 farZ);
    void freeBuffer(s32 contextIndex);
    u32 drawCompose(const DepthBlurContext* arg, u32 mode);
    u32 drawVignetting(const DepthBlurContext* arg, u32 mode);
    void setVignettingUniforms(const DepthBlurContext* arg, const ShaderProgram* program);
    void setReduceUniforms(const DepthBlurContext* arg, const ShaderProgram* program);
    u32 applyVignettingShape(u32 index, const VignettingShapeParameter* shape);
    void refreshViewSettings(u32 contextIndex);
};

// ---- the TU's template instances: SafeString family and agl::utl::Parameter<T> accessors ----
struct Text : sead::SafeText { static void destroy(Text* text, u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { static void destroy(TextBuffer* text, u32 flags); void assureTerminationImpl(); };
struct FixedText32 : TextBuffer { be<u8> mStorage[32]; static void destroy(FixedText32* text, u32 flags); };
struct FixedTextB : TextBuffer { static void destroy(FixedTextB* text, u32 flags); };
struct FormatText32 : FixedText32 {
    static void destroy(FormatText32* text, u32 flags);
    static FormatText32* create(FormatText32* text, const char* format, u32 first, u32 second, u32 third,
        u32 fourth, u32 fifth, u32 sixth);
};

// This unit's copies of the agl::utl::Parameter<T> accessors, on AGL#4's payload types.
struct BoolParameter : parameter_value::BooleanParameter {
    u32 type() const; be<u8>* ptr(); const be<u8>* constPtr() const; u32 size() const; void postApply();
};
struct IntParameter : parameter_value::IntegerParameter { u32 type() const; be<s32>* ptr(); const be<s32>* constPtr() const; u32 size() const; };
struct F32Parameter : parameter_value::FloatParameter { u32 type() const; be<f32>* ptr(); const be<f32>* constPtr() const; u32 size() const; };
struct Vec2Parameter : depth_blur_value::Vector2Parameter {
    u32 type() const; be<f32>* ptr(); const be<f32>* constPtr() const; u32 size() const;
};
struct Vec4Parameter : parameter_value::ColorParameter { u32 type() const; be<f32>* ptr(); const be<f32>* constPtr() const; u32 size() const; };
struct ColorParameter : parameter_value::ColorParameter { u32 type() const; be<f32>* ptr(); const be<f32>* constPtr() const; u32 size() const; };

// This unit's copies of the parameter object (parameter_tree::Value) and parameter list
// (parameter_tree::Node) hooks: default answers and the name-hash checks against a resource record.
struct ParameterObjectHooks : parameter_tree::Value {
    u32 isApplicable() const;
    u32 isApplicableSecond() const;
    void preWrite();
    u32 matchesName(const gptr<parameter_tree::MatchRecord>* record) const;
};
struct ParameterListHooks : parameter_tree::Node {
    u32 isApplicableThird() const;
    u32 matchesSecondName(const gptr<parameter_tree::MatchRecord>* record) const;
    void postWrite();
    void preRead();
    void postReadSecond();
};
// The parameter archive base of the host (an agl::ResourceRecord); this unit's deleting destructor.
struct ParameterOwner : ResourceRecord {
    static void destroy(ParameterOwner* owner, u32 flags);
};

}}

WWHD_SIZE(agl::dof::VignettingVertex, 0x10);
WWHD_OFFSET(agl::dof::VignettingShapeParameter, mShape, 0x5C);
WWHD_OFFSET(agl::dof::VignettingShapeParameter, mOwner, 0xA8);
WWHD_OFFSET(agl::dof::VignettingShapeParameter, mIndex, 0xAC);
WWHD_SIZE(agl::dof::VignettingShapeParameter, 0xB0);
WWHD_OFFSET(agl::dof::VignettingMesh, mIndices, 0xC);
WWHD_OFFSET(agl::dof::VignettingMesh, mVertexBuffer, 0x10);
WWHD_OFFSET(agl::dof::VignettingMesh, mVertexBindings, 0x164);
WWHD_OFFSET(agl::dof::VignettingMesh, mIndexBuffer, 0x258);
WWHD_SIZE(agl::dof::VignettingMesh, 0x270);
WWHD_OFFSET(agl::dof::ContextHost, mViewIndexMask, 0x10);
WWHD_OFFSET(agl::dof::ContextHost, mViewNames, 0x18);
WWHD_OFFSET(agl::dof::ContextHost, mMethods, 0x1C);
WWHD_OFFSET(agl::dof::ContextHost, mSettings, 0x20);
WWHD_OFFSET(agl::dof::ContextHost, mContextArrayCount, 0x31C);
WWHD_OFFSET(agl::dof::ContextHost, mViewSettings, 0x328);
WWHD_OFFSET(agl::dof::ContextHost, mExtraSettings, 0x330);
WWHD_SIZE(agl::dof::ContextHost, 0x334);
WWHD_OFFSET(agl::dof::DepthOfField, mParameterRoot, 0x334);
WWHD_OFFSET(agl::dof::DepthOfField, mTaskMethods, 0x4FC);
WWHD_OFFSET(agl::dof::DepthOfField, mInputTexture, 0x500);
WWHD_OFFSET(agl::dof::DepthOfField, mIndirectSampler, 0x504);
WWHD_OFFSET(agl::dof::DepthOfField, mComposeMode, 0x69C);
WWHD_OFFSET(agl::dof::DepthOfField, mArchive, 0x6A8);
WWHD_OFFSET(agl::dof::DepthOfField, mMeshes, 0x88C);
WWHD_OFFSET(agl::dof::DepthOfField, mEnableParameter, 0xD70);
WWHD_OFFSET(agl::dof::DepthOfField, mNearShape, 0xD80);
WWHD_OFFSET(agl::dof::DepthOfField, mFarShape, 0xE30);
WWHD_SIZE(agl::dof::DepthOfField, 0xEE0);
WWHD_OFFSET(agl::dof::FixedText32, mStorage, 0xC);
WWHD_SIZE(agl::dof::FixedText32, 0x2C);
WWHD_OFFSET(agl::dof::ParameterObjectHooks, mNameHash, 0x50);
WWHD_OFFSET(agl::dof::ParameterListHooks, mNameHash, 0x54);
WWHD_OFFSET(agl::dof::ParameterOwner, mInterfaceMethods, 0x10);
