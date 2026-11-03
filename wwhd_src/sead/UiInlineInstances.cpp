/* UI state-ID dispatch and wide-buffer termination instances.
 * Reconstructed from original game instructions and our merged canonical layouts.
 * No external reference used. */
#include "sead/UiInlineInstances.h"
#include "sead/ElementAt.h"

using sead::instances::MemberFunction;
using sead::instances::MethodEntry;
using sead::instances::StateIdLayout;

/* 02635D58 */
void sead::ui_instances::touch_slide_decide::StateId::execute(void* object) {
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

/* 02635D98 */
void sead::ui_instances::touch_slide_decide::StateId::leave(void* object) {
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

/* 02635DD8 */
u32 sead::ui_instances::touch_slide_decide::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0263650C */
void sead::ui_instances::item_screen_tu3::StateId::enter(void* object) {
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

/* 0263654C */
void sead::ui_instances::item_screen_tu3::StateId::execute(void* object) {
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

/* 0263658C */
void sead::ui_instances::item_screen_tu3::StateId::leave(void* object) {
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

/* 026365CC */
u32 sead::ui_instances::item_screen_tu3::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0263AD58 */
void sead::ui_instances::btn_map_icon_extra::StateId::enter(void* object) {
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

/* 0263AD98 */
void sead::ui_instances::btn_map_icon_extra::StateId::execute(void* object) {
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

/* 0263ADD8 */
void sead::ui_instances::btn_map_icon_extra::StateId::leave(void* object) {
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

/* 0263AE18 */
u32 sead::ui_instances::btn_map_icon_extra::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0263BB44 */
void sead::ui_instances::btn_map_pattern::StateId::enter(void* object) {
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

/* 0263BB84 */
void sead::ui_instances::btn_map_pattern::StateId::execute(void* object) {
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

/* 0263BBC4 */
void sead::ui_instances::btn_map_pattern::StateId::leave(void* object) {
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

/* 0263BC04 */
u32 sead::ui_instances::btn_map_pattern::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0263CB38 */
void sead::ui_instances::btn_menu::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 0263CB70 */
void sead::ui_instances::btn_menu::StateId::enter(void* object) {
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

/* 0263CBB0 */
void sead::ui_instances::btn_menu::StateId::execute(void* object) {
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

/* 0263CBF0 */
void sead::ui_instances::btn_menu::StateId::leave(void* object) {
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

/* 0263CC30 */
u32 sead::ui_instances::btn_menu::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0263DC2C */
void sead::ui_instances::btn_menu_arrow::StateId::enter(void* object) {
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

/* 0263DC6C */
void sead::ui_instances::btn_menu_arrow::StateId::execute(void* object) {
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

/* 0263DCAC */
void sead::ui_instances::btn_menu_arrow::StateId::leave(void* object) {
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

/* 0263DCEC */
u32 sead::ui_instances::btn_menu_arrow::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0263E1E4 */
void sead::ui_instances::btn_menu_heart::StateId::enter(void* object) {
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

/* 0263E224 */
void sead::ui_instances::btn_menu_heart::StateId::execute(void* object) {
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

/* 0263E264 */
void sead::ui_instances::btn_menu_heart::StateId::leave(void* object) {
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

/* 0263E2A4 */
u32 sead::ui_instances::btn_menu_heart::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02640FC8 */
void sead::ui_instances::btn_miiverse_comment::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 02641098 */
void sead::ui_instances::btn_miiverse_comment::StateId::enter(void* object) {
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

/* 026410D8 */
void sead::ui_instances::btn_miiverse_comment::StateId::execute(void* object) {
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

/* 02641118 */
void sead::ui_instances::btn_miiverse_comment::StateId::leave(void* object) {
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

/* 02641158 */
u32 sead::ui_instances::btn_miiverse_comment::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02644724 */
void sead::ui_instances::pictograph_post::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 02645638 */
void sead::ui_instances::btn_triforce::StateId::enter(void* object) {
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

/* 02645678 */
void sead::ui_instances::btn_triforce::StateId::execute(void* object) {
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

/* 026456B8 */
void sead::ui_instances::btn_triforce::StateId::leave(void* object) {
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

/* 026456F8 */
u32 sead::ui_instances::btn_triforce::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02645E60 */
void sead::ui_instances::character::StateId::enter(void* object) {
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

/* 02645EA0 */
void sead::ui_instances::character::StateId::execute(void* object) {
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

/* 02645EE0 */
void sead::ui_instances::character::StateId::leave(void* object) {
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

/* 02645F20 */
u32 sead::ui_instances::character::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026470B8 */
void sead::ui_instances::cannon_game_2::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 026470F8 */
void sead::ui_instances::cannon_game_2::StateId::enter(void* object) {
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

/* 02647138 */
void sead::ui_instances::cannon_game_2::StateId::execute(void* object) {
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

/* 02647178 */
void sead::ui_instances::cannon_game_2::StateId::leave(void* object) {
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

/* 026471B8 */
u32 sead::ui_instances::cannon_game_2::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02648874 */
void sead::ui_instances::command_a_extra::StateId::enter(void* object) {
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

/* 026488B4 */
void sead::ui_instances::command_a_extra::StateId::execute(void* object) {
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

/* 026488F4 */
void sead::ui_instances::command_a_extra::StateId::leave(void* object) {
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

/* 02648934 */
u32 sead::ui_instances::command_a_extra::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0264B4D0 */
void sead::ui_instances::command_guide_extra::StateId::enter(void* object) {
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

/* 0264B510 */
void sead::ui_instances::command_guide_extra::StateId::execute(void* object) {
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

/* 0264B550 */
void sead::ui_instances::command_guide_extra::StateId::leave(void* object) {
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

/* 0264B590 */
u32 sead::ui_instances::command_guide_extra::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0264C3D0 */
void sead::ui_instances::command_button::StateId::enter(void* object) {
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

/* 0264C410 */
void sead::ui_instances::command_button::StateId::execute(void* object) {
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

/* 0264C450 */
void sead::ui_instances::command_button::StateId::leave(void* object) {
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

/* 0264C490 */
u32 sead::ui_instances::command_button::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0264EBDC */
void sead::ui_instances::operate_wind::StateId::enter(void* object) {
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

/* 0264EC1C */
void sead::ui_instances::operate_wind::StateId::execute(void* object) {
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

/* 0264EC5C */
void sead::ui_instances::operate_wind::StateId::leave(void* object) {
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

/* 0264EC9C */
u32 sead::ui_instances::operate_wind::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0264F8F4 */
void sead::ui_instances::control_wind_ok_states::StateId::enter(void* object) {
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

/* 0264F934 */
void sead::ui_instances::control_wind_ok_states::StateId::execute(void* object) {
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

/* 0264F974 */
void sead::ui_instances::control_wind_ok_states::StateId::leave(void* object) {
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

/* 0264F9B4 */
u32 sead::ui_instances::control_wind_ok_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0265044C */
void sead::ui_instances::crawling_direction::StateId::enter(void* object) {
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

/* 0265048C */
void sead::ui_instances::crawling_direction::StateId::execute(void* object) {
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

/* 026504CC */
void sead::ui_instances::crawling_direction::StateId::leave(void* object) {
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

/* 0265050C */
u32 sead::ui_instances::crawling_direction::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0265146C */
void sead::ui_instances::cursor::StateId::enter(void* object) {
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

/* 026514AC */
void sead::ui_instances::cursor::StateId::execute(void* object) {
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

/* 026514EC */
void sead::ui_instances::cursor::StateId::leave(void* object) {
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

/* 0265152C */
u32 sead::ui_instances::cursor::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026527A0 */
void sead::ui_instances::msg_cursor::StateId::enter(void* object) {
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

/* 026527E0 */
void sead::ui_instances::msg_cursor::StateId::execute(void* object) {
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

/* 02652820 */
void sead::ui_instances::msg_cursor::StateId::leave(void* object) {
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

/* 02652860 */
u32 sead::ui_instances::msg_cursor::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 02653390 */
void sead::ui_instances::fixed_cursor::StateId::enter(void* object) {
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

/* 026533D0 */
void sead::ui_instances::fixed_cursor::StateId::execute(void* object) {
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

/* 02653410 */
void sead::ui_instances::fixed_cursor::StateId::leave(void* object) {
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

/* 02653450 */
u32 sead::ui_instances::fixed_cursor::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0265460C */
void sead::ui_instances::dungeon_key_states::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}
/* 02654650 */
void sead::ui_instances::dungeon_key_states::StateId::enter(void* object) {
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

/* 02654690 */
void sead::ui_instances::dungeon_key_states::StateId::execute(void* object) {
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

/* 026546D0 */
void sead::ui_instances::dungeon_key_states::StateId::leave(void* object) {
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

/* 02654710 */
u32 sead::ui_instances::dungeon_key_states::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 0265EC60 */
void sead::ui_instances::dungeon_map_part2::StateId::enter(void* object) {
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

/* 0265ECA0 */
void sead::ui_instances::dungeon_map_part2::StateId::execute(void* object) {
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

/* 0265ECE0 */
void sead::ui_instances::dungeon_map_part2::StateId::leave(void* object) {
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

/* 0265ED20 */
u32 sead::ui_instances::dungeon_map_part2::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}

/* 026662D0 */
void sead::ui_instances::dungeon_map_statue::StateId::enter(void* object) {
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

/* 02666310 */
void sead::ui_instances::dungeon_map_statue::StateId::execute(void* object) {
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

/* 02666350 */
void sead::ui_instances::dungeon_map_statue::StateId::leave(void* object) {
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

/* 02666390 */
u32 sead::ui_instances::dungeon_map_statue::StateId::resolveId() {
    StateIdLayout* parent = mParentState;
    if (parent->mId == -1) return mId;
    return gabi::tail_ptr<u32>(parent->mVtable->mResolveId, parent);
}


// Binary-identical wide-buffer idiom; no ownership association is claimed.
/* 0263B3A8 */
void sead::ui_instances::BufferedWideText::assureTermination() {
    u32 capacity = mCapacity;
    elementAt(mText, capacity - 1) = 0;
}

/* WWHD glue
*/
