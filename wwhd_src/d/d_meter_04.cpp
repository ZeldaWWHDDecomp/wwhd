#include "gabi.h"
using namespace gabi;

u32 meterPaneFactory(u32 object) {
    WWHD_FUNC(0x0259CF60, u32, object);
    if (!object) object = call<u32>(0x0273AD10, u32(0xC4));
    if (object) {
        call<void>(0x0252CCBC, object);
        store<u32>(object, 0x1004CBD8);
    }
    return object;
}
VERIFY(0x0259CF60, meterPaneFactory);

void meterPaneDeleteA(u32 object, u32 flags) {
    WWHD_FUNC(0x0259CFB4, void, object, flags);
    if (object) {
        call<void>(0x0252CCFC, object, u32(0));
        if (flags & 1) call<void>(0x0273AF40, object);
    }
}
VERIFY(0x0259CFB4, meterPaneDeleteA);

void meterPaneDeleteB(u32 object, u32 flags) {
    WWHD_FUNC(0x0259D10C, void, object, flags);
    if (object) {
        call<void>(0x0252CCFC, object, u32(0));
        if (flags & 1) call<void>(0x0273AF40, object);
    }
}
VERIFY(0x0259D10C, meterPaneDeleteB);

void meterPaneDeleteC(u32 object, u32 flags) {
    WWHD_FUNC(0x0259D160, void, object, flags);
    if (object) {
        call<void>(0x0252CCFC, object, u32(0));
        if (flags & 1) call<void>(0x0273AF40, object);
    }
}
VERIFY(0x0259D160, meterPaneDeleteC);

void meterPaneEmpty(u32 object) {
    WWHD_FUNC(0x0259D1B4, void, object);
}
VERIFY(0x0259D1B4, meterPaneEmpty);
