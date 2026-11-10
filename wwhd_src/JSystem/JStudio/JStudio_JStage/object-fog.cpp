// Fog adaptor setup and initial parameter values, reconstructed from WWHD with the tww reference.
#include "JSystem/JStudio/JStudio_JStage/object-fog.h"
using JStudio_JStage::FogVariablePrefix;
using JStudio_JStage::FogValueOutput;
using JStudio_JStage::FogVTable;
using JStudio_JStage::StageSystem;
void* JStudioFog_allocate(u32);
void JStudioFog_delete(void*);
void JStudioFog_constructArray(void*, u32, u32, u32);
void JStudioFog_destroyBase(jst_adaptors::TAdaptor*, u32);
void JStudioFog_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32);
void JStudioFog_setColor(jst_adaptors::TAdaptor*, const jst_adaptors::Index4*, const jst_adaptors::Color*);
void JStudioFog_registerGlobal(void*);
jst_timeline::TVariableValue* JStudioFog_constructVariableValue(jst_timeline::TVariableValue*);
void JStudioFog_updateImmediate(jst_timeline::TVariableValue*, f64);
static JStudio_JStage::FogOutputs sJStudioFogOutputs;
namespace {
static u32 sBaseVTable;
static u32 sFogVTable;
static u32 sOutputVTable;
static const jst_adaptors::Index4 sColorIndices;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sOutputRegister;
static FogVariablePrefix* prefix(JStudio_JStage::TAdaptor_fog* adaptor) { return reinterpret_cast<FogVariablePrefix*>(adaptor->pad04); }
static FogVTable* table(jst_adaptors::StageObject* object) { return reinterpret_cast<FogVTable*>(object->mVtable.get()); }
static void initializeOutput(FogValueOutput* output, s32 index, s16 setter, s16 getter) {
    output->__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sOutputVTable);
    output->mValueIndex = index;
    output->mSetter.d = 0; output->mSetter.i = setter; output->mSetter.f = 0;
    output->mGetter.d = 0; output->mGetter.i = getter; output->mGetter.f = 0;
}
}
/* 028413B0 */
JStudio_JStage::TAdaptor_fog* TAdaptor_fog_construct(JStudio_JStage::TAdaptor_fog* adaptor, const StageSystem* system, jst_adaptors::StageObject* object) {
    if (!adaptor) adaptor = static_cast<JStudio_JStage::TAdaptor_fog*>(JStudioFog_allocate(sizeof(JStudio_JStage::TAdaptor_fog)));
    if (!adaptor) return adaptor;
    auto* state = prefix(adaptor);
    state->mCount = 6;
    state->__vtbl = &sBaseVTable;
    adaptor->mVariableValues = reinterpret_cast<jst_adaptors::TVariableValues*>(state->mValues);
    JStudioFog_constructArray(state->mValues, 6, 20, gabi::guest_fn(reinterpret_cast<const void*>(&JStudioFog_constructVariableValue)));
    adaptor->mObject = object;
    state->mSystem = system;
    state->__vtbl = &sFogVTable;
    return adaptor;
}
/* 02841448 */
void JStudio_JStage::TAdaptor_fog::adaptor_do_end(const jst_adaptors::TObject* object) {
    auto* fog = mObject.get();
    auto* methods = table(fog);
    u32 get = methods->getFlags.get();
    u32 set = methods->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, fog);
    gabi::call_ptr<void>(set, fog, flags & ~1u);
}
/* 028414A0 */
void TAdaptor_fog_destroy(JStudio_JStage::TAdaptor_fog* adaptor, u32 flags) {
    if (!adaptor) return;
    prefix(adaptor)->__vtbl = &sFogVTable;
    adaptor->adaptor_do_end(nullptr);
    JStudioFog_destroyBase(adaptor, 0);
    if (flags & 1) JStudioFog_delete(adaptor);
}
/* 0284150C */
void JStudio_JStage::TAdaptor_fog::adaptor_do_prepare(const jst_adaptors::TObject* object) {
    for (auto* output = sJStudioFogOutputs.values; s32(output->mValueIndex) != -1; ++output) {
        auto* values = reinterpret_cast<jst_timeline::TVariableValue*>(mVariableValues.get());
        values[s32(output->mValueIndex)].pOutput_ = output;
    }
}
/* 02841544 */
void JStudio_JStage::TAdaptor_fog::adaptor_do_begin(const jst_adaptors::TObject* object) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    auto* fog = mObject.get();
    auto* methods = table(fog);
    u32 get = methods->getFlags.get();
    u32 set = methods->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, fog);
    gabi::call_ptr<void>(set, fog, flags | 1u);
    fog = mObject;
    u32 packed = gabi::call_ptr<u32>(table(fog)->getColor.get(), fog);
    gabi::FrameLocal<jst_adaptors::Color> color(8);
    color->word = packed;
    JStudioFog_setColor(this, &sColorIndices, color.get());
    for (auto* output = sJStudioFogOutputs.values; s32(output->mValueIndex) != -1; ++output) {
        s32 index = output->mValueIndex;
        f32 value = (mObject.get()->*output->mGetter)();
        auto* values = reinterpret_cast<jst_timeline::TVariableValue*>(mVariableValues.get());
        values[index].mUpdate.v = gabi::guest_fn(reinterpret_cast<const void*>(&JStudioFog_updateImmediate));
        values[index].mParam.val = value;
        values[index].mAge = 0;
    }
}
/* 028416D0 */
void JStudio_JStage::TAdaptor_fog::adaptor_do_data(const jst_adaptors::TObject* object, const void* id, u32 idSize, const void* data, u32 size) {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioFog_forwardData)), mObject.get(), id, idSize, data, size);
}
/* 028416E8 */
static void initializeFogAdaptor() {
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[1] = 0;
    sStaticBlock.words[0] = 0;
    JStudioFog_registerGlobal(&sStaticRegister);
    initializeOutput(&sJStudioFogOutputs.values[0], 4, 0x10, 0xF);
    initializeOutput(&sJStudioFogOutputs.values[1], 5, 0x12, 0x11);
    // The emitted wrapped-null allocation branch is unreachable for this fixed global slot.
    initializeOutput(&sJStudioFogOutputs.values[2], -1, 0, 0);
    JStudioFog_registerGlobal(&sOutputRegister);
}
/* WWHD glue
data sBaseVTable 101738AC
data sFogVTable 10173904
data sOutputVTable 101738E4
data sColorIndices 10172E78
data sJStudioFogOutputs 104B6384
data sStaticBlock 104B6374
data sStaticRegister 101FAEA0
data sOutputRegister 101FAEAC
ext 0273AD10 void* JStudioFog_allocate(u32)
ext 0273AF40 void JStudioFog_delete(void*)
ext 028EFFD0 void JStudioFog_constructArray(void*, u32, u32, u32)
ext 0283B09C void JStudioFog_destroyBase(jst_adaptors::TAdaptor*, u32)
ext 0284219C void JStudioFog_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32) callback
ext 0283A220 void JStudioFog_setColor(jst_adaptors::TAdaptor*, const jst_adaptors::Index4*, const jst_adaptors::Color*)
ext 028F026C void JStudioFog_registerGlobal(void*)
ext 02841814 jst_timeline::TVariableValue* JStudioFog_constructVariableValue(jst_timeline::TVariableValue*) callback
ext 02839DA0 void JStudioFog_updateImmediate(jst_timeline::TVariableValue*, f64) callback
*/
