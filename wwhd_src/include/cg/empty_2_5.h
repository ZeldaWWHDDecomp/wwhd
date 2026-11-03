/** Opaque receiver roles for empty game hooks. No complete object layout is implied. */
#pragma once
#include "bindings.h"
#include "cg/empty_2_4.h"

namespace cgnoop { namespace no_decide_button {
} }

namespace cgnoop { namespace parts_one_time {
struct OneTimeName {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_progress_bar {
struct ProgressBarName {
    void assureTermination();
};
} }

namespace cgnoop { namespace copy_text_pane {
struct UiSafeString {
    void copyEnsureString();
};
} }

namespace cgnoop { namespace parts_text_pane {
struct PaneString {
    void Pane_ensureString();
};
} }

namespace cgnoop { namespace layout_locale_search {
struct LayoutLocaleString {
    void LayoutLocale_assureString();
};
} }

namespace cgnoop { namespace text_break {
struct String {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_valid {
struct String {
    void assureTermination();
};
struct StateReceiver {
    void exitStateChangeValid();
    void executeStateValid();
    void executeStateInvalid();
};
} }

namespace cgnoop { namespace scene_ending_end {
struct EndingString {
    void assureTermination();
};
} }

namespace cgnoop { namespace scene_boot {
struct BootString {
    void assureTermination();
};
} }

namespace cgnoop { namespace scene_ex_save {
struct StateReceiver {
    void exitStateExplainEX();
    void exitStateSelectSaveDataEX();
    void enterStateEndEX();
    void executeStateEndEX();
    void exitStateEndEX();
};
} }

namespace cgnoop { namespace scene_save_data {
struct SafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace save_data {
void saveDataControllerSelectLeaveCallback();
void saveDataSaveDataSelectLeaveCallback();
void saveDataNameInputLeaveCallback();
void saveDataCancelExecuteCallback();
void saveDataCancelLeaveCallback();
void saveDataCancelKeyboardLeaveCallback();
void saveDataCheckDataLeaveCallback();
void saveDataWaitEndSelectLeaveCallback();
void saveDataEndSelectExecuteCallback();
void saveDataEndSelectLeaveCallback();
struct SaveDataNameString {
    void saveDataNameStringEnsure();
};
} }

namespace cgnoop { namespace scene_opening {
struct OpeningSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace ingame_ui {
struct IngameUi {
    void ingameMenuEnter();
    void ingameSaveEnter();
    void ingameOptionEnter();
    void ingameTaktSignListEnter();
    void ingameReturnTaktForProModeEnter();
    void ingameMiiverseSettingEnter();
    void ingameMiiverseExplainEnter();
    void ingameTelescopeEnter();
    void ingamePictographBoxEnter();
    void ingameToPictographListEnter();
    void ingameReturnPictographBoxEnter();
    void ingameExitPictographListEnter();
    void ingameSelectPictographListEnter();
    void ingameToPlayExit();
    void ingamePlayExit();
    void ingameToPauseDrcExit();
    void ingameToMenuExit();
    void ingameMenuExit();
    void ingameMenuSubWindowExit();
    void ingameToSaveExit();
    void ingameToOptionExit();
    void ingameOptionExit();
    void ingameToTaktExit();
    void ingameTaktExit();
    void ingameToTaktSignListExit();
    void ingameTaktSignListExit();
    void ingameReturnTaktForProModeExit();
    void ingameTaktPlayDemoExit();
    void ingameToWarpExit();
    void ingamePhotoViewMiiverseExit();
    void ingameToMiiverseSettingExit();
    void ingameToConfirmMiiverseCommentJumpExit();
    void ingameConfirmMiiverseCommentJumpExit();
    void ingameToConfirmMiiverseProfileJumpExit();
    void ingameConfirmMiiverseProfileJumpExit();
    void ingameMiiverseProfileJumpExit();
    void ingameToMiiverseExplainExit();
    void ingameToTelescopeExit();
    void ingameTelescopeExit();
    void ingameToPictographBoxExit();
    void ingamePictographBoxExit();
    void ingameToPictographListExit();
    void ingameReturnPictographBoxExit();
    void ingameExitPictographListExit();
    void ingameToSelectPictographListExit();
    void ingameSelectPictographListExit();
    void ingameToFishManHintExit();
    void ingameDemoTauraArrivalExit();
    void ingameToDemoFishManExit();
    void ingameDemoFishManExit();
    void ingameToDemoTriforceExit();
    void ingameDemoPictBottleExit();
    void ingameGameOverExit();
    void ingameChangeToTvModeExit();
    void ingameChangeToDrcModeExit();
};
} }

namespace cgnoop { namespace scene_ingame_ui_part2 {
struct SafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace scene_ingame_ui {
struct WideString {
    void assureTermination();
};
} }
