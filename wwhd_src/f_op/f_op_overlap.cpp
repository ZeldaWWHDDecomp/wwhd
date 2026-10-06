// Full HD overlap-process TU025DBBDC..025DBD23, trailing initializer attributed by adjacency.
#include "bindings.h"
namespace f_op_overlap_cpp {
static s32 fopOvlp_Draw(u32 task) {
    WWHD_FUNC(0x025DBBDC, s32, task);
    return gabi::call<s32>(0x025DF2C0, gabi::load<u32>(task + 0xC4), task);
}
VERIFY(0x025DBBDC, fopOvlp_Draw);
static s32 fopOvlp_Execute(u32 task) {
    WWHD_FUNC(0x025DBBE8, s32, task);
    return gabi::call<s32>(0x025DFCC4, gabi::load<u32>(task + 0xC4), task);
}
VERIFY(0x025DBBE8, fopOvlp_Execute);
static s32 fopOvlp_IsDelete(u32 task) {
    WWHD_FUNC(0x025DBBF4, s32, task);
    return gabi::call<s32>(0x025DFCCC, gabi::load<u32>(task + 0xC4), task);
}
VERIFY(0x025DBBF4, fopOvlp_IsDelete);
static s32 fopOvlp_Delete(u32 task) {
    WWHD_FUNC(0x025DBC00, s32, task);
    return gabi::call<s32>(0x025DFCD4, gabi::load<u32>(task + 0xC4), task);
}
VERIFY(0x025DBC00, fopOvlp_Delete);
static s32 fopOvlp_Create(u32 task) {
    WWHD_FUNC(0x025DBC28, s32, task);
    if (gabi::load<u8>(task + 0xC) == 0) {
        u32 profile = gabi::load<u32>(task + 0x10);
        gabi::call<void>(0x0201A33C, task + 0xC8, 1u);
        u32 method = gabi::load<u32>(profile + 0x24);
        gabi::store<u32>(task + 0xCC, 0xFFFFFFFFu);
        gabi::store<u32>(task + 0xC4, method);
    }
    return gabi::call<s32>(0x025DFCDC, gabi::load<u32>(task + 0xC4), task);
}
VERIFY(0x025DBC28, fopOvlp_Create);
static void __sinit_f_op_overlap_cpp() {
    WWHD_FUNC(0x025DBC90, void, (u32)0);
    sinit_header_statics(0x1048A524, 0x101F3694);
}
VERIFY(0x025DBC90, __sinit_f_op_overlap_cpp);
}
