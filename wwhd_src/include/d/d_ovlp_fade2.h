/* Second scene fade process. The HD rendering path adds two opaque graphics objects
 * after the GameCube fields. */
#pragma once
#include "SSystem/SComponent/c_phase.h"
#include "bindings.h"
#include "f_op/f_op_overlap.h"
struct fd2Snapshot {
    u8 mPadding[8];
    gptr<void> mSurface;
    u8 mPaddingC[0x10];
    gptr<void> mTarget;
};
WWHD_OFFSET(fd2Snapshot, mSurface, 8);
WWHD_OFFSET(fd2Snapshot, mTarget, 0x1C);
WWHD_SIZE(fd2Snapshot, 0x20);
struct fd2DrawList {
    be<u32> mVtable;
};
WWHD_SIZE(fd2DrawList, 4);
struct dOvlpFd2_dlst_c {
    be<u32> mVtable;
    Mtx34 mtx;
};
WWHD_OFFSET(dOvlpFd2_dlst_c, mtx, 4);
WWHD_SIZE(dOvlpFd2_dlst_c, 0x34);
class dOvlpFd2_c : public overlap_task_class {
  public:
    gabi::ptmf<void (dOvlpFd2_c::*)()> mProc;
    fd2DrawList dSnap_dlst;
    dOvlpFd2_dlst_c dOvlpFd2_dlst;
    be<s16> field_0x110, field_0x112, rotZ;
    u8 mPadding116[2];
    be<f32> scale;
    be<u8> mHasSnap;
    be<s8> mTimer;
    be<u8> field_0x11e;
    u8 mPadding11f;
    u8 mHdSnapshotState[0x40];
    u8 mHdRendererState[0x148];
    void draw();
    void execFirstSnap();
    void execFadeOut();
    void execNextSnap();
    void execFadeIn();
    void captureSnapshot(fd2Snapshot*);
};
WWHD_OFFSET(dOvlpFd2_c, mProc, 0xd0);
WWHD_OFFSET(dOvlpFd2_c, dSnap_dlst, 0xd8);
WWHD_OFFSET(dOvlpFd2_c, dOvlpFd2_dlst, 0xdc);
WWHD_OFFSET(dOvlpFd2_c, field_0x110, 0x110);
WWHD_OFFSET(dOvlpFd2_c, field_0x112, 0x112);
WWHD_OFFSET(dOvlpFd2_c, rotZ, 0x114);
WWHD_OFFSET(dOvlpFd2_c, scale, 0x118);
WWHD_OFFSET(dOvlpFd2_c, mHasSnap, 0x11c);
WWHD_OFFSET(dOvlpFd2_c, mTimer, 0x11d);
WWHD_OFFSET(dOvlpFd2_c, field_0x11e, 0x11e);
WWHD_OFFSET(dOvlpFd2_c, mHdSnapshotState, 0x120);
WWHD_OFFSET(dOvlpFd2_c, mHdRendererState, 0x160);
WWHD_SIZE(dOvlpFd2_c, 0x2A8);
struct fd2GameInfoView {
    u8 mPadding[0x5AC9];
    be<u8> mWindowNum;
    u8 mPadding5aca[0x158];
    be<u8> m2dShow;
    u8 mPadding5c23[0x10D];
    u8 mDrawList[0x26C];
};
WWHD_OFFSET(fd2GameInfoView, mWindowNum, 0x5AC9);
WWHD_OFFSET(fd2GameInfoView, m2dShow, 0x5C22);
WWHD_OFFSET(fd2GameInfoView, mDrawList, 0x5D30);
WWHD_SIZE(fd2GameInfoView, 0x5F9C);
struct fd2Handle {
    be<u32> mValue, mFlags;
};
WWHD_SIZE(fd2Handle, 8);
struct fd2TextureView {
    u8 mOpaque[0x198];
};
WWHD_SIZE(fd2TextureView, 0x198);
fd2GameInfoView* fd2_getGameInfo();
void fd2_mtxTrans(Mtx34* mtx, f32 x, f32 y, f32 z);
void fd2_setDrawList(void* list, void* head, void* tail, dOvlpFd2_dlst_c* entry);
void fd2_snapshotStateDelete(void* state, s32 flags);
void fd2_drawListDelete(void* entry, s32 flags);
void fd2_baseDelete(dOvlpFd2_c* self, s32 flags);
void fd2_releaseTexture(void* manager, u32 slot);
void fd2_unlinkFade(void* manager);
void fd2_baseConstruct(dOvlpFd2_c* self);
void fd2_drawListConstruct(void* entry);
void fd2_snapshotStateConstruct(void* state);
void fd2_rendererStateConstruct(void* state);
void fd2_linkFade(void* manager, dOvlpFd2_c* self);
void fd2_mtxIdentity(Mtx34* mtx);
u32 fd2_calcTimer(be<s8>* timer);
void fopOvlpM_Done(dOvlpFd2_c* self);
void fd2_offMonotone();
void fd2_chaseAngleS(be<s16>* angle, s32 target, s32 step);
u32 fopOvlpM_IsOutReq(dOvlpFd2_c* self);
void fopOvlpM_SceneIsStart();
void fd2_startFadeOut(u32 time);
void fd2_addCalc2(be<f32>* value, f32 target, f32 rate, f32 step);
u32 fd2_startFadeIn(void* manager, u32 time, u32 flag);
void fd2_addCalc0(be<f32>* value, f32 rate, f32 step);
void fopOvlpM_SceneIsStop();
void fd2_storeSnapshot(void* manager, u32 slot, fd2Snapshot* snapshot);
void fd2_setSurface(void* surface);
void fd2_setTargetSurface(void* target, void* surface);
void* fd2_getTexture(void* manager, u32 slot);
void fd2_textureViewConstruct(fd2TextureView* view, void* texture);
fd2Handle* fd2_getHandle(void* key);
u32 fd2_renderSnapshot(void* renderer, fd2TextureView* view, Mtx34* mtx, u32 old);
void fd2_textureViewDelete(fd2TextureView* view, s32 flags);
namespace fd2 {
// HD: encoded guest member addresses, not host C++ member pointers.
enum Phase : u32 { FIRST = 0x025A1F20, OUT = 0x025A1FB4, NEXT = 0x025A2150, IN = 0x025A21F0 };
} // namespace fd2
