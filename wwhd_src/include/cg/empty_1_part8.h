#pragma once
#include "wwhd.h"

/* Empty compiler-generated virtual functions whose vtables are known from the binary (CG-empty#1,
 * second batch). Receivers are opaque: none of these bodies reads or writes a field. */

// SafeStringBase: the termination hook (vtable slot +0x14) of the TU's abstract string base. Its
// vtable (pure-virtual placeholder in the destructor slot) is never installed; probably the base of
// the TU's string class whose vtable precedes it. Callers of slot +0x14 pass only the string and
// ignore any result.

// StateHooks (wind_operation): dOperate_wind_c's state callbacks with empty bodies. The state IDs
// Wait/InAll/Disp/OutWait/OutAll call virtual indices 25 + 3*state + (0 initialize, 1 execute,
// 2 finalize) on the wind screen (vtable 100EED70); these are the indices without a body. Same
// contract as the merged siblings (dOperate_wind_c::initializeState_InAll etc.): receiver only.

// StateIdBase: the default enter/execute/leave of the TU's state ID base class. Its vtable (the
// one right before the TU's installed state ID vtable, same 9-slot shape, same id getter in slot
// +0x14) is never installed; the derived state ID's overrides of these slots forward to the owner's
// member functions (identical forwarders in all 74 TUs, owner in r4). Contract from the state
// dispatchers that call these slots (six identical copies: 025FA070, 0268AFF0, 026FA2A4 =
// hdPartsState_c::enter/execute/exit, 0270C7A0, 02719940, 0271EDE8): r3 = the state ID, r4 = the
// owner object; enter/execute are tail calls whose results no caller reads, leave's result is
// dropped.

namespace cgempty {
namespace map_pattern {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace menu_button {
struct SafeStringBase {
    void assureTermination();
};
}
namespace menu_button {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace menu_arrow {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace menu_heart {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace miiverse_comment_button {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace triforce_button {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace character_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace cannon_game {
struct SafeStringBase {
    void assureTermination();
};
}
namespace cannon_game {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace command_prompt_a {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace command_guide {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace command_button_guide {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace wind_operation {
struct StateHooks {
    void initializeState_Wait();
    void finalizeState_Wait();
    void finalizeState_InAll();
    void initializeState_OutWait();
    void finalizeState_OutWait();
};
}
namespace wind_operation {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace wind_confirmation {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace crawling_direction {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace cursor_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace message_cursor {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace fixed_cursor {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace dungeon_key {
struct SafeStringBase {
    void assureTermination();
};
}
namespace dungeon_key {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace dungeon_map_root {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace dungeon_map_statue {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace course_selection {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace course_thanks {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace ending_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
namespace enemy_health {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
}
}
