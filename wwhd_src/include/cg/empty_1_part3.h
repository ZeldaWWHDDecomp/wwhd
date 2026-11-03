#pragma once
#include "wwhd.h"

// Opaque receivers: these hooks access no fields and imply no complete layout.
namespace cgempty {
namespace input_cursor {
struct IconName { void assureTermination(); };
}
namespace enemy_health {
struct Screen { void reset(); };
}

namespace dungeon_map_root {
struct StateHooks {
    void displayEnter();
    void displayExit();
    void blankEnter();
    void blankExecute();
    void blankExit();
    void loopLinkIconExit();
    void loopBossIconExit();
    void changeStoryExit();
    void getItemExit();
    void changeToNoDataExit();
    void changeToDisplayExit();
    void noDataExit();
    void toZoomInExit();
    void toZoomOutExit();
};
}
namespace dungeon_map_floor {
struct StateHooks {
    void blankEnter();
    void blankExecute();
    void blankExit();
};
}
namespace dungeon_map_marker {
struct StateHooks {
    void displayEnter();
    void displayExit();
    void blankEnter();
    void blankExecute();
    void blankExit();
};
}

namespace game_over_screen {
struct StateHooks { void waitExit(); };
}

namespace magic_bar {
struct StateHooks {
    void waitExit();
    void inExit();
    void displayEnter();
    void displayExit();
    void outExit();
};
}
namespace message_choice {
struct StateHooks {
    void openExit();
    void openNewMessageExit();
    void closeExit();
    void deletionEnter();
    void deletionExit();
    void selectExit();
    void closeWaitEnter();
    void closeWaitExit();
};
}
namespace information_screen {
struct StateHooks {
    void loopAllExit();
    void inAllExit();
    void outAllExit();
};
}
namespace heart_meter {
struct StateHooks {
    void waitExit();
    void inExit();
    void displayEnter();
    void displayExit();
    void outExit();
    void battleEnter();
    void battleExit();
    void fileSelectExit();
};
}
namespace boot_screen {
struct StateHooks {
    void loopExit();
    void outAllExit();
};
}

namespace menu_heart {
struct StateHooks {
    void heartPieceExit();
};
}
namespace fixed_cursor {
struct StateHooks {
    void blankEnter();
    void blankExit();
    void inAllExit();
    void loopAllExit();
    void outAllExit();
};
}
namespace dungeon_key {
struct StateHooks {
    void waitExit();
    void inExit();
    void displayEnter();
    void displayExit();
    void outExit();
};
}
namespace icon_slide_area {
struct StateHooks {
    void floatExit();
    void floatCancelExit();
    void switchExit();
};
}
namespace cannon_game {
struct StateHooks {
    void inAllExit();
    void waitEnter();
    void waitExecute();
    void waitExit();
};
}
namespace message_cursor {
struct StateHooks {
    void inAllExit();
    void loopAllExecute();
    void loopAllExit();
    void outAllExit();
};
}
namespace crawling_direction {
struct StateHooks {
    void waitExit();
    void displayExit();
    void endExit();
};
}
namespace command_button_guide {
struct StateHooks {
    void noButtonEnter();
    void noButtonExit();
    void inExit();
    void waitEnter();
    void waitExit();
    void outExit();
};
}
namespace main_menu {
struct StateHooks {
    void equipItemExit();
    void inArrowLeftExit();
    void inArrowRightExit();
    void outArrowLeftExit();
    void outArrowRightExit();
    void equipToCollectExit();
    void collectToEquipExit();
    void dragExit();
    void slideEnter();
    void inAllExit();
    void outAllExit();
    void inArrowExit();
    void outArrowExit();
    void itemCaptionInExit();
    void itemCaptionOutExit();
    void textFadeExit();
};
}
namespace boss_health {
struct StateHooks {
    void waitExit();
    void inExit();
    void displayEnter();
    void displayExit();
};
}
namespace cursor_screen {
struct StateHooks {
    void blankEnter();
    void blankExit();
    void inAllExit();
    void loopAllExit();
    void outAllExit();
};
}
namespace dungeon_map_statue {
struct StateHooks {
    void setStatueExit();
};
}
}
