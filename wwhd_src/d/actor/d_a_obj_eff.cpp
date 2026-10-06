#include "d/actor/d_a_obj_eff.h"
#include "bindings.h"
using Effect = daObjEff::Act_c;
using Smoke = daObjEff::SmokeCallback;
namespace {
template <class T> T read(u32 address, u32 offset = 0) {
  return *gabi::at<be<T>>(address + offset);
}
template <class T> void write(u32 address, u32 offset, T value) {
  *gabi::at<be<T>>(address + offset) = value;
}
void *ptr(u32 address, u32 offset = 0) {
  return gabi::at<void>(address + offset);
}

u32 play() { return gabi::call<u32>(0x025200D4); }
// GHS member-function descriptors contain adjustment, virtual slot, and target.
u32 memberTarget(Effect *&effect, u32 table) {
  u32 descriptor = table + (u32)effect->type * 8;
  s16 adjustment = read<s16>(descriptor), slot = read<s16>(descriptor, 2);
  effect = gabi::at<Effect>(gabi::ea(effect) + adjustment);
  if (slot < 0)
    return read<u32>(descriptor, 4);
  u32 vtable = read<u32>(gabi::ea(effect), (s32)read<s16>(descriptor, 6));
  return read<u32>(vtable, (u32)slot * 8 + 4);
}
u32 particle(Effect *effect, u16 id, cXyz *scale, cXyz *extra) {
  u32 callback = effect->callback;
  u32 controller = read<u32>(play(), 0x5AB0);
  return gabi::call<u32>(0x025A847C, ptr(controller), 2, (u32)id,
                         &effect->current.pos, ptr(0), scale, 180,
                         ptr(callback), -1, ptr(0), ptr(0), extra);
}
} // namespace
Smoke *eff_barrelCallbackCtor(Smoke *callback) {
  WWHD_FUNC(0x0233A258, Smoke *, callback);
  if (!callback)
    callback = gabi::call<Smoke *>(0x0273AD10, 0x24);
  if (callback) {
    gabi::call(0x025A5CEC, callback, ptr(0x10027BE0), 0, 0);
    callback->life = 60;
    callback->vtable = 0x10027BF4;
  }
  return callback;
}
VERIFY(0x0233A258, eff_barrelCallbackCtor);
Smoke *eff_stoolCallbackCtor(Smoke *callback) {
  WWHD_FUNC(0x0233A35C, Smoke *, callback);
  if (!callback)
    callback = gabi::call<Smoke *>(0x0273AD10, 0x24);
  if (callback) {
    gabi::call(0x025A5CEC, callback, ptr(0x10027BE4), 0, 0);
    callback->life = 60;
    callback->vtable = 0x10027C3C;
  }
  return callback;
}
VERIFY(0x0233A35C, eff_stoolCallbackCtor);
Smoke *eff_skullCallbackCtor(Smoke *callback) {
  WWHD_FUNC(0x0233A460, Smoke *, callback);
  if (!callback)
    callback = gabi::call<Smoke *>(0x0273AD10, 0x24);
  if (callback) {
    gabi::call(0x025A5CEC, callback, ptr(0x10027BE8), 0, 0);
    callback->life = 60;
    callback->vtable = 0x10027C84;
  }
  return callback;
}
VERIFY(0x0233A460, eff_skullCallbackCtor);
Smoke *eff_pineconeCallbackCtor(Smoke *callback) {
  WWHD_FUNC(0x0233A5BC, Smoke *, callback);
  if (!callback)
    callback = gabi::call<Smoke *>(0x0273AD10, 0x24);
  if (callback) {
    gabi::call(0x025A5CEC, callback, ptr(0x10027BEC), 0, 0);
    callback->life = 60;
    callback->vtable = 0x10027CCC;
  }
  return callback;
}
VERIFY(0x0233A5BC, eff_pineconeCallbackCtor);
Smoke *eff_woodboxCallbackCtor(Smoke *callback) {
  WWHD_FUNC(0x0233A6C0, Smoke *, callback);
  if (!callback)
    callback = gabi::call<Smoke *>(0x0273AD10, 0x24);
  if (callback) {
    gabi::call(0x025A5CEC, callback, ptr(0x10027BF0), 0, 0);
    callback->life = 60;
    callback->vtable = 0x10027D14;
  }
  return callback;
}
VERIFY(0x0233A6C0, eff_woodboxCallbackCtor);
void *eff_landCallbackCtor(void *callback) {
  WWHD_FUNC(0x0233A564, void *, callback);
  if (!callback)
    callback = gabi::call<void *>(0x0273AD10, 0x20);
  if (callback) {
    gabi::call(0x025A5B18, callback, 0);
    write<u32>(gabi::ea(callback), 0, 0x10027A88);
  }
  return callback;
}
VERIFY(0x0233A564, eff_landCallbackCtor);
void eff_barrelCallbackUpdate(Smoke *callback, void *emitter) {
  WWHD_FUNC(0x0233A2C4, void, callback, emitter);
  s32 life = (s32)((u32)(s32)callback->life - 1);
  callback->life = life;
  if (life <= 0) {
    gabi::call_ptr(read<u32>(callback->vtable, 0x44), callback, emitter);
    return;
  }
  if (life < 50)
    write<u8>(gabi::ea(emitter), 0x247, (u8)gabi::ftoi((f32)life * 3.6f));
}
VERIFY(0x0233A2C4, eff_barrelCallbackUpdate);
void eff_stoolCallbackUpdate(Smoke *callback, void *emitter) {
  WWHD_FUNC(0x0233A3C8, void, callback, emitter);
  s32 life = (s32)((u32)(s32)callback->life - 1);
  callback->life = life;
  if (life <= 0) {
    gabi::call_ptr(read<u32>(callback->vtable, 0x44), callback, emitter);
    return;
  }
  if (life < 50)
    write<u8>(gabi::ea(emitter), 0x247, (u8)gabi::ftoi((f32)life * 3.6f));
}
VERIFY(0x0233A3C8, eff_stoolCallbackUpdate);
void eff_skullCallbackUpdate(Smoke *callback, void *emitter) {
  WWHD_FUNC(0x0233A4CC, void, callback, emitter);
  s32 life = (s32)((u32)(s32)callback->life - 1);
  callback->life = life;
  if (life <= 0) {
    gabi::call_ptr(read<u32>(callback->vtable, 0x44), callback, emitter);
    return;
  }
  if (life < 50)
    write<u8>(gabi::ea(emitter), 0x247, (u8)gabi::ftoi((f32)life * 3.6f));
}
VERIFY(0x0233A4CC, eff_skullCallbackUpdate);
void eff_pineconeCallbackUpdate(Smoke *callback, void *emitter) {
  WWHD_FUNC(0x0233A628, void, callback, emitter);
  s32 life = (s32)((u32)(s32)callback->life - 1);
  callback->life = life;
  if (life <= 0) {
    gabi::call_ptr(read<u32>(callback->vtable, 0x44), callback, emitter);
    return;
  }
  if (life < 50)
    write<u8>(gabi::ea(emitter), 0x247, (u8)gabi::ftoi((f32)life * 3.6f));
}
VERIFY(0x0233A628, eff_pineconeCallbackUpdate);
void eff_woodboxCallbackUpdate(Smoke *callback, void *emitter) {
  WWHD_FUNC(0x0233A72C, void, callback, emitter);
  s32 life = (s32)((u32)(s32)callback->life - 1);
  callback->life = life;
  if (life <= 0) {
    gabi::call_ptr(read<u32>(callback->vtable, 0x44), callback, emitter);
    return;
  }
  if (life < 50)
    write<u8>(gabi::ea(emitter), 0x247, (u8)gabi::ftoi((f32)life * 3.6f));
}
VERIFY(0x0233A72C, eff_woodboxCallbackUpdate);
BOOL eff_createHeap(Effect *effect) {
  WWHD_FUNC(0x0233A7C4, BOOL, effect);
  u32 target = memberTarget(effect, 0x10027ADC);
  return gabi::call_ptr<s32>(target, effect) != 0;
}
VERIFY(0x0233A7C4, eff_createHeap);
BOOL eff_heapWrapper(Effect *effect) {
  WWHD_FUNC(0x0233A854, BOOL, effect);
  return gabi::call<BOOL>(0x0233A7C4, effect);
}
VERIFY(0x0233A854, eff_heapWrapper);
BOOL eff_set(Effect *effect) {
  WWHD_FUNC(0x0233A858, BOOL, effect);
  u32 target = memberTarget(effect, 0x10027B0C);
  return gabi::call_ptr<BOOL>(target, effect);
}
VERIFY(0x0233A858, eff_set);
s32 eff_create(Effect *effect) {
  WWHD_FUNC(0x0233A8A4, s32, effect);
  effect->type = gabi::call<s32>(0x0233B484, effect, 8, 0);
  u32 a = gabi::ea(effect), flags = read<u32>(a, 0x2E4);
  if (!(flags & 8)) {
    if (effect) {
      gabi::call(0x025D4ED0, effect);
      flags = read<u32>(a, 0x2E4);
      write<u32>(a, 0xB4, 0x10027A78);
    }
    flags |= 8;
    write<u32>(a, 0x2E4, flags);
  }
  u32 heap = read<u32>(0x10027B3C, (u32)effect->type * 4);
  if (!gabi::call<s32>(0x025D63E8, effect, ptr(0x0233A854), heap))
    return 5;
  if (!gabi::call<s32>(0x0233A858, effect))
    return 5;
  gabi::call(0x025DA884, ptr(a, 0xDC));
  return 4;
}
VERIFY(0x0233A8A4, eff_create);
void eff_deleteEffect(Effect *effect) {
  WWHD_FUNC(0x0233A968, void, effect);
  u32 target = memberTarget(effect, 0x10027B54);
  gabi::call_ptr(target, effect);
}
VERIFY(0x0233A968, eff_deleteEffect);
s32 eff_delete(Effect *effect) {
  WWHD_FUNC(0x0233A9B4, s32, effect);
  gabi::call(0x0233A968, effect);
  return 1;
}
VERIFY(0x0233A9B4, eff_delete);
void eff_executeEffect(Effect *effect) {
  WWHD_FUNC(0x0233A9D8, void, effect);
  u32 target = memberTarget(effect, 0x10027B84);
  gabi::call_ptr(target, effect);
}
VERIFY(0x0233A9D8, eff_executeEffect);
s32 eff_execute(Effect *effect) {
  WWHD_FUNC(0x0233AA24, s32, effect);
  gabi::call(0x0233A9D8, effect);
  return 1;
}
VERIFY(0x0233AA24, eff_execute);
BOOL eff_barrelHeap(Effect *effect) {
  WWHD_FUNC(0x0233AA48, BOOL, effect);
  u32 callback = gabi::call<u32>(0x0273AD10, 0x24);
  if (callback)
    callback = gabi::call<u32>(0x0233A258, ptr(callback));
  effect->callback = callback;
  return callback != 0;
}
VERIFY(0x0233AA48, eff_barrelHeap);
BOOL eff_stoolHeap(Effect *effect) {
  WWHD_FUNC(0x0233AA90, BOOL, effect);
  u32 callback = gabi::call<u32>(0x0273AD10, 0x24);
  if (callback)
    callback = gabi::call<u32>(0x0233A35C, ptr(callback));
  effect->callback = callback;
  return callback != 0;
}
VERIFY(0x0233AA90, eff_stoolHeap);
BOOL eff_skullHeap(Effect *effect) {
  WWHD_FUNC(0x0233AAD8, BOOL, effect);
  u32 callback = gabi::call<u32>(0x0273AD10, 0x24);
  if (callback)
    callback = gabi::call<u32>(0x0233A460, ptr(callback));
  effect->callback = callback;
  return callback != 0;
}
VERIFY(0x0233AAD8, eff_skullHeap);
BOOL eff_landHeap(Effect *effect) {
  WWHD_FUNC(0x0233AB20, BOOL, effect);
  u32 callback = gabi::call<u32>(0x0273AD10, 0x20);
  if (callback)
    callback = gabi::call<u32>(0x0233A564, ptr(callback));
  effect->callback = callback;
  return callback != 0;
}
VERIFY(0x0233AB20, eff_landHeap);
BOOL eff_pineconeHeap(Effect *effect) {
  WWHD_FUNC(0x0233AB68, BOOL, effect);
  u32 callback = gabi::call<u32>(0x0273AD10, 0x24);
  if (callback)
    callback = gabi::call<u32>(0x0233A5BC, ptr(callback));
  effect->callback = callback;
  return callback != 0;
}
VERIFY(0x0233AB68, eff_pineconeHeap);
BOOL eff_woodboxHeap(Effect *effect) {
  WWHD_FUNC(0x0233ABB0, BOOL, effect);
  u32 callback = gabi::call<u32>(0x0273AD10, 0x24);
  if (callback)
    callback = gabi::call<u32>(0x0233A6C0, ptr(callback));
  effect->callback = callback;
  return callback != 0;
}
VERIFY(0x0233ABB0, eff_woodboxHeap);
BOOL eff_barrelSet(Effect *effect) {
  WWHD_FUNC(0x0233ABF8, BOOL, effect);
  if (!read<u32>(0x10469774)) {
    write<f32>(0x10469744, 0, 2.5f);
    write<f32>(0x10469744, 8, 1.f);
    write<f32>(0x10469744, 4, 2.5f);
    write<u32>(0x10469774, 0, 1);
  }
  u32 emitter = particle(effect, 0x2027, nullptr, gabi::at<cXyz>(0x10469744));
  if (!emitter)
    return 0;
  write<u32>(emitter, 0x5C, 1);
  write<f32>(emitter, 0x68, 15.f);
  write<f32>(emitter, 0x6C, 10.f);
  write<f32>(emitter, 0x34, 20.f);
  write<u16>(emitter, 0x64, 40);
  return 1;
}
VERIFY(0x0233ABF8, eff_barrelSet);
BOOL eff_stoolSet(Effect *effect) {
  WWHD_FUNC(0x0233AD0C, BOOL, effect);
  if (!read<u32>(0x10469778)) {
    write<f32>(0x10469750, 0, 2.5f);
    write<f32>(0x10469750, 8, 1.f);
    write<f32>(0x10469750, 4, 2.5f);
    write<u32>(0x10469778, 0, 1);
  }
  u32 emitter = particle(effect, 0x2027, nullptr, gabi::at<cXyz>(0x10469750));
  if (!emitter)
    return 0;
  write<u32>(emitter, 0x5C, 1);
  write<u16>(emitter, 0x64, 30);
  write<f32>(emitter, 0x6C, 10.f);
  write<f32>(emitter, 0x68, 15.f);
  write<f32>(emitter, 0x34, 15.f);
  return 1;
}
VERIFY(0x0233AD0C, eff_stoolSet);
BOOL eff_skullSet(Effect *effect) {
  WWHD_FUNC(0x0233AE18, BOOL, effect);
  if (!read<u32>(0x1046977C)) {
    write<f32>(0x1046975C, 0, 1.6f);
    write<f32>(0x1046975C, 8, 1.f);
    write<f32>(0x1046975C, 4, 1.6f);
    write<u32>(0x1046977C, 0, 1);
  }
  u32 emitter = particle(effect, 0x2027, nullptr, gabi::at<cXyz>(0x1046975C));
  if (!emitter)
    return 0;
  write<u32>(emitter, 0x5C, 1);
  write<f32>(emitter, 0x68, 8.f);
  write<f32>(emitter, 0x34, 10.f);
  return 1;
}
VERIFY(0x0233AE18, eff_skullSet);
BOOL eff_woodboxSet(Effect *effect) {
  WWHD_FUNC(0x0233B164, BOOL, effect);
  if (!read<u32>(0x104697A0)) {
    write<f32>(0x10469768, 0, 2.5f);
    write<f32>(0x10469768, 8, 1.f);
    write<f32>(0x10469768, 4, 2.5f);
    write<u32>(0x104697A0, 0, 1);
  }
  u32 emitter = particle(effect, 0x2027, nullptr, gabi::at<cXyz>(0x10469768));
  if (!emitter)
    return 0;
  write<u32>(emitter, 0x5C, 1);
  write<f32>(emitter, 0x34, 30.f);
  return 1;
}
VERIFY(0x0233B164, eff_woodboxSet);
BOOL eff_landSet(Effect *effect) {
  WWHD_FUNC(0x0233AF18, BOOL, effect);
  gabi::Local<cXyz> scale;
  *scale = effect->scale;
  u32 emitter = particle(effect, 0x2027, scale.get(), nullptr);
  if (!emitter)
    return 0;
  write<u16>(emitter, 0x64, 30);
  write<f32>(emitter, 0x68, 10.f);
  write<f32>(emitter, 0x34, 20.f);
  write<u32>(emitter, 0x5C, 1);
  write<u16>(emitter, 0x60, 35);
  if (!read<u32>(0x10469780)) {
    write<u32>(0x10469780, 0, 1);
    write<f32>(0x10469788, 4, 0.f);
    write<f32>(0x10469788, 8, 1.f);
    write<f32>(0x10469788, 0, 1.f);
  }
  write<f32>(emitter, 8, read<f32>(0x10469788));
  write<f32>(emitter, 0x10, read<f32>(0x10469788, 8));
  write<f32>(emitter, 0xC, read<f32>(0x10469788, 4));
  if (!read<u32>(0x10469784)) {
    write<f32>(0x10469794, 8, 0.f);
    write<f32>(0x10469794, 0, 0.f);
    write<u32>(0x10469784, 0, 1);
    write<f32>(0x10469794, 4, 5.f);
  }
  write<f32>(emitter, 0x1C, read<f32>(0x10469794, 8));
  write<f32>(emitter, 0x14, read<f32>(0x10469794));
  write<f32>(emitter, 0x18, read<f32>(0x10469794, 4));
  return 1;
}
VERIFY(0x0233AF18, eff_landSet);
BOOL eff_pineconeSet(Effect *effect) {
  WWHD_FUNC(0x0233B0E8, BOOL, effect);
  return particle(effect, 0xA16B, nullptr, nullptr) != 0;
}
VERIFY(0x0233B0E8, eff_pineconeSet);
void eff_barrelDelete(Effect *effect) {
  WWHD_FUNC(0x0233B258, void, effect);
  u32 callback = effect->callback;
  if (callback)
    gabi::call_ptr(read<u32>(read<u32>(callback), 0x44), ptr(callback));
}
VERIFY(0x0233B258, eff_barrelDelete);
void eff_stoolDelete(Effect *effect) {
  WWHD_FUNC(0x0233B274, void, effect);
  u32 callback = effect->callback;
  if (callback)
    gabi::call_ptr(read<u32>(read<u32>(callback), 0x44), ptr(callback));
}
VERIFY(0x0233B274, eff_stoolDelete);
void eff_skullDelete(Effect *effect) {
  WWHD_FUNC(0x0233B290, void, effect);
  u32 callback = effect->callback;
  if (callback)
    gabi::call_ptr(read<u32>(read<u32>(callback), 0x44), ptr(callback));
}
VERIFY(0x0233B290, eff_skullDelete);
void eff_landDelete(Effect *effect) {
  WWHD_FUNC(0x0233B2AC, void, effect);
  u32 callback = effect->callback;
  if (callback)
    gabi::call_ptr(read<u32>(read<u32>(callback), 0x44), ptr(callback));
}
VERIFY(0x0233B2AC, eff_landDelete);
void eff_pineconeDelete(Effect *effect) {
  WWHD_FUNC(0x0233B2C8, void, effect);
  u32 callback = effect->callback;
  if (callback)
    gabi::call_ptr(read<u32>(read<u32>(callback), 0x44), ptr(callback));
}
VERIFY(0x0233B2C8, eff_pineconeDelete);
void eff_woodboxDelete(Effect *effect) {
  WWHD_FUNC(0x0233B2E4, void, effect);
  u32 callback = effect->callback;
  if (callback)
    gabi::call_ptr(read<u32>(read<u32>(callback), 0x44), ptr(callback));
}
VERIFY(0x0233B2E4, eff_woodboxDelete);
u32 eff_barrelExecute(Effect *effect) {
  WWHD_FUNC(0x0233B300, u32, effect);
  if (read<u8>(effect->callback, 0x10) & 1)
    return gabi::call<u32>(0x025D57E0, effect); /* fopAcM_delete */
  return (u32)gabi::ea(effect); /* original: r3 still effect */
}
VERIFY(0x0233B300, eff_barrelExecute);
u32 eff_stoolExecute(Effect *effect) {
  WWHD_FUNC(0x0233B314, u32, effect);
  if (read<u8>(effect->callback, 0x10) & 1)
    return gabi::call<u32>(0x025D57E0, effect); /* fopAcM_delete */
  return (u32)gabi::ea(effect); /* original: r3 still effect */
}
VERIFY(0x0233B314, eff_stoolExecute);
u32 eff_skullExecute(Effect *effect) {
  WWHD_FUNC(0x0233B328, u32, effect);
  if (read<u8>(effect->callback, 0x10) & 1)
    return gabi::call<u32>(0x025D57E0, effect); /* fopAcM_delete */
  return (u32)gabi::ea(effect); /* original: r3 still effect */
}
VERIFY(0x0233B328, eff_skullExecute);
u32 eff_landExecute(Effect *effect) {
  WWHD_FUNC(0x0233B33C, u32, effect);
  if (read<u8>(effect->callback, 0x10) & 1)
    return gabi::call<u32>(0x025D57E0, effect); /* fopAcM_delete */
  return (u32)gabi::ea(effect); /* original: r3 still effect */
}
VERIFY(0x0233B33C, eff_landExecute);
u32 eff_pineconeExecute(Effect *effect) {
  WWHD_FUNC(0x0233B350, u32, effect);
  if (read<u8>(effect->callback, 0x10) & 1)
    return gabi::call<u32>(0x025D57E0, effect); /* fopAcM_delete */
  return (u32)gabi::ea(effect); /* original: r3 still effect */
}
VERIFY(0x0233B350, eff_pineconeExecute);
u32 eff_woodboxExecute(Effect *effect) {
  WWHD_FUNC(0x0233B364, u32, effect);
  if (read<u8>(effect->callback, 0x10) & 1)
    return gabi::call<u32>(0x025D57E0, effect); /* fopAcM_delete */
  return (u32)gabi::ea(effect); /* original: r3 still effect */
}
VERIFY(0x0233B364, eff_woodboxExecute);
s32 eff_createWrapper(Effect *effect) {
  WWHD_FUNC(0x0233B378, s32, effect);
  return gabi::call<s32>(0x0233A8A4, effect);
}
VERIFY(0x0233B378, eff_createWrapper);
s32 eff_deleteWrapper(Effect *effect) {
  WWHD_FUNC(0x0233B37C, s32, effect);
  return gabi::call<s32>(0x0233A9B4, effect);
}
VERIFY(0x0233B37C, eff_deleteWrapper);
s32 eff_executeWrapper(Effect *effect) {
  WWHD_FUNC(0x0233B380, s32, effect);
  return gabi::call<s32>(0x0233AA24, effect);
}
VERIFY(0x0233B380, eff_executeWrapper);
BOOL eff_isDelete(Effect *effect) {
  WWHD_FUNC(0x0233B384, BOOL, effect);
  return 1;
}
VERIFY(0x0233B384, eff_isDelete);
void eff_staticInit() {
  WWHD_FUNC(0x0233B38C, void);
  for (u32 i = 0; i < 4; i++)
    write<u32>(0x10469734, i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101C8F78));
  write<f32>(0x10469728, 0, read<f32>(0x10027BD8));
  write<f32>(0x1046972C, 0, read<f32>(0x10027BDC));
  gabi::call(0x028ED6F8, ptr(0x10469730));
  gabi::call(0x028F026C, ptr(0x101C8F84));
  gabi::call(0x028EAB2C, ptr(0x10469731));
  gabi::call(0x028F026C, ptr(0x101C8F90));
}
VERIFY(0x0233B38C, eff_staticInit);
void eff_callbackNoop(void *callback) { WWHD_FUNC(0x0233B420, void, callback); }
VERIFY(0x0233B420, eff_callbackNoop);
void eff_callbackDraw(void *callback, void *emitter) {
  WWHD_FUNC(0x0233B424, void, callback, emitter);
}
VERIFY(0x0233B424, eff_callbackDraw);
void eff_actorDestructor(Effect *effect, s32 flags) {
  WWHD_FUNC(0x0233B428, void, effect, flags);
  if (effect) {
    gabi::call(0x025D50BC, effect, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, effect);
  }
}
VERIFY(0x0233B428, eff_actorDestructor);
BOOL eff_draw(Effect *effect) {
  WWHD_FUNC(0x0233B47C, BOOL, effect);
  return 1;
}
VERIFY(0x0233B47C, eff_draw);
u32 eff_parameter(Effect *effect, u32 bits, u32 shift) {
  WWHD_FUNC(0x0233B484, u32, effect, bits, shift);
  u32 mask = (bits & 32) ? 0u : 1u << (bits & 31);
  u32 value =
      (shift & 32) ? 0u : read<u32>(gabi::ea(effect), 0xB0) >> (shift & 31);
  return value & (mask - 1);
}
VERIFY(0x0233B484, eff_parameter);
