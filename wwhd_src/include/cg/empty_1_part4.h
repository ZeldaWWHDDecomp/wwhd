#pragma once
#include "wwhd.h"
#include "cg/empty_1_part3.h"

// Opaque state callback receivers; no complete object layouts are claimed.
namespace cgempty {

namespace fade_wipe {
struct StateHooks {
    void inAllExit();
    void outAllExit();
};
}
namespace command_guide {
struct StateHooks {
    void noButtonExit();
    void waitEnter();
    void waitExit();
    void outExit();
    void inExit();
};
}
namespace battleship_game {
struct StateHooks {
    void inAllExit();
    void outAllEnter();
    void waitEnter();
    void waitExecute();
    void waitExit();
};
}
namespace action_command {
struct StateHooks {
    void waitExit();
    void displayExit();
};
}
namespace character_screen {
struct StateHooks {
    void loopExit();
};
}
namespace enemy_health {
struct StateHooks {
    void waitEnter();
    void waitExit();
    void displayEnter();
    void displayExit();
};
}
namespace iris_wipe {
struct StateHooks {
    void fadeExit();
    void inAllExit();
    void outAllExit();
};
}
namespace icon_slide_mirror {
struct StateHooks {
    void floatExit();
    void floatCancelExit();
    void switchExit();
};
}

namespace ending_screen {
struct StateHooks {
    void allExit();
};
}
namespace best_shot_mark {
struct StateHooks {
    void inExit();
    void loopExit();
    void outExit();
    void invisibleExecute();
    void invisibleExit();
};
}
namespace sea_chart_map {
struct StateHooks {
    void inAllExit();
    void outAllExit();
    void scrollExit();
    void dragExit();
    void toZoomInExit();
    void toZoomOutExit();
    void zoomOutEnter();
    void zoomOutExit();
    void zoomInToCenteringPlayerExit();
    void zoomInEnter();
    void zoomInExit();
    void zoomInCenteringPlayerEnter();
    void zoomInCenteringPlayerExit();
};
}
namespace loop_boss_icon {
struct StateHooks {
    void waitExit();
};
}
namespace menu_map {
struct StateHooks {
    void inAllExit();
    void windowNowLoopExit();
    void treasureMapOutExit();
    void treasureMapInExit();
    void treasureMapChangeLeftExit();
    void treasureMapChangeRightExit();
    void areaNameOutExit();
    void areaNameInExit();
    void changeToNoDataExit();
    void changeToDisplayExit();
    void noDataExit();
    void displayExit();
    void blankEnter();
    void blankExecute();
    void blankExit();
    void mapHelpfulInAllExit();
    void mapHelpfulOutAllEnter();
    void mapHelpfulDisplayEnter();
    void mapHelpfulDisplayExit();
};
}

namespace sea_chart_link {
struct StateHooks {
    void displayExit();
    void blankEnter();
    void blankExecute();
    void blankExit();
    void inAllExit();
    void outAllExit();
};
}
namespace collection_icon {
struct StateHooks {
    void iconChangeExit();
};
}
namespace sea_chart_states {
struct StateHooks {
    void inAllExit();
    void outAllExit();
};
}
namespace information_controller {
struct StateHooks {
    void inAllExit();
    void outAllExit();
};
}
namespace battery_screen {
struct StateHooks {
    void waitExit();
    void displayExit();
};
}
namespace wind_confirmation {
struct StateHooks {
    void waitExit();
    void displayExit();
    void outWaitExit();
    void outAllExit();
};
}
}
