#pragma once
#include "wwhd.h"
#include "cg/empty_1_part4.h"

// Opaque state receivers; no fields or complete layouts are claimed.
namespace cgempty {
namespace touch_slide {
struct StateHooks {
    void touchSlideDecideExit();
    void touchSlideDecideCmdExit();
    void iconChangeExit();
    void arrowLoopExit();
    void floatExit();
    void floatCancelExit();
    void numExit();
    void switchExit();
};
}
namespace floor_button {
struct StateHooks {
    void offEnter();
    void offExit();
    void onCursorEnter();
    void onCursorExit();
    void selectCursorExit();
};
}
namespace command_prompt_a {
struct StateHooks {
    void inAllExit();
    void outAllExit();
};
}
namespace menu_arrow {
struct StateHooks {
    void selectTouchExit();
    void selectCursorExit();
    void decideExit();
};
}
namespace map_pattern {
struct StateHooks {
    void iconChangeExit();
    void mapPatternExit();
};
}
namespace menu_button {
struct StateHooks {
    void decideExit();
    void cancelExit();
};
}
namespace miiverse_comment_button {
struct StateHooks {
    void friendExit();
    void yeahExit();
};
}
namespace enemy_tower {
struct EntryHook { void enterIn(); };
}
namespace fairy {
struct EntryHook { void enterIn(); };
}
namespace heart_island {
struct EntryHook { void enterIn(); };
}
namespace heart_sea {
struct EntryHook { void enterIn(); };
}
namespace moon {
struct EntryHook { void enterIn(); };
}
namespace sea_chart_link {
struct EntryHook { void enterIn(); };
}
namespace keyboard_cursor {
struct StateHooks {
    void loopAllExit();
};
}
namespace keyboard_text_area {
struct StateHooks {
    void inAllExit();
    void outAllExit();
};
}
namespace dungeon_map_caption {
struct CaptionTextString { void assureTermination(); };
}
namespace mirror_literal {
struct SafeString { void assureTermination(); };
}
namespace main_menu_resource {
struct SafeString { void assureTermination(); };
}
namespace dungeon_item_icon {
struct StateHooks { void iconChangeExit(); };
}
}
