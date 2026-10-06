#include "gabi.h"
struct ThrowstoneName { u8 data[8]; };
BOOL Throwstone_CreateHeap(u32 self) {
    WWHD_FUNC(0x025C527C, BOOL, self);
    gabi::Local<ThrowstoneName> name;
    gmem_st32(name.a + 4, 0x10056034); gmem_st32(name.a, 0x10056070);
    u32 data = gabi::call<u32>(0x026066C4, gmem_ld32(0x101F4F28), name.get(), 3);
    if (!data) return FALSE;
    u32 model = gabi::call<u32>(0x025E38E0, data, 0x80000, 0x11000002);
    gmem_st32(self + 0x3B4, model);
    return model != 0;
}
VERIFY(0x025C527C, Throwstone_CreateHeap);
BOOL Throwstone_CheckCreateHeap(u32 self) {
    WWHD_FUNC(0x025C5310, BOOL, self);
    return gabi::call<BOOL>(0x025C527C, self);
}
VERIFY(0x025C5310, Throwstone_CheckCreateHeap);
static void throwstone_SetScale(u32 self) {
    f32 x = gabi::load<f32>(self + 0x330), z = gabi::load<f32>(self + 0x338);
    u32 model = gmem_ld32(self + 0x3B4);
    f32 y = gabi::load<f32>(self + 0x334);
    gabi::store<f32>(model + 0xBC, x); gabi::store<f32>(model + 0xC4, z);
    gabi::store<f32>(model + 0xC0, y);
}
static void throwstone_CopyMatrix(u32 self) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = gabi::load<f32>(0x1048D0CC + i * 4);
    u32 model = gmem_ld32(self + 0x3B4);
    for (u32 i = 0; i < 12; ++i) gabi::store<f32>(model + 0xC8 + i * 4, values[i]);
    gabi::call<void>(0x028E90D4, 0x1048D0CC, self + 0x3B8);
}
s32 Throwstone_Create(u32 self) {
    WWHD_FUNC(0x025C5314, s32, self);
    s32 phase = gabi::call<s32>(0x02520460, self + 0x3AC, 0x10056070);
    if (phase != 4) return phase;
    u32 flags = gmem_ld32(self + 0x2E4);
    if (!(flags & 8)) {
        if (self) {
            gabi::call<void>(0x025D4ED0, self);
            flags = gmem_ld32(self + 0x2E4);
            gmem_st32(self + 0xB4, 0x1005604C);
        }
        gmem_st32(self + 0x2E4, flags | 8);
    }
    if (!gabi::call<BOOL>(0x025D63E8, self, 0x025C5310, 0x4C0)) return 5;
    throwstone_SetScale(self);
    gabi::call<void>(0x028E93CC, 0x1048D0CC, gabi::load<f32>(self + 0x314),
        gabi::load<f32>(self + 0x318), gabi::load<f32>(self + 0x31C));
    gabi::call<void>(0x025F1B48, 0x1048D0CC, (s32)gabi::load<s16>(self + 0x328),
        (s32)gabi::load<s16>(self + 0x32A), (s32)gabi::load<s16>(self + 0x32C));
    throwstone_CopyMatrix(self);
    return phase;
}
VERIFY(0x025C5314, Throwstone_Create);
BOOL Throwstone_Delete(u32 self) {
    WWHD_FUNC(0x025C5498, BOOL, self);
    gabi::call<void>(0x025204C8, self + 0x3AC, 0x10056070);
    return TRUE;
}
VERIFY(0x025C5498, Throwstone_Delete);
BOOL Throwstone_Execute(u32 self) {
    WWHD_FUNC(0x025C54C8, BOOL, self);
    BOOL active = gabi::call<u32>(0x02527028, self, 0x6A, 0, 0, 0, 0, 0, 0) != 0;
    gmem_st8(self + 0x3E8, active);
    if (active) {
        throwstone_SetScale(self);
        gabi::call<void>(0x028E93CC, 0x1048D0CC, gabi::load<f32>(self + 0x314),
            gabi::load<f32>(self + 0x318), gabi::load<f32>(self + 0x31C));
        gabi::call<void>(0x025F1B48, 0x1048D0CC, (s32)gabi::load<s16>(self + 0x328),
            (s32)gabi::load<s16>(self + 0x32A), (s32)gabi::load<s16>(self + 0x32C));
        throwstone_CopyMatrix(self);
    }
    return TRUE;
}
VERIFY(0x025C54C8, Throwstone_Execute);
BOOL Throwstone_Draw(u32 self) {
    WWHD_FUNC(0x025C55E8, BOOL, self);
    if (gmem_ld8(self + 0x3E8)) {
        u32 save = gmem_ld32(0x101F84DC);
        if (gabi::call<BOOL>(0x025B8B94, save + 0x644, 0x310)) {
            u32 env = gabi::call<u32>(0x02555D0C);
            gabi::call<void>(0x025626A4, env, 0, self + 0x314, self + 0x110);
            env = gabi::call<u32>(0x02555D0C);
            gabi::call<void>(0x02562F5C, env, gmem_ld32(self + 0x3B4), self + 0x110);
            gabi::call<void>(0x025E2DE0, gmem_ld32(self + 0x3B4), 0);
        }
    }
    return TRUE;
}
VERIFY(0x025C55E8, Throwstone_Draw);
void Throwstone_StaticInit() {
    WWHD_FUNC(0x025C566C, void);
    gmem_st32(0x104871E0, 0); gmem_st32(0x104871D8, 0);
    gmem_st32(0x104871E4, 0); gmem_st32(0x104871DC, 0);
    gabi::call<void>(0x028F026C, 0x101EE6E8);
    f32 lower = gabi::load<f32>(0x10056064), upper = gabi::load<f32>(0x10056068);
    gabi::store<f32>(0x104871CC, lower); gabi::store<f32>(0x104871D0, upper);
    gabi::call<void>(0x028ED6F8, 0x104871D4);
    gabi::call<void>(0x028F026C, 0x101EE6F4);
    gabi::call<void>(0x028EAB2C, 0x104871D5);
    gabi::call<void>(0x028F026C, 0x101EE700);
}
VERIFY(0x025C566C, Throwstone_StaticInit);
BOOL Throwstone_IsDelete(u32 self) { WWHD_FUNC(0x025C5714, BOOL, self); return TRUE; }
VERIFY(0x025C5714, Throwstone_IsDelete);
void Throwstone_Destructor(u32 self, u32 flags) {
    WWHD_FUNC(0x025C571C, void, self, flags);
    if (self) {
        gabi::call<void>(0x025D50BC, self, 0);
        if (flags & 1) gabi::call<void>(0x0273AF40, self);
    }
}
VERIFY(0x025C571C, Throwstone_Destructor);
void Throwstone_EmptyInit() { WWHD_FUNC(0x025C5770, void); }
VERIFY(0x025C5770, Throwstone_EmptyInit);

/* 025C5700 this TU's sead::SafeString copy (vtable 10056034, built in daThrowstone_c::CreateHeap): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void Throwstone_SafeString_deletingDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x025C5700, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x025C5700, Throwstone_SafeString_deletingDtor);
