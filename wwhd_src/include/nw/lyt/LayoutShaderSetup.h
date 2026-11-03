/* Shader binding-name ranges and program/fetch setup used by the layout renderer. */
#pragma once
#include "nw/lyt/LayoutRenderStorage.h"
#include "nw/lyt/ResourceHelpers.h"
namespace nw { namespace lyt {
struct ShaderBindingName { be<u32> kind,slot; gptr<const be<u8>> name; };
struct ShaderBindingRange {
    gptr<ShaderBindingName> begin,end;
    static ShaderBindingRange* createForProgram(ShaderBindingRange* range,u32 program);
};
WWHD_SIZE(ShaderBindingName,12); WWHD_SIZE(ShaderBindingRange,8);
} }
void* shaderSetupAllocate(u32 size);
u32 shaderSetupFetchSize(u32 count,u32 type,u32 tessellation);
void shaderSetupInitializeFetch(nw::lyt::ShaderFetch*,void* program,u32 count,nw::lyt::ShaderAttribute* streams,u32 type,u32 tessellation);
void shaderSetupInvalidate(u32 mode,void* storage,u32 size);
