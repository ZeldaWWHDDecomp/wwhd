/* Cached conversion callback selection and per-word stores from assigned WWHD code. */
#include "agl/ShaderEndianConversions.h"
using namespace agl;
struct ShaderByteOrder_l {
    be<u32> value;
};
struct ShaderConversionTable_l {
    gfn operations[2];
};
static ShaderByteOrder_l sShaderByteOrder;
static u32 reverseShaderWord(u32);
static u32 identityShaderWord(u32);
static ShaderConversionTable_l sShaderConversions;
/* 027BF0CC */
void ShaderEndianConversions::convert(u32 order, void* buffer, u32 bytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 words = bytes >> 2;
    if (!words) return;
    u32 selector = order ^ static_cast<u32>(sShaderByteOrder.value);
    u32 operation = sShaderConversions.operations[selector].get();
    u32 cursor = gabi::ea(buffer);
    for (u32 remaining = words; remaining; --remaining, cursor += 4) {
        u32 word = *gabi::at<const be<u32>>(cursor);
        u32 result = gabi::call_ptr<u32>(operation, word);
        *gabi::at<be<u32>>(cursor) = result;
    }
}
/* WWHD glue
ext 02759A48 u32 reverseShaderWord(u32)
ext 02759B38 u32 identityShaderWord(u32)
data sShaderByteOrder 104A0CD4
data sShaderConversions 10147620
*/
