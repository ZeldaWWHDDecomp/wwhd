// Ambient light adaptor lifecycle and initial color, from the tww reference and WWHD.
#include "JSystem/JStudio/JStudio_JStage/object-ambientlight.h"
#include "JSystem/JStudio/jstudio-object.h"
using JStudio_JStage::AmbientVariablePrefix;
using JStudio_JStage::AmbientVTable;
using JStudio_JStage::StageSystem;
void* JStudioAmbient_allocate(u32);
void JStudioAmbient_delete(void*);
void JStudioAmbient_constructArray(void*, u32, u32, u32);
void JStudioAmbient_clear(void*, u32);
void JStudioAmbient_destroyBase(jst_adaptors::TAdaptor*, u32);
void JStudioAmbient_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32);
void JStudioAmbient_setColor(jst_adaptors::TAdaptor*, const jst_adaptors::Index4*, const jst_adaptors::Color*);
void JStudioAmbient_registerGlobal(void*);
static jst_timeline::TVariableValue* constructAmbientVariableValue(jst_timeline::TVariableValue*);
namespace {
static u32 sBaseVTable;
static u32 sAmbientVTable;
static const jst_adaptors::Index4 sColorIndices;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static AmbientVariablePrefix* prefix(JStudio_JStage::TAdaptor_ambientLight* adaptor) {
    return reinterpret_cast<AmbientVariablePrefix*>(adaptor->pad04);
}
static AmbientVTable* table(jst_adaptors::StageObject* object) {
    return reinterpret_cast<AmbientVTable*>(object->mVtable.get());
}
}
/* 028408C8 */
JStudio_JStage::TAdaptor_ambientLight* TAdaptor_ambientLight_construct(JStudio_JStage::TAdaptor_ambientLight* adaptor, const StageSystem* system, jst_adaptors::StageObject* object) {
    if (!adaptor) adaptor = static_cast<JStudio_JStage::TAdaptor_ambientLight*>(JStudioAmbient_allocate(sizeof(JStudio_JStage::TAdaptor_ambientLight)));
    if (!adaptor) return adaptor;
    auto* state = prefix(adaptor);
    state->mCount = 4;
    state->__vtbl = &sBaseVTable;
    adaptor->mVariableValues = reinterpret_cast<jst_adaptors::TVariableValues*>(state->mValues);
    JStudioAmbient_constructArray(state->mValues, 4, 20, gabi::guest_fn(reinterpret_cast<const void*>(&constructAmbientVariableValue)));
    adaptor->mLight = object;
    state->mSystem = system;
    state->__vtbl = &sAmbientVTable;
    return adaptor;
}
/* 02840960 */
void JStudio_JStage::TAdaptor_ambientLight::adaptor_do_end(const jst_adaptors::TObject* object) {
    auto* light = mLight.get();
    auto* methods = table(light);
    u32 get = methods->getFlags.get();
    u32 set = methods->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, light);
    gabi::call_ptr<void>(set, light, flags & ~1u);
}
/* 028409B8 */
void TAdaptor_ambientLight_destroy(JStudio_JStage::TAdaptor_ambientLight* adaptor, u32 flags) {
    if (!adaptor) return;
    prefix(adaptor)->__vtbl = &sAmbientVTable;
    adaptor->adaptor_do_end(nullptr);
    JStudioAmbient_destroyBase(adaptor, 0);
    if (flags & 1) JStudioAmbient_delete(adaptor);
}
/* 02840A24 */
void JStudio_JStage::TAdaptor_ambientLight::adaptor_do_begin(const jst_adaptors::TObject* object) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    auto* light = mLight.get();
    auto* methods = table(light);
    u32 get = methods->getFlags.get();
    u32 set = methods->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, light);
    gabi::call_ptr<void>(set, light, flags | 1u);
    light = mLight;
    u32 packed = gabi::call_ptr<u32>(table(light)->getColor.get(), light);
    gabi::FrameLocal<jst_adaptors::Color> color(8);
    color->word = packed;
    JStudioAmbient_setColor(this, &sColorIndices, color.get());
}
/* 02840B18 */
void JStudio_JStage::TAdaptor_ambientLight::adaptor_do_data(const jst_adaptors::TObject* object, const void* id, u32 idSize, const void* data, u32 size) {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioAmbient_forwardData)), mLight.get(), id, idSize, data, size);
}
/* 02840B30 */
static void initializeAmbientAdaptor() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioAmbient_registerGlobal)), &sStaticRegister);
}
/* 02840B58 */
static jst_timeline::TVariableValue* constructAmbientVariableValue(jst_timeline::TVariableValue* value) {
    if (!value) value = static_cast<jst_timeline::TVariableValue*>(JStudioAmbient_allocate(sizeof(jst_timeline::TVariableValue)));
    if (value) {
        value->field_0x4 = 0;
        value->field_0x8.v = 0;
        JStudioAmbient_clear(&value->field_0xc, 4);
        value->pOutput_ = &sJStudioOutputNone;
    }
    return value;
}
/* 02840BBC */
void JStudio_JStage::TAdaptor_ambientLight::adaptor_do_prepare(const jst_adaptors::TObject* object) {}
/* WWHD glue
data sBaseVTable 10173764
data sAmbientVTable 101737A4
data sColorIndices 10172E68
data sStaticBlock 104B62DC
data sStaticRegister 101FAE7C
ext 0273AD10 void* JStudioAmbient_allocate(u32)
ext 0273AF40 void JStudioAmbient_delete(void*)
ext 028EFFD0 void JStudioAmbient_constructArray(void*, u32, u32, u32)
ext 028F521C void JStudioAmbient_clear(void*, u32)
ext 0283AC58 void JStudioAmbient_destroyBase(jst_adaptors::TAdaptor*, u32)
ext 0284219C void JStudioAmbient_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32) callback
ext 0283A220 void JStudioAmbient_setColor(jst_adaptors::TAdaptor*, const jst_adaptors::Index4*, const jst_adaptors::Color*)
ext 028F026C void JStudioAmbient_registerGlobal(void*) callback
*/
