// Shader variation input layouts and the render pointer arrays.
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"
#include "sead/Foundation.h"
namespace sead { struct RegistryHeap; }
namespace agl { namespace render {
// sead::PointerBuffer with this unit's own per-TU copies of its storage, erase and heap-sort methods
// (027D78FC..027D7A4C; same bodies as the merged 0273B560/0273B584/0273B5DC/0273B624/0273B7B4).
struct RenderPointerArray : sead::PointerBuffer {
    be<u32>* entries() const { return static_cast<be<u32>*>(mStorage.get()); }
    void adoptStorage(s32,be<u32>*);
    void allocateStorage(s32,void*,s32);
    void releaseStorage();
    void eraseRange(s32,s32);
    void sort(u32);
};
// Shader variation input layout (0x44; the 0x48 archive layout of 027B9470 extends it): the stage's source name text,
// the compiled source and output texts the shader archive loader assigns (027B814C), the archive's macro names/values
// (027B7D78) and the variant selection (027BB710).
struct ShaderInputLayout {
    sead::SafeText mName;                          // "unititled" until the loader names it
    gptr<sead::SafeText> mCompiledSource;
    gptr<sead::SafeTextBuffer> mOutputText;
    RenderPointerArray mNames;
    RenderPointerArray mValues;
    RenderPointerArray mSelectedNames;
    RenderPointerArray mSelectedValues;
    gptr<void> mMethods;
    void releaseArrays();
    void reserve(s32 names,s32 selections,sead::RegistryHeap* heap);
    void resetSelection();
    void select(const char* name,const char* value);
};
WWHD_SIZE(RenderPointerArray,12);
WWHD_SIZE(ShaderInputLayout,0x44);
WWHD_OFFSET(ShaderInputLayout,mCompiledSource,8);
WWHD_OFFSET(ShaderInputLayout,mOutputText,12);
WWHD_OFFSET(ShaderInputLayout,mNames,0x10);
WWHD_OFFSET(ShaderInputLayout,mValues,0x1C);
WWHD_OFFSET(ShaderInputLayout,mSelectedNames,0x28);
WWHD_OFFSET(ShaderInputLayout,mSelectedValues,0x34);
WWHD_OFFSET(ShaderInputLayout,mMethods,0x40);
ShaderInputLayout* createShaderInputLayout(ShaderInputLayout*);
void destroyShaderInputLayout(ShaderInputLayout*,u32);
} }

void* allocateRenderPointerStorage(u32,void*,s32);
void releaseRenderPointerStorage(void*);
void moveRenderPointerStorage(void*,const void*,u32,u32);
// sead::PointerBuffer::allocate / release (merged 0273B584 / 0273B5DC), called by address from these TUs.
void releaseInputArray(agl::render::RenderPointerArray*);
void allocateInputArray(agl::render::RenderPointerArray*,s32,void*,s32);
