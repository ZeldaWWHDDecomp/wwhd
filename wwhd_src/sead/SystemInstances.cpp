/* WWHD per-TU library instances, reconstructed from instructions and owned game interfaces.
 * Scopes distinguish per-TU copies; unbound string instances carry no runtime ownership claim.
 * No external reference was used. */
#include "sead/SystemInstances.h"
#include "sead/ElementAt.h"
using namespace sead::instances;

/* 026DE4E4 */
void sead::system_instances::screen_item_shortcut::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026DE524 */
void sead::system_instances::screen_item_shortcut::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026DE564 */
u32 sead::system_instances::screen_item_shortcut::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026DF57C */
void sead::system_instances::screen_slide_assist_tv::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026DF5BC */
void sead::system_instances::screen_slide_assist_tv::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026DF5FC */
void sead::system_instances::screen_slide_assist_tv::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026DF63C */
u32 sead::system_instances::screen_slide_assist_tv::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026E2280 */
void sead::system_instances::screen_staff_roll::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026E22C0 */
void sead::system_instances::screen_staff_roll::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026E2300 */
void sead::system_instances::screen_staff_roll::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026E2340 */
u32 sead::system_instances::screen_staff_roll::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026E2A18 */
void sead::system_instances::screen_staff_roll_bg::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026E2A58 */
void sead::system_instances::screen_staff_roll_bg::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026E2A98 */
void sead::system_instances::screen_staff_roll_bg::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026E2AD8 */
u32 sead::system_instances::screen_staff_roll_bg::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026E45C8 */
void sead::system_instances::screen_staff_roll_bubble_base::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026E4608 */
void sead::system_instances::screen_staff_roll_bubble_base::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026E4648 */
void sead::system_instances::screen_staff_roll_bubble_base::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026E5BA8 */
void sead::system_instances::title_start_background_states::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026E5BE8 */
void sead::system_instances::title_start_background_states::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026E5C28 */
void sead::system_instances::title_start_background_states::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026E5C68 */
u32 sead::system_instances::title_start_background_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026E77A0 */
void sead::system_instances::screen_swim_time_hud::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026E77E0 */
void sead::system_instances::screen_swim_time_hud::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026E7820 */
void sead::system_instances::screen_swim_time_hud::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026E7860 */
u32 sead::system_instances::screen_swim_time_hud::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026E9240 */
void sead::system_instances::telescope_hud::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 026E92A0 */
void sead::system_instances::screen_telescope_cover::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026E92E0 */
void sead::system_instances::screen_telescope_cover::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026E9320 */
void sead::system_instances::screen_telescope_cover::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026E9360 */
u32 sead::system_instances::screen_telescope_cover::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026EA434 */
void sead::system_instances::time_display::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 026EA474 */
void sead::system_instances::screen_time::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026EA4B4 */
void sead::system_instances::screen_time::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026EA4F4 */
void sead::system_instances::screen_time::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026EA534 */
u32 sead::system_instances::screen_time::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026EBE08 */
void sead::system_instances::title_logo_states::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026EBE48 */
void sead::system_instances::title_logo_states::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026EBE88 */
void sead::system_instances::title_logo_states::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026EBEC8 */
u32 sead::system_instances::title_logo_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026ED73C */
void sead::system_instances::screen_tre_map::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026ED77C */
void sead::system_instances::screen_tre_map::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026ED7BC */
void sead::system_instances::screen_tre_map::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026ED7FC */
u32 sead::system_instances::screen_tre_map::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026EEA84 */
void sead::system_instances::unread_miiverse::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 026EEAC8 */
void sead::system_instances::screen_unread_miiverse::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026EEB08 */
void sead::system_instances::screen_unread_miiverse::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026EEB48 */
void sead::system_instances::screen_unread_miiverse::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026EEB88 */
u32 sead::system_instances::screen_unread_miiverse::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026EF128 */
void sead::system_instances::screen_warp_area::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026EF168 */
void sead::system_instances::screen_warp_area::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026EF1A8 */
void sead::system_instances::screen_warp_area::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026EF1E8 */
u32 sead::system_instances::screen_warp_area::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026F0AA0 */
void sead::system_instances::screen_warp_map::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026F0AE0 */
void sead::system_instances::screen_warp_map::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026F0B20 */
void sead::system_instances::screen_warp_map::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026F0B60 */
u32 sead::system_instances::screen_warp_map::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026F2FE8 */
void sead::system_instances::parts_basic_button::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026F3028 */
void sead::system_instances::parts_basic_button::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026F3068 */
void sead::system_instances::parts_basic_button::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026F30A8 */
u32 sead::system_instances::parts_basic_button::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026F5714 */
void sead::system_instances::parts_window::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 026F5744 */
void sead::system_instances::parts_window::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 026F6B74 */
void sead::system_instances::detached_text::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 026F7434 */
void sead::system_instances::parts_heart::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026F7474 */
void sead::system_instances::parts_heart::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026F74B4 */
void sead::system_instances::parts_heart::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026F74F4 */
u32 sead::system_instances::parts_heart::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026F7CC0 */
void sead::system_instances::parts_in_out_loop::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026F7D00 */
void sead::system_instances::parts_in_out_loop::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026F7D40 */
void sead::system_instances::parts_in_out_loop::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026F7D80 */
u32 sead::system_instances::parts_in_out_loop::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026F8804 */
void sead::system_instances::parts_key_icon::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026F8844 */
void sead::system_instances::parts_key_icon::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026F8884 */
void sead::system_instances::parts_key_icon::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026F88C4 */
u32 sead::system_instances::parts_key_icon::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026FA198 */
void sead::system_instances::parts::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 026FA1D8 */
void sead::system_instances::parts::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 026FA218 */
void sead::system_instances::parts::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 026FA258 */
u32 sead::system_instances::parts::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026FB218 */
void sead::system_instances::parts_layout::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 026FC050 */
void sead::system_instances::layout_resource_manager::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 026FC0CC */
void* sead::system_instances::layout_resource_manager::Delegate::invoke() {
    void* object = mObject;
    if (object == NULL) return this;
    s16 index = mCallback.mVirtualIndex;
    if (index == 0) return this;
    void* adjusted = static_cast<u8*>(object) + (s32)mCallback.mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mCallback.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mCallback.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    return gabi::tail_ptr<void*>(target, adjusted);
}

/* 0270081C */
void sead::system_instances::layout_wait_screen::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 02700984 */
void sead::system_instances::msg_window_states::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 027009C4 */
void sead::system_instances::msg_window_states::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 02700A04 */
void sead::system_instances::msg_window_states::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 02700A44 */
u32 sead::system_instances::msg_window_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02702488 */
void sead::system_instances::no_decide_button::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 027024C8 */
void sead::system_instances::no_decide_button::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 02702508 */
void sead::system_instances::no_decide_button::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 02702548 */
u32 sead::system_instances::no_decide_button::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02704BB0 */
void sead::system_instances::copy_text_pane::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 02704BE0 */
void sead::system_instances::parts_text_pane::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 02705150 */
void sead::system_instances::unbound_text::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 027079C0 */
void sead::system_instances::parts_valid::StateId::enter(void* object) {
    s16 index = mEnter.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mEnter.mThisAdjustment;
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

/* 02707A00 */
void sead::system_instances::parts_valid::StateId::execute(void* object) {
    s16 index = mExecute.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mExecute.mThisAdjustment;
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

/* 02707A40 */
void sead::system_instances::parts_valid::StateId::leave(void* object) {
    s16 index = mLeave.mVirtualIndex;
    void* adjusted = static_cast<u8*>(object) + (s32)mLeave.mThisAdjustment;
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

/* 02707A80 */
u32 sead::system_instances::parts_valid::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02707B64 */
void sead::system_instances::detached_wide_text::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* WWHD glue
*/
