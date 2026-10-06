#include "gabi.h"

u32 dBgWSv_Construct(u32 self) {
    WWHD_FUNC(0x024F5A18, u32, self);
    if (!self) self = gabi::call<u32>(0x0273AD10, 0xC4);
    if (self) {
        gabi::call<void>(0x024F23F4, self);
        gmem_st32(self + 0xC0, 0);
        gmem_st32(self + 4, 0x100445D0);
        gmem_st32(self + 0xBC, 0);
    }
    return self;
}
VERIFY(0x024F5A18, dBgWSv_Construct);

BOOL dBgWSv_Set(u32 self, u32 data, u32 flags) {
    WWHD_FUNC(0x024F5A78, BOOL, self, data, flags);
    if (gabi::call<BOOL>(0x0200A030, self, data, 0x63, 0)) return 1;
    gmem_st32(self + 0xBC, flags);
    if (flags & 1) return 0;
    u32 collision = gmem_ld32(self + 0x94);
    u32 vertices = gabi::call<u32>(0x0273ADAC, gmem_ld32(collision) * 12);
    gmem_st32(self + 0xC0, vertices);
    return vertices == 0;
}
VERIFY(0x024F5A78, dBgWSv_Set);

void dBgWSv_CopyBackVtx(u32 self) {
    WWHD_FUNC(0x024F5B08, void, self);
    u32 dest = gmem_ld32(self + 0xC0);
    if (!dest) return;
    u32 source = gmem_ld32(self + 0x90);
    if (!source) return;
    u32 collision = gmem_ld32(self + 0x94);
    if ((s32)gmem_ld32(collision) <= 0) return;
    u32 index = 0;
    u32 offset = 0;
    while (true) {
        u32 x = gmem_ld32(source + offset);
        u32 input = source + offset;
        dest += offset;
        gmem_st32(dest, x);
        gmem_st32(dest + 4, gmem_ld32(input + 4));
        gmem_st32(dest + 8, gmem_ld32(input + 8));
        collision = gmem_ld32(self + 0x94);
        ++index;
        s32 count = (s32)gmem_ld32(collision);
        offset += 12;
        if ((s32)index >= count) return;
        source = gmem_ld32(self + 0x90);
        dest = gmem_ld32(self + 0xC0);
    }
}
VERIFY(0x024F5B08, dBgWSv_CopyBackVtx);

BOOL dBgWSv_CrrPosWork(u32 self, u32 pos, u32 v0, u32 v1, u32 v2) {
    WWHD_FUNC(0x024F5B78, BOOL, self, pos, v0, v1, v2);
    u32 o0 = v0 * 12, o1 = v1 * 12, o2 = v2 * 12;
    u32 back = gmem_ld32(self + 0xC0);
    f32 z0 = gabi::load<f32>(back + o0 + 8);
    f32 x0 = gabi::load<f32>(back + o0);
    f32 bz = gabi::load<f32>(back + o1 + 8) - z0;
    f32 bx = gabi::load<f32>(back + o1) - x0;
    f32 cx = gabi::load<f32>(back + o2) - x0;
    f32 cz = gabi::load<f32>(back + o2 + 8) - z0;
    f32 epsilon = gabi::load<f32>(0x100030B8);
    if (std::fabs(bz) < epsilon || std::fabs(cz) < epsilon) return 1;
    f32 ratioC = cx / cz;
    f32 denB = gabi::fnmsubs(ratioC, bz, bx);
    f32 ratioB = bx / bz;
    f32 denC = gabi::fnmsubs(ratioB, cz, cx);
    if (std::fabs(denB) < epsilon || std::fabs(denC) < epsilon) return 1;
    f32 px = gabi::load<f32>(pos) - x0;
    gabi::store<f32>(pos, px);
    back = gmem_ld32(self + 0xC0);
    f32 pz = gabi::load<f32>(pos + 8) - gabi::load<f32>(back + o0 + 8);
    f32 numC = gabi::fnmsubs(ratioB, pz, px);
    gabi::store<f32>(pos + 8, pz);
    f32 numB = gabi::fnmsubs(ratioC, pz, px);
    u32 current = gmem_ld32(self + 0x90);
    f32 wC = numC / denC;
    f32 ax = gabi::load<f32>(current + o0);
    f32 tx = gabi::load<f32>(current + o2);
    f32 wB = numB / denB;
    f32 sx = gabi::load<f32>(current + o1);
    f32 az = gabi::load<f32>(current + o0 + 8);
    f32 sz = gabi::load<f32>(current + o1 + 8);
    f32 tz = gabi::load<f32>(current + o2 + 8);
    f32 dxB = sx - ax, dzB = sz - az;
    f32 dxC = tx - ax, dzC = tz - az;
    f32 resultX = gabi::fmadds(wB, dxB, wC * dxC);
    f32 resultZ = gabi::fmadds(wB, dzB, wC * dzC);
    gabi::store<f32>(pos, resultX);
    gabi::store<f32>(pos + 8, resultZ);
    current = gmem_ld32(self + 0x90);
    gabi::store<f32>(pos, resultX + gabi::load<f32>(current + o0));
    current = gmem_ld32(self + 0x90);
    gabi::store<f32>(pos + 8, resultZ + gabi::load<f32>(current + o0 + 8));
    return 0;
}
VERIFY(0x024F5B78, dBgWSv_CrrPosWork);

void dBgWSv_CrrPos(u32 self, u32 poly, u32 user, u32 accept, u32 pos, u32 angle, u32 shape) {
    WWHD_FUNC(0x024F5CBC, void, self, poly, user, accept, pos, angle, shape);
    if ((gmem_ld32(self + 0xBC) & 1) || !accept) return;
    u32 index = gmem_ld16(poly);
    u32 collision = gmem_ld32(self + 0x94);
    u32 triangle = gmem_ld32(collision + 12) + index * 10;
    u32 v0 = gmem_ld16(triangle), v1 = gmem_ld16(triangle + 2), v2 = gmem_ld16(triangle + 4);
    if (!gabi::call<BOOL>(0x024F5B78, self, pos, v0, v1, v2)) return;
    if (!gabi::call<BOOL>(0x024F5B78, self, pos, v1, v2, v0)) return;
    gabi::call<BOOL>(0x024F5B78, self, pos, v2, v0, v1);
}
VERIFY(0x024F5CBC, dBgWSv_CrrPos);

BOOL dBgWSv_TransPosWork(u32 self, u32 pos, u32 v0, u32 v1, u32 v2) {
    WWHD_FUNC(0x024F5D84, BOOL, self, pos, v0, v1, v2);
    u32 o0 = v0 * 12, o1 = v1 * 12, o2 = v2 * 12;
    u32 back = gmem_ld32(self + 0xC0);
    f32 x0 = gabi::load<f32>(back + o0), z0 = gabi::load<f32>(back + o0 + 8);
    f32 bz = gabi::load<f32>(back + o1 + 8) - z0;
    f32 bx = gabi::load<f32>(back + o1) - x0;
    f32 cx = gabi::load<f32>(back + o2) - x0;
    f32 cz = gabi::load<f32>(back + o2 + 8) - z0;
    f32 epsilon = gabi::load<f32>(0x100030B8);
    if (std::fabs(bz) < epsilon || std::fabs(cz) < epsilon) return 1;
    f32 ratioC = cx / cz;
    f32 denB = gabi::fnmsubs(ratioC, bz, bx);
    f32 ratioB = bx / bz;
    f32 denC = gabi::fnmsubs(ratioB, cz, cx);
    if (std::fabs(denB) < epsilon || std::fabs(denC) < epsilon) return 1;
    f32 px = gabi::load<f32>(pos) - x0, pz = gabi::load<f32>(pos + 8) - z0;
    f32 wB = gabi::fnmsubs(ratioC, pz, px) / denB;
    f32 wC = gabi::fnmsubs(ratioB, pz, px) / denC;
    f32 zero = gabi::load<f32>(0x100445BC), one = gabi::load<f32>(0x100445C0);
    if (wB < zero || wB > one || wC < zero || wC > one) return 1;
    u32 current = gmem_ld32(self + 0x90);
    f32 ax = gabi::load<f32>(current + o0), bxNew = gabi::load<f32>(current + o1) - ax;
    f32 ay = gabi::load<f32>(current + o0 + 4), byNew = gabi::load<f32>(current + o1 + 4) - ay;
    f32 az = gabi::load<f32>(current + o0 + 8), bzNew = gabi::load<f32>(current + o1 + 8) - az;
    f32 cxNew = gabi::load<f32>(current + o2) - ax;
    f32 cyNew = gabi::load<f32>(current + o2 + 4) - ay;
    f32 czNew = gabi::load<f32>(current + o2 + 8) - az;
    f32 resultX = gabi::fmadds(wB, bxNew, wC * cxNew);
    f32 resultY = gabi::fmadds(wB, byNew, wC * cyNew);
    f32 resultZ = gabi::fmadds(wB, bzNew, wC * czNew);
    gabi::store<f32>(pos, resultX);
    gabi::store<f32>(pos + 4, resultY);
    gabi::store<f32>(pos + 8, resultZ);
    current = gmem_ld32(self + 0x90);
    gabi::store<f32>(pos, resultX + gabi::load<f32>(current + o0));
    current = gmem_ld32(self + 0x90);
    gabi::store<f32>(pos + 4, resultY + gabi::load<f32>(current + o0 + 4));
    current = gmem_ld32(self + 0x90);
    gabi::store<f32>(pos + 8, resultZ + gabi::load<f32>(current + o0 + 8));
    return 0;
}
VERIFY(0x024F5D84, dBgWSv_TransPosWork);

void dBgWSv_TransPos(u32 self, u32 poly, u32 user, u32 accept, u32 pos, u32 angle, u32 shape) {
    WWHD_FUNC(0x024F5F38, void, self, poly, user, accept, pos, angle, shape);
    if ((gmem_ld32(self + 0xBC) & 1) || !accept) return;
    u32 index = gmem_ld16(poly);
    u32 collision = gmem_ld32(self + 0x94);
    u32 triangle = gmem_ld32(collision + 12) + index * 10;
    u32 v0 = gmem_ld16(triangle), v1 = gmem_ld16(triangle + 2), v2 = gmem_ld16(triangle + 4);
    if (!gabi::call<BOOL>(0x024F5D84, self, pos, v0, v1, v2)) return;
    if (!gabi::call<BOOL>(0x024F5D84, self, pos, v1, v2, v0)) return;
    gabi::call<BOOL>(0x024F5D84, self, pos, v2, v0, v1);
}
VERIFY(0x024F5F38, dBgWSv_TransPos);

void dBgWSv_StaticInit() {
    WWHD_FUNC(0x024F6000, void);
    gmem_st32(0x1046EE38, 0); gmem_st32(0x1046EE30, 0);
    gmem_st32(0x1046EE3C, 0); gmem_st32(0x1046EE34, 0);
    gabi::call<void>(0x028F026C, 0x101D53C0u);
    u32 lower = gmem_ld32(0x100445C8), upper = gmem_ld32(0x100445CC);
    gmem_stf32(0x1046EE24, lower); gmem_stf32(0x1046EE28, upper);
    gabi::call<void>(0x028ED6F8, 0x1046EE2Cu);
    gabi::call<void>(0x028F026C, 0x101D53CCu);
    gabi::call<void>(0x028EAB2C, 0x1046EE2Du);
    gabi::call<void>(0x028F026C, 0x101D53D8u);
}
VERIFY(0x024F6000, dBgWSv_StaticInit);

void dBgWSv_MatrixCrrPos(u32 self, u32 poly, u32 user, u32 accept, u32 pos, u32 angle, u32 shape) {
    WWHD_FUNC(0x024F6094, void, self, poly, user, accept, pos, angle, shape);
}
VERIFY(0x024F6094, dBgWSv_MatrixCrrPos);
