// agl::detail::ShaderHolder: the library's shader-program registry. It owns the six program archives
// (agl_common, agl_technique, ...) and one program pointer per built-in shader, and names the uniforms,
// samplers, attributes and uniform blocks those programs use. Layout from its constructor (0x42C bytes).
#pragma once
#include "sead/FileDevice.h"
#include "sead/Foundation.h"
#include "sead/HeapRegistry.h"
#include "agl/ShaderArchive.h"
#include "agl/ShaderProgram.h"

namespace agl { namespace detail {

// The holder's method table: only its deleting destructor slot is called from outside (AGL#6's disposer).
struct ShaderHolderVtable {
    sead::VtableSlot mHeader[2];
    sead::VtableSlot mDestroy;
};

struct ShaderHolder {
    // The program archives, in the order of their file names at sArchiveFileNames.
    enum ArchiveType {
        cArchive_agl_common,
        cArchive_agl_technique,
        cArchive_agl_technique_pfx,
        cArchive_agl_technique_lght,
        cArchive_agl_technique_shdw,
        cArchive_simple_model,
        cArchive_Num
    };
    // The built-in shaders, in the order of the shader table (one program pointer each).
    enum ShaderType {
        cShader_dev_util,
        cShader_depth_visualize,
        cShader_texture,
        cShader_reduce_2,
        cShader_reduce_4,
        cShader_reduce_8,
        cShader_reduce_16,
        cShader_color_correction,
        cShader_color_correction_map,
        cShader_xlu_snap,
        cShader_image_filter_gaussian,
        cShader_image_filter_cubic,
        cShader_image_filter_blur,
        cShader_texture_mult_color,
        cShader_red,
        cShader_green,
        cShader_blue,
        cShader_alpha,
        cShader_depth_raw,
        cShader_depth_linear,
        cShader_depth_mask,
        cShader_luminance,
        cShader_texture_2d_array,
        cShader_texture_3d,
        cShader_texture_cube_map,
        cShader_texture_cube_map_array,
        cShader_texture_texcoord,
        cShader_texture_multi_sample_1x,
        cShader_texture_multi_sample_2x,
        cShader_texture_multi_sample_4x,
        cShader_texture_multi_sample_8x,
        cShader_alpha_modify,
        cShader_texture_color_drift,
        cShader_color_quad,
        cShader_clear_quad,
        cShader_top_bottom_color,
        cShader_texture_gamma,
        cShader_draw_imm,
        cShader_draw_fan,
        cShader_frame_buffer_flip_y,
        cShader_frame_buffer_no_flip,
        cShader_bloom_mask,
        cShader_bloom_gaussian,
        cShader_bloom_compose,
        cShader_hdr_compose,
        cShader_dof_near_mask,
        cShader_dof_mipmap,
        cShader_dof_depth_mask,
        cShader_dof_final,
        cShader_dof_vignetting,
        cShader_dof_expand_reduce,
        cShader_fxaa,
        cShader_fxaa_coloronly,
        cShader_fxaa_luma,
        cShader_fxaa_reprojection,
        cShader_filter_aa_reprojection,
        cShader_reduce_aa,
        cShader_lightmap,
        cShader_lightmap_clear,
        cShader_local_lightmap,
        cShader_light_pre_pass_point_light,
        cShader_light_pre_pass_spot_light,
        cShader_light_pre_pass_proj_light,
        cShader_light_pre_pass_double_same_pos_with_back,
        cShader_light_pre_pass_tbr,
        cShader_light_pre_pass_tbr_sp,
        cShader_light_pre_pass_dev,
        cShader_multi_filter_reduce,
        cShader_multi_filter_expand,
        cShader_static_depth_shadow,
        cShader_cubemap_gaussian,
        cShader_cubemap_color_correction,
        cShader_cubemap_head_extract,
        cShader_cubemap_head_convolute_first,
        cShader_cubemap_head_convolute_other,
        cShader_cubemap_hdr_encode,
        cShader_cubemap_draw_illuminant,
        cShader_nv12decode,
        cShader_screen_pick,
        cShader_debug_cubemap,
        cShader_depth_shadow_debug,
        cShader_vsm,
        cShader_ssao_ao_buffer,
        cShader_ssao_blur,
        cShader_ssao_reduce,
        cShader_ssao_mask,
        cShader_occlusion_query,
        cShader_occlusion_renderer_clear_buf,
        cShader_occlusion_renderer,
        cShader_occluded_effect_lensflare,
        cShader_texture_compress,
        cShader_shadow_mask,
        cShader_radial_blur,
        cShader_radial_blur_compose,
        cShader_ssii,
        cShader_ssii_ref,
        cShader_ssii_reduce,
        cShader_ssii_reduce_g,
        cShader_ssii_pre_render,
        cShader_ssii_expand,
        cShader_ssii_anti_howling,
        cShader_normal_map,
        cShader_auto_exposure,
        cShader_sssss_blur,
        cShader_sssss_expand_sss,
        cShader_sssss_reduce,
        cShader_sssss_merge,
        cShader_sssss_merge_weight,
        cShader_glare_filter_seed,
        cShader_glare_filter_blur,
        cShader_glare_filter_depth,
        cShader_glare_filter_clear,
        cShader_flare_filter_flare,
        cShader_flare_filter_copy,
        cShader_planar_reflection,
        cShader_shadow_pre_pass,
        cShader_shadow_pre_pass_blur,
        cShader_shadow_pre_pass_post,
        cShader_decal_simple,
        cShader_decal_trail,
        cShader_decal_texture_drawer,
        cShader_normal_drawer_post,
        cShader_sky_dome,
        cShader_sky_screen,
        cShader_sky_cloud,
        cShader_sky_dither,
        cShader_debug_primitive,
        cShader_debug_shape_instanced,
        cShader_debug_point_instanced,
        cShader_debug_line_instanced,
        cShader_debug_triangle_instanced,
        cShader_cubemap_irradiance,
        cShader_Num
    };
    // The four name lists of a shader program.
    enum ResourceKind { cAttribute, cUniform, cUniformBlock, cSampler };
    struct ShaderEntry {
        be<u32> mArchive;
        gptr<const char> mName;
    };

    gptr<ShaderHolderVtable> mVtable;
    sead::Disposer mDisposer;
    sead::PointerBuffer mPrograms;
    gptr<ShaderProgram> mProgramStorage[cShader_Num];
    ShaderArchive_l mArchives[cArchive_Num];
    be<u8> mBuildDisplayLists;
    u8 mPadding[3];

    gptr<ShaderProgram>* programSlots() { return reinterpret_cast<gptr<ShaderProgram>*>(mPrograms.mStorage.get()); }
    // sead::PtrArray::at (inlined by the users): NULL past the end, unsigned compare. Built from the address so a
    // NULL program stays a guest address 0 (the callers read through it as the original does).
    ShaderProgram* program(u32 index) const {
        auto* slots = reinterpret_cast<const gptr<ShaderProgram>*>(mPrograms.mStorage.get());
        return gabi::at<ShaderProgram>(index < (u32)(s32)mPrograms.mCount ? slots[index].v.get() : 0);
    }
    static ShaderHolder* construct(ShaderHolder* holder);
    static ShaderHolder* createInstance(sead::RegistryHeap* heap);
    void destroy(u32 flags);
    void initialize(sead::ArchiveRes* resource, sead::RegistryHeap* heap);
};

} }

// The singleton and its registered disposer: data owned by agl/TimingStatics.cpp (one definition);
// ShaderHolder::createInstance stores them, AGL#6's disposer (TimingHistory.cpp) clears them.
extern gptr<agl::detail::ShaderHolder>& sShaderHolder;
extern gptr<sead::Disposer>& sShaderHolderDisposer;

WWHD_OFFSET(agl::detail::ShaderHolderVtable, mDestroy, 0x10);
WWHD_SIZE(agl::detail::ShaderHolder::ShaderEntry, 8);
WWHD_OFFSET(agl::detail::ShaderHolder, mDisposer, 4);
WWHD_OFFSET(agl::detail::ShaderHolder, mPrograms, 0x14);
WWHD_OFFSET(agl::detail::ShaderHolder, mProgramStorage, 0x20);
WWHD_OFFSET(agl::detail::ShaderHolder, mArchives, 0x230);
WWHD_OFFSET(agl::detail::ShaderHolder, mBuildDisplayLists, 0x428);
WWHD_SIZE(agl::detail::ShaderHolder, 0x42C);
