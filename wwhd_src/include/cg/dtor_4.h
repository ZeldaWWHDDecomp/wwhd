#pragma once
#include "wwhd.h"
// Opaque receiver declarations only; these destructors inspect no fields or complete layouts.
namespace cg {
namespace parts_layout {
void destroyLayoutStringValue(void* object, u32 flags);
void destroyLayoutStringBase(void* object, u32 flags);
void destroyLayoutStringBuffer(void* object, u32 flags);
struct LayoutFixedString { void destroy(u32 flags); };
}
namespace layout_res_mgr {
struct Manager { void destroy(u32 flags); };
struct ResourceString { void destroy(u32 flags); };
struct NameBase { void destroy(u32 flags); };
struct NameIntermediate { void destroy(u32 flags); };
struct NameFinal { void destroy(u32 flags); };
void destroyArchiveStringBase(void* object, u32 flags);
struct Path { void destroy(u32 flags); };
}
namespace layout_loop {
struct LoopString { void destroy(u32 flags); };
}
namespace layout_wait_screen {
struct DummyInput { void destroy(u32 flags); };
void destroyLayoutStringBase(void* object, u32 flags);
struct DummyBase { void destroy(u32 flags); };
struct DummyFinal { void destroy(u32 flags); };
}
namespace parts_no_control {
struct NoControlName { void destroy(u32 flags); };
}
namespace no_decide_button {
struct ButtonName { void destroy(u32 flags); };
}
namespace parts_one_time {
struct OneTimeName { void destroy(u32 flags); };
}
namespace parts_progress_bar {
struct ProgressBarName { void destroy(u32 flags); };
}
namespace copy_text_pane {
struct CopySafeString { void destroy(u32 flags); };
struct CopyString { void destroy(u32 flags); };
struct CopyBufferedString { void destroy(u32 flags); };
struct CopyFixedString { void destroy(u32 flags); };
}
namespace parts_text_pane {
void destroyTextStringBase(void* object, u32 flags);
void destroyTextStringBuffer(void* object, u32 flags);
}
namespace layout_locale {
struct LayoutLocaleString { void destroy(u32 flags); };
}
namespace parts_valid {
struct String { void destroy(u32 flags); };
}
namespace scene_ending_end {
struct EndingString { void destroy(u32 flags); };
}
namespace scene_boot {
struct BootString { void destroy(u32 flags); };
}
namespace scene_save_data {
struct SafeString { void destroy(u32 flags); };
struct SaveDataHolder { void destroy(u32 flags); };
}
namespace scene_opening {
struct OpeningSafeString { void destroy(u32 flags); };
}
namespace scene_ingame_ui {
struct GroupString { void destroy(u32 flags); };
}
namespace scene_main_screens {
struct SafeString { void destroy(u32 flags); };
}
namespace scene_staff_roll {
struct StaffSafeString { void destroy(u32 flags); };
}
namespace scene_title {
struct TitleSafeString { void destroy(u32 flags); };
}
namespace save_data {
struct Inventory { void destroy(u32 flags); };
struct Name { void destroy(u32 flags); };
struct PictureManager { void destroy(u32 flags); };
struct PictureInfo { void destroy(u32 flags); };
}
namespace picture_manager {
void destroyStringView(void* object, u32 flags);
void destroyBufferBase(void* object, u32 flags);
void destroyNameBase(void* object, u32 flags);
void destroyNameBuffer(void* object, u32 flags);
}
namespace player_name {
void destroyWideNameView(void* object, u32 flags);
struct PlayerNameSafeString { void destroy(u32 flags); };
struct NarrowName { void destroy(u32 flags); };
struct PlayerName { void destroy(u32 flags); };
}
namespace picture_accessor {
struct AccessorSafeString { void destroy(u32 flags); };
}
namespace render_misc {
struct MiscFixedName { void destroy(u32 flags); };
struct MiscBufferedName { void destroy(u32 flags); };
struct MiscGroupName { void destroy(u32 flags); };
struct MiscSafeString { void destroy(u32 flags); };
}
namespace render_resources {
struct RenderResources { void destroy(u32 flags); };
}
namespace color_fader {
struct ColorListenerInterface { void destroy(u32 flags); };
}
namespace fader_mgr {
struct ManagerListenerInterface { void destroy(u32 flags); };
}
namespace graphics_task {
struct GraphicsName { void destroy(u32 flags); };
}
namespace render_layers {
struct ShaderLocation { void destroy(u32 flags); };
}
namespace layer_2d {
struct UIName { void destroy(u32 flags); };
}
namespace layer_2d_drc {
struct ShdLayer2CName { void destroy(u32 flags); };
}
namespace layer_2d_drc_fore {
struct DRCForeName { void destroy(u32 flags); };
}
namespace layer_2d_fore {
struct ShdLayer2DForeCName { void destroy(u32 flags); };
}
namespace layer_3d {
struct Layer3DSafeString { void destroy(u32 flags); };
}
namespace layer_mask {
struct LayerMaskSafeString { void destroy(u32 flags); };
}
namespace layer_base {
void destroyLayerStringBase(void* object, u32 flags);
struct LayerBaseFixedString { void destroy(u32 flags); };
struct LayerBaseString { void destroy(u32 flags); };
struct LayerBaseName { void destroy(u32 flags); };
}
namespace layer_drc_mode {
struct LayerDrcModeSafeString { void destroy(u32 flags); };
}
namespace error_viewer {
struct ErrorViewerSafeString { void destroy(u32 flags); };
}
namespace fader {
struct String { void destroy(u32 flags); };
}
namespace multi_filter {
struct MultiFilterSafeString { void destroy(u32 flags); };
struct MultiFilterTextureImage { void destroy(u32 flags); };
}
namespace layer_shadow {
struct LayerShadowSafeString { void destroy(u32 flags); };
}
namespace render_commands {
struct CommandManagerBody { void destroy(u32 flags); };
}
namespace resource_tool_paths {
struct ResourceToolPathsSafeString { void destroy(u32 flags); };
void destroyPathStringBase(void* object, u32 flags);
void destroyConverterBufferBase(void* object, u32 flags);
void destroyConverterBuffer(void* object, u32 flags);
void destroyFilterBufferBase(void* object, u32 flags);
void destroyFilterBuffer(void* object, u32 flags);
}
namespace shader_archive {
struct ShaderArchiveSafeString { void destroy(u32 flags); };
}
}
static void cgReleaseStorage(void*);
