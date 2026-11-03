/** Opaque receiver roles for empty game hooks. No complete object layout is implied. */
#pragma once
#include "bindings.h"
#include "cg/empty_2_1.h"

namespace cgnoop { namespace pictograph_camera_resources {
} }

namespace cgnoop { namespace pictograph_message_pane {
struct MessageString {
    void assureTermination();
};
} }

namespace cgnoop { namespace pictograph_box_resources {
struct StateReceiver {
    void exitStateInAll();
    void exitStateOutAll();
};
struct PictographBox {
    void pictographInExit();
    void pictographOutExit();
    void pictographOutFinish();
    void pictographWaitExecute();
    void pictographWaitExit();
};
} }

namespace cgnoop { namespace parts_common {
struct PictographBox {
    void exitStateDisplay();
};
struct TitleControllerSelection {
    void exitStateDisplay();
    void exitStateInAll();
    void exitStateOutAll();
};
} }

namespace cgnoop { namespace pictograph_list {
struct ListNameString {
    void assureTermination();
};
} }

namespace cgnoop { namespace pictograph_list_resources {
struct StateReceiver {
    void executeStateBlank();
    void exitStateBlank();
    void exitStateInAll();
    void enterStateState();
    void executeStateState();
    void exitStateState();
    void enterStateDrag();
    void exitStateDrag();
    void exitStateSlideLeft();
    void exitStateSlideRight();
    void exitStateSlideEndZero();
    void exitStateSlideEndOne();
    void exitStateSlideEndTwo();
    void exitStateCheckDelete();
    void enterStateDelete();
    void exitStateDelete();
    void exitStateMoveOver();
    void exitStateCheckDecide();
    void exitStateBackToBox();
    void enterStatePosting();
    void exitStatePosting();
    void exitStateReset();
};
} }

namespace cgnoop { namespace menu_window_callbacks {
struct SourceString {
    u32 assureTermination();
};
} }

namespace cgnoop { namespace rupy {
struct RupyString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateWait();
    void exitStateIn();
    void exitStateOut();
};
} }

namespace cgnoop { namespace rupy_sword_counter_2 {
struct RupySwordSafeString {
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

namespace cgnoop { namespace title_scroll_key_icon {
struct ScrollKeyString {
    void assureTermination();
};
} }

namespace cgnoop { namespace secret_entrance_chart {
struct SecretEntranceString {
    void assureTermination();
};
} }

namespace cgnoop { namespace title_controller_screen {
struct ControllerSoundString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateWait();
};
} }

namespace cgnoop { namespace sequence_option {
struct OptionString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateOutAll();
    void enterStateButtonWait();
    void exitStateChangeConnect();
    void exitStateConnectedWait();
    void exitStateConnectWait();
};
} }

namespace cgnoop { namespace sequence_window_sequence {
struct SequenceString {
    void assureTermination();
};
} }

namespace cgnoop { namespace system_window {
struct StateReceiver {
    void exitStateInAll();
    void enterStateOutWait();
    void exitStateOutWait();
    void exitStateEndWait();
    void executeStateEnd();
    void exitStateEnd();
    void enterStateDecideWait();
    void exitStateDecideWait();
    void enterStateDrcWait();
    void exitStateDrcWait();
    void enterStateProWait();
    void exitStateProWait();
    void enterStateReset();
    void enterStateResetEnd();
    void executeStateResetEnd();
    void exitStateResetEnd();
};
} }

namespace cgnoop { namespace item_shortcut {
struct ItemShortcutSafeString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateWait();
    void exitStateIn();
    void enterStateDisp();
    void exitStateDisp();
    void exitStateOut();
    void exitStateChange();
};
} }

namespace cgnoop { namespace slide_assist_tv {
struct SlideString {
    void assureTermination();
};
struct StateReceiver {
    void enterStateBlank();
    void exitStateBlank();
    void enterStateDisplay();
    void exitStateDisplay();
    void exitStateOutWait();
    void exitStateInAll();
    void exitStateOutAll();
};
} }
