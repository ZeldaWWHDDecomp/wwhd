/* Brown wooden tables: HD removes the GC attribute checks and simple shadow.
 * TU02380D28..0238149F includes heap callback, initialization and destructors.
 */
#include "d/actor/d_a_obj_otble.h"
#include "bindings.h"
using daObj_Otble::Act_c;
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
struct ArchiveName {
  be<u32> name, vt;
};
WWHD_SIZE(ArchiveName, 8);
u32 resource(s32 index) {
  gabi::Local<ArchiveName> archive;
  archive->name = 0x1002E394;
  archive->vt = 0x1002E32C;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(),
                         index);
}
} // namespace

BOOL otble_createHeap(Act_c *table) {
  WWHD_FUNC(0x02380D28, BOOL, table);
  u32 modelData = resource(read<s32>(0x1002E384, (u32)table->variant * 4));
  if (!modelData)
    gabi::call(0x0273AA24, ptr(0x1002E39C), 193, ptr(0x1002E3B0));
  table->tableModel =
      gabi::call<u32>(0x025E38E0, ptr(modelData), 0x80000, 0x11000022);
  if (!table->tableModel)
    return 0;
  mDoMtx_stack_c::transS(table->current.pos.x, table->current.pos.y,
                         table->current.pos.z);
  mDoMtx_stack_c::YrotM(table->current.angle.y);
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &table->backgroundMatrix);
  table->background = gabi::call<u32>(0x024F23F4, ptr(0));
  if (!table->background)
    return 0;
  u32 collision = resource(read<s32>(0x1002E38C, (u32)table->variant * 4));
  if (gabi::call<s32>(0x0200A030, ptr(table->background), ptr(collision), 1,
                      &table->backgroundMatrix))
    return 0;
  return 1;
}
VERIFY(0x02380D28, otble_createHeap);
BOOL otble_createHeapCallback(Act_c *table) {
  WWHD_FUNC(0x02380E70, BOOL, table);
  return otble_createHeap(table);
}
VERIFY(0x02380E70, otble_createHeapCallback);
void otble_setMatrix(Act_c *table) {
  WWHD_FUNC(0x02380E74, void, table);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024F08A8, table->acch, ptr(play, 0x12A0));
  table->tevStr.mRoomNo = table->current.roomNo;
  play = gabi::call<u32>(0x025200D4);
  u32 color = gabi::call<u32>(0x024EEEB8, ptr(play, 0x12A0),
                              ptr(gabi::ea(table), 0x4D8));
  write<u8>(gabi::ea(table), 0x1CA, color);
  J3DModel *model = gabi::at<J3DModel>((u32)table->tableModel);
  J3DModel_setBaseScale(model, &table->scale);
  mDoMtx_stack_c::transS(table->current.pos.x, table->current.pos.y,
                         table->current.pos.z);
  mDoMtx_stack_c::YrotM(table->current.angle.y);
  model = gabi::at<J3DModel>((u32)table->tableModel);
  J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &table->backgroundMatrix);
  gabi::call(0x024F43DC, ptr(table->background));
}
VERIFY(0x02380E74, otble_setMatrix);
void otble_createInit(Act_c *table) {
  WWHD_FUNC(0x02380F8C, void, table);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, ptr(play, 0x12A0), ptr(table->background), table);
  gabi::call(0x024EFF44, table->acchCircle, 30.f, 30.f);
  gabi::call(0x024F06B4, table->acch, &table->current.pos, &table->old.pos,
             table, 1, table->acchCircle, &table->speed, ptr(0), ptr(0));
  u32 a = gabi::ea(table);
  write<u32>(a, 0x418, read<u32>(a, 0x418) | 0x40C);
  table->gravity = -6.5f;
  gabi::call(0x025D6870, table, ptr(0));
  otble_setMatrix(table);
  u32 model = table->tableModel;
  table->cullMtx = model ? model + 0xC8 : 0;
  if (table->variant == 1)
    gabi::call(0x025D674C, table, -100.f, -0.f, -200.f, 100.f, 150.f, 200.f);
  else
    gabi::call(0x025D674C, table, -100.f, -0.f, -100.f, 100.f, 150.f, 100.f);
  table->cullSizeFar = 10.f;
}
VERIFY(0x02380F8C, otble_createInit);
BOOL otble_execute(Act_c *table) {
  WWHD_FUNC(0x023810CC, BOOL, table);
  gabi::call(0x025D6870, table, ptr(0));
  otble_setMatrix(table);
  return 1;
}
VERIFY(0x023810CC, otble_execute);
BOOL otble_draw(Act_c *table) {
  WWHD_FUNC(0x02381108, BOOL, table);
  auto light = dKy_getEnvlight();
  settingTevStruct(light, 0, &table->current.pos, &table->tevStr);
  light = dKy_getEnvlight();
  setLightTevColorType(light, gabi::at<J3DModel>((u32)table->tableModel),
                       &table->tevStr);
  u32 play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 4, read<u32>(play, 0x5D74));
  mDoExt_modelUpdateDL(gabi::at<J3DModel>((u32)table->tableModel), 0);
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 4, read<u32>(play, 0x5D7C));
  return 1;
}
VERIFY(0x02381108, otble_draw);
s32 otble_create(Act_c *table) {
  WWHD_FUNC(0x023811A0, s32, table);
  if (!read<u32>(0x101FDC40)) {
    write<u32>(0x101FDC40, 0, 1);
    memcpy_g(ptr(0x101FDC44), ptr(0x1002E318), 8);
  }
  if (!(table->actor_condition & 8)) {
    if (table) {
      fopAc_ac_c_ct(table);
      table->__vtbl = 0x1002E374;
      gabi::call(0x024F0474, table->acch);
      u32 a = gabi::ea(table);
      write<u32>(a, 0x400, 0x1002E344);
      write<u32>(a, 0x410, 0x1002E354);
      write<u32>(a, 0x404, 0x1002E364);
      write<u8>(a, 0x408, 1);
      gabi::call(0x024EFE94, table->acchCircle);
    }
    table->actor_condition = table->actor_condition | 8;
  }
  u8 variant = (u32)table->mParameters & 255;
  if (variant > 1)
    variant = 1;
  table->variant = variant;
  s32 phase = gabi::call<s32>(0x02520460, &table->phase, ptr(0x1002E324));
  if (phase == 4) {
    u32 heapSize = read<u32>(0x101FDC44, (u32)table->variant * 4);
    if (!gabi::call<s32>(0x025D63E8, table, ptr(0x02380E70), heapSize))
      return 5;
    otble_createInit(table);
  }
  return phase;
}
VERIFY(0x023811A0, otble_create);
BOOL otble_delete(Act_c *table) {
  WWHD_FUNC(0x023812FC, BOOL, table);
  gabi::call(0x025204C8, &table->phase, ptr(0x1002E3F0));
  u32 background = table->background;
  if (background && read<u32>(background) < 0x100) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(play, 0x12A0), ptr(table->background));
  }
  return 1;
}
VERIFY(0x023812FC, otble_delete);
BOOL otble_Execute(Act_c *table) {
  WWHD_FUNC(0x02381360, BOOL, table);
  return otble_execute(table);
}
VERIFY(0x02381360, otble_Execute);
BOOL otble_Draw(Act_c *table) {
  WWHD_FUNC(0x02381364, BOOL, table);
  return otble_draw(table);
}
VERIFY(0x02381364, otble_Draw);
void otble_staticInit() {
  WWHD_FUNC(0x02381368, void);
  for (u32 offset = 0; offset < 16; offset += 4)
    write<u32>(0x1046BB90, offset, 0);
  gabi::call(0x028F026C, ptr(0x101CC1D8));
  write<f32>(0x1046BB84, 0, -3.1415927410125732f);
  write<f32>(0x1046BB88, 0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046BB8C));
  gabi::call(0x028F026C, ptr(0x101CC1E4));
  gabi::call(0x028EAB2C, ptr(0x1046BB8D));
  gabi::call(0x028F026C, ptr(0x101CC1F0));
}
VERIFY(0x02381368, otble_staticInit);
void otble_staticDtor(void *object, u32 flags) {
  WWHD_FUNC(0x023813FC, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x023813FC, otble_staticDtor);
void otble_staticNoop(void *object) { WWHD_FUNC(0x02381410, void, object); }
VERIFY(0x02381410, otble_staticNoop);
void otble_destructor(Act_c *table, u32 flags) {
  WWHD_FUNC(0x02381414, void, table, flags);
  if (table) {
    u32 a = gabi::ea(table);
    gabi::call(0x02018034, ptr(a, 0x5C8), 2);
    write<u32>(a, 0x410, 0x1002E354);
    write<u32>(a, 0x404, 0x1002E364);
    gabi::call(0x024EFD9C, table->acch, 0);
    gabi::call(0x025D50BC, table, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, table);
  }
}
VERIFY(0x02381414, otble_destructor);
BOOL otble_IsDelete(Act_c *table) {
  WWHD_FUNC(0x02381498, BOOL, table);
  return 1;
}
VERIFY(0x02381498, otble_IsDelete);
