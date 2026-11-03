// Assigned text callback prefixes; original fixed-buffer extents are not inferred from unused tables.
#pragma once
#include "sead/FileDevice.h"
namespace agl { namespace cascade_shadow_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
// These are prefix views; fixed capacities/full subclass sizes are unproved.
struct TextBuffer : sead::SafeTextBuffer { void assureTerminationImpl(); };
struct TextBufferA : TextBuffer { void destroy(u32 flags); };
struct TextBufferB : TextBuffer { void destroy(u32 flags); };
struct TextBufferC : TextBuffer { void destroy(u32 flags); };
struct TextBufferD : TextBuffer { void destroy(u32 flags); };
}}
namespace agl { namespace shadow_unit_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
// These are prefix views; fixed capacities/full subclass sizes are unproved.
struct TextBuffer : sead::SafeTextBuffer { void assureTerminationImpl(); };
struct TextBufferA : TextBuffer { void destroy(u32 flags); };
struct TextBufferB : TextBuffer { void destroy(u32 flags); };
struct TextBufferC : TextBuffer { void destroy(u32 flags); };
}}
namespace agl { namespace parameter_root_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
// These are prefix views; fixed capacities/full subclass sizes are unproved.
struct TextBuffer : sead::SafeTextBuffer { void assureTerminationImpl(); };
struct TextBufferA : TextBuffer { void destroy(u32 flags); };
struct TextBufferB : TextBuffer { void destroy(u32 flags); };
struct TextBufferC : TextBuffer { void destroy(u32 flags); };
struct TextBufferD : TextBuffer { void destroy(u32 flags); };
struct TextBufferE : TextBuffer { void destroy(u32 flags); };
struct TextBufferF : TextBuffer { void destroy(u32 flags); };
struct TextBufferG : TextBuffer { void destroy(u32 flags); };
struct TextBufferH : TextBuffer { void destroy(u32 flags); };
struct TextBufferI : TextBuffer { void destroy(u32 flags); };
}}
namespace agl { namespace screenshot_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
// These are prefix views; fixed capacities/full subclass sizes are unproved.
struct TextBuffer : sead::SafeTextBuffer { void assureTerminationImpl(); };
struct TextBufferA : TextBuffer { void destroy(u32 flags); };
struct TextBufferB : TextBuffer { void destroy(u32 flags); };
struct TextBufferC : TextBuffer { void destroy(u32 flags); };
struct TextBufferD : TextBuffer { void destroy(u32 flags); };
struct TextBufferE : TextBuffer { void destroy(u32 flags); };
struct TextBufferF : TextBuffer { void destroy(u32 flags); };
struct TextBufferG : TextBuffer { void destroy(u32 flags); };
struct TextBufferH : TextBuffer { void destroy(u32 flags); };
struct TextBufferI : TextBuffer { void destroy(u32 flags); };
struct TextBufferJ : TextBuffer { void destroy(u32 flags); };
struct TextBufferK : TextBuffer { void destroy(u32 flags); };
struct TextBufferL : TextBuffer { void destroy(u32 flags); };
struct TextBufferM : TextBuffer { void destroy(u32 flags); };
struct TextBufferN : TextBuffer { void destroy(u32 flags); };
struct TextBufferO : TextBuffer { void destroy(u32 flags); };
}}
namespace agl { namespace parameter_tree_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
// These are prefix views; fixed capacities/full subclass sizes are unproved.
struct TextBuffer : sead::SafeTextBuffer { void assureTerminationImpl(); };
struct TextBufferA : TextBuffer { void destroy(u32 flags); };
struct TextBufferB : TextBuffer { void destroy(u32 flags); };
struct TextBufferC : TextBuffer { void destroy(u32 flags); };
struct TextBufferD : TextBuffer { void destroy(u32 flags); };
struct TextBufferE : TextBuffer { void destroy(u32 flags); };
struct TextBufferF : TextBuffer { void destroy(u32 flags); };
struct TextBufferG : TextBuffer { void destroy(u32 flags); };
struct TextBufferH : TextBuffer { void destroy(u32 flags); };
}}
namespace agl { namespace parameter_blend_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
// These are prefix views; fixed capacities/full subclass sizes are unproved.
struct TextBuffer : sead::SafeTextBuffer { void assureTerminationImpl(); };
struct TextBufferA : TextBuffer { void destroy(u32 flags); };
struct TextBufferB : TextBuffer { void destroy(u32 flags); };
struct TextBufferC : TextBuffer { void destroy(u32 flags); };
struct TextBufferD : TextBuffer { void destroy(u32 flags); };
struct TextBufferE : TextBuffer { void destroy(u32 flags); };
struct TextBufferF : TextBuffer { void destroy(u32 flags); };
}}
