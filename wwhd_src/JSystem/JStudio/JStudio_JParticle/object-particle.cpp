// Particle adaptor setup, parent operations and weak callbacks, reconstructed against WWHD.
#include "JSystem/JStudio/JStudio_JParticle/object-particle.h"
#include "JSystem/JStudio/jstudio-object.h"
using JStudio_JParticle::ParticleVariablePrefix;
using JStudio_JParticle::ParticleParentPrefix;
using JStudio_JParticle::ParentNodeVTable;
using JStudio_JParticle::ParentSystem;
void* JStudioParticleObject_allocate(u32);
void JStudioParticleObject_delete(void*);
void JStudioParticleObject_constructArray(void*, u32, u32, u32);
void JStudioParticleObject_clear(void*, u32);
void JStudioParticleObject_destroyBase(jst_adaptors::TAdaptor*, u32);
void JStudioParticleObject_destroyOutput(jst_timeline::TOutput*, u32);
void JStudioParticleObject_forceDelete(jst_adaptors::JPAEmitterManager*, jst_adaptors::Emitter*, u32);
void JStudioParticleObject_setImmediate(jst_adaptors::TAdaptor*, const void*);
void JStudioParticleObject_registerGlobal(void*);
static jst_timeline::TVariableValue* constructParticleVariableValue(jst_timeline::TVariableValue*);
namespace {
static u32 sBaseVTable;
static u32 sParticleVTable;
static u32 sCallbackVTable;
static u32 sBeginVTable;
static u32 sEndVTable;
struct Immediate { be<u32> index; be<f32> value; };
struct ImmediateTable { Immediate values[18]; };
static ImmediateTable sDefaults;
static u32 sDefaultsInitialized;
static f32 sZeroValue;
static f32 sOneValue;
static f32 sMaxValue;
static JStudio_JParticle::TAdaptor_particle::TVVOOn_BEGIN_FADE_IN_ sBeginOutput;
static JStudio_JParticle::TAdaptor_particle::TVVOOn_END_FADE_OUT_ sEndOutput;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sBeginRegister;
static u32 sEndRegister;
static ParticleVariablePrefix* prefix(JStudio_JParticle::TAdaptor_particle* adaptor) {
    return reinterpret_cast<ParticleVariablePrefix*>(adaptor->pad04);
}
static ParticleParentPrefix* parent(JStudio_JParticle::TAdaptor_particle* adaptor) {
    return reinterpret_cast<ParticleParentPrefix*>(&adaptor->mSystem);
}
static jst_timeline::TVariableValue* values(JStudio_JParticle::TAdaptor_particle* adaptor) {
    return reinterpret_cast<jst_timeline::TVariableValue*>(adaptor->mVariableValues.get());
}
}
/* 0283E784 */
JStudio_JParticle::TAdaptor_particle* TAdaptor_particle_construct(JStudio_JParticle::TAdaptor_particle* adaptor, jst_adaptors::JPAEmitterManager* manager, const ParentSystem* system) {
    if (!adaptor) adaptor = static_cast<JStudio_JParticle::TAdaptor_particle*>(JStudioParticleObject_allocate(sizeof(JStudio_JParticle::TAdaptor_particle)));
    if (!adaptor) return adaptor;
    auto* state = prefix(adaptor);
    state->mCount = 20;
    state->__vtbl = &sBaseVTable;
    adaptor->mVariableValues = reinterpret_cast<jst_adaptors::TVariableValues*>(state->mValues);
    JStudioParticleObject_constructArray(state->mValues, 20, 20, gabi::guest_fn(reinterpret_cast<const void*>(&constructParticleVariableValue)));
    parent(adaptor)->mSystem = system;
    parent(adaptor)->mObject = nullptr;
    adaptor->mFadeDuration = 0;
    adaptor->mFadeElapsed = 0;
    adaptor->mEmitterManager = manager;
    state->__vtbl = &sParticleVTable;
    adaptor->mCallback.mVtable = gabi::ea(&sCallbackVTable);
    adaptor->mCallback.mObject = nullptr;
    parent(adaptor)->mNodeID = 0xFFFFFFFF;
    parent(adaptor)->mEnabled = 0;
    adaptor->mParticleID = 0xFFFFFFFF;
    adaptor->mFadeState = 0;
    adaptor->mCallback.mAdaptor = adaptor;
    adaptor->mEmitter = nullptr;
    return adaptor;
}
/* 0283E858 */
void TAdaptor_particle_destroy(JStudio_JParticle::TAdaptor_particle* adaptor, u32 flags) {
    if (!adaptor) return;
    auto* emitter = adaptor->mEmitter.get();
    prefix(adaptor)->__vtbl = &sParticleVTable;
    if (emitter) JStudioParticleObject_forceDelete(adaptor->mEmitterManager, emitter, 0);
    JStudioParticleObject_destroyBase(adaptor, 0);
    if (flags & 1) JStudioParticleObject_delete(adaptor);
}
/* 0283E8D0 */
void JStudio_JParticle::TAdaptor_particle::adaptor_do_prepare(const jst_adaptors::TObject* object) {
    if (!sDefaultsInitialized) {
        sDefaultsInitialized = 1;
        for (u32 i = 0; i < 17; ++i) {
            sDefaults.values[i].index = i;
            sDefaults.values[i].value = i < 6 ? f32(sZeroValue) : (i < 9 ? f32(sOneValue) : f32(sMaxValue));
        }
        sDefaults.values[17].index = 0xFFFFFFFF;
        sDefaults.values[17].value = sJStudioQuietNaN;
    }
    JStudioParticleObject_setImmediate(this, &sDefaults);
    values(this)[18].pOutput_ = &sBeginOutput;
    values(this)[19].pOutput_ = &sEndOutput;
    mCallback.mObject = const_cast<jst_adaptors::TObject*>(object);
}
/* 0283EA44 */
void JStudio_JParticle::TAdaptor_particle::adaptor_do_end(const jst_adaptors::TObject* object) {}
/* 0283EA94 */
void JStudio_JParticle::TAdaptor_particle::adaptor_do_PARTICLE(u32 operation, const void* data, u32 size) {
    if (operation == 0x19) mParticleID = *static_cast<const be<u32>*>(data);
}
/* 0283EAA8 */
void JStudio_JParticle::TAdaptor_particle::adaptor_do_PARENT(u32 operation, const void* data, u32 size) {
    if (operation != 0x18) return;
    const ParentSystem* system = parent(this)->mSystem;
    if (system) parent(this)->mObject = gabi::call_ptr<jst_adaptors::StageObject*>(system->__vtbl->findObject.get(), system, data, 0u);
}
/* 0283EB00 */
void JStudio_JParticle::TAdaptor_particle::adaptor_do_PARENT_NODE(u32 operation, const void* data, u32 size) {
    if (operation == 0x18) {
        auto* object = parent(this)->mObject.get();
        if (object) {
            auto* table = reinterpret_cast<ParentNodeVTable*>(object->mVtable.get());
            parent(this)->mNodeID = gabi::call_ptr<u32>(table->getNodeID.get(), object, data);
        }
    } else if (operation == 0x19) {
        parent(this)->mNodeID = *static_cast<const be<u32>*>(data);
    }
}
/* 0283EB7C */
void JStudio_JParticle::TAdaptor_particle::adaptor_do_PARENT_ENABLE(u32 operation, const void* data, u32 size) {
    if (operation == 2) parent(this)->mEnabled = u32(*static_cast<const be<u32>*>(data)) != 0;
}
/* 0283F2FC */
static void initializeParticleAdaptor() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JStudioParticleObject_registerGlobal(&sStaticRegister);
    sBeginOutput.__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sBeginVTable);
    JStudioParticleObject_registerGlobal(&sBeginRegister);
    sEndOutput.__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sEndVTable);
    JStudioParticleObject_registerGlobal(&sEndRegister);
}
/* 0283F378 */
void JStudio_JParticle::TAdaptor_particle::TJPACallback_::executeAfter(jst_adaptors::Emitter* emitter) {}
/* 0283F37C */
void JStudio_JParticle::TAdaptor_particle::TJPACallback_::draw(jst_adaptors::Emitter* emitter) {}
// HD: the callback table has an additional post-draw slot beyond the GC reference.
/* 0283F380 */
void JStudio_JParticle::TAdaptor_particle::TJPACallback_::drawAfter(jst_adaptors::Emitter* emitter) {}
/* 0283F384 */
static jst_timeline::TVariableValue* constructParticleVariableValue(jst_timeline::TVariableValue* value) {
    if (!value) value = static_cast<jst_timeline::TVariableValue*>(JStudioParticleObject_allocate(sizeof(jst_timeline::TVariableValue)));
    if (value) {
        value->mAge = 0;
        value->mUpdate.v = 0;
        JStudioParticleObject_clear(&value->mParam, 4);
        value->pOutput_ = &sJStudioOutputNone;
    }
    return value;
}
/* 0283F3E8 */
void TVVOOn_BEGIN_FADE_IN_particle_destroy(JStudio_JParticle::TAdaptor_particle::TVVOOn_BEGIN_FADE_IN_* output, u32 flags) {
    if (!output) return;
    JStudioParticleObject_destroyOutput(output, 0);
    if (flags & 1) JStudioParticleObject_delete(output);
}
/* 0283F43C */
void TVVOOn_END_FADE_OUT_particle_destroy(JStudio_JParticle::TAdaptor_particle::TVVOOn_END_FADE_OUT_* output, u32 flags) {
    if (!output) return;
    JStudioParticleObject_destroyOutput(output, 0);
    if (flags & 1) JStudioParticleObject_delete(output);
}
/* WWHD glue
data sBaseVTable 10173458
data sParticleVTable 101734FC
data sCallbackVTable 10173584
data sBeginVTable 10173554
data sEndVTable 1017356C
data sDefaults 104B6184
data sDefaultsInitialized 104B6214
data sZeroValue 101734B0
data sOneValue 10173448
data sMaxValue 101734B4
data sBeginOutput 104B6218
data sEndOutput 104B621C
data sStaticBlock 104B6174
data sStaticRegister 101FAE28
data sBeginRegister 101FAE34
data sEndRegister 101FAE40
ext 0273AD10 void* JStudioParticleObject_allocate(u32)
ext 0273AF40 void JStudioParticleObject_delete(void*)
ext 028EFFD0 void JStudioParticleObject_constructArray(void*, u32, u32, u32)
ext 028F521C void JStudioParticleObject_clear(void*, u32)
ext 0283B6F4 void JStudioParticleObject_destroyBase(jst_adaptors::TAdaptor*, u32)
ext 02839E5C void JStudioParticleObject_destroyOutput(jst_timeline::TOutput*, u32)
ext 0282215C void JStudioParticleObject_forceDelete(jst_adaptors::JPAEmitterManager*, jst_adaptors::Emitter*, u32)
ext 0283A118 void JStudioParticleObject_setImmediate(jst_adaptors::TAdaptor*, const void*)
ext 028F026C void JStudioParticleObject_registerGlobal(void*)
*/
