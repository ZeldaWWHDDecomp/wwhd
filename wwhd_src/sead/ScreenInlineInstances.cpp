/* Per-translation-unit UI state dispatch and bounded text termination.
 * Reconstructed from original game instructions; no external references used. */
#include "sead/ScreenInlineInstances.h"
#include "sead/ElementAt.h"

using sead::instances::MethodEntry;
using sead::instances::StateIdLayout;

/* 02667828 */
void sead::screen_instances::screen_e3_select_course::StateId::enter(void* object) {
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

/* 02667868 */
void sead::screen_instances::screen_e3_select_course::StateId::execute(void* object) {
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

/* 026678A8 */
void sead::screen_instances::screen_e3_select_course::StateId::leave(void* object) {
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

/* 026678E8 */
u32 sead::screen_instances::screen_e3_select_course::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02667C18 */
void sead::screen_instances::screen_e3_thank_you::StateId::enter(void* object) {
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

/* 02667C58 */
void sead::screen_instances::screen_e3_thank_you::StateId::execute(void* object) {
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

/* 02667C98 */
void sead::screen_instances::screen_e3_thank_you::StateId::leave(void* object) {
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

/* 02667CD8 */
u32 sead::screen_instances::screen_e3_thank_you::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026682E8 */
void sead::screen_instances::screen_ending_end::StateId::enter(void* object) {
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

/* 02668328 */
void sead::screen_instances::screen_ending_end::StateId::execute(void* object) {
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

/* 02668368 */
void sead::screen_instances::screen_ending_end::StateId::leave(void* object) {
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

/* 026683A8 */
u32 sead::screen_instances::screen_ending_end::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02668AC0 */
void sead::screen_instances::screen_enemy_hp::StateId::enter(void* object) {
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

/* 02668B00 */
void sead::screen_instances::screen_enemy_hp::StateId::execute(void* object) {
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

/* 02668B40 */
void sead::screen_instances::screen_enemy_hp::StateId::leave(void* object) {
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

/* 02668B80 */
u32 sead::screen_instances::screen_enemy_hp::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02669AAC */
void sead::screen_instances::screen_fade_wipe::StateId::enter(void* object) {
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

/* 02669AEC */
void sead::screen_instances::screen_fade_wipe::StateId::execute(void* object) {
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

/* 02669B2C */
void sead::screen_instances::screen_fade_wipe::StateId::leave(void* object) {
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

/* 02669B6C */
u32 sead::screen_instances::screen_fade_wipe::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0266A544 */
void sead::screen_instances::screen_fade_wipe_iris::StateId::enter(void* object) {
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

/* 0266A584 */
void sead::screen_instances::screen_fade_wipe_iris::StateId::execute(void* object) {
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

/* 0266A5C4 */
void sead::screen_instances::screen_fade_wipe_iris::StateId::leave(void* object) {
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

/* 0266A604 */
u32 sead::screen_instances::screen_fade_wipe_iris::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0266B724 */
void sead::screen_instances::detached_char_buffer::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 0266B77C */
void sead::screen_instances::screen_game_over::StateId::enter(void* object) {
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

/* 0266B7BC */
void sead::screen_instances::screen_game_over::StateId::execute(void* object) {
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

/* 0266B7FC */
void sead::screen_instances::screen_game_over::StateId::leave(void* object) {
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

/* 0266B83C */
u32 sead::screen_instances::screen_game_over::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0266C484 */
void sead::screen_instances::screen_game_over_char::StateId::enter(void* object) {
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

/* 0266C4C4 */
void sead::screen_instances::screen_game_over_char::StateId::execute(void* object) {
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

/* 0266C504 */
void sead::screen_instances::screen_game_over_char::StateId::leave(void* object) {
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

/* 0266E644 */
void sead::screen_instances::screen_heart_states::StateId::enter(void* object) {
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

/* 0266E684 */
void sead::screen_instances::screen_heart_states::StateId::execute(void* object) {
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

/* 0266E6C4 */
void sead::screen_instances::screen_heart_states::StateId::leave(void* object) {
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

/* 0266E704 */
u32 sead::screen_instances::screen_heart_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02670CB0 */
void sead::screen_instances::screen_icon_slide_area::StateId::enter(void* object) {
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

/* 02670CF0 */
void sead::screen_instances::screen_icon_slide_area::StateId::execute(void* object) {
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

/* 02670D30 */
void sead::screen_instances::screen_icon_slide_area::StateId::leave(void* object) {
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

/* 02670D70 */
u32 sead::screen_instances::screen_icon_slide_area::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02671B08 */
void sead::screen_instances::screen_icon_slide_area_mirror::StateId::enter(void* object) {
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

/* 02671B48 */
void sead::screen_instances::screen_icon_slide_area_mirror::StateId::execute(void* object) {
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

/* 02671B88 */
void sead::screen_instances::screen_icon_slide_area_mirror::StateId::leave(void* object) {
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

/* 02671BC8 */
u32 sead::screen_instances::screen_icon_slide_area_mirror::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02672340 */
void sead::screen_instances::screen_information::StateId::enter(void* object) {
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

/* 02672380 */
void sead::screen_instances::screen_information::StateId::execute(void* object) {
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

/* 026723C0 */
void sead::screen_instances::screen_information::StateId::leave(void* object) {
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

/* 02672400 */
u32 sead::screen_instances::screen_information::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02672BA8 */
void sead::screen_instances::screen_information_drc::StateId::enter(void* object) {
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

/* 02672BE8 */
void sead::screen_instances::screen_information_drc::StateId::execute(void* object) {
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

/* 02672C28 */
void sead::screen_instances::screen_information_drc::StateId::leave(void* object) {
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

/* 02672C68 */
u32 sead::screen_instances::screen_information_drc::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02673940 */
void sead::screen_instances::keyboard_text_area::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 02673978 */
void sead::screen_instances::keyboard_text_area::StateId::enter(void* object) {
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

/* 026739B8 */
void sead::screen_instances::keyboard_text_area::StateId::execute(void* object) {
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

/* 026739F8 */
void sead::screen_instances::keyboard_text_area::StateId::leave(void* object) {
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

/* 02673A38 */
u32 sead::screen_instances::keyboard_text_area::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0267400C */
void sead::screen_instances::keyboard_cursor::StateId::enter(void* object) {
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

/* 0267404C */
void sead::screen_instances::keyboard_cursor::StateId::execute(void* object) {
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

/* 0267408C */
void sead::screen_instances::keyboard_cursor::StateId::leave(void* object) {
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

/* 026740CC */
u32 sead::screen_instances::keyboard_cursor::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02674C98 */
void sead::screen_instances::screen_loop_boss_icon_states::StateId::enter(void* object) {
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

/* 02674CD8 */
void sead::screen_instances::screen_loop_boss_icon_states::StateId::execute(void* object) {
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

/* 02674D18 */
void sead::screen_instances::screen_loop_boss_icon_states::StateId::leave(void* object) {
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

/* 02674D58 */
u32 sead::screen_instances::screen_loop_boss_icon_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026759A0 */
void sead::screen_instances::screen_magic_bar::StateId::enter(void* object) {
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

/* 026759E0 */
void sead::screen_instances::screen_magic_bar::StateId::execute(void* object) {
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

/* 02675A20 */
void sead::screen_instances::screen_magic_bar::StateId::leave(void* object) {
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

/* 02675A60 */
u32 sead::screen_instances::screen_magic_bar::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0267B558 */
void sead::screen_instances::screen_sea_chart_menu::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 0267F314 */
void sead::screen_instances::screen_sea_chart_map::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 0267F3B0 */
void sead::screen_instances::screen_sea_chart_map::StateId::enter(void* object) {
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

/* 0267F3F0 */
void sead::screen_instances::screen_sea_chart_map::StateId::execute(void* object) {
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

/* 0267F430 */
void sead::screen_instances::screen_sea_chart_map::StateId::leave(void* object) {
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

/* 0267F470 */
u32 sead::screen_instances::screen_sea_chart_map::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02684DA8 */
void sead::screen_instances::sea_chart_link_pos::StateId::enter(void* object) {
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

/* 02684DE8 */
void sead::screen_instances::sea_chart_link_pos::StateId::execute(void* object) {
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

/* 02684E28 */
void sead::screen_instances::sea_chart_link_pos::StateId::leave(void* object) {
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

/* 02684E68 */
u32 sead::screen_instances::sea_chart_link_pos::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02685B98 */
void sead::screen_instances::sea_chart_state_handlers::StateId::enter(void* object) {
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

/* 02685BD8 */
void sead::screen_instances::sea_chart_state_handlers::StateId::execute(void* object) {
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

/* 02685C18 */
void sead::screen_instances::sea_chart_state_handlers::StateId::leave(void* object) {
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

/* 02685C58 */
u32 sead::screen_instances::sea_chart_state_handlers::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0268AF10 */
void sead::screen_instances::screen_map_display_coordinator::StateId::enter(void* object) {
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

/* 0268AF50 */
void sead::screen_instances::screen_map_display_coordinator::StateId::execute(void* object) {
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

/* 0268AF90 */
void sead::screen_instances::screen_map_display_coordinator::StateId::leave(void* object) {
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

/* 0268E7AC */
void sead::screen_instances::screen_menu_btns::StateId::enter(void* object) {
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

/* 0268E7EC */
void sead::screen_instances::screen_menu_btns::StateId::execute(void* object) {
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

/* 0268E82C */
void sead::screen_instances::screen_menu_btns::StateId::leave(void* object) {
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

/* 0268E86C */
u32 sead::screen_instances::screen_menu_btns::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02698568 */
void sead::screen_instances::screen_menu_main::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 026985D8 */
void sead::screen_instances::screen_menu_main::StateId::enter(void* object) {
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

/* 02698618 */
void sead::screen_instances::screen_menu_main::StateId::execute(void* object) {
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

/* 02698658 */
void sead::screen_instances::screen_menu_main::StateId::leave(void* object) {
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

/* 02698698 */
u32 sead::screen_instances::screen_menu_main::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0269EE58 */
void sead::screen_instances::detached_wide_buffer::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* 0269EFB8 */
void sead::screen_instances::screen_menu_map_states::StateId::enter(void* object) {
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
