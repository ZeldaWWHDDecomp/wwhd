/* WWHD per-TU text virtuals in resource layout and render-buffer code.
 * Fixed buffer capacities are unknown where these functions never install their tables.
 * Only the observed SafeText / SafeTextBuffer prefixes are declared. */
#pragma once
#include "sead/FileDevice.h"

namespace agl::resource_layout_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
}
namespace agl::render_buffer_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct OwnedText : sead::SafeText { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextD : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextE : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextF : sead::SafeTextBuffer { void destroy(u32 flags); };
}
WWHD_OFFSET(sead::SafeText, mText, 0);
WWHD_OFFSET(sead::SafeText, mVtable, 4);
WWHD_SIZE(sead::SafeText, 8);
WWHD_OFFSET(sead::SafeTextBuffer, mCapacity, 8);
WWHD_SIZE(sead::SafeTextBuffer, 12);
