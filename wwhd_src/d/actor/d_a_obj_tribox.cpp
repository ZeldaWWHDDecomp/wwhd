/* Full HD Hyrule Castle triangle block actor; entry points audited against RPX.
 */
#include "d/actor/d_a_obj_tribox.h"
using daObjTribox::Act_c;
u32 parameter(Act_c *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x023AB3B4, u32, actor, width, shift);
  u32 bits = gabi::load<u32>(gabi::ea(actor) + 0xB0);
  u32 mask = (width & 32) ? 0u : (1u << (width & 31));
  return ((shift & 32) ? 0u : (bits >> (shift & 31))) & (mask - 1);
}
VERIFY(0x023AB3B4, parameter);
void set_state(Act_c *actor) {
  WWHD_FUNC(0x023A87EC, void, actor);
  u32 type = gabi::call<u32>(0x023AB3B4, actor, 1, 16);
  u32 sw = gabi::call<u32>(0x023AB3B4, actor, 8, 8);
  u32 save = gabi::load<u32>(0x101F84DC);
  s8 room = gabi::load<s8>(gabi::ea(actor) + 0x2FE);
  bool enabled =
      gabi::call<u32>(0x025BA0C0, gabi::at<u8>(save + 0x20), sw, room) != 0;
  actor->mState = type ? (enabled ? 3 : 2) : (enabled ? 1 : 0);
}
VERIFY(0x023A87EC, set_state);
void sound_pos_delete() {
  WWHD_FUNC(0x023A8B3C, void);
  gabi::call(0x025E1B34, gabi::at<cXyz>(0x1046C784));
}
VERIFY(0x023A8B3C, sound_pos_delete);
void controll_clear(Act_c *actor) {
  WWHD_FUNC(0x023A8B48, void, actor);
  u32 type = gabi::call<u32>(0x023AB3B4, actor, 1, 16);
  u32 address = type == 1 ? 0x1046C780 : 0x1046C77C;
  s32 count = (s32)(gabi::load<u32>(address) - 1);
  gabi::store<s32>(address, count);
  if (count < 0)
    gabi::call(0x0273AA24, STR(0x10031A40), type == 1 ? 0x214 : 0x217,
               STR(type == 1 ? 0x10031A18 : 0x10031A2C));
}
VERIFY(0x023A8B48, controll_clear);
void eff_smoke_remove(Act_c *actor) {
  WWHD_FUNC(0x023A8BD4, void, actor);
  u32 callback = gabi::ea(actor) + 0x538;
  u32 vt = gabi::load<u32>(callback);
  gabi::call(gabi::load<u32>(vt + 0x44), gabi::at<u8>(callback));
}
VERIFY(0x023A8BD4, eff_smoke_remove);
void eff_sink_smoke_remove(Act_c *actor) {
  WWHD_FUNC(0x023A8BE4, void, actor);
  for (u32 offset : {0x574u, 0x594u, 0x5B4u}) {
    u32 callback = gabi::ea(actor) + offset, vt = gabi::load<u32>(callback);
    gabi::call(gabi::load<u32>(vt + 0x44), gabi::at<u8>(callback));
  }
}
VERIFY(0x023A8BE4, eff_sink_smoke_remove);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x023AB3AC, BOOL, actor);
  return TRUE;
}
VERIFY(0x023AB3AC, IsDelete);
s32 create_block_after(Act_c *actor) {
  WWHD_FUNC(0x023AB39C, s32, actor);
  return 5;
}
VERIFY(0x023AB39C, create_block_after);
void mode_correct_end(Act_c *actor) { WWHD_FUNC(0x023AB3A4, void, actor); }
VERIFY(0x023AB3A4, mode_correct_end);
void mode_correct_dummy(Act_c *actor) { WWHD_FUNC(0x023AB3A8, void, actor); }
VERIFY(0x023AB3A8, mode_correct_dummy);
void emptyVirtual() { WWHD_FUNC(0x023AB344, void); }
VERIFY(0x023AB344, emptyVirtual);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x023AB330, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x023AB330, deleteStatic);
void destruct(Act_c *p, s32 flags) {
  WWHD_FUNC(0x023AB348, void, p, flags);
  if (p) {
    gabi::call(0x025D50BC, p, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, p);
  }
}
VERIFY(0x023AB348, destruct);
void mode_block_wait_init(Act_c *actor) {
  WWHD_FUNC(0x023A96A4, void, actor);
  u32 base = gabi::ea(actor), flags = gabi::load<u32>(base + 0x2E0);
  actor->mMode = 0;
  gabi::store<s16>(base + 0x51C, 0);
  gabi::store<u32>(base + 0x2E0, flags | 0x80);
}
VERIFY(0x023A96A4, mode_block_wait_init);
void mode_correct_on_init(Act_c *actor) {
  WWHD_FUNC(0x023A9814, void, actor);
  u32 base = gabi::ea(actor), flags = gabi::load<u32>(base + 0x2E0);
  actor->mMode = 4;
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
}
VERIFY(0x023A9814, mode_correct_on_init);
void mode_correct_dummy_init(Act_c *actor) {
  WWHD_FUNC(0x023A98B8, void, actor);
  u32 base = gabi::ea(actor), flags = gabi::load<u32>(base + 0x2E0);
  actor->mMode = 9;
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
}
VERIFY(0x023A98B8, mode_correct_dummy_init);

void eff_smoke_end(Act_c *a) {
  WWHD_FUNC(0x023AA0AC, void, a);
  gabi::call(0x025A5F88, gabi::at<u8>(gabi::ea(a) + 0x538));
}
VERIFY(0x023AA0AC, eff_smoke_end);
void eff_sink_smoke_init(Act_c *a) {
  WWHD_FUNC(0x023AA0B4, void, a);
  gabi::store<u8>(gabi::ea(a) + 0x5D4, 0);
}
VERIFY(0x023AA0B4, eff_sink_smoke_init);
void eff_sink_smoke_end(Act_c *a) {
  WWHD_FUNC(0x023AA194, void, a);
  u32 base = gabi::ea(a);
  if (gabi::load<u8>(base + 0x5D4)) {
    gabi::store<u8>(base + 0x5D4, 0);
    gabi::call(0x025A5F88, gabi::at<u8>(base + 0x574));
    gabi::call(0x025A5F88, gabi::at<u8>(base + 0x594));
    gabi::call(0x025A5F88, gabi::at<u8>(base + 0x5B4));
  }
}
VERIFY(0x023AA194, eff_sink_smoke_end);
void vib_sink_init(Act_c *a) {
  WWHD_FUNC(0x023AA1E8, void, a);
  gabi::store<u8>(gabi::ea(a) + 0x5D5, 0);
}
VERIFY(0x023AA1E8, vib_sink_init);
void mode_correct_demoreq_init(Act_c *a) {
  WWHD_FUNC(0x023AAE2C, void, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E0);
  a->mMode = 6;
  gabi::store<s16>(base + 0x4E8, 10);
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
}
VERIFY(0x023AAE2C, mode_correct_demoreq_init);
void mode_correct_end_init(Act_c *a) {
  WWHD_FUNC(0x023AAE4C, void, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E0);
  a->mMode = 8;
  gabi::store<u32>(base + 0x2E0, flags | 0x80);
}
VERIFY(0x023AAE4C, mode_correct_end_init);
u32 mode_correct_demorun_init(Act_c *a) {
  WWHD_FUNC(0x023AAF38, u32, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E0);
  a->mMode = 7;
  gabi::store<s16>(base + 0x4E8, 50);
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
  return gabi::call<u32>(0x025E1988, 0x806);
}
VERIFY(0x023AAF38, mode_correct_demorun_init);
void execute_correct(Act_c *a) {
  WWHD_FUNC(0x023A8EF0, void, a);
  gabi::call(0x023A8CF0, a);
}
VERIFY(0x023A8EF0, execute_correct);
BOOL solidHeapCB(Act_c *a) {
  WWHD_FUNC(0x023A952C, BOOL, a);
  return gabi::call<BOOL>(0x023A92BC, a);
}
VERIFY(0x023A952C, solidHeapCB);
void reset() {
  WWHD_FUNC(0x023AB3D0, void);
  gabi::store<u32>(0x1046C780, 0);
  gabi::store<u8>(0x1046C791, 0);
  f32 y = gabi::load<f32>(0x101FFBAC), x = gabi::load<f32>(0x101FFBA8);
  gabi::store<u32>(0x1046C778, 0);
  gabi::store<f32>(0x1046C788, y);
  f32 z = gabi::load<f32>(0x101FFBB0);
  gabi::store<u32>(0x1046C77C, 0);
  gabi::store<f32>(0x1046C78C, z);
  gabi::store<u8>(0x1046C790, 0);
  gabi::store<f32>(0x1046C784, x);
}
VERIFY(0x023AB3D0, reset);

void mode_correct_off_init(Act_c *a) {
  WWHD_FUNC(0x023AADBC, void, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E0);
  a->mMode = 5;
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
}
VERIFY(0x023AADBC, mode_correct_off_init);
void mode_block_lower_init(Act_c *a) {
  WWHD_FUNC(0x023AAB88, void, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E0);
  a->mMode = 3;
  gabi::store<s16>(base + 0x5D6, 94);
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
  gabi::store<s16>(base + 0x4E8, 50);
}
VERIFY(0x023AAB88, mode_block_lower_init);
void mode_block_sink_init(Act_c *a) {
  WWHD_FUNC(0x023AA564, void, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E0);
  gabi::store<s16>(base + 0x4E8, 0);
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
  a->mMode = 2;
  gabi::store<f32>(base + 0x56C, 0.0f);
  gabi::call(0x023AA0B4, a);
  gabi::call(0x023AA1E8, a);
}
VERIFY(0x023AA564, mode_block_sink_init);
s32 Mthd_Create(Act_c *a) {
  WWHD_FUNC(0x023AB0FC, s32, a);
  return gabi::call<s32>(0x023A8A18, a);
}
VERIFY(0x023AB0FC, Mthd_Create);
BOOL Mthd_Delete(Act_c *a) {
  WWHD_FUNC(0x023AB100, BOOL, a);
  return gabi::call<BOOL>(0x023A8C48, a);
}
VERIFY(0x023AB100, Mthd_Delete);
BOOL Mthd_Execute(Act_c *a) {
  WWHD_FUNC(0x023AB104, BOOL, a);
  return gabi::call<BOOL>(0x023A9090, a);
}
VERIFY(0x023AB104, Mthd_Execute);
BOOL Mthd_Draw(Act_c *a) {
  WWHD_FUNC(0x023AB108, BOOL, a);
  return gabi::call<BOOL>(0x023A9154, a);
}
VERIFY(0x023AB108, Mthd_Draw);

void sound_pos_init(Act_c *a) {
  WWHD_FUNC(0x023A887C, void, a);
  u32 base = gabi::ea(a);
  u8 first = gabi::load<u8>(base + 0x56A);
  gabi::Local<cXyz> scaled;
  gabi::call(0x0201AE48, gabi::at<cXyz>(base + 0x2EC), scaled.get(),
             0.3333333432674408f);
  if (first) {
    f32 z = scaled->z, y = scaled->y, x = scaled->x;
    gabi::store<f32>(0x1046C78C, z);
    gabi::store<f32>(0x1046C788, y);
    gabi::store<f32>(0x1046C784, x);
  } else
    gabi::call(0x028E8D88, gabi::at<cXyz>(0x1046C784), scaled.get(),
               gabi::at<cXyz>(0x1046C784));
}
VERIFY(0x023A887C, sound_pos_init);
static inline void copy_model_matrix(u32 model) {
  f32 matrix[12];
  for (u32 i = 0; i < 12; ++i)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
}
void set_mtx(Act_c *a) {
  WWHD_FUNC(0x023A8D3C, void, a);
  f32 x = a->current.pos.x, y = a->current.pos.y, z = a->current.pos.z;
  gabi::call(0x028E93CC, gabi::at<u8>(0x1048D0CC), x, y, z);
  s16 ax = a->shape_angle.x, ay = a->shape_angle.y, az = a->shape_angle.z;
  gabi::call(0x025F1B48, gabi::at<u8>(0x1048D0CC), ax, ay, az);
  copy_model_matrix(gabi::ea((J3DModel *)a->mBlockModel));
  f32 height = gabi::load<f32>(0x1048D0CC + 0x1C) + 251.0f;
  gabi::store<f32>(0x1048D0CC + 0x1C, height);
  copy_model_matrix(gabi::ea((J3DModel *)a->mLightModel));
}
VERIFY(0x023A8D3C, set_mtx);
void init_mtx(Act_c *a) {
  WWHD_FUNC(0x023A9530, void, a);
  u32 model = gabi::ea((J3DModel *)a->mBlockModel);
  f32 x = a->scale.x, y = a->scale.y, z = a->scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  model = gabi::ea((J3DModel *)a->mLightModel);
  x = a->scale.x;
  y = a->scale.y;
  z = a->scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  gabi::call(0x023A8D3C, a);
}
VERIFY(0x023A9530, init_mtx);

BOOL create_heap(Act_c *a) {
  WWHD_FUNC(0x023A92BC, BOOL, a);
  gabi::Local<SafeString> blockName;
  blockName->mStringTop = 0x10031C00;
  blockName->__vtbl = 0x100318A8;
  void *blockData =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         blockName.get(), 6);
  if (!blockData)
    gabi::call(0x0273AA24, STR(0x10031B0C), 0x140, STR(0x10031AAC));
  a->mBlockModel =
      gabi::call<J3DModel *>(0x025E38E0, blockData, 0x80000, 0x11000022);
  gabi::Local<SafeString> collisionName;
  collisionName->mStringTop = 0x10031C00;
  collisionName->__vtbl = 0x100318A8;
  void *collision =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         collisionName.get(), 16);
  if (!collision)
    gabi::call(0x0273AA24, STR(0x10031B0C), 0x149, STR(0x10031ABC));
  u32 model = gabi::ea((J3DModel *)a->mBlockModel);
  if (model)
    a->mBackground =
        gabi::call<u8 *>(0x024F2478, collision, 1, gabi::at<u8>(model + 0xC8));
  else
    a->mBackground = 0;
  gabi::Local<SafeString> lightName;
  lightName->mStringTop = 0x10031C00;
  lightName->__vtbl = 0x100318A8;
  void *lightData =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         lightName.get(), 9);
  if (!lightData)
    gabi::call(0x0273AA24, STR(0x10031B0C), 0x159, STR(0x10031ACC));
  a->mLightModel =
      gabi::call<J3DModel *>(0x025E38E0, lightData, 0x80000, 0x11000022);
  gabi::Local<SafeString> blockAnimationName;
  blockAnimationName->mStringTop = 0x10031C00;
  blockAnimationName->__vtbl = 0x100318A8;
  void *blockAnimation =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         blockAnimationName.get(), 12);
  if (!blockAnimation)
    gabi::call(0x0273AA24, STR(0x10031B0C), 0x163, STR(0x10031AE4));
  s32 blockOk = gabi::call<s32>(0x025E8154, a->mBlockBrk, lightData,
                                blockAnimation, 1, 0, 1.0f, 0, -1, 0, 0);
  gabi::Local<SafeString> lightAnimationName;
  lightAnimationName->mStringTop = 0x10031C00;
  lightAnimationName->__vtbl = 0x100318A8;
  void *lightAnimation =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         lightAnimationName.get(), 13);
  if (!lightAnimation)
    gabi::call(0x0273AA24, STR(0x10031B0C), 0x16D, STR(0x10031AF8));
  s32 lightOk = gabi::call<s32>(0x025E8154, a->mLightBrk, lightData,
                                lightAnimation, 1, 2, 1.0f, 0, -1, 0, 0);
  BOOL result =
      a->mBlockModel && a->mBackground && a->mLightModel && blockOk && lightOk;
  if (!result)
    a->mBackground = 0;
  return result;
}
VERIFY(0x023A92BC, create_heap);

static inline void shared_sound(Act_c *a, u32 id) {
  u32 base = gabi::ea(a);
  if (gabi::load<u8>(base + 0x56A)) {
    u32 map = gabi::call<u32>(0x023A8EF4, a, gabi::at<cXyz>(0x1046C784));
    s8 room = gabi::load<s8>(base + 0x326);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call(0x025E1A40, id, gabi::at<cXyz>(0x1046C784), map, reverb);
  }
}
void sound_flash_light(Act_c *a) {
  WWHD_FUNC(0x023A9018, void, a);
  shared_sound(a, 0x6223);
}
VERIFY(0x023A9018, sound_flash_light);
void sound_flash_shine(Act_c *a) {
  WWHD_FUNC(0x023AA4EC, void, a);
  shared_sound(a, 0x6A24);
}
VERIFY(0x023AA4EC, sound_flash_shine);
void sound_sink_down_block(Act_c *a) {
  WWHD_FUNC(0x023AA3FC, void, a);
  shared_sound(a, 0x6222);
}
VERIFY(0x023AA3FC, sound_sink_down_block);
void sound_sink_stop_block(Act_c *a) {
  WWHD_FUNC(0x023AA474, void, a);
  shared_sound(a, 0x6A2C);
}
VERIFY(0x023AA474, sound_sink_stop_block);
BOOL execute(Act_c *a) {
  WWHD_FUNC(0x023A9090, BOOL, a);
  u32 type = gabi::call<u32>(0x023AB3B4, a, 1, 16);
  gabi::call(type ? 0x023A8EF0 : 0x023A8E74, a);
  if (!a->mLightModel)
    return TRUE;
  u32 base = gabi::ea(a);
  u8 flash = gabi::load<u8>(base + 0x570);
  if (flash == 1) {
    if (gabi::call<s32>(0x025E742C, a->mBlockBrk)) {
      gabi::store<u8>(base + 0x570, 2);
      gabi::call(0x023A9018, a);
      return TRUE;
    }
  } else if (flash == 2)
    gabi::call(0x025E742C, a->mLightBrk);
  flash = gabi::load<u8>(base + 0x570);
  if (flash == 1 || flash == 2)
    gabi::call(0x023A9018, a);
  return TRUE;
}
VERIFY(0x023A9090, execute);

BOOL draw(Act_c *a) {
  WWHD_FUNC(0x023A9154, BOOL, a);
  u32 state = a->mState;
  if (state != 0 && state != 3)
    return TRUE;
  u32 env = gabi::call<u32>(0x02555D0C), base = gabi::ea(a);
  gabi::call(0x025626A4, gabi::at<u8>(env), 0, &a->current.pos,
             gabi::at<u8>(base + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(env), (J3DModel *)a->mBlockModel,
             gabi::at<u8>(base + 0x110));
  if (gabi::load<u8>(base + 0x570)) {
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, gabi::at<u8>(env), (J3DModel *)a->mLightModel,
               gabi::at<u8>(base + 0x110));
    u8 flash = gabi::load<u8>(base + 0x570);
    u32 data = gabi::load<u32>(gabi::ea((J3DModel *)a->mLightModel) + 0xAC);
    u32 animation = base + (flash == 1 ? 0x3BC : 0x434);
    f32 frame = gabi::load<f32>(animation + 4);
    gabi::call(0x025E83FC, gabi::at<u8>(animation), gabi::at<u8>(data), frame);
    if (gabi::load<u8>(base + 0x570)) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D58));
      play = gabi::call<u32>(0x025200D4);
      gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D60));
      gabi::call(0x025E2DE0, (J3DModel *)a->mLightModel, 0);
      play = gabi::call<u32>(0x025200D4);
      gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
      play = gabi::call<u32>(0x025200D4);
      gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
    }
  }
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  gabi::call(0x025E2DE0, (J3DModel *)a->mBlockModel, 0);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  return TRUE;
}
VERIFY(0x023A9154, draw);

s32 chk_light(Act_c *a) {
  WWHD_FUNC(0x023A97D4, s32, a);
  u32 save = gabi::load<u32>(0x101F84DC);
  return gabi::call<u32>(0x025B8B94, gabi::at<u8>(save + 0x644), 0x3820) ? 2
                                                                         : 0;
}
VERIFY(0x023A97D4, chk_light);
s32 create_block_before(Act_c *a) {
  WWHD_FUNC(0x023A9998, s32, a);
  s32 phase = gabi::call<s32>(0x02520460, &a->mPhase, STR(0x10031C00));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, a, 0x023A952C, 0x11C0))
      return 5;
    gabi::call(0x023A96C0, a);
  }
  return phase;
}
VERIFY(0x023A9998, create_block_before);
s32 create_correct_before(Act_c *a) {
  WWHD_FUNC(0x023A9A18, s32, a);
  gabi::call(0x023A982C, a);
  return 4;
}
VERIFY(0x023A9A18, create_correct_before);
s32 create_correct_after(Act_c *a) {
  WWHD_FUNC(0x023A9A3C, s32, a);
  s32 phase = gabi::call<s32>(0x02520460, &a->mPhase, STR(0x10031C00));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, a, 0x023A952C, 0x4000))
      return 5;
    gabi::call(0x023A98D0, a);
  }
  return phase;
}
VERIFY(0x023A9A3C, create_correct_after);
BOOL remove(Act_c *a) {
  WWHD_FUNC(0x023A8C48, BOOL, a);
  gabi::call(0x023A8B3C, a);
  gabi::call(0x023A8B48, a);
  gabi::call(0x023A8BD4, a);
  gabi::call(0x023A8BE4, a);
  s32 state = a->mState;
  if (state == 0 || state == 3) {
    u32 bg = gabi::ea((u8 *)a->mBackground);
    if (bg) {
      gabi::store<u32>(bg + 0xB4, 0);
      bg = gabi::ea((u8 *)a->mBackground);
      if (bg && gabi::load<u32>(bg) < 0x100) {
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::call(0x020087EC, gabi::at<u8>(play + 0x12A0),
                   (u8 *)a->mBackground);
      }
    }
    gabi::call(0x025204C8, &a->mPhase, STR(0x10031C00));
  }
  return TRUE;
}
VERIFY(0x023A8C48, remove);
static inline s32 dispatch_member(Act_c *a, u32 entry) {
  s16 virtualIndex = gabi::load<s16>(entry + 2), delta = gabi::load<s16>(entry);
  u32 object = gabi::ea(a) + (s32)delta, target;
  if (virtualIndex < 0)
    target = gabi::load<u32>(entry + 4);
  else {
    s16 vtableOffset = gabi::load<s16>(entry + 6);
    u32 vt = gabi::load<u32>(object + (s32)vtableOffset);
    target = gabi::load<u32>(vt + (u32)virtualIndex * 8 + 4);
  }
  return gabi::call<s32>(target, gabi::at<Act_c>(object));
}
void mode_proc_call(Act_c *a) {
  WWHD_FUNC(0x023A8CF0, void, a);
  dispatch_member(a, 0x10031A54 + (u32)a->mMode * 8);
}
VERIFY(0x023A8CF0, mode_proc_call);
s32 create(Act_c *a) {
  WWHD_FUNC(0x023A8A18, s32, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, a);
      gabi::store<u32>(base + 0xB4, 0x100319E0);
      gabi::call(0x025E80D0, a->mBlockBrk);
      gabi::call(0x025E80D0, a->mLightBrk);
      gabi::call(0x025A5B18, gabi::at<u8>(base + 0x538), 1);
      gabi::call(0x025A5C04, gabi::at<u8>(base + 0x574), 1, 1, 0, 0);
      gabi::call(0x025A5C04, gabi::at<u8>(base + 0x594), 1, 1, 0, 0);
      gabi::call(0x025A5C04, gabi::at<u8>(base + 0x5B4), 1, 1, 0, 0);
      flags = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  gabi::call(0x023A87EC, a);
  gabi::call(0x023A890C, a);
  return dispatch_member(a, 0x100319F8 + (u32)a->mState * 8);
}
VERIFY(0x023A8A18, create);

void controll_set(Act_c *a) {
  WWHD_FUNC(0x023A890C, void, a);
  u32 base = gabi::ea(a);
  if (gabi::load<u8>(base + 0xC))
    return;
  u32 type = gabi::call<u32>(0x023AB3B4, a, 1, 16);
  if (type == 1) {
    gabi::call(0x025E0D38, gabi::at<u8>(base + 0x68), -3, 2, 0xFFFD);
    u32 count = gabi::load<u32>(0x1046C780);
    if (!count) {
      gabi::store<u8>(base + 0x56A, 1);
      gabi::store<u8>(0x1046C791, 0);
      gabi::store<u32>(0x1046C778, 0);
      gabi::store<u8>(0x1046C790, 0);
    } else
      gabi::store<u8>(base + 0x56A, 0);
    count = gabi::load<u32>(0x1046C780);
    gabi::store<u32>(0x1046C780, count + 1);
    gabi::call(0x023A887C, a);
  } else {
    u32 count = gabi::load<u32>(0x1046C77C);
    gabi::store<u8>(base + 0x56A, count == 0);
    count = gabi::load<u32>(0x1046C77C);
    gabi::store<u32>(0x1046C77C, count + 1);
  }
}
VERIFY(0x023A890C, controll_set);
void correct_before_init(Act_c *a) {
  WWHD_FUNC(0x023A982C, void, a);
  u32 base = gabi::ea(a);
  gabi::call(0x025D6768, a, 0.0f, 125.0f, 0.0f, 200.0f);
  if (gabi::load<u8>(base + 0x56A)) {
    u32 play = gabi::call<u32>(0x025200D4);
    s16 event = gabi::call<s32>(0x02543F10, gabi::at<u8>(play + 0x52C4),
                                STR(0x10031B60), 255);
    gabi::store<s16>(base + 0x572, event);
  } else
    gabi::store<s16>(base + 0x572, -1);
  gabi::call(0x023A9814, a);
}
VERIFY(0x023A982C, correct_before_init);

BOOL line_cross(Act_c *a, const cXyz *start, const cXyz *end) {
  WWHD_FUNC(0x023A9ABC, BOOL, a, start, end);
  gabi::call(0x024F1AFC, gabi::at<u8>(0x1046C6F0), start, end, a);
  gabi::store<u32>(0x1046C6F8, gabi::load<u32>(gabi::ea(a) + 4));
  u32 play = gabi::call<u32>(0x025200D4);
  return gabi::call<BOOL>(0x02008860, gabi::at<u8>(play + 0x12A0),
                          gabi::at<u8>(0x1046C6F0));
}
VERIFY(0x023A9ABC, line_cross);
Act_c *search_block(Act_c *other, Act_c *self) {
  WWHD_FUNC(0x023AA354, Act_c *, other, self);
  if (!gabi::call<u32>(0x025D4604, other) || !other)
    return nullptr;
  if (gabi::load<s16>(gabi::ea(other) + 8) != 0x2C)
    return nullptr;
  if (gabi::call<u32>(0x023AB3B4, other, 1, 16) != 0)
    return nullptr;
  f32 distance = gabi::call<f32>(0x025D6924, self, other);
  return distance < 225.0f ? other : nullptr;
}
VERIFY(0x023AA354, search_block);
void execute_block(Act_c *a) {
  WWHD_FUNC(0x023A8E74, void, a);
  u32 base = gabi::ea(a);
  gabi::call(0x023A8CF0, a);
  u32 direction = gabi::load<u32>(base + 0x4F0),
      pull = gabi::load<u32>(base + 0x4F8);
  gabi::store<u32>(base + 0x500, direction);
  gabi::store<u32>(base + 0x508, pull);
  f32 y = a->current.pos.y;
  u32 input = gabi::load<u32>(base + 0x4EC);
  gabi::store<u8>(base + 0x4EC, 0);
  gabi::store<u32>(base + 0x4FC, input);
  f32 x = a->current.pos.x;
  gabi::store<f32>(base + 0x380, y);
  gabi::store<f32>(base + 0x37C, x);
  u32 sign = gabi::load<u32>(base + 0x4F4);
  f32 z = a->current.pos.z;
  gabi::store<u32>(base + 0x504, sign);
  gabi::store<f32>(base + 0x384, z);
  gabi::call(0x023A8D3C, a);
  gabi::call(0x024F43DC, (u8 *)a->mBackground);
}
VERIFY(0x023A8E74, execute_block);

BOOL chk_space(Act_c *a) {
  WWHD_FUNC(0x023A9DA0, BOOL, a);
  u32 direction = gabi::load<u32>(gabi::ea(a) + 0x518) == 1 ? 2 : 1;
  return gabi::call<u32>(0x023A9B18, a, direction) ^ 1;
}
VERIFY(0x023A9DA0, chk_space);
void eff_flash(Act_c *a) {
  WWHD_FUNC(0x023A9DD8, void, a);
  gabi::Local<cXyz> position;
  f32 x = a->current.pos.x, y = a->current.pos.y, z = a->current.pos.z;
  position->x = x;
  position->y = y + 251.0f;
  position->z = z;
  u32 play = gabi::call<u32>(0x025200D4),
      particles = gabi::load<u32>(play + 0x5AB0);
  gabi::call(0x025A847C, gabi::at<u8>(particles), 1, 0x833A, position.get(),
             &a->shape_angle, 0, 255, 0, -1, 0, 0, 0);
}
VERIFY(0x023A9DD8, eff_flash);
static inline void copy_vector(cXyz *dst, const cXyz *src) {
  for (u32 i = 0; i < 3; ++i)
    gabi::store<u32>(gabi::ea(dst) + i * 4,
                     gabi::load<u32>(gabi::ea(src) + i * 4));
}
void eff_smoke_pos(Act_c *a) {
  WWHD_FUNC(0x023A9E64, void, a);
  u32 base = gabi::ea(a);
  s16 sign = gabi::load<s16>(base + 0x534);
  u32 index = gabi::load<u32>(base + 0x530);
  s32 first = (s32)(index + (sign < 0 ? 2u : 1u)),
      second = (s32)(index + (sign < 0 ? 1u : 2u));
  first %= 3;
  second %= 3;
  s16 angle = a->shape_angle.y;
  gabi::call(0x025F1884, gabi::at<u8>(0x1048D0CC), angle);
  gabi::Local<cXyz> edge1, edge2, difference, normalized, offset, position;
  gabi::call(0x028E9044, gabi::at<u8>(0x1048D0CC),
             gabi::at<cXyz>(0x1046C6CC + (u32)first * 12), edge1.get());
  gabi::call(0x028E9044, gabi::at<u8>(0x1048D0CC),
             gabi::at<cXyz>(0x1046C6CC + (u32)second * 12), edge2.get());
  gabi::call(0x0201ADE0, edge2.get(), difference.get(), edge1.get());
  gabi::call(0x0201B3C0, difference.get(), normalized.get());
  gabi::call(0x028E8E64, difference.get(), difference.get(), 10.0f);
  gabi::call(0x0201AD78, edge1.get(), offset.get(), difference.get());
  gabi::call(0x0201AD78, offset.get(), position.get(), &a->current.pos);
  u32 x = gabi::load<u32>(gabi::ea(position.get())),
      y = gabi::load<u32>(gabi::ea(position.get()) + 4),
      z = gabi::load<u32>(gabi::ea(position.get()) + 8);
  gabi::store<s16>(base + 0x564, 0);
  gabi::store<u32>(base + 0x55C, y);
  gabi::store<u32>(base + 0x560, z);
  gabi::store<u32>(base + 0x558, x);
  s16 yaw = gabi::call<s16>(0x0200F93C, edge1.get(), edge2.get());
  gabi::store<s16>(base + 0x568, 0);
  gabi::store<s16>(base + 0x566, yaw);
}
VERIFY(0x023A9E64, eff_smoke_pos);

void correct_after_init(Act_c *a) {
  WWHD_FUNC(0x023A98D0, void, a);
  u32 base = gabi::ea(a);
  f32 homeY = gabi::load<f32>(base + 0x2F0),
      sinkHeight = gabi::load<f32>(0x1046C6A4);
  a->current.pos.y = homeY + sinkHeight;
  u32 model = gabi::ea((J3DModel *)a->mBlockModel);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x023A9530, a);
  gabi::call(0x025D6768, a, 0.0f, 125.0f, 0.0f, 200.0f);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, gabi::at<u8>(play + 0x12A0), (u8 *)a->mBackground, a);
  u32 bg = gabi::ea((u8 *)a->mBackground);
  gabi::store<u32>(bg + 0xA8, 0);
  gabi::call(0x024F43DC, (u8 *)a->mBackground);
  bg = gabi::ea((u8 *)a->mBackground);
  gabi::store<u32>(bg + 0xB4, 0);
  u8 light = gabi::call<s32>(0x023A97D4, a);
  gabi::store<u8>(base + 0x570, light);
  gabi::call(0x023A98B8, a);
}
VERIFY(0x023A98D0, correct_after_init);
void block_init(Act_c *a) {
  WWHD_FUNC(0x023A96C0, void, a);
  u32 base = gabi::ea(a), model = gabi::ea((J3DModel *)a->mBlockModel);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x023A9530, a);
  gabi::call(0x025D6768, a, 0.0f, 125.0f, 0.0f, 200.0f);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, gabi::at<u8>(play + 0x12A0), (u8 *)a->mBackground, a);
  u32 bg = gabi::ea((u8 *)a->mBackground);
  gabi::store<u32>(bg + 0xA8, 0x024EE708);
  gabi::call(0x024F43DC, (u8 *)a->mBackground);
  bg = gabi::ea((u8 *)a->mBackground);
  gabi::store<u32>(bg + 0xB4, 0x023A956C);
  gabi::store<u8>(0x1046C790, 0);
  f32 z = a->current.pos.z;
  gabi::store<u32>(base + 0x52C, 0);
  gabi::store<f32>(base + 0x528, z);
  f32 y = a->current.pos.y;
  gabi::store<u32>(base + 0x4F4, 0);
  gabi::store<f32>(base + 0x524, y);
  gabi::store<u32>(base + 0x4F0, 0);
  gabi::store<u32>(base + 0x530, 0);
  gabi::store<u8>(base + 0x4FC, 0);
  gabi::store<u32>(base + 0x518, 0);
  gabi::store<u8>(base + 0x4EC, 0);
  gabi::store<u32>(base + 0x510, 0);
  gabi::store<u32>(base + 0x500, 0);
  gabi::store<u8>(base + 0x50C, 0);
  gabi::store<u32>(base + 0x4F8, 0);
  gabi::store<u32>(base + 0x514, 0);
  gabi::store<u32>(base + 0x504, 0);
  gabi::store<u32>(base + 0x508, 0);
  gabi::store<s16>(base + 0x534, 1);
  f32 x = a->current.pos.x;
  gabi::store<u8>(base + 0x570, 0);
  gabi::store<f32>(base + 0x520, x);
  gabi::call(0x023A96A4, a);
}
VERIFY(0x023A96C0, block_init);
void eff_smoke_start(Act_c *a) {
  WWHD_FUNC(0x023A9FC0, void, a);
  u32 base = gabi::ea(a);
  if (!gabi::load<u32>(0x1046C6C8)) {
    gabi::store<u32>(0x1046C6C8, 1);
    gabi::store<f32>(0x1046C6BC, 0.6000000238418579f);
    gabi::store<f32>(0x1046C6C4, 0.6000000238418579f);
    gabi::store<f32>(0x1046C6C0, 0.6000000238418579f);
  }
  gabi::call(0x023A9E64, a);
  s8 room = gabi::load<s8>(base + 0x326);
  u32 play = gabi::call<u32>(0x025200D4),
      particles = gabi::load<u32>(play + 0x5AB0);
  u32 emitter =
      gabi::call<u32>(0x025A847C, gabi::at<u8>(particles), 2, 0x2022,
                      gabi::at<cXyz>(base + 0x558),
                      gabi::at<csXyz>(base + 0x564), gabi::at<cXyz>(0x1046C6BC),
                      0xB9, gabi::at<u8>(base + 0x538), room, 0, 0, 0);
  if (emitter) {
    gabi::store<f32>(emitter + 0x70, 15.0f);
    gabi::store<f32>(emitter + 0x58, 0.15000000596046448f);
    gabi::store<f32>(emitter + 0x34, 1.0f);
    gabi::store<s16>(emitter + 0x60, 30);
  }
}
VERIFY(0x023A9FC0, eff_smoke_start);

void staticInitialize() {
  WWHD_FUNC(0x023AB10C, void);
  gabi::store<u32>(0x1046C6B4, 0);
  gabi::store<u32>(0x1046C6AC, 0);
  gabi::store<u32>(0x1046C6B8, 0);
  gabi::store<u32>(0x1046C6B0, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD678));
  gabi::store<f32>(0x1046C690, -3.1415927410125732f);
  gabi::store<f32>(0x1046C694, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046C6A8));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD684));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046C6A9));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD690));
  gabi::store<f32>(0x1046C698, 125.0f);
  f32 height = gabi::call<f32>(0x028F4384, 3.0f) * 41.66666793823242f;
  gabi::store<f32>(0x1046C6A4, -245.0f);
  f32 twice = height + height, width = gabi::load<f32>(0x1046C698);
  gabi::store<f32>(0x1046C69C, height);
  gabi::store<f32>(0x1046C6CC, 0.0f);
  gabi::store<f32>(0x1046C6D0, 0.0f);
  gabi::store<f32>(0x1046C6D4, -twice);
  gabi::store<f32>(0x1046C6D8, -width);
  gabi::store<f32>(0x1046C6DC, 0.0f);
  gabi::store<f32>(0x1046C6E0, height);
  gabi::store<f32>(0x1046C6E4, width);
  gabi::store<f32>(0x1046C6E8, 0.0f);
  gabi::store<f32>(0x1046C6A0, twice);
  gabi::store<f32>(0x1046C6EC, height);
  gabi::call(0x02008FEC, gabi::at<u8>(0x1046C6F0));
  for (u32 i = 0x5D; i <= 0x60; ++i)
    gabi::store<u8>(0x1046C6F0 + i, 0);
  gabi::store<u32>(0x1046C6F0, 0x1046C748);
  gabi::store<u32>(0x1046C758, 1);
  gabi::store<u32>(0x1046C754, 0x100319C0);
  gabi::store<u32>(0x1046C6F4, 0x1046C754);
  gabi::store<u32>(0x1046C700, 0x100319A0);
  gabi::store<u8>(0x1046C751, 0);
  gabi::store<u8>(0x1046C752, 0);
  gabi::store<u32>(0x1046C710, 0x100319B0);
  gabi::store<u8>(0x1046C74C, 1);
  gabi::store<u32>(0x1046C748, 0x100319D0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD69C));
}
VERIFY(0x023AB10C, staticInitialize);
void lineStaticDestruct(void *p, s32 flags) {
  WWHD_FUNC(0x023AB2B8, void, p, flags);
  if (p) {
    u32 base = gabi::ea(p);
    gabi::store<u32>(base + 0x58, 0x10031990);
    gabi::store<u32>(base + 0x64, 0x100318D0);
    gabi::store<u32>(base + 0x20, 0x100318C0);
    gabi::call(0x02008B4C, p, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, p);
  }
}
VERIFY(0x023AB2B8, lineStaticDestruct);

void eff_sink_smoke_start(Act_c *a) {
  WWHD_FUNC(0x023AA0C0, void, a);
  u32 base = gabi::ea(a);
  if (gabi::load<u8>(base + 0x5D4))
    return;
  gabi::store<u8>(base + 0x5D4, 1);
  gabi::Local<csXyz> angles;
  for (u32 i = 0; i < 3; ++i) {
    s16 z = a->shape_angle.z, y = a->shape_angle.y, x = a->shape_angle.x;
    angles->z = z;
    angles->y = (s16)(y + i * 0x5555);
    angles->x = x;
    u32 callback = base + 0x574 + i * 0x20;
    u32 play = gabi::call<u32>(0x025200D4),
        particles = gabi::load<u32>(play + 0x5AB0);
    gabi::call(0x025A847C, gabi::at<u8>(particles), 2, 0xA320, &a->current.pos,
               angles.get(), 0, 0xA0, gabi::at<u8>(callback), -1, 0, 0, 0);
  }
}
VERIFY(0x023AA0C0, eff_sink_smoke_start);
static inline void vertical_direction(cXyz *p) {
  p->x = 0.0f;
  p->y = 1.0f;
  p->z = 0.0f;
}
void vib_sink_start(Act_c *a) {
  WWHD_FUNC(0x023AA1F4, void, a);
  u32 base = gabi::ea(a);
  if (gabi::load<u8>(base + 0x5D5))
    return;
  gabi::store<u8>(base + 0x5D5, 1);
  u32 play = gabi::call<u32>(0x025200D4), vibration = play + 0x599C;
  gabi::Local<cXyz> direction;
  vertical_direction(direction.get());
  gabi::call(0x025CB374, gabi::at<u8>(vibration), 2, 1, direction.get());
  vertical_direction(direction.get());
  gabi::call(0x025CB374, gabi::at<u8>(vibration), 1, 6, direction.get());
  vertical_direction(direction.get());
  gabi::call(0x025CB408, gabi::at<u8>(vibration), 3, 15, direction.get());
}
VERIFY(0x023AA1F4, vib_sink_start);
void vib_sink_end(Act_c *a) {
  WWHD_FUNC(0x023AA2D8, void, a);
  u32 base = gabi::ea(a);
  if (!gabi::load<u8>(base + 0x5D5))
    return;
  gabi::store<u8>(base + 0x5D5, 0);
  u32 play = gabi::call<u32>(0x025200D4), vibration = play + 0x599C;
  gabi::call(0x025CB610, gabi::at<u8>(vibration), -1);
  gabi::Local<cXyz> direction;
  vertical_direction(direction.get());
  gabi::call(0x025CB374, gabi::at<u8>(vibration), 3, 15, direction.get());
}
VERIFY(0x023AA2D8, vib_sink_end);
void mode_block_walk_init(Act_c *a) {
  WWHD_FUNC(0x023AA5B0, void, a);
  u32 base = gabi::ea(a), flags = gabi::load<u32>(base + 0x2E0);
  f32 x = a->current.pos.x, y = a->current.pos.y, z = a->current.pos.z;
  gabi::store<f32>(base + 0x520, x);
  gabi::store<f32>(base + 0x524, y);
  gabi::store<u32>(base + 0x2E0, flags & ~0x80u);
  gabi::store<f32>(base + 0x528, z);
  a->mMode = 1;
  gabi::store<s16>(base + 0x4E8, 20);
  gabi::call(0x023A9FC0, a);
  u32 play = gabi::call<u32>(0x025200D4),
      player = gabi::load<u32>(play + 0x5B2C),
      status = gabi::load<u32>(player + 0x3B8);
  gabi::store<u32>(player + 0x3B8, status | 0x800);
}
VERIFY(0x023AA5B0, mode_block_walk_init);

void mode_correct_demoreq(Act_c *a) {
  WWHD_FUNC(0x023AAF5C, void, a);
  u32 base = gabi::ea(a);
  s16 event = gabi::load<s16>(base + 0x572);
  u32 play = gabi::call<u32>(0x025200D4);
  if (!gabi::call<u32>(0x02544044, gabi::at<u8>(play + 0x52C4), event)) {
    gabi::call(0x023AAF38, a);
    return;
  }
  if (gabi::load<u16>(base + 0xF8) == 2) {
    gabi::store<u8>(base + 0x571, 1);
    gabi::call(0x023AAF38, a);
    return;
  }
  event = gabi::load<s16>(base + 0x572);
  gabi::call(0x025D7A58, a, event, 255, 0xFFFF, 0, 1);
  u16 flags = gabi::load<u16>(base + 0xFA);
  gabi::store<u16>(base + 0xFA, flags | 2);
}
VERIFY(0x023AAF5C, mode_correct_demoreq);
void mode_correct_demorun(Act_c *a) {
  WWHD_FUNC(0x023AB018, void, a);
  u32 base = gabi::ea(a);
  s16 timer = gabi::load<s16>(base + 0x4E8);
  if (timer > 0) {
    timer = (s16)(timer - 1);
    gabi::store<s16>(base + 0x4E8, timer);
    if (!timer) {
      u32 sw = gabi::call<u32>(0x023AB3B4, a, 8, 8),
          save = gabi::load<u32>(0x101F84DC);
      s8 room = gabi::load<s8>(base + 0x2FE);
      gabi::call(0x025B9E38, gabi::at<u8>(save + 0x20), sw, room);
      gabi::store<u8>(0x1046C791, 1);
    }
    return;
  }
  if (gabi::load<u8>(base + 0x571)) {
    s16 event = gabi::load<s16>(base + 0x572);
    u32 play = gabi::call<u32>(0x025200D4);
    if (!gabi::call<s32>(0x025440C8, gabi::at<u8>(play + 0x52C4), event))
      return;
    play = gabi::call<u32>(0x025200D4);
    u16 flags = gabi::load<u16>(play + 0x52B8);
    gabi::store<u16>(play + 0x52B8, flags | 8);
    gabi::store<u8>(base + 0x571, 0);
  }
  gabi::call(0x023AAE4C, a);
}
VERIFY(0x023AB018, mode_correct_demorun);
struct TriboxGroundCheck_l {
  u8 storage[0x54];
};
u32 sound_get_mapinfo(Act_c *a, const cXyz *position) {
  WWHD_FUNC(0x023A8EF4, u32, a, position);
  gabi::Local<TriboxGroundCheck_l> check;
  u32 object = gabi::ea(check.get());
  gabi::call(0x02008E0C, check.get());
  gabi::store<u32>(object, object + 0x40);
  gabi::store<u8>(object + 0x47, 0);
  gabi::store<u32>(object + 4, object + 0x4C);
  f32 x = position->x, y = position->y, z = position->z;
  gabi::store<u8>(object + 0x45, 0);
  gabi::store<u32>(object + 0x50, 1);
  gabi::store<u8>(object + 0x4A, 0);
  gabi::store<u8>(object + 0x46, 0);
  gabi::store<u32>(object + 0x10, 0x10031920);
  gabi::store<u8>(object + 0x44, 1);
  gabi::store<u8>(object + 0x48, 0);
  gabi::store<f32>(object + 0x2C, z);
  gabi::store<u32>(object + 0x20, 0x10031930);
  gabi::store<u32>(object + 0x40, 0x10031950);
  gabi::store<f32>(object + 0x28, y + 50.0f);
  gabi::store<u8>(object + 0x49, 0);
  gabi::store<f32>(object + 0x24, x);
  gabi::store<u32>(object + 0x4C, 0x10031940);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x02008974, gabi::at<u8>(play + 0x12A0), check.get());
  u32 material = 13;
  if (gabi::load<u16>(object + 0x16) < 0x100) {
    play = gabi::call<u32>(0x025200D4);
    material = gabi::call<u32>(0x024EECAC, gabi::at<u8>(play + 0x12A0),
                               gabi::at<u8>(object + 0x14));
  }
  gabi::store<u32>(object + 0x20, 0x100318F0);
  gabi::store<u32>(object + 0x40, 0x10031910);
  gabi::store<u32>(object + 0x4C, 0x100318D0);
  gabi::call(0x02008DAC, check.get(), 0);
  return material;
}
VERIFY(0x023A8EF4, sound_get_mapinfo);

void mode_correct_off(Act_c *a) {
  WWHD_FUNC(0x023AADD4, void, a);
  if (gabi::call<u32>(0x025D5218, 0x023AA354, a)) {
    u32 count = gabi::load<u32>(0x1046C778);
    gabi::store<u32>(0x1046C778, count + 1);
    gabi::call(0x023AADBC, a);
  }
}
VERIFY(0x023AADD4, mode_correct_off);
void mode_correct_on(Act_c *a) {
  WWHD_FUNC(0x023AAE64, void, a);
  u32 base = gabi::ea(a);
  if (!gabi::call<u32>(0x025D5218, 0x023AA354, a)) {
    u32 count = gabi::load<u32>(0x1046C778);
    gabi::store<u32>(0x1046C778, count - 1);
    gabi::call(0x023A9814, a);
    return;
  }
  u8 leader = gabi::load<u8>(base + 0x56A);
  if (leader && gabi::load<s32>(0x1046C778) >= 3) {
    gabi::store<u8>(0x1046C790, 1);
    leader = gabi::load<u8>(base + 0x56A);
  } else if (!gabi::load<u8>(0x1046C790))
    return;
  gabi::call(leader ? 0x023AAE2C : 0x023AAE4C, a);
}
VERIFY(0x023AAE64, mode_correct_on);
Act_c *push_pullCB(Act_c *a, Act_c *other, s16 pushAngle, u32 label) {
  WWHD_FUNC(0x023A956C, Act_c *, a, other, pushAngle, label);
  u32 mode = label & 3, base = gabi::ea(a);
  if (!mode)
    return a;
  if (mode == 3)
    gabi::call(0x0273AA24, STR(0x10031B28), 0x2B0, STR(0x10031B3C));
  s16 angle = a->shape_angle.y;
  gabi::store<u32>(base + 0x4F8, (mode & 1) ^ 1);
  s16 relative = (s16)((s32)pushAngle - 0x8000 - (s32)angle);
  s32 direction;
  if ((u32)((s32)relative + 0x2AAA) < 0x5554)
    direction = 0;
  else
    direction = relative < 0x2AAA ? 2 : 1;
  gabi::store<s32>(base + 0x4F0, direction);
  s16 actorAngle = gabi::call<s16>(0x025D6894, a, other);
  direction = gabi::load<s32>(base + 0x4F0);
  angle = a->shape_angle.y;
  s16 offset = gabi::load<s16>(0x10031B20 + (u32)direction * 2);
  s16 delta = (s16)((s32)angle + (s32)offset - (s32)actorAngle);
  gabi::store<u32>(base + 0x4F4, (u32)(s32)delta >> 31);
  gabi::store<u8>(base + 0x4EC, 1);
  return a;
}
VERIFY(0x023A956C, push_pullCB);

void mode_block_wait(Act_c *a) {
  WWHD_FUNC(0x023AA618, void, a);
  u32 base = gabi::ea(a);
  if (gabi::load<u8>(0x1046C790)) {
    gabi::store<s16>(base + 0x51C, 0);
    gabi::call(0x023AA564, a);
    return;
  }
  if (!gabi::load<u8>(base + 0x4EC) ||
      gabi::load<u32>(base + 0x4F0) != gabi::load<u32>(base + 0x500) ||
      gabi::load<u32>(base + 0x4F4) != gabi::load<u32>(base + 0x504) ||
      gabi::load<u32>(base + 0x4F8) != gabi::load<u32>(base + 0x508)) {
    gabi::store<s16>(base + 0x51C, 0);
    return;
  }
  s16 timer = (s16)(gabi::load<s16>(base + 0x51C) + 1);
  gabi::store<s16>(base + 0x51C, timer);
  if (timer < 4)
    return;
  u32 sign = gabi::load<u32>(base + 0x4F4), pull;
  bool positive = false;
  if (sign == 0 && gabi::load<u32>(base + 0x4F8) == 1)
    positive = true;
  else if (sign == 1 && gabi::load<u32>(base + 0x4F8) == 0)
    positive = true;
  gabi::store<s16>(base + 0x534, positive ? 1 : -1);
  sign = gabi::load<u32>(base + 0x4F4);
  u32 direction = gabi::load<u32>(base + 0x4F0), addition = sign ? 1 : 2;
  pull = gabi::load<u32>(base + 0x4F8);
  s32 combined = (s32)(direction + addition);
  s32 pivot = combined % 3;
  gabi::store<u32>(base + 0x518, pull);
  sign = gabi::load<u32>(base + 0x4F4);
  u32 input = gabi::load<u32>(base + 0x4EC);
  gabi::store<u32>(base + 0x50C, input);
  gabi::store<u32>(base + 0x514, sign);
  gabi::store<u32>(base + 0x510, direction);
  gabi::store<s32>(base + 0x530, pivot);
  if (gabi::call<s32>(0x023A9DA0, a))
    gabi::call(0x023AA5B0, a);
}
VERIFY(0x023AA618, mode_block_wait);
void mode_block_lower(Act_c *a) {
  WWHD_FUNC(0x023AAD18, void, a);
  u32 base = gabi::ea(a);
  s16 timer = gabi::load<s16>(base + 0x4E8);
  if (timer < 0) {
    gabi::store<s16>(base + 0x5D6, 94);
    return;
  }
  if (timer > 0) {
    timer = (s16)(timer - 1);
    gabi::store<s16>(base + 0x4E8, timer);
    if (timer) {
      gabi::store<s16>(base + 0x5D6, 94);
      return;
    }
    gabi::call(0x023A9DD8, a);
    timer = gabi::load<s16>(base + 0x4E8);
    gabi::store<u8>(base + 0x570, 1);
    if (timer) {
      gabi::store<s16>(base + 0x5D6, 94);
      return;
    }
  }
  s16 soundTimer = gabi::load<s16>(base + 0x5D6);
  if (soundTimer > 0) {
    soundTimer = (s16)(soundTimer - 1);
    gabi::store<s16>(base + 0x5D6, soundTimer);
    if (!soundTimer)
      gabi::call(0x023AA4EC, a);
  }
}
VERIFY(0x023AAD18, mode_block_lower);
void mode_block_sink(Act_c *a) {
  WWHD_FUNC(0x023AABB0, void, a);
  u32 base = gabi::ea(a);
  if (!gabi::load<u8>(0x1046C791)) {
    gabi::store<s16>(base + 0x4E8, 0);
    gabi::store<f32>(base + 0x56C, 0.0f);
    return;
  }
  gabi::call(0x023AA0C0, a);
  gabi::call(0x023AA1F4, a);
  f32 velocity = gabi::load<f32>(base + 0x56C) - 0.800000011920929f;
  f32 sinkHeight = gabi::load<f32>(0x1046C6A4);
  s16 timer = gabi::load<s16>(base + 0x4E8);
  f32 homeY = gabi::load<f32>(base + 0x2F0);
  gabi::store<f32>(base + 0x56C, velocity);
  f32 target = homeY + sinkHeight;
  bool first = (u32)((s32)timer - 5) < 7;
  f32 damping = first ? 0.5f
                      : ((u32)((s32)timer - 65) < 9 ? 0.4000000059604645f
                                                    : 0.8199999928474426f);
  velocity *= damping;
  f32 next = a->current.pos.y + velocity;
  gabi::store<f32>(base + 0x56C, velocity);
  // PPC bge tests the absence of LT, including unordered comparisons.
  bool moving = !(next < target);
  if (moving) {
    a->current.pos.y = next;
    gabi::call(0x023AA3FC, a);
    timer = gabi::load<s16>(base + 0x4E8);
    gabi::store<s16>(base + 0x4E8, (s16)(timer + 1));
    return;
  }
  a->current.pos.y = target;
  gabi::call(0x023AA194, a);
  gabi::call(0x023AA2D8, a);
  gabi::call(0x023AA474, a);
  gabi::call(0x023AAB88, a);
}
VERIFY(0x023AABB0, mode_block_sink);

BOOL chk_wall(Act_c *a, s32 kind) {
  WWHD_FUNC(0x023A9B18, BOOL, a, kind);
  u32 base = gabi::ea(a);
  if (kind != 1 && kind != 2)
    gabi::call(0x0273AA24, STR(0x10031B7C), 0x30C, STR(0x10031B90));
  s16 sign = gabi::load<s16>(base + 0x534);
  u32 pivot = gabi::load<u32>(base + 0x530);
  s32 next = (s32)(pivot + (sign < 0 ? 2u : 1u));
  next %= 3;
  u32 corner = 0x1046C6CC + (u32)next * 12;
  s16 angle = a->shape_angle.y;
  gabi::call(0x025F1884, gabi::at<u8>(0x1048D0CC), angle);
  gabi::Local<cXyz> origin, rotated, end, temporary, scaled, scratch;
  gabi::call(0x028E9044, gabi::at<u8>(0x1048D0CC), gabi::at<cXyz>(corner),
             rotated.get());
  f32 y = a->current.pos.y, x = a->current.pos.x, z = a->current.pos.z;
  origin->x = x;
  origin->y = y + 5.0f;
  origin->z = z;
  gabi::call(0x0201AE48, rotated.get(), scratch.get(), 0.9900000095367432f);
  gabi::call(0x0201ADE0, origin.get(), scaled.get(), scratch.get());
  copy_vector(end.get(), scaled.get());
  if (gabi::call<s32>(0x023A9ABC, a, origin.get(), end.get()))
    return TRUE;
  if (kind != 2)
    return FALSE;
  gabi::call(0x0201ADE0, origin.get(), temporary.get(), rotated.get());
  copy_vector(origin.get(), temporary.get());
  sign = gabi::load<s16>(base + 0x534);
  gabi::call(0x025F1C28, gabi::at<u8>(0x1048D0CC), sign < 0 ? -0x2AAA : 0x2AAA);
  gabi::call(0x028E9044, gabi::at<u8>(0x1048D0CC), gabi::at<cXyz>(corner),
             rotated.get());
  gabi::call(0x0201AE48, rotated.get(), scaled.get(), 0.49000000953674316f);
  gabi::call(0x0201ADE0, origin.get(), temporary.get(), scaled.get());
  copy_vector(end.get(), temporary.get());
  if (gabi::call<s32>(0x023A9ABC, a, origin.get(), end.get()))
    return TRUE;
  f32 high = origin->y;
  origin->y = high + 30.0f;
  gabi::call(0x0201AE48, rotated.get(), temporary.get(), 0.9900000095367432f);
  gabi::call(0x0201ADE0, origin.get(), scaled.get(), temporary.get());
  copy_vector(end.get(), scaled.get());
  return gabi::call<s32>(0x023A9ABC, a, origin.get(), end.get()) ? TRUE : FALSE;
}
VERIFY(0x023A9B18, chk_wall);

void mode_block_walk(Act_c *a) {
  WWHD_FUNC(0x023AA7A4, void, a);
  u32 base = gabi::ea(a);
  s16 timer = (s16)(gabi::load<s16>(base + 0x4E8) - 1);
  f32 phase = (f32)timer * 0.15707963705062866f;
  gabi::store<s16>(base + 0x4E8, timer);
  bool finished = timer <= 0;
  f64 cosine = gabi::call<f64>(0x028F4BE0, phase);
  s16 sign = gabi::load<s16>(base + 0x534);
  f32 progress = (f32)(cosine + 1.0);
  progress *= 0.5f;
  progress *= (f32)sign;
  f32 rotation = progress * 1.0471975803375244f;
  s32 step = gabi::load<s32>(base + 0x52C);
  s16 homeAngle = gabi::load<s16>(base + 0x2FA);
  f32 baseAngle = gabi::fmadds((f32)step, 1.0471975803375244f,
                               (f32)homeAngle * 9.58738019107841e-05f);
  gabi::call(0x028E98C0, gabi::at<u8>(0x1048D0CC), 0x59, baseAngle);
  u32 pivot = gabi::load<u32>(base + 0x530);
  gabi::Local<cXyz> before, after, sum, position;
  gabi::call(0x028E9044, gabi::at<u8>(0x1048D0CC),
             gabi::at<cXyz>(0x1046C6CC + pivot * 12), before.get());
  gabi::call(0x025F2590, rotation);
  pivot = gabi::load<u32>(base + 0x530);
  gabi::call(0x028E9044, gabi::at<u8>(0x1048D0CC),
             gabi::at<cXyz>(0x1046C6CC + pivot * 12), after.get());
  gabi::call(0x0201AD78, gabi::at<cXyz>(base + 0x520), sum.get(), before.get());
  gabi::call(0x0201ADE0, sum.get(), position.get(), after.get());
  f32 fullAngle = baseAngle + rotation;
  f32 angleUnits = gabi::fmadds(fullAngle, 10430.3779296875f, 0.5f);
  u32 x = gabi::load<u32>(gabi::ea(position.get())),
      y = gabi::load<u32>(gabi::ea(position.get()) + 4),
      z = gabi::load<u32>(gabi::ea(position.get()) + 8);
  gabi::store<u32>(base + 0x314, x);
  gabi::store<u32>(base + 0x318, y);
  gabi::store<u32>(base + 0x31C, z);
  a->shape_angle.y = (s16)gabi::ftoi(angleUnits);
  if (finished) {
    gabi::call(0x023AA0AC, a);
    u32 play = gabi::call<u32>(0x025200D4),
        player = gabi::load<u32>(play + 0x5B2C),
        flags = gabi::load<u32>(player + 0x3B8);
    gabi::store<u32>(player + 0x3B8, flags & ~0x800u);
    u32 old = gabi::load<u32>(base + 0x52C);
    sign = gabi::load<s16>(base + 0x534);
    s32 combined = (s32)(old + (u32)(s32)sign);
    gabi::store<s32>(base + 0x52C, combined % 6);
    if (gabi::call<s32>(0x023A9B18, a, 1)) {
      play = gabi::call<u32>(0x025200D4);
      u32 material = gabi::call<u32>(0x024EECAC, gabi::at<u8>(play + 0x12A0),
                                     gabi::at<u8>(0x1046C704));
      s8 room = gabi::load<s8>(base + 0x326);
      s32 reverb = gabi::call<s32>(0x02520540, room);
      gabi::call(0x025E1A40, 0x2823, gabi::at<cXyz>(base + 0x37C), material,
                 reverb);
    }
    gabi::call(0x023A96A4, a);
    return;
  }
  gabi::call(0x023A9E64, a);
  gabi::Local<TriboxGroundCheck_l> check;
  u32 object = gabi::ea(check.get());
  gabi::call(0x02008E0C, check.get());
  f32 currentY = a->current.pos.y, currentZ = a->current.pos.z,
      currentX = a->current.pos.x;
  for (u32 i = 0x45; i <= 0x49; ++i)
    gabi::store<u8>(object + i, 0);
  gabi::store<u32>(object + 0x10, 0x10031920);
  gabi::store<u32>(object + 0x50, 1);
  gabi::store<f32>(object + 0x28, currentY + 50.0f);
  gabi::store<u32>(object, object + 0x40);
  gabi::store<f32>(object + 0x2C, currentZ);
  gabi::store<u32>(object + 4, object + 0x4C);
  gabi::store<u32>(object + 0x4C, 0x10031940);
  gabi::store<u8>(object + 0x4A, 0);
  gabi::store<u32>(object + 0x40, 0x10031950);
  gabi::store<u32>(object + 0x20, 0x10031930);
  gabi::store<u8>(object + 0x44, 1);
  gabi::store<f32>(object + 0x24, currentX);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x02008974, gabi::at<u8>(play + 0x12A0), check.get());
  u32 material = 0;
  if (gabi::load<u16>(object + 0x16) < 0x100) {
    play = gabi::call<u32>(0x025200D4);
    material = gabi::call<u32>(0x024EECAC, gabi::at<u8>(play + 0x12A0),
                               gabi::at<u8>(object + 0x14));
  }
  s8 room = gabi::load<s8>(base + 0x326);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, 0x2022, gabi::at<cXyz>(base + 0x37C), material,
             reverb);
  gabi::store<u32>(object + 0x20, 0x100318F0);
  gabi::store<u32>(object + 0x40, 0x10031910);
  gabi::store<u32>(object + 0x4C, 0x100318D0);
  gabi::call(0x02008DAC, check.get(), 0);
}
VERIFY(0x023AA7A4, mode_block_walk);

/* ---- leftover functions of the translation unit ---- */

/* 023AB428 __sinit_d_a_obj_tribox_static_cpp (the static part's own initializer; ctor list 1018B3CC): the header statics only */
static void __sinit_d_a_obj_tribox_static_cpp() {
    WWHD_FUNC(0x023AB428, void);
    sinit_header_statics(0x1046C75C, 0x101CD6F8);
}
VERIFY(0x023AB428, __sinit_d_a_obj_tribox_static_cpp);
