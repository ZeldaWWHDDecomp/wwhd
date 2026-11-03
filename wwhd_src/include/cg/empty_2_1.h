/** Opaque receiver roles for empty game hooks. No complete object layout is implied. */
#pragma once
#include "bindings.h"

namespace cgnoop { namespace msg_event_position {
struct StateReceiver {
    void enterStateAfterInput();
    void exitStateOpenTact();
    void exitStateOpenItemGet();
    void exitStateTact();
    void exitStateDemo();
    void exitStateDemoTwo();
    void exitStateInput();
    void exitStateAfterInput();
    void exitStateOutNow();
    void exitStateKeyIconDecideAnimEndWaitToOutNow();
    void exitStateMsgAlphaOut();
    void exitStateMsgAlphaIn();
    void exitStateFinish();
};
} }

namespace cgnoop {
struct MainWindowSourceString {
    void assureTermination();
};
}

namespace cgnoop { namespace msg_window_event_frame {
struct FrameString {
    void assureTermination();
};
struct StateReceiver {
    void executeStateLoop();
    void exitStateLoop();
};
} }

namespace cgnoop { namespace msg_window_main2 {
struct MainTwoSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace msg_only2 {
struct StateReceiver {
    void enterStateCloseWait();
    void enterStateAutoCloseWait();
    void enterStateHandCloseWait();
    void enterStateNpcAnswerWait();
    void enterStateStop();
    void enterStateContinue();
    void exitStateInAll();
    void exitStateOutNow();
    void exitStateCloseWait();
    void exitStateAutoCloseWait();
    void exitStateHandCloseWait();
    void exitStateNpcAnswerWait();
    void exitStateOutMsg();
    void exitStateInMsg();
    void exitStateStop();
    void exitStateOutAll();
    void exitStateContinue();
    void exitStateClose();
};
struct OnlyTwoSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace msg_window_scroll_layout {
struct StateReceiver {
    void enterStateStop();
    void enterStateAfterScroll();
    void enterStateDelete();
    void exitStateOpen();
    void exitStateBeforeStop();
    void exitStateStop();
    void exitStateScrollUp();
    void exitStateScrollDown();
    void exitStateAfterScroll();
    void exitStateCloseWait();
    void exitStateKeyIconDecideAnimEndWaitToClose();
    void exitStateClose();
    void exitStateDelete();
};
} }

namespace cgnoop { namespace msg_window_scroll_material {
struct MaterialScrollString {
    void assureTermination();
};
} }

namespace cgnoop { namespace no_operate_drc {
struct NoOperateString {
    void assureTermination();
};
struct StateReceiver {
    void enterStateBlank();
    void exitStateBlank();
    void enterStateDisplay();
    void exitStateDisplay();
    void exitStateInAll();
    void exitStateOutAll();
};
} }

namespace cgnoop { namespace pic_msg {
struct PicMsgString {
    void assureTermination();
};
struct StateReceiver {
    void exitStatePicMsg_ZeroZero();
    void exitStatePicMsg_ZeroOne();
    void exitStatePicMsg_ZeroTwo();
    void exitStatePicMsg_ZeroThree();
    void exitStatePicMsg_ZeroFour();
    void exitStatePicMsg_ZeroFive();
    void exitStatePicMsg_ZeroSix();
    void exitStatePicMsg_ZeroSeven();
    void exitStatePicMsg_ZeroEight();
    void exitStatePicMsg_ZeroNine();
    void exitStatePicMsg_OneZero();
    void exitStatePicMsg_OneOne();
    void exitStatePicMsg_OneTwo();
    void exitStatePicMsg_OneThree();
    void exitStatePicMsg_OneFour();
    void exitStatePicMsg_OneFive();
    void exitStatePicMsg_OneSix();
    void exitStatePicMsg_OneSeven();
    void exitStatePicMsg_OneEight();
    void exitStatePicMsg_OneNine();
    void exitStatePicMsg_TwoZero();
    void exitStatePicMsg_TwoOne();
    void exitStateOutAll();
};
} }

namespace cgnoop { namespace pause {
struct PauseString {
    void assureTermination();
};
struct StateReceiver {
    void enterStateBlank();
    void exitStateBlank();
    void enterStateSetTexture();
    void exitStateSetTexture();
    void exitStateInAll();
    void exitStateOutAll();
};
} }

namespace cgnoop { namespace pictograph {
struct BackdropString {
    void assureTermination();
};
} }

namespace cgnoop { namespace pictograph_camera_resources {
struct StateReceiver {
    void enterStateList();
    void executeStateBlank();
    void exitStateBlank();
    void exitStateInAll();
    void exitStateScale();
    void enterStateMaxPictogaraph();
    void exitStateMaxPictogaraph();
    void exitStateInCoverSelfie();
    void exitStateWaitCover();
    void exitStateOutCoverSelfieReverse();
    void executeStateList();
    void exitStateList();
    void exitStateReset();
};
} }

namespace cgnoop { namespace pictograph_camera {
struct CameraPaneString {
    void assureTermination();
};
} }
