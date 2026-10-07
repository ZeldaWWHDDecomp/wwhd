/* Scoped HD keyboard cursor geometry, WWHD binary reconstruction. */
#include "gabi.h"
using namespace gabi;
namespace hd_screen_keyboard_text_area {
void cursorPosition(u32 self, u32 output) {
    WWHD_FUNC(0x0267341C, void, self, output);
    FrameLocal<u8[12]> position(8);
    call<void>(0x02704D2C, position.get(), load<u32>(self + 0x58));
    const f32 x = load<f32>(position.a);
    const f32 y = load<f32>(position.a + 4);
    store<f32>(output, x);
    const f32 z = load<f32>(position.a + 8);
    store<f32>(output + 4, y);
    store<f32>(output + 8, z);
    const f32 half = load<f32>(0x100F6A20);
    const f32 width = call<f32>(0x02704A08, load<u32>(self + 0x58));
    store<f32>(output, fmadds(width, half, load<f32>(output)));
    const u32 pane = load<u32>(self + 0x58);
    const f32 inset = load<f32>(0x100F6A24);
    const f32 offset = fmsubs(load<f32>(pane + 0x40), half, inset);
    store<f32>(output + 4, fsubs_ppc(load<f32>(output + 4), offset));
}
VERIFY(0x0267341C, cursorPosition);
}
