// Per-translation-unit copies of the sead text classes' virtuals (deleting destructors and termination hooks).
// Each namespace is one TU's set of vtables (.rodata, identified by the tables these bodies sit in):
//   shader_input_text: Text 1016B360 (destructor 027D4C88 in ShaderStageState.cpp);
//   dispatch_text:     Text 1016BFFC (destructor 027D4E8C in RenderDispatchContext.cpp);
//   pool_text:         Text 1016C0C4, TextBuffer 1016C0DC, fixed buffers 1016C114 / 1016C12C / 1016C15C, and the
//                      resource list's table 1016C144 (RenderResourceList::destroy);
//   frame_text:        Text 1016C180, TextBuffer 1016C198, fixed buffers 1016C1E0 / 1016C1F8 / 1016C230 / 1016C248 /
//                      1016C260 / 1016C278, and the frame's surface view table 1016C210 (third slot).
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"
#include "sead/Viewport.h"
namespace agl { namespace render {
namespace shader_input_text {
struct Text : sead::SafeText { void assureTerminationImpl(); };
}
namespace dispatch_text {
struct Text : sead::SafeText { void assureTerminationImpl(); };
}
namespace pool_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
}
namespace frame_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextD : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextE : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextF : sead::SafeTextBuffer { void destroy(u32 flags); };
// The display frame's surface object (frame +0x744, vtable stored at +0x18 by 027D5948).
struct SurfaceView : sead::RenderSurface { gptr<void> mVtable; void destroy(u32 flags); };
}
} }
WWHD_SIZE(agl::render::frame_text::SurfaceView,0x1C);
