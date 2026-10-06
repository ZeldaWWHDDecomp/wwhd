/* HD shop selection, item/camera management and cursor rendering.
 * Native inventory025BB5FC..025BD5FF; preceding sea initializer excluded. */
#include "gabi.h"
using namespace gabi;
namespace d_shop_cpp {
static void copyVector(u32 dst, u32 src) {
  store<u32>(dst, load<u32>(src));
  store<u32>(dst + 4, load<u32>(src + 4));
  store<u32>(dst + 8, load<u32>(src + 8));
}
u32 soldOutAll(void *self) {
  WWHD_FUNC(0x025BCF44, u32, self);
  u32 p = ea(self);
  s16 count = load<s16>(p + 0x40);
  for (s16 i = 0; i < count; ++i)
    if (load<u8>(p + 0x28 + (s32)i) != 1)
      return 0;
  return 1;
}
VERIFY(0x025BCF44, soldOutAll);
void setItemList(void *self) {
  WWHD_FUNC(0x025BCF84, void, self);
  u32 p = ea(self);
  s16 index = load<s16>(p + 0x42);
  store<u32>(p + 0x24, load<u32>(0x101EB674 + (u32)((s32)index * 4)));
}
VERIFY(0x025BCF84, setItemList);
void setItemListExplicit(void *self, u32 list) {
  WWHD_FUNC(0x025BCFA0, void, self, list);
  store<u32>(ea(self) + 0x24, list);
}
VERIFY(0x025BCFA0, setItemListExplicit);
u32 selectedItemNo(void *self) {
  WWHD_FUNC(0x025BD22C, u32, self);
  u32 p = ea(self);
  s16 index = load<s16>(p);
  u32 entry = load<u32>(load<u32>(p + 0x24) + (u32)((s32)index * 4));
  return load<u8>(load<u32>(entry) + 3);
}
VERIFY(0x025BD22C, selectedItemNo);
u32 itemNo(void *self, s32 index) {
  WWHD_FUNC(0x025BD248, u32, self, index);
  u32 entry = load<u32>(load<u32>(ea(self) + 0x24) + (u32)index * 4);
  return load<u8>(load<u32>(entry) + 3);
}
VERIFY(0x025BD248, itemNo);
u32 selectedShowMsg(void *self) {
  WWHD_FUNC(0x025BD260, u32, self);
  u32 p = ea(self);
  s16 index = load<s16>(p);
  return load<u32>(load<u32>(load<u32>(p + 0x24) + (u32)((s32)index * 4)) + 4);
}
VERIFY(0x025BD260, selectedShowMsg);
u32 selectedBuyMsg(void *self) {
  WWHD_FUNC(0x025BD278, u32, self);
  u32 p = ea(self);
  s16 index = load<s16>(p);
  return load<u32>(load<u32>(load<u32>(p + 0x24) + (u32)((s32)index * 4)) + 8);
}
VERIFY(0x025BD278, selectedBuyMsg);
void cursorSetPos(void *self, void *pos) {
  WWHD_FUNC(0x025BD31C, void, self, pos);
  copyVector(ea(self), ea(pos));
}
VERIFY(0x025BD31C, cursorSetPos);
u32 zoomItem(void *self, void *pos) {
  WWHD_FUNC(0x025BCC7C, u32, self, pos);
  store<u16>(ea(self) + 0x3C, 1);
  copyVector(ea(self) + 0x30, ea(pos));
  return 1;
}
VERIFY(0x025BCC7C, zoomItem);
void staticInit() {
  WWHD_FUNC(0x025BD56C, void);
  store<u32>(0x1047C994, 0);
  store<u32>(0x1047C98C, 0);
  store<u32>(0x1047C998, 0);
  store<u32>(0x1047C990, 0);
  call<void>(0x028F026C, at<void>(0x101EB818));
  store<f32>(0x1047C980, load<f32>(0x10054610));
  store<f32>(0x1047C984, load<f32>(0x10054614));
  call<void>(0x028ED6F8, at<void>(0x1047C988));
  call<void>(0x028F026C, at<void>(0x101EB824));
  call<void>(0x028EAB2C, at<void>(0x1047C989));
  call<void>(0x028F026C, at<void>(0x101EB830));
}
VERIFY(0x025BD56C, staticInit);
s16 nextSelect(s32 direction, void *self) {
  WWHD_FUNC(0x025BB5FC, s16, direction, self);
  u32 p = ea(self);
  s16 data = load<s16>(p + 0x42), original = load<s16>(p), index = original;
  u32 table = load<u32>(0x101EB7B8 + (u32)((s32)data * 4));
  for (;;) {
    u32 entry = table + (u32)((s32)index * 8) + 8;
    if (direction >= 0 && direction <= 3)
      index = load<s16>(entry + (u32)direction * 2);
    if (index == original ||
        load<u32>(p + 4 + (u32)((s32)index * 4)) != 0xFFFFFFFFu)
      return index;
  }
}
VERIFY(0x025BB5FC, nextSelect);
static u32 findItem(u32 id) {
  Local<be<u32>> query;
  *query = id;
  if (id == 0xFFFFFFFFu)
    return 0;
  return call<u32>(0x025D5218, at<void>(0x025E1234), query.get());
}
void hideSelected(void *self) {
  WWHD_FUNC(0x025BCD90, void, self);
  u32 p = ea(self);
  s16 index = load<s16>(p);
  if (index < 0)
    return;
  u32 item = findItem(load<u32>(p + 4 + (u32)((s32)index * 4)));
  if (item)
    call<void>(0x021842B8, at<void>(item));
  store<u16>(p + 0x3E, 1);
}
VERIFY(0x025BCD90, hideSelected);
void soldOut(void *self, s32 index) {
  WWHD_FUNC(0x025BCECC, void, self, index);
  u32 p = ea(self), item = findItem(load<u32>(p + 4 + (u32)index * 4));
  if (item)
    call<void>(0x021842B8, at<void>(item));
  store<u8>(p + 0x28 + (u32)index, 1);
}
VERIFY(0x025BCECC, soldOut);
void cursorAnimation(void *self) {
  WWHD_FUNC(0x025BD290, void, self);
  u32 p = ea(self);
  call<void>(0x025E742C, at<void>(p + 0x20));
  s16 countdown = load<s16>(p + 0xA4);
  store<u16>(p + 0xA4, (u16)(countdown - 1));
  if (countdown > 0)
    return;
  s32 delay = ftoi(call<f32>(0x020198D8, load<f32>(0x100545E4)));
  f32 upper = load<f32>(p + 0x98), lower = load<f32>(p + 0x9C),
      current = load<f32>(p + 0xA0);
  f32 compare = fmsubs(fadds_ppc(upper, lower), load<f32>(0x10054604), current);
  store<u16>(p + 0xA4, (u32)delay + 15);
  store<f32>(p + 0xA0, compare >= 0 ? upper : lower);
}
VERIFY(0x025BD290, cursorAnimation);
void cameraBackup(void *self) {
  WWHD_FUNC(0x025BC5B0, void, self);
  u32 camera = load<u32>(call<u32>(0x025200D4) + 0x5AF8);
  if (!camera)
    return;
  copyVector(ea(self) + 0x2C, camera + 0xE8);
  copyVector(ea(self) + 0x38, camera + 0xDC);
  store<f32>(ea(self) + 0x44, load<f32>(camera + 0xD4));
}
VERIFY(0x025BC5B0, cameraBackup);
void cameraMove(void *self) {
  WWHD_FUNC(0x025BC714, void, self);
  u32 p = ea(self);
  s16 flag = load<s16>(p + 2);
  if (!flag)
    return;
  u32 receiver = p + (s32)load<s16>(p), target;
  if (flag < 0)
    target = load<u32>(p + 4);
  else
    target = load<u32>(load<u32>(receiver + (s32)load<s16>(p + 6)) +
                       (u32)((s32)flag * 8) + 4);
  call_ptr<void>(target, at<void>(receiver));
}
VERIFY(0x025BC714, cameraMove);

struct Vector {
  be<f32> x, y, z;
};
static u32 activeCamera() {
  s8 index = load<s8>(call<u32>(0x025200D4) + 0x5B30);
  return load<u32>(call<u32>(0x025200D4) + 0x5AF8 + (u32)((s32)index * 52));
}
u32 talkCameraAction(void *self) {
  WWHD_FUNC(0x025BC38C, u32, self);
  u32 p = ea(self), camera = activeCamera() + 0x248;
  call<void>(0x02514F44, at<void>(camera));
  call<void>(0x02515280, at<void>(camera), 1);
  Local<Vector> center, eye;
  copyVector(ea(center.get()), p + 0x10);
  copyVector(ea(eye.get()), p + 0x1C);
  call<void>(0x02514F88, at<void>(camera), center.get(), eye.get(), 0,
             load<f32>(p + 0x28));
  return 1;
}
VERIFY(0x025BC38C, talkCameraAction);
u32 normalCameraAction(void *self) {
  WWHD_FUNC(0x025BC4DC, u32, self);
  u32 p = ea(self), camera = activeCamera() + 0x248;
  call<void>(0x02514F44, at<void>(camera));
  call<void>(0x02515280, at<void>(camera), 1);
  f32 x = load<f32>(0x100545E8), y = load<f32>(0x100545EC),
      z = load<f32>(0x100545F0), ez = load<f32>(0x100545F4),
      fovy = load<f32>(0x100545D8);
  store<f32>(p + 0x20, y);
  store<f32>(p + 0x14, y);
  store<f32>(p + 0x28, fovy);
  store<f32>(p + 0x18, z);
  store<f32>(p + 0x10, x);
  store<f32>(p + 0x1C, x);
  store<f32>(p + 0x24, ez);
  Local<Vector> center, eye;
  center->x = x;
  center->y = y;
  center->z = z;
  eye->x = x;
  eye->y = y;
  eye->z = ez;
  call<void>(0x02514F88, at<void>(camera), center.get(), eye.get(), 0, fovy);
  return 1;
}
VERIFY(0x025BC4DC, normalCameraAction);
void cameraReset(void *self) {
  WWHD_FUNC(0x025BC620, void, self);
  u32 p = ea(self), camera = activeCamera() + 0x248;
  Local<Vector> center, eye;
  copyVector(ea(center.get()), p + 0x2C);
  copyVector(ea(eye.get()), p + 0x38);
  call<void>(0x02514F88, at<void>(camera), center.get(), eye.get(), 0,
             load<f32>(p + 0x44));
  call<void>(0x02514F44, at<void>(camera));
  copyVector(ea(eye.get()), p + 0x38);
  copyVector(ea(center.get()), p + 0x2C);
  call<void>(0x025151BC, at<void>(camera), center.get(), eye.get(), 0,
             load<f32>(p + 0x44));
  store<u16>(p, 0);
  store<u32>(p + 4, 0);
  store<u16>(p + 2, 0);
}
VERIFY(0x025BC620, cameraReset);
void *cameraConstruct(void *self) {
  WWHD_FUNC(0x025BBDF0, void *, self);
  u32 p = ea(self);
  if (!p)
    p = call<u32>(0x0273AD10, 0x50);
  if (!p)
    return nullptr;
  call<void>(0x028F521C, at<void>(p), 8);
  call<void>(0x028F521C, at<void>(p + 8), 8);
  f32 zero = load<f32>(0x100545D0), back = load<f32>(0x100545D4),
      fovy = load<f32>(0x100545D8);
  store<f32>(p + 0x14, zero);
  store<u16>(p + 0x4C, 0xFFFF);
  store<f32>(p + 0x20, zero);
  store<u16>(p + 0x4E, 0);
  store<u16>(p + 0x4A, 0);
  store<u16>(p + 0x48, 0);
  store<f32>(p + 0x24, back);
  store<f32>(p + 0x10, zero);
  store<f32>(p + 0x28, fovy);
  store<f32>(p + 0x18, zero);
  store<f32>(p + 0x44, zero);
  store<f32>(p + 0x1C, zero);
  return at<void>(p);
}
VERIFY(0x025BBDF0, cameraConstruct);
void *itemsConstruct(void *self) {
  WWHD_FUNC(0x025BC760, void *, self);
  u32 p = ea(self);
  if (!p)
    p = call<u32>(0x0273AD10, 0x44);
  if (!p)
    return nullptr;
  store<u16>(p, 0);
  call<void>(0x028F521C, at<void>(p + 4), 0x20);
  store<u32>(p + 0x24, 0);
  call<void>(0x028F521C, at<void>(p + 0x28), 8);
  store<u16>(p + 0x3C, 0);
  store<u16>(p + 0x3E, 0);
  store<u16>(p, 0xFFFF);
  store<u16>(p + 0x42, 0);
  store<u16>(p + 0x40, 0);
  for (u32 i = 0; i < 8; ++i) {
    store<u32>(p + 4 + i * 4, 0xFFFFFFFFu);
    store<u8>(p + 0x28 + i, 0);
  }
  return at<void>(p);
}
VERIFY(0x025BC760, itemsConstruct);
u32 normalCameraInit(void *self) {
  WWHD_FUNC(0x025BC438, u32, self);
  u32 p = ea(self), player = load<u32>(call<u32>(0x025200D4) + 0x5B2C);
  store<u32>(player + 0x3B8, load<u32>(player + 0x3B8) | 0x08000000);
  f32 x = load<f32>(0x100545E8), y = load<f32>(0x100545EC),
      z = load<f32>(0x100545F0), ez = load<f32>(0x100545F4),
      fovy = load<f32>(0x100545D8);
  store<u16>(p, 0);
  store<u32>(p + 4, 0x025BC4DC);
  store<f32>(p + 0x14, y);
  store<f32>(p + 0x18, z);
  store<f32>(p + 0x10, x);
  store<f32>(p + 0x24, ez);
  store<u16>(p + 0x4C, 0xFFFF);
  store<u16>(p + 2, 0xFFFF);
  store<f32>(p + 0x28, fovy);
  store<f32>(p + 0x1C, x);
  store<f32>(p + 0x20, y);
  return 1;
}
VERIFY(0x025BC438, normalCameraInit);

static u32 zeroPosition(u32 dst) {
  if (!dst)
    dst = call<u32>(0x0273AD10, 12);
  if (dst)
    for (u32 i = 0; i < 3; ++i)
      store<f32>(dst + i * 4, load<f32>(0x101FFBA8 + i * 4));
  return dst;
}
u32 selectedPosition(void *self, void *out) {
  WWHD_FUNC(0x025BCFA8, u32, self, out);
  u32 p = ea(self), dst = ea(out);
  s16 index = load<s16>(p);
  if (index < 0)
    return zeroPosition(dst);
  u32 item = findItem(load<u32>(p + 4 + (u32)((s32)index * 4)));
  if (!item)
    return zeroPosition(dst);
  Local<Vector> center;
  call<void>(0x02483FBC, at<void>(item), center.get());
  u32 pos = call<u32>(0x02483FB4, at<void>(item));
  return call<u32>(0x0201AD78, at<void>(pos), out, center.get());
}
VERIFY(0x025BCFA8, selectedPosition);
u32 selectedBasePosition(void *self, void *out) {
  WWHD_FUNC(0x025BD0EC, u32, self, out);
  u32 p = ea(self), dst = ea(out);
  s16 index = load<s16>(p);
  if (index < 0)
    return zeroPosition(dst);
  u32 item = findItem(load<u32>(p + 4 + (u32)((s32)index * 4)));
  if (!item)
    return zeroPosition(dst);
  Local<Vector> center;
  call<void>(0x02483FBC, at<void>(item), center.get());
  return call<u32>(0x0201AD78, at<void>(item + 0x2EC), out, center.get());
}
VERIFY(0x025BD0EC, selectedBasePosition);
u32 itemMove(void *self) {
  WWHD_FUNC(0x025BCCA4, u32, self);
  u32 p = ea(self);
  for (s32 i = 0; i < load<s16>(p + 0x40); ++i) {
    if (load<u32>(p + 4 + (u32)i * 4) != 0xFFFFFFFFu) {
      if (i == load<s16>(p))
        call<void>(0x025BCA2C, self, i);
      else
        call<void>(0x025BCBAC, self, i);
    }
    if (load<u8>(p + 0x28 + (s32)(s16)i) == 1) {
      u32 item = findItem(load<u32>(p + 4 + (u32)i * 4));
      if (item)
        call<void>(0x021842B8, at<void>(item));
    }
  }
  store<u16>(p + 0x3C, 0);
  return 1;
}
VERIFY(0x025BCCA4, itemMove);
void showItems(void *self) {
  WWHD_FUNC(0x025BCE04, void, self);
  u32 p = ea(self);
  for (s32 i = 0; i < load<s16>(p + 0x40); ++i) {
    u32 item = findItem(load<u32>(p + 4 + (u32)i * 4));
    if (!item || load<u8>(p + 0x28 + (u32)i) == 1)
      continue;
    call<void>(0x021842C8, at<void>(item));
    // Source components are captured together before stores, as in the native
    // copy.
    u32 z = load<u32>(item + 0x2F4), x = load<u32>(item + 0x2EC),
        y = load<u32>(item + 0x2F0);
    store<u32>(item + 0x314, x);
    store<u32>(item + 0x318, y);
    store<u32>(item + 0x31C, z);
    u32 rotation = call<u32>(0x02483FAC, at<void>(item));
    store<u16>(rotation + 2, load<u16>(item + 0x2FA));
  }
  store<u16>(p + 0x3E, 0);
}
VERIFY(0x025BCE04, showItems);

u32 maxCheck(s32 item, s32 unused) {
  WWHD_FUNC(0x025BB974, u32, item, unused);
  if (item == 130 || item == 131)
    return !call<u32>(0x025B6E90, at<void>(load<u32>(0x101F84DC) + 0x96));
  if (item == 140)
    return 0;
  u32 save;
  if ((u32)item - 16 < 3) {
    save = load<u32>(0x101F84DC);
    return load<u8>(save + 0x89) == load<u8>(save + 0x8F);
  }
  if ((u32)item - 11 < 4) {
    save = load<u32>(0x101F84DC);
    return load<u8>(save + 0x8A) == load<u8>(save + 0x90);
  }
  return 0;
}
VERIFY(0x025BB974, maxCheck);
u32 boughtError(void *self, s32 buy, s32 cost) {
  WWHD_FUNC(0x025BBA1C, u32, self, buy, cost);
  u32 p = ea(self);
  s16 index = load<s16>(p);
  u32 data = load<u32>(load<u32>(load<u32>(p + 0x24) + (u32)((s32)index * 4)));
  u8 flags = load<u8>(data + 0xC);
  u32 item = load<u32>(data), error = 0;
  if (flags & 1) {
    u32 prerequisite = load<u32>(data + 8);
    if (prerequisite == 39) {
      if (load<u8>(load<u32>(0x101F84DC) + 0x68) == 255)
        error = 1;
    } else if (!call<u32>(0x02520C0C, prerequisite & 255))
      error = 1;
  }
  if ((flags & 2) && call<u32>(0x02520C0C, (u32)load<u8>(data + 7)))
    error |= 2;
  if (flags & 8) {
    u32 save = load<u32>(0x101F84DC), count = 0;
    // Four bottle inventory slots are14..17. Generic inlined item lookup's
    // other slot ranges cannot be selected by this bounded loop.
    for (u32 i = 0; i < 4; ++i)
      if (load<u8>(save + 0x6A + i) != 255)
        ++count;
    if (!count)
      error |= 8;
  }
  if ((flags & 16) &&
      !call<u32>(0x025B5C54, at<void>(load<u32>(0x101F84DC) + 0x5C)))
    error |= 16;
  if ((flags & 4) && call<u32>(0x025BB974, (s32)item, 0))
    error |= 4;
  if (flags & 32) {
    u32 rupees = load<u16>(load<u32>(0x101F84DC) + 0x24);
    if (cost == -1)
      cost = 0;
    if ((s32)rupees < cost)
      error |= 32;
    else if (buy == 1 && !error) {
      u32 play = call<u32>(0x025200D4);
      store<u32>(play + 0x5B48, load<u32>(play + 0x5B48) - (u32)cost);
      call<void>(0x0254DA38, item & 255);
      return error;
    }
  }
  if (buy == 1 && !error)
    call<void>(0x0254DA38, item & 255);
  return error;
}
VERIFY(0x025BBA1C, boughtError);
void *cursorConstruct(void *self, void *modelData, void *animation, f32 scale) {
  WWHD_FUNC(0x025BBC54, void *, self, modelData, animation, scale);
  u32 p = ea(self);
  if (!p)
    p = call<u32>(0x0273AD10, 0xB8);
  if (!p)
    return nullptr;
  call<void>(0x025E80D0, at<void>(p + 0x20));
  for (u32 i = 0; i < 4; ++i)
    store<u32>(p + 0xC + i * 4,
               call<u32>(0x025E38E0, modelData, 0, 0x11020203));
  store<u32>(p + 0x1C, ea(animation));
  f32 one = load<f32>(0x100545C4);
  if (!call<u32>(0x025E8154, at<void>(p + 0x20), modelData, animation, 1, 2, 0,
                 -1, 0, 0, one))
    store<u32>(p + 0x1C, 0);
  store<f32>(p + 0xA8, scale);
  store<u8>(p + 0xB4, 0);
  f32 upper = load<f32>(0x100545C8), lower = load<f32>(0x100545CC);
  store<f32>(p + 0x98, upper);
  store<f32>(p + 0x9C, lower);
  store<f32>(p + 0xA0, upper);
  store<f32>(p + 0xAC, one);
  store<u16>(p + 0xA4, 15);
  store<f32>(p + 0xB0, one);
  return at<void>(p);
}
VERIFY(0x025BBC54, cursorConstruct);
void *cursorCreate(void *modelData, void *animation, f32 scale) {
  WWHD_FUNC(0x025BBD7C, void *, modelData, animation, scale);
  u32 p = call<u32>(0x025BBC54, (void *)nullptr, modelData, animation, scale);
  if (!p)
    return nullptr;
  for (u32 i = 0; i < 4; ++i)
    if (!load<u32>(p + 0xC + i * 4))
      return nullptr;
  if (!load<u32>(p + 0x1C))
    return nullptr;
  return at<void>(p);
}
VERIFY(0x025BBD7C, cursorCreate);

u32 cameraActionInit(void *self) {
  WWHD_FUNC(0x025BBE98, u32, self);
  u32 p = ea(self), player = load<u32>(call<u32>(0x025200D4) + 0x5B2C);
  store<u32>(player + 0x3B8, load<u32>(player + 0x3B8) | 0x08000000);
  s16 index = load<s16>(p + 0x4E);
  u32 slot = 0x101EB7DC + (u32)((s32)index * 4);
  store<u16>(p + 2, 0xFFFF);
  store<u16>(p, 0);
  store<u32>(p + 4, 0x025BBF4C);
  u32 data = load<u32>(slot);
  for (u32 i = 0; i < 3; ++i)
    store<f32>(p + 0x10 + i * 4, load<f32>(data + i * 4));
  data = load<u32>(slot);
  for (u32 i = 0; i < 3; ++i)
    store<f32>(p + 0x1C + i * 4, load<f32>(data + 0xC + i * 4));
  data = load<u32>(slot);
  f32 fovy = load<f32>(data + 0x18);
  store<u16>(p + 0x4C, 0xFFFF);
  store<f32>(p + 0x28, fovy);
  return 1;
}
VERIFY(0x025BBE98, cameraActionInit);
u32 cameraAction(void *self) {
  WWHD_FUNC(0x025BBF4C, u32, self);
  u32 p = ea(self), camera = activeCamera() + 0x248;
  call<void>(0x02514F44, at<void>(camera));
  call<void>(0x02515280, at<void>(camera), 1);
  s16 index = load<s16>(p + 0x4E), selected = load<s16>(p + 0x4C);
  u32 data = load<u32>(0x101EB7DC + (u32)((s32)index * 4));
  if (selected >= 0)
    data += 0x1C;
  Local<Vector> desiredCenter, desiredEye, center, eye;
  for (u32 i = 0; i < 3; ++i)
    store<f32>(ea(desiredCenter.get()) + i * 4, load<f32>(data + i * 4));
  for (u32 i = 0; i < 3; ++i)
    store<f32>(ea(desiredEye.get()) + i * 4, load<f32>(data + 0xC + i * 4));
  f32 fraction = load<f32>(0x100545DC), limit = load<f32>(0x100545E0),
      fovy = load<f32>(data + 0x18);
  call<void>(0x0200F164, at<void>(p + 0x10), desiredCenter.get(), fraction,
             limit);
  call<void>(0x0200F164, at<void>(p + 0x1C), desiredEye.get(), fraction, limit);
  call<void>(0x0200ED84, at<void>(p + 0x28), fovy, fraction,
             load<f32>(0x100545E4));
  copyVector(ea(center.get()), p + 0x10);
  copyVector(ea(eye.get()), p + 0x1C);
  call<void>(0x02514F88, at<void>(camera), center.get(), eye.get(), 0,
             load<f32>(p + 0x28));
  return 1;
}
VERIFY(0x025BBF4C, cameraAction);
u32 talkCameraInit(void *self, void *actor, void *desiredCenter,
                   void *desiredEye, f32 fovy) {
  WWHD_FUNC(0x025BC1B0, u32, self, actor, desiredCenter, desiredEye, fovy);
  u32 p = ea(self), a = ea(actor);
  if (load<s16>(p + 2) == -1 && load<s16>(p) == 0 &&
      load<u32>(p + 4) == 0x025BC38C)
    return 1;
  u32 player = load<u32>(call<u32>(0x025200D4) + 0x5B2C);
  store<u32>(player + 0x3B8, load<u32>(player + 0x3B8) | 0x08000000);
  store<u16>(p, 0);
  store<u16>(p + 2, 0xFFFF);
  store<u32>(p + 4, 0x025BC38C);
  call<void>(0x028E93CC, at<void>(0x1048D0CC), load<f32>(a + 0x390),
             load<f32>(a + 0x394), load<f32>(a + 0x398));
  call<void>(0x025F1C28, at<void>(0x1048D0CC), (s32)load<s16>(a + 0x322));
  Local<Vector> center, eye;
  call<void>(0x028E8F64, at<void>(0x1048D0CC), desiredCenter, center.get());
  call<void>(0x028E8F64, at<void>(0x1048D0CC), desiredEye, eye.get());
  copyVector(p + 0x10, ea(center.get()));
  copyVector(p + 0x1C, ea(eye.get()));
  store<f32>(p + 0x28, fovy);
  store<u16>(p + 0x4C, 0xFFFF);
  return 1;
}
VERIFY(0x025BC1B0, talkCameraInit);

u32 waitItem(void *self, s32 index) {
  WWHD_FUNC(0x025BCBAC, u32, self, index);
  u32 item = findItem(load<u32>(ea(self) + 4 + (u32)index * 4));
  if (!item)
    return 0;
  u32 rotation = call<u32>(0x02483FAC, at<void>(item));
  call<void>(0x0200F378, at<void>(rotation + 2), (s32)load<s16>(item + 0x2FA),
             4, 0x800, 0x80);
  u32 pos = call<u32>(0x02483FB4, at<void>(item));
  Local<Vector> home;
  copyVector(ea(home.get()), item + 0x2EC);
  call<void>(0x0200F164, at<void>(pos), home.get(), load<f32>(0x10054604),
             load<f32>(0x100545E0));
  return 1;
}
VERIFY(0x025BCBAC, waitItem);
u32 selectItem(void *self, s32 index) {
  WWHD_FUNC(0x025BCA2C, u32, self, index);
  u32 p = ea(self), item = findItem(load<u32>(p + 4 + (u32)index * 4));
  if (!item)
    return 1;
  u32 rotation = call<u32>(0x02483FAC, at<void>(item));
  u32 pos = call<u32>(0x02483FB4, at<void>(item));
  Local<Vector> home, target, center, difference;
  copyVector(ea(home.get()), item + 0x2EC);
  copyVector(ea(target.get()), p + 0x30);
  call<void>(0x02483FBC, at<void>(item), center.get());
  call<void>(0x028E8DAC, target.get(), center.get(), target.get());
  bool zoom = load<s16>(p + 0x3C) == 1;
  call<void>(0x0201ADE0, target.get(), difference.get(), home.get());
  f64 length = call<f64>(0x028E8DD0, difference.get());
  f64 distance = call<f64>(0x028F4384, length);
  f32 maxStep = (f32)(distance * load<f32>(zoom ? 0x100545FC : 0x10054600));
  call<void>(0x0200F164, at<void>(pos), zoom ? target.get() : home.get(),
             load<f32>(0x10054604), maxStep);
  store<u16>(rotation + 2, (u32)(s32)load<s16>(rotation + 2) + 0x400);
  return 1;
}
VERIFY(0x025BCA2C, selectItem);
u32 triggerCheck(void *stick, void *shop, void *message, u32 callback,
                 void *arg) {
  WWHD_FUNC(0x025BB698, u32, stick, shop, message, callback, arg);
  u32 m = load<u32>(0x101F4B5C), state = call<u32>(0x025F795C, at<void>(m));
  if (load<u8>(m + 0x922) || (state != 7 && state != 14 && state != 15))
    return 0;
  u32 p = ea(shop);
  s16 original = load<s16>(p), data = load<s16>(p + 0x42);
  u32 list = load<u32>(p + 0x24);
  call<void>(0x0258873C, stick);
  s32 left = call<s32>(0x025BB5FC, 0, shop),
      right = call<s32>(0x025BB5FC, 1, shop);
  s32 up = call<s32>(0x025BB5FC, 2, shop),
      down = call<s32>(0x025BB5FC, 3, shop);
  s32 next = left;
  bool changed = false;
  if (original != left &&
      (call<u32>(0x02588A0C, stick) || call<u32>(0x020077BC, 0)))
    changed = true;
  else if (original != right &&
           (call<u32>(0x02588A7C, stick) || call<u32>(0x020077E8, 0))) {
    next = right;
    changed = true;
  }
  if (original != up &&
      (call<u32>(0x02588AEC, stick) || call<u32>(0x02007764, 0))) {
    next = up;
    changed = true;
  } else if (original != down &&
             (call<u32>(0x02588B5C, stick) || call<u32>(0x02007790, 0))) {
    next = down;
    changed = true;
  }
  if (!changed)
    return 0;
  u32 text;
  if (next < 0) {
    text = callback ? call_ptr<u32>(callback, arg)
                    : load<u32>(0x101EB4AC + (u32)((s32)data * 4));
    store<u32>(ea(message), text);
    store<u32>(m + 0x924, text);
  } else {
    if (load<u32>(m + 0x924))
      return 0;
    u32 entry = load<u32>(list + (u32)next * 4);
    text = load<u32>(entry + (load<u8>(p + 0x28 + (u32)next) == 1 ? 0xC : 4));
    store<u32>(ea(message), text);
  }
  call<void>(0x025F74D0, at<void>(m), 15);
  store<u8>(m + 0x922, 1);
  call<void>(0x025F7DB0, at<void>(m), load<u32>(ea(message)), 0);
  store<u16>(p, (u32)next);
  call<void>(0x025E1988, 0x80E);
  return 1;
}
VERIFY(0x025BB698, triggerCheck);

struct Angles {
  be<s16> x, y, z;
};
void createItems(void *self, s32 count, s32 room) {
  WWHD_FUNC(0x025BC810, void, self, count, room);
  u32 p = ea(self);
  s16 data = load<s16>(p + 0x42);
  Local<Angles> angle;
  angle->x = load<s16>(0x101FFB14);
  angle->y = load<s16>(0x101FFB16);
  angle->z = load<s16>(0x101FFB18);
  auto itemAt = [&](s16 idx, u32 which) {
    u32 entry =
        load<u32>(load<u32>(0x101EB674 + (u32)((s32)idx * 4)) + which * 4);
    return load<u32>(load<u32>(entry));
  };
  if (data == 0) {
    Local<Vector> pos;
    copyVector(ea(pos.get()), load<u32>(0x101EB974));
    call<void>(0x025B8B94, at<void>(load<u32>(0x101F84DC) + 0x644), 0xD04);
    call<void>(0x025B8B94, at<void>(load<u32>(0x101F84DC) + 0x644), 0xD02);
    store<u32>(p + 4,
               call<u32>(0x025D87E4, pos.get(), itemAt(load<s16>(p + 0x42), 0),
                         angle.get(), room, 0, 0));
    f32 spacing = load<f32>(0x100545F8);
    pos->x = fadds_ppc(pos->x, spacing);
    if (call<u32>(0x025B8B94, at<void>(load<u32>(0x101F84DC) + 0x644), 0xD04)) {
      store<u32>(p + 8, call<u32>(0x025D87E4, pos.get(),
                                  itemAt(load<s16>(p + 0x42), 1), angle.get(),
                                  room, 0, 0));
      pos->x = fadds_ppc(pos->x, spacing);
    }
    if (call<u32>(0x025B8B94, at<void>(load<u32>(0x101F84DC) + 0x644), 0xD02))
      store<u32>(p + 0xC, call<u32>(0x025D87E4, pos.get(),
                                    itemAt(load<s16>(p + 0x42), 2), angle.get(),
                                    room, 0, 0));
  } else
    for (s32 i = 0; i < count; ++i) {
      s16 idx = load<s16>(p + 0x42);
      u32 list = load<u32>(0x101EB674 + (u32)((s32)idx * 4));
      u32 entry = load<u32>(list + (u32)i * 4),
          positions = load<u32>(0x101EB974 + (u32)((s32)idx * 4));
      u32 item = load<u32>(load<u32>(entry));
      store<u32>(p + 4 + (u32)i * 4,
                 call<u32>(0x025D87E4, at<void>(positions + (u32)i * 12), item,
                           angle.get(), room, 0, 0));
    }
  store<u16>(p + 0x40, (u32)count);
}
VERIFY(0x025BC810, createItems);
void cursorDraw(void *self) {
  WWHD_FUNC(0x025BD338, void, self);
  u32 p = ea(self), camera = load<u32>(call<u32>(0x025200D4) + 0x5AF8);
  s32 yaw =
      call<s32>(0x0200F93C, at<void>(camera + 0xE8), at<void>(camera + 0xDC));
  s16 pitch = (s16)(0u - (u32)call<s32>(0x0200F974, at<void>(camera + 0xE8),
                                        at<void>(camera + 0xDC)));
  if (!load<u8>(p + 0xB4))
    return;
  f32 mid = fmsubs(fadds_ppc(load<f32>(p + 0x98), load<f32>(p + 0x9C)),
                   load<f32>(0x10054604), load<f32>(p + 0xA0));
  f32 scaleY = mid >= 0 ? load<f32>(p + 0xB0) : load<f32>(p + 0xAC);
  store<u32>(0x104B4634, load<u32>(call<u32>(0x025200D4) + 0x5D84));
  u32 second = load<u32>(call<u32>(0x025200D4) + 0x5D88);
  f32 zero = load<f32>(0x100545D0), sqrtTwo = load<f32>(0x10054608);
  store<u32>(0x104B4638, second);
  for (u32 i = 0; i < 4; ++i) {
    call<void>(0x028E93CC, at<void>(0x1048D0CC), load<f32>(p), load<f32>(p + 4),
               load<f32>(p + 8));
    call<void>(0x025F19F8, at<void>(0x1048D0CC), (s32)pitch, yaw, 0);
    call<void>(0x025F1C5C, at<void>(0x1048D0CC),
               (s32)load<s16>(0x101EB810 + i * 2));
    call<void>(0x025F24E0, zero, fmuls_ppc(load<f32>(p + 0xA0), sqrtTwo), zero);
    f32 scale = load<f32>(p + 0xA8);
    call<void>(0x025F2518, scale, fmuls_ppc(scale, scaleY), scale);
    f32 matrix[12];
    for (u32 j = 0; j < 12; ++j)
      matrix[j] = load<f32>(0x1048D0CC + j * 4);
    u32 model = load<u32>(p + 0xC + i * 4);
    for (u32 j = 0; j < 12; ++j)
      store<f32>(model + 0xC8 + j * 4, matrix[j]);
  }
  u32 model = load<u32>(p + 0xC);
  call<void>(0x025E83FC, at<void>(p + 0x20), at<void>(load<u32>(model + 0xAC)),
             load<f32>(p + 0x24));
  for (u32 i = 0; i < 4; ++i)
    call<void>(0x025E2DE0, at<void>(load<u32>(p + 0xC + i * 4)), 0);
  store<u32>(0x104B4634, load<u32>(call<u32>(0x025200D4) + 0x5D78));
  store<u32>(0x104B4638, load<u32>(call<u32>(0x025200D4) + 0x5D7C));
}
VERIFY(0x025BD338, cursorDraw);

} // namespace d_shop_cpp
