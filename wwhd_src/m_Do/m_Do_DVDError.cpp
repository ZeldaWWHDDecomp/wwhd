// TU attribution inferred from HD adjacency (after the m_Do_audio companions 20F0/20F8/2100; after the printf initializer 28C8, before vibration 29F0); not proven by symbols or strings
#include "gabi.h"
using namespace gabi;

// TU attribution follows its separate registration cluster between audio and DVD thread.
// The original GameCube watcher APIs have no identified HD implementations here.
void m_Do_DVDError_sinit_hd() {
    WWHD_FUNC(0x025E2114, void);
    store<u32>(0x1048CE18, 0);
    store<u32>(0x1048CE10, 0);
    store<u32>(0x1048CE1C, 0);
    store<u32>(0x1048CE14, 0);
    call<void>(0x028F026C, at<void>(0x101F4708));
    f32 a = load<f32>(0x10058510);
    f32 b = load<f32>(0x10058514);
    store<f32>(0x1048CE04, a);
    store<f32>(0x1048CE08, b);
    call<void>(0x028ED6F8, at<void>(0x1048CE0C));
    call<void>(0x028F026C, at<void>(0x101F4714));
    call<void>(0x028EAB2C, at<void>(0x1048CE0D));
    call<void>(0x028F026C, at<void>(0x101F4720));
}
VERIFY(0x025E2114, m_Do_DVDError_sinit_hd);
