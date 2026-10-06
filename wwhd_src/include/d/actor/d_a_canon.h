#pragma once
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"
struct CanonProcess {
  be<s16> delta, slot;
  be<u32> target;
};
struct daCanon_c : fopAc_ac_c {
  CanonProcess process;
  dCcD_Stts status1;
  dCcD_Cyl collision1;
  dCcD_Stts status2;
  dCcD_Cyl collision2;
  be<u32> phase[2];
  gptr<J3DModel> model, ballModel;
  gptr<void> bombs[10], ships[5], battery;
  Mtx34 baseMatrix;
  be<u8> grid[10];
  u8 pad[2];
  be<u32> targetIds[5];
  cXyz launch, endpoint, ball;
  be<f32> recoil;
  cXyz muzzle;
  csXyz muzzleAngle;
  be<s16> unused1, unused2, flight, duration, timer, unused3, hits, remaining,
      phaseAngle, amplitude;
  be<u16> padding;
  be<u8> effectActive;
  u8 tail[3];
};
WWHD_OFFSET(daCanon_c, process, 0x3AC);
WWHD_OFFSET(daCanon_c, collision1, 0x3F0);
WWHD_OFFSET(daCanon_c, collision2, 0x55C);
WWHD_OFFSET(daCanon_c, model, 0x694);
WWHD_OFFSET(daCanon_c, baseMatrix, 0x6DC);
WWHD_OFFSET(daCanon_c, launch, 0x72C);
WWHD_OFFSET(daCanon_c, hits, 0x772);
WWHD_OFFSET(daCanon_c, effectActive, 0x77C);
static_assert(sizeof(daCanon_c) == 0x780);
namespace canon {
// A real nonleaf callee may save LR at callerSP+4. Keep payload above it.
template <class T> struct Local {
  struct Frame {
    be<u32> linkage[2];
    T payload;
  };
  static_assert(offsetof(Frame, payload) == 8);
  gabi::Local<Frame> frame;
  T *get() const { return gabi::at<T>(frame.a + 8); }
  T *operator->() const { return get(); }
  T &operator*() const { return *get(); }
  operator T *() const { return get(); }
};
} // namespace canon
