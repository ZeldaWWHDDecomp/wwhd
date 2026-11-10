#include "JSystem/JStudio/jstudio-object.h"
// Fog adaptor compiler-emitted value and output helpers.
#include "JSystem/JStudio/JStudio_JStage/object-fog_inline.h"
using namespace JStudio_JStage;
void* JStudioFogInline_allocate(u32);
void JStudioFogInline_delete(void*);
void JStudioFogInline_clear(void*,u32);
void JStudioLight_destroyArray(void*,u32,u32,u32,u32,u32);
void JStudioLight_destroyValue(jst_timeline::TVariableValue*,u32);
void JStudioFogInline_destroyOutput(FogValueOutput*,u32);
namespace {
}
/* 028417F0 */
static void destroyFogOutputs() {
    JStudioLight_destroyArray(&sJStudioFogOutputs,3,24,gabi::guest_fn(reinterpret_cast<const void*>(&JStudioFogInline_destroyOutput)),0,0);
}
/* 02841814 */
jst_timeline::TVariableValue* JStudioFogInline_constructVariableValue(jst_timeline::TVariableValue* value) {
    if (!value) value=static_cast<jst_timeline::TVariableValue*>(JStudioFogInline_allocate(sizeof(*value)));
    if (value) { value->mAge=0; value->mUpdate.v=0; JStudioFogInline_clear(&value->mParam,4); value->pOutput_=&sJStudioOutputNone; }
    return value;
}
/* 02841878 */
void FogValueOutput::output(f32 value,jst_adaptors::TAdaptor* adaptor) const {
    auto* fog=static_cast<jst_adaptors::TAdaptor_fog*>(adaptor);
    auto* object=fog->mObject.get();
    (object->*mSetter)(value);
}
/* 028418BC */
void JStudioFogInline_destroyOutput(FogValueOutput* output,u32 flags) {
    if (!output) return;
    JStudioLight_destroyValue(reinterpret_cast<jst_timeline::TVariableValue*>(output),0);
    if (flags&1) JStudioFogInline_delete(output);
}
/* WWHD glue:
ext 0273AD10 void* JStudioFogInline_allocate(u32)
ext 0273AF40 void JStudioFogInline_delete(void*)
ext 028F521C void JStudioFogInline_clear(void*,u32)
*/
