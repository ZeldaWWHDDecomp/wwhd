#include "gabi.h"

u32 dChain_PacketCreate(u32 count, u32 tev, f32 scale) {
    WWHD_FUNC(0x0251A2CC, u32, count, tev, scale);
    u32 packet = gabi::call<u32>(0x02519814, 0, count, tev, scale);
    if (packet && !gmem_ld32(packet + 0xAC)) {
        u32 vtable = gmem_ld32(packet + 12);
        u32 destructor = gmem_ld32(vtable + 12);
        gabi::call_ptr<void>(destructor, packet, 3);
        return 0;
    }
    return packet;
}
VERIFY(0x0251A2CC, dChain_PacketCreate);

// Release the two GPU allocations owned by an inlined shader resource pair.
static void chain_ReleaseResources(u32 resource) {
    for (u32 offset : {0u, 0x254u}) {
        u32 item = resource + offset;
        gabi::call<void>(0x027BF7E8, item + 0x158);
        u32 allocation = gmem_ld32(item + 0x250);
        gmem_st32(item, 0);
        if (allocation) {
            u32 heap = gabi::call<u32>(0x02755FEC, gmem_ld32(0x101F8B4C), allocation);
            u32 table = gmem_ld32(heap + 12);
            gabi::call_ptr<void>(gmem_ld32(table + 0x3C), heap, gmem_ld32(item + 0x250));
            gmem_st32(item + 0x24C, 0);
            gmem_st32(item + 0x250, 0);
        }
    }
    gmem_st32(resource + 0x4B8, 0);
}
static void chain_FreeHeapField(u32 object, u32 offset) {
    u32 heap = gabi::call<u32>(0x02755FEC, gmem_ld32(0x101F8B4C), gmem_ld32(object + offset));
    u32 table = gmem_ld32(heap + 12);
    gabi::call_ptr<void>(gmem_ld32(table + 0x3C), heap, gmem_ld32(object + offset));
}
struct ChainString { u8 data[8]; };
u32 dChain_PacketConstruct(u32 self, u32 count, u32 tev, f32 scale) {
    WWHD_FUNC(0x02519814, u32, self, count, tev, scale);
    gabi::Local<ChainString> names, archive, filename;
    if (!self) { self = gabi::call<u32>(0x0273AD10, 0xB32C); if (!self) return 0; }
    gabi::call<void>(0x027F1278, self);
    gmem_st32(self + 0xAC, 0); gmem_st32(self + 0xB0, 0);
    u32 string = self + 0xB4;
    gmem_st32(self + 12, 0x1004B514);
    if (!string) string = gabi::call<u32>(0x0273AD10, 8);
    if (string) { gmem_st32(string + 4, 0); gmem_st32(string, 0); }
    gabi::call<void>(0x027FD6F4, self + 0xBC);
    gabi::call<void>(0x028EFFD0, self + 0xC8, 256, 0xA8, 0x0251B3AC);
    u32 packet = self + 0xA8C8;
    gabi::call<void>(0x027FB40C, packet);
    gmem_st32(packet + 12, 0x1016EFB4);
    gabi::call<void>(0x028F521C, packet + 0x74, 0x2F0);
    f32 zero = gabi::load<f32>(0x10145180), one = gabi::load<f32>(0x1014517C);
    gabi::store<f32>(packet + 0x74, zero);
    gabi::store<f32>(packet + 0x78, zero);
    gabi::store<f32>(packet + 0x7c, zero);
    gabi::store<f32>(packet + 0xf8, zero);
    gabi::store<f32>(packet + 0x80, one);
    gabi::store<f32>(packet + 0xe4, zero);
    gabi::store<f32>(packet + 0x90, one);
    gabi::store<f32>(packet + 0xc8, zero);
    gabi::store<f32>(packet + 0xf0, one);
    gabi::store<f32>(packet + 0xfc, zero);
    gabi::store<f32>(packet + 0xc0, one);
    gabi::store<f32>(packet + 0xa8, zero);
    gabi::store<f32>(packet + 0xcc, zero);
    gabi::store<f32>(packet + 0xa0, one);
    gabi::store<f32>(packet + 0xa4, zero);
    gabi::store<f32>(packet + 0x8c, zero);
    gabi::store<f32>(packet + 0x10c, zero);
    gabi::store<f32>(packet + 0xd4, zero);
    gabi::store<f32>(packet + 0xac, zero);
    gabi::store<f32>(packet + 0xe8, zero);
    gabi::store<f32>(packet + 0xd0, one);
    gabi::store<f32>(packet + 0x108, zero);
    gabi::store<f32>(packet + 0xb0, one);
    gabi::store<f32>(packet + 0xec, zero);
    gabi::store<f32>(packet + 0x98, zero);
    gabi::store<f32>(packet + 0xb4, zero);
    gabi::store<f32>(packet + 0x94, zero);
    gabi::store<f32>(packet + 0xf4, zero);
    gabi::store<f32>(packet + 0x110, one);
    gabi::store<f32>(packet + 0x9c, zero);
    gabi::store<f32>(packet + 0x100, one);
    gabi::store<f32>(packet + 0xbc, zero);
    gabi::store<f32>(packet + 0x114, zero);
    gabi::store<f32>(packet + 0xd8, zero);
    gabi::store<f32>(packet + 0x88, zero);
    gabi::store<f32>(packet + 0xc4, zero);
    gabi::store<f32>(packet + 0x118, zero);
    gabi::store<f32>(packet + 0x104, zero);
    gabi::store<f32>(packet + 0xe0, one);
    gabi::store<f32>(packet + 0xdc, zero);
    gabi::store<f32>(packet + 0x11c, zero);
    gabi::store<f32>(packet + 0x84, zero);
    gabi::store<f32>(packet + 0xb8, zero);
    gabi::store<f32>(packet + 0x120, one);
    for (u32 off : {0x124u, 0x144u, 0x164u})
        gabi::call<void>(0x028EFFD0, packet + off, 2, 16, 0x0251B41C);
    for (u32 off = 0x184; off <= 0x2D4; off += 0x30)
        if (!(packet + off)) gabi::call<void>(0x0273AD10, 0x30);
    for (u32 off = 0x304; off <= 0x354; off += 0x10)
        if (!(packet + off)) gabi::call<void>(0x0273AD10, 0x10);
    u32 resources = self + 0xAC2C, actual = resources;
    if (!actual) actual = gabi::call<u32>(0x0273AD10, 0x4C0);
    if (actual) {
        gabi::call<void>(0x028EFFD0, actual, 2, 0x254, 0x0251B448);
        gmem_st32(actual + 0x4A8, 0); gmem_st32(actual + 0x4AC, 0);
        gmem_st32(actual + 0x4B8, 0); gmem_st32(actual + 0x4B0, 0x20);
        gmem_st8(actual + 0x4BC, 0);
        gmem_st32(actual, 0); gmem_st32(actual + 0x254, 0);
    }
    u32 indices = self + 0xB0EC, shape = self + 0xB104, temporary = self + 0xB29C;
    gabi::call<void>(0x027B5430, indices);
    gabi::call<void>(0x027BDF7C, shape);
    gabi::call<void>(0x027BE6B8, temporary);
    gmem_st32(self + 0x98, tev); gmem_st32(self + 0xA8, count);
    gabi::store<f32>(self + 0xA4, scale); gabi::store<f32>(self + 0xA0, scale);
    gabi::store<f32>(self + 0x9C, scale);
    u32 points = gabi::call<u32>(0x028EFFD0, 0, count, 12, 0);
    gmem_st32(self + 0xAC, points);
    gmem_st32(names.a, 0x1004B4A8); gmem_st32(names.a + 4, 0x1004B484);
    u32 manager = gabi::call<u32>(0x027FFCBC);
    s32 found = gabi::call<s32>(0x027B90AC, gmem_ld32(manager + 4), names.get());
    u32 chosen = 0;
    if (found >= 0) {
        u32 total = gmem_ld32(manager + 8), objects = gmem_ld32(manager + 12);
        u32 item = objects + ((u32)found < total ? (u32)found * 36 : 0);
        if (!gmem_ld8(item + 32)) {
            u32 file = gmem_ld32(manager + 4);
            u32 limit = gmem_ld32(file + 28), data = 0;
            if ((u32)found < limit) data = gmem_ld32(file + 32) + (u32)found * 0x84;
            gabi::call<void>(0x02800B0C, item, data, 0);
            total = gmem_ld32(manager + 8); objects = gmem_ld32(manager + 12);
        }
        chosen = objects + ((u32)found < total ? (u32)found * 36 : 0);
    }
    gabi::call<void>(0x0280068C, self + 0xB0, chosen, 0);
    gmem_st32(resources + 0x4AC, 0x13); gmem_st32(resources + 0x4B4, 0x1004B508);
    for (u32 offset : {0u, 0x254u}) {
        u32 item = resources + offset;
        if (!gmem_ld32(item)) {
            u32 heap = gabi::call<u32>(0x02756140, gmem_ld32(0x101F8B4C));
            u32 table = gmem_ld32(heap + 12);
            u32 allocation = gabi::call_ptr<u32>(gmem_ld32(table + 0x34), heap, 0x480, 0x40);
            if (allocation) { gmem_st32(item + 0x250, allocation); gmem_st32(item + 0x24C, 0x24); }
            gmem_st32(item, gmem_ld32(item + 0x250));
        }
        gabi::call<void>(0x027FF478, item + 4, gmem_ld32(item), 0x24, resources + 0x4AC);
    }
    gmem_st32(resources + 0x4B8, 0); gmem_st8(resources + 0x4BC, 1);
    u32 index = 0;
    while (index < gmem_ld32(self + 0xB0)) {
        u32 entry = gmem_ld32(self + 0xB8);
        if (index < gmem_ld32(self + 0xB4)) entry += index * 20;
        u32 saved = gmem_ld32(entry);
        gmem_st32(entry, 0);
        for (u32 field : {4u, 12u}) {
            u32 data = gmem_ld32(entry + field + 4);
            if (data) {
                u32 j = 0, offset = 0;
                while ((s32)j < (s32)gmem_ld32(entry + field)) {
                    u32 object = data + offset, table = gmem_ld32(object + 0xF0);
                    gabi::call_ptr<void>(gmem_ld32(table + 12), object, 2);
                    ++j; offset += 0xF4; data = gmem_ld32(entry + field + 4);
                }
                chain_FreeHeapField(entry, field + 4);
                gmem_st32(entry + field, 0); gmem_st32(entry + field + 4, 0);
            }
        }
        gmem_st32(entry, saved);
        for (u32 field : {4u, 12u}) {
            u32 heap = gabi::call<u32>(0x02756140, gmem_ld32(0x101F8B4C));
            u32 table = gmem_ld32(heap + 12);
            u32 allocation = gabi::call_ptr<u32>(gmem_ld32(table + 0x34), heap, 0xF4, 4);
            if (allocation) gabi::call<void>(0x027BF734, allocation);
            if (allocation) { gmem_st32(entry + field + 4, allocation); gmem_st32(entry + field, 1); }
        }
        for (u32 j = 0; j < 2; ++j)
            gabi::call<void>(0x027FF530, saved, gmem_ld32(entry + 8 + j * 8), resources + 4 + j * 0x254, resources + 0x4AC, 0);
        ++index;
    }
    gabi::call<void>(0x027FD838, self + 0xBC, 1, 0);
    gabi::call<void>(0x027FB5D4, packet, 0);
    for (u32 j = 0; j < 256; ++j) gabi::call<void>(0x027FB5D4, self + 0xC8 + j * 0xA8, 0);
    gabi::call<void>(0x027B54E0, indices, 0x101D5CAC, 4, gmem_ld32(0x101D5828));
    gmem_st32(indices + 4, 4);
    u32 active = gmem_ld32(resources + 0x4A8);
    u32 selected = resources + active * 0x254, data = gmem_ld32(selected), end = data + 0x480;
    if (data < end) {
        do { for (u32 k = 0; k < 32; ++k) gmem_st8((data & ~31u) + k, 0); data += 32; } while (data < end);
        active = gmem_ld32(resources + 0x4A8); selected = resources + active * 0x254;
    }
    u32 vertices = gmem_ld32(0x101D5824); data = gmem_ld32(selected);
    for (u32 j = 0; j < vertices; ++j) {
        for (u32 k = 0; k < 3; ++k) gabi::store<f32>(data + j * 32 + k * 4, gabi::load<f32>(0x101D582C + j * 12 + k * 4));
        for (u32 k = 0; k < 3; ++k) gabi::store<f32>(data + j * 32 + 12 + k * 4, gabi::load<f32>(0x101D59DC + j * 12 + k * 4));
        for (u32 k = 0; k < 2; ++k) gabi::store<f32>(data + j * 32 + 24 + k * 4, gabi::load<f32>(0x101D5B8C + j * 8 + k * 4));
        vertices = gmem_ld32(0x101D5824);
    }
    if (vertices) { active = gmem_ld32(resources + 0x4A8); selected = resources + active * 0x254; }
    u32 other = resources + (!active ? 0x254 : 0);
    for (u32 j = 0; j < 36; ++j) {
        u32 from = gmem_ld32(selected), to = gmem_ld32(other);
        for (u32 k = 0; k < 8; ++k) gabi::store<f32>(to + j * 32 + k * 4, gabi::load<f32>(from + j * 32 + k * 4));
    }
    active = gmem_ld32(resources + 0x4A8); selected = resources + active * 0x254;
    gabi::call<void>(0x027B5E94, selected + 4, 0, gmem_ld32(selected + 0x150));
    active = gmem_ld32(resources + 0x4A8); gmem_st32(resources + 0x4A8, !active);
    gabi::call<void>(0x0274FBF8, gmem_ld32(0x101F8B18));
    gmem_st32(archive.a + 4, 0x1004B484); gmem_st32(filename.a + 4, 0x1004B484);
    gmem_st32(filename.a, 0x1004B4C4); gmem_st32(archive.a, 0x1004B4B4);
    u32 loaded = gabi::call<u32>(0x026124B0, gmem_ld32(0x101F4F7C), archive.get(), filename.get(), 0);
    gabi::call<void>(0x02773870, temporary, loaded, 0x1004B49C);
    bool equal = true;
    for (u32 off : {4u,8u,12u,16u,20u,24u,56u,52u,28u}) {
        if (gmem_ld32(shape + off) != gmem_ld32(temporary + off)) { equal = false; break; }
    }
    if (!equal) gabi::call<void>(0x027BDEB4, shape, temporary);
    else {
        u32 last = gmem_ld32(temporary + 48), first = gmem_ld32(temporary + 40);
        gmem_st32(shape + 0xDC, last); gmem_st32(shape + 40, first);
        gmem_st32(shape + 0xD4, first); gmem_st32(shape + 48, last);
    }
    u8 flags = gmem_ld8(shape + 0x190);
    gmem_st32(shape + 0x160, 1); gmem_st32(shape + 0x15C, 1); gmem_st32(shape + 0x164, 1);
    gmem_st8(shape + 0x190, flags | 2);
    gabi::call<void>(0x0274FCCC, gmem_ld32(0x101F8B18));
    return self;
}
VERIFY(0x02519814, dChain_PacketConstruct);

void dChain_PacketDestroy(u32 self, u32 flags) {
    WWHD_FUNC(0x0251A324, void, self, flags);
    if (!self) return;
    gmem_st32(self + 12, 0x1004B514);
    u32 resources = self + 0xAC2C;
    chain_ReleaseResources(resources);
    u32 index = 0, offset = 0;
    while ((s32)index < (s32)gmem_ld32(self + 0xBC)) {
        u32 count = gmem_ld32(self + 0xBC);
        u32 entry = gmem_ld32(self + 0xC0);
        if (index < count) entry += offset;
        gabi::call<void>(0x027BEBEC, entry + 0x10);
        gabi::call<void>(0x027BEBEC, entry + 0x2C);
        ++index; offset += 0x23C;
    }
    gabi::call<void>(0x027BEBEC, self + 0xA8D8);
    gabi::call<void>(0x027BEBEC, self + 0xA8F4);
    for (u32 i = 0; i < 256; ++i) {
        gabi::call<void>(0x027BEBEC, self + 0xD8 + i * 0xA8);
        gabi::call<void>(0x027BEBEC, self + 0xF4 + i * 0xA8);
    }
    u32 points = gmem_ld32(self + 0xAC);
    if (points) {
        gabi::call<void>(0x028F0164, points, -1, 12, 0, 1, 0);
        gmem_st32(self + 0xAC, 0);
    }
    gabi::call<void>(0x027BE2B0, self + 0xB104, 2);
    gabi::call<void>(0x027B54A0, self + 0xB0EC, 2);
    if (resources) {
        chain_ReleaseResources(resources);
        gabi::call<void>(0x028F0164, resources, 2, 0x254, 0x0251B4A4, 0, 0);
    }
    gabi::call<void>(0x027FB528, self + 0xA8C8, 0);
    gabi::call<void>(0x028F0164, self + 0xC8, 256, 0xA8, 0x0251B504, 0, 0);
    gabi::call<void>(0x027FD764, self + 0xBC, 2);
    u32 entries = gmem_ld32(self + 0xB8);
    if (entries) {
        index = 0; offset = 0;
        u32 count = gmem_ld32(self + 0xB4);
        while ((s32)index < (s32)count) {
            u32 entry = entries + offset;
            if (entry) {
                gmem_st32(entry, 0);
                for (u32 field : {4u, 12u}) {
                    u32 data = gmem_ld32(entry + field + 4);
                    if (!data) continue;
                    u32 j = 0, position = 0;
                    while ((s32)j < (s32)gmem_ld32(entry + field)) {
                        u32 object = data + position;
                        u32 table = gmem_ld32(object + 0xF0);
                        gabi::call_ptr<void>(gmem_ld32(table + 12), object, 2);
                        ++j; position += 0xF4;
                        data = gmem_ld32(entry + field + 4);
                    }
                    chain_FreeHeapField(entry, field + 4);
                    gmem_st32(entry + field, 0);
                    gmem_st32(entry + field + 4, 0);
                }
                count = gmem_ld32(self + 0xB4);
                entries = gmem_ld32(self + 0xB8);
            }
            ++index; offset += 20;
        }
        chain_FreeHeapField(self, 0xB8);
        gmem_st32(self + 0xB4, 0); gmem_st32(self + 0xB8, 0);
    }
    gabi::call<void>(0x027F13DC, self, 0);
    if (flags & 1) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0251A324, dChain_PacketDestroy);

struct ChainVector { u8 data[12]; };
struct ChainColor { u8 data[16]; };
struct ChainMatrix { u8 data[48]; };

struct ChainRenderState { u8 data[0x11C]; };
static void chain_BindAttributes(u32 shader, u32 vertex) {
    u32 selected = vertex + 0x10 + gmem_ld32(vertex + 0x4C) * 0x1C;
    u32 attributes = gmem_ld32(shader + 12) ? gmem_ld32(shader + 16) : 0;
    s32 first = gabi::load<s16>(attributes + 12);
    u32 stride = gmem_ld32(selected + 4), buffer = gmem_ld32(selected + 12);
    s32 second = gabi::load<s16>(attributes + 14);
    s32 third = gabi::load<s16>(attributes + 16);
    if (second != -1) gabi::call<void>(0xC0006900, second, buffer, stride);
    if (first != -1) gabi::call<void>(0xC0006A38, first, buffer, stride);
    if (third != -1) gabi::call<void>(0xC00068A8, third, buffer, stride);
}
void dChain_Render(u32 self, u32 pass) {
    WWHD_FUNC(0x0251A74C, void, self, pass);
    gabi::Local<ChainRenderState> state;
    gabi::Local<ChainVector> pos, dir, step;
    gabi::Local<ChainColor> quat;
    u32 mode = gmem_ld32(pass + 12), shader = 0;
    if ((s32)mode < 4) {
        u32 count = gmem_ld32(self + 0xB4), entries = gmem_ld32(self + 0xB8);
        if (mode < count) entries += mode * 20;
        shader = gmem_ld32(entries);
    }
    u32 graphics = gabi::call<u32>(0x027F29D4, 0x104B45C0);
    u32 program = gmem_ld32(shader), current = gmem_ld32(graphics + 4);
    if (program != current) {
        u8 flags = gmem_ld8(program);
        u32 oldBinary = gmem_ld32(graphics);
        if (flags & 2) {
            gmem_st8(program, flags & ~2u);
            gabi::call<void>(0x027BB9E0, program, 0);
        }
        u32 resource = gmem_ld32(program + 0x7C);
        u32 binary = gmem_ld32(resource + 0x28);
        if (oldBinary != binary) gabi::call<void>(0x027B9F68, binary);
        u32 size = gmem_ld32(program + 12);
        if (size) {
            gabi::call<void>(0xC00060E0, gmem_ld32(program + 4), size);
            gmem_st32(graphics, binary); gmem_st32(graphics + 4, program);
        } else {
            gabi::call<void>(0x027BB7CC, program);
            gmem_st32(graphics + 4, program); gmem_st32(graphics, binary);
        }
    }
    mode = gmem_ld32(pass + 12);
    if (!mode) {
        u32 shape = gmem_ld32(pass + 20);
        if (shape) chain_BindAttributes(shader, gmem_ld32(shape + 4));
    } else if (mode == 1 || mode == 2) {
        chain_BindAttributes(shader, gmem_ld32(self + 0xC0));
        u32 packet = self + 0xA8C8;
        u32 table = gmem_ld32(packet + 12);
        gabi::call_ptr<void>(gmem_ld32(table + 0x2C), packet, shader);
        if (mode == 2) {
            u32 additional = gmem_ld32(pass + 0x30);
            if (additional) {
                table = gmem_ld32(additional + 12);
                gabi::call_ptr<void>(gmem_ld32(table + 0x2C), additional, shader);
            }
        }
        u32 data = gmem_ld32(shader + 20) ? gmem_ld32(shader + 24) : 0;
        gabi::call<void>(0x027BE53C, self + 0xB104, data + 4, -1, 0);
        if (mode == 2) gabi::call<void>(0x027FFE54, pass, shader);
    }
    gabi::call<void>(0x02750250, state.get());
    u32 word = gmem_ld32(state.a + 0xEC);
    gmem_st32(state.a + 12, 2);
    gmem_st8(state.a + 0xE0, 1);
    gmem_st32(state.a + 0xEC, ((((word & 0xFFFFFFF0u) + 7) & 0xFFFFFF0Fu) + 16));
    gabi::call<void>(0x0280037C, gmem_ld32(pass + 12), state.get());
    gabi::call<void>(0x02750370, state.get());
    u32 count = gmem_ld32(self + 0xA8), points = gmem_ld32(self + 0xAC);
    f32 x = gabi::load<f32>(points), adjustment = gabi::load<f32>(0x1047BBDC);
    f32 base = gabi::load<f32>(0x1004B4DC);
    gabi::store<f32>(pos.a, x);
    f32 y = gabi::load<f32>(points + 4);
    f32 segment = (adjustment + base) * gabi::load<f32>(self + 0xA4);
    gabi::store<f32>(pos.a + 4, y);
    gabi::store<f32>(pos.a + 8, gabi::load<f32>(points + 8));
    u32 index = 0, offset = 0;
    if ((s32)index >= (s32)(count - 1)) return;
    u32 next = points + 12;
    f32 zero = gabi::load<f32>(0x1004B4E0);
    do {
        gabi::call<void>(0x0201ADE0, next, dir.get(), pos.get());
        f32 squared = gabi::call<f32>(0x028E8DD0, dir.get());
        f32 length = gabi::call<f32>(0x028F4384, squared);
        if (gabi::call<BOOL>(0x0201B47C, dir.get())) {
            gabi::call<void>(0x023126A0, quat.get(), dir.get());
            if (length > zero) {
                u32 packet = self + 0xC8 + offset;
                u32 indices = self + 0xB0EC, resources = self + 0xAC2C;
                do {
                    u32 table = gmem_ld32(packet + 12);
                    gabi::call_ptr<void>(gmem_ld32(table + 0x2C), packet, shader);
                    mode = gmem_ld32(pass + 12);
                    u32 size = gmem_ld32(self + 0xB4);
                    offset += 0xA8; packet += 0xA8;
                    u32 entries = gmem_ld32(self + 0xB8);
                    if (mode < size) entries += mode * 20;
                    u32 selector = gmem_ld32(resources + 0x4A8) ? 0 : 8;
                    gabi::call<void>(0x027BFE5C, gmem_ld32(entries + selector + 8));
                    u32 number = gmem_ld32(indices + 12);
                    if (number) gabi::call<void>(0xC0006178, gmem_ld32(indices + 4), number,
                        gmem_ld32(indices), gmem_ld32(indices + 8), 0, 1);
                    length -= segment;
                    gabi::call<void>(0x0201AE48, dir.get(), step.get(), segment);
                    gabi::call<void>(0x028E8D88, pos.get(), step.get(), pos.get());
                } while (length > zero);
            }
        }
        count = gmem_ld32(self + 0xA8); ++index; next += 12;
    } while ((s32)index < (s32)(count - 1));
}
VERIFY(0x0251A74C, dChain_Render);

void dChain_BuildPackets(u32 self) {
    WWHD_FUNC(0x0251AE90, void, self);
    gabi::Local<ChainMatrix> view, matrix;
    gabi::Local<ChainColor> color, first, second, quat;
    gabi::Local<ChainVector> pos, dir, step;
    const u32 graphics = 0x104B45C0;
    gabi::call<void>(0x0251AD2C, view.get(), graphics + 0x38);
    gabi::call<void>(0x0255F8F4, gmem_ld32(self + 0x98));
    u32 global = gmem_ld32(graphics + 0x148);
    gabi::call<void>(0x027FDA54, self + 0xBC, 0, view.get(), graphics + 0x14C, global + 0x240);
    u32 tev = gmem_ld32(self + 0x98);
    u32 packet = gmem_ld32(self + 0xC0);
    gabi::call<void>(0x0251ADCC, color.get(), tev + 0x90);
    tev = gmem_ld32(self + 0x98);
    f32 brightness = gabi::load<f32>(tev + 0x28);
    gabi::call<void>(0x0274D458, first.get(), color.get(), brightness);
    for (u32 i = 0; i < 4; ++i) gmem_st32(packet + 0x1C4 + i * 4, gmem_ld32(first.a + i * 4));
    tev = gmem_ld32(self + 0x98);
    packet = gmem_ld32(self + 0xC0);
    gabi::call<void>(0x0251ADCC, color.get(), tev + 0x160);
    tev = gmem_ld32(self + 0x98);
    brightness = gabi::load<f32>(tev + 0x16C);
    gabi::call<void>(0x0274D458, second.get(), color.get(), brightness);
    for (u32 i = 0; i < 4; ++i) gmem_st32(packet + 0x1D4 + i * 4, gmem_ld32(second.a + i * 4));
    gabi::call<void>(0x027FE010, self + 0xBC);
    gabi::call<void>(0x0255F84C);
    tev = gmem_ld32(self + 0x98);
    f32 r = (f32)gabi::load<s16>(tev + 0x90), b = (f32)gabi::load<s16>(tev + 0x94);
    f32 g = (f32)gabi::load<s16>(tev + 0x92), a = (f32)gabi::load<s16>(tev + 0x96);
    f32 denominator = gabi::load<f32>(0x1004B4E4);
    u32 shader = self + 0xA8C8;
    gabi::store<f32>(shader + 0xB4, r / denominator);
    gabi::store<f32>(shader + 0xB8, g / denominator);
    gabi::store<f32>(shader + 0xBC, b / denominator);
    gabi::store<f32>(shader + 0xC0, a / denominator);
    r = (f32)gmem_ld8(tev + 0x98); g = (f32)gmem_ld8(tev + 0x99);
    b = (f32)gmem_ld8(tev + 0x9A); a = (f32)gmem_ld8(tev + 0x9B);
    gabi::store<f32>(shader + 0xC8, g / denominator);
    gabi::store<f32>(shader + 0xCC, b / denominator);
    gabi::store<f32>(shader + 0xC4, r / denominator);
    gabi::store<f32>(shader + 0xD0, a / denominator);
    tev = gmem_ld32(self + 0x98);
    f32 alpha = gabi::load<f32>(tev + 0x24);
    gabi::call<void>(0x0274D2AC, shader + 0xC4, alpha);
    packet = gmem_ld32(self + 0xC0);
    for (u32 i = 0; i < 4; ++i) gmem_st32(shader + 0x94 + i * 4, gmem_ld32(packet + 0x1C4 + i * 4));
    gabi::call<void>(0x027FB678, shader);
    u32 points = gmem_ld32(self + 0xAC);
    u32 count = gmem_ld32(self + 0xA8);
    f32 x = gabi::load<f32>(points);
    f32 adjustment = gabi::load<f32>(0x1047BBDC);
    f32 base = gabi::load<f32>(0x1004B4DC);
    gabi::store<f32>(pos.a, x);
    f32 y = gabi::load<f32>(points + 4);
    f32 segment = (adjustment + base) * gabi::load<f32>(self + 0xA4);
    gabi::store<f32>(pos.a + 4, y);
    gabi::store<f32>(pos.a + 8, gabi::load<f32>(points + 8));
    u32 index = 0, packetOffset = 0;
    s16 angle = 0;
    if ((s32)index >= (s32)(count - 1)) return;
    f32 zero = gabi::load<f32>(0x1004B4E0);
    u32 next = points + 12;
    const u32 stackMatrix = 0x1048D0CC;
    do {
        gabi::call<void>(0x0201ADE0, next, dir.get(), pos.get());
        f32 squared = gabi::call<f32>(0x028E8DD0, dir.get());
        f32 length = gabi::call<f32>(0x028F4384, squared);
        if (gabi::call<BOOL>(0x0201B47C, dir.get())) {
            gabi::call<void>(0x023126A0, quat.get(), dir.get());
            while (length > zero) {
                f32 px = gabi::load<f32>(pos.a), pz = gabi::load<f32>(pos.a + 8), py = gabi::load<f32>(pos.a + 4);
                gabi::call<void>(0x028E93CC, stackMatrix, px, py, pz);
                gabi::call<void>(0x025F25CC, quat.get());
                gabi::call<void>(0x025F1C5C, stackMatrix, (s32)angle);
                f32 sz = gabi::load<f32>(self + 0xA4), sy = gabi::load<f32>(self + 0xA0), sx = gabi::load<f32>(self + 0x9C);
                gabi::call<void>(0x025F2518, sx, sy, sz);
                f32 values[12];
                for (u32 i = 0; i < 12; ++i) values[i] = gabi::load<f32>(stackMatrix + i * 4);
                for (u32 i = 0; i < 12; ++i) gabi::store<f32>(matrix.a + i * 4, values[i]);
                u32 entry = self + 0xC8 + packetOffset;
                gabi::call<void>(0x028E90D4, matrix.get(), entry + 0x74);
                gabi::call<void>(0x027FB678, entry);
                length -= segment;
                packetOffset += 0xA8;
                gabi::call<void>(0x0201AE48, dir.get(), step.get(), segment);
                gabi::call<void>(0x028E8D88, pos.get(), step.get(), pos.get());
                angle = (s16)(u16)((u16)angle + 0x4000);
            }
        }
        ++index;
        count = gmem_ld32(self + 0xA8);
        next += 12;
    } while ((s32)index < (s32)(count - 1));
}
VERIFY(0x0251AE90, dChain_BuildPackets);

void chain_MatrixCopy(u32 dest, u32 source) {
    WWHD_FUNC(0x0251AD2C, void, dest, source);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = gabi::load<f32>(source + i * 4);
    for (u32 i = 0; i < 12; ++i) gabi::store<f32>(dest + i * 4, values[i]);
}
VERIFY(0x0251AD2C, chain_MatrixCopy);

void chain_ColorToFloat(u32 dest, u32 source) {
    WWHD_FUNC(0x0251ADCC, void, dest, source);
    f32 r = (f32)gabi::load<s16>(source);
    f32 b = (f32)gabi::load<s16>(source + 4);
    f32 g = (f32)gabi::load<s16>(source + 2);
    f32 a = (f32)gabi::load<s16>(source + 6);
    f32 denominator = gabi::load<f32>(0x1004B4E4);
    gabi::store<f32>(dest, r / denominator);
    gabi::store<f32>(dest + 4, g / denominator);
    gabi::store<f32>(dest + 8, b / denominator);
    gabi::store<f32>(dest + 12, a / denominator);
}
VERIFY(0x0251ADCC, chain_ColorToFloat);

void chain_StaticInit() {
    WWHD_FUNC(0x0251B304, void);
    gmem_st32(0x1046F04C, 0); gmem_st32(0x1046F044, 0);
    gmem_st32(0x1046F050, 0); gmem_st32(0x1046F048, 0);
    gabi::call<void>(0x028F026C, 0x101D5D3Cu);
    u32 lower = gmem_ld32(0x1004B4FC), upper = gmem_ld32(0x1004B500);
    gmem_stf32(0x1046F038, lower); gmem_stf32(0x1046F03C, upper);
    gabi::call<void>(0x028ED6F8, 0x1046F040u);
    gabi::call<void>(0x028F026C, 0x101D5D48u);
    gabi::call<void>(0x028EAB2C, 0x1046F041u);
    gabi::call<void>(0x028F026C, 0x101D5D54u);
}
VERIFY(0x0251B304, chain_StaticInit);

void chain_InlineDelete(u32 self, u32 flags) {
    WWHD_FUNC(0x0251B398, void, self, flags);
    if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0251B398, chain_InlineDelete);

u32 chain_InlinePacketCtor(u32 self) {
    WWHD_FUNC(0x0251B3AC, u32, self);
    if (!self) self = gabi::call<u32>(0x0273AD10, 0xA8);
    if (self) {
        gabi::call<void>(0x027FB40C, self);
        gmem_st32(self + 12, 0x1016EF84);
        gabi::call<void>(0x028F521C, self + 0x74, 0x34);
        if (self + 0x74 == 0) gabi::call<u32>(0x0273AD10, 0x30);
    }
    return self;
}
VERIFY(0x0251B3AC, chain_InlinePacketCtor);

u32 chain_InlineColorCtor(u32 self) {
    WWHD_FUNC(0x0251B41C, u32, self);
    return self ? self : gabi::call<u32>(0x0273AD10, 0x10);
}
VERIFY(0x0251B41C, chain_InlineColorCtor);

u32 chain_InlineResourceCtor(u32 self) {
    WWHD_FUNC(0x0251B448, u32, self);
    if (!self) self = gabi::call<u32>(0x0273AD10, 0x254);
    if (self) {
        gabi::call<void>(0x027B5BD8, self + 4);
        gabi::call<void>(0x027BF734, self + 0x158);
        gmem_st32(self + 0x250, 0);
        gmem_st32(self + 0x24C, 0);
    }
    return self;
}
VERIFY(0x0251B448, chain_InlineResourceCtor);

void chain_InlineResourceDtor(u32 self, u32 flags) {
    WWHD_FUNC(0x0251B4A4, void, self, flags);
    if (self) {
        gabi::call<void>(0x027BF880, self + 0x158, 2);
        gabi::call<void>(0x027B5CBC, self + 4, 2);
        if (flags & 1) gabi::call<void>(0x0273AF40, self);
    }
}
VERIFY(0x0251B4A4, chain_InlineResourceDtor);

void chain_InlinePacketDtor(u32 self, u32 flags) {
    WWHD_FUNC(0x0251B504, void, self, flags);
    if (self) {
        gabi::call<void>(0x027FB528, self, 0);
        if (flags & 1) gabi::call<void>(0x0273AF40, self);
    }
}
VERIFY(0x0251B504, chain_InlinePacketDtor);

void chain_InlineEmpty0(u32 self) { WWHD_FUNC(0x0251B558, void, self); }
VERIFY(0x0251B558, chain_InlineEmpty0);
void chain_InlineEmpty1(u32 self) { WWHD_FUNC(0x0251B55C, void, self); }
VERIFY(0x0251B55C, chain_InlineEmpty1);
