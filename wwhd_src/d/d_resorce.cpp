// HD resource manager; inventory and reconstruction in progress.
#include "bindings.h"
namespace d_resorce_cpp {
static void ControlDestructor(u32 self, u32 flags) {
 WWHD_FUNC(0x02603720, void, self, flags);
 if (!self) return;
 gabi::store<u32>(self, 0x100E1A5C);
 if (flags & 1) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x02603720, ControlDestructor);
static u32 GetIndex(u32 info, u32 index) {
 WWHD_FUNC(0x026066A0, u32, info, index);
 if (s32(index) >= gabi::load<s32>(info + 0x84)) return 0;
 u32 res = gabi::load<u32>(info + 0x60);
 return gabi::load<u32>(res + index * 4);
}
VERIFY(0x026066A0, GetIndex);
static u32 GetId(u32 info, u32 id) {
 WWHD_FUNC(0x026067D0, u32, info, id);
 if (s32(id) >= gabi::load<s32>(info + 0x88)) return 0;
 u32 res = gabi::load<u32>(info + 0x64);
 return gabi::load<u32>(res + id * 4);
}
VERIFY(0x026067D0, GetId);
static void Delete_0260A810(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A810, void, self, flags);
 if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A810, Delete_0260A810);
static void Delete_0260A840(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A840, void, self, flags);
 if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A840, Delete_0260A840);
static void Delete_0260A854(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A854, void, self, flags);
 if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A854, Delete_0260A854);
static void Delete_0260A868(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A868, void, self, flags);
 if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A868, Delete_0260A868);
static void Delete_0260A87C(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A87C, void, self, flags);
 if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A87C, Delete_0260A87C);
static void Delete_0260A890(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A890, void, self, flags);
 if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A890, Delete_0260A890);
static void Empty_0260A824(u32 self) { WWHD_FUNC(0x0260A824, void, self); }
VERIFY(0x0260A824, Empty_0260A824);
static void Empty_0260ABF8(u32 self) { WWHD_FUNC(0x0260ABF8, void, self); }
VERIFY(0x0260ABF8, Empty_0260ABF8);
static void Empty_0260AC3C(u32 self) { WWHD_FUNC(0x0260AC3C, void, self); }
VERIFY(0x0260AC3C, Empty_0260AC3C);
static void Release_0260ABFC(u32 self) {
 WWHD_FUNC(0x0260ABFC, void, self);
 u32 pool = gabi::load<u32>(self + 0x1C);
 gabi::store<u32>(self, gabi::load<u32>(pool + 4));
 u32 count = gabi::load<u32>(pool + 12);
 gabi::store<u32>(pool + 4, self);
 gabi::store<u32>(pool + 12, count - 1);
}
VERIFY(0x0260ABFC, Release_0260ABFC);
static void Release_0260AC1C(u32 self) {
 WWHD_FUNC(0x0260AC1C, void, self);
 u32 pool = gabi::load<u32>(self + 0x1C);
 gabi::store<u32>(self, gabi::load<u32>(pool + 4));
 u32 count = gabi::load<u32>(pool + 12);
 gabi::store<u32>(pool + 4, self);
 gabi::store<u32>(pool + 12, count - 1);
}
VERIFY(0x0260AC1C, Release_0260AC1C);
static void Release_0260AC40(u32 self) {
 WWHD_FUNC(0x0260AC40, void, self);
 u32 pool = gabi::load<u32>(self + 0x1C);
 gabi::store<u32>(self, gabi::load<u32>(pool + 4));
 u32 count = gabi::load<u32>(pool + 12);
 gabi::store<u32>(pool + 4, self);
 gabi::store<u32>(pool + 12, count - 1);
}
VERIFY(0x0260AC40, Release_0260AC40);
static void Terminate(u32 str) {
 WWHD_FUNC(0x0260A828, void, str);
 u32 data = gabi::load<u32>(str);
 u32 size = gabi::load<u32>(str + 8);
 gabi::store<u8>(data + size - 1, 0);
}
VERIFY(0x0260A828, Terminate);
static void Set_026082D0(u32 info, s32 index, u32 value) {
 WWHD_FUNC(0x026082D0, void, info, index, value);
 s32 count = gabi::load<s32>(info + 132);
 if (index < count) gabi::store<u32>(gabi::load<u32>(info + 96) + u32(index) * 4, value);
}
VERIFY(0x026082D0, Set_026082D0);
static void Set_026082EC(u32 info, s32 index, u32 value) {
 WWHD_FUNC(0x026082EC, void, info, index, value);
 s32 count = gabi::load<s32>(info + 136);
 if (index < count) gabi::store<u32>(gabi::load<u32>(info + 100) + u32(index) * 4, value);
}
VERIFY(0x026082EC, Set_026082EC);
static u32 Initialize(u32 heap) {
 WWHD_FUNC(0x02603680, u32, heap);
 u32 self = gabi::load<u32>(0x101F4F28);
 if (!self) {
  u32 allocated = gabi::call<u32>(0x0273B0D4, 0x2130u, heap, 4u);
  u32 disposer = allocated + 4;
  if (disposer) {
   gabi::call<void>(0x02752B0C, disposer, heap, 3u);
   gabi::store<u32>(disposer + 12, 0x100E1DA0);
  }
  gabi::store<u32>(0x101F4F2C, disposer);
  self = allocated ? gabi::call<u32>(0x0260344C, allocated) : allocated;
  gabi::store<u32>(0x101F4F28, self);
 }
 return self;
}
VERIFY(0x02603680, Initialize);
static void DisposeSingleton(u32 self, u32 flags) {
 WWHD_FUNC(0x02608198, void, self, flags);
 if (!self) return;
 gabi::store<u32>(self + 12, 0x100E1DA0);
 if (self == gabi::load<u32>(0x101F4F2C)) {
  u32 control = gabi::load<u32>(0x101F4F28);
  gabi::store<u32>(0x101F4F2C, 0);
  gabi::call<void>(0x02603720, control, 2u);
  gabi::store<u32>(0x101F4F28, 0);
 }
 gabi::call<void>(0x02752BEC, self, 0u);
 if (flags & 1) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x02608198, DisposeSingleton);
static void Dispose_0260827C(u32 self, u32 flags) {
 WWHD_FUNC(0x0260827C, void, self, flags);
 if (!self) return;
 gabi::call<void>(0x02752BEC, self, 0u);
 if (flags & 1) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260827C, Dispose_0260827C);
static void Dispose_0260A728(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A728, void, self, flags);
 if (!self) return;
 gabi::call<void>(0x02752BEC, self, 0u);
 if (flags & 1) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A728, Dispose_0260A728);
static void Delete_0260A9C4(u32 self, u32 flags) {
 WWHD_FUNC(0x0260A9C4, void, self, flags);
 if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0260A9C4, Delete_0260A9C4);
static void Transfer_02608234(u32 first, u32 unused, u32 second) {
 WWHD_FUNC(0x02608234, void, first, unused, second);
 u32 from = gabi::load<u32>(gabi::load<u32>(gabi::load<u32>(second) + 0x5C) + 0x14);
 u32 root = gabi::load<u32>(first);
 if (!from) return;
 u32 to = gabi::load<u32>(root + 0x14);
 gabi::call<void>(0x027E2EA4, to, from, 1u);
}
VERIFY(0x02608234, Transfer_02608234);
static void Transfer_02608258(u32 first, u32 unused, u32 second) {
 WWHD_FUNC(0x02608258, void, first, unused, second);
 u32 root = gabi::load<u32>(first);
 u32 from = gabi::load<u32>(root + 0x14);
 u32 archive = gabi::load<u32>(gabi::load<u32>(second) + 0x5C);
 if (!from) return;
 u32 to = gabi::load<u32>(archive + 0x14);
 gabi::call<void>(0x027E2EA4, to, from, 1u);
}
VERIFY(0x02608258, Transfer_02608258);
static void Adapter_0260AC60(u32 self, u32 node) {
 WWHD_FUNC(0x0260AC60, void, self, node);
 u32 key = gabi::load<u32>(node + 0x14);
 u32 root = gabi::load<u32>(self);
 gabi::Local<be<u32>> copy;
 *copy = key;
 gabi::call<void>(0x02608234, root, gabi::ea(copy.get()), node + 0x18);
}
VERIFY(0x0260AC60, Adapter_0260AC60);
static void Adapter_0260AC94(u32 self, u32 node) {
 WWHD_FUNC(0x0260AC94, void, self, node);
 u32 key = gabi::load<u32>(node + 0x14);
 u32 root = gabi::load<u32>(self);
 gabi::Local<be<u32>> copy;
 *copy = key;
 gabi::call<void>(0x02608258, root, gabi::ea(copy.get()), node + 0x18);
}
VERIFY(0x0260AC94, Adapter_0260AC94);
static u32 Find(u32 tree, u32 node, u32 key) {
 WWHD_FUNC(0x0260ACC8, u32, tree, node, key);
 if (node) {
  u32 want = gabi::load<u32>(key);
  do {
   u32 value = gabi::load<u32>(node + 12);
   if (value < want) node = gabi::load<u32>(node + 4);
   else if (value > want) node = gabi::load<u32>(node);
   else break;
  } while (node);
 }
 return node;
}
VERIFY(0x0260ACC8, Find);
static u32 CallbackCtor_0260B698(u32 self, u32 receiver, u32 data) {
 WWHD_FUNC(0x0260B698, u32, self, receiver, data);
 if (!self) self = gabi::call<u32>(0x0273AD10, 16u);
 if (self) {
  gabi::store<u32>(self + 4, receiver);
  u32 word = gabi::load<u32>(data);
  gabi::store<u32>(self + 8, word);
  word = gabi::load<u32>(data + 4);
  gabi::store<u32>(self, 269359796u);
  gabi::store<u32>(self + 12, word);
 }
 return self;
}
VERIFY(0x0260B698, CallbackCtor_0260B698);
static u32 CallbackCtor_0260B704(u32 self, u32 receiver, u32 data) {
 WWHD_FUNC(0x0260B704, u32, self, receiver, data);
 if (!self) self = gabi::call<u32>(0x0273AD10, 16u);
 if (self) {
  gabi::store<u32>(self + 4, receiver);
  u32 word = gabi::load<u32>(data);
  gabi::store<u32>(self + 8, word);
  word = gabi::load<u32>(data + 4);
  gabi::store<u32>(self, 269359812u);
  gabi::store<u32>(self + 12, word);
 }
 return self;
}
VERIFY(0x0260B704, CallbackCtor_0260B704);
static u32 ControlCtor(u32 self) {
 WWHD_FUNC(0x0260344C, u32, self);
 if (!self) self = gabi::call<u32>(0x0273AD10, 0x2130u);
 if (!self) return self;
 gabi::store<u32>(self, 0x100E1A5C);
 for (u32 offset : {0x14u, 0x1028u}) {
  u32 pool = self + offset;
  if (!pool) pool = gabi::call<u32>(0x0273AD10, 0x1014u);
  if (pool) {
   u32 data = pool + 20;
   gabi::store<u32>(pool, 0);
   gabi::store<u32>(pool + 12, 0);
   gabi::store<u32>(pool + 4, data);
   gabi::store<u32>(pool + 16, 128);
   for (u32 i = 0; i < 127; ++i) gabi::store<u32>(data + i * 32, data + (i + 1) * 32);
   gabi::store<u32>(data + 127 * 32, 0);
   gabi::store<u32>(pool + 8, data);
  }
 }
 for (u32 off : {0x2048u,0x203Cu,0x2040u,0x204Cu,0x2050u,0x2054u,0x2044u}) gabi::store<u32>(self + off, 0);
 u32 first = self + 0x2058, second = self + 0x2068, third = self + 0x2078, fourth = self + 0x20D0;
 if (!first) gabi::call<u32>(0x0273AD10, 16u);
 if (!second) gabi::call<u32>(0x0273AD10, 16u);
 if (!third) gabi::call<u32>(0x0273AD10, 88u);
 if (!fourth) gabi::call<u32>(0x0273AD10, 88u);
 gabi::store<u32>(self + 0x2128, 0);
 gabi::store<u8>(self + 0x212C, 0);
 for (u32 i=0; i<4; ++i) gabi::store<u32>(first+i*4,0);
 for (u32 i=0; i<4; ++i) gabi::store<u32>(second+i*4,0);
 for (u32 i=0; i<22; ++i) gabi::store<u32>(third+i*4,0);
 for (u32 i=0; i<22; ++i) gabi::store<u32>(fourth+i*4,0);
 return self;
}
VERIFY(0x0260344C, ControlCtor);
static void AllocatePool(u32 self, s32 count, u32 heap) {
 WWHD_FUNC(0x026055E4, void, self, count, heap);
 if (count <= 0) return;
 u32 data = gabi::call<u32>(0x0273B0D4, u32(count) * 32, heap, 4u);
 if (!data) return;
 gabi::store<u32>(self + 0xBC, count);
 gabi::store<u32>(self + 0xB0, data);
 for (s32 i=0; i<count-1; ++i) gabi::store<u32>(data + u32(i)*32, data + u32(i+1)*32);
 gabi::store<u32>(data + u32(count)*32 - 32, 0);
 gabi::store<u32>(self + 0xB4, data);
}
VERIFY(0x026055E4, AllocatePool);
static void StringAssure(u32 str) {
 u32 vt = gabi::load<u32>(str + 4);
 gabi::call_ptr<void>(gabi::load<u32>(vt + 20), str);
}
static u32 StringHash(u32 str) {
 StringAssure(str);
 u32 saved = gabi::load<u32>(str);
 StringAssure(str);
 u32 data = gabi::load<u32>(str);
 u32 length = 0;
 while (gabi::load<u8>(data + length)) {
  ++length;
  if (length > 0x40000) { length = 0; break; }
 }
 return gabi::call<u32>(0x0273B264, saved, length);
}
static u32 Lookup_026066C4(u32 self, u32 name, u32 index) {
 WWHD_FUNC(0x026066C4, u32, self, name, index);
 u32 hash = StringHash(name);
 u32 root = gabi::load<u32>(self + 20);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 20u, root, key.get());
 if (!node || !(node + 24)) return 0;
 u32 info = gabi::load<u32>(node + 24);
 return gabi::call<u32>(0x026066A0, info, index);
}
VERIFY(0x026066C4, Lookup_026066C4);
static u32 Lookup_026067F4(u32 self, u32 name, u32 index) {
 WWHD_FUNC(0x026067F4, u32, self, name, index);
 u32 hash = StringHash(name);
 u32 root = gabi::load<u32>(self + 20);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 20u, root, key.get());
 if (!node || !(node + 24)) return 0;
 u32 info = gabi::load<u32>(node + 24);
 return gabi::call<u32>(0x026067D0, info, index);
}
VERIFY(0x026067F4, Lookup_026067F4);
static u32 Lookup_02606AF8(u32 self, u32 name) {
 WWHD_FUNC(0x02606AF8, u32, self, name);
 u32 hash = StringHash(name);
 u32 root = gabi::load<u32>(self + 172);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 172u, root, key.get());
 if (!node || !(node + 24)) return 0;
 u32 info = gabi::load<u32>(node + 24);
 return info;
}
VERIFY(0x02606AF8, Lookup_02606AF8);
static u32 Lookup_0260524C(u32 self, u32 name) {
 WWHD_FUNC(0x0260524C, u32, self, name);
 u32 hash = StringHash(name);
 u32 root = gabi::load<u32>(self + 4136);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 4136u, root, key.get());
 if (!node || !(node + 24)) return 0;
 u32 info = gabi::load<u32>(node + 24);
 return gabi::load<u32>(info + 0xA8);
}
VERIFY(0x0260524C, Lookup_0260524C);
static u32 GetArchive(u32 self, u32 name) {
 WWHD_FUNC(0x026065A8, u32, self, name);
 u32 hash = StringHash(name);
 u32 root = gabi::load<u32>(self + 20);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 20, root, key.get());
 if (!node || !(node + 24)) return 0;
 u32 info = gabi::load<u32>(node + 24);
 return gabi::load<u32>(info + 0x5C);
}
VERIFY(0x026065A8, GetArchive);
static u32 LoadResourceIndex(u32 self, u32 index, u32 name) {
 WWHD_FUNC(0x0260702C, u32, self, index, name);
 u32 hash = StringHash(name);
 u32 root = gabi::load<u32>(self + 20);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 20, root, key.get());
 if (!node || !(node + 24)) return 0;
 u32 info = gabi::load<u32>(node + 24);
 return gabi::call<u32>(0x02606D74, info, index);
}
VERIFY(0x0260702C, LoadResourceIndex);
static void StaticInit() {
 WWHD_FUNC(0x0260A77C, void);
 gabi::store<u32>(0x1048DD28, 0);
 gabi::store<u32>(0x1048DD20, 0);
 gabi::store<u32>(0x1048DD2C, 0);
 gabi::store<u32>(0x1048DD24, 0);
 gabi::call<void>(0x028F026C, 0x101F4F04u);
 f32 a = gabi::load<f32>(0x100E1C34);
 f32 b = gabi::load<f32>(0x100E1C38);
 gabi::store<f32>(0x1048DD14, a);
 gabi::store<f32>(0x1048DD18, b);
 gabi::call<void>(0x028ED6F8, 0x1048DD1Cu);
 gabi::call<void>(0x028F026C, 0x101F4F10u);
 gabi::call<void>(0x028EAB2C, 0x1048DD1Du);
 gabi::call<void>(0x028F026C, 0x101F4F1Cu);
}
VERIFY(0x0260A77C, StaticInit);
static u32 Length(u32 data) {
 u32 n=0;
 while (gabi::load<u8>(data+n)) {
  if (++n > 0x40000) return 0;
 }
 return n;
}
struct StringBuffer128 { be<u32> data, vtable, capacity; u8 bytes[128]; };
struct SafeString { be<u32> data, vtable; };
static u32 ResourceByFilename(u32 info, u32 name) {
 WWHD_FUNC(0x02606D74, u32, info, name);
 gabi::Local<StringBuffer128> buffer;
 u32 str = gabi::ea(buffer.get()), data = str + 12;
 gabi::store<u32>(str + 4, 0x100E1A14);
 gabi::store<u8>(data + 127, 0);
 gabi::store<u32>(str + 8, 128);
 gabi::store<u32>(str, data);
 StringAssure(name);
 u32 n = Length(gabi::load<u32>(name));
 u32 capacity = gabi::load<u32>(str + 8);
 u32 sourceVt = gabi::load<u32>(name + 4);
 u32 target = gabi::load<u32>(sourceVt + 20);
 if (s32(n) >= s32(capacity)) n = capacity - 1;
 gabi::call_ptr<void>(target, name);
 u32 source = gabi::load<u32>(name);
 gabi::call<void>(0xC0009988, data, source, n, 0u);
 gabi::store<u8>(data + n, 0);
 data = gabi::load<u32>(str);
 gabi::store<u32>(str + 4, 0x100E1A2C);
 if (!gabi::load<u8>(data)) return 0;
 gabi::Local<SafeString> suffix;
 suffix->data = 0x100E1BA4;
 suffix->vtable = 0x100E1994;
 gabi::call<void>(0x0260A828, str);
 u32 oldLen = Length(gabi::load<u32>(str));
 u32 offset = oldLen;
 u32 suffixVt = gabi::load<u32>(gabi::ea(suffix.get()) + 4);
 u32 suffixTarget = gabi::load<u32>(suffixVt + 20);
 if (s32(offset) < 0) offset = 0;
 gabi::call_ptr<void>(suffixTarget, suffix.get());
 u32 appendLen = Length(gabi::load<u32>(gabi::ea(suffix.get())));
 u32 left = gabi::load<u32>(str + 8) - offset;
 if (s32(appendLen) >= s32(left)) appendLen = left - 1;
 if (s32(appendLen) > 0) {
  suffixVt = gabi::load<u32>(gabi::ea(suffix.get()) + 4);
  suffixTarget = gabi::load<u32>(suffixVt + 20);
  u32 dest = data + offset;
  gabi::call_ptr<void>(suffixTarget, suffix.get());
  source = gabi::load<u32>(gabi::ea(suffix.get()));
  gabi::call<void>(0xC0009988, dest, source, appendLen, 0u);
  u32 total = offset + appendLen;
  if (s32(total) > s32(oldLen)) gabi::store<u8>(data + total, 0);
 }
 u32 hash = StringHash(str);
 u32 root = gabi::load<u32>(info + 0x68);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, info + 0x68, root, key.get());
 if (!node || !(node + 24)) return 0;
 return gabi::load<u32>(node + 24);
}
VERIFY(0x02606D74, ResourceByFilename);
static void Traverse_0260B538(u32 node, u32 callback) {
 WWHD_FUNC(0x0260B538, void, node, callback);
 do {
  u32 child = gabi::load<u32>(node);
  if (child) gabi::call<void>(0x0260B538, child, callback);
  u32 receiver = gabi::load<u32>(callback + 4);
  if (receiver) {
   s16 selector = gabi::load<s16>(callback + 10);
   if (selector) {
    s16 adjust = gabi::load<s16>(callback + 8);
    u32 object = receiver + u32(s32(adjust));
    u32 target;
    if (selector < 0) target = gabi::load<u32>(callback + 12);
    else {
     s16 vtOffset = gabi::load<s16>(callback + 14);
     u32 vt = gabi::load<u32>(object + u32(s32(vtOffset)));
     target = gabi::load<u32>(vt + u32(s32(selector)) * 8 + 4);
    }
    gabi::call_ptr<void>(target, object, node);
   }
  }
  node = gabi::load<u32>(node + 4);
 } while (node);
}
VERIFY(0x0260B538, Traverse_0260B538);
static void Traverse_0260B5E8(u32 node, u32 callback) {
 WWHD_FUNC(0x0260B5E8, void, node, callback);
 do {
  u32 child = gabi::load<u32>(node);
  if (child) gabi::call<void>(0x0260B5E8, child, callback);
  u32 receiver = gabi::load<u32>(callback + 4);
  if (receiver) {
   s16 selector = gabi::load<s16>(callback + 10);
   if (selector) {
    s16 adjust = gabi::load<s16>(callback + 8);
    u32 object = receiver + u32(s32(adjust));
    u32 target;
    if (selector < 0) target = gabi::load<u32>(callback + 12);
    else {
     s16 vtOffset = gabi::load<s16>(callback + 14);
     u32 vt = gabi::load<u32>(object + u32(s32(vtOffset)));
     target = gabi::load<u32>(vt + u32(s32(selector)) * 8 + 4);
    }
    gabi::call_ptr<void>(target, object, node);
   }
  }
  node = gabi::load<u32>(node + 4);
 } while (node);
}
VERIFY(0x0260B5E8, Traverse_0260B5E8);
static void Allocate_02604470(u32 info, s32 count, u32 heap) {
 WWHD_FUNC(0x02604470, void, info, count, heap);
 u32 data = gabi::call<u32>(0x0273B0D4, u32(count)*4, heap, 4u);
 gabi::store<u32>(info + 96, data);
 gabi::store<u32>(info + 132, count);
 for (s32 i=0; i<count; ++i) {
  u32 current = gabi::load<u32>(info + 96);
  gabi::store<u32>(current + u32(i)*4, 0);
 }
}
VERIFY(0x02604470, Allocate_02604470);
static void Allocate_026044E0(u32 info, s32 count, u32 heap) {
 WWHD_FUNC(0x026044E0, void, info, count, heap);
 u32 data = gabi::call<u32>(0x0273B0D4, u32(count)*4, heap, 4u);
 gabi::store<u32>(info + 100, data);
 gabi::store<u32>(info + 136, count);
 for (s32 i=0; i<count; ++i) {
  u32 current = gabi::load<u32>(info + 100);
  gabi::store<u32>(current + u32(i)*4, 0);
 }
}
VERIFY(0x026044E0, Allocate_026044E0);
static u32 Insert(u32 tree, u32 node, u32 inserted) {
 WWHD_FUNC(0x0260AD08, u32, tree, node, inserted);
 if (!node) {
  gabi::store<u8>(inserted + 8, 1);
  gabi::store<u32>(inserted + 4, 0);
  gabi::store<u32>(inserted, 0);
  return inserted;
 }
 u32 key = gabi::load<u32>(inserted + 12);
 u32 oldKey = gabi::load<u32>(node + 12);
 u32 left, right;
 if (oldKey > key) {
  u32 child = gabi::load<u32>(node);
  left = gabi::call<u32>(0x0260AD08, tree, child, inserted);
  right = gabi::load<u32>(node + 4);
  gabi::store<u32>(node, left);
 } else if (oldKey < key) {
  u32 child = gabi::load<u32>(node + 4);
  right = gabi::call<u32>(0x0260AD08, tree, child, inserted);
  left = gabi::load<u32>(node);
  gabi::store<u32>(node + 4, right);
 } else {
  if (node != inserted) {
   right = gabi::load<u32>(node + 4);
   gabi::store<u32>(inserted + 4, right);
   left = gabi::load<u32>(node);
   gabi::store<u32>(inserted, left);
   u8 red = gabi::load<u8>(node + 8);
   gabi::store<u8>(inserted + 8, red);
   u32 vt = gabi::load<u32>(node + 16);
   gabi::call_ptr<void>(gabi::load<u32>(vt + 20), node);
  }
  node = inserted;
  right = gabi::load<u32>(node + 4);
  left = gabi::load<u32>(node);
 }
 if (right && gabi::load<u8>(right + 8) && (!left || !gabi::load<u8>(left + 8))) {
  u32 grandchild = gabi::load<u32>(right);
  gabi::store<u32>(node + 4, grandchild);
  gabi::store<u32>(right, node);
  u8 color = gabi::load<u8>(node + 8);
  gabi::store<u8>(right + 8, color);
  gabi::store<u8>(node + 8, 1);
  node = right;
  left = gabi::load<u32>(node);
 }
 if (left && gabi::load<u8>(left + 8)) {
  u32 child = gabi::load<u32>(left);
  if (child && gabi::load<u8>(child + 8)) {
   child = gabi::load<u32>(left + 4);
   gabi::store<u32>(node, child);
   gabi::store<u32>(left + 4, node);
   u8 color = gabi::load<u8>(node + 8);
   gabi::store<u8>(left + 8, color);
   gabi::store<u8>(node + 8, 1);
   node = left;
   left = gabi::load<u32>(node);
  }
 }
 if (left && gabi::load<u8>(left + 8)) {
  right = gabi::load<u32>(node + 4);
  if (right && gabi::load<u8>(right + 8)) {
   u8 color = gabi::load<u8>(node + 8);
   left = gabi::load<u32>(node);
   gabi::store<u8>(node + 8, color ^ 1);
   color = gabi::load<u8>(left + 8);
   gabi::store<u8>(left + 8, color ^ 1);
   right = gabi::load<u32>(node + 4);
   color = gabi::load<u8>(right + 8);
   gabi::store<u8>(right + 8, color ^ 1);
  }
 }
 return node;
}
VERIFY(0x0260AD08, Insert);
static u32 Relative(u32 address) {
 u32 offset = gabi::load<u32>(address);
 return offset ? address + offset : 0;
}
static void RawResource_0260990C(u32 info, u32 index, u32 id, u32 name) {
 WWHD_FUNC(0x0260990C, void, info, index, id, name);
 u32 vt = gabi::load<u32>(name + 4);
 u32 target = gabi::load<u32>(vt + 20);
 u32 archive = gabi::load<u32>(info + 0x5C);
 u32 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 u32 entries = Relative(header + 0x4C);
 u32 data = gabi::load<u32>(name);
 s32 found = gabi::call<s32>(0x027DF9B0, entries, data);
 if (found < 0) return;
 vt = gabi::load<u32>(name + 4);
 target = gabi::load<u32>(vt + 20);
 archive = gabi::load<u32>(info + 0x5C);
 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 entries = Relative(header + 0x4C);
 data = gabi::load<u32>(name);
 found = gabi::call<s32>(0x027DF9B0, entries, data);
 header = gabi::load<u32>(archive + 20);
 entries = Relative(header + 0x4C);
 u32 reference = Relative(entries + u32(found)*16 + 0x24);
 u32 resource = Relative(reference);
 gabi::call<void>(0x026082D0, info, index, resource);
 resource = Relative(reference);
 gabi::call<void>(0x026082EC, info, id, resource);
}
VERIFY(0x0260990C, RawResource_0260990C);
static void RawResource_02609E54(u32 info, u32 index, u32 id, u32 name) {
 WWHD_FUNC(0x02609E54, void, info, index, id, name);
 u32 vt = gabi::load<u32>(name + 4);
 u32 target = gabi::load<u32>(vt + 20);
 u32 archive = gabi::load<u32>(info + 0x5C);
 u32 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 u32 entries = Relative(header + 0x4C);
 u32 data = gabi::load<u32>(name);
 s32 found = gabi::call<s32>(0x027DF9B0, entries, data);
 if (found < 0) return;
 vt = gabi::load<u32>(name + 4);
 target = gabi::load<u32>(vt + 20);
 archive = gabi::load<u32>(info + 0x5C);
 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 entries = Relative(header + 0x4C);
 data = gabi::load<u32>(name);
 found = gabi::call<s32>(0x027DF9B0, entries, data);
 header = gabi::load<u32>(archive + 20);
 entries = Relative(header + 0x4C);
 u32 reference = Relative(entries + u32(found)*16 + 0x24);
 u32 resource = Relative(reference);
 gabi::call<void>(0x026082D0, info, index, resource);
 resource = Relative(reference);
 gabi::call<void>(0x026082EC, info, id, resource);
}
VERIFY(0x02609E54, RawResource_02609E54);
static void RawResource_02609F84(u32 info, u32 index, u32 id, u32 name) {
 WWHD_FUNC(0x02609F84, void, info, index, id, name);
 u32 vt = gabi::load<u32>(name + 4);
 u32 target = gabi::load<u32>(vt + 20);
 u32 archive = gabi::load<u32>(info + 0x5C);
 u32 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 u32 entries = Relative(header + 0x4C);
 u32 data = gabi::load<u32>(name);
 s32 found = gabi::call<s32>(0x027DF9B0, entries, data);
 if (found < 0) return;
 vt = gabi::load<u32>(name + 4);
 target = gabi::load<u32>(vt + 20);
 archive = gabi::load<u32>(info + 0x5C);
 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 entries = Relative(header + 0x4C);
 data = gabi::load<u32>(name);
 found = gabi::call<s32>(0x027DF9B0, entries, data);
 header = gabi::load<u32>(archive + 20);
 entries = Relative(header + 0x4C);
 u32 reference = Relative(entries + u32(found)*16 + 0x24);
 u32 resource = Relative(reference);
 gabi::call<void>(0x026082D0, info, index, resource);
 resource = Relative(reference);
 gabi::call<void>(0x026082EC, info, id, resource);
}
VERIFY(0x02609F84, RawResource_02609F84);
static void RawResource_0260A5F8(u32 info, u32 index, u32 id, u32 name) {
 WWHD_FUNC(0x0260A5F8, void, info, index, id, name);
 u32 vt = gabi::load<u32>(name + 4);
 u32 target = gabi::load<u32>(vt + 20);
 u32 archive = gabi::load<u32>(info + 0x5C);
 u32 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 u32 entries = Relative(header + 0x4C);
 u32 data = gabi::load<u32>(name);
 s32 found = gabi::call<s32>(0x027DF9B0, entries, data);
 if (found < 0) return;
 vt = gabi::load<u32>(name + 4);
 target = gabi::load<u32>(vt + 20);
 archive = gabi::load<u32>(info + 0x5C);
 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 entries = Relative(header + 0x4C);
 data = gabi::load<u32>(name);
 found = gabi::call<s32>(0x027DF9B0, entries, data);
 header = gabi::load<u32>(archive + 20);
 entries = Relative(header + 0x4C);
 u32 reference = Relative(entries + u32(found)*16 + 0x24);
 u32 resource = Relative(reference);
 gabi::call<void>(0x026082D0, info, index, resource);
 resource = Relative(reference);
 gabi::call<void>(0x026082EC, info, id, resource);
}
VERIFY(0x0260A5F8, RawResource_0260A5F8);
static u32 RotateLeft(u32 node, u32 right) {
 u32 child = gabi::load<u32>(right);
 gabi::store<u32>(node + 4, child);
 gabi::store<u32>(right, node);
 u8 color = gabi::load<u8>(node + 8);
 gabi::store<u8>(right + 8, color);
 gabi::store<u8>(node + 8, 1);
 return right;
}
static u32 RotateRight(u32 node, u32 left) {
 u32 child = gabi::load<u32>(left + 4);
 gabi::store<u32>(node, child);
 gabi::store<u32>(left + 4, node);
 u8 color = gabi::load<u8>(node + 8);
 gabi::store<u8>(left + 8, color);
 gabi::store<u8>(node + 8, 1);
 return left;
}
static void Flip(u32 node) {
 u8 color = gabi::load<u8>(node + 8);
 u32 left = gabi::load<u32>(node);
 gabi::store<u8>(node + 8, color ^ 1);
 color = gabi::load<u8>(left + 8);
 gabi::store<u8>(left + 8, color ^ 1);
 u32 right = gabi::load<u32>(node + 4);
 color = gabi::load<u8>(right + 8);
 gabi::store<u8>(right + 8, color ^ 1);
}
static u32 DeleteMinimum(u32 node) {
 WWHD_FUNC(0x0260A9D8, u32, node);
 u32 left = gabi::load<u32>(node);
 if (!left) return 0;
 if ((!left || !gabi::load<u8>(left + 8))) {
  u32 grandchild = gabi::load<u32>(left);
  if (!grandchild || !gabi::load<u8>(grandchild + 8)) {
   Flip(node);
   u32 right = gabi::load<u32>(node + 4);
   u32 child = gabi::load<u32>(right);
   if (child && gabi::load<u8>(child + 8)) {
    child = gabi::load<u32>(right);
    child = RotateRight(right, child);
    gabi::store<u32>(node + 4, child);
    node = RotateLeft(node, child);
    Flip(node);
   }
   left = gabi::load<u32>(node);
  }
 }
 u32 changed = gabi::call<u32>(0x0260A9D8, left);
 u32 right = gabi::load<u32>(node + 4);
 gabi::store<u32>(node, changed);
 if (right && gabi::load<u8>(right + 8)) node = RotateLeft(node, right);
 left = gabi::load<u32>(node);
 if (left && gabi::load<u8>(left + 8)) {
  u32 child = gabi::load<u32>(left);
  if (child && gabi::load<u8>(child + 8)) {
   node = RotateRight(node, left);
   left = gabi::load<u32>(node);
  }
 }
 if (left && gabi::load<u8>(left + 8)) {
  right = gabi::load<u32>(node + 4);
  if (right && gabi::load<u8>(right + 8)) Flip(node);
 }
 return node;
}
VERIFY(0x0260A9D8, DeleteMinimum);
static void CreateHeaps_02603ACC(u32 self) {
 WWHD_FUNC(0x02603ACC, void, self);
 if (gabi::load<u32>(self + 8268)) return;
 gabi::Local<SafeString> name;
 name->data = 269359856u;
 name->vtable = 0x100E1994;
 u32 parent = gabi::load<u32>(self + 8260);
 u32 root = gabi::call<u32>(0x02753004, 1572864u, name.get(), parent, 0xFFFFFFFFu, 0u);
 name->vtable = 0x100E1994;
 name->data = 269359872u;
 gabi::store<u32>(self + 8268, root);
 u32 sub = gabi::call<u32>(0x02753004, 51200u, name.get(), root, 1u, 0u);
 gabi::store<u32>(self + 8276, sub);
 gabi::Local<SafeString> group;
 for (u32 i=0; i<4; ++i) {
  group->vtable = 0x100E1994;
  u32 heap = gabi::load<u32>(self + 8268);
  group->data = 269359884u;
  u32 result = gabi::call<u32>(0x02753004, 51200u, group.get(), heap, 1u, 0u);
  gabi::store<u32>(self + 8296 + i*4, result);
 }
 gabi::Local<SafeString> archive;
 for (u32 i=0; i<22; ++i) {
  archive->vtable = 0x100E1994;
  u32 heap = gabi::load<u32>(self + 8268);
  archive->data = 269359896u;
  u32 result = gabi::call<u32>(0x02753004, 30720u, archive.get(), heap, 1u, 0u);
  gabi::store<u32>(self + 8400 + i*4, result);
 }
}
VERIFY(0x02603ACC, CreateHeaps_02603ACC);
static void CreateHeaps_026079B4(u32 self) {
 WWHD_FUNC(0x026079B4, void, self);
 if (gabi::load<u32>(self + 8264)) return;
 gabi::Local<SafeString> name;
 name->data = 269360084u;
 name->vtable = 0x100E1994;
 u32 parent = gabi::load<u32>(self + 8256);
 u32 root = gabi::call<u32>(0x02753004, 15728640u, name.get(), parent, 0xFFFFFFFFu, 0u);
 name->vtable = 0x100E1994;
 name->data = 269360072u;
 gabi::store<u32>(self + 8264, root);
 u32 sub = gabi::call<u32>(0x02753004, 1048576u, name.get(), root, 1u, 0u);
 gabi::store<u32>(self + 8272, sub);
 gabi::Local<SafeString> group;
 for (u32 i=0; i<4; ++i) {
  group->vtable = 0x100E1994;
  u32 heap = gabi::load<u32>(self + 8264);
  group->data = 269360064u;
  u32 result = gabi::call<u32>(0x02753004, 786432u, group.get(), heap, 1u, 0u);
  gabi::store<u32>(self + 8280 + i*4, result);
 }
 gabi::Local<SafeString> archive;
 for (u32 i=0; i<22; ++i) {
  archive->vtable = 0x100E1994;
  u32 heap = gabi::load<u32>(self + 8264);
  archive->data = 269360100u;
  u32 result = gabi::call<u32>(0x02753004, 440320u, archive.get(), heap, 1u, 0u);
  gabi::store<u32>(self + 8312 + i*4, result);
 }
}
VERIFY(0x026079B4, CreateHeaps_026079B4);
static void RawResource_026097C0(u32 info, u32 index, u32 id, u32 name) {
 WWHD_FUNC(0x026097C0, void, info, index, id, name);
 u32 vt = gabi::load<u32>(name + 4);
 u32 target = gabi::load<u32>(vt + 20);
 u32 archive = gabi::load<u32>(info + 0x5C);
 u32 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 u32 entries = Relative(header + 0x4C);
 u32 data = gabi::load<u32>(name);
 s32 found = gabi::call<s32>(0x027DF9B0, entries, data);
 if (found < 0) return;
 vt = gabi::load<u32>(name + 4);
 target = gabi::load<u32>(vt + 20);
 archive = gabi::load<u32>(info + 0x5C);
 header = gabi::load<u32>(archive + 20);
 gabi::call_ptr<void>(target, name);
 entries = Relative(header + 0x4C);
 data = gabi::load<u32>(name);
 found = gabi::call<s32>(0x027DF9B0, entries, data);
 header = gabi::load<u32>(archive + 20);
 entries = Relative(header + 0x4C);
 u32 reference = Relative(entries + u32(found)*16 + 0x24);
 u32 resource = Relative(reference);
 gabi::call<void>(0x020080C0, resource);
 resource = Relative(reference);
 gabi::call<void>(0x026082D0, info, index, resource);
 resource = Relative(reference);
 gabi::call<void>(0x026082EC, info, id, resource);
}
VERIFY(0x026097C0, RawResource_026097C0);
static void SetResource(u32 info, u32 name, u32 resource) {
 WWHD_FUNC(0x0260566C, void, info, name, resource);
 u32 hash = StringHash(name);
 s32 capacity = gabi::load<s32>(info + 0xBC);
 s32 count = gabi::load<s32>(info + 0xB8);
 u32 pool = info + 0xAC;
 if (count < capacity) {
  u32 node = gabi::load<u32>(pool + 4);
  if (node) {
   u32 next = gabi::load<u32>(node);
   gabi::store<u32>(pool + 4, next);
  }
  if (node) {
   gabi::store<u32>(node + 12, hash);
   gabi::store<u32>(node + 28, pool);
   gabi::store<u32>(node, 0);
   gabi::store<u8>(node + 8, 1);
   gabi::store<u32>(node + 20, hash);
   gabi::store<u32>(node + 24, resource);
   gabi::store<u32>(node + 16, 0x100E1A9C);
   gabi::store<u32>(node + 4, 0);
  }
  u32 oldCount = gabi::load<u32>(pool + 12);
  u32 root = gabi::load<u32>(pool);
  gabi::store<u32>(pool + 12, oldCount + 1);
  root = gabi::call<u32>(0x0260AD08, pool, root, node);
  gabi::store<u32>(pool, root);
  gabi::store<u8>(root + 8, 0);
 } else {
  u32 root = gabi::load<u32>(pool);
  gabi::Local<be<u32>> key;
  *key = hash;
  u32 node = gabi::call<u32>(0x0260ACC8, pool, root, key.get());
  if (node && node + 24) gabi::store<u32>(node + 24, resource);
 }
}
VERIFY(0x0260566C, SetResource);
static bool EqualBounded(u32 a, u32 b,bool leftInR4=false) {
 for (u32 i=0; i<0x40001; ++i) {
  if(leftInR4)gabi::cpu->r[4]=a+i;
  u8 x=gabi::load<u8>(a+i), y=gabi::load<u8>(b+i);
  if (x!=y) return false;
  if (!x) return true;
 }
 return false;
}
static u32 ResourceIdByName(u32 self, u32 archiveName, u32 name) {
 WWHD_FUNC(0x02606900, u32, self, archiveName, name);
 u32 hash = StringHash(archiveName);
 u32 root = gabi::load<u32>(self + 20);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 20, root, key.get());
 if (!node || !(node + 24)) return 0;
 s32 count = gabi::load<s32>(0x10080150);
 s32 found = -1;
 if (count <= 0) return 0;
 u32 table = 0x1009BB10;
 do {
  table += 12;
  if (hash == gabi::load<u32>(table)) {
   s32 names = gabi::load<s32>(table + 8);
   if (names > 0) {
    u32 array = gabi::load<u32>(table + 4);
    for (s32 index=0; index<names; ++index) {
     gabi::Local<SafeString> candidate;
     candidate->data = gabi::load<u32>(array + u32(index)*8);
     candidate->vtable = 0x100E1994;
     StringAssure(name);
     StringAssure(name);
     u32 saved = gabi::load<u32>(name);
     StringAssure(gabi::ea(candidate.get()));
     u32 other = gabi::load<u32>(gabi::ea(candidate.get()));
     if (saved == other) { found=index; break; }
     u32 actual = gabi::load<u32>(name);
     other = gabi::load<u32>(gabi::ea(candidate.get()));
     if (EqualBounded(actual,other)) { found=index; break; }
    }
   }
  }
 } while (--count);
 if (found <= 0) return 0;
 u32 info = gabi::load<u32>(node + 24);
 return gabi::call<u32>(0x026066A0, info, found);
}
VERIFY(0x02606900, ResourceIdByName);
static void UnloadNamed(u32 self, u32 name) {
 WWHD_FUNC(0x02607138, void, self, name);
 u32 hash = StringHash(name);
 u32 root = gabi::load<u32>(self + 20);
 gabi::Local<be<u32>> key;
 *key = hash;
 u32 first = gabi::call<u32>(0x0260ACC8, self + 20, root, key.get());
 if (first) first += 24;
 *key = hash;
 gabi::Local<be<u32>> keyAgain;
 *keyAgain = hash;
 u32 node = gabi::call<u32>(0x0260ACC8, self + 20, root, keyAgain.get());
 if (node && node + 24) {
  u32 changed = gabi::call<u32>(0x0260AEFC, self + 20, root, key.get());
  gabi::store<u32>(self + 20, changed);
  if (changed) gabi::store<u8>(changed + 8, 0);
 }
 u32 loader = gabi::load<u32>(0x101F4F54);
 gabi::call<void>(0x0260F61C, loader, hash);
 u32 info = gabi::load<u32>(first);
 u32 heap = gabi::load<u32>(info + 0x7C);
 if (heap) {
  heap = gabi::load<u32>(info + 0x7C);
  u32 vt = gabi::load<u32>(heap + 12);
  gabi::call_ptr<void>(gabi::load<u32>(vt + 0x24), heap);
  info = gabi::load<u32>(first);
 }
 heap = gabi::load<u32>(info + 0x80);
 u32 vt = gabi::load<u32>(heap + 12);
 gabi::call_ptr<void>(gabi::load<u32>(vt + 0x24), heap);
}
VERIFY(0x02607138, UnloadNamed);
static void DestroyHeap(u32 heap) {
 u32 vt=gabi::load<u32>(heap + 12);
 gabi::call_ptr<void>(gabi::load<u32>(vt + 0x24), heap);
}
static void DestroyHeaps_026072AC(u32 self) {
 WWHD_FUNC(0x026072AC, void, self);
 if (!gabi::load<u32>(self + 8264)) return;
 DestroyHeap(gabi::load<u32>(self + 8272));
 gabi::store<u32>(self + 8272, 0);
 for (u32 i=0;i<4;++i) {
  DestroyHeap(gabi::load<u32>(self + 8280 + i*4));
  gabi::store<u32>(self + 8280 + i*4, 0);
 }
 for (u32 i=0;i<22;++i) {
  DestroyHeap(gabi::load<u32>(self + 8312 + i*4));
  gabi::store<u32>(self + 8312 + i*4, 0);
 }
 DestroyHeap(gabi::load<u32>(self + 8264));
 gabi::store<u32>(self + 8264, 0);
}
VERIFY(0x026072AC, DestroyHeaps_026072AC);
static void DestroyHeaps_02607414(u32 self) {
 WWHD_FUNC(0x02607414, void, self);
 if (!gabi::load<u32>(self + 8268)) return;
 DestroyHeap(gabi::load<u32>(self + 8276));
 gabi::store<u32>(self + 8276, 0);
 for (u32 i=0;i<4;++i) {
  DestroyHeap(gabi::load<u32>(self + 8296 + i*4));
  gabi::store<u32>(self + 8296 + i*4, 0);
 }
 for (u32 i=0;i<22;++i) {
  DestroyHeap(gabi::load<u32>(self + 8400 + i*4));
  gabi::store<u32>(self + 8400 + i*4, 0);
 }
 DestroyHeap(gabi::load<u32>(self + 8268));
 gabi::store<u32>(self + 8268, 0);
}
VERIFY(0x02607414, DestroyHeaps_02607414);
static u32 FormatStringCtor(u32 self, u32 format, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
 WWHD_FUNC(0x0260A8A4, u32, self, format, a5, a6, a7, a8, a9, a10);
 u32 entry = gabi::cpu->r[1];
 bool floats = gabi::cpu->cr[6] != 0;
 gabi::Local<u8[0x90]> frame;
 u32 f = gabi::ea(frame.get()) + 8;
 const u32 registers[8] = {self,format,a5,a6,a7,a8,a9,a10};
 for (u32 i=0;i<8;++i) gabi::store<u32>(f + 0x18 + i*4, registers[i]);
 if (floats) for (u32 i=0;i<8;++i) {
  u64 bits; memcpy(&bits, &gabi::cpu->f[i+1].ps0, 8);
  gabi::store<u32>(f + 0x38 + i*8, u32(bits>>32));
  gabi::store<u32>(f + 0x3C + i*8, u32(bits));
 }
 if (!self) self=gabi::call<u32>(0x0273AD10,76u);
 if (!self) return self;
 u32 destination = self;
 if (!destination) destination=gabi::call<u32>(0x0273AD10,12u);
 if (destination) {
  gabi::store<u32>(destination,self+12);
  gabi::store<u32>(destination+4,0x100E19AC);
  gabi::store<u32>(destination+8,64);
  gabi::store<u8>(self+75,0);
 }
 gabi::store<u32>(self+4,0x100E19E4);
 u32 data = gabi::load<u32>(self);
 gabi::store<u8>(data,0);
 gabi::store<u32>(self+4,0x100E1A44);
 gabi::store<u8>(f+8,2);
 gabi::store<u32>(f+12,entry+8);
 gabi::store<u32>(f+16,f+24);
 gabi::store<u8>(f+9,0);
 gabi::call<void>(0x02759C10,self,format,f+8);
 return self;
}
VERIFY(0x0260A8A4, FormatStringCtor);
static void InitializeString64(u32 object, u32 vt) {
 u32 where=object;
 if (!where) where=gabi::call<u32>(0x0273AD10,12u);
 if (where) {
  gabi::store<u32>(where+4,0x100E19AC);
  gabi::store<u32>(where,object+12);
  gabi::store<u32>(where+8,64);
  gabi::store<u8>(object+75,0);
 }
 u32 data=gabi::load<u32>(object);
 gabi::store<u32>(object+4,0x100E19E4);
 gabi::store<u8>(data,0);
 gabi::store<u32>(object+4,vt);
}
static void CopyString(u32 destination, u32 source, u32 capacityAddress, s32 assureR4=-1,u32 capacityRegister=0) {
 StringAssure(source);
 u32 length=Length(gabi::load<u32>(source));
 u32 assureFourth=length?0u:gabi::cpu->r[4];
 u32 capacity=gabi::load<u32>(capacityAddress);
 u32 vt=gabi::load<u32>(source+4);
 u32 target=gabi::load<u32>(vt+20);
 if (s32(length)>=s32(capacity)) length=capacity-1;
 // Inline capacity loads remain live at the second assure callback.
 if(capacityRegister)gabi::cpu->r[capacityRegister]=capacity;
 if(assureR4>=0)gabi::call_ptr<void>(target,source,assureR4?capacity:assureFourth);
 else gabi::call_ptr<void>(target,source);
 u32 data=gabi::load<u32>(source);
 gabi::call<void>(0xC0009988,destination,data,length,0u);
 gabi::store<u8>(destination+length,0);
}
static u32 InfoCtor(u32 self,u32 archiveName,u32 path,u32 unused,u32 parent,u32 heap) {
 WWHD_FUNC(0x02605344,u32,self,archiveName,path,unused,parent,heap);
 if (!self) self=gabi::call<u32>(0x0273AD10,200u);
 if (!self) return self;
 gabi::call<void>(0x02752A84,self);
 u32 first=self+16;
 gabi::store<u32>(self+12,0x100E1DC0);
 if (!first) first=gabi::call<u32>(0x0273AD10,76u);
 if (first) InitializeString64(first,0x100E19FC);
 u32 rawSecond=self+0x5C, second=rawSecond;
 if (!second) second=gabi::call<u32>(0x0273AD10,76u);
 if (second) InitializeString64(second,0x100E19FC);
 gabi::store<u32>(self+0xA8,unused);
 u32 pool=self+0xAC;
 if (!pool) pool=gabi::call<u32>(0x0273AD10,20u);
 if (pool) {
  for (u32 off:{16u,0u,8u,12u,4u}) gabi::store<u32>(pool+off,0);
 }
 gabi::store<u32>(self+0xC4,heap);
 gabi::store<u32>(self+0xC0,parent);
 u32 destination=gabi::load<u32>(self+16);
 CopyString(destination,archiveName,self+24,-1,5);
 destination=gabi::load<u32>(rawSecond);
 CopyString(destination,path,rawSecond+8,-1,6);
 u32 archive=gabi::load<u32>(self+0xA8);
 if (archive) gabi::store<u32>(archive+32,rawSecond);
 return self;
}
VERIFY(0x02605344,InfoCtor);
static void StripExtension(u32 buffer, u32 name, u32 literal,s32 assureR4=-1) {
 u32 data=buffer+12;
 gabi::store<u8>(data+127,0);
 gabi::store<u32>(buffer,data);
 gabi::store<u32>(buffer+8,128);
 gabi::store<u32>(buffer+4,0x100E1A14);
 CopyString(data,name,buffer+8,assureR4);
 gabi::Local<SafeString> suffix;
 u32 suffixAddress=gabi::ea(suffix.get());
 suffix->vtable=0x100E1994;
 suffix->data=literal;
 gabi::store<u32>(buffer+4,0x100E1A2C);
 gabi::call<void>(0x0260A828,buffer);
 u32 length=Length(gabi::load<u32>(buffer));
 StringAssure(suffixAddress);
 u32 suffixLength=Length(gabi::load<u32>(suffixAddress));
 s32 position=s32(length-suffixLength);
 if (position>=0) {
  u32 remaining=position+1;
  if (position-1<0) remaining=1;
  for (;;) {
   gabi::Local<SafeString> part;
   u32 partAddress=gabi::ea(part.get());
   part->vtable=0x100E1994;
   part->data=gabi::load<u32>(buffer)+u32(position);
   gabi::call<void>(0x0260A824,partAddress);
   StringAssure(partAddress);
   u32 left=gabi::load<u32>(partAddress);
   StringAssure(suffixAddress);
   u32 right=gabi::load<u32>(suffixAddress);
   bool equal=left==right || s32(suffixLength)<=0;
   if (!equal) {
    equal=true;
    for (u32 i=0;i<suffixLength;++i) {
     u8 a=gabi::load<u8>(left+i);
     if (!a) { equal=!gabi::load<u8>(right+i); break; }
     u8 b=gabi::load<u8>(right+i);
     if (!b || a!=b) { equal=false; break; }
    }
   }
   if (equal) break;
   --remaining; --position;
   if (!remaining) break;
  }
 }
 if (position<0) {
  StringAssure(buffer);
  position=s32(Length(gabi::load<u32>(buffer))+1);
 }
 if (position>=gabi::load<s32>(buffer+8)) {
  StringAssure(buffer);
  Length(gabi::load<u32>(buffer));
 } else {
  u32 destination=gabi::load<u32>(buffer);
  if (position<0) position=0;
  gabi::store<u8>(destination+u32(position),0);
 }
}
static void Animation_02608F80(u32 info,u32 index,u32 id,u32 name,u32 heap) {
 WWHD_FUNC(0x02608F80,void,info,index,id,name,heap);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 StripExtension(str,name,0x100E1C22);
 u32 vt=gabi::load<u32>(str+4), target=gabi::load<u32>(vt+20);
 u32 archive=gabi::load<u32>(info+0x5C);
 u32 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 u32 entries=Relative(header+52);
 u32 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 if (s32(found)<0) return;
 archive=gabi::load<u32>(info+0x5C);
 header=gabi::load<u32>(archive+20);
 entries=Relative(header+52);
 u32 resource=Relative(entries+found*16+0x24);
 u32 animation=gabi::call<u32>(0x0273B050,16u,heap,4u);
 if (animation) {
  gabi::store<u32>(animation+8,0);
  gabi::store<u32>(animation+4,0x1016E4AC);
  f32 initial=gabi::load<f32>(0x100E1B58);
  gabi::store<u32>(animation+12,0);
  gabi::store<f32>(animation,initial);
 }
 gabi::store<u32>(animation+12,resource);
 gabi::call<void>(0x026082D0,info,index,animation);
 gabi::call<void>(0x026082EC,info,id,animation);
}
VERIFY(0x02608F80,Animation_02608F80);
static void Animation_026093A0(u32 info,u32 index,u32 id,u32 name,u32 heap) {
 WWHD_FUNC(0x026093A0,void,info,index,id,name,heap);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 StripExtension(str,name,0x100E1C24);
 u32 vt=gabi::load<u32>(str+4), target=gabi::load<u32>(vt+20);
 u32 archive=gabi::load<u32>(info+0x5C);
 u32 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 u32 entries=Relative(header+48);
 u32 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 if (s32(found)<0) return;
 archive=gabi::load<u32>(info+0x5C);
 header=gabi::load<u32>(archive+20);
 entries=Relative(header+48);
 u32 resource=Relative(entries+found*16+0x24);
 u32 animation=gabi::call<u32>(0x0273B050,16u,heap,4u);
 if (animation) {
  gabi::store<u32>(animation+8,0);
  gabi::store<u32>(animation+4,0x1016E4CC);
  f32 initial=gabi::load<f32>(0x100E1B58);
  gabi::store<u32>(animation+12,0);
  gabi::store<f32>(animation,initial);
 }
 gabi::store<u32>(animation+12,resource);
 gabi::call<void>(0x026082D0,info,index,animation);
 gabi::call<void>(0x026082EC,info,id,animation);
}
VERIFY(0x026093A0,Animation_026093A0);
static void Animation_02609A3C(u32 info,u32 index,u32 id,u32 name,u32 heap) {
 WWHD_FUNC(0x02609A3C,void,info,index,id,name,heap);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 StripExtension(str,name,0x100E1C26);
 u32 vt=gabi::load<u32>(str+4), target=gabi::load<u32>(vt+20);
 u32 archive=gabi::load<u32>(info+0x5C);
 u32 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 u32 entries=Relative(header+36);
 u32 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 if (s32(found)<0) return;
 archive=gabi::load<u32>(info+0x5C);
 header=gabi::load<u32>(archive+20);
 entries=Relative(header+36);
 u32 resource=Relative(entries+found*16+0x24);
 u32 animation=gabi::call<u32>(0x0273B050,36u,heap,4u);
 if (animation) {
  gabi::store<u32>(animation+32,resource);
  u32 a=gabi::load<u32>(resource+8);
  gabi::store<u16>(animation+2,a);
  u32 b=gabi::load<u32>(resource+12);
  gabi::store<u8>(animation+8,0);
  gabi::store<u16>(animation+4,b);
 }
 gabi::call<void>(0x026082D0,info,index,animation);
 gabi::call<void>(0x026082EC,info,id,animation);
}
VERIFY(0x02609A3C,Animation_02609A3C);
static void Loader_02608308(u32 info,u32 index,u32 id,u32 name,u32 heap) {
 WWHD_FUNC(0x02608308,void,info,index,id,name,heap);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 StripExtension(str,name,0x100E1C1C);
 u32 vt=gabi::load<u32>(str+4), target=gabi::load<u32>(vt+20);
 u32 archive=gabi::load<u32>(info+0x5C);
 u32 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 u32 entries=Relative(header+32);
 u32 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 if(s32(found)<0)return;
 archive=gabi::load<u32>(info+0x5C);
 header=gabi::load<u32>(archive+20);
 entries=Relative(header+32);
 u32 resource=Relative(entries+found*16+36);
 u32 resourceId=gabi::call<u32>(0x026031D0,archive,found);
 archive=gabi::load<u32>(info+0x5C);
 u32 archiveName=gabi::load<u32>(archive+32);
 StringAssure(archiveName);
 gabi::Local<SafeString> copy;
 copy->data=gabi::load<u32>(archiveName);
 copy->vtable=0x100E1994;
 u32 result=gabi::call<u32>(0x0272D384,resource,resourceId,copy.get(),heap);
 gabi::call<void>(0x026082D0,info,index,result);
 gabi::call<void>(0x026082EC,info,id,result);
}
VERIFY(0x02608308,Loader_02608308);
static void Loader_0260873C(u32 info,u32 index,u32 id,u32 name,u32 heap) {
 WWHD_FUNC(0x0260873C,void,info,index,id,name,heap);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 StripExtension(str,name,0x100E1C1E,1);
 u32 vt=gabi::load<u32>(str+4), target=gabi::load<u32>(vt+20);
 u32 archive=gabi::load<u32>(info+0x5C);
 u32 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 u32 entries=Relative(header+40);
 u32 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 if (s32(found)<0) return;
 vt=gabi::load<u32>(str+4); target=gabi::load<u32>(vt+20);
 archive=gabi::load<u32>(info+0x5C);
 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 entries=Relative(header+40);
 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 header=gabi::load<u32>(archive+20);
 entries=Relative(header+40);
 archive=gabi::load<u32>(info+0x5C);
 u32 archiveName=gabi::load<u32>(archive+32);
 u32 reference=entries+found*16+36;
 u32 nameVt=gabi::load<u32>(archiveName+4);
 u32 resource=Relative(reference);
 u32 nameTarget=gabi::load<u32>(nameVt+20);
 gabi::call_ptr<void>(nameTarget,archiveName);
 gabi::Local<SafeString> copy;
 copy->data=gabi::load<u32>(archiveName);
 copy->vtable=0x100E1994;
 u32 result=gabi::call<u32>(0x027F78A8,resource,copy.get(),heap);
 gabi::call<void>(0x026082D0,info,index,result);
 gabi::call<void>(0x026082EC,info,id,result);
}
VERIFY(0x0260873C,Loader_0260873C);
static void Loader_02608B90(u32 info,u32 index,u32 id,u32 name,u32 heap) {
 WWHD_FUNC(0x02608B90,void,info,index,id,name,heap);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 StripExtension(str,name,0x100E1C20,0);
 u32 vt=gabi::load<u32>(str+4), target=gabi::load<u32>(vt+20);
 u32 archive=gabi::load<u32>(info+0x5C);
 u32 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 u32 entries=Relative(header+56);
 u32 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 if(s32(found)<0)return;
 archive=gabi::load<u32>(info+0x5C);
 header=gabi::load<u32>(archive+20);
 entries=Relative(header+56);
 u32 resource=Relative(entries+found*16+36);
 u32 result=gabi::call<u32>(0x027F7A2C,resource,heap);
 gabi::call<void>(0x026082D0,info,index,result);
 gabi::call<void>(0x026082EC,info,id,result);
}
VERIFY(0x02608B90,Loader_02608B90);
static u32 ArchiveInfoCtor(u32 self,u32 name,u32 archive,u32 parent,u32 heap) {
 WWHD_FUNC(0x026042AC,u32,self,name,archive,parent,heap);
 if (!self) self=gabi::call<u32>(0x0273AD10,140u);
 if (!self) return self;
 gabi::call<void>(0x02752A84,self);
 u32 rawString=self+16;
 gabi::store<u32>(self+12,0x100E1DB0);
 u32 str=rawString;
 if (!str) str=gabi::call<u32>(0x0273AD10,76u);
 if(str)InitializeString64(str,0x100E19FC);
 gabi::store<u32>(self+96,0);
 u32 tree=self+104;
 gabi::store<u32>(self+100,0);
 gabi::store<u32>(self+92,archive);
 if(!tree)tree=gabi::call<u32>(0x0273AD10,20u);
 if(tree){
  gabi::store<u32>(tree+16,0);
  gabi::store<u32>(tree,0);
  gabi::store<u32>(tree+8,0);
  gabi::store<u32>(tree+12,0);
  gabi::store<u32>(tree+4,0);
 }
 gabi::store<u32>(self+124,parent);
 gabi::store<u32>(self+136,0);
 gabi::store<u32>(self+132,0);
 gabi::store<u32>(self+128,heap);
 u32 nameVt=gabi::load<u32>(name+4),target=gabi::load<u32>(nameVt+20);
 u32 destination=gabi::load<u32>(rawString);
 gabi::call_ptr<void>(target,name);
 u32 length=Length(gabi::load<u32>(name));
 u32 capacity=gabi::load<u32>(rawString+8);
 if(s32(length)>=s32(capacity))length=capacity-1;
 StringAssure(name);
 gabi::call<void>(0xC0009988,destination,gabi::load<u32>(name),length,0u);
 gabi::store<u8>(destination+length,0);
 archive=gabi::load<u32>(self+92);
 if(archive)gabi::store<u32>(archive+32,rawString);
 return self;
}
VERIFY(0x026042AC,ArchiveInfoCtor);
static u32 SelectHeap(u32 control,u32 name,u32 path,u32 heap) {
 WWHD_FUNC(0x02604094,u32,control,name,path,heap);
 if(!gabi::call<u32>(0x02603740,control,name,path,heap))return gabi::load<u32>(control+0x2044);
 if(!gabi::load<u32>(control+0x204C))gabi::call<void>(0x02603ACC,control);
 gabi::Local<SafeString> needle;
 u32 needleAddress=gabi::ea(needle.get());
 needle->vtable=0x100E1994;needle->data=0x100E1B4C;
 StringAssure(name);
 u32 length=Length(gabi::load<u32>(name));
 StringAssure(needleAddress);
 u32 needleLength=Length(gabi::load<u32>(needleAddress));
 s32 end=s32(length-needleLength),found=-1;
 if(end>=0) for(s32 i=0;i<=end;++i){
  gabi::Local<SafeString> part;
  u32 partAddress=gabi::ea(part.get());
  part->vtable=0x100E1994;part->data=gabi::load<u32>(name)+u32(i);
  gabi::call<void>(0x0260A824,partAddress);
  StringAssure(partAddress);
  u32 left=gabi::load<u32>(partAddress);
  StringAssure(needleAddress);
  u32 right=gabi::load<u32>(needleAddress);
  bool equal=left==right || s32(needleLength)<=0;
  if(!equal){
   equal=true;
   for(u32 j=0;j<needleLength;++j){
    u8 a=gabi::load<u8>(left+j);
    if(!a){equal=!gabi::load<u8>(right+j);break;}
    u8 b=gabi::load<u8>(right+j);
    if(!b || a!=b){equal=false;break;}
   }
  }
  if(equal){found=i;break;}
 }
 if(found<0)return gabi::load<u32>(control+0x204C);
 return gabi::call<u32>(0x02603C84,control,name);
}
VERIFY(0x02604094,SelectHeap);
static s32 FindSubstring(u32 name,u32 literal,bool assure=true) {
 gabi::Local<SafeString> needle;
 u32 needleAddress=gabi::ea(needle.get());
 needle->vtable=0x100E1994;needle->data=literal;
 if(assure)StringAssure(name);
 u32 length=Length(gabi::load<u32>(name));
 StringAssure(needleAddress);
 u32 needleLength=Length(gabi::load<u32>(needleAddress));
 s32 end=s32(length-needleLength),found=-1;
 if(end>=0) for(s32 i=0;i<=end;++i){
  gabi::Local<SafeString> part;
  u32 partAddress=gabi::ea(part.get());
  part->vtable=0x100E1994;part->data=gabi::load<u32>(name)+u32(i);
  gabi::call<void>(0x0260A824,partAddress);
  StringAssure(partAddress);
  u32 left=gabi::load<u32>(partAddress);
  StringAssure(needleAddress);
  u32 right=gabi::load<u32>(needleAddress);
  bool equal=left==right || s32(needleLength)<=0;
  if(!equal){
   equal=true;
   for(u32 j=0;j<needleLength;++j){
    u8 a=gabi::load<u8>(left+j);
    if(!a){equal=!gabi::load<u8>(right+j);break;}
    u8 b=gabi::load<u8>(right+j);
    if(!b || a!=b){equal=false;break;}
   }
  }
  if(equal){found=i;break;}
 }
 return found;
}
static bool LiteralEquals(u32 name,u32 literal,bool leftInR4=false) {
 gabi::Local<SafeString> comparison;
 u32 object=gabi::ea(comparison.get());
 comparison->vtable=0x100E1994;comparison->data=literal;
 StringAssure(name);
 StringAssure(name);
 u32 saved=gabi::load<u32>(name);
 StringAssure(object);
 u32 other=gabi::load<u32>(object);
 if(saved==other)return true;
 return EqualBounded(gabi::load<u32>(name),other,leftInR4);
}
static u32 NeedsSeparateHeap(u32 control,u32 name,u32 path) {
 WWHD_FUNC(0x02603740,u32,control,name,path);
 if(!LiteralEquals(path,0x100E1ADC,true))return LiteralEquals(path,0x100E1AEC);
 if(FindSubstring(name,0x100E1AE0)>=0)return 1;
 return LiteralEquals(name,0x100E1AE4);
}
VERIFY(0x02603740,NeedsSeparateHeap);
static u32 ChooseSpecialHeap(u32 control,u32 name) {
 WWHD_FUNC(0x02603C84,u32,control,name);
 if(LiteralEquals(name,0x100E1B24))return gabi::load<u32>(control+0x2054);
 if(LiteralEquals(name,0x100E1B2C))return gabi::load<u32>(control+0x2068);
 if(LiteralEquals(name,0x100E1B34))return gabi::load<u32>(control+0x206C);
 if(LiteralEquals(name,0x100E1B3C))return gabi::load<u32>(control+0x2070);
 if(LiteralEquals(name,0x100E1B44))return gabi::load<u32>(control+0x2074);
 for(u32 i=0;i<22;++i){
  u32 heap=gabi::load<u32>(control+0x20D0+i*4);
  u32 vt=gabi::load<u32>(heap+12);
  u32 target=gabi::load<u32>(vt+0x74);
  if(gabi::call_ptr<u32>(target,heap)>0x5000)return gabi::load<u32>(control+0x20D0+i*4);
 }
 return gabi::load<u32>(control+0x204C);
}
VERIFY(0x02603C84,ChooseSpecialHeap);
static void StorePool(u32 pool,u32 hash,u32 resource,u32 nodeVtable) {
 s32 capacity = gabi::load<s32>(pool + 16);
 s32 count = gabi::load<s32>(pool + 12);
 if (count < capacity) {
  u32 node = gabi::load<u32>(pool + 4);
  if (node) {
   u32 next = gabi::load<u32>(node);
   gabi::store<u32>(pool + 4, next);
  }
  if (node) {
   gabi::store<u32>(node + 12, hash);
   gabi::store<u32>(node + 28, pool);
   gabi::store<u32>(node, 0);
   gabi::store<u8>(node + 8, 1);
   gabi::store<u32>(node + 20, hash);
   gabi::store<u32>(node + 24, resource);
   gabi::store<u32>(node + 16, nodeVtable);
   gabi::store<u32>(node + 4, 0);
  }
  u32 oldCount = gabi::load<u32>(pool + 12);
  u32 root = gabi::load<u32>(pool);
  gabi::store<u32>(pool + 12, oldCount + 1);
  root = gabi::call<u32>(0x0260AD08, pool, root, node);
  gabi::store<u32>(pool, root);
  gabi::store<u8>(root + 8, 0);
 } else {
  u32 root = gabi::load<u32>(pool);
  gabi::Local<be<u32>> key;
  *key = hash;
  u32 node = gabi::call<u32>(0x0260ACC8, pool, root, key.get());
  if (node && node + 24) gabi::store<u32>(node + 24, resource);
 }
}
static u32 SetInfo(u32 control,u32 archiveName,u32 path,u32 parent) {
 WWHD_FUNC(0x0260623C,u32,control,archiveName,path,parent);
 u32 parentHeap=gabi::call<u32>(0x02604094,control,archiveName,path,parent);
 StringAssure(archiveName);
 gabi::Local<SafeString> copy;
 copy->vtable=0x100E1994;copy->data=gabi::load<u32>(archiveName);
 u32 heap=gabi::call<u32>(0x02754B38,0u,copy.get(),parentHeap,1u,0u);
 u32 archive=gabi::call<u32>(0x0273B050,36u,heap,4u);
 if(archive)archive=gabi::call<u32>(0x02602AD4,archive);
 if(!archive){DestroyHeap(heap);return 0;}
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 gabi::store<u8>(str+12,0);gabi::store<u32>(str+8,128);
 gabi::store<u8>(str+139,0);gabi::store<u32>(str,str+12);
 gabi::store<u32>(str+4,0x100E1A2C);
 StringAssure(path);
 u32 pathChars=gabi::load<u32>(path);
 StringAssure(archiveName);
 gabi::call<void>(0x02759C28,str,0x100E1B94u,pathChars,gabi::load<u32>(archiveName));
 u32 loaded=gabi::call<u32>(0x026123FC,gabi::load<u32>(0x101F4F7C),str);
 if(!loaded){DestroyHeap(heap);return 0;}
 u32 oldHeap=1;
 if(heap)oldHeap=gabi::call<u32>(0x02756170,gabi::load<u32>(0x101F8B4C),heap);
 gabi::call<void>(0x026031C4,archive,loaded,archiveName,heap);
 u32 info=gabi::call<u32>(0x0273B050,200u,heap,4u);
 if(info)info=gabi::call<u32>(0x02605344,info,path,archiveName,archive,parent,heap);
 u32 hash=StringHash(str);
 gabi::call<void>(0x026057F4,info);
 StorePool(control+0x1028,hash,info,0x100E1A84);
 if(parent)gabi::store<u32>(parent+16,gabi::load<u32>(info+92));
 u32 value=gabi::load<u32>(info+92);
 u32 heapVt=gabi::load<u32>(heap+12);
 gabi::store<u32>(heap+16,value);
 u32 target=gabi::load<u32>(heapVt+0x2C);
 gabi::call_ptr<void>(target,heap);
 if(oldHeap!=1)gabi::call<void>(0x02756170,gabi::load<u32>(0x101F8B4C),oldHeap);
 return 1;
}
VERIFY(0x0260623C,SetInfo);
static u32 SetArchive(u32 control,u32 name,u32 parent,u32 arg6) {
 WWHD_FUNC(0x02604EB4,u32,control,name,parent,arg6);
 u32 parentHeap=gabi::call<u32>(0x02604094,control,name,name,arg6);
 StringAssure(name);
 gabi::Local<SafeString> copy;
 copy->vtable=0x100E1994;copy->data=gabi::load<u32>(name);
 u32 heap=gabi::call<u32>(0x02754B38,0u,copy.get(),parentHeap,1u,0u);
 u32 archive=gabi::call<u32>(0x0273B050,36u,heap,4u);
 if(archive)archive=gabi::call<u32>(0x02602AD4,archive);
 if(!archive){DestroyHeap(heap);return 0;}
 u32 loaded=gabi::call<u32>(0x026123FC,gabi::load<u32>(0x101F4F7C),name);
 if(!loaded){DestroyHeap(heap);return 0;}
 u32 oldHeap=1;
 if(heap)oldHeap=gabi::call<u32>(0x02756170,gabi::load<u32>(0x101F8B4C),heap);
 gabi::call<void>(0x02602ECC,archive,loaded,name,heap);
 u32 info=gabi::call<u32>(0x0273B050,140u,heap,4u);
 if(info)info=gabi::call<u32>(0x026042AC,info,name,archive,parent,heap);
 u32 hash=StringHash(name);
 gabi::call<void>(0x02604550,info,name,heap);
 gabi::call<void>(0x02604A08,info,heap);
 u32 pool=control+20;
 StorePool(pool,hash,info,0x100E1A6C);
 u32 parentValue=0;
 if(parent){parentValue=gabi::load<u32>(info+16);gabi::store<u32>(parent+16,parentValue);}
 u32 value=gabi::load<u32>(info+16);
 u32 vt=gabi::load<u32>(heap+12);
 gabi::store<u32>(heap+16,value);
 u32 resizeTarget=gabi::load<u32>(vt+0x2C);
 // The inlined parent assignment leaves its value live in r8 at the virtual resize.
 if(parent)gabi::cpu->r[8]=parentValue;
 gabi::call_ptr<void>(resizeTarget,heap);
 gabi::Local<be<u32>[4]> receiverA;
 u32 receiverAAddress=gabi::ea(receiverA.get());
 gabi::store<u32>(receiverAAddress,archive);
 gabi::store<u32>(receiverAAddress+12,receiverAAddress);
 gabi::Local<be<u32>[2]> memberA;
 gabi::store<u32>(gabi::ea(memberA.get()),gabi::load<u32>(0x100E1980));
 gabi::store<u32>(gabi::ea(memberA.get())+4,gabi::load<u32>(0x100E1984));
 gabi::Local<be<u32>[4]> callbackA;
 gabi::call<void>(0x0260B698,callbackA.get(),receiverAAddress+12,memberA.get());
 u32 root=gabi::load<u32>(pool);
 if(root)gabi::call<void>(0x0260B538,root,callbackA.get());
 gabi::Local<be<u32>[4]> receiverB;
 u32 receiverBAddress=gabi::ea(receiverB.get());
 gabi::store<u32>(receiverBAddress,archive);
 gabi::store<u32>(receiverBAddress+12,receiverBAddress);
 gabi::Local<be<u32>[2]> memberB;
 gabi::store<u32>(gabi::ea(memberB.get()),gabi::load<u32>(0x100E1988));
 gabi::store<u32>(gabi::ea(memberB.get())+4,gabi::load<u32>(0x100E198C));
 gabi::Local<be<u32>[4]> callbackB;
 gabi::call<void>(0x0260B704,callbackB.get(),receiverBAddress+12,memberB.get());
 root=gabi::load<u32>(pool);
 if(root)gabi::call<void>(0x0260B5E8,root,callbackB.get());
 if(oldHeap!=1)gabi::call<void>(0x02756170,gabi::load<u32>(0x101F8B4C),oldHeap);
 return 1;
}
VERIFY(0x02604EB4,SetArchive);
static u32 LookupCombined(u32 control,u32 archiveName,u32 path,u32 resourceName) {
 WWHD_FUNC(0x02606BEC,u32,control,archiveName,path,resourceName);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 gabi::store<u32>(str,str+12);gabi::store<u8>(str+12,0);
 gabi::store<u8>(str+139,0);gabi::store<u32>(str+8,128);
 gabi::store<u32>(str+4,0x100E1A2C);
 StringAssure(path);
 u32 pathChars=gabi::load<u32>(path);
 StringAssure(archiveName);
 gabi::call<void>(0x02759C28,str,0x100E1B9Cu,pathChars,gabi::load<u32>(archiveName));
 u32 hash=StringHash(str);
 u32 root=gabi::load<u32>(control+0x1028);
 gabi::Local<be<u32>> key;*key=hash;
 u32 node=gabi::call<u32>(0x0260ACC8,control+0x1028,root,key.get());
 if(!node || !u32(node+24))return 0;
 return gabi::call<u32>(0x02606AF8,gabi::load<u32>(node+24),resourceName);
}
VERIFY(0x02606BEC,LookupCombined);
static u32 SelectObjectHeap(u32 control,u32 name,u32 path,u32 heap) {
 WWHD_FUNC(0x02607F80,u32,control,name,path,heap);
 if(!gabi::call<u32>(0x02603740,control,name,path,heap))return gabi::load<u32>(control+0x2040);
 if(!gabi::load<u32>(control+0x2048))gabi::call<void>(0x026079B4,control);
 if(FindSubstring(name,0x100E1C18)<0)return gabi::load<u32>(control+0x2048);
 return gabi::call<u32>(0x02607B68,control,name);
}
VERIFY(0x02607F80,SelectObjectHeap);
static u32 ChooseObjectHeap(u32 control,u32 name) {
 WWHD_FUNC(0x02607B68,u32,control,name);
 if(LiteralEquals(name,0x100E1BF0))return gabi::load<u32>(control+0x2050);
 if(LiteralEquals(name,0x100E1BF8))return gabi::load<u32>(control+0x2058);
 if(LiteralEquals(name,0x100E1C00))return gabi::load<u32>(control+0x205C);
 if(LiteralEquals(name,0x100E1C08))return gabi::load<u32>(control+0x2060);
 if(LiteralEquals(name,0x100E1C10))return gabi::load<u32>(control+0x2064);
 for(u32 i=0;i<22;++i){
  u32 heap=gabi::load<u32>(control+0x2078+i*4);
  u32 vt=gabi::load<u32>(heap+12);
  u32 target=gabi::load<u32>(vt+0x74);
  if(gabi::call_ptr<u32>(target,heap)>0x69000)return gabi::load<u32>(control+0x2078+i*4);
 }
 return gabi::load<u32>(control+0x2048);
}
VERIFY(0x02607B68,ChooseObjectHeap);
static void DeleteInfo(u32 control,u32 name,u32 path) {
 WWHD_FUNC(0x0260757C,void,control,name,path);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 gabi::store<u8>(str+139,0);gabi::store<u8>(str+12,0);
 gabi::store<u32>(str+4,0x100E1A2C);gabi::store<u32>(str+8,128);
 gabi::store<u32>(str,str+12);
 StringAssure(path);u32 pathChars=gabi::load<u32>(path);
 StringAssure(name);
 gabi::call<void>(0x02759C28,str,0x100E1BB0u,pathChars,gabi::load<u32>(name));
 u32 hash=StringHash(str);
 u32 root=gabi::load<u32>(control+0x1028);
 gabi::Local<be<u32>> key;*key=hash;
 u32 node=gabi::call<u32>(0x0260ACC8,control+0x1028,root,key.get());
 u32 infoAddress=node?node+24:0;
 if(!infoAddress){
  if(FindSubstring(name,0x100E1BAC)>=0)return;
  root=gabi::load<u32>(control+0x1028);
 }
 *key=hash;
 gabi::Local<be<u32>> again;*again=hash;
 node=gabi::call<u32>(0x0260ACC8,control+0x1028,root,again.get());
 if(node && node+24){
  root=gabi::call<u32>(0x0260AEFC,control+0x1028,root,key.get());
  gabi::store<u32>(control+0x1028,root);
  if(root)gabi::store<u8>(root+8,0);
 }
 gabi::call<void>(0x0260F61C,gabi::load<u32>(0x101F4F54),hash);
 u32 info=gabi::load<u32>(infoAddress);
 u32 heap=gabi::load<u32>(info+0xC0);
 if(heap){heap=gabi::load<u32>(info+0xC0);DestroyHeap(heap);info=gabi::load<u32>(infoAddress);}
 DestroyHeap(gabi::load<u32>(info+0xC4));
 if(LiteralEquals(name,0x100E1BB8)){
  gabi::call<void>(0x026072AC,control);
  gabi::call<void>(0x02607414,control);
 }
}
VERIFY(0x0260757C,DeleteInfo);
static void AppendString(u32 buffer,u32 source) {
 u32 data=gabi::load<u32>(buffer);
 u32 oldLength=Length(data),offset=oldLength;
 u32 vt=gabi::load<u32>(source+4),target=gabi::load<u32>(vt+20);
 if(s32(offset)<0)offset=0;
 gabi::call_ptr<void>(target,source);
 u32 length=Length(gabi::load<u32>(source));
 u32 left=gabi::load<u32>(buffer+8)-offset;
 if(s32(length)>=s32(left))length=left-1;
 if(s32(length)>0){
  vt=gabi::load<u32>(source+4);target=gabi::load<u32>(vt+20);
  u32 destination=data+offset;
  gabi::call_ptr<void>(target,source);
  gabi::call<void>(0xC0009988,destination,gabi::load<u32>(source),length,0u);
  u32 total=offset+length;
  if(s32(total)>s32(oldLength))gabi::store<u8>(data+total,0);
 }
}
static void LoadTableResources(u32 info,u32 name,u32 heap) {
 WWHD_FUNC(0x02604550,void,info,name,heap);
 u32 hash=StringHash(name);
 u32 idTableCount=gabi::load<u32>(0x100C45C0);
 u32 resourceCount=0,resourceRow=0,idCount=0,idRow=0;
 s32 tableCount=gabi::load<s32>(0x10080150);
 u32 resourceTable=0x1009BB1C,idTable=0x100DE8F4;
 for(s32 i=0;i<tableCount;++i){
  u32 row=resourceTable+u32(i)*12;
  if(gabi::load<u32>(row)==hash){resourceCount=gabi::load<u32>(row+8);resourceRow=i;break;}
 }
 for(s32 i=0;i<s32(idTableCount);++i){
  u32 row=idTable+u32(i)*12;
  if(gabi::load<u32>(row)==hash){idCount=gabi::load<u32>(row+8);idRow=i;break;}
 }
 gabi::call<void>(0x02604470,info,resourceCount,heap);
 gabi::call<void>(0x026044E0,info,idCount,heap);
 if(s32(resourceCount)<=0)return;
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 for(u32 i=0;i<resourceCount;++i){
  gabi::store<u8>(str+139,0);gabi::store<u32>(str+4,0x100E1A2C);
  gabi::store<u32>(str+8,128);gabi::store<u32>(str,str+12);
  gabi::store<u8>(str+12,0);
  u32 values=gabi::load<u32>(resourceTable+resourceRow*12+4);
  gabi::Local<SafeString> resource;
  resource->vtable=0x100E1994;resource->data=gabi::load<u32>(values+i*8);
  gabi::call<void>(0x0260A828,str);
  AppendString(str,gabi::ea(resource.get()));
  u32 chars=gabi::load<u32>(str);
  values=gabi::load<u32>(resourceTable+resourceRow*12+4);
  u8 first=gabi::load<u8>(chars);
  u32 type=gabi::load<u32>(values+i*8+4);
  if(!first)continue;
  u32 member=0x100E1C40+type*8;
  if(!gabi::load<s16>(member+2) || s32(idCount)<=0)continue;
  for(u32 id=0;id<idCount;++id){
   u32 names=gabi::load<u32>(idTable+idRow*12+4);
   gabi::Local<SafeString> candidate;
   candidate->vtable=0x100E1994;candidate->data=gabi::load<u32>(names+id*8);
   StringAssure(str);StringAssure(str);
   u32 saved=gabi::load<u32>(str);
   StringAssure(gabi::ea(candidate.get()));
   u32 other=gabi::load<u32>(gabi::ea(candidate.get()));
   if(saved!=other && !EqualBounded(gabi::load<u32>(str),other))continue;
   s16 selector=gabi::load<s16>(member+2);
   s16 adjustment=gabi::load<s16>(member);
   u32 object=info+u32(s32(adjustment));
   u32 target;
   if(selector<0)target=gabi::load<u32>(member+4);
   else{
    s16 offset=gabi::load<s16>(member+6);
    u32 vt=gabi::load<u32>(object+u32(s32(offset)));
    target=gabi::load<u32>(vt+u32(s32(selector))*8+4);
   }
   gabi::call_ptr<void>(target,object,i,id,str,heap);
   break;
  }
 }
}
VERIFY(0x02604550,LoadTableResources);
static void MaterialModelLoader(u32 info,u32 index,u32 id,u32 name,u32 heap) {
 WWHD_FUNC(0x0260A0B4,void,info,index,id,name,heap);
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 StripExtension(str,name,0x100E1C28);
 gabi::Local<SafeString> suffix;
 suffix->vtable=0x100E1994;suffix->data=0x100E1C2C;
 StringAssure(str);
 AppendString(str,gabi::ea(suffix.get()));
 u32 vt=gabi::load<u32>(str+4), target=gabi::load<u32>(vt+20);
 u32 archive=gabi::load<u32>(info+0x5C);
 u32 header=gabi::load<u32>(archive+20);
 gabi::call_ptr<void>(target,str);
 u32 entries=Relative(header+32);
 u32 found=gabi::call<u32>(0x027DF9B0,entries,gabi::load<u32>(str));
 if(s32(found)<0)return;
 archive=gabi::load<u32>(info+0x5C);
 header=gabi::load<u32>(archive+20);
 entries=Relative(header+32);
 u32 resource=Relative(entries+found*16+36);
 u32 resourceId=gabi::call<u32>(0x026031D0,archive,found);
 archive=gabi::load<u32>(info+0x5C);
 u32 archiveName=gabi::load<u32>(archive+32);
 StringAssure(archiveName);
 gabi::Local<SafeString> copy;
 copy->data=gabi::load<u32>(archiveName);
 copy->vtable=0x100E1994;
 u32 result=gabi::call<u32>(0x0272D384,resource,resourceId,copy.get(),heap);
 gabi::call<void>(0x026082D0,info,index,result);
 gabi::call<void>(0x026082EC,info,id,result);
}
VERIFY(0x0260A0B4,MaterialModelLoader);
static void LoadExtraResources(u32 info,u32 heap) {
 WWHD_FUNC(0x02604A08,void,info,heap);
 u32 archive=gabi::load<u32>(info+92);
 u32 header=gabi::load<u32>(archive+20);
 u32 count=gabi::load<u16>(header+0x66);
 if(!count)return;
 u32 data=gabi::call<u32>(0x0273B0D4,count*32,heap,4u);
 if(data){
  gabi::store<u32>(info+108,data);gabi::store<u32>(info+120,count);
  for(u32 i=0;i+1<count;++i)gabi::store<u32>(data+i*32,data+(i+1)*32);
  gabi::store<u32>(data+(count-1)*32,0);gabi::store<u32>(info+112,data);
 }
 gabi::Local<StringBuffer128> buffer;
 u32 str=gabi::ea(buffer.get());
 gabi::store<u32>(str,str+12);gabi::store<u8>(str+139,0);
 gabi::store<u32>(str+8,128);gabi::store<u8>(str+12,0);
 gabi::store<u32>(str+4,0x100E1A2C);
 for(u32 i=0;i<count;++i){
  u32 destination=gabi::load<u32>(str);
  archive=gabi::load<u32>(info+92);header=gabi::load<u32>(archive+20);
  u32 entries=Relative(header+76);
  gabi::Local<SafeString> name;
  name->vtable=0x100E1994;name->data=Relative(entries+i*16+32);
  u32 nameAddress=gabi::ea(name.get());
  gabi::call<void>(0x0260A824,nameAddress);
  u32 n=Length(gabi::load<u32>(nameAddress));
  u32 capacity=gabi::load<u32>(str+8);
  if(s32(n)>=s32(capacity))n=capacity-1;
  StringAssure(nameAddress);
  gabi::call<void>(0xC0009988,destination,gabi::load<u32>(nameAddress),n,0u);
  gabi::store<u8>(destination+n,0);
  if(FindSubstring(str,0x100E1B50)<0)continue;
  u32 hash=StringHash(str);
  archive=gabi::load<u32>(info+92);header=gabi::load<u32>(archive+20);
  entries=Relative(header+76);
  u32 reference=Relative(entries+i*16+36);
  u32 offset=gabi::load<u32>(reference);
  u32 value=offset?reference+offset:0;
  s32 capacityCount=gabi::load<s32>(info+120),used=gabi::load<s32>(info+116);
  u32 pool=info+104;
  if(used<capacityCount){
   u32 node=gabi::load<u32>(pool+4);
   if(node)gabi::store<u32>(pool+4,gabi::load<u32>(node));
   if(node){
    gabi::store<u32>(node,0);gabi::store<u32>(node+12,hash);
    gabi::store<u32>(node+16,0x100E1A9C);gabi::store<u32>(node+24,value);
    gabi::store<u32>(node+28,pool);gabi::store<u32>(node+4,0);
    gabi::store<u8>(node+8,1);gabi::store<u32>(node+20,hash);
   }
   u32 old=gabi::load<u32>(pool+12),root=gabi::load<u32>(pool);
   gabi::store<u32>(pool+12,old+1);
   root=gabi::call<u32>(0x0260AD08,pool,root,node);
   gabi::store<u32>(pool,root);gabi::store<u8>(root+8,0);
  } else {
   u32 root=gabi::load<u32>(pool);
   gabi::Local<be<u32>> key;*key=hash;
   u32 node=gabi::call<u32>(0x0260ACC8,pool,root,key.get());
   if(node && node+24)gabi::store<u32>(node+24,value);
  }
 }
}
VERIFY(0x02604A08,LoadExtraResources);
static bool Red(u32 node){return node && gabi::load<u8>(node+8);}
static u32 MoveRedLeft(u32 node){
 Flip(node);
 u32 right=gabi::load<u32>(node+4),left=gabi::load<u32>(right);
 if(Red(left)){
  u32 next=RotateRight(right,left);
  gabi::store<u32>(node+4,next);
  node=RotateLeft(node,next);
  Flip(node);
 }
 return node;
}
static u32 MoveRedRight(u32 node){
 Flip(node);
 u32 left=gabi::load<u32>(node);
 if(Red(gabi::load<u32>(left))){node=RotateRight(node,left);Flip(node);}
 return node;
}
static u32 Balance(u32 node){
 u32 right=gabi::load<u32>(node+4);
 if(Red(right))node=RotateLeft(node,right);
 u32 left=gabi::load<u32>(node);
 if(Red(left) && Red(gabi::load<u32>(left))){node=RotateRight(node,left);left=gabi::load<u32>(node);}
 if(Red(left) && Red(gabi::load<u32>(node+4)))Flip(node);
 return node;
}
static void ReleaseNode(u32 node){
 u32 vt=gabi::load<u32>(node+16);
 gabi::call_ptr<void>(gabi::load<u32>(vt+20),node);
}
static u32 DeleteTree(u32 tree,u32 node,u32 key){
 WWHD_FUNC(0x0260AEFC,u32,tree,node,key);
 u32 sought=gabi::load<u32>(key),old=gabi::load<u32>(node+12);
 u32 left=gabi::load<u32>(node);
 if(sought<old){
  if(!Red(left) && !Red(gabi::load<u32>(left)))node=MoveRedLeft(node);
  left=gabi::load<u32>(node);
  u32 replacement=gabi::call<u32>(0x0260AEFC,tree,left,key);
  gabi::store<u32>(node,replacement);
 } else {
  if(Red(left)){
   node=RotateRight(node,left);
   sought=gabi::load<u32>(key);old=gabi::load<u32>(node+12);
  }
  u32 right=gabi::load<u32>(node+4);
  if(sought==old && !right){ReleaseNode(node);return 0;}
  if(!Red(right) && !Red(gabi::load<u32>(right))){
   node=MoveRedRight(node);
   old=gabi::load<u32>(node+12);right=gabi::load<u32>(node+4);sought=gabi::load<u32>(key);
  }
  if(old==sought){
   u32 minimum=right,next=gabi::load<u32>(minimum);
   while(next){minimum=next;next=gabi::load<u32>(minimum);}
   u32 successor=gabi::call<u32>(0x0260ACC8,tree,right,minimum+12);
   left=gabi::load<u32>(right);
   if(!left){
    gabi::store<u32>(successor+4,0);
   }else{
    if(!Red(left) && !Red(gabi::load<u32>(left)))right=MoveRedLeft(right);
    left=gabi::load<u32>(right);
    u32 removed=gabi::call<u32>(0x0260A9D8,left);
    gabi::store<u32>(right,removed);
    right=Balance(right);
    gabi::store<u32>(successor+4,right);
   }
   gabi::store<u32>(successor,gabi::load<u32>(node));
   gabi::store<u8>(successor+8,gabi::load<u8>(node+8));
   ReleaseNode(node);
   node=successor;
  }else{
   u32 replacement=gabi::call<u32>(0x0260AEFC,tree,right,key);
   gabi::store<u32>(node+4,replacement);
  }
 }
 return Balance(node);
}
VERIFY(0x0260AEFC,DeleteTree);
static void Initialize128(u32 buffer,u32 vt=0x100E1A2C,bool clearFirst=true){
 gabi::store<u32>(buffer+4,vt);if(clearFirst)gabi::store<u8>(buffer+12,0);
 gabi::store<u32>(buffer+8,128);gabi::store<u8>(buffer+139,0);
 gabi::store<u32>(buffer,buffer+12);
}
static u32 ArchiveHeader(u32 info){return gabi::load<u32>(gabi::load<u32>(info+168)+20);}
static void FormatResourceName(u32 str,u32 entry,u32 format){
 gabi::Local<SafeString> name;
 u32 object=gabi::ea(name.get());
 name->vtable=0x100E1994;name->data=Relative(entry+32);
 gabi::call<void>(0x0260A824,object);
 gabi::call<void>(0x02759C28,str,format,gabi::load<u32>(object));
}
static void LoadAllResources(u32 info){
 WWHD_FUNC(0x026057F4,void,info);
 u32 header=ArchiveHeader(info);
 u32 textures=gabi::load<u16>(header+0x54),raw=gabi::load<u16>(header+0x66);
 u32 models=gabi::load<u16>(header+0x50),patterns=gabi::load<u16>(header+0x52);
 u32 color=gabi::load<u16>(header+0x58),materials=gabi::load<u16>(header+0x5C);
 u32 transforms=gabi::load<u16>(header+0x5A);
 u32 heap=gabi::load<u32>(info+196);
 gabi::call<void>(0x026055E4,info,models+textures+patterns+raw+materials+transforms+color,heap);
 gabi::Local<StringBuffer128> buffer;u32 str=gabi::ea(buffer.get());
 for(u32 i=0;i<models;++i){
  u32 entry=Relative(ArchiveHeader(info)+32)+i*16;
  u32 resource=Relative(entry+36);
  StringAssure(info+16);u32 archiveChars=gabi::load<u32>(info+16);
  StringAssure(info+92);u32 pathChars=gabi::load<u32>(info+92);
  u32 archive=gabi::load<u32>(info+168);
  u32 resourceId=gabi::call<u32>(0x026031D0,archive,i);
  gabi::Local<u8[76]> label;
  u32 labelResult=gabi::call<u32>(0x0260A8A4,label.get(),0x100E1B5Cu,archiveChars,pathChars);
  heap=gabi::load<u32>(info+196);
  u32 result=gabi::call<u32>(0x027F7B50,resource,resourceId,labelResult,heap);
  Initialize128(str);
  entry=Relative(ArchiveHeader(info)+32)+i*16;
  resource=Relative(entry+36);
  gabi::Local<SafeString> name;
  u32 nameAddress=gabi::ea(name.get());
  name->vtable=0x100E1994;name->data=Relative(resource+4);
  gabi::call<void>(0x0260A824,nameAddress);
  gabi::call<void>(0x02759C28,str,0x100E1B64u,gabi::load<u32>(nameAddress));
  gabi::call<void>(0x0260566C,info,str,result);
 }
 for(u32 i=0;i<textures;++i){
  u32 entry=Relative(ArchiveHeader(info)+40)+i*16;
  u32 resource=Relative(entry+36);
  heap=gabi::load<u32>(info+196);
  u32 result=gabi::call<u32>(0x027F78A8,resource,info+16,heap);
  entry=Relative(ArchiveHeader(info)+40)+i*16;
  gabi::Local<SafeString> name;
  name->vtable=0x100E1994;name->data=Relative(entry+32);
  gabi::call<void>(0x0260566C,info,name.get(),result);
 }
 for(u32 i=0;i<patterns;++i){
  heap=gabi::load<u32>(info+196);
  u32 animation=gabi::call<u32>(0x0273B050,36u,heap,4u);
  if(animation){
   u32 entry=Relative(ArchiveHeader(info)+36)+i*16;
   u32 resource=Relative(entry+36);
   gabi::store<u32>(animation+32,resource);
   gabi::store<u16>(animation+2,gabi::load<u32>(resource+8));
   u32 frames=gabi::load<u32>(resource+12);
   gabi::store<u8>(animation+8,0);gabi::store<u16>(animation+4,frames);
  }
  Initialize128(str);
  u32 entry=Relative(ArchiveHeader(info)+36)+i*16;
  FormatResourceName(str,entry,0x100E1B6C);
  gabi::call<void>(0x0260566C,info,str,animation);
 }
 for(u32 i=0;i<materials;++i){
  u32 archive=gabi::load<u32>(info+168);
  header=gabi::load<u32>(archive+20);
  u32 entry=Relative(header+56)+i*16,resource=Relative(entry+36);
  Initialize128(str);
  header=gabi::load<u32>(archive+20);
  entry=Relative(header+56)+i*16;
  FormatResourceName(str,entry,0x100E1B74);
  heap=gabi::load<u32>(info+196);
  u32 result=gabi::call<u32>(0x027F7A2C,resource,heap);
  gabi::call<void>(0x0260566C,info,str,result);
 }
 f32 initial=gabi::load<f32>(0x100E1B58);
 for(u32 i=0;i<transforms;++i){
  u32 archive=gabi::load<u32>(info+168);
  header=gabi::load<u32>(archive+20);
  u32 entry=Relative(header+52)+i*16,resource=Relative(entry+36);
  Initialize128(str);
  header=gabi::load<u32>(archive+20);entry=Relative(header+52)+i*16;
  FormatResourceName(str,entry,0x100E1B7C);
  heap=gabi::load<u32>(info+196);
  u32 animation=gabi::call<u32>(0x0273B050,16u,heap,4u);
  if(animation){
   gabi::store<u32>(animation+4,0x1016E4AC);gabi::store<u32>(animation+8,0);
   gabi::store<f32>(animation,initial);gabi::store<u32>(animation+12,0);
  }
  gabi::store<u32>(animation+12,resource);
  gabi::call<void>(0x0260566C,info,str,animation);
 }
 for(u32 i=0;i<color;++i){
  u32 archive=gabi::load<u32>(info+168);
  header=gabi::load<u32>(archive+20);
  u32 entry=Relative(header+48)+i*16,resource=Relative(entry+36);
  Initialize128(str);
  header=gabi::load<u32>(archive+20);entry=Relative(header+48)+i*16;
  FormatResourceName(str,entry,0x100E1B84);
  heap=gabi::load<u32>(info+196);
  u32 animation=gabi::call<u32>(0x0273B050,16u,heap,4u);
  if(animation){
   gabi::store<u32>(animation+4,0x1016E4CC);gabi::store<u32>(animation+8,0);
   gabi::store<f32>(animation,initial);gabi::store<u32>(animation+12,0);
  }
  gabi::store<u32>(animation+12,resource);
  gabi::call<void>(0x0260566C,info,str,animation);
 }
 for(u32 i=0;i<raw;++i){
  u32 archive=gabi::load<u32>(info+168);
  header=gabi::load<u32>(archive+20);
  u32 entry=Relative(header+76)+i*16;
  u32 reference=Relative(entry+36),offset=gabi::load<u32>(reference);
  u32 resource=offset?reference+offset:0;
  header=gabi::load<u32>(archive+20);entry=Relative(header+76)+i*16;
  Initialize128(str,0x100E1A14,false);
  gabi::Local<SafeString> name;
  u32 nameAddress=gabi::ea(name.get());
  name->vtable=0x100E1994;name->data=Relative(entry+32);
  gabi::call<void>(0x0260A824,nameAddress);
  u32 length=Length(gabi::load<u32>(nameAddress));
  u32 capacity=gabi::load<u32>(str+8),vt=gabi::load<u32>(nameAddress+4);
  if(s32(length)>=s32(capacity))length=capacity-1;
  gabi::call_ptr<void>(gabi::load<u32>(vt+20),nameAddress);
  gabi::call<void>(0xC0009988,str+12,gabi::load<u32>(nameAddress),length,0u);
  gabi::store<u32>(str+4,0x100E1A2C);gabi::store<u8>(str+12+length,0);
  gabi::call<void>(0x0260A828,str);
  s32 position=FindSubstring(str,0x100E1B8C,false);
  if(position>0)gabi::call<void>(0x020080C0,resource);
  entry=Relative(ArchiveHeader(info)+76)+i*16;
  name->vtable=0x100E1994;name->data=Relative(entry+32);
  gabi::call<void>(0x0260566C,info,name.get(),resource);
 }
}
VERIFY(0x026057F4,LoadAllResources);
}
