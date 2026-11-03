/** Opaque receiver roles for empty game hooks. No complete object layout is implied. */
#pragma once
#include "bindings.h"
#include "cg/empty_2_3.h"

namespace cgnoop { namespace parts_basic_button {
} }

namespace cgnoop { namespace parts_window {
struct WindowNodeString {
    void assureTermination();
};
struct WindowDelimiter {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_window_callbacks {
struct TreeVisitReceiver {
    void* visitNode(void* node);
};
} }

namespace cgnoop { namespace parts_button {
struct ButtonString {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_heart {
struct String {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_in_out_loop {
struct InOutLoopSafeString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateIn();
    void enterStateLoop();
    void exitStateLoop();
};
} }

namespace cgnoop { namespace parts_key_icon {
struct KeyIconSafeString {
    void assureTermination();
};
struct StateReceiver {
    void exitStateIn();
    void exitStateWait();
    void exitStateDecide();
    void exitStateOut();
};
} }

namespace cgnoop { namespace parts {
struct StateReceiver {
    void enterStateBlank();
    void executeStateBlank();
    void exitStateBlank();
    void enterStateDisplay();
    void exitStateDisplay();
};
} }

namespace cgnoop { namespace layout_res_mgr {
struct ResourceString {
    void assureTermination();
};
} }

namespace cgnoop { namespace layout_loop {
struct LoopString {
    void assureTermination();
};
} }

namespace cgnoop { namespace msg_window_states {
struct StateReceiver {
    void enterStateAutoStop();
    void enterStateHandStop();
    void enterStateSelectStartWait();
    void enterStateSelectNpcAnswerWait();
    void enterStateCloseWait();
    void enterStateAutoCloseWait();
    void enterStateHandCloseWait();
    void enterStateContinue();
    void exitStateCameraToOpen();
    void exitStateCameraToOutNow();
    void exitStateOpen();
    void exitStateOutNow();
    void exitStateKeyIconInAnimEndWaitToStop();
    void exitStateStop();
    void exitStateAutoStop();
    void exitStateKeyIconInAnimEndWaitToHandStop();
    void exitStateHandStop();
    void exitStateKeyIconInAnimEndWaitToSelectStartWait();
    void exitStateSelectStartWait();
    void exitStateSelectTwo();
    void exitStateSelectThree();
    void exitStateSelectNpcAnswerWait();
    void exitStateSelectOutContinue();
    void exitStateInput();
    void exitStateKeyIconInAnimEndWaitToCloseWait();
    void exitStateCloseWait();
    void exitStateAutoCloseWait();
    void exitStateKeyIconInAnimEndWaitToHandCloseWait();
    void exitStateHandCloseWait();
    void exitStateKeyIconInAnimEndWaitToContinue();
    void exitStateContinue();
    void exitStateKeyIconDecideAnimEndWaitToOutNow();
    void exitStateKeyIconDecideAnimEndWaitToOutNowTwo();
    void exitStateKeyIconOutAnimEndWaitToOutNow();
    void exitStateKeyIconDecideAnimEndWaitToClose();
    void exitStateKeyIconDecideAnimEndWaitToNpcAnswerWait();
    void exitStateKeyIconDecideAnimEndWaitToCloseAfterInput();
    void exitStateCloseAfterInput();
};
} }

namespace cgnoop { namespace layout_wait_screen {
struct ScreenString {
    void waitAssureName();
};
} }

namespace cgnoop { namespace msg_window_event_handlers {
struct MsgEvent_window {
    void hdMsgEvent_baseEnterTextWait();
    void hdMsgEvent_baseExitWait();
};
} }

namespace cgnoop { namespace msg_window_callbacks {
struct Receiver {
    void onEnterOpen();
    void onReset();
    void onMessage();
    void onAdvanceText();
    void onEnterAccept();
    void onUpdateCameraToOpen();
    void onEnterChoiceLayout();
    void onEnterSelectionConfirm();
    void onUpdateHandStopEnd();
    void onEnterStop();
    void onEnterClose();
    void onEnterCloseBlocked();
    void onStop();
};
} }

namespace cgnoop { namespace parts_no_control {
struct NoControlName {
    void assureTermination();
};
} }

namespace cgnoop { namespace no_decide_button {
struct ButtonName {
    void assureTermination();
};
struct StateReceiver {
    void enterStateOff();
    void exitStateOff();
    void enterStateOnTouch();
    void exitStateOnTouch();
    void enterStateOnCursor();
    void exitStateOnCursor();
    void exitStateSelectTouch();
    void exitStateSelectCursor();
    void exitStateUnSelect();
    void enterStateInvalid();
    void exitStateInvalid();
    void exitStateIn();
    void exitStateOut();
};
} }
