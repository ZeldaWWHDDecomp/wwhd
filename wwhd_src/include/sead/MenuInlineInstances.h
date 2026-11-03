/* UI inline instances; canonical layouts from merged SEAD#1, binary-only reconstruction. */
#pragma once
#include "sead/InlineInstances.h"

namespace sead { namespace menu_instances {
namespace menu_map_states {
struct StateId : instances::StateIdLayout {
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace menu_miiverse {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace menu_miiverse_comment {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace menu_miiverse_new_comment {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace menu_miiverse_setting {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace msg_choice_control {
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
namespace msg_only_window {
struct BufferedText {
    gptr<be<char>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace msg_only_control {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace msg_main_window {
struct BufferedText {
    gptr<be<char>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace msg_event_position {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace msg_window_event_frame {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace msg_only2 {
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
namespace msg_window_scroll_layout {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace no_operate_drc {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace pic_msg {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace pause {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace pictograph_camera {
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace pictograph_camera_resources {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace pictograph_box_resources {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace pictograph_list {
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace pictograph_list_resources {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace menu_window {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace rupy {
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
namespace rupy_sword_counter {
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<u32> mCapacity;
    void assureTermination();
};
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace rupy_sword_counter_2 {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace title_controller_screen {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace sequence_option {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace system_window {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(instances::StateIdLayout));
}
namespace item_shortcut {
struct StateId : instances::StateIdLayout {
    void enter(void* object);
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
} }
