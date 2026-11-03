/* Assigned shader-archive word-order conversion entry point. */
#pragma once
#include "wwhd.h"
namespace agl {
struct ShaderEndianConversions {
    static void convert(u32 order, void* buffer, u32 bytes);
};
}
