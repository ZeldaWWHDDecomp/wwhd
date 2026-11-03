/* UI state-ID dispatch and wide-buffer termination instances.
 * Reconstructed from original game instructions and our merged canonical layouts.
 * No external reference used. */
#include "sead/MenuInlineInstances.h"
#include "sead/ElementAt.h"

using sead::instances::MemberFunction;
using sead::instances::MethodEntry;
using sead::instances::StateIdLayout;

/* 0269EFF8 */
void sead::menu_instances::menu_map_states::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 0269F038 */
void sead::menu_instances::menu_map_states::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 0269F078 */
u32 sead::menu_instances::menu_map_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026A682C */
void sead::menu_instances::menu_miiverse::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026A686C */
void sead::menu_instances::menu_miiverse::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026A68AC */
void sead::menu_instances::menu_miiverse::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026A68EC */
u32 sead::menu_instances::menu_miiverse::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026A9CD8 */
void sead::menu_instances::menu_miiverse_comment::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026A9D18 */
void sead::menu_instances::menu_miiverse_comment::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026A9D58 */
void sead::menu_instances::menu_miiverse_comment::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AAFD0 */
void sead::menu_instances::menu_miiverse_new_comment::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AB010 */
void sead::menu_instances::menu_miiverse_new_comment::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AB050 */
void sead::menu_instances::menu_miiverse_new_comment::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AC104 */
void sead::menu_instances::menu_miiverse_setting::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AC144 */
void sead::menu_instances::menu_miiverse_setting::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AC184 */
void sead::menu_instances::menu_miiverse_setting::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AE924 */
void sead::menu_instances::msg_choice_control::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}
/* 026AE9C0 */
void sead::menu_instances::msg_choice_control::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AEA00 */
void sead::menu_instances::msg_choice_control::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AEA40 */
void sead::menu_instances::msg_choice_control::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026AEA80 */
u32 sead::menu_instances::msg_choice_control::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026B15E0 */
void sead::menu_instances::msg_only_window::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}
/* 026B1674 */
void sead::menu_instances::msg_only_control::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B16B4 */
void sead::menu_instances::msg_only_control::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B16F4 */
void sead::menu_instances::msg_only_control::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B1734 */
u32 sead::menu_instances::msg_only_control::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026B5458 */
void sead::menu_instances::msg_main_window::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}
/* 026B54F0 */
void sead::menu_instances::msg_event_position::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B5530 */
void sead::menu_instances::msg_event_position::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B5570 */
void sead::menu_instances::msg_event_position::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B55B0 */
u32 sead::menu_instances::msg_event_position::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026B5AB4 */
void sead::menu_instances::msg_window_event_frame::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B5AF4 */
void sead::menu_instances::msg_window_event_frame::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B5B34 */
void sead::menu_instances::msg_window_event_frame::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B5B74 */
u32 sead::menu_instances::msg_window_event_frame::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026B9824 */
void sead::menu_instances::msg_only2::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}
/* 026B98BC */
void sead::menu_instances::msg_only2::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B98FC */
void sead::menu_instances::msg_only2::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B993C */
void sead::menu_instances::msg_only2::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026B997C */
u32 sead::menu_instances::msg_only2::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026BD8E4 */
void sead::menu_instances::msg_window_scroll_layout::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026BD924 */
void sead::menu_instances::msg_window_scroll_layout::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026BD964 */
void sead::menu_instances::msg_window_scroll_layout::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026BD9A4 */
u32 sead::menu_instances::msg_window_scroll_layout::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026BE45C */
void sead::menu_instances::no_operate_drc::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026BE49C */
void sead::menu_instances::no_operate_drc::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026BE4DC */
void sead::menu_instances::no_operate_drc::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026BE51C */
u32 sead::menu_instances::no_operate_drc::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026C0180 */
void sead::menu_instances::pic_msg::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C01C0 */
void sead::menu_instances::pic_msg::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C0200 */
void sead::menu_instances::pic_msg::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C0240 */
u32 sead::menu_instances::pic_msg::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026C0F90 */
void sead::menu_instances::pause::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C0FD0 */
void sead::menu_instances::pause::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C1010 */
void sead::menu_instances::pause::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C1050 */
u32 sead::menu_instances::pause::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026C39C0 */
void sead::menu_instances::pictograph_camera::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 026C3A20 */
void sead::menu_instances::pictograph_camera_resources::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C3A60 */
void sead::menu_instances::pictograph_camera_resources::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C3AA0 */
void sead::menu_instances::pictograph_camera_resources::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C3AE0 */
u32 sead::menu_instances::pictograph_camera_resources::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026C5808 */
void sead::menu_instances::pictograph_box_resources::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C5848 */
void sead::menu_instances::pictograph_box_resources::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C5888 */
void sead::menu_instances::pictograph_box_resources::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C58C8 */
u32 sead::menu_instances::pictograph_box_resources::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026C962C */
void sead::menu_instances::pictograph_list::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 026C96F8 */
void sead::menu_instances::pictograph_list_resources::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C9738 */
void sead::menu_instances::pictograph_list_resources::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C9778 */
void sead::menu_instances::pictograph_list_resources::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026C97B8 */
u32 sead::menu_instances::pictograph_list_resources::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026CA3A0 */
void sead::menu_instances::menu_window::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CA3E0 */
void sead::menu_instances::menu_window::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CA420 */
void sead::menu_instances::menu_window::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CA460 */
u32 sead::menu_instances::menu_window::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026CB124 */
void sead::menu_instances::rupy::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 026CB160 */
void sead::menu_instances::rupy::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CB1A0 */
void sead::menu_instances::rupy::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CB1E0 */
void sead::menu_instances::rupy::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CB220 */
u32 sead::menu_instances::rupy::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026CBD8C */
void sead::menu_instances::rupy_sword_counter::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 026CBDD0 */
void sead::menu_instances::rupy_sword_counter_2::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CBE10 */
void sead::menu_instances::rupy_sword_counter_2::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CBE50 */
void sead::menu_instances::rupy_sword_counter_2::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026CBE90 */
u32 sead::menu_instances::rupy_sword_counter_2::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026D57E0 */
void sead::menu_instances::title_controller_screen::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026D5820 */
void sead::menu_instances::title_controller_screen::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026D5860 */
void sead::menu_instances::title_controller_screen::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026D58A0 */
u32 sead::menu_instances::title_controller_screen::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026D95B0 */
void sead::menu_instances::sequence_option::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026D95F0 */
void sead::menu_instances::sequence_option::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026D9630 */
void sead::menu_instances::sequence_option::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026D9670 */
u32 sead::menu_instances::sequence_option::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026DD378 */
void sead::menu_instances::system_window::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026DD3B8 */
void sead::menu_instances::system_window::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mExecute.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mExecute.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026DD3F8 */
void sead::menu_instances::system_window::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mLeave.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mLeave.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}

/* 026DD438 */
u32 sead::menu_instances::system_window::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026DE45C */
void sead::menu_instances::item_shortcut::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 026DE4A4 */
void sead::menu_instances::item_shortcut::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = (static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment);
    u32 target;
    if (index < 0) {
        target = mEnter.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mEnter.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::tail_ptr<void>(target, adjusted, object);
}


/* WWHD glue
*/
