#include "gabi.h"
using namespace gabi;

s32 fpcMtd_Method_hd(u32 method, void* data) {
    WWHD_FUNC(0x025DFCAC, s32, method, data);
    if (!method) return 1;
    return call_ptr<s32>(method, data);
}
VERIFY(0x025DFCAC, fpcMtd_Method_hd);

s32 fpcMtd_Execute_hd(void* methods, void* data) {
    WWHD_FUNC(0x025DFCC4, s32, methods, data);
    return fpcMtd_Method_hd(load<u32>(ea(methods)+8), data);
}
VERIFY(0x025DFCC4, fpcMtd_Execute_hd);

s32 fpcMtd_IsDelete_hd(void* methods, void* data) {
    WWHD_FUNC(0x025DFCCC, s32, methods, data);
    return fpcMtd_Method_hd(load<u32>(ea(methods)+12), data);
}
VERIFY(0x025DFCCC, fpcMtd_IsDelete_hd);

s32 fpcMtd_Delete_hd(void* methods, void* data) {
    WWHD_FUNC(0x025DFCD4, s32, methods, data);
    return fpcMtd_Method_hd(load<u32>(ea(methods)+4), data);
}
VERIFY(0x025DFCD4, fpcMtd_Delete_hd);

s32 fpcMtd_Create_hd(void* methods, void* data) {
    WWHD_FUNC(0x025DFCDC, s32, methods, data);
    return fpcMtd_Method_hd(load<u32>(ea(methods)), data);
}
VERIFY(0x025DFCDC, fpcMtd_Create_hd);

void f_pc_method_sinit_hd() {
    WWHD_FUNC(0x025DFCE4, void);
    store<u32>(0x1048AA74, 0);
    store<u32>(0x1048AA6C, 0);
    store<u32>(0x1048AA78, 0);
    store<u32>(0x1048AA70, 0);
    call<void>(0x028F026C, at<void>(0x101F3CAC));
    f32 a = load<f32>(0x100583C8);
    f32 b = load<f32>(0x100583CC);
    store<f32>(0x1048AA60, a);
    store<f32>(0x1048AA64, b);
    call<void>(0x028ED6F8, at<void>(0x1048AA68));
    call<void>(0x028F026C, at<void>(0x101F3CB8));
    call<void>(0x028EAB2C, at<void>(0x1048AA69));
    call<void>(0x028F026C, at<void>(0x101F3CC4));
}
VERIFY(0x025DFCE4, f_pc_method_sinit_hd);
