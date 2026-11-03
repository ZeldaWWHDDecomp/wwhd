// Compiler-emitted inline methods and extrapolation functions of JStudio functionvalue.
#include "JSystem/JStudio/functionvalue_inline.h"
using namespace JStudio;
using namespace JStudio::functionvalue;
f64 FunctionValue_fmod(f64,f64);
void FunctionValue_register(void*);
/* 0284A7DC */
void TFunctionValue_constant::prepare() {}
namespace JStudio { namespace functionvalue {
/* 0284A7E0 */
f64 extrapolateParameter_raw(f64 value,f64) { return value; }
/* 0284A7E4 */
f64 extrapolateParameter_repeat(f64 value,f64 maximum) {
    f64 result=FunctionValue_fmod(value,maximum);
    if (result<0.0) result+=maximum;
    return result;
}
/* 0284A834 */
f64 extrapolateParameter_clamp(f64 value,f64 maximum) {
    if (!(value>0.0)) return 0.0;
    return maximum-value>=0.0?value:maximum;
}
}}
namespace {
struct functionvalue_inline_StaticBlock { be<u32> words[4]; };
static functionvalue_inline_StaticBlock sfunctionvalue_inlineStaticBlock;
static u32 sfunctionvalue_inlineStaticRegister;
}

/* 0284A8BC */
static void __sinit_functionvalue_inline_cpp() {
    sfunctionvalue_inlineStaticBlock.words[2]=0; sfunctionvalue_inlineStaticBlock.words[0]=0; sfunctionvalue_inlineStaticBlock.words[3]=0; sfunctionvalue_inlineStaticBlock.words[1]=0;
    FunctionValue_register(&sfunctionvalue_inlineStaticRegister);
}
/* WWHD glue:
data sfunctionvalue_inlineStaticBlock 104B66F4
data sfunctionvalue_inlineStaticRegister 101FC784
*/
