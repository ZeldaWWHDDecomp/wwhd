/* HD UI inline instances; layouts from merged SEAD#1 and original instructions. */
#pragma once
#include "sead/InlineInstances.h"

namespace sead { namespace screen_instances {
namespace screen_e3_select_course {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_e3_thank_you {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_ending_end {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_enemy_hp {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_fade_wipe {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_fade_wipe_iris {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace detached_char_buffer {
struct BufferedText {
    gptr<be<char>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace screen_game_over {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_game_over_char {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_heart_states {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_icon_slide_area {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_icon_slide_area_mirror {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_information {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_information_drc {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace keyboard_text_area {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace keyboard_cursor {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_loop_boss_icon_states {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_magic_bar {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_sea_chart_menu {
struct BufferedText {
    gptr<be<char>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace screen_sea_chart_map {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
struct BufferedText {
    gptr<be<char>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace sea_chart_link_pos {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace sea_chart_state_handlers {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_map_display_coordinator {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_menu_btns {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace screen_menu_main {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace detached_wide_buffer {
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_menu_map_states {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
} }
