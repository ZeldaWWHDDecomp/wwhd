#include "gabi.h"

struct LayerFilter { gabi::be<u32> callback; gabi::be<u32> data; };

BOOL fpcLyIt_OnlyHere(u32 layer, u32 callback, u32 data) {
    WWHD_FUNC(0x025DED70, BOOL, layer, callback, data);
    gabi::Local<LayerFilter> filter;
    filter->callback = callback;
    filter->data = data;
    return gabi::call<BOOL>(0x0201ABF0, layer + 0x10, 0x0201A9F4u, filter.get());
}
VERIFY(0x025DED70, fpcLyIt_OnlyHere);

BOOL fpcLyIt_OnlyHereLY(u32 layer, u32 callback, u32 data) {
    WWHD_FUNC(0x025DEDA8, BOOL, layer, callback, data);
    u32 previous = gabi::call<u32>(0x025DED64);
    gabi::call<void>(0x025DEAB4, layer);
    BOOL result = gabi::call<BOOL>(0x025DED70, layer, callback, data);
    gabi::call<void>(0x025DEAB4, previous);
    return result;
}
VERIFY(0x025DEDA8, fpcLyIt_OnlyHereLY);

u32 fpcLyIt_Judge(u32 layer, u32 callback, u32 data) {
    WWHD_FUNC(0x025DEE20, u32, layer, callback, data);
    gabi::Local<LayerFilter> filter;
    filter->callback = callback;
    filter->data = data;
    return gabi::call<u32>(0x0201AC60, layer + 0x10, 0x0201AA08u, filter.get());
}
VERIFY(0x025DEE20, fpcLyIt_Judge);

u32 fpcLyIt_AllJudge(u32 callback, u32 data) {
    WWHD_FUNC(0x025DEE58, u32, callback, data);
    gabi::Local<LayerFilter> filter;
    filter->callback = callback;
    filter->data = data;
    u32 current = gabi::call<u32>(0x025DEAA8);
    while (current != 0) {
        u32 result = gabi::call<u32>(0x0201AC60, current + 0x10, 0x0201AA08u, filter.get());
        if (result != 0) return result;
        current = gmem_ld32(current + 8);
    }
    return 0;
}
VERIFY(0x025DEE58, fpcLyIt_AllJudge);

// Adjacent header-generated initializer; exact generating header is qualified.
void fpcLyIt_StaticInit() {
    WWHD_FUNC(0x025DEED4, void);
    gmem_st32(0x1048A820, 0);
    gmem_st32(0x1048A818, 0);
    gmem_st32(0x1048A824, 0);
    gmem_st32(0x1048A81C, 0);
    gabi::call<void>(0x028F026C, 0x101F3B50u);
    u32 lower = gmem_ld32(0x10058308);
    u32 upper = gmem_ld32(0x1005830C);
    gmem_stf32(0x1048A80C, lower);
    gmem_stf32(0x1048A810, upper);
    gabi::call<void>(0x028ED6F8, 0x1048A814u);
    gabi::call<void>(0x028F026C, 0x101F3B5Cu);
    gabi::call<void>(0x028EAB2C, 0x1048A815u);
    gabi::call<void>(0x028F026C, 0x101F3B68u);
}
VERIFY(0x025DEED4, fpcLyIt_StaticInit);
