// agl::detail::ShaderHolder: creates the six shader-program archives from the library's resource archive
// and names the uniforms, samplers, attributes and uniform blocks of every built-in shader program.
#include "agl/ShaderHolder.h"
#include "agl/ShaderArchiveRelocations.h"
#include "sead/ArchiveRes.h"
using namespace agl;
using namespace agl::detail;

static ShaderHolderVtable sShaderHolderVtable;
static u32 sInstanceDisposerVtable;
static u32 sNameTextVtable;

static const char* sArchiveFileNames[6] = {
    "agl_common", "agl_technique", "agl_technique_pfx", "agl_technique_lght", "agl_technique_shdw", "simple_model",
};
// Uniform names 2..17 of the sky dither program (its skinning matrices).
static const char* sWeightMatrixNames[16] = {
    "cWeightMtx[0]", "cWeightMtx[1]", "cWeightMtx[2]", "cWeightMtx[3]", "cWeightMtx[4]", "cWeightMtx[5]",
    "cWeightMtx[6]", "cWeightMtx[7]", "cWeightMtx[8]", "cWeightMtx[9]", "cWeightMtx[10]", "cWeightMtx[11]",
    "cWeightMtx[12]", "cWeightMtx[13]", "cWeightMtx[14]", "cWeightMtx[15]",
};
static const ShaderHolder::ShaderEntry sShaderTable[132] = {
    { cArchive_agl_common, "dev_util" },
    { cArchive_agl_common, "depth_visualize" },
    { cArchive_agl_common, "texture" },
    { cArchive_agl_common, "reduce_2" },
    { cArchive_agl_common, "reduce_4" },
    { cArchive_agl_common, "reduce_8" },
    { cArchive_agl_common, "reduce_16" },
    { cArchive_agl_technique_pfx, "color_correction" },
    { cArchive_agl_technique_pfx, "color_correction_map" },
    { cArchive_agl_common, "xlu_snap" },
    { cArchive_agl_common, "image_filter_gaussian" },
    { cArchive_agl_common, "image_filter_cubic" },
    { cArchive_agl_common, "image_filter_blur" },
    { cArchive_agl_common, "texture_mult_color" },
    { cArchive_agl_common, "red" },
    { cArchive_agl_common, "green" },
    { cArchive_agl_common, "blue" },
    { cArchive_agl_common, "alpha" },
    { cArchive_agl_common, "depth_raw" },
    { cArchive_agl_common, "depth_linear" },
    { cArchive_agl_common, "depth_mask" },
    { cArchive_agl_common, "luminance" },
    { cArchive_agl_common, "texture_2d_array" },
    { cArchive_agl_common, "texture_3d" },
    { cArchive_agl_common, "texture_cube_map" },
    { cArchive_agl_common, "texture_cube_map_array" },
    { cArchive_agl_common, "texture_texcoord" },
    { cArchive_agl_common, "texture_multi_sample_1x" },
    { cArchive_agl_common, "texture_multi_sample_2x" },
    { cArchive_agl_common, "texture_multi_sample_4x" },
    { cArchive_agl_common, "texture_multi_sample_8x" },
    { cArchive_agl_common, "alpha_modify" },
    { cArchive_agl_common, "texture_color_drift" },
    { cArchive_agl_common, "color_quad" },
    { cArchive_agl_common, "clear_quad" },
    { cArchive_agl_common, "top_bottom_color" },
    { cArchive_agl_common, "texture_gamma" },
    { cArchive_agl_common, "draw_imm" },
    { cArchive_agl_common, "draw_fan" },
    { cArchive_agl_common, "frame_buffer_flip_y" },
    { cArchive_agl_common, "frame_buffer_no_flip" },
    { cArchive_agl_technique_pfx, "bloom_mask" },
    { cArchive_agl_technique_pfx, "bloom_gaussian" },
    { cArchive_agl_technique_pfx, "bloom_compose" },
    { cArchive_agl_technique_pfx, "hdr_compose" },
    { cArchive_agl_technique_pfx, "dof_near_mask" },
    { cArchive_agl_technique_pfx, "dof_mipmap" },
    { cArchive_agl_technique_pfx, "dof_depth_mask" },
    { cArchive_agl_technique_pfx, "dof_final" },
    { cArchive_agl_technique_pfx, "dof_vignetting" },
    { cArchive_agl_technique_pfx, "dof_expand_reduce" },
    { cArchive_agl_technique_pfx, "fxaa" },
    { cArchive_agl_technique_pfx, "fxaa_coloronly" },
    { cArchive_agl_technique_pfx, "fxaa_luma" },
    { cArchive_agl_technique_pfx, "fxaa_reprojection" },
    { cArchive_agl_technique_pfx, "filter_aa_reprojection" },
    { cArchive_agl_technique_pfx, "reduce_aa" },
    { cArchive_agl_technique_lght, "lightmap" },
    { cArchive_agl_technique_lght, "lightmap_clear" },
    { cArchive_agl_technique_lght, "local_lightmap" },
    { cArchive_agl_technique_lght, "light_pre_pass_point_light" },
    { cArchive_agl_technique_lght, "light_pre_pass_spot_light" },
    { cArchive_agl_technique_lght, "light_pre_pass_proj_light" },
    { cArchive_agl_technique_lght, "light_pre_pass_double_same_pos_with_back" },
    { cArchive_agl_technique_lght, "light_pre_pass_tbr" },
    { cArchive_agl_technique_lght, "light_pre_pass_tbr_sp" },
    { cArchive_agl_technique_lght, "light_pre_pass_dev" },
    { cArchive_agl_technique, "multi_filter_reduce" },
    { cArchive_agl_technique, "multi_filter_expand" },
    { cArchive_agl_technique_shdw, "static_depth_shadow" },
    { cArchive_agl_common, "cubemap_gaussian" },
    { cArchive_agl_common, "cubemap_color_correction" },
    { cArchive_agl_common, "cubemap_head_extract" },
    { cArchive_agl_common, "cubemap_head_convolute_first" },
    { cArchive_agl_common, "cubemap_head_convolute_other" },
    { cArchive_agl_common, "cubemap_hdr_encode" },
    { cArchive_agl_common, "cubemap_draw_illuminant" },
    { cArchive_agl_common, "nv12decode" },
    { cArchive_agl_technique, "screen_pick" },
    { cArchive_agl_common, "debug_cubemap" },
    { cArchive_agl_technique_shdw, "depth_shadow_debug" },
    { cArchive_agl_technique_shdw, "vsm" },
    { cArchive_agl_technique_shdw, "ssao_ao_buffer" },
    { cArchive_agl_technique_shdw, "ssao_blur" },
    { cArchive_agl_technique_shdw, "ssao_reduce" },
    { cArchive_agl_technique_shdw, "ssao_mask" },
    { cArchive_agl_technique, "occlusion_query" },
    { cArchive_agl_technique, "occlusion_renderer_clear_buf" },
    { cArchive_agl_technique, "occlusion_renderer" },
    { cArchive_agl_technique, "occluded_effect_lensflare" },
    { cArchive_agl_common, "texture_compress" },
    { cArchive_agl_technique_shdw, "shadow_mask" },
    { cArchive_agl_technique, "radial_blur" },
    { cArchive_agl_technique, "radial_blur_compose" },
    { cArchive_agl_technique_lght, "ssii" },
    { cArchive_agl_technique_lght, "ssii_ref" },
    { cArchive_agl_technique_lght, "ssii_reduce" },
    { cArchive_agl_technique_lght, "ssii_reduce_g" },
    { cArchive_agl_technique_lght, "ssii_pre_render" },
    { cArchive_agl_technique_lght, "ssii_expand" },
    { cArchive_agl_technique_lght, "ssii_anti_howling" },
    { cArchive_agl_technique, "normal_map" },
    { cArchive_agl_technique_pfx, "auto_exposure" },
    { cArchive_agl_technique_lght, "sssss_blur" },
    { cArchive_agl_technique_lght, "sssss_expand_sss" },
    { cArchive_agl_technique_lght, "sssss_reduce" },
    { cArchive_agl_technique_lght, "sssss_merge" },
    { cArchive_agl_technique_lght, "sssss_merge_weight" },
    { cArchive_agl_technique_pfx, "glare_filter_seed" },
    { cArchive_agl_technique_pfx, "glare_filter_blur" },
    { cArchive_agl_technique_pfx, "glare_filter_depth" },
    { cArchive_agl_technique_pfx, "glare_filter_clear" },
    { cArchive_agl_technique_pfx, "flare_filter_flare" },
    { cArchive_agl_technique_pfx, "flare_filter_copy" },
    { cArchive_agl_technique_lght, "planar_reflection" },
    { cArchive_agl_technique_shdw, "shadow_pre_pass" },
    { cArchive_agl_technique_shdw, "shadow_pre_pass_blur" },
    { cArchive_agl_technique_shdw, "shadow_pre_pass_post" },
    { cArchive_agl_technique, "decal_simple" },
    { cArchive_agl_technique, "decal_trail" },
    { cArchive_agl_technique, "decal_texture_drawer" },
    { cArchive_agl_technique, "normal_drawer_post" },
    { cArchive_agl_technique_pfx, "sky_dome" },
    { cArchive_agl_technique_pfx, "sky_screen" },
    { cArchive_agl_technique_pfx, "sky_cloud" },
    { cArchive_agl_technique_pfx, "sky_dither" },
    { cArchive_agl_common, "debug_primitive" },
    { cArchive_agl_common, "debug_shape_instanced" },
    { cArchive_agl_common, "debug_point_instanced" },
    { cArchive_agl_common, "debug_line_instanced" },
    { cArchive_agl_common, "debug_triangle_instanced" },
    { cArchive_agl_common, "cubemap_irradiance" },
};

void* allocateObject(u32 size);
void* allocateFromHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
void releaseObject(void* object);
void registerInstanceDisposer(sead::Disposer* disposer, sead::RegistryHeap* heap, u32 kind);
typedef ShaderArchive_l* (*ArchiveConstructor)(ShaderArchive_l*);
typedef void (*ArchiveDestructor)(ShaderArchive_l*, u32);
void constructArray(void* array, u32 count, u32 size, ArchiveConstructor construct);
void destroyArray(void* array, u32 count, u32 size, ArchiveDestructor destroy, u32, u32);
ShaderArchive_l* constructProgramArchive(ShaderArchive_l* archive);
void destroyProgramArchive(ShaderArchive_l* archive, u32 flags);
sead::FixedSafeText<1024>* formatArchiveFileName(
    sead::FixedSafeText<1024>* text, const char* format, const char* name, const char* extension);
struct ArchiveFileHandle_l { gptr<void> mData; };
void createProgramArchive(ShaderArchive_l* archive, const ArchiveFileHandle_l* binary, const ArchiveFileHandle_l* source,
    u32 flags, sead::RegistryHeap* heap);
sead::SafeText* constructNameText(sead::SafeText* text, const char* name);
void destroyNameText(sead::SafeText* text, u32 flags);
void allocateAttributeNames(ShaderProgram* program, s32 count, sead::RegistryHeap* heap);
void allocateUniformNames(ShaderProgram* program, s32 count, sead::RegistryHeap* heap);
void allocateSamplerNames(ShaderProgram* program, s32 count, sead::RegistryHeap* heap);
void allocateUniformBlockNames(ShaderProgram* program, s32 count, sead::RegistryHeap* heap);

// GHS emitted the temporary sead::SafeString of a `setXxxName(index, name)` call in one of four shapes,
// chosen per call: built in place (vtable and text stored by the caller) or by the translation unit's
// out-of-line constructor, each with or without the temporary's (non-deleting) destructor call after
// the setter. The shape is part of the call sequence, so every call names its own.
enum NameTextForm { cInline, cInlineDestroyed, cConstructed, cConstructedDestroyed };

static void allocateNames(ShaderProgram* program, ShaderHolder::ResourceKind kind, s32 count, sead::RegistryHeap* heap) {
    switch (kind) {
    case ShaderHolder::cAttribute: allocateAttributeNames(program, count, heap); break;
    case ShaderHolder::cUniform: allocateUniformNames(program, count, heap); break;
    case ShaderHolder::cUniformBlock: allocateUniformBlockNames(program, count, heap); break;
    case ShaderHolder::cSampler: allocateSamplerNames(program, count, heap); break;
    }
}

static void setName(ShaderProgram* program, ShaderHolder::ResourceKind kind, s32 index, const char* name,
    NameTextForm form) {
    gabi::Local<sead::SafeText> temporary;
    sead::SafeText* text = temporary.get();
    if (form == cConstructed || form == cConstructedDestroyed) {
        text = constructNameText(temporary.get(), name);
    } else {
        temporary->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sNameTextVtable);
        temporary->mText = gabi::at<be<u8>>(gabi::guest_addr(name));
    }
    switch (kind) {
    case ShaderHolder::cAttribute: program->setAttributeName(index, text); break;
    case ShaderHolder::cUniform: program->setUniformName(index, text); break;
    case ShaderHolder::cUniformBlock: program->setUniformBlockName(index, text); break;
    case ShaderHolder::cSampler: program->setSamplerName(index, text); break;
    }
    if (form == cInlineDestroyed || form == cConstructedDestroyed) destroyNameText(temporary.get(), 2);
}

/* 02786364 */
ShaderHolder* ShaderHolder::construct(ShaderHolder* holder) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (holder == NULL) {
        holder = (ShaderHolder*)allocateObject(sizeof(ShaderHolder));
        if (holder == NULL) return holder;
    }
    holder->mVtable = &sShaderHolderVtable;
    // The inline member constructors test their own address (GHS allocates when it is NULL).
    auto* programs = gabi::at<sead::PointerBuffer>(gabi::ea(holder) + offsetof(ShaderHolder, mPrograms));
    if (programs == NULL) programs = (sead::PointerBuffer*)allocateObject(0x21C);
    if (programs != NULL) {
        programs->mStorage = NULL;
        programs->mCount = 0;
        programs->mCapacity = 0;
        programs->setStorage(cShader_Num, gabi::at<void>(gabi::ea(programs) + sizeof(sead::PointerBuffer)));
    }
    auto* archives = gabi::at<ShaderArchive_l>(gabi::ea(holder) + offsetof(ShaderHolder, mArchives));
    if (archives == NULL) archives = (ShaderArchive_l*)allocateObject(sizeof(ShaderArchive_l) * cArchive_Num);
    if (archives != NULL) constructArray(archives, cArchive_Num, sizeof(ShaderArchive_l), constructProgramArchive);
    holder->mBuildDisplayLists = 1;
    return holder;
}

/* 02786418 */
ShaderHolder* ShaderHolder::createInstance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderHolder* holder = sShaderHolder;
    if (holder != NULL) return holder;
    holder = (ShaderHolder*)allocateFromHeap(sizeof(ShaderHolder), heap, 4);
    // GHS forms the embedded address before testing the allocation result.
    auto* disposer = gabi::at<sead::Disposer>(gabi::ea(holder) + offsetof(ShaderHolder, mDisposer));
    if (disposer != NULL) {
        registerInstanceDisposer(disposer, heap, 3);
        disposer->mVtable = reinterpret_cast<sead::DisposerVtable*>(&sInstanceDisposerVtable);
    }
    sShaderHolderDisposer = disposer;
    if (holder != NULL) holder = construct(holder);
    sShaderHolder = holder;
    return holder;
}

/* 027864B8 */
void ShaderHolder::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    destroyArray(mArchives, cArchive_Num, sizeof(ShaderArchive_l), destroyProgramArchive, 0, 0);
    if (flags & 1) releaseObject(this);
}

/* 02786520 */
void ShaderHolder::initialize(sead::ArchiveRes* resource, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<sead::FixedSafeText<1024>> fileName;
    gabi::Local<ArchiveFileHandle_l> binaryHandle;
    gabi::Local<ArchiveFileHandle_l> sourceHandle;
    for (s32 archive = 0; archive < cArchive_Num; ++archive) {
        const char* extension = ShaderArchiveRelocations::archiveExtension();
        sead::SafeText* name = formatArchiveFileName(fileName.get(), "%s.%s", sArchiveFileNames[archive], extension);
        void* source = resource->getFile(name, NULL);
        extension = ShaderArchiveRelocations::binaryExtension();
        name = formatArchiveFileName(fileName.get(), "%s.%s", sArchiveFileNames[archive], extension);
        void* binary = resource->getFile(name, NULL);
        if (source != NULL || binary != NULL) {
            binaryHandle->mData = binary;
            sourceHandle->mData = source;
            u32 flags = (mBuildDisplayLists ^ 1) | 2;
            createProgramArchive(&mArchives[archive], binaryHandle.get(), sourceHandle.get(), flags, heap);
        }
    }

    for (s32 shader = 0; shader < cShader_Num; ++shader) {
        ShaderArchive_l* archive = &mArchives[(u32)sShaderTable[shader].mArchive];
        if (archive->mBinaryImageAddress == 0 && archive->mResource == NULL) {
            // No archive: the slot stays empty.
            if (mPrograms.mCount < mPrograms.mCapacity) {
                programSlots()[(s32)mPrograms.mCount] = NULL;
                mPrograms.mCount = mPrograms.mCount + 1;
            }
            continue;
        }
        gabi::Local<sead::SafeText> programName;
        programName->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sNameTextVtable);
        programName->mText = reinterpret_cast<be<u8>*>(const_cast<char*>(sShaderTable[shader].mName.get()));
        u32 index = (u32)archive->findProgram(programName.get());
        ShaderProgram* program = NULL;
        if (index < archive->mProgramCount)
            program = reinterpret_cast<ShaderProgram*>(&archive->mPrograms.get()[index]);
        if (mPrograms.mCount < mPrograms.mCapacity) {
            programSlots()[(s32)mPrograms.mCount] = program;
            mPrograms.mCount = mPrograms.mCount + 1;
        }

        switch (shader) {
    case cShader_dev_util:
        allocateNames(program, cUniform, 6, heap);
        setName(program, cUniform, 1, "cWorldViewMatrix[0]", cInline);
        setName(program, cUniform, 0, "cWorldViewProjectionMatrix[0]", cInline);
        setName(program, cUniform, 2, "cDiffuseColor0", cInline);
        setName(program, cUniform, 3, "cDiffuseColor1", cInline);
        setName(program, cUniform, 4, "cSpecularColor", cInline);
        setName(program, cUniform, 5, "cViewLightDir", cInline);
        break;
    case cShader_depth_visualize:
        allocateNames(program, cUniform, 5, heap);
        setName(program, cUniform, 0, "cNear", cInline);
        setName(program, cUniform, 1, "cFar", cInline);
        setName(program, cUniform, 2, "cDepth[0]", cInline);
        setName(program, cUniform, 3, "cColor[0]", cInline);
        setName(program, cUniform, 4, "cLineNum", cInline);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cTexDepth", cInline);
        break;
    case cShader_texture:
    case cShader_reduce_2:
    case cShader_reduce_4:
    case cShader_reduce_8:
    case cShader_reduce_16:
    case cShader_image_filter_gaussian:
    case cShader_image_filter_cubic:
    case cShader_image_filter_blur:
    case cShader_texture_mult_color:
    case cShader_red:
    case cShader_green:
    case cShader_blue:
    case cShader_alpha:
    case cShader_depth_raw:
    case cShader_depth_linear:
    case cShader_depth_mask:
    case cShader_luminance:
    case cShader_texture_2d_array:
    case cShader_texture_3d:
    case cShader_texture_cube_map:
    case cShader_texture_cube_map_array:
    case cShader_texture_texcoord:
    case cShader_texture_multi_sample_1x:
    case cShader_texture_multi_sample_2x:
    case cShader_texture_multi_sample_4x:
    case cShader_texture_multi_sample_8x:
    case cShader_alpha_modify:
    case cShader_texture_color_drift:
    case cShader_color_quad:
    case cShader_clear_quad:
    case cShader_top_bottom_color:
    case cShader_texture_gamma:
    case cShader_nv12decode:
        allocateNames(program, cAttribute, 1, heap);
        setName(program, cAttribute, 0, "aPosition", cInline);
        allocateNames(program, cUniform, 24, heap);
        setName(program, cUniform, 0, "uProjViewWorld[0]", cInline);
        setName(program, cUniform, 1, "cTexSize", cInline);
        setName(program, cUniform, 2, "uMipLevel", cInline);
        setName(program, cUniform, 3, "uSlice", cInline);
        setName(program, cUniform, 4, "uTexScale", cInline);
        setName(program, cUniform, 5, "uTexRotate", cInline);
        setName(program, cUniform, 6, "uTexTrans", cInline);
        setName(program, cUniform, 7, "uMultiSampleNum", cInline);
        setName(program, cUniform, 8, "uColor", cInline);
        setName(program, cUniform, 9, "uTexFlipY", cInline);
        setName(program, cUniform, 10, "uTexFetchAdjust", cInline);
        setName(program, cUniform, 11, "cColorDriftR", cInline);
        setName(program, cUniform, 12, "cColorDriftG", cInline);
        setName(program, cUniform, 13, "cColorDriftB", cInline);
        setName(program, cUniform, 14, "uColorT", cInline);
        setName(program, cUniform, 15, "uColorB", cInline);
        setName(program, cUniform, 16, "uCubeMapFace", cInline);
        setName(program, cUniform, 17, "uBlurOffset", cInline);
        setName(program, cUniform, 18, "uDepthNear", cInline);
        setName(program, cUniform, 20, "uInvDepthRange", cInline);
        setName(program, cUniform, 19, "uOneMinusNearDivFar", cInline);
        setName(program, cUniform, 21, "cNV12DecodeParam", cInline);
        setName(program, cUniform, 22, "uGamma", cInline);
        setName(program, cUniform, 23, "uQuadTriangleZ", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "uTexture", cInline);
        setName(program, cSampler, 1, "uTexture_1", cInline);
        break;
    case cShader_color_correction:
    case cShader_color_correction_map:
        allocateNames(program, cUniform, 13, heap);
        setName(program, cUniform, 0, "c3DTexCoordOffset", cInline);
        setName(program, cUniform, 1, "cHSBG", cInline);
        setName(program, cUniform, 2, "cCurveA[0]", cInline);
        setName(program, cUniform, 3, "cCurveB[0]", cInline);
        setName(program, cUniform, 4, "cToyCam_Offset1", cInline);
        setName(program, cUniform, 5, "cToyCam_Offset2", cInline);
        setName(program, cUniform, 6, "cToyCam_Level1", cInline);
        setName(program, cUniform, 7, "cToyCam_Level2", cInline);
        setName(program, cUniform, 8, "cToyCam_Saturation1", cInline);
        setName(program, cUniform, 9, "cToyCam_Saturation2", cInline);
        setName(program, cUniform, 10, "cToyCam_Brightness", cInline);
        setName(program, cUniform, 11, "cToyCam_Contrast", cInline);
        setName(program, cUniform, 12, "cToyCam_MulColor", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cTexColor", cInline);
        setName(program, cSampler, 1, "cTexCorrectionMap", cInline);
        break;
    case cShader_xlu_snap:
        allocateNames(program, cUniform, 25, heap);
        setName(program, cUniform, 0, "uProjViewWorld[0]", cInline);
        setName(program, cUniform, 1, "cTexSize", cInline);
        setName(program, cUniform, 4, "uTexCoordScale", cInline);
        setName(program, cUniform, 2, "uMipLevel", cInline);
        setName(program, cUniform, 24, "uAlpha", cInline);
        allocateNames(program, cSampler, 6, heap);
        setName(program, cSampler, 0, "uTexture", cInline);
        setName(program, cSampler, 2, "uSnapColor", cInline);
        setName(program, cSampler, 3, "uSnapDepth", cInline);
        setName(program, cSampler, 5, "uTargetDepth", cInline);
        break;
    case cShader_draw_imm:
        allocateNames(program, cUniform, 5, heap);
        setName(program, cUniform, 0, "cProjViewWorld[0]", cConstructedDestroyed);
        setName(program, cUniform, 1, "cColor", cConstructedDestroyed);
        setName(program, cUniform, 2, "cPosition[0]", cConstructedDestroyed);
        setName(program, cUniform, 3, "cPosition[1]", cConstructedDestroyed);
        setName(program, cUniform, 4, "cPosition[2]", cConstructedDestroyed);
        break;
    case cShader_draw_fan:
        allocateNames(program, cUniform, 3, heap);
        setName(program, cUniform, 0, "cProjViewWorld[0]", cConstructedDestroyed);
        setName(program, cUniform, 1, "cArcInfo", cConstructedDestroyed);
        setName(program, cUniform, 2, "cColor", cConstructedDestroyed);
        break;
    case cShader_frame_buffer_flip_y:
    case cShader_frame_buffer_no_flip:
        allocateNames(program, cAttribute, 1, heap);
        setName(program, cAttribute, 0, "aPosition", cConstructedDestroyed);
        allocateNames(program, cUniform, 24, heap);
        setName(program, cUniform, 0, "uProjViewWorld[0]", cConstructedDestroyed);
        setName(program, cUniform, 2, "uMipLevel", cConstructedDestroyed);
        allocateNames(program, cSampler, 9, heap);
        setName(program, cSampler, 0, "uTextureColor_0", cConstructedDestroyed);
        setName(program, cSampler, 1, "uTextureColor_1", cConstructedDestroyed);
        setName(program, cSampler, 2, "uTextureColor_2", cConstructedDestroyed);
        setName(program, cSampler, 3, "uTextureColor_3", cConstructedDestroyed);
        setName(program, cSampler, 4, "uTextureColor_4", cConstructedDestroyed);
        setName(program, cSampler, 5, "uTextureColor_5", cConstructedDestroyed);
        setName(program, cSampler, 6, "uTextureColor_6", cConstructedDestroyed);
        setName(program, cSampler, 7, "uTextureColor_7", cConstructedDestroyed);
        setName(program, cSampler, 8, "uTextureDepth", cConstructedDestroyed);
        break;
    case cShader_bloom_mask:
    case cShader_bloom_gaussian:
    case cShader_bloom_compose:
        allocateNames(program, cUniform, 10, heap);
        setName(program, cUniform, 0, "cLuminanceWeight", cConstructedDestroyed);
        setName(program, cUniform, 1, "cThresholdParam", cConstructedDestroyed);
        setName(program, cUniform, 5, "cComposeColor", cConstructedDestroyed);
        setName(program, cUniform, 2, "cDepthParam0", cConstructedDestroyed);
        setName(program, cUniform, 3, "cDepthParam1", cConstructedDestroyed);
        setName(program, cUniform, 4, "cNearFarTexSize", cConstructedDestroyed);
        setName(program, cUniform, 6, "cGaussianParam", cConstructedDestroyed);
        setName(program, cUniform, 7, "cLuminanceParam", cConstructedDestroyed);
        setName(program, cUniform, 8, "cParam6", cConstructedDestroyed);
        setName(program, cUniform, 9, "cParam7", cConstructedDestroyed);
        allocateNames(program, cSampler, 3, heap);
        setName(program, cSampler, 0, "cTextureColor", cConstructed);
        setName(program, cSampler, 1, "cTextureDepth", cConstructed);
        setName(program, cSampler, 2, "cTextureLuminanceOffset", cInline);
        break;
    case cShader_hdr_compose:
        allocateNames(program, cUniform, 3, heap);
        setName(program, cUniform, 1, "cExposure", cConstructed);
        setName(program, cUniform, 2, "cColorCorrectionCoeff", cInline);
        allocateNames(program, cSampler, 4, heap);
        setName(program, cSampler, 0, "cExposureTexture", cConstructed);
        setName(program, cSampler, 1, "cHDRImage", cInlineDestroyed);
        setName(program, cSampler, 2, "cBloom", cConstructed);
        setName(program, cSampler, 3, "cColorCorrectionTable", cInline);
        break;
    case cShader_dof_near_mask:
    case cShader_dof_mipmap:
    case cShader_dof_depth_mask:
    case cShader_dof_final:
    case cShader_dof_vignetting:
    case cShader_dof_expand_reduce:
        allocateNames(program, cUniform, 13, heap);
        setName(program, cUniform, 0, "cParam0", cInline);
        setName(program, cUniform, 1, "cIndirectTexParam", cInline);
        setName(program, cUniform, 2, "cIndirectTexMtx0", cInline);
        setName(program, cUniform, 3, "cIndirectTexMtx1", cInline);
        setName(program, cUniform, 4, "cMulParam", cInline);
        setName(program, cUniform, 5, "cAddParam", cInline);
        setName(program, cUniform, 6, "cNearFarParam", cInline);
        setName(program, cUniform, 7, "cTexParam", cInline);
        setName(program, cUniform, 8, "cVignettingRadius", cInlineDestroyed);
        setName(program, cUniform, 9, "cVignettingParam", cConstructedDestroyed);
        setName(program, cUniform, 10, "cVignettingTrans", cConstructedDestroyed);
        setName(program, cUniform, 11, "cVignettingColor", cConstructedDestroyed);
        setName(program, cUniform, 12, "cFarMulColor", cConstructedDestroyed);
        allocateNames(program, cSampler, 5, heap);
        setName(program, cSampler, 0, "cTexColor", cConstructedDestroyed);
        setName(program, cSampler, 1, "cTexDepth", cConstructedDestroyed);
        setName(program, cSampler, 2, "cTexMipMap", cConstructedDestroyed);
        setName(program, cSampler, 3, "cTexMipMapDepth", cConstructedDestroyed);
        setName(program, cSampler, 4, "cTexIndirect", cConstructedDestroyed);
        break;
    case cShader_fxaa:
    case cShader_fxaa_coloronly:
    case cShader_fxaa_luma:
    case cShader_fxaa_reprojection:
    case cShader_filter_aa_reprojection:
        allocateNames(program, cUniform, 11, heap);
        setName(program, cUniform, 0, "cFrameSize", cConstructed);
        setName(program, cUniform, 1, "cFrameRCP1H", cInline);
        setName(program, cUniform, 2, "cFrameRCPOffset", cConstructed);
        setName(program, cUniform, 3, "cLumaCoeff", cConstructed);
        setName(program, cUniform, 4, "cEdgeThreshold", cInline);
        setName(program, cUniform, 5, "cQualityParam", cConstructed);
        setName(program, cUniform, 6, "cAlphaOut", cConstructed);
        setName(program, cUniform, 7, "cFinalPowerGL", cInlineDestroyed);
        setName(program, cUniform, 8, "cPrevScreenCoordTrx[0]", cInlineDestroyed);
        setName(program, cUniform, 9, "cNearFar", cInlineDestroyed);
        setName(program, cUniform, 10, "cNearFarCache", cInlineDestroyed);
        allocateNames(program, cSampler, 5, heap);
        setName(program, cSampler, 0, "cTexBase", cInline);
        setName(program, cSampler, 1, "cTexLuma", cConstructed);
        setName(program, cSampler, 2, "cTexReduce", cConstructed);
        setName(program, cSampler, 3, "cTexDepth", cInline);
        setName(program, cSampler, 4, "cTexPrev", cConstructed);
        break;
    case cShader_reduce_aa:
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "cFrameRCP", cInlineDestroyed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cTexNormal", cInline);
        setName(program, cSampler, 1, "cTexReduce", cConstructed);
        break;
    case cShader_lightmap:
    case cShader_lightmap_clear:
        allocateNames(program, cUniformBlock, 3, heap);
        setName(program, cUniformBlock, 0, "Light", cInline);
        setName(program, cUniformBlock, 1, "LightView", cConstructed);
        setName(program, cUniformBlock, 2, "LightMip", cConstructed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cLUT", cInline);
        setName(program, cSampler, 1, "cNormal", cConstructed);
        break;
    case cShader_local_lightmap:
        allocateNames(program, cUniformBlock, 2, heap);
        setName(program, cUniformBlock, 0, "BlendInfo", cInline);
        setName(program, cUniformBlock, 1, "ProbeSH", cConstructed);
        allocateNames(program, cSampler, 3, heap);
        setName(program, cSampler, 0, "cNormal", cInlineDestroyed);
        setName(program, cSampler, 1, "cLightMap0", cInlineDestroyed);
        setName(program, cSampler, 2, "cLightMap1", cInlineDestroyed);
        break;
    case cShader_light_pre_pass_point_light:
    case cShader_light_pre_pass_spot_light:
    case cShader_light_pre_pass_proj_light:
        allocateNames(program, cUniformBlock, 4, heap);
        setName(program, cUniformBlock, 0, "Context", cInline);
        setName(program, cUniformBlock, 1, "PointLightView", cConstructed);
        setName(program, cUniformBlock, 2, "SpotLightView", cConstructed);
        setName(program, cUniformBlock, 3, "ProjLightView", cInline);
        allocateNames(program, cSampler, 6, heap);
        setName(program, cSampler, 0, "cDepth", cConstructed);
        setName(program, cSampler, 1, "cNormal", cInlineDestroyed);
        setName(program, cSampler, 2, "cSpecPowTable", cInlineDestroyed);
        setName(program, cSampler, 3, "cProjTex", cInlineDestroyed);
        setName(program, cSampler, 4, "cDepthShadow", cInlineDestroyed);
        setName(program, cSampler, 5, "cAlbedo", cInlineDestroyed);
        break;
    case cShader_light_pre_pass_double_same_pos_with_back:
        allocateNames(program, cUniformBlock, 2, heap);
        setName(program, cUniformBlock, 0, "Context", cInline);
        setName(program, cUniformBlock, 1, "DoubleSamePosWithBack", cConstructed);
        allocateNames(program, cSampler, 6, heap);
        setName(program, cSampler, 0, "cDepth", cInlineDestroyed);
        setName(program, cSampler, 1, "cNormal", cInlineDestroyed);
        setName(program, cSampler, 2, "cSpecPowTable", cInlineDestroyed);
        setName(program, cSampler, 3, "cProjTex", cInlineDestroyed);
        setName(program, cSampler, 4, "cDepthShadow", cInlineDestroyed);
        setName(program, cSampler, 5, "cAlbedo", cConstructed);
        break;
    case cShader_light_pre_pass_tbr:
    case cShader_light_pre_pass_tbr_sp:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "PointLightView", cInline);
        allocateNames(program, cSampler, 3, heap);
        setName(program, cSampler, 0, "cDepth", cInlineDestroyed);
        setName(program, cSampler, 1, "cNormal", cInlineDestroyed);
        setName(program, cSampler, 2, "cSpecPowTable", cInlineDestroyed);
        break;
    case cShader_light_pre_pass_dev:
        allocateNames(program, cUniform, 3, heap);
        setName(program, cUniform, 0, "cWVP[0]", cInline);
        setName(program, cUniform, 1, "cColor", cConstructed);
        setName(program, cUniform, 2, "cSpotCone_CenterScale", cConstructed);
        break;
    case cShader_multi_filter_reduce:
    case cShader_multi_filter_expand:
        allocateNames(program, cAttribute, 1, heap);
        setName(program, cAttribute, 0, "aPosition", cInline);
        allocateNames(program, cUniform, 4, heap);
        setName(program, cUniform, 0, "cTexOffsetScale", cInlineDestroyed);
        setName(program, cUniform, 1, "cTexWidthInv", cInlineDestroyed);
        setName(program, cUniform, 2, "cTexOffsetAdjust", cConstructed);
        setName(program, cUniform, 3, "c3DTexCoordOffset", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cTexture", cConstructed);
        break;
    case cShader_static_depth_shadow:
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "uParam0", cInline);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "uTexture", cConstructed);
        break;
    case cShader_cubemap_gaussian:
        allocateNames(program, cUniform, 7, heap);
        setName(program, cUniform, 0, "cTexFlipY", cInline);
        setName(program, cUniform, 1, "cCubeMapFace", cConstructed);
        setName(program, cUniform, 2, "cCubeMapArrayIndex", cInline);
        setName(program, cUniform, 3, "cMipLevel", cConstructed);
        setName(program, cUniform, 4, "cTextureWidth", cConstructed);
        setName(program, cUniform, 5, "cAngleScale", cInline);
        setName(program, cUniform, 6, "cOffsetScale", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "sTexCube", cInlineDestroyed);
        break;
    case cShader_cubemap_color_correction:
        allocateNames(program, cUniform, 7, heap);
        setName(program, cUniform, 0, "cTexFlipY", cInline);
        setName(program, cUniform, 2, "cCubeMapFace", cConstructed);
        setName(program, cUniform, 3, "cCubeMapArrayIndex", cConstructed);
        setName(program, cUniform, 1, "cMipLevel", cInline);
        setName(program, cUniform, 4, "cTextureWidth", cConstructed);
        setName(program, cUniform, 5, "cColorPower", cConstructed);
        setName(program, cUniform, 6, "cColorScale", cInline);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "sTexCube", cConstructed);
        break;
    case cShader_cubemap_head_extract:
        allocateNames(program, cUniform, 5, heap);
        setName(program, cUniform, 0, "cTexFlipY", cInline);
        setName(program, cUniform, 2, "cCubeMapFace", cConstructed);
        setName(program, cUniform, 3, "cCubeMapArrayIndex", cConstructed);
        setName(program, cUniform, 1, "cMipLevel", cInline);
        setName(program, cUniform, 4, "cTextureWidth", cConstructed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "sTexCube", cInlineDestroyed);
        setName(program, cSampler, 1, "sTexCorrectValue", cInlineDestroyed);
        break;
    case cShader_cubemap_head_convolute_first:
        allocateNames(program, cUniform, 2, heap);
        setName(program, cUniform, 0, "cTexFlipY", cInline);
        setName(program, cUniform, 1, "cTextureWidth", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "sTexHeadArray", cInline);
        break;
    case cShader_cubemap_head_convolute_other:
        allocateNames(program, cUniform, 2, heap);
        setName(program, cUniform, 0, "cTexFlipY", cConstructed);
        setName(program, cUniform, 1, "cTextureWidth", cInlineDestroyed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "sTexHead", cInline);
        break;
    case cShader_cubemap_hdr_encode:
        allocateNames(program, cUniform, 7, heap);
        setName(program, cUniform, 0, "cTexFlipY", cConstructed);
        setName(program, cUniform, 1, "cMipLevel", cInline);
        setName(program, cUniform, 2, "cCubeMapFace", cConstructed);
        setName(program, cUniform, 3, "cCubeMapArrayIndex", cConstructed);
        setName(program, cUniform, 4, "cTextureWidth", cInlineDestroyed);
        setName(program, cUniform, 5, "cDynamicRange", cInlineDestroyed);
        setName(program, cUniform, 6, "cHDRDecodePower", cInlineDestroyed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "sTexCube", cInline);
        setName(program, cSampler, 1, "sTexCorrectValue", cConstructed);
        break;
    case cShader_cubemap_draw_illuminant:
        allocateNames(program, cUniform, 8, heap);
        setName(program, cUniform, 0, "cProjMtx", cInline);
        setName(program, cUniform, 1, "cViewMtx", cConstructed);
        setName(program, cUniform, 2, "cCenterPos", cConstructed);
        setName(program, cUniform, 3, "cAspect", cInlineDestroyed);
        setName(program, cUniform, 4, "cScale", cInlineDestroyed);
        setName(program, cUniform, 5, "cColor", cInlineDestroyed);
        setName(program, cUniform, 6, "cIntensity", cInlineDestroyed);
        setName(program, cUniform, 7, "cTexFlipY", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "sTexture", cInline);
        break;
    case cShader_screen_pick:
        allocateNames(program, cAttribute, 4, heap);
        setName(program, cAttribute, 0, "aPosition", cInlineDestroyed);
        setName(program, cAttribute, 1, "aBlendWeight", cInlineDestroyed);
        setName(program, cAttribute, 2, "aBlendIndex", cInlineDestroyed);
        setName(program, cAttribute, 3, "aTexCoord0", cInlineDestroyed);
        allocateNames(program, cUniformBlock, 4, heap);
        setName(program, cUniformBlock, 0, "View", cInline);
        setName(program, cUniformBlock, 1, "Skeleton", cConstructed);
        setName(program, cUniformBlock, 2, "Material", cConstructed);
        setName(program, cUniformBlock, 3, "Shape", cInline);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cAlbedo", cConstructed);
        break;
    case cShader_debug_cubemap:
        allocateNames(program, cUniform, 10, heap);
        setName(program, cUniform, 0, "cProjViewMtx[0]", cInline);
        setName(program, cUniform, 1, "cWorldMtx[0]", cConstructed);
        setName(program, cUniform, 2, "cCamWorld", cConstructed);
        setName(program, cUniform, 3, "cUseReflection", cInline);
        setName(program, cUniform, 4, "cUseMipLevel", cConstructed);
        setName(program, cUniform, 5, "cDynamicRange", cConstructed);
        setName(program, cUniform, 6, "cHDRDecodePower", cInline);
        setName(program, cUniform, 7, "cAlphaAdd", cConstructed);
        setName(program, cUniform, 8, "cMipLevel", cConstructed);
        setName(program, cUniform, 9, "cCubeMapArrayIndex", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "sTexCube", cConstructed);
        setName(program, cSampler, 1, "sTexCorrectValue", cInlineDestroyed);
        break;
    case cShader_depth_shadow_debug:
        allocateNames(program, cUniform, 5, heap);
        setName(program, cUniform, 0, "cPVW[0]", cInline);
        setName(program, cUniform, 1, "cNearFarParam", cConstructed);
        setName(program, cUniform, 2, "cInvTextureSize", cConstructed);
        setName(program, cUniform, 3, "cTextureIndex", cInline);
        setName(program, cUniform, 4, "cTextureIndexY", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cTexture", cInlineDestroyed);
        break;
    case cShader_vsm:
        allocateNames(program, cUniform, 2, heap);
        setName(program, cUniform, 0, "cInvTexSize", cConstructed);
        setName(program, cUniform, 1, "cSlice", cInlineDestroyed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cTexture", cInline);
        break;
    case cShader_ssao_ao_buffer:
    case cShader_ssao_blur:
    case cShader_ssao_reduce:
    case cShader_ssao_mask:
        allocateNames(program, cUniform, 8, heap);
        setName(program, cUniform, 0, "cAOParam0", cInline);
        setName(program, cUniform, 1, "cAOParam1", cConstructed);
        setName(program, cUniform, 2, "cNearFar", cConstructed);
        setName(program, cUniform, 3, "cInvTexSize", cInline);
        setName(program, cUniform, 4, "cSamplePoint[0]", cConstructed);
        setName(program, cUniform, 5, "cRotate[0]", cConstructed);
        setName(program, cUniform, 6, "cReprojectionMtx[0]", cInline);
        setName(program, cUniform, 7, "cReprojectionMtxZ", cConstructed);
        allocateNames(program, cSampler, 5, heap);
        setName(program, cSampler, 0, "cTexture", cInlineDestroyed);
        setName(program, cSampler, 1, "cTextureRotate", cInlineDestroyed);
        setName(program, cSampler, 2, "cTextureHistory", cInlineDestroyed);
        setName(program, cSampler, 3, "cTextureDepth", cInlineDestroyed);
        setName(program, cSampler, 4, "cTextureMask", cInlineDestroyed);
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "cAO_UB", cInline);
        break;
    case cShader_occlusion_query:
        allocateNames(program, cUniformBlock, 3, heap);
        setName(program, cUniformBlock, 0, "View", cConstructed);
        setName(program, cUniformBlock, 1, "AABB", cInline);
        setName(program, cUniformBlock, 2, "Sphere", cConstructed);
        break;
    case cShader_occlusion_renderer_clear_buf:
        break;
    case cShader_occlusion_renderer:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Context", cInlineDestroyed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cDepth", cInline);
        break;
    case cShader_occluded_effect_lensflare:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Element", cConstructed);
        allocateNames(program, cSampler, 3, heap);
        setName(program, cSampler, 0, "cOcclusion", cInline);
        setName(program, cSampler, 1, "cTexture", cConstructed);
        setName(program, cSampler, 2, "cTexture2", cInlineDestroyed);
        break;
    case cShader_texture_compress:
        allocateNames(program, cUniform, 4, heap);
        setName(program, cUniform, 0, "cTexSizeRCP", cInline);
        setName(program, cUniform, 1, "cReferLevel", cConstructed);
        setName(program, cUniform, 2, "cReferSlice", cConstructed);
        setName(program, cUniform, 3, "cReferFace", cInline);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cTexture", cConstructed);
        break;
    case cShader_shadow_mask:
        allocateNames(program, cUniformBlock, 3, heap);
        setName(program, cUniformBlock, 0, "Common", cInline);
        setName(program, cUniformBlock, 1, "SphereBlock", cConstructed);
        setName(program, cUniformBlock, 2, "CylinderBlock", cConstructed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cLinearDepth", cInline);
        setName(program, cSampler, 1, "cTexColor", cConstructed);
        break;
    case cShader_radial_blur:
    case cShader_radial_blur_compose:
        allocateNames(program, cUniform, 3, heap);
        setName(program, cUniform, 0, "cRadius", cInline);
        setName(program, cUniform, 1, "cBlurPosition", cConstructed);
        setName(program, cUniform, 2, "cVtxColor[0]", cInlineDestroyed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cTexture", cInline);
        break;
    case cShader_ssii:
        allocateNames(program, cUniform, 8, heap);
        setName(program, cUniform, 0, "cTanFovyHalf", cInline);
        setName(program, cUniform, 1, "cProjOffset", cConstructed);
        setName(program, cUniform, 2, "cSrcStep", cInlineDestroyed);
        setName(program, cUniform, 3, "cDstStep", cInlineDestroyed);
        setName(program, cUniform, 4, "cIntensity", cInlineDestroyed);
        setName(program, cUniform, 5, "cRange", cInlineDestroyed);
        setName(program, cUniform, 7, "cSteep", cConstructed);
        setName(program, cUniform, 6, "cNear", cInline);
        allocateNames(program, cSampler, 8, heap);
        setName(program, cSampler, 0, "cSrcLight", cConstructed);
        setName(program, cSampler, 1, "cSrcAlbedo", cInline);
        setName(program, cSampler, 2, "cSrcDepth", cConstructed);
        setName(program, cSampler, 3, "cSrcNormal", cConstructed);
        setName(program, cSampler, 4, "cSrcPos", cInline);
        setName(program, cSampler, 5, "cDstAlbedo", cConstructed);
        setName(program, cSampler, 6, "cDstDepth", cConstructed);
        setName(program, cSampler, 7, "cDstNormal", cInlineDestroyed);
        break;
    case cShader_ssii_ref:
        allocateNames(program, cUniform, 10, heap);
        setName(program, cUniform, 0, "cTanFovyHalf", cInline);
        setName(program, cUniform, 1, "cProjOffset", cConstructed);
        setName(program, cUniform, 2, "cSrcStep", cConstructed);
        setName(program, cUniform, 3, "cDstStep", cInline);
        setName(program, cUniform, 4, "cIntensity", cConstructed);
        setName(program, cUniform, 5, "cRange", cConstructed);
        setName(program, cUniform, 7, "cSteep", cInline);
        setName(program, cUniform, 6, "cNear", cConstructed);
        setName(program, cUniform, 8, "cPow", cConstructed);
        setName(program, cUniform, 9, "cInflate", cInline);
        allocateNames(program, cSampler, 8, heap);
        setName(program, cSampler, 0, "cSrcLight", cConstructed);
        setName(program, cSampler, 1, "cSrcAlbedo", cInlineDestroyed);
        setName(program, cSampler, 2, "cSrcDepth", cInlineDestroyed);
        setName(program, cSampler, 3, "cSrcNormal", cConstructed);
        setName(program, cSampler, 4, "cSrcPos", cInline);
        setName(program, cSampler, 5, "cDstAlbedo", cConstructed);
        setName(program, cSampler, 6, "cDstDepth", cConstructed);
        setName(program, cSampler, 7, "cDstNormal", cInline);
        break;
    case cShader_ssii_reduce:
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "cStep", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSrc", cInline);
        break;
    case cShader_ssii_reduce_g:
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "cStep", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSrc", cInline);
        break;
    case cShader_ssii_pre_render:
        allocateNames(program, cUniform, 4, heap);
        setName(program, cUniform, 0, "cTanFovyHalf", cInlineDestroyed);
        setName(program, cUniform, 1, "cProjOffset", cInlineDestroyed);
        setName(program, cUniform, 2, "cRange", cInlineDestroyed);
        setName(program, cUniform, 3, "cNear", cConstructed);
        allocateNames(program, cSampler, 3, heap);
        setName(program, cSampler, 0, "cLight", cInline);
        setName(program, cSampler, 1, "cAlbedo", cConstructed);
        setName(program, cSampler, 2, "cDepth", cInline);
        break;
    case cShader_ssii_expand:
        allocateNames(program, cUniform, 6, heap);
        setName(program, cUniform, 0, "cTanFovyHalf", cConstructed);
        setName(program, cUniform, 1, "cProjOffset", cInlineDestroyed);
        setName(program, cUniform, 2, "cSrcStep", cInlineDestroyed);
        setName(program, cUniform, 3, "cDstStep", cInlineDestroyed);
        setName(program, cUniform, 4, "cRange", cConstructed);
        setName(program, cUniform, 5, "cNear", cInline);
        allocateNames(program, cSampler, 7, heap);
        setName(program, cSampler, 0, "cSrcLight0", cConstructed);
        setName(program, cSampler, 1, "cSrcLight1", cInline);
        setName(program, cSampler, 2, "cSrcNormal", cConstructed);
        setName(program, cSampler, 3, "cSrcDepth", cConstructed);
        setName(program, cSampler, 4, "cDstAlbedo", cInline);
        setName(program, cSampler, 5, "cDstNormal", cConstructed);
        setName(program, cSampler, 6, "cDstDepth", cConstructed);
        break;
    case cShader_ssii_anti_howling:
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "cThreshold", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cSrc0", cConstructed);
        setName(program, cSampler, 1, "cSrc1", cInline);
        break;
    case cShader_normal_map:
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "cParam0", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cHeightTexture", cInline);
        break;
    case cShader_auto_exposure:
        allocateNames(program, cUniform, 4, heap);
        setName(program, cUniform, 0, "cParam0", cConstructed);
        setName(program, cUniform, 1, "cParam1", cInline);
        setName(program, cUniform, 2, "cParam2", cConstructed);
        setName(program, cUniform, 3, "cParam3", cConstructed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cTexture", cInline);
        setName(program, cSampler, 1, "cPrevTexture", cConstructed);
        break;
    case cShader_sssss_blur:
        allocateNames(program, cUniform, 6, heap);
        setName(program, cUniform, 0, "cTanFovyHalf", cInline);
        setName(program, cUniform, 1, "cProjOffset", cConstructed);
        setName(program, cUniform, 2, "cBias", cConstructed);
        setName(program, cUniform, 3, "cStep", cInline);
        setName(program, cUniform, 4, "cDepthCorrection", cConstructed);
        setName(program, cUniform, 5, "cAlpha", cConstructed);
        allocateNames(program, cSampler, 3, heap);
        setName(program, cSampler, 0, "cSrcLight", cInline);
        setName(program, cSampler, 1, "cSrcDepth", cConstructed);
        setName(program, cSampler, 2, "cBiasTex", cInlineDestroyed);
        break;
    case cShader_sssss_expand_sss:
        allocateNames(program, cUniform, 2, heap);
        setName(program, cUniform, 0, "cWeight", cInline);
        setName(program, cUniform, 1, "cAlpha", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSrc", cInline);
        break;
    case cShader_sssss_reduce:
        allocateNames(program, cUniform, 2, heap);
        setName(program, cUniform, 0, "cSrcStep", cConstructed);
        setName(program, cUniform, 1, "cAlpha", cInlineDestroyed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cSrc0", cInline);
        setName(program, cSampler, 1, "cSrc1", cConstructed);
        break;
    case cShader_sssss_merge:
        allocateNames(program, cUniform, 4, heap);
        setName(program, cUniform, 0, "cBaseDepth", cInline);
        setName(program, cUniform, 1, "cNear", cConstructed);
        setName(program, cUniform, 2, "cRange", cConstructed);
        setName(program, cUniform, 3, "cOneMinusNearDivFar", cInlineDestroyed);
        allocateNames(program, cSampler, 6, heap);
        setName(program, cSampler, 0, "cSss0", cInline);
        setName(program, cSampler, 1, "cSss1", cConstructed);
        setName(program, cSampler, 2, "cSss2", cConstructed);
        setName(program, cSampler, 3, "cNoSss", cInline);
        setName(program, cSampler, 4, "cDepth", cConstructed);
        setName(program, cSampler, 5, "cTrans", cConstructed);
        break;
    case cShader_sssss_merge_weight:
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cSrc", cInline);
        setName(program, cSampler, 1, "cWeight", cConstructed);
        break;
    case cShader_glare_filter_seed:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Seed", cInline);
        break;
    case cShader_glare_filter_blur:
        allocateNames(program, cUniform, 3, heap);
        setName(program, cUniform, 0, "cSrcStep", cConstructed);
        setName(program, cUniform, 1, "cAlpha", cInline);
        setName(program, cUniform, 2, "cColor", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSrc", cInlineDestroyed);
        break;
    case cShader_glare_filter_depth:
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSrc", cInline);
        break;
    case cShader_glare_filter_clear:
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "cColor", cConstructed);
        break;
    case cShader_flare_filter_flare:
        allocateNames(program, cUniform, 6, heap);
        setName(program, cUniform, 0, "cGhostDispersal", cInline);
        setName(program, cUniform, 1, "cGhostColor[0]", cConstructed);
        setName(program, cUniform, 2, "cHaloWidth", cInlineDestroyed);
        setName(program, cUniform, 3, "cHaloColor", cInlineDestroyed);
        setName(program, cUniform, 4, "cChromaDistortion", cInlineDestroyed);
        setName(program, cUniform, 5, "cColor", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSrc", cInline);
        break;
    case cShader_flare_filter_copy:
        allocateNames(program, cUniform, 3, heap);
        setName(program, cUniform, 0, "cSrcStep", cInlineDestroyed);
        setName(program, cUniform, 1, "cAlpha", cInlineDestroyed);
        setName(program, cUniform, 2, "cColor", cInlineDestroyed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSrc", cInline);
        break;
    case cShader_planar_reflection:
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cColor", cConstructed);
        allocateNames(program, cUniform, 1, heap);
        setName(program, cUniform, 0, "cParam0", cInline);
        break;
    case cShader_shadow_pre_pass:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Common", cConstructed);
        allocateNames(program, cSampler, 5, heap);
        setName(program, cSampler, 0, "cLinearDepth", cInline);
        setName(program, cSampler, 1, "cAoBuffer", cConstructed);
        setName(program, cSampler, 2, "texDynamicShadowCascade", cInline);
        setName(program, cSampler, 3, "texDynamicShadow", cConstructed);
        setName(program, cSampler, 4, "texStaticDepthShadow", cConstructed);
        break;
    case cShader_shadow_pre_pass_blur:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Common", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cSrcTexture", cConstructed);
        setName(program, cSampler, 1, "cDepthTexture", cInline);
        break;
    case cShader_shadow_pre_pass_post:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Common", cConstructed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cSrcShadowBuffer", cInline);
        setName(program, cSampler, 1, "cAoBuffer", cConstructed);
        break;
    case cShader_decal_simple:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Common", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cDepthTexture", cConstructed);
        setName(program, cSampler, 1, "cTexture", cInlineDestroyed);
        break;
    case cShader_decal_trail:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Common", cInline);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cDepthTexture", cConstructed);
        setName(program, cSampler, 1, "cTexture", cInline);
        break;
    case cShader_decal_texture_drawer:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Common", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cTexture", cInline);
        break;
    case cShader_normal_drawer_post:
        allocateNames(program, cUniformBlock, 1, heap);
        setName(program, cUniformBlock, 0, "Common", cConstructed);
        allocateNames(program, cSampler, 2, heap);
        setName(program, cSampler, 0, "cTexture", cInline);
        setName(program, cSampler, 1, "cLinearDepthTexture", cConstructed);
        break;
    case cShader_sky_dome:
        allocateNames(program, cUniformBlock, 2, heap);
        setName(program, cUniformBlock, 0, "Context", cInline);
        setName(program, cUniformBlock, 1, "EyeParam", cConstructed);
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cSunColorTable", cInline);
        break;
    case cShader_sky_screen:
        allocateNames(program, cUniformBlock, 2, heap);
        setName(program, cUniformBlock, 0, "Context", cConstructed);
        setName(program, cUniformBlock, 1, "EyeParam", cInlineDestroyed);
        allocateNames(program, cSampler, 5, heap);
        setName(program, cSampler, 0, "cColor", cInline);
        setName(program, cSampler, 1, "cDepth", cConstructed);
        setName(program, cSampler, 2, "cSunColorTable", cConstructed);
        setName(program, cSampler, 3, "cGndColorTable", cInline);
        setName(program, cSampler, 4, "cGndAttnTable", cConstructed);
        break;
    case cShader_sky_cloud:
        allocateNames(program, cSampler, 4, heap);
        setName(program, cSampler, 0, "cColor1", cInline);
        setName(program, cSampler, 1, "cColor2", cConstructed);
        setName(program, cSampler, 2, "cGndColorTable", cConstructed);
        setName(program, cSampler, 3, "cGndAttnTable", cInlineDestroyed);
        allocateNames(program, cUniformBlock, 2, heap);
        setName(program, cUniformBlock, 0, "Context", cInline);
        setName(program, cUniformBlock, 1, "Cloud", cConstructed);
        break;
    case cShader_sky_dither:
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "cTextureBase", cInline);
        allocateNames(program, cUniform, 18, heap);
        setName(program, cUniform, 0, "cRatio", cConstructed);
        setName(program, cUniform, 1, "cViewport", cInlineDestroyed);
        for (s32 i = 0; i < 16; ++i)
            setName(program, cUniform, 2 + i, sWeightMatrixNames[i], cInlineDestroyed);
        break;
    case cShader_debug_primitive:
        allocateNames(program, cUniform, 3, heap);
        setName(program, cUniform, 0, "cPVW[0]", cConstructed);
        setName(program, cUniform, 1, "cWorld[0]", cInline);
        setName(program, cUniform, 2, "cColor", cConstructed);
        break;
    case cShader_debug_shape_instanced:
        allocateNames(program, cUniformBlock, 5, heap);
        setName(program, cUniformBlock, 0, "View", cInlineDestroyed);
        setName(program, cUniformBlock, 1, "ShapeInfoId", cInlineDestroyed);
        setName(program, cUniformBlock, 2, "ShapeInfo", cConstructed);
        break;
    case cShader_debug_point_instanced:
        allocateNames(program, cUniformBlock, 5, heap);
        setName(program, cUniformBlock, 0, "View", cInline);
        setName(program, cUniformBlock, 3, "PointInfoId", cConstructed);
        setName(program, cUniformBlock, 4, "PointInfo", cInline);
        break;
    case cShader_debug_line_instanced:
        allocateNames(program, cUniformBlock, 5, heap);
        setName(program, cUniformBlock, 0, "View", cConstructed);
        setName(program, cUniformBlock, 3, "PointInfoId", cInlineDestroyed);
        setName(program, cUniformBlock, 4, "PointInfo", cInlineDestroyed);
        break;
    case cShader_debug_triangle_instanced:
        allocateNames(program, cUniformBlock, 5, heap);
        setName(program, cUniformBlock, 0, "View", cInline);
        setName(program, cUniformBlock, 3, "PointInfoId", cConstructed);
        setName(program, cUniformBlock, 4, "PointInfo", cConstructed);
        break;
    case cShader_cubemap_irradiance:
        allocateNames(program, cSampler, 1, heap);
        setName(program, cSampler, 0, "sTexCube", cInline);
        allocateNames(program, cUniform, 5, heap);
        setName(program, cUniform, 0, "cTexFlipY", cInlineDestroyed);
        setName(program, cUniform, 1, "cCubeMapFace", cInlineDestroyed);
        setName(program, cUniform, 2, "cAngleParam", cInlineDestroyed);
        setName(program, cUniform, 3, "cMipLevel", cInlineDestroyed);
        break;
        }
    }

    for (s32 archive = 0; archive < cArchive_Num; ++archive) {
        ShaderArchive_l* programArchive = &mArchives[archive];
        if (programArchive->mBinaryImageAddress != 0 || programArchive->mResource != NULL) programArchive->update();
    }
}

/* WWHD glue
data sShaderHolderVtable 10165640 nocheck
data sInstanceDisposerVtable 10165630 nocheck
data sNameTextVtable 10163784 nocheck
data sArchiveFileNames 101F8D14
data sWeightMatrixNames 101F8D2C
data sShaderTable 1016479C
ext 0273AD10 void* allocateObject(u32)
ext 0273B0D4 void* allocateFromHeap(u32, sead::RegistryHeap*, s32)
ext 0273AF40 void releaseObject(void*)
ext 02752B0C void registerInstanceDisposer(sead::Disposer*, sead::RegistryHeap*, u32)
ext 0273B560 void sead::PointerBuffer::setStorage(s32, void*)
ext 028EFFD0 void constructArray(void*, u32, u32, ArchiveConstructor)
ext 028F0164 void destroyArray(void*, u32, u32, ArchiveDestructor, u32, u32)
ext 027B744C ShaderArchive_l* constructProgramArchive(ShaderArchive_l*) callback
ext 027B7860 void destroyProgramArchive(ShaderArchive_l*, u32) callback
ext 027BD2B4 const char* agl::ShaderArchiveRelocations::archiveExtension()
ext 027BD2C0 const char* agl::ShaderArchiveRelocations::binaryExtension()
ext 0278BA74 sead::FixedSafeText<1024>* formatArchiveFileName(sead::FixedSafeText<1024>*, const char*, const char*, const char*)
ext 027B8904 void createProgramArchive(ShaderArchive_l*, const ArchiveFileHandle_l*, const ArchiveFileHandle_l*, u32, sead::RegistryHeap*)
ext 027B90AC s32 agl::ShaderArchive_l::findProgram(const sead::SafeText*)
ext 027B9070 u32 agl::ShaderArchive_l::update()
ext 0278B9CC sead::SafeText* constructNameText(sead::SafeText*, const char*)
ext 0278B9B8 void destroyNameText(sead::SafeText*, u32)
ext 027BACC0 void allocateAttributeNames(ShaderProgram*, s32, sead::RegistryHeap*)
ext 027BAEF8 void allocateUniformNames(ShaderProgram*, s32, sead::RegistryHeap*)
ext 027BB160 void allocateSamplerNames(ShaderProgram*, s32, sead::RegistryHeap*)
ext 027BB3C8 void allocateUniformBlockNames(ShaderProgram*, s32, sead::RegistryHeap*)
ext 027BAE8C void agl::ShaderProgram::setAttributeName(u32, const sead::SafeText*)
ext 027BB0EC void agl::ShaderProgram::setUniformName(u32, const sead::SafeText*)
ext 027BB354 void agl::ShaderProgram::setSamplerName(u32, const sead::SafeText*)
ext 027BB5BC void agl::ShaderProgram::setUniformBlockName(u32, const sead::SafeText*)
str 1016381C
str 10163824
str 1016382C
str 10163834
str 1016383C
str 10163844
str 1016384C
str 10163854
str 1016385C
str 10163864
str 1016386C
str 10163874
str 1016387C
str 10163884
str 1016388C
str 10163894
str 1016389C
str 101638A4
str 101638AC
str 101638B4
str 101638BC
str 101638C4
str 101638CC
str 101638D4
str 101638DC
str 101638E4
str 101638EC
str 101638F4
str 101638FC
str 10163904
str 1016390C
str 10163914
str 1016391C
str 10163924
str 1016392C
str 10163934
str 1016393C
str 10163944
str 1016394C
str 10163954
str 1016395C
str 10163964
str 1016396C
str 10163974
str 1016397C
str 10163984
str 1016398C
str 10163994
str 1016399C
str 101639A4
str 101639AC
str 101639B4
str 101639BC
str 101639C4
str 101639CC
str 101639D4
str 101639DC
str 101639E4
str 101639EC
str 101639F4
str 101639FC
str 10163A04
str 10163A14
str 10163A24
str 10163A34
str 10163A44
str 10163A54
str 10163A64
str 10163A74
str 10163A84
str 10163A94
str 10163AA4
str 10163AB4
str 10163AC4
str 10163AD4
str 10163AE4
str 10163AF4
str 10163B04
str 10163B1C
str 10163B2C
str 10163B3C
str 10163B4C
str 10163B5C
str 10163B6C
str 10163B7C
str 10163B8C
str 10163B9C
str 10163BB4
str 10163BC8
str 10163BD4
str 10163BE8
str 10163BFC
str 10163C10
str 10163C1C
str 10163C28
str 10163C34
str 10163C40
str 10163C4C
str 10163C58
str 10163C6C
str 10163C78
str 10163C84
str 10163C90
str 10163C9C
str 10163CA8
str 10163CB4
str 10163CC0
str 10163CD4
str 10163CE0
str 10163CF4
str 10163D00
str 10163D20
str 10163D30
str 10163D3C
str 10163D48
str 10163D54
str 10163D60
str 10163D74
str 10163D80
str 10163D8C
str 10163D98
str 10163DA4
str 10163DB8
str 10163DC4
str 10163DD8
str 10163DEC
str 10163DF8
str 10163E04
str 10163E14
str 10163E20
str 10163E34
str 10163E48
str 10163E58
str 10163E68
str 10163E78
str 10163E88
str 10163E94
str 10163EAC
str 10163EB8
str 10163ED0
str 10163EE0
str 10163EEC
str 10163EFC
str 10163F0C
str 10163F18
str 10163F24
str 10163F30
str 10163F40
str 10163F50
str 10163F60
str 10163F78
str 10163F90
str 10163F9C
str 10163FA8
str 10163FB8
str 10163FC8
str 10163FD8
str 10163FE4
str 10163FF0
str 10164000
str 1016400C
str 10164018
str 1016402C
str 10164038
str 10164048
str 10164058
str 10164064
str 10164070
str 1016407C
str 10164088
str 10164094
str 101640A0
str 101640B4
str 101640C0
str 101640D0
str 101640E0
str 101640EC
str 101640FC
str 1016410C
str 1016411C
str 10164128
str 10164134
str 10164140
str 10164150
str 10164160
str 10164170
str 10164180
str 1016418C
str 1016419C
str 101641B0
str 101641C0
str 101641CC
str 101641D8
str 101641E4
str 101641F0
str 10164200
str 10164210
str 10164224
str 10164238
str 1016424C
str 1016425C
str 10164268
str 10164274
str 10164284
str 10164298
str 101642AC
str 101642C0
str 101642D4
str 101642E8
str 101642F8
str 10164304
str 10164314
str 10164324
str 10164334
str 10164344
str 10164350
str 10164364
str 10164374
str 10164384
str 10164394
str 101643A8
str 101643B4
str 101643C0
str 101643D0
str 101643E8
str 101643F4
str 10164400
str 1016440C
str 10164418
str 10164424
str 10164430
str 1016443C
str 10164448
str 10164454
str 10164464
str 10164470
str 10164480
str 10164490
str 101644A4
str 101644B0
str 101644C0
str 101644D4
str 101644E4
str 101644F0
str 10164504
str 10164510
str 1016451C
str 10164528
str 10164534
str 10164540
str 1016454C
str 1016455C
str 10164568
str 10164574
str 10164580
str 10164590
str 101645A0
str 101645B0
str 101645C0
str 101645CC
str 101645DC
str 101645EC
str 101645F8
str 10164604
str 10164614
str 10164624
str 10164634
str 10164640
str 1016464C
str 10164658
str 10164664
str 10164670
str 1016467C
str 10164688
str 10164694
str 101646A0
str 101646AC
str 101646BC
str 101646CC
str 101646E0
str 101646EC
str 101646F8
str 10164708
str 10164714
str 10164720
str 10164734
str 1016474C
str 10164760
str 1016476C
str 1016477C
str 1016478C
*/
