// Camera adaptor lifecycle, parent linkage and outputs, following tww object-camera.cpp.
#include "JSystem/JStudio/JStudio_JStage/object-camera.h"
#include "JSystem/JStudio/jstudio-control.h"
#include "JSystem/JStudio/jstudio-object.h"
using JStudio_JStage::CameraVariablePrefix;
using JStudio_JStage::CameraValueOutput;
using JStudio_JStage::StageSystem;
using JStudio_JStage::CameraVTable;
void* JStudioCamera_allocate(u32);
void JStudioCamera_delete(void*);
void JStudioCamera_constructArray(void*, u32, u32, u32);
void JStudioCamera_destroyArray(void*, u32, u32, u32, u32, u32);
void JStudioCamera_clear(void*, u32);
void JStudioCamera_destroyBase(jst_adaptors::TAdaptor*, u32);
void JStudioCamera_destroyOutput(jst_timeline::TOutput*, u32);
void JStudioCamera_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32);
void JStudioCamera_registerGlobal(void*);
static jst_timeline::TVariableValue* constructCameraVariableValue(jst_timeline::TVariableValue*);
void destroyCameraValueOutput(CameraValueOutput*, u32);
void JStudioCamera_setVector(jst_adaptors::TAdaptor*, const jst_adaptors::Index3*, const jst_adaptors::Vec*);
void JStudioCamera_multiply(const Mtx34*, const jst_adaptors::Vec*, jst_adaptors::Vec*);
void JStudioCamera_updateImmediate(jst_timeline::TVariableValue*, f64);
namespace {
static const jst_adaptors::Index3 sPositionIndices;
static const jst_adaptors::Index3 sTargetIndices;
static u32 sBaseVTable;
static u32 sCameraVTable;
static u32 sOutputVTable;
struct Outputs { CameraValueOutput values[5]; };
static Outputs sOutputs;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sOutputRegister;
static CameraVariablePrefix* prefix(JStudio_JStage::TAdaptor_camera* adaptor) { return reinterpret_cast<CameraVariablePrefix*>(adaptor->pad04); }
static CameraVTable* table(jst_adaptors::StageObject* object) { return reinterpret_cast<CameraVTable*>(object->mVtable.get()); }
static void initializeOutput(CameraValueOutput* output, s32 index, s16 setter, s16 getter) {
    output->__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sOutputVTable);
    output->mValueIndex = index;
    output->mSetter.d = 0; output->mSetter.i = setter; output->mSetter.f = 0;
    output->mGetter.d = 0; output->mGetter.i = getter; output->mGetter.f = 0;
}
}
/* 02840BC0 */
JStudio_JStage::TAdaptor_camera* TAdaptor_camera_construct(JStudio_JStage::TAdaptor_camera* adaptor, const StageSystem* system, jst_adaptors::StageObject* object) {
    if (!adaptor) adaptor = static_cast<JStudio_JStage::TAdaptor_camera*>(JStudioCamera_allocate(sizeof(JStudio_JStage::TAdaptor_camera)));
    if (!adaptor) return adaptor;
    auto* state = prefix(adaptor);
    state->mCount = 11;
    state->__vtbl = &sBaseVTable;
    adaptor->mVariableValues = reinterpret_cast<jst_adaptors::TVariableValues*>(state->mValues);
    JStudioCamera_constructArray(state->mValues, 11, 20, gabi::guest_fn(reinterpret_cast<const void*>(&constructCameraVariableValue)));
    adaptor->mObject = object;
    state->mSystem = system;
    state->__vtbl = &sCameraVTable;
    return adaptor;
}
/* 02840C58 */
void JStudio_JStage::TAdaptor_camera::adaptor_do_end(const jst_adaptors::TObject* object) {
    auto* camera = mObject.get();
    auto* methods = table(camera);
    u32 get = methods->getFlags.get();
    u32 set = methods->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, camera);
    gabi::call_ptr<void>(set, camera, flags & ~1u);
}
/* 02840CB0 */
void TAdaptor_camera_destroy(JStudio_JStage::TAdaptor_camera* adaptor, u32 flags) {
    if (!adaptor) return;
    prefix(adaptor)->__vtbl = &sCameraVTable;
    adaptor->adaptor_do_end(nullptr);
    JStudioCamera_destroyBase(adaptor, 0);
    if (flags & 1) JStudioCamera_delete(adaptor);
}
/* 02840D1C */
void JStudio_JStage::TAdaptor_camera::adaptor_do_prepare(const jst_adaptors::TObject* object) {
    for (auto* output = sOutputs.values; s32(output->mValueIndex) != -1; ++output) {
        auto* values = reinterpret_cast<jst_timeline::TVariableValue*>(mVariableValues.get());
        values[s32(output->mValueIndex)].pOutput_ = output;
    }
}
/* 02840D54 */
void JStudio_JStage::TAdaptor_camera::adaptor_do_begin(const jst_adaptors::TObject* object) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    auto* camera = mObject.get();
    auto* methods = table(camera);
    u32 get = methods->getFlags.get(), set = methods->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, camera);
    gabi::call_ptr<void>(set, camera, flags | 1u);
    struct BeginFrame { jst_adaptors::Vec source[2], transformed[2]; };
    gabi::FrameLocal<BeginFrame> frame(8);
    auto* control = reinterpret_cast<const JStudio::TControl*>(object->mControl.get());
    camera = mObject.get();
    gabi::call_ptr<void>(table(camera)->getViewPosition.get(), camera, &frame->source[0]);
    camera = mObject.get();
    gabi::call_ptr<void>(table(camera)->getViewTarget.get(), camera, &frame->source[1]);
    auto* source = frame->source;
    if (u8(control->mTransformOnGet)) {
        JStudioCamera_multiply(&control->mTransformOnGet_Matrix, &frame->source[0], &frame->transformed[0]);
        JStudioCamera_multiply(&control->mTransformOnGet_Matrix, &frame->source[1], &frame->transformed[1]);
        source = frame->transformed;
    }
    JStudioCamera_setVector(this, &sPositionIndices, &source[0]);
    JStudioCamera_setVector(this, &sTargetIndices, &source[1]);
    for (auto* output = sOutputs.values; s32(output->mValueIndex) != -1; ++output) {
        s32 index = output->mValueIndex;
        f32 value = (mObject.get()->*output->mGetter)();
        auto& variable = reinterpret_cast<jst_timeline::TVariableValue*>(mVariableValues.get())[index];
        variable.mAge = 0;
        variable.mUpdate.v = gabi::guest_fn(reinterpret_cast<const void*>(&JStudioCamera_updateImmediate));
        variable.mParam.val = value;
    }
}
/* 02841014 */
void JStudio_JStage::TAdaptor_camera::adaptor_do_data(const jst_adaptors::TObject* object, const void* id, u32 idSize, const void* data, u32 size) {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioCamera_forwardData)), mObject.get(), id, idSize, data, size);
}
/* 0284102C */
void JStudio_JStage::TAdaptor_camera::adaptor_do_PARENT(u32 operation, const void* data, u32 size) {
    if (operation != 0x18) return;
    const StageSystem* system = prefix(this)->mSystem;
    mParent = gabi::call_ptr<jst_adaptors::StageObject*>(system->__vtbl->findObject.get(), system, data, 0u);
}
/* 0284107C */
void JStudio_JStage::TAdaptor_camera::adaptor_do_PARENT_NODE(u32 operation, const void* data, u32 size) {
    if (operation == 0x18) {
        auto* object = mParent.get();
        if (object) mParentNodeID = gabi::call_ptr<u32>(table(object)->findNode.get(), object, data);
    } else if (operation == 0x19) {
        mParentNodeID = *static_cast<const be<u32>*>(data);
    }
}
/* 028410F8 */
void JStudio_JStage::TAdaptor_camera::adaptor_do_PARENT_ENABLE(u32 operation, const void* data, u32 size) {
    if (operation != 2) return;
    jst_adaptors::StageObject* object = nullptr;
    u32 nodeID = 0xFFFFFFFF;
    if (u32(*static_cast<const be<u32>*>(data))) { nodeID = mParentNodeID; object = mParent; }
    auto* camera = mObject.get();
    gabi::tail_ptr<void>(table(camera)->setParent.get(), camera, object, nodeID);
}
/* 02841130 */
static void initializeCameraAdaptor() {
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[1] = 0;
    sStaticBlock.words[0] = 0;
    JStudioCamera_registerGlobal(&sStaticRegister);
    initializeOutput(&sOutputs.values[0], 7, 0x22, 0x21);
    initializeOutput(&sOutputs.values[1], 6, 0x14, 0x13);
    initializeOutput(&sOutputs.values[2], 8, 0x10, 0xF);
    initializeOutput(&sOutputs.values[3], 9, 0x12, 0x11);
    // The emitted wrapped-null allocation branch is unreachable for this fixed global slot.
    initializeOutput(&sOutputs.values[4], -1, 0, 0);
    JStudioCamera_registerGlobal(&sOutputRegister);
}
/* 02841290 */
static void destroyCameraOutputs() {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioCamera_destroyArray)), sOutputs.values, 5u, 0x18u, gabi::guest_fn(reinterpret_cast<const void*>(&destroyCameraValueOutput)), 0u, 0u);
}
/* 028412B4 */
static jst_timeline::TVariableValue* constructCameraVariableValue(jst_timeline::TVariableValue* value) {
    if (!value) value = static_cast<jst_timeline::TVariableValue*>(JStudioCamera_allocate(sizeof(jst_timeline::TVariableValue)));
    if (value) { value->mAge = 0; value->mUpdate.v = 0; JStudioCamera_clear(&value->mParam, 4); value->pOutput_ = &sJStudioOutputNone; }
    return value;
}
/* 02841318 */
void JStudio_JStage::CameraValueOutput::output(f32 value, jst_adaptors::TAdaptor* adaptor) const {
    auto* camera = static_cast<JStudio_JStage::TAdaptor_camera*>(adaptor)->mObject.get();
    (camera->*mSetter)(value);
}
/* 0284135C */
void destroyCameraValueOutput(CameraValueOutput* output, u32 flags) {
    if (!output) return;
    JStudioCamera_destroyOutput(output, 0);
    if (flags & 1) JStudioCamera_delete(output);
}
/* WWHD glue
data sPositionIndices 10172EE8
data sTargetIndices 10172EF4
ext 0283A160 void JStudioCamera_setVector(jst_adaptors::TAdaptor*, const jst_adaptors::Index3*, const jst_adaptors::Vec*)
ext 028E8F64 void JStudioCamera_multiply(const Mtx34*, const jst_adaptors::Vec*, jst_adaptors::Vec*)
ext 02839DA0 void JStudioCamera_updateImmediate(jst_timeline::TVariableValue*, f64) callback
data sBaseVTable 101737FC
data sCameraVTable 10173854
data sOutputVTable 101737E4
data sOutputs 104B62FC
data sStaticBlock 104B62EC
data sStaticRegister 101FAE88
data sOutputRegister 101FAE94
ext 0273AD10 void* JStudioCamera_allocate(u32)
ext 0273AF40 void JStudioCamera_delete(void*)
ext 028EFFD0 void JStudioCamera_constructArray(void*, u32, u32, u32)
ext 028F0164 void JStudioCamera_destroyArray(void*, u32, u32, u32, u32, u32) callback
ext 028F521C void JStudioCamera_clear(void*, u32)
ext 0283AD68 void JStudioCamera_destroyBase(jst_adaptors::TAdaptor*, u32)
ext 02839E5C void JStudioCamera_destroyOutput(jst_timeline::TOutput*, u32)
ext 0284219C void JStudioCamera_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32) callback
ext 028F026C void JStudioCamera_registerGlobal(void*)
*/
