/** Opaque receiver roles for empty game hooks. No complete object layout is implied. */
#pragma once
#include "bindings.h"
#include "cg/empty_2_2.h"

namespace cgnoop { namespace slide_assist_tv {
} }

namespace cgnoop { namespace squid_chart {
struct SquidString {
    void assureTermination();
};
} }

namespace cgnoop { namespace squid_icon {
struct SquidIconSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace staff_roll {
struct StaffRollSafeString {
    void assureTermination();
};
struct StateReceiver {
    void enterStateAll();
    void exitStateAll();
    void enterStateCopyRight();
    void exitStateCopyRight();
};
} }

namespace cgnoop { namespace staff_roll_bg {
struct StaffRollBGSafeString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateAll();
};
} }

namespace cgnoop { namespace staff_roll_bubble {
struct BubbleSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace staff_roll_bubble_base {
struct BubbleBaseSafeString {
    void assureTermination();
};
struct StateReceiver {
    void enterStateWait();
    void executeStateWait();
    void exitStateWait();
    void exitStateMove();
};
} }

namespace cgnoop { namespace staff_roll_bubble_s {
struct BubbleSSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace title_start_background {
struct UiSafeString {
    void titleBackgroundAssureName();
};
} }

namespace cgnoop { namespace title_start_background_states {
struct StateReceiver {
    void exitStateInAll();
    void exitStateChangeText();
    void enterStateWait();
    void exitStateWait();
    void exitStateOutAll();
    void enterStateOutAllEnd();
    void executeStateOutAllEnd();
    void exitStateOutAllEnd();
};
} }

namespace cgnoop { namespace submarine_chart {
struct SubmarineString {
    void assureTermination();
};
} }

namespace cgnoop { namespace swim_time_hud {
struct SwimString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateWait();
    void exitStateIn();
    void exitStateDisp();
    void exitStateOut();
    void enterStateDead();
    void exitStateDead();
};
} }

namespace cgnoop { namespace telescope_cover {
struct StateReceiver {
    void enterStateDemoTwo();
    void exitStateInAll();
    void exitStateInCoverDemo();
    void exitStateOutCoverDemo();
    void exitStateOutAll();
    void exitStateOpenTwo();
    void exitStateDemo();
    void exitStateDemoTwo();
    void exitStateDemoThree();
    void exitStateDemoFour();
    void exitStateDemoEndWait();
    void exitStateReset();
};
} }

namespace cgnoop { namespace telescope_hud {
struct TelescopeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_common {
struct TelescopeHud {
    void exitStateBlank();
};
struct WarpMap {
    void enterStateDisplay();
    void exitStateDisplay();
    void enterStateBlank();
    void executeStateBlank();
    void exitStateBlank();
};
} }

namespace cgnoop { namespace terry_chart {
struct TerryString {
    void assureTermination();
};
} }

namespace cgnoop { namespace time {
struct TimeString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateWait();
    void exitStateIn();
    void enterStateDisp();
    void exitStateDisp();
};
} }

namespace cgnoop { namespace tingle_tower_chart {
struct TincleTowerString {
    void assureTermination();
};
} }

namespace cgnoop { namespace title_logo {
struct LogoPaneNameString {
    void assureTermination();
};
} }

namespace cgnoop { namespace title_logo_states {
struct StateReceiver {
    void exitStateWait();
    void exitStateInAll();
    void exitStateLoopText();
};
} }

namespace cgnoop { namespace tre_map {
struct String {
    void assureTermination();
};
struct StateReceiver {
    void exitStateHelpfulMapNoOpenToOpen();
    void exitStateHelpfulMapNoOpen();
    void exitStateHelpfulMapOpen();
    void exitStateTriforceMapNoOpenToUnOpen();
    void exitStateTriforceMapNoOpen();
    void exitStateTriforceMapUnOpen();
    void exitStateTreasureMapNoOpenToOpen();
    void exitStateTreasureMapNoOpen();
    void exitStateTreasureMapOpen();
};
} }

namespace cgnoop { namespace triforce_chart {
struct TriforceString {
    void assureTermination();
};
} }

namespace cgnoop { namespace unread_miiverse {
struct UnreadString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateWait();
    void exitStateIn();
    void enterStateDisp();
    void exitStateDisp();
    void exitStateOut();
};
} }

namespace cgnoop { namespace warp_area {
struct SafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace warp_map {
struct MapString {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_basic_button {
struct BasicString {
    void assureTermination();
};
struct StateReceiver {
    void enterStateEnd();
    void exitStateEnd();
    void enterStateOff();
    void exitStateOff();
    void enterStateOnTouch();
    void exitStateOnTouch();
    void enterStateOnCursor();
    void exitStateOnCursor();
    void exitStateSelectTouch();
    void exitStateSelectCursor();
    void exitStateUnSelect();
    void exitStateDecide();
    void exitStateCancel();
    void enterStateInvalid();
    void exitStateInvalid();
    void exitStateIn();
    void exitStateOut();
};
} }
