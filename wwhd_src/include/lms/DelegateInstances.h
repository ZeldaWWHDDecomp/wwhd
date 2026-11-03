/* Member-function delegates: {vtable, receiver, member-function pointer}. Every translation unit
 * that installs one keeps its own out-of-line copy of invoke() (GHS per-TU template copies), so the
 * same body appears once per class below, grouped by the installing unit.
 *
 * The member-function pointer is GHS's: a this-adjustment, an index (0 = null, negative = plain
 * function in mFunction, positive = virtual slot; then mVtableOffset locates the vtable pointer
 * inside the adjusted receiver) and the function or vtable offset. invoke() does nothing for a
 * null receiver or a null pointer and tail-calls the target otherwise.
 * Layouts reconstructed from this game's instructions only; no external reference used. */
#pragma once
#include "wwhd.h"
namespace lms { namespace delegates {
struct MethodEntry { be<u32> mAdjustment; be<u32> mFunction; };
struct DelegateLayout {
    be<u32> mVtable;
    gptr<be<u8>> mReceiver;
    be<s16> mThisAdjustment;
    be<s16> mVirtualIndex;
    union { be<u32> mFunction; struct { be<u16> mReserved; be<s16> mVtableOffset; }; };
};
WWHD_OFFSET(DelegateLayout, mReceiver, 4);
WWHD_OFFSET(DelegateLayout, mThisAdjustment, 8);
WWHD_OFFSET(DelegateLayout, mVirtualIndex, 10);
WWHD_OFFSET(DelegateLayout, mFunction, 12);
WWHD_OFFSET(DelegateLayout, mVtableOffset, 14);
WWHD_SIZE(DelegateLayout, 16);
namespace ending {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace boot {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace scene {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace opening {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace ingame_ui {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace staff_roll {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace title {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace color_fader {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
namespace fader_manager {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
struct DelegateC : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateC, 16);
struct DelegateD : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateD, 16);
struct DelegateE : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateE, 16);
}
namespace render_layer {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_ui {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_drc {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_drc_foreground {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_foreground {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_world {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_mask {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_drc_mode {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace error_viewer {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace fader {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace multi_filter {
struct Delegate : DelegateLayout { void invoke(); };
WWHD_SIZE(Delegate, 16);
}
namespace layer_shadow {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
struct DelegateC : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateC, 16);
}
namespace message_reader {
struct DelegateA : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateA, 16);
struct DelegateB : DelegateLayout { void invoke(); };
WWHD_SIZE(DelegateB, 16);
}
}}
