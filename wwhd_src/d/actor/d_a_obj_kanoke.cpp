/** Earth Temple coffin, derived from HD disassembly; */
#include "d/actor/d_a_obj_kanoke.h"
#include <cmath>
namespace Kanoke {
template <class T> T *at(u32 p, u32 off = 0) { return gabi::at<T>(p + off); }
template <class T> T *sub(daObjKanoke_c *a, u32 off) {
  return at<T>(gabi::ea(a), off);
}
u32 word(u32 p) { return *at<be<u32>>(p); }
void put(u32 p, u32 value) { *at<be<u32>>(p) = value; }
constexpr u32 matrix = 0x1048D0CC;
struct SafeString {
  be<u32> text, vtable;
};
u32 resource(gabi::Local<SafeString> &name, u32 id) {
  name->text = 0x1002BC7C;
  name->vtable = 0x1002BC44;
  return gabi::call<u32>(0x026066C4, at<void>(word(0x101F4F28)), name.get(),
                         id);
}
BOOL createHeap(daObjKanoke_c *a) {
  WWHD_FUNC(0x0236743C, BOOL, a);
  gabi::Local<SafeString> bodyName, bodyCollisionName, lidName,
      lidCollisionName;
  u32 data = resource(bodyName, 4);
  if (!data)
    return 0;
  u32 model = gabi::call<u32>(0x025E38E0, at<void>(data), 0, 0x11020203);
  a->bodyModel = at<void>(model);
  if (!model)
    return 0;
  u32 bg = gabi::call<u32>(0x024F23F4, (void *)nullptr);
  a->bodyBackground = at<void>(bg);
  if (!bg)
    return 0;
  data = resource(bodyCollisionName, 8);
  if (gabi::call<s32>(0x0200A030, (void *)a->bodyBackground, at<void>(data), 1,
                      sub<void>(a, 0x3c4)))
    return 0;
  data = resource(lidName, 5);
  if (!data)
    return 0;
  model = gabi::call<u32>(0x025E38E0, at<void>(data), 0, 0x11020203);
  a->lidModel = at<void>(model);
  if (!model)
    return 0;
  bg = gabi::call<u32>(0x024F23F4, (void *)nullptr);
  a->lidBackground = at<void>(bg);
  if (!bg)
    return 0;
  data = resource(lidCollisionName, 9);
  return gabi::call<s32>(0x0200A030, (void *)a->lidBackground, at<void>(data),
                         1, sub<void>(a, 0x3f4)) == 0;
}
VERIFY(0x0236743C, createHeap);
BOOL heapCallback(daObjKanoke_c *a) {
  WWHD_FUNC(0x023675C0, BOOL, a);
  return createHeap(a);
}
VERIFY(0x023675C0, heapCallback);
u8 getType(daObjKanoke_c *a) {
  WWHD_FUNC(0x023675C4, u8, a);
  return (u8)gabi::call<u32>(0x0236904C, a, 1, 0);
}
VERIFY(0x023675C4, getType);
u8 getSearch(daObjKanoke_c *a) {
  WWHD_FUNC(0x023675F0, u8, a);
  return (u8)gabi::call<u32>(0x0236904C, a, 5, 1);
}
VERIFY(0x023675F0, getSearch);
u8 getSwitch(daObjKanoke_c *a) {
  WWHD_FUNC(0x0236761C, u8, a);
  return (u8)gabi::call<u32>(0x0236904C, a, 8, 8);
}
VERIFY(0x0236761C, getSwitch);
u8 getSwitch2(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367648, u8, a);
  return (u8)gabi::call<u32>(0x0236904C, a, 8, 16);
}
VERIFY(0x02367648, getSwitch2);
void setBodyMatrix(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367674, void, a);
  gabi::call<void>(0x028E93CC, at<void>(matrix), (f32)a->current.pos.x,
                   (f32)a->current.pos.y, (f32)a->current.pos.z);
  gabi::call<void>(0x025F1C28, at<void>(matrix), (s16)a->shape_angle.y);
  gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->shape_angle.x);
  gabi::call<void>(0x025F24E0, (f32)a->pivot.x, (f32)a->pivot.y,
                   (f32)a->pivot.z);
  gabi::call<void>(0x025F1C28, at<void>(matrix), (s16)a->lidY);
  gabi::call<void>(0x025F24E0, -(f32)a->pivot.x, -(f32)a->pivot.y,
                   -(f32)a->pivot.z);
}
VERIFY(0x02367674, setBodyMatrix);
void setLidMatrix(daObjKanoke_c *a, cXyz *position) {
  WWHD_FUNC(0x0236770C, void, a, position);
  gabi::Local<cXyz> offset;
  gabi::call<void>(0x025F1884, at<void>(matrix), (s16)a->shape_angle.y);
  gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->shape_angle.x);
  gabi::call<void>(0x028E8F64, at<void>(matrix), &a->lidOffset, offset.get());
  f32 x = (f32)position->x + (f32)offset->x,
      y = (f32)position->y + (f32)offset->y,
      z = (f32)position->z + (f32)offset->z;
  gabi::call<void>(0x028E93CC, at<void>(matrix), x, y, z);
  gabi::call<void>(0x025F1C28, at<void>(matrix), (s16)a->shape_angle.y);
  gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->shape_angle.x);
  gabi::call<void>(0x025F24E0, (f32)a->pivot.x, (f32)a->pivot.y,
                   (f32)a->pivot.z);
  gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->lidX);
  gabi::call<void>(0x025F1C28, at<void>(matrix), (s16)a->lidY);
  gabi::call<void>(0x025F1C5C, at<void>(matrix), (s16)a->lidZ);
  gabi::call<void>(0x025F24E0, -(f32)a->pivot.x, -(f32)a->pivot.y,
                   -(f32)a->pivot.z);
}
VERIFY(0x0236770C, setLidMatrix);
daObjKanoke_c *construct(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367808, daObjKanoke_c *, a);
  if (!a)
    a = at<daObjKanoke_c>(gabi::call<u32>(0x0273AD10, 0x9ac));
  if (!a)
    return a;
  gabi::call<void>(0x025D4ED0, a);
  *sub<be<u32>>(a, 0xb4) = 0x1002BC6C;
  gabi::call<void>(0x0200BD2C, sub<void>(a, 0x424));
  gabi::call<void>(0x02515DA0, sub<void>(a, 0x440));
  *sub<be<u32>>(a, 0x43c) = 0x1004AE88;
  *sub<be<u32>>(a, 0x440) = 0x1004AEC0;
  gabi::call<void>(0x02515FB8, sub<void>(a, 0x460));
  *sub<be<u32>>(a, 0x574) = 0x100015A8;
  *sub<be<u32>>(a, 0x570) = 0x1002BC5C;
  gabi::call<void>(0x02018150, sub<void>(a, 0x578));
  *sub<be<u32>>(a, 0x49c) = 0x1004AF18;
  *sub<be<u32>>(a, 0x574) = 0x1004AF70;
  *sub<be<u32>>(a, 0x590) = 0x1004AF60;
  gabi::call<void>(0x028EFFD0, sub<void>(a, 0x598), 3, 0x138,
                   at<void>(0x02368F24));
  gabi::call<void>(0x025A5B18, sub<void>(a, 0x948), 1);
  a->type = getType(a);
  a->searchRange = getSearch(a);
  a->switchNo = getSwitch(a);
  u8 second = getSwitch2(a), type = a->type;
  a->switchNo2 = second;
  if (type == 0) {
    a->lidOffset.z = 0;
    a->pivot.z = 0;
    a->lidZ = 0;
    a->pivot.x = 0;
    a->angularSpeed = 0;
    a->pivot.y = 0;
    a->lidY = 0;
    a->lidOffset.x = 0;
    a->lidX = 0;
    a->lidOffset.y = 75;
  } else {
    a->pivot.x = 0;
    a->lidOffset.z = 0;
    f32 y = (f32)a->home.pos.y + 200.0f;
    a->shape_angle.x = 0x4000;
    a->pivot.y = 35;
    a->pivot.z = 200;
    a->current.pos.y = y;
    a->lidX = 0;
    a->lidOffset.y = 75;
    a->lidY = 0;
    a->lidZ = 0;
    a->lidOffset.x = 0;
    a->angularSpeed = 0;
  }
  *sub<be<u32>>(a, 0x940) = 0;
  *sub<be<u32>>(a, 0x944) = 0;
  a->state = 0;
  *sub<be<u8>>(a, 0x959) = 0;
  a->lightTimer = 0;
  a->hidden = 0;
  setBodyMatrix(a);
  gabi::call<void>(0x028E90D4, at<void>(matrix), sub<void>(a, 0x3c4));
  setLidMatrix(a, &a->current.pos);
  gabi::call<void>(0x028E90D4, at<void>(matrix), sub<void>(a, 0x3f4));
  return a;
}
VERIFY(0x02367808, construct);
void copyModelMatrix(u32 model) {
  f32 values[12];
  for (int i = 0; i < 12; i++)
    values[i] = *at<be<f32>>(matrix + i * 4);
  for (int i = 0; i < 12; i++)
    *at<be<f32>>(model + 0xc8 + i * 4) = values[i];
}
void setMatrix(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367A38, void, a);
  u8 flags = a->hidden;
  if (!(flags & 1)) {
    setBodyMatrix(a);
    copyModelMatrix(gabi::ea((void *)a->bodyModel));
    gabi::call<void>(0x028E90D4, at<void>(matrix), sub<void>(a, 0x3c4));
    flags = a->hidden;
  }
  if (!(flags & 2)) {
    setLidMatrix(a, &a->current.pos);
    copyModelMatrix(gabi::ea((void *)a->lidModel));
    gabi::call<void>(0x028E90D4, at<void>(matrix), sub<void>(a, 0x3f4));
  }
}
VERIFY(0x02367A38, setMatrix);
s32 createInit(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367B7C, s32, a);
  u32 play = gabi::call<u32>(0x025200D4);
  if (gabi::call<s32>(0x024EEA6C, at<void>(play + 0x12a0),
                      (void *)a->bodyBackground, a))
    return 5;
  u8 sw = a->switchNo;
  bool opened = false;
  if (sw != 255) {
    u32 save = word(0x101F84DC);
    opened = gabi::call<s32>(0x025BA0C0, at<void>(save + 0x20), sw,
                             (s8)*sub<be<u8>>(a, 0x2fe)) != 0;
  }
  if (opened) {
    u8 type = a->type;
    a->state = 7;
    if (type == 0) {
      play = gabi::call<u32>(0x025200D4);
      if (gabi::call<s32>(0x024EEA6C, at<void>(play + 0x12a0),
                          (void *)a->lidBackground, a))
        return 5;
      a->lidOffset.z = 0;
      a->pivot.y = 0;
      a->pivot.z = 0;
      a->lidOffset.y = 75;
      a->lidZ = -5600;
      a->lidOffset.x = 148;
      a->pivot.x = -48;
    } else
      a->hidden = (u8)a->hidden | 2;
  } else {
    play = gabi::call<u32>(0x025200D4);
    if (gabi::call<s32>(0x024EEA6C, at<void>(play + 0x12a0),
                        (void *)a->lidBackground, a))
      return 5;
  }
  gabi::call<void>(0x02515F14, sub<void>(a, 0x424), 255, 255, a);
  gabi::call<void>(0x025164C0, sub<void>(a, 0x460), at<void>(0x101CA6EC));
  *sub<be<u32>>(a, 0x4a4) = gabi::ea(a) + 0x424;
  gabi::call<void>(0x02018808, sub<void>(a, 0x578), &a->current.pos,
                   &a->current.pos);
  for (int i = 0; i < 3; i++) {
    u32 capsule = gabi::ea(a) + 0x598 + i * 0x138;
    gabi::call<void>(0x025164C0, at<void>(capsule), at<void>(0x101CA738));
    put(capsule + 0x44, gabi::ea(a) + 0x424);
    gabi::call<void>(0x02018808, at<void>(capsule + 0x118), &a->current.pos,
                     &a->current.pos);
  }
  u32 model = gabi::ea((void *)a->bodyModel);
  *sub<be<u32>>(a, 0x348) = model ? model + 0xc8 : 0;
  setMatrix(a);
  if (a->type == 0)
    gabi::call<void>(0x025D674C, a, -110.0f, 0.0f, -210.0f, 310.0f, 120.0f,
                     210.0f);
  else
    gabi::call<void>(0x025D674C, a, -110.0f, 0.0f, -210.0f, 110.0f, 520.0f,
                     210.0f);
  return 4;
}
VERIFY(0x02367B7C, createInit);
s32 create(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367E94, s32, a);
  u32 flags = *sub<be<u32>>(a, 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      construct(a);
      flags = *sub<be<u32>>(a, 0x2e4);
    }
    *sub<be<u32>>(a, 0x2e4) = flags | 8;
  }
  s32 phase =
      gabi::call<s32>(0x02520460, sub<void>(a, 0x3ac), at<void>(0x1002BCB8));
  if (phase != 4)
    return phase;
  if (gabi::call<s32>(0x025D63E8, a, at<void>(0x023675C0), 0x2400))
    return createInit(a);
  a->lidBackground = nullptr;
  a->bodyBackground = nullptr;
  return 5;
}
VERIFY(0x02367E94, create);
s32 methodCreate(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367F48, s32, a);
  return create(a);
}
VERIFY(0x02367F48, methodCreate);
BOOL remove(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367F4C, BOOL, a);
  if (*sub<be<u32>>(a, 0xf4)) {
    u32 bg = gabi::ea((void *)a->bodyBackground);
    if (bg && word(bg) < 256) {
      u32 p = gabi::call<u32>(0x025200D4);
      gabi::call<void>(0x020087EC, at<void>(p + 0x12a0),
                       (void *)a->bodyBackground);
    }
    bg = gabi::ea((void *)a->lidBackground);
    if (bg && word(bg) < 256) {
      u32 p = gabi::call<u32>(0x025200D4);
      gabi::call<void>(0x020087EC, at<void>(p + 0x12a0),
                       (void *)a->lidBackground);
    }
  }
  u32 target = word(word(gabi::ea(a) + 0x948) + 0x44);
  gabi::call<void>(target, sub<void>(a, 0x948));
  gabi::call<void>(0x025204C8, sub<void>(a, 0x3ac), at<void>(0x1002BCC0));
  return 1;
}
VERIFY(0x02367F4C, remove);
BOOL methodDelete(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367FF8, BOOL, a);
  return remove(a);
}
VERIFY(0x02367FF8, methodDelete);
BOOL execute(daObjKanoke_c *a) {
  WWHD_FUNC(0x02367FFC, BOOL, a);
  gabi::Local<cXyz> start, end;
  start->x = 0;
  start->y = 0;
  start->z = -100;
  end->x = 0;
  end->y = 0;
  end->z = 100;
  gabi::call<void>(0x025F1884, at<void>(matrix), (s16)a->shape_angle.y);
  gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->shape_angle.x);
  gabi::call<void>(0x028E8F64, at<void>(matrix), start.get(), start.get());
  gabi::call<void>(0x028E8F64, at<void>(matrix), end.get(), end.get());
  gabi::call<void>(0x028E8D88, start.get(), &a->current.pos, start.get());
  gabi::call<void>(0x028E8D88, end.get(), &a->current.pos, end.get());
  gabi::call<void>(0x02018808, sub<void>(a, 0x578), start.get(), end.get());
  *sub<be<f32>>(a, 0x594) = 140;
  u32 p = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x0200E240, at<void>(p + 0x26a4), sub<void>(a, 0x460));
  u32 entry = 0x101CA68C + (u8)a->state * 8;
  s16 adjust = *at<be<s16>>(entry), slot = *at<be<s16>>(entry + 2);
  u32 self = gabi::ea(a) + (s32)adjust, target;
  if (slot < 0)
    target = word(entry + 4);
  else {
    s16 tableOff = *at<be<s16>>(entry + 6);
    target = word(word(self + (s32)tableOff) + (u32)(s32)slot * 8 + 4);
  }
  gabi::call<void>(target, at<void>(self));
  setMatrix(a);
  u32 bg = gabi::ea((void *)a->bodyBackground);
  if (word(bg) < 256)
    gabi::call<void>(0x024F43DC, at<void>(bg));
  bg = gabi::ea((void *)a->lidBackground);
  if (word(bg) < 256)
    gabi::call<void>(0x024F43DC, at<void>(bg));
  return 1;
}
VERIFY(0x02367FFC, execute);
BOOL methodExecute(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368188, BOOL, a);
  return execute(a);
}
VERIFY(0x02368188, methodExecute);
BOOL draw(daObjKanoke_c *a) {
  WWHD_FUNC(0x0236818C, BOOL, a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x025626A4, at<void>(env), 1, &a->current.pos,
                   sub<void>(a, 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x02562F5C, at<void>(env), (void *)a->bodyModel,
                   sub<void>(a, 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x02562F5C, at<void>(env), (void *)a->lidModel,
                   sub<void>(a, 0x110));
  u32 p = gabi::call<u32>(0x025200D4);
  put(0x104B4634, word(p + 0x5d70));
  p = gabi::call<u32>(0x025200D4);
  put(0x104B4638, word(p + 0x5d74));
  u8 flags = a->hidden;
  if (!(flags & 1)) {
    gabi::call<void>(0x025E2DE0, (void *)a->bodyModel, 0);
    flags = a->hidden;
  }
  if (!(flags & 2))
    gabi::call<void>(0x025E2DE0, (void *)a->lidModel, 0);
  p = gabi::call<u32>(0x025200D4);
  put(0x104B4634, word(p + 0x5d78));
  p = gabi::call<u32>(0x025200D4);
  put(0x104B4638, word(p + 0x5d7c));
  return 1;
}
VERIFY(0x0236818C, draw);
BOOL methodDraw(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368258, BOOL, a);
  return draw(a);
}
VERIFY(0x02368258, methodDraw);
u8 getShake(daObjKanoke_c *a) {
  WWHD_FUNC(0x0236825C, u8, a);
  return (u8)gabi::call<u32>(0x0236904C, a, 1, 6);
}
VERIFY(0x0236825C, getShake);
void sound(daObjKanoke_c *a, u32 id) {
  s32 reverb = gabi::call<s32>(0x02520540, (s8)*sub<be<u8>>(a, 0x326));
  gabi::call<void>(0x025E1A40, id, &a->current.pos, 0, reverb);
}
void releaseLid(daObjKanoke_c *a) {
  u32 bg = gabi::ea((void *)a->lidBackground);
  if (word(bg) < 256) {
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x020087EC, at<void>(p + 0x12a0),
                     (void *)a->lidBackground);
  }
}
void normal(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368288, void, a);
  u8 sw = a->switchNo;
  bool trigger = false;
  if (sw != 255) {
    u32 save = word(0x101F84DC);
    trigger = gabi::call<s32>(0x025BA0C0, at<void>(save + 0x20), sw,
                              (s8)*sub<be<u8>>(a, 0x2fe)) != 0;
  }
  if (trigger)
    gabi::call<void>(0x0251621C, sub<void>(a, 0x460));
  else {
    u32 p = gabi::call<u32>(0x025200D4);
    bool lit =
        gabi::call<s32>(0x0252A038, at<void>(p + 0x5a20), &a->current.pos) != 0;
    if (!lit)
      lit = gabi::call<s32>(0x025162A4, sub<void>(a, 0x460)) != 0;
    if (lit) {
      s16 count = (s16)((s16)a->lightTimer + 1);
      a->lightTimer = count;
      if (count <= 20)
        return;
      *sub<be<u32>>(a, 0x488) = 0;
      gabi::call<void>(0x0251621C, sub<void>(a, 0x460));
    } else {
      u8 search = a->searchRange;
      a->lightTimer = 0;
      if (!search)
        return;
      p = gabi::call<u32>(0x025200D4);
      u32 player = word(p + 0x5b2c);
      gabi::Local<cXyz> delta;
      gabi::call<void>(0x0201ADE0, sub<cXyz>(a, 0x2ec), delta.get(),
                       at<cXyz>(player + 0x314));
      f64 square = gabi::call<f64>(0x028E8DD0, delta.get());
      f64 distance = gabi::call<f64>(0x028F4384, square);
      f32 limit = (f32)(u8)a->searchRange * 100.0f;
      if (!(distance < limit))
        return;
      gabi::call<void>(0x0251621C, sub<void>(a, 0x460));
    }
  }
  sw = a->switchNo;
  if (sw != 255) {
    u32 save = word(0x101F84DC);
    gabi::call<void>(0x025B9E38, at<void>(save + 0x20), sw,
                     (s8)*sub<be<u8>>(a, 0x2fe));
  }
  a->angularSpeed = 0;
  bool shake = getShake(a) != 0;
  if (shake) {
    s8 room = *sub<be<u8>>(a, 0x326);
    u8 type = a->type;
    a->timer = 0;
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call<void>(0x025E1A40, type == 0 ? 0x695b : 0x6959, &a->current.pos,
                     0, reverb);
    if (type == 0) {
      a->lidZ = 0;
      a->shakeSpeed = 256;
      a->state = 1;
    } else {
      a->lidY = 0;
      a->shakeSpeed = 1024;
      a->state = 4;
    }
  } else {
    u8 type = a->type;
    sound(a, type == 0 ? 0x695c : 0x695a);
    if (type == 0) {
      a->lidZ = 0;
      a->state = 2;
    } else {
      a->state = 5;
      releaseLid(a);
      a->lidX = 0;
    }
  }
}
VERIFY(0x02368288, normal);
void shakeYoko(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368534, void, a);
  s16 speed = (s16)((s16)a->shakeSpeed - 8);
  u16 phase = (u16)((s16)a->timer + 0x2000);
  a->timer = (s16)phase;
  a->shakeSpeed = speed;
  f32 sine = *at<be<f32>>(0x104A44F8 + (phase & 0xfff8));
  s16 angle = (s16)gabi::ftoi(sine * (f32)speed);
  a->lidZ = angle;
  s16 remaining = a->shakeSpeed;
  a->pivot.x = angle < 0 ? 100.0f : -100.0f;
  if (remaining <= 0) {
    sound(a, 0x695c);
    a->lidZ = 0;
    a->pivot.x = 0;
    a->state = 2;
  }
}
VERIFY(0x02368534, shakeYoko);
void smokeBegin(daObjKanoke_c *a, u32 id, u8 alpha) {
  u32 p = gabi::call<u32>(0x025200D4), particles = word(p + 0x5ab0);
  gabi::call<void>(0x025A847C, at<void>(particles), 2, id, sub<cXyz>(a, 0x968),
                   sub<void>(a, 0x974), 0, alpha, sub<void>(a, 0x948), -1, 0, 0,
                   0);
}
void flagSmoke(daObjKanoke_c *a, u32 emitter) {
  if (emitter)
    put(emitter + 0x254, word(emitter + 0x254) | 0x40);
}
void openYoko(daObjKanoke_c *a) {
  WWHD_FUNC(0x0236863C, void, a);
  f32 x = (f32)a->lidOffset.x + 4.0f;
  a->lidOffset.x = x;
  a->pivot.x = 100.0f - x;
  if (!(x > 100.0f))
    return;
  s16 speed = a->angularSpeed, angle = (s16)((s16)a->lidZ + speed);
  a->lidZ = angle;
  a->angularSpeed = (s16)(speed - 100);
  if (angle > -5600)
    return;
  a->state = 3;
  a->lidZ = -5600;
  gabi::call<void>(0x025F1884, at<void>(matrix), (s16)a->shape_angle.y);
  gabi::call<void>(0x025F24E0, 100.0f, 75.0f, 0.0f);
  gabi::call<void>(0x025F1C5C, at<void>(matrix), (s16)a->lidZ);
  gabi::call<void>(0x025F24E0, -100.0f, -75.0f, 0.0f);
  gabi::Local<be<f32>[12]> transform;
  gabi::call<void>(0x028E90D4, at<void>(matrix), transform.get());
  s16 yaw = a->shape_angle.y;
  *sub<be<s16>>(a, 0x974) = 0;
  *sub<be<s16>>(a, 0x976) = yaw;
  *sub<be<s16>>(a, 0x978) = 0;
  a->smokeAlpha = 180;
  gabi::Local<cXyz> sum, point;
  gabi::call<void>(0x0201AD78, &a->lidOffset, sum.get(), at<cXyz>(0x1046A3D0));
  point->copy(*sum);
  gabi::call<void>(0x028E8F64, transform.get(), point.get(), point.get());
  gabi::call<void>(0x0201AD78, point.get(), sum.get(), &a->current.pos);
  sub<cXyz>(a, 0x968)->copy(*sum);
  u32 emitter = *sub<be<u32>>(a, 0x94c);
  if (!emitter) {
    u8 alpha = (u8)gabi::ftoi((f32)a->smokeAlpha);
    smokeBegin(a, 0xa181, alpha);
    emitter = *sub<be<u32>>(a, 0x94c);
  }
  flagSmoke(a, emitter);
  a->timer = 60;
}
VERIFY(0x0236863C, openYoko);
void effectYoko(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368860, void, a);
  s16 timer = (s16)((s16)a->timer - 1);
  a->timer = timer;
  if (!timer) {
    gabi::call<void>(0x025A5F88, sub<void>(a, 0x948));
    a->state = 7;
    return;
  }
  if (timer > 50)
    return;
  f32 alpha = (f32)a->smokeAlpha - 3.6f;
  if (alpha < 0.0f)
    alpha = 0.0f;
  u32 emitter = *sub<be<u32>>(a, 0x94c);
  a->smokeAlpha = alpha;
  if (emitter)
    *at<be<u8>>(emitter + 0x247) = (u8)gabi::ftoi(alpha);
}
VERIFY(0x02368860, effectYoko);
void shakeTate(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368918, void, a);
  u16 phase = (u16)((s16)a->timer + 0x2000);
  s16 speed = (s16)((s16)a->shakeSpeed - 32);
  a->timer = (s16)phase;
  a->shakeSpeed = speed;
  f32 sine = *at<be<f32>>(0x104A44F8 + (phase & 0xfff8));
  s16 angle = (s16)gabi::ftoi(sine * (f32)speed);
  a->lidY = angle;
  s16 remaining = a->shakeSpeed;
  a->pivot.x = angle < 0 ? 100.0f : -100.0f;
  if (remaining <= 0) {
    sound(a, 0x695a);
    a->state = 5;
    releaseLid(a);
    a->lidX = 0;
    a->lidY = 0;
    a->pivot.x = 0;
  }
}
VERIFY(0x02368918, shakeTate);
void openTate(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368A44, void, a);
  s16 speed = a->angularSpeed, angle = (s16)((s16)a->lidX + speed);
  a->angularSpeed = (s16)(speed + 100);
  a->lidX = angle;
  if (angle >= 0x4000) {
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::Local<cXyz> up;
    up->z = 0;
    up->x = 0;
    up->y = 1;
    gabi::call<void>(0x025CB374, at<void>(p + 0x599c), 4, -33, up.get());
    s16 yaw = a->shape_angle.y;
    f32 y = *sub<be<f32>>(a, 0x410), x = *sub<be<f32>>(a, 0x400);
    *sub<be<s16>>(a, 0x976) = yaw;
    u8 hidden = a->hidden;
    *sub<be<s16>>(a, 0x978) = 0;
    *sub<be<f32>>(a, 0x96c) = y;
    f32 z = *sub<be<f32>>(a, 0x420);
    *sub<be<s16>>(a, 0x974) = 0;
    *sub<be<f32>>(a, 0x970) = z;
    a->lidX = 0x4000;
    *sub<be<f32>>(a, 0x968) = x;
    a->state = 6;
    a->hidden = hidden | 2;
    p = gabi::call<u32>(0x025200D4);
    u32 effect =
        gabi::call<u32>(0x025A847C, at<void>(word(p + 0x5ab0)), 0, 0x817f,
                        sub<cXyz>(a, 0x968), sub<void>(a, 0x974), 0, 255, 0, -1,
                        sub<void>(a, 0x1a8), sub<void>(a, 0x1a8), 0);
    u32 emitter = *sub<be<u32>>(a, 0x94c);
    *sub<be<u32>>(a, 0x940) = effect;
    a->smokeAlpha = 180;
    if (!emitter) {
      smokeBegin(a, 0xa180, 180);
      emitter = *sub<be<u32>>(a, 0x94c);
    }
    flagSmoke(a, emitter);
    a->timer = 60;
    return;
  }
  gabi::call<void>(0x025F1884, at<void>(matrix), (s16)a->shape_angle.y);
  gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->shape_angle.x);
  gabi::call<void>(0x025F24E0, 0.0f, 110.0f, 200.0f);
  gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->lidX);
  gabi::call<void>(0x025F24E0, 0.0f, -110.0f, -200.0f);
  gabi::Local<cXyz> sum, start, end;
  for (int i = 0; i < 3; i++) {
    u32 capsule = gabi::ea(a) + 0x598 + i * 0x138;
    gabi::call<void>(0x0201AD78, at<cXyz>(0x1046A3DC + i * 24), sum.get(),
                     &a->lidOffset);
    start->copy(*sum);
    gabi::call<void>(0x0201AD78, at<cXyz>(0x1046A3E8 + i * 24), sum.get(),
                     &a->lidOffset);
    end->copy(*sum);
    gabi::call<void>(0x028E8F64, at<void>(matrix), start.get(), start.get());
    gabi::call<void>(0x028E8F64, at<void>(matrix), end.get(), end.get());
    gabi::call<void>(0x028E8D88, start.get(), &a->current.pos, start.get());
    gabi::call<void>(0x028E8D88, end.get(), &a->current.pos, end.get());
    gabi::call<void>(0x02018808, at<void>(capsule + 0x118), start.get(),
                     end.get());
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x0200E240, at<void>(p + 0x26a4), at<void>(capsule));
  }
}
VERIFY(0x02368A44, openTate);
void effectTate(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368D34, void, a);
  s16 timer = (s16)((s16)a->timer - 1);
  a->timer = timer;
  if (!timer) {
    gabi::call<void>(0x025A5F88, sub<void>(a, 0x948));
    a->state = 7;
    return;
  }
  if (!*sub<be<u32>>(a, 0x94c) || timer > 50)
    return;
  f32 alpha = (f32)a->smokeAlpha - 3.6f;
  u32 emitter = *sub<be<u32>>(a, 0x94c);
  if (alpha < 0.0f)
    alpha = 0.0f;
  a->smokeAlpha = alpha;
  *at<be<u8>>(emitter + 0x247) = (u8)gabi::ftoi(alpha);
}
VERIFY(0x02368D34, effectTate);
void staticInit() {
  WWHD_FUNC(0x02368DF0, void);
  put(0x1046A3C4, 0);
  put(0x1046A3CC, 0);
  put(0x1046A3C0, 0);
  put(0x1046A3C8, 0);
  gabi::call<void>(0x028F026C, at<void>(0x101CA784));
  *at<be<f32>>(0x1046A3B4) = -3.1415927410125732f;
  *at<be<f32>>(0x1046A3B8) = 3.1415927410125732f;
  gabi::call<void>(0x028ED6F8, at<void>(0x1046A3BC));
  gabi::call<void>(0x028F026C, at<void>(0x101CA790));
  gabi::call<void>(0x028EAB2C, at<void>(0x1046A3BD));
  gabi::call<void>(0x028F026C, at<void>(0x101CA79C));
  *at<be<f32>>(0x1046A3DC) = 50;
  *at<be<f32>>(0x1046A3E0) = 0;
  *at<be<f32>>(0x1046A3E4) = -175;
  *at<be<f32>>(0x1046A3E8) = 50;
  *at<be<f32>>(0x1046A3D0) = 100;
  *at<be<f32>>(0x1046A3D4) = 0;
  *at<be<f32>>(0x1046A3EC) = 0;
  *at<be<f32>>(0x1046A3F0) = 175;
  *at<be<f32>>(0x1046A3F4) = 0;
  *at<be<f32>>(0x1046A3F8) = 0;
  *at<be<f32>>(0x1046A3FC) = -175;
  *at<be<f32>>(0x1046A400) = 0;
  *at<be<f32>>(0x1046A404) = 0;
  *at<be<f32>>(0x1046A408) = 175;
  *at<be<f32>>(0x1046A40C) = -50;
  *at<be<f32>>(0x1046A410) = 0;
  *at<be<f32>>(0x1046A414) = -175;
  *at<be<f32>>(0x1046A418) = -50;
  *at<be<f32>>(0x1046A41C) = 0;
  *at<be<f32>>(0x1046A420) = 175;
  *at<be<f32>>(0x1046A3D8) = 0;
}
VERIFY(0x02368DF0, staticInit);
void emptyDestructor(void *self, s32 flags) {
  WWHD_FUNC(0x02368F10, void, self, flags);
  if (self && (flags & 1))
    gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x02368F10, emptyDestructor);
void *capsuleConstruct(void *self) {
  WWHD_FUNC(0x02368F24, void *, self);
  if (!self)
    self = at<void>(gabi::call<u32>(0x0273AD10, 0x138));
  if (!self)
    return self;
  u32 p = gabi::ea(self);
  gabi::call<void>(0x02515FB8, self);
  put(p + 0x114, 0x100015A8);
  put(p + 0x110, 0x1002BC5C);
  gabi::call<void>(0x02018150, at<void>(p + 0x118));
  put(p + 0x3c, 0x1004AF18);
  put(p + 0x130, 0x1004AF60);
  put(p + 0x114, 0x1004AF70);
  return self;
}
VERIFY(0x02368F24, capsuleConstruct);
BOOL isDelete(daObjKanoke_c *a) {
  WWHD_FUNC(0x02368FB0, BOOL, a);
  return 1;
}
VERIFY(0x02368FB0, isDelete);
void emptyVirtual(void *self) { WWHD_FUNC(0x02368FB8, void, self); }
VERIFY(0x02368FB8, emptyVirtual);
void actorDestructor(daObjKanoke_c *a, s32 flags) {
  WWHD_FUNC(0x02368FBC, void, a, flags);
  if (!a)
    return;
  gabi::call<void>(0x028F0164, sub<void>(a, 0x598), 3, 0x138,
                   at<void>(0x02515980), 0, 0);
  gabi::call<void>(0x02515980, sub<void>(a, 0x460), 2);
  gabi::call<void>(0x02515860, sub<void>(a, 0x424), 2);
  gabi::call<void>(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call<void>(0x0273AF40, a);
}
VERIFY(0x02368FBC, actorDestructor);
void wait(daObjKanoke_c *a) { WWHD_FUNC(0x02369048, void, a); }
VERIFY(0x02369048, wait);
u32 parameter(const daObjKanoke_c *a, u32 width, u32 shift) {
  WWHD_FUNC(0x0236904C, u32, a, width, shift);
  u32 value = *at<be<u32>>(gabi::ea(a) + 0xb0), n = width & 63, s = shift & 63;
  u32 mask = (n < 32 ? (1u << n) : 0u) - 1u, shifted = s < 32 ? value >> s : 0u;
  return shifted & mask;
}
VERIFY(0x0236904C, parameter);
} // namespace Kanoke
