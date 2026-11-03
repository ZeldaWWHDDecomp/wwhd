#pragma once
#include "wwhd.h"
// Opaque receiver declarations only; these destructors inspect no fields or complete layouts.
namespace cg {
namespace font_manager {
struct SafeString { void destroy(u32 flags); };
void destroyStringBaseValue(void* object, u32 flags);
void destroyStringBufferValue(void* object, u32 flags);
struct BufferedString { void destroy(u32 flags); };
}
namespace msg_res_mgr {
struct SafeString { void destroy(u32 flags); };
struct Buffered256Base { void destroy(u32 flags); };
struct Buffered64Base { void destroy(u32 flags); };
struct Fixed64 { void destroy(u32 flags); };
struct Buffered128Base { void destroy(u32 flags); };
struct Fixed128 { void destroy(u32 flags); };
struct BufferedString { void destroy(u32 flags); };
struct Fixed256 { void destroy(u32 flags); };
struct MessageSet { void destroy(u32 flags); };
}
namespace msg_text {
void destroyTextStringValue(void* object, u32 flags);
struct HdMsgString { void destroy(u32 flags); };
struct HdMsgBuffered { void destroy(u32 flags); };
struct HdMsgFixed { void destroy(u32 flags); };
}
namespace text_unit {
struct Safe { void destroy(u32 flags); };
struct WSafe { void destroy(u32 flags); };
void destroyInlineTextBuffer(void* object, u32 flags);
void destroyNarrowStringBase(void* object, u32 flags);
void destroyWideStringBase(void* object, u32 flags);
struct BufferedString256 { void destroy(u32 flags); };
struct FixedString32 { void destroy(u32 flags); };
struct BufferedString1 { void destroy(u32 flags); };
struct FixedString1 { void destroy(u32 flags); };
}
namespace res_model {
struct SafeString { void destroy(u32 flags); };
void destroyStringBaseValue(void* object, u32 flags);
void destroyModelNameBuffer(void* object, u32 flags);
void destroyModelNameStorage(void* object, u32 flags);
void destroyPathStringValue(void* object, u32 flags);
void destroyPathStorageValue(void* object, u32 flags);
void destroyPathBufferValue(void* object, u32 flags);
}
namespace res_load_task {
struct SafeString { void destroy(u32 flags); };
struct BufferedString { void destroy(u32 flags); };
struct Fixed64Base { void destroy(u32 flags); };
struct Fixed64 { void destroy(u32 flags); };
void destroyPathStringBase(void* object, u32 flags);
struct Fixed128 { void destroy(u32 flags); };
}
namespace res_mgr {
struct SafeString { void destroy(u32 flags); };
void destroyDirectoryStringBase(void* object, u32 flags);
struct FixedStringBase { void destroy(u32 flags); };
struct FixedString64Base { void destroy(u32 flags); };
struct FixedString64 { void destroy(u32 flags); };
void destroyDirectoryBufferBase(void* object, u32 flags);
struct DirectoryEntryString { void destroy(u32 flags); };
struct FixedString32 { void destroy(u32 flags); };
void destroyPathBufferBase(void* object, u32 flags);
struct PathBuffer { void destroy(u32 flags); };
struct FixedString48Base { void destroy(u32 flags); };
struct FixedString48 { void destroy(u32 flags); };
}
namespace input_eventmgr {
struct SafeString { void destroy(u32 flags); };
}
namespace swkbd_mgr {
struct String { void destroy(u32 flags); };
}
namespace input_cursor {
struct Icon { void destroy(u32 flags); };
}
namespace screen_arrow {
struct SafeString { void destroy(u32 flags); };
}
namespace screen_auction_rupy {
struct RupyString { void destroy(u32 flags); };
}
namespace screen_auction_number {
struct String { void destroy(u32 flags); };
}
namespace screen_baton_input {
struct BatonSafeString { void destroy(u32 flags); };
}
namespace screen_baton_completion_point {
struct BatonInputString { void destroy(u32 flags); };
}
namespace screen_baton_sign {
struct BatonSignString { void destroy(u32 flags); };
}
namespace screen_baton_sign_for_window {
struct BatonWindowString { void destroy(u32 flags); };
}
namespace screen_baton_sign_list {
struct BatonListString { void destroy(u32 flags); };
}
namespace screen_battery {
struct BatteryString { void destroy(u32 flags); };
}
namespace screen_battleship_2 {
struct BattleshipSafeString { void destroy(u32 flags); };
}
namespace screen_battleship_game {
struct BattleshipGameSafeString { void destroy(u32 flags); };
}
namespace screen_best_shot_mark {
struct BestShotStringVtableBestShot { void destroy(u32 flags); };
}
namespace screen_blur_bg {
struct BlurString { void destroy(u32 flags); };
}
namespace screen_boat_race_failed {
struct FailedOwnerSString { void destroy(u32 flags); };
void destroyCaptionStringBase(void* object, u32 flags);
void destroyLayoutStringBase(void* object, u32 flags);
void destroyCaptionStringBuffer(void* object, u32 flags);
}
namespace screen_boat_race_failed_char {
struct Renamed { void destroy(u32 flags); };
}
namespace screen_boat_race_result_parts {
struct RaceResultSString { void destroy(u32 flags); };
}
namespace screen_boat_race_start {
struct RaceStartSString { void destroy(u32 flags); };
}
namespace screen_boot {
struct String { void destroy(u32 flags); };
}
namespace screen_boss_hp {
struct BossString { void destroy(u32 flags); };
}
namespace screen_btn_collect_icon {
struct CollectString { void destroy(u32 flags); };
}
namespace screen_btn_collect_icon_mask {
struct CollectMaskString { void destroy(u32 flags); };
}
namespace screen_btn_control_wind {
struct WindButtonString { void destroy(u32 flags); };
}
namespace screen_btn_difficulty {
struct String { void destroy(u32 flags); };
}
namespace screen_btn_dungeon_item_icon {
struct DungeonIconSafeString { void destroy(u32 flags); };
}
namespace screen_btn_floor {
struct FloorSafeString { void destroy(u32 flags); };
}
namespace screen_menu_item {
struct ItemSafeString { void destroy(u32 flags); };
}
namespace screen_touch_slide_decide {
struct TouchStaticString { void destroy(u32 flags); };
}
}
static void cgReleaseStorage(void*);
