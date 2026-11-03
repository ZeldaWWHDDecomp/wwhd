/** Opaque receiver roles for empty game hooks. No complete object layout is implied. */
#pragma once
#include "bindings.h"
#include "cg/empty_2_5.h"

namespace cgnoop { namespace ingame_ui {
} }

namespace cgnoop { namespace scene_main_screens {
struct MainScreenRegistrationString {
    void assureTermination();
};
} }

namespace cgnoop { namespace sys {
struct Receiver {
    void hdSystemInitializeFontManager();
};
} }

namespace cgnoop { namespace scene_save_end {
struct StateReceiver {
    void exitStateSaveInit();
    void enterStateSaveEnd();
    void executeStateSaveEnd();
    void exitStateSaveEnd();
};
} }

namespace cgnoop { namespace staff_roll {
struct SafeString {
    void staffSafeStringValidate();
};
} }

namespace cgnoop { namespace scene_staff_roll {
struct StateReceiver {
    void exitStateLoadBgm();
    void enterStateStartWait();
    void exitStateStartWait();
    void exitStateFadeOutIn();
    void exitStateCalc();
    void exitStateEnd();
};
} }

namespace cgnoop { namespace scene_title {
struct L_titleSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace save_play_log {
struct SeaPosition;
struct Receiver {
    cgnoop::save_play_log::SeaPosition* seaPositionView();
};
} }

namespace cgnoop { namespace picture_accessor {
struct AccessorSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace render_misc {
struct MiscSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace color_fader {
struct ColorListenerInterface {
    void assureTermination();
};
} }

namespace cgnoop { namespace fader_mgr {
struct ManagerListenerInterface {
    void assureTermination();
};
} }

namespace cgnoop { namespace m_Do_graphic {
struct MDoGph_DisplayHD {
    void mDoGph_applyFade();
};
} }

namespace cgnoop { namespace graphics_task {
void graphicsEmptyCallback();
void initializeGraphicsSupportService(void* receiver, void* context);
struct GraphicsName {
    void assureTermination();
};
} }

namespace cgnoop { namespace render_layers {
struct ShaderLocation {
    void assureTermination();
};
} }

namespace cgnoop { namespace layer_2d {
struct UIName {
    void assureTermination();
};
struct LayerTwoDBase {
    void initialize();
    void calculate();
    void draw(void* info);
    void finishDraw();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
struct UI {
    void calculate();
};
} }

namespace cgnoop { namespace layer_2d_drc {
struct HdLayerTwoDDRC_cName {
    void assureTermination();
};
struct LayerTwoDDRC {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace layer_2d_drc_fore {
struct DRCForeName {
    void assureTermination();
};
struct DRCFore {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace layer_2d_fore {
struct HdLayerTwoDFore_cName {
    void assureTermination();
};
struct LayerTwoDFore {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace layer_3d {
struct LayerThreedSafeString {
    void assureTermination();
};
struct LayerThreedLayer {
    void draw(void* info);
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace layer_mask {
struct LayerMaskSafeString {
    void assureTermination();
};
struct LayerMaskLayer {
    void draw(void* info);
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace layer_base {
struct LayerBaseLayer {
    void initialize();
    void calculate();
    void draw(void* info);
    void finishDraw();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace layer_drc_mode {
struct LayerDrcModeSafeString {
    void assureTermination();
};
struct LayerDrcModeLayer {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace error_viewer {
struct ErrorViewerSafeString {
    void assureTermination();
};
struct ErrorViewerLayer {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace fader {
struct String {
    void assureTermination();
};
struct Layer {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace multi_filter {
struct MultiFilterSafeString {
    void assureTermination();
};
struct MultiFilterLayer {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop {
struct ShadowSourceString {
    void assureTermination();
};
}

namespace cgnoop { namespace layer_shadow {
struct LayerShadowLayer {
    void draw(void* info);
    void finishDraw();
    void calculate();
    void drawMethodBegin(void* info, void* drawMethod);
    void drawMethodEnd(void* info, void* drawMethod);
    void prepareRenderList();
};
} }

namespace cgnoop { namespace resource_tool_paths {
struct ResourceToolPathsSafeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace shader_archive {
struct ShaderArchiveSafeString {
    void assureTermination();
};
} }
