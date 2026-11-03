// Renderer inline specializations use the canonical sead text layout.
#pragma once
#include "sead/FileDevice.h"
#include "sead/RuntimeType.h"
// Renderer input/output runtime types, owned by sead/RenderInlineInstances.cpp (guards 101FD6A8/101FD6AC, nodes
// 101FDF84/101FDF88); shared with units that initialise them inline (exported 2026-10-24).
extern be<u32>& sInputGuard;
extern sead::RuntimeType& sInputType;
extern be<u32>& sOutputGuard;
extern sead::RuntimeType& sOutputType;
