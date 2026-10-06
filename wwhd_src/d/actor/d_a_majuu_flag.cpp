// Forsaken Fortress flag: WWHD cloth simulation and GX2 packet.
//
#include "bindings.h"
#include "d/actor/d_a_majuu_flag.h"
namespace {
template<class T> T read(u32 a, u32 offset = 0) { return gabi::load<T>(a + offset); }
template<class T> void write(u32 a, u32 offset, T value) { gabi::store<T>(a + offset, value); }
void* ptr(u32 a, u32 offset = 0) { return gabi::at<void>(a + offset); }
}
void majuu_copyMatrix(void* destination, void* source) {
  WWHD_FUNC(0x021B879C, void, destination, source);
  u32 dst = gabi::ea(destination), src = gabi::ea(source);
  f32 snapshot[12];
  for (u32 i = 0; i < 12; ++i) snapshot[i] = read<f32>(src, i * 4);
  for (u32 i = 0; i < 12; ++i) write<f32>(dst, i * 4, snapshot[i]);
}
VERIFY(0x021B879C, majuu_copyMatrix);
void majuu_colorS10(void* destination, void* source) {
  WWHD_FUNC(0x021B883C, void, destination, source);
  u32 dst = gabi::ea(destination), src = gabi::ea(source);
  f32 divisor = read<f32>(0x100143EC);
  f32 red = f32(read<s16>(src)) / divisor;
  f32 blue = f32(read<s16>(src, 4)) / divisor;
  f32 green = f32(read<s16>(src, 2)) / divisor;
  f32 alpha = f32(read<s16>(src, 6)) / divisor;
  write<f32>(dst, 0, red);
  write<f32>(dst, 4, green);
  write<f32>(dst, 8, blue);
  write<f32>(dst, 12, alpha);
}
VERIFY(0x021B883C, majuu_colorS10);
void majuu_colorU8(void* destination, void* source) {
  WWHD_FUNC(0x021B8900, void, destination, source);
  u32 dst = gabi::ea(destination), src = gabi::ea(source);
  u8 red = read<u8>(src), green = read<u8>(src, 1);
  f32 divisor = read<f32>(0x100143EC);
  u8 blue = read<u8>(src, 2), alpha = read<u8>(src, 3);
  f32 rgba[4] = {f32(red) / divisor, f32(green) / divisor,
                 f32(blue) / divisor, f32(alpha) / divisor};
  for (u32 i = 0; i < 4; ++i) write<f32>(dst, i * 4, rgba[i]);
}
VERIFY(0x021B8900, majuu_colorU8);
void majuu_setNormalMatrix(daMajuuFlagPacket_c* packet) {
  WWHD_FUNC(0x021B8D98, void, packet);
  s16 rotation = read<s16>(gabi::ea(packet), 0x1A28);
  u32 matrix = read<u32>(0x1018C7B0);
  gabi::call(0x025F1884, ptr(matrix), rotation);
}
VERIFY(0x021B8D98, majuu_setNormalMatrix);
void majuu_setBackNormals(daMajuuFlagPacket_c* packet) {
  WWHD_FUNC(0x021B9244, void, packet);
  u32 base = gabi::ea(packet), offset = read<u8>(base, 0x1A2E) * 252;
  u32 destination = base + 0x1734 + offset, source = base + 0x153C + offset;
  for (u32 i = 0; i < 21; ++i) {
    write<u32>(destination, 0, read<u32>(0x101FFBA8));
    write<u32>(destination, 4, read<u32>(0x101FFBAC));
    write<u32>(destination, 8, read<u32>(0x101FFBB0));
    gabi::call(0x028E8DAC, ptr(destination), ptr(source), ptr(destination));
    destination += 12;
    source += 12;
  }
}
VERIFY(0x021B9244, majuu_setBackNormals);
BOOL majuu_isDelete(daMajuuFlag_c* actor) {
  WWHD_FUNC(0x021B981C, BOOL, actor);
  return 1;
}
VERIFY(0x021B981C, majuu_isDelete);
void* majuu_uniformPairConstruct(void* object) {
  WWHD_FUNC(0x021BB174, void*, object);
  u32 a = gabi::ea(object);
  if (!a) a = gabi::call<u32>(0x0273AD10, 0x254);
  if (a) {
    gabi::call(0x027B5BD8, ptr(a, 4));
    gabi::call(0x027BF734, ptr(a, 0x158));
    write<u32>(a, 0x250, 0);
    write<u32>(a, 0x24C, 0);
  }
  return ptr(a);
}
VERIFY(0x021BB174, majuu_uniformPairConstruct);
void* majuu_allocateEmpty(void* object) {
  WWHD_FUNC(0x021BB1D0, void*, object);
  return object ? object : ptr(gabi::call<u32>(0x0273AD10, 16));
}
VERIFY(0x021BB1D0, majuu_allocateEmpty);
void majuu_deleteEmpty(void* object, u32 flags) {
  WWHD_FUNC(0x021BB1FC, void, object, flags);
  if (object && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x021BB1FC, majuu_deleteEmpty);
void majuu_uniformPairDestroy(void* object, u32 flags) {
  WWHD_FUNC(0x021BB210, void, object, flags);
  u32 a = gabi::ea(object);
  if (a) {
    gabi::call(0x027BF880, ptr(a, 0x158), 2);
    gabi::call(0x027B5CBC, ptr(a, 4), 2);
    if (flags & 1) gabi::call(0x0273AF40, object);
  }
}
VERIFY(0x021BB210, majuu_uniformPairDestroy);
void majuu_emptyPacketCallback(void* object) {
  WWHD_FUNC(0x021BB698, void, object);
}
VERIFY(0x021BB698, majuu_emptyPacketCallback);
void majuu_emptyActorCallback(void* object) {
  WWHD_FUNC(0x021BBACC, void, object);
}
VERIFY(0x021BBACC, majuu_emptyActorCallback);

BOOL majuu_draw(daMajuuFlag_c* actor) {
  WWHD_FUNC(0x021B8CF4, BOOL, actor);
  u32 a = gabi::ea(actor);
  gabi::Local<u8[48]> temporaryMatrix;
  gabi::call(0x028E9098, temporaryMatrix.get());
  gabi::call(0x028E9108, temporaryMatrix.get(), ptr(a, 0x1E04), ptr(a, 0x163C));
  u32 environment = gabi::call<u32>(0x02555D0C);
  u32 lighting = a + 0x110;
  gabi::call(0x025626A4, ptr(environment), 0, ptr(a, 0x314), ptr(lighting));
  if (read<u8>(a, 0x1E02) == 1) {
    u32 game = gabi::call<u32>(0x025200D4);
    lighting = read<u32>(game, 0x5B2C) + 0x110;
  }
  write<u32>(a, 0x166C, lighting);
  u32 drawBuffer = read<u32>(0x104B4634);
  gabi::call(0x027F0E04, ptr(drawBuffer), ptr(a, 0x3AC), 0);
  gabi::call(0x021B89B4, ptr(a, 0x3AC));
  return 1;
}
VERIFY(0x021B8CF4, majuu_draw);
BOOL majuu_delete(daMajuuFlag_c* actor) {
  WWHD_FUNC(0x021B9824, BOOL, actor);
  u32 a = gabi::ea(actor);
  gabi::call(0x025204C8, ptr(a, 0x1DDC), ptr(0x10014418));
  u8 texture = read<u8>(a, 0x1E01);
  if (texture == 1) gabi::call(0x025204C8, ptr(a, 0x1DE4), ptr(0x10014420));
  else if (texture == 2) gabi::call(0x025204C8, ptr(a, 0x1DE4), ptr(0x10014428));
  else if (texture == 3) gabi::call(0x025204C8, ptr(a, 0x1DE4), ptr(0x10014430));
  return 1;
}
VERIFY(0x021B9824, majuu_delete);
BOOL majuu_execute(daMajuuFlag_c* actor) {
  WWHD_FUNC(0x021B9730, BOOL, actor);
  u32 a = gabi::ea(actor);
  if (!read<u8>(0x101F4829)) {
    u32 wind = gabi::call<u32>(0x0257DAA8);
    f32 windX = read<f32>(wind), windZ = read<f32>(wind, 8);
    s32 target = gabi::call<s32>(0x020195B0, windX, windZ);
    u32 parent = read<u32>(a, 0x1E34);
    if (parent && read<u32>(a, 0x1E38)) {
      gabi::Local<cXyz> forward, transformed;
      forward->x = read<f32>(0x101FFBCC);
      forward->y = read<f32>(0x101FFBD0);
      forward->z = read<f32>(0x101FFBD4);
      gabi::call(0x028E90D4, ptr(parent), ptr(0x1048D0CC));
      gabi::call(0x028E9044, ptr(0x1048D0CC), forward.get(), transformed.get());
      f32 x = transformed->x, z = transformed->z;
      target = s16(u32(target) - u32(gabi::call<s32>(0x020195B0, x, z)));
    }
    gabi::call(0x0200F428, ptr(a, 0x322), target, 8, 0x400);
    gabi::call(0x021B92DC, actor);
    gabi::call(0x021B8520, actor);
  }
  return 1;
}
VERIFY(0x021B9730, majuu_execute);

void majuu_setMatrix(daMajuuFlag_c* actor) {
  WWHD_FUNC(0x021B8520, void, actor);
  u32 a = gabi::ea(actor), parent = read<u32>(a, 0x1E34);
  constexpr u32 hio = 0x10464EDC, stackMatrix = 0x1048D0CC;
  f32 zero = read<f32>(0x100143DC);
  if (parent && read<u32>(a, 0x1E38)) {
    gabi::call(0x028E90D4, ptr(parent), ptr(stackMatrix));
    u32 position = read<u32>(a, 0x1E38);
    f32 x = read<f32>(position), y = read<f32>(position, 4), z = read<f32>(position, 8);
    gabi::call(0x025F24E0, x, y, z);
    s16 angle = read<s16>(a, 0x322);
    gabi::call(0x025F1C28, ptr(stackMatrix), angle);
    f32 transZ = read<f32>(0x100143E0);
    gabi::call(0x025F24E0, zero, zero, transZ);
    f32 scale = read<f32>(a, 0x1DFC) * read<f32>(hio, 0x20);
    gabi::call(0x025F2518, scale, scale, scale);
    gabi::Local<cXyz> origin;
    origin->x = read<f32>(0x101FFBA8);
    origin->y = read<f32>(0x101FFBAC);
    origin->z = read<f32>(0x101FFBB0);
    gabi::call(0x028E8F64, ptr(stackMatrix), origin.get(), ptr(a, 0x314));
    gabi::call(0x028E90D4, ptr(stackMatrix), ptr(a, 0x1E04));
  } else {
    f32 x = read<f32>(a, 0x314), z = read<f32>(a, 0x31C), y = read<f32>(a, 0x318);
    gabi::call(0x0200FAD8, 0, x, y, z);
    u8 custom = read<u8>(hio, 0x24);
    u32 matrix = read<u32>(0x1018C7B0);
    if (custom) {
      f32 degrees = read<f32>(0x100143E4);
      s16 angle = s16(gabi::ftoi(read<f32>(hio, 0x30) * degrees));
      gabi::call(0x025F1C5C, ptr(matrix), angle);
      angle = s16(gabi::ftoi(read<f32>(hio, 0x28) * degrees));
      matrix = read<u32>(0x1018C7B0);
      gabi::call(0x025F1BF4, ptr(matrix), angle);
      angle = s16(gabi::ftoi(read<f32>(hio, 0x2C) * degrees));
      matrix = read<u32>(0x1018C7B0);
      gabi::call(0x025F1C28, ptr(matrix), angle);
    } else {
      s16 angle = read<s16>(a, 0x324);
      gabi::call(0x025F1C5C, ptr(matrix), angle);
      matrix = read<u32>(0x1018C7B0);
      angle = read<s16>(a, 0x320);
      gabi::call(0x025F1BF4, ptr(matrix), angle);
      matrix = read<u32>(0x1018C7B0);
      angle = read<s16>(a, 0x322);
      gabi::call(0x025F1C28, ptr(matrix), angle);
      if (read<u8>(a, 0x1E00) == 4 || read<u8>(a, 0x1E01)) {
        f32 transZ = read<f32>(0x100143E8);
        gabi::call(0x0200FAD8, 1, zero, zero, transZ);
      } else {
        matrix = read<u32>(0x1018C7B0);
        s16 tilt = read<s16>(hio, 0x36);
        gabi::call(0x025F1BF4, ptr(matrix), tilt);
        f32 transZ = read<f32>(hio, 0x38);
        gabi::call(0x0200FAD8, 1, zero, zero, transZ);
      }
    }
    f32 scale = read<f32>(a, 0x1DFC) * read<f32>(hio, 0x20);
    gabi::call(0x0200FC74, 1, scale, scale, scale);
    matrix = read<u32>(0x1018C7B0);
    gabi::call(0x028E90D4, ptr(matrix), ptr(a, 0x1E04));
  }
}
VERIFY(0x021B8520, majuu_setMatrix);
void majuu_staticInit() {
  WWHD_FUNC(0x021BB050, void);
  for (u32 offset : {8u, 0u, 12u, 4u}) write<u32>(0x10464ECC, offset, 0);
  gabi::call(0x028F026C, ptr(0x101B9A24));
  write<f32>(0x10464EC0, 0, read<f32>(0x10014508));
  write<f32>(0x10464EC4, 0, read<f32>(0x1001450C));
  gabi::call(0x028ED6F8, ptr(0x10464EC8));
  gabi::call(0x028F026C, ptr(0x101B9A30));
  gabi::call(0x028EAB2C, ptr(0x10464EC9));
  gabi::call(0x028F026C, ptr(0x101B9A3C));
  constexpr u32 hio = 0x10464EDC;
  f32 wind1 = read<f32>(0x10014510), wind2 = read<f32>(0x100144BC);
  f32 scale = read<f32>(0x10014408);
  write<f32>(hio, 0x10, wind1);
  f32 translation = read<f32>(0x10014514);
  write<f32>(hio, 0x20, scale);
  f32 gravity = read<f32>(0x10014518);
  write<f32>(hio, 0x38, translation);
  f32 spring = read<f32>(0x1001451C);
  write<f32>(hio, 0x14, wind2);
  write<f32>(hio, 0x18, gravity);
  write<f32>(hio, 0x1C, spring);
  write<u8>(hio, 4, 1);
  write<u8>(hio, 6, 0);
  write<u8>(hio, 5, 0);
  write<u8>(hio, 0x24, 0);
  write<u8>(hio, 7, 0);
  write<u32>(hio, 0, 0x100143CC);
  write<u32>(hio, 0xC, 0x400);
  write<s32>(hio, 0x34, -1500);
  write<u8>(hio, 8, 0);
}
VERIFY(0x021BB050, majuu_staticInit);
void* majuu_packetConstruct(daMajuuFlagPacket_c* object) {
  WWHD_FUNC(0x021B98B4, void*, object);
  u32 a = gabi::ea(object);
  if (!a) a = gabi::call<u32>(0x0273AD10, 0x1A30);
  if (!a) return ptr(a);
  gabi::call(0x027F1278, ptr(a));
  write<u32>(a, 0x98, 0);
  write<u32>(a, 0xC, 0x1001452C);
  u32 embedded = a + 0x9C;
  if (!embedded) embedded = gabi::call<u32>(0x0273AD10, 8);
  if (embedded) {
    write<u32>(embedded, 4, 0);
    write<u32>(embedded, 0, 0);
  }
  u32 pairs = a + 0xA4;
  if (!pairs) pairs = gabi::call<u32>(0x0273AD10, 0x968);
  if (pairs) {
    gabi::call(0x028EFFD0, ptr(pairs), 4, 0x254, 0x021BB174);
    write<u32>(pairs, 0x950, 0);
    write<u32>(pairs, 0x960, 0);
    write<u32>(pairs, 0x958, 32);
    write<u8>(pairs, 0x964, 0);
    write<u32>(pairs, 0x954, 0);
    for (u32 offset : {0u, 0x4A8u, 0x254u, 0x6FCu}) write<u32>(pairs, offset, 0);
  }
  gabi::call(0x027B5430, ptr(a, 0xA0C));
  gabi::call(0x027FD6F4, ptr(a, 0xA24));
  gabi::call(0x027FB40C, ptr(a, 0xA30));
  write<u32>(a, 0xA3C, 0x1016EF84);
  gabi::call(0x028F521C, ptr(a, 0xAA4), 0x34);
  if (u32(a + 0xAA4) == 0) gabi::call(0x0273AD10, 48);
  gabi::call(0x027FB40C, ptr(a, 0xAD8));
  write<u32>(a, 0xAE4, 0x1016EFB4);
  gabi::call(0x028F521C, ptr(a, 0xB4C), 0x2F0);
  f32 zero = read<f32>(0x10145180), one = read<f32>(0x1014517C);
  write<f32>(a, 0xB4C, zero);
  write<f32>(a, 0xB50, zero);
  write<f32>(a, 0xB54, zero);
  write<f32>(a, 0xB5C, zero);
  write<f32>(a, 0xB78, one);
  write<f32>(a, 0xB84, zero);
  write<f32>(a, 0xB68, one);
  write<f32>(a, 0xB64, zero);
  write<f32>(a, 0xBB4, zero);
  write<f32>(a, 0xBD4, zero);
  write<f32>(a, 0xBA4, zero);
  write<f32>(a, 0xBC8, one);
  write<f32>(a, 0xB7C, zero);
  write<f32>(a, 0xBA0, zero);
  write<f32>(a, 0xBD0, zero);
  write<f32>(a, 0xBC0, zero);
  write<f32>(a, 0xB90, zero);
  write<f32>(a, 0xB6C, zero);
  write<f32>(a, 0xB80, zero);
  write<f32>(a, 0xB88, one);
  write<f32>(a, 0xB70, zero);
  write<f32>(a, 0xB94, zero);
  write<f32>(a, 0xB60, zero);
  write<f32>(a, 0xBBC, zero);
  write<f32>(a, 0xBC4, zero);
  write<f32>(a, 0xBD8, one);
  write<f32>(a, 0xBA8, one);
  write<f32>(a, 0xBB8, one);
  write<f32>(a, 0xBB0, zero);
  write<f32>(a, 0xB9C, zero);
  write<f32>(a, 0xBCC, zero);
  write<f32>(a, 0xB74, zero);
  write<f32>(a, 0xBAC, zero);
  write<f32>(a, 0xB58, one);
  write<f32>(a, 0xB8C, zero);
  write<f32>(a, 0xB98, one);
  write<f32>(a, 0xBDC, zero);
  write<f32>(a, 0xBE0, zero);
  write<f32>(a, 0xBE4, zero);
  write<f32>(a, 0xBE8, one);
  write<f32>(a, 0xBEC, zero);
  write<f32>(a, 0xBF0, zero);
  write<f32>(a, 0xBF4, zero);
  write<f32>(a, 0xBF8, one);

  for (u32 offset : {0xBFCu, 0xC1Cu, 0xC3Cu})
    gabi::call(0x028EFFD0, ptr(a, offset), 2, 16, 0x021BB1D0);
  for (u32 offset = 0xC5C; offset <= 0xDAC; offset += 0x30)
    if (u32(a + offset) == 0) gabi::call(0x0273AD10, 48);
  for (u32 offset = 0xDDC; offset <= 0xE2C; offset += 0x10)
    if (u32(a + offset) == 0) gabi::call(0x0273AD10, 16);
  write<u8>(a, 0xE3C, 0);
  gabi::call(0x027BE6B8, ptr(a, 0xE40));
  gabi::call(0x027BE6B8, ptr(a, 0xED0));
  gabi::call(0x027BDF7C, ptr(a, 0xF60));
  gabi::call(0x027BDF7C, ptr(a, 0x10F8));
  write<u16>(a, 0x1A28, 0);
  write<u16>(a, 0x1A2A, 0);
  write<u8>(a, 0x1A2F, 1);
  write<u8>(a, 0x1A2E, 0);
  write<u16>(a, 0x1A2C, 0);
  return ptr(a);
}
VERIFY(0x021B98B4, majuu_packetConstruct);

namespace {
// The pointer is reloaded after heap lookup: a guest callback may update its owner.
void freeOwnedField(u32 owner, u32 offset) {
  u32 allocation = read<u32>(owner, offset);
  u32 manager = read<u32>(0x101F8B4C);
  u32 heap = gabi::call<u32>(0x02755FEC, ptr(manager), ptr(allocation));
  u32 vtable = read<u32>(heap, 0xC), target = read<u32>(vtable, 0x3C);
  allocation = read<u32>(owner, offset);
  gabi::call_ptr(target, ptr(heap), ptr(allocation));
}
void releaseUniformBuffer(u32 buffer) {
  gabi::call(0x027BF7E8, ptr(buffer, 0x158));
  u32 allocation = read<u32>(buffer, 0x250);
  write<u32>(buffer, 0, 0);
  if (allocation) {
    // GHS reads the length as well, although the deallocation only needs the pointer.
    (void)read<u32>(buffer, 0x24C);
    freeOwnedField(buffer, 0x250);
    write<u32>(buffer, 0x24C, 0);
    write<u32>(buffer, 0x250, 0);
  }
}
void releaseUniformPairs(u32 pairs) {
  for (u32 i = 0; i < 2; ++i) {
    u32 pair = pairs + i * 0x4A8;
    releaseUniformBuffer(pair);
    releaseUniformBuffer(pair + 0x254);
  }
}
void releasePacketResources(u32 packet) {
  u32 pairs = packet + 0xA4;
  releaseUniformPairs(pairs);
  write<u32>(pairs, 0x960, 0);
  for (s32 index = 0; index < read<s32>(packet, 0xA24); ++index) {
    u32 count = read<u32>(packet, 0xA24), buffers = read<u32>(packet, 0xA28);
    if (u32(index) < count) buffers += u32(index) * 0x23C;
    gabi::call(0x027BEBEC, ptr(buffers, 0x10));
    gabi::call(0x027BEBEC, ptr(buffers, 0x2C));
  }
  for (u32 offset : {0xA40u, 0xA5Cu, 0xAE8u, 0xB04u})
    gabi::call(0x027BEBEC, ptr(packet, offset));
  gabi::call(0x027BE2B0, ptr(packet, 0x10F8), 2);
  gabi::call(0x027BE2B0, ptr(packet, 0xF60), 2);
  gabi::call(0x027FB528, ptr(packet, 0xAD8), 0);
  gabi::call(0x027FB528, ptr(packet, 0xA30), 0);
  gabi::call(0x027FD764, ptr(packet, 0xA24), 2);
  gabi::call(0x027B54A0, ptr(packet, 0xA0C), 2);
  if (pairs) {
    releaseUniformPairs(pairs);
    write<u32>(pairs, 0x960, 0);
    gabi::call(0x028F0164, ptr(pairs), 4, 0x254, 0x021BB210, 0, 0);
  }
  u32 materials = read<u32>(packet, 0xA0);
  if (materials) {
    for (s32 index = 0; index < read<s32>(packet, 0x9C); ++index) {
      u32 material = materials + u32(index) * 20;
      if (material) {
        u32 objects = read<u32>(material, 8);
        write<u32>(material, 0, 0);
        if (objects) {
          for (s32 element = 0; element < read<s32>(material, 4); ++element) {
            u32 object = objects + u32(element) * 0xF4;
            u32 table = read<u32>(object, 0xF0), target = read<u32>(table, 0xC);
            gabi::call_ptr(target, ptr(object), 2);
            objects = read<u32>(material, 8);
          }
          freeOwnedField(material, 8);
          write<u32>(material, 4, 0);
          write<u32>(material, 8, 0);
        }
        objects = read<u32>(material, 0x10);
        if (objects) {
          for (s32 element = 0; element < read<s32>(material, 0xC); ++element) {
            u32 object = objects + u32(element) * 0xF4;
            u32 table = read<u32>(object, 0xF0), target = read<u32>(table, 0xC);
            gabi::call_ptr(target, ptr(object), 2);
            objects = read<u32>(material, 0x10);
          }
          freeOwnedField(material, 0x10);
          write<u32>(material, 0xC, 0);
          write<u32>(material, 0x10, 0);
        }
        materials = read<u32>(packet, 0xA0);
      }
    }
    freeOwnedField(packet, 0xA0);
    write<u32>(packet, 0x9C, 0);
    write<u32>(packet, 0xA0, 0);
  }
}
}
void majuu_packetDestroy(daMajuuFlagPacket_c* object, u32 flags) {
  WWHD_FUNC(0x021BB270, void, object, flags);
  u32 packet = gabi::ea(object);
  if (!packet) return;
  write<u32>(packet, 0xC, 0x1001452C);
  releasePacketResources(packet);
  gabi::call(0x027F13DC, object, 0);
  if (flags & 1) gabi::call(0x0273AF40, object);
}
VERIFY(0x021BB270, majuu_packetDestroy);
void majuu_actorDestroy(daMajuuFlag_c* actor, u32 flags) {
  WWHD_FUNC(0x021BB69C, void, actor, flags);
  u32 a = gabi::ea(actor);
  if (!a) return;
  write<u32>(a, 0x3B8, 0x1001452C);
  releasePacketResources(a + 0x3AC);
  gabi::call(0x027F13DC, ptr(a, 0x3AC), 0);
  gabi::call(0x025D50BC, actor, 0);
  if (flags & 1) gabi::call(0x0273AF40, actor);
}
VERIFY(0x021BB69C, majuu_actorDestroy);

namespace {
void copyVectorWords(u32 destination, u32 source) {
  u32 x = read<u32>(source), y = read<u32>(source, 4), z = read<u32>(source, 8);
  write<u32>(destination, 0, x);
  write<u32>(destination, 4, y);
  write<u32>(destination, 8, z);
}
}
void majuu_setNormalVertex(daMajuuFlagPacket_c* packet, cXyz* output, s32 vertex) {
  WWHD_FUNC(0x021B8DA8, void, packet, output, vertex);
  u32 base = gabi::ea(packet);
  u32 positions = base + 0x1344 + read<u8>(base, 0x1A2E) * 252;
  gabi::Local<cXyz> temporary, cross, sum, center, first, second, normalized;
  u32 position = positions + u32(vertex) * 12;
  write<f32>(gabi::ea(center.get()), 0, read<f32>(position));
  write<f32>(gabi::ea(center.get()), 4, read<f32>(position, 4));
  write<f32>(gabi::ea(center.get()), 8, read<f32>(position, 8));
  f32 zero = read<f32>(0x100143DC);
  write<f32>(gabi::ea(sum.get()), 0, zero);
  write<f32>(gabi::ea(sum.get()), 4, zero);
  write<f32>(gabi::ea(sum.get()), 8, zero);
  u32 neighbors = 0x101B97D8 + u32(vertex) * 28;
  for (u32 face = 0; face < 6; ++face) {
    if (read<s32>(neighbors, 4) == -1) break;
    gabi::call(0x0201ADE0, ptr(positions + read<u32>(neighbors) * 12), temporary.get(), center.get());
    copyVectorWords(gabi::ea(first.get()), gabi::ea(temporary.get()));
    neighbors += 4;
    gabi::call(0x0201ADE0, ptr(positions + read<u32>(neighbors) * 12), temporary.get(), center.get());
    copyVectorWords(gabi::ea(second.get()), gabi::ea(temporary.get()));
    gabi::call(0x0201B080, second.get(), temporary.get(), first.get());
    copyVectorWords(gabi::ea(cross.get()), gabi::ea(temporary.get()));
    gabi::call(0x0201B1E4, cross.get(), temporary.get());
    copyVectorWords(gabi::ea(cross.get()), gabi::ea(temporary.get()));
    gabi::call(0x028E8D88, sum.get(), cross.get(), sum.get());
  }
  gabi::call(0x0201B1E4, sum.get(), normalized.get());
  copyVectorWords(gabi::ea(sum.get()), gabi::ea(normalized.get()));
  gabi::call(0x0200FCF0);
  // The triangular mesh row changes the phase origin for each row.
  u32 column = vertex == 0 ? 0 : vertex < 3 ? u32(vertex) : vertex < 6 ? u32(vertex) - 1 :
               vertex < 10 ? u32(vertex) - 3 : vertex < 15 ? u32(vertex) - 6 : u32(vertex) - 10;
  u16 phase = u16(column * u32(-800));
  f32 strength = read<f32>(0x10014400);
  f32 sine = read<f32>(0x104A44F8 + (u32(phase) >> 3) * 8);
  s16 angle = s16(gabi::ftoi(strength * sine));
  gabi::call(0x025F1C28, ptr(read<u32>(0x1018C7B0)), angle);
  gabi::call(0x0200FCD8, sum.get(), cross.get());
  gabi::call(0x0201B1E4, cross.get(), normalized.get());
  copyVectorWords(gabi::ea(output), gabi::ea(normalized.get()));
  gabi::call(0x0200FD38);
}
VERIFY(0x021B8DA8, majuu_setNormalVertex);

void majuu_updateUniforms(daMajuuFlagPacket_c* packet) {
  WWHD_FUNC(0x021B89B4, void, packet);
  u32 p = gabi::ea(packet), pairs = p + 0xA4;
  for (u32 side = 0; side < 2; ++side) {
    u32 current = read<u32>(pairs, 0x950);
    u32 destination = read<u32>(pairs + (side * 2 + current) * 0x254) - 0x20;
    u32 triangles = 0x101B94DC - 3;
    u32 normals = p + (side == 0 ? 0x153C : 0x1734);
    for (u32 vertex = 0; vertex < 35; ++vertex) {
      u32 index = read<u8>(p, 0x1A2E) * 21;
      triangles += 3;
      index += read<u8>(triangles);
      u32 position = p + 0x1344 + index * 12;
      f32 z = read<f32>(position, 8), x = read<f32>(position), y = read<f32>(position, 4);
      destination += 0x20;
      write<f32>(destination, 0, x);
      if (!side) {
        write<f32>(destination, 4, y);
        write<f32>(destination, 8, z);
      } else {
        write<f32>(destination, 8, z);
        write<f32>(destination, 4, y);
      }
      index = read<u8>(p, 0x1A2E) * 21 + read<u8>(triangles, 1);
      u32 normal = normals + index * 12;
      f32 nx = read<f32>(normal);
      f32 nz = read<f32>(normal, 8), ny = read<f32>(normal, 4);
      write<f32>(destination, 0x14, nz);
      write<f32>(destination, 0xC, nx);
      write<f32>(destination, 0x10, ny);
    }
  }
  u32 current = read<u32>(pairs, 0x950);
  u32 uniform = pairs + current * 0x254 + 4;
  for (u32 side = 0; side < 2; ++side) {
    u32 size = read<u32>(uniform, 0x14C);
    gabi::call(0x027B5E94, ptr(uniform), 0, size);
    uniform += 0x4A8;
  }
  current = read<u32>(pairs, 0x950);
  write<u32>(pairs, 0x950, current == 0 ? 1 : 0);
  gabi::call(0x0255F8F4, ptr(read<u32>(p, 0x12C0)));
  gabi::call(0x0255FE90, ptr(read<u32>(p, 0x12C0)));
  gabi::Local<u8[48]> view;
  gabi::call(0x021B879C, view.get(), ptr(0x104B45F8));
  u32 environment = read<u32>(0x104B4708);
  gabi::call(0x027FDA54, ptr(p, 0xA24), 0, view.get(), ptr(0x104B470C), ptr(environment, 0x240));
  gabi::Local<u8[16]> color, scaled;
  u32 lighting = read<u32>(p, 0x12C0), buffers = read<u32>(p, 0xA28);
  gabi::call(0x021B883C, color.get(), ptr(lighting, 0x90));
  f32 scale = read<f32>(read<u32>(p, 0x12C0), 0x28);
  gabi::call(0x0274D458, scaled.get(), color.get(), scale);
  for (u32 i = 0; i < 4; ++i) write<u32>(buffers, 0x1C4 + i * 4, read<u32>(gabi::ea(scaled.get()), i * 4));
  lighting = read<u32>(p, 0x12C0);
  buffers = read<u32>(p, 0xA28);
  gabi::call(0x021B883C, color.get(), ptr(lighting, 0x160));
  scale = read<f32>(read<u32>(p, 0x12C0), 0x16C);
  gabi::Local<u8[16]> secondary;
  gabi::call(0x0274D458, secondary.get(), color.get(), scale);
  for (u32 i = 0; i < 4; ++i) write<u32>(buffers, 0x1D4 + i * 4, read<u32>(gabi::ea(secondary.get()), i * 4));
  gabi::call(0x027FDFF4, ptr(p, 0xA24), 0);
  lighting = read<u32>(p, 0x12C0);
  gabi::call(0x021B883C, ptr(p, 0xB8C), ptr(lighting, 0x90));
  gabi::call(0x021B8900, ptr(p, 0xB9C), ptr(lighting, 0x98));
  scale = read<f32>(read<u32>(p, 0x12C0), 0x24);
  gabi::call(0x0274D2AC, ptr(p, 0xB9C), scale);
  if (read<u8>(lighting, 0x9F)) {
    gabi::call(0x021B8900, ptr(p, 0xBAC), ptr(lighting, 0x9C));
  } else {
    f32 zero = read<f32>(0x100143DC);
    write<f32>(p, 0xBB0, zero);
    write<f32>(p, 0xBAC, zero);
    write<f32>(p, 0xBB4, zero);
    write<f32>(p, 0xBB8, zero);
  }
  gabi::call(0x027FB678, ptr(p, 0xAD8));
  gabi::Local<u8[48]> model;
  gabi::call(0x021B879C, model.get(), ptr(p, 0x1290));
  gabi::call(0x028E90D4, model.get(), ptr(p, 0xAA4));
  gabi::call(0x027FB678, ptr(p, 0xA30));
}
VERIFY(0x021B89B4, majuu_updateUniforms);

void majuu_clothMove(daMajuuFlag_c* actor) {
  WWHD_FUNC(0x021B92DC, void, actor);
  u32 a = gabi::ea(actor);
  gabi::call(0x0257DAA8);
  u32 hio = 0x10464EDC;
  u8 previous = read<u8>(a, 0x1DDA), current = previous ^ 1;
  s16 oldPhase = read<s16>(a, 0x1DF4);
  write<u8>(a, 0x1DDA, current);
  u32 phase = u32(oldPhase) + read<u32>(hio, 0xC);
  f32 amplitude = read<f32>(0x10014404);
  write<u16>(a, 0x1DF4, u16(phase));
  f32 sine = read<f32>(0x104A44F8 + ((phase & 0xFFFF) >> 3) * 8);
  f32 one = read<f32>(0x10014408);
  f32 blend = gabi::fmadds(amplitude, sine, amplitude);
  f32 secondWind = read<f32>(hio, 0x14), complement = one - blend;
  f32 firstWind = read<f32>(hio, 0x10);
  f32 zero = read<f32>(0x100143DC);
  f32 windZ = gabi::fmadds(firstWind, blend, secondWind * complement);
  u32 positions = a + 0x16F0;
  u32 source = positions + previous * 252, destination = positions + current * 252;
  u32 oldNormals = a + 0x18E8 + previous * 252, velocities = a + 0x1CD8;
  gabi::Local<cXyz> wind, transformedWind, center, bias, scratch, difference, normalized, sum;
  wind->x = zero; wind->y = zero; wind->z = windZ;
  f32 strength = gabi::call<f32>(0x02578348);
  gabi::call(0x028E8E64, wind.get(), wind.get(), strength + strength);
  s16 angle = s16(-s32(read<s16>(a, 0x324)));
  gabi::call(0x025F181C, ptr(read<u32>(0x1018C7B0)), angle);
  angle = s16(-s32(read<s16>(a, 0x320)));
  gabi::call(0x025F1BF4, ptr(read<u32>(0x1018C7B0)), angle);
  gabi::call(0x0200FCD8, wind.get(), transformedWind.get());
  f32 damping = read<f32>(0x1001440C);
  f32 longRest = read<f32>(0x10014410), shortRest = read<f32>(0x10014414);
  for (u32 vertex = 0; vertex < 21; ++vertex) {
    u32 offset = vertex * 12, input = source + offset, output = destination + offset;
    // Preserve the original per-word copy; current and previous buffers may overlap in diagnostics.
    write<u32>(output, 0, read<u32>(input));
    write<u32>(output, 4, read<u32>(input, 4));
    write<u32>(output, 8, read<u32>(input, 8));
    center->x = read<f32>(input); center->y = read<f32>(input, 4); center->z = read<f32>(input, 8);
    gabi::call(0x028E8F44, transformedWind.get(), ptr(oldNormals + offset));
    u32 velocity = velocities + offset;
    if (vertex == 15 || vertex == 20) {
      bias->z = read<f32>(0x101FFBB0);
      bias->x = read<f32>(0x101FFBA8);
      bias->y = read<f32>(0x101FFBAC);
      gabi::call(0x028E8D88, ptr(velocity), bias.get(), ptr(velocity));
      gabi::call(0x028E8E64, ptr(velocity), ptr(velocity), damping);
      gabi::call(0x028E8D88, ptr(output), ptr(velocity), ptr(output));
      continue;
    }
    gabi::call(0x0201AE48, ptr(oldNormals + offset), scratch.get());
    f32 gravity = read<f32>(hio, 0x18);
    sum->x = scratch->x;
    sum->z = scratch->z;
    sum->y = f32(scratch->y) + gravity;
    u32 neighbors = 0x101B95E0 + vertex * 24;
    for (u32 edge = 0; edge < 5; ++edge) {
      s32 other = read<s32>(neighbors, edge * 4);
      if (other == -1) break;
      // Adjacent vertices in a row have the short rest length; row links use the long length.
      u32 delta = u32(other) - vertex;
      u32 sign = u32(s32(delta) >> 31);
      u32 distance = (delta ^ sign) - sign;
      f32 rest = other == 0 || vertex == 0 || s32(distance) > 1 ? longRest : shortRest;
      gabi::call(0x0201ADE0, ptr(source + u32(other) * 12), difference.get(), center.get());
      gabi::call(0x0201B1E4, difference.get(), normalized.get());
      f32 square = gabi::call<f32>(0x028E8DD0, difference.get());
      f32 length = gabi::call<f32>(0x028F4384, square);
      f32 spring = (length - rest) * read<f32>(hio, 0x1C);
      gabi::call(0x028E8E64, normalized.get(), normalized.get(), spring);
      gabi::call(0x028E8D88, sum.get(), normalized.get(), sum.get());
    }
    bias->z = sum->z; bias->x = sum->x; bias->y = sum->y;
    gabi::call(0x028E8D88, ptr(velocity), bias.get(), ptr(velocity));
    gabi::call(0x028E8E64, ptr(velocity), ptr(velocity), damping);
    gabi::call(0x028E8D88, ptr(output), ptr(velocity), ptr(output));
  }
  u32 normals = a + 0x18E8 + read<u8>(a, 0x1DDA) * 252;
  gabi::call(0x021B8D98, ptr(a, 0x3AC));
  for (s32 vertex = 0; vertex < 21; ++vertex) {
    gabi::call(0x021B8DA8, ptr(a, 0x3AC), ptr(normals), vertex);
    normals += 12;
  }
  gabi::call(0x021B9244, ptr(a, 0x3AC));
}
VERIFY(0x021B92DC, majuu_clothMove);

s32 majuu_create(daMajuuFlag_c* actor) {
  WWHD_FUNC(0x021BA650, s32, actor);
  u32 a = gabi::ea(actor), flags = read<u32>(a, 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, actor);
      write<u32>(a, 0xB4, 0x100143BC);
      gabi::call(0x021B98B4, ptr(a, 0x3AC));
      flags = read<u32>(a, 0x2E4);
    }
    write<u32>(a, 0x2E4, flags | 8);
  }
  u32 parameters = read<u32>(a, 0xB0);
  u8 flagType = u8(parameters), textureType = u8(parameters >> 24);
  write<u8>(a, 0x1E01, textureType);
  s32 phase = gabi::call<s32>(0x02520460, ptr(a, 0x1DDC), ptr(0x100144E8));
  if (phase != 4) return phase;
  textureType = read<u8>(a, 0x1E01);
  if (textureType >= 1 && textureType <= 3) {
    u32 archive = textureType == 1 ? 0x100144F0 : textureType == 2 ? 0x100144F8 : 0x10014500;
    phase = gabi::call<s32>(0x02520460, ptr(a, 0x1DE4), ptr(archive));
    if (phase != 4) return phase;
  }
  write<u8>(a, 0x1E00, flagType);
  u8 current = read<u8>(a, 0x1DDA);
  f32 randomExtent = read<f32>(0x100144BC), one = read<f32>(0x10014408), zero = read<f32>(0x100143DC);
  u32 positions = a + 0x16F0 + current * 252, normals = a + 0x18E8 + current * 252;
  for (u32 vertex = 0; vertex < 21; ++vertex) {
    u32 initial = 0x101B9338 + vertex * 12, position = positions + vertex * 12;
    write<f32>(position, 0, read<f32>(initial));
    write<f32>(position, 4, read<f32>(initial, 4));
    write<f32>(position, 8, read<f32>(initial, 8));
    write<f32>(normals, 0, one);
    write<f32>(normals, 4, zero);
    write<f32>(normals, 8, zero);
    u32 velocity = a + 0x1CD8 + vertex * 12;
    write<f32>(velocity, 0, zero);
    write<f32>(velocity, 4, zero);
    write<f32>(velocity, 8, zero);
    if (vertex != 15 && vertex != 20) {
      f32 jitter = gabi::call<f32>(0x02019918, randomExtent);
      write<f32>(position, 0, read<f32>(position) + jitter);
      jitter = gabi::call<f32>(0x02019918, randomExtent);
      write<f32>(position, 4, read<f32>(position, 4) + jitter);
      jitter = gabi::call<f32>(0x02019918, randomExtent);
      write<f32>(position, 8, read<f32>(position, 8) + jitter);
    }
  }
  gabi::call(0x021B9244, ptr(a, 0x3AC));
  textureType = read<u8>(a, 0x1E01);
  write<u8>(a, 0x1E02, 0);
  f32 scale = read<f32>(0x100144C0);
  if (!textureType) {
    if (flagType == 2) scale = read<f32>(0x100144C4);
    else if (flagType == 3) {
      scale = read<f32>(0x100144C8);
      write<u8>(a, 0x1E02, 1);
    } else if (flagType != 4) scale = one;
  } else if (flagType != 255) {
    // The byte parameter is converted to float before the fused multiply-add.
    scale = gabi::fmadds(read<f32>(0x100144CC), f32(flagType), scale);
  }
  write<f32>(a, 0x1DFC, scale);
  gabi::call(0x021B8520, actor);
  f32 minZ = read<f32>(0x100144D8), minX = read<f32>(0x100144D0);
  f32 maxZ = read<f32>(0x100144E4), maxY = read<f32>(0x100144E0);
  f32 maxX = read<f32>(0x100144DC), minY = read<f32>(0x100144D4);
  write<u32>(a, 0x348, a + 0x1E04);
  gabi::call(0x025D674C, actor, minX, minY, minZ, maxX, maxY, maxZ);
  write<f32>(a, 0x364, randomExtent);
  u32 wind = gabi::call<u32>(0x0257DAA8);
  f32 windX = read<f32>(wind), windZ = read<f32>(wind, 8);
  s32 angle = gabi::call<s32>(0x020195B0, windX, windZ);
  textureType = read<u8>(a, 0x1E01);
  write<u16>(a, 0x322, u16(angle));
  if (textureType == 3) for (u32 i = 0; i < 20; ++i) gabi::call(0x021B92DC, actor);
  write<u32>(a, 0x3C4, a);
  gabi::call(0x021B9C78, ptr(a, 0x3AC));
  return 4;
}
VERIFY(0x021BA650, majuu_create);

namespace {
void bindFlagProgram(u32 program) {
  u32 state = gabi::call<u32>(0x027F29D4, ptr(0x104B45C0));
  u32 shader = read<u32>(program), previous = read<u32>(state, 4);
  if (shader == previous) return;
  u8 flags = read<u8>(shader);
  u32 previousFetch = read<u32>(state);
  if (flags & 2) {
    write<u8>(shader, 0, flags & ~2);
    gabi::call(0x027BB9E0, ptr(shader), 0);
  }
  u32 fetch = read<u32>(read<u32>(shader, 0x7C), 0x28);
  if (previousFetch != fetch) gabi::call(0x027B9F68, ptr(fetch));
  u32 displayListSize = read<u32>(shader, 0xC);
  if (displayListSize) {
    gabi::call(0xC00060E0, ptr(read<u32>(shader, 4)), displayListSize);
    write<u32>(state, 0, fetch);
    write<u32>(state, 4, shader);
  } else {
    gabi::call(0x027BB7CC, ptr(shader));
    write<u32>(state, 4, shader);
    write<u32>(state, 0, fetch);
  }
}
void bindFlagUniform(u32 object, u32 program) {
  u32 block = object + 0x10 + read<u32>(object, 0x4C) * 28;
  u32 slots = read<u32>(program, 0xC) ? read<u32>(program, 0x10) : 0;
  s16 vertexSlot = read<s16>(slots, 0xC);
  u32 size = read<u32>(block, 4), data = read<u32>(block, 0xC);
  s16 pixelSlot = read<s16>(slots, 0xE), geometrySlot = read<s16>(slots, 0x10);
  if (pixelSlot != -1) gabi::call(0xC0006900, pixelSlot, ptr(data), size);
  if (vertexSlot != -1) gabi::call(0xC0006A38, vertexSlot, ptr(data), size);
  if (geometrySlot != -1) gabi::call(0xC00068A8, geometrySlot, ptr(data), size);
}
void bindFlagObject(u32 object, u32 program) {
  u32 table = read<u32>(object, 0xC), target = read<u32>(table, 0x2C);
  gabi::call_ptr(target, ptr(object), ptr(program));
}
void drawFlagIndexed(u32 packet) {
  u32 count = read<u32>(packet, 0xA18);
  if (count) {
    u32 type = read<u32>(packet, 0xA0C), indices = read<u32>(packet, 0xA14);
    u32 primitive = read<u32>(packet, 0xA10);
    gabi::call(0xC0006178, primitive, count, type, ptr(indices), 0, 1);
  }
}
}
void majuu_packetDraw(daMajuuFlagPacket_c* packet, void* context) {
  WWHD_FUNC(0x021BAB14, void, packet, context);
  u32 p = gabi::ea(packet), ctx = gabi::ea(context), program = 0;
  s32 pass = read<s32>(ctx, 0xC);
  if (pass < 4) {
    u32 count = read<u32>(p, 0x9C), material = read<u32>(p, 0xA0);
    if (u32(pass) < count) material += u32(pass) * 20;
    program = read<u32>(material);
  }
  bindFlagProgram(program);
  pass = read<s32>(ctx, 0xC);
  if (pass == 0) {
    bindFlagObject(p + 0xA30, program);
    u32 scene = read<u32>(ctx, 0x14);
    if (scene) bindFlagUniform(read<u32>(scene, 4), program);
  } else if (pass == 1 || pass == 2) {
    bindFlagUniform(read<u32>(p, 0xA28), program);
    bindFlagObject(p + 0xAD8, program);
    bindFlagObject(p + 0xA30, program);
    if (pass == 2) {
      u32 extra = read<u32>(ctx, 0x30);
      if (extra) bindFlagObject(extra, program);
      gabi::call(0x027FFE54, context, ptr(program));
    }
  }
  u32 texture = read<u32>(program, 0x14) ? read<u32>(program, 0x18) : 0;
  gabi::call(0x027BE53C, ptr(p, 0xF60), ptr(texture, 4), -1, 0);
  texture = read<u32>(program, 0x14) > 1 ? read<u32>(program, 0x18) + 0x14 : 0;
  gabi::call(0x027BE53C, ptr(p, 0x10F8), ptr(texture, 4), -1, 0);
  gabi::Local<u8[0x11C]> drawState;  // frame r1+8 .. saved registers at r1+0x124
  u32 state = gabi::ea(drawState.get());
  gabi::call(0x02750250, drawState.get());
  u32 flags = read<u32>(state, 0xEC);
  write<u32>(state, 8, 0);
  write<u32>(state, 0xC, 2);
  write<u8>(state, 0xE0, 0);
  write<u32>(state, 0xEC, ((((flags & ~0xFu) + 7) & 0xFFFFFF0Fu) + 0x10));
  gabi::call(0x0280037C, read<u32>(ctx, 0xC), drawState.get());
  gabi::call(0x02750370, drawState.get());
  u32 count = read<u32>(p, 0x9C), index = read<u32>(ctx, 0xC), material = read<u32>(p, 0xA0);
  u32 current = read<u32>(p, 0x9F4);
  if (index < count) material += index * 20;
  u32 sideOffset = current == 0 ? 8 : 0;
  gabi::call(0x027BFE5C, ptr(read<u32>(material + sideOffset, 8)));
  drawFlagIndexed(p);
  write<u32>(state, 8, 1);
  gabi::call(0x02750684, drawState.get());
  count = read<u32>(p, 0x9C); index = read<u32>(ctx, 0xC);
  current = read<u32>(p, 0x9F4); material = read<u32>(p, 0xA0);
  if (index < count) material += index * 20;
  sideOffset = current == 0 ? 8 : 0;
  count = read<u32>(material + sideOffset, 4);
  u32 attributes = read<u32>(material + sideOffset, 8);
  if (count > 1) attributes += 0xF4;
  gabi::call(0x027BFE5C, ptr(attributes));
  drawFlagIndexed(p);
  gabi::call(0x02750370, ptr(0x104B474C));
}
VERIFY(0x021BAB14, majuu_packetDraw);

namespace {
u32 allocateFlagBuffer(u32 size, s32 alignment) {
  u32 heap = gabi::call<u32>(0x02756140, ptr(read<u32>(0x101F8B4C)));
  u32 table = read<u32>(heap, 0xC), target = read<u32>(table, 0x34);
  return gabi::call_ptr<u32>(target, ptr(heap), size, alignment);
}
void releaseFlagAttributes(u32 material, u32 countOffset) {
  u32 objects = read<u32>(material, countOffset + 4);
  if (!objects) return;
  for (s32 i = 0; i < read<s32>(material, countOffset); ++i) {
    u32 object = objects + u32(i) * 0xF4;
    u32 table = read<u32>(object, 0xF0), target = read<u32>(table, 0xC);
    gabi::call_ptr(target, ptr(object), 2);
    objects = read<u32>(material, countOffset + 4);
  }
  freeOwnedField(material, countOffset + 4);
  write<u32>(material, countOffset, 0);
  write<u32>(material, countOffset + 4, 0);
}
void zeroFlagCacheLines(u32 address, u32 size) {
  u32 end = address + size;
  if (address < end) do {
    u32 line = address & ~31u;
    for (u32 offset = 0; offset < 32; offset += 4) write<u32>(line, offset, 0);
    address += 32;
  } while (address < end);
}
bool flagTextureMatches(u32 texture, u32 image) {
  const u32 offsets[] = {4, 8, 12, 16, 20, 24, 0x38, 0x34, 0x1C};
  for (u32 offset : offsets) if (read<u32>(texture, offset) != read<u32>(image, offset)) return false;
  return true;
}
void updateFlagTexture(u32 packet, u32 textureOffset, u32 imageOffset) {
  u32 texture = packet + textureOffset, image = packet + imageOffset;
  if (!flagTextureMatches(texture, image)) {
    gabi::call(0x027BDEB4, ptr(texture), ptr(image));
    u8 flags = read<u8>(texture, 0x190);
    write<u32>(texture, 0x160, 2);
    write<u32>(texture, 0x15C, 2);
    write<u32>(texture, 0x164, 2);
    write<u8>(texture, 0x190, flags | 2);
  } else {
    u32 data = read<u32>(image, 0x28), mipmaps = read<u32>(image, 0x30);
    write<u32>(texture, 0xD4, data);
    write<u32>(texture, 0xDC, mipmaps);
    write<u32>(texture, 0x30, mipmaps);
    u8 flags = read<u8>(texture, 0x190);
    write<u32>(texture, 0x15C, 2);
    write<u32>(texture, 0x28, data);
    write<u8>(texture, 0x190, flags | 2);
    write<u32>(texture, 0x160, 2);
    write<u32>(texture, 0x164, 2);
  }
}
}
void majuu_packetSetup(daMajuuFlagPacket_c* packet) {
  WWHD_FUNC(0x021B9C78, void, packet);
  u32 p = gabi::ea(packet), nameSpace = 0x100143A4;
  gabi::Local<u32[2]> shaderName;
  write<u32>(gabi::ea(shaderName.get()), 4, nameSpace);
  write<u32>(gabi::ea(shaderName.get()), 0, 0x10014460);
  u32 actor = read<u32>(p, 0x18), repository = gabi::call<u32>(0x027FFCBC);
  u32 shaderArchive = read<u32>(repository, 4);
  s32 index = gabi::call<s32>(0x027B90AC, ptr(shaderArchive), shaderName.get());
  u32 shader = 0;
  if (index >= 0) {
    u32 count = read<u32>(repository, 8), records = read<u32>(repository, 0xC);
    u32 record = records + (u32(index) < count ? u32(index) * 36 : 0);
    if (!read<u8>(record, 0x20)) {
      shaderArchive = read<u32>(repository, 4);
      u32 shaderCount = read<u32>(shaderArchive, 0x1C), compiled = 0;
      if (u32(index) < shaderCount) compiled = read<u32>(shaderArchive, 0x20) + u32(index) * 132;
      gabi::call(0x02800B0C, ptr(record), ptr(compiled), 0);
      count = read<u32>(repository, 8); records = read<u32>(repository, 0xC);
    }
    shader = records + (u32(index) < count ? u32(index) * 36 : 0);
  }
  gabi::call(0x0280068C, ptr(p, 0x98), ptr(shader), 0);
  write<u32>(p, 0x9F8, 19);
  u32 pairs = p + 0xA4;
  write<u32>(p, 0xA00, 0x10014520);
  for (u32 kind = 0; kind < 2; ++kind) for (u32 side = 0; side < 2; ++side) {
    u32 buffer = pairs + kind * 0x254 + side * 0x4A8;
    u32 data = read<u32>(buffer);
    if (!data) {
      u32 allocation = allocateFlagBuffer(0x460, 0x40);
      if (allocation) {
        write<u32>(buffer, 0x250, allocation);
        write<u32>(buffer, 0x24C, 35);
      }
      data = read<u32>(buffer, 0x250);
      write<u32>(buffer, 0, data);
    }
    gabi::call(0x027FF478, ptr(buffer, 4), ptr(data), 35, ptr(pairs, 0x954));
  }
  write<u32>(pairs, 0x960, 0);
  write<u8>(pairs, 0x964, 1);
  for (u32 materialIndex = 0; materialIndex < read<u32>(p, 0x98); ++materialIndex) {
    u32 count = read<u32>(p, 0x9C), material = read<u32>(p, 0xA0);
    if (materialIndex < count) material += materialIndex * 20;
    u32 program = read<u32>(material);
    write<u32>(material, 0, 0);
    releaseFlagAttributes(material, 4);
    releaseFlagAttributes(material, 12);
    write<u32>(material, 0, program);
    for (u32 kind = 0; kind < 2; ++kind) {
      u32 allocation = allocateFlagBuffer(0x1E8, 4);
      for (u32 side = 0; side < 2; ++side) {
        u32 object = allocation + side * 0xF4;
        if (object) gabi::call(0x027BF734, ptr(object));
      }
      if (allocation) {
        write<u32>(material, 8 + kind * 8, allocation);
        write<u32>(material, 4 + kind * 8, 2);
      }
    }
    for (u32 kind = 0; kind < 2; ++kind) for (u32 side = 0; side < 2; ++side) {
      u32 descriptor = material + 4 + kind * 8;
      u32 objectCount = read<u32>(descriptor), object = read<u32>(descriptor, 4);
      if (side < objectCount) object += side * 0xF4;
      gabi::call(0x027FF530, ptr(program), ptr(object), ptr(pairs, 4 + kind * 0x254 + side * 0x4A8), ptr(pairs, 0x954), 0);
    }
  }
  gabi::call(0x027FE084, ptr(p, 0xA24), 1, 0);
  u32 indicesCount = read<u32>(0x101B9314);
  gabi::call(0x027B54E0, ptr(p, 0xA0C), ptr(0x101B9548), 4, indicesCount);
  f32 zero = read<f32>(0x100143DC);
  write<u32>(p, 0xA10, 4);
  u32 current = read<u32>(pairs, 0x950);
  for (u32 side = 0; side < 2; ++side) {
    u32 buffer = pairs + (side * 2 + current) * 0x254;
    u32 data = read<u32>(buffer);
    u32 end = data + 0x460;
    if (data < end) {
      zeroFlagCacheLines(data, 0x460);
      current = read<u32>(p, 0x9F4);
      pairs = p + 0xA4;
      buffer = pairs + (side * 2 + current) * 0x254;
    }
    u32 vertices = read<u32>(0x101B9310);
    data = read<u32>(buffer);
    for (u32 vertex = 0; vertex < vertices; ++vertex) {
      u32 triangle = 0x101B94DC + vertex * 3;
      u32 position = 0x101B9338 + read<u8>(triangle) * 12;
      f32 z = read<f32>(position, 8), x = read<f32>(position), y = read<f32>(position, 4);
      u32 output = data + vertex * 32;
      write<f32>(output, 0, x); write<f32>(output, 4, y); write<f32>(output, 8, z);
      write<f32>(output, 12, zero); write<f32>(output, 16, zero); write<f32>(output, 20, zero);
      u32 uv = 0x101B9434 + read<u8>(triangle, 2) * 8;
      f32 u = read<f32>(uv), v = read<f32>(uv, 4);
      write<f32>(output, 24, u); write<f32>(output, 28, v);
      vertices = read<u32>(0x101B9310);
    }
    if (vertices) current = read<u32>(pairs, 0x950);
  }
  u32 alternate = pairs + (current == 0 ? 0x254 : 0);
  for (u32 side = 0; side < 2; ++side) {
    u32 original = pairs + (side * 2 + current) * 0x254;
    u32 opposite = alternate + side * 0x4A8;
    for (u32 vertex = 0; vertex < 35; ++vertex) {
      u32 input = read<u32>(original) + vertex * 32, output = read<u32>(opposite) + vertex * 32;
      for (u32 component = 0; component < 8; ++component) write<u32>(output, component * 4, read<u32>(input, component * 4));
    }
    current = read<u32>(pairs, 0x950);
  }
  u32 uniform = pairs + current * 0x254 + 4;
  for (u32 side = 0; side < 2; ++side) {
    u32 size = read<u32>(uniform, 0x14C);
    gabi::call(0x027B5E94, ptr(uniform), 0, size);
    uniform += 0x4A8;
  }
  current = read<u32>(pairs, 0x950);
  write<u32>(pairs, 0x950, current == 0 ? 1 : 0);
  gabi::call(0x0274FBF8, ptr(read<u32>(0x101F8B18)));
  u8 textureType = read<u8>(actor, 0x1E01);
  u32 image = 0;
  if (!textureType) {
    gabi::Local<u32[2]> archive, texture;
    write<u32>(gabi::ea(archive.get()), 4, nameSpace);
    write<u32>(gabi::ea(texture.get()), 4, nameSpace);
    write<u32>(gabi::ea(archive.get()), 0, 0x10014470);
    write<u32>(gabi::ea(texture.get()), 0, 0x10014480);
    image = gabi::call<u32>(0x026124B0, ptr(read<u32>(0x101F4F7C)), archive.get(), texture.get(), 0);
    gabi::call(0x02773870, ptr(p, 0xE40), ptr(image), ptr(0x10014450));
  } else {
    if (textureType <= 3) {
      gabi::Local<u32[2]> texture;
      write<u32>(gabi::ea(texture.get()), 4, nameSpace);
      write<u32>(gabi::ea(texture.get()), 0, textureType == 1 ? 0x10014438 : textureType == 2 ? 0x10014440 : 0x10014458);
      image = gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), texture.get(), 3);
    }
    if (!image) gabi::call(0x0273AA24, ptr(0x10014498), 0x197, ptr(0x100144AC));
    gabi::call(0x02773798, ptr(p, 0xE40), ptr(read<u32>(image, 0x20)));
  }
  gabi::Local<u32[2]> texture;
  write<u32>(gabi::ea(texture.get()), 4, nameSpace);
  write<u32>(gabi::ea(texture.get()), 0, 0x10014448);
  image = gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), texture.get(), 3);
  if (!image) gabi::call(0x0273AA24, ptr(0x10014498), 0x19F, ptr(0x100144AC));
  gabi::call(0x02773798, ptr(p, 0xED0), ptr(read<u32>(image, 0x20)));
  gabi::call(0x0274FCCC, ptr(read<u32>(0x101F8B18)));
  updateFlagTexture(p, 0xF60, 0xE40);
  updateFlagTexture(p, 0x10F8, 0xED0);
}
VERIFY(0x021B9C78, majuu_packetSetup);
