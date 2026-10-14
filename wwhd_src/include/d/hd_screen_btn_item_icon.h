/* HD-only item icon button. Descriptive names; allocator view pending its framework unit. */
#pragma once
#include "d/hd_screen_menu_item.h"
struct hdItemIconAllocatorVtbl_l { u8 _00[0xC]; gfn mDelete; };
struct hdItemIconAllocator_l {
    u8 _00[0xC]; gptr<hdItemIconAllocatorVtbl_l> mpVtable;
    void vDelete(s32 flags) { gabi::call_ptr(mpVtable->mDelete.get(), this, flags); }
};
WWHD_OFFSET(hdItemIconAllocator_l, mpVtable, 0xC);
WWHD_SIZE(hdItemIconAllocator_l, 0x10);
struct hdItemIconContext_c {
    gptr<hdAnimSet> mpAnimation;
    gptr<hdPartsPaneSRT_c> mpRootPane;
    gptr<hdItemPaneArray_c> mpPaneGroups[3];
    gptr<be<u8>> mpVisible;
};
WWHD_OFFSET(hdItemIconContext_c, mpRootPane, 4);
WWHD_OFFSET(hdItemIconContext_c, mpPaneGroups, 8);
WWHD_OFFSET(hdItemIconContext_c, mpVisible, 0x14);
WWHD_SIZE(hdItemIconContext_c, 0x18);
struct hdItemIconVtbl_l {
    u8 _00[0x354]; gfn mCachePosition;
    u8 _358[4]; gfn mTranslatePoint;
};
WWHD_OFFSET(hdItemIconVtbl_l, mCachePosition, 0x354);
WWHD_OFFSET(hdItemIconVtbl_l, mTranslatePoint, 0x35C);
struct hdItemIcon_c : hdMenuItem_c {
    hdItemIconAllocator_l mAllocator;
    be<u8> mShown;
    u8 _129[3];
    static hdItemIcon_c* construct(hdItemIcon_c* self);
    static void destroy(hdItemIcon_c* self, s32 flags);
    static hdItemIcon_c* create(hdScreenHeap* heap);
    static void release();
    u32 load(void* resource, u32 arg1, u32 arg2);
    void finalize();
    void execute();
    void draw(void* info);
    void showFromTouch(hdItemPoint_c* point);
    void hideFromUnselect();
    void showFromCursor(hdItemPoint_c* point);
    void positionAtPoint(hdItemPoint_c* point);
    void hideAfterCancel();
    void hideAfterFloat();
    void animateToPoint(hdItemPoint_c* point);
    void showAtPoint(hdItemPoint_c* point);
    void hideAfterKey();
    void getContext(hdItemIconContext_c* context);
    void cachePosition();
    void translatePoint(hdItemPoint_c* point);
    hdPartsPaneSRT_c* layoutRootPane() { return (hdPartsPaneSRT_c*)mButton.mpLayout->lyt()->mpRootPane.get(); }
    hdItemIconVtbl_l* iconVtable() { return (hdItemIconVtbl_l*)mButton.mVtable.get(); }
    void vCachePosition() { gabi::call_ptr(iconVtable()->mCachePosition.get(), this); }
    void vTranslatePoint(hdItemPoint_c* point) { gabi::call_ptr(iconVtable()->mTranslatePoint.get(), this, point); }
};
WWHD_OFFSET(hdItemIcon_c, mAllocator, 0x118);
WWHD_OFFSET(hdItemIcon_c, mShown, 0x128);
WWHD_SIZE(hdItemIcon_c, 0x12C);
