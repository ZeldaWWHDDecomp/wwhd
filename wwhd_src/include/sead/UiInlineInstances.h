/* UI inline instances; canonical layouts from merged SEAD#1, binary-only reconstruction. */
#pragma once
#include "sead/InlineInstances.h"

namespace sead { namespace ui_instances {
namespace touch_slide_decide {
struct StateId : instances::StateIdLayout {
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace item_screen_tu3 {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace btn_map_icon_extra {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace btn_map_pattern {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace btn_menu {
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
namespace btn_menu_arrow {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace btn_menu_heart {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace btn_miiverse_comment {
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
namespace pictograph_post {
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace btn_triforce {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace character {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace cannon_game_2 {
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
namespace command_a_extra {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace command_guide_extra {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace command_button {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace operate_wind {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace control_wind_ok_states {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace crawling_direction {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace cursor {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace msg_cursor {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace fixed_cursor {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace dungeon_key_states {
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
namespace dungeon_map_part2 {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace dungeon_map_statue {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
// This buffer instance has no established owning table in the current port.
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
} }
