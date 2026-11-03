/* Per-TU nullable member dispatch. Reconstructed from original instructions only. */
#include "lms/DelegateInstances.h"

/* 02708424 */
void lms::delegates::ending::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02708478 */
void lms::delegates::ending::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02708BAC */
void lms::delegates::boot::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02708C00 */
void lms::delegates::boot::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0270C534 */
void lms::delegates::scene::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0270C588 */
void lms::delegates::scene::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0270D048 */
void lms::delegates::opening::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0270D09C */
void lms::delegates::opening::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0271954C */
void lms::delegates::ingame_ui::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027195A0 */
void lms::delegates::ingame_ui::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0271EC0C */
void lms::delegates::staff_roll::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0271EC60 */
void lms::delegates::staff_roll::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0271F58C */
void lms::delegates::title::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0271F5E0 */
void lms::delegates::title::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027283EC */
void lms::delegates::color_fader::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02728440 */
void lms::delegates::color_fader::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02729058 */
void lms::delegates::fader_manager::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027290AC */
void lms::delegates::fader_manager::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02729100 */
void lms::delegates::fader_manager::DelegateC::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02729154 */
void lms::delegates::fader_manager::DelegateD::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027291A8 */
void lms::delegates::fader_manager::DelegateE::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0272B310 */
void lms::delegates::render_layer::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0272E4C4 */
void lms::delegates::layer_ui::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0272EB88 */
void lms::delegates::layer_drc::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0272F378 */
void lms::delegates::layer_drc_foreground::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0272F864 */
void lms::delegates::layer_foreground::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027337B4 */
void lms::delegates::layer_world::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02733E98 */
void lms::delegates::layer_mask::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027348C8 */
void lms::delegates::layer_drc_mode::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02734D84 */
void lms::delegates::error_viewer::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02735260 */
void lms::delegates::fader::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02735C9C */
void lms::delegates::multi_filter::Delegate::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02738054 */
void lms::delegates::layer_shadow::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027380A8 */
void lms::delegates::layer_shadow::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027380FC */
void lms::delegates::layer_shadow::DelegateC::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0273A00C */
void lms::delegates::message_reader::DelegateA::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 0273A060 */
void lms::delegates::message_reader::DelegateB::invoke() {
    be<u8>* receiver = mReceiver;
    if (receiver == nullptr) return;
    s16 index = mVirtualIndex;
    if (index == 0) return;
    be<u8>* adjusted = receiver + (s32)mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mFunction;
    } else {
        auto slot = reinterpret_cast<gptr<MethodEntry>*>(adjusted + (s32)mVtableOffset);
        MethodEntry* table = *slot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* WWHD glue
*/
