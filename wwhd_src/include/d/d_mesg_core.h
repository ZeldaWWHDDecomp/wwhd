/* HD message manager core.
 * tww d_mesg.cpp distributes this work among globals and message processors.
 */
#pragma once
#include "bindings.h"

struct dMesg_reset_work { void reset(); };
struct dMesg_screen_vtable {
    u8 _000[0x41C];
    gfn reset;
};
struct dMesg_render_vtable {
    u8 _000[0x1EC];
    gfn reset;
};
struct dMesg_screen_work {
    u8 _000[4];
    gptr<dMesg_screen_vtable> __vtbl;
    void reset() { gabi::call_ptr(__vtbl->reset.get(), this); }
};
struct dMesg_render_work {
    u8 _000[4];
    gptr<dMesg_render_vtable> __vtbl;
    void reset() { gabi::call_ptr(__vtbl->reset.get(), this); }
};
struct dMesg_word_work {
    u8 _000;
    be<u8> mType;
};
struct dMesg_core_c {
    enum : u32 { __vt = 0x100E0CD0 };
    void advanceTimers();
    void update();
    void reset();

    /* 0x000 */ u8 mDisposer[0xC];
    /* 0x00C */ be<u32> __vtbl;
    /* 0x010 */ be<u32> mFrameCount;
    /* 0x014 */ u8 mProcessorWork[0x240 - 0x14];
    /* 0x240 */ gptr<dMesg_word_work> mWordWork;
    /* 0x244 */ u8 _244[0x938 - 0x244];
    /* 0x938 */ be<u32> mMsgNo;
    /* 0x93C */ u8 _93C[0xC];
    /* 0x948 */ be<u32> mSelectNum;
    /* 0x94C */ be<u32> mMode;
    /* 0x950 */ gptr<dMesg_reset_work> mResetWork;
    /* 0x954 */ gptr<dMesg_screen_work> mScreenWork;
    /* 0x958 */ gptr<dMesg_render_work> mRenderWork;
    /* 0x95C */ gptr<dMesg_screen_work> mAuxScreenWork;
    /* 0x960 */ gptr<void> mSequenceWork;
    /* 0x964 */ gptr<void> mResourceWork;
    /* 0x968 */ be<u32> field_0x968;
    /* 0x96C */ be<u32> field_0x96c;
    /* 0x970 */ be<u32> field_0x970;
    /* 0x974 */ be<f32> field_0x974;
    /* 0x978 */ be<f32> field_0x978;
    /* 0x97C */ be<f32> field_0x97c;
    /* 0x980 */ be<f32> field_0x980;
    /* 0x984 */ be<u8> field_0x984;
    /* 0x985 */ be<u8> field_0x985;
    /* 0x986 */ u8 _986[2];
    /* 0x988 */ be<f32> field_0x988;
    /* 0x98C */ be<f32> field_0x98c;
    /* 0x990 */ be<f32> field_0x990;
    /* 0x994 */ be<u32> mTimers[3];
    /* 0x9A0 */ be<u8> field_0x9a0;
    /* 0x9A1 */ be<u8> field_0x9a1;
    /* 0x9A2 */ u8 _9A2[2];
};
// HD: fields without a demonstrated tww equivalent keep explicit unknown-field names.
WWHD_OFFSET(dMesg_core_c, mWordWork, 0x240);
WWHD_OFFSET(dMesg_core_c, mMsgNo, 0x938);
WWHD_OFFSET(dMesg_core_c, mResetWork, 0x950);
WWHD_OFFSET(dMesg_core_c, mTimers, 0x994);
WWHD_SIZE(dMesg_core_c, 0x9A4);
struct dMesg_core_play_view {
    u8 _0000[0x5BB2];
    be<u8> mMesgStatus;
    u8 _5BB3[0x5BD2 - 0x5BB3];
    be<u8> mFrameFlag;
    u8 _5BD3[0x5C20 - 0x5BD3];
    be<u8> mSpecialMesgFlag;
};
WWHD_OFFSET(dMesg_core_play_view, mSpecialMesgFlag, 0x5C20);
inline dMesg_core_play_view* dMesg_core_getPlay() {
    return reinterpret_cast<dMesg_core_play_view*>(dComIfGp_get());
}
dMesg_core_c* dMesg_core_ct(dMesg_core_c* self);
void dMesg_core_initProcessorWork(void* work);
void* dMesg_core_allocate(u32 size, JKRHeap* heap, s32 alignment);
void dMesg_core_initDisposer(dMesg_core_c* self, JKRHeap* heap, u32 flags);
void dMesg_core_setMesgStatus(dMesg_core_c* self, u32 status);
u32 dMesg_core_getCurrentMsgNo();
void dMesg_core_resetSequence(void* work);
void dMesg_core_resetResource(void* work);
