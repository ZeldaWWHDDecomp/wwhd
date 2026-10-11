/* Message manager layouts with tww names. HD preserves only selected pane and picture helpers.
 * HD: J2D bounds move four bytes earlier; the vtable is at 0xC8. Pane-helper offsets match tww.
 * Private views avoid extending padding in shared J2D/heap layouts. Local only. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_node.h"
#include "f_pc/f_pc_priority.h"
#include "gc_api/SSystem/c_malloc.h"

using Mtx = be<f32>[3][4];
enum J2DMirror : u32 { J2DMirror_Y = 1, J2DMirror_X = 2 };

struct fopMsgM_f2d_class {
    /* 0x00 */ be<f32> x;
    /* 0x04 */ be<f32> y;
};
struct fopMsg_positionWordsHD {
    /* 0x00 */ be<u32> x;
    /* 0x04 */ be<u32> y;
    /* 0x08 */ be<u32> z;
};
struct fopMsg_prm_class {
    /* 0x00 */ gptr<fopAc_ac_c> mpActor;
    union {
        /* 0x04 */ cXyz mPos;
        /* 0x04 */ fopMsg_positionWordsHD mPosWords;
    };
    /* 0x10 */ be<u32> mMsgNo;
    /* 0x14 */ be<u32> field_0x14;
    /* 0x18 */ be<u32> field_0x18;
};
WWHD_SIZE(fopMsg_prm_class, 0x1C);
struct fopMsg_prm_timer : fopMsg_prm_class {
    /* 0x1C */ be<s32> mTimerMode;
    /* 0x20 */ be<u16> mLimitTimeMs;
    /* 0x22 */ be<u8> mShowType;
    /* 0x23 */ be<u8> mIconType;
    /* 0x24 */ fopMsgM_f2d_class mTimerPos;
    /* 0x2C */ fopMsgM_f2d_class mRupeePos;
};
WWHD_OFFSET(fopMsg_prm_timer, mTimerMode, 0x1C);
WWHD_OFFSET(fopMsg_prm_timer, mTimerPos, 0x24);
WWHD_OFFSET(fopMsg_prm_timer, mRupeePos, 0x2C);
WWHD_SIZE(fopMsg_prm_timer, 0x34);
struct fopMsgM_pane_class {
    /* 0x00 */ gptr<void> pane;
    /* 0x04 */ fopMsgM_f2d_class mPosTopLeftOrig;
    /* 0x0C */ fopMsgM_f2d_class mPosTopLeft;
    /* 0x14 */ fopMsgM_f2d_class mPosCenterOrig;
    /* 0x1C */ fopMsgM_f2d_class mPosCenter;
    /* 0x24 */ fopMsgM_f2d_class mSizeOrig;
    /* 0x2C */ fopMsgM_f2d_class mSize;
    /* 0x34 */ be<u8> mInitAlpha;
    /* 0x35 */ be<u8> mNowAlpha;
    /* 0x36 */ be<s16> mUserArea;
};
WWHD_OFFSET(fopMsgM_pane_class, mPosTopLeft, 0xC);
WWHD_OFFSET(fopMsgM_pane_class, mPosCenter, 0x1C);
WWHD_OFFSET(fopMsgM_pane_class, mSizeOrig, 0x24);
WWHD_OFFSET(fopMsgM_pane_class, mSize, 0x2C);
WWHD_OFFSET(fopMsgM_pane_class, mInitAlpha, 0x34);
WWHD_SIZE(fopMsgM_pane_class, 0x38);

// HD: TU-private J2D views expose only fields and vtable slots used here.
struct J2DPane {
    struct Bounds {
        /* 0x00 */ fopMsgM_f2d_class i;
        /* 0x08 */ fopMsgM_f2d_class f;
    };
    struct VTable {
        /* 0x000 */ u8 mReserved[0x5C];
        /* 0x05C */ be<u32> search;
        /* 0x060 */ u8 mReserved60[4];
        /* 0x064 */ be<u32> makeMatrix;
        /* 0x068 */ u8 mReserved68[0x7C - 0x68];
        /* 0x07C */ be<u32> drawFullSet2;
    };
    /* 0x000 */ u8 mReserved[8];
    /* 0x08 */ Bounds mBounds;
    /* 0x18 */ Bounds mGlobalBounds;
    /* 0x028 */ u8 mReserved28[0xBC - 0x28];
    /* 0x0BC */ gptr<J2DPane> mpParent;
    /* 0x0C0 */ u8 mReservedC0[8];
    /* 0xC8 */ gptr<VTable> __vtbl;
    void calcMtx();
    void makeMatrix(f32 x, f32 y) { gabi::call_ptr(__vtbl->makeMatrix, this, x, y); }
};
WWHD_OFFSET(J2DPane, mBounds, 8);
WWHD_OFFSET(J2DPane, mGlobalBounds, 0x18);
WWHD_OFFSET(J2DPane, mpParent, 0xBC);
WWHD_OFFSET(J2DPane, __vtbl, 0xC8);
WWHD_SIZE(J2DPane, 0xCC);
WWHD_OFFSET(J2DPane::VTable, search, 0x5C);
WWHD_OFFSET(J2DPane::VTable, makeMatrix, 0x64);
WWHD_OFFSET(J2DPane::VTable, drawFullSet2, 0x7C);
WWHD_SIZE(J2DPane::VTable, 0x80);
struct J2DScreen : J2DPane {
    J2DPane* search(u32 tag) { return gabi::call_ptr<J2DPane*>(__vtbl->search, this, tag); }
};
WWHD_SIZE(J2DScreen, 0xCC);
struct J2DPicture : J2DPane {
    /* 0x0CC */ gptr<void> mpTexture[1];
    /* 0x0D0 */ u8 mReservedD0[0xDE - 0xD0];
    /* 0xDE */ be<u8> mBinding;
    /* 0xDF */ be<u8> mMirror;
    /* 0x0E0 */ u8 mReservedE0[0x124 - 0xE0];
};
WWHD_OFFSET(J2DPicture, mpTexture, 0xCC);
WWHD_OFFSET(J2DPicture, mBinding, 0xDE);
WWHD_OFFSET(J2DPicture, mMirror, 0xDF);
WWHD_SIZE(J2DPicture, 0x124);
struct MyPicture : J2DPicture {
    /* 0x124 */ be<f32> m124;
    /* 0x128 */ be<f32> m128;
    /* 0x12C */ be<f32> m12C;
    /* 0x130 */ be<f32> m130;
    /* 0x134 */ be<u8> m134;
    /* 0x135 */ u8 mPadding[3];
    void drawSelf(f32 i_posx, f32 i_posy, Mtx* i_mtx);
    void drawFullSet2(f32 i_posx, f32 i_posy, f32 i_width, f32 i_height, u32 i_binding,
                      u32 i_mirror, u32 i_tumble, Mtx* i_mtx);
};
WWHD_OFFSET(MyPicture, m124, 0x124);
WWHD_OFFSET(MyPicture, m134, 0x134);
WWHD_SIZE(MyPicture, 0x138);
struct JKRExpHeap {
    struct VTable {
        /* 0x000 */ u8 mReserved[0x24];
        /* 0x024 */ be<u32> destroy;
    };
    /* 0x000 */ u8 mReserved[0xC];
    /* 0x0C */ gptr<VTable> __vtbl;
    /* 0x010 */ gptr<const char> mName;
    // HD: guest pointer assignment must map a host diagnostic literal to its guest copy.
    void setName(const char* name) { mName.v = gabi::guest_addr(name); }
    void destroy() { gabi::call_ptr(__vtbl->destroy, this); }
};
WWHD_OFFSET(JKRExpHeap, __vtbl, 0xC);
WWHD_OFFSET(JKRExpHeap, mName, 0x10);
WWHD_SIZE(JKRExpHeap, 0x14);
WWHD_OFFSET(JKRExpHeap::VTable, destroy, 0x24);
WWHD_SIZE(JKRExpHeap::VTable, 0x28);
struct STControl {
    /* 0x000 */ u8 mReserved[0x24];
    /* 0x024 */ be<u32> __vtbl;
    void setWaitParm(s16, s16, s16, s16, f32, f32, s16, u16);
    void init();
};
WWHD_OFFSET(STControl, __vtbl, 0x24);
WWHD_SIZE(STControl, 0x28);
struct fopMsgTableEntryHD {
    /* 0x000 */ be<u8> mValue;
    /* 0x001 */ u8 mReserved[7];
};
WWHD_SIZE(fopMsgTableEntryHD, 8);
// HD: preserving the assertion result and the full process-name width requires explicit ABI
// bridges.
u32 JUTAssertion_showAssertResultHD(const char*, s32, const char*);
process_node_class* fopScnM_SearchByIDHD(u32);
BOOL fpcBs_Is_JustOfTypeHD(s32, s32);
s32 fpcPi_ChangeHD(void*, u32, u32, u32);
u32 fpcLy_CurrentLayerHD();
s32 fpcSCtRq_RequestHD(u32, u32, u32, u32, fopMsg_prm_class*);
void* mDoExt_getGameHeapHD();
JKRExpHeap* JKRExpHeap_createHD(u32, void*, BOOL);
void fopMsgM_operatorDeleteHD(void*);
void J2DPicture_dtorHD(J2DPicture*, s32);
void J2DPicture_drawFullSetHD(J2DPicture*, f32, f32, f32, f32, u32, u32, u32, Mtx*);

void fopMsgM_cposMove(fopMsgM_pane_class* i_pane);
