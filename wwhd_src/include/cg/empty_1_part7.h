#pragma once
#include "wwhd.h"

/* Empty compiler-generated virtual functions whose vtables are known from the binary (CG-empty#1,
 * second batch). Receivers are opaque: none of these bodies reads or writes a field. */

// PackResource: the 0x24-byte resource the res_mgr factory (02613E38) constructs (vtable 100E21C4
// at +0x10). finishCreate (slot +0x24): the resource manager calls it for each resource on its
// pending list (0275D0DC, result unused), then drops the resource from that list; probably the
// post-creation hook. createFromBuffer (slot +0x34): tail-called by the buffer setter (0275D21C) with
// r4 = buffer, r5 = size, r6 = heap (moved from its r8); its caller ignores the result. r7 still holds
// the setter's ownership flag there; nothing shows that it is an argument.

// SafeString: the plain string's termination hook (slot +0x14 after its destructor); its vtable is
// never installed in this TU. Same contract as every string termination hook: the string only.

// SafeStringBase: the termination hook (vtable slot +0x14) of the TU's abstract string base. Its
// vtable (pure-virtual placeholder in the destructor slot) is never installed; probably the base of
// the TU's string class whose vtable precedes it. Callers of slot +0x14 pass only the string and
// ignore any result.

// StateIdBase: the default enter/execute/leave of the TU's state ID base class. Its vtable (the
// one right before the TU's installed state ID vtable, same 9-slot shape, same id getter in slot
// +0x14) is never installed; the derived state ID's overrides of these slots forward to the owner's
// member functions (identical forwarders in all 74 TUs, owner in r4). Contract from the state
// dispatchers that call these slots (six identical copies: 025FA070, 0268AFF0, 026FA2A4 =
// hdPartsState_c::enter/execute/exit, 0270C7A0, 02719940, 0271EDE8): r3 = the state ID, r4 = the
// owner object; enter/execute are tail calls whose results no caller reads, leave's result is
// dropped.

namespace cgempty {
namespace message_text {
struct SafeString {
    void assureTermination();
};
}
namespace message_text {
struct SafeStringBase {
    void assureTermination();
};
}
namespace state_task {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace resource_pack {
struct PackResource {
    void finishCreate();
    void createFromBuffer(void* buffer, u32 size, void* heap);
};
}
namespace baton_sign {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace baton_window {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace baton_sign_list {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace battery_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace battleship_game {
struct SafeStringBase {
    void assureTermination();
};
}
namespace battleship_game {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace best_shot_mark {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace blur_background {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace boat_race_failure {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace boat_race_failure_character {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace boat_race_result {
struct SafeStringBase {
    void assureTermination();
};
}
namespace boat_race_result {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace boat_race_start {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace boot_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace boss_health {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace collection_icon {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace collection_icon_mask {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace wind_control {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace difficulty_button {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace dungeon_item_icon {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace floor_button {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace item_menu {
struct SafeStringBase {
    void assureTermination();
};
}
namespace menu_item {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace touch_slide {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace item_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace map_icon {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace map_icon_count {
struct SafeStringBase {
    void assureTermination();
};
}
}
