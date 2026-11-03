#pragma once
#include "wwhd.h"

// Opaque immutable string receivers; no object layout or capacity is assumed.
namespace cgempty {
namespace collection_screen {
struct SafeString { void assureTermination(); };
}
namespace action_command {
struct SafeString { void assureTermination(); };
}
namespace command_guide {
struct SafeString { void assureTermination(); };
}
namespace wind_operation {
struct SafeString { void assureTermination(); };
}
namespace wind_confirmation {
struct SafeString { void assureTermination(); };
}
namespace crawling_direction {
struct SafeString { void assureTermination(); };
}
namespace crawling_arrow {
struct SafeString { void assureTermination(); };
}
namespace cursor_screen {
struct SafeString { void assureTermination(); };
}
namespace message_cursor {
struct SafeString { void assureTermination(); };
}
namespace fixed_cursor {
struct SafeString { void assureTermination(); };
}
namespace dungeon_key {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_caption {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_boss {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_door {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_down {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_floor {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_first_floor {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_second_floor {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_third_floor {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_fourth_floor {
struct SafeString { void assureTermination(); };
}

namespace dungeon_map_fifth_floor {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_link {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_makore {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_medley {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_ship {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_statue {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_treasure {
struct SafeString { void assureTermination(); };
}
namespace dungeon_map_up {
struct SafeString { void assureTermination(); };
}
namespace course_selection {
struct SafeString { void assureTermination(); };
}
namespace course_thanks {
struct SafeString { void assureTermination(); };
}
namespace ending_screen {
struct SafeString { void assureTermination(); };
}
namespace enemy_health {
struct SafeString { void assureTermination(); };
}
namespace enemy_tower {
struct SafeString { void assureTermination(); };
}
namespace fade_wipe {
struct SafeString { void assureTermination(); };
}
namespace iris_wipe {
struct SafeString { void assureTermination(); };
}
namespace fairy_screen {
struct SafeString { void assureTermination(); };
}
namespace game_over_screen {
struct SafeString { void assureTermination(); };
}
namespace game_over_character {
struct SafeString { void assureTermination(); };
}
namespace ghost_ship {
struct SafeString { void assureTermination(); };
}
namespace heart_part {
struct SafeString { void assureTermination(); };
}
namespace heart_meter {
struct SafeString { void assureTermination(); };
}
namespace heart_island {
struct SafeString { void assureTermination(); };
}
namespace heart_sea {
struct SafeString { void assureTermination(); };
}
namespace icon_slide_area {
struct SafeString { void assureTermination(); };
}
namespace information_screen {
struct SafeString { void assureTermination(); };
}
namespace information_controller {
struct SafeString { void assureTermination(); };
}
namespace keyboard_text_area {
struct SafeString { void assureTermination(); };
}
namespace keyboard_cursor {
struct SafeString { void assureTermination(); };
}
namespace keyboard_key {
struct SafeString { void assureTermination(); };
}
namespace loading_icon {
struct SafeString { void assureTermination(); };
}
namespace loop_boss_icon {
struct SafeString { void assureTermination(); };
}
namespace magic_bar {
struct SafeString { void assureTermination(); };
}
namespace hud_main {
struct SafeString { void assureTermination(); };
}
namespace sea_chart_menu {
struct SafeString { void assureTermination(); };
}
namespace sea_chart_map {
struct SafeString { void assureTermination(); };
}
namespace sea_chart_link {
struct SafeString { void assureTermination(); };
}
namespace map_display {
struct SafeString { void assureTermination(); };
}
namespace menu_buttons {
struct SafeString { void assureTermination(); };
}
namespace main_menu {
struct WideSafeString { void assureTermination(); };
}
namespace menu_map {
struct SafeString { void assureTermination(); };
}
namespace miiverse_menu {
struct SafeString { void assureTermination(); };
}
namespace miiverse_comment {
struct SafeString { void assureTermination(); };
}
namespace miiverse_new_comment {
struct SafeString { void assureTermination(); };
}
namespace miiverse_settings {
struct SafeString { void assureTermination(); };
}
namespace moon_screen {
struct SafeString { void assureTermination(); };
}
namespace message_choice {
struct SafeString { void assureTermination(); };
}
namespace message_control {
struct SafeString { void assureTermination(); };
}

namespace baton_sign {
struct Screen {
    void waitEnter();
    void waitExit();
    void inExit();
    void displayEnter();
    void displayExit();
    void outExit();
};
}

namespace blur_background {
struct Screen {
    void blankEnter();
    void blankExit();
    void setTextureEnter();
    void setTextureExit();
    void inAllExit();
    void outAllExit();
};
}
namespace baton_window {
struct Screen {
    void inExit();
    void waitEnter();
    void waitExecute();
    void waitExit();
    void outExit();
};
}
namespace triforce_button {
struct Screen {
    void triforceDemoEndExecute();
    void triforceDemoEndExit();
    void loopExit();
    void triforceDemoExit();
};
}

namespace baton_sign_list {
struct Screen {
    void displayExit();
    void displayTouchExit();
    void inAllExit();
    void outAllExit();
};
}
}
