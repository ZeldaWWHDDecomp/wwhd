/* Moving-background actor, GameCube names and HD layout.
 * HD inherits the actor vtable at 0xB4; its tail starts at 0x3AC, total size 0x3E0. */
#pragma once
#include "f_op/f_op_actor.h"
struct dBgW {
    enum { MOVE_BG_e=1 };
    /* 0x00 */ be<u32> m_id;
    u8 collisionState[0xA4];
    /* 0xA8 */ be<u32> m_crr_func;
};
struct MoveBgVtable {
    u8 baseMethods[0x14];
    be<u32> CreateHeap, reserved18, Create, reserved20, Execute, reserved28, Draw, reserved30,
        Delete;
};
class dBgS_MoveBgActor : public fopAc_ac_c {
  public:
    s32 MoveBGCreateHeap();
    s32 MoveBGCreate(const char *, u32, u32, u32);
    s32 MoveBGExecute();
    s32 MoveBGDelete();
    // HD: preserve dynamic calls through the inherited GHS virtual table.
    s32 DispatchCreateHeap() {
        return gabi::call_ptr<s32>(gabi::at<MoveBgVtable>(__vtbl)->CreateHeap, this);
    }
    s32 DispatchCreate() {
        return gabi::call_ptr<s32>(gabi::at<MoveBgVtable>(__vtbl)->Create, this);
    }
    s32 DispatchExecute(be<u32> *out) {
        return gabi::call_ptr<s32>(gabi::at<MoveBgVtable>(__vtbl)->Execute, this, out);
    }
    s32 DispatchDelete() {
        return gabi::call_ptr<s32>(gabi::at<MoveBgVtable>(__vtbl)->Delete, this);
    }
    /* 0x3AC */ gptr<dBgW> mpBgW;
    /* 0x3B0 */ Mtx34 mBgMtx;
};
WWHD_OFFSET(dBgS_MoveBgActor, mpBgW, 0x3AC);
WWHD_OFFSET(dBgS_MoveBgActor, mBgMtx, 0x3B0);
WWHD_SIZE(dBgS_MoveBgActor, 0x3E0);
WWHD_OFFSET(dBgW, m_crr_func, 0xA8);
struct MoveBgSetup {
    gptr<const char> m_name;
    be<u32> m_dzb_id, m_set_func;
};
struct MoveBgResourceKey {
    gptr<const char> name;
    be<u32> table;
};
struct MoveBgGameInfo {
    u8 save[0x12A0];
    u8 mBgS;
};
using MoveBgHeapCallback = s32 (*)(dBgS_MoveBgActor *);
void *operator_new(u32);
void operator_delete(void *);
void constructActor(dBgS_MoveBgActor *);
void destroyActor(dBgS_MoveBgActor *, u32);
dBgW *constructBackground(dBgW *);
void *getObjectResource(void *, MoveBgResourceKey *, u32);
s32 SetBackground(dBgW *, void *, u32, Mtx34 *);
s32 fopAcM_entrySolidHeap(dBgS_MoveBgActor *, MoveBgHeapCallback, u32);
MoveBgGameInfo *dComIfGp_getGameInfo();
s32 RegisterBackground(void *, dBgW *, dBgS_MoveBgActor *);
void ReleaseBackground(void *, dBgW *);
void MoveBackground(dBgW *);
void PSMTXTrans(Mtx34 *, f32, f32, f32);
void mDoMtx_YrotM(Mtx34 *, s32);
void mDoMtx_stack_scaleM(f32, f32, f32);
void PSMTXCopy(const Mtx34 *, Mtx34 *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
