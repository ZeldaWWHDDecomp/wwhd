/** Receivers of the empty virtual functions whose contracts come from their vtables (CG-empty#2, second
 * batch). Opaque: none of these bodies reads or writes a field, so no layout is implied.
 *
 * StateIdBase: the default enter/execute/leave of a TU's state ID base class. Its vtable (the one right
 * before the TU's installed state ID vtable, same 9-slot shape: pure-virtual placeholders at +0x0C/+0x1C/
 * +0x3C/+0x44, id getter `lwz r3,0(r3)` at +0x14) is never installed. The installed state ID overrides
 * these slots (+0x24/+0x2C/+0x34) with the delegate forwarders that call the owner's member function
 * (owner in r4, unadjusted); the state machine (e.g. hdPartsState_c::enter/execute/exit) calls the slots
 * with r3 = the state ID and r4 = the owner and reads no result. Same contract as CG-empty#1.
 *
 * SafeStringBase: the termination hook (slot +0x14) of a TU's abstract string base; its vtable has a
 * pure-virtual destructor slot and is never installed. The plain string classes (LayoutStringValue,
 * TextStringBase, StringView, WideNameView, LayerStringBase) have a real deleting destructor in slot
 * +0x0C but their vtables are never installed either; BootSupportTypeString is the static string the boot
 * static initializer builds. Every string termination dispatcher calls slot +0x14 with the string only.
 *
 * Layer hooks (agl render layers): drawMethodBegin/drawMethodEnd are slots +0x54/+0x5C, which the layer
 * draw-group loop (027C25AC, reached from the renderer through 027C424C) calls around each draw method of
 * the group with r4 = the render info and r5 = the draw method; prepareRenderList is slot +0x84, which the
 * renderer's list builder (027D72B8) calls with the layer only before it queues the layer.
 *
 * EntryHook::enterIn: slot +0xFC of the hdSeaChartLinkPos_c-derived chart screens; hdSeaChartLinkPos_c::
 * enterIn calls it last, with the chart only.
 *
 * StateReceiver / TitleControllerSelection state phases: the state is registered with a virtual index n
 * (enter n, execute n+1, exit n+2; slot n sits at vtable +8n+4), the receiver only, as their merged
 * siblings. MessageWindow::executeState is the parts' executeState slot (+0x5C, hdPartsVtbl_c, called with
 * the parts only). Receiver::onStop (slot +0x45C) is called by hdLayoutWaitScreen_c::reset with the window
 * only, result unused. */
#pragma once
#include "bindings.h"
#include "cg/empty_2_6.h"


namespace cgnoop { namespace msg_event_position {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace msg_window_event_frame {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace msg_only2 {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace msg_window_scroll_layout {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace no_operate_drc {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace pic_msg {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace pause {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace pictograph_camera {
struct SafeStringBase {
    void assureTermination();
};
} }

namespace cgnoop { namespace pictograph_camera_resources {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace pictograph_box_resources {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace pictograph_list {
struct SafeStringBase {
    void assureTermination();
};
} }

namespace cgnoop { namespace pictograph_list_resources {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace menu_window_callbacks {
struct StateReceiver {
    void enterStateWait();
    void exitStateWait();
    void exitStateInAll();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace rupy {
struct SafeStringBase {
    void assureTermination();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace rupy_sword_counter_2 {
struct SafeStringBase {
    void assureTermination();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace secret_entrance_chart {
struct EntryHook {
    void enterIn();
};
} }

namespace cgnoop { namespace title_controller_screen {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace sequence_option {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace system_window {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace item_shortcut {
struct SafeStringBase {
    void assureTermination();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace slide_assist_tv {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace squid_chart {
struct EntryHook {
    void enterIn();
};
} }

namespace cgnoop { namespace staff_roll {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace staff_roll_bg {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace staff_roll_bubble_base {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace title_start_background_states {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace submarine_chart {
struct EntryHook {
    void enterIn();
};
} }

namespace cgnoop { namespace swim_time_hud {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace telescope_hud {
struct SafeStringBase {
    void assureTermination();
};
} }

namespace cgnoop { namespace telescope_cover {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace terry_chart {
struct EntryHook {
    void enterIn();
};
} }

namespace cgnoop { namespace time {
struct SafeStringBase {
    void assureTermination();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace tingle_tower_chart {
struct EntryHook {
    void enterIn();
};
} }

namespace cgnoop { namespace title_logo_states {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace tre_map {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace triforce_chart {
struct EntryHook {
    void enterIn();
};
} }

namespace cgnoop { namespace unread_miiverse {
struct SafeStringBase {
    void assureTermination();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace warp_area {
struct StateReceiver {
    void exitStateLoop();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace warp_map {
struct StateReceiver {
    void exitStateAreaNameOut();
    void exitStateAreaNameIn();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace parts_basic_button {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace parts_heart {
struct StateReceiver {
    void enterStateWait();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace parts_in_out_loop {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace parts_key_icon {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace parts {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace parts_layout {
struct LayoutStringValue {
    void assureTermination();
};
} }

namespace cgnoop { namespace msg_window_states {
struct MessageWindow {
    void executeState();
};
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace no_decide_button {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace parts_text_pane {
struct TextStringBase {
    void assureTermination();
};
} }

namespace cgnoop { namespace parts_valid {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace scene_boot_common {
struct BootSupportTypeString {
    void assureTermination();
};
} }

namespace cgnoop { namespace scene_ex_save {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace save_data {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace ingame_ui {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace scene_save_end {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace scene_staff_roll {
struct StateIdBase {
    void enter(void* owner);
    void execute(void* owner);
    void leave(void* owner);
};
} }

namespace cgnoop { namespace picture_manager {
struct StringView {
    void assureTermination();
};
} }

namespace cgnoop { namespace player_name {
struct WideNameView {
    void assureTermination();
};
struct SafeStringBase {
    void assureTermination();
};
} }

namespace cgnoop { namespace render_misc {
struct SafeStringBase {
    void assureTermination();
};
} }

namespace cgnoop { namespace layer_base {
struct LayerStringBase {
    void assureTermination();
};
} }
