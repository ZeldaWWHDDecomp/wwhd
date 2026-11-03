// Per-TU copies of the sead text classes' virtuals (see RenderStorageCopies.h for the vtables they sit in):
// deleting destructors that release only when asked to, empty termination hooks for plain texts, and the
// buffered texts' hook that terminates the buffer at its last character.
#include "agl/RenderStorageCopies.h"
#include "sead/HeapRegistry.h"
using namespace agl::render;

/* 027D58B4 */
void agl::render::pool_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D58E4 */
void agl::render::pool_text::TextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D58F8 */
void agl::render::pool_text::FixedTextA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D590C */
void agl::render::pool_text::FixedTextB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D5920 */
void agl::render::pool_text::FixedTextC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D770C */
void agl::render::frame_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D773C */
void agl::render::frame_text::TextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D7750 */
void agl::render::frame_text::FixedTextA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D7764 */
void agl::render::frame_text::FixedTextB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D7778 */
void agl::render::frame_text::SurfaceView::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D78AC */
void agl::render::frame_text::FixedTextC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D78C0 */
void agl::render::frame_text::FixedTextD::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D78D4 */
void agl::render::frame_text::FixedTextE::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D78E8 */
void agl::render::frame_text::FixedTextF::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* 027D4C9C */
void agl::render::shader_input_text::Text::assureTerminationImpl() {}
/* 027D4EA0 */
void agl::render::dispatch_text::Text::assureTerminationImpl() {}
/* 027D58C8 */
void agl::render::pool_text::Text::assureTerminationImpl() {}
/* 027D7720 */
void agl::render::frame_text::Text::assureTerminationImpl() {}
/* 027D58CC */
void agl::render::pool_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(s32)mCapacity - 1] = 0;
}
/* 027D7724 */
void agl::render::frame_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(s32)mCapacity - 1] = 0;
}
/* WWHD glue
*/
