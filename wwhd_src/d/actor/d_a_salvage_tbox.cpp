// Local WWHD salvage chest reconstruction.
#include "d/actor/d_a_salvage_tbox.h"
namespace {
template <class T> T load(u32 p, u32 o = 0) { return gabi::load<T>(p + o); }
template <class T> void store(u32 p, u32 o, T v) { gabi::store<T>(p + o, v); }
u8 *ptr(u32 p) { return gabi::at<u8>(p); }
struct SafeName {
  be<u32> text, vt;
};
void copyFloat3(u32 dst, u32 src) {
  f32 x = load<f32>(src), y = load<f32>(src, 4), z = load<f32>(src, 8);
  store(dst, 0, x);
  store(dst, 4, y);
  store(dst, 8, z);
}
void invalidate(u32 emitter) {
  u32 flags = load<u32>(emitter, 0x254);
  store<s32>(emitter, 0x5C, -1);
  store(emitter, 0x254, flags | 1);
}
void clearEmitters(daSTBox_c *a) {
  for (int i = 0; i < 3; i++) {
    u32 e = gabi::ea((void *)a->emitters[i]);
    if (e) {
      store(e, 0x254, load<u32>(e, 0x254) & ~0x40u);
      e = gabi::ea((void *)a->emitters[i]);
      invalidate(e);
      a->emitters[i] = 0;
    }
  }
}
void removeShadow(daSTBox_c *a) {
  u32 e = gabi::ea((void *)a->shadow.emitter);
  if (e) {
    store<u32>(e, 0x1E4, 0);
    invalidate(gabi::ea((void *)a->shadow.emitter));
  }
  a->shadow.emitter = 0;
}
} // namespace
f32 STBox_getWaterY(cXyz *position) {
  WWHD_FUNC(0x02467D34, f32, position);
  f32 y = position->y;
  f32 x = position->x, z = position->z;
  position->y = gabi::fadds_ppc(y, 500.0f);
  if (gabi::call<s32>(0x0246B6A4, x, z)) {
    x = position->x;
    z = position->z;
    return gabi::call<f32>(0x0246BA0C, x, z);
  }
  return gabi::call<f32>(0x024F1478, position);
}
VERIFY(0x02467D34, STBox_getWaterY);
s32 STBox_CreateHeap(daSTBox_c *a) {
  WWHD_FUNC(0x02467D9C, s32, a);
  u32 type = a->boxType;
  u32 res = load<u32>(0x101F4F28);
  s32 index = load<s16>(0x10039C40 + type * 2);
  gabi::Local<SafeName> name;
  name->text = 0x10039C30;
  name->vt = 0x10039AD4;
  auto *data =
      gabi::call<J3DModelData *>(0x026066C4, ptr(res), name.get(), index);
  if (!data)
    gabi::call(0x0273AA24, STR(0x10039B28), 0x25F, STR(0x10039B40));
  auto *model = gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x11000022);
  a->model = model;
  return model != nullptr;
}
VERIFY(0x02467D9C, STBox_CreateHeap);
s32 STBox_CheckHeap(daSTBox_c *a) {
  WWHD_FUNC(0x02467E48, s32, a);
  return STBox_CreateHeap(a);
}
VERIFY(0x02467E48, STBox_CheckHeap);
void STBox_set_mtx(daSTBox_c *a) {
  WWHD_FUNC(0x02467E4C, void, a);
  f32 x = a->scale.x, y = a->scale.y, z = a->scale.z;
  u32 m = gabi::ea((J3DModel *)a->model);
  store(m, 0xBC, x);
  store(m, 0xC0, y);
  store(m, 0xC4, z);
  x = a->current.pos.x;
  y = a->current.pos.y;
  z = a->current.pos.z;
  gabi::call(0x028E93CC, ptr(0x1048D0CC), x, y, z);
  s32 angle = a->current.angle.y;
  gabi::call(0x025F1C28, ptr(0x1048D0CC), angle);
  f32 matrix[12];
  for (int i = 0; i < 12; i++)
    matrix[i] = load<f32>(0x1048D0CC + i * 4);
  m = gabi::ea((J3DModel *)a->model);
  for (int i = 0; i < 12; i++)
    store(m, 0xC8 + i * 4, matrix[i]);
}
VERIFY(0x02467E4C, STBox_set_mtx);
s32 STBox_Delete(daSTBox_c *a) {
  WWHD_FUNC(0x024683D8, s32, a);
  clearEmitters(a);
  gabi::call(0x025A9270, ptr(gabi::ea(a) + 0x3C8));
  removeShadow(a);
  gabi::call(0x025204C8, &a->phase, STR(0x10039C30));
  u32 save = load<u32>(0x101F84DC);
  u32 reg = gabi::call<u32>(0x025B8BB0, ptr(save + 0x644), 0xADFF);
  if (a->boxType == 2) {
    save = load<u32>(0x101F84DC);
    gabi::call(0x025B8AF4, ptr(save + 0x644), 0xADFF, (reg + 1) & 255);
  }
  return 1;
}
VERIFY(0x024683D8, STBox_Delete);
s32 STBox_DeleteWrapper(daSTBox_c *a) {
  WWHD_FUNC(0x024684F0, s32, a);
  return STBox_Delete(a);
}
VERIFY(0x024684F0, STBox_DeleteWrapper);
s32 STBox_Draw(daSTBox_c *a) {
  WWHD_FUNC(0x024684F4, s32, a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 0, &a->current.pos, &a->tevStr);
  env = gabi::call<u32>(0x02555D0C);
  auto *model = (J3DModel *)a->model;
  gabi::call(0x02562F5C, ptr(env), model, &a->tevStr);
  model = a->model;
  gabi::call(0x025E2DE0, model, 0);
  return 1;
}
VERIFY(0x024684F4, STBox_Draw);
void STBox_getMaxWaterY(daSTBox_shadowEcallBack_c *a, cXyz *p) {
  WWHD_FUNC(0x02468840, void, a, p);
  f32 x = p->x, z = p->z;
  if (gabi::call<s32>(0x0246B6A4, x, z)) {
    x = p->x;
    z = p->z;
    f32 wave = gabi::call<f32>(0x0246BA0C, x, z);
    f32 y = gabi::fadds_ppc(wave, 2.0f);
    p->y = y;
    f32 flat = a->flatWaterY;
    if (flat > y)
      p->y = gabi::fadds_ppc(flat, 2.0f);
  } else {
    f32 flat = a->flatWaterY;
    if (flat != -1000000000.0f)
      p->y = gabi::fadds_ppc(flat, 2.0f);
    else
      p->y = a->waterY;
  }
}
VERIFY(0x02468840, STBox_getMaxWaterY);
void STBox_matrixCopy(Mtx34 *dst, Mtx34 *src) {
  WWHD_FUNC(0x02468B80, void, dst, src);
  f32 values[12];
  for (int i = 0; i < 12; i++)
    values[i] = load<f32>(gabi::ea(src) + i * 4);
  for (int i = 0; i < 12; i++)
    store(gabi::ea(dst), i * 4, values[i]);
}
VERIFY(0x02468B80, STBox_matrixCopy);
void STBox_initWait02(daSTBox_c *a, s32 staff) {
  WWHD_FUNC(0x02469298, void, a, staff);
  a->timer = 20;
}
VERIFY(0x02469298, STBox_initWait02);
void STBox_initWaitGetItem(daSTBox_c *a, s32 staff) {
  WWHD_FUNC(0x024692A4, void, a, staff);
  gabi::call(0x025DA884, ptr(gabi::ea(a) + 0xDC));
  clearEmitters(a);
  gabi::call(0x025A9270, ptr(gabi::ea(a) + 0x3C8));
}
VERIFY(0x024692A4, STBox_initWaitGetItem);
void STBox_initDrop(daSTBox_c *a, s32 staff) {
  WWHD_FUNC(0x0246932C, void, a, staff);
  a->gravity = -4.0f;
}
VERIFY(0x0246932C, STBox_initDrop);
s32 STBox_actWait02(daSTBox_c *a, s32 staff) {
  WWHD_FUNC(0x02469554, s32, a, staff);
  u32 play = gabi::call<u32>(0x025200D4);
  u32 ship = load<u32>(play, 0x5B3C);
  u32 top = load<u32>(ship, 0x71C);
  if (top) {
    u32 type = a->boxType;
    f32 x = load<f32>(top), y = load<f32>(top, 4);
    f32 offset = load<f32>(0x10039B1C + type * 4);
    f32 z = load<f32>(top, 8);
    a->current.pos.x = x;
    a->current.pos.z = z;
    a->current.pos.y = gabi::fsubs_ppc(y, offset);
  }
  return 0;
}
VERIFY(0x02469554, STBox_actWait02);
s32 STBox_actDrop(daSTBox_c *a, s32 staff) {
  WWHD_FUNC(0x024695C4, s32, a, staff);
  gabi::call(0x025D6870, a, 0);
  gabi::Local<cXyz> p;
  copyFloat3(gabi::ea(p.get()), gabi::ea(a) + 0x314);
  f32 y = a->current.pos.y;
  f32 water = STBox_getWaterY(p.get());
  if (y < gabi::fsubs_ppc(water, 50.0f))
    return 1;
  gabi::Local<cXyz> q;
  copyFloat3(gabi::ea(q.get()), gabi::ea(a) + 0x314);
  y = a->current.pos.y;
  water = STBox_getWaterY(q.get());
  if (y < water) {
    if (a->splashStarted == 0) {
      s32 room = a->current.roomNo;
      s32 reverb = gabi::call<s32>(0x02520540, room);
      gabi::call(0x025E1A40, 0x6919, &a->eyePos, 0, reverb);
      gabi::call(0x025DAE64, &a->current.pos, 0, 0.8f, 1.0f);
      a->splashStarted = 1;
    }
    gabi::call(0x025A9270, ptr(gabi::ea(a) + 0x3C8));
  }
  return 0;
}
VERIFY(0x024695C4, STBox_actDrop);
void STBox_emptyDestructor(void *p, s32 flag) {
  WWHD_FUNC(0x024697D0, void, p, flag);
  if (p && (flag & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x024697D0, STBox_emptyDestructor);
cXyz *STBox_vectorConstructor(cXyz *p) {
  WWHD_FUNC(0x024697E4, cXyz *, p);
  if (!p)
    p = gabi::call<cXyz *>(0x0273AD10, 12);
  return p;
}
VERIFY(0x024697E4, STBox_vectorConstructor);
s32 STBox_IsDelete(void *p) {
  WWHD_FUNC(0x02469810, s32, p);
  return 1;
}
VERIFY(0x02469810, STBox_IsDelete);
namespace {
void *particleSet(daSTBox_c *a, u32 group, u32 id, u32 pos, u32 angle,
                  u32 scale, u32 alpha, u32 callback) {
  u32 play = gabi::call<u32>(0x025200D4);
  u32 control = load<u32>(play, 0x5AB0);
  return gabi::call<void *>(0x025A847C, ptr(control), group, id, ptr(pos),
                            ptr(angle), ptr(scale), alpha, ptr(callback), -1, 0,
                            0, 0);
}
void setupEmitter(daSTBox_c *a, int index, f32 scale, f32 y) {
  u32 e = gabi::ea((void *)a->emitters[index]);
  if (!e)
    return;
  store(e, 0x254, load<u32>(e, 0x254) | 0x40);
  e = gabi::ea((void *)a->emitters[index]);
  store(e, 0x238, 1.5f);
  store(e, 0x23C, 1.5f);
  store(e, 0x240, 1.0f);
  e = gabi::ea((void *)a->emitters[index]);
  store(e, 8, scale);
  store(e, 12, 1.0f);
  store(e, 16, scale);
  e = gabi::ea((void *)a->emitters[index]);
  store(e, 20, 0.0f);
  store(e, 28, 0.0f);
  store(e, 24, y);
}
s32 eventMember(u32 table, u32 actor, s32 staff) {
  s32 delta = load<s16>(table), slot = load<s16>(table, 2);
  u32 self = actor + delta;
  u32 target;
  if (slot < 0)
    target = load<u32>(table, 4);
  else {
    u32 v = load<u32>(self + (s32)load<s16>(table, 6));
    target = load<u32>(v + slot * 8 + 4);
  }
  return gabi::call<s32>(target, ptr(self), staff);
}
} // namespace
void STBox_CreateInit(daSTBox_c *a) {
  WWHD_FUNC(0x02467F24, void, a);
  gabi::Local<cXyz> crane;
  u32 play = gabi::call<u32>(0x025200D4);
  u32 ship = load<u32>(play, 0x5B3C);
  u32 top = ship ? load<u32>(ship, 0x71C) : 0;
  if (top) {
    crane->x = load<f32>(top);
    crane->z = load<f32>(top, 8);
    gabi::Local<cXyz> temp;
    temp->x = load<f32>(top);
    temp->y = load<f32>(top, 4);
    temp->z = load<f32>(top, 8);
    f32 water = STBox_getWaterY(temp.get());
    a->cranePos.z = crane->z;
    a->cranePos.y = water;
    a->cranePos.x = crane->x;
  } else {
    a->cranePos.x = crane->x;
    a->cranePos.y = crane->y;
    a->cranePos.z = crane->z;
  }
  u32 m = gabi::ea((J3DModel *)a->model);
  a->cullMtx = m ? m + 0xC8 : 0;
  gabi::call(0x025D674C, a, -150.0f, -0.0f, -150.0f, 150.0f, 150.0f, 150.0f);
  STBox_set_mtx(a);
  u32 item = load<u8>(gabi::ea(a), 0xB3);
  a->itemNo = (item - 1u < 4) ? 5 : item;
  a->splashStarted = 0;
  a->bgmStarted = 0;
  for (int i = 0; i < 2; i++)
    a->emitters[i] = particleSet(a, 0, 0x38, gabi::ea(a) + 0x314,
                                 gabi::ea(a) + 0x320, 0, 255, 0);
  u32 type = a->boxType;
  if (type == 1 || type == 2) {
    a->emitters[2] = particleSet(a, 0, 0x38, gabi::ea(a) + 0x314,
                                 gabi::ea(a) + 0x320, 0, 255, 0);
    for (int i = 0; i < 3; i++)
      setupEmitter(a, i, 3.0f, 20.0f);
    if (!(void *)a->shadow.emitter) {
      particleSet(a, 5, 0x53, gabi::ea(a) + 0x440, gabi::ea(a) + 0x320, 0, 0,
                  gabi::ea(a) + 0x3DC);
      a->shadow.position.copy(a->cranePos);
      a->shadow.textureScale = 4.0f;
      a->itemPID = 0xFFFFFFFF;
      a->shadow.scroll = -0.1f;
      return;
    }
  } else if (type == 0) {
    for (int i = 0; i < 2; i++)
      setupEmitter(a, i, 3.5f, -20.0f);
  }
  a->itemPID = 0xFFFFFFFF;
}
VERIFY(0x02467F24, STBox_CreateInit);
s32 STBox_Create(daSTBox_c *a) {
  WWHD_FUNC(0x024682CC, s32, a);
  u32 condition = a->actor_condition;
  if (!(condition & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, a);
      a->__vtbl = 0x10039B0C;
      gabi::call(0x025A9084, ptr(gabi::ea(a) + 0x3C8));
      a->shadow.vtable = 0x10039C4C;
      gabi::call(0x028EFFD0, ptr(gabi::ea(a) + 0x3F0), 3, 12, ptr(0x024697E4));
      condition = a->actor_condition;
    }
    a->actor_condition = condition | 8;
  }
  a->boxType = ((u32)a->mParameters >> 8) & 15;
  s32 phase = gabi::call<s32>(0x02520460, &a->phase, STR(0x10039C30));
  if (phase == 4) {
    u32 type = a->boxType;
    s32 size = load<s16>(0x10039C38 + type * 2);
    if (!gabi::call<s32>(0x025D63E8, a, ptr(0x02467E48), size))
      return 5;
    STBox_CreateInit(a);
  }
  return phase;
}
VERIFY(0x024682CC, STBox_Create);
s32 STBox_CreateWrapper(daSTBox_c *a) {
  WWHD_FUNC(0x024683D4, s32, a);
  return STBox_Create(a);
}
VERIFY(0x024683D4, STBox_CreateWrapper);
s32 STBox_Execute(daSTBox_c *a) {
  WWHD_FUNC(0x02468550, s32, a);
  u32 play = gabi::call<u32>(0x025200D4);
  s32 staff =
      gabi::call<s32>(0x02542D88, ptr(play + 0x52C4), STR(0x10039B88), 0, 0);
  play = gabi::call<u32>(0x025200D4);
  u32 ship = load<u32>(play, 0x5B3C);
  gabi::Local<cXyz> ripple;
  f32 water = 0;
  if (ship) {
    copyFloat3(gabi::ea(ripple.get()), ship + 0x1308);
    gabi::Local<cXyz> temp;
    copyFloat3(gabi::ea(temp.get()), gabi::ea(ripple.get()));
    water = STBox_getWaterY(temp.get());
  }
  play = gabi::call<u32>(0x025200D4);
  if (load<u8>(play, 0x5292) && load<u16>(gabi::ea(a), 0xF8) != 1 &&
      staff != -1) {
    play = gabi::call<u32>(0x025200D4);
    s32 action = gabi::call<s32>(0x02542EDC, ptr(play + 0x52C4), staff,
                                 ptr(0x101CFF78), 5, 0, 0);
    play = gabi::call<u32>(0x025200D4);
    if (action == -1)
      gabi::call(0x02543280, ptr(play + 0x52C4), staff);
    else {
      u32 offset = (u32)action * 8;
      if (gabi::call<s32>(0x025447C8, ptr(play + 0x52C4), staff))
        eventMember(0x101CFF08 + offset, gabi::ea(a), staff);
      if (eventMember(0x101CFF30 + offset, gabi::ea(a), staff)) {
        play = gabi::call<u32>(0x025200D4);
        gabi::call(0x02543280, ptr(play + 0x52C4), staff);
      }
    }
  }
  for (int i = 0; i < 3; i++) {
    u32 e = gabi::ea((void *)a->emitters[i]);
    if (e) {
      u32 kind = load<u8>(e, 0x262);
      f32 x = a->current.pos.x, z = a->current.pos.z, y = a->current.pos.y;
      store(e, 0x234, z);
      store(e, 0x230, y);
      store(e, 0x22C, x);
      if (kind >= 7)
        store(e, 0x230, -load<f32>(e, 0x230));
    }
  }
  f32 y = a->current.pos.y;
  if (y < water) {
    a->shadow.position.copy(*ripple.get());
    a->shadow.depth = gabi::fsubs_ppc(water, (f32)a->current.pos.y);
    f32 raised = gabi::fadds_ppc(water, 2.0f);
    a->shadow.flatWaterY = raised;
    a->shadow.waterY = raised;
  } else
    removeShadow(a);
  STBox_set_mtx(a);
  return 1;
}
VERIFY(0x02468550, STBox_Execute);
s32 STBox_ExecuteWrapper(daSTBox_c *a) {
  WWHD_FUNC(0x0246883C, s32, a);
  return STBox_Execute(a);
}
VERIFY(0x0246883C, STBox_ExecuteWrapper);
s32 STBox_actWait(daSTBox_c *a, s32 staff) {
  WWHD_FUNC(0x0246933C, s32, a, staff);
  u32 play = gabi::call<u32>(0x025200D4);
  u32 ship = load<u32>(play, 0x5B3C);
  if (!ship)
    gabi::call(0x0273AA24, STR(0x10039C04), 0x3D5, STR(0x10039C00));
  u32 top = load<u32>(ship, 0x71C);
  if (!top)
    gabi::call(0x0273AA24, STR(0x10039C04), 0x3DC, STR(0x10039C00));
  gabi::Local<cXyz> temp;
  temp->x = load<f32>(top);
  temp->y = gabi::fadds_ppc(load<f32>(top, 4), 5000.0f);
  temp->z = load<f32>(top, 8);
  f32 water = STBox_getWaterY(temp.get());
  u32 type = a->boxType;
  a->current.angle.y = load<s16>(ship, 0x32A);
  f32 y = gabi::fsubs_ppc(load<f32>(top, 4), load<f32>(0x10039B1C + type * 4)),
      x = load<f32>(top), z = load<f32>(top, 8);
  a->current.pos.x = x;
  a->current.pos.y = y;
  store(gabi::ea(a), 0x394, y);
  store(gabi::ea(a), 0x390, x);
  store(gabi::ea(a), 0x398, z);
  a->current.pos.z = z;
  if ((type == 1 || type == 2) && (f32)a->current.pos.y > water &&
      a->bgmStarted == 0) {
    gabi::call(0x025E1918, 0x8000005Du);
    a->bgmStarted = 1;
  }
  if (a->rippleStarted == 0) {
    a->particlePos.x = a->current.pos.x;
    a->particlePos.z = a->current.pos.z;
    a->particlePos.y = gabi::fadds_ppc((f32)a->current.pos.y, 2500.0f);
    f32 height = gabi::call<f32>(0x024F17D4, &a->particlePos);
    f32 y = a->current.pos.y;
    a->particlePos.y = height;
    if (y > gabi::fsubs_ppc(height, 10.0f)) {
      particleSet(a, 5, 0x35C, gabi::ea(a) + 0x434, 0, gabi::ea(a) + 0x330, 255,
                  gabi::ea(a) + 0x3C8);
      a->rippleStarted = 1;
      store(gabi::ea(a), 0x3D8, 12.0f);
    }
  }
  type = a->boxType;
  if ((type == 1 || type == 2) && (u32)a->itemPID == 0xFFFFFFFF) {
    s32 room = load<s8>(0x1047E6C8);
    u32 item = a->itemNo;
    u32 pid =
        gabi::call<u32>(0x025D7E88, &a->current.pos, item, -1, room, 0, 0);
    a->itemPID = pid;
    if (pid != 0xFFFFFFFF) {
      play = gabi::call<u32>(0x025200D4);
      store(play, 0x52A0, pid);
    }
  }
  return 0;
}
VERIFY(0x0246933C, STBox_actWait);
void STBox_shadowExecute(daSTBox_shadowEcallBack_c *a, void *emitter) {
  WWHD_FUNC(0x024688F4, void, a, emitter);
  u32 e = gabi::ea(emitter);
  struct Color {
    be<u8> r, g, b, a;
  };
  gabi::Local<Color> amb, diff;
  gabi::call(0x025602F0, amb.get(), diff.get());
  store<u8>(e, 0x248, diff->r);
  store<u8>(e, 0x249, diff->g);
  store<u8>(e, 0x24A, diff->b);
  store<u8>(e, 0x244, amb->r);
  store<u8>(e, 0x245, amb->g);
  store<u8>(e, 0x246, amb->b);
  if (a->ended) {
    invalidate(e);
    a->emitter = 0;
  }
  if (load<s32>(e, 0x5C) == 0 && a->ended == 0) {
    f32 x = a->position.x, y = a->position.y, z = a->position.z;
    u32 kind = load<u8>(e, 0x262);
    store(e, 0x22C, x);
    store(e, 0x230, y);
    store(e, 0x234, z);
    if (kind >= 7)
      store(e, 0x230, -load<f32>(e, 0x230));
    f32 direction = a->direction;
    u32 angle = gabi::ea((csXyz *)a->angle);
    s32 yaw = load<s16>(angle, 2);
    if (direction < 0)
      yaw = (s16)((u32)yaw + 0x8000);
    gabi::call(0x028245AC, 0, yaw, 0, ptr(e + 0x1F0));
    f32 depth = a->depth;
    if (depth < 0.0f || depth > 2000.0f)
      store<u8>(e, 0x247, 0);
    else {
      f32 alpha = (120.0f * gabi::fsubs_ppc(2000.0f, depth)) / 2000.0f;
      s32 value = gabi::ftoi(alpha);
      if (depth < 160.0f)
        value = gabi::ftoi(0.75f * depth);
      store<u8>(e, 0x247, value);
    }
  } else {
    u32 kind = load<u8>(e, 0x262);
    f32 x = load<f32>(e, 0x22C), y = a->waterY;
    store(e, 0x22C, x);
    u32 alpha = load<u8>(e, 0x247);
    store(e, 0x230, y);
    if (kind >= 7)
      store(e, 0x230, -load<f32>(e, 0x230));
    gabi::Local<be<s16>> fade;
    *fade = alpha;
    gabi::call(0x0200F564, fade.get(), 0, 5);
    store<u8>(e, 0x247, 255);
    *fade = 255;
  }
  u32 link = load<u32>(e, 0x1AC);
  while (link) {
    u32 particle = load<u32>(link);
    f32 x = load<f32>(particle, 0x10);
    u32 next = load<u32>(link, 12);
    gabi::Local<cXyz> pos;
    pos->x = x;
    pos->y = load<f32>(particle, 0x14);
    pos->z = load<f32>(particle, 0x18);
    STBox_getMaxWaterY(a, pos.get());
    store(particle, 0x10, (f32)pos->x);
    store(particle, 0x14, (f32)pos->y);
    store(particle, 0x18, (f32)pos->z);
    link = next;
  }
}
VERIFY(0x024688F4, STBox_shadowExecute);
namespace {
u32 visibleParticles(u32 emitter) {
  if (load<u32>(emitter, 0x1B4) < 6)
    return 0;
  u32 n = 0;
  for (u32 link = load<u32>(emitter, 0x1AC); link; link = load<u32>(link, 12)) {
    u32 particle = load<u32>(link);
    if (load<u8>(particle, 0x10C) == 0)
      n++;
  }
  return n;
}
u32 third(u32 n) {
  f32 f = (f32)n * 0.3333333432674408f;
  return f < 2147483648.0f ? (u32)gabi::ftoi(f)
                           : (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}
struct RenderState {
  u8 bytes[0x11C];
};
WWHD_SIZE(RenderState, 0x11C);
void depthState(bool after) {
  gabi::Local<RenderState> state;
  gabi::call(0x02750250, state.get());
  u32 p = gabi::ea(state.get());
  if (after)
    store(p, 12, load<u32>(p, 12) | 2);
  store<u8>(p, 1, 0);
  store<u32>(p, 4, 0);
  store<u8>(p, 0, 0);
  gabi::call(0x02750534, state.get());
}
} // namespace
void STBox_shadowDraw(daSTBox_shadowEcallBack_c *a, void *emitter) {
  WWHD_FUNC(0x02468C20, void, a, emitter);
  u32 e = gabi::ea(emitter);
  u32 count = visibleParticles(e);
  if (count < 6)
    return;
  if (load<u8>(0x101EA4E4) & 1)
    depthState(false);
  u32 steps = third(count);
  f32 increment = 1.0f / (f32)(steps - 1);
  gabi::Local<Mtx34> matrix;
  gabi::call(0x028E9098, matrix.get());
  f32 scroll = a->scroll, frame = load<f32>(e, 0x194), scale = a->textureScale;
  store(gabi::ea(matrix.get()), 0x14, scale);
  store(gabi::ea(matrix.get()), 0x1C, frame * scroll);
  u32 first = load<u32>(e, 0x1AC), particle = load<u32>(first);
  gabi::Local<Mtx34> copy;
  STBox_matrixCopy(copy.get(), matrix.get());
  u32 material = load<u32>(particle, 0xEC);
  gabi::call(0x027FC80C, ptr(material), copy.get(), 1);
  u32 shadowEmitter = gabi::ea((void *)a->emitter);
  gabi::call(0x028255F8, ptr(particle), ptr(shadowEmitter));
  u32 link = load<u32>(e, 0x1AC);
  f32 textureY = 0;
  for (u32 step = 0; step < steps; step++) {
    if (step == 0) {
      for (int j = 0; j < 3; j++) {
        u32 pt = load<u32>(link);
        copyFloat3(gabi::ea(a) + 0x14 + j * 12, pt + 0x28);
        link = load<u32>(link, 12);
      }
    } else {
      shadowEmitter = gabi::ea((void *)a->emitter);
      u32 buffers = load<u32>(shadowEmitter, 0x310);
      u32 active = load<u32>(buffers, 0x2BA70);
      u32 record = buffers + (active + 2 * step - 2) * 0x254;
      u32 vertex = load<u32>(record);
      u32 end = vertex + 0x60;
      for (u32 p = vertex; p < end; p += 0x20) {
        u32 aligned = p & ~31u;
        for (int k = 0; k < 32; k++)
          store<u8>(aligned, k, 0);
      }
      if (vertex < end) {
        active = load<u32>(buffers, 0x2BA70);
        record = buffers + (active + 2 * step - 2) * 0x254;
      }
      u32 written = 0;
      f32 textureX = 0;
      f32 priorY = gabi::fsubs_ppc(textureY, increment);
      for (int j = 0; j < 3;) {
        u32 pt = load<u32>(link);
        gabi::Local<cXyz> pos;
        copyFloat3(gabi::ea(pos.get()), pt + 0x28);
        STBox_getMaxWaterY(a, pos.get());
        pt = load<u32>(link);
        if (load<u8>(pt, 0x10C) == 0) {
          u32 v = load<u32>(record) + written * 0x28;
          store(v, 0, (f32)pos->x);
          store(v, 4, (f32)pos->y);
          store(v, 8, (f32)pos->z);
          v = load<u32>(record) + written * 0x28;
          store(v, 12, textureX);
          store(v, 16, textureY);
          f32 x = a->previous[written].x, y = a->previous[written].y,
              z = a->previous[written].z;
          v = load<u32>(record) + written * 0x28;
          store(v, 24, y);
          store(v, 28, z);
          store(v, 20, x);
          v = load<u32>(record) + written * 0x28;
          store(v, 36, priorY);
          store(v, 32, textureX);
          a->previous[written].x = pos->x;
          a->previous[written].y = pos->y;
          a->previous[written].z = pos->z;
          textureX = gabi::fadds_ppc(textureX, 0.5f);
          written++;
          j++;
        }
        link = load<u32>(link, 12);
      }
    }
    textureY = gabi::fadds_ppc(textureY, increment);
  }
  shadowEmitter = gabi::ea((void *)a->emitter);
  u32 buffers = load<u32>(shadowEmitter, 0x310),
      active = load<u32>(buffers, 0x2BA70);
  u32 record = buffers + active * 0x254 + 4;
  for (u32 i = 1; i < steps; i++) {
    u32 value = load<u32>(record, 0x14C);
    gabi::call(0x027B5E94, ptr(record), 0, value);
    record += 0x4A8;
  }
  active = load<u32>(buffers, 0x2BA70);
  store<u32>(buffers, 0x2BA70, active == 0);
  material = load<u32>(particle, 0xEC);
  gabi::call(0x027FB678, ptr(material));
}
VERIFY(0x02468C20, STBox_shadowDraw);
void STBox_shadowDrawAfter(daSTBox_shadowEcallBack_c *a, void *emitter) {
  WWHD_FUNC(0x0246909C, void, a, emitter);
  u32 e = gabi::ea(emitter);
  u32 count = visibleParticles(e);
  if (count < 6)
    return;
  u32 first = load<u32>(e, 0x1AC);
  if (load<u8>(0x101EA4E4) & 1) {
    depthState(true);
    first = load<u32>(e, 0x1AC);
  }
  u32 steps = third(count), particle = load<u32>(first),
      material = load<u32>(particle, 0xEC);
  u32 vt = load<u32>(material, 12), target = load<u32>(vt, 0x2C);
  u32 se = gabi::ea((void *)a->emitter), arg = load<u32>(se, 0x26C);
  gabi::call(target, ptr(material), ptr(arg));
  se = gabi::ea((void *)a->emitter);
  u32 object = load<u32>(particle, 0xF0);
  arg = load<u32>(se, 0x26C);
  gabi::call(0x027FE5B8, ptr(object), ptr(arg));
  se = gabi::ea((void *)a->emitter);
  u32 buffers = load<u32>(se, 0x310), active = load<u32>(buffers, 0x2BA70);
  u32 record = buffers + (active == 0 ? 1 : 0) * 0x254;
  for (u32 i = 1; i < steps; i++) {
    if (load<u32>(buffers, 0x2BA80))
      gabi::call(0x027BFE5C, ptr(record + 0x158));
    u32 arg3 = load<u32>(0x104B4A88), arg6 = load<u32>(0x104B4A8C),
        arg5 = load<u32>(0x104B4A84);
    gabi::call(0xC0006178, arg3, 6, arg5, arg6, 0, 1);
    record += 0x4A8;
  }
  store<u8>(particle, 0x120, 1);
}
VERIFY(0x0246909C, STBox_shadowDrawAfter);
void STBox_staticInit() {
  WWHD_FUNC(0x024696E8, void);
  store<u32>(0x1046D840, 12, 0);
  store<u32>(0x1046D840, 8, 0);
  store<u32>(0x1046D840, 4, 0);
  store<u32>(0x1046D840, 0, 0);
  gabi::call(0x028F026C, ptr(0x101CFF8C));
  f32 low = load<f32>(0x10039C28), high = load<f32>(0x10039C2C);
  store(0x1046D834, 0, low);
  store(0x1046D838, 0, high);
  gabi::call(0x028ED6F8, ptr(0x1046D83C));
  gabi::call(0x028F026C, ptr(0x101CFF98));
  gabi::call(0x028EAB2C, ptr(0x1046D83D));
  gabi::call(0x028F026C, ptr(0x101CFFA4));
  f32 scroll = load<f32>(0x10039B74), scale = load<f32>(0x10039B78);
  store<u8>(0x1046D850, 4, 255);
  store<u32>(0x1046D850, 8, 0);
  store<u8>(0x1046D850, 12, 120);
  f32 depth = load<f32>(0x10039BC8);
  store(0x1046D850, 24, scale);
  store(0x1046D850, 16, depth);
  store<u32>(0x1046D850, 0, 0x10039C8C);
  store(0x1046D850, 20, scroll);
}
VERIFY(0x024696E8, STBox_staticInit);

/* ---- leftover functions of the translation unit ---- */

/* 02469818 daSTBox_shadowEcallBack_c::executeAfter (JPACallBackBase default, empty; vtable slot 10039C70) */
static void STBox_shadow_executeAfter(void* p) {
    WWHD_FUNC(0x02469818, void, p);
}
VERIFY(0x02469818, STBox_shadow_executeAfter);

/* 0246981C daSTBox_shadowEcallBack_c::setup (vtable slot 10039C88). GameCube: field_0x4 = 0, mpAngle,
 * mpEmitter. HD adds: look up the "salvage" shader program in the program archive (load it on first
 * use) and set it on the emitter (+0x26C), then notify the emitter (0281E76C). */
static void STBox_shadowSetup(void* cb, void* emitter, void* pos, void* angle) {
    WWHD_FUNC(0x0246981C, void, cb, emitter, pos, angle);
    u32 t = gabi::ea(cb), e = gabi::ea(emitter);
    gabi::store<u32>(t + 0x44, gabi::ea(angle));
    gabi::store<u32>(t + 0x54, e);
    gabi::store<s16>(t + 4, 0);
    gabi::Local<u32[2]> name; /* sead::SafeString {text, vtable} */
    gabi::store<u32>(gabi::ea(name.get()) + 4, 0x10039AD4);
    gabi::store<u32>(gabi::ea(name.get()), 0x10039AC4); /* "salvage" */
    u32 arc = gabi::call<u32>(0x027FFCBC);
    s32 idx = gabi::call<s32>(0x027B90AC, gabi::load<u32>(arc + 4), name.get());
    u32 prog;
    if (idx < 0) {
        prog = 0;
    } else {
        u32 n = gabi::load<u32>(arc + 8);
        prog = gabi::load<u32>(arc + 0xC);
        u32 ent = (u32)idx < n ? prog + idx * 0x24 : prog;
        if (gabi::load<u8>(ent + 0x20) == 0) {
            u32 a = gabi::load<u32>(arc + 4);
            u32 prm = (u32)idx < gabi::load<u32>(a + 0x1C) ? gabi::load<u32>(a + 0x20) + idx * 0x84 : 0;
            gabi::call(0x02800B0C, ent, prm, 0);
            n = gabi::load<u32>(arc + 8);
            prog = gabi::load<u32>(arc + 0xC);
        }
        if ((u32)idx < n) prog += idx * 0x24;
    }
    gabi::store<u32>(e + 0x26C, prog);
    gabi::call(0x0281E76C, gabi::load<u32>(t + 0x54));
}
VERIFY(0x0246981C, STBox_shadowSetup);

/* 0246994C daSTBox_c::initWait (empty; event_init_tbl entry 101CFF0C, as on GameCube) */
static void STBox_initWait(void* p) {
    WWHD_FUNC(0x0246994C, void, p);
}
VERIFY(0x0246994C, STBox_initWait);

/* 02469950 daSTBox_c::initWaitDummy (empty; event_init_tbl entry 101CFF24, as on GameCube) */
static void STBox_initWaitDummy(void* p) {
    WWHD_FUNC(0x02469950, void, p);
}
VERIFY(0x02469950, STBox_initWaitDummy);

/* ---- TU tail ---- */

/* 02469954 daSTBox_c::actWaitGetItem (event_action_tbl entry 101CFF44; returns TRUE as on GameCube) */
static BOOL STBox_actWaitGetItem(void* p, int i) {
    WWHD_FUNC(0x02469954, BOOL, p, i);
    return TRUE;
}
VERIFY(0x02469954, STBox_actWaitGetItem);

/* 0246995C daSTBox_c::actWaitDummy (event_action_tbl entry 101CFF4C; returns TRUE as on GameCube) */
static BOOL STBox_actWaitDummy(void* p, int i) {
    WWHD_FUNC(0x0246995C, BOOL, p, i);
    return TRUE;
}
VERIFY(0x0246995C, STBox_actWaitDummy);

/* 02469964 daSTBox_c deleting destructor (HD virtual destructor, actor vtable slot 10039B18):
 * the fopAc_ac_c base destructor, then operator delete when bit 0 of the flags is set */
static void STBox_deletingDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02469964, void, p, flags);
    if (p == 0) return;
    gabi::call(0x025D50BC, p, 0u);
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x02469964, STBox_deletingDtor);

/* 024699B8 this TU's sead::SafeString copy: empty assureTerminationImpl_ (vtable 10039AD4) */
static void STBox_SafeString_assureTerminationImpl(u32 p) {
    WWHD_FUNC(0x024699B8, void, p);
}
VERIFY(0x024699B8, STBox_SafeString_assureTerminationImpl);
