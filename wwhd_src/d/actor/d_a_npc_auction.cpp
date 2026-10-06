/* Local WWHD auction NPC reconstruction. */
#include "d/actor/d_a_npc_auction.h"
using Actor = daNpcAuction_c;
using gabi::call;
using gabi::load;
using gabi::store;
namespace {
template <class T> T read(Actor *a, u32 offset) {
  return load<T>(gabi::ea(a) + offset);
}
template <class T> void write(Actor *a, u32 offset, T value) {
  store<T>(gabi::ea(a) + offset, value);
}
struct SafeString {
  be<u32> text, vtable;
};
} // namespace
u32 auctionCreateHeap(Actor *a);
s32 auctionXyEvent(Actor *a, s32 event);
u32 auctionXyCheck(Actor *a, s32 event);

u8 auctionNpcNumber(Actor *a) {
  WWHD_FUNC(0x021EB750, u8, a);
  s8 index = read<s8>(a, 0x2DD);
  return (u32)(s32)index < 8 ? (u8)index : 0;
}
VERIFY(0x021EB750, auctionNpcNumber);
s32 auctionRandom(Actor *a, s32 limit) {
  WWHD_FUNC(0x021EB870, s32, a, limit);
  s32 result = gabi::ftoi(call<f32>(0x020198D8, (f32)limit));
  return (u32)result == (u32)limit ? 0 : result;
}
VERIFY(0x021EB870, auctionRandom);
void auctionWaitInit(Actor *a) {
  WWHD_FUNC(0x021EBB90, void, a);
  write<s16>(a, 0x7DE, -1);
  write<u32>(a, 0x7E0, 0x021ED81C);
  write<s16>(a, 0x7DC, 0);
}
VERIFY(0x021EBB90, auctionWaitInit);
u32 auctionXyCheck(Actor *a, s32 event) {
  WWHD_FUNC(0x021EBBB0, u32, a, event);
  return 0;
}
VERIFY(0x021EBBB0, auctionXyCheck);
u32 auctionXyCheckThunk(Actor *a, s32 event) {
  WWHD_FUNC(0x021EBBB8, u32, a, event);
  return auctionXyCheck(a, event);
}
VERIFY(0x021EBBB8, auctionXyCheckThunk);
s32 auctionXyEventThunk(Actor *a, s32 event) {
  WWHD_FUNC(0x021EBC78, s32, a, event);
  return auctionXyEvent(a, event);
}
VERIFY(0x021EBC78, auctionXyEventThunk);
void auctionEmitterEnable(Actor *a) {
  WWHD_FUNC(0x021EC200, void, a);
  write<u8>(a, 0x8B5, 1);
}
VERIFY(0x021EC200, auctionEmitterEnable);
u32 auctionHeapThunk(Actor *a) {
  WWHD_FUNC(0x021EB74C, u32, a);
  return auctionCreateHeap(a);
}
VERIFY(0x021EB74C, auctionHeapThunk);
u32 auctionIsDelete(Actor *a) {
  WWHD_FUNC(0x021EE404, u32, a);
  return 1;
}
VERIFY(0x021EE404, auctionIsDelete);
void auctionSafeStringDestructor(u32 object, s32 flags) {
  WWHD_FUNC(0x021EE3F0, void, object, flags);
  if (object && (flags & 1))
    call(0x0273AF40, object);
}
VERIFY(0x021EE3F0, auctionSafeStringDestructor);
void auctionSafeStringPrepare(u32 object) {
  WWHD_FUNC(0x021EE4A8, void, object);
}
VERIFY(0x021EE4A8, auctionSafeStringPrepare);
Actor *auctionConstructor(Actor *a) {
  WWHD_FUNC(0x021EB76C, Actor *, a);
  u32 self = gabi::ea(a);
  if (!self)
    self = call<u32>(0x0273AD10, 0x8B8);
  if (!self)
    return nullptr;
  a = gabi::at<Actor>(self);
  call(0x025A1458, self);
  store<u32>(self + 0xB4, 0x10016804);
  call(0x028F521C, self + 0x7E4, 8);
  write<u32>(a, 0x7F4, 0);
  write<u32>(a, 0x7F8, 0);
  call(0x025E7820, self + 0x7FC);
  write<u16>(a, 0x8A0, 0);
  write<u32>(a, 0x870, 0);
  write<u32>(a, 0x89C, 0);
  write<u32>(a, 0x88C, 0);
  write<f32>(a, 0x894, 0.0f);
  write<u32>(a, 0x898, 0);
  write<u16>(a, 0x8A2, 0);
  write<f32>(a, 0x890, 0.0f);
  write<u32>(a, 0x874, 0);
  call(0x028F521C, self + 0x8A4, 2);
  write<u8>(a, 0x8B1, 0);
  write<u8>(a, 0x8B0, 1);
  write<u32>(a, 0x88C, 0);
  write<u16>(a, 0x8A6, 0);
  write<u8>(a, 0x8AF, 0);
  write<u8>(a, 0x8B6, 0);
  write<u32>(a, 0x874, 0);
  write<u8>(a, 0x8AC, 0);
  write<u8>(a, 0x8AB, 0);
  write<u8>(a, 0x8AE, 0);
  write<u8>(a, 0x8B5, 0);
  write<u32>(a, 0x870, 0);
  for (u32 off : {0x8A8u, 0x8AAu, 0x8ADu, 0x8B4u, 0x8B2u, 0x8A9u, 0x8B3u})
    write<u8>(a, off, 0);
  return a;
}
VERIFY(0x021EB76C, auctionConstructor);
void auctionDestructor(Actor *a, s32 flags) {
  WWHD_FUNC(0x021EE40C, void, a, flags);
  u32 self = gabi::ea(a);
  if (!self)
    return;
  call(0x02515A70, self + 0x690, 2);
  call(0x02515860, self + 0x654, 2);
  call(0x02018034, self + 0x628, 2);
  write<u32>(a, 0x470, 0x1001630C);
  write<u32>(a, 0x464, 0x1001631C);
  call(0x024EFD9C, self + 0x450, 0);
  call(0x025D50BC, self, 0);
  if (flags & 1)
    call(0x0273AF40, self);
}
VERIFY(0x021EE40C, auctionDestructor);
u32 auctionInitTexture(Actor *a, u32 restart) {
  WWHD_FUNC(0x021EB318, u32, a, restart);
  u32 model = read<u32>(a, 0x7F4);
  if (!model)
    model = load<u32>(read<u32>(a, 0x44C) + 0x90);
  u32 material = load<u32>(model + 0xAC);
  u8 costume = read<u8>(a, 0x8AD);
  u32 animation = read<u32>(a, 0x89C);
  u32 name = load<u32>(0x101BB7FC + 4 * costume);
  gabi::Local<SafeString> resource;
  resource->text = name;
  resource->vtable = 0x100162F4;
  u32 resources = load<u32>(0x101F4F28);
  u32 texture = call<u32>(0x026067F4, resources, resource.get(), animation);
  write<u32>(a, 0x7F8, texture);
  if (!texture) {
    call(0x0273AA24, 0x100165FCu, 0x902, 0x10016610u);
    texture = read<u32>(a, 0x7F8);
  }
  u32 ok = call<u32>(0x025E789C, gabi::ea(a) + 0x7FC, material, texture, 1, 2,
                     0, -1, restart, 0, 1.0f);
  if (!ok)
    return 0;
  write<u8>(a, 0x8AB, 0);
  write<u16>(a, 0x8A0, 0);
  return 1;
}
VERIFY(0x021EB318, auctionInitTexture);
s32 auctionXyEvent(Actor *a, s32 event) {
  WWHD_FUNC(0x021EBBBC, s32, a, event);
  u32 play = call<u32>(0x025200D4);
  s32 index = call<s32>(0x02543F10, play + 0x52C4, 0x100166D8u, 255);
  play = call<u32>(0x025200D4);
  u8 item = load<u8>(play + (u32)event + 0x5BBB);
  bool missing = false;
  if (item == 150 || item == 151) {
    u32 save = load<u32>(0x101F84DC);
    missing =
        !call<u32>(0x025B8B94, save + 0x644, item == 150 ? 0x1008 : 0x1004);
  }
  if (missing) {
    index = read<s16>(a, 0x8A4);
    write<u8>(a, 0x8B0, 0);
  }
  return index;
}
VERIFY(0x021EBBBC, auctionXyEvent);
u32 auctionCreateHeap(Actor *a) {
  WWHD_FUNC(0x021EB450, u32, a);
  u32 self = gabi::ea(a);
  gabi::Local<SafeString> modelName, animationName, extraName;
  u8 costume = read<u8>(a, 0x8AD);
  u32 resource = load<u32>(0x101F4F28);
  u32 modelNumber = load<u32>(0x1001633C + 4 * costume);
  modelName->text = load<u32>(0x101BB7FC + 4 * costume);
  modelName->vtable = 0x100162F4;
  u32 modelData = call<u32>(0x026067F4, resource, modelName.get(), modelNumber);
  costume = read<u8>(a, 0x8AD);
  u8 animationIndex = read<u8>(a, 0x8B2);
  resource = load<u32>(0x101F4F28);
  u32 animationNumber =
      load<u32>(0x10016418 + 4 * ((u32)costume * 10 + animationIndex));
  animationName->text = load<u32>(0x101BB7FC + 4 * costume);
  animationName->vtable = 0x100162F4;
  u32 animation =
      call<u32>(0x026067F4, resource, animationName.get(), animationNumber);
  u32 morf = call<u32>(0x025E4F64, 0, modelData, 0, 0, animation, 2, 0, -1, 1,
                       0, 0x80000, 0x11020203, 1.0f);
  write<u32>(a, 0x44C, morf);
  if (!morf || !load<u32>(morf + 0x90))
    return 0;
  costume = read<u8>(a, 0x8AD);
  s32 extraModel = load<s32>(0x1001636C + 4 * costume);
  if (extraModel >= 0) {
    extraName->text = load<u32>(0x101BB7FC + 4 * costume);
    extraName->vtable = 0x100162F4;
    resource = load<u32>(0x101F4F28);
    u32 extraData =
        call<u32>(0x026067F4, resource, extraName.get(), extraModel);
    u32 extra = call<u32>(0x025E38E0, extraData, 0x80000, 0x15020022);
    write<u32>(a, 0x7F4, extra);
    if (!extra)
      return 0;
  }
  u32 names = call<u32>(0x027F68FC, modelData);
  u32 delta = load<u32>(names + 0x10);
  u32 strings = delta ? names + 0x10 + delta : 0;
  s8 head = (s8)call<s32>(0x027DF9B0, strings, 0x10016630u);
  write<s8>(a, 0x3B4, head);
  if (head < 0)
    call(0x0273AA24, 0x10016638u, 0x4A9, 0x1001664Cu);
  names = call<u32>(0x027F68FC, modelData);
  delta = load<u32>(names + 0x10);
  strings = delta ? names + 0x10 + delta : 0;
  s8 backbone = (s8)call<s32>(0x027DF9B0, strings, 0x10016668u);
  write<s8>(a, 0x3B5, backbone);
  if (backbone < 0)
    call(0x0273AA24, 0x10016638u, 0x4AD, 0x10016674u);
  if (!auctionInitTexture(a, 0))
    return 0;
  u32 tree = call<u32>(0x027F3F94, modelData);
  u16 count = load<u16>(tree + 8);
  for (u16 joint = 0; joint < count; ++joint) {
    s8 headIndex = read<s8>(a, 0x3B4);
    if ((u32)joint == (u32)(s32)headIndex ||
        (u32)joint == (u32)(s32)read<s8>(a, 0x3B5)) {
      u32 extent = load<u32>(modelData + 4);
      u32 entry = load<u32>(modelData + 8);
      if (joint < extent)
        entry += (u32)joint * 0x1C;
      u8 npc = read<u8>(a, 0x8AC);
      store<u32>(entry + 8, load<u32>(0x101BB6B8 + 4 * npc));
    }
    tree = call<u32>(0x027F3F94, modelData);
    count = load<u16>(tree + 8);
  }
  morf = read<u32>(a, 0x44C);
  u32 model = load<u32>(morf + 0x90);
  store<u32>(model + 0xB8, self);
  call(0x024EFF44, self + 0x614, 30.0f, 30.0f);
  call(0x024F06B4, self + 0x450, self + 0x314, self + 0x300, self, 1,
       self + 0x614, self + 0x33C, self + 0x320, self + 0x328);
  return 1;
}
VERIFY(0x021EB450, auctionCreateHeap);
void auctionSetMatrix(Actor *a) {
  WWHD_FUNC(0x021EBC7C, void, a);
  u32 model = load<u32>(read<u32>(a, 0x44C) + 0x90);
  f32 z = read<f32>(a, 0x338), x = read<f32>(a, 0x330), y = read<f32>(a, 0x334);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  store<f32>(model + 0xC4, z);
  x = read<f32>(a, 0x314);
  y = read<f32>(a, 0x318);
  z = read<f32>(a, 0x31C);
  call(0x028E93CC, 0x1048D0CCu, x, y, z);
  call(0x025F1C28, 0x1048D0CCu, read<s16>(a, 0x322));
  model = load<u32>(read<u32>(a, 0x44C) + 0x90);
  f32 matrix[12];
  for (u32 i = 0; i < 12; ++i)
    matrix[i] = load<f32>(0x1048D0CC + 4 * i);
  for (u32 i = 0; i < 12; ++i)
    store<f32>(model + 0xC8 + 4 * i, matrix[i]);
}
VERIFY(0x021EBC7C, auctionSetMatrix);
u32 auctionCreateInit(Actor *a) {
  WWHD_FUNC(0x021EBD5C, u32, a);
  u32 self = gabi::ea(a);
  u8 npc = read<u8>(a, 0x8AC);
  write<f32>(a, 0x374, -9.0f);
  call(0x0259F814, self + 0x3E0, load<u32>(0x101BB698 + 4 * npc), self);
  u32 morf = read<u32>(a, 0x44C);
  write<u8>(a, 0x8A8, 0);
  write<u16>(a, 0x8A2, 0);
  write<u8>(a, 0x8A9, 0);
  u32 model = load<u32>(morf + 0x90);
  write<u32>(a, 0x348, model ? model + 0xC8 : 0);
  auctionWaitInit(a);
  u8 attention = read<u8>(a, 0x8AC) == 0 ? 0xAD : 0xA9;
  write<u8>(a, 0x389, attention);
  write<u32>(a, 0x39C, 10);
  write<u8>(a, 0x38B, attention);
  u32 play = call<u32>(0x025200D4);
  s32 event = call<s32>(0x02543F10, play + 0x52C4, 0x100166F4u, 255);
  write<s16>(a, 0x8A4, (s16)event);
  call(0x0253E9B0, self + 0xF8, 0x10016704u);
  u8 costume = read<u8>(a, 0x8AD);
  write<u32>(a, 0x100, 0x021EBC78);
  write<u32>(a, 0x104, 0x021EBBB8);
  write<f32>(a, 0x890, load<f32>(0x10465A78 + (u32)costume * 0x34));
  auctionSetMatrix(a);
  model = load<u32>(read<u32>(a, 0x44C) + 0x90);
  call(0x027F4D5C, model);
  call(0x02515F14, self + 0x654, 255, 255, self);
  call(0x02516518, self + 0x690, 0x101EA190u);
  write<u32>(a, 0x6D4, self + 0x654);
  call(0x025A15AC, self, 60.0f, 150.0f);
  return 4;
}
VERIFY(0x021EBD5C, auctionCreateInit);
u32 auctionPhaseTwo(Actor *a) {
  WWHD_FUNC(0x021EBF80, u32, a);
  u32 self = gabi::ea(a);
  u8 costume = read<u8>(a, 0x8AD);
  u32 result =
      call<u32>(0x02520460, self + 0x7E4, load<u32>(0x101BB7FC + 4 * costume));
  if (result == 4) {
    if (call<u32>(0x025D63E8, self, 0x021EB74Cu, 0x10000))
      return auctionCreateInit(a);
    write<u32>(a, 0x44C, 0);
    return 5;
  }
  return result;
}
VERIFY(0x021EBF80, auctionPhaseTwo);
u32 auctionCreate(Actor *a) {
  WWHD_FUNC(0x021EC014, u32, a);
  if (!load<u32>(0x101FDB54)) {
    store<u32>(0x101FDB54, 1);
    call(0xC000A848, gabi::at<void>(0x101FDB58u), gabi::at<void>(0x101BB728u),
         12);
  }
  return call<u32>(0x02525FE4, gabi::ea(a) + 0x7EC, 0x101FDB58u, a);
}
VERIFY(0x021EC014, auctionCreate);
u32 auctionDelete(Actor *a) {
  WWHD_FUNC(0x021EC084, u32, a);
  u8 costume = read<u8>(a, 0x8AD);
  call(0x025204C8, gabi::ea(a) + 0x7E4, load<u32>(0x101BB7FC + 4 * costume));
  if (read<u32>(a, 0xF4)) {
    u32 morf = read<u32>(a, 0x44C);
    if (morf)
      call(0x025E563C, morf);
  }
  return 1;
}
VERIFY(0x021EC084, auctionDelete);
u32 auctionIsExecute(Actor *a) {
  WWHD_FUNC(0x021EC0E8, u32, a);
  u8 costume = read<u8>(a, 0x8AD);
  u32 status = read<u32>(a, 0x2E0);
  u32 mask = load<u32>(0x101BB6D8 + 4 * costume);
  gabi::Local<be<u16>> process;
  *process = (u16)0x17E;
  write<u32>(a, 0x2E0, status & ~mask);
  u32 manager = call<u32>(0x025D5218, 0x025E121Cu, process.get());
  if (!manager)
    return 0;
  u32 shift = read<u8>(a, 0x8AC) & 63;
  u8 active = load<u8>(manager + 0x938);
  if (!(active & (shift < 32 ? 1u << shift : 0)))
    return 0;
  costume = read<u8>(a, 0x8AD);
  status = read<u32>(a, 0x2E0);
  mask = load<u32>(0x101BB6D8 + 4 * costume);
  write<u32>(a, 0x2E0, status | mask);
  return 1;
}
VERIFY(0x021EC0E8, auctionIsExecute);

void auctionCheckOrder(Actor *a) {
  WWHD_FUNC(0x021EC1A8, void, a);
  u16 state = read<u16>(a, 0xF8);
  if (state == 2) {
    write<u8>(a, 0x8AA, 0);
  } else if (state == 1) {
    u8 order = read<u8>(a, 0x8AA);
    if ((order == 1 || order == 2) && !read<u8>(a, 0x8A8) &&
        !read<u8>(a, 0x8B1))
      write<u8>(a, 0x8A8, 1);
  }
}
VERIFY(0x021EC1A8, auctionCheckOrder);

void auctionEventMessageInit(Actor *a, s32 staff) {
  WWHD_FUNC(0x021EC20C, void, a, staff);
  u32 play = call<u32>(0x025200D4);
  u32 substance = call<u32>(0x0254487C, play + 0x52C4, staff, 0x10016714u, 3);
  u32 message = substance ? load<u32>(substance)
                          : call<u32>(load<u32>(read<u32>(a, 0xB4) + 0x1C), a);
  write<u32>(a, 0x7C0, message);
}
VERIFY(0x021EC20C, auctionEventMessageInit);

void auctionSetAnimation(Actor *a, s32 animation, s32 mode, f32 blend) {
  WWHD_FUNC(0x021EC288, void, a, animation, mode, blend);
  u8 costume = read<u8>(a, 0x8AD);
  u32 resources = load<u32>(0x101F4F28);
  u32 archive = load<u32>(0x101BB7FC + 4 * costume);
  u32 resourceId =
      load<u32>(0x10016418 + 4 * ((u32)costume * 10 + (u32)animation));
  gabi::Local<SafeString> name;
  name->text = archive;
  name->vtable = 0x100162F4;
  u32 resource = call<u32>(0x026067F4, resources, name.get(), resourceId);
  call(0x025E4A98, read<u32>(a, 0x44C), resource, mode, 0, blend, 1.0f, 0.0f,
       -1.0f);
  write<u8>(a, 0x8B2, (u8)animation);
}
VERIFY(0x021EC288, auctionSetAnimation);

void auctionSetAnimationIfNeeded(Actor *a, s32 animation, s32 mode, f32 blend) {
  WWHD_FUNC(0x021EC360, void, a, animation, mode, blend);
  if ((u32)read<u8>(a, 0x8B2) != (u32)animation || mode == 0) {
    auctionSetAnimation(a, animation, mode, blend);
    write<u32>(a, 0x88C, 0);
  }
}
VERIFY(0x021EC360, auctionSetAnimationIfNeeded);

void auctionClearEmitter(Actor *a) {
  WWHD_FUNC(0x021EC3AC, void, a);
  u32 emitter = read<u32>(a, 0x870);
  if (!emitter)
    return;
  u32 flags = load<u32>(emitter + 0x254);
  store<u32>(emitter + 0x5C, 0xFFFFFFFF);
  u32 node = load<u32>(emitter + 0x1AC);
  store<u32>(emitter + 0x254, flags | 1);
  while (node) {
    u32 particle = load<u32>(node);
    store<u32>(particle + 0xCC, load<u32>(particle + 0xCC) | 2);
    node = load<u32>(node + 0xC);
  }
  write<u32>(a, 0x870, 0);
}
VERIFY(0x021EC3AC, auctionClearEmitter);

u32 auctionEventMessage(Actor *a) {
  WWHD_FUNC(0x021EC6F4, u32, a);
  return call<u32>(0x025A11EC, a, 0) == 0x12;
}
VERIFY(0x021EC6F4, auctionEventMessage);

void auctionSetAnimationTable(Actor *a, u32 table) {
  WWHD_FUNC(0x021EC878, void, a, table);
  u8 animation = load<u8>(table);
  if (animation == 255) {
    write<u32>(a, 0x88C, 0);
    return;
  }
  write<u32>(a, 0x88C, table);
  s8 repeats = load<s8>(table + 2);
  write<s8>(a, 0x8B4, repeats);
  animation = load<u8>(table);
  if (repeats > 0 || read<u8>(a, 0x8B2) != animation)
    auctionSetAnimation(a, animation, repeats > 0 ? 0 : 2,
                        (f32)load<u8>(table + 1));
}
VERIFY(0x021EC878, auctionSetAnimationTable);

void auctionEventOrder(Actor *a) {
  WWHD_FUNC(0x021ECD6C, void, a);
  u8 order = read<u8>(a, 0x8AA);
  if (order != 1 && order != 2)
    return;
  write<u16>(a, 0xFA, read<u16>(a, 0xFA) | 1);
  u32 save = load<u32>(0x101F84DC);
  u32 event = call<u32>(0x025B8B94, save + 0x644, 0x1404);
  order = read<u8>(a, 0x8AA);
  if (event)
    write<u16>(a, 0xFA, read<u16>(a, 0xFA) | 0x20);
  if (order == 2)
    call(0x025D76A8, a);
}
VERIFY(0x021ECD6C, auctionEventOrder);

void auctionPlayTexture(Actor *a) {
  WWHD_FUNC(0x021ECDF0, void, a);
  if (call<u32>(0x02055B64, gabi::ea(a) + 0x8A0))
    return;
  u32 texture = read<u32>(a, 0x7F8);
  s32 frames = call<s32>(load<u32>(load<u32>(texture + 4) + 0x14), texture);
  if ((s32)read<u8>(a, 0x8AB) >= frames) {
    texture = read<u32>(a, 0x7F8);
    frames = call<s32>(load<u32>(load<u32>(texture + 4) + 0x14), texture);
    write<u8>(a, 0x8AB, (u8)((u32)read<u8>(a, 0x8AB) - (u32)frames));
    write<u16>(a, 0x8A0, (u16)((u32)auctionRandom(a, 60) + 60));
  } else {
    write<u8>(a, 0x8AB, (u8)(read<u8>(a, 0x8AB) + 1));
  }
}
VERIFY(0x021ECDF0, auctionPlayTexture);

void auctionPlayAnimation(Actor *a) {
  WWHD_FUNC(0x021ECE9C, void, a);
  if (!call<u32>(0x025E535C, read<u32>(a, 0x44C), 0, 0, 0))
    return;
  if (!read<u32>(a, 0x88C))
    return;
  s8 repeats = read<s8>(a, 0x8B4);
  if (repeats <= 0)
    return;
  repeats = (s8)(repeats - 1);
  write<s8>(a, 0x8B4, repeats);
  u32 table = read<u32>(a, 0x88C);
  if (!repeats) {
    table += 3;
    write<u32>(a, 0x88C, table);
    auctionSetAnimationTable(a, table);
  } else {
    auctionSetAnimation(a, load<u8>(table), 0, 0.0f);
  }
}
VERIFY(0x021ECE9C, auctionPlayAnimation);

u32 auctionEventMain(Actor *a);
void auctionPrivateCut(Actor *a) {
  WWHD_FUNC(0x021EC724, void, a);
  u32 name = load<u32>(0x101BB698 + 4 * read<u8>(a, 0x8AC));
  u32 play = call<u32>(0x025200D4);
  s32 staff = call<s32>(0x02542D88, play + 0x52C4, name, 0, 0);
  if (staff == -1)
    return;
  play = call<u32>(0x025200D4);
  s8 action =
      (s8)call<u32>(0x02542EDC, play + 0x52C4, staff, 0x101BB7D0u, 2, 1, 0);
  write<s8>(a, 0x8B6, action);
  play = call<u32>(0x025200D4);
  if (action == -1) {
    call(0x02543280, play + 0x52C4, staff);
    return;
  }
  if (call<u32>(0x025447C8, play + 0x52C4, staff)) {
    action = read<s8>(a, 0x8B6);
    if (action == 0)
      auctionEmitterEnable(a);
    else if (action == 1)
      auctionEventMessageInit(a, staff);
    else {
      play = call<u32>(0x025200D4);
      call(0x02543280, play + 0x52C4, staff);
      return;
    }
  }
  action = read<s8>(a, 0x8B6);
  if (action == 0) {
    if (!auctionEventMain(a))
      return;
  } else if (action == 1) {
    if (!auctionEventMessage(a))
      return;
  }
  play = call<u32>(0x025200D4);
  call(0x02543280, play + 0x52C4, staff);
}
VERIFY(0x021EC724, auctionPrivateCut);

u32 auctionEventMain(Actor *a) {
  WWHD_FUNC(0x021EC400, u32, a);
  gabi::Local<be<u16>> process;
  *process = 0x17E;
  u32 manager = call<u32>(0x025D5218, 0x025E121Cu, process.get());
  if (!manager)
    return 0;
  u8 flags = load<u8>(manager + 0x94C);
  bool particle = false;
  auto animate = [&](s32 index, u8 bit) {
    auctionSetAnimationIfNeeded(a, index, 2, 8.0f);
    write<u8>(a, 0x8B3, read<u8>(a, 0x8B3) | bit);
  };
  if (flags & 4) {
    if (!(read<u8>(a, 0x8B3) & 8)) {
      animate(3, 8);
      flags = load<u8>(manager + 0x94C);
      particle = (flags & 4) != 0;
    } else
      particle = true;
  } else if (load<u8>(manager + 0x941) == read<u8>(a, 0x8AC)) {
    if (flags & 1) {
      if (read<u8>(a, 0x8B3) & 4)
        goto clear;
      animate(2, 4);
    } else if (flags & 8) {
      if (read<u8>(a, 0x8B3) & 0x10)
        goto clear;
      animate(4, 0x10);
    } else if (flags & 0x20) {
      if (read<u8>(a, 0x8B3) & 0x40)
        goto clear;
      animate(6, 0x40);
    } else if (flags & 0x10) {
      if (!(read<u8>(a, 0x8B3) & 0x20))
        animate(5, 0x20);
      write<u8>(a, 0x8B5, 0);
    } else {
      u8 animation = read<u8>(a, 0x8B2);
      write<u8>(a, 0x8B3, 0);
      if (animation)
        auctionSetAnimationIfNeeded(a, 0, 2, 8.0f);
    }
    flags = load<u8>(manager + 0x94C);
    particle = (flags & 4) != 0;
  } else {
    u8 animation = read<u8>(a, 0x8B2);
    write<u8>(a, 0x8B3, 0);
    if (animation)
      auctionSetAnimationIfNeeded(a, 0, 2, 8.0f);
    flags = load<u8>(manager + 0x94C);
    particle = (flags & 4) != 0;
  }
  if (!particle)
    goto clear;
  if (!read<u32>(a, 0x870)) {
    s32 room = read<s8>(a, 0x326);
    u32 play = call<u32>(0x025200D4);
    u32 self = gabi::ea(a);
    u32 emitter =
        call<u32>(0x025A847C, load<u32>(play + 0x5AB0), 0, 0x819D, self + 0x314,
                  0, 0, 255, 0, room, self + 0x1A8, self + 0x1A8, 0);
    write<u32>(a, 0x870, emitter);
    if (!emitter)
      return 0;
    store<u32>(emitter + 0x254, load<u32>(emitter + 0x254) | 0x40);
    if (!read<u32>(a, 0x870))
      return 0;
  }
  {
    u32 model = load<u32>(read<u32>(a, 0x44C) + 0x90);
    s32 joint = read<s8>(a, 0x3B4);
    u32 matrices = load<u32>(model + 0x2C);
    u16 flags = load<u16>(matrices + 4);
    u32 base = load<u32>(matrices + 0x10);
    store<u16>(matrices + 4, flags | 0x10);
    call(0x028E90D4, base + (u32)joint * 0x30, load<u32>(0x1018C7B0));
    call(0x025F1C5C, load<u32>(0x1018C7B0), -0x4000);
    u8 costume = read<u8>(a, 0x8AD);
    call(0x028E93CC, 0x1048D0CCu, 0.0f, load<f32>(0x10465A84 + costume * 0x34),
         0.0f);
    u32 matrix = load<u32>(0x1018C7B0);
    call(0x028E9108, matrix, 0x1048D0CCu, matrix);
    u32 emitter = read<u32>(a, 0x870);
    call(0x028249B0, load<u32>(0x1018C7B0), emitter + 0x1F0, emitter + 0x22C);
    return 0;
  }
clear:
  auctionClearEmitter(a);
  return 0;
}
VERIFY(0x021EC400, auctionEventMain);

void auctionAnimationFromMessage(Actor *a) {
  WWHD_FUNC(0x021EC93C, void, a);
  u32 play = call<u32>(0x025200D4);
  u8 kind = read<u8>(a, 0x8AC);
  u8 tag = load<u8>(play + 0x5BC5);
  u32 table = 0;
  if (tag <= 7) {
    if (kind == 0 || kind == 7) {
      static constexpr u32 tables[8] = {0x101BB680, 0x101BB680, 0x101BB683,
                                        0x101BB686, 0x101BB670, 0x101BB689,
                                        0x101BB734, 0x101BB68C};
      table = tables[tag];
    } else {
      static constexpr u32 tables[8] = {0x101BB680, 0x101BB68F, 0x101BB692,
                                        0x101BB695, 0x101BB678, 0x101BB695,
                                        0x101BB678, 0x101BB678};
      table = tables[tag];
    }
    auctionSetAnimationTable(a, table);
    if ((kind == 0 || kind == 7) && tag == 6) {
      write<u32>(a, 0x898, 0x8DD);
      write<u16>(a, 0x8A6, 100);
    }
  }
  play = call<u32>(0x025200D4);
  store<u8>(play + 0x5BC5, 255);
}
VERIFY(0x021EC93C, auctionAnimationFromMessage);

void auctionEventMove(Actor *a) {
  WWHD_FUNC(0x021ECB68, void, a);
  auto ended = [](u32 name) {
    u32 play = call<u32>(0x025200D4);
    return call<u32>(0x0254457C, play + 0x52C4, name) != 0;
  };
  if (ended(0x10016764) || ended(0x10016744) || ended(0x10016778)) {
    write<f32>(a, 0x890, 200.0f);
    return;
  }
  if (ended(0x10016754)) {
    if (!read<u8>(a, 0x8B1)) {
      u32 play = call<u32>(0x025200D4);
      store<u16>(play + 0x52B8, load<u16>(play + 0x52B8) | 8);
      write<u8>(a, 0x8A8, 0);
      write<u8>(a, 0x8B0, 1);
      return;
    }
    if (!read<u8>(a, 0x8AC)) {
      gabi::Local<be<u16>> process;
      *process = 0x17E;
      u32 manager = call<u32>(0x025D5218, 0x025E121Cu, process.get());
      if (manager) {
        u8 selection = read<u8>(a, 0x8B1);
        if (selection == 1) {
          store<u8>(manager + 0x939, 1);
          write<u8>(a, 0x8B0, 1);
          write<u8>(a, 0x8A8, 0);
          return;
        }
        if (selection == 2) {
          store<u8>(manager + 0x939, 2);
          store<u8>(manager + 0x93B, read<u8>(a, 0x8AF));
        }
      }
    }
    write<u8>(a, 0x8B0, 1);
    write<u8>(a, 0x8A8, 0);
    return;
  }
  u8 previousCut = read<u8>(a, 0x440);
  if (call<u32>(0x0259F858, gabi::ea(a) + 0x3E0)) {
    if (!read<u8>(a, 0x440))
      write<u8>(a, 0x440, previousCut);
  } else {
    auctionPrivateCut(a);
    if (read<u8>(a, 0x8AC) == 7)
      auctionAnimationFromMessage(a);
  }
}
VERIFY(0x021ECB68, auctionEventMove);

void auctionWaitAction(Actor *a) {
  WWHD_FUNC(0x021ED81C, void, a);
  write<u8>(a, 0x8AA, read<u8>(a, 0x8A9) != 0);
  if (read<u8>(a, 0x8A8) == 1) {
    if (call<u32>(0x025A11EC, a, 1) == 0x12) {
      write<u8>(a, 0x8A8, 0);
      auctionSetAnimationIfNeeded(a, 7, 2, 8.0f);
      if (!read<u8>(a, 0x8B1)) {
        u32 play = call<u32>(0x025200D4);
        store<u16>(play + 0x52B8, load<u16>(play + 0x52B8) | 8);
      } else if (!read<u8>(a, 0x8AC)) {
        gabi::Local<be<u16>> process;
        *process = 0x17E;
        u32 manager = call<u32>(0x025D5218, 0x025E121Cu, process.get());
        if (manager) {
          u8 selection = read<u8>(a, 0x8B1);
          if (selection == 1)
            store<u8>(manager + 0x939, 1);
          else if (selection == 2) {
            store<u8>(manager + 0x939, 2);
            store<u8>(manager + 0x93B, read<u8>(a, 0x8AF));
          }
        }
      }
    }
    auctionAnimationFromMessage(a);
  }
  if (!read<u8>(a, 0x8B2))
    auctionSetAnimationIfNeeded(a, 7, 2, 8.0f);
}
VERIFY(0x021ED81C, auctionWaitAction);

u32 auctionGetMessage(Actor *a) {
  WWHD_FUNC(0x021EDBF8, u32, a);
  u8 kind = read<u8>(a, 0x8AC);
  auto eventBit = [](u32 flag) {
    return call<u32>(0x025B8B94, load<u32>(0x101F84DC) + 0x644, flag) != 0;
  };
  if (!kind) {
    u32 play = call<u32>(0x025200D4);
    u32 selection = load<u8>(play + 0x52B0);
    if (selection - 1 <= 3) {
      play = call<u32>(0x025200D4);
      u8 item = load<u8>(play + 0x52B1);
      if (item == 150 || item == 151) {
        if (eventBit(item == 150 ? 0x1008 : 0x1004))
          return 0x27F7;
        write<u8>(a, 0x8AF, item == 151);
        return 0x27EB;
      }
      return 0x27EA;
    }
    if (eventBit(0xA02) && !call<u32>(0x02520C0C, 0x69))
      return 0x27F8;
    if (eventBit(0x1002))
      return call<u32>(0x0205CD90) ? 0x27DB : 0x27F0;
    call(0x025B8B68, load<u32>(0x101F84DC) + 0x644, 0x1002);
    return 0x27DA;
  }
  if (kind == 6)
    return 0x1D35;
  gabi::Local<be<u16>> process;
  *process = 0x17E;
  u32 manager = call<u32>(0x025D5218, 0x025E121Cu, process.get());
  if (!manager)
    call(0x0273AA24, 0x1001678Cu, 0x80A, 0x100167A0u);
  u8 costume = read<u8>(a, 0x8AD);
  u8 current = load<u8>(manager + 0x93C);
  u32 table = 0x101BB740 + costume * 12;
  if (!current)
    return load<u32>(table);
  return load<u32>(table + (current == read<u8>(a, 0x8AC) ? 4 : 8));
}
VERIFY(0x021EDBF8, auctionGetMessage);

u32 auctionNextMessageStatus(Actor *a, u32 message) {
  WWHD_FUNC(0x021ED964, u32, a, message);
  u32 player = load<u32>(0x101F4B5C);
  gabi::Local<be<u16>> process;
  *process = 0x17E;
  u32 manager = call<u32>(0x025D5218, 0x025E121Cu, process.get());
  u32 current = load<u32>(message);
  u32 next = 0;
  u32 status = 15;
  switch (current) {
  case 0x27DA:
    next = 0x27DC;
    break;
  case 0x27DC:
    next = 0x27DD;
    break;
  case 0x27DD:
    next = 0x27DE;
    break;
  case 0x27DB:
  case 0x27DE:
    next = 0x27DF;
    break;
  case 0x27DF:
    if (!load<u32>(player + 0x948)) {
      store<u32>(message, 0x27E1);
      call(0x025B8B68, load<u32>(0x101F84DC) + 0x644, 0x1408);
      return status;
    }
    next = 0x27E0;
    break;
  case 0x27E1:
    next = 0x27E2;
    break;
  case 0x27E2:
    next = load<u32>(player + 0x948) ? 0x27E4 : 0x27E3;
    break;
  case 0x27E5:
    next = 0x27E6;
    break;
  case 0x27E6:
    next = 0x27E7;
    break;
  case 0x27E7:
    next = load<u32>(player + 0x948) ? 0x27E8 : 0x27E9;
    break;
  case 0x27E3:
    next = 0x27E9;
    break;
  case 0x27E4:
  case 0x27E8:
    next = 0x27E5;
    break;
  case 0x27E9:
    write<u8>(a, 0x8B1, 1);
    status = 16;
    break;
  case 0x27EC:
    if (!load<u32>(player + 0x948)) {
      store<u32>(message, 0x27EE);
      call(0x025B7270, load<u32>(0x101F84DC) + 0x96);
      return status;
    }
    next = 0x27ED;
    break;
  case 0x27EE:
    next = 0x27EF;
    break;
  case 0x27EF:
    write<u8>(a, 0x8B1, 2);
    status = 16;
    break;
  case 0x1D35:
    next = 0x1D36;
    break;
  case 0x1D36:
    next = 0x1D37;
    break;
  case 0x1D37:
    next = manager && load<u8>(manager + 0x93F) ? 0x1D39 : 0x1D38;
    break;
  default:
    status = 16;
    break;
  }
  if (next)
    store<u32>(message, next);
  return status;
}
VERIFY(0x021ED964, auctionNextMessageStatus);

u32 auctionPhaseOne(Actor *a) {
  WWHD_FUNC(0x021EB8DC, u32, a);
  u32 flags = read<u32>(a, 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      auctionConstructor(a);
      flags = read<u32>(a, 0x2E4);
    }
    write<u32>(a, 0x2E4, flags | 8);
  }
  gabi::Local<be<u32>> managerOut;
  if (!call<u32>(0x025D5578, 0x17E, managerOut.get()))
    return 0;
  u32 manager = *managerOut;
  if (!manager)
    return 0;
  u8 kind = load<u8>(manager + 0x924 + auctionNpcNumber(a));
  if (kind >= 8) {
    call(0x0273AA24, 0x100166A0u, 0x429, 0x100166B4u);
    return 5;
  }
  write<u8>(a, 0x8AC, kind);
  u32 actorId = a ? read<u32>(a, 4) : 0xFFFFFFFF;
  u32 ownerSlot = manager + 0x850 + 4 * kind;
  if (load<u32>(ownerSlot) != 0xFFFFFFFF)
    return 0;
  store<u32>(ownerSlot, actorId);
  u32 low = load<u32>(0x100163CC + kind * 8);
  u32 high = load<u32>(0x100163D0 + kind * 8);
  u8 costume;
  if (kind >= 2 && kind <= 5) {
    u32 pairOffset = kind <= 3 ? 0x934 : 0x935;
    u8 previous = load<u8>((u32)*managerOut + pairOffset);
    costume = (u8)((u32)auctionRandom(a, (s32)(high - low + 1)) + low);
    manager = *managerOut;
    if (previous == 255) {
      store<u8>(manager + pairOffset, costume);
      manager = *managerOut;
    } else if (costume == load<u8>(manager + pairOffset)) {
      costume = (u8)(costume == (u8)high ? costume - 1 : costume + 1);
    }
  } else {
    costume = (u8)((u32)auctionRandom(a, (s32)(high - low + 1)) + low);
    manager = *managerOut;
  }
  write<u8>(a, 0x8AD, costume);
  store<u8>(manager + kind + 0x92C, costume);
  write<u32>(a, 0x89C, load<u32>(0x1001639C + 4 * costume));
  return 2;
}
VERIFY(0x021EB8DC, auctionPhaseOne);

namespace {
template <bool alternate> u32 auctionJointRotation(u32 jointObject, s32 phase) {
  if (phase != 0)
    return 1;
  u32 model = load<u32>(0x104B462C);
  Actor *a = gabi::at<Actor>(load<u32>(model + 0xB8));
  u32 jointData = call<u32>(0x027F7878, jointObject);
  u32 matrices = load<u32>(model + 0x2C);
  u16 joint = load<u16>(jointData + 4);
  u16 flags = load<u16>(matrices + 4);
  u32 base = load<u32>(matrices + 0x10);
  store<u16>(matrices + 4, flags | 0x10);
  call(0x028E90D4, base + joint * 0x30, load<u32>(0x1018C7B0));
  if ((u32)joint == (u32)(s32)read<s8>(a, 0x3B4)) {
    s16 angle = read<s16>(a, 0x3AE);
    call(alternate ? 0x025F1BF4 : 0x025F1C28, load<u32>(0x1018C7B0),
         alternate ? angle : (s16)-angle);
    angle = read<s16>(a, 0x3AC);
    call(0x025F1C5C, load<u32>(0x1018C7B0), (s16)-angle);
  }
  if ((u32)joint == (u32)(s32)read<s8>(a, 0x3B5)) {
    call(0x025F1BF4, load<u32>(0x1018C7B0), read<s16>(a, 0x3B2));
    s16 angle = read<s16>(a, 0x3B0);
    call(0x025F1C5C, load<u32>(0x1018C7B0), alternate ? (s16)-angle : angle);
  }
  matrices = load<u32>(model + 0x2C);
  u32 matrix = load<u32>(0x1018C7B0);
  flags = load<u16>(matrices + 4);
  base = load<u32>(matrices + 0x10);
  store<u16>(matrices + 4, flags | 0x10);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = load<f32>(matrix + 4 * i);
  for (u32 i = 0; i < 12; ++i)
    store<f32>(base + joint * 0x30 + 4 * i, values[i]);
  call(0x028E90D4, load<u32>(0x1018C7B0), 0x104B4868u);
  return 1;
}
} // namespace
u32 auctionJointCallback(u32 jointObject, s32 phase) {
  WWHD_FUNC(0x021EB068, u32, jointObject, phase);
  return auctionJointRotation<false>(jointObject, phase);
}
VERIFY(0x021EB068, auctionJointCallback);
u32 auctionAlternateJointCallback(u32 jointObject, s32 phase) {
  WWHD_FUNC(0x021EB1C0, u32, jointObject, phase);
  return auctionJointRotation<true>(jointObject, phase);
}
VERIFY(0x021EB1C0, auctionAlternateJointCallback);

u32 auctionDraw(Actor *a) {
  WWHD_FUNC(0x021ED628, u32, a);
  if (!auctionIsExecute(a))
    return 1;
  u32 model = load<u32>(read<u32>(a, 0x44C) + 0x90);
  u32 extra = read<u32>(a, 0x7F4);
  u32 material = load<u32>(model + 0xAC);
  if (extra)
    material = load<u32>(extra + 0xAC);
  u32 lighting = call<u32>(0x02555D0C);
  call(0x025626A4, lighting, 0, gabi::ea(a) + 0x314, gabi::ea(a) + 0x110);
  u32 morf = read<u32>(a, 0x44C);
  lighting = call<u32>(0x02555D0C);
  call(0x02562F5C, lighting, load<u32>(morf + 0x90), gabi::ea(a) + 0x110);
  if (extra) {
    lighting = call<u32>(0x02555D0C);
    call(0x02562F5C, lighting, read<u32>(a, 0x7F4), gabi::ea(a) + 0x110);
  }
  call(0x025E7B3C, gabi::ea(a) + 0x7FC, material, read<u8>(a, 0x8AB));
  call(0x025E54D8, read<u32>(a, 0x44C));
  if (extra) {
    s32 joint = read<s8>(a, 0x3B4);
    u32 matrices = load<u32>(model + 0x2C);
    u16 flags = load<u16>(matrices + 4);
    u32 matrix = load<u32>(matrices + 0x10) + (u32)joint * 0x30;
    extra = read<u32>(a, 0x7F4);
    store<u16>(matrices + 4, flags | 0x10);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
      values[i] = load<f32>(matrix + 4 * i);
    for (u32 i = 0; i < 12; ++i)
      store<f32>(extra + 0xC8 + 4 * i, values[i]);
    call(0x025E2DE0, read<u32>(a, 0x7F4), 0);
  }
  store<u32>(material + 0x38, 0);
  u8 costume = read<u8>(a, 0x8AD);
  call(0x025BED80, load<u8>(0x1001640C + costume), a, 1.0f, 1.0f, 1.0f);
  return 1;
}
VERIFY(0x021ED628, auctionDraw);

void auctionLookBack(Actor *a);
u32 auctionExecute(Actor *a) {
  WWHD_FUNC(0x021ED31C, u32, a);
  if (!auctionIsExecute(a))
    return 0;
  u32 parameters = 0x10465A58 + read<u8>(a, 0x8AD) * 0x34;
  s16 angles[9];
  for (u32 i = 0; i < 9; ++i)
    angles[i] = load<s16>(parameters + 4 + 2 * i);
  call(0x0259E08C, gabi::ea(a) + 0x3AC, angles[1], angles[3], angles[5],
       angles[7], angles[0], angles[2], angles[4], angles[6], angles[8]);
  auctionCheckOrder(a);
  u32 play = call<u32>(0x025200D4);
  bool action = !load<u8>(play + 0x5292);
  if (!action)
    action = read<u16>(a, 0xF8) == 1 && read<u8>(a, 0x8B0) != 0;
  if (action) {
    s32 offset = read<s16>(a, 0x7DC);
    s32 virtualIndex = read<s16>(a, 0x7DE);
    u32 receiver = gabi::ea(a) + (u32)offset;
    write<u8>(a, 0x8B5, 0);
    u32 target;
    if (virtualIndex < 0)
      target = read<u32>(a, 0x7E0);
    else {
      s32 tableOffset = read<s16>(a, 0x7E2);
      target = load<u32>(load<u32>(receiver + (u32)tableOffset) +
                         (u32)virtualIndex * 8 + 4);
    }
    call(target, receiver);
  } else
    auctionEventMove(a);
  auctionEventOrder(a);
  auctionPlayTexture(a);
  auctionPlayAnimation(a);
  play = call<u32>(0x025200D4);
  call(0x024F08A8, gabi::ea(a) + 0x450, play + 0x12A0);
  call(0x025A15AC, a, 60.0f, 150.0f);
  parameters = 0x10465A58 + read<u8>(a, 0x8AD) * 0x34;
  f32 x = read<f32>(a, 0x314), z = read<f32>(a, 0x31C), y = read<f32>(a, 0x318);
  f32 height = load<f32>(parameters + 0x18);
  write<f32>(a, 0x398, z);
  write<f32>(a, 0x390, x);
  write<f32>(a, 0x394, y + height);
  height = load<f32>(parameters + 0x28);
  write<f32>(a, 0x384, z);
  write<f32>(a, 0x37C, x);
  write<f32>(a, 0x380, y + height);
  auctionLookBack(a);
  auctionSetMatrix(a);
  s16 timer = read<s16>(a, 0x8A6);
  if (timer) {
    timer = (s16)(timer - 1);
    write<s16>(a, 0x8A6, timer);
    if (!timer)
      call(0x025E1988, read<u32>(a, 0x898));
  }
  parameters = 0x10465A58 + read<u8>(a, 0x8AD) * 0x34;
  write<f32>(a, 0x894, load<f32>(parameters + 0x30));
  return 0;
}
VERIFY(0x021ED31C, auctionExecute);

void auctionLookBack(Actor *a) {
  WWHD_FUNC(0x021ECF4C, void, a);
  u8 costume = read<u8>(a, 0x8AD);
  u32 parameters = 0x10465A58 + costume * 0x34;
  u8 emitterEnabled = read<u8>(a, 0x8B5);
  s16 facing = read<s16>(a, 0x322);
  f32 eyeHeight = load<f32>(parameters);
  s16 turn = load<s16>(parameters + 0x16);
  u32 track = 1;
  gabi::Local<cXyz> target;
  u32 targetAddress = 0;
  if (emitterEnabled) {
    gabi::Local<be<u16>> process;
    *process = 0x17E;
    u32 manager = call<u32>(0x025D5218, 0x025E121Cu, process.get());
    if (manager) {
      f32 ownX = read<f32>(a, 0x37C);
      f32 x = load<f32>(manager + 0x37C);
      f32 y = load<f32>(manager + 0x380);
      f32 z = load<f32>(manager + 0x384);
      if (ownX != x || read<f32>(a, 0x380) != y || read<f32>(a, 0x384) != z) {
        target->set(x, y, z);
        targetAddress = gabi::ea(target.get());
      }
    }
  } else if (read<u8>(a, 0x440)) {
    f32 x = read<f32>(a, 0x434), y = read<f32>(a, 0x438);
    write<u8>(a, 0x3B6, 1);
    f32 z = read<f32>(a, 0x43C);
    target->set(x, y, z);
    targetAddress = gabi::ea(target.get());
    track = 0;
    if (!read<u8>(a, 0x8A9))
      write<u8>(a, 0x8A9, 1);
  } else {
    u32 play = call<u32>(0x025200D4);
    u32 player = load<u32>(play + 0x5B34);
    gabi::Local<cXyz> actorPosition;
    actorPosition->set(read<f32>(a, 0x314), read<f32>(a, 0x318),
                       read<f32>(a, 0x31C));
    costume = read<u8>(a, 0x8AD);
    gabi::Local<cXyz> playerPosition;
    f32 px = load<f32>(player + 0x314), py = load<f32>(player + 0x318);
    f32 distanceLimit = read<f32>(a, 0x890);
    parameters = 0x10465A58 + costume * 0x34;
    f32 pz = load<f32>(player + 0x31C);
    playerPosition->set(px, py, pz);
    s32 angleLimit = load<s16>(parameters + 0x1C);
    s32 frontalLimit = 0x4000;
    gabi::Local<be<f32>> distance;
    gabi::Local<be<s16>> angle;
    call(0x0259D624, actorPosition.get(), playerPosition.get(), distance.get(),
         angle.get());
    u8 attentive = read<u8>(a, 0x8A9);
    s16 currentFacing = read<s16>(a, 0x322);
    s16 relative = (s16)((s32)(s16)*angle - currentFacing);
    f32 actualDistance = *distance;
    if (attentive) {
      distanceLimit += 40.0f;
      angleLimit += 0x71C;
      frontalLimit = 0x471C;
    }
    *angle = relative;
    // PPC ble branches whenever the greater-than bit is clear, including
    // unordered.
    if (distanceLimit > actualDistance) {
      s32 absolute = relative < 0 ? -(s32)relative : relative;
      if (frontalLimit > absolute) {
        gabi::Local<cXyz> eye;
        call(0x0259D54C, eye.get(), eyeHeight);
        target->copy(*eye);
        targetAddress = gabi::ea(target.get());
        track = 1;
        if (!read<u8>(a, 0x8A9))
          write<u8>(a, 0x8A9, 1);
      } else if (angleLimit > absolute) {
        u8 talking = read<u8>(a, 0x8A8);
        if (talking) {
          gabi::Local<cXyz> eye;
          call(0x0259D54C, eye.get(), eyeHeight);
          target->copy(*eye);
          targetAddress = gabi::ea(target.get());
          write<u8>(a, 0x3B6, 1);
          track = 0;
          attentive = read<u8>(a, 0x8A9);
        }
        if (!attentive)
          write<u8>(a, 0x8A9, 1);
      } else
        goto far;
    } else {
    far:
      u8 kind = read<u8>(a, 0x8AC);
      write<u8>(a, 0x8A9, 0);
      if (!kind) {
        facing = read<s16>(a, 0x2FA);
        track = 0;
        write<u8>(a, 0x3B6, 1);
      }
    }
  }
  if (read<u8>(a, 0x8A8)) {
    track = 0;
    write<u8>(a, 0x3B6, 1);
  }
  s16 smoothing = 0;
  if (read<u8>(a, 0x3B6)) {
    s16 overrideTurn = read<s16>(a, 0x442);
    if (overrideTurn)
      turn = overrideTurn;
    call(0x0200F428, gabi::ea(a) + 0x8A2, turn, 4, 0x800);
    smoothing = read<s16>(a, 0x8A2);
  } else
    write<s16>(a, 0x8A2, 0);
  gabi::Local<cXyz> origin;
  origin->set(read<f32>(a, 0x37C), read<f32>(a, 0x380), read<f32>(a, 0x384));
  call(0x0259DED0, gabi::ea(a) + 0x3AC, gabi::ea(a) + 0x322, targetAddress,
       origin.get(), facing, smoothing, track);
  u16 x = read<u16>(a, 0x320), y = read<u16>(a, 0x322), z = read<u16>(a, 0x324);
  write<u16>(a, 0x328, x);
  write<u16>(a, 0x32A, y);
  write<u16>(a, 0x32C, z);
}
VERIFY(0x021ECF4C, auctionLookBack);

void auctionStaticInitialize() {
  WWHD_FUNC(0x021EDDC8, void);
  store<u32>(0x10465A54, 0x0);
  store<u32>(0x10465A50, 0x0);
  store<u32>(0x10465A4C, 0x0);
  store<u32>(0x10465A48, 0x0);
  call(0x028F026C, 0x101BB7D8u);

  f32 constant_100167B0_1 = load<f32>(0x100167B0);
  f32 constant_100167B4_1 = load<f32>(0x100167B4);
  store<f32>(0x10465A3C, constant_100167B0_1);
  store<f32>(0x10465A40, constant_100167B4_1);
  call(0x028ED6F8, 0x10465A44u);

  call(0x028F026C, 0x101BB7E4u);

  call(0x028EAB2C, 0x10465A45u);

  call(0x028F026C, 0x101BB7F0u);

  call(0x0259DA18, 0x10465A58u);

  store<u16>(0x10465A5C, 0xFA0);
  store<u16>(0x10465A5E, 0x0);
  f32 constant_100167C4_1 = load<f32>(0x100167C4);
  store<u16>(0x10465A60, 0x1F40);
  store<u16>(0x10465A62, 0x1F40);
  store<u16>(0x10465A64, 0xF18C);
  f32 constant_100167B8_1 = load<f32>(0x100167B8);
  f32 constant_10016694_1 = load<f32>(0x10016694);
  store<f32>(0x10465A58, constant_10016694_1);
  f32 constant_100167C0_1 = load<f32>(0x100167C0);
  store<f32>(0x10465A70, constant_100167B8_1);
  store<u16>(0x10465A66, 0x0);
  store<u16>(0x10465A68, 0xE0C0);
  f32 constant_100167BC_1 = load<f32>(0x100167BC);
  store<f32>(0x10465A78, constant_10016694_1);
  store<f32>(0x10465A80, constant_100167BC_1);
  store<f32>(0x10465A84, constant_100167C0_1);
  store<f32>(0x10465A88, constant_100167C4_1);
  store<u8>(0x10465A76, 0x0);
  store<u16>(0x10465A6A, 0xE0C0);
  store<u16>(0x10465A6C, 0x5DC);
  store<u16>(0x10465A6E, 0x6A4);
  store<u32>(0x10465A7C, 0x1001632C);
  store<u16>(0x10465A74, 0x7FFF);
  call(0x0259DA18, 0x10465A8Cu);

  store<u16>(0x10465A90, 0xFA0);
  store<u16>(0x10465A92, 0x0);
  store<u32>(0x10465AB0, 0x1001632C);
  f32 constant_100167CC_1 = load<f32>(0x100167CC);
  store<f32>(0x10465A8C, constant_10016694_1);
  store<u16>(0x10465A94, 0xFA0);
  store<u16>(0x10465A96, 0x1B58);
  store<u16>(0x10465A98, 0xF060);
  store<u8>(0x10465AAA, 0x0);
  f32 constant_100166F0_1 = load<f32>(0x100166F0);
  store<u16>(0x10465A9A, 0x0);
  f32 constant_100167C8_1 = load<f32>(0x100167C8);
  store<u16>(0x10465A9C, 0xE4A8);
  store<u16>(0x10465A9E, 0xE0C0);
  store<u16>(0x10465AA0, 0x3E8);
  store<u16>(0x10465AA2, 0x5DC);
  store<u16>(0x10465AA8, 0x7FFF);
  store<f32>(0x10465AA4, constant_100167C8_1);
  store<f32>(0x10465AAC, constant_10016694_1);
  store<f32>(0x10465AB4, constant_100166F0_1);
  store<f32>(0x10465AB8, constant_100167CC_1);
  store<f32>(0x10465ABC, constant_100167C8_1);
  call(0x0259DA18, 0x10465AC0u);

  store<u16>(0x10465AC4, 0xFA0);
  store<u16>(0x10465AC6, 0x0);
  store<u16>(0x10465AC8, 0x1F40);
  store<u16>(0x10465ACA, 0x1F40);
  store<u16>(0x10465ACC, 0xE4A8);
  store<u16>(0x10465ACE, 0x0);
  store<u16>(0x10465AD0, 0xE0C0);
  store<u16>(0x10465AD2, 0xE0C0);
  store<u16>(0x10465AD4, 0x3E8);
  store<u16>(0x10465AD6, 0x514);
  store<u16>(0x10465ADC, 0x7FFF);
  f32 constant_100167D0_1 = load<f32>(0x100167D0);
  store<f32>(0x10465AC0, constant_10016694_1);
  store<f32>(0x10465AD8, constant_100167D0_1);
  store<u32>(0x10465AE4, 0x1001632C);
  store<f32>(0x10465AE0, constant_10016694_1);
  store<f32>(0x10465AE8, constant_100166F0_1);
  store<f32>(0x10465AEC, constant_10016694_1);
  store<u8>(0x10465ADE, 0x0);
  store<f32>(0x10465AF0, constant_100167D0_1);
  call(0x0259DA18, 0x10465AF4u);

  store<u16>(0x10465AF8, 0xFA0);
  store<u16>(0x10465AFA, 0x0);
  store<u16>(0x10465AFC, 0x1388);
  store<u32>(0x10465B18, 0x1001632C);
  store<u16>(0x10465AFE, 0x1F40);
  store<u16>(0x10465B00, 0xF254);
  store<u16>(0x10465B02, 0x0);
  store<f32>(0x10465AF4, constant_10016694_1);
  f32 constant_100167D4_1 = load<f32>(0x100167D4);
  store<u16>(0x10465B04, 0xEC78);
  store<f32>(0x10465B0C, constant_100167D4_1);
  store<f32>(0x10465B14, constant_10016694_1);
  store<f32>(0x10465B1C, constant_100166F0_1);
  store<f32>(0x10465B20, constant_10016694_1);
  store<f32>(0x10465B24, constant_100167D4_1);
  store<u16>(0x10465B06, 0xE0C0);
  store<u16>(0x10465B08, 0x3E8);
  store<u16>(0x10465B0A, 0x5DC);
  store<u8>(0x10465B12, 0x0);
  store<u16>(0x10465B10, 0x7FFF);
  call(0x0259DA18, 0x10465B28u);

  store<u16>(0x10465B2C, 0xFA0);
  store<u16>(0x10465B2E, 0x0);
  store<u16>(0x10465B30, 0x2328);
  store<u16>(0x10465B32, 0x1388);
  store<u16>(0x10465B34, 0xEC78);
  store<u8>(0x10465B46, 0x0);
  store<f32>(0x10465B28, constant_10016694_1);
  store<u16>(0x10465B36, 0x0);
  store<u16>(0x10465B38, 0xDCD8);
  store<f32>(0x10465B40, constant_100167D0_1);
  store<u16>(0x10465B3A, 0xEC78);
  store<f32>(0x10465B48, constant_10016694_1);
  store<f32>(0x10465B50, constant_100166F0_1);
  store<f32>(0x10465B54, constant_10016694_1);
  store<f32>(0x10465B58, constant_100167D0_1);
  store<u16>(0x10465B3C, 0x7D0);
  store<u16>(0x10465B3E, 0x5DC);
  store<u32>(0x10465B4C, 0x1001632C);
  store<u16>(0x10465B44, 0x7FFF);
  call(0x0259DA18, 0x10465B5Cu);

  store<u8>(0x10465B7A, 0x0);
  store<u16>(0x10465B60, 0x9C4);
  store<u16>(0x10465B62, 0x0);
  store<u16>(0x10465B64, 0x1B58);
  store<u16>(0x10465B66, 0x1B58);
  store<u32>(0x10465B80, 0x1001632C);
  store<u16>(0x10465B68, 0xFC18);
  store<u16>(0x10465B6A, 0x0);
  store<u16>(0x10465B6C, 0xE4A8);
  store<u16>(0x10465B6E, 0xE4A8);
  store<u16>(0x10465B70, 0x2BC);
  store<u16>(0x10465B72, 0x514);
  store<u16>(0x10465B78, 0x7FFF);
  f32 constant_100167D8_1 = load<f32>(0x100167D8);
  store<f32>(0x10465B5C, constant_10016694_1);
  store<f32>(0x10465B74, constant_100167D8_1);
  store<f32>(0x10465B7C, constant_10016694_1);
  f32 constant_100167DC_1 = load<f32>(0x100167DC);
  store<f32>(0x10465B84, constant_100166F0_1);
  store<f32>(0x10465B88, constant_100167DC_1);
  store<f32>(0x10465B8C, constant_100167D8_1);
  call(0x0259DA18, 0x10465B90u);

  store<f32>(0x10465B90, constant_10016694_1);
  f32 constant_100167E0_1 = load<f32>(0x100167E0);
  store<u8>(0x10465BAE, 0x0);
  store<f32>(0x10465BA8, constant_100167E0_1);
  store<u16>(0x10465B94, 0x9C4);
  store<u16>(0x10465B96, 0x0);
  store<u16>(0x10465B98, 0x1B58);
  store<u16>(0x10465B9A, 0x1B58);
  store<u16>(0x10465B9C, 0xFC18);
  store<u16>(0x10465B9E, 0x0);
  store<u16>(0x10465BA0, 0xE4A8);
  store<u16>(0x10465BA2, 0xE4A8);
  store<u16>(0x10465BA4, 0x2BC);
  store<u16>(0x10465BA6, 0x514);
  store<u32>(0x10465BB4, 0x1001632C);
  store<f32>(0x10465BB0, constant_10016694_1);
  store<f32>(0x10465BB8, constant_100166F0_1);
  store<f32>(0x10465BBC, constant_10016694_1);
  store<f32>(0x10465BC0, constant_100167E0_1);
  store<u16>(0x10465BAC, 0x7FFF);
  call(0x0259DA18, 0x10465BC4u);

  store<u16>(0x10465BC8, 0xFA0);
  store<u16>(0x10465BCA, 0x0);
  store<u8>(0x10465BE2, 0x0);
  store<u16>(0x10465BCC, 0x2328);
  store<f32>(0x10465BC4, constant_10016694_1);
  store<f32>(0x10465BDC, constant_100167D0_1);
  store<f32>(0x10465BE4, constant_10016694_1);
  store<f32>(0x10465BEC, constant_100166F0_1);
  store<f32>(0x10465BF0, constant_10016694_1);
  store<f32>(0x10465BF4, constant_100167D0_1);
  store<u16>(0x10465BCE, 0x1770);
  store<u16>(0x10465BD0, 0xFC18);
  store<u16>(0x10465BD2, 0x0);
  store<u16>(0x10465BD4, 0xDCD8);
  store<u16>(0x10465BD6, 0xE890);
  store<u16>(0x10465BD8, 0x7D0);
  store<u16>(0x10465BDA, 0x5DC);
  store<u32>(0x10465BE8, 0x1001632C);
  store<u16>(0x10465BE0, 0x7FFF);
  call(0x0259DA18, 0x10465BF8u);

  store<u16>(0x10465BFC, 0xFA0);
  store<u16>(0x10465BFE, 0x0);
  store<u16>(0x10465C00, 0x1F40);
  store<u16>(0x10465C02, 0x1388);
  store<u16>(0x10465C04, 0xF060);
  store<u16>(0x10465C06, 0x0);
  store<u16>(0x10465C08, 0xE0C0);
  store<u16>(0x10465C0A, 0xEC78);
  store<u16>(0x10465C0C, 0x7D0);
  store<u16>(0x10465C0E, 0x9C4);
  store<f32>(0x10465BF8, constant_10016694_1);
  store<f32>(0x10465C10, constant_100167C8_1);
  store<f32>(0x10465C18, constant_10016694_1);
  store<u8>(0x10465C16, 0x0);
  store<u16>(0x10465C14, 0x7FFF);
  f32 constant_100167C0_2 = load<f32>(0x100167C0);
  store<f32>(0x10465C20, constant_100166F0_1);
  store<f32>(0x10465C24, constant_100167C0_2);
  store<u32>(0x10465C1C, 0x1001632C);
  store<f32>(0x10465C28, constant_100167C8_1);
  call(0x0259DA18, 0x10465C2Cu);

  store<f32>(0x10465C2C, constant_10016694_1);
  f32 constant_100167E4_1 = load<f32>(0x100167E4);
  store<u8>(0x10465C4A, 0x0);
  store<f32>(0x10465C44, constant_100167E4_1);
  store<u16>(0x10465C30, 0x9C4);
  store<u16>(0x10465C32, 0x7D0);
  store<u16>(0x10465C34, 0x1770);
  store<u16>(0x10465C36, 0x1F40);
  store<u16>(0x10465C38, 0xFE0C);
  store<u16>(0x10465C3A, 0xF830);
  store<u16>(0x10465C3C, 0xE890);
  store<u16>(0x10465C3E, 0xE0C0);
  store<u16>(0x10465C40, 0x3E8);
  store<u16>(0x10465C42, 0x5DC);
  store<u32>(0x10465C50, 0x1001632C);
  store<f32>(0x10465C4C, constant_10016694_1);
  store<f32>(0x10465C54, constant_100167C8_1);
  store<f32>(0x10465C58, constant_10016694_1);
  store<f32>(0x10465C5C, constant_100167E4_1);
  store<u16>(0x10465C48, 0x7FFF);
  call(0x0259DA18, 0x10465C60u);

  store<u32>(0x10465C84, 0x1001632C);
  store<u16>(0x10465C64, 0x200);
  store<u16>(0x10465C66, 0x1388);
  store<u16>(0x10465C68, 0x200);
  store<u16>(0x10465C6A, 0x1770);
  f32 constant_100167F0_1 = load<f32>(0x100167F0);
  f32 constant_100167EC_1 = load<f32>(0x100167EC);
  f32 constant_100167E8_1 = load<f32>(0x100167E8);
  f32 constant_100167E0_2 = load<f32>(0x100167E0);
  store<f32>(0x10465C60, constant_100167E8_1);
  store<f32>(0x10465C78, constant_100167E0_2);
  store<f32>(0x10465C80, constant_100167EC_1);
  store<f32>(0x10465C88, constant_100167F0_1);
  store<f32>(0x10465C8C, constant_10016694_1);
  store<u16>(0x10465C6C, 0xFE00);
  store<u16>(0x10465C6E, 0xEC78);
  store<u16>(0x10465C70, 0xFE00);
  f32 constant_100167F4_1 = load<f32>(0x100167F4);
  store<u8>(0x10465C7E, 0x0);
  store<u16>(0x10465C72, 0xE890);
  store<u16>(0x10465C74, 0x1000);
  store<u16>(0x10465C76, 0x800);
  store<u16>(0x10465C7C, 0x7FFF);
  store<f32>(0x10465C90, constant_100167F4_1);
  call(0x0259DA18, 0x10465C94u);

  store<u16>(0x10465C98, 0x200);
  store<u16>(0x10465C9A, 0x1388);
  store<u16>(0x10465C9C, 0x200);
  store<u16>(0x10465C9E, 0x1770);
  store<u16>(0x10465CA0, 0xFE00);
  store<u16>(0x10465CA2, 0xEC78);
  store<u16>(0x10465CA4, 0xFE00);
  store<u16>(0x10465CA6, 0xE890);
  store<u16>(0x10465CA8, 0x1000);
  store<u16>(0x10465CAA, 0x800);
  store<u16>(0x10465CB0, 0x7FFF);
  f32 constant_100167E8_2 = load<f32>(0x100167E8);
  f32 constant_100167F8_1 = load<f32>(0x100167F8);
  store<f32>(0x10465C94, constant_100167E8_2);
  store<f32>(0x10465CAC, constant_100167F8_1);
  f32 constant_100167F0_2 = load<f32>(0x100167F0);
  store<f32>(0x10465CB4, constant_10016694_1);
  store<f32>(0x10465CBC, constant_100167F0_2);
  store<f32>(0x10465CC0, constant_10016694_1);
  store<u32>(0x10465CB8, 0x1001632C);
  f32 constant_100167F4_2 = load<f32>(0x100167F4);
  store<u8>(0x10465CB2, 0x0);
  store<f32>(0x10465CC4, constant_100167F4_2);
}
VERIFY(0x021EDDC8, auctionStaticInitialize);
