#include "d/actor/d_a_obj_pirateship.h"
#include "bindings.h"
using Ship = daObjPirateship_c;
namespace {
template <class T> T read(u32 p, u32 off = 0) { return gabi::load<T>(p + off); }
template <class T> void write(u32 p, u32 off, T v) {
  gabi::store<T>(p + off, v);
}
template <class T = void> T *ptr(u32 p, u32 off = 0) {
  return gabi::at<T>(p + off);
}
u32 guestAddress(const void *p) { return gabi::ea(p); }
u32 baseMatrix(u32 model) { return model ? model + 0xC8 : 0; }
} // namespace
void pirateship_ride(void *bg, void *ship, fopAc_ac_c *rider) {
  WWHD_FUNC(0x02384068, void, bg, ship, rider);
  rider->actor_status = rider->actor_status & ~0x80u;
}
VERIFY(0x02384068, pirateship_ride);
BOOL pirateship_path(cXyz *position, cXyz *from, cXyz *to, Ship *ship) {
  WWHD_FUNC(0x02384078, BOOL, position, from, to, ship);
  gabi::Local<cXyz> start, end, direction;
  f32 sx = from->x, tx = to->x, y = ship->current.pos.y, tz = to->z,
      sz = from->z;
  start->set(sx, y, sz);
  end->set(tx, y, tz);
  gabi::call(0x0201ADE0, end.get(), direction.get(), start.get());
  if (!gabi::call<BOOL>(0x0201B47C, direction.get()))
    return 1;
  s16 heading =
      gabi::call<s16>(0x020195B0, (f32)direction->x, (f32)direction->z);
  s16 turn = gabi::call<s16>(0x0200F378, &ship->current.angle.y, heading, 0x40,
                             0x40, 8);
  ship->shape_angle.y = ship->current.angle.y;
  f32 cosine = read<f32>(0x104A44FC, ((u16)turn >> 3) * 8);
  f32 speed = (read<f32>(0x1046BC88) + 5.0f) * std::fabs(cosine);
  return gabi::call<BOOL>(0x0200F62C, position, end.get(), speed) ? 1 : 0;
}
VERIFY(0x02384078, pirateship_path);
void pirateship_CreateWave(Ship *ship) {
  WWHD_FUNC(0x02384570, void, ship);
  ship->wakePos.copy(ship->current.pos);
  if (!ship->tailEmitter) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x025A847C, ptr(read<u32>(play, 0x5AB0)), 5, 0x8315,
               &ship->wakePos, &ship->current.angle, 0, 0,
               ptr(guestAddress(ship), 0x63C), -1, 0, 0, 0);
    u32 emitter = ship->tailEmitter;
    if (emitter)
      for (u32 offset : {0x224u, 0x240u, 0x238u, 0x23Cu, 0x220u, 0x228u})
        write<f32>(emitter, offset, 20.0f);
  }
  if (!ship->splashEmitter) {
    gabi::Local<be<u32>> secondary;
    gabi::call(0x025602F0, ptr(guestAddress(ship), 0x690), secondary.get());
    u32 play = gabi::call<u32>(0x025200D4);
    ship->splashEmitter =
        gabi::call<u32>(0x025A847C, ptr(read<u32>(play, 0x5AB0)), 0, 0x833D,
                        &ship->current.pos, &ship->current.angle, 0, 255, 0, -1,
                        ptr(guestAddress(ship), 0x690), 0, 0);
  }
}
VERIFY(0x02384570, pirateship_CreateWave);
void pirateship_DeleteWave(Ship *ship) {
  WWHD_FUNC(0x02384A48, void, ship);
  u32 emitter = ship->splashEmitter;
  if (emitter) {
    u32 flags = read<u32>(emitter, 0x254);
    write<s32>(emitter, 0x5C, -1);
    write<u32>(emitter, 0x254, flags | 1);
    ship->splashEmitter = 0;
  }
  gabi::call(0x025A9E38, ptr(guestAddress(ship), 0x63C));
}
VERIFY(0x02384A48, pirateship_DeleteWave);
BOOL pirateship_demo(Ship *ship) {
  WWHD_FUNC(0x02384974, BOOL, ship);
  u8 id = ship->demoActorID;
  if (!id || id > 32)
    return 0;
  u32 manager = read<u32>(0x101D5FFC);
  if (!manager) {
    gabi::call(0x0273AA24, ptr(0x1002E8E0), 0x23A, ptr(0x1002E8D0));
    manager = read<u32>(0x101D5FFC);
  }
  if (!gabi::call<u32>(0x02526E70, ptr(manager), id))
    return 0;
  gabi::call(0x02527028, ship, 10, 0, ptr(0x1002E9DC), 0, 0, 0, 0);
  return 1;
}
VERIFY(0x02384974, pirateship_demo);
void pirateship_SetWave(Ship *ship) {
  WWHD_FUNC(0x02384C2C, void, ship);
  u32 emitter = ship->tailEmitter;
  if (emitter) {
    f32 scale = read<f32>(0x1046BCA4);
    for (u32 offset : {0x224u, 0x238u, 0x240u, 0x23Cu, 0x228u, 0x220u})
      write<f32>(emitter, offset, scale);
    gabi::call(0x028E90D4, ptr(baseMatrix(ship->shipModel)), ptr(0x1048D0CC));
    gabi::call(0x028E8F64, ptr(0x1048D0CC), ptr(0x1046BC98), &ship->wakePos);
    f32 waterY = ship->wakePos.y;
    u32 flags = read<u32>(guestAddress(ship), 0x6BC);
    write<f32>(guestAddress(ship), 0x644, waterY);
    f32 indirect = read<f32>(0x1046BCA8), size = read<f32>(0x1046BCAC);
    write<f32>(guestAddress(ship), 0x678, indirect);
    write<f32>(guestAddress(ship), 0x67C, size);
    write<f32>(guestAddress(ship), 0x680, read<f32>(0x1046BCB0));
    write<f32>(guestAddress(ship), 0x648,
               (flags & 0x800) ? read<f32>(guestAddress(ship), 0x850)
                               : (f32)ship->wakePos.y);
    write<f32>(guestAddress(ship), 0x684, read<f32>(0x1046BCB4));
    emitter = ship->tailEmitter;
    write<f32>(emitter, 0x70, read<f32>(0x1046BCB8));
  }
  if (ship->splashEmitter) {
    gabi::Local<be<u32>> secondary;
    gabi::call(0x025602F0, ptr(guestAddress(ship), 0x690), secondary.get());
    emitter = ship->splashEmitter;
    u8 b = read<u8>(guestAddress(ship), 0x692),
       r = read<u8>(guestAddress(ship), 0x690),
       g = read<u8>(guestAddress(ship), 0x691);
    write<u8>(emitter, 0x244, r);
    write<u8>(emitter, 0x246, b);
    write<u8>(emitter, 0x245, g);
    emitter = ship->splashEmitter;
    f32 x = ship->current.pos.x, y = ship->current.pos.y,
        z = ship->current.pos.z;
    if (read<u8>(emitter, 0x262) >= 7)
      y = -y;
    write<f32>(emitter, 0x230, y);
    write<f32>(emitter, 0x22C, x);
    write<f32>(emitter, 0x234, z);
    emitter = ship->splashEmitter;
    s16 az = ship->current.angle.z, ay = ship->current.angle.y,
        ax = ship->current.angle.x;
    gabi::call(0x028245AC, ax, ay, az, ptr(emitter, 0x1F0));
  }
}
VERIFY(0x02384C2C, pirateship_SetWave);
BOOL pirateship_Create(Ship *ship) {
  WWHD_FUNC(0x02384F20, BOOL, ship);
  ship->cullMtx = baseMatrix(ship->shipModel);
  gabi::call(0x025D674C, ship, -1400.f, -200.f, -1800.f, 1400.f, 3300.f,
             2200.f);
  ship->cullSizeFar = 10.f;
  return 1;
}
VERIFY(0x02384F20, pirateship_Create);
BOOL pirateship_Execute(Ship *ship, be<u32> *matrix) {
  WWHD_FUNC(0x02384FA4, BOOL, ship, matrix);
  *matrix = 0;
  return 1;
}
VERIFY(0x02384FA4, pirateship_Execute);
void pirateship_hioDelete(void *obj, u32 flags) {
  WWHD_FUNC(0x023860F4, void, obj, flags);
  if (obj && (flags & 1))
    gabi::call(0x0273AF40, obj);
}
VERIFY(0x023860F4, pirateship_hioDelete);
BOOL pirateship_IsDelete(Ship *ship) {
  WWHD_FUNC(0x02386108, BOOL, ship);
  return 1;
}
VERIFY(0x02386108, pirateship_IsDelete);
void pirateship_stringNoop(void *str) { WWHD_FUNC(0x02386110, void, str); }
VERIFY(0x02386110, pirateship_stringNoop);
void *pirateship_vectorCtor(void *str) {
  WWHD_FUNC(0x02386114, void *, str);
  if (!str)
    str = gabi::call<void *>(0x0273AD10, 12);
  return str;
}
VERIFY(0x02386114, pirateship_vectorCtor);
BOOL pirateship_Delete(Ship *ship) {
  WWHD_FUNC(0x02386140, BOOL, ship);
  return 1;
}
VERIFY(0x02386140, pirateship_Delete);
void pirateship_pirateCreate(Ship *ship, be<s32> *indices) {
  WWHD_FUNC(0x023844DC, void, ship, indices);
  ship->piratesCreated = 1;
  while ((s32)*indices != -1) {
    u32 entry = 0x101CC50C + (u32)(s32)*indices * 0x1C;
    s8 room = ship->tevStr.mRoomNo;
    u32 parent = read<u32>(guestAddress(ship), 4), params = read<u32>(entry, 4),
        name = read<u32>(entry);
    gabi::call(0x025D5D88, ptr(name), parent, params, ptr(entry, 8), room,
               ptr(entry, 0x14), 0, 0);
    indices = ptr<be<s32>>(guestAddress(indices), 4);
  }
}
VERIFY(0x023844DC, pirateship_pirateCreate);
void pirateship_setMtx(Ship *ship) {
  WWHD_FUNC(0x02384690, void, ship);
  f32 x = ship->scale.x, z = ship->scale.z, y = ship->scale.y;
  u32 model = ship->shipModel;
  write<f32>(model, 0xBC, x);
  write<f32>(model, 0xC4, z);
  write<f32>(model, 0xC0, y);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)ship->current.pos.x,
             (f32)ship->current.pos.y, (f32)ship->current.pos.z);
  gabi::call(0x025F1B48, ptr(0x1048D0CC), (s16)ship->shape_angle.x,
             (s16)ship->shape_angle.y, (s16)ship->shape_angle.z);
  J3DModel_setBaseTRMtx(ptr<J3DModel>(ship->shipModel), ptr<Mtx34>(0x1048D0CC));
  u32 door = ship->doorActor;
  if (door) {
    if (!read<u32>(0x1046BCCC)) {
      ptr<cXyz>(0x1046BCC0)->set(0.f, 400.f, 475.f);
      write<u32>(0x1046BCCC, 0, 1);
      door = ship->doorActor;
    }
    gabi::call(0x028E90D4, ptr(baseMatrix(ship->shipModel)), ptr(0x1048D0CC));
    gabi::Local<cXyz> pos;
    gabi::call(0x028E8F64, ptr(0x1048D0CC), ptr(0x1046BCC0), pos.get());
    gabi::call(0x0252A7E8, ptr(door), pos.get(),
               (s16)((s16)ship->shape_angle.y + 0x8000));
  } else {
    u32 id = ship->doorID, found = 0;
    gabi::Local<be<u32>> searchID;
    *searchID = id;
    if (id != 0xFFFFFFFF)
      found = gabi::call<u32>(0x025D5218, ptr(0x025E1234), searchID.get());
    ship->doorActor = found;
  }
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)ship->current.pos.x,
             (f32)ship->current.pos.y + 3.f, (f32)ship->current.pos.z);
  gabi::call(0x025F1C28, ptr(0x1048D0CC), (s16)ship->shape_angle.y);
  J3DModel_setBaseTRMtx(ptr<J3DModel>(ship->seaModel), ptr<Mtx34>(0x1048D0CC));
  gabi::call(0x025E742C, ptr(guestAddress(ship), 0x400));
}
VERIFY(0x02384690, pirateship_setMtx);
BOOL pirateship_CreateHeap(Ship *ship) {
  WWHD_FUNC(0x02384DA0, BOOL, ship);
  struct Safe {
    be<u32> name, vt;
  };
  struct Res {
    be<u32> name, vt, pad;
  };
  gabi::Local<Safe> archive;
  archive->name = 0x1002EAB0;
  archive->vt = 0x1002E818;
  u32 data = gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)),
                             archive.get(), 14);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002EA04), 0x189, ptr(0x1002EA1C));
  ship->shipModel = gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x11000002);
  if (read<s8>(0x1046BC84) < 0)
    write<u8>(0x1046BC84, 0,
              gabi::call<u8>(0x025F0A10, ptr(0x1002E9FC), ptr(0x1046BC84)));
  if (!ship->shipModel)
    return 0;
  gabi::Local<Safe> arc2;
  gabi::Local<Res> sea;
  arc2->name = 0x1002EAB0;
  arc2->vt = 0x1002E818;
  sea->name = 0x1002EA30;
  sea->vt = 0x1002E818;
  sea->pad = 0x1002E818;
  u32 seaData = gabi::call<u32>(0x02606900, ptr(read<u32>(0x101F4F28)),
                                arc2.get(), sea.get());
  ship->seaModel = gabi::call<u32>(0x025E38E0, ptr(seaData), 0, 0x11020203);
  if (!ship->seaModel)
    return 0;
  gabi::Local<Safe> arc3;
  gabi::Local<Res> btk;
  arc3->name = 0x1002EAB0;
  arc3->vt = 0x1002E818;
  btk->name = 0x1002EA44;
  btk->vt = 0x1002E818;
  btk->pad = 0x1002E818;
  u32 animation = gabi::call<u32>(0x02606900, ptr(read<u32>(0x101F4F28)),
                                  arc3.get(), btk.get());
  return gabi::call<BOOL>(0x025E7CE0, ptr(guestAddress(ship), 0x400),
                          ptr(seaData), ptr(animation), 1, 2, 1.f, 0, -1, 0, 0)
             ? 1
             : 0;
}
VERIFY(0x02384DA0, pirateship_CreateHeap);
void pirateship_partsCreate(Ship *ship) {
  WWHD_FUNC(0x0238418C, void, ship);
  u8 variant = ship->mParameters >> 24;
  u32 id = gabi::call<u32>(
      0x025D5AA4, ptr(0x1002E944), read<u32>(guestAddress(ship), 4), 0,
      &ship->current.pos, (s8)ship->tevStr.mRoomNo, &ship->current.angle, 0, 0);
  ship->sailID = id;
  if (id == 0xFFFFFFFF)
    gabi::call(0x0273AA24, ptr(0x1002E94C), 0x123, ptr(0x1002E964));
  ship->flagID = gabi::call<u32>(
      0x025D5A20, 0xAD, read<u32>(guestAddress(ship), 4), 0, &ship->current.pos,
      (s8)ship->tevStr.mRoomNo, &ship->current.angle, 0, -1, 0);
  if (variant != 3) {
    gabi::Local<cXyz> pos;
    u32 trig = ((u16)ship->current.angle.y >> 3) * 8;
    f32 y = (f32)ship->current.pos.y + 700.f;
    f32 sine = read<f32>(0x104A44F8, trig),
        cosine = read<f32>(0x104A44FC, trig);
    f32 x = gabi::fmadds(850.f, sine, (f32)ship->current.pos.x),
        z = gabi::fmadds(850.f, cosine, (f32)ship->current.pos.z);
    pos->set(x, y, z);
    u32 catapult = gabi::call<u32>(
        0x025D5A20, 0x3A, read<u32>(guestAddress(ship), 4), 0, pos.get(),
        (s8)ship->tevStr.mRoomNo, &ship->current.angle, 0, -1, 0);
    trig = ((u16)ship->current.angle.y >> 3) * 8;
    x = ship->current.pos.x;
    y = (f32)ship->current.pos.y + 838.f;
    cosine = read<f32>(0x104A44FC, trig);
    z = gabi::fmadds(-788.f, cosine, (f32)ship->current.pos.z);
    sine = read<f32>(0x104A44F8, trig);
    x = gabi::fmadds(-788.f, sine, x);
    ship->catapultID = catapult;
    pos->set(x, y, z);
    ship->rudderID = gabi::call<u32>(
        0x025D5A20, 0x3C, read<u32>(guestAddress(ship), 4), 0, pos.get(),
        (s8)ship->tevStr.mRoomNo, &ship->current.angle, 0, -1, 0);
  }
  gabi::Local<cXyz> doorPos;
  gabi::Local<csXyz> angle;
  doorPos->set(ship->current.pos.x, ship->current.pos.y, ship->current.pos.z);
  gabi::call(0x0201A478, angle.get(), 0xFFF,
             (s16)((s16)ship->current.angle.y + 0x8000), 0);
  u32 doorType = (ship->mParameters >> 8) & 255;
  if (doorType > 1)
    doorType = 0;
  bool unlocked =
      gabi::call<BOOL>(0x025B8B94, ptr(read<u32>(0x101F84DC), 0x644), 0x520) !=
      0;
  angle->z = unlocked ? 0x1B18 : 0;
  u32 trig = ((u16)ship->current.angle.y >> 3) * 8;
  f32 x = gabi::fmadds(475.f, read<f32>(0x104A44F8, trig), (f32)doorPos->x);
  f32 z = gabi::fmadds(475.f, read<f32>(0x104A44FC, trig), (f32)doorPos->z);
  f32 y = (f32)doorPos->y + 400.f;
  doorPos->set(x, y, z);
  id = gabi::call<u32>(0x025D5A20, 0x131, read<u32>(guestAddress(ship), 4),
                       read<u32>(0x101CC490, doorType * 4), doorPos.get(),
                       (s8)ship->tevStr.mRoomNo, angle.get(), 0, -1, 0);
  ship->doorActor = 0;
  ship->doorID = id;
}
VERIFY(0x0238418C, pirateship_partsCreate);
BOOL pirateship_methodDelete(Ship *ship) {
  WWHD_FUNC(0x02385AEC, BOOL, ship);
  s8 child = read<s8>(0x1046BC84);
  if (child >= 0) {
    gabi::call(0x025F0A18, child);
    write<s8>(0x1046BC84, 0, -1);
  }
  pirateship_DeleteWave(ship);
  gabi::call(0x025E1B34, &ship->cruiseSoundPos);
  gabi::call(0x025E1B34, &ship->sailSoundPos);
  s32 result = gabi::call<s32>(0x024F1F64, ship);
  gabi::call(0x02520488, ptr(0x1002EAB0));
  return result != 0;
}
VERIFY(0x02385AEC, pirateship_methodDelete);
BOOL pirateship_methodDraw(Ship *ship) {
  WWHD_FUNC(0x02385F6C, BOOL, ship);
  s32 result = gabi::call<s32>(read<u32>(ship->__vtbl, 0x2C), ship);
  return result != 0;
}
VERIFY(0x02385F6C, pirateship_methodDraw);
namespace {
struct PirateshipSafeString {
  be<u32> name, vt;
};
void safeCheck(PirateshipSafeString *s) {
  gabi::call(read<u32>(s->vt, 0x14), s);
}
bool stringEqual(u32 a, u32 b) {
  if (a == b)
    return true;
  for (u32 i = 0; i < 0x40001; i++) {
    u8 x = read<u8>(a, i), y = read<u8>(b, i);
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}
bool stageEqual(u32 literal) {
  gabi::Local<PirateshipSafeString> a, b;
  a->name = literal;
  a->vt = 0x1002E818;
  u32 play = gabi::call<u32>(0x025200D4);
  b->name = play + 0x5134;
  b->vt = 0x1002E818;
  safeCheck(a.get());
  safeCheck(a.get());
  u32 name = a->name;
  safeCheck(b.get());
  return stringEqual(name, b->name);
}
} // namespace
void pirateship_sound(Ship *ship) {
  WWHD_FUNC(0x02384A78, void, ship);
  if (ship->visible != 1)
    return;
  gabi::call(0x028E90D4, ptr(baseMatrix(ship->shipModel)), ptr(0x1048D0CC));
  if (stageEqual(0x1002E9F4)) {
    gabi::Local<cXyz> p;
    p->set(0, 800, 2000);
    gabi::call(0x028E8F64, ptr(0x1048D0CC), p.get(), &ship->cruiseSoundPos);
    gabi::call(0x025E19CC, 0x7034, &ship->cruiseSoundPos);
  }
  if (ship->sailSound) {
    gabi::Local<cXyz> p;
    p->set(0, 1500, 300);
    gabi::call(0x028E8F64, ptr(0x1048D0CC), p.get(), &ship->sailSoundPos);
    gabi::call(0x025E19CC, 0x50BE, &ship->sailSoundPos);
  }
}
VERIFY(0x02384A78, pirateship_sound);
void pirateship_sinit() {
  WWHD_FUNC(0x02385FA0, void);
  for (u32 i = 0; i < 16; i += 4)
    write<u32>(0x1046BC74, i, 0);
  gabi::call(0x028F026C, ptr(0x101CC498));
  write<f32>(0x1046BC6C, 0, 3.1415927410125732f);
  write<f32>(0x1046BC68, 0, -3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046BC70));
  gabi::call(0x028F026C, ptr(0x101CC4A4));
  gabi::call(0x028EAB2C, ptr(0x1046BC71));
  gabi::call(0x028F026C, ptr(0x101CC4B0));
  write<f32>(0x1046BC84, 4, 2);
  write<f32>(0x1046BC84, 8, 100);
  write<f32>(0x1046BC84, 0x20, 4);
  write<f32>(0x1046BC84, 0x24, -0.02f);
  write<f32>(0x1046BC84, 0x28, 5);
  write<f32>(0x1046BC84, 0x2C, 300);
  write<u32>(0x1046BC84, 0x38, 0x1002E8C0);
  u32 yz = read<u32>(0x101FFBA8, 4), x = read<u32>(0x101FFBA8);
  write<u32>(0x1046BC84, 0x14, x);
  write<u32>(0x1046BC84, 0x18, yz);
  write<s16>(0x1046BC84, 2, 0);
  write<s16>(0x1046BC84, 0xC, 250);
  write<s16>(0x1046BC84, 0xE, 500);
  u32 z = read<u32>(0x101FFBA8, 8);
  write<s8>(0x1046BC84, 0, -1);
  write<u8>(0x1046BC84, 1, 0);
  write<u8>(0x1046BC84, 0x10, 0);
  write<f32>(0x1046BC84, 0x30, 3);
  write<f32>(0x1046BC84, 0x34, 17);
  write<u32>(0x1046BC84, 0x1C, z);
}
VERIFY(0x02385FA0, pirateship_sinit);
BOOL pirateship_Draw(Ship *ship) {
  WWHD_FUNC(0x02384FB4, BOOL, ship);
  if (!ship->visible)
    return 0;
  auto env = dKy_getEnvlight();
  settingTevStruct(env, 0, &ship->current.pos, &ship->tevStr);
  env = dKy_getEnvlight();
  setLightTevColorType(env, ptr<J3DModel>(ship->shipModel), &ship->tevStr);
  u32 play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4638, 0, read<u32>(play, 0x5D74));
  gabi::call(0x025E2DE0, ptr(ship->shipModel), 0);
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4638, 0, read<u32>(play, 0x5D7C));
  gabi::Local<PirateshipSafeString> name, stage;
  name->name = 0x1002EA80;
  name->vt = 0x1002E818;
  stage->name = 0x1047E6B8;
  stage->vt = 0x1002E818;
  pirateship_stringNoop(name.get());
  safeCheck(name.get());
  u32 str = name->name;
  safeCheck(stage.get());
  if (!stringEqual(str, stage->name)) {
    u32 modelData = read<u32>(ship->seaModel, 0xAC),
        material = read<u32>(modelData, 0x10);
    struct RGBA8 {
      be<u8> r, g, b, a;
    };
    struct RGBA16 {
      be<s16> r, g, b, a;
    };
    struct RGBAf {
      be<f32> r, g, b, a;
    };
    gabi::Local<RGBA8> primary, secondary;
    gabi::Local<RGBA16> color;
    gabi::Local<RGBAf> normalized, converted;
    gabi::call(0x025602F0, primary.get(), secondary.get());
    color->g = primary->g;
    color->r = primary->r;
    color->b = primary->b;
    color->a = 255;
    u32 block = read<u32>(material, 0x18), vt = read<u32>(block, 4);
    gabi::call(read<u32>(vt, 0x24), ptr(block), 1, color.get());
    normalized->r = (f32)(s16)color->r / 255.f;
    normalized->g = (f32)(s16)color->g / 255.f;
    normalized->b = (f32)(s16)color->b / 255.f;
    normalized->a = (f32)(s16)color->a / 255.f;
    gabi::call(0x0274D458, converted.get(), normalized.get(), 1.f);
    write<u32>(material, 0xA0, read<u32>(material, 0xA0) | 0x20);
    u32 destination = gabi::call<u32>(0x027F9F0C, ptr(material, 0xA0), 5);
    f32 b = converted->b, r = converted->r, g = converted->g;
    write<f32>(destination, 4, g);
    write<f32>(destination, 8, b);
    write<f32>(destination, 0, r);
    write<f32>(destination, 12, (f32)(s16)color->a / 255.f);
    gabi::call(0x025E7FC4, ptr(guestAddress(ship), 0x400), ptr(modelData),
               read<f32>(guestAddress(ship), 0x404));
    gabi::call(0x025E2DE0, ptr(ship->seaModel), 0);
  }
  return 1;
}
VERIFY(0x02384FB4, pirateship_Draw);
namespace {
struct PirateshipGroundCheck {
  u8 bytes[0x54];
};
void groundInit(PirateshipGroundCheck *chk, Ship *ship) {
  u32 p = guestAddress(chk);
  gabi::call(0x02008E0C, chk);
  write<u32>(p, 0x10, 0x1002E850);
  write<u32>(p, 0x20, 0x1002E860);
  for (u32 off = 0x44; off < 0x4B; off++)
    write<u8>(p, off, 0);
  write<u32>(p, 0x40, 0x1002E880);
  write<u32>(p, 0, p + 0x40);
  write<u32>(p, 4, p + 0x4C);
  write<u32>(p, 0x4C, 0x1002E870);
  write<u32>(p, 0x50, 1);
  ptr<cXyz>(p, 0x24)->set(ship->current.pos.x, (f32)ship->current.pos.y + 50.f,
                          ship->current.pos.z);
  write<u32>(p, 0x30, read<u32>(p, 0x30) & ~2u);
}
void groundDestroy(PirateshipGroundCheck *chk) {
  u32 p = guestAddress(chk);
  write<u32>(p, 0x4C, 0x1002E840);
  write<u32>(p, 0x40, 0x1002E880);
  write<u32>(p, 0x20, 0x1002E860);
  gabi::call(0x02008DAC, chk, 0);
}
bool eventBit(u32 flag) {
  return gabi::call<BOOL>(0x025B8B94, ptr(read<u32>(0x101F84DC), 0x644),
                          flag) != 0;
}
void searchPart(Ship *ship, u32 idOffset, u32 actorOffset) {
  u32 id = read<u32>(guestAddress(ship), idOffset);
  gabi::Local<be<u32>> key;
  *key = id;
  write<u32>(guestAddress(ship), actorOffset, 0);
  u32 actor = 0;
  if (id != 0xFFFFFFFF)
    actor = gabi::call<u32>(0x025D5218, ptr(0x025E1234), key.get());
  write<u32>(guestAddress(ship), actorOffset, actor);
}
BOOL executeShip(Ship *ship) {
  gabi::Local<PirateshipGroundCheck> chk;
  // EABI callees save LR at SP+4; keep that slot below the live check.
  gabi::Local<be<u32>[4]> linkage;
  groundInit(chk.get(), ship);
  u32 play = gabi::call<u32>(0x025200D4);
  f32 ground = gabi::call<f32>(0x02008974, ptr(play, 0x12A0), chk.get());
  if (!(ground <= -1000000000.f)) {
    play = gabi::call<u32>(0x025200D4);
    s32 room = gabi::call<s32>(0x024EF130, ptr(play, 0x12A0),
                               ptr(guestAddress(chk.get()), 0x14));
    if ((u32)room >= 64)
      gabi::call(0x0273AA24, ptr(0x1002E8EC), 0x32C, ptr(0x1002E904));
    ship->current.roomNo = (s8)room;
  }
  bool demo = pirateship_demo(ship) != 0;
  if (!demo) {
    if (!read<u8>(0x1046BC85) && ship->path) {
      gabi::call(0x02587D24, &ship->current.pos, &ship->pathPoint,
                 ptr(ship->path), 3.f, ptr(0x02384078), ship);
      s16 bob = ship->bobAngle;
      s16 target = read<s16>(0x1046BC84,
                             ((u32)((s32)bob + 0x4000) >= 0x8001) ? 0xE : 0xC);
      gabi::call(0x0200F428, &ship->bobStep, target, 0x10, 0x300);
      u16 angle = (s16)ship->bobAngle + (s16)ship->bobStep;
      ship->bobAngle = (s16)angle;
      f32 amplitude = read<f32>(0x1046BC8C) + 10.f;
      ship->current.pos.y =
          gabi::fmadds(amplitude, read<f32>(0x104A44F8, (angle >> 3) * 8),
                       (f32)ship->home.pos.y);
    }
    ship->tilt = read<s16>(0x1046BC86);
  }
  if (!ship->visible) {
    if (!demo || !eventBit(0x310)) {
      groundDestroy(chk.get());
      return 0;
    }
    if (!eventBit(1))
      pirateship_CreateWave(ship);
    ship->visible = 1;
    play = gabi::call<u32>(0x025200D4);
    gabi::call(0x024EEA6C, ptr(play, 0x12A0),
               ptr(read<u32>(guestAddress(ship), 0x3AC)), ship);
  } else if (!ship->piratesCreated && eventBit(1)) {
    ship->piratesCreated = 1;
    pirateship_pirateCreate(ship, ptr<be<s32>>(0x101CC434));
    pirateship_DeleteWave(ship);
  }
  searchPart(ship, 0x478, 0x474);
  searchPart(ship, 0x480, 0x47C);
  searchPart(ship, 0x490, 0x48C);
  pirateship_setMtx(ship);
  pirateship_sound(ship);
  pirateship_SetWave(ship);
  BOOL result = gabi::call<s32>(0x024F1E9C, ship) != 0;
  groundDestroy(chk.get());
  return result;
}
} // namespace
BOOL pirateship_execute(Ship *ship) {
  WWHD_FUNC(0x02385B70, BOOL, ship);
  return executeShip(ship);
}
VERIFY(0x02385B70, pirateship_execute);
BOOL pirateship_create(Ship *ship) {
  WWHD_FUNC(0x023852E0, BOOL, ship);
  u32 p = guestAddress(ship);
  if (!(ship->actor_condition & 8)) {
    if (ship) {
      gabi::call(0x024F1D40, ship);
      ship->__vtbl = 0x1002EABC;
      gabi::call(0x025E7C6C, ptr(p, 0x400));
      gabi::call(0x0200BD2C, ptr(p, 0x4C8));
      gabi::call(0x02515DA0, ptr(p, 0x4E4));
      write<u32>(p, 0x4E0, 0x1004AE88);
      write<u32>(p, 0x4E4, 0x1004AEC0);
      gabi::call(0x02515FB8, ptr(p, 0x504));
      write<u32>(p, 0x618, 0x100015A8);
      write<u32>(p, 0x614, 0x1002E830);
      gabi::call(0x02018590, ptr(p, 0x61C));
      write<u32>(p, 0x630, 0x1004B150);
      write<u32>(p, 0x540, 0x1004B108);
      write<u32>(p, 0x618, 0x1004B160);
      write<u32>(p, 0x63C, 0x10052268);
      gabi::call(0x028EFFD0, ptr(p, 0x64C), 3, 12, ptr(0x02386114));
      gabi::call(0x024F0474, ptr(p, 0x694));
      write<u32>(p, 0x6B4, 0x1002E8A0);
      write<u32>(p, 0x6A4, 0x1002E890);
      write<u32>(p, 0x6A8, 0x1002E8B0);
      write<u8>(p, 0x6AC, 1);
    }
    ship->actor_condition = ship->actor_condition | 8;
  }
  s32 phase = gabi::call<s32>(0x02520460, &ship->phase, ptr(0x1002EAB0));
  if (phase != 4)
    return phase;
  s32 collision = stageEqual(0x1002E814) ? 6 : 5;
  phase = gabi::call<s32>(0x024F1D9C, ship, ptr(0x1002EAB0), collision,
                          ptr(0x024EE708), 0x7500);
  write<u32>(read<u32>(p, 0x3AC), 0xB0, 0x02384068);
  ship->sailSound = (ship->mParameters & 15) != 0;
  pirateship_partsCreate(ship);
  u8 variant = ship->mParameters >> 24;
  ship->splashEmitter = 0;
  switch (variant) {
  case 0:
    if (!eventBit(0x310)) {
      ship->visible = 0;
      ship->piratesCreated = 0;
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x020087EC, ptr(play, 0x12A0), ptr(read<u32>(p, 0x3AC)));
    } else {
      ship->visible = 1;
      if (eventBit(1)) {
        pirateship_pirateCreate(ship, ptr<be<s32>>(0x101CC47C));
        ship->piratesCreated = 1;
      } else {
        ship->piratesCreated = 0;
        pirateship_CreateWave(ship);
      }
      if (!ship->visible) {
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::call(0x020087EC, ptr(play, 0x12A0), ptr(read<u32>(p, 0x3AC)));
      }
    }
    break;
  case 1:
    pirateship_pirateCreate(ship, ptr<be<s32>>(0x101CC43C));
    ship->piratesCreated = 1;
    ship->visible = 1;
    pirateship_CreateWave(ship);
    if (!ship->visible) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x020087EC, ptr(play, 0x12A0), ptr(read<u32>(p, 0x3AC)));
    }
    break;
  case 2: {
    u32 play = gabi::call<u32>(0x025200D4);
    if (read<s16>(play, 0x513C) == 0x12)
      pirateship_pirateCreate(ship, ptr<be<s32>>(0x101CC454));
    ship->piratesCreated = 1;
    ship->visible = 1;
    break;
  }
  case 4:
    pirateship_pirateCreate(ship, ptr<be<s32>>(0x101CC46C));
    ship->piratesCreated = 1;
    ship->visible = 1;
    break;
  default:
    ship->piratesCreated = 1;
    ship->visible = 1;
    break;
  }
  u8 pathIndex = (ship->mParameters >> 16) & 255;
  ship->path = 0;
  ship->pathPoint = 0;
  if (pathIndex != 255)
    ship->path =
        gabi::call<u32>(0x025AAF88, pathIndex, (s8)ship->current.roomNo);
  ship->bobAngle = 0;
  ship->bobStep = 0x180;
  pirateship_setMtx(ship);
  executeShip(ship);
  return phase;
}
VERIFY(0x023852E0, pirateship_create);

/* ---- leftover functions of the translation unit ---- */

/* 02386148 daObjPirateship::Act_c::~Act_c (deleting; vtable slot 1002EAC8) */
static void pirateship_Act_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02386148, void, p, flags);
    if (p != nullptr) {
        u32 t = gabi::ea(p);
        gabi::store<u32>(t + 0x6B4, 0x1002E8A0);
        gabi::store<u32>(t + 0x6A8, 0x1002E8B0);
        gabi::call(0x024EFD9C, t + 0x694, 0); /* line check */
        gabi::call(0x02515A70, t + 0x504, 2); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02515860, t + 0x4C8, 2); /* dCcD_Stts */
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x02386148, pirateship_Act_c_dt);

/* 023861D8 daObjPirateship::Method::IsDelete (profile method table 101CC4F8): TRUE */
static BOOL pirateship_Method_IsDelete(void* p) {
    WWHD_FUNC(0x023861D8, BOOL, p);
    return TRUE;
}
VERIFY(0x023861D8, pirateship_Method_IsDelete);
