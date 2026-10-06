// WWHD process nodes: complete 025DFFA8..025E032F translation unit.
#include "bindings.h"
namespace f_pc_node_cpp {
static s32 fpcNd_DrawMethod(u32 methods, u32 data) {
    WWHD_FUNC(0x025DFFA8, s32, methods, data);
    return gabi::call<s32>(0x025DFCAC, gabi::load<u32>(methods + 0x10), data);
}
VERIFY(0x025DFFA8, fpcNd_DrawMethod);
static s32 fpcNd_Draw(u32 node) {
    WWHD_FUNC(0x025DFFB0, s32, node);
    s32 result = 0;
    if (gabi::load<u8>(node + 0x1AC) == 0) {
        u32 saved = gabi::call<u32>(0x025DED64);
        gabi::call<void>(0x025DEAB4, node + 0xC0);
        result = fpcNd_DrawMethod(gabi::load<u32>(node + 0xBC), node);
        gabi::call<void>(0x025DEAB4, saved);
    }
    return result;
}
VERIFY(0x025DFFB0, fpcNd_Draw);
static s32 fpcNd_Execute(u32 node) {
    WWHD_FUNC(0x025E0024, s32, node);
    u32 saved = gabi::call<u32>(0x025DED64);
    gabi::call<void>(0x025DEAB4, node + 0xC0);
    s32 result = gabi::call<s32>(0x025DFCC4, gabi::load<u32>(node + 0xBC), node);
    gabi::call<void>(0x025DEAB4, saved);
    return result;
}
VERIFY(0x025E0024, fpcNd_Execute);
static u32 fpcNd_IsCreatingFromUnder(u32 node) {
    WWHD_FUNC(0x025E0080, u32, node);
    if (node == 0) return 0;
    u32 subtype = gabi::load<u32>(node + 0xB8);
    if (gabi::call<s32>(0x025DD258, gabi::load<u32>(0x101F3D60), subtype) == 0) return 0;
    u32 layer = node + 0xC0;
    if (gabi::call<s32>(0x025DEA6C, layer) == 0)
        return gabi::call<u32>(0x025DEE20, layer, 0x025E0080u, 0u);
    return node;
}
VERIFY(0x025E0080, fpcNd_IsCreatingFromUnder);
static s32 fpcNd_IsDeleteTiming(u32 node) {
    WWHD_FUNC(0x025E012C, s32, node);
    if (gabi::load<s32>(0x101F3D64) == 1 && fpcNd_IsCreatingFromUnder(node) != 0) return 0;
    return 1;
}
VERIFY(0x025E012C, fpcNd_IsDeleteTiming);
static s32 fpcNd_IsDelete(u32 node) {
    WWHD_FUNC(0x025E017C, s32, node);
    return gabi::call<s32>(0x025DFCCC, gabi::load<u32>(node + 0xBC), node);
}
VERIFY(0x025E017C, fpcNd_IsDelete);
static s32 fpcNd_Delete(u32 node) {
    WWHD_FUNC(0x025E0188, s32, node);
    if (gabi::call<s32>(0x025DEA30, node + 0xC0) != 0) return 0;
    if (gabi::call<s32>(0x025DFCD4, gabi::load<u32>(node + 0xBC), node) != 1) return 0;
    gabi::store<u32>(node + 0xB8, 0);
    return gabi::call<s32>(0x025DEB58, node + 0xC0);
}
VERIFY(0x025E0188, fpcNd_Delete);
static s32 fpcNd_Create(u32 node) {
    WWHD_FUNC(0x025E01FC, s32, node);
    if (gabi::load<u8>(node + 0xC) == 0) {
        u32 profile = gabi::load<u32>(node + 0x10);
        u32 subtype = gabi::call<u32>(0x025DD268, 0x101F3D60u);
        gabi::store<u32>(node + 0xB8, subtype);
        gabi::store<u32>(node + 0xBC, gabi::load<u32>(profile + 0x1C));
        gabi::call<void>(0x025DEBF0, node + 0xC0, node, node + 0xEC, 0x10u);
        gabi::store<u8>(node + 0x1AC, 0);
    }
    u32 saved = gabi::call<u32>(0x025DED64);
    gabi::call<void>(0x025DEAB4, node + 0xC0);
    s32 result = gabi::call<s32>(0x025DFCDC, gabi::load<u32>(node + 0xBC), node);
    gabi::call<void>(0x025DEAB4, saved);
    return result;
}
VERIFY(0x025E01FC, fpcNd_Create);
static void __sinit_f_pc_node_cpp() {
    WWHD_FUNC(0x025E029C, void, (u32)0);
    sinit_header_statics(0x1048AAD0, 0x101F3D3C);
}
VERIFY(0x025E029C, __sinit_f_pc_node_cpp);
}
