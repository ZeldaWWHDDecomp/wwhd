/* Wii U sead inline-instance layouts, reconstructed from the game instructions. */
#pragma once
#include "wwhd.h"
#include "sead/RuntimeType.h"

namespace sead { namespace instances {
struct MethodEntry { be<u32> mAdjustment; be<u32> mFunction; };
struct MemberFunction {
    be<s16> mThisAdjustment;
    be<s16> mVirtualIndex;
    union {
        be<u32> mFunction;
        struct { be<s16> mReserved; be<s16> mVtableOffset; };
    };
};
struct StateMethods { u8 mPrefix[0x14]; be<u32> mResolveId; };
struct StateIdLayout {
    be<s32> mId;
    gptr<const char> mName;
    gptr<StateMethods> mVtable;
    MemberFunction mEnter;
    MemberFunction mExecute;
    MemberFunction mLeave;
    MemberFunction mReservedCallback;
    gptr<StateIdLayout> mParentState;
};
struct DelegateLayout { be<u32> mVtable; gptr<void> mObject; MemberFunction mCallback; };
WWHD_SIZE(MemberFunction, 8);
WWHD_OFFSET(MemberFunction, mVirtualIndex, 2);
WWHD_OFFSET(MemberFunction, mVtableOffset, 6);
WWHD_SIZE(MethodEntry, 8);
WWHD_OFFSET(MethodEntry, mFunction, 4);
WWHD_OFFSET(StateIdLayout, mEnter, 0xC);
WWHD_OFFSET(StateIdLayout, mExecute, 0x14);
WWHD_OFFSET(StateIdLayout, mLeave, 0x1C);
WWHD_OFFSET(StateIdLayout, mParentState, 0x2C);
WWHD_SIZE(StateIdLayout, 0x30);
WWHD_OFFSET(DelegateLayout, mObject, 4);
WWHD_OFFSET(DelegateLayout, mCallback, 8);
WWHD_SIZE(DelegateLayout, 0x10);
namespace font_mgr {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace msg_res_mgr {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace msg_text {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace msg_text {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace state_task {
struct Delegate : DelegateLayout {
    void* invoke();
};
WWHD_SIZE(Delegate, sizeof(DelegateLayout));
}
namespace state_task {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace text_unit_proc {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace text_unit_proc {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace text_ruby {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace res_model {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace res_load_task {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace res_mgr {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace res_mgr {
struct Delegate : DelegateLayout {
    void* invoke();
};
WWHD_SIZE(Delegate, sizeof(DelegateLayout));
}
namespace screen_arrow {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_auction_rupy {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_auction_number {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_baton_sign {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_baton_sign_for_window {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_baton_sign_list_states {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_battery {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_battleship_game {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_battleship_game {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_best_shot_mark_init {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_blur_bg {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_blur_bg {
struct BufferedText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedText, mCapacity, 8);
WWHD_SIZE(BufferedText, 0xC);
}
namespace screen_boat_race_failed {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_boat_race_failed_char_states {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_boat_race_failed_char_states {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_boat_race_result_parts {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_boat_race_start {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_boot {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_boss_hp {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_btn_collect_icon {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_btn_collect_icon_mask {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_btn_control_wind {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_btn_difficulty {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_btn_dungeon_item_icon {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_btn_floor {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_btn_floor {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_menu_item {
struct StateId : StateIdLayout {
    void enter(void* object);
    void execute(void* object);
    void leave(void* object);
    u32 resolveId();
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace screen_menu_item {
struct BufferedWideText { gptr<be<u16>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
WWHD_OFFSET(BufferedWideText, mCapacity, 8);
WWHD_SIZE(BufferedWideText, 0xC);
}
namespace screen_touch_slide_decide {
struct StateId : StateIdLayout {
    void enter(void* object);
};
WWHD_SIZE(StateId, sizeof(StateIdLayout));
}
namespace res_mgr {
u32 isPackResource(void* object, const RuntimeType* type);
}
} }
