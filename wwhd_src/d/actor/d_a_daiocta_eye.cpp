/* Big Octo eye, reconstructed from local WWHD code. */
#include "d/actor/d_a_daiocta_eye.h"
namespace OctoEye {
using Actor = daDaiocta_Eye_c;
template <class T> T *at(u32 p) { return gabi::at<T>(p); }
template <class T> T read(u32 p) { return *at<be<T>>(p); }
template <class T> void write(u32 p, T value) { *at<be<T>>(p) = value; }
u32 ea(Actor *a) { return gabi::ea(a); }
constexpr u32 Matrix = 0x1048d0cc, Hio = 0x10463a68;
struct SafeString {
  be<u32> text, vtable;
};
struct ResourceLocal {
  be<u32> linkage[2];
  SafeString name;
};
u32 resource(gabi::Local<ResourceLocal> &local, u32 id) {
  local->name.text = 0x1000d568;
  local->name.vtable = 0x1000d4b4;
  return gabi::call<u32>(0x026066c4, at<void>(read<u32>(0x101f4f28)),
                         &local->name, id);
}
void copyMatrix(u32 from, u32 to) {
  float values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = read<float>(from + 4 * i);
  for (u32 i = 0; i < 12; ++i)
    write<float>(to + 4 * i, values[i]);
}
void parentSound(u32 parent, u32 sound) {
  if (!parent || !(parent + 0x37c))
    return;
  s8 room = read<s8>(parent + 0x326);
  u32 id = read<u32>(parent + 4);
  s8 reverb = gabi::call<s8>(0x02520540, room);
  gabi::call(0x025e1aa4, sound, at<void>(parent + 0x37c), id, 0, reverb);
}
void actorSound(Actor *a, u32 sound, s32 extra) {
  s8 reverb = gabi::call<s8>(0x02520540, read<s8>(ea(a) + 0x326));
  gabi::call(0x025e1a40, sound, at<void>(ea(a) + 0x37c), extra, reverb);
}
void removeParticle(Actor *a) {
  u32 callback = ea(a) + 0x654;
  u32 target = read<u32>(read<u32>(callback) + 0x44);
  gabi::call_ptr<void>(target, at<void>(callback));
}
void NodeControl(Actor *a, void *node, void *model) {
  WWHD_FUNC(0x0211d314, void, a, node, model);
  u32 joint = gabi::call<u32>(0x027f7878, node);
  u32 index = read<u16>(joint + 4), modelEA = gabi::ea(model);
  u32 matrices = read<u32>(modelEA + 0x2c);
  u32 source = read<u32>(matrices + 0x10) + index * 48;
  write<u16>(matrices + 4, read<u16>(matrices + 4) | 0x10);
  gabi::call(0x028e90d4, at<void>(source), at<void>(Matrix));
  if (index == 2) {
    s16 x = a->eyeRotation.x, y = a->eyeRotation.y, z = a->eyeRotation.z;
    gabi::call(0x025f1b48, at<void>(Matrix), x, y, z);
    float sx = a->eyeScale.x, sy = a->eyeScale.y, sz = a->eyeScale.z;
    gabi::call(0x025f2518, sx, sy, sz);
  }
  gabi::call(0x028e90d4, at<void>(Matrix), at<void>(0x104b4868));
  matrices = read<u32>(modelEA + 0x2c);
  u32 destination = read<u32>(matrices + 0x10) + index * 48;
  write<u16>(matrices + 4, read<u16>(matrices + 4) | 0x10);
  copyMatrix(Matrix, destination);
}
VERIFY(0x0211d314, NodeControl);
BOOL NodeCallback(void *node, s32 timing) {
  WWHD_FUNC(0x0211d42c, BOOL, node, timing);
  if (timing == 0) {
    u32 model = read<u32>(0x104b462c);
    u32 actor = read<u32>(model + 0xb8);
    if (actor)
      NodeControl(at<Actor>(actor), node, at<void>(model));
  }
  return 1;
}
VERIFY(0x0211d42c, NodeCallback);
BOOL CreateHeap(Actor *a) {
  WWHD_FUNC(0x0211d474, BOOL, a);
  gabi::Local<ResourceLocal> modelName, brkName, btkName;
  u32 data = resource(modelName, 0x10);
  if (!data)
    gabi::call(0x0273aa24, at<void>(0x1000d4f0), 0xe9, at<void>(0x1000d504));
  u32 model = gabi::call<u32>(0x025e38e0, at<void>(data), 0x80000, 0x11000222);
  a->model = at<void>(model);
  if (!model)
    return 0;
  write<u32>(model + 0xb8, ea(a));
  u32 brk = resource(brkName, 0x18);
  a->brkResource = at<void>(brk);
  if (!brk)
    return 0;
  if (!gabi::call<s32>(0x025e8154, at<void>(ea(a) + 0x3cc), at<void>(data),
                       at<void>(brk), 1, 0, 1.0f, 0, -1, 0, 0))
    return 0;
  u32 btk = resource(btkName, 0x23);
  a->btkResource = at<void>(btk);
  if (!btk)
    return 0;
  if (!gabi::call<s32>(0x025e7ce0, at<void>(ea(a) + 0x448), at<void>(data),
                       at<void>(btk), 1, 0, 1.0f, 0, -1, 0, 0))
    return 0;
  u32 count = read<u32>(data + 4), joint = read<u32>(data + 8);
  if (count > 2)
    joint += 0x38;
  write<u32>(joint + 8, 0x0211d42c);
  u32 hit =
      gabi::call<u32>(0x02552b60, a->model.get(), at<void>(0x101b417c), 1);
  a->jointHit = at<void>(hit);
  if (!hit)
    return 0;
  write<u32>(ea(a) + 0x36c, hit);
  return 1;
}
VERIFY(0x0211d474, CreateHeap);
BOOL HeapCallback(Actor *a) {
  WWHD_FUNC(0x0211d658, BOOL, a);
  return CreateHeap(a);
}
VERIFY(0x0211d658, HeapCallback);
void DeathInit(Actor *a) {
  WWHD_FUNC(0x0211d65c, void, a);
  a->mode = 2;
  a->eyeScale.y = 1;
  a->damaged = 1;
  a->eyeScale.z = 1;
  a->eyeScale.x = 1;
  gabi::Local<ResourceLocal> brkName, btkName;
  u32 brk = resource(brkName, 0x19);
  a->brkResource = at<void>(brk);
  u32 data = read<u32>(gabi::ea(a->model.get()) + 0xac);
  gabi::call(0x025e8154, at<void>(ea(a) + 0x3cc), at<void>(data), at<void>(brk),
             1, 0, 1.0f, 0, -1, 1, 0);
  u32 btk = resource(btkName, 0x24);
  a->btkResource = at<void>(btk);
  data = read<u32>(gabi::ea(a->model.get()) + 0xac);
  gabi::call(0x025e7ce0, at<void>(ea(a) + 0x448), at<void>(data), at<void>(btk),
             1, 0, 1.0f, 0, -1, 1, 0);
  parentSound(gabi::ea(a->parent.get()), 0x48cb);
}
VERIFY(0x0211d65c, DeathInit);
void CoHit(Actor *a, void *other) {
  WWHD_FUNC(0x0211d7cc, void, a, other);
  if (!other || read<s16>(gabi::ea(other) + 8) != 0x126)
    return;
  if (!gabi::call<BOOL>(0x020cb92c, other, 4))
    return;
  if (a->dead) {
    a->damagedByBomb = 1;
    return;
  }
  s8 room = read<s8>(ea(a) + 0x326);
  write<u8>(ea(a) + 0x3a1, 0);
  a->dead = 1;
  a->bombKilled = 1;
  u32 play = gabi::call<u32>(0x025200d4);
  gabi::call(0x025a847c, at<void>(read<u32>(play + 0x5ab0)), 0, 0x8206,
             at<void>(ea(a) + 0x668), at<void>(ea(a) + 0x328), 0, 0xff,
             at<void>(ea(a) + 0x654), room, 0, 0, 0);
  DeathInit(a);
}
VERIFY(0x0211d7cc, CoHit);
void CollisionCallback(Actor *a, void *info, void *other) {
  WWHD_FUNC(0x0211d8b8, void, a, info, other);
  if (other)
    CoHit(a, other);
}
VERIFY(0x0211d8b8, CollisionCallback);
void CreateInit(Actor *a) {
  WWHD_FUNC(0x0211d8c8, void, a);
  u32 model = gabi::ea(a->model.get());
  a->appeared = 0;
  write<u32>(ea(a) + 0x348, model ? model + 0xc8 : 0);
  gabi::call(0x02515f14, at<void>(ea(a) + 0x600), 0, 0, a);
  gabi::call(0x0251677c, at<void>(ea(a) + 0x4d4), at<void>(0x101b411c));
  u32 parentId = read<u32>(ea(a) + 0x2e8);
  write<u8>(ea(a) + 0x3a1, 4);
  a->eyeScale.y = 1;
  a->eyeScale.x = 1;
  write<u32>(ea(a) + 0x518, ea(a) + 0x600);
  write<u32>(ea(a) + 0x5b8, 0x0211d8b8);
  write<u8>(ea(a) + 0x3a0, 4);
  a->eyeScale.z = 1;
  if (parentId != 0xffffffff) {
    gabi::Local<be<u32>> id;
    *id = parentId;
    u32 parent = gabi::call<u32>(0x025d5218, at<void>(0x025e1234), id.get());
    if (parent && gabi::call<BOOL>(0x025d4604, at<void>(parent)) &&
        read<s16>(parent + 8) == 0xe1)
      a->parent = at<void>(parent);
  }
}
VERIFY(0x0211d8c8, CreateInit);
s32 Create(Actor *a) {
  WWHD_FUNC(0x0211d9bc, s32, a);
  u32 flags = read<u32>(ea(a) + 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      gabi::call(0x025d4ed0, a);
      write<u32>(ea(a) + 0xb4, 0x1000d4cc);
      gabi::call(0x025e80d0, at<void>(ea(a) + 0x3cc));
      gabi::call(0x025e7c6c, at<void>(ea(a) + 0x448));
      gabi::call(0x025166f0, at<void>(ea(a) + 0x4d4));
      gabi::call(0x0200bd2c, at<void>(ea(a) + 0x600));
      gabi::call(0x02515da0, at<void>(ea(a) + 0x61c));
      write<u32>(ea(a) + 0x618, 0x1004ae88);
      write<u32>(ea(a) + 0x61c, 0x1004aec0);
      gabi::call(0x025a5894, at<void>(ea(a) + 0x654), 0, 0);
      flags = read<u32>(ea(a) + 0x2e4);
    }
    write<u32>(ea(a) + 0x2e4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, at<void>(ea(a) + 0x3bc),
                              at<void>(0x1000d568));
  if (phase == 4) {
    if (!gabi::call<BOOL>(0x025d63e8, a, at<void>(0x0211d658), 0xb20))
      return 5;
    CreateInit(a);
  }
  return phase;
}
VERIFY(0x0211d9bc, Create);
s32 CreateWrapper(Actor *a) {
  WWHD_FUNC(0x0211dad0, s32, a);
  return Create(a);
}
VERIFY(0x0211dad0, CreateWrapper);
BOOL Delete(Actor *a) {
  WWHD_FUNC(0x0211dad4, BOOL, a);
  gabi::call(0x025204c8, at<void>(ea(a) + 0x3bc), at<void>(0x1000d568));
  removeParticle(a);
  return 1;
}
VERIFY(0x0211dad4, Delete);
BOOL DeleteWrapper(Actor *a) {
  WWHD_FUNC(0x0211db24, BOOL, a);
  return Delete(a);
}
VERIFY(0x0211db24, DeleteWrapper);
void ModeCall(Actor *a) {
  WWHD_FUNC(0x0211db28, void, a);
  u32 descriptor = 0x1000d518 + u32(a->mode) * 8;
  s16 adjustment = read<s16>(descriptor), index = read<s16>(descriptor + 2);
  u32 receiver = ea(a) + adjustment, target;
  if (index < 0)
    target = read<u32>(descriptor + 4);
  else {
    s16 offset = read<s16>(descriptor + 6);
    target = read<u32>(read<u32>(receiver + offset) + u32(index) * 8 + 4);
  }
  gabi::call_ptr<void>(target, at<void>(receiver));
}
VERIFY(0x0211db28, ModeCall);
void SetMatrix(Actor *a) {
  WWHD_FUNC(0x0211db74, void, a);
  float x = read<float>(ea(a) + 0x330), y = read<float>(ea(a) + 0x334),
        z = read<float>(ea(a) + 0x338);
  u32 model = gabi::ea(a->model.get());
  write<float>(model + 0xbc, x);
  write<float>(model + 0xc0, y);
  write<float>(model + 0xc4, z);
  x = read<float>(ea(a) + 0x314);
  y = read<float>(ea(a) + 0x318);
  z = read<float>(ea(a) + 0x31c);
  gabi::call(0x028e93cc, at<void>(Matrix), x, y, z);
  s16 rx = read<s16>(ea(a) + 0x328), ry = read<s16>(ea(a) + 0x32a),
      rz = read<s16>(ea(a) + 0x32c);
  gabi::call(0x025f1b48, at<void>(Matrix), rx, ry, rz);
  copyMatrix(Matrix, gabi::ea(a->model.get()) + 0xc8);
  x = read<float>(ea(a) + 0x314);
  y = read<float>(ea(a) + 0x318);
  z = read<float>(ea(a) + 0x31c);
  gabi::call(0x028e93cc, at<void>(Matrix), x, y, z);
  rx = read<s16>(ea(a) + 0x328);
  ry = read<s16>(ea(a) + 0x32a);
  rz = read<s16>(ea(a) + 0x32c);
  gabi::call(0x025f1b48, at<void>(Matrix), rx, ry, rz);
  x = read<float>(Hio + 0x10);
  y = read<float>(Hio + 0x14);
  z = read<float>(Hio + 0x18);
  gabi::call(0x025f24e0, x, y, z);
  a->particlePosition.x = read<float>(Matrix + 0xc);
  a->particlePosition.y = read<float>(Matrix + 0x1c);
  a->particlePosition.z = read<float>(Matrix + 0x2c);
}
VERIFY(0x0211db74, SetMatrix);
BOOL Execute(Actor *a) {
  WWHD_FUNC(0x0211dcac, BOOL, a);
  if (!a->appeared)
    return 0;
  if (a->dead)
    write<u32>(ea(a) + 0x4ec, read<u32>(ea(a) + 0x4ec) & ~1u);
  for (u32 i = 0; i < 6; ++i)
    write<u16>(ea(a) + 0x4c8 + 2 * i, read<u16>(Hio + 0x24 + 2 * i));
  gabi::call(0x025e742c, at<void>(ea(a) + 0x3cc));
  gabi::call(0x025e742c, at<void>(ea(a) + 0x448));
  a->damaged = 0;
  a->damagedByBomb = 0;
  ModeCall(a);
  SetMatrix(a);
  float y = read<float>(ea(a) + 0x318), z = read<float>(ea(a) + 0x31c);
  write<float>(ea(a) + 0x398, z);
  float above = y + 100.0f;
  float x = read<float>(ea(a) + 0x314);
  u32 zbits = read<u32>(ea(a) + 0x31c), xbits = read<u32>(ea(a) + 0x314);
  write<u32>(ea(a) + 0x384, zbits);
  write<u32>(ea(a) + 0x37c, xbits);
  u32 ybits = read<u32>(ea(a) + 0x318);
  write<float>(ea(a) + 0x390, x);
  write<float>(ea(a) + 0x394, above);
  write<u32>(ea(a) + 0x380, ybits);
  for (u32 i = 0; i < 3; ++i) {
    s16 scale = s16(read<s16>(Hio + 0x22) + 1), step = read<s16>(Hio + 0x20),
        target = read<s16>(ea(a) + 0x4bc + 2 * i);
    gabi::call(0x0200f428, at<void>(ea(a) + 0x4c2 + 2 * i), target, scale,
               step);
  }
  gabi::call(0x02018c8c, at<void>(ea(a) + 0x5ec), read<float>(Hio + 0x1c));
  gabi::call(0x02018d40, at<void>(ea(a) + 0x5ec), at<void>(ea(a) + 0x314));
  u32 play = gabi::call<u32>(0x025200d4);
  gabi::call(0x0200e240, at<void>(play + 0x26a4), at<void>(ea(a) + 0x4d4));
  gabi::Local<cXyz> probe;
  x = read<float>(ea(a) + 0x314);
  y = read<float>(ea(a) + 0x318);
  a->particleRotation.z = 0;
  s16 rotation = read<s16>(ea(a) + 0x32a);
  probe->x = x;
  probe->y = y + 1000.0f;
  a->particleRotation.y = rotation;
  z = read<float>(ea(a) + 0x31c);
  a->particleRotation.x = 0;
  probe->z = z;
  float water = gabi::call<float>(0x024f17d4, probe.get());
  y = read<float>(ea(a) + 0x318);
  // PPC bge checks !(LT), including unordered values.
  if (!(y < water)) {
    if (a->mode == 2) {
      write<u32>(ea(a) + 0x39c, 0);
      return 0;
    }
    write<u32>(ea(a) + 0x39c, 4);
    write<u8>(ea(a) + 0x38a, 0x22);
  } else if (read<u32>(ea(a) + 0x658)) {
    removeParticle(a);
    if (a->mode == 2) {
      write<u32>(ea(a) + 0x39c, 0);
      return 0;
    }
  }
  if (a->mode == 2)
    write<u32>(ea(a) + 0x39c, 0);
  else {
    play = gabi::call<u32>(0x025200d4);
    if (read<u8>(play + 0x5292))
      write<u32>(ea(a) + 0x39c, 0);
  }
  return 0;
}
VERIFY(0x0211dcac, Execute);
BOOL ExecuteWrapper(Actor *a) {
  WWHD_FUNC(0x0211deec, BOOL, a);
  return Execute(a);
}
VERIFY(0x0211deec, ExecuteWrapper);
BOOL Draw(Actor *a) {
  WWHD_FUNC(0x0211def0, BOOL, a);
  if (!a->appeared)
    return 0;
  u32 light = gabi::call<u32>(0x02555d0c);
  gabi::call(0x025626a4, at<void>(light), 0, at<void>(ea(a) + 0x314),
             at<void>(ea(a) + 0x110));
  light = gabi::call<u32>(0x02555d0c);
  gabi::call(0x02562f5c, at<void>(light), a->model.get(),
             at<void>(ea(a) + 0x110));
  u32 data = read<u32>(gabi::ea(a->model.get()) + 0xac);
  float frame = read<float>(ea(a) + 0x44c);
  gabi::call(0x025e7fc4, at<void>(ea(a) + 0x448), at<void>(data), frame);
  data = read<u32>(gabi::ea(a->model.get()) + 0xac);
  frame = read<float>(ea(a) + 0x3d0);
  gabi::call(0x025e83fc, at<void>(ea(a) + 0x3cc), at<void>(data), frame);
  gabi::call(0x025e2de0, a->model.get(), 0);
  data = read<u32>(gabi::ea(a->model.get()) + 0xac);
  write<u32>(data + 0x44, 0);
  data = read<u32>(gabi::ea(a->model.get()) + 0xac);
  write<u32>(data + 0x48, 0);
  return 1;
}
VERIFY(0x0211def0, Draw);
BOOL DrawWrapper(Actor *a) {
  WWHD_FUNC(0x0211dfb4, BOOL, a);
  return Draw(a);
}
VERIFY(0x0211dfb4, DrawWrapper);
void WaitInit(Actor *a) {
  WWHD_FUNC(0x0211dfb8, void, a);
  a->mode = 0;
}
VERIFY(0x0211dfb8, WaitInit);
void DamageInit(Actor *a) {
  WWHD_FUNC(0x0211dfc4, void, a);
  a->mode = 1;
  a->damaged = 1;
  u32 model = gabi::ea(a->model.get());
  a->scaleAnimationIndex = 8;
  u32 data = read<u32>(model + 0xac);
  gabi::Local<ResourceLocal> brkName, btkName;
  u32 brk = resource(brkName, 0x18);
  a->brkResource = at<void>(brk);
  gabi::call(0x025e8154, at<void>(ea(a) + 0x3cc), at<void>(data), at<void>(brk),
             1, 0, 1.0f, 0, -1, 1, 0);
  u32 btk = resource(btkName, 0x23);
  a->btkResource = at<void>(btk);
  gabi::call(0x025e7ce0, at<void>(ea(a) + 0x448), at<void>(data), at<void>(btk),
             1, 0, 1.0f, 0, -1, 1, 0);
  parentSound(gabi::ea(a->parent.get()), 0x48c8);
}
VERIFY(0x0211dfc4, DamageInit);
void CheckHit(Actor *a) {
  WWHD_FUNC(0x0211e114, void, a);
  gabi::call(0x02515e50, at<void>(ea(a) + 0x61c));
  u32 hit = gabi::call<u32>(0x02516300, at<void>(ea(a) + 0x4d4));
  if (!hit)
    return;
  gabi::call(0x025e1fd8);
  u32 type = read<u32>(hit + 0x10);
  s32 damage;
  u32 sound;
  if (type & (0x4000 | 0x80000 | 0x40000)) {
    damage = 2;
    sound = 0x2834;
  } else if (type & 0x100000) {
    damage = 4;
    sound = 0x2834;
  } else if (type & 0x8000) {
    damage = 2;
    sound = 0x2834;
  } else if (type & 0x40) {
    damage = 1;
    sound = 0x2833;
  } else
    return;
  actorSound(a, sound, 0x20);
  write<u8>(ea(a) + 0x3a1, u8(read<u8>(ea(a) + 0x3a1) - damage));
  u32 play = gabi::call<u32>(0x025200d4), player = read<u32>(play + 0x5b2c);
  gabi::Local<cXyz> position, scale;
  position->x = read<float>(ea(a) + 0x5a0);
  position->y = read<float>(ea(a) + 0x5a4);
  position->z = read<float>(ea(a) + 0x5a8);
  play = gabi::call<u32>(0x025200d4);
  gabi::call(0x025a847c, at<void>(read<u32>(play + 0x5ab0)), 0, 0x10,
             position.get(), 0, 0, 0xff, 0, -1, 0, 0, 0);
  if (read<s8>(ea(a) + 0x3a1) > 0) {
    DamageInit(a);
    return;
  }
  scale->x = 2;
  scale->y = 2;
  scale->z = 2;
  play = gabi::call<u32>(0x025200d4);
  gabi::call(0x025a847c, at<void>(read<u32>(play + 0x5ab0)), 0, 0xf,
             position.get(), at<void>(player + 0x328), scale.get(), 0xff, 0, -1,
             0, 0, 0);
  actorSound(a, 0x2828, 0);
  write<u8>(ea(a) + 0x3a1, 0);
  DeathInit(a);
}
VERIFY(0x0211e114, CheckHit);
void Wait(Actor *a) {
  WWHD_FUNC(0x0211e40c, void, a);
  CheckHit(a);
}
VERIFY(0x0211e40c, Wait);
void Damage(Actor *a) {
  WWHD_FUNC(0x0211e410, void, a);
  gabi::call(0x02587be0, at<void>(ea(a) + 0x63c), at<void>(0x101b4194), 8,
             at<void>(ea(a) + 0x648), 0.2f, 10.0f, 0.025f);
  float x = a->eyeScale.x;
  a->eyeScale.z = x;
  a->eyeScale.y = x;
  bool btkStopped =
      (read<u8>(ea(a) + 0x457) & 1) || read<float>(ea(a) + 0x448) == 0.0f;
  bool stopped = false;
  if (btkStopped)
    stopped =
        (read<u8>(ea(a) + 0x3db) & 1) || read<float>(ea(a) + 0x3cc) == 0.0f;
  if (stopped) {
    if (a->scaleAnimationIndex == 0)
      WaitInit(a);
  } else if (a->scaleAnimationIndex == 0)
    a->scaleAnimationIndex = 8;
}
VERIFY(0x0211e410, Damage);
void Death(Actor *a) {
  WWHD_FUNC(0x0211e4e8, void, a);
  a->dead = 1;
}
VERIFY(0x0211e4e8, Death);
void *HioConstruct(void *object) {
  WWHD_FUNC(0x0211e4f4, void *, object);
  u32 p = gabi::ea(object);
  if (!p) {
    p = gabi::call<u32>(0x0273ad10, 0x5c);
    if (!p)
      return nullptr;
  }
  write<u32>(p, 0x1000d4dc);
  gabi::call(0x02552be8, at<void>(p + 0x30));
  write<s16>(p + 0x20, 0x1000);
  write<s16>(p + 0x26, 0x1000);
  write<s16>(p + 4, 0xf);
  write<float>(p + 0x1c, 195.0f);
  write<s16>(p + 0x28, 0x1000);
  write<s16>(p + 0x2c, 0x2000);
  write<s16>(p + 0x2e, 0x2000);
  write<float>(p + 0x18, 30);
  write<float>(p + 0x14, 0);
  write<float>(p + 8, 5);
  write<s16>(p + 0x2a, 0x7fff);
  write<s16>(p + 0x22, 4);
  write<float>(p + 0xc, 8);
  write<float>(p + 0x10, 0);
  write<s16>(p + 0x24, 0x4000);
  return at<void>(p);
}
VERIFY(0x0211e4f4, HioConstruct);
void StaticInit() {
  WWHD_FUNC(0x0211e5c4, void);
  write<u32>(0x10463a60, 0);
  write<u32>(0x10463a58, 0);
  write<u32>(0x10463a64, 0);
  write<u32>(0x10463a5c, 0);
  gabi::call(0x028f026c, at<void>(0x101b41b4));
  float minimum = read<float>(0x1000d560), maximum = read<float>(0x1000d564);
  write<float>(0x10463a4c, minimum);
  write<float>(0x10463a50, maximum);
  gabi::call(0x028ed6f8, at<void>(0x10463a54));
  gabi::call(0x028f026c, at<void>(0x101b41c0));
  gabi::call(0x028eab2c, at<void>(0x10463a55));
  gabi::call(0x028f026c, at<void>(0x101b41cc));
  HioConstruct(at<void>(Hio));
}
VERIFY(0x0211e5c4, StaticInit);
void HioDestroy(void *object, u32 flags) {
  WWHD_FUNC(0x0211e664, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273af40, object);
}
VERIFY(0x0211e664, HioDestroy);
BOOL IsDelete(Actor *a) {
  WWHD_FUNC(0x0211e678, BOOL, a);
  return 1;
}
VERIFY(0x0211e678, IsDelete);
void Destroy(Actor *a, u32 flags) {
  WWHD_FUNC(0x0211e680, void, a, flags);
  if (!a)
    return;
  gabi::call(0x02515860, at<void>(ea(a) + 0x600), 2);
  gabi::call(0x02515ae8, at<void>(ea(a) + 0x4d4), 2);
  gabi::call(0x025d50bc, a, 0);
  if (flags & 1)
    gabi::call(0x0273af40, a);
}
VERIFY(0x0211e680, Destroy);
void HioMessage(void *object) { WWHD_FUNC(0x0211e6ec, void, object); }
VERIFY(0x0211e6ec, HioMessage);
} // namespace OctoEye
