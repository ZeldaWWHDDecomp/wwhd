// Shader program stage (vertex/pixel/geometry slot of agl::ShaderProgram, constructed by 027D4CCC from the program
// constructor 027B9F78) and the shader TU's sead text helpers.
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"

namespace agl { namespace render {
struct ShaderInputLayout;
struct StageMethodTable {
    u8 mHeader[16];
    sead::VtableSlot mGetKind;               // 0 vertex, 1 pixel, 2 geometry (derived stage tables)
};
struct ShaderStageState {
    gptr<void> mCode;
    gptr<ShaderInputLayout> mVariations;     // the variation layout the shader archive loader assigns
    be<u16> mStateWords[2];
    be<u8> mStateBytes[4];
    gptr<StageMethodTable> mMethods;
    void setCode(void*);
    u32 refreshText(u32 enabled);
};
WWHD_SIZE(ShaderStageState,20);
WWHD_OFFSET(ShaderStageState,mVariations,4);
WWHD_OFFSET(ShaderStageState,mStateWords,8);
WWHD_OFFSET(ShaderStageState,mStateBytes,12);
WWHD_OFFSET(ShaderStageState,mMethods,16);
WWHD_OFFSET(StageMethodTable,mGetKind,16);

void destroyShaderText(sead::SafeText*,u32);
void terminateShaderTextBuffer(sead::SafeTextBuffer*);
void destroyShaderTextBuffer(sead::SafeTextBuffer*,u32);
ShaderStageState* createShaderStage(ShaderStageState*);
} }

void* allocateRenderStorage(u32);
void releaseRenderStorage(void*);

void generateShaderStageText(agl::render::ShaderInputLayout*,u32,sead::SafeTextBuffer*,u32,u32);
