#include "gabi.h"

// HD passes three integer arguments: this, collision data, correction flags.
BOOL dBgWDeform_Set(u32 self, u32 data, u32 flags) {
    WWHD_FUNC(0x024F49E8, BOOL, self, data, flags);
    if (gabi::call<BOOL>(0x0200A030, self, data, 0x33, 0)) return 1;
    gmem_st32(self + 0xBC, flags);
    if (flags & 1) return 0;
    u32 collision = gmem_ld32(self + 0x94);
    u32 count = gmem_ld32(collision);
    u32 vertices = gabi::call<u32>(0x0273ADAC, count * 12);
    gmem_st32(self + 0xC0, vertices);
    return vertices == 0;
}
VERIFY(0x024F49E8, dBgWDeform_Set);

// Adjacent header math initializer; precise generating-header attribution qualified.
void dBgWDeform_StaticInit() {
    WWHD_FUNC(0x024F4A78, void);
    gmem_st32(0x1046EE00, 0);
    gmem_st32(0x1046EDF8, 0);
    gmem_st32(0x1046EE04, 0);
    gmem_st32(0x1046EDFC, 0);
    gabi::call<void>(0x028F026C, 0x101D5378u);
    u32 lower = gmem_ld32(0x10044070);
    u32 upper = gmem_ld32(0x10044074);
    gmem_stf32(0x1046EDEC, lower);
    gmem_stf32(0x1046EDF0, upper);
    gabi::call<void>(0x028ED6F8, 0x1046EDF4u);
    gabi::call<void>(0x028F026C, 0x101D5384u);
    gabi::call<void>(0x028EAB2C, 0x1046EDF5u);
    gabi::call<void>(0x028F026C, 0x101D5390u);
}
VERIFY(0x024F4A78, dBgWDeform_StaticInit);
