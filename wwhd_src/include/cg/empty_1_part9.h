#pragma once
#include "wwhd.h"

/* Empty compiler-generated virtual functions whose vtables are known from the binary (CG-empty#1,
 * second batch). Receivers are opaque: none of these bodies reads or writes a field. */

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
namespace fade_wipe {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace iris_wipe {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace game_over_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace game_over_character {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace heart_meter {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace icon_slide_area {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace icon_slide_mirror {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace information_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace information_controller {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace keyboard_text_area {
struct SafeStringBase {
    void assureTermination();
};
}
namespace keyboard_text_area {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace keyboard_cursor {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace loop_boss_icon {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace magic_bar {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace sea_chart_map {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace sea_chart_link {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace sea_chart_states {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace map_display {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace menu_buttons {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace main_menu {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace menu_map {
struct SafeStringBase {
    void assureTermination();
};
}
namespace menu_map {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace miiverse_menu {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace miiverse_comment {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace miiverse_new_comment {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace miiverse_settings {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace message_choice {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace message_only {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
}
