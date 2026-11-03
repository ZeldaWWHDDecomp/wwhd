/* Address-only views for observed empty and constant-return virtual bodies.
 * Family membership follows image tables. Virtual roles and unused argument
 * lists remain unspecified; the names describe only proven body behavior. */
#pragma once
#include "wwhd.h"
namespace agl { namespace utility {
struct ParameterDefaultBody_l { void emptyHook(); };
struct MatrixModeDefaultBodies_l { void emptyHook(); u32 constantOne(); };
struct ShadowDefaultBodies_l { void emptyHookA(); u32 constantOne(); void emptyHookB(); };
} }
