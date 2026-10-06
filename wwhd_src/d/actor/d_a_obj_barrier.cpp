/* Hyrule electric barrier. Actual HD TU02322390..0232409B,35 functions,
 * including specialized effect BCK construction and trailing cleanup helpers.
 */
#include "d/actor/d_a_obj_barrier.h"
#include "bindings.h"
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
struct BarrierArchive {
  be<u32> name, vt;
};
u32 resource(s32 index) {
  gabi::Local<BarrierArchive> arc;
  arc->name = 0x10026024;
  arc->vt = 0x1002602C;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), arc.get(),
                         index);
}
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 player() { return read<u32>(play(), 0x5B2C); }
void assertion(u32 file, s32 line, u32 expression) {
  gabi::call(0x0273AA24, ptr(file), line, ptr(expression));
}
void setRenderList(u32 first, u32 second) {
  write<u32>(0x104B4634, 0, read<u32>(play(), first));
  write<u32>(0x104B4634, 4, read<u32>(play(), second));
}
} // namespace
BOOL barrier_animationInit(BarrierAnimation *animation) {
  WWHD_FUNC(0x02322390, BOOL, animation);
  u32 data = resource(10), btk = resource(18), brk = resource(14);
  BOOL ok = 1;
  if (!data || !btk || !brk) {
    assertion(0x1002611C, 411, 0x10026118);
    ok = 0;
  } else {
    animation->model =
        gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x01000200);
    s32 btkOK = gabi::call<s32>(0x025E7CE0, &animation->btk, ptr(data),
                                ptr(btk), 1, 2, 1.f, 0, -1, 0, 0);
    s32 brkOK = gabi::call<s32>(0x025E8154, &animation->brk, ptr(data),
                                ptr(brk), 1, 2, 1.f, 0, -1, 0, 0);
    if (!animation->model || !btkOK || !brkOK)
      ok = 0;
  }
  return ok;
}
VERIFY(0x02322390, barrier_animationInit);
BOOL barrier_createHeap(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x023229A4, BOOL, barrier);
  s32 anm = gabi::call<s32>(0x02322390, &barrier->animation),
      effect = gabi::call<s32>(0x02322760, &barrier->effect);
  BOOL ok = 1;
  if (!anm || !effect)
    ok = 0;
  else {
    u32 dzb = resource(22);
    barrier->background =
        gabi::call<u32>(0x024F2478, ptr(dzb), 1, &barrier->backgroundMatrix);
    if (!barrier->background)
      ok = 0;
  }
  return ok;
}
VERIFY(0x023229A4, barrier_createHeap);
BOOL barrier_heapCallback(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02322A50, BOOL, barrier);
  return gabi::call<BOOL>(0x023229A4, barrier);
}
VERIFY(0x02322A50, barrier_heapCallback);
void barrier_initMatrix(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02322A54, void, barrier);
  u32 model = barrier->animation.model;
  J3DModel_setBaseScale(gabi::at<J3DModel>(model), &barrier->scale);
  mDoMtx_stack_c::transS(barrier->current.pos.x, barrier->current.pos.y,
                         barrier->current.pos.z);
  mDoMtx_stack_c::YrotM(barrier->shape_angle.y);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>((u32)barrier->animation.model),
                        mDoMtx_stack_c::get());
  mDoMtx_stack_c::scaleM(barrier->scale.x, barrier->scale.y, barrier->scale.z);
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &barrier->backgroundMatrix);
}
VERIFY(0x02322A54, barrier_initMatrix);
void barrier_effectCreate(BarrierEffect *effect) {
  WWHD_FUNC(0x02322B48, void, effect);
  for (u32 i = 0; i < 4; i++) {
    u32 model = effect->model[i];
    f32 z = read<f32>(0x10469298, 8), x = read<f32>(0x10469298),
        y = read<f32>(0x10469298, 4);
    write<f32>(model, 0xBC, x);
    write<f32>(model, 0xC4, z);
    write<f32>(model, 0xC0, y);
  }
}
VERIFY(0x02322B48, barrier_effectCreate);
BOOL barrier_Create(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02322E88, BOOL, barrier);
  return gabi::call<BOOL>(0x02322B7C, barrier);
}
VERIFY(0x02322E88, barrier_Create);
BOOL barrier_delete(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02322E8C, BOOL, barrier);
  if (barrier->active) {
    gabi::call(0x025204C8, &barrier->phase, ptr(0x10026024));
    if (read<u32>(gabi::ea(barrier), 0xF4) && barrier->background) {
      if (read<u32>(barrier->background) < 256)
        gabi::call(0x020087EC, ptr(play(), 0x12A0), ptr(barrier->background));
      barrier->background = 0;
    }
  }
  return 1;
}
VERIFY(0x02322E8C, barrier_delete);
BOOL barrier_Delete(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02322F10, BOOL, barrier);
  return gabi::call<BOOL>(0x02322E8C, barrier);
}
VERIFY(0x02322F10, barrier_Delete);
BOOL barrier_effectCheckActor(BarrierEffect *effect, void *actor) {
  WWHD_FUNC(0x02323000, BOOL, effect, actor);
  for (u32 i = 0; i < 4; i++)
    if (effect->hitActor[i] == gabi::ea(actor))
      return 1;
  return 0;
}
VERIFY(0x02323000, barrier_effectCheckActor);
void barrier_effectExecute(BarrierEffect *effect) {
  WWHD_FUNC(0x02323AB4, void, effect);
  u32 flags = effect->activeFlags;
  for (u32 i = 0; i < 4; i++)
    if (flags & (1u << i)) {
      effect->btk[i].play();
      if (effect->btk[i].mFrameCtrl.checkState(1) ||
          effect->btk[i].mFrameCtrl.mRate == 0.f) {
        flags ^= 1u << i;
        effect->hitActor[i] = 0;
      }
    }
  effect->activeFlags = flags;
}
VERIFY(0x02323AB4, barrier_effectExecute);
BOOL barrier_execute(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02323B74, BOOL, barrier);
  barrier->animation.btk.play();
  gabi::call(0x02322F14, barrier);
  gabi::call(0x024F43DC, ptr(barrier->background));
  gabi::call(0x02515E50, ptr(gabi::ea(barrier), 0x4F8));
  gabi::call(0x02515E50, ptr(gabi::ea(barrier), 0x534));
  gabi::call(0x023234F0, barrier);
  if (!gabi::call<s32>(0x023235B4, barrier))
    gabi::call(0x023236E8, barrier);
  if (!gabi::call<s32>(0x02323A44, barrier))
    gabi::call(0x02323AB4, &barrier->effect);
  return 1;
}
VERIFY(0x02323B74, barrier_execute);
BOOL barrier_Execute(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02323C00, BOOL, barrier);
  return gabi::call<BOOL>(0x02323B74, barrier);
}
VERIFY(0x02323C00, barrier_Execute);
BOOL barrier_Draw(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02323E28, BOOL, barrier);
  return gabi::call<BOOL>(0x02323D60, barrier);
}
VERIFY(0x02323E28, barrier_Draw);
BOOL barrier_IsDelete() {
  WWHD_FUNC(0x02323E2C, BOOL);
  return 1;
}
VERIFY(0x02323E2C, barrier_IsDelete);
u32 barrier_parameter(daObjBarrier_c *barrier, u32 width, u32 shift) {
  WWHD_FUNC(0x02324080, u32, barrier, width, shift);
  u32 mask = (width & 32) ? 0 : (1u << (width & 31));
  u32 value = (shift & 32) ? 0 : ((u32)barrier->mParameters >> (shift & 31));
  return value & (mask - 1);
}
VERIFY(0x02324080, barrier_parameter);

void barrier_brkAnimationPlay(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02322F14, void, barrier);
  gabi::Local<cXyz> delta, horizontal;
  gabi::call(0x0201ADE0, ptr(player(), 0x314), delta.get(),
             &barrier->current.pos);
  horizontal->x = delta->x;
  horizontal->y = 0.f;
  horizontal->z = delta->z;
  f64 distance = gabi::call<f64>(0x028F4384,
                                 gabi::call<f64>(0x028E8DD0, horizontal.get()));
  f32 radius = gabi::fmsubs(1000.f, barrier->scale.x, 150.f);
  f32 depth = distance > radius ? 0.f : f32(radius - distance);
  barrier->animation.brkFrame =
      depth > 700.f ? 1.f : gabi::fmadds(1.f - depth / 700.f, 59.f, 1.f);
}
VERIFY(0x02322F14, barrier_brkAnimationPlay);
void barrier_breakStartWait(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x0232384C, void, barrier);
  if (gabi::call<s32>(0x025B8B94, ptr(read<u32>(0x101F84DC), 0x644), 0x3980) !=
      1)
    return;
  u32 link = player();
  gabi::Local<cXyz> delta, horizontal;
  gabi::call(0x0201ADE0, ptr(link, 0x314), delta.get(), &barrier->current.pos);
  horizontal->x = delta->x;
  horizontal->y = 0.f;
  horizontal->z = delta->z;
  f64 distance = gabi::call<f64>(0x028F4384,
                                 gabi::call<f64>(0x028E8DD0, horizontal.get()));
  if (distance < 8800.f)
    return;
  if (read<u8>(read<u32>(0x101F84DC), 0x2E) != 0x3E)
    return;
  if (!read<u16>(play(), 0x5BAC))
    return;
  u8 cut = read<u8>(link, 0x3AC);
  if (cut < 1 || cut > 10)
    return;
  gabi::call(0x025B8B68, ptr(read<u32>(0x101F84DC), 0x644), 0x2C02);
  barrier->eventID =
      gabi::call<s32>(0x02543F10, ptr(play(), 0x52C4), ptr(0x1002622C), 255);
  barrier->procedure = 1;
}
VERIFY(0x0232384C, barrier_breakStartWait);
void barrier_breakOrder(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02323958, void, barrier);
  if (read<u16>(gabi::ea(barrier), 0xF8) == 2) {
    barrier->procedure = 2;
    return;
  }
  gabi::call(0x025D7A58, barrier, (s32)(s16)barrier->eventID, 255, 65535, 0, 1);
  write<u16>(gabi::ea(barrier), 0xFA, read<u16>(gabi::ea(barrier), 0xFA) | 2);
}
VERIFY(0x02323958, barrier_breakOrder);
void barrier_breakEndWait(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x023239D4, void, barrier);
  s32 event = (s16)barrier->eventID;
  if (gabi::call<s32>(0x025440C8, ptr(play(), 0x52C4), event)) {
    u32 game = play();
    write<u16>(game, 0x52B8, read<u16>(game, 0x52B8) | 8);
    gabi::call(0x025D57E0, barrier);
    gabi::call(0x025F05D0, 26, 0);
  }
}
VERIFY(0x023239D4, barrier_breakEndWait);
BOOL barrier_breakCheck(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02323A44, BOOL, barrier);
  if (barrier->moya)
    return 0;
  switch ((u32)barrier->procedure) {
  case 0:
    gabi::call(0x0232384C, barrier);
    return 0;
  case 1:
    gabi::call(0x02323958, barrier);
    return 1;
  case 2:
    gabi::call(0x023239D4, barrier);
    return 1;
  default:
    return 0;
  }
}
VERIFY(0x02323A44, barrier_breakCheck);
void *barrier_bckConstructor(void *object) {
  WWHD_FUNC(0x02323EE0, void *, object);
  u32 address = gabi::ea(object);
  if (!address) {
    address = gabi::call<u32>(0x0273AD10, 0x8C);
    if (!address)
      return nullptr;
  }
  gabi::call(0x027F2BC0, ptr(address), 0);
  write<u32>(address, 0x10, 0x1016E54C);
  gabi::call(0x027DA984, ptr(address, 0x14));
  write<u32>(address, 0x88, 0);
  write<u32>(address, 0x48, 0x1016D820);
  write<u32>(address, 0x58, 0);
  write<u32>(address, 0x84, 0);
  write<u32>(address, 0x10, 0x10026054);
  write<u32>(address, 0x80, 0);
  write<u32>(address, 0x7C, 0);
  return ptr(address);
}
VERIFY(0x02323EE0, barrier_bckConstructor);
void barrier_trivialDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x02323F70, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x02323F70, barrier_trivialDestructor);
void barrier_bckDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x02323F84, void, object, flags);
  if (object) {
    gabi::call(0x027F3628, ptr(gabi::ea(object), 0x10), 0);
    if (flags & 1)
      gabi::call(0x0273AF40, object);
  }
}
VERIFY(0x02323F84, barrier_bckDestructor);
void barrier_actorDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x02323FD8, void, object, flags);
  if (!object)
    return;
  u32 address = gabi::ea(object);
  gabi::call(0x028F0164, ptr(address, 0x998), 4, 0x8C, ptr(0x02323F84), 0, 0);
  gabi::call(0x02515A70, ptr(address, 0x684), 2);
  gabi::call(0x02515A70, ptr(address, 0x554), 2);
  gabi::call(0x02515860, ptr(address, 0x518), 2);
  gabi::call(0x02515860, ptr(address, 0x4DC), 2);
  gabi::call(0x025D50BC, object, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, object);
}
VERIFY(0x02323FD8, barrier_actorDestructor);
void barrier_noop(void *object) { WWHD_FUNC(0x0232407C, void, object); }
VERIFY(0x0232407C, barrier_noop);

void barrier_effectDraw(BarrierEffect *effect) {
  WWHD_FUNC(0x02323C04, void, effect);
  u32 flags = effect->activeFlags;
  for (u32 i = 0; i < 4; i++)
    if (flags & (1u << i)) {
      u32 model = effect->model[i];
      gabi::call(0x025E7FC4, &effect->btk[i], ptr(read<u32>(model, 0xAC)),
                 (f32)effect->btk[i].mFrameCtrl.mFrame);
      f32 frame = (s16)gabi::ftoi(effect->btk[i].mFrameCtrl.mFrame);
      gabi::call(0x025E86B8, &effect->bck[i], ptr(read<u32>(model, 0xAC)),
                 frame);
      frame = (s16)gabi::ftoi(effect->btk[i].mFrameCtrl.mFrame);
      gabi::call(0x025E83FC, &effect->brk[i], ptr(read<u32>(model, 0xAC)),
                 frame);
      setRenderList(0x5D8C, 0x5D90);
      gabi::call(0x025E2DE0, ptr(model), 0);
      setRenderList(0x5D78, 0x5D7C);
    }
}
VERIFY(0x02323C04, barrier_effectDraw);
BOOL barrier_draw(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02323D60, BOOL, barrier);
  if (!barrier->procedure) {
    if (!barrier->moya && barrier->animation.brkFrame > 1.f) {
      gabi::call(0x025E7FC4, &barrier->animation.btk,
                 ptr(read<u32>(barrier->animation.model, 0xAC)),
                 (f32)barrier->animation.btk.mFrameCtrl.mFrame);
      f32 frame = (s16)gabi::ftoi(barrier->animation.brkFrame);
      gabi::call(0x025E83FC, &barrier->animation.brk,
                 ptr(read<u32>(barrier->animation.model, 0xAC)), frame);
      gabi::call(0x025E2DE0, ptr(barrier->animation.model), 0);
    }
    gabi::call(0x02323C04, &barrier->effect);
  }
  return 1;
}
VERIFY(0x02323D60, barrier_draw);
void barrier_staticInit() {
  WWHD_FUNC(0x02323E34, void);
  for (s32 i = 3; i >= 0; i--)
    write<u32>(0x10469288, i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101C7FA4));
  write<f32>(0x1046927C, 0, read<f32>(0x10026238));
  write<f32>(0x10469280, 0, read<f32>(0x1002623C));
  gabi::call(0x028ED6F8, ptr(0x10469284));
  gabi::call(0x028F026C, ptr(0x101C7FB0));
  gabi::call(0x028EAB2C, ptr(0x10469285));
  gabi::call(0x028F026C, ptr(0x101C7FBC));
  for (u32 i = 0; i < 3; i++)
    write<f32>(0x10469298, i * 4, 1.f);
}
VERIFY(0x02323E34, barrier_staticInit);
void barrier_checkCollisionAttack(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x023234F0, void, barrier);
  u32 address = gabi::ea(barrier);
  if (gabi::call<s32>(0x025160DC, ptr(address, 0x554))) {
    u32 actor = gabi::call<u32>(0x02515BBC, ptr(address, 0x5A4));
    u32 link = player();
    if (actor && actor == link) {
      gabi::Local<cXyz> center, hit;
      center->x = read<f32>(address, 0x66C);
      center->y = read<f32>(address, 0x670);
      center->z = read<f32>(address, 0x674);
      f32 radius = 1000.f * barrier->scale.x;
      hit->x = read<f32>(actor, 0x314);
      hit->y = read<f32>(actor, 0x318);
      hit->z = read<f32>(actor, 0x31C);
      gabi::call(0x0232302C, &barrier->effect, ptr(actor), center.get(),
                 hit.get(), 1, radius);
    }
    gabi::call(0x02516094, ptr(address, 0x554));
  }
}
VERIFY(0x023234F0, barrier_checkCollisionAttack);
BOOL barrier_checkCollisionTarget(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x023235B4, BOOL, barrier);
  u32 address = gabi::ea(barrier);
  if (gabi::call<s32>(0x025162A4, ptr(address, 0x684))) {
    u32 actor = gabi::call<u32>(0x02515BBC, ptr(address, 0x718));
    if (actor) {
      f32 z = read<f32>(actor, 0x31C), y = read<f32>(actor, 0x318),
          x = read<f32>(actor, 0x314);
      s32 target = 1;
      if (actor == player()) {
        target = 0;
        if (gabi::call<u32>(0x02516300, ptr(address, 0x684))) {
          y = read<f32>(address, 0x754);
          x = read<f32>(address, 0x750);
          z = read<f32>(address, 0x758);
        }
      }
      gabi::Local<cXyz> center, hit;
      center->x = read<f32>(address, 0x79C);
      center->y = read<f32>(address, 0x7A0);
      center->z = read<f32>(address, 0x7A4);
      f32 radius = 1000.f * barrier->scale.x;
      hit->x = x;
      hit->y = y;
      hit->z = z;
      gabi::call(0x0232302C, &barrier->effect, ptr(actor), center.get(),
                 hit.get(), target, radius);
    }
    gabi::call(0x0251621C, ptr(address, 0x684));
  }
  return 0;
}
VERIFY(0x023235B4, barrier_checkCollisionTarget);
void barrier_registerCollision(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x023236E8, void, barrier);
  u32 address = gabi::ea(barrier);
  gabi::Local<cXyz> center, delta;
  center->x = barrier->current.pos.x;
  center->y = barrier->current.pos.y - 300.f;
  center->z = barrier->current.pos.z;
  f32 height = gabi::fmadds(10000.f, barrier->scale.y, 300.f),
      radius = 1000.f * barrier->scale.x;
  gabi::call(0x020182E0, ptr(address, 0x66C), center.get());
  gabi::call(0x020184DC, ptr(address, 0x66C), radius - 60.f);
  gabi::call(0x02018428, ptr(address, 0x66C), height);
  gabi::call(0x020182E0, ptr(address, 0x79C), center.get());
  gabi::call(0x020184DC, ptr(address, 0x79C), radius - 20.f);
  gabi::call(0x02018428, ptr(address, 0x79C), height);
  gabi::call(0x0201ADE0, &barrier->current.pos, delta.get(),
             ptr(player(), 0x314));
  write<u32>(address, 0x5D0, read<u32>(gabi::ea(delta.get())));
  write<u32>(address, 0x5D8, read<u32>(gabi::ea(delta.get()), 8));
  write<u32>(address, 0x5D4, read<u32>(gabi::ea(delta.get()), 4));
  gabi::call(0x0200E240, ptr(play(), 0x26A4), ptr(address, 0x554));
  gabi::call(0x0200E240, ptr(play(), 0x26A4), ptr(address, 0x684));
}
VERIFY(0x023236E8, barrier_registerCollision);

BOOL barrier_effectInit(BarrierEffect *effect) {
  WWHD_FUNC(0x02322760, BOOL, effect);
  u32 data = resource(11), btk = resource(19), bck = resource(7),
      brk = resource(15);
  if (!data || !btk || !bck || !brk) {
    assertion(0x10026184, 1048, 0x10026180);
    return 0;
  }
  for (u32 i = 0; i < 4; i++) {
    effect->model[i] =
        gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x05020200);
    gabi::call(0x02322518, effect, i);
    s32 btkOK = gabi::call<s32>(0x025E7CE0, &effect->btk[i], ptr(data),
                                ptr(btk), 1, 0, 1.f, 0, -1, 0, 0);
    s32 bckOK = gabi::call<s32>(0x025E8508, &effect->bck[i], ptr(data),
                                ptr(bck), 1, 0, 0.f, 0, -1, 0);
    s32 brkOK = gabi::call<s32>(0x025E8154, &effect->brk[i], ptr(data),
                                ptr(brk), 1, 0, 0.f, 0, -1, 0, 0);
    if (!effect->model[i] || !btkOK || !bckOK || !brkOK)
      return 0;
  }
  return 1;
}
VERIFY(0x02322760, barrier_effectInit);
BOOL barrier_create(daObjBarrier_c *barrier) {
  WWHD_FUNC(0x02322B7C, BOOL, barrier);
  u32 a = gabi::ea(barrier);
  s32 phase = 5;
  if (!(read<u32>(a, 0x2E4) & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, barrier);
      write<u32>(a, 0xB4, 0x1002607C);
      gabi::call(0x025E7C6C, &barrier->animation.btk);
      gabi::call(0x025E80D0, &barrier->animation.brk);
      barrier->animation.brkFrame = 0.f;
      gabi::call(0x0200BD2C, ptr(a, 0x4DC));
      gabi::call(0x02515DA0, ptr(a, 0x4F8));
      write<u32>(a, 0x4F4, 0x1004AE88);
      write<u32>(a, 0x4F8, 0x1004AEC0);
      gabi::call(0x0200BD2C, ptr(a, 0x518));
      gabi::call(0x02515DA0, ptr(a, 0x534));
      write<u32>(a, 0x530, 0x1004AE88);
      write<u32>(a, 0x534, 0x1004AEC0);
      gabi::call(0x02515FB8, ptr(a, 0x554));
      write<u32>(a, 0x668, 0x100015A8);
      write<u32>(a, 0x664, 0x10026044);
      gabi::call(0x02018590, ptr(a, 0x66C));
      write<u32>(a, 0x590, 0x1004B108);
      write<u32>(a, 0x668, 0x1004B160);
      write<u32>(a, 0x680, 0x1004B150);
      gabi::call(0x02515FB8, ptr(a, 0x684));
      write<u32>(a, 0x798, 0x100015A8);
      write<u32>(a, 0x794, 0x10026044);
      gabi::call(0x02018590, ptr(a, 0x79C));
      write<u32>(a, 0x6C0, 0x1004B108);
      write<u32>(a, 0x7B0, 0x1004B150);
      write<u32>(a, 0x798, 0x1004B160);
      gabi::call(0x028EFFD0, ptr(a, 0x7C8), 4, 0x74, ptr(0x025E7C6C));
      gabi::call(0x028EFFD0, ptr(a, 0x998), 4, 0x8C, ptr(0x02323EE0));
      gabi::call(0x028EFFD0, ptr(a, 0xBC8), 4, 0x78, ptr(0x025E80D0));
      barrier->effect.activeFlags = 0;
    }
    write<u32>(a, 0x2E4, read<u32>(a, 0x2E4) | 8);
  }
  if (read<u8>(a, 0xC)) {
    if (!barrier->active)
      return phase;
  } else {
    s32 moya = gabi::call<s32>(0x02324080, barrier, 1, 8);
    barrier->moya = moya;
    if (!moya && gabi::call<s32>(0x025B8B94, ptr(read<u32>(0x101F84DC), 0x644),
                                 0x2C02) == 1)
      barrier->active = 0;
    else
      barrier->active = 1;
    if (!barrier->active)
      return phase;
  }
  phase = gabi::call<s32>(0x02520460, &barrier->phase, ptr(0x10026024));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, barrier, ptr(0x02322A50), 0x30E0))
    return 5;
  if (gabi::call<s32>(0x024EEA6C, ptr(play(), 0x12A0), ptr(barrier->background),
                      barrier))
    return 5;
  write<u32>(barrier->background, 0xA8, 0);
  u32 model = barrier->animation.model;
  barrier->scale.z = barrier->scale.x;
  write<u32>(a, 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x02322A54, barrier);
  gabi::call(0x02515F14, ptr(a, 0x4DC), 255, 255, barrier);
  gabi::call(0x02516518, ptr(a, 0x554), ptr(0x1002608C));
  write<u32>(a, 0x598, a + 0x4DC);
  write<u32>(a, 0x594, read<u32>(a, 0x594) | 2);
  gabi::call(0x02515F14, ptr(a, 0x518), 255, 255, barrier);
  gabi::call(0x02516518, ptr(a, 0x684), ptr(0x100260D0));
  write<u32>(a, 0x6C8, a + 0x518);
  write<u32>(a, 0x6C4, read<u32>(a, 0x6C4) | 2);
  if (gabi::call<s32>(0x02324080, barrier, 1, 16))
    write<u8>(a, 0x568, 0);
  gabi::call(0x02322B48, &barrier->effect);
  return phase;
}
VERIFY(0x02322B7C, barrier_create);

void barrier_effectBirth(BarrierEffect *effect, void *actor, cXyz *center,
                         cXyz *hit, s32 check, f32 radius) {
  WWHD_FUNC(0x0232302C, void, effect, actor, center, hit, check, radius);
  if (check && gabi::call<s32>(0x02323000, effect, actor))
    return;
  gabi::Local<csXyz> angle;
  gabi::Local<cXyz> position;
  // Real callees write the caller linkage word at SP+4. Keep it outside locals.
  gabi::Local<BarrierArchive> outgoingLinkage;
  gabi::call(0x0201A478, angle.get(), 0, 0, 0);
  u32 yaw = gabi::call<u32>(0x020195B0, (f32)center->x - (f32)hit->x,
                            (f32)center->z - (f32)hit->z);
  angle->y = yaw;
  u32 trig = 0x104A44F8 + ((yaw & 0xFFFF) >> 3) * 8;
  position->z = gabi::fnmsubs(radius, read<f32>(trig, 4), center->z);
  position->x = gabi::fnmsubs(radius, read<f32>(trig), center->x);
  u32 flags = effect->activeFlags;
  position->y = hit->y;
  s32 chosen = -1;
  for (u32 i = 0; i < 4; i++)
    if (!(flags & (1u << i))) {
      chosen = i;
      break;
    }
  if (chosen == -1) {
    f32 maximum = -1.f;
    for (u32 i = 0; i < 4; i++)
      if (effect->btk[i].mFrameCtrl.mFrame > maximum) {
        maximum = effect->btk[i].mFrameCtrl.mFrame;
        chosen = i;
      }
  }
  for (u32 i = 0; i < 4; i++)
    if (flags & (1u << i)) {
      if (i == (u32)chosen)
        break;
      effect->btk[i].mFrameCtrl.mRate =
          ((f32)(s16)effect->btk[i].mFrameCtrl.mEnd -
           (f32)effect->btk[i].mFrameCtrl.mFrame) /
          3.f;
      break;
    }
  if (chosen == -1)
    assertion(0x100261BC, 952, 0x100261D0);
  u32 a = gabi::ea(effect), index = (u32)chosen;
  u32 bit = (index & 32) ? 0 : 1u << (index & 31);
  effect->activeFlags = (u32)effect->activeFlags | bit;
  u32 pos = a + 0x5F4 + index * 12;
  write<f32>(pos, 0, position->x);
  write<f32>(pos, 4, position->y);
  write<f32>(pos, 8, position->z);
  write<s16>(a + 0x624 + index * 2, 0, angle->y);
  write<u32>(a + 0x62C + index * 4, 0, gabi::ea(actor));
  u32 data = read<u32>(read<u32>(a + 4 + index * 4), 0xAC);
  u32 btk = resource(19);
  if (!btk)
    assertion(0x100261BC, 969, 0x100261DC);
  u32 bck = resource(7);
  if (!bck)
    assertion(0x100261BC, 974, 0x100261F0);
  u32 brk = resource(15);
  if (!brk)
    assertion(0x100261BC, 979, 0x10026204);
  gabi::call(0x025E7CE0, ptr(a + 0x14 + index * 0x74), ptr(data), ptr(btk), 1,
             0, 1.f, 0, -1, 1, 0);
  gabi::call(0x025E8508, ptr(a + 0x1E4 + index * 0x8C), ptr(data), ptr(bck), 1,
             0, 0.f, 0, -1, 1);
  gabi::call(0x025E8154, ptr(a + 0x414 + index * 0x78), ptr(data), ptr(brk), 1,
             0, 0.f, 0, -1, 1, 0);
  mDoMtx_stack_c::transS(position->x, position->y, position->z);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), 0, (s32)(s16)angle->y, 0);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>(read<u32>(a + 4 + index * 4)),
                        mDoMtx_stack_c::get());
  gabi::call(0x025A847C, ptr(read<u32>(play(), 0x5AB0)), 0, 0x81A9,
             position.get(), angle.get(), 0, 255, 0, -1, 0, 0, 0);
}
VERIFY(0x0232302C, barrier_effectBirth);
namespace {
u32 shiftLeft(u32 value, u32 shift) {
  return (shift & 32) ? 0 : value << (shift & 31);
}
u32 shiftRight(u32 value, u32 shift) {
  return (shift & 32) ? 0 : value >> (shift & 31);
}
struct BarrierTextureScratch {
  u8 bytes[80];
};
} // namespace
void barrier_effectSetDummyTexture(BarrierEffect *effect, u32 index) {
  WWHD_FUNC(0x02322518, void, effect, index);
  gabi::Local<BarrierTextureScratch> scratch;
  u32 image = gabi::ea(scratch.get()) + 8;
  u32 data = read<u32>(read<u32>(gabi::ea(effect) + 4 + index * 4), 0xAC);
  u32 texture = read<u32>(data, 0x30), names = read<u32>(data, 0x34);
  if (!texture)
    assertion(0x10026138, 821, 0x10026158);
  if (!names)
    assertion(0x10026138, 822, 0x10026168);
  for (u16 i = 0; i < read<u16>(texture); i++) {
    u32 name = gabi::call<u32>(0x027ED1F0, ptr(names), (u32)i);
    if (!name) {
      assertion(0x10026138, 827, 0x1002614C);
      continue;
    }
    u32 j = 0;
    bool match;
    do {
      u8 a = read<u8>(name, j), b = read<u8>(0x10026130, j);
      match = a == b;
      if (!match || !a)
        break;
      j++;
    } while (true);
    if (!match)
      continue;
    u32 handle = gabi::call<u32>(0x027F81A4, ptr(read<u32>(0x101F9968)), 6);
    u32 object = gabi::call<u32>(
        0x02773680, ptr(gabi::call<u32>(0x0273AD10, 0xC0)), ptr(handle));
    write<u32>(image, 0x20, object);
    write<u16>(image, 2, read<u32>(object, 8));
    write<u8>(image, 8, 0);
    write<u16>(image, 4, read<u32>(object, 0xC));
    u32 offset = (u32)i * 0x24, dest = read<u32>(texture, 4) + offset;
    for (u32 k = 0; k < 9; k++)
      write<u32>(dest, k * 4, read<u32>(image, k * 4));
    dest = read<u32>(texture, 4) + offset;
    write<u32>(dest, 0x1C, read<u32>(dest, 0x1C) + image - dest);
    dest = read<u32>(texture, 4) + offset;
    write<u32>(dest, 0xC, read<u32>(dest, 0xC) + image - dest);
    dest = read<u32>(texture, 4) + offset;
    write<u32>(dest, 0x20, read<u32>(image, 0x20));
    u32 hi = read<u32>(texture, 0x18), lo = read<u32>(texture, 0x1C),
        shift = i < 64 ? i : (u32)i - 64;
    u32 shiftedHi = shiftLeft(hi, shift) | shiftRight(lo, 32 - shift) |
                    shiftLeft(lo, shift + 32),
        shiftedLo = shiftLeft(lo, shift);
    if (i < 64) {
      u32 oldLo = read<u32>(texture, 0xC), oldHi = read<u32>(texture, 8);
      write<u32>(texture, 8, oldHi | shiftedHi);
      write<u32>(texture, 0xC, oldLo | shiftedLo);
    } else {
      u32 oldHi = read<u32>(texture, 0x10), oldLo = read<u32>(texture, 0x14);
      write<u32>(texture, 0x14, oldLo | shiftedLo);
      write<u32>(texture, 0x10, oldHi | shiftedHi);
    }
  }
}
VERIFY(0x02322518, barrier_effectSetDummyTexture);
