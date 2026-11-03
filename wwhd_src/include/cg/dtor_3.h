#pragma once
#include "wwhd.h"
// Opaque receiver declarations only; these destructors inspect no fields or complete layouts.
namespace cg {
namespace screen_sea_chart_map {
struct SeaMapFixedString { void destroy(u32 flags); };
}
namespace sea_chart_grid {
struct ChartGrid { void destroy(u32 flags); };
}
namespace sea_chart_link_pos {
struct LinkSafeString { void destroy(u32 flags); };
}
namespace screen_map_display_coordinator {
struct MapSafeString { void destroy(u32 flags); };
}
namespace screen_menu_btns {
struct MenuString { void destroy(u32 flags); };
}
namespace screen_menu_main {
struct MenuMainResourceName { void destroy(u32 flags); };
}
namespace screen_menu_map_states {
struct MenuMapString { void destroy(u32 flags); };
}
namespace screen_menu_miiverse {
struct MiiverseString { void destroy(u32 flags); };
}
namespace screen_menu_miiverse_comment {
struct CommentSoundString { void destroy(u32 flags); };
}
namespace screen_menu_miiverse_new_comment {
struct SequenceSafeString { void destroy(u32 flags); };
}
namespace screen_menu_miiverse_setting {
struct SettingString { void destroy(u32 flags); };
}
namespace screen_moon {
struct MoonSafeString { void destroy(u32 flags); };
}
namespace msg_choice_control {
struct TalkControlString { void destroy(u32 flags); };
struct TalkMessageName { void destroy(u32 flags); };
}
namespace msg_choice {
void destroyCaptionStringBase(void* object, u32 flags);
void destroyLayoutStringBase(void* object, u32 flags);
}
namespace msg_choice_window {
struct ChoiceDummyString { void destroy(u32 flags); };
struct ChoiceDummyOwned { void destroy(u32 flags); };
}
namespace msg_only_choice_panes {
struct MsgOnlyChoicePaneString { void destroy(u32 flags); };
}
namespace msg_only {
void destroyMessageStringBase(void* object, u32 flags);
void destroyMessageStringBuffer(void* object, u32 flags);
}
namespace msg_only_window {
struct MsgOnlyName { void destroy(u32 flags); };
}
namespace msg_main_window {
struct MsgMainSourceString { void destroy(u32 flags); };
void destroyMessageStringBase(void* object, u32 flags);
struct MsgMainDummyString { void destroy(u32 flags); };
struct MsgMainDummyOwned { void destroy(u32 flags); };
}
namespace screen_msg_window_event_frame {
struct FrameString { void destroy(u32 flags); };
}
namespace screen_msg_window_main2 {
struct Main2SafeString { void destroy(u32 flags); };
}
namespace screen_msg_only2 {
struct Only2SafeString { void destroy(u32 flags); };
void destroyMessageStringBase(void* object, u32 flags);
struct Only2Label { void destroy(u32 flags); };
struct Only2LabelVtableDone { void destroy(u32 flags); };
}
namespace msg_window_scroll_extra {
struct ScrollString { void destroy(u32 flags); };
}
namespace screen_no_operate_drc {
struct NoOperateString { void destroy(u32 flags); };
}
namespace screen_opening {
struct OpeningString { void destroy(u32 flags); };
}
namespace screen_pause {
struct PauseString { void destroy(u32 flags); };
}
namespace screen_pictograph {
struct BackdropString { void destroy(u32 flags); };
}
namespace screen_pictograph_camera {
struct CameraPaneString { void destroy(u32 flags); };
}
namespace screen_pictograph_background {
struct BackgroundString { void destroy(u32 flags); };
}
namespace screen_pictograph_list {
struct ListNameString { void destroy(u32 flags); };
}
namespace d_menu_window {
struct SafeString { void destroy(u32 flags); };
}
namespace screen_rupy {
struct RupyString { void destroy(u32 flags); };
}
namespace screen_rupy_sword_counter_2 {
struct RupySwordSafeString { void destroy(u32 flags); };
}
namespace title_scroll_key_icon {
struct ScrollKeyString { void destroy(u32 flags); };
}
namespace screen_secret_entrance_chart {
struct SecretEntranceString { void destroy(u32 flags); };
}
namespace title_controller_screen {
struct ControllerSoundString { void destroy(u32 flags); };
}
namespace screen_sequence_option {
struct OptionString { void destroy(u32 flags); };
}
namespace screen_sequence_window_sequence {
struct SequenceString { void destroy(u32 flags); };
}
namespace screen_item_shortcut {
struct ItemShortcutSafeString { void destroy(u32 flags); };
}
namespace screen_slide_assist_tv {
struct SlideString { void destroy(u32 flags); };
}
namespace screen_squid_chart {
struct SquidString { void destroy(u32 flags); };
}
namespace screen_squid_icon {
struct SquidIconSafeString { void destroy(u32 flags); };
}
namespace screen_staff_roll {
struct StaffRollSafeString { void destroy(u32 flags); };
}
namespace screen_staff_roll_bg {
struct StaffRollBGSafeString { void destroy(u32 flags); };
}
namespace screen_staff_roll_bubble {
struct BubbleSafeString { void destroy(u32 flags); };
}
namespace screen_staff_roll_bubble_base {
struct BubbleBaseSafeString { void destroy(u32 flags); };
}
namespace screen_staff_roll_bubble_s {
struct BubbleSSafeString { void destroy(u32 flags); };
}
namespace title_start_background {
struct BackgroundString { void destroy(u32 flags); };
}
namespace screen_submarine_chart {
struct SubmarineString { void destroy(u32 flags); };
}
namespace screen_swim_time_hud {
struct SwimString { void destroy(u32 flags); };
}
namespace screen_telescope_cover {
struct CoverString { void destroy(u32 flags); };
}
namespace screen_terry_chart {
struct TerryString { void destroy(u32 flags); };
}
namespace screen_time {
struct TimeString { void destroy(u32 flags); };
}
namespace screen_tingle_tower_chart {
struct TincleTowerString { void destroy(u32 flags); };
}
namespace title_logo {
struct LogoPaneNameString { void destroy(u32 flags); };
}
namespace screen_tre_map {
struct String { void destroy(u32 flags); };
}
namespace screen_triforce_chart {
struct TriforceString { void destroy(u32 flags); };
}
namespace screen_unread_miiverse {
struct UnreadString { void destroy(u32 flags); };
}
namespace screen_warp_area {
struct SafeString { void destroy(u32 flags); };
}
namespace screen_warp_map {
struct MapString { void destroy(u32 flags); };
}
namespace parts_basic_button {
struct BasicString { void destroy(u32 flags); };
}
namespace parts_window {
struct WindowNodeString { void destroy(u32 flags); };
struct WindowBuffer { void destroy(u32 flags); };
struct WindowString { void destroy(u32 flags); };
struct WindowConfigName { void destroy(u32 flags); };
}
namespace parts_button {
struct ButtonString { void destroy(u32 flags); };
void destroyButtonStringBase(void* object, u32 flags);
void destroyCaptionStringBase(void* object, u32 flags);
void destroyCaptionStringBuffer(void* object, u32 flags);
}
namespace parts_heart {
struct String { void destroy(u32 flags); };
}
namespace parts_in_out_loop {
struct InOutLoopSafeString { void destroy(u32 flags); };
}
namespace parts_key_icon {
struct KeyIconSafeString { void destroy(u32 flags); };
}
namespace parts {
struct SafeString { void destroy(u32 flags); };
}
}
static void cgReleaseStorage(void*);
