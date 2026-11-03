/* Type checks, core-target reset and forwarding wrappers from assigned WWHD code. */
#include "agl/RenderTarget.h"
using namespace agl;
struct CoreTargetSlots_l {
    be<u32> count;
    gptr<gptr<RenderTarget>> data;
};
struct TypeInitialization_l {
    be<u32> ready;
};
static CoreTargetSlots_l sCoreTargetSlots;
static TypeInitialization_l sRenderTypeReady;
static TypeInitialization_l sBaseTypeReady;
static TypeInitialization_l sRootTypeReady;
static RenderTargetTypeNode_l sRenderType;
static RenderTargetTypeNode_l sBaseType;
static RenderTargetTypeNode_l sRootType;
static u8 sRenderTypeDefinition;
static u8 sBaseTypeDefinition;
static u8 sRootTypeDefinition;
static void releaseRenderTarget(void*);
static void clearRenderTarget(RenderTarget*, u32, u32, const RenderClearColor_l*, u32, f64);
static void bindRenderTarget(RenderTarget*, u32);
/* 027B9E68 */
void RenderTarget::resetCoreTargets() {
    sCoreTargetSlots.count = 0;
    sCoreTargetSlots.data = nullptr;
}
/* 027B9E7C */
void RenderTarget::destroy(RenderTarget* target, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (target && (flags & 1)) releaseRenderTarget(target);
}
/* 027B9E90 */
bool RenderTarget::isType(const RenderTargetTypeNode_l* requested) const {
    if (!static_cast<u32>(sRenderTypeReady.ready)) {
        sRenderTypeReady.ready = 1;
        sRenderType.definition = &sRenderTypeDefinition;
    }
    if (requested == &sRenderType) return true;
    if (!static_cast<u32>(sBaseTypeReady.ready)) {
        sBaseTypeReady.ready = 1;
        sBaseType.definition = &sBaseTypeDefinition;
    }
    if (requested == &sBaseType) return true;
    if (!static_cast<u32>(sRootTypeReady.ready)) {
        sRootTypeReady.ready = 1;
        sRootType.definition = &sRootTypeDefinition;
    }
    return requested == &sRootType;
}
/* 027B9F4C */
void RenderTarget::clearFirst(u32 flags, const RenderClearColor_l* color, u32 stencil, f64 depth) {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRenderTarget(this, 0, flags, color, stencil, depth);
}
/* 027B9F60 */
void RenderTarget::bindFirst() {
    gabi::AutoFrame frame{gabi::current_function()};
    bindRenderTarget(this, 0);
}
/* WWHD glue
data sCoreTargetSlots 104A2090
data sRenderTypeReady 101FD7BC
data sBaseTypeReady 101FD728
data sRootTypeReady 101FD724
data sRenderType 101FDCF8
data sBaseType 101FDCF4
data sRootType 101FDCF0
data sRenderTypeDefinition 10169BA8
data sBaseTypeDefinition 10169B98
data sRootTypeDefinition 10169B68
ext 0273AF40 void releaseRenderTarget(void*)
ext 027B997C void clearRenderTarget(RenderTarget*,u32,u32,const RenderClearColor_l*,u32,f64)
ext 027B9D00 void bindRenderTarget(RenderTarget*,u32)
*/
