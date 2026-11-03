/* Local render-target prefixes and type-node references from assigned accesses. */
#pragma once
#include "wwhd.h"
namespace agl {
struct RenderColorTarget_l;
struct RenderDepthTarget_l;
struct RenderClearColor_l {
    be<f32> red, green, blue, alpha;
};
struct RenderTargetTypeNode_l {
    gptr<const void> definition;
};
struct RenderTarget {
    u8 prefix[0x1C];
    gptr<RenderColorTarget_l> colors[8];
    gptr<RenderDepthTarget_l> depth;
    static void resetCoreTargets();
    static void destroy(RenderTarget*, u32 flags);
    bool isType(const RenderTargetTypeNode_l*) const;
    void clearFirst(u32 flags, const RenderClearColor_l*, u32 stencil, f64 depth);
    void bindFirst();
};
WWHD_SIZE(RenderClearColor_l, 16);
WWHD_SIZE(RenderTargetTypeNode_l, 4);
WWHD_OFFSET(RenderTargetTypeNode_l, definition, 0);
WWHD_SIZE(RenderTarget, 0x40);
WWHD_OFFSET(RenderTarget, colors, 0x1C);
WWHD_OFFSET(RenderTarget, depth, 0x3C);
}
