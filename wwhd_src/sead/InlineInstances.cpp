/* sead buffered strings, delegates and state-ID instances in the HD translation units.
 * These per-TU copies retain their own entry points and the Wii U member-function ABI.
 * reconstructed from the game's instructions; no external reference code used. */
#include "sead/InlineInstances.h"
#include "sead/Resource.h"
#include "sead/ElementAt.h"

using namespace sead::instances;
extern void releaseObject(void*);
static sead::DisposerVtable sDirectFactoryBaseDisposerVtable;
static u32 sPackTypeTable;
static u32 sDirectResourceTypeTable;
static u32 sResourceTypeTable;
static u32 sPackTypeGuard;
static u32 sDirectResourceTypeGuard;
static u32 sResourceTypeGuard;
static sead::RuntimeType sPackType;
static sead::RuntimeType sDirectResourceType;
static sead::RuntimeType sResourceType;

/* 025F3884 */
void sead::instances::font_mgr::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 025F641C */
void sead::instances::msg_res_mgr::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 025F73B0 */
void sead::instances::msg_text::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 025F73E0 */
void sead::instances::msg_text::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}


/* 025F9EFC */
void* sead::instances::state_task::Delegate::invoke() {
    void* object = mObject;
    if (object == NULL) return this;
    s16 index = mCallback.mVirtualIndex;
    if (index == 0) return this;
    void* adjusted = (static_cast<u8*>(object) + (s32)mCallback.mThisAdjustment);
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


/* 025F9F64 */
void sead::instances::state_task::StateId::enter(void* object) {
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

/* 025F9FA4 */
void sead::instances::state_task::StateId::execute(void* object) {
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

/* 025F9FE4 */
void sead::instances::state_task::StateId::leave(void* object) {
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

/* 025FA024 */
u32 sead::instances::state_task::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02601AB0 */
void sead::instances::text_unit_proc::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 02601AE0 */
void sead::instances::text_unit_proc::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}


/* 026025B0 */
void sead::instances::text_ruby::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}


/* 0260329C */
void sead::instances::res_model::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 0260F8C0 */
void sead::instances::res_load_task::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 02613970 */
void sead::instances::res_mgr::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 02613DE4 */
void* sead::instances::res_mgr::Delegate::invoke() {
    void* object = mObject;
    if (object == NULL) return this;
    s16 index = mCallback.mVirtualIndex;
    if (index == 0) return this;
    void* adjusted = (static_cast<u8*>(object) + (s32)mCallback.mThisAdjustment);
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


/* 0261BB1C */
void sead::instances::screen_arrow::StateId::enter(void* object) {
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

/* 0261BB5C */
void sead::instances::screen_arrow::StateId::execute(void* object) {
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

/* 0261BB9C */
void sead::instances::screen_arrow::StateId::leave(void* object) {
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

/* 0261BBDC */
u32 sead::instances::screen_arrow::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0261CF9C */
void sead::instances::screen_auction_rupy::StateId::enter(void* object) {
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

/* 0261CFDC */
void sead::instances::screen_auction_rupy::StateId::execute(void* object) {
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

/* 0261D01C */
void sead::instances::screen_auction_rupy::StateId::leave(void* object) {
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

/* 0261D05C */
u32 sead::instances::screen_auction_rupy::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0261D7E4 */
void sead::instances::screen_auction_number::StateId::enter(void* object) {
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

/* 0261D824 */
void sead::instances::screen_auction_number::StateId::execute(void* object) {
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

/* 0261D864 */
void sead::instances::screen_auction_number::StateId::leave(void* object) {
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

/* 0261D8A4 */
u32 sead::instances::screen_auction_number::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02620FB8 */
void sead::instances::screen_baton_sign::StateId::enter(void* object) {
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

/* 02620FF8 */
void sead::instances::screen_baton_sign::StateId::execute(void* object) {
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

/* 02621038 */
void sead::instances::screen_baton_sign::StateId::leave(void* object) {
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

/* 02621078 */
u32 sead::instances::screen_baton_sign::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02621788 */
void sead::instances::screen_baton_sign_for_window::StateId::enter(void* object) {
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

/* 026217C8 */
void sead::instances::screen_baton_sign_for_window::StateId::execute(void* object) {
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

/* 02621808 */
void sead::instances::screen_baton_sign_for_window::StateId::leave(void* object) {
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

/* 02621848 */
u32 sead::instances::screen_baton_sign_for_window::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 026230B8 */
void sead::instances::screen_baton_sign_list_states::StateId::enter(void* object) {
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

/* 026230F8 */
void sead::instances::screen_baton_sign_list_states::StateId::execute(void* object) {
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

/* 02623138 */
void sead::instances::screen_baton_sign_list_states::StateId::leave(void* object) {
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

/* 02623178 */
u32 sead::instances::screen_baton_sign_list_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 026237A0 */
void sead::instances::screen_battery::StateId::enter(void* object) {
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

/* 026237E0 */
void sead::instances::screen_battery::StateId::execute(void* object) {
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

/* 02623820 */
void sead::instances::screen_battery::StateId::leave(void* object) {
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

/* 02623860 */
u32 sead::instances::screen_battery::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02624A40 */
void sead::instances::screen_battleship_game::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}


/* 02624A84 */
void sead::instances::screen_battleship_game::StateId::enter(void* object) {
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

/* 02624AC4 */
void sead::instances::screen_battleship_game::StateId::execute(void* object) {
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

/* 02624B04 */
void sead::instances::screen_battleship_game::StateId::leave(void* object) {
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

/* 02624B44 */
u32 sead::instances::screen_battleship_game::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02625410 */
void sead::instances::screen_best_shot_mark_init::StateId::enter(void* object) {
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

/* 02625450 */
void sead::instances::screen_best_shot_mark_init::StateId::execute(void* object) {
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

/* 02625490 */
void sead::instances::screen_best_shot_mark_init::StateId::leave(void* object) {
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

/* 026254D0 */
u32 sead::instances::screen_best_shot_mark_init::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02625F84 */
void sead::instances::screen_blur_bg::StateId::enter(void* object) {
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

/* 02625FC4 */
void sead::instances::screen_blur_bg::StateId::execute(void* object) {
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

/* 02626004 */
void sead::instances::screen_blur_bg::StateId::leave(void* object) {
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

/* 02626044 */
u32 sead::instances::screen_blur_bg::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02626B14 */
void sead::instances::screen_blur_bg::BufferedText::assureTermination() {
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}


/* 02626B6C */
void sead::instances::screen_boat_race_failed::StateId::enter(void* object) {
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

/* 02626BAC */
void sead::instances::screen_boat_race_failed::StateId::execute(void* object) {
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

/* 02626BEC */
void sead::instances::screen_boat_race_failed::StateId::leave(void* object) {
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

/* 02626C2C */
u32 sead::instances::screen_boat_race_failed::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 026275DC */
void sead::instances::screen_boat_race_failed_char_states::StateId::enter(void* object) {
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

/* 0262761C */
void sead::instances::screen_boat_race_failed_char_states::StateId::execute(void* object) {
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

/* 0262765C */
void sead::instances::screen_boat_race_failed_char_states::StateId::leave(void* object) {
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


/* 026282AC */
void sead::instances::screen_boat_race_failed_char_states::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}


/* 026282E4 */
void sead::instances::screen_boat_race_result_parts::StateId::enter(void* object) {
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

/* 02628324 */
void sead::instances::screen_boat_race_result_parts::StateId::execute(void* object) {
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

/* 02628364 */
void sead::instances::screen_boat_race_result_parts::StateId::leave(void* object) {
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

/* 026283A4 */
u32 sead::instances::screen_boat_race_result_parts::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02628AE4 */
void sead::instances::screen_boat_race_start::StateId::enter(void* object) {
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

/* 02628B24 */
void sead::instances::screen_boat_race_start::StateId::execute(void* object) {
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

/* 02628B64 */
void sead::instances::screen_boat_race_start::StateId::leave(void* object) {
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

/* 02628BA4 */
u32 sead::instances::screen_boat_race_start::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 026293B0 */
void sead::instances::screen_boot::StateId::enter(void* object) {
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

/* 026293F0 */
void sead::instances::screen_boot::StateId::execute(void* object) {
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

/* 02629430 */
void sead::instances::screen_boot::StateId::leave(void* object) {
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

/* 02629470 */
u32 sead::instances::screen_boot::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02629F04 */
void sead::instances::screen_boss_hp::StateId::enter(void* object) {
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

/* 02629F44 */
void sead::instances::screen_boss_hp::StateId::execute(void* object) {
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

/* 02629F84 */
void sead::instances::screen_boss_hp::StateId::leave(void* object) {
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

/* 02629FC4 */
u32 sead::instances::screen_boss_hp::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0262AC7C */
void sead::instances::screen_btn_collect_icon::StateId::enter(void* object) {
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

/* 0262ACBC */
void sead::instances::screen_btn_collect_icon::StateId::execute(void* object) {
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

/* 0262ACFC */
void sead::instances::screen_btn_collect_icon::StateId::leave(void* object) {
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

/* 0262AD3C */
u32 sead::instances::screen_btn_collect_icon::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0262C570 */
void sead::instances::screen_btn_collect_icon_mask::StateId::enter(void* object) {
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

/* 0262C5B0 */
void sead::instances::screen_btn_collect_icon_mask::StateId::execute(void* object) {
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

/* 0262C5F0 */
void sead::instances::screen_btn_collect_icon_mask::StateId::leave(void* object) {
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

/* 0262C630 */
u32 sead::instances::screen_btn_collect_icon_mask::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0262CFC8 */
void sead::instances::screen_btn_control_wind::StateId::enter(void* object) {
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

/* 0262D008 */
void sead::instances::screen_btn_control_wind::StateId::execute(void* object) {
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

/* 0262D048 */
void sead::instances::screen_btn_control_wind::StateId::leave(void* object) {
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

/* 0262D088 */
u32 sead::instances::screen_btn_control_wind::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0262E63C */
void sead::instances::screen_btn_difficulty::StateId::enter(void* object) {
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

/* 0262E67C */
void sead::instances::screen_btn_difficulty::StateId::execute(void* object) {
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

/* 0262E6BC */
void sead::instances::screen_btn_difficulty::StateId::leave(void* object) {
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

/* 0262E6FC */
u32 sead::instances::screen_btn_difficulty::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0262EC50 */
void sead::instances::screen_btn_dungeon_item_icon::StateId::enter(void* object) {
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

/* 0262EC90 */
void sead::instances::screen_btn_dungeon_item_icon::StateId::execute(void* object) {
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

/* 0262ECD0 */
void sead::instances::screen_btn_dungeon_item_icon::StateId::leave(void* object) {
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

/* 0262ED10 */
u32 sead::instances::screen_btn_dungeon_item_icon::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 0262F6F8 */
void sead::instances::screen_btn_floor::StateId::enter(void* object) {
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

/* 0262F738 */
void sead::instances::screen_btn_floor::StateId::execute(void* object) {
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

/* 0262F778 */
void sead::instances::screen_btn_floor::StateId::leave(void* object) {
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

/* 0262F7B8 */
u32 sead::instances::screen_btn_floor::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02633F2C */
void sead::instances::screen_btn_floor::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}


/* 02633F9C */
void sead::instances::screen_menu_item::StateId::enter(void* object) {
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

/* 02633FDC */
void sead::instances::screen_menu_item::StateId::execute(void* object) {
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

/* 0263401C */
void sead::instances::screen_menu_item::StateId::leave(void* object) {
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

/* 0263405C */
u32 sead::instances::screen_menu_item::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


/* 02635CC8 */
void sead::instances::screen_menu_item::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}


/* 02635D18 */
void sead::instances::screen_touch_slide_decide::StateId::enter(void* object) {
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



/* the destroy entries of the archive and pack resource factories (DirectResourceFactoryBase::destroy inlined) */
/* 026138AC */
static void destroyArchiveFactory(sead::DirectResourceFactoryBase* factory, u32 flags) {
    if (factory == NULL) return;
    factory->mDisposer.mVtable = &sDirectFactoryBaseDisposerVtable;
    factory->ResourceFactory::destroy(0);
    if (flags & 1) releaseObject(factory);
}

/* 0261390C */
static void destroyPackFactory(sead::DirectResourceFactoryBase* factory, u32 flags) {
    if (factory == NULL) return;
    factory->mDisposer.mVtable = &sDirectFactoryBaseDisposerVtable;
    factory->ResourceFactory::destroy(0);
    if (flags & 1) releaseObject(factory);
}

/* 02613CA4 */
u32 sead::instances::res_mgr::isPackResource(void* object, const RuntimeType* type) {
    if (sPackTypeGuard == 0) {
        sPackTypeGuard = 1;
        sPackType.mTable = &sPackTypeTable;
    }
    if (type == &sPackType) return 1;
    if (sDirectResourceTypeGuard == 0) {
        sDirectResourceTypeGuard = 1;
        sDirectResourceType.mTable = &sDirectResourceTypeTable;
    }
    if (type == &sDirectResourceType) return 1;
    if (sResourceTypeGuard == 0) {
        sResourceTypeGuard = 1;
        sResourceType.mTable = &sResourceTypeTable;
    }
    return type == &sResourceType;
}

/* WWHD glue
 data sDirectFactoryBaseDisposerVtable 1015E028 nocheck
 data sPackTypeTable 100E21B4 nocheck
 data sDirectResourceTypeTable 100E21A4 nocheck
 data sResourceTypeTable 100E20F4 nocheck
 data sPackTypeGuard 101FD778
 data sDirectResourceTypeGuard 101FD6E0
 data sResourceTypeGuard 101FD6DC
 data sPackType 101FDD64
 data sDirectResourceType 101FD944
 data sResourceType 101FD94C
 ext 0275CFD0 void sead::ResourceFactory::destroy(u32)
 ext 0273AF40 void releaseObject(void*)
*/
