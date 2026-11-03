/* Compiler-generated deleting destructors, grouped by their own translation-unit roles. */
#include "cg/dtor_4.h"
static constexpr u32 ReleaseStorage = 1;

/* 026FB200 */
void cg::parts_layout::destroyLayoutStringValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 026FB230 */
void cg::parts_layout::destroyLayoutStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 026FB244 */
void cg::parts_layout::destroyLayoutStringBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 026FB258 */
void cg::parts_layout::LayoutFixedString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026FB3DC */
void cg::layout_res_mgr::Manager::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026FC038 */
void cg::layout_res_mgr::ResourceString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026FC068 */
void cg::layout_res_mgr::NameBase::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026FC07C */
void cg::layout_res_mgr::NameIntermediate::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026FC090 */
void cg::layout_res_mgr::NameFinal::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026FC0A4 */
void cg::layout_res_mgr::destroyArchiveStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 026FC0B8 */
void cg::layout_res_mgr::Path::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026FC4C0 */
void cg::layout_loop::LoopString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02700804 */
void cg::layout_wait_screen::DummyInput::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02700834 */
void cg::layout_wait_screen::destroyLayoutStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02700848 */
void cg::layout_wait_screen::DummyBase::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0270085C */
void cg::layout_wait_screen::DummyFinal::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02700C84 */
void cg::parts_no_control::NoControlName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02702428 */
void cg::no_decide_button::ButtonName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02702890 */
void cg::parts_one_time::OneTimeName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02702F80 */
void cg::parts_progress_bar::ProgressBarName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02704B98 */
void cg::copy_text_pane::CopySafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02704BC8 */
void cg::copy_text_pane::CopyString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02704BFC */
void cg::copy_text_pane::CopyBufferedString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02704C10 */
void cg::copy_text_pane::CopyFixedString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02705138 */
void cg::parts_text_pane::destroyTextStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02705168 */
void cg::parts_text_pane::destroyTextStringBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027060A8 */
void cg::layout_locale::LayoutLocaleString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02707988 */
void cg::parts_valid::String::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0270840C */
void cg::scene_ending_end::EndingString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02708B94 */
void cg::scene_boot::BootString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0270C4FC */
void cg::scene_save_data::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0270C5DC */
void cg::scene_save_data::SaveDataHolder::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0270D030 */
void cg::scene_opening::OpeningSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02719530 */
void cg::scene_ingame_ui::GroupString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0271B198 */
void cg::scene_main_screens::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0271EBF4 */
void cg::scene_staff_roll::StaffSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0271F574 */
void cg::scene_title::TitleSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0271F858 */
void cg::save_data::Inventory::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 027206CC */
void cg::save_data::Name::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02725560 */
void cg::save_data::PictureManager::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02725690 */
void cg::picture_manager::destroyStringView(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027256A4 */
void cg::picture_manager::destroyBufferBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027256D4 */
void cg::picture_manager::destroyNameBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027256E8 */
void cg::picture_manager::destroyNameBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027269A8 */
void cg::player_name::destroyWideNameView(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027269D8 */
void cg::player_name::PlayerNameSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02726A0C */
void cg::player_name::NarrowName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02726A20 */
void cg::player_name::PlayerName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02726AC0 */
void cg::save_data::PictureInfo::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02727554 */
void cg::picture_accessor::AccessorSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02727584 */
void cg::render_misc::MiscFixedName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 027275B8 */
void cg::render_misc::MiscBufferedName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 027275CC */
void cg::render_misc::MiscGroupName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02727860 */
void cg::render_resources::RenderResources::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02727F64 */
void cg::render_misc::MiscSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 027283D4 */
void cg::color_fader::ColorListenerInterface::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02729040 */
void cg::fader_mgr::ManagerListenerInterface::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0272B04C */
void cg::graphics_task::GraphicsName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0272D668 */
void cg::render_layers::ShaderLocation::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0272E4AC */
void cg::layer_2d::UIName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0272EB70 */
void cg::layer_2d_drc::ShdLayer2CName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0272F360 */
void cg::layer_2d_drc_fore::DRCForeName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0272F84C */
void cg::layer_2d_fore::ShdLayer2DForeCName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0273379C */
void cg::layer_3d::Layer3DSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02733E80 */
void cg::layer_mask::LayerMaskSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02734598 */
void cg::layer_base::destroyLayerStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027345AC */
void cg::layer_base::LayerBaseFixedString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 027345DC */
void cg::layer_base::LayerBaseString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 027345F0 */
void cg::layer_base::LayerBaseName::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 027348B0 */
void cg::layer_drc_mode::LayerDrcModeSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02734D6C */
void cg::error_viewer::ErrorViewerSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02735248 */
void cg::fader::String::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02735C84 */
void cg::multi_filter::MultiFilterSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02735CF0 */
void cg::multi_filter::MultiFilterTextureImage::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02738010 */
void cg::layer_shadow::LayerShadowSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02738404 */
void cg::render_commands::CommandManagerBody::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02738964 */
void cg::resource_tool_paths::ResourceToolPathsSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02738994 */
void cg::resource_tool_paths::destroyPathStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027389A8 */
void cg::resource_tool_paths::destroyConverterBufferBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027389BC */
void cg::resource_tool_paths::destroyConverterBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027389D0 */
void cg::resource_tool_paths::destroyFilterBufferBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 027389E4 */
void cg::resource_tool_paths::destroyFilterBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02739380 */
void cg::shader_archive::ShaderArchiveSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* WWHD glue
ext 0273AF40 void cgReleaseStorage(void*)
*/
