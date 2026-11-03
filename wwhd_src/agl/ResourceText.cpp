/* Compiler-kept text virtuals for resource layouts and render buffers.
 * Layouts and instruction order come from the assigned WWHD functions. */
// References: WWHD's own instructions and strings only.
#include "agl/ResourceText.h"

static void releaseObject(void* object);
static void releaseTextStorage(void* storage);
static sead::SafeTextVtable sOwnedTextVtable;

/* 027B5B6C */
void agl::resource_layout_text::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B5B80 */
void agl::resource_layout_text::Text::assureTerminationImpl() {}
/* 027B5B84 */
void agl::resource_layout_text::FixedTextA::assureTerminationImpl() {
    auto text = mText.get();
    u32 capacity = mCapacity;
    *(gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)) = 0;
}
/* 027B5B9C */
void agl::resource_layout_text::FixedTextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B5BB0 */
void agl::resource_layout_text::FixedTextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B5BC4 */
void agl::resource_layout_text::FixedTextC::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B9364 */
void agl::render_buffer_text::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B9378 */
void agl::render_buffer_text::Text::assureTerminationImpl() {}
/* 027B937C */
void agl::render_buffer_text::FixedTextA::assureTerminationImpl() {
    auto text = mText.get();
    u32 capacity = mCapacity;
    *(gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)) = 0;
}
/* 027B9394 */
void agl::render_buffer_text::FixedTextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B93A8 */
void agl::render_buffer_text::OwnedText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    auto storage = mText.get();
    mVtable = &sOwnedTextVtable;
    if (storage != nullptr)
        releaseTextStorage(storage);
    if (flags & 1)
        releaseObject(this);
}
/* 027B940C */
void agl::render_buffer_text::FixedTextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B9420 */
void agl::render_buffer_text::FixedTextC::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B9434 */
void agl::render_buffer_text::FixedTextD::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B9448 */
void agl::render_buffer_text::FixedTextE::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B945C */
void agl::render_buffer_text::FixedTextF::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* WWHD glue
ext 0273AF40 void releaseObject(void* object) callback
ext 0273AFC8 void releaseTextStorage(void* storage)
data sOwnedTextVtable 10169A10
*/
