#include "wwhd.h"
#include "gabi.h"

// HD global counter storage; unsigned arithmetic preserves the PPC word wrap.
void cCt_Counter(s32 resetCounter1) {
    WWHD_FUNC(0x0200E6EC, void, resetCounter1);
    u32 counter0 = gabi::load<u32>(0x101FF558);
    if (resetCounter1 == 1) {
        gabi::store<u32>(0x101FF55C, 0);
    } else {
        u32 counter1 = gabi::load<u32>(0x101FF55C);
        gabi::store<u32>(0x101FF55C, counter1 + 1u);
    }
    gabi::store<u32>(0x101FF558, counter0 + 1u);
}
VERIFY(0x0200E6EC, cCt_Counter);

// HD emitted per-TU math/header global initialization following Counter.
void c_counter_static_init() {
    WWHD_FUNC(0x0200E728, void);
    gabi::store<u32>(0x101FF550, 0);
    gabi::store<u32>(0x101FF548, 0);
    gabi::store<u32>(0x101FF554, 0);
    gabi::store<u32>(0x101FF54C, 0);
    gabi::call<void>(0x028F026C, gabi::at<void>(0x1018C6D8));
    f32 negativePi = gabi::load<f32>(0x10001E3C);
    f32 positivePi = gabi::load<f32>(0x10001E40);
    gabi::store<f32>(0x101FF53C, negativePi);
    gabi::store<f32>(0x101FF540, positivePi);
    gabi::call<void>(0x028ED6F8, gabi::at<void>(0x101FF544));
    gabi::call<void>(0x028F026C, gabi::at<void>(0x1018C6E4));
    gabi::call<void>(0x028EAB2C, gabi::at<void>(0x101FF545));
    gabi::call<void>(0x028F026C, gabi::at<void>(0x1018C6F0));
}
VERIFY(0x0200E728, c_counter_static_init);
