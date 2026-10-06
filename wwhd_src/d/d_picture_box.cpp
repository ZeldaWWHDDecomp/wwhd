/* Qualified HD twelve-photo storage replacement. Legacy02725E0C map is a
 * behavioral association, not preservation of the GC API or TU boundary.
 * Exact scope02725BE8..027262BC plus02726360; old UI/record TU excluded. */
#include "gabi.h"
using namespace gabi;
namespace picture_store_cpp {
void destroy(void *self, u32 flags) {
  WWHD_FUNC(0x02725BE8, void, self, flags);
  if (self && (flags & 1))
    call<void>(0x0273AF40, self);
}
VERIFY(0x02725BE8, destroy);
void noop() { WWHD_FUNC(0x02725BFC, void); }
VERIFY(0x02725BFC, noop);
void initializeSlot(void *self, u32 slot) {
  WWHD_FUNC(0x02725C00, void, self, slot);
  if (slot < 12) {
    call<void>(0x027257DC, at<void>(ea(self) + slot * 0x50040));
    store<u8>(ea(self) + 0x3C0300 + slot, 255);
  }
}
VERIFY(0x02725C00, initializeSlot);
void initialize(void *self) {
  WWHD_FUNC(0x02725C60, void, self);
  for (u32 i = 0; i < 12; ++i)
    call<void>(0x02725C00, self, i);
  store<u8>(ea(self) + 0x3C030C, 0);
  for (u32 i = 0; i < 3; ++i)
    store<u8>(ea(self) + 0x3C030D + i, 0);
}
VERIFY(0x02725C60, initialize);
u32 construct(void *self) {
  WWHD_FUNC(0x02725CE0, u32, self);
  u32 p = ea(self);
  if (!p)
    p = call<u32>(0x0273AD10, 0x3C0340);
  if (p) {
    call<void>(0x028EFFD0, at<void>(p), 12, 0x50040, 0x02725780);
    call<void>(0x02725C60, at<void>(p));
  }
  return p;
}
VERIFY(0x02725CE0, construct);
void clearSlot(void *self, u32 slot) {
  WWHD_FUNC(0x02725D48, void, self, slot);
  if (slot < 12) {
    call<void>(0x027258F4, at<void>(ea(self) + slot * 0x50040));
    store<u8>(ea(self) + 0x3C0300 + slot, 255);
  }
}
VERIFY(0x02725D48, clearSlot);
void clear(void *self) {
  WWHD_FUNC(0x02725DA8, void, self);
  for (u32 i = 0; i < 12; ++i)
    call<void>(0x02725D48, self, i);
  store<u8>(ea(self) + 0x3C030C, 0);
}
VERIFY(0x02725DA8, clear);
void eraseAndNotify(void *self, u32 slot) {
  WWHD_FUNC(0x02725E0C, void, self, slot);
  if (slot >= 12)
    return;
  u32 p = ea(self);
  call<void>(0x02725938, at<void>(p + slot * 0x50040));
  u32 order = p + 0x3C0300;
  for (u32 i = 0; i < 12; ++i) {
    if ((u32)(s32)load<s8>(order + i) == slot) {
      for (u32 j = i; j < 11; ++j)
        store<u8>(order + j, load<u8>(order + j + 1));
      store<u8>(order + 11, 255);
      call<void>(0x027172C8, at<void>(load<u32>(0x101F8344)), slot);
      return;
    }
  }
}
VERIFY(0x02725E0C, eraseAndNotify);
void erase(void *self, u32 slot) {
  WWHD_FUNC(0x02725EE4, void, self, slot);
  if (slot >= 12)
    return;
  u32 p = ea(self);
  call<void>(0x02725938, at<void>(p + slot * 0x50040));
  u32 order = p + 0x3C0300;
  for (u32 i = 0; i < 12; ++i) {
    if ((u32)(s32)load<s8>(order + i) == slot) {
      for (u32 j = i; j < 11; ++j)
        store<u8>(order + j, load<u8>(order + j + 1));
      store<u8>(order + 11, 255);
      return;
    }
  }
}
VERIFY(0x02725EE4, erase);
u32 valid(void *self, u32 slot) {
  WWHD_FUNC(0x02725FAC, u32, self, slot);
  if (slot >= 12)
    return 0;
  return call<u32>(0x0272598C, at<void>(ea(self) + slot * 0x50040));
}
VERIFY(0x02725FAC, valid);
u32 label(void *self, u32 slot) {
  WWHD_FUNC(0x027260DC, u32, self, slot);
  if (slot >= 12)
    return 208;
  return call<u32>(0x027259B4, at<void>(ea(self) + slot * 0x50040));
}
VERIFY(0x027260DC, label);
u32 state(void *self, u32 slot) {
  WWHD_FUNC(0x02726120, u32, self, slot);
  if (slot >= 12)
    return 3;
  return call<u32>(0x027259C4, at<void>(ea(self) + slot * 0x50040));
}
VERIFY(0x02726120, state);
u32 propertyA(void *self, u32 slot) {
  WWHD_FUNC(0x02726164, u32, self, slot);
  if (slot >= 12)
    return 0;
  return call<u32>(0x02725A1C, at<void>(ea(self) + slot * 0x50040));
}
VERIFY(0x02726164, propertyA);
u32 propertyB(void *self, u32 slot) {
  WWHD_FUNC(0x02726188, u32, self, slot);
  if (slot >= 12)
    return 0;
  return call<u32>(0x02725A24, at<void>(ea(self) + slot * 0x50040));
}
VERIFY(0x02726188, propertyB);
void enableAll(void *self) {
  WWHD_FUNC(0x02725FD0, void, self);
  for (u32 i = 0; i < 12; ++i)
    call<void>(0x0272599C, at<void>(ea(self) + i * 0x50040));
}
VERIFY(0x02725FD0, enableAll);
void disableAll(void *self) {
  WWHD_FUNC(0x02726044, void, self);
  for (u32 i = 0; i < 12; ++i)
    call<void>(0x027259A8, at<void>(ea(self) + i * 0x50040));
}
VERIFY(0x02726044, disableAll);
void disableSlot(void *self, u32 slot) {
  WWHD_FUNC(0x02726028, void, self, slot);
  if (slot < 12)
    call<void>(0x027259A8, at<void>(ea(self) + slot * 0x50040));
}
VERIFY(0x02726028, disableSlot);
u32 record(void *self, u32 slot) {
  WWHD_FUNC(0x0272609C, u32, self, slot);
  if (slot >= 12)
    slot = 0;
  return ea(self) + slot * 0x50040;
}
VERIFY(0x0272609C, record);
u32 constRecord(void *self, u32 slot) {
  WWHD_FUNC(0x027260BC, u32, self, slot);
  if (slot >= 12)
    slot = 0;
  return ea(self) + slot * 0x50040;
}
VERIFY(0x027260BC, constRecord);
void setLabel(void *self, u32 slot, u32 value) {
  WWHD_FUNC(0x02726100, void, self, slot, value);
  if (slot < 12)
    call<void>(0x027259BC, at<void>(ea(self) + slot * 0x50040), value);
}
VERIFY(0x02726100, setLabel);
void setState(void *self, u32 slot, u32 value) {
  WWHD_FUNC(0x02726144, void, self, slot, value);
  if (slot < 12)
    call<void>(0x027259CC, at<void>(ea(self) + slot * 0x50040), value);
}
VERIFY(0x02726144, setState);
s32 orderedSlot(void *self, u32 slot) {
  WWHD_FUNC(0x027261AC, s32, self, slot);
  if (slot >= 12)
    return -1;
  return load<s8>(ea(self) + 0x3C0300 + slot);
}
VERIFY(0x027261AC, orderedSlot);
void appendOrder(void *self, u32 slot) {
  WWHD_FUNC(0x027261D0, void, self, slot);
  if (slot >= 12)
    return;
  for (u32 i = 0; i < 12; ++i) {
    s8 v = load<s8>(ea(self) + 0x3C0300 + i);
    if ((u32)(s32)v == slot)
      return;
    if (v == -1) {
      store<u8>(ea(self) + 0x3C0300 + i, slot);
      return;
    }
  }
}
VERIFY(0x027261D0, appendOrder);
s32 firstEmpty(void *self) {
  WWHD_FUNC(0x02726218, s32, self);
  for (u32 i = 0; i < 12; ++i)
    if (call<u32>(0x027259D4, at<void>(ea(self) + i * 0x50040)) == 0)
      return i;
  return -1;
}
VERIFY(0x02726218, firstEmpty);
void setSelection(void *self, u32 value) {
  WWHD_FUNC(0x027262B0, void, self, value);
  store<u8>(ea(self) + 0x3C030C, value);
}
VERIFY(0x027262B0, setSelection);
void copy(void *self, void *other) {
  WWHD_FUNC(0x027262BC, void, self, other);
  for (u32 i = 0; i < 12; ++i) {
    u32 src = call<u32>(0x027260BC, other, i);
    call<void>(0x0272597C, at<void>(ea(self) + i * 0x50040), at<void>(src));
  }
  for (u32 i = 0; i < 12; ++i)
    store<u8>(ea(self) + 0x3C0300 + i, load<u8>(ea(other) + 0x3C0300 + i));
  store<u8>(ea(self) + 0x3C030C, load<u8>(ea(other) + 0x3C030C));
}
VERIFY(0x027262BC, copy);
void staticInit() {
  WWHD_FUNC(0x02726360, void);
  for (u32 i = 0; i < 4; ++i)
    store<u32>(0x1049F820 + i * 4, 0);
  call<void>(0x028F026C, at<void>(0x101F8598));
  store<f32>(0x1049F804, load<f32>(0x10140C28));
  store<f32>(0x1049F808, load<f32>(0x10140C2C));
  call<void>(0x028ED6F8, at<void>(0x1049F81C));
  call<void>(0x028F026C, at<void>(0x101F85A4));
  call<void>(0x028EAB2C, at<void>(0x1049F81D));
  call<void>(0x028F026C, at<void>(0x101F85B0));
  store<f32>(0x1049F80C, load<f32>(0x10140C30));
  store<f32>(0x1049F814, load<f32>(0x10140C34));
  store<f32>(0x1049F810, load<f32>(0x10140C30));
  store<f32>(0x1049F818, load<f32>(0x10140C34));
}
VERIFY(0x02726360, staticInit);
} // namespace picture_store_cpp
