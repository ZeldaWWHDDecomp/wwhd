// Text lifetime/termination entries reconstructed from assigned WWHD instructions.
#include "bindings.h"
#include "agl/LibraryTextEntries.h"
/* 02798F20 */
void agl::cascade_shadow_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02798F64 */
void agl::cascade_shadow_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02798F34 */
void agl::cascade_shadow_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02798F78 */
void agl::cascade_shadow_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 027990AC */
void agl::cascade_shadow_text::TextBufferD::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02798F48 */
void agl::cascade_shadow_text::Text::assureTerminationImpl() {}
/* 02798F4C */
void agl::cascade_shadow_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = (u32)mCapacity;
    gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)[0] = 0;
}
/* 02799F6C */
void agl::shadow_unit_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02799F9C */
void agl::shadow_unit_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02799FB0 */
void agl::shadow_unit_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A068 */
void agl::shadow_unit_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02799F80 */
void agl::shadow_unit_text::Text::assureTerminationImpl() {}
/* 02799F84 */
void agl::shadow_unit_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = (u32)mCapacity;
    gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)[0] = 0;
}
/* 0279A47C */
void agl::parameter_root_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A4AC */
void agl::parameter_root_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A4C0 */
void agl::parameter_root_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A4D4 */
void agl::parameter_root_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A4E8 */
void agl::parameter_root_text::TextBufferD::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A4FC */
void agl::parameter_root_text::TextBufferE::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A510 */
void agl::parameter_root_text::TextBufferF::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A524 */
void agl::parameter_root_text::TextBufferG::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A538 */
void agl::parameter_root_text::TextBufferH::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A54C */
void agl::parameter_root_text::TextBufferI::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279A490 */
void agl::parameter_root_text::Text::assureTerminationImpl() {}
/* 0279A494 */
void agl::parameter_root_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = (u32)mCapacity;
    gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)[0] = 0;
}
/* 0279B5E4 */
void agl::screenshot_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B628 */
void agl::screenshot_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B664 */
void agl::screenshot_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B678 */
void agl::screenshot_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B5F8 */
void agl::screenshot_text::TextBufferD::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B68C */
void agl::screenshot_text::TextBufferE::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B714 */
void agl::screenshot_text::TextBufferF::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B728 */
void agl::screenshot_text::TextBufferG::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B63C */
void agl::screenshot_text::TextBufferH::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B650 */
void agl::screenshot_text::TextBufferI::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B73C */
void agl::screenshot_text::TextBufferJ::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B750 */
void agl::screenshot_text::TextBufferK::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B764 */
void agl::screenshot_text::TextBufferL::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B778 */
void agl::screenshot_text::TextBufferM::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B78C */
void agl::screenshot_text::TextBufferN::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B7A0 */
void agl::screenshot_text::TextBufferO::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279B60C */
void agl::screenshot_text::Text::assureTerminationImpl() {}
/* 0279B610 */
void agl::screenshot_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = (u32)mCapacity;
    gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)[0] = 0;
}
/* 0279C73C */
void agl::parameter_tree_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C780 */
void agl::parameter_tree_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C794 */
void agl::parameter_tree_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C7A8 */
void agl::parameter_tree_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C750 */
void agl::parameter_tree_text::TextBufferD::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C7BC */
void agl::parameter_tree_text::TextBufferE::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C7D0 */
void agl::parameter_tree_text::TextBufferF::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C7E4 */
void agl::parameter_tree_text::TextBufferG::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C7F8 */
void agl::parameter_tree_text::TextBufferH::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279C764 */
void agl::parameter_tree_text::Text::assureTerminationImpl() {}
/* 0279C768 */
void agl::parameter_tree_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = (u32)mCapacity;
    gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)[0] = 0;
}
/* 0279D6E4 */
void agl::parameter_blend_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279D714 */
void agl::parameter_blend_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279D728 */
void agl::parameter_blend_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279D73C */
void agl::parameter_blend_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279D750 */
void agl::parameter_blend_text::TextBufferD::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279D764 */
void agl::parameter_blend_text::TextBufferE::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279D778 */
void agl::parameter_blend_text::TextBufferF::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 0279D6F8 */
void agl::parameter_blend_text::Text::assureTerminationImpl() {}
/* 0279D6FC */
void agl::parameter_blend_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = (u32)mCapacity;
    gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)[0] = 0;
}
/* WWHD glue
*/
