/* The parameter construction GHS inlined into the SSAO and shadow-map constructors: a member parameter is
 * allocated when its address is NULL, created (ParameterBase::create), given its per-unit vtable, and named
 * (ParameterBase::initialize) from three SafeString temporaries in the constructor's frame. */
#pragma once
#include "agl/ScreenEffectParameters.h"
#include "agl/ParameterTree.h"
void* allocateEffectParameter(u32 size);
namespace agl { namespace effect_construction {
// Frame offsets of the name, label and meta-text temporaries of one parameter.
struct TextSlots { u32 mName, mLabel, mMeta; };
ParameterBase* constructParameter(ParameterBase* parameter, u32 size, u32 methods, sead::SafeTextVtable* textMethods,
                                  const char* name, const char* label, be<u8>* meta, parameter_tree::Value* owner,
                                  TextSlots slots, bool metaFirst = false);
} }
