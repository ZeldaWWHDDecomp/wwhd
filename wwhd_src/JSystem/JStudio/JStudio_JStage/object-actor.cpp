// Actor adaptor lifecycle and scalar operations, following tww object-actor.cpp and WWHD.
#include "JSystem/JStudio/JStudio_JStage/object-actor.h"
#include "JSystem/JStudio/jstudio-control.h"
#include "JSystem/JStudio/jstudio-object.h"
using JStudio_JStage::ActorVariablePrefix;
using JStudio_JStage::ActorVTable;
using JStudio_JStage::ActorIDMethod;
using JStudio_JStage::PackedMethod;
using JStudio_JStage::ValueOutput;
using JStudio_JStage::AnimationFrameOutput;
using JStudio_JStage::StageSystem;
void JStudioActor_registerGlobal(void*);
u32 JStudioActor_selectOutside(u32);
void* JStudioActor_allocate(u32);
void JStudioActor_delete(void*);
void JStudioActor_constructArray(void*, u32, u32, u32);
void JStudioActor_clear(void*, u32);
void JStudioActor_destroyOutput(jst_timeline::TOutput*, u32);
void JStudioActor_destroyArray(void*, u32, u32, u32, u32, u32);
void destroyActorValueOutput(JStudio_JStage::ValueOutput*, u32);
void destroyActorFrameOutput(JStudio_JStage::AnimationFrameOutput*, u32);
void JStudioActor_destroyBase(jst_adaptors::TAdaptor*, u32);
void JStudioActor_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32);
static jst_timeline::TVariableValue* constructActorVariableValue(jst_timeline::TVariableValue*);
void JStudioActor_setVector(jst_adaptors::TAdaptor*, const jst_adaptors::Index3*, const jst_adaptors::Vec*);
void JStudioActor_multiply(const Mtx34*, const jst_adaptors::Vec*, jst_adaptors::Vec*);
void JStudioActor_updateImmediate(jst_timeline::TVariableValue*, f64);
namespace {
static const jst_adaptors::Index3 sPositionIndices;
static const jst_adaptors::Index3 sTargetIndices;
static const jst_adaptors::Index3 sScalingIndices;
static u32 sBaseVTable;
static u32 sActorVTable;
struct ValueOutputs { ValueOutput values[2]; };
struct FrameOutputs { AnimationFrameOutput values[3]; };
static ValueOutputs sValueOutputs;
static FrameOutputs sFrameOutputs;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sValueRegister;
static u32 sFrameRegister;
static u32 sValueVTable;
static u32 sFrameVTable;
static PackedMethod sShapeMethod;
static PackedMethod sAnimationMethod;
static PackedMethod sTextureMethod;
static ActorVariablePrefix* prefix(JStudio_JStage::TAdaptor_actor* adaptor) {
    return reinterpret_cast<ActorVariablePrefix*>(adaptor->pad04);
}
static jst_timeline::TVariableValue* values(JStudio_JStage::TAdaptor_actor* adaptor) {
    return reinterpret_cast<jst_timeline::TVariableValue*>(adaptor->mVariableValues.get());
}
static ActorVTable* actorTable(jst_adaptors::StageObject* object) {
    return reinterpret_cast<ActorVTable*>(object->mVtable.get());
}
static void applyID(JStudio_JStage::TAdaptor_actor* adaptor, const PackedMethod& prototype, u32 operation, const void* data, u32 size) {
    gabi::Local<ActorIDMethod> method;
    method->d = prototype.adjust;
    method->i = prototype.index;
    method->f = 0;
    adaptor->setJSG_ID_(*method.get(), operation, data, size);
}
}
/* 0283FAB0 */
JStudio_JStage::TAdaptor_actor* TAdaptor_actor_construct(JStudio_JStage::TAdaptor_actor* adaptor, const StageSystem* system, jst_adaptors::StageObject* object) {
    if (!adaptor) adaptor = static_cast<JStudio_JStage::TAdaptor_actor*>(JStudioActor_allocate(sizeof(JStudio_JStage::TAdaptor_actor)));
    if (!adaptor) return adaptor;
    auto* state = prefix(adaptor);
    state->mCount = 14;
    state->__vtbl = &sBaseVTable;
    adaptor->mVariableValues = reinterpret_cast<jst_adaptors::TVariableValues*>(state->mValues);
    JStudioActor_constructArray(state->mValues, 14, 20, gabi::guest_fn(reinterpret_cast<const void*>(&constructActorVariableValue)));
    state->mSystem = system;
    adaptor->mParent = nullptr;
    adaptor->mAnimationMode = 0;
    adaptor->mRelation = nullptr;
    adaptor->mRelationNodeID = 0xFFFFFFFF;
    state->__vtbl = &sActorVTable;
    adaptor->mParentNodeID = 0xFFFFFFFF;
    adaptor->mObject = object;
    adaptor->mTextureAnimationMode = 0;
    return adaptor;
}
/* 0283FB68 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_end(const jst_adaptors::TObject* object) {
    auto* actor = mObject.get();
    auto* table = actorTable(actor);
    u32 get = table->getFlags.get();
    u32 set = table->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, actor);
    gabi::call_ptr<void>(set, actor, flags & ~1u);
}
/* 0283FBC0 */
void TAdaptor_actor_destroy(JStudio_JStage::TAdaptor_actor* adaptor, u32 flags) {
    if (!adaptor) return;
    prefix(adaptor)->__vtbl = &sActorVTable;
    adaptor->adaptor_do_end(nullptr);
    JStudioActor_destroyBase(adaptor, 0);
    if (flags & 1) JStudioActor_delete(adaptor);
}
/* 0283FC2C */
void JStudio_JStage::TAdaptor_actor::adaptor_do_prepare(const jst_adaptors::TObject* object) {
    for (auto* output = sValueOutputs.values; s32(output->mValueIndex) != -1; ++output)
        values(this)[s32(output->mValueIndex)].pOutput_ = output;
    for (auto* output = sFrameOutputs.values; s32(output->mValueIndex) != -1; ++output)
        values(this)[s32(output->mValueIndex)].pOutput_ = output;
}
/* 0283FC98 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_begin(const jst_adaptors::TObject* object) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    auto* actor = mObject.get();
    auto* methods = actorTable(actor);
    u32 get = methods->getFlags.get(), set = methods->setFlags.get();
    u32 flags = gabi::call_ptr<u32>(get, actor);
    gabi::call_ptr<void>(set, actor, flags | 1u);
    struct BeginFrame { jst_adaptors::Vec transformed[2], source[2], scaling; };
    gabi::FrameLocal<BeginFrame> frame(8);
    auto* control = reinterpret_cast<const JStudio::TControl*>(object->mControl.get());
    actor = mObject.get();
    gabi::call_ptr<void>(actorTable(actor)->getTranslation.get(), actor, &frame->source[0]);
    actor = mObject.get();
    gabi::call_ptr<void>(actorTable(actor)->getRotation.get(), actor, &frame->source[1]);
    actor = mObject.get();
    gabi::call_ptr<void>(actorTable(actor)->getScaling.get(), actor, &frame->scaling);
    auto* source = frame->source;
    if (u8(control->mTransformOnGet)) {
        JStudioActor_multiply(&control->mTransformOnGet_Matrix, &frame->source[0], &frame->transformed[0]);
        frame->transformed[1].words[0] = frame->source[1].words[0];
        frame->transformed[1].y = f32(frame->source[1].y) + f32(control->mTransformOnGet_RotationY);
        frame->transformed[1].words[2] = frame->source[1].words[2];
        source = frame->transformed;
    }
    JStudioActor_setVector(this, &sPositionIndices, &source[0]);
    JStudioActor_setVector(this, &sTargetIndices, &source[1]);
    JStudioActor_setVector(this, &sScalingIndices, &frame->scaling);
    auto initializeValue = [this](s32 index, f32 value) {
        auto& variable = values(this)[index];
        variable.field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(&JStudioActor_updateImmediate));
        variable.field_0xc.val = value;
        variable.field_0x4 = 0;
    };
    for (auto* output = sValueOutputs.values; s32(output->mValueIndex) != -1; ++output) {
        s32 index = output->mValueIndex;
        f32 value = (mObject.get()->*output->mGetter)();
        initializeValue(index, value);
    }
    for (auto* output = sFrameOutputs.values; s32(output->mValueIndex) != -1; ++output) {
        s32 index = output->mValueIndex;
        f32 value = (mObject.get()->*output->mGetter)();
        initializeValue(index, value);
    }
}
/* 028400A4 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_data(const jst_adaptors::TObject* object, const void* id, u32 idSize, const void* data, u32 size) {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioActor_forwardData)), mObject.get(), id, idSize, data, size);
}
/* 028400BC */
void JStudio_JStage::TAdaptor_actor::setJSG_ID_(const ActorIDMethod& method, u32 operation, const void* data, u32 size) {
    if (operation == 0x19) (mObject.get()->*method)(u32(*static_cast<const be<u32>*>(data)));
}
/* 0284010C */
void JStudio_JStage::TAdaptor_actor::adaptor_do_SHAPE(u32 operation, const void* data, u32 size) {
    applyID(this, sShapeMethod, operation, data, size);
}
/* 02840154 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_ANIMATION(u32 operation, const void* data, u32 size) {
    applyID(this, sAnimationMethod, operation, data, size);
}
/* 028401B0 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_TEXTURE_ANIMATION(u32 operation, const void* data, u32 size) {
    applyID(this, sTextureMethod, operation, data, size);
}
/* 0284019C */
void JStudio_JStage::TAdaptor_actor::adaptor_do_ANIMATION_MODE(u32 operation, const void* data, u32 size) {
    if (operation == 2) mAnimationMode = *static_cast<const be<u32>*>(data);
}
/* 028401F8 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_TEXTURE_ANIMATION_MODE(u32 operation, const void* data, u32 size) {
    if (operation == 2) mTextureAnimationMode = *static_cast<const be<u32>*>(data);
}
/* 0284020C */
void JStudio_JStage::TAdaptor_actor::adaptor_do_PARENT(u32 operation, const void* data, u32 size) {
    if (operation != 0x18) return;
    const StageSystem* system = prefix(this)->mSystem;
    mParent = gabi::call_ptr<jst_adaptors::StageObject*>(system->__vtbl->findObject.get(), system, data, 0u);
}
/* 02840310 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_RELATION(u32 operation, const void* data, u32 size) {
    if (operation != 0x18) return;
    const StageSystem* system = prefix(this)->mSystem;
    mRelation = gabi::call_ptr<jst_adaptors::StageObject*>(system->__vtbl->findObject.get(), system, data, 0u);
}
/* 0284025C */
void JStudio_JStage::TAdaptor_actor::adaptor_do_PARENT_NODE(u32 operation, const void* data, u32 size) {
    if (operation == 0x18) {
        auto* object = mParent.get();
        if (object) mParentNodeID = gabi::call_ptr<u32>(actorTable(object)->findNode.get(), object, data);
    } else if (operation == 0x19) {
        mParentNodeID = *static_cast<const be<u32>*>(data);
    }
}
/* 02840360 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_RELATION_NODE(u32 operation, const void* data, u32 size) {
    if (operation == 0x18) {
        auto* object = mRelation.get();
        if (object) mRelationNodeID = gabi::call_ptr<u32>(actorTable(object)->findNode.get(), object, data);
    } else if (operation == 0x19) {
        mRelationNodeID = *static_cast<const be<u32>*>(data);
    }
}
/* 028402D8 */
void JStudio_JStage::TAdaptor_actor::adaptor_do_PARENT_ENABLE(u32 operation, const void* data, u32 size) {
    if (operation != 2) return;
    jst_adaptors::StageObject* object = nullptr;
    u32 nodeID = 0xFFFFFFFF;
    if (u32(*static_cast<const be<u32>*>(data))) {
        nodeID = mParentNodeID;
        object = mParent;
    }
    auto* actor = mObject.get();
    gabi::tail_ptr<void>(actorTable(actor)->setParent.get(), actor, object, nodeID);
}
/* 028403DC */
void JStudio_JStage::TAdaptor_actor::adaptor_do_RELATION_ENABLE(u32 operation, const void* data, u32 size) {
    if (operation != 2) return;
    bool enabled = u32(*static_cast<const be<u32>*>(data)) != 0;
    auto* actor = mObject.get();
    gabi::tail_ptr<void>(actorTable(actor)->setRelation.get(), actor, u32(enabled), mRelation.get(), u32(mRelationNodeID));
}
/* 02840410 */
void JStudio_JStage::AnimationFrameOutput::output(f32 value, jst_adaptors::TAdaptor* adaptor) const {
    // The static table encodes the byte after the mode word, matching the original -1 load.
    u32 mode = *reinterpret_cast<const be<u32>*>(reinterpret_cast<const u8*>(adaptor) + u32(mModeOffset) - 1);
    auto* actor = static_cast<JStudio_JStage::TAdaptor_actor*>(adaptor)->mObject.get();
    f32 maximum = (actor->*mMaxGetter)();
    if (((mode >> 8) & 0xFF) == 1) value = maximum - value;
    if (maximum > 0.0f) {
        u32 outside = JStudioActor_selectOutside(mode & 0xFF);
        value = f32(gabi::call_ptr<f64>(outside, f64(value), f64(maximum)));
    }
    (actor->*mSetter)(value);
}
/* 02840568 */
static void initializeActorAdaptor() {
    for (auto& word : sStaticBlock.words) word = 0;
    JStudioActor_registerGlobal(&sStaticRegister);
    auto initializeValue = [](ValueOutput& output, s32 index, s16 setter, s16 getter) {
        output.__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sValueVTable);
        output.mValueIndex = index;
        output.mSetter.d = 0; output.mSetter.i = setter; output.mSetter.f = 0;
        output.mGetter.d = 0; output.mGetter.i = getter; output.mGetter.f = 0;
    };
    initializeValue(sValueOutputs.values[0], 1, 0x1B, 0x1A);
    // Both sentinel addresses are fixed globals; the original wrapped-null allocation paths cannot occur.
    initializeValue(sValueOutputs.values[1], -1, 0, 0);
    JStudioActor_registerGlobal(&sValueRegister);
    auto initializeFrame = [](AnimationFrameOutput& output, s32 index, u32 mode, s16 setter, s16 getter, s16 maximum) {
        output.__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sFrameVTable);
        output.mValueIndex = index; output.mModeOffset = mode;
        output.mSetter.d = 0; output.mSetter.i = setter; output.mSetter.f = 0;
        output.mGetter.d = 0; output.mGetter.i = getter; output.mGetter.f = 0;
        output.mMaxGetter.d = 0; output.mMaxGetter.i = maximum; output.mMaxGetter.f = 0;
    };
    initializeFrame(sFrameOutputs.values[0], 0, 0x13D, 0x18, 0x17, 0x19);
    initializeFrame(sFrameOutputs.values[1], 2, 0x141, 0x1F, 0x1E, 0x20);
    initializeFrame(sFrameOutputs.values[2], -1, 0, 0, 0, 0);
    JStudioActor_registerGlobal(&sFrameRegister);
}
/* 02840778 */
static jst_timeline::TVariableValue* constructActorVariableValue(jst_timeline::TVariableValue* value) {
    if (!value) value = static_cast<jst_timeline::TVariableValue*>(JStudioActor_allocate(sizeof(jst_timeline::TVariableValue)));
    if (value) {
        value->field_0x4 = 0;
        value->field_0x8.v = 0;
        JStudioActor_clear(&value->field_0xc, 4);
        value->pOutput_ = &sJStudioOutputNone;
    }
    return value;
}
/* 02840730 */
static void destroyActorValueOutputs() {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioActor_destroyArray)), sValueOutputs.values, 2u, 0x18u, gabi::guest_fn(reinterpret_cast<const void*>(&destroyActorValueOutput)), 0u, 0u);
}
/* 02840754 */
static void destroyActorFrameOutputs() {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioActor_destroyArray)), sFrameOutputs.values, 3u, 0x24u, gabi::guest_fn(reinterpret_cast<const void*>(&destroyActorFrameOutput)), 0u, 0u);
}
/* 028407DC */
void destroyActorFrameOutput(JStudio_JStage::AnimationFrameOutput* output, u32 flags) {
    if (!output) return;
    JStudioActor_destroyOutput(output, 0);
    if (flags & 1) JStudioActor_delete(output);
}
/* 02840830 */
void JStudio_JStage::ValueOutput::output(f32 value, jst_adaptors::TAdaptor* adaptor) const {
    auto* actor = static_cast<JStudio_JStage::TAdaptor_actor*>(adaptor)->mObject.get();
    (actor->*mSetter)(value);
}
/* 02840874 */
void destroyActorValueOutput(JStudio_JStage::ValueOutput* output, u32 flags) {
    if (!output) return;
    JStudioActor_destroyOutput(output, 0);
    if (flags & 1) JStudioActor_delete(output);
}
/* WWHD glue
data sPositionIndices 10172EB8
data sTargetIndices 10172EC4
data sScalingIndices 10172ED0
ext 0283A160 void JStudioActor_setVector(jst_adaptors::TAdaptor*, const jst_adaptors::Index3*, const jst_adaptors::Vec*)
ext 028E8F64 void JStudioActor_multiply(const Mtx34*, const jst_adaptors::Vec*, jst_adaptors::Vec*)
ext 02839DA0 void JStudioActor_updateImmediate(jst_timeline::TVariableValue*, f64) callback
data sStaticBlock 104B6230
data sStaticRegister 101FAE58
data sValueRegister 101FAE64
data sFrameRegister 101FAE70
data sValueVTable 10173604
data sFrameVTable 10173744
data sBaseVTable 1017361C
data sActorVTable 101736B4
data sValueOutputs 104B6240
data sFrameOutputs 104B6270
data sShapeMethod 101735EC
data sAnimationMethod 101735F4
data sTextureMethod 101735FC
ext 028F026C void JStudioActor_registerGlobal(void*) callback
ext 028488A0 u32 JStudioActor_selectOutside(u32)
ext 0273AD10 void* JStudioActor_allocate(u32)
ext 0273AF40 void JStudioActor_delete(void*)
ext 028EFFD0 void JStudioActor_constructArray(void*, u32, u32, u32)
ext 028F521C void JStudioActor_clear(void*, u32)
ext 02839E5C void JStudioActor_destroyOutput(jst_timeline::TOutput*, u32)
ext 028F0164 void JStudioActor_destroyArray(void*, u32, u32, u32, u32, u32) callback
ext 0283A74C void JStudioActor_destroyBase(jst_adaptors::TAdaptor*, u32)
ext 0284219C void JStudioActor_forwardData(jst_adaptors::StageObject*, const void*, u32, const void*, u32) callback
*/
