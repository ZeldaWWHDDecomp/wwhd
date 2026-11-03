/* Copy-construction helpers for layout materials. */
#pragma once
#include "nw/lyt/MaterialGpuStorage.h"
namespace nw { namespace lyt {
using MaterialCopyConstructor = ShaderLocation*(*)(ShaderLocation*);
} }
void* materialCopyAllocate(u32 size);
void materialCopyConstructArray(void* storage,u32 count,u32 stride,nw::lyt::MaterialCopyConstructor constructor);
void materialCopyMemory(void* output,const void* input,u32 size);
