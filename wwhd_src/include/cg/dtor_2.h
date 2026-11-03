#pragma once
#include "wwhd.h"
// Opaque receiver declarations only; these destructors inspect no fields or complete layouts.
namespace cg {
namespace screen_btn_item_icon {
struct IconString { void destroy(u32 flags); };
}
namespace btn_map_icon_extra {
struct MapIconStaticString { void destroy(u32 flags); };
}
namespace btn_map_icon_number {
struct MapCountString { void destroy(u32 flags); };
}
namespace btn_map_pattern {
struct MapPatternStaticString { void destroy(u32 flags); };
}
namespace screen_btn_menu {
struct BtnMenuSafeString { void destroy(u32 flags); };
}
namespace screen_btn_menu_arrow {
struct ArrowSafeString { void destroy(u32 flags); };
}
namespace screen_btn_menu_heart {
struct HeartSafeString { void destroy(u32 flags); };
}
namespace screen_btn_miiverse_comment {
struct CommentString { void destroy(u32 flags); };
struct WideString { void destroy(u32 flags); };
}
namespace screen_btn_miiverse_new_comment {
struct NewCommentString { void destroy(u32 flags); };
}
namespace screen_btn_miiverse_picture {
struct PictureButtonString { void destroy(u32 flags); };
}
namespace screen_btn_pearl {
struct PearlString { void destroy(u32 flags); };
}
namespace screen_btn_pictograph {
struct PictographString { void destroy(u32 flags); };
}
namespace screen_pictograph_post {
struct PostStringVtablePost { void destroy(u32 flags); };
}
namespace screen_btn_triforce {
struct TriforceString { void destroy(u32 flags); };
}
namespace screen_character {
struct CharacterSafeString { void destroy(u32 flags); };
}
namespace screen_cannonball {
struct CannonballSafeString { void destroy(u32 flags); };
}
namespace screen_cannon_game_2 {
struct CannonGameSafeString { void destroy(u32 flags); };
}
namespace screen_collection {
struct CollectionString { void destroy(u32 flags); };
}
namespace screen_command_a_extra {
struct CommandASafeString { void destroy(u32 flags); };
}
namespace screen_command_guide_extra {
struct GuideMessageString { void destroy(u32 flags); };
}
namespace d_operate_wind {
struct SafeString { void destroy(u32 flags); };
}
namespace screen_control_wind_ok_states {
struct WindOKString { void destroy(u32 flags); };
}
namespace screen_crawling_direction {
struct CrawlString { void destroy(u32 flags); };
}
namespace screen_crawling_direction_arrow {
struct ArrowString { void destroy(u32 flags); };
}
namespace screen_cursor {
struct CursorSafeString { void destroy(u32 flags); };
}
namespace screen_msg_cursor {
struct MsgCursorSafeString { void destroy(u32 flags); };
}
namespace screen_fixed_cursor {
struct FixedCursorSafeString { void destroy(u32 flags); };
}
namespace cursor_mgr {
struct CursorMgr { void destroy(u32 flags); };
}
namespace screen_dungeon_key_states {
struct DungeonKeyString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_part1 {
struct CaptionName { void destroy(u32 flags); };
}
namespace screen_dungeon_map_boss_factories {
struct BossString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_door {
struct DoorMarkerString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_down {
struct DownString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_floor_main {
struct FloorMainFloorString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_floor_first {
struct FirstFloorString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_floor_second {
struct SecondFloorString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_floor_third {
struct ThirdFloorString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_floor_fourth {
struct FourthFloorString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_floor_fifth {
struct FifthFloorString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_link {
struct LinkMarkerString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_makore {
struct MakoreMarkerString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_medley {
struct MedleyMarkerString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_ship {
struct ShipMarkerString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_statue {
struct StatueString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_treasure {
struct TreasureString { void destroy(u32 flags); };
}
namespace screen_dungeon_map_up {
struct UpString { void destroy(u32 flags); };
}
namespace screen_e3_select_course {
struct String { void destroy(u32 flags); };
}
namespace screen_e3_thank_you {
struct String { void destroy(u32 flags); };
}
namespace screen_ending_end {
struct String { void destroy(u32 flags); };
}
namespace screen_enemy_hp {
struct EnemyString { void destroy(u32 flags); };
}
namespace screen_enemy_tower {
struct EnemyTowerSafeString { void destroy(u32 flags); };
}
namespace screen_fade_wipe {
struct FadeString { void destroy(u32 flags); };
}
namespace screen_fade_wipe_iris {
struct IrisString { void destroy(u32 flags); };
}
namespace screen_fairy {
struct FairySafeString { void destroy(u32 flags); };
}
namespace screen_game_over {
struct String { void destroy(u32 flags); };
void destroyCaptionStringBase(void* object, u32 flags);
void destroyLayoutStringBase(void* object, u32 flags);
void destroyCaptionStringBuffer(void* object, u32 flags);
}
namespace screen_game_over_char {
struct String { void destroy(u32 flags); };
}
namespace screen_ghost_ship {
struct GhostShipSafeString { void destroy(u32 flags); };
}
namespace screen_heart_part {
struct HeartPartString { void destroy(u32 flags); };
}
namespace screen_heart_states {
struct HeartAllString { void destroy(u32 flags); };
}
namespace screen_heart_island {
struct HeartIslandSafeString { void destroy(u32 flags); };
}
namespace screen_heart_sea {
struct HeartSeaSafeString { void destroy(u32 flags); };
}
namespace screen_icon_slide_area {
struct ControllerLiteral { void destroy(u32 flags); };
}
namespace screen_icon_slide_area_mirror {
struct MirrorLiteral { void destroy(u32 flags); };
}
namespace screen_information {
struct InformationString { void destroy(u32 flags); };
}
namespace screen_information_drc {
struct InformationString { void destroy(u32 flags); };
}
namespace keyboard_text_area {
struct TextAreaSafeString { void destroy(u32 flags); };
}
namespace keyboard_cursor {
struct CursorSafeString { void destroy(u32 flags); };
}
namespace keyboard_key_icon {
struct KeyIconString { void destroy(u32 flags); };
}
namespace screen_loading_icon {
struct LoadingIconString { void destroy(u32 flags); };
}
namespace screen_loop_boss_icon_states {
struct LoopBossString { void destroy(u32 flags); };
}
namespace screen_magic_bar {
struct MagicString { void destroy(u32 flags); };
}
namespace screen_hud_main {
struct HudMainString { void destroy(u32 flags); };
}
namespace screen_sea_chart_menu {
struct SeaMenuString { void destroy(u32 flags); };
void destroyMenuStringBase(void* object, u32 flags);
void destroyMenuStringBuffer(void* object, u32 flags);
struct SeaMenuMarkerString { void destroy(u32 flags); };
}
namespace screen_sea_chart_map {
struct SeaMapLocalString { void destroy(u32 flags); };
void destroyMapStringBase(void* object, u32 flags);
void destroyMapStringBuffer(void* object, u32 flags);
}
}
static void cgReleaseStorage(void*);
