// WWHD port of the sea barrel course actor. See wwhd_src/README.md.
#include "d/actor/d_a_coming2.h"
using daComing2::Act_c;
namespace {
template<class T> T read(u32 address, u32 offset = 0) {
  return gabi::load<T>(address + offset);
}
template<class T> void write(u32 address, u32 offset, T value) {
  gabi::store<T>(address + offset, value);
}
void* ptr(u32 address, u32 offset = 0) { return gabi::at<void>(address + offset); }
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 player() { return read<u32>(play(), 0x5B2C); }
u32 dispatch(Act_c* actor, u32 descriptor) {
  s32 delta = read<s16>(descriptor);
  s32 index = read<s16>(descriptor, 2);
  u32 object = gabi::ea(actor) + delta;
  u32 target;
  if (index < 0)
    target = read<u32>(descriptor, 4);
  else {
    s32 vptrOffset = read<s16>(descriptor, 6);
    u32 table = read<u32>(object + vptrOffset);
    target = read<u32>(table, (u32)index * 8 + 4);
  }
  return gabi::call<u32>(target, ptr(object));
}
}

BOOL coming2_processInit(Act_c* actor, s32 process) {
  WWHD_FUNC(0x02113304, BOOL, actor, process);
  if ((u32)process < 5 && dispatch(actor, 0x1000C778 + (u32)process * 8)) {
    actor->process = process;
    return 1;
  }
  return 0;
}
VERIFY(0x02113304, coming2_processInit);

void coming2_processMain(Act_c* actor) {
  WWHD_FUNC(0x02113680, void, actor);
  s32 process = actor->process;
  if ((u32)process < 5)
    dispatch(actor, 0x1000C944 + (u32)process * 8);
}
VERIFY(0x02113680, coming2_processMain);

BOOL coming2_requestBarrelExit(Act_c* actor, s32 index) {
  WWHD_FUNC(0x021136D8, BOOL, actor, index);
  u32 id = actor->barrels[index].processId;
  if (id == 0xFFFFFFFF)
    return 1;
  gabi::Local<be<u32>> barrel;
  if (gabi::call<BOOL>(0x025D54C4, id, barrel.get()) && *barrel != 0) {
    write<u8>(*barrel, 0x590, 1);
    return 1;
  }
  return 0;
}
VERIFY(0x021136D8, coming2_requestBarrelExit);

BOOL coming2_requestAllBarrelExit(Act_c* actor) {
  WWHD_FUNC(0x02113D2C, BOOL, actor);
  s32 completed = 0;
  for (s32 index = 0; index < 15; ++index)
    if (gabi::call<BOOL>(0x021136D8, actor, index))
      ++completed;
  return completed == 15;
}
VERIFY(0x02113D2C, coming2_requestAllBarrelExit);

BOOL coming2_requestAllBuoyExit(Act_c* actor) {
  WWHD_FUNC(0x02113DA8, BOOL, actor);
  gabi::Local<be<u32>> buoy;
  for (s32 index = 0; index < 2; ++index) {
    u32 id = actor->buoys[index].processId;
    if (id != 0xFFFFFFFF && gabi::call<BOOL>(0x025D54C4, id, buoy.get()) && *buoy != 0) {
      write<u8>(*buoy, 0x590, 1);
      actor->buoys[index].processId = 0xFFFFFFFF;
    }
  }
  return actor->buoys[0].processId == 0xFFFFFFFF && actor->buoys[1].processId == 0xFFFFFFFF;
}
VERIFY(0x02113DA8, coming2_requestAllBuoyExit);

void coming2_barrelExecute(Act_c* actor, s32 index) {
  WWHD_FUNC(0x02113754, void, actor, index);
  auto& barrel = actor->barrels[index];
  u32 id = barrel.processId;
  if (id == 0xFFFFFFFF)
    return;
  gabi::Local<be<u32>> found;
  if (gabi::call<s32>(0x025D54C4, id, found.get()) == 1) {
    s16 timer = barrel.timer; // The HD reload occurs before reading the found actor's flag.
    if (*found != 0)
      barrel.itemGiven = read<u8>(*found, 0x592);
    if (timer == 0)
      gabi::call(0x021136D8, actor, index);
    else if (timer > 0)
      barrel.timer = timer - 1;
  } else if (barrel.itemGiven == 0) {
    gabi::call(0x0211321C, actor, index);
    gabi::call(0x02113304, actor, 0);
  }
  // HD accepts any nonzero flag here, unlike GC's final equality with 1.
  if (barrel.itemGiven != 0) {
    if ((u32)(s32)(s16)actor->nextBarrel >= 15)
      gabi::call(0x02113304, actor, 0);
    else {
      gabi::call(0x021136D8, actor, index);
      gabi::call(0x0211321C, actor, index);
      actor->spawnRequested = 1;
    }
  }
}
VERIFY(0x02113754, coming2_barrelExecute);

void coming2_barrelMain(Act_c* actor) {
  WWHD_FUNC(0x02113888, void, actor);
  for (s32 index = 0; index < 15; ++index)
    gabi::call(0x02113754, actor, index);
}
VERIFY(0x02113888, coming2_barrelMain);

void coming2_clearMain(Act_c* actor) {
  WWHD_FUNC(0x02113F5C, void, actor);
  if (gabi::call<BOOL>(0x02113D2C, actor) && gabi::call<BOOL>(0x02113DA8, actor)) {
    gabi::call(0x02113254, actor);
    gabi::call(0x02113288, actor);
    gabi::call(0x021132F4, actor);
    gabi::call(0x02113304, actor, 1);
  }
}
VERIFY(0x02113F5C, coming2_clearMain);

s32 coming2_create(Act_c* actor) {
  WWHD_FUNC(0x021133BC, s32, actor);
  if (!(actor->actor_condition & 8)) {
    if (actor) {
      gabi::call(0x025D4ED0, actor);
      actor->__vtbl = 0x1000C924;
    }
    actor->actor_condition = (u32)actor->actor_condition | 8;
  }
  actor->previousRoom = -1;
  gabi::call(0x02113254, actor);
  gabi::call(0x02113288, actor);
  gabi::call(0x021132F4, actor);
  gabi::call(0x02113304, actor, 0);
  u32 currentPlayer = player();
  actor->startingPosition.x = actor->current.pos.x;
  actor->startingPosition.z = actor->current.pos.z;
  actor->startingPosition.y = actor->current.pos.y;
  actor->startingAngle = actor->shape_angle.y;
  if (currentPlayer)
    actor->previousRoom = read<u8>(currentPlayer, 0x326);
  return 4;
}
VERIFY(0x021133BC, coming2_create);

void coming2_chaseShip(Act_c* actor) {
  WWHD_FUNC(0x02113478, void, actor);
  gabi::Local<be<u32>> id;
  *id = actor->parentActorID;
  if (*id != 0xFFFFFFFF) {
    u32 ship = gabi::call<u32>(0x025D5218, ptr(0x025E1234), id.get());
    if (ship) {
      actor->current.pos.x = read<f32>(ship, 0x314);
      actor->current.pos.y = read<f32>(ship, 0x318);
      actor->current.pos.z = read<f32>(ship, 0x31C);
      actor->shape_angle.x = read<s16>(ship, 0x328);
      actor->shape_angle.y = read<s16>(ship, 0x32A);
      actor->shape_angle.z = read<s16>(ship, 0x32C);
    }
  }
}
VERIFY(0x02113478, coming2_chaseShip);

BOOL coming2_inLargeSea(Act_c* actor, const cXyz* position) {
  WWHD_FUNC(0x02113AB0, BOOL, actor, position);
  return std::fabs((f32)position->x) < 349000.0f && std::fabs((f32)position->z) < 349000.0f;
}
VERIFY(0x02113AB0, coming2_inLargeSea);

BOOL coming2_makeParam(Act_c* actor, cXyz* position, be<s32>* type, be<s32>* item, be<u8>* hasFlag) {
  WWHD_FUNC(0x02113E50, BOOL, actor, position, type, item, hasFlag);
  gabi::Local<cXyz> waterPosition;
  *waterPosition = actor->spawnPosition;
  if (!gabi::call<BOOL>(0x02113AE8, actor, &waterPosition->y, waterPosition.get()))
    return 0;
  *position = *waterPosition;
  *type = 1;
  *hasFlag = 0;
  u32 index = (s32)(s16)actor->nextBarrel;
  *item = index < 15 ? read<s32>(0x101B3C1C + index * 4) : 0x16;
  return 1;
}
VERIFY(0x02113E50, coming2_makeParam);

void coming2_waitMain(Act_c* actor) {
  WWHD_FUNC(0x02113FD4, void, actor);
  u32 currentPlayer = player();
  if (!currentPlayer)
    return;
  if (read<u32>(play(), 0x5CD8) & 0x10000) {
    if ((f32)actor->shipSpeed > 35.0f &&
        gabi::call<s32>(0x025B8BB0, ptr(read<u32>(0x101F84DC), 0x644), 0x7EFF) >= 2) {
      s16 timer = actor->waitTimer;
      if (timer > 0) {
        timer = (s16)(timer - 1);
        actor->waitTimer = timer;
      }
      if ((s8)actor->previousRoom != read<s8>(currentPlayer, 0x326) && timer == 0) {
        actor->startingPosition.y = actor->current.pos.y;
        actor->startingAngle = actor->shape_angle.y;
        actor->startingPosition.z = actor->current.pos.z;
        actor->startingPosition.x = actor->current.pos.x;
        gabi::call(0x02113304, actor, 2);
      }
    } else
      gabi::call(0x02113304, actor, 1);
  } else {
    actor->previousRoom = read<u8>(currentPlayer, 0x326);
    gabi::call(0x02113304, actor, 1);
  }
}
VERIFY(0x02113FD4, coming2_waitMain);

BOOL coming2_setBuoysInit(Act_c* actor) {
  WWHD_FUNC(0x021140E0, BOOL, actor);
  u32 currentPlayer = player();
  if (!currentPlayer)
    return 0;
  actor->startingRoom = read<u8>(currentPlayer, 0x326);
  // HD consumes two points as two distinct get/decrement/set operations.
  for (s32 i = 0; i < 2; ++i) {
    u32 count = gabi::call<u32>(0x025B8BB0, ptr(read<u32>(0x101F84DC), 0x644), 0x7EFF);
    u32 decremented = count - 1;
    u32 value = (s32)decremented < 0 ? 0 : (decremented & 0xFF);
    gabi::call(0x025B8AF4, ptr(read<u32>(0x101F84DC), 0x644), 0x7EFF, value);
  }
  return 1;
}
VERIFY(0x021140E0, coming2_setBuoysInit);

struct Coming2Safety { be<u32> position; be<s32> safe; };
void* coming2_safetyCallback(fopAc_ac_c* actor, Coming2Safety* callback) {
  WWHD_FUNC(0x021141AC, void*, actor, callback);
  if (gabi::call<BOOL>(0x025D4604, actor) && actor->group == 2) {
    u32 position = callback->position;
    f32 z = (f32)actor->current.pos.z - read<f32>(position, 8);
    f32 x = (f32)actor->current.pos.x - read<f32>(position);
    f32 squared = gabi::fmadds(x, x, z * z);
    if (squared < 200.0f) {
      callback->safe = 0;
      return nullptr;
    }
  }
  return actor;
}
VERIFY(0x021141AC, coming2_safetyCallback);

BOOL coming2_positionClear(cXyz* position) {
  WWHD_FUNC(0x02114250, BOOL, position);
  gabi::Local<Coming2Safety> callback;
  callback->position = gabi::ea(position);
  callback->safe = 1;
  gabi::call(0x025D5218, ptr(0x021141AC), callback.get());
  return callback->safe == 1;
}
VERIFY(0x02114250, coming2_positionClear);

BOOL coming2_execute(Act_c* actor) {
  WWHD_FUNC(0x021138E0, BOOL, actor);
  gabi::Local<be<u32>> id;
  *id = actor->parentActorID;
  if (*id != 0xFFFFFFFF) {
    u32 ship = gabi::call<u32>(0x025D5218, ptr(0x025E1234), id.get());
    if (ship) {
      gabi::call(0x02113478, actor);
      gabi::call(0x021134F8, actor);
      if (read<u8>(play(), 0x5292) != 0 || read<u32>(ship, 0x704) != 0 || read<u32>(ship, 0x70C) != 0) {
        actor->previousRoom = read<u8>(player(), 0x326);
        gabi::call(0x02113304, actor, 0);
      } else
        gabi::call(0x02113680, actor);
      gabi::call(0x02113888, actor);
    }
  }
  return 1;
}
VERIFY(0x021138E0, coming2_execute);

void coming2_staticInit() {
  WWHD_FUNC(0x02114820, void);
  write<u32>(0x10462D44, 8, 0);
  write<u32>(0x10462D44, 0, 0);
  write<u32>(0x10462D44, 12, 0);
  write<u32>(0x10462D44, 4, 0);
  gabi::call(0x028F026C, ptr(0x101B3C7C));
  f32 first = read<f32>(0x1000C994);
  f32 second = read<f32>(0x1000C998);
  write<f32>(0x10462D38, 0, first);
  write<f32>(0x10462D3C, 0, second);
  gabi::call(0x028ED6F8, ptr(0x10462D40));
  gabi::call(0x028F026C, ptr(0x101B3C88));
  gabi::call(0x028EAB2C, ptr(0x10462D41));
  gabi::call(0x028F026C, ptr(0x101B3C94));
}
VERIFY(0x02114820, coming2_staticInit);

void coming2_groundCheckDestroy(void* object, u32 flags) {
  WWHD_FUNC(0x021148B4, void, object, flags);
  if (object) {
    u32 address = gabi::ea(object);
    write<u32>(address, 0x20, 0x1000C7D4);
    write<u32>(address, 0x40, 0x1000C7F4);
    write<u32>(address, 0x4C, 0x1000C7B4);
    gabi::call(0x02008DAC, object, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, object);
  }
}
VERIFY(0x021148B4, coming2_groundCheckDestroy);

void coming2_waterCheckDestroy(void* object, u32 flags) {
  WWHD_FUNC(0x0211492C, void, object, flags);
  if (object) {
    u32 address = gabi::ea(object);
    write<u32>(address, 0x20, 0x1000C844);
    write<u32>(address, 0x24, 0x1000C864);
    write<u32>(address, 0x30, 0x1000C7B4);
    gabi::call(0x02008B4C, ptr(address, 0x10), 0);
    if (flags & 1)
      gabi::call(0x0273AF40, object);
  }
}
VERIFY(0x0211492C, coming2_waterCheckDestroy);

void coming2_lineCheckDestroy(void* object, u32 flags) {
  WWHD_FUNC(0x021149A4, void, object, flags);
  if (object) {
    u32 address = gabi::ea(object);
    write<u32>(address, 0x58, 0x1000C8D4);
    write<u32>(address, 0x64, 0x1000C7B4);
    write<u32>(address, 0x20, 0x1000C7A4);
    gabi::call(0x02008B4C, object, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, object);
  }
}
VERIFY(0x021149A4, coming2_lineCheckDestroy);

void coming2_destroy(Act_c* actor, u32 flags) {
  WWHD_FUNC(0x02114A1C, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x02114A1C, coming2_destroy);

void coming2_renewScope(Act_c* actor) {
  WWHD_FUNC(0x021134F8, void, actor);
  f32 z = (f32)actor->current.pos.z - (f32)actor->old.pos.z;
  f32 x = (f32)actor->current.pos.x - (f32)actor->old.pos.x;
  f32 speed = gabi::call<f32>(0x028F4384, gabi::fmadds(x, x, z * z));
  actor->shipSpeed = speed;
  f32 clamped = speed - 35.0f >= 0.0f ? speed : 35.0f;
  gabi::Local<cXyz> ahead;
  ahead->set(0.0f, 100.0f, clamped * 135.0f);
  s32 index = actor->nextBarrel;
  s32 angle = ((index + (s16)actor->side) & 1) ? -0x546 : 0x546;
  s32 spread = (((u32)index >> 2) & 15) * 360;
  angle = (s16)((s16)actor->startingAngle + (angle > 0 ? angle + spread : angle - spread));
  const cXyz* origin = index == 0 ? &actor->current.pos : &actor->barrels[index - 1].position;
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)origin->x, (f32)origin->y, (f32)origin->z);
  gabi::call(0x025F1C28, ptr(0x1048D0CC), angle);
  gabi::Local<Mtx34> matrix;
  gabi::call(0x028E90D4, ptr(0x1048D0CC), matrix.get());
  gabi::call(0x028E8F64, matrix.get(), ahead.get(), &actor->spawnPosition);
}
VERIFY(0x021134F8, coming2_renewScope);

u32 coming2_noShipObstacle(Act_c* actor, cXyz* position) {
  WWHD_FUNC(0x021139AC, u32, actor, position);
  constexpr u32 checker = 0x10462D54;
  if (read<u32>(0x10462E64) == 0) {
    write<u32>(0x10462E64, 0, 1);
    gabi::call(0x02008FEC, ptr(checker));
    write<u32>(checker, 0x68, 1);
    write<u32>(checker, 0x64, 0x1000C904);
    write<u32>(checker, 0, checker + 0x58);
    write<u32>(checker, 0x10, 0x1000C8E4);
    write<u32>(checker, 4, checker + 0x64);
    write<u32>(checker, 0x20, 0x1000C8F4);
    write<u8>(checker, 0x5D, 0);
    write<u8>(checker, 0x62, 0);
    write<u8>(checker, 0x5C, 1);
    write<u8>(checker, 0x60, 0);
    write<u8>(checker, 0x61, 0);
    write<u8>(checker, 0x5F, 0);
    write<u32>(checker, 0x58, 0x1000C914);
    write<u8>(checker, 0x5E, 0);
    gabi::call(0x028F026C, ptr(0x101B3C58));
  }
  gabi::call(0x024F1AFC, ptr(checker), &actor->current.pos, position, 0);
  return gabi::call<u32>(0x02008860, ptr(play(), 0x12A0), ptr(checker)) ^ 1;
}
VERIFY(0x021139AC, coming2_noShipObstacle);

BOOL coming2_waterHeight(Act_c* actor, be<f32>* height, const cXyz* position) {
  WWHD_FUNC(0x02113AE8, BOOL, actor, height, position);
  constexpr f32 invalidHeight = -1000000000.0f;
  f32 wave = invalidHeight;
  if (gabi::call<BOOL>(0x0246B6A4, (f32)position->x, (f32)position->z))
    wave = gabi::call<f32>(0x0246BA0C, (f32)position->x, (f32)position->z);
  else {
    constexpr u32 water = 0x10462DC0;
    if (read<u32>(0x10462E68) == 0) {
      write<u32>(0x10462E68, 0, 1);
      gabi::call(0x024F22DC, ptr(water));
      gabi::call(0x028F026C, ptr(0x101B3C64));
    }
    f32 x = position->x, y = position->y, z = position->z;
    write<f32>(water, 0x38, x);
    write<f32>(water, 0x40, z);
    write<f32>(water, 0x3C, y - 1000.0f);
    write<f32>(water, 0x44, y + 1000.0f);
    if (gabi::call<BOOL>(0x024EF7C0, ptr(play(), 0x12A0), ptr(water)))
      wave = read<f32>(water, 0x48);
  }
  *height = wave;
  if (!(wave > invalidHeight))
    return 0;
  constexpr u32 ground = 0x10462E10;
  if (read<u32>(0x10462E6C) == 0) {
    write<u32>(0x10462E6C, 0, 1);
    gabi::call(0x02008E0C, ptr(ground));
    write<u32>(ground, 0x50, 1);
    write<u32>(ground, 0, ground + 0x40);
    write<u32>(ground, 0x10, 0x1000C804);
    write<u8>(ground, 0x45, 0);
    write<u8>(ground, 0x4A, 0);
    write<u32>(ground, 0x4C, 0x1000C824);
    write<u8>(ground, 0x44, 1);
    write<u32>(ground, 4, ground + 0x4C);
    write<u32>(ground, 0x20, 0x1000C814);
    write<u8>(ground, 0x48, 0);
    write<u8>(ground, 0x49, 0);
    write<u8>(ground, 0x47, 0);
    write<u8>(ground, 0x46, 0);
    write<u32>(ground, 0x40, 0x1000C834);
    gabi::call(0x028F026C, ptr(0x101B3C70));
  }
  f32 x = position->x, y = position->y, z = position->z;
  write<f32>(ground, 0x24, x);
  write<f32>(ground, 0x2C, z);
  write<f32>(ground, 0x28, y + 20000.0f);
  f32 floor = gabi::call<f32>(0x02008974, ptr(play(), 0x12A0), ptr(ground));
  return !(floor > wave - 100.0f);
}
VERIFY(0x02113AE8, coming2_waterHeight);

void coming2_checkStartMain(Act_c* actor) {
  WWHD_FUNC(0x021144C4, void, actor);
  if ((s16)actor->crossingTimer < 180) {
    f32 ax = actor->old.pos.x, az = actor->old.pos.z;
    f32 bx = actor->current.pos.x, bz = actor->current.pos.z;
    f32 cx = actor->buoys[0].position.x, cz = actor->buoys[0].position.z;
    f32 dx = actor->buoys[1].position.x, dz = actor->buoys[1].position.z;
    f32 segmentX = bx - ax, segmentZ = bz - az;
    f32 side0 = gabi::fmsubs(segmentX, cz - az, segmentZ * (cx - ax));
    f32 side1 = gabi::fmsubs(segmentX, dz - az, segmentZ * (dx - ax));
    if (side0 * side1 < 0.0f) {
      f32 buoyX = cx - dx, buoyZ = cz - dz;
      f32 oldSide = gabi::fmsubs(buoyX, az - dz, buoyZ * (ax - dx));
      f32 newSide = gabi::fmsubs(buoyX, bz - dz, buoyZ * (bx - dx));
      if (oldSide * newSide < 0.0f) {
        gabi::call(0x02113DA8, actor);
        gabi::call(0x02113304, actor, 4);
      }
    }
    actor->crossingTimer = (s16)actor->crossingTimer + 1;
  } else {
    u32 currentPlayer = player();
    if (currentPlayer) {
      actor->previousRoom = read<u8>(currentPlayer, 0x326);
      gabi::call(0x02113304, actor, 0);
    }
  }
}
VERIFY(0x021144C4, coming2_checkStartMain);

void coming2_gameMain(Act_c* actor) {
  WWHD_FUNC(0x02114630, void, actor);
  u32 originalIndex = (s32)(s16)actor->nextBarrel;
  if (originalIndex >= 15)
    return;
  if ((s16)actor->spawnRequested == 1) {
    gabi::Local<cXyz> position;
    gabi::Local<be<s32>> type, item;
    gabi::Local<be<u8>> hasFlag;
    if (actor->barrels[originalIndex].processId == 0xFFFFFFFF &&
        gabi::call<BOOL>(0x02113E50, actor, position.get(), type.get(), item.get(), hasFlag.get())) {
      if (gabi::call<BOOL>(0x02113AB0, actor, position.get())) {
        if (gabi::call<BOOL>(0x02114250, position.get()) &&
            gabi::call<BOOL>(0x021139AC, actor, position.get())) {
          s32 barrelType = *type, droppedItem = *item;
          u32 flag = *hasFlag;
          gabi::Local<csXyz> rotation;
          gabi::call(0x0201A478, rotation.get(), 0, (s32)(s16)actor->startingAngle, 0);
          u32 parameters = (droppedItem & 0x3F) | 0x007F0000 |
                           ((u32)barrelType << 24) | ((flag ^ 1) << 8) | 0x10000000;
          u32 id = gabi::call<u32>(0x025D5834, 0x1C9, parameters, position.get(), -1,
                                   rotation.get(), 0, -1, 0);
          actor->barrels[(s16)actor->nextBarrel].processId = id;
          if (id != 0xFFFFFFFF) {
            actor->spawnRequested = 0;
            actor->barrels[originalIndex].itemGiven = 0;
            actor->barrels[(s16)actor->nextBarrel].position = *position;
            actor->barrels[(s16)actor->nextBarrel].timer = 0x93 - (s16)actor->nextBarrel;
            // HD stores 1 and returns immediately; GC stores 0 then increments below.
            actor->spawnTimer = 1;
            actor->nextBarrel = (s16)actor->nextBarrel + 1;
            return;
          }
        }
      } else
        gabi::call(0x02113304, actor, 0);
    }
    s16 timer = actor->spawnTimer;
    actor->spawnTimer = timer + 1;
    if (timer >= 60)
      gabi::call(0x02113304, actor, 0);
  } else
    actor->spawnTimer = 0;
}
VERIFY(0x02114630, coming2_gameMain);

void coming2_setBuoysMain(Act_c* actor) {
  WWHD_FUNC(0x02114298, void, actor);
  f32 lateral;
  do {
    lateral = gabi::call<f32>(0x02019918, 1600.0f);
  } while (!(std::fabs(lateral) > 700.0f));
  struct Positions { cXyz point[2]; };
  gabi::Local<Positions> offsets;
  f32 forward = (f32)actor->shipSpeed * 120.0f;
  offsets->point[0].set(lateral + 500.0f, 0.0f, forward);
  offsets->point[1].set(lateral + -500.0f, 0.0f, forward);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)actor->current.pos.x,
             (f32)actor->current.pos.y, (f32)actor->current.pos.z);
  gabi::call(0x025F1C28, ptr(0x1048D0CC), (s32)(s16)actor->shape_angle.y);
  gabi::Local<Mtx34> matrix;
  gabi::call(0x028E90D4, ptr(0x1048D0CC), matrix.get());
  gabi::Local<cXyz> position;
  gabi::Local<csXyz> rotation;
  for (s32 index = 0; index < 2; ++index) {
    if (actor->buoys[index].processId == 0xFFFFFFFF) {
      gabi::call(0x028E8F64, matrix.get(), &offsets->point[index], position.get());
      if (gabi::call<BOOL>(0x02113AE8, actor, &position->y, position.get()) &&
          gabi::call<BOOL>(0x02113AB0, actor, position.get()) &&
          gabi::call<BOOL>(0x02114250, position.get()) &&
          gabi::call<BOOL>(0x021139AC, actor, position.get())) {
        gabi::call(0x0201A478, rotation.get(), 0, (s32)(s16)actor->startingAngle, 0);
        u32 id = gabi::call<u32>(0x025D5834, 0x1C9, 0x117F043F, position.get(), -1,
                                 rotation.get(), 0, -1, 0);
        actor->buoys[index].processId = id;
        if (id != 0xFFFFFFFF)
          actor->buoys[index].position = *position;
      }
    }
  }
  if (actor->buoys[0].processId != 0xFFFFFFFF && actor->buoys[1].processId != 0xFFFFFFFF)
    gabi::call(0x02113304, actor, 3);
  else
    gabi::call(0x02113304, actor, 0);
}
VERIFY(0x02114298, coming2_setBuoysMain);

void coming2_initBarrel(Act_c* actor, s32 index) {
  WWHD_FUNC(0x0211321C, void, actor, index);
  auto& barrel = actor->barrels[index];
  barrel.processId = 0xFFFFFFFF;
  barrel.timer = 0;
  barrel.itemGiven = 0;
  barrel.position = actor->current.pos;
}
VERIFY(0x0211321C, coming2_initBarrel);

void coming2_initBarrels(Act_c* actor) {
  WWHD_FUNC(0x02113254, void, actor);
  for (s32 index = 0; index < 15; ++index)
    gabi::call(0x0211321C, actor, index);
}
VERIFY(0x02113254, coming2_initBarrels);

void coming2_initCourse(Act_c* actor) {
  WWHD_FUNC(0x02113288, void, actor);
  actor->nextBarrel = 0;
  actor->waitTimer = 600;
  actor->side = gabi::ftoi(gabi::call<f32>(0x020198D8, 2.0f)) & 1;
  actor->spawnRequested = 0;
  actor->spawnTimer = 0;
}
VERIFY(0x02113288, coming2_initCourse);

void coming2_initBuoys(Act_c* actor) {
  WWHD_FUNC(0x021132F4, void, actor);
  actor->buoys[0].processId = 0xFFFFFFFF;
  actor->buoys[1].processId = 0xFFFFFFFF;
}
VERIFY(0x021132F4, coming2_initBuoys);

BOOL coming2_clearInit(Act_c* actor) {
  WWHD_FUNC(0x02113F18, BOOL, actor);
  u32 currentPlayer = player();
  if (currentPlayer)
    actor->previousRoom = read<s8>(currentPlayer, 0x326);
  return 1;
}
VERIFY(0x02113F18, coming2_clearInit);

BOOL coming2_waitInit(Act_c* actor) {
  WWHD_FUNC(0x02113FBC, BOOL, actor);
  actor->waitTimer = 600;
  actor->nextBarrel = 0;
  return 1;
}
VERIFY(0x02113FBC, coming2_waitInit);

BOOL coming2_checkStartInit(Act_c* actor) {
  WWHD_FUNC(0x021144B4, BOOL, actor);
  actor->crossingTimer = 0;
  return 1;
}
VERIFY(0x021144B4, coming2_checkStartInit);

BOOL coming2_gameInit(Act_c* actor) {
  WWHD_FUNC(0x02114604, BOOL, actor);
  s8 startingRoom = actor->startingRoom;
  u32 count = actor->gamesStarted;
  actor->spawnRequested = 1;
  actor->previousRoom = startingRoom;
  actor->gamesStarted = count + 1;
  actor->spawnTimer = 0;
  return 1;
}
VERIFY(0x02114604, coming2_gameInit);

s32 coming2_createMethod(Act_c* actor) {
  WWHD_FUNC(0x02114808, s32, actor);
  return gabi::call<s32>(0x021133BC, actor);
}
VERIFY(0x02114808, coming2_createMethod);
BOOL coming2_deleteMethod(Act_c* actor) {
  WWHD_FUNC(0x0211480C, BOOL, actor);
  return 1;
}
VERIFY(0x0211480C, coming2_deleteMethod);
BOOL coming2_executeMethod(Act_c* actor) {
  WWHD_FUNC(0x02114814, BOOL, actor);
  return gabi::call<BOOL>(0x021138E0, actor);
}
VERIFY(0x02114814, coming2_executeMethod);
BOOL coming2_drawMethod(Act_c* actor) {
  WWHD_FUNC(0x02114818, BOOL, actor);
  return 1;
}
VERIFY(0x02114818, coming2_drawMethod);
BOOL coming2_isDelete(Act_c* actor) {
  WWHD_FUNC(0x02114A70, BOOL, actor);
  return 1;
}
VERIFY(0x02114A70, coming2_isDelete);
