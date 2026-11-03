#pragma once
#include "wwhd.h"

// Opaque receivers only: immutable string termination hooks access no fields.
namespace cgempty {
namespace font_manager {
struct SafeString { void assureTermination(); };
}
namespace message_resources {
struct SafeString { void assureTermination(); };
}
namespace text_unit {
struct SafeString { void assureTermination(); };
struct WideSafeString { void assureTermination(); };
}
namespace ruby_text {
struct WideSafeString { void assureTermination(); };
}
namespace model_resources {
struct SafeString { void assureTermination(); };
}

// State callbacks are opaque: their original bodies perform no work.
namespace course_selection {
struct Screen {
    void waitExecute();
    void waitFinalize();
    void outAllEndInitialize();
    void outAllEndExecute();
    void outAllEndFinalize();
};
}
namespace game_over_character {
struct Screen {
    void inLeave();
    void delayLeave();
    void completeLeave();
};
}
namespace map_display {
struct Coordinator {
    void blankEnter();
    void blankExecute();
    void blankLeave();
    void displayMenuEnter();
    void displayMenuLeave();
    void displayDungeonEnter();
    void displayDungeonLeave();
    void inAllMenuLeave();
    void inAllDungeonLeave();
    void outAllMenuLeave();
    void outAllDungeonLeave();
};
}
namespace miiverse_comment {
struct Screen {
    void inNewPostExit();
    void slideDownExit();
    void waitExecute();
    void waitExit();
    void photoExit();
};
}
namespace miiverse_new_comment {
struct Screen {
    void waitEnter();
    void waitExecute();
    void waitExit();
    void inOutExit();
    void appearExit();
};
}
namespace miiverse_settings {
struct Screen {
    void waitEnter();
    void waitExecute();
    void waitExit();
    void inputEnter();
    void slideDownExit();
};
}

namespace resource_load_task {
struct SafeString {
    void assureTermination();
};
}
namespace resource_pack {
struct SafeString {
    void assureTermination();
};
}
namespace input_events {
struct SafeString {
    void assureTermination();
};
}
namespace software_keyboard {
struct SafeString {
    void assureTermination();
};
}
namespace keyboard_text {
struct Pane {
    void enter();
    void leave();
};
}

namespace arrow_screen {
struct SafeString { void assureTermination(); };
}
namespace auction_currency {
struct SafeString { void assureTermination(); };
}
namespace auction_number {
struct SafeString { void assureTermination(); };
}
namespace baton_input {
struct SafeString { void assureTermination(); };
}
namespace baton_completion {
struct SafeString { void assureTermination(); };
}
namespace baton_sign {
struct SafeString { void assureTermination(); };
}
namespace baton_window {
struct SafeString { void assureTermination(); };
}
namespace battery_screen {
struct SafeString { void assureTermination(); };
}

namespace baton_sign_list {
struct SafeString { void assureTermination(); };
}
namespace battleship_screen {
struct SafeString { void assureTermination(); };
}
namespace battleship_game {
struct SafeString { void assureTermination(); };
}
namespace best_shot_mark {
struct SafeString { void assureTermination(); };
}
namespace blur_background {
struct SafeString { void assureTermination(); };
}
namespace boat_race_failure {
struct SafeString { void assureTermination(); };
}
namespace boat_race_failure_character {
struct SafeString { void assureTermination(); };
}
namespace boat_race_result {
struct SafeString { void assureTermination(); };
}
namespace boat_race_start {
struct SafeString { void assureTermination(); };
}
namespace boot_screen {
struct SafeString { void assureTermination(); };
}
namespace boss_health {
struct SafeString { void assureTermination(); };
}
namespace collection_icon {
struct SafeString { void assureTermination(); };
}

namespace collection_icon_mask {
struct SafeString { void assureTermination(); };
}
namespace wind_control {
struct SafeString { void assureTermination(); };
}
namespace difficulty_selection {
struct SafeString { void assureTermination(); };
}
namespace dungeon_item_icon {
struct SafeString { void assureTermination(); };
}
namespace floor_button {
struct SafeString { void assureTermination(); };
}
namespace item_menu {
struct SafeString { void assureTermination(); };
}
namespace touch_slide {
struct SafeString { void assureTermination(); };
struct WideSafeString { void assureTermination(); };
}
namespace item_icon {
struct SafeString { void assureTermination(); };
}
namespace map_icon {
struct SafeString { void assureTermination(); };
}
namespace map_icon_count {
struct SafeString { void assureTermination(); };
}
namespace map_pattern {
struct SafeString { void assureTermination(); };
}
namespace menu_button {
struct SafeString { void assureTermination(); };
}
namespace menu_arrow {
struct SafeString { void assureTermination(); };
}
namespace menu_heart {
struct SafeString { void assureTermination(); };
}
namespace miiverse_comment_button {
struct SafeString { void assureTermination(); };
struct WideSafeString { void assureTermination(); };
}
namespace miiverse_new_comment_button {
struct SafeString { void assureTermination(); };
}
namespace miiverse_picture_button {
struct SafeString { void assureTermination(); };
}
namespace pearl_button {
struct SafeString { void assureTermination(); };
}
namespace pictograph_button {
struct SafeString { void assureTermination(); };
}
namespace pictograph_post {
struct SafeString { void assureTermination(); };
struct WideSafeString { void assureTermination(); };
}
namespace triforce_button {
struct SafeString { void assureTermination(); };
}
namespace character_screen {
struct SafeString { void assureTermination(); };
}
namespace cannonball_screen {
struct SafeString { void assureTermination(); };
}
namespace cannon_game {
struct SafeString { void assureTermination(); };
}
}
