/* WWHD tower pedestal. */
#pragma once
#include "bindings.h"
namespace daPedestal {
struct Action {
  be<s16> delta, slot;
  be<u32> function;
};
struct Brk {
  u8 state[0x78];
};
struct Callback {
  be<u32> vt;
  gptr<u8> emitter;
  gptr<cXyz> pos;
  gptr<csXyz> angle;
  void execute(u8 *);
  void end();
  void makeEmitter(u16, const cXyz *, const csXyz *, const cXyz *);
  void setup(u8 *);
};
struct daPds_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mpModel;
  Brk mBrk;
  Mtx34 mMtx;
  gptr<u8> mpBgW;
  Callback mGlow;
  Action mAction;
  be<f32> mTargetY, mFrame;
  be<s8> mStatus;
  be<u8> mType, mAnmIndex;
  be<s8> mPlayResult;
  be<s16> mTimer0, mTimer1, mTimer2, mTimer3;
  u8 _490[4];
  be<f32> mValue;
  u8 _498[4];
  BOOL initBrkAnm(u8, BOOL);
  BOOL CreateHeap();
  BOOL wakeupCheck();
  BOOL finishCheck();
  void set_mtx();
  void CreateInit();
  s32 _create();
  BOOL _delete();
  void playBrkAnm();
  s32 getMyStaffId();
  BOOL eventProc();
  BOOL setAction(Action *, u32);
  void action(u32);
  BOOL _execute();
  BOOL _draw();
  void initialDefault(s32);
  BOOL actionDefault(s32);
  void initialMoveEvent(s32);
  BOOL actionMoveEvent(s32);
  void initialEffectSet(s32);
  void initialEffectEnd(s32);
  BOOL waitAction(u32);
};
WWHD_SIZE(Action, 8);
WWHD_SIZE(Callback, 0x10);
WWHD_OFFSET(daPds_c, mBrk, 0x3B8);
WWHD_OFFSET(daPds_c, mMtx, 0x430);
WWHD_OFFSET(daPds_c, mGlow, 0x464);
WWHD_OFFSET(daPds_c, mAction, 0x474);
WWHD_OFFSET(daPds_c, mStatus, 0x484);
WWHD_OFFSET(daPds_c, mValue, 0x494);
WWHD_SIZE(daPds_c, 0x49C);
} // namespace daPedestal
