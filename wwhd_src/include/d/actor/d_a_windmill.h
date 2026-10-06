#pragma once
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"
struct WindmillCapsuleData {
  cXyz start, end;
  be<f32> radius;
};
struct daWindMill_c : fopAc_ac_c {
  be<u32> phase[2];
  gptr<J3DModel> model;
  dCcD_Stts status;
  dCcD_Sph spheres[9];
  dCcD_Cps capsules[4];
  WindmillCapsuleData capsuleData[4];
  gptr<void> background;
  Mtx34 backgroundMatrix;
  dCcD_Cyl cylinder;
  be<u8> type;
  u8 pad;
  be<s16> angle[3];
  be<u32> windTagId;
};
WWHD_OFFSET(daWindMill_c, model, 0x3B4);
WWHD_OFFSET(daWindMill_c, spheres, 0x3F4);
WWHD_OFFSET(daWindMill_c, capsules, 0xE80);
WWHD_OFFSET(daWindMill_c, capsuleData, 0x1360);
WWHD_OFFSET(daWindMill_c, background, 0x13D0);
WWHD_OFFSET(daWindMill_c, cylinder, 0x1404);
WWHD_OFFSET(daWindMill_c, type, 0x1534);
WWHD_OFFSET(daWindMill_c, windTagId, 0x153C);
static_assert(sizeof(daWindMill_c) == 0x1540);

// Reserve the caller linkage words below every guest temporary payload.
namespace wind {
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
} // namespace wind
