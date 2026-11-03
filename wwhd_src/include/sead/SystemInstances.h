/* Own per-TU instances using the merged binary-derived sead layouts. */
#pragma once
#include "sead/InlineInstances.h"
namespace sead { namespace system_instances {
namespace screen_item_shortcut {
struct StateId : sead::instances::StateIdLayout {
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_slide_assist_tv {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_staff_roll {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_staff_roll_bg {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_staff_roll_bubble_base {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace title_start_background_states {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_swim_time_hud {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace telescope_hud {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mText, 0);
WWHD_OFFSET(BufferedWideText, mVtable, 4);
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_telescope_cover {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace time_display {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mText, 0);
WWHD_OFFSET(BufferedWideText, mVtable, 4);
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_time {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace title_logo_states {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_tre_map {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace unread_miiverse {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mText, 0);
WWHD_OFFSET(BufferedWideText, mVtable, 4);
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_unread_miiverse {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_warp_area {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace screen_warp_map {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace parts_basic_button {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace parts_window {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mText, 0);
WWHD_OFFSET(BufferedText, mVtable, 4);
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace parts_window {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mText, 0);
WWHD_OFFSET(BufferedWideText, mVtable, 4);
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace detached_text {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mText, 0);
WWHD_OFFSET(BufferedText, mVtable, 4);
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace parts_heart {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace parts_in_out_loop {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace parts_key_icon {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace parts {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace parts_layout {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mText, 0);
WWHD_OFFSET(BufferedText, mVtable, 4);
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace layout_resource_manager {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mText, 0);
WWHD_OFFSET(BufferedText, mVtable, 4);
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace layout_resource_manager {
struct Delegate : sead::instances::DelegateLayout { void* invoke(); };
WWHD_SIZE(Delegate, sizeof(sead::instances::DelegateLayout));
}
namespace layout_wait_screen {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mText, 0);
WWHD_OFFSET(BufferedText, mVtable, 4);
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace msg_window_states {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace no_decide_button {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace copy_text_pane {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mText, 0);
WWHD_OFFSET(BufferedText, mVtable, 4);
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace parts_text_pane {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mText, 0);
WWHD_OFFSET(BufferedWideText, mVtable, 4);
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace unbound_text {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mText, 0);
WWHD_OFFSET(BufferedText, mVtable, 4);
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace parts_valid {
struct StateId : sead::instances::StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(sead::instances::StateIdLayout));
}
namespace detached_wide_text {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mText, 0);
WWHD_OFFSET(BufferedWideText, mVtable, 4);
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
} }
