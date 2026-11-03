/* Compiler-generated deleting destructors, grouped by their own translation-unit roles. */
#include "cg/dtor_1.h"
static constexpr u32 ReleaseStorage = 1;

/* 025F386C */
void cg::font_manager::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F389C */
void cg::font_manager::destroyStringBaseValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 025F38B0 */
void cg::font_manager::destroyStringBufferValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 025F38C4 */
void cg::font_manager::BufferedString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F6404 */
void cg::msg_res_mgr::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F6434 */
void cg::msg_res_mgr::Buffered256Base::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F6448 */
void cg::msg_res_mgr::Buffered64Base::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F645C */
void cg::msg_res_mgr::Fixed64::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F6470 */
void cg::msg_res_mgr::Buffered128Base::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F6484 */
void cg::msg_res_mgr::Fixed128::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F6498 */
void cg::msg_res_mgr::BufferedString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F64AC */
void cg::msg_res_mgr::Fixed256::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F65E0 */
void cg::msg_res_mgr::MessageSet::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F7398 */
void cg::msg_text::destroyTextStringValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 025F73C8 */
void cg::msg_text::HdMsgString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F73FC */
void cg::msg_text::HdMsgBuffered::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 025F7410 */
void cg::msg_text::HdMsgFixed::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02601A70 */
void cg::text_unit::Safe::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02601A84 */
void cg::text_unit::WSafe::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02601A98 */
void cg::text_unit::destroyInlineTextBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02601AC8 */
void cg::text_unit::destroyNarrowStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02601AFC */
void cg::text_unit::destroyWideStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02601B10 */
void cg::text_unit::BufferedString256::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02601B24 */
void cg::text_unit::FixedString32::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02601B38 */
void cg::text_unit::BufferedString1::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02601B4C */
void cg::text_unit::FixedString1::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02603284 */
void cg::res_model::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026032B4 */
void cg::res_model::destroyStringBaseValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 026032C8 */
void cg::res_model::destroyModelNameBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 026032DC */
void cg::res_model::destroyModelNameStorage(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02603410 */
void cg::res_model::destroyPathStringValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02603424 */
void cg::res_model::destroyPathStorageValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02603438 */
void cg::res_model::destroyPathBufferValue(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 0260F83C */
void cg::res_load_task::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0260F8D8 */
void cg::res_load_task::BufferedString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0260F964 */
void cg::res_load_task::Fixed64Base::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0260F978 */
void cg::res_load_task::Fixed64::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0260F98C */
void cg::res_load_task::destroyPathStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 0260F9A0 */
void cg::res_load_task::Fixed128::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613884 */
void cg::res_mgr::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613898 */
void cg::res_mgr::destroyDirectoryStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02613988 */
void cg::res_mgr::FixedStringBase::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613BBC */
void cg::res_mgr::FixedString64Base::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613BD0 */
void cg::res_mgr::FixedString64::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613BE4 */
void cg::res_mgr::destroyDirectoryBufferBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02613BF8 */
void cg::res_mgr::DirectoryEntryString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613C0C */
void cg::res_mgr::FixedString32::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613E90 */
void cg::res_mgr::destroyPathBufferBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02613EA4 */
void cg::res_mgr::PathBuffer::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613EB8 */
void cg::res_mgr::FixedString48Base::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02613ECC */
void cg::res_mgr::FixedString48::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02616EB8 */
void cg::input_eventmgr::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0261A16C */
void cg::swkbd_mgr::String::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0261AA88 */
void cg::input_cursor::Icon::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0261BAE8 */
void cg::screen_arrow::SafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0261CF68 */
void cg::screen_auction_rupy::RupyString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0261D7B4 */
void cg::screen_auction_number::String::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02620474 */
void cg::screen_baton_input::BatonSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02620768 */
void cg::screen_baton_completion_point::BatonInputString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02620F74 */
void cg::screen_baton_sign::BatonSignString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02621748 */
void cg::screen_baton_sign_for_window::BatonWindowString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262307C */
void cg::screen_baton_sign_list::BatonListString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262376C */
void cg::screen_battery::BatteryString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02623A48 */
void cg::screen_battleship_2::BattleshipSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02624A24 */
void cg::screen_battleship_game::BattleshipGameSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 026253D0 */
void cg::screen_best_shot_mark::BestShotStringVtableBestShot::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02625F40 */
void cg::screen_blur_bg::BlurString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02626AE8 */
void cg::screen_boat_race_failed::FailedOwnerSString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02626AFC */
void cg::screen_boat_race_failed::destroyCaptionStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02626B2C */
void cg::screen_boat_race_failed::destroyLayoutStringBase(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02626B40 */
void cg::screen_boat_race_failed::destroyCaptionStringBuffer(void* object, u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!object) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(object);
}

/* 02627584 */
void cg::screen_boat_race_failed_char::Renamed::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02628290 */
void cg::screen_boat_race_result_parts::RaceResultSString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02628AB8 */
void cg::screen_boat_race_start::RaceStartSString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262937C */
void cg::screen_boot::String::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02629EC8 */
void cg::screen_boss_hp::BossString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262AC4C */
void cg::screen_btn_collect_icon::CollectString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262C518 */
void cg::screen_btn_collect_icon_mask::CollectMaskString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262CF84 */
void cg::screen_btn_control_wind::WindButtonString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262E5E4 */
void cg::screen_btn_difficulty::String::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262EC20 */
void cg::screen_btn_dungeon_item_icon::DungeonIconSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 0262F6B8 */
void cg::screen_btn_floor::FloorSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02633F10 */
void cg::screen_menu_item::ItemSafeString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* 02635CAC */
void cg::screen_touch_slide_decide::TouchStaticString::destroy(u32 flags) {
    gabi::AutoFrame automatic{gabi::current_function()};
    if (!this) return;
    if ((flags & ReleaseStorage) != 0) cgReleaseStorage(this);
}

/* WWHD glue
ext 0273AF40 void cgReleaseStorage(void*)
*/
