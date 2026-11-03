// Shader program stage construction and the shader TU's sead text virtuals.
#include "agl/ShaderStageState.h"
using namespace agl::render;

static StageMethodTable sStageMethodTable;

/* 027D4C88 */
void agl::render::destroyShaderText(sead::SafeText* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1u)) releaseRenderStorage(text);
}
/* 027D4CA0 */
void agl::render::terminateShaderTextBuffer(sead::SafeTextBuffer* text) {
    text->mText.get()[(s32)text->mCapacity - 1] = 0;
}
/* 027D4CB8 */
void agl::render::destroyShaderTextBuffer(sead::SafeTextBuffer* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1u)) releaseRenderStorage(text);
}
/* 027D4CCC */
agl::render::ShaderStageState* agl::render::createShaderStage(ShaderStageState* stage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!stage) stage = static_cast<ShaderStageState*>(allocateRenderStorage(sizeof(ShaderStageState)));
    if (!stage) return nullptr;
    stage->mStateBytes[2] = 0;
    stage->mMethods = &sStageMethodTable;
    stage->mStateBytes[3] = 0;
    stage->mCode = nullptr;
    stage->mStateBytes[1] = 0;
    stage->mVariations = nullptr;
    stage->mStateBytes[0] = 0;
    stage->mStateWords[0] = 0;
    stage->mStateWords[1] = 0;
    return stage;
}
/* 027D4E84 */
void agl::render::ShaderStageState::setCode(void* code) {
    mCode = code;
}

/* WWHD glue
data sStageMethodTable 1016C014
ext 0273AD10 void* allocateRenderStorage(u32)
ext 0273AF40 void releaseRenderStorage(void*)
*/
