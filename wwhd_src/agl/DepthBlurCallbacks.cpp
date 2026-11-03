// Local depth-blur archive callbacks and texture adapters, from this game's binary.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
#include "agl/RenderBuffer.h"
#include "agl/DepthOfFieldDraw.h"
// The parameter host (agl::dof::ContextHost) whose first view parameters these callbacks refresh.
using agl::dof::ContextHost;
static void notifyArchiveChange(ContextHost* archive, u32 reason);
static void prepareDepthBlurSurface(void* blur, u32 view, u32 format, s32 width, s32 height);

/* 0277D244 */
static void applyDepthBlurParameterChanges(ContextHost* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    archive->mSettings.updateShaderPrograms();
    archive->mSettings.updateIndirectTransform();
    notifyArchiveChange(archive, 0);
}

/* 0277D288 */
static void applyDepthBlurDelegateChanges(void* delegate) {
    gabi::AutoFrame frame{gabi::current_function()};
    // The delegate is the depth-of-field object's parameter root.
    applyDepthBlurParameterChanges(gabi::at<ContextHost>(gabi::ea(delegate) - offsetof(agl::dof::DepthOfField, mParameterRoot)));
}

/* 0277FBA8 */
static void prepareDepthBlurTexture(void* blur, u32 view, const agl::RenderBuffer* buffer) {
    gabi::AutoFrame frame{gabi::current_function()};
    const agl::ColorSurface* color = buffer->mColors[0];
    s32 width = color->mBase.mMinimumWidth;
    const s32 requestedWidth = color->mBase.mSurface.mWidth;
    const s32 requestedHeight = color->mBase.mSurface.mHeight;
    s32 height = color->mBase.mMinimumHeight;
    if (requestedWidth > width) width = requestedWidth;
    u32 format = color->mBase.mFormat;
    if (requestedHeight > height) height = requestedHeight;
    prepareDepthBlurSurface(blur, view, format, width, height);
}


/* WWHD glue
 ext 0279A6B0 void notifyArchiveChange(ContextHost* archive, u32 reason)
 ext 0277F6BC void prepareDepthBlurSurface(void* blur, u32 view, u32 format, s32 width, s32 height)
*/
