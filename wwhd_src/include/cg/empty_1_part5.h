#pragma once
#include "wwhd.h"

// Opaque state receivers; no object fields or complete layout are claimed.
namespace cgempty {

namespace collection_icon_mask {
struct StateHooks {
    void offEnter();
    void offExit();
    void onTouchEnter();
    void onTouchExit();
    void onCursorEnter();
    void onCursorExit();
    void selectTouchExit();
    void selectCursorExit();
    void unselectExit();
    void equipOffExit();
    void equipOnExit();
};
}

namespace boat_race_failure {
struct StateHooks {
    void waitExit();
};
}
namespace menu_buttons {
struct StateHooks {
    void displayEnter();
    void displayExit();
    void inAllExit();
    void menuOpenExit();
    void menuCloseExit();
    void outAllExit();
};
}
namespace wind_control {
struct StateHooks {
    void offEnter();
    void offExit();
    void onTouchEnter();
    void onTouchExit();
    void selectTouchExit();
    void selectCursorExit();
};
}
namespace boat_race_result {
struct StateHooks {
    void waitExit();
    void inAllExit();
};
}
namespace boat_race_failure_character {
struct StateHooks {
    void inExit();
    void waitExit();
    void sleepExit();
};
}

namespace state_task {
struct StateHooks {
    void waitEnter();
    void waitExecute();
    void waitExit();
    void inProgressExit();
    void finishExecute();
    void finishExit();
};
}
namespace difficulty_button {
struct StateHooks {
    void offEnter();
    void offExit();
    void onTouchEnter();
    void onTouchExit();
    void onCursorEnter();
    void onCursorExit();
    void selectTouchExit();
    void selectCursorExit();
    void unSelectExit();
};
}
namespace menu_item {
struct StateHooks {
    void offEnter();
    void offExit();
    void onTouchExit();
    void onCursorEnter();
    void onCursorExit();
    void selectTouchExit();
    void selectCursorExit();
    void unSelectExit();
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
namespace map_icon {
struct StateHooks {
    void reverseExit();
    void zoomInExit();
    void fullZoomInExit();
    void mapPatternExit();
    void fullZoomOutExit();
    void zoomOutExit();
    void treasureGetExit();
    void openDemoExit();
    void inExit();
    void waitExit();
};
}
namespace message_only {
struct StateHooks {
    void waitEnter();
    void waitExit();
    void outNowExit();
    void autoCloseWaitEnter();
    void autoCloseWaitExit();
    void handCloseWaitEnter();
    void handCloseWaitExit();
    void npcAnswerWaitEnter();
    void npcAnswerWaitExit();
    void continueEnter();
    void continueExit();
    void closeExit();
    void keyIconDecideAnimEndWaitToOutNowExit();
    void keyIconDecideAnimEndWaitToOutNow2Exit();
    void keyIconOutAnimEndWaitToOutNowExit();
    void keyIconDecideAnimEndWaitToCloseExit();
};
}
}
