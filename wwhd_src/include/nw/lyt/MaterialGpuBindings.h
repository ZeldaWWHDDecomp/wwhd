/* Material texture and sampler binding helpers. */
#pragma once
#include "nw/lyt/MaterialGpuStorage.h"
#include "nw/lyt/TextureBinding.h"
void materialBindTexture(nw::lyt::TextureObject*,u32);
void materialBindSampler(nw::lyt::LayoutSampler*,u32);
void materialBindingUniforms(u32,u32,const void*);
