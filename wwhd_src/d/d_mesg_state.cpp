// HD message state, tagged-string expansion and scope transitions.
// Owned native entries 025F85A8..025F8EC4 exclusive; controller and
// registration separate.
#include "gabi.h"
using namespace gabi;
namespace d_mesg_state_cpp {
static void ensure(u32 object) {
  call_ptr<void>(load<u32>(load<u32>(object + 4) + 0x14), object);
}
static s32 length(u32 p, u32 stride) {
  s32 n = 0;
  if (stride == 1 ? load<u8>(p) == 0 : load<u16>(p) == 0)
    return 0;
  do {
    ++n;
    p += stride;
    if (n > 0x40000)
      return 0;
  } while (stride == 1 ? load<u8>(p) != 0 : load<u16>(p) != 0);
  return n;
}
static void descriptor(u32 p, u32 buffer, u32 vtable, u32 capacity) {
  store<u32>(p, buffer);
  store<u32>(p + 4, vtable);
  store<u32>(p + 8, capacity);
}
void paneAction(void *self) {
  WWHD_FUNC(0x025F85A8, void, self);
  call<void>(0x026AE410, load<u32>(ea(self) + 0x960));
}
VERIFY(0x025F85A8, paneAction);
void textAction(void *self) {
  WWHD_FUNC(0x025F85B0, void, self);
  call<void>(0x02651EE4, load<u32>(ea(self) + 0x964));
}
VERIFY(0x025F85B0, textAction);
void textAdvance(void *self) {
  WWHD_FUNC(0x025F85B8, void, self);
  call<void>(0x02652020, load<u32>(ea(self) + 0x964));
}
VERIFY(0x025F85B8, textAdvance);
u32 textReset(void *self) {
  WWHD_FUNC(0x025F85C0, u32, self);
  return call<u32>(0x026521B8, load<u32>(ea(self) + 0x964));
}
VERIFY(0x025F85C0, textReset);
void positionActor(void *self, void *actor) {
  WWHD_FUNC(0x025F85C8, void, self, actor);
  u32 pane = load<u32>(load<u32>(ea(self) + 0x964) + 0x54);
  float x = load<float>(pane + 0x3c) + load<float>(0x100E0C98);
  float limit = load<float>(0x100E0C9C);
  u32 y = load<u32>(ea(actor) + 0x40);
  u8 flags = load<u8>(ea(actor) + 0x44);
  if (!(x >= limit))
    x = limit;
  store<float>(ea(actor) + 0x3c, x);
  store<u32>(ea(actor) + 0x40, y);
  store<u8>(ea(actor) + 0x44, flags | 0x10);
}
VERIFY(0x025F85C8, positionActor);
void setContext(void *self, u32 context) {
  WWHD_FUNC(0x025F8610, void, self, context);
  store<u32>(ea(self) + 0x970, context);
}
VERIFY(0x025F8610, setContext);
void playerName(void *self, void *destination) {
  WWHD_FUNC(0x025F8618, void, self, destination);
  Local<u8[32]> narrow;
  Local<u32[2]> source;
  u32 d = ea(destination);
  descriptor(narrow.a, narrow.a + 12, 0x100E0C34, 20);
  store<u8>(narrow.a + 12, 0);
  store<u8>(narrow.a + 31, 0);
  u32 name = call<u32>(0x02720170, load<u32>(0x101F84DC) + 0x12c0);
  ensure(name + 0x20);
  store<u32>(source.a, load<u32>(name + 0x20));
  store<u32>(source.a + 4, 0x100E0AE4);
  call<void>(0x025F8F80, source.a);
  s32 n = length(load<u32>(source.a), 1);
  u32 capacity = load<u32>(narrow.a + 8);
  if (n >= (s32)capacity)
    n = capacity - 1;
  ensure(source.a);
  call<void>(0xC0009988, narrow.a + 12, load<u32>(source.a), (u32)n, 0);
  store<u8>(narrow.a + 12 + n, 0);
  ensure(d);
  u32 out = load<u32>(d);
  ensure(narrow.a);
  call<u32>(0x0275BAA0, out, load<u32>(d + 8), load<u32>(narrow.a), 0xffffffff);
}
VERIFY(0x025F8618, playerName);
void expandTagged(void *self, void *destination, u32 input, u32 context) {
  WWHD_FUNC(0x025F8780, void, self, destination, input, context);
  u32 d = ea(destination);
  u16 token = load<u16>(input);
  while (token) {
    if (token == 14 || token == 15) {
      u32 tag = input;
      input += token == 14 ? load<u16>(tag + 6) + 8 : 6;
      if (load<u16>(tag + 2) == 2 && load<u16>(tag + 4) == 0) {
        Local<u8[52]> name;
        descriptor(name.a, name.a + 12, 0x100E0C1C, 20);
        store<u16>(name.a + 12, 0);
        store<u16>(name.a + 50, 0);
        call<void>(0x025F8618, ea(self), name.a, context);
        u32 out = load<u32>(d);
        ensure(d);
        s32 current = length(load<u32>(d), 2);
        s32 offset = current < 0 ? 0 : current;
        ensure(name.a);
        s32 n = length(load<u32>(name.a), 2);
        s32 remaining = (s32)(load<u32>(d + 8) - (u32)offset);
        if (n >= remaining)
          n = remaining - 1;
        if (n > 0) {
          u32 dst = out + (u32)offset * 2;
          ensure(name.a);
          call<void>(0xC0009988, dst, load<u32>(name.a), (u32)n * 2, 0);
          if ((s32)((u32)offset + (u32)n) > current)
            store<u16>(out + ((u32)offset + (u32)n) * 2, 0);
        }
      }
    } else {
      ensure(d);
      u32 out = load<u32>(d);
      s32 n = length(out, 2);
      if (n < (s32)(load<u32>(d + 8) - 1)) {
        store<u16>(out + (u32)n * 2, token);
        store<u16>(out + (u32)n * 2 + 2, 0);
      }
      input += 2;
    }
    token = load<u16>(input);
  }
}
VERIFY(0x025F8780, expandTagged);
void lookupText(void *self, void *destination, u32 id) {
  WWHD_FUNC(0x025F8A08, void, self, destination, id);
  Local<u8[268]> key;
  descriptor(key.a, key.a + 12, 0x100E0B74, 256);
  store<u8>(key.a + 12, 0);
  store<u8>(key.a + 267, 0);
  call<void>(0x02759C28, key.a, 0x100E0CA0, id & 0xffff);
  u32 table = call<u32>(0x025F4FC0, load<u32>(0x101F4AE8), key.a);
  if (!table)
    return;
  ensure(key.a);
  u32 index = call<u32>(0x0273A1CC, load<u32>(table), load<u32>(key.a));
  if (index >= 0xfffffffe)
    return;
  u32 text = 0;
  if (index < load<u32>(table + 4))
    text = call<u32>(0x0273A2E8, load<u32>(table), index);
  Local<u8[46]> expanded;
  descriptor(expanded.a, expanded.a + 12, 0x100E0BD4, 17);
  store<u16>(expanded.a + 12, 0);
  store<u16>(expanded.a + 44, 0);
  call<void>(0x025F8780, ea(self), expanded.a, text, id);
  ensure(expanded.a);
  u32 p = load<u32>(expanded.a), out = ea(destination), n = 0;
  while (n < 16) {
    u16 c = load<u16>(p);
    if (!c)
      break;
    store<u16>(out, c);
    ++n;
    out += 2;
    p += 2;
  }
  store<u16>(out, 0);
}
VERIFY(0x025F8A08, lookupText);
u32 enterScope() {
  WWHD_FUNC(0x025F8B5C, u32);
  u32 g = call<u32>(0x025200D4);
  if ((load<u32>(g + 0x5cd8) & 0x200000) != 0) {
    g = call<u32>(0x025200D4);
    if (load<u8>(g + 0x5bb3) == 11) {
      g = call<u32>(0x025200D4);
      if (load<u8>(g + 0x5292) == 0) {
        g = call<u32>(0x025200D4);
        store<u8>(g + 0x5bb3, 13);
        return 1;
      }
    }
  }
  g = call<u32>(0x025200D4);
  if (load<u8>(g + 0x5bb3) == 17) {
    g = call<u32>(0x025200D4);
    store<u8>(g + 0x5bb2, 13);
    return 1;
  }
  return 0;
}
VERIFY(0x025F8B5C, enterScope);
u32 releaseScope() {
  WWHD_FUNC(0x025F8BEC, u32);
  u32 g = call<u32>(0x025200D4);
  if (load<u8>(g + 0x5bb3) != 13)
    return 0;
  g = call<u32>(0x025200D4);
  store<u8>(g + 0x5bb3, 11);
  return 1;
}
VERIFY(0x025F8BEC, releaseScope);
u32 taggedLength(void *self, u32 input) {
  WWHD_FUNC(0x025F8C38, u32, self, input);
  u32 n = 0;
  u16 token = load<u16>(input);
  while (token) {
    if (token == 14) {
      u32 skip = load<u16>(input + 6) + 8;
      input += skip;
      n += (u32)((s32)skip >> 1);
    } else if (token == 15) {
      input += 6;
      n += 3;
    } else {
      input += 2;
      ++n;
    }
    token = load<u16>(input);
  }
  return n;
}
VERIFY(0x025F8C38, taggedLength);
u32 copyTagged(void *self, void *destination, u32 input) {
  WWHD_FUNC(0x025F8CDC, u32, self, destination, input);
  u32 count = call<u32>(0x025F8C38, ea(self), input);
  u32 d = ea(destination), out = load<u32>(d);
  Local<u32[2]> source;
  store<u32>(source.a, input);
  store<u32>(source.a + 4, 0x100E0B14);
  s32 n = (s32)count;
  if (n < 0) {
    call<void>(0x025F8FB0, source.a);
    n = length(load<u32>(source.a), 2);
  }
  if (n >= (s32)load<u32>(d + 8))
    n = load<u32>(d + 8) - 1;
  ensure(source.a);
  call<void>(0xC0009988, out, load<u32>(source.a), (u32)n * 2, 0);
  store<u16>(out + (u32)n * 2, 0);
  return count;
}
VERIFY(0x025F8CDC, copyTagged);
u32 ready(void *self) {
  WWHD_FUNC(0x025F8DD0, u32, self);
  if (load<u32>(ea(self) + 0x94c) != 0)
    return 0;
  return call<u32>(0x025F795C, ea(self)) != 0;
}
VERIFY(0x025F8DD0, ready);
void paneReset(void *self) {
  WWHD_FUNC(0x025F8E18, void, self);
  u32 p = load<u32>(ea(self) + 0x960);
  if (p)
    call<void>(0x026AE41C, p);
}
VERIFY(0x025F8E18, paneReset);
void destroyHandler(void *self, u32 flags) {
  WWHD_FUNC(0x025F8E28, void, self, flags);
  u32 p = ea(self);
  if (!p)
    return;
  store<u32>(p + 12, 0x100E0CD0);
  if (p == load<u32>(0x101F4B60)) {
    u32 owner = load<u32>(0x101F4B5C);
    store<u32>(0x101F4B60, 0);
    call<void>(0x025F76EC, owner, 2);
    store<u32>(0x101F4B5C, 0);
  }
  call<void>(0x02752BEC, p, 0);
  if (flags & 1)
    call<void>(0x0273AF40, p);
}
VERIFY(0x025F8E28, destroyHandler);
} // namespace d_mesg_state_cpp
